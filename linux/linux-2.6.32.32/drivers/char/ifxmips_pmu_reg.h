/******************************************************************************
**
** FILE NAME    : ifxmips_pmu_reg.h
** PROJECT      : IFX UEIP
** MODULES      : IFX Power Management Unit
**
** DATE         : 30 Mar 2010
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : Power Management Unit Register defintion
** COPYRIGHT    :       Copyright (c) 2010
**                      Lantiq Deutschland GmbH
**                      Am Campeon 3, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 30 Mar,2010   Lei Chuanhua    First UEIP release
*******************************************************************************/
/*!
  \file ifxmips_pmu_reg.h
  \ingroup IFX_PMU
  \brief ifx pmu driver register definition
*/

#ifndef IFXMIPS_PMU_REG_H
#define IFXMIPS_PMU_REG_H

/***********************************************************************/
/*  Module      :  PMU register address and bits                       */
/***********************************************************************/

#define IFX_PMU                                 (KSEG1 | 0x1F102000)

#ifndef CONFIG_AR10
/* PMU Clock Gating Control Register */
#define IFX_PMU_PWDCR                           ((volatile u32*)(IFX_PMU + 0x001C))
#define IFX_PMU_SR                              ((volatile u32*)(IFX_PMU + 0x0020))

/* The following is new for VR9 and future platform */
#define IFX_PMU_PWDCR1                          ((volatile u32*)(IFX_PMU + 0x0024))
#define IFX_PMU_SR1                             ((volatile u32*)(IFX_PMU + 0x0028))
#else

/* PMU Clock Gating Control Register */
#define IFX_PMU_CLKGSR1                         ((volatile u32*)(IFX_PMU + 0x0020))
#define IFX_PMU_CLKGCR1_A                       ((volatile u32*)(IFX_PMU + 0x0024))
#define IFX_PMU_CLKGCR1_B                       ((volatile u32*)(IFX_PMU + 0x0028))
#define IFX_PMU_CLKGSR2                         ((volatile u32*)(IFX_PMU + 0x0030))
#define IFX_PMU_CLKGCR2_A                       ((volatile u32*)(IFX_PMU + 0x0034))
#define IFX_PMU_CLKGCR2_B                       ((volatile u32*)(IFX_PMU + 0x0038))

/* PMU Clock Gating Control Register */
#define IFX_PMU_ANALOG_SR                       ((volatile u32*)(IFX_PMU + 0x0040))
#define IFX_PMU_ANALOGCR_A                      ((volatile u32*)(IFX_PMU + 0x0044))
#define IFX_PMU_ANALOGCR_B                      ((volatile u32*)(IFX_PMU + 0x0048))
#endif /* CONFIG_AR10 */
#ifdef CONFIG_IFX_PMU_POWER_GATING
/* PMU Power Gating Register */

/* Power Down Control and Status Register */
#define IFX_PMU_PWCSR                           ((volatile u32*)(IFX_PMU + 0x0100))
#define IFX_PMU_PWCSR_POWER_DOMAIN              0x0000000F
#define IFX_PMU_PWCSR_POWER_DOMAIN_S            0
#define IFX_PMU_PWCSR_POWER_OFF                 0x00000010
#define IFX_PMU_PWCSR_SLOW_CLK_MODE             0x00000020
#define IFX_PMU_PWCSR_BYPASS_MODE               0x01000000
#define IFX_PMU_PWCSR_BYPASS_CMD                0xFE000000
#define IFX_PMU_PWCSR_BYPASS_CMD_S              25

/* Reset Delay Register, HW Debug only */
#define IFX_PMU_RD                              ((volatile u32*)(IFX_PMU + 0x0104))

/* Power On Delay Register, HW Debug only */
#define IFX_PMU_POD                             ((volatile u32*)(IFX_PMU + 0x0108))

/* Power Domain Selection Register, for checking status purpose */
#define IFX_PMU_PD_SEL                          ((volatile u32*)(IFX_PMU + 0x010C))
#define IFX_PMU_PD_SEL_PD                       0x0000000F
#define IFX_PMU_PD_SEL_PD_S                     0

/* Wakeup Routing Location Register */
#define IFX_PMU_WKUP_LOC                        ((volatile u32*)(IFX_PMU + 0x0200))

/* Power down and Wakeup Source Gating Register */
#define IFX_PMU_WKUP_GT                         ((volatile u32*)(IFX_PMU + 0x0204))
#define IFX_PMU_WKUP_GT_SI_SLEEP_BLOCK          0x00000001
#define IFX_PMU_WKUP_GT_ICU0_BLOCK              0x00000002
#define IFX_PMU_WKUP_GT_ICU1_BLOCK              0x00000004
#define IFX_PMU_WKUP_GT_SWITCH_BLOCK            0x00000008
#define IFX_PMU_WKUP_GT_FXS_OFF_HOOK_BLOCK      0x00000010
#define IFX_PMU_WKUP_GT_DSL_CPU_BLOCK           0x00000020
#define IFX_PMU_WKUP_GT_DSL_PPE_BLOCK           0x00000040
#define IFX_PMU_WKUP_GT_CPU_INT_BLOCK           0x00000080

#define IFX_PMU_WKUP_GT_BLOCK_ALL               0x000000FF
#define IFX_PMU_WKUP_GT_BLOCK_WKUP              0x000000FE

#endif /* CONFIG_IFX_PMU_POWER_GATING */

#endif /* IFXMIPS_PMU_REG_H */
