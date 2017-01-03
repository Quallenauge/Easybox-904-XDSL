/******************************************************************************
**
** FILE NAME    : ifxmips_pmu_amazon_se.h
** PROJECT      : IFX UEIP
** MODULES      : PMU
**
** DATE         : 02 Sep 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX PMU amazon_se specific header file
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
  \file ifxmips_pmu_amazon_se.h
  \ingroup IFX_PMU
  \brief PMU driver ASE specific file
*/

#ifndef IFXMIPS_PMU_AMAZON_SE_H
#define IFXMIPS_PMU_AMAZON_SE_H

#define IFX_PMU_MODULE_ID_IDX_MIN  IFX_PMU_MODULE_USB_PHY
#define IFX_PMU_MODULE_ID_IDX_MAX  IFX_PMU_MODULE_PPE_ENET0

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
                    {IFX_PMU_MODULE_USB_PHY, IFX_PMU_MODULE_USB_PHY, 0, "USB-PHY",  1},
                    {IFX_PMU_MODULE_FPI2, IFX_PMU_MODULE_FPI2, 0, "FPI2",     0},
                    {IFX_PMU_MODULE_SDIO, IFX_PMU_MODULE_SDIO, 0, "SDIO",     1},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {IFX_PMU_MODULE_DMA, IFX_PMU_MODULE_DMA, 0, "DMA",      1}, /* Default off, has to be controlled */
                    {IFX_PMU_MODULE_USB_CTRL, IFX_PMU_MODULE_USB_CTRL, 0, "USB-CTRL", 1},
                    {IFX_PMU_MODULE_EPHY, IFX_PMU_MODULE_EPHY, 0, "EPHY",     1},
                    {IFX_PMU_MODULE_SPI, IFX_PMU_MODULE_SPI, 0, "SPI",      1},
                    {IFX_PMU_MODULE_DSL_DFE, IFX_PMU_MODULE_DSL_DFE , 0, "DSL-DFE",  1},
                    {IFX_PMU_MODULE_EBU, IFX_PMU_MODULE_EBU, 0, "EBU",      1},
                    {IFX_PMU_MODULE_LEDC, IFX_PMU_MODULE_LEDC, 0, "LEDC",     1},
                    {IFX_PMU_MODULE_GPTC, IFX_PMU_MODULE_GPTC, 0, "GPTC",     1},
                    {IFX_PMU_MODULE_PPE_TPE, IFX_PMU_MODULE_PPE_TPE, 0, "PPE",      1},
                    {IFX_PMU_MODULE_FPI0, IFX_PMU_MODULE_FPI0, 0, "FPI0",     0},
                    {IFX_PMU_MODULE_AHB, IFX_PMU_MODULE_AHB, 0, "AHB",      1},
                    {-1, -1, 0, "",         0},
                    {IFX_PMU_MODULE_UART1, IFX_PMU_MODULE_UART1, 0, "UART1",    0}, /* Keep console on */  
                    {IFX_PMU_MODULE_WDT0, IFX_PMU_MODULE_WDT0, 0, "WDT0",     1},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {IFX_PMU_MODULE_PPE_TC, IFX_PMU_MODULE_PPE_TC , 0, "PPE-TC",   1},
                    {-1, -1, 0, "",         0},
                    {IFX_PMU_MODULE_PPE_ENET0, IFX_PMU_MODULE_PPE_ENET0, 0, "PPE-ENET0",1},
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
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    {-1, -1, 0, "",         0},
                    },
};
#endif /* IFXMIPS_PMU_AMAZON_SE_H */

