/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_rml.h
   \remarks Register Mapping Layer header file, for Infineon Ethernet switch
            drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_RML_H_
#define _IFX_ETHSW_RML_H_

#include "ifx_ethsw_api.h"

typedef struct
{
	IFX_return_t                (* register_read)(IFX_void_t *, IFX_int32_t, IFX_uint32_t *);
	IFX_return_t                (* register_write)(IFX_void_t *, IFX_int32_t, IFX_uint32_t);
	IFX_void_t                  *pRegAccessHandle;
	IFX_uint32_t                nBaseAddress;
	IFX_ETHSW_regMapper_t		*tableHandle;
} IFX_PSB6970_RML_Dev_t;

typedef struct
{
	IFX_return_t                		(* register_read)(IFX_void_t *, IFX_int32_t, IFX_uint32_t *);
	IFX_return_t                		(* register_write)(IFX_void_t *, IFX_int32_t, IFX_uint32_t);
	IFX_void_t                  		*pRegAccessHandle;
	IFX_uint32_t                		nBaseAddress;
	IFX_PSB6970_devType_t       			eDev;
}IFX_PSB6970_RML_Init_t;


IFX_return_t IFX_PSB6970_RML_RegisterGet(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value);
IFX_return_t IFX_PSB6970_RML_RegisterSet(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value);
IFX_return_t IFX_PSB6970_RML_Read(IFX_void_t * pDevCtx, 
									IFX_ETHSW_regMapperSelector_t commonbit, 
									IFX_uint32_t portIdx, 
									IFX_uint32_t *pvalue);
IFX_return_t IFX_PSB6970_RML_Write(IFX_void_t *pDevCtx, 
							IFX_ETHSW_regMapperSelector_t commonbit,
							IFX_uint32_t portIdx,
							IFX_uint32_t value);
IFX_void_t *IFX_PSB6970_RML_init(IFX_PSB6970_RML_Init_t *pInit);
#endif    /* _IFX_ETHSW_RML_H_ */
