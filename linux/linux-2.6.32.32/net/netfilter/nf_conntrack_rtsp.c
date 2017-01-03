/*
 * RTSP extension for IP connection tracking
 * (C) 2003 by Tom Marshall <tmarshall@real.com>
 * based on ip_conntrack_irc.c
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 *
 * Module load syntax:
 *   insmod ip_conntrack_rtsp.o ports=port1,port2,...port<MAX_PORTS>
 *                              max_outstanding=n setup_timeout=secs
 *
 * If no ports are specified, the default will be port 554.
 *
 * With max_outstanding you can define the maximum number of not yet
 * answered SETUP requests per RTSP session (default 8).
 * With setup_timeout you can specify how long the system waits for
 * an expected data channel (default 300 seconds).
 */


#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <net/tcp.h>

//#include <linux/netfilter_ipv4/lockhelp.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/netfilter/nf_conntrack_rtsp.h>
#include <net/netfilter/nf_conntrack_expect.h>

#include <linux/ctype.h>
#define NF_NEED_STRNCASECMP
#define NF_NEED_STRTOU16
#define NF_NEED_STRTOU32
#define NF_NEED_NEXTLINE
#include <linux/netfilter_helpers.h>
#define NF_NEED_MIME_NEXTLINE
#include <linux/netfilter_mime.h>

#define IFX_RTSP_NAT_ALG
#undef IFX_RTSP_NAT_ALG
#define IFX_TEST
#undef IFX_TEST

#ifdef IFX_RTSP_NAT_ALG
//#include <linux/netfilter_ipv4/lockhelp.h>

//DECLARE_RWLOCK_EXTERN(ip_rtsp_rwlock);
//#define ASSERT_READ_LOCK MUST_BE_READ_LOCKED(&ip_rtsp_rwlock);
//#define ASSERT_WRITE_LOCK MUST_BE_WRITE_LOCKED(&ip_rtsp_rwlock);

 extern rwlock_t ip_rtsp_rwlock ;
 #include <linux/netfilter_ipv4/listhelp.h>
//#include "rtsp_conn_alg.h"
#include <linux/netfilter/ifx_alg.h>

#endif

#define MAX_SIMUL_SETUP 8 /* XXX: use max_outstanding */

//#define IP_NF_RTSP_DEBUG
#undef IP_DYNAMIC_RTSP_DEBUG

#define INFOP(args...) printk(KERN_INFO __FILE__ ":" __FUNCTION__ ":" args)

#ifdef IP_DYNAMIC_RTSP_DEBUG
#define NCDEBUG(format,args...) printk(" \n Neeraj_CONN " ": " __FILE__  " : " __FUNCTION__ " : "format,##args)
#else
#define NCDEBUG(format,args...)
#endif

#ifdef IP_NF_RTSP_DEBUG
//#define DEBUGP(args...) printk(KERN_DEBUG __FILE__ ":" __FUNCTION__ ":" args)
#define DEBUGP(args...) printk(args)
#else
#define DEBUGP(args...)
//#define DEBUGP printk
#endif

//#define DEBUGP printk

#define MAX_PORTS 8
#ifndef IFX_RTSP_NAT_ALG

static int ports[MAX_PORTS];
static int num_ports = 0;
#endif

static int max_outstanding = 8;
static unsigned int setup_timeout =100; //300;


MODULE_AUTHOR("Tom Marshall <tmarshall@real.com>");
MODULE_DESCRIPTION("RTSP connection tracking module");
MODULE_LICENSE("GPL");

#ifdef MODULE_PARM

#ifndef IFX_RTSP_NAT_ALG
MODULE_PARM(ports, "1-" __MODULE_STRING(MAX_PORTS) "i");
MODULE_PARM_DESC(ports, "port numbers of RTSP servers");
MODULE_PARM(max_outstanding, "i");
MODULE_PARM_DESC(max_outstanding, "max number of outstanding SETUP requests per RTSP session");
MODULE_PARM(setup_timeout, "i");
MODULE_PARM_DESC(setup_timeout, "timeout on for unestablished data channels");
#endif

#endif


//DECLARE_LOCK(ip_rtsp_lock);

DEFINE_SPINLOCK(ip_rtsp_lock);

#ifndef  IFX_RTSP_NAT_ALG
struct module* nf_conntrack_rtsp = THIS_MODULE;
#endif


/* protects rtsp part of conntracks */

//DECLARE_LOCK_EXTERN(ip_rtsp_lock);

 extern spinlock_t ip_rtsp_lock;
 
// 6th july DECLARE_RWLOCK(ip_rtsp_rwlock);


/*
 * Max mappings we will allow for one RTSP connection (for RTP, the number
 * of allocated ports is twice this value).  Note that SMIL burns a lot of
 * ports so keep this reasonably high.  If this is too low, you will see a
 * lot of "no free client map entries" messages.
 */
#define MAX_PORT_MAPS 16

#ifndef IFX_RTSP_NAT_ALG
static char rtsp_buffer[65536];
//static DEFINE_SPINLOCK(rtsp_buffer_lock);
unsigned int (*nf_nat_rtsp_hook)(struct sk_buff **pskb,
                                 enum ip_conntrack_info ctinfo,
				 unsigned int matchoff, unsigned int matchlen,
                                 struct nf_ct_rtsp_expect *expinfo,
                                 struct nf_conntrack_expect *exp);

void (*nf_nat_rtsp_hook_expectfn)(struct nf_conn *ct, struct nf_conntrack_expect *exp);

extern void nf_ct_expect_insert_rtcp (struct nf_conntrack_expect *exp) ;

EXPORT_SYMBOL_GPL(nf_nat_rtsp_hook);
#endif

#ifdef IFX_RTSP_NAT_ALG

static int rtsp_conn_device_open(struct inode *,struct file *);

int rtsp_conn_device_ioctl(struct inode *,struct file *,unsigned int ,unsigned long); 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static int rtsp_conn_device_release(struct inode *,struct file *);
#else
static void rtsp_conn_device_release(struct inode *,struct file *);
#endif

static int rtsp_conn_device_port_deregister(struct rtsp_params *);

