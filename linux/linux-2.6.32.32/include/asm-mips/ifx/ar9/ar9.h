/******************************************************************************
**
** FILE NAME    : ar9.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : header file for AR9
** COPYRIGHT    :       Copyright (c) 2009
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
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



#ifndef AR9_H
#define AR9_H
#include <asm/bootinfo.h>
/******************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
******************************************************************************/
#define MACH_GROUP_IFX MACH_GROUP_AR9
#define MACH_TYPE_IFX  MACH_AR9


/***********************************************************************/
/*  Module      :  WDT register address and bits                       */
/***********************************************************************/

#define IFX_WDT                                 (KSEG1 | 0x1F880000)

/***Watchdog Timer Control Register ***/
#define IFX_WDT_CR                              ((volatile u32*)(IFX_WDT + 0x03F0))
#define IFX_WDT_CR_GEN                          (1 << 31)
#define IFX_WDT_CR_DSEN                         (1 << 30)
#define IFX_WDT_CR_LPEN                         (1 << 29)
#define IFX_WDT_CR_PWL_GET(value)               (((value) >> 26) & ((1 << 2) - 1))
#define IFX_WDT_CR_PWL_SET(value)               (((( 1 << 2) - 1) & (value)) << 26)
#define IFX_WDT_CR_CLKDIV_GET(value)            (((value) >> 24) & ((1 << 2) - 1))
#define IFX_WDT_CR_CLKDIV_SET(value)            (((( 1 << 2) - 1) & (value)) << 24)
#define IFX_WDT_CR_PW_GET(value)                (((value) >> 16) & ((1 << 8) - 1))
#define IFX_WDT_CR_PW_SET(value)                (((( 1 << 8) - 1) & (value)) << 16)
#define IFX_WDT_CR_RELOAD_GET(value)            (((value) >> 0) & ((1 << 16) - 1))
#define IFX_WDT_CR_RELOAD_SET(value)            (((( 1 << 16) - 1) & (value)) << 0)

/***Watchdog Timer Status Register***/
#define IFX_WDT_SR                              ((volatile u32*)(IFX_WDT + 0x03F8))
#define IFX_WDT_SR_EN                           (1 << 31)
#define IFX_WDT_SR_AE                           (1 << 30)
#define IFX_WDT_SR_PRW                          (1 << 29)
#define IFX_WDT_SR_EXP                          (1 << 28)
#define IFX_WDT_SR_PWD                          (1 << 27)
#define IFX_WDT_SR_DS                           (1 << 26)
#define IFX_WDT_SR_VALUE_GET(value)             (((value) >> 0) & ((1 << 16) - 1))
#define IFX_WDT_SR_VALUE_SET(value)             (((( 1 << 16) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/

#define IFX_RCU                                 (KSEG1 | 0x1F203000)

#define IFX_RCU_RST_REQ                         ((volatile u32*)(IFX_RCU + 0x0010))
#define IFX_RCU_RST_STAT                        ((volatile u32*)(IFX_RCU + 0x0014))
#define IFX_USB_CFG                             ((volatile u32*)(IFX_RCU + 0x0018))
#define IFX_RCU_PPE_CONF                        ((volatile u32*)(IFX_RCU + 0x002C))

/***Reset Request Register***/
#define IFX_RCU_RST_REQ_SRST                    (1 << 30)
#define IFX_RCU_RST_REQ_ARC_JTAG                (1 << 20)
#define IFX_RCU_RST_REQ_AFE                     (1 << 11)
#define IFX_RCU_RST_REQ_PPE                     (1 << 8)
#define IFX_RCU_RST_REQ_DFE                     (1 << 7)

/* CPU0, CPU1, CPUSUB, HRST, WDT0, WDT1, DMA, ETHPHY1, ETHPHY0 */
#define IFX_RCU_RST_REQ_ALL                     IFX_RCU_RST_REQ_SRST



/***********************************************************************/
/*  Module      :  BCU  register address and bits                      */
/***********************************************************************/

#define IFX_BCU_BASE_ADDR                       (KSEG1 | 0x1E100000)
#define IFX_SLAVE_BCU_BASE_ADDR                 (KSEG1 | 0x1C200400)

/***BCU Control Register (0010H)***/
#define IFX_BCU_CON                             ((volatile u32*)(0x0010 + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_CON                       ((volatile u32*)(0x0010 + IFX_SLAVE_BCU_BASE_ADDR))
#define IFX_BCU_STARVATION_MASK                 (0xFF << 24)
#define IFX_BCU_STARVATION_SHIFT                24
#define IFX_BCU_TOUT_MASK                       0xFFFF
#define IFX_BCU_CON_SPC(value)                  (((( 1 << 8) - 1) & (value)) << 24)
#define IFX_BCU_CON_SPE                         (1 << 19)
#define IFX_BCU_CON_PSE                         (1 << 18)
#define IFX_BCU_CON_DBG                         (1 << 16)
#define IFX_BCU_CON_TOUT(value)                 (((( 1 << 16) - 1) & (value)) << 0)

/***BCU Error Control Capture Register (0020H)***/
#define IFX_BCU_ECON                            ((volatile u32*)(0x0020 + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_ECON                      ((volatile u32*)(0x0020 + IFX_SLAVE_BCU_BASE_ADDR))
#define IFX_BCU_ECON_TAG(value)                 (((( 1 << 4) - 1) & (value)) << 24)
#define IFX_BCU_ECON_RDN                        (1 << 23)
#define IFX_BCU_ECON_WRN                        (1 << 22)
#define IFX_BCU_ECON_SVM                        (1 << 21)
#define IFX_BCU_ECON_ACK(value)                 (((( 1 << 2) - 1) & (value)) << 19)
#define IFX_BCU_ECON_ABT                        (1 << 18)
#define IFX_BCU_ECON_RDY                        (1 << 17)
#define IFX_BCU_ECON_TOUT                       (1 << 16)
#define IFX_BCU_ECON_ERRCNT(value)              (((( 1 << 16) - 1) & (value)) << 0)
#define IFX_BCU_ECON_OPC(value)                 (((( 1 << 4) - 1) & (value)) << 28)

/***BCU Error Address Capture Register (0024 H)***/
#define IFX_BCU_EADD                            ((volatile u32*)(0x0024 + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_EADD                      ((volatile u32*)(0x0024 + IFX_SLAVE_BCU_BASE_ADDR))

/***BCU Error Data Capture Register (0028H)***/
#define IFX_BCU_EDAT                            ((volatile u32*)(0x0028 + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_EDAT                      ((volatile u32*)(0x0028 + IFX_SLAVE_BCU_BASE_ADDR))
#define IFX_BCU_IRNEN                           ((volatile u32*)(0x00F4 + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_IRNEN                     ((volatile u32*)(0x00F4 + IFX_SLAVE_BCU_BASE_ADDR))
#define IFX_BCU_IRNICR                          ((volatile u32*)(0x00F8 + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_IRNICR                    ((volatile u32*)(0x00F8 + IFX_SLAVE_BCU_BASE_ADDR))
#define IFX_BCU_IRNCR                           ((volatile u32*)(0x00FC + IFX_BCU_BASE_ADDR))
#define IFX_SLAVE_BCU_IRNCR                     ((volatile u32*)(0x00FC + IFX_SLAVE_BCU_BASE_ADDR))



/***********************************************************************/
/*  Module      :  MEI register address and bits                       */
/***********************************************************************/

#define IFX_MEI_SPACE_ACCESS                    (KSEG1 | 0x1E116000)
#define IFX_DFE_LDST_BASE_ADDR                  (KSEG1 | 0x1EF00000)

/***    Register address offsets, relative to MEI_SPACE_ADDRESS ***/
#define IFX_MEI_DATA_XFR                        ((volatile u32*)(0x0000 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_VERSION                         ((volatile u32*)(0x0004 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_ARC_GP_STAT                     ((volatile u32*)(0x0008 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_DATA_XFR_STAT                   ((volatile u32*)(0x000C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XFR_ADDR                        ((volatile u32*)(0x0010 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_MAX_WAIT                        ((volatile u32*)(0x0014 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_TO_ARC_INT                      ((volatile u32*)(0x0018 + IFX_MEI_SPACE_ACCESS))
#define IFX_ARC_TO_MEI_INT                      ((volatile u32*)(0x001C + IFX_MEI_SPACE_ACCESS))
#define IFX_ARC_TO_MEI_INT_MASK                 ((volatile u32*)(0x0020 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_DEBUG_WAD                       ((volatile u32*)(0x0024 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_DEBUG_RAD                       ((volatile u32*)(0x0028 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_DEBUG_DATA                      ((volatile u32*)(0x002C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_DEBUG_DEC                       ((volatile u32*)(0x0030 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_CONFIG                          ((volatile u32*)(0x0034 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_RST_CONTROL                     ((volatile u32*)(0x0038 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_DBG_MASTER                      ((volatile u32*)(0x003C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_CLK_CONTROL                     ((volatile u32*)(0x0040 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_BIST_CONTROL                    ((volatile u32*)(0x0044 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_BIST_STAT                       ((volatile u32*)(0x0048 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XDATA_BASE_SH                   ((volatile u32*)(0x004c + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XDATA_BASE                      ((volatile u32*)(0x0050 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR_BASE                   ((volatile u32*)(0x0054 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR0                       ((volatile u32*)(0x0054 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR1                       ((volatile u32*)(0x0058 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR2                       ((volatile u32*)(0x005C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR3                       ((volatile u32*)(0x0060 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR4                       ((volatile u32*)(0x0064 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR5                       ((volatile u32*)(0x0068 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR6                       ((volatile u32*)(0x006C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR7                       ((volatile u32*)(0x0070 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR8                       ((volatile u32*)(0x0074 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR9                       ((volatile u32*)(0x0078 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR10                      ((volatile u32*)(0x007C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR11                      ((volatile u32*)(0x0080 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR12                      ((volatile u32*)(0x0084 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR13                      ((volatile u32*)(0x0088 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR14                      ((volatile u32*)(0x008C + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR15                      ((volatile u32*)(0x0090 + IFX_MEI_SPACE_ACCESS))
#define IFX_MEI_XMEM_BAR16                      ((volatile u32*)(0x0094 + IFX_MEI_SPACE_ACCESS))


