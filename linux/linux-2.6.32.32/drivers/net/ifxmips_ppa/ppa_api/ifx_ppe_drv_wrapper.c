/******************************************************************************
**
** FILE NAME    : ifx_ppe_drv_wrapper.c
** PROJECT      : PPA
** MODULES      : PPA Wrapper for PPE Driver API
**
** DATE         : 14 Mar 2011
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Wrapper for PPE Driver API
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 14 MAR 2011  Shao Guohua       Initiate Version
*******************************************************************************/

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
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/atmdev.h>
#include <net/sock.h>

/*
 *  Chip Specific Head File
 */
#include <net/ifx_ppa_api.h>
#include "ifx_ppe_drv_wrapper.h"
#include "../platform/ifx_ppa_ppe_hal.h"
#include "../platform/ifx_ppa_datapath.h"

/*Hook API for PPE Driver's Datapath layer: these hook will be set in PPE datapath driver*/
/* First part is for direct path */
struct ppe_directpath_data *ifx_ppa_drv_g_ppe_directpath_data = NULL;
int32_t (*ifx_ppa_drv_directpath_send_hook)(uint32_t, PPA_BUF *, int32_t, uint32_t) = NULL;
int32_t (*ifx_ppa_drv_directpath_rx_stop_hook)(uint32_t, uint32_t) = NULL;
int32_t (*ifx_ppa_drv_directpath_rx_start_hook)(uint32_t, uint32_t) = NULL;

/*    others:: these hook will be set in PPE datapath driver  */
int (*ifx_ppa_drv_get_dslwan_qid_with_vcc_hook)(struct atm_vcc *vcc)= NULL;
int (*ifx_ppa_drv_get_netif_qid_with_pkt_hook)(struct sk_buff *skb, void *arg, int is_atm_vcc)= NULL;
int (*ifx_ppa_drv_ppe_clk_change_hook)(unsigned int arg, unsigned int flags)= NULL;
int32_t (*ifx_ppa_drv_datapath_generic_hook)(PPA_GENERIC_DATAPATH_HOOK_CMD cmd, void *buffer, uint32_t flag)=NULL;
//below hook will be exposed from datapath driver and called by its hal driver.
int32_t (*ifx_ppa_drv_datapath_mac_entry_setting)(uint8_t  *mac, uint32_t fid, uint32_t portid, uint32_t agetime, uint32_t st_entry , uint32_t action) = NULL;

/*Hook API for PPE Driver's HAL layer: these hook will be set in PPE HAL driver */
int32_t (*ifx_ppa_drv_hal_generic_hook)(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag)=NULL;


int ifx_ppa_drv_directpath_send(uint32_t if_id, struct sk_buff *skb, int32_t len, uint32_t flags)
{
    if( !ifx_ppa_drv_directpath_send_hook ) return IFX_EINVAL;
    return ifx_ppa_drv_directpath_send_hook(if_id, skb, len, flags);    
}

int ifx_ppa_drv_directpath_rx_stop(uint32_t if_id, uint32_t flags)
{
     if( !ifx_ppa_drv_directpath_rx_stop_hook ) return IFX_EINVAL;
     return ifx_ppa_drv_directpath_rx_stop_hook(if_id, flags);  
}

int ifx_ppa_drv_directpath_rx_start(uint32_t if_id, uint32_t flags)
{
     if( !ifx_ppa_drv_directpath_rx_start_hook ) return IFX_EINVAL;
     return ifx_ppa_drv_directpath_rx_start_hook(if_id, flags);  
}

int ifx_ppa_drv_get_dslwan_qid_with_vcc(struct atm_vcc *vcc)
{
    if( !ifx_ppa_drv_get_dslwan_qid_with_vcc_hook ) return 0;
    else return ifx_ppa_drv_get_dslwan_qid_with_vcc_hook(vcc);
}

int ifx_ppa_drv_get_netif_qid_with_pkt(struct sk_buff *skb, void *arg, int is_atm_vcc)
{
    if( !ifx_ppa_drv_get_netif_qid_with_pkt_hook ) return 0;
    else return ifx_ppa_drv_get_netif_qid_with_pkt_hook(skb, arg, is_atm_vcc);

}

