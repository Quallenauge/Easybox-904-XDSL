/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _IFX_FLOW_FLOW_H_
#define _IFX_FLOW_FLOW_H_

#include "ifx_types.h"
#include "ifx_ethsw.h"

/* =================================== */
/* Global typedef forward declarations */
/* =================================== */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* IOCTL MAGIC */
#define IFX_FLOW_MAGIC ('F')

/* Group definitions for Doxygen */
/** \defgroup FLOW_IOCTL XWAY VRX200 and XWAY GRX200 Family Specific Ethernet Switch API
    This chapter describes the XWAY VRX200 and XWAY GRX200 Family specific
    interfaces for accessing and configuring the services of the Ethernet switch module.
    These features are a device-specific enhancement of the generic Switch API part. */
/*@{*/

/** \defgroup FLOW_IOCTL_DEBUG Debug Features
    XWAY VRX200 and XWAY GRX200 Family specific features for system
    integration and debug sessions.
*/
/** \defgroup FLOW_IOCTL_IRQ Interrupt Handling
    Configure XWAY VRX200 and XWAY GRX200 Family specific hardware
    support to generate interrupts
    and read out the interrupt sources.
*/
/** \defgroup FLOW_IOCTL_OAM Operation, Administration, and Management Functions
    This chapter summarizes the functions that are provided to monitor the
    data traffic passing through the device.
*/
/** \defgroup FLOW_IOCTL_CLASSUNIT Packet Classification Engine
    Configures and controls the classification unit of the XWAY VRX200
    and XWAY GRX200 Family hardware.
*/

/*@}*/

/* -------------------------------------------------------------------------- */
/*                 Structure and Enumeration Type Defintions                  */
/* -------------------------------------------------------------------------- */

/** \addtogroup FLOW_IOCTL_DEBUG */
/*@{*/

/** Register access parameter to directly read or write switch
    internal registers.
    Used by \ref IFX_FLOW_REGISTER_SET and \ref IFX_FLOW_REGISTER_GET. */
typedef struct
{
   /** Register Address Offset for read or write access. */
   IFX_uint16_t                                   nRegAddr;
   /** Value to write to or read from 'nRegAddr'. */
   IFX_uint16_t                                   nData;
}IFX_FLOW_register_t;

/*@}*/ /* FLOW_IOCTL_DEBUG */

/** \addtogroup FLOW_IOCTL_IRQ */
/*@{*/

/** Interrupt Source Selector.
    Used by \ref IFX_FLOW_irq_t. */
typedef enum
{
   /** Wake-on-LAN Interrupt.
       The parameter 'nPortId' specifies the relative MAC port. */
   IFX_FLOW_IRQ_WOL                               = 0,
   /** Port Limit Alert Interrupt. This interrupt is asserted when the number
       of learned MAC addresses exceeds the configured limit for
       the ingress port.
       The parameter 'nPortId' specifies the relative MAC port. */
   IFX_FLOW_IRQ_LIMIT_ALERT                       = 1,
   /** Port Lock Alert Interrupt.
       This interrupt is asserted when a source MAC address is learned on a
       locked port and is received on another port.
       The parameter 'nPortId' specifies the relative MAC port. */
   IFX_FLOW_IRQ_LOCK_ALERT                        = 2
}IFX_FLOW_irqSrc_t;

/** Interrupt bits. Depending on the hardware device type, not all interrupts might be available.
    Used by \ref IFX_FLOW_IRQ_MASK_GET, \ref IFX_FLOW_IRQ_MASK_SET,
    \ref IFX_FLOW_IRQ_GET and \ref IFX_FLOW_IRQ_STATUS_CLEAR. */
typedef struct
{
   /** Ethernet Port number (zero-based counting). The valid range is hardware dependent.
       An error code is delivered if the selected port is not
       available. This port parameter is needed for some interrupts
       that are specified by 'nIrqSrc'. For all other interrupts, this
       parameter is "don't care". */
   IFX_uint32_t                                   nPortId;
   /** Interrupt source. */
   IFX_FLOW_irqSrc_t                              eIrqSrc;
}IFX_FLOW_irq_t;

