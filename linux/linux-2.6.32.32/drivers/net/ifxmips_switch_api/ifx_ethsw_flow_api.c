/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_flow_api.c
   \remarks implement SWITCH API declared on ifx_ethsw_flow_ll.h
 *****************************************************************************/
#include <ifx_ethsw_flow_core.h>
#include <ifx_ethsw_pce.h>
#include <ifx_ethsw_flow_ll.h>
#include <ifx_ethsw_core_platform.h>

#define FLOW_TABLE_DEBUG	0
/********************/
/* Global Variables */
/********************/
IFX_FLOW_switchDev_t *pCoreDev[IFX_FLOW_DEV_MAX];
IFX_uint8_t VERSION_NAME[]= SWITCH_API_MODULE_NAME " for GSWITCH Platform";
IFX_uint8_t VERSION_NUMBER[] = SWITCH_API_DRIVER_VERSION;
IFX_uint8_t MICRO_CODE_VERSION_NAME[]= " GSWIP macro revision ID";
IFX_uint8_t MICRO_CODE_VERSION_NUMBER[] = "0x"MICRO_CODE_VERSION;
static IFX_uint8_t  MULTICAST_ROUTERPORT_COUNTER = 0;
static IFX_uint16_t METER_COUNTER = 0;
static IFX_uint8_t PortRedirectFlag = 0;

/******************************/
/* Local Macros & Definitions */
/******************************/
static PORT_STATE_t port_state_table[] = {
	{ IFX_ETHSW_STP_PORT_STATE_FORWARD, IFX_ETHSW_8021X_PORT_STATE_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_FORWARDING_ENABLE, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_FORWARD, IFX_ETHSW_8021X_PORT_STATE_UNAUTHORIZED, 1, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_FORWARD, IFX_ETHSW_8021X_PORT_STATE_RX_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_RX_ENABLE_TX_DISABLE, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_FORWARD, IFX_ETHSW_8021X_PORT_STATE_TX_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_RX_DISABLE_TX_ENABLE, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_DISABLE, IFX_ETHSW_8021X_PORT_STATE_AUTHORIZED, 0, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_DISABLE, IFX_ETHSW_8021X_PORT_STATE_UNAUTHORIZED, 0, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_DISABLE, IFX_ETHSW_8021X_PORT_STATE_RX_AUTHORIZED, 0, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_DISABLE, IFX_ETHSW_8021X_PORT_STATE_TX_AUTHORIZED, 0, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_LEARNING, IFX_ETHSW_8021X_PORT_STATE_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_LEARNING, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_LEARNING, IFX_ETHSW_8021X_PORT_STATE_UNAUTHORIZED, 1, IFX_FLOW_PORT_STATE_LEARNING, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_LEARNING, IFX_ETHSW_8021X_PORT_STATE_RX_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_LEARNING, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_LEARNING, IFX_ETHSW_8021X_PORT_STATE_TX_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_LEARNING, 1 },
	{ IFX_ETHSW_STP_PORT_STATE_BLOCKING, IFX_ETHSW_8021X_PORT_STATE_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_BLOCKING, IFX_ETHSW_8021X_PORT_STATE_UNAUTHORIZED, 1, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_BLOCKING, IFX_ETHSW_8021X_PORT_STATE_RX_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 },
	{ IFX_ETHSW_STP_PORT_STATE_BLOCKING, IFX_ETHSW_8021X_PORT_STATE_TX_AUTHORIZED, 1, IFX_FLOW_PORT_STATE_LISTENING_ONLY, 0 }
};

static IFX_ETHSW_CapDesc_t CAP_Description[] = {
	{ IFX_ETHSW_CAP_TYPE_PORT,                "Number of physical Ethernet ports"},
	{ IFX_ETHSW_CAP_TYPE_VIRTUAL_PORT,        "Number of virtual Ethernet ports"},
	{ IFX_ETHSW_CAP_TYPE_BUFFER_SIZE,         "Size of internal packet memory [in Bytes]"},
	{ IFX_ETHSW_CAP_TYPE_SEGMENT_SIZE,        "Number of Segment size per device"},
	{ IFX_ETHSW_CAP_TYPE_PRIORITY_QUEUE,      "Number of priority queues per device"},
	{ IFX_ETHSW_CAP_TYPE_METER,               "Number of meter instances"},
	{ IFX_ETHSW_CAP_TYPE_RATE_SHAPER,         "Number of rate shaper instances"},
	{ IFX_ETHSW_CAP_TYPE_VLAN_GROUP,          "Number of VLAN groups that can be configured on the switch hardware"},
	{ IFX_ETHSW_CAP_TYPE_FID,                 "Number of Forwarding database IDs [FIDs]"},
	{ IFX_ETHSW_CAP_TYPE_MAC_TABLE_SIZE,      "Number of MAC table entries"},
	{ IFX_ETHSW_CAP_TYPE_MULTICAST_TABLE_SIZE,"Number of multicast level 3 hardware table entries"},
	{ IFX_ETHSW_CAP_TYPE_PPPOE_SESSION,       "Number of supported PPPoE sessions"},
	{ IFX_ETHSW_CAP_TYPE_LAST,                "Last Capability Index"}
};

/*****************/
/* Function Body */
/*****************/
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
IFX_return_t IFX_FLOW_MULTICAST_SW_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	
	IFXOS_PRINT_INT_RAW("|======MULTICAST SW Table =======|\n");
	for (i = 0; i < 64; i++) {
		pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("\t|Index = %d | SIP_LSB = %03d | DIP_LSB = %03d | PortMAP = %02x\n",	\
				i, (pData.key[1] & 0x3F), (pData.key[0] & 0x3F), pData.val[0]);
		}
	}
	return IFX_SUCCESS;
}   /* -----  end of function IP DA/SA LSB Table  ----- */
#endif

static void IFX_FLOW_PortCfg_Init (IFX_void_t *pDevCtx)
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t i, value;
	
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_PPORTS_OFFSET,	\
			VR9_ETHSW_CAP_1_PPORTS_SHIFT, VR9_ETHSW_CAP_1_PPORTS_SIZE, &value);
	pEthSWDev->nPortNumber = value;
	for ( i = 0; i < pEthSWDev->nPortNumber; i++ ) {
		memset(&pEthSWDev->PortConfig[i], 0 , sizeof(IFX_FLOW_portConfig_t));
		pEthSWDev->PortConfig[i].nLearningLimit = 0xFF;
		pEthSWDev->PortConfig[i].bPortEnable = 1;
	}
	pEthSWDev->STP_8021x_Config.eForwardPort = IFX_ETHSW_PORT_FORWARD_DEFAULT;
	pEthSWDev->STP_8021x_Config.n8021X_ForwardPortId = pEthSWDev->nCPU_Port/* VRX_PLATFORM_CPU_PORT */;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_VPORTS_OFFSET,	\
			VR9_ETHSW_CAP_1_VPORTS_SHIFT, VR9_ETHSW_CAP_1_VPORTS_SIZE, &value);
	pEthSWDev->nTotalPortNumber = value + pEthSWDev->nPortNumber;
}   /* -----  end of function IFX_FLOW_PortCfg_Init  ----- */

static void IFX_FLOW_MULTICAST_SW_Table_Init (IFX_void_t *pDevCtx)
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t i,value;
	
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_8_MCAST_OFFSET,	\
			VR9_ETHSW_CAP_8_MCAST_SHIFT, VR9_ETHSW_CAP_8_MCAST_SIZE, &value);
		memset(&pEthSWDev->IGMP_Flags, 0 , sizeof(IFX_FLOW_IGMP_t));
		pEthSWDev->IGMP_Flags.nSwTblSize = value;
		for ( i = 0; i < pEthSWDev->IGMP_Flags.nSwTblSize; i++ ) {
			pEthSWDev->IGMP_Flags.multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
			pEthSWDev->IGMP_Flags.multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
			pEthSWDev->IGMP_Flags.multicast_sw_table[i].SrcIp_MSB_Index = 0x1F;
			pEthSWDev->IGMP_Flags.multicast_sw_table[i].DisIp_MSB_Index = 0x1F;
		}

}   /* -----  end of function IFX_FLOW_VLAN_Table_Init  ----- */

/* Multicast Software Table Include/Exclude Add function */
IFX_int32_t ifx_multicast_sw_table_write(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar)
{
	IFX_FLOW_switchDev_t *pDEVHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t    i,j, Dip_tab_index, Sip_tab_index = 0xFF, WildCard_tab_index = 0xFF;
	IFX_boolean_t   DIP_SIP_MATCH = IFX_FALSE,WILDCARDFOUND = IFX_FALSE;
	IFX_void_t *pDev = IFX_NULL;
	IFX_PCE_t *pIPTmHandle = &pDEVHandle->PCE_Handler;
	IFX_FLOW_IGMP_t *pIGMPTbHandle = &pDEVHandle->IGMP_Flags;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_PCE_IP_DASA_LSB_TBL_t  dasa_lsb_tbl;
	
	memset(&pData, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	memset(&dasa_lsb_tbl, 0, sizeof(IFX_PCE_IP_DASA_LSB_TBL_t));
	if ((pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE)	\
		|| (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE)) {
			if (pDEVHandle->IGMP_Flags.bIGMPv3 != 1) {
				IFXOS_PRINT_INT_RAW("The bIGMPv3 need to enable\n");
				return IFX_ERROR;
			}
	}
	/* First, search for DIP in the DA/SA table (DIP LSB) */
	for ( i = 0; i < 4 ; i++) {
		dasa_lsb_tbl.ip_lsb[i] = (( pPar->uIP_Gda.nIPv4 >> ( i * 8)) & 0xFF);
	}
	/* DIP LSB Nibble Mask */
	dasa_lsb_tbl.mask = 0xFF00;
	Dip_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
	if ((pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE)	\
			|| (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)) {
		if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) {
			/* Second, search for SIP in the DA/SA table ( SIP LSB) */
			for ( i = 0; i < 4 ; i++) {
				dasa_lsb_tbl.ip_lsb[i] = (( pPar->uIP_Gsa.nIPv4 >> ( i * 8)) & 0xFF);
			}
			/* DIP LSB Nibble Mask */
			dasa_lsb_tbl.mask = 0xFF00;
			Sip_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
		} else {
			/* Place the WildCard Entry */
			for ( i = 0; i < 4 ; i++) {
				dasa_lsb_tbl.ip_lsb[i] = 0;
			}
			dasa_lsb_tbl.mask = 0xFFFF;
			WildCard_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
		/* SourceIP is wildcard, and start search for all DistIP and add the port entry */
			for(i=0; i<pDEVHandle->IGMP_Flags.nSwTblSize; i++) {
				/* Check if port was already exist */
				if ((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
						(pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE)) {
					switch (pIGMPTbHandle->multicast_sw_table[i].eModeMember ) {
						case IFX_ETHSW_IGMP_MEMBER_DONT_CARE:
							if (((pIGMPTbHandle->multicast_sw_table[i].PortMap >> pPar->nPortId) & 0x1 )== 1) {
								pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[WildCard_tab_index]--;
								return IFX_SUCCESS;
							} else {
							pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index]++;
							}
							/* Add the port */
							pIGMPTbHandle->multicast_sw_table[i].PortMap |= (1 << pPar->nPortId);
							pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_DONT_CARE;
							WILDCARDFOUND = IFX_TRUE;
							/* Write Into HW Table */
							pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = WildCard_tab_index;
							pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
							break;
						case IFX_ETHSW_IGMP_MEMBER_EXCLUDE:
							pIGMPTbHandle->multicast_sw_table[i].PortMap &= ~(1 << pPar->nPortId);
							/* Zero Port Map, delet the rule */
							if (pIGMPTbHandle->multicast_sw_table[i].PortMap == 0) {
								pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
								pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
								pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
								/* Delet the entry from Multicast sw Table */
								pIGMPTbHandle->multicast_sw_table[i].valid = IFX_FALSE;
							}
							pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
							break;
						case IFX_ETHSW_IGMP_MEMBER_INCLUDE:
							if (((pIGMPTbHandle->multicast_sw_table[i].PortMap >> pPar->nPortId) & 0x1 )== 1) {
								pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[WildCard_tab_index]--;
								return IFX_SUCCESS;
							} else {
								pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index]++;
								/* Add the port */
								pIGMPTbHandle->multicast_sw_table[i].PortMap |= (1 << pPar->nPortId);
							}
							pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
							break;
					} /* end switch */
					/* Now, we write into Multicast SW Table */
					pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
					pData.table_index = i;
					pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
					pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
					pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
					pData.valid = 1;
					ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
				}
		}
	}
	/* Search all sw table for SrcIP && DisIP match */
	for (i=0; i<pDEVHandle->IGMP_Flags.nSwTblSize; i++) {
		if ((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
			(pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index == Sip_tab_index) &&	\
			( pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE)) {
			if (pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) {
				if (((pIGMPTbHandle->multicast_sw_table[i].PortMap >> pPar->nPortId) & 0x1 )== 1) {
					pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index]--;
					pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Sip_tab_index]--;
					return IFX_SUCCESS;
				}
				
				/* Add the port */
				pIGMPTbHandle->multicast_sw_table[i].PortMap |= (1 << pPar->nPortId);
				pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
				DIP_SIP_MATCH = IFX_TRUE;
				/* Write Into HW Table */
				pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = Sip_tab_index;
				pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
			} else if (pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE) {
				/* conflect rule replace by Include rule */
				/* Add the port */
				pIGMPTbHandle->multicast_sw_table[i].PortMap |= (1 << pPar->nPortId);
				pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
				DIP_SIP_MATCH = IFX_TRUE;
				/* Write Into HW Table */
				pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = Sip_tab_index;
				pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
			}
			/* Now, we write into Multicast SW Table */
			pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
			pData.table_index = i;
			pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
			pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
			pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
			pData.valid = pIGMPTbHandle->multicast_sw_table[i].valid;
			ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
		} else if((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
			(pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index == WildCard_tab_index) &&	\
			( pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE)) {
			if (pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE) {
				if (((pIGMPTbHandle->multicast_sw_table[i].PortMap >> pPar->nPortId) & 0x1 )== 1) {
					/* Found it, Remove the Port */
					pIGMPTbHandle->multicast_sw_table[i].PortMap &= ~(1 << pPar->nPortId);
					/* check if the entry is empty port */
					if (pIGMPTbHandle->multicast_sw_table[i].PortMap == 0) {
						/* Remove the entry */
						pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
						pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
						pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
						/* Delet the entry from Multicast sw Table */
						pIGMPTbHandle->multicast_sw_table[i].valid = IFX_FALSE;
					} else {
						pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
					}
					/* Now, we write into Multicast SW Table */
					pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
					pData.table_index = i;
					pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
					pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
					pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
					pData.valid = pIGMPTbHandle->multicast_sw_table[i].valid;
					ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
				}
			}
		}
	}
	/* Create the new DstIP & SrcIP entry */
	if ((WILDCARDFOUND == IFX_FALSE) && (DIP_SIP_MATCH == IFX_FALSE)) {
		if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) {
			i = 0;
			while ( i < pDEVHandle->IGMP_Flags.nSwTblSize ) {
				/* Find a new empty entry to add */
				if (pIGMPTbHandle->multicast_sw_table[i].valid == IFX_FALSE)
					break;
				i++;
			}
		} else if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE) {
			i = 63;
			while ( i > 0 ) {
				/* Find a new empty entry to add */
				if (pIGMPTbHandle->multicast_sw_table[i].valid == IFX_FALSE)
					break;
				i--;
			}
		}
	
		if ( i >= 0 && i < pDEVHandle->IGMP_Flags.nSwTblSize) {
			pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = Dip_tab_index;
			if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE)
				pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = Sip_tab_index;
			else if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
				pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = WildCard_tab_index;
			pIGMPTbHandle->multicast_sw_table[i].eModeMember = pPar->eModeMember;
			if (pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) {
				/* additional search to attach the same GDA Wildcard port */
				for ( (j=pDEVHandle->IGMP_Flags.nSwTblSize - 1); j > i; j--) {
					if ((pIGMPTbHandle->multicast_sw_table[j].eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE) &&	\
						(pIGMPTbHandle->multicast_sw_table[j].DisIp_LSB_Index == Dip_tab_index)) {
							pIGMPTbHandle->multicast_sw_table[i].PortMap = pIGMPTbHandle->multicast_sw_table[j].PortMap;
							break;
					}
				}
			}
			pIGMPTbHandle->multicast_sw_table[i].PortMap |= (1 << pPar->nPortId);
			pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
		} else {
			IFXOS_PRINT_INT_RAW("The Multicast SW Table for wildcard add is full.\n");
			return IFX_ERROR;
		}
		/* Now, we write into Multicast SW Table */
		pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
		pData.table_index = i;
		pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
		pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
		pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
		pData.valid = 1;
		ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
	}
	/* Check if there is still place for a new entry in the table */
	i = 0;
	while ( i < pDEVHandle->IGMP_Flags.nSwTblSize ) {
		/* Find a new empty entry to add */
		if (pIGMPTbHandle->multicast_sw_table[i].valid == IFX_FALSE)
			break;
		i++;
	}
	if ( i >= pDEVHandle->IGMP_Flags.nSwTblSize )
		return IFX_ERROR;
   } else if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE) {
   		/* SIP LSB */
   		for ( i = 0; i < 4 ; i++) {
   			dasa_lsb_tbl.ip_lsb[i] = (( pPar->uIP_Gsa.nIPv4 >> ( i * 8)) & 0xFF);
   		}
   		/* DIP LSB Nibble Mask */
   		dasa_lsb_tbl.mask = 0xFF00;
   		if(dasa_lsb_tbl.ip_lsb[3] == 0 && dasa_lsb_tbl.ip_lsb[2] == 0 &&	\
   			dasa_lsb_tbl.ip_lsb[1] == 0 && dasa_lsb_tbl.ip_lsb[0] == 0) {
   			IFXOS_PRINT_INT_RAW("Exclude Rule Source IP is Wildcard. ERROR\n");
   			return IFX_ERROR;
   		}
   		Sip_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
   		/* Search all sw table for SrcIP && DisIP match */
   		for (i=0; i<pDEVHandle->IGMP_Flags.nSwTblSize; i++) {
   			if ((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
   				(pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE)) {
   				if (pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index == Sip_tab_index) {
   					/* Remove the port */
   					pIGMPTbHandle->multicast_sw_table[i].PortMap &= ~(1 << pPar->nPortId);
   					pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
   					/* Now, we write into Multicast SW Table */
   					pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
   					pData.table_index = i;
   					pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
   					pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
   					pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
   					pData.valid = pIGMPTbHandle->multicast_sw_table[i].valid;
   					ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
   					DIP_SIP_MATCH = IFX_TRUE;
   				} else {
   					pIGMPTbHandle->multicast_sw_table[i].PortMap |= (1 << pPar->nPortId);
   					if ((pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE) ||	\
   						(pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE)) {
   						/* Now, we write into Multicast SW Table */
   						pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
   						pData.table_index = i;
   						pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
   						pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
   						pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
   						pData.valid = pIGMPTbHandle->multicast_sw_table[i].valid;
   						ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
   						WILDCARDFOUND = IFX_TRUE;
   					}
   				}
   			}
  	 	}
   		if ((DIP_SIP_MATCH == IFX_TRUE) && (WILDCARDFOUND == IFX_TRUE)) {
   			pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index]--;
   			pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Sip_tab_index]--;
   			return IFX_SUCCESS;
   		}
  	 	/* Add the new entry */
  	 	if(DIP_SIP_MATCH == IFX_FALSE) {
   			i = 0;
   			while ( i < pDEVHandle->IGMP_Flags.nSwTblSize ) {
   				/* Find a new empty entry to add */
   				if (pIGMPTbHandle->multicast_sw_table[i].valid == IFX_FALSE)
   					break;
   				i++;
   			}
   			if ( i >= 0 && i < pDEVHandle->IGMP_Flags.nSwTblSize) {
   				pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = Dip_tab_index;
   				pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = Sip_tab_index;
   				pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
   				pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
   				pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
   				/* Now, we write into Multicast SW Table */
   				pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
   				pData.table_index = i;
   				pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
   				pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
   				pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
   				pData.valid = 1;
   				ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
   			} else {
   				IFXOS_PRINT_INT_RAW("The Table is full.\n");
   				return IFX_ERROR;
   			}
   			/* The While routine to re-org the table */
  	 	}
   		/* If no wildcard DisIP entry found, create one */
		if (WILDCARDFOUND == IFX_FALSE) {
   			/* Place the WildCard Entry */
   			for ( i = 0; i < 4; i++ ) {
   				dasa_lsb_tbl.ip_lsb[i] = 0;
   			}
   			dasa_lsb_tbl.mask = 0xFFFF;
   			WildCard_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
   			i = 63;
   			while ( i > 0 ) {
   				/* Find a new empty entry to add */
   				if (pIGMPTbHandle->multicast_sw_table[i].valid == IFX_FALSE)
   					break;
   				i--;
   			}
   			if ( i >= 0 && i < pDEVHandle->IGMP_Flags.nSwTblSize) {
   				pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = Dip_tab_index;
   				pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = WildCard_tab_index;
   				pIGMPTbHandle->multicast_sw_table[i].PortMap = (1 << pPar->nPortId);
   				pIGMPTbHandle->multicast_sw_table[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
   				pIGMPTbHandle->multicast_sw_table[i].valid = IFX_TRUE;
   				/* Now, we write into Multicast SW Table */
   				pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
   				pData.table_index = i;
   				pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
   				pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
   				pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
   				pData.valid = 1;
   				ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
   			} else {
   				IFXOS_PRINT_INT_RAW("The Table is full.\n");
   				return IFX_ERROR;
   			}
   		/* The While routine to re-org the table */
  	 	}
	}
	/* Debug */
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
	IFXOS_PRINT_INT_RAW("Multicast SW Table Print\n");
	IFXOS_PRINT_INT_RAW("========================\n");
	for (i=0; i < 64 ;i++) {
		/* Print out debug info first */
		IFXOS_PRINT_INT_RAW("\t|Index = %d | Valid = %d | SIP_LSB = %03d | DIP_LSB = %03d | PortMAP = %02x\n",	\
			i,pIGMPTbHandle->multicast_sw_table[i].valid,pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index,	\
			pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index, pIGMPTbHandle->multicast_sw_table[i].PortMap);
	}
	/* Debug */
	/* Print out the Multicast SW Table */
	IFX_FLOW_MULTICAST_SW_Table_Print();
	/* Print out the IP DA/SA Taboe */
	IFX_FLOW_IP_DASA_LSB_Table_Print();
	/* Print out debug info first */
	IFXOS_PRINT_INT_RAW("|DEBUG: GroupIPv4 = %03d.%03d.%03d.%03d \n",	\
		(pPar->uIP_Gda.nIPv4 & 0xFF),((pPar->uIP_Gda.nIPv4  >> 8) & 0xFF),	\
		((pPar->uIP_Gda.nIPv4  >> 16) & 0xFF),((pPar->uIP_Gda.nIPv4  >> 24) & 0xFF));
	 /* Print out debug info first */
	 IFXOS_PRINT_INT_RAW("|DEBUG: SourceIPv4 = %03d.%03d.%03d.%03d \n",
                       (pPar->uIP_Gsa.nIPv4 & 0xFF),
                       ((pPar->uIP_Gsa.nIPv4  >> 8) & 0xFF),
                       ((pPar->uIP_Gsa.nIPv4  >> 16) & 0xFF),
                       ((pPar->uIP_Gsa.nIPv4  >> 24) & 0xFF));
#endif /* FLOW_TABLE_DEBUG */
	return IFX_SUCCESS;
}

/* Multicast Software Table Include/Exclude Remove function */
IFX_int32_t ifx_multicast_sw_table_remove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar)
{
	IFX_FLOW_switchDev_t *pDEVHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t    i, j, k, table_index = 0;
	IFX_uint32_t    Dip_tab_index, Sip_tab_index, WildCard_tab_index;
	IFX_boolean_t   MATCH = IFX_FALSE;
	IFX_void_t *pDev = IFX_NULL;
	IFX_PCE_t *pIPTmHandle = &pDEVHandle->PCE_Handler;
	IFX_FLOW_IGMP_t *pIGMPTbHandle = &pDEVHandle->IGMP_Flags;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_PCE_IP_DASA_LSB_TBL_t  dasa_lsb_tbl;
	
	memset(&pData, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	memset(&dasa_lsb_tbl, 0, sizeof(IFX_PCE_IP_DASA_LSB_TBL_t));
	/* First, search for DIP in the DA/SA table,  DIP LSB */
	for ( i = 0 ; i < 4 ; i++ ) {
		dasa_lsb_tbl.ip_lsb[i] = (( pPar->uIP_Gda.nIPv4 >> ( i * 8)) & 0xFF);
	}
	/* DIP LSB Nibble Mask */
	dasa_lsb_tbl.mask = 0xFF00;
	Dip_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
	/* Place the WildCard Entry */
	for ( i = 0 ; i < 4 ; i++ ) {
		dasa_lsb_tbl.ip_lsb[i] = 0;
	}
	dasa_lsb_tbl.mask = 0xFFFF;
	WildCard_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
	/* Second, search for SIP in the DA/SA table, SIP LSB */
	memset(&dasa_lsb_tbl, 0, sizeof(IFX_PCE_IP_DASA_LSB_TBL_t));
	for ( i = 0 ; i < 4 ; i++ ) {
		dasa_lsb_tbl.ip_lsb[i] = (( pPar->uIP_Gsa.nIPv4 >> ( i * 8)) & 0xFF);
	}
	/* DIP LSB Nibble Mask */
	dasa_lsb_tbl.mask = 0xFF00;
	Sip_tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pIPTmHandle->pce_sub_tbl, &dasa_lsb_tbl);
	if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE) {
		/* Search all sw table for SrcIP && DisIP match */
		for ( i = 0; i < pDEVHandle->IGMP_Flags.nSwTblSize; i++ ) {
			if ((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
				(pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE)) {
				if (((pIGMPTbHandle->multicast_sw_table[i].PortMap >> pPar->nPortId) & 0x1 )== 1) {
					table_index = i;
					pIGMPTbHandle->multicast_sw_table[i].PortMap &= ~(1 << pPar->nPortId);
					if (pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index] > 0) {
						ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(&pIPTmHandle->pce_sub_tbl, Dip_tab_index);
					}
					if (pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index] == 0) {
						/* Delet the sub table */
						ifx_pce_tm_ip_dasa_lsb_tbl_delete(&pIPTmHandle->pce_sub_tbl, Dip_tab_index);
					}
					if (pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[WildCard_tab_index] > 0) {
						ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(&pIPTmHandle->pce_sub_tbl, WildCard_tab_index);
					}
#if 0
					if (pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[WildCard_tab_index] == 0) {
						/* Delet the sub table */
						IFXOS_PRINT_INT_RAW("WildCard delet\n");
						ifx_pce_tm_ip_dasa_lsb_tbl_delete(&pIPTmHandle->pce_sub_tbl, WildCard_tab_index);
					}
#endif
					/* Check the port map status */
					if (pIGMPTbHandle->multicast_sw_table[i].PortMap == 0) {
						pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
						pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
						pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
						/* Delet the entry from Multicast sw Table */
						pIGMPTbHandle->multicast_sw_table[i].valid = IFX_FALSE;
					}
					MATCH = IFX_TRUE;
				}
			}
		}
	} else if ((pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) ||	\
		(pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE)) {
		if (Sip_tab_index == WildCard_tab_index) {
			IFXOS_PRINT_INT_RAW("The Source IP address should be specify\n");
			return IFX_ERROR;
		} else {
			/* Search all sw table for SrcIP && DisIP match */
			for (i=0; i<pDEVHandle->IGMP_Flags.nSwTblSize; i++) {
				if ((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
					(pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index == Sip_tab_index) &&	\
					( pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE)) {
					pIGMPTbHandle->multicast_sw_table[i].PortMap &= ~(1 << pPar->nPortId);
					/* Additional search to the WildCard entry if the Port are overlap */
					if ((pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) &&	\
						(pIGMPTbHandle->multicast_sw_table[i].PortMap != 0)) {
						/* additional search to attach the same GDA Wildcard port */
						for ( k=63 ; k > i; k--) {
							if ((pIGMPTbHandle->multicast_sw_table[k].eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE) &&	\
								(pIGMPTbHandle->multicast_sw_table[k].DisIp_LSB_Index == Dip_tab_index) &&	\
								(pIGMPTbHandle->multicast_sw_table[k].PortMap == pIGMPTbHandle->multicast_sw_table[i].PortMap)) {
								pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
								break;
							}
						}
					}
					if (pIGMPTbHandle->multicast_sw_table[i].PortMap == 0) {
						ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(&pIPTmHandle->pce_sub_tbl, Dip_tab_index);
						ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(&pIPTmHandle->pce_sub_tbl, Sip_tab_index);
						ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(&pIPTmHandle->pce_sub_tbl, WildCard_tab_index);
						/* Found the match, check if the index still taken by others */
						if ((pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index] == 0) &&	\
							( pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Sip_tab_index] == 0)) {
							/* Delet the sub table */
							ifx_pce_tm_ip_dasa_lsb_tbl_delete(&pIPTmHandle->pce_sub_tbl, Dip_tab_index);
							ifx_pce_tm_ip_dasa_lsb_tbl_delete(&pIPTmHandle->pce_sub_tbl, Sip_tab_index);
							/* Delet the entry from Multicast sw Table */
							pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
							pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
							pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
							pIGMPTbHandle->multicast_sw_table[i].valid = IFX_FALSE;
						}
						if (pIGMPTbHandle->multicast_sw_table[i].eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE) {
							/* Delet the entry from Multicast sw Table */
							pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
							pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
							pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
							pIGMPTbHandle->multicast_sw_table[i].valid = IFX_FALSE;
						}
					}
					MATCH = IFX_TRUE;
				}
				if (pPar->eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE) {
					if ((pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index == Dip_tab_index) &&	\
						(pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index == WildCard_tab_index) &&	\
						( pIGMPTbHandle->multicast_sw_table[i].valid == IFX_TRUE))	{
						pIGMPTbHandle->multicast_sw_table[i].PortMap &= ~(1 << pPar->nPortId);
						if (pIGMPTbHandle->multicast_sw_table[i].PortMap == 0) {
							/* Found the match, check if the index still taken by others */
							if ((pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[Dip_tab_index] == 0) &&	\
								( pIPTmHandle->pce_sub_tbl.ip_dasa_lsb_tbl_cnt[WildCard_tab_index] == 0)) {
								/* Delet the sub table */
								ifx_pce_tm_ip_dasa_lsb_tbl_delete(&pIPTmHandle->pce_sub_tbl, Dip_tab_index);
								ifx_pce_tm_ip_dasa_lsb_tbl_delete(&pIPTmHandle->pce_sub_tbl, WildCard_tab_index);
							}
							/* Delet the entry from Multicast sw Table */
							pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index = 0x3F;
							pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index = 0x3F;
							pIGMPTbHandle->multicast_sw_table[i].PortMap = 0;
							pIGMPTbHandle->multicast_sw_table[i].valid = IFX_FALSE;
							
							/* Change the Main entry to the include mode */
							for (j=0; j < i; j ++) {
								if ((pIGMPTbHandle->multicast_sw_table[j].DisIp_LSB_Index == Dip_tab_index) &&	\
									(pIGMPTbHandle->multicast_sw_table[j].SrcIp_LSB_Index == Sip_tab_index) &&	\
									( pIGMPTbHandle->multicast_sw_table[j].valid == IFX_TRUE)) {
									pIGMPTbHandle->multicast_sw_table[j].eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
									/* Check if there is still some other port used */
									if (pIGMPTbHandle->multicast_sw_table[j].PortMap == 0) {
										/* Delet the entry from Multicast sw Table */
										pIGMPTbHandle->multicast_sw_table[j].SrcIp_LSB_Index = 0x3F;
										pIGMPTbHandle->multicast_sw_table[j].DisIp_LSB_Index = 0x3F;
										pIGMPTbHandle->multicast_sw_table[j].PortMap = 0;
										pIGMPTbHandle->multicast_sw_table[j].valid = IFX_FALSE;
									}
								}
							}
						}
					}
				}
			}
		}
	} else {
		IFXOS_PRINT_INT_RAW("The Mode Member ERROR !!\n");
	}
	/* Now, we write into Multicast SW Table */
	for (i=0; i < pDEVHandle->IGMP_Flags.nSwTblSize ;i++) {
		pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
		pData.table_index = i;
		pData.key[1] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index;
		pData.key[0] = (0x1f << 8) | pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index;
		pData.val[0] = pIGMPTbHandle->multicast_sw_table[i].PortMap;
 		pData.valid = pIGMPTbHandle->multicast_sw_table[i].valid;
		ifx_ethsw_xwayflow_pce_table_write(pDev, &pData);
	}
	if (MATCH == IFX_FALSE)
		IFXOS_PRINT_INT_RAW("The GIP/SIP not found\n");
   /* Debug */
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
	IFXOS_PRINT_INT_RAW("Multicast SW Table Print\n");
	IFXOS_PRINT_INT_RAW("========================\n");
	for (i=0; i < 64 ;i++) {
		/* Print out debug info first */
		IFXOS_PRINT_INT_RAW("\t|Index = %d | Valid = %d | SIP_LSB = %03d | DIP_LSB = %03d | PortMAP = %02x\n",	\
			i, pIGMPTbHandle->multicast_sw_table[i].valid,pIGMPTbHandle->multicast_sw_table[i].SrcIp_LSB_Index,	\
			pIGMPTbHandle->multicast_sw_table[i].DisIp_LSB_Index,pIGMPTbHandle->multicast_sw_table[i].PortMap);
	}
	/* Print out the Multicast SW Table */
	IFX_FLOW_MULTICAST_SW_Table_Print();
	/* Print out the IP DA/SA Taboe */
	IFX_FLOW_IP_DASA_LSB_Table_Print();
