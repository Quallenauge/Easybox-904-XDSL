/******************************************************************************
**
** FILE NAME    : ifx_pmu.h
** PROJECT      : IFX UEIP
** MODULES      : PMU
**
** DATE         : 28 May 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX Cross-Platform Power Management Unit driver header file
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
** 28 May 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/
#ifndef IFX_PMU_H
#define IFX_PMU_H

/*!
 \defgroup IFX_PMU PMU driver module   
 \brief UEIP Project - PMU module. 
*/

/*!
 \defgroup IFX_PMU_DEFINITIONS Definition and structions
 \ingroup IFX_PMU
 \brief definitions for ifx pmu driver 
*/

/*!
 \file ifx_pmu.h
 \ingroup IFX_PMU  
 \brief header file for PMU external interface
*/
/*!  
 \addtogroup IFX_PMU_DEFINITIONS
*/
/* @{ */

/*! \def IFX_PMU_MODULE_USB0_PHY 
    \brief USB0 PHY Module 
 */
#define IFX_PMU_MODULE_USB0_PHY   (0)

/*! \def IFX_PMU_MODULE_USB_PHY 
    \brief USB PHY Module 
 */
#define IFX_PMU_MODULE_USB_PHY    IFX_PMU_MODULE_USB0_PHY

/*! \def IFX_PMU_MODULE_FPIS 
    \brief FPI Slave Module 
 */
#define IFX_PMU_MODULE_FPIS       (1)

/*! \def IFX_PMU_MODULE_FPI1 
    \brief FPI bus 1 Module 
 */
#define IFX_PMU_MODULE_FPI1       IFX_PMU_MODULE_FPIS

/*! \def IFX_PMU_MODULE_FPI2 
    \brief FPI bus 2 Module 
 */
#define IFX_PMU_MODULE_FPI2       IFX_PMU_MODULE_FPI1

/*! \def IFX_PMU_MODULE_DFEV0 
    \brief DFEV 0 Module 
 */
#define IFX_PMU_MODULE_DFEV0      (2)
#ifdef CONFIG_AMAZON_SE
/*! \def IFX_PMU_MODULE_SDIO 
    \brief SDIO Module, ASE only 
 */
#define IFX_PMU_MODULE_SDIO       IFX_PMU_MODULE_DFEV0
#endif 
/*! \def IFX_PMU_MODULE_DFEV1 
    \brief DFEV 1 Module
 */
#define IFX_PMU_MODULE_DFEV1      (3)

/*! \def IFX_PMU_MODULE_VO_MIPS 
    \brief Voice MIPS Module
 */
#define IFX_PMU_MODULE_VO_MIPS    IFX_PMU_MODULE_DFEV0

/*! \def IFX_PMU_MODULE_VODEC 
    \brief Voice Decoder  Module
 */
#define IFX_PMU_MODULE_VODEC      IFX_PMU_MODULE_DFEV1

/*! \def IFX_PMU_MODULE_PCI 
    \brief PCI Module
 */
#define IFX_PMU_MODULE_PCI        (4)

/*! \def IFX_PMU_MODULE_DMA 
    \brief DMA Module
 */
#define IFX_PMU_MODULE_DMA        (5)

/*! \def IFX_PMU_MODULE_USB0_CTRL 
    \brief USB0 Controller Module
 */
#define IFX_PMU_MODULE_USB0_CTRL  (6)

/*! \def IFX_PMU_MODULE_USB_CTRL 
    \brief USB Controller Module
 */
#define IFX_PMU_MODULE_USB_CTRL   IFX_PMU_MODULE_USB0_CTRL

/*! \def IFX_PMU_MODULE_USIF 
    \brief USIF  Module
 */
#define IFX_PMU_MODULE_USIF       (7)

/*! \def IFX_PMU_MODULE_UART0 
    \brief UART0/ASC0  Module
 */
#define IFX_PMU_MODULE_UART0      IFX_PMU_MODULE_USIF

/*! \def IFX_PMU_MODULE_EPHY 
    \brief EPHY  Module
 */
#define IFX_PMU_MODULE_EPHY       IFX_PMU_MODULE_UART0

/*! \def IFX_PMU_MODULE_SPI 
    \brief SSC/SPI Module
 */
#define IFX_PMU_MODULE_SPI        (8)

/*! \def IFX_PMU_MODULE_DSL_DFE 
    \brief DSL /DFE Module
 */
