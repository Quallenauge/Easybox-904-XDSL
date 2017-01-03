/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_core.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 3 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Implementation
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 03 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */

#define VER_FAMILY      0x60        //  bit 0: res
                                    //      1: Danube
                                    //      2: Twinpass
                                    //      3: Amazon-SE
                                    //      4: res
                                    //      5: AR9
                                    //      6: GR9
#define VER_DRTYPE      0x20        //  bit 0: Normal Data Path driver
                                    //      1: Indirect-Fast Path driver
                                    //      2: HAL driver
                                    //      3: Hook driver
                                    //      4: Stack/System Adaption Layer driver
                                    //      5: PPA API driver
#define VER_INTERFACE   0x07        //  bit 0: MII 0
                                    //      1: MII 1
                                    //      2: ATM WAN
                                    //      3: PTM WAN
#define VER_ACCMODE     0x03        //  bit 0: Routing
                                    //      1: Bridging
#define VER_MAJOR       0
#define VER_MID         0
#define VER_MINOR       4



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/autoconf.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/version.h>
//#include <linux/types.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#if defined(CONFIG_IFX_PPA_API_PROC)
//#include <linux/proc_fs.h>
//#endif
//#include <linux/netdevice.h>
//#include <linux/in.h>
//#include <net/sock.h>
//#include <net/ip_vs.h>
//#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include "ifx_ppa_ss.h"
#include "ifx_ppa_api_common.h"
#include <net/ifx_ppa_api.h>
#include "ifx_ppe_drv_wrapper.h"
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppa_hook.h"
#include "ifx_ppa_api_core.h"
#include "ifx_ppa_api_tools.h"
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
#include "ifx_ppa_api_pwm.h"
#endif
#ifdef CONFIG_IFX_PPA_MFE
#include "ifx_ppa_api_mfe.h"
#endif
#ifdef CONFIG_IFX_PPA_QOS
#include "ifx_ppa_api_qos.h"
#endif
#include "ifx_ppa_api_mib.h"

/*
 * ####################################
 *              Definition
 * ####################################
 */

#define MIN_HITS                        2


/*
 * ####################################
 *              Data Type
 * ####################################
 */


/*
 * ####################################
 *           Global Variable
 * ####################################
 */

static uint32_t g_init = 0;

static uint32_t g_broadcast_bridging_entry = ~0;

static uint32_t g_min_hits = MIN_HITS;
static uint8_t g_bridging_mac_learning = 1;
static uint8_t mc_updating_flag = 0;

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