static int rtsp_conn_device_port_register(struct rtsp_params *);


struct file_operations rtsp_Fops_conn = {
llseek: NULL,   /* seek */
read: NULL, 
write: NULL,
readdir: NULL,   /* readdir */
poll: NULL,   /* select */
ioctl: rtsp_conn_device_ioctl,   /* ioctl */
mmap: NULL,   /* mmap */
open: rtsp_conn_device_open,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
flush: NULL,  /* flush */
#endif
release: rtsp_conn_device_release  /* a.k.a. close */
};

struct list_head rtsp_conn_registration_list;/* stores the registered port and protocols*/


/* initialize the list */
LIST_HEAD(rtsp_conn_registration_list);

#define BUF_LEN 80
static int Device_Open = 0;
static char Message[BUF_LEN];
static char *Message_Ptr;

#endif

/*** default port list was here in the masq code: 554, 3030, 4040 ***/

#define SKIP_WSPACE(ptr,len,off) while(off < len && isspace(*(ptr+off))) { off++; }

/*
 * Parse an RTSP packet.
 *
 * Returns zero if parsing failed.
 *
 * Parameters:
 *  IN      ptcp        tcp data pointer
 *  IN      tcplen      tcp data len
 *  IN/OUT  ptcpoff     points to current tcp offset
 *  OUT     phdrsoff    set to offset of rtsp headers
 *  OUT     phdrslen    set to length of rtsp headers
 *  OUT     pcseqoff    set to offset of CSeq header
 *  OUT     pcseqlen    set to length of CSeq header
 */
static int
rtsp_parse_message(char* ptcp, uint tcplen, uint* ptcpoff,
		uint* phdrsoff, uint* phdrslen,
		uint* pcseqoff, uint* pcseqlen)
{
	uint    entitylen = 0;
	uint    lineoff;
	uint    linelen;

	if (!nf_nextline(ptcp, tcplen, ptcpoff, &lineoff, &linelen))
	{
		return 0;
	}

	*phdrsoff = *ptcpoff;
	while (nf_mime_nextline(ptcp, tcplen, ptcpoff, &lineoff, &linelen))
	{
		if (linelen == 0)
		{
			if (entitylen > 0)
			{
				*ptcpoff += min(entitylen, tcplen - *ptcpoff);
			}
			break;
		}
		if (lineoff+linelen > tcplen)
		{
			//INFOP("!! overrun !!\n");
			break;
		}

		if (nf_strncasecmp(ptcp+lineoff, "CSeq:", 5) == 0)
		{
			*pcseqoff = lineoff;
			*pcseqlen = linelen;
		}
		if (nf_strncasecmp(ptcp+lineoff, "Content-Length:", 15) == 0)
		{
			uint off = lineoff+15;
			SKIP_WSPACE(ptcp+lineoff, linelen, off);
			nf_strtou32(ptcp+off, &entitylen);
		}
	}
	*phdrslen = (*ptcpoff) - (*phdrsoff);

	return 1;
}

#ifdef CONFIG_IFX_ALG_QOS

static void rtsp_expect(struct nf_conn *ct, struct nf_conntrack_expect *this)
{
	DEBUGP("rtsp_expect: helper for %p added\n", ct);

	// Tranport protocol helper
	ct->ifx_alg_qos_mark = IFX_ALG_APP_RTSP | IFX_ALG_PROTO_RTP ;

	DEBUGP("rtsp_expect: helper for %p added and ifx_alg_qos_mark is %x\n", ct,ct->ifx_alg_qos_mark);

	//return NF_ACCEPT;   /* unused */
}
#endif

/*
 * Find lo/hi client ports (if any) in transport header
 * In:
 *   ptcp, tcplen = packet
 *   tranoff, tranlen = buffer to search
 *
 * Out:
 *   pport_lo, pport_hi = lo/hi ports (host endian)
 *
 * Returns nonzero if any client ports found
 *
 * Note: it is valid (and expected) for the client to request multiple
 * transports, so we need to parse the entire line.
 */
static int
rtsp_parse_transport(char* ptran, uint tranlen,
		struct nf_ct_rtsp_expect* prtspexp)
{
	int     rc = 0;
	uint    off = 0;


	if (tranlen < 10 || !iseol(ptran[tranlen-1]) ||
			nf_strncasecmp(ptran, "Transport:", 10) != 0)
	{
		//INFOP("sanity check failed\n");
		return 0;
	}
	DEBUGP("tran='%.*s'\n", (int)tranlen, ptran);
	off += 10;
	SKIP_WSPACE(ptran, tranlen, off);

	/* Transport: tran;field;field=val,tran;field;field=val,... */
	while (off < tranlen)
	{
		const char* pparamend;
		uint        nextparamoff;

		pparamend = memchr(ptran+off, ',', tranlen-off);
		pparamend = (pparamend == NULL) ? ptran+tranlen : pparamend+1;
		nextparamoff = pparamend-ptran;

		while (off < nextparamoff)
		{
			const char* pfieldend;
			uint        nextfieldoff;

			pfieldend = memchr(ptran+off, ';', nextparamoff-off);
			nextfieldoff = (pfieldend == NULL) ? nextparamoff : pfieldend-ptran+1;

			if (strncmp(ptran+off, "client_port=", 12) == 0)
			{
				u_int16_t   port;
				uint        numlen;

				off += 12;
				numlen = nf_strtou16(ptran+off, &port);
				off += numlen;
				//for testing 
			//	prtspexp->pbtype = pb_single;
				
				if (prtspexp->loport != 0 && prtspexp->loport != port)
				{
					DEBUGP("multiple ports found, port %hu ignored\n", port);
				}
				else
				{
					prtspexp->loport = prtspexp->hiport = port;
					if (ptran[off] == '-')
					{
						off++;
						numlen = nf_strtou16(ptran+off, &port);
						off += numlen;
						prtspexp->pbtype = pb_range;
						prtspexp->hiport = port;
                       
					    //printk ("\n INSIDE the RANGE \n ");
						// If we have a range, assume rtp:
						// loport must be even, hiport must be loport+1
						if ((prtspexp->loport & 0x0001) != 0 ||
								prtspexp->hiport != prtspexp->loport+1)
						{
							DEBUGP("incorrect range: %hu-%hu, correcting\n",
									prtspexp->loport, prtspexp->hiport);
							prtspexp->loport &= 0xfffe;
							prtspexp->hiport = prtspexp->loport+1;
						}
					}
					else if (ptran[off] == '/')
					{
						off++;
						numlen = nf_strtou16(ptran+off, &port);
						off += numlen;
						prtspexp->pbtype = pb_discon;
						prtspexp->hiport = port;
					}
					rc = 1;
				}
			}

			/*
			 * Note we don't look for the destination parameter here.
			 * If we are using NAT, the NAT module will handle it.  If not,
			 * and the client is sending packets elsewhere, the expectation
			 * will quietly time out.
			 */

			off = nextfieldoff;
		}

		off = nextparamoff;
	}


