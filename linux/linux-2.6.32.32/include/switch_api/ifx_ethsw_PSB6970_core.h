/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_ral.h
   \remarks PSB6970 Core Layer header file, for Infineon Ethernet switch
            drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_PSB6970_CORE_H_
#define _IFX_ETHSW_PSB6970_CORE_H_

#include "ifx_ethsw_api.h"
#define IFX_PSB6970_MFC_ENTRY_MAX 8
#define IFX_PSB6970_MFC_RULES_MAX 24
#define IFX_PSB6970_REGISTER_NUMBER_MAX 0x122
#define IFX_PSB6970_VLAN_ENTRY_MAX 16
#define IFX_PSB6970_PORT_NUMBER_MAX 7
#define IFX_PSB6970_AR9_INTSW_PORT_NUMBER 3
#define IFX_PSB6970_TANTOSXG_PORT_NUMBER 7
#define IFX_AR9_REGISTER_NUMBER_MAX 0x1B4
/** Description */
typedef struct 
{
    /** Transparent Mode */
    IFX_boolean_t                   bTVM;
    IFX_ETHSW_QoS_Scheduler_t       eType;
    IFX_boolean_t                   bPHYDown;
    IFX_boolean_t                   eFlow;
}IFX_PSB6970_portConfig_t;

/** Description */
typedef struct 
{
  IFX_boolean_t                     valid;
   /* VLAN ID */
  IFX_uint16_t                      vid;
   /* Forward ID */
  IFX_uint32_t                      fid;
   /* Port Member */
  IFX_uint16_t                      pm;
   /* Tag Member */
  IFX_uint16_t                      tm;
}IFX_PSB6970_VLAN_tableEntry_t;

/** Description */
typedef struct 
{
   IFX_boolean_t  bMF_ethertype;
   IFX_uint16_t   nVCET;
}IFX_PSB6970_MFC_etherTypeEntry_t;
typedef struct 
{
   IFX_boolean_t  bMF_protocol;
   IFX_uint8_t    nPFR;
}IFX_PSB6970_MFC_protocolEntry_t;
typedef struct 
{
   IFX_boolean_t  bMF_port;
   IFX_boolean_t  bPortSrc;
   IFX_uint16_t   nBasePt;
   IFX_uint8_t    nPRange;
}IFX_PSB6970_MFC_portEntry_t;

typedef struct {
   IFX_PSB6970_devType_t                eDev;
   IFX_PSB6970_portConfig_t             PortConfig[IFX_PSB6970_PORT_NUMBER_MAX];
   IFX_boolean_t                        bVLAN_Aware;   
   IFX_PSB6970_VLAN_tableEntry_t        VLAN_Table[IFX_PSB6970_VLAN_ENTRY_MAX];
   IFX_void_t                           *pRML_Dev;
#ifdef IFX_ETHSW_API_COC
   IFX_void_t                           *pPMCtx;
#endif
   IFX_uint8_t                          nPortNumber;
   IFX_PSB6970_MFC_etherTypeEntry_t     MFC_etherTypeEntrys[IFX_PSB6970_MFC_ENTRY_MAX];
   IFX_PSB6970_MFC_protocolEntry_t      MFC_protocolEntrys[IFX_PSB6970_MFC_ENTRY_MAX];
   IFX_PSB6970_MFC_portEntry_t          MFC_portEntrys[IFX_PSB6970_MFC_ENTRY_MAX];
   IFX_PSB6970_QoS_MfcCfg_t             MFC_RulesEntrys[IFX_PSB6970_MFC_RULES_MAX];
   IFX_uint8_t                          RulesIndex;
   IFX_uint32_t                         Registers[IFX_PSB6970_REGISTER_NUMBER_MAX];
   IFX_ETHSW_multicastSnoopMode_t       eIGMP_Mode;
   IFX_uint16_t							vlan_table_index;
} IFX_PSB6970_switchDev_t;

typedef struct 
{
   IFX_void_t                           *pDev;
   IFX_PSB6970_devType_t                eDev;
}IFX_PSB6970_switchCoreInit_t;

typedef struct 
{
  IFX_uint8_t                           nTable_Index;
  IFX_uint8_t                           nOP; // operation (1:add, 2:delete)
  IFX_uint16_t                          nVId;
  IFX_uint32_t                          nFId;
}IFX_PSB6970_VLAN_Filter_registerOperation_t;

typedef enum 
{
   /** 1522 bytes */
   IFX_ETHSW_MAXPKTLEN_1522_BYTES                     = 0,
   /** 1518 bytes */
   IFX_ETHSW_MAXPKTLEN_1518_BYTES                     = 1,
   /** 1536 bytes */
   IFX_ETHSW_MAXPKTLEN_1536_BYTES                     = 2,
   /** Reserved */
   IFX_ETHSW_MAXPKTLEN_RESERVED                       = 3

}IFX_ETHSW_maxPacketLength_t; // xxx
/** Command for access counter */

