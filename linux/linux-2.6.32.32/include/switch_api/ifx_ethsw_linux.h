/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_linux.h
   \remarks Generic switch API header file, for Infineon Ethernet switch
            drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_LINUX_H_
#define _IFX_ETHSW_LINUX_H_

#include "ifx_ethsw_api.h"

typedef struct
{
   IFX_ETHSW_lowLevelFkts_t *pLlTable;
   void                     *pLlHandle;
   char                     paramBuffer[PARAM_BUFFER_SIZE];
}IFX_ETHSW_ioctlHandle_t;

typedef struct
{
   IFX_boolean_t                           bInternalSwitch;
   /** Number of similar Low Level External Switch Devices */
   IFX_uint8_t                             nExternalSwitchNum;
   IFX_ETHSW_ioctlHandle_t                 *pIoctlHandle;
   /** Array of pEthSWDev pointers associated with this driver context */
   IFX_void_t                              *pEthSWDev[IFX_ETHSW_DEV_MAX];
} IFX_ETHSW_IOCTL_WRAPPER_CTX_t;

typedef struct 
{
   IFX_ETHSW_lowLevelFkts_t 			*pLlTable;
}IFX_ETHSW_IOCTL_wrapperInit_t;

typedef struct {
    unsigned char minor_number;
} IFX_ETHSW_devoneData_t;

IFX_return_t IFX_ETHSW_Drv_Register(IFX_uint32_t Major);
IFX_return_t IFX_ETHSW_Drv_UnRegister (IFX_uint32_t Major);
IFX_void_t *IFX_ETHSW_IOCTL_WrapperInit(IFX_ETHSW_IOCTL_wrapperInit_t *pInit);
IFX_return_t IFX_ETHSW_IOCTL_WrapperDeviceAdd(IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlDev, IFX_void_t *pCoreDev, IFX_uint8_t nMinorNum);
IFX_return_t IFX_ETHSW_IOCTL_WrapperCleanUp(IFX_void_t);

#endif    /* _IFX_ETHSW_LINUX_H_ */