/*@}*/ /* FLOW_IOCTL_IRQ */

/** \addtogroup FLOW_IOCTL_CLASSUNIT */
/*@{*/

/** Rule selection for IPv4/IPv6.
    Used by \ref IFX_FLOW_PCE_pattern_t. */
typedef enum
{
   /** Rule Pattern for IP selection disabled. */
   IFX_FLOW_PCE_IP_DISABLED                       = 0,
   /** Rule Pattern for IPv4. */
   IFX_FLOW_PCE_IP_V4                             = 1,
   /** Rule Pattern for IPv6. */
   IFX_FLOW_PCE_IP_V6                             = 2
}IFX_FLOW_PCE_IP_t;

/** Packet Classification Engine Pattern Configuration.
    Used by \ref IFX_FLOW_PCE_rule_t. */
typedef struct
{
   /** Index */
   IFX_uint32_t                                   nIndex;

   /** Index is used (enabled) or set to unused (disabled) */
   IFX_boolean_t                                  bEnable;

   /** Port ID used */
   IFX_boolean_t                                  bPortIdEnable;
   /** Port ID */
   IFX_uint8_t                                    nPortId;

   /** DSCP value used */
   IFX_boolean_t                                  bDSCP_Enable;
   /** DSCP value */
   IFX_uint8_t                                    nDSCP;

   /** PCP value used */
   IFX_boolean_t                                  bPCP_Enable;
   /** PCP value */
   IFX_uint8_t                                    nPCP;

   /** Packet length used */
   IFX_boolean_t                                  bPktLngEnable;
   /** Packet length */
   IFX_uint16_t                                   nPktLng;
   /** Packet length Range */
   IFX_uint16_t                                   nPktLngRange;

   /** Destination MAC address used */
   IFX_boolean_t                                  bMAC_DstEnable;
   /** Destination MAC address */
   IFX_uint8_t                                    nMAC_Dst[6];
   /** Destination MAC address mask */
   IFX_uint16_t                                   nMAC_DstMask;

   /** Source MAC address used */
   IFX_boolean_t                                  bMAC_SrcEnable;
   /** Source MAC address */
   IFX_uint8_t                                    nMAC_Src[6];
   /** Source MAC address mask */
   IFX_uint16_t                                   nMAC_SrcMask;

   /** MSB Application field used */
   IFX_boolean_t                                  bAppDataMSB_Enable;
   /** MSB Application field.
       The first 2 bytes of the packet content following the IP header
       for TCP/UDP packets (source port field), or the first 2 bytes of packet content
       following the Ethertype for non-IP packets. Any part of this
       content can be masked-out by a programmable bit
       mask 'nAppMaskRangeMSB'. */
   IFX_uint16_t                                   nAppDataMSB;
   /** MSB Application mask/range selection.
       If set to IFX_TRUE, the field 'nAppMaskRangeMSB' is used as a
       range parameter, otherwise it is used as a nibble mask field. */
   IFX_boolean_t                                  bAppMaskRangeMSB_Select;
   /** MSB Application mask/range. When used as a range parameter,
       1 bit represents 1 nibble mask of the 'nAppDataMSB' field. */
   IFX_uint16_t                                   nAppMaskRangeMSB;

   /** LSB Application used */
   IFX_boolean_t                                  bAppDataLSB_Enable;
   /** LSB Application field.
       The following 2 bytes of the packet behind the 'nAppDataMSB' field.
       This is the destination port field for TCP/UDP packets,
       or byte 3 and byte 4 of the packet content following the Ethertype
       for non-IP packets. Any part of this content can be masked-out
       by a programmable bit mask 'nAppMaskRangeLSB'. */
   IFX_uint16_t                                   nAppDataLSB;
   /** LSB Application mask/range selection.
       If set to IFX_TRUE, the field 'nAppMaskRangeLSB' is used as
       a range parameter, otherwise it is used as a nibble mask field. */
   IFX_boolean_t                                  bAppMaskRangeLSB_Select;
   /** LSB Application mask/range. When used as a range parameter,
       1 bit represents 1 nibble mask of the 'nAppDataLSB' field. */
   IFX_uint16_t                                   nAppMaskRangeLSB;

   /** DIP Selection. */
   IFX_FLOW_PCE_IP_t                              eDstIP_Select;
   /** DIP */
   IFX_ETHSW_IP_t                                 nDstIP;
   /** DIP Nibble Mask.
       1 bit represents 1 nibble mask of the 'nDstIP' field. */
   IFX_uint32_t                                   nDstIP_Mask;

   /** SIP Selection. */
   IFX_FLOW_PCE_IP_t                              eSrcIP_Select;
   /** SIP */
   IFX_ETHSW_IP_t                                 nSrcIP;
   /** SIP Nibble Mask.
       1 bit represents 1 nibble mask of the 'nSrcIP' field. */
   IFX_uint32_t                                   nSrcIP_Mask;

   /** Ethertype used. */
   IFX_boolean_t                                  bEtherTypeEnable;
   /** Ethertype */
   IFX_uint16_t                                   nEtherType;
   /** Ethertype Mask.
       (1 bit represents 1 nibble mask of the 'nEtherType' field) */
   IFX_uint16_t                                   nEtherTypeMask;

   /** IP protocol used */
   IFX_boolean_t                                  bProtocolEnable;
   /** IP protocol */
   IFX_uint8_t                                    nProtocol;
   /** IP protocol Mask.
       1 bit represents 1 nibble mask of the 'nProtocol' field. */
   IFX_uint8_t                                    nProtocolMask;

   /** PPPoE used. */
   IFX_boolean_t                                  bSessionIdEnable;
   /** PPPoE */
   IFX_uint16_t                                   nSessionId;

   /** VLAN used. */
   IFX_boolean_t                                  bVid;
   /** VLAN */
   IFX_uint16_t                                   nVid;
}IFX_FLOW_PCE_pattern_t;

