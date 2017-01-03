#ifndef __IFX_PPA_API_H__20081031_1913__
#define __IFX_PPA_API_H__20081031_1913__


/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api.h
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 31 OCT 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Header File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 31 OCT 2008  Xu Liang        Initiate Version
*******************************************************************************/
/*! \file ifx_ppa_api.h
    \brief This file contains es.
           provide PPA API.
*/

/** \defgroup PPA_API PPA Kernel Hook and Userspace Function API
    \brief PPA is a loadable network module. Hence, it exports its API though function pointer hooks. Callers need to check that hooks are non-NULL before invoking them. The hooks are initialized when the PPA is initialized. Certain API which are control / configuration related are also exposed to user space applications through the ioctl API. The PPA Kernel and Userspace API are discussed in the following sections:
*/
/* @{*/
/** \defgroup PPA_IOCTL PPA Userspace API
    \brief  The subset of PPA API which is exposed to userspace for control and configuration of the PPA is invoked through 
            an ioctls()/system call interface as described in this section. 
            The API is defined in the following two source files:
            - ifx_ppa_api.h: Header file for PPA API
            - ifx_ppa_api.c: C Implementation file for PPA API
*/

/** \defgroup PPA_HOOK_API PPA Hook API
    \brief  PPA is a loadable network module. Hence, it exports its API though function pointer hooks.  Callers need to check that hooks are non-NULL before invoking them. The hooks are initialized  when the PPA is initialized. 
            - ifx_ppa_hook.h: Header file for PPA API
            - ifx_ppa_hook.c: C Implementation file for PPA API
*/

/** \defgroup PPA_PWM_API PPA Power Management API
    \brief  PPA Power Management  API provide PPA Power Management and IOCTL API
            The API is defined in the following two source files
            - ifx_ppa_api_pwm.h: Header file for PPA API
            - ifx_ppa_api_pwm.c: C Implementation file for PPA Power management API
            - ifx_ppa_api_pwm_logic.c: C impelementation file for Powr management Logic and interface with PPE driver
*/
 /** \defgroup PPA_API_DIRECTPATH PPA Direct Path API
    \brief  This section describes the PPA DirectPath API that allows a driver on a CPU to bypass the protocol stack and send and receive packets directly from the PPA acceleration function. For a 2-CPU system, this API is used to communicate with devices whose drivers are running on the 2nd CPU (or Core 1) - usually Core 1 is not running any protocol stack, and all protocol stack intelligence is on Core 0. This API is not yet implemented for PPE D4 or A4 firmware. It is provided as advance information on the DirectPath interfaces.The PPA DirectPath aims to accelerate packet processing by reducing CPU load  when the protocol stack processes the packet. It allows a CPU-bound driver to directly talk to the PPA and to the PPE engine bypassing the stack path and providing a short-cut. 
            - ifx_ppa_api_directpath.h: Header file for PPA API 
            - ifx_ppa_api_directpath.c: C Implementation file for PPA API
*/

/** \defgroup PPA_ADAPTATION_LAYER PPA Stack Adaptation API
    \brief  PPA module aims for OS and Protocol stack independence, and the 
            core PPA logic does not access any OS or Protocol stack implementation
            specific structures directly. The PPA Protocol Stack Adaptation layer 
            provides API that allows for straight-forward and structured OS / protocol
            stack porting of the PPA just by porting the Adaptation Layer (AL) API.
            The AL API is defined in the following two source files
            - ifx_ppa_stack_al.h: Header file for AL layer
            - ifx_ppa_stack_al.c: C Implementation file for AL API
*/
/* @}*/



#include <net/ifx_ppa_api_common.h>
#include <net/ifx_ppa_stack_al.h>
#ifdef __KERNEL__
  #include <net/ifx_ppa_api_directpath.h>
#endif



/*
 * ####################################
 *              Definition
 * ####################################
 */
/*!
    \brief PPA_MAX_IFS_NUM
*/
#define PPA_MAX_IFS_NUM                         10  /*!< Maximum interface number supported */



/*!
    \brief PPA_MAX_MC_IFS_NUM
*/
#define PPA_MAX_MC_IFS_NUM                      8   /*!< Maximum number of Multicast supporting interfaces */

/*!
    \brief PPA_MAX_VLAN_FILTER
*/
#define PPA_MAX_VLAN_FILTER                     32  /*!< Maximum number of VLAN fitler */ 


/*!
    \brief PPA_IOC_MAGIC
*/ 
#define PPA_IOC_MAGIC                           ((uint32_t)'p') /*!< Magic number to differentiate PPA ioctl commands */

/*!
    \brief IFX_SUCCESS
*/
#define IFX_SUCCESS                             0   /*!< Operation was successful. */

/*!
    \brief IFX_FAILURE
*/
#define IFX_FAILURE                             -1  /*!< Operation failed */

/*!
    \brief IFX_EPERM
*/
#define IFX_EPERM                               -2  /*!<   not permitted */

/*!
    \brief IFX_EIO
*/
#define IFX_EIO                                 -5  /*!<   I/O/Hardware/Firmware error */ 

/*!
    \brief IFX_EAGAIN
*/
#define IFX_EAGAIN                              -11 /*!<   try again later */

/*!
    \brief IFX_ENOMEM
*/
#define IFX_ENOMEM                              -12 /*!<   out of memory */

/*!
    \brief IFX_EACCESS
*/
#define IFX_EACCESS                             IFX_EPERM 

/*!
    \brief IFX_EFAULT
*/
#define IFX_EFAULT                              -14 /*!<   bad address */

/*!
    \brief IFX_EBUSY
*/
#define IFX_EBUSY                               -16 /*!<   busy */

/*!
    \brief IFX_EINVAL
*/
#define IFX_EINVAL                              -22 /*!<   invalid argument */

/*!
    \brief IFX_ENOTAVAIL
*/
#define IFX_ENOTAVAIL                           -97

/*!
    \brief IFX_ENOTPOSSIBLE
*/
#define IFX_ENOTPOSSIBLE                        -98

/*!
    \brief IFX_ENOTIMPL
*/
#define IFX_ENOTIMPL                            -99 /*!<   not implemented  */

/*!
    \brief IFX_ENABLED
*/
#define IFX_ENABLED                             1   /*!< Status enabled / Device was enabled  */ 

/*!
    \brief IFX_DISABLED
*/
#define IFX_DISABLED                            0   /*!< Status disabled / Device was disabled. */

/*
 *  flags
 */
 
/*!
    \brief PPA_F_BEFORE_NAT_TRANSFORM
    \note PPA Routing Session add hook is called before NAT transform has taken place. \n
    In Linux OS, this NSFORM corresponds to the netfilter PREROUTING hook 
*/   
#define PPA_F_BEFORE_NAT_TRANSFORM              0x00000001 

/*!
    \brief PPA_F_ACCEL_MODE
    \note notify PPA to enable or disable acceleration for one routing session. It is only for Hook/ioctl, not for PPE FW usage
*/   
#define PPA_F_ACCEL_MODE              0x00000002

/*!
    \brief PPA_F_SESSION_ORG_DIR 
    \note Packet in original direction of session i.e. the direction in which the session was established  
*/
#define PPA_F_SESSION_ORG_DIR                   0x00000010

/*!
    \brief PPA_F_SESSION_REPLY_DIR
    \note Packet in reply direction of session i.e. opposite to the direction in which session was initiated.
*/
#define PPA_F_SESSION_REPLY_DIR                 0x00000020

/*!
    \brief PPA_F_SESSION_BIDIRECTIONAL
    \note For PPA Session add, add a bidirectional session, else unidirection session is assumed.
*/
#define PPA_F_SESSION_BIDIRECTIONAL             (PPA_F_SESSION_ORG_DIR | PPA_F_SESSION_REPLY_DIR)

/*!
    \brief PPA_F_BRIDGED_SESSION
    \note Denotes that the PPA session is bridged 
*/
#define PPA_F_BRIDGED_SESSION                   0x00000100

/*!
    \brief PPA_F_SESSION_NEW_DSCP
    \note Denotes that the PPA session has DSCP remarking enabled
*/
#define PPA_F_SESSION_NEW_DSCP                  0x00001000

/*!
    \brief PPA_F_SESSION_VLAN
    \note Denotes that the PPA session has VLAN tagging enabled.
*/
#define PPA_F_SESSION_VLAN                      0x00002000

/*!
    \brief PPA_F_MTU
    \note Denotes that the PPA session has a MTU limit specified
*/
#define PPA_F_MTU                               0x00004000

/*!
    \brief PPA_F_SESSION_OUT_VLAN
    \note Denotes that the PPA session has Outer VLAN tagging enable 
*/
#define PPA_F_SESSION_OUT_VLAN                  0x00008000

/*!
    \brief PPA_F_BRIDGE_LOCAL
    \note  Denotes that the PPA bridge session is for a flow terminated at the CPE (i.e. not bridged out). Such an entry will not be accelerated
*/
#define PPA_F_BRIDGE_LOCAL                      0x00010000

/*!
    \brief PPA_F_LAN_IF
    \note Indicates that the interface is a LAN interface
*/
#define PPA_F_LAN_IF                            0x01000000

/*!
    \brief PPA_F_STATIC_ENTRY
    \note Indicates that it is a static entry
*/
#define PPA_F_STATIC_ENTRY                      0x20000000

/*!
    \brief PPA_F_DROP_PACKET
    \note Denotes that the PPA session has a drop action specified. In other words, this acts as a fast path \n
          packet filter drop action
*/
#define PPA_F_DROP_PACKET                       0x40000000

/*!
    \brief PPA_F_BRIDGE_ACCEL_MODE
    \note Flag denoting that the PPA should accelerate bridging sessions. Reserved currently
*/
#define PPA_F_BRIDGE_ACCEL_MODE                 0x80000000

/*
 *  interface flags
 */

/*!
    \brief IFX_PPA_SESSION_NOT_ADDED
*/
#define IFX_PPA_SESSION_NOT_ADDED               -1 /*!< PPA Session Add failed. This can happen either because the Session is not yet ready for addition or \n
                                                       that PPA cannot accelerate the session because the packet is looped back */

/*!
    \brief IFX_PPA_SESSION_ADDED
*/
#define IFX_PPA_SESSION_ADDED                   0  /*!< Indicates PPA was able to successfully add the session */

/*!
    \brief IFX_PPA_SESSION_EXISTS
*/
#define IFX_PPA_SESSION_EXISTS                  1  /*!< Indicates PPA already has the session added. This is also a success indication  */


/*
 *  ifx_ppa_inactivity_status return value
 */
/*!
    \brief IFX_PPA_HIT
    \note PPA Session is active i.e. was hit with packets within the configured inactivity time inter
*/
 #define IFX_PPA_HIT                             0
 
/*!
    \brief IFX_PPA_TIMEOUT
    \note PPA Session is inactive and hence has timed out
*/
#define IFX_PPA_TIMEOUT                         1

/*!
    \brief PPA_F_VLAN_FILTER_IFNAME
*/
#define PPA_F_VLAN_FILTER_IFNAME                0 /*!< Port based VLAN */

/*!
    \brief PPA_F_VLAN_FILTER_IP_SRC
*/
#define PPA_F_VLAN_FILTER_IP_SRC                1 /*!< SRC IP based VLAN */

/*!
    \brief PPA_F_VLAN_FILTER_ETH_PROTO
*/
#define PPA_F_VLAN_FILTER_ETH_PROTO             2 /*!< Ethernet Type based VLAN */

/*!
    \brief PPA_F_VLAN_FILTER_VLAN_TAG
*/
#define PPA_F_VLAN_FILTER_VLAN_TAG              3 /*!< Vlan tag based VLAN */

/*!
    \brief PPA_INVALID_QID
*/
#define PPA_INVALID_QID                         0xFFFF  /*!< Invalid VLAN ID. Note, it is used only in IOCTL */

/*!   
    \brief PPA_VLAN_TAG_MASK 
*/
#define PPA_VLAN_TAG_MASK                 0xFFFF1FFF  /*!< VLAN MASK to remove VLAN priority*/

/*!
    \brief MAX_HOOK_NAME_LEN
*/
#define MAX_HOOK_NAME_LEN                         71  /*!< maximum hook name length */


/*!
    \brief PPA_PORT_MODE_ETH
*/
#define PPA_PORT_MODE_ETH        1  /*!< Ethernet Port */
/*!
    \brief PPA_PORT_MODE_DSL
*/
#define PPA_PORT_MODE_DSL        2  /*!< DSL Port */

/*!
    \brief PPA_PORT_MODE_EXT
*/
#define PPA_PORT_MODE_EXT        3  /*!< Extension Port, like USB/WLAN */

/*!
    \brief PPA_PORT_MODE_CPU
*/
#define PPA_PORT_MODE_CPU        4  /*!< CPU */



#ifdef NO_DOXY
#define VLAN_ID_SPLIT(full_id, pri, cfi, vid)   pri=( (full_id) >> 13 ) & 7; cfi=( (full_id) >>12) & 1; vid= (full_id) & 0xFFF
#define VLAN_ID_CONBINE(full_id, pri, cfi, vid)   full_id =( ( (uint16_t)(pri) & 7) << 13 )  | ( ( (uint16_t)( cfi) & 1) << 12  )  | ((uint16_t) (vid) & 0xFFF ) 


/*
 *  internal flag
 */

#define SESSION_INTERNAL_FLAG_BASE              0
#define SESSION_IS_REPLY                        0x00000001
#define SESSION_BRIDGING_VCI_CHECK              0x00000002
#define SESSION_IS_TCP                          0x00000004
#define SESSION_ADDED_IN_HW                     0x00000010
#define SESSION_NON_ACCE_MASK   ~SESSION_ADDED_IN_HW    //for ioctl only
#define SESSION_CAN_NOT_ACCEL                   0x00000020
#define SESSION_STATIC                          0x00000040
#define SESSION_DROP                            0x00000080
#define SESSION_VALID_NAT_IP                    0x00000100
#define SESSION_VALID_NAT_PORT                  0x00000200
#define SESSION_VALID_NAT_SNAT                  0x00000400  //  src IP is replaced, otherwise dest IP is replaced
#define SESSION_VALID_VLAN_INS                  0x00001000
#define SESSION_VALID_VLAN_RM                   0x00002000
#define SESSION_VALID_OUT_VLAN_INS              0x00004000
#define SESSION_VALID_OUT_VLAN_RM               0x00008000
#define SESSION_VALID_PPPOE                     0x00010000
#define SESSION_VALID_NEW_SRC_MAC               0x00020000
#define SESSION_VALID_SRC_MAC                   SESSION_VALID_NEW_SRC_MAC
#define SESSION_VALID_MTU                       0x00040000
#define SESSION_VALID_NEW_DSCP                  0x00080000
#define SESSION_VALID_DSLWAN_QID                0x00100000
#define SESSION_TX_ITF_IPOA                     0x00200000
#define SESSION_TX_ITF_PPPOA                    0x00400000
#define SESSION_TX_ITF_IPOA_PPPOA_MASK          (SESSION_TX_ITF_IPOA | SESSION_TX_ITF_PPPOA)
#define SESSION_SRC_MAC_DROP_EN                 0x01000000
#define SESSION_TUNNEL_6RD                      0x02000000
#define SESSION_LAN_ENTRY                       0x10000000
#define SESSION_WAN_ENTRY                       0x20000000
#define SESSION_IS_IPV6                         0x40000000
#endif

/*
 * ####################################
 *              Data Type
 * ####################################
 */

/* -------------------------------------------------------------------------- */
/*                 Structure and Enumeration Type Defintions                  */
/* -------------------------------------------------------------------------- */

/** \addtogroup  PPA_HOOK_API */
/*@{*/

/*!
    \brief This is the data structure for PPA Interface Info specification.
*/
typedef struct {
    PPA_IFNAME *ifname;     /*!< Name of the stack interface */
    uint32_t    if_flags;   /*!< Flags for Interface. Valid values are below: PPA_F_LAN_IF and PPA_F_WAN_IF */
    uint32_t    port;       /*!< physical port id  for this Interface. Valid values are below: 0 ~  */
} PPA_IFINFO;

/*!
    \brief This is the data structure for PPA Packet header verification checks.
*/
typedef struct ppa_verify_checks {
    uint32_t    f_ip_verify             :1; /*!< Enable/Disable IP verification checks.  Valid values are IFX_ENABLED or IFX_DISABLED */
    uint32_t    f_tcp_udp_verify        :1; /*!< Enable/Disable TCP/UDP verification checks. Valid values are IFX_ENABLED or IFX_DISABLED */
    uint32_t    f_tcp_udp_err_drop      :1; /*!< Enable/Disable drop packet if TCP/UDP checksum is wrong. \n
                                                If packet is not dropped, then it is forwarded to the control CPU. \n
                                                Valid values are IFX_ENABLED or IFX_DISABLED */
    uint32_t    f_drop_on_no_hit        :1; /*!< Drop unicast packets on no hit, forward to MIPS/Control CPU otherwise (default). Valid values are IFX_ENABLED or IFX_DISABLED */
    uint32_t    f_mc_drop_on_no_hit     :1; /*!< Drop multicast on no hit, forward to MIPS/Control CPU otherwise (default). Valid values are IFX_ENABLED or IFX_DISABLED */
} PPA_VERIFY_CHECKS;

