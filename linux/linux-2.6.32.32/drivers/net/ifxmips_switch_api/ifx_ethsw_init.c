/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_init.c
   \remarks implement SWITCH API initialization routine.
 *****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <asm/ifx/ifx_pmu.h>
#include <ifx_ethsw_init.h>

#define IFX_ETHSW_MAJOR_NUMBER 81
#define INTERNL_SWITCH		0
#define EXTERNL_SWITCH		1

#ifdef AR9
extern IFX_ETHSW_lowLevelFkts_t ifx_psb6970_fkt_tbl;
/*  AR9 board containing no external TANTOS-3G device */
int IFX_ETHSW_Init_AR9(void)
{
	IFX_return_t result;
    IFX_PSB6970_RAL_Init_t RAL_Init;
    IFX_PSB6970_RAL_Dev_t *pRALDevAR9;
    IFX_PSB6970_RML_Init_t RML_Init;
    IFX_PSB6970_RML_Dev_t *pRMLDevAR9;
    IFX_PSB6970_switchCoreInit_t SwitchCoreInit;
    IFX_PSB6970_switchDev_t *pEthSWDevAR9;
    IFX_ETHSW_IOCTL_wrapperInit_t Ioctl_Wrapper_Init;
    IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlWrapper;

	/* Register Char Device */
	result =  IFX_ETHSW_Drv_Register(IFX_ETHSW_MAJOR_NUMBER);
	if (result != IFX_SUCCESS ) {
		IFXOS_PRINT_INT_RAW("Register Char Device Fail!\n");
		return result;
	}
	/* Init PSB6970 RAL Layer */
	RAL_Init.eDev = IFX_PSB6970_DEV_AR9;
	pRALDevAR9 = IFX_PSB6970_RAL_init(&RAL_Init);
	if ( pRALDevAR9 == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RAL Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRALDevAR9;
    }
	/* Init PSB6970 RML Layer */
	RML_Init.register_read = pRALDevAR9->register_read;
	RML_Init.register_write = pRALDevAR9->register_write;
	RML_Init.pRegAccessHandle = pRALDevAR9;
	RML_Init.eDev = IFX_PSB6970_DEV_AR9;
	pRMLDevAR9 = IFX_PSB6970_RML_init(&RML_Init);
	if ( pRMLDevAR9 == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRMLDevAR9;
    }
	/* Init PSB6970 Switch Core Layer */
	SwitchCoreInit.eDev = IFX_PSB6970_DEV_AR9;
	SwitchCoreInit.pDev = pRMLDevAR9;
	pEthSWDevAR9 = IFX_PSB6970_SwitchCoreInit(&SwitchCoreInit);
	if (pEthSWDevAR9 == NULL) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
	/* Init Linux Wrapper Layer */
	Ioctl_Wrapper_Init.pLlTable = &ifx_psb6970_fkt_tbl;
	pIoctlWrapper = IFX_ETHSW_IOCTL_WrapperInit(&Ioctl_Wrapper_Init);
	if ( pIoctlWrapper == NULL ) {
		IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (WrapperInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
	/* Add internal Switch */
	IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevAR9, INTERNL_SWITCH);
	return IFX_SUCCESS;
}

/*  AR9 board containing no external TANTOS-3G device */
int IFX_ETHSW_CleanUp_AR9(void)
{
	/* Free the device data block */
	IFX_ETHSW_Drv_UnRegister(IFX_ETHSW_MAJOR_NUMBER);
	IFX_PSB6970_SwitchCoreCleanUP();
	IFX_ETHSW_IOCTL_WrapperCleanUp();
	return IFX_SUCCESS;
}

/*  AR9 board containing one external TANTOS-3G device (over MDIO) */
int IFX_ETHSW_Init_TANTOS(void)
{
	IFX_return_t result;
	IFX_PSB6970_RAL_Init_t RAL_Init;
	IFX_PSB6970_RAL_Dev_t *pRALDevT3G;
	IFX_PSB6970_RML_Init_t RML_Init;
	IFX_PSB6970_RML_Dev_t *pRMLDevT3G;
	IFX_PSB6970_switchCoreInit_t SwitchCoreInit;
	IFX_PSB6970_switchDev_t *pEthSWDevT3G;
	IFX_ETHSW_IOCTL_wrapperInit_t Ioctl_Wrapper_Init;
	IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlWrapper;
	
	/* Register Char Device */
	result =  IFX_ETHSW_Drv_Register(IFX_ETHSW_MAJOR_NUMBER);
	if ( result != IFX_SUCCESS ) {
		IFXOS_PRINT_INT_RAW("Register Char Device Fail!\n");
		return result;
	}
	
    /* Init PSB6970 RAL Layer */
    RAL_Init.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    pRALDevT3G = IFX_PSB6970_RAL_init(&RAL_Init);
    if ( pRALDevT3G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RAL Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRALDevT3G;
    }
    /* Init PSB6970 RML Layer */
    RML_Init.register_read = pRALDevT3G->register_read;
    RML_Init.register_write = pRALDevT3G->register_write;
    RML_Init.pRegAccessHandle = pRALDevT3G;
    RML_Init.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    pRMLDevT3G = IFX_PSB6970_RML_init(&RML_Init);
    if ( pRMLDevT3G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRMLDevT3G;
    }
    /* Init PSB6970 Switch Core Layer */
    SwitchCoreInit.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    SwitchCoreInit.pDev = pRMLDevT3G;
    pEthSWDevT3G = IFX_PSB6970_SwitchCoreInit(&SwitchCoreInit);
    if (pEthSWDevT3G == NULL) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (CoreInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /*  Init Linux Wrapper Layer */
    Ioctl_Wrapper_Init.pLlTable = &ifx_psb6970_fkt_tbl;
    pIoctlWrapper = IFX_ETHSW_IOCTL_WrapperInit(&Ioctl_Wrapper_Init);
    if ( pIoctlWrapper == NULL ) {
		IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (WrapperInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* add external switch */
    IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevT3G, EXTERNL_SWITCH);
    return IFX_SUCCESS;
}

/*  AR9 board containing one external TANTOS-3G device (over MDIO) */
int IFX_ETHSW_CleanUp_TANTOS(void)
{
	/* Free the device data block */
	IFX_ETHSW_Drv_UnRegister(IFX_ETHSW_MAJOR_NUMBER);
	IFX_PSB6970_SwitchCoreCleanUP();
	IFX_ETHSW_IOCTL_WrapperCleanUp();
	return IFX_SUCCESS;
}

/*  AR9 board containing one internal switch and one external TANTOS-3G device (over MDIO) */
int IFX_ETHSW_Init_AR9_TANTOS(void)
{
	IFX_return_t result;
    IFX_PSB6970_RAL_Init_t RAL_Init;
    IFX_PSB6970_RAL_Dev_t *pRALDevAR9, *pRALDevT3G;
    IFX_PSB6970_RML_Init_t RML_Init;
    IFX_PSB6970_RML_Dev_t *pRMLDevAR9, *pRMLDevT3G;
    IFX_PSB6970_switchCoreInit_t SwitchCoreInit;
    IFX_PSB6970_switchDev_t *pEthSWDevAR9, *pEthSWDevT3G;
    IFX_ETHSW_IOCTL_wrapperInit_t Ioctl_Wrapper_Init;
    IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlWrapper;
    
    /* Register Char Device */
    result =  IFX_ETHSW_Drv_Register(IFX_ETHSW_MAJOR_NUMBER);
    if ( result != IFX_SUCCESS ) {
    	IFXOS_PRINT_INT_RAW("Register Char Device Fail!\n");
    	return result;
    }
    /* Enable Switch Module */
    SWITCH_PMU_SETUP(IFX_PMU_ENABLE);
    /* Init PSB6970 RAL Layer */
    RAL_Init.eDev = IFX_PSB6970_DEV_AR9;
    pRALDevAR9 = IFX_PSB6970_RAL_init(&RAL_Init);
    if ( pRALDevAR9 == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RAL Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRALDevAR9;
    }
    RAL_Init.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    pRALDevT3G = IFX_PSB6970_RAL_init(&RAL_Init);
    if ( pRALDevT3G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RAL Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRALDevT3G;
    }
    /* Init PSB6970 RML Layer */
    RML_Init.register_read = pRALDevAR9->register_read;
    RML_Init.register_write = pRALDevAR9->register_write;
    RML_Init.pRegAccessHandle = pRALDevAR9;
    RML_Init.eDev = IFX_PSB6970_DEV_AR9;
    pRMLDevAR9 = IFX_PSB6970_RML_init(&RML_Init);
    if ( pRMLDevAR9 == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRMLDevAR9;
    }
    RML_Init.register_read = pRALDevT3G->register_read;
    RML_Init.register_write = pRALDevT3G->register_write;
    RML_Init.pRegAccessHandle = pRALDevT3G;
    RML_Init.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    pRMLDevT3G = IFX_PSB6970_RML_init(&RML_Init);
    if ( pRMLDevT3G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (int)pRMLDevT3G;
    }
    /* Init PSB6970 Switch Core Layer */
    SwitchCoreInit.eDev = IFX_PSB6970_DEV_AR9;
    SwitchCoreInit.pDev = pRMLDevAR9;
    pEthSWDevAR9 = IFX_PSB6970_SwitchCoreInit(&SwitchCoreInit);
    SwitchCoreInit.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    SwitchCoreInit.pDev = pRMLDevT3G;
    pEthSWDevT3G = IFX_PSB6970_SwitchCoreInit(&SwitchCoreInit);
    if (pEthSWDevT3G == NULL) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* Init Linux Wrapper Layer */
    Ioctl_Wrapper_Init.pLlTable = &ifx_psb6970_fkt_tbl;
    pIoctlWrapper = IFX_ETHSW_IOCTL_WrapperInit(&Ioctl_Wrapper_Init);
    if ( pIoctlWrapper == NULL ) {
		IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (WrapperInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* add internal switch */
    IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevAR9, INTERNL_SWITCH );
    /* add external switch */
    IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevT3G, EXTERNL_SWITCH );
    return IFX_SUCCESS;
}

/*  AR9 board containing one internal switch and one external TANTOS-3G device (over MDIO) */
int IFX_ETHSW_CleanUp_AR9_TANTOS(void)
{
	/* Free the device data block */
    IFX_ETHSW_Drv_UnRegister(IFX_ETHSW_MAJOR_NUMBER);
    IFX_PSB6970_SwitchCoreCleanUP();
    IFX_ETHSW_IOCTL_WrapperCleanUp();
    return IFX_SUCCESS;
}
#endif

#ifdef DANUBE
extern IFX_ETHSW_lowLevelFkts_t ifx_psb6970_fkt_tbl;
/*  Danube board containing one external TANTOS-3G device */
int IFX_ETHSW_Init_DANUBE(void)
{
    IFX_return_t result;
    IFX_PSB6970_RAL_Init_t RAL_Init;
    IFX_PSB6970_RAL_Dev_t *pRALDevT3G;
    IFX_PSB6970_RML_Init_t RML_Init;
    IFX_PSB6970_RML_Dev_t *pRMLDevT3G;
    IFX_PSB6970_switchCoreInit_t SwitchCoreInit;
    IFX_PSB6970_switchDev_t *pEthSWDevT3G;
    IFX_ETHSW_IOCTL_wrapperInit_t Ioctl_Wrapper_Init;
    IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlWrapper;

    /*  Register Char Device */
    result =  IFX_ETHSW_Drv_Register(IFX_ETHSW_MAJOR_NUMBER);
    if ( result != IFX_SUCCESS ) {
    	IFXOS_PRINT_INT_RAW("Register Char Device Fail!\n");
        return result;
    }
    /*  Init PSB6970 RAL Layer */
    RAL_Init.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    pRALDevT3G = IFX_PSB6970_RAL_init(&RAL_Init);
    if ( pRALDevT3G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RAL Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (pRALDevT3G);
    }
    /* Init PSB6970 RML Layer */
    RML_Init.register_read = pRALDevT3G->register_read;
    RML_Init.register_write = pRALDevT3G->register_write;
    RML_Init.pRegAccessHandle = pRALDevT3G;
    RML_Init.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    pRMLDevT3G = IFX_PSB6970_RML_init(&RML_Init);
    if ( pRMLDevT3G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (pRMLDevT3G);
    }
    /* Init PSB6970 Switch Core Layer */
    SwitchCoreInit.eDev = IFX_PSB6970_DEV_TANTOS_3G;
    SwitchCoreInit.pDev = pRMLDevT3G;
    pEthSWDevT3G = IFX_PSB6970_SwitchCoreInit(&SwitchCoreInit);
    if (pEthSWDevT3G == NULL) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* Init Linux Wrapper Layer */
    Ioctl_Wrapper_Init.pLlTable = &ifx_psb6970_fkt_tbl;
    pIoctlWrapper = IFX_ETHSW_IOCTL_WrapperInit(&Ioctl_Wrapper_Init);
    if ( pIoctlWrapper == NULL ) {
		IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (WrapperInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
     /* add external switch */
    IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevT3G, EXTERNL_SWITCH);
    return IFX_SUCCESS;
}

/*  Danube board containing one external TANTOS-3G device */
int IFX_ETHSW_CleanUp_DANUBE(void)
{
	/* Free the device data block */
    IFX_ETHSW_Drv_UnRegister(IFX_ETHSW_MAJOR_NUMBER);
    IFX_PSB6970_SwitchCoreCleanUP();
    IFX_ETHSW_IOCTL_WrapperCleanUp();
    return IFX_SUCCESS;
}
#endif

#ifdef AMAZON_SE
extern IFX_ETHSW_lowLevelFkts_t ifx_psb6970_fkt_tbl;
/*  Amazon_SE board containing one external TANTOS-0G device */
int IFX_ETHSW_Init_AMAZON_SE(void)
{
    IFX_return_t result;
    IFX_PSB6970_RAL_Init_t RAL_Init;
    IFX_PSB6970_RAL_Dev_t *pRALDevT0G;
    IFX_PSB6970_RML_Init_t RML_Init;
    IFX_PSB6970_RML_Dev_t *pRMLDevT0G;
    IFX_PSB6970_switchCoreInit_t SwitchCoreInit;
    IFX_PSB6970_switchDev_t *pEthSWDevT0G;
    IFX_ETHSW_IOCTL_wrapperInit_t Ioctl_Wrapper_Init;
    IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlWrapper;

    /* Register Char Device */
    result =  IFX_ETHSW_Drv_Register(IFX_ETHSW_MAJOR_NUMBER);
    if ( result != IFX_SUCCESS ) {
        IFXOS_PRINT_INT_RAW("Register Char Device Fail!\n");
        return result;
    }
	
    /* Init PSB6970 RAL Layer */
    RAL_Init.eDev = IFX_PSB6970_DEV_TANTOS_0G;
    pRALDevT0G = IFX_PSB6970_RAL_init(&RAL_Init);
    if ( pRALDevT0G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RAL Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (pRALDevT0G);
    }
    /* Init PSB6970 RML Layer */
    RML_Init.register_read = pRALDevT0G->register_read;
    RML_Init.register_write = pRALDevT0G->register_write;
    RML_Init.pRegAccessHandle = pRALDevT0G;
    RML_Init.eDev = IFX_PSB6970_DEV_TANTOS_0G;
    pRMLDevT0G = IFX_PSB6970_RML_init(&RML_Init);
    if ( pRMLDevT0G == IFX_NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return (pRMLDevT0G);
    }
    /* Init PSB6970 Switch Core Layer */
    SwitchCoreInit.eDev = IFX_PSB6970_DEV_TANTOS_0G;
    SwitchCoreInit.pDev = pRMLDevT0G;
    pEthSWDevT0G = IFX_PSB6970_SwitchCoreInit(&SwitchCoreInit);
    if (pEthSWDevT0G == NULL) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* Init Linux Wrapper Layer */
    Ioctl_Wrapper_Init.pLlTable = &ifx_psb6970_fkt_tbl;
    pIoctlWrapper = IFX_ETHSW_IOCTL_WrapperInit(&Ioctl_Wrapper_Init);
    if ( pIoctlWrapper == NULL ) {
		IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (WrapperInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* add external switch */
    IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevT0G, EXTERNL_SWITCH); 
    return IFX_SUCCESS;
}

/*  Amazon_SE board containing one external TANTOS-0G device */
int IFX_ETHSW_CleanUp_AMAZON_SE(void)
{
    /* Free the device data block */
    IFX_ETHSW_Drv_UnRegister(IFX_ETHSW_MAJOR_NUMBER);
    IFX_PSB6970_SwitchCoreCleanUP();
    IFX_ETHSW_IOCTL_WrapperCleanUp();
    return IFX_SUCCESS;
}
#endif

#ifdef VR9
extern IFX_ETHSW_lowLevelFkts_t ifx_flow_fkt_tbl;

int IFX_ETHSW_Init_VR9(void)
{
    IFX_return_t result;
    IFX_FLOW_RAL_Init_t RAL_Init;
    IFX_FLOW_RAL_Dev_t *pRALDev;
    IFX_FLOW_switchCoreInit_t SwitchCoreInit;
    IFX_FLOW_switchDev_t *pEthSWDevVR9;    
    IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlWrapper;
    IFX_ETHSW_IOCTL_wrapperInit_t Ioctl_Wrapper_Init;

    /* Register Char Device */
    result =  IFX_ETHSW_Drv_Register(IFX_ETHSW_MAJOR_NUMBER);
    if ( result != IFX_SUCCESS ) {
    	IFXOS_PRINT_INT_RAW("Register Char Device Fail!\n");
    	return result;
    }
    /* PMU Enable */
	PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
	PPE_DPLUS_PMU_SETUP(IFX_PMU_ENABLE);
	PPE_DPLUSS_PMU_SETUP(IFX_PMU_ENABLE);
	SWITCH_PMU_SETUP(IFX_PMU_ENABLE);
    /* Init FLOW RAL Layer */
    RAL_Init.eDev = IFX_FLOW_DEV_INT;
    pRALDev = IFX_FLOW_RAL_init(&RAL_Init);
    /* Init FLOW Switch Core Layer */
    SwitchCoreInit.eDev = IFX_FLOW_DEV_INT;
    SwitchCoreInit.pDev = pRALDev;
    pEthSWDevVR9 = IFX_FLOW_SwitchCoreInit(&SwitchCoreInit);
    if (pEthSWDevVR9 == NULL) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (Init Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    pEthSWDevVR9->nCPU_Port = VRX_CPU_PORT;
    Ioctl_Wrapper_Init.pLlTable = &ifx_flow_fkt_tbl;
    pIoctlWrapper = IFX_ETHSW_IOCTL_WrapperInit(&Ioctl_Wrapper_Init);
    if ( pIoctlWrapper == NULL ) {
    	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (WrapperInit Failed) \n", __FILE__, __FUNCTION__, __LINE__);
    	return IFX_ERROR;
    }
    /* add Internal switch */
    IFX_ETHSW_IOCTL_WrapperDeviceAdd(pIoctlWrapper, pEthSWDevVR9, INTERNL_SWITCH);
    return IFX_SUCCESS;
}

int IFX_ETHSW_CleanUp_VR9(void)
{
    /* Free the device data block */
    IFX_ETHSW_Drv_UnRegister(IFX_ETHSW_MAJOR_NUMBER);
    IFX_FLOW_SwitchCoreCleanUP();
    IFX_ETHSW_IOCTL_WrapperCleanUp();
    return IFX_SUCCESS;
}
#endif

/*
 * IFX_ETHSW_Switch_API_Init  the init function, called when the module is loaded.
 * Returns zero if successfully loaded, nonzero otherwise.
 */
static int __init IFX_ETHSW_Switch_API_Init(void)
{
    /* Print Version Number */
#ifdef IFXOS_SUPPORT
    IFXOS_PRINT_INT_RAW("IFX SWITCH API, Version %s.\n", SWITCH_API_DRIVER_VERSION);
#else
    printk("IFX SWITCH API, Version %s\n", SWITCH_API_DRIVER_VERSION);
#endif
#ifdef AR9
    IFX_ETHSW_Init_AR9_TANTOS();
#endif
#ifdef DANUBE
    IFX_ETHSW_Init_DANUBE();
#endif
#ifdef AMAZON_SE
    IFX_ETHSW_Init_AMAZON_SE();
#endif
#ifdef VR9
    IFX_ETHSW_Init_VR9();
#endif
    return IFX_SUCCESS;
}

/*
 * IFX_ETHSW_Switch_API_Exit  the exit function, called when the module is removed.
 */
static void __exit IFX_ETHSW_Switch_API_Exit(void)
{
#ifdef AR9
    IFX_ETHSW_CleanUp_AR9_TANTOS();
#endif
#ifdef DANUBE
    IFX_ETHSW_CleanUp_DANUBE();
#endif
#ifdef AMAZON_SE
    IFX_ETHSW_CleanUp_AMAZON_SE();
#endif
#ifdef VR9
    IFX_ETHSW_CleanUp_VR9();
#endif
}

module_init(IFX_ETHSW_Switch_API_Init);
module_exit(IFX_ETHSW_Switch_API_Exit);

MODULE_AUTHOR("LANTIQ");
MODULE_DESCRIPTION("IFX ETHSW SWITCH API");
MODULE_LICENSE("GPL");
MODULE_VERSION(SWITCH_API_DRIVER_VERSION);
