/******************************************************************************
**
** FILE NAME    : ifxmips_ppa_proc_v5.c
** PROJECT      : UEIP
** MODULES      : ATM + MII0/1 Acceleration Package (PPA)
**
** DATE         : 01 MAY 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM + MII0/1 Driver with Acceleration Firmware (A5/D5/E5)
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 01 May 2008  Xu Liang        Initiate Version
*******************************************************************************/



/*!
  \defgroup AMAZON_S_PPA_PPE_A5_HAL Amazon-S (AR9) PPA PPE A5 HAL layer driver module
  \brief Amazon-S (AR9) PPA PPE A5 HAL layer driver module
 */

/*!
  \defgroup AMAZON_S_PPA_PPE_A5_HAL_COMPILE_PARAMS Compile Parametere
  \ingroup AMAZON_S_PPA_PPE_A5_HAL
  \brief compile options to turn on/off some feature
 */

/*!
  \defgroup AMAZON_S_PPA_PPE_A5_HAL_COMPILE_PARAMS Exported Functions
  \ingroup AMAZON_S_PPA_PPE_A5_HAL
  \brief exported functions for other driver use
 */

/*!
  \file amazon_s_ppa_ppe_a5_hal.c
  \ingroup AMAZON_S_PPA_PPE_A5_HAL
  \brief Amazon-S (AR9) PPA PPE A5 HAL layer driver file
 */

/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */

#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
//#include <linux/skbuff.h>
#include <asm/uaccess.h>

/*
 *  Chip Specific Head File
 */
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifxmips_ppa_hal_ar10_a5.h"
#include "ifxmips_ppa_proc_ar10_a5.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */
#define PPE_MAX_ETH1_QOS_QUEUE 8

/*!
  \addtogroup AMAZON_S_PPA_PPE_A5_HAL_COMPILE_PARAMS
 */
/*@{*/
/*!
  \brief Turn on/off debugging message and disable inline optimization.
 */
#define ENABLE_DEBUG                            1

/*!
  \brief Turn on/off ASSERT feature, print message while condition is not fulfilled.
 */
#define ENABLE_ASSERT                           1
/*@}*/

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
  #define ENABLE_DEBUG_PRINT                    1
  #define DISABLE_INLINE                        1
#endif

#if defined(DISABLE_INLINE) && DISABLE_INLINE
  #define INLINE
#else
  #define INLINE                                inline
#endif

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
  #undef  dbg
  #define dbg(format, arg...)                   do { printk(KERN_WARNING __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#elif !defined(dbg)
  #define dbg(format, arg...)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
  #define MODULE_PARM_ARRAY(a, b)   module_param_array(a, int, NULL, 0)
  #define MODULE_PARM(a, b)         module_param(a, int, 0)
#else
  #define MODULE_PARM_ARRAY(a, b)   MODULE_PARM(a, b)
#endif

/*
 * ####################################
 *             Declaration
 * ####################################
 */
static INLINE int stricmp(const char *, const char *);
static INLINE int strincmp(const char *, const char *, int);
static INLINE int get_token(char **, char **, int *, int *);
static INLINE int get_number(char **, int *, int);
static INLINE void get_ip_port(char **, int *, unsigned int *);
static INLINE void get_mac(char **, int *, unsigned int *);
static INLINE void ignore_space(char **, int *);
static INLINE int print_route(char *, int, int, struct rout_forward_compare_tbl *, struct rout_forward_action_tbl *);
static INLINE int print_mc(char *, int, struct wan_rout_multicast_cmp_tbl *, struct wan_rout_multicast_act_tbl *);
static INLINE uint32_t get_phys_port_num(void);

static char * get_wanitf(int iswan);

/*
 *  local implement memcpy
 */
#if defined(ENABLE_MY_MEMCPY) && ENABLE_MY_MEMCPY
  static INLINE void *my_memcpy(unsigned char *, const unsigned char *, unsigned int);
#else
  #define my_memcpy             memcpy
#endif

//#define ppa_memset              memset


/*
 * ####################################
 *           Global Variable
 * ####################################
 */

//static int g_ipv6_acc_en = 1;
static unsigned int g_ipv4_lan_collision_rout_fwdc_table_base;
static unsigned int g_ipv4_wan_collision_rout_fwda_table_base;

int32_t (*ifx_ppa_drv_add_ipv6_routing_entry_fn)(uint32_t f_is_lan,
                               uint32_t src_ip[4],
                               uint32_t src_port,
                               uint32_t dst_ip[4],
                               uint32_t dst_port,
                               uint32_t f_is_tcp,    //  1: TCP, 0: UDP
                               uint32_t route_type,
                               uint32_t new_ip,
                               uint32_t new_port,
                               uint8_t  new_mac[PPA_ETH_ALEN],
                               uint32_t new_src_mac_ix,
                               uint32_t mtu_ix,
                               uint32_t f_new_dscp_enable,
                               uint32_t new_dscp,
                               uint32_t f_vlan_ins_enable,
                               uint32_t new_vci,
                               uint32_t f_vlan_rm_enable,
                               uint32_t pppoe_mode,
                               uint32_t pppoe_ix,
                               uint32_t f_out_vlan_ins_enable,
                               uint32_t out_vlan_ix,
                               uint32_t f_out_vlan_rm_enable,
                               uint32_t dslwan_qid,
                               uint32_t dest_list,
                               uint32_t *p_entry) = NULL;


int32_t (*ifx_ppa_drv_add_routing_entry_fn)(uint32_t f_is_lan,
                          uint32_t src_ip,
                          uint32_t src_port,
                          uint32_t dst_ip,
                          uint32_t dst_port,
                          uint32_t f_is_tcp,    //  1: TCP, 0: UDP
                          uint32_t route_type,
                          uint32_t new_ip,
                          uint32_t new_port,
                          uint8_t  new_mac[PPA_ETH_ALEN],
                          uint32_t new_src_mac_ix,
                          uint32_t mtu_ix,
                          uint32_t f_new_dscp_enable,
                          uint32_t new_dscp,
                          uint32_t f_vlan_ins_enable,
                          uint32_t new_vci,
                          uint32_t f_vlan_rm_enable,
                          uint32_t pppoe_mode,
                          uint32_t pppoe_ix,
                          uint32_t f_out_vlan_ins_enable,
                          uint32_t out_vlan_ix,
                          uint32_t f_out_vlan_rm_enable,
                          uint32_t dslwan_qid,
                          uint32_t dest_list,
                          uint32_t *p_entry) = NULL;


void (*ifx_ppa_drv_del_routing_entry_fn)(uint32_t entry) = NULL;

/*
 * ####################################
 *           Extern Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

int ifx_ppa_drv_proc_read_hal_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *strtitle[8] = {
        "ETH0", "ETH1", NULL, NULL, NULL, NULL, "ATM (IPoA/PPPoA)", "ATM (EoA)"
    };

    int len = 0;
    int i;

    for ( i = 0; i < sizeof(strtitle) / sizeof(*strtitle); i++ )
    {
        if ( !strtitle[i] )
            continue;

        len += sprintf(page + off + len,     "%s\n", strtitle[i]);
        len += sprintf(page + off + len,     "    ig_fast_brg_pkts:         %u\n", ITF_MIB_TBL(i)->ig_fast_brg_pkts);
        len += sprintf(page + off + len,     "    ig_fast_brg_bytes:        %u\n", ITF_MIB_TBL(i)->ig_fast_brg_bytes);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv4_udp_pkts: %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv4_udp_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv4_tcp_pkts: %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv4_tcp_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv4_mc_pkts:  %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv4_mc_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv4_bytes:    %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv4_bytes);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv6_udp_pkts: %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv6_udp_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv6_tcp_pkts: %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv6_tcp_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_ipv6_bytes:    %u\n", ITF_MIB_TBL(i)->ig_fast_rt_ipv6_bytes);
        len += sprintf(page + off + len,     "    ig_cpu_pkts:              %u\n", ITF_MIB_TBL(i)->ig_cpu_pkts);
        len += sprintf(page + off + len,     "    ig_cpu_bytes:             %u\n", ITF_MIB_TBL(i)->ig_cpu_bytes);
        len += sprintf(page + off + len,     "    ig_drop_pkts:             %u\n", ITF_MIB_TBL(i)->ig_drop_pkts);
        len += sprintf(page + off + len,     "    ig_drop_bytes:            %u\n", ITF_MIB_TBL(i)->ig_drop_bytes);
        len += sprintf(page + off + len,     "    eg_fast_pkts:             %u\n", ITF_MIB_TBL(i)->eg_fast_pkts);
    }

    *eof = 1;

    return len;
}

int ifx_ppa_drv_proc_write_hal_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;
    u32 eth_clear;
    int i;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        return 0;
    }

    eth_clear = 0;
    if ( stricmp(p, "clear") == 0 || stricmp(p, "clear all") == 0
        || stricmp(p, "clean") == 0 || stricmp(p, "clean all") == 0 )
        eth_clear = 0xFF;
    else if ( stricmp(p, "clear eth0") == 0 || stricmp(p, "clear 0") == 0
            || stricmp(p, "clean eth0") == 0 || stricmp(p, "clean 0") == 0 )
        eth_clear = 0x01;
    else if ( stricmp(p, "clear eth1") == 0 || stricmp(p, "clear 1") == 0
            || stricmp(p, "clean eth1") == 0 || stricmp(p, "clean 1") == 0 )
        eth_clear = 0x02;
    else if ( stricmp(p, "clear cpu") == 0 || stricmp(p, "clear 2") == 0
            || stricmp(p, "clean cpu") == 0 || stricmp(p, "clean 2") == 0 )
        eth_clear = 0x04;
    else if (stricmp(p, "clear 3") == 0 || stricmp(p, "clean 3") == 0 )
        eth_clear = 0x08;
    else if (stricmp(p, "clear 4") == 0 || stricmp(p, "clean 4") == 0 )
        eth_clear = 0x10;
    else if (stricmp(p, "clear 5") == 0 || stricmp(p, "clean 5") == 0 )
        eth_clear = 0x20;
    else if (stricmp(p, "clear 6") == 0 || stricmp(p, "clean 6") == 0 )
        eth_clear = 0x40;
    else if (stricmp(p, "clear 7") == 0 || stricmp(p, "clean 7") == 0 )
        eth_clear = 0x80;
    else if ((stricmp(p, "clear atm") == 0 || stricmp(p, "clean atm") == 0))
        eth_clear = 0x40 | 0x80;  /*clear 6 & 7 */
    else if (stricmp(p, "clear dpth") == 0 || stricmp(p, "clean dpth") == 0)
        eth_clear = 0x08 | 0x10 | 0x20;   /* 3 & 4 & 5*/

    for(i = 0; i < get_phys_port_num(); i ++)
    {
        if(eth_clear & (1 << i))
        {
            ppa_memset((void *)ITF_MIB_TBL(i), 0, sizeof(struct itf_mib));
        }
    }

