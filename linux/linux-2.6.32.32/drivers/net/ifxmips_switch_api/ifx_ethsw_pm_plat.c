/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pm_plat.c
   \remarks implement power management platform dependency code
 *****************************************************************************/
#include <ifx_ethsw_pm.h>
#include <ifx_ethsw_pm_plat.h>
#include <ifx_ethsw_PSB6970_core.h>

IFX_ETHSW_PMPlatCTX_t *gPMPlatCtx[IFX_ETHSW_PM_MODULENR_MAX];
/**
   This is init function in the power management hardware-dependent module.

   \param pDevCtx  This parameter is a pointer to the IFX_ETHSW_PMPlatCTX_t context.
   \param nModuleNr  This parameter is a module number for PMCU module

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_void_t *IFX_ETHSW_PM_PLAT_Init(IFX_void_t *pCtx, IFX_uint8_t nModuleNr)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx;
    IFX_uint8_t i;
    
    pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t*) IFXOS_BlockAlloc (sizeof (IFX_ETHSW_PMPlatCTX_t));
    if ( pPMPlatCtx == IFX_NULL)
        return IFX_NULL;

    // init Tantos3G on Ar9 platform
    pPMPlatCtx->pPMCtx = pCtx;
    pPMPlatCtx->nPHYNum = PHY_NO;
#if defined (AR9)
    pPMPlatCtx->PHY[0].nPHYAddr = 0;
    pPMPlatCtx->PHY[1].nPHYAddr = 1;
    pPMPlatCtx->PHY[2].nPHYAddr = 2;
    //pPMPlatCtx->PHY[3].nPHYAddr = 4; // Power management doesn't support external phy
#elif defined (AMAZON_SE)
    pPMPlatCtx->PHY[0].nPHYAddr = 0;
    pPMPlatCtx->PHY[1].nPHYAddr = 1;
    pPMPlatCtx->PHY[2].nPHYAddr = 2;
    pPMPlatCtx->PHY[3].nPHYAddr = 3;
#elif defined (DANUBE)
    pPMPlatCtx->PHY[0].nPHYAddr = 0;
    pPMPlatCtx->PHY[1].nPHYAddr = 1;
    pPMPlatCtx->PHY[2].nPHYAddr = 2;
    pPMPlatCtx->PHY[3].nPHYAddr = 3;
    pPMPlatCtx->PHY[4].nPHYAddr = 4;
#endif
    for (i=0; i<PHY_NO; i++)
    {
        pPMPlatCtx->PHY[i].bStatus = IFX_TRUE;
        pPMPlatCtx->PHY[i].bLinkForce = IFX_FALSE;
    }

    gPMPlatCtx[nModuleNr] = pPMPlatCtx;

    return pPMPlatCtx;

}

/**
   This is cleanup function in the power management hardware-dependent module.

   \param pDevCtx  This parameter is a pointer to the IFX_ETHSW_PMPlatCTX_t context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_PM_PLAT_CleanUp(IFX_void_t *pCtx)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pCtx;

    // kfree
    if (pPMPlatCtx != IFX_NULL)
    {
        IFXOS_BlockFree(pPMPlatCtx);
        pPMPlatCtx = IFX_NULL;
    }

    return IFX_SUCCESS;
}
/**
   This is IFX_ETHSW_PHY_MDstatusGet function to call switch core layer.

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_boolean_t IFX_ETHSW_PHY_MDstatusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAddr)
{
    IFX_boolean_t Status;

    // The Tantos CoC feature is only dedicated to the Tantos internal PHYs
    //if ( nPHYAddr == 4 ) // this is a external PHY.
    //    Status = IFX_PSB6970_PHY_linkStatusGet(pDevCtx, nPHYAddr);
    //else 
        Status = IFX_PSB6970_PHY_mediumDetectStatusGet(pDevCtx, nPHYAddr);

    return Status;
}

/**
   This is IFX_ETHSW_PHY_statusSet function to store MD status into PMPlatCtx structure

   \param pDevCtx  This parameter is a pointer to the IFX_ETHSW_PMPlatCTX_t context.

   \param nPHYIdx  This parameter is PHY index.

   \return Return the result for calling function.
*/
IFX_boolean_t IFX_ETHSW_PHY_statusSet(IFX_void_t *pDevCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t bStatus)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pDevCtx;

    pPMPlatCtx->PHY[nPHYIdx].bStatus = bStatus;

    return IFX_SUCCESS;
}

