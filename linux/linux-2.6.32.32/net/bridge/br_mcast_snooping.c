/*
 * Bridge multicast snooping.
 *
 * Copyright (c)
 *
 *
 *
 *
 *
 *
 */
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/netfilter_bridge.h>
#include <linux/in.h>
#include <net/ipv6.h>
#include <linux/ip.h>
#include <linux/igmp.h>
#include <linux/jhash.h>
#include <linux/timer.h>
#ifdef CONFIG_SYSCTL
#include <linux/sysctl.h>
#endif
#include "br_private.h"

int bridge_igmp_snooping = 0;
int bridge_mld_snooping = 0;

#ifdef CONFIG_SYSCTL
static struct ctl_table_header *br_mcast_sysctl_header;

static
int br_mcast_sysctl_call_tables(ctl_table * ctl, int write,
			    void __user * buffer, size_t * lenp, loff_t * ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write && *(int *)(ctl->data))
		*(int *)(ctl->data) = 1;
	return ret;
}

static ctl_table br_mcast_table[] = {
	{
#if 0
		.ctl_name	= NET_BRIDGE_IGMP_SNOOPING,
#endif
		.procname	= "bridge-igmp-snooping",
		.data		= &bridge_igmp_snooping,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &br_mcast_sysctl_call_tables,
	},
	{
#if 0
		.ctl_name	= NET_BRIDGE_MLD_SNOOPING,
#endif
		.procname	= "bridge-mld-snooping",
		.data		= &bridge_mld_snooping,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &br_mcast_sysctl_call_tables,
	},
	{ .ctl_name = 0 }
};

static struct ctl_path brnf_path[] = {
	{ .procname = "net", .ctl_name = CTL_NET, },
	{ .procname = "bridge", .ctl_name = NET_BRIDGE, },
	{ }
};
#endif

void br_mcast_snoop_init(void)
{
#ifdef CONFIG_SYSCTL
	br_mcast_sysctl_header = register_sysctl_paths(brnf_path, br_mcast_table);
	if (br_mcast_sysctl_header == NULL) {
		printk(KERN_WARNING "Failed to register bridge mcast snoop sysctl table.\n");
		return;
	}
#endif
}

void br_mcast_snoop_deinit(void)
{
#ifdef CONFIG_SYSCTL
	unregister_sysctl_table(br_mcast_sysctl_header);
#endif
}

static void igmp_router_timer_handler(unsigned long data)
{
	struct net_bridge_port *port = (struct net_bridge_port *)data;

	port->igmp_router_port = 0;
}

static void mld_router_timer_handler(unsigned long data)
{
	struct net_bridge_port *port = (struct net_bridge_port *)data;

	port->mld_router_port = 0;
}

void br_mcast_port_init(struct net_bridge_port *port)
{
	/* init mghash secret */
	get_random_bytes(&port->mghash_secret, sizeof(port->mghash_secret));
	get_random_bytes(&port->mghash_secret6, sizeof(port->mghash_secret6));

	/* init router port timer */
	port->igmp_router_timer.function = igmp_router_timer_handler;
	port->igmp_router_timer.data = (unsigned int)port;
	init_timer(&port->igmp_router_timer);

	port->mld_router_timer.function = mld_router_timer_handler;
	port->mld_router_timer.data = (unsigned int)port;
	init_timer(&port->mld_router_timer);
}

void br_mcast_port_cleanup(struct net_bridge_port *port)
{
	del_timer(&port->igmp_router_timer);
	del_timer(&port->mld_router_timer);
}

static void init_ipaddr(ipaddr_t *addr, ptype_t type, void *addrp)
{
    if (addr == NULL)
        return;

    if (type == IPV4) {
        addr->type = IPV4;
        if (addrp)
            addr->addr.ip4.s_addr = *((unsigned int *)addrp);
        else
            addr->addr.ip4.s_addr = 0;
    }
    else if (type == IPV6) {
        struct in6_addr *in6 = (struct in6_addr *)addrp;
        addr->type = IPV6;
        if (in6) {
			addr->addr.ip6.s6_addr32[0] = in6->s6_addr32[0];
			addr->addr.ip6.s6_addr32[1] = in6->s6_addr32[1];
			addr->addr.ip6.s6_addr32[2] = in6->s6_addr32[2];
			addr->addr.ip6.s6_addr32[3] = in6->s6_addr32[3];
        } else {
			addr->addr.ip6.s6_addr32[0] = 0;
			addr->addr.ip6.s6_addr32[1] = 0;
			addr->addr.ip6.s6_addr32[2] = 0;
			addr->addr.ip6.s6_addr32[3] = 0;
		}
	}
}

