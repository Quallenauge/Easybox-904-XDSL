#ifndef __IFX_PPA_API_CORE_H__20081103_1920__
#define __IFX_PPA_API_CORE_H__20081103_1920__



/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_core.h
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 3 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Implementation Header File
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

int32_t ifx_ppa_is_init(void);



#endif  //  __IFX_PPA_API_CORE_H__20081103_1920__
/* @} */