/**
   This is IFX_ETHSW_PHY_statusGet function to store MD status into PMPlatCtx structure

   \param pDevCtx  This parameter is a pointer to the IFX_ETHSW_PMPlatCTX_t context.

   \param nPHYIdx  This parameter is PHY index.

   \return Return the result for calling function.
*/
IFX_boolean_t IFX_ETHSW_PHY_statusGet(IFX_void_t *pDevCtx, IFX_uint8_t nPHYIdx)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pDevCtx;

    return pPMPlatCtx->PHY[nPHYIdx].bStatus;
}

/**
   This is IFX_ETHSW_PHY_powerDown function to call switch core layer.

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_return_t IFX_ETHSW_PHY_powerDown(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAddr)
{
    return IFX_PSB6970_PHY_PDN_Set(pDevCtx, nPHYAddr);
}

/**
   This is IFX_ETHSW_PHY_powerUp function to call switch core layer.

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_return_t IFX_ETHSW_PHY_powerUp(IFX_void_t *pDevCtx, IFX_uint8_t nPHYAddr)
{
    return IFX_PSB6970_PHY_PDN_Clear(pDevCtx, nPHYAddr);
}

/**
   This is IFX_ETHSW_AllPHY_powerDown function to call switch core layer.

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_return_t IFX_ETHSW_AllPHY_powerDown(IFX_void_t *pDevCtx, IFX_void_t *pPlatCtx)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pPlatCtx;
    IFX_uint8_t i;
    for (i=0; i<pPMPlatCtx->nPHYNum; i++)
    {
       if (pPMPlatCtx->PHY[i].bLinkForce == IFX_TRUE)
            continue;

       if (IFX_PSB6970_PHY_PDN_Set(pDevCtx ,pPMPlatCtx->PHY[i].nPHYAddr) != IFX_SUCCESS)
       {
           IFXOS_PRINT_INT_RAW(  "[%d]: IFX_ETHSW_PHY_powerDown[%d] error\n",__LINE__,i);
           return IFX_ERROR;
       }
       IFX_ETHSW_DEBUG_PRINT("PHY[%d] power down\n", i);
    }

    return IFX_SUCCESS;
}

/**
   This is IFX_ETHSW_AllPHY_powerUp function to call switch core layer.

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_return_t IFX_ETHSW_AllPHY_powerUp(IFX_void_t *pDevCtx, IFX_void_t *pPlatCtx)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pPlatCtx;
    IFX_uint8_t i;

    for (i=0; i<pPMPlatCtx->nPHYNum; i++)
    {
       if (pPMPlatCtx->PHY[i].bLinkForce == IFX_TRUE)
            continue;

       if (IFX_PSB6970_PHY_PDN_Clear(pDevCtx ,pPMPlatCtx->PHY[i].nPHYAddr) != IFX_SUCCESS)
       {
           IFXOS_PRINT_INT_RAW(  "[%d]: IFX_ETHSW_PHY_powerUp[%d] error\n",__LINE__,i);
           return IFX_ERROR;
       }
       IFX_ETHSW_DEBUG_PRINT("PHY[%d] power up\n", i);
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_ETHSW_PM_PLAT_linkForceSet(IFX_void_t *pPlatCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t bLinkForce)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pPlatCtx;

    pPMPlatCtx->PHY[nPHYIdx].bLinkForce = bLinkForce;
    
    return IFX_SUCCESS;
}

IFX_return_t IFX_ETHSW_PM_PLAT_linkForceGet(IFX_void_t *pPlatCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t *pLinkForce)
{
    IFX_ETHSW_PMPlatCTX_t *pPMPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pPlatCtx;

    *pLinkForce = pPMPlatCtx->PHY[nPHYIdx].bLinkForce;
    
    return IFX_SUCCESS;
}

