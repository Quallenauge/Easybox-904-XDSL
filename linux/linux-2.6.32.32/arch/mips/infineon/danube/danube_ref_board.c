/******************************************************************************
**
** FILE NAME    : danube_ref_board.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for Danube
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
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_ledc.h>
#include <asm/ifx/ifx_led.h>


//  GPIO PIN to Module Mapping and default PIN configuration
struct ifx_gpio_ioctl_pin_config g_board_gpio_pin_map[] = {
    /*
     *  ASC0
     */
    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0,  9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    {IFX_GPIO_MODULE_ASC0, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  PAGE BUTTON DRIVER for DECT
     */
    {IFX_GPIO_MODULE_PAGE, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR },

    /*
     *  COSIC DRIVER for DECT
     */
    {IFX_GPIO_MODULE_DECT, IFX_GPIO_PIN_ID(0, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    
    /*
     *  SSC (SPI)
     */
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 0), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(1, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    {IFX_GPIO_MODULE_SPI_FLASH, IFX_GPIO_PIN_ID(0, 15), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     * LEDC (Unified Danube Board does not use LEDC)
     */
    {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     * LED (LED configuration for Danube V3 board)
     */
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  EBU CS1 (Unified Danube Board use EBU to control LEDs)
     */
    {IFX_GPIO_MODULE_EBU_LED, IFX_GPIO_PIN_ID(1, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  MII1 (MII mode)
     */
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 5), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 8), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(0, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 14), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    {IFX_GPIO_MODULE_PPA, IFX_GPIO_PIN_ID(1, 15), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},

    /*
     *  TAPI - VMMC
     */
    //  TDM/FSC - used as INPUT by default (FSC Slave)
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(0, 0), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //  TDM/DO
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(1, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //  TDM/DI
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(1, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET},
    //  TDM/DCL - used as INPUT by default (DCL Slave)
    {IFX_GPIO_MODULE_TAPI_VMMC, IFX_GPIO_PIN_ID(1, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  TAPI - FXO
     */
    //  FXO CSQ
    {IFX_GPIO_MODULE_TAPI_FXO, IFX_GPIO_PIN_ID(0, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET},
    //  GPTimer 3A - used as FXO irqQ
    {IFX_GPIO_MODULE_TAPI_FXO, IFX_GPIO_PIN_ID(0, 6), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET | IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET},

    //  module_id of last item must be IFX_GPIO_PIN_AVAILABLE
    {IFX_GPIO_PIN_AVAILABLE, 0, 0}
};
EXPORT_SYMBOL(g_board_gpio_pin_map);

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
    .number_of_enabled_led  = 0,    //  turn off LEDC
    .data_mask              = (1 << 16) - 1,
    .data                   = (1 << 4) | (1 << 5),  // LED4 - Warning LED turn on by default, LED5 - USB LED turn on by default
    .mips0_access_mask      = (1 << 16) - 1,
    .mips0_access           = (1 << 16) - 1,
    .f_data_clock_on_rising = 0,    //  falling edge
};
EXPORT_SYMBOL(g_board_ledc_hw_config);

struct ifx_led_device g_board_led_hw_config[] = {
    {
        .name               = "wps_led",
        .default_trigger    = NULL,
        .phys_id            = 0,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "wireless_led",        
        .default_trigger    = NULL,
        .phys_id            = 1,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "warning_led",
        .default_trigger    = NULL,
        .phys_id            = 2,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "voip_led",
        .default_trigger    = NULL,
        .phys_id            = 3,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "sdlink_led",
        .default_trigger    = NULL,
        .phys_id            = 4,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "usbdev_led",
        .default_trigger    = NULL,
        .phys_id            = 5,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "fxs2link_led",
        .default_trigger    = NULL,
        .phys_id            = 6,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "fxs1link_led",
        .default_trigger    = NULL,
        .phys_id            = 7,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "fxs_relay",
        .default_trigger    = NULL,
        .phys_id            = 8,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
/*    {
        .name               = "dect_cs_d",
        .default_trigger    = NULL,
        .phys_id            = 9,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },*/
    {
        .name               = "vdsl2_spi_cs",
        .default_trigger    = NULL,
        .phys_id            = 10,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "vdsl2_reset",
        .default_trigger    = NULL,
        .phys_id            = 11,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "dect_mode",
        .default_trigger    = NULL,
        .phys_id            = 12,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "dect_isdn_fxs_reset",
        .default_trigger    = NULL,
        .phys_id            = 13,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "isdn_cs2",
        .default_trigger    = NULL,
        .phys_id            = 14,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
    {
        .name               = "usb_power_on",
        .default_trigger    = "USB_VBUS",
        .phys_id            = 15,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_EBU,
    },
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
        .flags              = IFX_LED_DEVICE_FLAG_INVALID,
    }
};
EXPORT_SYMBOL(g_board_led_hw_config);

#ifdef CONFIG_MTD_IFX_NOR

/* NOR flash partition table */
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

#ifdef CONFIG_MTD_IFX_NAND

/* NOR flash partition table */
const struct mtd_partition g_ifx_mtd_nand_partitions[] = {
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
	     .offset  = 0x00280000,
	     .size    = 0x00510000,
    }
};
const int g_ifx_mtd_nand_partion_num = ARRAY_SIZE(g_ifx_mtd_nand_partitions);
EXPORT_SYMBOL(g_ifx_mtd_nand_partion_num);
EXPORT_SYMBOL(g_ifx_mtd_nand_partitions);
#endif /* CONFIG_IFX_MTD_NAND */



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

};
EXPORT_SYMBOL(g_ifx_mtd_spi_partitions);

#endif /* defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) */


