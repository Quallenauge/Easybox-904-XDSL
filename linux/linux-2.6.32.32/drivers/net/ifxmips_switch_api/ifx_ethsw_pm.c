/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pm.c
   \remarks implement power management function 
 *****************************************************************************/
#include <ifx_ethsw_pm.h>
#include <ifx_ethsw_pm_plat.h>
#include <ifx_ethsw_pm_pmcu.h>

IFX_ETHSW_PM_CTX_t *gPMCtx[IFX_ETHSW_PM_MODULENR_MAX];
IFX_return_t IFX_ETHSW_PM_ThreadStart(IFX_void_t *pCtx);
IFX_return_t IFX_ETHSW_PM_ThreadStop(IFX_void_t *pCtx);
static int IFX_ETHSW_PM_Polling_Thread(IFX_void_t *pCtx);

#ifdef IFXOS_SUPPORT 
IFX_return_t IFX_ETHSW_Kill_Proc(IFX_int32_t tid);
IFX_boolean_t IFX_ETHSW_Signal_Pending(IFX_void_t *pCtx);
#endif

/**
   This is the power management module function for init

   \param pDevCtx  This parameter is a pointer to the power management device context.
   \param eDev This parameter is a device type.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_void_t *IFX_ETHSW_PM_powerManegementInit(IFX_void_t *pDev, IFX_uint8_t nModuleNr)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx;

    // create control block for pm module
    pPMCtx = (IFX_ETHSW_PM_CTX_t*) IFXOS_BlockAlloc( sizeof (IFX_ETHSW_PM_CTX_t));
    if (pPMCtx == IFX_NULL)
        goto err;

    // initialize the control block
    pPMCtx->pCoreDev = pDev;
    pPMCtx->bPMThreadEnable = IFX_FALSE;

#ifndef IFXOS_SUPPORT 
    pPMCtx->pPMThread = IFX_NULL;
#endif


#ifdef IFX_ETHSW_API_COC_PMCU
    pPMCtx->pPmcuCtx = IFX_ETHSW_PM_PMCU_Init((IFX_void_t*)pPMCtx, nModuleNr);
    if (pPMCtx->pPmcuCtx == IFX_NULL)
        goto err;
#endif

    pPMCtx->pPlatCtx = IFX_ETHSW_PM_PLAT_Init((IFX_void_t*)pPMCtx, nModuleNr);
    if (pPMCtx->pPlatCtx == IFX_NULL)
        goto err;

    // assign control block to global variable
    gPMCtx[nModuleNr] = pPMCtx;

#ifdef IFXOS_SUPPORT 
    IFXOS_ThreadInit(&PHY_ThrCntrl, "PM_Thread", IFX_ETHSW_PM_Polling_Thread, IFX_ETHSW_STACKSIZE, IFX_ETHSW_PRIORITY, pPMCtx, 0 ); 
#else
    // create PHY-SM
    init_waitqueue_head(&pPMCtx->PHY_wait);

    pPMCtx->pPMThread = kthread_create(IFX_ETHSW_PM_Polling_Thread, IFX_NULL, "PM_Thread");

    if (IS_ERR(pPMCtx->pPMThread)) {
        PTR_ERR(pPMCtx->pPMThread);
        goto err;
    }

    //wake_up_process(pPMCtx->pPMThread);
#endif

    return pPMCtx;

err:
#ifdef IFX_ETHSW_API_COC_PMCU
    if (pPMCtx->pPmcuCtx != IFX_NULL)
    {
        IFX_ETHSW_PM_PMCU_CleanUp(pPMCtx->pPmcuCtx);    
        pPMCtx->pPmcuCtx = IFX_NULL;
    }
#endif

    if (pPMCtx != IFX_NULL)
    {
        IFXOS_BlockFree(pPMCtx);
        pPMCtx = IFX_NULL;
    }

    return IFX_NULL;
}

/**
   This is the power management module function for cleanup.

   \param pDevCtx  This parameter is a pointer to the power management device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_PM_powerManegementCleanUp(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

#ifdef IFX_ETHSW_API_COC_PMCU
    if (pPMCtx->pPmcuCtx != IFX_NULL)
    {
        IFX_ETHSW_PM_PMCU_CleanUp(pPMCtx->pPmcuCtx);    
    }
#endif

    if (pPMCtx->pPlatCtx != IFX_NULL)
    {
        IFX_ETHSW_PM_PLAT_CleanUp(pPMCtx->pPlatCtx);    
    }

#ifdef IFXOS_SUPPORT 
    IFX_ETHSW_Kill_Proc(PHY_ThrCntrl.tid);
    IFXOS_ThreadShutdown(&PHY_ThrCntrl, 3000);
#else
    kthread_stop(pPMCtx->pPMThread);
#endif

    // free memory for control block
    if (pPMCtx != IFX_NULL)
    {
        IFXOS_BlockFree(pPMCtx);
        pPMCtx = IFX_NULL;
    }


    return IFX_SUCCESS;
}

/**
   This is the power management module function to get pm status

   \param pDevCtx  This parameter is a pointer to the power management device context.

   \return Return value as follows:
   - IFX_TRUE: if enable
   - IFX_FALSE: if disable
*/
IFX_boolean_t IFX_ETHSW_PM_powerManegementStatusGet(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

    return pPMCtx->bPMThreadEnable;
}

