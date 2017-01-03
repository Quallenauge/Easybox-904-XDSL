/******************************************************************************
**
** FILE NAME    : ifx_dcdc.c
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
#include "linux/autoconf.h"
#include "linux/module.h"
#include "linux/moduleparam.h"
#include "linux/types.h"
#include "linux/errno.h"
#include "linux/proc_fs.h"
#include "linux/ioctl.h"
#include "linux/init.h"
#include "linux/fs.h"
#include "linux/kernel.h"
#include "asm/uaccess.h"
#include "ifx_dcdc.h"

/*DCDC device pointer allocated by the init routine*/
DcDcDevt  *ifx_dcdc_dev = NULL;
extern Ifx_dcdc_platform_callbacks_table Ifx_dcdc_Platform_CallBack_table;
extern void ifx_dcdc_dump_regs(void);
extern  unsigned int SupportedVoltageLevels[];
/*Proc Dir entry for DCDC converter*/
struct proc_dir_entry *ifx_dcdc_proc_entry = NULL;

/* DCDC CONVERTER KERNEL APIs */
#if 0
Commenting out code for setting the POWER STATE as POWER STATE->VOLTAGE
mapping table is to be maitained by the CPU driver

/*==============================================================================
*  Function:        ifx_dcdc_power_state_set
*  Description :    function to adjust the core voltage with respect to the
*                   Desired power state
*  Params:
*      power_state :  one of the defined power state in coc
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_power_state_set ( int power_state )
{
    unsigned long flags;
    int err;

    spin_lock_irqsave(&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_voltage_set_cb )
          (power_state);
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to change the Power state",__FUNCTION__);
       return err;
    }
    ifx_dcdc_dev->Power_state = power_state;
    ifx_dcdc_dev->Current_voltage = power_state;

    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_power_state_set);
#endif
/*==============================================================================
*  Function:        ifx_dcdc_power_voltage_set
*  Description :    function to explicitly adjust the core voltage
*  Params:
*      voltage_value :  voltage value to be set
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_power_voltage_set ( int voltage_value)
{
    unsigned long flags;
    int err = 0;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_voltage_set_cb )(voltage_value);
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to change the voltage",__FUNCTION__);
       return err;
    }
    ifx_dcdc_dev->Current_voltage  =  (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_voltage_get_cb )();
    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_power_voltage_set);
#if 0
Commenting out the code for to query the current voltage as this now will be
supported by the PMCU driver

/*==============================================================================
*  Function:        ifx_dcdc_power_voltage_get
*  Description :    function to get the current core voltage
*  Params:
*      voltage_value :  pointer to retrieve the current voltage value
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_power_voltage_get ( int *voltage_value)
{
    unsigned long flags;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    *voltage_value =  ifx_dcdc_dev->Current_voltage;

    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_power_voltage_get);
#endif
/*==============================================================================
*  Function:        ifx_dcdc_duty_cycle_max_set
*  Description :    function to set the duty cycle maximum satuation
*  Params:
*      max_set_value :  Desired maximum satuation value
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_duty_cycle_max_set( int max_set_value)
{
    unsigned long flags;
    int err = 0;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_DutyCycleMax_set_cb )
          (max_set_value);
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to change the duty cycle max",__FUNCTION__);
       return err;
    }
    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_duty_cycle_max_set);
/*==============================================================================
*  Function:        ifx_dcdc_pwm_switching_freq_set
*  Description :    function to set the duty cycle maximum satuation
*  Params:
*      pwm_freq_value :  Desired pwm switching frequency
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_pwm_switching_freq_set( int pwm_freq_value)
{
    unsigned long flags;
    int err = 0;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_PwmSwitchingFreq_set_cb)
          (pwm_freq_value);
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to change the duty cycle max",__FUNCTION__);
       return err;
    }
    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_pwm_switching_freq_set);
/*==============================================================================
*  Function:        ifx_dcdc_non_ov_delay_set
*  Description :    function to set two overlap-time-delay values delay_p and
*                   delay_n respectly for PMOS turn off to the NMOS turn on and
*                   PMOS turn on to the NMOS turn off
*  Params:
*      non_ov_delay_value :  Two non-verlap time delay values
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_non_ov_delay_set( unsigned int Delay_p, unsigned int Delay_n)
{
    unsigned long flags;
    int err = 0;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_NonOvDelay_set_cb)
          (Delay_p, Delay_n);
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to change the NonOvDelay",__FUNCTION__);
       return err;
    }
    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_non_ov_delay_set);
/*==============================================================================
*  Function:        ifx_dcdc_pfm_mode_enable
*  Description :    function to enable the pfm mode
*  Params:
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_pfm_mode_enable(void)
{
    unsigned long flags;
    int err = 0;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_PfmModeEnable_cb)();
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to Enable the PFM Mode",__FUNCTION__);
       return err;
    }
    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_pfm_mode_enable);
/*==============================================================================
*  Function:        ifx_dcdc_pfm_mode_disable
*  Description :    function to disable the pfm mode
*  Params:
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
int ifx_dcdc_pfm_mode_disable(void)
{
    unsigned long flags;
    int err = 0;

    spin_lock_irqsave (&ifx_dcdc_dev->ifx_dcdc_lock, flags);

    err = (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_PfmModeDisable_cb)();
    if (err){
       spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
       printk("\%s:Failed to Disable the PFM Mode",__FUNCTION__);
       return err;
    }
    spin_unlock_irqrestore (&ifx_dcdc_dev->ifx_dcdc_lock, flags);
    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_dcdc_pfm_mode_disable);
/*==============================================================================
*  Function:        ifx_dcdc_read
*  Description :    DCDC converter proc read routine
*  Params:
*  Return Value:    Returns Number of bytes written
*===============================================================================
*/
static int ifx_dcdc_read(char *buf, char **start, off_t off, int count,
                         int *eof, void *data)
{
    int len = 0;
    int i =0;

    len = sprintf(buf, "\nDCDC Info:\nVersion:\t\t%-10s\nCurrent Voltage \t%d mV\n",
                         IFX_DCDC_DRV_VERSION,
                         ifx_dcdc_dev->Current_voltage);
    len += sprintf (buf + len, "Supported Voltage Level:\n");
    for(i=0;i < IFX_DCDC_MAX_VOLTAGE_LEVELS;i++)
        len += sprintf(buf + len, "%d  mV\n",SupportedVoltageLevels[i]);
    return len;
}
/*==============================================================================
*  Function:        ifx_dcdc_init_proc_entry
*  Description :    function to create the proc entry for DCDC converter
*  Params:
*  Return Value:    IFX_SUCCESS or -ENOMEM
*===============================================================================
*/
static int ifx_dcdc_init_proc_entry(void)
{

    ifx_dcdc_proc_entry = create_proc_entry ("dcdc", 0, NULL);
    if (ifx_dcdc_proc_entry == NULL){
        printk("\nError: %s failed to create the proc entry", __FUNCTION__);
        return -ENOMEM;
    }
    ifx_dcdc_proc_entry->read_proc = ifx_dcdc_read;
    return IFX_SUCCESS;
}
/*==============================================================================
*  Function:        ifx_dcdc_open
*  Description :    Open function for DCDC dev
*  Params:
*      inode :          inode pointer of DCDC dev
*      filep :          file poiter of DCDC dev
*  Return Value:    0 or error value
*===============================================================================
*/
static int ifx_dcdc_open (struct inode *inode, struct file *filep)
{
    IFX_DCDC_PRINT ("%s: Called\n", __FUNCTION__);
    return 0;
}
/*==============================================================================
*  Function:        ifx_dcdc_release
*  Description :    Release function for DCDC dev
*  Params:
*      inode :          inode pointer of DCDC dev
*      filep :          file poiter of DCDC dev
*  Return Value:    0 or error value
*===============================================================================
*/
static int ifx_dcdc_release (struct inode *inode, struct file *filep)
{
    IFX_DCDC_PRINT ("%s: Called\n", __FUNCTION__);
    return 0;
}
/*==============================================================================
*  Function:        ifx_dcdc_ioctl
*  Description :    ioctl function for DCDC dev
*  Params:
*      inode :          inode pointer of DCDC dev
*      filep :          file pointer of DCDC dev
*      cmd   :          ioctl command
*      arg   :          ioctl argument
*  Return Value:    0 or error value
*===============================================================================
*/
static int ifx_dcdc_ioctl (struct inode *inode, struct file *filep,
                          unsigned int cmd, unsigned long arg)
{
    IFX_DCDC_PRINT ("%s: Called\n",__FUNCTION__);

    switch (cmd){
        case IFX_DCDC_VERSION:
            /*Return DCDC driver version to user*/
             copy_to_user((char*)arg, IFX_DCDC_DRV_VERSION,
                           sizeof(IFX_DCDC_DRV_VERSION));
             break;
#if 0
Commenting out code as Power state is maintained by the CPU driver and DCDC driver
is not supposed to provide the power state query support
        case IFX_DCDC_PWSTATE_QUERY:
             /*Return the power state to user*/
             copy_to_user ((int*)arg, &ifx_dcdc_dev->Power_state, sizeof(int));
             break;
#endif
        case IFX_DCDC_VOLTAGE_QUERY:
             /*Return the voltage to the user*/
             copy_to_user((int *)arg,  &ifx_dcdc_dev->Current_voltage, sizeof(int));
             break;
        case IFX_DCDC_VOLTAGE_LEVEL_QUERY:
             /*Return the voltage level to user*/
             copy_to_user((int *)arg, &SupportedVoltageLevels, (sizeof(int) * IFX_DCDC_MAX_VOLTAGE_LEVELS));
             break;
    }
    ifx_dcdc_dump_regs ();
    return 0;
}
/*
* File operations struct of DCDC converter driver
*/
static struct file_operations ifx_dcdc_fops = {
    .owner     =  THIS_MODULE,
    .open      =  ifx_dcdc_open,
    .release   =  ifx_dcdc_release,
    .ioctl     =  ifx_dcdc_ioctl,
};