#endif	/* FLOW_TABLE_DEBUG */
   return IFX_SUCCESS;
}

/*
 *  Function:    Internal function to program the registers when 802.1x and STP API are called.
 *  Description: Referene the matrix table to program the LRNLIM, PSTATE and PEN bit
 *               according to the Software architecture spec design.
 */
static void set_port_state(IFX_void_t *pDevCtx, IFX_uint32_t PortID, IFX_uint32_t ifx_stp_state, IFX_uint32_t ifx_8021_state)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t i;
	
	for (i = 0; i < sizeof(port_state_table)/sizeof(PORT_STATE_t); i++) {
		PORT_STATE_t *pTable = &port_state_table[i];
		if ((pTable->ifx_stp_state == ifx_stp_state) && (pTable->ifx_8021_state == ifx_8021_state)) {
			pEthSWDevHandle->PortConfig[PortID].nLearningLimit = pTable->lrnlim;
			pEthSWDevHandle->PortConfig[PortID].bPortEnable = pTable->pen_reg;
			pEthSWDevHandle->PortConfig[PortID].nPortState = pTable->pstate_reg;
			/* Learning Limit */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * PortID)),	\
				VR9_PCE_PCTRL_1_LRNLIM_SHIFT, VR9_PCE_PCTRL_1_LRNLIM_SIZE,	\
				pEthSWDevHandle->PortConfig[PortID].nLearningLimit);
			/* Port State */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_0_PSTATE_OFFSET + (0xA * PortID)),	\
				VR9_PCE_PCTRL_0_PSTATE_SHIFT, VR9_PCE_PCTRL_0_PSTATE_SIZE,	\
				pEthSWDevHandle->PortConfig[PortID].nPortState);
			/* Port Enable */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_SDMA_PCTRL_PEN_OFFSET + (0xA * PortID)),	\
				VR9_SDMA_PCTRL_PEN_SHIFT, VR9_SDMA_PCTRL_PEN_SIZE,	\
				pEthSWDevHandle->PortConfig[PortID].bPortEnable);
		}
	}
}

/*
 *  Function:    Internal function to calculate the Rate when Shaper and Meter API is called.
 *  Description: Calculate the Rate by input Ibs, Exp and Mant.
 *               The algorithm designed based on software architecture spec.
 */
static IFX_uint32_t RateCalc(IFX_uint32_t pIbsIdx, IFX_uint32_t pExp, IFX_uint32_t pMant)
{
	static const IFX_uint16_t ibs_table[] = {8*8, 32*8, 64*8, 96*8};
	IFX_uint16_t ibs;
	IFX_uint32_t Rate;
	
	if  ((pIbsIdx == 0) && (pExp == 0) && (pMant == 0))
		return 0;
	ibs = ibs_table[pIbsIdx];
	Rate = ((ibs * 25000) >> pExp) / pMant;
	return Rate;
}

/*
 *  Function:    Internal function to calculate the Token when Shaper and Meter API is called.
 *  Description: Calculate the Token by input Rate, Ibs, Exp and Mant.
 *               The algorithm designed based on software architecture spec.
 */
static IFX_return_t calcToken(IFX_uint32_t Rate, IFX_uint32_t *pIbsIdx, IFX_uint32_t *pExp, IFX_uint32_t *pMant)
{
	static const IFX_uint16_t ibs_table[] = {8*8, 32*8, 64*8, 96*8};
	IFX_uint8_t i;

	for (i = 3; i >= 0; i--) {
		IFX_uint32_t exp;
		IFX_uint16_t ibs = ibs_table[i];
		for (exp = 0; exp < 16; exp++) {
			IFX_uint32_t mant =  ((ibs * 25000) >> exp) / Rate ;
			if (mant < (1 << 10) )  {
				/* target is to get the biggest mantissa value that can be used for the 10-Bit register */
				*pIbsIdx = i;
				*pExp = exp;
				*pMant = mant;
				return IFX_SUCCESS; 
			}
		}
	}
}

static void IFX_FLOW_VLAN_Table_Init ( IFX_void_t *pDevCtx)
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint16_t i;
	
	for ( i = 0; i < IFX_FLOW_VLAN_ENTRY_MAX; i++ ) {
		memset(&pEthSWDev->VLAN_Table[i], 0, sizeof(IFX_FLOW_VLAN_tableEntry_t));
	}
}   /* -----  end of function IFX_FLOW_VLAN_Table_Init  ----- */

#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
void IFX_FLOW_PCE_TRAFFIC_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i;
	
	IFXOS_PRINT_INT_RAW("|======PCE Traffic FLOW Table =======|\n");
	for (i = 0; i < 64; i++) {
		pData.table = IFX_ETHSW_PCE_TFLOW_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("=================================================================================\n");
			IFXOS_PRINT_INT_RAW("\t|Index = %03x|\n", i);
			IFXOS_PRINT_INT_RAW("\t|PPPoE index = %03x|Packet length index = %03x|\n", (pData.key[7]>>8) & 0xFF, pData.key[7] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|DSCP value  = %03x|PCP value           = %03x|\n", (pData.key[6]>>8) & 0x7F, pData.key[6] & 0xF);
			IFXOS_PRINT_INT_RAW("\t|Des MAC     = %03x|Src MAC             = %03x|\n", (pData.key[5]>>8) & 0xFF, pData.key[5] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|Des APP     = %03x|Src APP             = %03x|\n", (pData.key[4]>>8) & 0xFF, pData.key[4] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|DIP MSB     = %03x|DPI LSB             = %03x|\n", (pData.key[3]>>8) & 0xFF, pData.key[3] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|SIP MSB     = %03x|SPI LSB             = %03x|\n", (pData.key[2]>>8) & 0x1F, pData.key[2] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|IP Protocol = %03x|Enter Type          = %03x|\n", (pData.key[1]>>8) & 0xFF, pData.key[1] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|VLAN index  = %03x|Port ID             = %03x|\n\n",(pData.key[0]>>8) & 0xFF, pData.key[0] & 0xFF);

			IFXOS_PRINT_INT_RAW("\t|Port map act= %03x|VLAN action         = %03x|\n", pData.val[0] & 0x1, pData.val[0] & (1 << 1));
			IFXOS_PRINT_INT_RAW("\t|Traffic act = %03x|Remarking action    = %03x|\n", pData.val[0] & (1 << 2), pData.val[0] & (1 << 3));
			IFXOS_PRINT_INT_RAW("\t|Cross VLAN  = %03x|Cross state action  = %03x|\n", pData.val[0] & (1 << 4), pData.val[0] & (1 << 5));
			IFXOS_PRINT_INT_RAW("\t|Critical act= %03x|Time stamp action   = %03x|\n", pData.val[0] & (1 << 6), pData.val[0] & (1 << 7));
			IFXOS_PRINT_INT_RAW("\t|IRQ Egress  = %03x|Learning action     = %03x|\n", pData.val[0] & (1 << 8), pData.val[0] & (1 << 9));
			IFXOS_PRINT_INT_RAW("\t|Snooping act= %03x|Metering action     = %03x|\n", pData.val[0] & (1 << 10), pData.val[0] & (1 << 11));
			IFXOS_PRINT_INT_RAW("\t|RMON act    = %03x|VLAN default        = %03x|\n", pData.val[0] & (1 << 12), pData.val[0] & (1 << 13));
			IFXOS_PRINT_INT_RAW("\t|TrafficClass= %03x|IRQ Special Tag     = %03x|\n", pData.val[0] & (1 << 14), pData.val[0] & (1 << 15));
			IFXOS_PRINT_INT_RAW("\t|Traffic act = %03x|VLAN action         = %03x|\n\n", pData.val[0] & (1 << 2), pData.val[0] & (1 << 3));
			IFXOS_PRINT_INT_RAW("\t|Port Map    = %03x|\n", pData.val[1]);
			IFXOS_PRINT_INT_RAW("\t|Alt FID     = %03x|Alt VLAN index      = %03x|\n",pData.val[2]>>8, pData.val[2] & 0xFF);
			IFXOS_PRINT_INT_RAW("\t|VALUE[3]    = %08x|\n",pData.val[3]);
			IFXOS_PRINT_INT_RAW("\t|VALUE[4]    = %08x|\n",pData.val[4]);
		}
	}
}   /* -----  end of function Traffic Flow Table  ----- */

void IFX_FLOW_MulticastHW_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i, j, empty = 1, port = 0;
	
	IFXOS_PRINT_INT_RAW("|======Multicast Hardware Data Table =======|\n");
	for (i = 0; i < 64; i++) {
		pData.table = IFX_ETHSW_PCE_MULTICAST_HW_INDEX;
		pData.table_index = i;
		empty = 1;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		for (j = 0; j < 12; j++) {
			if (((pData.val[0] >> j) & 0x1) == 1) {
				port = j;
				/* Print out debug info first */
				IFXOS_PRINT_INT_RAW("|Index = %d | GroupIPv4 = %03d.%03d.%03d.%03d | Port = %d\n",i,	\
					(pData.key[1] >> 8) & 0xFF, (pData.key[1] & 0xFF),	\
					(pData.key[0] >> 8) & 0xFF, (pData.key[0] & 0xFF), port);
			}
		}
	}
}   /* -----  end of function PPPoE Data Table  ----- */

void IFX_FLOW_PPPOE_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	
	IFXOS_PRINT_INT_RAW("|======PPPoE Data Table =======|\n");
	for (i = 0; i < 16; i++) {
		pData.table = IFX_ETHSW_PCE_PPPOE_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("|Index = %d |PPPoE Session ID = %02x \n", i, pData.key[0]);
		}
	}
}   /* -----  end of function PPPoE Data Table  ----- */

void IFX_FLOW_PROTOCOL_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	
	IFXOS_PRINT_INT_RAW("|======Protocol Data Table =======|\n");
	for (i = 0; i < 32; i++) {
		pData.table = IFX_ETHSW_PCE_PROTOCOL_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("\t|Index = %d |EnterType = %02x | EnterType or protocol Mask = %02x\n",	\
				i, (pData.key[0]), pData.mask);
		}
	}
}   /* -----  end of function Protocol Data Table  ----- */

void IFX_FLOW_APP_DATA_MSB_LSB_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	
	IFXOS_PRINT_INT_RAW("|======Application Data Table =======|\n");
	for (i = 0; i < 64; i++) {
		pData.table = IFX_ETHSW_PCE_APPLICATION_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
       IFXOS_PRINT_INT_RAW("\t|Index = %d |Application Data = %02x | Mode Mask = %03x | Mode Range = %02x\n",	\
       	i, pData.key[0], pData.mask, pData.mask);
       }
    }
}   /* -----  end of function Application Data Table  ----- */


void IFX_FLOW_PacketLength_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;

	IFXOS_PRINT_INT_RAW("|======Packet Length Table =======|\n");
	for (i = 0; i < 16; i++) {
		pData.table = IFX_ETHSW_PCE_PACKET_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("\t|Index = %d | Packet Length = %02x | Packet Length Range = %02x\n",	\
				i, (pData.key[0]), pData.mask);
		}
	}
}   /* -----  end of function Packet Length Table  ----- */

void IFX_FLOW_MAC_DASA_LSB_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	
	IFXOS_PRINT_INT_RAW("|====== MAC DASA Table =======|\n");
	for (i = 0; i < 64; i++) {
		pData.table = IFX_ETHSW_PCE_MAC_DASA_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("\t|MAC DA/SA LSB Table = %02d | MAC = %03x:%03x:%03x:%03x:%03x:%03x| mask = %02x\n",	\
			i, (pData.key[0] & 0xFF), ((pData.key[0] >> 8) & 0xFF), (pData.key[1] & 0xFF),	\
			((pData.key[1] >> 8) & 0xFF),(pData.key[2] & 0xFF), ((pData.key[2] >> 8) & 0xFF), pData.mask);
		}
	}
}   /* -----  end of function MAC DA/SA LSB Table  ----- */

void IFX_FLOW_IP_DASA_LSB_Table_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	IFXOS_PRINT_INT_RAW("|======IP DASA LSB Table =======|\n");
	for (i = 0; i < 64; i++) {
		pData.table = IFX_ETHSW_PCE_IP_DASA_LSB_INDEX;
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("\t|IP DA/SA LSB Table = %02d | DescIP = %03d.%03d.%03d.%03d | mask = %02x\n",	\
			i, ((pData.key[1] >> 8) & 0xFF), (pData.key[1] & 0xFF), ((pData.key[0] >> 8) & 0xFF),	\
			(pData.key[0] & 0xFF), pData.mask);
		}
	}
}   /* -----  end of function IP DA/SA LSB Table  ----- */

void IFX_FLOW_VLAN_ActionMappingTable_Print(IFX_void_t)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i ;
	
	IFXOS_PRINT_INT_RAW("|======VLAN Action/Mapping Table =======|\n");
	for (i = 0; i < IFX_FLOW_VLAN_ENTRY_MAX; i++) {
		pData.table = IFX_ETHSW_PCE_ACTVLAN_INDEX; // PCE VLAN ACTIVE Table
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		if (pData.valid == 1) {
			/* Print out debug info first */
			IFXOS_PRINT_INT_RAW("\t|  VLAN Action Table  | Table = %02d  | Byte0 = %d | Byte1 = %d| FID = %d	\
			 | RFLAG = %d\n", i, (pData.key[0] & 0xFF), ((pData.key[0] >> 8) & 0xFF), (pData.val[0] & 0xFF),	\
			 ((pData.key[0] >> 8) & 0x1 ) );
		}
	}
	for (i = 0; i < IFX_FLOW_VLAN_ENTRY_MAX; i++) {
		pData.table = IFX_ETHSW_PCE_VLANMAP_INDEX; // PCE VLAN Mapping Table
		pData.table_index = i;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
		/* Print out debug info first */
		IFXOS_PRINT_INT_RAW("\t|  VLAN Mapping Table  | Index = %02d  | Byte0 = %d | Byte1 = %d	\
		| PortMap = %x | TagMap = %x\n", pData.table_index, (pData.val[0] & 0xFF),	\
		((pData.val[0] >> 8) & 0xFF), pData.val[1], pData.val[2]);
	}
}   /* -----  end of function VLAN Action & Mapping Table  ----- */

void IFX_FLOW_VLAN_Table_Print ( IFX_void_t *pDevCtx )
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t i ;
	
	for ( i = 0; i < IFX_FLOW_VLAN_ENTRY_MAX; i += 1 ) {
		IFXOS_PRINT_INT_RAW("entry[%2d]:\n", i);
		IFXOS_PRINT_INT_RAW("            vid = %d\n", pEthSWDev->VLAN_Table[i].vid);
		IFXOS_PRINT_INT_RAW("            fid = %d\n", pEthSWDev->VLAN_Table[i].fid);
		//    IFXOS_PRINT_INT_RAW("            tf  = %d\n", pEthSWDev->VLAN_Table[i].tf);
		IFXOS_PRINT_INT_RAW("            pm  = %d\n", pEthSWDev->VLAN_Table[i].pm);
		IFXOS_PRINT_INT_RAW("            tm  = %d\n", pEthSWDev->VLAN_Table[i].tm);
	}
}  /* -----  end of function IFX_FLOW_VLAN_Table_Print  ----- */
#endif /* FLOW_TABLE_DEBUG) */

IFX_uint8_t IFX_FLOW_VLAN_Table_Index_Find ( IFX_void_t *pDevCtx, IFX_uint16_t vid )
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t i, index = 0xFF;
	for ( i = 0; i < IFX_FLOW_VLAN_ENTRY_MAX; i++ ) {
		if (vid == pEthSWDev->VLAN_Table[i].vid ) {
			index = i;
			break;
		}
	}
	return index;
}   /* -----  end of function IFX_FLOW_VLAN_Table_Index_Find  ----- */

IFX_uint8_t IFX_FLOW_VLAN_Table_Empty_Entry_find ( IFX_void_t *pDevCtx  )
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t i, index = 0xFF;
	
	for ( i = 1; i < IFX_FLOW_VLAN_ENTRY_MAX; i += 1 ) { 
		if (pEthSWDev->VLAN_Table[i].valid == IFX_FALSE) {
			index = i;
			break;
		}
	}
	if ((index == 0xFF) && (pEthSWDev->VLAN_Table[0].valid == IFX_FALSE))
		return 0;
	return index;
}   /* -----  end of function IFX_FLOW_VLAN_Table_Empty_Entry_find  ----- */

static void IFX_FLOW_VLAN_Table_Entry_Set ( IFX_void_t *pDevCtx, IFX_uint8_t table_index,	\
			IFX_FLOW_VLAN_tableEntry_t *pTable_Entry )
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	
	pEthSWDev->VLAN_Table[table_index].valid = pTable_Entry->valid;
	pEthSWDev->VLAN_Table[table_index].reserved = pTable_Entry->reserved;
	pEthSWDev->VLAN_Table[table_index].vid = pTable_Entry->vid;
	pEthSWDev->VLAN_Table[table_index].fid = pTable_Entry->fid;
	//  pEthSWDev->VLAN_Table[table_index].tf = pTable_Entry->tf;
	pEthSWDev->VLAN_Table[table_index].pm = pTable_Entry->pm;
	pEthSWDev->VLAN_Table[table_index].tm = pTable_Entry->tm;
}   /* -----  end of function IFX_FLOW_VLAN_Table_Entry_Set  ----- */

void IFX_FLOW_VLAN_Table_Entry_Get ( IFX_void_t *pDevCtx, IFX_uint8_t table_index,	\
	IFX_FLOW_VLAN_tableEntry_t *pTable_Entry )
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	
	pTable_Entry->valid = pEthSWDev->VLAN_Table[table_index].valid;
	pTable_Entry->reserved = pEthSWDev->VLAN_Table[table_index].reserved;
	pTable_Entry->vid = pEthSWDev->VLAN_Table[table_index].vid;
	pTable_Entry->fid = pEthSWDev->VLAN_Table[table_index].fid;
	//  pTable_Entry->tf = pEthSWDev->VLAN_Table[table_index].tf;
	pTable_Entry->pm = pEthSWDev->VLAN_Table[table_index].pm;
	pTable_Entry->tm = pEthSWDev->VLAN_Table[table_index].tm;
	
}   /* -----  end of function IFX_FLOW_VLAN_Table_Entry_Get  ----- */

/**
*	This is the switch core layer init function.
*	\param pInit This parameter is a pointer to the switch core context.
*	\return Return value as follows:
*	pDev: if successful */
IFX_void_t *IFX_FLOW_SwitchCoreInit(IFX_FLOW_switchCoreInit_t *pInit )
{
	IFX_FLOW_switchDev_t *pDev;
	
	pDev = (IFX_FLOW_switchDev_t*) IFXOS_BlockAlloc (sizeof (IFX_FLOW_switchDev_t));
	if (!pDev) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (memory allocation failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return pDev;
    }
	memset(pDev, 0, sizeof(IFX_FLOW_switchDev_t));
	pDev->pRAL_Dev = pInit->pDev;
	pCoreDev[pInit->eDev] = pDev;
	pDev->eDev = pInit->eDev;
	//   pDev->nPortNumber = IFX_FLOW_PORT_NUMBER_MAX;
	/* -----  end switch  ----- */
	IFX_FLOW_VLAN_Table_Init(pDev);
	IFX_FLOW_PortCfg_Init(pDev);
	IFX_FLOW_MULTICAST_SW_Table_Init(pDev);
	ifx_pce_table_init(&pDev->PCE_Handler);
	pDev->bResetCalled = IFX_FALSE;
	pDev->bHW_InitCalled = IFX_FALSE;
	pDev->MAC_AgeTimer = DEFAULT_AGING_TIMEOUT;
	IFXOS_PRINT_INT_RAW("Switch API: PCE MicroCode loaded !!\n");
	IFX_VR9_Switch_PCE_Micro_Code_Int();
	return pDev;
}

/**
*	This is the switch core layer cleanup function.
*	\return Return value as follows:
*	IFX_SUCCESS: if successful */
void IFX_FLOW_SwitchCoreCleanUP(IFX_void_t )
{
	IFX_uint8_t i;
	IFX_FLOW_switchDev_t *pDev;
	for ( i=0; i<IFX_FLOW_DEV_MAX; i++) {
		pDev = (IFX_FLOW_switchDev_t*) pCoreDev[i];
		if (pDev) {
			IFXOS_BlockFree(pDev);
			pDev = IFX_NULL;
		}
	}
}

IFX_return_t IFX_FLOW_MAC_TableClear(IFX_void_t *pDevCtx)
{
	/*  flush all entries from the MAC table */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_MTFL_OFFSET,	\
		VR9_PCE_GCTRL_0_MTFL_SHIFT, VR9_PCE_GCTRL_0_MTFL_SIZE, 1);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MAC_TableEntryAdd(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableAdd_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_uint32_t index = 0;
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;

	if ( (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber) && ( !(pPar->nPortId & 0x8000)) )
		return IFX_ERROR;
	pData.valid = 1;
	for (;;) {
		pData.table =IFX_ETHSW_PCE_MAC_BRIDGE_INDEX; // PCE MAC Table Address
		pData.table_index = index;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);	
		if (pData.valid == 0) break;
		index++;
		if (index > IFX_HW_MAC_TABLE_MAX ) {
			IFXOS_PRINT_INT_RAW("ERROR: (MAC table full) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
			return IFX_ERROR;
		}
	}
	pData.table_index = index;
	pData.table = IFX_ETHSW_PCE_MAC_BRIDGE_INDEX; // PCE MAC Table Address
	pData.key[0] = pPar->nMAC[4] << 8 | pPar->nMAC[5];
	pData.key[1] = pPar->nMAC[2] << 8 | pPar->nMAC[3];
	pData.key[2] = pPar->nMAC[0] << 8 | pPar->nMAC[1];
	pData.key[3] = pPar->nFId;
	pData.valid = 1;
	if(pPar->bStaticEntry) {
		if (pPar->nPortId & 0x8000 ) { 
			pData.val[0] = ( pPar->nPortId & 0x7FFF);
			pData.val[1] = 1; 
		} else {
			pData.val[0] = (1 << pPar->nPortId);
			pData.val[1] = 0x1; 
		}
	} else {
		pData.val[0] =  ( ( (pPar->nPortId & 0xF)<< 4 ) | (pPar->nAgeTimer & 0xF) );
	}
	ifx_ethsw_xwayflow_pce_table_cam_write(pDevCtx, &pData);
	return IFX_SUCCESS;
}


