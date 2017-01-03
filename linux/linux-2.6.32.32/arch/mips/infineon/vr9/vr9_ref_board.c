/******************************************************************************
**
** FILE NAME    : vr9_ref_board.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for VR9
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


/* ctc */
/* GPIO PIN to Module Mapping and default PIN configuration */
struct ifx_gpio_ioctl_pin_config g_board_gpio_pin_map[] = {
    //  module_id of last item must be IFX_GPIO_PIN_AVAILABLE
    //{IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 0), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    //{IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

#ifdef CONFIG_USE_EMULATOR
    /* CS0 MXIC */
    //{IFX_GPIO_MODULE_SPI_FLASH, IFX_GPIO_PIN_ID(0, 15), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* CS1 ATMEL */
    //{IFX_GPIO_MODULE_SPI_FLASH, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#else
    //{IFX_GPIO_MODULE_SPI_FLASH, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#endif /* CONFIG_USE_EMULATOR */
//    {IFX_GPIO_MODULE_SPI_EEPROM, IFX_GPIO_PIN_ID(0, 15), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

   //{IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
   //{IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(0, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
   //{IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(0, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
   //{IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
   {IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(0, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR },
   {IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
  //{IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(1, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
  //{IFX_GPIO_MODULE_USIF_UART, IFX_GPIO_PIN_ID(1, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},

    //{IFX_GPIO_MODULE_USIF_SPI, IFX_GPIO_PIN_ID(0, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    //{IFX_GPIO_MODULE_USIF_SPI, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_USIF_SPI, IFX_GPIO_PIN_ID(2, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#ifdef CONFIG_USE_EMULATOR
    /* CS0 MXIC */
    //{IFX_GPIO_MODULE_USIF_SPI_SFLASH, IFX_GPIO_PIN_ID(2, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* CS1 ATMEL */
    //{IFX_GPIO_MODULE_USIF_SPI_SFLASH, IFX_GPIO_PIN_ID(2, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#else
    /* CS1 MXIC */
    //{IFX_GPIO_MODULE_USIF_SPI_SFLASH, IFX_GPIO_PIN_ID(2, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* CS2 MXIC */
    //{IFX_GPIO_MODULE_USIF_SPI_SFLASH, IFX_GPIO_PIN_ID(0, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#endif /* CONFIG_USE_EMULATOR */

    /*
     *  Serial In Controller
     */
#if defined(CONFIG_IFX_SI) || defined(CONFIG_IFX_SI_MODULE)
    /* SIN_CE (CE#) */
    //{IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* SIN_SH */
    //{IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(2, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* SIN_ST (PE#) */
    //{IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(2, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    /* SIN_SD */
    //{IFX_GPIO_MODULE_SI, IFX_GPIO_PIN_ID(2, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
#endif

    /*
     *  LED Controller
     */
#ifdef CONFIG_LTQ_RT288
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(2, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
// RT288 , Turn on RT288 Internal GPHY LEDs , and take this advantege  , GPIO 7 config to output mode instead of default parking in CFRAME.
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(2, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(2, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#else // GRX288
//  {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
//  {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
//  {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
//  {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
//In all models except RT288 , change GPIO7's setting to general GPIO output mode.
#endif

    /*
     *  Internal Switch
     */
//	{IFX_GPIO_MODULE_INTERNAL_SWITCH, IFX_GPIO_PIN_ID(0, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},


     /*
     *  PAGE BUTTON DRIVER for DECT
     */
//#ifdef CONFIG_IFX_USIF_SPI
//	{IFX_GPIO_MODULE_PAGE, IFX_GPIO_PIN_ID(2, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
//#else
//	{IFX_GPIO_MODULE_PAGE, IFX_GPIO_PIN_ID(0, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
//#endif
    //{IFX_GPIO_MODULE_PAGE, IFX_GPIO_PIN_ID(2, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    /*
     *  COSIC DRIVER for DECT
     */
    /* DECT_SPI_INT use GPIO9 as Interrupt input */
//	{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},
    //{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},
    /* DECT_SPI_CS use GPIO13 as SPI_CS3  */
//	{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    // DECT_SPI_CS use GPIO22 as SPI_CS2  //Gene 12/18/2009
//#ifdef CONFIG_IFX_USIF_SPI
//	{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(2, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
//#else
//	{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
//#endif
    //{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    /* DECT_RESET uses GPIO14 as DECT reset */
//	{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  USB
     */
#if defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)
 #if 1
//	{IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
    {IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET},
 #else
    {IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS1, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
    {IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS2, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
 #endif
#endif

#if 0
    /*
     *  DSL NTR
     */
    {IFX_GPIO_MODULE_DSL_NTR | IFX_GPIO_MODULE_EARLY_REGISTER, IFX_GPIO_PIN_ID(0, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
#endif

    {IFX_GPIO_PIN_AVAILABLE, 0, 0},
};
EXPORT_SYMBOL(g_board_gpio_pin_map);

#if defined(CONFIG_IFX_SI) || defined(CONFIG_IFX_SI_MODULE)
ifx_si_config_param_t g_board_si_hw_config = {
    .operation_mask         = IFX_SI_CFG_OP_UPDATE_SOURCE | IFX_SI_CFG_OP_UPDATE_MODE | IFX_SI_CFG_OP_UPDATE_GROUP | IFX_SI_CFG_OP_UPDATE_SHIFTCLKDIV,
    .update_source_type     = IFX_SI_UPD_SRC_FPID,
    .sampling_clk_div       = IFX_SI_FPI_SAMPLECLK_DIV_160,
    .sync_mode              = IFX_SI_ASYNC_MODE,    //  AR9/VR9: 74HCT165 : Async Mode
    .input_group_type       = IFX_SI_GROUP_08,
    .shift_in_clk_div       = IFX_SI_FPI_SHIFTCLK_DIV_016,
};
EXPORT_SYMBOL(g_board_si_hw_config);
#endif

struct ifx_ledc_config_param g_board_ledc_hw_config = {
    .operation_mask         = IFX_LEDC_CFG_OP_UPDATE_SOURCE | IFX_LEDC_CFG_OP_BLINK | IFX_LEDC_CFG_OP_UPDATE_CLOCK | IFX_LEDC_CFG_OP_STORE_MODE | IFX_LEDC_CFG_OP_SHIFT_CLOCK | IFX_LEDC_CFG_OP_DATA_OFFSET | IFX_LEDC_CFG_OP_NUMBER_OF_LED | IFX_LEDC_CFG_OP_DATA | IFX_LEDC_CFG_OP_MIPS0_ACCESS | IFX_LEDC_CFG_OP_DATA_CLOCK_EDGE,
    //.source_mask            = 0xFF,
    //.source                 = 0xFF, //  LEDs controlled by EXT Src
    //nsur
    .source_mask            = 3,
    .source                 = 0, //  by default ALL LEDs are controlled by LEDC DATA
    .blink_mask             = (1 << 24) - 1,
    .blink                  = 0,    //  disable blink for all LEDs
    .update_clock           = LED_CON1_UPDATE_SRC_SOFTWARE,
    .fpid                   = 0,
    .store_mode             = 0,    //  single store
    .fpis                   = 0,
    .data_offset            = 0,
    .number_of_enabled_led  = 24,
    .data_mask              = (1 << 24) - 1,
    .data                   = (1 << 0) | (1 << 1) | (1 << 14),  // LED0,1 - 1 is turning off, LED14 - Warning LED turns on by default
    .mips0_access_mask      = (1 << 24) - 1,
    .mips0_access           = (1 << 24) - 1,
    .f_data_clock_on_rising = 0,    //  falling edge
};
EXPORT_SYMBOL(g_board_ledc_hw_config);

#ifdef CONFIG_LTQ_RT288
struct ifx_led_device g_board_led_hw_config[] = {
    {
        .name               = "usb1_link_led",
        .default_trigger    = NULL,
        .phys_id            = 4,
        .value_on           = 1,
        .value_off          = 0,
        //.flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "usb2_link_led",
        .default_trigger    = NULL,
        .phys_id            = 6,
        .value_on           = 1,
        .value_off          = 0,
        //.flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "internet_led",
        .default_trigger    = NULL,
        .phys_id            = 22,
        .value_on           = 0,
        .value_off          = 1,
        //.flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .name               = "wps_led",
        .default_trigger    = NULL,
        .phys_id            = 35,
        .value_on           = 1,
        .value_off          = 0,
        //.flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .flags              = IFX_LED_DEVICE_FLAG_INVALID,
    }
};
#else
struct ifx_led_device g_board_led_hw_config[] = {
    {
        .name               = "broadband_led",
        .default_trigger    = NULL,
        .phys_id            = 0,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name               = "internet_led",
        .default_trigger    = NULL,
        .phys_id            = 1,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 2,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
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
        .default_trigger    = NULL,
        .phys_id            = 9,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 10,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 11,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name    	    = "wps_led",
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
        .name		    = "usb2_link_led",
        .default_trigger    = NULL,
        .phys_id            = 15,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 16,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 17,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name		    = "usb1_link_led",
        .default_trigger    = NULL,
        .phys_id            = 18,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name		    = "fxo_act_led",
        .default_trigger    = NULL,
        .phys_id            = 19,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name		    = "internet_red_led",
        .default_trigger    = NULL,
        .phys_id            = 20,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name		    = "voip_led",
        .default_trigger    = NULL,
        .phys_id            = 21,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .name		    = "warning_led",
        .default_trigger    = NULL,
        .phys_id            = 22,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_LEDC,
    },
    {
        .default_trigger    = NULL,
        .phys_id            = 23,
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

/* NOR flash partion table */
#if (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 2)
#define IFX_MTD_NOR_PARTITION_SIZE    0x001B0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name       = "U-Boot",       /* U-Boot firmware */
        .offset     = 0x00000000,
        .size       = 0x00020000, //128K
/*      .mask_flags = MTD_WRITEABLE,  force read-only */
    },
    {
        .name       = "firmware", /* firmware */
        .offset     = 0x00020000,
        .size       = 0x00030000, //192K
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "rootfs,kernel,Data,Environment",       /* default partition */
        .offset     = 0x00050000,
        .size       = IFX_MTD_NOR_PARTITION_SIZE,
/*      mask_flags  = MTD_WRITEABLE,   force read-only */
    },
};
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 4)
#define IFX_MTD_NOR_PARTITION_SIZE    0x003A0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name       = "U-Boot",       /* U-Boot firmware */
        .offset     = 0x00000000,
        .size       = 0x00020000, //128K
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "firmware", /* firmware */
        .offset     = 0x00020000,
        .size       = 0x00040000, //256K
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "rootfs,kernel,Data,Environment",       /* default partition */
        .offset     = 0x00060000,
        .size       = IFX_MTD_NOR_PARTITION_SIZE,
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
};
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 8)
#define IFX_MTD_NOR_PARTITION_SIZE    0x007A0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
         .name    = "U-Boot",
         .offset  = 0,
         .size    = 0x00080000,
    },
    {
          .name    = "Linux",
          .offset  = 0x00080000,
          .size    = 0x00200000,
    },
    {
          .name    = "Rootfs",
          .offset  = 0x00380000,
          .size    = 0x00200000,
    },

};
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 16)
#define IFX_MTD_NOR_PARTITION_SIZE    0x00FA0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name       = "U-Boot",        /* U-Boot firmware */
        .offset     = 0x00000000,
        .size       = 0x00020000, //128K
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "firmware", /* firmware */
        .offset     = 0x00020000,
        .size       = 0x00040000, //256K
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "rootfs,kernel,Data,Environment",       /* default partition */
        .offset     = 0x00060000,
        .size       = IFX_MTD_NOR_PARTITION_SIZE,
/*      mask_flags  = MTD_WRITEABLE,    force read-only */
    },
};
#else
#error  "Configure IFX MTD NOR flash size first!!"
#endif
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
       .size    = 0x00080000,
    },
    {
       .name    = "kernel",
       .offset  = 0x00080000,
       .size    = 0x00100000,
    },
    {
       .name    = "rootfs",
       .offset  = 0x00180000,
       .size    = 0x00280000,
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
       .size    = 0x00580000,
    },
