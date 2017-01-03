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
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/ifx/ifx_clk.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_pmcu.h>

/**
   \defgroup LQ_CPUFREQ CPU Frequency Driver
   \ingroup LQ_COC
   Ifx cpu frequency driver module
*/
/* @{ */


/**
   \defgroup LQ_CPUFREQ_PRG Program Guide
   \ingroup LQ_CPUFREQ
   <center>
   <B>"CPU Frequency Driver ( CPUFREQ )" </B>.
   </center>    
   \section Purpose Purpose
    The CPU Frequency driver was introduced to map the hardware independent power states (D0,D1,D2,D3) to the 
	hardware dependent clock frequencies. Since UGW5.1 the driver supports additionally the voltage scaling feature for VR9 only.
	The voltage scaling means that each frequency pair for CPU/DDR has also a corresponding voltage value for the 1V core power domain.
	Means higher frequency -> higher voltage and vice versa.
	Because of several critical frequency transitions, the CPUFREQ driver takes also care about these and provide a workaround if possible.\n\n\n
   Supported features in PROC FS:
        - read version code of cpufreq driver
        - read chip_id from HW register
        - enable/disable voltage scaling support
        - set log level of the cpufreq driver
   \n
   \n
  
*/

/*=============================================================================*/
/* CPUFREQ DEBUG switch                                                        */
/*=============================================================================*/
#define CPUFREQ_DEBUG_MODE

/* CPUFREQ driver version  */
#define IFX_CPUFREQ_DRV_VERSION  "3.0.2.0"

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

/* CPUFREQ printk specific's; colored print_message support */
/******************************************************************************/
#define BLINK   "\033[31;1m"
#define RED     "\033[31;1m"
#define YELLOW  "\033[33;1m"
#define GREEN   "\033[32;2m"
#define BLUE    "\033[34;1m"
#define CYAN    "\033[36;2m"
#define DIM     "\033[37;1m"
#define NORMAL  "\033[0m"

