/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_directpath.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 19 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Directpath Functions
**                File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 19 NOV 2008  Xu Liang        Initiate Version
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
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_api_directpath.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppe_drv_wrapper.h"
#include "ifx_ppa_api_misc.h"


/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *              Data Type
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

static PPA_LOCK g_directpath_port_lock;
static uint32_t g_start_ifid = ~0, g_end_ifid = ~0;



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



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*
 *  PPA DirectPath Functions
 */

int32_t ifx_ppa_directpath_register_dev(uint32_t *p_if_id, PPA_NETIF *netif, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags)
{
    int32_t ret;
    uint32_t if_id;
    uint8_t mac[PPA_ETH_ALEN] = {0};
    uint32_t tmp_flags;
    PPA_BUF *skb_list, *skb, *skb_list_bak;
    unsigned long sys_flag;

    if ( p_if_id == NULL || !ifx_ppa_drv_g_ppe_directpath_data  )
        return IFX_EINVAL;

    if ( (flags & PPA_F_DIRECTPATH_REGISTER) )
    {
        if ( !(flags & PPA_F_DIRECTPATH_ETH_IF) )
            //  currently, only ethernet interface is supported
            return IFX_EINVAL;

        if ( !netif || !pDirectpathCb
            || !pDirectpathCb->rx_fn )
            return IFX_EINVAL;

        if ( ppa_get_netif_hwaddr(netif, mac) != IFX_SUCCESS )
        {
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_get_netif_hwaddr fail \n");
            return IFX_EINVAL;
        }

        if ( ppa_get_netif_name(netif) == NULL )
            return IFX_EINVAL;

        tmp_flags = 0;
        if ( (flags & PPA_F_DIRECTPATH_CORE1) )
            tmp_flags |= PPE_DIRECTPATH_CORE1;
        else
            tmp_flags |= PPE_DIRECTPATH_CORE0;
        if ( (flags & PPA_F_DIRECTPATH_ETH_IF) )
            tmp_flags |= PPE_DIRECTPATH_ETH;

        ppa_lock_get(&g_directpath_port_lock);
        for ( if_id = 0;
              if_id < g_end_ifid - g_start_ifid && (ifx_ppa_drv_g_ppe_directpath_data[if_id].flags & (PPE_DIRECTPATH_DATA_ENTRY_VALID | PPE_DIRECTPATH_ITF_TYPE_MASK | tmp_flags)) != tmp_flags;
              if_id++ )
            {
                ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "direct_flag[%d]=%x\n", if_id, ifx_ppa_drv_g_ppe_directpath_data[if_id].flags );
            }
        if ( if_id < g_end_ifid - g_start_ifid
            && ppa_phys_port_add(ppa_get_netif_name(netif), if_id + g_start_ifid) == IFX_SUCCESS )
        {
            tmp_flags = (ifx_ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_MASK) | PPE_DIRECTPATH_DATA_ENTRY_VALID | PPE_DIRECTPATH_DATA_RX_ENABLE;
            ppa_memset(&ifx_ppa_drv_g_ppe_directpath_data[if_id], 0, sizeof(ifx_ppa_drv_g_ppe_directpath_data[if_id]));
            ifx_ppa_drv_g_ppe_directpath_data[if_id].callback   = *pDirectpathCb;
            ifx_ppa_drv_g_ppe_directpath_data[if_id].netif      = netif;
            ifx_ppa_drv_g_ppe_directpath_data[if_id].ifid       = if_id + g_start_ifid;
            ppa_memcpy(ifx_ppa_drv_g_ppe_directpath_data[if_id].mac, mac, PPA_ETH_ALEN);
            ifx_ppa_drv_g_ppe_directpath_data[if_id].flags = tmp_flags;
            *p_if_id = if_id + g_start_ifid;
            ret = IFX_SUCCESS;
        }
        else
        {
            ret = IFX_FAILURE;
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "direct register fail\n" );
        }
        ppa_lock_release(&g_directpath_port_lock);

        return ret;
    }
    else
    {
        if ( *p_if_id < g_start_ifid || *p_if_id >= g_end_ifid )
        {
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"directp unregister wrong id: %d\n", *p_if_id);
            return IFX_EINVAL;
        }

        if_id = *p_if_id - g_start_ifid;
        ppa_lock_get(&g_directpath_port_lock);
        if ( (ifx_ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
        {
            ppa_phys_port_remove(if_id + g_start_ifid);
            sys_flag = ppa_disable_int();
            tmp_flags = ifx_ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_MASK;
            skb_list = ifx_ppa_drv_g_ppe_directpath_data[if_id].skb_list;
            ppa_memset(ifx_ppa_drv_g_ppe_directpath_data + if_id, 0, sizeof(*ifx_ppa_drv_g_ppe_directpath_data));
            ifx_ppa_drv_g_ppe_directpath_data[if_id].flags = tmp_flags;
            ppa_enable_int(sys_flag);
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"directp unregiter ok and restore direct_flag[%d]=%x\n", if_id, ifx_ppa_drv_g_ppe_directpath_data[if_id].flags );
        }
        else
            skb_list = NULL;
        ppa_lock_release(&g_directpath_port_lock);

        if ( skb_list )
        {
            skb_list_bak = skb_list;
            do
            {
                skb = skb_list;
                skb_list = ppa_buf_get_next(skb_list);
                ppa_buf_free(skb);
            } while ( skb_list != NULL && skb_list != skb_list_bak );
        }
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"unregiter directpath ok\n");
        return IFX_SUCCESS;
    }
}

