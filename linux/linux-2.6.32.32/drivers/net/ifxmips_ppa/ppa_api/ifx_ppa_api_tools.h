#ifndef __IFX_PPA_API_TOOLS_H__20100318_1920__
#define __IFX_PPA_API_TOOLS_H__20100318_1920__

/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_toos.h
** PROJECT      : PPA
** MODULES      : PPA API (Tools APIs)
**
** DATE         : 18 March 2010
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Protocol Stack Tools API Implementation Header File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 18 March 2010 Shao Guohua        Initiate Version
*******************************************************************************/
/*! \file ifx_ppa_api_core.h
    \brief This file contains es.
           provide PPA API.
*/

/** \addtogroup PPA_CORE_API PPA Core API
    \brief  PPA Core API provide PPA core accleration logic and API
            The API is defined in the following two source files
            - ifx_ppa_api_core.h: Header file for PPA API
            - ifx_ppa_api_core.c: C Implementation file for PPA API
*/
/* @{ */


/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *              Data Type
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */
extern int32_t ppa_hook_list_init(void);
extern int32_t ppa_hook_list_destroy(void);
extern int32_t ppa_add_hook_map( int8_t *name, uint32_t hook_address, uint32_t real_func);
extern int32_t get_ppa_hook_list_count(void);
extern int32_t ppa_hook_list_start_iteration(uint32_t *ppos, PPA_HOOK_INFO_LIST **info);
extern int32_t ppa_hook_list_iterate_next(uint32_t *ppos, PPA_HOOK_INFO_LIST **info);
extern void ppa_hook_list_stop_iteration(void);
extern int32_t ppa_enable_hook( int8_t *name, uint32_t enable, uint32_t flag);

extern int32_t ppa_ioctl_set_hook(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_get_hook_list(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_read_mem(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_set_mem(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);

#endif  //  __IFX_PPA_API_CORE_H__20081103_1920__
/* @} */

