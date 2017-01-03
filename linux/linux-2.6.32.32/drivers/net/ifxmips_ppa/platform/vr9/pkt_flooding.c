#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/etherdevice.h>  /*  eth_type_trans  */
#include <linux/ethtool.h>      /*  ethtool_cmd     */
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <net/xfrm.h>
#include <linux/tcp.h>

#include <linux/if_vlan.h>

#define DEBUG_DROP_FLOODING_PACKET 0 // 1: debug message; 2: debug message and packet content

#define FLOODING_FRAGMENT_TYPE 0

#define FLOODING_TIME_INTERVAL (10 * HZ) // 10 seconds
#define ONE_SECOND_INTERVAL (1 * HZ) // 1 second

#define FLOODING_FRAGMENT_DEFAULT_MAX_PACKET_NUMBER_IN_ONE_SECOND 8000 // packet number
#define FLOODING_FRAGMENT_DEFAULT_ALLOW_PASS_NUM 2

#define PKT_FLOODING_ENTRY_MAX 16

typedef struct pkt_flooding_t_
{
	unsigned char bUsed;
	unsigned char type; // 0: Fragment; 6: TCP; 17: UDP => use protocol number as type
	unsigned char isIPv6; // 0: IPv4; 1: IPv6
    unsigned int  src_ip; // IPv4 address
    unsigned int  ipv6_src_ip[16]; // IPv6 address
    unsigned short dest_port;
    unsigned long fragment_pass_count;
    unsigned long hit_time;
    unsigned long filter_count;
} pkt_flooding_t;

unsigned char gStopFloodingPacket = 1;
unsigned char gEnableDbg = 0;
unsigned char gFilterRuleCount = 0;
unsigned char gFragmentAllowPassNum = FLOODING_FRAGMENT_DEFAULT_ALLOW_PASS_NUM; // pass FLOODING_FRAGMENT_DEFAULT_ALLOW_PASS_NUM, then block one fragment packet
unsigned long gFragmentMaxPacketNumInOneSecond = FLOODING_FRAGMENT_DEFAULT_MAX_PACKET_NUMBER_IN_ONE_SECOND; // If the total fragment packet count is bigger than this gFragmentMaxPacketNum value, we must start to stop fragment packets.
unsigned long gFragment_packet_count_in_one_second = 0;
unsigned long gFragment_hit_time = 0, gFragment_hit_timeout = 0;
unsigned char gEnableFragmentFilter = 0 ; // Because fragment rule only has one entry, we use this variable to ignore lookup function.
signed char gFragmentFilterIndex = -1 ;

static pkt_flooding_t g_pkt_flooding_tbl[PKT_FLOODING_ENTRY_MAX] = {0};

static int pkt_flooding_init()
{
	memset( g_pkt_flooding_tbl, 0, sizeof(g_pkt_flooding_tbl) );
	gFilterRuleCount = 0;

	return 0; // ok
}

static int pkt_flooding_lookup(unsigned char isIPv6, unsigned char type, unsigned int src_ip, unsigned char *ipv6_src_ip, unsigned short dest_port, int *free_idx)
{
	int i = 0, found_idx = -1;
	pkt_flooding_t *entry = NULL;
	unsigned char zero_ipv6_addr[16] = {0};

	if( free_idx ) *free_idx = -1;
	for( i=0 ; i < PKT_FLOODING_ENTRY_MAX ; i++ ){
		entry = &g_pkt_flooding_tbl[i];

		if(entry->bUsed == 0)
		{
			if( free_idx && *free_idx == -1 ) *free_idx = i;
			continue;
		}

		if( (found_idx == -1) && (entry->type == type) && (entry->isIPv6 == isIPv6) )
		{
			switch(type)
			{
				case FLOODING_FRAGMENT_TYPE: // Fragment
					if(isIPv6 == 0)
					{	// IPv4
						if(src_ip != 0)
						{	// need to check source ip address
							if(entry->src_ip != src_ip) continue;
							if(entry->dest_port != dest_port) continue;
						}
					}
					else
					{	// IPv6
						if(memcmp(entry->ipv6_src_ip, zero_ipv6_addr, 16) != 0)
						{
							if(memcmp(entry->ipv6_src_ip, ipv6_src_ip, 16) != 0) continue;
							if(entry->dest_port != dest_port) continue;
						}
					}
				break;

				case IPPROTO_TCP: // TCP
				case IPPROTO_UDP: // UDP
					if(isIPv6 == 0)
					{	// IPv4
						if(src_ip == 0) continue;

						if(entry->src_ip != src_ip) continue;
						if(entry->dest_port != dest_port) continue;
					}
					else
					{	// IPv6
						if(memcmp(entry->ipv6_src_ip, zero_ipv6_addr, 16) == 0) continue;
						if(memcmp(entry->ipv6_src_ip, ipv6_src_ip, 16) != 0) continue;
						if(entry->dest_port != dest_port) continue;
					}
				break;
			}
			found_idx = i;
			if(free_idx==NULL) return found_idx;
		}
	}

	return found_idx;
}