/** IGMP Snooping Control.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disabled. IGMP Snooping is disabled. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_DISABLE         = 0,
   /** Default. Regular Packet. No IGMP Snooping action required. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REGULAR         = 1,
   /** IGMP Report/Join Message. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REPORT          = 2,
   /** IGMP Leave Message. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_LEAVE           = 3,
   /**  Router Solicitation/Advertisement message. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_AD              = 4,
   /** IGMP Query Message. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY           = 5,
   /** IGMP Group Specific Query Message. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP     = 6,
   /** IGMP General Query message without Router Solicitation. */
   IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER = 7
}IFX_FLOW_PCE_ActionIGMP_Snoop_t;

/** MAC Address Learning control.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** MAC Address Learning action is disabled. MAC address learning is
       not influenced by this rule. */
   IFX_FLOW_PCE_ACTION_LEARNING_DISABLE           = 0,
   /** Learning is based on the forwarding decision. If the packet is discarded,
       the address is not learned. If the packet is forwarded to any egress port,
       the address is learned. */
   IFX_FLOW_PCE_ACTION_LEARNING_REGULAR           = 1,
   /** Force No Learning. The address is not learned; forwarding decision
       ignored. */
   IFX_FLOW_PCE_ACTION_LEARNING_FORCE_NOT         = 2,
   /** Force Learning. The address is learned, the forwarding decision ignored.
       Note: The MAC Learning Control signals delivered to Port-Map filtering
       and combined with Final Forwarding Decision. The result is used as a
       feedback for MAC Address learning in the Bridging Table. */
   IFX_FLOW_PCE_ACTION_LEARNING_FORCE             = 3
}IFX_FLOW_PCE_ActionLearning_t;