#define IFX_PMU_MODULE_DSL_DFE    (9)

/*! \def IFX_PMU_MODULE_EBU 
    \brief EBU Module
 */
#define IFX_PMU_MODULE_EBU        (10)

/*! \def IFX_PMU_MODULE_LEDC 
    \brief LEDc Controller  Module
 */
#define IFX_PMU_MODULE_LEDC       (11)

/*! \def IFX_PMU_MODULE_GPTC 
    \brief GPTU  Module
 */
#define IFX_PMU_MODULE_GPTC       (12)

/*! \def IFX_PMU_MODULE_AHBS 
    \brief AHB Slave Module
 */
#define IFX_PMU_MODULE_AHBS       (13)

/*! \def IFX_PMU_MODULE_PCIE1_PHY 
    \brief PCIe 1 PHY module
 */
#define IFX_PMU_MODULE_PCIE1_PHY   IFX_PMU_MODULE_AHBS

/*! \def IFX_PMU_MODULE_PPE_TPE 
    \brief PPE/TPE Module
 */
#define IFX_PMU_MODULE_PPE_TPE    IFX_PMU_MODULE_AHBS

/*! \def IFX_PMU_MODULE_VLYNQ 
    \brief VLYNQ Module
 */
#define IFX_PMU_MODULE_VLYNQ      IFX_PMU_MODULE_AHBS

/*! \def IFX_PMU_MODULE_FPIM 
    \brief FPI Master Module
 */
#define IFX_PMU_MODULE_FPIM       (14)

/*! \def IFX_PMU_MODULE_ADSL_AFE 
    \brief AR10 ADSL AFE Module
 */
#define IFX_PMU_MODULE_ADSL_AFE   IFX_PMU_MODULE_FPIM

/*! \def IFX_PMU_MODULE_FPI0 
    \brief FPI bus 0 Module
 */
#define IFX_PMU_MODULE_FPI0       IFX_PMU_MODULE_FPIM

/*! \def IFX_PMU_MODULE_AHBM 
    \brief AHB Master Module
 */
#define IFX_PMU_MODULE_AHBM       (15)

/*! \def IFX_PMU_MODULE_DCDC_2V5 
    \brief AR10 DCDC 2.5V Module
 */
#define IFX_PMU_MODULE_DCDC_2V5   IFX_PMU_MODULE_AHBM

/*! \def IFX_PMU_MODULE_AHB 
    \brief AHB  Module
 */
#define IFX_PMU_MODULE_AHB        IFX_PMU_MODULE_AHBM
#ifndef CONFIG_AMAZON_SE
/*! \def IFX_PMU_MODULE_SDIO 
    \brief SDIO  Module, ARX,Danube, VRX
 */
#define IFX_PMU_MODULE_SDIO       (16)
#endif
/*! \def IFX_PMU_MODULE_UART1 
    \brief UART1/ASC1 module
 */
#define IFX_PMU_MODULE_UART1      (17)

/*! \def IFX_PMU_MODULE_PPE_QSB 
    \brief PPE QSB module
 */
#define IFX_PMU_MODULE_PPE_QSB    (18)

/*! \def IFX_PMU_MODULE_DCDC_1VX 
    \brief AR10 DCDC 1.5V and 1.8V Module
 */
#define IFX_PMU_MODULE_DCDC_1VX    IFX_PMU_MODULE_PPE_QSB

/*! \def IFX_PMU_MODULE_PPE_SLL01 
    \brief PPE SLL01 module
 */
#define IFX_PMU_MODULE_PPE_SLL01  (19)

/*! \def IFX_PMU_MODULE_DCDC_1V0 
    \brief AR10 DCDC 1.0V Module
 */
#define IFX_PMU_MODULE_DCDC_1V0    IFX_PMU_MODULE_PPE_SLL01

/*! \def IFX_PMU_MODULE_WDT0 
    \brief WDT0 module
 */
#define IFX_PMU_MODULE_WDT0       IFX_PMU_MODULE_PPE_QSB

/*! \def IFX_PMU_MODULE_WDT1 
    \brief WDT1 module
 */
#define IFX_PMU_MODULE_WDT1       IFX_PMU_MODULE_PPE_SLL01

/*! \def IFX_PMU_MODULE_DEU 
    \brief DEU module
 */
#define IFX_PMU_MODULE_DEU        (20)

/*! \def IFX_PMU_MODULE_PPE_TC 
    \brief PPE TC module
 */
