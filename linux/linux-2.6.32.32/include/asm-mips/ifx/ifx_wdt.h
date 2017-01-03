/******************************************************************************
**
** FILE NAME    : ifx_wdt.h
** PROJECT      : IFX UEIP
** MODULES      : WDT
**
** DATE         : 12 Aug 2009
** AUTHOR       : Lee Yao Chye
** DESCRIPTION  : Watchdog Timer
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
** HISTORY
** $Date        $Author         $Comment
** 12 Aug 2009  Lee Yao Chye    Initial UEIP version
** 19 Oct 2009  Lee Yao Chye    Add Linux style ioctls
*******************************************************************************/
#ifndef IFX_WDT_H
#define IFX_WDT_H

/*!
  \defgroup IFX_WDT_IOCTL IOCTL Commands
  \ingroup IFX_WDT
  \brief IOCTL Commands used by user application.
 */


#include <linux/watchdog.h>

/*!
  \addtogroup IFX_WDT_IOCTL
 */
/*@{*/

/* wdt ioctl control */
#define IFX_WDT_IOC_MAGIC             0xc0

/*!
  \def IFX_WDT_IOC_START
  \brief wdt IOCTL Command - similar to WDIOC_SETTIMEOUT

   This command is used to configure the watchdog timeout and start the watchdog.
 */
#define IFX_WDT_IOC_START            _IOW(IFX_WDT_IOC_MAGIC, 0, int)

/*!
  \def IFX_WDT_IOC_STOP
  \brief wdt IOCTL Command - stop the watchdog

   This command is used to stop the watchdog.
 */
#define IFX_WDT_IOC_STOP             _IO(IFX_WDT_IOC_MAGIC, 1)

/*!
  \def IFX_WDT_IOC_PING
  \brief wdt IOCTL Command - similar to WDIOC_KEEPALIVE

   This command is used to keep the watchdog alive.
 */
#define IFX_WDT_IOC_PING             _IO(IFX_WDT_IOC_MAGIC, 2)

/*!
  \def IFX_WDT_IOC_SET_PWL
  \brief wdt IOCTL Command - set prewarning level

   This command is used to configure the prewarning level.
       0 for 1/2 of the max WDT period
       1 for 1/4 of the max WDT period
       2 for 1/8 of the max WDT period
       3 for 1/16 of the max WDT period
 */
#define IFX_WDT_IOC_SET_PWL          _IOW(IFX_WDT_IOC_MAGIC, 3, int)

/*!
  \def IFX_WDT_IOC_SET_DSEN
  \brief wdt IOCTL Command - configure debug suspend

   This command is used to configure debug suspend.
 */
#define IFX_WDT_IOC_SET_DSEN         _IOW(IFX_WDT_IOC_MAGIC, 4, int)

/*!
  \def IFX_WDT_IOC_SET_LPEN
  \brief wdt IOCTL Command - configure low power clock freeze mode

   This command is used to configure low power clock freeze mode.
 */
#define IFX_WDT_IOC_SET_LPEN         _IOW(IFX_WDT_IOC_MAGIC, 5, int)

/*!
  \def IFX_WDT_IOC_GET_STATUS
  \brief wdt IOCTL Command - reads the watchdog status register

   This command is used to read the watchdog status register.
 */
#define IFX_WDT_IOC_GET_STATUS       _IOR(IFX_WDT_IOC_MAGIC, 6, int)

/*!
  \def IFX_WDT_IOC_SET_CLKDIV
  \brief wdt IOCTL Command - configures the watchdog clock divider

   This command is used to configure the watchdog clock divider.
       0 for CLK_WDT = 1 x CLK_TIMER
       1 for CLK_WDT = 64 x CLK_TIMER
       2 for CLK_WDT = 4096 x CLK_TIMER
       3 for CLK_WDT = 262144 x CLK_TIMER        
 */
#define IFX_WDT_IOC_SET_CLKDIV       _IOW(IFX_WDT_IOC_MAGIC, 7, int) 

/*@}*/


#endif /* IFX_WDT_H */