int32_t ifx_ppa_directpath_send(uint32_t rx_if_id, PPA_BUF *skb, int32_t len, uint32_t flags)
{
#if !defined(CONFIG_IFX_PPA_API_DIRECTPATH_BRIDGING)
    uint8_t mac[PPA_ETH_ALEN] = {0};
#endif

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;

    if ( rx_if_id < g_start_ifid || rx_if_id >= g_end_ifid || skb == NULL  )
        return IFX_EINVAL;

    if ( !(ifx_ppa_drv_g_ppe_directpath_data[rx_if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
        return IFX_EPERM;

#if !defined(CONFIG_IFX_PPA_API_DIRECTPATH_BRIDGING)
    ppa_get_pkt_rx_dst_mac_addr(skb, mac);
    if ( ppa_memcmp(mac, ifx_ppa_drv_g_ppe_directpath_data[rx_if_id - g_start_ifid].mac, PPA_ETH_ALEN) != 0 )
    {
        //  bridge
        rx_if_id -= g_start_ifid;
        if ( (ifx_ppa_drv_g_ppe_directpath_data[rx_if_id].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
        {
            ifx_ppa_drv_g_ppe_directpath_data[rx_if_id].callback.rx_fn(ifx_ppa_drv_g_ppe_directpath_data[rx_if_id].netif, NULL, skb, len);
            return IFX_SUCCESS;
        }
        else
            return IFX_EAGAIN;
    }
#endif

#if defined(CONFIG_IFX_PPA_API_DIRECTPATH_TASKLET)
  #warning CONFIG_IFX_PPA_API_DIRECTPATH_TASKLET not implemented yet
    return IFX_ENOTIMPL;
#else
    return ifx_ppa_drv_directpath_send(rx_if_id, skb, len, flags);
#endif
}

int32_t ifx_ppa_directpath_rx_stop(uint32_t if_id, uint32_t flags)
{
    int32_t ret;

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;

    ppa_lock_get(&g_directpath_port_lock);
    if ( if_id >= g_start_ifid && if_id < g_end_ifid
        && (ifx_ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ifx_ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags &= ~PPE_DIRECTPATH_DATA_RX_ENABLE;
       ret = ifx_ppa_drv_directpath_rx_stop(if_id, flags) == 0 ? IFX_SUCCESS : IFX_FAILURE;
    }
    else
        ret = IFX_EINVAL;
    ppa_lock_release(&g_directpath_port_lock);

    return ret;
}

int32_t ifx_ppa_directpath_rx_restart(uint32_t if_id, uint32_t flags)
{
    int32_t ret;

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;
    
    ppa_lock_get(&g_directpath_port_lock);
    if ( if_id >= g_start_ifid && if_id < g_end_ifid
        && (ifx_ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        ifx_ppa_drv_g_ppe_directpath_data[if_id - g_start_ifid].flags |= PPE_DIRECTPATH_DATA_RX_ENABLE;
        ret = ifx_ppa_drv_directpath_rx_start(if_id, flags) == 0 ? IFX_SUCCESS : IFX_FAILURE;
  
    }
    else
        ret = IFX_EINVAL;
    ppa_lock_release(&g_directpath_port_lock);

    return ret;
}

PPA_NETIF *ifx_ppa_get_netif_for_ppa_ifid(uint32_t if_id)
{
    PPA_NETIF *ret = NULL;

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return NULL;

    ppa_lock_get(&g_directpath_port_lock);
    if ( if_id >= g_start_ifid && if_id < g_end_ifid )
    {
        if_id -= g_start_ifid;
        if ( (ifx_ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
            ret = ifx_ppa_drv_g_ppe_directpath_data[if_id].netif;
    }
    ppa_lock_release(&g_directpath_port_lock);

    return ret;
}

int32_t ifx_ppa_get_ifid_for_netif(PPA_NETIF *netif)
{
    uint32_t if_id;

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;
    
    if ( netif == NULL )
        return IFX_EINVAL;

    ppa_lock_get(&g_directpath_port_lock);
    for ( if_id = 0; if_id < g_end_ifid - g_start_ifid; if_id++ )
        if ( (ifx_ppa_drv_g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID)
            && ppa_is_netif_equal(netif, ifx_ppa_drv_g_ppe_directpath_data[if_id].netif) )
        {
            ppa_lock_release(&g_directpath_port_lock);
            return if_id + g_start_ifid;
        }
    ppa_lock_release(&g_directpath_port_lock);

    return IFX_FAILURE;
}

int32_t ppa_directpath_data_start_iteration(uint32_t *ppos, struct ppe_directpath_data **info)
{
    uint32_t l;

    if ( g_start_ifid == ~0 )
    {
        *info = NULL;
        return IFX_FAILURE;
    }

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;

    ppa_lock_get(&g_directpath_port_lock);

    l = *ppos;
    if ( l + g_start_ifid < g_end_ifid )
    {
        ++*ppos;
        *info = ifx_ppa_drv_g_ppe_directpath_data + l;
        return IFX_SUCCESS;
    }
    else
    {
        *info = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_directpath_data_iterate_next(uint32_t *ppos, struct ppe_directpath_data **info)
{
    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;
    
    if ( *ppos + g_start_ifid < g_end_ifid )
    {
        *info = ifx_ppa_drv_g_ppe_directpath_data + *ppos;
        ++*ppos;
        return IFX_SUCCESS;
    }
    else
    {
        *info = NULL;
        return IFX_FAILURE;
    }
}

void ppa_directpath_data_stop_iteration(void)
{
    ppa_lock_release(&g_directpath_port_lock);
}

void ppa_directpath_get_ifid_range(uint32_t *p_start_ifid, uint32_t *p_end_ifid)
{
    if ( p_start_ifid )
        *p_start_ifid = g_start_ifid;
    if ( p_end_ifid )
        *p_end_ifid = g_end_ifid;
}



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

int32_t ppa_api_directpath_init(void)
{
    uint32_t i;
    uint32_t last_valid_ifid;
    uint32_t tmp_flags;
    PPE_IFINFO if_info;
    PPE_COUNT_CFG count={0};

    if( !ifx_ppa_drv_g_ppe_directpath_data ) return IFX_EINVAL;
    if ( ppa_lock_init(&g_directpath_port_lock) )
    {
        err("Failed in creating lock for directpath port.");
        return IFX_EIO;
    }

    ifx_ppa_drv_get_number_of_phys_port(&count, 0);
    last_valid_ifid = count.num- 1;
    ppa_memset( &if_info, 0, sizeof(if_info) );
    for ( i = 0; i < count.num; i++ )
    {
        if_info.port = i;
        ifx_ppa_drv_get_phys_port_info( &if_info, 0);
        if ( (if_info.if_flags & (IFX_PPA_PHYS_PORT_FLAGS_VALID | IFX_PPA_PHYS_PORT_FLAGS_TYPE_MASK)) == (IFX_PPA_PHYS_PORT_FLAGS_VALID | IFX_PPA_PHYS_PORT_FLAGS_TYPE_EXT)
            && (if_info.if_flags  & (IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU0 | IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU1)) != 0 )
        {
            if ( g_start_ifid == ~0 )
                g_start_ifid = i;
            last_valid_ifid = i;
            tmp_flags = PPE_DIRECTPATH_ETH;
            if ( (if_info.if_flags  & IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU0) )
                tmp_flags |= PPE_DIRECTPATH_CORE0;
            if ( (if_info.if_flags  & IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU1) )
                tmp_flags |= PPE_DIRECTPATH_CORE1;
            ifx_ppa_drv_g_ppe_directpath_data[i - g_start_ifid].flags = tmp_flags;
        }
    }
    g_end_ifid = last_valid_ifid + 1;

    return IFX_SUCCESS;
}

void ppa_api_directpath_exit(void)
{
    uint32_t i;

    for ( i = 0; i < g_end_ifid - g_start_ifid; i++ )
        if( ifx_ppa_drv_g_ppe_directpath_data ) ifx_ppa_drv_g_ppe_directpath_data[i].flags = 0;

    g_start_ifid = g_end_ifid = ~0;

    ppa_lock_destroy(&g_directpath_port_lock);
}

EXPORT_SYMBOL(ppa_directpath_data_start_iteration);
EXPORT_SYMBOL(ppa_directpath_data_iterate_next);
EXPORT_SYMBOL(ppa_directpath_data_stop_iteration);
EXPORT_SYMBOL(ppa_directpath_get_ifid_range);