/***********************************************************************/
/*  Module      :  GPIO register address and bits                      */
/***********************************************************************/

#define IFX_GPIO                                (KSEG1 | 0x1E100B00)

#define IFX_GPIO_Pn_BASE(n)                     (IFX_GPIO + 0x0010 + 0x0030 * (n))  //  only valid for first 3 ports (0-2)

/***Port 0 Data Output Register (0010H)***/
#define IFX_GPIO_P0_OUT                         ((volatile u32 *)(IFX_GPIO + 0x0010))
/***Port 1 Data Output Register (0040H)***/
#define IFX_GPIO_P1_OUT                         ((volatile u32 *)(IFX_GPIO + 0x0040))
/***Port 2 Data Output Register (0070H)***/
#define IFX_GPIO_P2_OUT                         ((volatile u32 *)(IFX_GPIO + 0x0070))
/***Port 3 Data Output Register (00A0H)***/
#define IFX_GPIO_P3_OUT                         ((volatile u32 *)(IFX_GPIO + 0x00A0))
/***Port 0 Data Input Register (0014H)***/
#define IFX_GPIO_P0_IN                          ((volatile u32 *)(IFX_GPIO + 0x0014))
/***Port 1 Data Input Register (0044H)***/
#define IFX_GPIO_P1_IN                          ((volatile u32 *)(IFX_GPIO + 0x0044))
/***Port 2 Data Input Register (0074H)***/
#define IFX_GPIO_P2_IN                          ((volatile u32 *)(IFX_GPIO + 0x0074))
/***Port 3 Data Input Register (00A4H)***/
#define IFX_GPIO_P3_IN                          ((volatile u32 *)(IFX_GPIO + 0x00A4))
/***Port 0 Direction Register (0018H)***/
#define IFX_GPIO_P0_DIR                         ((volatile u32 *)(IFX_GPIO + 0x0018))
/***Port 1 Direction Register (0048H)***/
#define IFX_GPIO_P1_DIR                         ((volatile u32 *)(IFX_GPIO + 0x0048))
/***Port 2 Direction Register (0078H)***/
#define IFX_GPIO_P2_DIR                         ((volatile u32 *)(IFX_GPIO + 0x0078))
/***Port 3 Direction Register (0048H)***/
#define IFX_GPIO_P3_DIR                         ((volatile u32 *)(IFX_GPIO + 0x00A8))
/***Port 0 Alternate Function Select Register 0 (001C H) ***/
#define IFX_GPIO_P0_ALTSEL0                     ((volatile u32 *)(IFX_GPIO + 0x001C))
/***Port 1 Alternate Function Select Register 0 (004C H) ***/
#define IFX_GPIO_P1_ALTSEL0                     ((volatile u32 *)(IFX_GPIO + 0x004C))
/***Port 2 Alternate Function Select Register 0 (007C H) ***/
#define IFX_GPIO_P2_ALTSEL0                     ((volatile u32 *)(IFX_GPIO + 0x007C))
/***Port 3 Alternate Function Select Register 0 (00AC H) ***/
#define IFX_GPIO_P3_ALTSEL0                     ((volatile u32 *)(IFX_GPIO + 0x00AC))
/***Port 0 Alternate Function Select Register 1 (0020 H) ***/
#define IFX_GPIO_P0_ALTSEL1                     ((volatile u32 *)(IFX_GPIO + 0x0020))
/***Port 1 Alternate Function Select Register 0 (0050 H) ***/
#define IFX_GPIO_P1_ALTSEL1                     ((volatile u32 *)(IFX_GPIO + 0x0050))
/***Port 2 Alternate Function Select Register 0 (0080 H) ***/
#define IFX_GPIO_P2_ALTSEL1                     ((volatile u32 *)(IFX_GPIO + 0x0080))
/***Port 3 Alternate Function Select Register 0 (0064 H) ***/
#define IFX_GPIO_P3_ALTSEL1                     ((volatile u32 *)(IFX_GPIO + 0x0064))
/***Port 0 Open Drain Control Register (0024H)***/
#define IFX_GPIO_P0_OD                          ((volatile u32 *)(IFX_GPIO + 0x0024))
/***Port 1 Open Drain Control Register (0054H)***/
#define IFX_GPIO_P1_OD                          ((volatile u32 *)(IFX_GPIO + 0x0054))
/***Port 2 Open Drain Control Register (0084H)***/
#define IFX_GPIO_P2_OD                          ((volatile u32 *)(IFX_GPIO + 0x0084))
/***Port 3 Open Drain Control Register (0034H)***/
#define IFX_GPIO_P3_OD                          ((volatile u32 *)(IFX_GPIO + 0x0034))
/***Port 0 Input Schmitt-Trigger Off Register (0028 H) ***/
#define IFX_GPIO_P0_STOFF                       ((volatile u32 *)(IFX_GPIO + 0x0028))
/***Port 1 Input Schmitt-Trigger Off Register (0058 H) ***/
#define IFX_GPIO_P1_STOFF                       ((volatile u32 *)(IFX_GPIO + 0x0058))
/***Port 2 Input Schmitt-Trigger Off Register (0088 H) ***/
#define IFX_GPIO_P2_STOFF                       ((volatile u32 *)(IFX_GPIO + 0x0088))
/***Port 3 Input Schmitt-Trigger Off Register (0094 H) ***/
//  not available
/***Port 0 Pull Up/Pull Down Select Register (002C H)***/
#define IFX_GPIO_P0_PUDSEL                      ((volatile u32 *)(IFX_GPIO + 0x002C))
/***Port 1 Pull Up/Pull Down Select Register     (005C H)***/
#define IFX_GPIO_P1_PUDSEL                      ((volatile u32 *)(IFX_GPIO + 0x005C))
/***Port 2 Pull Up/Pull Down Select Register     (008C H)***/
#define IFX_GPIO_P2_PUDSEL                      ((volatile u32 *)(IFX_GPIO + 0x008C))
/***Port 3 Pull Up/Pull Down Select Register     (0038 H)***/
#define IFX_GPIO_P3_PUDSEL                      ((volatile u32 *)(IFX_GPIO + 0x0038))
/***Port 0 Pull Up Device Enable Register (0030 H)***/
#define IFX_GPIO_P0_PUDEN                       ((volatile u32 *)(IFX_GPIO + 0x0030))
/***Port 1 Pull Up Device Enable Register (0060 H)***/
#define IFX_GPIO_P1_PUDEN                       ((volatile u32 *)(IFX_GPIO + 0x0060))
/***Port 2 Pull Up Device Enable Register (0090 H)***/
#define IFX_GPIO_P2_PUDEN                       ((volatile u32 *)(IFX_GPIO + 0x0090))
/***Port 3 Pull Up Device Enable Register (003c H)***/
#define IFX_GPIO_P3_PUDEN                       ((volatile u32 *)(IFX_GPIO + 0x003C))



/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/

#define IFX_CGU                                 (KSEG1 | 0x1F103000)

/***CGU Clock PLL0 ***/
#define IFX_CGU_PLL0_CFG                        ((volatile u32*)(IFX_CGU + 0x0004))
/***CGU Clock PLL1 ***/
#define IFX_CGU_PLL1_CFG                        ((volatile u32*)(IFX_CGU + 0x0008))
/***CGU Clock PLL2 ***/
#define IFX_CGU_PLL2_CFG			            ((volatile u32*)(IFX_CGU + 0x000C))
/***CGU Clock SYS Mux Register***/
#define IFX_CGU_SYS                             ((volatile u32*)(IFX_CGU + 0x0010))

#define IFX_CGU_SYS_SEL                         0x00000018
#define IFX_CGU_SYS_SEL_S                       3
#define IFX_CGU_SYS_SEL_333                     0
#define IFX_CGU_SYS_SEL_393                     2

/**Update CGU Register***/
#define IFX_CGU_UPDATE             		        ((volatile u32*)(IFX_CGU + 0x0014))
/***CGU Interface Clock Register***/
#define IFX_CGU_IF_CLK                          ((volatile u32*)(IFX_CGU + 0x0018))
/***CGU SDRAM Memory Control Register***/
#define IFX_CGU_SMD                		        ((volatile u32*)(IFX_CGU + 0x0020))
/***CGU CT Status Register 1***/
#define IFX_CGU_CT1SR              		        ((volatile u32*)(IFX_CGU + 0x0028))
/***CGU CT Kval Register***/
#define IFX_CGU_CT_KVAL            		        ((volatile u32*)(IFX_CGU + 0x002C))
/***CGU PCM Control Register***/
#define IFX_CGU_PCMCR              		        ((volatile u32*)(IFX_CGU + 0x0030))
/***CGU PCI Clock Control Register***/
#define IFX_CGU_PCI_CR             		        ((volatile u32*)(IFX_CGU + 0x0034))

#define IFX_PCI_CLK_SHIFT			 20
#define IFX_PCI_CLK_MASK			(0x1F << IFX_PCI_CLK_SHIFT)
#define IFX_PCI_33MHZ			    (0xe << IFX_PCI_CLK_SHIFT)
#define IFX_PCI_60MHZ			    (0x7 << IFX_PCI_CLK_SHIFT)
#define IFX_PCI_INTERNAL_CLK_SRC    0x00010000 /* Internal means output */

#define IFX_PCI_CLK_FROM_CGU        0x80000000
#define IFX_PCI_CLK_RESET_FROM_CGU  0x40000000
#define IFX_PCI_DELAY_SHIFT         21
#define IFX_PCI_DELAY_MASK          (0x7 << IFX_PCI_DELAY_SHIFT)

/***********************************************************************/
/*  Module      :  MCD register address and bits                       */
/***********************************************************************/

#define IFX_MCD                                 (KSEG1 | 0x1F106000)

/***Manufacturer Identification Register***/
#define IFX_MCD_MANID                           ((volatile u32*)(IFX_MCD + 0x0024))
#define IFX_MCD_MANID_MANUF(value)              (((( 1 << 11) - 1) & (value)) << 5)