static int pkt_flooding_add(unsigned char isIPv6, unsigned char type, unsigned int src_ip, unsigned char *ipv6_src_ip, unsigned short dest_port)
{
	int idx, free_idx ;
	pkt_flooding_t *entry ;

	idx = pkt_flooding_lookup(isIPv6, type, src_ip, ipv6_src_ip, dest_port, &free_idx );
	if( idx >= 0 )
	{
		printk("[pkt_flooding_add] find an existed entry %d\n", idx);
		return idx;
	}
	if( free_idx == -1 )
	{
		printk("[pkt_flooding_add] no free entry\n");
		return -1; // no free
	}

	printk("[pkt_flooding_add] add index %d, jiffies=%lu\n", free_idx, jiffies);
	entry = &g_pkt_flooding_tbl[free_idx];
	memset( entry, 0, sizeof(pkt_flooding_t));
	entry->bUsed = 1;
	entry->type = type;
	entry->isIPv6 = isIPv6;
	entry->src_ip = src_ip;
	memcpy(entry->ipv6_src_ip, ipv6_src_ip, 16);
	entry->dest_port = dest_port;
	entry->hit_time = jiffies;

	if(entry->type == FLOODING_FRAGMENT_TYPE)
	{
		gEnableFragmentFilter = 1;
		gFragmentFilterIndex = free_idx;
	}

	gFilterRuleCount += 1;

	return free_idx;
}

static int pkt_flooding_remove( unsigned char index )
{
	pkt_flooding_t *entry = NULL;

	if( (index < 0) || (index >= PKT_FLOODING_ENTRY_MAX) )
	{
		printk("[pkt_flooding_remove] error index:%d\n", index);
		return 0;
	}

	printk("[pkt_flooding_remove] remove index %d, jiffies=%lu\n", index, jiffies);
	entry = &g_pkt_flooding_tbl[index];

	if(entry->type == FLOODING_FRAGMENT_TYPE)
	{
		gEnableFragmentFilter = 0;
		gFragmentFilterIndex = -1;
	}

	memset( entry, 0, sizeof(pkt_flooding_t));

	gFilterRuleCount -= 1;

	return 1; // removed
}

/*
example:
echo "add IPv4 Fragment" > /proc/eth/stop_flooding_packet
echo "add IPv4 TCP 192.168.2.200:0" > /proc/eth/stop_flooding_packet
echo "add IPv4 UDP 192.168.2.200:0" > /proc/eth/stop_flooding_packet
echo "add IPv4 TCP 192.168.2.100:0" > /proc/eth/stop_flooding_packet
echo "add IPv4 UDP 192.168.2.100:0" > /proc/eth/stop_flooding_packet

echo "del IPv4 Fragment" > /proc/eth/stop_flooding_packet
echo "del IPv4 TCP 192.168.2.200:0" > /proc/eth/stop_flooding_packet
echo "del IPv4 UDP 192.168.2.200:0" > /proc/eth/stop_flooding_packet

echo "allow-fragment-pass-num <allow number>" > /proc/eth/stop_flooding_packet
*/
static void proc_write_stop_flooding_packet_help( void )
{
	printk(	"Packet flooding filter:\n"
			"    help									- show this help message\n"
			"    reset									- clear counters\n"
			"    enable									- enable packet flooding filter function\n"
			"    disable								- disable packet flooding filter function\n"
			"    debug-enable							- enable debug function\n"
			"    debug-disable							- disable debug function\n"
			"    add IPv4 Fragment						- add IPv4 Fragment filter function\n"
			"    del IPv4 Fragment						- remove IPv4 Fragment filter function\n"
			"    add IPv4 TCP <source ip address>:0		- add IPv4 TCP flooding function to the specified source address\n"
			"    del IPv4 TCP <source ip address>:0		- remove IPv4 TCP flooding function to the specified source address\n"
			"    add IPv4 UDP <source ip address>:0		- add IPv4 UDP flooding function to the specified source address\n"
			"    del IPv4 UDP <source ip address>:0		- remove IPv4 UDP flooding function to the specified source address\n"
			"    allow-fragment-pass-num <pass num>		- fragment allow pass number\n"
			"    max-fragment-packet-num <pass num>		- fragment max packet number\n"
		);
}

