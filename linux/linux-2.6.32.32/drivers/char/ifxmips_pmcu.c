/******************************************************************************

                               Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#define LINUX_2_6
#ifdef LINUX_2_6
   #include <linux/version.h>
   #ifndef UTS_RELEASE
      #include <linux/utsrelease.h>
   #endif /* UTC_RELEASE */
#endif /* LINUX_2_6 */

#ifndef AUTOCONF_INCLUDED
   #include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_pmcu.h>
#include "ifxmips_pmcu.h"


/**
   \defgroup LQ_PMCU_PRG Program Guide
   \ingroup LQ_PMCU
   <center>
   <B>"Power Management Control Unit ( PMCU )" </B>.
   </center>    
   \section Purpose Purpose
    The PMCU is the central unit of the power management framework. It is responsible for processing
    requests to change the power states of the hardware modules whose device drivers have registered
    with it. The PMCU ensures a smooth power state transition by resolving all dependencies before finally
    changing to the requested power state.\n\n\n
   Supported Power States:
        <table>
        <tr><td>D0</td> <td>on State</td> </tr>
        <tr><td>D1</td> <td>standby</td> </tr>
        <tr><td>D2</td> <td>low power standby</td> </tr>
        <tr><td>D3</td> <td>off State</td> </tr>
     </table>
   \n
   \n
   <B>"High Level PMCU Architecture" </B>.
   \image html cpe_power_sw_architecture2.jpg
   \n
   \n
   \section Function_Calltree Function Calltree of PMCU
   \image html pmcucalltree.gif
   \n 
   \section Registration_Example Example for Registration to the PMCU 
   All modules/drivers/subsystems which contribute to power 
   saving features or/and are effected by these must register to 
   the PMCU. For example if the feature frequency change is 
   getting active, most of the subsystems are effected because 
   of reduced CPU performance and reduced DDR bandwidth. In case 
   a subsystem is registered to the PMCU, the subsysten may deny 
   a frequency change if it needs full performance of CPU and 
   DDR bandwidth at this time. If the subsystem is idle the PMCU 
   should be informed to check if the entire system can run with 
   a reduced frequency. \n 
   The following steps must be done to register to the pmcu:\n 
   1. fill the registry structure <B>IFX_PMCU_REGISTER_t pmcuRegister;</B> 
      as shown in the example below.\n  
   2. define the registrated callback functions with the module specific functionality.\n 
   3. call the registration function <B>ifx_pmcu_register ( &pmcuRegister );</B> 
      with a defined registry structure during the initialisation phase of the module\n
   4. all modules where no dependency list is defined (see list below), define a NULL pointer instead.\n
   \n 
   <B>Registration Example:</B>\n
\code 
//define dependency list;  state D0D3 means don't care
//static declaration is necessary to let gcc accept this static initialisation.
//The dependency list shows the pmcu module which module specific power state correspond
//to the actual requested power state.
static IFX_PMCU_MODULE_DEP_t depListWlan= 
{ 
     1,
     {
         {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
     }
};
    :
// This functions returns the current power state of the module 
static IFX_PMCU_RETURN_t 
ifx_wlan_stateGet(IFX_PMCU_STATE_t *pmcuModState) {
    printk(KERN_DEBUG "ifx_wlan_stateGet is called\n");

    if(wlanPwrFea_g == 1){
        printk(KERN_DEBUG "current power state of WLAN module is D0\n");
        *pmcuModState = IFX_PMCU_STATE_D0; // set here the right value
    }
    if(wlanPwrFea_g == 0){
        printk(KERN_DEBUG "power feature of WLAN module is disabled\n");
        *pmcuModState = IFX_PMCU_STATE_INVALID; // must be set to INVALID
    }
    return IFX_PMCU_RETURN_SUCCESS;
}

// The function should be used to enable/disable the module specific power saving methods
static IFX_PMCU_RETURN_t 
ifx_wlan_pwrFeatureSwitch(IFX_PMCU_PWR_STATE_ENA_t pmcuPwrStateEna)
{
    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_ON) {
        wlanPwrFea_g = 1;
        printk(KERN_DEBUG "All WLAN power saving features are switched on\n");
        return IFX_PMCU_RETURN_SUCCESS;
    }
    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_OFF) {
        wlanPwrFea_g = 0;
        printk(KERN_DEBUG "All WLAN power saving features are switched off\n");
        return IFX_PMCU_RETURN_SUCCESS;
    }
    return IFX_PMCU_RETURN_SUCCESS;
}

// This function should be used to do all the necessary clean-up's before a the real
// power state change is initiated; e.g. flush all serial buffers inside the UART  before 
// the frequency will be changed. 
static IFX_PMCU_RETURN_t 
ifx_wlan_preChange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState) 
{ 
    printk(KERN_DEBUG "ifx_wlan_preChange is called\n");
    return IFX_PMCU_RETURN_SUCCESS;
}


// This function initiate the real power state change. The module should do all the necessary
//   adpations to the new state.
static IFX_PMCU_RETURN_t 
ifx_wlan_stateChange(IFX_PMCU_STATE_t newState)
{
    printk(KERN_DEBUG "ifx_wlan_stateChange is called\n");
    return IFX_PMCU_RETURN_SUCCESS;
}

// This function should be used to do all the necessary post processing after a the real
//   power state change was initiated.
static IFX_PMCU_RETURN_t 
ifx_wlan_postChange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
    printk(KERN_DEBUG "ifx_wlan_postChange is called\n");
    return IFX_PMCU_RETURN_SUCCESS;
}
     :
 void ifx_wlan_init(void) { :
   IFX_PMCU_REGISTER_t pmcuRegister;
     :
   memset (&pmcuRegister, 0, sizeof(pmcuRegister));
   pmcuRegister.pmcuModule=IFX_PMCU_MODULE_WLAN;
   pmcuRegister.pmcuModuleNr=0;
   pmcuRegister.pmcuModuleDep = &depListWlan;
   pmcuRegister.pre = ifx_wlan_preChange;
   pmcuRegister.post = ifx_wlan_postChange;
   pmcuRegister.ifx_pmcu_state_change = ifx_wlan_stateChange;
   pmcuRegister.ifx_pmcu_state_get = ifx_wlan_stateGet;
   pmcuRegister.ifx_pmcu_pwr_feature_switch = ifx_wlan_pwrFeatureSwitch;
   ifx_pmcu_register ( &pmcuRegister );
     :
 }
\endcode
    
   \section Dependency_Tables Dependency Tables of the subsystems
    <B>CPU</B>\n
    \code
    static IFX_PMCU_MODULE_DEP_t depList=
    {
        9,
        {
            {IFX_PMCU_MODULE_VE,   IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D2},
            {IFX_PMCU_MODULE_PPE,  IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_USB,  IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_DSL,  IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_WLAN, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_DECT, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_GPTC, IFX_PMCU_STATE_D0,   IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_SPI,  IFX_PMCU_STATE_D0,   IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D3},
            {IFX_PMCU_MODULE_UART, IFX_PMCU_STATE_D0,   IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D3}
        }
    };
    \endcode
    
    <B>VE</B>\n
    \code
    static IFX_PMCU_MODULE_DEP_t depList=
    {
        1,
        {
            {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D0D3}
        }
    };
    \endcode
    
   <B>PPE</B>\n 
   \code 
   static IFX_PMCU_MODULE_DEP_t depList= 
   { 
        1,
        {
            {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
        }
   };
    \endcode
    
   <B>USB</B>\n 
   \code 
   static IFX_PMCU_MODULE_DEP_t depList= 
   { 
        1,
        {
            {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
        }
   };
    \endcode
    
   <B>DSL</B>\n 
   \code 
   static IFX_PMCU_MODULE_DEP_t depList= 
   { 
        1,
        {
            {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
        }
   };
    \endcode
    
   <B>WLAN</B>\n 
   \code 
   static IFX_PMCU_MODULE_DEP_t depList= 
   { 
        1,
        {
            {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
        }
   };
    \endcode
    
   <B>DECT</B>\n 
   \code 
   static IFX_PMCU_MODULE_DEP_t depList= 
   { 
        1,
        {
            {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
        }
   };
    \endcode
    
*/


/* PMCU driver device name used during OS registration */
#define IFX_PMCU_DEVICE_NAME  "ifx_pmcu"

/* PMCU driver version  */
#define IFX_PMCU_DRV_VERSION  "2.1.3.0"


/* size of request ringbuffer */
#define IFX_REQ_BUFFER_SIZE 5

/* max number of entries in dependency list 
   +1 represents the entry the requested module inside the ifx_pmcu_preCall[][] array */
#define IFX_DEPLIST_SIZE (10 + 1)

/* max number of dependency entries inclusive the nested dependencies */
#define IFX_PMCU_ACCU_DEPLIST_MAX 15

/* PMCU printk specific's; colored print_message support */
/******************************************************************************/
#define BLINK   "\033[31;1m"
#define RED     "\033[31;1m"
#define YELLOW  "\033[33;1m"
#define GREEN   "\033[32;2m"
#define BLUE    "\033[34;1m"
#define CYAN    "\033[36;2m"
#define DIM     "\033[37;1m"
#define NORMAL  "\033[0m"

