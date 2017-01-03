/******************************************************************************
**
** FILE NAME    : ifx_rcu.h
** PROJECT      : UEIP
** MODULES      : RCU (Reset Control Unit)
**
** DATE         : 17 Jun 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global RCU driver header file
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

#ifndef IFX_RCU_H
#define IFX_RCU_H



/*!
  \defgroup IFX_RCU UEIP Project - RCU (Reset) driver module
  \brief UEIP Project - RCU (Reset) driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_RCU_API APIs
  \ingroup IFX_RCU
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_RCU_IOCTL IOCTL Commands
  \ingroup IFX_RCU
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_RCU_STRUCT Structures
  \ingroup IFX_RCU
  \brief Structures used by user application.
 */

/*!
  \file ifx_rcu.h
  \ingroup IFX_RCU
  \brief RCU driver header file
 */



/*
 * ####################################
 *              Definition
 * ####################################
 */

enum {
    IFX_RCU_DOMAIN_HRST  = 0,
    IFX_RCU_DOMAIN_CPU0,        //  Main CPU
    IFX_RCU_DOMAIN_FPI,         //  FPI bus
    IFX_RCU_DOMAIN_DSLDSP,      //  DSL DSP
    IFX_RCU_DOMAIN_USB1,        //  USB1 & PHY
    IFX_RCU_DOMAIN_USB0,        //  USB0 & PHY
    IFX_RCU_DOMAIN_ETHMAC1,     //  2nd Ethernet MAC (Danube Only)
    IFX_RCU_DOMAIN_AHB,         //  AHB bus
    IFX_RCU_DOMAIN_DSLDFE,      //  DSL DFE
    IFX_RCU_DOMAIN_PPE,         //  PPE module
    IFX_RCU_DOMAIN_DMA,         //  DMA module
    IFX_RCU_DOMAIN_SDIO,        //  SDIO module
    IFX_RCU_DOMAIN_DSLAFE,      //  DSL AFE
    IFX_RCU_DOMAIN_VOICE,       //  Voice DFE/AFE
    IFX_RCU_DOMAIN_PCI,         //  PCI bus
    IFX_RCU_DOMAIN_MC,          //  Memory Controller module
    IFX_RCU_DOMAIN_SW,          //  Internal Switch
    IFX_RCU_DOMAIN_TDM,         //  TDM interface (Voice)
    IFX_RCU_DOMAIN_DSLTC,       //  (ATM) TC module in PPE
    IFX_RCU_DOMAIN_CPU1,        //  2nd CPU (Danube only)
    IFX_RCU_DOMAIN_EPHY,        //  EPHY (Amazon-SE only)
    IFX_RCU_DOMAIN_GPHY0,       //  GPHY0 (VR9 only)
    IFX_RCU_DOMAIN_GPHY1,       //  GPHY1 (VR9 only)
    IFX_RCU_DOMAIN_ARC,         //  ARC (DSL DSP)
    IFX_RCU_DOMAIN_PCIE_PHY,    //  PCIexpress PHY (VR9 only)
    IFX_RCU_DOMAIN_PCIE,        //  PCIexpress core (VR9 only)
    IFX_RCU_DOMAIN_HSNAND,      //  High Speed NAND Flash Interface (VR9 only)
    //  add more component in the future
    IFX_RCU_DOMAIN_MAX,
};

#define IFX_RCU_DECLARE_DOMAIN_NAME(var)    \
    char *var[] = {                         \
        "HRST",                             \
        "Main CPU",                         \
        "FPI bus",                          \
        "DSL DSP",                          \
        "USB1 & PHY",                       \
        "USB0 & PHY",                       \
        "2nd MAC",                          \
        "AHB bus",                          \
        "DSL DFE",                          \
        "PPE",                              \
        "DMA",                              \
        "SDIO",                             \
        "DSL AFE",                          \
        "Voice DFE/AFE",                    \
        "PCI bus",                          \
        "Memory Controller",                \
        "Internal Switch",                  \
        "TDM interface",                    \
        "DSL TC",                           \
        "2nd CPU",                          \
        "EPHY",                             \
        "GPHY0",                            \
        "GPHY1",                            \
        "ARC",                              \
        "PCIe PHY",                         \
        "PCIe core",                        \
        "High Speed NAND",                  \
    }

