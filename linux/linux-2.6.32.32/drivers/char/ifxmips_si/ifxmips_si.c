/******************************************************************************
**
** FILE NAME    : ifxmips_si.c
** PROJECT      : UEIP
** MODULES      : Serial In Controller
**
** DATE         : 26 Apr 2010
** AUTHOR       : Xu Liang
** DESCRIPTION  : Serial In Controller driver common source file
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
** $Date          $Author         $Comment
** Apr 26, 2010   Xu Liang        Init Version
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */

#define IFX_SI_VER_MAJOR                1
#define IFX_SI_VER_MID                  0
#define IFX_SI_VER_MINOR                3



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
#include <linux/cdev.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_gptu.h>
#include "ifxmips_si.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */

#if 0
// Abstraction for required GPIOs
// SIN_CE (GPIO 1) -> CE(ALow) chip enable
// SIN_SH (GPIO 44)-> CP Clock Input edge triggered
// SIN_ST (GPIO 45)-> PL(ALow) async parallel load
// SIN_D  (GPIO 47)-> Q7 serial out from the last stage
// gpio_num = 16*port + pin; port = gpio_num >> 4, pin = gpio_num % 16
#define IFX_GPIO_SIN_CE             1
#define IFX_GPIO_SIN_SH             44
#define IFX_GPIO_SIN_ST             45
#define IFX_GPIO_SIN_D              47
#define GET_GPIO_PORT(gpio_num)     (gpio_num >> 4)
#define GET_GPIO_PIN(gpio_num)      (gpio_num % 16)
#endif

#define SI_GPT_SRC_TIMER            TIMER3A



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  File Operations
 */
static int si_open(struct inode *inode, struct file *filep);
static int si_release(struct inode *inode, struct file *filelp);
static int si_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

/*
 *  Software Update Serial In
 */
static INLINE int update_si(void);

/*
 *  Turn On/Off Serial In Controller
 */
static INLINE int turn_on_si(void);
static INLINE void turn_off_si(void);

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
static int proc_read_reg(char *, char **, off_t, int, int *, void *);

/*
 *  Init Help Functions
 */
static INLINE int ifx_si_version(char *);

/*
 *  External Variable
 */
extern ifx_si_config_param_t g_board_si_hw_config;  //  defined in board specific C file



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

static DEFINE_SPINLOCK(g_si_reg_lock);
static int g_f_sw_update = 0;
static int g_f_si_on = 0;
static int g_gpt_freq = 0;

static int g_si_major;
static struct file_operations g_si_fops = {
    .owner      = THIS_MODULE,
    .open       = si_open,
    .release    = si_release,
    .ioctl      = si_ioctl,
};

static unsigned int g_dbg_enable = DBG_ENABLE_MASK_ERR;

static struct proc_dir_entry* g_proc_dir = NULL;



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static int si_open(struct inode *inode, struct file *filep)
{
    return 0;
}

static int si_release(struct inode *inode, struct file *filep)
{
    return 0;
}

static int si_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int retval = 0;

    if ( _IOC_TYPE(cmd) != IFX_SI_IOC_MAGIC || _IOC_NR(cmd) >= IFX_SI_IOC_MAXNR )
        return -ENOTTY;

    if ( ((_IOC_DIR(cmd) & _IOC_READ ) && !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd)))
        || ((_IOC_DIR(cmd) & _IOC_WRITE ) && !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd))) )
        return -EFAULT;

    switch (cmd) {
    case IFX_SI_IOC_VERSION:
        {
            ifx_si_ioctl_version_t version = {
                .major = IFX_SI_VER_MAJOR,
                .mid   = IFX_SI_VER_MID,
                .minor = IFX_SI_VER_MINOR
            };
            retval = copy_to_user((void *)arg, (void *)&version, sizeof(version));
        }
        break;
    case IFX_SI_IOC_TEST:
        {
#if 0
            dprintk("IOCTL: IFX_SI_IOC_TEST\n");
            dbgprint_si_regs();
            test_setUpdateSrc();
            test_setSWUBit();
            test_setSyncMode();
            test_setInputGroup();
            test_setSamplingFPICLKDiv();
            test_setSerialShiftClock();
            test_interrupts();
#endif
            retval = 0;
        }
        break;
    case IFX_SI_IOC_SET_CONFIG:
        {
            ifx_si_config_param_t param;

            retval = copy_from_user((void *)&param, (void *)arg, sizeof(ifx_si_config_param_t));
            if ( retval == 0 )
                retval = ifx_si_config(&param);
        }
        break;
    case IFX_SI_IOC_GET_SHIFTIN_DATA:
        {
            uint32_t data = ifx_si_get_data();
            retval = copy_to_user((char *)arg, &data, sizeof(uint32_t));
        }
        break;
    default:
        printk(KERN_ERR "IOCTL: Undefined IOCTL call!\n");
        retval = -EACCES;
        break;
    }
    return retval;
}

