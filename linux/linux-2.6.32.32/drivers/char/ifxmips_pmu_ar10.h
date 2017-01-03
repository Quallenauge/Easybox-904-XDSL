/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_pmu_ar10.h
  \ingroup IFX_PMU
  \brief PMU driver AR10 specific file
*/
#ifndef IFXMIPS_PMU_AR10_H
#define IFXMIPS_PMU_AR10_H

#define IFX_PMU_MODULE_ID_IDX_MIN  IFX_PMU_MODULE_USB0_PHY
#define IFX_PMU_MODULE_ID_IDX_MAX  IFX_PMU_MODULE_MSI1

static ifx_pmu_t g_ifx_pmu = {
    .pmu_pwdcr[0]  = IFX_PMU_CLKGCR1_A,
    .pmu_pwdcr[1]  = IFX_PMU_CLKGCR2_A,
    .pmu_pwdcr[2]  = IFX_PMU_ANALOGCR_A,
    .pmu_pwdcrb[0] = IFX_PMU_CLKGCR1_B,
    .pmu_pwdcrb[1] = IFX_PMU_CLKGCR2_B,
    .pmu_pwdcrb[2] = IFX_PMU_ANALOGCR_B,
    .pmu_sr[0]     = IFX_PMU_CLKGSR1,
    .pmu_sr[1]     = IFX_PMU_CLKGSR2,
    .pmu_sr[2]     = IFX_PMU_ANALOG_SR,
    .version       = IFX_PMU_CLK_V1,
    .module        = {
                    {IFX_PMU_MODULE_USB0_PHY, 64, 0, "USB0-PHY", 1},
                    {-1, -1, 0, "FPIS",     0}, /* N.A */
                    {IFX_PMU_MODULE_DFEV0, IFX_PMU_MODULE_DFEV0, 0, "DFEV0",    1},
                    {IFX_PMU_MODULE_DFEV1, IFX_PMU_MODULE_DFEV1, 0, "DFEV1",    1},
                    {-1, -1, 0, "PCI",      0}, /* N.A */
                    {IFX_PMU_MODULE_DMA, IFX_PMU_MODULE_DMA, 0, "DMA",      1},
                    {IFX_PMU_MODULE_USB0_CTRL, IFX_PMU_MODULE_USB0_CTRL, 0, "USB0-CTRL",1},
                    {IFX_PMU_MODULE_USIF, IFX_PMU_MODULE_USIF, 0, "USIF",     1},
                    {IFX_PMU_MODULE_SPI, IFX_PMU_MODULE_SPI, 0 , "SPI",      1},
                    {IFX_PMU_MODULE_DSL_DFE, IFX_PMU_MODULE_DSL_DFE, 0, "DSL-DFE",  1},
                    {IFX_PMU_MODULE_EBU, IFX_PMU_MODULE_EBU, 0, "EBU",      1},
                    {IFX_PMU_MODULE_LEDC, IFX_PMU_MODULE_LEDC, 0, "LEDC",     1},
                    {IFX_PMU_MODULE_GPTC, IFX_PMU_MODULE_GPTC, 0, "GPTC",     1},
                    {IFX_PMU_MODULE_PCIE1_PHY, 73, 0, "PCIE1-PHY",     1},
                    {IFX_PMU_MODULE_ADSL_AFE, 80, 0, "ADSL-AFE",     1},
                    {IFX_PMU_MODULE_DCDC_2V5, 81, 0, "DCDc-2V5",     1},
                    {-1, -1, 0, "SDIO",     0},
                    {IFX_PMU_MODULE_UART1, IFX_PMU_MODULE_UART1, 0, "UART1",    0}, /* Keep console on */  
                    {IFX_PMU_MODULE_DCDC_1VX, 82, 0, "DCDC-1VX",  1},
                    {IFX_PMU_MODULE_DCDC_1V0, 83, 0, "DCDC-1V0",1},
                    {IFX_PMU_MODULE_DEU, IFX_PMU_MODULE_DEU, 0, "DEU",      1},
                    {IFX_PMU_MODULE_PPE_TC, IFX_PMU_MODULE_PPE_TC, 0, "PPE-TC",   1},
                    {IFX_PMU_MODULE_PPE_EMA, IFX_PMU_MODULE_PPE_EMA, 0, "PPE-EMA",  1},
                    {IFX_PMU_MODULE_PPE_DPLUS, IFX_PMU_MODULE_PPE_DPLUS, 0, "PPE-DPLUS",1},
                    {-1, -1, 0, "PPE-DPLUSS", 0},
                    {IFX_PMU_MODULE_TDM, IFX_PMU_MODULE_TDM, 0, "TDM",      1},
                    {IFX_PMU_MODULE_USB1_PHY, 65, 0, "USB1-PHY", 1},
                    {IFX_PMU_MODULE_USB1_CTRL, IFX_PMU_MODULE_USB1_CTRL, 0, "USB1-CTRL",1},
                    {IFX_PMU_MODULE_SWITCH, IFX_PMU_MODULE_SWITCH, 0, "SWITCH",   1},
                    {IFX_PMU_MODULE_GPHY0, IFX_PMU_MODULE_GPHY0, 0, "GPHY0",  1},
                    {IFX_PMU_MODULE_GPHY1, IFX_PMU_MODULE_GPHY1, 0, "GPHY1",  1},
                    {IFX_PMU_MODULE_GPHY2, IFX_PMU_MODULE_GPHY2, 0, "GPHY2",  1},
                    {IFX_PMU_MODULE_PCIE0_PHY, 72, 0, "PCIE0-PHY", 1},
                    {IFX_PMU_MODULE_PCIE0_CTRL, IFX_PMU_MODULE_PCIE0_CTRL, 0, "PCIE0-CTRL",1},
                    {IFX_PMU_MODULE_PCIE1_CTRL, 49, 0, "PCIE1-CTL",         1},
                    {IFX_PMU_MODULE_PDI1, 52, 0, "PDI1",      1},
                    {IFX_PMU_MODULE_PDI0, IFX_PMU_MODULE_PDI0, 0, "PDI0",      1},
                    {IFX_PMU_MODULE_MSI0, IFX_PMU_MODULE_MSI0, 0, "MSI0",      1},
                    {IFX_PMU_MODULE_DDR_CKE, IFX_PMU_MODULE_DDR_CKE, 0, "DDR_CKE",  0},
                    {IFX_PMU_MODULE_MSI1, 53, 0, "MSI1",         1},
                    },
};
#ifdef CONFIG_IFX_PMU_POWER_GATING
static ifx_pmu_pg_priv_t g_ifx_pmu_pg[] = {
    {
        .pg = {IFX_POWER_DOMAIN(USB), 0},
        .real_domain = 0,
        .domain_name = "USB",
        .refcnt      = 0,
        .used        = 0,
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
        .used        = 0,
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
        .used        = 0,
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
        .used        = 0,
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
#endif /* IFXMIPS_PMU_AR10_H */