#define CPUFREQ_EMERG(module,fmt,arg...)\
        if(log_level_g > 0){\
        printk(KERN_EMERG RED #module NORMAL ": " fmt,##arg);\
        }

#define CPUFREQ_ERR(module,fmt,arg...)\
        if(log_level_g > 0){\
        printk(KERN_ERR RED #module NORMAL ": " fmt,##arg);\
        }

#define CPUFREQ_WARNING(module,fmt,arg...)\
        if(log_level_g > 0){\
        printk(KERN_WARNING YELLOW #module NORMAL ": " fmt,##arg);\
        }

#define CPUFREQ_INFO(module,fmt,arg...)\
        if(log_level_g > 1){\
        printk(KERN_INFO GREEN #module NORMAL ": " fmt,##arg);\
        }

#define CPUFREQ_DEBUG(module,fmt,arg...)\
        if(log_level_g > 2){\
        printk(KERN_DEBUG CYAN #module NORMAL ": " fmt,##arg);\
        }
/******************************************************************************/

extern u32 cgu_set_clock(u32 cpu_clk, u32 ddr_clk, u32 fpi_clk);
#ifdef CONFIG_VR9
extern unsigned int ifx_get_cpu_hz(void);
#endif

typedef struct
{
	int cpu_freq;     /*cpu freqency in MHz*/
    int ddr_freq;     /*ddr freqency in MHz*/
    int core_voltage; /*core voltage in mV*/
}IFX_CPUFREQ_STAT_t;

typedef struct
{
   IFX_PMCU_STATE_t oldState;  /* oldState */
   IFX_PMCU_STATE_t newState;  /* newState */
   IFX_int32_t        permit;  /* permission for this transition */
   IFX_PMCU_STATE_t intState1; /* intermediate State1 */
   IFX_PMCU_STATE_t intState2; /* intermediate State2 */
   IFX_int32_t      stateDir;  /* state change direction up,down */
}IFX_FREQ_TRANSITION_t;


/*=============================================================================*/
/* MODULE DEFINES                                                              */
/*=============================================================================*/
#define IFX_NO_TRANS       0x000     /* frequency transition prohibited */
#define IFX_TRANS_PERMIT   0x001     /* frequency transition permitted  */
#define IFX_TRANS_CRITICAL 0x002     /* frequency transition critical, need intermediate step */
#define IFX_STATE_NC       0x100     /* no state change */
#define IFX_STATE_UP       0x200     /* state change up, means step from lower to higher frequency */
#define IFX_STATE_DOWN     0x300     /* state change down, means step from higher to lower frequency */

#ifdef CPUFREQ_DEBUG_MODE
#define IFX_TRACE_COUNT   50     /* test purpose only */
#endif


/*=============================================================================*/
/* MODULE GLOBAL VARIABLES                                                     */
/*=============================================================================*/
#ifdef CONFIG_IFX_DCDC
/** \ingroup LQ_CPUFREQ
    \brief  switching On/Off the voltage scaling support 
            -  0  = disable voltage scaling
            -  1  = enable voltage scaling (default)
*/
static int dcdc_ctrl_g = 1;
#endif

/** \ingroup LQ_CPUFREQ
    \brief  Control the  garrulity of the CPUFREQ module 
            - <0 = quiet
            -  0  = ERRORS, WARNINGS (default)
            -  1  = + INFOs
            -  2  = ALL
*/
static int log_level_g = 1;

/** \ingroup LQ_CPUFREQ
    \brief  cpu frequency in MHz 
*/
static int cpu_freq_g = 0;

/** \ingroup LQ_CPUFREQ
    \brief  ddr frequency in MHz 
*/
static int ddr_freq_g = 0;

/** \ingroup LQ_CPUFREQ
    \brief  current chip_id
			- ARX188       0x016C
			- ARX168       0x016D
			- ARX182       0x016E  before November 2009
			- ARX182_2   0x016F  after  November 2009 
			- GRX188       0x0170
			- GRX168       0x0171
			- VRX288       0x01C0
			- VRX282       0x01C1
			- VRX268       0x01C2
			- GRX288       0x01C9
    
*/
static int chip_id_g;

/** \ingroup LQ_CPUFREQ
    \brief   hold always the last power state of cpu driver
*/
static IFX_PMCU_STATE_t lastPmcuState_g = IFX_PMCU_STATE_D0;

/** \ingroup LQ_CPUFREQ
    \brief   freqtab_p points to the right frequency table depending
    		 on the underlying hardware. The pointer is initialized
             during driver init.
*/
static IFX_CPUFREQ_STAT_t* freqtab_p = NULL;

/** \ingroup LQ_CPUFREQ
    \brief   freqTransp points to the right frequency transition table depending on the underlying hardware. The pointer is initialized
             during driver init.
*/
static IFX_FREQ_TRANSITION_t* freqTransp = NULL;

#ifdef CONFIG_PROC_FS
/* proc entry variables */
static struct proc_dir_entry* ifx_cpufreq_dir_cpufreq_g = NULL;
#endif


/** \ingroup LQ_CPUFREQ
    \brief  Frequency table for ARX168/ARX182 
            -  1st Col   = CPU frequency in [MHz]
            -  2cd Col  = DDR frequency in [MHz]
            -  3rd Col  = core power domain voltage in [mV]
*/
IFX_CPUFREQ_STAT_t cpufreq_state_1[]={
	{333,167,0},/* D0 */
	{  0,  0,0},/* D1, not defined*/
	{167,167,0},/* D2 */
	{111,111,0} /* D3 */
};

/** \ingroup LQ_CPUFREQ
    \brief  Frequency table for ARX188/GRX188 
            -  1st Col   = CPU frequency in [MHz]
            -  2cd Col  = DDR frequency in [MHz]
            -  3rd Col  = core power domain voltage in [mV]
*/
IFX_CPUFREQ_STAT_t cpufreq_state_2[]={
	{393,196,0},/* D0 */
	{  0,  0,0},/* D1, not defined*/
	{196,196,0},/* D2 */
	{131,131,0} /* D3 */
};

/** \ingroup LQ_CPUFREQ
    \brief  Frequency table for VRX268 
            -  1st Col   = CPU frequency in [MHz]
            -  2cd Col  = DDR frequency in [MHz]
            -  3rd Col  = core power domain voltage in [mV]
*/
IFX_CPUFREQ_STAT_t cpufreq_state_3[]={
	{333,167,1000},/* D0 */
	{  0,  0,   0},/* D1, not defined*/
	{167,167,1000},/* D2 */
	{125,125, 930} /* D3 */
};

/** \ingroup LQ_CPUFREQ
    \brief  Frequency table for VRX288/GRX288 
            -  1st Col   = CPU frequency in [MHz]
            -  2cd Col  = DDR frequency in [MHz]
            -  3rd Col  = core power domain voltage in [mV]
*/
IFX_CPUFREQ_STAT_t cpufreq_state_4[]={
#ifdef CONFIG_UBOOT_CONFIG_VR9_DDR1
	{500,200,1175},/* D0 */
#else 
	{500,250,1175},/* D0 */
#endif      
	{393,196,1050},/* D1,*/
	{333,167,1000},/* D2 */
	{125,125, 930} /* D3 */
};

/** \ingroup LQ_CPUFREQ
    \brief  Dependency list of the cpu frequency driver
          State D0D3 means don't care.
	Static declaration is necessary to let gcc accept this static initialisation.
  */
/* define dependency list;  state D0D3 means don't care */
/* static declaration is necessary to let gcc accept this static initialisation. */
static IFX_PMCU_MODULE_DEP_t depList=
{
	5,
	{
		{IFX_PMCU_MODULE_VE,   IFX_PMCU_STATE_D0D3,IFX_PMCU_STATE_D2,IFX_PMCU_STATE_D2,IFX_PMCU_STATE_D2},
		{IFX_PMCU_MODULE_PPE,  IFX_PMCU_STATE_D0D3,IFX_PMCU_STATE_D3,IFX_PMCU_STATE_D3,IFX_PMCU_STATE_D3},
		{IFX_PMCU_MODULE_GPTC, IFX_PMCU_STATE_D0,  IFX_PMCU_STATE_D1,IFX_PMCU_STATE_D2,IFX_PMCU_STATE_D3},
		{IFX_PMCU_MODULE_SPI,  IFX_PMCU_STATE_D0,  IFX_PMCU_STATE_D1,IFX_PMCU_STATE_D2,IFX_PMCU_STATE_D3},
		{IFX_PMCU_MODULE_UART, IFX_PMCU_STATE_D0,  IFX_PMCU_STATE_D1,IFX_PMCU_STATE_D2,IFX_PMCU_STATE_D3}
	}
};


#ifdef CONFIG_VR9
/** \ingroup LQ_CPUFREQ
    \brief  Frequency transition table for VRX268
          Intermediate states are only relevant if permit = IFX_TRANS_CRITICAL
	Static declaration is necessary to let gcc accept this static initialisation.
            -  1st Col   = old power state
            -  2cd Col  = new power state
            -  3rd Col  = permission
            -  4th Col  = intermediate state 1
            -  5th Col  = intermediate state 2
            -  6th Col  = up or down step
  */
static IFX_FREQ_TRANSITION_t freq_transition_3[]={
/* {      oldState     ,     newState     ,     permit       , intermediate state1 , intermediate state2 }  */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0, IFX_NO_TRANS     , IFX_PMCU_STATE_D0   , IFX_PMCU_STATE_D0 , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1 , IFX_STATE_DOWN},  /* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D2, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2 , IFX_STATE_DOWN},  /* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1 , IFX_STATE_DOWN},  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D0, IFX_NO_TRANS     , IFX_PMCU_STATE_D0   , IFX_PMCU_STATE_D0 , IFX_STATE_UP  },  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1 , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D2, IFX_NO_TRANS     , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2 , IFX_STATE_DOWN},  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D3, IFX_NO_TRANS     , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3 , IFX_STATE_DOWN},  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1 , IFX_STATE_UP  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1 , IFX_STATE_UP  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D2, IFX_NO_TRANS     , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2 , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3 , IFX_STATE_DOWN},  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D1 , IFX_STATE_UP  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2 , IFX_STATE_UP  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D2, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2 , IFX_STATE_UP  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_NO_TRANS     , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3 , IFX_STATE_NC  }   /* */
};

/** \ingroup LQ_CPUFREQ
    \brief  Frequency transition table for XRX288
          Intermediate states are only relevant if permit = IFX_TRANS_CRITICAL
	Static declaration is necessary to let gcc accept this static initialisation.
            -  1st Col   = old power state
            -  2cd Col  = new power state
            -  3rd Col  = permission
            -  4th Col  = intermediate state 1
            -  5th Col  = intermediate state 2
            -  6th Col  = up or down step
  */
static IFX_FREQ_TRANSITION_t freq_transition_4[]={
/* {      oldState     ,     newState     ,       permit      , intermediate state1, intermediate stat2 }    */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0, IFX_NO_TRANS      , IFX_PMCU_STATE_D0  , IFX_PMCU_STATE_D0 , IFX_STATE_NC  },/* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D1, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D1  , IFX_PMCU_STATE_D1 , IFX_STATE_DOWN},/* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D2, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D2  , IFX_PMCU_STATE_D2 , IFX_STATE_DOWN},/* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D1  , IFX_PMCU_STATE_D1 , IFX_STATE_DOWN},/* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D0  , IFX_PMCU_STATE_D0 , IFX_STATE_UP  },/* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D1, IFX_NO_TRANS      , IFX_PMCU_STATE_D1  , IFX_PMCU_STATE_D1 , IFX_STATE_NC  },/* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D2, IFX_TRANS_CRITICAL, IFX_PMCU_STATE_D0  , IFX_PMCU_STATE_D0 , IFX_STATE_DOWN},/* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D3  , IFX_PMCU_STATE_D3 , IFX_STATE_DOWN},/* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D1  , IFX_PMCU_STATE_D1 , IFX_STATE_UP  },/* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D1, IFX_TRANS_CRITICAL, IFX_PMCU_STATE_D0  , IFX_PMCU_STATE_D0 , IFX_STATE_UP  },/* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D2, IFX_NO_TRANS      , IFX_PMCU_STATE_D2  , IFX_PMCU_STATE_D2 , IFX_STATE_NC  },/* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D3  , IFX_PMCU_STATE_D3 , IFX_STATE_DOWN},/* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT  , IFX_PMCU_STATE_D2  , IFX_PMCU_STATE_D1 , IFX_STATE_UP  },/* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D1, IFX_TRANS_CRITICAL, IFX_PMCU_STATE_D0  , IFX_PMCU_STATE_D0 , IFX_STATE_UP  },/* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D2, IFX_TRANS_CRITICAL, IFX_PMCU_STATE_D0  , IFX_PMCU_STATE_D0 , IFX_STATE_UP  },/* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_NO_TRANS      , IFX_PMCU_STATE_D3  , IFX_PMCU_STATE_D3 , IFX_STATE_NC  } /* */
};
#endif

#ifdef CONFIG_AR9
/** \ingroup LQ_CPUFREQ
    \brief  Frequency transition table for XRX1XX
           Intermediate states are only relevant if permit = IFX_TRANS_CRITICAL
	Static declaration is necessary to let gcc accept this static initialisation.
            -  1st Col   = old power state
            -  2cd Col  = new power state
            -  3rd Col  = permission
            -  4th Col  = intermediate state 1
            -  5th Col  = intermediate state 2
            -  6th Col  = up or down step
  */
static IFX_FREQ_TRANSITION_t freq_transition_2[]={
/* {      oldState     ,     newState     ,     permit       , intermediate state1 , intermediate state2, DCDC control  }  */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0, IFX_NO_TRANS     , IFX_PMCU_STATE_D0   , IFX_PMCU_STATE_D0  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D2, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D0, IFX_NO_TRANS     , IFX_PMCU_STATE_D0   , IFX_PMCU_STATE_D0  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D2, IFX_NO_TRANS     , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D1, IFX_PMCU_STATE_D3, IFX_NO_TRANS     , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D0   , IFX_PMCU_STATE_D0  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D2, IFX_NO_TRANS     , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D2, IFX_PMCU_STATE_D3, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D0, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D0   , IFX_PMCU_STATE_D0  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D1, IFX_NO_TRANS     , IFX_PMCU_STATE_D1   , IFX_PMCU_STATE_D1  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D2, IFX_TRANS_PERMIT , IFX_PMCU_STATE_D2   , IFX_PMCU_STATE_D2  , IFX_STATE_NC  },  /* */
    { IFX_PMCU_STATE_D3, IFX_PMCU_STATE_D3, IFX_NO_TRANS     , IFX_PMCU_STATE_D3   , IFX_PMCU_STATE_D3  , IFX_STATE_NC  }   /* */
};
#endif

#ifdef CPUFREQ_DEBUG_MODE
    IFX_FREQ_TRANSITION_t state_trace[IFX_TRACE_COUNT]; /* test purpose only */
    IFX_int32_t trace_count = 0;                        /* test purpose only */
#endif


/*=============================================================================*/
/* EXTERNAL FUNCTIONS PROTOTYPES                                                  */
/*=============================================================================*/
#ifdef CONFIG_IFX_DCDC
    extern int ifx_dcdc_power_voltage_set ( int voltage_value);
#endif

/*=============================================================================*/
/* LOCAL FUNCTIONS PROTOTYPES                                                  */
/*=============================================================================*/
#ifdef CONFIG_PROC_FS
static IFX_int_t ifx_cpufreq_proc_version(IFX_char_t *buf, IFX_char_t **start, off_t offset, IFX_int_t count, IFX_int_t *eof, IFX_void_t *data);
static IFX_int_t ifx_cpufreq_proc_chipid(IFX_char_t *buf, IFX_char_t **start, off_t offset, IFX_int_t count, IFX_int_t *eof, IFX_void_t *data);
static int ifx_cpufreq_proc_read_log_level(char *, char **, off_t, int, int *, void *);
static int ifx_cpufreq_proc_write_log_level(struct file *, const char *, unsigned long, void *);
#endif
static void ifx_cpufreq_set_freqtab(void);
static IFX_int32_t ifx_get_transition_permit(IFX_PMCU_STATE_t oldState, IFX_PMCU_STATE_t newState, 
                                             IFX_PMCU_STATE_t* intState1, IFX_PMCU_STATE_t* intState2);

/**
   Callback function registered at the PMCU.
   This function is called by the PMCU whenever a frequency change is requested, to inform all affected modules
   before the frequency change really happen.

   \param[in]   pmcuModule  module identifier
   \param[in]  newState new requested power state 
   \param[in]  oldState old power state 

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error
*/
static IFX_PMCU_RETURN_t 
ifx_cpufreq_prechange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
	CPUFREQ_DEBUG(CPUFREQ, "ifx_cpufreq_prechange is called\n");
	/* check if the requested new powerstate is supported by HW */
	if( (freqtab_p + (newState-1))->cpu_freq == 0 ){
		CPUFREQ_DEBUG(CPUFREQ, "Requested Powerstate is NOT supported\n");
		return IFX_PMCU_RETURN_DENIED;
	}
	CPUFREQ_DEBUG(CPUFREQ, "Requested Powerstate is supported\n");
	return IFX_PMCU_RETURN_SUCCESS;
}

/**
   Callback function registered at the PMCU.
   This function is called by the PMCU whenever a frequency change was requested, to inform all affected modules
   after a frequency change was really executed or in case the request was denied.

   \param[in]   pmcuModule  module identifier
   \param[in]  newState new requested power state 
   \param[in]  oldState old power state 

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error
*/
static IFX_PMCU_RETURN_t 
ifx_cpufreq_postchange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
{
	CPUFREQ_DEBUG(CPUFREQ, "ifx_cpufreq_postchange is called\n");
	return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Callback function registered at the PMCU.
   This function is called by the PMCU to get the current power state status of the cpu frequency driver.

   \param[out]   pmcuState  current power state

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error
*/
static IFX_PMCU_RETURN_t 
ifx_cpufreq_state_get(IFX_PMCU_STATE_t *pmcuState)
{
#ifdef CONFIG_AR9
	cpu_freq_g=(int)cgu_get_cpu_clock()/1000000;
	ddr_freq_g=(int)cgu_get_io_region_clock()/1000000;
#endif
#ifdef CONFIG_VR9
	cpu_freq_g=ifx_get_cpu_hz()/1000000;
	ddr_freq_g=(int)ifx_get_ddr_hz()/1000000;
#endif

	*pmcuState = lastPmcuState_g;
	return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Callback function registered at the PMCU.
   This function is called by the PMCU to set a new power state for the cpu frequency driver.

   \param[in]   pmcuState  new power state

   \return Returns value as follows:
     - IFX_PMCU_RETURN_SUCCESS: if successful
     - IFX_PMCU_RETURN_ERROR: in case of an error
*/
IFX_PMCU_RETURN_t  ifx_cpufreq_state_set(IFX_PMCU_STATE_t pmcuState)
{
    IFX_PMCU_STATE_t intState1 = 0;
    IFX_PMCU_STATE_t intState2 = 0;
    IFX_int32_t retVal;

#ifdef CPUFREQ_DEBUG_MODE
    if(trace_count >= IFX_TRACE_COUNT) {
        trace_count = 0;
    }
#endif
	/* initialize frequency table pointer if not already done by init function */
	if (freqtab_p == NULL) {
		ifx_cpufreq_set_freqtab();
	}

	/* HINT: IFX_PMCU_STATE_D0 = 1 not 0 !!! */
	if( (pmcuState < IFX_PMCU_STATE_D0) || (pmcuState > IFX_PMCU_STATE_D3) ){
		return IFX_ERROR; /* requested powerState out of range */
	}

	/* check if state refers to a valid frequency */
	if( (freqtab_p + (pmcuState-1))->cpu_freq == 0 ){
		return IFX_ERROR;
	}

	CPUFREQ_DEBUG(CPUFREQ, "setting cpu_freq_g=%d,ddr_freq_g=%d,core_voltage=%d\n",\
		   (freqtab_p + (pmcuState-1))->cpu_freq,\
           (freqtab_p + (pmcuState-1))->ddr_freq,\
           (freqtab_p + (pmcuState-1))->core_voltage);

   /* Check if we have a critical frequency transition */
   retVal = ifx_get_transition_permit(lastPmcuState_g, pmcuState, &intState1, &intState2);
   #ifdef CPUFREQ_DEBUG_MODE
       state_trace[trace_count].newState = pmcuState;
       state_trace[trace_count].oldState = lastPmcuState_g;
       state_trace[trace_count].intState1 = intState1;
       state_trace[trace_count].intState2 = intState2;
       state_trace[trace_count].permit = retVal;
       trace_count++;
   #endif
   if((retVal & 0xFF) == IFX_NO_TRANS) {
       return IFX_PMCU_RETURN_SUCCESS; /* nothing to do */
   }


   if((retVal & 0xFF) == IFX_TRANS_CRITICAL) {
       #ifdef CONFIG_IFX_DCDC
          if(dcdc_ctrl_g == 1) {
              CPUFREQ_DEBUG(CPUFREQ, "Critical transition always via D0 state\n");
              ifx_dcdc_power_voltage_set((freqtab_p + (IFX_PMCU_STATE_D0-1))->core_voltage);
              retVal = retVal & 0xFF; /*clear up/down direction*/
              retVal = retVal | IFX_STATE_DOWN;/*from D0 we go always down.*/
          }
       #endif
       cgu_set_clock((freqtab_p + (intState1-1))->cpu_freq,
                     (freqtab_p + (intState1-1))->ddr_freq,
                     (freqtab_p + (intState1-1))->ddr_freq);
       /*Currently not in use. Only one intermediate step necessary. 
         If necessary take care on DCDC voltage scaling.*/
       if(intState1 != intState2) {
           cgu_set_clock((freqtab_p + (intState2-1))->cpu_freq,
                         (freqtab_p + (intState2-1))->ddr_freq,
                         (freqtab_p + (intState2-1))->ddr_freq);
       }
   } else {
       #ifdef CONFIG_IFX_DCDC
          if(((retVal & 0xF00) == IFX_STATE_UP) && (dcdc_ctrl_g == 1)) {
              CPUFREQ_DEBUG(CPUFREQ, "UP step, dcdc voltage scaling is called\n");
              ifx_dcdc_power_voltage_set((freqtab_p + (pmcuState-1))->core_voltage);
          }
       #endif
   }

   cgu_set_clock((freqtab_p + (pmcuState-1))->cpu_freq,
                 (freqtab_p + (pmcuState-1))->ddr_freq,
                 (freqtab_p + (pmcuState-1))->ddr_freq);

#ifdef CONFIG_IFX_DCDC
   if(((retVal & 0xF00) == IFX_STATE_DOWN) && (dcdc_ctrl_g == 1)) {
       CPUFREQ_DEBUG(CPUFREQ, "DOWN step, dcdc voltage scaling is called\n");
       (void)ifx_dcdc_power_voltage_set((freqtab_p + (pmcuState-1))->core_voltage);
   }
#endif

   /* remember the last state */
	lastPmcuState_g = pmcuState;
	return IFX_PMCU_RETURN_SUCCESS;
}


/**
   Internal function of the CPUFREQ driver to reference, depending on the underlying hardware, 
   the correct frequency table. The function initialize the two pointer freqtab_p and freqTransp,
   which are pointing to the right frequency table and frequency transition table.
 */
static void ifx_cpufreq_set_freqtab(void){
   /* read the current chipid from the HW register. */
   chip_id_g = (IFX_REG_R32(IFX_MPS_CHIPID) & 0x0FFFF000) >> 12;
   switch (chip_id_g) {
      case GRX168:
      case ARX168:
      case ARX182:
      case ARX182_2:
         freqtab_p = cpufreq_state_1;
         #ifdef CONFIG_AR9
         freqTransp = freq_transition_2;
         #endif
         break;
      case ARX188:
      case GRX188:
         freqtab_p = cpufreq_state_2;
         #ifdef CONFIG_AR9
         freqTransp = freq_transition_2;
         #endif
         break;
      case VRX268:
         freqtab_p = cpufreq_state_3;
         #ifdef CONFIG_VR9
         freqTransp = freq_transition_3;
         #endif
         break;
      case VRX288:
      case GRX288:
         freqtab_p = cpufreq_state_4;
         #ifdef CONFIG_VR9
         freqTransp = freq_transition_4;
         #endif
         break;
      default:
         freqtab_p = cpufreq_state_1;
         #ifdef CONFIG_VR9
         freqTransp = freq_transition_4;
         #endif
         #ifdef CONFIG_AR9
         freqTransp = freq_transition_2;
         #endif
         break;
   }
}

/**
   Internal function of the CPUFREQ driver to get the permission of the currently requested power state change.
   Check if it is a CRITICAL transition or not.
   
     \param[in]   oldState  old power state
     \param[in]   newState  new power state
     \param[out]   intState1  intermediate_1 power state
     \paramout]   intState2  intermediate_2 power state

 */static IFX_int_t ifx_get_transition_permit(IFX_PMCU_STATE_t oldState, IFX_PMCU_STATE_t newState, 
                                           IFX_PMCU_STATE_t* intState1, IFX_PMCU_STATE_t* intState2)
{
    IFX_int_t i;
    IFX_FREQ_TRANSITION_t freqTrans;

    for(i=0;i<16;i++) {
        freqTrans = *(freqTransp + i);
        if(freqTrans.oldState == oldState) {
            if(freqTrans.newState == newState) {
                *intState1 = freqTrans.intState1;
                *intState2 = freqTrans.intState2;
                return (freqTrans.permit | freqTrans.stateDir);
            }
        }
    }
    return (IFX_NO_TRANS | IFX_STATE_NC);
}

#ifdef CONFIG_PROC_FS
static int ifx_cpufreq_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(buf+len, "IFX_CPUFREQ Version = %s\n",IFX_CPUFREQ_DRV_VERSION);
	len += sprintf(buf+len, "Compiled on %s, %s for Linux kernel %s\n",__DATE__, __TIME__, UTS_RELEASE);
	return len;
}

static int ifx_cpufreq_proc_chipid(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
   int len = 0;
   int chipId;

   chipId = IFX_REG_R32(IFX_MPS_CHIPID);
   len += sprintf(buf+len, "CHIP_ID_REG = %08x\n",chipId);
   chipId = (chipId & 0x0FFFF000) >> 12;
   switch (chipId) {
      case ARX188:
         len += sprintf(buf+len, "CHIP_ID = ARX188\n");
         break;
      case ARX168:
         len += sprintf(buf+len, "CHIP_ID = ARX168\n");
         break;
      case ARX182:
      case ARX182_2:
         len += sprintf(buf+len, "CHIP_ID = ARX182\n");
         break;
      case GRX188:
         len += sprintf(buf+len, "CHIP_ID = GRX188\n");
         break;
      case GRX168:
         len += sprintf(buf+len, "CHIP_ID = GRX168\n");
         break;
      case VRX288:
         len += sprintf(buf+len, "CHIP_ID = VRX288\n");
         break;
      case VRX268:
         len += sprintf(buf+len, "CHIP_ID = VRX268\n");
         break;
      case VRX282:
         len += sprintf(buf+len, "CHIP_ID = VRX282\n");
         break;
      case GRX288:
         len += sprintf(buf+len, "CHIP_ID = GRX288\n");
         break;
      default:
         len += sprintf(buf+len, "CHIP_ID = undefined\n");
         break;
   }
   return len;
}

static int ifx_cpufreq_proc_read_log_level(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

	len += sprintf(buf + off + len, "log_level = %d\n", log_level_g);
	len += sprintf(buf + off + len, "Control the  garrulity of the CPUFREQ module\n");
	len += sprintf(buf + off + len, "     0 = quiet\n");
	len += sprintf(buf + off + len, "     1 = EMERG, ERRORS, WARNINGS (default)\n");
	len += sprintf(buf + off + len, "     2 = + INFOs\n");
	len += sprintf(buf + off + len, "     3 = + DEBUG\n");
    *eof = 1;
    return len;
}

static int ifx_cpufreq_proc_write_log_level(struct file *file, const char *buf, unsigned long count, void *data)
{
	char str[20];
	char str1[2];
	int len, rlen;

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	strncpy(str1,str,1);
	str1[1]='\0';
	log_level_g = (int)simple_strtol(str1, NULL, 0);

	return count;
}


#ifdef CONFIG_IFX_DCDC
static int ifx_cpufreq_proc_read_dcdc_ctrl(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

	len += sprintf(buf + off + len, "dcdc_ctrl = %d\n", dcdc_ctrl_g);
	len += sprintf(buf + off + len, "switching On/Off the voltage scaling support\n");
	len += sprintf(buf + off + len, "     0 = disable\n");
	len += sprintf(buf + off + len, "     1 = enable\n");
    *eof = 1;
    return len;
}

static int ifx_cpufreq_proc_write_dcdc_ctrl(struct file *file, const char *buf, unsigned long count, void *data)
{
	char str[20];
	char str1[2];
	int len, rlen;

	len = count < sizeof(str) ? count : sizeof(str) - 1;
	rlen = len - copy_from_user(str, buf, len);
	strncpy(str1,str,1);
	str1[1]='\0';
	dcdc_ctrl_g = (int)simple_strtol(str1, NULL, 0);

	return count;
}
#endif

static int ifx_cpufreq_proc_EntriesInstall(void)
{
	/*create pmcu proc entry*/
	struct proc_dir_entry *res;

	ifx_cpufreq_dir_cpufreq_g = proc_mkdir("driver/ifx_cpufreq", NULL);
	create_proc_read_entry("version", 0, ifx_cpufreq_dir_cpufreq_g, ifx_cpufreq_proc_version, NULL);
	create_proc_read_entry("chipid", 0, ifx_cpufreq_dir_cpufreq_g, ifx_cpufreq_proc_chipid, NULL);
	res = create_proc_entry("log_level", 0,	ifx_cpufreq_dir_cpufreq_g);
	if ( res ){
		res->read_proc  = ifx_cpufreq_proc_read_log_level;
		res->write_proc = ifx_cpufreq_proc_write_log_level;
	}

#ifdef CONFIG_IFX_DCDC
    res = create_proc_entry("dcdc_ctrl", 0,	ifx_cpufreq_dir_cpufreq_g);
    if ( res ){
        res->read_proc  = ifx_cpufreq_proc_read_dcdc_ctrl;
        res->write_proc = ifx_cpufreq_proc_write_dcdc_ctrl;
    }
#endif

    return 0;
}

/**
	Initialize and install the proc entry

	\return
	-1 or 0 on success
*/
static int ifx_cpufreq_proc_EntriesRemove(void)
{

#ifdef CONFIG_IFX_DCDC
    remove_proc_entry("dcdc_ctrl", ifx_cpufreq_dir_cpufreq_g);
#endif
    remove_proc_entry("log_level", ifx_cpufreq_dir_cpufreq_g);
	remove_proc_entry("chipid", ifx_cpufreq_dir_cpufreq_g);
	remove_proc_entry("version", ifx_cpufreq_dir_cpufreq_g);
	remove_proc_entry(ifx_cpufreq_dir_cpufreq_g->name, NULL);
	ifx_cpufreq_dir_cpufreq_g = NULL;
	return 0;
}
#endif

static int __init ifx_cpufreq_init(void)
{
	IFX_PMCU_REGISTER_t pmcuRegister;
	
    #ifdef CPUFREQ_DEBUG_MODE
    /* init powerstate transition trace */
    memset (&state_trace, 0, sizeof(state_trace));
    #endif

    /* register CPU to PMCU */
	memset (&pmcuRegister, 0, sizeof(pmcuRegister));
	pmcuRegister.pmcuModule=IFX_PMCU_MODULE_CPU;
	pmcuRegister.pmcuModuleNr=0;
	pmcuRegister.pmcuModuleDep = &depList;
	pmcuRegister.pre = ifx_cpufreq_prechange;
	pmcuRegister.post = ifx_cpufreq_postchange;
	/* Functions used to change CPU state */
	/* and to get actual CPU state */
	pmcuRegister.ifx_pmcu_state_change = ifx_cpufreq_state_set;
	pmcuRegister.ifx_pmcu_state_get = ifx_cpufreq_state_get;
	ifx_pmcu_register ( &pmcuRegister );

	ifx_cpufreq_state_get(&lastPmcuState_g);

	/* set to right frequency table according to the underlaying hardware */
	ifx_cpufreq_set_freqtab();

#ifdef CONFIG_PROC_FS
	ifx_cpufreq_proc_EntriesInstall();
#endif

	return 0;
}

static void __exit ifx_cpufreq_exit (void)
{
	IFX_PMCU_REGISTER_t pmcuRegister;
	/*unregister CPU from PMCU */
	memset (&pmcuRegister, 0, sizeof(pmcuRegister));
	pmcuRegister.pmcuModule=IFX_PMCU_MODULE_CPU;
	pmcuRegister.pmcuModuleNr=0;
	pmcuRegister.pmcuModuleDep = &depList;
	pmcuRegister.pre = ifx_cpufreq_prechange;
	pmcuRegister.post = ifx_cpufreq_postchange;
	/* Functions used to change CPU state */
	/* and to get actual CPU state */
	pmcuRegister.ifx_pmcu_state_change = ifx_cpufreq_state_set;
	pmcuRegister.ifx_pmcu_state_get = ifx_cpufreq_state_get;
	ifx_pmcu_unregister ( &pmcuRegister );

#ifdef CONFIG_PROC_FS
	ifx_cpufreq_proc_EntriesRemove();
#endif
	return;
}

module_init(ifx_cpufreq_init);
module_exit(ifx_cpufreq_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("Lantiq Deutschland GmbH");
MODULE_DESCRIPTION ("LANTIQ CPUFREQ driver");
MODULE_SUPPORTED_DEVICE ("Amazon SE, Danube, XRX100, XRX200");

/* @} */ /* LQ_CPUFREQ */