static INLINE int update_si(void)
{
    unsigned long sys_flags;
    int i = 1000;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    if ( g_f_sw_update ) {
        for ( ; (IFX_REG_R32(g_si_port_priv.reg.con) & CON_SW_UPDATE_MASK) && i > 0; i-- ); //  prevent conflict of two consecutive update
        IFX_REG_W32_MASK(CON_SW_UPDATE_MASK, CON_SW_UPDATE_VAL(1), g_si_port_priv.reg.con);
    }
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return i == 0 ? -EBUSY : 0;
}

static INLINE int turn_on_si(void)
{
#if 0
    int port, pin;
    //CE#
    port = GET_GPIO_PORT(IFX_GPIO_SIN_CE);
    pin = GET_GPIO_PIN(IFX_GPIO_SIN_CE);
    bsp_port_clear_output(port, pin, PORT_MODULE_SI);
    //PL# low pulse
    port = GET_GPIO_PORT(IFX_GPIO_SIN_ST);
    pin = GET_GPIO_PIN(IFX_GPIO_SIN_ST);
    bsp_port_set_output(port, pin, PORT_MODULE_SI);
    bsp_port_clear_output(port, pin, PORT_MODULE_SI);
    udelay(100); // now arbitrary, Todo: check with hw
    bsp_port_set_output(port, pin, PORT_MODULE_SI);
#endif

    unsigned long sys_flags;
    int ret = 0;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    if ( g_f_si_on == 0 ) {
        if ( (ret = ifx_gpio_register(IFX_GPIO_MODULE_SI)) != IFX_SUCCESS )
            ret = -EBUSY;
        else {
            LEDC_PMU_SETUP(IFX_PMU_ENABLE); //  shared between LEDC and SI
            g_f_si_on = 1;
            ret = 0;
        }
    }
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return ret;
}

static INLINE void turn_off_si(void)
{
#if 0
    int port, pin;
    //CE#
    port = GET_GPIO_PORT(IFX_GPIO_SIN_CE);
    pin = GET_GPIO_PIN(IFX_GPIO_SIN_CE);
    bsp_port_set_output(port, pin, PORT_MODULE_SI);
#endif

    unsigned long sys_flags;
    unsigned int f_si_on;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    if ( (f_si_on = g_f_si_on) != 0 ) {
        ifx_gpio_deregister(IFX_GPIO_MODULE_SI);
        LEDC_PMU_SETUP(IFX_PMU_DISABLE);
        g_f_si_on = 0;
    }
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    if ( f_si_on )
        release_gpt(SI_GPT_SRC_TIMER);
}

static INLINE int setup_gpt(int timer, unsigned long freq)
{
    unsigned long sys_flags;
    unsigned long gpt_freq;
    unsigned long divider;
    int ret;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    gpt_freq = g_gpt_freq;
    g_gpt_freq = freq;
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    if ( gpt_freq == freq )
        return 0;
    else if ( gpt_freq != 0 )
        release_gpt(timer);

    divider = (ifx_get_fpi_hz() / freq) * (10 / 2);
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
    if ( ret == IFX_SUCCESS ) {
        ifx_gptu_timer_start(timer, 0);
        return 0;
    }
    else {
        release_gpt(timer);
        return -EBUSY;
    }
}

static INLINE void release_gpt(int timer)
{
    unsigned long sys_flags;
    unsigned long gpt_freq;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    gpt_freq = g_gpt_freq;
    g_gpt_freq = 0;
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    ifx_gptu_timer_free(timer);
}

static INLINE void proc_file_create(void)
{
    g_proc_dir = proc_mkdir("driver/ifx_si", NULL);

    create_proc_read_entry("version",
                            0,
                            g_proc_dir,
                            proc_read_version,
                            NULL);

    create_proc_read_entry("reg",
                            0,
                            g_proc_dir,
                            proc_read_reg,
                            NULL);
}

