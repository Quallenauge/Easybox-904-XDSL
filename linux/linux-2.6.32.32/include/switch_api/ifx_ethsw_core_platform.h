/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_core_platform.h
   \remarks Platform dependancy Core Layer header file, for Infineon Ethernet
            switch drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_CORE_PLATFORM_H_
#define _IFX_ETHSW_CORE_PLATFORM_H_

#include "ifx_types.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#define SW_WRITE_REG32(data,addr)	IFX_REG_W32((data), (volatile unsigned  int *)addr)
#define SW_READ_REG32(addr)			IFX_REG_R32((volatile unsigned  int *)addr)

#define VRX_RCU_BASE_ADDR			(0xBF203000)
#define VRX_INT_BASE_ADDR			(0xBF107000)
#define GSWIP_BASE_ADDR				(0xBE108000)
#define GSWIP_TOP_BASE_ADDR			(GSWIP_BASE_ADDR + (0x0C40 * 4))
#define GSWIP_REG_ACCESS(addr)		*((volatile IFX_int32_t *)(addr))

//#define VR9_CHIP_ID_MAGIC_NUMBER		(0x101C0083)
/* GPHY0 Firmware Base Address Register			 */
#define GFS_ADD0					(VRX_RCU_BASE_ADDR + 0x20)
/* GPHY1 Firmware Base Address Register			 */
#define GFS_ADD1					(VRX_RCU_BASE_ADDR + 0x68)
/* Chip Identification Register  */
#define MPS_CHIPID					(VRX_INT_BASE_ADDR + 0x344)
/* GPHY01 MDIO Address Register  */
#define GFMDIO_ADD					(VRX_RCU_BASE_ADDR + 0x44)
/* PHY Address Register PORT 5  */
#define PHY_ADDR_5					(GSWIP_BASE_ADDR + (0xC50 * 4))
/* PHY Address Register PORT 4  */
#define PHY_ADDR_4					(GSWIP_BASE_ADDR + (0xC51 * 4))
/* PHY Address Register PORT 3  */
#define PHY_ADDR_3					(GSWIP_BASE_ADDR + (0xC52 * 4))
/* PHY Address Register PORT 2  */
#define PHY_ADDR_2					(GSWIP_BASE_ADDR + (0xC53 * 4))
/* PHY Address Register PORT 1  */
#define PHY_ADDR_1					(GSWIP_BASE_ADDR + (0xC54 * 4))
/* PHY Address Register PORT 0  */
#define PHY_ADDR_0					(GSWIP_BASE_ADDR + (0xC55 * 4))
/* Transmit and Receive Buffer Control Register */
#define MAC_PDI_CTRL_6				(GSWIP_BASE_ADDR + (0x951 * 4))
/* Transmit and Receive Buffer Control Register */
#define PMAC_RX_IPG					(GSWIP_BASE_ADDR + (0xCCB * 4))
/** MDC Clock Configuration Register 0 */
#define MDC_CFG_0_REG				(GSWIP_TOP_BASE_ADDR + (0x0B * 4))
#define MDC_CFG_0_PEN_SET(port)		(0x1 << port )
#define MDC_CFG_0_PEN_GET(port, reg_data)	((reg_data >> port ) & 0x1 )
/** MDC Clock Configuration Register 1 */
#define MDC_CFG_1_REG  
/** MDIO Control Register */
#define MDIO_CTRL_REG				(GSWIP_TOP_BASE_ADDR + (8 * 4))
/** MDIO Busy*/
#define MDIO_CTRL_MBUSY				0x1000
#define MDIO_CTRL_OP_MASK			0x0C00
#define MDIO_CTRL_OP_WR				0x0400
#define MDIO_CTRL_OP_RD				0x0800
#define MDIO_CTRL_PHYAD_SET(arg)	((arg & 0x1F) << 5)
#define MDIO_CTRL_PHYAD_GET(arg)	( (arg >> 5 ) & 0x1F)
#define MDIO_CTRL_REGAD(arg)		( arg & 0x1F)
/** MDIO Read Data Register */
#define MDIO_READ_REG				(GSWIP_TOP_BASE_ADDR + (9 * 4))
#define MDIO_READ_RDATA(arg)		(arg & 0xFFFF)
/** MDIO Write Data Register */
#define MDIO_WRITE_REG				(GSWIP_TOP_BASE_ADDR + (0x0A * 4))
#define MDIO_READ_WDATA(arg)		(arg & 0xFFFF)

/**********************/
/* Function Prototype */
/**********************/
IFX_int32_t platform_device_reset_trigger(IFX_void_t);
IFX_int32_t platform_device_reset_release(IFX_void_t);
IFX_int32_t platform_device_init(IFX_void_t *pDevCtx);

#endif    /* _IFX_ETHSW_CORE_PLATFORM_H_ */