/**
   This is the power management module function to start state machine

   \param pDevCtx  This parameter is a pointer to the power management device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/

IFX_return_t IFX_ETHSW_PM_ThreadStart(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

    wake_up_process(pPMCtx->pPMThread);
    // set bPMThreadEnable
    pPMCtx->bPMThreadEnable = IFX_TRUE;

    return IFX_SUCCESS;
}

/**
   This is the power management module function to stop state machine

   \param pDevCtx  This parameter is a pointer to the power management device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_PM_ThreadStop(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

    pPMCtx->bPMThreadEnable = IFX_FALSE;

    return IFX_SUCCESS;
}


/**
   This is kernel thread for phy in the power management module

   \param pDevCtx  This parameter is a pointer to the power management device context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
static int IFX_ETHSW_PM_Polling_Thread(IFX_void_t *pCtx) 
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = gPMCtx[IFX_ETHSW_PM_MODULENR_TANTOS3G];
    IFX_ETHSW_PMPlatCTX_t *pPlatCtx = (IFX_ETHSW_PMPlatCTX_t *)pPMCtx->pPlatCtx;

    IFX_uint8_t i, long_on = 0;
    IFX_uint8_t MD[PHY_NO];
    IFX_boolean_t PHY_MDStatus;
    IFX_boolean_t PHY_Status;
    IFX_uint8_t nPHYAddr;
    IFX_boolean_t bLinkForce;

    for(;;)
    {
       if (pPMCtx->bPMThreadEnable == IFX_FALSE)
       {
            set_current_state(TASK_INTERRUPTIBLE);
            schedule();
       }

//if (pPMCtx->bPMThreadEnable == IFX_TRUE)
{
       IFX_ETHSW_DEBUG_PRINT(  "==============================\n");
       IFX_ETHSW_DEBUG_PRINT(  "************ DEBUG> long_on=%d\n", long_on);

       // reset MD value
       for (i=0; i<PHY_NO; i++)
       {
           MD[i]=0;
       }

       IFX_ETHSW_DEBUG_PRINT(  "All phy power up\n");

       // wake up all phy
       if (IFX_ETHSW_AllPHY_powerUp(pPMCtx->pCoreDev ,pPMCtx->pPlatCtx) != IFX_SUCCESS)
       {
          IFXOS_PRINT_INT_RAW(  "[%d]: IFX_ETHSW_AllPHY_powerUp error\n",__LINE__);
          return IFX_ERROR;
       }

       IFX_ETHSW_DEBUG_PRINT(  "Start ON_TIMER\n");

       // start ON_TIMER=128ms
#ifdef IFXOS_SUPPORT 
        IFXOS_MSecSleep(128);
        if (IFX_ETHSW_Signal_Pending(current) == IFX_TRUE)
            break;
#else
       interruptible_sleep_on_timeout(&pPMCtx->PHY_wait, 128 * HZ / 1000);
       if (kthread_should_stop())
            break;
#endif
       IFX_ETHSW_DEBUG_PRINT(  "Stop ON_TIMER\n");

       IFX_ETHSW_DEBUG_PRINT(  "Get all PHY status\n");

       // check PHY status
       for( i=0; i<PHY_NO; i++)
       {
           IFX_ETHSW_PM_PLAT_linkForceGet(pPMCtx->pPlatCtx, i,  &bLinkForce);
           if ( bLinkForce == IFX_TRUE )
               continue;

           nPHYAddr = pPlatCtx->PHY[i].nPHYAddr;
           PHY_MDStatus = IFX_ETHSW_PHY_MDstatusGet(pPMCtx->pCoreDev, nPHYAddr);

           IFX_ETHSW_DEBUG_PRINT(  "PHY_MDStatus[%d]=%d\n", i, PHY_MDStatus);

           if ( PHY_MDStatus == IFX_TRUE )
           {
               MD[i]=1;
               if (g_debug == 1)
               {
                    PHY_Status = IFX_ETHSW_PHY_statusGet(pPlatCtx ,i);
                    if ( PHY_Status == IFX_FALSE )
                       IFXOS_PRINT_INT_RAW("PHY[%d]=ON\n", i);
               } 

               // store the MD status for debug
               IFX_ETHSW_PHY_statusSet(pPlatCtx ,i, IFX_TRUE);
           }
           else
           {
               if (g_debug == 1)
               {
                    PHY_Status = IFX_ETHSW_PHY_statusGet(pPlatCtx ,i);
                    if ( PHY_Status == IFX_TRUE )
                       IFXOS_PRINT_INT_RAW("PHY[%d]=OFF\n", i);
               } 
               // store the MD status for debug
               IFX_ETHSW_PHY_statusSet(pPlatCtx ,i, IFX_FALSE);
           }
       }
    
       // power down internal phy 
       for( i=0; i<PHY_NO; i++)
       {
           IFX_ETHSW_PM_PLAT_linkForceGet(pPMCtx->pPlatCtx, i,  &bLinkForce);
           if ( bLinkForce == IFX_TRUE )
               continue;

           if ( (MD[i]==0) && (long_on != i))
           {
               // power down phy
               nPHYAddr = pPlatCtx->PHY[i].nPHYAddr;
               if (IFX_ETHSW_PHY_powerDown(pPMCtx->pCoreDev, nPHYAddr) != IFX_SUCCESS)
               {
                   IFXOS_PRINT_INT_RAW(  "[%d]: IFX_ETHSW_PHY_powerDown[%d] error\n",__LINE__,i);
               }
               IFX_ETHSW_DEBUG_PRINT(  "PHY[%d] power down\n", i);
           }
       }

       // start OFF_TIMER=2048ms
       IFX_ETHSW_DEBUG_PRINT(  "Start OFF_TIMER\n");

#ifdef IFXOS_SUPPORT 
        IFXOS_MSecSleep(2048);
        if (IFX_ETHSW_Signal_Pending(current) == IFX_TRUE)
            break;
#else
       interruptible_sleep_on_timeout(&pPMCtx->PHY_wait, 2048 * HZ / 1000);
       if (kthread_should_stop())
            break;
#endif

       long_on++;
       if ( long_on >= PHY_NO )
           long_on=0;
}
#if 0
else
{
#ifdef IFXOS_SUPPORT 
        IFXOS_MSecSleep(1000);
        if (IFX_ETHSW_Signal_Pending(current) == IFX_TRUE)
            break;
#else
       interruptible_sleep_on_timeout(&pPMCtx->PHY_wait, 1000 * HZ / 1000);
       if (kthread_should_stop())
            break;
#endif

}
#endif
    }

    return IFX_SUCCESS;
}

#ifdef IFXOS_SUPPORT 
/**
   This is kill proc function 

   \param tid  This parameter is a task id.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_Kill_Proc(IFX_int32_t tid)
{
    pid_t pid = (pid_t) tid; 
    kill_proc(pid, SIGTERM, 1);
    return IFX_SUCCESS;
}

/**
   This is signal pending function 

   \param pCtx  This parameter is a pointer to the task_struct context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_boolean_t IFX_ETHSW_Signal_Pending(IFX_void_t *pCtx)
{
    struct task_struct *p = (struct task_struct *)pCtx;
    if (signal_pending(p))
        return IFX_TRUE;
    else 
        return IFX_FALSE;
}
#endif


/**
   This is IFX_ETHSW_powerStateD0 function to De-Activate Power Management
   Switch API terminats the internal thread (if currently running). 
   All PHYs are left enabled to allow link establishment. 

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_return_t IFX_ETHSW_powerStateD0(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

    // stop thread if needed. 
    if (pPMCtx->bPMThreadEnable != IFX_FALSE)
    {
        IFX_ETHSW_PM_ThreadStop(pPMCtx);

        if (IFX_ETHSW_AllPHY_powerUp(pPMCtx->pCoreDev ,pPMCtx->pPlatCtx) != IFX_SUCCESS)
        {
           IFXOS_PRINT_INT_RAW(  "[%d]: IFX_ETHSW_AllPHY_powerDown error\n",__LINE__);
           return IFX_ERROR;
        }
    }

    return IFX_SUCCESS;
}

/**
   This is IFX_ETHSW_powerStateD1 function to Activate Power Management
   Switch API runs an internal thread to poll the PHYs for link state. 
   The PHYs are disabled while no link is detected. 

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_switchDev_t context.

   \return Return the result for calling function.
*/
IFX_return_t IFX_ETHSW_powerStateD1(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

    // start thread if needed. 
    if (pPMCtx->bPMThreadEnable != IFX_TRUE)
    {
        IFX_ETHSW_PM_ThreadStart(pPMCtx);
    }

    return IFX_SUCCESS;
}