static INLINE void proc_file_delete(void)
{
    remove_proc_entry("version", g_proc_dir);
    remove_proc_entry("reg", g_proc_dir);
    remove_proc_entry("driver/ifx_si", NULL);
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_si_version(buf + len);
    len += sprintf(buf + len, "build: %s %s\n", __DATE__, __TIME__);
    len += sprintf(buf + len, "major.minor: %d.0\n", g_si_major);

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

static int proc_read_reg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "cpu     (0x%08x): 0x%08x\n", (unsigned int)g_si_port_priv.reg.cpu,    IFX_REG_R32(g_si_port_priv.reg.cpu));
    len += sprintf(page + off + len, "con     (0x%08x): 0x%08x\n", (unsigned int)g_si_port_priv.reg.con,    IFX_REG_R32(g_si_port_priv.reg.con));
    len += sprintf(page + off + len, "int_con (0x%08x): 0x%08x\n", (unsigned int)g_si_port_priv.reg.int_con,IFX_REG_R32(g_si_port_priv.reg.int_con));
    len += sprintf(page + off + len, "int_clr (0x%08x): 0x%08x\n", (unsigned int)g_si_port_priv.reg.int_clr,IFX_REG_R32(g_si_port_priv.reg.int_clr));
    len += sprintf(page + off + len, "dl_con  (0x%08x): 0x%08x\n", (unsigned int)g_si_port_priv.reg.dl_con, IFX_REG_R32(g_si_port_priv.reg.dl_con));

    *eof = 1;
    return len;
}

static INLINE int ifx_si_version(char *buf)
{
    return ifx_drv_ver(buf, "Serial In Controller", IFX_SI_VER_MAJOR, IFX_SI_VER_MID, IFX_SI_VER_MINOR);
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/**
 * Read the register value of shift-in data (SI_CPU[15:0]).
 * \param none
 * \return uint32_t 32-bit data in the register.
 */
unsigned int ifx_si_get_data()
{
    unsigned int ret;
    unsigned long sys_flags;

    update_si();

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    ret = IFX_REG_R32(g_si_port_priv.reg.cpu);
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return ret;
}
EXPORT_SYMBOL(ifx_si_get_data);

unsigned int ifx_si_get_bit(unsigned int bit)
{
    ASSERT(bit < 16, "parameter is out of range (0-15)");
    return (ifx_si_get_data() >> bit) & 0x01;
}
EXPORT_SYMBOL(ifx_si_get_bit);

int ifx_si_config(ifx_si_config_param_t *param)
{
    unsigned long sys_flags;
    int ret;

    if ( param == NULL ) {
        err("param == NULL");
        return -EINVAL;
    }

    if ( (param->operation_mask & IFX_SI_CFG_OP_UPDATE_GROUP) && param->input_group_type > 0 ) {
        if ( (ret = turn_on_si()) != 0 ) {
            err("turn_on_si fail");
            return ret;
        }
    }

    if ( g_f_si_on == 0 ) {
        err("Serial In Controller is turned off!");
        return -EIO;
    }

    if ( (param->operation_mask & IFX_SI_CFG_OP_UPDATE_SOURCE) ) {
        ASSERT(param->update_source_type < IFX_SI_UPD_SRC_ERR, "incorrect parameter update_source_type");
        switch ( param->update_source_type ) {
        case IFX_SI_UPD_SRC_SW:
            release_gpt(SI_GPT_SRC_TIMER);
            spin_lock_irqsave(&g_si_reg_lock, sys_flags);
            IFX_REG_W32_MASK(CON_US_MASK, CON_US_VAL(param->update_source_type), g_si_port_priv.reg.con);
            g_f_sw_update = 1;
            spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);
            break;
        case IFX_SI_UPD_SRC_GPT3:
            if ( setup_gpt(SI_GPT_SRC_TIMER, param->sampling_clk_div) != 0 ) {
                err("setup_gpt fail");
                return -EBUSY;
            }
            spin_lock_irqsave(&g_si_reg_lock, sys_flags);
            IFX_REG_W32_MASK(CON_US_MASK, CON_US_VAL(param->update_source_type), g_si_port_priv.reg.con);
            g_f_sw_update = 0;
            spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);
            break;
        case IFX_SI_UPD_SRC_FPID:
            if ( param->sampling_clk_div > 3 ) {
                err("sampling_clk_div excced range (0-3): %u", param->sampling_clk_div);
                return -EINVAL;
            }
            release_gpt(SI_GPT_SRC_TIMER);
            spin_lock_irqsave(&g_si_reg_lock, sys_flags);
            IFX_REG_W32_MASK(CON_US_MASK | CON_SAMPLING_CLK_MASK, CON_US_VAL(param->update_source_type) | CON_SAMPLING_CLK_VAL(param->sampling_clk_div), g_si_port_priv.reg.con);
            g_f_sw_update = 0;
            spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);
            break;
        default:
            err("update_source_type not correct: %u", param->update_source_type);
            return -EINVAL;
        }
    }

    if ( (param->operation_mask & IFX_SI_CFG_OP_UPDATE_MODE) ) {
        spin_lock_irqsave(&g_si_reg_lock, sys_flags);
        IFX_REG_W32_MASK(CON_SYNC_MASK, CON_SYNC_VAL(param->sync_mode), g_si_port_priv.reg.con);
        spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);
    }

    if ( (param->operation_mask & IFX_SI_CFG_OP_UPDATE_SHIFTCLKDIV) ) {
        if ( param->shift_in_clk_div > 3 ) {
            err("shift_in_clk_div excced range (0-3): %u", param->shift_in_clk_div);
            return -EINVAL;
        }
        spin_lock_irqsave(&g_si_reg_lock, sys_flags);
        IFX_REG_W32_MASK(CON_SHIFT_CLK_MASK, CON_SHIFT_CLK_VAL(param->shift_in_clk_div), g_si_port_priv.reg.con);
        spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);
    }

    if ( (param->operation_mask & IFX_SI_CFG_OP_UPDATE_GROUP) ) {
        spin_lock_irqsave(&g_si_reg_lock, sys_flags);
        IFX_REG_W32_MASK(CON_GROUP_MASK, CON_GROUP_VAL(param->input_group_type), g_si_port_priv.reg.con);
        spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

        if ( param->input_group_type == IFX_SI_GROUP_00 )
            turn_off_si();
    }

    return 0;
}
EXPORT_SYMBOL(ifx_si_config);