#endif
};

const int g_ifx_mtd_nand_partion_num = ARRAY_SIZE(g_ifx_mtd_nand_partitions);
EXPORT_SYMBOL(g_ifx_mtd_nand_partion_num);
EXPORT_SYMBOL(g_ifx_mtd_nand_partitions);
#endif /* CONFIG_MTD_IFX_NAND */

#if defined(CONFIG_MTD_IFX_HSNAND) && !defined(CONFIG_MTD_CMDLINE_PARTS)

const struct mtd_partition g_ifx_mtd_nand_partitions[] = {
#if (CONFIG_MTD_IFX_HSNAND_FLASH_SIZE == 4)
    {
       .name    = "U-Boot",
       .offset  = 0x00000000,
       .size    = 0x00080000,
    },
    {
       .name    = "kernel",
       .offset  = 0x00080000,
       .size    = 0x00100000,
    },
    {
       .name    = "rootfs",
       .offset  = 0x00180000,
       .size    = 0x00280000,
    },
#elif (CONFIG_MTD_IFX_HSNAND_FLASH_SIZE == 8)
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
       .size    = 0x00580000,
    },
#endif
};

const int g_ifx_mtd_nand_partion_num = ARRAY_SIZE(g_ifx_mtd_nand_partitions);
EXPORT_SYMBOL(g_ifx_mtd_nand_partion_num);
EXPORT_SYMBOL(g_ifx_mtd_nand_partitions);
#endif /* CONFIG_MTD_IFX_HSNAND */


#if defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) \
    || defined(CONFIG_IFX_USIF_SPI_FLASH) || defined (CONFIG_IFX_USIF_SPI_FLASH_MODULE)
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

