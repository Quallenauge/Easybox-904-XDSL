/******************************************************************************

                               Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _IFX_PMCU_H_
#define _IFX_PMCU_H_

#include "ifx_types.h" 

/**
   \file ifx_pmcu.h
   PMCU (Power Management Control Unit) , userinterface controls and kernel API

   Header file contains defines, structures, constants and prototypes for I/O
*/


/**
   \defgroup LQ_PMCU_IOCTL IOCTL's
   \ingroup LQ_PMCU
*/
/* @{ */
/*=============================================================================*/
/* PMCU IOCTL DEFINES                                                          */
/*=============================================================================*/
/** Set a given module into a specific power state

   \param IFX_PMCU_MODULE_STATE_t* Pointer to a
    \ref IFX_PMCU_MODULE_STATE_t structure.

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_PMCU_MODULE_STATE_t param;
   param.pmcuModule=IFX_PMCU_MODULE_CPU;
   param.pmcuModuleNr=0;
   param.pmcuState=IFX_PMCU_STATE_D0;
   ioctl(fd, IFX_PMCU_STATE_REQ, &param);
   \endcode
*/
#define IFX_PMCU_STATE_REQ  0


/** Get the current power state from a given module
   \param IFX_PMCU_MODULE_STATE_t* Pointer to a
    \ref IFX_PMCU_MODULE_STATE_t structure.

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_PMCU_MODULE_STATE_t param;
   param.pmcuModule=IFX_PMCU_MODULE_CPU;
   param.pmcuModuleNr=0;
   param.pmcuState=IFX_PMCU_STATE_INVALID;
   ioctl(fd, IFX_PMCU_STATE_GET, &param);
   \endcode
*/
#define IFX_PMCU_STATE_GET  1


/** Change the log level for the PMCU module
   \param integer value which specifies the log level
          - <0 = quiet
          -  0 = only ERRORS,WARNINGS (default)
          -  1 = + INFO
          -  2 = all

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   int loglevel = 2;
   ioctl(fd, IFX_PMCU_LOG_LEVEL, &loglevel);
   \endcode
*/
#define IFX_PMCU_LOG_LEVEL  2


/** Control the acceptance of the powerstate request for the
    PMCU module. To have a central possibility for a global
    enable/disable of the powerstate requests coming from other
    modules, this IOCTL is introduced.
   \param integer value to enable/disable the powerstate 
          request inside the PMCU driver.
           - 0 =   All powerstate requests received by the PMCU
                   will be rejected. Default
           - 1  =  Powerstate requests are accepted by the PMCU

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   enable the powerstate request acceptance
   int reqCtrl = 1; 
   ioctl(fd, IFX_PMCU_REQ_CTRL, &reqCtrl); 
   \endcode
*/
#define IFX_PMCU_REQ_CTRL  3

//##########################################
/** Switch the available power management features of the system
	on and off. 
	
   \param IFX_PMCU_MODULE_PWR_FEATURE_STATE_t* Pointer to struct
    \ref IFX_PMCU_MODULE_PWR_FEATURE_STATE_t structure.

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

   \code
   IFX_PMCU_MODULE_PWR_FEATURE_STATE_t param;
   param.pmcuModule=IFX_PMCU_MODULE_CPU;
   param.pmcuModuleNr=0;
   param.pmcuPwrFeatureState=IFX_PMCU_PWR_STATE_OFF;
   ioctl(fd, IFX_PMCU_PWR_FEATURE_SWITCH, &param); 
   \endcode
*/
#define IFX_PMCU_PWR_FEATURE_SWITCH  4

//##########################################



/* @} */ /* LQ_PMCU_IOCTL */

/**
   \defgroup LQ_PMCU Power Management Control Unit
   \ingroup LQ_COC
   Ifx pmcu driver module
*/
/* @{ */
/*=============================================================================*/
/* PMCU ENUMERATION                                                                                                                */
/*=============================================================================*/
/** IFX_PMCU_MODULE_t
   Definition of the modules identifier
 */