typedef enum
{
    IFX_PSB6970_CAC_INDIRECT_READ                       = 0,
    IFX_PSB6970_CAC_GET_PORT_COUNTER                    = 1,
    IFX_PSB6970_CAC_RESET_PORT_COUNTER                  = 2,
    IFX_PSB6970_CAC_RESET_ALL_COUNTER                   = 3
}IFX_PSB6970_CAC_command_t;

/** Description */
typedef enum
{
    /** Create a new address */
    IFX_PSB6970_MACTABLE_ENTRY_CREATE                       = 0x07,
    IFX_PSB6970_MACTABLE_ENTRY_OVERWRITE                    = 0x0F,
    IFX_PSB6970_MACTABLE_ENTRY_ERASE                        = 0x1F,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_EMPTY_ADDR            = 0x20,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_PORT               = 0x29,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_FID                = 0x2A,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC                = 0x2C,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_AND_FID        = 0x2E,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_AND_PORT       = 0x2D,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_FID_AND_PORT       = 0x2B,
    IFX_PSB6970_MACTABLE_ENTRY_SEARCH_BY_MAC_FID_AND_PORT   = 0x2F,
    IFX_PSB6970_MACTABLE_INIT_TO_LOCATION                   = 0x34,
    IFX_PSB6970_MACTABLE_INIT_TO_FIRST                      = 0x30
}IFX_PSB6970_MAC_tableCommand_t;

/** Description */
typedef enum
{
    /**  */
    IFX_PSB6970_MAC_COMMAND_OK                          = 0,
    IFX_PSB6970_MAC_COMMAND_ALL_ENTRY_USED              = 1,
    IFX_PSB6970_MAC_COMMAND_ENTRY_NOT_FOUND             = 2,
    IFX_PSB6970_MAC_COMMAND_TEMP_STATE                  = 3,
    IFX_PSB6970_MAC_COMMAND_ERROR                       = 5
}IFX_PSB6970_MAC_commandResult_t;

/**********************/
/* Function Prototype */
/**********************/
IFX_return_t IFX_PSB6970_VLAN_awareEnable(IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_VLAN_awareDisable(IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_PHY_PDN_Set(IFX_void_t *pDevCtx, IFX_uint8_t PHYAD);
IFX_return_t IFX_PSB6970_PHY_PDN_Clear(IFX_void_t *pDevCtx, IFX_uint8_t PHYAD);
IFX_return_t IFX_PSB6970_portDisable(IFX_void_t *pDevCtx, IFX_uint8_t nPortID);
IFX_return_t IFX_PSB6970_portEnable(IFX_void_t *pDevCtx, IFX_uint8_t nPortID);
IFX_boolean_t IFX_PSB6970_PHY_linkStatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPortID);
IFX_boolean_t IFX_PSB6970_PHY_mediumDetectStatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPortID);
IFX_boolean_t IFX_PSB6970_portLinkStatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPortID);
IFX_return_t IFX_PSB6970_VLAN_Table_Init ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_PortConfig_Init ( IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_VLAN_Table_Clear ( IFX_void_t *pDevCtx );
IFX_boolean_t IFX_PSB6970_VLAN_Id_Exist ( IFX_void_t *pDevCtx, IFX_uint16_t vid );
IFX_return_t IFX_PSB6970_VLAN_Table_Print ( IFX_void_t *pDevCtx );
IFX_return_t IFX_PSB6970_PortConfig_Print ( IFX_void_t *pDevCtx );
IFX_uint8_t IFX_PSB6970_VLAN_Table_Index_Find ( IFX_void_t *pDevCtx, IFX_uint16_t vid );
IFX_uint8_t IFX_PSB6970_VLAN_Table_Entry_Avariable ( IFX_void_t *pDevCtx  );
IFX_return_t IFX_PSB6970_VLAN_Table_Entry_Set ( IFX_void_t *pDevCtx, IFX_uint8_t table_index, IFX_PSB6970_VLAN_tableEntry_t *pTable_Entry );
IFX_return_t IFX_PSB6970_VLAN_Table_Entry_Get ( IFX_void_t *pDevCtx, IFX_uint8_t table_index, IFX_PSB6970_VLAN_tableEntry_t *pTable_Entry );
IFX_boolean_t IFX_PSB6970_switchStatusGet(IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_switchDeviceEnable(IFX_void_t *pDevCtx);
IFX_return_t IFX_PSB6970_switchDeviceDisable(IFX_void_t *pDevCtx);
IFX_void_t *IFX_PSB6970_SwitchCoreInit(IFX_PSB6970_switchCoreInit_t *pInit );
IFX_return_t IFX_PSB6970_SwitchCoreCleanUP(IFX_void_t );
#endif    /* _IFX_ETHSW_PSB6970_CORE_H_ */