int ifx_ppa_drv_ppe_clk_change(unsigned int arg, unsigned int flags)
{
    if( !ifx_ppa_drv_ppe_clk_change_hook ) return 0;
    else return ifx_ppa_drv_ppe_clk_change_hook(arg, flags);
}

uint32_t ifx_ppa_drv_dp_sb_addr_to_fpi_addr_convert(PPA_FPI_ADDR*a, uint32_t flag)
{
    if( !ifx_ppa_drv_datapath_generic_hook ) return IFX_FAILURE;
    return ifx_ppa_drv_datapath_generic_hook(PPA_GENERIC_DATAPATH_ADDR_TO_FPI_ADDR, (void *)a, flag ); 
    
}

//for PPE driver's datapath APIs
EXPORT_SYMBOL(ifx_ppa_drv_g_ppe_directpath_data);
EXPORT_SYMBOL(ifx_ppa_drv_directpath_send_hook);
EXPORT_SYMBOL(ifx_ppa_drv_directpath_rx_stop_hook);
EXPORT_SYMBOL(ifx_ppa_drv_directpath_rx_start_hook);
EXPORT_SYMBOL(ifx_ppa_drv_directpath_send);
EXPORT_SYMBOL(ifx_ppa_drv_directpath_rx_stop);
EXPORT_SYMBOL(ifx_ppa_drv_directpath_rx_start);

EXPORT_SYMBOL(ifx_ppa_drv_get_dslwan_qid_with_vcc_hook);
EXPORT_SYMBOL(ifx_ppa_drv_get_netif_qid_with_pkt_hook);
EXPORT_SYMBOL(ifx_ppa_drv_ppe_clk_change_hook);
EXPORT_SYMBOL(ifx_ppa_drv_get_dslwan_qid_with_vcc);
EXPORT_SYMBOL(ifx_ppa_drv_get_netif_qid_with_pkt);
EXPORT_SYMBOL(ifx_ppa_drv_ppe_clk_change);
EXPORT_SYMBOL(ifx_ppa_drv_hal_generic_hook);
EXPORT_SYMBOL(ifx_ppa_drv_datapath_generic_hook);
EXPORT_SYMBOL(ifx_ppa_drv_datapath_mac_entry_setting);
EXPORT_SYMBOL(ifx_ppa_drv_dp_sb_addr_to_fpi_addr_convert);
//Below wrapper is for PPE driver datapath..---- Not sure whether need to add synchronization or not ---end


/*all below wrapper is for PPE driver's hal API */

uint32_t ifx_ppa_drv_hal_init(uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_INIT, (void *)NULL, flag ); 
}

uint32_t ifx_ppa_drv_hal_exit(uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_EXIT, (void *)NULL, flag ); 
}

uint32_t ifx_ppa_drv_get_ppe_hal_id(PPA_VERSION *v, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;
    
    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_HAL_VERSION,(void *)v, flag );    
}

uint32_t ifx_ppa_drv_get_firmware_id(PPA_VERSION *v, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_PPE_FW_VERSION,(void *)v, flag );
}


uint32_t ifx_ppa_drv_get_number_of_phys_port(PPE_COUNT_CFG *count, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return 0;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_PHYS_PORT_NUM,(void *)count, flag );
}

uint32_t ifx_ppa_drv_get_phys_port_info(PPE_IFINFO *info, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_PHYS_PORT_INFO,(void *)info, flag );
}

uint32_t ifx_ppa_drv_get_max_entries(PPA_MAX_ENTRY_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_MAX_ENTRIES,(void *)entry, flag );
}


uint32_t ifx_ppa_drv_set_mixed_wan_vlan_id(PPE_WAN_VID_RANGE *vid, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_MIX_WAN_VLAN_ID,(void *)vid, flag );
}

uint32_t ifx_ppa_drv_get_mixed_wan_vlan_id(PPE_WAN_VID_RANGE *vid, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_MIX_WAN_VLAN_ID,(void *)vid, flag );
}

