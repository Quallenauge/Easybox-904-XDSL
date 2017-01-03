/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_pmon_ar10.h
  \ingroup IFX_PMON
  \brief ifx pmon ar10 specific definition
*/
#ifndef IFXMIPS_PMON_AR10_H
#define IFXMIPS_PMON_AR10_H

#define IFX_PMON_PHY_BASE    0x1F800200
#define IFX_PMON_VIRT_BASE  (IFX_PMON_PHY_BASE | KSEG1)
#define IFX_PMON_SIZE        256

/* Quick access for specified TC and Counter 0/1*/
#define IFX_PMON_PCEESCR(TC, CNT)  ((volatile u32 *)(IFX_PMON_VIRT_BASE + ((TC) << 4) + ((CNT) << 3)))


#define IFX_MIPS_MAX_TC      IFX_MIPS_TC3

static inline void pmon_platform_init(void)
{
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC0, 0));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC0, 1));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC1, 0));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC1, 1));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC2, 0));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC2, 1));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC3, 0));
    IFX_REG_W32(0x0, IFX_PMON_PCEESCR(IFX_MIPS_TC3, 1));    
}

static inline void pmon_platform_event_set(int event, int tc, int counter)
{
    IFX_REG_W32(event, IFX_PMON_PCEESCR(tc, counter));
}

#endif /* IFXMIPS_PMON_AR10_H */

