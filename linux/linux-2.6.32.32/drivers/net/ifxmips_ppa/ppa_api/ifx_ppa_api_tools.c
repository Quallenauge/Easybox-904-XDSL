/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_tools.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 18 March 2010
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Protocol Stack Tools API Implementation
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 18 March 2010  Shao Guohua        Initiate Version
*******************************************************************************/

/*
 *  Common Head File
 */
#include <linux/autoconf.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/version.h>
//#include <linux/types.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#include <linux/netdevice.h>
//#include <linux/in.h>
//#include <net/sock.h>
//#include <net/ip_vs.h>
//#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include "ifx_ppa_ss.h"
#include "ifx_ppa_api_common.h"
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppa_hook.h"
#include "ifx_ppa_api_core.h"

#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
#include "ifx_ppa_api_pwm.h"
#include "ifx_ppa_api_pwm_logic.h"
#endif

#include "ifx_ppe_drv_wrapper.h"
#include "../platform/ifx_ppa_ppe_hal.h"
#include "../platform/ifx_ppa_datapath.h"

/*
* exported hook table:
*/
static uint32_t ppa_hook_list_num = 0;
static uint8_t   ppa_hook_list_init_flag = 0;
static struct PPA_HOOK_INFO_LIST *ppa_hook_list = NULL;
static struct PPA_HOOK_INFO_LIST *ppa_hook_list_tail = NULL;
static PPA_LOCK                   ppa_hook_list_lock;

/*** HOOK LIST MAPPING ------------------------------------begin */
int32_t ppa_hook_list_init(void)
{
    if ( ppa_lock_init(&ppa_hook_list_lock) )
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Failed in creating lock for ppa hook map list.\n");
        return IFX_EIO;
    }
    ppa_hook_list_init_flag = 1;

    return IFX_SUCCESS;
}

int32_t get_ppa_hook_list_count(void)
{
    uint32_t n=0;

      if ( !ppa_hook_list_init_flag ) return n;

    ppa_lock_get(&ppa_hook_list_lock);
    n =  ppa_hook_list_num;
    ppa_lock_release(&ppa_hook_list_lock);

    return n;
}

int32_t ppa_add_hook_map( int8_t *name, uint32_t hook_address, uint32_t real_func)
{
    PPA_HOOK_INFO_LIST *tmp;

    if ( !ppa_hook_list_init_flag ) return IFX_FAILURE;

    if( ppa_strlen(name) >= MAX_HOOK_NAME_LEN)
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_add_hook_map fail:  hook name too long\n");
        return IFX_FAILURE;
    }
    tmp= ( PPA_HOOK_INFO_LIST *) ppa_malloc(sizeof(PPA_HOOK_INFO_LIST));
    if( tmp == NULL )
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_add_hook_map:  ppa_malloc fail\n");
        return IFX_FAILURE;
    }
    ppa_memset( (void *)tmp, 0, sizeof(*tmp) );
    tmp->info.hook_addr = hook_address;
    tmp->info.real_func  = real_func;
    tmp->info.hook_flag = 1;
    tmp->next = NULL;
    ppa_strcpy(tmp->info.hookname, name);

    ppa_lock_get(&ppa_hook_list_lock);
    if( ppa_hook_list == NULL )
    {
        ppa_hook_list = tmp;
        ppa_hook_list_tail = tmp;
    }
    else
    {
        ppa_hook_list_tail->next = tmp;
        ppa_hook_list_tail = tmp;
    }

    ppa_hook_list_num++;
    ppa_lock_release(&ppa_hook_list_lock);

    return IFX_SUCCESS;
}

