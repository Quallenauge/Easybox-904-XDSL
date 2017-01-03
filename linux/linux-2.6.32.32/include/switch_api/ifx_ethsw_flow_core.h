/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_flow_core.h
   \remarks FLOW Core Layer header file, for Infineon Ethernet switch
            drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_FLOW_CORE_H_
#define _IFX_ETHSW_FLOW_CORE_H_

//#include "ifx_ethsw_api.h"
#include "ifx_ethsw_reg_access.h"
#include "ifx_ethsw_pce.h"


/**********************************/
/* External Variables & Functions */
/**********************************/
// checked: extern IFX_ETHSW_HL_DRV_CTX_t gHlDrvCtx;
#define MAX_PORT_NUMBER							12 /* including virtual ports*/
#define MULTICAST_HW_TABLE_MAX					64
#define MAC_ADDRESS_LENGTH						6

#define IFX_HW_VLAN_MAP_TABLE_MAX				64
#define IFX_FLOW_VLAN_ENTRY_MAX					IFX_HW_VLAN_MAP_TABLE_MAX
#define IFX_FLOW_PORT_NUMBER_MAX				7
#define IFX_HW_MAC_TABLE_MAX					2048
#define IFX_FLOW_INTERNAL_PORT_NUMBER			2
#define IFX_FLOW_EXTERNAL_PORT_NUMBER			3
#define IFX_FLOW_MULTICAST_SW_TBL_SIZE			64
#define IFX_FLOW_MULTICAST_HW_TBL_SIZE			64


//#define VRX_PLATFORM_CPU_PORT					6
#define VRX_CPU_PORT							6
#define ARX_CPU_PORT							6
#define GHN_CPU_PORT							2
#define VRX_PLATFORM_CAP_SEGNENT				256
#define VRX_PLATFORM_CAP_FID					64
#define IFX_ETHSW_RMON_COUNTER_OFFSET			64
#define IFX_ETHSW_FLOW_TOP_REG_OFFSET			0xC40
#define IFX_ETHSW_MAX_PACKET_LENGTH				9600
#define IFX_ETHSW_MAC_BRIDGE_TABLE_INDEX		0xB
#define IFX_FLOW_PORT_STATE_LISTENING_ONLY		0
#define IFX_FLOW_PORT_STATE_RX_ENABLE_TX_DISABLE	1
#define IFX_FLOW_PORT_STATE_RX_DISABLE_TX_ENABLE	2
#define IFX_FLOW_PORT_STATE_LEARNING			4
#define IFX_FLOW_PORT_STATE_FORWARDING_ENABLE	7
#define RMON_EXTEND_TRAFFIC_FLOW_COUNT_1		0x28

#define DEFAULT_AGING_TIMEOUT					300
/* Define Aging Counter Mantissa Value */
#define AGETIMER_1_DAY							0xFB75
#define AGETIMER_1_HOUR							0xA7BA
#define AGETIMER_300_SEC						0xDF84
#define AGETIMER_10_SEC							0x784
#define AGETIMER_1_SEC							0xBF

/* Define Duplex Mode */
#define DUPLEX_AUTO								0
#define DUPLEX_EN								1
#define DUPLEX_DIS								3

/** Description */
typedef struct {
	/* Cap Type */
	IFX_ETHSW_capType_t	Cap_Type;
	/* Description String */
	IFX_char_t	Desci[IFX_ETHSW_CAP_STRING_LEN];
}IFX_ETHSW_CapDesc_t;

typedef struct {
	/* STP State */
	IFX_ETHSW_STP_PortState_t	ifx_stp_state;
	/* 8021X State */
	IFX_ETHSW_8021X_portState_t	ifx_8021_state;
	/* PEN Register */
	IFX_uint8_t		pen_reg;
	/* PSTATE Register */
	IFX_uint8_t		pstate_reg;
	/* LRN limit */
	IFX_uint8_t		lrnlim;
}PORT_STATE_t;

/** Description */
typedef struct
{
	/* Port Enable */
	IFX_boolean_t		bPortEnable;
	/** Transparent Mode */
	IFX_boolean_t		bTVM;
	/* Learning Limit Action */
	IFX_boolean_t		bLearningLimitAction;
	/* Automatic MAC address table learning locking */
	IFX_boolean_t		bLearningPortLocked;
	/* Automatic MAC address table learning limitation */
	IFX_uint16_t		nLearningLimit;
	/* Port State */
	IFX_uint16_t		nPortState;
	/* Port State for STP */
	IFX_ETHSW_STP_PortState_t	ifx_stp_state;
	/* Port State for 8021.x */
	IFX_ETHSW_8021X_portState_t	ifx_8021x_state;
}IFX_FLOW_portConfig_t;

/** Description */
typedef struct
{
	/* 8021x Port Forwarding State */
	IFX_ETHSW_portForward_t	eForwardPort;
	/* 8021X Forwarding Port ID*/
	IFX_uint8_t				n8021X_ForwardPortId;
	/* STP port State */
	IFX_ETHSW_portForward_t	eSTPPortState;
	/* STP Port ID */
	IFX_uint16_t			nSTP_PortID;
}IFX_FLOW_STP_8021X_t;

/** Description */
typedef struct
{
	IFX_boolean_t		valid;
	/* VLAN ID */
	IFX_uint16_t		vid;
	/* Forward ID */
	IFX_uint32_t		fid;
	/* Port Member */
	IFX_uint16_t		pm;
	/* Tag Member */
	IFX_uint16_t		tm;
	/* VID reserved */
	IFX_boolean_t		reserved;
}IFX_FLOW_VLAN_tableEntry_t;


