/*
 * Ethernet netdevice using ATM AAL5 as underlying carrier
 * (RFC1483 obsoleted by RFC2684) for Linux
 *
 * Authors: Marcell GAL, 2000, XDSL Ltd, Hungary
 *          Eric Kinzie, 2006-2007, US Naval Research Laboratory
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <linux/ip.h>
#include <asm/uaccess.h>
#include <net/arp.h>
#include <linux/atm.h>
#include <linux/atmdev.h>
#include <linux/capability.h>
#include <linux/seq_file.h>

#include <linux/atmbr2684.h>
#ifdef CONFIG_WAN_VLAN_SUPPORT
#include <linux/if_vlan.h>
#endif

#include "common.h"

#ifdef SKB_DEBUG
static void skb_debug(const struct sk_buff *skb)
{
#define NUM2PRINT 50
	char buf[NUM2PRINT * 3 + 1];	/* 3 chars per byte */
	int i = 0;
	for (i = 0; i < skb->len && i < NUM2PRINT; i++) {
		sprintf(buf + i * 3, "%2.2x ", 0xff & skb->data[i]);
	}
	printk(KERN_DEBUG "br2684: skb[%p]: %s\n", skb->data, buf);
}
#else
#define skb_debug(skb)	do {} while (0)
#endif

#define BR2684_LLC_LEN		3
#define BR2684_SNAP_LEN		3
#define BR2684_ETHERTYPE_LEN	2
#define BR2684_PID_LEN		2
#define BR2684_PAD_LEN		2

#define LLC		0xaa, 0xaa, 0x03
#define SNAP_BRIDGED	0x00, 0x80, 0xc2
#define SNAP_ROUTED	0x00, 0x00, 0x00
#define PID_ETHERNET	0x00, 0x07
#define ETHERTYPE_IPV4	0x08, 0x00
#define ETHERTYPE_IPV6	0x86, 0xdd
#define PAD_BRIDGED	0x00, 0x00

static const unsigned char ethertype_ipv4[] = { ETHERTYPE_IPV4 };
static const unsigned char ethertype_ipv6[] = { ETHERTYPE_IPV6 };
static const unsigned char llc_oui_pid_pad[] =
			{ LLC, SNAP_BRIDGED, PID_ETHERNET, PAD_BRIDGED };
static const unsigned char llc_oui_ipv4[] = { LLC, SNAP_ROUTED, ETHERTYPE_IPV4 };
static const unsigned char llc_oui_ipv6[] = { LLC, SNAP_ROUTED, ETHERTYPE_IPV6 };

enum br2684_encaps {
	e_vc = BR2684_ENCAPS_VC,
	e_llc = BR2684_ENCAPS_LLC,
};

struct br2684_vcc {
	struct atm_vcc *atmvcc;
	struct net_device *device;
	/* keep old push, pop functions for chaining */
	void (*old_push) (struct atm_vcc * vcc, struct sk_buff * skb);
	void (*old_pop)(struct atm_vcc *vcc, struct sk_buff *skb);
	enum br2684_encaps encaps;
	struct list_head brvccs;
#ifdef CONFIG_ATM_BR2684_IPFILTER
	struct br2684_filter filter;
#endif /* CONFIG_ATM_BR2684_IPFILTER */
	unsigned copies_needed, copies_failed;
};

#ifdef CONFIG_WAN_VLAN_SUPPORT
extern struct wan_vlan_struct;
uint32_t g_br2684_tag_vlan_enable;
#endif
struct br2684_dev {
	struct net_device *net_dev;
	struct list_head br2684_devs;
	int number;
	struct list_head brvccs;	/* one device <=> one vcc (before xmas) */
	int mac_was_set;
	enum br2684_payload payload;
#ifdef CONFIG_WAN_VLAN_SUPPORT
	struct wan_vlan_struct vlan;
#endif
};

/*
 * This lock should be held for writing any time the list of devices or
 * their attached vcc's could be altered.  It should be held for reading
 * any time these are being queried.  Note that we sometimes need to
 * do read-locking under interrupt context, so write locking must block
 * the current CPU's interrupts
 */
static DEFINE_RWLOCK(devs_lock);

static LIST_HEAD(br2684_devs);

static inline struct br2684_dev *BRPRIV(const struct net_device *net_dev)
{
	return (struct br2684_dev *)netdev_priv(net_dev);
}

static inline struct net_device *list_entry_brdev(const struct list_head *le)
{
	return list_entry(le, struct br2684_dev, br2684_devs)->net_dev;
}

static inline struct br2684_vcc *BR2684_VCC(const struct atm_vcc *atmvcc)
{
	return (struct br2684_vcc *)(atmvcc->user_back);
}

static inline struct br2684_vcc *list_entry_brvcc(const struct list_head *le)
{
	return list_entry(le, struct br2684_vcc, brvccs);
}

static inline struct br2684_vcc *pick_outgoing_vcc(const struct sk_buff *skb,
						   const struct br2684_dev *brdev)
{
	return list_empty(&brdev->brvccs) ? NULL : list_entry_brvcc(brdev->brvccs.next);	/* 1 vcc/dev right now */
}

