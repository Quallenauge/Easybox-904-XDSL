/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include "ifx_types.h"
#include "ifx_ethsw_flow_ll.h"
#include "ifx_ethsw_ll_fkt.h"

#undef IFX_ETHSW_SW_FKT
#define IFX_ETHSW_SW_FKT(x, y) x? (IFX_ll_fkt)y : IFX_NULL

#ifndef CONFIG_8021X
 #define CONFIG_8021X 1
#endif
#ifndef CONFIG_MULTICAST
 #define CONFIG_MULTICAST 1
#endif
#ifndef CONFIG_QOS
 #define CONFIG_QOS 1
#endif
#ifndef CONFIG_STP
 #define CONFIG_STP 1
#endif
#ifndef CONFIG_VLAN
 #define CONFIG_VLAN 1
#endif
#ifndef CONFIG_WOL
 #define CONFIG_WOL 1
#endif

IFX_ll_fkt ifx_ethsw_FLOW_fkt_ptr_tbl [] =
{
   /* 0x00 */
   (IFX_ll_fkt) IFX_NULL,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_READ ; Index: 0x01 */
   (IFX_ll_fkt) IFX_FLOW_MAC_TableEntryRead,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_QUERY ; Index: 0x02 */
   (IFX_ll_fkt) IFX_FLOW_MAC_TableEntryQuery,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_ADD ; Index: 0x03 */
   (IFX_ll_fkt) IFX_FLOW_MAC_TableEntryAdd,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE ; Index: 0x04 */
   (IFX_ll_fkt) IFX_FLOW_MAC_TableEntryRemove,
   /* Command: IFX_ETHSW_MAC_TABLE_CLEAR ; Index: 0x05 */
   (IFX_ll_fkt) IFX_FLOW_MAC_TableClear,
   /* Command: IFX_ETHSW_STP_PORT_CFG_SET ; Index: 0x06 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_FLOW_STP_PortCfgSet),
   /* Command: IFX_ETHSW_STP_PORT_CFG_GET ; Index: 0x07 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_FLOW_STP_PortCfgGet),
   /* Command: IFX_ETHSW_STP_BPDU_RULE_SET ; Index: 0x08 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_FLOW_STP_BPDU_RuleSet),
   /* Command: IFX_ETHSW_STP_BPDU_RULE_GET ; Index: 0x09 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_FLOW_STP_BPDU_RuleGet),
   /* Command: IFX_ETHSW_8021X_EAPOL_RULE_GET ; Index: 0x0A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_FLOW_8021X_EAPOL_RuleGet),
   /* Command: IFX_ETHSW_8021X_EAPOL_RULE_SET ; Index: 0x0B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_FLOW_8021X_EAPOL_RuleSet),
   /* Command: IFX_ETHSW_8021X_PORT_CFG_GET ; Index: 0x0C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_FLOW_8021X_PortCfgGet),
   /* Command: IFX_ETHSW_8021X_PORT_CFG_SET ; Index: 0x0D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_FLOW_8021X_PortCfgSet),
   /* Command: IFX_ETHSW_VLAN_RESERVED_ADD ; Index: 0x0E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_ReservedAdd),
   /* Command: IFX_ETHSW_VLAN_RESERVED_REMOVE ; Index: 0x0F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_ReservedRemove),
   /* Command: IFX_ETHSW_VLAN_PORT_CFG_GET ; Index: 0x10 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_PortCfgGet),
   /* Command: IFX_ETHSW_VLAN_PORT_CFG_SET ; Index: 0x11 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_PortCfgSet),
   /* Command: IFX_ETHSW_VLAN_ID_CREATE ; Index: 0x12 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_IdCreate),
   /* Command: IFX_ETHSW_VLAN_ID_DELETE ; Index: 0x13 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_IdDelete),
   /* Command: IFX_ETHSW_VLAN_PORT_MEMBER_ADD ; Index: 0x14 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_PortMemberAdd),
   /* Command: IFX_ETHSW_VLAN_PORT_MEMBER_REMOVE ; Index: 0x15 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_PortMemberRemove),
   /* Command: IFX_ETHSW_VLAN_PORT_MEMBER_READ ; Index: 0x16 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_PortMemberRead),
   /* Command: IFX_ETHSW_VLAN_ID_GET ; Index: 0x17 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_FLOW_VLAN_IdGet),
   /* Command: IFX_ETHSW_QOS_PORT_CFG_SET ; Index: 0x18 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_PortCfgSet),
   /* Command: IFX_ETHSW_QOS_PORT_CFG_GET ; Index: 0x19 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_PortCfgGet),
   /* Command: IFX_ETHSW_QOS_DSCP_CLASS_SET ; Index: 0x1A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_DSCP_ClassSet),
   /* Command: IFX_ETHSW_QOS_DSCP_CLASS_GET ; Index: 0x1B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_DSCP_ClassGet),
   /* Command: IFX_ETHSW_QOS_PCP_CLASS_SET ; Index: 0x1C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_PCP_ClassSet),
   /* Command: IFX_ETHSW_QOS_PCP_CLASS_GET ; Index: 0x1D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_PCP_ClassGet),
   /* Command: IFX_ETHSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET ; Index: 0x1E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_DSCP_DropPrecedenceCfgSet),
   /* Command: IFX_ETHSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET ; Index: 0x1F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_DSCP_DropPrecedenceCfgGet),
   /* Command: IFX_ETHSW_QOS_PORT_REMARKING_CFG_SET ; Index: 0x20 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_PortRemarkingCfgSet),
   /* Command: IFX_ETHSW_QOS_PORT_REMARKING_CFG_GET ; Index: 0x21 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_PortRemarkingCfgGet),
   /* Command: IFX_ETHSW_QOS_CLASS_DSCP_SET ; Index: 0x22 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ClassDSCP_Set),
   /* Command: IFX_ETHSW_QOS_CLASS_DSCP_GET ; Index: 0x23 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ClassDSCP_Get),
   /* Command: IFX_ETHSW_QOS_CLASS_PCP_SET ; Index: 0x24 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ClassPCP_Set),
   /* Command: IFX_ETHSW_QOS_CLASS_PCP_GET ; Index: 0x25 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ClassPCP_Get),
   /* Command: IFX_ETHSW_QOS_SHAPER_CFG_SET ; Index: 0x26 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ShaperCfgSet),
   /* Command: IFX_ETHSW_QOS_SHAPER_CFG_GET ; Index: 0x27 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ShaperCfgGet),
   /* Command: IFX_ETHSW_QOS_SHAPER_QUEUE_ASSIGN ; Index: 0x28 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ShaperQueueAssign),
   /* Command: IFX_ETHSW_QOS_SHAPER_QUEUE_DEASSIGN ; Index: 0x29 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ShaperQueueDeassign),
   /* Command: IFX_ETHSW_QOS_SHAPER_QUEUE_GET ; Index: 0x2A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_ShaperQueueGet),
   /* Command: IFX_ETHSW_QOS_WRED_CFG_SET ; Index: 0x2B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_WredCfgSet),
   /* Command: IFX_ETHSW_QOS_WRED_CFG_GET ; Index: 0x2C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_WredCfgGet),
   /* Command: IFX_ETHSW_QOS_WRED_QUEUE_CFG_SET ; Index: 0x2D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_WredQueueCfgSet),
   /* Command: IFX_ETHSW_QOS_WRED_QUEUE_CFG_GET ; Index: 0x2E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_WredQueueCfgGet),
   /* Command: IFX_ETHSW_QOS_METER_CFG_SET ; Index: 0x2F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_MeterCfgSet),
   /* Command: IFX_ETHSW_QOS_METER_CFG_GET ; Index: 0x30 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_MeterCfgGet),
   /* Command: IFX_ETHSW_QOS_METER_PORT_ASSIGN ; Index: 0x31 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_MeterPortAssign),
   /* Command: IFX_ETHSW_QOS_METER_PORT_DEASSIGN ; Index: 0x32 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_MeterPortDeassign),
   /* Command: IFX_ETHSW_QOS_METER_PORT_GET ; Index: 0x33 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_MeterPortGet),
   /* Command: IFX_ETHSW_QOS_STORM_CFG_SET ; Index: 0x34 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_StormCfgSet),
   /* Command: IFX_ETHSW_QOS_STORM_CFG_GET ; Index: 0x35 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_StormCfgGet),
   /* Command: IFX_ETHSW_QOS_SCHEDULER_CFG_SET ; Index: 0x36 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_SchedulerCfgSet),
   /* Command: IFX_ETHSW_QOS_SCHEDULER_CFG_GET ; Index: 0x37 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_SchedulerCfgGet),
   /* Command: IFX_ETHSW_QOS_QUEUE_PORT_SET ; Index: 0x38 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_QueuePortSet),
   /* Command: IFX_ETHSW_QOS_QUEUE_PORT_GET ; Index: 0x39 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_FLOW_QoS_QueuePortGet),
   /* Command: IFX_ETHSW_MULTICAST_SNOOP_CFG_SET ; Index: 0x3A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastSnoopCfgSet),
   /* Command: IFX_ETHSW_MULTICAST_SNOOP_CFG_GET ; Index: 0x3B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastSnoopCfgGet),
   /* Command: IFX_ETHSW_MULTICAST_ROUTER_PORT_ADD ; Index: 0x3C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastRouterPortAdd),
   /* Command: IFX_ETHSW_MULTICAST_ROUTER_PORT_REMOVE ; Index: 0x3D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastRouterPortRemove),
   /* Command: IFX_ETHSW_MULTICAST_ROUTER_PORT_READ ; Index: 0x3E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastRouterPortRead),
   /* Command: IFX_ETHSW_MULTICAST_TABLE_ENTRY_ADD ; Index: 0x3F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastTableEntryAdd),
   /* Command: IFX_ETHSW_MULTICAST_TABLE_ENTRY_REMOVE ; Index: 0x40 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastTableEntryRemove),
   /* Command: IFX_ETHSW_MULTICAST_TABLE_ENTRY_READ ; Index: 0x41 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_FLOW_MulticastTableEntryRead),
   /* Command: IFX_ETHSW_HW_INIT ; Index: 0x42 */
   (IFX_ll_fkt) IFX_FLOW_HW_Init,
   /* Command: IFX_ETHSW_VERSION_GET ; Index: 0x43 */
   (IFX_ll_fkt) IFX_FLOW_VersionGet,
   /* Command: IFX_ETHSW_CAP_GET ; Index: 0x44 */
   (IFX_ll_fkt) IFX_FLOW_CapGet,
   /* Command: IFX_ETHSW_CFG_SET ; Index: 0x45 */
   (IFX_ll_fkt) IFX_FLOW_CfgSet,
   /* Command: IFX_ETHSW_CFG_GET ; Index: 0x46 */
   (IFX_ll_fkt) IFX_FLOW_CfgGet,
   /* Command: IFX_ETHSW_ENABLE ; Index: 0x47 */
   (IFX_ll_fkt) IFX_FLOW_Enable,
   /* Command: IFX_ETHSW_DISABLE ; Index: 0x48 */
   (IFX_ll_fkt) IFX_FLOW_Disable,
   /* Command: IFX_ETHSW_PORT_CFG_GET ; Index: 0x49 */
   (IFX_ll_fkt) IFX_FLOW_PortCfgGet,
   /* Command: IFX_ETHSW_PORT_CFG_SET ; Index: 0x4A */
   (IFX_ll_fkt) IFX_FLOW_PortCfgSet,
   /* Command: IFX_ETHSW_CPU_PORT_CFG_SET ; Index: 0x4B */
   (IFX_ll_fkt) IFX_FLOW_CPU_PortCfgSet,
   /* Command: IFX_ETHSW_CPU_PORT_CFG_GET ; Index: 0x4C */
   (IFX_ll_fkt) IFX_FLOW_CPU_PortCfgGet,
   /* Command: IFX_ETHSW_CPU_PORT_EXTEND_CFG_SET ; Index: 0x4D */
   (IFX_ll_fkt) IFX_FLOW_CPU_PortExtendCfgSet,
   /* Command: IFX_ETHSW_CPU_PORT_EXTEND_CFG_GET ; Index: 0x4E */
   (IFX_ll_fkt) IFX_FLOW_CPU_PortExtendCfgGet,
   /* Command: IFX_ETHSW_PORT_LINK_CFG_GET ; Index: 0x4F */
   (IFX_ll_fkt) IFX_FLOW_PortLinkCfgGet,
   /* Command: IFX_ETHSW_PORT_LINK_CFG_SET ; Index: 0x50 */
   (IFX_ll_fkt) IFX_FLOW_PortLinkCfgSet,
   /* Command: IFX_ETHSW_PORT_RGMII_CLK_CFG_SET ; Index: 0x51 */
   (IFX_ll_fkt) IFX_FLOW_PortRGMII_ClkCfgSet,
   /* Command: IFX_ETHSW_PORT_RGMII_CLK_CFG_GET ; Index: 0x52 */
   (IFX_ll_fkt) IFX_FLOW_PortRGMII_ClkCfgGet,
   /* Command: IFX_ETHSW_PORT_PHY_QUERY ; Index: 0x53 */
   (IFX_ll_fkt) IFX_FLOW_PortPHY_Query,
   /* Command: IFX_ETHSW_PORT_PHY_ADDR_GET ; Index: 0x54 */
   (IFX_ll_fkt) IFX_FLOW_PortPHY_AddrGet,
   /* Command: IFX_ETHSW_PORT_REDIRECT_GET ; Index: 0x55 */
   (IFX_ll_fkt) IFX_FLOW_PortRedirectGet,
   /* Command: IFX_ETHSW_PORT_REDIRECT_SET ; Index: 0x56 */
   (IFX_ll_fkt) IFX_FLOW_PortRedirectSet,
   /* Command: IFX_ETHSW_MONITOR_PORT_CFG_GET ; Index: 0x57 */
   (IFX_ll_fkt) IFX_FLOW_MonitorPortCfgGet,
   /* Command: IFX_ETHSW_MONITOR_PORT_CFG_SET ; Index: 0x58 */
   (IFX_ll_fkt) IFX_FLOW_MonitorPortCfgSet,
   /* Command: IFX_ETHSW_RMON_GET ; Index: 0x59 */
   (IFX_ll_fkt) IFX_FLOW_RMON_Get,
   /* Command: IFX_ETHSW_RMON_CLEAR ; Index: 0x5A */
   (IFX_ll_fkt) IFX_FLOW_RMON_Clear,
   /* Command: IFX_ETHSW_MDIO_CFG_GET ; Index: 0x5B */
   (IFX_ll_fkt) IFX_FLOW_MDIO_CfgGet,
   /* Command: IFX_ETHSW_MDIO_CFG_SET ; Index: 0x5C */
   (IFX_ll_fkt) IFX_FLOW_MDIO_CfgSet,
   /* Command: IFX_ETHSW_MDIO_DATA_READ ; Index: 0x5D */
   (IFX_ll_fkt) IFX_FLOW_MDIO_DataRead,
   /* Command: IFX_ETHSW_MDIO_DATA_WRITE ; Index: 0x5E */
   (IFX_ll_fkt) IFX_FLOW_MDIO_DataWrite,
   /* Command: IFX_ETHSW_WOL_CFG_SET ; Index: 0x5F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_FLOW_WoL_CfgSet),
   /* Command: IFX_ETHSW_WOL_CFG_GET ; Index: 0x60 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_FLOW_WoL_CfgGet),
   /* Command: IFX_ETHSW_WOL_PORT_CFG_SET ; Index: 0x61 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_FLOW_WoL_PortCfgSet),
   /* Command: IFX_ETHSW_WOL_PORT_CFG_GET ; Index: 0x62 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_FLOW_WoL_PortCfgGet),
};

IFX_ETHSW_lowLevelFkts_t ifx_ethsw_FLOW_fkt_tbl =
{
   IFX_NULL , /* pNext */
   (IFX_uint16_t) IFX_ETHSW_MAGIC , /* nType */
   99 , /* nNumFkts */
   ifx_ethsw_FLOW_fkt_ptr_tbl /* pFkts */
};

