/******************************************************************************
**
** FILE NAME    : ifxmips_gptu_pm.c
** PROJECT      : IFX UEIP
** MODULES      : GPTU Power Management
**
** DATE         : 16 Dec 2009
** AUTHOR       : Huang Xiaogang 
** DESCRIPTION  : GPTU Driver Power Managment
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
** 16 Dec,2009  Huang Xiaogang  First UEIP release
*******************************************************************************/
/*!
  \defgroup IFX_GPTU_PM Power Management functions
  \ingroup IFX_GPTU
  \brief IFX gptu power management driver functions
*/

/*!
 \file ifxmips_gptu_pm.c
 \ingroup IFX_GPTU    
 \brief source file for GPTU Power Management
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
#include <asm/system.h>

/* Project header */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmcu.h>

/** 
 * \fn static IFX_PMCU_RETURN_t ifx_gptu_pmcu_state_req(IFX_PMCU_STATE_t pmcuState)
 * \brief the callback function to request pmcu state in the power management hardware-dependent module
 *
 * \param pmcuState This parameter is a PMCU state.
 *
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
 * \ingroup IFX_GPTU_PM
 */
static IFX_PMCU_RETURN_t 
ifx_gptu_pmcu_state_req(IFX_PMCU_STATE_t pmcuState)
{
    switch(pmcuState) 
    {
        case IFX_PMCU_STATE_D0: 
            return IFX_PMCU_RETURN_SUCCESS;  
        case IFX_PMCU_STATE_D1: // Not Applicable
            return IFX_PMCU_RETURN_DENIED; 
        case IFX_PMCU_STATE_D2: // Not Applicable
            return IFX_PMCU_RETURN_DENIED; 
        case IFX_PMCU_STATE_D3: // Module clock gating and Power gating
            return IFX_PMCU_RETURN_SUCCESS;        
        default:
            return IFX_PMCU_RETURN_DENIED;
    }
}

/** 
 * \fn static IFX_PMCU_RETURN_t ifx_gptu_pmcu_state_get(IFX_PMCU_STATE_t *pmcuState)
 * \brief the callback function to get pmcu state in the power management hardware-dependent module

 * \param pmcuState Pointer to return power state.
 *
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
 * \ingroup IFX_GPTU_PM
 */
static IFX_PMCU_RETURN_t 
ifx_gptu_pmcu_state_get(IFX_PMCU_STATE_t *pmcuState)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/**
 * \fn IFX_PMCU_RETURN_t ifx_gptu_pmcu_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
 * \brief Apply all callbacks registered to be executed before a state change for pmcuModule
 * 
 * \param   pmcuModule      Module
 * \param   newState        New state
 * \param   oldState        Old state
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \ingroup IFX_GPTU_PM
 */
static IFX_PMCU_RETURN_t 
ifx_gptu_pmcu_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/**
 * \fn IFX_PMCU_RETURN_t ifx_gptu_pmcu_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
 * \brief Apply all callbacks registered to be executed before a state change for pmcuModule
 * 
 * \param   pmcuModule      Module
 * \param   newState        New state
 * \param   oldState        Old state
 * \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
 * \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
 * \ingroup IFX_GPTU_PM
 */
static IFX_PMCU_RETURN_t 
ifx_gptu_pmcu_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/**
 * \fn IFX_PMCU_RETURN_t ifx_gptu_pmcu_destroy(void)
 * \brief Apply all callbacks registered to be executed before a state change for pmcuModule
 * 
 * \return IFX_PMCU_RETURN_SUCCESS  Successfully
 * \return IFX_PMCU_RETURN_ERROR    Error
 * \ingroup IFX_GPTU_PM
 */
static IFX_PMCU_RETURN_t 
ifx_gptu_pmcu_destroy(void)
{
    return IFX_PMCU_RETURN_SUCCESS;
}

/** 
 * \fn static void ifx_gptu_pmcu_init(void)
 * \brief Register with central PMCU module
 *
 * \return none
 * \ingroup IFX_GPTU_PM
 */
void
ifx_gptu_pmcu_init(void)
{
    IFX_PMCU_REGISTER_t pmcuRegister;

    /* XXX, hook driver context */

    /* State function register */
    memset(&pmcuRegister, 0, sizeof(IFX_PMCU_REGISTER_t));
    pmcuRegister.pmcuModule = IFX_PMCU_MODULE_SPI;
    pmcuRegister.pmcuModuleNr = 0;
    pmcuRegister.ifx_pmcu_state_req = ifx_gptu_pmcu_state_req;
    pmcuRegister.ifx_pmcu_state_get = ifx_gptu_pmcu_state_get;
    pmcuRegister.pre = ifx_gptu_pmcu_prechange;
    pmcuRegister.post= ifx_gptu_pmcu_postchange;
    pmcuRegister.ifx_pmcu_destroy = ifx_gptu_pmcu_destroy;
    ifx_pmcu_register(&pmcuRegister); 

#if 0
{
    IFX_PMCU_MODULE_INFO_t pmcuInfo;

    /* Information function register */
    memset(&pmcuInfo, 0, sizeof(IFX_PMCU_MODULE_INFO_t));
    /* This is required to program PMCU with initial Switch state */
    pmcuInfo.pmcuModule = IFX_PMCU_MODULE_SPI;
    pmcuInfo.pmcuModuleNr = 0;
    pmcuInfo.pmcuState = IFX_PMCU_STATE_D0;
    pmcuInfo.pmcuEvent = IFX_PMCU_EVENT_ACTIVATED;
    ifx_pmcu_info(&pmcuInfo);
}
#endif
}

/** 
 * \fn static void ifx_gptu_pmcu_exit(void)
 * \brief Unregister with central PMCU module
 *
 * \return none
 * \ingroup IFX_GPTU_PM
 */
void
ifx_gptu_pmcu_exit(void)
{
    IFX_PMCU_REGISTER_t pmcuUnRegister;

   /* XXX, hook driver context */
   
    pmcuUnRegister.pmcuModule = IFX_PMCU_MODULE_SPI; 
    pmcuUnRegister.pmcuModuleNr = 0;
    ifx_pmcu_unregister(&pmcuUnRegister);
}

