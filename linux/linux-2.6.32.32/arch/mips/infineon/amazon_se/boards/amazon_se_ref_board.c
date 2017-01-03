/******************************************************************************
**
** FILE NAME    : amazon_se_ref_board.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : source file for Amazon-SE
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
#include <asm/ifx/ifx_ledc.h>
#include <asm/ifx/ifx_led.h>

/*
struct ifx_si_eiu_config g_si_eiu_config = {
    .irq            = -1,   //  no serial input
    .intsync        = 0,
    .sampling_clk   = 0,
    .shift_clk      = 0,
    .group          = 0,
    .active_high    = 0,
};
*/

#if defined(IFX_LEDGPIO_USB_VBUS) || defined(IFX_LEDGPIO_USB_VBUS1) || defined(IFX_LEDGPIO_USB_VBUS2)
	#undef IFX_GPIO_USB_VBUS
	#undef IFX_GPIO_USB_VBUS1
	#undef IFX_GPIO_USB_VBUS2
#endif

//  GPIO PIN to Module Mapping and default PIN configuration
struct ifx_gpio_ioctl_pin_config g_board_gpio_pin_map[] = {
    //  module_id of last item must be IFX_GPIO_PIN_AVAILABLE
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(0, 8), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR},
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(0, 9), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_SSC, IFX_GPIO_PIN_ID(0, 10), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    {IFX_GPIO_MODULE_SPI_FLASH, IFX_GPIO_PIN_ID(0, 7), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    // XWAY ASE WAVE board doesn't support shift register
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LEDC, IFX_GPIO_PIN_ID(0, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  GPIO LEDs (Amazon-SE Reference Board use GPIO to control LEDs)
     */

    // XWAY ASE WAVE board support only 2 LED.
    // GPIO2 for ADSL_DAT , GPIO3 for ADSL_LINK
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 1), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 2), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    {IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 3), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 4), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 12), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(0, 13), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 8), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
    //{IFX_GPIO_MODULE_LED, IFX_GPIO_PIN_ID(1, 11), IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},

    /*
     *  USB
     */
	#if defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)
		#if   defined(IFX_LEDGPIO_USB_VBUS)
			{IFX_GPIO_MODULE_LED, IFX_LEDGPIO_USB_VBUS, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
		#endif
		#if   defined(IFX_GPIO_USB_VBUS)
			{IFX_GPIO_MODULE_USB, IFX_GPIO_USB_VBUS, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET | IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET},
		#endif
	#endif
	#if   (defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)) && defined(CONFIG_USB_HOST_IFX_LED)
		{IFX_GPIO_MODULE_LED, IFX_LEDGPIO_USB_LED, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},
	#elif (defined(CONFIG_USB_GADGET_IFX) || defined(CONFIG_USB_GADGET_IFX_MODULE)) && defined(CONFIG_USB_GADGET_IFX_LED)
//		{IFX_GPIO_MODULE_LED, IFX_LEDGPIO_USB_LED, IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR | IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET},	
	#endif
    {IFX_GPIO_PIN_AVAILABLE, 0, 0},
};
EXPORT_SYMBOL(g_board_gpio_pin_map);

struct ifx_ledc_config_param g_board_ledc_hw_config = {
    .operation_mask         = IFX_LEDC_CFG_OP_UPDATE_SOURCE | IFX_LEDC_CFG_OP_BLINK | IFX_LEDC_CFG_OP_UPDATE_CLOCK | IFX_LEDC_CFG_OP_STORE_MODE | IFX_LEDC_CFG_OP_SHIFT_CLOCK | IFX_LEDC_CFG_OP_DATA_OFFSET | IFX_LEDC_CFG_OP_NUMBER_OF_LED | IFX_LEDC_CFG_OP_DATA | IFX_LEDC_CFG_OP_DATA_CLOCK_EDGE,
    .source_mask            = 3,
    .source                 = 0,    //  by default all LEDs controlled by LEDC DATA
    .blink_mask             = (1 << 16) - 1,
    .blink                  = 0,    //  disable blink for all LEDs
    .update_clock           = LED_CON1_UPDATE_SRC_SOFTWARE,
    .fpid                   = 0,
    .store_mode             = 0,    //  single store
    .fpis                   = 0,
    .data_offset            = 0,
    .number_of_enabled_led  = 0,    //  disable LEDC
    .data_mask              = (1 << 16) - 1,
    .data                   = (1 << 2), // LED2 - Power LED turn on by default
    .mips0_access_mask      = (1 << 16) - 1,
    .mips0_access           = (1 << 16) - 1,
    .f_data_clock_on_rising = 0,    //  falling edge
};
EXPORT_SYMBOL(g_board_ledc_hw_config);