/** Flow Meter Assignment control.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Action Disable. */
   IFX_FLOW_PCE_ACTION_METER_DISABLE              = 0,
   /** Action Enable. Assign Flow-Meter-Num to Meter-ID-0. */
   IFX_FLOW_PCE_ACTION_METER_1                    = 1,
   /** Action Enable. Assign Flow-Meter-Num to Meter-ID-1. */
   IFX_FLOW_PCE_ACTION_METER_2                    = 2,
   /** Action Enable. Assign pair of meters: Flow-Meter-Num to Meter-ID-0.
       Flow-Meter-Num+1 to Meter-ID-1. */
   IFX_FLOW_PCE_ACTION_METER_1_2                  = 3
}IFX_FLOW_PCE_ActionMeter_t;

/** Traffic Class Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disabled. Traffic class action is disabled. */
   IFX_FLOW_PCE_ACTION_TRAFFIC_CLASS_DISABLE      = 0,
   /** Regular Class. Traffic class action is enabled and the CoS
       classification traffic class is used. */
   IFX_FLOW_PCE_ACTION_TRAFFIC_CLASS_REGULAR      = 1,
   /** Alternative Class. Traffic class action is enabled and the
       class of the 'nTrafficClassAlter' field is used. */
   IFX_FLOW_PCE_ACTION_TRAFFIC_CLASS_ALTERNATIVE = 2,
}IFX_FLOW_PCE_ActionTrafficClass_t;

/** Interrupt Control Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disabled. Interrupt Control Action is disabled for this rule. */
   IFX_FLOW_PCE_ACTION_IRQ_DISABLE                = 0,
   /** Regular Packet. The Interrupt Control Action is enabled, the packet is
       treated as a regular packet and no interrupt event is generated. */
   IFX_FLOW_PCE_ACTION_IRQ_REGULAR                = 1,
   /** Interrupt Event. The Interrupt Control Action is enabled and an
       interrupt event is generated. */
   IFX_FLOW_PCE_ACTION_IRQ_EVENT                  = 2
}IFX_FLOW_PCE_ActionIrq_t;

/** Cross State Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disable. The Cross State Action is disabled. */
   IFX_FLOW_PCE_ACTION_CROSS_STATE_DISABLE        = 0,
   /** Regular Packet. The Cross State Action is enabled and the packet is
       treated as a non-Cross-State packet (regular packet). Therefore it does
       not ignore Port-State filtering rules. */
   IFX_FLOW_PCE_ACTION_CROSS_STATE_REGULAR        = 1,
   /** Cross-State packet. The Cross State Action is enabled and the packet is
       treated as a Cross-State packet. It ignores the Port-State
       filtering rules. */
   IFX_FLOW_PCE_ACTION_CROSS_STATE_CROSS          = 2
}IFX_FLOW_PCE_ActionCrossState_t;

/** Critical Frame Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disable. The Critical Frame Action is disabled. */
   IFX_FLOW_PCE_ACTION_CRITICAL_FRAME_DISABLE     = 0,
   /** Regular Packet. The Critical Frame Action is enabled and the packet is
       treated as a non-Critical Frame. */
   IFX_FLOW_PCE_ACTION_CRITICAL_FRAME_REGULAR     = 1,
   /** Critical Packet. The Critical Frame Action is enabled and the packet is
       treated as a Critical Frame. */
   IFX_FLOW_PCE_ACTION_CRITICAL_FRAME_CRITICAL    = 2
}IFX_FLOW_PCE_ActionCriticalFrame_t;

/** Timestamp Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disable. Timestamp Action is disabled for this rule. */
   IFX_FLOW_PCE_ACTION_TIMESTAMP_DISABLE          = 0,
   /** Regular Packet. The Timestamp Action is enabled for this rule.
       The packet is treated as a regular packet and no timing information
       is stored. */
   IFX_FLOW_PCE_ACTION_TIMESTAMP_REGULAR          = 1,
   /** Receive/Transmit Timing packet. Ingress and Egress Timestamps for
       this packet should be stored. */
   IFX_FLOW_PCE_ACTION_TIMESTAMP_STORED           = 2
}IFX_FLOW_PCE_ActionTimestamp_t;