/*!
    \brief This is the data structure for PPA Initialization kernel hook function
*/
typedef struct {
    PPA_VERIFY_CHECKS   lan_rx_checks;      /*!<   LAN Ingress packet checks */
    PPA_VERIFY_CHECKS   wan_rx_checks;      /*!<   WAN Ingress packet checks */
    uint32_t    num_lanifs;                 /*!<   Number of LAN side interfaces */
    PPA_IFINFO *p_lanifs;                   /*!<   Pointer to array of LAN Interfaces. */
    uint32_t    num_wanifs;                 /*!<   Number of WAN side interfaces */
    PPA_IFINFO *p_wanifs;                   /*!<   Pointer to array of WAN Interfaces. */
    uint32_t    max_lan_source_entries;     /*!<   Maximum Number of session entries with LAN source */
    uint32_t    max_wan_source_entries;     /*!<   Maximum Number of session entries with WAN source */
    uint32_t    max_mc_entries;             /*!<   Maximum Number of multicast sessions */
    uint32_t    max_bridging_entries;       /*!<   Maximum Number of bridging entries */
    uint32_t    add_requires_min_hits;      /*!<   Minimum number of calls to ppa_add_session() before session would be added in h/w - calls from the same hook position in stack. Currently, set to 1 */
} PPA_INIT_INFO;

/*!
    \brief This is the data structure for additional session related information for the PPA. It specifies on a per session basis
            attributes like VLAN tagging, DSCP remarking etc. This structure depends on the PPE acceleration firmware
            capabilities. New versions of PPA will only support the capabilities as in PPE A4/D4 firmware,
            The current PPA driver (for PPE A4/D4 firmware) supports 2-level of VLANs (or stacked VLANs). The outer VLAN is
            the one used for separating LAN and WAN traffic on a switch (for Ethernet WAN). Inner VLAN tag is application
            specific VLAN. In case, there is no outer VLAN tag required (for LAN/WAN separation on the switch), then this
            field is not specified.
*/
typedef struct {
    uint32_t    new_dscp            :6; /*!<   New DSCP code point value for the session.Valid values are 0-63. */
    uint32_t    dscp_remark         :1; /*!<   DSCP remarking needs to be carried out for the session.Valid values are:IFX_ENABLED and IFX_DISABLED */
    uint32_t    vlan_insert         :1; /*!<   If inner VLAN tag should be inserted into the frame at egress. Valid values are: IFX_ENABLED and IFX_DISABLED */
    uint32_t    vlan_remove         :1; /*!<  If inner VLAN untagging should be performed on the received frame. Untagging, if enabled, is \n
                                            carried out before any VLAN tag insert. Valid values are:IFX_ENABLED and IFX_DISABLED */
    uint32_t    out_vlan_insert     :1; /*!<   If outer VLAN tag should be inserted into the frame at egress. Valid values are: IFX_ENABLED and IFX_DISABLED */
    uint32_t    out_vlan_remove     :1; /*!<  If outer VLAN untagging should be performed on the received frame. Untagging, if enabled, is \n
                                            carried out before any VLAN tag insert. Valid values are:IFX_ENABLED and IFX_DISABLED */
    uint16_t    dslwan_qid_remark   :1; /*!<   if dslwan qid should be set. Valid values are: IFX_ENABLED and IFX_DISABLED */
    uint32_t    reserved1           :4; /*!<   reserved */
    uint32_t    vlan_prio           :3; /*!<   802.1p VLAN priority configuration. Valid values are 0-7. */
    uint32_t    vlan_cfi            :1; /*!<   lways set to 1 for Ethernet frames */
    uint32_t    vlan_id             :12;/*!<   VLAN Id to be used to tag the frame. Valid values are 0-4095. */
    uint16_t    mtu;                    /*!<   MTU of frames classified to this session */
    uint16_t    dslwan_qid;             /*!<   dslwan_qid. Valid values are 0 ~ 16 */
    uint32_t    session_flags;          /*!<  Session flags used to identify which fields in the PPA_SESSION_EXTRA structure are valid in \n
                                            a call to the PPA Session Modify API. \n
                                            Valid values are one or more of: \n
                                                PPA_F_SESSION_NEW_DSCP \n
                                                PPA_F_SESSION_VLAN \n
                                                PPA_F_SESSION_OUT_VLAN \n
                                                PPA_F_MTU \n
                                         */
    uint32_t    out_vlan_tag;           /*!<   VLAN tag value including VLAN Id */


                                            
    uint16_t    accel_enable:1;        /*!<   to enable/disable acceleartion for one specified routing session. It will be used only in PPA API level, not HAL and PPE FW level */                                 
} PPA_SESSION_EXTRA;

/*!
    \brief This is the data structure which specifies an interface and its TTL value as applicable for multicast routing.
*/
typedef struct {
    PPA_IFNAME *ifname; /*!<   Pointer to interface name.  */
    uint8_t     ttl;    /*!<  Time to Live (TTL) value of interface which is used for multicast routing to decide if a packet can be routed onto that interface
                            Note, it is not used at present.
                         */
} IF_TTL_ENTRY;

/*!
    \brief This is the data structure for basic IPV4/IPV6 address
*/
typedef union {
        uint32_t ip;     /*!< ipv4 address */
        uint32_t ip6[4];   /*!< ipv6 address */
}IP_ADDR;

/*!
    \brief This is the data structure for complex IPV4/IPV6 address
*/
typedef struct {
    uint32_t f_ipv6; /*!< flag to specify the ipv4 version: 0---IPV4, 1 -- IPV6 */
    IP_ADDR ip;  /*!< multiple ip address format support */
} IP_ADDR_C;


/*!
    \brief This is the data structure for PPA Multicast Group membership. It specifies the interfaces which are members of
            the specified IP Multicast Group address. Please see the discussion on outer and inner VLAN tags in the
            section on PPA_SESSION_EXTRA data structure.
*/
typedef struct {
    IP_ADDR_C       ip_mc_group;    /*!<   Multicast IP address group */
    int8_t          num_ifs;        /*!<   Number of Interfaces which are member of this Multicast IP group address */
    IF_TTL_ENTRY    array_mem_ifs[PPA_MAX_MC_IFS_NUM];  /*!< Array of interface elements of maximum PPA_MAX_MC_IFS_NUM elements.
                                                         Actual number of entries is specified by num_ifs */
    uint8_t         if_mask;        /*!<   Mask of Interfaces corresponding to num_ifs interfaces specified in array_mem_ifs. For internaly use only. */
    PPA_IFNAME     *src_ifname;     /*!<   the source interface of specified multicast IP address group */
    uint32_t        vlan_insert     :1;     /*!<   If inner VLAN tag should be inserted into the frame at egress. Valid values are: IFX_ENABLED and IFX_DISABLED */
    uint32_t        vlan_remove     :1;     /*!<  If inner VLAN untagging should be performed on the received frame. Untagging, if enabled, is
                                                carried out before any VLAN tag insert. Valid values are:IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_vlan_insert :1;     /*!<   If outer VLAN tag should be inserted into the frame at egress. Valid values are: IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_vlan_remove :1;     /*!<  If outer VLAN untagging should be performed on the received frame. Untagging, if enabled, is
                                                carried out before any VLAN tag insert. Valid values are:IFX_ENABLED and IFX_DISABLED */
    uint32_t        dslwan_qid_remark:1;    /*!<   not use at present */
    uint32_t        reserved1       :3;     /*!<   valid in A4/A5 */
    uint32_t        vlan_prio       :3;     /*!<   802.1p VLAN priority configuration. Valid values are 0-7. */
    uint32_t        vlan_cfi        :1;     /*!<   Always set to 1 for Ethernet frames */
    uint32_t        vlan_id         :12;    /*!<   VLAN Id to be used to tag the frame. Valid values are 0-4095 */
    uint32_t        out_vlan_tag;           /*!<   Outer VLAN tag value including VLAN Id. */
    uint32_t        new_dscp_en     :1;     /*!<   If new dscp value should be set. Valid values are:IFX_ENABLED and IFX_DISABLED */
    uint32_t        res             :15;    /*!<   reserved */  
    uint32_t        new_dscp        :16;    /*!<   New DSCP code point value for the session.Valid values are 0-63. */
    uint16_t        dslwan_qid;             /*!<   not use at present */

    uint32_t        bridging_flag;          /*!<   0 - routing mode/igmp proxy, 1 - bridge mode/igmp snooping. */
    uint8_t         mac[PPA_ETH_ALEN];      /*!<  reserved for future */
    uint8_t         SSM_flag;     /*!< Set the flag if source specific forwarding is required default 0*/ 
    PPA_IPADDR      source_ip;    /*!<  source ip address */
} PPA_MC_GROUP;

/*!
    \brief This data structure is an abstraction for unicast and multicast routing sessions.
             Pointer to any kind of PPA session
*/
typedef void PPA_U_SESSION;

/*!
    \brief This is the data structure for standard packet and byte statistics for an interface.
*/
typedef struct {
    uint32_t    tx_pkts;            /*!<   Number of transmitted packets through the interface */
    uint32_t    rx_pkts;            /*!<   Number of received packets through the interface */
    uint32_t    tx_discard_pkts;    /*!<   Number of packets discarded while transmitting through the interface. */
    uint32_t    tx_error_pkts;      /*!<   Number of transmit errors through the interface. */
    uint32_t    rx_discard_pkts;    /*!<   Number of received packets through the interface that were discarded */
    uint32_t    rx_error_pkts;      /*!<   Number of received errors through the interface. */
    uint32_t    tx_bytes;           /*!<   Number of transmit bytes through the interface */
    uint32_t    rx_bytes;           /*!<   Number of received bytes through the interface */
} PPA_IF_STATS;

/*!
    \brief This is the data structure for PPA accelerated statistics for an interface. Depending on the platform and
             acceleration capabilities, some of the statistics may not be available.
*/
typedef struct {
    uint32_t    fast_routed_tcp_pkts;       /*!< Fastpath routed TCP unicast packets Tx */
    uint32_t    fast_routed_udp_pkts;       /*!< Fastpath routed UDP unicast packets Tx */
    uint32_t    fast_routed_udp_mcast_pkts; /*!< Fastpath routed UDP multicast packets Tx */
    uint32_t    fast_drop_pkts;             /*!< Fastpath ingress pkts dropped */
    uint32_t    fast_drop_bytes;            /*!< Fastpath ingress bytes dropped */
    uint32_t    fast_ingress_cpu_pkts;      /*!< Fastpath ingress CPU pkts */
    uint32_t    fast_ingress_cpu_bytes;     /*!< Fastpath ingress CPU bytes */
    uint32_t    rx_pkt_errors;              /*!< Fastpath packet error */
    uint32_t    fast_bridged_ucast_pkts;    /*!< Fastpath bridged unicast pkts */
    uint32_t    fast_bridged_mcast_pkts;    /*!< Fastpath bridged multicast pkts */
    uint32_t    fast_bridged_bcast_pkts;    /*!< Fastpath bridged broadcast pkts */
    uint32_t    fast_bridged_bytes;         /*!< Fastpath bridged bytes */
} PPA_ACCEL_STATS;

/*!
    \brief This is the data structure for VLAN tag control on a per interface basis. It is currently supported only for bridging
    paths. For PPE A4 firmware, 2 levels of VLAN is configurable, while for older PPE D4 firmware, only inner VLAN
    tag is configurable. Please see discussion in section PPA_SESSION_EXTRA. Briefly, couter VLAN tag
    configuration is used for LAN and WAN isolation on the same external switch, while the other set of VLAN tag
    configuration is driven from application needs (i.e. not stripped off when the packet hits the wire).
*/
typedef struct {
    uint32_t        unmodified      :1; /*!< Indicates if there is no VLAN tag modification. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        insertion       :1; /*!< Indicates if there is a VLAN tag inserted. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        remove          :1; /*!< Indicates if there is a VLAN tag removed. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        replace         :1; /*!< Indicates if there is a VLAN tag replaced. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_unmodified  :1; /*!< Indicates if there is no outer VLAN tag modification. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_insertion   :1; /*!< Indicates if there is a outer VLAN tag inserted. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_remove      :1; /*!< Indicates if there is a outer VLAN tag removed. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_replace     :1; /*!< Indicates if there is  a outerVLAN tag replaced. Valid values are IFX_ENABLED and IFX_DISABLED */
} PPA_VLAN_TAG_CTRL;

/*!
    \brief This is the data structure for VLAN configuration control on a per interface basis. It is currently supported only for
    bridging paths.
*/
typedef struct {
    uint32_t        src_ip_based_vlan   :1; /*!< Indicates if Source IP address filter based VLAN tagging is enabled for this interface. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        eth_type_based_vlan :1; /*!< Indicates if Ethernet header type based VLAN tagging is enabled for this interface. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        vlanid_based_vlan   :1; /*!< Indicates if VLAN re-tagging is enabled based on existing VLAN Id of received frame. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        port_based_vlan     :1; /*!< Indicates if port based VLAN tagging is enabled for this interface. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        vlan_aware          :1; /*!< Indicates if bridge is VLAN aware and enforces VLAN based forwarding for this interface. Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        out_vlan_aware      :1; /*!< Indicates if bridge is outer VLAN aware and enforces VLAN based forwarding for this
                                              interface. If this field is not enabled, then outer VLAN processing is don't care.
                                              interface. If this field is not enabled, then outer VLAN processing is don't care.
                                              Valid values are IFX_ENABLED and IFX_DISABLED */
} PPA_VLAN_CFG;

/*!
    \brief Union of PPA VLAN filter criteria.
*/
typedef union 
{
        PPA_IFNAME     *ifname; /*!< Pointer to interface name on which VLAN filter match is to be performed. */
        IPADDR          ip_src; /*!< IP source address of ingress frame for VLAN filter matching. */
        uint32_t        eth_protocol;       /*!< Ethernet protocol as a match filter for VLAN filter matching */
        uint32_t        ingress_vlan_tag;   /*!< Ingress frame VLAN tag as match criteria for VLAN filter matching */
} match_criteria_vlan;

/*!
    \brief This data structure specifies the filter or match criteria for applying VLAN transforms based on rules. It is currently supported only for bridging paths.
*/
typedef struct {
    match_criteria_vlan    match_field;            /*!< Union of VLAN filter criteria */
    uint32_t        match_flags;    /*!< Indicates which VLAN filter criteria is specified in this VLAN match entry.
                                      Valid values are one of the following: \n
                                      PPA_F_VLAN_FILTER_IFNAME \n
                                      PPA_F_VLAN_FILTER_IP_SRC \n
                                      PPA_F_VLAN_FILTER_ETH_PROTO \n
                                      PPA_F_VLAN_FILTER_VLAN_TAG \n
                                     */
} PPA_VLAN_MATCH_FIELD;

/*!
    \brief This is the data structure for PPA VLAN configuration ioctl() on a per interface basis from userspace. It is currently
supported only for bridging paths.
*/
typedef struct {
    uint16_t        vlan_vci;   /*!< VLAN Information including VLAN Id, 802.1p and CFI bits. */
    uint16_t        qid;        /*!< queue index */
    uint32_t        out_vlan_id; /*!< out vlan id */
    uint32_t        inner_vlan_tag_ctrl;/*!< none(0)/remove(1)/insert(2)/replac(3), for vlan tag based only. */
    uint32_t        out_vlan_tag_ctrl;  /*!< none(0)/remove(1)/insert(2)/replac(3), for vlan tag based only. */
    uint16_t        num_ifs;    /*!< Number of interfaces in the array of PPA_IFINFO structures. */
    PPA_IFINFO     *vlan_if_membership; /*!< Pointer to array of interface info structures for each interface which is a member of this VLAN group. The number of entries is given by num_ifs. */
} PPA_VLAN_INFO;

/*!
    \brief This is the data structure for PPA VLAN filter configuration. It is currently supported only for bridging paths
*/
typedef struct {
    PPA_VLAN_MATCH_FIELD    match_field;    /*!< VLAN Match field information */
    PPA_VLAN_INFO           vlan_info;      /*!< VLAN Group and Membership Info */
} PPA_VLAN_FILTER_CONFIG;

/*!
    \brief This is the data structure for cout information, like lan interface count, LAN acceleration count and so on  
*/
typedef struct {
    uint32_t    count;  /*!< the number */
    uint32_t    flag;   /*!< the flag */
} PPA_CMD_COUNT_INFO;

/*!
    \brief This is the data structure for get some structure size 
*/
typedef struct {
    uint32_t    rout_session_size;  /*!< the structure size of one routing session */
    uint32_t    mc_session_size;   /*!< the structure size of one multicast session */
    uint32_t   br_session_size;    /*!< the structure size of one bridge session */
    uint32_t   netif_size;         /*!< the structure size of one network interface information*/
} PPA_CMD_SIZE_INFO;
/*@}*/ /* PPA_HOOK_API */

/*
 *  ioctl command structures
 */

/** \addtogroup  PPA_IOCTL */
/*@{*/

/*!
    \brief This is the data structure for PPA Interface information used from the userspacef
*/
typedef struct {
    PPA_IFNAME  ifname[PPA_IF_NAME_SIZE];   /*!<  Name of the stack interface ( provide storage buffer )  */
    uint32_t    if_flags;   /*!< Flags for Interface. Valid values are below: PPA_F_LAN_IF and PPA_F_WAN_IF */
} PPA_CMD_IFINFO;

