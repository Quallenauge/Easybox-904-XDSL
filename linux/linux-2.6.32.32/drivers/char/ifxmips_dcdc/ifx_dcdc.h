/******************************************************************************
**
** FILE NAME    : ifx_dcdc.h
** PROJECT      : IFX UEIP
** MODULES      : IFX DCDC converter driver
** DATE         : 07 Dec 2010
** AUTHOR       : Sameer Ahmad
** DESCRIPTION  : IFX Cross platform DCDC converter driver
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date                $Author                 $Comment
** 07 Dec 2010           Sameer Ahmad
*******************************************************************************/
#ifndef _IFX_DCDC_H_
#define _IFX_DCDC_H_
#include "linux/spinlock.h"

//#define IFX_DEBUG
#ifdef IFX_DEBUG
#define IFX_DCDC_PRINT(format, arg...) \
        printk(format, ##arg)
#else
#define IFX_DCDC_PRINT(format, arg...) \
        do {} while(0)
#endif

/*DCDC drive ioctl control*/

/*DCDC driver IOCTL magic number*/
#define   IFX_DCDC_IOCTL_MAGIC       0xd0
/*IOCTL command to read the DCDC driver version*/
#define   IFX_DCDC_VERSION                        _IOR(IFX_DCDC_IOCTL_MAGIC, 0, char*)
/*IOCTL comand to read the power state*/
#define   IFX_DCDC_PWSTATE_QUERY                  _IOR(IFX_DCDC_IOCTL_MAGIC, 1, int)
/*IOCTL command to query the voltage*/
#define   IFX_DCDC_VOLTAGE_QUERY                  _IOR(IFX_DCDC_IOCTL_MAGIC, 2, int)
/*IOCTL command to query the voltage level*/
#define   IFX_DCDC_VOLTAGE_LEVEL_QUERY            _IOR(IFX_DCDC_IOCTL_MAGIC, 3, int)

/* Name of the DCDC device*/
#define IFX_DCDC_DEV_NAME                "ifx_dcdc"
#define IFX_DCDC_DEV_NAMSIZ              16
#define IFX_DCDC_DRV_VERSION             "1.0.3"
#define IFX_SUCCESS                      0
#define IFX_ERROR                        -1
/* DCDC Converter Core Voltage Level: Table to Map the Power state to voltage
* is maintained by the CPU driver and we define the core voltage level params
* as integers and following is the mapping
* 930                ---->   .93  V
* 1000               ---->  1.0   V
* 1175               ---->  1.175 V
*/
#define   IFX_DCDC_CORE_VOLTAGE_930               930
#define   IFX_DCDC_CORE_VOLTAGE_1000              1000
#define   IFX_DCDC_CORE_VOLTAGE_1050              1050
#define   IFX_DCDC_CORE_VOLTAGE_1175              1175
#define   IFX_DCDC_MAX_VOLTAGE_LEVELS             3
#define   IFX_DIGREF_DEFAULT_VALUE                1000
#define   IFX_DIGREF_DEFAULT_BITMASK              0x7f

/*Define Bit mask corrosponding to the core voltage*/
#define   IFX_DCDC_CORE_VOLTAGE_930_BITMASK       0x0
#define   IFX_DCDC_CORE_VOLTAGE_1000_BITMASK      0x1
#define   IFX_DCDC_CORE_VOLTAGE_1050_BITMASK      0x2
#define   IFX_DCDC_CORE_VOLTAGE_1175_BITMASK      0x3
/*DCDC ABSOLUTE REGISTER BASE ADDRESS*/
#define   IFX_DCDC_MODULE_BASE_ADDRESS           0x1F106A00
#define   IFX_DCDC_ABSOLUTE_REGISTER_ADDR        KSEG1ADDR(IFX_DCDC_MODULE_BASE_ADDRESS)
/*
* DCDC Device struct
*/
typedef  struct _dcdc_dev {
    char           name[16];
    int            major_num;
    int            minor_num;
    unsigned int   Current_voltage;
    unsigned int   Power_state;
    spinlock_t     ifx_dcdc_lock;
}DcDcDevt;

/*
* DCDC Converter Registers definition
*/

/*Clock setup register 0*/
typedef union _Clock_setup0_t {
    unsigned char byte;
    struct _Clock_setup0_reg {
        unsigned char    mdll_byp      : 1;
        unsigned char    clk_sel_p     : 1;
        unsigned char    mdll_m        : 6;
    }Clock_setup0_reg;
} __attribute__((packed)) Clock_setup0_t;

/*Clock setup register 1*/
typedef union _Clock_setup1_t {
    unsigned char byte;
    struct _Clock_setup1_reg {
        unsigned char    res0          : 2;
        unsigned char    sel_div25     : 1;
        unsigned char    clk_sel_adc   : 1;
        unsigned char    mdll_ix2      : 1;
        unsigned char    mdll_res      : 2;
        unsigned char    res1          : 1;
    }Clock_setup1_reg;
} __attribute__((packed)) Clock_setup1_t;

/*BIAS setting register*/
typedef union _Bias_Vreg0_t {
    unsigned char byte;
    struct _Bias_reg {
        unsigned char    res0           : 1;
        unsigned char    ii_loadinc     : 1;
        unsigned char    ii_red         : 1;
        unsigned char    vreg_sel       : 2;
        unsigned char    vref_sel       : 3;
    }Bias_reg;
} __attribute__((packed)) Bias_Vreg0_t;

/*ADC setting register*/
typedef union _Adc_Gen0_t {
    unsigned char byte;
    struct _Adc_reg0 {
        unsigned char    iset_lsb_dr           : 1;
        unsigned char    del_inc_p             : 1;
        unsigned char    del_inc_n             : 1;
        unsigned char    out_inv               : 1;
        unsigned char    os_en8                : 1;
        unsigned char    os_en                 : 1;
        unsigned char    pfm_mode              : 1;
        unsigned char    forc_sta_dc           : 1;
    }Adc_reg0;
}  __attribute__((packed)) Adc_Gen0_t;

/*ADC setting register*/
typedef union _Adc_Gen1_t {
    unsigned char byte;
    struct _Adc_reg1 {
        unsigned char    set_ROM_sel           : 2;
        unsigned char    set_coma              : 3;
        unsigned char    set_compr             : 3;
    }Adc_reg1;
}  __attribute__((packed)) Adc_Gen1_t;

/*ADC Configure setting register*/
typedef union _Adc_CON0_t {
    unsigned char byte;
    struct _Adc_Con0 {
        unsigned char    res0                  : 1;
        unsigned char    set_scr               : 2;
        unsigned char    set_compc             : 3;
        unsigned char    set_off_cal           : 1;
        unsigned char    set_rsta              : 1;
    }Adc_Con0;
}  __attribute__((packed)) Adc_CON0_t;

/*ADC Configure setting register*/
typedef union _Adc_CON1_t {
    unsigned char byte;
    struct _Adc_Con1 {
        unsigned char    set_start             : 4;
        unsigned char    set_Rstart            : 4;
    }Adc_Con1;
}  __attribute__((packed)) Adc_CON1_t;

/*OSC Configure setting register*/
typedef union _Osc_Conf_t {
    unsigned char byte;
    struct _Osc_Conf {
        unsigned char    pd_osc             : 1;
        unsigned char    res0               : 4;
        unsigned char    SHP_By             : 1;
        unsigned char    CAPUP              : 1;
        unsigned char    res1               : 1;
    }Osc_Conf;
}  __attribute__((packed)) Osc_Conf_t;

/*OSC Status register*/
typedef union _Osc_Status_t {
    unsigned char byte;
    struct _Osc_Status {
        unsigned char    res0               : 7;
        unsigned char    Clk_Det            : 1;
    }Osc_Status;
}  __attribute__((packed)) Osc_Status_t;

/*NON OV delay register*/
typedef union _NonOv_Delay_t {
    unsigned char byte;
    struct _NonOv_Delay {
            unsigned char   Delay_P         :4;
            unsigned char   Delay_N         :4;
    }NonOv_Delay;
}__attribute__((packed))NonOv_Delay_t;

/* Register MAP of DCDC converter*/

typedef struct _Dcdc_register_map {
    unsigned char           B0_Coeh;
    unsigned char           B0_Coel;
    unsigned char           B1_Coeh;
    unsigned char           B1_Coel;
    unsigned char           B2_Coeh;
    unsigned char           B2_Coel;
    Clock_setup0_t          Clk_Set0;
    Clock_setup1_t          Clk_Set1;
    unsigned char           PWM_Confh;
    unsigned char           PWM_Confl;
    Bias_Vreg0_t            Bias_Vreg0;
    unsigned char           Bias_Vreg1;
    Adc_Gen0_t              Adc_Gen0;
    Adc_Gen1_t              Adc_Gen1;
    Adc_CON0_t              Adc_Con0;
    Adc_CON1_t              Adc_Con1;
    unsigned char           res0[5];
    NonOv_Delay_t           Non_OV_Delay;
    unsigned char           res1;
    unsigned char           Duty_Cycle_max_sat;
    unsigned char           Duty_Cycle_min_sat;
    unsigned char           res2[6];
    Osc_Conf_t              Osc_Conf;
    Osc_Status_t            Osc_Sta;
}  __attribute__((packed)) Dcdc_register_map;

/*Define a pointer to the platform dependent set function*/
typedef int (*IfxDcdcPlatformSetCallback_t)(int arg);
/*Define a pointer to the platform dependent get function*/
typedef int (*IfxDcdcPlatformGetCallback_t)(void);
/*Define a pointer to the platform dependent Enable/Disable function*/
typedef int (*IfxDcdcPlatformEnableDisableCallback_t)(void);
/*CB type to set the Non-OV Delay*/
typedef int (*IfxDcdcPlatformSetCallbackType2_t)(unsigned int arg1,
                                                 unsigned int arg2);

/*Table which contains the pointers to the platform dependent callbacks
* These callbacks would be used to access platform dependent DCDC converter
* Registers, Each platform should provide these callbacks*/
typedef struct  _Ifx_dcdc_platform_callbacks_table {
    IfxDcdcPlatformSetCallback_t            Ifx_dcdc_voltage_set_cb;
    IfxDcdcPlatformGetCallback_t            Ifx_dcdc_voltage_get_cb;
    IfxDcdcPlatformSetCallback_t            Ifx_dcdc_DutyCycleMax_set_cb;
    IfxDcdcPlatformSetCallback_t            Ifx_dcdc_PwmSwitchingFreq_set_cb;
    IfxDcdcPlatformSetCallbackType2_t       Ifx_dcdc_NonOvDelay_set_cb;
    IfxDcdcPlatformEnableDisableCallback_t  Ifx_dcdc_PfmModeEnable_cb;
    IfxDcdcPlatformEnableDisableCallback_t  Ifx_dcdc_PfmModeDisable_cb;
} Ifx_dcdc_platform_callbacks_table;
#endif