static void proc_write_stop_flooding_packet_reset( void )
{
	pkt_flooding_init();
}

/*
typedef struct pkt_flooding_t_
{
	unsigned char bUsed;
	unsigned char type; // 0: Fragment; 6: TCP; 17: UDP
	unsigned char isIPv6; // 0: IPv4; 1: IPv6
    unsigned int  src_ip; // IPv4 address
    unsigned int  ipv6_src_ip[16]; // IPv6 address
    unsigned short dest_port;
} pkt_flooding_t;
*/
static int proc_read_stop_flooding_packet(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
	pkt_flooding_t *entry ;
    int i;

    len += sprintf(page + len, "Stop Fragment Packet: %s\n", gStopFloodingPacket ? "enable" : "disable");
    len += sprintf(page + len, "Debug mode: %s\n", gEnableDbg ? "enable" : "disable");
    len += sprintf(page + len, "Fragment Allow Pass Number: %d\n", gFragmentAllowPassNum);
    len += sprintf(page + len, "Fragment Allow Max Packet Number In One Second: %d\n", gFragmentMaxPacketNumInOneSecond);
    len += sprintf(page + len, "Fragment Packet Number In One Second: %d\n", gFragment_packet_count_in_one_second);
    len += sprintf(page + len, "Fragment hit time: 0x%x, Fragment hit timeout: 0x%x\n", gFragment_hit_time, gFragment_hit_timeout);
    len += sprintf(page + len, "Rule Count: %d\n", gFilterRuleCount);
    len += sprintf(page + len, "gEnableFragmentFilter: %d, gFragmentFilterIndex: %d\n", gEnableFragmentFilter, gFragmentFilterIndex);

	for(i=0; i<PKT_FLOODING_ENTRY_MAX; i++)
	{
		entry = &g_pkt_flooding_tbl[i];
		if(entry->bUsed == 0) continue;

    	len += sprintf(page + len, "index: %d\n", i);
    	len += sprintf(page + len, "	type: %d\n", entry->type);
    	len += sprintf(page + len, "	isIPv6: %d\n", entry->isIPv6);
    	if(entry->isIPv6 == 0)
    	{	// IPv4
    		len += sprintf(page + len, "	src_ip: 0x%x\n", ntohl(entry->src_ip));
    	}
    	else
    	{	// IPv6
    	}
    	len += sprintf(page + len, "	dest_port: %d\n", ntohs(entry->dest_port));
    	len += sprintf(page + len, "	filter count: %lu\n", entry->filter_count);
    	if(entry->type == FLOODING_FRAGMENT_TYPE)
    	{
    		len += sprintf(page + len, "	fragment pass count: %lu\n", entry->fragment_pass_count);
    	}
    	len += sprintf(page + len, "	hit_time: %lu\n", entry->hit_time);
	}

    *eof = 1;

    return len;
}

#if 0
static INLINE void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}

