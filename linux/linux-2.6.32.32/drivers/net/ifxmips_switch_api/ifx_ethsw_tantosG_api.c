/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_tantosG_api.c
   \remarks implement SWITCH API declared on ifx_ethsw_PSB6970_ll.h
 *****************************************************************************/

#include <ifx_ethsw_PSB6970_core.h>
#include <ifx_ethsw_pm.h>

/********************/
/* Global Variables */
/********************/
IFX_PSB6970_switchDev_t *pCoreDev[IFX_PSB6970_DEV_MAX];

/******************************/
/* Local Macros & Definitions */
/******************************/
#define IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE        0xFF
#define IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE         0xFF
#define IFX_PSB6970_MFC_ENTRY_NOEXIST                   0xFF
#define IFX_PSB6970_VLAN_TABLE_ENTRY_ADD                1
#define IFX_PSB6970_VLAN_TABLE_ENTRY_DELETE             2
#define IFX_PSB6970_MDIO_OP_WRITE                       0x01
#define IFX_PSB6970_MDIO_OP_READ                        0x02
#define IFX_PSB6970_TIMEOUTCOUNT                        1000
#define IFX_PSB6970_INTERNAL_SWITCH_PHY_ID_BASE         0x10
#define IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID    0x11
#define IFX_PSB6970_PRIORITY_QUEUE_COUNT                4
#define IFX_PSB6970_FILTER_ENTRY_NUMBER                 8

/**********************************/
/* External Variables & Functions */
/**********************************/
extern unsigned int g_debug;
extern IFX_return_t IFX_PSB6970_RML_Read(IFX_void_t * pCxtHandle,
                                    IFX_ETHSW_regMapperSelector_t commonbit,
                                    IFX_uint32_t portIdx,
                                    IFX_uint32_t *value);
extern IFX_return_t IFX_PSB6970_RML_Write(IFX_void_t *pCxtHandle,
                            IFX_ETHSW_regMapperSelector_t commonbit,
                            IFX_uint32_t portIdx,
                            IFX_uint32_t value);
extern IFX_return_t IFX_PSB6970_RML_RegisterGet(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value);
extern IFX_return_t IFX_PSB6970_RML_RegisterSet(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value);
IFX_return_t IFX_PSB6970_MAC_TableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRead_t *pPar);
IFX_return_t IFX_PSB6970_MAC_TableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRemove_t *pPar);
IFX_uint8_t IFX_PSB6970_PHY_LINK_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD);
IFX_uint8_t IFX_PSB6970_GPHY_SPEED_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD);
IFX_uint8_t IFX_PSB6970_GPHY_LINK_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD);
IFX_uint8_t IFX_PSB6970_GPHY_DUPLEX_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD);
IFX_return_t IFX_PSB6970_PHY_Reset(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD);
IFX_return_t calcToken(IFX_uint32_t Rate, IFX_uint32_t *TokenR, IFX_uint32_t *TokenTimer);
IFX_uint16_t IFX_PSB6970_PHYID_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD);
IFX_return_t IFX_PSB6970_VLAN_Table_SW_Init ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_VLAN_Table_HW_Init ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_MFC_Init ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_MFC_SW_Init ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_MFC_HW_Init ( IFX_void_t *pDevCtx);
IFX_uint8_t IFX_PSB6970_MFC_Index_Find ( IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcMatchField_t *pPar );
IFX_uint8_t IFX_PSB6970_MFC_Entry_Avariable ( IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcMatchField_t *pPar );
IFX_return_t IFX_PSB6970_BackupAllRegisters ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_RestoreAllRegisters ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_PHY_forcedDuplex(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portDuplex_t eDuplex);
IFX_return_t IFX_PSB6970_PHY_forcedSpeed(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portSpeed_t eSpeed);
IFX_return_t IFX_PSB6970_PHY_AutoNegotiation(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_enDis_t eEnable );
IFX_return_t IFX_PSB6970_GPHY_forcedSpeed(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portSpeed_t eSpeed);
IFX_return_t IFX_PSB6970_PHY_FlowControl(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portFlow_t eFlow );

typedef struct {
   IFX_char_t     *cDesc;
   IFX_uint32_t   nCap;
}SWAPI_API_CAPABILITY_GET_t;

SWAPI_API_CAPABILITY_GET_t Tantos3G_Cap[] =
{
  {"Physical Ethernet ports", 7},
  {"Virtual Ethernet ports", 0},
  {"Internal packet memory", 80*1024},
  {"Priority queues per device", 4},
  {"Meter instances", 0},
  {"Rate shaper instances", 0},
  {"VLAN groups", 16},
  {"Forwarding database IDs", 4},
  {"MAC table entries", 2*1024},
  {"Multicast level 3 hardware table entries", 32},
  {"PPPoE sessions ", 1}
};

SWAPI_API_CAPABILITY_GET_t AR9_Cap[] =
{
  {"Physical Ethernet ports", 3},
  {"Virtual Ethernet ports", 6},
  {"Internal packet memory", 32*1024},
  {"Priority queues per device", 4},
  {"Meter instances", 0},
  {"Rate shaper instances", 0},
  {"VLAN groups", 16},
  {"Forwarding database IDs", 4},
  {"MAC table entries", 512},
  {"Multicast level 3 hardware table entries", 0},
  {"PPPoE sessions ", 0}
};

typedef struct {
   /* * Name or ID of the version information. */
   IFX_char_t   *cName;
   /* * Version string information. */
   IFX_char_t   *cVersion;
}SWAPI_API_VERSION_GET_t;

SWAPI_API_VERSION_GET_t ver[] =
{
  {"Switch API Version", SWITCH_API_DRIVER_VERSION }
};

/****************/
/* IGMP Section */
/****************/

#define MULTICAST_TABLE_ENTRY_MAX 64
#define IFX_ETHSW_TANTOS3G_IGMPTC5_ADDR 0x115

typedef struct
{
       IFX_uint8_t                                 nPortId;
       IFX_ETHSW_IP_Select_t                       eIPVersion;
       IFX_ETHSW_IP_t                              uIP_Gda;
       IFX_ETHSW_IP_t                              uIP_Gsa;
       IFX_ETHSW_IGMP_MemberMode_t                 eModeMember;
}IFX_ETHSW_multicastTableEntry_t;

typedef enum
{
   IFX_ETHSW_IGMP_GID_GET                    = 0,
   IFX_ETHSW_IGMP_SIP_GET                    = 1,
   IFX_ETHSW_IGMP_SET                        = 2
}IFX_ETHSW_IGMP_ACCESS_TYPE_t;

typedef struct
{
   IFX_uint8_t                                 nPortId; // 0,1,2...
   IFX_ETHSW_IGMP_MemberMode_t                 eModeMember;
   IFX_ETHSW_IGMP_ACCESS_TYPE_t                eAccessType;
   IFX_ETHSW_IP_Select_t                       eIPVersion;

   IFX_uint16_t                                uCmd;
   IFX_uint8_t                                 nS3PMI;
   IFX_uint8_t                                 nS3PMV;
   IFX_ETHSW_IP_t                              uIP_Gda;
   IFX_ETHSW_IP_t                              uIP_Gsa;
   IFX_uint32_t                                nGDA; // handle GDA of IPv4 to add/substract 0xE0000000
}IFX_ETHSW_multicastTableRegisterAccess;

typedef enum
{
    IFX_ETHSW_IGMP_COMMAND_OK                       = 0,
    IFX_ETHSW_IGMP_COMMAND_GROUP_TABLE_IS_FULL      = 1,
    IFX_ETHSW_IGMP_COMMAND_GROUP_NOT_FOUND          = 2,
    IFX_ETHSW_IGMP_COMMAND_SOURCE_TABLE_IS_FULL     = 3,
    IFX_ETHSW_IGMP_COMMAND_SOURCE_NOT_FOUND         = 4,
    IFX_ETHSW_IGMP_COMMAND_ERROR                    = 5
}IFX_ETHSW_IGMP_commandResult_t;

IFX_ETHSW_multicastTableEntry_t gMulticastTable[MULTICAST_TABLE_ENTRY_MAX];
IFX_uint32_t gMulticastTableEntryNumber = 0;
IFX_uint32_t gMulticastTableEntryCount = 0;

// function portotype
IFX_return_t IFX_PSB6970_IGMPV3_Enable(IFX_void_t *pDevCtx, IFX_uint32_t ipv4);
IFX_return_t IFX_PSB6970_WriteIPv4_GDA(IFX_void_t *pDevCtx, IFX_uint32_t ipv4);
IFX_return_t IFX_PSB6970_WriteGDA(IFX_void_t *pDevCtx, IFX_uint32_t ipv4);
IFX_return_t IFX_PSB6970_WriteIPv6_GDA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6);
IFX_return_t IFX_PSB6970_ReadIPv4_GDA(IFX_void_t *pDevCtx, IFX_uint32_t *ipv4);
IFX_return_t IFX_PSB6970_ReadGDA(IFX_void_t *pDevCtx, IFX_uint32_t *ipv4);
IFX_return_t IFX_PSB6970_ReadIPv6_GDA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6);
IFX_return_t IFX_PSB6970_WriteIPv4_GSA(IFX_void_t *pDevCtx, IFX_uint32_t ipv4);
IFX_return_t IFX_PSB6970_WriteIPv6_GSA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6);
IFX_return_t IFX_PSB6970_ReadIPv4_GSA(IFX_void_t *pDevCtx, IFX_uint32_t *ipv4);
IFX_return_t IFX_PSB6970_ReadIPv6_GSA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6);
IFX_ETHSW_IGMP_commandResult_t IFX_PSB6970_MulticastTableAccessCommand(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_Create_GroupTable(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_Create_GroupAndSourceListTable(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_GetMemberMode(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_GetFirstGIDEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_GetNextGIDEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_GetFirstSIPEntryByPort(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_GetNextSIPEntryByPort(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_AddSingleSIPEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_RemoveSingleSIPEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_RemoveMultipleSIPEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_RemoveIGMP_HWTable(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_CreateSIPEntryByGID(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg);
IFX_return_t IFX_PSB6970_CopyIPV6(IFX_void_t *pdest, IFX_void_t *psrc);
IFX_return_t IFX_PSB6970_CopyIPV6GDA(IFX_void_t *pdest, IFX_uint32_t nGDA);
IFX_return_t IFX_PSB6970_CopyGDAIPV6(IFX_uint32_t *nGDA, IFX_void_t *pdest);
IFX_return_t IFX_PSB6970_ResetIPV6(IFX_void_t *pIPv6);
IFX_return_t IFX_PSB6970_IGMP_Init(IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_CheckIP_VersionByGDA(IFX_void_t *pDevCtx, IFX_ETHSW_IP_Select_t *ipversion);
IFX_return_t IFX_PSB6970_CheckIP_VersionByGSA(IFX_void_t *pDevCtx, IFX_ETHSW_IP_Select_t *ipversion);


/*****************/
/* Function Body */
/*****************/

IFX_return_t IFX_PSB6970_MAC_TableClear(IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_MAC_tableRead_t foundEntry;
    IFX_ETHSW_MAC_tableRemove_t removeEntry;
    IFX_uint8_t i, j;

    for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
    {
        foundEntry.bInitial = IFX_TRUE;
        foundEntry.nPortId = i;
        do {
            IFX_PSB6970_MAC_TableEntryRead(pEthSWDevHandle, &foundEntry);
            if (foundEntry.bLast == IFX_FALSE)
            {
                removeEntry.nFId = foundEntry.nFId;
                for (j=0; j<6; j++)
                    removeEntry.nMAC[j] = foundEntry.nMAC[j];
                IFX_PSB6970_MAC_TableEntryRemove(pEthSWDevHandle, &removeEntry);
            }
        } while (foundEntry.bLast == IFX_FALSE);
    }
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MAC_TableEntryAdd(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableAdd_t *pPar)
{
    IFX_uint32_t value;
    IFX_uint32_t nCommandAndAccess;
    IFX_uint16_t portmap;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint16_t nTimeOutCnt;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);

    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    /* write nFId to ATC3[107].FID[1:0] bits */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_FID, 0, pPar->nFId) != IFX_SUCCESS)
        return IFX_ERROR;

    /* write nPortId to ATC3[107].FID[10:4] bits */
    portmap = 0;
    portmap |= 1 << portIdx;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_PMAP, 0, portmap) != IFX_SUCCESS)
        return IFX_ERROR;

    if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
    {
        if (pPar->bStaticEntry == IFX_TRUE)
        {
            nCommandAndAccess = 1 << AR9_ADR_TB_CTL2_REG_INFOT_SHIFT;
        }
        else
        {
            nCommandAndAccess = pPar->nAgeTimer;
        }
        value = pPar->nMAC[2] << 24 |  pPar->nMAC[3] << 16 | pPar->nMAC[4] << 8 | pPar->nMAC[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR31_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        value = pPar->nMAC[0] << 8 | pPar->nMAC[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR47_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        nCommandAndAccess |= IFX_PSB6970_MACTABLE_ENTRY_CREATE<<16;

        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_ADR_TB_CTL2_REG_CMD_OFFSET, nCommandAndAccess);
        IFX_ETHSW_DEBUG_PRINT(  "MAC=%x:%x:%x:%x:%x:%x\n\n",
            pPar->nMAC[0], pPar->nMAC[1], pPar->nMAC[2],
            pPar->nMAC[3], pPar->nMAC[4], pPar->nMAC[5]);
    }
    else
    {
        if (pPar->bStaticEntry == IFX_TRUE)
        {
            /* set bStaticEntry to ATC4[108].INFOT[12] bit */
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_INFOT, 0, IFX_TRUE) != IFX_SUCCESS)
                return IFX_ERROR;
            /* write nAgeTimer to ATC4[108].ITAT[10:0] bits */
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ITAT, 0, 0) != IFX_SUCCESS)
                return IFX_ERROR;
        }
        else
        {
            /* clear bStaticEntry to ATC4[108].INFOT[12] bit */
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_INFOT, 0, IFX_FALSE) != IFX_SUCCESS)
                return IFX_ERROR;
            /* write nAgeTimer to ATC4[108].ITAT[10:0] bits */
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ITAT, 0, pPar->nAgeTimer) != IFX_SUCCESS)
                return IFX_ERROR;
        }

        /* write nMAC[1],nMAC[0] to ATC0[104].ADDR15_0[15:0] bits */
        value = pPar->nMAC[4] << 8 | pPar->nMAC[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        /* write nMAC[3],nMAC[2] to ATC1[105].ADDR31_16[15:0] bits */
        value = pPar->nMAC[2] << 8 | pPar->nMAC[3];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        /* write nMAC[5],nMAC[4] to ATC2[106].ADDR47_32[15:0] bits */
        value = pPar->nMAC[0] << 8 | pPar->nMAC[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR47_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        /* write IFX_PSB6970_MACTABLE_ENTRY_CREATE to ATC5[109] bits */
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_ATC5_CMD_OFFSET, IFX_PSB6970_MACTABLE_ENTRY_CREATE);
    }

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    /*  Read the ATS5[10F] to check the command result */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_RSLT, 0, &value) != IFX_SUCCESS)
       return IFX_ERROR;

    if ( value == IFX_PSB6970_MAC_COMMAND_OK )
        return IFX_SUCCESS;
    else
    {
        IFXOS_PRINT_INT_RAW(  "IFX_ERROR\n");
        return IFX_ERROR;
    }
}

IFX_return_t IFX_PSB6970_MAC_TableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRead_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint32_t value = 0;
    IFX_uint16_t portmap=0;
    IFX_PSB6970_MAC_commandResult_t result;
    IFX_uint16_t nTimeOutCnt;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    nTimeOutCnt = 0;
    do {
       /*  Poll the BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
    {
        if ( pPar->bInitial== IFX_TRUE )
        {
            IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_ADR_TB_CTL2_REG_CMD_OFFSET, IFX_PSB6970_MACTABLE_INIT_TO_FIRST<<16 );
        }
    }
    else
    {
        if ( pPar->bInitial== IFX_TRUE )
        {
            IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_ATC5_CMD_OFFSET, IFX_PSB6970_MACTABLE_INIT_TO_FIRST );
        }
    }

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    /* write nPortId to ATC3[107].FID[10:4] bits */
    portmap = 0;
    portmap |= 1 << portIdx;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_PMAP, 0, portmap) != IFX_SUCCESS)
        return IFX_ERROR;

    if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
    {
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_ADR_TB_CTL2_REG_CMD_OFFSET, (IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_PORT<<16) );
    }
    else
    {
        /* write IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_PORT to ATC5[109] bits */
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_ATC5_CMD_OFFSET, IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_PORT );
    }


    nTimeOutCnt = 0;
    do {
       /*  Poll the BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    /*  Read the ATS5[10F] to check the command result */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_RSLT, 0, &value) != IFX_SUCCESS)
       return IFX_ERROR;

    result = value;

    if ( result == IFX_PSB6970_MAC_COMMAND_OK )
    {
        /* read nFId from ATS3[107].FID[1:0] bits */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_FIDS, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
        pPar->nFId = value;

        /* read nAgeTimer from ATS4[108].ITAT[10:0] bits */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ITATS, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
        pPar->nAgeTimer = value;

        /* read bStaticEntry from ATS4[108].INFOT[12] bit */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_INFOTS, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->bStaticEntry = value;

        if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
        {
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDRS31_0, 0, &value) != IFX_SUCCESS)
                    return IFX_ERROR;
                pPar->nMAC[5] = value & 0xFF;
                pPar->nMAC[4] = (value >> 8) & 0xFF;
                pPar->nMAC[3] = (value >> 16) & 0xFF;
                pPar->nMAC[2] = (value >> 24) & 0xFF;
                /* read nMAC[5],nMAC[4] from ATS2[106].ADDRS47_32[15:0] bits */
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDRS47_32, 0, &value) != IFX_SUCCESS)
                    return IFX_ERROR;
                pPar->nMAC[1] = value & 0xFF;
                pPar->nMAC[0] = value >> 8;

        }
        else
        {
                /* read nMAC[1],nMAC[0] from ATS0[104].ADDRS15_0[15:0] bits */
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDRS15_0, 0, &value) != IFX_SUCCESS)
                    return IFX_ERROR;
                pPar->nMAC[5] = value & 0xFF;
                pPar->nMAC[4] = value >> 8;
                /* read nMAC[3],nMAC[2] from ATS1[105].ADDRS31_16[15:0] bits */
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDRS31_16, 0, &value) != IFX_SUCCESS)
                    return IFX_ERROR;
                pPar->nMAC[3] = value & 0xFF;
                pPar->nMAC[2] = value >> 8;
                /* read nMAC[5],nMAC[4] from ATS2[106].ADDRS47_32[15:0] bits */
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDRS47_32, 0, &value) != IFX_SUCCESS)
                    return IFX_ERROR;
                pPar->nMAC[1] = value & 0xFF;
                pPar->nMAC[0] = value >> 8;
        }

        pPar->bLast = IFX_FALSE;

    }

    if ( result == IFX_PSB6970_MAC_COMMAND_ENTRY_NOT_FOUND )
    {
        pPar->bLast = IFX_TRUE;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MAC_TableEntryQuery(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableQuery_t *pPar)
{
	IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value = 0;
    IFX_uint16_t nTimeOutCnt;
	pPar->bFound = IFX_FALSE;
    nTimeOutCnt = 0;
    do {
       /*  Poll the BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
    {
    	IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_ADR_TB_CTL2_REG_CMD_OFFSET, IFX_PSB6970_MACTABLE_INIT_TO_FIRST<<16 );
    }
    else
    {
    	IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_ATC5_CMD_OFFSET, IFX_PSB6970_MACTABLE_INIT_TO_FIRST );
    }

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

	value = pPar->nFId;
   /* write nFId from ATS3[107].FID[1:0] bits */
   if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_FIDS, 0, value) != IFX_SUCCESS)
   		return IFX_ERROR;

    if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
    {
        value = pPar->nMAC[2] << 24 |  pPar->nMAC[3] << 16 | pPar->nMAC[4] << 8 | pPar->nMAC[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR31_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        value = pPar->nMAC[0] << 8 | pPar->nMAC[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR47_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_ADR_TB_CTL2_REG_CMD_OFFSET, IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_AND_FID);
    }
    else
    {
        /* write nMAC[1],nMAC[0] to ATC0[104].ADDR15_0[15:0] bits */
        value = pPar->nMAC[4] << 8 | pPar->nMAC[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        /* write nMAC[3],nMAC[2] to ATC1[105].ADDR31_16[15:0] bits */
        value = pPar->nMAC[2] << 8 | pPar->nMAC[3];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        /* write nMAC[5],nMAC[4] to ATC2[106].ADDR47_32[15:0] bits */
        value = pPar->nMAC[0] << 8 | pPar->nMAC[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR47_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        /* write IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_AND_FID to ATC5[109] bits */
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_ATC5_CMD_OFFSET, IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_AND_FID);
    }

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    /*  Read the ATS5[10F] to check the command result */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_RSLT, 0, &value) != IFX_SUCCESS)
       return IFX_ERROR;

    if ( value == IFX_PSB6970_MAC_COMMAND_OK )
    {
        /* read nFId from ATS3[107].FID[1:0] bits */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_FIDS, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
        pPar->nFId = value;

        /* read nAgeTimer from ATS4[108].ITAT[10:0] bits */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ITATS, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
        pPar->nAgeTimer = value;

        /* read bStaticEntry from ATS4[108].INFOT[12] bit */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_INFOTS, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->bFound = IFX_TRUE;  
    } 
     return IFX_SUCCESS;
}
IFX_return_t IFX_PSB6970_MAC_TableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRemove_t *pPar)
{
    IFX_uint32_t value;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint16_t nTimeOutCnt;

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MAC_TABLE_BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    /* write nFId to ATC3[107].FID[1:0] bits */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_FID, 0, pPar->nFId) != IFX_SUCCESS)
        return IFX_ERROR;

    if (pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 )
    {
        value = pPar->nMAC[2] << 24 |  pPar->nMAC[3] <<16 | pPar->nMAC[4] << 8 | pPar->nMAC[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR31_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        value = pPar->nMAC[0] << 8 | pPar->nMAC[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR47_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_ADR_TB_CTL2_REG_CMD_OFFSET, (IFX_PSB6970_MACTABLE_ENTRY_ERASE<<16) );
    }
    else
    {
        /* write nMAC[1],nMAC[0] to ATC0[104].ADDR15_0[15:0] bits */
        value = pPar->nMAC[4] << 8 | pPar->nMAC[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        /* write nMAC[3],nMAC[2] to ATC1[105].ADDR31_16[15:0] bits */
        value = pPar->nMAC[2] << 8 | pPar->nMAC[3];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        /* write nMAC[5],nMAC[4] to ATC2[106].ADDR47_32[15:0] bits */
        value = pPar->nMAC[0] << 8 | pPar->nMAC[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MAC_TABLE_ADDR47_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        /* write IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_AND_FID to ATC5[109] bits */
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_ATC5_CMD_OFFSET, IFX_PSB6970_MACTABLE_ENTRY_ERASE );
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;
    //IFX_ETHSW_regMapperSelector_t sel;

    portIdx = pPar->nPortId;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // read PxBC[15:14] register to eEnable
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_SPS, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

#if 0
    if ( value == 1 ) // Disabled/Discarding State.
         pPar->eEnable = IFX_ETHSW_PORT_DISABLE;
    else
         pPar->eEnable = IFX_ETHSW_PORT_ENABLE;
#else
    if ( value == 1 ) // Disabled/Discarding State.
         pPar->eEnable = IFX_ETHSW_PORT_DISABLE;
    else
         pPar->eEnable = IFX_ETHSW_PORT_ENABLE_RXTX;
#endif

    // read UPMBPM[14:8] register to bUnicastUnknownDrop
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_UP, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    value = (value >> pPar->nPortId) & 0x00000001;

    if ( value == IFX_TRUE )
         pPar->bUnicastUnknownDrop = IFX_FALSE;
    else
         pPar->bUnicastUnknownDrop = IFX_TRUE;

    // read MPMRPM[14:8] register to bMulticastUnknownDrop
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_MP, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    value = (value >> pPar->nPortId) & 0x00000001;
    if ( value == IFX_TRUE )
         pPar->bMulticastUnknownDrop = IFX_FALSE;
    else
         pPar->bMulticastUnknownDrop = IFX_TRUE;

    // read MPMRPM[6:0] register to bReservedPacketDrop
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RP, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    value = (value >> pPar->nPortId) & 0x00000001;
    if ( value == IFX_TRUE )
         pPar->bReservedPacketDrop = IFX_FALSE;
    else
         pPar->bReservedPacketDrop = IFX_TRUE;

    // read UPMBPM[6:0] register to bBroadcastDrop
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_BP, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    value = (value >> pPar->nPortId) & 0x00000001;
    if ( value == IFX_TRUE )
         pPar->bBroadcastDrop = IFX_FALSE;
    else
         pPar->bBroadcastDrop = IFX_TRUE;

    // read PxEC[15] register to bAging
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_AD, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    if ( value == IFX_TRUE )
         pPar->bAging = IFX_FALSE;
    else
         pPar->bAging = IFX_TRUE;

    // Not Supported on AR9/Tantos3G platform
    //pPar->bLearningLimitAction = XXX;

    // read bLearningMAC_PortLock from PxEC[02].LD[14] register
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_LD, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    if ( value == IFX_TRUE )
         pPar->bLearningMAC_PortLock = IFX_TRUE;
    else
         pPar->bLearningMAC_PortLock = IFX_FALSE;

    // read nLearningLimit from P0EC[02].MNA024[12:8] register
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_MNA24, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->nLearningLimit = value;

    // read ePortMonitor from P0EC[02].IPMO[5:4] register
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_IPMO, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->ePortMonitor = value;

    // read flow control
    pPar->eFlowCtrl = pEthSWDevHandle->PortConfig[portIdx].eFlow;


    return IFX_SUCCESS;

}

IFX_return_t IFX_PSB6970_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;
    IFX_uint16_t                       portmap;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_ETHSW_regMapperSelector_t sel=COMMON_BIT_LATEST;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->eFlowCtrl != IFX_ETHSW_FLOW_OFF && pPar->eFlowCtrl != IFX_ETHSW_FLOW_AUTO )
    {
        IFXOS_PRINT_INT_RAW("Flow Control parameter error\n");
        return IFX_ERROR;
    }

#if 0
    if(pPar->eEnable != IFX_ETHSW_PORT_DISABLE && pPar->eEnable != IFX_ETHSW_PORT_ENABLE )
    {
        IFXOS_PRINT_INT_RAW(  "Enable parameter error! \n");
        return IFX_ERROR;
    }
#endif

    if(pPar->nLearningLimit >= 32 )
    {
        IFXOS_PRINT_INT_RAW(  "Learning Linit parameter error! \n");
        return IFX_ERROR;
    }

    // write eEnable to PxBC[15:14] register
#if 0
    if ( pPar->eEnable == IFX_ETHSW_PORT_DISABLE )
#else
    if ( pPar->eEnable == IFX_ETHSW_PORT_DISABLE )
#endif
    {
        // 01B      Disabled/Discarding State.
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_SPS, portIdx, 1)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
#if 0
    else if ( pPar->eEnable == IFX_ETHSW_PORT_ENABLE )
#else
    else if ( pPar->eEnable == IFX_ETHSW_PORT_ENABLE_RXTX )
#endif
    {
        // 00B      Forwarding State.
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_SPS, portIdx, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        // IFX_ETHSW_PORT_ENABLE_RX: // not support
        // IFX_ETHSW_PORT_ENABLE_TX: // not support
        IFXOS_PRINT_INT_RAW(  "eEnable error\n");
    }

    // write bUnicastUnknownDrop to UPMBPM[14:8] register
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_UP, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    portmap = value;
    switch ( pPar->bUnicastUnknownDrop ) {
        case IFX_TRUE:
             portmap &= ~(1 << pPar->nPortId);
            break;
        case IFX_FALSE:
            portmap |= 1 << pPar->nPortId;
            break;
        default:
             IFXOS_PRINT_INT_RAW(  "bUnicastUnknownDrop error\n");
            break;
    }               /* -----  end switch  ----- */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_UP, 0, portmap)!= IFX_SUCCESS)
        return IFX_ERROR;

    // write bMulticastUnknownDrop to MPMRPM[14:8] register
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_MP, 0, &value);
    portmap = value;
    switch ( pPar->bMulticastUnknownDrop ) {
        case IFX_TRUE:
             portmap &= ~(1 << pPar->nPortId);
            break;
        case IFX_FALSE:
            portmap |= 1 << pPar->nPortId;
            break;
        default:
             IFXOS_PRINT_INT_RAW(  "bMulticastUnknownDrop error\n");
            break;
    }               /* -----  end switch  ----- */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_MP, 0, portmap)!= IFX_SUCCESS)
        return IFX_ERROR;

    // write bReservedPacketDrop to MPMRPM[6:0] register
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RP, 0, &value);
    portmap = value;
    if ( pPar->bReservedPacketDrop == IFX_TRUE )
    {
        portmap &= ~(1 << pPar->nPortId);
    }
    else
    {
        portmap |= 1 << pPar->nPortId;
    }
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RP, 0, portmap)!= IFX_SUCCESS)
        return IFX_ERROR;

    // write bBroadcastDrop to UPMBPM[6:0] register
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_BP, 0, &value);
    portmap = value;
    if ( pPar->bBroadcastDrop == IFX_TRUE )
    {
        portmap &= ~(1 << pPar->nPortId);
    }
    else
    {
        portmap |= 1 << pPar->nPortId;
    }
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_BP, 0, portmap)!= IFX_SUCCESS)
        return IFX_ERROR;

    // write bAging to PxEC[15] register
    value = !pPar->bAging;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_AD, portIdx, value) != IFX_SUCCESS)
        return IFX_ERROR;

    // Not Supported on AR9/Tantos3G platform
    // write bLearningLimitAction to XXX[XXX] register

    // write bLearningMAC_PortLock to PxEC[02].LD[14] register
    value = pPar->bLearningMAC_PortLock;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_LD, portIdx, value) != IFX_SUCCESS)
        return IFX_ERROR;

    // write nLearningLimit to P0EC[02].MNA024[12:8] register
    value = pPar->nLearningLimit;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_MNA24, portIdx, value) != IFX_SUCCESS)
        return IFX_ERROR;

    // write ePortMonitor to IPMO register
    value = pPar->ePortMonitor;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IPMO, portIdx, value) != IFX_SUCCESS)
        return IFX_ERROR;

    // write flow control
    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
    {
        if ( portIdx==0 || portIdx==1 )
        {
            if ( pPar->eFlowCtrl == IFX_ETHSW_FLOW_OFF )
            {
                if (portIdx==0)
                    sel = PORT_RGMII_GMII_P0FCE;
                else if (portIdx==1)
                    sel = PORT_RGMII_GMII_P1FCE;

                // disable flow control
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, sel, 0, IFX_DISABLE) != IFX_SUCCESS)
                    return IFX_ERROR;
            }
            else if ( pPar->eFlowCtrl == IFX_ETHSW_FLOW_AUTO )
            {
                if (portIdx==0)
                    sel = PORT_RGMII_GMII_P0FCE;
                else if (portIdx==1)
                    sel = PORT_RGMII_GMII_P1FCE;

                // enable flow control
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, sel, 0, IFX_ENABLE) != IFX_SUCCESS)
                    return IFX_ERROR;
            }
        }

        if ( portIdx == 1 ) // GPHY
        {
            IFX_PSB6970_PHY_FlowControl(pEthSWDevHandle, portIdx, pPar->eFlowCtrl);
        }
        pEthSWDevHandle->PortConfig[portIdx].eFlow = pPar->eFlowCtrl;
    }
    else
    {   // for Tantos3G RGMII/GMII Port Control Register
        if ( portIdx==4 || portIdx==5 || portIdx==6 )
        {
            if ( pPar->eFlowCtrl == IFX_ETHSW_FLOW_OFF )
            {
                if (portIdx==4)
                    sel = PORT_RGMII_GMII_P4FCE;
                else if (portIdx==5)
                    sel = PORT_RGMII_GMII_P5FCE;
                else if (portIdx==6)
                    sel = PORT_RGMII_GMII_P6FCE;

                // disable flow control
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, sel, 0, IFX_DISABLE) != IFX_SUCCESS)
                    return IFX_ERROR;
            }
            else if ( pPar->eFlowCtrl == IFX_ETHSW_FLOW_AUTO )
            {
                if (portIdx==4)
                    sel = PORT_RGMII_GMII_P4FCE;
                else if (portIdx==5)
                    sel = PORT_RGMII_GMII_P5FCE;
                else if (portIdx==6)
                    sel = PORT_RGMII_GMII_P6FCE;
                // enable flow control
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, sel, 0, IFX_ENABLE) != IFX_SUCCESS)
                    return IFX_ERROR;
            }
        }

  /*      // for Tantos3G phy
#if defined(AR9)
        if ( portIdx<=2 || portIdx==4 )
#elif defined(DANUBE)
        if ( portIdx <= 4 ) // internal PHY
#elif defined(AMAZON_SE)
        if ( portIdx <= 3 ) // internal PHY
#endif
*/
        {
            IFX_PSB6970_PHY_FlowControl(pEthSWDevHandle, portIdx, pPar->eFlowCtrl);
        }
        // save eFlowCtrl
        pEthSWDevHandle->PortConfig[portIdx].eFlow = pPar->eFlowCtrl;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_STP_BPDU_RuleGet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_BPDU_Rule_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t valid=0, span=0, act=0;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RA00_VALID, 0, &valid)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RA00_SPAN, 0, &span)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, &act)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( valid == 1 && span == 1)
    {
        if ( act == 0)
            pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DEFAULT;
        else if ( act == 1)
            pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DISCARD ;
        else if ( act == 2)
            pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_CPU ;
        else
            IFXOS_PRINT_INT_RAW(  "Not Supported!\n");

    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported!\n");
        return IFX_ERROR;
    }

