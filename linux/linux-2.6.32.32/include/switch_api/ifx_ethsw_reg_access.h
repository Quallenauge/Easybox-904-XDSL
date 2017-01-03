/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_reg_access.h"
   \remarks
 *****************************************************************************/
#ifndef _IFX_ETHSW_REG_ACCESS_H
#define _IFX_ETHSW_REG_ACCESS_H

#include "ifx_ethsw_api.h"
#include "ifx_ethsw_vr9_reg_access.h"

#define VR9_BASE_ADDRESS                                 (KSEG1 | 0x1E108000)
#define VR9_REG32_ACCESS(addr)                            *((volatile IFX_int32_t *)(addr))

typedef struct
{
        IFX_return_t                (* register_write)(IFX_void_t *, IFX_int16_t, IFX_int16_t, IFX_int16_t, IFX_uint32_t);
        IFX_uint32_t                (* register_read)(IFX_void_t *, IFX_int16_t , IFX_int16_t , IFX_int16_t , IFX_uint32_t *);
        IFX_FLOW_devType_t					eDev;
        IFX_uint32_t                nBaseAddress;
} IFX_FLOW_RAL_Dev_t;

typedef struct
{
        IFX_FLOW_devType_t       eDev;
}IFX_FLOW_RAL_Init_t;

IFX_void_t *IFX_FLOW_RAL_init(IFX_FLOW_RAL_Init_t *pInit);


#endif    /* _IFX_ETHSW_REG_ACCESS.H */
