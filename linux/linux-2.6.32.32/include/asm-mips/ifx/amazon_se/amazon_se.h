/******************************************************************************
**
** FILE NAME    : amazon_se.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : header file for Amazon-SE
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



#ifndef AMAZON_SE_H
#define AMAZON_SE_H
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
#define MACH_GROUP_IFX MACH_GROUP_AMAZON_SE
#define MACH_TYPE_IFX  MACH_AMAZON_SE


#define IFX_MC_BASE                             (KSEG1 | 0x1F800000)
#define IFX_MC_ERRCAUSE                         ((volatile u32*)(IFX_MC_BASE + 0x10))
#define IFX_MC_ERRADD                           ((volatile u32*)(IFX_MC_BASE + 0x20))
#define IFX_MC_PRIO                             ((volatile u32*)(IFX_MC_BASE + 0x50))
#define IFX_MC_CON                              ((volatile u32*)(IFX_MC_BASE + 0x60))
#define IFX_MC_SRAM0                            ((volatile u32*)(IFX_MC_BASE + 0x100))

/* This defers from Amazon_SE Hardware Users Manual Revision 1.0.pdf which is wrong */
#define IFX_MC_PRIO_AHB_SHIFT                   6
#define IFX_MC_PRIO_FPI_SHIFT                   8
#define IFX_MC_PRIO_DMA_SHIFT                   2
#define IFX_MC_PRIO_2NDPRIOPORT_SHIFT           4
#define IFX_MC_PRIO_CPU0_SHIFT                  0

#define IFX_MC_PRIO_0                           0
#define IFX_MC_PRIO_1                           1
#define IFX_MC_PRIO_2                           2
#define IFX_MC_PRIO_3                           3



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

#define IFX_RST_ALL                             (1 << 30)
#define IFX_RCU_ARC_DFE_RESET                   (1 << 7)
#define IFX_RCU_AHB_RESET                       (1 << 6)
#define IFX_RCU_PPE_RESET                       (1 << 8)

/***Reset Request Register***/
#define IFX_RCU_RST_REQ_CPU0                    (1 << 31)
#define IFX_RCU_RST_REQ_CPU1                    (1 << 3)
#define IFX_RCU_RST_REQ_CPUSUB                  (1 << 29)
#define IFX_RCU_RST_REQ_HRST                    (1 << 28)
#define IFX_RCU_RST_REQ_WDT0                    (1 << 27)
#define IFX_RCU_RST_REQ_WDT1                    (1 << 26)
#define IFX_RCU_RST_REQ_CFG_GET(value)          (((value) >> 23) & ((1 << 3) - 1))
#define IFX_RCU_RST_REQ_CFG_SET(value)          (((( 1 << 3) - 1) & (value)) << 23)
#define IFX_RCU_RST_REQ_MUX_ARC                 (1 << 23)
#define IFX_RCU_RST_REQ_SWTBOOT                 (1 << 22)
#define IFX_RCU_RST_REQ_DMA                     (1 << 21)
#define IFX_RCU_RST_REQ_ARC_JTAG                (1 << 29)
#define IFX_RCU_RST_REQ_ETHPHY0                 (1 << 19)
#define IFX_RCU_RST_REQ_CPU0_BR                 (1 << 18)

#define IFX_RCU_RST_REQ_AFE                     (1 << 11)
#define IFX_RCU_RST_REQ_DFE                     (1 << 7)

/* CPU0, CPU1, CPUSUB, HRST, WDT0, WDT1, DMA, ETHPHY1, ETHPHY0 */
#define IFX_RCU_RST_REQ_ALL                     IFX_RST_ALL



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

/*All mei related registers are defined in amazon_se_mei_bsp.h */


/***********************************************************************/
/*  Module      :  GPIO register address and bits                       */
/***********************************************************************/

#define IFX_GPIO                                (KSEG1 | 0x1E100B00)

#define IFX_GPIO_Pn_BASE(n)                     (IFX_GPIO + 0x0010 + 0x0030 * (n))