#if 0
    if ( valid == 1 && span == 1 && act == 0)
        pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DEFAULT;
    else if ( valid == 1 && span == 1 && act == 1)
        pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DISCARD ;
    else if ( valid == 1 && span == 1 && act == 2)
        pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_CPU ;
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported!\n");
        return IFX_ERROR;
    }
#endif

    // Not Supported nForwardPortId !
    pPar->nForwardPortId = 0;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_STP_BPDU_RuleSet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_BPDU_Rule_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_VALID, 0, 1)!= IFX_SUCCESS)
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_SPAN, 0, 1)!= IFX_SUCCESS)
        return IFX_ERROR;

    if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DEFAULT )
    {
        // regard as normal packet
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DISCARD )
    {
        // discard span packet
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_CPU )
    {
        // forward to cpu port
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, 2)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else // IFX_ETHSW_PORT_FORWARD_PORT
    {
        IFXOS_PRINT_INT_RAW(  "Out of range !\n");
        return IFX_ERROR;
    }
    // Not Supported nForwardPortId !
#if 0
    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DEFAULT )
    {
        // regard as normal packet
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_VALID, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_SPAN, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DISCARD )
    {
        // discard span packet
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_VALID, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_SPAN, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_CPU )
    {
        // forward to cpu port
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_VALID, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_SPAN, 0, 1)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA00_ACT, 0, 2)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else // IFX_ETHSW_PORT_FORWARD_PORT
    {
        IFXOS_PRINT_INT_RAW(  "Out of range !\n");
        return IFX_ERROR;
    }
#endif


    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_STP_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_SPS, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    pPar->ePortState = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_STP_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    value = pPar->ePortState;
    if ( value > IFX_ETHSW_STP_PORT_STATE_BLOCKING )
    {
        IFXOS_PRINT_INT_RAW(  "Out of range !\n");
        return IFX_ERROR;
    }


    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_SPS, portIdx, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_IdCreate(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdCreate_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    //IFX_PSB6970_VLAN_Filter_registerOperation_t data;

    IFX_int_t table_index;
    IFX_PSB6970_VLAN_tableEntry_t VLAN_Table_Entry;

    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

    // check if vid exist ?
    if (IFX_PSB6970_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId) != IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE )
    {
        IFXOS_PRINT_INT_RAW(  "This vid exists\n");
        return IFX_ERROR;
    }

    // get avariable vlan entry
    table_index = IFX_PSB6970_VLAN_Table_Entry_Avariable(pEthSWDevHandle);
    if ( table_index == IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE ) {
        IFXOS_PRINT_INT_RAW(  "There is no table entry avariable\n");
        return IFX_ERROR;
    }


    // write to mirrored table
    VLAN_Table_Entry.valid = IFX_TRUE;
    VLAN_Table_Entry.vid = pPar->nVId;
    VLAN_Table_Entry.fid = pPar->nFId;
    VLAN_Table_Entry.pm = 0;
    VLAN_Table_Entry.tm = 0;
    IFX_PSB6970_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);

    //data.nTable_Index = table_index;
    //data.nVId = pPar->nVId;
    //data.nFId = pPar->nFId;
    //data.nOP  = IFX_PSB6970_VLAN_TABLE_ENTRY_ADD;
    // VLAN_Filter_Register_Write(pDevCtx, &data);

    // write to VV bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, table_index, IFX_TRUE) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to VID bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VID, table_index, pPar->nVId) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to FID bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VFID, table_index, pPar->nFId) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to TM bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to M bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_IdDelete(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdDelete_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_int_t table_index;
    IFX_PSB6970_VLAN_tableEntry_t VLAN_Table_Entry;
    IFX_uint32_t value;
    IFX_uint8_t i;

    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

    table_index = IFX_PSB6970_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);

    if (table_index == IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE)
    {
        IFXOS_PRINT_INT_RAW(  "This vid doesn't exists\n");
        return IFX_ERROR;
    }

    IFX_PSB6970_VLAN_Table_Entry_Get ( pEthSWDevHandle, table_index, &VLAN_Table_Entry );
    if ( VLAN_Table_Entry.pm != 0 )
    {
        IFXOS_PRINT_INT_RAW(  "Please remove port member first! \n");
        return IFX_ERROR;
    }

    for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
    {
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, 0, &value);
        if ( value == pPar->nVId )
        {
            IFXOS_PRINT_INT_RAW(  "Port[%d] use the VLAN group as PVID ! \n", i);
            return IFX_ERROR;
        }
    }

    VLAN_Table_Entry.valid = IFX_FALSE;
    VLAN_Table_Entry.vid = 0;
    VLAN_Table_Entry.fid = 0;
    VLAN_Table_Entry.pm = 0;
    VLAN_Table_Entry.tm = 0;
    IFX_PSB6970_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);

    // write to VV bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to VID bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VID, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to FID bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VFID, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to M bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to TM bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, table_index, 0) != IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_IdGet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdGet_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_int_t table_index;
    IFX_PSB6970_VLAN_tableEntry_t VLAN_Table_Entry;

    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

if (g_debug == 1) // xxx
    if (pPar->nVId == 99)
    {
        IFX_PSB6970_PortConfig_Print ( pEthSWDevHandle );
        IFX_PSB6970_VLAN_Table_Print ( pEthSWDevHandle );
        return IFX_SUCCESS;
    }

    table_index = IFX_PSB6970_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);

    if (table_index == IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE)
    {
        IFXOS_PRINT_INT_RAW(  "This vid doesn't exists\n");
        return IFX_ERROR;
    }

    IFX_PSB6970_VLAN_Table_Entry_Get ( pEthSWDevHandle, table_index, &VLAN_Table_Entry );

#ifdef DEBUG_VLAN
        IFXOS_PRINT_INT_RAW(  "          valid = 0x%x\n", pEthSWDevHandle->VLAN_Table[table_index].valid);
        IFXOS_PRINT_INT_RAW(  "            vid = 0x%x\n", pEthSWDevHandle->VLAN_Table[table_index].vid);
        IFXOS_PRINT_INT_RAW(  "            fid = 0x%x\n", pEthSWDevHandle->VLAN_Table[table_index].fid);
        IFXOS_PRINT_INT_RAW(  "            pm  = 0x%x\n", pEthSWDevHandle->VLAN_Table[table_index].pm);
        IFXOS_PRINT_INT_RAW(  "            tm  = 0x%x\n", pEthSWDevHandle->VLAN_Table[table_index].tm);
#endif

    pPar->nFId = VLAN_Table_Entry.fid;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portCfg_t *pPar)
{

    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint32_t value;

    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, portIdx, &value);
    pPar->nPortVId = value;

    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VC, portIdx, &value);
    pPar->bVLAN_UnknownDrop = value;

    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VSD, portIdx, &value);
    pPar->bVLAN_ReAssign = !value;

    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VMCE, portIdx, &value);
    if ( value == IFX_TRUE )
        pPar->eVLAN_MemberViolation = IFX_ETHSW_VLAN_MEMBER_VIOLATION_BOTH;
    else
        pPar->eVLAN_MemberViolation = IFX_ETHSW_VLAN_MEMBER_VIOLATION_EGRESS;

    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_AOVTP, portIdx, &value);
    if ( value == IFX_TRUE )
            pPar->eAdmitMode = IFX_ETHSW_VLAN_ADMIT_TAGGED;
    else
            pPar->eAdmitMode = IFX_ETHSW_VLAN_ADMIT_ALL;

    pPar->bTVM = pEthSWDevHandle->PortConfig[portIdx].bTVM;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_int_t table_index;
    IFX_PSB6970_VLAN_tableEntry_t VLAN_Table_Entry;

    // check condition
    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

    if ( pPar->bTVM == IFX_TRUE)
    {
        if (pPar->bVLAN_UnknownDrop == IFX_TRUE || pPar->bVLAN_ReAssign == IFX_TRUE || pPar->eAdmitMode == IFX_ETHSW_VLAN_ADMIT_TAGGED  )
        {
            IFXOS_PRINT_INT_RAW(  "Can't set TVM mode under current state\n");
            return IFX_ERROR;
        }
    }


    table_index = IFX_PSB6970_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nPortVId);
    if (table_index == IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE)
    {
        IFXOS_PRINT_INT_RAW(  "This vid doesn't exists\n");
        return IFX_ERROR;
    }

    IFX_PSB6970_VLAN_Table_Entry_Get ( pEthSWDevHandle, table_index, &VLAN_Table_Entry );

    // write PVID bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, portIdx, pPar->nPortVId) != IFX_SUCCESS)
        return IFX_ERROR;

    // write DFID bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_DFID, portIdx, VLAN_Table_Entry.fid) != IFX_SUCCESS)
        return IFX_ERROR;

    // write DVPM bits
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_DVPM, portIdx, VLAN_Table_Entry.pm) != IFX_SUCCESS)
        return IFX_ERROR;

    // write PVTAGMP bit
    if ((VLAN_Table_Entry.tm >> portIdx) & 1 )
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, portIdx, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, portIdx, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
    }


    // write VC bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VC, portIdx, pPar->bVLAN_UnknownDrop) != IFX_SUCCESS)
        return IFX_ERROR;

    // write VSD bit
    if ( pPar->bVLAN_ReAssign == IFX_TRUE )
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VSD, portIdx, 0) != IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VSD, portIdx, 1) != IFX_SUCCESS)
            return IFX_ERROR;

    }

    // write VMCE bit
    switch ( pPar->eVLAN_MemberViolation ) {
        case IFX_ETHSW_VLAN_MEMBER_VIOLATION_NO:
            IFXOS_PRINT_INT_RAW(  "Not Supported: IFX_ETHSW_VLAN_MEMBER_VIOLATION_NO\n");
            break;

        case IFX_ETHSW_VLAN_MEMBER_VIOLATION_INGRESS:
            IFXOS_PRINT_INT_RAW(  "Not Supported: IFX_ETHSW_VLAN_MEMBER_VIOLATION_INGRESS\n");
            break;

        case IFX_ETHSW_VLAN_MEMBER_VIOLATION_EGRESS:
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VMCE, portIdx, IFX_FALSE) != IFX_SUCCESS)
                return IFX_ERROR;
            break;

        case IFX_ETHSW_VLAN_MEMBER_VIOLATION_BOTH:
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VMCE, portIdx, IFX_TRUE) != IFX_SUCCESS)
                return IFX_ERROR;
            break;
        default:
            IFXOS_PRINT_INT_RAW(  "VLAN_MemberViolation Error!\n");
            break;
    }               /* -----  end switch  ----- */

    // write AOVTP bit
    switch ( pPar->eAdmitMode ) {
        case IFX_ETHSW_VLAN_ADMIT_ALL:
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_AOVTP, portIdx, IFX_FALSE) != IFX_SUCCESS)
                return IFX_ERROR;
            break;

        case IFX_ETHSW_VLAN_ADMIT_UNTAGGED:
            IFXOS_PRINT_INT_RAW(  "Not Supported: IFX_ETHSW_VLAN_ADMIT_UNTAGGED\n");
            return IFX_ERROR;

        case IFX_ETHSW_VLAN_ADMIT_TAGGED:
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_AOVTP, portIdx, IFX_TRUE) != IFX_SUCCESS)
                return IFX_ERROR;
            break;
        default:
            IFXOS_PRINT_INT_RAW(  "AdmitMode Error!\n");
            break;
    }               /* -----  end switch  ----- */

    // write IFNTE bit
#if 1
    if ( pPar->bTVM != pEthSWDevHandle->PortConfig[portIdx].bTVM)
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IFNTE, portIdx, pPar->bTVM) != IFX_SUCCESS)
            return IFX_ERROR;
        pEthSWDevHandle->PortConfig[portIdx].bTVM = pPar->bTVM;
    }
#else
    if ( pPar->bTVM == IFX_TRUE)
    {
        if ( pEthSWDevHandle->PortConfig[portIdx].bTVM == IFX_TRUE)
        {
            IFXOS_PRINT_INT_RAW(  "Transparent Mode has already enabled!\n");
        }
        else
        {
            pEthSWDevHandle->PortConfig[portIdx].bTVM = IFX_TRUE;
            // set PORT_IFNTE bit
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IFNTE, portIdx, IFX_TRUE) != IFX_SUCCESS)
                return IFX_ERROR;
        }

    }
    else
    {
        pEthSWDevHandle->PortConfig[portIdx].bTVM = IFX_FALSE;
        // clear PORT_IFNTE bit
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IFNTE, portIdx, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;

    }
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_PortMemberAdd(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberAdd_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_int_t table_index;
    IFX_PSB6970_VLAN_tableEntry_t VLAN_Table_Entry;
    IFX_uint32_t value;
    IFX_uint16_t                       portmap;
    IFX_uint16_t                       tagmap;
    IFX_uint8_t i, valid;

    // check
    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

    table_index = IFX_PSB6970_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);

    if (table_index == IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE)
    {
        IFXOS_PRINT_INT_RAW(  "This vid doesn't exists\n");
        return IFX_ERROR;
    }

    // write to mirror table
    IFX_PSB6970_VLAN_Table_Entry_Get(pEthSWDevHandle, table_index, &VLAN_Table_Entry);

    // write to port member
    VLAN_Table_Entry.pm |= 1 << portIdx;
    IFX_PSB6970_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);

    // write to tag member
    if (pPar->bVLAN_TagEgress == IFX_TRUE)
    {
        // VLAN_Table_Entry.tm |= 1 << portIdx;
        for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
        {
            if (pEthSWDevHandle->VLAN_Table[i].valid == IFX_TRUE)
            {
                pEthSWDevHandle->VLAN_Table[i].tm |= 1 << portIdx;
            }
        }
    }
    else
    {
        //VLAN_Table_Entry.tm &= ~(1 << portIdx);
        for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
        {
            if (pEthSWDevHandle->VLAN_Table[i].valid == IFX_TRUE)
            {
                pEthSWDevHandle->VLAN_Table[i].tm &= ~(1 << portIdx);
            }
        }
    }

    // write to M bits
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, table_index, &value);
    portmap = value;
    portmap |= 1 << portIdx;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, table_index, portmap) != IFX_SUCCESS)
        return IFX_ERROR;

#if 1
    // write to TM bits
    for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
    {
        // read VV bits
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, i, &value);
        valid = value;
        // write to TM bits
        if (valid == IFX_TRUE)
        {
            // read TM bits
            IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, i, &value);
            tagmap = value;

            if (pPar->bVLAN_TagEgress)
                tagmap |= 1 << portIdx;
            else
                tagmap &= ~(1 << portIdx);

            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, i, tagmap) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }
    // write PVTAGMP bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, portIdx, pPar->bVLAN_TagEgress) != IFX_SUCCESS)
        return IFX_ERROR;
#else
    if (pPar->bVLAN_TagEgress)
    {
        // write to TM bits
        for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
        {
            // read VV bits
            IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, i, &value);
            valid = value;
            // read TM bits
            IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, i, &value);
            tagmap = value;
            tagmap |= 1 << portIdx;
            // write to TM bits
            if (valid == IFX_TRUE)
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, i, tagmap) != IFX_SUCCESS)
                    return IFX_ERROR;
            }
        }
        // write PVTAGMP bit
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, portIdx, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        // write to TM bits
        for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
        {
            // read VV bits
            IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, i, &value);
            valid = value;
            // write to TM bits
            if (valid == IFX_TRUE)
            {
                // read TM bits
                IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, i, &value);
                tagmap = value;
                tagmap &= ~(1 << portIdx);
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, i, tagmap) != IFX_SUCCESS)
                    return IFX_ERROR;
            }
        }
        // write PVTAGMP bit
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, portIdx, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
    }
#endif

    for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
    {
        // read PVID per port
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, i, &value);
        if ( value == pPar->nVId )
        {
            // write DVPM bits on related port
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_DVPM, i, portmap) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }


    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_PortMemberRemove(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberRemove_t *pPar)
{
    IFX_PSB6970_VLAN_tableEntry_t VLAN_Table_Entry;
    IFX_int_t table_index;
    IFX_uint32_t value;
    IFX_uint16_t                       portmap;
    IFX_uint16_t                       tagmap;
    IFX_uint8_t                       i;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;

    if (pEthSWDevHandle->bVLAN_Aware != IFX_TRUE)
    {
        IFXOS_PRINT_INT_RAW(  "Please enable VLAN_Aware first\n");
        return IFX_ERROR;
    }

    table_index = IFX_PSB6970_VLAN_Table_Index_Find(pEthSWDevHandle, pPar->nVId);

    if (table_index == IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE)
    {
        IFXOS_PRINT_INT_RAW(  "This vid doesn't exists\n");
        return IFX_ERROR;
    }

    // remove pm in mirror table
    IFX_PSB6970_VLAN_Table_Entry_Get(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
    VLAN_Table_Entry.pm &= ~(1 << portIdx);
    // VLAN_Table_Entry.tm &= ~(1 << portIdx);
    IFX_PSB6970_VLAN_Table_Entry_Set(pEthSWDevHandle, table_index, &VLAN_Table_Entry);
    // remove tm in mirror table
        for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
        {
            if (pEthSWDevHandle->VLAN_Table[i].valid == IFX_TRUE)
            {
                pEthSWDevHandle->VLAN_Table[i].tm &= ~(1 << portIdx);
            }
        }

    // write to M bits
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, table_index, &value);
    portmap = value;
    portmap &= ~(1 << portIdx);
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, table_index, portmap) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to TM bits
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, table_index, &value);
    tagmap = value;
    tagmap &= ~(1 << portIdx);
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, table_index, tagmap) != IFX_SUCCESS)
        return IFX_ERROR;

    for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
    {
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, i, &value);
        if ( value == pPar->nVId )
        {
            // clear DVPM bits on related port
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_DVPM, i, portmap) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_ReservedAdd(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_reserved_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_regMapperSelector_t rms = COMMON_BIT_LATEST;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->nVId == 0  ) {
        rms = GLOBAL_RVID0;
    }
    else if ( pPar->nVId == 1  ) {
        rms = GLOBAL_RVID1;
    }
    else if ( pPar->nVId == 0xFFF  ) {
        rms = GLOBAL_RVIDFFF;
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported nVId = %d\n", pPar->nVId);
        return IFX_ERROR;
    }

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, 0, IFX_TRUE)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_ReservedRemove(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_reserved_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->nVId == 0  ) {
        rms = GLOBAL_RVID0;
    }
    else if ( pPar->nVId == 1  ) {
        rms = GLOBAL_RVID1;
    }
    else if ( pPar->nVId == 0xFFF  ) {
        rms = GLOBAL_RVIDFFF;
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported nVId = %d\n", pPar->nVId);
        return IFX_ERROR;
    }

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, 0, IFX_FALSE)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_DSCP_ClassGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_ClassCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t DSCPIdx;
    IFX_uint32_t value;

    for (DSCPIdx=0; DSCPIdx<64; DSCPIdx++)
    {
        if (DSCPIdx < 16)
        {
            if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQA, DSCPIdx, &value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (DSCPIdx < 16*2)
        {
            if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQB, DSCPIdx - 16, &value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (DSCPIdx < 16*3)
        {
            if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQC, DSCPIdx - 16*2, &value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (DSCPIdx < 16*4)
        {
            if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQD, DSCPIdx - 16*3, &value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        pPar->nTrafficClass[DSCPIdx] = value;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_DSCP_ClassSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_ClassCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t DSCPIdx;
    IFX_uint32_t value;

    for (DSCPIdx=0; DSCPIdx<64; DSCPIdx++)
    {
        value = pPar->nTrafficClass[DSCPIdx];
        if (DSCPIdx < 16)
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQA, DSCPIdx, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (DSCPIdx < 16*2)
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQB, DSCPIdx - 16, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (DSCPIdx < 16*3)
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQC, DSCPIdx - 16*2, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (DSCPIdx < 16*4)
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, DIFFSERV_PQD, DSCPIdx - 16*3, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
    }

    return IFX_SUCCESS;
}
#if 0
IFX_return_t IFX_PSB6970_QoS_MeterCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MeterCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MeterPortAssign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterPort_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MeterPortDeassign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterPort_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MeterPortGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_meterPortGet_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}
#endif
IFX_return_t IFX_PSB6970_QoS_PCP_ClassGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_PCP_ClassCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t PCPIdx;
    IFX_uint32_t value;

    for (PCPIdx=0; PCPIdx<8; PCPIdx++)
    {
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, DOT1X_PRIORITY_1PPQ, PCPIdx, &value)!= IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nTrafficClass[PCPIdx] = value;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PCP_ClassSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_PCP_ClassCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t PCPIdx;
    IFX_uint32_t value;

    for (PCPIdx=0; PCPIdx<8; PCPIdx++)
    {
        value = pPar->nTrafficClass[PCPIdx];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, DOT1X_PRIORITY_1PPQ, PCPIdx, value)!= IFX_SUCCESS)
            return IFX_ERROR;
    }

    return IFX_SUCCESS;
}
#if 0
IFX_return_t IFX_PSB6970_QoS_DSCP_DropPrecedenceCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_DropPrecedenceCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_DSCP_DropPrecedenceCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_DropPrecedenceCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortRemarkingCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portRemarkingCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortRemarkingCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portRemarkingCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ClassDSCP_Set(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassDSCP_Cfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ClassDSCP_Get(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassDSCP_Cfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ClassPCP_Set(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassPCP_Cfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ClassPCP_Get(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ClassPCP_Cfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}
#endif

IFX_return_t IFX_PSB6970_QoS_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t vpe, spe, ipvlan, pp, ppe;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VPE, portIdx, &vpe)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_SPE, portIdx, &spe)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_IPVLAN, portIdx, &ipvlan)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PP, portIdx, &pp)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PPE, portIdx, &ppe)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( vpe == 0 && ipvlan == 0 && spe == 0 )
        pPar->eClassMode = IFX_ETHSW_QOS_CLASS_SELECT_NO;
    else if ( vpe == 0 && ipvlan == 0 && spe == 1 )
        pPar->eClassMode = IFX_ETHSW_QOS_CLASS_SELECT_DSCP;
    else if ( vpe == 1 && ipvlan == 0 && spe == 0 )
        pPar->eClassMode = IFX_ETHSW_QOS_CLASS_SELECT_PCP;
    else if ( vpe == 1 && ipvlan == 1 && spe == 1 )
        pPar->eClassMode = IFX_ETHSW_QOS_CLASS_SELECT_DSCP_PCP;
    else if ( vpe == 1 && ipvlan == 0 && spe == 1 )
        pPar->eClassMode = IFX_ETHSW_QOS_CLASS_SELECT_PCP_DSCP;
    else
        pPar->eClassMode = IFX_ETHSW_QOS_CLASS_SELECT_NO;

    pPar->nTrafficClass = pp;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint8_t vpe=0, spe=0, ipvlan=0, pp=0, ppe=0;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->eClassMode == IFX_ETHSW_QOS_CLASS_SELECT_NO )
    {
        ppe = 1;
        vpe = 0;
        ipvlan = 0;
        spe = 0;
    }
    else if ( pPar->eClassMode == IFX_ETHSW_QOS_CLASS_SELECT_DSCP )
    {
        ppe = 0;
        vpe = 0;
        ipvlan = 0;
        spe = 1;
    }
    else if ( pPar->eClassMode == IFX_ETHSW_QOS_CLASS_SELECT_PCP )
    {
        ppe = 0;
        vpe = 1;
        ipvlan = 0;
        spe = 0;
    }
    else if ( pPar->eClassMode == IFX_ETHSW_QOS_CLASS_SELECT_DSCP_PCP )
    {
        ppe = 0;
        vpe = 1;
        ipvlan = 1;
        spe = 1;
    }
    else if ( pPar->eClassMode == IFX_ETHSW_QOS_CLASS_SELECT_PCP_DSCP )
    {
        ppe = 0;
        vpe = 1;
        ipvlan = 0;
        spe = 1;
    }
    else
         IFXOS_PRINT_INT_RAW(  "Not Supported !\n");

    pp  = pPar->nTrafficClass;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VPE, portIdx, vpe)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_SPE, portIdx, spe)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IPVLAN, portIdx, ipvlan)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PP, portIdx, pp)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PPE, portIdx, ppe)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}
#if 0
IFX_return_t IFX_PSB6970_QoS_SchedulerCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_schedulerCfg_t *pPar)
{

    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_SchedulerCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_schedulerCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ShaperCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ShaperCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ShaperQueueAssign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperQueue_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_ShaperQueueDeassign(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_ShaperQueue_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_StormAdd(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_stormCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_StormRemove(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_stormCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}
#endif
// implement internal function for IGMP
IFX_return_t IFX_PSB6970_IGMPV3_Enable(IFX_void_t *pDevCtx, IFX_uint32_t ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_boolean_t IGMPV3_Enable;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, &IGMPV3_Enable) != IFX_SUCCESS)
        return IFX_ERROR;

    return IGMPV3_Enable;
}

IFX_return_t IFX_PSB6970_WriteIPv4_GDA(IFX_void_t *pDevCtx, IFX_uint32_t ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value = 0;

    // Write the IPv4 GDA to register
    value = ipv4 & 0xFFFF;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_GID15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
    value = (ipv4 >> 16 ) & 0xFFFF;

    // to subtract 0xE000 to meet the Tantos 3G hardware
    value -= 0xE000;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_GID31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WriteGDA(IFX_void_t *pDevCtx, IFX_uint32_t ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value = 0;

    // Write the IPv4 GDA to register
    value = ipv4 & 0xFFFF;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_GID15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
    value = (ipv4 >> 16 ) & 0xFFFF;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_GID31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WriteIPv6_GDA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    // Write the IPv6 GDA to register
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_GID15_0, 0, ipv6[7]) != IFX_SUCCESS)
            return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_GID31_16, 0, ipv6[6]) != IFX_SUCCESS)
            return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_ReadIPv4_GDA(IFX_void_t *pDevCtx, IFX_uint32_t *ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID0, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    *ipv4 = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID1, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    // to add 0xE000 to meet the Tantos 3G hardware
    value += 0xE000 ;

    *ipv4 |= (value << 16) ;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_ReadGDA(IFX_void_t *pDevCtx, IFX_uint32_t *ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID0, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    *ipv4 = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID1, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    *ipv4 |= (value << 16) ;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_ReadIPv6_GDA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID0, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    ipv6[7] = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID1, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    ipv6[6] = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WriteIPv4_GSA(IFX_void_t *pDevCtx, IFX_uint32_t ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value = 0;

    // Write the IPv4 GSA to register
    value = ipv4 & 0xFFFF;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SIP15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
    value = (ipv4 >> 16 ) & 0xFFFF;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SIP31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SIP47_32, 0, 0) != IFX_SUCCESS)
            return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WriteIPv6_GSA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    // Write the IPv6 GSA to register
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SIP15_0, 0, ipv6[7]) != IFX_SUCCESS)
            return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SIP31_16, 0, ipv6[6]) != IFX_SUCCESS)
            return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SIP47_32, 0, ipv6[5]) != IFX_SUCCESS)
            return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_ReadIPv4_GSA(IFX_void_t *pDevCtx, IFX_uint32_t *ipv4)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID0, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    *ipv4 = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID1, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    *ipv4 |= (value << 16) ;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_ReadIPv6_GSA(IFX_void_t *pDevCtx, IFX_uint16_t *ipv6)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID0, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    ipv6[7] = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID1, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    ipv6[6] = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID2, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    ipv6[5] = value;

    return IFX_SUCCESS;
}