int32_t ppa_enable_hook( int8_t *name, uint32_t enable, uint32_t flag)
{
    PPA_HOOK_INFO_LIST *tmp;
    int32_t res = IFX_FAILURE;

     if ( !ppa_hook_list_init_flag ) return res;

    ppa_lock_get(&ppa_hook_list_lock);
    tmp = ppa_hook_list;
    while( tmp )
    {
        if( strcmp( tmp->info.hookname, name ) == 0 )
        {
            if( enable)
            {
                *( uint32_t * )tmp->info.hook_addr = tmp->info.real_func;
                tmp->info.hook_flag =1;
            }
            else
            {
                *( uint32_t *)tmp->info.hook_addr = 0;
                tmp->info.hook_flag =0;
            }

            res = IFX_SUCCESS;
            break;
        }
        tmp  = tmp ->next;
    }
    ppa_lock_release(&ppa_hook_list_lock);

    return res;
}

int32_t ppa_hook_list_destroy(void)
{
    PPA_HOOK_INFO_LIST *tmp;

    if ( !ppa_hook_list_init_flag ) return IFX_FAILURE;

    ppa_lock_get(&ppa_hook_list_lock);
    while( ppa_hook_list )
    {
            tmp = ppa_hook_list ;
            ppa_hook_list = ppa_hook_list ->next;
            ppa_free(tmp);
    }

    ppa_hook_list_num = 0;
    ppa_hook_list_init_flag = 0;
    ppa_lock_release(&ppa_hook_list_lock);

    ppa_lock_destroy(&ppa_hook_list_lock);
    return IFX_SUCCESS;
}