IFX_ll_fkt ifx_flow_fkt_ptr_tbl [] =
{
   /* 0x00 */
   (IFX_ll_fkt) IFX_NULL,
   /* Command: IFX_FLOW_REGISTER_SET ; Index: 0x01 */
   (IFX_ll_fkt) IFX_FLOW_RegisterSet,
   /* Command: IFX_FLOW_REGISTER_GET ; Index: 0x02 */
   (IFX_ll_fkt) IFX_FLOW_RegisterGet,
   /* Command: IFX_FLOW_IRQ_MASK_GET ; Index: 0x03 */
   (IFX_ll_fkt) IFX_FLOW_IrqMaskGet,
   /* Command: IFX_FLOW_IRQ_MASK_SET ; Index: 0x04 */
   (IFX_ll_fkt) IFX_FLOW_IrqMaskSet,
   /* Command: IFX_FLOW_IRQ_GET ; Index: 0x05 */
   (IFX_ll_fkt) IFX_FLOW_IrqGet,
   /* Command: IFX_FLOW_IRQ_STATUS_CLEAR ; Index: 0x06 */
   (IFX_ll_fkt) IFX_FLOW_IrqStatusClear,
   /* Command: IFX_FLOW_PCE_RULE_WRITE ; Index: 0x07 */
   (IFX_ll_fkt) IFX_FLOW_PceRuleWrite,
   /* Command: IFX_FLOW_PCE_RULE_READ ; Index: 0x08 */
   (IFX_ll_fkt) IFX_FLOW_PceRuleRead,
   /* Command: IFX_FLOW_PCE_RULE_DELETE ; Index: 0x09 */
   (IFX_ll_fkt) IFX_FLOW_PceRuleDelete,
   /* Command: IFX_FLOW_RESET ; Index: 0x0A */
   (IFX_ll_fkt) IFX_FLOW_Reset,
   /* Command: IFX_FLOW_RMON_EXTEND_GET ; Index: 0x0B */
   (IFX_ll_fkt) IFX_FLOW_RMON_ExtendGet,
};

IFX_ETHSW_lowLevelFkts_t ifx_flow_fkt_tbl =
{
   & ifx_ethsw_FLOW_fkt_tbl , /* pNext */
   (IFX_uint16_t) IFX_FLOW_MAGIC , /* nType */
   12 , /* nNumFkts */
   ifx_flow_fkt_ptr_tbl /* pFkts */
};