IFX_return_t IFX_ETHSW_powerManagementActivated(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

#ifdef IFX_ETHSW_API_COC_PMCU
    if ( IFX_ETHSW_PM_PMCU_StateReq(IFX_PMCU_STATE_D1) != IFX_SUCCESS)
        return IFX_ERROR;
#endif

    IFX_ETHSW_powerStateD1(pPMCtx);

    return IFX_SUCCESS;
}

IFX_return_t IFX_ETHSW_powerManagementDeActivated(IFX_void_t *pCtx)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

#ifdef IFX_ETHSW_API_COC_PMCU
    if ( IFX_ETHSW_PM_PMCU_StateReq(IFX_PMCU_STATE_D0) != IFX_SUCCESS)
        return IFX_ERROR;
#endif

    IFX_ETHSW_powerStateD0(pPMCtx);

    return IFX_SUCCESS;
}

IFX_return_t IFX_ETHSW_PM_linkForceSet(IFX_void_t *pCtx, IFX_uint8_t nPHYIdx, IFX_boolean_t bLinkForce)
{
    IFX_ETHSW_PM_CTX_t *pPMCtx = (IFX_ETHSW_PM_CTX_t *)pCtx;

    IFX_ETHSW_PM_PLAT_linkForceSet(pPMCtx->pPlatCtx, nPHYIdx, bLinkForce);

    return IFX_SUCCESS;
}