#ifdef CONFIG_WAN_VLAN_SUPPORT
static inline void dump_skb(u32 len, char * data)
{
	int i;
#if 0
	printk("addr = %p\n",data);
	for(i=0;i<len;i++){
		 printk("0x%x ",(u8)(data[i]));
		 if (i % 16 == 15)
			  printk("\n");
	}
	printk("\n");
#endif
}
static int br2684_insert_vlan_tag(struct sk_buff *skb, struct br2684_dev *brdev)
{
	struct br2684_vcc *brvcc = pick_outgoing_vcc(skb, brdev);
	struct vlan_ethhdr *vehdr=NULL;

	//printk("brdev = (%p) payload=(%d) p_bridged=(%d)\n", brdev, brdev->payload, p_bridged);
	/* cannot be null, as already checked */
	//if ( brvcc->payload != p_bridged ) {
	if ( brdev->payload != p_bridged ) {
		 printk("brvcc is not bridged!Cannot insert VLAN header\n");
		 return -1;
	}
	if ( g_br2684_tag_vlan_enable == 0) {
		 return -1;
	}
	//printk("g_br2684_tag_vlan_enabled!\nProceed to add vlan tag\n");
	//dump_skb(32, skb->data);

	//if (skb_headroom(skb) < VLAN_HLEN ) {
	//      struct sk_buff *skb2 = skb_realloc_headroom(skb, VLAN_HLEN);
	//
	//      printk("headroom [%d] less than VLAN_HLEN [%d] skb2 [%p]\n",skb_headroom(skb), VLAN_HLEN, skb2);
	//      brvcc->copies_needed++;
	//      dev_kfree_skb(skb);
	//      printk("skb is freed\n");
	//      if (skb2 == NULL) {
	//		printk("skb is NULL returning\n");
	//		brvcc->copies_failed++;
	//		return 0;
	//      }
	//      skb = skb2;
	//      skb2=NULL;
	//}
	vehdr = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);
    //printk("vehdr = %p\n",vehdr);
	memmove(skb->data, skb->data+VLAN_HLEN, 2*ETH_ALEN);
	vehdr->h_vlan_proto = __constant_htons(ETH_P_8021Q);
	// vehdr->h_vlan_TCI = brdev->vlan.vlan_vci;
	if(skb->mark==0)
	{
		vehdr->h_vlan_TCI = brdev->vlan.vlan_vci;
	}
	else
	{
		vehdr->h_vlan_TCI = (((skb->mark-1) << 13) |( brdev->vlan.vlan_vci & 0x1FFF ));
	}

	//printk("\nh_vlan_proto=[%d]   h_vlan_TCI=[%d]\n", vehdr->h_vlan_proto, vehdr->h_vlan_TCI);
	//printk("VLAN header INSERTED in this skb\n");
	//dump_skb(32, skb->data);

	return 0;
}

int br2684_remove_vlan_tag( struct sk_buff *skb, struct br2684_dev *brdev, int f_untag)
{
	struct br2684_vcc *brvcc = pick_outgoing_vcc(skb, brdev);
	struct vlan_ethhdr *vehdr=NULL;

	//printk("brdev = (%p) payload=(%d) p_bridged=(%d)\n", brdev, brdev->payload, p_bridged);
	/* cannot be null, as already checked */
	//if ( brvcc->payload != p_bridged ) {
	if ( brdev->payload != p_bridged ) {
		 printk("brvcc is not bridged!Cannot insert VLAN header\n");
		 return -1;
	}
	if ( g_br2684_tag_vlan_enable == 0) {
		 return -1;
	}

	/* Check if pkt has a VLAN tag. */
	vehdr = (struct vlan_ethhdr *) (skb->data);

    //printk("vehdr = %p\n",vehdr);
    //dump_skb(32, skb->data);
    //printk("(UNTAG):vci [%d] : dev vci [%d] f_untag [%d]\n", vehdr->h_vlan_TCI, brdev->vlan.vlan_vci, f_untag);
	if ( f_untag || vehdr->h_vlan_TCI == (brdev->vlan.vlan_vci & 0x00FF)) {
			  char *ptr1 = (char *) &(vehdr->h_vlan_encapsulated_proto);
			  char *ptr2 = (char *) &(vehdr->h_vlan_proto);
			  int i=0;
			  int rem_len=2*ETH_ALEN;

			  skb->protocol = vehdr->h_vlan_encapsulated_proto;
			  for (i=0; i<rem_len; i++)
			  {
					    *(--ptr1) = *(--ptr2);
			  }
			  skb_pull(skb, VLAN_HLEN);
			  skb->protocol = ((u16 *) skb->data)[-1];
			  //printk("VLAN header REMOVED in this skb\n");
			  //dump_skb(32, skb->data);
	}
	return 0;
}
int br2684_remove_vlan_tag_rcv( struct sk_buff *skb, struct br2684_dev *brdev, int f_untag)
{
	struct br2684_vcc *brvcc = pick_outgoing_vcc(skb, brdev);
	struct vlan_ethhdr *vehdr=NULL;

	//printk("\nEntering : (%s)\n", __FUNCTION__);

	//printk("brdev = (%p) payload=(%d) p_bridged=(%d)\n", brdev, brdev->payload, p_bridged);
	/* cannot be null, as already checked */
	//if ( brvcc->payload != p_bridged ) {
	if ( brdev->payload != p_bridged ) {
		 printk("brvcc is not bridged!Cannot insert VLAN header\n");
		 return -1;
	}
	if ( g_br2684_tag_vlan_enable == 0) {
		 return -1;
	}

	/* Check if pkt has a VLAN tag. */
	vehdr = (struct vlan_ethhdr *) (eth_hdr(skb)); //in RCV path, skb->data is pointing to ntwk hdr.

    //dump_skb(32, skb->data);
    //printk("(REMOVE):vci [%d] : dev vci [%d] f_untag [%d]\n", vehdr->h_vlan_TCI, brdev->vlan.vlan_vci, f_untag);
	if ( f_untag || vehdr->h_vlan_TCI == (brdev->vlan.vlan_vci & 0x0FFF)) {
			  char *ptr1 = (char *) &(vehdr->h_vlan_encapsulated_proto);
			  char *ptr2 = (char *) &(vehdr->h_vlan_proto);
			  int i=0;
			  int rem_len=2*ETH_ALEN;

			  skb->data = eth_hdr(skb);

			  for (i=0; i<rem_len; i++)
			  {
					    *(--ptr1) = *(--ptr2);
					    *(ptr2) = 0x00;
			  }
			  skb_pull(skb, VLAN_HLEN);
			  //printk("VLAN header REMOVED in this skb\n");
			  //dump_skb(32, skb->data);
		#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
			  skb->mac.raw +=VLAN_HLEN;
		#else
			  skb->mac_header +=VLAN_HLEN;
		#endif
			  skb->data += ETH_HLEN; //again make it point to ntwk hdr
			  skb->protocol = ((u16 *) skb->data)[-1];
	}
	return 0;
}
#endif

