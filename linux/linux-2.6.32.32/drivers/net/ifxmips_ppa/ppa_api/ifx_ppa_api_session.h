#ifndef __IFX_PPA_API_SESSION_H__20081104_1139__
#define __IFX_PPA_API_SESSION_H__20081104_1139__



/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_session.h
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 4 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Session Operation Functions Header
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
/*! \file ifx_ppa_api_session.h
    \brief This file contains all ppa routing session api.
*/

/** \defgroup PPA_SESSION_API PPA Session API
    \brief  PPA Session API provide API to get/set/delete/modify all ppa routing session
            - ifx_ppa_api_session.h: Header file for PPA PROC API
            - ifx_ppa_api_session.c: C Implementation file for PPA API
*/
/* @{ */ 


/*
 * ####################################
 *              Definition
 * ####################################
 */
 /*
 *  default settings
 */
#define DEFAULT_TIMEOUT_IN_SEC                  3600    //  1 hour
#define DEFAULT_BRIDGING_TIMEOUT_IN_SEC         60      //  1 minute
#define DEFAULT_MTU                             1500    //  IP frame size (including IP header)
#define DEFAULT_CH_ID                           0
#define DEFAULT_HIT_POLLING_TIME                5      //  change to 5 seconds from original 1 minute for MIB Poll requirement
#define DEFAULT_BRIDGING_HIT_POLLING_TIME       2       //  2 seconds


#define PPA_IS_NAT_SESSION(flags)               ((flags) & (SESSION_VALID_NAT_IP | SESSION_VALID_NAT_PORT))

#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
  #define SESSION_DBG_NOT_REACH_MIN_HITS        0x00000001
  #define SESSION_DBG_ALG                       0x00000002
  #define SESSION_DBG_ZERO_DST_MAC              0x00000004
  #define SESSION_DBG_TCP_NOT_ESTABLISHED       0x00000008
  #define SESSION_DBG_RX_IF_NOT_IN_IF_LIST      0x00000010
  #define SESSION_DBG_TX_IF_NOT_IN_IF_LIST      0x00000020
  #define SESSION_DBG_RX_IF_UPDATE_FAIL         0x00000040
  #define SESSION_DBG_TX_IF_UPDATE_FAIL         0x00000080
  #define SESSION_DBG_SRC_BRG_IF_NOT_IN_BRG_TBL 0x00000100
  #define SESSION_DBG_SRC_IF_NOT_IN_IF_LIST     0x00000200
  #define SESSION_DBG_DST_BRG_IF_NOT_IN_BRG_TBL 0x00000400
  #define SESSION_DBG_DST_IF_NOT_IN_IF_LIST     0x00000800
  #define SESSION_DBG_ADD_PPPOE_ENTRY_FAIL      0x00001000
  #define SESSION_DBG_ADD_MTU_ENTRY_FAIL        0x00002000
  #define SESSION_DBG_ADD_MAC_ENTRY_FAIL        0x00004000
  #define SESSION_DBG_ADD_OUT_VLAN_ENTRY_FAIL   0x00008000
  #define SESSION_DBG_RX_PPPOE                  0x00010000
  #define SESSION_DBG_TX_PPPOE                  0x00020000
  #define SESSION_DBG_TX_BR2684_EOA             0x00040000
  #define SESSION_DBG_TX_BR2684_IPOA            0x00080000
  #define SESSION_DBG_TX_PPPOA                  0x00100000
  #define SESSION_DBG_GET_DST_MAC_FAIL          0x00200000
  #define SESSION_DBG_RX_INNER_VLAN             0x00400000
  #define SESSION_DBG_RX_OUTER_VLAN             0x00800000
  #define SESSION_DBG_TX_INNER_VLAN             0x01000000
  #define SESSION_DBG_TX_OUTER_VLAN             0x02000000
  #define SESSION_DBG_RX_VLAN_CANT_SUPPORT      0x04000000
  #define SESSION_DBG_TX_VLAN_CANT_SUPPORT      0x08000000

  #define SET_DBG_FLAG(pitem, flag)             ((pitem)->debug_flags |= (flag))
  #define CLR_DBG_FLAG(pitem, flag)             ((pitem)->debug_flags &= ~(flag))
