/******************************************************************************

                               Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _IFXMIPS_PMCU_H_
#define _IFXMIPS_PMCU_H_

/*=============================================================================*/
/* Register definition for the CHIPID_Register                                 */
/*=============================================================================*/
#define IFX_MPS               (KSEG1 | 0x1F107000)
#define IFX_MPS_CHIPID        ((volatile u32*)(IFX_MPS + 0x0344))
#define ARX188                0x016C
#define ARX168                0x016D
#define ARX182                0x016E /* before November 2009 */
#define ARX182_2              0x016F /* after November 2009 */
#define GRX188                0x0170
#define GRX168                0x0171
#define VRX288                0x01C0
#define VRX282                0x01C1
#define VRX268                0x01C2
#define GRX288                0x01C9

/**
   \defgroup LQ_PMCU Power Management Control Unit
   \ingroup LQ_COC
   Ifx pmcu driver module
*/
/* @{ */

/** IFX_PMCU_MODULE_ID_MAX defined in IFX_PMCU_MODULE_t  */
#define MAX_PMCU_MODULE       IFX_PMCU_MODULE_ID_MAX

/** max number of instances per module (right now free definition) */
#define MAX_PMCU_SUB_MODULE   6

/*=============================================================================*/
/* PMCU internal ENUMERATION                                                   */
/*=============================================================================*/
/** IFX_PMCU_REQ_ID_t
    Status of a powerState REQUEST
*/
typedef enum
{
   /** no pending powerState request pending in the ringBuffer */
   IFX_PMCU_NO_PENDING_REQ     = 0,
   /** powerState request pending in the ringBuffer */
   IFX_PMCU_PENDING_REQ        = 1,
   /** unknown powerState request in buffer or buffer overflow */
   IFX_PMCU_PENDING_REQ_ERROR  = -1,
} IFX_PMCU_REQ_ID_t;


/*=============================================================================*/
/* PMCU internal STRUCTURES                                                    */
/*=============================================================================*/
/** IFX_PMCU_REQ_STATE_t
    struct used in REQUEST ringBuffer to keep all relevant info's for one
    powerState request
*/
typedef struct
{
   IFX_PMCU_MODULE_STATE_t moduleState;
   IFX_PMCU_REQ_ID_t       reqId;
}IFX_PMCU_REQ_STATE_t;


/** IFX_PMCU_REGISTRY_INFO_t
    Status of the module registration
*/
typedef enum
{
   /** module not registered at all */
   IFX_PMCU_MODULE_NOT_REGISTRED = 0,
   /** module not registered at the moment */
   IFX_PMCU_MODULE_UNREGISTERED  = 1,
   /** module registered */
   IFX_PMCU_MODULE_REGISTERED    = 2,
} IFX_PMCU_REGISTRY_INFO_t;


/** IFX_PMCU_MAP_t
   Structure used to hold the complete register, dependency and powerState information
   of the registered modules
*/
typedef struct
{
   /** array of IFX_PMCU_REGISTER_t elements for each instance of the module */
   IFX_PMCU_REGISTER_t       pmcuRegister[MAX_PMCU_SUB_MODULE];
   /** array of latest requested and accepted powerState for each instance of a module */
   IFX_PMCU_STATE_t          pmcuNewState[MAX_PMCU_SUB_MODULE];
   /** array of previous accepted powerState for each instance of a module */
   IFX_PMCU_STATE_t          pmcuOldState[MAX_PMCU_SUB_MODULE];
   /** registry info for each instance of a module */
   IFX_PMCU_REGISTRY_INFO_t  registryInfo[MAX_PMCU_SUB_MODULE];
   /** not used right now */
   IFX_uint32_t              dbgFlag;
   /** modules name */
   IFX_char_t                *pmcuModuleName;
} IFX_PMCU_MAP_t;