static INLINE int get_number(char **p, int *len, int is_hex)
{
    int ret = 0;
    int n = 0;

    if ( (*p)[0] == '0' && (*p)[1] == 'x' )
    {
        is_hex = 1;
        (*p) += 2;
        (*len) -= 2;
    }

    if ( is_hex )
    {
        while ( *len && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F')) )
        {
            if ( **p >= '0' && **p <= '9' )
                n = **p - '0';
            else if ( **p >= 'a' && **p <= 'f' )
               n = **p - 'a' + 10;
            else if ( **p >= 'A' && **p <= 'F' )
                n = **p - 'A' + 10;
            ret = (ret << 4) | n;
            (*p)++;
            (*len)--;
        }
    }
    else
    {
        while ( *len && **p >= '0' && **p <= '9' )
        {
            n = **p - '0';
            ret = ret * 10 + n;
            (*p)++;
            (*len)--;
        }
    }

    return ret;
}

static INLINE void get_ip_port(char **p, int *len, unsigned int *val)
{
    int i;
    unsigned int tmp_val[17] = {0};
    int is_ipv6 = 0;

    memset(val, 0, sizeof(*val) * 6);

    for ( i = 0; i < 4; i++ )
    {
        ignore_space(p, len);
        if ( !*len )
            break;
        val[i] = get_number(p, len, 0);
    }

    if ( **p == '.' )
    {
        is_ipv6 = 1;
        for ( i = 0; i < 16 - 4; i++ )
        {
            ignore_space(p, len);
            if ( !*len )
                break;
            tmp_val[i] = get_number(p, len, 0);
        }
    }

    ignore_space(p, len);
    if ( *len )
        val[4] = get_number(p, len, 0);

    if ( is_ipv6 )
    {
        val[5] = 6;
        for ( i = 0; i < 16 - 4; i++ )
            val[i + 6] = tmp_val[i];
    }
    else
        val[5] = 4;
}
#endif

/*
Flooding rule input:
Enable: enable
Disable: disable
Reset: reset
Add/delete rule to drop flooding Fragment/TCP/UDP packets: add/del IPv4/IPv6 Fragment/TCP/UDP <source ip> <source port>
Timer check: User application is responsible for timer check.
*/
static int proc_write_stop_flooding_packet( struct file* file, const char* buffer, unsigned long count, void* data )
{
	char			*pPtr, *pTmpPtr;
	char			sBuf[128];
	unsigned char	proto;
	unsigned short  dest_port = 0;
	int				tmp, en, cnt;
	int 			operation = 0; // 0: add; 1: del
	int				proto_family = 0; // 0: IPv4; 1: IPv6
	int				packet_type = 0; // 0: Fragment; 6: TCP; 17: UDP
	int				len, rlen;
    char 			*p;
    unsigned int 	val[20];
    char 			*p1, *p2;
    int 			is_ipv6 = 0;
    unsigned int    src_ip = 0;;
    unsigned int	ipv6_src_ip[4] = {0};
    unsigned char	ipv6_addr[16] = {0};
    int				find_idx = -1;
    int 			i;
	pkt_flooding_t *entry = NULL;

	printk("[proc_write_stop_flooding_packet] start\n");

	memset(sBuf, 0, sizeof(sBuf));

	/* trim the tailing space, tab and LF/CR*/
	if ( count > 0 )
	{
		if (count >= sizeof(sBuf))
			count = sizeof(sBuf) - 1;

		if (copy_from_user(sBuf, buffer, count))
			goto proc_write_stop_flooding_packet_exit;

		pPtr = (char*)sBuf + count - 1;
		len = count;
		for (; *pPtr==' ' || *pPtr=='\t' || *pPtr=='\n' || *pPtr=='\r'; pPtr++)
		{
			*pPtr = '\0';
		}
	}
	else
	{
		proc_write_stop_flooding_packet_help();
		goto proc_write_stop_flooding_packet_exit;
	}

	/* reset */
	if ( strnicmp( sBuf, "reset", strlen("reset") ) == 0 )
	{
		printk("	process reset, len=%d\n", len);

		len -= strlen("reset");
		proc_write_stop_flooding_packet_reset();
		goto proc_write_stop_flooding_packet_exit;
	}

	/* enable */
	if ( strnicmp( sBuf, "enable", strlen("enable") ) == 0 )
	{
		printk("	process enable, len=%d\n", len);

		len -= strlen("enable");
		gStopFloodingPacket = 1;
		goto proc_write_stop_flooding_packet_exit;
	}

	/* disable */
	if ( strnicmp( sBuf, "disable", strlen("disable") ) == 0 )
	{
		printk("	process disable, len=%d\n", len);

		len -= strlen("disable");
		gStopFloodingPacket = 0;
		goto proc_write_stop_flooding_packet_exit;
	}

	/* debug-enable */
	if ( strnicmp( sBuf, "debug-enable", strlen("debug-enable") ) == 0 )
	{
		printk("	process debug-enable, len=%d\n", len);

		len -= strlen("debug-enable");
		gEnableDbg = 1;
		goto proc_write_stop_flooding_packet_exit;
	}

	/* debug-disable */
	if ( strnicmp( sBuf, "debug-disable", strlen("debug-disable") ) == 0 )
	{
		printk("	process debug-disable, len=%d\n", len);

		len -= strlen("debug-disable");
		gEnableDbg = 0;
		goto proc_write_stop_flooding_packet_exit;
	}

	/* allow-fragment-pass-num */
	if ( strnicmp( sBuf, "allow-fragment-pass-num", strlen("allow-fragment-pass-num") ) == 0 )
	{
		printk("	process allow-fragment-pass-num, len=%d\n", len);

		len -= strlen("allow-fragment-pass-num");
		for (pPtr=sBuf+strlen("allow-fragment-pass-num"); *pPtr==' ' || *pPtr=='\t'; pPtr++)
		{
			len--;
		}
		pTmpPtr=pPtr;
		for (; *pTmpPtr!=' ' && *pTmpPtr!='\t' && *pTmpPtr!='\0'; pTmpPtr++) { }
		memset(val, 0, sizeof(val));
		memcpy(val, pPtr, pTmpPtr - pPtr);
		pPtr = pTmpPtr;
		gFragmentAllowPassNum=atoi(val);

		for( i=0 ; i < PKT_FLOODING_ENTRY_MAX ; i++ )
		{
			entry = &g_pkt_flooding_tbl[i];

			if(entry->bUsed == 0)
				continue;

			if(entry->type == FLOODING_FRAGMENT_TYPE)
			{
				pkt_flooding_remove(i);
			}
		}

		goto proc_write_stop_flooding_packet_exit;
	}

	/* max-fragment-packet-num */
	if ( strnicmp( sBuf, "max-fragment-packet-num", strlen("max-fragment-packet-num") ) == 0 )
	{
		printk("	process max-fragment-packet-num, len=%d\n", len);

		len -= strlen("max-fragment-packet-num");
		for (pPtr=sBuf+strlen("max-fragment-packet-num"); *pPtr==' ' || *pPtr=='\t'; pPtr++)
		{
			len--;
		}
		pTmpPtr=pPtr;
		for (; *pTmpPtr!=' ' && *pTmpPtr!='\t' && *pTmpPtr!='\0'; pTmpPtr++) { }
		memset(val, 0, sizeof(val));
		memcpy(val, pPtr, pTmpPtr - pPtr);
		pPtr = pTmpPtr;
		gFragmentMaxPacketNumInOneSecond=atoi(val);

		gFragment_packet_count_in_one_second = 0;
		gFragment_hit_time = 0;
		gFragment_hit_timeout = 0; // one second timeout for fragment type

		for( i=0 ; i < PKT_FLOODING_ENTRY_MAX ; i++ )
		{
			entry = &g_pkt_flooding_tbl[i];

			if(entry->bUsed == 0)
				continue;

			if(entry->type == FLOODING_FRAGMENT_TYPE)
			{
				pkt_flooding_remove(i);
			}
		}

		goto proc_write_stop_flooding_packet_exit;
	}

	/* add / del*/
	if ( strnicmp( sBuf, "add", strlen("add") ) == 0 || strnicmp( sBuf, "del", strlen("del") ) == 0 )
	{
		if (strnicmp( sBuf, "add", strlen("add") ) == 0) {
			printk("	process add, len=%d\n", len);

			len -= strlen("add");
			operation = 0;
			for (pPtr=sBuf+strlen("add"); *pPtr==' ' || *pPtr=='\t'; pPtr++)
			{
				len--;
			}
		} else {
			printk("	process del, len=%d\n", len);

			len -= strlen("del");
			operation = 1;
			for (pPtr=sBuf+strlen("del"); *pPtr==' ' || *pPtr=='\t'; pPtr++)
			{
				len--;
			}
		}

		/* IPv4/IPv6 */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++)
		{
			len--;
		}
		if (*pPtr == '\0') {
			proc_write_stop_flooding_packet_help();
			goto proc_write_stop_flooding_packet_exit;
		}
		if ( strnicmp( pPtr, "IPv4", strlen("IPv4") ) == 0 || strnicmp( pPtr, "IPv6", strlen("IPv6") ) == 0 )
		{
			if (strnicmp( pPtr, "IPv4", strlen("IPv4") ) == 0) {
				printk("	process IPv4, len=%d\n", len);

				len -= strlen("IPv4");
				proto_family = 0;
				pPtr += strlen("IPv4");
			} else {
				printk("	process IPv6, len=%d\n", len);

				len -= strlen("IPv6");
				proto_family = 1;
				pPtr += strlen("IPv6");
			}
		}
		else
		{
			proc_write_stop_flooding_packet_help();
			goto proc_write_stop_flooding_packet_exit;
		}

		// Fragment/TCP/UDP
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++)
		{
			len--;
		}
		if (*pPtr == '\0') {
			proc_write_stop_flooding_packet_help();
			goto proc_write_stop_flooding_packet_exit;
		}
		if ( strnicmp( pPtr, "Fragment", strlen("Fragment") ) == 0 || strnicmp( pPtr, "TCP", strlen("TCP") ) == 0 || strnicmp( pPtr, "UDP", strlen("UDP") ) == 0 )
		{
			if (strnicmp( pPtr, "Fragment", strlen("Fragment") ) == 0) {
				printk("	process Fragment, len=%d\n", len);

				len -= strlen("Fragment");
				packet_type = FLOODING_FRAGMENT_TYPE;
				pPtr += strlen("Fragment");
			}
			else if (strnicmp( pPtr, "TCP", strlen("TCP") ) == 0) {
				printk("	process TCP, len=%d\n", len);

				len -= strlen("TCP");
				packet_type = IPPROTO_TCP;
				pPtr += strlen("TCP");
			}
			else {
				printk("	process UDP, len=%d\n", len);

				len -= strlen("UDP");
				packet_type = IPPROTO_UDP;
				pPtr += strlen("UDP");
			}
		}
		else
		{
			proc_write_stop_flooding_packet_help();
			goto proc_write_stop_flooding_packet_exit;
		}

		// source ip address
		printk("	process srouce ip address and destination port, len=%d\n", len);
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++)
		{
			len--;
		}
		if (*pPtr == '\0') {
			if(packet_type == FLOODING_FRAGMENT_TYPE)
			{	// Fragment => drop all fragment packets
				goto proc_write_stop_flooding_packet_setting;
			}
			proc_write_stop_flooding_packet_help();
			goto proc_write_stop_flooding_packet_exit;
		}
		p2 = pPtr;
		get_ip_port(&p2, &len, val);
		if ( val[5] == 4 )
		{
			//printk("	get_ip_port() return val[5]=0x%x, line %d\n", val[5], __LINE__);
			if ( is_ipv6 )
				ipv6_src_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
			else
				src_ip = htonl((val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3]);
		}
		else
		{
			//printk("	get_ip_port() return val[5]=0x%x, line %d\n", val[5], __LINE__);
			is_ipv6 = 1;
			ipv6_src_ip[0] = htonl((val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3]);
			ipv6_src_ip[1] = htonl((val[6] << 24) | (val[7] << 16) | (val[8] << 8) | val[9]);
			ipv6_src_ip[2] = htonl((val[10] << 24) | (val[11] << 16) | (val[12] << 8) | val[13]);
			ipv6_src_ip[3] = htonl((val[14] << 24) | (val[15] << 16) | (val[16] << 8) | val[17]);
		}
		// destport
		dest_port = htons(val[4]);
		printk("	process dest_port=0x%x\n", dest_port);

		if(is_ipv6)
		{
			memcpy(ipv6_addr, (unsigned char*)&ipv6_src_ip[0], 4);
			memcpy(ipv6_addr+4, (unsigned char*)&ipv6_src_ip[1], 4);
			memcpy(ipv6_addr+8, (unsigned char*)&ipv6_src_ip[2], 4);
			memcpy(ipv6_addr+12, (unsigned char*)&ipv6_src_ip[3], 4);
		}

		// setting