/***Port 0 Data Output Register (0010H)***/
#define IFX_GPIO_P0_OUT                         ((volatile u32 *)(IFX_GPIO + 0x0010))
/***Port 1 Data Output Register (0040H)***/
#define IFX_GPIO_P1_OUT                         ((volatile u32 *)(IFX_GPIO + 0x0040))
/***Port 0 Data Input Register (0014H)***/
#define IFX_GPIO_P0_IN                          ((volatile u32 *)(IFX_GPIO + 0x0014))
/***Port 1 Data Input Register (0044H)***/
#define IFX_GPIO_P1_IN                          ((volatile u32 *)(IFX_GPIO + 0x0044))
/***Port 0 Direction Register (0018H)***/
#define IFX_GPIO_P0_DIR                         ((volatile u32 *)(IFX_GPIO + 0x0018))
/***Port 1 Direction Register (0048H)***/
#define IFX_GPIO_P1_DIR                         ((volatile u32 *)(IFX_GPIO + 0x0048))
/***Port 0 Alternate Function Select Register 0 (001C H) ***/
#define IFX_GPIO_P0_ALTSEL0                     ((volatile u32 *)(IFX_GPIO + 0x001C))
/***Port 1 Alternate Function Select Register 0 (004C H) ***/
#define IFX_GPIO_P1_ALTSEL0                     ((volatile u32 *)(IFX_GPIO + 0x004C))
/***Port 0 Alternate Function Select Register 1 (0020 H) ***/
#define IFX_GPIO_P0_ALTSEL1                     ((volatile u32 *)(IFX_GPIO + 0x0020))
/***Port 1 Alternate Function Select Register 0 (0050 H) ***/
#define IFX_GPIO_P1_ALTSEL1                     ((volatile u32 *)(IFX_GPIO + 0x0050))
/***Port 0 Open Drain Control Register (0024H)***/
#define IFX_GPIO_P0_OD                          ((volatile u32 *)(IFX_GPIO + 0x0024))
/***Port 1 Open Drain Control Register (0054H)***/
#define IFX_GPIO_P1_OD                          ((volatile u32 *)(IFX_GPIO + 0x0054))
/***Port 0 Input Schmitt-Trigger Off Register (0028 H) ***/
#define IFX_GPIO_P0_STOFF                       ((volatile u32 *)(IFX_GPIO + 0x0028))
/***Port 1 Input Schmitt-Trigger Off Register (0058 H) ***/
#define IFX_GPIO_P1_STOFF                       ((volatile u32 *)(IFX_GPIO + 0x0058))
/***Port 0 Pull Up/Pull Down Select Register (002C H)***/
#define IFX_GPIO_P0_PUDSEL                      ((volatile u32 *)(IFX_GPIO + 0x002C))
/***Port 1 Pull Up/Pull Down Select Register (005C H)***/
#define IFX_GPIO_P1_PUDSEL                      ((volatile u32 *)(IFX_GPIO + 0x005C))
/***Port 0 Pull Up Device Enable Register (0030 H)***/
#define IFX_GPIO_P0_PUDEN                       ((volatile u32 *)(IFX_GPIO + 0x0030))
/***Port 1 Pull Up Device Enable Register (0060 H)***/
#define IFX_GPIO_P1_PUDEN                       ((volatile u32 *)(IFX_GPIO + 0x0060))



/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/

#define IFX_CGU                                 (KSEG1 | 0x1F103000)

/***CGU Clock PLL0 ***/
#define IFX_CGU_PLL0_CFG                        ((volatile u32*)(IFX_CGU + 0x0004))
/***CGU Clock PLL1 ***/
#define IFX_CGU_PLL1_CFG                        ((volatile u32*)(IFX_CGU + 0x0008))
/***CGU Clock PLL2 ***/
#define IFX_CGU_PLL2_CFG                        ((volatile u32*)(IFX_CGU + 0x000C))
/***CGU Clock SYS Mux Register***/
#define IFX_CGU_SYS                             ((volatile u32*)(IFX_CGU + 0x0010))
/***CGU Interface Clock Control Register***/
#define IFX_CGU_IF_CLK                          ((volatile u32*)(IFX_CGU + 0x0018))
/***CGU Osc Control ***/
#define IFX_CGU_OSC_CON                         ((volatile u32*)(IFX_CGU + 0x001c))

#define IFX_CGU_PCI_CR                          ((volatile u32*)(IFX_CGU + 0x0034))



/***********************************************************************/
/*  Module      :  MCD register address and bits                       */
/***********************************************************************/
#define IFX_MCD                                 (KSEG1 | 0x1F106000)

/***Manufacturer Identification Register***/
#define IFX_MCD_MANID                           ((volatile u32*)(IFX_MCD+ 0x0024))
#define IFX_MCD_MANID_MANUF(value)              (((( 1 << 11) - 1) & (value)) << 5)