IFX_return_t IFX_FLOW_MAC_TableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRead_t *pPar)
{

	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;

	if (pPar->bInitial == IFX_TRUE) {
		pEthSWDevHandle->mac_table_index = 0; /*Start from the index 0 */
		pPar->bInitial = IFX_FALSE;
	}
	if (pEthSWDevHandle->mac_table_index >= IFX_HW_MAC_TABLE_MAX) {
		memset(pPar, 0, sizeof(IFX_ETHSW_MAC_tableRead_t));
		pPar->bLast = IFX_TRUE;
		pEthSWDevHandle->mac_table_index = 0;
		return IFX_SUCCESS;
	}
	pData.table = IFX_ETHSW_PCE_MAC_BRIDGE_INDEX;
	do {
		pData.table_index = pEthSWDevHandle->mac_table_index;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);   
		pEthSWDevHandle->mac_table_index++;
		if (pData.valid != 0)
			break;
	}while (pEthSWDevHandle->mac_table_index < IFX_HW_MAC_TABLE_MAX);
	if (pData.valid != 0) {
		pPar->nFId = pData.key[3] & 0x3F;
		pPar->bStaticEntry = (pData.val[1] & 0x1) ;
		if (pPar->bStaticEntry == 1) {
			pPar->nAgeTimer = 0;
			pPar->nPortId = pData.val[0];
		} else {
			pPar->nAgeTimer = pData.val[0] & 0xF;
 			pPar->nPortId = (pData.val[0] >> 4) & 0xF;
		}
		pPar->nMAC[0] = pData.key[2] >> 8;;
		pPar->nMAC[1] = pData.key[2] & 0xFF;
		pPar->nMAC[2] = pData.key[1] >> 8;
		pPar->nMAC[3] = pData.key[1] & 0xFF;
		pPar->nMAC[4] = pData.key[0] >> 8;
		pPar->nMAC[5] = pData.key[0] & 0xFF;
		pPar->bInitial = IFX_FALSE;
		pPar->bLast = IFX_FALSE;
	} else {
		memset(pPar, 0, sizeof(IFX_ETHSW_MAC_tableRead_t));
		pPar->bLast = IFX_TRUE;
		pEthSWDevHandle->mac_table_index = 0;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MAC_TableEntryQuery(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableQuery_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	int index;
	
	pPar->bFound = IFX_FALSE;
	memset(&pData, 0, sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	pData.table = IFX_ETHSW_PCE_MAC_BRIDGE_INDEX;
	index = 0;
	do {
		pData.table_index = index;
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);   
		if (pData.valid == 1) {
			if ( (pPar->nFId == (pData.key[3] & 0x3F) ) 	\
				&& ( pPar->nMAC[0] == (pData.key[2] >> 8) )	\
				&& ( pPar->nMAC[1] == (pData.key[2] & 0xFF))	\
				&& ( pPar->nMAC[2] == (pData.key[1] >> 8) )	\
				&& ( pPar->nMAC[3] == (pData.key[1] & 0xFF) ) \
				&& ( pPar->nMAC[4] == (pData.key[0] >> 8) )	\
				&& ( pPar->nMAC[5] == (pData.key[0] & 0xFF ) ) ) {
					pPar->bFound = IFX_TRUE;
					pPar->bStaticEntry = (pData.val[1] & 0x1) ;
					if ((pData.val[1] & 0x1) == 1) {
						pPar->nAgeTimer = 0;
						pPar->nPortId = pData.val[0];
					} else {
						pPar->nAgeTimer = pData.val[0] & 0xF;
						pPar->nPortId = (pData.val[0] >> 4) & 0xF;
					}
					break;
//					return IFX_SUCCESS;
				}
			}
		index++;
	} while (index < IFX_HW_MAC_TABLE_MAX);
//	return IFX_ERROR;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MAC_TableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRemove_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_uint32_t MAC_Table_Entry, value,RemoveFlag = 0,i ;
	
	/* Learning Limit Port Lock */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_MTFL_OFFSET,	\
		VR9_PCE_GCTRL_0_MTFL_SHIFT, VR9_PCE_GCTRL_0_MTFL_SIZE, &value);
		/* Do nothing since the Delet all bit is set */
	if (!value) { /*if value is 1 means, flush all entries from the MAC table */
		for ( MAC_Table_Entry = 0; MAC_Table_Entry < IFX_HW_MAC_TABLE_MAX ; MAC_Table_Entry++) {
			pData.table = IFX_ETHSW_PCE_MAC_BRIDGE_INDEX; // PCE MAC Table Address
			pData.table_index = MAC_Table_Entry;
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);
			if((pPar->nMAC[0] == (pData.key[2] >> 8)) && (pPar->nMAC[1] == (pData.key[2] & 0xFF))	\
				&& (pPar->nMAC[2] == (pData.key[1] >> 8)) && (pPar->nMAC[3] == (pData.key[1] & 0xFF))	\
				&& (pPar->nMAC[4] == (pData.key[0] >> 8)) && (pPar->nMAC[5] == (pData.key[0] & 0xFF))	\
				&& (pPar->nFId == (pData.key[3] & 0x3F))) {
					for (i=0; i < 4; i++ ) pData.key[i] = 0;
					for (i=0; i < 2; i++ ) pData.val[i] = 0;
				pData.table = IFX_ETHSW_PCE_MAC_BRIDGE_INDEX;
				pData.table_index = MAC_Table_Entry;
				ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pData);
				RemoveFlag = 1;
			}
		}
	}
	if (RemoveFlag == 0) {
		IFXOS_PRINT_INT_RAW("WARNING: (MAC Address search Not Found) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value, Monitor_rx = 0, Monitor_tx = 0, PEN, EN, PACT, RX, TX;

	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	/* See if PORT enable or not */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_SDMA_PCTRL_PEN_OFFSET + (0x6 * portIdx)),	\
		VR9_SDMA_PCTRL_PEN_SHIFT, VR9_SDMA_PCTRL_PEN_SIZE, &PEN);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_FDMA_PCTRL_EN_OFFSET + (0x6 * portIdx)),	\
		VR9_FDMA_PCTRL_EN_SHIFT, VR9_FDMA_PCTRL_EN_SIZE, &EN);
	/* Port Enable feature only support 6 port */
	 if ( portIdx > pEthSWDevHandle->nPortNumber ) {
		pPar->eEnable = IFX_ENABLE;
	} else {
		if ((PEN == 1) && (EN == 1))
			pPar->eEnable = IFX_ETHSW_PORT_ENABLE_RXTX;
		else if ((PEN == 1) && (EN == 0))
			pPar->eEnable = IFX_ETHSW_PORT_ENABLE_RX;
		else if ((PEN == 0) && (EN == 1))
			pPar->eEnable = IFX_ETHSW_PORT_ENABLE_TX;
		else
			pPar->eEnable = IFX_ETHSW_PORT_DISABLE;
	}
	/* Learning Limit */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_1_LRNLIM_SHIFT, VR9_PCE_PCTRL_1_LRNLIM_SIZE, &value);
	pPar->nLearningLimit = value;

	/* Learning Limit Port Lock */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PCTRL_0_PLOCK_OFFSET + (0xA * portIdx)) ,	\
		VR9_PCE_PCTRL_0_PLOCK_SHIFT, VR9_PCE_PCTRL_0_PLOCK_SIZE, &value);
	pPar->bLearningMAC_PortLock = value;
	/* Aging */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_0_AGEDIS_OFFSET,	\
		VR9_PCE_PCTRL_0_AGEDIS_SHIFT, VR9_PCE_PCTRL_0_AGEDIS_SIZE, &value);
	pPar->bAging = value;
	/* UnicastUnknownDrop */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_3_UUCMAP_OFFSET,	\
		VR9_PCE_PMAP_3_UUCMAP_SHIFT, VR9_PCE_PMAP_3_UUCMAP_SIZE, &value);
	/* UnicastUnknownDrop feature only support 6 port */
	if ( portIdx < pEthSWDevHandle->nPortNumber ) {
		if( (value & ( 1 << portIdx)) == 0)
			pPar->bUnicastUnknownDrop = IFX_ENABLE;
		else
			pPar->bUnicastUnknownDrop = 0;
	} else {
		pPar->bUnicastUnknownDrop = IFX_DISABLE;
	}
	/* MulticastUnknownDrop */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_2_DMCPMAP_OFFSET,	\
		VR9_PCE_PMAP_2_DMCPMAP_SHIFT, VR9_PCE_PMAP_2_DMCPMAP_SIZE, &value);
	/* MulticastUnknownDrop feature only support 6 port */
	if ( portIdx < pEthSWDevHandle->nPortNumber ) {
		if((value & (1 << portIdx)) == 0) {
			pPar->bMulticastUnknownDrop = 1;
			pPar->bBroadcastDrop = 1;
		} else {
			pPar->bMulticastUnknownDrop = 0;
			pPar->bBroadcastDrop = 0;
		}
	} else {
		pPar->bMulticastUnknownDrop = 0;
		pPar->bBroadcastDrop = 0;
	}
	/* Require to check later - 3M */
	pPar->bReservedPacketDrop = 0;
	/* Port Monitor */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_3_RXVMIR_SHIFT, VR9_PCE_PCTRL_3_RXVMIR_SIZE, &Monitor_rx);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PCTRL_3_TXMIR_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_3_TXMIR_SHIFT, VR9_PCE_PCTRL_3_TXMIR_SIZE, &Monitor_tx);
	if ((Monitor_rx == 1 ) && (Monitor_tx == 1))
		pPar->ePortMonitor = IFX_ETHSW_PORT_MONITOR_RXTX;
	else if ((Monitor_rx == 1 ) && (Monitor_tx == 0))
		pPar->ePortMonitor = IFX_ETHSW_PORT_MONITOR_RX;
	else if ((Monitor_rx == 0 ) && (Monitor_tx == 1))
		pPar->ePortMonitor = IFX_ETHSW_PORT_MONITOR_TX;
	else
		pPar->ePortMonitor = IFX_ETHSW_PORT_MONITOR_NONE;

	/* Flow Control */
	if ( portIdx > pEthSWDevHandle->nPortNumber ) {
		pPar->eFlowCtrl = IFX_ETHSW_FLOW_OFF;
	} else {
		/* Check MDC_CFG_0.PEN_x */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MDC_CFG_0_PEN_0_OFFSET,	\
			(VR9_MDC_CFG_0_PEN_0_SHIFT + portIdx), VR9_MDC_CFG_0_PEN_0_SIZE, &PEN);
		/* Check MDIO STAT.PACT */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_STAT_0_PACT_OFFSET + portIdx),	\
			VR9_MDIO_STAT_0_PACT_SHIFT, VR9_MDIO_STAT_0_PACT_SIZE, &PACT);
	}
	RX = 0; TX = 0;
	if ((PEN == 1) && (PACT == 1)) {
		/* Read directly from MDIO register */
		IFX_ETHSW_MDIO_data_t mdio_data;
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_ADDR_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET - portIdx),	\
			VR9_PHY_ADDR_0_ADDR_SHIFT, VR9_PHY_ADDR_0_ADDR_SIZE, &value);
		mdio_data.nAddressDev = value;
		mdio_data.nAddressReg = 0x0;
		mdio_data.nData = 0;

		IFX_FLOW_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

		if ((( mdio_data.nData >> 12 ) & 0x1 ) == 1)
			pPar->eFlowCtrl = IFX_ETHSW_FLOW_AUTO;
		else
			pPar->eFlowCtrl = IFX_ETHSW_FLOW_OFF;
	} else {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_FCONTX_OFFSET - ( 0x1 * portIdx)),	\
			VR9_PHY_ADDR_0_FCONTX_SHIFT, VR9_PHY_ADDR_0_FCONTX_SIZE , &TX);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_FCONRX_OFFSET - ( 0x1 * portIdx)),	\
			VR9_PHY_ADDR_0_FCONRX_SHIFT, VR9_PHY_ADDR_0_FCONRX_SIZE, &RX);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MAC_CTRL_0_FCON_OFFSET + ( 0xC * portIdx)),	\
			VR9_MAC_CTRL_0_FCON_SHIFT, VR9_MAC_CTRL_0_FCON_SIZE, &value);
		pPar->eFlowCtrl = value;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value, EN, PEN, PACT, RX, TX, Monitor_rx, Monitor_tx;

	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber )
		return IFX_ERROR;

	/* Learning Limit Port Lock */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_0_PLOCK_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_0_PLOCK_SHIFT, VR9_PCE_PCTRL_0_PLOCK_SIZE, pPar->bLearningMAC_PortLock);
	/* Learning Limit Action */
	if (pPar->nLearningLimit == 0)
		value = 0;
	else if (pPar->nLearningLimit == 0xFFFF)
		value = 0xFF;
	else
		value = pPar->nLearningLimit;

	/* Learning Limit */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_1_LRNLIM_SHIFT, VR9_PCE_PCTRL_1_LRNLIM_SIZE, value);
	/* Aging */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_AGEDIS_OFFSET,	\
		VR9_PCE_PCTRL_0_AGEDIS_SHIFT, VR9_PCE_PCTRL_0_AGEDIS_SIZE, pPar->bAging);

	/* UnicastUnknownDrop Read first */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_3_UUCMAP_OFFSET,	\
		VR9_PCE_PMAP_3_UUCMAP_SHIFT, VR9_PCE_PMAP_3_UUCMAP_SIZE, &value);

	if (pPar->bUnicastUnknownDrop == 1) {
		if ( portIdx > pEthSWDevHandle->nPortNumber )
			return IFX_ERROR;
		else
			value &= ~( 1 << portIdx);
	} else {
		value |= 1 << portIdx;
	}
	/* UnicastUnknownDrop write back */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PMAP_3_UUCMAP_OFFSET,	\
		VR9_PCE_PMAP_3_UUCMAP_SHIFT, VR9_PCE_PMAP_3_UUCMAP_SIZE, value);

	/* MulticastUnknownDrop */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_2_DMCPMAP_OFFSET,	\
		VR9_PCE_PMAP_2_DMCPMAP_SHIFT, VR9_PCE_PMAP_2_DMCPMAP_SIZE, &value);

	if (pPar->bMulticastUnknownDrop == 1) {
		if ( portIdx > pEthSWDevHandle->nPortNumber )
			return IFX_ERROR;
		else
			value &= ~( 1 << portIdx);
	} else {
		value |= 1 << portIdx;
	}
	/* MulticastUnknownDrop */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PMAP_2_DMCPMAP_OFFSET,	\
		VR9_PCE_PMAP_2_DMCPMAP_SHIFT, VR9_PCE_PMAP_2_DMCPMAP_SIZE, value);

	/* bReservedPacketDrop */
	/* bBroadcastDrop */

	/* Flow Control */
	if ( portIdx < pEthSWDevHandle->nPortNumber) {
		/* Check MDC_CFG_0.PEN_x */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MDC_CFG_0_PEN_0_OFFSET,	\
			(VR9_MDC_CFG_0_PEN_0_SHIFT + portIdx), VR9_MDC_CFG_0_PEN_0_SIZE, &PEN);
		/* Check MDIO STAT.PACT */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_STAT_0_PACT_OFFSET + portIdx),	\
			VR9_MDIO_STAT_0_PACT_SHIFT, VR9_MDIO_STAT_0_PACT_SIZE, &PACT);
		RX = 0; TX = 0;
		if ((PEN == 1) && (PACT == 1)) {
			IFX_ETHSW_MDIO_data_t mdio_data;
			/* Write directly to MDIO register */
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_ADDR_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET - portIdx),	\
				VR9_PHY_ADDR_0_ADDR_SHIFT, VR9_PHY_ADDR_0_ADDR_SIZE, &value);
			mdio_data.nAddressDev = value;
			mdio_data.nAddressReg = 0x4;
			mdio_data.nData = 0;
			IFX_FLOW_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
			mdio_data.nData |= 0xC00;
			IFX_FLOW_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);
			/* Restart Auto negotiation */
			mdio_data.nAddressReg = 0x0;
			mdio_data.nData = 0;
			IFX_FLOW_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
			mdio_data.nData |= 0x1200;
			IFX_FLOW_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);
	} else {
		if ((pPar->eFlowCtrl == IFX_ETHSW_FLOW_AUTO) && (pPar->eFlowCtrl == IFX_ETHSW_FLOW_OFF)) {
			RX = 0;	TX = 0;
		} else if (pPar->eFlowCtrl == IFX_ETHSW_FLOW_RX) {
			RX = 1;	TX = 0;
		} else if (pPar->eFlowCtrl == IFX_ETHSW_FLOW_TX) {
			RX = 0;	TX = 1;
		} else if (pPar->eFlowCtrl == IFX_ETHSW_FLOW_RXTX) {
			RX = 1;	TX = 1;
		}
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MAC_CTRL_0_FCON_OFFSET + ( 0xC * portIdx)),	\
			VR9_MAC_CTRL_0_FCON_SHIFT, VR9_MAC_CTRL_0_FCON_SIZE, pPar->eFlowCtrl);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PHY_ADDR_0_FCONTX_OFFSET - ( 0x1 * portIdx)),	\
			VR9_PHY_ADDR_0_FCONTX_SHIFT, VR9_PHY_ADDR_0_FCONTX_SIZE , TX);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PHY_ADDR_0_FCONRX_OFFSET - ( 0x1 * portIdx)),	\
			VR9_PHY_ADDR_0_FCONRX_SHIFT, VR9_PHY_ADDR_0_FCONRX_SIZE, RX);
		}
	}
	/* Port Monitor */
	if (pPar->ePortMonitor == IFX_ETHSW_PORT_MONITOR_RXTX) {
		Monitor_rx =1;
		Monitor_tx =1;
	} else if (pPar->ePortMonitor == IFX_ETHSW_PORT_MONITOR_RX) {
		Monitor_rx = 1;
		Monitor_tx = 0;
	} else if (pPar->ePortMonitor == IFX_ETHSW_PORT_MONITOR_TX) {
		Monitor_rx = 0;
		Monitor_tx = 1;
	} else {
		Monitor_rx = 0;
		Monitor_tx = 0;
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_3_RXVMIR_SHIFT, VR9_PCE_PCTRL_3_RXVMIR_SIZE, Monitor_rx);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_3_TXMIR_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_3_TXMIR_SHIFT, VR9_PCE_PCTRL_3_TXMIR_SIZE, Monitor_tx);
	if(pPar->eEnable == IFX_ETHSW_PORT_ENABLE_RXTX) {
		PEN = 1;
		EN = 1;
	} else if (pPar->eEnable == IFX_ETHSW_PORT_ENABLE_RX) {
		PEN = 1;
		EN = 0;
	} else if (pPar->eEnable == IFX_ETHSW_PORT_ENABLE_TX) {
		PEN = 0;
		EN = 1;
	} else {
		PEN = 0;
		EN = 0;
	}
	/* Set SDMA_PCTRL_PEN PORT enable */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_SDMA_PCTRL_PEN_OFFSET + (6 * portIdx)),	\
		VR9_SDMA_PCTRL_PEN_SHIFT, VR9_SDMA_PCTRL_PEN_SIZE, PEN);
	/* Set FDMA_PCTRL_EN PORT enable  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_EN_OFFSET + (0x6 * portIdx)),	\
		VR9_FDMA_PCTRL_EN_SHIFT, VR9_FDMA_PCTRL_EN_SIZE, EN);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_STP_BPDU_RuleGet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_BPDU_Rule_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_STP_8021X_t *pStateHandle = &pEthSWDevHandle->STP_8021x_Config;
	
	pPar->eForwardPort = pStateHandle->eSTPPortState;
	pPar->nForwardPortId = pStateHandle->nSTP_PortID;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_STP_BPDU_RuleSet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_BPDU_Rule_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_STP_8021X_t *pStateHandle = &pEthSWDevHandle->STP_8021x_Config;
	IFX_FLOW_PCE_rule_t PCE_rule;
	
	pStateHandle->eSTPPortState = pPar->eForwardPort;
	pStateHandle->nSTP_PortID   = pPar->nForwardPortId;
	memset(&PCE_rule, 0, sizeof(IFX_FLOW_PCE_rule_t));
	/* Attached the PCE rule for BPDU packet */
	/*fill the required fields, other fields are zeros */
	PCE_rule.pattern.nIndex = 61;
	PCE_rule.pattern.bEnable = IFX_TRUE;
	PCE_rule.pattern.bMAC_DstEnable = IFX_TRUE;
	PCE_rule.pattern.nMAC_Dst[0] = 0x01;
	PCE_rule.pattern.nMAC_Dst[1] = 0x80;
	PCE_rule.pattern.nMAC_Dst[2] = 0xC2;
	PCE_rule.pattern.nMAC_Dst[3] = 0x00;
	PCE_rule.pattern.nMAC_Dst[4] = 0x00;
	PCE_rule.pattern.nMAC_Dst[5] = 0x00;
	PCE_rule.action.eCrossStateAction = IFX_FLOW_PCE_ACTION_CROSS_STATE_CROSS;
	if ((pStateHandle->eSTPPortState < 4) && (pStateHandle->eSTPPortState > 0))
		PCE_rule.action.ePortMapAction = pStateHandle->eSTPPortState + 1;
	else
		IFXOS_PRINT_INT_RAW("The Forward port action incorrect\n");
	PCE_rule.action.nForwardPortMap = (1 << pStateHandle->nSTP_PortID);
	/* We prepare everything and write into PCE Table */
	if (0 != ifx_pce_rule_write(&pEthSWDevHandle->PCE_Handler,&PCE_rule))
		return IFX_ERROR;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_STP_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
	pPar->ePortState = pEthSWDevHandle->PortConfig[pPar->nPortId].ifx_stp_state;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_STP_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
	pEthSWDevHandle->PortConfig[pPar->nPortId].ifx_stp_state = pPar->ePortState;
	/* Config the Table */
	set_port_state(pDevCtx, pPar->nPortId, pEthSWDevHandle->PortConfig[pPar->nPortId].ifx_stp_state,	\
		pEthSWDevHandle->PortConfig[pPar->nPortId].ifx_8021x_state );
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_IdCreate(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdCreate_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t table_index;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	/**  validation */
	if (IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId) != 0xFF ) {
		IFXOS_PRINT_INT_RAW("This vid exists\n");
		return IFX_ERROR;
	}
	table_index = IFX_FLOW_VLAN_Table_Empty_Entry_find(pEthSWDevHandle);
	if ( table_index == 0xFF ) {
		IFXOS_PRINT_INT_RAW("There is no table entry avariable\n");
		return IFX_ERROR;
	}
	// write to mirrored table
	memset(&VLAN_Table_Entry, 0, sizeof(IFX_FLOW_VLAN_tableEntry_t));
	memset(&pcetable, 0, sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	VLAN_Table_Entry.valid = IFX_TRUE;
	VLAN_Table_Entry.vid = pPar->nVId;
	VLAN_Table_Entry.fid = pPar->nFId;
	IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	/* write to ACTIVE VLAN Table */
	/* We fill all the PCE table requirement data */
	pcetable.table_index = table_index; //index of the VLAN ID configuration
	pcetable.table = IFX_ETHSW_PCE_ACTVLAN_INDEX;
	pcetable.key[0] = pPar->nVId;
	pcetable.val[0] = pPar->nFId; //Replacement Flag=0
	pcetable.valid = IFX_TRUE;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	/* write to VLAN MAPPING Table */
	pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX;
	pcetable.val[0] = pPar->nVId;
	pcetable.val[1] = 0; //reset Port Map and Tag Map values
	pcetable.val[2] = 0; //reset Port Map and Tag Map values
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
	IFX_FLOW_VLAN_ActionMappingTable_Print();
#endif
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_IdDelete(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdDelete_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint8_t table_index;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;
	IFX_PCE_t *pVlanTmHandle = &pEthSWDevHandle->PCE_Handler;
	
	memset(&VLAN_Table_Entry, 0, sizeof(IFX_FLOW_VLAN_tableEntry_t));
	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);
	if (table_index == 0xFF) {
		IFXOS_PRINT_INT_RAW("ERROR: (VID does not exists) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	if ( get_ifx_pce_tm_vlan_act_tbl_index(&pVlanTmHandle->pce_sub_tbl,table_index)!= IFX_SUCCESS) {
		IFXOS_PRINT_INT_RAW("ERROR: (VID: 0x%0x used by flow table) %s:%s:%d \n",pPar->nVId, __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	/*Remove the vlan entry from the Software index table */
	IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	pcetable.table_index = table_index; /*index of the VLAN ID configuration */
	pcetable.table = IFX_ETHSW_PCE_ACTVLAN_INDEX;
	pcetable.valid = IFX_FALSE;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_IdGet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdGet_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t table_index;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;
	
	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);
	if (table_index == 0xFF) {
		IFXOS_PRINT_INT_RAW("ERROR: (VID does not exists) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	IFX_FLOW_VLAN_Table_Entry_Get ( pEthSWDevHandle, table_index, &VLAN_Table_Entry );
	pPar->nFId = VLAN_Table_Entry.fid;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portCfg_t *pPar)
{
	IFX_uint32_t value;
	IFX_int_t table_index;
	IFX_boolean_t bUVR, bVIMR, bVEMR;
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;
	
	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
   /* nPortId:port identification (0...6)
    * nPVid: read from PCE_DEFPVID,
    * retrieve the corresponding VLAN ID from the Active VLAN Table */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_DEFPVID_PVID_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_DEFPVID_PVID_SHIFT, VR9_PCE_DEFPVID_PVID_SIZE, &value);
    		
    table_index = value;
    IFX_FLOW_VLAN_Table_Entry_Get ( pEthSWDevHandle, table_index, &VLAN_Table_Entry );
    pPar->nPortVId = VLAN_Table_Entry.vid;
    /* bVlanUnknownDrop:
    * read from PCE_VCTRL.VIMR.UVR  */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_VCTRL_UVR_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_VCTRL_UVR_SHIFT, VR9_PCE_VCTRL_UVR_SIZE, &value);
    bUVR = value;
    if ( bUVR == 1 )
    	pPar->bVLAN_UnknownDrop = IFX_TRUE;
    else
    	pPar->bVLAN_UnknownDrop = IFX_FALSE;
	/* bVlanReAssign:
    * read from PCE_VCTRL.VSR */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_VCTRL_VSR_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_VCTRL_VSR_SHIFT, VR9_PCE_VCTRL_VSR_SIZE, &value);
    pPar->bVLAN_ReAssign = value;
    /* bVlanMemberViolationIngress:
    * read from PCE_VCTRL.VIMR    */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_VCTRL_VIMR_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_VCTRL_VIMR_SHIFT, VR9_PCE_VCTRL_VIMR_SIZE, &value);
    bVIMR = value;
    /* bVlanMemberViolationEgress:
    * read from PCE_VCTRL.VEMR */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_VCTRL_VEMR_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_VCTRL_VEMR_SHIFT, VR9_PCE_VCTRL_VEMR_SIZE, &value);
    bVEMR = value;
    if (bVIMR == IFX_FALSE && bVEMR == IFX_FALSE)
    	pPar->eVLAN_MemberViolation = IFX_ETHSW_VLAN_MEMBER_VIOLATION_NO;
    if (bVIMR == IFX_TRUE && bVEMR == IFX_FALSE)
    	pPar->eVLAN_MemberViolation = IFX_ETHSW_VLAN_MEMBER_VIOLATION_INGRESS;
    if (bVIMR == IFX_FALSE && bVEMR == IFX_TRUE)
    	pPar->eVLAN_MemberViolation = IFX_ETHSW_VLAN_MEMBER_VIOLATION_EGRESS;
    if (bVIMR == IFX_TRUE && bVEMR == IFX_TRUE)
    	pPar->eVLAN_MemberViolation = IFX_ETHSW_VLAN_MEMBER_VIOLATION_BOTH;
    /* eAdmitMode:
    * read from PCE_VCTRL.VINR  */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_VCTRL_VINR_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_VCTRL_VINR_SHIFT, VR9_PCE_VCTRL_VINR_SIZE, &value);
    switch ( value ) {
    	case 0:
    		pPar->eAdmitMode = IFX_ETHSW_VLAN_ADMIT_ALL;
    		break;
    	case 1:
    		pPar->eAdmitMode = IFX_ETHSW_VLAN_ADMIT_TAGGED;
    		break;
    	case 2:
    		pPar->eAdmitMode = IFX_ETHSW_VLAN_ADMIT_UNTAGGED;
    		break;
    	default:
    		break;
    } /* -----  end switch  ----- */
    /* bTLS:
    * read from PCE_PCTRL.TVM  */
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_0_TVM_OFFSET + (10 * pPar->nPortId),	\
    		VR9_PCE_PCTRL_0_TVM_SHIFT, VR9_PCE_PCTRL_0_TVM_SIZE, &value);
    pPar->bTVM = value;
    return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value;
	IFX_uint8_t table_index;

	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	/**  nPortId: port identification (0...6)
	* nPVid: write the VLAN index to PCE_DEFPVID */
	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nPortVId);
	if (table_index == 0xFF) {
		IFXOS_PRINT_INT_RAW("ERROR: (VID does not exists) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	value = table_index;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_DEFPVID_PVID_OFFSET + (10 * pPar->nPortId),	\
			VR9_PCE_DEFPVID_PVID_SHIFT, VR9_PCE_DEFPVID_PVID_SIZE, value);
	/** bVlanUnknownDrop:
	* write to PCE_VCTRL.VIMR/.UVR **/
	value = 0;
	if ( pPar->bVLAN_UnknownDrop == IFX_TRUE ) {
		value = 1;	
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_UVR_OFFSET + (10 * pPar->nPortId),	\
			VR9_PCE_VCTRL_UVR_SHIFT, VR9_PCE_VCTRL_UVR_SIZE, value);
	/** bVlanReAssign:
	* write to PCE_VCTRL.VSR */
	value = pPar->bVLAN_ReAssign;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VSR_OFFSET + (10 * pPar->nPortId),	\
			VR9_PCE_VCTRL_VSR_SHIFT, VR9_PCE_VCTRL_VSR_SIZE, value);
	/** bVlanMemberViolationIngress:
	* write to PCE_VCTRL.VIMR */
	switch ( pPar->eVLAN_MemberViolation ) {
		case IFX_ETHSW_VLAN_MEMBER_VIOLATION_NO:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VIMR_OFFSET + (10 * pPar->nPortId),	\
						VR9_PCE_VCTRL_VIMR_SHIFT, VR9_PCE_VCTRL_VIMR_SIZE, 0);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VEMR_OFFSET + (10 * pPar->nPortId),	\
						VR9_PCE_VCTRL_VEMR_SHIFT, VR9_PCE_VCTRL_VEMR_SIZE, 0);
			break;
		case IFX_ETHSW_VLAN_MEMBER_VIOLATION_INGRESS:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VIMR_OFFSET + (10 * pPar->nPortId),	\
						VR9_PCE_VCTRL_VIMR_SHIFT, VR9_PCE_VCTRL_VIMR_SIZE, 1);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VEMR_OFFSET + (10 * pPar->nPortId),	\
						VR9_PCE_VCTRL_VEMR_SHIFT, VR9_PCE_VCTRL_VEMR_SIZE, 0);
			break;
		case IFX_ETHSW_VLAN_MEMBER_VIOLATION_EGRESS:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VIMR_OFFSET + (10 * pPar->nPortId),	\
					VR9_PCE_VCTRL_VIMR_SHIFT, VR9_PCE_VCTRL_VIMR_SIZE, 0);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VEMR_OFFSET + (10 * pPar->nPortId),	\
					VR9_PCE_VCTRL_VEMR_SHIFT, VR9_PCE_VCTRL_VEMR_SIZE, 1);
			break;
		case IFX_ETHSW_VLAN_MEMBER_VIOLATION_BOTH:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VIMR_OFFSET + (10 * pPar->nPortId),	\
						VR9_PCE_VCTRL_VIMR_SHIFT, VR9_PCE_VCTRL_VIMR_SIZE, 1);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VEMR_OFFSET + (10 * pPar->nPortId),	\
					VR9_PCE_VCTRL_VEMR_SHIFT, VR9_PCE_VCTRL_VEMR_SIZE, 1);
			break;
		default:
			IFXOS_PRINT_INT_RAW("WARNING: (eVLAN_MemberViolation) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
	} /* -----  end switch  ----- */
	/** eAdmitMode:
	* write to PCE_VCTRL.VINR */
	switch ( pPar->eAdmitMode ) {
		case IFX_ETHSW_VLAN_ADMIT_ALL:
			value = 0;
			break;
		case IFX_ETHSW_VLAN_ADMIT_TAGGED:
			value = 1;
			break;
		case IFX_ETHSW_VLAN_ADMIT_UNTAGGED:
			value = 2;
			break;
		default:
			value = 0;
			IFXOS_PRINT_INT_RAW("WARNING: (eAdmitMode) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
	} /* -----  end switch  ----- */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_VCTRL_VINR_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_VCTRL_VINR_SHIFT, VR9_PCE_VCTRL_VINR_SIZE, value);
	/** bTLS:
	* write to PCE_PCTRL.TVM */
	value = 0;
	if ( pPar->bTVM == IFX_TRUE) {
		value = 1;
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_TVM_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_0_TVM_SHIFT, VR9_PCE_PCTRL_0_TVM_SIZE, value);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_PortMemberAdd(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberAdd_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint8_t table_index;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;

	if ( (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber) && ( !(pPar->nPortId & 0x80)) )
		return IFX_ERROR;
		
	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);
	if (table_index == 0xFF) {
		IFXOS_PRINT_INT_RAW("ERROR: (VID does not exists) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	IFX_FLOW_VLAN_Table_Entry_Get(pEthSWDevHandle, table_index, &VLAN_Table_Entry); 
	if (VLAN_Table_Entry.reserved == IFX_TRUE) {
		IFXOS_PRINT_INT_RAW("ERROR: (VID was already reserved) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	/*  Support  portmap information. 
	*  To differentiate between port index and portmap, the MSB (highest data bit) should be 1. */
	if (pPar->nPortId & 0x80) {
		VLAN_Table_Entry.pm |= ( (pPar->nPortId) & 0x7F ) ;
		if (pPar->bVLAN_TagEgress)
			VLAN_Table_Entry.tm |= ( (pPar->nPortId) & 0x7F ) ;
		else
			VLAN_Table_Entry.tm &= ~((pPar->nPortId) & 0x7F);
	} else {
		VLAN_Table_Entry.pm |= 1 << pPar->nPortId; /*  single port index */
		if (pPar->bVLAN_TagEgress)
			VLAN_Table_Entry.tm |= 1 << pPar->nPortId;
		else
			VLAN_Table_Entry.tm &= ~(1 << pPar->nPortId);
	}
	IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	/* Read VID from the table */
	pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX;
	pcetable.table_index = table_index; 
	ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
	pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX; 
	pcetable.table_index = table_index; 
	pcetable.val[1] = VLAN_Table_Entry.pm;
	pcetable.val[2] = VLAN_Table_Entry.tm;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_PortMemberRead(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberRead_t *pPar)
{

	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	
	if (pPar->bInitial == IFX_TRUE) {
		pEthSWDevHandle->vlan_table_index = 0; /*Start from the index 0 */
		pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX;
		pcetable.table_index = pEthSWDevHandle->vlan_table_index; /* vlan_index retrieved from the Active VLAN table */
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pPar->nVId				= (pcetable.val[0] & 0xFFF);
		pPar->nPortId			= (pcetable.val[1] & 0xFF);
		if ( pcetable.val[2] )
			pPar->nTagId = IFX_ENABLE;
		else
			pPar->nTagId = IFX_DISABLE;
		pPar->bInitial = IFX_FALSE;
		pPar->bLast = IFX_FALSE;
	}

	if ( (pPar->bLast != IFX_TRUE)  ) {
		if (pEthSWDevHandle->vlan_table_index < IFX_HW_VLAN_MAP_TABLE_MAX) {
			pEthSWDevHandle->vlan_table_index++;
			pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX;
			pcetable.table_index = pEthSWDevHandle->vlan_table_index; /* vlan_index retrieved from the Active VLAN table */
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
			pPar->nVId				= (pcetable.val[0] & 0xFFF);
			pPar->nPortId			= (pcetable.val[1] & 0xFF);
			if ( pcetable.val[2] )
				pPar->nTagId = IFX_ENABLE;
			else
				pPar->nTagId = IFX_DISABLE;
		} else {
			pPar->bLast = IFX_TRUE;
			pEthSWDevHandle->vlan_table_index = 0;
		}
	} 
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_PortMemberRemove(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberRemove_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;
	IFX_uint8_t table_index;
	if ( (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber) && ( !(pPar->nPortId & 0x80)) )
		return IFX_ERROR;
	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);
	if (table_index == 0xFF) {
		IFXOS_PRINT_INT_RAW("This vid doesn't exists\n");
		return IFX_ERROR;
	}
	IFX_FLOW_VLAN_Table_Entry_Get(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	/*  Support  portmap information. 
	*  To differentiate between port index and portmap, the MSB (highest data bit) should be 1. 
	*/
	if (pPar->nPortId & 0x80) {
		VLAN_Table_Entry.pm  &= ~( (pPar->nPortId) & 0x7F ) ; 
	} else {
		VLAN_Table_Entry.pm &= ~(1 << pPar->nPortId);
	}
	VLAN_Table_Entry.tm &= ~(1 << pPar->nPortId);
	IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	pcetable.table = IFX_ETHSW_PCE_VLANMAP_INDEX;
	pcetable.table_index = table_index; //vlan_index retrieved from the Active VLAN table
	ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
	pcetable.val[1] &= ~(1 << pPar->nPortId);
	pcetable.val[2] &= ~(1 << pPar->nPortId);
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_ReservedAdd(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_reserved_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint8_t table_index;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;

	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);

	if (table_index == 0xFF) {
		IFXOS_PRINT_INT_RAW("This vid doesn't exists, Please create VID first\n");
		return IFX_ERROR;
	}
	IFX_FLOW_VLAN_Table_Entry_Get(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	if ( VLAN_Table_Entry.pm != 0) {
		IFXOS_PRINT_INT_RAW("This VID was already add into the member, can not reserve\n");
		return IFX_ERROR;
	}
	if ( VLAN_Table_Entry.tm != 0) {
		IFXOS_PRINT_INT_RAW("This VID was already add into the member, can not reserve\n");
		return IFX_ERROR;
	}
	VLAN_Table_Entry.reserved = IFX_TRUE;
	IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	pcetable.table_index = table_index; //index of the VLAN ID configuration
	pcetable.table = IFX_ETHSW_PCE_ACTVLAN_INDEX;
	pcetable.val[0] |= (1 << 8);
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VLAN_ReservedRemove(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_reserved_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint8_t table_index;
	IFX_FLOW_VLAN_tableEntry_t VLAN_Table_Entry;

	table_index = IFX_FLOW_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);
   if (table_index == 0xFF) {
   		IFXOS_PRINT_INT_RAW("This vid doesn't exists, Please create VID first\n");
   		return IFX_ERROR;
   	}
	IFX_FLOW_VLAN_Table_Entry_Get(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	if ( VLAN_Table_Entry.pm != 0) {
		IFXOS_PRINT_INT_RAW("This VID was already add into the member, can not remove the reserve\n");
		return IFX_ERROR;
	}
	if ( VLAN_Table_Entry.tm != 0) {
		IFXOS_PRINT_INT_RAW("This VID was already add into the member, can not remove the reserve\n");
		return IFX_ERROR;
	}
	if (VLAN_Table_Entry.reserved == IFX_FALSE) {
		IFXOS_PRINT_INT_RAW("This VID was not reserve, please reserve it first\n");
		return IFX_ERROR;
	} else {
		VLAN_Table_Entry.reserved = IFX_FALSE;
		IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	}
	IFX_FLOW_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
	pcetable.table_index = table_index; //index of the VLAN ID configuration
	pcetable.table = IFX_ETHSW_PCE_ACTVLAN_INDEX;
	pcetable.val[0] &=~ (1 << 8);
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_MeterCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterCfg_t *pPar)
{
	IFX_uint32_t nMeterId = pPar->nMeterId;
	IFX_uint32_t  value, exp, mant, ibs;

	if( nMeterId > 7 )
		return IFX_ERROR;

	/* Enable/Disable the meter shaper */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TCM_CTRL_TCMEN_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CTRL_TCMEN_SHIFT, VR9_PCE_TCM_CTRL_TCMEN_SIZE, &value);
	pPar->bEnable = value;
	/* Committed Burst Size */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TCM_CBS_CBS_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CBS_CBS_SHIFT, VR9_PCE_TCM_CBS_CBS_SIZE, &value);
	pPar->nCbs = (value * 64);
	/* Excess Burst Size (EBS [bytes]) */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TCM_EBS_EBS_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_EBS_EBS_SHIFT, VR9_PCE_TCM_EBS_EBS_SIZE, &value);
	pPar->nEbs = (value * 64);
	/* Rate Counter Exponent */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TCM_CIR_EXP_EXP_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CIR_EXP_EXP_SHIFT, VR9_PCE_TCM_CIR_EXP_EXP_SIZE, &exp);
	/* Rate Counter Mantissa */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TCM_CIR_MANT_MANT_OFFSET + (nMeterId * 7),
                              VR9_PCE_TCM_CIR_MANT_MANT_SHIFT, VR9_PCE_TCM_CIR_MANT_MANT_SIZE, &mant);
   /* Rate Counter iBS */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TCM_IBS_IBS_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_IBS_IBS_SHIFT, VR9_PCE_TCM_IBS_IBS_SIZE, &ibs);
	/* calc the Rate */
	pPar->nRate = RateCalc(ibs, exp, mant);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_MeterCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterCfg_t *pPar)
{
	IFX_uint32_t nMeterId = pPar->nMeterId;
	IFX_uint32_t nCbs = 0, nEbs = 0;
	IFX_uint32_t  exp = 0, mant = 0, rate = 0, ibs = 0;

	if( nMeterId > 7 )
		return IFX_ERROR;
	/* Committed Burst Size */
	if (pPar->nCbs > 0xFFC0)
		nCbs = 0x3FF;
	else
		nCbs = ((pPar->nCbs + 63) / 64);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TCM_CBS_CBS_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CBS_CBS_SHIFT, VR9_PCE_TCM_CBS_CBS_SIZE, nCbs);
	/* Excess Burst Size (EBS [bytes]) */
	if (pPar->nEbs > 0xFFC0)
		nEbs = 0x3FF;
	else
		nEbs = ((pPar->nEbs + 63) / 64);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TCM_EBS_EBS_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_EBS_EBS_SHIFT, VR9_PCE_TCM_EBS_EBS_SIZE, nEbs);
	/* Calc the Rate and convert to MANT and EXP*/
	rate = pPar->nRate; 
	if (rate)
		calcToken(rate, &ibs, &exp, &mant);
	/* Rate Counter Exponent */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TCM_CIR_EXP_EXP_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CIR_EXP_EXP_SHIFT, VR9_PCE_TCM_CIR_EXP_EXP_SIZE, exp);
	/* Rate Counter Mantissa */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TCM_CIR_MANT_MANT_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CIR_MANT_MANT_SHIFT, VR9_PCE_TCM_CIR_MANT_MANT_SIZE, mant);
	/* Rate Counter iBS */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TCM_IBS_IBS_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_IBS_IBS_SHIFT, VR9_PCE_TCM_IBS_IBS_SIZE, ibs);
	/* Enable/Disable the meter shaper */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TCM_CTRL_TCMEN_OFFSET + (nMeterId * 7),	\
		VR9_PCE_TCM_CTRL_TCMEN_SHIFT, VR9_PCE_TCM_CTRL_TCMEN_SIZE, pPar->bEnable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_MeterPortAssign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterPort_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  EgressPortId, IngressPortId, value1, value2;
	IFX_boolean_t EntryFound_Tbl = IFX_FALSE;
	IFX_boolean_t noEmptyEntry_Tb1 = IFX_FALSE;

	memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));

	value1 = 0;
	while ( value1 < 8 ) {
		pcetable.table = IFX_ETHSW_PCE_METER_INS_0_INDEX;
		pcetable.table_index = value1; //index of the Meter Ins 0 configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);

		if (pcetable.valid == 1) {
			IngressPortId= pcetable.key[0] & 0xF;
			EgressPortId= (pcetable.key[0] >> 8) & 0xF;
			if ((EgressPortId == pPar->nPortEgressId) && (IngressPortId == pPar->nPortIngressId)) {
				EntryFound_Tbl = IFX_TRUE;
				value2 = 0;
				while ( value2 < 8 ) {
					pcetable.table = IFX_ETHSW_PCE_METER_INS_1_INDEX;
					pcetable.table_index = value2; //index of the Meter Ins 1 configuration
					ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
					if (pcetable.valid == 1) {
						IngressPortId= pcetable.key[0] & 0xF;
						EgressPortId= (pcetable.key[0] >> 8) & 0xF;
						if ((EgressPortId == pPar->nPortEgressId) && (IngressPortId == pPar->nPortIngressId))
							return IFX_ERROR;
					}
					value2++;
				}
			}
		}
		value1++;
	}
	/*  Not in the original table, write a new one */
	if ( EntryFound_Tbl == IFX_FALSE ) {
		value1 = 0;
		memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
		/* Search for whole Table 1*/
		while ( value1 < 8 ) {
			pcetable.table = IFX_ETHSW_PCE_METER_INS_0_INDEX;
			pcetable.table_index = value1; //index of the Meter Ins 0 configuration
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
			/* We found the empty one */
			if (pcetable.valid == 0) {
				switch (pPar->eDir) {
					case IFX_ETHSW_DIRECTION_BOTH:
						pcetable.key[0] = (((pPar->nPortEgressId & 0xF ) << 8) |	\
							( pPar->nPortIngressId & 0xF));
						pcetable.mask = 0;
					break;
					case IFX_ETHSW_DIRECTION_EGRESS:
						pcetable.key[0] = (((pPar->nPortEgressId & 0xF) << 8) | 0xF);
						pcetable.mask = 1;
						break;
					case IFX_ETHSW_DIRECTION_INGRESS:
						pcetable.key[0] = (0xF00 | (pPar->nPortIngressId & 0xF) );
						pcetable.mask = 4;
						break;
					default:
						pcetable.key[0] = 0;
						pcetable.mask = 5;
				}
				pcetable.val[0] = pPar->nMeterId & 0x3F;
				pcetable.valid = 1;
				ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
				return IFX_SUCCESS;
			}
			value1++;
		}
		if (value1 >= 8)
			noEmptyEntry_Tb1 = IFX_TRUE;
	}
	
	/* The Table 1 is full, We go search table 2 */
	if ( (noEmptyEntry_Tb1 == IFX_TRUE) || (EntryFound_Tbl == IFX_TRUE)) {
		value2 = 0;
		memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
		while ( value2 < 8 ) {
			pcetable.table = IFX_ETHSW_PCE_METER_INS_1_INDEX;
			pcetable.table_index = value2; //index of the Meter Ins 0 configuration
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
			/* We found the empty one */
			if (pcetable.valid == 0) {
				switch (pPar->eDir) {
					case IFX_ETHSW_DIRECTION_BOTH:
						pcetable.key[0] = (((pPar->nPortEgressId & 0xF ) << 8) |	\
							( pPar->nPortIngressId & 0xF));
						pcetable.mask = 0;
					break;
					case IFX_ETHSW_DIRECTION_EGRESS:
						pcetable.key[0] = (((pPar->nPortEgressId & 0xF) << 8) | 0xF);
						pcetable.mask = 1;
						break;
					case IFX_ETHSW_DIRECTION_INGRESS:
						pcetable.key[0] = (0xF00 | (pPar->nPortIngressId & 0xF) );
						pcetable.mask = 4;
						break;
					default:
						pcetable.key[0] = 0;
						pcetable.mask = 5;
				}
				pcetable.val[0] = pPar->nMeterId & 0x3F;
				pcetable.valid = 1;
				ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
				noEmptyEntry_Tb1 = IFX_FALSE;
				return IFX_SUCCESS;
			}
			value2++;
		}
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_MeterPortDeassign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterPort_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  EgressPortId, IngressPortId, MeterId;
	IFX_boolean_t EntryFound_Tbl = IFX_FALSE;
	IFX_uint32_t i, j;

	for ( i = 0; i < 2;i++) {
		memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
		if ( i == 0)
			pcetable.table = IFX_ETHSW_PCE_METER_INS_0_INDEX;
		else
			pcetable.table = IFX_ETHSW_PCE_METER_INS_1_INDEX;
		for ( j = 0; j < 8; j++) {
			pcetable.table_index = j; //index of the Meter Ins 1 configuration
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
			if (pcetable.valid == 1) {
				IngressPortId= pcetable.key[0] & 0xF;
				EgressPortId= (pcetable.key[0] >> 8) & 0xF;
				MeterId = pcetable.val[0] & 0x1F;
				if ((EgressPortId == pPar->nPortEgressId) &&	\
					(IngressPortId == pPar->nPortIngressId) && (MeterId == pPar->nMeterId)) {
					if ( i == 0)
						pcetable.table = IFX_ETHSW_PCE_METER_INS_0_INDEX;
					else
						pcetable.table = IFX_ETHSW_PCE_METER_INS_1_INDEX;
					pcetable.key[0] = 0;
					pcetable.val[0] = 0;
					pcetable.valid = 0;
					ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
					EntryFound_Tbl = IFX_TRUE;
					IFXOS_PRINT_INT_RAW("Found the entry, delet it\n");
				}
				
			}
		}
	}
	if (EntryFound_Tbl == IFX_FALSE)
		IFXOS_PRINT_INT_RAW("The Entry not found\n");
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_MeterPortGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterPortGet_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	
	memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	if (pPar->bInitial == IFX_TRUE) {
		METER_COUNTER = 0;
		pPar->bInitial = IFX_FALSE;
	} else {
		if ( METER_COUNTER > 14)
			pPar->bLast = IFX_TRUE;
	}
	if( METER_COUNTER > 7)
		pcetable.table = IFX_ETHSW_PCE_METER_INS_1_INDEX;
	else
		pcetable.table = IFX_ETHSW_PCE_METER_INS_0_INDEX;
	pcetable.table_index = METER_COUNTER;
	ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
	if ( pcetable.valid) {
		pPar->nMeterId = (pcetable.val[0] & 0x3F);
		pPar->nPortEgressId = ((pcetable.key[0] >> 8) & 0xF);
		pPar->nPortIngressId = (pcetable.key[0] & 0xF);
		if ( (pcetable.mask & 0x5) == 0)
				pPar->eDir = IFX_ETHSW_DIRECTION_BOTH;
		else if ( (pcetable.mask & 0x5) == 1)
			pPar->eDir = IFX_ETHSW_DIRECTION_EGRESS;
		else if ( (pcetable.mask & 0x5) == 4)
			pPar->eDir = IFX_ETHSW_DIRECTION_INGRESS;
		else
			pPar->eDir = IFX_ETHSW_DIRECTION_NONE;
	}
	METER_COUNTER++;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_DSCP_ClassGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_ClassCfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for ( value =0; value <= 63; value++) {
		pcetable.table = IFX_ETHSW_PCE_DSCP_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pPar->nTrafficClass[value] = pcetable.val[0] & 0xF;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_DSCP_ClassSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_ClassCfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for( value =0; value <= 63; value++) {
		pcetable.table = IFX_ETHSW_PCE_DSCP_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pcetable.val[0] &= ~( 0xF  );
		pcetable.val[0] |= (pPar->nTrafficClass[value] & 0xF );
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ClassDSCP_Get(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassDSCP_Cfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for ( value =0; value < 16; value++) {
		pcetable.table = IFX_ETHSW_PCE_REMARKING_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pPar->nDSCP[value] = pcetable.val[0] & 0x3F;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ClassDSCP_Set(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassDSCP_Cfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;
	IFX_uint32_t  nDSCP, nPCP;

	for( value =0; value < 16; value++) {
		pcetable.table = IFX_ETHSW_PCE_REMARKING_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		nPCP = (pcetable.val[0] >> 8 ) & 0x7;
		nDSCP = pPar->nDSCP[value] & 0x3F;
		pcetable.val[0] = (nPCP << 8) | nDSCP;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_DSCP_DropPrecedenceCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_DropPrecedenceCfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;
	
	for ( value =0; value <= 63; value++) {
		memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
		pcetable.table = IFX_ETHSW_PCE_DSCP_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pPar->nDSCP_DropPrecedence[value] = (pcetable.val[0] >> 4) & 0x3;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_DSCP_DropPrecedenceCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_DropPrecedenceCfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for( value =0; value <= 63; value++) {
		memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
		pcetable.table = IFX_ETHSW_PCE_DSCP_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pcetable.val[0] &= ~( 0x3 << 4 );
		pcetable.val[0] |= ( (pPar->nDSCP_DropPrecedence[value] & 0x3) << 4);
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_PortRemarkingCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portRemarkingCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t  value, value_CLPEN=0, value_DPEN=0, value_DSCPMOD=0;

	if ( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_0_CLPEN_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_0_CLPEN_SHIFT, VR9_PCE_PCTRL_0_CLPEN_SIZE, &value_CLPEN);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_0_DPEN_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_0_DPEN_SHIFT, VR9_PCE_PCTRL_0_DPEN_SIZE, &value_DPEN);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_2_DSCPMOD_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_2_DSCPMOD_SHIFT, VR9_PCE_PCTRL_2_DSCPMOD_SIZE, &value_DSCPMOD);

	if (( value_CLPEN == 0 ) && ( value_DPEN == 0 ) && (value_DSCPMOD == 0))
		pPar->eDSCP_IngressRemarkingEnable = IFX_ETHSW_DSCP_REMARK_DISABLE;
	else if (( value_CLPEN == 1 ) && ( value_DPEN == 0 ) && ( value_DSCPMOD == 1 ))
		pPar->eDSCP_IngressRemarkingEnable = IFX_ETHSW_DSCP_REMARK_TC6;
	else if (( value_CLPEN == 1 ) && ( value_DPEN == 1 ) && ( value_DSCPMOD == 1 ))
		pPar->eDSCP_IngressRemarkingEnable = IFX_ETHSW_DSCP_REMARK_TC3;
	else if (( value_CLPEN == 0 ) && ( value_DPEN == 1 ) && ( value_DSCPMOD == 1 ))
		pPar->eDSCP_IngressRemarkingEnable = IFX_ETHSW_DSCP_REMARK_DP3;
	else if (( value_CLPEN == 1 ) && ( value_DPEN == 1 ) && ( value_DSCPMOD == 1 ))
		pPar->eDSCP_IngressRemarkingEnable = IFX_ETHSW_DSCP_REMARK_DP3_TC3;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_0_PCPEN_OFFSET + (10 * pPar->nPortId),\
		 VR9_PCE_PCTRL_0_PCPEN_SHIFT, VR9_PCE_PCTRL_0_PCPEN_SIZE, &value);
	pPar->bPCP_IngressRemarkingEnable = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_FDMA_PCTRL_DSCPRM_OFFSET + (6 * pPar->nPortId),	\
		VR9_FDMA_PCTRL_DSCPRM_SHIFT, VR9_FDMA_PCTRL_DSCPRM_SIZE, &value);
	pPar->bDSCP_EgressRemarkingEnable = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_FDMA_PCTRL_VLANMOD_OFFSET + (6 * pPar->nPortId),	\
		VR9_FDMA_PCTRL_VLANMOD_SHIFT, VR9_FDMA_PCTRL_VLANMOD_SIZE, &value);
	if ( value == 3)
		pPar->bPCP_EgressRemarkingEnable = IFX_ENABLE;
	else
		pPar->bPCP_EgressRemarkingEnable = IFX_DISABLE;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_PortRemarkingCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portRemarkingCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t  value, value_CLPEN=0, value_DPEN=0, value_DSCPMOD=0;

	if ( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;

	switch (pPar->eDSCP_IngressRemarkingEnable) {
		case IFX_ETHSW_DSCP_REMARK_DISABLE:
			value_CLPEN = 0;
			value_DPEN = 0;
			value_DSCPMOD = 0;
			break;
		case IFX_ETHSW_DSCP_REMARK_TC6:
         	value_CLPEN = 1;
         	value_DPEN = 0;
         	value_DSCPMOD = 1;
      		break;
		case IFX_ETHSW_DSCP_REMARK_TC3:
        	 value_CLPEN = 1;
        	 value_DPEN = 1;
        	 value_DSCPMOD = 1;
     	 	break;
		case IFX_ETHSW_DSCP_REMARK_DP3:
			value_CLPEN = 0;
			value_DPEN = 1;
			value_DSCPMOD = 1;
			break;
		case IFX_ETHSW_DSCP_REMARK_DP3_TC3:
			value_CLPEN = 1;
			value_DPEN = 1;
			value_DSCPMOD = 1;
			break;
	}

	if (pPar->eDSCP_IngressRemarkingEnable == IFX_FALSE ) {
		value_CLPEN = 0;
		value_DPEN = 0;
		value_DSCPMOD = 0;
	}

	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_CLPEN_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_0_CLPEN_SHIFT, VR9_PCE_PCTRL_0_CLPEN_SIZE, value_CLPEN);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_DPEN_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_0_DPEN_SHIFT, VR9_PCE_PCTRL_0_DPEN_SIZE, value_DPEN);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_DSCPMOD_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_2_DSCPMOD_SHIFT, VR9_PCE_PCTRL_2_DSCPMOD_SIZE, value_DSCPMOD);
	if (pPar->bDSCP_EgressRemarkingEnable > 0 )
		value = pPar->bDSCP_EgressRemarkingEnable;
	else
		value = 0;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_FDMA_PCTRL_DSCPRM_OFFSET + (6 * pPar->nPortId),	\
		VR9_FDMA_PCTRL_DSCPRM_SHIFT, VR9_FDMA_PCTRL_DSCPRM_SIZE, value);

	if (pPar->bPCP_IngressRemarkingEnable > 0 )
		value = pPar->bPCP_IngressRemarkingEnable;
	else
		value = 0;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_PCPEN_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_0_PCPEN_SHIFT, VR9_PCE_PCTRL_0_PCPEN_SIZE, value);

	if (pPar->bPCP_EgressRemarkingEnable > 0)
		value = 3;
	else
		value = 0;

	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_FDMA_PCTRL_VLANMOD_OFFSET + (6 * pPar->nPortId),	\
		VR9_FDMA_PCTRL_VLANMOD_SHIFT, VR9_FDMA_PCTRL_VLANMOD_SIZE, value);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ClassPCP_Get(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassPCP_Cfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for ( value =0; value < 16; value++) {
		pcetable.table = IFX_ETHSW_PCE_REMARKING_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pPar->nPCP[value] = (pcetable.val[0] >> 8 ) & 0x7;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ClassPCP_Set(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassPCP_Cfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  nDSCP, nPCP, value;

	for( value =0; value < 16; value++) {
		pcetable.table = IFX_ETHSW_PCE_REMARKING_INDEX;
		pcetable.table_index = value; //index of the DSCP configuration
 		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		nDSCP = pcetable.val[0] & 0x3F;
		nPCP = pPar->nPCP[value] & 0x7;
		pcetable.val[0] = (nPCP << 8) | nDSCP;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_PCP_ClassGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_PCP_ClassCfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for ( value =0; value < 8; value++) {
		pcetable.table = IFX_ETHSW_PCE_PCP_INDEX;
		pcetable.table_index = value; //index of the PCP configuration
		//IFXOS_PRINT_INT_RAW("table_index = %d\n",value);
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
		pPar->nTrafficClass[value] = pcetable.val[0] & 0xF;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_PCP_ClassSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_PCP_ClassCfg_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  value;

	for ( value = 0; value < 8; value++) {
		pcetable.table = IFX_ETHSW_PCE_PCP_INDEX;
		pcetable.table_index = value; //index of the PCP configuration
		pcetable.val[0] = pPar->nTrafficClass[value] & 0xF;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t  value, value_DSCP, value_PCP;

	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_2_DSCP_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_2_DSCP_SHIFT, VR9_PCE_PCTRL_2_DSCP_SIZE, &value_DSCP);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_2_PCP_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_2_PCP_SHIFT, VR9_PCE_PCTRL_2_PCP_SIZE, &value_PCP);

	if ((value_DSCP == 1) && (value_PCP == 1))
		pPar->eClassMode = 4;
	else if ((value_DSCP == 2) && (value_PCP == 1))
		pPar->eClassMode = 3;
	else if ((value_DSCP == 0) && (value_PCP == 1))
		pPar->eClassMode = 2;
	else if ((value_DSCP == 2) && (value_PCP == 0))
		pPar->eClassMode = 1;
	else if ((value_DSCP == 0) && (value_PCP == 0))
		pPar->eClassMode = 0;
	else
		pPar->eClassMode = 0;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PCTRL_2_PCLASS_OFFSET + (10 * pPar->nPortId),	\
		VR9_PCE_PCTRL_2_PCLASS_SHIFT, VR9_PCE_PCTRL_2_PCLASS_SIZE, &value);
	pPar->nTrafficClass = value;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;

	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;

	switch (pPar->eClassMode) {
		case IFX_ETHSW_QOS_CLASS_SELECT_NO:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_DSCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_DSCP_SHIFT, VR9_PCE_PCTRL_2_DSCP_SIZE, 0);
         	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_PCP_OFFSET + (10 * pPar->nPortId),	\
         		VR9_PCE_PCTRL_2_PCP_SHIFT, VR9_PCE_PCTRL_2_PCP_SIZE, 0);
		break;
 		case IFX_ETHSW_QOS_CLASS_SELECT_DSCP:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_DSCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_DSCP_SHIFT, VR9_PCE_PCTRL_2_DSCP_SIZE, 2);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_PCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_PCP_SHIFT, VR9_PCE_PCTRL_2_PCP_SIZE, 0);
		break;
		case IFX_ETHSW_QOS_CLASS_SELECT_PCP:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_DSCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_DSCP_SHIFT, VR9_PCE_PCTRL_2_DSCP_SIZE, 0);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_PCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_PCP_SHIFT, VR9_PCE_PCTRL_2_PCP_SIZE, 1);
		break;
		case IFX_ETHSW_QOS_CLASS_SELECT_DSCP_PCP:
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_DSCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_DSCP_SHIFT, VR9_PCE_PCTRL_2_DSCP_SIZE, 2);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_PCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_PCP_SHIFT, VR9_PCE_PCTRL_2_PCP_SIZE, 1);
		break;
		case IFX_ETHSW_QOS_CLASS_SELECT_PCP_DSCP:
 			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_DSCP_OFFSET + (10 * pPar->nPortId),	\
 				VR9_PCE_PCTRL_2_DSCP_SHIFT, VR9_PCE_PCTRL_2_DSCP_SIZE, 1);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_PCP_OFFSET + (10 * pPar->nPortId),	\
				VR9_PCE_PCTRL_2_PCP_SHIFT, VR9_PCE_PCTRL_2_PCP_SIZE, 1);
		break;
	}

	if (pPar->nTrafficClass > 0xF)
		return IFX_ERROR;
	else
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_2_PCLASS_OFFSET + (10 * pPar->nPortId),	\
			VR9_PCE_PCTRL_2_PCLASS_SHIFT, VR9_PCE_PCTRL_2_PCLASS_SIZE, pPar->nTrafficClass);

	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_QueuePortGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_queuePort_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;

	memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	/* Set the PCE Queue Mapping Table */
	pcetable.table = IFX_ETHSW_PCE_QUEUE_MAP_INDEX;
	pcetable.table_index = (((pPar->nPortId << 4) & 0x70) | (pPar->nTrafficClassId & 0xF));
	ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
	pPar->nQueueId = (pcetable.val[0] & 0x1F);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_QueuePortSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_queuePort_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t  EgressPortId = 0, value;

	memset(&pcetable, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	/* Set the PCE Queue Mapping Table */
	pcetable.table = IFX_ETHSW_PCE_QUEUE_MAP_INDEX;
	pcetable.table_index = ((pPar->nPortId << 4) & 0x70 ) | (pPar->nTrafficClassId & 0xF);
	pcetable.val[0] = (pPar->nQueueId & 0x1F);
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	/* Assign the Egress Port Id and Enable the Queue */
	EgressPortId = (pPar->nPortId & 0x7);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
		VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, EgressPortId);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
		VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, pPar->nQueueId);
	/* Specify the Table Address */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
		VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0xE);
	/* Assign Write operation */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_OPMOD_OFFSET,	\
		VR9_BM_RAM_CTRL_OPMOD_SHIFT, VR9_BM_RAM_CTRL_OPMOD_SIZE, 1);
	value = 1;
	/* Active */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
		 VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
	}
	/* Specify the PQMCTXT = 0x9 */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
		VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0);
	/* Table Access Operation Mode Write */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_OPMOD_OFFSET,	\
		VR9_BM_RAM_CTRL_OPMOD_SHIFT, VR9_BM_RAM_CTRL_OPMOD_SIZE, 0);
	/* Active */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
		VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, 0);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_SchedulerCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_schedulerCfg_t *pPar)
{
	IFX_int32_t nQueueId = pPar->nQueueId;
	IFX_uint32_t  value;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value);
	if ( nQueueId >= value )
		return IFX_ERROR;
	/* Set the BM RAM ADDR  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
		VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, nQueueId);
	/* Specify the WFQ = 0x8 */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
		VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0x8);
	value = 1;
	/* Active */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
		VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
	}
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
		VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, &value);
	pPar->nWeight = value;
	if( value == 0xFFFF || value == 0x1800)
 		pPar->eType = IFX_ETHSW_QOS_SCHEDULER_STRICT;
	else
		pPar->eType = IFX_ETHSW_QOS_SCHEDULER_WFQ;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_SchedulerCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_schedulerCfg_t *pPar)
{
	IFX_int32_t nQueueId = pPar->nQueueId;
	IFX_uint32_t  value;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value);
	
	if ( nQueueId >= value )
		return IFX_ERROR;
	/* Set the BM RAM ADDR  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
		VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, nQueueId);
	/* Specify the WFQ = 0x8 */
 	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
 		VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0x8);
	if( pPar->eType == IFX_ETHSW_QOS_SCHEDULER_STRICT )
		value = 0xFFFF;
	else
		value = pPar->nWeight;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
		VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, value);
	/* Table Access Operation Mode Write */
 	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_OPMOD_OFFSET,	\
 		VR9_BM_RAM_CTRL_OPMOD_SHIFT, VR9_BM_RAM_CTRL_OPMOD_SIZE, 0x1);
	value = 1;
	/* Active */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
		VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
	}
	/* Specify the PQMCTXT = 0x9 */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
		VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0);
	/* Table Access Operation Mode Write */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_OPMOD_OFFSET,	\
		VR9_BM_RAM_CTRL_OPMOD_SHIFT, VR9_BM_RAM_CTRL_OPMOD_SIZE, 0);
	/* Active */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
		VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, 0);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ShaperCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperCfg_t *pPar)
{
	IFX_int32_t nRateShaperId = pPar->nRateShaperId;
	IFX_uint32_t  value, exp, mant, ibs;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_3_SHAPERS_OFFSET,	\
		VR9_ETHSW_CAP_3_SHAPERS_SHIFT, VR9_ETHSW_CAP_3_SHAPERS_SIZE, &value);
	if ( nRateShaperId >= value)
		return IFX_ERROR;
	/* Enable/Disable the rate shaper  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_RS_CTRL_RSEN_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CTRL_RSEN_SHIFT, VR9_RS_CTRL_RSEN_SIZE, &value);
	pPar->bEnable = value;
	/* Committed Burst Size (CBS [bytes]) */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_RS_CBS_CBS_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CBS_CBS_SHIFT, VR9_RS_CBS_CBS_SIZE, &value);
	pPar->nCbs = (value * 64);
	/** Rate [Mbit/s] */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_RS_CIR_EXP_EXP_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CIR_EXP_EXP_SHIFT, VR9_RS_CIR_EXP_EXP_SIZE, &exp);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_RS_CIR_MANT_MANT_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CIR_MANT_MANT_SHIFT, VR9_RS_CIR_MANT_MANT_SIZE, &mant);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_RS_IBS_IBS_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_IBS_IBS_SHIFT, VR9_RS_IBS_IBS_SIZE, &ibs);
	/* calc the Rate */
	pPar->nRate = RateCalc(ibs, exp, mant);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ShaperCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperCfg_t *pPar)
{
	IFX_int32_t nRateShaperId = pPar->nRateShaperId;
	IFX_uint32_t  value, exp = 0, mant = 0, rate = 0, ibs = 0;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_3_SHAPERS_OFFSET,	\
		VR9_ETHSW_CAP_3_SHAPERS_SHIFT, VR9_ETHSW_CAP_3_SHAPERS_SIZE, &value);
	if ( nRateShaperId >= value)
		return IFX_ERROR;
		
	/* Committed Burst Size */
	if (pPar->nCbs > 0xFFC0)
		value = 0x3FF;
	else
		value = ((pPar->nCbs + 63) / 64);
	/* Committed Burst Size (CBS [bytes]) */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CBS_CBS_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CBS_CBS_SHIFT, VR9_RS_CBS_CBS_SIZE, value);
	/* Rate [kbit/s] */
	/* Calc the Rate */
	rate = pPar->nRate;
	if (rate)
		calcToken(rate, &ibs, &exp, &mant);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CIR_EXP_EXP_OFFSET + (nRateShaperId * 0x5),	\
		 VR9_RS_CIR_EXP_EXP_SHIFT, VR9_RS_CIR_EXP_EXP_SIZE, exp);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CIR_MANT_MANT_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CIR_MANT_MANT_SHIFT, VR9_RS_CIR_MANT_MANT_SIZE, mant);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_IBS_IBS_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_IBS_IBS_SHIFT, VR9_RS_IBS_IBS_SIZE, ibs);
	/* Enable/Disable the rate shaper  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CTRL_RSEN_OFFSET + (nRateShaperId * 0x5),	\
		VR9_RS_CTRL_RSEN_SHIFT, VR9_RS_CTRL_RSEN_SIZE, pPar->bEnable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ShaperQueueAssign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperQueue_t *pPar)
{
	IFX_uint8_t nRateShaperId = pPar->nRateShaperId;
	IFX_uint8_t nQueueId = pPar->nQueueId;
	IFX_uint32_t value1_RS, value1_ShaperId, value2_RS, value2_ShaperId;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value1_RS);
	
	if ( nQueueId >= value1_RS )
		return IFX_ERROR;
		
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_3_SHAPERS_OFFSET,	\
		VR9_ETHSW_CAP_3_SHAPERS_SHIFT, VR9_ETHSW_CAP_3_SHAPERS_SIZE, &value1_RS);
	if ( nRateShaperId >= value1_RS)
		return IFX_ERROR;

	/* Check Rate Shaper 1 Enable  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_EN1_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_EN1_SHIFT, VR9_PQM_RS_EN1_SIZE, &value1_RS);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_RS1_OFFSET + (nRateShaperId * 2),	\
		VR9_PQM_RS_RS1_SHIFT, VR9_PQM_RS_RS1_SIZE, &value1_ShaperId);
	/* Check Rate Shaper 2 Enable  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_EN2_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_EN2_SHIFT, VR9_PQM_RS_EN2_SIZE, &value2_RS);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_RS2_OFFSET + (nRateShaperId * 2),	\
		VR9_PQM_RS_RS2_SHIFT, VR9_PQM_RS_RS2_SIZE, &value2_ShaperId);
	if (( value1_RS == 1) && ( value1_ShaperId == nRateShaperId))
		return IFX_SUCCESS;
	else if (( value2_RS == 1) && ( value2_ShaperId == nRateShaperId))
		return IFX_SUCCESS;
	else if ( value1_RS == 0) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_RS1_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_RS1_SHIFT, VR9_PQM_RS_RS1_SIZE, nRateShaperId);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_EN1_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_EN1_SHIFT, VR9_PQM_RS_EN1_SIZE, 0x1);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CTRL_RSEN_OFFSET + (nRateShaperId * 0x5),	\
			VR9_RS_CTRL_RSEN_SHIFT, VR9_RS_CTRL_RSEN_SIZE, 0x1);
	} else if ( value2_RS == 0) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_RS2_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_RS2_SHIFT, VR9_PQM_RS_RS2_SIZE, nRateShaperId);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_EN2_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_EN2_SHIFT, VR9_PQM_RS_EN2_SIZE, 0x1);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CTRL_RSEN_OFFSET + (nRateShaperId * 0x5),	\
			VR9_RS_CTRL_RSEN_SHIFT, VR9_RS_CTRL_RSEN_SIZE, 0x1);
	} else {
		return IFX_ERROR;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ShaperQueueDeassign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperQueue_t *pPar)
{
	IFX_uint8_t nRateShaperId = pPar->nRateShaperId;
	IFX_uint8_t nQueueId = pPar->nQueueId;
	IFX_uint32_t value1, value2;
	IFX_uint32_t value3, value4;
	
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value1);
	if ( nQueueId >= value1 )
		return IFX_ERROR;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_3_SHAPERS_OFFSET,	\
		VR9_ETHSW_CAP_3_SHAPERS_SHIFT, VR9_ETHSW_CAP_3_SHAPERS_SIZE, &value1);
	if ( nRateShaperId >= value1)
		return IFX_ERROR;
	/* Rate Shaper 1 Read  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_EN1_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_EN1_SHIFT, VR9_PQM_RS_EN1_SIZE, &value1);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_RS1_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_RS1_SHIFT, VR9_PQM_RS_RS1_SIZE, &value2);
	/* Rate Shaper 2 Read  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_EN2_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_EN2_SHIFT, VR9_PQM_RS_EN2_SIZE, &value3);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_RS2_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_RS2_SHIFT, VR9_PQM_RS_RS2_SIZE, &value4);
	if ((value1 == 1) && (value2 == nRateShaperId)) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_EN1_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_EN1_SHIFT, VR9_PQM_RS_EN1_SIZE, 0);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_RS1_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_RS1_SHIFT, VR9_PQM_RS_RS1_SIZE, 0);
		return IFX_SUCCESS;
	} else if ((value3 == 1) && (value4 == nRateShaperId)) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_EN2_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_EN2_SHIFT, VR9_PQM_RS_EN2_SIZE, 0);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PQM_RS_RS2_OFFSET + (nQueueId * 2),	\
			VR9_PQM_RS_RS2_SHIFT, VR9_PQM_RS_RS2_SIZE, 0);
		return IFX_SUCCESS;
	} else {
		return IFX_ERROR;
	}
	if ((value1 == 0) && (value3 == 0)) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_RS_CTRL_RSEN_OFFSET + (nRateShaperId * 0x5),	\
			VR9_RS_CTRL_RSEN_SHIFT, VR9_RS_CTRL_RSEN_SIZE, 0);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_ShaperQueueGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperQueueGet_t *pPar)
{
	IFX_uint32_t value1_RS,value2_RS, value;
	IFX_uint8_t nQueueId = pPar->nQueueId;
	
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value1_RS);
	if ( nQueueId > value1_RS )
		return IFX_ERROR;
	pPar->bAssigned = IFX_FALSE;
	pPar->nRateShaperId = 0;
	/* Check Rate Shaper 1 Enable  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_EN1_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_EN1_SHIFT, VR9_PQM_RS_EN1_SIZE, &value1_RS);
	if (value1_RS == 1 ) {
		pPar->bAssigned = IFX_TRUE;
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_RS1_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_RS1_SHIFT, VR9_PQM_RS_RS1_SIZE, &value);
		pPar->nRateShaperId = value;
	}
	/* Check Rate Shaper 2 Enable  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_EN2_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_EN2_SHIFT, VR9_PQM_RS_EN2_SIZE, &value2_RS);
	if (value1_RS == 2 ) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PQM_RS_RS2_OFFSET + (nQueueId * 2),	\
		VR9_PQM_RS_RS2_SHIFT, VR9_PQM_RS_RS2_SIZE, &value);
		pPar->nRateShaperId = value;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_StormCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_stormCfg_t *pPar)
{
	IFX_uint32_t SCONBC, SCONMC, SCONUC, MeterID;

	if ((pPar->bBroadcast == IFX_FALSE) && (pPar->bMulticast == IFX_FALSE)	\
			&& (pPar->bUnknownUnicast == IFX_FALSE)) {
		/*  Storm Control Mode  */
      	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONMOD_OFFSET,	\
      		VR9_PCE_GCTRL_0_SCONMOD_SHIFT, VR9_PCE_GCTRL_0_SCONMOD_SIZE, 0);
		/* Meter instances used for broadcast traffic  */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONBC_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONBC_SHIFT, VR9_PCE_GCTRL_0_SCONBC_SIZE, 0);
		/* Meter instances used for multicast traffic  */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONMC_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONMC_SHIFT, VR9_PCE_GCTRL_0_SCONMC_SIZE, 0);
		/* Meter instances used for unknown unicast traffic  */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONUC_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONUC_SHIFT, VR9_PCE_GCTRL_0_SCONUC_SIZE, 0);
	}
	/*  Meter ID */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONMET_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONMET_SHIFT, VR9_PCE_GCTRL_0_SCONMET_SIZE, &MeterID);
	/* Meter instances used for broadcast traffic  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONBC_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONBC_SHIFT, VR9_PCE_GCTRL_0_SCONBC_SIZE, &SCONBC);
	/* Meter instances used for multicast traffic  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONMC_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONMC_SHIFT, VR9_PCE_GCTRL_0_SCONMC_SIZE, &SCONMC);
	/* Meter instances used for unknown unicast traffic  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONUC_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONUC_SHIFT, VR9_PCE_GCTRL_0_SCONUC_SIZE, &SCONUC);

	if ((SCONBC == 1) || (SCONMC == 1) ||(SCONMC == 1)) {
		if (pPar->nMeterId == (MeterID + 1)) {
			/*  Storm Control Mode  */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONMOD_OFFSET,	\
				VR9_PCE_GCTRL_0_SCONMOD_SHIFT, VR9_PCE_GCTRL_0_SCONMOD_SIZE, 3);
		} else if (pPar->nMeterId != MeterID)
			return IFX_ERROR;
	} else {
		/*  Storm Control Mode  */
      	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONMOD_OFFSET,	\
      		VR9_PCE_GCTRL_0_SCONMOD_SHIFT, VR9_PCE_GCTRL_0_SCONMOD_SIZE, 1);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONMET_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONMET_SHIFT, VR9_PCE_GCTRL_0_SCONMET_SIZE, pPar->nMeterId);
	}

	/* Meter instances used for broadcast traffic  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONBC_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONBC_SHIFT, VR9_PCE_GCTRL_0_SCONBC_SIZE, pPar->bBroadcast);
	/* Meter instances used for multicast traffic  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONMC_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONMC_SHIFT, VR9_PCE_GCTRL_0_SCONMC_SIZE, pPar->bMulticast);
	/* Meter instances used for unknown unicast traffic  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_SCONUC_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONUC_SHIFT, VR9_PCE_GCTRL_0_SCONUC_SIZE, pPar->bUnknownUnicast);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_StormCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_stormCfg_t *pPar)
{
	IFX_uint32_t value;

	/*  Storm Control Mode  */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONMOD_OFFSET,	\
		VR9_PCE_GCTRL_0_SCONMOD_SHIFT, VR9_PCE_GCTRL_0_SCONMOD_SIZE, &value);

	if (value == 0) {
		pPar->nMeterId = IFX_FALSE;
		pPar->bBroadcast = IFX_FALSE;
		pPar->bMulticast = IFX_FALSE;
		pPar->bUnknownUnicast =  IFX_FALSE;
	}  else {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONMET_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONMET_SHIFT, VR9_PCE_GCTRL_0_SCONMET_SIZE, &value);
		pPar->nMeterId = value;
		/* Meter instances used for broadcast traffic  */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONBC_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONBC_SHIFT, VR9_PCE_GCTRL_0_SCONBC_SIZE, &value );
		pPar->bBroadcast = value;
		/* Meter instances used for multicast traffic  */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONMC_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONMC_SHIFT, VR9_PCE_GCTRL_0_SCONMC_SIZE, &value );
		pPar->bMulticast = value;
		/* Meter instances used for unknown unicast traffic  */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_SCONUC_OFFSET,	\
			VR9_PCE_GCTRL_0_SCONUC_SHIFT, VR9_PCE_GCTRL_0_SCONUC_SIZE, &value);
		pPar->bUnknownUnicast = value;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_WredCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_WRED_Cfg_t *pPar)
{
	/* Description: 'Drop Probability Profile' */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_QUEUE_GCTRL_DPROB_OFFSET,	\
		VR9_BM_QUEUE_GCTRL_DPROB_SHIFT, VR9_BM_QUEUE_GCTRL_DPROB_SIZE, &pPar->eProfile);
	/* WRED Red Threshold - Minimum */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_WRED_RTH_0_MINTH_OFFSET,	\
		VR9_BM_WRED_RTH_0_MINTH_SHIFT, VR9_BM_WRED_RTH_0_MINTH_SIZE, &pPar->nRed_Min);
	/* WRED Red Threshold - Maximum */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_WRED_RTH_1_MAXTH_OFFSET,	\
		VR9_BM_WRED_RTH_1_MAXTH_SHIFT, VR9_BM_WRED_RTH_1_MAXTH_SIZE, &pPar->nRed_Max);
	/* WRED Yellow Threshold - Minimum */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_WRED_YTH_0_MINTH_OFFSET,	\
		VR9_BM_WRED_YTH_0_MINTH_SHIFT, VR9_BM_WRED_YTH_0_MINTH_SIZE, &pPar->nYellow_Min);
	/* WRED Yellow Threshold - Maximum */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_WRED_YTH_1_MAXTH_OFFSET,	\
		VR9_BM_WRED_YTH_1_MAXTH_SHIFT, VR9_BM_WRED_YTH_1_MAXTH_SIZE, &pPar->nYellow_Max);
	/* WRED Green Threshold - Minimum */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_WRED_GTH_0_MINTH_OFFSET,	\
		VR9_BM_WRED_GTH_0_MINTH_SHIFT, VR9_BM_WRED_GTH_0_MINTH_SIZE, &pPar->nGreen_Min);
	/* WRED Green Threshold - Maximum */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_WRED_GTH_1_MAXTH_OFFSET,	\
		VR9_BM_WRED_GTH_1_MAXTH_SHIFT, VR9_BM_WRED_GTH_1_MAXTH_SIZE, &pPar->nGreen_Max);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_WredCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_WRED_Cfg_t *pPar)
{
	/* Set the global threshold */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_QUEUE_GCTRL_GL_MOD_OFFSET,	\
		VR9_BM_QUEUE_GCTRL_GL_MOD_SHIFT,VR9_BM_QUEUE_GCTRL_GL_MOD_SIZE, 0x1);
	/* Description: 'Drop Probability Profile' */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_QUEUE_GCTRL_DPROB_OFFSET,	\
		VR9_BM_QUEUE_GCTRL_DPROB_SHIFT, VR9_BM_QUEUE_GCTRL_DPROB_SIZE, pPar->eProfile);
	/* WRED Red Threshold - Minimum */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_WRED_RTH_0_MINTH_OFFSET,	\
		VR9_BM_WRED_RTH_0_MINTH_SHIFT, VR9_BM_WRED_RTH_0_MINTH_SIZE, pPar->nRed_Min);
	/* WRED Red Threshold - Maximum */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_WRED_RTH_1_MAXTH_OFFSET,	\
		VR9_BM_WRED_RTH_1_MAXTH_SHIFT, VR9_BM_WRED_RTH_1_MAXTH_SIZE, pPar->nRed_Max);
	/* WRED Yellow Threshold - Minimum */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_WRED_YTH_0_MINTH_OFFSET,	\
		VR9_BM_WRED_YTH_0_MINTH_SHIFT, VR9_BM_WRED_YTH_0_MINTH_SIZE, pPar->nYellow_Min);
	/* WRED Yellow Threshold - Maximum */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_WRED_YTH_1_MAXTH_OFFSET,	\
		VR9_BM_WRED_YTH_1_MAXTH_SHIFT, VR9_BM_WRED_YTH_1_MAXTH_SIZE, pPar->nYellow_Max);
	/* WRED Green Threshold - Minimum */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_WRED_GTH_0_MINTH_OFFSET,	\
		VR9_BM_WRED_GTH_0_MINTH_SHIFT, VR9_BM_WRED_GTH_0_MINTH_SIZE, pPar->nGreen_Min);
	/* WRED Green Threshold - Maximum */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_WRED_GTH_1_MAXTH_OFFSET,	\
		VR9_BM_WRED_GTH_1_MAXTH_SHIFT, VR9_BM_WRED_GTH_1_MAXTH_SIZE, pPar->nGreen_Max);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_WredQueueCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_WRED_QueueCfg_t *pPar)
{
	IFX_uint32_t nQueueId = pPar->nQueueId;
	IFX_uint32_t  value, addr;
	IFX_uint32_t  data0, data1;
	IFX_uint8_t ColorCode;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value);
	
	if ( nQueueId >= value )
		return IFX_ERROR;
	/* For different color 0(not drop) 1(Green) 2(Yellow) 3(Red) */
	for (ColorCode = 0; ColorCode < 4; ColorCode++) {
		/* Set the BM RAM ADDR  */
		addr = ((nQueueId << 3 ) | ColorCode);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
			VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, addr);
		/* Specify the PQMCTXT = 0x9 */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
			VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0x9);
		value = 1;
		/* Active */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
		while (value == 1) {
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
				VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
			value = value & 0x8000;
		}
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
			VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, &data0);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_1_VAL1_OFFSET,	\
			VR9_BM_RAM_VAL_1_VAL1_SHIFT, VR9_BM_RAM_VAL_1_VAL1_SIZE, &data1);
		switch (ColorCode) {
			case 3:
				pPar->nRed_Max = data1;
				pPar->nRed_Min = data0;
			break;
			case 2:
				pPar->nYellow_Max = data1;
				pPar->nYellow_Min = data0;
			break;
			case 1:
				pPar->nGreen_Max = data1;
				pPar->nGreen_Min = data0;
			break;
			case 0:
#if 0
				pPar->nRed_Max = 0;
				pPar->nRed_Min = 0;
				pPar->nYellow_Max = 0;
				pPar->nYellow_Min = 0;
				pPar->nGreen_Max = 0;
				pPar->nGreen_Min = 0;
#endif
			break;
		}
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_QoS_WredQueueCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_WRED_QueueCfg_t *pPar)
{
	IFX_uint32_t nQueueId = pPar->nQueueId;
	IFX_uint32_t  value, addr;
	IFX_uint32_t  data0 = 0, data1 = 0;
	IFX_uint8_t ColorCode;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
		VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value);
	
	if ( nQueueId >= value )
		return IFX_ERROR;
	/* For different color 0(not drop) 1(Green) 2(Yellow) 3(Red) */
	for (ColorCode = 0; ColorCode < 4; ColorCode++) {
		/* Set the BM RAM ADDR  */
		addr = (((nQueueId << 3 ) & 0xF8) | ColorCode);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
			VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, addr);
		/* Specify the PQMCTXT = 0x9 */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
			VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0x9);
		/* Table Access Operation Mode Write */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_OPMOD_OFFSET,	\
			VR9_BM_RAM_CTRL_OPMOD_SHIFT, VR9_BM_RAM_CTRL_OPMOD_SIZE, 0x1);

		switch (ColorCode) {
			case 3:
				data1 = pPar->nRed_Max;
				data0 = pPar->nRed_Min;
				break;
			case 2:
				data1 = pPar->nYellow_Max;
				data0 = pPar->nYellow_Min;
				break;
			case 1:
				data1 = pPar->nGreen_Max;
				data0 = pPar->nGreen_Min;
				break;
			case 0:
				data0 = 0;
				data1 = 0;
				break;
		}
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
			VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, data0);

		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_VAL_1_VAL1_OFFSET,	\
			VR9_BM_RAM_VAL_1_VAL1_SHIFT, VR9_BM_RAM_VAL_1_VAL1_SIZE, data1);
		value = 1;
		/* Active */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
		while (value == 1) {
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
				VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
		}
		/* Specify the PQMCTXT = 0x9 */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
			VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, 0);
		/* Table Access Operation Mode Write */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_OPMOD_OFFSET,	\
			VR9_BM_RAM_CTRL_OPMOD_SHIFT, VR9_BM_RAM_CTRL_OPMOD_SIZE, 0);
		/* Active */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, 0);
	}
	/* Set the local threshold */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_QUEUE_GCTRL_GL_MOD_OFFSET,	\
		VR9_BM_QUEUE_GCTRL_GL_MOD_SHIFT,VR9_BM_QUEUE_GCTRL_GL_MOD_SIZE, 0);
   return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastRouterPortAdd(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouter_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value;
	IFX_uint8_t portIdx = pPar->nPortId;

	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;

	/* Read the Default Router Port Map - DRPM */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_DRPM_DRPM_OFFSET,	\
		VR9_PCE_IGMP_DRPM_DRPM_SHIFT, VR9_PCE_IGMP_DRPM_DRPM_SIZE, &value);

	if(((value >> portIdx) & 0x1 ) == 1) {
		IFXOS_PRINT_INT_RAW("Error: the prot was already in the member\n");
	} else {
		value = (value | (1 << portIdx));
		/* Write the Default Router Port Map - DRPM  */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_DRPM_DRPM_OFFSET,	\
			VR9_PCE_IGMP_DRPM_DRPM_SHIFT, VR9_PCE_IGMP_DRPM_DRPM_SIZE, value);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastRouterPortRead(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouterRead_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value_1, value_2;

	if (pPar->bInitial == IFX_TRUE) {
		/* Read the Default Router Port Map - DRPM*/
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_DRPM_DRPM_OFFSET,	\
			VR9_PCE_IGMP_DRPM_DRPM_SHIFT, VR9_PCE_IGMP_DRPM_DRPM_SIZE, &value_1);
		/* Read the Default Router Port Map - IGPM */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_STAT_IGPM_OFFSET,	\
			VR9_PCE_IGMP_STAT_IGPM_SHIFT, VR9_PCE_IGMP_STAT_IGPM_SIZE, &value_2);
		pEthSWDev->IGMP_Flags.eRouterPort = (value_1 | value_2);
		pPar->bInitial = IFX_FALSE;
		MULTICAST_ROUTERPORT_COUNTER = 0;
	}
	if (pPar->bLast == IFX_FALSE) {
		/* Need to clarify the different between DRPM & IGPM */
		while (((pEthSWDev->IGMP_Flags.eRouterPort >> MULTICAST_ROUTERPORT_COUNTER) & 0x1) == 0) {
			MULTICAST_ROUTERPORT_COUNTER += 1;
			if (MULTICAST_ROUTERPORT_COUNTER > 12) {
				pPar->bLast = IFX_TRUE;
				return IFX_SUCCESS;
			}
		}
		pPar->nPortId = MULTICAST_ROUTERPORT_COUNTER;
		if (MULTICAST_ROUTERPORT_COUNTER < 13)
			MULTICAST_ROUTERPORT_COUNTER += 1;
		else
			pPar->bLast = IFX_TRUE;
	}
    return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastRouterPortRemove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouter_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value_1, value_2;
	IFX_uint8_t portIdx = pPar->nPortId;

	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	/* Read the Default Router Port Map - DRPM */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_DRPM_DRPM_OFFSET,	\
		VR9_PCE_IGMP_DRPM_DRPM_SHIFT, VR9_PCE_IGMP_DRPM_DRPM_SIZE, &value_1);
	/* Read the Default Router Port Map - IGPM */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_STAT_IGPM_OFFSET,	\
		VR9_PCE_IGMP_STAT_IGPM_SHIFT, VR9_PCE_IGMP_STAT_IGPM_SIZE, &value_2);
	if(((value_1 >> portIdx) & 0x1 ) == 0) {
	IFXOS_PRINT_INT_RAW("Error: the prot was not in the member\n");
	} else {
	value_1 = (value_1 & ~(1 << portIdx));
	/* Write the Default Router Port Map - DRPM*/
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_DRPM_DRPM_OFFSET,	\
		VR9_PCE_IGMP_DRPM_DRPM_SHIFT, VR9_PCE_IGMP_DRPM_DRPM_SIZE, value_1);
	}
 	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastSnoopCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_multicastSnoopCfg_t *pPar)
{
	IFX_uint32_t value=0;
	IFX_uint32_t data_1, data_2;
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;

	/* eIGMP_Mode read */
	pPar->eIGMP_Mode = pEthSWDevHandle->IGMP_Flags.eIGMP_Mode;
	// Get bIGMPv3
    pPar->bIGMPv3 = pEthSWDevHandle->IGMP_Flags.bIGMPv3;
    // Get bCrossVLAN
	pPar->bCrossVLAN = pEthSWDevHandle->IGMP_Flags.bCrossVLAN;
    // Get eForwardPort
    pPar->eForwardPort = pEthSWDevHandle->IGMP_Flags.eForwardPort;
    pPar->nForwardPortId = pEthSWDevHandle->IGMP_Flags.nForwardPortId;
    // Set nClassOfService
    pPar->nClassOfService = pEthSWDevHandle->IGMP_Flags.nClassOfService;
    // Get nRobust
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_CTRL_ROB_OFFSET,	\
		VR9_PCE_IGMP_CTRL_ROB_SHIFT, VR9_PCE_IGMP_CTRL_ROB_SIZE, &value );
	pPar->nRobust = value;
	// Get nQueryInterval
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_CTRL_DMRT_OFFSET,	\
		VR9_PCE_IGMP_CTRL_DMRT_SHIFT, VR9_PCE_IGMP_CTRL_DMRT_SIZE, &value );
	pPar->nQueryInterval = value;
	// Get eSuppressionAggregation
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_CTRL_REPSUP_OFFSET,	\
		VR9_PCE_IGMP_CTRL_REPSUP_SHIFT, VR9_PCE_IGMP_CTRL_REPSUP_SIZE, &data_1);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_CTRL_JASUP_OFFSET,	\
		VR9_PCE_IGMP_CTRL_JASUP_SHIFT, VR9_PCE_IGMP_CTRL_JASUP_SIZE, &data_2 );

	if ( data_1 == 0 && data_2 == 0 ) {
		pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_TRANSPARENT;
    } else if (data_1 == 1 && data_2 == 0) {
		pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_REPORT;
    } else if (data_1 == 1 && data_2 == 1) {
		pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_REPORT_JOIN;
	} else {
		pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_TRANSPARENT;
    }
    // Get bFastLeave
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_CTRL_FLEAVE_OFFSET,
                        VR9_PCE_IGMP_CTRL_FLEAVE_SHIFT, VR9_PCE_IGMP_CTRL_FLEAVE_SIZE, &value);
	if (value == 1)
		pPar->bFastLeave = 1;
	else
		pPar->bFastLeave = 0;
	// Get bLearningRouter
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_CTRL_SRPEN_OFFSET,	\
		VR9_PCE_IGMP_CTRL_SRPEN_SHIFT, VR9_PCE_IGMP_CTRL_SRPEN_SIZE, &value);
	pPar->bLearningRouter = value;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastSnoopCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_multicastSnoopCfg_t *pPar)
{
	IFX_uint32_t data_1 = 0, data_2 = 0, portID = 6;
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_PCE_rule_t PCE_rule;
	IFX_uint32_t i, table_index_hw = 40, table_index_sw = 50;
	
	/* Choose IGMP Mode */
	switch (pPar->eIGMP_Mode) {
		case IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED:
			/* Snooping of Router Port Disable */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_SRPEN_OFFSET,	\
				VR9_PCE_IGMP_CTRL_SRPEN_SHIFT, VR9_PCE_IGMP_CTRL_SRPEN_SIZE, 0);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_IGMP_OFFSET,	\
				VR9_PCE_GCTRL_0_IGMP_SHIFT, VR9_PCE_GCTRL_0_IGMP_SIZE, 0);
			for (i = 0; i <= portID ; i++) {
				ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_MCST_OFFSET + (0xA * i),	\
					VR9_PCE_PCTRL_0_MCST_SHIFT, VR9_PCE_PCTRL_0_MCST_SIZE, 0);
			}
			break;
		case IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING:
			/* Snooping of Router Port Enable */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_IGMP_OFFSET,	\
				VR9_PCE_GCTRL_0_IGMP_SHIFT, VR9_PCE_GCTRL_0_IGMP_SIZE, 0);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_SRPEN_OFFSET,	\
				VR9_PCE_IGMP_CTRL_SRPEN_SHIFT, VR9_PCE_IGMP_CTRL_SRPEN_SIZE, 1);
			for (i = 0; i <= portID ; i++) {
				ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_MCST_OFFSET + (0xA * i),	\
					VR9_PCE_PCTRL_0_MCST_SHIFT, VR9_PCE_PCTRL_0_MCST_SIZE, 1);
			}
			break;
		case IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD:
			/* Snooping of Router Port Forward */
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_SRPEN_OFFSET,	\
				VR9_PCE_IGMP_CTRL_SRPEN_SHIFT, VR9_PCE_IGMP_CTRL_SRPEN_SIZE, 0);
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_IGMP_OFFSET,	\
				VR9_PCE_GCTRL_0_IGMP_SHIFT, VR9_PCE_GCTRL_0_IGMP_SIZE, 1);
			for (i = 0; i <= portID ; i++) {
				ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PCTRL_0_MCST_OFFSET + (0xA * i),	\
					VR9_PCE_PCTRL_0_MCST_SHIFT, VR9_PCE_PCTRL_0_MCST_SIZE, 1);
			}
			break;
		default:
			IFXOS_PRINT_INT_RAW("This Mode doesn't exists\n");
			return IFX_ERROR;
	} /* -----  end switch  ----- */
	/* Set the Flag for eIGMP_Mode flag*/
	pEthSWDevHandle->IGMP_Flags.eIGMP_Mode = pPar->eIGMP_Mode;
	/* Set bIGMPv3 flag*/
	pEthSWDevHandle->IGMP_Flags.bIGMPv3 =  pPar->bIGMPv3;
	/* Set bCrossVLAN flag*/
	pEthSWDevHandle->IGMP_Flags.bCrossVLAN = pPar->bCrossVLAN;
	/* Set eForwardPort flag */
	pEthSWDevHandle->IGMP_Flags.eForwardPort = pPar->eForwardPort;
	/* Set nForwardPortId */
	if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_CPU) {
		pEthSWDevHandle->IGMP_Flags.nForwardPortId = (1<< pEthSWDevHandle->nCPU_Port);
	} else {
		pEthSWDevHandle->IGMP_Flags.nForwardPortId = pPar->nForwardPortId;
	} 
	pEthSWDevHandle->IGMP_Flags.nClassOfService = pPar->nClassOfService;