typedef enum 
{
   IFX_PMCU_MODULE_PMCU       = 0, /** all modules registered within PMCU */
   IFX_PMCU_MODULE_CPU        = 1,
   IFX_PMCU_MODULE_ETH        = 2,
   IFX_PMCU_MODULE_USB        = 3,
   IFX_PMCU_MODULE_DSL        = 4,
   IFX_PMCU_MODULE_WLAN       = 5,
   IFX_PMCU_MODULE_DECT       = 6,
   IFX_PMCU_MODULE_FXS        = 7,
   IFX_PMCU_MODULE_FXO        = 8,
   IFX_PMCU_MODULE_VE         = 9,
   IFX_PMCU_MODULE_PPE        = 10,
   IFX_PMCU_MODULE_SWITCH     = 11,
   IFX_PMCU_MODULE_UART       = 12,
   IFX_PMCU_MODULE_SPI        = 13,
   IFX_PMCU_MODULE_SDIO       = 14,
   IFX_PMCU_MODULE_PCI        = 15,
   IFX_PMCU_MODULE_VLYNQ      = 16,
   IFX_PMCU_MODULE_DEU        = 17,
   IFX_PMCU_MODULE_CPU_PS     = 18,
   IFX_PMCU_MODULE_GPTC       = 19,
   IFX_PMCU_MODULE_USIF_UART  = 20,    /* VR9 usage */
   IFX_PMCU_MODULE_USIF_SPI   = 21,    /* VR9 usage */
   IFX_PMCU_MODULE_PCIE       = 22,    /* VR9 usage */
   IFX_PMCU_MODULE_ID_MAX     = 23,
} IFX_PMCU_MODULE_t;


/** IFX_PMCU_STATE_t
      Definition of power management state
*/
typedef enum
{
   /* ATTENTION:  Keep in mind that this list correspond to the ifx_pmcu_stateMap[][] array. 
                  If you have to change this list for any reason, ifx_pmcu_stateMap[][] must be adapted too. */

   /** State Invalid. */
   IFX_PMCU_STATE_INVALID = 0,
   /** State D0. Fully on */
   IFX_PMCU_STATE_D0 = 1,
   /** State D1. Device dependent */
   IFX_PMCU_STATE_D1 = 2,
   /** State D2. Device dependent */
   IFX_PMCU_STATE_D2 = 3,
   /** State D3. Off */
   IFX_PMCU_STATE_D3 = 4,
   /** don't care state */
   IFX_PMCU_STATE_D0D3 = 5,
} IFX_PMCU_STATE_t;


//#################################################
typedef enum
{
   /** State Invalid. */
   IFX_PMCU_PWR_STATE_INVALID = 0,
   /** Features State on */
   IFX_PMCU_PWR_STATE_ON = 1,
   /** Features State off */
   IFX_PMCU_PWR_STATE_OFF = 2,
} IFX_PMCU_PWR_STATE_ENA_t;
//#################################################



/** IFX_PMCU_TRANSITION_t
   Event passed to the PMCU
   \remarks Reserved for future use.
*/
typedef enum
{
   /** No event to be reported */
   IFX_PMCU_EVENT_NONE        = 0,
   /** State change */
   IFX_PMCU_EVENT_STATE       = 1,
   /** Module activated */
   IFX_PMCU_EVENT_ACTIVATED   = 2,
   /** Module deactivated */
   IFX_PMCU_EVENT_DEACTIVATED = 3,
   /** Device dependent event EXT1 */
   IFX_PMCU_EVENT_EXT1        = 4,
   /** Device dependent event EXT2 */
   IFX_PMCU_EVENT_EXT2        = 5,
} IFX_PMCU_TRANSITION_t;


/** IFX_PMCU_RETURN_t
   Return value for PMCU functions and callbacks
*/
typedef enum
{
   /** Operation success */
   IFX_PMCU_RETURN_SUCCESS = 0,
   /** Operation denied */
   IFX_PMCU_RETURN_DENIED  = 1,
   /** Error condition */
   IFX_PMCU_RETURN_ERROR   = 2,
} IFX_PMCU_RETURN_t;


/*=============================================================================*/
/* PMCU STRUCTURES                                                             */
/*=============================================================================*/
/** IFX_PMCU_MODULE_STATE_t
   Structure hold the module-ID, the moduleSub-ID and the PowerState of one module.
*/
typedef struct 
{
   /** Module identifier */
   IFX_PMCU_MODULE_t pmcuModule;
   /** instance identification of a Module; values 0,1,2,..... (0=first instance) */
   IFX_uint8_t       pmcuModuleNr;
   /** Module PowerState */
   IFX_PMCU_STATE_t  pmcuState;
} IFX_PMCU_MODULE_STATE_t;

//###################################
/** IFX_PMCU_MODULE_PWR_FEATURE_STATE_t
   Structure hold the module-ID and the PowerFeature_State to be set for a dedicated module.
*/
typedef struct
{
   /** Module identifier */
   IFX_PMCU_MODULE_t pmcuModule;
   /** instance identification of a Module; values 0,1,2,..... (0=first instance) */
   IFX_uint8_t       pmcuModuleNr;
   /** Module PowerFeatureState */
   IFX_PMCU_PWR_STATE_ENA_t  pmcuPwrFeatureState;
} IFX_PMCU_MODULE_PWR_FEATURE_STATE_t;
//###################################