enum {
    IFX_RCU_MODULE_USB,
    IFX_RCU_MODULE_ETH,
    IFX_RCU_MODULE_ATM,
    IFX_RCU_MODULE_PTM,
    IFX_RCU_MODULE_PPA,
    IFX_RCU_MODULE_DMA,
    IFX_RCU_MODULE_SDIO,
    IFX_RCU_MODULE_MEI,
    IFX_RCU_MODULE_TAPI,
    IFX_RCU_MODULE_PCI,
    IFX_RCU_MODULE_NAND,
    //  add more component in the future
    IFX_RCU_MODULE_MAX,
};

#define IFX_RCU_DECLARE_MODULE_NAME(var)    \
    char *var[] = {                         \
        "USB",                              \
        "ETH",                              \
        "ATM",                              \
        "PTM",                              \
        "PPA",                              \
        "DMA",                              \
        "SDIO",                             \
        "MEI",                              \
        "TAPI",                             \
        "PCI",                              \
        "NAND",                             \
    }

typedef int (*ifx_rcu_callbackfn)(unsigned int reset_domain_id, unsigned int module_id, int f_after_reset, unsigned long arg);



/*
 * ####################################
 *                IOCTL
 * ####################################
 */

/*!
  \addtogroup IFX_RCU_STRUCT
 */
/*@{*/

/*!
  \struct ifx_rcu_ioctl_version
  \brief Structure used for query of driver version.
 */
struct ifx_rcu_ioctl_version {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver */
    unsigned int    minor;  /*!< output, minor number of driver */
};

/*!
  \struct ifx_rcu_ioctl_query_rst_domain
  \brief Structure used to get reset status of given hardware module.
 */
struct ifx_rcu_ioctl_query_rst_domain {
    unsigned int    domain_id;  /*!< input, hardware module ID */
    int             f_reset;    /*!< output, reset status */
};

/*@}*/


/*!
  \addtogroup IFX_RCU_IOCTL
 */
/*@{*/
#define IFX_RCU_IOC_MAGIC               0xe0
/*!
  \def IFX_RCU_IOC_VERSION
  \brief RCU IOCTL Command - Get driver version number.

   This command uses struct "ifx_rcu_ioctl_version" as parameter to RCU driver version number.
 */
#define IFX_RCU_IOC_VERSION             _IOR( IFX_RCU_IOC_MAGIC, 0, struct ifx_rcu_ioctl_version)
/*!
  \def IFX_RCU_IOC_QUERY_RST_DOMAIN
  \brief RCU IOCTL Command - Get reset status of given hardware module.

   This command uses struct "ifx_rcu_ioctl_query_rst_domain" as parameter to get reset status of given hardware module.
 */
#define IFX_RCU_IOC_QUERY_RST_DOMAIN    _IOWR(IFX_RCU_IOC_MAGIC, 1, struct ifx_rcu_ioctl_query_rst_domain)
/*@}*/



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
  /*
   *    RST_REQ/RST_STAT Register Access
   *    For accessing to special bits in these two registers, such as fusing, endianess
   */
  unsigned int ifx_rcu_rst_req_read(void);
  void ifx_rcu_rst_req_write(unsigned int value, unsigned int mask);
  unsigned int ifx_rcu_rst_stat_read(void);
  /*
   *    Reset Operation
   */
  int ifx_rcu_request(unsigned int reset_domain_id, unsigned int module_id, ifx_rcu_callbackfn callbackfn, unsigned long arg);
  int ifx_rcu_free(unsigned int reset_domain_id, unsigned int module_id);
  int ifx_rcu_stat_get(unsigned int reset_domain_id);
  int ifx_rcu_rst(unsigned int reset_domain_id, unsigned int module_id);
#endif



#endif  //  IFX_RCU_H