static int32_t ppa_init(PPA_INIT_INFO *p_info, uint32_t flags)
{
    int32_t ret;
    uint8_t broadcast_mac[PPA_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32_t i;
    PPA_MAX_ENTRY_INFO entry={0};
    PPE_FAST_MODE_CFG fast_mode={0};
    PPE_WAN_VID_RANGE vlan_id;
    PPE_ACC_ENABLE acc_cfg={0};
    PPE_BR_MAC_INFO br_mac={0};

    ifx_ppa_drv_get_max_entries(&entry, 0);

    ret = IFX_EINVAL;
    if ( p_info->max_lan_source_entries + p_info->max_wan_source_entries > (entry.max_lan_entries + entry.max_wan_entries ))
    {
        dbg("Two many entries:%d > %d",  p_info->max_lan_source_entries + p_info->max_wan_source_entries , (entry.max_lan_entries + entry.max_wan_entries ) );
        goto MAX_SOURCE_ENTRIES_ERROR;
    }
    if ( p_info->max_mc_entries > entry.max_mc_entries)
    {
        dbg("Two many multicast entries:%d > %d",   p_info->max_mc_entries , entry.max_mc_entries);
        goto MAX_MC_ENTRIES_ERROR;
    }
    if ( p_info->max_bridging_entries > entry.max_bridging_entries)
    {
        dbg("Two many bridge entries:%d > %d",   p_info->max_bridging_entries , entry.max_bridging_entries);
        goto MAX_BRG_ENTRIES_ERROR;
    }

    //  disable accelation mode by default
    acc_cfg.f_is_lan = 1;
    acc_cfg.f_enable = IFX_PPA_ACC_MODE_NONE;
    if( ifx_ppa_drv_set_acc_mode( &acc_cfg, 0) != IFX_SUCCESS )
    {
        dbg("ifx_ppa_drv_set_acc_mode lan fail\n");
    }
    acc_cfg.f_is_lan = 0;
    acc_cfg.f_enable = IFX_PPA_ACC_MODE_NONE;
    if( ifx_ppa_drv_set_acc_mode( &acc_cfg, 0) != IFX_SUCCESS )
    {
        dbg("ifx_ppa_drv_set_acc_mode  wan fail\n");
    }

    if( ppa_hook_list_init() != IFX_SUCCESS )
    {
        dbg("ppa_hook_list_init  fail\n");
        goto MAX_BRG_ENTRIES_ERROR;
    }

    if ( (entry.max_lan_entries + entry.max_wan_entries ) || entry.max_mc_entries )
    {
        PPE_ROUTING_CFG cfg;

        //set LAN acceleration
        ppa_memset( &cfg, 0, sizeof(cfg));
        cfg.f_is_lan = 1;
        cfg.entry_num = p_info->max_lan_source_entries;
        cfg.mc_entry_num = 0;
        cfg.f_ip_verify = p_info->lan_rx_checks.f_ip_verify;
        cfg.f_tcpudp_verify=p_info->lan_rx_checks.f_tcp_udp_verify;
        cfg.f_tcpudp_err_drop=p_info->lan_rx_checks.f_tcp_udp_err_drop;
        cfg.f_drop_on_no_hit = p_info->lan_rx_checks.f_drop_on_no_hit;
        cfg.f_mc_drop_on_no_hit = 0;
        cfg.flags = IFX_PPA_SET_ROUTE_CFG_ENTRY_NUM | IFX_PPA_SET_ROUTE_CFG_IP_VERIFY | IFX_PPA_SET_ROUTE_CFG_TCPUDP_VERIFY | IFX_PPA_SET_ROUTE_CFG_DROP_ON_NOT_HIT;
        if( ifx_ppa_drv_set_route_cfg(&cfg, 0) != IFX_SUCCESS )
            dbg("ifx_ppa_drv_set_route_cfg lan fail\n");

        //set WAN acceleration
        ppa_memset( &cfg, 0, sizeof(cfg));
        cfg.f_is_lan = 0;
        cfg.entry_num = p_info->max_wan_source_entries;
        cfg.mc_entry_num = p_info->max_mc_entries;
        cfg.f_ip_verify = p_info->wan_rx_checks.f_ip_verify;
        cfg.f_tcpudp_verify=p_info->wan_rx_checks.f_tcp_udp_verify;
        cfg.f_tcpudp_err_drop=p_info->wan_rx_checks.f_tcp_udp_err_drop;
        cfg.f_drop_on_no_hit = p_info->wan_rx_checks.f_drop_on_no_hit;
        cfg.f_mc_drop_on_no_hit =  p_info->wan_rx_checks.f_mc_drop_on_no_hit;
        cfg.flags = IFX_PPA_SET_ROUTE_CFG_ENTRY_NUM | IFX_PPA_SET_ROUTE_CFG_MC_ENTRY_NUM | IFX_PPA_SET_ROUTE_CFG_IP_VERIFY | IFX_PPA_SET_ROUTE_CFG_TCPUDP_VERIFY | IFX_PPA_SET_ROUTE_CFG_DROP_ON_NOT_HIT | IFX_PPA_SET_ROUTE_CFG_MC_DROP_ON_NOT_HIT;
        if( ifx_ppa_drv_set_route_cfg( &cfg, 0) != IFX_SUCCESS )
            dbg("ifx_ppa_drv_set_route_cfg wan fail\n");
    }

    if ( entry.max_bridging_entries )
    {
        PPE_BRDG_CFG br_cfg;
        PPE_COUNT_CFG count={0};
        if( ifx_ppa_drv_get_number_of_phys_port( &count, 0) == IFX_SUCCESS )
        {
            dbg("ifx_ppa_drv_get_number_of_phys_port  fail\n");
        }

        ppa_memset( &br_cfg, 0, sizeof(br_cfg));
        br_cfg.entry_num = p_info->max_bridging_entries;
        br_cfg.br_to_src_port_mask = (1 << count.num) - 1;   //  br_to_src_port_mask
        br_cfg.flags = IFX_PPA_SET_BRIDGING_CFG_ENTRY_NUM | IFX_PPA_SET_BRIDGING_CFG_BR_TO_SRC_PORT_EN | IFX_PPA_SET_BRIDGING_CFG_DEST_VLAN_EN | IFX_PPA_SET_BRIDGING_CFG_SRC_VLAN_EN | IFX_PPA_SET_BRIDGING_CFG_MAC_CHANGE_DROP;

        if( ifx_ppa_drv_set_bridging_cfg( &br_cfg, 0) != IFX_SUCCESS )
            dbg("ifx_ppa_drv_set_bridging_cfg  fail\n");
    }

    if ( (ret = ppa_api_netif_manager_init()) != IFX_SUCCESS || !IFX_PPA_IS_PORT_CPU0_AVAILABLE() )
    {
        if( ret != IFX_SUCCESS )
            dbg("ppa_api_netif_manager_init  fail\n");
        else
            dbg("CPU0 not available\n");
        goto PPA_API_NETIF_CREATE_INIT_FAIL;
    }

    if ( (ret = ppa_api_session_manager_init()) != IFX_SUCCESS )
    {
        dbg("ppa_api_session_manager_init  fail\n");
        goto PPA_API_SESSION_MANAGER_INIT_FAIL;
    }
    for ( i = 0; i < p_info->num_lanifs; i++ )
        if ( p_info->p_lanifs[i].ifname != NULL && ppa_netif_add(p_info->p_lanifs[i].ifname, 1, NULL) != IFX_SUCCESS )
            err("Failed in adding LAN side network interface - %s, reason could be no sufficient memory or LAN/WAN rule violation with physical network interface.", p_info->p_lanifs[i].ifname);
    for ( i = 0; i < p_info->num_wanifs; i++ )
        if ( p_info->p_wanifs[i].ifname != NULL && ppa_netif_add(p_info->p_wanifs[i].ifname, 0, NULL) != IFX_SUCCESS )
            err("Failed in adding WAN side network interface - %s, reason could be no sufficient memory or LAN/WAN rule violation with physical network interface.", p_info->p_wanifs[i].ifname);

/***** network interface mode will not be set by PPA API
    it should be configured by low level driver (eth/atm driver)
    #ifdef CONFIG_IFX_PPA_A4
        set_if_type(eth0_iftype, 0);
        set_wan_vlan_id((0x010 << 16) | 0x000);  //  high 16 bits is upper of WAN VLAN ID, low 16 bits is min WAN VLAN ID
    #else
        set_if_type(eth0_iftype, 0);
        set_if_type(eth1_iftype, 1);
    #endif
*/
    //  this is default setting for LAN/WAN mix mode to use VLAN tag to differentiate LAN/WAN traffic
    vlan_id.vid =(  0x010 << 16) | 0x000; //  high 16 bits is upper of WAN VLAN ID, low 16 bits is min WAN VLAN ID
    if( ifx_ppa_drv_set_mixed_wan_vlan_id( &vlan_id, 0) != IFX_SUCCESS )//  high 16 bits is upper of WAN VLAN ID, low 16 bits is min WAN VLAN ID
        dbg("ifx_ppa_drv_set_mixed_wan_vlan_id fail\n");

/*** WFQ is hidden and not to be changed after firmware running. ***
  set_if_wfq(ETX_WFQ_D4, 1);
  set_fastpath_wfq(FASTPATH_WFQ_D4);
  set_dplus_wfq(DPLUS_WFQ_D4);
*/
    for ( i = 0; i < 8; i++ )
    {
        PPE_DEST_LIST cfg;
        cfg.uc_dest_list = 1 << IFX_PPA_PORT_CPU0;
        cfg.mc_dest_list = 1 << IFX_PPA_PORT_CPU0;
        cfg.if_no = i;
        if( ifx_ppa_drv_set_default_dest_list( &cfg, 0 ) != IFX_SUCCESS )
            dbg("ifx_ppa_drv_set_default_dest_list fail\n");
    }

#if defined(CONFIG_AR9) || defined(CONFIG_VR9)
    fast_mode.mode = IFX_PPA_SET_FAST_MODE_CPU1_INDIRECT | IFX_PPA_SET_FAST_MODE_ETH1_DIRECT;
    fast_mode.flags = IFX_PPA_SET_FAST_MODE_CPU1 | IFX_PPA_SET_FAST_MODE_ETH1;
    if( ifx_ppa_drv_set_fast_mode( &fast_mode, 0 ) != IFX_SUCCESS )
        dbg("ifx_ppa_drv_set_fast_mode fail\n");
#else
    fast_mode.mode = IFX_PPA_SET_FAST_MODE_CPU1_DIRECT | IFX_PPA_SET_FAST_MODE_ETH1_DIRECT;
    fast_mode.flags = IFX_PPA_SET_FAST_MODE_CPU1 | IFX_PPA_SET_FAST_MODE_ETH1;
    if( ifx_ppa_drv_set_fast_mode( &fast_mode, 0 ) != IFX_SUCCESS )
        dbg("ifx_ppa_drv_set_fast_mode fail\n");
#endif

    br_mac.port = IFX_PPA_PORT_CPU0;
    ppa_memcpy(br_mac.mac, broadcast_mac, sizeof(br_mac.mac));
    br_mac.f_src_mac_drop = 0;
    br_mac.dslwan_qid = 0;
    br_mac.dest_list = IFX_PPA_DEST_LIST_CPU0;
    br_mac.p_entry = g_broadcast_bridging_entry;

    if( ifx_ppa_drv_add_bridging_entry(&br_mac, 0) != IFX_SUCCESS )
        dbg("ifx_ppa_drv_add_bridging_entry broadcast fail\n");
    g_broadcast_bridging_entry = br_mac.p_entry;

    g_min_hits = p_info->add_requires_min_hits;

    ppa_hook_enable_fn              = ifx_ppa_enable;
    ppa_hook_get_status_fn          = ifx_ppa_get_status;

    ppa_add_hook_map( "ppa_hook_enable_fn", (uint32_t )&ppa_hook_enable_fn, (uint32_t )ppa_hook_enable_fn);
    ppa_add_hook_map( "ppa_hook_get_status_fn", (uint32_t ) &ppa_hook_get_status_fn, (uint32_t ) ppa_hook_get_status_fn);

    if ( (entry.max_lan_entries + entry.max_wan_entries ) )
    {
        ppa_hook_session_add_fn         = ifx_ppa_session_add;
        ppa_hook_session_del_fn         = ifx_ppa_session_delete;
        ppa_hook_session_modify_fn      = ifx_ppa_session_modify;
        ppa_hook_session_get_fn         = ifx_ppa_session_get;

        ppa_add_hook_map( "ppa_hook_session_add_fn", (uint32_t ) &ppa_hook_session_add_fn, (uint32_t ) ppa_hook_session_add_fn);
        ppa_add_hook_map( "ppa_hook_session_del_fn", (uint32_t ) &ppa_hook_session_del_fn, (uint32_t ) ppa_hook_session_del_fn);
        ppa_add_hook_map(  "ppa_hook_session_modify_fn", (uint32_t ) &ppa_hook_session_modify_fn, (uint32_t ) ppa_hook_session_modify_fn);
        ppa_add_hook_map(  "ppa_hook_session_get_fn", (uint32_t ) &ppa_hook_session_get_fn, (uint32_t ) ppa_hook_session_get_fn);
    }

    if ( entry.max_mc_entries )
    {
        ppa_hook_mc_group_update_fn     = ifx_ppa_mc_group_update;
        ppa_hook_mc_group_get_fn        = ifx_ppa_mc_group_get;
        ppa_hook_mc_entry_modify_fn     = ifx_ppa_mc_entry_modify;
        ppa_hook_mc_entry_get_fn        = ifx_ppa_mc_entry_get;
        ppa_hook_multicast_pkt_srcif_add_fn = ifx_ppa_multicast_pkt_srcif_add;

        ppa_add_hook_map( "ppa_hook_mc_group_update_fn", (uint32_t ) &ppa_hook_mc_group_update_fn, (uint32_t ) ppa_hook_mc_group_update_fn);
        ppa_add_hook_map( "ppa_hook_mc_group_get_fn", (uint32_t ) &ppa_hook_mc_group_get_fn, (uint32_t ) ppa_hook_mc_group_get_fn);
        ppa_add_hook_map( "ppa_hook_mc_entry_modify_fn", (uint32_t ) &ppa_hook_mc_entry_modify_fn, (uint32_t ) ppa_hook_mc_entry_modify_fn);
        ppa_add_hook_map( "ppa_hook_mc_entry_get_fn", (uint32_t ) &ppa_hook_mc_entry_get_fn, (uint32_t ) ppa_hook_mc_entry_get_fn);
        ppa_add_hook_map( "ppa_hook_multicast_pkt_srcif_add_fn", (uint32_t ) &ppa_hook_multicast_pkt_srcif_add_fn, (uint32_t ) ppa_hook_multicast_pkt_srcif_add_fn);
    }

    if ( entry.max_lan_entries + entry.max_wan_entries || entry.max_mc_entries )
    {
        ppa_hook_inactivity_status_fn   = ifx_ppa_inactivity_status;
        ppa_hook_set_inactivity_fn      = ifx_ppa_set_session_inactivity;

        ppa_add_hook_map(  "ppa_hook_inactivity_status_fn", (uint32_t ) &ppa_hook_inactivity_status_fn, (uint32_t ) ppa_hook_inactivity_status_fn);
        ppa_add_hook_map( "ppa_hook_set_inactivity_fn", (uint32_t ) &ppa_hook_set_inactivity_fn, (uint32_t ) ppa_hook_set_inactivity_fn);
    }

    if ( entry.max_bridging_entries )
    {
        ppa_hook_bridge_entry_add_fn               = ifx_ppa_bridge_entry_add;
        ppa_hook_bridge_entry_delete_fn            = ifx_ppa_bridge_entry_delete;
        ppa_hook_bridge_entry_hit_time_fn          = ifx_ppa_bridge_entry_hit_time;
        ppa_hook_bridge_entry_inactivity_status_fn = ifx_ppa_bridge_entry_inactivity_status;
        ppa_hook_set_bridge_entry_timeout_fn       = ifx_ppa_set_bridge_entry_timeout;
        ppa_hook_bridge_enable_fn                  = ifx_ppa_hook_bridge_enable;

        ppa_hook_set_bridge_if_vlan_config_fn       = ifx_ppa_set_bridge_if_vlan_config;
        ppa_hook_get_bridge_if_vlan_config_fn       = ifx_ppa_get_bridge_if_vlan_config;
        ppa_hook_vlan_filter_add_fn                 = ifx_ppa_vlan_filter_add;
        ppa_hook_vlan_filter_del_fn                 = ifx_ppa_vlan_filter_del;
        ppa_hook_vlan_filter_get_all_fn             = ifx_ppa_vlan_filter_get_all;
        ppa_hook_vlan_filter_del_all_fn             = ifx_ppa_vlan_filter_del_all;

        ppa_add_hook_map( "ppa_hook_bridge_entry_add_fn", (uint32_t ) &ppa_hook_bridge_entry_add_fn, (uint32_t ) ppa_hook_bridge_entry_add_fn);
        ppa_add_hook_map( "ppa_hook_bridge_entry_delete_fn", (uint32_t ) &ppa_hook_bridge_entry_delete_fn, (uint32_t ) ppa_hook_bridge_entry_delete_fn);
        ppa_add_hook_map( "ppa_hook_bridge_entry_hit_time_fn", (uint32_t ) &ppa_hook_bridge_entry_hit_time_fn, (uint32_t ) ppa_hook_bridge_entry_hit_time_fn);
        ppa_add_hook_map( "ppa_hook_bridge_entry_inactivity_status_fn", (uint32_t ) &ppa_hook_bridge_entry_inactivity_status_fn, (uint32_t ) ppa_hook_bridge_entry_inactivity_status_fn);
        ppa_add_hook_map( "ppa_hook_set_bridge_entry_timeout_fn", (uint32_t ) &ppa_hook_set_bridge_entry_timeout_fn, (uint32_t ) &ppa_hook_set_bridge_entry_timeout_fn);
        ppa_add_hook_map( "ppa_hook_bridge_enable_fn", (uint32_t ) &ppa_hook_bridge_enable_fn, (uint32_t ) ppa_hook_bridge_enable_fn);

        ppa_add_hook_map( "ppa_hook_set_bridge_if_vlan_config_fn", (uint32_t ) &ppa_hook_set_bridge_if_vlan_config_fn, (uint32_t ) ppa_hook_set_bridge_if_vlan_config_fn);
        ppa_add_hook_map( "ppa_hook_get_bridge_if_vlan_config_fn", (uint32_t ) &ppa_hook_get_bridge_if_vlan_config_fn, (uint32_t ) ppa_hook_get_bridge_if_vlan_config_fn);
        ppa_add_hook_map( "ppa_hook_vlan_filter_add_fn", (uint32_t ) &ppa_hook_vlan_filter_add_fn,(uint32_t ) ppa_hook_vlan_filter_add_fn);
        ppa_add_hook_map( "ppa_hook_vlan_filter_del_fn", (uint32_t ) &ppa_hook_vlan_filter_del_fn, (uint32_t ) ppa_hook_vlan_filter_del_fn);
        ppa_add_hook_map( "ppa_hook_vlan_filter_get_all_fn", (uint32_t ) &ppa_hook_vlan_filter_get_all_fn, (uint32_t ) ppa_hook_vlan_filter_get_all_fn);
        ppa_add_hook_map( "ppa_hook_vlan_filter_del_fn", (uint32_t ) &ppa_hook_vlan_filter_del_fn, (uint32_t ) ppa_hook_vlan_filter_del_fn);

    }

    ppa_hook_get_if_stats_fn        = ifx_ppa_get_if_stats;
    ppa_hook_get_accel_stats_fn     = ifx_ppa_get_accel_stats;
    ppa_add_hook_map("ppa_hook_get_if_stats_fn",  (uint32_t ) &ppa_hook_get_if_stats_fn,  (uint32_t ) ppa_hook_get_if_stats_fn);
    ppa_add_hook_map( "ppa_hook_get_accel_stats_fn", (uint32_t ) &ppa_hook_get_accel_stats_fn, (uint32_t ) ppa_hook_get_accel_stats_fn);

    ppa_hook_set_if_mac_address_fn  = ifx_ppa_set_if_mac_address;
    ppa_hook_get_if_mac_address_fn  = ifx_ppa_get_if_mac_address;
    ppa_add_hook_map( "ppa_hook_set_if_mac_address_fn", (uint32_t ) &ppa_hook_set_if_mac_address_fn, (uint32_t ) ppa_hook_set_if_mac_address_fn);
    ppa_add_hook_map( "ppa_hook_get_if_mac_address_fn", (uint32_t ) &ppa_hook_get_if_mac_address_fn, (uint32_t ) ppa_hook_get_if_mac_address_fn);

    ppa_hook_add_if_fn              = ifx_ppa_add_if;
    ppa_hook_del_if_fn              = ifx_ppa_del_if;
    ppa_hook_get_if_fn              = ifx_ppa_get_if;

    ppa_add_hook_map( "ppa_hook_add_if_fn", (uint32_t ) &ppa_hook_add_if_fn, (uint32_t ) ppa_hook_add_if_fn);
    ppa_add_hook_map( "ppa_hook_del_if_fn", (uint32_t ) &ppa_hook_del_if_fn, (uint32_t ) ppa_hook_del_if_fn);
    ppa_add_hook_map( "ppa_hook_get_if_fn", (uint32_t ) &ppa_hook_get_if_fn, (uint32_t ) ppa_hook_get_if_fn);



#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
    ppa_hook_directpath_register_dev_fn = ifx_ppa_directpath_register_dev;
    ppa_hook_directpath_send_fn         = ifx_ppa_directpath_send;
    ppa_hook_directpath_rx_stop_fn      = ifx_ppa_directpath_rx_stop;
    ppa_hook_directpath_rx_restart_fn   = ifx_ppa_directpath_rx_restart;
    ppa_hook_get_netif_for_ppa_ifid_fn  = ifx_ppa_get_netif_for_ppa_ifid;
    ppa_hook_get_ifid_for_netif_fn      = ifx_ppa_get_ifid_for_netif;

    ppa_add_hook_map( "ppa_hook_directpath_register_dev_fn", (uint32_t ) &ppa_hook_directpath_register_dev_fn, (uint32_t ) ppa_hook_directpath_register_dev_fn);
    ppa_add_hook_map( "ppa_hook_directpath_send_fn", (uint32_t ) &ppa_hook_directpath_send_fn, (uint32_t ) ppa_hook_directpath_send_fn);
    ppa_add_hook_map( "ppa_hook_directpath_rx_stop_fn", (uint32_t ) &ppa_hook_directpath_rx_stop_fn, (uint32_t ) ppa_hook_directpath_rx_stop_fn);
    ppa_add_hook_map( "ppa_hook_directpath_rx_restart_fn", (uint32_t ) &ppa_hook_directpath_rx_restart_fn, (uint32_t ) ppa_hook_directpath_rx_restart_fn);
    ppa_add_hook_map( "ppa_hook_get_netif_for_ppa_ifid_fn", (uint32_t ) &ppa_hook_get_netif_for_ppa_ifid_fn, (uint32_t ) ppa_hook_get_netif_for_ppa_ifid_fn);
    ppa_add_hook_map( "ppa_hook_get_ifid_for_netif_fn", (uint32_t ) &ppa_hook_get_ifid_for_netif_fn, (uint32_t ) ppa_hook_get_ifid_for_netif_fn);
#endif

    ppa_hook_wan_mii0_vlan_range_add_fn = ifx_ppa_hook_wan_mii0_vlan_range_add;
    ppa_hook_wan_mii0_vlan_range_del_fn = ifx_ppa_hook_wan_mii0_vlan_range_del;
    ppa_hook_wan_mii0_vlan_ranges_get_fn= ifx_ppa_hook_wan_mii0_vlan_ranges_get;

    ppa_add_hook_map( "ppa_hook_wan_mii0_vlan_range_add_fn", (uint32_t ) &ppa_hook_wan_mii0_vlan_range_add_fn, (uint32_t ) ppa_hook_wan_mii0_vlan_range_add_fn);
    ppa_add_hook_map( "ppa_hook_wan_mii0_vlan_range_del_fn", (uint32_t ) &ppa_hook_wan_mii0_vlan_range_del_fn, (uint32_t ) ppa_hook_wan_mii0_vlan_range_del_fn);
    ppa_add_hook_map( "ppa_hook_wan_mii0_vlan_ranges_get_fn", (uint32_t ) &ppa_hook_wan_mii0_vlan_ranges_get_fn, (uint32_t ) ppa_hook_wan_mii0_vlan_ranges_get_fn);

#ifdef CONFIG_IFX_PPA_QOS
    ifx_ppa_hook_get_qos_qnum = ifx_ppa_get_qos_qnum;
    ppa_add_hook_map( "ifx_ppa_hook_get_qos_qnum", (uint32_t ) &ifx_ppa_hook_get_qos_qnum, (uint32_t ) ifx_ppa_hook_get_qos_qnum);

    ifx_ppa_hook_get_qos_mib = ifx_ppa_get_qos_mib;
    ppa_add_hook_map( "ifx_ppa_hook_get_qos_mib", (uint32_t ) &ifx_ppa_hook_get_qos_mib, (uint32_t ) ifx_ppa_hook_get_qos_mib);
#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
    ifx_ppa_hook_set_ctrl_qos_rate= ifx_ppa_set_ctrl_qos_rate;
    ifx_ppa_hook_get_ctrl_qos_rate = ifx_ppa_get_ctrl_qos_rate;
    ifx_ppa_hook_set_qos_rate = ifx_ppa_set_qos_rate;
    ifx_ppa_hook_get_qos_rate = ifx_ppa_get_qos_rate;
    ifx_ppa_hook_reset_qos_rate = ifx_ppa_reset_qos_rate;

    ppa_add_hook_map( "ifx_ppa_hook_set_ctrl_qos_rate", (uint32_t ) &ifx_ppa_hook_set_ctrl_qos_rate, (uint32_t ) ifx_ppa_hook_set_ctrl_qos_rate);
    ppa_add_hook_map( "ifx_ppa_hook_get_ctrl_qos_rate", (uint32_t ) &ifx_ppa_hook_get_ctrl_qos_rate, (uint32_t ) ifx_ppa_hook_get_ctrl_qos_rate);
    ppa_add_hook_map( "ifx_ppa_hook_set_qos_rate",      (uint32_t ) &ifx_ppa_hook_set_qos_rate,       (uint32_t ) ifx_ppa_hook_set_qos_rate);
    ppa_add_hook_map( "ifx_ppa_hook_get_qos_rate",      (uint32_t ) &ifx_ppa_hook_get_qos_rate,       (uint32_t ) ifx_ppa_hook_get_qos_rate);
    ppa_add_hook_map( "ifx_ppa_hook_reset_qos_rate",    (uint32_t ) &ifx_ppa_hook_reset_qos_rate,     (uint32_t ) ifx_ppa_hook_reset_qos_rate);

    if( ifx_ppa_drv_init_qos_rate(0) != IFX_SUCCESS )
        dbg("ifx_ppa_drv_init_qos_rate  fail\n");
#endif //end of CONFIG_IFX_PPA_QOS_RATE_SHAPING


#ifdef CONFIG_IFX_PPA_QOS_WFQ
    ifx_ppa_hook_set_ctrl_qos_wfq= ifx_ppa_set_ctrl_qos_wfq;
    ifx_ppa_hook_get_ctrl_qos_wfq = ifx_ppa_get_ctrl_qos_wfq;
    ifx_ppa_hook_set_qos_wfq = ifx_ppa_set_qos_wfq;
    ifx_ppa_hook_get_qos_wfq = ifx_ppa_get_qos_wfq;
    ifx_ppa_hook_reset_qos_wfq = ifx_ppa_reset_qos_wfq;

    ppa_add_hook_map( "ifx_ppa_hook_set_ctrl_qos_wfq", (uint32_t ) &ifx_ppa_hook_set_ctrl_qos_wfq, (uint32_t ) ifx_ppa_hook_set_ctrl_qos_wfq);
    ppa_add_hook_map( "ifx_ppa_hook_get_ctrl_qos_wfq", (uint32_t ) &ifx_ppa_hook_get_ctrl_qos_wfq, (uint32_t ) ifx_ppa_hook_get_ctrl_qos_wfq);
    ppa_add_hook_map( "ifx_ppa_hook_set_qos_wfq",      (uint32_t ) &ifx_ppa_hook_set_qos_wfq,       (uint32_t ) ifx_ppa_hook_set_qos_wfq);
    ppa_add_hook_map( "ifx_ppa_hook_get_qos_wfq",      (uint32_t ) &ifx_ppa_hook_get_qos_wfq,       (uint32_t ) ifx_ppa_hook_get_qos_wfq);
    ppa_add_hook_map( "ifx_ppa_hook_reset_qos_wfq",    (uint32_t ) &ifx_ppa_hook_reset_qos_wfq,     (uint32_t ) ifx_ppa_hook_reset_qos_wfq);

    if( ifx_ppa_drv_init_qos_wfq( 0) != IFX_SUCCESS )
        dbg("ifx_ppa_drv_init_qos_wfq  fail\n");
#endif //end of CONFIG_IFX_PPA_QOS_WFQ
#endif  //end of CONFIG_IFX_PPA_QOS

#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
    ifx_ppa_pwm_init();
#endif

#ifdef CONFIG_IFX_PPA_MFE
    ifx_ppa_hook_multifield_control = ifx_ppa_multifield_control;
    ifx_ppa_hook_get_multifield_status = ifx_ppa_get_multifield_status;
    ifx_ppa_hook_get_multifield_max_flow = ifx_ppa_get_multifield_max_flow;
    ifx_ppa_hook_add_multifield_flow = ifx_ppa_add_multifield_flow;
    ifx_ppa_hook_del_multifield_flow =ifx_ppa_del_multifield_flow;
    ifx_ppa_hook_quick_del_multifield_flow = ifx_ppa_quick_del_multifield_flow;
    if( ifx_ppa_multifield_control(0, 0) != IFX_SUCCESS ) //by default, disable it
        dbg("ifx_ppa_multifield_control  fail\n");

    ppa_add_hook_map( "ifx_ppa_hook_multifield_control", (uint32_t ) &ifx_ppa_hook_multifield_control, (uint32_t ) ifx_ppa_hook_multifield_control);
    ppa_add_hook_map("ifx_ppa_hook_get_multifield_status",  (uint32_t ) &ifx_ppa_hook_get_multifield_status,  (uint32_t ) ifx_ppa_hook_get_multifield_status);
    ppa_add_hook_map( "ifx_ppa_hook_get_multifield_max_flow", (uint32_t ) &ifx_ppa_hook_get_multifield_max_flow, (uint32_t ) ifx_ppa_hook_get_multifield_max_flow);
    ppa_add_hook_map( "ifx_ppa_hook_add_multifield_flow", (uint32_t ) &ifx_ppa_hook_add_multifield_flow, (uint32_t ) ifx_ppa_hook_add_multifield_flow);
    ppa_add_hook_map( "ifx_ppa_hook_del_multifield_flow", (uint32_t ) &ifx_ppa_hook_del_multifield_flow, (uint32_t ) ifx_ppa_hook_del_multifield_flow);
    ppa_add_hook_map( "ifx_ppa_hook_quick_del_multifield_flow", (uint32_t ) &ifx_ppa_hook_quick_del_multifield_flow,(uint32_t ) ifx_ppa_hook_quick_del_multifield_flow);

#endif //end of CONFIG_IFX_PPA_MFE

    ifx_ppa_hook_get_dsl_mib = ifx_ppa_get_dsl_mib;
    ifx_ppa_hook_get_port_mib = ifx_ppa_get_ports_mib;

    g_init = 1;
    return IFX_SUCCESS;

PPA_API_SESSION_MANAGER_INIT_FAIL:
    ppa_api_session_manager_exit();
PPA_API_NETIF_CREATE_INIT_FAIL:
    ppa_api_netif_manager_exit();
MAX_BRG_ENTRIES_ERROR:
MAX_MC_ENTRIES_ERROR:
MAX_SOURCE_ENTRIES_ERROR:
    err("failed in PPA init");
    return ret;
}

static void ppa_exit(void)
{
    PPE_BR_MAC_INFO br_mac={0};

    ppa_hook_enable_fn              = NULL;
    ppa_hook_get_status_fn          = NULL;

    ppa_hook_session_add_fn         = NULL;
    ppa_hook_session_del_fn         = NULL;
    ppa_hook_session_modify_fn      = NULL;
    ppa_hook_session_get_fn         = NULL;

    ppa_hook_mc_group_update_fn     = NULL;
    ppa_hook_mc_group_get_fn        = NULL;
    ppa_hook_mc_entry_modify_fn     = NULL;
    ppa_hook_mc_entry_get_fn        = NULL;
    ppa_hook_multicast_pkt_srcif_add_fn = NULL;

    ppa_hook_inactivity_status_fn   = NULL;
    ppa_hook_set_inactivity_fn      = NULL;

    ppa_hook_bridge_entry_add_fn               = NULL;
    ppa_hook_bridge_entry_delete_fn            = NULL;
    ppa_hook_bridge_entry_hit_time_fn          = NULL;
    ppa_hook_bridge_entry_inactivity_status_fn = NULL;
    ppa_hook_set_bridge_entry_timeout_fn       = NULL;

    ppa_hook_set_bridge_if_vlan_config_fn       = NULL;
    ppa_hook_get_bridge_if_vlan_config_fn       = NULL;
    ppa_hook_vlan_filter_add_fn                 = NULL;
    ppa_hook_vlan_filter_del_fn                 = NULL;
    ppa_hook_vlan_filter_get_all_fn             = NULL;
    ppa_hook_vlan_filter_del_all_fn             = NULL;

    ppa_hook_get_if_stats_fn        = NULL;
    ppa_hook_get_accel_stats_fn     = NULL;

    ppa_hook_set_if_mac_address_fn  = NULL;
    ppa_hook_get_if_mac_address_fn  = NULL;

    ppa_hook_add_if_fn              = NULL;
    ppa_hook_del_if_fn              = NULL;
    ppa_hook_get_if_fn              = NULL;

    ppa_hook_directpath_register_dev_fn = NULL;
    ppa_hook_directpath_send_fn         = NULL;
    ppa_hook_directpath_rx_stop_fn      = NULL;
    ppa_hook_directpath_rx_restart_fn   = NULL;
    ppa_hook_get_netif_for_ppa_ifid_fn  = NULL;
    ppa_hook_get_ifid_for_netif_fn      = NULL;

    ppa_hook_wan_mii0_vlan_range_add_fn = NULL;
    ppa_hook_wan_mii0_vlan_range_del_fn = NULL;
    ppa_hook_wan_mii0_vlan_ranges_get_fn= NULL;

#ifdef CONFIG_IFX_PPA_QOS
    ifx_ppa_hook_get_qos_qnum = NULL;
    ifx_ppa_hook_get_qos_mib = NULL;
#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
    ifx_ppa_hook_set_ctrl_qos_rate= NULL;
    ifx_ppa_hook_get_ctrl_qos_rate = NULL;
    ifx_ppa_hook_set_qos_rate = NULL;
    ifx_ppa_hook_get_qos_rate = NULL;
    ifx_ppa_hook_reset_qos_rate = NULL;
#endif //end of CONFIG_IFX_PPA_QOS_RATE_SHAPING

#ifdef CONFIG_IFX_PPA_QOS_WFQ
    ifx_ppa_hook_set_ctrl_qos_wfq= NULL;
    ifx_ppa_hook_get_ctrl_qos_wfq = NULL;
    ifx_ppa_hook_set_qos_wfq = NULL;
    ifx_ppa_hook_get_qos_wfq = NULL;
    ifx_ppa_hook_reset_qos_wfq = NULL;
#endif //end of CONFIG_IFX_PPA_QOS_WFQ
#endif  //end of CONFIG_IFX_PPA_QOS

#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
    ifx_ppa_pwm_exit();
#endif

#ifdef CONFIG_IFX_PPA_MFE
    ifx_ppa_hook_multifield_control = NULL;
    ifx_ppa_hook_get_multifield_status = NULL;
    ifx_ppa_hook_get_multifield_max_flow = NULL;
    ifx_ppa_hook_add_multifield_flow = NULL;
    ifx_ppa_hook_del_multifield_flow = NULL;
    ifx_ppa_hook_quick_del_multifield_flow = NULL;

    ifx_ppa_multifield_control(0, 0); //by default, disable it
    ifx_ppa_quick_del_multifield_flow(-1, 0);
#endif //end of CONFIG_IFX_PPA_MFE

    ifx_ppa_hook_get_dsl_mib = NULL;
    ifx_ppa_hook_get_port_mib = NULL;

    ifx_ppa_vlan_filter_del_all(0); //sgh add,

    br_mac.p_entry = g_broadcast_bridging_entry;
    ifx_ppa_drv_del_bridging_entry(&br_mac, 0);

    ppa_api_session_manager_exit();

    ppa_api_netif_manager_exit();

    ppa_hook_list_destroy();

    g_init = 0;
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*
 *  Function for internal use
 */

int32_t ifx_ppa_is_init(void)
{
    return g_init ? 1 : 0;
}

/*
 *  PPA Initialization Functions
 */

void ifx_ppa_subsystem_id(uint32_t *p_family,
                          uint32_t *p_type,
                          uint32_t *p_if,
                          uint32_t *p_mode,
                          uint32_t *p_major,
                          uint32_t *p_mid,
                          uint32_t *p_minor)
{
    if ( p_family )
        *p_family = 0;

    if ( p_type )
        *p_type = 0;

    if ( p_if )
        *p_if = 0;

    if ( p_mode )
        *p_mode = 0;

    if ( p_major )
        *p_major = PPA_SUBSYSTEM_MAJOR;

    if ( p_mid )
        *p_mid = PPA_SUBSYSTEM_MID;

    if ( p_minor )
        *p_minor = PPA_SUBSYSTEM_MINOR;
}

void ifx_ppa_get_api_id(uint32_t *p_family,
                        uint32_t *p_type,
                        uint32_t *p_if,
                        uint32_t *p_mode,
                        uint32_t *p_major,
                        uint32_t *p_mid,
                        uint32_t *p_minor)
{
    if ( p_family )
        *p_family = VER_FAMILY;

    if ( p_type )
        *p_type = VER_DRTYPE;

    if ( p_if )
        *p_if = VER_INTERFACE;

    if ( p_mode )
        *p_mode = VER_ACCMODE;

    if ( p_major )
        *p_major = VER_MAJOR;

    if ( p_mid )
        *p_mid = VER_MID;

    if ( p_minor )
        *p_minor = VER_MINOR;
}

int32_t ifx_ppa_init(PPA_INIT_INFO *p_info, uint32_t flags)
{
    int32_t ret;

    if ( !p_info )
        return IFX_EINVAL;

    if ( g_init )
        ppa_exit();

    if ( (ret = ppa_init(p_info, flags)) == IFX_SUCCESS )
        printk("ifx_ppa_init - init succeeded\n");
    else
        printk("ifx_ppa_init - init failed (%d)\n", ret);

    return ret;
}

void ifx_ppa_exit(void)
{
    if ( g_init )
        ppa_exit();
}

/*
 *  PPA Enable/Disable and Status Functions
 */

int32_t ifx_ppa_enable(uint32_t lan_rx_ppa_enable, uint32_t wan_rx_ppa_enable, uint32_t flags)
{
    u32 sys_flag;
    PPE_ACC_ENABLE acc_cfg;

    if ( g_init )
    {
        lan_rx_ppa_enable = lan_rx_ppa_enable ? IFX_PPA_ACC_MODE_ROUTING : IFX_PPA_ACC_MODE_NONE;
        wan_rx_ppa_enable = wan_rx_ppa_enable ? IFX_PPA_ACC_MODE_ROUTING : IFX_PPA_ACC_MODE_NONE;
        sys_flag = ppa_disable_int();
        acc_cfg.f_is_lan =1;
        acc_cfg.f_enable = lan_rx_ppa_enable;
        ifx_ppa_drv_set_acc_mode(&acc_cfg, 0);

        acc_cfg.f_is_lan =0;
        acc_cfg.f_enable = wan_rx_ppa_enable;
        ifx_ppa_drv_set_acc_mode(&acc_cfg, 0);
        ppa_enable_int(sys_flag);
        return IFX_SUCCESS;
    }
    return IFX_FAILURE;
}

int32_t ifx_ppa_get_status(uint32_t *lan_rx_ppa_enable, uint32_t *wan_rx_ppa_enable, uint32_t flags)
{
    if ( g_init )
    {
        PPE_ACC_ENABLE cfg;

        cfg.f_is_lan = 1;
        ifx_ppa_drv_get_acc_mode(&cfg, 0);
		if( lan_rx_ppa_enable ) *lan_rx_ppa_enable = cfg.f_enable;

        cfg.f_is_lan = 0;
        ifx_ppa_drv_get_acc_mode(&cfg, 0);
		if( wan_rx_ppa_enable ) *wan_rx_ppa_enable = cfg.f_enable;
        return IFX_SUCCESS;
    }
    return IFX_FAILURE;
}

/*
 *  PPA Routing Session Operation Functions
 */

int32_t ifx_ppa_session_add(PPA_BUF *ppa_buf, PPA_SESSION *p_session, uint32_t flags)
{
    int32_t ret;
    struct session_list_item *p_item;

#define PPA_SPEEDUP_TEST
#define PPA_TCP_DELAY_PKTS          20
#define PPA_UDP_DELAY_PKTS          20
//#define PPA_ADDING_PER_PKTS         2
    int32_t ppa_lookup_session_failed=0;

#ifdef PPA_SPEEDUP_TEST
    /* Speed up software path. In case session exists and certain flags are set
       short-circuit the loop */
     //get session
    if ( !p_session )
    {
        p_session = ppa_get_session(ppa_buf);
        if ( !p_session )
        {
            dbg("no conntrack:%s", flags & PPA_F_BEFORE_NAT_TRANSFORM?"Before NAT":"After NAT");
            return IFX_PPA_SESSION_NOT_ADDED;
        }
    }
    if ( ppa_lookup_session(p_session, flags & PPA_F_SESSION_REPLY_DIR, &p_item) == IFX_PPA_SESSION_EXISTS )
    {
        dbg("session exists, p_item = 0x%08x", (u32)p_item);
        p_item->num_adds++;             //  increment number of add hits for this session
        if (p_item->flags & (SESSION_ADDED_IN_HW | SESSION_CAN_NOT_ACCEL ) )
        {
            /*
             * Session exists, but this packet will take s/w path nonetheless.
             * Can happen for individual pkts of a session, or for complete sessions!
             * For eg., if WAN upstream acceleration is disabled.
             */
            if ( (flags & PPA_F_BEFORE_NAT_TRANSFORM) )
            {
                p_item->mips_bytes += ppa_buf->len + PPA_ETH_HLEN + PPA_ETH_CRCLEN;
                //ygchen, already increased before
                //p_item->num_adds++;             //  increment number of add hits for this session
            }

            return IFX_PPA_SESSION_ADDED;
        }
    }
    else{
        ppa_lookup_session_failed=1;
    }
#endif

    //  ignore packets output by the device
    if ( ppa_is_pkt_host_output(ppa_buf) )
    {
        dbg("ppa_is_pkt_host_output");
        return IFX_PPA_SESSION_NOT_ADDED;
    }

    //  ignore incoming broadcast
    if ( ppa_is_pkt_broadcast(ppa_buf) )
    {
        dbg("ppa_is_pkt_broadcast");
        return IFX_PPA_SESSION_NOT_ADDED;
    }

    //  ignore multicast packet in unitcast routing but learn multicast source interface automatically
    if (  ppa_is_pkt_multicast(ppa_buf) )
    {
        dbg( "ppa_is_pkt_multicast");

        /*auto learn multicast source interface*/
        if ( (flags & PPA_F_BEFORE_NAT_TRANSFORM ) && ppa_hook_multicast_pkt_srcif_add_fn )
            ppa_hook_multicast_pkt_srcif_add_fn(ppa_buf, NULL);

        return IFX_PPA_SESSION_NOT_ADDED;
    }

    //  ignore loopback packet
    if ( ppa_is_pkt_loopback(ppa_buf) )
    {
        dbg("ppa_is_pkt_loopback");
        return IFX_PPA_SESSION_NOT_ADDED;
    }

    //  ignore protocols other than TCP/UDP, since some of them (e.g. ICMP) can't be handled safe in this arch
    if ( ppa_get_pkt_ip_proto(ppa_buf) != IFX_IPPROTO_UDP && ppa_get_pkt_ip_proto(ppa_buf) != IFX_IPPROTO_TCP )
    {
        dbg("protocol: %d", (uint32_t)ppa_get_pkt_ip_proto(ppa_buf));
        return IFX_PPA_SESSION_NOT_ADDED;
    }

    //  ignore special broadcast type (windows netbios)
    if ( ppa_get_pkt_src_port(ppa_buf) == 137 || ppa_get_pkt_dst_port(ppa_buf) == 137
        || ppa_get_pkt_src_port(ppa_buf) == 138 || ppa_get_pkt_dst_port(ppa_buf) == 138 )
    {
        dbg("src port = %d, dst port = %d", ppa_get_pkt_src_port(ppa_buf), ppa_get_pkt_dst_port(ppa_buf));
        return IFX_PPA_SESSION_NOT_ADDED;
    }

    //  ignore fragment packet
    if ( ppa_is_pkt_fragment(ppa_buf) )
    {
        dbg("fragment");
        return IFX_PPA_SESSION_NOT_ADDED;
    }

#ifndef PPA_SPEEDUP_TEST
    //  get session
    if ( !p_session )
    {
        p_session = ppa_get_session(ppa_buf);
        if ( !p_session )
        {
            dbg("why no session ???\n");
            return IFX_PPA_SESSION_NOT_ADDED;
        }
    }
#endif
    dbg("%s", flags & PPA_F_BEFORE_NAT_TRANSFORM?"Before NAT":"After NAT");
    if ( (flags & PPA_F_BEFORE_NAT_TRANSFORM) )
    {
#ifdef PPA_SPEEDUP_TEST    //ygchen, already checked before
        if ( ppa_lookup_session_failed == 0 )
#else
        if ( ppa_lookup_session(p_session, flags & PPA_F_SESSION_REPLY_DIR, &p_item) == IFX_PPA_SESSION_EXISTS )
#endif
        {
            dbg("session exists, p_item = 0x%08x", (u32)p_item);
            p_item->mips_bytes += ppa_buf->len + PPA_ETH_HLEN + PPA_ETH_CRCLEN;
#ifndef PPA_SPEEDUP_TEST    //ygchen, already increased before
            p_item->num_adds++;             //  increment number of add hits for this session
#endif
            return IFX_PPA_SESSION_ADDED;
        }
        else if ( ppa_add_session(ppa_buf, p_session, &p_item, flags) != IFX_SUCCESS )
        {
            dbg("ppa_add_session failed");
            return IFX_PPA_SESSION_NOT_ADDED;
        }
        else{
            p_item->num_adds++;             //  increment number of add hits for this session
            p_item->mips_bytes += ppa_buf->len + PPA_ETH_HLEN + PPA_ETH_CRCLEN;
        }
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
        p_item->priority = ppa_get_pkt_priority(ppa_buf);
#endif
        dump_list_item(p_item, "after ppa_add_session");

        return IFX_PPA_SESSION_ADDED;
    }
    else
    {
        //  in case compiler optimization problem
        PPA_SYNC();

        //  handle routed packet only
        if ( !ppa_is_pkt_routing(ppa_buf) )
        {
            dbg("not routing packet");
            return IFX_PPA_SESSION_NOT_ADDED;
        }
#ifdef PPA_SPEEDUP_TEST    //ygchen, already checked before
        if ( ppa_lookup_session_failed == 1 )
#else
        if ( ppa_lookup_session(p_session, flags & PPA_F_SESSION_REPLY_DIR, &p_item) != IFX_PPA_SESSION_EXISTS )
#endif
        {
            int8_t strbuf[64];
            dbg("post routing point but no session exist: dst_ip = %s",
                ppa_get_pkt_ip_string(ppa_get_pkt_dst_ip(ppa_buf), ppa_is_pkt_ipv6(ppa_buf), strbuf));
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        //  not enough hit
        if ( p_item->num_adds < g_min_hits )
        {
            dbg("p_item->num_adds (%d) < g_min_hits (%d)", p_item->num_adds, g_min_hits);
            SET_DBG_FLAG(p_item, SESSION_DBG_NOT_REACH_MIN_HITS);
            return IFX_PPA_SESSION_NOT_ADDED;
        }
        else
        {
            CLR_DBG_FLAG(p_item, SESSION_DBG_NOT_REACH_MIN_HITS);
        }

        //  can not be accelerated
        if ( (p_item->flags & SESSION_CAN_NOT_ACCEL) )
        {
            dbg("can not accelerate (pretested)");
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        //  added in hardware
        if ( (p_item->flags & SESSION_ADDED_IN_HW) )
        {
            dbg("session added in firmware table already");
            return IFX_PPA_SESSION_EXISTS;
        }
/*
        // ygchen, to reduce CPU overhead especially when HW PPA table is full 
        if ( (p_item->num_adds > PPA_UDP_DELAY_PKTS) && (p_item->num_adds&(PPA_ADDING_PER_PKTS-1) != (PPA_ADDING_PER_PKTS-1) ) )
        {
            dbg("skip this ppa adding, p_item->num_adds=%d", p_item->num_adds);
            return IFX_PPA_SESSION_NOT_ADDED;
        }
*/
        //  check if session needs to be handled in MIPS for conntrack handling (e.g. ALG)
        if ( ppa_check_is_special_session(ppa_buf, p_session) )
        {
            dbg("can not accelerate: %08X", (uint32_t)p_session);
            p_item->flags |= SESSION_CAN_NOT_ACCEL;
            SET_DBG_FLAG(p_item, SESSION_DBG_ALG);
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        //  for TCP, check whether connection is established
        if ( ppa_get_pkt_ip_proto(ppa_buf) == IFX_IPPROTO_TCP )
        {
            if ( !ppa_is_tcp_established(p_item->session) )
            {
                dbg("tcp not established: %08X", (uint32_t)p_session);
                SET_DBG_FLAG(p_item, SESSION_DBG_TCP_NOT_ESTABLISHED);
                return IFX_PPA_SESSION_NOT_ADDED;
            }
            //ygchen
            else if ( p_item->num_adds < PPA_TCP_DELAY_PKTS ) {
                dbg("tcp delay ppa_session_add, num_adds=%d", p_item->num_adds);
                SET_DBG_FLAG(p_item, SESSION_DBG_TCP_NOT_ESTABLISHED);
                return IFX_PPA_SESSION_NOT_ADDED;
            }                
            else
            {
                p_item->flags |= SESSION_IS_TCP;
                CLR_DBG_FLAG(p_item, SESSION_DBG_TCP_NOT_ESTABLISHED);
            }
        }
        //ygchen, to make sure most of UDP sessions in PPA are active sessions
        else{
            if ( p_item->num_adds < PPA_UDP_DELAY_PKTS ) {
                dbg("udp delay ppa_session_add, num_adds=%d", p_item->num_adds);
                return IFX_PPA_SESSION_NOT_ADDED;
            }
        }            
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
        p_item->priority = ppa_get_pkt_priority(ppa_buf);
#endif

        //  get information needed by hardware/firmware
        if ( (ret = ppa_update_session(ppa_buf, p_item, flags)) != IFX_SUCCESS )
        {
            dbg("update session fail");
            if ( ret != IFX_EAGAIN )
                p_item->flags |= SESSION_CAN_NOT_ACCEL;
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        dump_list_item(p_item, "after ppa_update_session");

        //  check whether this session is aligned with restriction of firmware
        //  e.g. VLAN can not be supported by physical interface
        if ( (p_item->flags & SESSION_CAN_NOT_ACCEL) )
        {
            dbg("session not aligned with restriction of firmware, e.g. update netif fail, too many VLAN applied");
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        //  protect before ARP
        if ( !(p_item->flags & SESSION_TX_ITF_IPOA_PPPOA_MASK)
            && p_item->dst_mac[0] == 0
            && p_item->dst_mac[1] == 0
            && p_item->dst_mac[2] == 0
            && p_item->dst_mac[3] == 0
            && p_item->dst_mac[4] == 0
            && p_item->dst_mac[5] == 0 )
        {
            SET_DBG_FLAG(p_item, SESSION_DBG_ZERO_DST_MAC);
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        if ( (ret = ppa_hw_add_session(p_item, 0)) != IFX_SUCCESS )
        {
            if ( ret != IFX_EAGAIN )
                p_item->flags |= SESSION_CAN_NOT_ACCEL;
            dbg("ppa_hw_add_session(p_item) fail");
            return IFX_PPA_SESSION_NOT_ADDED;
        }
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
        ifx_ppa_pwm_activate_module();
#endif

        dbg("hardware/firmware entry added");

        return IFX_PPA_SESSION_ADDED;
    }
}

int32_t ifx_ppa_session_delete(PPA_SESSION *p_session, uint32_t flags)
{
    int32_t ret = IFX_FAILURE;
    uint32_t flag_template[2] = {PPA_F_SESSION_ORG_DIR, PPA_F_SESSION_REPLY_DIR};
    struct session_list_item *p_item;
    uint32_t i;

    for ( i = 0; i < NUM_ENTITY(flag_template); i++ )
    {
        if ( !(flags & flag_template[i]) )
            continue;

        //  i = 0, org dir, i = 1, reply dir
        if ( ppa_lookup_session(p_session, i, &p_item) != IFX_PPA_SESSION_EXISTS )
            continue;

        dump_list_item(p_item, "ifx_ppa_session_delete");

        ppa_hw_del_session(p_item);

        ppa_remove_session(p_item);

        ret = IFX_SUCCESS;
    }

#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
    ifx_ppa_pwm_deactivate_module();
#endif

    return ret;
}

int32_t ifx_ppa_session_modify(PPA_SESSION *p_session, PPA_SESSION_EXTRA *p_extra, uint32_t flags)
{
    struct session_list_item *p_item;

    if ( ppa_lookup_session(p_session, flags & PPA_F_SESSION_REPLY_DIR, &p_item) == IFX_PPA_SESSION_EXISTS )
    {
        ppa_update_session_extra(p_extra, p_item, flags);
        if ( (p_item->flags & SESSION_ADDED_IN_HW) && (flags != 0)  )
        {
            if( !(p_item->flags & SESSION_CAN_NOT_ACCEL) )
            {
                if ( ppa_hw_update_session_extra(p_item, flags) != IFX_SUCCESS )
                {
                    //  update failed
                    ppa_hw_del_session(p_item);
                    p_item->flags &= ~SESSION_ADDED_IN_HW;
                    return IFX_FAILURE;
                }
            }
            else //just remove the accelerated session from PPE FW, no need to update other flags since PPA hook will rewrite them.
            {
                ppa_hw_del_session(p_item);
                p_item->flags &= ~SESSION_ADDED_IN_HW;
            }
        }
        return IFX_SUCCESS;
    }
    else
        return IFX_FAILURE;
}

int32_t ifx_ppa_session_get(PPA_SESSION ***pp_sessions, PPA_SESSION_EXTRA **pp_extra, int32_t *p_num_entries, uint32_t flags)
{
//#warning ifx_ppa_session_get is not implemented
    return IFX_ENOTIMPL;
}

/*
 *  PPA Multicast Routing Session Operation Functions
 */

int32_t ifx_ppa_mc_group_update(PPA_MC_GROUP *ppa_mc_entry, uint32_t flags)
{
    struct mc_group_list_item *p_item;
    PPE_MC_INFO mc={0};

    { //for print debug information only
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ifx_ppa_mc_group_update for group: %d.%d.%d.%d ", ppa_mc_entry->ip_mc_group.ip.ip >> 24, (ppa_mc_entry->ip_mc_group.ip.ip >> 16) & 0xFF, (ppa_mc_entry->ip_mc_group.ip.ip >> 8) & 0xFF, ppa_mc_entry->ip_mc_group.ip.ip & 0xFF);
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "from %s ", ppa_mc_entry->src_ifname ? ppa_mc_entry->src_ifname: "NULL" );

        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "to ");
        if( ppa_mc_entry->num_ifs ==0 ) ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "NULL" );
        else
        {
            int i, bit;
            for(i=0, bit=1; i<ppa_mc_entry->num_ifs; i++ )
            {
                if ( ppa_mc_entry->if_mask & bit )
                    ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "%s ", ppa_mc_entry->array_mem_ifs[i].ifname? ppa_mc_entry->array_mem_ifs[i].ifname:"NULL");

                bit = bit<<1;
            }
        }
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "with ssm=%d\n",  ppa_mc_entry->SSM_flag );
    }
    if ( ppa_mc_entry->ip_mc_group.f_ipv6 > 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ifx_ppa_mc_group_update not support non-ipv4 multicast yet:%d\n", ppa_mc_entry->ip_mc_group.f_ipv6);
        return IFX_FAILURE;
    }
    if( ppa_mc_entry->ip_mc_group.ip.ip == 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ifx_ppa_mc_group_update not support zero ip address\n");
        return IFX_FAILURE;
    }
    if ( ppa_mc_entry->SSM_flag == 1 )  /*at present, PPE FW does not support source-ip based multicast acceleration. so we have to delete this session now */
    {
        /*temporary we force num_ifs to zero to force to delete this acceleraton session */
        ppa_mc_entry->num_ifs = 0;
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "SSM_flag is set\n" );
    }

    if ( ppa_lookup_mc_group(ppa_mc_entry->ip_mc_group, &p_item) != IFX_PPA_SESSION_EXISTS )
    {
        if( ppa_mc_entry->num_ifs == 0 )
                return IFX_SUCCESS;

        //  add new mc groups
        if ( ppa_add_mc_group(ppa_mc_entry, &p_item, flags) != IFX_SUCCESS )
        {
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_add_mc_group fail\n");
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        if ( p_item->src_netif == NULL )    // only added in PPA level, not PPE FW level since source interface not get yet.
        {
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "IGMP request no src_netif. No acceleration !\n");
            return IFX_PPA_SESSION_ADDED;
        }
        if ( ppa_hw_add_mc_group(p_item) != IFX_SUCCESS )
        {
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): fail", ppa_mc_entry->ip_mc_group.ip.ip >> 24, (ppa_mc_entry->ip_mc_group.ip.ip >> 16) & 0xFF, (ppa_mc_entry->ip_mc_group.ip.ip >> 8) & 0xFF, ppa_mc_entry->ip_mc_group.ip.ip & 0xFF);
            ppa_remove_mc_group(p_item);
            return IFX_PPA_SESSION_NOT_ADDED;
        }
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
        ifx_ppa_pwm_activate_module();
#endif
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "hardware added");

        return IFX_PPA_SESSION_ADDED;
    }
    else
    {
        //  update
        uint32_t bfNeedChange, change_out_vlan_entry=0;
        struct mc_group_list_item tmp_mc_item = {0};
        ppa_init_mc_group_list_item ( &tmp_mc_item );
        tmp_mc_item.flags = p_item->flags ; //based on original flag, we try to calculate the new flag
        tmp_mc_item.new_dscp = p_item->new_dscp;

        if( ppa_mc_group_checking(ppa_mc_entry, &tmp_mc_item, flags ) !=IFX_SUCCESS )
        {    //sanity check ffailure
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "old_new flag:%x_%xl\n",p_item->flags, tmp_mc_item.flags );
            if(  p_item->flags & SESSION_ADDED_IN_HW )
            { //remove from hw
                mc_updating_flag = 1;
                ppa_hw_del_mc_group(p_item);
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
                ifx_ppa_pwm_deactivate_module();
#endif
            }
            ppa_remove_mc_group(p_item);
            if( mc_updating_flag ) mc_updating_flag = 0;

            if( ppa_mc_entry->num_ifs == 0 )
            {

                return IFX_SUCCESS;
            }
            else
            {
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_mc_group_checking fail\n");
                return IFX_FAILURE;
            }
        }

        //  if no interface, delete
        ASSERT(p_item->num_ifs == 0, "Why run to here with num_ifs zero");

        if( p_item->src_netif == NULL && tmp_mc_item.src_netif  == NULL )
        {  //before leave update p_item
            ppa_mc_group_checking(ppa_mc_entry, p_item, flags );
            return IFX_SUCCESS;
        }


        //check whether any down stream interface changes
        bfNeedChange = 0;
        if(  tmp_mc_item.flags != p_item->flags )
            bfNeedChange = 1;
        else if( tmp_mc_item.dest_ifid != p_item->dest_ifid )
             bfNeedChange = 1;
        else if ( (p_item->flags & SESSION_VALID_VLAN_INS ) && (p_item->new_vci != tmp_mc_item.new_vci ) )
            bfNeedChange = 1;
        else if ( (p_item->flags & SESSION_VALID_OUT_VLAN_INS) && (p_item->out_vlan_tag != tmp_mc_item.out_vlan_tag) )
        {
            bfNeedChange = 1;
            change_out_vlan_entry = 1;
        }
        else if ( (p_item->flags & SESSION_VALID_SRC_MAC) && (p_item->src_mac_entry != tmp_mc_item.src_mac_entry) )
            bfNeedChange = 1;
        else if ( (p_item->flags & SESSION_VALID_NEW_DSCP) && (p_item->new_dscp != tmp_mc_item.new_dscp) )
            bfNeedChange = 1;
        else if( p_item->dslwan_qid != p_item->dslwan_qid )
            bfNeedChange = 1;

        if( p_item->src_netif != NULL && tmp_mc_item.src_netif  == NULL )
        {   //still no need to change
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "why no valid src_if now\n");
            tmp_mc_item.src_netif  = p_item->src_netif; //use old src_netif although there is no src_if in tmp_mc_item.
        }
        else if( p_item->src_netif == NULL && tmp_mc_item.src_netif  != NULL )
        {  //need to change in this case for
            bfNeedChange = 1;
            if( p_item->flags & SESSION_ADDED_IN_HW)
            { //wrong
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "Wrong here: last call no src_if but already added into HW ???\n");;
                mc_updating_flag = 1;
                ppa_hw_del_mc_group(p_item);
                ppa_remove_mc_group(p_item);
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
                ifx_ppa_pwm_deactivate_module();
#endif
                mc_updating_flag = 0;
                return IFX_FAILURE;
            }
        }
        else if ( p_item->src_netif  != tmp_mc_item.src_netif )
        {
            bfNeedChange = 1;
        }

        if ( !bfNeedChange )
        {
            ppa_mc_group_checking(ppa_mc_entry, p_item, flags );
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "no change \n");
            return IFX_SUCCESS;
        }

        ppa_mc_group_checking(ppa_mc_entry, p_item, flags );
        if( !(p_item->flags & SESSION_ADDED_IN_HW) )
        {
            if ( ppa_hw_add_mc_group(p_item) != IFX_SUCCESS )
            {
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group2(%d.%d.%d.%d): fail", ppa_mc_entry->ip_mc_group.ip.ip >> 24, (ppa_mc_entry->ip_mc_group.ip.ip >> 16) & 0xFF, (ppa_mc_entry->ip_mc_group.ip.ip >> 8) & 0xFF, ppa_mc_entry->ip_mc_group.ip.ip & 0xFF);
                ppa_remove_mc_group(p_item);
                return IFX_PPA_SESSION_NOT_ADDED;
            }
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
            ifx_ppa_pwm_activate_module();
#endif
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "hardware added 2");
        }
        else
        {
            mc.p_entry = p_item->mc_entry;
            mc.f_vlan_ins_enable = p_item->flags & SESSION_VALID_VLAN_INS;
            mc.new_vci = p_item->new_vci;
            mc.f_vlan_rm_enable = p_item->flags & SESSION_VALID_VLAN_RM;
            mc.f_src_mac_enable = p_item->flags & SESSION_VALID_SRC_MAC;
            mc.src_mac_ix = p_item->src_mac_entry;
            mc.pppoe_mode = p_item->flags & SESSION_VALID_PPPOE;
            mc.f_out_vlan_ins_enable = p_item->flags & SESSION_VALID_OUT_VLAN_INS;
            if ( mc.f_out_vlan_ins_enable && change_out_vlan_entry )
            {
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "Need update out_vlan_entry from %d ", p_item->out_vlan_entry);
                mc.out_vlan_info.vlan_entry = p_item->out_vlan_entry; //get old out_vlan_entry and delete it
                ifx_ppa_drv_del_outer_vlan_entry(&mc.out_vlan_info, 0);
                p_item->out_vlan_entry = ~0;
                //  create new OUT VLAN entry
                mc.out_vlan_info.vlan_id = p_item->out_vlan_tag;
                if ( ifx_ppa_drv_add_outer_vlan_entry( &mc.out_vlan_info, 0) != IFX_SUCCESS )
                {
                   ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "Need update out_vlan_entry from %d ", p_item->out_vlan_entry);
                    mc_updating_flag = 1;
                    ppa_hw_del_mc_group(p_item);
                    ppa_remove_mc_group(p_item);
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
                   ifx_ppa_pwm_deactivate_module();
#endif
                    mc_updating_flag = 0;
                    return IFX_FAILURE;
                }
                p_item->out_vlan_entry = mc.out_vlan_info.vlan_entry; //save the new entry to
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "to %d \n", p_item->out_vlan_entry );
            }
            else mc.out_vlan_info.vlan_entry = p_item->out_vlan_entry;

            mc.f_out_vlan_rm_enable = p_item->flags & SESSION_VALID_OUT_VLAN_RM;
            mc.f_new_dscp_enable= p_item->flags & SESSION_VALID_NEW_DSCP;
            mc.new_dscp = p_item->new_dscp;
            mc.dest_qid = p_item->dslwan_qid;
            mc.dest_list = p_item->dest_ifid;
            mc.update_flags= IFX_PPA_UPDATE_WAN_MC_ENTRY_DEST_LIST;

            if ( ifx_ppa_drv_update_wan_mc_entry(&mc, 0) != IFX_SUCCESS )
            {
                //  update failed
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "update_wan_mc_entry failed for group: %d.%d.%d.%d", ppa_mc_entry->ip_mc_group.ip.ip >> 24, (ppa_mc_entry->ip_mc_group.ip.ip >> 16) & 0xFF, (ppa_mc_entry->ip_mc_group.ip.ip >> 8) & 0xFF, ppa_mc_entry->ip_mc_group.ip.ip & 0xFF);
                mc_updating_flag = 1;
                ppa_hw_del_mc_group(p_item);
                ppa_remove_mc_group(p_item);
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
                ifx_ppa_pwm_deactivate_module();