/* Caller should hold read_lock(&devs_lock) */
static struct net_device *br2684_find_dev(const struct br2684_if_spec *s)
{
	struct list_head *lh;
	struct net_device *net_dev;
	switch (s->method) {
	case BR2684_FIND_BYNUM:
		list_for_each(lh, &br2684_devs) {
			net_dev = list_entry_brdev(lh);
			if (BRPRIV(net_dev)->number == s->spec.devnum)
				return net_dev;
		}
		break;
	case BR2684_FIND_BYIFNAME:
		list_for_each(lh, &br2684_devs) {
			net_dev = list_entry_brdev(lh);
			if (!strncmp(net_dev->name, s->spec.ifname, IFNAMSIZ))
				return net_dev;
		}
		break;
	}
	return NULL;
}

/* chained vcc->pop function.  Check if we should wake the netif_queue */
static void br2684_pop(struct atm_vcc *vcc, struct sk_buff *skb)
{
	struct br2684_vcc *brvcc = BR2684_VCC(vcc);
	struct net_device *net_dev = skb->dev;

	pr_debug("br2684_pop(vcc %p ; net_dev %p )\n", vcc, net_dev);
	brvcc->old_pop(vcc, skb);

	if (!net_dev)
		return;

	if (atm_may_send(vcc, 0))
		netif_wake_queue(net_dev);

}
/*
 * Send a packet out a particular vcc.  Not to useful right now, but paves
 * the way for multiple vcc's per itf.  Returns true if we can send,
 * otherwise false
 */
static int br2684_xmit_vcc(struct sk_buff *skb, struct net_device *dev,
			   struct br2684_vcc *brvcc)
{
	struct br2684_dev *brdev = BRPRIV(dev);
	struct atm_vcc *atmvcc;
#ifdef CONFIG_WAN_VLAN_SUPPORT
	int minheadroom = (brvcc->encaps == e_llc) ? 14 : 6;
#else
	int minheadroom = (brvcc->encaps == e_llc) ? 10 : 2;
#endif // CONFIG_WAN_VLAN_SUPPORT

	if (skb_headroom(skb) < minheadroom) {
		struct sk_buff *skb2 = skb_realloc_headroom(skb, minheadroom);
		brvcc->copies_needed++;
		dev_kfree_skb(skb);
		if (skb2 == NULL) {
			brvcc->copies_failed++;
			return 0;
		}
		skb = skb2;
	}
#ifdef CONFIG_WAN_VLAN_SUPPORT
	if(g_br2684_tag_vlan_enable) {
		if ( brdev->vlan.untag_vlan) {
			br2684_remove_vlan_tag(skb, brdev, 1);
		}
		if ( brdev->vlan.tag_vlan_enable) {
			br2684_insert_vlan_tag(skb, brdev);
		}
	}
#endif
	if (brvcc->encaps == e_llc) {
		if (brdev->payload == p_bridged) {
			skb_push(skb, sizeof(llc_oui_pid_pad));
			skb_copy_to_linear_data(skb, llc_oui_pid_pad,
						sizeof(llc_oui_pid_pad));
		} else if (brdev->payload == p_routed) {
			unsigned short prot = ntohs(skb->protocol);

			skb_push(skb, sizeof(llc_oui_ipv4));
			switch (prot) {
			case ETH_P_IP:
				skb_copy_to_linear_data(skb, llc_oui_ipv4,
							sizeof(llc_oui_ipv4));
				break;
			case ETH_P_IPV6:
				skb_copy_to_linear_data(skb, llc_oui_ipv6,
							sizeof(llc_oui_ipv6));
				break;
			default:
				dev_kfree_skb(skb);
				return 0;
			}
		}
	} else { /* e_vc */
		if (brdev->payload == p_bridged) {
			skb_push(skb, 2);
			memset(skb->data, 0, 2);
		} else { /* p_routed */
			skb_pull(skb, ETH_HLEN);
		}
	}
	skb_debug(skb);

	ATM_SKB(skb)->vcc = atmvcc = brvcc->atmvcc;
	pr_debug("atm_skb(%p)->vcc(%p)->dev(%p)\n", skb, atmvcc, atmvcc->dev);
	atomic_add(skb->truesize, &sk_atm(atmvcc)->sk_wmem_alloc);
	ATM_SKB(skb)->atm_options = atmvcc->atm_options;
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;
	atmvcc->send(atmvcc, skb);

	if (!atm_may_send(atmvcc, 0)) {
		netif_stop_queue(brvcc->device);
		/*check for race with br2684_pop*/
		if (atm_may_send(atmvcc, 0))
			netif_start_queue(brvcc->device);
	}

	return 1;
}

static netdev_tx_t br2684_start_xmit(struct sk_buff *skb,
				     struct net_device *dev)
{
	struct br2684_dev *brdev = BRPRIV(dev);
	struct br2684_vcc *brvcc;

	pr_debug("br2684_start_xmit, skb_dst(skb)=%p\n", skb_dst(skb));
	read_lock(&devs_lock);
	brvcc = pick_outgoing_vcc(skb, brdev);
	if (brvcc == NULL) {
		pr_debug("no vcc attached to dev %s\n", dev->name);
		dev->stats.tx_errors++;
		dev->stats.tx_carrier_errors++;
		/* netif_stop_queue(dev); */
		dev_kfree_skb(skb);
		read_unlock(&devs_lock);
		return NETDEV_TX_OK;
	}
	if (!br2684_xmit_vcc(skb, dev, brvcc)) {
		/*
		 * We should probably use netif_*_queue() here, but that
		 * involves added complication.  We need to walk before
		 * we can run.
		 *
		 * Don't free here! this pointer might be no longer valid!
		 */
		dev->stats.tx_errors++;
		dev->stats.tx_fifo_errors++;
	}
	read_unlock(&devs_lock);
	return NETDEV_TX_OK;
}