/***Chip Identification Register***/
#define IFX_MCD_CHIPID                          ((volatile u32*)(IFX_MCD+ 0x0028))
#define IFX_MCD_CHIPID_VERSION_GET(value)       (((value) >> 28) & ((1 << 4) - 1))
#define IFX_MCD_CHIPID_VERSION_SET(value)       (((( 1 << 4) - 1) & (value)) << 28)
#define IFX_MCD_CHIPID_PART_NUMBER_GET(value)   (((value) >> 12) & ((1 << 16) - 1))
#define IFX_MCD_CHIPID_PART_NUMBER_SET(value)   (((( 1 << 16) - 1) & (value)) << 12)
#define IFX_MCD_CHIPID_MANID_GET(value)         (((value) >> 1) & ((1 << 11) - 1))
#define IFX_MCD_CHIPID_MANID_SET(value)         (((( 1 << 11) - 1) & (value)) << 1)

#define IFX_CHIPID_STANDARD                     0x00EB
#define IFX_CHIPID_YANGTSE                      0x00ED

/***Redesign Tracing Identification Register***/
#define IFX_MCD_RTID                            ((volatile u32*)(IFX_MCD+ 0x002C))
#define IFX_MCD_RTID_LC                         (1 << 15)
#define IFX_MCD_RTID_RIX(value)                 (((( 1 << 3) - 1) & (value)) << 0)

#define IFX_FUSE_BASE_ADDR                      (KSEG1 | 0x1F107354)


/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/

#define IFX_EBU                                 (KSEG1 | 0x1E105300)

/***EBU Clock Control Register***/
#define IFX_EBU_CLC                             ((volatile u32*)(IFX_EBU + 0x0000))
#define IFX_EBU_CLC_DISS                        (1 << 1)
#define IFX_EBU_CLC_DISR                        (1 << 0)

/***EBU Global Control Register***/
#define IFX_EBU_CON                             ((volatile u32 *)(IFX_EBU + 0x0010))
#define IFX_EBU_CON0                            ((volatile u32 *)(IFX_EBU + 0x0060))
#define IFX_EBU_CON1                            ((volatile u32 *)(IFX_EBU + 0x0064))

#define IFX_EBU_CON_EBU_TA_MASK                 7
#define IFX_EBU_CON_EBU_TA_SHIFT                29
#define IFX_EBU_CON_DTACS(value)                (((( 1 << 3) - 1) & (value)) << 20)
#define IFX_EBU_CON_DTARW(value)                (((( 1 << 3) - 1) & (value)) << 16)
#define IFX_EBU_CON_TOUTC(value)                (((( 1 << 8) - 1) & (value)) << 8)
#define IFX_EBU_CON_ARBMODE(value)              (((( 1 << 2) - 1) & (value)) << 6)
#define IFX_EBU_CON_ARBSYNC                     (1 << 5)
#define IFX_EBU_CON_1                           (1 << 3)

/* IFX_EBU_CON0 */
#define IFX_EBU_CON0_WAITWRC_MASK               0xF00
#define IFX_EBU_CON0_WAITWRC_SHIFT              8
#define IFX_EBU_CON0_WAITRDC_MASK               0xC0
#define IFX_EBU_CON0_WAITRDC_SHIFT              6
#define IFX_EBU_CON0_RECOVC_MASK                0xC
#define IFX_EBU_CON0_CMULT_MASK                 3
#define IFX_EBU_CON0_CMULT_SHIFT                0
#define IFX_EBU_CON0_WRDIS                      (1 << 31)

/***EBU Address Select Register 0***/
#define IFX_EBU_ADDSEL0                         ((volatile u32*)(IFX_EBU + 0x0020))
#define IFX_EBU_ADDSEL0_BASE(value)             (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL0_MASK(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL0_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL0_REGEN                   (1 << 0)

/***EBU Address Select Register 1***/
#define IFX_EBU_ADDSEL1                         ((volatile u32*)(IFX_EBU + 0x0024))
#define IFX_EBU_ADDSEL1_BASE(value)            (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL1_MASK(value)            (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL1_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL1_REGEN                   (1 << 0)

/***EBU Address Select Register 2***/
#define IFX_EBU_ADDSEL2                         ((volatile u32*)(IFX_EBU + 0x0028))
#define IFX_EBU_ADDSEL2_BASE(value)             (((( 1 << 20) - 1) & (value)) << 12)
#define IFX_EBU_ADDSEL2_MASK(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define IFX_EBU_ADDSEL2_MIRRORE                 (1 << 1)
#define IFX_EBU_ADDSEL2_REGEN                   (1 << 0)

