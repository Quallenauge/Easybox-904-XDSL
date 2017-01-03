/******************************************************************************
**
** FILE NAME    : ifxmips_pmu_vr9.h
** PROJECT      : IFX UEIP
** MODULES      : PMU
**
** DATE         : 02 Sep 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX PMU vr9 specific header file
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
** 02 Sep 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/
/*!
  \file ifxmips_pmu_vr9.h
  \ingroup IFX_PMU
  \brief PMU driver VRX specific file
*/
#ifndef IFXMIPS_PMU_VR9_H
#define IFXMIPS_PMU_VR9_H

#define IFX_PMU_MODULE_ID_IDX_MIN  IFX_PMU_MODULE_USB0_PHY
#define IFX_PMU_MODULE_ID_IDX_MAX  IFX_PMU_MODULE_DDR_CKE

static ifx_pmu_t g_ifx_pmu = {
    .pmu_pwdcr[0]  = IFX_PMU_PWDCR,
    .pmu_pwdcr[1]  = IFX_PMU_PWDCR1,
    .pmu_pwdcr[2]  = NULL,
    .pmu_pwdcrb[0] = NULL,
    .pmu_pwdcrb[1] = NULL,
    .pmu_pwdcrb[2] = NULL,    
    .pmu_sr[0]     = IFX_PMU_SR,
    .pmu_sr[1]     = IFX_PMU_SR1,
    .pmu_sr[2]     = NULL,
    .version       = IFX_PMU_CLK_V1,
    .module        = {
                    {IFX_PMU_MODULE_USB0_PHY, IFX_PMU_MODULE_USB0_PHY, 0, "USB0-PHY", 1},
                    {IFX_PMU_MODULE_FPIS, IFX_PMU_MODULE_FPIS, 0, "FPIS",     0}, /* Always on */
                    {IFX_PMU_MODULE_DFEV0, IFX_PMU_MODULE_DFEV0, 0, "DFEV0",    1},
                    {IFX_PMU_MODULE_DFEV1, IFX_PMU_MODULE_DFEV1, 0, "DFEV1",    1},
                    {IFX_PMU_MODULE_PCI, IFX_PMU_MODULE_PCI, 0, "PCI",      0}, /* Not used in VRX */
                    {IFX_PMU_MODULE_DMA, IFX_PMU_MODULE_DMA, 0, "DMA",      1},
                    {IFX_PMU_MODULE_USB0_CTRL, IFX_PMU_MODULE_USB0_CTRL, 0, "USB0-CTRL",1},
                    {IFX_PMU_MODULE_USIF, IFX_PMU_MODULE_USIF, 0, "USIF",     1},
                    {IFX_PMU_MODULE_SPI, IFX_PMU_MODULE_SPI, 0 , "SPI",      1},
                    {IFX_PMU_MODULE_DSL_DFE, IFX_PMU_MODULE_DSL_DFE, 0, "DSL-DFE",  1},
                    {IFX_PMU_MODULE_EBU, IFX_PMU_MODULE_EBU, 0, "EBU",      0},
                    {IFX_PMU_MODULE_LEDC, IFX_PMU_MODULE_LEDC, 0, "LEDC",     1},
                    {IFX_PMU_MODULE_GPTC, IFX_PMU_MODULE_GPTC, 0, "GPTC",     1},
                    {IFX_PMU_MODULE_AHBS, IFX_PMU_MODULE_AHBS, 0, "AHBS",     0}, /* Final chip hardcode not used */
                    {IFX_PMU_MODULE_FPIM, IFX_PMU_MODULE_FPIM, 0, "FPIM",     0}, /* Always on */
                    {IFX_PMU_MODULE_AHBM, IFX_PMU_MODULE_AHBM, 0, "AHBM",     1},
                    {IFX_PMU_MODULE_SDIO, IFX_PMU_MODULE_SDIO, 0, "SDIO",     1},
                    {IFX_PMU_MODULE_UART1, IFX_PMU_MODULE_UART1, 0, "UART1",    0}, /* Keep console on */  
                    {IFX_PMU_MODULE_PPE_QSB, IFX_PMU_MODULE_PPE_QSB, 0, "PPE-QSB",  1},
                    {IFX_PMU_MODULE_PPE_SLL01, IFX_PMU_MODULE_PPE_SLL01, 0, "PPE-SSL01",1},
                    {IFX_PMU_MODULE_DEU, IFX_PMU_MODULE_DEU, 0, "DEU",      1},
                    {IFX_PMU_MODULE_PPE_TC, IFX_PMU_MODULE_PPE_TC, 0, "PPE-TC",   1},
                    {IFX_PMU_MODULE_PPE_EMA, IFX_PMU_MODULE_PPE_EMA, 0, "PPE-EMA",  1},
                    {IFX_PMU_MODULE_PPE_DPLUSM, IFX_PMU_MODULE_PPE_DPLUSM, 0, "PPE-DPLUSM",1},
                    {IFX_PMU_MODULE_PPE_DPLUSS, IFX_PMU_MODULE_PPE_DPLUSS, 0, "PPE-DPLUSS",1},
                    {IFX_PMU_MODULE_TDM, IFX_PMU_MODULE_TDM, 0, "TDM",      1},
                    {IFX_PMU_MODULE_USB1_PHY, IFX_PMU_MODULE_USB1_PHY, 0, "USB1-PHY", 1},
                    {IFX_PMU_MODULE_USB1_CTRL, IFX_PMU_MODULE_USB1_CTRL, 0, "USB1-CTRL",1},
                    {IFX_PMU_MODULE_SWITCH, IFX_PMU_MODULE_SWITCH, 0, "SWITCH",   1},
                    {IFX_PMU_MODULE_PPE_TOP, IFX_PMU_MODULE_PPE_TOP, 0, "PPE-TOP",  1},
                    {IFX_PMU_MODULE_GPHY, IFX_PMU_MODULE_GPHY, 0, "GPHY",     1},
                    {IFX_PMU_MODULE_PCIE_L0_CLK, IFX_PMU_MODULE_PCIE_L0_CLK, 0, "PCIE_L0_CLK",1},
                    {IFX_PMU_MODULE_PCIE_PHY, IFX_PMU_MODULE_PCIE_PHY, 0, "PCIE-PHY", 1},
                    {IFX_PMU_MODULE_PCIE_CTRL, IFX_PMU_MODULE_PCIE_CTRL, 0, "PCIE-CTRL",1},
                    {-1, -1, 0, "ARC",         0},
                    {-1, -1, 0, "HSNAND",      0},
                    {IFX_PMU_MODULE_PDI, IFX_PMU_MODULE_PDI, 0, "PDI",      1},
                    {IFX_PMU_MODULE_MSI, IFX_PMU_MODULE_MSI, 0, "MSI",      1},
                    {IFX_PMU_MODULE_DDR_CKE, IFX_PMU_MODULE_DDR_CKE, 0, "DDR_CKE",  0},
                    {-1, -1, 0, "",         0},
                    },
};