#define IFX_PMU_MODULE_PPE_TC     (21)

/*! \def IFX_PMU_MODULE_PPE_EMA 
    \brief PPE EMA module
 */
#define IFX_PMU_MODULE_PPE_EMA    (22)

/*! \def IFX_PMU_MODULE_PPE_ENET1 
    \brief PPE ENET1 module
 */
#define IFX_PMU_MODULE_PPE_ENET1  IFX_PMU_MODULE_PPE_EMA

/*! \def IFX_PMU_MODULE_PPE_DPLUSM 
    \brief Dplus Master module
 */
#define IFX_PMU_MODULE_PPE_DPLUSM (23)

/*! \def IFX_PMU_MODULE_PPE_DPLUS 
    \brief Dplus module
 */
#define IFX_PMU_MODULE_PPE_DPLUS  IFX_PMU_MODULE_PPE_DPLUSM

/*! \def IFX_PMU_MODULE_PPE_ENET0 
    \brief PPE ENET0 module
 */
#define IFX_PMU_MODULE_PPE_ENET0  IFX_PMU_MODULE_PPE_DPLUSM

/*! \def IFX_PMU_MODULE_PPE_DPLUSS 
    \brief Dplus Slave module
 */
#define IFX_PMU_MODULE_PPE_DPLUSS (24)

/*! \def IFX_PMU_MODULE_DDR_MEM 
    \brief DDR MC module
 */
#define IFX_PMU_MODULE_DDR_MEM    IFX_PMU_MODULE_PPE_DPLUSS

/*! \def IFX_PMU_MODULE_TDM 
    \brief TDM module
 */
#define IFX_PMU_MODULE_TDM        (25)

/*! \def IFX_PMU_MODULE_USB1_PHY 
    \brief USB 1 PHY module
 */
#define IFX_PMU_MODULE_USB1_PHY   (26)

/*! \def IFX_PMU_MODULE_USB1_CTRL 
    \brief USB1 Controller module
 */
#define IFX_PMU_MODULE_USB1_CTRL  (27)

/*! \def IFX_PMU_MODULE_SWITCH 
    \brief Switch module
 */
#define IFX_PMU_MODULE_SWITCH     (28)

/*! \def IFX_PMU_MODULE_PPE_TOP 
    \brief PPE Top module
 */
#define IFX_PMU_MODULE_PPE_TOP    (29)

/*! \def IFX_PMU_MODULE_DDR_DPD 
    \brief DDR DPD module
 */
#define IFX_PMU_MODULE_DDR_DPD    IFX_PMU_MODULE_PPE_TOP

/*! \def IFX_PMU_MODULE_GPHY0 
    \brief GPHY0 module
 */
#define IFX_PMU_MODULE_GPHY0    IFX_PMU_MODULE_PPE_TOP


/*! \def IFX_PMU_MODULE_GPHY 
    \brief Internal GPHY module
 */
#define IFX_PMU_MODULE_GPHY       (30)

/*! \def IFX_PMU_MODULE_GPHY1 
    \brief Internal GPHY1 module
 */
#define IFX_PMU_MODULE_GPHY1       IFX_PMU_MODULE_GPHY

/*! \def IFX_PMU_MODULE_PCIE_L0_CLK 
    \brief PCIe L0 Clock  module
 */
#define IFX_PMU_MODULE_PCIE_L0_CLK (31)

/*! \def IFX_PMU_MODULE_GPHY2 
    \brief Internal GPHY2 module
 */
#define IFX_PMU_MODULE_GPHY2      IFX_PMU_MODULE_PCIE_L0_CLK

/*! \def IFX_PMU_MODULE_PCIE_PHY 
    \brief PCIe PHY module
 */
#define IFX_PMU_MODULE_PCIE_PHY   (32)

/*! \def IFX_PMU_MODULE_PCIE0_PHY 
    \brief PCIe 0 PHY module
 */
#define IFX_PMU_MODULE_PCIE0_PHY   IFX_PMU_MODULE_PCIE_PHY

/*! \def IFX_PMU_MODULE_PCIE_CTRL 
    \brief PCIe Controller module
 */
#define IFX_PMU_MODULE_PCIE_CTRL  (33)

/*! \def IFX_PMU_MODULE_PCIE0_CTRL 
    \brief PCIe Controller 0 module
 */