/***EBU Address Select Register 3***/
#define IFX_EBU_ADDSEL3                         ((volatile u32*)(IFX_EBU + 0x0028))
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
#define IFX_EBU_BUSCON1_WRDIS                   (1 << 31)
//#define IFX_EBU_BUSCON1_ALEC(value)            (((( 1 << 2) - 1) & (value)) << 29)
//#define IFX_EBU_BUSCON1_BCGEN (value)           (((( 1 << 2) - 1) & (value)) << 27)
//#define IFX_EBU_BUSCON1_AGEN (value)            (((( 1 << 2) - 1) & (value)) << 24)
//#define IFX_EBU_BUSCON1_CMULTR (value)          (((( 1 << 2) - 1) & (value)) << 22)
//#define IFX_EBU_BUSCON1_WAIT (value)            (((( 1 << 2) - 1) & (value)) << 20)
//#define IFX_EBU_BUSCON1_WAITINV                 (1 << 19)
//#define IFX_EBU_BUSCON1_SETUP                   (1 << 18)
//#define IFX_EBU_BUSCON1_PORTW (value)           (((( 1 << 2) - 1) & (value)) << 16)
//#define IFX_EBU_BUSCON1_WAITRDC (value)         (((( 1 << 7) - 1) & (value)) << 9)
//#define IFX_EBU_BUSCON1_WAITWRC (value)         (((( 1 << 3) - 1) & (value)) << 6)
//#define IFX_EBU_BUSCON1_HOLDC (value)           (((( 1 << 2) - 1) & (value)) << 4)
//#define IFX_EBU_BUSCON1_RECOVC (value)          (((( 1 << 2) - 1) & (value)) << 2)
//#define IFX_EBU_BUSCON1_CMULT (value)           (((( 1 << 2) - 1) & (value)) << 0)
#define IFX_EBU_BUSCON1_SETUP                   (1 << 22)
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

#define IFX_EBU_PCC_CON                         ((volatile u32*)(IFX_EBU + 0x0090))
#define IFX_EBU_PCC_STAT                        ((volatile u32*)(IFX_EBU + 0x0094))
#define IFX_EBU_PCC_ISTAT                       ((volatile u32*)(IFX_EBU + 0x00A0))
#define IFX_EBU_PCC_IEN                         ((volatile u32*)(IFX_EBU + 0x00A4))


/* NAND Flash Controller control Register */
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



/* NAND Flash Device RD/BY State Register */
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

/***********************************************************************/
/*  Module      :  ASC1 register address and bits                      */
/***********************************************************************/

#define IFX_ASC1                                (KSEG1 | 0x1E100C00)

/***ASC Clock Control Register***/
#define IFX_ASC1_CLC                            ((volatile u32*)(IFX_ASC1 + 0x0000))
#define IFX_ASC1_CLC_RMC(value)                 (((( 1 << 8) - 1) & (value)) << 8)
#define IFX_ASC1_CLC_DISS                       (1 << 1)
#define IFX_ASC1_CLC_DISR                       (1 << 0)

/***ASC Port Input Select Register***/
#define IFX_ASC1_PISEL                          ((volatile u32*)(IFX_ASC1 + 0x0004))
#define IFX_ASC1_PISEL                          ((volatile u32*)(IFX_ASC1 + 0x0004))
#define IFX_ASC1_PISEL_RIS                      (1 << 0)

/***ASC Control Register***/
#define IFX_ASC1_CON                            ((volatile u32*)(IFX_ASC1 + 0x0010))
#define IFX_ASC1_CON_BEN                        (1 << 20)
#define IFX_ASC1_CON_TOEN                       (1 << 20)
#define IFX_ASC1_CON_ROEN                       (1 << 19)
#define IFX_ASC1_CON_RUEN                       (1 << 18)
#define IFX_ASC1_CON_FEN                        (1 << 17)
#define IFX_ASC1_CON_PAL                        (1 << 16)
#define IFX_ASC1_CON_R                          (1 << 15)
#define IFX_ASC1_CON_ACO                        (1 << 14)
#define IFX_ASC1_CON_LB                         (1 << 13)
#define IFX_ASC1_CON_ERCLK                      (1 << 10)
#define IFX_ASC1_CON_FDE                        (1 << 9)
#define IFX_ASC1_CON_BRS                        (1 << 8)
#define IFX_ASC1_CON_STP                        (1 << 7)
#define IFX_ASC1_CON_SP                         (1 << 6)
#define IFX_ASC1_CON_ODD                        (1 << 5)
#define IFX_ASC1_CON_PEN                        (1 << 4)
#define IFX_ASC1_CON_M(value)                   (((( 1 << 3) - 1) & (value)) << 0)