uint32_t ifx_ppa_drv_set_route_cfg(PPE_ROUTING_CFG *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_ROUT_CFG,(void *)cfg, flag );
}


uint32_t ifx_ppa_drv_set_bridging_cfg(PPE_BRDG_CFG *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_BRDG_CFG,(void *)cfg, flag );
}


uint32_t ifx_ppa_drv_set_fast_mode(PPE_FAST_MODE_CFG *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_FAST_MODE_CFG,(void *)cfg, flag );
}

uint32_t ifx_ppa_drv_set_default_dest_list( PPE_DEST_LIST *cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_DEST_LIST, (void *)cfg, flag ); 
}   

uint32_t ifx_ppa_drv_get_acc_mode(PPE_ACC_ENABLE *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_ACC_ENABLE,(void *)cfg, flag );
}

uint32_t ifx_ppa_drv_set_acc_mode(PPE_ACC_ENABLE *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_ACC_ENABLE,(void *)cfg, flag );
}

uint32_t ifx_ppa_drv_set_bridge_if_vlan_config(PPE_BRDG_VLAN_CFG *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_BRDG_VLAN_CFG,(void *)cfg, flag );
}

uint32_t ifx_ppa_drv_get_bridge_if_vlan_config(PPE_BRDG_VLAN_CFG *cfg, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_BRDG_VLAN_CFG,(void *)cfg, flag );
}


uint32_t ifx_ppa_drv_add_vlan_map(PPE_BRDG_VLAN_FILTER_MAP *fitler, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_BRDG_VLAN_FITLER,(void *)fitler, flag );
}

uint32_t ifx_ppa_drv_del_vlan_map(PPE_BRDG_VLAN_FILTER_MAP *fitler, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_BRDG_VLAN_FITLER,(void *)fitler, flag );
}


uint32_t ifx_ppa_drv_get_vlan_map(PPE_BRDG_VLAN_FILTER_MAP *fitler, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_BRDG_VLAN_FITLER,(void *)fitler, flag );
}


uint32_t ifx_ppa_drv_del_all_vlan_map(uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_BRDG_VLAN_ALL_FITLER_MAP,(void *)NULL, flag );
}

uint32_t ifx_ppa_drv_is_ipv6_enabled(uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_IPV6_FLAG,(void *)NULL, flag );
}


uint32_t ifx_ppa_drv_add_routing_entry(PPE_ROUTING_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_ROUTE_ENTRY,(void *)entry, flag );
}
    
uint32_t ifx_ppa_drv_del_routing_entry(PPE_ROUTING_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_ROUTE_ENTRY,(void *)entry, flag );
}
    
uint32_t ifx_ppa_drv_update_routing_entry(PPE_ROUTING_INFO *entry , uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_UPDATE_ROUTE_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_add_wan_mc_entry(PPE_MC_INFO *entry , uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_MC_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_del_wan_mc_entry(PPE_MC_INFO *entry, uint32_t flag )
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_MC_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_update_wan_mc_entry(PPE_MC_INFO *entry, uint32_t flag )
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_UPDATE_MC_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_add_bridging_entry(PPE_BR_MAC_INFO *entry, uint32_t flag )
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_BR_MAC_BRIDGING_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_del_bridging_entry(PPE_BR_MAC_INFO *entry, uint32_t flag )
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_BR_MAC_BRIDGING_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_add_tunnel_entry(PPE_TUNNEL_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    if(entry->tunnel_type == SESSION_TUNNEL_6RD){
        return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_6RD_TUNNEL_ENTRY,(void *)entry, flag );
    }

    return IFX_FAILURE;
}

uint32_t ifx_ppa_drv_del_tunnel_entry(PPE_TUNNEL_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    if(entry->tunnel_type == SESSION_TUNNEL_6RD){
        return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_6RD_TUNNEL_ENTRY,(void *)entry, flag );
    }

    return IFX_FAILURE;
}


