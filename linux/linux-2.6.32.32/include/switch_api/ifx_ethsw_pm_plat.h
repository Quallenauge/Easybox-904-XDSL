/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pm_plat.h
   \remarks power management header file for platform dependency. 
 ****************************************************************************/
#ifndef _IFX_ETHSW_PM_PLAT_H_
#define _IFX_ETHSW_PM_PLAT_H_
#include <ifx_ethsw_api.h>

/*********************************************/
/* Structure and Enumeration Type Defintions */
/*********************************************/
#if defined(AR9)
    #define PHY_NO 3
#elif defined(AMAZON_SE)
    #define PHY_NO 4
#elif defined(DANUBE)
    #define PHY_NO 5
#endif


typedef struct {
    IFX_boolean_t               bStatus; // for debug
    IFX_boolean_t               bLinkForce;
    IFX_uint8_t                 nPHYAddr;
} IFX_ETHSW_PHY_t;

typedef struct {
    IFX_void_t                  *pPMCtx;
    IFX_uint8_t                 nPHYNum;
    IFX_ETHSW_PHY_t             PHY[PHY_NO];
} IFX_ETHSW_PMPlatCTX_t;

/************************/
/* Function Propotype   */
/************************/
IFX_void_t *IFX_ETHSW_PM_PLAT_Init(IFX_void_t *pCtx, IFX_uint8_t nModuleNr);
IFX_return_t IFX_ETHSW_PM_PLAT_CleanUp(IFX_void_t *pCtx);
IFX_boolean_t IFX_ETHSW_PHY_MDstatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAddr);
IFX_boolean_t IFX_ETHSW_PHY_statusSet(IFX_void_t *pDevCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t bStatus);
IFX_boolean_t IFX_ETHSW_PHY_statusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPHYIdx);
IFX_return_t IFX_ETHSW_PHY_powerDown(IFX_void_t *pDevCtx, IFX_uint8_t nPHYNum);
IFX_return_t IFX_ETHSW_PHY_powerUp(IFX_void_t *pDevCtx, IFX_uint8_t nPHYNum);
IFX_return_t IFX_ETHSW_AllPHY_powerDown(IFX_void_t *pDevCtx, IFX_void_t *pPlatCtx);
IFX_return_t IFX_ETHSW_AllPHY_powerUp(IFX_void_t *pDevCtx, IFX_void_t *pPlatCtx);
IFX_return_t IFX_ETHSW_PM_PLAT_linkForceSet(IFX_void_t *pPlatCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t bLinkForce);
IFX_return_t IFX_ETHSW_PM_PLAT_linkForceGet(IFX_void_t *pPlatCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t *pLinkForce);

#endif    /* _IFX_ETHSW_PM_PLAT_H_ */
