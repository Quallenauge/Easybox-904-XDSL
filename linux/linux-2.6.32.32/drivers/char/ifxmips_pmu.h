/******************************************************************************
**
** FILE NAME    : ifxmips_pmu.h
** PROJECT      : IFX UEIP
** MODULES      : PMU
**
** DATE         : 28 May 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX Cross-Platform Power Management Unit driver header file
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
** 28 May 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/

/*!
  \file ifxmips_pmu.h
  \ingroup IFX_PMU
  \brief PMU driver internal header file
*/

#ifndef IFXMIPS_PMU_H
#define IFXMIPS_PMU_H

#include <asm/ifx/ifx_pmu.h>
#include "ifxmips_pmu_reg.h"

#define IFX_PMU_CLK_V1 1
#define IFX_PMU_CLK_V2 2

typedef struct ifx_pmu_dev{
    char name[16];
    int major;
    int minor;

}ifx_pmu_dev_t;

typedef struct ifx_pmu_clk_priv {
    u32    module_idx; /* Module index inside */
    u32    module; /* Real module bit */
    u32    refcnt;
    u8     name[16];
    u32    used;
}ifx_pmu_clk_priv_t;

typedef struct ifx_pmu {
    volatile u32 *pmu_pwdcr[3];
    volatile u32 *pmu_pwdcrb[3];
    volatile u32 *pmu_sr[3];
    u32      version; /* version leads to different register set */
    ifx_pmu_clk_priv_t module[40]; /* Subject to change */
} ifx_pmu_t;

#ifdef CONFIG_IFX_PMU_POWER_GATING
typedef struct ifx_pmu_pg_priv {
    ifx_pmu_pg_t pg;
    u32          real_domain;
    u8           domain_name[16];
    u32          refcnt;
    u32          used;
    
}ifx_pmu_pg_priv_t;
#endif /* CONFIG_IFX_PMU_POWER_GATING */

#if defined(CONFIG_DANUBE)
#include "ifxmips_pmu_danube.h"
#elif defined(CONFIG_AMAZON_SE)
#include "ifxmips_pmu_amazon_se.h"
#elif defined(CONFIG_AR9)
#include "ifxmips_pmu_ar9.h"
#elif defined(CONFIG_VR9)
#include "ifxmips_pmu_vr9.h"
#elif defined(CONFIG_AR10)
#include "ifxmips_pmu_ar10.h"
#else
#error "PMU Platform not specified!"
#endif

#define IFX_PMU_MAX_MODULE         (IFX_PMU_MODULE_ID_IDX_MAX + 1)

#endif /* IFXMIPS_PMU_H */

