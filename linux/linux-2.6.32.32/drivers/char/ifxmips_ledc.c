/******************************************************************************
**
** FILE NAME    : ifxmips_ledc.c
** PROJECT      : UEIP
** MODULES      : LED Controller (Serial Out)
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : LED Controller driver common source file
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

#define IFX_LEDC_VER_MAJOR              1
#define IFX_LEDC_VER_MID                0
#define IFX_LEDC_VER_MINOR              6



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/version.h>
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
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_gptu.h>
#include "ifxmips_ledc.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */

#ifdef CONFIG_DANUBE
  #define LEDC_GPT_SRC_TIMER            TIMER2B
#else
  #define LEDC_GPT_SRC_TIMER            TIMER2A
#endif



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  File Operations
 */
static int ifx_ledc_open(struct inode *inode, struct file *filep);
static int ifx_ledc_release(struct inode *inode, struct file *filelp);
static int ifx_ledc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

/*
 *  Software Update LED
 */
static INLINE int update_led(void);

/*
 *  Turn On/Off LED Controller
 */

static INLINE int turn_on_ledc(void);
static INLINE void turn_off_ledc(void);

/*
 *  GPT Setup & Release
 */
static INLINE int setup_gpt(int, unsigned long);
static INLINE void release_gpt(int);

/*
 *  Proc File Functions
 */
static INLINE void proc_file_create(void);
static INLINE void proc_file_delete(void);
static int proc_read_version(char *, char **, off_t, int, int *, void *);

/*
 *  Proc Help Functions
 */

/*
 *  Init Help Functions
 */
static INLINE int ifx_ledc_version(char *);

/*
 *  External Variable
 */
extern struct ifx_ledc_config_param g_board_ledc_hw_config; //  defined in board specific C file



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

spinlock_t g_ledc_register_lock;//  use spinlock rather than semaphore or mutex
                                //  because most functions run in user context
                                //  and they do not take much time to finish operation

static unsigned int g_f_ledc_on = 0;
static unsigned long g_gpt_freq = 0;

static struct file_operations g_ledc_fops = {
    .open    = ifx_ledc_open,
    .release = ifx_ledc_release,
    .ioctl   = ifx_ledc_ioctl
};

static unsigned int g_dbg_enable = DBG_ENABLE_MASK_ERR;

static struct proc_dir_entry* g_ledc_dir = NULL;



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static int ifx_ledc_open(struct inode *inode, struct file *filep)
{
    return IFX_SUCCESS;
}

static int ifx_ledc_release(struct inode *inode, struct file *filelp)
{
    return IFX_SUCCESS;
}

static int ifx_ledc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;

    //  check magic number
    if ( _IOC_TYPE(cmd) != IFX_LEDC_IOC_MAGIC )
        return IFX_ERROR;

    //  check read/write right
    if ( ((_IOC_DIR(cmd) & _IOC_WRITE) && !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd)))
        || ((_IOC_DIR (cmd) & _IOC_READ) && !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd))) )
        return IFX_ERROR;

    switch (cmd) {
    case IFX_LEDC_IOC_VERSION:
        {
            struct ifx_ledc_ioctl_version version = {
                .major = IFX_LEDC_VER_MAJOR,
                .mid   = IFX_LEDC_VER_MID,
                .minor = IFX_LEDC_VER_MINOR
            };
            ret = copy_to_user((void *)arg, (void *)&version, sizeof(version));
        }
        break;
    case IFX_LEDC_IOC_SET_CONFIG:
        {
            struct ifx_ledc_config_param param;

            ret = copy_from_user((void *)&param, (void *)arg, sizeof(param));
            if ( ret == IFX_SUCCESS )
                ret = ifx_ledc_config(&param);
        }
        break;
    default:
        ret = IFX_ERROR;
    }

    return ret;
}

/*
 *  Description:
 *    Update LEDs with data stored in register (Software Update).
 *  Input:
 *    none
 *  Output:
 *    int --- IFX_SUCCESS:  Success
 *            else:         Error Code
 */
