/******************************************************************************
**
** FILE NAME    : amazon_se.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for Amazon-SE
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
#include <asm/ifx/ifx_pmu.h>

/*
 *  Chip Specific Variable/Function
 */

#define IFX_BIU_B2S  ((volatile u32*)(IFX_WDT + 0x0300))

void __init ifx_chip_setup(void)
{
    //  This configuration is to solve following issue
    //    EBU and ARC use the same bus, if EBU is accessing NOR flash (for long time)
    //    and can not yield the bus to ARC, ARC will drop the link.
    //    This issue can only be observed while NOR flash is being accessed heavily,
    //    e.g. Web browsing.
    //printk("Configuring MC_PRIO for ASE NOR flash\n");
    *IFX_BIU_B2S &= ~(1 << 1);
    *IFX_MC_PRIO &= ~(3 << IFX_MC_PRIO_AHB_SHIFT);
    *IFX_MC_PRIO |= (IFX_MC_PRIO_3 << IFX_MC_PRIO_AHB_SHIFT);

    ifx_pmu_disable_all_modules();
#ifdef CONFIG_MTD_IFX_NOR
    EBU_PMU_SETUP(IFX_PMU_ENABLE);
#endif
}