/*=============================================================================*/
/* PMCU internal FUNCTION PROTOTYPES                                           */
/*=============================================================================*/
/**
   Function is used to put a incoming powerState request into the request
   ringBuffer reqBuffer[]. This buffer collect all powerState requests until
   the next interrupt trigger the PMCU tasklet, which process all request's
   stored in reqBuffer[].

   \param[in]   req  request information (module, powerState, reqID)

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

     \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_put_req( IFX_PMCU_REQ_STATE_t req );

/**
   Function is used to get a powerState request from the request
   ringBuffer reqBuffer[] for further processing. The function returns always the request
   marked by the current read pointer.

   \return Returns value as follows:
     - IFX_PMCU_REQ_STATE_t

     \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_REQ_STATE_t ifx_pmcu_get_req( IFX_void_t );

/**
   Recursive function to parse all dependency list's to create a accumulated list
   of modules and their powerStates, for the requested powerState/module.
   The accumulated list is a static array called ifx_pmcu_accuDepList[].

   \param[in]   pmcuModuleState  (module, subNr, newState)

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

     \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_parse_deplist (IFX_PMCU_MODULE_STATE_t pmcuModuleState);

/**
   Apply all registered callbacks to be executed before a stateChange could be triggered.
   The input for this function is the accumulated dependency list ifx_pmcu_accuDepList[] which 
   is filled by ifx_pmcu_parse_deplist().

   \param[out]  pmcuPreCallIndex index to the last element inside the ifx_pmcu_preCall[][] array

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error
     - IFX_PMCU_RETURN_DENIED in case that the powerState change was rejected

     \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_prechange (IFX_int8_t* pmcuPreCallIndex);


/**
   Apply all callbacks registered to be executed if a state change is requested and accepted.

   \param[in]   pmcuModuleState  (module, subNr, newState)
   \param[out]  pmcuPreCallIndex index to the last element inside the ifx_pmcu_preCall[][] array

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

   \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_statechange (IFX_PMCU_MODULE_STATE_t pmcuModuleState, IFX_int8_t* pmcuPreCallIndex);


/**
   Apply all registered postCallbacks to be executed after a stateChange for a module was done,
   or a powerState request was denied.

   \param[in]   pmcuModuleState  (module, subNr, newState)
   \param[out]  pmcuPreCallIndex index to the last element inside the ifx_pmcu_preCall[][] array

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

   \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_postchange (IFX_PMCU_MODULE_STATE_t pmcuModuleState, IFX_int8_t* pmcuPreCallIndex);

/**
   This function is called by the linux kernel as a tasklet process, means running in Interrupt-Context.
   The complete processing of a requested powerState change is controlled by this function.
   Getting the right request from the reqBuffer[], parsing all dependency tables, and invoke all necessary
   pre, stateChange and post callbacks.

   \param[in]   data  not used
   \ingroup IFX_PMCU_KERNEL
*/
static IFX_void_t ifx_pmcu_process_req_tasklet(unsigned long data);


/**
   The function will enable/disable the given power feature of the defined module. 
   Module is specified by module and subNr. 
   PowerFeatureId and enable/disable is defined by enum in newPwrFeatureState. 
   This function is only used by IOCTL function IFX_PMCU_PWR_FEATURE_SWITCH. 

   \param[in]   pwrFeaSwitch  (module, subNr, newPwrFeatureState)

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_DENIED: in case of an error

   \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_pwr_feature_switch (IFX_PMCU_MODULE_PWR_FEATURE_STATE_t pwrFeaSwitch);


/**
   The function fetch the current power state directly from the module, and return the 
   actual state to caller function. If the state IFX_PMCU_STATE_INVALID is returned, 
   probably the power feature of the module is disabled.

   \param[in]   pmcuModState  (module[in], subNr[in], pmcuState[out])

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_DENIED: in case of an error

   \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_get_state (IFX_PMCU_MODULE_STATE_t* pmcuModState);

#endif   /* _IFXMIPS_PMCU_H_ */

/* @} */ /* LQ_PMCU */

