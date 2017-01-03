/* ============================================================================
 * Copyright (C) 2003[- 2004] ?Infineon Technologies AG.
 *
 * All rights reserved.
 * ============================================================================
 *
 * ============================================================================
 *
 * This document contains proprietary information belonging to Infineon 
 * Technologies AG. Passing on and copying of this document, and communication
 * of its contents is not permitted without prior written authorisation.
 * 
 * ============================================================================
 */


/* ===========================================================================
 *
 * File Name: ipt_ifxattack.c
 * Author : Nirav Salot
 * Date: 
 *
 * ===========================================================================
 *
 * Project: Danube/DoS Attacks
 * Block: <block/module name>
 *
 * ===========================================================================
 * Contents:  This file contains the code to handle DoS attacks.
 * 
 * ===========================================================================
 * References: <List of design documents covering this file.>
 */

/*
 * ===========================================================================
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/tcp.h>
#include <net/udp.h>

#include <linux/string.h>
#include <linux/ctype.h>

#include <linux/netfilter_ipv4/ipt_ifxattack.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/netfilter/nf_conntrack_ftp.h>

/* ===========================================================================
 */
/* Kernel module to match IFX Attacks values. */
#define TH_SYN 0x02

int xstrncasecmp(char *str1,char *str2,int len) {
	int i;

	for (i = 0; i < len; i++) {
		char a1,a2;
		a1 = tolower(str1[i]);
	    a2 = tolower(str2[i]);
		if (a1 != a2)
			return a1 - a2;
	}
	return 0;
}

int xstrtolower(char *str) {
	int i;

	for(i = 0; i < strlen(str); i++)
		str[i] = tolower(str[i]);
	return 0;
}

/* Returns 1 if the attack is matched, 0 otherwise */
static inline int
attack_match(enum ifxattack_name attack_name,
		const struct sk_buff *skb,
		int *hotdrop)
{
	const struct iphdr *iph = ip_hdr(skb); //(void *)skb->nh.iph;
	const struct tcphdr *tcph = (void*)iph + iph->ihl * 4; //(void *)skb->nh.iph + skb->nh.iph->ihl*4;
	const struct udphdr *udph = (void*)iph + iph->ihl * 4; //(void *)skb->nh.iph + skb->nh.iph->ihl*4;
	int ret = 0;
	int ip_data_len = iph->tot_len - iph->ihl * 4;

#if defined(CONFIG_CODE_REDI) || defined(CONFIG_CODE_REDII) || defined(CONFIG_ICQ_DOS)
	const char CODE_REDI_SIGNATURE[] = "GET /default.ida?NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN%u9090%u6858%ucbd3%u7801%u9090%u6858%ucbd3%u7801%u9090%u6858%ucbd3%u7801%u9090%u9090%u8190%u00c3%u0003%u8b00%u531b%u53ff%u0078%u0000%u00=a";

	const char CODE_REDII_SIGNATURE[] = "GET /default.ida?XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX%u9090%u6858%ucbd3%u7801%u9090%u6858%ucbd3%u7801%u9090%u6858%ucbd3%u7801%u9090%u9090%u8190%u00c3%u0003%u8b00%u531b%u53ff%u0078%u0000%u00=a";

	const char ICQ_DOS_SIGNATURE[] = "GET /cgi-bin/guestbook.cgi? ";

	if (attack_name == CODE_REDI || attack_name == CODE_REDII || attack_name == ICQ_DOS) {
		char localbuf[512];
		int totsize = 0;
		int cursig_len = 0;
		const char *pcursig = NULL;

#ifdef CONFIG_CODE_REDI
		if (attack_name == CODE_REDI) {
			cursig_len = strlen(CODE_REDI_SIGNATURE);
			pcursig = CODE_REDI_SIGNATURE;
		}
#endif
#ifdef CONFIG_CODE_REDII
		if (attack_name == CODE_REDII) {
			cursig_len = strlen(CODE_REDII_SIGNATURE);
			pcursig = CODE_REDII_SIGNATURE;
		}
#endif
#ifdef CONFIG_ICQ_DOS
		if (attack_name == ICQ_DOS) {
			cursig_len = strlen(ICQ_DOS_SIGNATURE);
			pcursig = ICQ_DOS_SIGNATURE;
		}
#endif
		memset(localbuf,0x00,sizeof(localbuf));
		totsize = ip_data_len - tcph->doff * 4;
		if (totsize > cursig_len) 
		{
			if (totsize > sizeof(localbuf) - 1)
				totsize = sizeof(localbuf) - 1;
			memcpy(localbuf, (void *)tcph + tcph->doff * 4, totsize);
			if (memcmp(localbuf, pcursig, cursig_len) == 0) {
				ret = 1;
			}
		}
	}
#endif

#ifdef CONFIG_IIS_FRONTPAGE
#define	IIS_FRONTPAGE_SIGNATURE1	"POST /_vti_bin/shtml.dll/_vti_rpc"
#define	IIS_FRONTPAGE_SIGNATURE2	"POST /_vti_bin/_vti_aut/author.dll"
#define CONTENT_LENGTH				"Content-Length: "

	if (attack_name == IIS_FRONTPAGE) {
		char localbuf[512];
		int totsize = 0;

		memset(localbuf,0x00,sizeof(localbuf));
		totsize = ip_data_len - tcph->doff * 4;
		if (totsize > strlen(IIS_FRONTPAGE_SIGNATURE2)) {
			if (totsize > sizeof(localbuf) - 1)
				totsize = sizeof(localbuf) - 1;
			memcpy(localbuf, (void *)tcph + tcph->doff * 4, totsize);
			if ((memcmp(localbuf, IIS_FRONTPAGE_SIGNATURE1, strlen(IIS_FRONTPAGE_SIGNATURE1)) == 0) || (memcmp(localbuf,IIS_FRONTPAGE_SIGNATURE2, strlen(IIS_FRONTPAGE_SIGNATURE2))== 0)) {
				char *pcontent_length = NULL;

				pcontent_length = strstr(localbuf,CONTENT_LENGTH);
				if(pcontent_length) {
					pcontent_length += strlen(CONTENT_LENGTH);
					localbuf[strstr(pcontent_length,"\r\n") - localbuf] = '\0';
					if(memcmp(pcontent_length,"5058",strlen("5058")) == 0)
						ret = 1;
				}
			}
		}
	}
#endif

#ifdef CONFIG_LAND_ATTACK
	if (attack_name == LAND_ATTACK) {
		if (iph->saddr == iph->daddr) {
			ret = 1;
		}
	}
#endif

#if 0
#ifdef CONFIG_FRAGGLE
	if (attack_name == FRAGGLE) {
		struct rtable *rt = NULL;
		struct sk_buff *skb_c = NULL;

		skb_c = skb_clone(skb, 0);
		if (skb_c == NULL) {
			printk("ifxattack : Fraggle -> Could not clone skb");
			goto frag_out;
		}
		if (ip_route_input(skb_c, iph->daddr, iph->saddr, iph->tos, skb->dev)) {
			printk("ifxattack : Fraggle -> Could not ip_route_input");
			goto frag_out;
		}
		rt = (struct rtable *)skb_c->dst;
		if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST)) {
			ret = 1;
		}
