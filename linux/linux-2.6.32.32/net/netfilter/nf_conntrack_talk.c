/* 
 * talk extension for IP connection tracking. 
 * Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 **
 *     Module load syntax:
 *     insmod ip_nat_talk.o talk=[0|1] ntalk=[0|1] ntalk2=[01]
 *
 *		talk=[0|1]	disable|enable old talk support
 *	       ntalk=[0|1]	disable|enable ntalk support
 *	      ntalk2=[0|1]	disable|enable ntalk2 support
 *
 *     The default is talk=1 ntalk=1 ntalk2=1
 *
 *     The helper does not support simultaneous talk requests.
 **
 *
 *		ASCII art on talk protocols
 *	
 *	
 *	caller server		    callee server
 *		|     \	          /
 *		|	\       /
 *		|	  \   /
 *		|	    /  
 *	 	|	  /   \
 *	      2 |     1 /       \ 3
 *	caller client  ----------- callee client
 *	               		 4
 *
 *	1. caller client <-> callee server: LOOK_UP, then ANNOUNCE invitation 
 *    ( 2. caller client <-> caller server: LEAVE_INVITE to server )
 *	3. callee client <-> caller server: LOOK_UP invitation
 *	4. callee client <-> caller client: talk data channel
 *
 * [1]: M. Hunter, talk: a historical protocol for interactive communication
 *      draft-hunter-talk-00.txt
 * [2]: D.B. Chapman, E.D. Zwicky: Building Internet Firewalls (O'Reilly)	
 */
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <linux/udp.h>
#include <net/udp.h>
#include <linux/moduleparam.h>


#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_helper.h>

#include <linux/netfilter/nf_conntrack_talk.h>

/* Default all talk protocols are supported */
static int talk = 1;
static int ntalk = 1;
static int ntalk2 = 1;
MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>");
MODULE_DESCRIPTION("talk connection tracking module");
MODULE_LICENSE("GPL");
#ifdef MODULE_PARM
MODULE_PARM(talk, "i");
MODULE_PARM_DESC(talk, "support (old) talk protocol");
MODULE_PARM(ntalk, "i");
MODULE_PARM_DESC(ntalk, "support ntalk protocol");
MODULE_PARM(ntalk2, "i");
MODULE_PARM_DESC(ntalk2, "support ntalk2 protocol");
#endif

//DECLARE_LOCK(ip_talk_lock);

rwlock_t ip_talk_rwlock ;

static DEFINE_SPINLOCK(ip_talk_lock);

struct module *nf_conntrack_talk = THIS_MODULE;

static char talk_buffer[65536];

extern spinlock_t ip_talk_lock;

unsigned int (*nf_nat_talk_resp_hook)(struct sk_buff **pskb,
                                      struct nf_conntrack_expect *exp,
                                      u_char type,
                                      u_char answer,
                                      struct talk_addr *addr);
EXPORT_SYMBOL_GPL(nf_nat_talk_resp_hook);

unsigned int (*nf_nat_talk_msg_hook)(struct sk_buff **pskb,
                                     struct nf_conn *ct,
                                     u_char type,
                                     struct talk_addr *addr,
                                     struct talk_addr *ctl_addr);
EXPORT_SYMBOL_GPL(nf_nat_talk_msg_hook);




#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

//static int talk_expect(struct ip_conntrack *ct);
//static int ntalk_expect(struct ip_conntrack *ct);

void  talk_expect(struct nf_conn *ct,
                       struct nf_conntrack_expect *exp);


void  ntalk_expect(struct nf_conn *ct,
                        struct nf_conntrack_expect *exp);


 void  (*talk_expectfn[2])(struct nf_conn *ct, struct nf_conntrack_expect *exp) = {talk_expect, ntalk_expect};

static int talk_help_response(struct sk_buff **pskb,
		              struct nf_conn *ct,
		              enum ip_conntrack_info ctinfo,
		              int talk_port,
		              u_char mode,
		              u_char type,
		              u_char answer,
		              struct talk_addr *addr)
{
	int dir = CTINFO2DIR(ctinfo);
	//struct ip_conntrack_expect expect, *exp = &expect;
	//struct ip_ct_talk_expect *exp_talk_info = &exp->help.exp_talk_info;

