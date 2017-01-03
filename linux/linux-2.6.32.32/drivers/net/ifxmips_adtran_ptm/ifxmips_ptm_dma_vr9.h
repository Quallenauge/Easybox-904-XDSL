/******************************************************************************
**
** FILE NAME    : ifxmips_ptm_common.h
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : PTM driver header file (common definitions)
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 17 JUN 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFXMIPS_PTM_DMA_VR9_H
#define IFXMIPS_PTM_DMA_VR9_H



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  DMA Register
 */
extern unsigned int ifx_vr9_dma_base_addr;
#define IFX_DMA                                 KSEG1ADDR(ifx_vr9_dma_base_addr)
#define IFX_DMA_ORG                             0x1E104100

#define IFX_DMA_BASE                            IFX_DMA
#define IFX_DMA_CLC                             (volatile u32*)(IFX_DMA_BASE + 0x00)
#define IFX_DMA_ID                              (volatile u32*)(IFX_DMA_BASE + 0x08)
#define IFX_DMA_CTRL                            (volatile u32*)(IFX_DMA_BASE + 0x10)
#define IFX_DMA_CPOLL                           (volatile u32*)(IFX_DMA_BASE + 0x14)

#define IFX_DMA_CS(i)                           (volatile u32*)(IFX_DMA_BASE + 0x18 + 0x38 * (i))
#define IFX_DMA_CCTRL(i)                        (volatile u32*)(IFX_DMA_BASE + 0x1C + 0x38 * (i))
#define IFX_DMA_CDBA(i)                         (volatile u32*)(IFX_DMA_BASE + 0x20 + 0x38 * (i))
#define IFX_DMA_CDLEN(i)                        (volatile u32*)(IFX_DMA_BASE + 0x24 + 0x38 * (i))
#define IFX_DMA_CIS(i)                          (volatile u32*)(IFX_DMA_BASE + 0x28 + 0x38 * (i))
#define IFX_DMA_CIE(i)                          (volatile u32*)(IFX_DMA_BASE + 0x2C + 0x38 * (i))

#define IFX_DMA_CGBL                            (volatile u32*)(IFX_DMA_BASE + 0x30)

#define IFX_DMA_PS(i)                           (volatile u32*)(IFX_DMA_BASE + 0x40 + 0x30 * (i))
#define IFX_DMA_PCTRL(i)                        (volatile u32*)(IFX_DMA_BASE + 0x44 + 0x30 * (i))

#define IFX_DMA_IRNEN                           (volatile u32*)(IFX_DMA_BASE + 0xf4)
#define IFX_DMA_IRNCR                           (volatile u32*)(IFX_DMA_BASE + 0xf8)
#define IFX_DMA_IRNICR                          (volatile u32*)(IFX_DMA_BASE + 0xfc)



#endif  //  IFXMIPS_PTM_DMA_VR9_H