/*
   input parameter: Cmd
                    eAccessType

   return value   : success / fail
                    RegisterAccess.nGDA
                    RegisterAccess.nS3PMI;
                    RegisterAccess.nS3PMV;
                    RegisterAccess.eIPVersion

                    //bInitial
                    //bLast
                    //nPortId
                    //eModeMember

   example        :
                    IFX_uint16_t IGMPTC5=0;
                    IFX_uint32_t nGDA;
                    IFX_uint8_t nS3PMI;
                    IFX_uint8_t nS3PMV;
                    // Get the first GID information
                    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 3); // ICMD
                    RegisterAccess.uCmd = IGMPTC5;
                    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_GID_GET;
                    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
                        return IFX_ERROR;
                    eIPVersion = RegisterAccess.eIPVersion;
                    nGDA = RegisterAccess.nGDA;
                    nS3PMI = RegisterAccess.nS3PMI;
                    nS3PMV = RegisterAccess.nS3PMV;
 */
IFX_ETHSW_IGMP_commandResult_t IFX_PSB6970_MulticastTableAccessCommand(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_IGMP_commandResult_t result;
    IFX_uint32_t value = 0;
    IFX_uint32_t counter;

    counter = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_S4BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
             if (counter > 10000)
                return IFX_ERROR;
             else
                counter++;
    }while (value != 0);

    // IGMP command (ICMD,INVC,FMODE,PORT)
    IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, IFX_ETHSW_TANTOS3G_IGMPTC5_ADDR, pReg->uCmd);

    counter = 0;
    do {
       /*  Poll the busy BUSY bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_S4BUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
             if (counter > 10000)
                return IFX_ERROR;
             else
                counter++;
    }while (value != 0);

    /*  Read the IGMPTS4 to check the command result */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_S4R, 0, &value) != IFX_SUCCESS)
       return IFX_ERROR;

    result = value;
    if ( result == IFX_ETHSW_IGMP_COMMAND_OK )
    {
        // Check access type
        IFX_ETHSW_IP_Select_t ipversion;
        if (pReg->eAccessType == IFX_ETHSW_IGMP_GID_GET)
        {
            IFX_PSB6970_ReadGDA(pEthSWDevHandle, &value);
            pReg->nGDA = value;
        }
        else if (pReg->eAccessType == IFX_ETHSW_IGMP_SIP_GET)
        {
            IFX_PSB6970_CheckIP_VersionByGSA(pEthSWDevHandle, &ipversion);
            if (ipversion == IFX_ETHSW_IP_SELECT_IPV6)
                IFX_PSB6970_ReadIPv6_GSA(pEthSWDevHandle, (IFX_uint16_t*)&pReg->uIP_Gsa.nIPv6);
            else
            {
                IFX_PSB6970_ReadIPv4_GSA(pEthSWDevHandle, &value);
                pReg->uIP_Gsa.nIPv4 = value;
            }
            pReg->eIPVersion = ipversion;
        }


        // Port Map Valid Status
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_S3PMV, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pReg->nS3PMV = value;

        // Port Map Include/exclude Status
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_S3PMI, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pReg->nS3PMI = value;

    }

    return result;
}


IFX_return_t IFX_PSB6970_Create_GroupTable(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t i;

    for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
    {
        if ( ((( pReg->nS3PMV & 0x3F ) >> i ) & 0x01) == 0x01)
        {
            gMulticastTable[gMulticastTableEntryNumber].nPortId = i;
            if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
            {
                IFX_PSB6970_CopyIPV6(&gMulticastTable[gMulticastTableEntryNumber].uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
                IFX_PSB6970_ResetIPV6(&gMulticastTable[gMulticastTableEntryNumber].uIP_Gsa.nIPv6);
            }
            else
            {
                gMulticastTable[gMulticastTableEntryNumber].uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
                gMulticastTable[gMulticastTableEntryNumber].uIP_Gsa.nIPv4 = 0;
            }
            gMulticastTable[gMulticastTableEntryNumber].eIPVersion = pReg->eIPVersion;
            gMulticastTable[gMulticastTableEntryNumber].eModeMember = 0;
            gMulticastTableEntryNumber++;
            IFX_ETHSW_DEBUG_PRINT("gMulticastTableEntryNumber=%d\n", gMulticastTableEntryNumber);
        }
    }

    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.uIP_Gda.nIPv4
                    RegisterAccess.uIP_Gsa.nIPv4
                    RegisterAccess.eModeMember

   return value   : success

   example        :
                    RegisterAccess.nPortId = 0;
                    RegisterAccess.uIP_Gda.nIPv4 = 0xe1000001;
                    RegisterAccess.uIP_Gsa.nIPv4 = 0xc0a80101;
                    RegisterAccess.eModeMember = IFX_FALSE;
                    RegisterAccess.eIPVersion = pPar->eIPVersion;
                    if ( IFX_PSB6970_Create_GroupAndSourceListTable(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
 */
IFX_return_t IFX_PSB6970_Create_GroupAndSourceListTable(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    gMulticastTable[gMulticastTableEntryNumber].nPortId = pReg->nPortId;
    if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
    {
        IFX_PSB6970_CopyIPV6(&gMulticastTable[gMulticastTableEntryNumber].uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
        IFX_PSB6970_CopyIPV6(&gMulticastTable[gMulticastTableEntryNumber].uIP_Gsa.nIPv6, &pReg->uIP_Gsa.nIPv6);
    }
    else
    {
        gMulticastTable[gMulticastTableEntryNumber].uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
        gMulticastTable[gMulticastTableEntryNumber].uIP_Gsa.nIPv4 = pReg->uIP_Gsa.nIPv4;
    }
    gMulticastTable[gMulticastTableEntryNumber].eIPVersion = pReg->eIPVersion;
    if ( ( pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE ) && ( pReg->uIP_Gsa.nIPv4 == 0 ) )
      gMulticastTable[gMulticastTableEntryNumber].eModeMember = IFX_ETHSW_IGMP_MEMBER_DONT_CARE;
    else
      gMulticastTable[gMulticastTableEntryNumber].eModeMember = pReg->eModeMember;
    gMulticastTableEntryNumber++;

    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.uIP_Gda.nIPv4 / RegisterAccess.uIP_Gda.nIPv6

   return value   : success / fail
                    RegisterAccess.eModeMember

   example        :
                    IFX_ETHSW_IGMP_MemberMode_t eModeMember;
                    RegisterAccess.nPortId= 0;
                    RegisterAccess.uIP_Gda.nIPv4 = 0xe1000001;
                    RegisterAccess.eIPVersion = pPar->eIPVersion;
                    if ( IFX_PSB6970_GetMemberMode(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
                    eModeMember = RegisterAccess.eModeMember;
 */
IFX_return_t IFX_PSB6970_GetMemberMode(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;

    RegisterAccess.nPortId= pReg->nPortId;
    if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        IFX_PSB6970_CopyGDAIPV6(&RegisterAccess.nGDA, &pReg->uIP_Gda.nIPv6);
    else
        RegisterAccess.nGDA = pReg->uIP_Gda.nIPv4 - 0xE0000000;

    if ( IFX_PSB6970_GetFirstSIPEntryByPort(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
        return IFX_ERROR;

    pReg->eModeMember = RegisterAccess.eModeMember;

    return IFX_SUCCESS;
}

/*
   input parameter: eIPVersion

   return value   : success / fail
                    RegisterAccess.uIP_Gda.nIPv4

   example        :
                    IFX_uint8_t nS3PMI;
                    IFX_uint8_t nS3PMV;
                    if ( IFX_PSB6970_GetFirstGIDEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
                    nS3PMI = RegisterAccess.nS3PMI;
                    nS3PMV = RegisterAccess.nS3PMV;
                    nGDA = RegisterAccess.nGDA;
 */
IFX_return_t IFX_PSB6970_GetFirstGIDEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint16_t IGMPTC5=0;

    // Get the first GID information
    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 3); // ICMD
    RegisterAccess.uCmd = IGMPTC5;
    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_GID_GET;

    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
        return IFX_ERROR;

    pReg->nGDA = RegisterAccess.nGDA;

    pReg->nS3PMI = RegisterAccess.nS3PMI;
    pReg->nS3PMV = RegisterAccess.nS3PMV;

    return IFX_SUCCESS;
}

/*
   input parameter: none

   return value   : success / fail
                    RegisterAccess.nGDA;

   example        :
                    IFX_uint8_t nS3PMI;
                    IFX_uint8_t nS3PMV;
                    if ( IFX_PSB6970_GetNextGIDEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
                    nS3PMI = RegisterAccess.nS3PMI;
                    nS3PMV = RegisterAccess.nS3PMV;
                    nGDA = RegisterAccess.nGDA;
 */
IFX_return_t IFX_PSB6970_GetNextGIDEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint16_t IGMPTC5=0;

    // Get the first GID information
    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 4); // ICMD
    RegisterAccess.uCmd = IGMPTC5;
    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_GID_GET;
    //RegisterAccess.eIPVersion = pReg->eIPVersion;
    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
        return IFX_ERROR;

    pReg->nGDA = RegisterAccess.nGDA;
    pReg->nS3PMI = RegisterAccess.nS3PMI;
    pReg->nS3PMV = RegisterAccess.nS3PMV;

    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.uIP_Gda.nIPv4 / RegisterAccess.uIP_Gda.nIPv6
                    RegisterAccess.eIPVersion;

   return value   : success / fail
                    RegisterAccess.uIP_Gsa.nIPv4
                    RegisterAccess.uIP_Gsa.nIPv4

   example        :
                    IFX_ETHSW_IP_t uIP_Gsa;
                    IFX_ETHSW_IGMP_MemberMode_t eModeMember;
                    RegisterAccess.nPortId= 0;
                    RegisterAccess.uIP_Gda.nIPv4 = 0xe1000001;
                    RegisterAccess.eIPVersion = pPar->eIPVersion;
                    if ( IFX_PSB6970_GetFirstSIPEntryByPort(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
                    uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;
                    eModeMember = RegisterAccess.eModeMember;
 */
IFX_return_t IFX_PSB6970_GetFirstSIPEntryByPort(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint16_t IGMPTC5=0;

    // Write the GDA to register
    IFX_PSB6970_WriteGDA(pDevCtx, pReg->nGDA);

    IGMPTC5=SET_BITS(IGMPTC5, 2, 0, pReg->nPortId); // PORT
    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 6); // ICMD
    RegisterAccess.uCmd = IGMPTC5;

    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_SIP_GET;
    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
    {
        return IFX_ERROR;
    }

    if ( RegisterAccess.eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        IFX_PSB6970_CopyIPV6(&pReg->uIP_Gsa.nIPv6, &RegisterAccess.uIP_Gsa.nIPv6);
    else
        pReg->uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;

    pReg->eIPVersion = RegisterAccess.eIPVersion;

    if ( ((( RegisterAccess.nS3PMI & 0x3F ) >> pReg->nPortId ) & 0x01) == 0x01)
        pReg->eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
    else
        pReg->eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;

    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.nGDA

   return value   : success / fail
                    RegisterAccess.uIP_Gsa.nIPv4 / RegisterAccess.uIP_Gsa.nIPv6
                    RegisterAccess.eIPVersion

   example        :
                    IFX_ETHSW_IP_t uIP_Gsa;
                    IFX_ETHSW_IGMP_MemberMode_t eModeMember;
                    RegisterAccess.nPortId= 0;
                    RegisterAccess.nGDA = 0xe1000001;
                    if ( IFX_PSB6970_GetNextSIPEntryByPort(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
                    eIPVersion = RegisterAccess.eIPVersion;
                    uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;
                    eModeMember = RegisterAccess.eModeMember;
 */
IFX_return_t IFX_PSB6970_GetNextSIPEntryByPort(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint16_t IGMPTC5=0;

    // Write the GDA to register
    IFX_PSB6970_WriteGDA(pDevCtx, pReg->nGDA);

    IGMPTC5=SET_BITS(IGMPTC5, 2, 0, pReg->nPortId); // PORT
    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 7); // ICMD
    RegisterAccess.uCmd = IGMPTC5;
    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_SIP_GET;
    //RegisterAccess.eIPVersion = pReg->eIPVersion;
    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
    {
        return IFX_ERROR;
    }
    else
    {
        if ( RegisterAccess.eIPVersion== IFX_ETHSW_IP_SELECT_IPV6)
            IFX_PSB6970_CopyIPV6(&pReg->uIP_Gsa.nIPv6, &RegisterAccess.uIP_Gsa.nIPv6);
        else
            pReg->uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;
        pReg->eIPVersion = RegisterAccess.eIPVersion;

        if ( ((( RegisterAccess.nS3PMI & 0x3F ) >> pReg->nPortId ) & 0x01) == 0x01)
            pReg->eModeMember = IFX_ETHSW_IGMP_MEMBER_EXCLUDE;
        else
            pReg->eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
        return IFX_SUCCESS;
    }
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.uIP_Gda.nIPv4
                    RegisterAccess.uIP_Gsa.nIPv4
                    RegisterAccess.eModeMember

   return value   : success / fail
   example        :
                    RegisterAccess.nPortId= 0;
                    RegisterAccess.eIPVersion = IFX_ETHSW_IP_SELECT_IPV4;
                    RegisterAccess.uIP_Gda.nIPv4 = 0xe1000001;
                    RegisterAccess.uIP_Gsa.nIPv4 = 0xc0a80101;
                    RegisterAccess.eModeMember = IFX_FALSE;
                    if ( IFX_PSB6970_AddSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
 */

IFX_return_t IFX_PSB6970_AddSingleSIPEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint16_t IGMPTC5=0;
    IFX_ETHSW_IP_t uIP_Gsa;

    // Check IP version?
    if (pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
    {
        // Write the IPv6 GDA to register
        IFX_PSB6970_WriteIPv6_GDA(pDevCtx, (IFX_uint16_t *)&pReg->uIP_Gda.nIPv6);
        // Write the IPv6 GSA to register
        if (pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        {
            IFX_PSB6970_ResetIPV6(&uIP_Gsa.nIPv6);
            IFX_PSB6970_WriteIPv6_GSA(pDevCtx, (IFX_uint16_t *)&uIP_Gsa.nIPv6);
        }
        else
            IFX_PSB6970_WriteIPv6_GSA(pDevCtx, (IFX_uint16_t *)&pReg->uIP_Gsa.nIPv6);
    }
    else
    {
        // Write the IPv4 GDA to register
        IFX_PSB6970_WriteIPv4_GDA(pDevCtx, pReg->uIP_Gda.nIPv4);
        IFX_ETHSW_DEBUG_PRINT("Write GDA=%x to register\n", pReg->uIP_Gda.nIPv4);
        // Write the IPv4 GSA to register
        if (pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        {
            uIP_Gsa.nIPv4 = 0x0;
            IFX_PSB6970_WriteIPv4_GSA(pDevCtx, uIP_Gsa.nIPv4);
        }
        else
            IFX_PSB6970_WriteIPv4_GSA(pDevCtx, pReg->uIP_Gsa.nIPv4);
    }

    IGMPTC5=SET_BITS(IGMPTC5, 2, 0, pReg->nPortId); // PORT

    if (pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE)
        IGMPTC5=SET_BITS(IGMPTC5, 4, 4, 0); // include
    else if (pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE)
        IGMPTC5=SET_BITS(IGMPTC5, 4, 4, 1); // exclude
    else if (pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        IGMPTC5=SET_BITS(IGMPTC5, 4, 4, 1); // exclude
    else
        return IFX_ERROR;

    IGMPTC5=SET_BITS(IGMPTC5, 5, 5, 1); // INVC
    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 0); // ICMD
    RegisterAccess.uCmd = IGMPTC5;
    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_SET;
    RegisterAccess.eIPVersion = pReg->eIPVersion;

    // add a SIP entry.
    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
        return IFX_ERROR;


    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.uIP_Gda.nIPv4 / RegisterAccess.uIP_Gda.nIPv6
                    RegisterAccess.uIP_Gsa.nIPv4 / RegisterAccess.uIP_Gsa.nIPv6
                    RegisterAccess.eIPVersion

   return value   : success / fail
   example        :
                    RegisterAccess.nPortId= 0;
                    RegisterAccess.uIP_Gda.nIPv4 = 0xe1000001;
                    RegisterAccess.uIP_Gsa.nIPv4 = 0xc0a80101;
                    RegisterAccess.eIPVersion = pPar->eIPVersion;
                    if ( IFX_PSB6970_RemoveSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
 */

IFX_return_t IFX_PSB6970_RemoveSingleSIPEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint16_t IGMPTC5=0;
    IFX_ETHSW_IGMP_MemberMode_t eModeMember;
    IFX_ETHSW_IP_t uIP_Gsa;

    // Get original eModeMember on port
    RegisterAccess.nPortId= pReg->nPortId;
    if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
    else
        RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
    RegisterAccess.eIPVersion = pReg->eIPVersion;
    if ( IFX_PSB6970_GetMemberMode(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
        return IFX_ERROR;
    eModeMember = RegisterAccess.eModeMember;

    // Check IP version?
    if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
    {
        // Write the IPv6 GDA to register
        IFX_PSB6970_WriteIPv6_GDA(pDevCtx, (IFX_uint16_t *)&pReg->uIP_Gda.nIPv6);
        // Write the IPv6 GSA to register
        if (eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        {
            IFX_PSB6970_ResetIPV6(&uIP_Gsa.nIPv6);
            IFX_PSB6970_WriteIPv6_GSA(pDevCtx, (IFX_uint16_t *)&uIP_Gsa.nIPv6);
        }
        else
            IFX_PSB6970_WriteIPv6_GSA(pDevCtx, (IFX_uint16_t *)&pReg->uIP_Gsa.nIPv6);
    }
    else
    {
        // Write the IPv4 GDA to register
        IFX_PSB6970_WriteIPv4_GDA(pDevCtx, pReg->uIP_Gda.nIPv4);
        // Write the IPv4 GSA to register
        if (eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        {
            uIP_Gsa.nIPv4 = 0;
            IFX_PSB6970_WriteIPv4_GSA(pDevCtx, uIP_Gsa.nIPv4);
        }
        else
            IFX_PSB6970_WriteIPv4_GSA(pDevCtx, pReg->uIP_Gsa.nIPv4);
    }

    IGMPTC5=SET_BITS(IGMPTC5, 2, 0, pReg->nPortId); // PORT

    if (eModeMember == IFX_ETHSW_IGMP_MEMBER_INCLUDE)
        IGMPTC5=SET_BITS(IGMPTC5, 4, 4, 0); // include
    else if (eModeMember == IFX_ETHSW_IGMP_MEMBER_EXCLUDE)
        IGMPTC5=SET_BITS(IGMPTC5, 4, 4, 1); // exclude
    else if (eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        IGMPTC5=SET_BITS(IGMPTC5, 4, 4, 1); // exclude
    else
        return IFX_ERROR;

    IGMPTC5=SET_BITS(IGMPTC5, 5, 5, 0); // INVC
    IGMPTC5=SET_BITS(IGMPTC5, 10, 8, 0); // ICMD
    RegisterAccess.uCmd = IGMPTC5;
    RegisterAccess.eAccessType = IFX_ETHSW_IGMP_SET;
    RegisterAccess.eIPVersion = pReg->eIPVersion;

    // Remove a SIP entry.
    if (IFX_PSB6970_MulticastTableAccessCommand(pDevCtx, &RegisterAccess) != IFX_ETHSW_IGMP_COMMAND_OK)
        return IFX_ERROR;

    if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
    {
        IFX_ETHSW_DEBUG_PRINT("Remove IPv6_GDA=%x:%x, IPv6_GSA=%x:%x:%x\n",
            pReg->uIP_Gda.nIPv6[6], pReg->uIP_Gda.nIPv6[7],
            pReg->uIP_Gsa.nIPv6[5], pReg->uIP_Gsa.nIPv6[6], pReg->uIP_Gsa.nIPv6[7]);
    }
    else
    {
        IFX_ETHSW_DEBUG_PRINT("Remove IPv4_GDA=%x, IPv4_GSA=%x\n",
            pReg->uIP_Gda.nIPv4, pReg->uIP_Gsa.nIPv4);
    }
    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nPortId
                    RegisterAccess.uIP_Gda.nIPv4 / RegisterAccess.uIP_Gda.nIPv6
                    RegisterAccess.eIPVersion;

   return value   : success / fail
   example        :
                    RegisterAccess.nPortId= 0;
                    RegisterAccess.uIP_Gda.nIPv4 = 0xe1000001;
                    RegisterAccess.eIPVersion = pReg->eIPVersion;
                    if ( IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
 */

IFX_return_t IFX_PSB6970_RemoveMultipleSIPEntry(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_boolean_t result;
    IFX_ETHSW_IGMP_MemberMode_t eModeMember;
    IFX_ETHSW_IP_t uIP_Gsa;

    RegisterAccess.nPortId= pReg->nPortId;
    if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        IFX_PSB6970_CopyGDAIPV6(&RegisterAccess.nGDA, &pReg->uIP_Gda.nIPv6);
    else
        RegisterAccess.nGDA = pReg->uIP_Gda.nIPv4 - 0xE0000000;
    // Get first eModeMember & GSA
    if ( IFX_PSB6970_GetFirstSIPEntryByPort(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
        return IFX_ERROR;
    else
    {
        if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
            IFX_PSB6970_CopyIPV6(&uIP_Gsa.nIPv6, &RegisterAccess.uIP_Gsa.nIPv6);
        else
            uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;
        eModeMember = RegisterAccess.eModeMember;

        RegisterAccess.nPortId= pReg->nPortId;
        if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        {
            IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
            IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gsa.nIPv6, &uIP_Gsa.nIPv6);
        }
        else
        {
            RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
            RegisterAccess.uIP_Gsa.nIPv4 = uIP_Gsa.nIPv4;
        }
        RegisterAccess.eIPVersion = pReg->eIPVersion;
        // Remove first GSA
        if ( IFX_PSB6970_RemoveSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
            return IFX_ERROR;

        if ( pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
        {
            // Add IPv4 entry.
            RegisterAccess.nPortId= pReg->nPortId;
            RegisterAccess.eIPVersion = pReg->eIPVersion;
            RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
            RegisterAccess.uIP_Gsa.nIPv4 = 0;
            RegisterAccess.eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
            if ( IFX_PSB6970_AddSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;

            // Add IPv4 entry.
            RegisterAccess.nPortId= pReg->nPortId;
            RegisterAccess.eIPVersion = pReg->eIPVersion;
            RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
            RegisterAccess.uIP_Gsa.nIPv4 = 0;
            RegisterAccess.eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
            // We need to remove this entry again due to the hw limition
            if ( IFX_PSB6970_RemoveSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;
        }

        do
        {
            RegisterAccess.nPortId= pReg->nPortId;
            if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                IFX_PSB6970_CopyGDAIPV6(&RegisterAccess.nGDA, &pReg->uIP_Gda.nIPv6);
            else
                RegisterAccess.nGDA = pReg->uIP_Gda.nIPv4;

            // Get next eModeMember & GSA
            result =  IFX_PSB6970_GetNextSIPEntryByPort(pDevCtx, &RegisterAccess);
            if ( result == IFX_SUCCESS)
            {
                if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                {
                    IFX_PSB6970_CopyIPV6(&uIP_Gsa.nIPv6, &RegisterAccess.uIP_Gsa.nIPv6);
                }
                else
                {
                    uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;
                }
                eModeMember = RegisterAccess.eModeMember;

                RegisterAccess.nPortId = pReg->nPortId;
                if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                {
                    IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
                    IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gsa.nIPv6, &uIP_Gsa.nIPv6);
                }
                else
                {
                    RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
                    RegisterAccess.uIP_Gsa.nIPv4 = uIP_Gsa.nIPv4;
                }
                RegisterAccess.eIPVersion = pReg->eIPVersion;
                // Remove next GSA
                if ( IFX_PSB6970_RemoveSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                    return IFX_ERROR;

                if ( pReg->eModeMember == IFX_ETHSW_IGMP_MEMBER_DONT_CARE)
                {
                  // Add IPv4 entry.
                  RegisterAccess.nPortId= pReg->nPortId;
                  RegisterAccess.eIPVersion = pReg->eIPVersion;
                  RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
                  RegisterAccess.uIP_Gsa.nIPv4 = 0;
                  RegisterAccess.eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
                  if ( IFX_PSB6970_AddSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                      return IFX_ERROR;

                  // Add IPv4 entry.
                  RegisterAccess.nPortId= pReg->nPortId;
                  RegisterAccess.eIPVersion = pReg->eIPVersion;
                  RegisterAccess.uIP_Gda.nIPv4 = pReg->uIP_Gda.nIPv4;
                  RegisterAccess.uIP_Gsa.nIPv4 = 0;
                  RegisterAccess.eModeMember = IFX_ETHSW_IGMP_MEMBER_INCLUDE;
                  // We need to remove this entry again due to the hw limition
                  if ( IFX_PSB6970_RemoveSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                     return IFX_ERROR;
                }

                if ( pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                {
                    IFX_ETHSW_DEBUG_PRINT("Remove a entry (GDA=%x:%x, GSA=%x:%x:%x) on Port[%d]\n",
                          pReg->uIP_Gda.nIPv6[6], pReg->uIP_Gda.nIPv6[7],
                          uIP_Gsa.nIPv6[5], uIP_Gsa.nIPv6[6], uIP_Gsa.nIPv6[7], pReg->nPortId);
                }
                else
                {
                    IFX_ETHSW_DEBUG_PRINT("Remove a entry(GDA=%x, GSA=%x) on Port[%d]\n",
                        pReg->uIP_Gda.nIPv4, uIP_Gsa.nIPv4, pReg->nPortId);
                }

            }

        } while ( result == IFX_FALSE);
    }
    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.eIPVersion;

   return value   : success / fail
   example        :
                    RegisterAccess.eIPVersion = pReg->eIPVersion;
                    if ( IFX_PSB6970_RemoveIGMP_HWTable(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
 */

IFX_return_t IFX_PSB6970_RemoveIGMP_HWTable(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_ETHSW_IP_t uIP_Gda;
    IFX_uint32_t ipv4_gda=0;
    IFX_boolean_t result;
    IFX_uint8_t i;

    // Get the first GID information
    RegisterAccess.eIPVersion = pReg->eIPVersion;
    if ( IFX_PSB6970_GetFirstGIDEntry(pDevCtx, &RegisterAccess) == IFX_SUCCESS)
    {
        if (pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        {
            IFX_PSB6970_CopyIPV6(&uIP_Gda.nIPv6, &RegisterAccess.uIP_Gda.nIPv6);
            IFX_ETHSW_DEBUG_PRINT("First IPv6_GDA = %x:%x\n", RegisterAccess.uIP_Gda.nIPv6[6], RegisterAccess.uIP_Gda.nIPv6[7] );
        }
        else
            ipv4_gda = RegisterAccess.uIP_Gda.nIPv4;

        for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
        {
            RegisterAccess.nPortId= i;
            if (pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &uIP_Gda.nIPv6);
            else
                RegisterAccess.uIP_Gda.nIPv4 = ipv4_gda;
            RegisterAccess.eIPVersion = pReg->eIPVersion;
            IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess);
        }

        do
        {
            // Get the next GID information
            RegisterAccess.eIPVersion = pReg->eIPVersion;
            result = IFX_PSB6970_GetNextGIDEntry(pDevCtx, &RegisterAccess);
            if ( result == IFX_SUCCESS)
            {
                if (pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                    IFX_PSB6970_CopyIPV6(&uIP_Gda.nIPv6, &RegisterAccess.uIP_Gda.nIPv6);
                else
                    ipv4_gda = RegisterAccess.uIP_Gda.nIPv4;

                // remove
                for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
                {
                    RegisterAccess.nPortId= i;
                    if (pReg->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                        IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &uIP_Gda.nIPv6);
                    else
                        RegisterAccess.uIP_Gda.nIPv4 = ipv4_gda;
                    RegisterAccess.eIPVersion = pReg->eIPVersion;
                    IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess);
                }

            }

        } while ( result == IFX_SUCCESS );

    }
    return IFX_SUCCESS;
}

/*
   input parameter: RegisterAccess.nGDA
                    RegisterAccess.eIPVersion;

   return value   : success / fail
                    create group and source list entry
   example        :
                    RegisterAccess.nGDA = 0xe1000001;
                    if ( IFX_PSB6970_CreateSIPEntryByGID(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;
 */
IFX_return_t IFX_PSB6970_CreateSIPEntryByGID(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRegisterAccess *pReg)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_ETHSW_IGMP_MemberMode_t eModeMember;
    IFX_boolean_t result;
    IFX_uint8_t i;
    IFX_ETHSW_IP_t uIP_Gsa;
    IFX_ETHSW_IP_Select_t eIPVersion;

    for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
    {
        RegisterAccess.nPortId= i;
        RegisterAccess.nGDA = pReg->nGDA;
        // Get first eModeMember & GSA
        if ( IFX_PSB6970_GetFirstSIPEntryByPort(pDevCtx, &RegisterAccess) == IFX_SUCCESS)
        {
            // IPv4 or IPv6 ?
            eIPVersion = RegisterAccess.eIPVersion;
            if ( eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                IFX_PSB6970_CopyIPV6(&uIP_Gsa.nIPv6, &RegisterAccess.uIP_Gsa.nIPv6);
            else
                uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;

            eModeMember = RegisterAccess.eModeMember;

            // Save the first GSA information into mirror table
            RegisterAccess.nPortId = i;
            if ( eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
            {
                //IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
                IFX_PSB6970_CopyIPV6GDA(&RegisterAccess.uIP_Gda.nIPv6, pReg->nGDA);
                IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gsa.nIPv6, &uIP_Gsa.nIPv6);
            }
            else
            {
                RegisterAccess.uIP_Gda.nIPv4 = pReg->nGDA + 0xE0000000;
                RegisterAccess.uIP_Gsa.nIPv4 = uIP_Gsa.nIPv4;
            }
            RegisterAccess.eModeMember = eModeMember;
            RegisterAccess.eIPVersion = eIPVersion;
            if ( IFX_PSB6970_Create_GroupAndSourceListTable(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;

            do
            {
                RegisterAccess.nPortId= i;
                RegisterAccess.nGDA = pReg->nGDA;
                // Get next eModeMember & GSA
                result = IFX_PSB6970_GetNextSIPEntryByPort(pDevCtx, &RegisterAccess);
                if ( result == IFX_SUCCESS)
                {
                    eIPVersion = RegisterAccess.eIPVersion;
                    if ( eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                        IFX_PSB6970_CopyIPV6(&uIP_Gsa.nIPv6, &RegisterAccess.uIP_Gsa.nIPv6);
                    else
                        uIP_Gsa.nIPv4 = RegisterAccess.uIP_Gsa.nIPv4;

                    eModeMember = RegisterAccess.eModeMember;

                    // Save next SIP information into mirror table
                    RegisterAccess.nPortId = i;
                    if ( eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                    {
                        //IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pReg->uIP_Gda.nIPv6);
                        IFX_PSB6970_CopyIPV6GDA(&RegisterAccess.uIP_Gda.nIPv6, pReg->nGDA);
                        IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gsa.nIPv6, &uIP_Gsa.nIPv6);
                    }
                    else
                    {
                        RegisterAccess.uIP_Gda.nIPv4 = pReg->nGDA + 0xE0000000;
                        RegisterAccess.uIP_Gsa.nIPv4 = uIP_Gsa.nIPv4;
                    }
                    RegisterAccess.eModeMember = eModeMember;
                    RegisterAccess.eIPVersion = eIPVersion;
                    if ( IFX_PSB6970_Create_GroupAndSourceListTable(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;

                }

            } while ( result == IFX_SUCCESS);
        }
    }
    return IFX_SUCCESS;
}


IFX_return_t IFX_PSB6970_CopyIPV6(IFX_void_t *pdest, IFX_void_t *psrc)
{
    memcpy(pdest, psrc,sizeof(IFX_ETHSW_IP_t));
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CopyIPV6GDA(IFX_void_t *pdest, IFX_uint32_t nGDA)
{
    IFX_uint16_t *nIPv6 = (IFX_uint16_t *)pdest;
    nIPv6[7] = nGDA & 0xFFFF;
    nIPv6[6] = (nGDA >> 16) & 0xFFFF;
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CopyGDAIPV6(IFX_uint32_t *nGDA, IFX_void_t *pdest)
{
    IFX_uint16_t *nIPv6 = (IFX_uint16_t *)pdest;
    *nGDA = nIPv6[7];
    *nGDA += (nIPv6[6] << 16) ;
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_ResetIPV6(IFX_void_t *pIPv6)
{
    memset(pIPv6, 0, sizeof(IFX_ETHSW_IP_t));
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_IGMP_Init(IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;

    pEthSWDev->eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED;
    RegisterAccess.eIPVersion = IFX_ETHSW_IP_SELECT_IPV4;
    IFX_PSB6970_RemoveIGMP_HWTable(pDevCtx, &RegisterAccess);
    RegisterAccess.eIPVersion = IFX_ETHSW_IP_SELECT_IPV6;
    IFX_PSB6970_RemoveIGMP_HWTable(pDevCtx, &RegisterAccess);

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CheckIP_VersionByGDA(IFX_void_t *pDevCtx, IFX_ETHSW_IP_Select_t *ipversion)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID1, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

#ifdef SUPPORT_IPV6
    if ( value < 0xE000 )
        *ipversion = IFX_ETHSW_IP_SELECT_IPV4;
    else
        *ipversion = IFX_ETHSW_IP_SELECT_IPV6;
#else
        *ipversion = IFX_ETHSW_IP_SELECT_IPV4;
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CheckIP_VersionByGSA(IFX_void_t *pDevCtx, IFX_ETHSW_IP_Select_t *ipversion)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SIPGID2, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

#ifdef SUPPORT_IPV6
    if ( value == 0x0000 )
        *ipversion = IFX_ETHSW_IP_SELECT_IPV4;
    else
        *ipversion = IFX_ETHSW_IP_SELECT_IPV6;
#else
        *ipversion = IFX_ETHSW_IP_SELECT_IPV4;
#endif

    return IFX_SUCCESS;
}


IFX_return_t IFX_PSB6970_MulticastRouterPortAdd(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouter_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;
    IFX_uint8_t portIdx = pPar->nPortId;

    if (portIdx > 6)
        return IFX_ERROR;

    if (pEthSWDevHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING)
    {
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_DRP, 0, &value);

        if(((value >> portIdx) & 0x1 )== 1) {
            IFXOS_PRINT_INT_RAW(  "Error: the port was already in the member\n");
        } else {
            value = (value | (1 << portIdx));
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_DRP, 0, value) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }
#if 0
    else if (pEthSWDevHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD)
    {
        value = pEthSWDevHandle->nRouterPort;
        if(((value >> portIdx) & 0x1 )== 1) {
            IFXOS_PRINT_INT_RAW(  "Error: the port was already in the member\n");
        } else {
            value = (value | (1 << portIdx));
            pEthSWDevHandle->nRouterPort = value;
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_MP, 0, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }

    }
#endif
    else
        return IFX_ERROR;
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MulticastRouterPortRead(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouterRead_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if (pEthSWDevHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING)
    {
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_DRP, 0, &value);
        pPar->nPortId = value;
    }
#if 0
    else if (pEthSWDevHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD)
    {
        pPar->nPortId = pEthSWDevHandle->nRouterPort;
    }
#endif
    else
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MulticastRouterPortRemove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouter_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;
    IFX_uint8_t portIdx = pPar->nPortId;

    if (portIdx > 6)
        return IFX_ERROR;


    if (pEthSWDevHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING)
    {
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_DRP, 0, &value);

        if(((value >> portIdx) & 0x1 )== 0) {
            IFXOS_PRINT_INT_RAW(  "Error: the port was not in the member\n");
        } else {
            value = (value & ~(1 << portIdx));
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_DRP, 0, value) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }
#if 0
    else if (pEthSWDevHandle->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD)
    {
        value = pEthSWDevHandle->nRouterPort;
        if(((value >> portIdx) & 0x1 )== 0) {
            IFXOS_PRINT_INT_RAW(  "Error: the port was not in the member\n");
        } else {
            value = (value & ~(1 << portIdx));
            pEthSWDevHandle->nRouterPort = value;
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_MP, 0, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }

    }
#endif
    else
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MulticastSnoopCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_multicastSnoopCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    pPar->eIGMP_Mode = pEthSWDevHandle->eIGMP_Mode;
    // Get IGMP V3
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, &pPar->bIGMPv3);

    // Get MS.SCPTCP
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPTCP, 0, &value);
    pPar->bCrossVLAN = value;

    // Get MS.SCPA
    if (pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD )
    {
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPA, 0, &value);
        if ( value == 1)
            pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DEFAULT;
        else if ( value == 0)
            pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DISCARD;
        else if ( value == 2)
            pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_CPU;
        else
        {
            IFXOS_PRINT_INT_RAW(  "Error\n");
            //return IFX_ERROR;
        }
        pPar->nForwardPortId = 0;
    }
    else
    {
        pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DEFAULT;
        pPar->nForwardPortId = 0;

    }

    // get MS.SCPP to 'Queue number'
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPP, 0, &value);
    pPar->nClassOfService = value;

    // Get MS.RV
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_RV, 0, &value);
    pPar->nRobust = value;

    // Get MIC.QI
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_QI, 0, &value);
    pPar->nQueryInterval = value;

    // Get CCR.IRSJA
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, CONGESTION_IRSJA, 0, &value);
    if ( value == 0 )
        pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_REPORT_JOIN;
    else if ( value== 3 )
        pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_TRANSPARENT;
    else
    {
        IFXOS_PRINT_INT_RAW(  "This Mode doesn't exists\n");
        //pPar->eSuppressionAggregation = IFX_ETHSW_MULTICAST_REPORT;
    }

    // Get HIOR.HISFL
    IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_HISFL, 0, &value);
    pPar->bFastLeave = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MulticastSnoopCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_multicastSnoopCfg_t *pPar)
{
	IFX_uint32_t value=0, i;
	IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
	
	switch (pPar->eIGMP_Mode) {
		case IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED:
			/* default values */
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xF6, 0x0000)!= IFX_SUCCESS)
				return IFX_ERROR;
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE4, 0x7C80)!= IFX_SUCCESS)
				return IFX_ERROR;
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE3, 0x4000)!= IFX_SUCCESS)
				return IFX_ERROR;
			//if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE6, 0x7F7F)!= IFX_SUCCESS)
			//    return IFX_ERROR;
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xEF, 0x0000)!= IFX_SUCCESS)
                return IFX_ERROR;
			for (i = 0; i < IFX_PSB6970_TANTOSXG_PORT_NUMBER; i++) {
				if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IMTE, i, 0) != IFX_SUCCESS)
					return IFX_ERROR;
			}
			pEthSWDevHandle->eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED;
			break;
		case IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING:
			if ( pPar->bIGMPv3 == IFX_TRUE ) {
				IFXOS_PRINT_INT_RAW(  " IGMPV3  doesn't support for Autolearning mode\n");
				return IFX_ERROR;
			}
			/*Enable HW IGMP Snooping (HISE) & 
			All the DA=33.33.0.0.0.x packets are treated as normal IGMP,
			All the DA=01.0.5E.0.0.x packets are treated as normal IGMP,
			All the DIP=224.0.0.x packets are treated as normal IGMP and  
			Use IP for searching group ID  enabled*/
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xF6, 0x010F)!= IFX_SUCCESS)
				return IFX_ERROR;
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE4, 0x7C80)!= IFX_SUCCESS)
				return IFX_ERROR;
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE3, 0x4000)!= IFX_SUCCESS)
				return IFX_ERROR;
			//if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE6, 0x7F7F)!= IFX_SUCCESS)
			//    return IFX_ERROR;
			if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xEF, 0x0000)!= IFX_SUCCESS)
				return IFX_ERROR;
			 /* IGMP/MLD Enabled for all ports */
			for (i = 0; i < IFX_PSB6970_TANTOSXG_PORT_NUMBER; i++) {
				if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IMTE, i, 1) != IFX_SUCCESS)
                	return IFX_ERROR;
			}
			pEthSWDevHandle->eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_AUTOLEARNING;
			// Set MS.RV
			IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_RV, 0, pPar->nRobust);
			// Set MIC.QI
			IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_QI, 0, pPar->nQueryInterval);

            break;
        case IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD:
        	/*Enable HW IGMP Snooping (HISE) & 
			All the DA=33.33.0.0.0.x packets are treated as normal IGMP,
			All the DA=01.0.5E.0.0.x packets are treated as normal IGMP,
			All the DIP=224.0.0.x packets are treated as normal IGMP and  
			Use IP for searching group ID  enabled*/
        	if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xF6, 0x010F)!= IFX_SUCCESS)
        		return IFX_ERROR;
        	if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE4, 0x7C80)!= IFX_SUCCESS)
        		return IFX_ERROR;
        	if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE3, 0x4000)!= IFX_SUCCESS)
        		return IFX_ERROR;
        	//if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xE6, 0x7F7F)!= IFX_SUCCESS)
        	//    return IFX_ERROR;
        	if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, 0xEF, 0x0000)!= IFX_SUCCESS)
        		return IFX_ERROR;
             for (i = 0; i < IFX_PSB6970_TANTOSXG_PORT_NUMBER; i++) {
				if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_IMTE, i, 1) != IFX_SUCCESS)
                	return IFX_ERROR;
			}
			pEthSWDevHandle->eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD;
			break;
		default:
			IFXOS_PRINT_INT_RAW(  "eIGMP_Mode: This Mode doesn't exists\n");
			return IFX_ERROR;
	}  /* -----  end switch  ----- */

	/* enable IGMP V3 */
	if (pPar->bIGMPv3 == IFX_TRUE )
		IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, 1);
    else
        IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, 0);

    // Set MS.SCPTCP
    if ( pPar->bCrossVLAN == IFX_TRUE )
    {
        IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPTCP, 0, IFX_TRUE);
        IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_IPMPT, 0, IFX_TRUE);
    }
    else
    {
        IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPTCP, 0, IFX_FALSE);
        IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_IPMPT, 0, IFX_FALSE);
    }

	if (pPar->eIGMP_Mode == IFX_ETHSW_MULTICAST_SNOOP_MODE_FORWARD) {
		switch (pPar->eForwardPort ) {
    		case IFX_ETHSW_PORT_FORWARD_DEFAULT:
    			// Set MS.SCPA
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPA, 0, 1);
				// Set HIPI
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_HIPI, 0, 1);
				break;
			case IFX_ETHSW_PORT_FORWARD_DISCARD:
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPA, 0, 0);
				// Set HIPI
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_HIPI, 0, 1);
				break;
			case IFX_ETHSW_PORT_FORWARD_CPU:
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPA, 0, 2);
				// Set HIPI
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_HIPI, 0, 0);
				break;
			case IFX_ETHSW_PORT_FORWARD_PORT:
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPA, 0, 3);
				// Set HIPI
				IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_HIPI, 0, 0);
			default:
				IFXOS_PRINT_INT_RAW(  "[%d]: \n",__LINE__);
				IFXOS_PRINT_INT_RAW(   "[%d]: eForwardPort: This Mode doesn't exists\n",__LINE__);
				return IFX_ERROR;
			}
        
        // Par->nForwardPortId doesn't support in Tantos3G.
      // Not supported forwarding to any other selectedDuplex port on Tantos3G
    }

    IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPPE, 0, 1);
    // Set MS.SCPP to 'Queue number'
    IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_SCPP, 0, pPar->nClassOfService);

    // Set CCR.IRSJA
    if ( pPar->eSuppressionAggregation == IFX_ETHSW_MULTICAST_REPORT_JOIN )
        value = 0;
    else if ( pPar->eSuppressionAggregation == IFX_ETHSW_MULTICAST_REPORT )
        value = 1;
    else if ( pPar->eSuppressionAggregation == IFX_ETHSW_MULTICAST_TRANSPARENT )
        value = 3;
    else {
    	IFXOS_PRINT_INT_RAW(  "[%d]: \n",__LINE__);
    	IFXOS_PRINT_INT_RAW(   "[%d]: eForwardPort should be IFX_ETHSW_PORT_FORWARD_CPU\n",__LINE__);
        return IFX_ERROR;
    }

    IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, CONGESTION_IRSJA, 0, value);

    // Set HIOR.HISFL
    IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MULTICAST_HISFL, 0, pPar->bFastLeave);

    return IFX_SUCCESS;
}


