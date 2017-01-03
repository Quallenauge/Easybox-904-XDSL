/******************************************************************************
**
** FILE NAME    : ifx_clk.h
** PROJECT      : IFX UEIP
** MODULES      : CGU
**
** DATE         : 28 May 2009
** AUTHOR       : Huang Xiaogang
** DESCRIPTION  : IFX Cross-Platform Clock Generation Unit driver header file
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
** 28 May 2009  Huang Xiaogang  The first UEIP release
*******************************************************************************/

#ifndef IFX_CLK_H
#define IFX_CLK_H

/*!
  \defgroup IFX_CGU UEIP Project - CGU driver module
  \brief UEIP Project - CGU driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_CGU_API APIs
  \ingroup IFX_CGU
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_CGU_IOCTL IOCTL Commands
  \ingroup IFX_CGU
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_CGU_STRUCT Structures
  \ingroup IFX_CGU
  \brief Structures used by user application.
 */

/*! \file ifx_clk.h
    \brief This file contains the interface of clock(cgu) driver.
*/

#define IFX_CGU_MAJOR                  246

#define IFX_CGU_VER_MAJOR              1
#define IFX_CGU_VER_MID                1
#define IFX_CGU_VER_MINOR              27

/*!
  \addtogroup IFX_CGU_STRUCT
 */
/*@{*/

/*!
  \struct ifx_cgu_ioctl_version
  \brief Structure used for query of driver version.
 */
struct ifx_cgu_ioctl_version {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver */
    unsigned int    minor;  /*!< output, minor number of driver */
};
/*@}*/

/*!
  \addtogroup IFX_CGU_IOCTL
 */
/*@{*/
#define IFX_CGU_IOC_MAGIC               'u'
/*!
  \def IFX_CGU_GET_CLOCK_RATES 
  \brief Get Clock rates
 */
#define IFX_CGU_GET_CLOCK_RATES         _IOR(IFX_CGU_IOC_MAGIC, 0, struct cgu_clock_rates)
/*!
  \def IFX_CGU_IOC_VERSION
  \brief Get CGU driver version
 */
#define IFX_CGU_IOC_VERSION            	_IOR(IFX_CGU_IOC_MAGIC, 1, struct ifx_cgu_ioctl_version)
#define CGU_IOC_MAXNR                   1
/*@}*/

#ifndef CONFIG_USE_EMULATOR
        #define PLL0_CLK_SPEED        1000000000
#endif

/*
 *  Data Type Used to Call ioctl(GET_CLOCK_RATES)
 */
struct cgu_clock_rates {
    u32     mips0;
    u32     mips1;
    u32     cpu;
    u32     io_region;
    u32     fpi_bus1;
    u32     fpi_bus2;
    u32     pp32;
    u32     pci;
    u32     mii0;
    u32     mii1;
    u32     usb;
    u32     clockout0;
    u32     clockout1;
    u32     clockout2;
    u32     clockout3;
};

#if defined(__KERNEL__)
    extern u32 cgu_get_cpu_clock(void);
    extern u32 cgu_get_io_region_clock(void);
#if defined(CONFIG_VR9)
    extern u32 ifx_get_ddr_hz(void);
#endif
#if defined(CONFIG_DANUBE) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
    extern u32 cgu_get_mips_clock(int);
    extern u32 cgu_get_fpi_bus_clock (int);
#else
    extern u32 cgu_get_mips_clock(void);
    extern u32 cgu_get_fpi_bus_clock(void);
#endif
    extern u32 cgu_get_pp32_clock(void);
    extern u32 cgu_get_qsb_clock(void);
    extern u32 cgu_get_pci_clock(void);
    extern u32 cgu_get_ethernet_clock(void);
    extern u32 cgu_get_usb_clock(void);
    extern u32 cgu_get_clockout(int);
#endif  //  defined(__KERNEL__)

#endif  //  IFX_CLK_H