/***Chip Identification Register***/
#define IFX_MCD_CHIPID                          ((volatile u32*)(IFX_MCD + 0x0028))
#define IFX_MCD_CHIPID_VERSION_GET(value)       (((value) >> 28) & ((1 << 4) - 1))
#define IFX_MCD_CHIPID_VERSION_SET(value)       (((( 1 << 4) - 1) & (value)) << 28)
#define IFX_MCD_CHIPID_PART_NUMBER_GET(value)   (((value) >> 12) & ((1 << 16) - 1))
#define IFX_MCD_CHIPID_PART_NUMBER_SET(value)   (((( 1 << 16) - 1) & (value)) << 12)
#define IFX_MCD_CHIPID_MANID_GET(value)         (((value) >> 1) & ((1 << 11) - 1))
#define IFX_MCD_CHIPID_MANID_SET(value)         (((( 1 << 11) - 1) & (value)) << 1)

#define IFX_CHIPID_STANDARD                     0x00EB
#define IFX_CHIPID_YANGTSE                      0x00ED

/***Redesign Tracing Identification Register***/
#define IFX_MCD_RTID                            ((volatile u32*)(IFX_MCD + 0x002C))
#define IFX_MCD_RTID_LC                         (1 << 15)
#define IFX_MCD_RTID_RIX(value)                 (((( 1 << 3) - 1) & (value)) << 0)



/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/

#define IFX_EBU                                 (KSEG1 | 0x1E105300)

/***EBU Clock Control Register***/
#define IFX_EBU_CLC                             ((volatile u32*)(IFX_EBU + 0x0000))
#define IFX_EBU_CLC_DISS                        (1 << 1)
#define IFX_EBU_CLC_DISR                        (1 << 0)

#define IFX_EBU_ID                              ((volatile u32*)(IFX_EBU + 0x0008))

/***EBU Global Control Register***/
#define IFX_EBU_CON                             ((volatile u32*)(IFX_EBU + 0x0010))
#define IFX_EBU_CON_DTACS(value)                (((( 1 << 3) - 1) & (value)) << 20)
#define IFX_EBU_CON_DTARW(value)                (((( 1 << 3) - 1) & (value)) << 16)
#define IFX_EBU_CON_TOUTC(value)                (((( 1 << 8) - 1) & (value)) << 8)
#define IFX_EBU_CON_ARBMODE(value)              (((( 1 << 2) - 1) & (value)) << 6)
#define IFX_EBU_CON_ARBSYNC                     (1 << 5)

/***EBU Address Select Register 0***/
#define IFX_EBU_ADDSEL0                         ((volatile u32*)(IFX_EBU + 0x0020))
#define IFX_EBU_ADDSEL0_BASE(value)             (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL0_MASK(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL0_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL0_REGEN                   (1 << 0)

/***EBU Address Select Register 1***/
#define IFX_EBU_ADDSEL1                         ((volatile u32*)(IFX_EBU + 0x0024))
#define IFX_EBU_ADDSEL1_BASE(value)             (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL1_MASK(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL1_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL1_REGEN                   (1 << 0)

/***EBU Address Select Register 2***/
#define IFX_EBU_ADDSEL2                         ((volatile u32*)(IFX_EBU + 0x0028))
#define IFX_EBU_ADDSEL2_BASE(value)             (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL2_MASK(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL2_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL2_REGEN                   (1 << 0)

/***EBU Address Select Register 3***/
#define IFX_EBU_ADDSEL3                         ((volatile u32*)(IFX_EBU + 0x002C))
#define IFX_EBU_ADDSEL3_BASE(value)             (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL3_MASK(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL3_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL3_REGEN                   (1 << 0)

/***EBU Bus Configuration Register 0***/
#define IFX_EBU_BUSCON0                         ((volatile u32*)(IFX_EBU + 0x0060))

#define IFX_EBU_BUSCON0_CMULT                   0x00000003
#define IFX_EBU_BUSCON0_CMULT_S                 0
enum {
    IFX_EBU_BUSCON0_CMULT1 = 0,
    IFX_EBU_BUSCON0_CMULT4,
    IFX_EBU_BUSCON0_CMULT8,
    IFX_EBU_BUSCON0_CMULT16,  /* Default after reset */
};

#define IFX_EBU_BUSCON0_RECOVC                  0x00000000c
#define IFX_EBU_BUSCON0_RECOVC_S                2
enum {
    IFX_EBU_BUSCON0_RECOVC0 = 0,
    IFX_EBU_BUSCON0_RECOVC1,
    IFX_EBU_BUSCON0_RECOVC2,
    IFX_EBU_BUSCON0_RECOVC3,  /* Default */
};
#define IFX_EBU_BUSCON0_HOLDC                   0x00000030
#define IFX_EBU_BUSCON0_HOLDC_S                 4
enum {
    IFX_EBU_BUSCON0_HOLDC0 = 0,
    IFX_EBU_BUSCON0_HOLDC1,
    IFX_EBU_BUSCON0_HOLDC2,
    IFX_EBU_BUSCON0_HOLDC3,  /* Default */
};
#define IFX_EBU_BUSCON0_WAITRDC                 0x000000c0
#define IFX_EBU_BUSCON0_WAITRDC_S               6
enum {
    IFX_EBU_BUSCON0_WAITRDC0 = 0,
    IFX_EBU_BUSCON0_WAITRDC1,
    IFX_EBU_BUSCON0_WAITRDC2,
    IFX_EBU_BUSCON0_WAITRDC3,  /* Default */
};
#define IFX_EBU_BUSCON0_WAITWRC                 0x00000700
#define IFX_EBU_BUSCON0_WAITWRC_S               8
enum {
    IFX_EBU_BUSCON0_WAITWRC0 = 0,
    IFX_EBU_BUSCON0_WAITWRC1,
    IFX_EBU_BUSCON0_WAITWRC2,
    IFX_EBU_BUSCON0_WAITWRC3,
    IFX_EBU_BUSCON0_WAITWRC4,
    IFX_EBU_BUSCON0_WAITWRC5,
    IFX_EBU_BUSCON0_WAITWRC6,
    IFX_EBU_BUSCON0_WAITWRC7, /* Default */
};

#define IFX_EBU_BUSCON0_BCGEN                   0x00003000
#define IFX_EBU_BUSCON0_BCGEN_S                 12
enum {
    IFX_EBU_BUSCON0_BCGEN_CS = 0,
    IFX_EBU_BUSCON0_BCGEN_INTEL, /* Default */
    IFX_EBU_BUSCON0_BCGEN_MOTOROLA,
    IFX_EBU_BUSCON0_BCGEN_RES,
};

#define IFX_EBU_BUSCON0_ALEC                    0x0000c000
#define IFX_EBU_BUSCON0_ALEC_S                  14
enum {
    IFX_EBU_BUSCON0_ALEC0 = 0,
    IFX_EBU_BUSCON0_ALEC1,
    IFX_EBU_BUSCON0_ALEC2,
    IFX_EBU_BUSCON0_ALEC3,   /* Default */
};

#define IFX_EBU_BUSCON0_XDM                     0x00030000
#define IFX_EBU_BUSCON0_XDM_S                   16
enum {
    IFX_EBU_BUSCON0_XDM8 = 0,
    IFX_EBU_BUSCON0_XDM16,  /* Default */
};

#define IFX_EBU_BUSCON0_VN_EN                   0x00040000

#define IFX_EBU_BUSCON0_WAITINV_HI              0x00080000 /* low by default */

#define IFX_EBU_BUSCON0_WAIT                    0x00300000
#define IFX_EBU_BUSCON0_WAIT_S                  20
enum {
    IFX_EBU_BUSCON0_WAIT_DISABLE = 0,
    IFX_EBU_BUSCON0_WAIT_ASYNC,
    IFX_EBU_BUSCON0_WAIT_SYNC,
};
#define IFX_EBU_BUSCON0_SETUP_EN                0x00400000 /* Disable by default */

#define IFX_EBU_BUSCON0_AGEN                    0x07000000
#define IFX_EBU_BUSCON0_AGEN_S                  24
enum {
    IFX_EBU_BUSCON0_AGEN_DEMUX = 0, /* Default */
    IFX_EBU_BUSCON0_AGEN_RES,
    IFX_EBU_BUSCON0_AGEN_MUX,
};

#define IFX_EBU_BUSCON0_PG_EN                   0x20000000
#define IFX_EBU_BUSCON0_ADSWP                   0x40000000 /* Disable by default */
#define IFX_EBU_BUSCON0_WRDIS                   0x80000000 /* Disable by default */

/***EBU Bus Configuration Register 1***/
#define IFX_EBU_BUSCON1                         ((volatile u32*)(IFX_EBU + 0x0064))
#define IFX_EBU_BUSCON1_CMULT                   0x00000003
#define IFX_EBU_BUSCON1_CMULT_S                 0
enum {
    IFX_EBU_BUSCON1_CMULT1 = 0,
    IFX_EBU_BUSCON1_CMULT4,
    IFX_EBU_BUSCON1_CMULT8,
    IFX_EBU_BUSCON1_CMULT16,  /* Default after reset */
};