#endif
                mc_updating_flag = 0;
                return IFX_FAILURE;
            }
        }
    }

    return IFX_SUCCESS;
}

int32_t ifx_ppa_mc_group_get(IP_ADDR_C  ip_mc_group, PPA_MC_GROUP *ppa_mc_entry, uint32_t flags)
{
    struct mc_group_list_item *p_item;
    struct mc_group_list_item item;
    PPA_IFNAME *ifname;
    uint32_t idx;
    uint32_t bit;
    uint32_t i;

    if ( ppa_lookup_mc_group(ip_mc_group, &p_item) != IFX_PPA_SESSION_EXISTS )
        return IFX_ENOTAVAIL;

    item = *p_item;

    ppa_memcpy( &ppa_mc_entry->ip_mc_group, &item.ip_mc_group, sizeof( ppa_mc_entry->ip_mc_group  ) );


    for ( i = 0, bit = 1, idx = 0; i < PPA_MAX_MC_IFS_NUM; i++, bit <<= 1 )
        if ( (item.if_mask & bit) )
        {
            ifname = ppa_get_netif_name(item.netif[i]);
            if ( ifname )
            {
                ppa_mc_entry->array_mem_ifs[idx].ifname = ifname;
                ppa_mc_entry->array_mem_ifs[idx].ttl    = item.ttl[i];
                ppa_mc_entry->if_mask |= (1 << idx);
                idx++;
            }
        }

    ppa_mc_entry->num_ifs = idx;
    //ppa_mc_entry->if_mask = (1 << idx) - 1;

    return IFX_SUCCESS;
}