	return rc;
}

/*
 * Find lo/hi server and client ports (if any) in transport header
 * In:
 *   ptcp, tcplen = packet
 *   tranoff, tranlen = buffer to search
 *
 * Out:
 *   pport_lo, pport_hi = lo/hi ports (host endian)
 *
 * Returns nonzero if any client ports found
 *
 * Note: it is valid (and expected) for the client to request multiple
 * transports, so we need to parse the entire line.
*/

#if 0
static int
rtsp_parse_transport_reply (char* ptran, uint tranlen,
		struct nf_ct_rtsp_expect* prtspexp)
{
	int     rc = 0;
	uint    off = 0;

	if (tranlen < 10 || !iseol(ptran[tranlen-1]) ||
			nf_strncasecmp(ptran, "Transport:", 10) != 0)
	{
		//INFOP("sanity check failed\n");
		return 0;
	}
	DEBUGP("tran='%.*s'\n", (int)tranlen, ptran);
	off += 10;
	SKIP_WSPACE(ptran, tranlen, off);

	/* Transport: tran;field;field=val,tran;field;field=val,... */
	while (off < tranlen)
	{
		const char* pparamend;
		uint        nextparamoff;

		pparamend = memchr(ptran+off, ',', tranlen-off);
		pparamend = (pparamend == NULL) ? ptran+tranlen : pparamend+1;
		nextparamoff = pparamend-ptran;

		DEBUGP ("[435] Default Streaming using RTP..\n");
		prtspexp->rtsp_protocol = IP_CT_RTSP_RTP;

		while (off < nextparamoff)
		{
			const char* pfieldend;
			uint        nextfieldoff;

			pfieldend = memchr(ptran+off, ';', nextparamoff-off);
			nextfieldoff = (pfieldend == NULL) ? nextparamoff : pfieldend-ptran+1;


			if (strncmp(ptran+off, "x-real-rdt/udp", 14) == 0)
			{
				DEBUGP ("[491] Streaming using RDT..\n");
				prtspexp->rtsp_protocol = IP_CT_RTSP_RDT;
			}

			/*
			 * Note we don't look for the destination parameter here.
			 * If we are using NAT, the NAT module will handle it.  If not,
			 * and the client is sending packets elsewhere, the expectation
			 * will quietly time out.
			 */

			off = nextfieldoff;
		}

		off = nextparamoff;
	}

	return rc;
}

#endif

/* Expect for RTCP port */
static void expected(struct nf_conn *ct, struct nf_conntrack_expect *exp)                                                                            
{                                                                                                                                                    
    if(nf_nat_rtsp_hook_expectfn) {                                                                                                                  
        nf_nat_rtsp_hook_expectfn(ct,exp);                                                                                                           
    }                                                                                                                                                
}                                                                                                                                                    
  


/*** conntrack functions ***/

/* outbound packet: client->server */
int
help_out(struct sk_buff *skb,unsigned char *rb_ptr,unsigned int datalen,
		struct nf_conn* ct, enum ip_conntrack_info ctinfo)
{
	int dir = CTINFO2DIR(ctinfo);   /* = IP_CT_DIR_ORIGINAL */
	//struct  tcphdr* tcph = (void*)iph + iph->ihl * 4;
	//uint    tcplen = pktlen - iph->ihl * 4;
	//char*   pdata = (char*)tcph + tcph->doff * 4;
	//uint    datalen = tcplen - tcph->doff * 4
      struct iphdr* iph = ip_hdr(skb); //(struct iphdr*)(*pskb)->nh.iph;
      struct tcphdr* tcph = (struct tcphdr*)((void*)iph + iph->ihl*4);

	struct nf_ct_rtsp_expect expinfo;
	uint    dataoff = 0;
        char *pdata = rb_ptr;
	struct nf_conntrack_expect *exp, *exp1;
         int ret = NF_ACCEPT;
	typeof(nf_nat_rtsp_hook) nf_nat_rtsp;

	struct sk_buff *pskb;
         struct nf_conn *ct1;                                                                                                             
         u_int32_t   newip1;                                                                                                                   


   DEBUGP("dir %u\n", dir);

#ifdef CONFIG_IFX_ALG_QOS //  Suresh


	ct->ifx_alg_qos_mark = IFX_ALG_APP_RTSP;
	DEBUGP ("\nRTSP_ALG: help_out marked ct->ifx_alg_qos_mark to : %x ***\n",
							                                ct->ifx_alg_qos_mark );
#endif