struct ifx_led_device g_board_led_hw_config[] = {
#if defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)
	#if   defined(IFX_LEDGPIO_USB_VBUS)
	    {
	        .name               = "USB_VBUS",
	        .default_trigger    = "USB_VBUS",
	        .phys_id            = IFX_LEDGPIO_USB_VBUS,
	        .value_on           = 1,
	        .value_off          = 0,
	        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
	    },
	#endif
#endif
#if   (defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)) && defined(CONFIG_USB_HOST_IFX_LED)
	    {
	        .default_trigger    = IFX_LED_TRIGGER_USB_LINK,
	        .phys_id            = IFX_LEDGPIO_USB_LED,
	        .value_on           = 0,
	        .value_off          = 1,
	        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
	    },
#elif (defined(CONFIG_USB_GADGET_IFX) || defined(CONFIG_USB_GADGET_IFX_MODULE)) && defined(CONFIG_USB_GADGET_IFX_LED)
	    {
//	        .default_trigger    = IFX_LED_TRIGGER_USB_LINK,
//	        .phys_id            = IFX_LEDGPIO_USB_LED,
//	        .value_on           = 0,
//	        .value_off          = 1,
//	        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
	    },
#endif
    {
        .name		    = "broadband_led",
	//.default_trigger    = IFX_LED_TRIGGER_DSL_LINK, 
        .default_trigger    = NULL,
        .phys_id            = 3,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
	.name		    = "internet_led",
        //.default_trigger    = IFX_LED_TRIGGER_DSL_DATA, 
        .default_trigger    = NULL,
        .phys_id            = 2,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
 // XWAY ASE WAVE board supports only 2 LEDs.
/*
    {
        .default_trigger    = IFX_LED_TRIGGER_EPHY_LINK,
        .phys_id            = 3,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .default_trigger    = IFX_LED_TRIGGER_EPHY_SPEED,
        .phys_id            = 4,
        .value_on           = 1,
        .value_off          = 0,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .default_trigger    = IFX_LED_TRIGGER_WAN_STATUS,
        .phys_id            = 12,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .default_trigger    = IFX_LED_TRIGGER_POWER_ON,
        .phys_id            = 13,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .default_trigger    = IFX_LED_TRIGGER_WARNING,
        .phys_id            = 24,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
    {
        .default_trigger    = IFX_LED_TRIGGER_USB_LINK,
        .phys_id            = 27,
        .value_on           = 0,
        .value_off          = 1,
        .flags              = IFX_LED_DEVICE_FLAG_PHYS_GPIO,
    },
*/
    {
        .flags              = IFX_LED_DEVICE_FLAG_INVALID,
    }
};
EXPORT_SYMBOL(g_board_led_hw_config);


#ifdef CONFIG_MTD_IFX_NOR

#if (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 2)
#define IFX_MTD_NOR_PARTITION_SIZE    0x001B0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name       = "U-Boot",     /* U-Boot firmware */
        .offset     = 0x00000000,
        .size       = 0x00020000,   //128K
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "firmware",   /* firmware */
        .offset     = 0x00020000,
        .size       = 0x00030000,   //192K
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "rootfs,kernel,Data,Environment",     /* default partition */
        .offset     = 0x00050000,
        .size       = IFX_MTD_NOR_PARTITION_SIZE,
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
};
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 4)
#define IFX_MTD_NOR_PARTITION_SIZE    0x003C0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name       = "U-Boot",
        .offset     = 0x00000000,
        .size       = 0x00010000,
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "Firmware",
        .offset     = 0x00010000,
        .size       = 0x00030000,
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "RootFS,Kernel,Data,Environment",     /* default partition */
        .offset     = 0x00040000,
        .size       = IFX_MTD_NOR_PARTITION_SIZE,
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
};
#elif (CONFIG_MTD_IFX_NOR_FLASH_SIZE == 8)
#define IFX_MTD_NOR_PARTITION_SIZE    0x007A0000
const struct mtd_partition g_ifx_mtd_nor_partitions[] = {
    {
        .name       = "U-Boot",     /* U-Boot firmware */
        .offset     = 0x00000000,
        .size       = 0x00020000,   //128K
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "firmware",   /* firmware */
        .offset     = 0x00020000,
        .size       = 0x00040000,   //256K
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
    {
        .name       = "rootfs,kernel,Data,Environment",     /* default partition */
        .offset     = 0x00060000,
        .size       = IFX_MTD_NOR_PARTITION_SIZE,
/*      .mask_flags = MTD_WRITEABLE,    force read-only */
    },
};
#else
#error  "Configure MTD NOR flash size first!!"
#endif
const int g_ifx_mtd_partion_num = ARRAY_SIZE(g_ifx_mtd_nor_partitions);

EXPORT_SYMBOL(g_ifx_mtd_nor_partitions);
EXPORT_SYMBOL(g_ifx_mtd_partion_num);
#endif /* CONFIG_IFX_MTD_NOR */
/*fix me, need more concept to define the partitions, and need add partition for 2M*/
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
	     .size    = 0x00220000,
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
};
EXPORT_SYMBOL(g_ifx_mtd_spi_partitions);

#endif /* defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) */