uint32_t ifx_ppa_drv_add_pppoe_entry(PPE_PPPOE_INFO *entry, uint32_t flag )
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_PPPOE_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_del_pppoe_entry(PPE_PPPOE_INFO *entry, uint32_t flag )
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_PPPOE_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_add_mtu_entry(PPE_MTU_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_MTU_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_del_mtu_entry(PPE_MTU_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_MTU_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_get_mtu_entry(PPE_MTU_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_MTU_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_get_routing_entry_bytes(PPE_ROUTING_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_ROUTE_ACC_BYTES,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_add_mac_entry(PPE_ROUTE_MAC_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_MAC_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_del_mac_entry(PPE_ROUTE_MAC_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_MAC_ENTRY,(void *)entry, flag );
}
    
uint32_t ifx_ppa_drv_get_mac_entry(PPE_ROUTE_MAC_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_MAC_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_add_outer_vlan_entry( PPE_OUT_VLAN_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_ADD_OUT_VLAN_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_del_outer_vlan_entry(PPE_OUT_VLAN_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_DEL_OUT_VLAN_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_get_outer_vlan_entry(PPE_OUT_VLAN_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_OUT_VLAN_ENTRY,(void *)entry, flag );
}

uint32_t ifx_ppa_drv_get_itf_mib( PPE_ITF_MIB_INFO *mib, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_ITF_MIB,(void *)mib, flag );
}

uint32_t ifx_ppa_drv_get_dsl_mib(PPA_DSL_QUEUE_MIB *mib, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) 
        return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_DSL_MIB,(void *)mib, flag); 
}


uint32_t ifx_ppa_drv_get_ports_mib(PPA_PORT_MIB *mib, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_PORT_MIB,(void *)mib, flag ); 
}    

//#ifdef CONFIG_IFX_PPA_MFE
uint32_t ifx_ppa_drv_multifield_control(PPE_ENABLE_CFG *enable, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_CONTROL,(void *)enable, flag ); 
}    


uint32_t ifx_ppa_drv_get_multifield_status(PPE_ENABLE_CFG *enable, uint32_t flag)  //hook
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_STATUS,(void *)enable, flag ); 
}

uint32_t ifx_ppa_drv_get_multifield_max_flow( PPE_COUNT_CFG *count, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_STATUS,(void *)count, flag ); 
}

uint32_t ifx_ppa_drv_get_multifield_max_entry( PPE_COUNT_CFG *count, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_GET_FLOW_MAX_ENTRY,(void *)count, flag ); 
}

uint32_t ifx_ppa_drv_add_multifield_entry(PPE_MULTIFILED_FLOW *flow, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_ADD_FLOW,(void *)flow, flag ); 
}

uint32_t ifx_ppa_drv_get_multifield_entry( PPE_MULTIFILED_FLOW *flow, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_GET_FLOW,(void *)flow, flag ); 
}

uint32_t ifx_ppa_drv_del_multifield_entry(PPE_MULTIFILED_FLOW *flow, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_DEL_FLOW,(void *)flow, flag ); 
}

uint32_t ifx_ppa_drv_del_multifield_entry_via_index(PPE_MULTIFILED_FLOW *flow, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_MFE_DEL_FLOW_VIA_ENTRY,(void *)flow, flag ); 
}
//#endif //end of CONFIG_IFX_PPA_MFE

uint32_t ifx_ppa_drv_test_and_clear_hit_stat(PPE_ROUTING_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_TEST_CLEAR_ROUTE_HIT_STAT,(void *)entry, flag ); 
}

uint32_t ifx_ppa_drv_test_and_clear_bridging_hit_stat(PPE_BR_MAC_INFO *entry, uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_TEST_CLEAR_BR_HIT_STAT,(void *)entry, flag ); 
}

uint32_t ifx_ppa_drv_get_max_vfilter_entries(PPE_VFILTER_COUNT_CFG *count, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_MAX_VFILTER_ENTRY_NUM,(void *)count, flag ); 
}

//#ifdef CONFIG_IFX_PPA_QOS
uint32_t ifx_ppa_drv_get_qos_qnum( PPE_QOS_COUNT_CFG *count, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_QUEUE_NUM, (void *)count, flag ); 
}

