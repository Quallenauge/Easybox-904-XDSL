/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pm_pmcu.h
   \remarks power management header file 
 ****************************************************************************/
#ifndef _IFX_ETHSW_PM_PMCU_H_
#define _IFX_ETHSW_PM_PMCU_H_

#include <ifx_ethsw_api.h>
#include <ifx_pmcu.h>

/*********************************************/
/* Structure and Enumeration Type Defintions */
/*********************************************/
typedef struct {
    IFX_void_t          *pPMCtx;
	IFX_PMCU_STATE_t 	ePMCU_State;
	IFX_uint8_t 		nModuleNr;
} IFX_ETHSW_PM_PMCUCtx_t;


/************************/
/* Function Declaration */
/************************/
IFX_void_t *IFX_ETHSW_PM_PMCU_Init(IFX_void_t *pCtx, IFX_uint8_t nModuleNr);
IFX_return_t IFX_ETHSW_PM_PMCU_CleanUp(IFX_void_t *pCtx);
IFX_return_t IFX_ETHSW_PM_PMCU_StateReq(IFX_PMCU_STATE_t newState);

#endif    /* _IFX_ETHSW_PM_PMCU_H_ */
