#ifndef __IFX_PPA_API_NETIF_H__20081104_1138__
#define __IFX_PPA_API_NETIF_H__20081104_1138__



/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_netif.h
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 4 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Network Interface Functions Header
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
** 04 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/
/*! \file ifx_ppa_api_netif.h
    \brief This file contains network related api
*/

/** \defgroup PPA_NETIF_API PPA Network Interface API
    \brief  provide network interface related api to get/add/delete/update information
            - ifx_ppa_api_netif.h: Header file for PPA API
            - ifx_ppa_api_netif.c: C Implementation file for PPA API
*/
/* @{ */ 

/*
 * ####################################
 *              Definition
 * ####################################
 */

#define IFX_PPA_IS_PORT_CPU0_AVAILABLE()        (g_phys_port_cpu == ~0 ? 0 : 1)
#define IFX_PPA_IS_PORT_ATM_AVAILABLE()         (g_phys_port_atm_wan == ~0 ? 0 : 1)

#define IFX_PPA_PORT_CPU0                       g_phys_port_cpu
#define IFX_PPA_PORT_ATM                        g_phys_port_atm_wan

#define IFX_PPA_DEST_LIST_CPU0                  (1 << g_phys_port_cpu)
#define IFX_PPA_DEST_LIST_ATM                   (1 << g_phys_port_atm_wan)

#define IFX_PPA_PORT_ATM_VLAN_FLAGS             g_phys_port_atm_wan_vlan

/*
 *  net interface type
 */
#define NETIF_VLAN                              0x00000001
#define NETIF_BRIDGE                            0x00000002
#define NETIF_PHY_ETH                           0x00000010
#define NETIF_PHY_ATM                           0x00000020
#define NETIF_PHY_TUNNEL                        0x00000040
#define NETIF_BR2684                            0x00000100
#define NETIF_EOA                               0x00000200
#define NETIF_IPOA                              0x00000400
#define NETIF_PPPOATM                           0x00000800
#define NETIF_PPPOE                             0x00001000
#define NETIF_VLAN_INNER                        0x00002000
#define NETIF_VLAN_OUTER                        0x00004000
#define NETIF_VLAN_CANT_SUPPORT                 0x00008000
#define NETIF_LAN_IF                            0x00010000
#define NETIF_WAN_IF                            0x00020000
#define NETIF_PHY_IF_GOT                        0x00040000
#define NETIF_PHYS_PORT_GOT                     0x00080000
#define NETIF_MAC_AVAILABLE                     0x00100000
#define NETIF_MAC_ENTRY_CREATED                 0x00200000



/*
 * ####################################
 *              Data Type
 * ####################################
 */

struct phys_port_info {
    struct phys_port_info      *next;
    unsigned int                mode    :2; //  0: CPU, 1: LAN, 2: WAN, 3: MIX (LAN/WAN)
    unsigned int                type    :2; //  0: CPU, 1: ATM, 2: ETH, 3: EXT
    unsigned int                vlan    :2; //  0: no VLAN, 1: inner VLAN, 2: outer VLAN
    unsigned int                port    :26;
    PPA_IFNAME                  ifname[PPA_IF_NAME_SIZE];
};

struct netif_info {
    struct netif_info          *next;
    PPA_ATOMIC                  count;

    PPA_IFNAME                  name[PPA_IF_NAME_SIZE];
    PPA_IFNAME                  phys_netif_name[PPA_IF_NAME_SIZE];
    PPA_NETIF                  *netif;
    PPA_VCC                    *vcc;
    uint8_t                     mac[PPA_ETH_ALEN];
    uint32_t                    flags;  //  NETIF_VLAN, NETIF_BRIDGE, NETIF_PHY_ETH, NETIF_PHY_ATM,
                                        //  NETIF_BR2684, NETIF_EOA, NETIF_IPOA, NETIF_PPPOATM,
                                        //  NETIF_PPPOE, NETIF_PHY_TUNNEL
                                        //  NETIF_VLAN_INNER, NETIF_VLAN_OUTER, NETIF_VLAN_CANT_SUPPORT,
                                        //  NETIF_LAN_IF, NETIF_WAN_IF,
                                        //  NETIF_PHY_IF_GOT, NETIF_PHYS_PORT_GOT,
                                        //  NETIF_MAC_AVAILABLE, NETIF_MAC_ENTRY_CREATED
    uint32_t                    vlan_layer;
    uint32_t                    inner_vid;
    uint32_t                    outer_vid;
    uint32_t                    pppoe_session_id;
    uint32_t                    dslwan_qid;
    uint32_t                    phys_port;
    uint32_t                    mac_entry;
    PPA_NETIF                   *out_vlan_netif;
    PPA_NETIF                   *in_vlan_netif;
    
};



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  variable
 */
extern uint32_t g_phys_port_cpu;
extern uint32_t g_phys_port_atm_wan;
extern uint32_t g_phys_port_atm_wan_vlan;

/*
 *  physical network interface
 */
int32_t ppa_phys_port_add(PPA_IFNAME *, uint32_t);
void ppa_phys_port_remove(uint32_t);
int32_t ppa_phys_port_get_first_eth_lan_port(uint32_t *, PPA_IFNAME **);

int32_t ppa_phys_port_start_iteration(uint32_t *, struct phys_port_info **);
int32_t ppa_phys_port_iterate_next(uint32_t *, struct phys_port_info **);
void ppa_phys_port_stop_iteration(void);

/*
 *  network interface
 */
int32_t ppa_netif_add(PPA_IFNAME *, int, struct netif_info **);
void ppa_netif_remove(PPA_IFNAME *, int);
int32_t ppa_netif_lookup(PPA_IFNAME *, struct netif_info **);
void ppa_netif_put(struct netif_info *);
int32_t ppa_netif_update(PPA_NETIF *, PPA_IFNAME *);

int32_t ppa_netif_start_iteration(uint32_t *, struct netif_info **);
int32_t ppa_netif_iterate_next(uint32_t *, struct netif_info **);
void ppa_netif_stop_iteration(void);

/*
 *  inline functions
 */

/*
 *  Init/Uninit Functions
 */
int32_t ppa_api_netif_manager_init(void);
void ppa_api_netif_manager_exit(void);
int32_t ppa_api_netif_manager_create(void);
void ppa_api_netif_manager_destroy(void);



/* @} */
#endif  //  __IFX_PPA_API_NETIF_H__20081104_1138__