static int is_same_ipaddr(ipaddr_t *addr1, ipaddr_t *addr2)
{
    if (addr1 == NULL || addr2 == NULL)
        return false;

    if (addr1->type == IPV4 && addr2->type == IPV4) {
        return addr1->addr.ip4.s_addr == addr2->addr.ip4.s_addr;
    }
    else if (addr1->type == IPV6 && addr2->type == IPV6) {
        return addr1->addr.ip6.s6_addr32[0] == addr2->addr.ip6.s6_addr32[0] &&
		       addr1->addr.ip6.s6_addr32[1] == addr2->addr.ip6.s6_addr32[1] &&
			   addr1->addr.ip6.s6_addr32[2] == addr2->addr.ip6.s6_addr32[2] &&
			   addr1->addr.ip6.s6_addr32[3] == addr2->addr.ip6.s6_addr32[3];
    }

    return 0;
}

static void copy_ipaddr(ipaddr_t *to, ipaddr_t *from)
{
    if (to == NULL || from == NULL)
        return;

    memcpy(to, from, sizeof(ipaddr_t));
}

static int IS_ADDR_UNSPECIFIED(ipaddr_t *addr)
{
    if (addr->type == IPV4)
        return addr->addr.ip4.s_addr == 0;
    else if (addr->type == IPV6)
        return (addr->addr.ip6.s6_addr32[0] == 0) && 
		       (addr->addr.ip6.s6_addr32[1] == 0) && 
		       (addr->addr.ip6.s6_addr32[2] == 0) && 
		       (addr->addr.ip6.s6_addr32[3] == 0);

    return 0;
}

static inline u32 br_gaddr_hash(struct net_bridge_port *port, ipaddr_t *gaddr)
{
	u32 hash;

	if (gaddr->type == IPV4) {
		u32 initval = gaddr->addr.ip4.s_addr;
		hash = jhash_1word(initval, port->mghash_secret) & (BR_HASH_SIZE - 1);
	} else if (gaddr->type == IPV6) {
		u32 *k = (u32 *)&gaddr->addr.ip6.s6_addr32[0];
		hash = jhash2(k, 4, port->mghash_secret6) & (BR_HASH_SIZE - 1);
	}

	return hash;
}

static inline struct net_bridge_mg_entry *_br_mg_get_entry(struct hlist_head *head, 
                                                           ipaddr_t *gaddr)
{
	struct net_bridge_mg_entry *entry;
	struct hlist_node *h;

	hlist_for_each_entry_rcu(entry, h, head, hlist) {
		if(is_same_ipaddr(&entry->gaddr, gaddr))
			return entry;
	}

	return NULL;
}

/* Search multicast group table for a group */
struct net_bridge_mg_entry *br_mg_get_entry(struct net_bridge_port *port, ipaddr_t *gaddr)
{
	struct hlist_head *head;
	unsigned int hash;

	hash = br_gaddr_hash(port, gaddr);
	head = &port->mghash[hash];

	return _br_mg_get_entry(head, gaddr);
}

static void _br_mg_free_entry(struct rcu_head *head)
{
	struct net_bridge_mg_entry *entry = 
		container_of(head, struct net_bridge_mg_entry, rcu);

	kfree(entry);
}

static void br_mg_delete_entry(struct net_bridge_mg_entry *entry)
{
	hlist_del_rcu(&entry->hlist);
	call_rcu(&entry->rcu, _br_mg_free_entry);
}

static int br_mg_update_entry(struct net_bridge_port *port, struct net_bridge_mg_entry *old, 
                              struct hlist_head *head, ipaddr_t *gaddr, u8 filter, 
                              u8 compat, u32 saddr_cnt, ipaddr_t *saddr)
{
	struct net_bridge_mg_entry *new;
	int i;

	/* prepare a new temporary group entry */
	new = kzalloc(sizeof(struct net_bridge_mg_entry) + saddr_cnt * sizeof(ipaddr_t), 
	              GFP_ATOMIC);

	if(unlikely(!new))
		return -1;

	copy_ipaddr(&new->gaddr, gaddr);
	new->port = port;
	new->filter_mode = filter;
	new->compat_mode = compat;
	new->saddr_cnt = saddr_cnt;
	for(i = 0; i < saddr_cnt; i++)
		copy_ipaddr(&new->saddr[i], &saddr[i]);

	/* delete old entry */
	br_mg_delete_entry(old);

	/* add new entry */
	hlist_add_head_rcu(&new->hlist, head);

	return 0;
}

static void br_mg_del_table(struct net_bridge_port *port)
{
	int i;

	spin_lock(&port->mghash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct net_bridge_mg_entry *entry;
		struct hlist_node *h, *n;

		hlist_for_each_entry_safe(entry, h, n, &port->mghash[i], hlist) {
			br_mg_delete_entry(entry);
		}
	}
	spin_unlock(&port->mghash_lock);
}

