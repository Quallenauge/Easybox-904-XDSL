/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_mib.c
** PROJECT      : PPA
** MODULES      : PPA API (Generic MIB APIs)
**
** DATE         : 3 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Miscellaneous Functions
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 03 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#if defined(CONFIG_IFX_PPA_API_PROC)
#include <linux/proc_fs.h>
#endif
#include <linux/netdevice.h>
#include <linux/in.h>
#include <net/sock.h>
#include <net/ip_vs.h>
#include <asm/time.h>

#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>

#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppe_drv_wrapper.h"

#include "ifx_ppa_api_mib.h"


int32_t ifx_ppa_get_dsl_mib(PPA_DSL_QUEUE_MIB *mib, uint32_t flag)
{
    if( mib ) mib->flag = flag;
    return ifx_ppa_drv_get_dsl_mib(mib, 0);
}

int32_t ifx_ppa_clear_dsl_mib(PPA_DSL_QUEUE_MIB *mib, uint32_t flag)
{
    if(!ifx_ppa_drv_hal_generic_hook ) 
        return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_CLEAR_DSL_MIB,(void *)mib, 0 ); 
}

int32_t ppa_ioctl_get_dsl_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
    int res=IFX_FAILURE;

    
    ppa_memset(&cmd_info->dsl_mib_info.mib, 0, sizeof(cmd_info->dsl_mib_info.mib) );

    res = ifx_ppa_get_dsl_mib( &cmd_info->dsl_mib_info.mib, cmd_info->dsl_mib_info.flags);
    
    if ( ppa_copy_to_user( (void *)arg, &cmd_info->dsl_mib_info, sizeof(cmd_info->dsl_mib_info)) != 0 )
        return IFX_FAILURE;

    return res;
}

int32_t ppa_ioctl_clear_dsl_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
    int32_t res = ifx_ppa_clear_dsl_mib( &cmd_info->dsl_mib_info.mib, cmd_info->dsl_mib_info.flags);    
    
    return res;
}

int32_t ifx_ppa_get_ports_mib(PPA_PORT_MIB *mib)
{
   return ifx_ppa_drv_get_ports_mib( mib , 0);    
}

int32_t ppa_ioctl_get_ports_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
    int res=IFX_FAILURE;

    ppa_memset(&cmd_info->port_mib_info.mib, 0, sizeof(cmd_info->port_mib_info.mib) );

    res = ifx_ppa_get_ports_mib( &cmd_info->port_mib_info.mib );
    
    if ( ppa_copy_to_user( (void *)arg, &cmd_info->port_mib_info, sizeof(cmd_info->port_mib_info)) != 0 )
        return IFX_FAILURE;

    return res;
}

int32_t ifx_ppa_clear_port_mib(PPA_PORT_MIB *mib, uint32_t flag)
{
    if(!ifx_ppa_drv_hal_generic_hook ) 
        return IFX_FAILURE;

    return ifx_ppa_drv_hal_generic_hook(PPA_GENERIC_HAL_CLEAR_PORT_MIB,(void *)mib, 0 ); 
}

int32_t ppa_ioctl_clear_ports_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
    int32_t res = ifx_ppa_clear_port_mib( &cmd_info->port_mib_info.mib, cmd_info->port_mib_info.flags);    
    
    return res;
}

EXPORT_SYMBOL(ifx_ppa_get_ports_mib);
EXPORT_SYMBOL(ppa_ioctl_get_dsl_mib);
EXPORT_SYMBOL(ppa_ioctl_clear_dsl_mib);
EXPORT_SYMBOL(ppa_ioctl_get_ports_mib);
EXPORT_SYMBOL(ppa_ioctl_clear_ports_mib);