#define IFX_EBU_BUSCON1_RECOVC                  0x00000000c
#define IFX_EBU_BUSCON1_RECOVC_S                2
enum {
    IFX_EBU_BUSCON1_RECOVC0 = 0,
    IFX_EBU_BUSCON1_RECOVC1,
    IFX_EBU_BUSCON1_RECOVC2,
    IFX_EBU_BUSCON1_RECOVC3,  /* Default */
};
#define IFX_EBU_BUSCON1_HOLDC                   0x00000030
#define IFX_EBU_BUSCON1_HOLDC_S                 4
enum {
    IFX_EBU_BUSCON1_HOLDC0 = 0,
    IFX_EBU_BUSCON1_HOLDC1,
    IFX_EBU_BUSCON1_HOLDC2,
    IFX_EBU_BUSCON1_HOLDC3,  /* Default */
};
#define IFX_EBU_BUSCON1_WAITRDC                 0x000000c0
#define IFX_EBU_BUSCON1_WAITRDC_S               6
enum {
    IFX_EBU_BUSCON1_WAITRDC0 = 0,
    IFX_EBU_BUSCON1_WAITRDC1,
    IFX_EBU_BUSCON1_WAITRDC2,
    IFX_EBU_BUSCON1_WAITRDC3,  /* Default */
};
#define IFX_EBU_BUSCON1_WAITWRC                 0x00000700
#define IFX_EBU_BUSCON1_WAITWRC_S               8
enum {
    IFX_EBU_BUSCON1_WAITWRC0 = 0,
    IFX_EBU_BUSCON1_WAITWRC1,
    IFX_EBU_BUSCON1_WAITWRC2,
    IFX_EBU_BUSCON1_WAITWRC3,
    IFX_EBU_BUSCON1_WAITWRC4,
    IFX_EBU_BUSCON1_WAITWRC5,
    IFX_EBU_BUSCON1_WAITWRC6,
    IFX_EBU_BUSCON1_WAITWRC7, /* Default */
};
#define IFX_EBU_BUSCON1_BCGEN                   0x00003000
#define IFX_EBU_BUSCON1_BCGEN_S                 12
enum {
    IFX_EBU_BUSCON1_BCGEN_CS = 0,
    IFX_EBU_BUSCON1_BCGEN_INTEL, /* Default */
    IFX_EBU_BUSCON1_BCGEN_MOTOROLA,
    IFX_EBU_BUSCON1_BCGEN_RES,
};
#define IFX_EBU_BUSCON1_ALEC                    0x0000c000
#define IFX_EBU_BUSCON1_ALEC_S                  14
enum {
    IFX_EBU_BUSCON1_ALEC0 = 0,
    IFX_EBU_BUSCON1_ALEC1,
    IFX_EBU_BUSCON1_ALEC2,
    IFX_EBU_BUSCON1_ALEC3,   /* Default */
};
#define IFX_EBU_BUSCON1_SETUP                   (1 << 22)
#define IFX_EBU_BUSCON1_WRDIS                   (1 << 31)
//#define IFX_EBU_BUSCON1_ALEC (value)            (((( 1 << 2) - 1) & (value)) << 29)
//#define IFX_EBU_BUSCON1_BCGEN (value)           (((( 1 << 2) - 1) & (value)) << 27)
#define IFX_EBU_BUSCON1_AGEN(value)             (((( 1 << 2) - 1) & (value)) << 24)
#define IFX_EBU_BUSCON1_CMULTR(value)           (((( 1 << 2) - 1) & (value)) << 22)
#define IFX_EBU_BUSCON1_WAIT(value)             (((( 1 << 2) - 1) & (value)) << 20)
#define IFX_EBU_BUSCON1_WAITINV                 (1 << 19)
#define IFX_EBU_BUSCON1_PORTW(value)            (((( 1 << 2) - 1) & (value)) << 16)
//#define IFX_EBU_BUSCON1_WAITRDC (value)         (((( 1 << 7) - 1) & (value)) << 9)
//#define IFX_EBU_BUSCON1_WAITWRC (value)         (((( 1 << 3) - 1) & (value)) << 6)
//#define IFX_EBU_BUSCON1_HOLDC (value)           (((( 1 << 2) - 1) & (value)) << 4)
//#define IFX_EBU_BUSCON1_RECOVC (value)          (((( 1 << 2) - 1) & (value)) << 2)
//#define IFX_EBU_BUSCON1_CMULT (value)           (((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 2***/
#define IFX_EBU_BUSCON2                         ((volatile u32*)(IFX_EBU + 0x0068))
#define IFX_EBU_BUSCON2_WRDIS                   (1 << 31)
#define IFX_EBU_BUSCON2_ALEC(value)             (((( 1 << 2) - 1) & (value)) << 29)
#define IFX_EBU_BUSCON2_BCGEN(value)            (((( 1 << 2) - 1) & (value)) << 27)
#define IFX_EBU_BUSCON2_AGEN(value)             (((( 1 << 2) - 1) & (value)) << 24)
#define IFX_EBU_BUSCON2_CMULTR(value)           (((( 1 << 2) - 1) & (value)) << 22)
#define IFX_EBU_BUSCON2_WAIT(value)             (((( 1 << 2) - 1) & (value)) << 20)
#define IFX_EBU_BUSCON2_WAITINV                 (1 << 19)
#define IFX_EBU_BUSCON2_SETUP                   (1 << 18)
#define IFX_EBU_BUSCON2_PORTW(value)            (((( 1 << 2) - 1) & (value)) << 16)
#define IFX_EBU_BUSCON2_WAITRDC(value)          (((( 1 << 7) - 1) & (value)) << 9)
#define IFX_EBU_BUSCON2_WAITWRC(value)          (((( 1 << 3) - 1) & (value)) << 6)
#define IFX_EBU_BUSCON2_HOLDC(value)            (((( 1 << 2) - 1) & (value)) << 4)
#define IFX_EBU_BUSCON2_RECOVC(value)           (((( 1 << 2) - 1) & (value)) << 2)
#define IFX_EBU_BUSCON2_CMULT(value)            (((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 2***/
#define IFX_EBU_BUSCON3                         ((volatile u32*)(IFX_EBU + 0x006C))
#define IFX_EBU_BUSCON3_WRDIS                   (1 << 31)
#define IFX_EBU_BUSCON3_ADSWP(value)            (1 << 30)
#define IFX_EBU_BUSCON3_PG_EN(value)            (1 << 29)
#define IFX_EBU_BUSCON3_AGEN(value)             (((( 1 << 3) - 1) & (value)) << 24)
#define IFX_EBU_BUSCON3_SETUP                   (1 << 22)
#define IFX_EBU_BUSCON3_WAIT(value)             (((( 1 << 2) - 1) & (value)) << 20)
#define IFX_EBU_BUSCON3_WAITINV                 (1 << 19)
#define IFX_EBU_BUSCON3_VN_EN                   (1 << 18)
#define IFX_EBU_BUSCON3_PORTW(value)            (((( 1 << 2) - 1) & (value)) << 16)
#define IFX_EBU_BUSCON3_ALEC(value)             (((( 1 << 2) - 1) & (value)) << 14)
#define IFX_EBU_BUSCON3_BCGEN(value)            (((( 1 << 2) - 1) & (value)) << 12)
#define IFX_EBU_BUSCON3_WAITWDC(value)          (((( 1 << 4) - 1) & (value)) << 8)
#define IFX_EBU_BUSCON3_WAITRRC(value)          (((( 1 << 2) - 1) & (value)) << 6)
#define IFX_EBU_BUSCON3_HOLDC(value)            (((( 1 << 2) - 1) & (value)) << 4)
#define IFX_EBU_BUSCON3_RECOVC(value)           (((( 1 << 2) - 1) & (value)) << 2)
#define IFX_EBU_BUSCON3_CMULT(value)            (((( 1 << 2) - 1) & (value)) << 0)

/* PC Card Control Register */
#define IFX_EBU_PCC_CON                         ((volatile u32*)(IFX_EBU+ 0x0090))
#define IFX_EBU_PCC_CON_PCCARD_ON               0x00000001
#define IFX_EBU_PCC_CON_IREQ_RISING_EDGE        0x00000002
#define IFX_EBU_PCC_CON_IREQ_FALLING_EDGE       0x00000004
#define IFX_EBU_PCC_CON_IREQ_BOTH_EDGE          0x00000006
#define IFX_EBU_PCC_CON_IREQ_DIS                0x00000008
#define IFX_EBU_PCC_CON_IREQ_HIGH_LEVEL_DETECT  0x0000000A
#define IFX_EBU_PCC_CON_IREQ_LOW_LEVEL_DETECT   0x0000000C

#define IFX_EBU_PCC_STAT                        ((volatile u32*)(IFX_EBU+ 0x0094))
#define IFX_EBU_PCC_ISTAT                       ((volatile u32*)(IFX_EBU+ 0x00A0))
#define IFX_EBU_PCC_IEN                         ((volatile u32*)(IFX_EBU+ 0x00A4))
#define IFX_EBU_PCC_IEN_PCI_EN                  0x00000010

#define IFX_EBU_PCC_INT_OUT                     ((volatile u32*)(IFX_EBU+ 0x00A8))
#define IFX_EBU_PCC_IRS                         ((volatile u32*)(IFX_EBU+ 0x00AC))

#define IFX_EBU_NAND_CON                        (volatile u32*)(IFX_EBU + 0xB0)
#define IFX_EBU_NAND_CON_NANDM                  (1<<0)
#define IFX_EBU_NAND_CON_NANDM_S                 0
enum {
    IFX_EBU_NAND_CON_NANDM_DISABLE = 0,/* Default after reset */
    IFX_EBU_NAND_CON_NANDM_ENABLE,
};

#define IFX_EBU_NAND_CON_CSMUX_E                 (1<<1)
#define IFX_EBU_NAND_CON_CSMUX_E_S                 1
enum {
    IFX_EBU_NAND_CON_CSMUX_E_DISABLE = 0,/* Default after reset */
    IFX_EBU_NAND_CON_CSMUX_E_ENABLE,
};

#define IFX_EBU_NAND_CON_CS_P                   (1<<4)
#define IFX_EBU_NAND_CON_CS_P_S                 4
enum {
    IFX_EBU_NAND_CON_CS_P_HIGH = 0,
    IFX_EBU_NAND_CON_CS_P_LOW,     /* Default after reset */
};

#define IFX_EBU_NAND_CON_SE_P                   (1<<5)
#define IFX_EBU_NAND_CON_SE_P_S                 5
enum {
    IFX_EBU_NAND_CON_SE_P_HIGH = 0,
    IFX_EBU_NAND_CON_SE_P_LOW,     /* Default after reset */
};

#define IFX_EBU_NAND_CON_WP_P                   (1<<6)
#define IFX_EBU_NAND_CON_WP_P_S                 6
enum {
    IFX_EBU_NAND_CON_WP_P_HIGH = 0,
    IFX_EBU_NAND_CON_WP_P_LOW,     /* Default after reset */
};