static INLINE int update_led(void)
{
    unsigned long sys_flags;
    int i = 1000;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    for ( ; LED_CON0_SW_UPDATE && i > 0; i-- );   //  prevent conflict of two consecutive update
    LED_CON0_SW_UPDATE_SET(1);
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    return i == 0 ? IFX_ERROR : IFX_SUCCESS;
}

static INLINE int turn_on_ledc(void)
{
    unsigned long sys_flags;
    unsigned int f_ledc_on;
    int ret;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    f_ledc_on = g_f_ledc_on;
    g_f_ledc_on = 1;
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    if ( f_ledc_on == 0 ) {
        if ( (ret = ifx_gpio_register(IFX_GPIO_MODULE_LEDC)) != IFX_SUCCESS ) {
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            g_f_ledc_on = 0;
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
            return ret;
        }
        LEDC_PMU_SETUP(IFX_PMU_ENABLE);
    }

    return IFX_SUCCESS;
}

static INLINE void turn_off_ledc(void)
{
    unsigned long sys_flags;
    unsigned int f_ledc_on;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    f_ledc_on = g_f_ledc_on;
    g_f_ledc_on = 0;
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    if ( f_ledc_on != 0 ) {
        LEDC_PMU_SETUP(IFX_PMU_DISABLE);
        ifx_gpio_deregister(IFX_GPIO_MODULE_LEDC);

        release_gpt(LEDC_GPT_SRC_TIMER);
    }
}

static INLINE int setup_gpt(int timer, unsigned long freq)
{
    unsigned long sys_flags;
    unsigned long gpt_freq;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    gpt_freq = g_gpt_freq;
    g_gpt_freq = freq;
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    if ( gpt_freq == freq )
        return IFX_SUCCESS;

    if ( gpt_freq == 0 ) {
        int ret;

        unsigned long divider = (ifx_get_fpi_hz() / freq) * (10 / 2);

#ifdef CONFIG_DANUBE
        if ( divider > 0xFFFF )
            divider = 0xFFFF;
#endif

        if ( divider < 0x0200 )
            divider = 0x0200;

        ret = ifx_gptu_timer_request(timer,
                                       TIMER_FLAG_SYNC
                                     | ((divider & 0xFFFF0000) != 0 ? TIMER_FLAG_32BIT : TIMER_FLAG_16BIT)
                                     | TIMER_FLAG_INT_SRC
                                     | TIMER_FLAG_CYCLIC | TIMER_FLAG_TIMER | TIMER_FLAG_DOWN
                                     | TIMER_FLAG_RISE_EDGE
                                     | TIMER_FLAG_CALLBACK_IN_IRQ,
                                     divider,
                                     0,
                                     0);
        if ( ret == IFX_SUCCESS )
            ifx_gptu_timer_start(timer, 0);
        else {
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            g_gpt_freq = 0;
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
            ifx_gptu_timer_free(timer);
            return IFX_ERROR;
        }
    }

    return IFX_SUCCESS;
}

static INLINE void release_gpt(int timer)
{
    unsigned long sys_flags;
    unsigned long gpt_freq;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    gpt_freq = g_gpt_freq;
    g_gpt_freq = 0;
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    if ( gpt_freq != 0 ) {
        ifx_gptu_timer_free(timer);
    }
}

static INLINE void proc_file_create(void)
{
    g_ledc_dir = proc_mkdir("driver/ifx_ledc", NULL);

    create_proc_read_entry("version",
                            0,
                            g_ledc_dir,
                            proc_read_version,
                            NULL);
}

static INLINE void proc_file_delete(void)
{
    remove_proc_entry("version", g_ledc_dir);

    remove_proc_entry("driver/ifx_ledc", NULL);
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_ledc_version(buf + len);

    if ( offset >= len ) {
        *start = buf;
        *eof = 1;
        return 0;
    }
    *start = buf + offset;
    if ( (len -= offset) > count )
        return count;
    *eof = 1;
    return len;
}