#define IFX_PMU_MODULE_PCIE0_CTRL  IFX_PMU_MODULE_PCIE_CTRL

/*! \def IFX_PMU_MODULE_AHB_ARC 
    \brief AHB ARC module
 */
#define IFX_PMU_MODULE_AHB_ARC    (34)  /* XXX */

/*! \def IFX_PMU_MODULE_PCIE1_CTRL 
    \brief PCIe Controller 1 module
 */
#define IFX_PMU_MODULE_PCIE1_CTRL   IFX_PMU_MODULE_AHB_ARC

/*! \def IFX_PMU_MODULE_HSNAND 
    \brief High Speed NAND module
 */
#define IFX_PMU_MODULE_HSNAND     (35)  /* XXX */

/*! \def IFX_PMU_MODULE_PDI1 
    \brief PCIe PDI 1 module
 */
#define IFX_PMU_MODULE_PDI1       IFX_PMU_MODULE_HSNAND


/*! \def IFX_PMU_MODULE_PDI 
    \brief PDI module
 */
#define IFX_PMU_MODULE_PDI        (36)

/*! \def IFX_PMU_MODULE_PDI0 
    \brief PCIe PDI module
 */
#define IFX_PMU_MODULE_PDI0       IFX_PMU_MODULE_PDI

/*! \def IFX_PMU_MODULE_MSI 
    \brief PCIe MSI module
 */
#define IFX_PMU_MODULE_MSI        (37)

/*! \def IFX_PMU_MODULE_MSI0 
    \brief PCIe MSI 0 module
 */
#define IFX_PMU_MODULE_MSI0       IFX_PMU_MODULE_MSI

/*! \def IFX_PMU_MODULE_DDR_CKE 
    \brief DDR CKE module
 */
#define IFX_PMU_MODULE_DDR_CKE    (38)


/*! \def IFX_PMU_MODULE_MSI1 
    \brief PCIe MSI 1 module
 */
#define IFX_PMU_MODULE_MSI1       (39)

/* Will be exported to user space, __ prefix is used, see linux/types.h */
typedef struct ifx_pmu_clk {
    __u32 module;  /* Clok module index */
    __u32 enable;  /* enable or disable  */
} ifx_pmu_clk_t;

/* Used by ioctl */
#define IFX_PMU_IOC_MAGIC   0xe0

#define IFX_PMU_IOC_CLK_ENABLE        (1)
#define IFX_PMU_IOC_CLK_DISABLE       (2)
#define IFX_PMU_IOC_CLK_GET           (3)

#define IFX_PMU_IOC_CLK_GATING_ENABLE    _IOW(IFX_PMU_IOC_MAGIC, IFX_PMU_IOC_CLK_ENABLE ,ifx_pmu_clk_t)
#define IFX_PMU_IOC_CLK_GATING_DISABLE   _IOW(IFX_PMU_IOC_MAGIC, IFX_PMU_IOC_CLK_DISABLE ,ifx_pmu_clk_t)
#define IFX_PMU_IOC_GET_CLK_GATING       _IOR(IFX_PMU_IOC_MAGIC, IFX_PMU_IOC_CLK_GET ,ifx_pmu_clk_t)


#ifdef CONFIG_IFX_PMU_POWER_GATING

/* 
 * Power Gating Power domain Index definition
 * The underlying power domain continues to evolve and change. To maintain the
 * forward compatibiblity with future hardware platforms. Index is better. 
 * However, every new platform has to maintain one index-to-power domain table.
 */

/*! \def IFX_PMU_PG_DOMAIN_USB
    \brief USB power domain 
 */
#define IFX_PMU_PG_DOMAIN_USB          0

/*! \def IFX_PMU_PG_DOMAIN_PCIE
    \brief PCI express power domain 
 */
#define IFX_PMU_PG_DOMAIN_PCIE         1

/*! \def IFX_PMU_PG_DOMAIN_PCIE
    \brief SLIC plus TDM power domain 
 */
#define IFX_PMU_PG_DOMAIN_SLIC_TDM     2

/*! \def IFX_PMU_PG_DOMAIN_DEU
    \brief DEU power domain 
 */
#define IFX_PMU_PG_DOMAIN_DEU          3

/*! \def IFX_PMU_PG_DOMAIN_FPI_TOP
    \brief FPI Top power domain 
 */
#define IFX_PMU_PG_DOMAIN_FPI_TOP      4