#define PMCU_EMERG(module,fmt,arg...)\
        if(log_level_g > 0){\
        printk(KERN_EMERG RED #module NORMAL ": " fmt,##arg);\
        }

#define PMCU_ERR(module,fmt,arg...)\
        if(log_level_g > 0){\
        printk(KERN_ERR RED #module NORMAL ": " fmt,##arg);\
        }

#define PMCU_WARNING(module,fmt,arg...)\
        if(log_level_g > 0){\
        printk(KERN_WARNING YELLOW #module NORMAL ": " fmt,##arg);\
        }

#define PMCU_INFO(module,fmt,arg...)\
        if(log_level_g > 1){\
        printk(KERN_INFO GREEN #module NORMAL ": " fmt,##arg);\
        }

#define PMCU_DEBUG(module,fmt,arg...)\
        if(log_level_g > 2){\
        printk(KERN_DEBUG CYAN #module NORMAL ": " fmt,##arg);\
        }
/******************************************************************************/

#undef PMCU_DEBUG_ENABLE
//#define PMCU_DEBUG_ENABLE
#ifdef PMCU_DEBUG_ENABLE
   /* debug macro to print the content of the request ringBuffer */
   #define PMCU_DEBUG_PRINT_REQ_BUFFER(max) \
      { \
      int i; \
      for (i=0; i<max;i++){ \
         PMCU_DEBUG(PMCU, "reqBuf[%d]: pmcuModule=%s\t pmcuModuleNr=%d\t pmcuState=%s\t reqId=%d\n",\
         i,ifx_pmcu_mod[ifx_pmcu_reqBuffer[i].moduleState.pmcuModule],ifx_pmcu_reqBuffer[i].moduleState.pmcuModuleNr,\
         ifx_pmcu_ps[ifx_pmcu_reqBuffer[i].moduleState.pmcuState],ifx_pmcu_reqBuffer[i].reqId); \
      } \
      }

   /* debug macro to print the content of the ifx_pmcu_preCall[][] array */
   #define PMCU_DEBUG_PRINT_PRECALL() \
      { \
      int i;\
      for (i=0; i<IFX_DEPLIST_SIZE;i++){ \
         PMCU_DEBUG\
         (\
         PMCU, "ifx_pmcu_preCall[%d]: pmcuModule=%s\t pmcuModuleNr=%d\t state=%s\t preCall=%d\t changeCall=%d\t postCall=%d\t index=%d\n",\
         i,ifx_pmcu_mod[ifx_pmcu_preCall[i][0]],ifx_pmcu_preCall[i][1],ifx_pmcu_ps[ifx_pmcu_preCall[i][2]],ifx_pmcu_preCall[i][3],\
         ifx_pmcu_preCall[i][4],ifx_pmcu_preCall[i][5], pmcuPreCallIndex\
         );\
      } \
      }

   /* debug macro to print the content of the accumulated dependency list */
   #define PMCU_DEBUG_PRINT_ACCU_DEPLIST(max) \
      { \
      int i; \
      for (i=0; i<max;i++){ \
         PMCU_DEBUG(PMCU, "ifx_pmcu_accuDepList[%d]: pmcuModule=%s\t pmcuModuleNr=%d\t pmcuState=%s\t index=%d\n",\
         i,ifx_pmcu_mod[ifx_pmcu_accuDepList[i].pmcuModule],ifx_pmcu_accuDepList[i].pmcuModuleNr,\
         ifx_pmcu_ps[ifx_pmcu_accuDepList[i].pmcuState],ifx_pmcu_indexAccuDepL); \
      } \
      }

   /* debug macro to print the content of the ifx_pmcu_ModMem */
   #define PMCU_DEBUG_PRINT_MOD_MEM() \
      { \
      int i; \
      for (i=0; i<IFX_PMCU_MODULE_ID_MAX;i++){ \
         PMCU_DEBUG(PMCU, "ifx_pmcu_ModMem[%d]: pmcuModule=%s\t marker=%d\n",\
         i,ifx_pmcu_mod[i],ifx_pmcu_ModMem[i]); \
      } \
      }
#else
   #define PMCU_DEBUG_PRINT_REQ_BUFFER
   #define PMCU_DEBUG_PRINT_PRECALL
   #define PMCU_DEBUG_PRINT_ACCU_DEPLIST
   #define PMCU_DEBUG_PRINT_MOD_MEM
#endif


/*=============================================================================*/
/* MODULE GLOBAL VARIABLES                                                     */
/*=============================================================================*/
/* major number definition
   if the macro PMCU_DYNAMIC_MAJOR_NO is defined the initial major number 243 will be overwritten
   by the automatic returned major number from the kernel */
#define PMCU_DYNAMIC_MAJOR_NO
static IFX_int_t ifx_pmcu_major = 243;

/** \ingroup LQ_PMCU
    \brief  The ifx_pmcu_lock is used to protect all data structures accessed from Interrupt- AND 
            Process-Context.
*/
static DEFINE_SPINLOCK(ifx_pmcu_lock);
//static spinlock_t ifx_pmcu_lock = SPIN_LOCK_UNLOCKED;

/** \ingroup LQ_PMCU
    \brief  The ifx_pmcu_map_mutex is used to protect all data structures accessed from several threads
            except interrupt context. All data structures accessed from several threads AND from Interrupt-Context
            must use both locks (spinlock and mutex) for protection.
*/
DECLARE_MUTEX(ifx_pmcu_map_mutex);


/* TODO: not used upto now */
static wait_queue_head_t ifx_pmcu_wait_q;

/** \ingroup LQ_PMCU
    \brief  Control the  garrulity of the PMCU module 
            - 0  = quiet
            - 1  = EMERG, ERRORS, WARNINGS (default)
            - 2  = + INFOs
            - 3  = + DEBUG
*/
int log_level_g         = 1;

/** \ingroup LQ_PMCU
    \brief  Control the  powerstate request acceptance of the
            PMCU module.
            - 0 =   All powerstate requests received by the PMCU
                    will be rejected. Default
            - 1  =  Powerstate requests are accepted by the PMCU
*/
int request_control_g   = 0;

/* module names; array correspond to enum IFX_PMCU_MODULE_t */
static IFX_char_t* ifx_pmcu_mod[IFX_PMCU_MODULE_ID_MAX+1] = {
   "PMCU" ,
   "CPU " ,
   "ETH " ,
   "USB " ,
   "DSL " ,
   "WLAN" ,
   "DECT" ,
   "FXS " ,
   "FXO " ,
   "VE  " ,
   "PPE " ,
   "SWITCH" ,
   "UART" ,
   "SPI " ,
   "SDIO" ,
   "PCI " ,
   "VYLINQ" ,
   "DEU " ,
   "CPU_PS" ,
   "GPTC" ,
   "USIF_UART" ,
   "USIF_SPI" ,
   "PCIE" ,
   "INVALID" ,
};

/* Power state translation */
static IFX_char_t* ifx_pmcu_ps[] = {
   "-1",
   "D0",
   "D1",
   "D2",
   "D3",
   "D0D3" ,
};

/** \ingroup LQ_PMCU
    \brief  Dependency of the current state against the mask from the dependency list.
            - 0 = nothing to do for this module
            - 1 = call state change
    \attention Keep in mind that this list correspond to the IFX_PMCU_STATE_t enumeration.
*/
static IFX_uint8_t ifx_pmcu_stateMap[6][5] = {  /* [mask from depList] [current mode] */
   /* current state     INV,D0,D1,D2,D3 */
   /*invalid  */       { 0, 0, 0, 0, 0 },
   /*mask_D0  */       { 0, 0, 1, 1, 1 },
   /*mask_D1  */       { 0, 1, 0, 1, 1 },
   /*mask_D2  */       { 0, 1, 1, 0, 1 },
   /*mask_D3  */       { 0, 1, 1, 1, 0 },
   /*mask_D0D3*/       { 0, 0, 0, 0, 0 }
};

/** \ingroup LQ_PMCU
    \brief  The preCall array is used to mark all modules informed about a power state change request 
            by calling the preCallback function of the module. This info is used to decide afterwards 
            which stateChangeCallbacks and postCallbacks must be called. Hint: Each callback call can 
            deny the entire request for a power state change!
    \image html ifx_pmcu_precall.jpg
*/
static IFX_int32_t ifx_pmcu_preCall[IFX_DEPLIST_SIZE][6]; /* [depList entry] [module,sub,state,pre,change,post] */

/** \ingroup LQ_PMCU
    \brief   PMCU internal module map table. holds current status, registration entries, 
    callback functions, modulenames and dependency lists.
    \image html ifx_pmcu_map.jpg
*/
static IFX_PMCU_MAP_t ifx_pmcu_map[IFX_PMCU_MODULE_ID_MAX+1];

#ifdef CONFIG_PROC_FS
   /* proc entry variables */
   static struct proc_dir_entry* g_ifx_pmcu_dir_pmcu = NULL;
   static struct proc_dir_entry* g_ifx_pmcu_dir_debug = NULL;
#endif

/** \ingroup LQ_PMCU
    \brief  The powerState request ringBuffer collect the incoming power state requests. The ringBuffer has
            a size of IFX_REQ_BUFFER_SIZE, and will continuously start from the beginning with each overflow.  
    \image html ifx_pmcu_reqbuffer.jpg
*/
static IFX_PMCU_REQ_STATE_t ifx_pmcu_reqBuffer[ IFX_REQ_BUFFER_SIZE ];

/** \ingroup LQ_PMCU
    \brief  The reqGetIndex points to the next request entry to be processed.  
*/
static IFX_uint32_t ifx_pmcu_reqGetIndex = 0;

/** \ingroup LQ_PMCU
    \brief  The reqPutIndex points to the next free entry inside the ringBuffer to place 
            the next incoming request.  
*/
static IFX_uint32_t ifx_pmcu_reqPutIndex = 0;

/** \ingroup LQ_PMCU
    \brief  The reqBufferSize is the ringBuffer watchdog to signalize a real overflow of the ringBuffer.
            A real overflow means that the reqbuffer contains already IFX_REQ_BUFFER_SIZE request entries
            and a new incoming request can not be placed and will be discarded.
*/
static IFX_uint32_t ifx_pmcu_reqBufferSize = 0;

/** \ingroup LQ_PMCU
    \brief  Accumulated dependency list after parsing all dependency tables, inclusive nested dependencies.
    \image html ifx_pmcu_accudeplist.jpg
*/
static IFX_PMCU_MODULE_STATE_t ifx_pmcu_accuDepList[IFX_PMCU_ACCU_DEPLIST_MAX];

/** \ingroup LQ_PMCU
    \brief  This index points to the next free entry inside the ifx_pmcu_accuDepList[].  
*/
static IFX_uint8_t ifx_pmcu_indexAccuDepL;

/** \ingroup LQ_PMCU
    \brief  This structure is used during parsing the dependency tables, to mark all modules already available
            in the accumulated dependeny list. Just to avoid recursive dependency relations. As soon as a 
            module was put into the ifx_pmcu_accuDepList, the module is marked as known inside this array.
    \image html ifx_pmcu_modmem.jpg
*/
static IFX_uint8_t ifx_pmcu_ModMem[IFX_PMCU_MODULE_ID_MAX];


/*=============================================================================*/
/* LOCAL FUNCTIONS PROTOTYPES                                                  */
/*=============================================================================*/
static ssize_t ifx_pmcu_read( struct file *filp, IFX_char_t *buf, size_t count, loff_t *ppos );
static ssize_t ifx_pmcu_write( struct file *filp, const IFX_char_t *buf, size_t count, loff_t *ppos );
static IFX_int_t ifx_pmcu_ioctl(struct inode *inode, struct file *file, IFX_uint_t cmd, unsigned long arg);
static IFX_int_t ifx_pmcu_open(struct inode * inode, struct file * file);
static IFX_int_t ifx_pmcu_release(struct inode *inode, struct file *file);
static IFX_uint_t ifx_pmcu_poll(struct file *filp, poll_table *wait);
#ifdef CONFIG_PROC_FS
   static IFX_int_t ifx_pmcu_proc_device_list(IFX_char_t *buf, IFX_char_t **start, off_t offset, IFX_int_t count, IFX_int_t *eof, IFX_void_t *data);
   static IFX_int_t ifx_pmcu_proc_version(IFX_char_t *buf, IFX_char_t **start, off_t offset, IFX_int_t count, IFX_int_t *eof, IFX_void_t *data);
   static IFX_int_t ifx_pmcu_proc_get_internal_map(IFX_char_t *buf, IFX_char_t **start, off_t offset, IFX_int_t count, IFX_int_t *eof, IFX_void_t *data);
#endif


/*=============================================================================*/
/* TASKLET DECLARATION                                                         */
/*=============================================================================*/
DECLARE_TASKLET(ifx_pmcu_tasklet,ifx_pmcu_process_req_tasklet,0);


/*=============================================================================*/
/* FUNCTION DEFINITIONS                                                        */
/*=============================================================================*/
static IFX_PMCU_RETURN_t ifx_pmcu_put_req( IFX_PMCU_REQ_STATE_t req )
{
   IFX_uint32_t i;
   IFX_ulong_t iflags;

   if ( ifx_pmcu_reqBufferSize >= IFX_REQ_BUFFER_SIZE ){
      return IFX_PMCU_RETURN_ERROR;
   }

   spin_lock_irqsave(&ifx_pmcu_lock, iflags);
   
   /* first check if there is already a request for this module pending.
      If this is the case, reject the actual one. */
   for(i=0;i<IFX_REQ_BUFFER_SIZE;i++) {
       if(ifx_pmcu_reqBuffer[i].reqId == IFX_PMCU_NO_PENDING_REQ){
           continue;
       }
       if((ifx_pmcu_reqBuffer[i].moduleState.pmcuModule == req.moduleState.pmcuModule) &&
          (ifx_pmcu_reqBuffer[i].moduleState.pmcuModuleNr == req.moduleState.pmcuModuleNr) && 
          (ifx_pmcu_reqBuffer[i].moduleState.pmcuState == req.moduleState.pmcuState) ){
           spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
           return IFX_PMCU_RETURN_SUCCESS;
       }
   }

   req.reqId = IFX_PMCU_PENDING_REQ;
   memcpy(&ifx_pmcu_reqBuffer[ ifx_pmcu_reqPutIndex ], &req, sizeof(req));
   ifx_pmcu_reqPutIndex++;
   if ( ifx_pmcu_reqPutIndex >= IFX_REQ_BUFFER_SIZE ){
      ifx_pmcu_reqPutIndex = 0;
   }
   ifx_pmcu_reqBufferSize++;
   
   spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
  
   //PMCU_DEBUG_PRINT_REQ_BUFFER(5);
   
   return IFX_PMCU_RETURN_SUCCESS;
}


static IFX_PMCU_REQ_STATE_t ifx_pmcu_get_req( IFX_void_t )
{
   IFX_PMCU_REQ_STATE_t req;

   req.reqId = IFX_PMCU_PENDING_REQ_ERROR;

   if ( !ifx_pmcu_reqBufferSize ){
      return req;
   }
   ifx_pmcu_reqBufferSize--;
   memcpy(&req, &ifx_pmcu_reqBuffer[ ifx_pmcu_reqGetIndex ], sizeof(req));
   ifx_pmcu_reqBuffer[ ifx_pmcu_reqGetIndex ].reqId = IFX_PMCU_NO_PENDING_REQ;
   ifx_pmcu_reqGetIndex++;
   if ( ifx_pmcu_reqGetIndex >= IFX_REQ_BUFFER_SIZE ){
      ifx_pmcu_reqGetIndex = 0;
   }
   return req;
}

/* this function runs in interrupt context */
static IFX_void_t ifx_pmcu_process_req_tasklet(unsigned long data)
{
   
   IFX_PMCU_REQ_STATE_t reqState;
   IFX_PMCU_RETURN_t ret;
   IFX_PMCU_MODULE_STATE_t pmcuModuleState;
   IFX_int8_t pmcuPreCallIndex = -1;
   
   PMCU_DEBUG(PMCU, "ifx_pmcu_process_req_tasklet() is called\n");

   while(ifx_pmcu_reqBufferSize > 0) {
       pmcuPreCallIndex = -1;
       /* init ifx_pmcu_preCall[][] array */
       memset(ifx_pmcu_accuDepList, 0, sizeof(ifx_pmcu_accuDepList));
       /* init accumulated dependency list */
       memset(ifx_pmcu_preCall, 0, sizeof(ifx_pmcu_preCall));
       /* init memory to process modules only once. */
       memset(ifx_pmcu_ModMem, 0, sizeof(ifx_pmcu_ModMem));
       /* fetch request from ringBuffer */
       /* init index of the ifx_pmcu_accuDepList */
       ifx_pmcu_indexAccuDepL = 0;
       
       reqState = ifx_pmcu_get_req();
       if(reqState.reqId == IFX_PMCU_PENDING_REQ_ERROR){
          PMCU_ERR(PMCU, "PMCU Request Buffer underflow\n");
          return;
       }
       if(reqState.reqId == IFX_PMCU_NO_PENDING_REQ){
          PMCU_ERR(PMCU, "No valid PMCU Request in buffer, but tasklet is scheduled!!!!!\n");
          return;
       }
       /* handle PMCU request */
       PMCU_DEBUG(PMCU, "REQ: pmcuModule=%s, pmcuModuleNr=%d, pmcuState=%s\n", ifx_pmcu_mod[reqState.moduleState.pmcuModule],
                                                                               reqState.moduleState.pmcuModuleNr,
                                                                               ifx_pmcu_ps[reqState.moduleState.pmcuState]);
    
       /* parse the dependency list and all nested dependencies */
       memcpy(&pmcuModuleState, &reqState.moduleState, sizeof(pmcuModuleState));
       if(ifx_pmcu_parse_deplist(pmcuModuleState) == IFX_PMCU_RETURN_ERROR){
          PMCU_ERR(PMCU, "Error occured during dependencyList parsing\n");
          continue;//return;
       }
       //PMCU_DEBUG_PRINT_ACCU_DEPLIST(IFX_PMCU_ACCU_DEPLIST_MAX); /* IFX_PMCU_ACCU_DEPLIST_MAX, ifx_pmcu_indexAccuDepL */
       //PMCU_DEBUG_PRINT_MOD_MEM();
    
       /* prepare pmcuModuleState for pre step */
       /* --> start pre processing by calling preCallBack functions of all affected modules */
       ret = ifx_pmcu_prechange(&pmcuPreCallIndex);
       //PMCU_DEBUG_PRINT_PRECALL(); /* printout after pre processing */
    
       if (ret == IFX_PMCU_RETURN_SUCCESS) {
          /* --> start processing state change by calling callBack functions of all affected modules */
          ret = ifx_pmcu_statechange(pmcuModuleState, &pmcuPreCallIndex);
          if (ret != IFX_PMCU_RETURN_SUCCESS) {
             /* error during ifx_pmcu_state_change() is not allowed! Report FatalError to the application*/
             PMCU_ERR(PMCU, "Error was returned from ifx_pmcu_state_change()\n");
          }
          //PMCU_DEBUG_PRINT_PRECALL(); /* printout after statechange */
       }
    
       /* --> start post processing by calling postCallBack functions of all affected modules */
       /* if any error occur during pre or statechange processing set new PowerState to old PowerState, to signalize no change */
       if(ret != IFX_PMCU_RETURN_SUCCESS){
          pmcuModuleState.pmcuState = ifx_pmcu_map[pmcuModuleState.pmcuModule].pmcuOldState[pmcuModuleState.pmcuModuleNr];
          PMCU_DEBUG(PMCU, "Error/Denied was returned from prechange/statechange\n");
       }
       ret = ifx_pmcu_postchange(pmcuModuleState, &pmcuPreCallIndex);
       if (ret != IFX_PMCU_RETURN_SUCCESS) {
          /* error during ifx_pmcu_postchange() is not allowed! Report FatalError to the application*/
          PMCU_ERR(PMCU, "Error was returned from ifx_pmcu_postchange()\n");
       }
       //PMCU_DEBUG_PRINT_PRECALL(); /* printout after post processing */
   }
}


IFX_PMCU_RETURN_t ifx_pmcu_state_req (IFX_PMCU_MODULE_t pmcuModule, IFX_uint8_t pmcuModuleNr, IFX_PMCU_STATE_t newState)
{
   IFX_PMCU_REQ_STATE_t reqState;
   IFX_int32_t ret;
   
   PMCU_DEBUG(PMCU, "ifx_pmcu_state_req() is called\n");
   /* check if the acception of the powerstate request is enabled */
   if (request_control_g == 0) {
       PMCU_INFO(PMCU, "The Powerstate request feature is disabled. All requests are rejected\n");
       return IFX_PMCU_RETURN_SUCCESS;
   }

   reqState.moduleState.pmcuModule = pmcuModule;
   reqState.moduleState.pmcuModuleNr = pmcuModuleNr;
   reqState.moduleState.pmcuState = newState;
   reqState.reqId = IFX_PMCU_NO_PENDING_REQ;

   /* check if the requested power state is supported for the HW.
      If this isn't the case request is rejected.
      Ask the cpu frequency driver if this Powerstate is supported by the HW */
   if(ifx_pmcu_map[IFX_PMCU_MODULE_CPU].pmcuRegister[0].pre == NULL){
	   PMCU_DEBUG(PMCU, "Frequency driver has no valid preCallback defined\n");
	   return IFX_PMCU_RETURN_DENIED;
   }
   ret = ifx_pmcu_map[IFX_PMCU_MODULE_CPU].pmcuRegister[0].pre(IFX_PMCU_MODULE_CPU, newState, IFX_PMCU_STATE_INVALID);
   if(ret != IFX_PMCU_RETURN_SUCCESS){
	  PMCU_DEBUG(PMCU, "Requested Powerstate %s is not supported by HW\n",ifx_pmcu_ps[newState]);
	  return IFX_PMCU_RETURN_DENIED;
   }   
         
   /* put new powerState request into request buffer */
   if(ifx_pmcu_put_req(reqState) == IFX_PMCU_RETURN_ERROR){
      PMCU_ERR(PMCU, "PMCU RequestBuffer overflow !!!\n");
   }
   
   /* fetch powerState request from requestBuffer with the next interrupt in the system */
   tasklet_hi_schedule(&ifx_pmcu_tasklet);

   return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Register a device driver to the PMCU

   \param [in]  pmcuRegister  Registration Information

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

     \ingroup IFX_PMCU_KERNEL
*/

IFX_PMCU_RETURN_t ifx_pmcu_register (IFX_PMCU_REGISTER_t* pmcuRegister)
{
   IFX_PMCU_MAP_t *entry = NULL;
   IFX_PMCU_RETURN_t retVal = IFX_PMCU_RETURN_ERROR;
   IFX_PMCU_MODULE_DEP_t *pmcuModuleDep;
   size_t depSize;
   IFX_ulong_t iflags;
   
   down(&ifx_pmcu_map_mutex);
   spin_lock_irqsave(&ifx_pmcu_lock, iflags);
   
   PMCU_DEBUG(PMCU, "pmcu_register is called by module %s\n",ifx_pmcu_mod[pmcuRegister->pmcuModule]);

   entry = &(ifx_pmcu_map[pmcuRegister->pmcuModule]);

   /* check registry status */
   if ( (entry->registryInfo[pmcuRegister->pmcuModuleNr] == IFX_PMCU_MODULE_NOT_REGISTRED) ||
        (entry->registryInfo[pmcuRegister->pmcuModuleNr] == IFX_PMCU_MODULE_UNREGISTERED))
   {
      /* check if at least pre/post callback function is defined */
      if ( (pmcuRegister->pre != NULL) && (pmcuRegister->post != NULL) ) {
         /* copy module registry struct into PMCU structure */
         memcpy(&entry->pmcuRegister[pmcuRegister->pmcuModuleNr], pmcuRegister, sizeof(IFX_PMCU_REGISTER_t));
         /* check if dependency list is defined */
         if(pmcuRegister->pmcuModuleDep == NULL){
            PMCU_DEBUG(PMCU, "PMCU/%s registry message: dependency list not defined\n",ifx_pmcu_mod[pmcuRegister->pmcuModule]);
            /* set registry flag for this module */
            entry->registryInfo[pmcuRegister->pmcuModuleNr] = IFX_PMCU_MODULE_REGISTERED;
            /* as defined the module must have D0 powerState after startup */
            entry->pmcuNewState[pmcuRegister->pmcuModuleNr] = IFX_PMCU_STATE_D0;
            entry->pmcuOldState[pmcuRegister->pmcuModuleNr] = IFX_PMCU_STATE_D0;
            spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
            up(&ifx_pmcu_map_mutex);
            return IFX_PMCU_RETURN_SUCCESS;
         }
         /* copy dependency list into PMCU structure because only a pointer is provided by the module
             because I do not know the size of the dependency array, I have to allocate memory during runtime*/
         depSize = (sizeof(IFX_PMCU_STATES_DEP_t)) * (pmcuRegister->pmcuModuleDep->nDepth) + (sizeof(pmcuRegister->pmcuModuleDep->nDepth)); 
         pmcuModuleDep = (IFX_PMCU_MODULE_DEP_t*)kmalloc(depSize, GFP_ATOMIC); /* use atomic kmalloc to be on the save side */
         if(pmcuModuleDep == NULL){ /* kmalloc failed */
            /* set NULL pointer */
            entry->pmcuRegister[pmcuRegister->pmcuModuleNr].pmcuModuleDep = NULL;
            PMCU_ERR(PMCU, "PMCU/%s memory allocation for dependency list failed\n",entry->pmcuModuleName);
            spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
            up(&ifx_pmcu_map_mutex);
            return IFX_PMCU_RETURN_ERROR;
         }
         //PMCU_DEBUG(PMCU, "PMCU/%s kmalloc for dependency list was successful during registration\n",ifx_pmcu_mod[pmcuRegister->pmcuModule]);
         /* copy dependency content */
         memcpy(pmcuModuleDep, pmcuRegister->pmcuModuleDep, depSize);
         /* overwrite the dependency pointer from the module in the registry struct with the pointer to the local PMCU copy */
         entry->pmcuRegister[pmcuRegister->pmcuModuleNr].pmcuModuleDep = pmcuModuleDep;
         /* set registry flag for this module */
         entry->registryInfo[pmcuRegister->pmcuModuleNr] = IFX_PMCU_MODULE_REGISTERED;
         /* as defined the module must have D0 powerState after startup */
         entry->pmcuNewState[pmcuRegister->pmcuModuleNr] = IFX_PMCU_STATE_D0;
         entry->pmcuOldState[pmcuRegister->pmcuModuleNr] = IFX_PMCU_STATE_D0;
         retVal = IFX_PMCU_RETURN_SUCCESS;

      } else {
         PMCU_ERR(PMCU, "PMCU/%s registry error: pre/post callback not defined\n",entry->pmcuModuleName);
         spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
         up(&ifx_pmcu_map_mutex);
         return IFX_PMCU_RETURN_ERROR;
      }

   } else { /* module already registered */
      retVal = IFX_PMCU_RETURN_DENIED;
   }

   spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
   up(&ifx_pmcu_map_mutex);
   return retVal;
}

/**
   Unregister a device driver to the PMCU

   \param [in] pmcuUnregister  Unregistration Information. Only module name is required.

   \return Returns value as follows:
     - IFX_SUCCESS: if successful
     - IFX_ERROR: in case of an error

     \ingroup IFX_PMCU_KERNEL
*/
IFX_PMCU_RETURN_t ifx_pmcu_unregister (IFX_PMCU_REGISTER_t* pmcuUnregister)
{
   IFX_PMCU_MAP_t *entry = NULL;
   IFX_ulong_t iflags;
   
   down(&ifx_pmcu_map_mutex);
   spin_lock_irqsave(&ifx_pmcu_lock, iflags);
   PMCU_DEBUG(PMCU, "pmcu_unregister is called by module %s\n",ifx_pmcu_mod[pmcuUnregister->pmcuModule]);
   entry = &(ifx_pmcu_map[pmcuUnregister->pmcuModule]);
   if(entry->registryInfo[pmcuUnregister->pmcuModuleNr] == IFX_PMCU_MODULE_REGISTERED ){
      /* free dynamic allocated memorey of dependency list */
      if(entry->pmcuRegister[pmcuUnregister->pmcuModuleNr].pmcuModuleDep != NULL){
         kfree(entry->pmcuRegister[pmcuUnregister->pmcuModuleNr].pmcuModuleDep);
         //PMCU_DEBUG(PMCU, "PMCU/%s kfree for dependency list was called during unregistration\n",entry->pmcuModuleName);
      }
      entry->registryInfo[pmcuUnregister->pmcuModuleNr] = IFX_PMCU_MODULE_UNREGISTERED;
      entry->pmcuNewState[pmcuUnregister->pmcuModuleNr] = IFX_PMCU_STATE_INVALID;
      entry->pmcuOldState[pmcuUnregister->pmcuModuleNr] = IFX_PMCU_STATE_INVALID;
      memset(&entry->pmcuRegister[pmcuUnregister->pmcuModuleNr], 0, sizeof(IFX_PMCU_REGISTER_t));
   }
   spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
   up(&ifx_pmcu_map_mutex);
   return IFX_PMCU_RETURN_SUCCESS;
};

static IFX_PMCU_RETURN_t ifx_pmcu_pwr_feature_switch (IFX_PMCU_MODULE_PWR_FEATURE_STATE_t pwrFeaSwitch)
{
   IFX_int32_t ret;
   IFX_PMCU_MODULE_STATE_t pmcuModState;

   PMCU_DEBUG(PMCU, "ifx_pmcu_pwr_feature_state() is called\n");

   /* check if the module supports enable/disable of power feature(s) */
   if(ifx_pmcu_map[pwrFeaSwitch.pmcuModule].pmcuRegister[pwrFeaSwitch.pmcuModuleNr].ifx_pmcu_pwr_feature_switch == NULL){
	   PMCU_DEBUG(PMCU, "Module %s has no valid Callback defined for pwr_feature_switch\n",ifx_pmcu_mod[pwrFeaSwitch.pmcuModule]);
	   return IFX_PMCU_RETURN_DENIED;
   }
   ret = ifx_pmcu_map[pwrFeaSwitch.pmcuModule].pmcuRegister[pwrFeaSwitch.pmcuModuleNr].ifx_pmcu_pwr_feature_switch(pwrFeaSwitch.pmcuPwrFeatureState);
   if(ret != IFX_PMCU_RETURN_SUCCESS){
	  PMCU_DEBUG(PMCU, "Requested Power Feature State is not supported by module %s\n",ifx_pmcu_mod[pwrFeaSwitch.pmcuModule]);
	  return IFX_PMCU_RETURN_DENIED;
   }   
   /* update power state in pmcu_map[] */
   pmcuModState.pmcuModule = pwrFeaSwitch.pmcuModule;
   pmcuModState.pmcuModuleNr = pwrFeaSwitch.pmcuModuleNr;
   pmcuModState.pmcuState = IFX_PMCU_STATE_INVALID;

   ret = ifx_pmcu_get_state(&pmcuModState);
   if(ret != IFX_PMCU_RETURN_SUCCESS){
      ifx_pmcu_map[pwrFeaSwitch.pmcuModule].pmcuOldState[pwrFeaSwitch.pmcuModuleNr] = IFX_PMCU_PWR_STATE_INVALID;
      return IFX_PMCU_RETURN_DENIED;
   } else {
      ifx_pmcu_map[pwrFeaSwitch.pmcuModule].pmcuOldState[pwrFeaSwitch.pmcuModuleNr] = pmcuModState.pmcuState;
   }
   return IFX_PMCU_RETURN_SUCCESS;
}


static IFX_PMCU_RETURN_t ifx_pmcu_get_state (IFX_PMCU_MODULE_STATE_t* pmcuModState)
{
   IFX_int32_t ret;
   IFX_PMCU_STATE_t pmcuState;

   PMCU_DEBUG(PMCU, "ifx_pmcu_get_state() is called\n");

   /* check if the module is registered. */
   if(ifx_pmcu_map[pmcuModState->pmcuModule].registryInfo[pmcuModState->pmcuModuleNr] != IFX_PMCU_MODULE_REGISTERED){
      PMCU_ERR(PMCU,"Module %s is not registered; no power state can be returned.\n",ifx_pmcu_mod[pmcuModState->pmcuModule]);
   }

   /* check if the callback for getting module status is defined. */
   if(ifx_pmcu_map[pmcuModState->pmcuModule].pmcuRegister[pmcuModState->pmcuModuleNr].ifx_pmcu_state_get == NULL){
	   PMCU_DEBUG(PMCU, "Module %s has no valid Callback defined for ifx_pmcu_state_get\n",ifx_pmcu_mod[pmcuModState->pmcuModule]);
	   return IFX_PMCU_RETURN_DENIED;
   }
   ret = ifx_pmcu_map[pmcuModState->pmcuModule].pmcuRegister[pmcuModState->pmcuModuleNr].ifx_pmcu_state_get(&pmcuState);
   if(ret != IFX_PMCU_RETURN_SUCCESS){
	  PMCU_DEBUG(PMCU, "Get state function return with error status for module %s\n",ifx_pmcu_mod[pmcuModState->pmcuModule]);
	  return IFX_PMCU_RETURN_DENIED;
   }   
   pmcuModState->pmcuState = pmcuState;      
   return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Recursive function to parse all dependency list's and create a accumulated list
   of modules and their necessary powerStates, for the requested powerState/module.
   The accumulated list is a static array called mcAccuDepList[].

   \param[in]   pmcuModuleState  (module, subNr, newState)

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

     \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_parse_deplist (IFX_PMCU_MODULE_STATE_t pmcuModuleState)
{
   IFX_PMCU_MODULE_DEP_t *entryModDep = NULL; /* points to the Dependency struct of the requested subModule */
   IFX_uint8_t i = 0;
   IFX_PMCU_MODULE_STATE_t moduleState;
   IFX_PMCU_MODULE_t pmcuModule = IFX_PMCU_MODULE_PMCU;
   IFX_PMCU_STATE_t pmcuState = IFX_PMCU_STATE_INVALID, pmcuOldState = IFX_PMCU_STATE_INVALID;
   IFX_uint8_t pmcuModuleNr = 0;
 
   //PMCU_DEBUG(PMCU, "parse start: pmcuState=%s, pmcuModule=%s\n",ifx_pmcu_ps[pmcuModuleState.pmcuState],ifx_pmcu_mod[pmcuModuleState.pmcuModule]);
 
   /* is module already processed? */
   if(!ifx_pmcu_ModMem[pmcuModuleState.pmcuModule]){
      /* No. Module is not in the list. Create a new entry */
      ifx_pmcu_ModMem[pmcuModuleState.pmcuModule] = 1; /* set marker */
      memcpy(&ifx_pmcu_accuDepList[ifx_pmcu_indexAccuDepL], &pmcuModuleState, sizeof(pmcuModuleState));
      ifx_pmcu_indexAccuDepL++;
   }
   /* points to the Dependency struct of the requested subModule */
   entryModDep = ifx_pmcu_map[pmcuModuleState.pmcuModule].pmcuRegister[pmcuModuleState.pmcuModuleNr].pmcuModuleDep;
   /* check if a dependency list exists for the module */
   if(entryModDep == NULL){
      /* end of processing; no further dependency list available. */
     return IFX_PMCU_RETURN_SUCCESS;
   }
   /* Dependency list exist.
      Check if dependency list exceeds maximum number of entries 'IFX_DEPLIST_SIZE' */
   if(entryModDep->nDepth >= IFX_DEPLIST_SIZE){
      PMCU_ERR(PMCU, "Dependency list of module '%s' is greater than %i\n",ifx_pmcu_mod[pmcuModuleState.pmcuModule],IFX_DEPLIST_SIZE);
      return IFX_PMCU_RETURN_ERROR;
   }
   /* loop over the dependency list */
   for (i = 0; i < entryModDep->nDepth; i++) {
      switch(pmcuModuleState.pmcuState){ /* switch on requested powerState */
         case IFX_PMCU_STATE_D0:
            //PMCU_DEBUG(PMCU, "change to powerState IFX_PMCU_STATE_D0\n");
            pmcuState = entryModDep->moduleStates[i].onState;
            pmcuModule = entryModDep->moduleStates[i].pmcuModule;
            pmcuModuleNr = 0;/* only sub module 0 is supported */
            break;
         case IFX_PMCU_STATE_D1:
            //PMCU_DEBUG(PMCU, "change to powerState IFX_PMCU_STATE_D1\n");
            pmcuState = entryModDep->moduleStates[i].standBy;
            pmcuModule = entryModDep->moduleStates[i].pmcuModule;
            pmcuModuleNr = 0;/* only sub module 0 is supported */
            break;
         case IFX_PMCU_STATE_D2:
            //PMCU_DEBUG(PMCU, "change to powerState IFX_PMCU_STATE_D2\n");
            pmcuState = entryModDep->moduleStates[i].lpStandBy;
            pmcuModule = entryModDep->moduleStates[i].pmcuModule;
            pmcuModuleNr = 0;/* only sub module 0 is supported */
            break;
         case IFX_PMCU_STATE_D3:
            //PMCU_DEBUG(PMCU, "change to powerState IFX_PMCU_STATE_D3\n");
            pmcuState = entryModDep->moduleStates[i].offState;
            pmcuModule = entryModDep->moduleStates[i].pmcuModule;
            pmcuModuleNr = 0;/* only sub module 0 is supported */
            break;
         default:
            PMCU_ERR(PMCU, "invalid PowerState was selected\n");
            /* error handling invalid state */
            return IFX_PMCU_RETURN_ERROR;
      }
      /* get current state of the module from the dependency list */
      pmcuOldState = ifx_pmcu_map[pmcuModule].pmcuOldState[pmcuModuleNr];
      /* check if there is anything todo for this module */   
      /* compare the CURRENT state with the MASK from the dependency list */

      //PMCU_DEBUG(PMCU, "oldState=%s, DepMask=%s, pmcuModule=%s\n",ifx_pmcu_ps[pmcuOldState],ifx_pmcu_ps[pmcuState],ifx_pmcu_mod[pmcuModule]);
      //PMCU_DEBUG_PRINT_MOD_MEM();
      /* is module already processed? if yes skip this module */
      if(ifx_pmcu_ModMem[pmcuModule]){
         /* yes. Module is already in the list. goto next entry */
         continue;
      }
      //PMCU_DEBUG(PMCU, "pmcu_stateMap=%d\n",ifx_pmcu_stateMap[pmcuState][pmcuOldState]);
      if(ifx_pmcu_stateMap[pmcuState][pmcuOldState] == 1){
         /* here we have the recursive call of this function */
         moduleState.pmcuModule = pmcuModule;
         moduleState.pmcuState = pmcuState;
         moduleState.pmcuModuleNr = pmcuModuleNr;
         ifx_pmcu_parse_deplist(moduleState);
      }else{
         /* fetch next module from depenedency list */
         PMCU_DEBUG(PMCU, "no dependency activity necessary for module '%s'\n",ifx_pmcu_mod[pmcuModule]);
      }
   }
   return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Apply all callbacks registered to be executed before a state change for pmcuModule.
   The input for this function is the accumulated dependency list ifx_pmcu_accuDepList[] which 
   is filled by ifx_pmcu_parse_deplist().

   \param[out]  pmcuPreCallIndex index to the last element inside the ifx_pmcu_preCall[][] array

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error
     - IFX_PMCU_RETURN_DENIED in case that the powerState change was rejected

     \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_prechange (IFX_int8_t* pmcuPreCallIndex)
{
   IFX_PMCU_RETURN_t ret = IFX_PMCU_RETURN_SUCCESS;
   IFX_int8_t i;
   IFX_PMCU_MODULE_t pmcuModule;
   IFX_PMCU_STATE_t pmcuState, pmcuOldState;
   IFX_uint8_t pmcuModuleNr;
   
   PMCU_DEBUG(PMCU, "preCallback processing is called\n");

   /* check if we have one or more entries in the table */
   if(ifx_pmcu_indexAccuDepL == 0){
      PMCU_DEBUG(PMCU, "nothing todo; accumulated depList is empty\n");
   }
   /* loop over the accumulated dependency list in reverse order to follow the processing sequence 
      defined in the dependency lists */
//   for (i = 0; i < ifx_pmcu_indexAccuDepL; i++) {
   for (i = (ifx_pmcu_indexAccuDepL-1); i >= 0; i--) {
      pmcuModule = ifx_pmcu_accuDepList[i].pmcuModule;
      pmcuModuleNr = ifx_pmcu_accuDepList[i].pmcuModuleNr;
      pmcuState = ifx_pmcu_accuDepList[i].pmcuState;
      pmcuOldState = ifx_pmcu_map[pmcuModule].pmcuOldState[pmcuModuleNr];
      /* check if module is registered in PMCU */
      if(ifx_pmcu_map[pmcuModule].registryInfo[pmcuModuleNr] != IFX_PMCU_MODULE_REGISTERED){
         PMCU_ERR(PMCU, "module '%s' listed in dependency list is not registered in PMCU\n",ifx_pmcu_mod[pmcuModule]);
         continue;
      }
      /* check if preCallback is defined */
      if(ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].pre == NULL){
         PMCU_DEBUG(PMCU, "no preCallback defined for module '%s'\n",ifx_pmcu_mod[pmcuModule]);
         continue;
      }
      /* mark pre call in list for later use in change/post callbacks */
      (*pmcuPreCallIndex)++; /* next free entry */
      ifx_pmcu_preCall[*pmcuPreCallIndex][0] = pmcuModule; /* module name */
      ifx_pmcu_preCall[*pmcuPreCallIndex][1] = pmcuModuleNr; /* subModule no. */
      ifx_pmcu_preCall[*pmcuPreCallIndex][2] = pmcuState; /* powerState */
      ifx_pmcu_preCall[*pmcuPreCallIndex][3] = 1; /* preCall was made */
      if(ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].pre != NULL){
         /* call pre callback of the module */
         PMCU_DEBUG(PMCU, "pre Callback of module '%s' is called\n",ifx_pmcu_mod[pmcuModule]);
         ret = ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].pre(pmcuModule, pmcuState, pmcuOldState);
      }
      if(ret != IFX_PMCU_RETURN_SUCCESS){
         /* if one denied will be returned, pre-processing can be interrupted */
         PMCU_DEBUG(PMCU, "preCallback of module '%s' return DENIED\n",ifx_pmcu_mod[pmcuModule]);
         return IFX_PMCU_RETURN_DENIED;
      }   
      PMCU_DEBUG(PMCU, "module '%s' accept preCallback\n",ifx_pmcu_mod[pmcuModule]);
   }
   return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Apply all callbacks registered to be executed if a state change is requested and accepted.

   \param[in]   pmcuModuleState  (module, subNr, newState)
   \param[out]  pmcuPreCallIndex index to the last element inside the ifx_pmcu_preCall[][] array

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

   \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_statechange (IFX_PMCU_MODULE_STATE_t pmcuModuleState, IFX_int8_t* pmcuPreCallIndex)
{
   IFX_PMCU_RETURN_t ret = IFX_PMCU_RETURN_SUCCESS;
   IFX_uint8_t i;
   IFX_uint8_t pmcuModuleNr;
   IFX_PMCU_MODULE_t pmcuModule;
   IFX_PMCU_STATE_t pmcuState;

   PMCU_DEBUG(PMCU, "statechangeCallback processing is called\n");
   
   /* check if there is anything todo */
   if(*pmcuPreCallIndex == -1){
      PMCU_DEBUG(PMCU, "nothing todo for statechange processing\n");
      return IFX_PMCU_RETURN_SUCCESS;
   }
   
   /* loop over the ifx_pmcu_preCall[][] list to call the statechange callbacks */
   for(i = 0; i <= *pmcuPreCallIndex; i++){
      if(ifx_pmcu_preCall[i][3] != 1){
         PMCU_ERR(PMCU, "error in ifx_pmcu_preCall[][] list during statechangeCallback processing\n");
         return IFX_PMCU_RETURN_ERROR;
      }
      pmcuModule = ifx_pmcu_preCall[i][0]; /* fetch module name */
      pmcuModuleNr = ifx_pmcu_preCall[i][1]; /* fetch subModule No. */
      pmcuState = ifx_pmcu_preCall[i][2]; /* fetch powerState */
      ifx_pmcu_preCall[i][4] = 1; /* statechangeCall was made */
      if(ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].ifx_pmcu_state_change != NULL){
         PMCU_DEBUG(PMCU, "ifx_pmcu_state_change Callback of module '%s' is called\n",ifx_pmcu_mod[pmcuModule]);
         ret = ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].ifx_pmcu_state_change(pmcuState);
      }
      if(ret != IFX_PMCU_RETURN_SUCCESS){
         PMCU_ERR(PMCU, "ifx_pmcu_state_change Callback of module '%s' return ERROR\n",ifx_pmcu_mod[pmcuModule]);
         return IFX_PMCU_RETURN_ERROR;
      }
   }
   return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Apply all callbacks registered to be executed after a state change for pmcuModule was done,
   or a request was denied.

   \param[in]   pmcuModuleState  (module, subNr, newState)
   \param[out]  pmcuPreCallIndex index to the last element inside the ifx_pmcu_preCall[][] array

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error

   \ingroup IFX_PMCU_KERNEL
*/
static IFX_PMCU_RETURN_t ifx_pmcu_postchange (IFX_PMCU_MODULE_STATE_t pmcuModuleState, IFX_int8_t* pmcuPreCallIndex)
{
   IFX_PMCU_RETURN_t ret = IFX_PMCU_RETURN_SUCCESS;
   IFX_uint8_t i,denied = 0;
   IFX_uint8_t pmcuModuleNr;
   IFX_PMCU_MODULE_t pmcuModule;
   IFX_PMCU_STATE_t pmcuState, pmcuOldState;

   PMCU_DEBUG(PMCU, "postCallback processing is called\n");
   
   /* check if there is anything todo */
   if(*pmcuPreCallIndex == -1){
      PMCU_DEBUG(PMCU, "nothing todo for statechange processing\n");
      return IFX_PMCU_RETURN_SUCCESS;
   }
   
   /* check if stateChange was accepted or denied */
   if(ifx_pmcu_map[pmcuModuleState.pmcuModule].pmcuOldState[pmcuModuleState.pmcuModuleNr] == pmcuModuleState.pmcuState){
      PMCU_DEBUG(PMCU, "requested powerState was denied\n");
      denied = 1; /* set denied indicator */
   }
   
   /* loop over the ifx_pmcu_preCall[][] list to call the post callbacks */
   for(i = 0; i <= *pmcuPreCallIndex; i++){
      if(ifx_pmcu_preCall[i][3] != 1){
         PMCU_ERR(PMCU, "error in ifx_pmcu_preCall[][] list during postCallback processing\n");
         return IFX_PMCU_RETURN_ERROR;
      }
      pmcuModule = ifx_pmcu_preCall[i][0]; /* fetch module name */
      pmcuModuleNr = ifx_pmcu_preCall[i][1]; /* fetch subModule No. */
      pmcuState = ifx_pmcu_preCall[i][2]; /* fetch powerState */
      ifx_pmcu_preCall[i][5] = 1; /* postCall was made */
      pmcuOldState = ifx_pmcu_map[pmcuModule].pmcuOldState[pmcuModuleNr];
      if(denied){
         pmcuState = pmcuOldState;
      }
      if(ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].post != NULL){
         PMCU_DEBUG(PMCU, "post Callback of module '%s' is called\n",ifx_pmcu_mod[pmcuModule]);
         ret = ifx_pmcu_map[pmcuModule].pmcuRegister[pmcuModuleNr].post(pmcuModule, pmcuState, pmcuOldState);
      }
      if(ret != IFX_PMCU_RETURN_SUCCESS){
         PMCU_ERR(PMCU, "post Callback of module '%s' return ERROR\n",ifx_pmcu_mod[pmcuModule]);
         return IFX_PMCU_RETURN_ERROR;
      }
      /* store new state inside PMCU mapTable */
      ifx_pmcu_map[pmcuModule].pmcuOldState[pmcuModuleNr] = pmcuState;
      ifx_pmcu_map[pmcuModule].pmcuNewState[pmcuModuleNr] = pmcuState;
   }
   return IFX_PMCU_RETURN_SUCCESS;
}


static ssize_t ifx_pmcu_read( struct file *filp, char *buf, size_t count, loff_t *ppos )
{
   return count;
}


static ssize_t ifx_pmcu_write( struct file *filp, const char *buf, size_t count, loff_t *ppos )
{
   return 0;
}


static int ifx_pmcu_open(struct inode * inode, struct file * file)
{
   return 0;
}


static int ifx_pmcu_release(struct inode *inode, struct file *file)
{
   return 0;
}


static unsigned int ifx_pmcu_poll(struct file *filp, poll_table *wait)
{
   poll_wait(filp, &ifx_pmcu_wait_q, wait);

   return 0;
}


/**
   Configuration / Control for the device.

   \param  inode       Pointer to the inode.
   \param  file        Pointer to the file descriptor.
   \param  cmd         IOCTL identifier.
   \param  arg         Optional argument.

   \return
   - 0 and positive values - success
   - negative value - ioctl failed
*/
static int ifx_pmcu_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
   int ret = IFX_ERROR;
   int retValue;
   IFX_PMCU_MODULE_STATE_t usrModuleState;
   IFX_int32_t llevel = 0;
   IFX_int32_t req_ctrl;
   IFX_PMCU_MODULE_PWR_FEATURE_STATE_t pwrFeatureState;

   switch (cmd)
   {
      case IFX_PMCU_STATE_GET:
      {
         PMCU_DEBUG(PMCU, "ifx_pmcu_ioctl IFX_PMCU_STATE_GET is called\n");
         copy_from_user(&usrModuleState, (IFX_PMCU_MODULE_STATE_t *)arg, sizeof(IFX_PMCU_MODULE_STATE_t));
         down(&ifx_pmcu_map_mutex);
         retValue = ifx_pmcu_get_state (&usrModuleState);
         up(&ifx_pmcu_map_mutex);
         if(retValue == IFX_PMCU_RETURN_SUCCESS){
            ret = IFX_SUCCESS;
         }
         copy_to_user((IFX_PMCU_MODULE_STATE_t *)arg, &usrModuleState, sizeof(IFX_PMCU_MODULE_STATE_t));
         ret = IFX_SUCCESS;
         break;
      }
      case IFX_PMCU_STATE_REQ:
      {
         PMCU_DEBUG(PMCU, "ifx_pmcu_ioctl IFX_PMCU_STATE_REQ is called\n");
         copy_from_user(&usrModuleState, (IFX_PMCU_MODULE_STATE_t *)arg, sizeof(IFX_PMCU_MODULE_STATE_t));
         retValue = ifx_pmcu_state_req (usrModuleState.pmcuModule, usrModuleState.pmcuModuleNr, usrModuleState.pmcuState);
         if(retValue == IFX_PMCU_RETURN_SUCCESS){
            ret = IFX_SUCCESS;
         }
         break;
      }
      case IFX_PMCU_REQ_CTRL:
      {
         PMCU_DEBUG(PMCU, "ifx_pmcu_ioctl IFX_PMCU_REQ_CTRL is called\n");
         copy_from_user(&req_ctrl, (IFX_int32_t *)arg, sizeof(IFX_int32_t));
         if (req_ctrl == 0) {
             request_control_g = 0;
         } else {
             request_control_g = 1;
         }
         ret = IFX_SUCCESS;
         break;
      }
      case IFX_PMCU_LOG_LEVEL:
      {
         PMCU_DEBUG(PMCU, "ifx_pmcu_ioctl IFX_PMCU_LOG_LEVEL is called\n");
         copy_from_user(&llevel, (IFX_int32_t *)arg, sizeof(IFX_int32_t));
         log_level_g = llevel;
         ret = IFX_SUCCESS;
         break;
      }
      case IFX_PMCU_PWR_FEATURE_SWITCH:
      {
         PMCU_DEBUG(PMCU, "ifx_pmcu_ioctl IFX_PMCU_PWR_FEATURE_SWITCH is called\n");
         copy_from_user(&pwrFeatureState, (IFX_PMCU_MODULE_PWR_FEATURE_STATE_t *)arg, sizeof(IFX_PMCU_MODULE_PWR_FEATURE_STATE_t));
         down(&ifx_pmcu_map_mutex);
         retValue = ifx_pmcu_pwr_feature_switch (pwrFeatureState);
         up(&ifx_pmcu_map_mutex);
         if(retValue == IFX_PMCU_RETURN_SUCCESS){
            ret = IFX_SUCCESS;
         }
         break;
      }
      default:
         ret = IFX_ERROR;
   }

   return ret;
}


static struct file_operations ifx_pmcu_fops = {
#ifdef MODULE
   owner:
      THIS_MODULE,
#endif   /* MODULE */
   read:
      ifx_pmcu_read,
   write:
      ifx_pmcu_write,
   ioctl:
      ifx_pmcu_ioctl,
   open:
      ifx_pmcu_open,
   release:
      ifx_pmcu_release,
   poll:
      ifx_pmcu_poll,
};


#ifdef CONFIG_PROC_FS
static int ifx_pmcu_proc_device_list(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
   int len = 0;
   int i = 0, j = 0;

   len += sprintf(buf+len, "PMCU Request Control = %s\n\n",((request_control_g == 1)?"Enabled":"Disabled"));
   len += sprintf(buf+len, "Legend:\n");
   len += sprintf(buf+len, "\tr\t:registered (-=not registered, 1=unregistered, 2=registered)\n");
   len += sprintf(buf+len, "\tSub#\t:Submodule or module instance\n");
   len += sprintf(buf+len, "\tps\t:power state (-1=invalid state, D0=on, D1=lp1, D2=lp2, D3=off n.d.=no pmcu_state_get() defined)\n\n");
   len += sprintf(buf+len, "Modules\t\tSub0\tSub1\tSub2\tSub3\tSub4\tSub5\n");
   len += sprintf(buf+len, "\t\tr/ps\tr/ps\tr/ps\tr/ps\tr/ps\tr/ps\n");

   down(&ifx_pmcu_map_mutex);
   for (i = 0; i < IFX_PMCU_MODULE_ID_MAX ; i++) {
      if(strlen(ifx_pmcu_map[i].pmcuModuleName) < 8 ){
         len += sprintf(buf + len, "%s\t\t", ifx_pmcu_map[i].pmcuModuleName);
      }else{
         len += sprintf(buf + len, "%s\t", ifx_pmcu_map[i].pmcuModuleName);
      }
      for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
         if( (ifx_pmcu_map[i].registryInfo[j] != IFX_PMCU_MODULE_REGISTERED) || (ifx_pmcu_map[i].pmcuRegister[j].pmcuModule == IFX_PMCU_MODULE_PMCU) ){
            len += sprintf(buf+len,"-\t");
         }else{
            len += sprintf(buf + len, "%d/", ifx_pmcu_map[i].registryInfo[j]);
            len += sprintf(buf+len,"%s\t",ifx_pmcu_ps[ifx_pmcu_map[i].pmcuOldState[j]]);
         }
      }
      len += sprintf(buf+len,"\n");
   }
   up(&ifx_pmcu_map_mutex);

   return len;
}

static int ifx_pmcu_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
   int len = 0;

   len += sprintf(buf+len, "IFX_PMCU Version = %s\n",IFX_PMCU_DRV_VERSION);
   len += sprintf(buf+len, "Compiled on %s, %s for Linux kernel %s\n",__DATE__, __TIME__, UTS_RELEASE);

   return len;
}

static int ifx_pmcu_proc_get_internal_map(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
   int len = 0;
   int i=0, j = 0;

   len += sprintf(buf+len, "internal MAP-Table\n");

   down(&ifx_pmcu_map_mutex);
   len += sprintf(buf + len, "pmcu_map[%d]->pmcuModuleName=%s\n", (int)data, ifx_pmcu_map[(int)data].pmcuModuleName);
   for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
      len += sprintf(buf + len, "registryInfo[%d]=%d\t", j, ifx_pmcu_map[(int)data].registryInfo[j]);
   }
   len += sprintf(buf+len,"\n");
   for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
      len += sprintf(buf + len, "pmcuOldState[%d]=%d\t", j, ifx_pmcu_map[(int)data].pmcuOldState[j]);
   }
   len += sprintf(buf+len,"\n");
   for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
      len += sprintf(buf + len, "pmcuNewState[%d]=%d\t", j, ifx_pmcu_map[(int)data].pmcuNewState[j]);
   }
   len += sprintf(buf+len,"\n\n");
   for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
      len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModule=%d\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModule);
      len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleNr=%d\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleNr);
      len += sprintf(buf + len, "pmcuRegister[%d]->ifx_pmcu_state_change=%p\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].ifx_pmcu_state_change);
      len += sprintf(buf + len, "pmcuRegister[%d]->ifx_pmcu_state_get=%p\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].ifx_pmcu_state_get);
      len += sprintf(buf + len, "pmcuRegister[%d]->ifx_pmcu_pwr_feature_switch=%p\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].ifx_pmcu_pwr_feature_switch);
      len += sprintf(buf + len, "pmcuRegister[%d]->pre=%p\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].pre);
      len += sprintf(buf + len, "pmcuRegister[%d]->post=%p\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].post);
      len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep=%p\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep);
      if(ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep != NULL){
         len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep->nDepth=%d\n", j, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->nDepth);
      }
      len += sprintf(buf+len,"\n");
   }
   for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
      if(ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep != NULL){
         for(i=0;i<ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->nDepth;i++){
            len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep[%d]->pmcuModule=%s\n", j, i, ifx_pmcu_mod[ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->moduleStates[i].pmcuModule]);
            len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep[%d]->onState\t=%d\n", j, i, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->moduleStates[i].onState);
            len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep[%d]->standBy\t=%d\n", j, i, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->moduleStates[i].standBy);
            len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep[%d]->lpStandBy\t=%d\n", j, i, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->moduleStates[i].lpStandBy);
            len += sprintf(buf + len, "pmcuRegister[%d]->pmcuModuleDep[%d]->offState\t=%d\n", j, i, ifx_pmcu_map[(int)data].pmcuRegister[j].pmcuModuleDep->moduleStates[i].offState);
            len += sprintf(buf+len,"\n");
         }
      }
   }
   up(&ifx_pmcu_map_mutex);

   len += sprintf(buf+len,"\n");
   return len;
}


/**
   Initialize and install the proc entry

   \return
   -1 or 0 on success
*/
static int ifx_pmcu_proc_EntriesInstall(void)
{
   /*create pmcu proc entry*/
   g_ifx_pmcu_dir_pmcu = proc_mkdir("pmcu", NULL);
   create_proc_read_entry("device_list", 0, g_ifx_pmcu_dir_pmcu, ifx_pmcu_proc_device_list, NULL);
   create_proc_read_entry("version", 0, g_ifx_pmcu_dir_pmcu, ifx_pmcu_proc_version, NULL);
   g_ifx_pmcu_dir_debug = proc_mkdir("pmcu/debug", NULL);
   create_proc_read_entry("cpu", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_CPU);
   create_proc_read_entry("eth", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_ETH);
   create_proc_read_entry("usb", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_USB);
   create_proc_read_entry("dsl", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_DSL);
   create_proc_read_entry("wlan", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_WLAN);
   create_proc_read_entry("dect", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_DECT);
   create_proc_read_entry("fxs", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_FXS);
   create_proc_read_entry("fxo", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_FXO);
   create_proc_read_entry("ve", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_VE);
   create_proc_read_entry("ppe", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_PPE);
   create_proc_read_entry("switch", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_SWITCH);
   create_proc_read_entry("uart", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_UART);
   create_proc_read_entry("spi", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_SPI);
   create_proc_read_entry("sdio", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_SDIO);
   create_proc_read_entry("pci", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_PCI);
   create_proc_read_entry("vlynq", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_VLYNQ);
   create_proc_read_entry("deu", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_DEU);
   create_proc_read_entry("cpu_ps", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_CPU_PS);
   create_proc_read_entry("gptu", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_GPTC);
   create_proc_read_entry("usif_uart", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_USIF_UART);
   create_proc_read_entry("usif_spi", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_USIF_SPI);
   create_proc_read_entry("pcie", 0, g_ifx_pmcu_dir_debug, ifx_pmcu_proc_get_internal_map, (void*)IFX_PMCU_MODULE_PCIE);
   return 0;
}


/**
   Initialize and install the proc entry

   \return
   -1 or 0 on success
*/
static int ifx_pmcu_proc_EntriesRemove(void)
{
   remove_proc_entry("pcie", g_ifx_pmcu_dir_debug);
   remove_proc_entry("usif_spi", g_ifx_pmcu_dir_debug);
   remove_proc_entry("usif_uart", g_ifx_pmcu_dir_debug);
   remove_proc_entry("gptu", g_ifx_pmcu_dir_debug);
   remove_proc_entry("cpu_ps", g_ifx_pmcu_dir_debug);
   remove_proc_entry("deu", g_ifx_pmcu_dir_debug);
   remove_proc_entry("vlynq", g_ifx_pmcu_dir_debug);
   remove_proc_entry("pci", g_ifx_pmcu_dir_debug);
   remove_proc_entry("sdio", g_ifx_pmcu_dir_debug);
   remove_proc_entry("spi", g_ifx_pmcu_dir_debug);
   remove_proc_entry("uart", g_ifx_pmcu_dir_debug);
   remove_proc_entry("switch", g_ifx_pmcu_dir_debug);
   remove_proc_entry("ppe", g_ifx_pmcu_dir_debug);
   remove_proc_entry("ve", g_ifx_pmcu_dir_debug);
   remove_proc_entry("fxo", g_ifx_pmcu_dir_debug);
   remove_proc_entry("fxs", g_ifx_pmcu_dir_debug);
   remove_proc_entry("dect", g_ifx_pmcu_dir_debug);
   remove_proc_entry("wlan", g_ifx_pmcu_dir_debug);
   remove_proc_entry("dsl", g_ifx_pmcu_dir_debug);
   remove_proc_entry("usb", g_ifx_pmcu_dir_debug);
   remove_proc_entry("eth", g_ifx_pmcu_dir_debug);
   remove_proc_entry("cpu", g_ifx_pmcu_dir_debug);
   remove_proc_entry(g_ifx_pmcu_dir_debug->name, NULL);
   g_ifx_pmcu_dir_debug = NULL;
   remove_proc_entry("version", g_ifx_pmcu_dir_pmcu);
   remove_proc_entry("device_list", g_ifx_pmcu_dir_pmcu);
   remove_proc_entry(g_ifx_pmcu_dir_pmcu->name, NULL);
   g_ifx_pmcu_dir_pmcu = NULL;
   return 0;
}
#endif /* CONFIG_PROC_FS */


static int __init ifx_pmcu_init(void)
{
   int err = 0;
   int i = 0;
   IFX_ulong_t iflags;

   PMCU_DEBUG(PMCU, "ifx_pmcu_init is called\n");
   /* PMCU device map table is initialized during declaration */

   /* init powerState request ringbuffer */
   memset(ifx_pmcu_reqBuffer, 0, sizeof(ifx_pmcu_reqBuffer));
   
   /* register port device */
#ifdef PMCU_DYNAMIC_MAJOR_NO
   ifx_pmcu_major = register_chrdev (0, IFX_PMCU_DEVICE_NAME, &ifx_pmcu_fops);
   if (ifx_pmcu_major < 0) {
      PMCU_ERR(PMCU, "ifx-pmcu: Error! Could not register pmcu device. #%d\n", err);
      return ifx_pmcu_major;
   }
#else
   err = register_chrdev (ifx_pmcu_major, IFX_PMCU_DEVICE_NAME, &ifx_pmcu_fops);
   if (err != 0) {
      PMCU_ERR(PMCU, "ifx-pmcu: Error! Could not register pmcu device. #%d\n", err);
      return err;
   }
#endif

#ifdef CONFIG_PROC_FS
   ifx_pmcu_proc_EntriesInstall();
#endif
   down(&ifx_pmcu_map_mutex);
   spin_lock_irqsave(&ifx_pmcu_lock, iflags);
/* Init PMCU device map table: only module name; prerequisite->compiler initialise maptable to NULL!!
   Sequence of calling module_init by the kernel can not be influenced in a regulare way. Therefore it may happen that
   a pmcu_register is called by an other module before pmcu_module_init is called. That's why the map table must be
   initialised before runtime! */
   for (i = 0; i < IFX_PMCU_MODULE_ID_MAX; i++) {
      ifx_pmcu_map[i].pmcuModuleName = ifx_pmcu_mod[i];
   }
   spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
   up(&ifx_pmcu_map_mutex);
   
   init_waitqueue_head(&ifx_pmcu_wait_q);


   PMCU_DEBUG(PMCU, "Lantiq PMCU Device Driver Version %s\n", IFX_PMCU_DRV_VERSION);

   return 0;
}

static void __exit ifx_pmcu_exit (void)
{
   int i = 0, j = 0;
   IFX_ulong_t iflags;

   PMCU_DEBUG(PMCU, "ifx_pmcu_exit is called\n");
   down(&ifx_pmcu_map_mutex);
   spin_lock_irqsave(&ifx_pmcu_lock, iflags);
   /* clear PMCU device map table: first free allocated memory for dependency list's; than set entire map table to '0'*/
   for (i = 0; i < IFX_PMCU_MODULE_ID_MAX; i++) {
      for (j = 0; j < MAX_PMCU_SUB_MODULE; j++) {
         if(ifx_pmcu_map[i].pmcuRegister[j].pmcuModuleDep != NULL){
            kfree(ifx_pmcu_map[i].pmcuRegister[j].pmcuModuleDep);
         }
      }
   }
   memset(ifx_pmcu_map, 0, sizeof(IFX_PMCU_MAP_t));
   spin_unlock_irqrestore(&ifx_pmcu_lock, iflags);
   up(&ifx_pmcu_map_mutex);
   
   #ifdef CONFIG_PROC_FS
   ifx_pmcu_proc_EntriesRemove();
#endif

   unregister_chrdev(ifx_pmcu_major, IFX_PMCU_DEVICE_NAME);

   return;
}

module_init (ifx_pmcu_init);
module_exit (ifx_pmcu_exit);

EXPORT_SYMBOL(ifx_pmcu_state_req);
EXPORT_SYMBOL(ifx_pmcu_register);
EXPORT_SYMBOL(ifx_pmcu_unregister);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("Lantiq Deutschland GmbH");
MODULE_DESCRIPTION ("LANTIQ PMCU driver");
MODULE_SUPPORTED_DEVICE ("Amazon SE, Danube, XRX100, XRX200");
