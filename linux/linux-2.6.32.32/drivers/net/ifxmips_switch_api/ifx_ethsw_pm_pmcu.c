/**************************************************************************** Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pm_pmcu.c
   \remarks implement power management abstraction for pmcu module
 *****************************************************************************/
#include <ifx_ethsw_pm.h>
#include <ifx_ethsw_pm_pmcu.h>

IFX_ETHSW_PM_PMCUCtx_t *gPM_pmcuCtx[IFX_ETHSW_PM_MODULENR_MAX];

IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_stateRequest( IFX_PMCU_STATE_t pmcuState);
IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_stateGet( IFX_PMCU_STATE_t *pmcuState );
IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_preCB( IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState);
IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_postCB( IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState);

/**
   This is init function.

   \param pDevCtx  This parameter is a pointer to the IFX_ETHSW_PM_PMCUCtx_t context.
   \param nModuleNr  This parameter is a module number for PMCU module

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_void_t *IFX_ETHSW_PM_PMCU_Init(IFX_void_t *pCtx, IFX_uint8_t nModuleNr)
{
    IFX_ETHSW_PM_PMCUCtx_t *pPmcuCtx;
    IFX_PMCU_REGISTER_t pmcuRegister;

    // allocate memory space.
    pPmcuCtx = (IFX_ETHSW_PM_PMCUCtx_t*) IFXOS_BlockAlloc (sizeof (IFX_ETHSW_PM_PMCUCtx_t));
    pPmcuCtx->pPMCtx = pCtx;
    pPmcuCtx->ePMCU_State = IFX_PMCU_STATE_D0;
    pPmcuCtx->nModuleNr = nModuleNr;

    // assgin to global pointer
    gPM_pmcuCtx[nModuleNr] = pPmcuCtx;

    // register nModuleNr and set callback function
    memset (&pmcuRegister, 0, sizeof(IFX_PMCU_REGISTER_t));
    pmcuRegister.pmcuModule=IFX_PMCU_MODULE_SWITCH;
    pmcuRegister.pmcuModuleNr=nModuleNr;
    pmcuRegister.pmcuModuleDep = IFX_NULL;
    pmcuRegister.ifx_pmcu_state_change=IFX_ETHSW_PM_PMCU_stateRequest; 
    pmcuRegister.ifx_pmcu_state_get=IFX_ETHSW_PM_PMCU_stateGet;
    pmcuRegister.pre=IFX_ETHSW_PM_PMCU_preCB;
    pmcuRegister.post=IFX_ETHSW_PM_PMCU_postCB;
    
    if ( ifx_pmcu_register ( &pmcuRegister ) == IFX_ERROR)
    {
        IFXOS_BlockFree(pPmcuCtx);
        pPmcuCtx = IFX_NULL;
        gPM_pmcuCtx[nModuleNr] = IFX_NULL;
    }

    return pPmcuCtx;
}

/**
   This is a cleanup function.

   \param pDevCtx  This parameter is a pointer to the IFX_ETHSW_PM_PMCUCtx_t context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_PM_PMCU_CleanUp(IFX_void_t *pCtx)
{
    IFX_PMCU_REGISTER_t pmcuUnRegister;
    IFX_ETHSW_PM_PMCUCtx_t *pPmcuCtx = (IFX_ETHSW_PM_PMCUCtx_t *)pCtx;

    // unregister nModuleNr
    pmcuUnRegister.pmcuModule = IFX_PMCU_MODULE_SWITCH; 
    pmcuUnRegister.pmcuModuleNr = pPmcuCtx->nModuleNr;
    if (ifx_pmcu_unregister(&pmcuUnRegister) == IFX_ERROR)
    {
        IFXOS_PRINT_INT_RAW("[%d]: unregister error\n",__LINE__);
        //return IFX_ERROR;
    }

    // kfree
    if (pPmcuCtx != IFX_NULL)
    {
        IFXOS_BlockFree(pPmcuCtx);
        pPmcuCtx = IFX_NULL;
    }

    return IFX_SUCCESS;
}

/**
   This is the callback function to request pmcu state in the 
   power management hardware-dependent module

   \param pmcuState This parameter is a PMCU state.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_stateRequest( IFX_PMCU_STATE_t pmcuState)
{
    IFX_ETHSW_PM_PMCUCtx_t *pPmcuCtx;

    pPmcuCtx = gPM_pmcuCtx[IFX_ETHSW_PM_MODULENR_TANTOS3G];

    switch(pmcuState) 
    {
        case IFX_PMCU_STATE_D0: // De-Activated Power Management
             IFXOS_PRINT_INT_RAW(  "IFX_PMCU_STATE_D0\n");
             IFX_ETHSW_powerStateD0(pPmcuCtx->pPMCtx);
             break;
        case IFX_PMCU_STATE_D1: // Activated Power Management
             IFX_ETHSW_powerStateD1(pPmcuCtx->pPMCtx);
             IFXOS_PRINT_INT_RAW(  "IFX_PMCU_STATE_D1\n");
             break;
        case IFX_PMCU_STATE_D2: // Not Applicable
             IFXOS_PRINT_INT_RAW(  "Not Applicable\n");
             return IFX_PMCU_RETURN_ERROR;
        case IFX_PMCU_STATE_D3: // Not Applicable
             IFXOS_PRINT_INT_RAW(  "Not Applicable\n");
             return IFX_PMCU_RETURN_ERROR;
        default:
             return IFX_PMCU_RETURN_ERROR;
        
    }
    pPmcuCtx->ePMCU_State = pmcuState;

    return IFX_PMCU_RETURN_SUCCESS;
}

/**
   This is the callback function to get pmcu state in the 
   power management hardware-dependent module


   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_stateGet( IFX_PMCU_STATE_t *pmcuState )
{
    IFX_ETHSW_PM_PMCUCtx_t *pPmcuCtx;

    pPmcuCtx = gPM_pmcuCtx[IFX_ETHSW_PM_MODULENR_TANTOS3G];

    *pmcuState = pPmcuCtx->ePMCU_State;
 
    return IFX_PMCU_RETURN_SUCCESS;
}

IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_preCB( IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    switch (pmcuModule)
    {
        case IFX_PMCU_MODULE_CPU:
                break;
        case IFX_PMCU_MODULE_ETH:
                break;
        case IFX_PMCU_MODULE_USB:
                break;
        case IFX_PMCU_MODULE_DSL:
                break;
        case IFX_PMCU_MODULE_WLAN:
                break;
        case IFX_PMCU_MODULE_DECT:
                break;
        case IFX_PMCU_MODULE_FXS:
                break;
        case IFX_PMCU_MODULE_FXO:
                break;
        case IFX_PMCU_MODULE_VE:
                break;
        case IFX_PMCU_MODULE_PPE:
                break;
        case IFX_PMCU_MODULE_SWITCH :
                break;
        case IFX_PMCU_MODULE_UART :
                break;
        case IFX_PMCU_MODULE_SPI :
                break;
        case IFX_PMCU_MODULE_SDIO :
                break;
        case IFX_PMCU_MODULE_PCI :
                break;
        case IFX_PMCU_MODULE_VLYNQ :
                break;
        case IFX_PMCU_MODULE_DEU :
                break;
        case IFX_PMCU_MODULE_CPU_PS :
                break;
        case IFX_PMCU_MODULE_GPTC :
                break;
        case IFX_PMCU_MODULE_USIF_UART :
                break;
        case IFX_PMCU_MODULE_USIF_SPI :
                break;
        case IFX_PMCU_MODULE_PCIE:
                break;
        default:
                return IFX_PMCU_RETURN_ERROR;

    }
    return IFX_PMCU_RETURN_SUCCESS;
}

IFX_PMCU_RETURN_t IFX_ETHSW_PM_PMCU_postCB( IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    switch (pmcuModule)
    {
        case IFX_PMCU_MODULE_CPU:
                break;
        case IFX_PMCU_MODULE_ETH:
                break;
        case IFX_PMCU_MODULE_USB:
                break;
        case IFX_PMCU_MODULE_DSL:
                break;
        case IFX_PMCU_MODULE_WLAN:
                break;
        case IFX_PMCU_MODULE_DECT:
                break;
        case IFX_PMCU_MODULE_FXS:
                break;
        case IFX_PMCU_MODULE_FXO:
                break;
        case IFX_PMCU_MODULE_VE:
                break;
        case IFX_PMCU_MODULE_PPE:
                break;
        case IFX_PMCU_MODULE_SWITCH :
                break;
        case IFX_PMCU_MODULE_UART :
                break;
        case IFX_PMCU_MODULE_SPI :
                break;
        case IFX_PMCU_MODULE_SDIO :
                break;
        case IFX_PMCU_MODULE_PCI :
                break;
        case IFX_PMCU_MODULE_VLYNQ :
                break;
        case IFX_PMCU_MODULE_DEU :
                break;
        case IFX_PMCU_MODULE_CPU_PS :
                break;
        case IFX_PMCU_MODULE_GPTC :
                break;
        case IFX_PMCU_MODULE_USIF_UART :
                break;
        case IFX_PMCU_MODULE_USIF_SPI :
                break;
        case IFX_PMCU_MODULE_PCIE:
                break;
        default:
                return IFX_PMCU_RETURN_ERROR;

    }
    return IFX_PMCU_RETURN_SUCCESS;
}

IFX_return_t IFX_ETHSW_PM_PMCU_StateReq(IFX_PMCU_STATE_t newState)
{
    if (ifx_pmcu_state_req ( IFX_PMCU_MODULE_SWITCH, 0, newState) != IFX_PMCU_RETURN_SUCCESS)
        return IFX_ERROR;

    return IFX_SUCCESS;
}
