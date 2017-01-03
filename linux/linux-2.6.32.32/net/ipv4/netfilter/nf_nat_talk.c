/* 
 * talk extension for UDP NAT alteration. 
 * Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 **
 *
 * Modifications:
 * 2005-02-13 Harald Welte <laforge@netfilter.org>
 * 	- update to 2.6.x API
 * 	- update to post 2.6.11 helper infrastructure
 * 	- use c99 structure initializers
 *  
 */
#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/kernel.h>
#include <net/tcp.h>
#include <net/udp.h>
//#include <linux/udp.h>
//#include <linux/netfilter.h>
//#include <linux/skbuff.h>

#include<linux/moduleparam.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_helper.h>
#include <net/netfilter/nf_nat_rule.h>
#include <linux/netfilter/nf_conntrack_talk.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_expect.h>


MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>");
MODULE_DESCRIPTION("talk network address translation module");

#if 0
#define DEBUGP printk
#define IP_NAT_TALK_DEBUG
#else
#define DEBUGP(format, args...)
#endif



 void
nat_talk_expect(struct nf_conn *ct,
		struct nf_conntrack_expect *exp)
{
	nf_nat_follow_master(ct, exp);

       

	talk_expect(ct, exp);
}

static void
nat_ntalk_expect(struct nf_conn *ct,
		 struct nf_conntrack_expect *exp)
{
	nf_nat_follow_master(ct, exp);
        

	ntalk_expect(ct, exp);
}

static int
mangle_packet(struct sk_buff **pskb,
	      struct nf_conn *ct,
	      u_int32_t newip,
	      u_int16_t port,
	      struct talk_addr *addr,
	      struct talk_addr *ctl_addr)
{
	struct iphdr *iph = ip_hdr(*pskb); //(*pskb)->nh.iph;
	struct udphdr *udph = (void *)iph + iph->ihl * 4;
	size_t udplen = (*pskb)->len - iph->ihl * 4;

	/* Fortunately talk sends a structure with the address and
	   port in it. The size of the packet won't change. */

	if (ctl_addr == NULL) {
		/* response */
		if (addr->ta_addr == INADDR_ANY)
			return 1;
		DEBUGP("ip_nat_talk_mangle_packet: response orig "
		       "%u.%u.%u.%u:%u, inserting %u.%u.%u.%u:%u\n", 
		       NIPQUAD(addr->ta_addr), ntohs(addr->ta_port),
		       NIPQUAD(newip), ntohs(port));
		addr->ta_addr = newip;
		addr->ta_port = port;
	} else {
		/* message */
		if (addr->ta_addr != INADDR_ANY) {
			/* Change address inside packet to match way we're
			 * mapping this connection. */
			DEBUGP("ip_nat_talk_mangle_packet: message orig addr "
			       "%u.%u.%u.%u:%u, inserting %u.%u.%u.%u:%u\n", 
			       NIPQUAD(addr->ta_addr), ntohs(addr->ta_port),
			       NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip), 
			       ntohs(addr->ta_port));
			addr->ta_addr = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
		}
		DEBUGP("ip_nat_talk_mangle_packet: message orig ctl_addr "
		       "%u.%u.%u.%u:%u, inserting %u.%u.%u.%u:%u\n", 
		       NIPQUAD(ctl_addr->ta_addr), ntohs(ctl_addr->ta_port),
		       NIPQUAD(newip), ntohs(port));
		ctl_addr->ta_addr = newip;
		ctl_addr->ta_port = port;
	}

	/* Fix checksums */
	(*pskb)->csum = csum_partial((char *)udph + sizeof(struct udphdr), udplen - sizeof(struct udphdr), 0);
	udph->check = 0;
	udph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, udplen, IPPROTO_UDP,
				        csum_partial((char *)udph, sizeof(struct udphdr), (*pskb)->csum));
		
	ip_send_check(iph);
	return 1;
}

static unsigned int talk_help_msg(struct sk_buff **pskb,
			 struct nf_conn *ct,
		         u_char type,
		         struct talk_addr *addr,
		         struct talk_addr *ctl_addr)
{
	u_int32_t newip;
	u_int16_t port;
	
