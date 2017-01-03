#ifndef __IFX_PPA_API_COMMON_H__20100203__1740__
#define __IFX_PPA_API_COMMON_H__20100203__1740__

/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_common.h
** PROJECT      : PPA
** MODULES      : PPA Common header file
**
** DATE         : 3 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Common Header File
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

#define NO_DOXY                 1

 /*force dynamic ppe driver's module parameter */
#define IFX_PPA_DP_DBG_PARAM_ENABLE  1   //for PPA automation purpose. for non-linux os porting, just disable it

#if IFX_PPA_DP_DBG_PARAM_ENABLE
    extern int ifx_ppa_drv_dp_dbg_param_enable;
    extern int  ifx_ppa_drv_dp_dbg_param_ethwan;
    extern int ifx_ppa_drv_dp_dbg_param_wanitf;
    extern int ifx_ppa_drv_dp_dbg_param_ipv6_acc_en;
    extern int ifx_ppa_drv_dp_dbg_param_wanqos_en;
#endif // end of IFX_PPA_DP_DBG_PARAM_ENABLE

#endif

