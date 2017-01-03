/******************************************************************************
**
** FILE NAME    : ifxmips_usif_uart_pm.c
** PROJECT      : IFX UEIP
** MODULES      : USIF SPI Mode Power Management
**
** DATE         : 18 Dec 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : USIF SPI Mode Driver Power Managment
** COPYRIGHT    :       Copyright (c) 2009
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
** 18 Dec,2009   Lei Chuanhua    First UEIP release
*******************************************************************************/
/*!
  \defgroup IFX_USIF_UART_PM Power Management functions
  \ingroup IFX_USIF_UART
  \brief IFX USIF UART power management driver functions
*/

/*!
 \file ifxmips_usif_uart_pm.c
 \ingroup IFX_USIF_UART    
 \brief source file for USIF UART Mode Power Management
*/

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/serial_core.h>
#include <asm/system.h>

/* Project header */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmcu.h>
#include "ifxmips_usif_uart.h"

/** 
 * \fn static IFX_PMCU_RETURN_t ifx_usif_uart_pmcu_state_change(IFX_PMCU_STATE_t pmcuState)
 * \brief the callback function to request pmcu state in the power management hardware-dependent module
 *
 * \param pmcuState This parameter is a PMCU state.
 *
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
 * \ingroup IFX_USIF_UART_PM
 */
static IFX_PMCU_RETURN_t 
ifx_usif_uart_pmcu_state_change(IFX_PMCU_STATE_t pmcuState)
{
    switch(pmcuState) 
    {
        case IFX_PMCU_STATE_D0:
            return IFX_PMCU_RETURN_SUCCESS;
        case IFX_PMCU_STATE_D1: // Not Applicable
            return IFX_PMCU_RETURN_SUCCESS; 
        case IFX_PMCU_STATE_D2: // Not Applicable
            return IFX_PMCU_RETURN_SUCCESS;
        case IFX_PMCU_STATE_D3: // Module clock gating and Power gating
            return IFX_PMCU_RETURN_SUCCESS;
        default:
            return IFX_PMCU_RETURN_DENIED;
    }
}

/** 
 * \fn static IFX_PMCU_RETURN_t ifx_usif_uart_pmcu_state_get(IFX_PMCU_STATE_t *pmcuState)
 * \brief the callback function to get pmcu state in the power management hardware-dependent module

 * \param pmcuState Pointer to return power state.
 *
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
 * \ingroup IFX_USIF_UART_PM
 */
static IFX_PMCU_RETURN_t 
ifx_usif_uart_pmcu_state_get(IFX_PMCU_STATE_t *pmcuState)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/**
 * \fn IFX_PMCU_RETURN_t ifx_usif_uart_pmcu_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
 * \brief Apply all callbacks registered to be executed before a state change for pmcuModule
 * 
 * \param   pmcuModule      Module
 * \param   newState        New state
 * \param   oldState        Old state
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \ingroup IFX_USIF_UART_PM
 */
static IFX_PMCU_RETURN_t 
ifx_usif_uart_pmcu_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/**
 * \fn IFX_PMCU_RETURN_t ifx_usif_uart_pmcu_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
 * \brief Apply all callbacks registered to be executed before a state change for pmcuModule
 * 
 * \param   pmcuModule      Module
 * \param   newState        New state
 * \param   oldState        Old state
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \ingroup IFX_USIF_UART_PM
 */
static IFX_PMCU_RETURN_t 
ifx_usif_uart_pmcu_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/** 
 * \fn static void ifx_usif_uart_pmcu_init(IFX_USIF_UART_PORT_t *port)
 * \brief Register with central PMCU module
 * \param port Pointer to struct ifx_usif_uart_port which will hook to driver context later
 *
 * \return none
 * \ingroup IFX_USIF_UART_PM
 */
void
ifx_usif_uart_pmcu_init(IFX_USIF_UART_PORT_t *port)
{
    IFX_PMCU_REGISTER_t pmcuRegister;

    /* XXX, hook driver context */

    /* State function register */
    memset(&pmcuRegister, 0, sizeof(IFX_PMCU_REGISTER_t));
    pmcuRegister.pmcuModule = IFX_PMCU_MODULE_USIF_UART;
    pmcuRegister.pmcuModuleNr = 0;
    pmcuRegister.ifx_pmcu_state_change = ifx_usif_uart_pmcu_state_change;
    pmcuRegister.ifx_pmcu_state_get = ifx_usif_uart_pmcu_state_get;
    pmcuRegister.pre = ifx_usif_uart_pmcu_prechange;
    pmcuRegister.post= ifx_usif_uart_pmcu_postchange;
    ifx_pmcu_register(&pmcuRegister); 
}

/** 
 * \fn static void ifx_usif_uart_pmcu_exit(IFX_USIF_UART_PORT_t *port)
 * \brief Unregister with central PMCU module
 *
 * \param port Pointer to struct ifx_usif_uart_port which will hook to driver context late
 *
 * \return none
 * \ingroup IFX_USIF_UART_PM
 */
void
ifx_usif_uart_pmcu_exit(IFX_USIF_UART_PORT_t *port)
{
    IFX_PMCU_REGISTER_t pmcuUnRegister;

   /* XXX, hook driver context */
   
    pmcuUnRegister.pmcuModule = IFX_PMCU_MODULE_USIF_UART; 
    pmcuUnRegister.pmcuModuleNr = 0;
    ifx_pmcu_unregister(&pmcuUnRegister);
}