#define IFX_EBU_NAND_CON_PRE_P                   (1<<7)
#define IFX_EBU_NAND_CON_PRE_P_S                 7
enum {
    IFX_EBU_NAND_CON_PRE_P_HIGH = 0,
    IFX_EBU_NAND_CON_PRE_P_LOW,     /* Default after reset */
};

#define IFX_EBU_NAND_CON_IN_CS                   (3<<8)
#define IFX_EBU_NAND_CON_IN_CS_S                 8
enum {
    IFX_EBU_NAND_CON_IN_CS0 = 0,    /* Default after reset */
    IFX_EBU_NAND_CON_IN_CS1,
};

#define IFX_EBU_NAND_CON_OUT_CS                   (3<<10)
#define IFX_EBU_NAND_CON_OUT_CS_S                 10
enum {
    IFX_EBU_NAND_CON_OUT_CS0 = 0,   /* Default after reset */
    IFX_EBU_NAND_CON_OUT_CS1,
};
#define IFX_EBU_NAND_WAIT                       (volatile u32*)(IFX_EBU + 0xB4)
#define IFX_EBU_NAND_WAIT_RD                    (0x1)
#define IFX_EBU_NAND_WAIT_BY_E                  (1<<1)
#define IFX_EBU_NAND_WAIT_RD_E                  (1<<2)
#define IFX_EBU_NAND_WAIT_WR_C                  (1<<3)

#define IFX_EBU_NAND_ECC0                       (volatile u32*)(IFX_EBU + 0xB8)
#define IFX_EBU_NAND_ECC_AC                     (volatile u32*)(IFX_EBU + 0xBC)




/***********************************************************************/
/*  Module      :  SDRAM register address and bits                     */
/***********************************************************************/

#define IFX_SDRAM                               (KSEG1 | 0x1F800000)

/***MC Access Error Cause Register***/
#define IFX_SDRAM_MC_ERRCAUSE                   ((volatile u32*)(IFX_SDRAM + 0x0100))
#define IFX_SDRAM_MC_ERRCAUSE_ERR               (1 << 31)
#define IFX_SDRAM_MC_ERRCAUSE_PORT(value)       (((( 1 << 4) - 1) & (value)) << 16)
#define IFX_SDRAM_MC_ERRCAUSE_CAUSE(value)      (((( 1 << 2) - 1) & (value)) << 0)
#define IFX_SDRAM_MC_ERRCAUSE_Res(value)        (((( 1 << NaN) - 1) & (value)) << NaN)

/***MC Access Error Address Register***/
#define IFX_SDRAM_MC_ERRADDR                    ((volatile u32*)(IFX_SDRAM + 0x0108))

/***MC I/O General Purpose Register***/
#define IFX_SDRAM_MC_IOGP                       ((volatile u32*)(IFX_SDRAM + 0x0800))
#define IFX_SDRAM_MC_IOGP_GPR6(value)           (((( 1 << 4) - 1) & (value)) << 28)
#define IFX_SDRAM_MC_IOGP_GPR5(value)           (((( 1 << 4) - 1) & (value)) << 24)
#define IFX_SDRAM_MC_IOGP_GPR4(value)           (((( 1 << 4) - 1) & (value)) << 20)
#define IFX_SDRAM_MC_IOGP_GPR3(value)           (((( 1 << 4) - 1) & (value)) << 16)
#define IFX_SDRAM_MC_IOGP_GPR2(value)           (((( 1 << 4) - 1) & (value)) << 12)
#define IFX_SDRAM_MC_IOGP_CPS                   (1 << 11)
#define IFX_SDRAM_MC_IOGP_CLKDELAY(value)       (((( 1 << 3) - 1) & (value)) << 8)
#define IFX_SDRAM_MC_IOGP_CLKRAT(value)         (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_SDRAM_MC_IOGP_RDDEL(value)          (((( 1 << 4) - 1) & (value)) << 0)

/***MC Self Refresh Register***/
#define IFX_SDRAM_MC_SELFRFSH                   ((volatile u32*)(IFX_SDRAM + 0x0A00))
#define IFX_SDRAM_MC_SELFRFSH_PWDS              (1 << 1)
#define IFX_SDRAM_MC_SELFRFSH_PWD               (1 << 0)
#define IFX_SDRAM_MC_SELFRFSH_Res(value)        (((( 1 << 30) - 1) & (value)) << 2)

/***MC Enable Register***/
#define IFX_SDRAM_MC_CTRLENA                    ((volatile u32*)(IFX_SDRAM + 0x1000))
#define IFX_SDRAM_MC_CTRLENA_ENA                (1 << 0)
#define IFX_SDRAM_MC_CTRLENA_Res(value)         (((( 1 << 31) - 1) & (value)) << 1)

/***MC Mode Register Setup Code***/
#define IFX_SDRAM_MC_MRSCODE                    ((volatile u32*)(IFX_SDRAM + 0x1008))
#define IFX_SDRAM_MC_MRSCODE_UMC(value)         (((( 1 << 5) - 1) & (value)) << 7)
#define IFX_SDRAM_MC_MRSCODE_CL(value)          (((( 1 << 3) - 1) & (value)) << 4)
#define IFX_SDRAM_MC_MRSCODE_WT                 (1 << 3)
#define IFX_SDRAM_MC_MRSCODE_BL(value)          (((( 1 << 3) - 1) & (value)) << 0)

/***MC Configuration Data-word Width Register***/
#define IFX_SDRAM_MC_CFGDW                      ((volatile u32*)(IFX_SDRAM + 0x1010))
#define IFX_SDRAM_MC_CFGDW_DW(value)            (((( 1 << 4) - 1) & (value)) << 0)
#define IFX_SDRAM_MC_CFGDW_Res(value)           (((( 1 << 28) - 1) & (value)) << 4)

/***MC Configuration Physical Bank 0 Register***/
#define IFX_SDRAM_MC_CFGPB0                     ((volatile u32*)(IFX_SDRAM + 0x1018))
#define IFX_SDRAM_MC_CFGPB0_MCSEN0(value)       (((( 1 << 4) - 1) & (value)) << 12)
#define IFX_SDRAM_MC_CFGPB0_BANKN0(value)       (((( 1 << 4) - 1) & (value)) << 8)
#define IFX_SDRAM_MC_CFGPB0_ROWW0(value)        (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_SDRAM_MC_CFGPB0_COLW0(value)        (((( 1 << 4) - 1) & (value)) << 0)
#define IFX_SDRAM_MC_CFGPB0_Res(value)          (((( 1 << 16) - 1) & (value)) << 16)

/***MC Latency Register***/
#define IFX_SDRAM_MC_LATENCY                    ((volatile u32*)(IFX_SDRAM + 0x1038))
#define IFX_SDRAM_MC_LATENCY_TRP(value)         (((( 1 << 4) - 1) & (value)) << 16)
#define IFX_SDRAM_MC_LATENCY_TRAS(value)        (((( 1 << 4) - 1) & (value)) << 12)
#define IFX_SDRAM_MC_LATENCY_TRCD(value)        (((( 1 << 4) - 1) & (value)) << 8)
#define IFX_SDRAM_MC_LATENCY_TDPL(value)        (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_SDRAM_MC_LATENCY_TDAL(value)        (((( 1 << 4) - 1) & (value)) << 0)
#define IFX_SDRAM_MC_LATENCY_Res(value)         (((( 1 << 12) - 1) & (value)) << 20)

/***MC Refresh Cycle Time Register***/
#define IFX_SDRAM_MC_TREFRESH                   ((volatile u32*)(IFX_SDRAM + 0x1040))
#define IFX_SDRAM_MC_TREFRESH_TREF(value)       (((( 1 << 13) - 1) & (value)) << 0)
#define IFX_SDRAM_MC_TREFRESH_Res(value)        (((( 1 << 19) - 1) & (value)) << 13)

/***MC Status Register***/
#define IFX_SDRAM_MC_STAT                   	((volatile u32*)(IFX_SDRAM + 0x0070))

/***MC DDR Control Register 00***/
#define IFX_DDR_MC_DC00                   	((volatile u32*)(IFX_SDRAM + 0x1000))
/***MC DDR Control Register 03***/
#define IFX_DDR_MC_DC03                         ((volatile u32*)(IFX_SDRAM + 0x1030))
/***MC DDR Control Register 17***/
#define IFX_DDR_MC_DC17                         ((volatile u32*)(IFX_SDRAM + 0x1110))


/***********************************************************************/
/*  Module      :  ASC0 register address and bits                      */
/***********************************************************************/

#define IFX_ASC0                                (KSEG1 | 0x1E100400)

#define IFX_ASC0_TBUF                           ((volatile u32*)(IFX_ASC0 + 0x0020))
#define IFX_ASC0_RBUF                           ((volatile u32*)(IFX_ASC0 + 0x0024))
#define IFX_ASC0_FSTAT                          ((volatile u32*)(IFX_ASC0 + 0x0048))



/***********************************************************************/
/*  Module      :  ASC1 register address and bits                      */
/***********************************************************************/

#define IFX_ASC1                                (KSEG1 | 0x1E100C00)

#define IFX_ASC1_TBUF                           ((volatile u32*)(IFX_ASC1 + 0x0020))
#define IFX_ASC1_RBUF                           ((volatile u32*)(IFX_ASC1 + 0x0024))
#define IFX_ASC1_FSTAT                          ((volatile u32*)(IFX_ASC1 + 0x0048))



/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/

#define IFX_DMA                                 (KSEG1 | 0x1E104100)

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



/***********************************************************************/
/*  Module      :  Debug register address and bits                     */
/***********************************************************************/

#define IFX_Debug                               (KSEG1 | 0x1F106000)

/***MCD Break Bus Switch Register***/
#define IFX_Debug_MCD_BBS                       ((volatile u32*)(IFX_Debug + 0x0000))
#define IFX_Debug_MCD_BBS_BTP1                  (1 << 19)
#define IFX_Debug_MCD_BBS_BTP0                  (1 << 18)
#define IFX_Debug_MCD_BBS_BSP1                  (1 << 17)
#define IFX_Debug_MCD_BBS_BSP0                  (1 << 16)
#define IFX_Debug_MCD_BBS_BT5EN                 (1 << 15)
#define IFX_Debug_MCD_BBS_BT4EN                 (1 << 14)
#define IFX_Debug_MCD_BBS_BT5                   (1 << 13)
#define IFX_Debug_MCD_BBS_BT4                   (1 << 12)
#define IFX_Debug_MCD_BBS_BS5EN                 (1 << 7)
#define IFX_Debug_MCD_BBS_BS4EN                 (1 << 6)
#define IFX_Debug_MCD_BBS_BS5                   (1 << 5)
#define IFX_Debug_MCD_BBS_BS4                   (1 << 4)