	unsigned int verdict = NF_ACCEPT;

	DEBUGP("ip_nat_talk_help_msg: addr: %u.%u.%u.%u:%u, ctl_addr: %u.%u.%u.%u:%u, type %d\n",
		NIPQUAD(addr->ta_addr), ntohs(addr->ta_port),
		NIPQUAD(ctl_addr->ta_addr), ntohs(ctl_addr->ta_port),
		type);

	/* Change address inside packet to match way we're mapping
	   this connection. */
	newip = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
	port  = ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.udp.port;
	DEBUGP("ip_nat_talk_help_msg: inserting: %u.%u.%u.%u:%u\n",
		NIPQUAD(newip), ntohs(port));

	if (!mangle_packet(pskb, ct, newip, port, addr, ctl_addr))
		verdict = NF_DROP;

	return verdict;
}

static unsigned int talk_help_resp(struct sk_buff **pskb,
			      struct nf_conntrack_expect *exp,
		              u_char type,
			      u_char answer,
		              struct talk_addr *addr)
{
	struct nf_conn *ct = exp->master;
	u_int32_t newip;
	u_int16_t port, *pport = NULL, *tport;

	DEBUGP("ip_nat_talk_help_response: addr: %u.%u.%u.%u:%u, "
	       "type %d answer %d\n", NIPQUAD(addr->ta_addr),
	       ntohs(addr->ta_port), type, answer);
	
	DEBUGP("ip_nat_talk_help_response: talkinfo port %u (%s)\n", 
		ntohs(exp->tuple.dst.u.tcp.port),
		type == LOOK_UP ? "LOOK_UP" : "ANNOUNCE");

	/* Change address inside packet to match way we're mapping
	   this connection. */
	newip = ct->tuplehash[type == LOOK_UP ? IP_CT_DIR_ORIGINAL : 
						IP_CT_DIR_REPLY].tuple.dst.u3.ip;
	/* We can read expect here without conntrack lock, since it's
	   only set in ip_conntrack_talk , with ip_talk_lock held
	   writable */ 
	if (type == LOOK_UP) {
		//pport = &exp->saved_proto.tcp.port;
		tport = &exp->tuple.dst.u.tcp.port;
	} else {
		//pport = &exp->saved_proto.udp.port;
		tport = &exp->tuple.dst.u.udp.port;
	}

	*pport = *tport;

	exp->tuple.dst.u3.ip = newip;

	if (exp->expectfn == talk_expect)
		exp->expectfn = nat_talk_expect;
	else if (exp->expectfn == ntalk_expect)
		exp->expectfn = nat_ntalk_expect;
	else
		BUG();

	/* Try to get same port: if not, try to change it. */
	for (port = ntohs(*pport); port != 0; port++) {
		*tport = htons(port);

		if (nf_ct_expect_related(exp) == 0) {
			DEBUGP("ip_nat_talk_help_response: using "
			       "%u.%u.%u.%u:%u\n", NIPQUAD(newip), port);
			break;
		}
	}
	if (port == 0) {
	//	ip_conntrack_expect_free(exp);
		return NF_DROP;
	}

	if (!mangle_packet(pskb, ct, newip, htons(port), addr, NULL)) {
		nf_ct_unexpect_related(exp);
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static int __init init(void)
{
	BUG_ON(rcu_dereference(nf_nat_talk_msg_hook));
	BUG_ON(rcu_dereference(nf_nat_talk_resp_hook));
	
	//ip_nat_talk_msg_hook = talk_help_msg;
	//ip_nat_talk_resp_hook = talk_help_response;
	
	rcu_assign_pointer(nf_nat_talk_msg_hook,talk_help_msg );
	rcu_assign_pointer(nf_nat_talk_resp_hook,talk_help_resp );
	
	return 0;
}

static void __exit fini(void)
{
	//ip_nat_talk_resp_hook = NULL;
	//ip_nat_talk_msg_hook = NULL;
	rcu_assign_pointer(nf_nat_talk_msg_hook,NULL );
	 rcu_assign_pointer(nf_nat_talk_resp_hook,NULL );
	/* Make sure noone calls it, meanwhile */
	synchronize_net();
}

module_init(init);
module_exit(fini);