        int ret;
        struct nf_conntrack_expect *exp;
        u_int16_t exp_talk_port;
         struct nf_conntrack_man cmd = {}; 
         typeof(nf_nat_talk_resp_hook) talk_help_resp;
        
	DEBUGP("ip_ct_talk_help_response: %u.%u.%u.%u:%u, type %d answer %d\n",
		NIPQUAD(addr->ta_addr), ntohs(addr->ta_port),
		type, answer);

	if (!(answer == SUCCESS && type == mode))
		return NF_ACCEPT;
	
	//memset(&expect, 0, sizeof(expect));
	
         exp = nf_ct_expect_alloc(ct);
        if (exp == NULL) {
                return NF_DROP;
        }

	
	if (type == ANNOUNCE) {

		DEBUGP("ip_ct_talk_help_response: ANNOUNCE\n");

		/* update the talk info */
		//LOCK_BH(&ip_talk_lock);
		spin_lock_bh(&ip_talk_lock);

		exp_talk_port = htons(talk_port);

		/* expect callee client -> caller server message */
	/*	exp->tuple = ((struct  nf_conntrack_tuple)
			{ { {.u3 =  ct->tuplehash[dir].tuple.src.u3},
			    { 0 }, PF_INET },
			  { ct->tuplehash[dir].tuple.dst.u3,
			    { .udp = { .port = htons(talk_port) } },
			    IPPROTO_UDP }});
		exp->mask = ((struct nf_conntrack_tuple)
			{ { 0xFFFFFFFF, { 0 },{0xFFFF} },
			  { 0xFFFFFFFF, { .udp = { .port = 0xFFFF } }, 0xFF }});*/
		cmd.l3num = nf_ct_l3num(ct);
		 memcpy(cmd.u3.all, &ct->tuplehash[dir].tuple.src.u3.all,
				 sizeof(cmd.u3.all));

		nf_ct_expect_init(exp, NF_CT_EXPECT_CLASS_DEFAULT, cmd.l3num,
				&ct->tuplehash[dir].tuple.src.u3,
				&ct->tuplehash[dir].tuple.dst.u3,
				IPPROTO_UDP, NULL, (__be16 *)&exp_talk_port);

        /*        exp->tuple.src.u3 =  ct->tuplehash[dir].tuple.src.u3;              
                exp->mask.src.u3.ip = 0xffffffff;*/
                exp->tuple.src.u.udp.port = 0 ;
                exp->mask.src.u.udp.port = 0;
                /*
                exp->tuple.dst.u3 = ct->tuplehash[dir].tuple.dst.u3;
                exp->mask.src.u3.ip = 0xffffffff;
		exp->tuple.dst.u.udp.port =  htons(talk_port);		
                exp->mask.dst.u.udp.port = 0xffff;
                exp->tuple.dst.protonum = IPPROTO_UDP ;
                exp->mask.dst.protonum = 0xff;
		*/

		exp->expectfn = talk_expectfn[talk_port - TALK_PORT];
	        exp->master = ct ; // added

		DEBUGP("ip_ct_talk_help_response: callee client %u.%u.%u.%u:%u -> caller daemon %u.%u.%u.%u:%u!\n",
		       NIPQUAD(exp->tuple.src.u3.ip), ntohs(exp->tuple.src.u.udp.port),
		       NIPQUAD(exp->tuple.dst.u3.ip), ntohs(exp->tuple.dst.u.udp.port));

		/* Ignore failure; should only happen with NAT */
		//ip_conntrack_expect_related(ct, &expect);

		talk_help_resp = rcu_dereference(nf_nat_talk_resp_hook);

		 if (talk_help_resp)
                        ret = talk_help_resp(pskb, exp, type, answer,
                                                    addr);
                else if (nf_ct_expect_related(exp) != 0) {
//                        ip_conntrack_expect_free(exp);
                        ret = NF_DROP;
                }


	//	UNLOCK_BH(&ip_talk_lock);
		spin_unlock_bh(&ip_talk_lock);

	}
	if (type == LOOK_UP) {

		DEBUGP("ip_ct_talk_help_response: LOOK_UP\n");

		/* update the talk info */
	//	LOCK_BH(&ip_talk_lock);
		spin_lock_bh(&ip_talk_lock);

		exp_talk_port = htons(addr->ta_port);

		/* expect callee client -> caller client connection */
		/*exp->tuple = ((struct nf_conntrack_tuple)
			{ { {.u3 = ct->tuplehash[!dir].tuple.src.u3},
			    { 0 },PF_INET },
			  { {.ip = addr->ta_addr},
			    { .udp = { .port = addr->ta_port } },
			    IPPROTO_UDP }});
		exp->mask = ((struct nf_conntrack_tuple)
			{ { 0xFFFFFFFF, { 0 },{0xFFFF} },
			  { 0xFFFFFFFF, { .udp = { .port = 0xFFFF } }, 0xFF }});*/

		cmd.l3num = nf_ct_l3num(ct);
		memcpy(cmd.u3.all, &ct->tuplehash[dir].tuple.src.u3.all,
				sizeof(cmd.u3.all));
		nf_ct_expect_init(exp, NF_CT_EXPECT_CLASS_DEFAULT, cmd.l3num,
				&ct->tuplehash[dir].tuple.src.u3,
				(union nf_inet_addr *)&addr->ta_addr,
				IPPROTO_UDP, NULL, (__be16 *)&exp_talk_port);
		/*
                exp->tuple.src.u3 =  ct->tuplehash[!dir].tuple.src.u3;
                exp->mask.src.u3.ip = 0xffffffff;*/
                exp->tuple.src.u.udp.port = 0 ;
                exp->mask.src.u.udp.port = 0;
/*
                exp->tuple.dst.u3.ip = addr->ta_addr;
                exp->mask.src.u3.ip = 0xffffffff;
                exp->tuple.dst.u.udp.port =  addr->ta_port;
                exp->mask.dst.u.udp.port = 0xffff;
                exp->tuple.dst.protonum = IPPROTO_UDP ;
                exp->mask.dst.protonum = 0xff;
*/
		
		exp->expectfn = NULL;
		exp->master= ct;
		DEBUGP("ip_ct_talk_help_response: callee client %u.%u.%u.%u:%u -> caller client %u.%u.%u.%u:%u!\n",
		       NIPQUAD(exp->tuple.src.u3.ip), ntohs(exp->tuple.src.u.tcp.port),
		       NIPQUAD(exp->tuple.dst.u3.ip), ntohs(exp->tuple.dst.u.tcp.port));

		/* Ignore failure; should only happen with NAT */
		//ip_conntrack_expect_related(ct, &expect);

                talk_help_resp = rcu_dereference(nf_nat_talk_resp_hook);
         
       		if (talk_help_resp)
                        ret = talk_help_resp(pskb, exp, type, answer,
                                                    addr);
                else if (nf_ct_expect_related(exp) != 0) {
//                        ip_conntrack_expect_free(exp);
                        ret = NF_DROP;
                }


	//	UNLOCK_BH(&ip_talk_lock);
       spin_unlock_bh(&ip_talk_lock);

	}
		    