/***ASC Staus Register***/
#define IFX_ASC1_STATE                          ((volatile u32*)(IFX_ASC1 + 0x0014))
/***ASC Write Hardware Modified Control Register***/
#define IFX_ASC1_WHBSTATE                       ((volatile u32*)(IFX_ASC1 + 0x0018))
#define IFX_ASC1_WHBSTATE_SETBE                 (1 << 113)
#define IFX_ASC1_WHBSTATE_SETTOE                (1 << 12)
#define IFX_ASC1_WHBSTATE_SETROE                (1 << 11)
#define IFX_ASC1_WHBSTATE_SETRUE                (1 << 10)
#define IFX_ASC1_WHBSTATE_SETFE                 (1 << 19)
#define IFX_ASC1_WHBSTATE_SETPE                 (1 << 18)
#define IFX_ASC1_WHBSTATE_CLRBE                 (1 << 17)
#define IFX_ASC1_WHBSTATE_CLRTOE                (1 << 6)
#define IFX_ASC1_WHBSTATE_CLRROE                (1 << 5)
#define IFX_ASC1_WHBSTATE_CLRRUE                (1 << 4)
#define IFX_ASC1_WHBSTATE_CLRFE                 (1 << 3)
#define IFX_ASC1_WHBSTATE_CLRPE                 (1 << 2)
#define IFX_ASC1_WHBSTATE_SETREN                (1 << 1)
#define IFX_ASC1_WHBSTATE_CLRREN                (1 << 0)

/***ASC Baudrate Timer/Reload Register***/
#define IFX_ASC1_BG                             ((volatile u32*)(IFX_ASC1 + 0x0050))
#define IFX_ASC1_BG_BR_VALUE(value)             (((( 1 << 13) - 1) & (value)) << 0)

/***ASC Fractional Divider Register***/
#define IFX_ASC1_FDV                            ((volatile u32*)(IFX_ASC1 + 0x0018))
#define IFX_ASC1_FDV_FD_VALUE(value)            (((( 1 << 9) - 1) & (value)) << 0)

/***ASC Transmit Buffer Register***/
#define IFX_ASC1_TBUF                           ((volatile u32*)(IFX_ASC1 + 0x0020))
#define IFX_ASC1_TBUF_TD_VALUE(value)           (((( 1 << 9) - 1) & (value)) << 0)

/***ASC Receive Buffer Register***/
#define IFX_ASC1_RBUF                           ((volatile u32*)(IFX_ASC1 + 0x0024))
#define IFX_ASC1_RBUF_RD_VALUE(value)           (((( 1 << 9) - 1) & (value)) << 0)

/***ASC Autobaud Control Register***/
#define IFX_ASC1_ABCON                          ((volatile u32*)(IFX_ASC1 + 0x0030))
#define IFX_ASC1_ABCON_RXINV                    (1 << 11)
#define IFX_ASC1_ABCON_TXINV                    (1 << 10)
#define IFX_ASC1_ABCON_ABEM(value)              (((( 1 << 2) - 1) & (value)) << 8)
#define IFX_ASC1_ABCON_FCDETEN                  (1 << 4)
#define IFX_ASC1_ABCON_ABDETEN                  (1 << 3)
#define IFX_ASC1_ABCON_ABSTEN                   (1 << 2)
#define IFX_ASC1_ABCON_AUREN                    (1 << 1)
#define IFX_ASC1_ABCON_ABEN                     (1 << 0)

/***Receive FIFO Control Register***/
#define IFX_ASC1_RXFCON                         ((volatile u32*)(IFX_ASC1 + 0x0040))
#define IFX_ASC1_RXFCON_RXFITL(value)           (((( 1 << 6) - 1) & (value)) << 8)
#define IFX_ASC1_RXFCON_RXFFLU                  (1 << 1)
#define IFX_ASC1_RXFCON_RXFEN                   (1 << 0)

/***Transmit FIFO Control Register***/
#define IFX_ASC1_TXFCON                         ((volatile u32*)(IFX_ASC1 + 0x0044))
#define IFX_ASC1_TXFCON_TXFITL(value)           (((( 1 << 6) - 1) & (value)) << 8)
#define IFX_ASC1_TXFCON_TXFFLU                  (1 << 1)
#define IFX_ASC1_TXFCON_TXFEN                   (1 << 0)

