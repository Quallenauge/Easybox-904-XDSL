/**
** FILE NAME    : ifxmips_mmc_amazon_se.h
** PROJECT      : IFX UEIP
** MODULES      : MMC
** DATE         : 03 June 2009
** AUTHOR       : Reddy Mallikarjuna
** DESCRIPTION  : IFX MMC driver header file
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
** $Date            $Author         $Comment
** 03 June 2009     Reddy Mallikarjuna  Initial release
*******************************************************************************/

#ifndef _IFXMIPS_MMC_AMAZON_SE_H_
#define _IFXMIPS_MMC_AMAZON_SE_H_

/*!
  \file ifxmips_mmc_amazon_se.h
  \ingroup IFX_MMC_DRV
  \brief IFX MMC module gpio definition for Amazon-SE platforms
*/

#include <asm/ifx/ifx_gpio.h>

static const int mmc_gpio_module_id = IFX_GPIO_MODULE_SDIO;

/** change the GPIO pins based on HW changes*/
/** IFX_MCLCMD       GPIO 19 */
#define IFX_MCLCMD          19
/** IFX_MCLCLK       GPIO 15 */
#define IFX_MCLCLK          15
/*IFX_MCLDATA0     GPIO 12*/
#define IFX_MCLDATA0        12
/*IFX_MCLDATA1     GPIO 26 */
#define IFX_MCLDATA1    26 
/*IFX_MCLDATA2     GPIO 28 */
#define IFX_MCLDATA2        28
/*IFX_MCLDATA3     GPIO 20 */
#define IFX_MCLDATA3     20

void amazon_se_mmc_gpio_configure (void)
{
 /* MCLCLK : P0.15 ALT 11, MMC clock */
    ifx_gpio_pin_reserve(IFX_MCLCLK, mmc_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_MCLCLK, mmc_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MCLCLK, mmc_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MCLCLK, mmc_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_MCLCLK, mmc_gpio_module_id);
    ifx_gpio_pudsel_set(IFX_MCLCLK, mmc_gpio_module_id);
    ifx_gpio_puden_set(IFX_MCLCLK, mmc_gpio_module_id);

 /* MCLCMD : P1.3 ALT 11, MMC Command*/
    ifx_gpio_pin_reserve(IFX_MCLCMD, mmc_gpio_module_id);
    ifx_gpio_dir_in_set(IFX_MCLCMD, mmc_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MCLCMD, mmc_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MCLCMD, mmc_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_MCLCMD, mmc_gpio_module_id);
    ifx_gpio_pudsel_set(IFX_MCLCMD, mmc_gpio_module_id);
    ifx_gpio_puden_set(IFX_MCLCMD, mmc_gpio_module_id);

 /* MCLDATA0 : P0.12 ALT 11, MMC Data 0 */
    ifx_gpio_pin_reserve(IFX_MCLDATA0, mmc_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MCLDATA0, mmc_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MCLDATA0, mmc_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_MCLDATA0, mmc_gpio_module_id);
    ifx_gpio_pudsel_set(IFX_MCLDATA0, mmc_gpio_module_id);
    ifx_gpio_puden_set(IFX_MCLDATA0, mmc_gpio_module_id);

 /* MCLDATA1 : P1.10 ALT 11, MMC DATA1*/
    ifx_gpio_pin_reserve(IFX_MCLDATA1, mmc_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MCLDATA1, mmc_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MCLDATA1, mmc_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_MCLDATA1, mmc_gpio_module_id);
    ifx_gpio_pudsel_set(IFX_MCLDATA1, mmc_gpio_module_id);
    ifx_gpio_puden_set(IFX_MCLDATA1, mmc_gpio_module_id);

 /* MCLDATA2 : P1.12 ALT 11, MMC DATA2*/
    ifx_gpio_pin_reserve(IFX_MCLDATA2, mmc_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MCLDATA2, mmc_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MCLDATA2, mmc_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_MCLDATA2, mmc_gpio_module_id);
    ifx_gpio_pudsel_set(IFX_MCLDATA2, mmc_gpio_module_id);
    ifx_gpio_puden_set(IFX_MCLDATA2, mmc_gpio_module_id);

 /* MCLDATA3 : P1.4 ALT 11, MMC DATA3*/
    ifx_gpio_pin_reserve(IFX_MCLDATA3, mmc_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MCLDATA3, mmc_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MCLDATA3, mmc_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_MCLDATA3, mmc_gpio_module_id);
    ifx_gpio_pudsel_set(IFX_MCLDATA3, mmc_gpio_module_id);
    ifx_gpio_puden_set(IFX_MCLDATA3, mmc_gpio_module_id);
}

#endif /* _IFXMIPS_MMC_AMAZON_SE_H_ */