IFX_return_t IFX_PSB6970_MulticastTableEntryAdd(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint32_t value = 0;
    IFX_boolean_t IGMPV3_Enable;
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_ETHSW_IGMP_MemberMode_t eModeMember;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    IGMPV3_Enable = value;


    // TANTOS Switch API only support IGMPv1/v2/v3 for IPv4
#ifndef SUPPORT_IPV6
    if ( pPar->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        return IFX_ERROR;
#endif

    if ( (pPar->uIP_Gsa.nIPv4 == 0) && (pPar->eModeMember != IFX_ETHSW_IGMP_MEMBER_DONT_CARE) )
        return IFX_ERROR;

    if ( IGMPV3_Enable )
    {
        if (pPar->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        {
            // Remove previous IPv6 entry if eModeMember conflict in the same group
            RegisterAccess.nPortId= portIdx;
            IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pPar->uIP_Gda.nIPv6);
            RegisterAccess.eIPVersion = pPar->eIPVersion;
            if ( IFX_PSB6970_GetMemberMode(pDevCtx, &RegisterAccess) == IFX_SUCCESS)
            {
                eModeMember = RegisterAccess.eModeMember;
                if ( eModeMember != pPar->eModeMember)
                {
                    RegisterAccess.nPortId= portIdx;
                    IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pPar->uIP_Gda.nIPv6);
                    RegisterAccess.eIPVersion = pPar->eIPVersion;
                    if ( IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;

                }
            }

            // Add a IPv6 entry
            RegisterAccess.nPortId= portIdx;
            RegisterAccess.eIPVersion = IFX_ETHSW_IP_SELECT_IPV6;
            IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pPar->uIP_Gda.nIPv6);
            IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gsa.nIPv6, &pPar->uIP_Gsa.nIPv6);
            RegisterAccess.eModeMember = pPar->eModeMember;
            if ( IFX_PSB6970_AddSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;
            IFX_ETHSW_DEBUG_PRINT("Add entry(GDA=%x:%x, GSA=%x:%x:%x)\n",
                  pPar->uIP_Gda.nIPv6[6], pPar->uIP_Gda.nIPv6[7],
                  pPar->uIP_Gsa.nIPv6[5], pPar->uIP_Gsa.nIPv6[6], pPar->uIP_Gsa.nIPv6[7]);
        }
        else
        {
            // Remove previous IPv4 entry if eModeMember conflict in the same group
            RegisterAccess.nPortId= portIdx;
            RegisterAccess.uIP_Gda.nIPv4 = pPar->uIP_Gda.nIPv4;
            RegisterAccess.eIPVersion = pPar->eIPVersion;
            if ( IFX_PSB6970_GetMemberMode(pDevCtx, &RegisterAccess) == IFX_SUCCESS)
            {
                eModeMember = RegisterAccess.eModeMember;
                if ( eModeMember != pPar->eModeMember)
                {
                    RegisterAccess.nPortId= portIdx;
                    RegisterAccess.uIP_Gda.nIPv4 = pPar->uIP_Gda.nIPv4;
                    RegisterAccess.eIPVersion = pPar->eIPVersion;
                    if ( IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        return IFX_ERROR;

                }
            }

            // Add IPv4 entry.
            RegisterAccess.nPortId= portIdx;
            RegisterAccess.eIPVersion = pPar->eIPVersion;
            RegisterAccess.uIP_Gda.nIPv4 = pPar->uIP_Gda.nIPv4;
            RegisterAccess.uIP_Gsa.nIPv4 = pPar->uIP_Gsa.nIPv4;
            RegisterAccess.eModeMember = pPar->eModeMember;
            if ( IFX_PSB6970_AddSingleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;
            IFX_ETHSW_DEBUG_PRINT("Add entry(GDA=%x, GSA=%x)\n",
                pPar->uIP_Gda.nIPv4, pPar->uIP_Gsa.nIPv4);

        }
    }
    else
    {
        IFXOS_PRINT_INT_RAW(    "[%d]: please enable IGMP V3 first\n",__LINE__);
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

/*
 *  input parameter: bInitial
 *                  eIPVersion
 *  return value:
 *                  bInitial
 *                  bLast
 *                  nPortId
 *                  uIP_Gda
 *                  uIP_Gsa
 *                  eModeMember
 */
IFX_return_t IFX_PSB6970_MulticastTableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRead_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value = 0;
    IFX_uint16_t i;
    IFX_boolean_t IGMPV3_Enable;
    IFX_boolean_t result;
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;
    IFX_uint8_t nS3PMI;
    IFX_uint8_t nS3PMV;
    IFX_uint32_t nGDA;
    IFX_ETHSW_IP_Select_t eIPVersion = IFX_ETHSW_IP_SELECT_IPV4;
	if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 ) {
		IFXOS_PRINT_INT_RAW(  "This will not supported on AR9\n");
		return IFX_ERROR;
	}
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    IGMPV3_Enable = value;

    // API will create mirror table
    // to hold the all entrys of group table and source list table
    if ( pPar->bInitial == IFX_TRUE )
    {
        // Reset prefix mirror table if exist?
        for (i=0; i<MULTICAST_TABLE_ENTRY_MAX; i++)
        {
            gMulticastTable[i].nPortId = -1;
            gMulticastTable[i].eIPVersion = IFX_ETHSW_IP_SELECT_IPV4;
            gMulticastTable[i].uIP_Gda.nIPv4 = 0;
            gMulticastTable[i].uIP_Gsa.nIPv4 = 0;
            IFX_PSB6970_ResetIPV6(&gMulticastTable[i].uIP_Gda.nIPv6);
            IFX_PSB6970_ResetIPV6(&gMulticastTable[i].uIP_Gsa.nIPv6);
            gMulticastTable[i].eModeMember = IFX_ETHSW_IGMP_MEMBER_DONT_CARE;

        }
        gMulticastTableEntryNumber = 0;
        gMulticastTableEntryCount = 0;

        // Get the first GID information
        if ( IFX_PSB6970_GetFirstGIDEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
        {
            // no IGMP table exist
            pPar->bInitial = IFX_FALSE;
            pPar->bLast = IFX_TRUE;
            return IFX_ERROR;
        }
        else
        {
            nGDA = RegisterAccess.nGDA;
            nS3PMI = RegisterAccess.nS3PMI;
            nS3PMV = RegisterAccess.nS3PMV;

            // check IGMP V3?
            if ( IGMPV3_Enable )
            {
                RegisterAccess.nGDA = nGDA;
                // Save first all SIP in GID information into mirror table
                if ( IFX_PSB6970_CreateSIPEntryByGID(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                {
                    //pPar->bInitial = IFX_FALSE;
                    //pPar->bLast = IFX_TRUE;
                    //return IFX_ERROR;
                }
                IFX_ETHSW_DEBUG_PRINT("First IPv6_GSA = %x:%x:%x\n", RegisterAccess.uIP_Gsa.nIPv6[5], RegisterAccess.uIP_Gsa.nIPv6[6], RegisterAccess.uIP_Gsa.nIPv6[7] );
            }
            else
            {
                // Save first GID information into mirror table

                // only support IPv4
                RegisterAccess.uIP_Gda.nIPv4 = nGDA + 0xE0000000;
                RegisterAccess.nS3PMI = nS3PMI;
                RegisterAccess.nS3PMV = nS3PMV;
                RegisterAccess.eIPVersion = IFX_ETHSW_IP_SELECT_IPV4;
                IFX_PSB6970_Create_GroupTable(pDevCtx, &RegisterAccess);
            }

            do
            {
                // Get the next GID information
                //RegisterAccess.eIPVersion = pPar->eIPVersion;
                result = IFX_PSB6970_GetNextGIDEntry(pDevCtx, &RegisterAccess);
                if ( result == IFX_SUCCESS)
                {
                    nGDA = RegisterAccess.nGDA;
                    nS3PMI = RegisterAccess.nS3PMI;
                    nS3PMV = RegisterAccess.nS3PMV;

                    // check IGMP V3?
                    if ( IGMPV3_Enable )
                    {
                        RegisterAccess.nGDA = nGDA;
                        if ( IFX_PSB6970_CreateSIPEntryByGID(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                        {
                            // Do nothing
                        }
                    }
                    else
                    {
                        // Save next GID information into mirror table

                        // only support IPv4
                        RegisterAccess.uIP_Gda.nIPv4 = nGDA + 0xE0000000;
                        RegisterAccess.nS3PMI = nS3PMI;
                        RegisterAccess.nS3PMV = nS3PMV;
                        RegisterAccess.eIPVersion = eIPVersion;
                        IFX_PSB6970_Create_GroupTable(pDevCtx, &RegisterAccess);
                    }

                }

            } while ( result == IFX_SUCCESS );

            if ( gMulticastTableEntryNumber == 0)
            {
                pPar->bInitial = IFX_FALSE;
                pPar->bLast = IFX_TRUE;
                return IFX_SUCCESS;
            }

            // return first entry from mirror table.
            pPar->bLast = IFX_FALSE;
            pPar->nPortId = gMulticastTable[gMulticastTableEntryCount].nPortId;
            pPar->eIPVersion = gMulticastTable[gMulticastTableEntryCount].eIPVersion;
            if ( pPar->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
            {
                IFX_PSB6970_CopyIPV6(&pPar->uIP_Gda.nIPv6, &gMulticastTable[gMulticastTableEntryCount].uIP_Gda.nIPv6);
                IFX_PSB6970_CopyIPV6(&pPar->uIP_Gsa.nIPv6, &gMulticastTable[gMulticastTableEntryCount].uIP_Gsa.nIPv6);
            }
            else
            {
                pPar->uIP_Gda.nIPv4 = gMulticastTable[gMulticastTableEntryCount].uIP_Gda.nIPv4;
                pPar->uIP_Gsa.nIPv4 = gMulticastTable[gMulticastTableEntryCount].uIP_Gsa.nIPv4;
            }
            pPar->eModeMember = gMulticastTable[gMulticastTableEntryCount].eModeMember;
            gMulticastTableEntryCount++;
        }
    }
    else
    {
        // check mirror table
        if ( gMulticastTableEntryNumber > 0)
        {
            // check if no more entry ?
            if (gMulticastTableEntryCount < gMulticastTableEntryNumber)
            {
                // return next entry from mirror table.
                pPar->nPortId = gMulticastTable[gMulticastTableEntryCount].nPortId;
                pPar->eIPVersion = gMulticastTable[gMulticastTableEntryCount].eIPVersion;
                if ( pPar->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
                {
                    IFX_PSB6970_CopyIPV6(&pPar->uIP_Gda.nIPv6, &gMulticastTable[gMulticastTableEntryCount].uIP_Gda.nIPv6);
                    IFX_PSB6970_CopyIPV6(&pPar->uIP_Gsa.nIPv6, &gMulticastTable[gMulticastTableEntryCount].uIP_Gsa.nIPv6);
                }
                else
                {
                    pPar->uIP_Gda.nIPv4 = gMulticastTable[gMulticastTableEntryCount].uIP_Gda.nIPv4;
                    pPar->uIP_Gsa.nIPv4 = gMulticastTable[gMulticastTableEntryCount].uIP_Gsa.nIPv4;
                }
                pPar->eModeMember = gMulticastTable[gMulticastTableEntryCount].eModeMember;

                // increase counter for mirror table entry
                gMulticastTableEntryCount++;
                pPar->bLast = IFX_FALSE;
            }
            else
            {
                pPar->bLast = IFX_TRUE;
            }
        }
        else
        {
            // this will return error
            //IFXOS_PRINT_INT_RAW(  "Please use bInitial=1 first\n");
            return IFX_ERROR;

        }
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MulticastTableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value = 0;
    IFX_boolean_t IGMPV3_Enable;
    IFX_ETHSW_multicastTableRegisterAccess RegisterAccess;

	if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 ) {
		IFXOS_PRINT_INT_RAW(  "This will not supported on AR9\n");
		return IFX_ERROR;
	}
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MULTICAST_IGMPV3E, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    IGMPV3_Enable = value;

    // TANTOS Switch API only support IGMPv1/v2/v3 for IPv4
#ifndef SUPPORT_IPV6
    if ( pPar->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        return IFX_ERROR;
#endif

    if ( IGMPV3_Enable )
    {
        if (pPar->eIPVersion == IFX_ETHSW_IP_SELECT_IPV6)
        {
            // Remove all the Source IP V6 in the designated GID
            // from source list membership table

            RegisterAccess.nPortId= pPar->nPortId;
            IFX_PSB6970_CopyIPV6(&RegisterAccess.uIP_Gda.nIPv6, &pPar->uIP_Gda.nIPv6);
            RegisterAccess.eIPVersion = pPar->eIPVersion;
            if ( IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;
        }
        else
        {
            // Remove all the Source IP V4 in the designated GID
            // from source list membership table
            RegisterAccess.nPortId= pPar->nPortId;
            RegisterAccess.uIP_Gda.nIPv4 = pPar->uIP_Gda.nIPv4;
            RegisterAccess.eIPVersion = pPar->eIPVersion;
            RegisterAccess.eModeMember = pPar->eModeMember;
            if ( IFX_PSB6970_RemoveMultipleSIPEntry(pDevCtx, &RegisterAccess) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "[%d]: please enable IGMP V3 first\n",__LINE__);
        return IFX_ERROR;
    }
    return IFX_SUCCESS;

}

IFX_return_t IFX_PSB6970_8021X_EAPOL_RuleGet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_EAPOL_Rule_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t valid, mg, act;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RA01_VALID, 1, &valid)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RA01_MG, 1, &mg)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RA01_ACT, 1, &act)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( valid == IFX_TRUE)
    {
        if ( mg == 1 && act == 1 )
             pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DISCARD ;
        if ( mg == 1 && act == 2 )
             pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_CPU ;
        if ( mg == 1 && act == 0 )
             pPar->eForwardPort = IFX_ETHSW_PORT_FORWARD_DEFAULT;
    }
    else // IFX_ETHSW_PORT_FORWARD_PORT
         IFXOS_PRINT_INT_RAW(  "There is no EAPOL rule !\n");

    // Not Supported nForwardPortId !
    pPar->nForwardPortId = 0;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_8021X_EAPOL_RuleSet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_EAPOL_Rule_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t act=0;

    if (pPar->nForwardPortId != 0)
        return IFX_ERROR;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||// Tantos 3G
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA01_VALID, 1, IFX_TRUE)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA01_MG, 1, IFX_TRUE)!= IFX_SUCCESS)
            return IFX_ERROR;

        if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DEFAULT )
            act = 0; // Broadcast to all ports
        else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DISCARD )
            act = 1; // Discard the packets
        else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_CPU )
            act = 2; // to cpu
        else // IFX_ETHSW_PORT_FORWARD_PORT
        {
            IFXOS_PRINT_INT_RAW(  "Out of range on eForwardPort parameter!\n");
            return IFX_ERROR;
        }

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA01_ACT, 1, act)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA03_VALID, 0, IFX_TRUE)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA03_MG, 0, IFX_TRUE)!= IFX_SUCCESS)
            return IFX_ERROR;

        if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DEFAULT )
            act = 0;  // Broadcast to all ports
        else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_DISCARD )
            act = 1; // Discard the packets
        else if ( pPar->eForwardPort == IFX_ETHSW_PORT_FORWARD_CPU )
            act = 2; // to cpu
        else // IFX_ETHSW_PORT_FORWARD_PORT
        {
            IFXOS_PRINT_INT_RAW(  "Out of range on eForwardPort parameter!\n");
            return IFX_ERROR;
        }

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RA03_ACT, 0, act)!= IFX_SUCCESS)
            return IFX_ERROR;
    }


    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_8021X_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPortId;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PAS, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    // IFX_ETHSW_8021X_PORT_STATE_RX_AUTHORIZED and IFX_ETHSW_8021X_PORT_STATE_TX_AUTHORIZED were not supported
    pPar->eState = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_8021X_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_portCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value=0;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->eState == IFX_ETHSW_8021X_PORT_STATE_AUTHORIZED )
         value = 0;
    else if ( pPar->eState == IFX_ETHSW_8021X_PORT_STATE_UNAUTHORIZED )
         value = 1;
    else
    {
        IFXOS_PRINT_INT_RAW(  "Out of range on eState parameter!\n");
        return IFX_ERROR;
    }

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_PAS, portIdx, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CPU_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on AR9\n");
        return IFX_ERROR;
    }
    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    //  Validating that a command request does not conflict with an existing configuration

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MIRROR_CPN, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( value == portIdx )
       pPar->bCPU_PortValid = IFX_TRUE;
    else
       pPar->bCPU_PortValid = IFX_FALSE;

    // read STRE bit
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MIRROR_STRE, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bSpecialTagIngress = value;

    // read STTE bit
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MIRROR_STTE, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bSpecialTagEgress = value;

    // read CCCRC bit
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MIRROR_CCCRC, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bFcsCheck = !value;

    // Not supported for bFcsGenerate
    pPar->bFcsGenerate = 0;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CPU_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on AR9\n");
        return IFX_ERROR;
    }

    if ( pPar->bFcsGenerate != 0 )
    {
        IFXOS_PRINT_INT_RAW(  "Not supported for bFcsGenerate\n");
        return IFX_ERROR;
    }
    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->bCPU_PortValid == IFX_TRUE )
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MIRROR_CPN, 0, portIdx)!= IFX_SUCCESS)
            return IFX_ERROR;
    }

    // set STRE bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MIRROR_STRE, 0, pPar->bSpecialTagIngress)!= IFX_SUCCESS)
        return IFX_ERROR;

    // set STTE bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MIRROR_STTE, 0, pPar->bSpecialTagEgress)!= IFX_SUCCESS)
        return IFX_ERROR;

    // set CCCRC bit
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MIRROR_CCCRC, 0, !pPar->bFcsCheck)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Not supported for bFcsGenerate

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CPU_PortExtendCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortExtendCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CPU_PortExtendCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortExtendCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CapGet(IFX_void_t *pDevCtx, IFX_ETHSW_cap_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    SWAPI_API_CAPABILITY_GET_t *pCap;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||// Tantos 3G
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
        pCap = Tantos3G_Cap;
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
        pCap = AR9_Cap;
    else
       return IFX_ERROR;

    if (pPar->nCapType >= IFX_ETHSW_CAP_TYPE_LAST)
       return IFX_ERROR;

    strcpy(pPar->cDesc, pCap[pPar->nCapType].cDesc);
    pPar->nCap = pCap[pPar->nCapType].nCap;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_cfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    /* read eMAC_TableAgeTimer from SGC1[E0].ATS[12:10] bits */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, GLOBAL_ATS, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    switch ( value ) {
        case 0:
             pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_1_SEC;
             break;
        case 1:
             pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_10_SEC;
             break;
        case 2:
             pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_300_SEC;
             break;
        case 3:
             pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_1_HOUR;
             break;
        case 4:
             pPar->eMAC_TableAgeTimer = IFX_ETHSW_AGETIMER_1_DAY;
             break;
    }               /* -----  end switch  ----- */

    /* read bVLAN_Aware from pEthSWDevHandle->bVLAN_Aware */
    pPar->bVLAN_Aware = pEthSWDevHandle->bVLAN_Aware;

    /* read nMaxPacketLen from SGC1[E0]:MPL[9:8] bits */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, GLOBAL_MPL, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    if (value == 0)
      pPar->nMaxPacketLen = 1522;
    else if ( value == 1 )
      pPar->nMaxPacketLen = 1518;
    else if ( value == 2 )
      pPar->nMaxPacketLen = 1536;
      

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        //IFXOS_PRINT_INT_RAW(  "The bPauseMAC_ModeSrc and nPauseMAC_Src[6] were not supported on AR9 internal switch\n");
        pPar->bPauseMAC_ModeSrc = IFX_FALSE;
        pPar->nPauseMAC_Src[0] = 0;
        pPar->nPauseMAC_Src[1] = 0;
        pPar->nPauseMAC_Src[2] = 0;
        pPar->nPauseMAC_Src[3] = 0;
        pPar->nPauseMAC_Src[4] = 0;
        pPar->nPauseMAC_Src[5] = 0;
    }
    else
    {
        // nPauseMAC_Src[x]:     0        1   2 3  4 5
        // PAUSE_ADDR3     : 47_41 x  39_32 31_16 15_0
        /* read bPauseMAC_ModeSrc from SMA1[E9]:PAC[8] bit */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PAUSE_PAC, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->bPauseMAC_ModeSrc = value;

        /* read nPauseMAC_Src[6] from SMA1 ~ SMA3 register */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR15_0, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nPauseMAC_Src[5] = value        & 0x00FF;
        pPar->nPauseMAC_Src[4] = (value >> 8) & 0x00FF;
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR31_16, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nPauseMAC_Src[3] = value        & 0x00FF;
        pPar->nPauseMAC_Src[2] = (value >> 8) & 0x00FF;
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR39_32, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nPauseMAC_Src[1] = value;
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR47_41, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nPauseMAC_Src[0] = value << 0x01;

    }
    return IFX_SUCCESS;
}