#else
  #define SET_DBG_FLAG(pitem, flag)             do { } while ( 0 )
  #define CLR_DBG_FLAG(pitem, flag)             do { } while ( 0 )
#endif


#define SKB_PRIORITY_DEBUG 1



/*
 * ####################################
 *              Data Type
 * ####################################
 */
struct session_list_item {
    struct session_list_item   *next;

    PPA_SESSION                *session;
    //PPA_SESSION_EXTRA           session_extra;
    uint16_t                    ip_proto;
    uint16_t                    ip_tos;
    PPA_IPADDR					src_ip;
    uint16_t                    src_port;
    uint8_t                     src_mac[PPA_ETH_ALEN];
    PPA_IPADDR                  dst_ip;
    uint16_t                    dst_port;
    uint8_t                     dst_mac[PPA_ETH_ALEN];
    PPA_IPADDR					nat_ip;         //  IP address to be replaced by NAT if NAT applies
    uint16_t                    nat_port;       //  Port to be replaced by NAT if NAT applies
    uint16_t                    num_adds;       //  Number of attempts to add session
    PPA_NETIF                  *rx_if;
    PPA_NETIF                  *tx_if;
    uint32_t                    timeout;
    uint32_t                    last_hit_time;  //  Updated by bookkeeping thread
    uint32_t                    new_dscp;
    uint16_t                    pppoe_session_id;
    uint16_t                    new_vci;
    uint32_t                    out_vlan_tag;
    uint32_t                    mtu;
    uint16_t                    dslwan_qid;
    uint16_t                    dest_ifid;

    uint32_t                    flags;          //  Internal flag : SESSION_IS_REPLY, SESSION_IS_TCP,
                                                //                  SESSION_ADDED_IN_HW, SESSION_CAN_NOT_ACCEL
                                                //                  SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT,
                                                //                  SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM,
                                                //                  SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM,
                                                //                  SESSION_VALID_PPPOE, SESSION_VALID_NEW_SRC_MAC,
                                                //                  SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP,
                                                //                  SESSION_VALID_DSLWAN_QID,
                                                //                  SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA
                                                //                  SESSION_LAN_ENTRY, SESSION_WAN_ENTRY, SESSION_IS_IPV6
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
    uint32_t                    debug_flags;
#endif

    uint32_t                    routing_entry;
    uint32_t                    pppoe_entry;
    uint32_t                    mtu_entry;
    uint32_t                    src_mac_entry;
    uint32_t                    out_vlan_entry;
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
    uint32_t                    priority;    //skb priority. for debugging only
#endif
    uint64_t                    mips_bytes; //bytes handled by mips
    uint64_t                    acc_bytes; //bytes handled by PPE acceleration
    uint32_t                    last_bytes; //last updated bytes handled by PPE acceleration
    uint32_t                    tunnel_idx; //tunnel idx for PPE action table
};

struct mc_group_list_item {
    struct mc_group_list_item  *next;

    IP_ADDR_C              ip_mc_group;
    uint32_t                    num_ifs;
    PPA_NETIF                  *netif[PPA_MAX_MC_IFS_NUM];
    uint32_t                    ttl[PPA_MAX_MC_IFS_NUM];
    uint32_t                    if_mask;
    PPA_NETIF                  *src_netif;
    uint16_t                    new_dscp;
    uint16_t                    new_vci;
    uint32_t                    out_vlan_tag;
    uint16_t                    dslwan_qid;
    uint16_t                    dest_ifid;