/*!
    \brief This is the data structure for PPA Init used from the userspace
*/
typedef struct {
    PPA_VERIFY_CHECKS   lan_rx_checks;          /*!<  LAN Ingress checks */
    PPA_VERIFY_CHECKS   wan_rx_checks;          /*!<  WAN Ingress checks */
    uint32_t        num_lanifs;                 /*!<   Number of LAN side interfaces */
    PPA_CMD_IFINFO  p_lanifs[PPA_MAX_IFS_NUM];  /*!<  Array of LAN Interface Info structures (provides storage buffer). */
    uint32_t        num_wanifs;                 /*!<   Number of WAN side interfaces */
    PPA_CMD_IFINFO  p_wanifs[PPA_MAX_IFS_NUM];  /*!<  Array of WAN Interface Info structures (provides storage buffer). */
    uint32_t        max_lan_source_entries;     /*!<   Number of session entries with LAN source */
    uint32_t        max_wan_source_entries;     /*!<   Number of session entries with WAN source */
    uint32_t        max_mc_entries;             /*!<   Number of multicast sessions */
    uint32_t        max_bridging_entries;       /*!<   Number of bridging entries */
    uint32_t        add_requires_min_hits;      /*!<   Minimum number of calls to ppa_add before session would be added in h/w */
    uint32_t        flags;  /*!< Flags for PPA Initialization. Currently this field is reserved. */

} PPA_CMD_INIT_INFO;

/*!
    \brief This is the data structure for PPA Acceleration Enable / Disable configuration
*/
typedef struct {
    uint32_t        lan_rx_ppa_enable;  /*!<  lan Interface specific flags. Current Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        wan_rx_ppa_enable;  /*!<  wan Interface specific flags. Current Valid values are IFX_ENABLED and IFX_DISABLED */
    uint32_t        flags;              /*!< Reserved currently */
} PPA_CMD_ENABLE_INFO;

/*!
    \brief This is the data structure for MAC table entry used in PPA ioctl interface
*/
typedef struct {
    IP_ADDR_C              mcast_addr; /*!< MC  address of the entry */
    IP_ADDR                  source_ip;  /*!< source ip */
    uint8_t                     SSM_flag; /*!< ssm flag */
    PPA_SESSION_EXTRA   mc_extra;   /*!< Pointer to PPA Multicast session parameters like VLAN configuration, DSCP remarking */
    uint32_t            flags;      /*!< Flags for the PPA Multicast entry info structure. Reserved currently. */

} PPA_CMD_MC_ENTRY;

/*!
    \brief This is the data structure for learned MAC address used in PPA ioctl interface
*/
typedef struct {
    uint8_t         mac_addr[PPA_ETH_ALEN];   /*!< MAC address learned */
    PPA_IFNAME      ifname[PPA_IF_NAME_SIZE]; /*!< The interface which learned the MAC address */
    uint32_t        flags;    /*!< for future */
} PPA_CMD_MAC_ENTRY;

/*!
    \brief This is the data structure for PPA VLAN configuration ioctl() on a per interface basis from userspace. It is currently
    supported only for bridging paths.
*/
typedef struct
{
    PPA_IFNAME          if_name[PPA_IF_NAME_SIZE];  /*!< Pointer to interface name for which VLAN related configuration is specified. */
    PPA_VLAN_TAG_CTRL   vlan_tag_ctrl;  /*!< VLAN Tag Control structure for the interface */
    PPA_VLAN_CFG        vlan_cfg;       /*!< VLAN Configuration control structure for the interface */
    uint32_t            flags;          /*!< Flags field. Reserved currently and omitted in implementation. */
} PPA_CMD_BR_IF_VLAN_CONFIG;


/*!
  \brief Union for VLAN filter matching criteria. 
*/       
typedef union {
        PPA_IFNAME      ifname[PPA_IF_NAME_SIZE];  /*!<  Pointer to interface name on which VLAN filter match is to be performed. */
        IPADDR          ip_src;       /*!< IP source address of ingress frame for VLAN filter matching. */               
        uint32_t        eth_protocol; /*!< Ethernet protocol as a match filter for VLAN filter matching. */  
        uint32_t        ingress_vlan_tag; /*!< Ingress frame VLAN tag as match criteria for VLAN filter matching. */
} filter_criteria;          


/*!
  \brief This data structure specifies the filter or match criteria for applying VLAN transforms based on rules. It is currently supported only for bridging paths.
*/
typedef struct {
    filter_criteria     match_field;          /*!< Union for VLAN filter criteria. */       
    uint32_t        match_flags;      /*!< Indicates which VLAN filter criteria is specified in this VLAN match entry. \n
                                            Valid values are one of the following: \n
                                            - PPA_F_VLAN_FILTER_IFNAME \n
                                            - PPA_F_VLAN_FILTER_IP_SRC \n
                                            - PPA_F_VLAN_FILTER_ETH_PROTO \n
                                            - PPA_F_VLAN_FILTER_VLAN_TAG
                                        */
} PPA_CMD_VLAN_MATCH_FIELD;

/*!
  \brief This is the data structure for PPA VLAN configuration ioctl() on a per interface basis from userspace. It is currently supported only for bridging paths.
*/  
typedef struct {
    uint16_t        vlan_vci;       /*!< VLAN Information including VLAN Id, 802.1p and CFI bits */
    uint16_t        qid;            /*!< dest_qos */
    uint32_t        out_vlan_id;    /*!< new out vlan id */
    uint32_t        out_vlan_tag_ctrl;  /*!< unmodify(0)/remove(1)/insert(2)/replac(3), for vlan tag based only. */
    uint32_t        inner_vlan_tag_ctrl;/*!< unmodify(0)/remove(1)/insert(2)/replac(3), for vlan tag based only. */
    uint16_t        num_ifs;         /*!< Number of interfaces in the array of PPA_IFINFO structures.  */
    PPA_CMD_IFINFO  vlan_if_membership[PPA_MAX_IFS_NUM]; /*!< Pointer to array of interface info structures for each interface which is a member of this VLAN group. The number of entries is given by num_ifs. */
} PPA_CMD_VLAN_INFO;

/*!
    \brief This is the data structure for basic VLAN filter setting in PPA ioctl interface
*/
typedef struct {
    PPA_CMD_VLAN_MATCH_FIELD    match_field;  /*!< vlan filter match field */
    PPA_CMD_VLAN_INFO           vlan_info;    /*!< vlan information */
} _PPA_CMD_VLAN_FILTER_CONFIG;

/*!
    \brief This is the data structure for VLAN filter configure in PPA ioctl interface
*/
typedef struct {
    _PPA_CMD_VLAN_FILTER_CONFIG vlan_filter_cfg;  /*!< vlan filter basc information */
    uint32_t                    flags;            /*!< flag */
} PPA_CMD_VLAN_FILTER_CONFIG;

/*!
    \brief This is the data structure for PPA VLAN configuration as passed to the PPA ioctl() API from userspace. It is
currently supported only for bridging paths.
*/
typedef struct {
    PPA_CMD_COUNT_INFO          count_info; /*!< Number of filters returned in pointer to array of filters. */
    PPA_CMD_VLAN_FILTER_CONFIG  filters[1]; /*!< it is a dummy array. Userspace should apply storage buffer for it */
} PPA_CMD_VLAN_ALL_FILTER_CONFIG;
/*!
    \brief This is the data structure for PPA accelerated statistics for an interface. Depending on the platform and
             acceleration capabilities, some of the statistics may not be available.
*/
typedef struct {
    PPA_IFNAME      ifname[PPA_IF_NAME_SIZE];   /*!< interface name ( provides storage buffer) */
    uint8_t         mac[PPA_ETH_ALEN];  /*!< MAC address of the Ethernet Interface ( provides storage buffer) */
    uint32_t        flags;              /*!< reserved for future */
} PPA_CMD_IF_MAC_INFO;

/*!
    \brief This is the data structure for LAN/WAN interface setting
*/
typedef struct {
    uint32_t        num_ifinfos;             /*!< number of interface in the list */
    PPA_CMD_IFINFO  ifinfo[PPA_MAX_IFS_NUM]; /*!< buffer for storing network interface list */
} PPA_CMD_IFINFOS;

/*!
    \brief This is the data structure for Multicast group related ioctl
*/
typedef struct {
    uint8_t             mac[PPA_ETH_ALEN];                                /*!< mac address of the multicast group, reserved for future */
    PPA_IFNAME          lan_ifname[PPA_MAX_MC_IFS_NUM][PPA_IF_NAME_SIZE]; /*!< downstream interface list buffer */
    PPA_IFNAME          src_ifname[PPA_IF_NAME_SIZE];                     /*!< source interface which receive multicast streaming packet */
    uint32_t            num_ifs;                                          /*!< downstream interface number */
    uint32_t            bridging_flag;  /*!< IGMP Proxy/snooping flag:  0 - routing mode/igmp proxy, 1 - bring mode/igmp snooping. */ 

    uint32_t            new_dscp_en;    /*!< dscp editing flag: 1 -- need to edit, 0 --unmodify */
    PPA_CMD_MC_ENTRY    mc;             /*!< multicast group information */ 
} PPA_CMD_MC_GROUP_INFO;
/*!
    \brief This is the data structure for get all Multicast group via ioctl
*/
typedef struct {
    PPA_CMD_COUNT_INFO      count_info;       /*!< the multicast counter */
    PPA_CMD_MC_GROUP_INFO   mc_group_list[1]; /*!< Note, here is a dummy array, user need to malloc memory accordingly to the session number */
} PPA_CMD_MC_GROUPS_INFO;

/*!
    \brief This is the data structure contains PPA session information.
*/
typedef struct {
    uint16_t                    ip_proto;      /*!< IP portocol TCP,UDP.  */
    uint16_t                    ip_tos;        /*!< IP ToS value  */   
    PPA_IPADDR                  src_ip;        /*!< source IP address  */
    uint16_t                    src_port;      /*!< source port  */  
    PPA_IPADDR                  dst_ip;        /*!< destination IP address  */ 
    uint16_t                    dst_port;      /*!< destination port  */ 
    PPA_IPADDR                  nat_ip;         /*!< IP address to be replaced by NAT if NAT applies */
    uint16_t                    nat_port;       /*!< Port to be replaced by NAT if NAT applies */
    uint32_t                    new_dscp;       /*!< If DSCP remarking required  */
    uint16_t                    new_vci;        /*!<  new vci ( in fact, it is new inner vlan id )*/
    uint32_t                    out_vlan_tag;   /*!< Out VLAN tag  */
    uint16_t                    dslwan_qid;     /*!< WAN qid  */
    uint16_t                    dest_ifid;      /*!< Destination interface  */

    uint32_t                    flags;          /*!<   Internal flag : SESSION_IS_REPLY, SESSION_IS_TCP, \n
                                                                       SESSION_ADDED_IN_HW, SESSION_CAN_NOT_ACCEL \n
                                                                       SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT, \n
                                                                       SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM, \n
                                                                       SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM, \n
                                                                       SESSION_VALID_PPPOE, SESSION_VALID_NEW_SRC_MAC, \n
                                                                       SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP, \n
                                                                       SESSION_VALID_DSLWAN_QID, \n
                                                                       SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA \n
                                                                       SESSION_LAN_ENTRY, SESSION_WAN_ENTRY,    */                                                                      
    PPA_IFNAME                rx_if_name[PPA_IF_NAME_SIZE]; /*!< receive interface name. Note, in struct session_list_item, rx_if and tx_if is a pointer, so here we have to make a workaround for it. */
    PPA_IFNAME                tx_if_name[PPA_IF_NAME_SIZE]; /*!< txansmit interface name. */
    uint64_t                  mips_bytes;                   /*!< bytes processed by the mips */
    uint64_t                  hw_bytes;                     /*!< bytes proccesed by hareware acceleration unit*/
    uint32_t                  session;                     /*!< PPA SESSION pointer. Note, here we just use its address to delete a session for ioctl*/
} PPA_CMD_SESSION_ENTRY;

/*!
    \brief This is the data structure contains PPA session extra information.
*/
typedef struct {    
    uint32_t          session;                     /*!< PPA SESSION pointer. Note, here we just use its address to modify a session for ioctl*/
    PPA_SESSION_EXTRA  session_extra; /*!< PPA SESSION extra pointer. */ 
    uint32_t                    flags;   /*!<   Internal flag : PPA_F_SESSION_NEW_DSCP \n
                                                            PPA_F_MTU, PPA_F_SESSION_OUT_VLAN, PPA_F_ACCEL_MODE ....\n                                                            
                                                */
}PPA_CMD_SESSION_EXTRA_ENTRY;


typedef struct {    
    uint32_t          session;                     /*!< PPA SESSION pointer. Note, here we just use its address to modify a session for ioctl*/
    int32_t          timer_in_sec; /*!< PPA SESSION polling timer in seconds. */ 
    uint32_t                    flags;   /*!<   Reserved for future */
}PPA_CMD_SESSION_TIMER;


/*!
    \brief This is the data structure for routing session information
*/
typedef struct {
    PPA_CMD_COUNT_INFO      count_info;         /*!< session count */
    PPA_CMD_SESSION_ENTRY   session_list[1];    /*!< Note, here is a dummy array, user need to malloc memory accordingly to the session number */
} PPA_CMD_SESSIONS_INFO;

/*!
    \brief This is the data structure for routing detail session information
*/
typedef struct { 
    uint16_t                    ip_proto;      /*!< IP portocol TCP,UDP.  */
    PPA_IPADDR             src_ip;        /*!< source IP address  */
    uint16_t                    src_port;      /*!< source port  */  
    PPA_IPADDR             dst_ip;        /*!< destination IP address  */ 
    uint16_t                    dst_port;      /*!< destination port  */ 
    PPA_IPADDR             nat_ip;         /*!< IP address to be replaced by NAT if NAT applies */
    uint16_t                    nat_port;       /*!< Port to be replaced by NAT if NAT applies */
    uint32_t                    new_dscp;       /*!< If DSCP remarking required  */
    uint16_t                    in_vci_vlanid;        /*!<  new vci ( in fact, it is new inner vlan id )*/
    uint32_t                    out_vlan_tag;   /*!< Out VLAN tag  */
    uint16_t                    qid;     /*!< WAN qid  */ 
    uint32_t                    flags;          /*!<   Internal flag : SESSION_IS_REPLY, SESSION_IS_TCP, \n
                                                                       SESSION_ADDED_IN_HW, SESSION_CAN_NOT_ACCEL \n
                                                                       SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT, \n
                                                                       SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM, \n
                                                                       SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM, \n
                                                                       SESSION_VALID_PPPOE, SESSION_VALID_NEW_SRC_MAC, \n
                                                                       SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP, \n
                                                                       SESSION_VALID_DSLWAN_QID, \n
                                                                       SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA \n
                                                                       SESSION_LAN_ENTRY, SESSION_WAN_ENTRY,    */                                                                      
    uint32_t                dest_ifid; /*!< txansmit interface name. */
    uint8_t                  src_mac[PPA_ETH_ALEN];    /*!< src mac addres */
    uint8_t                  dst_mac[PPA_ETH_ALEN];    /*!< dst mac address */
    uint16_t                 pppoe_session_id;   /*!< pppoe session id */
    
    uint32_t                 mtu; /*!<  mtu */
}PPA_CMD_SESSIONS_DETAIL_INFO; 
    
/*!
    \brief This is the data structure for basic ppa Versions
*/
typedef struct {
     uint32_t index; /*!< index for PP32 */
     uint32_t family; /*!< ppa version hardware family */
     uint32_t type; /*!< ppa version hardware type */
     uint32_t itf;/*!< ppa version itf */
     uint32_t mode; /*!< ppa version mode */
     uint32_t major; /*!< ppa version major version number */
     uint32_t mid; /*!< ppa version mid version number */
     uint32_t minor;  /*!< ppa version minor version number */
} PPA_VERSION;

/*!
    \brief This is the data structure for ppa wan mode information
*/
typedef struct{
    uint32_t   wan_port_map;   /*!< wan port map information*/   
    uint32_t   mixed;  /*!< mixed flag */       
} PPA_WAN_INFO;

/*!
    \brief This is the data structure for ppa supported feature list information
*/
typedef struct{
    uint8_t   ipv6_en;   /*!< ipv6 enable/disable status */   
    uint8_t   qos_en;   /*!< qos enable/disable status  */       
} PPA_FEATURE_INFO;

/*!
    \brief This is the data structure for PPA subsystem Versions, like ppa subsystem, ppe fw, ppe driver and so on
*/
typedef struct {
    PPA_VERSION ppa_api_ver;               /*!< PPA API verion */
    PPA_VERSION ppa_stack_al_ver;          /*!< PPA stack verion */  
    PPA_VERSION ppe_hal_ver;               /*!< PPA HAL verion */ 
    PPA_VERSION ppe_fw_ver[2];                /*!< PPA FW verion */
    PPA_VERSION ppa_subsys_ver;            /*!< PPA Subsystem verion */
    PPA_WAN_INFO ppa_wan_info;            /*!< PPA WAN INFO */
    PPA_FEATURE_INFO  ppe_fw_feature;  /*!< PPE FW feature lists */
    PPA_FEATURE_INFO  ppa_feature;  /*!< PPA Level feature lists */
    
} PPA_CMD_VERSION_INFO;