/***MCD Multiplexer Control Register***/
#define IFX_Debug_MCD_MCR                       ((volatile u32*)(IFX_Debug+ 0x0008))
#define IFX_Debug_MCD_MCR_MUX5                  (1 << 4)
#define IFX_Debug_MCD_MCR_MUX4                  (1 << 3)
#define IFX_Debug_MCD_MCR_MUX1                  (1 << 0)



/***********************************************************************/
/*  Module      :  ICU register address and bits                       */
/***********************************************************************/

#define IFX_ICU                                 (KSEG1 | 0x1F880200)

#define IFX_ICU_IM0_ISR                         ((volatile u32*)(IFX_ICU + 0x0000))
#define IFX_ICU_IM0_IER                         ((volatile u32*)(IFX_ICU + 0x0008))
#define IFX_ICU_IM0_IOSR                        ((volatile u32*)(IFX_ICU + 0x0010))
#define IFX_ICU_IM0_IRSR                        ((volatile u32*)(IFX_ICU + 0x0018))
#define IFX_ICU_IM0_IMR                         ((volatile u32*)(IFX_ICU + 0x0020))

#define IFX_ICU_IM1_ISR                         ((volatile u32*)(IFX_ICU + 0x0028))
#define IFX_ICU_IM1_IER                         ((volatile u32*)(IFX_ICU + 0x0030))
#define IFX_ICU_IM1_IOSR                        ((volatile u32*)(IFX_ICU + 0x0038))
#define IFX_ICU_IM1_IRSR                        ((volatile u32*)(IFX_ICU + 0x0040))
#define IFX_ICU_IM1_IMR                         ((volatile u32*)(IFX_ICU + 0x0048))

#define IFX_ICU_IM2_ISR                         ((volatile u32*)(IFX_ICU + 0x0050))
#define IFX_ICU_IM2_IER                         ((volatile u32*)(IFX_ICU + 0x0058))
#define IFX_ICU_IM2_IOSR                        ((volatile u32*)(IFX_ICU + 0x0060))
#define IFX_ICU_IM2_IRSR                        ((volatile u32*)(IFX_ICU + 0x0068))
#define IFX_ICU_IM2_IMR                         ((volatile u32*)(IFX_ICU + 0x0070))

#define IFX_ICU_IM3_ISR                         ((volatile u32*)(IFX_ICU + 0x0078))
#define IFX_ICU_IM3_IER                         ((volatile u32*)(IFX_ICU + 0x0080))
#define IFX_ICU_IM3_IOSR                        ((volatile u32*)(IFX_ICU + 0x0088))
#define IFX_ICU_IM3_IRSR                        ((volatile u32*)(IFX_ICU + 0x0090))
#define IFX_ICU_IM3_IMR                         ((volatile u32*)(IFX_ICU + 0x0098))

#define IFX_ICU_IM4_ISR                         ((volatile u32*)(IFX_ICU + 0x00A0))
#define IFX_ICU_IM4_IER                         ((volatile u32*)(IFX_ICU + 0x00A8))
#define IFX_ICU_IM4_IOSR                        ((volatile u32*)(IFX_ICU + 0x00B0))
#define IFX_ICU_IM4_IRSR                        ((volatile u32*)(IFX_ICU + 0x00B8))
#define IFX_ICU_IM4_IMR                         ((volatile u32*)(IFX_ICU + 0x00C0))

/***Interrupt Vector Value Register***/
#define IFX_ICU_IM_VEC_5                        ((volatile u32*)(IFX_ICU + 0x00C8))
#define IFX_ICU_IM_VEC                          ((volatile u32*)(IFX_ICU + 0x00D0))

/***********************************************************************/

#define IFX_ICU_VPE1                            (KSEG1 | 0x1F880300)
#define IFX_ICU1                                IFX_ICU_VPE1

#define IFX_ICU_VPE1_IM0_ISR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0000))
#define IFX_ICU_VPE1_IM0_IER                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0008))
#define IFX_ICU_VPE1_IM0_IOSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0010))
#define IFX_ICU_VPE1_IM0_IRSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0018))
#define IFX_ICU_VPE1_IM0_IMR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0020))

#define IFX_ICU_VPE1_IM1_ISR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0028))
#define IFX_ICU_VPE1_IM1_IER                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0030))
#define IFX_ICU_VPE1_IM1_IOSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0038))
#define IFX_ICU_VPE1_IM1_IRSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0040))
#define IFX_ICU_VPE1_IM1_IMR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0048))

#define IFX_ICU_VPE1_IM2_ISR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0050))
#define IFX_ICU_VPE1_IM2_IER                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0058))
#define IFX_ICU_VPE1_IM2_IOSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0060))
#define IFX_ICU_VPE1_IM2_IRSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0068))
#define IFX_ICU_VPE1_IM2_IMR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0070))

#define IFX_ICU_VPE1_IM3_ISR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0078))
#define IFX_ICU_VPE1_IM3_IER                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0080))
#define IFX_ICU_VPE1_IM3_IOSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0088))
#define IFX_ICU_VPE1_IM3_IRSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x0090))
#define IFX_ICU_VPE1_IM3_IMR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x0098))

#define IFX_ICU_VPE1_IM4_ISR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x00A0))
#define IFX_ICU_VPE1_IM4_IER                    ((volatile u32*)(IFX_ICU_VPE1 + 0x00A8))
#define IFX_ICU_VPE1_IM4_IOSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x00B0))
#define IFX_ICU_VPE1_IM4_IRSR                   ((volatile u32*)(IFX_ICU_VPE1 + 0x00B8))
#define IFX_ICU_VPE1_IM4_IMR                    ((volatile u32*)(IFX_ICU_VPE1 + 0x00C0))

/***Interrupt Vector Value Register***/
#define IFX_ICU_VPE1_IM_VEC_5                   ((volatile u32*)(IFX_ICU_VPE1 + 0x00C8))
#define IFX_ICU_VPE1_IM_VEC                     ((volatile u32*)(IFX_ICU_VPE1 + 0x00D0))

/***Interrupt Vector Value Mask***/
#define IFX_ICU_IM0_VEC_MASK                    (0x3F << 0)
#define IFX_ICU_IM1_VEC_MASK                    (0x3F << 6)
#define IFX_ICU_IM2_VEC_MASK                    (0x3F << 12)
#define IFX_ICU_IM3_VEC_MASK                    (0x3F << 18)
#define IFX_ICU_IM4_VEC_MASK                    (0x3F << 24)

#define IFX_ICU_IM0_ISR_IR(value)               (1 << (value))
#define IFX_ICU_IM0_IER_IR(value)               (1 << (value))
#define IFX_ICU_IM1_ISR_IR(value)               (1 << (value))
#define IFX_ICU_IM1_IER_IR(value)               (1 << (value))
#define IFX_ICU_IM2_ISR_IR(value)               (1 << (value))
#define IFX_ICU_IM2_IER_IR(value)               (1 << (value))
#define IFX_ICU_IM3_ISR_IR(value)               (1 << (value))
#define IFX_ICU_IM3_IER_IR(value)               (1 << (value))
#define IFX_ICU_IM4_ISR_IR(value)               (1 << (value))
#define IFX_ICU_IM4_IER_IR(value)               (1 << (value))
#define IFX_ICU_IM5_ISR_IR(value)               (1 << (value))
#define IFX_ICU_IM5_IER_IR(value)               (1 << (value))

/***External Interrupt Control Register***/
#define IFX_ICU_EIU                             (KSEG1 | 0x1F101000)
#define IFX_ICU_EIU_EXIN_C                      ((volatile u32 *)(IFX_ICU_EIU + 0x0000))
#define IFX_ICU_EIU_INIC                        ((volatile u32 *)(IFX_ICU_EIU + 0x0004))
#define IFX_ICU_EIU_INC                         ((volatile u32 *)(IFX_ICU_EIU + 0x0008))
#define IFX_ICU_EIU_INEN                        ((volatile u32 *)(IFX_ICU_EIU + 0x000C))
#define IFX_YIELDEN(n)                          ((volatile u32 *)(IFX_ICU_EIU + 0x0010 + (n) * 4)
#define IFX_NMI_CR                              ((volatile u32 *)(IFX_ICU_EIU + 0x00F0))
#define IFX_NMI_SR                              ((volatile u32 *)(IFX_ICU_EIU + 0x00F4))



/***********************************************************************/
/*  Module      :  MPS register address and bits                       */
/***********************************************************************/

#define IFX_MPS                                 (KSEG1 | 0x1F107000)

#define IFX_MPS_CHIPID                          ((volatile u32*)(IFX_MPS + 0x0344))
#define IFX_MPS_CHIPID_VERSION_GET(value)       (((value) >> 28) & ((1 << 4) - 1))
#define IFX_MPS_CHIPID_VERSION_SET(value)       (((( 1 << 4) - 1) & (value)) << 28)
#define IFX_MPS_CHIPID_PARTNUM_GET(value)       (((value) >> 12) & ((1 << 16) - 1))
#define IFX_MPS_CHIPID_PARTNUM_SET(value)       (((( 1 << 16) - 1) & (value)) << 12)
#define IFX_MPS_CHIPID_MANID_GET(value)         (((value) >> 1) & ((1 << 10) - 1))
#define IFX_MPS_CHIPID_MANID_SET(value)         (((( 1 << 10) - 1) & (value)) << 1)