uint32_t ifx_ppa_drv_get_qos_status( PPA_QOS_STATUS *status, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_STATUS, (void *)status, flag ); 
}
  
  
uint32_t ifx_ppa_drv_get_qos_mib( PPE_QOS_MIB_INFO *mib, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_MIB, (void *)mib, flag ); 
}

//#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
uint32_t ifx_ppa_drv_set_ctrl_qos_rate(PPE_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CTRL, (void *)enable_cfg, flag ); 
}
uint32_t ifx_ppa_drv_get_ctrl_qos_rate(PPE_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CTRL, (void *)enable_cfg, flag ); 
}

uint32_t ifx_ppa_drv_set_qos_rate( PPE_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag )
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag ); 
}

uint32_t ifx_ppa_drv_get_qos_rate( PPE_QOS_RATE_SHAPING_CFG *cfg, uint32_t flag )
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag ); 
}

uint32_t ifx_ppa_drv_reset_qos_rate( PPE_QOS_RATE_SHAPING_CFG *cfg , uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG, (void *)cfg, flag ); 
}

uint32_t ifx_ppa_drv_init_qos_rate(uint32_t flag)
{
    if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_INIT_QOS_RATE_SHAPING, (void *)NULL, flag ); 
}   
//#endif  //end of CONFIG_IFX_PPA_QOS_RATE_SHAPING

//#ifdef CONFIG_IFX_PPA_QOS_WFQ
uint32_t ifx_ppa_drv_set_ctrl_qos_wfq(PPE_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_QOS_WFQ_CTRL, (void *)enable_cfg, flag ); 
}

uint32_t ifx_ppa_drv_get_ctrl_qos_wfq(PPE_QOS_ENABLE_CFG *enable_cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_WFQ_CTRL, (void *)enable_cfg, flag ); 
}

uint32_t ifx_ppa_drv_set_qos_wfq( PPE_QOS_WFQ_CFG *cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_SET_QOS_WFQ_CFG, (void *)cfg, flag ); 
}
uint32_t ifx_ppa_drv_get_qos_wfq( PPE_QOS_WFQ_CFG *cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_GET_QOS_WFQ_CFG, (void *)cfg, flag ); 
}

uint32_t ifx_ppa_drv_reset_qos_wfq( PPE_QOS_WFQ_CFG *cfg, uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_RESET_QOS_WFQ_CFG, (void *)cfg, flag ); 
}
uint32_t ifx_ppa_drv_init_qos_wfq(uint32_t flag)
{
     if( !ifx_ppa_drv_hal_generic_hook ) return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_INIT_QOS_WFQ, (void *)NULL, flag ); 
}   
//#endif  //end of CONFIG_IFX_PPA_QOS_WFQ
//#endif  //end of CONFIG_IFX_PPA_QOS