/*!
    \brief This is the data structure for basic vlan range
*/
typedef struct  {
    uint32_t start_vlan_range;  /*!< WAN interface start vlan id */
    uint32_t end_vlan_range;    /*!< WAN interface end vlan id */
}PPA_VLAN_RANGE;

/*!
    \brief This is the data structure VLAN range in mixed mode
*/
typedef struct {
    PPA_CMD_COUNT_INFO  count_info;  /*!< PPA Count info */
    PPA_VLAN_RANGE      ranges[1];  /*!< it is dummy array, need to malloc in userspace */
} PPA_CMD_VLAN_RANGES;

/*!
    \brief This is the data structure for MAC INFO 
*/
typedef struct {
    PPA_CMD_COUNT_INFO  count_info;          /*!< PPA Count info */
    PPA_CMD_MAC_ENTRY   session_list[1];    /*!< it is a dummy array, need to malloc bedore use it in userspace */
} PPA_CMD_ALL_MAC_INFO;

/*!
    \brief This is the data structure for BRIGE MAC LEARNING ENABLE/DISABLE INFO 
*/
typedef struct {
    uint32_t        bridge_enable;  /*!< enable/disable bridging mac address learning flag */
    uint32_t        flags;   /*!< reserved for future */
} PPA_CMD_BRIDGE_ENABLE_INFO;

typedef struct { 
    uint32_t   t; /*!<  Time Tick */
    uint32_t   w; /*!<  weight */
    uint32_t   s; /*!<  burst */
    uint32_t   r; /*!<  Replenish */
    uint32_t   d; /*!<  ppe internal variable */
    uint32_t   tick_cnt; /*!<  ppe internal variable */
    uint32_t   b; /*!<  ppe internal variable */

    /*For PPA Level only */
    uint32_t   reg_addr;  /*!<  register address */
    uint32_t bit_rate_kbps;  /*!<  rate shaping in kbps */  
    uint32_t weight_level;   /*!<  internal wfq weight */
    
}PPA_QOS_INTERVAL;

typedef struct  {
    //struct wtx_qos_q_desc_cfg
    uint32_t    threshold; /*!<  qos wtx threshold */
    uint32_t    length;  /*!<  qos wtx length  */
    uint32_t    addr; /*!<  qos wtx address */
    uint32_t    rd_ptr; /*!<  qos wtx read pointer  */
    uint32_t    wr_ptr; /*!<  qos wtx write pointer */

        /*For PPA Level only */
   uint32_t   reg_addr;  /*!<  register address */     
}PPA_QOS_DESC_CFG_INTERNAL;


/*!
    \brief This is the data structure for PPA QOS to get the maximum queue number supported for one physical port
*/
typedef struct {
    uint32_t        portid;   /*!<  the phisical port id which support qos queue */    
    uint32_t        queue_num;  /*!<  the maximum queue number is supported */
    uint32_t        flags;    /*!<  Reserved currently */
} PPA_CMD_QUEUE_NUM_INFO;

/*!
    \brief This is the data structure for PPA QOS MIB Counter
*/
typedef struct {
    uint32_t        total_rx_pkt;   /*!<  all packets received by this queue */
    uint32_t        total_rx_bytes; /*!<  all bytes received by thi queue */
    uint32_t        total_tx_pkt;   /*!<  all packets trasmitted by this queue */
    uint32_t        total_tx_bytes; /*!<  all bytes trasmitted by thi queue */
    
    uint32_t        cpu_path_small_pkt_drop_cnt;  /*!< all small packets dropped in CPU path for lack of TX DMA descriptor in the queue*/
    uint32_t        cpu_path_total_pkt_drop_cnt;  /*!< all packets dropped in CPU path for lack of TX DMA descriptor in the queue*/
    uint32_t        fast_path_small_pkt_drop_cnt; /*!< all small packets dropped in fast path for lack of TX DMA descriptor */
    uint32_t        fast_path_total_pkt_drop_cnt; /*!< all packets dropped in fast path for lack of TX DMA descriptor */
} PPA_QOS_MIB;

/*!
    \brief This is the data structure for PPA QOS to get the maximum queue number supported for one physical port
*/
typedef struct {
    uint32_t        portid; /*!<  the phisical port id which support qos queue */
    uint32_t        queueid;  /*!<  the queue id for the mib */
    PPA_QOS_MIB     mib;    /*!<  the mib information for the current specified queue */
    uint32_t        flags;  /*!<  Reserved currently */
} PPA_CMD_QOS_MIB_INFO;



/*!
    \brief This is the data structure for PPA QOS to be enabled/disabled
*/
typedef struct {
    uint32_t        portid;  /*!<  which support qos queue. */
    uint32_t        enable;  /*!<  enable/disable  flag */
    uint32_t        flags;   /*!<  Reserved currently */
} PPA_CMD_QOS_CTRL_INFO;

/*!
    \brief This is the data structure for PPA Rate Shapping Set/Get/Reset one queue's rate limit
*/
typedef struct {
    uint32_t        portid;   /*!<  the phisical port id which support qos queue */
    uint32_t        queueid;  /*!<  the queu id. Now it only support 0 ~ 7 */
    uint32_t        rate;     /*!<  rate limit in kbps  */
    uint32_t        burst;    /*!<  rate limit in kbps  */
    uint32_t        flags;    /*!<  Reserved currently */
} PPA_CMD_RATE_INFO;



/*!
    \brief This is the data structure for PPA WFQ Set/Get/Reset one queue's weight
*/
typedef struct {
    uint32_t        portid;   /*!<  the phisical port id which support qos queue */
    uint32_t        queueid;  /*!<  the queu id. Now it only support 0 ~ 7 */    
    uint32_t        weight;   /*!<  WFQ weight. The value is from 0 ~ 100 */
    uint32_t        flags;    /*!<  Reserved currently */
} PPA_CMD_WFQ_INFO;

#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
/*!
    \brief Union of ppa power transitin watermark.
*/
union watermark {
        uint32_t ppa_pwm_wm1;  /*!< Watermark value for PPE transition between D0 and D1 */
        uint32_t ppa_pwm_wm2;  /*!< Watermark value for PPE transition between D1 and D2 */
        uint32_t ppa_pwm_wm3;  /*!< Watermark value for PPE transition between D2 and D3 */
};

/*!
    \brief This is the data structure definition for PPA PWM states water mark
*/
typedef struct {
    int16_t flag;   /*!< flag indicating if watermark type. flag=1: watermark is packet count; flag=2: watermark is byte count  */
    int32_t time_interval; /*!< time interval of watermarks in milliseconds. */
    union watermark WM;  /*!< Watermark value for PPE transition */
}WM_t;

/*!
    \brief This is the data structure for PPA Power management basic watermark configuration 
*/
typedef struct {    
    WM_t ppa_pwm_wm1;  /*!< Watermark value for PPE transition between D0 and D1 */
    WM_t ppa_pwm_wm2;  /*!< Watermark value for PPE transition between D1 and D2*/
    WM_t ppa_pwm_wm3;  /*!< Watermark value for PPE transition between D2 and D3*/
}PPA_PWM_WM_t;

/*!
    \brief This is the data structure for PPA Power management configuration
*/
typedef struct {
    uint8_t ppa_pwm;  /*!< PPA power management mode: 0/1-OFF/ON */
    PPA_PWM_WM_t ppa_pwm_wm_up;  /*!< Watermark value for PPE transition for various states. */
    PPA_PWM_WM_t ppa_pwm_wm_down;  /*!< Watermark value for PPE transition for various states. */
    PPA_PWM_STATE_t e_ppa_pwm_init_state;  /*!< Initial power/performance state for PPE */
    uint32_t flag;  /*!< reserved.*/
}PPA_PWM_CONFIG_t;
#endif //end of CONFIG_IFX_PMCU

/*!
    \brief This is the data structure for Mutiple Field Based Classification And VLAN Assigment feature's basic auto-learning VLAN related information.
*/
typedef struct
{
    PPA_IFNAME tx_ifname[PPA_IF_NAME_SIZE]; /*!<  destination interface name, like eth0.3. If blank, then match all interface */
    PPA_IFNAME rx_ifname[PPA_IF_NAME_SIZE]; /*!<  receiving interface name, like eth1.2, If blank, then match all interface */
}PPA_MULTIFIELD_VLAN_INFO_AUTO;

/*!
    \brief This is the data structure for Mutiple Field Based Classification And VLAN Assigment's manual-learning VLAN information based on default key selection.
*/
typedef struct
{
    uint8_t tx_if_id; /*!<  physical destination interface id match, like 0 for eth0, 1 for eth1. It is part of key 14 */
    uint8_t rx_if_id; /*!<  physical destination interface id match, like 0 for eth0, 1 for eth1. It is part of key 15 */
    uint8_t is_vlan; /*!< VLAN Flag match. 1: only match single vlan, 2: match double vlan, 0: match no vlan packet. It is part of key14 */
    uint8_t is_vlan_mask; /*!< VLAN Flag mask*/

    uint8_t out_vlan_pri;  /*!< outer vlan priority ( 3 bits only) match. Based on PPA default Key Selection, it is part of key 8/l2_off14 */
    uint8_t out_vlan_pri_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/
    uint8_t out_vlan_cfi;  /*!< outer vlan cfi ( 1 bits only) match. Based on PPA default Key Selection, it is part of key 8/l2_off14 */
    uint8_t out_vlan_cfi_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/

    uint16_t out_vlan_vid;  /*!< outer vlan id ( 1 bits only) match. Based on PPA default Key Selection, it is part of key 8 and 9/l2_off14-15 */
    uint16_t out_vlan_vid_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/

    uint8_t in_vlan_pri;  /*!< inner vlan priority ( 3 bits only) match. Based on PPA default Key Selection, it is part of key 12/l2_off18 */
    uint8_t in_vlan_pri_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/
    uint8_t in_vlan_cfi;  /*!< inner vlan cfi ( 1 bits only) match. Based on PPA default Key Selection, it is part of key 12/l2_off18 */
    uint8_t in_vlan_cfi_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/

    uint16_t in_vlan_vid;  /*!< inner vlan id ( 1 bits only) match. Based on PPA default Key Selection, it is part of key 12 and 13/l2_off18-19 */
    uint16_t in_vlan_vid_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/

    uint8_t  action_out_vlan_insert;  /*!< out vlan insert action */
    uint8_t  action_in_vlan_insert;    /*!< inner vlan insert action */
    uint8_t  action_out_vlan_remove;   /*!< out vlan remove action */
    uint8_t  action_in_vlan_remove;    /*!< inner vlan remove action */

    uint8_t  new_out_vlan_pri;  /*!< action: new out vlan priority  */
    uint8_t  new_out_vlan_cfi;  /*!< action:new out vlan cfi */
    uint8_t  new_in_vlan_pri;   /*!< action:new inner vlan priority */
    uint8_t  new_in_vlan_cfi;   /*!< action:new inner vlan cfi  */

    uint16_t new_out_vlan_vid;   /*!< action:new out vlan id */
    uint16_t new_in_vlan_vid;      /*!< action:new inner vlan id */ 

    uint16_t new_out_vlan_tpid;   /*!< action:new out vlan tpid */ 
}PPA_MULTIFIELD_VLAN_INFO_MANUAL;

/*!
    \brief This is the data structure for Mutiple Field Based Classification And VLAN Assigment's VLAN KEY/MASK/ACTION based on default key selection.
*/
typedef struct  
{
    uint8_t bfauto;  /*!< flag to use simple autoway to add a multiple field editing flow. It is used by PPA API level and hook/ppacmd only*/
    PPA_MULTIFIELD_VLAN_INFO_AUTO vlan_info_auto;  /*!< auto-learn vlan key/mask/action. Note, it is only for add/delete a rule, not for get commands */
    PPA_MULTIFIELD_VLAN_INFO_MANUAL vlan_info_manual; /*!< munually provide vlan key/mask/action. Even vlan_info_auto is used, ppa will set vlan_info_manual structure for PPE driver */
       
}PPA_MULTIFIELD_VLAN_INFO;

/*!
    \brief This is the data structure for Mutiple Field Based Classification And VLAN Assigment's configuration based on default key selection.
*/
typedef struct  
{                                                  
    uint16_t ether_type;  /*!< ethernet type match, like 0x0800. Based on PPA default Key Selection, it is key0_1*/
    uint16_t ether_type_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/

    uint8_t dscp;  /*!< dscp(tos) match. 1: key of  dscp in ip header, like 0x08. Based on PPA default Key Selection, it is key2*/
    uint8_t dscp_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/
    uint8_t pkt_length;  /*!< packet length ( less than) match. Its value is got from ethernet packet length /64. Based on PPA default Key Selection, it is key3*/
    uint8_t pkt_length_mask;  /*!< packet length mask.*/

    uint32_t s_ip;  /*!< source ip match, like 0x0a000009 ( 10.0.0.9) . Based on PPA default Key Selection, it is key4 ~ Key7*/
    uint32_t s_ip_mask;  /*!< match mask to specify the bits to match. Note, 0 means need to match and 1 means not to match*/

    uint8_t l3_off0;  /*!< L3 Offset 0 match. Baed on PPA default key selection, it is key10 */
    uint8_t l3_off0_mask; /*!< L3 Offset 0 mask */
    uint8_t l3_off1; /*!< L3 Offset 1 match. Baed on PPA default key selection, it is key11 */
    uint8_t l3_off1_mask; /*!< L3 Offset 1 mask */
    
    uint8_t ipv4;  /*!< ipv4 match ( 1 bit). 1: match only ipv4.  0: match none ipv4 packet. It is part of key15. */
    uint8_t ipv4_mask;  /*!< ipv4 mask. 0 -need to match, 1-no need to match */
    uint8_t ipv6;  /*!< < ipv6 match ( 1 bit). 1: match only ipv6.  0: match none ipv6 packet. 1: match only ipv6, 0: match none ipv6 packet. It is part of key15 */
    uint8_t ipv6_mask;  /*!< ipv6 mask. 0 -need to match, 1-no need to match */
    
    uint8_t pppoe_session;  /*!< pppoe session flag match: 1: match pppoe session only. 0-- match none pppoe session packet. It is part of key14 */
    uint8_t pppoe_session_mask;  /*!< pppoe session flag mask. 0 -need to match, 1-no need to match */
    uint8_t fwd_cpu; /*!< action: forward packet to CPU or not. \n
                                       1: forward to CPU.
                                       0: forward to its original destination port\n */
                            
    uint8_t queue_id; /*!< action: which queue assign for the current flow. */

    PPA_MULTIFIELD_VLAN_INFO vlan_info; /*!< specify vlan key/mask/action. Based on PPA default Key Selection. It relates to key8/key9, key 12/key13 and key14/key15 and part of vlan action*/
} PPA_MULTIFIELD_DEFAULT_INFO;

/*!
    \brief This is the data structure for Mutiple Field Based Classification And VLAN Assigment's configuration based on second default key selection.
    \note, for future only now
*/

typedef struct  
{
 
}PPA_MULTIFIELD_DEFAULT2_INFO;

/*!
    \brief This is the data structure for Mutiple Field Based Classification And VLAN Assigment's configuration based on different key selection.
    \note More key selection based configuration will be implemented. Note, different key selectoin may have different configuration.  \n
    PPA should parse the cfg according to current  key selection mode
    
*/
typedef union  
{
    PPA_MULTIFIELD_DEFAULT_INFO cfg0; /*!< multiple field configuration based on default key selection. */
    PPA_MULTIFIELD_DEFAULT2_INFO cfg2; /*!< multiple field configuration based on second default key selection. */
}PPA_MULTIFIELD_FLOW_INFO;

/*!
    \brief This is the data structure for IOCTL of Mutiple Field Based Classification And VLAN Assigment's configuration.
    
*/
typedef struct 
{    
    int32_t index; /*!< for get command, it is input, for add command, it is input. for del, it is input ( index must be valid in this case, -1 means delete all flow ) */
    uint32_t flag; /*!< reserved for future */
    PPA_MULTIFIELD_FLOW_INFO flow;     /*!< the Mutiple Field Based Classification And VLAN Assigment configuration/information */
} PPA_CMD_MULTIFIELD_FLOW_INFO ; 

/*!
    \brief This is the data structure for IOCTL to enable/disable Mutiple Field Based Classification And VLAN Assigment.
    
*/
typedef struct PPA_CMD_ENABLE_MULTIFIELD_INFO
{
    uint32_t enable_multifield;  /*!< flag of enable/disable the Mutiple Field Based Classification And VLAN Assigment feature  */
    uint32_t flag;  /*!< reserved for future */
} PPA_CMD_ENABLE_MULTIFIELD_INFO;

/*!
    \brief This is the data structure for getting all exported PPA hooks.
*/
typedef struct 
{
    uint8_t hookname[MAX_HOOK_NAME_LEN]; /*!< hook name */
    uint32_t hook_addr;   /*!< hook address */
    uint32_t real_func;    /*!< hook pointer to real function*/
    uint8_t hook_flag;   /*!< hooked flag: 0-disabled, 1-enabled */
}PPA_HOOK_INFO;

/*!
    \brief This is the data structure for  PPA hooks list
*/
typedef struct PPA_HOOK_INFO_LIST {
    PPA_HOOK_INFO info;  /*!< ppa hook info */

    struct PPA_HOOK_INFO_LIST *next; /*!< point to next ppa hook info */
} PPA_HOOK_INFO_LIST;