IFX_return_t IFX_PSB6970_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_cfg_t *pPar)
{
    IFX_uint32_t value=0;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        if (pPar->bPauseMAC_ModeSrc != IFX_FALSE)
        // IFXOS_PRINT_INT_RAW(  "The nPauseMAC_Src was not supported on AR9 internal switch\n");
            return IFX_ERROR;
    }

    /* write eMAC_TableAgeTimer to SGC1[E0].ATS[12:10] bits */
    switch ( pPar->eMAC_TableAgeTimer ) {
        case IFX_ETHSW_AGETIMER_1_SEC:
             value = 0;
             break;
        case IFX_ETHSW_AGETIMER_10_SEC:
             value = 1;
             break;
        case IFX_ETHSW_AGETIMER_300_SEC:
             value = 2;
             break;
        case IFX_ETHSW_AGETIMER_1_HOUR:
             value = 3;
             break;
        case IFX_ETHSW_AGETIMER_1_DAY:
             value = 4;
             break;
        default:
             IFXOS_PRINT_INT_RAW(  "eMAC_TableAgeTimer error\n");
             break;
    }               /* -----  end switch  ----- */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, GLOBAL_ATS, 0, value) != IFX_SUCCESS)
        return IFX_ERROR;

    /* write bVLAN_Aware to XXX[XXX] register */
    if ( pPar->bVLAN_Aware == IFX_TRUE )
         IFX_PSB6970_VLAN_awareEnable(pEthSWDevHandle);
    else
         IFX_PSB6970_VLAN_awareDisable(pEthSWDevHandle);

    /* write nMaxPacketLen to SGC1[E0]:MPL[9:8] bits */
    if(pPar->nMaxPacketLen <= 1518 )
       value = 1;
    else if (( pPar->nMaxPacketLen > 1518 ) && ( pPar->nMaxPacketLen <= 1522 ))
       value = 0;
    else if ( pPar->nMaxPacketLen > 1522 )
       value = 2;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, GLOBAL_MPL, 0, value) != IFX_SUCCESS)
        return IFX_ERROR;

    if (pPar->bPauseMAC_ModeSrc == IFX_TRUE)
    {
        // nPauseMAC_Src[x]:     0        1   2 3  4 5
        // PAUSE_ADDR3     : 47_41 x  39_32 31_16 15_0

        /* write nPauseMAC_Src[6] to SMA1[E9] ~ SMA3[EB] register */
        value = pPar->nPauseMAC_Src[4] << 8 | pPar->nPauseMAC_Src[5];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        value = pPar->nPauseMAC_Src[2] << 8 | pPar->nPauseMAC_Src[3];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        value = pPar->nPauseMAC_Src[1];
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR39_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        // write to PAUSE_ADDR47_41, skip bit 0 [PAUSE_PAC]
        value = pPar->nPauseMAC_Src[0] >> 0x01;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR47_41, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;

        /* write bPauseMAC_ModeSrc to SMA1[E9]:PAC[8] bit */
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_PAC, 0, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_PAC, 0, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR15_0, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR31_16, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR39_32, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PAUSE_ADDR47_41, 0, value) != IFX_SUCCESS)
            return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_Disable(IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
        return IFX_ERROR;

    /* write SGC2[E1].SE[15] bit */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, GLOBAL_SE, 0, IFX_FALSE) != IFX_SUCCESS)
       return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_Enable(IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
        return IFX_ERROR;

    /* write SGC2[E1].SE[15] bit */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, GLOBAL_SE, 0, IFX_TRUE) != IFX_SUCCESS)
       return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_HW_Init(IFX_void_t *pDevCtx, IFX_ETHSW_HW_Init_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;


    if ( pPar->eInitMode <= IFX_ETHSW_HW_INIT_NO)
    {
        IFX_PSB6970_VLAN_Table_SW_Init(pEthSWDevHandle);
        IFX_PSB6970_MFC_SW_Init(pEthSWDevHandle);
    }

    if ( pPar->eInitMode == IFX_ETHSW_HW_INIT_WR)
    {
        IFX_PSB6970_RestoreAllRegisters(pEthSWDevHandle);
    }

    if ( pPar->eInitMode > IFX_ETHSW_HW_INIT_NO)
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported !\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MDIO_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_cfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    // Not Supported pPar->bMDIO_Enable

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MCS, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( value == 1 )
         pPar->nMDIO_Speed = 0;	//IFX_ETHSW_MDIO_SPEED_HIGH;
    else if ( value == 4 )
         pPar->nMDIO_Speed = 1;	//IFX_ETHSW_MDIO_SPEED_NORM;
    else if ( value == 12 )
         pPar->nMDIO_Speed = 2;	//IFX_ETHSW_MDIO_SPEED_SLOW;
    else
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MDIO_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_cfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    // Not Supported pPar->bMDIO_Enable

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->nMDIO_Speed == 0 /*IFX_ETHSW_MDIO_SPEED_HIGH */)
         value = 1;
    else if ( pPar->nMDIO_Speed == 1 /* IFX_ETHSW_MDIO_SPEED_NORM */ )
         value = 4;
    else if ( pPar->nMDIO_Speed == 2 /* IFX_ETHSW_MDIO_SPEED_SLOW*/ )
         value = 12;
    else
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MCS, 0, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MDIO_DataRead(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_data_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;
    IFX_uint16_t nTimeOutCnt;

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy MDIO bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MDIO_MBUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    // if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_WD, 0, pPar->nData) != IFX_SUCCESS)
    //     return IFX_ERROR;

    /*  Set the operation code to write operation */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_OP, 0, IFX_PSB6970_MDIO_OP_READ) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  set the PHY address */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_PHYAD, 0, pPar->nAddressDev) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  set the register address to address inside the PHY address location */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_REGAD, 0, pPar->nAddressReg) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  start the data transmission */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_MBUSY, 0, 1) != IFX_SUCCESS)
        return IFX_ERROR;

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy MDIO bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MDIO_MBUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

   if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MDIO_RD, 0, &value) != IFX_SUCCESS)
       return IFX_ERROR;

    pPar->nData = value & 0xFFFF;
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MDIO_DataWrite(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_data_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;
    IFX_uint16_t nTimeOutCnt;

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy MDIO bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MDIO_MBUSY, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_WD, 0, pPar->nData) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  Set the operation code to write operation */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_OP, 0, IFX_PSB6970_MDIO_OP_WRITE) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  set the PHY address */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_PHYAD, 0, pPar->nAddressDev) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  set the register address to address inside the PHY address location */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_REGAD, 0, pPar->nAddressReg) != IFX_SUCCESS)
        return IFX_ERROR;

    /*  start the data transmission */
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MDIO_MBUSY, 0, 1) != IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_MonitorPortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_monitorPortCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, MIRROR_SNIFFPN, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( value == portIdx )
        pPar->bMonitorPort = IFX_TRUE;
    else
        pPar->bMonitorPort = IFX_FALSE;

    return IFX_SUCCESS;

}

IFX_return_t IFX_PSB6970_MonitorPortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_monitorPortCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    //  Validating that a command request does not conflict with an existing configuration
    if(pPar->bMonitorPort == IFX_TRUE )
    {
        // Translate the requested configuration into device specific register
        // commands and write the configuration over Register Access Layer to the device.
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, MIRROR_SNIFFPN, 0, portIdx)!= IFX_SUCCESS)
            return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortLinkCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portLinkCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;
    IFX_uint32_t speed, speed_high;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G || // Tantos 3G
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G )  // Tantos 0G
    {
    	/*
#if defined(AR9)
        if ( portIdx <= 2 ) // internal PHY
#elif defined(DANUBE)
        if ( portIdx <= 4 ) // internal PHY
#elif defined(AMAZON_SE)
        if ( portIdx <= 3 ) // internal PHY
#endif*/
 //       {
            if ( pEthSWDevHandle->PortConfig[portIdx].bPHYDown == IFX_FALSE)
            {
                // read duplex status
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PDS, portIdx, &value)!= IFX_SUCCESS)
                    return IFX_ERROR;

                if ( value == 1 )
                     pPar->eDuplex = IFX_ETHSW_DUPLEX_FULL;
                else
                     pPar->eDuplex = IFX_ETHSW_DUPLEX_HALF;

                // read speed status
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PSS, portIdx, &speed)!= IFX_SUCCESS)
                    return IFX_ERROR;
                if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PSHS, portIdx, &speed_high)!= IFX_SUCCESS)
                    return IFX_ERROR;

                if (speed_high == IFX_TRUE)
                {
                    pPar->eSpeed = IFX_ETHSW_PORT_SPEED_1000;
                }
                else
                {
                    if (speed == IFX_TRUE)
                        pPar->eSpeed = IFX_ETHSW_PORT_SPEED_100;
                    else
                        pPar->eSpeed = IFX_ETHSW_PORT_SPEED_10;
                }
            }
            else
            {
                 pPar->eDuplex = 0;
                 pPar->eSpeed = 0;
            }
            // read MII mode
            pPar->eMII_Mode = IFX_ETHSW_PORT_HW_MII;

//
/*        }
#if defined(AR9)
        else if ( portIdx == 4 )
        {
            if ( pEthSWDevHandle->PortConfig[portIdx].bPHYDown == IFX_FALSE)
            {
                // read duplex status
                value = IFX_PSB6970_GPHY_DUPLEX_STATUS_Get(pEthSWDevHandle, portIdx);
                if ( value == 1)
                    pPar->eDuplex = IFX_ETHSW_DUPLEX_FULL;
                else
                    pPar->eDuplex = IFX_ETHSW_DUPLEX_HALF;
                // read speed status
                // 0:10M, 1:100M, 2:1000M
                value = IFX_PSB6970_GPHY_SPEED_STATUS_Get(pEthSWDevHandle, portIdx);
                if ( value == 0)
                    pPar->eSpeed = IFX_ETHSW_PORT_SPEED_10;
                else if (value == 1)
                    pPar->eSpeed = IFX_ETHSW_PORT_SPEED_100;
                else if (value == 2)
                  pPar->eSpeed = IFX_ETHSW_PORT_SPEED_1000;
            }
            else
            {
                pPar->eDuplex = 0;
                pPar->eSpeed = 0;
            }

            // read MII mode
            if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, GLOBAL_P4M, 0, &value)!= IFX_SUCCESS)
              return IFX_ERROR;
            if (value==0)
                pPar->eMII_Mode = IFX_ETHSW_PORT_HW_MII;
            else if (value==3)
                pPar->eMII_Mode = IFX_ETHSW_PORT_HW_RGMII;
        }
#endif
*/
        if ( pEthSWDevHandle->PortConfig[portIdx].bPHYDown == IFX_FALSE)
        {
            // read link status
            value = IFX_PSB6970_PHY_LINK_STATUS_Get(pEthSWDevHandle, portIdx);
            if ( value == 0 ) // down
               pPar->eLink = IFX_ETHSW_PORT_LINK_DOWN;
            else if ( value == 1 ) // up
               pPar->eLink = IFX_ETHSW_PORT_LINK_UP;
        }
        else
           pPar->eLink = IFX_ETHSW_PORT_LINK_DOWN;


        // read clock mode
        // 0: N/A 1: Master 2:Slave
        pPar->eClkMode = IFX_ETHSW_PORT_CLK_NA;

    }
    else // AR9 internal switch
    {
		if ( pEthSWDevHandle->PortConfig[portIdx].bPHYDown == IFX_FALSE) {
		// read duplex status
			if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PDS, portIdx, &value)!= IFX_SUCCESS)
				return IFX_ERROR;
			if ( value == 1 )
				pPar->eDuplex = IFX_ETHSW_DUPLEX_FULL;
			else
				pPar->eDuplex = IFX_ETHSW_DUPLEX_HALF;
			// read speed status
		
			if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PSHS, portIdx, &speed_high)!= IFX_SUCCESS)
				return IFX_ERROR;
			if (speed_high == IFX_TRUE) {
				pPar->eSpeed = IFX_ETHSW_PORT_SPEED_1000;
			}  else {
				if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PSS, portIdx, &speed)!= IFX_SUCCESS)
					return IFX_ERROR;
				if (speed == IFX_TRUE)
					pPar->eSpeed = IFX_ETHSW_PORT_SPEED_100;
				else
					pPar->eSpeed = IFX_ETHSW_PORT_SPEED_10;
			}
			if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PLS, portIdx, &speed)!= IFX_SUCCESS)
				return IFX_ERROR;
			if ( value == 1 )
				pPar->eLink = IFX_ETHSW_PORT_LINK_UP;
			else 
				pPar->eLink = IFX_ETHSW_PORT_LINK_DOWN;
		} else {
			pPar->eDuplex = 0;
          	pPar->eSpeed = 0;
          	pPar->eLink = IFX_ETHSW_PORT_LINK_DOWN;
        }
        if ( portIdx == 1 ) {
        	// read MII mode
        	if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1IS, 0, &value)!= IFX_SUCCESS)
        		return IFX_ERROR;
        } else if (portIdx == 0 ) {
         	if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P0IS, 0, &value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        if ( value == 0 )
        	pPar->eMII_Mode = IFX_ETHSW_PORT_HW_RGMII;
        else if ( value == 1 )
        	pPar->eMII_Mode = IFX_ETHSW_PORT_HW_MII;
        else if ( value == 3 )
        	pPar->eMII_Mode = IFX_ETHSW_PORT_HW_RMII;
		if ( portIdx == 1 ) {
			// read clock mode
			if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1CKIO, 0, &value)!= IFX_SUCCESS)
				return IFX_ERROR;
		} else if (portIdx == 0 ) {
			// read clock mode
			if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P0CKIO, 0, &value)!= IFX_SUCCESS)
				return IFX_ERROR;
		}
		if ( value == 0 )
			pPar->eClkMode = IFX_ETHSW_PORT_CLK_SLAVE;
		else
			pPar->eClkMode = IFX_ETHSW_PORT_CLK_MASTER;

    }
    pPar->eMII_Type = IFX_ETHSW_PORT_MAC;

    return IFX_SUCCESS;

}

IFX_return_t IFX_PSB6970_PortLinkCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portLinkCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;

#ifdef AR9
    IFX_uint32_t value=0;
#endif

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // TantosXG
    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||// Tantos 3G
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
    {
/*
#if defined(AR9)
        if ( portIdx <= 2 ) // internal PHY
#elif defined(DANUBE)
        if ( portIdx <= 4 ) // internal PHY
#elif defined(AMAZON_SE)
        if ( portIdx <= 3 ) // internal PHY
#endif*/
 //       {
            // handle speed and duplex
            if ( pPar->bDuplexForce == IFX_TRUE || pPar->bSpeedForce == IFX_TRUE)
            {
                // no autonegotiation
                IFX_PSB6970_PHY_AutoNegotiation(pEthSWDevHandle, portIdx, IFX_DISABLE);

                // Set duplex parameter
                if ( pPar->bDuplexForce == IFX_TRUE )
                    IFX_PSB6970_PHY_forcedDuplex(pEthSWDevHandle, portIdx, pPar->eDuplex);

                // Set speed parameter
                if ( pPar->bSpeedForce == IFX_TRUE)
                    IFX_PSB6970_PHY_forcedSpeed(pEthSWDevHandle, portIdx, pPar->eSpeed);
            }
            else
            {
                // autonegotiation
                IFX_PSB6970_PHY_AutoNegotiation(pEthSWDevHandle, portIdx, IFX_ENABLE);
            }
            // handle link
            if ( pPar->bLinkForce == IFX_TRUE)
            {
                if ( pPar->eLink == IFX_ETHSW_PORT_LINK_UP )
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 0) != IFX_SUCCESS)
                        return IFX_ERROR;
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 1) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process phy
                    IFX_PSB6970_PHY_PDN_Clear(pEthSWDevHandle, portIdx);
                    pEthSWDevHandle->PortConfig[portIdx].bPHYDown = IFX_FALSE;
#ifdef IFX_ETHSW_API_COC
                    IFX_ETHSW_PM_linkForceSet(pEthSWDevHandle->pPMCtx, portIdx, IFX_TRUE);
#endif
                }
                else if ( pPar->eLink == IFX_ETHSW_PORT_LINK_DOWN )
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 0) != IFX_SUCCESS)
                        return IFX_ERROR;
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 1) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process phy
                    IFX_PSB6970_PHY_PDN_Set(pEthSWDevHandle, portIdx);
                    pEthSWDevHandle->PortConfig[portIdx].bPHYDown = IFX_TRUE;
#ifdef IFX_ETHSW_API_COC
                    IFX_ETHSW_PM_linkForceSet(pEthSWDevHandle->pPMCtx, portIdx, IFX_TRUE);
#endif
                }
                else
                    return IFX_ERROR;
            }
            else
            {
                pEthSWDevHandle->PortConfig[portIdx].bPHYDown = IFX_FALSE;
#ifdef IFX_ETHSW_API_COC
                IFX_ETHSW_PM_linkForceSet(pEthSWDevHandle->pPMCtx, portIdx, IFX_FALSE);
#endif
                // process port
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 0) != IFX_SUCCESS)
                    return IFX_ERROR;
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 0) != IFX_SUCCESS)
                    return IFX_ERROR;
                // process phy
                IFX_PSB6970_PHY_PDN_Clear(pEthSWDevHandle, portIdx);
            }
//        }
/*
#if defined(AR9)
        else if ( portIdx == 4 ) // external GPHY
        {
            // handle speed and duplex
            if ( pPar->bDuplexForce == IFX_TRUE || pPar->bSpeedForce == IFX_TRUE)
            {
                // no autonegotiation
                IFX_PSB6970_PHY_AutoNegotiation(pEthSWDevHandle, portIdx, IFX_DISABLE);

                // Set duplex parameter
                if ( pPar->bDuplexForce == IFX_TRUE )
                    IFX_PSB6970_PHY_forcedDuplex(pEthSWDevHandle, portIdx, pPar->eDuplex);

                // Set speed parameter
                if ( pPar->bSpeedForce == IFX_TRUE)
                    IFX_PSB6970_GPHY_forcedSpeed(pEthSWDevHandle, portIdx, pPar->eSpeed);
            }
            else
            {
                // autonegotiation
                IFX_PSB6970_PHY_AutoNegotiation(pEthSWDevHandle, portIdx, IFX_ENABLE);
            }
            // handle link
            if ( pPar->bLinkForce == IFX_TRUE)
            {
                if ( pPar->eLink == IFX_ETHSW_PORT_LINK_UP )
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 0) != IFX_SUCCESS)
                        return IFX_ERROR;
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 1) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process phy
                    IFX_PSB6970_PHY_PDN_Clear(pEthSWDevHandle, portIdx);
                }
                else if ( pPar->eLink == IFX_ETHSW_PORT_LINK_DOWN )
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 0) != IFX_SUCCESS)
                        return IFX_ERROR;
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 1) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process phy
                    IFX_PSB6970_PHY_PDN_Set(pEthSWDevHandle, portIdx);
                }
                else
                    return IFX_ERROR;
            }
            else
            {
                // process port
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 0) != IFX_SUCCESS)
                    return IFX_ERROR;
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 0) != IFX_SUCCESS)
                    return IFX_ERROR;
                // process phy
                IFX_PSB6970_PHY_PDN_Clear(pEthSWDevHandle, portIdx);
            }
        }
        // eMII_Mode was not supported onSet Tantos3G
        // eMII_Type was not supported on Tantos3G
        // eClkMode was not supported on Tantos3G
#endif
*/
    }
#if defined(AR9)
    else // AR9 internal switch
    {
        if ( portIdx == 1 ) // GPHY
        {
            // handle speed and duplex
            if ( pPar->bDuplexForce == IFX_TRUE || pPar->bSpeedForce == IFX_TRUE)
            {
                // no autonegotiation
                IFX_PSB6970_PHY_AutoNegotiation(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID, IFX_DISABLE);

                // Set duplex parameter
                if ( pPar->bDuplexForce == IFX_TRUE )
                {
                    // process port
                    if ( pPar->eDuplex == IFX_ETHSW_DUPLEX_FULL )
                    {
                        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1DUP, 0, 1) != IFX_SUCCESS)
                            return IFX_ERROR;
                    }
                    else
                    {
                        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1DUP, 0, 0) != IFX_SUCCESS)
                            return IFX_ERROR;
                    }
                    // process gphy
                    IFX_PSB6970_PHY_forcedDuplex(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID, pPar->eDuplex);
                }

                // Set speed parameter
                if ( pPar->eSpeed == IFX_ETHSW_PORT_SPEED_10 )
                    value = 0;
                else if ( pPar->eSpeed == IFX_ETHSW_PORT_SPEED_100 )
                    value = 1;
                else if ( pPar->eSpeed == IFX_ETHSW_PORT_SPEED_1000 )
                    value = 2;
                else
                    return IFX_ERROR;

                if ( pPar->bSpeedForce == IFX_TRUE)
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1SPD, 0, value) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process gphy
                    IFX_PSB6970_GPHY_forcedSpeed(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID, pPar->eSpeed);
                }
            }
            else
            {
                // autonegotiation
                IFX_PSB6970_PHY_AutoNegotiation(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID, IFX_ENABLE);
            }
            // handle link
            if ( pPar->bLinkForce == IFX_TRUE)
            {
                if ( pPar->eLink == IFX_ETHSW_PORT_LINK_UP )
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 0) != IFX_SUCCESS)
                        return IFX_ERROR;
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 1) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process gphy
                    IFX_PSB6970_PHY_PDN_Clear(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID);
                    pEthSWDevHandle->PortConfig[portIdx].bPHYDown = IFX_FALSE;
                }
                else if ( pPar->eLink == IFX_ETHSW_PORT_LINK_DOWN )
                {
                    // process port
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 0) != IFX_SUCCESS)
                        return IFX_ERROR;
                    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 1) != IFX_SUCCESS)
                        return IFX_ERROR;
                    // process gphy
                    IFX_PSB6970_PHY_PDN_Set(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID);
                    pEthSWDevHandle->PortConfig[portIdx].bPHYDown = IFX_TRUE;
                }
                else
                    return IFX_ERROR;
            }
            else
            {
                // process port
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLP, portIdx, 0) != IFX_SUCCESS)
                    return IFX_ERROR;
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FLD, portIdx, 0) != IFX_SUCCESS)
                    return IFX_ERROR;
                // process phy
                IFX_PSB6970_PHY_PDN_Clear(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID);
                pEthSWDevHandle->PortConfig[portIdx].bPHYDown = IFX_FALSE;
            }

            // write MII mode
            if (pPar->eMII_Mode == IFX_ETHSW_PORT_HW_MII )
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1IS, 0, 1) != IFX_SUCCESS)
                   return IFX_ERROR;
            }
            else if (pPar->eMII_Mode == IFX_ETHSW_PORT_HW_RMII)
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1IS, 0, 3) != IFX_SUCCESS)
                   return IFX_ERROR;
            }
            else if (pPar->eMII_Mode == IFX_ETHSW_PORT_HW_RGMII )
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1IS, 0, 0) != IFX_SUCCESS)
                   return IFX_ERROR;
            }
            else
               return IFX_ERROR;
            // 0: input, 1: output
            if ( pPar->eClkMode == IFX_ETHSW_PORT_CLK_SLAVE )
                value = 0;
            else if ( pPar->eClkMode == IFX_ETHSW_PORT_CLK_MASTER )
                value = 1;
            else
                return IFX_ERROR;

            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1CKIO, 0, value) != IFX_SUCCESS)
                return IFX_ERROR;
        }
    }
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortPHY_AddrGet(IFX_void_t *pDevCtx, IFX_ETHSW_portPHY_Addr_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, GLOBAL_PHYBA, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    // Request device configuration. Provide this information to the calling application.
    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G)
    {
#if  defined(AR9)
        if ( portIdx == 3 || portIdx == 5 || portIdx == 6 )
            return IFX_ERROR;

        if ( value == IFX_FALSE )
            pPar->nAddressDev = portIdx;
        else
            pPar->nAddressDev = portIdx + 8;
#elif defined(DANUBE)
        if ( portIdx >= 5 )
            return IFX_ERROR;
        pPar->nAddressDev = portIdx;
#endif
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)
    {
#if  defined(AMAZON_SE)
        if ( portIdx >= 4 )
            return IFX_ERROR;
        pPar->nAddressDev = portIdx;
#endif
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        if (portIdx == 1)
            pPar->nAddressDev = 0x11;
        else
            return IFX_ERROR;
    }
    else
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortPHY_Query(IFX_void_t *pDevCtx, IFX_ETHSW_portPHY_Query_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    //IFX_uint32_t value;

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register commands
    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G)
    {
#if 0
        value = IFX_PSB6970_PHYID_Get(pEthSWDevHandle, portIdx);
        if ( value == 0xFFFF )
            pPar->bPHY_Present = IFX_FALSE;
        else
        {
            pPar->bPHY_Present = IFX_TRUE;
        }
#endif

#if defined(AR9)
        if ( portIdx == 3 || portIdx == 5 || portIdx == 6)
            pPar->bPHY_Present = IFX_FALSE;
        else
            pPar->bPHY_Present = IFX_TRUE;
#elif defined(DANUBE)
        if ( portIdx > 4)
            pPar->bPHY_Present = IFX_FALSE;
        else
            pPar->bPHY_Present = IFX_TRUE;
#endif
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)
    {
#if defined(AMAZON_SE)
        if ( portIdx > 3)
            pPar->bPHY_Present = IFX_FALSE;
        else
            pPar->bPHY_Present = IFX_TRUE;
#endif
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
#if 0
        value = IFX_PSB6970_PHYID_Get(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_PHY_ID_BASE + portIdx);
        if ( value == 0xFFFF )
            pPar->bPHY_Present = IFX_FALSE;
        else
        {
            pPar->bPHY_Present = IFX_TRUE;
        }
#endif
        if ( portIdx == 1)
            pPar->bPHY_Present = IFX_TRUE;
        else
            pPar->bPHY_Present = IFX_FALSE;
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)
    {
#if defined(AMAZON_SE)
        if ( portIdx > 3)
            pPar->bPHY_Present = IFX_FALSE;
        else
            pPar->bPHY_Present = IFX_TRUE;
#endif
    }
    else
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortRGMII_ClkCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portRGMII_ClkCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on Tantos3G\n");
        return IFX_ERROR;
    }

    portIdx = pPar->nPortId;

    // If nDelayRx >= 8
    //  use 2ns
    //  else if nDelayRx == 7
    //      use 1.75ns
    //      else if nDelayRx == 6
    //          use 1.5ns
    //          else if nDelayRx == 2
    //              use 0.5ns
    //              ese
    //                  use 0ns
    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // Translate the requested configuration into device specific register commands
    if ( portIdx == 0)
    {
        // Request device configuration. Provide this information to the calling application.
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P0RDLY, 0, &value)!= IFX_SUCCESS)
            return IFX_ERROR;
        if ( value == 0 )
            pPar->nDelayRx = 0;
        else if ( value == 1 )
            pPar->nDelayRx = 6;
        else if ( value == 2 )
            pPar->nDelayRx = 7;
        else if ( value == 3 )
            pPar->nDelayRx = 8;

        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P0TDLY, 0, &value)!= IFX_SUCCESS)
            return IFX_ERROR;
        if ( value == 0 )
            pPar->nDelayTx = 0;
        else if ( value == 1 )
            pPar->nDelayTx = 6;
        else if ( value == 2 )
            pPar->nDelayTx = 7;
        else if ( value == 3 )
            pPar->nDelayTx = 8;
    }
    else if ( portIdx == 1)
    {
        // Request device configuration. Provide this information to the calling application.
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1RDLY, 0, &value)!= IFX_SUCCESS)
            return IFX_ERROR;
        if ( value == 0 )
            pPar->nDelayRx = 0;
        else if ( value == 1 )
            pPar->nDelayRx = 6;
        else if ( value == 2 )
            pPar->nDelayRx = 7;
        else if ( value == 3 )
            pPar->nDelayRx = 8;

        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1TDLY, 0, &value)!= IFX_SUCCESS)
            return IFX_ERROR;
        if ( value == 0 )
            pPar->nDelayTx = 0;
        else if ( value == 1 )
            pPar->nDelayTx = 6;
        else if ( value == 2 )
            pPar->nDelayTx = 7;
        else if ( value == 3 )
            pPar->nDelayTx = 8;
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported.\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortRGMII_ClkCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portRGMII_ClkCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t nDelayRx, nDelayTx;
    IFX_uint8_t portIdx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on Tantos3G\n");
        return IFX_ERROR;
    }

    portIdx = pPar->nPortId;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    // If nDelayRx >= 8
    //  use 2ns
    //  else if nDelayRx == 7
    //      use 1.75ns
    //      else if nDelayRx == 6
    //          use 1.5ns
    //          else if nDelayRx == 2
    //              use 0.5ns
    //              ese
    //                  use 0ns
    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.
    if ( pPar->nDelayRx == 8 )
        nDelayRx = 3;
    else if ( pPar->nDelayRx == 7 )
        nDelayRx = 2;
    else if ( pPar->nDelayRx == 6 )
        nDelayRx = 1;
    else if ( pPar->nDelayRx == 0 )
        nDelayRx = 0;
    else
    {
        //IFXOS_PRINT_INT_RAW(  "Delay value should be 0, 6 (1.5ns), 7 (1.75ns) or 8 (2ns)\n");
        return IFX_ERROR;
    }

    if ( pPar->nDelayTx == 8 )
        nDelayTx = 3;
    else if ( pPar->nDelayTx == 7 )
        nDelayTx = 2;
    else if ( pPar->nDelayTx == 6 )
        nDelayTx = 1;
    else if ( pPar->nDelayTx == 0 )
        nDelayTx = 0;
    else
    {
        //IFXOS_PRINT_INT_RAW(  "Delay value should be 0, 6 (1.5ns), 7 (1.75ns) or 8 (2ns)\n");
        return IFX_ERROR;
    }

    if ( portIdx == 0)
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P0RDLY, 0, nDelayRx )!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P0TDLY, 0, nDelayTx )!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if ( portIdx == 1)
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1RDLY, 0, nDelayRx )!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RGMII_GMII_P1TDLY, 0, nDelayTx )!= IFX_SUCCESS)
            return IFX_ERROR;

    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not Supported.\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortRedirectGet(IFX_void_t *pDevCtx, IFX_ETHSW_portRedirectCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint32_t value = 0;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on Tantos3G\n");
        return IFX_ERROR;
    }
    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on Tantos0G\n");
        return IFX_ERROR;
    }

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    /* read bRedirectEgress from Px_CTL_REG[0004].REDIR[13] bit  */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_REDIR, portIdx, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bRedirectEgress = value;

    /* read bRedirectIngress from Px_CTL_REG[0004].DFWD[19] bit  */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_DFWD, portIdx, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bRedirectIngress = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PortRedirectSet(IFX_void_t *pDevCtx, IFX_ETHSW_portRedirectCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint32_t value = 0;

    /* Check internal or external switch device ? */
    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G )
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on Tantos3G\n");
        return IFX_ERROR;
    }

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G )
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on Tantos0G\n");
        return IFX_ERROR;
    }

    if(portIdx >= 2 )
        return IFX_ERROR;

    /* write bRedirectEgress to Px_CTL_REG[0004].REDIR[13] bit  */
    value = pPar->bRedirectEgress;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_REDIR, portIdx, value) != IFX_SUCCESS)
        return IFX_ERROR;

    /* write bRedirectIngress to Px_CTL_REG[0004].REDIR[19] bit  */
    value = pPar->bRedirectIngress;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_DFWD, portIdx, value) != IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}