frag_out:
		if (skb_c) 
			kfree_skb(skb_c);
	}
#endif
#endif

#ifdef CONFIG_MALFORMED_MIME

#define	STR_EOL			"\r\n"
#define CONTENT_TYPE	"Content-Type:"
#define STR_CHARSET		"charset="

	if (attack_name == MALFORMED_MIME) {
		int totsize = ip_data_len - iph->ihl * 4 - tcph->doff * 4;
		char *curpos = NULL;
		char *EofConLine = NULL;
		int i = 0;
		char *pbegin = NULL;

		pbegin = curpos = (void *)tcph + tcph->doff * 4;
		do {
			EofConLine = NULL;
			EofConLine = strstr(curpos, STR_EOL);
			if (EofConLine == NULL)
				break;
			while (*(EofConLine + strlen(STR_EOL)) == '\t') {
				EofConLine = strstr(EofConLine + strlen(STR_EOL) + 1,STR_EOL);
				if (EofConLine == NULL) 
					break;
			}
			if (EofConLine == NULL) 
				break;
			if( xstrncasecmp(curpos,CONTENT_TYPE,strlen(CONTENT_TYPE))== 0) 
			//if( strncmp(curpos,CONTENT_TYPE,strlen(CONTENT_TYPE))== 0) 
			{
				char content_line[256];
				char *marker = NULL;
				memset(content_line,0,sizeof(content_line));
				strncpy(content_line,curpos ,EofConLine - curpos + strlen(STR_EOL));
				xstrtolower(content_line);
				marker = strstr(content_line,STR_CHARSET);
				
				if(marker != NULL) { // now check if it is non empty
					//printf("### charset found in [%s]\n",marker);
					marker += strlen(STR_CHARSET); //to skip the "charset="
					if( strncmp(marker,"\"\"",strlen("\"\"")) == 0 || strncmp(marker,STR_EOL,strlen(STR_EOL)) == 0) {
						//printf("### Attack found\n");
						ret = 1; //attack confirmed
						break;
					}
				}	
			}
			curpos = EofConLine + strlen(STR_EOL);
			i = curpos - pbegin;
			if (strncmp(EofConLine + strlen(STR_EOL),STR_EOL,strlen(STR_EOL)) == 0)
				break;
		}while( i < totsize );
	}