#ifdef CONFIG_IFX_PPA_QOS
    if( eth_clear == 0xFF )
    {
        for ( i = 0; i < PPE_MAX_ETH1_QOS_QUEUE; i++ )
        {
            ppa_memset((void *)ETH_WAN_TX_MIB_TABLE(i), 0, sizeof(struct eth_wan_mib_table));
        }
    }
#endif

    return count;
}

int ifx_ppa_drv_proc_read_route(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    struct rout_forward_compare_tbl *pcompare;
    struct rout_forward_action_tbl *paction;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Wan Routing Table\n");
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;

    pcompare = (struct rout_forward_compare_tbl *)ROUT_WAN_HASH_CMP_TBL(0);
    paction = (struct rout_forward_action_tbl *)ROUT_WAN_HASH_ACT_TBL(0);
    for ( i = 0; i < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_route(str, i, 0, pcompare, paction);
            if ( len <= off && len + llen > off )
            {
                my_memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                my_memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        paction++;
    }

    pcompare = (struct rout_forward_compare_tbl *)ROUT_WAN_COLL_CMP_TBL(0);
    paction = (struct rout_forward_action_tbl *)ROUT_WAN_COLL_ACT_TBL(0);
    for ( i = 0; i < WAN_ROUT_TBL_CFG->rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_route(str, i + MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK, 0, pcompare, paction);
            if ( len <= off && len + llen > off )
            {
                my_memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                my_memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        paction++;
    }

    llen = sprintf(str, "Lan Routing Table\n");
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_ROUTE_OVERRUN_END;

    pcompare = (struct rout_forward_compare_tbl *)ROUT_LAN_HASH_CMP_TBL(0);
    paction = (struct rout_forward_action_tbl *)ROUT_LAN_HASH_ACT_TBL(0);
    for ( i = 0; i < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_route(str, i, 1, pcompare, paction);
            if ( len <= off && len + llen > off )
            {
                my_memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                my_memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        paction++;
    }

    pcompare = (struct rout_forward_compare_tbl *)ROUT_LAN_COLL_CMP_TBL(0);
    paction = (struct rout_forward_action_tbl *)ROUT_LAN_COLL_ACT_TBL(0);
    for ( i = 0; i < LAN_ROUT_TBL_CFG->rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_route(str, i + MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK, 1, pcompare, paction);
            if ( len <= off && len + llen > off )
            {
                my_memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                my_memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        paction++;
    }

    *eof = 1;

    return len - off;

PROC_READ_ROUTE_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_hal_route(struct file *file, const char *buf, unsigned long count, void *data)
{
    static const char *command[] = {
        "add",      //  0
        "del",      //  1
        "LAN",      //  2
        "WAN",      //  3
        "new",      //  4
        "src",      //  5
        "dest",     //  6
        "MAC",      //  7
        "route",    //  8
        "type",     //  9
        "DSCP",     //  10
        "MTU",      //  11
        "index",    //  12
        "VLAN",     //  13
        "insert",   //  14
        "remove",   //  15
        "list",     //  16
        "PPPoE",    //  17
        "mode",     //  18
        "ch",       //  19
        "id",       //  20
        "delete",   //  21
        "disable",  //  22
        "enable",   //  23
        "transparent",  //  24
        "termination",  //  25
        "NULL",     //  26
        "IPv4",     //  27
        "NAT",      //  28
        "NAPT",     //  29
        "entry",    //  30
        "tcp",      //  31
        "help",     //  32
        "vci",      //  33
        "yes",      //  34
        "no",       //  35
        "qid",      //  36
        "outer",    //  37
        "IP",       //  38
        "IPv6",     //  39
    };

    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "ATM"};
    static const int dest_list_strlen[] = {4, 4, 4, 8, 8, 8, 8, 3};

    int state;              //  1: new,
    int prev_cmd;
    int operation;          //  1: add, 2: delete
    int type;               //  1: LAN, 2: WAN, 0: auto detect
    int entry;
    struct rout_forward_compare_tbl compare_tbl;
    struct rout_forward_action_tbl action_tbl;
    uint32_t ipv6_src_ip[4] = {0}, ipv6_dst_ip[4] = {0};
    int is_ipv6 = 0;
    unsigned int val[20];
    char local_buf[1024];
    int len;
    char *p1, *p2;
    int colon;
    int i, j;
    u32 mask;
    u32 bit;
    u32 *pu1, *pu2, *pu3;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    state = 0;
    prev_cmd = 0;
    operation = 0;
    type = 0;
    entry = -1;

    ppa_memset(&compare_tbl, 0, sizeof(compare_tbl));
    ppa_memset(&action_tbl, 0, sizeof(action_tbl));

    p1 = local_buf;
    colon = 1;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        for ( i = 0; i < sizeof(command) / sizeof(*command); i++ )
            if ( stricmp(p1, command[i]) == 0 )
            {
                switch ( i )
                {
                case 0:
                    if ( !state && !operation )
                    {
                        operation = 1;
//                      printk("add\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 1:
                case 21:
                    if ( !state && !operation )
                    {
                        operation = 2;
//                      printk("delete\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 2:
                    if ( !state && !type )
                    {
                        type = 1;
//                      printk("lan\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 3:
                    if ( !state && !type )
                    {
                        type = 2;
//                      printk("wan\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 4:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 4;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 5:
                    if ( state == 1 )
                    {
                        if ( prev_cmd == 4 )
                        {
                            //  new src
                            if ( !type )
                                type = 1;

                            //  check for "new src mac index"
                            ignore_space(&p2, &len);
                            if ( strincmp(p2, "mac ", 4) == 0 )
                            {
                                state = 2;
                                prev_cmd = 5;
                                break;
                            }
                            else
                            {
                                get_ip_port(&p2, &len, val);
//                              printk("new src: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                                action_tbl.new_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                                action_tbl.new_port = val[4];
                            }
                        }
                        else
                            state = 0;
                    }
                    if ( state == 0 )
                    {
                        //  src
                        get_ip_port(&p2, &len, val);
//                      printk("src: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                        if ( val[5] == 4 )
                        {
                            if ( is_ipv6 )
                                ipv6_src_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            else
                                compare_tbl.src_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                        }
                        else
                        {
                            is_ipv6 = 1;
                            ipv6_src_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            ipv6_src_ip[1] = (val[6] << 24) | (val[7] << 16) | (val[8] << 8) | val[9];
                            ipv6_src_ip[2] = (val[10] << 24) | (val[11] << 16) | (val[12] << 8) | val[13];
                            ipv6_src_ip[3] = (val[14] << 24) | (val[15] << 16) | (val[16] << 8) | val[17];
                        }
                        compare_tbl.src_port = val[4];
                    }
                    state = prev_cmd = 0;
                    break;
                case 6:
                    if ( state == 1 )
                    {
                        if ( prev_cmd == 4 )
                        {
                            //  new dest
                            if ( !type )
                                type = 2;

                            get_ip_port(&p2, &len, val);
//                          printk("new dest: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                            action_tbl.new_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            action_tbl.new_port = val[4];
                        }
                        else
                            state = 0;
                    }
                    if ( state == 0 )
                    {
                        if ( !colon )
                        {
                            int llen;

                            llen = len;
                            p1 = p2;
                            while ( llen && *p1 <= ' ' )
                            {
                                llen--;
                                p1++;
                            }
                            if ( llen && (*p1 == ':' || (*p1 >= '0' && *p1 <= '9')) )
                                colon = 1;
                        }
                        if ( colon )
                        {
                            //  dest
                            get_ip_port(&p2, &len, val);
//                          printk("dest: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                            if ( val[5] == 4 )
                            {
                                if ( is_ipv6 )
                                    ipv6_dst_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                                else
                                    compare_tbl.dest_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            }
                            else
                            {
                                is_ipv6 = 1;
                                ipv6_dst_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                                ipv6_dst_ip[1] = (val[6] << 24) | (val[7] << 16) | (val[8] << 8) | val[9];
                                ipv6_dst_ip[2] = (val[10] << 24) | (val[11] << 16) | (val[12] << 8) | val[13];
                                ipv6_dst_ip[3] = (val[14] << 24) | (val[15] << 16) | (val[16] << 8) | val[17];
                            }
                            compare_tbl.dest_port = val[4];
                        }
                        else
                        {
                            state = 1;
                            prev_cmd = 6;
                            break;
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 7:
                    if ( state == 1 && prev_cmd == 4 )
                    {
                        //  new MAC
                        get_mac(&p2, &len, val);
//                      printk("new MAC: %02X.%02X.%02X.%02X:%02X:%02X\n", val[0], val[1], val[2], val[3], val[4], val[5]);
                        action_tbl.new_dest_mac54 = (val[0] << 8) | val[1];
                        action_tbl.new_dest_mac30 = (val[2] << 24) | (val[3] << 16) | (val[4] << 8) | val[5];
                    }
                    else if ( state == 2 && prev_cmd == 5 )
                    {
                        state = 3;
                        prev_cmd = 7;
                        break;
                    }
                    state = prev_cmd = 0;
                    break;
                case 8:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 8;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 9:
                    if ( state == 1 && prev_cmd == 8 )
                    {
                        state = 2;
                        prev_cmd = 9;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 10:
                    if ( state == 1 && prev_cmd == 4 )
                    {
                        ignore_space(&p2, &len);
                        if ( len && *p2 >= '0' && *p2 <= '9' )
                        {
                            //  new DSCP
                            val[0] = get_number(&p2, &len, 0);
//                          printk("new DSCP: %d\n", val[0]);
                            if ( !action_tbl.new_dscp_en )
                            {
                                action_tbl.new_dscp_en = 1;
                                action_tbl.new_dscp = val[0];
                            }
                        }
                        else if ( (len == 8 || (len > 8 && (p2[8] <= ' ' || p2[8] == ','))) && strincmp(p2, "original", 8) == 0 )
                        {
                            p2 += 8;
                            len -= 8;
                            //  new DSCP original
//                          printk("new DSCP: original\n");
                            //  the reset value is 0, so don't need to do anything
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 11:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 11;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 12:
                    if ( state == 1 )
                    {
                        if ( prev_cmd == 11 )
                        {
                            //  MTU index
                            ignore_space(&p2, &len);
                            val[0] = get_number(&p2, &len, 0);
//                          printk("MTU index: %d\n", val[0]);
                            action_tbl.mtu_ix = val[0];
                        }
                        else if ( prev_cmd == 13 )
                        {
                            //  VLAN insert enable
                            //  VLAN index
                            ignore_space(&p2, &len);
                            val[0] = get_number(&p2, &len, 0);
//                          printk("VLAN insert: enable, index %d\n", val[0]);
                            if ( !action_tbl.in_vlan_ins )
                            {
                                action_tbl.in_vlan_ins = 1;
                                action_tbl.new_in_vci = val[0];
                            }
                        }
                        else if ( prev_cmd == 17 )
                        {
                            //  PPPoE index
                            ignore_space(&p2, &len);
                            val[0] = get_number(&p2, &len, 0);
//                          printk("PPPoE index: %d\n", val[0]);
                            action_tbl.pppoe_ix = val[0];
                        }
                    }
                    else if ( state == 3 && prev_cmd == 7 )
                    {
                        //  new src mac index
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
                        action_tbl.new_src_mac_ix = val[0];
                    }
                    else if ( state == 2 && prev_cmd == 13 )
                    {
                        //  outer VLAN enable
                        //  outer VLAN index
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
//                      printk("outer VLAN insert: enable, index %d\n", val[0]);
                        if ( !action_tbl.out_vlan_ins )
                        {
                            action_tbl.out_vlan_ins = 1;
                            action_tbl.out_vlan_ix = val[0];
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 13:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 13;
                    }
                    else if ( state == 1 && prev_cmd == 37 )
                    {
                        state = 2;
                        prev_cmd = 13;
                        printk("outer vlan\n");
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 14:
                    if ( (state == 1 || state == 2) && prev_cmd == 13 )
                    {
                        state++;
                        prev_cmd = 14;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 15:
                    if ( (state == 1 || state == 2) && prev_cmd == 13 )
                    {
                        state++;
                        prev_cmd = 15;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 16:
                    if ( state == 1 && prev_cmd == 6 )
                    {
                        mask = 0;
                        do
                        {
                            ignore_space(&p2, &len);
                            if ( !len )
                                break;
                            for ( j = 0, bit = 1; j < sizeof(dest_list) / sizeof(*dest_list); j++, bit <<= 1 )
                                if ( (len == dest_list_strlen[j] || (len > dest_list_strlen[j] && (p2[dest_list_strlen[j]] <= ' ' || p2[dest_list_strlen[j]] == ','))) && strincmp(p2, dest_list[j], dest_list_strlen[j]) == 0 )
                                {
                                    p2 += dest_list_strlen[j];
                                    len -= dest_list_strlen[j];
                                    mask |= bit;
                                    break;
                                }
                        } while ( j < sizeof(dest_list) / sizeof(*dest_list) );
//                      if ( mask )
//                      {
//                          //  dest list
//                          printk("dest list:");
//                          for ( j = 0, bit = 1; j < sizeof(dest_list) / sizeof(*dest_list); j++, bit <<= 1 )
//                              if ( (mask & bit) )
//                              {
//                                  printk(" %s", dest_list[j]);
//                              }
//                          printk("\n");
//                      }
//                      else
//                          printk("dest list: none\n");
                        action_tbl.dest_list = mask;
                    }
                    state = prev_cmd = 0;
                    break;
                case 17:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 17;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 18:
                    if ( state == 1 && prev_cmd == 17 )
                    {
                        state = 2;
                        prev_cmd = 18;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 19:
                    if ( state == 1 && prev_cmd == 6 )
                    {
                        state = 2;
                        prev_cmd = 19;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 20:
                    if ( state == 2 && prev_cmd == 19 )
                    {
                        //  dest ch id
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
//                      printk("dest ch id: %d\n", val[0]);
                        //action_tbl.dest_chid = val[0];
                    }
                    state = prev_cmd = 0;
                    break;
                case 22:
                case 23:
                    if ( state == 2 )
                    {
                        if ( prev_cmd == 14 )
                        {
                            //  VLAN insert
//                          printk("VLAN insert: %s (%d)", command[i], i - 22);
                            if ( (i - 22) )
                            {
                                ignore_space(&p2, &len);
                                if ( len > 5 && (p2[5] <= ' ' || p2[5] == ':') && strincmp(p2, "index", 5) == 0 )
                                {
                                    p2 += 6;
                                    len -= 6;
                                    //  VLAN index
                                    ignore_space(&p2, &len);
                                    val[0] = get_number(&p2, &len, 0);
//                                  printk(", index %d", val[0]);
                                    if ( !action_tbl.in_vlan_ins )
                                    {
                                        action_tbl.in_vlan_ins = 1;
                                        //action_tbl.vlan_ix = val[0];
                                        action_tbl.new_in_vci = val[0];
                                    }
                                }
                                else if ( len > 3 && (p2[3] <= ' ' || p2[3] == ':') && strincmp(p2, "vci", 3) == 0 )
                                {
                                    p2 += 4;
                                    len -= 4;
                                    //  New VCI
                                    ignore_space(&p2, &len);
                                    val[0] = get_number(&p2, &len, 1);
//                                  printk(", vci 0x%04X", val[0]);
                                    if ( !action_tbl.in_vlan_ins )
                                    {
                                        action_tbl.in_vlan_ins = 1;
                                        action_tbl.new_in_vci = val[0];
                                    }
                                }
                            }
                            else
                            {
                                action_tbl.in_vlan_ins = 0;
                                action_tbl.new_in_vci = 0;
                            }
//                          printk("\n");
                        }
                        else if ( prev_cmd == 15 )
                        {
                            //  VLAN remove
//                          printk("VLAN remove: %s (%d)\n", command[i], i - 22);
                            action_tbl.in_vlan_rm = i - 22;
                        }
                    }
                    else if ( state == 3 )
                    {
                        if ( prev_cmd == 14 )
                        {
                            //  outer vlan insert
//                          printk("outer VLAN insert: %s (%d)", command[i], i - 22);
                            if ( (i - 22) )
                            {
                                ignore_space(&p2, &len);
                                if ( len > 5 && (p2[5] <= ' ' || p2[5] == ':') && strincmp(p2, "index", 5) == 0 )
                                {
                                    p2 += 6;
                                    len -= 6;
                                    //  outer VLAN index
                                    ignore_space(&p2, &len);
                                    val[0] = get_number(&p2, &len, 0);
//                                  printk(", index %d", val[0]);
                                    if ( !action_tbl.out_vlan_ins )
                                    {
                                        action_tbl.out_vlan_ins = 1;
                                        action_tbl.out_vlan_ix = val[0];
                                    }
                                }
                            }
                            else
                            {
                                action_tbl.out_vlan_ins = 0;
                                action_tbl.out_vlan_ix = 0;
                            }
//                          printk("\n");
                        }
                        else if ( prev_cmd == 15 )
                        {
                            //  outer VLAN remove
//                          printk("outer VLAN remove: %s (%d)\n", command[i], i - 22);
                            action_tbl.out_vlan_rm = i - 22;
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 24:
                case 25:
                    if ( state == 2 && prev_cmd == 18 )
                    {
                        //  PPPoE mode
//                      printk("PPPoE mode: %s (%d)\n", command[i], i - 24);
                        action_tbl.pppoe_mode = i - 24;
                    }
                    state = prev_cmd = 0;
                    break;
                case 38:
                case 39:
                    i = 27;
                case 26:
                case 27:
                case 28:
                case 29:
                    if ( state == 2 && prev_cmd == 9 )
                    {
                        //  route type
//                      printk("route type: %s (%d)\n", command[i], i - 26);
                        action_tbl.rout_type = i - 26;
                    }
                    state = prev_cmd = 0;
                    break;
                case 30:
                    if ( !state )
                    {
                        if ( entry < 0 )
                        {
                            ignore_space(&p2, &len);
                            if ( len && *p2 >= '0' && *p2 <= '9' )
                            {
                                entry = get_number(&p2, &len, 0);
                                //  entry
//                              printk("entry: %d\n", entry);
                            }
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 31:  // if this flag is not presented, it's UDP by default
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 31;
                        action_tbl.protocol = 1;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 32:
                    printk("add\n");
                    printk("  LAN/WAN entry ???\n");
                    printk("    compare\n");
                    printk("      src:  ???.???.???.???:????\n");
                    printk("      dest: ???.???.???.???:????\n");
                    printk("    action\n");
                    printk("      new src/dest:      ???.???.???.???:????\n");
                    printk("      new MAC:           ??:??:??:??:??:?? (HEX)\n");
                    printk("      route type:        NULL/IP/NAT/NAPT\n");
                    printk("      new DSCP:          original/??\n");
                    printk("      MTU index:         ??\n");
                    printk("      VLAN insert:       disable/enable, VCI ???? (HEX)\n");
                    printk("      VLAN remove:       disable/enable\n");
                    printk("      dest list:         ETH0/ETH1/CPU0/EXT_INT?\n");
                    printk("      PPPoE mode:        transparent/termination\n");
                    printk("      PPPoE index:       ??\n");
                    printk("      new src MAC index: ??\n");
                    printk("      tcp:               yes/no\n");
                    printk("      dest qid:          ??\n");
                    printk("\n");
                    printk("delete\n");
                    printk("  LAN/WAN entry ???\n");
                    printk("    compare\n");
                    printk("      src:  ???.???.???.???:????\n");
                    printk("      dest: ???.???.???.???:????\n");
                    printk("\n");
                    state = prev_cmd = 0;
                    break;
                case 33:
                    if ( state == 1 && prev_cmd == 13 )
                    {
                        //  vlan vci
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 1);
                        if ( !action_tbl.in_vlan_ins )
                        {
                            action_tbl.in_vlan_ins = 1;
                            action_tbl.new_in_vci = val[0];
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 34:
                    if ( state == 1 && prev_cmd == 31 )
                        //  tcp yes
                        action_tbl.protocol = 1;
                    state = prev_cmd = 0;
                    break;
                case 35:
                    if ( state == 1 && prev_cmd == 31 )
                        //  tcp no
                        action_tbl.protocol = 0;
                    state = prev_cmd = 0;
                    break;
                case 36:
                    if ( state == 1 && prev_cmd == 6 )
                    {
                        //  dest qid
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
                        action_tbl.dest_qid = val[0];
                    }
                    state = prev_cmd = 0;
                    break;
                case 37:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 37;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                default:
                    state = prev_cmd = 0;
                }

                break;
            }

        if ( i == sizeof(command) / sizeof(*command) )
            state = prev_cmd = 0;

        p1 = p2;
        colon = 1;
    }

    if ( operation == 2 )
    {
        u32 is_lan = 0;

        //  delete
        pu1 = (u32*)&compare_tbl;
        pu2 = NULL;
        pu3 = NULL;
        if ( entry < 0 )
        {
            //  search the entry number
            if ( *pu1 && pu1[1] )
            {
                if ( (!type || type == 1) )
                {
                    //  LAN
                    for ( entry = 0; entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK; entry++ )
                    {
                        pu2 = (u32*)ROUT_LAN_HASH_CMP_TBL(entry);
                        if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                        {
                            pu3 = (u32*)ROUT_LAN_HASH_ACT_TBL(entry);
                            break;
                        }
                    }
                    if ( entry == MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        for ( entry = 0; entry < LAN_ROUT_TBL_CFG->rout_num; entry++ )
                        {
                            pu2 = (u32*)ROUT_LAN_COLL_CMP_TBL(entry);
                            if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                            {
                                pu3 = (u32*)ROUT_LAN_COLL_ACT_TBL(entry);
                                break;
                            }
                        }
                        if ( entry == LAN_ROUT_TBL_CFG->rout_num )
                            pu2 = NULL;
                        else
                            entry += MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK;
                    }
                    if ( pu3 != NULL )
                        is_lan = 1;
                }
                if ( (!type && !pu2) || type == 2 )
                {
                    //  WAN
                    for ( entry = 0; entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK; entry++ )
                    {
                        pu2 = (u32*)ROUT_WAN_HASH_CMP_TBL(entry);
                        if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                        {
                            pu3 = (u32*)ROUT_WAN_HASH_ACT_TBL(entry);
                            break;
                        }
                    }
                    if ( entry == MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        for ( entry = 0; entry < WAN_ROUT_TBL_CFG->rout_num; entry++ )
                        {
                            pu2 = (u32*)ROUT_WAN_COLL_CMP_TBL(entry);
                            if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                            {
                                pu3 = (u32*)ROUT_WAN_COLL_ACT_TBL(entry);
                                break;
                            }
                        }
                        if ( entry == WAN_ROUT_TBL_CFG->rout_num )
                            pu2 = NULL;
                        else
                            entry += MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK;
                    }
                    if ( pu3 != NULL )
                        is_lan = 0;
                }
            }
        }
        else
        {
            if ( *pu1 && pu1[1] )
            {
                pu3 = NULL;
                //  check compare
                if ( !type || type == 1 )
                {
                    //  LAN
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_LAN_HASH_CMP_TBL(entry);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_LAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + LAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_LAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_LAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 1;
                }
                if ( !type || type == 2 )
                {
                    //  WAN
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_WAN_HASH_CMP_TBL(entry);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_WAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + WAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_WAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_WAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 0;
                }
            }
            else if ( !*pu1 && !pu1[1] )
            {
                if ( type == 1 )
                {
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_LAN_HASH_CMP_TBL(entry);
                        pu3 = (u32*)ROUT_LAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + LAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_LAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        pu3 = (u32*)ROUT_LAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 1;
                }
                else if ( type == 2 )
                {
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_WAN_HASH_CMP_TBL(entry);
                        pu3 = (u32*)ROUT_WAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + WAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_WAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        pu3 = (u32*)ROUT_WAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 0;
                }
            }
        }
#if 0
        if ( pu2 )
        {
            *pu2 = 0;
            *(pu2 + 1) = 0;
            *(pu2 + 2) = 0;
        }
        if ( pu3 )
        {
            *pu3 = 0;
            *(pu3 + 1) = 0;
            *(pu3 + 2) = 0;
            *(pu3 + 3) = 0;
            *(pu3 + 4) = 0;
            *(pu3 + 5) = 0;
        }
#else
        if ( pu2 && pu3 )
        {
            is_lan <<= 31;
            if(ifx_ppa_drv_del_routing_entry_fn != NULL)
                ifx_ppa_drv_del_routing_entry_fn((u32)entry | is_lan);
        }
#endif
    }
    else if ( operation == 1 && type && ((!is_ipv6 && *(u32*)&compare_tbl && *((u32*)&compare_tbl + 1)) || (is_ipv6 && *((u32*)&compare_tbl + 2))) )
    {
#if 0   //  TODO
        pu2 = NULL;
        if ( entry < 0 )
        {
            int max_entry;

            //  add
            pu1 = (u32*)&compare_tbl;
            pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
            max_entry = type == 1 ? LAN_ROUT_TBL_CFG->lan_rout_num : WAN_ROUT_TBL_CFG->wan_rout_num;
            for ( entry = 0; entry < max_entry; entry++, pu2 += 4 )
                if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                    break;
            if ( entry == max_entry )
            {
                pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
                for ( entry = 0; entry < max_entry; entry++, pu2 += 4 )
                    if ( !*pu2 && !pu2[1] )
                        break;
                if ( entry == max_entry )
                    entry = -1;
            }
        }
        else
        {
            int max_entry;
            int tmp_entry;

            //  replace
            pu1 = (u32*)&compare_tbl;
            pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
            max_entry = type == 1 ? LAN_ROUT_TBL_CFG->lan_rout_num : WAN_ROUT_TBL_CFG->wan_rout_num;
            for ( tmp_entry = 0; tmp_entry < max_entry; tmp_entry++, pu2 += 4 )
                if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                    break;
            if ( tmp_entry < max_entry )
                entry = tmp_entry;
            else if ( !(type == 1 && entry < LAN_ROUT_TBL_CFG->lan_rout_num) && !(type == 2 && entry < WAN_ROUT_TBL_CFG->wan_rout_num) )
                entry = -1;
        }

        if ( entry >= 0 )
        {
            //  add or replace
            if ( type == 1 )
            {
                pu1 = (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(entry);
                pu2 = (u32*)LAN_ROUT_FORWARD_ACTION_TBL(entry);
            }
            else
            {
                pu1 = (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(entry);
                pu2 = (u32*)WAN_ROUT_FORWARD_ACTION_TBL(entry);
            }
            my_memcpy((unsigned char *)pu1, (unsigned char *)&compare_tbl, sizeof(compare_tbl));
            my_memcpy((unsigned char *)pu2, (unsigned char *)&action_tbl, sizeof(action_tbl));
        }
#else
        int test_entry = -1;
        u8 test_mac[PPA_ETH_ALEN] = {0};
        u32 dest_list = action_tbl.dest_list;
        int result_add = -1;

        test_mac[0] = (action_tbl.new_dest_mac54 >> 8) & 0xFF;
        test_mac[1] = action_tbl.new_dest_mac54 & 0xFF;
        test_mac[2] = (action_tbl.new_dest_mac30 >> 24) & 0xFF;
        test_mac[3] = (action_tbl.new_dest_mac30 >> 16) & 0xFF;
        test_mac[4] = (action_tbl.new_dest_mac30 >> 8) & 0xFF;
        test_mac[5] = action_tbl.new_dest_mac30 & 0xFF;
        //if ( (dest_list & (1 << 7)) )
        //    dest_list = (dest_list & ~(1 << 7)) | IFX_PPA_DEST_LIST_ATM;

        if ( is_ipv6 )
        {
            if(ifx_ppa_drv_add_ipv6_routing_entry_fn != NULL)
                result_add = ifx_ppa_drv_add_ipv6_routing_entry_fn(type == 1 ? 1 : 0,
                                                    ipv6_src_ip,
                                                    compare_tbl.src_port,
                                                    ipv6_dst_ip,
                                                    compare_tbl.dest_port,
                                                    action_tbl.protocol,    //  1: TCP, 0: UDP
                                                    action_tbl.rout_type,
                                                    action_tbl.new_ip,
                                                    action_tbl.new_port,
                                                    test_mac,
                                                    action_tbl.new_src_mac_ix,
                                                    action_tbl.mtu_ix,
                                                    action_tbl.new_dscp_en,
                                                    action_tbl.new_dscp,
                                                    action_tbl.in_vlan_ins,
                                                    action_tbl.new_in_vci,
                                                    action_tbl.in_vlan_rm,
                                                    action_tbl.pppoe_mode,
                                                    action_tbl.pppoe_ix,
                                                    action_tbl.out_vlan_ins,
                                                    action_tbl.out_vlan_ix,
                                                    action_tbl.out_vlan_rm,
                                                    action_tbl.dest_qid,
                                                    dest_list,
                                                    //action_tbl.dest_list | IFX_PPA_DEST_LIST_NO_REMAP,
                                                    &test_entry);
        }
        else
        {
            if(ifx_ppa_drv_add_routing_entry_fn != NULL)
                result_add = ifx_ppa_drv_add_routing_entry_fn(type == 1 ? 1 : 0,
                                               compare_tbl.src_ip,
                                               compare_tbl.src_port,
                                               compare_tbl.dest_ip,
                                               compare_tbl.dest_port,
                                               action_tbl.protocol,    //  1: TCP, 0: UDP
                                               action_tbl.rout_type,
                                               action_tbl.new_ip,
                                               action_tbl.new_port,
                                               test_mac,
                                               action_tbl.new_src_mac_ix,
                                               action_tbl.mtu_ix,
                                               action_tbl.new_dscp_en,
                                               action_tbl.new_dscp,
                                               action_tbl.in_vlan_ins,
                                               action_tbl.new_in_vci,
                                               action_tbl.in_vlan_rm,
                                               action_tbl.pppoe_mode,
                                               action_tbl.pppoe_ix,
                                               action_tbl.out_vlan_ins,
                                               action_tbl.out_vlan_ix,
                                               action_tbl.out_vlan_rm,
                                               action_tbl.dest_qid,
                                               dest_list,
                                               //action_tbl.dest_list | IFX_PPA_DEST_LIST_NO_REMAP,
                                               &test_entry);
        }

        if ( !result_add )
            //printk("%s entry add successfully: entry = %d\n", type == 1 ? "LAN" : "WAN", test_entry);
            printk("%s entry add successfully: entry = %d\n", (test_entry & (1 << 31)) ? "LAN" : "WAN", test_entry & ~(1 << 31));
        else
            printk("%s entry add fail\n", type == 1 ? "LAN" : "WAN");
#endif
    }

    return count;
}

int ifx_ppa_drv_proc_read_mc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    volatile struct wan_rout_multicast_cmp_tbl *pcompare;
    volatile struct wan_rout_multicast_act_tbl *paction;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Multicast Table\n");
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;

    pcompare = ROUT_WAN_MC_CMP_TBL(0);
    paction = ROUT_WAN_MC_ACT_TBL(0);
    for ( i = 0; i < MAX_WAN_MC_ENTRIES; i++ )
    {
        if ( pcompare->wan_dest_ip )
        {
            llen = print_mc(str, i, (struct wan_rout_multicast_cmp_tbl *)pcompare, (struct wan_rout_multicast_act_tbl *)paction);
            if ( len <= off && len + llen > off )
            {
                my_memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                my_memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_MC_OVERRUN_END;
        }

        pcompare++;
        paction++;
    }

    *eof = 1;

    return len - off;

PROC_READ_MC_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_read_genconf(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen = 0;
    int i;
    unsigned long bit;

    pstr = *start = page;

    __sync();

    llen += sprintf(str + llen, "CFG_WRX_HTUTS    (0x%08X): %d\n", (u32)CFG_WRX_HTUTS, *CFG_WRX_HTUTS);
    llen += sprintf(str + llen, "CFG_WRDES_DELAY  (0x%08X): %d\n", (u32)CFG_WRDES_DELAY, *CFG_WRDES_DELAY);
    llen += sprintf(str + llen, "WRX_EMACH_ON     (0x%08X): AAL - %s, OAM - %s\n", (u32)WRX_EMACH_ON, (*WRX_EMACH_ON & 0x01) ? "on" : "off",
                                                                                                      (*WRX_EMACH_ON & 0x02) ? "on" : "off");
    llen += sprintf(str + llen, "WTX_EMACH_ON     (0x%08X):", (u32)WTX_EMACH_ON);
    for ( i = 0, bit = 1; i < 15; i++, bit <<= 1 )
    {
        llen += sprintf(str + llen, " %d - %s", i, (*WTX_EMACH_ON & bit) ? "on " : "off");
        if ( i == 3 || i == 7 || i == 11 )
            llen += sprintf(str + llen, "\n                               ");
    }
    llen += sprintf(str + llen, "\n");
    llen += sprintf(str + llen, "WRX_HUNT_BITTH   (0x%08X): %d\n", (u32)WRX_HUNT_BITTH, *WRX_HUNT_BITTH);

    llen += sprintf(str + llen, "ETH_PORTS_CFG    (0x%08X): WAN VLAN ID Hi - %d, WAN VLAN ID Lo - %d\n", (u32)ETH_PORTS_CFG, ETH_PORTS_CFG->wan_vlanid_hi, ETH_PORTS_CFG->wan_vlanid_lo);

    llen += sprintf(str + llen, "LAN_ROUT_TBL_CFG (0x%08X): num - %d, TTL check - %s, no hit drop - %s\n",
                                                            (u32)LAN_ROUT_TBL_CFG,
                                                            LAN_ROUT_TBL_CFG->rout_num,
                                                            LAN_ROUT_TBL_CFG->ttl_en ? "on" : "off",
                                                            LAN_ROUT_TBL_CFG->rout_drop  ? "on" : "off"
                                                            );
    llen += sprintf(str + llen, "                               IP checksum - %s, TCP/UDP checksum - %s, checksum error drop - %s\n",
                                                            LAN_ROUT_TBL_CFG->ip_ver_en ? "on" : "off",
                                                            LAN_ROUT_TBL_CFG->tcpdup_ver_en ? "on" : "off",
                                                            LAN_ROUT_TBL_CFG->iptcpudperr_drop ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "WAN_ROUT_TBL_CFG (0x%08X): num - %d, TTL check - %s, no hit drop - %s\n",
                                                            (u32)WAN_ROUT_TBL_CFG,
                                                            WAN_ROUT_TBL_CFG->rout_num,
                                                            WAN_ROUT_TBL_CFG->ttl_en ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->rout_drop  ? "on" : "off"
                                                            );
    llen += sprintf(str + llen, "                               MC num - %d, MC drop - %s\n",
                                                            WAN_ROUT_TBL_CFG->wan_rout_mc_num,
                                                            WAN_ROUT_TBL_CFG->wan_rout_mc_drop ? "on" : "off"
                                                            );
    llen += sprintf(str + llen, "                               IP checksum - %s, TCP/UDP checksum - %s, checksum error drop - %s\n",
                                                            WAN_ROUT_TBL_CFG->ip_ver_en ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->tcpdup_ver_en ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->iptcpudperr_drop ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "GEN_MODE_CFG1    (0x%08X): App2 - %s, U/S - %s, U/S early discard - %s\n"
                                "                               classification table entry - %d, IPv6 route entry - %d\n"
                                "                               session based rate control - %s, IPv4 WAN ingress hash alg - %s\n"
                                "                               multiple field based classification and VLAN assignment for bridging - %s\n"
                                "                               D/S IPv4 multicast acceleration - %s, IPv6 acceleration - %s\n"
                                "                               WAN acceleration - %s, LAN acceleration - %s, switch isolation - %s\n",
                                                            (u32)GEN_MODE_CFG1,
                                                            GEN_MODE_CFG1->app2_indirect ? "indirect" : "direct",
                                                            GEN_MODE_CFG1->us_indirect ? "indirect" : "direct",
                                                            GEN_MODE_CFG1->us_early_discard_en ? "enable" : "disable",
                                                            GEN_MODE_CFG1->classification_num,
                                                            GEN_MODE_CFG1->ipv6_rout_num,
                                                            GEN_MODE_CFG1->session_ctl_en ? "enable" : "disable",
                                                            GEN_MODE_CFG1->wan_hash_alg ? "src IP" : "dest port",
                                                            GEN_MODE_CFG1->brg_class_en ? "enable" : "disable",
                                                            GEN_MODE_CFG1->ipv4_mc_acc_mode ? "IP/port pairs" : "dst IP only",
                                                            GEN_MODE_CFG1->ipv6_acc_en ? "enable" : "disable",
                                                            GEN_MODE_CFG1->wan_acc_en ? "enable" : "disable",
                                                            GEN_MODE_CFG1->lan_acc_en ? "enable" : "disable",
                                                            GEN_MODE_CFG1->sw_iso_mode ? "isolated" : "not isolated"
                                                            );
    llen += sprintf(str + llen, "WAN Interfaces:                %s \n", get_wanitf(1));
    llen += sprintf(str + llen, "LAN Interfaces:                %s \n", get_wanitf(0));
    llen += sprintf(str + llen, "GEN_MODE_CFG2    (0x%08X):", (u32)GEN_MODE_CFG2);
    for ( i = 0, bit = 1; i < 8; i++, bit <<= 1 )
    {
        llen += sprintf(str + llen, " %d - %s", i, (GEN_MODE_CFG2->itf_outer_vlan_vld & bit) ? "on " : "off");
        if ( i == 3 )
            llen += sprintf(str + llen, "\n                              ");
    }
    llen += sprintf(str + llen, "\n");

    llen += sprintf(str + llen, "TX_QOS_CFG       (0x%08X): time_tick - %d, overhd_bytes - %d, eth1_eg_qnum - %d\n"
                                "                               eth1_burst_chk - %s, eth1 - %s, rate shaping - %s, WFQ - %s\n",
                                                            (u32)TX_QOS_CFG,
                                                            TX_QOS_CFG->time_tick,
                                                            TX_QOS_CFG->overhd_bytes,
                                                            TX_QOS_CFG->eth1_eg_qnum,
                                                            TX_QOS_CFG->eth1_burst_chk ? "on" : "off",
                                                            TX_QOS_CFG->eth1_qss ? "FW QoS" : "HW QoS",
                                                            TX_QOS_CFG->shape_en ? "on" : "off",
                                                            TX_QOS_CFG->wfq_en ? "on" : "off");

    llen += sprintf(str + llen, "KEY_SEL_n        (0x%08X): %08x %08x %08x %08x\n",
                                                            (u32)KEY_SEL_n(0),
                                                            *KEY_SEL_n(0), *KEY_SEL_n(1), *KEY_SEL_n(2), *KEY_SEL_n(3));

    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_GENCONF_OVERRUN_END;

    *eof = 1;

    return len - off;

PROC_READ_GENCONF_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_genconf(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    int f_wan_hi = 0;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        return 0;
    }

    if ( strincmp(p, "wan hi ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        f_wan_hi = 1;
    }
    else if ( strincmp(p, "wan high ", 9) == 0 )
    {
        p += 9;
        rlen -= 9;
        f_wan_hi = 1;
    }
    else if ( strincmp(p, "wan lo ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        f_wan_hi = -1;
    }
    else if ( strincmp(p, "wan low ", 8) == 0 )
    {
        p += 8;
        rlen -= 8;
        f_wan_hi = -1;
    }
    else if ( strincmp(p, "eth0 type ", 10) == 0 )
    {
        p += 10;
        rlen -= 10;
        if ( stricmp(p, "lan") == 0 )
            ETH_PORTS_CFG->eth0_type = 0;
        else if ( stricmp(p, "wan") == 0 )
            ETH_PORTS_CFG->eth0_type = 1;
        else if ( stricmp(p, "mix") == 0 )
            ETH_PORTS_CFG->eth0_type = 2;
    }

    if ( f_wan_hi )
    {
        int num;

        num = get_number(&p, &rlen, 0);
        if ( f_wan_hi > 0 )
            ETH_PORTS_CFG->wan_vlanid_hi = num;
        else
            ETH_PORTS_CFG->wan_vlanid_lo = num;
    }

    return count;
}

int ifx_ppa_drv_proc_read_pppoe(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "PPPoE Table (0x%08X) - Session ID:\n", (u32)PPPOE_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *PPPOE_CFG_TBL(i));
        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_PPPOE_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_PPPOE_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_pppoe(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

int ifx_ppa_drv_proc_read_mtu(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "MTU Table (0x%08X):\n", (u32)MTU_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *MTU_CFG_TBL(i));
        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_MTU_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_MTU_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_mtu(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

int ifx_ppa_drv_proc_read_hit(char *page, char **start, off_t off, int count, int *eof, void *data)
{
   int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;
    int n;
    unsigned long bit;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "Unicast Routing Hit Table (0x%08X):\n", (u32)ROUT_LAN_HASH_HIT_STAT_TBL(0));
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "             1 2 3 4 5 6 7 8 9 10\n");
    pstr += llen;
    len += llen;

    n = 1;
    for ( i = 0; i < 8; i++ )
        for ( bit = 0x80000000; bit; bit >>= 1 )
        {
            if ( n % 10 == 1 )
                llen = sprintf(str, "  %3d - %3d:", n, n + 9);

            llen += sprintf(str + llen, " %d", (*ROUT_LAN_HASH_HIT_STAT_TBL(i) & bit) ? 1 : 0);

            if ( n++ % 10 == 0 )
            {
                llen += sprintf(str + llen, "\n");

                if ( len <= off && len + llen > off )
                {
                    my_memcpy(pstr, str + off - len, len + llen - off);
                    pstr += len + llen - off;
                }
                else if ( len > off )
                {
                    my_memcpy(pstr, str, llen);
                    pstr += llen;
                }
                len += llen;
                if ( len >= len_max )
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_HIT_OVERRUN_END;
    }

    llen = sprintf(str, "Multicast Routing Hit Table (0x%08X):\n", (u32)ROUT_WAN_MC_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_HIT_OVERRUN_END;

    n = 1;
    for ( i = 0; i < 1; i++ )
        for ( bit = 0x80000000; bit; bit >>= 1 )
        {
            if ( n % 10 == 1 )
                llen = sprintf(str, "  %3d - %3d:", n, n + 9);

            llen += sprintf(str + llen, " %d", (*ROUT_WAN_MC_HIT_STAT_TBL(i) & bit) ? 1 : 0);

            if ( n++ % 10 == 0 )
            {
                llen += sprintf(str + llen, "\n");

                if ( len <= off && len + llen > off )
                {
                    my_memcpy(pstr, str + off - len, len + llen - off);
                    pstr += len + llen - off;
                }
                else if ( len > off )
                {
                    my_memcpy(pstr, str, llen);
                    pstr += llen;
                }
                len += llen;
                if ( len >= len_max )
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_HIT_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_HIT_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_hit(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        return 0;
    }

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clean") == 0 )
    {
        ppa_memset((void*)ROUT_LAN_HASH_HIT_STAT_TBL(0), 0, 64);
        ppa_memset((void*)ROUT_LAN_COLL_HIT_STAT_TBL(0), 0, 2);
        ppa_memset((void*)ROUT_WAN_HASH_HIT_STAT_TBL(0), 0, 64);
        ppa_memset((void*)ROUT_WAN_COLL_HIT_STAT_TBL(0), 0, 2);
        ppa_memset((void*)ROUT_WAN_MC_HIT_STAT_TBL(0), 0, 2);
    }

    return count;
}

int ifx_ppa_drv_proc_read_mac(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;
    unsigned int mac52, mac10;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "MAC Table:\n");
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  ROUT_MAC_CFG_TBL (0x%08X)\n", (u32)ROUT_MAC_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 16; i++ )
    {
        mac52 = *ROUT_MAC_CFG_TBL(i);
        mac10 = *(ROUT_MAC_CFG_TBL(i) + 1);

        llen = sprintf(str, "    %2d: %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1, mac52 >> 24, (mac52 >> 16) & 0xFF, (mac52 >> 8) & 0xFF, mac52 & 0xFF, mac10 >> 24, (mac10 >> 16) & 0xFF);
        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_MAC_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_mac(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

int ifx_ppa_drv_proc_read_out_vlan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "Outer VLAN Table (0x%08X):\n", (u32)OUTER_VLAN_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 32; i++ )
    {
        llen = sprintf(str, "  %d: 0x%08X\n", i, *OUTER_VLAN_TBL(i));
        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_MAC_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    return len - llen - off;
}

int ifx_ppa_drv_proc_write_out_vlan(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

int ifx_ppa_drv_proc_read_ipv6_ip(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;

    int i, j, x;
    volatile unsigned char *p;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "IPv6 IP Table:\n");
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;

    for ( x = 0; x < MAX_IPV6_IP_ENTRIES_BLOCK; x++ )
        for ( i = 0; i < MAX_IPV6_IP_ENTRIES_PER_BLOCK; i++ )
            if ( IPv6_IP_IDX_TBL(x, i)[0] != 0 || IPv6_IP_IDX_TBL(x, i)[1] != 0 || IPv6_IP_IDX_TBL(x, i)[2] != 0 || IPv6_IP_IDX_TBL(x, i)[3] != 0 )
            {
                p = (volatile char *)IPv6_IP_IDX_TBL(x, i);
                llen = sprintf(str, "%3d - %u", x * MAX_IPV6_IP_ENTRIES_PER_BLOCK + i, (unsigned int)p[0]);
                for ( j = 1; j < 16; j++ )
                    llen += sprintf(str + llen, ".%u", (unsigned int)p[j]);
                llen += sprintf(str + llen, "\n");

                if ( len <= off && len + llen > off )
                {
                    my_memcpy(pstr, str + off - len, len + llen - off);
                    pstr += len + llen - off;
                }
                else if ( len > off )
                {
                    my_memcpy(pstr, str, llen);
                    pstr += llen;
                }
                len += llen;
                if ( len >= len_max )
                    goto PROC_READ_IPv6_IP_OVERRUN_END;
            }

    *eof = 1;

    return len - off;

PROC_READ_IPv6_IP_OVERRUN_END:
    return len - llen - off;
}

static char * get_wanitf(int iswan)
{
    static char itfs[64];
    char *allitfs[] = {
        "ETH0",
        "ETH1",
        "", //reserve for CPU
        "EXT1",
        "EXT2",
        "EXT3",
        "EXT4",
        "EXT5",
        NULL
    };

    int wan_itf = *CFG_WAN_PORTMAP;
    int wan_mixmap = *CFG_MIXED_PORTMAP;
    int i, len = 0;

    int max_exts_idx = 3 + 2;

    memset(itfs, 0, sizeof(itfs));
    if(wan_mixmap != 0 && !iswan){ //mix mode dont have lan
        return itfs;
    }

    for(i = 0; allitfs[i] != NULL ; i ++){
        if(i == 2) continue; //skip CPU port

        if((iswan && (wan_itf & (1 << i))) || (!iswan && !(wan_itf & (1 << i)))){
            if(i > max_exts_idx){
                if(!iswan){
                    break; //DSL cannot be lan
                }
                else{
                    len += sprintf(itfs + len, "%s ", "DSL");
                    break; //DSL only listed once although in A5 we have 2 ports support DSL
                }
            }

            len += sprintf(itfs + len, "%s ", allitfs[i]);

            if(iswan && (wan_mixmap &  (1 << i))){
                len += sprintf(itfs + len, "(Mixed) ");
            }
        }
    }

    return itfs;
}

static INLINE int stricmp(const char *p1, const char *p2)
{
    int c1, c2;

    while ( *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
    }

    return *p1 - *p2;
}

static INLINE int strincmp(const char *p1, const char *p2, int n)
{
    int c1 = 0, c2;

    while ( n && *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
        n--;
    }

    return n ? *p1 - *p2 : c1;
}

static INLINE int get_token(char **p1, char **p2, int *len, int *colon)
{
    int tlen = 0;

    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z') || (**p1 >= '0' && **p1<= '9')) )
    {
        (*p1)++;
        (*len)--;
    }
    if ( !*len )
        return 0;

    if ( *colon )
    {
        *colon = 0;
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            if ( **p2 == ':' )
            {
                *colon = 1;
                break;
            }
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }
    else
    {
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }

    return tlen;
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

static INLINE void get_mac(char **p, int *len, unsigned int *val)
{
    int i;

    ppa_memset(val, 0, sizeof(*val) * PPA_ETH_ALEN);

    for ( i = 0; i < PPA_ETH_ALEN; i++ )
    {
        ignore_space(p, len);
        if ( !*len )
            break;
        val[i] = get_number(p, len, 1);
    }
}

static INLINE void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}

static INLINE uint32_t get_phys_port_num(void)
{
        return 8;
}

#if defined(ENABLE_MY_MEMCPY) && ENABLE_MY_MEMCPY
static INLINE void *my_memcpy(unsigned char *dest, const unsigned char *src, unsigned int count)
{
    char *d = (char *)dest, *s = (char *)src;

    if (count >= 32) {
        int i = 8 - (((unsigned long) d) & 0x7);

        if (i != 8)
            while (i-- && count--) {
                *d++ = *s++;
            }

        if (((((unsigned long) d) & 0x7) == 0) &&
                ((((unsigned long) s) & 0x7) == 0)) {
            while (count >= 32) {
                unsigned long long t1, t2, t3, t4;
                t1 = *(unsigned long long *) (s);
                t2 = *(unsigned long long *) (s + 8);
                t3 = *(unsigned long long *) (s + 16);
                t4 = *(unsigned long long *) (s + 24);
                *(unsigned long long *) (d) = t1;
                *(unsigned long long *) (d + 8) = t2;
                *(unsigned long long *) (d + 16) = t3;
                *(unsigned long long *) (d + 24) = t4;
                d += 32;
                s += 32;
                count -= 32;
            }
            while (count >= 8) {
                *(unsigned long long *) d =
                                            *(unsigned long long *) s;
                d += 8;
                s += 8;
                count -= 8;
            }
        }

        if (((((unsigned long) d) & 0x3) == 0) &&
                ((((unsigned long) s) & 0x3) == 0)) {
            while (count >= 4) {
                *(unsigned long *) d = *(unsigned long *) s;
                d += 4;
                s += 4;
                count -= 4;
            }
        }

        if (((((unsigned long) d) & 0x1) == 0) &&
                ((((unsigned long) s) & 0x1) == 0)) {
            while (count >= 2) {
                *(unsigned short *) d = *(unsigned short *) s;
                d += 2;
                s += 2;
                count -= 2;
            }
        }
    }

    while (count--) {
        *d++ = *s++;
    }

    return d;
}
#endif

static INLINE int print_route(char *buf, int i, int f_is_lan, struct rout_forward_compare_tbl *pcompare, struct rout_forward_action_tbl *pwaction)
{
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "res", "ATM"};
    static const char *mpoa_type[] = {"EoA without FCS", "EoA with FCS", "PPPoA", "IPoA"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d\n", i);
    len += sprintf(buf + len,          "    compare (0x%08X)\n", (u32)pcompare);
    len += sprintf(buf + len,          "      src:  %d.%d.%d.%d:%d\n", pcompare->src_ip >> 24,  (pcompare->src_ip >> 16) & 0xFF,  (pcompare->src_ip >> 8) & 0xFF,  pcompare->src_ip & 0xFF, pcompare->src_port);
    len += sprintf(buf + len,          "      dest: %d.%d.%d.%d:%d\n", pcompare->dest_ip >> 24, (pcompare->dest_ip >> 16) & 0xFF, (pcompare->dest_ip >> 8) & 0xFF, pcompare->dest_ip & 0xFF, pcompare->dest_port);
    len += sprintf(buf + len,          "    action  (0x%08X)\n", (u32)pwaction);
    len += sprintf(buf + len,          "      new %s    %d.%d.%d.%d:%d\n", f_is_lan ? "src :" : "dest:", pwaction->new_ip >> 24, (pwaction->new_ip >> 16) & 0xFF, (pwaction->new_ip >> 8) & 0xFF, pwaction->new_ip & 0xFF, pwaction->new_port);
    len += sprintf(buf + len,          "      new MAC :    %02X:%02X:%02X:%02X:%02X:%02X\n", (pwaction->new_dest_mac54 >> 8) & 0xFF, pwaction->new_dest_mac54 & 0xFF, pwaction->new_dest_mac30 >> 24, (pwaction->new_dest_mac30 >> 16) & 0xFF, (pwaction->new_dest_mac30 >> 8) & 0xFF, pwaction->new_dest_mac30 & 0xFF);
    switch ( pwaction->rout_type )
    {
    case 1:  len += sprintf(buf + len, "      route type:  IP\n");   break;
    case 2:  len += sprintf(buf + len, "      route type:  NAT\n");  break;
    case 3:  len += sprintf(buf + len, "      route type:  NAPT\n"); break;
    default: len += sprintf(buf + len, "      route type:  NULL\n");
    }
    if ( pwaction->new_dscp_en )
        len += sprintf(buf + len,      "      new DSCP:    %d\n", pwaction->new_dscp);
    else
        len += sprintf(buf + len,      "      new DSCP:    original (not modified)\n");
    len += sprintf(buf + len,          "      MTU index:   %d\n", pwaction->mtu_ix);
    if ( pwaction->in_vlan_ins )
        len += sprintf(buf + len,      "      VLAN insert: enable, VCI 0x%04x\n", pwaction->new_in_vci);
    else
        len += sprintf(buf + len,      "      VLAN insert: disable\n");
    len += sprintf(buf + len,          "      VLAN remove: %s\n", pwaction->in_vlan_rm ? "enable" : "disable");
    if ( !pwaction->dest_list )
        len += sprintf(buf + len,      "      dest list:   none\n");
    else
    {
        len += sprintf(buf + len,      "      dest list:   ");
        for ( bit = 1, j = k = 0; bit < 1 << 8; bit <<= 1, j++ )
            if ( (pwaction->dest_list & bit) )
            {
                if ( k )
                    len += sprintf(buf + len, ", ");
                len += sprintf(buf + len, dest_list[j]);
                k = 1;
            }
        len += sprintf(buf + len, "\n");
    }
    if ( pwaction->pppoe_mode )
    {
        len += sprintf(buf + len,      "      PPPoE mode:  termination\n");
        if ( f_is_lan )
            len += sprintf(buf + len,  "      PPPoE index: %d\n", pwaction->pppoe_ix);
    }
    else
        len += sprintf(buf + len,      "      PPPoE mode:  transparent\n");
    len += sprintf(buf + len,          "      new src MAC index: %d\n", pwaction->new_src_mac_ix);
    if ( pwaction->out_vlan_ins )
        len += sprintf(buf + len,      "      outer VLAN insert: enable, index %d\n", pwaction->out_vlan_ix);
    else
        len += sprintf(buf + len,      "      outer VLAN insert: disable\n");
    if ( !f_is_lan)
        len += sprintf(buf + len,      "      outer VLAN remove: %s (forced to be enable for ATM PVC differentiation)\n", pwaction->out_vlan_rm ? "enable" : "disable");
    else
        len += sprintf(buf + len,      "      outer VLAN remove: %s\n", pwaction->out_vlan_rm ? "enable" : "disable");
    len += sprintf(buf + len,          "      tcp:         %s\n", pwaction->protocol ? "yes" : "no (UDP)");
    len += sprintf(buf + len,          "      entry valid: %s\n", pwaction->entry_vld ? "yes" : "no");
    if ( f_is_lan )
    {
        len += sprintf(buf + len,      "      mpoa type:   %s\n", mpoa_type[pwaction->mpoa_type]);
        len += sprintf(buf + len,      "      dslwan qid:  %d (conn %d)\n", pwaction->dest_qid & 0xFF, (pwaction->dest_qid >> 8) & 0xFF);
    }
    else
        len += sprintf(buf + len,      "      dest qid (dslwan qid): %d\n", pwaction->dest_qid);
    len += sprintf(buf + len,          "      accl bytes: %u\n", pwaction->bytes);

    return len;
}

static INLINE int print_mc(char *buf, int i, struct wan_rout_multicast_cmp_tbl *pcompare, struct wan_rout_multicast_act_tbl *paction)
{
    static const char *rout_type[] = {"no IP level editing", "TTL editing", "reserved - 2", "reserved - 3"};
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "res", "ATM"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d - %s\n", i, paction->entry_vld ? "valid" : "invalid");
    len += sprintf(buf + len,          "    compare (0x%08X)\n", (u32)pcompare);
    len += sprintf(buf + len,          "      wan_dest_ip:  %d.%d.%d.%d\n", (pcompare->wan_dest_ip >> 24) & 0xFF,  (pcompare->wan_dest_ip >> 16) & 0xFF,  (pcompare->wan_dest_ip >> 8) & 0xFF,  pcompare->wan_dest_ip & 0xFF);
    len += sprintf(buf + len,          "    action  (0x%08X)\n", (u32)paction);
    len += sprintf(buf + len,          "      rout_type:    %s\n", rout_type[paction->rout_type]);
    if ( paction->new_dscp_en )
        len += sprintf(buf + len,      "      new DSCP:     %d\n", paction->new_dscp);
    else
        len += sprintf(buf + len,      "      new DSCP:     original (not modified)\n");
    if ( paction->in_vlan_ins )
        len += sprintf(buf + len,      "      VLAN insert:  enable, VCI 0x%04x\n", paction->new_in_vci);
    else
        len += sprintf(buf + len,      "      VLAN insert:  disable\n");
    len += sprintf(buf + len,          "      VLAN remove:  %s\n", paction->in_vlan_rm ? "enable" : "disable");
    if ( !paction->dest_list )
        len += sprintf(buf + len,      "      dest list:    none\n");
    else
    {
        len += sprintf(buf + len,      "      dest list:   ");
        for ( bit = 1, j = k = 0; bit < 1 << 8; bit <<= 1, j++ )
            if ( (paction->dest_list & bit) )
            {
                if ( k )
                    len += sprintf(buf + len, ", ");
                len += sprintf(buf + len, dest_list[j]);
                k = 1;
            }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len,          "      PPPoE mode:   %s\n", paction->pppoe_mode ? "termination" : "transparent");
    if ( paction->new_src_mac_en )
        len += sprintf(buf + len,      "      new src MAC index: %d\n", paction->new_src_mac_ix);
    else
        len += sprintf(buf + len,      "      new src MAC index: disabled\n");
    if ( paction->out_vlan_ins )
        len += sprintf(buf + len,      "      outer VLAN insert: enable, index %d\n", paction->out_vlan_ix);
    else
        len += sprintf(buf + len,      "      outer VLAN insert: disable\n");
    len += sprintf(buf + len,          "      outer VLAN remove: %s\n", paction->out_vlan_rm ? "enable" : "disable");
    len += sprintf(buf + len,          "      dest_qid:          %d\n", paction->dest_qid);

    return len;
}


/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */
#if 0
void ifx_ppa_drv_proc_init(uint32_t ipv6_en)
{
    g_ipv6_acc_en = ipv6_en ? 1: 0;

    g_ipv4_lan_collision_rout_fwdc_table_base = __IPV4_LAN_COLLISION_ROUT_FWDC_TABLE_BASE;
    g_ipv4_wan_collision_rout_fwda_table_base = __IPV4_WAN_COLLISION_ROUT_FWDA_TABLE_BASE;
    ifx_ppa_drv_add_ipv6_routing_entry_fn = NULL;
    ifx_ppa_drv_add_routing_entry_fn = NULL;
    ifx_ppa_drv_del_routing_entry_fn = NULL;
}
#endif