    uint32_t                    flags;          //  Internal flag : SESSION_IS_REPLY,
                                                //                  SESSION_ADDED_IN_HW, SESSION_CAN_NOT_ACCEL
                                                //                  SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT,
                                                //                  SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM,
                                                //                  SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM,
                                                //                  SESSION_VALID_PPPOE, SESSION_VALID_SRC_MAC,
                                                //                  SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP,
                                                //                  SESSION_VALID_DSLWAN_QID,
                                                //                  SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA
                                                //                  SESSION_LAN_ENTRY, SESSION_WAN_ENTRY
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
    uint32_t                    debug_flags;
#endif

    uint32_t                    mc_entry;
    uint32_t                    src_mac_entry;
    uint32_t                    out_vlan_entry;
    uint32_t                    bridging_flag;  //  sgh add: 0 - routing mode/igmp proxy, 1 - bring mode/igmp snooping.
    uint8_t                      SSM_flag;   /*!< Set the flag if source specific forwarding is required default 0*/ 
    PPA_IPADDR            source_ip;    /*!<  source ip address */
    
};

struct bridging_session_list_item {
    struct bridging_session_list_item
                               *next;

    uint8_t                     mac[PPA_ETH_ALEN];
    PPA_NETIF                  *netif;
    uint16_t                    vci;
    uint16_t                    new_vci;
    uint32_t                    timeout;
    uint32_t                    last_hit_time;  //  Updated by bookkeeping thread
    uint16_t                    dslwan_qid;
    uint16_t                    dest_ifid;

    uint32_t                    flags;          //  Internal flag : SESSION_BRIDGING_VCI_CHECK
                                                //                  SESSION_ADDED_IN_HW, SESSION_CAN_NOT_ACCEL
                                                //                  SESSION_STATIC, SESSION_DROP
                                                //                  SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM
                                                //                  SESSION_SRC_MAC_DROP_EN
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
    uint32_t                    debug_flags;
#endif

    uint32_t                    bridging_entry;
};



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Operation Functions
 */

//  routing session operation
int32_t ppa_lookup_session(PPA_SESSION *, uint32_t, struct session_list_item **);
int32_t ppa_add_session(PPA_BUF *, PPA_SESSION *, struct session_list_item **, uint32_t);
int32_t ppa_update_session(PPA_BUF *, struct session_list_item *, uint32_t);
int32_t ppa_update_session_extra(PPA_SESSION_EXTRA *, struct session_list_item *, uint32_t);
void ppa_remove_session(struct session_list_item *);
void dump_list_item(struct session_list_item *, char *);

int32_t ppa_session_start_iteration(uint32_t *, struct session_list_item **);
int32_t ppa_session_iterate_next(uint32_t *, struct session_list_item **);
void ppa_session_stop_iteration(void);

//  routing session hardware/firmware operation
int32_t ppa_hw_add_session(struct session_list_item *, uint32_t);
int32_t ppa_hw_update_session_extra(struct session_list_item *, uint32_t);
void ppa_hw_del_session(struct session_list_item *);

//  multicast routing operation
int32_t ppa_lookup_mc_group(IP_ADDR_C, struct mc_group_list_item **);
int32_t ppa_mc_group_checking(PPA_MC_GROUP *, struct mc_group_list_item *, uint32_t);
int32_t ppa_add_mc_group(PPA_MC_GROUP *, struct mc_group_list_item **, uint32_t);
int32_t ppa_update_mc_group_extra(PPA_SESSION_EXTRA *, struct mc_group_list_item *, uint32_t);
void ppa_remove_mc_group(struct mc_group_list_item *);

int32_t ppa_mc_group_start_iteration(uint32_t *, struct mc_group_list_item **);
int32_t ppa_mc_group_iterate_next(uint32_t *, struct mc_group_list_item **);
void ppa_mc_group_stop_iteration(void);

//  multicast routing hardware/firmware operation
int32_t ppa_hw_add_mc_group(struct mc_group_list_item *);
int32_t ppa_hw_update_mc_group_extra(struct mc_group_list_item *, uint32_t);
void ppa_hw_del_mc_group(struct mc_group_list_item *);