	while (dataoff < datalen)
	{
		uint    cmdoff = dataoff;
		uint    hdrsoff = 0;
		uint    hdrslen = 0;
		uint    cseqoff = 0;
		uint    cseqlen = 0;
		uint    lineoff = 0;
		uint    linelen = 0;
		uint    off;
	//	int     rc;

		if (!rtsp_parse_message(pdata, datalen, &dataoff,
					&hdrsoff, &hdrslen,
					&cseqoff, &cseqlen))
		{
			break;      /* not a valid message */
		}

		if (strncmp(pdata+cmdoff, "SETUP ", 6) != 0)
		{
			continue;   /* not a SETUP message */
		}
		DEBUGP("found a setup message\n");

		memset(&expinfo, 0, sizeof(expinfo));

		off = 0;
		while (nf_mime_nextline(pdata+hdrsoff, hdrslen, &off,
					&lineoff, &linelen))
		{
			if (linelen == 0)
			{
				break;
			}
			if (off > hdrsoff+hdrslen)
			{
				//INFOP("!! overrun !!");
				break;
			}

			if (nf_strncasecmp(pdata+hdrsoff+lineoff, "Transport:", 10) == 0)
			{
				rtsp_parse_transport(pdata+hdrsoff+lineoff, linelen,
						&expinfo);
			}
		}

		if (expinfo.loport == 0)
		{
			printk("no udp transports found\n");
			continue;   /* no udp transports found */
		}

		DEBUGP("udp transport found, ports=(%d,%hu,%hu) \n",
				(int)expinfo.pbtype,
				expinfo.loport,
				expinfo.hiport);


		exp = nf_ct_expect_alloc(ct);
		        if (!exp) {
                		ret = NF_DROP;
		                goto out;
			         }


		//LOCK_BH(&ip_rtsp_lock);
		spin_lock_bh(&ip_rtsp_lock);
	
		nf_ct_expect_init(exp, NF_CT_EXPECT_CLASS_DEFAULT,
				ct->tuplehash[!dir].tuple.src.l3num,
				&ct->tuplehash[!dir].tuple.src.u3,
				&ct->tuplehash[!dir].tuple.dst.u3,
				IPPROTO_UDP, NULL, &(expinfo.loport));

		 //exp->id = ntohl(tcph->seq) + hdrsoff; /* mark all the headers */ //added
		expinfo.len = hdrslen; // added
        	exp->master = ct ;

		exp->tuple.src.u3 = ct->tuplehash[!dir].tuple.src.u3;
	   //	exp->mask.src.u3.ip  = 0xffffffff; //orginal
	  
	    //exp->timeout = time_out;
	   // exp->mask.src.u3.ip  = 0;
	//	exp->timeout.data = (unsigned long)exp;
	    //exp->timeout.function = expectation_timed_out;
	  //  exp->timeout.expires = jiffies + time_out * HZ;
				

	   
	    exp->tuple.src.u.tcp.port = 0;
//		exp->mask.src.u.udp.port  = 0 ; 
		
	    exp->tuple.src.l3num = PF_INET ;
		//exp->mask.src.u.all = 0xffff;
		
		exp->tuple.dst.u3 = ct->tuplehash[dir].tuple.src.u3;
		//exp->mask.dst.u3.ip  = 0xffffffff;
	//	exp->tuple.dst.u.udp.port = expinfo.loport; //check
	//	exp->mask.dst.u.udp.port  = (expinfo.pbtype == pb_range) ? 0xfffe : 0xffff; //check
		//exp->mask.dst.u.udp.port  = 0xffff;

		if (expinfo.pbtype == pb_range) {
		//	printk("Changing expectation mask to handle multiple ports\n");
		//	exp->mask.src.u.udp.port  = 0xfffe;  // commented for
		//	testing : desai
		}

		exp->tuple.dst.protonum = IPPROTO_UDP;
		//exp->mask.dst.protonum  = 0xff;
        
		exp->helper = NULL;
		exp->flags = 0;
		exp->expectfn = expected; //NULL; //testing : desai

        expinfo.hiport = expinfo.loport + 1 ;
		
		//Nirav
#ifdef CONFIG_IFX_ALG_QOS
		exp->expectfn =  rtsp_expect;
		//exp.help.exp_rtsp_info.rtcp_dir = IP_CT_DIR_ORIGINAL; //check out 
		//exp->helper = &transport_helper;

		exp->master->rtcp_expect_registered = 2 ;
		expinfo.rtcp_dir = IP_CT_DIR_ORIGINAL;
#endif

		DEBUGP("expect_related %u.%u.%u.%u:%u-%u.%u.%u.%u:%u    %u\n",
				NIPQUAD(exp->tuple.src.u3.ip),
				ntohs(exp->tuple.src.u.udp.port),
				NIPQUAD(exp->tuple.dst.u3.ip),
				ntohs(exp->tuple.dst.u.udp.port),expinfo.loport);
                 
#if 0 // IFX_TEST
   if(nf_conntrack_expect_related(exp) != 0)
   {
       printk ("\n expect FAILED \n");
	   goto test;
   }
#endif
		//printk("Changing pskb --->skb 1\n"); 
				 
		nf_nat_rtsp = rcu_dereference(nf_nat_rtsp_hook);

		//printk("Changing pskb --->skb 2 \n");

		//*pskb = skb;
		pskb = (struct sk_buff *)skb;

		//printk("Changing pskb --->skb 3 \n");

                 if(nf_nat_rtsp)
                 {
                 
                     /* pass the request off to the nat helper */
                ret = nf_nat_rtsp(&pskb, ctinfo, hdrsoff, hdrslen, &expinfo, exp);
               // nf_conntrack_expect_related(exp); 

                }
                else if (nf_ct_expect_related(exp) != 0) {
                printk("ip_conntrack_expect_related failed\n");
                //ip_conntrack_expect_free(exp);
               // ret  = NF_DROP;
		}
		// UNLOCK_BH(&ip_rtsp_lock);
//test:
                                                                                                                                                    
//// For RTCP port
			exp1 = nf_ct_expect_alloc(ct);
			if (!exp1) { 
				ret = NF_DROP;
				goto out;
			}
			expinfo.loport += 1;
			nf_ct_expect_init(exp1, NF_CT_EXPECT_CLASS_DEFAULT,
					 ct->tuplehash[!dir].tuple.src.l3num,
					 &ct->tuplehash[!dir].tuple.src.u3,
					 &ct->tuplehash[!dir].tuple.dst.u3,
					 IPPROTO_UDP, NULL, &(expinfo.loport));

			exp1->tuple.src.u3 = ct->tuplehash[!dir].tuple.src.u3;
			exp1->mask.src.u3.ip  = 0xffffffff; //orginal
			exp1->tuple.src.u.tcp.port = 0;
			exp1->mask.src.u.udp.port  = 0 ;
			exp1->tuple.src.l3num = PF_INET ;
			//exp1->mask.src.l3num = 0xffff;
			exp1->tuple.dst.u3 = ct->tuplehash[dir].tuple.src.u3;
			//exp1->mask.dst.u3.ip  = 0xffffffff;
			exp1->tuple.dst.u.udp.port = htons(expinfo.loport);
			//exp1->mask.dst.u.udp.port  = 0xffff;
			exp1->tuple.dst.protonum = IPPROTO_UDP;
			//exp1->mask.dst.protonum  = 0xff;
			expinfo.pbtype = pb_single;
			exp1->helper = NULL;
			exp1->flags = 0;
			exp1->expectfn = expected;
			ct1 = exp->master;
			newip1 = ct1->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
			exp1->tuple.dst.u3.ip = newip1;
			exp1->saved_proto.udp.port = exp1->tuple.dst.u.udp.port;
			exp1->dir = !dir;

		DEBUGP("expect_related_RTCP %u.%u.%u.%u:%u-%u.%u.%u.%u:%u    %u\n",
				NIPQUAD(exp->tuple.src.u3.ip),
				ntohs(exp->tuple.src.u.udp.port),
				NIPQUAD(exp->tuple.dst.u3.ip),
				ntohs(exp->tuple.dst.u.udp.port),expinfo.loport);
	
		if (nf_ct_expect_related(exp1) != 0) {
			nf_ct_expect_insert_rtcp(exp1);
		}

		spin_unlock_bh(&ip_rtsp_lock);
                
		/* pass the request off to the nat helper */
	/*	rc = nf_conntrack_expect_related(ct, &exp);
		//UNLOCK_BH(&ip_rtsp_lock);
		if (rc == 0)
		{
			DEBUGP("ip_conntrack_expect_related succeeded\n");
		}
		else
		{
			INFOP("ip_conntrack_expect_related failed (%d)\n", rc);
		}*/

		//Nirav : Registering for RTCP in both directions...
#if 0   // made as 0
		//From server to client ---
        //         LOCK_BH(&ip_rtsp_lock);

		spin_lock_bh(&ip_rtsp_lock);
		
             exp1 = nf_conntrack_expect_alloc(ct);
               if (!exp1) {
                      ret = NF_DROP;
					  printk("DROP\n");
                        goto out;
                     }
																							  
          exp1->id = ntohl(tcph->seq) + hdrsoff; /* mark all the headers */ //added
		  expinfo.len = hdrslen; // 
        expinfo.rtcp_dir = IP_CT_DIR_ORIGINAL;

		//expinfo.loport += 1;
                exp1->master= ct ;
 
        // exp1->expectfn = expected ;

		exp1->tuple.src.u3.ip = ct->tuplehash[!dir].tuple.src.u3.ip;
	//	exp1->mask.src.u3.ip  = 0xffffffff;
		 exp1->mask.src.u3.ip  = 0;
		exp1->tuple.src.l3num = PF_INET ;
	    exp1->mask.src.l3num = 0xffff;
		
		exp1->mask.src.u.udp.port = 0 ;
		
		exp1->tuple.dst.u3.ip = ct->tuplehash[dir].tuple.src.u3.ip;
		exp1->mask.dst.u3.ip  = 0xffffffff;
		exp1->tuple.dst.u.udp.port = (expinfo.loport+1);
		exp1->mask.dst.u.udp.port  = 0xffff;
		exp1->tuple.dst.protonum = IPPROTO_UDP;
		exp1->mask.dst.protonum  = 0xff;
		expinfo.pbtype = pb_single;
		expinfo.rtcp_dir = IP_CT_DIR_ORIGINAL;

		printk("expect_related %u.%u.%u.%u:%u-%u.%u.%u.%u:%u ,  %u \n",
				NIPQUAD(exp1->tuple.src.u3.ip),
				ntohs(exp1->tuple.src.u.tcp.port),
				NIPQUAD(exp1->tuple.dst.u3.ip),
				ntohs(exp1->tuple.dst.u.tcp.port), PF_INET);

#if 0 // IFX_TEST
   if(nf_conntrack_expect_related(exp1) != 0)
      {
	         printk ("\n expect1 FAILED \n");
			 goto test1; 
       }
#endif
				
#if 1
nf_nat_rtsp = rcu_dereference(nf_nat_rtsp_hook);
             if (nf_nat_rtsp)
                /* pass the request off to the nat helper */
                {
		printk("INside 2 ip_nat_hook \n");
		ret = nf_nat_rtsp(pskb, ctinfo, &expinfo, exp1);
		//nf_conntrack_expect_put(exp1);
		}
	        else if (nf_conntrack_expect_related(exp1) != 0) {
               // INFOP("ip_conntrack_expect_related failed\n");
       //         ip_conntrack_expect_free(exp);
               // ret  = NF_DROP;
        	}
 
	//	 UNLOCK_BH(&ip_rtsp_lock);

//test1:
	spin_unlock_bh(&ip_rtsp_lock);
#endif

#if 0
if (nf_conntrack_expect_related(exp1) != 0) {
               // INFOP("ip_conntrack_expect_related failed\n");
	          //         ip_conntrack_expect_free(exp);
               // ret  = NF_DROP;
           }
												 
spin_unlock_bh(&ip_rtsp_lock);
#endif

		/* pass the request off to the nat helper */
	       /*	rc = ip_conntrack_expect_related(ct, &exp);
		UNLOCK_BH(&ip_rtsp_lock);
		if (rc == 0)
		{
			DEBUGP("ip_conntrack_expect_related succeeded\n");
		}
		else
		{
			INFOP("ip_conntrack_expect_related failed (%d)\n", rc);
		}*/

#if 0
		//From client to server ---
		exp.tuple.src.ip = ct->tuplehash[dir].tuple.src.ip;
		exp.mask.src.ip  = 0xffffffff;
		exp.tuple.dst.ip = ct->tuplehash[!dir].tuple.src.ip;
		exp.mask.dst.ip  = 0xffffffff;
		exp.tuple.src.u.udp.port = exp.help.exp_rtsp_info.loport;
		exp.mask.src.u.udp.port  = 0xffff;
		exp.tuple.dst.u.all = 0;
		exp.mask.dst.u.all = 0;
		exp.tuple.dst.protonum = IPPROTO_UDP;
		exp.mask.dst.protonum  = 0xffff;
		exp.help.exp_rtsp_info.rtcp_dir = IP_CT_DIR_REPLY;

		DEBUGP("expect_related %u.%u.%u.%u:%u-%u.%u.%u.%u:%u\n",
				NIPQUAD(exp.tuple.src.ip),
				ntohs(exp.tuple.src.u.tcp.port),
				NIPQUAD(exp.tuple.dst.ip),
				ntohs(exp.tuple.dst.u.tcp.port));

		/* pass the request off to the nat helper */
		rc = ip_conntrack_expect_related(ct, &exp);
		//UNLOCK_BH(&ip_rtsp_lock);
		if (rc == 0)
		{
			DEBUGP("ip_conntrack_expect_related succeeded\n");
		}
		else
		{
			INFOP("ip_conntrack_expect_related failed (%d)\n", rc);
		}
#endif
#endif
       /****for testing ****/
   // nf_conntrack_expect_related(exp);
	//nf_conntrack_expect_related(exp1);

		//nf_conntrack_expect_put(exp);
		nf_ct_expect_put(exp);
		goto out;
	}

//    nf_conntrack_expect_put(exp); //works without this check out,IT MAY CRASH , IT FREES MEMORY
 
out:
	//return NF_ACCEPT;
          return ret;
}