	return NF_ACCEPT;
}

/* FIXME: This should be in userspace.  Later. */
 static int talk_help(struct sk_buff **pskb,
		     struct nf_conn *ct,
		     enum ip_conntrack_info ctinfo,
		     int talk_port,
		     u_char mode, unsigned int protoff)
{
	//struct udphdr *udph = (void *)iph + iph->ihl * 4;
	//const char *data = (const char *)udph + sizeof(struct udphdr);
     //    int ret;

        unsigned int dataoff;
        struct udphdr udph, *uh;
        char *tb_ptr, *data;

        typeof(nf_nat_talk_msg_hook) talk_help_msg ; 
	int dir = CTINFO2DIR(ctinfo);
	size_t udplen;

	DEBUGP("ip_ct_talk_help: help entered\n");

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED
	    && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		DEBUGP("ip_ct_talk_help: Conntrackinfo = %u\n", ctinfo);
		return NF_ACCEPT;
	}

	/* Not whole UDP header? */
/*	udplen = len - iph->ihl * 4;
	if (udplen < sizeof(struct udphdr)) {
		DEBUGP("ip_ct_talk_help: too short for udph, udplen = %u\n", (unsigned)udplen);
		return NF_ACCEPT;
	}*/

	/* Checksum invalid?  Ignore. */
	/* FIXME: Source route IP option packets --RR */
	/*if (csum_tcpudp_magic(iph->saddr, iph->daddr, udplen, IPPROTO_UDP,
			      csum_partial((char *)udph, udplen, 0))) {
		DEBUGP("ip_ct_talk_help: bad csum: %p %u %u.%u.%u.%u %u.%u.%u.%u\n",
		       udph, udplen, NIPQUAD(iph->saddr),
		       NIPQUAD(iph->daddr));
		return NF_ACCEPT;
	}*/
	

 	  /* Not whole UDP header? */
        uh = skb_header_pointer(*pskb, protoff,
                                sizeof(udph), &udph);
        if (uh == NULL) {
                DEBUGP("ip_ct_talk_help: short for udph\n");
                return NF_ACCEPT;
        }

        udplen = (*pskb)->len - protoff ; //(*pskb)->nh.iph->ihl*4;
        dataoff = protoff + sizeof(udph);
        if (dataoff >= (*pskb)->len)
                return NF_ACCEPT;

       // LOCK_BH(&talk_buffer_lock);
        tb_ptr = skb_header_pointer(*pskb, dataoff,
                                    (*pskb)->len - dataoff, talk_buffer);
        BUG_ON(tb_ptr == NULL);

        data = tb_ptr;

      talk_help_msg = rcu_dereference(nf_nat_talk_msg_hook);

	DEBUGP("ip_ct_talk_help: %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n",
		NIPQUAD(iph->saddr), ntohs(udph->source), NIPQUAD(iph->daddr), ntohs(udph->dest));

	if (dir == IP_CT_DIR_ORIGINAL && talk_help_msg)
        {
	    if (talk_port == TALK_PORT) {
                        if (udplen == sizeof(struct udphdr) +
                                        sizeof(struct talk_msg)) {
                                struct talk_msg *tm = (struct talk_msg *)data;
                                return talk_help_msg(pskb, ct, tm->type,
                                                &tm->addr, &tm->ctl_addr);
                        }
                } else {
                        if (ntalk &&
                            udplen == sizeof(struct udphdr) +
                                        sizeof(struct ntalk_msg) &&
                            ((struct ntalk_msg *)data)->vers == NTALK_VERSION){
                                struct ntalk_msg *tm = (struct ntalk_msg *)data;
                                return talk_help_msg(pskb, ct, tm->type,
                                                &tm->addr, &tm->ctl_addr);
                        } else if (ntalk2 &&
                                   udplen >= sizeof(struct udphdr) +
                                                sizeof(struct ntalk2_msg) &&
                                   ((struct ntalk2_msg *)data)->vers == NTALK2_VERSION &&
                                   udplen == sizeof(struct udphdr)
                                             + sizeof(struct ntalk2_msg)
                                             + ((struct ntalk2_msg *)data)->extended) {
                                struct ntalk2_msg *tm = (struct ntalk2_msg *)data;
                                return talk_help_msg(pskb, ct, tm->type,
                                                &tm->addr, &tm->ctl_addr);
                        }
                }

        	return NF_ACCEPT;
        }
		
	if (talk_port == TALK_PORT
	    && udplen == sizeof(struct udphdr) + sizeof(struct talk_response))
		return talk_help_response(pskb, ct, ctinfo, talk_port, mode,
					  ((struct talk_response *)data)->type, 
					  ((struct talk_response *)data)->answer,
					  &(((struct talk_response *)data)->addr));
	else if (talk_port == NTALK_PORT
	 	  && ntalk
		  && udplen == sizeof(struct udphdr) + sizeof(struct ntalk_response)
		  && ((struct ntalk_response *)data)->vers == NTALK_VERSION)
		return talk_help_response(pskb, ct, ctinfo, talk_port, mode,
					  ((struct ntalk_response *)data)->type, 
					  ((struct ntalk_response *)data)->answer,
					  &(((struct ntalk_response *)data)->addr));
	else if (talk_port == NTALK_PORT
		 && ntalk2
		 && udplen >= sizeof(struct udphdr) + sizeof(struct ntalk2_response)
		 && ((struct ntalk2_response *)data)->vers == NTALK2_VERSION)
		return talk_help_response(pskb, ct, ctinfo, talk_port, mode,
					  ((struct ntalk2_response *)data)->type, 
					  ((struct ntalk2_response *)data)->answer,
					  &(((struct ntalk2_response *)data)->addr));
	else {
		DEBUGP("ip_ct_talk_help: not ntalk/ntalk2 response, datalen %u != %u or %u + max 256\n", 
		       (unsigned)udplen - sizeof(struct udphdr), 
		       sizeof(struct ntalk_response), sizeof(struct ntalk2_response));
		return NF_ACCEPT;
	}	
}