/*! \def IFX_PMU_PG_DOMAIN_PPE
    \brief PPE power domain 
 */
#define IFX_PMU_PG_DOMAIN_PPE          5

/*! \def IFX_PMU_PG_DOMAIN_SWITCH
    \brief Switch power domain 
 */
#define IFX_PMU_PG_DOMAIN_SWITCH       6

/*! \def IFX_PMU_PG_DOMAIN_MIPS
    \brief MIPS Core power domain 
 */
#define IFX_PMU_PG_DOMAIN_MIPS         7

/*! \def IFX_PMU_PG_DOMAIN_DSL_DFE
    \brief DSL DFE power domain 
 */
#define IFX_PMU_PG_DOMAIN_DSL_DFE      8

/*! \def IFX_PMU_PG_DOMAIN_WLAN
    \brief Built-in WiFi power domain 
 */
#define IFX_PMU_PG_DOMAIN_WLAN         9

/*! \def IFX_PMU_PG_DOMAIN_GPHY0
    \brief Giga bit PHY0 power domain 
 */
#define IFX_PMU_PG_DOMAIN_GPHY0        10

/*! \def IFX_PMU_PG_DOMAIN_GPHY1
    \brief Giga bit PHY1 power domain 
 */
#define IFX_PMU_PG_DOMAIN_GPHY1        11

/*! \def IFX_PMU_PG_DOMAIN_GPHY2
    \brief Giga bit PHY2 power domain 
 */
#define IFX_PMU_PG_DOMAIN_GPHY2        12

#define IFX_PMU_PG_DOMAIN_RES0         13
#define IFX_PMU_PG_DOMAIN_RES1         14
#define IFX_PMU_PG_DOMAIN_RES2         15

/* XXX, more definition */

#define IFX_POWER_DOMAIN(X) IFX_PMU_PG_DOMAIN_##X

/* Will be exported to user space, __ prefix is used, see linux/types.h */
typedef struct ifx_pmu_pg {
    __u32 power_domain;  /* Power Domain index */
    __u32 flags;         /* Future use */
} ifx_pmu_pg_t;


/* XXX, don't overlap with Clock Gating */
#define IFX_PMU_IOC_PG_ENABLE        (60)
#define IFX_PMU_IOC_PG_DISABLE       (61)
#define IFX_PMU_IOC_PG_GET           (62)

#define IFX_PMU_IOC_POWER_GATING_ENABLE    _IOW(IFX_PMU_IOC_MAGIC, IFX_PMU_IOC_PG_ENABLE ,ifx_pmu_pg_t)
#define IFX_PMU_IOC_POWER_GATING_DISABLE   _IOW(IFX_PMU_IOC_MAGIC, IFX_PMU_IOC_PG_DISABLE ,ifx_pmu_pg_t)
#define IFX_PMU_IOC_GET_POWER_GATING       _IOR(IFX_PMU_IOC_MAGIC, IFX_PMU_IOC_PG_GET ,ifx_pmu_pg_t)

#endif /* CONFIG_IFX_PMU_POWER_GATING */
/* @} */

#ifdef __KERNEL__

#define IFX_PMU_ENABLE    1
#define IFX_PMU_DISABLE   0

#define USB0_PHY_PMU_SETUP(__x)   ifx_pmu_set(IFX_PMU_MODULE_USB0_PHY, (__x))
#define USB_PHY_PMU_SETUP(__x)    USB0_PHY_PMU_SETUP((__x))
#define FPIS_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_FPIS, (__x))
#define FPI1_PMU_SETUP(__x)       FPIS_PMU_SETUP((__x))
#define FPI2_PMU_SETUP(__x)       FPIS_PMU_SETUP((__x))
#define DFEV0_PMU_SETUP(__x)      ifx_pmu_set(IFX_PMU_MODULE_DFEV0, (__x))
#ifdef CONFIG_AMAZON_SE
#define SDIO_PMU_SETUP(__x)       DFEV0_PMU_SETUP((__x))
#endif 
#define DFEV1_PMU_SETUP(__x)      ifx_pmu_set(IFX_PMU_MODULE_DFEV1, (__x))
#define VO_MIPS_PMU_SETUP(__x)    DFEV0_PMU_SETUP((__x))
#define VODEC_PMU_SETUP(__x)      DFEV1_PMU_SETUP((__x))
#define PCI_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_PCI, (__x))
#define DMA_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_DMA, (__x))
#define USB0_CTRL_PMU_SETUP(__x)  ifx_pmu_set(IFX_PMU_MODULE_USB0_CTRL, (__x))
#define USIF_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_USIF, (__x))
#define UART0_PMU_SETUP(__x)      USIF_PMU_SETUP((__x)) 
#define EPHY_PMU_SETUP(__x)       USIF_PMU_SETUP((__x)) 
#define SPI_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_SPI, (__x))
#define DSL_DFE_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_DSL_DFE, (__x))
#define EBU_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_EBU, (__x))
#define LEDC_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_LEDC, (__x))
#define GPTC_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_GPTC, (__x))