/* inbound packet: server->client */
static inline int
help_in(struct sk_buff *skb , size_t pktlen,
		struct nf_conn* ct, enum ip_conntrack_info ctinfo)
{

// ct->ifx_alg_qos_mark = IFX_ALG_APP_RTSP | IFX_ALG_PROTO_RTP ; //for test

  //DEBUGP ("\nRTSP_ALG: help_out marked ct->ifx_alg_qos_mark to : %x ***\n",
   //                                                           ct->ifx_alg_qos_mark );

	return NF_ACCEPT;
}

static int
help(struct sk_buff *skb,unsigned int protoff,
		struct nf_conn* ct, enum ip_conntrack_info ctinfo)
{
	/* tcplen not negative guarenteed by ip_conntrack_tcp.c */
	//struct tcphdr* tcph = (void*)iph + iph->ihl * 4;
	//u_int32_t tcplen = pktlen - iph->ihl * 4;
         struct tcphdr _tcph, *th;
         unsigned int dataoff, datalen;
         char *rb_ptr;
         int ret = NF_DROP;

	/* Until there's been traffic both ways, don't look in packets. */
	if (ctinfo != IP_CT_ESTABLISHED && ctinfo != IP_CT_ESTABLISHED + IP_CT_IS_REPLY)
	{
		DEBUGP("conntrackinfo = %u\n", ctinfo);
		return NF_ACCEPT;
	} 


