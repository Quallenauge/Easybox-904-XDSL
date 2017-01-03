/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_netif.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 3 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Network Interface Functions
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
#include "ifx_ppe_drv_wrapper.h"
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_netif.h"
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
 *  implemented in PPA PPE Low Level Driver (Data Path)
 */
//extern int get_dslwan_qid_with_vcc(PPA_VCC *vcc);



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

uint32_t g_phys_port_cpu = ~0;
uint32_t g_phys_port_atm_wan = ~0;
uint32_t g_phys_port_atm_wan_vlan = 0;
static struct phys_port_info *g_phys_port_info = NULL;
static PPA_LOCK g_phys_port_lock;

static struct netif_info *g_netif = NULL;
static PPA_LOCK g_netif_lock;



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
 *  Physical Network Interface Operation Function
 */
static INLINE struct phys_port_info *ppa_phys_port_alloc_item(void)
{
    struct phys_port_info *obj;

    obj = (struct phys_port_info *)ppa_malloc(sizeof(*obj));
    if ( obj )
        ppa_memset(obj, 0, sizeof(*obj));
    return obj;
}

static INLINE void ppa_phys_port_free_item(struct phys_port_info *obj)
{
    ppa_free(obj);
}

static void ppa_phys_port_free_list(void)
{
    struct phys_port_info *obj;

    ppa_lock_get(&g_phys_port_lock);
    while ( g_phys_port_info )
    {
        obj = g_phys_port_info;
        g_phys_port_info = g_phys_port_info->next;

        ppa_phys_port_free_item(obj);
    }
    g_phys_port_cpu = ~0;
    g_phys_port_atm_wan = ~0;
    ppa_lock_release(&g_phys_port_lock);
}

static int32_t ppa_phys_port_lookup(PPA_IFNAME ifname[PPA_IF_NAME_SIZE], struct phys_port_info **pp_info)
{
    int32_t ret = IFX_ENOTAVAIL;
    struct phys_port_info *obj;

    ppa_lock_get(&g_phys_port_lock);
    for ( obj = g_phys_port_info; obj; obj = obj->next )
        if ( strcmp(obj->ifname, ifname) == 0 )
        {
            ret = IFX_SUCCESS;
            if ( pp_info )
                *pp_info = obj;
            break;
        }
    ppa_lock_release(&g_phys_port_lock);

    return ret;
}

/*
 *  Network Interface Operation Functions
 */

static INLINE struct netif_info * ppa_netif_alloc_item(void)    //  alloc_netif_info
{
    struct netif_info *obj;

    obj = (struct netif_info *)ppa_malloc(sizeof(*obj));
    if ( obj )
    {
        ppa_memset(obj, 0, sizeof(*obj));
        obj->mac_entry = ~0;
        ppa_atomic_set(&obj->count, 1);
    }
    return obj;
}

static INLINE void ppa_netif_free_item(struct netif_info *obj)  //  free_netif_info
{
    if ( ppa_atomic_dec(&obj->count) == 0 )
    {
        PPE_ROUTE_MAC_INFO mac_info = {0};
        mac_info.mac_ix = obj->mac_entry;
        ifx_ppa_drv_del_mac_entry( &mac_info, 0);
        ppa_free(obj);
    }
}

static INLINE void ppa_netif_lock_list(void)    //  lock_netif_info_list
{
    ppa_lock_get(&g_netif_lock);
}

static INLINE void ppa_netif_unlock_list(void)  //  unlock_netif_info_list
{
    ppa_lock_release(&g_netif_lock);
}

static INLINE void ppa_netif_add_item(struct netif_info *obj)   //  add_netif_info
{
    ppa_atomic_inc(&obj->count);
    ppa_netif_lock_list();
    obj->next = g_netif;
    g_netif = obj;
    ppa_netif_unlock_list();
}