/* voice channel 0 ... 3 interrupt enable register */
#define IFX_MPS_VC0ENR                          ((volatile u32*)(IFX_MPS + 0x0000))
#define IFX_MPS_VC1ENR                          ((volatile u32*)(IFX_MPS + 0x0004))
#define IFX_MPS_VC2ENR                          ((volatile u32*)(IFX_MPS + 0x0008))
#define IFX_MPS_VC3ENR                          ((volatile u32*)(IFX_MPS + 0x000C))
/* voice channel 0 ... 3 interrupt status read register */
#define IFX_MPS_RVC0SR                          ((volatile u32*)(IFX_MPS + 0x0010))
#define IFX_MPS_RVC1SR                          ((volatile u32*)(IFX_MPS + 0x0014))
#define IFX_MPS_RVC2SR                          ((volatile u32*)(IFX_MPS + 0x0018))
#define IFX_MPS_RVC3SR                          ((volatile u32*)(IFX_MPS + 0x001C))
/* voice channel 0 ... 3 interrupt status set register */
#define IFX_MPS_SVC0SR                          ((volatile u32*)(IFX_MPS + 0x0020))
#define IFX_MPS_SVC1SR                          ((volatile u32*)(IFX_MPS + 0x0024))
#define IFX_MPS_SVC2SR                          ((volatile u32*)(IFX_MPS + 0x0028))
#define IFX_MPS_SVC3SR                          ((volatile u32*)(IFX_MPS + 0x002C))
/* voice channel 0 ... 3 interrupt status clear register */
#define IFX_MPS_CVC0SR                          ((volatile u32*)(IFX_MPS + 0x0030))
#define IFX_MPS_CVC1SR                          ((volatile u32*)(IFX_MPS + 0x0034))
#define IFX_MPS_CVC2SR                          ((volatile u32*)(IFX_MPS + 0x0038))
#define IFX_MPS_CVC3SR                          ((volatile u32*)(IFX_MPS + 0x003C))
/* common status 0 and 1 read register */
#define IFX_MPS_RAD0SR                          ((volatile u32*)(IFX_MPS + 0x0040))
#define IFX_MPS_RAD1SR                          ((volatile u32*)(IFX_MPS + 0x0044))
/* common status 0 and 1 set register */
#define IFX_MPS_SAD0SR                          ((volatile u32*)(IFX_MPS + 0x0048))
#define IFX_MPS_SAD1SR                          ((volatile u32*)(IFX_MPS + 0x004C))
/* common status 0 and 1 clear register */
#define IFX_MPS_CAD0SR                          ((volatile u32*)(IFX_MPS + 0x0050))
#define IFX_MPS_CAD1SR                          ((volatile u32*)(IFX_MPS + 0x0054))
/* common status 0 and 1 enable register */
#define IFX_MPS_AD0ENR                          ((volatile u32*)(IFX_MPS + 0x0058))
#define IFX_MPS_AD1ENR                          ((volatile u32*)(IFX_MPS + 0x005C))
/* notification enable register */
#define IFX_MPS_CPU0_NFER                       ((volatile u32*)(IFX_MPS + 0x0060))
#define IFX_MPS_CPU1_NFER                       ((volatile u32*)(IFX_MPS + 0x0064))
/* CPU to CPU interrup request register */
#define IFX_MPS_CPU0_2_CPU1_IRR                 ((volatile u32*)(IFX_MPS + 0x0070))
#define IFX_MPS_CPU0_2_CPU1_IER                 ((volatile u32*)(IFX_MPS + 0x0074))
/* Global interrupt request and request enable register */
#define IFX_MPS_GIRR                            ((volatile u32*)(IFX_MPS + 0x0078))
#define IFX_MPS_GIER                            ((volatile u32*)(IFX_MPS + 0x007C))

#define IFX_MPS_SRAM                            ((volatile u32*)(KSEG1 | 0x1F200000))

#define IFX_MPS_VCPU_FW_AD                      ((volatile u32*)(KSEG1 | 0x1F2001E0))

#define IFX_FUSE_BASE_ADDR                      (KSEG1 | 0x1F107354)



/************************************************************************/
/*   Module       :   DEU register address and bits                     */
/************************************************************************/

#define IFX_DEU_BASE_ADDR                       (KSEG1 | 0x1E103100)

/*   DEU Control Register */
#define IFX_DEU_CLK                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0000))
#define IFX_DEU_ID                              ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0008))

/*   DEU control register */
#define IFX_DES_CON                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0010))
#define IFX_DES_IHR                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0014))
#define IFX_DES_ILR                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0018))
#define IFX_DES_K1HR                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x001C))
#define IFX_DES_K1LR                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0020))
#define IFX_DES_K3HR                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0024))
#define IFX_DES_K3LR                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0028))
#define IFX_DES_IVHR                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x002C))
#define IFX_DES_IVLR                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0030))
#define IFX_DES_OHR                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0040))
#define IFX_DES_OLR                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0050))

/* AES DEU register */
#define IFX_AES_CON                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0050))
#define IFX_AES_ID3R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0054))
#define IFX_AES_ID2R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0058))
#define IFX_AES_ID1R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x005C))
#define IFX_AES_ID0R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0060))

/* AES Key register */
#define IFX_AES_K7R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0064))
#define IFX_AES_K6R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0068))
#define IFX_AES_K5R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x006C))
#define IFX_AES_K4R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0070))
#define IFX_AES_K3R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0074))
#define IFX_AES_K2R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0078))
#define IFX_AES_K1R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x007C))
#define IFX_AES_K0R                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0080))

/* AES vector register */
#define IFX_AES_IV3R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0084))
#define IFX_AES_IV2R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0088))
#define IFX_AES_IV1R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x008C))
#define IFX_AES_IV0R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0090))
#define IFX_AES_0D3R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0094))
#define IFX_AES_0D2R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0098))
#define IFX_AES_OD1R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x009C))
#define IFX_AES_OD0R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00A0))

/* ARC4 DEU register */
#define IFX_ARC4_CON                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0100))
#define IFX_ARC4_IDLEN                          ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0114))
#define IFX_ARC4_ID3R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0118))
#define IFX_ARC4_ID2R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x011C))
#define IFX_ARC4_ID1R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0120))
#define IFX_ARC4_ID0R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0124))

/* ARC4 Key register */
#define IFX_ARC4_K3R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0104))
#define IFX_ARC4_K2R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0108))
#define IFX_ARC4_K1R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x010C))
#define IFX_ARC4_K0R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0110))

/* ARC4 vector register */
#define IFX_ARC4_OD3R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0128))
#define IFX_ARC4_OD2R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x012C))
#define IFX_ARC4_OD1R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0130))
#define IFX_ARC4_OD0R                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x0134))

/* hash control register */
#define IFX_HASH_CON                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B0))
#define IFX_HASH_MR                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B4))
#define IFX_HASH_D1R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B8))
#define IFX_HASH_D2R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00BC))
#define IFX_HASH_D3R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00C0))
#define IFX_HASH_D4R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00C4))
#define IFX_HASH_D5R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00C8))

#define IFX_HMAC_KIDX                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00D0))
#define IFX_HMAC_KEY                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00D4))
#define IFX_HMAC_DBN                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00D8))

#define IFX_DEU_DMA_CON                         ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00EC))

#define IFX_DEU_IRNEN                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00F4))
#define IFX_DEU_IRNCR                           ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00F8))
#define IFX_DEU_IRNICR                          ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00FC))



/************************************************************************/
/*   Module       :   PPE register address and bits                     */
/************************************************************************/

#define IFX_PPE32_BASE                          (KSEG1 | 0x1E180000)
#define IFX_PPE32_DEBUG_BREAK_TRACE_REG         (IFX_PPE32_BASE + (0x0000 * 4))
#define IFX_PPE32_INT_MASK_STATUS_REG           (IFX_PPE32_BASE + (0x0030 * 4))
#define IFX_PPE32_INT_RESOURCE_REG              (IFX_PPE32_BASE + (0x0040 * 4))
#define IFX_PPE32_CDM_CODE_MEM_B0               (IFX_PPE32_BASE + (0x1000 * 4))
#define IFX_PPE32_CDM_CODE_MEM_B1               (IFX_PPE32_BASE + (0x2000 * 4))
#define IFX_PPE32_DATA_MEM_MAP_REG_BASE         (IFX_PPE32_BASE + (0x4000 * 4))

#define IFX_PPE32_SRST                          (IFX_PPE32_BASE + 0x10080)

/*
 *    ETOP MDIO Registers
 */
#define IFX_PP32_ETOP_MDIO_CFG                  ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0600 * 4)))
#define IFX_PP32_ETOP_MDIO_ACC                  ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0601 * 4)))
#define IFX_PP32_ETOP_CFG                       ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0602 * 4)))
#define IFX_PP32_ETOP_IG_VLAN_COS               ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0603 * 4)))
#define IFX_PP32_ETOP_IG_DSCP_COS3              ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0604 * 4)))
#define IFX_PP32_ETOP_IG_DSCP_COS2              ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0605 * 4)))
#define IFX_PP32_ETOP_IG_DSCP_COS1              ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0606 * 4)))
#define IFX_PP32_ETOP_IG_DSCP_COS0              ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0607 * 4)))
#define IFX_PP32_ETOP_IG_PLEN_CTRL              ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0608 * 4)))
#define IFX_PP32_ETOP_ISR                       ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x060A * 4)))
#define IFX_PP32_ETOP_IER                       ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x060B * 4)))
#define IFX_PP32_ETOP_VPID                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x060C * 4)))
#define IFX_PP32_ENET_MAC_CFG                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0610 * 4)))
#define IFX_PP32_ENETS_DBA                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0612 * 4)))
#define IFX_PP32_ENETS_CBA                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0613 * 4)))
#define IFX_PP32_ENETS_CFG                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0614 * 4)))
#define IFX_PP32_ENETS_PGCNT                    ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0615 * 4)))
#define IFX_PP32_ENETS_PKTCNT                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0616 * 4)))
#define IFX_PP32_ENETS_BUF_CTRL                 ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0617 * 4)))
#define IFX_PP32_ENETS_COS_CFG                  ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0618 * 4)))
#define IFX_PP32_ENETS_IGDROP                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0619 * 4)))
#define IFX_PP32_ENETS_IGERR                    ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x061A * 4)))
#define IFX_PP32_ENET_MAC_DA0                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x061B * 4)))
#define IFX_PP32_ENET_MAC_DA1                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x061C * 4)))

