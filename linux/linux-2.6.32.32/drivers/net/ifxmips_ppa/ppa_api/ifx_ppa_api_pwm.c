/******************************************************************************
**
** FILE NAME    : ifx_ppa_api_pwm.c
** PROJECT      : UEIP
** MODULES      : PPA API (Power Management APIs)
**
** DATE         : 16 DEC 2009
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Protocol Stack Power Management API Implementation
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author              $Comment
** 16 DEC 2009  Shao Guohua          Initiate Version
*******************************************************************************/
/*!
  \defgroup IFX_PPA_API_PWM PPA API Power Management functions
  \ingroup IFX_PPA_API
  \brief IFX PPA API Power Management functions
*/

/*!
 \file ifx_ppa_api_pwm.c
 \ingroup IFX_PPA_API
 \brief source file for PPA API Power Management
*/
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/time.h>

//PMCU specific Head File
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_pmcu.h>

//PPA Specific Head File
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_pwm_logic.h"



/* define dependency list;  state D0D3 means don't care */
/* static declaration is necessary to let gcc accept this static initialisation. */
static IFX_PMCU_MODULE_DEP_t depList=
{
    1,
    {
        {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3}
    }
};

/*!
 \brief the callback function by pmcu to request PPA Power Mangement to change to new state
    \param pmcuState This parameter is a PMCU state.
    \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
    \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
    \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
    \ingroup IFX_PPA_API_PWM
 */
static IFX_PMCU_RETURN_t ppa_pwm_state_set(IFX_PMCU_STATE_t pmcuState)
{
    if( ppa_pwm_set_current_state(pmcuState, 0) == IFX_SUCCESS )
        return IFX_PMCU_RETURN_SUCCESS;
    else
        return IFX_PMCU_RETURN_DENIED;
}


/*!
 \brief the callback function by pmcu to get PPA current Power management state
    \param pmcuState Pointer to return power state.
    \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
    \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
    \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
    \ingroup IFX_PPA_API_PWM
 */
static IFX_PMCU_RETURN_t ppa_pwm_state_get(IFX_PMCU_STATE_t *pmcuState)
{
    if( pmcuState )
        *pmcuState = ppa_pwm_get_current_status(0);

    return IFX_PMCU_RETURN_SUCCESS;
}


/*!
 \brief callback function by pmcu before a state change
    \param   pmcuModule      Module
    \param   newState        New state
    \param   oldState        Old state
    \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
    \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
    \return IFX_PMCU_RETURN_DENIED  Not allowed to operate power state
    \ingroup IFX_PPA_API_PWM
 */
static IFX_PMCU_RETURN_t  ifx_ppa_pwm_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    if ( ppa_pwm_pre_set_current_state(newState, 0) == IFX_SUCCESS )
        return IFX_PMCU_RETURN_SUCCESS;
    else
        return IFX_PMCU_RETURN_DENIED;
}


/*!
 \brief callback function by pmcu after a state change
    \param   pmcuModule      Module
    \param   newState        New state
    \param   oldState        Old state
    \return IFX_PMCU_RETURN_SUCCESS Set Power State successfully
    \return IFX_PMCU_RETURN_ERROR   Failed to set power state.
    \ingroup IFX_PPA_API_PWM
 */
static IFX_PMCU_RETURN_t ifx_ppa_pwm_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    if ( ppa_pwm_post_set_current_state(newState, 0) == IFX_SUCCESS )
        return IFX_PMCU_RETURN_SUCCESS;
    else
        return IFX_PMCU_RETURN_DENIED;
}

/*!
      \brief Request D0 power state when any session is added into table.
       \return none
       \ingroup IFX_PPA_API_PWM
*/
void ifx_ppa_pwm_activate_module(void)
{
    if ( ppa_pwm_get_current_status(0) != IFX_PMCU_STATE_D0 )
        ifx_pmcu_state_req(IFX_PMCU_MODULE_PPE, 0, IFX_PMCU_STATE_D0);
}

/*!
      \brief Request D3 power state when any session is removed from table.
       \return none
       \ingroup IFX_PPA_API_PWM
*/
void ifx_ppa_pwm_deactivate_module(void)
{
    if ( ppa_pwm_get_current_status(0) != IFX_PMCU_STATE_D3 && ppa_pwm_pre_set_current_state(IFX_PMCU_STATE_D3, 0) == IFX_SUCCESS )
        ifx_pmcu_state_req(IFX_PMCU_MODULE_PPE, 0, IFX_PMCU_STATE_D3);
}

/*!
      \brief Init PPA Power management
       \return none
       \ingroup IFX_PPA_API_PWM
*/
void ifx_ppa_pwm_init(void)
{
    IFX_PMCU_REGISTER_t pmcuRegister;
    ppa_pwm_logic_init();

    memset(&pmcuRegister, 0, sizeof(pmcuRegister));

    pmcuRegister.pmcuModule     = IFX_PMCU_MODULE_PPE;
    pmcuRegister.pmcuModuleNr   = 0;
    pmcuRegister.pmcuModuleDep  = &depList;

    /* Register callback function for PMCU*/
    pmcuRegister.pre                    = ifx_ppa_pwm_prechange;
    pmcuRegister.ifx_pmcu_state_change  = ppa_pwm_state_set;
    pmcuRegister.post                   = ifx_ppa_pwm_postchange;
    pmcuRegister.ifx_pmcu_state_get     = ppa_pwm_state_get;

    if( ifx_pmcu_register ( &pmcuRegister ) != IFX_PMCU_RETURN_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_ERR, "ppa pwm failed to ifx_pmcu_register\n");
        return ;
    }

    ppa_debug(DBG_ENABLE_MASK_PWM, "ppa pwm init ok !\n");
}


/*!
      \brief Exit PPA Power management
       \return none
       \ingroup IFX_PPA_API_PWM
*/
void ifx_ppa_pwm_exit(void)
{
    IFX_PMCU_REGISTER_t pmcuUnRegister;

    memset (&pmcuUnRegister, 0, sizeof(pmcuUnRegister));
    pmcuUnRegister.pmcuModule   = IFX_PMCU_MODULE_PPE;
    pmcuUnRegister.pmcuModuleNr = 0;
    ifx_pmcu_unregister(&pmcuUnRegister);
    ppa_debug(DBG_ENABLE_MASK_PWM, "ppa pwm exit !\n");
}