typedef struct
{
	/* PortMap */
	IFX_uint8_t		nPortId;
	/* Group Destination IP address */
	IFX_ETHSW_IP_t	uIP_Gda;
	/* Group Source IP address */
	IFX_ETHSW_IP_t	uIP_Gsa;
	/* Static GIP */
	IFX_boolean_t	bStatic;
}IFX_MulticastHW_table_Entry_t;

typedef struct
{
	/* PortMap */
	/* Src IP MSB index*/
	IFX_uint16_t		SrcIp_MSB_Index;
	/* Dis IP MSB index*/
	IFX_uint16_t		DisIp_MSB_Index;
	/* Src IP LSB index*/
	IFX_uint16_t		SrcIp_LSB_Index;
	/* Dis IP LSB index*/
	IFX_uint16_t		DisIp_LSB_Index;
	 /* PortMap */
	IFX_uint16_t		PortMap;
	/* Membber Mode */
	IFX_uint16_t		eModeMember;
	 /* Valid */
	IFX_boolean_t		valid;
}IFX_MulticastSW_table_Entry_t;

/** Description */
typedef struct
{
	/* eIGMP_Mode */
	IFX_uint16_t		eIGMP_Mode;
	/* bIGMPv3 */
	IFX_boolean_t		bIGMPv3;
	/* eForwardPort */
	IFX_uint16_t		eForwardPort;
	/* nForwardPortId */
	IFX_uint8_t			nForwardPortId;
	/* bCrossVLAN */
	IFX_boolean_t		bCrossVLAN;
	/* nCOS */
	IFX_uint8_t			nClassOfService;
	/* Multicast HW Table */
//	IFX_MulticastHW_table_Entry_t	multicast_hw_table[IFX_FLOW_MULTICAST_HW_TBL_SIZE];
	/* Multicast SW Table */
	IFX_MulticastSW_table_Entry_t	multicast_sw_table[IFX_FLOW_MULTICAST_SW_TBL_SIZE];
	/* Multicast Router Port */
	IFX_uint16_t		 eRouterPort;
	/* SW Table side */
	IFX_uint8_t			nSwTblSize;   
}IFX_FLOW_IGMP_t;

/** Description */
typedef struct
{
	/* Static Entry*/
	IFX_uint16_t		bStaticEntry;
	/* Aging Time */
	IFX_int32_t			nAgeTimer;
	/* Forwarding Database ID */
	IFX_uint32_t		nFId;
	/* Ethernet Port number */
	IFX_uint32_t		nPortId;
	/* PortMap */
	IFX_uint8_t			nMAC[MAC_ADDRESS_LENGTH];
}IFX_MAC_table_Entry_t;


typedef struct {
	IFX_FLOW_devType_t			eDev;
	IFX_FLOW_portConfig_t		PortConfig[IFX_FLOW_PORT_NUMBER_MAX];
	IFX_boolean_t				bVLAN_Aware;
	IFX_FLOW_VLAN_tableEntry_t	VLAN_Table[IFX_HW_VLAN_MAP_TABLE_MAX];
	IFX_FLOW_STP_8021X_t		STP_8021x_Config;
	IFX_FLOW_IGMP_t				IGMP_Flags;
	IFX_PCE_t					PCE_Handler;
	IFX_void_t					*pRAL_Dev;
	IFX_void_t					*pPMCtx;
	IFX_uint8_t					nPortNumber;
	IFX_uint8_t					nTotalPortNumber;
	IFX_uint8_t					nManagementPortNumber;
	IFX_uint32_t				MAC_AgeTimer;
	IFX_boolean_t				bResetCalled;
	IFX_boolean_t				bHW_InitCalled;
	IFX_uint8_t					vlan_table_index;
	IFX_uint16_t				mac_table_index;
	IFX_uint16_t				multi_hw_table_index;
	IFX_uint16_t				multi_sw_table_index;
	IFX_uint8_t					nCPU_Port;
} IFX_FLOW_switchDev_t;

typedef struct
{
	IFX_void_t					*pDev;
	IFX_FLOW_devType_t			eDev;
}IFX_FLOW_switchCoreInit_t;

typedef struct
{
	IFX_uint8_t			nTable_Index;
	IFX_uint8_t			nOP; // operation (1:add, 2:delete)
	IFX_uint16_t		nVId;
	IFX_uint32_t		nFId;
}IFX_FLOW_VLAN_Filter_registerOperation_t;

/**********************/
/* Function Prototype */
/**********************/

/**********************/
/* Function Prototype */
/**********************/
IFX_return_t IFX_FLOW_VLAN_Table_Print ( IFX_void_t *pDevCtx );
IFX_uint8_t IFX_FLOW_VLAN_Table_Index_Find ( IFX_void_t *pDevCtx, IFX_uint16_t vid );

//IFX_int32_t ifx_multicast_sw_table_ip_dasa_search(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_IP_DASA_LSB_TBL_t *pValue);
//IFX_int32_t ifx_multicast_sw_table_write(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar);
//IFX_int32_t ifx_multicast_sw_table_remove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar);
IFX_boolean_t IFX_FLOW_switchStatusGet(IFX_void_t *pDevCtx);
IFX_return_t IFX_FLOW_switchDeviceEnable(IFX_void_t *pDevCtx);
IFX_return_t IFX_FLOW_switchDeviceDisable(IFX_void_t *pDevCtx);
IFX_void_t *IFX_FLOW_SwitchCoreInit(IFX_FLOW_switchCoreInit_t *pInit );
void IFX_FLOW_SwitchCoreCleanUP(IFX_void_t );

extern IFX_uint32_t ifx_ethsw_ll_DirectAccessRead(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t * value);
extern IFX_return_t ifx_ethsw_ll_DirectAccessWrite(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t value);

#endif    /* _IFX_ETHSW_PSB6970_CORE_H_ */