EXPORT_SYMBOL( ifx_ppa_drv_get_ppe_hal_id);
EXPORT_SYMBOL(ifx_ppa_drv_get_max_entries );
EXPORT_SYMBOL( ifx_ppa_drv_update_routing_entry);
EXPORT_SYMBOL(ifx_ppa_drv_get_ports_mib );
EXPORT_SYMBOL( ifx_ppa_drv_get_max_vfilter_entries);
EXPORT_SYMBOL( ifx_ppa_drv_add_mac_entry);
EXPORT_SYMBOL( ifx_ppa_drv_get_dsl_mib);
EXPORT_SYMBOL( ifx_ppa_drv_del_mtu_entry);
EXPORT_SYMBOL( ifx_ppa_drv_del_bridging_entry);
EXPORT_SYMBOL(ifx_ppa_drv_del_outer_vlan_entry );
EXPORT_SYMBOL(ifx_ppa_drv_set_qos_rate );
EXPORT_SYMBOL(ifx_ppa_drv_del_vlan_map );
EXPORT_SYMBOL(ifx_ppa_drv_set_acc_mode );
EXPORT_SYMBOL( ifx_ppa_drv_del_pppoe_entry);
EXPORT_SYMBOL(ifx_ppa_drv_get_acc_mode );
EXPORT_SYMBOL( ifx_ppa_drv_del_mac_entry);
EXPORT_SYMBOL( ifx_ppa_drv_set_route_cfg);
EXPORT_SYMBOL( ifx_ppa_drv_set_bridge_if_vlan_config);
EXPORT_SYMBOL( ifx_ppa_drv_get_mixed_wan_vlan_id);
EXPORT_SYMBOL( ifx_ppa_drv_set_fast_mode);
EXPORT_SYMBOL( ifx_ppa_drv_add_mtu_entry);
EXPORT_SYMBOL( ifx_ppa_drv_add_vlan_map);
EXPORT_SYMBOL(ifx_ppa_drv_get_firmware_id );
EXPORT_SYMBOL(ifx_ppa_drv_del_all_vlan_map );
EXPORT_SYMBOL( ifx_ppa_drv_test_and_clear_hit_stat);
EXPORT_SYMBOL(ifx_ppa_drv_get_phys_port_info );
EXPORT_SYMBOL(ifx_ppa_drv_get_ctrl_qos_wfq );
EXPORT_SYMBOL(ifx_ppa_drv_del_wan_mc_entry );
EXPORT_SYMBOL( ifx_ppa_drv_hal_init);
EXPORT_SYMBOL( ifx_ppa_drv_add_wan_mc_entry);
EXPORT_SYMBOL( ifx_ppa_drv_add_pppoe_entry);
EXPORT_SYMBOL(ifx_ppa_drv_add_tunnel_entry);
EXPORT_SYMBOL(ifx_ppa_drv_del_tunnel_entry);
EXPORT_SYMBOL( ifx_ppa_drv_get_mtu_entry);
EXPORT_SYMBOL( ifx_ppa_drv_hal_exit);
EXPORT_SYMBOL( ifx_ppa_drv_get_itf_mib);
EXPORT_SYMBOL(ifx_ppa_drv_get_routing_entry_bytes );
EXPORT_SYMBOL( ifx_ppa_drv_set_bridging_cfg);
EXPORT_SYMBOL( ifx_ppa_drv_get_bridge_if_vlan_config);
EXPORT_SYMBOL( ifx_ppa_drv_get_ctrl_qos_rate);
EXPORT_SYMBOL( ifx_ppa_drv_del_routing_entry);
EXPORT_SYMBOL( ifx_ppa_drv_set_qos_wfq);
EXPORT_SYMBOL( ifx_ppa_drv_set_mixed_wan_vlan_id);
EXPORT_SYMBOL( ifx_ppa_drv_get_qos_mib);
EXPORT_SYMBOL(ifx_ppa_drv_update_wan_mc_entry );
EXPORT_SYMBOL( ifx_ppa_drv_get_qos_rate);
EXPORT_SYMBOL(ifx_ppa_drv_reset_qos_rate);
EXPORT_SYMBOL( ifx_ppa_drv_set_ctrl_qos_rate);
EXPORT_SYMBOL(ifx_ppa_drv_get_qos_status);
EXPORT_SYMBOL(ifx_ppa_drv_get_qos_qnum);
EXPORT_SYMBOL( ifx_ppa_drv_add_bridging_entry);
EXPORT_SYMBOL( ifx_ppa_drv_test_and_clear_bridging_hit_stat);
EXPORT_SYMBOL( ifx_ppa_drv_get_number_of_phys_port);
EXPORT_SYMBOL( ifx_ppa_drv_init_qos_rate);
EXPORT_SYMBOL( ifx_ppa_drv_init_qos_wfq);
EXPORT_SYMBOL(ifx_ppa_drv_set_default_dest_list);
EXPORT_SYMBOL( ifx_ppa_drv_get_vlan_map);
EXPORT_SYMBOL( ifx_ppa_drv_add_routing_entry);
EXPORT_SYMBOL( ifx_ppa_drv_add_outer_vlan_entry);
EXPORT_SYMBOL( ifx_ppa_drv_reset_qos_wfq);
EXPORT_SYMBOL( ifx_ppa_drv_get_qos_wfq);
EXPORT_SYMBOL( ifx_ppa_drv_set_ctrl_qos_wfq);
EXPORT_SYMBOL( ifx_ppa_drv_get_outer_vlan_entry);