#define AHBS_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_AHBS, (__x))
#define VLYNQ_PMU_SETUP(__x)      AHBS_PMU_SETUP((__x)) 
#define PPE_TPE_PMU_SETUP(__x)    AHBS_PMU_SETUP((__x))
#define PCIE1_PHY_PMU_SETUP(__x)  AHBS_PMU_SETUP((__x))

#define FPIM_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_FPIM, (__x))
#define FPI0_PMU_SETUP(__x)       FPIM_PMU_SETUP((__x))
#define ADSL_AFE_PMU_SETUP(__x)   FPIM_PMU_SETUP((__x))

#define AHBM_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_AHBM, (__x))
#define AHB_PMU_SETUP(__x)        AHBM_PMU_SETUP((__x))
#define DCDC_2V5_PMU_SETUP(__x)   AHBM_PMU_SETUP((__x))

#ifndef CONFIG_AMAZON_SE
#define SDIO_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_SDIO, (__x))
#endif 
#define UART1_PMU_SETUP(__x)      ifx_pmu_set(IFX_PMU_MODULE_UART1, (__x))
#define PPE_QSB_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_PPE_QSB, (__x))
#define WDT0_PMU_SETUP(__x)       PPE_QSB_PMU_SETUP((__x))
#define DCDC_1VX_PMU_SETUP(__x)   PPE_QSB_PMU_SETUP((__x))

#define PPE_SLL01_PMU_SETUP(__x)  ifx_pmu_set(IFX_PMU_MODULE_PPE_SLL01, (__x))
#define DCDC_1V0_PMU_SETUP(__x)   PPE_SLL01_PMU_SETUP((__x)) 

#define WDT1_PMU_SETUP(__x)       PPE_SLL01_PMU_SETUP((__x))
#define DEU_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_DEU, (__x))
#define PPE_TC_PMU_SETUP(__x)     ifx_pmu_set(IFX_PMU_MODULE_PPE_TC, (__x))
#define PPE_EMA_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_PPE_EMA, (__x))
#define PPE_ENET1_PMU_SETUP(__x)  PPE_EMA_PMU_SETUP((__x))
#define PPE_DPLUSM_PMU_SETUP(__x) ifx_pmu_set(IFX_PMU_MODULE_PPE_DPLUSM, (__x))
#define PPE_DPLUS_PMU_SETUP(__x)  PPE_DPLUSM_PMU_SETUP((__x))
#define PPE_ENET0_PMU_SETUP(__x)  PPE_DPLUS_PMU_SETUP((__x))
#define PPE_DPLUSS_PMU_SETUP(__x) ifx_pmu_set(IFX_PMU_MODULE_PPE_DPLUSS, (__x))
#define DDR_MEM_PMU_SETUP(__x)    PPE_DPLUSS_PMU_SETUP((__x)) 
#define TDM_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_TDM, (__x))
#define USB1_PHY_PMU_SETUP(__x)   ifx_pmu_set(IFX_PMU_MODULE_USB1_PHY, (__x))
#define USB1_CTRL_PMU_SETUP(__x)  ifx_pmu_set(IFX_PMU_MODULE_USB1_CTRL, (__x))
#define SWITCH_PMU_SETUP(__x)     ifx_pmu_set(IFX_PMU_MODULE_SWITCH, (__x))

#define PPE_TOP_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_PPE_TOP, (__x))
#define GPHY0_PMU_SETUP(__x)      PPE_TOP_PMU_SETUP((__x))
#define DDR_DPD_PMU_SETUP(__x)    PPE_TOP_PMU_SETUP((__x))

#define GPHY_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_GPHY, (__x))
#define GPHY1_PMU_SETUP(__x)      GPHY_PMU_SETUP((__x))