int lookup_help(struct sk_buff *skb, unsigned int protoff,
		       struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	 struct sk_buff *pskb;
	 pskb = (struct sk_buff *)skb;
	return talk_help(&pskb, ct, ctinfo, TALK_PORT, LOOK_UP, protoff);
}

int lookup_nhelp(struct sk_buff *skb, unsigned int protoff,
		        struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	struct sk_buff *pskb;
	pskb = (struct sk_buff *)skb;
	return talk_help(&pskb, ct, ctinfo, NTALK_PORT, LOOK_UP, protoff);
}

#if 0
static struct nf_conntrack_helper lookup_helpers[2] = 
	{ { { NULL, NULL },
	    "talk",					/* name */
	 //   0,						/* flags */
	    NULL,					/* module */
	    1,						/* max_expected */
	    240,					/* timeout */
            { { { 0}, { .udp = { .port = __constant_htons(TALK_PORT) } },PF_INET },	/* tuple */
	      { {0}, { 0 }, IPPROTO_UDP } },
	    { { { 0}, { .udp = { .port = 0xFFFF } },0xFFFF },			/* mask */
	      { {0}, { 0 }, 0xFF } },
	    lookup_help },				/* helper */
          { { NULL, NULL },
            "ntalk",					
	   // 0,						
	    NULL,					
	    1,						
	    240,					
	    { { { 0}, { .udp = { .port = __constant_htons(NTALK_PORT) } },PF_INET },	
	       { { 0}, { 0 }, IPPROTO_UDP  } },
	    { { { 0}, { .udp = { .port = 0xFFFF } },0xFFFF },			
	      { { 0}, { 0 }, 0xFF } },
    	    lookup_nhelp }				
        };
