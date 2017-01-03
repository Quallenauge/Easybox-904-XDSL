/******************************************************************************
**
** FILE NAME    : ar9_ref_board.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for AR9
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
** $Date        $Author         $Comment
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kallsyms.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_board.h>
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_si.h>
#include <asm/ifx/ifx_ledc.h>
#include <asm/ifx/ifx_led.h>


//  GPIO PIN to Module Mapping and default PIN configuration
struct ifx_gpio_ioctl_pin_config g_board_gpio_pin_map[] = {
    /*
     *  ASC0 (not used in refernce board, pin-9 is assigned to GPHY)
     */
//    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0,  9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
//    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
//    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
//    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  SSC (SPI)
     */
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 0), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    {IFX_GPIO_MODULE_SPI_FLASH, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  Serial In Controller
     */
#if defined(CONFIG_IFX_SI) || defined(CONFIG_IFX_SI_MODULE)
    /* SIN_CE (CE#) */
    {IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* SIN_SH */
    {IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(2, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* SIN_ST (PE#) */
    {IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(2, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* SIN_SD */
    {IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(2, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
#endif

#if defined(CONFIG_IFX_GW188)
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 15), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(2, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#elif defined(CONFIG_LTQ_VB300)
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(2, 17), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(2, 18), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(2, 19), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(2, 20), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#else
    /*
     *  LED Controller
     */
    {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#endif

    /*
     *  TAPI - VMMC
     */
    //  TDM/FSC - used as INPUT by default (FSC Slave)
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(0, 0), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //  TDM/DO
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(1, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //  TDM/DI
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(2, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    //  TDM/DCL - used as INPUT by default (DCL Slave)
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(2, 8), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  TAPI - FXO
     */
    //  FXO rstQ
    {IFX_GPIO_MODULE_TAPI_FXO, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},
    //  FXO CSQ
    {IFX_GPIO_MODULE_TAPI_FXO, IFX_GPIO_PIN_ID(3, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET},

    /*
     *  EXIN5 - reserved by ICU driver when IRQ is requested, it will be reserved by FXO
     */
    //{IFX_GPIO_MODULE_EXIN, IFX_GPIO_PIN_ID(2, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},
#ifdef CONFIG_LTQ_VB300
    {IFX_GPIO_MODULE_EXIN, IFX_GPIO_PIN_ID(0, 0), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},
    {IFX_GPIO_MODULE_EXIN, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},
#endif
    /*
     *  PAGE BUTTON DRIVER for DECT
     */
    {IFX_GPIO_MODULE_PAGE, IFX_GPIO_PIN_ID(2, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR },

    /*
     *  COSIC DRIVER for DECT
     */
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(2, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},

#if defined(CONFIG_IFX_GW188)
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(2, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#else
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#endif

#if defined(CONFIG_IFX_GW188)
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(2, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#elif defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE)
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(2,1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#else
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#endif
    /*
     *  USB
     */
#if defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)
    {IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS1, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
    {IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS2, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
#endif

    //  module_id of last item must be IFX_GPIO_PIN_AVAILABLE
    {IFX_GPIO_PIN_AVAILABLE, 0, 0}
};
EXPORT_SYMBOL(g_board_gpio_pin_map);

#if defined(CONFIG_IFX_SI) || defined(CONFIG_IFX_SI_MODULE)
ifx_si_config_param_t g_board_si_hw_config = {
    .operation_mask         = IFX_SI_CFG_OP_UPDATE_SOURCE | IFX_SI_CFG_OP_UPDATE_MODE | IFX_SI_CFG_OP_UPDATE_GROUP | IFX_SI_CFG_OP_UPDATE_SHIFTCLKDIV,
    .update_source_type     = IFX_SI_UPD_SRC_FPID,
    .sampling_clk_div       = IFX_SI_FPI_SAMPLECLK_DIV_160,
    .sync_mode              = IFX_SI_ASYNC_MODE,    //  AR9/VR9: 74HCT165 : Async Mode
    .input_group_type       = IFX_SI_GROUP_00,      //  disable serial in module by default
    .shift_in_clk_div       = IFX_SI_FPI_SHIFTCLK_DIV_016,
};
EXPORT_SYMBOL(g_board_si_hw_config);
#endif

struct ifx_ledc_config_param g_board_ledc_hw_config = {
    .operation_mask         = IFX_LEDC_CFG_OP_UPDATE_SOURCE | IFX_LEDC_CFG_OP_BLINK | IFX_LEDC_CFG_OP_UPDATE_CLOCK | IFX_LEDC_CFG_OP_STORE_MODE | IFX_LEDC_CFG_OP_SHIFT_CLOCK | IFX_LEDC_CFG_OP_DATA_OFFSET | IFX_LEDC_CFG_OP_NUMBER_OF_LED | IFX_LEDC_CFG_OP_DATA | IFX_LEDC_CFG_OP_MIPS0_ACCESS | IFX_LEDC_CFG_OP_DATA_CLOCK_EDGE,
    .source_mask            = 3,
    .source                 = 0,    //  by default all LEDs controlled by LEDC DATA
    .blink_mask             = (1 << 16) - 1,
    .blink                  = 0,    //  disable blink for all LEDs
    .update_clock           = LED_CON1_UPDATE_SRC_SOFTWARE,
    .fpid                   = 0,
    .store_mode             = 0,    //  single store
    .fpis                   = 0,
    .data_offset            = 0,
    .number_of_enabled_led  = 16,
    .data_mask              = (1 << 16) - 1,
    .data                   = (1 << 0) | (1 << 1) | (1 << 14),  // LED0,1 - 1 is turning off, LED14 - Warning LED turns on by default
    .mips0_access_mask      = (1 << 16) - 1,
    .mips0_access           = (1 << 16) - 1,
    .f_data_clock_on_rising = 0,    //  falling edge
};
EXPORT_SYMBOL(g_board_ledc_hw_config);
#if defined(CONFIG_IFX_GW188)
struct ifx_led_device g_board_led_hw_config[] = {
    {
        .name               = "internet_led",
        .default_trigger    = NULL,
        .phys_id            = 5,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "broadband_led",
        .default_trigger    = NULL,
        .phys_id            = 4,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "warning_led",
        .default_trigger    = NULL,
        .phys_id            = 15,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "fxs1_link_led",
        .default_trigger    = NULL,
        .phys_id            = 19,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "fxs2_link_led",
        .default_trigger    = NULL,
        .phys_id            = 20,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "fxo_act_led",
        .default_trigger    = NULL,
        .phys_id            = 22,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "usb1_link_led",
        .default_trigger    = NULL,
        .phys_id            = 26,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "usb2_link_led",
        .default_trigger    = NULL,
        .phys_id            = 28,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "wps_led",
        .default_trigger    = NULL,
        .phys_id            = 45,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .flags              = IFX_LED_DEVICE_FLAG_INVALID,
    }
};
#elif defined(CONFIG_LTQ_VB300)
struct ifx_led_device g_board_led_hw_config[] = {
    {
        .name               = "led_wlan1",
        .default_trigger    = NULL,
        .phys_id            = 49,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "led_wlan0",
        .default_trigger    = NULL,
        .phys_id            = 50,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "led_band",
        .default_trigger    = NULL,
        .phys_id            = 51,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "led_pwr",
        .default_trigger    = NULL,
        .phys_id            = 52,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .flags              = IFX_LED_DEVICE_FLAG_INVALID,
    }
};
#else
struct ifx_led_device g_board_led_hw_config[] = {
    {
        .name               = "internet_led",
        .default_trigger    = NULL,
        .phys_id            = 0,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "broadband_led",
        .default_trigger    = NULL,
        .phys_id            = 1,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "internet_red_led",
        .default_trigger    = NULL,
        .phys_id            = 2,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "wps_red_led",
        .default_trigger    = NULL,
        .phys_id            = 3,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 4,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 5,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 6,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "voip_led",
        .default_trigger    = NULL,
        .phys_id            = 7,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 8,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "wps_led",
        .default_trigger    = NULL,
        .phys_id            = 9,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
#if (defined(CONFIG_USB_GADGET_IFX) || defined(CONFIG_USB_GADGET_IFX_MODULE)) && defined(CONFIG_USB_GADGET_IFX_LED)
    {
        .name               = "usbdev_led",
        .default_trigger    = "usb_link",
        .phys_id            = IFX_LEDLED_USB_LED,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
#endif
    {
        .default_trigger    = NULL,
        .phys_id            = 11,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 12,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 13,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 14,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "wireless_led",
        .default_trigger    = NULL,
        .phys_id            = 15,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .flags              = IFX_LED_DEVICE_FLAG_INVALID,
    }
};
#endif
EXPORT_SYMBOL(g_board_led_hw_config);

#ifdef CONFIG_MTD_IFX_NOR

/* NOR flash partiontion table */
#if (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 2)
#define IFX_MTD_NOR_PARTITION_SIZE    0x001F0000
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 4)
#define IFX_MTD_NOR_PARTITION_SIZE    0x003F0000
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 8)
#define IFX_MTD_NOR_PARTITION_SIZE    0x007F0000
#else
#error  "Configure IFXCPE MTD flash size first!!"
#endif
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name    = "U-Boot",
        .offset  = 0x00000000,
        .size    = 0x00010000,
    },
    {
        .name    = "kernel,rootfs,firmware,data,environment",
        .offset  = 0x00010000,
        .size    = IFX_MTD_NOR_PARTITION_SIZE,
    },
};
const int g_ifx_mtd_partion_num = ARRAY_SIZE(g_ifx_mtd_nor_partitions);
EXPORT_SYMBOL(g_ifx_mtd_partion_num);
EXPORT_SYMBOL(g_ifx_mtd_nor_partitions);

#endif /* CONFIG_IFX_MTD_NOR */

#if defined(CONFIG_MTD_IFX_NAND) && !defined(CONFIG_MTD_CMDLINE_PARTS)

const struct mtd_partition g_ifx_mtd_nand_partitions[] = {
#if (CONFIG_MTD_IFX_NAND_FLASH_SIZE == 4)
     {
          .name    = "U-Boot",
          .offset  = 0x00000000,
          .size    = 0x00008000,
     },
     {
          .name    = "kernel",
          .offset  = 0x00080000,
          .size    = 0x00100000,
     },
     {
          .name    = "rootfs",
          .offset  = 0x00180000,
          .size    = 0x00260000,
     },

#elif (CONFIG_MTD_IFX_NAND_FLASH_SIZE == 8)
    {
       .name    = "U-Boot",
       .offset  = 0x00000000,
       .size    = 0x00080000,
    },
    {
       .name    = "kernel",
       .offset  = 0x00080000,
       .size    = 0x00200000,
    },
    {
       .name    = "rootfs",
       .offset  = 0x00280000,
       .size    = 0x00510000,
    },
#endif
};

const int g_ifx_mtd_nand_partion_num = ARRAY_SIZE(g_ifx_mtd_nand_partitions);
EXPORT_SYMBOL(g_ifx_mtd_nand_partion_num);
EXPORT_SYMBOL(g_ifx_mtd_nand_partitions);

#endif /* CONFIG_MTD_IFX_NAND */


#if defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE)
/*
 * spi flash partition information
 * Here are partition information for all known series devices.
 * See include/linux/mtd/partitions.h for definition of the mtd_partition
 * structure.
 */
#define IFX_MTD_SPI_PARTITION_2MB_SIZE    0x001B0000
#define IFX_MTD_SPI_PARTITION_4MB_SIZE    0x003A0000
#define IFX_MTD_SPI_PARTITION_8MB_SIZE    0x007A0000
#define IFX_MTD_SPI_PARTITION_16MB_SIZE   0x00FA0000

const struct mtd_partition g_ifx_mtd_spi_partitions[IFX_SPI_FLASH_MAX][IFX_MTD_SPI_PART_NB] = {
    {{0, 0, 0}},

/* 256K Byte */
    {{
        .name   =      "spi-boot",      /* U-Boot firmware */
        .offset =      0x00000000,
        .size   =      0x00040000,         /* 256 */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }, {0}, {0},
    },

/* 512K Byte */
    {{0, 0, 0}},

/* 1M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00010000,        /* 64K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-firmware", /* firmware */
        .offset =       0x00010000,
        .size   =       0x00030000,        /* 64K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00030000,
        .size   =       0x000C0000,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},

/* 2M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00020000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-firmware", /* firmware */
        .offset =       0x00020000,
        .size   =       0x00030000,        /* 192K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00050000,
        .size   =       IFX_MTD_SPI_PARTITION_2MB_SIZE,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},

/* 4M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00020000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-firmware", /* firmware */
        .offset =       0x00020000,
        .size   =       0x00040000,        /* 256K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00060000,
        .size   =       IFX_MTD_SPI_PARTITION_4MB_SIZE,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},

/* 8M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00020000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =      "spi-firmware",  /* firmware */
        .offset =      0x00020000,
        .size   =      0x00030000,         /* 192K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00050000,
        .size   =       IFX_MTD_SPI_PARTITION_8MB_SIZE,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},

/* 16M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00020000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =      "spi-firmware",  /* firmware */
        .offset =      0x00020000,
        .size   =      0x00030000,         /* 192K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00050000,
        .size   =       IFX_MTD_SPI_PARTITION_16MB_SIZE,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},
};
EXPORT_SYMBOL(g_ifx_mtd_spi_partitions);

#endif /* defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) */