/*Delete a group from multicast group table*/
int br_mg_del_record(struct net_bridge_port *port, ipaddr_t *gaddr)
{
	struct net_bridge_mg_entry *entry;
	struct hlist_head *head;
	unsigned int hash;

	spin_lock(&port->mghash_lock);

	hash = br_gaddr_hash(port, gaddr);
	head = &port->mghash[hash];

	if (!IS_ADDR_UNSPECIFIED(gaddr)) {
		entry = _br_mg_get_entry(head, gaddr);
		if (entry) br_mg_delete_entry(entry);
	} else {
		br_mg_del_table(port);
	}

	spin_unlock(&port->mghash_lock);

	return 0;
}

/* Add a group to multicast group table */
int br_mg_add_entry(struct net_bridge_port *port, 
					ipaddr_t *gaddr, u8 filter, u8 compat, u32 saddr_cnt, ipaddr_t *saddr)
{
	struct net_bridge_mg_entry *entry;
	struct hlist_head *head;
	unsigned int hash;
	int i;

	spin_lock(&port->mghash_lock);

	hash = br_gaddr_hash(port, gaddr);
	head = &port->mghash[hash];

	entry = _br_mg_get_entry(head, gaddr);
	if(!entry) {
		/* Create new mg entry */
		entry = kzalloc(sizeof(*entry) + saddr_cnt * sizeof(ipaddr_t), GFP_ATOMIC);
		if(unlikely(!entry)) {
			spin_unlock(&port->mghash_lock);
			return -1;
		}

		entry->port = port;
		copy_ipaddr(&entry->gaddr, gaddr);
		entry->filter_mode = filter;
		entry->compat_mode = compat;
		entry->saddr_cnt = saddr_cnt;
		for(i = 0; i < saddr_cnt; i++)
			copy_ipaddr(&entry->saddr[i], &saddr[i]);

		hlist_add_head_rcu(&entry->hlist, head);
	}
	else {
		/* Update existing entry */
		br_mg_update_entry(port, entry, head, gaddr, filter, compat, saddr_cnt, saddr);
	}

	spin_unlock(&port->mghash_lock);

	return 0;
}

/* Handle IGMP / MLD query */
static int br_forward_query(struct net_bridge_port *port, ipaddr_t *gaddr, int ver)
{
	struct net_bridge_mg_entry *entry;
	u8 compat_mode;

	/* Handle Group specific query */
	if(NULL == (entry = br_mg_get_entry(port, gaddr))) {
		/* no matching group found */
		return 0;
	}

	rcu_read_lock();	// XXX: needed ???
	compat_mode = entry->compat_mode;
	rcu_read_unlock();
	
#if 1 /* Terry 20121115, don't care the version for IGMP query forwarding */
	return 1;
#else
	if (compat_mode == ver)
		return 1;
	else
		return 0;
#endif
}

/* Handle multicast data */
static int br_snoop_multicast_data(struct net_bridge_port *port, ipaddr_t *gaddr, 
                                   ipaddr_t *saddr)
{
	struct net_bridge_mg_entry *entry;
	int i, found = 0, filter_mode = 1, ret = 1;

	if(NULL == (entry = br_mg_get_entry(port, gaddr))) {
		/* no matching group found */
		return 0;
	}

	rcu_read_lock();	// XXX: needed ???
	filter_mode = entry->filter_mode;

	for (i = 0; i < entry->saddr_cnt; i++) {
		if(is_same_ipaddr(&entry->saddr[i], saddr)) {
			found = 1;
			break;
		}
	}
	rcu_read_unlock();

	if(found) {
		ret = filter_mode == 1 ? 1 : 0;
	}
	else {
		ret = filter_mode == 0 ? 1 : 0;
	}

	return ret;
}

struct mld2_query {
    __u8 type;
    __u8 code;
    __sum16 csum;
    __be16 mrc;
    __be16 resv1;
    struct in6_addr mca;
#if defined(__LITTLE_ENDIAN_BITFIELD)
    __u8 qrv:3,
         suppress:1,
         resv2:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
    __u8 resv2:4,
         suppress:1,
         qrv:3;
#else
#error "Please fix <asm/byteorder.h>"
#endif
    __u8 qqic;
    __be16 nsrcs;
    struct in6_addr srcs[0];
};

#define IN6_IS_ADDR_MULTICAST(a) (((__const uint8_t *) (a))[0] == 0xff)

