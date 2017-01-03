/******************************************************************************
**
** FILE NAME    : emulation.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Lei Chuan Hua
** DESCRIPTION  : header file for AR10 
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
** 27 May 2009   Lei Chuan Hua   The first UEIP release
*******************************************************************************/



#ifndef EMULATION_H
#define EMULATION_H

#ifdef CONFIG_USE_EMULATOR

#ifdef CONFIG_USE_VENUS
    #define EMULATOR_CPU_SPEED    4000000 //3333333
    #define PLL0_CLK_SPEED        2500000
#elif defined(CONFIG_USE_PALLADIUM)
    #define EMULATOR_CPU_SPEED    120000
    #define PLL0_CLK_SPEED        120000
#else
    #define EMULATOR_CPU_SPEED    25000
    #define PLL0_CLK_SPEED        25000
#endif  /* CONFIG_USE_VENUS */
#else  /* Real chip */
    #define PLL0_CLK_SPEED        1000000000
#endif /* CONFIG_USE_EMULATOR */
#endif /* */
 /* EMULATION_H */