#define PCIE_L0_CLK_PMU_SETUP(__x) ifx_pmu_set(IFX_PMU_MODULE_PCIE_L0_CLK, (__x))
#define GPHY2_PMU_SETUP(__x)       PCIE_L0_CLK_PMU_SETUP((__x))

#define PCIE_PHY_PMU_SETUP(__x)   ifx_pmu_set(IFX_PMU_MODULE_PCIE_PHY, (__x))
#define PCIE0_PHY_PMU_SETUP(__x)  PCIE_PHY_PMU_SETUP((__x))

#define PCIE_CTRL_PMU_SETUP(__x)  ifx_pmu_set(IFX_PMU_MODULE_PCIE_CTRL, (__x))
#define PCIE0_CTRL_PMU_SETUP(__x) PCIE_CTRL_PMU_SETUP((__x))

#define AHB_ARC_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_AHB_ARC, (__x))
#define PCIE1_CTRL_PMU_SETUP(__x) AHB_ARC_PMU_SETUP((__x))

#define HSNAND_PMU_SETUP(__x)     ifx_pmu_set(IFX_PMU_MODULE_HSNAND, (__x))
#define PDI1_PMU_SETUP(__x)       HSNAND_PMU_SETUP((__x))

#define PDI_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_PDI, (__x))
#define PDI0_PMU_SETUP(__x)       PDI_PMU_SETUP((__x))

#define MSI_PMU_SETUP(__x)        ifx_pmu_set(IFX_PMU_MODULE_MSI, (__x))
#define MSI0_PMU_SETUP(__x)       MSI_PMU_SETUP((__x))

#define DDR_CKE_PMU_SETUP(__x)    ifx_pmu_set(IFX_PMU_MODULE_DDR_CKE, (__x))

#define MSI1_PMU_SETUP(__x)       ifx_pmu_set(IFX_PMU_MODULE_MSI1, (__x))

extern int ifx_pmu_set(int module, int value);
extern void ifx_pmu_enable_all_modules(void);
extern void ifx_pmu_disable_all_modules(void);
#ifdef CONFIG_IFX_PMU_POWER_GATING
extern int ifx_pmu_pg_enable(ifx_pmu_pg_t *pg);
extern void ifx_pmu_pg_enable_all_domains(void);
extern void ifx_pmu_pg_disable_all_domains(void);
extern int ifx_pmu_pg_disable(ifx_pmu_pg_t *pg);
extern int ifx_pmu_pg_mips_enable(void);
extern int ifx_pmu_pg_mips_disable(void);
extern int ifx_pmu_pg_usb_enable(void);
extern int ifx_pmu_pg_usb_disable(void);
extern int ifx_pmu_pg_pcie_enable(void);
extern int ifx_pmu_pg_pcie_disable(void);
extern int ifx_pmu_pg_switch_enable(void);
extern int ifx_pmu_pg_switch_disable(void);
extern int ifx_pmu_pg_deu_enable(void);
extern int ifx_pmu_pg_deu_disable(void);
extern int ifx_pmu_pg_ppe_enable(void);
extern int ifx_pmu_pg_ppe_disable(void);
extern int ifx_pmu_pg_dsl_dfe_enable(void);
extern int ifx_pmu_pg_dsl_dfe_disable(void);
extern int ifx_pmu_pg_fpi_top_enable(void);
extern int ifx_pmu_pg_fpi_top_disable(void);
extern int ifx_pmu_pg_slic_tdm_enable(void);
extern int ifx_pmu_pg_slic_tdm_disable(void);
extern int ifx_pmu_pg_wkup_loc_setup(void *loc);
#else
static inline void ifx_pmu_pg_enable_all_domains(void)
{
}

static inline void ifx_pmu_pg_disable_all_domains(void)
{
}

static inline int ifx_pmu_pg_mips_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_mips_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_usb_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_usb_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_pcie_enable(void)
{
    return 0;

}

static inline int ifx_pmu_pg_pcie_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_switch_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_switch_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_deu_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_deu_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_ppe_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_ppe_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_dsl_dfe_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_dsl_dfe_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_fpi_top_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_fpi_top_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_slic_tdm_enable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_slic_tdm_disable(void)
{
    return 0;
}

static inline int ifx_pmu_pg_wkup_loc_setup(void *loc)
{
    return 0;
}
#endif /* CONFIG_IFX_PMU_POWER_GATING */

#endif /* __KERNEL__ */
#endif /* IFX_PMU_H */