//  routing polling timer
void ppa_set_polling_timer(uint32_t, uint32_t);

//  bridging session operation
int32_t ppa_bridging_lookup_session(uint8_t *, PPA_NETIF *, struct bridging_session_list_item **);
int32_t ppa_bridging_add_session(uint8_t *, PPA_NETIF *, struct bridging_session_list_item **, uint32_t);
void ppa_bridging_remove_session(struct bridging_session_list_item *);
void dump_bridging_list_item(struct bridging_session_list_item *, char *);

int32_t ppa_bridging_session_start_iteration(uint32_t *, struct bridging_session_list_item **);
int32_t ppa_bridging_session_iterate_next(uint32_t *, struct bridging_session_list_item **);
void ppa_bridging_session_stop_iteration(void);

//  bridging session hardware/firmware operation
int32_t ppa_bridging_hw_add_session(struct bridging_session_list_item *);
void ppa_bridging_hw_del_session(struct bridging_session_list_item *);

//  bridging polling timer
void ppa_bridging_set_polling_timer(uint32_t);

//  special function;
void ppa_remove_sessions_on_netif(PPA_IFNAME *, int);
void ppa_free_session_list_item(struct session_list_item *p_item);
/*
 *  Init/Uninit Functions
 */
int32_t ppa_api_session_manager_init(void);
void ppa_api_session_manager_exit(void);
int32_t ppa_api_session_manager_create(void);
void ppa_api_session_manager_destroy(void);

struct session_list_item *ppa_alloc_session_list_item(void);
void ppa_insert_session_item(struct session_list_item *);
uint32_t ppa_api_get_session_poll_timer(void);
/* @} */

void ppa_init_mc_group_list_item(struct mc_group_list_item *);


/* ctc */
#ifdef CONFIG_ARC_PPA_FILTER

#define		ARC_PPA_FILTER_MAX				64

/* L1 */
#define		ARC_PPA_FILTER_MASK_RXIF		0x0001
/* L2 */
#define		ARC_PPA_FILTER_MASK_MAC			0x0002
#define		ARC_PPA_FILTER_MASK_PRIO		0x0004
/* L3 */
#define		ARC_PPA_FILTER_MASK_TOS			0x0010
#define		ARC_PPA_FILTER_MASK_PROTO		0x0020
#define		ARC_PPA_FILTER_MASK_SRCIP		0x0040
#define		ARC_PPA_FILTER_MASK_DSTIP		0x0080
#define		ARC_PPA_FILTER_MASK_SRCIPV6		0x0100
#define		ARC_PPA_FILTER_MASK_DSTIPV6		0x0200
/* L4 */
#define		ARC_PPA_FILTER_MASK_SRCPORT		0x0400
#define		ARC_PPA_FILTER_MASK_DSTPORT		0x0800
/* misc */
#define		ARC_PPA_FILTER_MASK_PKTDATA		0x4000

#define		ARC_PPA_FILTER_PKTDATA_MAX		4

typedef struct {
	uint16_t		mask;
	char			rxif[9];
	uint8_t			srcmac[6];
	uint8_t			prio;
	uint8_t			tos;
	uint8_t			l4proto;
	PPA_IPADDR		src_ip;
	PPA_IPADDR		dst_ip;
	uint16_t		src_port;
	uint16_t		dst_port;
	uint8_t			pd_type[ ARC_PPA_FILTER_PKTDATA_MAX ];
	uint8_t			pd_off[ ARC_PPA_FILTER_PKTDATA_MAX ];
	uint8_t			pd_mask[ ARC_PPA_FILTER_PKTDATA_MAX ];
	uint8_t			pd_val[ ARC_PPA_FILTER_PKTDATA_MAX ];
	uint32_t		hit;
} stArcPpaFilter;

#endif /*CONFIG_ARC_PPA_FILTER*/



#endif  //  __IFX_PPA_API_SESSION_H__20081104_1139__