int ifx_si_irq_active_high(unsigned int bit, int active_high)
{
    unsigned long sys_flags;
    unsigned int clr, set;

    if ( bit > 15 )
        return -EINVAL;

    if ( active_high) {
        clr = 0;
        set = 1 << (bit + 16);
    }
    else {
        clr = 1 << (bit + 16);
        set = 0;
    }

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    IFX_REG_W32_MASK(clr, set, g_si_port_priv.reg.int_con);
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return 0;
}
EXPORT_SYMBOL(ifx_si_irq_active_high);

int ifx_si_irq_enable(unsigned int bit, int enable)
{
    unsigned long sys_flags;
    unsigned int clr, set;

    if ( bit > 15 )
        return -EINVAL;

    if ( enable) {
        clr = 0;
        set = 1 << bit;
    }
    else {
        clr = 1 << bit;
        set = 0;
    }

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    IFX_REG_W32_MASK(clr, set, g_si_port_priv.reg.int_con);
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return 0;
}
EXPORT_SYMBOL(ifx_si_irq_enable);

unsigned int ifx_si_irq_ier(void)
{
    unsigned int ret;
    unsigned long sys_flags;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    ret = IFX_REG_R32(g_si_port_priv.reg.int_con) & 0xFFFF;
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return ret;
}
EXPORT_SYMBOL(ifx_si_irq_ier);

unsigned int ifx_si_irq_isr(void)
{
    unsigned int ret;
    unsigned long sys_flags;

    spin_lock_irqsave(&g_si_reg_lock, sys_flags);
    ret = IFX_REG_R32(g_si_port_priv.reg.cpu) & IFX_REG_R32(g_si_port_priv.reg.int_con) & 0xFFFF;
    spin_unlock_irqrestore(&g_si_reg_lock, sys_flags);

    return ret;
}
EXPORT_SYMBOL(ifx_si_irq_isr);



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

static int __devinit si_init(void)
{
    int ret;
    char ver_str[256];

    if ( (g_board_si_hw_config.operation_mask & IFX_SI_CFG_OP_UPDATE_GROUP) ) {
        ret = ifx_si_config(&g_board_si_hw_config);
        if ( ret != 0 ) {
            err("SI board dependent init fail - %d", ret);
            return ret;
        }
    }

    ret = register_chrdev(IFX_SI_MAJOR, "ifx_si", &g_si_fops);
#if IFX_SI_MAJOR == 0
    g_si_major = ret;
#else
    g_si_major = IFX_SI_MAJOR;
#endif
    if ( ret < 0 ) {
        turn_off_si();
        err("Can not register SI device - %d", ret);
        return ret;
    }

    proc_file_create();

    ifx_si_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    return 0;
}


static void __exit si_exit(void)
{
    proc_file_delete();

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    if ( unregister_chrdev(g_si_major, "ifx_si") ) {
        err("Can not unregister SI device (major %d)!", g_si_major);
    }
#else
    unregister_chrdev(g_si_major, "ifx_si");
#endif

    turn_off_si();
}

module_init(si_init);
module_exit(si_exit);

MODULE_AUTHOR("Xu Liang");
MODULE_LICENSE("GPL");
