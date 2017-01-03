/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_ll.h"
   \remarks 
 *****************************************************************************/
 
#ifndef _IFX_ETHSW_LL_ACCESS_H_
#define _IFX_ETHSW_LL_ACCESS_H_

#include "ifx_ethsw.h"
#include "regmapper.h"

#define INTERNAL_ACCESS		0
#define EXTERNAL_ACCESS		1

#define AMAZON_S_SW						(KSEG1 | 0x1E108000)
#define BASE_ADDRESS                                            (KSEG1 | 0x1E108000)
#define REG32_ACCESS(addr)					*((volatile IFX_int32_t *)(addr))
#define AMAZON_S_SW_REG(off)				((volatile IFX_int32_t*)(AMAZON_S_SW + (off)))


#define IFX_DEV_TANTOS_3G_BASEADDR		AMAZON_S_SW_REG(0x0cc)
#define IFX_DEV_AR9_BASEADDR				AMAZON_S_SW_REG(0x000)


typedef enum
{
	IFX_RML_DEV_TANTOS_3G,
	IFX_RML_DEV_AR9
} IFX_RML_Dev_t;

typedef struct
{
	/** ?? */
	IFX_return_t					(* register_write)(IFX_void_t *, IFX_int32_t, IFX_uint32_t );
	/** ?? */	
	IFX_int32_t					(* register_read)(IFX_ETHSW_regMapper_t *, IFX_int32_t, IFX_uint32_t *);
	/** ?? */	
	IFX_void_t 					* pRegAccessHandle;
	/** ?? */	
	IFX_uint32_t 					nBaseAddress;
	/** ?? */	
	IFX_RML_Dev_t 				eDev;
   	/** Low Level Ethernet Switch driverName */
   	IFX_char_t					*drvName;	
} IFX_ETHSW_RML_Init_t;


typedef struct
{
	/** ?? */
	IFX_return_t					(* register_write)(IFX_void_t *, IFX_int32_t, IFX_uint32_t );
	/** ?? */	
	IFX_int32_t					(* register_read)(IFX_ETHSW_regMapper_t *, IFX_int32_t, IFX_uint32_t *);
	/** ?? */	
	IFX_void_t 					*pRegAccessHandle;
	/** ?? */	
	IFX_uint32_t 					nBaseAddress;
	/** ?? */	
	IFX_RML_Dev_t 				eDev;
	/** ?? */	
	IFX_ETHSW_regMapper_t		*tableHandle;
} RML_Dev_t;


/* AMAZON_S GPORT SWITCH Register */

#define AMAZON_S_BASEADDR                             (KSEG1 | 0x1E108000)
#define AMAZON_S_DF_PORTMAP				   (AMAZON_S_BASEADDR + 0x02C)


IFX_return_t IFX_ethsw_RML_Read(RML_Dev_t* pCxtHandle, 
									IFX_ETHSW_regMapperSelector_t commonbit, 
									IFX_uint32_t portIdx, 
									IFX_uint32_t *value);
IFX_return_t IFX_ethsw_RML_Write(RML_Dev_t *pCxtHandle, 
							IFX_ETHSW_regMapperSelector_t commonbit,
							IFX_uint32_t portIdx,
							IFX_uint32_t value);

IFX_return_t IFX_ethsw_RML_Tantos3G_Read(RML_Dev_t* pCxtHandle, 
									IFX_ETHSW_regMapperSelector_t commonbit, 
									IFX_uint32_t portIdx, 
									IFX_uint32_t *value);

IFX_return_t IFX_ethsw_RML_Tantos3G_Write(RML_Dev_t *pCxtHandle, 
							IFX_ETHSW_regMapperSelector_t commonbit,
							IFX_uint32_t portIdx,
							IFX_uint32_t value);

IFX_return_t IFX_Register_RML_init(IFX_ETHSW_RML_Init_t *pInit);

#endif