#endif
static const struct nf_conntrack_expect_policy talk_exp_policy = {
	.max_expected   = 1,
	.timeout        = 240,
};

struct nf_conntrack_helper lookup_helpers[2] =
{
{

        .name                   = "talk",
        .me                     = NULL,
	.expect_policy 		= &talk_exp_policy,
        .tuple.src.l3num        = PF_INET,
        .tuple.src.u.udp.port   = __constant_htons(TALK_PORT),
        .tuple.dst.protonum     = IPPROTO_UDP,
        .tuple.dst.u.udp.port   = 0,
       // .mask.src.l3num         = 0xffff,
        //.mask.src.u.udp.port    = __constant_htons(0xffff),
        //.mask.dst.protonum      = 0xff,
        //.mask.dst.u.udp.port    = 0xffff,
        .help                   = lookup_help,


},

{
	.name                   = "ntalk",
        .me                     = NULL,
	.expect_policy          = &talk_exp_policy,
        .tuple.src.l3num        = PF_INET,
        .tuple.src.u.udp.port   = __constant_htons(NTALK_PORT),
        .tuple.dst.protonum     = IPPROTO_UDP,
        .tuple.dst.u.udp.port   = 0,
        //.mask.src.l3num         = 0xffff,
        //.mask.src.u.udp.port    = __constant_htons(0xffff),
        //.mask.dst.protonum      = 0xff,
        //.mask.dst.u.udp.port    = 0xffff,
        .help                   = lookup_nhelp,



}
};

void talk_expect(struct nf_conn *ct,struct nf_conntrack_expect *exp)
{

     struct nf_conn_help *ct_help = nfct_help(ct);
	 
	DEBUGP("ip_conntrack_talk: calling talk_expectfn for ct %p\n", ct);
	
	ct_help->helper = &lookup_helpers[0];
	
	      

	//return NF_ACCEPT;       /* unused */
}