/*
 * We remember when the MAC gets set, so we don't override it later with
 * the ESI of the ATM card of the first VC
 */
static int br2684_mac_addr(struct net_device *dev, void *p)
{
#ifdef CONFIG_IFX_ATM
	int err = 0;
	struct sockaddr *addr = p;
	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
#else
	int err = eth_mac_addr(dev, p);
#endif
	if (!err)
		BRPRIV(dev)->mac_was_set = 1;
	return err;
}

#ifdef CONFIG_WAN_VLAN_SUPPORT
static int br2684_setvlansettings(struct atm_vcc *atmvcc, unsigned long arg)
{
	struct br2684_vcc *brvcc=NULL;
	struct br2684_filter_set fs;
	struct wan_vlan_struct tmpvlan;
	struct br2684_dev *brdev;
	struct net_device *net_dev;
	struct br2684_dev *brdev1;

	if (copy_from_user(&tmpvlan, (void *)arg, sizeof(struct wan_vlan_struct))) {
		 printk("Error in copy_from_user for vlan struct\n");
		 return -EFAULT;
	}
	//printk("if [%d] untag [%d] tag [%d] vlanvci [%x]\n", tmpvlan.vlan_if_num, tmpvlan.untag_vlan, tmpvlan.tag_vlan_enable, tmpvlan.vlan_vci);
	fs.ifspec.spec.devnum = tmpvlan.vlan_if_num+1;
	//fs.ifspec.method = BR2684_FIND_BYNUM;
	/* Use BR2684_FIND_BYIFNAME with the interface name copied from user space */
    //printk("tnpvlan name = %s\n",tmpvlan.vlan_if_name);
	sprintf(fs.ifspec.spec.ifname, "%s", tmpvlan.vlan_if_name);
	fs.ifspec.spec.ifname[IFNAMSIZ-1] = '\0';
	fs.ifspec.method = BR2684_FIND_BYIFNAME;
	//printk("fs.ifspec.spec.ifname [%s]\n", fs.ifspec.spec.ifname);

	read_lock(&devs_lock);
#if 0
	brdev = br2684_find_dev(&fs.ifspec);
#else
	net_dev = br2684_find_dev(&fs.ifspec);
	if (net_dev == NULL) {
		 //printk(KERN_ERR
		 //	"br2684: tried to attach to non-existant device\n");
		 brdev = NULL;
	}
	 else
	{
		 brdev = BRPRIV(net_dev);
	}
#endif
#if 0
	if (brdev==NULL)
		 printk("brdev NULL!\n");
#endif
	if (brdev == NULL || list_empty(&brdev->brvccs) ||
	    brdev->brvccs.next != brdev->brvccs.prev) {  /* >1 VCC */
		 if (brdev && list_empty(&brdev->brvccs))
			  printk("list empty brdev->brvccs\n");
		 else if ( brdev && brdev->brvccs.next != brdev->brvccs.prev)
			  printk("more than 1 vcc :( \n");
		 brvcc = NULL;
	}
	else {
		 //printk("getting brvcc !!\n");
		 brvcc = list_entry_brvcc(brdev->brvccs.next);
	}
	read_unlock(&devs_lock);

	if (brvcc == NULL) {
#if 0
		 printk("WAN-VLAN configuration : Cannot find brvcc for this atmvcc\n");
#endif
		 return -ESRCH;
	}
	//brvcc->brdev->vlan = kmalloc(sizeof(struct wan_vlan_struct), GFP_KERNEL);
	brdev->vlan.vlan_if_num = tmpvlan.vlan_if_num;
	brdev->vlan.tag_vlan_enable = tmpvlan.tag_vlan_enable;
	brdev->vlan.untag_vlan = tmpvlan.untag_vlan;
	brdev->vlan.vlan_vci = tmpvlan.vlan_vci;

	/* Mark br2684 VLAN settings for easy check */
	if (tmpvlan.tag_vlan_enable /*&& tmpvlan.vlan_vci != 0*/) {
		/* Could be priority tag with vid = 0 as well */
		brdev->net_dev->priv_flags |= IFF_BR2684_VLAN;
	} else {
		brdev->net_dev->priv_flags &= ~IFF_BR2684_VLAN;
	}

	pr_debug("For %s vlan is [%d %d %d %u]\n", brdev->net_dev->name, brdev->vlan.vlan_if_num,
		brdev->vlan.tag_vlan_enable, brdev->vlan.untag_vlan, brdev->vlan.vlan_vci);

	return 0;
}
#endif

#ifdef CONFIG_ATM_BR2684_IPFILTER
/* this IOCTL is experimental. */
static int br2684_setfilt(struct atm_vcc *atmvcc, void __user * arg)
{
	struct br2684_vcc *brvcc;
	struct br2684_filter_set fs;

	if (copy_from_user(&fs, arg, sizeof fs))
		return -EFAULT;
	if (fs.ifspec.method != BR2684_FIND_BYNOTHING) {
		/*
		 * This is really a per-vcc thing, but we can also search
		 * by device.
		 */
		struct br2684_dev *brdev;
		read_lock(&devs_lock);
		brdev = BRPRIV(br2684_find_dev(&fs.ifspec));
		if (brdev == NULL || list_empty(&brdev->brvccs) || brdev->brvccs.next != brdev->brvccs.prev)	/* >1 VCC */
			brvcc = NULL;
		else
			brvcc = list_entry_brvcc(brdev->brvccs.next);
		read_unlock(&devs_lock);
		if (brvcc == NULL)
			return -ESRCH;
	} else
		brvcc = BR2684_VCC(atmvcc);
	memcpy(&brvcc->filter, &fs.filter, sizeof(brvcc->filter));
	return 0;
}

