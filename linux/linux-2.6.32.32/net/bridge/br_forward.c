/*
 *	Forwarding decision
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#if defined(CONFIG_LTQ_NETFILTER_PROCFS) && (defined(CONFIG_BRIDGE_NF_EBTABLES) || defined(CONFIG_BRIDGE_NF_EBTABLES_MODULE))
extern int brnf_filter_forward_enable;
extern int brnf_filter_local_out_enable;
extern int brnf_filter_post_routing_enable;
#endif

/* Don't forward packets to originating port or forwarding diasabled */
static inline int should_deliver(const struct net_bridge_port *p,
				 const struct sk_buff *skb)
{
	return (((p->flags & BR_HAIRPIN_MODE) || skb->dev != p->dev) &&
		p->state == BR_STATE_FORWARDING);
}

static inline unsigned packet_length(const struct sk_buff *skb)
{
	return skb->len - (skb->protocol == htons(ETH_P_8021Q) ? VLAN_HLEN : 0);
}

int br_dev_queue_push_xmit(struct sk_buff *skb)
{
	/* drop mtu oversized packets except gso */
	if (packet_length(skb) > skb->dev->mtu && !skb_is_gso(skb))
		kfree_skb(skb);
	else {
		/* ip_refrag calls ip_fragment, doesn't copy the MAC header. */
		if (nf_bridge_maybe_copy_header(skb))
			kfree_skb(skb);
		else {
			skb_push(skb, ETH_HLEN);

			dev_queue_xmit(skb);
		}
	}

	return 0;
}

#ifdef CONFIG_LTQ_BR_OPT
int __bridge br_forward_finish(struct sk_buff *skb)
#else
int br_forward_finish(struct sk_buff *skb)
#endif
{
	skb_mark_priority(skb);
#if defined(CONFIG_LTQ_NETFILTER_PROCFS) && (defined(CONFIG_BRIDGE_NF_EBTABLES) || defined(CONFIG_BRIDGE_NF_EBTABLES_MODULE))
       if (!brnf_filter_post_routing_enable)
               return br_dev_queue_push_xmit(skb);
#endif
	return NF_HOOK(PF_BRIDGE, NF_BR_POST_ROUTING, skb, NULL, skb->dev,
		       br_dev_queue_push_xmit);

}

static void __br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
	skb->dev = to->dev;
#if defined(CONFIG_LTQ_NETFILTER_PROCFS) && (defined(CONFIG_BRIDGE_NF_EBTABLES) || defined(CONFIG_BRIDGE_NF_EBTABLES_MODULE))
       if (!brnf_filter_local_out_enable)
               return br_forward_finish(skb);
#endif
	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_OUT, skb, NULL, skb->dev,
			br_forward_finish);
}

#ifdef CONFIG_LTQ_BR_OPT
static void __bridge __br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
#else
static void __br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
#endif
{
	struct net_device *indev;

	if (skb_warn_if_lro(skb)) {
		kfree_skb(skb);
		return;
	}

  #ifdef CONFIG_ARC_BR_PORT_ISOLATION /*ctc*/
  {
	extern int br_if_iso_entry_existed( char* ifname1, char* ifname2 );
	extern int br_if_iso_is_debug_on( void );
	if (br_if_iso_entry_existed( skb->dev->name, to->dev->name ))
	{
		if (br_if_iso_is_debug_on())
			printk( "%s::%s: drop packet from %s to %s\n", __FILE__, __FUNCTION__, skb->dev->name, to->dev->name );
		kfree_skb(skb);
		return;
	}
	else
	{
		if (br_if_iso_is_debug_on())
			printk( "%s::%s: pass packet from %s to %s\n", __FILE__, __FUNCTION__, skb->dev->name, to->dev->name );
	}
  }
  #endif /* CONFIG_ARC_BR_PORT_ISOLATION */

	indev = skb->dev;
	skb->dev = to->dev;
	skb_forward_csum(skb);

#if defined(CONFIG_LTQ_NETFILTER_PROCFS) && (defined(CONFIG_BRIDGE_NF_EBTABLES) || defined(CONFIG_BRIDGE_NF_EBTABLES_MODULE))
       if (!brnf_filter_forward_enable)
               return br_forward_finish(skb);
#endif
	NF_HOOK(PF_BRIDGE, NF_BR_FORWARD, skb, indev, skb->dev,
			br_forward_finish);
}

/* called with rcu_read_lock */
void br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_deliver(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called with rcu_read_lock */
#ifdef CONFIG_LTQ_BR_OPT
void __bridge br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
#else
void br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
#endif
{
	if (should_deliver(to, skb)) {
		__br_forward(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called under bridge lock */
static void br_flood(struct net_bridge *br, struct sk_buff *skb,
	void (*__packet_hook)(const struct net_bridge_port *p,
			      struct sk_buff *skb))
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;

	prev = NULL;

	list_for_each_entry_rcu(p, &br->port_list, list) {
		if (should_deliver(p, skb)) {
			if (prev != NULL) {
				struct sk_buff *skb2;

#ifdef CONFIG_IFX_IGMP_SNOOPING
				if ((bridge_igmp_snooping || bridge_mld_snooping) && 
				    (eth_hdr(skb)->h_dest[0] & 0x1) && 
				    (br_selective_flood(prev, skb) == 0)) {
						prev = p;
						continue;
				}
#endif

				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					br->dev->stats.tx_dropped++;
					kfree_skb(skb);
					return;
				}

				__packet_hook(prev, skb2);
			}

			prev = p;
		}
	}

	if (prev != NULL) {
#ifdef CONFIG_IFX_IGMP_SNOOPING
		if ((bridge_igmp_snooping || bridge_mld_snooping) && 
		    (eth_hdr(skb)->h_dest[0] & 0x1) && 
		    (br_selective_flood(prev, skb) == 0))
			kfree_skb(skb);
		else
#endif
		__packet_hook(prev, skb);
		return;
	}

	kfree_skb(skb);
}


/* called with rcu_read_lock */
void br_flood_deliver(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, __br_deliver);
}

/* called under bridge lock */
void br_flood_forward(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, __br_forward);
}