void ntalk_expect(struct nf_conn *ct, struct nf_conntrack_expect *exp)
{

    struct nf_conn_help *ct_help = nfct_help(ct);

	DEBUGP("ip_conntrack_talk: calling ntalk_expectfn for ct %p\n", ct);

   ct_help->helper = &lookup_helpers[1];
	 
	//return NF_ACCEPT;       /* unused */
}

static int help(struct sk_buff **pskb,unsigned int protoff,
		struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	return talk_help(pskb, ct, ctinfo, TALK_PORT, ANNOUNCE, protoff);
}

static int nhelp(struct sk_buff **pskb,unsigned int protoff,
		 struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	return talk_help(pskb, ct, ctinfo, NTALK_PORT, ANNOUNCE, protoff);
}

#if 0
static struct nf_conntrack_helper talk_helpers[2] = 
	{ { { NULL, NULL },
	    "talk",					/* name */
	    0,						/* flags */
	    THIS_MODULE,				/* module */
	    1,						/* max_expected */
	    240,					/* timeout */
	    { { .ip =  0 , {.udp = { .port = __constant_htons(TALK_PORT) } },PF_INET },	/* tuple */
 	      { .ip = 0 , { 0 }, IPPROTO_UDP } },
	    { { .ip = 0 , { .udp = { .port = 0xFFFF } },0xFFFF },			/* mask */
	      { .ip = 0 , { 0 }, 0xFF } },
	    help },					/* helper */
          { { NULL, NULL },
	    "ntalk",					/* name */
	    0,						/* flags */
	    THIS_MODULE,				/* module */
	    1,						/* max_expected */
	    240,					/* timeout */
	    { { .ip = 0  , { .udp = { .port = __constant_htons(NTALK_PORT) } },PF_INET },	/* tuple */
	      { .ip=0 , { 0 }, IPPROTO_UDP } },
	    { { .ip =  0, { .udp = { .port = 0xFFFF } },0xFFFF },			/* mask */
	      { .ip= 0, { 0 }, 0xFF } },
	    nhelp }					/* helper */
	};

#endif

static struct nf_conntrack_helper talk_helpers[2] = {
       {
        .name                   = "talk",
        .me                     = THIS_MODULE,
	.expect_policy 		= &talk_exp_policy,
        .tuple.src.l3num        = PF_INET,
        .tuple.src.u.udp.port   = __constant_htons(TALK_PORT),
        .tuple.dst.protonum     = IPPROTO_UDP,
        .tuple.dst.u.udp.port   = 0,
  //      .mask.src.l3num         = 0xffff,
    //    .mask.src.u.udp.port    = __constant_htons(0xffff),
     //   .mask.dst.protonum      = 0xff,
      //  .mask.dst.u.udp.port    = 0xffff, 
        .help                   = help,
        
        },
        {
 	.name                   = "ntalk",
        .me                     = THIS_MODULE,
	.expect_policy          = &talk_exp_policy,
        .tuple.src.l3num        = PF_INET,
        .tuple.src.u.udp.port   = __constant_htons(NTALK_PORT),
        .tuple.dst.protonum     = IPPROTO_UDP,
        .tuple.dst.u.udp.port   = 0,
       // .mask.src.l3num         = 0xffff,
       // .mask.src.u.udp.port    = __constant_htons(0xffff),
       // .mask.dst.protonum      = 0xff,
       // .mask.dst.u.udp.port    = 0xffff,
        .help                   = nhelp,

        }
};


static int __init init(void)
{
	if (talk > 0)
	{
		printk("tuple->src.u.port[0]=%d\n",talk_helpers[0].tuple.src.u.udp.port);
		nf_conntrack_helper_register(&talk_helpers[0]);
	}
	if (ntalk > 0 || ntalk2 > 0)
	{
		printk("tuple->src.u.port[1]=%d\n",talk_helpers[1].tuple.src.u.udp.port);
		nf_conntrack_helper_register(&talk_helpers[1]);
	}
		
	return 0;
}

static void __exit fini(void)
{
	if (talk > 0)
		nf_conntrack_helper_unregister(&talk_helpers[0]);
	if (ntalk > 0 || ntalk2 > 0)
		nf_conntrack_helper_unregister(&talk_helpers[1]);
}

EXPORT_SYMBOL(ip_talk_lock);

module_init(init);
module_exit(fini);

