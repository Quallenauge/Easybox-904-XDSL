/*
 * netfilter module for userspace bridged Ethernet frames logging daemons
 *
 *	Authors:
 *	Bart De Schuymer <bdschuym@pandora.be>
 *	Harald Welte <laforge@netfilter.org>
 *
 *  November, 2004
 *
 * Based on ipt_ULOG.c, which is
 * (C) 2000-2002 by Harald Welte <laforge@netfilter.org>
 *
 * This module accepts two parameters:
 *
 * nlbufsiz:
 *   The parameter specifies how big the buffer for each netlink multicast
 * group is. e.g. If you say nlbufsiz=8192, up to eight kb of packets will
 * get accumulated in the kernel until they are sent to userspace. It is
 * NOT possible to allocate more than 128kB, and it is strongly discouraged,
 * because atomically allocating 128kB inside the network rx softirq is not
 * reliable. Please also keep in mind that this buffer size is allocated for
 * each nlgroup you are using, so the total kernel memory usage increases
 * by that factor.
 *
 * flushtimeout:
 *   Specify, after how many hundredths of a second the queue should be
 *   flushed even if it is not full yet.
 *
 */

#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/socket.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/netlink.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_ulog.h>
#include <net/sock.h>
/*#include "../br_private.h"*/

#define	NETLINK_PKTCAP		30