/*!
    \brief This is the data structure for getting all exported PPA hooks.
*/
typedef struct 
{
    uint32_t hook_count; /*!< hook counter */ 
    uint32_t flag; /*!< reserved for future */ 
    PPA_HOOK_INFO list[1];   /*!< it is a dummy array. Userspace should apply storage buffer for it.  */
}PPA_CMD_HOOK_LIST_INFO;

/*!
    \brief This is the data structure for enable/disable ppa hook
*/
typedef struct
{
    uint8_t hookname[MAX_HOOK_NAME_LEN]; /*!< hook name */
    uint32_t enable; /*!< enable/disable ppa hook  */ 
    uint32_t flag; /*!< reserved for future */ 
}PPA_HOOK_ENABLE_INFO;

/*!
    \brief This is the data structure for IOCTL to enable/disable ppa hook
*/
typedef PPA_HOOK_ENABLE_INFO PPA_CMD_HOOK_ENABLE_INFO;

/*!
    \brief This is the data structure to get the memory value.
*/
typedef struct 
{
    uint32_t  addr;  /*!< The memory adddress to read */
    uint32_t  addr_mapped;  /*!< The mapped memory adddress to read */
    uint32_t shift;  /*!< the bits to shitf */
    uint32_t size; /*!< size of bits to read*/
    uint32_t repeat; /*!< read repeat times  */
    uint32_t flag; /*!< reserved for future */ 
    uint32_t buffer[1]; /*!< the buffer to store the value.  it is a dummy array. Userspace should apply storage buffer for it. Its size should be at least size * sizeof (uint32_t) */
}PPA_READ_MEM_INFO;

#ifdef NO_DOXY
typedef PPA_READ_MEM_INFO PPA_CMD_READ_MEM_INFO;

/*!
    \brief This is the data structure to set the memory value.
*/
typedef struct 
{
    uint32_t addr;  /*!< The memory adddress to set */
    uint32_t  addr_mapped;  /*!< The mapped memory adddress to read */
    uint32_t shift;  /*!< the bits to shitf */
    uint32_t size; /*!< size of bits */
    uint32_t value;  /*!< value of the data*/
    uint32_t repeat; /*!< set repeat times*/
    uint32_t flag; /*!< reserved for future */ 
}PPA_SET_MEM_INFO;

/*!
    \brief This is the data structure to set the memory value for IOCTL.
*/
typedef PPA_SET_MEM_INFO PPA_CMD_SET_MEM_INFO;
#endif

/*!
    \brief This is the data structure to get the maximum entries, like lan/wan/mc/bridging
*/
typedef struct 
{
    uint32_t        max_lan_entries;      /*!< Maximum LAN session entries */
    uint32_t        max_wan_entries;      /*!< Maximum WAN session entries */
    uint32_t        max_mc_entries;       /*!< Maximum Multicast session entries */
    uint32_t        max_bridging_entries;/*!< Maximum Bridge session entries */    
    uint32_t        max_ipv6_addr_entries;      /*!< Maximum IPV6 address entries */
    uint32_t        max_fw_queue;                       /*!< Maximum PPE FW queue number */
    uint32_t        max_6rd_entries;       /*!<Maximum 6rd tunnel entries */
} PPA_MAX_ENTRY_INFO;

/*!
    \brief This is the data structure to get the maximum entries for IOCTL.
*/
typedef struct{
    PPA_MAX_ENTRY_INFO entries; /*!< max entry info*/
    uint32_t        flags;    /*!< reserved for future */
} PPA_CMD_MAX_ENTRY_INFO;

/*!
    \brief This is the data structure to get the physical port id For IOCTL.
*/
typedef struct{
    PPA_IFNAME ifname[PPA_IF_NAME_SIZE]; /*!< the interface name */
    uint32_t   portid;   /*!< the port id */   
    uint32_t   flags;    /*!< reserved for future */
} PPA_CMD_PORTID_INFO;


/*!
    \brief This is the data structure for DSL queue mib 
*/
typedef struct{
    uint32_t   queue_num;   /*!< the queue number filled in the drop mib*/   
    uint32_t   drop_mib[16]; /*!< the drop mib counter */

    //later other DSL mib counter will be added here
    uint32_t   flag; /*!< reserved for futurer */    
} PPA_DSL_QUEUE_MIB;

/*!
    \brief This is the data structure for general  ENABLE/DISABLE INFO 
*/
typedef struct {
    uint32_t        enable;  /*!< enable/disable flag */
    uint32_t        flags;   /*!< reserved for future */
} PPA_CMD_GENERAL_ENABLE_INFO;

/*!
    \brief This is the data structure to get the DSL queue drop mib for IOCTL.
*/
typedef struct{
    PPA_DSL_QUEUE_MIB mib;  /*!< dsl queue mib coutner. */
    uint32_t   flags;    /*!< reserved for future */
} PPA_CMD_DSL_MIB_INFO;

/*!
    \brief This is the data structure for PORT mib 
*/
typedef struct{  
    uint32_t     ig_fast_brg_pkts;           /*!<  fast bridge receving packets */
    uint32_t     ig_fast_brg_bytes;          /*!< the fast bridge receving bytes */

    uint32_t     ig_fast_rt_ipv4_udp_pkts;   /*!<  the fast ipv4 routing udp receving packets */
    uint32_t     ig_fast_rt_ipv4_tcp_pkts;   /*!<  the fast ipv4 routing tcp receving packets */
    uint32_t     ig_fast_rt_ipv4_mc_pkts;    /*!<  the fast ipv4 routing multicast receving packets */
    uint32_t     ig_fast_rt_ipv4_bytes;      /*!<  the fast ipv4 receving bytes */

    uint32_t     ig_fast_rt_ipv6_udp_pkts;   /*!<  the fast ipv6 routing udp receving packets*/
    uint32_t     ig_fast_rt_ipv6_tcp_pkts;   /*!<  the fast ipv6 routing tcp receving packets */
    uint32_t     ig_fast_rt_ipv6_bytes;      /*!<  the fast ipv6 routing receving bytes */

    uint32_t     ig_cpu_pkts;                /*!cpu packets */
    uint32_t     ig_cpu_bytes;               /*!<  cpu bytes */

    uint32_t     ig_drop_pkts;               /*!<  drop packets */
    uint32_t     ig_drop_bytes;              /*!<  drop bytes */

    uint32_t     eg_fast_pkts;               /*!<  the fast transmiting packets  */

    uint32_t     port_flag;                      /*!<  port flag: PORT_MODE_ETH, PORT_MODE_DSL, PORT_MODE_EXT  */
    
} PPA_PORT_MIB_INFO;

/*!
    \brief This is the data structure for PORT mib 
*/
typedef struct{  
    PPA_PORT_MIB_INFO mib_info[8];     /*!<  mib array: eth0, eth1, DSL, DSL(IPOA/PPPOA), ext1, ext2, ext3, ext4 */
    uint32_t                    port_num;   /*!< maximum port number supported */
    uint32_t                    flags;   /*!< reserved for future */
}PPA_PORT_MIB;


/*!
    \brief This is the data structure for changing to FPI address
*/
typedef struct{  
    uint32_t                    addr_orig;   /*!< original address */
    uint32_t                    addr_fpi;   /*!< converted FPI address */
    uint32_t                    flags;   /*!< reserved for future */
}PPA_FPI_ADDR;


#ifdef NO_DOXY
typedef struct {
    uint32_t mtu_ix;
    uint32_t mtu;  // for add/del/get mtu entry only 
} PPE_MTU_INFO;

typedef struct {
    uint32_t pppoe_ix;
    uint32_t pppoe_session_id;  // for add/del/get a pppoe entry only 
} PPE_PPPOE_INFO;

typedef struct {
    uint32_t mac_ix;
    uint8_t mac[PPA_ETH_ALEN]; // for add/del/get a MAC entry only 
} PPE_ROUTE_MAC_INFO;

typedef struct {
    uint32_t vlan_id;
    uint32_t vlan_entry;
} PPE_OUT_VLAN_INFO;

typedef struct {
    uint32_t tunnel_idx;
    uint32_t tunnel_type;
    struct net_device *tx_dev;
} PPE_TUNNEL_INFO;

typedef struct {
    uint32_t f_is_lan;
    IP_ADDR_C src_ip;
    uint32_t src_port;
    IP_ADDR_C dst_ip;
    uint32_t dst_port;
    uint32_t f_is_tcp;   //  1: TCP, 0: UDP
    uint32_t route_type;
    IP_ADDR_C new_ip;   //NAT IP
    uint16_t new_port; //NAT UDP/TCP Port
    uint8_t  new_dst_mac[PPA_ETH_ALEN];
    PPE_ROUTE_MAC_INFO src_mac;
    PPE_MTU_INFO mtu_info;
    uint32_t pppoe_mode;
    PPE_PPPOE_INFO pppoe_info;
    PPE_TUNNEL_INFO tnnl_info;

    uint32_t f_new_dscp_enable;
    uint32_t new_dscp;
    uint32_t f_vlan_ins_enable;
    uint32_t new_vci;
    uint32_t f_vlan_rm_enable;

    uint32_t f_out_vlan_ins_enable;
    PPE_OUT_VLAN_INFO out_vlan_info;
    uint32_t f_out_vlan_rm_enable;
    uint32_t dslwan_qid;
    uint32_t dest_list;
    uint32_t entry;
    uint32_t update_flags;  //for update_routing_entry only
    uint64_t bytes;  //for MIB
    uint32_t f_hit;  //only for test_and_clear_hit_stat
} PPE_ROUTING_INFO;


/*!
    \brief This is the data structure to get the PORT queue mib for IOCTL.
*/
typedef struct{
    PPA_PORT_MIB mib;  /*!< port mib counter. */
    uint32_t   flags;    /*!< reserved for future */
} PPA_CMD_PORT_MIB_INFO;

typedef struct{
    uint32_t f_is_lan;
    uint32_t entry_num;
    uint32_t mc_entry_num;
    uint32_t f_ip_verify;
    uint32_t f_tcpudp_verify;
    uint32_t f_tcpudp_err_drop;
    uint32_t f_drop_on_no_hit;
    uint32_t f_mc_drop_on_no_hit;
    uint32_t flags; 
} PPE_ROUTING_CFG;

typedef struct{
    uint32_t entry_num;
    uint32_t br_to_src_port_mask; uint32_t br_to_src_port_en;
    uint32_t f_dest_vlan_en;
    uint32_t f_src_vlan_en;
    uint32_t f_mac_change_drop;
    uint32_t flags;
} PPE_BRDG_CFG;

typedef struct{
    uint32_t mode;
    uint32_t flags;
} PPE_FAST_MODE_CFG;

typedef struct {
    uint32_t        f_is_lan;  
    uint32_t        f_enable; 
    uint32_t        flags;
} PPE_ACC_ENABLE;

typedef struct {
    uint32_t uc_dest_list;
    uint32_t mc_dest_list;
    uint32_t if_no;
} PPE_DEST_LIST;

typedef struct {
    uint32_t if_no;
    uint32_t f_eg_vlan_insert;
    uint32_t f_eg_vlan_remove;
    uint32_t f_ig_vlan_aware;
    uint32_t f_ig_src_ip_based;
    uint32_t f_ig_eth_type_based;
    uint32_t f_ig_vlanid_based;
    uint32_t f_ig_port_based;
    uint32_t f_eg_out_vlan_insert;
    uint32_t f_eg_out_vlan_remove;
    uint32_t f_ig_out_vlan_aware;
} PPE_BRDG_VLAN_CFG;


typedef struct {
    uint32_t entry;  /*so far it is only for get command*/
    uint32_t ig_criteria_type;
    uint32_t ig_criteria;;
    uint32_t new_vci;
    uint32_t dest_qos;
    PPE_OUT_VLAN_INFO out_vlan_info;
    uint32_t in_out_etag_ctrl;
    uint32_t vlan_port_map;
} PPE_BRDG_VLAN_FILTER_MAP;

typedef struct {
    uint32_t dest_ip_compare;
    uint32_t f_vlan_ins_enable;
    uint32_t new_vci;
    uint32_t f_vlan_rm_enable;
    uint32_t f_src_mac_enable;
    uint32_t src_mac_ix;
    uint32_t pppoe_mode;
    uint32_t f_out_vlan_ins_enable;
    PPE_OUT_VLAN_INFO out_vlan_info;
    uint32_t f_out_vlan_rm_enable;
    uint32_t f_new_dscp_enable;
    uint32_t new_dscp;
    uint32_t dest_qid;
    uint32_t dest_list;
    uint32_t route_type;
    uint32_t p_entry;
    uint32_t update_flags;  //only for update only,not for new added one
} PPE_MC_INFO;

typedef struct {
    uint32_t port;
    uint8_t  mac[PPA_ETH_ALEN];
    uint32_t f_src_mac_drop;
    uint32_t dslwan_qid;
    uint32_t dest_list;
    uint32_t p_entry;
    uint32_t f_hit; //only for test_and_clear_bridging_hit_stat
} PPE_BR_MAC_INFO;

struct ppe_itf_mib {
    uint32_t             ig_fast_brg_pkts;           // 0 bridge ?
    uint32_t             ig_fast_brg_bytes;          // 1 ?

    uint32_t             ig_fast_rt_ipv4_udp_pkts;   // 2 IPV4 routing
    uint32_t             ig_fast_rt_ipv4_tcp_pkts;   // 3
    uint32_t             ig_fast_rt_ipv4_mc_pkts;    // 4
    uint32_t             ig_fast_rt_ipv4_bytes;      // 5

    uint32_t             ig_fast_rt_ipv6_udp_pkts;   // 6 IPV6 routing
    uint32_t             ig_fast_rt_ipv6_tcp_pkts;   // 7
    uint32_t             res0;                       // 8
    uint32_t             ig_fast_rt_ipv6_bytes;      // 9

    uint32_t             res1;                       // A
    uint32_t             ig_cpu_pkts;
    uint32_t             ig_cpu_bytes;

    uint32_t             ig_drop_pkts;
    uint32_t             ig_drop_bytes;

    uint32_t             eg_fast_pkts;
};


typedef struct {
    uint32_t itf ; //port
    struct ppe_itf_mib mib;
    uint32_t flag;
} PPE_ITF_MIB_INFO;

typedef struct {
    uint32_t f_enable ; 
    uint32_t flags ; 
} PPE_ENABLE_CFG;

typedef struct {
    int32_t num ; 
    uint32_t flags ; 
} PPE_COUNT_CFG;

typedef struct {
    uint32_t vfitler_type;
    int32_t num ; 
    uint32_t flags ; 
} PPE_VFILTER_COUNT_CFG;

typedef struct {    
    PPA_MULTIFIELD_FLOW_INFO multifield_info;
    uint32_t entry;
    uint32_t flag;
} PPE_MULTIFILED_FLOW;

typedef struct {
    uint32_t portid;
    uint32_t num ; 
    uint32_t flags ; 
} PPE_QOS_COUNT_CFG;

typedef struct {
    uint32_t portid;
    uint32_t queueid;
    PPA_QOS_MIB mib;
    uint32_t reg_addr; 
    uint32_t flag;
} PPE_QOS_MIB_INFO;

typedef struct {
    uint32_t portid;
    uint32_t queueid;
    uint32_t weight_level;
    uint32_t flag;
} PPE_QOS_WFQ_CFG;

typedef struct {
    uint32_t portid;
    uint32_t f_enable;
    uint32_t flag;
} PPE_QOS_ENABLE_CFG;

typedef struct {
    uint32_t portid;
    uint32_t queueid;
    uint32_t rate_in_kbps;
    uint32_t burst;
    uint32_t flag;
} PPE_QOS_RATE_SHAPING_CFG;


typedef struct {
    uint32_t vid;
} PPE_WAN_VID_RANGE;

typedef struct {
    PPA_IFNAME ifname[PPA_IF_NAME_SIZE];     /*!< Name of the stack interface */
    uint32_t    if_flags;   /*!< Flags for Interface. Valid values are below: PPA_F_LAN_IF and PPA_F_WAN_IF */
    uint32_t    port;   /*!< physical port id  for this Interface. Valid values are below: 0 ~  */
} PPE_IFINFO;

#endif  //end of NO_DOXY

typedef struct
{
    uint32_t qos_queue_portid; /*!<  the port id which support qos. at present, only one port can support QOS at run time */

    //port's qos status
    uint32_t time_tick     ; /*!<    number of PP32 cycles per basic time tick */
    uint32_t overhd_bytes  ; /*!<    number of overhead bytes per packet in rate shaping */
    uint32_t eth1_eg_qnum  ; /*!<    maximum number of egress QoS queues; */        
    uint32_t eth1_burst_chk; /*!<    always 1, more accurate WFQ */    
    uint32_t eth1_qss      ; /*!<    1-FW QoS, 0-HW QoS */
    uint32_t shape_en      ; /*!<    1-enable rate shaping, 0-disable */
    uint32_t wfq_en        ; /*!<    1-WFQ enabled, 0-strict priority enabled */
	
    uint32_t tx_qos_cfg_addr; /*!<  qos cfg address */
    uint32_t pp32_clk;   /*!<  pp32 clock  */
    uint32_t basic_time_tick; /*!<  pp32 qos time tick  */
	
    uint32_t wfq_multiple; /*!<  qos wfq multipler  */
    uint32_t wfq_multiple_addr; /*!<  qos wfq multipler address  */
	
    uint32_t wfq_strict_pri_weight; /*!<  strict priority's weight value */
    uint32_t wfq_strict_pri_weight_addr; /*!<  strict priority's weight address  */	

    uint32_t    wan_port_map;  /*!<  wan port interface register value  */
    uint32_t    wan_mix_map;   /*!<  mixed register value  */


    PPA_QOS_INTERVAL qos_port_rate_internal;   /*!<  internal qos port parameters  */

    uint32_t max_buffer_size;  /*!<  must match with below arrays, ie, here set to 8 */
    PPE_QOS_MIB_INFO  mib[8];
    PPA_QOS_INTERVAL queue_internal[8];   /*!<  internal qos queue parameters */
    PPA_QOS_DESC_CFG_INTERNAL desc_cfg_interanl[8];/*!<  internal desc cfg parameters */
  
} PPA_QOS_STATUS;