/***FIFO Status Register***/
#define IFX_ASC1_FSTAT                          ((volatile u32*)(IFX_ASC1 + 0x0048))
#define IFX_ASC1_FSTAT_TXFFL(value)             (((( 1 << 6) - 1) & (value)) << 8)
#define IFX_ASC1_FSTAT_RXFFL(value)             (((( 1 << 6) - 1) & (value)) << 0)
#define IFX_ASC1_FSTAT_TXFREE_GET(value)        (((value) >> 24) & ((1 << 6) - 1))
#define IFX_ASC1_FSTAT_TXFREE_SET(value)        (((( 1 << 6) - 1) & (value)) << 24)
#define IFX_ASC1_FSTAT_RXFREE_GET(value)        (((value) >> 16) & ((1 << 6) - 1))
#define IFX_ASC1_FSTAT_RXFREE_SET(value)        (((( 1 << 6) - 1) & (value)) << 16)
#define IFX_ASC1_FSTAT_TXFFL_GET(value)         (((value) >> 8) & ((1 << 6) - 1))
#define IFX_ASC1_FSTAT_TXFFL_SET(value)         (((( 1 << 6) - 1) & (value)) << 8)
#define IFX_ASC1_FSTAT_RXFFL_GET(value)         (((value) >> 0) & ((1 << 6) - 1))
#define IFX_ASC1_FSTAT_RXFFL_SET(value)         (((( 1 << 6) - 1) & (value)) << 0)


/***ASC Autobaud Status Register***/
#define IFX_ASC1_ABSTAT                         ((volatile u32*)(IFX_ASC1 + 0x0034))
#define IFX_ASC1_ABSTAT_DETWAIT                 (1 << 4)
#define IFX_ASC1_ABSTAT_SCCDET                  (1 << 3)
#define IFX_ASC1_ABSTAT_SCSDET                  (1 << 2)
#define IFX_ASC1_ABSTAT_FCCDET                  (1 << 1)
#define IFX_ASC1_ABSTAT_FCSDET                  (1 << 0)

/***ASC Write HW Modified Autobaud Status Register***/
#define IFX_ASC1_WHBABSTAT                      ((volatile u32*)(IFX_ASC1 + 0x003C))
#define IFX_ASC1_WHBABSTAT_SETDETWAIT           (1 << 9)
#define IFX_ASC1_WHBABSTAT_CLRDETWAIT           (1 << 8)
#define IFX_ASC1_WHBABSTAT_SETSCCDET            (1 << 7)
#define IFX_ASC1_WHBABSTAT_CLRSCCDET            (1 << 6)
#define IFX_ASC1_WHBABSTAT_SETSCSDET            (1 << 5)
#define IFX_ASC1_WHBABSTAT_CLRSCSDET            (1 << 4)
#define IFX_ASC1_WHBABSTAT_SETFCCDET            (1 << 3)
#define IFX_ASC1_WHBABSTAT_CLRFCCDET            (1 << 2)
#define IFX_ASC1_WHBABSTAT_SETFCSDET            (1 << 1)
#define IFX_ASC1_WHBABSTAT_CLRFCSDET            (1 << 0)

/***ASC IRNCR0 **/
#define IFX_ASC1_IRNREN                         ((volatile u32*)(IFX_ASC1 + 0x00F4))
#define IFX_ASC1_IRNICR                         ((volatile u32*)(IFX_ASC1 + 0x00FC))
/***ASC IRNCR1 **/
#define IFX_ASC1_IRNCR                          ((volatile u32*)(IFX_ASC1 + 0x00F8))
#define IFX_ASC_IRNCR_TIR                       0x1
#define IFX_ASC_IRNCR_RIR                       0x2
#define IFX_ASC_IRNCR_EIR                       0x4



/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/

#define IFX_DMA                                 (KSEG1 | 0x1E104100)

#define IFX_DMA_BASE                            IFX_DMA
#define IFX_DMA_CLC                             (volatile u32*)IFX_DMA_BASE
#define IFX_DMA_ID                              (volatile u32*)(IFX_DMA_BASE + 0x08)
#define IFX_DMA_CTRL                            (volatile u32*)(IFX_DMA_BASE + 0x10)
#define IFX_DMA_CPOLL                           (volatile u32*)(IFX_DMA_BASE + 0x14)
#define IFX_DMA_CS                              (volatile u32*)(IFX_DMA_BASE + 0x18)
#define IFX_DMA_CCTRL                           (volatile u32*)(IFX_DMA_BASE + 0x1C)
#define IFX_DMA_CDBA                            (volatile u32*)(IFX_DMA_BASE + 0x20)
#define IFX_DMA_CDLEN                           (volatile u32*)(IFX_DMA_BASE + 0x24)
#define IFX_DMA_CIS                             (volatile u32*)(IFX_DMA_BASE + 0x28)
#define IFX_DMA_CIE                             (volatile u32*)(IFX_DMA_BASE + 0x2C)
#define IFX_DMA_CGBL                            (volatile u32*)(IFX_DMA_BASE + 0x30)
#define IFX_DMA_PS                              (volatile u32*)(IFX_DMA_BASE + 0x40)
#define IFX_DMA_PCTRL                           (volatile u32*)(IFX_DMA_BASE + 0x44)

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
#define IFX_Debug_MCD_MCR                       ((volatile u32*)(IFX_Debug + 0x0008))
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
#define IFX_ICU_IM0_IMR_IID                     (1 << 31)
#define IFX_ICU_IM0_IMR_IN_GET(value)           (((value) >> 0) & ((1 << 5) - 1))
#define IFX_ICU_IM0_IMR_IN_SET(value)           (((( 1 << 5) - 1) & (value)) << 0)
#define IFX_ICU_IM0_IR(value)                   (1 << (value))