/** Forwarding Group Action Selector.
    This flow table action and the 'bFlowID_Action' action
    can be used exclusively.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disable. Forwarding Group Action is disabled. */
   IFX_FLOW_PCE_ACTION_PORTMAP_DISABLE            = 0,
   /** Regular Packet. Forwarding Action enabled. Select Default
       Port-Map (result of Default Forwarding Classification). */
   IFX_FLOW_PCE_ACTION_PORTMAP_REGULAR            = 1,
   /** Discard. Discard the packets. */
   IFX_FLOW_PCE_ACTION_PORTMAP_DISCARD            = 2,
   /** Forward to the CPU port. This requires that the CPU port is previously
       set by calling \ref IFX_ETHSW_CPU_PORT_CFG_SET. */
   IFX_FLOW_PCE_ACTION_PORTMAP_CPU                = 3,
   /** Forward to a portmap, selected by the parameter 'nForwardPortMap'.
       Please note that this feature is not supported by all
       hardware platforms. */
   IFX_FLOW_PCE_ACTION_PORTMAP_ALTERNATIVE        = 4,
   /** The packet is treated as Multicast Router
       Solicitation/Adverticement or Query packet. */
   IFX_FLOW_PCE_ACTION_PORTMAP_MULTICAST_ROUTER   = 5,
   /** The packet is interpreted as Multicast packet and learned in the
       multicast group table. */
   IFX_FLOW_PCE_ACTION_PORTMAP_MULTICAST_HW_TABLE = 6
}IFX_FLOW_PCE_ActionPortmap_t;

/** VLAN Group Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disabled. The VLAN Action is disabled. */
   IFX_FLOW_PCE_ACTION_VLAN_DISABLE               = 0,
   /** Regular VLAN. VLAN Action enabled. Select Default VLAN ID. */
   IFX_FLOW_PCE_ACTION_VLAN_REGULAR               = 1,
   /** Alternative VLAN. VLAN Action enabled.
       Select Alternative VLAN as configured in 'nVLAN_Id'. It requires that
       this VLAN ID is configured by
       calling \ref IFX_ETHSW_VLAN_ID_CREATE in advance. */
   IFX_FLOW_PCE_ACTION_VLAN_ALTERNATIVE           = 2
}IFX_FLOW_PCE_ActionVLAN_t;

/** Cross VLAN Action Selector.
    Used by \ref IFX_FLOW_PCE_action_t. */
typedef enum
{
   /** Disabled. The Cross VLAN Action is disabled. */
   IFX_FLOW_PCE_ACTION_CROSS_VLAN_DISABLE         = 0,
   /** Regular VLAN Packet. Do not ignore VLAN filtering rules. */
   IFX_FLOW_PCE_ACTION_CROSS_VLAN_REGULAR         = 1,
   /** Cross-VLAN packet. Ignore VLAN filtering  rules.*/
   IFX_FLOW_PCE_ACTION_CROSS_VLAN_CROSS           = 2
}IFX_FLOW_PCE_ActionCrossVLAN_t;

/** Packet Classification Engine Action Configuration.
    Used by \ref IFX_FLOW_PCE_rule_t. */