int32_t ifx_ppa_mc_entry_modify(IP_ADDR_C ip_mc_group, PPA_MC_GROUP *ppa_mc_entry, PPA_SESSION_EXTRA *p_extra, uint32_t flags)
{
    struct mc_group_list_item *p_item;

    if ( ppa_lookup_mc_group(ip_mc_group, &p_item) == IFX_PPA_SESSION_EXISTS )
    {
        ppa_update_mc_group_extra(p_extra, p_item, flags);
        if ( (p_item->flags & SESSION_ADDED_IN_HW) )
        {
            if ( ppa_hw_update_mc_group_extra(p_item, flags) != IFX_SUCCESS )
            {
                //  update failed
                mc_updating_flag = 1;
                ppa_hw_del_mc_group(p_item);
                ppa_remove_mc_group(p_item);
                mc_updating_flag = 0;
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
                ifx_ppa_pwm_deactivate_module();
#endif
                return IFX_FAILURE;
            }
        }
        return IFX_SUCCESS;
    }
    else
        return IFX_FAILURE;
}

int32_t ifx_ppa_mc_entry_get(IP_ADDR_C ip_mc_group, PPA_SESSION_EXTRA *p_extra, uint32_t flags)
{
    struct mc_group_list_item *p_item;

    if ( !p_extra )
        return IFX_EINVAL;

    if ( ppa_lookup_mc_group(ip_mc_group, &p_item) == IFX_PPA_SESSION_EXISTS )
    {
        ppa_memset(p_extra, 0, sizeof(*p_extra));

        p_extra->session_flags = flags;

        if ( (flags & PPA_F_SESSION_NEW_DSCP) )
        {
            if ( (p_item->flags & SESSION_VALID_NEW_DSCP) )
            {
                p_extra->dscp_remark = 1;
                p_extra->new_dscp = p_item->new_dscp;
            }
        }

        if ( (flags & PPA_F_SESSION_VLAN) )
        {
            if ( (p_item->flags & SESSION_VALID_VLAN_INS) )
            {
                p_extra->vlan_insert = 1;
                p_extra->vlan_prio   = p_item->new_vci >> 13;
                p_extra->vlan_cfi    = (p_item->new_vci >> 12) & 0x01;
                p_extra->vlan_id     = p_item->new_vci & ((1 << 12) - 1);
            }

            if ( (p_item->flags & SESSION_VALID_VLAN_RM) )
                p_extra->vlan_remove = 1;
        }

        if ( (flags & PPA_F_SESSION_OUT_VLAN) )
        {
            if ( (p_item->flags & SESSION_VALID_OUT_VLAN_INS) )
            {
                p_extra->out_vlan_insert = 1;
                p_extra->out_vlan_tag    = p_item->out_vlan_tag;
            }

            if ( (p_item->flags & SESSION_VALID_OUT_VLAN_RM) )
                p_extra->out_vlan_remove = 1;
        }

         p_extra->dslwan_qid_remark = 1;
         p_extra->dslwan_qid        = p_item->dslwan_qid;
         p_extra->out_vlan_tag      = p_item->out_vlan_tag;

        return IFX_SUCCESS;
    }
    else
        return IFX_FAILURE;
}

