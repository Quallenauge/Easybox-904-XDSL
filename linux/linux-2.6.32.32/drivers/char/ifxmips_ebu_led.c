/******************************************************************************
**
** FILE NAME    : ifxmips_ebu_led.c
** PROJECT      : UEIP
** MODULES      : EBU to Control LEDs
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : EBU LED Controller driver common source file
** COPYRIGHT    :       Copyright (c) 2006
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
** 16 JUL 2009  Xu Liang        Init Version
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */

#define IFX_EBU_LED_VER_MAJOR           1
#define IFX_EBU_LED_VER_MID             0
#define IFX_EBU_LED_VER_MINOR           6



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kallsyms.h>
#include <asm/delay.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_gpio.h>
#include "ifxmips_ebu_led.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

spinlock_t g_register_lock;//  use spinlock rather than semaphore or mutex
                                //  because most functions run in user context
                                //  and they do not take much time to finish operation

static unsigned int g_led_value = 0;
static unsigned int g_dbg_enable = DBG_ENABLE_MASK_ERR;



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static INLINE int ifx_ebu_led_version(char *buf)
{
    return ifx_drv_ver(buf, "EBU LED Controller", IFX_EBU_LED_VER_MAJOR, IFX_EBU_LED_VER_MID, IFX_EBU_LED_VER_MINOR);
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*!
  \fn       int ifx_ebu_led_set_data(unsigned int led, unsigned int data)
  \brief    Turn on/off LED.

            User uses this function to turn on/off given LED.

  \param    led     - unsigned int, ID of LED
  \param    data    - unsigned int, 0: off, 1: on
  \return   IFX_SUCCESS     Operation succeed.
  \return   IFX_ERROR       Operation fail.
  \ingroup  IFX_LEDEBU_API
 */
int ifx_ebu_led_set_data(unsigned int led, unsigned int data)
{
    unsigned long sys_flags;

    spin_lock_irqsave(&g_register_lock, sys_flags);
    if ( data )
        g_led_value |= 1 << led;
    else
        g_led_value &= ~(1 << led);
    //  EBU LED is quite tricky.
    //  System write low 16 bits first, then high 16 bits.
    //  The consecutive output will not affect low speed device like LED.
    //  Because high 16 bits always overwrite low 16 bits in very short time.
    //  But this has side-effect on high speed device like PCI reset signal.
    //  To workaround, always write the same content on both low and high 16 bits.
    IFX_REG_W32((g_led_value << 16) | g_led_value, EBU_LED_BASE_ADDR);
    spin_unlock_irqrestore(&g_register_lock, sys_flags);

    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_ebu_led_set_data);

/*!
  \fn       void ifx_ebu_led_enable(void)
  \brief    Enable EBU (for LED control) and do hardware initialization.

            LED driver use this function initialize EBU for LED control.

  \ingroup  IFX_LEDEBU_API
 */
void ifx_ebu_led_enable(void)
{
    unsigned long sys_flags;
    unsigned int tmp;

    if ( ifx_gpio_register(IFX_GPIO_MODULE_EBU_LED) != IFX_SUCCESS ) {
        dbg("GPIO register fail, please check whether EBU Address Range 1 is shared with other module.");
    }

    spin_lock_irqsave(&g_register_lock, sys_flags);
    tmp = IFX_REG_R32(IFX_EBU_ADDSEL1);
    tmp = (tmp & ~0x1FFFF000) | CPHYSADDR(EBU_CS1_BASE_ADDR);   //  base address
    if ( (tmp & 0x01) == 0 || (tmp & 0xF0) > 0x30 )
        tmp = (tmp & ~0xF0) | 0x30;
    tmp |= 0x01;
    IFX_REG_W32(tmp, IFX_EBU_ADDSEL1);
    IFX_REG_W32_MASK((1 << 31) | (3 << 16), 1 << 16, IFX_EBU_BUSCON1);
    spin_unlock_irqrestore(&g_register_lock, sys_flags);
}
EXPORT_SYMBOL(ifx_ebu_led_enable);

/*!
  \fn       void ifx_ebu_led_disable(void)
  \brief    Disable EBU (for LED control) and release resources.

            LED driver use this function release resources allocated for EBU (LED control).

  \ingroup  IFX_LEDEBU_API
 */
void ifx_ebu_led_disable(void)
{
    ifx_gpio_deregister(IFX_GPIO_MODULE_EBU_LED);
}
EXPORT_SYMBOL(ifx_ebu_led_disable);



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

/**
 *  ifx_port_init - Initialize port structures
 *
 *  This function initializes the internal data structures of the driver
 *  and will create the proc file entry and device.
 *
 *      Return Value:
 *  @OK = OK
 */
static int __devinit ifx_ledc_init(void)
{
    char ver_str[256];

    spin_lock_init(&g_register_lock);

    ifx_ebu_led_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    return IFX_SUCCESS;
}

static void __exit ifx_ledc_exit(void)
{
}

module_init(ifx_ledc_init);
module_exit(ifx_ledc_exit);