static int br_selective_flood6(struct net_bridge_port *p, struct sk_buff *skb)
{
	struct ipv6hdr *iph;
	ipaddr_t daddr, gaddr, saddr;
	int mldlen;
	int ver = 3;

	iph = (struct ip6hdr *)skb_network_header(skb);

	/* Also not interested if IP dest address is not a multicast address */
	if(!IN6_IS_ADDR_MULTICAST(&iph->daddr))
		return 1;

	init_ipaddr(&daddr, IPV6, &iph->daddr);
	init_ipaddr(&saddr, IPV6, &iph->saddr);

	if(iph->nexthdr == IPPROTO_HOPOPTS) {
		struct ipv6_opt_hdr *opt = (struct ipv6_opt_hdr *)((void *)iph + sizeof(struct ipv6hdr));

		if (opt->nexthdr == IPPROTO_ICMPV6) {
			struct mld2_query *query = NULL;
			struct icmp6hdr *icmp6h = (struct icmp6hdr *) ((void *)opt + ipv6_optlen(opt));

			switch(icmp6h->icmp6_type) {
				case ICMPV6_MGM_QUERY:
					if (p->mld_router_port) {
						return 0;
					}
					query = (struct mld2_query *)icmp6h;
					mldlen = ntohs(iph->payload_len) - ipv6_optlen(opt);
					if (mldlen == 24) {
						ver = MLDV1;
					} else if (mldlen >= 28) {
						ver = MLDV2;
					} else
						return 1; /* Do not handle malformed MLD query packet */
					init_ipaddr(&gaddr, IPV6, &query->mca);
					return br_forward_query(p, &gaddr, ver);
				case ICMPV6_MGM_REPORT:
				case ICMPV6_MGM_REDUCTION:
				case ICMPV6_MLD2_REPORT:
					if (p->mld_router_port) {
						return 1;
					} else
						return 0;	/* Do not flood MLD reports if not router port */
				default:
					break;
			}
		}
	}

	return br_snoop_multicast_data(p, &daddr, &saddr);
}

static int br_selective_flood4(struct net_bridge_port *p, struct sk_buff *skb)
{
	struct iphdr *iph;
	struct igmphdr *igmph;
	struct igmpv3_query *igmph3;
	ipaddr_t daddr, gaddr, saddr;
	int ret = 1;
	int igmplen;
	int ver = 3;

	iph = (struct iphdr *)skb_network_header(skb);

	/* Also not interested if IP dest address is not a multicast address */
	if(!IN_MULTICAST(ntohl(iph->daddr)))
		return 1;

	init_ipaddr(&daddr, IPV4, &iph->daddr);
	init_ipaddr(&saddr, IPV4, &iph->saddr);

	if(iph->protocol == IPPROTO_IGMP) {
		igmph = (struct igmphdr *) ((void *)iph + 4 * (int)iph->ihl);

		switch(igmph->type) {
		case IGMP_HOST_MEMBERSHIP_QUERY:
			if (p->igmp_router_port) 
				return 0;
			igmph3 = (struct igmpv3_query *)igmph;
			igmplen = ntohs((int)iph->tot_len) - 4 * (int)iph->ihl; 
			if(igmplen == 8) {
				ver = igmph->code == 0 ? 1 : 2;
			}
			else if(igmplen > 8 && igmplen < 12)
				return 1;	/* Do not handle malformed IGMP query packet */
#if 1 /* Terry 20120727, this workaround may be removed if PPA is enabled */
	  /* Terry 20121110, need to handle IGMPv2 case */
			if (igmph->group == 0)
				return 1;   /* Do not handle IGMPv3 Generic query */
#endif
			init_ipaddr(&gaddr, IPV4, &igmph->group);
			return br_forward_query(p, &gaddr, ver);
		case IGMP_HOST_MEMBERSHIP_REPORT:
		case IGMPV2_HOST_MEMBERSHIP_REPORT:
		case IGMPV3_HOST_MEMBERSHIP_REPORT:
		case IGMP_HOST_LEAVE_MESSAGE:
			if (p->igmp_router_port)
				return 1;
			else
				return 0;	/* Do not flood IGMP reports */
		default:
			break;
		}

		return ret;
	}

	/* Skip snooping 224.0.0.x and 239.0.0.x */
	if((ntohl(iph->daddr) & 0xffffff00U) == 0xe0000000U ||
		(ntohl(iph->daddr) & 0xffffff00U) == 0xef000000U)
		return 1;

#if 1
	/* Terry 20121220, bypass the packets to 239.255.0.0/16 */
	/* This may only for DT requirement. */
	if ((ntohl(iph->daddr) & 0xffff0000U) == 0xefff0000U)
		return 1;
#endif

	return br_snoop_multicast_data(p, &daddr, &saddr);
}

int br_selective_flood(struct net_bridge_port *p, struct sk_buff *skb)
{
	if (skb->protocol == htons(ETH_P_IP)) {
		if (bridge_igmp_snooping)
			return br_selective_flood4(p, skb);
		else
			return 1;
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		if (bridge_mld_snooping)
			return br_selective_flood6(p, skb);
		else
			return 1;
	} else
		return 1;
}