        th = skb_header_pointer(skb, protoff,
                            sizeof(_tcph), &_tcph);
      if (!th)
             return NF_ACCEPT;

    /* No data ? */
	dataoff = protoff + th->doff*4;
	datalen = skb->len - dataoff;
	if (dataoff >= skb->len)
            return NF_ACCEPT;

    	//spin_lock_bh(&rtsp_buffer_lock);
    	rb_ptr = skb_header_pointer(skb, dataoff,
                                skb->len - dataoff, rtsp_buffer);
    	BUG_ON(rb_ptr == NULL);

	/* Not whole TCP header? */
/*	if (tcplen < sizeof(struct tcphdr) || tcplen < tcph->doff * 4)
	{
		DEBUGP("tcplen = %u\n", (unsigned)tcplen);
		return NF_ACCEPT;
	}*/

	/* Checksum invalid?  Ignore. */
	/* FIXME: Source route IP option packets --RR */
/*	if (tcp_v4_check(tcph, tcplen, iph->saddr, iph->daddr,
				csum_partial((char*)tcph, tcplen, 0)))
	{
		DEBUGP("bad csum: %p %u %u.%u.%u.%u %u.%u.%u.%u\n",
				tcph, tcplen, NIPQUAD(iph->saddr), NIPQUAD(iph->daddr));
		return NF_ACCEPT;
	}*/

	switch (CTINFO2DIR(ctinfo))
	{
		case IP_CT_DIR_ORIGINAL:
			
			//help_out(iph, pktlen, ct, ctinfo);
                        ret = help_out(skb, rb_ptr, datalen, ct, ctinfo);
			break;
		case IP_CT_DIR_REPLY:
			DEBUGP("Calling help_in ********* \n");
			 //ret = help_in(pskb, datalen, ct, ctinfo);
			 return NF_ACCEPT;
			break;
		default:
			/* oops */
                       break;
	}

	return NF_ACCEPT;
}

#ifndef IFX_RTSP_NAT_ALG
static struct nf_conntrack_helper rtsp_helpers[MAX_PORTS];
static char rtsp_names[MAX_PORTS][10];
static struct nf_conntrack_expect_policy rtsp_expect_policy;
#endif

/* This function is intentionally _NOT_ defined as __exit */
	static void
fini(void)
{
#ifndef IFX_RTSP_NAT_ALG
	int i;
	for (i = 0; i < num_ports; i++)
	{
		DEBUGP("unregistering port %d\n", ports[i]);
		nf_conntrack_helper_unregister(&rtsp_helpers[i]);
	}

#else

	struct list_head  *cur_item;
	struct list_head  *temp_item;
	struct rtsp_conn_registration_data* cur_registration;
	int ret =0;

	list_for_each_safe(cur_item, temp_item, &rtsp_conn_registration_list)
	{
		cur_registration = list_entry(cur_item, struct rtsp_conn_registration_data, list);
		NCDEBUG("cur_registration is %p",cur_registration);

		nf_conntrack_helper_unregister(cur_registration->conntrack_helper);

		kfree(cur_registration->conntrack_helper->name );

		kfree(cur_registration->conntrack_helper);

		list_del(&(cur_registration->list));
		kfree(cur_registration);

		MOD_DEC_USE_COUNT;


	}

	ret = unregister_chrdev(MAJOR_NUM_RTSP_CONN, DEVICE_FILE_NAME_RTSP_CONN);
	/* If there's an error, report it */ 
	if (ret < 0)
		printk("Error in Connection track module_unregister_chrdev: %d\n", ret);

#endif
}

	static int __init
