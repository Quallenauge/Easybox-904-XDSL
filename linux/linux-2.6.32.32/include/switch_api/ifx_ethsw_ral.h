/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_ral.h
   \remarks Register Access Layer header file, for Infineon Ethernet switch
            drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_RAL_H_
#define _IFX_ETHSW_RAL_H_

#include "ifx_ethsw_api.h"

#define REG32_ACCESS(addr)					*((volatile IFX_int32_t *)(addr))

#define IFX_PSB6970_AR9_BASEADDR	        (KSEG1 | 0x1E108000)
#define IFX_PSB6970_TANTOS_3G_BASEADDR	    (KSEG1 | 0x1E1080CC)
#define IFX_DANUBE_PPE_BASEADDR	            (KSEG1 | 0x1E191804)
#define IFX_AMAZON_SE_PPE_BASEADDR	        (KSEG1 | 0x1E191804)

#define IFX_PSB6970_MDIO_OP_SHIFT        10
#define IFX_PSB6970_MDIO_MBUSY_SHIFT     15
#define IFX_PSB6970_MDIO_WD_SHIFT        16
#define IFX_PSB6970_MDIO_READ_OPERATION  0x02
#define IFX_PSB6970_MDIO_WRITE_OPERATION 0x01

#define IFX_DANUBE_MDIO_RA_SHIFT            31
#define IFX_DANUBE_MDIO_RW_SHIFT            30
#define IFX_DANUBE_MDIO_REGADDR_SHIFT       16
#define IFX_AMAZON_SE_MDIO_RA_SHIFT         31
#define IFX_AMAZON_SE_MDIO_RW_SHIFT         30
#define IFX_AMAZON_SE_MDIO_REGADDR_SHIFT    16
typedef struct
{
	IFX_return_t                (* register_write)(IFX_void_t *, IFX_int32_t, IFX_uint32_t);
	IFX_return_t                (* register_read)(IFX_void_t *, IFX_int32_t, IFX_uint32_t *);
	IFX_PSB6970_devType_t       eDev;
	IFX_uint32_t                nBaseAddress;
} IFX_PSB6970_RAL_Dev_t;

typedef struct
{
	IFX_PSB6970_devType_t       eDev;
}IFX_PSB6970_RAL_Init_t;

IFX_return_t IFX_PSB6970_RAL_Register_DirectRead(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value);
IFX_return_t IFX_PSB6970_RAL_Register_DirectWrite(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value);
IFX_return_t IFX_PSB6970_RAL_Register_MDIORead(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value);
IFX_return_t IFX_PSB6970_RAL_Register_MDIOWrite(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value);
IFX_void_t *IFX_PSB6970_RAL_init(IFX_PSB6970_RAL_Init_t *pInit);
#endif    /* _IFX_ETHSW_RAL_H_ */