static INLINE void ppa_netif_remove_item(PPA_IFNAME ifname[PPA_IF_NAME_SIZE], struct netif_info **pp_info)  //  remove_netif_info
{
    struct netif_info *p_prev, *p_cur;

    if ( pp_info )
        *pp_info = NULL;
    p_prev = NULL;
    ppa_netif_lock_list();
    for ( p_cur = g_netif; p_cur; p_prev = p_cur, p_cur = p_cur->next )
        if ( strcmp(p_cur->name, ifname) == 0 )
        {
            if ( !p_prev )
                g_netif = p_cur->next;
            else
                p_prev->next = p_cur->next;
            if ( pp_info )
                *pp_info = p_cur;
            else
                ppa_netif_free_item(p_cur);
            break;
        }
    ppa_netif_unlock_list();
}

static void ppa_netif_free_list(void)    //  free_netif_info_list
{
    struct netif_info *obj;

    ppa_netif_lock_list();
    while ( g_netif )
    {
        obj = g_netif;
        g_netif = g_netif->next;

        ppa_netif_free_item(obj);
    }
    ppa_netif_unlock_list();
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

int32_t ppa_phys_port_add(PPA_IFNAME *ifname, uint32_t port)
{
    struct phys_port_info *obj;
    uint32_t mode = 0;
    uint32_t type = 0;
    uint32_t vlan = 0;
    PPE_IFINFO if_info;

    if ( !ifname )
        return IFX_EINVAL;

    ppa_memset( &if_info, 0, sizeof(if_info) );
    if_info.port = port;
    ifx_ppa_drv_get_phys_port_info( &if_info, 0);
    if ( (if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_VALID) )
    {
        switch ( if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_MODE_MASK )
        {
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_LAN: mode = 1; break;
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_WAN: mode = 2; break;
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_MIX: mode = 3;
        }
        switch ( if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_TYPE_MASK )
        {
        case IFX_PPA_PHYS_PORT_FLAGS_TYPE_CPU: type = 0; break;
        case IFX_PPA_PHYS_PORT_FLAGS_TYPE_ATM: type = 1; break;
        case IFX_PPA_PHYS_PORT_FLAGS_TYPE_ETH: type = 2; break;
        case IFX_PPA_PHYS_PORT_FLAGS_TYPE_EXT: type = 3;
        }
        vlan = (if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_OUTER_VLAN) ? 2 : 1;
    }

    ppa_lock_get(&g_phys_port_lock);
    for ( obj = g_phys_port_info; obj; obj = obj->next )
        if ( obj->port == port )
        {
            strcpy(obj->ifname, ifname);
            obj->mode   = mode;
            obj->type   = type;
            obj->vlan   = vlan;
        }
    ppa_lock_release(&g_phys_port_lock);

    if ( !obj )
    {
        obj = ppa_phys_port_alloc_item();
        if ( !obj )
            return IFX_ENOMEM;
        strcpy(obj->ifname, ifname);
        obj->mode   = mode;
        obj->type   = type;
        obj->port   = port;
        obj->vlan   = vlan;
        ppa_lock_get(&g_phys_port_lock);
        obj->next = g_phys_port_info;
        g_phys_port_info = obj;
        ppa_lock_release(&g_phys_port_lock);
    }

    return IFX_SUCCESS;
}

void ppa_phys_port_remove(uint32_t port)
{
    struct phys_port_info *p_prev, *p_cur;

    p_prev = NULL;
    ppa_lock_get(&g_phys_port_lock);
    for ( p_cur = g_phys_port_info; p_cur; p_prev = p_cur, p_cur = p_cur->next )
        if ( p_cur->port == port )
        {
            if ( !p_prev )
                g_phys_port_info = p_cur->next;
            else
                p_prev->next = p_cur->next;
            ppa_lock_release(&g_phys_port_lock);
            ppa_phys_port_free_item(p_cur);
            return;
        }
    ppa_lock_release(&g_phys_port_lock);
}

int32_t ppa_phys_port_get_first_eth_lan_port(uint32_t *ifid, PPA_IFNAME **ifname)
{
    int32_t ret;
    struct phys_port_info *obj;

    if ( !ifid || !ifname )
        return IFX_EINVAL;

    ret = IFX_ENOTAVAIL;

    ppa_lock_get(&g_phys_port_lock);
    for ( obj = g_phys_port_info; obj; obj = obj->next )
        if ( obj->mode == 1 && obj->type == 2 ) //  LAN side ETH interface
        {
            *ifid = obj->port;
            *ifname = &(obj->ifname[0]);
            ret = IFX_SUCCESS;
            break;
        }
    ppa_lock_release(&g_phys_port_lock);

    return ret;
}

int32_t ppa_phys_port_start_iteration(uint32_t *ppos, struct phys_port_info **ifinfo)
{
    uint32_t l;
    struct phys_port_info *p;

    ppa_lock_get(&g_phys_port_lock);

    for ( p = g_phys_port_info, l = *ppos; p && l; p = p->next, l-- );

    if ( l == 0 && p )
    {
        ++*ppos;
        *ifinfo = p;
        return IFX_SUCCESS;
    }
    else
    {
        *ifinfo = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_phys_port_iterate_next(uint32_t *ppos, struct phys_port_info **ifinfo)
{
    if ( *ifinfo )
    {
        ++*ppos;
        *ifinfo = (*ifinfo)->next;
        return *ifinfo ? IFX_SUCCESS : IFX_FAILURE;
    }
    else
        return IFX_FAILURE;
}

void ppa_phys_port_stop_iteration(void)
{
    ppa_lock_release(&g_phys_port_lock);
}

int32_t ppa_netif_add(PPA_IFNAME *ifname, int f_is_lan, struct netif_info **pp_ifinfo)
{
    struct netif_info *p_ifinfo;
    PPA_NETIF *netif, *netif_tmp;
    PPA_VCC *vcc;
    int32_t dslwan_qid;
    PPA_IFNAME underlying_ifname[PPA_IF_NAME_SIZE];
    uint32_t vid[2];
    struct phys_port_info *p_phys_port = NULL;
    #define DEFAULT_OUTER_VLAN_ID 0x81000000
    PPA_NETIF *tmp_vlan_netif[2]={NULL};

    if ( ppa_netif_lookup(ifname, &p_ifinfo) != IFX_SUCCESS )
    {
        p_ifinfo = ppa_netif_alloc_item();
        if ( !p_ifinfo )
            return IFX_ENOMEM;

        strcpy(p_ifinfo->name, ifname);

        netif = ppa_get_netif(ifname);
        if ( netif )
        {
            p_ifinfo->netif = netif;

            if(netif->type == ARPHRD_SIT){
                p_ifinfo->flags |= NETIF_PHY_TUNNEL;
                if(ppa_get_6rd_phyif_fn == NULL || (netif_tmp = ppa_get_6rd_phyif_fn(netif)) == NULL){
                    printk("%s: Add sit device: %s, but cannot find the physical device\n", __FUNCTION__, netif->name);
                }else{
                    dbg("sit device:%s, physical device:%s\n", netif->name, netif_tmp->name);
                    netif = netif_tmp;
                }
            }
            
            if ( ppa_get_physical_if(netif, NULL, p_ifinfo->phys_netif_name) == IFX_SUCCESS )
                p_ifinfo->flags |= NETIF_PHY_IF_GOT;

            if ( ppa_if_is_ipoa(netif, NULL) && ppa_br2684_get_vcc(netif, &vcc) == IFX_SUCCESS && (dslwan_qid = ifx_ppa_drv_get_dslwan_qid_with_vcc(vcc)) >= 0 )
            {
                p_ifinfo->flags |= NETIF_PHY_ATM | NETIF_BR2684 | NETIF_IPOA;
                p_ifinfo->vcc = vcc;
                p_ifinfo->dslwan_qid = dslwan_qid;
                goto PPA_NETIF_ADD_ATM_BASED_NETIF_DONE;
            }
            else if ( ppa_check_is_ppp_netif(netif) )
            {
                if ( ppa_check_is_pppoe_netif(netif) && ppa_pppoe_get_physical_if(netif, NULL, underlying_ifname) == IFX_SUCCESS )
                {
                    p_ifinfo->pppoe_session_id = ppa_pppoe_get_pppoe_session_id(netif);
                    if ( ppa_get_netif_hwaddr(netif, p_ifinfo->mac) == IFX_SUCCESS )
                        p_ifinfo->flags |= NETIF_PPPOE | NETIF_MAC_AVAILABLE;
                    else
                        p_ifinfo->flags |= NETIF_PPPOE;
                    netif = ppa_get_netif(underlying_ifname);
                }
                else if ( ppa_if_is_pppoa(netif, NULL) && ppa_pppoa_get_vcc(netif, &vcc) == IFX_SUCCESS && (dslwan_qid = ifx_ppa_drv_get_dslwan_qid_with_vcc(vcc)) >= 0 )
                {
                    p_ifinfo->flags |= NETIF_PHY_ATM | NETIF_PPPOATM;
                    p_ifinfo->vcc = vcc;
                    p_ifinfo->dslwan_qid = dslwan_qid;
                    goto PPA_NETIF_ADD_ATM_BASED_NETIF_DONE;
                }
            }
            else if ( ppa_get_netif_hwaddr(netif, p_ifinfo->mac) == IFX_SUCCESS
                && (p_ifinfo->mac[0] | p_ifinfo->mac[1] | p_ifinfo->mac[2] | p_ifinfo->mac[3] | p_ifinfo->mac[4] | p_ifinfo->mac[5]) != 0 )
                p_ifinfo->flags |= NETIF_MAC_AVAILABLE;

            while ( netif && ppa_if_is_vlan_if(netif, NULL) && ppa_vlan_get_underlying_if(netif, NULL, underlying_ifname) == IFX_SUCCESS )
            {
                PPA_NETIF *new_netif;
                p_ifinfo->flags |= NETIF_VLAN;
                if ( p_ifinfo->vlan_layer < NUM_ENTITY(vid) )
                {
                    vid[p_ifinfo->vlan_layer] = ppa_get_vlan_id(netif);
                    tmp_vlan_netif[p_ifinfo->vlan_layer]=netif;
                }
                p_ifinfo->vlan_layer++;
                new_netif = ppa_get_netif(underlying_ifname);
				if (new_netif == netif) {
					/* VLAN interface and underlying interface share the same name! Break the loop */
					break;
				}
				netif=new_netif;
            }

            if ( netif )
            {
                if ( ppa_if_is_br_if(netif, NULL) )
                    p_ifinfo->flags |= NETIF_BRIDGE;
                else if ( ppa_if_is_br2684(netif, NULL) && ppa_br2684_get_vcc(netif, &vcc) == IFX_SUCCESS && (dslwan_qid = ifx_ppa_drv_get_dslwan_qid_with_vcc(vcc)) >= 0 )
                {
                    p_ifinfo->flags |= NETIF_PHY_ATM | NETIF_BR2684 | NETIF_EOA;
                    p_ifinfo->vcc = vcc;
                    p_ifinfo->dslwan_qid = dslwan_qid;
                }
                else
                    p_ifinfo->flags |= NETIF_PHY_ETH;
            }
        }

PPA_NETIF_ADD_ATM_BASED_NETIF_DONE:
        if ( (p_ifinfo->flags & NETIF_PHY_ATM) )
        {
            //  ATM port is WAN port only
            if ( !f_is_lan )
            {
                if ( p_ifinfo->vlan_layer > 0 )
                {
                    if ( p_ifinfo->vlan_layer <= g_phys_port_atm_wan_vlan )
                    {
                        if ( p_ifinfo->vlan_layer == 2 )
                        {
                            p_ifinfo->inner_vid = vid[0];
                            p_ifinfo->outer_vid = DEFAULT_OUTER_VLAN_ID | vid[1];
                            p_ifinfo->flags |= NETIF_VLAN_INNER | NETIF_VLAN_OUTER;
                            p_ifinfo->in_vlan_netif = tmp_vlan_netif[0];
                            p_ifinfo->out_vlan_netif = tmp_vlan_netif[1];
                        }
                        else if ( p_ifinfo->vlan_layer == 1 )
                        {
                            if ( g_phys_port_atm_wan_vlan == 2 )
                            {
                                p_ifinfo->outer_vid = DEFAULT_OUTER_VLAN_ID | vid[0];
                                p_ifinfo->flags |= NETIF_VLAN_OUTER;
                                p_ifinfo->out_vlan_netif = tmp_vlan_netif[0];
                            }
                            else if ( g_phys_port_atm_wan_vlan == 1 )
                            {
                                p_ifinfo->inner_vid = vid[0];
                                p_ifinfo->flags |= NETIF_VLAN_INNER;
                                p_ifinfo->in_vlan_netif = tmp_vlan_netif[0];
                            }
                        }
                    }
                    else
                        p_ifinfo->flags |= NETIF_VLAN_CANT_SUPPORT;
                }
                p_ifinfo->phys_port = g_phys_port_atm_wan;
                p_ifinfo->flags |= NETIF_PHYS_PORT_GOT;
            }
        }
        else if ( (p_ifinfo->flags & NETIF_PHY_IF_GOT) && !(p_ifinfo->flags & NETIF_BRIDGE) )
        {
            if ( ppa_phys_port_lookup(p_ifinfo->phys_netif_name, &p_phys_port) == IFX_SUCCESS )
            {
                if ( p_ifinfo->vlan_layer > 0 )
                {
                    if ( p_ifinfo->vlan_layer <= p_phys_port->vlan )
                    {
                        if ( p_ifinfo->vlan_layer == 2 )
                        {
                            p_ifinfo->inner_vid = vid[0];
                            p_ifinfo->outer_vid = DEFAULT_OUTER_VLAN_ID | vid[1];
                            p_ifinfo->flags |= NETIF_VLAN_INNER | NETIF_VLAN_OUTER;
                            p_ifinfo->in_vlan_netif = tmp_vlan_netif[0];
                            p_ifinfo->out_vlan_netif = tmp_vlan_netif[1];
                        }
                        else if ( p_ifinfo->vlan_layer == 1 )
                        {
                            if ( p_phys_port->vlan == 2 )
                            {
                                p_ifinfo->outer_vid = DEFAULT_OUTER_VLAN_ID | vid[0];
                                p_ifinfo->flags |= NETIF_VLAN_OUTER;
                                p_ifinfo->out_vlan_netif = tmp_vlan_netif[0];
                            }
                            else if ( p_phys_port->vlan == 1 )
                            {
                                p_ifinfo->inner_vid = vid[0];
                                p_ifinfo->flags |= NETIF_VLAN_INNER;
                                p_ifinfo->in_vlan_netif = tmp_vlan_netif[0];                                
                            }
                        }
                    }
                    else
                        p_ifinfo->flags |= NETIF_VLAN_CANT_SUPPORT;
                }
                p_ifinfo->phys_port = p_phys_port->port;
                p_ifinfo->flags |= NETIF_PHYS_PORT_GOT;
            }
        }

        if ( (p_ifinfo->flags & NETIF_MAC_AVAILABLE) )
        {
            PPE_ROUTE_MAC_INFO mac_info={0};
            ppa_memcpy(mac_info.mac, p_ifinfo->mac, sizeof(mac_info.mac));
            if ( ifx_ppa_drv_add_mac_entry( &mac_info, 0) == IFX_SUCCESS )
            {
                p_ifinfo->mac_entry = mac_info.mac_ix;
                p_ifinfo->flags |= NETIF_MAC_ENTRY_CREATED;
            }
            else
                err("add_mac_entry failed");
        }

        ppa_netif_add_item(p_ifinfo);
    }

    p_ifinfo->flags |= f_is_lan ? NETIF_LAN_IF : NETIF_WAN_IF;
    if ( pp_ifinfo )
        *pp_ifinfo = p_ifinfo;
    else
        ppa_netif_free_item(p_ifinfo);

    return IFX_SUCCESS;
}

void ppa_netif_remove(PPA_IFNAME *ifname, int f_is_lan)
{
    struct netif_info *p_ifinfo;

    if ( ppa_netif_lookup(ifname, &p_ifinfo) == IFX_SUCCESS )
    {
        p_ifinfo->flags &= f_is_lan ? ~NETIF_LAN_IF : ~NETIF_WAN_IF;
        if ( !(p_ifinfo->flags & (NETIF_LAN_IF | NETIF_WAN_IF)) )
        {
            ppa_netif_remove_item(ifname, NULL);
            if ( (p_ifinfo->flags & NETIF_MAC_ENTRY_CREATED) )
            {
                PPE_ROUTE_MAC_INFO mac_info={0};
                mac_info.mac_ix = p_ifinfo->mac_entry;
                ifx_ppa_drv_del_mac_entry(&mac_info, 0);
                p_ifinfo->mac_entry = ~0;
                p_ifinfo->flags &= ~NETIF_MAC_ENTRY_CREATED;
            }
        }

        ppa_netif_free_item(p_ifinfo);
    }
}

int32_t ppa_netif_lookup(PPA_IFNAME *ifname, struct netif_info **pp_info)    //  netif_info_is_added
{
    int32_t ret = IFX_ENOTAVAIL;
    struct netif_info *p;

    ppa_netif_lock_list();
    for ( p = g_netif; p; p = p->next )
        if ( strcmp(p->name, ifname) == 0 )
        {
            ret = IFX_SUCCESS;
            if ( pp_info )
            {
                ppa_atomic_inc(&p->count);
                *pp_info = p;
            }
            break;
        }
    ppa_netif_unlock_list();

    return ret;
}

void ppa_netif_put(struct netif_info *p_info)
{
    ppa_netif_free_item(p_info);
}

int32_t ppa_netif_update(PPA_NETIF *netif, PPA_IFNAME *ifname)
{
    struct netif_info *p_info;
    int f_need_update = 0;
    uint32_t flags;    
    PPE_ROUTE_MAC_INFO mac_info={0};

    if ( netif )
        ifname = ppa_get_netif_name(netif);
    else
        netif = ppa_get_netif(ifname);

    if ( !netif || !ifname ){
        dbg("%s fail: cannot find device\n", __FUNCTION__);
        return IFX_EINVAL;
    }

    if ( ppa_netif_lookup(ifname, &p_info) != IFX_SUCCESS ){
        dbg("%s fail: device: %s not accelerated\n", __FUNCTION__, ifname);
        return IFX_ENOTAVAIL;
    }

    flags = p_info->flags;

    if ( !ppa_is_netif_equal(netif, p_info->netif) )
        f_need_update = 1;
    else if ( (flags & NETIF_PHYS_PORT_GOT) == 0 && (flags & (NETIF_BRIDGE | NETIF_PHY_IF_GOT)) != (NETIF_BRIDGE | NETIF_PHY_IF_GOT) )
        f_need_update = 1;
    else if ( (flags & NETIF_PPPOE) )
    {
        if ( !ppa_check_is_pppoe_netif(netif) )
            f_need_update = 1;
        else
            p_info->pppoe_session_id = ppa_pppoe_get_pppoe_session_id(netif);
    }
    else if ( (flags & NETIF_PPPOATM) )
    {
        if ( !ppa_if_is_pppoa(netif, NULL) )
            f_need_update = 1;
    }

    if ( !f_need_update && (flags & (NETIF_BRIDGE | NETIF_PHY_ETH | NETIF_EOA)) != 0 )
    {
        //  update MAC address
        if ( ppa_get_netif_hwaddr(netif, mac_info.mac) == IFX_SUCCESS
            && (mac_info.mac[0] | mac_info.mac[1] | mac_info.mac[2] | mac_info.mac[3] | mac_info.mac[4] | mac_info.mac[5]) != 0 )
        {
            if ( ppa_memcmp(p_info->mac, mac_info.mac, PPA_ETH_ALEN) != 0 )
            {                
                mac_info.mac_ix = p_info->mac_entry;
                ifx_ppa_drv_del_mac_entry( &mac_info, 0);
                if ( ifx_ppa_drv_add_mac_entry( &mac_info, 0) == IFX_SUCCESS )
                {
                    p_info->mac_entry = mac_info.mac_ix;
                    p_info->flags |= NETIF_MAC_ENTRY_CREATED;
                }
                else
                {
                    p_info->mac_entry = ~0;
                    p_info->flags &= ~NETIF_MAC_ENTRY_CREATED;
                }
                ppa_memcpy(p_info->mac, mac_info.mac, PPA_ETH_ALEN);
                p_info->flags |= NETIF_MAC_AVAILABLE;
            }
        }
        else
        {
            mac_info.mac_ix = p_info->mac_entry;
            ifx_ppa_drv_del_mac_entry( &mac_info, 0);
            p_info->mac_entry = ~0;
            p_info->flags &= ~(NETIF_MAC_ENTRY_CREATED | NETIF_MAC_AVAILABLE);
        }
    }

    ppa_netif_free_item(p_info);

    if ( f_need_update )
    {
        if ( (flags & NETIF_LAN_IF) )
            ppa_netif_remove(ifname, 1);
        if ( (flags & NETIF_WAN_IF) )
            ppa_netif_remove(ifname, 0);

        if ( (flags & NETIF_LAN_IF) && ppa_netif_add(ifname, 1, NULL) != IFX_SUCCESS ){
            dbg("%s: update lan interface %s fail\n", __FUNCTION__, ifname);
            return IFX_FAILURE;
        }

        if ( (flags & NETIF_WAN_IF) && ppa_netif_add(ifname, 0, NULL) != IFX_SUCCESS ){
            dbg("%s: update wan interface %s fail\n", __FUNCTION__, ifname);
            return IFX_FAILURE;
        }
    }

    return IFX_SUCCESS;
}

int32_t ppa_netif_start_iteration(uint32_t *ppos, struct netif_info **ifinfo)
{
    uint32_t l;
    struct netif_info *p;

    ppa_netif_lock_list();

    for ( p = g_netif, l = *ppos; p && l; p = p->next, l-- );

    if ( l == 0 && p )
    {
        ++*ppos;
        *ifinfo = p;
        return IFX_SUCCESS;
    }
    else
    {
        *ifinfo = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_netif_iterate_next(uint32_t *ppos, struct netif_info **ifinfo)
{
    if ( *ifinfo )
    {
        ++*ppos;
        *ifinfo = (*ifinfo)->next;
        return *ifinfo ? IFX_SUCCESS : IFX_FAILURE;
    }
    else
        return IFX_FAILURE;
}

void ppa_netif_stop_iteration(void)
{
    ppa_netif_unlock_list();
}

/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

int32_t ppa_api_netif_manager_init(void)
{
    struct phys_port_info *p_phys_port_info;
    struct ppe_directpath_data *info;
    uint32_t pos;
    int i;
    PPE_IFINFO if_info;
    PPE_COUNT_CFG count={0};

    ppa_netif_free_list();
    ppa_phys_port_free_list();

    ifx_ppa_drv_get_number_of_phys_port( &count, 0);

    
    for ( i = 0; i < count.num; i++ )
    {
        if_info.port = i;
        ifx_ppa_drv_get_phys_port_info( &if_info, 0);
        switch ( (if_info.if_flags & (IFX_PPA_PHYS_PORT_FLAGS_TYPE_MASK | IFX_PPA_PHYS_PORT_FLAGS_MODE_MASK | IFX_PPA_PHYS_PORT_FLAGS_VALID)) )
        {
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_CPU_VALID:
            if ( g_phys_port_cpu == ~0 )
                g_phys_port_cpu = i;
            break;
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_ATM_WAN_VALID:
            if ( g_phys_port_atm_wan == ~0 )
                g_phys_port_atm_wan = i;
            g_phys_port_atm_wan_vlan = (if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_OUTER_VLAN) ? 2 : 1;
            break;
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_ETH_LAN_VALID:
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_ETH_WAN_VALID:
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_ETH_MIX_VALID:
        case IFX_PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID:
            if ( if_info.ifname[0] )
            {
                p_phys_port_info = ppa_phys_port_alloc_item();
                if ( !p_phys_port_info )
                    goto PPA_API_NETIF_MANAGER_INIT_FAIL;
                strcpy(p_phys_port_info->ifname, if_info.ifname);
                switch ( (if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_MODE_MASK) )
                {
                case IFX_PPA_PHYS_PORT_FLAGS_MODE_LAN: p_phys_port_info->mode = 1; break;
                case IFX_PPA_PHYS_PORT_FLAGS_MODE_WAN: p_phys_port_info->mode = 2; break;
                case IFX_PPA_PHYS_PORT_FLAGS_MODE_MIX: p_phys_port_info->mode = 3;
                }
                p_phys_port_info->type = (if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_TYPE_MASK) == IFX_PPA_PHYS_PORT_FLAGS_TYPE_ETH ? 2 : 3;
                p_phys_port_info->vlan = (if_info.if_flags & IFX_PPA_PHYS_PORT_FLAGS_OUTER_VLAN) ? 2 : 1;
                p_phys_port_info->port = i;
                ppa_lock_get(&g_phys_port_lock);
                p_phys_port_info->next = g_phys_port_info;
                g_phys_port_info = p_phys_port_info;
                ppa_lock_release(&g_phys_port_lock);
            }
        }
    }

    pos = 0;
    if ( ppa_directpath_data_start_iteration(&pos, &info) == IFX_SUCCESS )
    {
        do
        {
            if ( (info->flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) && info->netif )
                ppa_phys_port_add(info->netif->name, info->ifid);
        } while ( ppa_directpath_data_iterate_next(&pos, &info) == IFX_SUCCESS );
    }
    ppa_directpath_data_stop_iteration();

    return IFX_SUCCESS;

PPA_API_NETIF_MANAGER_INIT_FAIL:
    ppa_phys_port_free_list();
    return IFX_ENOMEM;
}

void ppa_api_netif_manager_exit(void)
{
    ppa_netif_free_list();
    ppa_phys_port_free_list();
}

int32_t ppa_api_netif_manager_create(void)
{
    if ( ppa_lock_init(&g_phys_port_lock) )
    {
        err("Failed in creating lock for physical network interface list.");
        return IFX_EIO;
    }

    if ( ppa_lock_init(&g_netif_lock) )
    {
        ppa_lock_destroy(&g_phys_port_lock);
        err("Failed in creating lock for network interface list.");
        return IFX_EIO;
    }

    return IFX_SUCCESS;
}

void ppa_api_netif_manager_destroy(void)
{
    ppa_lock_destroy(&g_netif_lock);
    ppa_lock_destroy(&g_phys_port_lock);
}



EXPORT_SYMBOL(g_phys_port_cpu);
EXPORT_SYMBOL(g_phys_port_atm_wan);
EXPORT_SYMBOL(g_phys_port_atm_wan_vlan);
EXPORT_SYMBOL(ppa_phys_port_start_iteration);
EXPORT_SYMBOL(ppa_phys_port_iterate_next);
EXPORT_SYMBOL(ppa_phys_port_stop_iteration);
EXPORT_SYMBOL(ppa_netif_update);
EXPORT_SYMBOL(ppa_netif_start_iteration);
EXPORT_SYMBOL(ppa_netif_iterate_next);
EXPORT_SYMBOL(ppa_netif_stop_iteration);