IFX_return_t IFX_PSB6970_RMON_Clear(IFX_void_t *pDevCtx, IFX_ETHSW_RMON_clear_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint16_t CAC_Command=0;
    IFX_uint32_t value;
    IFX_uint16_t nTimeOutCnt;


    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    nTimeOutCnt = 0;
    do {
       /*  Poll the busy BAS bit till the hardware is ready to take a new command */
       if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_BAS, 0, &value) != IFX_SUCCESS)
           return IFX_ERROR;
       nTimeOutCnt++;
    }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
    if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
        IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        /* write Reset Port Counter Command to CAC[10:9] bits */
        CAC_Command=SET_BITS(CAC_Command, AR9_RMON_CTL_REG_CAC_SHIFT+AR9_RMON_CTL_REG_CAC_SIZE-1,
                             AR9_RMON_CTL_REG_CAC_SHIFT, IFX_PSB6970_CAC_RESET_PORT_COUNTER);

        /* write portIdx to PORTC[8:6] bits */
        CAC_Command=SET_BITS(CAC_Command, AR9_RMON_CTL_REG_PORTC_SHIFT+AR9_RMON_CTL_REG_PORTC_SIZE-1,
                             AR9_RMON_CTL_REG_PORTC_SHIFT, portIdx);

        /* write busy/access start to BAS[11] bits */
        CAC_Command=SET_BITS(CAC_Command, AR9_RMON_CTL_REG_BAS_SHIFT, AR9_RMON_CTL_REG_BAS_SHIFT, IFX_TRUE);
        /*  start the data transmission */
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_RMON_CTL_REG_CAC_OFFSET, CAC_Command);
    }
    else
    {
        /* write Reset Port Counter Command to CAC[10:9] bits */
        CAC_Command=SET_BITS(CAC_Command, TANTOS_3G_RCC_CAC_SHIFT+TANTOS_3G_RCC_CAC_SIZE-1,
                                TANTOS_3G_RCC_CAC_SHIFT, IFX_PSB6970_CAC_RESET_PORT_COUNTER);

        /* write portIdx to PORTC[8:6] bits */
        CAC_Command=SET_BITS(CAC_Command, TANTOS_3G_RCC_PORTC_SHIFT+TANTOS_3G_RCC_PORTC_SIZE-1,
                                TANTOS_3G_RCC_PORTC_SHIFT, portIdx);

        /* write offset to OFFSET[5:0] bits */
        // if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, RMON_PORTC, 0, offset) != IFX_SUCCESS)
        //  return IFX_ERROR;

        /* write busy/access start to BAS[11] bits */
        CAC_Command=SET_BITS(CAC_Command, TANTOS_3G_RCC_BAS_SHIFT, TANTOS_3G_RCC_BAS_SHIFT, IFX_TRUE);
        /*  start the data transmission */
        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_RCC_CAC_OFFSET, CAC_Command);

    }


    return IFX_SUCCESS;
}

IFX_uint32_t IFX_PSB6970_RMON_Get_By_Offset(IFX_void_t *pDevCtx, IFX_uint8_t portIdx, IFX_uint8_t offset)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint16_t CAC_Command=0;
    IFX_uint32_t value, counter;
    IFX_uint16_t nTimeOutCnt;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        nTimeOutCnt = 0;
        do {
           /*  Poll the busy RMON_BAS bit till the hardware is ready to take a new command */
           if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_BAS, 0, &value) != IFX_SUCCESS)
               return IFX_ERROR;
           nTimeOutCnt++;
        }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
        if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
            IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

        CAC_Command=SET_BITS(CAC_Command, (AR9_RMON_CTL_REG_CAC_SHIFT+AR9_RMON_CTL_REG_CAC_SIZE-1),
                                AR9_RMON_CTL_REG_CAC_SHIFT, IFX_PSB6970_CAC_INDIRECT_READ);

        CAC_Command=SET_BITS(CAC_Command, (AR9_RMON_CTL_REG_PORTC_SHIFT+AR9_RMON_CTL_REG_PORTC_SIZE-1),
                                AR9_RMON_CTL_REG_PORTC_SHIFT, portIdx);

        CAC_Command=SET_BITS(CAC_Command, (AR9_RMON_CTL_REG_OFFSET_SHIFT+AR9_RMON_CTL_REG_OFFSET_SIZE-1),
                                AR9_RMON_CTL_REG_OFFSET_SHIFT, offset);

        /*  start the data transmission */
        CAC_Command=SET_BITS(CAC_Command, AR9_RMON_CTL_REG_BAS_SHIFT, AR9_RMON_CTL_REG_BAS_SHIFT, IFX_TRUE);

        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, AR9_RMON_CTL_REG_CAC_OFFSET, CAC_Command);

        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_COUNTER, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
    #ifdef DEBUG_RMON
        IFXOS_PRINT_INT_RAW(  "\n%s[%d]: RMON_COUNTER = 0x%x\n",__func__,__LINE__, value);
    #endif
        counter = value;

    }
    else
    {
        nTimeOutCnt = 0;
        do {
           /*  Poll the busy RMON_BAS bit till the hardware is ready to take a new command */
           if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_BAS, 0, &value) != IFX_SUCCESS)
               return IFX_ERROR;
           nTimeOutCnt++;
        }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
        if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
            IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

        /* write Indirect Read Command to RCC[11B].CAC[10:9] bits */
        //CAC_Command=SET_BITS(CAC_Command, 10, 9, IFX_PSB6970_CAC_INDIRECT_READ);
        CAC_Command=SET_BITS(CAC_Command, (TANTOS_3G_RCC_CAC_SHIFT+TANTOS_3G_RCC_CAC_SIZE-1),
                                TANTOS_3G_RCC_CAC_SHIFT, IFX_PSB6970_CAC_INDIRECT_READ);
                                //TANTOS_3G_RCC_CAC_SHIFT, IFX_PSB6970_CAC_GET_PORT_COUNTER);

        /* write portIdx to RCC[11B].PORTC[8:6] bits */
        //CAC_Command=SET_BITS(CAC_Command, 8, 6, portIdx);
        CAC_Command=SET_BITS(CAC_Command, (TANTOS_3G_RCC_PORTC_SHIFT+TANTOS_3G_RCC_PORTC_SIZE-1),
                                TANTOS_3G_RCC_PORTC_SHIFT, portIdx);

        /* write offset to RCC[11B].OFFSET[5:0] bits */
        //CAC_Command=SET_BITS(CAC_Command, 5, 0, offset);
        CAC_Command=SET_BITS(CAC_Command, (TANTOS_3G_RCC_OFFSET_SHIFT+TANTOS_3G_RCC_OFFSET_SIZE-1),
                                TANTOS_3G_RCC_OFFSET_SHIFT, offset);

        /*  start the data transmission */
        //CAC_Command=SET_BITS(CAC_Command, 11, 11, 1);
        CAC_Command=SET_BITS(CAC_Command, TANTOS_3G_RCC_BAS_SHIFT, TANTOS_3G_RCC_BAS_SHIFT, IFX_TRUE);

        IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, TANTOS_3G_RCC_CAC_OFFSET, CAC_Command);

        nTimeOutCnt = 0;
        do {
           /*  Poll the busy RMON_BAS bit till the hardware is ready to take a new command */
           if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_BAS, 0, &value) != IFX_SUCCESS)
               return IFX_ERROR;
           nTimeOutCnt++;
        }while (value != 0 && nTimeOutCnt < IFX_PSB6970_TIMEOUTCOUNT);
        if (nTimeOutCnt >= IFX_PSB6970_TIMEOUTCOUNT)
            IFXOS_PRINT_INT_RAW(  "\n %s[%d]: Time is up!\n\n",__func__,__LINE__);

        /* read Low Register from RCSL[11C].COUNTER[15:0] bits  */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_LOW_COUNTER, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        counter = value;
    #ifdef DEBUG_RMON
        IFXOS_PRINT_INT_RAW(  "\n%s[%d]: RMON_LOW_COUNTER = 0x%x\n",__func__,__LINE__, value);
    #endif

        /* read High Register from RCSL[11D].COUNTER[15:0] bits  */
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, RMON_HIGH_COUNTER, 0, &value) != IFX_SUCCESS)
            return IFX_ERROR;
        counter &= 0x0000FFFF;
        counter |= value << 16;
    #ifdef DEBUG_RMON
        IFXOS_PRINT_INT_RAW(  "\n%s[%d]: RMON_HIGH_COUNTER = 0x%x\n",__func__,__LINE__, value);
    #endif

    }
    return counter;
}

IFX_return_t IFX_PSB6970_RMON_Get(IFX_void_t *pDevCtx, IFX_ETHSW_RMON_cnt_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx = pPar->nPortId;
    IFX_uint32_t value_low, value_high;
    IFX_uint64_t counter;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    pPar->nRxUnicastPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x00);
    pPar->nRxBroadcastPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x01);
    pPar->nRxMulticastPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x02);
    pPar->nRxGoodPkts = pPar->nRxUnicastPkts + pPar->nRxBroadcastPkts + pPar->nRxMulticastPkts;
    pPar->nRxFCSErrorPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x03);
    pPar->nRxUnderSizeGoodPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x04);
    pPar->nRxOversizeGoodPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x05);
    pPar->nRxUnderSizeErrorPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x06);
    pPar->nRxGoodPausePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x07);
    pPar->nRxOversizeErrorPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x08);
    pPar->nRxAlignErrorPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x09);
    pPar->nRxFilteredPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x0a);
    pPar->nRx64BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x0b);
    pPar->nRx127BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x0c);
    pPar->nRx255BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x0d);
    pPar->nRx511BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x0e);
    pPar->nRx1023BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x0f);
    pPar->nRxMaxBytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x10);
    pPar->nTxUnicastPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x11);
    pPar->nTxBroadcastPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x12);
    pPar->nTxMulticastPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x13);
    pPar->nTxGoodPkts = pPar->nTxUnicastPkts + pPar->nTxBroadcastPkts + pPar->nTxMulticastPkts;
    pPar->nTxSingleCollCount = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x14);
    pPar->nTxMultCollCount = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x15);
    pPar->nTxLateCollCount = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x16);
    pPar->nTxExcessCollCount = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x17);
    pPar->nTxCollCount = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x18);
    pPar->nTxPauseCount = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x19);
    pPar->nTx64BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x1a);
    pPar->nTx127BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x1b);
    pPar->nTx255BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x1c);
    pPar->nTx511BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x1d);
    pPar->nTx1023BytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x1e);
    pPar->nTxMaxBytePkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x1f);
    pPar->nTxDroppedPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x20);
    pPar->nTxAcmDroppedPkts = 0; // Not supported
    pPar->nRxDroppedPkts = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x21);

    value_low = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x23);
    value_high = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x22);
    counter = value_high;
    counter = counter << 32;
    counter = counter + value_low;
    pPar->nRxGoodBytes = counter;

    value_low = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x25);
    value_high = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x24);
    counter = value_high;
    counter = counter << 32;
    counter = counter + value_low;
    pPar->nRxBadBytes = counter;

    value_low = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x27);
    value_high = IFX_PSB6970_RMON_Get_By_Offset(pEthSWDevHandle, portIdx, 0x26);
    counter = value_high;
    counter = counter << 32;
    counter = counter + value_low;
    pPar->nTxGoodBytes = counter;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VersionGet(IFX_void_t *pDevCtx, IFX_ETHSW_version_t *pPar)
{
   if (pPar->nId >= (sizeof(ver)/sizeof(SWAPI_API_VERSION_GET_t)))
      return IFX_ERROR;
	memset(pPar, 0x00, sizeof(IFX_ETHSW_version_t));

   strcpy(pPar->cName, ver[pPar->nId].cName);
   strcpy(pPar->cVersion, ver[pPar->nId].cVersion);
	return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WoL_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_Cfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported.\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WoL_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_Cfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported.\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WoL_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_PortCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported.\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_WoL_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_PortCfg_t *pPar)
{
    IFXOS_PRINT_INT_RAW(  "Not Supported.\n");
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_Reset(IFX_void_t *pDevCtx, IFX_PSB6970_reset_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t phyID;

    // only support IFX_PSB6970_RESET_EPHY
    if ( pPar->eReset == IFX_PSB6970_RESET_EPHY )
    {
        if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ) // Tantos 3G
        {
            for (phyID=0; phyID<5; phyID++ )
            {
                if (IFX_PSB6970_PHY_Reset(pEthSWDevHandle, phyID) != IFX_SUCCESS )
                    return IFX_ERROR;
            }
        }
        else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G ) // Tantos 0G
        {
            for (phyID=0; phyID<4; phyID++ )
            {
                if (IFX_PSB6970_PHY_Reset(pEthSWDevHandle, phyID) != IFX_SUCCESS )
                    return IFX_ERROR;
            }
        }
        else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9 ) // AR9 internal switch
        {
            if (IFX_PSB6970_PHY_Reset(pEthSWDevHandle, IFX_PSB6970_INTERNAL_SWITCH_EXTERNAL_GPHY_ID) != IFX_SUCCESS )
                return IFX_ERROR;
        }
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "Not supported ! \n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to get vlan table index by using vid

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param vid This parameter is a vlan id.

   \return Return value as follows:
   - vlan table index: if avariable.
   - IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE: if unavariable.
*/
IFX_uint8_t IFX_PSB6970_MFC_Index_Find ( IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcMatchField_t *pPar )
{
    IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t i, index = IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE;
    IFX_PSB6970_QoS_MfPrioClassfields_t FieldSelection;

    FieldSelection = pPar->eFieldSelection;
    if (FieldSelection == IFX_PSB6970_QOS_MF_ETHERTYPE)
    {
        IFX_uint16_t nEtherType;
        nEtherType = pPar->nEtherType;
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i += 1 )
        {
            if (nEtherType == pEthSWDev->MFC_etherTypeEntrys[i].nVCET && pEthSWDev->MFC_etherTypeEntrys[i].bMF_ethertype == IFX_TRUE )
            {
               index = i;
               break;
            }
        }
    }
    else if (FieldSelection == IFX_PSB6970_QOS_MF_PROTOCOL)
    {
        IFX_uint8_t nProtocol;
        nProtocol = pPar->nProtocol;
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i += 1 )
        {
            if (nProtocol == pEthSWDev->MFC_protocolEntrys[i].nPFR &&
                pEthSWDev->MFC_protocolEntrys[i].bMF_protocol == IFX_TRUE )
            {
               index = i;
               break;
            }
        }

    }
    else if (FieldSelection == IFX_PSB6970_QOS_MF_SRCPORT)
    {
        IFX_uint16_t   nBasePt;
        IFX_uint8_t    nPRange;
        nBasePt = pPar->nPortSrc;
        nPRange = pPar->nPortSrcRange;
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i += 1 )
        {
            if (nBasePt == pEthSWDev->MFC_portEntrys[i].nBasePt &&
                nPRange == pEthSWDev->MFC_portEntrys[i].nPRange &&
                pEthSWDev->MFC_portEntrys[i].bPortSrc == IFX_TRUE &&
                pEthSWDev->MFC_portEntrys[i].bMF_port == IFX_TRUE )
            {
               index = i;
               break;
            }
        }
    }
    else if (FieldSelection == IFX_PSB6970_QOS_MF_DSTPORT)
    {
        IFX_uint16_t   nBasePt;
        IFX_uint8_t    nPRange;
        nBasePt = pPar->nPortDst;
        nPRange = pPar->nPortDstRange;
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i += 1 )
        {
            if (nBasePt == pEthSWDev->MFC_portEntrys[i].nBasePt &&
                nPRange == pEthSWDev->MFC_portEntrys[i].nPRange &&
                pEthSWDev->MFC_portEntrys[i].bPortSrc == IFX_FALSE &&
                pEthSWDev->MFC_portEntrys[i].bMF_port == IFX_TRUE )
            {
               index = i;
               break;
            }
        }
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "FieldSelection Error!\n");
        return IFX_ERROR;

    }

    return index;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Index_Find  ----- */


/**
   This is the switch core layer function to check vlan table entry avariable.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - vlan table index: if avariable.
   - IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE: if unavariable.
*/
IFX_uint8_t IFX_PSB6970_MFC_Entry_Avariable ( IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcMatchField_t *pPar )
{
    IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t i, index = IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE;
    IFX_PSB6970_QoS_MfPrioClassfields_t FieldSelection;

    FieldSelection = pPar->eFieldSelection;
    if (FieldSelection == IFX_PSB6970_QOS_MF_ETHERTYPE)
    {
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
        {
            if (pEthSWDev->MFC_etherTypeEntrys[i].bMF_ethertype == IFX_FALSE)
            {
               index = i;
               break;
            }
        }
    }
    else if (FieldSelection == IFX_PSB6970_QOS_MF_PROTOCOL)
    {
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
        {
            if (pEthSWDev->MFC_protocolEntrys[i].bMF_protocol == IFX_FALSE)
            {
               index = i;
               break;
            }
        }

    }
    else if ( FieldSelection == IFX_PSB6970_QOS_MF_SRCPORT ||
             FieldSelection == IFX_PSB6970_QOS_MF_DSTPORT)
    {
        for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
        {
            if (pEthSWDev->MFC_portEntrys[i].bMF_port == IFX_FALSE)
            {
               index = i;
               break;
            }
        }

    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "FieldSelection Error!\n");
        return IFX_ERROR;

    }
    return index;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Entry_Avariable  ----- */