#ifdef CONFIG_IFX_PMU_POWER_GATING
static ifx_pmu_pg_priv_t g_ifx_pmu_pg[] = {
    {
        .pg = {IFX_POWER_DOMAIN(USB), 0},
        .real_domain = 0,
        .domain_name = "USB",
        .refcnt      = 0,
        .used        = 1,
    },
    {
        .pg = {IFX_POWER_DOMAIN(PCIE), 0},
        .real_domain = 1,
        .domain_name = "PCIE",
        .refcnt      = 0,
        .used        = 0,
    },
    {
        .pg = {IFX_POWER_DOMAIN(SLIC_TDM), 0},
        .real_domain = 2,
        .domain_name = "SLIC_TDM",
        .refcnt      = 0,
        .used        = 1,
    },
    {
        .pg = {IFX_POWER_DOMAIN(DEU), 0},
        .real_domain = 3,
        .domain_name = "DEU",
        .refcnt      = 0,
        .used        = 0,
    },
    {
        .pg = {IFX_POWER_DOMAIN(FPI_TOP), 0},
        .real_domain = 4,
        .domain_name = "FPI_TOP",
        .refcnt      = 0,
        .used        = 0,
    },
    {    
        .pg = {IFX_POWER_DOMAIN(PPE), 0},
        .real_domain = 5,
        .domain_name = "PPE",
        .refcnt      = 0,
        .used        = 1,
    },
    {
        .pg = {IFX_POWER_DOMAIN(SWITCH), 0},
        .real_domain = 8,
        .domain_name = "SWITCH",
        .refcnt      = 0,
        .used        = 0,
    },
    {
        .pg = {IFX_POWER_DOMAIN(MIPS), 0},
        .real_domain = 9,
        .domain_name = "MIPS",
        .refcnt      = 0,
        .used        = 1,
    },
    {
        .pg = {IFX_POWER_DOMAIN(DSL_DFE), 0},
        .real_domain = 11,
        .domain_name = "DSL_DFE",
        .refcnt      = 0,
        .used        = 1,
    },
};

static inline int ifx_pmu_power_domain_valid(u32 domain_idx)
{
    if (domain_idx == IFX_POWER_DOMAIN(USB) || domain_idx == IFX_POWER_DOMAIN(PCIE) ||
        domain_idx == IFX_POWER_DOMAIN(SLIC_TDM) || domain_idx == IFX_POWER_DOMAIN(DEU) ||
        domain_idx == IFX_POWER_DOMAIN(FPI_TOP) || domain_idx == IFX_POWER_DOMAIN(PPE) ||
        domain_idx == IFX_POWER_DOMAIN(SWITCH) || domain_idx == IFX_POWER_DOMAIN(MIPS) ||
        domain_idx == IFX_POWER_DOMAIN(DSL_DFE)) {
        if (g_ifx_pmu_pg[domain_idx].used) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        return 0;
    }
}
#endif /* CONFIG_IFX_PMU_POWER_GATING */
#endif /* IFXMIPS_PMU_VR9_H */