int32_t ppa_hook_list_start_iteration(uint32_t *ppos,  PPA_HOOK_INFO_LIST **info)
{
    uint32_t l;
    PPA_HOOK_INFO_LIST *p;

     if ( !ppa_hook_list_init_flag ) return IFX_FAILURE;

    ppa_lock_get(&ppa_hook_list_lock);

    for ( p = ppa_hook_list, l = *ppos; p && l; p = p->next, l-- );

    if ( l == 0 && p )
    {
        ++*ppos;
        *info = p;
        return IFX_SUCCESS;
    }
    else
    {
        *info = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_hook_list_iterate_next(uint32_t *ppos, PPA_HOOK_INFO_LIST **info)
{
     if ( !ppa_hook_list_init_flag ) return IFX_FAILURE;

    if ( *info )
    {
        ++*ppos;
        *info = (*info)->next;
        return *info ? IFX_SUCCESS : IFX_FAILURE;
    }
    else
        return IFX_FAILURE;
}

void ppa_hook_list_stop_iteration(void)
{
     if ( !ppa_hook_list_init_flag ) return ;

    ppa_lock_release(&ppa_hook_list_lock);
}

int32_t ppa_ioctl_get_hook_list(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    PPA_HOOK_INFO tmp;
    PPA_HOOK_INFO_LIST *hook_list = NULL;
    uint32_t pos = 0;
    uint32_t i=0;

    if ( ppa_copy_from_user( &cmd_info->hook_list_info, (void *)arg, sizeof(cmd_info->hook_list_info)) != 0 )
        return IFX_FAILURE;

    if( ppa_hook_list_start_iteration( &pos, &hook_list ) == IFX_FAILURE )
    {
        ppa_hook_list_stop_iteration();
        return IFX_FAILURE;
    }

    if( cmd_info->hook_list_info.hook_count <= 0 ) return IFX_SUCCESS;

    while( hook_list &&  i< cmd_info->hook_list_info.hook_count )
    {
        if( ppa_strlen(hook_list->info.hookname) >= sizeof(tmp.hookname) )
        {
            ppa_hook_list_stop_iteration();
            return IFX_FAILURE;
        }

        if ( ppa_copy_to_user( (void *)&(((PPA_CMD_HOOK_LIST_INFO*)arg)->list[i]), &hook_list->info, sizeof(PPA_HOOK_INFO)) != 0 )
        {
            ppa_hook_list_stop_iteration();
            return IFX_FAILURE;
        }

        i++;
        if( ppa_hook_list_iterate_next( &pos, &hook_list) != IFX_SUCCESS  )
            break;
    }

    cmd_info->hook_list_info.hook_count = i;
    if ( ppa_copy_to_user( (void *)arg, &cmd_info->hook_list_info.hook_count, sizeof(cmd_info->hook_list_info.hook_count)) != 0 )
    {
        ppa_hook_list_stop_iteration();
        return IFX_FAILURE;
    }

    ppa_hook_list_stop_iteration();
    return IFX_SUCCESS;
}

int32_t ppa_ioctl_set_hook(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */

    if ( ppa_copy_from_user( &cmd_info->hook_control_info, (void *)arg, sizeof(cmd_info->hook_control_info)) != 0 )
        return IFX_FAILURE;

    return ppa_enable_hook( cmd_info->hook_control_info.hookname, cmd_info->hook_control_info.enable, cmd_info->hook_control_info.flag );
}

/*** HOOK LIST MAPPING ------------------------------------end */

/*** Memory read/write ------------------------------------begin */
/**
* \brief directly read memory address with 4 bytes alignment.
* \param  reg_addr memory address ( it must be 4 bytes alignment )
* \param  shift to the expected bits ( its value is from 0 ~ 31)
* \param  size the bits number ( its value is from 1 ~ 32 ). Note: shift + size <= 32
* \param  repeat repeat time to write the memory.
* \param  new value
* \return on Success return IFX_SUCCESS
*/
int32_t ifx_ppa_api_mem_read(uint32_t reg_addr, uint32_t shift, uint32_t size, uint32_t *buffer)
{
    volatile uint32_t value;
    uint32_t mask=0;

   //read data from specified address
    value = *(uint32_t *)reg_addr;

    //prepare the mask
    if( size > 32 )
        return IFX_FAILURE;
    else if( size == 32 )
        mask=-1;
    else
        mask = ( 1 << size ) - 1 ;

    /* Bit shifting to the exract bit field */
    value = (value >> shift);

    *buffer = (value & mask);
    return IFX_SUCCESS;
}

/**
* \brief directly write memory address with
* \param  reg_addr memory address ( it must be 4 bytes alignment )
* \param  shift to the expected bits ( its value is from 0 ~ 31)
* \param  size the bits number ( its value is from 1 ~ 32 )
* \param  repeat repeat time to write the memory.
* \param  new value
* \return on Success return IFX_SUCCESS
*/
int32_t ifx_ppa_api_mem_write(uint32_t reg_addr, uint32_t shift, uint32_t size, uint32_t value)
{
    volatile uint32_t orgi_value;
    uint32_t mask=0;

    /* Read the Whole 32bit register */
    orgi_value = *(uint32_t *)reg_addr;

    /* Prepare the mask	*/
     if( size > 32 )
        return IFX_FAILURE;
    else if( size == 32 )
        mask=-1;
    else
        mask = ( 1 << size ) - 1 ;
    mask = (mask << shift);

    /* shift the value to the right place and mask the rest of the bit*/
    value = ( value << shift ) & mask;

    /*  Mask out the bit field from the read register and place in the new value */
    value = ( orgi_value & ~mask ) | value ;

    /* Write into register */
    *(uint32_t *)reg_addr = value ;

    return IFX_SUCCESS;
}

int32_t ppa_ioctl_read_mem(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    uint32_t value, i;
    PPA_FPI_ADDR fpi_addr={0};

    if ( ppa_copy_from_user( &cmd_info->read_mem_info, (void *)arg, sizeof(cmd_info->read_mem_info)) != 0 )
        return IFX_FAILURE;

    if( cmd_info->read_mem_info.repeat == 0 ) return IFX_FAILURE;

    fpi_addr.addr_orig = cmd_info->read_mem_info.addr;
    if( ifx_ppa_drv_dp_sb_addr_to_fpi_addr_convert(&fpi_addr, 0) != 0 ) 
    { 
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ifx_ppa_drv_dp_sb_addr_to_fpi_addr_convert failure\n");        
        return IFX_FAILURE;
    }
    cmd_info->read_mem_info.addr_mapped= fpi_addr.addr_fpi;
    if( cmd_info->read_mem_info.addr_mapped % 4 != 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "0x%x_%d not aligned to 4 bytes\n", cmd_info->read_mem_info.addr, cmd_info->read_mem_info.addr_mapped);
        return IFX_FAILURE;
    }
    if ( ppa_copy_to_user( (void *)&(((PPA_CMD_READ_MEM_INFO*)arg)->addr_mapped), &cmd_info->read_mem_info.addr_mapped, sizeof(cmd_info->read_mem_info.addr_mapped)) != 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_copy_to_user failed 1 \n");
        return IFX_FAILURE;
    }

    for(i=0; i<cmd_info->read_mem_info.repeat;  i++ )
    {
        value = 0;
        if ( ifx_ppa_api_mem_read( cmd_info->read_mem_info.addr_mapped + i * 4, cmd_info->read_mem_info.shift, cmd_info->read_mem_info.size, &value ) == IFX_FAILURE )
        {
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ifx_ppa_api_mem_read failed\n");
            return IFX_FAILURE;
        }
        if ( ppa_copy_to_user( (void *)&(((PPA_CMD_READ_MEM_INFO*)arg)->buffer[i]), &value, sizeof(value)) != 0 )
        {
            ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_copy_to_user failed 2\n");
            return IFX_FAILURE;
        }
    }

    return IFX_SUCCESS;
}

int32_t ppa_ioctl_set_mem(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    uint32_t i;
    PPA_FPI_ADDR fpi_addr={0};

    if ( ppa_copy_from_user( &cmd_info->set_mem_info, (void *)arg, sizeof(cmd_info->set_mem_info)) != 0 )
        return IFX_FAILURE;

    fpi_addr.addr_orig = cmd_info->set_mem_info.addr;
    if( ifx_ppa_drv_dp_sb_addr_to_fpi_addr_convert(&fpi_addr, 0) != 0) 
    {        
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ifx_ppa_drv_dp_sb_addr_to_fpi_addr_convert failure\n");        
        return IFX_FAILURE;
    }
    cmd_info->set_mem_info.addr_mapped= fpi_addr.addr_fpi;
    if( cmd_info->set_mem_info.addr_mapped % 4 != 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "0x%x_%d not aligned to 4 bytes\n", cmd_info->set_mem_info.addr_mapped, cmd_info->set_mem_info.addr_mapped);
        return IFX_FAILURE;
    }

    if( cmd_info->set_mem_info.repeat == 0 ) return IFX_SUCCESS;
    for(i=0; i<cmd_info->set_mem_info.repeat;  i++ )
    {
        if( ifx_ppa_api_mem_write( cmd_info->set_mem_info.addr_mapped + i * 4, cmd_info->set_mem_info.shift, cmd_info->set_mem_info.size, cmd_info->set_mem_info.value ) == IFX_FAILURE )
            return IFX_FAILURE;
    }
     if ( ppa_copy_to_user( (void *)&(((PPA_CMD_SET_MEM_INFO*)arg)->addr_mapped), &cmd_info->set_mem_info.addr_mapped, sizeof(cmd_info->set_mem_info.addr_mapped)) != 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_copy_to_user failed \n");
        return IFX_FAILURE;
    }


    return IFX_SUCCESS;
}


/*** Memory read/write -----------------------------------end */

EXPORT_SYMBOL(ppa_hook_list_init);
EXPORT_SYMBOL(get_ppa_hook_list_count);
EXPORT_SYMBOL(ppa_add_hook_map);
EXPORT_SYMBOL(ppa_enable_hook);
EXPORT_SYMBOL(ppa_hook_list_destroy);
EXPORT_SYMBOL(ppa_hook_list_start_iteration);
EXPORT_SYMBOL(ppa_hook_list_iterate_next);
EXPORT_SYMBOL(ppa_hook_list_stop_iteration);
EXPORT_SYMBOL(ppa_ioctl_get_hook_list);
EXPORT_SYMBOL(ppa_ioctl_set_hook);
EXPORT_SYMBOL(ifx_ppa_api_mem_read);
EXPORT_SYMBOL(ifx_ppa_api_mem_write);
EXPORT_SYMBOL(ppa_ioctl_read_mem);
EXPORT_SYMBOL(ppa_ioctl_set_mem);