/*!
    \brief This is the data structure for PPA QOS to get the status
*/
typedef struct {
    PPA_QOS_STATUS qstat; /*!< qos status buffer */
    uint32_t        flags;    /*!<  Reserved currently */
} PPA_CMD_QOS_STATUS_INFO;



/*!
    \brief This is the data structure for IOCTL
*/
typedef union
{
    PPA_CMD_INIT_INFO               init_info;    /*!< PPA init parameter */      
    PPA_CMD_ENABLE_INFO             ena_info;     /*!< PPA enable parameter */      
    PPA_CMD_MC_ENTRY                mc_entry;     /*!< PPA multicast parameter */ 
    PPA_CMD_MAC_ENTRY               mac_entry;    /*!< PPA mac parameter */ 
    PPA_CMD_BR_IF_VLAN_CONFIG       br_vlan;      /*!< PPA bridge parameter */ 
    PPA_CMD_VLAN_FILTER_CONFIG      vlan_filter;  /*!< PPA vlan filter parameter */ 
    PPA_CMD_VLAN_ALL_FILTER_CONFIG  all_vlan_filter;/*!< PPA get all vlan filter list parameter */ 
    PPA_CMD_IF_MAC_INFO             if_mac;       /*!< PPA interface mac parameter */ 
    PPA_CMD_IFINFO                  if_info;      /*!< PPA interface parameter */ 
    PPA_CMD_IFINFOS                 all_if_info;  /*!< PPA all interface list parameter */ 
    PPA_CMD_MC_GROUP_INFO           mc_add_info;   /*!< PPA add multcast parameter */ 
    PPA_CMD_COUNT_INFO              count_info;   /*!< PPA get count parameter */ 
    PPA_CMD_SESSIONS_INFO           session_info; /*!< PPA unicast session parameter */ 
    PPA_CMD_SESSION_EXTRA_ENTRY session_extra_info; /*!< PPA extra session parameter */ 
    PPA_CMD_SESSION_TIMER session_timer_info; /*!< PPA extra session timer parameter */ 
    PPA_CMD_SESSIONS_DETAIL_INFO   detail_session_info; /*!< PPA unicast session detail parameter for add a testing routing acceleration entry into PPE FW directly*/ 
    PPA_CMD_VERSION_INFO            ver;          /*!< PPA version parameter */ 
    PPA_CMD_MC_GROUPS_INFO          mc_groups;    /*!< PPA multicast group parameter */ 
    PPA_CMD_ALL_MAC_INFO            all_br_info;  /*!< PPA all bridge list parameter */ 
    PPA_VLAN_RANGE                  wan_vlanid_range; /*!< PPA wan vlan range parameter */ 
    PPA_CMD_VLAN_RANGES             all_wan_vlanid_range_info; /*!< PPA all wan vlan range list parameter */ 
    PPA_CMD_SIZE_INFO               size_info; /*!< PPA major structure size parameter */ 
    PPA_CMD_BRIDGE_ENABLE_INFO      br_enable_info; /*!< PPA enable/disable bridging mac learning parameter */
#ifdef CONFIG_IFX_PPA_QOS
    PPA_CMD_QOS_STATUS_INFO        qos_status_info; /*!< PPA qos status parameter */
    PPA_CMD_QUEUE_NUM_INFO          qnum_info;  /*!< PPA qos queue parameter */
    PPA_CMD_QOS_CTRL_INFO           qos_ctrl_info;  /*!< PPA qos control parameter */
    PPA_CMD_QOS_MIB_INFO            qos_mib_info; /*!< PPA qos mib parameter */
#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
    PPA_CMD_RATE_INFO               qos_rate_info;  /*!< PPA qos rate shapping parameter */
#endif  //end of CONFIG_IFX_PPA_QOS_RATE_SHAPING
#ifdef CONFIG_IFX_PPA_QOS_WFQ
    PPA_CMD_WFQ_INFO                qos_wfq_info;  /*!< PPA qos wfq parameter */
#endif  //end of CONFIG_IFX_PPA_QOS_WFQ
#endif  //end of CONFIG_IFX_PPA_QOS
    PPA_CMD_HOOK_LIST_INFO          hook_list_info;   /*!< PPA all registered hook list */
    PPA_CMD_HOOK_ENABLE_INFO        hook_control_info; /*!< PPA control information: enable/disable */
#ifdef NO_DOXY
    PPA_CMD_READ_MEM_INFO           read_mem_info;    /*!< PPA read memory info */
    PPA_CMD_SET_MEM_INFO            set_mem_info;  /*!< PPA write memory info */
#endif    
#ifdef CONFIG_IFX_PPA_MFE
    PPA_CMD_ENABLE_MULTIFIELD_INFO  mf_info;        /*!< PPA multiple field edting status info */
    PPA_CMD_MULTIFIELD_FLOW_INFO    mf_flow_info;   /*!< PPA multiple field flow info */
#endif //end of CONFIG_IFX_PPA_MFE

    PPA_CMD_MAX_ENTRY_INFO          acc_entry_info; /*!< PPA maximum entries supported */
    PPA_CMD_PORTID_INFO             portid_info;   /*!< PPA portid from interface name */
    PPA_CMD_DSL_MIB_INFO            dsl_mib_info;  /*!< PPA DSL queue mib counter. At present only drop counter is supported */  
    PPA_CMD_PORT_MIB_INFO           port_mib_info;  /*!< PPA PORT mib counter.  */  
    PPA_CMD_GENERAL_ENABLE_INFO     genernal_enable_info; /*!< General PPA enable/disable info.  */ 
} PPA_CMD_DATA;


/*@}*/ /* PPA_IOCTL */


/* -------------------------------------------------------------------------- */
/*                        IOCTL Command Definitions                           */
/* -------------------------------------------------------------------------- */
/** \addtogroup  PPA_IOCTL */
/*@{*/

/**  PPA IOCTL NR values
*/
typedef enum {
PPA_CMD_INIT_NR=0,             /*!< NR for PPA_CMD_INI  */
PPA_CMD_EXIT_NR,               /*!< NR for PPA_CMD_EXIT  */
PPA_CMD_ENABLE_NR,             /*!< NR for PPA_CMD_ENABLE  */  
PPA_CMD_GET_STATUS_NR,         /*!< NR for PPA_CMD_GET_STATUS  */
PPA_CMD_MODIFY_MC_ENTRY_NR,    /*!< NR for PPA_CMD_MODIFY_MC_ENTRY  */
PPA_CMD_GET_MC_ENTRY_NR,       /*!< NR for  PPA_CMD_GET_MC_ENTRY */ 
PPA_CMD_ADD_MAC_ENTRY_NR,      /*!< NR for PPA_CMD_ADD_MAC_ENTRY  */
PPA_CMD_DEL_MAC_ENTRY_NR,      /*!< NR for  PPA_CMD_DEL_MAC_ENTRY */
PPA_CMD_SET_VLAN_IF_CFG_NR,    /*!< NR for PPA_CMD_SET_VLAN_IF_CFG  */
PPA_CMD_GET_VLAN_IF_CFG_NR,    /*!< NR for PPA_CMD_GET_VLAN_IF_CFG  */
PPA_CMD_ADD_VLAN_FILTER_CFG_NR,         /*!< NR for PPA_CMD_ADD_VLAN_FILTER_CFG  */
PPA_CMD_DEL_VLAN_FILTER_CFG_NR,         /*!< NR for PPA_CMD_DEL_VLAN_FILTER_CFG  */
PPA_CMD_GET_ALL_VLAN_FILTER_CFG_NR,     /*!< NR for PPA_CMD_GET_ALL_VLAN_FILTER_CFG  */
PPA_CMD_DEL_ALL_VLAN_FILTER_CFG_NR,     /*!< NR for PPA_CMD_DEL_ALL_VLAN_FILTER_CFG  */
PPA_CMD_SET_IF_MAC_NR,   /*!< NR for PPA_CMD_SET_IF_MAC  */
PPA_CMD_GET_IF_MAC_NR,   /*!< NR for PPA_CMD_GET_IF_MAC */ 
PPA_CMD_ADD_LAN_IF_NR,   /*!< NR for PPA_CMD_ADD_LAN_IF   */
PPA_CMD_ADD_WAN_IF_NR,   /*!< NR for PPA_CMD_ADD_WAN_IF   */
PPA_CMD_DEL_LAN_IF_NR,   /*!< NR for PPA_CMD_DEL_LAN_IF  */
PPA_CMD_DEL_WAN_IF_NR,   /*!< NR for PPA_CMD_DEL_WAN_IF   */
PPA_CMD_GET_LAN_IF_NR,   /*!< NR for PPA_CMD_GET_LAN_IF  */
PPA_CMD_GET_WAN_IF_NR,   /*!< NR for  PPA_CMD_GET_WAN_IF */
PPA_CMD_ADD_MC_NR,       /*!< NR for  PPA_CMD_ADD_MC */
PPA_CMD_GET_MC_GROUPS_NR,           /*!< NR for PPA_CMD_GET_MC_GROUPS  */
PPA_CMD_GET_COUNT_LAN_SESSION_NR,   /*!< NR for  PPA_CMD_GET_COUNT_LAN_SESSION */
PPA_CMD_GET_COUNT_WAN_SESSION_NR,   /*!< NR for  PPA_CMD_GET_COUNT_WAN_SESSION */
PPA_CMD_GET_COUNT_MC_GROUP_NR,      /*!< NR for PPA_CMD_GET_COUNT_MC_GROUP  */
PPA_CMD_GET_COUNT_VLAN_FILTER_NR,   /*!< NR for PPA_CMD_GET_COUNT_VLAN_FILTER  */
PPA_CMD_GET_LAN_SESSIONS_NR,        /*!< NR for PPA_CMD_GET_LAN_SESSIONS  */  
PPA_CMD_GET_WAN_SESSIONS_NR,        /*!< NR for PPA_CMD_GET_WAN_SESSIONS  */
PPA_CMD_GET_VERSION_NR,             /*!< NR for PPA_CMD_GET_VERSION  */
PPA_CMD_GET_COUNT_MAC_NR,           /*!< NR for PPA_CMD_GET_COUNT_MAC  */ 
PPA_CMD_GET_ALL_MAC_NR,             /*!< NR for PPA_CMD_GET_ALL_MAC  */
PPA_CMD_WAN_MII0_VLAN_RANGE_ADD_NR,       /*!< NR for PPA_CMD_WAN_MII0_VLAN_RANGE_ADD  */
PPA_CMD_WAN_MII0_VLAN_RANGE_GET_NR,       /*!< NR for PPA_CMD_WAN_MII0_VLAN_RANGE_GET  */
PPA_CMD_GET_COUNT_WAN_MII0_VLAN_RANGE_NR, /*!< NR for PPA_CMD_GET_COUNT_WAN_MII0_VLAN_RANGE  */
PPA_CMD_GET_SIZE_NR,                      /*!< NR for PPA_CMD_GET_SIZE   */
PPA_CMD_BRIDGE_ENABLE_NR,                 /*!< NR for PPA_CMD_BRIDGE_ENABLE  */
PPA_CMD_GET_BRIDGE_STATUS_NR,             /*!< NR for PPA_CMD_GET_BRIDGE_STATUS  */
PPA_CMD_GET_QOS_QUEUE_MAX_NUM_NR,         /*!< NR for PPA_CMD_GET_QOS_QUEUE_MAX_NUM  */
PPA_CMD_SET_QOS_WFQ_NR,                   /*!< NR for PPA_CMD_SET_QOS_WFQ  */
PPA_CMD_GET_QOS_WFQ_NR,                   /*!< NR for PPA_CMD_SET_QOS_WFQ  */
PPA_CMD_RESET_QOS_WFQ_NR,                 /*!< NR for PPA_CMD_GET_QOS_WFQ  */
PPA_CMD_ENABLE_MULTIFIELD_NR,             /*!< NR for PPA_CMD_ENABLE_MULTIFIELD  */
PPA_CMD_GET_MULTIFIELD_STATUS_NR,         /*!< NR for PPA_CMD_GET_MULTIFIELD_STATUS  */
PPA_CMD_GET_MULTIFIELD_ENTRY_MAX_NR,      /*!< NR for PPA_CMD_GET_MULTIFIELD_ENTRY_MAX  */
PPA_CMD_GET_MULTIFIELD_KEY_NUM_NR,        /*!< NR for  PPA_CMD_GET_MULTIFIELD_KEY_NUM */
PPA_CMD_ADD_MULTIFIELD_NR,                /*!< NR for PPA_CMD_ADD_MULTIFIELD  */
PPA_CMD_GET_MULTIFIELD_NR,                /*!< NR for  PPA_CMD_GET_MULTIFIELD */
PPA_CMD_DEL_MULTIFIELD_NR,                /*!< NR for PPA_CMD_DEL_MULTIFIELD  */
PPA_CMD_DEL_MULTIFIELD_VIA_INDEX_NR,      /*!< NR for PPA_CMD_DEL_MULTIFIELD_VIA_INDEX  */ 
PPA_CMD_GET_HOOK_COUNT_NR,                /*!< NR for PPA_CMD_GET_HOOK_COUNT  */
PPA_CMD_GET_HOOK_LIST_NR,                 /*!< NR for PPA_CMD_GET_HOOK_LIST  */   
PPA_CMD_SET_HOOK_NR,                      /*!< NR for PPA_CMD_SET_HOOK   */ 
PPA_CMD_READ_MEM_NR,                      /*!< NR for  PPA_CMD_SET_MEM */
PPA_CMD_SET_MEM_NR,                       /*!< NR for PPA_CMD_SET_MEM  */
PPA_CMD_SET_CTRL_QOS_WFQ_NR,              /*!< NR for PPA_CMD_SET_CTRL_QOS_WFQ  */
PPA_CMD_GET_CTRL_QOS_WFQ_NR,              /*!< NR for  PPA_CMD_GET_CTRL_QOS_WFQ */
PPA_CMD_SET_CTRL_QOS_RATE_NR,             /*!< NR for PPA_CMD_SET_CTRL_QOS_RATE   */
PPA_CMD_GET_CTRL_QOS_RATE_NR,             /*!< NR for PPA_CMD_GET_CTRL_QOS_RATE  */
PPA_CMD_SET_QOS_RATE_NR,                  /*!< NR for PPA_CMD_SET_QOS_RATE  */ 
PPA_CMD_GET_QOS_RATE_NR,                  /*!< NR for PPA_CMD_GET_QOS_RATE  */ 
PPA_CMD_RESET_QOS_RATE_NR,                /*!< NR for PPA_CMD_RESET_QOS_RATE  */
PPA_CMD_GET_QOS_MIB_NR,                   /*!< NR for PPA_CMD_GET_QOS_MIB  */
PPA_CMD_GET_MAX_ENTRY_NR,                 /*!< NR for PPA_CMD_GET_MAX_ENTRY  */
PPA_CMD_GET_PORTID_NR,                         /*!< NR for PPA_GET_CMD_PORTID  */
PPA_CMD_GET_DSL_MIB_NR,                        /*!< NR for PPA_GET_DSL_MIB  */
PPA_CMD_CLEAR_DSL_MIB_NR,                      /*!< NR for PPA_CLEAR_DSL_MIB  */
PPA_CMD_DEL_SESSION_NR,                             /*!< NR for PPA_CMD_DEL_SESSION  */
PPA_CMD_ADD_SESSION_NR,                             /*!< NR for PPA_CMD_ADD_SESSION  */
PPA_CMD_MODIFY_SESSION_NR,                             /*!< NR for PPA_CMD_MODIFY_SESSION  */
PPA_CMD_SET_SESSION_TIMER_NR,                             /*!< NR for PPA_CMD_SET_SESSION_TIMER  */
PPA_CMD_GET_SESSION_TIMER_NR,                             /*!< NR for PPA_CMD_GET_SESSION_TIMER  */
PPA_CMD_GET_PORT_MIB_NR,                        /*!< NR for PPA_GET_PORT_MIB  */
PPA_CMD_CLEAR_PORT_MIB_NR,                      /*!< NR for PPA_CLEAR_PORT_MIB  */
PPA_CMD_SET_HAL_DBG_FLAG_NR,                    /*!< NR for PPA_CMD_SET_HAL_DBG_FLAG  */
PPA_CMD_GET_QOS_STATUS_NR,                       /*!< NR for PPA_CMD_GET_QOS_STATUS  */

/*  PPA_IOC_MAXNR should be the last one in the enumberation */    
PPA_IOC_MAXNR                            /*!< NR for PPA_IOC_MAXNR  */
}PPA_IOC_NR;

