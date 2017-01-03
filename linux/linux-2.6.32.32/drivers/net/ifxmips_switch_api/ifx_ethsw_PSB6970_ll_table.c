/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include "ifx_types.h"
#include "ifx_ethsw_PSB6970_ll.h"
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

IFX_ll_fkt ifx_ethsw_PSB6970_fkt_ptr_tbl [] =
{
   /* 0x00 */
   (IFX_ll_fkt) IFX_NULL,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_READ ; Index: 0x01 */
   (IFX_ll_fkt) IFX_PSB6970_MAC_TableEntryRead,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_QUERY ; Index: 0x02 */
   (IFX_ll_fkt) IFX_PSB6970_MAC_TableEntryQuery,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_ADD ; Index: 0x03 */
   (IFX_ll_fkt) IFX_PSB6970_MAC_TableEntryAdd,
   /* Command: IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE ; Index: 0x04 */
   (IFX_ll_fkt) IFX_PSB6970_MAC_TableEntryRemove,
   /* Command: IFX_ETHSW_MAC_TABLE_CLEAR ; Index: 0x05 */
   (IFX_ll_fkt) IFX_PSB6970_MAC_TableClear,
   /* Command: IFX_ETHSW_STP_PORT_CFG_SET ; Index: 0x06 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_PSB6970_STP_PortCfgSet),
   /* Command: IFX_ETHSW_STP_PORT_CFG_GET ; Index: 0x07 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_PSB6970_STP_PortCfgGet),
   /* Command: IFX_ETHSW_STP_BPDU_RULE_SET ; Index: 0x08 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_PSB6970_STP_BPDU_RuleSet),
   /* Command: IFX_ETHSW_STP_BPDU_RULE_GET ; Index: 0x09 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_STP , IFX_PSB6970_STP_BPDU_RuleGet),
   /* Command: IFX_ETHSW_8021X_EAPOL_RULE_GET ; Index: 0x0A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_PSB6970_8021X_EAPOL_RuleGet),
   /* Command: IFX_ETHSW_8021X_EAPOL_RULE_SET ; Index: 0x0B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_PSB6970_8021X_EAPOL_RuleSet),
   /* Command: IFX_ETHSW_8021X_PORT_CFG_GET ; Index: 0x0C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_PSB6970_8021X_PortCfgGet),
   /* Command: IFX_ETHSW_8021X_PORT_CFG_SET ; Index: 0x0D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_8021X , IFX_PSB6970_8021X_PortCfgSet),
   /* Command: IFX_ETHSW_VLAN_RESERVED_ADD ; Index: 0x0E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_ReservedAdd),
   /* Command: IFX_ETHSW_VLAN_RESERVED_REMOVE ; Index: 0x0F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_ReservedRemove),
   /* Command: IFX_ETHSW_VLAN_PORT_CFG_GET ; Index: 0x10 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_PortCfgGet),
   /* Command: IFX_ETHSW_VLAN_PORT_CFG_SET ; Index: 0x11 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_PortCfgSet),
   /* Command: IFX_ETHSW_VLAN_ID_CREATE ; Index: 0x12 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_IdCreate),
   /* Command: IFX_ETHSW_VLAN_ID_DELETE ; Index: 0x13 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_IdDelete),
   /* Command: IFX_ETHSW_VLAN_PORT_MEMBER_ADD ; Index: 0x14 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_PortMemberAdd),
   /* Command: IFX_ETHSW_VLAN_PORT_MEMBER_REMOVE ; Index: 0x15 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_PortMemberRemove),
   /* Command: IFX_ETHSW_VLAN_PORT_MEMBER_READ ; Index: 0x16 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_PortMemberRead),
   /* Command: IFX_ETHSW_VLAN_ID_GET ; Index: 0x17 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_VLAN , IFX_PSB6970_VLAN_IdGet),
   /* Command: IFX_ETHSW_QOS_PORT_CFG_SET ; Index: 0x18 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortCfgSet),
   /* Command: IFX_ETHSW_QOS_PORT_CFG_GET ; Index: 0x19 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortCfgGet),
   /* Command: IFX_ETHSW_QOS_DSCP_CLASS_SET ; Index: 0x1A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_DSCP_ClassSet),
   /* Command: IFX_ETHSW_QOS_DSCP_CLASS_GET ; Index: 0x1B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_DSCP_ClassGet),
   /* Command: IFX_ETHSW_QOS_PCP_CLASS_SET ; Index: 0x1C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PCP_ClassSet),
   /* Command: IFX_ETHSW_QOS_PCP_CLASS_GET ; Index: 0x1D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PCP_ClassGet),
   /* 0x1E */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x1F */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x20 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x21 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x22 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x23 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x24 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x25 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x26 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x27 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x28 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x29 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x2A */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x2B */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x2C */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x2D */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x2E */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x2F */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x30 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x31 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x32 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x33 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x34 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x35 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x36 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x37 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x38 */
   (IFX_ll_fkt) IFX_NULL,
   /* 0x39 */
   (IFX_ll_fkt) IFX_NULL,
   /* Command: IFX_ETHSW_MULTICAST_SNOOP_CFG_SET ; Index: 0x3A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastSnoopCfgSet),
   /* Command: IFX_ETHSW_MULTICAST_SNOOP_CFG_GET ; Index: 0x3B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastSnoopCfgGet),
   /* Command: IFX_ETHSW_MULTICAST_ROUTER_PORT_ADD ; Index: 0x3C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastRouterPortAdd),
   /* Command: IFX_ETHSW_MULTICAST_ROUTER_PORT_REMOVE ; Index: 0x3D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastRouterPortRemove),
   /* Command: IFX_ETHSW_MULTICAST_ROUTER_PORT_READ ; Index: 0x3E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastRouterPortRead),
   /* Command: IFX_ETHSW_MULTICAST_TABLE_ENTRY_ADD ; Index: 0x3F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastTableEntryAdd),
   /* Command: IFX_ETHSW_MULTICAST_TABLE_ENTRY_REMOVE ; Index: 0x40 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastTableEntryRemove),
   /* Command: IFX_ETHSW_MULTICAST_TABLE_ENTRY_READ ; Index: 0x41 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_MULTICAST , IFX_PSB6970_MulticastTableEntryRead),
   /* Command: IFX_ETHSW_HW_INIT ; Index: 0x42 */
   (IFX_ll_fkt) IFX_PSB6970_HW_Init,
   /* Command: IFX_ETHSW_VERSION_GET ; Index: 0x43 */
   (IFX_ll_fkt) IFX_PSB6970_VersionGet,
   /* Command: IFX_ETHSW_CAP_GET ; Index: 0x44 */
   (IFX_ll_fkt) IFX_PSB6970_CapGet,
   /* Command: IFX_ETHSW_CFG_SET ; Index: 0x45 */
   (IFX_ll_fkt) IFX_PSB6970_CfgSet,
   /* Command: IFX_ETHSW_CFG_GET ; Index: 0x46 */
   (IFX_ll_fkt) IFX_PSB6970_CfgGet,
   /* Command: IFX_ETHSW_ENABLE ; Index: 0x47 */
   (IFX_ll_fkt) IFX_PSB6970_Enable,
   /* Command: IFX_ETHSW_DISABLE ; Index: 0x48 */
   (IFX_ll_fkt) IFX_PSB6970_Disable,
   /* Command: IFX_ETHSW_PORT_CFG_GET ; Index: 0x49 */
   (IFX_ll_fkt) IFX_PSB6970_PortCfgGet,
   /* Command: IFX_ETHSW_PORT_CFG_SET ; Index: 0x4A */
   (IFX_ll_fkt) IFX_PSB6970_PortCfgSet,
   /* Command: IFX_ETHSW_CPU_PORT_CFG_SET ; Index: 0x4B */
   (IFX_ll_fkt) IFX_PSB6970_CPU_PortCfgSet,
   /* Command: IFX_ETHSW_CPU_PORT_CFG_GET ; Index: 0x4C */
   (IFX_ll_fkt) IFX_PSB6970_CPU_PortCfgGet,
   /* Command: IFX_ETHSW_CPU_PORT_EXTEND_CFG_SET ; Index: 0x4D */
   (IFX_ll_fkt) IFX_PSB6970_CPU_PortExtendCfgSet,
   /* Command: IFX_ETHSW_CPU_PORT_EXTEND_CFG_GET ; Index: 0x4E */
   (IFX_ll_fkt) IFX_PSB6970_CPU_PortExtendCfgGet,
   /* Command: IFX_ETHSW_PORT_LINK_CFG_GET ; Index: 0x4F */
   (IFX_ll_fkt) IFX_PSB6970_PortLinkCfgGet,
   /* Command: IFX_ETHSW_PORT_LINK_CFG_SET ; Index: 0x50 */
   (IFX_ll_fkt) IFX_PSB6970_PortLinkCfgSet,
   /* Command: IFX_ETHSW_PORT_RGMII_CLK_CFG_SET ; Index: 0x51 */
   (IFX_ll_fkt) IFX_PSB6970_PortRGMII_ClkCfgSet,
   /* Command: IFX_ETHSW_PORT_RGMII_CLK_CFG_GET ; Index: 0x52 */
   (IFX_ll_fkt) IFX_PSB6970_PortRGMII_ClkCfgGet,
   /* Command: IFX_ETHSW_PORT_PHY_QUERY ; Index: 0x53 */
   (IFX_ll_fkt) IFX_PSB6970_PortPHY_Query,
   /* Command: IFX_ETHSW_PORT_PHY_ADDR_GET ; Index: 0x54 */
   (IFX_ll_fkt) IFX_PSB6970_PortPHY_AddrGet,
   /* Command: IFX_ETHSW_PORT_REDIRECT_GET ; Index: 0x55 */
   (IFX_ll_fkt) IFX_PSB6970_PortRedirectGet,
   /* Command: IFX_ETHSW_PORT_REDIRECT_SET ; Index: 0x56 */
   (IFX_ll_fkt) IFX_PSB6970_PortRedirectSet,
   /* Command: IFX_ETHSW_MONITOR_PORT_CFG_GET ; Index: 0x57 */
   (IFX_ll_fkt) IFX_PSB6970_MonitorPortCfgGet,
   /* Command: IFX_ETHSW_MONITOR_PORT_CFG_SET ; Index: 0x58 */
   (IFX_ll_fkt) IFX_PSB6970_MonitorPortCfgSet,
   /* Command: IFX_ETHSW_RMON_GET ; Index: 0x59 */
   (IFX_ll_fkt) IFX_PSB6970_RMON_Get,
   /* Command: IFX_ETHSW_RMON_CLEAR ; Index: 0x5A */
   (IFX_ll_fkt) IFX_PSB6970_RMON_Clear,
   /* Command: IFX_ETHSW_MDIO_CFG_GET ; Index: 0x5B */
   (IFX_ll_fkt) IFX_PSB6970_MDIO_CfgGet,
   /* Command: IFX_ETHSW_MDIO_CFG_SET ; Index: 0x5C */
   (IFX_ll_fkt) IFX_PSB6970_MDIO_CfgSet,
   /* Command: IFX_ETHSW_MDIO_DATA_READ ; Index: 0x5D */
   (IFX_ll_fkt) IFX_PSB6970_MDIO_DataRead,
   /* Command: IFX_ETHSW_MDIO_DATA_WRITE ; Index: 0x5E */
   (IFX_ll_fkt) IFX_PSB6970_MDIO_DataWrite,
   /* Command: IFX_ETHSW_WOL_CFG_SET ; Index: 0x5F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_PSB6970_WoL_CfgSet),
   /* Command: IFX_ETHSW_WOL_CFG_GET ; Index: 0x60 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_PSB6970_WoL_CfgGet),
   /* Command: IFX_ETHSW_WOL_PORT_CFG_SET ; Index: 0x61 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_PSB6970_WoL_PortCfgSet),
   /* Command: IFX_ETHSW_WOL_PORT_CFG_GET ; Index: 0x62 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_WOL , IFX_PSB6970_WoL_PortCfgGet),
};

IFX_ETHSW_lowLevelFkts_t ifx_ethsw_PSB6970_fkt_tbl =
{
   IFX_NULL , /* pNext */
   (IFX_uint16_t) IFX_ETHSW_MAGIC , /* nType */
   99 , /* nNumFkts */
   ifx_ethsw_PSB6970_fkt_ptr_tbl /* pFkts */
};

