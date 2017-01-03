/******************************************************************************
**
** FILE NAME    : ifxmips_rcu_vr9.h
** PROJECT      : UEIP
** MODULES      : RCU (Reset Control Unit)
**
** DATE         : 17 Jun 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : RCU driver header file for VR9
** COPYRIGHT    :       Copyright (c) 2006
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
** 17 JUN 2009  Xu Liang        Init Version
*******************************************************************************/



#ifndef IFXMIPS_RCU_VR9_H
#define IFXMIPS_RCU_VR9_H



static ifx_rcu_domain_t g_rcu_domains[IFX_RCU_DOMAIN_MAX] = {
    {
        //  IFX_RCU_DOMAIN_HRST
        .affected_domains   = 1 << IFX_RCU_DOMAIN_HRST,
        .rst_req_value      = 1 << 0,
        .rst_req_mask       = 1 << 0,
        .rst_stat_mask      = 1 << 0,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_CPU0
        .affected_domains   = 1 << IFX_RCU_DOMAIN_CPU0,
        .rst_req_value      = 1 << 1,
        .rst_req_mask       = 1 << 1,
        .rst_stat_mask      = 1 << 1,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_FPI
        .affected_domains   = 1 << IFX_RCU_DOMAIN_FPI,
        .rst_req_value      = 1 << 2,
        .rst_req_mask       = 1 << 2,
        .rst_stat_mask      = 1 << 2,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_DSLDSP
        .affected_domains   = (1 << IFX_RCU_DOMAIN_DSLDSP) | (1 << IFX_RCU_DOMAIN_DSLDFE) | (1 << IFX_RCU_DOMAIN_ARC) | (1 << IFX_RCU_DOMAIN_DSLAFE),
        .rst_req_value      = (1 << 3) | (1 << 11), //  bit 3 - ARC, bit 11 - accelerator, both are subset of bit 7 - DFE core (overall) reset
        .rst_req_mask       = (1 << 3) | (1 << 11),
        .rst_stat_mask      = (1 << 3) | (1 << 11),
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_USB1
        .affected_domains   = (1 << IFX_RCU_DOMAIN_USB1) | (1 << IFX_RCU_DOMAIN_USB0),
        .rst_req_value      = 1 << 4,
        .rst_req_mask       = 1 << 4,
        .rst_stat_mask      = 1 << 4,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_USB0
        .affected_domains   = (1 << IFX_RCU_DOMAIN_USB1) | (1 << IFX_RCU_DOMAIN_USB0),
        .rst_req_value      = 1 << 4,
        .rst_req_mask       = 1 << 4,
        .rst_stat_mask      = 1 << 4,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_ETHMAC1
        .affected_domains   = 1 << IFX_RCU_DOMAIN_ETHMAC1,
        .rst_req_value      = 0,
        .rst_req_mask       = 0,
        .rst_stat_mask      = 0,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_AHB
        .affected_domains   = 1 << IFX_RCU_DOMAIN_AHB,
        .rst_req_value      = 1 << 6,
        .rst_req_mask       = 1 << 6,
        .rst_stat_mask      = 1 << 6,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_DSLDFE
        .affected_domains   = (1 << IFX_RCU_DOMAIN_DSLDFE) | (1 << IFX_RCU_DOMAIN_DSLDSP) | (1 << IFX_RCU_DOMAIN_ARC) | (1 << IFX_RCU_DOMAIN_DSLAFE),
        .rst_req_value      = 1 << 7,
        .rst_req_mask       = 1 << 7,
        .rst_stat_mask      = 1 << 7,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_PPE
        .affected_domains   = (1 << IFX_RCU_DOMAIN_PPE) | (1 << IFX_RCU_DOMAIN_DSLDSP) | (1 << IFX_RCU_DOMAIN_DSLDFE) | (1 << IFX_RCU_DOMAIN_ARC) | (1 << IFX_RCU_DOMAIN_DSLAFE) | (1 << IFX_RCU_DOMAIN_DSLTC),
        .rst_req_value      = 1 << 8,
        .rst_req_mask       = 1 << 8,
        .rst_stat_mask      = 1 << 8,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_DMA
        .affected_domains   = 1 << IFX_RCU_DOMAIN_DMA,
        .rst_req_value      = 1 << 9,
        .rst_req_mask       = 1 << 9,
        .rst_stat_mask      = 1 << 9,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_SDIO
        .affected_domains   = 1 << IFX_RCU_DOMAIN_SDIO,
        .rst_req_value      = 1 << 10,
        .rst_req_mask       = 1 << 10,
        .rst_stat_mask      = 1 << 10,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_DSLAFE
        .affected_domains   = 1 << IFX_RCU_DOMAIN_DSLAFE,
        .rst_req_value      = 1 << 11,
        .rst_req_mask       = 1 << 11,
        .rst_stat_mask      = 1 << 11,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_VOICE
        .affected_domains   = 1 << IFX_RCU_DOMAIN_VOICE,
        .rst_req_value      = 0,
        .rst_req_mask       = 0,
        .rst_stat_mask      = 0,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_PCI
        .affected_domains   = 1 << IFX_RCU_DOMAIN_PCI,
        .rst_req_value      = 1 << 13,
        .rst_req_mask       = 1 << 13,
        .rst_stat_mask      = 1 << 13,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_MC
        .affected_domains   = 1 << IFX_RCU_DOMAIN_MC,
        .rst_req_value      = 1 << 14,
        .rst_req_mask       = 1 << 14,
        .rst_stat_mask      = 1 << 14,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_SW
        .affected_domains   = 1 << IFX_RCU_DOMAIN_SW,
        .rst_req_value      = 1 << 21,
        .rst_req_mask       = 1 << 21,
        .rst_stat_mask      = 1 << 16,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_TDM
        .affected_domains   = 1 << IFX_RCU_DOMAIN_TDM,
        .rst_req_value      = 1 << 19,
        .rst_req_mask       = 1 << 19,
        .rst_stat_mask      = 1 << 25,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_DSLTC
        .affected_domains   = 1 << IFX_RCU_DOMAIN_DSLTC,
        .rst_req_value      = 1 << 23,
        .rst_req_mask       = 1 << 23,
        .rst_stat_mask      = 1 << 23,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_CPU1
        .affected_domains   = 1 << IFX_RCU_DOMAIN_CPU1,
        .rst_req_value      = 0,
        .rst_req_mask       = 0,
        .rst_stat_mask      = 0,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_EPHY
        .affected_domains   = 1 << IFX_RCU_DOMAIN_EPHY,
        .rst_req_value      = 0,
        .rst_req_mask       = 0,
        .rst_stat_mask      = 0,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_GPHY0
        .affected_domains   = 1 << IFX_RCU_DOMAIN_GPHY0,
        .rst_req_value      = 1 << 31,
        .rst_req_mask       = 1 << 31,
        .rst_stat_mask      = 1 << 30,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_GPHY1
        .affected_domains   = 1 << IFX_RCU_DOMAIN_GPHY1,
        .rst_req_value      = 1 << 29,
        .rst_req_mask       = 1 << 29,
        .rst_stat_mask      = 1 << 28,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_ARC
        .affected_domains   = 1 << IFX_RCU_DOMAIN_ARC,
        .rst_req_value      = 1 << 3,
        .rst_req_mask       = 1 << 3,
        .rst_stat_mask      = 1 << 3,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_PCIE_PHY
        .affected_domains   = 1 << IFX_RCU_DOMAIN_PCIE_PHY,
        .rst_req_value      = 1 << 12,
        .rst_req_mask       = 1 << 12,
        .rst_stat_mask      = 1 << 24,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_PCIE
        .affected_domains   = 1 << IFX_RCU_DOMAIN_PCIE,
        .rst_req_value      = 1 << 22,
        .rst_req_mask       = 1 << 22,
        .rst_stat_mask      = 1 << 22,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
    {
        //  IFX_RCU_DOMAIN_HSNAND
        .affected_domains   = 1 << IFX_RCU_DOMAIN_HSNAND,
        .rst_req_value      = 1 << 16,
        .rst_req_mask       = 1 << 16,
        .rst_stat_mask      = 1 << 5,
        .latch              = 0,
        .udelay             = 0,
        .handlers           = NULL,
    },
};



#endif  //  IFXMIPS_RCU_VR9_H