/**  PPA Initialization Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_INIT_INFO The parameter points to a
          \ref PPA_CMD_INIT_INFO structure   
   \return The return value can be any one of the following: \n
            - IFX_SUCCESS \n
            - IFX_FAILURE 
*/
#define PPA_CMD_INIT                            _IOW(PPA_IOC_MAGIC,  PPA_CMD_INIT_NR,  PPA_CMD_INIT_INFO)



/**  PPA Un-init or exit command. Value is manipulated by _IO() macro for final value
   \return The return value can be any one of the following: \n
            - IFX_SUCCESS \n
            - IFX_FAILURE 
*/
#define PPA_CMD_EXIT                            _IO(PPA_IOC_MAGIC,   PPA_CMD_EXIT_NR)

/**  PPA Acceleration Enable / Disable Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_ENABLE_INFO The parameter points to a
          \ref PPA_CMD_ENABLE_INFO structure  
   \return The return value can be any one of the following: \n
            - IFX_SUCCESS \n
            - IFX_FAILURE         
*/
#define PPA_CMD_ENABLE                          _IOW(PPA_IOC_MAGIC,  PPA_CMD_ENABLE_NR,  PPA_CMD_ENABLE_INFO)

/**  PPA Acceleration Get Status Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_ENABLE_INFO The parameter points to a
          \ref PPA_CMD_ENABLE_INFO structure. Enable or disable configuration status.    
*/
#define PPA_CMD_GET_STATUS                      _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_STATUS_NR,  PPA_CMD_ENABLE_INFO)

/**  PPA Modify Multicast session parameters Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MC_ENTRY The parameter points to a
          \ref PPA_CMD_MC_ENTRY structure   
*/
#define PPA_CMD_MODIFY_MC_ENTRY                 _IOW(PPA_IOC_MAGIC,  PPA_CMD_MODIFY_MC_ENTRY_NR,  PPA_CMD_MC_ENTRY)

/**  PPA Get Multicast session parameters Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_MC_ENTRY The parameter points to a
          \ref PPA_CMD_MC_ENTRY structure   
*/
#define PPA_CMD_GET_MC_ENTRY                    _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_MC_ENTRY_NR,  PPA_CMD_MC_ENTRY)

/**  PPA Add a MAC entry to the bridging MAC table Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MAC_ENTRY The parameter points to a
          \ref PPA_CMD_MAC_ENTRY structure   
*/
#define PPA_CMD_ADD_MAC_ENTRY                   _IOW(PPA_IOC_MAGIC,  PPA_CMD_ADD_MAC_ENTRY_NR,  PPA_CMD_MAC_ENTRY)

/**  PPA Delete a MAC entry to the bridging MAC table Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MAC_ENTRY The parameter points to a
          \ref PPA_CMD_MAC_ENTRY structure   
*/
#define PPA_CMD_DEL_MAC_ENTRY                   _IOW(PPA_IOC_MAGIC,  PPA_CMD_DEL_MAC_ENTRY_NR,  PPA_CMD_MAC_ENTRY)

/**  PPA Set Interface VLAN configuration Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_BR_IF_VLAN_CONFIG The parameter points to a
          \ref PPA_CMD_BR_IF_VLAN_CONFIG structure   
*/
#define PPA_CMD_SET_VLAN_IF_CFG                 _IOW(PPA_IOC_MAGIC,  PPA_CMD_SET_VLAN_IF_CFG_NR,  PPA_CMD_BR_IF_VLAN_CONFIG)

/**  PPA Get All VLAN Filter Configuration Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_BR_IF_VLAN_CONFIG The parameter points to a
          \ref PPA_CMD_BR_IF_VLAN_CONFIG structure   
*/
#define PPA_CMD_GET_VLAN_IF_CFG                 _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_VLAN_IF_CFG_NR,  PPA_CMD_BR_IF_VLAN_CONFIG)

/**  PPA Add VLAN filter configuration Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_VLAN_FILTER_CONFIG The parameter points to a
          \ref PPA_CMD_VLAN_FILTER_CONFIG structure   
*/
#define PPA_CMD_ADD_VLAN_FILTER_CFG             _IOW(PPA_IOC_MAGIC,  PPA_CMD_ADD_VLAN_FILTER_CFG_NR, PPA_CMD_VLAN_FILTER_CONFIG)

/**  PPA Delete VLAN filter configuration Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_VLAN_FILTER_CONFIG The parameter points to a
          \ref PPA_CMD_VLAN_FILTER_CONFIG structure   
*/
#define PPA_CMD_DEL_VLAN_FILTER_CFG             _IOW(PPA_IOC_MAGIC,  PPA_CMD_DEL_VLAN_FILTER_CFG_NR, PPA_CMD_VLAN_FILTER_CONFIG)

/**  PPA Delete all VLAN Filter Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_VLAN_ALL_FILTER_CONFIG The parameter points to a
          \ref PPA_CMD_VLAN_ALL_FILTER_CONFIG structure   
*/
#define PPA_CMD_GET_ALL_VLAN_FILTER_CFG         _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_ALL_VLAN_FILTER_CFG_NR, PPA_CMD_VLAN_ALL_FILTER_CONFIG)

/**  PPA Get All VLAN Filter Configuration Command. Value is manipulated by _IOR() macro for final value
*/
#define PPA_CMD_DEL_ALL_VLAN_FILTER_CFG         _IO(PPA_IOC_MAGIC,   PPA_CMD_DEL_ALL_VLAN_FILTER_CFG_NR)

/**  PA Set Interface MAC address Command. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_IF_MAC_INFO The parameter points to a
          \ref PPA_CMD_IF_MAC_INFO structure   
*/
#define PPA_CMD_SET_IF_MAC                      _IOW(PPA_IOC_MAGIC,  PPA_CMD_SET_IF_MAC_NR, PPA_CMD_IF_MAC_INFO)

/**  PPA Get Interface MAC address Command. Value is anipulated by _IOWR() macro for final value
   \param PPA_CMD_IF_MAC_INFO The parameter points to a
          \ref PPA_CMD_IF_MAC_INFO structure   
*/
#define PPA_CMD_GET_IF_MAC                      _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_IF_MAC_NR, PPA_CMD_IF_MAC_INFO)

/**  PPA Add LAN Interface Command. It is used to register a LAN network interface with the PPA. Value is anipulated by _IOW() macro for final value
   \param PPA_CMD_IFINFO The parameter points to a
          \ref PPA_CMD_IFINFO structure   
*/
#define PPA_CMD_ADD_LAN_IF                      _IOW(PPA_IOC_MAGIC,  PPA_CMD_ADD_LAN_IF_NR, PPA_CMD_IFINFO)

/**  PPA Add WAN interface Command. It is used to register a WN network interface with the PPA. Value is anipulated by _IOW() macro for final value
   \param PPA_CMD_IFINFO The parameter points to a
          \ref PPA_CMD_IFINFO structure   
*/
#define PPA_CMD_ADD_WAN_IF                      _IOW(PPA_IOC_MAGIC,  PPA_CMD_ADD_WAN_IF_NR, PPA_CMD_IFINFO)

/**  A Delete LAN Interface Command. It is used to de-register a LAN network interface. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_IFINFO The parameter points to a
          \ref PPA_CMD_IFINFO structure   
*/
#define PPA_CMD_DEL_LAN_IF                      _IOW(PPA_IOC_MAGIC,  PPA_CMD_DEL_LAN_IF_NR, PPA_CMD_IFINFO)

/**  PA Delete WAN Interface Command. It is used to de-register a WAN network interface. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_IFINFO The parameter points to a
          \ref PPA_CMD_IFINFO structure   
*/
#define PPA_CMD_DEL_WAN_IF                      _IOW(PPA_IOC_MAGIC,  PPA_CMD_DEL_WAN_IF_NR, PPA_CMD_IFINFO)

/**  PA Get all LAN Interface Information Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_IFINFOS The parameter points to a
          \ref PPA_CMD_IFINFOS structure   
*/
#define PPA_CMD_GET_LAN_IF                      _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_LAN_IF_NR, PPA_CMD_IFINFOS)

/**  PPA Get all WAN Interface Information Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_IFINFOS The parameter points to a
          \ref PPA_CMD_IFINFOS structure   
*/
#define PPA_CMD_GET_WAN_IF                      _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_WAN_IF_NR, PPA_CMD_IFINFOS)

/**  PPA Add a Multicast session Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_MC_GROUP_INFO The parameter points to a
          \ref PPA_CMD_MC_GROUP_INFO structure   
*/
#define PPA_CMD_ADD_MC                          _IOR(PPA_IOC_MAGIC,  PPA_CMD_ADD_MC_NR, PPA_CMD_MC_GROUP_INFO)

/**  PPA Get a Multicast session Command. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_MC_GROUPS_INFO The parameter points to a
          \ref PPA_CMD_MC_GROUPS_INFO structure   
*/
#define PPA_CMD_GET_MC_GROUPS                   _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_MC_GROUPS_NR, PPA_CMD_MC_GROUPS_INFO)

/**  PPA Get LAN accelerated session counter. Value is manipulated by _IOWR() macro for final value 
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_COUNT_LAN_SESSION           _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_COUNT_LAN_SESSION_NR, PPA_CMD_COUNT_INFO)

/**  PPA PPA Get WAN accelerated session counter. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_COUNT_WAN_SESSION           _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_COUNT_WAN_SESSION_NR, PPA_CMD_COUNT_INFO)

/**  PPA Get Multicast accelerated  session counter. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_COUNT_MC_GROUP              _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_COUNT_MC_GROUP_NR, PPA_CMD_COUNT_INFO)

/**  PPA Get All VLAN filter count. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_COUNT_VLAN_FILTER           _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_COUNT_VLAN_FILTER_NR, PPA_CMD_COUNT_INFO)

/**  PPA PPA Get LAN accelerated session information. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_SESSIONS_INFO The parameter points to a
          \ref PPA_CMD_SESSIONS_INFO structure   
*/
#define PPA_CMD_GET_LAN_SESSIONS                _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_LAN_SESSIONS_NR, PPA_CMD_SESSIONS_INFO)

/**  PPA Get WAN accelerated session information. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_SESSIONS_INFO The parameter points to a
          \ref PPA_CMD_SESSIONS_INFO structure   
*/
#define PPA_CMD_GET_WAN_SESSIONS                _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_WAN_SESSIONS_NR, PPA_CMD_SESSIONS_INFO)

/**  PPA ADD routing session. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_SESSIONS_DETAIL_INFO The parameter points to a
          \ref PPA_CMD_SESSIONS_DETAIL_INFO structure   
*/
#define PPA_CMD_ADD_SESSION                _IOWR(PPA_IOC_MAGIC, PPA_CMD_ADD_SESSION_NR, PPA_CMD_SESSIONS_DETAIL_INFO)

/**  PPA DEL accelerated routing session. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_SESSIONS_INFO The parameter points to a
          \ref PPA_CMD_SESSIONS_INFO structure   
*/
#define PPA_CMD_DEL_SESSION                _IOWR(PPA_IOC_MAGIC, PPA_CMD_DEL_SESSION_NR, PPA_CMD_SESSIONS_INFO)


/**  PPA DEL accelerated routing session. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_MODIFY_SESSION The parameter points to a
          \ref PPA_CMD_MODIFY_SESSION structure   
*/
#define PPA_CMD_MODIFY_SESSION                _IOWR(PPA_IOC_MAGIC, PPA_CMD_MODIFY_SESSION_NR, PPA_CMD_SESSION_EXTRA_ENTRY)

/**  PPA Set accelerated routing polling timer. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_SET_SESSION_TIMER The parameter points to a
          \ref PPA_CMD_SET_SESSION_TIMER structure   
*/
#define PPA_CMD_SET_SESSION_TIMER                _IOWR(PPA_IOC_MAGIC, PPA_CMD_SET_SESSION_TIMER_NR, PPA_CMD_SESSION_TIMER)


/**  PPA Get accelerated routing polling timer. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_GET_SESSION_TIMER The parameter points to a
          \ref PPA_CMD_GET_SESSION_TIMER structure   
*/
#define PPA_CMD_GET_SESSION_TIMER                _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_SESSION_TIMER_NR, PPA_CMD_SESSION_TIMER)



/**  PPA Get PPA subsystem version. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_VERSION_INFO The parameter points to a
          \ref PPA_CMD_VERSION_INFO structure   
*/
#define PPA_CMD_GET_VERSION                     _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_VERSION_NR, PPA_CMD_VERSION_INFO)

/**  PPA Get bridge mac counter. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_COUNT_MAC                   _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_COUNT_MAC_NR, PPA_CMD_COUNT_INFO)

/**  PPA Get all mac address value. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_ALL_MAC_INFO The parameter points to a
          \ref PPA_CMD_ALL_MAC_INFO structure   
*/
#define PPA_CMD_GET_ALL_MAC                     _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_ALL_MAC_NR, PPA_CMD_ALL_MAC_INFO )

/**  PPA Add VLAN range for WAN interface in mixed mode. Value is manipulated by _IOR() macro for final value
   \param PPA_VLAN_RANGE The parameter points to a
          \ref PPA_VLAN_RANGE structure   
*/
#define PPA_CMD_WAN_MII0_VLAN_RANGE_ADD         _IOR(PPA_IOC_MAGIC,  PPA_CMD_WAN_MII0_VLAN_RANGE_ADD_NR, PPA_VLAN_RANGE)

/**  PPA Get VLAN range for WAN interface in mixed mode. Value is manipulated by _IOR() macro for final value 
   \param PPA_CMD_VLAN_RANGES The parameter points to a
          \ref PPA_CMD_VLAN_RANGES structure   
*/
#define PPA_CMD_WAN_MII0_VLAN_RANGE_GET         _IOR(PPA_IOC_MAGIC,  PPA_CMD_WAN_MII0_VLAN_RANGE_GET_NR, PPA_CMD_VLAN_RANGES)

/**  PPA Get VLAN range count in mixed mode. Value is manipulated by _IOWR() macro for final value 
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_COUNT_WAN_MII0_VLAN_RANGE   _IOWR(PPA_IOC_MAGIC, PPA_CMD_GET_COUNT_WAN_MII0_VLAN_RANGE_NR, PPA_CMD_COUNT_INFO)

/**  PPA Get some information entry size. It is for internal usage. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_SIZE_INFO The parameter points to a
          \ref PPA_CMD_SIZE_INFO structure   
*/
#define PPA_CMD_GET_SIZE                    _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_SIZE_NR, PPA_CMD_SIZE_INFO)

/**  PPA enable/disable ppa bridge mac learning hooks. Value is manipulated by _IOWR() macro for final value
   \param PPA_CMD_BRIDGE_ENABLE_INFO The parameter points to a
          \ref PPA_CMD_BRIDGE_ENABLE_INFO structure   
*/
#define PPA_CMD_BRIDGE_ENABLE                    _IOW(PPA_IOC_MAGIC, PPA_CMD_BRIDGE_ENABLE_NR, PPA_CMD_BRIDGE_ENABLE_INFO)

/**  PPA get ppa bridge mac learning hooks enable/disabe status. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_GET_BRIDGE_STATUS The parameter points to a
          \ref PPA_CMD_GET_BRIDGE_STATUS structure   
*/
#define PPA_CMD_GET_BRIDGE_STATUS          _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_BRIDGE_STATUS_NR, PPA_CMD_BRIDGE_ENABLE_INFO)

#ifdef CONFIG_IFX_PPA_QOS
/** PPA GET QOS status. Value is manipulated by _IOR() macro for final value
    \param[out] PPA_CMD_QOS_STATUS_INFO The parameter points to a
                \ref PPA_CMD_QOS_STATUS_INFO structure 
*/
#define PPA_CMD_GET_QOS_STATUS  _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_QOS_STATUS_NR, PPA_CMD_QOS_STATUS_INFO) 


/** PPA GET the maximum queue supported for WFQ/RateShapping. Value is manipulated by _IOR() macro for final value
    \param[out] PPA_CMD_QUEUE_NUM_INFO The parameter points to a
                \ref PPA_CMD_QUEUE_NUM_INFO structure 
    \note: portid is input parameter, and queue_num  is output value . 
*/
#define PPA_CMD_GET_QOS_QUEUE_MAX_NUM  _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_QOS_QUEUE_MAX_NUM_NR, PPA_CMD_QUEUE_NUM_INFO) 

/** PPA GET the QOS mib counter. Value is manipulated by _IOR() macro for final value
    \param[out] PPA_CMD_QUEUE_NUM_INFO The parameter points to a
                \ref PPA_CMD_QUEUE_NUM_INFO structure 
    \note: portid is input parameter, and queue_num  is output value . 
*/
#define PPA_CMD_GET_QOS_MIB  _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_QOS_MIB_NR, PPA_CMD_QOS_MIB_INFO) 



#ifdef CONFIG_IFX_PPA_QOS_WFQ
/** PPA Enable/Disable QOS WFQ feature. Value is manipulated by _IOW() macro for final value
    \param[in] PPA_CMD_QOS_CTRL_INFO The parameter points to a
                            \ref PPA_CMD_QOS_CTRL_INFO structure 
*/
#define PPA_CMD_SET_CTRL_QOS_WFQ _IOW(PPA_IOC_MAGIC, PPA_CMD_SET_CTRL_QOS_WFQ_NR, PPA_CMD_QOS_CTRL_INFO) 