proc_write_stop_flooding_packet_setting:
		printk("	gStopFloodingPacket=%d\n", gStopFloodingPacket);
		printk("	list input data:\n");
		printk("	operation(0:add; 1:del)=%d\n", operation);
		printk("	proto_family(0:IPv4; 1:IPv6)=%d\n", proto_family);
		printk("	packet_type(0:Fragment; 6:TCP; 17:UDP)=%d\n", packet_type);
		printk("	is_ipv6=%d\n", is_ipv6);
		if(proto_family==0)
		{	// IPv4
			printk("	src_ip=0x%x\n", ntohl(src_ip));
		}
		else
		{	// IPv6
			printk("	ipv6_src_ip=%08x:%08x:%08x:%08x\n", ipv6_src_ip[0], ipv6_src_ip[1], ipv6_src_ip[2], ipv6_src_ip[3]);

		}
		printk("	dest_port=%d\n", ntohs(dest_port));
		printk("	end of listing input data\n");

		find_idx = pkt_flooding_lookup(proto_family, packet_type, src_ip, ipv6_addr, dest_port, NULL);
		printk("	pkt_flooding_lookup() return %d\n", find_idx);
		if(operation==0)
		{	// add
			if(find_idx == -1)
			{
				pkt_flooding_add(proto_family, packet_type, src_ip, ipv6_addr, dest_port);
			}
			else
			{
				printk("	existed index %d\n", find_idx);
			}
		}
		else
		{	// delete
			if(find_idx != -1)
			{
				printk("	remove index %d\n", find_idx);
				pkt_flooding_remove(find_idx);
			}
		}

		goto proc_write_stop_flooding_packet_exit;
	}

	proc_write_stop_flooding_packet_help();