IFX_return_t IFX_PSB6970_QoS_MfcAdd(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;
    IFX_uint8_t entry_index, i;
    IFX_PSB6970_QoS_MfcCfg_t *ptr;

    if (pPar->sFilterInfo.ePortForward > IFX_ETHSW_PORT_FORWARD_CPU )
    {
        IFXOS_PRINT_INT_RAW(  "Out of range on ePortForward parameter!\n");
        return IFX_ERROR;
    }

    // Request device configuration. Provide this information to the calling application.
    if (pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_ETHERTYPE )
    {
        // check if VCET exist ?
        if (IFX_PSB6970_MFC_Index_Find(pEthSWDevHandle, &(pPar->sFilterMatchField)) != IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
        {
            IFXOS_PRINT_INT_RAW(  "This rule exists\n");
            return IFX_ERROR;
        }

        // get avariable mfc entry index
        entry_index = IFX_PSB6970_MFC_Entry_Avariable(pEthSWDevHandle, &(pPar->sFilterMatchField));
        if ( entry_index == IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
        {
            IFXOS_PRINT_INT_RAW(  "There is no entry avariable\n");
            return IFX_ERROR;
        }

        // write to mirror table
        pEthSWDevHandle->MFC_etherTypeEntrys[entry_index].bMF_ethertype = IFX_TRUE;
        pEthSWDevHandle->MFC_etherTypeEntrys[entry_index].nVCET = pPar->sFilterMatchField.nEtherType;

        // write to hardware register
        value = pPar->sFilterMatchField.nEtherType;
        if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G||   // Tantos 3G
             pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET_ALL, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)  // AR9
        {
            if ( entry_index == 0 || entry_index == 2 || entry_index == 4 || entry_index == 6 )
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET0, entry_index/2, value)!= IFX_SUCCESS)
                    return IFX_ERROR;
            }
            else if ( entry_index == 1 || entry_index == 3 || entry_index == 5 || entry_index == 7 )
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET1, (entry_index-1)/2, value)!= IFX_SUCCESS)
                    return IFX_ERROR;
            }
        }
        value = pPar->sFilterInfo.ePortForward;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_ATF, entry_index, value)!= IFX_SUCCESS)
            return IFX_ERROR;

        value = pPar->sFilterInfo.nTrafficClass;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_QTF, entry_index, value)!= IFX_SUCCESS)
            return IFX_ERROR;

        // write to rules table
        for (i=0; i<IFX_PSB6970_MFC_RULES_MAX; i++)
        {
            if ((pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection == IFX_PSB6970_MFC_ENTRY_NOEXIST ))
                break;
        }
        ptr = &(pEthSWDevHandle->MFC_RulesEntrys[i]);
        ptr->sFilterMatchField.eFieldSelection = IFX_PSB6970_QOS_MF_ETHERTYPE;
        ptr->sFilterMatchField.nEtherType = pPar->sFilterMatchField.nEtherType;
        ptr->sFilterInfo.ePortForward = pPar->sFilterInfo.ePortForward;
        ptr->sFilterInfo.nTrafficClass = pPar->sFilterInfo.nTrafficClass;
    }
    else if (pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_PROTOCOL )
    {
        // check if VCET exist ?
        if (IFX_PSB6970_MFC_Index_Find(pEthSWDevHandle, &(pPar->sFilterMatchField)) != IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
        {
            IFXOS_PRINT_INT_RAW(  "This rules exists\n");
            return IFX_ERROR;
        }

        // get avariable mfc entry index
        entry_index = IFX_PSB6970_MFC_Entry_Avariable(pEthSWDevHandle, &(pPar->sFilterMatchField));
        if ( entry_index == IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
        {
            IFXOS_PRINT_INT_RAW(  "There is no entry avariable\n");
            return IFX_ERROR;
        }

        // write to mirror table
        pEthSWDevHandle->MFC_protocolEntrys[entry_index].bMF_protocol = IFX_TRUE;
        pEthSWDevHandle->MFC_protocolEntrys[entry_index].nPFR = pPar->sFilterMatchField.nProtocol;

#ifdef DEBUG
    IFXOS_PRINT_INT_RAW(  "DEBUG> entry_index=%d, nProtocol=%d, nPFR=%d\n",
        entry_index,
        pPar->sFilterMatchField.nProtocol,
        pEthSWDevHandle->MFC_protocolEntrys[entry_index].nPFR);
#endif

        // write to hardware register
        value = pPar->sFilterMatchField.nProtocol;
        if (entry_index == 0 || entry_index == 2|| entry_index == 4 || entry_index == 6 )
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_PFR0, entry_index/2, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (entry_index == 1 || entry_index == 3|| entry_index == 5 || entry_index == 7 )
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_PFR1, (entry_index-1)/2, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else
            return IFX_ERROR;

        value = pPar->sFilterInfo.ePortForward;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_APF, entry_index, value)!= IFX_SUCCESS)
            return IFX_ERROR;
        // write to rules table
        for (i=0; i<IFX_PSB6970_MFC_RULES_MAX; i++)
        {
            if ((pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection == IFX_PSB6970_MFC_ENTRY_NOEXIST ))
                break;
        }
        ptr = &(pEthSWDevHandle->MFC_RulesEntrys[i]);
        ptr->sFilterMatchField.eFieldSelection = IFX_PSB6970_QOS_MF_PROTOCOL;
        ptr->sFilterMatchField.nProtocol = pPar->sFilterMatchField.nProtocol;
        ptr->sFilterInfo.ePortForward = pPar->sFilterInfo.ePortForward;
    }
    else if (pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_SRCPORT ||
        pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_DSTPORT )
    {
        // check if VCET exist ?
        if (IFX_PSB6970_MFC_Index_Find(pEthSWDevHandle, &(pPar->sFilterMatchField)) != IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
        {
            IFXOS_PRINT_INT_RAW(  "This ether type exists\n");
            return IFX_ERROR;
        }

        // get avariable mfc entry index
        entry_index = IFX_PSB6970_MFC_Entry_Avariable(pEthSWDevHandle, &(pPar->sFilterMatchField));
        if ( entry_index == IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
        {
            IFXOS_PRINT_INT_RAW(  "There is no entry avariable\n");
            return IFX_ERROR;
        }

        // write to mirrored table
        pEthSWDevHandle->MFC_portEntrys[entry_index].bMF_port = IFX_TRUE;
        if (pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_SRCPORT)
        {
            pEthSWDevHandle->MFC_portEntrys[entry_index].bPortSrc = IFX_TRUE;
            pEthSWDevHandle->MFC_portEntrys[entry_index].nBasePt =
                pPar->sFilterMatchField.nPortSrc;
            pEthSWDevHandle->MFC_portEntrys[entry_index].nPRange =
                pPar->sFilterMatchField.nPortSrcRange;
        }
        else
        {
            pEthSWDevHandle->MFC_portEntrys[entry_index].bPortSrc = IFX_FALSE;
            pEthSWDevHandle->MFC_portEntrys[entry_index].nBasePt =
                pPar->sFilterMatchField.nPortDst;
            pEthSWDevHandle->MFC_portEntrys[entry_index].nPRange =
                pPar->sFilterMatchField.nPortDstRange;
        }

        // write to hardware register
        if (pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_SRCPORT)
        {
            value = 2; //src port
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_COMP, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
            value = pPar->sFilterMatchField.nPortSrc;
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_BASEPT, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
            value = pPar->sFilterMatchField.nPortSrcRange;
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_PRANGE, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else
        {
            value = 1; //dst port
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_COMP, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
            value = pPar->sFilterMatchField.nPortDst;
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_BASEPT, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
            value = pPar->sFilterMatchField.nPortDstRange;
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_PRANGE, entry_index, value)!= IFX_SUCCESS)
                return IFX_ERROR;
        }

        value = pPar->sFilterInfo.ePortForward;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_ATUF, entry_index, value)!= IFX_SUCCESS)
            return IFX_ERROR;

        value = pPar->sFilterInfo.nTrafficClass;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_TUPF, entry_index, value)!= IFX_SUCCESS)
            return IFX_ERROR;

        // write to rules table
        for (i=0; i<IFX_PSB6970_MFC_RULES_MAX; i++)
        {
            if ((pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection == IFX_PSB6970_MFC_ENTRY_NOEXIST ))
                break;
        }
        ptr = &(pEthSWDevHandle->MFC_RulesEntrys[i]);
        ptr->sFilterMatchField.eFieldSelection = pPar->sFilterMatchField.eFieldSelection;
        if (pPar->sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_SRCPORT)
        {
            ptr->sFilterMatchField.nPortSrc = pPar->sFilterMatchField.nPortSrc;
            ptr->sFilterMatchField.nPortSrcRange = pPar->sFilterMatchField.nPortSrcRange;
        }
        else
        {
            ptr->sFilterMatchField.nPortDst = pPar->sFilterMatchField.nPortDst;
            ptr->sFilterMatchField.nPortDstRange = pPar->sFilterMatchField.nPortDstRange;
        }
        ptr->sFilterInfo.ePortForward = pPar->sFilterInfo.ePortForward;
        ptr->sFilterInfo.nTrafficClass = pPar->sFilterInfo.nTrafficClass;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MfcEntryRead(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcEntryRead_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_boolean_t found;
    IFX_PSB6970_QoS_MfcCfg_t *pRulesEntry;

    if ( pPar->bInitial == IFX_TRUE )
        pEthSWDevHandle->RulesIndex = 0;

    found = IFX_FALSE;
    do {
        pRulesEntry = &(pEthSWDevHandle->MFC_RulesEntrys[pEthSWDevHandle->RulesIndex]);

        if (pRulesEntry->sFilterMatchField.eFieldSelection != IFX_PSB6970_MFC_ENTRY_NOEXIST)
        {
            pPar->sFilter.sFilterMatchField.nPortSrc=
                    pRulesEntry->sFilterMatchField.nPortSrc;
            pPar->sFilter.sFilterMatchField.nPortDst=
                    pRulesEntry->sFilterMatchField.nPortDst;
            pPar->sFilter.sFilterMatchField.nPortSrcRange=
                    pRulesEntry->sFilterMatchField.nPortSrcRange;
            pPar->sFilter.sFilterMatchField.nPortDstRange=
                    pRulesEntry->sFilterMatchField.nPortDstRange;
            pPar->sFilter.sFilterMatchField.nProtocol=
                    pRulesEntry->sFilterMatchField.nProtocol;
            pPar->sFilter.sFilterMatchField.nEtherType=
                    pRulesEntry->sFilterMatchField.nEtherType;
            pPar->sFilter.sFilterMatchField.eFieldSelection=
                    pRulesEntry->sFilterMatchField.eFieldSelection;
            pPar->sFilter.sFilterInfo.nTrafficClass=
                    pRulesEntry->sFilterInfo.nTrafficClass;
            pPar->sFilter.sFilterInfo.ePortForward=
                    pRulesEntry->sFilterInfo.ePortForward;
            pEthSWDevHandle->RulesIndex++;
            found = IFX_TRUE;
            break;
        }
        else
            pEthSWDevHandle->RulesIndex++;
    } while ( pEthSWDevHandle->RulesIndex < IFX_PSB6970_MFC_RULES_MAX);

    if ( found == IFX_TRUE )
        pPar->bLast = IFX_FALSE;

    if ( pEthSWDevHandle->RulesIndex >= IFX_PSB6970_MFC_RULES_MAX)
    {
        pEthSWDevHandle->RulesIndex = 0;
        pPar->bLast = IFX_TRUE;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MfcDel(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcMatchField_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t entry_index, i;
    IFX_PSB6970_QoS_MfcCfg_t *ptr;

    entry_index = IFX_PSB6970_MFC_Index_Find(pEthSWDevHandle, pPar);
    if (entry_index == IFX_PSB6970_MFC_ENTRY_INDEX_UNAVARIABLE )
    {
        IFXOS_PRINT_INT_RAW(  "This rule doesn't exists\n");
        return IFX_ERROR;
    }

    if (pPar->eFieldSelection == IFX_PSB6970_QOS_MF_ETHERTYPE )
    {
        // clear mirror table
        pEthSWDevHandle->MFC_etherTypeEntrys[entry_index].bMF_ethertype = IFX_FALSE;
        pEthSWDevHandle->MFC_etherTypeEntrys[entry_index].nVCET = 0;

        // clear hardware register
        if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||// Tantos 3G
             pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET_ALL, entry_index, 0)!= IFX_SUCCESS)
                return IFX_ERROR;
        } if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)  // AR9
        {
            if ( entry_index == 0 || entry_index == 2 || entry_index == 4 || entry_index == 6 )
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET0, entry_index/2, 0)!= IFX_SUCCESS)
                    return IFX_ERROR;
            }
            else if ( entry_index == 1 || entry_index == 3 || entry_index == 5 || entry_index == 7 )
            {
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET1, (entry_index-1)/2, 0)!= IFX_SUCCESS)
                    return IFX_ERROR;
            }
        }

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_ATF, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_QTF, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
        // clear rules table
        for (i=0; i<IFX_PSB6970_MFC_RULES_MAX; i++)
        {
            if ((pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection ==
                    IFX_PSB6970_QOS_MF_ETHERTYPE ) &&
                 pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nEtherType ==
                    pPar->nEtherType )
                break;
        }
        ptr = &(pEthSWDevHandle->MFC_RulesEntrys[i]);
        ptr->sFilterMatchField.eFieldSelection = IFX_PSB6970_MFC_ENTRY_NOEXIST;
        ptr->sFilterMatchField.nEtherType = 0;
        ptr->sFilterInfo.ePortForward = 0;
        ptr->sFilterInfo.nTrafficClass = 0;
    }
    else if (pPar->eFieldSelection == IFX_PSB6970_QOS_MF_PROTOCOL )
    {
        // clear mirror table
        pEthSWDevHandle->MFC_protocolEntrys[entry_index].bMF_protocol = IFX_FALSE;
        pEthSWDevHandle->MFC_protocolEntrys[entry_index].nPFR = 0;

        // clear hardware register
        if (entry_index == 0 || entry_index == 2|| entry_index == 4 || entry_index == 6 )
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_PFR0, entry_index/2, 0)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else if (entry_index == 1 || entry_index == 3|| entry_index == 5 || entry_index == 7 )
        {
            if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_PFR1, (entry_index-1)/2, 0)!= IFX_SUCCESS)
                return IFX_ERROR;
        }
        else
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_APF, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        // write to rules table
        for (i=0; i<IFX_PSB6970_MFC_RULES_MAX; i++)
        {
            if ((pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection ==
                    IFX_PSB6970_QOS_MF_PROTOCOL ) &&
                 pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nProtocol ==
                    pPar->nProtocol )
                break;
        }
        ptr = &(pEthSWDevHandle->MFC_RulesEntrys[i]);
        ptr->sFilterMatchField.eFieldSelection = IFX_PSB6970_MFC_ENTRY_NOEXIST;
        ptr->sFilterMatchField.nProtocol = 0;
        ptr->sFilterInfo.ePortForward = 0;
    }
    else if (pPar->eFieldSelection == IFX_PSB6970_QOS_MF_SRCPORT ||
        pPar->eFieldSelection == IFX_PSB6970_QOS_MF_DSTPORT )
    {
        // clear mirror table
        pEthSWDevHandle->MFC_portEntrys[entry_index].bMF_port = IFX_FALSE;
        pEthSWDevHandle->MFC_portEntrys[entry_index].bPortSrc = IFX_FALSE;
        pEthSWDevHandle->MFC_portEntrys[entry_index].nBasePt = 0;
        pEthSWDevHandle->MFC_portEntrys[entry_index].nPRange = 0;

        // clear hardware register
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_COMP, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_BASEPT, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_PRANGE, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_ATUF, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_TUPF, entry_index, 0)!= IFX_SUCCESS)
            return IFX_ERROR;

        // write to rules table
        for (i=0; i<IFX_PSB6970_MFC_RULES_MAX; i++)
        {
            if ( (pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_SRCPORT ) &&
                 ( pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortSrc == pPar->nPortSrc ) &&
                 ( pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortSrcRange == pPar->nPortSrcRange ) )
                break;
            if ( (pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection == IFX_PSB6970_QOS_MF_DSTPORT ) &&
                 ( pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortDst == pPar->nPortDst ) &&
                 ( pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortDstRange == pPar->nPortDstRange ) )
                break;
        }
        ptr = &(pEthSWDevHandle->MFC_RulesEntrys[i]);
        ptr->sFilterMatchField.eFieldSelection = IFX_PSB6970_MFC_ENTRY_NOEXIST;
        ptr->sFilterMatchField.nPortSrc = 0;
        ptr->sFilterMatchField.nPortSrcRange = 0;
        ptr->sFilterMatchField.nPortDst = 0;
        ptr->sFilterMatchField.nPortDstRange = 0;
        ptr->sFilterInfo.nTrafficClass = 0;
        ptr->sFilterInfo.ePortForward = 0;
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_MfcPortCfgGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcPortCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPort;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_TCPE, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bPriorityPort = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_TPE, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bPriorityEtherType = value;

    return IFX_SUCCESS;

}

IFX_return_t IFX_PSB6970_QoS_MfcPortCfgSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcPortCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPort;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->bPriorityPort <= 1 )
    {
        value = pPar->bPriorityPort;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_TCPE, portIdx, value)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "bPriorityPort: out of range !\n");
        return IFX_ERROR;
    }

    if ( pPar->bPriorityEtherType <= 1 )
    {
        value = pPar->bPriorityEtherType;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_TPE, portIdx, value)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
    {
        IFXOS_PRINT_INT_RAW(  "bPriorityEtherType: out of range !\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;

}

IFX_return_t IFX_PSB6970_QoS_PortPolicerGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portPolicerCfg_t *pPar)
{
#if 1
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t itt=0, itr=0;

    // key are portid, traffic class and type
    portIdx = pPar->nPort;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, &itt)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITR, portIdx, &itr)!= IFX_SUCCESS)
        return IFX_ERROR;

    if (itt==0)
        pPar->nRate = itr * 8 / 512;
    else if (itt==1)
        pPar->nRate = itr * 8 / 128;
    else if (itt==2)
        pPar->nRate = itr * 8 / 32;
    else if (itt==3)
        pPar->nRate = itr * 8 / 8;
#else

    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPort;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITR, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->nRate = value;
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortPolicerSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portPolicerCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;

    // Check that configuration parameters are inside the supported range.
    portIdx = pPar->nPort;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, 3)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITR, portIdx, pPar->nRate)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperCfgGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;

    portIdx = pPar->nPort;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_RMWFQ, portIdx, &value)!= IFX_SUCCESS)
        return IFX_ERROR;

    if (value == 0 )
        pPar->eWFQ_Type = IFX_PSB6970_QoS_WFQ_WEIGHT;
    else
        pPar->eWFQ_Type = IFX_PSB6970_QoS_WFQ_RATE;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperCfgSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;

    portIdx = pPar->nPort;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if (pPar->eWFQ_Type == IFX_PSB6970_QoS_WFQ_WEIGHT)
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RMWFQ, portIdx, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else if (pPar->eWFQ_Type == IFX_PSB6970_QoS_WFQ_RATE)
    {
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RMWFQ, portIdx, 1)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
         IFXOS_PRINT_INT_RAW(  "Not Supported eType!\n");


    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperStrictGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperStrictCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t itt=0, itr=0;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;

    portIdx = pPar->nPort;

    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->nTrafficClass == 3 )
        rms = PORT_EGRESS_PSPQ3TR;
    else if ( pPar->nTrafficClass == 2 )
        rms = PORT_EGRESS_PSPQ2TR;
    else if ( pPar->nTrafficClass == 1 )
        rms = PORT_EGRESS_PSPQ1TR;
    else if ( pPar->nTrafficClass == 0 )
        rms = PORT_EGRESS_PSPQ0TR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, &itt)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, rms, portIdx, &itr)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (itt==0)
        pPar->nRate = itr * 8 / 512;
    else if (itt==1)
        pPar->nRate = itr * 8 / 128;
    else if (itt==2)
        pPar->nRate = itr * 8 / 32;
    else if (itt==3)
    {
      if (itr == 0 )
        pPar->nRate = 0;
      else
        //pPar->nRate = (itr * 8 / 8) - 1;
	pPar->nRate = itr * 8 / 8;
    }
#ifdef DEBUG
    IFXOS_PRINT_INT_RAW(  "DEBUG> %s[%d]: entering! itt=%d, itr=%d\n",__func__,__LINE__, itt, itr);
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperStrictSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperStrictCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t Rate;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;


    // Check that configuration parameters are inside the supported range.
    portIdx = pPar->nPort;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->nTrafficClass == 3 )
        rms = PORT_EGRESS_PSPQ3TR;
    else if ( pPar->nTrafficClass == 2 )
        rms = PORT_EGRESS_PSPQ2TR;
    else if ( pPar->nTrafficClass == 1 )
        rms = PORT_EGRESS_PSPQ1TR;
    else if ( pPar->nTrafficClass == 0 )
        rms = PORT_EGRESS_PSPQ0TR;
    else
    {
        IFXOS_PRINT_INT_RAW(  "Out of range on nTrafficClass parameter !\n");
        return IFX_ERROR;
    }

    // use the same Time Tick as ingress queue
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, 3)!= IFX_SUCCESS)
        return IFX_ERROR;

    if (pPar->nRate == 0)
      Rate = 0;
    else
       Rate = pPar->nRate;
      //Rate = pPar->nRate + 1;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, portIdx, Rate)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperWfqGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperWFQ_Cfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t itt=0, itr=0;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;

    portIdx = pPar->nPort;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->nTrafficClass == 3 )
        rms = PORT_EGRESS_PWQ3TR;
    else if ( pPar->nTrafficClass == 2 )
        rms = PORT_EGRESS_PWQ2TR;
    else if ( pPar->nTrafficClass == 1 )
        rms = PORT_EGRESS_PWQ1TR;
    else if ( pPar->nTrafficClass == 0 )
        rms = PORT_EGRESS_PWQ0TR;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, &itt)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, rms, portIdx, &itr)!= IFX_SUCCESS)
        return IFX_ERROR;
    if (itt==0)
        pPar->nRate = itr * 8 / 512;
    else if (itt==1)
        pPar->nRate = itr * 8 / 128;
    else if (itt==2)
        pPar->nRate = itr * 8 / 32;
    else if (itt==3)
        pPar->nRate = itr * 8 / 8;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperWfqSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperWFQ_Cfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;

    portIdx = pPar->nPort;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->nRate > 128 )
    {
        IFXOS_PRINT_INT_RAW(  "Out of range on nRate parameter !\n");
        return IFX_ERROR;
    }

    if ( pPar->nTrafficClass == 3 )
        rms = PORT_EGRESS_PWQ3TR;
    else if ( pPar->nTrafficClass == 2 )
        rms = PORT_EGRESS_PWQ2TR;
    else if ( pPar->nTrafficClass == 1 )
        rms = PORT_EGRESS_PWQ1TR;
    else if ( pPar->nTrafficClass == 0 )
        rms = PORT_EGRESS_PWQ0TR;
    else
    {
        IFXOS_PRINT_INT_RAW(  "Out of range on nTrafficClass parameter !\n");
        return IFX_ERROR;
    }

    // use the same Time Tick as ingress queue
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, 3)!= IFX_SUCCESS)
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, portIdx, pPar->nRate)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

#if 0
IFX_return_t IFX_PSB6970_QoS_PortShaperGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShapterCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    IFX_uint32_t value;
//    IFX_uint32_t itt=0, itr=0;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;

    // key are portid, traffic class and type
    portIdx = pPar->nPort;
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->eType == IFX_ETHSW_QOS_SCHEDULER_STRICT )
    {
        if ( pPar->nTrafficClass == 3 )
            rms = PORT_EGRESS_PSPQ3TR;
        else if ( pPar->nTrafficClass == 2 )
            rms = PORT_EGRESS_PSPQ2TR;
        else if ( pPar->nTrafficClass == 1 )
            rms = PORT_EGRESS_PSPQ1TR;
        else if ( pPar->nTrafficClass == 0 )
            rms = PORT_EGRESS_PSPQ0TR;

#if 0
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, &itt)!= IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, rms, portIdx, &itr)!= IFX_SUCCESS)
            return IFX_ERROR;
        if (itt==0)
            pPar->nRate = itr * 8 / 512;
        else if (itt==1)
            pPar->nRate = itr * 8 / 128;
        else if (itt==2)
            pPar->nRate = itr * 8 / 32;
        else if (itt==3)
            pPar->nRate = itr * 8 / 8;
#else
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, rms, portIdx, &value)!= IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nRate = value;
#endif


    #ifdef DEBUG
        //IFXOS_PRINT_INT_RAW(  "DEBUG> %s[%d]: entering! itt=%d, itr=%d\n",__func__,__LINE__, itt, itr);
    #endif
    }
    else if ( pPar->eType == IFX_ETHSW_QOS_SCHEDULER_WFQ )
    {
        if ( pPar->nTrafficClass == 3 )
            rms = PORT_EGRESS_PWQ3TR;
        else if ( pPar->nTrafficClass == 2 )
            rms = PORT_EGRESS_PWQ2TR;
        else if ( pPar->nTrafficClass == 1 )
            rms = PORT_EGRESS_PWQ1TR;
        else if ( pPar->nTrafficClass == 0 )
            rms = PORT_EGRESS_PWQ0TR;
#if 0
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, rms, portIdx, &itr)!= IFX_SUCCESS)
            return IFX_ERROR;
        pPar->nRate = itr;
#else
        if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, rms, portIdx, &pPar->nRate)!= IFX_SUCCESS)
            return IFX_ERROR;
#endif
    }
    else
         IFXOS_PRINT_INT_RAW(  "Not Supported eType!\n");


    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_PortShaperSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShapterCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t portIdx;
    //IFX_uint32_t itt=0, itr=0;
    IFX_ETHSW_regMapperSelector_t rms=COMMON_BIT_LATEST;

    portIdx = pPar->nPort;

    // Check that configuration parameters are inside the supported range.
    if(portIdx >= pEthSWDevHandle->nPortNumber )
        return IFX_ERROR;

    if ( pPar->nTrafficClass > 3 )
    {
        IFXOS_PRINT_INT_RAW(  "Out of range on nTrafficClass parameter !\n");
        return IFX_ERROR;
    }

    if ( pPar->eType == IFX_ETHSW_QOS_SCHEDULER_STRICT )
    {
#if 0
        if (calcToken(pPar->nRate, &itr, &itt) != IFX_SUCCESS)
            return IFX_ERROR;
#endif

        if ( pPar->nTrafficClass == 3 )
            rms = PORT_EGRESS_PSPQ3TR;
        else if ( pPar->nTrafficClass == 2 )
            rms = PORT_EGRESS_PSPQ2TR;
        else if ( pPar->nTrafficClass == 1 )
            rms = PORT_EGRESS_PSPQ1TR;
        else if ( pPar->nTrafficClass == 0 )
            rms = PORT_EGRESS_PSPQ0TR;

        pEthSWDevHandle->PortConfig[portIdx].eType = IFX_ETHSW_QOS_SCHEDULER_STRICT;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RMWFQ, portIdx, IFX_TRUE)!= IFX_SUCCESS)
            return IFX_ERROR;

        // use the same Time Tick as ingress queue
#if 0
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_INGRESS_PITT, portIdx, itt)!= IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, portIdx, itr)!= IFX_SUCCESS)
            return IFX_ERROR;
#else
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, portIdx, pPar->nRate)!= IFX_SUCCESS)
            return IFX_ERROR;
#endif

        // clear WFQ register
#if 0
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_EGRESS_PWQ3TR, portIdx, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_EGRESS_PWQ2TR, portIdx, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_EGRESS_PWQ1TR, portIdx, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_EGRESS_PWQ0TR, portIdx, 0)!= IFX_SUCCESS)
            return IFX_ERROR;
#endif
#ifdef DEBUG
    //IFXOS_PRINT_INT_RAW(  "DEBUG> %s[%d]: entering! itt=%d, itr=%d\n",__func__,__LINE__, itt, itr);
#endif

    }
    else if ( pPar->eType == IFX_ETHSW_QOS_SCHEDULER_WFQ )
    {
        if ( pPar->nTrafficClass == 3 )
            rms = PORT_EGRESS_PWQ3TR;
        else if ( pPar->nTrafficClass == 2 )
            rms = PORT_EGRESS_PWQ2TR;
        else if ( pPar->nTrafficClass == 1 )
            rms = PORT_EGRESS_PWQ1TR;
        else if ( pPar->nTrafficClass == 0 )
            rms = PORT_EGRESS_PWQ0TR;

        pEthSWDevHandle->PortConfig[portIdx].eType = IFX_ETHSW_QOS_SCHEDULER_WFQ;

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_RMWFQ, portIdx, IFX_FALSE)!= IFX_SUCCESS)
            return IFX_ERROR;

        if ( pPar->nRate > 128 )
        {
            IFXOS_PRINT_INT_RAW(  "Out of range on nRate parameter !\n");
            return IFX_ERROR;
        }

        if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, rms, portIdx, pPar->nRate)!= IFX_SUCCESS)
            return IFX_ERROR;
    }
    else
         IFXOS_PRINT_INT_RAW(  "Not Supported eType!\n");

    return IFX_SUCCESS;
}
#endif

IFX_return_t IFX_PSB6970_QoS_StormGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_stormCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_B, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bBroadcast = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_M, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bMulticast = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_U, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->bUnicast = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_10_TH, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->nThreshold10M = value;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_100_TH, 0, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->nThreshold100M = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_QoS_StormSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_stormCfg_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    // Translate the requested configuration into device specific register
    // commands and write the configuration over Register Access Layer to the device.

    value = pPar->bBroadcast;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_B, 0, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    value = pPar->bMulticast;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_M, 0, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    value = pPar->bUnicast;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_U, 0, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    value = pPar->nThreshold10M;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_10_TH, 0, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    value = pPar->nThreshold100M;
    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, CONGESTION_STORM_100_TH, 0, value)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PowerManagementGet(IFX_void_t *pDevCtx, IFX_PSB6970_powerManagement_t *pPar)
{
#ifdef IFX_ETHSW_API_COC
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on AR9 internal switch\n");
        return IFX_ERROR;
    }

    pPar->bEnable = IFX_ETHSW_PM_powerManegementStatusGet(pEthSWDevHandle->pPMCtx);
#else
    IFXOS_PRINT_INT_RAW(  "Not Supported PowerManagement\n");
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PowerManagementSet(IFX_void_t *pDevCtx, IFX_PSB6970_powerManagement_t *pPar)
{
#ifdef IFX_ETHSW_API_COC
    IFX_boolean_t bStateMachineEnable;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)
    {
        IFXOS_PRINT_INT_RAW(  "This API was not supported on AR9 internal switch\n");
        return IFX_ERROR;
    }

    bStateMachineEnable = IFX_ETHSW_PM_powerManegementStatusGet(pEthSWDevHandle->pPMCtx);

    if (pPar->bEnable == IFX_TRUE)
    {
        if (bStateMachineEnable == IFX_DISABLE)
        {
            if (IFX_ETHSW_powerManagementActivated(pEthSWDevHandle->pPMCtx) == IFX_ERROR)
                return IFX_ERROR;
        }
        else
            IFXOS_PRINT_INT_RAW(  "Power Management already enable\n");
    }
    else
    {
        if (bStateMachineEnable == IFX_ENABLE)
        {
            if (IFX_ETHSW_powerManagementDeActivated(pEthSWDevHandle->pPMCtx) == IFX_ERROR)
                return IFX_ERROR;
        }
        else
            IFXOS_PRINT_INT_RAW(  "Power Management already disable\n");

    }
#else
    IFXOS_PRINT_INT_RAW(  "Not Supported PowerManagement\n");
#endif

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_RegisterGet(IFX_void_t *pDevCtx, IFX_PSB6970_register_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint32_t value;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
    {
        if (  pPar->nRegAddr % 4 != 0 )
            return IFX_ERROR;
    }
    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_RegisterGet(pEthSWDevHandle->pRML_Dev, pPar->nRegAddr, &value)!= IFX_SUCCESS)
        return IFX_ERROR;
    pPar->nData = value;

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_RegisterSet(IFX_void_t *pDevCtx, IFX_PSB6970_register_t *pPar)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
    {
        if (  pPar->nRegAddr % 4 != 0 )
            return IFX_ERROR;
    }
    // Translate the requested configuration into device specific register commands
    if (IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, pPar->nRegAddr, pPar->nData)!= IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

/*
 *  Private Function
 */


IFX_return_t VLAN_Filter_Register_Write(IFX_void_t *pDevCtx, IFX_PSB6970_VLAN_Filter_registerOperation_t *pPar)
{

    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t VLAN_Valid;

    if (pPar->nOP==IFX_PSB6970_VLAN_TABLE_ENTRY_ADD)
       VLAN_Valid = 1;
    else
       VLAN_Valid = 0;


    switch ( pPar->nTable_Index ) {
        case 0:
                // write to VID bits
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, 0, VLAN_Valid) != IFX_SUCCESS)
                    return IFX_ERROR;
                // write to VID bits
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VID, 0, pPar->nVId) != IFX_SUCCESS)
                    return IFX_ERROR;
                // write to FID bits
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VFID, 0, pPar->nFId) != IFX_SUCCESS)
                    return IFX_ERROR;
            break;
        case 1:
                // write to VID bits
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV1, 0, VLAN_Valid) != IFX_SUCCESS)
                    return IFX_ERROR;
                // write to VID bits
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VID1, 0, pPar->nVId) != IFX_SUCCESS)
                    return IFX_ERROR;
                // write to FID bits
                if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VFID1, 0, pPar->nFId) != IFX_SUCCESS)
                    return IFX_ERROR;
            break;

        default:
            break;
    }               /* -----  end switch  ----- */
  return IFX_SUCCESS;
}


/**
   This is the switch core layer function to enable vlan aware.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_awareEnable(IFX_void_t *pDevCtx)
{
    IFX_uint8_t i;
    IFX_ETHSW_VLAN_IdCreate_t vlan_group;
    IFX_ETHSW_VLAN_portMemberAdd_t pm;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

     if (pEthSWDevHandle->bVLAN_Aware == IFX_TRUE)
     {
        IFXOS_PRINT_INT_RAW(  "VLAN_Aware has already enabled\n");
        return IFX_ERROR;
     }
     IFX_PSB6970_VLAN_awareDisable(pEthSWDevHandle);
     /* set bVLAN_Aware variable */
     pEthSWDevHandle->bVLAN_Aware = IFX_TRUE;
     for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
     {
         /* set PxPBVM[03H]:TBVE[13] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_TBVE, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* set PxPBVM[03H]:VC[11] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VC, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* set PxPBVM[03H]:VSD[10] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VSD, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* clear PxPBVM[03H]:AOVTP[9] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_AOVTP, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* set PxPBVM[03H]:VMCE[8] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VMCE, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* clear PxPBVM[03H]:BYPASS[7] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_BYPASS, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;

         //if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, i, 1) != IFX_SUCCESS)
         //   return IFX_ERROR;

         //if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, i, IFX_FALSE) != IFX_SUCCESS)
         //   return IFX_ERROR;
     }
     /* add the first vlan group to vlan filter table */
     vlan_group.nVId = 1;
     vlan_group.nFId = 0;
     IFX_PSB6970_VLAN_IdCreate(pEthSWDevHandle, &vlan_group);
     /* All ports are members on this VLAN group */
     for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
     {
         pm.nVId = 1;
         pm.nPortId = i;
         pm.bVLAN_TagEgress = IFX_FALSE;
         IFX_PSB6970_VLAN_PortMemberAdd(pEthSWDevHandle, &pm);
     }
    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_VLAN_PortMemberRead(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberRead_t *pPar)
{
	IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
	IFX_uint32_t	regValue;
    
	if (pPar->bInitial == IFX_TRUE) {
		pEthSWDevHandle->vlan_table_index = 0;
		// read VV bits
        IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, pEthSWDevHandle->vlan_table_index, &regValue);
        if (regValue == IFX_TRUE) {
        	IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VID, pEthSWDevHandle->vlan_table_index, &regValue);
        	pPar->nVId				= (regValue & 0xFFF);
        	IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, pEthSWDevHandle->vlan_table_index, &regValue);
			pPar->nPortId			= (regValue & 0x7F);
			IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, pEthSWDevHandle->vlan_table_index, &regValue);
			if ( regValue )
				pPar->nTagId 	= IFX_ENABLE;
			else
				pPar->nTagId 	= IFX_DISABLE;
        }
		pPar->bInitial = IFX_FALSE;
		pPar->bLast = IFX_FALSE;
	}

	if ( (pPar->bLast != IFX_TRUE)  ) {
		if (pEthSWDevHandle->vlan_table_index < IFX_PSB6970_VLAN_ENTRY_MAX) {
			pEthSWDevHandle->vlan_table_index++;
			IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VV, pEthSWDevHandle->vlan_table_index, &regValue);
			if (regValue == IFX_TRUE) {
				IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_VID, pEthSWDevHandle->vlan_table_index, &regValue);
				pPar->nVId				= (regValue & 0xFFF);
				IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_M, pEthSWDevHandle->vlan_table_index, &regValue);
				pPar->nPortId			= (regValue & 0x7F);
				IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, VLAN_FILTER_TM, pEthSWDevHandle->vlan_table_index, &regValue);
				if ( regValue )
					pPar->nTagId 	= IFX_ENABLE;
				else
					pPar->nTagId 	= IFX_DISABLE;
			}
		}
	} else {
		pPar->bLast = IFX_TRUE;
		pEthSWDevHandle->vlan_table_index = 0;
	} 
	return IFX_SUCCESS;
}


/**
   This is the switch core layer function to disable vlan aware.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_awareDisable(IFX_void_t *pDevCtx)
{
    //IFX_uint8_t i;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    IFX_PSB6970_VLAN_Table_SW_Init(pEthSWDevHandle);
    IFX_PSB6970_VLAN_Table_HW_Init(pEthSWDevHandle);


#if 0
     /* clear bVLAN_Aware variable */
     pEthSWDevHandle->bVLAN_Aware = IFX_FALSE;

     for (i=0; i<pEthSWDevHandle->nPortNumber; i++)
     {
         /* clear PxPBVM[03H]:TBVE[13] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_TBVE, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* clear PxPBVM[03H]:VC[11] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VC, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* set PxPBVM[03H]:VSD[10] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VSD, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* clear PxPBVM[03H]:AOVTP[9] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_AOVTP, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* clear PxPBVM[03H]:VMCE[8] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_VMCE, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
         /* set PxPBVM[03H]:BYPASS[7] bit */
         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_BYPASS, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;

         //if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVID, i, 1) != IFX_SUCCESS)
         //   return IFX_ERROR;

         if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_VLAN_PVTAGMP, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
     }
     IFX_PSB6970_VLAN_Table_Clear(pEthSWDevHandle);
