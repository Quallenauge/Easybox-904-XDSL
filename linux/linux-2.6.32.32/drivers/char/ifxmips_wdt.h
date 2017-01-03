/******************************************************************************
**
** FILE NAME    : ifxmips_wdt.h
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
*******************************************************************************/
#ifndef IFXMIPS_WDT_H
#define IFXMIPS_WDT_H

#define IFX_WDT_REG32(addr)           (*((volatile u32*)(addr)))

/* Define for device driver code */
#define DEVICE_NAME "ifx_wdt"

#define IFX_WDT_PW1 0x000000BE /**< First password for access */
#define IFX_WDT_PW2 0x000000DC /**< Second password for access */

#define IFX_WDT_CLKDIV0_VAL 1
#define IFX_WDT_CLKDIV1_VAL 64
#define IFX_WDT_CLKDIV2_VAL 4096
#define IFX_WDT_CLKDIV3_VAL 262144
#define IFX_WDT_CLKDIV0 0
#define IFX_WDT_CLKDIV1 1
#define IFX_WDT_CLKDIV2 2
#define IFX_WDT_CLKDIV3 3

#define IFX_WDT_READ_TIMER_VAL_MASK	(0x0000FFFF)	

#endif /* IFXMIPS_WDT_H */