IFX_ll_fkt ifx_psb6970_fkt_ptr_tbl [] =
{
   /* 0x00 */
   (IFX_ll_fkt) IFX_NULL,
   /* Command: IFX_PSB6970_RESET ; Index: 0x01 */
   (IFX_ll_fkt) IFX_PSB6970_Reset,
   /* Command: IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET ; Index: 0x02 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortShaperCfgSet),
   /* Command: IFX_PSB6970_QOS_PORT_SHAPER_CFG_GET ; Index: 0x03 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortShaperCfgGet),
   /* Command: IFX_PSB6970_QOS_PORT_SHAPER_STRICT_SET ; Index: 0x04 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortShaperStrictSet),
   /* Command: IFX_PSB6970_QOS_PORT_SHAPER_STRICT_GET ; Index: 0x05 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortShaperStrictGet),
   /* Command: IFX_PSB6970_QOS_PORT_SHAPER_WFQ_SET ; Index: 0x06 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortShaperWfqSet),
   /* Command: IFX_PSB6970_QOS_PORT_SHAPER_WFQ_GET ; Index: 0x07 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortShaperWfqGet),
   /* Command: IFX_PSB6970_QOS_PORT_POLICER_SET ; Index: 0x08 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortPolicerSet),
   /* Command: IFX_PSB6970_QOS_PORT_POLICER_GET ; Index: 0x09 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_PortPolicerGet),
   /* Command: IFX_PSB6970_QOS_MFC_PORT_CFG_SET ; Index: 0x0A */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_MfcPortCfgSet),
   /* Command: IFX_PSB6970_QOS_MFC_PORT_CFG_GET ; Index: 0x0B */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_MfcPortCfgGet),
   /* Command: IFX_PSB6970_QOS_MFC_ADD ; Index: 0x0C */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_MfcAdd),
   /* Command: IFX_PSB6970_QOS_MFC_DEL ; Index: 0x0D */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_MfcDel),
   /* Command: IFX_PSB6970_QOS_MFC_ENTRY_READ ; Index: 0x0E */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_MfcEntryRead),
   /* Command: IFX_PSB6970_QOS_STORM_SET ; Index: 0x0F */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_StormSet),
   /* Command: IFX_PSB6970_QOS_STORM_GET ; Index: 0x10 */
   (IFX_ll_fkt) IFX_ETHSW_SW_FKT( CONFIG_QOS , IFX_PSB6970_QoS_StormGet),
   /* Command: IFX_PSB6970_POWER_MANAGEMENT_SET ; Index: 0x11 */
   (IFX_ll_fkt) IFX_PSB6970_PowerManagementSet,
   /* Command: IFX_PSB6970_POWER_MANAGEMENT_GET ; Index: 0x12 */
   (IFX_ll_fkt) IFX_PSB6970_PowerManagementGet,
   /* Command: IFX_PSB6970_REGISTER_SET ; Index: 0x13 */
   (IFX_ll_fkt) IFX_PSB6970_RegisterSet,
   /* Command: IFX_PSB6970_REGISTER_GET ; Index: 0x14 */
   (IFX_ll_fkt) IFX_PSB6970_RegisterGet,
};

IFX_ETHSW_lowLevelFkts_t ifx_psb6970_fkt_tbl =
{
   & ifx_ethsw_PSB6970_fkt_tbl , /* pNext */
   (IFX_uint16_t) IFX_PSB6970_MAGIC , /* nType */
   21 , /* nNumFkts */
   ifx_psb6970_fkt_ptr_tbl /* pFkts */
};