#endif

#ifdef CONFIG_UDP_BOMB
	if (attack_name == UDP_BOMB) {
		if(udph->len != ip_data_len)
			ret = 1;
	}
#endif

#ifdef CONFIG_FTP_PORT_REST
	if (attack_name == FTP_PORT_REST) {
		struct nf_conn *ct = NULL;
		struct nf_conn_help *help = NULL;
		enum ip_conntrack_info ctinfo;
		struct nf_ct_ftp_master *ct_ftp_info = NULL;

		ct = nf_ct_get(skb, &ctinfo);
		help = nfct_help(ct);
		if (help != NULL) {
			ct_ftp_info = &(help->help.ct_ftp_info);
			if (ct_ftp_info) {
				if (ct_ftp_info->active_ftp_port > 0 && ct_ftp_info->active_ftp_port <= 1024)
					ret = 1;
			}
		}
	}
#endif
	return ret;
}

static int
match(const struct sk_buff *skb, const struct xt_match_param *par)
{
	const struct ipt_ifxattack_match_info *info = par->matchinfo;

	//return attack_match(info->attack_name, tcph, skb->len - skb->nh.iph->ihl*4, hotdrop);
	return attack_match(info->attack_name, skb, (int*)par->hotdrop);
}

static int
checkentry(const struct xt_mtchk_param *par)
{
	const struct ipt_ifxattack_match_info *info = par->matchinfo;
	const struct ipt_ip *ip = par->entryinfo;

#ifdef CONFIG_LAND_ATTACK
	/* Nothing */
#endif
#ifdef CONFIG_CODE_REDI
	/* Must specify -p tcp */
	if (info->attack_name == CODE_REDI && (ip->proto != IPPROTO_TCP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: code_redI only works on TCP packets\n");
		return 0;
	}
#endif
#ifdef CONFIG_CODE_REDII
	/* Must specify -p tcp */
	if (info->attack_name == CODE_REDII && (ip->proto != IPPROTO_TCP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: code_redII only works on TCP packets\n");
		return 0;
	}
#endif
#ifdef CONFIG_ICQ_DOS
	/* Must specify -p tcp */
	if (info->attack_name == ICQ_DOS && (ip->proto != IPPROTO_TCP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: icq_dos only works on TCP packets\n");
		return 0;
	}
#endif
#ifdef CONFIG_MALFORMED_MIME
	/* Must specify -p tcp */
	if (info->attack_name == MALFORMED_MIME && (ip->proto != IPPROTO_TCP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: malformed_mime only works on TCP packets\n");
		return 0;
	}
#endif
#ifdef CONFIG_IIS_FRONTPAGE
	/* Must specify -p tcp */
	if (info->attack_name == IIS_FRONTPAGE && (ip->proto != IPPROTO_TCP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: iis_frontpage only works on TCP packets\n");
		return 0;
	}
#endif
#ifdef CONFIG_UDP_BOMB
	/* Must specify -p udp */
	if (info->attack_name == UDP_BOMB && (ip->proto != IPPROTO_UDP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: udp_bomb only works on UDP packets\n");
		return 0;
	}
#endif
#ifdef CONFIG_FTP_PORT_REST
	/* Must specify -p tcp */
	if (info->attack_name == FTP_PORT_REST && (ip->proto != IPPROTO_TCP || (ip->invflags & IPT_INV_PROTO))) {
		printk("ifxattack: ftp_port only works on TCP packets\n");
		return 0;
	}
#endif
	return 1;
}

static struct xt_match ifxattack_match = {
	.list = { NULL, NULL },
	.name = "ifxattack",
	.family = NFPROTO_IPV4,
	.match = match,
	.checkentry = checkentry,
	.matchsize = sizeof(struct ipt_ifxattack_match_info),
	.destroy = NULL,
	.me = THIS_MODULE
};
					
static int __init init(void)
{
	return xt_register_match(&ifxattack_match);
}

static void __exit fini(void)
{
	xt_unregister_match(&ifxattack_match);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");