typedef struct
{
   /** Action "Traffic Class" Group.
       Traffic class action enable */
   IFX_FLOW_PCE_ActionTrafficClass_t              eTrafficClassAction;
   /** Alternative Traffic class */
   IFX_uint8_t                                    nTrafficClassAlternate;

   /** Action "IGMP Snooping" Group.
       IGMP Snooping control and enable. Please note that the 'nPortMapAction'
       configuration is ignored in case the IGMP snooping is enabled.
       Here, on read operations,
       'nPortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_DISABLE' is returned. */
   IFX_FLOW_PCE_ActionIGMP_Snoop_t                eSnoopingTypeAction;

   /** Action "Learning" Group.
       Learning action control and enable */
   IFX_FLOW_PCE_ActionLearning_t                  eLearningAction;

   /** Action "Interrupt" Group.
       Interrupt action generate and enable */
   IFX_FLOW_PCE_ActionIrq_t                       eIrqAction;

   /** Action "Cross State" Group.
       Cross state action control and enable */
   IFX_FLOW_PCE_ActionCrossState_t                eCrossStateAction;

   /** Action "Critical Frames" Group.
       Critical Frame action control and enable */
   IFX_FLOW_PCE_ActionCriticalFrame_t             eCritFrameAction;

   /** Action "Timestamp" Group. Time stamp action control and enable */
   IFX_FLOW_PCE_ActionTimestamp_t                 eTimestampAction;

   /** Action "Forwarding" Group.
       Port map action enable. This port forwarding configuration is ignored
       in case the action "IGMP Snooping" is enabled via the
       parameter 'nSnoopingTypeAction'. */
   IFX_FLOW_PCE_ActionPortmap_t                   ePortMapAction;
   /** Target portmap for forwarded packets, only used if selected by
       'nPortMapAction'. Forwarding is done
       if 'nPortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_ALTERNATIVE'.
       Every bit in the portmap represents one port (port 0 = LSB bit). */
   IFX_uint32_t                                   nForwardPortMap;

   /** Action "Remarking" Group. Remarking action enable */
   IFX_boolean_t                                  bRemarkAction;
   /** PCP remarking enable.
       Remarking enabling means that remarking is possible in case
       the port configuration or metering enables remarking on that
       packet. Disabling remarking means that it is forced to
       not remarking this packet, independent of any port remarking of
       metering configuration. */
   IFX_boolean_t                                  bRemarkPCP;
   /** DSCP remarking enable
       Remarking enabling means that remarking is possible in case
       the port configuration or metering enables remarking on that
       packet. Disabling remarking means that it is forced to
       not remarking this packet, independent of any port remarking of
       metering configuration. */
   IFX_boolean_t                                  bRemarkDSCP;
   /** Class remarking enable
       Remarking enabling means that remarking is possible in case
       the port configuration or metering enables remarking on that
       packet. Disabling remarking means that it is forced to
       not remarking this packet, independent of any port remarking of
       metering configuration. */
   IFX_boolean_t                                  bRemarkClass;

   /** Action "Meter" Group. Meter action control and enable. */
   IFX_FLOW_PCE_ActionMeter_t                     eMeterAction;
   /** Meter ID */
   IFX_uint8_t                                    nMeterId;

   /** Action "RMON" Group. RMON action enable */
   IFX_boolean_t                                  bRMON_Action;
   /** Counter ID (The index starts counting from zero). */
   IFX_uint8_t                                    nRMON_Id;

   /** Action "VLAN" Group. VLAN action enable */
   IFX_FLOW_PCE_ActionVLAN_t                      eVLAN_Action;
   /** Alternative VLAN Id */
   IFX_uint16_t                                   nVLAN_Id;

   /** Action "Cross VLAN" Group. Cross VLAN action enable */
   IFX_FLOW_PCE_ActionCrossVLAN_t                 eVLAN_CrossAction;

   /** Action "Flow ID".
       The Switch supports enhancing the egress packets by a device specific
       special tag header. This header contains detailed switch classification
       results. One header file is a 'Flow ID', which can be explicite set as
       flow table action when hitting a table rule.
       If selected, the Flow ID is given by the parameter 'nFlowID'. */
   IFX_boolean_t                                  bFlowID_Action;

   /** Flow ID */
   IFX_uint16_t                                   nFlowID;
}IFX_FLOW_PCE_action_t;

/** Parameter to add/read a rule to/from the packet classification engine.
    Used by \ref IFX_FLOW_PCE_RULE_WRITE and \ref IFX_FLOW_PCE_RULE_READ. */
typedef struct
{
   /** Rule Pattern Part. */
   IFX_FLOW_PCE_pattern_t                         pattern;
   /** Rule Action Part. */
   IFX_FLOW_PCE_action_t                          action;
}IFX_FLOW_PCE_rule_t;

/** Parameter to delete a rule from the packet classification engine.
    Used by \ref IFX_FLOW_PCE_RULE_DELETE. */
typedef struct
{
   /** Action Index in the Packet Classification Engine.
       It corresponds to the table rule index. */
   IFX_uint32_t                                   nIndex;
}IFX_FLOW_PCE_ruleDelete_t;