#define PRINTR(format, args...) do { if (net_ratelimit()) \
                                printk(format , ## args); } while (0)

static unsigned int nlbufsiz = NLMSG_GOODSIZE;
static unsigned int flushtimeout = 10;

typedef struct {
	unsigned int qlen;		/* number of nlmsgs' in the skb */
	struct nlmsghdr *lastnlh;	/* netlink header of last msg in skb */
	struct sk_buff *skb;		/* the pre-allocated skb */
	struct timer_list timer;	/* the timer function */
	spinlock_t lock;		/* the per-queue lock */
} ebt_ulog_buff_t;

static ebt_ulog_buff_t ulog_buffers[EBT_ULOG_MAXNLGROUPS];
static struct sock *ebtulognl;

#define	EBT_ULOG_MIN_PKT_LEN	20

#define EBT_ULOG_PKT_MAX_INFO_COUNT 1

// Interface Type ID
#define	EBT_ULOG_PKT_WAN_TYPE	0
#define	EBT_ULOG_PKT_LAN_TYPE	1
#define	EBT_ULOG_PKT_WLAN_TYPE	2

// Interface Type String
#define	EBT_ULOG_PKT_WAN_TYPE_STRING	"wan"
#define	EBT_ULOG_PKT_LAN_TYPE_STRING	"lan"
#define	EBT_ULOG_PKT_WLAN_TYPE_STRING	"wlan"

// Capture type
#define	EBT_ULOG_PKT_BY_INTERFACE 0
#define	EBT_ULOG_PKT_BY_INTERFACE_TYPE 1

typedef struct {
	short			vpi;
	unsigned short	vci;
} ebt_ulog_pvc;

typedef struct {
	unsigned long	rxCnt;
	unsigned long	rxCpy;
	unsigned long	txCnt;
	unsigned long	txCpy;
} ebt_ulog_cnt;

typedef struct {
	unsigned char	bEnable;
	unsigned char	nCaptureType; // 0: by interface(eth0.66, let0); 1: by interface type(wan, lan, wlan)
								  // Ex: UMTS is lte3 interface => 0: only lte3; 1: All wan interfaces(lte0, lte1, lte2, lte3)
	unsigned char	sIfName[32];
	unsigned char	nInterfaceType; // 0: wan; 1: lan: 2: wlan
	unsigned char	nInterfaceIndex;
	unsigned short	nVlanID; // 0 ~ 4096
	ebt_ulog_pvc	vpivci;
	unsigned char	bEnableRX;
	unsigned char	bEnableTX;
	ebt_ulog_cnt	counter;
} ebt_ulog_pkt_info;

static int				pktcap_enable = 0;
static int				pktcap_pkt_len_min = EBT_ULOG_MIN_PKT_LEN;

static ebt_ulog_pkt_info		pktcap_infolist[EBT_ULOG_PKT_MAX_INFO_COUNT];
static unsigned char	gbPktcap_debug_enable = 0;

extern void dump_pkt_info(ebt_ulog_pkt_info *pInfo);
extern ebt_ulog_pkt_info *FindPktInfo(unsigned char *sIfName, int nInterfaceType, int nInterfaceIndex, short vpi, unsigned short vci, unsigned short vlan_id);

/* send one ulog_buff_t to userspace */
static void ulog_send(unsigned int nlgroup)
{
	ebt_ulog_buff_t *ub = &ulog_buffers[nlgroup];

	if (timer_pending(&ub->timer))
		del_timer(&ub->timer);

	if (!ub->skb)
		return;

	/* last nlmsg needs NLMSG_DONE */
	if (ub->qlen > 1)
		ub->lastnlh->nlmsg_type = NLMSG_DONE;

	NETLINK_CB(ub->skb).dst_group = nlgroup + 1;
	netlink_broadcast(ebtulognl, ub->skb, 0, nlgroup + 1, GFP_ATOMIC);

	ub->qlen = 0;
	ub->skb = NULL;
}

/* timer function to flush queue in flushtimeout time */
static void ulog_timer(unsigned long data)
{
	spin_lock_bh(&ulog_buffers[data].lock);
	if (ulog_buffers[data].skb)
		ulog_send(data);
	spin_unlock_bh(&ulog_buffers[data].lock);
}

static struct sk_buff *ulog_alloc_skb(unsigned int size)
{
	struct sk_buff *skb;
	unsigned int n;

	n = max(size, nlbufsiz);
	skb = alloc_skb(n, GFP_ATOMIC);
	if (!skb) {
		PRINTR(KERN_ERR "ebt_ulog: can't alloc whole buffer "
		       "of size %ub!\n", n);
		if (n > size) {
			/* try to allocate only as much as we need for
			 * current packet */
			skb = alloc_skb(size, GFP_ATOMIC);
			if (!skb)
				PRINTR(KERN_ERR "ebt_ulog: can't even allocate "
				       "buffer of size %ub\n", size);
		}
	}

	return skb;
}

static void ebt_ulog_packet(unsigned int hooknr, const struct sk_buff *skb, int off,
   int dir, unsigned char *sIfName, const struct ebt_ulog_info *uloginfo, const char *prefix)
{
	ebt_ulog_packet_msg_t*	pm;
	size_t					size, copy_len;
	struct nlmsghdr*		nlh;
	unsigned int			group = uloginfo->nlgroup;
	ebt_ulog_buff_t*		ub = &ulog_buffers[group];
	spinlock_t*				lock = &ub->lock;

	if(gbPktcap_debug_enable)
		printk("[ebt_ulog_packet]\n");

	if ((uloginfo->cprange == 0) ||
	    (uloginfo->cprange > skb->len - off))
		copy_len = skb->len - off;
	else
		copy_len = uloginfo->cprange;

	size = NLMSG_SPACE(sizeof(*pm) + copy_len);
	if (size > nlbufsiz) {
		PRINTR("ebt_ulog: Size %Zd needed, but nlbufsiz=%d\n",
		       size, nlbufsiz);
		return;
	}

	spin_lock_bh(lock);

	if (!ub->skb) {
		if (!(ub->skb = ulog_alloc_skb(size)))
			goto alloc_failure;
	} else if (size > skb_tailroom(ub->skb)) {
		ulog_send(group);

		if (!(ub->skb = ulog_alloc_skb(size)))
			goto alloc_failure;
	}

	nlh = NLMSG_PUT(ub->skb, 0, ub->qlen, 0,
	                size - NLMSG_ALIGN(sizeof(*nlh)));
	ub->qlen++;

	pm = NLMSG_DATA(nlh);

	/* Fill in the ulog data */
	pm->version = EBT_ULOG_VERSION;
	do_gettimeofday(&pm->stamp);
	if (ub->qlen == 1)
		__net_timestamp(ub->skb);
	pm->data_len = copy_len;
	pm->mark = skb->mark;
	pm->hook = hooknr;
	if (uloginfo->prefix != NULL)
		strcpy(pm->prefix, uloginfo->prefix);
	else
		*(pm->prefix) = '\0';

	if (dir == 0) { /*ingress*/
		strcpy(pm->physindev, sIfName );
		strcpy(pm->indev, pm->physindev);
	} else
		pm->indev[0] = pm->physindev[0] = '\0';

	if (dir == 1) {
		/* If out exists, then out is a bridge port */
		strcpy(pm->physoutdev, sIfName );
		strcpy(pm->outdev, pm->physoutdev);
	} else
		pm->outdev[0] = pm->physoutdev[0] = '\0';

	if (skb_copy_bits(skb, off, pm->data, copy_len) < 0)
		BUG();

	if (ub->qlen > 1)
		ub->lastnlh->nlmsg_flags |= NLM_F_MULTI;

	ub->lastnlh = nlh;

	if (ub->qlen >= uloginfo->qthreshold)
		ulog_send(group);
	else if (!timer_pending(&ub->timer)) {
		ub->timer.expires = jiffies + flushtimeout * HZ / 1000;
		add_timer(&ub->timer);
	}

unlock:
	spin_unlock_bh(lock);

	return;

nlmsg_failure:
	printk(KERN_CRIT "ebt_ulog: error during NLMSG_PUT. This should "
	       "not happen, please report to author.\n");
	goto unlock;
alloc_failure:
	goto unlock;
}

/* this function is registered with the netfilter core
 *  skb - packet buffer
 *  off - offset of head of packet
 *  dir - packet direction, 0 - rx/ingress, 1 - tx/egress
 */
void ebt_log_packet( const struct sk_buff *skb, int off, int dir, int nInterfaceType, short vpi, int vci, unsigned short vlan_id)
{
	struct ebt_ulog_info loginfo;
	ebt_ulog_pkt_info *pInfo = NULL;

	if (!skb || (int)(skb->len-off) < pktcap_pkt_len_min)
		return;

	//printk("[ebt_log_packet] skb->dev->name=%s, off=%d, nInterfaceType=%d, dir=%d, vpi=%d, vci=%d, vlan_id=%d\n", skb->dev->name, off, nInterfaceType, dir, vpi, vci, vlan_id);
	pInfo = FindPktInfo( (skb->dev != NULL) ? skb->dev->name : NULL, nInterfaceType, 0, vpi, vci, vlan_id);
	if(pInfo == NULL) return;
	//dump_pkt_info(pInfo);

	if ( dir == 0 )
		pInfo->counter.rxCnt++;
	else
		pInfo->counter.txCnt++;

	// check loggable
	if(dir == 0)
	{
		if(pInfo->bEnableRX == 0)
		{
			printk("[ebt_log_packet] dir=0, bEnableRX=0, return\n");
			return;
		}
	}
	else if(dir == 1)
	{
		if(pInfo->bEnableTX == 0)
		{
			printk("[ebt_log_packet] dir=1, bEnableTX=0, return\n");
			return;
		}
	}
	else
	{
		printk("[ebt_log_packet] dir != 0 && dir != 1, return\n");
		return;
	}
	if( (vpi != 0) && (vci != 0) )
	{
		if( (pInfo->vpivci.vpi != vpi) || (pInfo->vpivci.vci != vci) )
		{
			printk("[ebt_log_packet] vpi, vci error, return\n");
			return;
		}
	}
	if(pInfo->nCaptureType == EBT_ULOG_PKT_BY_INTERFACE)
	{
		if(vlan_id != 0)
		{
			if(pInfo->nVlanID != vlan_id)
			{
				printk("[ebt_log_packet] pInfo->nVlanID, vlan_id=%d, return\n", pInfo->nVlanID, vlan_id);
				return;
			}
		}
	}
	//

	if ( dir == 0 )
		pInfo->counter.rxCpy++;
	else
		pInfo->counter.txCpy++;

	loginfo.nlgroup = EBT_ULOG_DEFAULT_NLGROUP;
	loginfo.cprange = 0;
	loginfo.qthreshold = EBT_ULOG_DEFAULT_QTHRESHOLD;
	loginfo.prefix[0] = '\0';

	ebt_ulog_packet( 0, skb, off, dir, pInfo->sIfName, &loginfo, "WAN_CAP");
}

EXPORT_SYMBOL(ebt_log_packet);

static void proc_write_pktcap_reset_info(int nIndex)
{
	if( (nIndex < 0) || (nIndex >= EBT_ULOG_PKT_MAX_INFO_COUNT) )
		return;
	memset(&pktcap_infolist[nIndex], 0, sizeof(ebt_ulog_pkt_info) );
	pktcap_infolist[nIndex].vpivci.vpi = -1;
}

static void proc_write_pktcap_clear_info_counter()
{
	int i;

	for(i=0; i<EBT_ULOG_PKT_MAX_INFO_COUNT; i++)
	{
		if(pktcap_infolist[i].bEnable == 0) continue;

		memset(&pktcap_infolist[i].counter, 0, sizeof(ebt_ulog_cnt) );
	}
}

static void proc_write_pktcap_reset( void )
{
	int	cnt;

	pktcap_enable = 0;

	for (cnt = 0; cnt < EBT_ULOG_PKT_MAX_INFO_COUNT; cnt++ ) {
		proc_write_pktcap_reset_info(cnt);
	}

	pktcap_pkt_len_min = EBT_ULOG_MIN_PKT_LEN;
}

static void proc_write_pktcap_help( void )
{
	printk(	"Packet capturing:\n"
			"    help                  - show this help message\n"
			"    reset                 - reset packet capturing function to default settings\n"
			"    clear                 - clear counters\n"
			"    enable                - enable packet capturing function\n"
			"    disable               - disable packet capturing function\n"
			"    on  <port> <dir>      - enable <port> <dir> packet capturing\n"
			"    off <port> <dir>      - disable <port> <dir> packet capturing\n"
			"    add <vpi> <vci> <dir> - add ADSL <vpi> <vci> <dir> into packet capturing\n"
			"    del <vpi> <vci> <dir> - remove ADSL <vpi> <vci> <dir> from packet capturing\n"
			"    set <pkt_min_len>     - filter out those packets whose lengthes are less than <pkt_min_len>\n"
			"  <port>                  - eth0, eth1, wlan, ipoa, dsl, or ID 0~15\n"
			"  <dir>                   - rx, tx\n"
			"  <vpi>                   - 0 - 255\n"
			"  <vci>                   - 0 - 65535\n"
			"  <pkt_min_len>           - 0 - 65535, default %d\n"
			, EBT_ULOG_MIN_PKT_LEN
		);
}

static long arc_simple_strtol( const char *cp, char **endp, unsigned int base )
{
	while (*cp==' ' || *cp=='\t')
		cp++;

	if(*cp=='-')
		return -simple_strtoul(cp+1,endp,base);
	return simple_strtoul(cp,endp,base);
}

ebt_ulog_pkt_info *GetFreePktInfo()
{
	int i;

	for(i=0; i<EBT_ULOG_PKT_MAX_INFO_COUNT; i++)
	{
		if(pktcap_infolist[i].bEnable == 1) continue;
		printk("[GetFreePktInfo] find free index %d:0x%08x\n", i, &pktcap_infolist[i]);
		return &pktcap_infolist[i];
	}
	printk("[GetFreePktInfo] can not find free entry\n");
	return NULL;
}

ebt_ulog_pkt_info *FindPktInfo(unsigned char *sIfName, int nInterfaceType, int nInterfaceIndex, short vpi, unsigned short vci, unsigned short vlan_id)
{
	int i, ret;
	unsigned char bATMInterface = 0;

	//printk("[FindPktInfo] sIfName=%s, nInterfaceType=%d, nInterfaceIndex=%d, vpi=%d, vci=%d, vlan_id=%d\n", sIfName, nInterfaceType, nInterfaceIndex, vpi, vci, vlan_id);
	if( (vpi >= 0) && (vci != 0) )
	{
		bATMInterface = 1;
	}
	for(i=0; i<EBT_ULOG_PKT_MAX_INFO_COUNT; i++)
	{
		if(pktcap_infolist[i].bEnable == 0) continue;

		//dump_pkt_info(&pktcap_infolist[i]);

		if(pktcap_infolist[i].nCaptureType == EBT_ULOG_PKT_BY_INTERFACE)
		{
			// ATM: sIfName is NULL
			if(sIfName != NULL)
			{
				if ( strnicmp( pktcap_infolist[i].sIfName, sIfName, strlen(sIfName) ) != 0 )
					continue;
			}
		}

		if(pktcap_infolist[i].nInterfaceType != nInterfaceType)
			continue;

#if 0
		if(pktcap_infolist[i].nInterfaceIndex != nInterfaceIndex)
			continue;
#endif

		if( (vpi >= 0) && (vci != 0) )
		{
			if(pktcap_infolist[i].vpivci.vpi != vpi)
				continue;
			if(pktcap_infolist[i].vpivci.vci != vci)
				continue;
		}

		if(pktcap_infolist[i].nCaptureType == EBT_ULOG_PKT_BY_INTERFACE)
		{
			if(vlan_id != 0)
			{
				if(pktcap_infolist[i].nVlanID != vlan_id)
					continue;
			}
		}

		if(gbPktcap_debug_enable)
			printk("[FindPktInfo] find index %d:0x%08x\n", i, &pktcap_infolist[i]);
		return &pktcap_infolist[i];
	}
	//printk("[FindPktInfo] can not find the existed entry\n");
	return NULL;
}

/*
typedef struct {
	unsigned char	bEnable;
	unsigned char	bEnableRX;
	unsigned char	bEnableTX;
	unsigned char	nInterfaceType; // 0: wan; 1: lan: 2: wlan
	unsigned char	sIfName[32];
	unsigned char	nInterfaceIndex;
	unsigned short	nVlanID; // 0 ~ 4096
	ebt_ulog_pvc	vpivci;
	ebt_ulog_cnt	counter;
} ebt_ulog_pkt_info;
*/
void dump_pkt_info(ebt_ulog_pkt_info *pInfo)
{
	if(pInfo == NULL) return;
	printk("[dump_pkt_info]\n");
	printk("[dump_pkt_info] bEnable=%d, bEnableRX=%d, bEnableTX=%d, nInterfaceType=%d, nCaptureType=%d\n", pInfo->bEnable, pInfo->bEnableRX, pInfo->bEnableTX, pInfo->nInterfaceType, pInfo->nCaptureType);
	printk("[dump_pkt_info] sIfName=%s, nInterfaceIndex=%d, nVlanID=%d, vpi=%d, vci=%d\n", pInfo->sIfName, pInfo->nInterfaceIndex, pInfo->nVlanID, pInfo->vpivci.vpi, pInfo->vpivci.vci);
	printk("[dump_pkt_info] end\n");
}

#define atoi(str) simple_strtoul(((str != NULL) ? str : ""), NULL, 0)
static int proc_write_pktcap( struct file*	file, const char* buf, unsigned long count, void* data )
{
	char*			pPtr;
	char			sBuf[128];
	int				portbitmap;
	long 			portid;
	short			vpi = -1;
	unsigned short 	vci = 0;
	ebt_ulog_pvc*	ppvclist;
	int				tmp, en, cnt;
	int				nCaptureType = 0; // 0: by interface; 1: by interface type
	int 			nInterfaceType; // 0: wan; 1: lan; 2: wlan
	int				nInterfaceIndex;
	unsigned char	sIfName[32];
	unsigned short	vlan_id = 0;
	int 			i;
	char*			pTmpPtr;
	char			sTmpData[128];
	int				len;
	ebt_ulog_pkt_info *pInfo = NULL;
	int				nEnableRX = -1, nEnableTX = -1;

	sBuf[0] = sBuf[sizeof(sBuf)-1] = '\0';

	printk("[proc_write_pktcap]\n");
	/* trim the tailing space, tab and LF/CR*/
	if ( count > 0 )
	{
		if (count >= sizeof(sBuf))
			count = sizeof(sBuf) - 1;

		if (copy_from_user(sBuf, buf, count))
			return count;

		pPtr = (char*)sBuf + count - 1;

		for (; *pPtr==' ' || *pPtr=='\t' || *pPtr=='\n' || *pPtr=='\r'; pPtr++)
		{
			*pPtr = '\0';
		}
	}

	if ( strnicmp( sBuf, "reset", sizeof("reset")-1 ) == 0 )
	{
		proc_write_pktcap_reset();
		return count;
	}

	/* clear */
	if ( strnicmp( sBuf, "clear", sizeof("clear")-1 ) == 0 )
	{
		proc_write_pktcap_clear_info_counter();
		return count;
	}

	/* enable */
	if ( strnicmp( sBuf, "enable", sizeof("enable")-1 ) == 0 )
	{
		pktcap_enable = 1;
		return count;
	}

	/* disable */
	if ( strnicmp( sBuf, "disable", sizeof("disable")-1 ) == 0 )
	{
		pktcap_enable = 0;
		return count;
	}

	/* debug-on */
	if ( strnicmp( sBuf, "debug-on", sizeof("debug-on")-1 ) == 0 )
	{
		gbPktcap_debug_enable = 1;
		return count;
	}

	/* debug-off */
	if ( strnicmp( sBuf, "debug-off", sizeof("debug-off")-1 ) == 0 )
	{
		gbPktcap_debug_enable = 0;
		return count;
	}

	// on wan $IFNAME $INTERFACE_INDEX vpivci $VPI $VCI vlan $VLAN_ID rx
	// on wan $IFNAME $INTERFACE_INDEX vpivci $VPI $VCI vlan $VLAN_ID tx
	// off wan $IFNAME $INTERFACE_INDEX vlan $VLAN_ID rx
	// off wan $IFNAME $INTERFACE_INDEX vlan $VLAN_ID tx
	/* on / off */
	if ( strnicmp( sBuf, "on", sizeof("on")-1 ) == 0 || strnicmp( sBuf, "off", sizeof("off")-1 ) == 0 )
	{	// enable or disable => "on" or "off"
		if(pktcap_enable==0)
		{
			printk("[proc_write_pktcap] You must enable packet capture first\n");
			return count;
		}

		if (strnicmp( sBuf, "on", sizeof("on")-1 ) == 0) {
			en = 1;
			for (pPtr=sBuf+sizeof("on"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

			printk("[proc_write_pktcap] on\n");
		} else {
			en = 0;
			for (pPtr=sBuf+sizeof("off"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

			printk("[proc_write_pktcap] off\n");
		}

		// capture type: "wan", "lan" or "wlan"
		if(strnicmp(pPtr, EBT_ULOG_PKT_WAN_TYPE_STRING, sizeof(EBT_ULOG_PKT_WAN_TYPE_STRING)-1 ) == 0)
		{
			nInterfaceType = EBT_ULOG_PKT_WAN_TYPE;
			pPtr += sizeof(EBT_ULOG_PKT_WAN_TYPE_STRING);

			printk("[proc_write_pktcap] wan\n");
		}
		else if(strnicmp(pPtr, EBT_ULOG_PKT_LAN_TYPE_STRING, sizeof(EBT_ULOG_PKT_LAN_TYPE_STRING)-1 ) == 0)
		{
			nInterfaceType = EBT_ULOG_PKT_LAN_TYPE;
			pPtr += sizeof(EBT_ULOG_PKT_LAN_TYPE_STRING);

			printk("[proc_write_pktcap] lan\n");
		}
		else if(strnicmp(pPtr, EBT_ULOG_PKT_WLAN_TYPE_STRING, sizeof(EBT_ULOG_PKT_WLAN_TYPE_STRING)-1 ) == 0)
		{
			nInterfaceType = EBT_ULOG_PKT_WLAN_TYPE;
			pPtr += sizeof(EBT_ULOG_PKT_WLAN_TYPE_STRING);

			printk("[proc_write_pktcap] wlan\n");
		}

		// interface name: "eth0*", "eth1*", "ptm*", "nas*" or "lte*"
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if( (strnicmp(pPtr, "eth0", sizeof("eth0")-1 ) == 0)
			|| (strnicmp(pPtr, "eth1", sizeof("eth1")-1 ) == 0)
			|| (strnicmp(pPtr, "ptm", sizeof("ptm")-1 ) == 0)
			|| (strnicmp(pPtr, "nas", sizeof("nas")-1 ) == 0)
			|| (strnicmp(pPtr, "lte", sizeof("lte")-1 ) == 0) )
		{
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			memset(sIfName, 0, sizeof(sIfName));
			memcpy(sIfName, pPtr, pTmpPtr - pPtr);
			pPtr = pTmpPtr;

			// check whther we can get vlan id. => find '.' within sIfName[]
			len = strlen(sIfName);
			i=0;
			pTmpPtr=NULL;
			while(len > 0)
			{
				if(sIfName[i] == '.')
				{
					pTmpPtr=sIfName + i + 1;
					break;
				}
				len--;
				i++;
			}
			vlan_id = atoi(pTmpPtr);

			// interface index: 0-based
			for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			memset(sTmpData, 0, sizeof(sTmpData));
			memcpy(sTmpData, pPtr, pTmpPtr - pPtr);
			nInterfaceIndex=atoi(sTmpData);
			pPtr = pTmpPtr;
			printk("[proc_write_pktcap] sIfName=%s, nInterfaceIndex=%d, vlan_id=%d\n", sIfName, nInterfaceIndex, vlan_id);
		}

		// capture type: 0(by interface); 1(by interface type)
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if( (strnicmp(pPtr, "capture_type", sizeof("capture_type")-1 ) == 0) )
		{
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			pPtr = pTmpPtr;

			// capture tpe
			for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			memset(sTmpData, 0, sizeof(sTmpData));
			memcpy(sTmpData, pPtr, pTmpPtr - pPtr);
			nCaptureType=atoi(sTmpData);
			pPtr = pTmpPtr;

			printk("[proc_write_pktcap] nCaptureType=%d\n", nCaptureType);
		}

		// vpivci
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if( (strnicmp(pPtr, "vpivci", sizeof("vpivci")-1 ) == 0) )
		{
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			pPtr = pTmpPtr;

			// vpi
			for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			memset(sTmpData, 0, sizeof(sTmpData));
			memcpy(sTmpData, pPtr, pTmpPtr - pPtr);
			vpi=atoi(sTmpData);
			pPtr = pTmpPtr;

			// vci
			for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			memset(sTmpData, 0, sizeof(sTmpData));
			memcpy(sTmpData, pPtr, pTmpPtr - pPtr);
			vci=atoi(sTmpData);
			pPtr = pTmpPtr;

			printk("[proc_write_pktcap] vpi=%d, vci=%d\n", vpi, vci);
		}

		// vlan
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if( (strnicmp(pPtr, "vlan", sizeof("vlan")-1 ) == 0) )
		{
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			pPtr = pTmpPtr;

			// vlan id
			for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
			pTmpPtr=pPtr;
			for (; *pTmpPtr!=' ' && *pTmpPtr!='\t'; pTmpPtr++) { }
			memset(sTmpData, 0, sizeof(sTmpData));
			memcpy(sTmpData, pPtr, pTmpPtr - pPtr);
			vlan_id=atoi(sTmpData);
			pPtr = pTmpPtr;

			printk("[proc_write_pktcap] vlan_id=%d\n", vlan_id);
		}

		// rx or tx
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if( (strnicmp(pPtr, "rx", sizeof("rx")-1 ) == 0) )
		{
			nEnableRX = 1;
			printk("[proc_write_pktcap] rx\n");
		}
		else if( (strnicmp(pPtr, "tx", sizeof("tx")-1 ) == 0) )
		{
			nEnableTX = 1;
			printk("[proc_write_pktcap] tx\n");
		}

		// set data to pkt info
		printk("[proc_write_pktcap] sIfName=%s, nInterfaceType=%d, vpi=%d, vci=%d, vlan_id=%d, line %d\n", sIfName, nInterfaceType, vpi, vci, vlan_id, __LINE__);
		if(en==1)
		{
			pInfo = FindPktInfo(sIfName, nInterfaceType, nInterfaceIndex, vpi, vci, vlan_id);
			if(pInfo == NULL)
			{
				pInfo = GetFreePktInfo();
				if(pInfo == NULL)
				{
					printk("[proc_write_pktcap] en=1, can not get free pkt ifno, line %d\n", __LINE__);
					return count;
				}
			}

			pInfo->bEnable = 1;
			strcpy(pInfo->sIfName, sIfName);
			pInfo->nCaptureType = nCaptureType;
			pInfo->nInterfaceType = nInterfaceType;
			pInfo->nInterfaceIndex = nInterfaceIndex;
			if(vpi != -1)
			{
				pInfo->vpivci.vpi = vpi;
				pInfo->vpivci.vci = vci;
			}
			if(vlan_id != 0)
			{
				pInfo->nVlanID = vlan_id;
			}
			if(nEnableRX != -1)
				pInfo->bEnableRX = nEnableRX;
			if(nEnableTX != -1)
				pInfo->bEnableTX = nEnableTX;
			dump_pkt_info(pInfo);
		}
		else
		{	// reset
			pInfo = FindPktInfo(sIfName, nInterfaceType, nInterfaceIndex, vpi, vci, vlan_id);
			if(pInfo == NULL)
			{
				printk("[proc_write_pktcap] en=0, can not get pkt ifno, line %d\n", __LINE__);
				return count;
			}
			memset(pInfo, 0, sizeof(ebt_ulog_pkt_info) );
			pInfo->vpivci.vpi = -1;
		}
		return count;
	}

	proc_write_pktcap_help();
	return count;
}

static int proc_read_pktcap(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int	cnt;
	int	len = 0;
	int i;

	len += sprintf( buf+len, "packet capturing: %s\n", (pktcap_enable ? "enable" : "disable") );

/*
typedef struct {
	unsigned char	bEnable;
	unsigned char	sIfName[32];
	unsigned char	nInterfaceType; // 0: wan; 1: lan: 2: wlan
	unsigned char	nInterfaceIndex;
	unsigned short	nVlanID; // 0 ~ 4096
	ebt_ulog_pvc	vpivci;
	unsigned char	bEnableRX;
	unsigned char	bEnableTX;
	ebt_ulog_cnt	counter;
} ebt_ulog_pkt_info;
*/
	for(i=0; i<EBT_ULOG_PKT_MAX_INFO_COUNT; i++)
	{
		if(pktcap_infolist[i].bEnable == 0) continue;
		len += sprintf( buf+len, "index:%d\n", i );
		len += sprintf( buf+len, "	sIfName:%s\n", pktcap_infolist[i].sIfName );
		len += sprintf( buf+len, "	nInterfaceType:%d\n", pktcap_infolist[i].nInterfaceType );
		len += sprintf( buf+len, "	nInterfaceIndex:%d\n", pktcap_infolist[i].nInterfaceIndex );
		len += sprintf( buf+len, "	nVlanID:%d\n", pktcap_infolist[i].nVlanID );
		len += sprintf( buf+len, "	vpi:%d\n", pktcap_infolist[i].vpivci.vpi );
		len += sprintf( buf+len, "	vci:%d\n", pktcap_infolist[i].vpivci.vci );
		len += sprintf( buf+len, "	bEnableRX:%d\n", pktcap_infolist[i].bEnableRX );
		len += sprintf( buf+len, "	bEnableTX:%d\n", pktcap_infolist[i].bEnableTX );
		len += sprintf( buf+len, "	rxCnt:%d\n", pktcap_infolist[i].counter.rxCnt );
		len += sprintf( buf+len, "	rxCpy:%d\n", pktcap_infolist[i].counter.rxCpy );
		len += sprintf( buf+len, "	txCnt:%d\n", pktcap_infolist[i].counter.txCnt );
		len += sprintf( buf+len, "	txCpy:%d\n", pktcap_infolist[i].counter.txCpy );
	}

	return len;
}

static int ebt_ulog_init(void)
{
	struct proc_dir_entry*	proc_file_conf;
	int i, ret = 0;

	if (nlbufsiz >= 128*1024) {
		printk(KERN_NOTICE "ebt_ulog: Netlink buffer has to be <= 128kB,"
		       " please try a smaller nlbufsiz parameter.\n");
		return -EINVAL;
	}

	/* initialize ulog_buffers */
	for (i = 0; i < EBT_ULOG_MAXNLGROUPS; i++) {
		init_timer(&ulog_buffers[i].timer);
		ulog_buffers[i].timer.function = ulog_timer;
		ulog_buffers[i].timer.data = i;
		spin_lock_init(&ulog_buffers[i].lock);
	}

	ebtulognl = netlink_kernel_create(&init_net, NETLINK_PKTCAP,
					  EBT_ULOG_MAXNLGROUPS, NULL, NULL,
					  THIS_MODULE);
	if (!ebtulognl)
		ret = -ENOMEM;

  #ifdef CONFIG_ARC_PPP_ULOG
  {	extern void dev_ppp_proc_set_nl_sock( struct sock* new_nl_sock );
	dev_ppp_proc_set_nl_sock( ebtulognl );
  }
  #endif /* CONFIG_ARC_PPP_ULOG */

	proc_write_pktcap_reset();

//	proc_mkdir("eth", NULL);
	proc_file_conf = create_proc_entry("eth/pktcap", 0666, NULL);
	if (proc_file_conf == NULL) {
		printk( "error creating entry in /proc/eth/pktcap\n" );
	} else {
		proc_file_conf->write_proc = proc_write_pktcap;
		proc_file_conf->read_proc  = proc_read_pktcap;
	}

	return ret;
}

static void ebt_ulog_fini(void)
{
	ebt_ulog_buff_t *ub;
	int i;

  #ifdef CONFIG_ARC_PPP_ULOG
  {	extern void dev_ppp_proc_set_nl_sock( struct sock* new_nl_sock );
	dev_ppp_proc_set_nl_sock( NULL );
  }
  #endif /* CONFIG_ARC_PPP_ULOG */

    remove_proc_entry("eth/pktcap", NULL);

	for (i = 0; i < EBT_ULOG_MAXNLGROUPS; i++) {
		ub = &ulog_buffers[i];
		if (timer_pending(&ub->timer))
			del_timer(&ub->timer);
		spin_lock_bh(&ub->lock);
		if (ub->skb) {
			kfree_skb(ub->skb);
			ub->skb = NULL;
		}
		spin_unlock_bh(&ub->lock);
	}
	sock_release(ebtulognl->sk_socket);
}