/*** Prepare and write into PCE Traffic Table ***
* We write the rule from the end of the Table
* For Hardware snooping with 8 rule and software snooping with another 5 rule.*/
	/* If IGMP mode set to AutoLearning then the following Rule have to add it */
	if (pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		for (i = table_index_hw; i <= (table_index_hw + 7); i++) {
			memset(&PCE_rule, 0 , sizeof(IFX_FLOW_PCE_rule_t));
			PCE_rule.pattern.nIndex = i;
			PCE_rule.pattern.bEnable = IFX_TRUE;
			PCE_rule.pattern.bAppDataMSB_Enable = IFX_TRUE;
			if ((i == table_index_hw + 0) || (i == table_index_hw + 1) || (i == table_index_hw + 2))
				PCE_rule.pattern.nAppDataMSB = 0x1100;
			else if (i == table_index_hw + 3)
				PCE_rule.pattern.nAppDataMSB = 0x1200;
			else if (i == table_index_hw + 4)
				PCE_rule.pattern.nAppDataMSB = 0x1600;
			else if (i == table_index_hw + 5)
				PCE_rule.pattern.nAppDataMSB = 0x1700;
			else if (i == table_index_hw + 6)
				PCE_rule.pattern.nAppDataMSB = 0x3100;
			else if (i == table_index_hw + 7)
				PCE_rule.pattern.nAppDataMSB = 0x3000;

			PCE_rule.pattern.bAppMaskRangeMSB_Select = 1;
			PCE_rule.pattern.nAppMaskRangeMSB = 0x3;
			if ((i == table_index_hw + 0)|| (i == table_index_hw + 1) || (i == table_index_hw + 6)	\
				|| (i == table_index_hw + 7))
				PCE_rule.pattern.eDstIP_Select = IFX_TRUE;
			if ((i == table_index_hw + 0) || (i == table_index_hw + 1))
				PCE_rule.pattern.nDstIP.nIPv4 = 0xE0000001;
			else if (i == table_index_hw + 6)
				PCE_rule.pattern.nDstIP.nIPv4 = 0xE0000002;
			else if (i == table_index_hw + 7)
				PCE_rule.pattern.nDstIP.nIPv4 = 0xE00000A6;
			PCE_rule.pattern.nDstIP_Mask = 0xFF00;
			if (i == table_index_hw + 1)
				PCE_rule.pattern.eSrcIP_Select = IFX_TRUE;
			else
				PCE_rule.pattern.eSrcIP_Select = 0;
			if (i == table_index_hw + 1)
				PCE_rule.pattern.nSrcIP_Mask = 0xFF00;
			else
				PCE_rule.pattern.nSrcIP_Mask = 0xFFFF;
			PCE_rule.pattern.bEtherTypeEnable = IFX_TRUE;
			PCE_rule.pattern.nEtherType = 0x800;
			PCE_rule.pattern.bProtocolEnable = IFX_TRUE;
			PCE_rule.pattern.nProtocol = 0x2;
			if (pEthSWDevHandle->IGMP_Flags.nClassOfService == 0 ) {
				PCE_rule.action.eTrafficClassAction = 0;
				PCE_rule.action.nTrafficClassAlternate = 0;
			} else {
				PCE_rule.action.eTrafficClassAction = 1;
				PCE_rule.action.nTrafficClassAlternate = pEthSWDevHandle->IGMP_Flags.nClassOfService;
			}
			if (i == table_index_hw + 0)
				PCE_rule.action.eSnoopingTypeAction =IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY;
			else if (i == table_index_hw + 1)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER;
			else if (i == table_index_hw + 2)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP;
			else if (i == table_index_hw + 3)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REPORT;
			else if (i == table_index_hw + 4)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REPORT;
			else if (i == table_index_hw + 5)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_LEAVE;
			else if (i == table_index_hw + 6)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_AD;
			else if (i == table_index_hw + 7)
				PCE_rule.action.eSnoopingTypeAction = IFX_FLOW_PCE_ACTION_IGMP_SNOOP_AD;
/*			if ((i == table_index_hw + 3) || (i == table_index_hw + 4) || (i == table_index_hw + 5)) */
			PCE_rule.action.ePortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_MULTICAST_ROUTER;
			if (pPar->bCrossVLAN)
				PCE_rule.action.eVLAN_CrossAction = IFX_FLOW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				PCE_rule.action.eVLAN_CrossAction = IFX_FLOW_PCE_ACTION_CROSS_VLAN_DISABLE;
			/* We prepare everything and write into PCE Table */
			if (0 != ifx_pce_rule_write(&pEthSWDevHandle->PCE_Handler,&PCE_rule)) {
				IFXOS_PRINT_INT_RAW("ERROR: : %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
				return IFX_ERROR;
			}
		}
	}
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
	IFX_FLOW_PROTOCOL_Table_Print();
	IFX_FLOW_IP_DASA_LSB_Table_Print();
	IFX_FLOW_APP_DATA_MSB_LSB_Table_Print();
	// IFX_FLOW_PCE_TRAFFIC_Table_Print();
#endif
	/* If IGMP mode set to forwarding then the following Rule have to add it */
	if (pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD) {
		for (i = table_index_sw; i < (table_index_sw + 6); i++) {
			memset(&PCE_rule, 0 , sizeof(IFX_FLOW_PCE_rule_t));
			PCE_rule.pattern.nIndex = i;
			PCE_rule.pattern.bEnable = IFX_TRUE;
			PCE_rule.pattern.bAppDataMSB_Enable = 1;

			if (i == table_index_sw + 0)
				PCE_rule.pattern.nAppDataMSB = 0x1200;
			else if (i == table_index_sw + 1)
				PCE_rule.pattern.nAppDataMSB = 0x1300;
			else if (i == table_index_sw + 2)
				PCE_rule.pattern.nAppDataMSB = 0x1600;
			else if (i == table_index_sw + 3)
				PCE_rule.pattern.nAppDataMSB = 0x1700;
			else if (i == table_index_sw + 4)
				PCE_rule.pattern.nAppDataMSB = 0x2200;
			else if (i == table_index_sw + 5)
				PCE_rule.pattern.nAppDataMSB = 0x1100;

			PCE_rule.pattern.bAppMaskRangeMSB_Select = 1;
			PCE_rule.pattern.nAppMaskRangeMSB = 0x3;
			PCE_rule.pattern.bEtherTypeEnable = IFX_TRUE;
			PCE_rule.pattern.nEtherType = 0x800;
			PCE_rule.pattern.bProtocolEnable = IFX_TRUE;
			PCE_rule.pattern.nProtocol = 0x2;

			if (pEthSWDevHandle->IGMP_Flags.nClassOfService != 0 ) {
				PCE_rule.action.eTrafficClassAction = 1;
				PCE_rule.action.nTrafficClassAlternate = pEthSWDevHandle->IGMP_Flags.nClassOfService;
			}
			/*  Set eForwardPort */
			if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_PORT ) {
				PCE_rule.action.nForwardPortMap = (1 << pPar->nForwardPortId);
				PCE_rule.action.ePortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			} else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_CPU ) {
				PCE_rule.action.nForwardPortMap = (1<< pEthSWDevHandle->nCPU_Port);
				PCE_rule.action.ePortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			} else if (pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DEFAULT ) {
				PCE_rule.action.ePortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_REGULAR;
			} else {
				PCE_rule.action.ePortMapAction = IFX_ETHSW_PORT_FORWARD_DISCARD;
			}
			if (pPar->bCrossVLAN)
				PCE_rule.action.eVLAN_CrossAction = IFX_FLOW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				PCE_rule.action.eVLAN_CrossAction = IFX_FLOW_PCE_ACTION_CROSS_VLAN_DISABLE;
			/* We prepare everything and write into PCE Table */
			if (0 != ifx_pce_rule_write(&pEthSWDevHandle->PCE_Handler,&PCE_rule))
				return IFX_ERROR;
		}	
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
		IFX_FLOW_PROTOCOL_Table_Print();
		IFX_FLOW_PCE_TRAFFIC_Table_Print();