/** IFX_PMCU_STATES_DEP_t
   Structure that describes dependencies on the given power-states of the
   given module.
*/
typedef struct
{
   /** Module identifier */
   IFX_PMCU_MODULE_t pmcuModule;
   /** possible values: D0, D1, D2, D3 */
   IFX_uint8_t       onState;
   /** possible values: D0, D1, D2, D3 */
   IFX_uint8_t       standBy;
   /** possible values: D0, D1, D2, D3 */
   IFX_uint8_t       lpStandBy;
   /** possible values: D0, D1, D2, D3 */
   IFX_uint8_t       offState;
} IFX_PMCU_STATES_DEP_t;


/** IFX_PMCU_MODULE_DEP_t
   Structure to list dependencies on multiple modules.
*/
typedef struct
{
   /** number of entries in the dependency list moduleStates[] */
   IFX_uint32_t          nDepth;
   /** power state dependency list(s) of one module */
   IFX_PMCU_STATES_DEP_t moduleStates[];
} IFX_PMCU_MODULE_DEP_t;


/**IFX_PMCU_EVENT_t
   Structure used to report an information to the PMCU
   \remarks Reserved for future use.
 */
typedef struct
{
   /** Module identifier */
   IFX_PMCU_MODULE_t     pmcuModule;
   /** instance identification of a Module; values 0,1,2,..... (0=first instance) */
   IFX_uint8_t           pmcuModuleNr;
   /** Current module state  */
   IFX_PMCU_STATE_t      pmcuState;
   /** Transition passed to the PMCU */
   IFX_PMCU_TRANSITION_t pmcuTransition;
} IFX_PMCU_EVENT_t;


/** IFX_PMCU_REGISTER_t
   Structure used to register/unregister a driver to the PMCU
*/
typedef struct
{
   /** Module identifier */
   IFX_PMCU_MODULE_t      pmcuModule;
   /** instance identification of a Module; values 0,1,2,..... (0=first instance) */
   IFX_uint8_t            pmcuModuleNr;
   /** pointer to the module dependency list.
       The dependency list will be copied into a static structure inside the PMCU. */
   IFX_PMCU_MODULE_DEP_t *pmcuModuleDep;
   /** Callback to be called before module changes it's state to new */
   IFX_PMCU_RETURN_t    (*pre)                   ( IFX_PMCU_MODULE_t pmcuModule,
                                                   IFX_PMCU_STATE_t newState,
                                                   IFX_PMCU_STATE_t oldState);
   /** Callback used to change module's power state */
   IFX_PMCU_RETURN_t    (*ifx_pmcu_state_change) ( IFX_PMCU_STATE_t pmcuState );
   /** Callback to be called after module changes it's state to new state */
   IFX_PMCU_RETURN_t    (*post)                  ( IFX_PMCU_MODULE_t pmcuModule,
                                                   IFX_PMCU_STATE_t newState,
                                                   IFX_PMCU_STATE_t oldState);
   /** Optional: Callback used to get module's power state. Set to NULL if unused */
   IFX_PMCU_RETURN_t    (*ifx_pmcu_state_get)    ( IFX_PMCU_STATE_t *pmcuState );
//###################################
   /** Callback used to enable/disable the power features of the module */
   IFX_PMCU_RETURN_t    (*ifx_pmcu_pwr_feature_switch) ( IFX_PMCU_PWR_STATE_ENA_t pmcuPwrStateEna );
//###################################
   
} IFX_PMCU_REGISTER_t;
/* @} */ /* LQ_PMCU */

/**
   \defgroup LQ_PMCU_KERNEL_API Kernel API
   \ingroup LQ_PMCU
*/
/* @{ */
/*=============================================================================*/
/* PMCU FUNCTION PROTOTYPES                                                    */
/*=============================================================================*/
#ifdef __KERNEL__
/**
   PMCU kernel API prototypes
*/

/**
   Request module state change to PMCU

   \param [in]  pmcuModule        define module for state change
   \param [in]  pmcuModuleNr      instance identification of a Module; values 0,1,2,..... (0=first instance)
   \param [in]  newState          requested power state

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

*/
IFX_PMCU_RETURN_t ifx_pmcu_state_req (IFX_PMCU_MODULE_t pmcuModule, IFX_uint8_t pmcuModuleNr, IFX_PMCU_STATE_t newState);


/**
   Register a module, which is affected by a powerState change or initiate a powerState change, to the PMCU

   \param [in]  pmcuRegister  Registration Information

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

*/
IFX_PMCU_RETURN_t ifx_pmcu_register (IFX_PMCU_REGISTER_t* pmcuRegister);


/**
   Unregister a module from the PMCU.

   \param [in] pmcuUnregister  Unregistration Information. Only module name is required.

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

*/
IFX_PMCU_RETURN_t ifx_pmcu_unregister (IFX_PMCU_REGISTER_t* pmcuUnregister);

#endif   /* __KERNEL__ */

/* @} */ /* LQ_PMCU_KERNEL_API */

#endif   /* _IFX_PMCU_H_ */