static INLINE int ifx_ledc_version(char *buf)
{
    return ifx_drv_ver(buf, "LED Controller", IFX_LEDC_VER_MAJOR, IFX_LEDC_VER_MID, IFX_LEDC_VER_MINOR);
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*!
  \fn       int ifx_ledc_set_blink(unsigned int led, unsigned int blink)
  \brief    Enable/Disable blink of LED.

            User uses this function to enable/disable blink mode of given LED.

  \param    led     - unsigned int, ID of LED
  \param    blink   - unsigned int, 0: blink mode off, 1: blink mode on
  \return   IFX_SUCCESS     Operation succeed.
  \return   IFX_ERROR       Operation fail.
  \ingroup  IFX_LEDC_API
 */
int ifx_ledc_set_blink(unsigned int led, unsigned int blink)
{
    unsigned long sys_flags;

    if ( led >= IFX_LEDC_MAX_LED )
        return IFX_ERROR;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    LED_CON0_LEDn_BLINK_SET(led, blink);
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_ledc_set_blink);

/*!
  \fn       int ifx_ledc_set_data(unsigned int led, unsigned int data)
  \brief    Turn on/off LED.

            User uses this function to turn on/off given LED.

  \param    led     - unsigned int, ID of LED
  \param    data    - unsigned int, 0: off, 1: on
  \return   IFX_SUCCESS     Operation succeed.
  \return   IFX_ERROR       Operation fail.
  \ingroup  IFX_LEDC_API
 */
int ifx_ledc_set_data(unsigned int led, unsigned int data)
{
    unsigned long sys_flags;

    if ( led >= IFX_LEDC_MAX_LED )
        return IFX_ERROR;

    spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
    LED_CPU0_LEDn_ON_SET(led, data);
    spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);

    if ( LED_CON1_UPDATE_SRC == 0x00 )
        return update_led();
    else
        return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_ledc_set_data);

/*
 *  Description:
 *    Config LED controller.
 *  Input:
 *    param   --- struct led_config_param*, the members are listed below:
 *                  operation_mask         - Select operations to be performed
 *                  led                    - LED to change update source
 *                  source                 - Corresponding update source
 *                  blink_mask             - LEDs to set blink mode
 *                  blink                  - Set to blink mode or normal mode
 *                  update_clock           - Select the source of update clock
 *                  fpid                   - If FPI is the source of update clock, set the divider
 *                  store_mode             - Set clock mode or single pulse mode for store signal
 *                  fpis                   - If FPI is the source of shift clock, set the divider
 *                  data_offset            - Set cycles to be inserted before data is transmitted
 *                  number_of_enabled_led  - Total number of LED to be enabled
 *                  data_mask              - LEDs to set value
 *                  data                   - Corresponding value
 *                  mips0_access_mask      - LEDs to set access right
 *                  mips0_access;          - 1: the corresponding data is output from MIPS0, 0: MIPS1
 *                  f_data_clock_on_rising - 1: data clock on rising edge, 0: data clock on falling edge
 *  Output:
 *    int    --- IFX_SUCCESS:   Success
 *               IFX_ERROR:     Fail
 */
/*!
  \fn       int ifx_ledc_config(struct ifx_ledc_config_param *param)
  \brief    Config LED controller.

            User uses this function to configure LED controller.

  \param    param --- struct led_config_param *, input the config options.
  \return   IFX_SUCCESS     Operation succeed.
  \return   IFX_ERROR       Operation fail.
  \ingroup  IFX_LEDC_API
 */