init(void)
{

	int  ret;

#ifndef IFX_RTSP_NAT_ALG

	struct nf_conntrack_helper *hlpr;
	char *tmpname;
	int i = 0;

#endif

	printk("ip_conntrack_rtsp v" IP_NF_RTSP_VERSION " loading\n");

	if (max_outstanding < 1)
	{
		printk("ip_conntrack_rtsp: max_outstanding must be a positive integer\n");
		return -EBUSY;
	}
	if (setup_timeout < 0)
	{
		printk("ip_conntrack_rtsp: setup_timeout must be a positive integer\n");
		return -EBUSY;
	}
#ifndef IFX_RTSP_NAT_ALG
	
	rtsp_expect_policy.max_expected = max_outstanding;
	rtsp_expect_policy.timeout = setup_timeout;

	/* If no port given, default to standard rtsp port */
	if (ports[0] == 0)
	{
		ports[0] = RTSP_PORT;
	}

	for (i = 0; (i < MAX_PORTS) && ports[i]; i++)
	{
		hlpr = &rtsp_helpers[i];
		memset(hlpr, 0, sizeof(struct nf_conntrack_helper));
    		 hlpr->tuple.src.l3num = PF_INET;
                //hlpr->mask.src.l3num = 0xFFFF;
		hlpr->tuple.src.u.tcp.port = htons(ports[i]);
		hlpr->tuple.dst.protonum = IPPROTO_TCP;
		//hlpr->mask.src.u.tcp.port = 0xFFFF;
		//hlpr->mask.dst.protonum = 0xFF;
//		hlpr->max_expected = max_outstanding;
//		hlpr->timeout = setup_timeout;
		//hlpr->flags = IP_CT_HELPER_F_REUSE_EXPECT;
		hlpr->expect_policy = &rtsp_expect_policy;
		hlpr->me = THIS_MODULE;
		hlpr->help = help;

		tmpname = &rtsp_names[i][0];
		if (ports[i] == RTSP_PORT)
		{
			sprintf(tmpname, "rtsp");
		}
		else
		{
			sprintf(tmpname, "rtsp-%d", i);
		}
		hlpr->name = tmpname;

		DEBUGP("port #%d: %d\n", i, ports[i]);

		ret = nf_conntrack_helper_register(hlpr);

		if (ret)
		{
			printk("nf_conntrack_rtsp: ERROR registering port %d\n", ports[i]);
			fini();
			return -EBUSY;
		}
		num_ports++;
	}
#else
#warning IFX_RTSP_NAT_ALG

	NCDEBUG("Registering Rtsp_connection_device\n");
	ret = register_chrdev(MAJOR_NUM_RTSP_CONN, DEVICE_FILE_NAME_RTSP_CONN, &rtsp_Fops_conn);

	/* Negative values signify an error */
	if (ret < 0) 
	{
		printk (" Sorry, registering the character device fails ret is %d ",ret);
		return -1;
	}


#endif
	return 0;

}

#ifdef IFX_RTSP_NAT_ALG
int rtsp_conn_device_ioctl(
		struct inode *inode,
		struct file *file,
		unsigned int ioctl_num,/* The number of the ioctl */
		unsigned long ioctl_param) /* The parameter to it */
{
	struct rtsp_params *rtspparam;
	int ret = 0 ;
	/* Switch according to the ioctl called */
	switch (ioctl_num) {
		//case IOCTL_SET_RTSP_MSG:

		//	break;

		case IOCTL_DEREGISTER_RTSP_PORT_CONN:

			rtspparam=(struct rtsp_params*)ioctl_param;
			NCDEBUG("rtspparam.rtsp_port %d,rtspparam.ip_proto %d",rtspparam->rtsp_port,rtspparam->ip_proto);
			ret = rtsp_conn_device_port_deregister(rtspparam);
			if (ret < 0 )
			{
				printk("\n ERROR : rtsp_conn_device_port_deregister is failed \n");
				return -1;
			}

			break;
		case IOCTL_REGISTER_RTSP_PORT_CONN:

			NCDEBUG("Received IOCTL_REGISTER_PORT\n");
			rtspparam = (struct rtsp_params*)ioctl_param;

			NCDEBUG("RTSP conn Port: [%d]\n",rtspparam->rtsp_port);
			if(rtspparam->ip_proto== IP_PROTO_TCP)
				NCDEBUG("RTSP conn Proto: TCP\n");
			else if(rtspparam->ip_proto== IP_PROTO_UDP)
				NCDEBUG("RTSP conn Proto: UDP\n");
			NCDEBUG("rtspparam.rtsp_port %d,rtspparam.ip_proto %d",rtspparam->rtsp_port,rtspparam->ip_proto);
			ret = rtsp_conn_device_port_register(rtspparam);
			if (ret < 0 )
			{
				printk("\n ERROR : rtsp_conn_device_port_register is failed \n");
				return -1;
			}

			break;
	}

	return 0;
}

/* This function is called whenever a process attempts 
 * to open the device file */
static int rtsp_conn_device_open(struct inode *inode,struct file *file)
{
#ifdef DEBUG
	printk ("device_open(%p)\n", file);
#endif

	/* We don't want to talk to two processes at the 
	 * same time */
	if (Device_Open)
	{
		return -EBUSY;
	}

	/* If this was a process, we would have had to be 
	 * more careful here, because one process might have 
	 * checked Device_Open right before the other one 
	 * tried to increment it. However, we're in the 
	 * kernel, so we're protected against context switches.
	 *
	 * This is NOT the right attitude to take, because we
	 * might be running on an SMP box, but we'll deal with
	 * SMP in a later chapter.
	 */ 

	Device_Open++;

	/* Initialize the message */
	Message_Ptr = Message;

	return 0;
}