/*@}*/ /* FLOW_IOCTL_CLASSUNIT */

/** \addtogroup FLOW_IOCTL_OAM */
/*@{*/

/** Reset selection.
    Used by \ref IFX_FLOW_reset_t. */
typedef enum
{
   /** Switch Macro reset */
   IFX_FLOW_RESET_SWITCH                          = 0,
   /** MDIO master interface reset */
   IFX_FLOW_RESET_MDIO                            = 1,
}IFX_FLOW_resetMode_t;

/** Reset selection.
    Used by \ref IFX_FLOW_RESET. */
typedef struct
{
   /** Reset selection. */
   IFX_FLOW_resetMode_t                           eReset;
}IFX_FLOW_reset_t;

/** Number of extended RMON counter. */
#define IFX_FLOW_RMON_EXTEND_NUM                    24

/**
   Hardware platform extended RMON Counters.
   This structure contains additional RMON counters of one Ethernet Switch Port.
   These counters can be used by the packet classification engine and can be
   freely assigned to dedicated packet rules and flows.
    Used by \ref IFX_FLOW_RMON_EXTEND_GET. */
typedef struct
{
   /** Ethernet Port number (zero-based counting). The valid range is hardware
       dependent. An error code is delivered if the selected port is not
       available. */
   IFX_uint8_t                                    nPortId;
   /** Traffic flow counters */
   IFX_uint32_t                                   nTrafficFlowCnt[IFX_FLOW_RMON_EXTEND_NUM];
}IFX_FLOW_RMON_extendGet_t;

/*@}*/ /* FLOW_IOCTL_OAM */

/* -------------------------------------------------------------------------- */
/*                        IOCTL Command Definitions                           */
/* -------------------------------------------------------------------------- */

/** \addtogroup FLOW_IOCTL_DEBUG */
/*@{*/

/**
   Write to an internal register. The register offset defines which register to access in
   which table. This routine only accesses the M4599_PDI and
   the ETHSW_PDI of the switch. All PHY registers are accessed
   via \ref IFX_ETHSW_MDIO_DATA_WRITE and \ref IFX_ETHSW_MDIO_DATA_READ.
   Note that the switch API implementation checks whether the given address is
   inside the valid address range. It returns with an error in case an invalid
   address is given.

   \param IFX_FLOW_register_t Pointer to \ref IFX_FLOW_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_FLOW_REGISTER_SET     _IOW(IFX_FLOW_MAGIC, 0x01, IFX_FLOW_register_t)

/**
   Read an internal register. The register offset defines which register to access in
   which table. This routine only accesses the M4599_PDI and
   the ETHSW_PDI of the switch. All PHY registers are accessed
   via \ref IFX_ETHSW_MDIO_DATA_WRITE and \ref IFX_ETHSW_MDIO_DATA_READ.
   Note that the switch API implementation checks whether the given address is
   inside the valid address range. It returns with an error in case an invalid
   address is given.

   \param IFX_FLOW_register_t Pointer to \ref IFX_FLOW_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_FLOW_REGISTER_GET     _IOWR(IFX_FLOW_MAGIC, 0x02, IFX_FLOW_register_t)

/*@}*/ /* FLOW_IOCTL_DEBUG */