/* Returns 1 if packet should be dropped */
static inline int
packet_fails_filter(__be16 type, struct br2684_vcc *brvcc, struct sk_buff *skb)
{
	if (brvcc->filter.netmask == 0)
		return 0;	/* no filter in place */
	if (type == htons(ETH_P_IP) &&
	    (((struct iphdr *)(skb->data))->daddr & brvcc->filter.
	     netmask) == brvcc->filter.prefix)
		return 0;
	if (type == htons(ETH_P_ARP))
		return 0;
	/*
	 * TODO: we should probably filter ARPs too.. don't want to have
	 * them returning values that don't make sense, or is that ok?
	 */
	return 1;		/* drop */
}
#endif /* CONFIG_ATM_BR2684_IPFILTER */

static void br2684_close_vcc(struct br2684_vcc *brvcc)
{
	pr_debug("removing VCC %p from dev %p\n", brvcc, brvcc->device);
	write_lock_irq(&devs_lock);
	list_del(&brvcc->brvccs);
	write_unlock_irq(&devs_lock);
	brvcc->atmvcc->user_back = NULL;	/* what about vcc->recvq ??? */
	brvcc->old_push(brvcc->atmvcc, NULL);	/* pass on the bad news */
	kfree(brvcc);
	module_put(THIS_MODULE);
}

/* when AAL5 PDU comes in: */
static void br2684_push(struct atm_vcc *atmvcc, struct sk_buff *skb)
{
	struct br2684_vcc *brvcc = BR2684_VCC(atmvcc);
	struct net_device *net_dev = brvcc->device;
	struct br2684_dev *brdev = BRPRIV(net_dev);

	pr_debug("br2684_push\n");

	if (unlikely(skb == NULL)) {
		/* skb==NULL means VCC is being destroyed */
		br2684_close_vcc(brvcc);
		if (list_empty(&brdev->brvccs)) {
			write_lock_irq(&devs_lock);
			list_del(&brdev->br2684_devs);
			write_unlock_irq(&devs_lock);
			unregister_netdev(net_dev);
			free_netdev(net_dev);
		}
		return;
	}

	skb_debug(skb);
	atm_return(atmvcc, skb->truesize);
	pr_debug("skb from brdev %p\n", brdev);
	if (brvcc->encaps == e_llc) {

		if (skb->len > 7 && skb->data[7] == 0x01)
			__skb_trim(skb, skb->len - 4);

		/* accept packets that have "ipv[46]" in the snap header */
		if ((skb->len >= (sizeof(llc_oui_ipv4)))
		    &&
		    (memcmp
		     (skb->data, llc_oui_ipv4,
		      sizeof(llc_oui_ipv4) - BR2684_ETHERTYPE_LEN) == 0)) {
			if (memcmp
			    (skb->data + 6, ethertype_ipv6,
			     sizeof(ethertype_ipv6)) == 0)
				skb->protocol = htons(ETH_P_IPV6);
			else if (memcmp
				 (skb->data + 6, ethertype_ipv4,
				  sizeof(ethertype_ipv4)) == 0)
				skb->protocol = htons(ETH_P_IP);
			else
				goto error;
			skb_pull(skb, sizeof(llc_oui_ipv4));
			skb_reset_network_header(skb);
			skb->pkt_type = PACKET_HOST;
			/*
			 * Let us waste some time for checking the encapsulation.
			 * Note, that only 7 char is checked so frames with a valid FCS
			 * are also accepted (but FCS is not checked of course).
			 */
		} else if ((skb->len >= sizeof(llc_oui_pid_pad)) &&
			   (memcmp(skb->data, llc_oui_pid_pad, 7) == 0)) {
			skb_pull(skb, sizeof(llc_oui_pid_pad));
			skb->protocol = eth_type_trans(skb, net_dev);
		} else
			goto error;

	} else { /* e_vc */
		if (brdev->payload == p_routed) {
			struct iphdr *iph;

			skb_reset_network_header(skb);
			iph = ip_hdr(skb);
			if (iph->version == 4)
				skb->protocol = htons(ETH_P_IP);
			else if (iph->version == 6)
				skb->protocol = htons(ETH_P_IPV6);
			else
				goto error;
			skb->pkt_type = PACKET_HOST;
		} else { /* p_bridged */
			/* first 2 chars should be 0 */
			if (*((u16 *) (skb->data)) != 0)
				goto error;
			skb_pull(skb, BR2684_PAD_LEN);
			skb->protocol = eth_type_trans(skb, net_dev);
		}
	}

#ifdef CONFIG_ATM_BR2684_IPFILTER
	if (unlikely(packet_fails_filter(skb->protocol, brvcc, skb)))
		goto dropped;
#endif /* CONFIG_ATM_BR2684_IPFILTER */
	skb->dev = net_dev;
	ATM_SKB(skb)->vcc = atmvcc;	/* needed ? */
	pr_debug("received packet's protocol: %x\n", ntohs(skb->protocol));
	skb_debug(skb);
/* Adopted from 2.4 BSP */
	if (!(net_dev->flags & IFF_MULTICAST) && (skb->pkt_type == PACKET_MULTICAST))
	{ /* drop multicast packets */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
		brdev->stats.rx_dropped++;
	#else
		struct net_device *dev = skb->dev;
		dev->stats.rx_dropped++;
	#endif
		dev_kfree_skb(skb);
		return;
	}
/*165001*/
	/* sigh, interface is down? */
	if (unlikely(!(net_dev->flags & IFF_UP)))
		goto dropped;
	net_dev->stats.rx_packets++;
	net_dev->stats.rx_bytes += skb->len;
	memset(ATM_SKB(skb), 0, sizeof(struct atm_skb_data));
#ifdef CONFIG_WAN_VLAN_SUPPORT
	if(g_br2684_tag_vlan_enable) {
		br2684_remove_vlan_tag_rcv(skb, brdev, 0);
	}
#endif
	netif_rx(skb);
	return;

dropped:
	net_dev->stats.rx_dropped++;
	goto free_skb;
error:
	net_dev->stats.rx_errors++;
free_skb:
	dev_kfree_skb(skb);
	return;
}

