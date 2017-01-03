/**
** FILE NAME    : ifxmips_eth_gpio.h
** PROJECT      : IFX UEIP
** MODULES      : ETH
** DATE         : 03 June 2009
** AUTHOR       : Reddy Mallikarjuna
** DESCRIPTION  : IFX ETH driver header file
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

#ifndef _IFXMIPS_ETH_GPIO_H_
#define _IFXMIPS_ETH_GPIO_H_
/*!
  \file ifxmips_eth_gpio.h
  \ingroup IFX_ETH_DRV
  \brief IFX Eth module gpio definition for Amazon-SE platforms
*/
#include <asm/ifx/ifx_gpio.h>

static const int eth_gpio_module_id = IFX_GPIO_MODULE_INTERNAL_SWITCH;

/** change the GPIO pins based on HW changes*/
/** IFX_RMIITXD0       GPIO 0 */
#define IFX_RMIITXD0                0
/** IFX_RMIITXD1       GPIO 4 */
#define IFX_RMIITXD1                4
/** IFX_TXEN     GPIO 13*/
#define IFX_TXEN                    13
/** IFX_MDC       GPIO 27 */
#define IFX_MDC                     27
/** IFX_MDIO       GPIO 24 */
#define IFX_MDIO                    24
/** IFX_RMIIRXD0  P0.9/P1.9  GPIO 9/25 */
#define IFX_RMIIRXD0                25
/** IFX_RMIIRXD1  P0.8/P1.7  GPIO 8/23 */
#define IFX_RMIIRXD1                23
/*IFX_RXDV  P0.7/P1.5  GPIO 7/21 */
#define IFX_RXDV                    21
/** IFX_REFCLK  P0.10/P1.6  GPIO 10/22 */
#define IFX_REFCLK                  22

void amazon_se_eth_gpio_configure (int en)
{
 /* TXD0 : P0.0 ALT 10*/
    if (en) {
        ifx_gpio_pin_reserve(IFX_RMIITXD0, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(IFX_RMIITXD0, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_RMIITXD0, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_RMIITXD0, eth_gpio_module_id);
 /* TXD1 : P0.4 ALT 11*/
        ifx_gpio_pin_reserve(IFX_RMIITXD1, eth_gpio_module_id);
        ifx_gpio_altsel0_set(IFX_RMIITXD1, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_RMIITXD1, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_RMIITXD1, eth_gpio_module_id);
 /* TXEN : P0.13 ALT 10 */
        ifx_gpio_pin_reserve(IFX_TXEN, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(IFX_TXEN, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_TXEN, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_TXEN, eth_gpio_module_id);
/* MDIO : P1.8 ALT 11*/
        ifx_gpio_pin_reserve(IFX_MDIO, eth_gpio_module_id);
        ifx_gpio_dir_out_set(IFX_MDIO, eth_gpio_module_id);
        ifx_gpio_altsel0_set(IFX_MDIO, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_MDIO, eth_gpio_module_id);
/* MDC : P1.11 ALT 11*/
        ifx_gpio_pin_reserve(IFX_MDC, eth_gpio_module_id);
        ifx_gpio_dir_out_set(IFX_MDC, eth_gpio_module_id);
        ifx_gpio_altsel0_set(IFX_MDC, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_MDC, eth_gpio_module_id);
/* RXD0 : P0.9/P1.9 ALT 10*/
        ifx_gpio_pin_reserve(IFX_RMIIRXD0, eth_gpio_module_id);
        ifx_gpio_dir_in_set(IFX_RMIIRXD0, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(IFX_RMIIRXD0, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_RMIIRXD0, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_RMIIRXD0, eth_gpio_module_id);
 /* RXD1 : P0.8/P1.7 ALT 10*/
        ifx_gpio_pin_reserve(IFX_RMIIRXD1, eth_gpio_module_id);
        ifx_gpio_dir_in_set(IFX_RMIIRXD1, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(IFX_RMIIRXD1, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_RMIIRXD1, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_RMIIRXD1, eth_gpio_module_id);
 /* RXDV : P0.7/P1.5 ALT 10 */
        ifx_gpio_pin_reserve(IFX_RXDV, eth_gpio_module_id);
        ifx_gpio_dir_in_set(IFX_RXDV, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(IFX_RXDV, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_RXDV, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_RXDV, eth_gpio_module_id);
/* REFCLK : P0.10/P1.6 ALT 10 */
        ifx_gpio_pin_reserve(IFX_REFCLK, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(IFX_REFCLK, eth_gpio_module_id);
        ifx_gpio_altsel1_set(IFX_REFCLK, eth_gpio_module_id);
        ifx_gpio_open_drain_set(IFX_REFCLK, eth_gpio_module_id);
    } else {
        ifx_gpio_pin_free(IFX_RMIITXD0, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_RMIITXD1, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_TXEN, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_MDIO, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_MDC, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_RMIIRXD0, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_RMIIRXD1, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_RXDV, eth_gpio_module_id);
        ifx_gpio_pin_free(IFX_REFCLK, eth_gpio_module_id);
    }
}

#endif /* _IFXMIPS_ETH_GPIO_H_ */
