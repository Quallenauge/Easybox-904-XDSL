
/******************************************************************************
**
** FILE NAME    : ifxmips_pmu_ar9.h
** PROJECT      : IFX UEIP
** MODULES      : PMU
**
** DATE         : 02 Sep 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX PMU AR9 specific header file
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
  \file ifxmips_pmu_ar9.h
  \ingroup IFX_PMU
  \brief PMU driver ARX specific file
*/
#ifndef IFXMIPS_PMU_AR9_H

#define IFXMIPS_PMU_AR9_H

/* Module id index */
#define IFX_PMU_MODULE_ID_IDX_MIN  IFX_PMU_MODULE_USB_PHY
#define IFX_PMU_MODULE_ID_IDX_MAX  IFX_PMU_MODULE_DDR_DPD

static ifx_pmu_t g_ifx_pmu = {
    .pmu_pwdcr[0]  = IFX_PMU_PWDCR,
    .pmu_pwdcr[1]  = NULL,
    .pmu_pwdcr[2]  = NULL,
    .pmu_pwdcrb[0] = NULL,
    .pmu_pwdcrb[1] = NULL,
    .pmu_pwdcrb[2] = NULL,    
    .pmu_sr[0]     = IFX_PMU_SR,
    .pmu_sr[1]     = NULL,
    .pmu_sr[2]     = NULL,
    .version       = IFX_PMU_CLK_V1,
    .module        = {
                    {IFX_PMU_MODULE_USB0_PHY, IFX_PMU_MODULE_USB0_PHY, 0, "USB0-PHY", 1},
                    {IFX_PMU_MODULE_FPI1, IFX_PMU_MODULE_FPI1, 0, "FPI1",     0},
                    {IFX_PMU_MODULE_DFEV0, IFX_PMU_MODULE_DFEV0, 0, "DFEV0",    1},
                    {IFX_PMU_MODULE_DFEV1, IFX_PMU_MODULE_DFEV1, 0, "DFEV1",    1},
                    {IFX_PMU_MODULE_PCI, IFX_PMU_MODULE_PCI, 0, "PCI",      0},
                    {IFX_PMU_MODULE_DMA, IFX_PMU_MODULE_DMA, 0, "DMA",      1},
                    {IFX_PMU_MODULE_USB0_CTRL, IFX_PMU_MODULE_USB0_CTRL, 0, "USB0-CTRL",1},
                    {IFX_PMU_MODULE_UART0, IFX_PMU_MODULE_UART0, 0, "UART0",    1},
                    {IFX_PMU_MODULE_SPI, IFX_PMU_MODULE_SPI, 0, "SPI",      1},
                    {IFX_PMU_MODULE_DSL_DFE, IFX_PMU_MODULE_DSL_DFE, 0, "DSL-DFE",  1},
                    {IFX_PMU_MODULE_EBU, IFX_PMU_MODULE_EBU, 0, "EBU",      0},
                    {IFX_PMU_MODULE_LEDC, IFX_PMU_MODULE_LEDC, 0, "LEDC",     1},
                    {IFX_PMU_MODULE_GPTC, IFX_PMU_MODULE_GPTC, 0, "GPTC",     1},
                    {IFX_PMU_MODULE_VLYNQ, IFX_PMU_MODULE_VLYNQ, 0, "VLYNQ",    1},
                    {IFX_PMU_MODULE_FPI0, IFX_PMU_MODULE_FPI0, 0, "FPI0",     0},
                    {IFX_PMU_MODULE_AHB, IFX_PMU_MODULE_AHB, 0, "AHB",      0},
                    {16, 16, 0, "SDIO",     1},
                    {IFX_PMU_MODULE_UART1, IFX_PMU_MODULE_UART1, 0, "UART1",    0}, /* Keep console on */  
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {IFX_PMU_MODULE_DEU, IFX_PMU_MODULE_DEU, 0, "DEU",      1},
                    {IFX_PMU_MODULE_PPE_TC, IFX_PMU_MODULE_PPE_TC, 0, "PPE-TC",   1},
                    {IFX_PMU_MODULE_PPE_EMA, IFX_PMU_MODULE_PPE_EMA, 0, "PPE-EMA",  1},
                    {IFX_PMU_MODULE_PPE_DPLUS, IFX_PMU_MODULE_PPE_DPLUS, 0, "PPE-DPLUS",1},
                    {IFX_PMU_MODULE_DDR_MEM, IFX_PMU_MODULE_DDR_MEM, 0, "DDR-MEM",  0},
                    {IFX_PMU_MODULE_TDM, IFX_PMU_MODULE_TDM, 0, "TDM",      1},
                    {IFX_PMU_MODULE_USB1_PHY, IFX_PMU_MODULE_USB1_PHY, 0, "USB1-PHY", 1},
                    {IFX_PMU_MODULE_USB1_CTRL, IFX_PMU_MODULE_USB1_CTRL, 0, "USB1-CTRL",1},
                    {IFX_PMU_MODULE_SWITCH, IFX_PMU_MODULE_SWITCH, 0, "SWITCH",   1},
                    {IFX_PMU_MODULE_DDR_DPD, IFX_PMU_MODULE_DDR_DPD, 0, "DDR-DPD",  0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    },
};


#endif /* IFXMIPS_PMU_AR9_H */