/** PPA get QOS WFQ feature status: enabled or disabled. Value is manipulated by _IOR() macro for final value
    \param[in] PPA_CMD_QOS_CTRL_INFO The parameter points to a
                            \ref PPA_CMD_QOS_CTRL_INFO structure 
*/
#define PPA_CMD_GET_CTRL_QOS_WFQ _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_CTRL_QOS_WFQ_NR, PPA_CMD_QOS_CTRL_INFO) 

/** PPA Set WFQ weight. Value is manipulated by _IOW() macro for final value
    \param PPA_CMD_WFQ_INFO The parameter points to a
    \ref PPA_CMD_WFQ_INFO structure 
*/
#define PPA_CMD_SET_QOS_WFQ _IOW(PPA_IOC_MAGIC, PPA_CMD_SET_QOS_WFQ_NR, PPA_CMD_WFQ_INFO) 

/** PPA Get WFQ weight. Value is manipulated by _IOR() macro for final value
    \param[out] PPA_CMD_WFQ_INFO The parameter points to a
                     \ref PPA_CMD_WFQ_INFO structure 
    \note portid, queueid and weight should be set accordingly. 
*/
#define PPA_CMD_GET_QOS_WFQ _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_QOS_WFQ_NR, PPA_CMD_WFQ_INFO) 

/** PPA Reset WFQ weight. Value is manipulated by _IOW() macro for final value
    \param[out] PPA_CMD_WFQ_INFO The parameter points to a
                            \ref PPA_CMD_WFQ_INFO structure 
    \note: portid/queueid is input parameter, and weight is output value. 
*/
#define PPA_CMD_RESET_QOS_WFQ _IOW(PPA_IOC_MAGIC, PPA_CMD_RESET_QOS_WFQ_NR, PPA_CMD_WFQ_INFO) 

#endif  //end of CONFIG_IFX_PPA_QOS_WFQ

#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
/** PPA Enable/Disable QOS Rate Shaping feature. Value is manipulated by _IOW() macro for final value
    \param[in] PPA_CMD_QOS_CTRL_INFO The parameter points to a
                            \ref PPA_CMD_QOS_CTRL_INFO structure 
*/
#define PPA_CMD_SET_CTRL_QOS_RATE _IOW(PPA_IOC_MAGIC, PPA_CMD_SET_CTRL_QOS_RATE_NR, PPA_CMD_QOS_CTRL_INFO) 


/** PPA get QOS Rate Shaping feature status: enabled or disabled. Value is manipulated by _IOR() macro for final value
    \param[in] PPA_CMD_QOS_CTRL_INFO The parameter points to a
                            \ref PPA_CMD_QOS_CTRL_INFO structure 
*/
#define PPA_CMD_GET_CTRL_QOS_RATE _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_CTRL_QOS_RATE_NR, PPA_CMD_QOS_CTRL_INFO) 

/** PPA Set QOS rate shaping. Value is manipulated by _IOW() macro for final value
    \param[in] PPA_CMD_RATE_INFO The parameter points to a
    \ref PPA_CMD_RATE_INFO structure 
*/
#define PPA_CMD_SET_QOS_RATE _IOW(PPA_IOC_MAGIC, PPA_CMD_SET_QOS_RATE_NR, PPA_CMD_RATE_INFO) 

/** PPA Get QOS Rate shaping configuration. Value is manipulated by _IOR() macro for final value
    \param[out] PPA_CMD_RATE_INFO The parameter points to a
                     \ref PPA_CMD_RATE_INFO structure 
    \note portid, queueid and weight should be set accordingly. 
*/
#define PPA_CMD_GET_QOS_RATE _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_QOS_RATE_NR, PPA_CMD_RATE_INFO) 

/** PPA Reset QOS Rate shaping. Value is manipulated by _IOW() macro for final value
    \param[in] PPA_CMD_RATE_INFO The parameter points to a
                            \ref PPA_CMD_RATE_INFO structure 
    \note: portid/queueid is input parameter, and weight is output value. 
*/
#define PPA_CMD_RESET_QOS_RATE _IOW(PPA_IOC_MAGIC, PPA_CMD_RESET_QOS_RATE_NR, PPA_CMD_RATE_INFO) 

#endif  //end of CONFIG_IFX_PPA_QOS_RATE_SHAPING

#endif //end of CONFIG_IFX_PPA_QOS

/**  PPA enable multiple field feature. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_ENABLE_MULTIFIELD_INFO The parameter points to a
          \ref PPA_CMD_ENABLE_MULTIFIELD_INFO structure   
*/
#define PPA_CMD_ENABLE_MULTIFIELD     _IOW(PPA_IOC_MAGIC,  PPA_CMD_ENABLE_MULTIFIELD_NR,  PPA_CMD_ENABLE_MULTIFIELD_INFO)

/**  PPA enable multiple field feature. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_ENABLE_MULTIFIELD_INFO The parameter points to a
          \ref PPA_CMD_ENABLE_MULTIFIELD_INFO structure   
*/
#define PPA_CMD_GET_MULTIFIELD_STATUS     _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_MULTIFIELD_STATUS_NR,  PPA_CMD_ENABLE_MULTIFIELD_INFO)

/**  PPA get multiple field max entry number supported. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_MULTIFIELD_ENTRY_MAX     _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_MULTIFIELD_ENTRY_MAX_NR,  PPA_CMD_COUNT_INFO)

/**  PPA get multiple field key number suported per compare set. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_MULTIFIELD_KEY_NUM     _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_MULTIFIELD_KEY_NUM_NR,  PPA_CMD_COUNT_INFO)   #not use at present

/**  PPA add a  multiple field compare set. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MULTIFIELD_FLOW_INFO The parameter points to a
          \ref PPA_CMD_MULTIFIELD_FLOW_INFO structure   
*/
#define PPA_CMD_ADD_MULTIFIELD     _IOW(PPA_IOC_MAGIC,  PPA_CMD_ADD_MULTIFIELD_NR,  PPA_CMD_MULTIFIELD_FLOW_INFO)

/**  PPA get a multiple field flow information. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MULTIFIELD_FLOW_INFO The parameter points to a
          \ref PPA_CMD_MULTIFIELD_FLOW_INFO structure   
*/
#define PPA_CMD_GET_MULTIFIELD     _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_MULTIFIELD_NR,  PPA_CMD_MULTIFIELD_FLOW_INFO)

/**  PPA delete a multiple field compare set according to specified compare key/mask/key_sel information. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MULTIFIELD_FLOW_INFO The parameter points to a
          \ref PPA_CMD_MULTIFIELD_FLOW_INFO structure   
*/
#define PPA_CMD_DEL_MULTIFIELD     _IOW(PPA_IOC_MAGIC,  PPA_CMD_DEL_MULTIFIELD_NR,  PPA_CMD_MULTIFIELD_FLOW_INFO)

/**  PPA delete a multiple field compare set according to specified index. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_MULTIFIELD_FLOW_INFO The parameter points to a
          \ref PPA_CMD_MULTIFIELD_FLOW_INFO structure   
*/
#define PPA_CMD_DEL_MULTIFIELD_VIA_INDEX  _IOW(PPA_IOC_MAGIC,  PPA_CMD_DEL_MULTIFIELD_VIA_INDEX_NR,  PPA_CMD_MULTIFIELD_FLOW_INFO)


/**  PPA get all exported hook count. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_GET_HOOK_COUNT       _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_HOOK_COUNT_NR,  PPA_CMD_COUNT_INFO)

/**  PPA get the exported hook list. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_HOOK_LIST_INFO The parameter points to a
          \ref PPA_CMD_HOOK_LIST_INFO structure   
*/
#define PPA_CMD_GET_HOOK_LIST           _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_HOOK_LIST_NR,  PPA_CMD_HOOK_LIST_INFO)

/**  PPA to enable/disable the exported hook. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_HOOK_ENABLE_INFO The parameter points to a
          \ref PPA_CMD_HOOK_ENABLE_INFO structure   
*/
#define PPA_CMD_SET_HOOK     _IOW(PPA_IOC_MAGIC,  PPA_CMD_SET_HOOK_NR,  PPA_CMD_HOOK_ENABLE_INFO)

#ifdef NO_DOXY
/**  PPA get the memory value. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_MEM_INFO The parameter points to a
          \ref PPA_CMD_MEM_INFO structure   
*/
#define PPA_CMD_READ_MEM         _IOR(PPA_IOC_MAGIC,  PPA_CMD_READ_MEM_NR,  PPA_CMD_READ_MEM_INFO)

/**  PPA to set the memory value. Value is manipulated by _IOW() macro for final value
   \param PPA_CMD_COUNT_INFO The parameter points to a
          \ref PPA_CMD_COUNT_INFO structure   
*/
#define PPA_CMD_SET_MEM     _IOW(PPA_IOC_MAGIC,  PPA_CMD_SET_MEM_NR,  PPA_CMD_SET_MEM_INFO)
#endif

/**  PPA to get the maximum accleration entry number. Value is manipulated by _IOR() macro for final value
    \param PPA_CMD_MAX_ENTRY_INFO The parameter points to a
          \ref PPA_CMD_MAX_ENTRY_INFO structure   
*/
#define PPA_CMD_GET_MAX_ENTRY   _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_MAX_ENTRY_NR,  PPA_CMD_MAX_ENTRY_INFO)

/**  PPA to get the port id of one specified interface name. Value is manipulated by _IOR() macro for final value
    \param PPA_CMD_MAX_ENTRY_INFO The parameter points to a
          \ref PPA_CMD_MAX_ENTRY_INFO structure   
*/
#define PPA_CMD_GET_PORTID   _IOR(PPA_IOC_MAGIC,  PPA_CMD_GET_PORTID_NR,  PPA_CMD_PORTID_INFO)

/**  PPA Get DSL MIB info. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_GET_DSL_MIB The parameter points to a
          \ref PPA_CMD_GET_DSL_MIB structure   
*/
#define PPA_CMD_GET_DSL_MIB                    _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_DSL_MIB_NR, PPA_CMD_DSL_MIB_INFO)

/**  PPA Clear DSL MIB info. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_CLEAR_DSL_MIB The parameter points to a
          \ref PPA_CMD_CLEAR_DSL_MIB structure   
*/
#define PPA_CMD_CLEAR_DSL_MIB                    _IOR(PPA_IOC_MAGIC, PPA_CMD_CLEAR_DSL_MIB_NR, PPA_CMD_DSL_MIB_INFO)

/**  PPA Get PPE port MIB info. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_GET_PORT_MIB The parameter points to a
          \ref PPA_CMD_GET_PORT_MIB structure   
*/
#define PPA_CMD_GET_PORT_MIB                    _IOR(PPA_IOC_MAGIC, PPA_CMD_GET_PORT_MIB_NR, PPA_CMD_PORT_MIB_INFO)

/**  PPA Clear PORT MIB info. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_CLEAR_PORT_MIB The parameter points to a
          \ref PPA_CMD_CLEAR_PORT_MIB structure   
*/
#define PPA_CMD_CLEAR_PORT_MIB                    _IOW(PPA_IOC_MAGIC, PPA_CMD_CLEAR_PORT_MIB_NR, PPA_CMD_PORT_MIB_INFO)

/**  PPA Enable/disable HAL debug flag. Value is manipulated by _IOR() macro for final value
   \param PPA_CMD_SET_HAL_DBG_FLAG The parameter points to a
          \ref PPA_CMD_SET_HAL_DBG_FLAG structure   
*/
#define PPA_CMD_SET_HAL_DBG_FLAG               _IOW(PPA_IOC_MAGIC, PPA_CMD_SET_HAL_DBG_FLAG_NR, PPA_CMD_ENABLE_INFO)

/*@}*/ /* PPA_IOCTL */


/*
 * ####################################
 *             Declaration
 * ####################################
 */

#ifdef __KERNEL__
#ifdef NO_DOXY

#define ENUM_STRING(x)   #x
  void ifx_ppa_subsystem_id(uint32_t *,
                            uint32_t *,
                            uint32_t *,
                            uint32_t *,
                            uint32_t *,
                            uint32_t *,
                            uint32_t *);

  void ifx_ppa_get_api_id(uint32_t *,
                          uint32_t *,
                          uint32_t *,
                          uint32_t *,
                          uint32_t *,
                          uint32_t *,
                          uint32_t *);

  int32_t ifx_ppa_is_init(void);

  int32_t ifx_ppa_init(PPA_INIT_INFO *, uint32_t);
  void ifx_ppa_exit(void);

  int32_t ifx_ppa_enable(uint32_t, uint32_t, uint32_t);
  int32_t ifx_ppa_get_status(uint32_t *, uint32_t *, uint32_t);

  int32_t ifx_ppa_session_add(PPA_BUF *, PPA_SESSION *, uint32_t);
  int32_t ifx_ppa_session_delete(PPA_SESSION *, uint32_t);
  int32_t ifx_ppa_session_modify(PPA_SESSION *, PPA_SESSION_EXTRA *, uint32_t);
  int32_t ifx_ppa_session_get(PPA_SESSION ***, PPA_SESSION_EXTRA **, int32_t *, uint32_t);

  int32_t ifx_ppa_mc_group_update(PPA_MC_GROUP *, uint32_t);
  int32_t ifx_ppa_mc_group_get(IP_ADDR_C, PPA_MC_GROUP *, uint32_t);
  int32_t ifx_ppa_mc_entry_modify(IP_ADDR_C, PPA_MC_GROUP *, PPA_SESSION_EXTRA *, uint32_t);
  int32_t ifx_ppa_mc_entry_get(IP_ADDR_C, PPA_SESSION_EXTRA *, uint32_t);
  int32_t ifx_ppa_multicast_pkt_srcif_add(PPA_BUF *, PPA_NETIF *);

  int32_t ifx_ppa_inactivity_status(PPA_U_SESSION *);
  int32_t ifx_ppa_set_session_inactivity(PPA_U_SESSION *, int32_t);

  int32_t ifx_ppa_bridge_entry_add(uint8_t *, PPA_NETIF *, uint32_t);
  int32_t ifx_ppa_bridge_entry_delete(uint8_t *, uint32_t);
  int32_t ifx_ppa_bridge_entry_hit_time(uint8_t *, uint32_t *);
  int32_t ifx_ppa_bridge_entry_inactivity_status(uint8_t *);
  int32_t ifx_ppa_set_bridge_entry_timeout(uint8_t *, uint32_t);
  int32_t ifx_ppa_hook_bridge_enable(uint32_t f_enable, uint32_t flags);
  int32_t ifx_ppa_hook_get_bridge_status(uint32_t *f_enable, uint32_t flags);

  int32_t ifx_ppa_set_bridge_if_vlan_config(PPA_NETIF *, PPA_VLAN_TAG_CTRL *, PPA_VLAN_CFG *, uint32_t);
  int32_t ifx_ppa_get_bridge_if_vlan_config(PPA_NETIF *, PPA_VLAN_TAG_CTRL *, PPA_VLAN_CFG *, uint32_t);
  int32_t ifx_ppa_vlan_filter_add(PPA_VLAN_MATCH_FIELD *, PPA_VLAN_INFO *, uint32_t);
  int32_t ifx_ppa_vlan_filter_del(PPA_VLAN_MATCH_FIELD *, PPA_VLAN_INFO *, uint32_t);
  int32_t ifx_ppa_vlan_filter_get_all(int32_t *, PPA_VLAN_FILTER_CONFIG *, uint32_t);
  int32_t ifx_ppa_vlan_filter_del_all(uint32_t);

  int32_t ifx_ppa_get_if_stats(PPA_IFNAME *, PPA_IF_STATS *, uint32_t);
  int32_t ifx_ppa_get_accel_stats(PPA_IFNAME *, PPA_ACCEL_STATS *, uint32_t);

  int32_t ifx_ppa_set_if_mac_address(PPA_IFNAME *, uint8_t *, uint32_t);
  int32_t ifx_ppa_get_if_mac_address(PPA_IFNAME *, uint8_t *, uint32_t);

  int32_t ifx_ppa_add_if(PPA_IFINFO *, uint32_t);
  int32_t ifx_ppa_del_if(PPA_IFINFO *, uint32_t);
  int32_t ifx_ppa_get_if(int32_t *, PPA_IFINFO **, uint32_t);

  int32_t ifx_ppa_hook_wan_mii0_vlan_range_add(PPA_VLAN_RANGE *, uint32_t);
  int32_t ifx_ppa_hook_wan_mii0_vlan_range_del(PPA_VLAN_RANGE *, int32_t);
  int32_t ifx_ppa_hook_wan_mii0_vlan_ranges_get(int32_t *, PPA_VLAN_RANGE *, uint32_t);

  int32_t ifx_ppa_get_max_entries(PPA_MAX_ENTRY_INFO *max_entry, uint32_t flags);
  int32_t ppa_ip_comare( PPA_IPADDR ip1, PPA_IPADDR ip2, uint32_t flag );
  int32_t ppa_ip_sprintf( char *buf, PPA_IPADDR ip, uint32_t flag);
#endif //NO_DOXY

#endif //end of  __KERNEL__

#ifdef __KERNEL__
  #include <net/ifx_ppa_hook.h>
#endif


#endif  //   __IFX_PPA_API_H__20081031_1913__