/** \addtogroup FLOW_IOCTL_IRQ */
/*@{*/
/**
   Get the interrupt enable configuration. This assignment can be set using \ref IFX_FLOW_IRQ_MASK_SET.

   \param IFX_FLOW_irq_t Pointer to
      an \ref IFX_FLOW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
#define IFX_FLOW_IRQ_MASK_GET     _IOWR(IFX_FLOW_MAGIC, 0x03, IFX_FLOW_irq_t)

/**
   Set the interrupt enable configuration. This assignment can be read using \ref IFX_FLOW_IRQ_MASK_GET.
   Setting interrupts that are not supported by hardware results in an error response.

   \param IFX_FLOW_irq_t Pointer to
      an \ref IFX_FLOW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
#define IFX_FLOW_IRQ_MASK_SET     _IOW(IFX_FLOW_MAGIC, 0x04, IFX_FLOW_irq_t)

/**
   Read the interrupt status. Interrupt status indications can be cleared using \ref IFX_FLOW_IRQ_STATUS_CLEAR.

   \param IFX_FLOW_irq_t Pointer to
      an \ref IFX_FLOW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
#define IFX_FLOW_IRQ_GET          _IOWR(IFX_FLOW_MAGIC, 0x05, IFX_FLOW_irq_t)

/**
   Clear individual interrupt status bits. Interrupt status indications can be read using \ref IFX_FLOW_IRQ_GET.

   \param IFX_FLOW_irq_t Pointer to
      an \ref IFX_FLOW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
#define IFX_FLOW_IRQ_STATUS_CLEAR _IOW(IFX_FLOW_MAGIC, 0x06, IFX_FLOW_irq_t)

/*@}*/ /* FLOW_IOCTL_IRQ */

/** \addtogroup FLOW_IOCTL_CLASSUNIT */
/*@{*/

/**
   This command writes a rule pattern and action to the table of the packet
   classification engine. The pattern part describes the parameter to identify an
   incoming packet to which the dedicated actions should be applied.
   A rule can be read using the command \ref IFX_FLOW_PCE_RULE_READ.

   \param IFX_FLOW_PCE_rule_t Pointer to \ref IFX_FLOW_PCE_rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_FLOW_PCE_RULE_WRITE   _IOW(IFX_FLOW_MAGIC, 0x07, IFX_FLOW_PCE_rule_t)

/**
   This command allows the reading out of a rule pattern and action of the
   packet classification engine.
   A rule can be written using the command \ref IFX_FLOW_PCE_RULE_WRITE.

   \param IFX_FLOW_PCE_rule_t Pointer to \ref IFX_FLOW_PCE_rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_FLOW_PCE_RULE_READ    _IOWR(IFX_FLOW_MAGIC, 0x08, IFX_FLOW_PCE_rule_t)

/**
   This command deletes a complete rule from the packet classification engine.
   A delete operation is done on the rule of a dedicated index 'nIndex'.
   A rule can be written over using the command \ref IFX_FLOW_PCE_RULE_WRITE.

   \param IFX_FLOW_PCE_ruleDelete_t Pointer to \ref IFX_FLOW_PCE_ruleDelete_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_FLOW_PCE_RULE_DELETE  _IOW(IFX_FLOW_MAGIC, 0x09, IFX_FLOW_PCE_ruleDelete_t)

/*@}*/ /* FLOW_IOCTL_CLASSUNIT */

/** \addtogroup FLOW_IOCTL_OAM */
/*@{*/

/**
   Forces a hardware reset of the switch device or switch macro. The device
   automatically comes back out of reset and contains the initial values.
   All previous configurations are lost.

   \param IFX_FLOW_reset_t Pointer to an \ref IFX_FLOW_reset_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

   \remarks Not supported for all devices
*/
#define IFX_FLOW_RESET            _IOW(IFX_FLOW_MAGIC, 0x0A, IFX_FLOW_reset_t)

/**
   Read out additional traffic flow (RMON) counters.
   The zero-based 'nPortId' structure element describes the physical switch
   port for the requested statistic information.

   \param IFX_FLOW_RMON_extendGet_t  Pointer to a pre-allocated
   \ref IFX_FLOW_RMON_extendGet_t structure. The structure element 'nPortId' is
   an input parameter that describes from which port to read the RMON counter.
   All remaining structure elements are filled with the counter values.
   The counter assignment needs to be done during the flow definition,
   for example in \ref IFX_FLOW_PCE_RULE_WRITE.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_FLOW_RMON_EXTEND_GET  _IOWR(IFX_FLOW_MAGIC, 0x0B, IFX_FLOW_RMON_extendGet_t)

/*@}*/ /* FLOW_IOCTL_OAM */

#endif /* _IFX_FLOW_FLOW_H_ */