/*
 *  PPA Unicast Session Timeout Functions
 */

int32_t ifx_ppa_inactivity_status(PPA_U_SESSION *p_session)
{
    int f_flag;
    int f_timeout;
    int32_t ret, ret_reply;
    struct session_list_item *p_item, *p_item_reply;

    ret = ppa_lookup_session((PPA_SESSION *)p_session, 0, &p_item);
    ret_reply = ppa_lookup_session((PPA_SESSION *)p_session, 1, &p_item_reply);
    if ( ret != IFX_PPA_SESSION_EXISTS && ret_reply != IFX_PPA_SESSION_EXISTS )
        return IFX_PPA_SESSION_NOT_ADDED;

    if ( ((ret == IFX_PPA_SESSION_EXISTS && p_item->ip_proto == IFX_IPPROTO_TCP) || (ret_reply == IFX_PPA_SESSION_EXISTS && p_item_reply->ip_proto == IFX_IPPROTO_TCP))
        && !ppa_is_tcp_established(p_session) )
        return IFX_PPA_TIMEOUT;

    f_flag = 0;
    f_timeout = 1;

    if ( ret == IFX_PPA_SESSION_EXISTS && (p_item->flags & SESSION_ADDED_IN_HW) )
    {
        uint32_t tmp;

        f_flag = 1;

        tmp = ppa_get_time_in_sec();
        if ( p_item->timeout >= tmp - p_item->last_hit_time)
        {
            f_timeout = 0;
            dbg("session %08x, p_item->timeout (%d) + p_item->last_hit_time (%d) > ppa_get_time_in_sec (%d)", (unsigned int)p_session, p_item->timeout, p_item->last_hit_time, tmp);
        }
        else
        {
            dbg("session %08x, p_item->timeout (%d) + p_item->last_hit_time (%d) <= ppa_get_time_in_sec (%d)", (unsigned int)p_session, p_item->timeout, p_item->last_hit_time, tmp);
        }
    }

    if ( ret_reply == IFX_PPA_SESSION_EXISTS && (p_item_reply->flags & SESSION_ADDED_IN_HW) )
    {
        uint32_t tmp;

        f_flag = 1;

        tmp = ppa_get_time_in_sec();
        if ( p_item_reply->timeout >= tmp - p_item_reply->last_hit_time)
        {
            f_timeout = 0;
            dbg("session %08x, p_item_reply->timeout (%d) + p_item_reply->last_hit_time (%d) > ppa_get_time_in_sec (%d)", (unsigned int)p_session, p_item_reply->timeout, p_item_reply->last_hit_time, tmp);
        }
        else
        {
            dbg("session %08x, p_item_reply->timeout (%d) + p_item_reply->last_hit_time (%d) <= ppa_get_time_in_sec (%d)", (unsigned int)p_session, p_item_reply->timeout, p_item_reply->last_hit_time, tmp);
        }
    }

    //  not added in hardware
    if ( !f_flag )
        return IFX_PPA_SESSION_NOT_ADDED;

    return f_timeout ? IFX_PPA_TIMEOUT : IFX_PPA_HIT;
}