extern void (*ppa_hook_mpoa_setup)(struct atm_vcc *, int, int);
/*
 * Assign a vcc to a dev
 * Note: we do not have explicit unassign, but look at _push()
 */
static int br2684_regvcc(struct atm_vcc *atmvcc, void __user * arg)
{
	struct sk_buff_head queue;
	int err;
	struct br2684_vcc *brvcc;
	struct sk_buff *skb, *tmp;
	struct sk_buff_head *rq;
	struct br2684_dev *brdev;
	struct net_device *net_dev;
	struct atm_backend_br2684 be;
	unsigned long flags;

	if (copy_from_user(&be, arg, sizeof be))
		return -EFAULT;
	brvcc = kzalloc(sizeof(struct br2684_vcc), GFP_KERNEL);
	if (!brvcc)
		return -ENOMEM;
	write_lock_irq(&devs_lock);
	net_dev = br2684_find_dev(&be.ifspec);
	if (net_dev == NULL) {
		printk(KERN_ERR
		       "br2684: tried to attach to non-existant device\n");
		err = -ENXIO;
		goto error;
	}
	brdev = BRPRIV(net_dev);
	if (atmvcc->push == NULL) {
		err = -EBADFD;
		goto error;
	}
	if (!list_empty(&brdev->brvccs)) {
		/* Only 1 VCC/dev right now */
		err = -EEXIST;
		goto error;
	}
	if (be.fcs_in != BR2684_FCSIN_NO || be.fcs_out != BR2684_FCSOUT_NO ||
	    be.fcs_auto || be.has_vpiid || be.send_padding || (be.encaps !=
							       BR2684_ENCAPS_VC
							       && be.encaps !=
							       BR2684_ENCAPS_LLC)
	    || be.min_size != 0) {
		err = -EINVAL;
		goto error;
	}
	pr_debug("br2684_regvcc vcc=%p, encaps=%d, brvcc=%p\n", atmvcc,
		 be.encaps, brvcc);
	if (list_empty(&brdev->brvccs) && !brdev->mac_was_set) {
		unsigned char *esi = atmvcc->dev->esi;
		if (esi[0] | esi[1] | esi[2] | esi[3] | esi[4] | esi[5])
			memcpy(net_dev->dev_addr, esi, net_dev->addr_len);
		else
			net_dev->dev_addr[2] = 1;
	}
	list_add(&brvcc->brvccs, &brdev->brvccs);
	write_unlock_irq(&devs_lock);
	brvcc->device = net_dev;
	brvcc->atmvcc = atmvcc;
	atmvcc->user_back = brvcc;
	brvcc->encaps = (enum br2684_encaps)be.encaps;
	brvcc->old_push = atmvcc->push;
	brvcc->old_pop = atmvcc->pop;
	barrier();
	atmvcc->push = br2684_push;
	atmvcc->pop = br2684_pop;
    if ( ppa_hook_mpoa_setup )
		ppa_hook_mpoa_setup(atmvcc, brdev->payload == p_routed ? 3 : 0, brvcc->encaps == BR2684_ENCAPS_LLC ? 1 : 0);     //  IPoA or EoA w/o FCS

	__skb_queue_head_init(&queue);
	rq = &sk_atm(atmvcc)->sk_receive_queue;

	spin_lock_irqsave(&rq->lock, flags);
	skb_queue_splice_init(rq, &queue);
	spin_unlock_irqrestore(&rq->lock, flags);

	skb_queue_walk_safe(&queue, skb, tmp) {
		struct net_device *dev = skb->dev;

		if(dev != NULL)
		{
			dev->stats.rx_bytes -= skb->len;
			dev->stats.rx_packets--;
		}
		br2684_push(atmvcc, skb);
	}
	__module_get(THIS_MODULE);
	return 0;
      error:
	write_unlock_irq(&devs_lock);
	kfree(brvcc);
	return err;
}

static const struct net_device_ops br2684_netdev_ops = {
	.ndo_start_xmit 	= br2684_start_xmit,
	.ndo_set_mac_address	= br2684_mac_addr,
	.ndo_change_mtu		= eth_change_mtu,
	.ndo_validate_addr	= eth_validate_addr,
};

static const struct net_device_ops br2684_netdev_ops_routed = {
	.ndo_start_xmit 	= br2684_start_xmit,
	.ndo_set_mac_address	= br2684_mac_addr,
	.ndo_change_mtu		= eth_change_mtu
};

static int br2684_unregvcc(struct atm_vcc *atmvcc, void __user *arg)
{
	int err;
	struct br2684_vcc *brvcc;
	struct br2684_dev *brdev;
	struct net_device *net_dev;
	struct atm_backend_br2684 be;

	if (copy_from_user(&be, arg, sizeof be))
		return -EFAULT;
	write_lock_irq(&devs_lock);
	net_dev = br2684_find_dev(&be.ifspec);
	if (net_dev == NULL) {
		printk(KERN_ERR
			"br2684: tried to unregister to non-existant device\n");
		err = -ENXIO;
		goto error;
	}
	brdev = BRPRIV(net_dev);
	while (!list_empty(&brdev->brvccs)) {
		brvcc = list_entry_brvcc(brdev->brvccs.next);
		br2684_close_vcc(brvcc);
	}
	list_del(&brdev->br2684_devs);
	write_unlock_irq(&devs_lock);
	unregister_netdev(net_dev);
	free_netdev(net_dev);
	atmvcc->push = NULL;
	vcc_release_async(atmvcc, -ETIMEDOUT);
	return 0;
error:
	write_unlock_irq(&devs_lock);
	return err;
}

static void br2684_setup(struct net_device *netdev)
{
	struct br2684_dev *brdev = BRPRIV(netdev);

	ether_setup(netdev);
	brdev->net_dev = netdev;

	netdev->netdev_ops = &br2684_netdev_ops;

	INIT_LIST_HEAD(&brdev->brvccs);
}