#define IFX_PP32_ENETF_DBA                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0630 * 4)))
#define IFX_PP32_ENETF_CBA                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0631 * 4)))
#define IFX_PP32_ENETF_CFG                      ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0632 * 4)))
#define IFX_PP32_ENETF_PGCNT                    ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0633 * 4)))
#define IFX_PP32_ENETF_PKTCNT                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0634 * 4)))
#define IFX_PP32_ENETF_HFCTRL                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0635 * 4)))
#define IFX_PP32_ENETF_TXCTRL                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0636 * 4)))

#define IFX_PP32_ENETF_VLCOS0                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0638 * 4)))
#define IFX_PP32_ENETF_VLCOS1                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x0639 * 4)))
#define IFX_PP32_ENETF_VLCOS2                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x063A * 4)))
#define IFX_PP32_ENETF_VLCOS3                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x063B * 4)))
#define IFX_PP32_ENETF_EGERR                    ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x063C * 4)))
#define IFX_PP32_ENETF_EGDROP                   ((volatile u32 *)(IFX_PPE32_DATA_MEM_MAP_REG_BASE + (0x063D * 4)))


/* Sharebuff SB RAM2 control data */
#define IFX_PP32_SB2_DATABASE                   ((IFX_PPE32_BASE + (0x8C00 * 4)))
#define IFX_PP32_SB2_CTRLBASE                   ((IFX_PPE32_BASE + (0x92E0 * 4)))



/************************************************************************/
/*   Module       :   3-port Switch register address and bits           */
/************************************************************************/

#define IFX_SW                                  (KSEG1 | 0x1E108000)

#define IFX_SW_PS                               (IFX_SW + 0x000)
#define IFX_SW_P0_CTL                           (IFX_SW + 0x004)
#define IFX_SW_P1_CTL                           (IFX_SW + 0x008)
#define IFX_SW_P2_CTL                           (IFX_SW + 0x00C)
#define IFX_SW_P0_VLAN                          (IFX_SW + 0x010)
#define IFX_SW_P1_VLAN                          (IFX_SW + 0x014)
#define IFX_SW_P2_VLAN                          (IFX_SW + 0x018)
#define IFX_SW_P0_INCTL                         (IFX_SW + 0x020)
#define IFX_SW_P1_INCTL                         (IFX_SW + 0x024)
#define IFX_SW_P2_INCTL                         (IFX_SW + 0x028)
#define IFX_SW_DF_PORTMAP                       (IFX_SW + 0x02C)
#define IFX_SW_P0_ECS_Q32                       (IFX_SW + 0x030)
#define IFX_SW_P0_ECS_Q10                       (IFX_SW + 0x034)
#define IFX_SW_P0_ECW_Q32                       (IFX_SW + 0x038)
#define IFX_SW_P0_ECW_Q10                       (IFX_SW + 0x03C)
#define IFX_SW_P1_ECS_Q32                       (IFX_SW + 0x040)
#define IFX_SW_P1_ECS_Q10                       (IFX_SW + 0x044)
#define IFX_SW_P1_ECW_Q32                       (IFX_SW + 0x048)
#define IFX_SW_P1_ECW_Q10                       (IFX_SW + 0x04C)
#define IFX_SW_P2_ECS_Q32                       (IFX_SW + 0x050)
#define IFX_SW_P2_ECS_Q10                       (IFX_SW + 0x054)
#define IFX_SW_P2_ECW_Q32                       (IFX_SW + 0x058)
#define IFX_SW_P2_ECW_Q10                       (IFX_SW + 0x05C)
#define IFX_SW_INT_ENA                          (IFX_SW + 0x060)
#define IFX_SW_INT_ST                           (IFX_SW + 0x064)
#define IFX_SW_GCTL0                            (IFX_SW + 0x068)
#define IFX_SW_GCTL1                            (IFX_SW + 0x06C)
#define IFX_SW_ARP                              (IFX_SW + 0x070)
#define IFX_SW_STRM_CTL                         (IFX_SW + 0x074)
#define IFX_SW_RGMII_CTL                        (IFX_SW + 0x078)
#define IFX_SW_1P_PRT                           (IFX_SW + 0x07C)
#define IFX_SW_GBKT_SZBS                        (IFX_SW + 0x080)
#define IFX_SW_GBKT_SZEBS                       (IFX_SW + 0x084)
#define IFX_SW_BF_TH                            (IFX_SW + 0x088)
#define IFX_SW_PMAC_HD_CTL                      (IFX_SW + 0x08C)
#define IFX_SW_PMAC_SA1                         (IFX_SW + 0x090)
#define IFX_SW_PMAC_SA2                         (IFX_SW + 0x094)
#define IFX_SW_PMAC_DA1                         (IFX_SW + 0x098)
#define IFX_SW_PMAC_DA2                         (IFX_SW + 0x09C)
#define IFX_SW_PMAC_VLAN                        (IFX_SW + 0x0A0)
#define IFX_SW_PMAC_TX_IPG                      (IFX_SW + 0x0A4)
#define IFX_SW_PMAC_RX_IPG                      (IFX_SW + 0x0A8)
#define IFX_SW_ADR_TB_CTL0                      (IFX_SW + 0x0AC)
#define IFX_SW_ADR_TB_CTL1                      (IFX_SW + 0x0B0)
#define IFX_SW_ADR_TB_CTL2                      (IFX_SW + 0x0B4)
#define IFX_SW_ADR_TB_ST0                       (IFX_SW + 0x0B8)
#define IFX_SW_ADR_TB_ST1                       (IFX_SW + 0x0BC)
#define IFX_SW_ADR_TB_ST2                       (IFX_SW + 0x0C0)
#define IFX_SW_RMON_CTL                         (IFX_SW + 0x0C4)
#define IFX_SW_RMON_ST                          (IFX_SW + 0x0C8)
#define IFX_SW_MDIO_CTL                         (IFX_SW + 0x0CC)
#define IFX_SW_MDIO_DATA                        (IFX_SW + 0x0D0)
#define IFX_SW_TP_FLT_ACT                       (IFX_SW + 0x0D4)
#define IFX_SW_PRTCL_FLT_ACT                    (IFX_SW + 0x0D8)
#define IFX_SW_VLAN_FLT0                        (IFX_SW + 0x100)
#define IFX_SW_VLAN_FLT1                        (IFX_SW + 0x104)
#define IFX_SW_VLAN_FLT2                        (IFX_SW + 0x108)
#define IFX_SW_VLAN_FLT3                        (IFX_SW + 0x10C)
#define IFX_SW_VLAN_FLT4                        (IFX_SW + 0x110)
#define IFX_SW_VLAN_FLT5                        (IFX_SW + 0x114)
#define IFX_SW_VLAN_FLT6                        (IFX_SW + 0x118)
#define IFX_SW_VLAN_FLT7                        (IFX_SW + 0x11C)
#define IFX_SW_VLAN_FLT8                        (IFX_SW + 0x120)
#define IFX_SW_VLAN_FLT9                        (IFX_SW + 0x124)
#define IFX_SW_VLAN_FLT10                       (IFX_SW + 0x128)
#define IFX_SW_VLAN_FLT11                       (IFX_SW + 0x12C)
#define IFX_SW_VLAN_FLT12                       (IFX_SW + 0x130)
#define IFX_SW_VLAN_FLT13                       (IFX_SW + 0x134)
#define IFX_SW_VLAN_FLT14                       (IFX_SW + 0x138)
#define IFX_SW_VLAN_FLT15                       (IFX_SW + 0x13C)
#define IFX_SW_TP_FLT10                         (IFX_SW + 0x140)
#define IFX_SW_TP_FLT32                         (IFX_SW + 0x144)
#define IFX_SW_TP_FLT54                         (IFX_SW + 0x148)
#define IFX_SW_TP_FLT76                         (IFX_SW + 0x14C)
#define IFX_SW_DFSRV_MAP0                       (IFX_SW + 0x150)
#define IFX_SW_DFSRV_MAP1                       (IFX_SW + 0x154)
#define IFX_SW_DFSRV_MAP2                       (IFX_SW + 0x158)
#define IFX_SW_DFSRV_MAP3                       (IFX_SW + 0x15C)
#define IFX_SW_TCP_PF0                          (IFX_SW + 0x160)
#define IFX_SW_TCP_PF1                          (IFX_SW + 0x164)
#define IFX_SW_TCP_PF2                          (IFX_SW + 0x168)
#define IFX_SW_TCP_PF3                          (IFX_SW + 0x16C)
#define IFX_SW_TCP_PF4                          (IFX_SW + 0x170)
#define IFX_SW_TCP_PF5                          (IFX_SW + 0x174)
#define IFX_SW_TCP_PF6                          (IFX_SW + 0x178)
#define IFX_SW_TCP_PF7                          (IFX_SW + 0x17C)
#define IFX_SW_RA_03_00                         (IFX_SW + 0x180)
#define IFX_SW_RA_07_04                         (IFX_SW + 0x184)
#define IFX_SW_RA_0B_08                         (IFX_SW + 0x188)
#define IFX_SW_RA_0F_0C                         (IFX_SW + 0x18C)
#define IFX_SW_RA_13_10                         (IFX_SW + 0x190)
#define IFX_SW_RA_17_14                         (IFX_SW + 0x194)
#define IFX_SW_RA_1B_18                         (IFX_SW + 0x198)
#define IFX_SW_RA_1F_1C                         (IFX_SW + 0x19C)
#define IFX_SW_RA_23_20                         (IFX_SW + 0x1A0)
#define IFX_SW_RA_27_24                         (IFX_SW + 0x1A4)
#define IFX_SW_RA_2B_28                         (IFX_SW + 0x1A8)
#define IFX_SW_RA_2F_2C                         (IFX_SW + 0x1AC)
#define IFX_SW_F0                               (IFX_SW + 0x1B0)
#define IFX_SW_F1                               (IFX_SW + 0x1B4)

/*
 *  Routine for Voice
 */
extern const void (*ifx_bsp_basic_mps_decrypt)(unsigned int addr, int n);


#endif /* AR9_H */

