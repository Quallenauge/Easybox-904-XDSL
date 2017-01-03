/******************************************************************************
**
** FILE NAME    : ifx_ppa_api_pws.c
** PROJECT      : UEIP
** MODULES      : PPA API (Power Saving APIs)
**
** DATE         : 16 DEC 2009
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Protocol Stack Power Saving Logic API Implementation
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author            $Comment
** 16 DEC 2009  Shao Guohua        Initiate Version
*******************************************************************************/
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppe_drv_wrapper.h"


static IFX_PMCU_STATE_t ppa_psw_stat_curr   = IFX_PMCU_STATE_D0;



int32_t ppa_pwm_logic_init(void)
{
    return IFX_SUCCESS;
}

IFX_PMCU_STATE_t ppa_pwm_get_current_status(int32_t flag)
{
    return ppa_psw_stat_curr;
}

int32_t ppa_pwm_set_current_state(IFX_PMCU_STATE_t e_state, int32_t flag)
{
    return IFX_SUCCESS;
}

int32_t ppa_pwm_pre_set_current_state(IFX_PMCU_STATE_t e_state, int32_t flag)
{
    int32_t ret;
    uint32_t pos;
    void *p_item;

    if ( ppa_psw_stat_curr == IFX_PMCU_STATE_D0 && e_state != IFX_PMCU_STATE_D0 )
    {
        pos = 0;
        ret = ppa_session_start_iteration(&pos, (struct session_list_item **)&p_item);
        if ( ret == IFX_SUCCESS ) {
            do {
                if ( (((struct session_list_item *)p_item)->flags & SESSION_ADDED_IN_HW) ) {
                    ppa_session_stop_iteration();
                    return IFX_ERROR;
                }
            } while ( ppa_session_iterate_next(&pos, (struct session_list_item **)&p_item) == IFX_SUCCESS );
        }
        ppa_session_stop_iteration();

/*
        pos = 0;
        ret = ppa_bridging_session_start_iteration(&pos, (struct bridging_session_list_item **)&p_item);
        ppa_bridging_session_stop_iteration();
        if ( ret == IFX_SUCCESS && p_item != NULL )
            return IFX_ERROR;
*/

        pos = 0;
        ret = ppa_mc_group_start_iteration(&pos, (struct mc_group_list_item **)&p_item);
        ppa_mc_group_stop_iteration();
        if ( ret == IFX_SUCCESS && p_item != NULL )
            return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int32_t ppa_pwm_post_set_current_state(IFX_PMCU_STATE_t e_state, int32_t flag)
{
    unsigned long sys_flag;

    local_irq_save(sys_flag);
    if ( ppa_psw_stat_curr != e_state )
    {
        int pwm_level = e_state == IFX_PMCU_STATE_D0 ? IFX_PPA_PWM_LEVEL_D0 : IFX_PPA_PWM_LEVEL_D3;

         ifx_ppa_drv_ppe_clk_change(pwm_level, 0);

        ppa_psw_stat_curr = e_state;
    }
    local_irq_restore(sys_flag);

    return IFX_SUCCESS;
}
