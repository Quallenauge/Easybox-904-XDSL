/******************************************************************************
**
** FILE NAME    : vr9.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for VR9
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
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/common_routines.h>

/*
 *  Chip Specific Variable/Function
 */

static char g_pkt_base[2048] __initdata __attribute__((__aligned__(32)));
static unsigned int g_desc_base[2] __initdata __attribute__((__aligned__(32)));
static void __init ifx_dplus_clean(void)
{
#define DMRX_CFG                        ((volatile unsigned int *)0xBE235850)
#define DMRX_PGCNT                      ((volatile unsigned int *)0xBE235854)
#define DMRX_PKTCNT                     ((volatile unsigned int *)0xBE235858)
#define DSRX_CFG                        ((volatile unsigned int *)0xBE235C48)
#define DSRX_PGCNT                      ((volatile unsigned int *)0xBE235C4C)

#define VR9_SWIP_MACRO                  KSEG1ADDR(0x1E108000)
#define VR9_SWIP_MACRO_REG(off)         ((volatile u32*)(VR9_SWIP_MACRO + (off) * 4))
#define VR9_SWIP_TOP                    (VR9_SWIP_MACRO | (0x0C40 * 4))
#define VR9_SWIP_TOP_REG(off)           ((volatile u32*)(VR9_SWIP_TOP + (off) * 4))
#define PCE_PCTRL_REG(port, reg)        VR9_SWIP_MACRO_REG(0x480 + (port) * 0xA + (reg))    //  port < 12, reg < 4
#define FDMA_PCTRL_REG(port)            VR9_SWIP_MACRO_REG(0xA80 + (port) * 6)  //  port < 7
#define SDMA_PCTRL_REG(port)            VR9_SWIP_MACRO_REG(0xBC0 + (port) * 6)  //  port < 7

    volatile unsigned int *desc_base = (volatile unsigned int *)KSEG1ADDR((unsigned int)g_desc_base);
    int i, j, k;

    for ( i = 0; i < 6; i++ )
        IFX_REG_W32_MASK(1, 0, SDMA_PCTRL_REG(i));  //  stop port 0 - 5

    if ( (IFX_REG_R32(DMRX_PGCNT) & 0x0FFF) == 0 && (IFX_REG_R32(DSRX_PGCNT) & 0x0FFF) == 0 )
        return;

    IFX_REG_W32(0, IFX_DMA_PS(0));
    IFX_REG_W32(0x1F68, IFX_DMA_PCTRL(0));
    IFX_REG_W32(0, IFX_DMA_IRNEN);  // disable all DMA interrupt

    for ( k = 0; k < 8; k++ ) {
        unsigned int imap[8] = {0, 2, 4, 6, 20, 21, 22, 23};

        i = imap[k];
        IFX_REG_W32(i, IFX_DMA_CS(0));
        IFX_REG_W32_MASK(0, 2, IFX_DMA_CCTRL(0));       //  reset channel
        while ( (IFX_REG_R32(IFX_DMA_CCTRL(0)) & 2) );  //  wait until reset finish
        IFX_REG_W32(0, IFX_DMA_CIE(0));                 //  disable channel interrupt
        IFX_REG_W32(1, IFX_DMA_CDLEN(0));               //  only 1 descriptor
        IFX_REG_W32(CPHYSADDR((unsigned int)desc_base), IFX_DMA_CDBA(0));       //  use local variable (array) as descriptor base address
        desc_base[0] = 0x80000000 | sizeof(g_pkt_base);
        desc_base[1] = CPHYSADDR((unsigned int)g_pkt_base);

        IFX_REG_W32_MASK(0, 1, IFX_DMA_CCTRL(0));       //  start receiving
        while ( 1 ) {
            for ( j = 0; j < 10000 && (desc_base[0] & 0x80000000) != 0; j++ );  //  assume packet can be finished within 10000 loops
            if ( (desc_base[0] & 0x80000000) != 0 )     //  no more packet
                break;
            desc_base[0] = 0x80000000 | sizeof(g_pkt_base);
        }
        IFX_REG_W32_MASK(1, 0, IFX_DMA_CCTRL(0));       //  stop receiving
    }

    if ( (IFX_REG_R32(DMRX_PGCNT) & 0x0FFF) != 0 || (IFX_REG_R32(DMRX_PKTCNT) & 0x0FFF) != 0 || (IFX_REG_R32(DSRX_PGCNT) & 0x0FFF) != 0 )
        prom_printf("%s error: IFX_REG_R32(DMRX_PGCNT) = 0x%08x, IFX_REG_R32(DMRX_PKTCNT) = 0x%08x, IFX_REG_R32(DSRX_PGCNT) = 0x%08x\n", __func__, IFX_REG_R32(DMRX_PGCNT), IFX_REG_R32(DMRX_PKTCNT), IFX_REG_R32(DSRX_PGCNT));
}

static inline void __init ifx_xbar_fpi_burst_disable(void)
{
    u32 reg;

    /* bit 1 as 1 --burst; bit 1 as 0 -- single */
    reg = IFX_REG_R32(IFX_XBAR_ALWAYS_LAST);
    reg &= ~ IFX_XBAR_FPI_BURST_EN;
    IFX_REG_W32(reg, IFX_XBAR_ALWAYS_LAST);
}

static inline void __init ifx_xbar_ahb_slave_big_endian(void)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_RCU_AHB_ENDIAN);
    reg |= IFX_RCU_AHB_BE_XBAR_S;
    IFX_REG_W32(reg, IFX_RCU_AHB_ENDIAN);
}


#define IFX_PMU_USIF_ECO_FIX                  ((volatile u32*)(0xBF10220C))

static inline void  __init ifx_pmu_usif_eco_fix(void)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_PMU_USIF_ECO_FIX);
    reg |= 0x40000000;
    IFX_REG_W32(reg, IFX_PMU_USIF_ECO_FIX);
}

static void ifx_mps_decrypt_addr_detect(void)
{
    if (  ifx_is_vr9_a21_chip() )
        ifx_bsp_basic_mps_decrypt = (const void (*)(unsigned int, int))0xbf001f38;
}

/*
 *  Chip Specific Variable/Function
 */
void __init ifx_chip_setup(void)
{
    ifx_xbar_fpi_burst_disable();
    ifx_xbar_ahb_slave_big_endian();
    ifx_pmu_usif_eco_fix();
    ifx_dplus_clean();
    ifx_pmu_disable_all_modules();
    //ifx_pmu_pg_disable_all_domains();
    ifx_mps_decrypt_addr_detect();
}

int ifx_is_vr9_a21_chip(void)
{
    int ret = 0;
    ifx_chipid_t chipid;
    
    ifx_get_chipid(&chipid);
    if (((chipid.family_id == IFX_FAMILY_xRX200) && 
        (chipid.family_ver == IFX_FAMILY_xRX200_A2x))
        || chipid.family_id == IFX_FAMILY_UNKNOWN) {
        ret = 1;
    }
    return ret;
}

/*
 *  Voice firmware decryt function pointer in Boot Rom
 */
const void (*ifx_bsp_basic_mps_decrypt)(unsigned int addr, int n) = (const void (*)(unsigned int, int))0xbf001ea4;
EXPORT_SYMBOL(ifx_bsp_basic_mps_decrypt);