proc_write_stop_flooding_packet_exit:
	printk("[proc_write_stop_flooding_packet] end\n");
	return count;
}

#define PPP_IP		0x21	/* Internet Protocol */
int IsFloodingPakcet(unsigned char *packet_data, int data_len)
{
	struct ethhdr *eth =NULL;
	struct iphdr	*iph = NULL;
	unsigned char *data = NULL;
	int len, i;
	unsigned short proto = FLOODING_FRAGMENT_TYPE;
	unsigned char bFragmentPacket = 0;
	unsigned int src_ip = 0;
	unsigned dest_port = 0;
	struct tcphdr *pTCPHdr = NULL;
	unsigned long cur_time = jiffies;
    unsigned char	ipv6_addr[16] = {0};

	//if( (gStopFloodingPacket == 0) || (gFilterRuleCount == 0) )
	if( (gStopFloodingPacket == 0) )
		return 0;

#if (DEBUG_DROP_FLOODING_PACKET > 0)
	printk("[IsFloodingPakcet] start(line %d)\n", __LINE__);
#endif

#if (DEBUG_DROP_FLOODING_PACKET > 1)
	printk("	packet_data(line %d)\n", __LINE__);
	print_packet(packet_data, data_len);
#endif

	len = data_len;
	data = packet_data;
	eth = (struct ethhdr *)data;
	if( ntohs(eth->h_proto) == ETH_P_8021Q)
	{
#if (DEBUG_DROP_FLOODING_PACKET > 0)
		printk("	ETH_P_8021Q(line %d)\n", __LINE__);
#endif
		data += 4;
		len -= 4;
		eth = (struct ethhdr *)data;
	}
	switch( ntohs(eth->h_proto) )
	{
		case ETH_P_8021Q:
#if (DEBUG_DROP_FLOODING_PACKET > 0)
			printk("	ETH_P_8021Q, more vlan tag, ignore(line %d)\n", __LINE__);
#endif
			goto IsFloodingPacket_Exit;
		break;

		case ETH_P_PPP_SES:
#if (DEBUG_DROP_FLOODING_PACKET > 0)
			printk("	ETH_P_PPP_SES, proto=%x(line %d)\n", *((unsigned short *)(data + ETH_HLEN + 6)), __LINE__);
#if (DEBUG_DROP_FLOODING_PACKET > 1)
			print_packet(data + ETH_HLEN + 6, len - ETH_HLEN - 6);
#endif
#endif
			if(ntohs(*((unsigned short *)(data + ETH_HLEN + 6))) == PPP_IP)
			{
				iph = (struct iphdr*)(data + ETH_HLEN + 6 + 2);
			}
			else
			{
#if (DEBUG_DROP_FLOODING_PACKET > 0)
				printk("	ETH_P_PPP_SES, not IP, ignore(line %d)\n", __LINE__);
#endif
				goto IsFloodingPacket_Exit;
			}
		break;

		case ETH_P_IP:
		case ETH_P_IPV6:
#if (DEBUG_DROP_FLOODING_PACKET > 0)
			printk("	ETH_P_IP or ETH_P_IPV6(line %d)\n", __LINE__);
#if (DEBUG_DROP_FLOODING_PACKET > 1)
			print_packet(data + ETH_HLEN, len - ETH_HLEN);
#endif
#endif
			iph = (struct iphdr*)(data + ETH_HLEN);
		break;

		default:
#if (DEBUG_DROP_FLOODING_PACKET > 0)
			printk("	not support, ignore(line %d)\n", __LINE__);
#if (DEBUG_DROP_FLOODING_PACKET > 1)
			print_packet(data, len);
#endif
#endif
				goto IsFloodingPacket_Exit;
	}

	if( iph->version == 4 )
	{	// IPv4
		if (iph->frag_off & htons(IP_MF | IP_OFFSET))
		{	// Fragment
#if (DEBUG_DROP_FLOODING_PACKET > 1)
			printk("	find fragment packet, data=\n");
			print_packet(data, len);
#endif

			bFragmentPacket = 1;
			src_ip = iph->saddr;

			if(gFragment_hit_time == 0)
			{
				gFragment_packet_count_in_one_second = 0;
				gFragment_hit_time = jiffies;
				gFragment_hit_timeout = jiffies + ONE_SECOND_INTERVAL; // one second timeout for fragment type
			}
			else if(time_after(cur_time, gFragment_hit_timeout))
			{
				if(gFragment_packet_count_in_one_second > gFragmentMaxPacketNumInOneSecond)
				{
					if(gEnableFragmentFilter == 0)
						pkt_flooding_add(0, FLOODING_FRAGMENT_TYPE, 0, ipv6_addr, 0);
				}
				else
				{
					if(gEnableFragmentFilter && (gFragmentFilterIndex != -1) )
						pkt_flooding_remove(gFragmentFilterIndex);
				}

				gFragment_packet_count_in_one_second = 0;
				gFragment_hit_time = jiffies;
				gFragment_hit_timeout = jiffies + ONE_SECOND_INTERVAL; // one second timeout for fragment type
			}
			else
			{
				gFragment_packet_count_in_one_second++;
			}
		}
		else
		{	// Protocol
			if( (gFilterRuleCount == 0) || ( (gFilterRuleCount == 1) && gEnableFragmentFilter ) )
				goto IsFloodingPacket_Exit;

			bFragmentPacket = 0;
			proto = iph->protocol;
			src_ip = iph->saddr;
			switch(proto)
			{
				case IPPROTO_TCP:
				case IPPROTO_UDP:
					pTCPHdr = (struct tcphdr *)(((unsigned char *)iph) + iph->ihl * 4);
					dest_port = pTCPHdr->dest;
				break;

				default:
					goto IsFloodingPacket_Exit;
			}
		}

		// check rule now
		for(i=0; i<PKT_FLOODING_ENTRY_MAX; i++)
		{
			if(g_pkt_flooding_tbl[i].bUsed == 0)
				continue;

			if( (bFragmentPacket == 1) && (g_pkt_flooding_tbl[i].type == FLOODING_FRAGMENT_TYPE) )
			{
				if( (g_pkt_flooding_tbl[i].src_ip == 0) || (g_pkt_flooding_tbl[i].src_ip == src_ip) )
				{	// drop all fragment packets
#if (DEBUG_DROP_FLOODING_PACKET > 0)
					printk("	find fragment index %d, src_ip=0x%x, line %d\n", i, src_ip, __LINE__);
#endif
					if(g_pkt_flooding_tbl[i].fragment_pass_count >= gFragmentAllowPassNum)
					{
						if(gEnableDbg)
							printk("[IsFloodingPacket] fragment blick, g_pkt_flooding_tbl[%d].fragment_pass_count=%d, line %d\n", i, g_pkt_flooding_tbl[i].fragment_pass_count, __LINE__);

						g_pkt_flooding_tbl[i].fragment_pass_count=0;
						g_pkt_flooding_tbl[i].hit_time = jiffies;
						g_pkt_flooding_tbl[i].filter_count += 1;
						goto IsFloodingPacket_Match_Exit;
					}
					if(gEnableDbg)
						printk("[IsFloodingPacket] fragment pass, g_pkt_flooding_tbl[%d].fragment_pass_count=%d, line %d\n", i, g_pkt_flooding_tbl[i].fragment_pass_count, __LINE__);

					g_pkt_flooding_tbl[i].fragment_pass_count++;
				}
			}
			else if( (g_pkt_flooding_tbl[i].type == proto) && (g_pkt_flooding_tbl[i].src_ip == src_ip) )
			{
#if (DEBUG_DROP_FLOODING_PACKET > 0)
				printk("	find index %d, src_ip=0x%x, dest_port=0x%x, line %d\n", i, src_ip, dest_port, __LINE__);
#endif

				g_pkt_flooding_tbl[i].hit_time = jiffies;
				g_pkt_flooding_tbl[i].filter_count += 1;
				goto IsFloodingPacket_Match_Exit;
			}
			else
			{
				if(gEnableDbg)
					printk("	cur_time=%lu, g_pkt_flooding_tbl[%d].hit_time=%lu, line %d\n", cur_time, i, g_pkt_flooding_tbl[i].hit_time, __LINE__);

				if(g_pkt_flooding_tbl[i].type != FLOODING_FRAGMENT_TYPE)
				{
					//if( (cur_time - g_pkt_flooding_tbl[i].hit_time) > FLOODING_TIME_INTERVAL )
					if(time_after(cur_time, g_pkt_flooding_tbl[i].hit_time + FLOODING_TIME_INTERVAL))
					{
						pkt_flooding_remove(i);
					}
				}
			}
		}
	}
	else if( (((struct iphdr *)iph)->version == 6) )
	{	// IPv6
		goto IsFloodingPacket_Exit;
	}

IsFloodingPacket_Exit:
#if (DEBUG_DROP_FLOODING_PACKET > 0)
	printk("[IsFloodingPakcet] end(line %d)\n", __LINE__);
#endif
	return 0;

IsFloodingPacket_Match_Exit:
#if (DEBUG_DROP_FLOODING_PACKET > 0)
	printk("[IsFloodingPakcet] end(line %d)\n", __LINE__);
#endif
	return 1;
}