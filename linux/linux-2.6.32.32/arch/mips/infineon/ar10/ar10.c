/******************************************************************************
**
** FILE NAME    : ar10.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for AR10 
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



#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kallsyms.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>

static inline void ifx_xbar_fpi_burst_disable(void)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_XBAR_ALWAYS_LAST);
    reg &= ~ IFX_XBAR_FPI_BURST_EN;
    IFX_REG_W32(reg, IFX_XBAR_ALWAYS_LAST);
}

/*
 *  Chip Specific Variable/Function
 */
void __init ifx_chip_setup(void)
{
    ifx_xbar_fpi_burst_disable();
}

/*
 *  Voice firmware decryt function pointer in Boot Rom
 */
const void (*ifx_bsp_basic_mps_decrypt)(unsigned int addr, int n) = (const void (*)(unsigned int, int))0xbfc01ea4;
EXPORT_SYMBOL(ifx_bsp_basic_mps_decrypt);