int32_t ifx_ppa_set_session_inactivity(PPA_U_SESSION *p_session, int32_t timeout)
{
    int32_t ret, ret_reply;
    struct session_list_item *p_item, *p_item_reply;

    if( p_session == NULL ) //for modifying ppa routing hook timer purpose
    {
        ppa_set_polling_timer(timeout, 1);
        return IFX_SUCCESS;
    }
    ret = ppa_lookup_session((PPA_SESSION *)p_session, 0, &p_item);
    ret_reply = ppa_lookup_session((PPA_SESSION *)p_session, 1, &p_item_reply);
    if ( ret != IFX_PPA_SESSION_EXISTS && ret_reply != IFX_PPA_SESSION_EXISTS )
        return IFX_ENOTAVAIL;

    if ( ret == IFX_PPA_SESSION_EXISTS )
        p_item->timeout = timeout;

    if ( ret_reply == IFX_PPA_SESSION_EXISTS )
        p_item_reply->timeout = timeout;

    ppa_set_polling_timer(timeout, 0);

    return IFX_SUCCESS;
}

/*
 *  PPA Bridge Session Operation Functions
 */

int32_t ifx_ppa_bridge_entry_add(uint8_t *mac_addr, PPA_NETIF *netif, uint32_t flags)
{
    struct bridging_session_list_item *p_item;

    if( !g_bridging_mac_learning ) return IFX_FAILURE;

    if ( ppa_bridging_lookup_session(mac_addr, netif, &p_item) == IFX_PPA_SESSION_EXISTS )
    {
        if ( (p_item->flags & SESSION_ADDED_IN_HW) )    //  added in hardware/firmware
            return IFX_PPA_SESSION_ADDED;
    }
    else if ( ppa_bridging_add_session(mac_addr, netif, &p_item, flags) != 0 )
        return IFX_PPA_SESSION_NOT_ADDED;

    if ( ppa_bridging_hw_add_session(p_item) != IFX_SUCCESS )
    {
        dbg("ppa_bridging_hw_add_session(%02x:%02x:%02x:%02x:%02x:%02x): fail", (uint32_t)p_item->mac[0], (uint32_t)p_item->mac[1], (uint32_t)p_item->mac[2], (uint32_t)p_item->mac[3], (uint32_t)p_item->mac[4], (uint32_t)p_item->mac[5]);
        return IFX_PPA_SESSION_NOT_ADDED;
    }

    dbg("hardware added");

    return IFX_PPA_SESSION_ADDED;
}

int32_t ifx_ppa_hook_bridge_enable(uint32_t f_enable, uint32_t flags)
{
    g_bridging_mac_learning = f_enable;
    return IFX_SUCCESS;
}

int32_t ifx_ppa_hook_get_bridge_status(uint32_t *f_enable, uint32_t flags)
{
    if( f_enable )
        *f_enable = g_bridging_mac_learning;
    return IFX_SUCCESS;
}

int32_t ifx_ppa_bridge_entry_delete(uint8_t *mac_addr, uint32_t flags)
{
    struct bridging_session_list_item *p_item;
    if( !g_bridging_mac_learning ) return IFX_FAILURE;

    if ( ppa_bridging_lookup_session(mac_addr, NULL, &p_item) != IFX_PPA_SESSION_EXISTS )
        return IFX_FAILURE;

    dump_bridging_list_item(p_item, "ifx_ppa_bridge_entry_delete");

    //  ppa_bridging_remove_session->ppa_bridging_free_session_list_item->ppa_bridging_hw_del_session will delete MAC entry from Firmware/Hardware
    ppa_bridging_remove_session(p_item);

    return IFX_SUCCESS;
}

int32_t ifx_ppa_bridge_entry_hit_time(uint8_t *mac_addr, uint32_t *p_hit_time)
{
    struct bridging_session_list_item *p_item;

    if ( ppa_bridging_lookup_session(mac_addr, NULL, &p_item) != IFX_PPA_SESSION_EXISTS )
        return IFX_PPA_SESSION_NOT_ADDED;

    *p_hit_time = p_item->last_hit_time;
    return IFX_PPA_HIT;
}

int32_t ifx_ppa_bridge_entry_inactivity_status(uint8_t *mac_addr)
{
    struct bridging_session_list_item *p_item;

    if ( ppa_bridging_lookup_session(mac_addr, NULL, &p_item) != IFX_PPA_SESSION_EXISTS )
        return IFX_PPA_SESSION_NOT_ADDED;

    //  not added in hardware
    if ( !(p_item->flags & SESSION_ADDED_IN_HW) )
        return IFX_PPA_SESSION_NOT_ADDED;

    if ( (p_item->flags & SESSION_STATIC) )
        return IFX_PPA_HIT;

    if ( p_item->timeout < ppa_get_time_in_sec() - p_item->last_hit_time )  //  use < other than <= to avoid "false positives"
        return IFX_PPA_TIMEOUT;

    return IFX_PPA_HIT;
}