static void br2684_setup_routed(struct net_device *netdev)
{
	struct br2684_dev *brdev = BRPRIV(netdev);

	brdev->net_dev = netdev;
	netdev->hard_header_len = 0;
	netdev->netdev_ops = &br2684_netdev_ops_routed;
	netdev->addr_len = 0;
	netdev->mtu = 1500;
	netdev->type = ARPHRD_PPP;
	netdev->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	netdev->tx_queue_len = 100;
	INIT_LIST_HEAD(&brdev->brvccs);
}

static int br2684_create(void __user * arg)
{
	int err;
	struct net_device *netdev;
	struct br2684_dev *brdev;
	struct atm_newif_br2684 ni;
	enum br2684_payload payload;

	pr_debug("br2684_create\n");

	if (copy_from_user(&ni, arg, sizeof ni)) {
		return -EFAULT;
	}

	if (ni.media & BR2684_FLAG_ROUTED)
		payload = p_routed;
	else
		payload = p_bridged;
	ni.media &= 0xffff;	/* strip flags */

	if (ni.media != BR2684_MEDIA_ETHERNET || ni.mtu != 1500) {
		return -EINVAL;
	}

	netdev = alloc_netdev(sizeof(struct br2684_dev),
			      ni.ifname[0] ? ni.ifname : "nas%d",
			      (payload == p_routed) ?
			      br2684_setup_routed : br2684_setup);
	if (!netdev)
		return -ENOMEM;

	brdev = BRPRIV(netdev);

	pr_debug("registered netdev %s\n", netdev->name);
	/* open, stop, do_ioctl ? */
	err = register_netdev(netdev);
	if (err < 0) {
		printk(KERN_ERR "br2684_create: register_netdev failed\n");
		free_netdev(netdev);
		return err;
	}
	/* Mark br2684 device */
	netdev->priv_flags |= IFF_BR2684;

	write_lock_irq(&devs_lock);
	brdev->payload = payload;
	brdev->number = list_empty(&br2684_devs) ? 1 :
	    BRPRIV(list_entry_brdev(br2684_devs.prev))->number + 1;
	list_add_tail(&brdev->br2684_devs, &br2684_devs);
	write_unlock_irq(&devs_lock);
	return 0;
}

/*
 * This handles ioctls actually performed on our vcc - we must return
 * -ENOIOCTLCMD for any unrecognized ioctl
 */
static int br2684_ioctl(struct socket *sock, unsigned int cmd,
			unsigned long arg)
{
	struct atm_vcc *atmvcc = ATM_SD(sock);
	void __user *argp = (void __user *)arg;
	atm_backend_t b;

	int err;
	switch (cmd) {
	case ATM_SETBACKEND:
	case ATM_NEWBACKENDIF:
	case ATM_DELBACKENDIF:
		err = get_user(b, (atm_backend_t __user *) argp);
		if (err)
			return -EFAULT;
		if (b != ATM_BACKEND_BR2684)
			return -ENOIOCTLCMD;
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		if (cmd == ATM_SETBACKEND)
			return br2684_regvcc(atmvcc, argp);
		else if (cmd == ATM_DELBACKENDIF)
			return br2684_unregvcc(atmvcc, argp);
		else
			return br2684_create(argp);
#ifdef CONFIG_ATM_BR2684_IPFILTER
	case BR2684_SETFILT:
		if (atmvcc->push != br2684_push)
			return -ENOIOCTLCMD;
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		err = br2684_setfilt(atmvcc, argp);

		return err;
#endif /* CONFIG_ATM_BR2684_IPFILTER */
#ifdef CONFIG_WAN_VLAN_SUPPORT
	case ATM_BR2684_VLAN_CONFIG:
	{
		int opt=0;
		//MOD_INC_USE_COUNT;
		__module_get(THIS_MODULE);
		err = get_user(opt, (int *) arg);
		if (err)
			return -EFAULT;
		if(opt==1) {
			g_br2684_tag_vlan_enable = 1;
		} else {
			g_br2684_tag_vlan_enable = 0;
		}
		//MOD_DEC_USE_COUNT;
		module_put(THIS_MODULE);
		return err;
	}
	case ATM_BR2684_VLAN_VCC_CONFIG:
		//MOD_INC_USE_COUNT;
		__module_get(THIS_MODULE);
		err = br2684_setvlansettings(atmvcc, arg);
		//MOD_DEC_USE_COUNT;
		module_put(THIS_MODULE);
		return err;
#endif
	}
	return -ENOIOCTLCMD;
}

static struct atm_ioctl br2684_ioctl_ops = {
	.owner = THIS_MODULE,
	.ioctl = br2684_ioctl,
};

#ifdef CONFIG_PROC_FS
static void *br2684_seq_start(struct seq_file *seq, loff_t * pos)
	__acquires(devs_lock)
{
	read_lock(&devs_lock);
	return seq_list_start(&br2684_devs, *pos);
}

static void *br2684_seq_next(struct seq_file *seq, void *v, loff_t * pos)
{
	return seq_list_next(v, &br2684_devs, pos);
}

static void br2684_seq_stop(struct seq_file *seq, void *v)
	__releases(devs_lock)
{
	read_unlock(&devs_lock);
}

static int br2684_seq_show(struct seq_file *seq, void *v)
{
	const struct br2684_dev *brdev = list_entry(v, struct br2684_dev,
						    br2684_devs);
	const struct net_device *net_dev = brdev->net_dev;
	const struct br2684_vcc *brvcc;

	seq_printf(seq, "dev %.16s: num=%d, mac=%pM (%s)\n",
		   net_dev->name,
		   brdev->number,
		   net_dev->dev_addr,
		   brdev->mac_was_set ? "set" : "auto");

	list_for_each_entry(brvcc, &brdev->brvccs, brvccs) {
		seq_printf(seq, "  vcc %d.%d.%d: encaps=%s payload=%s"
			   ", failed copies %u/%u"
			   "\n", brvcc->atmvcc->dev->number,
			   brvcc->atmvcc->vpi, brvcc->atmvcc->vci,
			   (brvcc->encaps == e_llc) ? "LLC" : "VC",
			   (brdev->payload == p_bridged) ? "bridged" : "routed",
			   brvcc->copies_failed, brvcc->copies_needed);
#ifdef CONFIG_ATM_BR2684_IPFILTER
#define b1(var, byte)	((u8 *) &brvcc->filter.var)[byte]
#define bs(var)		b1(var, 0), b1(var, 1), b1(var, 2), b1(var, 3)
		if (brvcc->filter.netmask != 0)
			seq_printf(seq, "    filter=%d.%d.%d.%d/"
				   "%d.%d.%d.%d\n", bs(prefix), bs(netmask));
#undef bs
#undef b1
#endif /* CONFIG_ATM_BR2684_IPFILTER */
	}
	return 0;
}

