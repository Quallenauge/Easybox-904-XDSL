/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_api.h"
   \remarks
 *****************************************************************************/

#ifndef _IFX_SWITCH_API_H
#define _IFX_SWITCH_API_H

#include "ifx_ethsw.h"
#include "ifx_ethsw_PSB6970.h"
#include "ifx_ethsw_ll_fkt.h"
#include "regmapper.h"
#include "Tantos3G.h"
#include "AR9.h"
#include "ifx_ethsw_flow.h"
#include "VR9_switch.h"
#include "VR9_top.h"

#ifdef IFXOS_SUPPORT
	#define LINUX
    /** This is the unsigned long datatype.
    On 32bit systems it is 4 byte wide.
    */
	typedef unsigned long   IFX_ulong_t;
    /** This is the size data type (32 or 64 bit) */
	typedef IFX_ulong_t     IFX_size_t;
	#include "ifxos_print.h"
	#include "ifxos_thread.h"
	#include "ifxos_memory_alloc.h"
#else
#ifdef __KERNEL__
    #include <asm/delay.h>
    #include <linux/slab.h>
    #include <linux/string.h>
#endif
#endif

#ifndef IFXOS_SUPPORT
    #define IFXOS_PRINT_INT_RAW(fmt, args...)       printk(fmt, ##args)
    #define IFXOS_DBG_PRINT_USR(fmt, args...)       printk(KERN_DEBUG fmt "\r", ##args)
    #define IFXOS_USecSleep(n)                      udelay(n)
    #define IFXOS_BlockAlloc(n)                     kmalloc(n, GFP_KERNEL)
    #define IFXOS_BlockFree(n)                      kfree(n)
#endif

#define PARAM_BUFFER_SIZE               2048
#define GET_BITS(x, msb, lsb)           (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
#define SET_BITS(x, msb, lsb, value)    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))
#define IFX_ETHSW_DEV_MAX               2
#define SWITCH_API_MODULE_NAME          "IFX ETHSW SWITCH API"
#define SWITCH_API_DRIVER_VERSION       "1.1.7.2"
#define MICRO_CODE_VERSION              "212"

extern unsigned int g_debug;
#define IFX_ETHSW_DEBUG_PRINT(fmt, args...)