/*==============================================================================
*  Function:        ifx_dcdc_init
*  Description :    Init function of DCDC converter driver
*  Params:          void
*  Return Value:    0 or error value
*===============================================================================
*/
static int __init ifx_dcdc_init (void)
{
    int ret;

    ifx_dcdc_dev = (DcDcDevt *) kmalloc (sizeof (DcDcDevt), GFP_KERNEL);
    if (ifx_dcdc_dev == NULL){
        printk("%s: Failed to allocate mem for dcdc dev", __FUNCTION__);
        return -ENOMEM;
    }

    memset (ifx_dcdc_dev, 0, sizeof(DcDcDevt));
    memcpy (ifx_dcdc_dev->name, IFX_DCDC_DEV_NAME, IFX_DCDC_DEV_NAMSIZ);

    ifx_dcdc_dev->ifx_dcdc_lock = SPIN_LOCK_UNLOCKED;

    ret = register_chrdev(0, IFX_DCDC_DEV_NAME, &ifx_dcdc_fops);
    if (ret < 0){
        printk("%s: Dev registeration Failed", __FUNCTION__);
        return ret;
    }
    ifx_dcdc_dev->major_num = ret;

/*Create the proc entry for DCDC converter*/
    ifx_dcdc_init_proc_entry ();

/* Now read the current voltage from the DCDC converter*/
    ifx_dcdc_dev->Current_voltage =
                 (*Ifx_dcdc_Platform_CallBack_table.Ifx_dcdc_voltage_get_cb )();

    IFX_DCDC_PRINT ("IFX device registeration Successfull");
    return 0;
}
/*==============================================================================
*  Function:        ifx_dcdc_exit
*  Description :    Exit function of DCDC converter driver
*  Params:          void
*  Return Value:    None
*===============================================================================
*/
static void __exit ifx_dcdc_exit (void)
{
    unregister_chrdev (ifx_dcdc_dev->major_num, ifx_dcdc_dev->name);
    kfree (ifx_dcdc_dev);
}

MODULE_AUTHOR ("SAMEER AHMAD");
MODULE_DESCRIPTION ("DCDC CONVERTER DRIVER");
MODULE_LICENSE("GPL");
module_init(ifx_dcdc_init);
module_exit(ifx_dcdc_exit);