#endif

    return IFX_SUCCESS;
}


/**
   This is the switch core layer function to set PHY PDN bit

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_PHY_PDN_Set(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    mdio_data.nData = SET_BITS(mdio_data.nData, 11, 11, 1);
    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to clear PHY PDN bit

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_PHY_PDN_Clear(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    mdio_data.nData = SET_BITS(mdio_data.nData, 11, 11, 0);
    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to disable port

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPortID  This parameter is port id.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_portDisable(IFX_void_t *pDevCtx, IFX_uint8_t nPortID)
{
    IFX_PSB6970_PHY_PDN_Set( pDevCtx, nPortID);

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to enable port

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPortID  This parameter is port id.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_portEnable(IFX_void_t *pDevCtx, IFX_uint8_t nPortID)
{

    IFX_PSB6970_PHY_PDN_Clear( pDevCtx, nPortID);

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to get medium detect status

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPortID  This parameter is port id.

   \return Return value as follows:
   - IFX_TRUE: if exists
   - IFX_FALSE: if non-exists
*/
IFX_boolean_t IFX_PSB6970_PHY_mediumDetectStatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPortID)
{
    IFX_ETHSW_MDIO_data_t mdio_data;
    IFX_uint32_t value;
    IFX_boolean_t bLinkStatus;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    mdio_data.nAddressDev = nPortID;
    mdio_data.nAddressReg = 0x16;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
    value = GET_BITS(mdio_data.nData,10,10);

    if (value)
        bLinkStatus = IFX_TRUE;
    else
        bLinkStatus = IFX_FALSE;

    return bLinkStatus;

}

/**
   This is the switch core layer function to get medium detect status

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPortID  This parameter is port id.

   \return Return value as follows:
   - IFX_TRUE: if exists
   - IFX_FALSE: if non-exists
*/
IFX_boolean_t IFX_PSB6970_PHY_linkStatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPortID)
{
    IFX_ETHSW_MDIO_data_t mdio_data;
    IFX_uint32_t value;
    IFX_boolean_t bLinkStatus;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    mdio_data.nAddressDev = nPortID;
    mdio_data.nAddressReg = 0x01;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
    value = GET_BITS(mdio_data.nData,2,2);

    if (value)
        bLinkStatus = IFX_TRUE;
    else
        bLinkStatus = IFX_FALSE;

    return bLinkStatus;

}

/**
   This is the switch core layer function to get port link status

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPortID  This parameter is port id.

   \return Return value as follows:
   - IFX_TRUE: if exists
   - IFX_FALSE: if non-exists
*/
IFX_boolean_t IFX_PSB6970_portLinkStatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPortID)
{

    IFX_uint32_t value;
    IFX_boolean_t bLinkStatus;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, PORT_PLS, nPortID, &value) != IFX_SUCCESS)
        return IFX_ERROR;

    if (value)
        bLinkStatus = IFX_TRUE;
    else
        bLinkStatus = IFX_FALSE;

    return bLinkStatus;

}

/**
   This is the switch core layer function to init vlan table structure

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_Table_SW_Init ( IFX_void_t *pDevCtx)
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i;

  for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i += 1 )
  {
      pEthSWDev->VLAN_Table[i].valid = 0;
      pEthSWDev->VLAN_Table[i].vid = 0;
      pEthSWDev->VLAN_Table[i].fid = 0;
      pEthSWDev->VLAN_Table[i].pm = 0;
      pEthSWDev->VLAN_Table[i].tm = 0;
  }
  pEthSWDev->bVLAN_Aware = IFX_FALSE;
  //IFX_PSB6970_VLAN_awareDisable(pEthSWDev);

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_SW_Init  ----- */

/**
   This is the switch core layer function to init vlan hardware bits

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_Table_HW_Init ( IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t i;

    for (i=0; i<pEthSWDev->nPortNumber; i++)
    {
        /* assign DFID bits */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_DFID, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        /* clear TBVE bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_TBVE, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* clear VC bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_VC, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* set VSD bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_VSD, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* clear AOVTP bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_AOVTP, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* clear VMCE bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_VMCE, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* set BYPASS bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_BYPASS, i, IFX_TRUE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* assign DVPM bits */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_DVPM, i, 0x7F) != IFX_SUCCESS)
            return IFX_ERROR;

        /* assign PP bits */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_PP, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        /* clear PPE bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_PPE, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* clear PVTAGMP bit */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_PVTAGMP, i, IFX_FALSE) != IFX_SUCCESS)
            return IFX_ERROR;
        /* assign PVID bits */
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, PORT_VLAN_PVID, i, 1) != IFX_SUCCESS)
            return IFX_ERROR;
    }

    // init vlan filter table
    for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i += 1 )
    {
        // write to VV bit
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VV, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        // write to VP bit
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VP, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        // write to VID bits
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VID, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        // write to FID bits
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VFID, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        // write to TM bits
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_TM, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
        // write to M bits
        if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_M, i, 0) != IFX_SUCCESS)
            return IFX_ERROR;
    }

    return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_HW_Init  ----- */

/**
   This is the switch core layer function to init vlan table structure

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_Table_Init ( IFX_void_t *pDevCtx)
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;

  IFX_PSB6970_VLAN_Table_SW_Init(pEthSWDev);
  IFX_PSB6970_VLAN_Table_HW_Init(pEthSWDev);

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Init  ----- */

/**
   This is the switch core layer function to init port config structure

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_PortConfig_Init ( IFX_void_t *pDevCtx)
{

  IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i;
  for ( i = 0; i < pEthSWDevHandle->nPortNumber; i++ )
  {
      pEthSWDevHandle->PortConfig[i].bTVM = IFX_FALSE;
      pEthSWDevHandle->PortConfig[i].eType = IFX_ETHSW_QOS_SCHEDULER_WFQ;
      pEthSWDevHandle->PortConfig[i].bPHYDown = IFX_FALSE;
      pEthSWDevHandle->PortConfig[i].eFlow = IFX_ETHSW_FLOW_AUTO;
  }

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_PortConfig_Init  ----- */

/**
   This is the switch core layer function to init MFC software config structure

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_MFC_SW_Init ( IFX_void_t *pDevCtx)
{

  IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i;
  for ( i = 0; i < IFX_PSB6970_MFC_RULES_MAX; i++ )
  {
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortSrc = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortDst = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortSrcRange = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nPortDstRange = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nProtocol = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.nEtherType = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterMatchField.eFieldSelection = IFX_PSB6970_MFC_ENTRY_NOEXIST;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterInfo.nTrafficClass = 0;
      pEthSWDevHandle->MFC_RulesEntrys[i].sFilterInfo.ePortForward = 0;
  }
  pEthSWDevHandle->RulesIndex = 0;

  for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
  {
      pEthSWDevHandle->MFC_etherTypeEntrys[i].bMF_ethertype = IFX_FALSE;
      pEthSWDevHandle->MFC_etherTypeEntrys[i].nVCET = 0;
  }
  for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
  {
      pEthSWDevHandle->MFC_protocolEntrys[i].bMF_protocol = IFX_FALSE;
      pEthSWDevHandle->MFC_protocolEntrys[i].nPFR = 0;
  }
  for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
  {
      pEthSWDevHandle->MFC_portEntrys[i].bMF_port = IFX_FALSE;
      pEthSWDevHandle->MFC_portEntrys[i].bPortSrc = IFX_FALSE;
      pEthSWDevHandle->MFC_portEntrys[i].nBasePt = 0;
      pEthSWDevHandle->MFC_portEntrys[i].nPRange = 0;
  }

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_MFC_SW_Init  ----- */

/**
   This is the switch core layer function to init MFC hardware config structure

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_MFC_HW_Init ( IFX_void_t *pDevCtx)
{

  IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i;

  for ( i = 0; i < IFX_PSB6970_MFC_ENTRY_MAX; i++ )
  {
      // clear register of ether type
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_VCET_ALL, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_ATF, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, TYPE_FILTER_QTF, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      // clear register of protocol type
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_PFR0, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_PFR1, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PROTOCOL_FILTER_APF, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      // clear register of port type
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_COMP, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_BASEPT, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_PRANGE, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_ATUF, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
      if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, PORT_FILTER_TUPF, i, 0)!= IFX_SUCCESS)
          return IFX_ERROR;
  }

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_MFC_HW_Init  ----- */

/**
   This is the switch core layer function to init MFC config structure

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_MFC_Init ( IFX_void_t *pDevCtx)
{

  IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

  IFX_PSB6970_MFC_SW_Init ( pEthSWDevHandle);
  IFX_PSB6970_MFC_HW_Init ( pEthSWDevHandle);

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_MFC_Init  ----- */

/**
   This is the switch core layer function to clear vlan table

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: if fail
*/
IFX_return_t IFX_PSB6970_VLAN_Table_Clear ( IFX_void_t *pDevCtx )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i;
  for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i += 1 )
  {
      pEthSWDev->VLAN_Table[i].valid = 0;
      pEthSWDev->VLAN_Table[i].vid = 0;
      pEthSWDev->VLAN_Table[i].fid = 0;
      pEthSWDev->VLAN_Table[i].pm = 0;
      pEthSWDev->VLAN_Table[i].tm = 0;
    // write to VV bit
    if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VV, i, 0) != IFX_SUCCESS) // xxx
        return IFX_ERROR;
    // write to VID bits
    if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VID, i, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to FID bits
    if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_VFID, i, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to M bits
    if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_M, i, 0) != IFX_SUCCESS)
        return IFX_ERROR;
    // write to TM bits
    if (IFX_PSB6970_RML_Write(pEthSWDev->pRML_Dev, VLAN_FILTER_TM, i, 0) != IFX_SUCCESS)
        return IFX_ERROR;
  }

  return IFX_SUCCESS;
}

/**
   This is the switch core layer function to check vlan id if exist

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_TRUE: if exists
   - IFX_FALSE: if non-exists
*/
IFX_boolean_t IFX_PSB6970_VLAN_Id_Exist ( IFX_void_t *pDevCtx, IFX_uint16_t vid )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i , found = IFX_FALSE;


  for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i += 1 )
  {
        if (vid == pEthSWDev->VLAN_Table[i].vid )
           found = IFX_TRUE;
  }

  return found;
}       /* -----  end of function IFX_PSB6970_VLAN_Id_Exist  ----- */

/**
   This is the internal test function to print vlan table

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_Table_Print ( IFX_void_t *pDevCtx )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i ;

  IFXOS_PRINT_INT_RAW(  "idx | valid |  vid  | fid |   pm |   tm\n");
  IFXOS_PRINT_INT_RAW(  "=======================================\n");
  for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i += 1 )
  {
        IFXOS_PRINT_INT_RAW(  "%3d | %3d   | %4d  |  %1d  | %4d | %4d\n",
                 i,
                 pEthSWDev->VLAN_Table[i].valid,
                 pEthSWDev->VLAN_Table[i].vid,
                 pEthSWDev->VLAN_Table[i].fid,
                 pEthSWDev->VLAN_Table[i].pm,
                 pEthSWDev->VLAN_Table[i].tm
              );
  }
  IFXOS_PRINT_INT_RAW(  "\n");

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Print  ----- */

/**
   This is the internal test function to print port configuration

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_PortConfig_Print ( IFX_void_t *pDevCtx )
{
    IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t i ;

    IFXOS_PRINT_INT_RAW(  " Port | bTVM\n");
    IFXOS_PRINT_INT_RAW(  "=============\n");
    for ( i=0; i<pEthSWDev->nPortNumber ; i++)
    {
        IFXOS_PRINT_INT_RAW(  "   %d  |   %c\n",
                    i,
                    (pEthSWDev->PortConfig[i].bTVM == IFX_TRUE)?'X':' '
                );

    }
    IFXOS_PRINT_INT_RAW(  "\n");

    return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_PortConfig_Print  ----- */

/**
   This is the switch core layer function to get vlan table index by using vid

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param vid This parameter is a vlan id.

   \return Return value as follows:
   - vlan table index: if avariable.
   - IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE: if unavariable.
*/
IFX_uint8_t IFX_PSB6970_VLAN_Table_Index_Find ( IFX_void_t *pDevCtx, IFX_uint16_t vid )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i, index = IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE;

  for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i += 1 )
  {
        if (vid == pEthSWDev->VLAN_Table[i].vid && pEthSWDev->VLAN_Table[i].valid == IFX_TRUE )
        {
           index = i;
           break;
        }
  }

  return index;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Index_Find  ----- */


/**
   This is the switch core layer function to check vlan table entry avariable.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - vlan table index: if avariable.
   - IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE: if unavariable.
*/
IFX_uint8_t IFX_PSB6970_VLAN_Table_Entry_Avariable ( IFX_void_t *pDevCtx  )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  IFX_uint8_t i, index = IFX_PSB6970_VLAN_TABLE_INDEX_UNAVARIABLE;

  for ( i = 0; i < IFX_PSB6970_VLAN_ENTRY_MAX; i++ )
  {
        if (pEthSWDev->VLAN_Table[i].valid == IFX_FALSE)
        {
           index = i;
           break;
        }
  }
  return index;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Entry_Avariable  ----- */

/**
   This is the switch core layer function to set vlan table entry.

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param table_index This parameter is a vlan table index.
   \param pTable_Entry This parameter is a pointer to IFX_PSB6970_VLAN_tableEntry_t structure.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_Table_Entry_Set ( IFX_void_t *pDevCtx, IFX_uint8_t table_index, IFX_PSB6970_VLAN_tableEntry_t *pTable_Entry )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;
  pEthSWDev->VLAN_Table[table_index].valid = pTable_Entry->valid;
  pEthSWDev->VLAN_Table[table_index].vid = pTable_Entry->vid;
  pEthSWDev->VLAN_Table[table_index].fid = pTable_Entry->fid;
  pEthSWDev->VLAN_Table[table_index].pm = pTable_Entry->pm;
  pEthSWDev->VLAN_Table[table_index].tm = pTable_Entry->tm;

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Entry_Set  ----- */

/**
   This is the switch core layer function to get vlan table entry.

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param table_index This parameter is a vlan table index.
   \param pTable_Entry This parameter is a pointer to IFX_PSB6970_VLAN_tableEntry_t structure.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_VLAN_Table_Entry_Get ( IFX_void_t *pDevCtx, IFX_uint8_t table_index, IFX_PSB6970_VLAN_tableEntry_t *pTable_Entry )
{
  IFX_PSB6970_switchDev_t *pEthSWDev = (IFX_PSB6970_switchDev_t*)pDevCtx;

  pTable_Entry->valid = pEthSWDev->VLAN_Table[table_index].valid;
  pTable_Entry->vid = pEthSWDev->VLAN_Table[table_index].vid;
  pTable_Entry->fid = pEthSWDev->VLAN_Table[table_index].fid;
  pTable_Entry->pm = pEthSWDev->VLAN_Table[table_index].pm;
  pTable_Entry->tm = pEthSWDev->VLAN_Table[table_index].tm;

  return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_VLAN_Table_Entry_Get  ----- */

/**
   This is the switch core layer function to get switch enable/disable status.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_boolean_t IFX_PSB6970_switchStatusGet(IFX_void_t *pDevCtx)
{
    IFX_uint32_t value;
    IFX_boolean_t bStatus=IFX_FALSE;
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    /* read bEnable from SGC2[E1].SE[15] bit  */
    if (IFX_PSB6970_RML_Read(pEthSWDevHandle->pRML_Dev, GLOBAL_SE, 0, &value) != IFX_SUCCESS)
        return IFX_ERROR;
    if ( value == IFX_TRUE)
             bStatus = IFX_TRUE;

    return bStatus;
}

/**
   This is the switch core layer function to enable switch device.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
#if 0
IFX_return_t IFX_PSB6970_switchDeviceEnable(IFX_void_t *pDevCtx)
{
   IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

   if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, GLOBAL_SE, 0, IFX_TRUE) != IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to disable switch device.

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_switchDeviceDisable(IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
        return IFX_ERROR;

    if (IFX_PSB6970_RML_Write(pEthSWDevHandle->pRML_Dev, GLOBAL_SE, 0, IFX_FALSE) != IFX_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}
#endif

/**
   This is the switch core layer function to all PHY power up

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_allPHY_powerup(IFX_void_t *pDevCtx)
{
   IFX_uint_t i;
   IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

   // enable all phy
   if (g_debug == 1)
        IFXOS_PRINT_INT_RAW(  "All PHY power up\n");

   for (i=0; i<=2; i++)
   {
        IFX_PSB6970_portEnable(pEthSWDevHandle, i);
   }

    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to all phy power down

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_allPHY_powerdown(IFX_void_t *pDevCtx)
{
    IFX_uint_t i;
   IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    // disable all phy
    if (g_debug == 1)
        IFXOS_PRINT_INT_RAW(  "All PHY power down\n");

    for (i=0; i<=2; i++)
    {
        IFX_PSB6970_portDisable(pEthSWDevHandle, i);
    }

    return IFX_SUCCESS;
}

/**
   This is the switch core layer init function.

   \param pInit This parameter is a pointer to the switch core context.

   \return Return value as follows:
   - pDev: if successful
*/
IFX_void_t *IFX_PSB6970_SwitchCoreInit(IFX_PSB6970_switchCoreInit_t *pInit )
{
    IFX_PSB6970_switchDev_t *pDev;

    pDev = (IFX_PSB6970_switchDev_t*) IFXOS_BlockAlloc (sizeof (IFX_PSB6970_switchDev_t));
	if (pDev == IFX_NULL) {
		 IFXOS_PRINT_INT_RAW(  "\n %s[%d]: (Memory allocation failed)!\n\n",__func__,__LINE__);
		 return (pDev);	
	}
    pDev->pRML_Dev = pInit->pDev;
    pDev->eDev = pInit->eDev;

    pCoreDev[pDev->eDev] = pDev;

    switch ( pDev->eDev ) {
        case IFX_PSB6970_DEV_AR9:
            pDev->nPortNumber = IFX_PSB6970_AR9_INTSW_PORT_NUMBER;
#ifdef IFX_ETHSW_API_COC
            pDev->pPMCtx = IFX_NULL;
#endif
            break;
        case IFX_PSB6970_DEV_TANTOS_3G:
        case IFX_PSB6970_DEV_TANTOS_0G:
            pDev->nPortNumber = IFX_PSB6970_TANTOSXG_PORT_NUMBER;
#ifdef IFX_ETHSW_API_COC
            pDev->pPMCtx = IFX_ETHSW_PM_powerManegementInit(pDev, IFX_ETHSW_PM_MODULENR_TANTOS3G); // for first external Tantos3G
#endif
            break;
        default:
            break;
    }               /* -----  end switch  ----- */

    IFX_PSB6970_BackupAllRegisters(pDev);
    IFX_PSB6970_VLAN_Table_Init(pDev);
    IFX_PSB6970_PortConfig_Init(pDev);
    IFX_PSB6970_MFC_Init(pDev);
    pDev->eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_DISABLED;

    return pDev;
}

/**
   This is the switch core layer cleanup function.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_SwitchCoreCleanUP(IFX_void_t )
{
    IFX_uint8_t i;
    IFX_PSB6970_switchDev_t *pDev;

    for (i=0; i<IFX_PSB6970_DEV_MAX; i++)
    {
        pDev = (IFX_PSB6970_switchDev_t*) pCoreDev[i];
        if (pDev != IFX_NULL)
        {
#ifdef IFX_ETHSW_API_COC
            if (pDev->pPMCtx != IFX_NULL)
            {
                IFX_ETHSW_PM_powerManegementCleanUp(pDev->pPMCtx);
            }
#endif
            IFXOS_BlockFree(pDev);
            pDev = IFX_NULL;
        }
    }
    return IFX_SUCCESS;
}

/**
   This is the switch core layer function to get PHY link status bit

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_uint8_t IFX_PSB6970_PHY_LINK_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t value;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 1;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    value = GET_BITS(mdio_data.nData, 2, 2);

    return value;
}

/**
   This is the switch core layer function to get GPHY speed status bit

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_uint8_t IFX_PSB6970_GPHY_SPEED_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
   IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
   IFX_ETHSW_MDIO_data_t mdio_data;
   IFX_uint32_t PhyID2, PhyID3;
   IFX_uint8_t value;

   /* Check GPHY ID 2*/
   mdio_data.nAddressDev = nPHYAD;
   mdio_data.nAddressReg = 0x2;
   IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
   PhyID2 = mdio_data.nData;

   /* Check GPHY ID 3*/
   mdio_data.nAddressDev = nPHYAD;
   mdio_data.nAddressReg = 0x3;
   IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
   PhyID3 = mdio_data.nData;

   if (( PhyID2 == 0x302) && ( PhyID3 == 0x60d1))
   {
      mdio_data.nAddressDev = nPHYAD;
      mdio_data.nAddressReg = 0x18;
      IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

      // 0:10M, 1:100M, 2:1000M
      value = GET_BITS(mdio_data.nData, 1, 0);
   }
   else
   {
      IFX_ETHSW_MDIO_data_t mdio_data;
      mdio_data.nAddressDev = nPHYAD;
      mdio_data.nAddressReg = 0x1C;
      IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

      // 0:10M, 1:100M, 2:1000M
      value = GET_BITS(mdio_data.nData, 4, 3);
   }

   return value;
}

/**
   This is the switch core layer function to get GPHY link status bit

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_uint8_t IFX_PSB6970_GPHY_LINK_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t value;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0x11;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    value = GET_BITS(mdio_data.nData, 12, 12);

    return value;
}

/**
   This is the switch core layer function to get GPHY duplex status bit

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_uint8_t IFX_PSB6970_GPHY_DUPLEX_STATUS_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
   IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
   IFX_ETHSW_MDIO_data_t mdio_data;
   IFX_uint32_t PhyID2, PhyID3;
   IFX_uint8_t value;

   /* Check GPHY ID 2*/
   mdio_data.nAddressDev = nPHYAD;
   mdio_data.nAddressReg = 0x2;
   IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
   PhyID2 = mdio_data.nData;

   /* Check GPHY ID 3*/
   mdio_data.nAddressDev = nPHYAD;
   mdio_data.nAddressReg = 0x3;
   IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
   PhyID3 = mdio_data.nData;

   if (( PhyID2 == 0x302) && ( PhyID3 == 0x60d1))
   {
      mdio_data.nAddressDev = nPHYAD;
      mdio_data.nAddressReg = 0x18;
      IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

      // 0: Half duplex, 1: Full duplex
      value = GET_BITS(mdio_data.nData, 3, 3);
   }
   else
   {
      mdio_data.nAddressDev = nPHYAD;
      mdio_data.nAddressReg = 0x1C;
      IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

      // 0: Half duplex, 1: Full duplex
      value = GET_BITS(mdio_data.nData, 5, 5);
   }

   return value;
}

/**
   This is the switch core layer function to  GPHY reset

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_PHY_Reset(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    mdio_data.nData = SET_BITS(mdio_data.nData, 15, 15, 1);
    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

/**
   This is the private function to calculate the TokenR and Token Timer.


   \param Rate given in MBit/s
   \param *TokenR contain TokenR value
   \param *TokenTimer contain Token Timer value

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t calcToken(IFX_uint32_t Rate, IFX_uint32_t *TokenR, IFX_uint32_t *TokenTimer)
{

   /*  Token timer set to 512 us */
   *TokenTimer = 0;

   *TokenR = Rate * 512/8;

   if (*TokenR >= (1 << 10)) {
      *TokenTimer = 1;
      /*  Token timer set to 128 us */
      *TokenR = Rate * 128/8;
      if (*TokenR >= (1 << 10)) {
         *TokenTimer = 2;
         /*  Token timer set to 32 us */
         *TokenR = Rate * 32/8;
         if (*TokenR >= (1 << 10)) {
            *TokenTimer = 3;
            /*  Token timer set to 8 us */
            *TokenR = Rate * 8/8;
            if (*TokenR >= (1 << 10)) {
               return IFX_ERROR;
            }
         }
      }
   }
   return IFX_SUCCESS;
}

/**
   This is the switch core layer function to get PHY ID

   \param pDevCtx  This parameter is a pointer to the switch device context.
   \param nPHYAD  This parameter is phy address.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_uint16_t IFX_PSB6970_PHYID_Get(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint8_t value;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 2;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    value = GET_BITS(mdio_data.nData, 0, 15);

    return value;
}

/**
   This is the switch core layer function to backup all value of registers

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_BackupAllRegisters ( IFX_void_t *pDevCtx)
{

    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint16_t i;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||// Tantos 3G
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
    {
        for (i=0; i<IFX_PSB6970_REGISTER_NUMBER_MAX; i++)
        {
            IFX_uint32_t value;
            IFX_PSB6970_RML_RegisterGet(pEthSWDevHandle->pRML_Dev, i, &value);
            pEthSWDevHandle->Registers[i] = value;
        }
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
    {
 //       for (i=0; i<0x1B4; i=i+4)
 		  for (i=0; i<(IFX_AR9_REGISTER_NUMBER_MAX/4); i++)
        {
            IFX_uint32_t value;
//            IFX_PSB6970_RML_RegisterGet(pEthSWDevHandle->pRML_Dev, i, &value);
			IFX_PSB6970_RML_RegisterGet(pEthSWDevHandle->pRML_Dev, (i*4), &value);
            pEthSWDevHandle->Registers[i] = value;
        }
    }

    return IFX_SUCCESS;

}       /* -----  end of function IFX_PSB6970_BackupAllRegisters  ----- */

/**
   This is the switch core layer function to restore all value of registers

   \param pDevCtx  This parameter is a pointer to the switch device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RestoreAllRegisters ( IFX_void_t *pDevCtx)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_uint16_t i;

    if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_3G ||// Tantos 3G
         pEthSWDevHandle->eDev == IFX_PSB6970_DEV_TANTOS_0G)  // Tantos 0G
    {
        for (i=0; i<IFX_PSB6970_REGISTER_NUMBER_MAX; i++)
        {
            IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, i, pEthSWDevHandle->Registers[i] );
        }
    }
    else if ( pEthSWDevHandle->eDev == IFX_PSB6970_DEV_AR9)   // AR9 internal switch
    {
//        for (i=0; i<0x1B4; i=i+4)
         for (i=0; i<(IFX_AR9_REGISTER_NUMBER_MAX/4); i++)
        {
//            IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, i, pEthSWDevHandle->Registers[i] );
			  IFX_PSB6970_RML_RegisterSet(pEthSWDevHandle->pRML_Dev, (i*4), pEthSWDevHandle->Registers[i] );
            
        }
    }
    return IFX_SUCCESS;
}       /* -----  end of function IFX_PSB6970_RestoreAllRegisters  ----- */

IFX_return_t IFX_PSB6970_PHY_forcedDuplex(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portDuplex_t eDuplex)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_MDIO_data_t mdio_data;

    // read Control Register
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
    // set duplex mode
    if ( eDuplex == IFX_ETHSW_DUPLEX_HALF )
        mdio_data.nData = SET_BITS(mdio_data.nData, 8, 8, 0);
    else
        mdio_data.nData = SET_BITS(mdio_data.nData, 8, 8, 1);
    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

// this function used for PHY and GPHY
IFX_return_t IFX_PSB6970_PHY_forcedSpeed(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portSpeed_t eSpeed)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_MDIO_data_t mdio_data;

    // read Control Register
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);
    // set Speed Selection LSB
    if ( eSpeed == IFX_ETHSW_PORT_SPEED_10 )
        mdio_data.nData = SET_BITS(mdio_data.nData, 13, 13, 0);
    else if ( eSpeed == IFX_ETHSW_PORT_SPEED_100 )
        mdio_data.nData = SET_BITS(mdio_data.nData, 13, 13, 1);
    else
        return IFX_ERROR;
    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PHY_AutoNegotiation(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_enDis_t eEnable )
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_MDIO_data_t mdio_data;

    // read Control Register
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    // set ANEN bit
    if (eEnable == IFX_ENABLE)
        mdio_data.nData = SET_BITS(mdio_data.nData, 12, 12, 1);
    else
        mdio_data.nData = SET_BITS(mdio_data.nData, 12, 12, 0);
    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_GPHY_forcedSpeed(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portSpeed_t eSpeed)
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;

    IFX_ETHSW_MDIO_data_t mdio_data;
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 0;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    switch (eSpeed)
    {
        case IFX_ETHSW_PORT_SPEED_10:
                mdio_data.nData = SET_BITS(mdio_data.nData, 6, 6, 0);
                mdio_data.nData = SET_BITS(mdio_data.nData, 13, 13, 0);
                break;
        case IFX_ETHSW_PORT_SPEED_100:
                mdio_data.nData = SET_BITS(mdio_data.nData, 6, 6, 0);
                mdio_data.nData = SET_BITS(mdio_data.nData, 13, 13, 1);
                break;
        case IFX_ETHSW_PORT_SPEED_1000:
                mdio_data.nData = SET_BITS(mdio_data.nData, 6, 6, 1);
                mdio_data.nData = SET_BITS(mdio_data.nData, 13, 13, 0);
                break;
        default:
                break;
    }

    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

IFX_return_t IFX_PSB6970_PHY_FlowControl(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAD, IFX_ETHSW_portFlow_t eFlow )
{
    IFX_PSB6970_switchDev_t *pEthSWDevHandle = (IFX_PSB6970_switchDev_t*)pDevCtx;
    IFX_ETHSW_MDIO_data_t mdio_data;

    // read ANAR Register
    mdio_data.nAddressDev = nPHYAD;
    mdio_data.nAddressReg = 4;
    IFX_PSB6970_MDIO_DataRead(pEthSWDevHandle, &mdio_data);

    // set ASM_DIR & PAUSE bit
    if (eFlow == IFX_ETHSW_FLOW_AUTO)
        mdio_data.nData = SET_BITS(mdio_data.nData, 11, 10, 1);
    //else if (eFlow == IFX_ETHSW_FLOW_RXTX)
    //    mdio_data.nData = SET_BITS(mdio_data.nData, 11, 10, 1);
    else if (eFlow == IFX_ETHSW_FLOW_OFF)
        mdio_data.nData = SET_BITS(mdio_data.nData, 11, 10, 0);
    else
        return IFX_ERROR;

    IFX_PSB6970_MDIO_DataWrite(pEthSWDevHandle, &mdio_data);

    return IFX_SUCCESS;
}