union ifx_sw_param
{
    /* Ethernet Bridging Functions*/
	IFX_ETHSW_MAC_tableAdd_t				MAC_tableAdd;
    IFX_ETHSW_MAC_tableRead_t				MAC_tableRead;
    IFX_ETHSW_MAC_tableRemove_t				MAC_tableRemove;
    IFX_ETHSW_portCfg_t						portcfg;
    IFX_ETHSW_STP_portCfg_t					STP_portCfg;
    IFX_ETHSW_STP_BPDU_Rule_t				STP_BPDU_Rule;
    /* VLAN Functions */
    IFX_ETHSW_VLAN_IdCreate_t				vlan_IdCreate;
    IFX_ETHSW_VLAN_IdDelete_t				vlan_IdDelete;
    IFX_ETHSW_VLAN_IdGet_t					vlan_IdGet;
    IFX_ETHSW_VLAN_portCfg_t				vlan_portcfg;
    IFX_ETHSW_VLAN_portMemberAdd_t			vlan_portMemberAdd;
	IFX_ETHSW_VLAN_portMemberRead_t			vlan_portMemberRead;
    IFX_ETHSW_VLAN_portMemberRemove_t		vlan_portMemberRemove;
    IFX_ETHSW_VLAN_reserved_t				vlan_Reserved;
    IFX_ETHSW_VLAN_IdGet_t					vlan_VidFid;
    /* Operation, Administration, and Management Functions */
	IFX_ETHSW_cfg_t							cfg_Data;
    IFX_ETHSW_MDIO_cfg_t					mdio_cfg;
    IFX_ETHSW_MDIO_data_t					mdio_Data;
    IFX_ETHSW_portLinkCfg_t					portlinkcfgGet;
    IFX_ETHSW_portLinkCfg_t					portlinkcfgSet;
    IFX_ETHSW_portPHY_Addr_t				phy_addr;
    IFX_ETHSW_portRGMII_ClkCfg_t			portRGMII_clkcfg;
    IFX_ETHSW_CPU_PortExtendCfg_t			portextendcfg;
    IFX_ETHSW_portRedirectCfg_t				portRedirectData;
    IFX_ETHSW_RMON_clear_t					RMON_clear;
    IFX_ETHSW_RMON_cnt_t					RMON_cnt;
    IFX_FLOW_RMON_extendGet_t				RMON_ExtendGet;
    IFX_ETHSW_monitorPortCfg_t				monitorPortCfg;
    IFX_ETHSW_cap_t							cap;
    IFX_ETHSW_portPHY_Query_t				phy_Query;
    IFX_ETHSW_CPU_PortCfg_t					CPU_PortCfg;
    IFX_ETHSW_version_t						Version;
    IFX_PSB6970_reset_t						Reset;
    IFX_ETHSW_HW_Init_t						HW_Init;
    IFX_ETHSW_8021X_portCfg_t				PNAC_portCfg;
    IFX_ETHSW_8021X_EAPOL_Rule_t			PNAC_EAPOL_Rule;
    /* Multicast Functions */
    IFX_ETHSW_multicastRouter_t				multicast_RouterPortAdd;
    IFX_ETHSW_multicastRouter_t				multicast_RouterPortRemove;
    IFX_ETHSW_multicastRouterRead_t			multicast_RouterPortRead;
    IFX_ETHSW_multicastTable_t				multicast_TableEntryAdd;
    IFX_ETHSW_multicastTable_t				multicast_TableEntryRemove;
    IFX_ETHSW_multicastTableRead_t			multicast_TableEntryRead;
    IFX_ETHSW_multicastSnoopCfg_t			multicast_SnoopCfgSet;
    IFX_ETHSW_multicastSnoopCfg_t			multicast_SnoopCfgGet;
    /* Quality of Service Functions */
    IFX_ETHSW_QoS_portCfg_t					qos_portcfg;
    IFX_ETHSW_QoS_queuePort_t				qos_queueport;
    IFX_ETHSW_QoS_DSCP_ClassCfg_t			qos_dscpclasscfgget;
    IFX_ETHSW_QoS_DSCP_ClassCfg_t			qos_dscpclasscfgset;
    IFX_ETHSW_QoS_PCP_ClassCfg_t			qos_pcpclasscfgget;
    IFX_ETHSW_QoS_PCP_ClassCfg_t			qos_pcpclasscfgset;
    IFX_PSB6970_QoS_portShaperCfg_t			qos_portShapterCfg;
    IFX_PSB6970_QoS_portShaperStrictCfg_t	qos_portShapterStrictCfg;
    IFX_PSB6970_QoS_portShaperWFQ_Cfg_t		qos_portShapterWFQ_Cfg;
    IFX_PSB6970_QoS_portPolicerCfg_t		qos_portPolicerCfg;
    IFX_PSB6970_QoS_stormCfg_t				qos_stormCfg;
    IFX_PSB6970_QoS_MfcPortCfg_t			qos_MfcPortCfg;
    IFX_PSB6970_QoS_MfcCfg_t				qos_MfcCfg;
    IFX_PSB6970_QoS_MfcMatchField_t			qos_MfcMatchField;
    IFX_PSB6970_QoS_MfcEntryRead_t			qos_MfcEntryRead;
    IFX_ETHSW_QoS_ClassDSCP_Cfg_t			qos_classdscpcfgget;
    IFX_ETHSW_QoS_ClassDSCP_Cfg_t			qos_classdscpcfgset;
    IFX_ETHSW_QoS_ClassPCP_Cfg_t			qos_classpcpcfgget;
    IFX_ETHSW_QoS_ClassPCP_Cfg_t			qos_classpcpcfgset;
    IFX_ETHSW_QoS_ShaperCfg_t				qos_shappercfg;
    IFX_ETHSW_QoS_ShaperQueue_t				qos_shapperqueue;
    IFX_ETHSW_QoS_stormCfg_t				qos_stormcfg;
    IFX_ETHSW_QoS_schedulerCfg_t			qos_schedulecfg;
    IFX_ETHSW_QoS_WRED_Cfg_t				qos_wredcfg;
    IFX_ETHSW_QoS_WRED_QueueCfg_t			qos_wredqueuecfg;
    IFX_ETHSW_QoS_meterCfg_t				qos_metercfg;
    IFX_ETHSW_QoS_meterPort_t				qos_meterport;
    IFX_ETHSW_QoS_meterPortGet_t			qos_meterportget;
    IFX_ETHSW_QoS_portRemarkingCfg_t		qos_portremarking;
    /* Power Management Functions */
    IFX_PSB6970_powerManagement_t			power_management;
    /* Packet Classification Engine */
    IFX_FLOW_PCE_rule_t						pce_rule;
    IFX_FLOW_PCE_ruleDelete_t				pce_ruledelete;
  /* Debug Features */
#if defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
    IFX_PSB6970_register_t					register_access;
#elif defined(VR9)
    IFX_FLOW_register_t						register_access;
#endif
};

typedef enum
{
	IFX_PSB6970_DEV_AR9		 = 0,
	IFX_PSB6970_DEV_TANTOS_3G,
    IFX_PSB6970_DEV_TANTOS_0G,
    IFX_PSB6970_DEV_MAX
} IFX_PSB6970_devType_t;

typedef enum
{
	IFX_FLOW_DEV_INT	= 0,
	IFX_FLOW_DEV_MAX
} IFX_FLOW_devType_t;

#endif