int32_t ifx_ppa_set_bridge_entry_timeout(uint8_t *mac_addr, uint32_t timeout)
{
    struct bridging_session_list_item *p_item;

    if ( ppa_bridging_lookup_session(mac_addr, NULL, &p_item) != IFX_PPA_SESSION_EXISTS )
        return IFX_FAILURE;

    if ( !(p_item->flags & SESSION_STATIC) )
        p_item->timeout = timeout;

    ppa_bridging_set_polling_timer(timeout);

    return IFX_SUCCESS;
}

/*
 *  PPA Bridge VLAN Config Functions
 */

int32_t ifx_ppa_set_bridge_if_vlan_config(PPA_NETIF *netif, PPA_VLAN_TAG_CTRL *vlan_tag_control, PPA_VLAN_CFG *vlan_cfg, uint32_t flags)
{
    struct netif_info *ifinfo;
    PPE_BRDG_VLAN_CFG cfg={0};

    if ( ppa_netif_update(netif, NULL) != IFX_SUCCESS )
        return IFX_FAILURE;

    if ( ppa_netif_lookup(ppa_get_netif_name(netif), &ifinfo) != IFX_SUCCESS )
        return IFX_FAILURE;

    if ( !(ifinfo->flags & NETIF_PHYS_PORT_GOT) )
    {
        ppa_netif_put(ifinfo);
        return IFX_FAILURE;
    }

    cfg.if_no = ifinfo->phys_port;
    cfg.f_eg_vlan_insert= vlan_tag_control->insertion | vlan_tag_control->replace;
    cfg.f_eg_vlan_remove= vlan_tag_control->remove | vlan_tag_control->replace;
    cfg.f_ig_vlan_aware = vlan_cfg->vlan_aware | (vlan_tag_control->unmodified ? 0 : 1) | vlan_tag_control->insertion | vlan_tag_control->remove | vlan_tag_control->replace |(vlan_tag_control->out_unmodified ? 0 : 1) | vlan_tag_control->out_insertion | vlan_tag_control->out_remove | vlan_tag_control->out_replace;
    cfg.f_ig_src_ip_based = vlan_cfg->src_ip_based_vlan,
    cfg.f_ig_eth_type_based= vlan_cfg->eth_type_based_vlan;
    cfg.f_ig_vlanid_based = vlan_cfg->vlanid_based_vlan;
    cfg.f_ig_port_based= vlan_cfg->port_based_vlan;
    cfg.f_eg_out_vlan_insert = vlan_tag_control->out_insertion | vlan_tag_control->out_replace;
    cfg.f_eg_out_vlan_remove = vlan_tag_control->out_remove | vlan_tag_control->out_replace;
    cfg.f_ig_out_vlan_aware = vlan_cfg->out_vlan_aware | (vlan_tag_control->out_unmodified ? 0 : 1) | vlan_tag_control->out_insertion | vlan_tag_control->out_remove | vlan_tag_control->out_replace;
    ifx_ppa_drv_set_bridge_if_vlan_config( &cfg, 0);

    ppa_netif_put(ifinfo);

    return IFX_SUCCESS;
}

int32_t ifx_ppa_get_bridge_if_vlan_config(PPA_NETIF *netif, PPA_VLAN_TAG_CTRL *vlan_tag_control, PPA_VLAN_CFG *vlan_cfg, uint32_t flags)
{
    struct netif_info *ifinfo;
    PPE_BRDG_VLAN_CFG cfg={0};

    if ( ppa_netif_update(netif, NULL) != IFX_SUCCESS )
        return IFX_FAILURE;

    if ( ppa_netif_lookup(ppa_get_netif_name(netif), &ifinfo) != IFX_SUCCESS )
        return IFX_FAILURE;

    if ( !(ifinfo->flags & NETIF_PHYS_PORT_GOT) )
    {
        ppa_netif_put(ifinfo);
        return IFX_FAILURE;
    }

    cfg.if_no = ifinfo->phys_port;
    ifx_ppa_drv_get_bridge_if_vlan_config( &cfg, 0);

    vlan_tag_control->unmodified    = cfg.f_ig_vlan_aware ? 0 : 1;
    vlan_tag_control->insertion     = cfg.f_eg_vlan_insert ? 1 : 0;
    vlan_tag_control->remove        = cfg.f_eg_vlan_remove ? 1 : 0;
    vlan_tag_control->replace       = cfg.f_eg_vlan_insert && cfg.f_eg_vlan_remove ? 1 : 0;
    vlan_cfg->vlan_aware            = cfg.f_ig_vlan_aware ? 1 : 0;
    vlan_cfg->src_ip_based_vlan     = cfg.f_ig_src_ip_based ? 1 : 0;
    vlan_cfg->eth_type_based_vlan   = cfg.f_ig_eth_type_based ? 1 : 0;
    vlan_cfg->vlanid_based_vlan     = cfg.f_ig_vlanid_based ? 1 : 0;
    vlan_cfg->port_based_vlan       = cfg.f_ig_port_based ? 1 : 0;
    vlan_tag_control->out_unmodified    = cfg.f_ig_out_vlan_aware ? 0 : 1;
    vlan_tag_control->out_insertion     = cfg.f_eg_out_vlan_insert ? 1 : 0;
    vlan_tag_control->out_remove        = cfg.f_eg_out_vlan_remove ? 1 : 0;
    vlan_tag_control->out_replace       = cfg.f_eg_out_vlan_insert && cfg.f_eg_out_vlan_remove ? 1 : 0;
    vlan_cfg->out_vlan_aware            = cfg.f_ig_out_vlan_aware ? 1 : 0;

    ppa_netif_put(ifinfo);

    return IFX_SUCCESS;
}

int32_t ifx_ppa_vlan_filter_add(PPA_VLAN_MATCH_FIELD *vlan_match_field, PPA_VLAN_INFO *vlan_info, uint32_t flags)
{
    struct netif_info *ifinfo;
    int i;
    PPE_BRDG_VLAN_FILTER_MAP vlan_filter={0};

    vlan_filter.out_vlan_info.vlan_id= vlan_info->out_vlan_id;
    if ( ifx_ppa_drv_add_outer_vlan_entry( &vlan_filter.out_vlan_info , 0) != IFX_SUCCESS )
    {
        dbg("add out_vlan_ix error for out vlan id:%d", vlan_info->out_vlan_id);
        return IFX_FAILURE;
    }

    switch ( vlan_match_field->match_flags )
    {
    case PPA_F_VLAN_FILTER_IFNAME:
        if ( ppa_netif_update(NULL, vlan_match_field->match_field.ifname) != IFX_SUCCESS )
            return IFX_FAILURE;
        if ( ppa_netif_lookup(vlan_match_field->match_field.ifname, &ifinfo) != IFX_SUCCESS )
            return IFX_FAILURE;
        if ( !(ifinfo->flags & NETIF_PHYS_PORT_GOT) )
        {
            ppa_netif_put(ifinfo);
            return IFX_FAILURE;
        }
        vlan_filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF;
        vlan_filter.ig_criteria         = ifinfo->phys_port;
        ppa_netif_put(ifinfo);
        break;
    case PPA_F_VLAN_FILTER_IP_SRC:
        vlan_filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP;
        vlan_filter.ig_criteria         = vlan_match_field->match_field.ip_src;
        break;
    case PPA_F_VLAN_FILTER_ETH_PROTO:
        vlan_filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE;
        vlan_filter.ig_criteria         = vlan_match_field->match_field.eth_protocol;
        break;
    case PPA_F_VLAN_FILTER_VLAN_TAG:
        vlan_filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN;
        vlan_filter.ig_criteria         = vlan_match_field->match_field.ingress_vlan_tag;
        break;
    default:
        return IFX_FAILURE;
    }

    vlan_filter.new_vci             = vlan_info->vlan_vci;

    vlan_filter.vlan_port_map = 0;
    for ( i = 0; i < vlan_info->num_ifs; i++ )
    {
        if ( ppa_netif_update(NULL, vlan_info->vlan_if_membership[i].ifname) != IFX_SUCCESS )
            continue;
        if ( ppa_netif_lookup(vlan_info->vlan_if_membership[i].ifname, &ifinfo) != IFX_SUCCESS )
            continue;
        if ( (ifinfo->flags & NETIF_PHYS_PORT_GOT) )
            vlan_filter.vlan_port_map |= 1 << ifinfo->phys_port;
        ppa_netif_put(ifinfo);
    }

    vlan_filter.dest_qos = vlan_info->qid;
    vlan_filter.in_out_etag_ctrl = vlan_info->inner_vlan_tag_ctrl | vlan_info->out_vlan_tag_ctrl;

    return ifx_ppa_drv_add_vlan_map( &vlan_filter, 0);
}

int32_t ifx_ppa_vlan_filter_del(PPA_VLAN_MATCH_FIELD *vlan_match_field, PPA_VLAN_INFO *vlan_info, uint32_t flags)
{
    PPE_BRDG_VLAN_FILTER_MAP filter={0};
    PPE_VFILTER_COUNT_CFG vfilter_count={0};
    struct netif_info *ifinfo;
    uint32_t i;
    uint8_t bfMatch=0;
    int32_t res;

    switch ( vlan_match_field->match_flags )
    {
    case PPA_F_VLAN_FILTER_IFNAME:
        if ( ppa_netif_update(NULL, vlan_match_field->match_field.ifname) != IFX_SUCCESS )
            return IFX_FAILURE;
        if ( ppa_netif_lookup(vlan_match_field->match_field.ifname, &ifinfo) != IFX_SUCCESS )
            return IFX_FAILURE;
        if ( !(ifinfo->flags & NETIF_PHYS_PORT_GOT) )
        {
            ppa_netif_put(ifinfo);
            return IFX_FAILURE;
        }
        filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF;
        filter.ig_criteria         = ifinfo->phys_port;
        ppa_netif_put(ifinfo);
        break;
    case PPA_F_VLAN_FILTER_IP_SRC:
        filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP;
        filter.ig_criteria         = vlan_match_field->match_field.ip_src;
        break;
    case PPA_F_VLAN_FILTER_ETH_PROTO:
        filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE;
        filter.ig_criteria         = vlan_match_field->match_field.eth_protocol;
        break;
    case PPA_F_VLAN_FILTER_VLAN_TAG:
        filter.ig_criteria_type    = IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN;
        filter.ig_criteria         = vlan_match_field->match_field.ingress_vlan_tag;
        break;
    default:
        return IFX_FAILURE;
    }

    //check whether there is such kind of vlan filter to delete
    vfilter_count.vfitler_type = filter.ig_criteria_type;
    ifx_ppa_drv_get_max_vfilter_entries( &vfilter_count, 0 );
    for ( i = 0; i < vfilter_count.num; i++ )
    {
        filter.entry = i;
        if ( (res = ifx_ppa_drv_get_vlan_map(&filter , 0) ) != -1 ) //get fail. break;
        {
            bfMatch = 1;
            break;
        }
    }

    if ( !bfMatch )
    {
        dbg("ifx_ppa_vlan_filter_del: canot find such kinds of vlan filter \n");
        return IFX_FAILURE;
    }
    if ( res == 0 ) //blank item
        return IFX_SUCCESS;

    ifx_ppa_drv_del_vlan_map( &filter, 0 );

    ifx_ppa_drv_del_outer_vlan_entry( &filter.out_vlan_info, 0);

    return IFX_SUCCESS;
}

int32_t ifx_ppa_vlan_filter_get_all(int32_t *num_filters, PPA_VLAN_FILTER_CONFIG *vlan_filters, uint32_t flags)
{
//#warning ifx_ppa_vlan_filter_get_all is not implemented, too many memory allocation problem
    return IFX_ENOTIMPL;
}

int32_t ifx_ppa_vlan_filter_del_all(uint32_t flags)
{
    int32_t i, j;
    uint32_t vlan_filter_type[]={PPA_F_VLAN_FILTER_IFNAME, PPA_F_VLAN_FILTER_IP_SRC, PPA_F_VLAN_FILTER_ETH_PROTO, PPA_F_VLAN_FILTER_VLAN_TAG};
    PPE_BRDG_VLAN_FILTER_MAP filter={0};
    PPE_VFILTER_COUNT_CFG vfilter_count={0};

    for ( i = 0; i < NUM_ENTITY(vlan_filter_type); i++ )
    {
        vfilter_count.vfitler_type = vlan_filter_type[i];
        ifx_ppa_drv_get_max_vfilter_entries( &vfilter_count, 0);

        for ( j = 0; j < vfilter_count.num; j++ )
        {
            filter.ig_criteria_type = vlan_filter_type[i];
            filter.entry = j;
            if( ifx_ppa_drv_get_vlan_map( &filter, 0)  == 1 )
            {
                ifx_ppa_drv_del_outer_vlan_entry( &filter.out_vlan_info, 0);
            }
        }
    }

    ifx_ppa_drv_del_all_vlan_map( 0);
    return IFX_SUCCESS;
}

/*
 *  PPA MIB Counters Operation Functions
 */

int32_t ifx_ppa_get_if_stats(PPA_IFNAME *ifname, PPA_IF_STATS *p_stats, uint32_t flags)
{
    struct netif_info *p_info;
    uint32_t port_flags;
    PPE_ITF_MIB_INFO itf_mib={0};

    if ( !ifname || !p_stats )
        return IFX_EINVAL;

    if ( ppa_netif_lookup(ifname, &p_info) != IFX_SUCCESS )
        return IFX_EIO;
    itf_mib.itf= p_info->phys_port;
    port_flags = p_info->flags;
    ppa_netif_put(p_info);

    if ( !(port_flags & NETIF_PHYS_PORT_GOT) )
        return IFX_EIO;

    ifx_ppa_drv_get_itf_mib(&itf_mib, 0);

    p_stats->rx_pkts            = itf_mib.mib.ig_cpu_pkts;
    p_stats->tx_discard_pkts    = itf_mib.mib.ig_drop_pkts;
    p_stats->rx_bytes           = itf_mib.mib.ig_cpu_bytes;

    return IFX_SUCCESS;
}