#define IFX_ICU_IM1_ISR                         ((volatile u32*)(IFX_ICU + 0x0028))
#define IFX_ICU_IM1_IER                         ((volatile u32*)(IFX_ICU + 0x0030))
#define IFX_ICU_IM1_IOSR                        ((volatile u32*)(IFX_ICU + 0x0038))
#define IFX_ICU_IM1_IRSR                        ((volatile u32*)(IFX_ICU + 0x0040))
#define IFX_ICU_IM1_IMR                         ((volatile u32*)(IFX_ICU + 0x0048))
#define IFX_ICU_IM1_IMR_IID                     (1 << 31)
#define IFX_ICU_IM1_IMR_IN_GET(value)           (((value) >> 0) & ((1 << 5) - 1))
#define IFX_ICU_IM1_IMR_IN_SET(value)           (((( 1 << 5) - 1) & (value)) << 0)
#define IFX_ICU_IM1_IR(value)                   (1 << (value))

#define IFX_ICU_IM2_ISR                         ((volatile u32*)(IFX_ICU + 0x0050))
#define IFX_ICU_IM2_IER                         ((volatile u32*)(IFX_ICU + 0x0058))
#define IFX_ICU_IM2_IOSR                        ((volatile u32*)(IFX_ICU + 0x0060))
#define IFX_ICU_IM2_IRSR                        ((volatile u32*)(IFX_ICU + 0x0068))
#define IFX_ICU_IM2_IMR                         ((volatile u32*)(IFX_ICU + 0x0070))
#define IFX_ICU_IM2_IMR_IID                     (1 << 31)
#define IFX_ICU_IM2_IMR_IN_GET(value)           (((value) >> 0) & ((1 << 5) - 1))
#define IFX_ICU_IM2_IMR_IN_SET(value)           (((( 1 << 5) - 1) & (value)) << 0)
#define IFX_ICU_IM2_IR(value)                   (1 << (value))

#define IFX_ICU_IM3_ISR                         ((volatile u32*)(IFX_ICU + 0x0078))
#define IFX_ICU_IM3_IER                         ((volatile u32*)(IFX_ICU + 0x0080))
#define IFX_ICU_IM3_IOSR                        ((volatile u32*)(IFX_ICU + 0x0088))
#define IFX_ICU_IM3_IRSR                        ((volatile u32*)(IFX_ICU + 0x0090))
#define IFX_ICU_IM3_IMR                         ((volatile u32*)(IFX_ICU + 0x0098))
#define IFX_ICU_IM3_IMR_IID                     (1 << 31)
#define IFX_ICU_IM3_IMR_IN_GET(value)           (((value) >> 0) & ((1 << 5) - 1))
#define IFX_ICU_IM3_IMR_IN_SET(value)           (((( 1 << 5) - 1) & (value)) << 0)
#define IFX_ICU_IM3_IR(value)                   (1 << (value))

#define IFX_ICU_IM4_ISR                         ((volatile u32*)(IFX_ICU + 0x00A0))
#define IFX_ICU_IM4_IER                         ((volatile u32*)(IFX_ICU + 0x00A8))
#define IFX_ICU_IM4_IOSR                        ((volatile u32*)(IFX_ICU + 0x00B0))
#define IFX_ICU_IM4_IRSR                        ((volatile u32*)(IFX_ICU + 0x00B8))
#define IFX_ICU_IM4_IMR                         ((volatile u32*)(IFX_ICU + 0x00C0))
#define IFX_ICU_IM4_IMR_IID                     (1 << 31)
#define IFX_ICU_IM4_IMR_IN_GET(value)           (((value) >> 0) & ((1 << 5) - 1))
#define IFX_ICU_IM4_IMR_IN_SET(value)           (((( 1 << 5) - 1) & (value)) << 0)
#define IFX_ICU_IM4_IR(value)                   (1 << (value))