int ifx_ledc_config(struct ifx_ledc_config_param *param)
{
    unsigned long sys_flags;
    unsigned long mask;
    int i;

    if ( param == NULL ) {
        dbg("param == NULL");
        return IFX_ERROR;
    }

    if ( (param->operation_mask & IFX_LEDC_CFG_OP_NUMBER_OF_LED) && param->number_of_enabled_led > 0 && param->number_of_enabled_led <= IFX_LEDC_MAX_LED ) {
        if ( turn_on_ledc() != IFX_SUCCESS ) {
            dbg("turn_on_ledc fail");
            return IFX_ERROR;
        }
    }

    if ( g_f_ledc_on == 0 ) {
        dbg("ledc turned off");
        return IFX_ERROR;
    }

    /*  ADSL or LED */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_UPDATE_SOURCE) ) {
        mask = param->source_mask;
        while ( (i = clz(mask)) >= 0 ) {
            if ( i >= LED_CON0_EXT_SRC_MAX ) {
                dbg("source_mask exceed range: 0x%08lx", param->source_mask);
                return IFX_ERROR;
            }
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CON0_EXT_SRCn_SET(i, param->source & (1 << i));
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
            mask ^= 1 << i;
        }
    }

    /*  Blink   */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_BLINK) ) {
        mask = param->blink_mask;
        while ( (i = clz(mask)) >= 0 ) {
            if ( i >= IFX_LEDC_MAX_LED ) {
                dbg("blink_mask exceed range: 0x%08lx", param->blink_mask);
                return IFX_ERROR;
            }
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CON0_LEDn_BLINK_SET(i, param->blink & (1 << i));
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
            mask ^= 1 << i;
        }
    }

    /*  Edge    */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_DATA_CLOCK_EDGE) ) {
        spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
        LED_CON0_FALLING_EDGE_SET(param->f_data_clock_on_rising ? 0 : 1);
        spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
    }

    /*  Update Clock    */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_UPDATE_CLOCK) ) {
        if ( param->update_clock > 0x02 ) {
            dbg("update_clock exceed range: %lu", param->update_clock);
            return IFX_ERROR;
        }

        switch ( param->update_clock ) {
            case LED_CON1_UPDATE_SRC_SOFTWARE:
                spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
                LED_CON1_UPDATE_SRC_SET(0);
                spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
                release_gpt(LEDC_GPT_SRC_TIMER);
                break;
            case LED_CON1_UPDATE_SRC_GPT:
                if ( setup_gpt(LEDC_GPT_SRC_TIMER, param->fpid) != IFX_SUCCESS ) { //  Timer 2B
                    dbg("setup_gpt fail");
                    return IFX_ERROR;
                }
                spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
                LED_CON1_UPDATE_SRC_SET(1);
                spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
                break;
            case LED_CON1_UPDATE_SRC_FPI:
                if ( param->fpid > 3 ) {
                    dbg("fpid exceed range: %lu", param->fpid);
                    return IFX_ERROR;
                }
                spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
#if defined(CONFIG_AMAZON_SE)
                IFX_REG_W32_MASK(0, 1 << 16, IFX_CGU_IF_CLK);
#endif
                LED_CON1_FPID_SET(param->fpid);
                LED_CON1_UPDATE_SRC_SET(2);
                spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
                release_gpt(LEDC_GPT_SRC_TIMER);
                break;
            default:
                dbg("update_clock not correct: %lu", param->update_clock);
                return IFX_ERROR;
        }
    }

    /*  Store Mode  */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_STORE_MODE) ) {
        spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
        LED_CON1_CLOCK_STORE_SET(param->store_mode);
        spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
    }

    /*  Shift Clock */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_SHIFT_CLOCK) ) {
        if ( param->fpis > 0x03 ) {
            dbg("fpis exceed range: %lu", param->fpis);
            return IFX_ERROR;
        }
        spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
        LED_CON1_FPIS_SET(param->fpis);
        spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
    }

    /*  Data Offset */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_DATA_OFFSET) ) {
        if ( param->data_offset > 0x03 ) {
            dbg("data_offset exceed range: %lu", param->data_offset);
            return IFX_ERROR;
        }
        spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
        LED_CON1_DATA_OFFSET_SET(param->data_offset);
        spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
    }

    /*  Number of LED   */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_NUMBER_OF_LED) ) {
        if ( param->number_of_enabled_led > IFX_LEDC_MAX_LED ) {
            dbg("number_of_enabled_led exceed range: 0x%08lx", param->number_of_enabled_led);
            return IFX_ERROR;
        }
        else if ( param->number_of_enabled_led > 16 ) {
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CON1_GROUP_SET(LED_CON1_GROUP2  | LED_CON1_GROUP1 | LED_CON1_GROUP0);
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
        } else if ( param->number_of_enabled_led > 8 ) {
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CON1_GROUP_SET(LED_CON1_GROUP1 | LED_CON1_GROUP0);
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
        } else if ( param->number_of_enabled_led > 0 ) {
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CON1_GROUP_SET(LED_CON1_GROUP0);
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
        } else {
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CON1_GROUP_SET(0);
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
        }
    }

    /*  Access Right    */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_MIPS0_ACCESS) ) {
        mask = param->mips0_access_mask;
        while ( (i = clz(mask)) >= 0 ) {
            if ( i >= IFX_LEDC_MAX_LED ) {
                dbg("mips0_access_mask exceed range: 0x%08lx", param->mips0_access_mask);
                return IFX_ERROR;
            }
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_MIPS1_ACCESS_LEDn_SET(i, ~param->mips0_access & (1 << i));
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
            mask ^= 1 << i;
        }
    }

    /*  LED Data    */
    if ( (param->operation_mask & IFX_LEDC_CFG_OP_DATA) ) {
        mask = param->data_mask;
        while ( (i = clz(mask)) >= 0 ) {
            if ( i >= IFX_LEDC_MAX_LED ) {
                dbg("data_mask exceed range: 0x%08lx", param->data_mask);
                return IFX_ERROR;
            }
            spin_lock_irqsave(&g_ledc_register_lock, sys_flags);
            LED_CPU0_LEDn_ON_SET(i, param->data & (1 << i));
            spin_unlock_irqrestore(&g_ledc_register_lock, sys_flags);
            mask ^= 1 << i;
        }
        if ( LED_CON1_UPDATE_SRC == 0x00 )
            update_led();
    }

    if ( (param->operation_mask & IFX_LEDC_CFG_OP_NUMBER_OF_LED) && param->number_of_enabled_led == 0 )
        turn_off_ledc();

    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_ledc_config);



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
    int ret;
    char ver_str[256];

    spin_lock_init(&g_ledc_register_lock);

    if ( (g_board_ledc_hw_config.operation_mask & IFX_LEDC_CFG_OP_NUMBER_OF_LED) && g_board_ledc_hw_config.number_of_enabled_led > 0 && g_board_ledc_hw_config.number_of_enabled_led <= IFX_LEDC_MAX_LED ) {
        ret =ifx_ledc_config(&g_board_ledc_hw_config);
        if ( ret != IFX_SUCCESS ) {
            err("LEDC board dependent init fail - %d", ret);
            return ret;
        }
    }

    /* register port device */
    ret = register_chrdev(IFX_LEDC_MAJOR, "ifx_ledc", &g_ledc_fops);
    if ( ret != 0 ) {
        turn_off_ledc();
        err("Can not register LEDC device - %d", ret);
        return ret;
    }

    proc_file_create();

    ifx_ledc_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    return IFX_SUCCESS;
}

static void __exit ifx_ledc_exit(void)
{
    proc_file_delete();

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    if ( unregister_chrdev(IFX_LEDC_MAJOR, "ifx_ledc") ) {
        err("Can not unregister LEDC device (major %d)!", IFX_LEDC_MAJOR);
    }
#else
    unregister_chrdev(IFX_LEDC_MAJOR, "ifx_ledc");
#endif

    turn_off_ledc();
}

module_init(ifx_ledc_init);
module_exit(ifx_ledc_exit);