int32_t ifx_ppa_get_accel_stats(PPA_IFNAME *ifname, PPA_ACCEL_STATS *p_stats, uint32_t flags)
{
    struct netif_info *p_info;
    uint32_t port;
    uint32_t port_flags;
    PPE_ITF_MIB_INFO mib = {0};

    if ( !ifname || !p_stats )
        return IFX_EINVAL;

    if ( ppa_netif_lookup(ifname, &p_info) != IFX_SUCCESS )
        return IFX_EIO;
    port = p_info->phys_port;
    port_flags = p_info->flags;
    ppa_netif_put(p_info);

    if ( !(port_flags & NETIF_PHYS_PORT_GOT) )
        return IFX_EIO;

    mib.itf = p_info->phys_port;
    mib.flag = flags;
    ifx_ppa_drv_get_itf_mib(&mib, 0);

    p_stats->fast_routed_tcp_pkts       = mib.mib.ig_fast_rt_ipv4_tcp_pkts + mib.mib.ig_fast_rt_ipv6_tcp_pkts;
    p_stats->fast_routed_udp_pkts       = mib.mib.ig_fast_rt_ipv4_udp_pkts + mib.mib.ig_fast_rt_ipv6_udp_pkts;
    p_stats->fast_routed_udp_mcast_pkts = mib.mib.ig_fast_rt_ipv4_mc_pkts;
    p_stats->fast_drop_pkts             = mib.mib.ig_drop_pkts;
    p_stats->fast_drop_bytes            = mib.mib.ig_drop_bytes;
    p_stats->fast_ingress_cpu_pkts      = mib.mib.ig_cpu_pkts;
    p_stats->fast_ingress_cpu_bytes     = mib.mib.ig_cpu_bytes;
    p_stats->fast_bridged_ucast_pkts    = mib.mib.ig_fast_brg_pkts;
    p_stats->fast_bridged_bytes         = mib.mib.ig_fast_brg_bytes;

    return IFX_SUCCESS;
}

/*
 *  PPA Network Interface Operation Functions
 */

int32_t ifx_ppa_set_if_mac_address(PPA_IFNAME *ifname, uint8_t *mac_addr, uint32_t flags)
{
    struct netif_info *ifinfo;
    PPE_ROUTE_MAC_INFO mac_info={0};

    if ( !ifname || !mac_addr )
        return IFX_EINVAL;

    if ( ppa_netif_lookup(ifname, &ifinfo) != IFX_SUCCESS )
        return IFX_FAILURE;

    if ( (ifinfo->flags & NETIF_MAC_ENTRY_CREATED) )
    {
        mac_info.mac_ix= ifinfo->mac_entry;
        ifx_ppa_drv_del_mac_entry(&mac_info, 0);
        ifinfo->mac_entry = ~0;
        ifinfo->flags &= ~NETIF_MAC_ENTRY_CREATED;
    }

    ppa_memcpy(ifinfo->mac, mac_addr, PPA_ETH_ALEN);
    ifinfo->flags |= NETIF_MAC_AVAILABLE;

    ppa_memcmp(mac_info.mac, mac_addr, sizeof(mac_info.mac));
    if ( ifx_ppa_drv_add_mac_entry( &mac_info, 0) == IFX_SUCCESS )
    {
        ifinfo->mac_entry = mac_info.mac_ix;
        ifinfo->flags |= NETIF_MAC_ENTRY_CREATED;
    }

    ppa_netif_put(ifinfo);

    return IFX_SUCCESS;
}

int32_t ifx_ppa_get_if_mac_address(PPA_IFNAME *ifname, uint8_t *mac_addr, uint32_t flags)
{
    int32_t ret;
    struct netif_info *ifinfo;

    if ( !ifname || !mac_addr )
        return IFX_EINVAL;

    if ( ppa_netif_lookup(ifname, &ifinfo) != IFX_SUCCESS )
        return IFX_FAILURE;

    if ( (ifinfo->flags & NETIF_MAC_AVAILABLE) )
    {
        ppa_memcpy(mac_addr,ifinfo->mac, PPA_ETH_ALEN);
        ret = IFX_SUCCESS;
    }
    else
        ret = IFX_FAILURE;

    ppa_netif_put(ifinfo);

    return ret;
}

int32_t ifx_ppa_add_if(PPA_IFINFO *ifinfo, uint32_t flags)
{
    if ( !ifinfo )
        return IFX_EINVAL;

    return ppa_netif_add(ifinfo->ifname, ifinfo->if_flags & PPA_F_LAN_IF, NULL);
}

int32_t ifx_ppa_del_if(PPA_IFINFO *ifinfo, uint32_t flags)
{
    ppa_netif_remove(ifinfo->ifname, ifinfo->if_flags & PPA_F_LAN_IF);

    ppa_remove_sessions_on_netif(ifinfo->ifname, ifinfo->if_flags & PPA_F_LAN_IF);

    return IFX_SUCCESS;
}

int32_t ifx_ppa_get_if(int32_t *num_ifs, PPA_IFINFO **ifinfo, uint32_t flags)
{
    uint32_t pos = 0;
    struct netif_info *info;
    int32_t num = 0;
    PPA_IFINFO *p_ifinfo;

    if ( !num_ifs || !ifinfo )
        return IFX_EINVAL;

    p_ifinfo = (PPA_IFINFO *)ppa_malloc(100 * sizeof(PPA_IFINFO));  //  assume max 100 netif
    if(!p_ifinfo){
        return IFX_ENOMEM;
    }

    if ( ppa_netif_start_iteration(&pos, &info) != IFX_SUCCESS ){
        ppa_free(p_ifinfo);
        return IFX_FAILURE;
    }

    do
    {
        if ( (info->flags & NETIF_LAN_IF) )
        {
            p_ifinfo[num].ifname = info->name;
            p_ifinfo[num].if_flags = PPA_F_LAN_IF;
            num++;
        }
        if ( (info->flags & NETIF_WAN_IF) )
        {
            p_ifinfo[num].ifname = info->name;
            p_ifinfo[num].if_flags = 0;
            num++;
        }
    } while ( ppa_netif_iterate_next(&pos, &info) == IFX_SUCCESS );

    ppa_netif_stop_iteration();

    *num_ifs = num;
    *ifinfo = p_ifinfo;

    return IFX_SUCCESS;
}

int32_t ifx_ppa_multicast_pkt_srcif_add(PPA_BUF *pkt_buf, PPA_NETIF *rx_if)
{
    IP_ADDR_C ip={0};
    struct mc_group_list_item *p_item;
    struct netif_info *p_netif_info;
    int32_t res = IFX_PPA_SESSION_NOT_ADDED;

    if( !rx_if )
    {
        rx_if = ppa_get_pkt_src_if( pkt_buf);
    }

    //Now it only support IPv4 yet
    ip.ip.ip = ppa_get_pkt_src_ip2( pkt_buf);
    ip.f_ipv6 = 0;

    if( ip.ip.ip == 0 )
    {
        return res;
    }

    if( mc_updating_flag ) return IFX_PPA_SESSION_NOT_ADDED;
    if ( ppa_lookup_mc_group(ip, &p_item) == IFX_PPA_SESSION_EXISTS )
    {
        if( p_item->src_netif &&  p_item->src_netif != rx_if )
        { /*at present, we don't allowed to change multicast src_if */
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "Not matched src if: original srcif is %s, but new srcif is %s: %d.%d.%d.%d\n", ppa_get_netif_name(p_item->src_netif), ppa_get_netif_name(rx_if), ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip & 0xFF);
            return res;
        }
        if( p_item->flags & SESSION_ADDED_IN_HW )
        { //already added into HW. no change here
            return IFX_PPA_SESSION_ADDED;
        }

        if( ppa_is_netif_bridged(NULL, rx_if) )
            p_item->bridging_flag =1; //If the receive interface is in bridge, then regard it as bridge mode
        else
            p_item->bridging_flag =0;

        //  add to HW if possible
        if (  ppa_netif_lookup( ppa_get_netif_name( rx_if), &p_netif_info) != IFX_SUCCESS )
        {
            ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ifx_ppa_multicast_pkt_srcif_add cannot get interface %s for  multicast session info: %d.%d.%d.%d\n", ppa_get_netif_name(rx_if), ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip & 0xFF) ;
            return IFX_PPA_SESSION_NOT_ADDED;
        }

        if ( ppa_netif_update(rx_if, NULL) == IFX_SUCCESS && (p_netif_info->flags & NETIF_PHYS_PORT_GOT) )
        {
            p_item->src_mac_entry = p_netif_info->mac_entry;

            //  PPPoE and source mac
            if ( !p_item->bridging_flag )
            {
                if( p_netif_info->flags & NETIF_PPPOE )
                    p_item->flags |= SESSION_VALID_PPPOE;
                p_item->flags |= SESSION_VALID_SRC_MAC;
            }

            //  VLAN
            ASSERT(p_netif_info->flags & NETIF_VLAN_CANT_SUPPORT, "MC processing can support two layers of VLAN only");

            if ( (p_netif_info->flags & NETIF_VLAN_OUTER) )
                p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
            if ( (p_netif_info->flags & NETIF_VLAN_INNER) )
                p_item->flags |= SESSION_VALID_VLAN_RM;

            p_item->src_netif = p_netif_info->netif;
            ppa_netif_put(p_netif_info);

            if( ppa_hw_add_mc_group(p_item) != IFX_SUCCESS )
            {
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): fail ???", ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip& 0xFF);
            }
            else
            {
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
                ifx_ppa_pwm_activate_module();
#endif
                res = IFX_PPA_SESSION_ADDED;
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_add_mc_group(%d.%d.%d.%d): sucessfully", ip.ip.ip >> 24, (ip.ip.ip >> 16) & 0xFF, (ip.ip.ip >> 8) & 0xFF, ip.ip.ip & 0xFF);
            }
        }
    }
    else
    {
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "Not found the multicast group in existing list:%u.%u.%u.%u\n", NIPQUAD((ip.ip.ip)) );
    }

    return res;
}

int32_t ifx_ppa_hook_wan_mii0_vlan_range_add(PPA_VLAN_RANGE *vlan_range, uint32_t flags)
{
    if ( vlan_range )
    {
        PPE_WAN_VID_RANGE vid={0};
        dbg("vlanrange: from %x to %x", vlan_range->start_vlan_range, vlan_range->end_vlan_range);
        vid.vid = (vlan_range->start_vlan_range & 0xFFF ) | ( (vlan_range->end_vlan_range & 0xFFF ) << 16) ;
        ifx_ppa_drv_set_mixed_wan_vlan_id( &vid, 0 );

        return IFX_SUCCESS;
    }
    return IFX_FAILURE;
}

int32_t ifx_ppa_hook_wan_mii0_vlan_range_del(PPA_VLAN_RANGE *vlan_range, int32_t flags)
{
    if ( vlan_range )
    {
        PPE_WAN_VID_RANGE vid = {0};
        vid.vid = (vlan_range->start_vlan_range & 0xFFF ) | ( (vlan_range->end_vlan_range & 0xFFF ) << 16) ;
        ifx_ppa_drv_set_mixed_wan_vlan_id(&vid, 0);
        return IFX_SUCCESS;
    }

    return IFX_FAILURE;
}

int32_t ifx_ppa_hook_wan_mii0_vlan_ranges_get(int32_t *num_ranges, PPA_VLAN_RANGE *vlan_ranges, uint32_t flags)
{
    if ( vlan_ranges && num_ranges )
    {
        PPE_WAN_VID_RANGE vlanid = {0};

        ifx_ppa_drv_get_mixed_wan_vlan_id(&vlanid, 0);

        vlan_ranges->start_vlan_range = vlanid.vid& 0xFFF;
        vlan_ranges->end_vlan_range = ( vlanid.vid>> 12 ) & 0xFFF;

        *num_ranges = 1;
        return IFX_SUCCESS;
    }

    return IFX_FAILURE;
}

int32_t ifx_ppa_get_max_entries(PPA_MAX_ENTRY_INFO *max_entry, uint32_t flags)
{
    if( !max_entry ) return IFX_FAILURE;

    ifx_ppa_drv_get_max_entries(max_entry, 0);

    return IFX_SUCCESS;
}

int32_t ppa_ip_sprintf( char *buf, PPA_IPADDR ip, uint32_t flag)
{
    int32_t len=0;
    if( buf)  {
#ifdef CONFIG_IFX_PPA_IPv6_ENABLE
        if( flag & SESSION_IS_IPV6 ) {
            len = ppa_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", NIP6(ip.ip6) );
        }
        else
#endif
            len = ppa_sprintf(buf, "%u.%u.%u.%u", NIPQUAD(ip.ip) );

    }

    return len;
}

int32_t ppa_ip_comare( PPA_IPADDR ip1, PPA_IPADDR ip2, uint32_t flag )
{
#ifdef CONFIG_IFX_PPA_IPv6_ENABLE
    if( flag & SESSION_IS_IPV6 )
    {
        return ppa_memcmp(ip1.ip6, ip2.ip6, sizeof(ip1.ip6) );
    }
    else
#endif
    {
         return ppa_memcmp(&ip1.ip, &ip2.ip, sizeof(ip1.ip) );
    }
}


EXPORT_SYMBOL(ifx_ppa_is_init);
EXPORT_SYMBOL(ifx_ppa_init);
EXPORT_SYMBOL(ifx_ppa_exit);
EXPORT_SYMBOL(ifx_ppa_enable);
EXPORT_SYMBOL(ifx_ppa_add_if);
EXPORT_SYMBOL(ifx_ppa_del_if);
EXPORT_SYMBOL(ifx_ppa_get_max_entries);
EXPORT_SYMBOL( ppa_ip_sprintf);
EXPORT_SYMBOL(ppa_ip_comare);