#endif
	}
	if(pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED) {
		table_index_hw = 40;
		for (i = table_index_hw; i <= (table_index_hw + 7); i++) {
			PCE_rule.pattern.nIndex = i;
			PCE_rule.pattern.bEnable = IFX_FALSE;
			/* We prepare everything and write into PCE Table */
			if (0 != ifx_pce_pattern_delete(&pEthSWDevHandle->PCE_Handler, i))
				return IFX_ERROR;
		}

		table_index_hw = 50;
		for (i = table_index_hw; i < (table_index_hw + 6); i++) {
			PCE_rule.pattern.nIndex = i;
			PCE_rule.pattern.bEnable = IFX_FALSE;

			/* We prepare everything and write into PCE Table */
			if (0 != ifx_pce_pattern_delete(&pEthSWDevHandle->PCE_Handler, i))
				return IFX_ERROR;
		}
	}
	// Set nRobust
	if ( pPar->nRobust < 4) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_ROB_OFFSET,	\
			VR9_PCE_IGMP_CTRL_ROB_SHIFT, VR9_PCE_IGMP_CTRL_ROB_SIZE, pPar->nRobust );
	} else {
		IFXOS_PRINT_INT_RAW("The Robust time would only support 0..3\n");
		return IFX_ERROR;
	}
	// Set nQueryInterval
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_DMRTEN_OFFSET,	\
		VR9_PCE_IGMP_CTRL_DMRTEN_OFFSET, VR9_PCE_IGMP_CTRL_DMRTEN_OFFSET, 1);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_DMRT_OFFSET,	\
		VR9_PCE_IGMP_CTRL_DMRT_SHIFT, VR9_PCE_IGMP_CTRL_DMRT_SIZE, pPar->nQueryInterval );

	// Set eSuppressionAggregation
	if (pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		switch (pPar->eSuppressionAggregation) {
			case IFX_ETHSW_MULTICAST_REPORT_JOIN:
				data_2 = 1;
				data_1 = 1;
				break;
			case IFX_ETHSW_MULTICAST_REPORT:
				data_2 = 0;
				data_1 = 1;
				break;
			case IFX_ETHSW_MULTICAST_TRANSPARENT:
				data_2 = 0;
				data_1 = 0;
				break;
		}       /* -----  end switch  ----- */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_REPSUP_OFFSET,	\
			VR9_PCE_IGMP_CTRL_REPSUP_SHIFT, VR9_PCE_IGMP_CTRL_REPSUP_SIZE, data_1);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_JASUP_OFFSET,	\
			VR9_PCE_IGMP_CTRL_JASUP_SHIFT, VR9_PCE_IGMP_CTRL_JASUP_SIZE, data_2 );
	}
	// Set bFastLeave, supported only if SNOOPING MODE AUTOLEARNING
	if (pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_SRPEN_OFFSET,	\
			VR9_PCE_IGMP_CTRL_SRPEN_SHIFT, VR9_PCE_IGMP_CTRL_SRPEN_SIZE, pPar->bLearningRouter);
		if (pPar->bFastLeave == 1)
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_FLEAVE_OFFSET,	\
				VR9_PCE_IGMP_CTRL_FLEAVE_SHIFT, VR9_PCE_IGMP_CTRL_FLEAVE_SIZE, 1);
		else
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_FLEAVE_OFFSET,	\
				VR9_PCE_IGMP_CTRL_FLEAVE_SHIFT, VR9_PCE_IGMP_CTRL_FLEAVE_SIZE, 0);
	} else {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_CTRL_FLEAVE_OFFSET,	\
			VR9_PCE_IGMP_CTRL_FLEAVE_SHIFT, VR9_PCE_IGMP_CTRL_FLEAVE_SIZE, 0);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastTableEntryAdd(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_IGMP_t      *pIGMPTbHandle   = &pEthSWDevHandle->IGMP_Flags;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	
	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	memset(&pData, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		IFX_uint32_t index = 0, i, available= 0;
		if (pEthSWDevHandle->IGMP_Flags.bIGMPv3 == 1)
			return IFX_ERROR;
		/* Read Out all of the HW Table */
		for (i=0; i < IFX_FLOW_MULTICAST_HW_TBL_SIZE; i++) {
			pData.table = IFX_ETHSW_PCE_MULTICAST_HW_INDEX; //Multicast Table Address
			pData.table_index = i;
			ifx_ethsw_xwayflow_pce_table_read(pEthSWDevHandle, &pData);
			if (pData.valid) {
				if ( ( pData.key[0] == (pPar->uIP_Gda.nIPv4 & 0xFFFF) )	\
					&&  (pData.key[1] == ((pPar->uIP_Gda.nIPv4 >> 16 ) & 0xFFFF) ) ) {
					index = i;
					available = 1;
					break;
				}
			}
		}
		pData.table = IFX_ETHSW_PCE_MULTICAST_HW_INDEX; 
		if (available == 0 ) {
			index = IFX_FLOW_MULTICAST_HW_TBL_SIZE; /*Assume table is full */
			for (i=0; i < IFX_FLOW_MULTICAST_HW_TBL_SIZE; i++) {
				pData.table_index = i;
				ifx_ethsw_xwayflow_pce_table_read(pEthSWDevHandle, &pData);
				if (pData.valid == 0) {
					index = i;  /* Free index */
					break;
				}
			}
		}
		if ( index < IFX_FLOW_MULTICAST_HW_TBL_SIZE ) {
			pData.table			= IFX_ETHSW_PCE_MULTICAST_HW_INDEX; 
			pData.table_index	= index;
			pData.key[1]		= ((pPar->uIP_Gda.nIPv4 >> 16 ) & 0xFFFF);
			pData.key[0]		= (pPar->uIP_Gda.nIPv4 & 0xFFFF);
			pData.val[0]		|= (1 << portIdx);
			pData.val[4]		|= (1 << 14);
			pData.valid			= 1;
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
		/* Print out debug info first */
		IFXOS_PRINT_INT_RAW("|DEBUG: GroupIPv4 = %03d.%03d.%03d.%03d \n",	\
			(pData.key[1] >> 8) & 0xFF, (pData.key[1] & 0xFF),	\
			(pData.key[0] >> 8) & 0xFF, (pData.key[0] & 0xFF));
#endif
			ifx_ethsw_xwayflow_pce_table_write(pEthSWDevHandle, &pData);
		} else {
			IFXOS_PRINT_INT_RAW("Error: (IGMP HW Table is full) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
			return IFX_ERROR;
		}

	} else if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD) {
		/* Program the Multicast SW Table */
		ifx_multicast_sw_table_write(pEthSWDevHandle, pPar);
	} else {
		/* Disable All Multicast SW Table */
		IFXOS_PRINT_INT_RAW("Please Select the IGMP Mode through Multicast Snooping Configuration API\n");
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastTableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRead_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_IGMP_t      *pIGMPTbHandle   = &pEthSWDevHandle->IGMP_Flags;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	
	if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED) {
		IFXOS_PRINT_INT_RAW("Error: (IGMP snoop is not enabled) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		if (pPar->bInitial == IFX_TRUE) {
			pEthSWDevHandle->multi_hw_table_index = 0; /*Start from the index 0 */
			pPar->bInitial = IFX_FALSE;
		}
		if (pEthSWDevHandle->multi_hw_table_index >= IFX_FLOW_MULTICAST_HW_TBL_SIZE) {
			memset(pPar, 0, sizeof(IFX_ETHSW_multicastTableRead_t));
			pPar->bLast = IFX_TRUE;
			pEthSWDevHandle->multi_hw_table_index = 0;
			return IFX_SUCCESS;
		}
		pData.table = IFX_ETHSW_PCE_MULTICAST_HW_INDEX;
		do {
			pData.table_index = pEthSWDevHandle->multi_hw_table_index;
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);   
			pEthSWDevHandle->multi_hw_table_index++;
			if (pData.valid != 0)
				break;
		} while (pEthSWDevHandle->multi_hw_table_index < IFX_FLOW_MULTICAST_HW_TBL_SIZE);
		if (pData.valid != 0) {
			pPar->nPortId 		= pData.val[0];
			pPar->uIP_Gda.nIPv4 = ( (pData.key[1] << 16) | pData.key[0] );
			pPar->uIP_Gsa.nIPv4 = 0;
			pPar->eModeMember 	= IFX_ETHSW_IGMP_MEMBER_DONT_CARE;
			pPar->eIPVersion 	= IFX_ETHSW_IP_SELECT_IPV4;
			
			pPar->bInitial = IFX_FALSE;
			pPar->bLast = IFX_FALSE;
		} else {
			memset(pPar, 0, sizeof(IFX_ETHSW_multicastTableRead_t));
			pPar->bLast = IFX_TRUE;
			pEthSWDevHandle->multi_hw_table_index = 0;
		}
	}
	/*Snooping in Forward mode */
	if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD) {
		IFX_uint32_t Dest_index, Src_index;
		if (pPar->bInitial == IFX_TRUE) {
			pEthSWDevHandle->multi_sw_table_index = 0; /*Start from the index 0 */
			pPar->bInitial = IFX_FALSE;
		}
		if (pEthSWDevHandle->multi_sw_table_index >= IFX_FLOW_MULTICAST_SW_TBL_SIZE) {
			memset(pPar, 0, sizeof(IFX_ETHSW_multicastTableRead_t));
			pPar->bLast = IFX_TRUE;
			pEthSWDevHandle->multi_sw_table_index = 0;
			return IFX_SUCCESS;
		}
		pData.table = IFX_ETHSW_PCE_MULTICAST_SW_INDEX;
		do {
			pData.table_index = pEthSWDevHandle->multi_sw_table_index;
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData);   
			pEthSWDevHandle->multi_sw_table_index++;
			if (pData.valid != 0)
				break;
		}while (pEthSWDevHandle->multi_sw_table_index < IFX_FLOW_MULTICAST_SW_TBL_SIZE);
		if (pData.valid == 1) {
			IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData_IP_Table;
			pPar->nPortId 		= pData.val[0];
			Dest_index			= pData.key[0] & 0x3F;
			Src_index			= pData.key[1] & 0x3F;
//			pPar->eModeMember 	= IFX_ETHSW_IGMP_MEMBER_DONT_CARE;	
			pPar->eIPVersion 	= IFX_ETHSW_IP_SELECT_IPV4;
			memset(&pData_IP_Table, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
			pData_IP_Table.table = IFX_ETHSW_PCE_IP_DASA_LSB_INDEX;
			/* Search the DIP */
			pData_IP_Table.table_index = Dest_index;
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData_IP_Table);
			pPar->uIP_Gda.nIPv4 = (pData_IP_Table.key[1] << 16) | (pData_IP_Table.key[0] & 0xFFFF);
			memset(&pData_IP_Table, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
			pData_IP_Table.table = IFX_ETHSW_PCE_IP_DASA_LSB_INDEX;
			/* Search the SIP */
			pData_IP_Table.table_index = Src_index;
			ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pData_IP_Table);
			pPar->uIP_Gsa.nIPv4 = (pData_IP_Table.key[1] << 16) | (pData_IP_Table.key[0] & 0xFFFF);
//			if (pPar->uIP_Gsa.nIPv4 != 0 ) 
				pPar->eModeMember = pIGMPTbHandle->multicast_sw_table[pEthSWDevHandle->multi_sw_table_index-1].eModeMember;
			pPar->bInitial = IFX_FALSE;
			pPar->bLast = IFX_FALSE;
		} else {
			memset(pPar, 0, sizeof(IFX_ETHSW_multicastTableRead_t));
			pPar->bLast = IFX_TRUE;
			pEthSWDevHandle->multi_sw_table_index = 0;
		}
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MulticastTableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_IGMP_t      *pIGMPTbHandle   = &pEthSWDevHandle->IGMP_Flags;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pData;
	IFX_uint32_t i;
	IFX_boolean_t DeletFlag = IFX_FALSE;
	IFX_uint32_t port = 0;
	
	if (pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	memset(&pData, 0 , sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		if (pEthSWDevHandle->IGMP_Flags.bIGMPv3 == 1)
			return IFX_ERROR;
		/* Read Out all of the HW Table */
		for (i=0; i < IFX_FLOW_MULTICAST_HW_TBL_SIZE; i++) {
			pData.table = IFX_ETHSW_PCE_MULTICAST_HW_INDEX; //Multicast Table Address
			pData.table_index = i;
			ifx_ethsw_xwayflow_pce_table_read(pEthSWDevHandle, &pData);
			/* Fill into Structure */
			if (((pData.val[0] >> portIdx) & 0x1 )== 1) {
				if (pPar->uIP_Gda.nIPv4 == ((pData.key[1] << 16) | (pData.key[0] & 0xFFFF))) {
					port = (pData.val[0] & ( ~(1 << portIdx)));
					if (port == 0) {
						pData.val[0] = 0;
						pData.key[1] = 0;
						pData.val[4] = 0;
					} else {
						pData.val[0] &= ~(1 << portIdx);
					}
					DeletFlag = IFX_TRUE;
					ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pData);
				}
			}
		}
		if (DeletFlag == IFX_FALSE)
			IFXOS_PRINT_INT_RAW("The input did not found \n");
	} else if (pIGMPTbHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD) {
		/* Program the Multicast SW Table */
		ifx_multicast_sw_table_remove(pEthSWDevHandle, pPar);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CapGet(IFX_void_t *pDevCtx, IFX_ETHSW_cap_t *pPar)
{
	IFX_uint32_t value, data1, data2;

	if (pPar->nCapType >= IFX_ETHSW_CAP_TYPE_LAST)
		return IFX_ERROR;
	else
		strcpy(pPar->cDesc, CAP_Description[pPar->nCapType].Desci);

	/* As request, attached the code in the next version*/
	switch (pPar->nCapType) {
		case IFX_ETHSW_CAP_TYPE_PORT:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_PPORTS_OFFSET,	\
				VR9_ETHSW_CAP_1_PPORTS_SHIFT, VR9_ETHSW_CAP_1_PPORTS_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_VIRTUAL_PORT:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_VPORTS_OFFSET,	\
				VR9_ETHSW_CAP_1_VPORTS_SHIFT, VR9_ETHSW_CAP_1_VPORTS_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_BUFFER_SIZE:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_11_BSIZEL_OFFSET,	\
				VR9_ETHSW_CAP_11_BSIZEL_SHIFT, VR9_ETHSW_CAP_11_BSIZEL_SIZE, &data1);
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_12_BSIZEH_OFFSET,	\
				VR9_ETHSW_CAP_12_BSIZEH_SHIFT, VR9_ETHSW_CAP_12_BSIZEH_SIZE, &data2);
			pPar->nCap = (data2 << 16 | data1);
			break;
		case IFX_ETHSW_CAP_TYPE_SEGMENT_SIZE:
			/* This is Hard coded */
			pPar->nCap = VRX_PLATFORM_CAP_SEGNENT;
			break;
		case IFX_ETHSW_CAP_TYPE_PRIORITY_QUEUE:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_1_QUEUE_OFFSET,	\
				VR9_ETHSW_CAP_1_QUEUE_SHIFT, VR9_ETHSW_CAP_1_QUEUE_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_METER:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_3_METERS_OFFSET,	\
				VR9_ETHSW_CAP_3_METERS_SHIFT, VR9_ETHSW_CAP_3_METERS_SIZE, &value);
			pPar->nCap = value;
			break;
      case IFX_ETHSW_CAP_TYPE_RATE_SHAPER:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_3_SHAPERS_OFFSET,	\
				VR9_ETHSW_CAP_3_SHAPERS_SHIFT, VR9_ETHSW_CAP_3_SHAPERS_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_VLAN_GROUP:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_4_VLAN_OFFSET,	\
				VR9_ETHSW_CAP_4_VLAN_SHIFT, VR9_ETHSW_CAP_4_VLAN_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_FID:
			/* This is Hard coded */
			pPar->nCap = VRX_PLATFORM_CAP_FID;
			break;
		case IFX_ETHSW_CAP_TYPE_MAC_TABLE_SIZE:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_10_MACBT_OFFSET,	\
				VR9_ETHSW_CAP_10_MACBT_SHIFT, VR9_ETHSW_CAP_10_MACBT_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_MULTICAST_TABLE_SIZE:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_8_MCAST_OFFSET,
				VR9_ETHSW_CAP_8_MCAST_SHIFT, VR9_ETHSW_CAP_8_MCAST_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_PPPOE_SESSION:
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_ETHSW_CAP_4_PPPOE_OFFSET,	\
				VR9_ETHSW_CAP_4_PPPOE_SHIFT, VR9_ETHSW_CAP_4_PPPOE_SIZE, &value);
			pPar->nCap = value;
			break;
		case IFX_ETHSW_CAP_TYPE_LAST:
			pPar->nCap = 12;
			break;
		default:
			pPar->nCap = 0;
			break;
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_cfg_t *pPar)
{
   IFX_uint32_t value, data1, data2;

	/* Aging Counter Mantissa Value */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_AGE_1_MANT_OFFSET,
                                 VR9_PCE_AGE_1_MANT_SHIFT, VR9_PCE_AGE_1_MANT_SIZE, &data2);

   if ( data2 == AGETIMER_1_DAY)
      pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_1_DAY;
   else if (data2 == AGETIMER_1_HOUR)
      pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_1_HOUR;
   else if (data2 == AGETIMER_300_SEC)
      pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_300_SEC;
   else if (data2 == AGETIMER_10_SEC)
      pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_10_SEC;
   else if (data2 == AGETIMER_1_SEC)
      pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_1_SEC;
   else
      pPar->eMAC_TableAgeTimer = 0;
    

	/* IGMP Group Aging Time Mantissa */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_IGMP_AGE_0_MANT_OFFSET,	\
		VR9_PCE_IGMP_AGE_0_MANT_SHIFT, VR9_PCE_IGMP_AGE_0_MANT_SIZE, &data1);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MAC_FLEN_LEN_OFFSET,	\
		VR9_MAC_FLEN_LEN_SHIFT, VR9_MAC_FLEN_LEN_SIZE, &value);
	pPar->nMaxPacketLen = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MAC_PFAD_CFG_SAMOD_OFFSET,	\
		VR9_MAC_PFAD_CFG_SAMOD_SHIFT, VR9_MAC_PFAD_CFG_SAMOD_SIZE, &value);
	pPar->bPauseMAC_ModeSrc = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_VLAN_OFFSET,	\
		VR9_PCE_GCTRL_0_VLAN_SHIFT, VR9_PCE_GCTRL_0_VLAN_SIZE, &value);
	pPar->bVLAN_Aware = value;
	/* MAC Address Learning Limitation Mode */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_GCTRL_0_PLIMMOD_OFFSET ,	\
		VR9_PCE_GCTRL_0_PLIMMOD_SHIFT, VR9_PCE_GCTRL_0_PLIMMOD_SIZE, &pPar->bLearningLimitAction);

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MAC_PFSA_0_PFAD_OFFSET,	\
		VR9_MAC_PFSA_0_PFAD_SHIFT, VR9_MAC_PFSA_0_PFAD_SIZE, &value);

	pPar->nPauseMAC_Src[5] = value & 0xFF;
	pPar->nPauseMAC_Src[4] = (value >> 8) & 0xFF;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MAC_PFSA_1_PFAD_OFFSET,	\
		VR9_MAC_PFSA_1_PFAD_SHIFT, VR9_MAC_PFSA_1_PFAD_SIZE, &value);
	pPar->nPauseMAC_Src[3] = value & 0xFF;
	pPar->nPauseMAC_Src[2] = (value >> 8) & 0xFF;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_MAC_PFSA_2_PFAD_OFFSET,	\
		VR9_MAC_PFSA_2_PFAD_SHIFT, VR9_MAC_PFSA_2_PFAD_SIZE, &value);
	pPar->nPauseMAC_Src[1] = value & 0xFF;
	pPar->nPauseMAC_Src[0] = (value >> 8) & 0xFF;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_cfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t MANT = 0, EXP = 0, value, i;

	switch (pPar->eMAC_TableAgeTimer) {
		case IFX_ETHSW_AGETIMER_1_SEC:
			MANT = AGETIMER_1_SEC; EXP = 0x2;
			pEthSWDevHandle->MAC_AgeTimer = 1;
			break;
		case IFX_ETHSW_AGETIMER_10_SEC:
			MANT = AGETIMER_10_SEC; EXP = 0x2;
			pEthSWDevHandle->MAC_AgeTimer = 10;
			break;
		case IFX_ETHSW_AGETIMER_300_SEC:
			MANT = AGETIMER_300_SEC; EXP = 0x2;
			pEthSWDevHandle->MAC_AgeTimer = 300;
			break;
		case IFX_ETHSW_AGETIMER_1_HOUR:
			MANT = AGETIMER_1_HOUR; EXP = 0x6;
			pEthSWDevHandle->MAC_AgeTimer = 3600;
			break;
		case IFX_ETHSW_AGETIMER_1_DAY:
			MANT = AGETIMER_1_DAY; EXP = 0x10;
			pEthSWDevHandle->MAC_AgeTimer = 86400;
			break;
	}

	/* Aging Counter Exponent Value */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_AGE_0_EXP_OFFSET,	\
		VR9_PCE_AGE_0_EXP_SHIFT, VR9_PCE_AGE_0_EXP_SIZE, EXP);
	/* Aging Counter Mantissa Value */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_AGE_1_MANT_OFFSET,	\
		VR9_PCE_AGE_1_MANT_SHIFT, VR9_PCE_AGE_1_MANT_SIZE, MANT);
	
	/* IGMP Group Aging Time Exponent */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_AGE_0_EXP_OFFSET,	\
		VR9_PCE_IGMP_AGE_0_EXP_SHIFT, VR9_PCE_IGMP_AGE_0_EXP_SIZE, EXP);
	/* IGMP Group Aging Time Mantissa */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_IGMP_AGE_0_MANT_OFFSET,	\
		VR9_PCE_IGMP_AGE_0_MANT_SHIFT, VR9_PCE_IGMP_AGE_0_MANT_SIZE, MANT);
	/* Maximum Ethernet packet length */
	if ( pPar->nMaxPacketLen < 0xFFFF )
		value = pPar->nMaxPacketLen;
	else
		value = IFX_ETHSW_MAX_PACKET_LENGTH;

	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_MAC_FLEN_LEN_OFFSET,	\
		VR9_MAC_FLEN_LEN_SHIFT, VR9_MAC_FLEN_LEN_SIZE, value);
	if (pPar->nMaxPacketLen > 0x5EE) {
		for (i = 0; i < 6; i++) {
			ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MAC_CTRL_2_MLEN_OFFSET + (i * 0xC)),	\
				VR9_MAC_CTRL_2_MLEN_SHIFT, VR9_MAC_CTRL_2_MLEN_SIZE, 1);
		}
	}
	/* MAC Address Learning Limitation Mode */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_PLIMMOD_OFFSET ,	\
		VR9_PCE_GCTRL_0_PLIMMOD_SHIFT, VR9_PCE_GCTRL_0_PLIMMOD_SIZE, pPar->bLearningLimitAction);


	/* VLAN-aware Switching           */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_VLAN_OFFSET,	\
		VR9_PCE_GCTRL_0_VLAN_SHIFT, VR9_PCE_GCTRL_0_VLAN_SIZE, pPar->bVLAN_Aware);

	/* MAC Source Address Mode */
	if (pPar->bPauseMAC_ModeSrc == IFX_TRUE) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_MAC_PFAD_CFG_SAMOD_OFFSET,	\
			VR9_MAC_PFAD_CFG_SAMOD_SHIFT, VR9_MAC_PFAD_CFG_SAMOD_SIZE, pPar->bPauseMAC_ModeSrc);

		value = pPar->nPauseMAC_Src[4] << 8 | pPar->nPauseMAC_Src[5];

		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_MAC_PFSA_0_PFAD_OFFSET,	\
			VR9_MAC_PFSA_0_PFAD_SHIFT, VR9_MAC_PFSA_0_PFAD_SIZE, value);

		value = pPar->nPauseMAC_Src[2] << 8 | pPar->nPauseMAC_Src[3];

		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_MAC_PFSA_1_PFAD_OFFSET,	\
			VR9_MAC_PFSA_1_PFAD_SHIFT, VR9_MAC_PFSA_1_PFAD_SIZE, value);

		value = pPar->nPauseMAC_Src[0] << 8 | pPar->nPauseMAC_Src[1];

		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_MAC_PFSA_2_PFAD_OFFSET,	\
			VR9_MAC_PFSA_2_PFAD_SHIFT, VR9_MAC_PFSA_2_PFAD_SIZE, value);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_HW_Init(IFX_void_t *pDevCtx, IFX_ETHSW_HW_Init_t *pPar)
{
	IFX_FLOW_switchDev_t *pDev = (IFX_FLOW_switchDev_t*)pDevCtx;
/*
	if (pDev->bResetCalled == IFX_FALSE) {
		platform_device_reset_trigger();
		pDev->bResetCalled = IFX_TRUE;
	}
*/
	platform_device_init(pDev);
	pDev->bHW_InitCalled = IFX_TRUE;
	/* Software Table Init */
	IFX_FLOW_VLAN_Table_Init(pDev);
	IFX_FLOW_PortCfg_Init(pDev);
	IFX_FLOW_MULTICAST_SW_Table_Init(pDev);
	ifx_pce_table_init(&pDev->PCE_Handler);
	/* HW Init */
	IFX_VR9_Switch_PCE_Micro_Code_Int();
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MDIO_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_cfg_t *pPar)
{
	IFX_uint32_t value;

	/* Get MDC_CFG_1.FREQ value */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDC_CFG_1_FREQ_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDC_CFG_1_FREQ_SHIFT, VR9_MDC_CFG_1_FREQ_SIZE, &value);
	pPar->nMDIO_Speed = value & 0xFF;
	/* Get MDC_CFG_1.MCEN value */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDC_CFG_1_MCEN_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDC_CFG_1_MCEN_SHIFT, VR9_MDC_CFG_1_MCEN_SIZE, &value);
	pPar->bMDIO_Enable = value;
	if(value == 1)
		pPar->bMDIO_Enable = IFX_ENABLE;
	else
		pPar->bMDIO_Enable = IFX_DISABLE;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MDIO_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_cfg_t *pPar)
{
	IFX_uint32_t value;

	/* Set MDC_CFG_1.FREQ value */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MDC_CFG_1_FREQ_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDC_CFG_1_FREQ_SHIFT, VR9_MDC_CFG_1_FREQ_SIZE, pPar->nMDIO_Speed);
	if (pPar->bMDIO_Enable)
		value = 0x3F;
	else
		value = 0;
	/* Set Auto-Polling of connected PHYs - For all ports */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MDC_CFG_0_PEN_0_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDC_CFG_0_PEN_0_SHIFT, 6, value);
	/* Set MDC_CFG_1.MCEN value */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MDC_CFG_1_MCEN_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDC_CFG_1_MCEN_SHIFT, VR9_MDC_CFG_1_MCEN_SIZE, pPar->bMDIO_Enable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MDIO_DataRead(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_data_t *pPar)
{
	IFX_uint32_t value;

	value = 1;
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_CTRL_MBUSY_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MDIO_CTRL_MBUSY_SHIFT, VR9_MDIO_CTRL_MBUSY_SIZE, &value);
	}
	value = ((0x2 << 10 ) | ((pPar->nAddressDev & 0x1F) << 5) | (pPar->nAddressReg & 0x1F));
	/* Special write command, becouse we need to write "MDIO Control Register" once at a time */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MDIO_CTRL_MBUSY_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET), 0, 16, value);
	value = 1;
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_CTRL_MBUSY_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MDIO_CTRL_MBUSY_SHIFT, VR9_MDIO_CTRL_MBUSY_SIZE, &value);
	}
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_READ_RDATA_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDIO_READ_RDATA_SHIFT, VR9_MDIO_READ_RDATA_SIZE, &value);
	pPar->nData = value & 0xFFFF;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MDIO_DataWrite(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_data_t *pPar)
{
	IFX_uint32_t value;

	value = 1;
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_CTRL_MBUSY_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MDIO_CTRL_MBUSY_SHIFT, VR9_MDIO_CTRL_MBUSY_SIZE, &value);
	}
	value = pPar->nData & 0xFFFF;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MDIO_WRITE_WDATA_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MDIO_WRITE_WDATA_SHIFT, VR9_MDIO_WRITE_WDATA_SIZE, value);
	value = ((0x1 << 10 ) | ((pPar->nAddressDev & 0x1F) << 5) | (pPar->nAddressReg & 0x1F));
	/* Special write command, becouse we need to write "MDIO Control Register" once at a time */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MDIO_CTRL_MBUSY_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET), 0, 16, value);
	value = 1;
	while (value == 1) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_CTRL_MBUSY_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MDIO_CTRL_MBUSY_SHIFT, VR9_MDIO_CTRL_MBUSY_SIZE, &value);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MonitorPortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_monitorPortCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;
	
	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
	/* Get PCE Port Map 1 */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_1_MPMAP_OFFSET ,	\
		VR9_PCE_PMAP_1_MPMAP_SHIFT, VR9_PCE_PMAP_1_MPMAP_SIZE, &value);
	if ((( value & (1 << portIdx)) >> portIdx) == 1)
		pPar->bMonitorPort = IFX_TRUE;
	else
		pPar->bMonitorPort = IFX_FALSE;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_MonitorPortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_monitorPortCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;

	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
	/* Get PCE Port Map 1 */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_1_MPMAP_OFFSET ,	\
		VR9_PCE_PMAP_1_MPMAP_SHIFT, VR9_PCE_PMAP_1_MPMAP_SIZE, &value);
	if (pPar->bMonitorPort == 1)
		value |= (pPar->bMonitorPort << portIdx);
	else
		value = (value & ~(1 << portIdx));
	/* Set PCE Port Map 1 */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PMAP_1_MPMAP_OFFSET ,	\
		VR9_PCE_PMAP_1_MPMAP_SHIFT, VR9_PCE_PMAP_1_MPMAP_SIZE, value);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortLinkCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portLinkCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value;
	IFX_uint8_t portIdx = pPar->nPortId;

	if( pPar->nPortId > (pEthSWDevHandle->nPortNumber-1) )
		return IFX_ERROR;
	/* Duplex */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_FDUP_OFFSET - (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PHY_ADDR_0_FDUP_SHIFT, VR9_PHY_ADDR_0_FDUP_SIZE, &value);
	switch(value) {
		case 0:
			pPar->bDuplexForce = IFX_FALSE;
			pPar->eDuplex = IFX_ETHSW_DUPLEX_FULL;
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_STAT_0_FDUP_OFFSET + (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MDIO_STAT_0_FDUP_SHIFT, VR9_MDIO_STAT_0_FDUP_SIZE, &value);
			if ( value == 0)
				pPar->eDuplex = IFX_ETHSW_DUPLEX_HALF;	
			break;
		case 1 : 
			pPar->bDuplexForce = IFX_TRUE;
			pPar->eDuplex = IFX_ETHSW_DUPLEX_FULL;
			break;
		case 3:
			pPar->bDuplexForce = IFX_TRUE;
			pPar->eDuplex = IFX_ETHSW_DUPLEX_HALF;
			break;
		}
   /* Speed */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_SPEED_OFFSET - (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PHY_ADDR_0_SPEED_SHIFT, VR9_PHY_ADDR_0_SPEED_SIZE, &value);
	switch ( value) {
		case 0:
		case 1:
		case 2:
			pPar->bSpeedForce = IFX_TRUE;
			break;
		case 3: /*Auto polling mode */
			pPar->bSpeedForce = IFX_FALSE;
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_STAT_0_SPEED_OFFSET + (0x1 * portIdx) + 	\
				IFX_ETHSW_FLOW_TOP_REG_OFFSET),	VR9_MDIO_STAT_0_SPEED_SHIFT, VR9_MDIO_STAT_0_SPEED_SIZE, &value);
			
		break;
	}
	pPar->eSpeed = IFX_ETHSW_PORT_SPEED_10;
	switch (value) {
		case 1:
			pPar->eSpeed = IFX_ETHSW_PORT_SPEED_100;
			break;
		case 2:
			pPar->eSpeed = IFX_ETHSW_PORT_SPEED_1000;
			break;
	}
	/* State */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_LNKST_OFFSET - (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PHY_ADDR_0_LNKST_SHIFT, VR9_PHY_ADDR_0_LNKST_SIZE, &value);
	switch(value) {
		case 0:
			pPar->bLinkForce = IFX_FALSE;
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MDIO_STAT_0_LSTAT_OFFSET + (0x1 * portIdx) + 	\
				IFX_ETHSW_FLOW_TOP_REG_OFFSET),	VR9_MDIO_STAT_0_LSTAT_SHIFT, VR9_MDIO_STAT_0_LSTAT_SIZE, &value);
			if (value == IFX_TRUE)
				pPar->eLink = IFX_ETHSW_PORT_LINK_UP;
			else
				pPar->eLink = IFX_ETHSW_PORT_LINK_DOWN;
			break;
		case 1:
			pPar->bLinkForce = IFX_TRUE;
			pPar->eLink = IFX_ETHSW_PORT_LINK_UP;
			break;
		case 2:
			pPar->bLinkForce = IFX_TRUE;
			pPar->eLink = IFX_ETHSW_PORT_LINK_DOWN;
			break;
	}

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MII_CFG_0_MIIMODE_OFFSET + (0x2 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_MII_CFG_0_MIIMODE_SHIFT, VR9_MII_CFG_0_MIIMODE_SIZE, &value);

	switch (value) {
		case 0:
			pPar->eMII_Mode = IFX_ETHSW_PORT_HW_MII;
			pPar->eMII_Type = IFX_ETHSW_PORT_PHY;
			break;
		case 1:
			pPar->eMII_Mode = IFX_ETHSW_PORT_HW_MII ;
			pPar->eMII_Type = IFX_ETHSW_PORT_MAC;
			break;
		case 2:
			pPar->eMII_Mode = IFX_ETHSW_PORT_HW_RMII;
			pPar->eMII_Type = IFX_ETHSW_PORT_PHY;
			break;
		case 3:
			pPar->eMII_Mode = IFX_ETHSW_PORT_HW_RMII;
			pPar->eMII_Type = IFX_ETHSW_PORT_MAC;
			break;
		case 4:
			pPar->eMII_Mode = IFX_ETHSW_PORT_HW_RGMII;
			pPar->eMII_Type = IFX_ETHSW_PORT_MAC;
			break;
	}
	pPar->eClkMode = IFX_ETHSW_PORT_CLK_NA;
	if (pPar->eMII_Mode == IFX_ETHSW_PORT_HW_RMII ) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MII_CFG_0_RMII_OFFSET + (0x2 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MII_CFG_0_RMII_SHIFT, VR9_MII_CFG_0_RMII_SIZE, &value);
		if (value == 1)
			pPar->eClkMode = IFX_ETHSW_PORT_CLK_MASTER;
		else
			pPar->eClkMode = IFX_ETHSW_PORT_CLK_SLAVE;	
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortLinkCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portLinkCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t data,phy_addr ;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_ETHSW_MDIO_data_t mdio_data; 
	if( pPar->nPortId > (pEthSWDevHandle->nPortNumber-1) )
		return IFX_ERROR;
	if (pPar->bSpeedForce == IFX_ETHSW_PORT_SPEED_200 ) {
		IFXOS_PRINT_INT_RAW("Error: (GSWITCH not supported) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}		
	if (pPar->bDuplexForce == IFX_TRUE) {
		if( pPar->eDuplex == IFX_ETHSW_DUPLEX_FULL)
			data = DUPLEX_EN;
		else
			data = DUPLEX_DIS;
	} else {
		data = DUPLEX_AUTO; 
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PHY_ADDR_0_FDUP_OFFSET - (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PHY_ADDR_0_FDUP_SHIFT, VR9_PHY_ADDR_0_FDUP_SIZE, data);
	/* Specify PHY Address */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_ADDR_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET - portIdx),	\
		VR9_PHY_ADDR_0_ADDR_SHIFT, VR9_PHY_ADDR_0_ADDR_SIZE, &phy_addr);
	mdio_data.nAddressDev = phy_addr;
	/* Speed */	 
	data = 3; /*default value*/
	if (pPar->bSpeedForce == IFX_TRUE ) {
		switch(pPar->eSpeed) {
			case IFX_ETHSW_PORT_SPEED_10:
				data = 0;
				break;
			case IFX_ETHSW_PORT_SPEED_100:
				data = 1;
				break;
			case IFX_ETHSW_PORT_SPEED_200:
/*				IFXOS_PRINT_INT_RAW("Warning: (GSWITCH not supported) %s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__); */
				break;
			case IFX_ETHSW_PORT_SPEED_1000:
				data = 2;
				break;
		}
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PHY_ADDR_0_SPEED_OFFSET - (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PHY_ADDR_0_SPEED_SHIFT, VR9_PHY_ADDR_0_SPEED_SIZE, data);
	data = 0;
	mdio_data.nAddressReg = 0x0;
	mdio_data.nAddressDev = phy_addr;
	mdio_data.nData = 0x1000;
	if(pPar->bLinkForce == IFX_TRUE ) {
		if (pPar->eLink == IFX_ETHSW_PORT_LINK_UP ) {
			data = 1;
		} else {
			data = 2;
			mdio_data.nData = 0x800;
		}
	} 
	IFX_FLOW_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PHY_ADDR_0_LNKST_OFFSET - (0x1 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PHY_ADDR_0_LNKST_SHIFT, VR9_PHY_ADDR_0_LNKST_SIZE, data);
	data = 4 /*IFX_ETHSW_PORT_HW_RGMII */; /*default mode */
	switch(pPar->eMII_Mode) {
		case IFX_ETHSW_PORT_HW_MII:
			data = 1;
			if (pPar->eMII_Type == IFX_ETHSW_PORT_PHY )
				data = 0;	
			break;
		case IFX_ETHSW_PORT_HW_RMII:
			data = 3;
			if (pPar->eMII_Type == IFX_ETHSW_PORT_PHY )
				data = 2;	
			break;
		case IFX_ETHSW_PORT_HW_RGMII:
			data = 4;
			break;
		case IFX_ETHSW_PORT_HW_GMII:
/*			data = 1; */
			break; 
	}
	if (! (pPar->eMII_Mode == IFX_ETHSW_PORT_HW_GMII) )
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MII_CFG_0_MIIMODE_OFFSET + (0x2 * portIdx) +	\
			IFX_ETHSW_FLOW_TOP_REG_OFFSET),	VR9_MII_CFG_0_MIIMODE_SHIFT, VR9_MII_CFG_0_MIIMODE_SIZE, data);
	data = 0;
	if (pPar->eMII_Mode == IFX_ETHSW_PORT_HW_RMII ) {
		if (pPar->eClkMode == IFX_ETHSW_PORT_CLK_MASTER) {
			data = 1;
		}
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MII_CFG_0_RMII_OFFSET+ (0x2 * portIdx) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_MII_CFG_0_RMII_SHIFT, VR9_MII_CFG_0_RMII_SIZE, data);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortPHY_AddrGet(IFX_void_t *pDevCtx, IFX_ETHSW_portPHY_Addr_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;

	if( pPar->nPortId > ( pEthSWDevHandle->nPortNumber - 1) )
		return IFX_ERROR;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PHY_ADDR_0_ADDR_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET - portIdx),	\
		VR9_PHY_ADDR_0_ADDR_SHIFT, VR9_PHY_ADDR_0_ADDR_SIZE, &value);
	pPar->nAddressDev = value;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortPHY_Query(IFX_void_t *pDevCtx, IFX_ETHSW_portPHY_Query_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;

	if( pPar->nPortId > ( pEthSWDevHandle->nPortNumber - 1 ) )
		return IFX_ERROR;

	/* Read out the MAC_PSTAT.PACT */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MAC_PSTAT_PACT_OFFSET + (0xC * portIdx)),	\
		VR9_MAC_PSTAT_PACT_SHIFT, VR9_MAC_PSTAT_PACT_SIZE , &value);
	pPar->bPHY_Present = value;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortRGMII_ClkCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portRGMII_ClkCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value;
	
	if( pPar->nPortId > (pEthSWDevHandle->nPortNumber-1) )
		return IFX_ERROR;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCDU_0_RXDLY_OFFSET + (0x2 * pPar->nPortId) + IFX_ETHSW_FLOW_TOP_REG_OFFSET,	\
		VR9_PCDU_0_RXDLY_SHIFT, VR9_PCDU_0_RXDLY_SIZE, &value);
	IFXOS_PRINT_INT_RAW("nDelayRx = %d\n",value);
	pPar->nDelayRx = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCDU_0_TXDLY_OFFSET + (0x2 * pPar->nPortId) + IFX_ETHSW_FLOW_TOP_REG_OFFSET,	\
		VR9_PCDU_0_TXDLY_SHIFT, VR9_PCDU_0_TXDLY_SIZE, &value);
	IFXOS_PRINT_INT_RAW("nDelayTx = %d\n",value);
	pPar->nDelayTx = value;

	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortRGMII_ClkCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portRGMII_ClkCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;

	if( pPar->nPortId > (pEthSWDevHandle->nPortNumber - 1) )
		return IFX_ERROR;
		
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCDU_0_RXDLY_OFFSET + (0x2 * pPar->nPortId) + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PCDU_0_RXDLY_SHIFT, VR9_PCDU_0_RXDLY_SIZE, pPar->nDelayRx);

	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCDU_0_TXDLY_OFFSET + (0x2 * pPar->nPortId) +IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PCDU_0_TXDLY_SHIFT, VR9_PCDU_0_TXDLY_SIZE, pPar->nDelayTx);

	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortRedirectGet(IFX_void_t *pDevCtx, IFX_ETHSW_portRedirectCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;

	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber )
		return IFX_ERROR;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PCTRL_3_EDIR_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_3_EDIR_SHIFT, VR9_PCE_PCTRL_3_EDIR_SIZE, &value);
	pPar->bRedirectEgress = value;
	if ( PortRedirectFlag > 0 )
		pPar->bRedirectIngress = IFX_TRUE;
	else
		pPar->bRedirectIngress = IFX_FALSE;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PortRedirectSet(IFX_void_t *pDevCtx, IFX_ETHSW_portRedirectCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_PCE_rule_t PCE_rule;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t RedirectPort = 0, value = 0, i;

	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber )
		return IFX_ERROR;
	/* The Redirect port need to set as monitor port */
	/* Get PCE Port Map 1 */
	//   ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_PMAP_1_MPMAP_OFFSET ,
	//		VR9_PCE_PMAP_1_MPMAP_SHIFT, VR9_PCE_PMAP_1_MPMAP_SIZE, &value);
	value |= (1 << pEthSWDevHandle->nManagementPortNumber);
	RedirectPort = value;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PMAP_1_MPMAP_OFFSET ,	\
		VR9_PCE_PMAP_1_MPMAP_SHIFT, VR9_PCE_PMAP_1_MPMAP_SIZE, value);
	value = pPar->bRedirectEgress;
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_3_EDIR_OFFSET + (0xA * portIdx)),	\
		VR9_PCE_PCTRL_3_EDIR_SHIFT, VR9_PCE_PCTRL_3_EDIR_SIZE, value);

	if ( pPar->bRedirectIngress == IFX_TRUE)
		PortRedirectFlag |= (1 << pPar->nPortId);
	else
		PortRedirectFlag &= ~(1 << pPar->nPortId) ;

	for (i = 0; i < pEthSWDevHandle->nPortNumber; i++) {
		if ((( PortRedirectFlag >> i ) & 0x1 )== 1) {
			memset(&PCE_rule, 0 , sizeof(IFX_FLOW_PCE_rule_t));
			PCE_rule.pattern.nIndex = (30 + i);
			PCE_rule.pattern.bEnable = IFX_TRUE;
			PCE_rule.pattern.bPortIdEnable = IFX_TRUE;
			PCE_rule.pattern.nPortId = i;
			if (pPar->bRedirectIngress == IFX_TRUE)
				PCE_rule.action.ePortMapAction = IFX_FLOW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			PCE_rule.action.nForwardPortMap = RedirectPort;
			/* We prepare everything and write into PCE Table */
			if (0 != ifx_pce_rule_write(&pEthSWDevHandle->PCE_Handler,&PCE_rule))
				return IFX_ERROR;
		}  else {
			if (0 != ifx_pce_pattern_delete(&pEthSWDevHandle->PCE_Handler, 30+i))
				return IFX_ERROR;
		}
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_RMON_Clear(IFX_void_t *pDevCtx, IFX_ETHSW_RMON_clear_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;

	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;

	/* Reset all RMON counter */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RMON_CTRL_RAM1_RES_OFFSET + (portIdx * 2),	\
		VR9_BM_RMON_CTRL_RAM1_RES_SHIFT, VR9_BM_RMON_CTRL_RAM1_RES_SIZE, 0x1);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RMON_CTRL_RAM2_RES_OFFSET + (portIdx * 2),	\
		VR9_BM_RMON_CTRL_RAM2_RES_SHIFT, VR9_BM_RMON_CTRL_RAM2_RES_SIZE, 0x1);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_RMON_Get(IFX_void_t *pDevCtx, IFX_ETHSW_RMON_cnt_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t  value;
	IFX_uint32_t  data, data0, data1;
	IFX_uint32_t  r_frame = 0, r_unicast = 0, r_multicast = 0;
	IFX_uint32_t  t_frame = 0, t_unicast = 0, t_multicast = 0;
	IFX_uint32_t  R_GoodByteCount_L = 0,  R_BadByteCount_L = 0, T_GoodByteCount_L = 0;
	IFX_uint64_t  R_GoodByteCount_H = 0,  R_BadByteCount_H = 0, T_GoodByteCount_H = 0;
	IFX_uint8_t i;

	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;

	/* Enable the RMON Counter  */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_PCFG_CNTEN_OFFSET + (portIdx * 2),	\
		VR9_BM_PCFG_CNTEN_SHIFT, VR9_BM_PCFG_CNTEN_SIZE, 1);
	memset(pPar, 0 , sizeof(IFX_ETHSW_RMON_cnt_t));
	pPar->nPortId = portIdx;

	for (i = 0; i < IFX_ETHSW_RMON_COUNTER_OFFSET; i++) {
		/* Set the BM RAM ADDR  */
		/* Receive Frame Count  */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
			VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, i);
		/* Specify the port ID */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
			VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, portIdx);
		value = 1;
		/* Active */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
		do {
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
		} while(value);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
			VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, &data0);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_1_VAL1_OFFSET,	\
			VR9_BM_RAM_VAL_1_VAL1_SHIFT, VR9_BM_RAM_VAL_1_VAL1_SIZE, &data1);
		//    IFXOS_PRINT_INT_RAW("The Data we get is = %x\n",(data1 << 16 | data0));
		data = (data1 << 16 | data0);
		switch (i){
			case 0x1F:
				/* Receive Frme Count */
				pPar->nRxGoodPkts = data;
				r_frame = data;
				break;
          case 0x23:
            /* Receive Unicast Frame Count */
            pPar->nRxUnicastPkts = data;
            r_unicast = data;
            break;
          case 0x22:
            /* Receive Multicast Frame Count1 */
            pPar->nRxMulticastPkts = data;
            r_multicast = data;
            break;
          case 0x21:
            /* Receive CRC Errors Count */
            pPar->nRxFCSErrorPkts = data;
            break;
          case 0x1D:
            /* Receive Undersize Good Count */
            pPar->nRxUnderSizeGoodPkts = data;
            break;
          case 0x1B:
            /* Receive Oversize Good Count */
            pPar->nRxOversizeGoodPkts = data;
            break;
          case 0x1E:
            /* Receive Undersize Bad Count */
            pPar->nRxUnderSizeErrorPkts = data;
            break;
          case 0x20:
            /* Receive Pause Good Count */
            pPar->nRxGoodPausePkts = data;
            break;
          case 0x1C:
            /* Receive Oversize Bad Count */
            pPar->nRxOversizeErrorPkts = data;
            break;
          case 0x1A:
            /* Receive Alignment Errors Count */
            pPar->nRxAlignErrorPkts = data;
            break;
          case 0x12:
            /* Receive Size 64 Frame Count1 */
            pPar->nRx64BytePkts = data;
            break;
          case 0x13:
            /* Receive Size 65-127 Frame Count */
            pPar->nRx127BytePkts = data;
            break;
          case 0x14:
            /* Receive Size 128-255 Frame Count */
            pPar->nRx255BytePkts = data;
            break;
          case 0x15:
            /* Receive Size 256-511 Frame Count */
            pPar->nRx511BytePkts = data;
            break;
          case 0x16:
            /* Receive Size 512-1023 Frame Count */
            pPar->nRx1023BytePkts = data;
            break;
          case 0x17:
            /* Receive Size Greater 1023 Frame Count */
            pPar->nRxMaxBytePkts = data;
            break;
          case 0x18:
            /* Receive Discard (Tail-Drop) Frame Count */
            pPar->nRxDroppedPkts = data;
            break;
          case 0x19:
            /* Receive Drop (Filter) Frame Count */
            pPar->nRxFilteredPkts = data;
            break;
          case 0x24:
            /* Receive Good Byte Count (Low) */
            R_GoodByteCount_L = data;
            break;
          case 0x25:
            /* Receive Good Byte Count (High) */
            R_GoodByteCount_H = data;
            break;
          case 0x26:
            /* Receive Bad Byte Count (Low) */
            R_BadByteCount_L = data;
            break;
          case 0x27:
            /* Receive Bad Byte Count (High) */
            R_BadByteCount_H = data;
            break;
      /* =================== */
          case 0x0C:
            /* Transmit Frame Count */
            pPar->nTxGoodPkts = data;
            t_frame = data;
            break;
          case 0x06:
            /* Transmit Unicast Frame Count */
            pPar->nTxUnicastPkts = data;
            t_unicast = data;
            break;
          case 0x07:
            /* Transmit Multicast Frame Count1 */
            pPar->nTxMulticastPkts = data;
            t_multicast = data;
            break;
          case 0x00:
            /* Transmit Size 64 Frame Count */
            pPar->nTx64BytePkts = data;
            break;
          case 0x01:
            /* Transmit Size 65-127 Frame Count */
            pPar->nTx127BytePkts = data;
            break;
          case 0x02:
            /* Transmit Size 128-255 Frame Count */
            pPar->nTx255BytePkts = data;
            break;
          case 0x03:
            /* Transmit Size 256-511 Frame Count */
            pPar->nTx511BytePkts = data;
            break;
          case 0x04:
            /* Transmit Size 512-1023 Frame Count */
            pPar->nTx1023BytePkts = data;
            break;
          case 0x05:
            /* Transmit Size Greater 1024 Frame Count */
            pPar->nTxMaxBytePkts = data;
            break;
          case 0x08:
            /* Transmit Single Collision Count. */
            pPar->nTxSingleCollCount = data;
            break;
          case 0x09:
            /* Transmit Multiple Collision Count */
            pPar->nTxMultCollCount = data;
            break;
          case 0x0A:
            /* Transmit Late Collision Count */
            pPar->nTxLateCollCount = data;
            break;
          case 0x0B:
            /* Transmit Excessive Collision.*/
            pPar->nTxExcessCollCount = data;
            break;
          case 0x0D:
            /* Transmit Pause Frame Count */
            pPar->nTxPauseCount = data;
            break;
          case 0x10:
            /* Transmit Drop Frame Count */
            pPar->nTxDroppedPkts = data;
            break;
          case 0x0E:
            /* Transmit Good Byte Count (Low) */
            T_GoodByteCount_L = data;
            break;
          case 0x0F:
            /* Transmit Good Byte Count (High) */
            T_GoodByteCount_H = data;
            break;
          case 0x11:
            /* Transmit Dropped Packet Cound, based on Congestion Management.*/
            pPar->nTxAcmDroppedPkts = data;
            break;
        }
   }
	/* Receive Broadcase Frme Count */
	pPar->nRxBroadcastPkts = r_frame - r_unicast - r_multicast;
	/* Transmit Broadcase Frme Count */
	pPar->nTxBroadcastPkts = t_frame - t_unicast - t_multicast;
	/* Receive Good Byte Count */
	pPar->nRxGoodBytes = (R_GoodByteCount_H << 32) | R_GoodByteCount_L;
	/* Receive Bad Byte Count */
	pPar->nRxBadBytes = (R_BadByteCount_H << 32) | R_BadByteCount_L;
	/* Transmit Good Byte Count */
	pPar->nTxGoodBytes = (T_GoodByteCount_H << 32) | T_GoodByteCount_L;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_8021X_EAPOL_RuleGet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_EAPOL_Rule_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_STP_8021X_t *pStateHandle = &pEthSWDevHandle->STP_8021x_Config;

	pPar->eForwardPort = pStateHandle->eForwardPort;
	pPar->nForwardPortId = pStateHandle->n8021X_ForwardPortId;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_8021X_EAPOL_RuleSet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_EAPOL_Rule_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_STP_8021X_t *pStateHandle = &pEthSWDevHandle->STP_8021x_Config;
	IFX_FLOW_PCE_rule_t PCE_rule;

	pStateHandle->eForwardPort = pPar->eForwardPort;
	pStateHandle->n8021X_ForwardPortId   = pPar->nForwardPortId;
	memset(&PCE_rule, 0 , sizeof(IFX_FLOW_PCE_rule_t));
	PCE_rule.pattern.nIndex = 60;
	PCE_rule.pattern.bEnable = IFX_TRUE;
	PCE_rule.pattern.bMAC_DstEnable = IFX_TRUE;
	PCE_rule.pattern.nMAC_Dst[0] = 0x01;
	PCE_rule.pattern.nMAC_Dst[1] = 0x80;
	PCE_rule.pattern.nMAC_Dst[2] = 0xC2;
	PCE_rule.pattern.nMAC_Dst[3] = 0x00;
	PCE_rule.pattern.nMAC_Dst[4] = 0x00;
	PCE_rule.pattern.nMAC_Dst[5] = 0x03;
	PCE_rule.pattern.nMAC_Src[5] = 0;
    PCE_rule.pattern.bEtherTypeEnable = IFX_TRUE;
    PCE_rule.pattern.nEtherType = 0x888E;
    PCE_rule.action.eCrossStateAction = IFX_FLOW_PCE_ACTION_CROSS_STATE_CROSS;
    if ((pStateHandle->eForwardPort < 4) && (pStateHandle->eForwardPort > 0))
    	PCE_rule.action.ePortMapAction = pStateHandle->eForwardPort + 1;
    else
    	IFXOS_PRINT_INT_RAW("The Forward port action incorrect\n");
    PCE_rule.action.nForwardPortMap = (1 << pPar->nForwardPortId);
    /* We prepare everything and write into PCE Table */
    if (0 != ifx_pce_rule_write(&pEthSWDevHandle->PCE_Handler,&PCE_rule))
    	return IFX_ERROR;
