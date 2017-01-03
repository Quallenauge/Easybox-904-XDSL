/******************************************************************************
**
** FILE NAME    : ar9.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for AR9
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

static char g_pkt_base[2048] __initdata __attribute__((__aligned__(32)));
static unsigned int g_desc_base[2] __initdata __attribute__((__aligned__(32)));
static void __init ifx_dplus_clean(void)
{
#define DMRX_PGCNT          ((volatile unsigned int *)0xbe191854)
#define DMRX_PKTCNT         ((volatile unsigned int *)0xbe191858)

    volatile unsigned int *desc_base = (volatile unsigned int *)KSEG1ADDR((unsigned int)g_desc_base);
    int i, j;

    IFX_REG_W32_MASK(1 << 18, 1 << 17, (volatile unsigned int *)IFX_SW_P0_CTL); //  force port 0 link down
    IFX_REG_W32_MASK(1 << 18, 1 << 17, (volatile unsigned int *)IFX_SW_P1_CTL); //  force port 1 link down

    if ( (IFX_REG_R32(DMRX_PGCNT) & 0x00FF) == 0 )
        return;

    IFX_REG_W32_MASK(1 << 17, 1 << 18, (volatile unsigned int *)IFX_SW_P2_CTL); //  force port 2 link up

    IFX_REG_W32(0, IFX_DMA_PS(0));
    IFX_REG_W32(0x1F68, IFX_DMA_PCTRL(0));
    IFX_REG_W32(0, IFX_DMA_IRNEN);  // disable all DMA interrupt

    for ( i = 0; i < 8; i += 2 ) {
        IFX_REG_W32(i, IFX_DMA_CS(0));
        IFX_REG_W32_MASK(0, 2, IFX_DMA_CCTRL(0));       //  reset channel
        while ( (IFX_REG_R32(IFX_DMA_CCTRL(0)) & 2) );  //  wait until reset finish
        IFX_REG_W32(1, IFX_DMA_CDLEN(0));               //  only 1 descriptor
        IFX_REG_W32(CPHYSADDR((unsigned int)desc_base), IFX_DMA_CDBA(0));       //  use local variable (array) as descriptor base address
        desc_base[0] = 0x80000000 | sizeof(g_pkt_base);
        desc_base[1] = CPHYSADDR((unsigned int)g_pkt_base);

        IFX_REG_W32_MASK(0, 1, IFX_DMA_CCTRL(0));       //  start receiving
        while ( 1 ) {
            for ( j = 0; j < 1000 && (desc_base[0] & 0x80000000) != 0; j++ );   //  assume packet can be finished within 1000 loops
            if ( (desc_base[0] & 0x80000000) != 0 )     //  no more packet
                break;
            desc_base[0] = 0x80000000 | sizeof(g_pkt_base);
        }
        IFX_REG_W32_MASK(1, 0, IFX_DMA_CCTRL(0));       //  stop receiving
    }

    IFX_REG_W32_MASK(3 << 17, 0, (volatile unsigned int *)IFX_SW_P0_CTL);       //  do not force port 0 link down
    IFX_REG_W32_MASK(3 << 17, 0, (volatile unsigned int *)IFX_SW_P1_CTL);       //  do not force port 1 link down
    IFX_REG_W32_MASK(3 << 17, 0, (volatile unsigned int *)IFX_SW_P2_CTL);       //  do not force port 2 link down

    if ( (IFX_REG_R32(DMRX_PGCNT) & 0x00FF) != 0 || (IFX_REG_R32(DMRX_PKTCNT) & 0x00FF) != 0 )
        prom_printf("%s error: IFX_REG_R32(DMRX_PGCNT) = 0x%08x, IFX_REG_R32(DMRX_PKTCNT) = 0x%08x\n", __func__, IFX_REG_R32(DMRX_PGCNT), IFX_REG_R32(DMRX_PKTCNT));
}

void __init ifx_chip_setup(void)
{
    ifx_dplus_clean();
    ifx_pmu_disable_all_modules();
}

const void (*ifx_bsp_basic_mps_decrypt)(unsigned int addr, int n) = (const void (*)(unsigned int, int))0xbf0017c4;
EXPORT_SYMBOL(ifx_bsp_basic_mps_decrypt);