static const struct seq_operations br2684_seq_ops = {
	.start = br2684_seq_start,
	.next = br2684_seq_next,
	.stop = br2684_seq_stop,
	.show = br2684_seq_show,
};

static int br2684_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &br2684_seq_ops);
}

static const struct file_operations br2684_proc_ops = {
	.owner = THIS_MODULE,
	.open = br2684_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

extern struct proc_dir_entry *atm_proc_root;	/* from proc.c */
#endif /* CONFIG_PROC_FS */

static int __init br2684_init(void)
{
#ifdef CONFIG_PROC_FS
	struct proc_dir_entry *p;
	p = proc_create("br2684", 0, atm_proc_root, &br2684_proc_ops);
	if (p == NULL)
		return -ENOMEM;
#endif
	register_atm_ioctl(&br2684_ioctl_ops);
	return 0;
}

static void __exit br2684_exit(void)
{
	struct net_device *net_dev;
	struct br2684_dev *brdev;
	struct br2684_vcc *brvcc;
	deregister_atm_ioctl(&br2684_ioctl_ops);

#ifdef CONFIG_PROC_FS
	remove_proc_entry("br2684", atm_proc_root);
#endif

	while (!list_empty(&br2684_devs)) {
		net_dev = list_entry_brdev(br2684_devs.next);
		brdev = BRPRIV(net_dev);
		while (!list_empty(&brdev->brvccs)) {
			brvcc = list_entry_brvcc(brdev->brvccs.next);
			br2684_close_vcc(brvcc);
		}

		list_del(&brdev->br2684_devs);
		unregister_netdev(net_dev);
		free_netdev(net_dev);
	}
}

#if defined(CONFIG_IFX_PPA_API) || defined(CONFIG_IFX_PPA_API_MODULE)
int ppa_br2684_get_vcc(struct net_device *netdev, struct atm_vcc **pvcc)
{
    if ( netdev && (uint32_t)br2684_start_xmit == (uint32_t)netdev->netdev_ops->ndo_start_xmit)
    {
        struct br2684_dev *brdev;
        struct br2684_vcc *brvcc;


        brdev = (struct br2684_dev *)BRPRIV(netdev);
        brvcc = list_empty(&brdev->brvccs) ? NULL : list_entry(brdev->brvccs.next, struct br2684_vcc, brvccs);

        if ( brvcc )
        {
            *pvcc = brvcc->atmvcc;
            return 0;
        }
    }

    return -1;
}

int32_t ppa_if_is_br2684(struct net_device *netdev, char *ifname)
{
    if ( !netdev )
    {
        netdev = dev_get_by_name(&init_net,ifname);
        if ( !netdev )
            return 0;   //  can not get
        else
            dev_put(netdev);
    }

    return (uint32_t)br2684_start_xmit == (uint32_t)netdev->netdev_ops->ndo_start_xmit ? 1 : 0;
}

int32_t ppa_if_is_ipoa(struct net_device *netdev, char *ifname)
{
    if ( !netdev )
    {
        netdev = dev_get_by_name(&init_net,ifname);
        if ( !netdev )
            return 0;
        else
            dev_put(netdev);
    }

    if ( ppa_if_is_br2684(netdev, ifname) )
    {
        struct br2684_dev *brdev;
        //struct br2684_vcc *brvcc;

//  #if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
        //brdev = (struct br2684_dev *)netif->priv;
        brdev = BRPRIV(netdev);
//  #else
//        brdev = (struct br2684_dev *)((char *)(netif) - (unsigned long)(&((struct br2684_dev *)0)->net_dev));
//  #endif
        //brvcc = list_empty(&brdev->brvccs) ? NULL : list_entry(brdev->brvccs.next, struct br2684_vcc, brvccs);

        //return brvcc && brvcc->payload == p_routed ? 1 : 0;
        return brdev && brdev->payload == p_routed ? 1 : 0;
    }

    return 0;
}
#endif

#ifdef CONFIG_WAN_VLAN_SUPPORT
int br2684_vlan_dev_get_vid(struct net_device *dev, uint16_t *vid)
{
	int ret=0;
	struct br2684_dev *brdev;

	if (!dev || !vid)
		return -EINVAL;

	dev_hold(dev);
	brdev = BRPRIV(dev);

	if (brdev->vlan.tag_vlan_enable) {
		*vid = brdev->vlan.vlan_vci;
	} else {
		ret=-EINVAL;
	}

	pr_debug("(%s) Returning VLAN Id [%d]; VLAN enable [%d] for [%s]\n",
		__func__, *vid, brdev->vlan.tag_vlan_enable, dev->name);

	dev_put(dev);
	return ret;
}
EXPORT_SYMBOL(br2684_vlan_dev_get_vid);
#endif

module_init(br2684_init);
module_exit(br2684_exit);

MODULE_AUTHOR("Marcell GAL");
MODULE_DESCRIPTION("RFC2684 bridged protocols over ATM/AAL5");
MODULE_LICENSE("GPL");
#if defined(CONFIG_IFX_PPA_API_MODULE)
  EXPORT_SYMBOL(ppa_if_is_ipoa);
  EXPORT_SYMBOL(ppa_if_is_br2684);
  EXPORT_SYMBOL(ppa_br2684_get_vcc);
#endif