#if defined(FLOW_TABLE_DEBUG) && FLOW_TABLE_DEBUG
	IFX_FLOW_MAC_DASA_LSB_Table_Print();
#endif
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_8021X_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_portConfig_t *pPortHandle ;
	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
/*	IFX_FLOW_portConfig_t *pPortHandle = &pEthSWDevHandle->PortConfig[pPar->nPortId]; */
	pPortHandle = &pEthSWDevHandle->PortConfig[pPar->nPortId];

	pPar->eState = pPortHandle->ifx_8021x_state;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_8021X_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_portCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_FLOW_portConfig_t *pPortHandle;
	
	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
	pPortHandle = &pEthSWDevHandle->PortConfig[pPar->nPortId];
/*	IFX_FLOW_portConfig_t *pPortHandle = &pEthSWDevHandle->PortConfig[pPar->nPortId]; */

	pPortHandle->ifx_8021x_state = pPar->eState;
	/* Config the Table */
	set_port_state(pDevCtx, pPar->nPortId, pPortHandle->ifx_stp_state, pPortHandle->ifx_8021x_state);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CPU_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortCfg_t *pPar)
{
   IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;

   if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
      return IFX_ERROR;
	if (portIdx ==  pEthSWDevHandle->nCPU_Port /* VRX_PLATFORM_CPU_PORT */)
		pPar->bCPU_PortValid = IFX_ENABLE;
	else
		pPar->bCPU_PortValid = IFX_DISABLE;
	/* Special Tag Egress*/
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_FDMA_PCTRL_STEN_OFFSET + ( 0x6 * portIdx)),	\
		VR9_FDMA_PCTRL_STEN_SHIFT, VR9_FDMA_PCTRL_STEN_SIZE, &value);
	pPar->bSpecialTagEgress = value;
	/* Special Tag Igress*/
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PCTRL_0_IGSTEN_OFFSET + ( 0xa * portIdx)),	\
		VR9_PCE_PCTRL_0_IGSTEN_SHIFT, VR9_PCE_PCTRL_0_IGSTEN_SIZE, &value);
	pPar->bSpecialTagIngress = value;
	/* FCS Check */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_SDMA_PCTRL_FCSIGN_OFFSET + ( 0x6 * portIdx)),	\
		VR9_SDMA_PCTRL_FCSIGN_SHIFT, VR9_SDMA_PCTRL_FCSIGN_SIZE, &value);
	pPar->bFcsCheck = value;
	/* FCS Generate */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_MAC_CTRL_0_FCS_OFFSET + ( 0xC * portIdx)),	\
		VR9_MAC_CTRL_0_FCS_SHIFT, VR9_MAC_CTRL_0_FCS_SIZE, &value);
	pPar->bFcsGenerate = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_FDMA_PCTRL_ST_TYPE_OFFSET + ( 0x6 * portIdx)),	\
		VR9_FDMA_PCTRL_ST_TYPE_SHIFT, VR9_FDMA_PCTRL_ST_TYPE_SIZE, &value);
	 pPar->bSpecialTagEthType = value  ;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CPU_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t RST, AS, AST, RXSH;

   if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
      return IFX_ERROR;
	if (portIdx == pEthSWDevHandle->nCPU_Port /* VRX_PLATFORM_CPU_PORT */)
		pPar->bCPU_PortValid = IFX_ENABLE;
	else
 		pPar->bCPU_PortValid = IFX_DISABLE;
	pEthSWDevHandle->nManagementPortNumber = portIdx;
	/* Special Tag Egress*/
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_STEN_OFFSET + ( 0x6 * portIdx)),	\
		VR9_FDMA_PCTRL_STEN_SHIFT, VR9_FDMA_PCTRL_STEN_SIZE, pPar->bSpecialTagEgress);
	/* VRX CPU port */
	if ( portIdx == pEthSWDevHandle->nCPU_Port /* VRX_PLATFORM_CPU_PORT */ ) {
		if (pPar->bSpecialTagEgress == IFX_FALSE) {
			RST = 1;
			AS = 0;
		} else {
			RST = 0;
			AS = 1;
		}
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_RST_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_HD_CTL_RST_SHIFT, VR9_PMAC_HD_CTL_RST_SIZE, RST);

		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_AS_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_HD_CTL_AS_SHIFT, VR9_PMAC_HD_CTL_AS_SIZE, AS);
	}
	/* Special Tag Igress*/
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PCTRL_0_IGSTEN_OFFSET + ( 0xa * portIdx)),	\
		VR9_PCE_PCTRL_0_IGSTEN_SHIFT, VR9_PCE_PCTRL_0_IGSTEN_SIZE, pPar->bSpecialTagIngress);
	if (pPar->bSpecialTagIngress == IFX_FALSE) {
		AST = 0;
		RXSH = 0;
	} else {
		AST = 1;
		RXSH = 1;
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_AST_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_AST_SHIFT, VR9_PMAC_HD_CTL_AST_SIZE, AST);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_RXSH_OFFSET+ IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_RXSH_SHIFT, VR9_PMAC_HD_CTL_RXSH_SIZE, RXSH);
	/* FCS Check */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_SDMA_PCTRL_FCSIGN_OFFSET + ( 0x6 * portIdx)),	\
		VR9_SDMA_PCTRL_FCSIGN_SHIFT, VR9_SDMA_PCTRL_FCSIGN_SIZE, pPar->bFcsCheck);
	/* FCS Generate */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_MAC_CTRL_0_FCS_OFFSET + ( 0xC * portIdx)),	\
		VR9_MAC_CTRL_0_FCS_SHIFT, VR9_MAC_CTRL_0_FCS_SIZE, pPar->bFcsGenerate);
	if ( pPar->bSpecialTagEthType == IFX_ETHSW_CPU_ETHTYPE_FLOWID ) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_ST_TYPE_OFFSET + ( 0x6 * portIdx)),	\
			VR9_FDMA_PCTRL_ST_TYPE_SHIFT, VR9_FDMA_PCTRL_ST_TYPE_SIZE, 1);
	} else {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_ST_TYPE_OFFSET + ( 0x6 * portIdx)),	\
			VR9_FDMA_PCTRL_ST_TYPE_SHIFT, VR9_FDMA_PCTRL_ST_TYPE_SIZE, 0);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CPU_PortExtendCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortExtendCfg_t *pPar)
{
	IFX_uint32_t  value, value_add, value_vlan;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_HD_CTL_ADD_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_ADD_SHIFT, VR9_PMAC_HD_CTL_ADD_SIZE, &value_add);

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_HD_CTL_TAG_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_TAG_SHIFT, VR9_PMAC_HD_CTL_TAG_SIZE, &value_vlan);
	if (value_add == 0 && value_vlan == 0)
		pPar->eHeaderAdd = 0;
	else if ( value_add == 1 && value_vlan == 0)
		pPar->eHeaderAdd = 1;
	else if ( value_add == 1 && value_vlan == 1)
		pPar->eHeaderAdd = 2;
	else
		pPar->eHeaderAdd = 0;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_HD_CTL_RL2_OFFSET +IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_RL2_SHIFT, VR9_PMAC_HD_CTL_RL2_SIZE, &value);
	pPar->bHeaderRemove = value;
	memset(&pPar->sHeader, 0 , sizeof(IFX_ETHSW_CPU_Header_t));
	if ( value_add == 1 ) {
 		/* Output the Src MAC */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_SA3_SA_15_0_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_SA3_SA_15_0_SHIFT, VR9_PMAC_SA3_SA_15_0_SIZE, &value);
		pPar->sHeader.nMAC_Src[0] = value & 0xFF;
		pPar->sHeader.nMAC_Src[1] = ((value >> 8 ) & 0xFF);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_SA2_SA_31_16_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_SA2_SA_31_16_SHIFT, VR9_PMAC_SA2_SA_31_16_SIZE, &value);
		pPar->sHeader.nMAC_Src[2] = value & 0xFF;
		pPar->sHeader.nMAC_Src[3] = ((value >> 8 ) & 0xFF);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_SA1_SA_47_32_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_SA1_SA_47_32_SHIFT, VR9_PMAC_SA1_SA_47_32_SIZE, &value);
		pPar->sHeader.nMAC_Src[4] = value & 0xFF;
		pPar->sHeader.nMAC_Src[5] = ((value >> 8 ) & 0xFF);
		/* Output the Dst MAC */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_DA3_DA_15_0_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_DA3_DA_15_0_SHIFT, VR9_PMAC_DA3_DA_15_0_SIZE, &value);
		pPar->sHeader.nMAC_Dst[0] = value & 0xFF;;
		pPar->sHeader.nMAC_Dst[1] = ((value >> 8 ) & 0xFF);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_DA2_DA_31_16_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_DA2_DA_31_16_SHIFT, VR9_PMAC_DA2_DA_31_16_SIZE, &value);
		pPar->sHeader.nMAC_Dst[2] = value & 0xFF;;
		pPar->sHeader.nMAC_Dst[3] = ((value >> 8 ) & 0xFF);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_DA1_SA_47_32_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_DA1_SA_47_32_SHIFT, VR9_PMAC_DA1_SA_47_32_SIZE, &value);
		pPar->sHeader.nMAC_Dst[4] = value & 0xFF;;
		pPar->sHeader.nMAC_Dst[5] = ((value >> 8 ) & 0xFF);
		/* Input the Ethernet Type */
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_TL_TYPE_LEN_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_TL_TYPE_LEN_SHIFT, VR9_PMAC_TL_TYPE_LEN_SIZE, &value);
		pPar->sHeader.nEthertype = value;
	}
   if ( value_vlan == 1) {
      ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_VLAN_PRI_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
      	VR9_PMAC_VLAN_PRI_SHIFT, VR9_PMAC_VLAN_PRI_SIZE, &value);
      pPar->sHeader.nVLAN_Prio = value;
      ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_VLAN_CFI_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
      	VR9_PMAC_VLAN_CFI_SHIFT, VR9_PMAC_VLAN_CFI_SIZE, &value);
      pPar->sHeader.nVLAN_CFI = value;
      ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_VLAN_VLAN_ID_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
      	VR9_PMAC_VLAN_VLAN_ID_SHIFT, VR9_PMAC_VLAN_VLAN_ID_SIZE, &value);
      pPar->sHeader.nVLAN_ID = value;
   }
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_HD_CTL_FC_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_FC_SHIFT, VR9_PMAC_HD_CTL_FC_SIZE, &value);
	pPar->ePauseCtrl = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_HD_CTL_RC_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_RC_SHIFT, VR9_PMAC_HD_CTL_RC_SIZE, &value);
	pPar->bFcsRemove = value;
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PMAC_EWAN_EWAN_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_EWAN_EWAN_SHIFT, VR9_PMAC_EWAN_EWAN_SIZE, &value);
	pPar->nWAN_Ports = value;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_CPU_PortExtendCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortExtendCfg_t *pPar)
{
	IFX_uint32_t  value_add =0, value_vlan=0;
	IFX_uint32_t  nMAC;

	switch (pPar->eHeaderAdd) {
		case IFX_ETHSW_CPU_HEADER_NO:
			value_add = 0;
			value_vlan = 0;
			break;
		case IFX_ETHSW_CPU_HEADER_MAC:
			value_add = 1;
			value_vlan = 0;
			break;
		case IFX_ETHSW_CPU_HEADER_VLAN:
			value_add = 1;
			value_vlan = 1;
			break;
	}
	if ((pPar->bHeaderRemove == IFX_ENABLE) && (pPar->eHeaderAdd != IFX_ETHSW_CPU_HEADER_NO)) {
		IFXOS_PRINT_INT_RAW("The Header Can't be remove because the Header Add parameter is not 0");
		return IFX_ERROR;
	}  else {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_RL2_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_HD_CTL_RL2_SHIFT, VR9_PMAC_HD_CTL_RL2_SIZE, pPar->bHeaderRemove);
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_ADD_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_ADD_SHIFT, VR9_PMAC_HD_CTL_ADD_SIZE, value_add);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_TAG_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_TAG_SHIFT, VR9_PMAC_HD_CTL_TAG_SIZE, value_vlan);
	if (pPar->eHeaderAdd == IFX_ETHSW_CPU_HEADER_MAC) {
		/* Input the Src MAC */
		nMAC = pPar->sHeader.nMAC_Src[0] | pPar->sHeader.nMAC_Src[1] << 8;
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_SA3_SA_15_0_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_SA3_SA_15_0_SHIFT, VR9_PMAC_SA3_SA_15_0_SIZE, nMAC);
		nMAC = pPar->sHeader.nMAC_Src[2] | pPar->sHeader.nMAC_Src[3] << 8;
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_SA2_SA_31_16_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_SA2_SA_31_16_SHIFT, VR9_PMAC_SA2_SA_31_16_SIZE, nMAC);
		nMAC = pPar->sHeader.nMAC_Src[4] | pPar->sHeader.nMAC_Src[5] << 8;
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_SA1_SA_47_32_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_SA1_SA_47_32_SHIFT, VR9_PMAC_SA1_SA_47_32_SIZE, nMAC);
		/* Input the Dst MAC */
		nMAC = pPar->sHeader.nMAC_Dst[0] | pPar->sHeader.nMAC_Dst[1] << 8;
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_DA3_DA_15_0_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_DA3_DA_15_0_SHIFT, VR9_PMAC_DA3_DA_15_0_SIZE, nMAC);
		nMAC = pPar->sHeader.nMAC_Dst[2] | pPar->sHeader.nMAC_Dst[3] << 8;
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_DA2_DA_31_16_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_DA2_DA_31_16_SHIFT, VR9_PMAC_DA2_DA_31_16_SIZE, nMAC);
		nMAC = pPar->sHeader.nMAC_Dst[4] | pPar->sHeader.nMAC_Dst[5] << 8;
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_DA1_SA_47_32_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_DA1_SA_47_32_SHIFT, VR9_PMAC_DA1_SA_47_32_SIZE, nMAC);
		/* Input the Ethernet Type */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_TL_TYPE_LEN_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_TL_TYPE_LEN_SHIFT, VR9_PMAC_TL_TYPE_LEN_SIZE, pPar->sHeader.nEthertype);
	}
	if (pPar->eHeaderAdd == IFX_ETHSW_CPU_HEADER_VLAN) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_VLAN_PRI_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
			VR9_PMAC_VLAN_PRI_SHIFT, VR9_PMAC_VLAN_PRI_SIZE, pPar->sHeader.nVLAN_Prio);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_VLAN_CFI_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_VLAN_CFI_SHIFT, VR9_PMAC_VLAN_CFI_SIZE, pPar->sHeader.nVLAN_CFI);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_VLAN_VLAN_ID_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_VLAN_VLAN_ID_SHIFT, VR9_PMAC_VLAN_VLAN_ID_SIZE, pPar->sHeader.nVLAN_ID);
	}
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_FC_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_FC_SHIFT, VR9_PMAC_HD_CTL_FC_SIZE, pPar->ePauseCtrl);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_HD_CTL_RC_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_HD_CTL_RC_SHIFT, VR9_PMAC_HD_CTL_RC_SIZE, pPar->bFcsRemove);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PMAC_EWAN_EWAN_OFFSET + IFX_ETHSW_FLOW_TOP_REG_OFFSET),	\
		VR9_PMAC_EWAN_EWAN_SHIFT, VR9_PMAC_EWAN_EWAN_SIZE, pPar->nWAN_Ports);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_WoL_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_Cfg_t *pPar)
{
	IFX_uint32_t value;

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_GLB_CTRL_PASSEN_OFFSET,	\
	VR9_WOL_GLB_CTRL_PASSEN_SHIFT, VR9_WOL_GLB_CTRL_PASSEN_SIZE, &value);
	pPar->bWolPasswordEnable = value;
	/* ToDo: Require future fill in the DstMac & Password */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_DA_2_DA2_OFFSET,VR9_WOL_DA_2_DA2_SHIFT,	\
		VR9_WOL_DA_2_DA2_SIZE, &value );
	pPar->nWolMAC[0] = (value >> 8 & 0xFF);
	pPar->nWolMAC[1] = (value & 0xFF);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_DA_1_DA1_OFFSET,VR9_WOL_DA_1_DA1_SHIFT,	\
		VR9_WOL_DA_1_DA1_SIZE, &value );
	pPar->nWolMAC[2] = (value >> 8 & 0xFF);
	pPar->nWolMAC[3] = (value & 0xFF);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_DA_0_DA0_OFFSET,VR9_WOL_DA_0_DA0_SHIFT,	\
		VR9_WOL_DA_0_DA0_SIZE, &value );
	pPar->nWolMAC[4] = (value >> 8 & 0xFF);
	pPar->nWolMAC[5] = (value & 0xFF);

	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_PW_2_PW2_OFFSET,VR9_WOL_PW_2_PW2_SHIFT,	\
		VR9_WOL_PW_2_PW2_SIZE, &value );
	pPar->nWolPassword[0] = (value >> 8 & 0xFF);
	pPar->nWolPassword[1] = (value & 0xFF);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_PW_1_PW1_OFFSET,VR9_WOL_PW_1_PW1_SHIFT,	\
		VR9_WOL_PW_1_PW1_SIZE, &value );
	pPar->nWolPassword[2] = (value >> 8 & 0xFF);
	pPar->nWolPassword[3] = (value & 0xFF);
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_WOL_PW_0_PW0_OFFSET,VR9_WOL_PW_0_PW0_SHIFT,	\
		VR9_WOL_PW_0_PW0_SIZE, &value );
	pPar->nWolPassword[4] = (value >> 8 & 0xFF);
	pPar->nWolPassword[5] = (value & 0xFF);
	
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_WoL_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_Cfg_t *pPar)
{
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_GLB_CTRL_PASSEN_OFFSET,	\
		VR9_WOL_GLB_CTRL_PASSEN_SHIFT, VR9_WOL_GLB_CTRL_PASSEN_SIZE, pPar->bWolPasswordEnable);
	/* ToDo: Require future fill in the DstMac & Password */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_DA_2_DA2_OFFSET, VR9_WOL_DA_2_DA2_SHIFT,	\
		VR9_WOL_DA_2_DA2_SIZE, ( ( (pPar->nWolMAC[0] & 0xFF) << 8 )| (pPar->nWolMAC[1] & 0xFF) ) );
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_DA_1_DA1_OFFSET,VR9_WOL_DA_1_DA1_SHIFT,	\
		VR9_WOL_DA_1_DA1_SIZE, ( ( (pPar->nWolMAC[2] &0xFF) << 8)| (pPar->nWolMAC[3] & 0xFF)) );
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_DA_0_DA0_OFFSET,VR9_WOL_DA_0_DA0_SHIFT,	\
		VR9_WOL_DA_0_DA0_SIZE, ( ((pPar->nWolMAC[4] & 0xFF) << 8) | (pPar->nWolMAC[5] & 0xFF)) );
		
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_PW_2_PW2_OFFSET,VR9_WOL_PW_2_PW2_SHIFT,	\
		VR9_WOL_PW_2_PW2_SIZE, ( ((pPar->nWolPassword[0] & 0xFF) << 8)| (pPar->nWolPassword[1] & 0xFF)) );
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_PW_1_PW1_OFFSET,VR9_WOL_PW_1_PW1_SHIFT,	\
		VR9_WOL_PW_1_PW1_SIZE, (  ( (pPar->nWolPassword[2] & 0xFF) << 8) | (pPar->nWolPassword[3] & 0xFF)) );
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_WOL_PW_0_PW0_OFFSET,VR9_WOL_PW_0_PW0_SHIFT,	\
		VR9_WOL_PW_0_PW0_SIZE, ( ( (pPar->nWolPassword[4] & 0xFF)  << 8 )| (pPar->nWolPassword[5] & 0xFF)));
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_WoL_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_PortCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	/* Read the WOL_CTRL.PORT */
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_WOL_CTRL_PORT_OFFSET + (0xA * pPar->nPortId)),	\
		VR9_WOL_CTRL_PORT_SHIFT, VR9_WOL_CTRL_PORT_SIZE, &pPar->bWakeOnLAN_Enable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_WoL_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_PortCfg_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber )
		return IFX_ERROR;
	/* Write to WOL_CTRL.PORT */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_WOL_CTRL_PORT_OFFSET + (0xA * pPar->nPortId)),	\
		VR9_WOL_CTRL_PORT_SHIFT, VR9_WOL_CTRL_PORT_SIZE, pPar->bWakeOnLAN_Enable);
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_RegisterGet(IFX_void_t *pDevCtx, IFX_FLOW_register_t *pPar)
{
	IFX_uint32_t regValue, regAddr = pPar->nRegAddr;
	regValue = VR9_REG32_ACCESS(VR9_BASE_ADDRESS + regAddr * 4);
	pPar->nData = regValue;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_RegisterSet(IFX_void_t *pDevCtx, IFX_FLOW_register_t *pPar)
{
	IFX_uint32_t regValue = pPar->nData, regAddr = pPar->nRegAddr  ;
	VR9_REG32_ACCESS(VR9_BASE_ADDRESS + regAddr * 4) = regValue;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_IrqGet(IFX_void_t *pDevCtx, IFX_FLOW_irq_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	/* ToDo: Require future clarify for how to display */
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_IrqMaskGet(IFX_void_t *pDevCtx, IFX_FLOW_irq_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t value;

	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
 		return IFX_ERROR;
	/* Set PCE.PIER */
	if (pPar->eIrqSrc == IFX_FLOW_IRQ_WOL) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PIER_WOL_OFFSET + (0xA * portIdx)),	\
			VR9_PCE_PIER_WOL_SHIFT, VR9_PCE_PIER_WOL_SIZE, &value);
	IFXOS_PRINT_INT_RAW("The Wake-on-LAN Interrupt is %d\n",value);
	} else if(pPar->eIrqSrc == IFX_FLOW_IRQ_LIMIT_ALERT) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PIER_LOCK_OFFSET + (0xA * portIdx)),	\
			VR9_PCE_PIER_LOCK_SHIFT, VR9_PCE_PIER_LOCK_SIZE, &value);
		IFXOS_PRINT_INT_RAW("The Port Limit Alert Interrupt is %d\n",value);
	} else if(pPar->eIrqSrc == IFX_FLOW_IRQ_LOCK_ALERT) {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, (VR9_PCE_PIER_LIM_OFFSET + (0xA * portIdx)),	\
			VR9_PCE_PIER_LIM_SHIFT, VR9_PCE_PIER_LIM_SIZE, &value);
		IFXOS_PRINT_INT_RAW("The Port Lock Alert Interrupt is %d\n",value);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_IrqMaskSet(IFX_void_t *pDevCtx, IFX_FLOW_irq_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;

	if( pPar->nPortId > pEthSWDevHandle->nTotalPortNumber)
		return IFX_ERROR;
	/* Set PCE.PIER */
	if (pPar->eIrqSrc == IFX_FLOW_IRQ_WOL) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PIER_WOL_OFFSET + (0xA * portIdx)),	\
			VR9_PCE_PIER_WOL_SHIFT, VR9_PCE_PIER_WOL_SIZE, 1);
		IFXOS_PRINT_INT_RAW("Enable Wake-on-LAN Interrupt \n");
	} else if(pPar->eIrqSrc == IFX_FLOW_IRQ_LIMIT_ALERT) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx,(VR9_PCE_PIER_LOCK_OFFSET + (0xA * portIdx)),	\
			VR9_PCE_PIER_LOCK_SHIFT, VR9_PCE_PIER_LOCK_SIZE, 1);
		IFXOS_PRINT_INT_RAW("Enable Port Limit Alert Interrupt \n");
	} else if(pPar->eIrqSrc == IFX_FLOW_IRQ_LOCK_ALERT) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_PCE_PIER_LIM_OFFSET + (0xA * portIdx)),	\
			VR9_PCE_PIER_LIM_SHIFT, VR9_PCE_PIER_LIM_SIZE, 1);
		IFXOS_PRINT_INT_RAW("Enable Port Lock Alert Interrupt \n");
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_IrqStatusClear(IFX_void_t *pDevCtx, IFX_FLOW_irq_t *pPar)
{
	/* ToDo: Request future clarify */
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PceRuleRead(IFX_void_t *pDevCtx, IFX_FLOW_PCE_rule_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;

	if (0 != ifx_pce_rule_read(&pEthSWDevHandle->PCE_Handler, pPar))
		return IFX_ERROR;
  return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PceRuleWrite(IFX_void_t *pDevCtx, IFX_FLOW_PCE_rule_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;

	if (0 != ifx_pce_rule_write(&pEthSWDevHandle->PCE_Handler, pPar))
 		return IFX_ERROR;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_PceRuleDelete(IFX_void_t *pDevCtx, IFX_FLOW_PCE_ruleDelete_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint32_t value;

	value = pPar->nIndex;
	if (0 != ifx_pce_pattern_delete(&pEthSWDevHandle->PCE_Handler, value))
		return IFX_ERROR;
	if (0 != ifx_pce_action_delete(&pEthSWDevHandle->PCE_Handler, value))
		return IFX_ERROR;
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_RMON_ExtendGet(IFX_void_t *pDevCtx, IFX_FLOW_RMON_extendGet_t *pPar)
{
	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t portIdx = pPar->nPortId;
	IFX_uint32_t  value;
	IFX_uint8_t   i;
	IFX_uint32_t  data0, data1;

	if( pPar->nPortId > pEthSWDevHandle->nPortNumber )
		return IFX_ERROR;
	memset(pPar, 0 , sizeof(IFX_FLOW_RMON_extendGet_t));
	for (i = 0; i < IFX_FLOW_RMON_EXTEND_NUM; i++) {
		/* Set the BM RAM ADDR  */
		/* Receive Frame Count  */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_ADDR_ADDR_OFFSET,	\
			VR9_BM_RAM_ADDR_ADDR_SHIFT, VR9_BM_RAM_ADDR_ADDR_SIZE, (i+RMON_EXTEND_TRAFFIC_FLOW_COUNT_1));
		/* Specify the port ID */
 		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_ADDR_OFFSET,	\
 			VR9_BM_RAM_CTRL_ADDR_SHIFT, VR9_BM_RAM_CTRL_ADDR_SIZE, portIdx);
		value = 1;
		/* Active */
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
			VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, value);
		while (value == 1) {
			ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_CTRL_BAS_OFFSET,	\
				VR9_BM_RAM_CTRL_BAS_SHIFT, VR9_BM_RAM_CTRL_BAS_SIZE, &value);
		}
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_0_VAL0_OFFSET,	\
			VR9_BM_RAM_VAL_0_VAL0_SHIFT, VR9_BM_RAM_VAL_0_VAL0_SIZE, &data0);
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_BM_RAM_VAL_1_VAL1_OFFSET,	\
			VR9_BM_RAM_VAL_1_VAL1_SHIFT, VR9_BM_RAM_VAL_1_VAL1_SIZE, &data1);
		pPar->nTrafficFlowCnt[i] = (data1 << 16 | data0);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_Reset(IFX_void_t *pDevCtx, IFX_FLOW_reset_t *pPar)
{
	IFX_FLOW_switchDev_t *pDev = (IFX_FLOW_switchDev_t*)pDevCtx;

	pDev->bResetCalled = IFX_TRUE;
	pDev->bHW_InitCalled = IFX_FALSE;
	/* Reset the Switch via RCU reset bit*/
	platform_device_reset_trigger();
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_VersionGet(IFX_void_t *pDevCtx, IFX_ETHSW_version_t *pPar)
{
	if (pPar->nId == 0) {
		memcpy(pPar->cName, VERSION_NAME, sizeof(VERSION_NAME));
		memcpy(pPar->cVersion, VERSION_NUMBER, sizeof(VERSION_NUMBER));
	} else if (pPar->nId == 1) {
		memcpy(pPar->cName, MICRO_CODE_VERSION_NAME, sizeof(MICRO_CODE_VERSION_NAME));
		memcpy(pPar->cVersion, MICRO_CODE_VERSION_NUMBER, sizeof(MICRO_CODE_VERSION_NUMBER));
	} else {
		memcpy(pPar->cName, "", 0);
		memcpy(pPar->cVersion, "", 0);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_Enable(IFX_void_t *pDevCtx)
{
	IFX_FLOW_switchDev_t *pDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t j;

	if (pDev->bHW_InitCalled == IFX_FALSE)
		platform_device_init(pDev);
	/* Enable all physical port - FDMA_PCTRL.EN & SDMA_PCTRL.PEN */
	for (j =0; j < pDev->nPortNumber; j++) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_EN_OFFSET + (j * 0x6)),	\
			VR9_FDMA_PCTRL_EN_SHIFT, VR9_FDMA_PCTRL_EN_SIZE, 1);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),	\
			VR9_SDMA_PCTRL_PEN_SHIFT, VR9_SDMA_PCTRL_PEN_SIZE, 1);
	}
	return IFX_SUCCESS;
}

IFX_return_t IFX_FLOW_Disable(IFX_void_t *pDevCtx)
{
	IFX_FLOW_switchDev_t *pDev = (IFX_FLOW_switchDev_t*)pDevCtx;
	IFX_uint8_t j;

	/* Disable all physical port - FDMA_PCTRL.EN & SDMA_PCTRL.PEN */
	for (j =0; j < pDev->nPortNumber; j++) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_EN_OFFSET + (j * 0x6)),	\
			VR9_FDMA_PCTRL_EN_SHIFT, VR9_FDMA_PCTRL_EN_SIZE, 0);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),	\
		VR9_SDMA_PCTRL_PEN_SHIFT, VR9_SDMA_PCTRL_PEN_SIZE, 0);
	}
	return IFX_SUCCESS;
}