#define IFX_ICU_IM5_ISR                         ((volatile u32*)(IFX_ICU + 0x00C8))
#define IFX_ICU_IM5_IER                         ((volatile u32*)(IFX_ICU + 0x00D0))
#define IFX_ICU_IM5_IOSR                        ((volatile u32*)(IFX_ICU + 0x00D8))
#define IFX_ICU_IM5_IRSR                        ((volatile u32*)(IFX_ICU + 0x00E0))
#define IFX_ICU_IM5_IMR                         ((volatile u32*)(IFX_ICU + 0x00E8))
#define IFX_ICU_IM5_IMR_IID                     (1 << 31)
#define IFX_ICU_IM5_IMR_IN_GET(value)           (((value) >> 0) & ((1 << 5) - 1))
#define IFX_ICU_IM5_IMR_IN_SET(value)           (((( 1 << 5) - 1) & (value)) << 0)
#define IFX_ICU_IM5_IR(value)                   (1 << (value))

/***Interrupt Vector Value Register***/
#define IFX_ICU_IM_VEC                          ((volatile u32*)(IFX_ICU+ 0x00f8))

/***Interrupt Vector Value Mask***/
#define IFX_ICU_IM0_VEC_MASK                    0x0000003f
#define IFX_ICU_IM1_VEC_MASK                    0x00000fc0
#define IFX_ICU_IM2_VEC_MASK                    0x0003f000
#define IFX_ICU_IM3_VEC_MASK                    0x00fc0000
#define IFX_ICU_IM4_VEC_MASK                    0x3f000000

#define IFX_ICU_IM0_ISR_IR(value)               (1<<(value))
#define IFX_ICU_IM0_IER_IR(value)               (1<<(value))
#define IFX_ICU_IM1_ISR_IR(value)               (1<<(value))
#define IFX_ICU_IM1_IER_IR(value)               (1<<(value))
#define IFX_ICU_IM2_ISR_IR(value)               (1<<(value))
#define IFX_ICU_IM2_IER_IR(value)               (1<<(value))
#define IFX_ICU_IM3_ISR_IR(value)               (1<<(value))
#define IFX_ICU_IM3_IER_IR(value)               (1<<(value))
#define IFX_ICU_IM4_ISR_IR(value)               (1<<(value))
#define IFX_ICU_IM4_IER_IR(value)               (1<<(value))
#define IFX_ICU_IM5_ISR_IR(value)               (1<<(value))
#define IFX_ICU_IM5_IER_IR(value)               (1<<(value))

/***External Interrupt Control Register***/
#define IFX_ICU_EIU                             (KSEG1+0x1f101000)
#define IFX_ICU_EIU_EXIN_C                      ((volatile u32*)(IFX_ICU_EIU + 0x0000))
#define IFX_ICU_EIU_INIC                        ((volatile u32*)(IFX_ICU_EIU + 0x0004))
#define IFX_ICU_EIU_INC                         ((volatile u32*)(IFX_ICU_EIU + 0x0008))
#define IFX_ICU_EIU_INEN                        ((volatile u32*)(IFX_ICU_EIU + 0x000c))

/***********************************************************************/
/*  Module      :  MPS register address and bits                       */
/***********************************************************************/

#define IFX_MPS                                 (KSEG1 | 0x1F107000)

#define IFX_MPS_CHIPID                          ((volatile u32*)(IFX_MPS + 0x0344))
#define IFX_MPS_CHIPID_VERSION_GET(value)       (((value) >> 28) & 0xF)
#define IFX_MPS_CHIPID_VERSION_SET(value)       (((value) & 0xF) << 28)
#define IFX_MPS_CHIPID_PARTNUM_GET(value)       (((value) >> 12) & 0xFFFF)
#define IFX_MPS_CHIPID_PARTNUM_SET(value)       (((value) & 0xFFFF) << 12)
#define IFX_MPS_CHIPID_MANID_GET(value)         (((value) >> 1) & 0x7FF)
#define IFX_MPS_CHIPID_MANID_SET(value)         (((value) & 0x7FF) << 1)



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

/* hash control registe */
#define IFX_HASH_CON                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B0))
#define IFX_HASH_MR                             ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B4))
#define IFX_HASH_D1R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00B8 ))
#define IFX_HASH_D2R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00BC ))
#define IFX_HASH_D3R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00C0 ))
#define IFX_HASH_D4R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00C4))
#define IFX_HASH_D5R                            ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00C8))

#define IFX_CON                                 ((volatile u32 *)(IFX_DEU_BASE_ADDR + 0x00EC))

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



#endif //IFX_H