/* This function is called when a process closes the 
 * device file. It doesn't have a return value because 
 * it cannot fail. Regardless of what else happens, you 
 * should always be able to close a device (in 2.0, a 2.2
 * device file could be impossible to close).
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static int rtsp_conn_device_release(struct inode *inode, 
		struct file *file)
#else
static void rtsp_conn_device_release(struct inode *inode, 
		struct file *file)
#endif
{
#ifdef DEBUG
	printk ("device_release(%p,%p)\n", inode, file);
#endif

	/* We're now ready for our next caller */
	Device_Open --;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	return 0;
#endif
}

static int rtsp_conn_device_port_register(struct rtsp_params* rtspparam)
{

	int port = RTSP_PORT;

	struct rtsp_conn_registration_data *rtsp_conn_reg_data;
	int ret = 0;
	char *tmp_name;

	struct nf_conntrack_helper *rtsp_conntrack_hlpr;


	rtsp_conn_reg_data = kmalloc(sizeof(struct rtsp_conn_registration_data), GFP_ATOMIC);
	if(rtsp_conn_reg_data == NULL)
	{
		NCDEBUG("RTSP : hlpr rtsp_reg_data Memory not available\n");
		return -1;
	}

	rtsp_conntrack_hlpr = kmalloc(sizeof(struct ip_conntrack_helper), GFP_ATOMIC);
	if(rtsp_conntrack_hlpr == NULL)
	{
		NCDEBUG("RTSP : rtsp_conntrack_hlpr Memory not available\n");

		return -1;
	}

	tmp_name = kmalloc(20, GFP_ATOMIC);
	if(tmp_name == NULL)
	{
		NCDEBUG("RTSP : tmp_name memeory not available \n");

		return -1;
	}


	memset(rtsp_conn_reg_data,0,sizeof(struct rtsp_conn_registration_data));

	memset(rtsp_conntrack_hlpr,0,sizeof(struct ip_conntrack_helper));
	memset(tmp_name,'\0',20);


	NCDEBUG("\n INIT_LIST_HEAD try init list... rtsp_conn_reg_data %p",rtsp_conn_reg_data);
	/* Init list head of the rtsp_registration_data*/
	INIT_LIST_HEAD(&(rtsp_conn_reg_data->list));

	port = rtspparam->rtsp_port;




	rtsp_conntrack_hlpr->tuple.src.u.tcp.port = htons(port);
	rtsp_conntrack_hlpr->tuple.dst.protonum = IPPROTO_TCP;
	rtsp_conntrack_hlpr->mask.src.u.tcp.port = 0xFFFF;


	rtsp_conntrack_hlpr->list.next =NULL;
	rtsp_conntrack_hlpr->list.prev =NULL;

	rtsp_conntrack_hlpr->mask.dst.protonum = 0xFFFF;
	rtsp_conntrack_hlpr->max_expected = max_outstanding;
	rtsp_conntrack_hlpr->timeout = setup_timeout;
	rtsp_conntrack_hlpr->flags = IP_CT_HELPER_F_REUSE_EXPECT;
	//      rtsp_conntrack_hlpr->me = ip_conntrack_rtsp;
	rtsp_conntrack_hlpr->me = THIS_MODULE;
	rtsp_conntrack_hlpr->help = help;


	if (port == RTSP_PORT)
	{
		sprintf(tmp_name, "rtsp");
	}
	else
	{
		sprintf(tmp_name, "rtsp-%d", port);
	}

	rtsp_conntrack_hlpr->name = tmp_name;

	NCDEBUG("port # %d and name is %s\n", port,rtsp_conntrack_hlpr->name);

	NCDEBUG(" calling ip_conntrack_helper_register  and name is %s",rtsp_conntrack_hlpr->name);
	ret = nf_conntrack_helper_register(rtsp_conntrack_hlpr);
	NCDEBUG(" \n Connection register helper register ...........return %d\n", ret);

	if (ret < 0)
	{
		printk("ip_conntrack_rtsp: rtsp_conntrack_hlpr ERROR registering port %d\n", port);
		fini();
		return -EBUSY;
	}



	MOD_INC_USE_COUNT;


	rtsp_conn_reg_data->conntrack_helper = rtsp_conntrack_hlpr;
	rtsp_conn_reg_data->proto = rtspparam->ip_proto;
	rtsp_conn_reg_data->port = port;

	NCDEBUG("RTSP: Adding rtsp info to the list\n");
	list_prepend(&(rtsp_conn_registration_list), &(rtsp_conn_reg_data->list));


	return 0;	   
}

static int  get_reg_data(const struct rtsp_conn_registration_data* data, int port, enum AlgControlProtocol proto)
{
	if((data->port == port)&&(data->proto == proto))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}


static int rtsp_conn_device_port_deregister(struct rtsp_params* rtspparam)
{
	struct rtsp_conn_registration_data* rtsp_conn_reg_data;

	rtsp_conn_reg_data = LIST_FIND(&rtsp_conn_registration_list,
			get_reg_data,
			struct rtsp_conn_registration_data *,
			rtspparam->rtsp_port, 
			rtspparam->ip_proto );

	NCDEBUG("\n rtsp_reg_data is %p\n",rtsp_conn_reg_data);

	if(rtsp_conn_reg_data == NULL)
	{
		NCDEBUG("RTSP: No registration for this port exists\n");
		return -1;
	}

	ip_conntrack_helper_unregister(rtsp_conn_reg_data->conntrack_helper);
	NCDEBUG("RTSP: Conntrack helper unregistered\n");

	NCDEBUG("Trying to free rtsp_conntrack_hlpr->name ");
	kfree(rtsp_conn_reg_data->conntrack_helper->name );

	NCDEBUG(" Trying to free conntrack helper");
	kfree(rtsp_conn_reg_data->conntrack_helper);

	NCDEBUG("RTSP: Freeing up rtsp_registration_data->list\n");
	list_del(&(rtsp_conn_reg_data->list));

	NCDEBUG(" Trying to free rtsp_reg_data");
	kfree(rtsp_conn_reg_data);

	MOD_DEC_USE_COUNT;


	return 0;
}

#endif


#ifdef CONFIG_IP_NF_NAT_NEEDED
EXPORT_SYMBOL(ip_rtsp_lock);
#endif

module_init(init);
module_exit(fini);

