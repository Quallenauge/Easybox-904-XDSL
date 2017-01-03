/******************************************************************************
**
** FILE NAME    : ifxmips_pmon.c
** PROJECT      : IFX UEIP
** MODULES      : PMON
**
** DATE         : 21 July 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX Performance Monitor
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
** 21 July 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/
/*!
  \defgroup IFX_PMON_FUNCTIONS External APIs
  \ingroup IFX_PMON
  \brief IFX PMON driver external functions
*/

/*!
  \defgroup IFX_PMON_OS OS APIs
  \ingroup IFX_PMON
  \brief IFX PMON driver os interface functions
*/

/*!
  \defgroup IFX_PMON_INTERNAL Internal functions
  \ingroup IFX_PMON
  \brief IFX PMON driver internal functions
*/

/*!
  \file ifxmips_pmon.c
  \ingroup IFX_PMON
  \brief IFX PMON driver source file
*/
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include<linux/module.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include <linux/miscdevice.h>
#include<linux/fs.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_pmon.h"

//#define IFX_PMON_DBG

#if defined(IFX_PMON_DBG)
#define IFX_PMON_PRINT(format, arg...)   \
    printk(format, ##arg)
#define INLINE 
#else
#define IFX_PMON_PRINT(format, arg...)   \
    do {} while(0)
#define INLINE inline
#endif

#define IFX_PMON_VER_MAJOR          1
#define IFX_PMON_VER_MID            1
#define IFX_PMON_VER_MINOR          1

static struct proc_dir_entry *ifx_pmon_proc;

static DECLARE_MUTEX(ifx_pmon_sem);

/**
 * \fn     static int ifx_pmon_set(int event, int tc, int perf_idx)
 *
 * \brief  This function is used to configure event and the corresponding counter
 *
 * \param  event         module id defined in ifx_pmon.h
 * \param  tc            MT TC index
 * \param  counter       counter index 0~1
 *
 * \return -EINVAL       Invalid event or performance counter
 * \return  0            OK
 * \ingroup  IFX_PMON_INTERNAL
 */
static int 
ifx_pmon_set(int event, int tc, int counter)
{
    int ret = 0;
    
    /* Sanity check first */
    if (tc < IFX_MIPS_TC0 || tc > IFX_MIPS_TC3) {
        ret = -EINVAL;
        goto done;
    }
    if (counter < IFX_PMON_XTC_COUNTER0 || counter > IFX_PMON_XTC_COUNTER1) {
        ret = -EINVAL;
        goto done;
    }
    
    IFX_PMON_PRINT("%s: event %d tc %d counter %d\n", __func__, event, tc, counter);
    /* XXX, A little over protected */
    down(&ifx_pmon_sem);   
    pmon_platform_event_set(event, tc, counter);
    up(&ifx_pmon_sem);   
done:
    return ret;
}

/**
 * \fn   static INLINE void ifx_pmon_reg_init(void) 
 *
 * \brief  This function is used to initialize PMON hardware registers
 *
 * \return none  
 * \ingroup  IFX_PMON_INTERNAL
 */

static INLINE void
ifx_pmon_reg_init(void) 
{
    pmon_platform_init();
}

/** 
 * \fn int  ifx_pmon_open(struct inode *inode, struct file *filp)
 * 
 * \param   inode Inode of device or 0/1 for calls from kernel mode 
 * \param   filp  File pointer of device 
 * \return  0     OK 
 * \ingroup  IFX_PMON_OS
 */
static INLINE int 
ifx_pmon_open(struct inode *inode, struct file *filp)
{
    return 0;
}

/** 
 * \fn int  ifx_pmon_release(struct inode *inode, struct file *filp)
 * \param   inode Inode of device or 0 or 1 for calls from kernel mode
 * \param   filp  File pointer of device 
 * \return  0     OK 
 * \ingroup IFX_PMON_OS
 */
static INLINE  int 
ifx_pmon_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/** 
 * \fn int  ifx_pmon_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
 *
 * \brief PMON IOCTL
 * \param   inode        Inode of device  
 * \param   filp         File structure of device 
 * \param   cmd          IOCTL command  
 * \param   arg          Argument for some IOCTL commands 
 *
 * \return  -EINVAL      Invalid IOCTL command
 * \return  -EACCES       Wrong access rights
 * \return  -ENOIOCTLCMD  No Ioctl command
 * \return  -EFAULT       Failed to get user data 
 * \return  0             Successful
 * \ingroup  IFX_PMON_OS
 */
static int 
ifx_pmon_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    
    /* Magic number */
    if ( _IOC_TYPE(cmd) != IFX_PMON_IOC_MAGIC ) {
        return -EINVAL;
    }

    /* Access rights */
    if ( ((_IOC_DIR(cmd) & _IOC_WRITE) && !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd)))
        || ((_IOC_DIR (cmd) & _IOC_READ) && !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd)))){
        return -EACCES;
    }

    switch (cmd) {
        case IFX_PMON_IOC_VERSION:
            {
                struct ifx_pmon_ioctl_version version = {
                    .major = IFX_PMON_VER_MAJOR,
                    .mid   = IFX_PMON_VER_MID,
                    .minor = IFX_PMON_VER_MINOR
                };
                ret = copy_to_user((void *)arg, (void *)&version, sizeof(version));
            }
            break;
            
        case IFX_PMON_IOC_EVENT:
            {
                struct ifx_pmon_ioctl_event event = {0};

                if (copy_from_user((void *)&event, (void *)arg, sizeof(event))) {
                    ret = -EFAULT;
                    goto done;
                }
                if (ifx_pmon_set(event.pmon_event, event.tc, event.counter)) {
                    ret = -EFAULT;
                    goto done;
                }
            }
            break;
            
        case IFX_PMON_IOC_DISABLE:
            {
                int disable = 0;
                
                if (__get_user(disable, (int*)arg)) {
                    ret = -EFAULT;
                    goto done;
                }
                if (disable) {
                    ifx_pmon_reg_init();
                }
            }
            break;
            
        default:
            ret = -ENOIOCTLCMD;
    }
done:
    return ret;
}

static struct file_operations ifx_pmon_fops = {
    .owner     = THIS_MODULE,
    .open      = ifx_pmon_open,
    .release   = ifx_pmon_release,
    .ioctl     = ifx_pmon_ioctl,
};

/**
 * \fn static inline int ifx_pmon_drv_ver(char *buf)
 * \brief Display PMON driver version after initilazation succeeds
 * 
 * \return number of bytes will be printed
 * \ingroup IFX_PMON_INTERNAL 
 */
static inline int 
ifx_pmon_drv_ver(char *buf)
{
    return ifx_drv_ver(buf, "PMON", IFX_PMON_VER_MAJOR, IFX_PMON_VER_MID, IFX_PMON_VER_MINOR);
}

/**
 * \fn    static inline int ifx_pmon_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
 *
 * \brief  This function is used read the proc entries used by PMON.
 *         Here, the proc entry is used to read version number
 *
 * \param  buf    Proc buffer
 * \param  start  start of the proc entry
 * \param  offset if set to zero, do not proceed to print proc data
 * \param  count  Maximum proc print size
 * \param  eof    end of the proc entry
 * \param  data   unused
 *
 * \return len    Lenght of data in buffer            
 *
 * \ingroup IFX_PMON_INTERNAL
 */
static inline int 
ifx_pmon_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    /* No sanity check cos length is smaller than one page */
    len += ifx_pmon_drv_ver(buf + len);
    *eof = 1;
    return len;    
} 

/**
 * \fn static int ifx_pmon_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
 *
 * \brief  This function is used read the proc entries used by PMON.
 *         Here, the proc entry is used to register status and module
 *         reference number.
 *
 * \param  buf    Proc buffer
 * \param  start  start of the proc entry
 * \param  offset if set to zero, do not proceed to print proc data
 * \param  count  Maximum proc print size
 * \param  eof    end of the proc entry
 * \param  data   unused
 *
 * \return Failure or success           
 *
 * \ingroup  IFX_PMON_INTERNAL
 */
static int 
ifx_pmon_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int i;
    int j;
    int k = 0;
    int len = 0;

    len += sprintf(buf + len, "PMON Register Dump\n");
    for (i = 0; i <= IFX_MIPS_MAX_TC; i++) { /* TC is index */
        for (j = 0; j < IFX_PMON_MAX_PERF_CNT_PER_TC; j++) {
            len += sprintf(buf + len, "PMON_PCEESCR%d(%p) : 0x%08x\n", 
                       k, IFX_PMON_PCEESCR(i, j), IFX_REG_R32(IFX_PMON_PCEESCR(i, j)));
            k++;
        }
    }    
    *eof = 1;
    return len;
}

static void 
ifx_pmon_proc_create(void)
{
    ifx_pmon_proc = proc_mkdir("driver/ifx_pmon", NULL);

    create_proc_read_entry("version",
                            0,
                            ifx_pmon_proc,
                            ifx_pmon_proc_version,
                            NULL);

    create_proc_read_entry("pmon",
                            0,
                            ifx_pmon_proc,
                            ifx_pmon_proc_read,
                            NULL);
}

static void 
ifx_pmon_proc_delete(void)
{
    remove_proc_entry("version", ifx_pmon_proc);

    remove_proc_entry("pmon", ifx_pmon_proc);

    remove_proc_entry("driver/ifx_pmon", NULL);
}

/**
 * \fn     static int __init ifx_pmon_init(void)
 *
 * \brief  This is module initialisation function for PMON
 *
 * \return =0 OK
 * \return <0 error number
 *
 * \ingroup IFX_PMON_OS
 */
static struct miscdevice ifx_pmon_miscdevice = {IFX_PMON_MISC_MINOR, "pmon", &ifx_pmon_fops};

static int __init 
ifx_pmon_init(void)
{
    int ret;
    char ver_str[128] = {0};

    ifx_pmon_reg_init();

    if ((ret = misc_register(&ifx_pmon_miscdevice)) < 0) {
        printk(KERN_ERR "%s: misc_register failed with %d\n", __func__, ret);
        return ret;
    }
    ifx_pmon_proc_create();
    ifx_pmon_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    return 0;
}

/**
 * \fn     static void __exit ifx_pmon_exit(void)
 *
 * \brief  This is module exit function for PMON
 *
 * \return None           
 *
 * \ingroup  IFX_PMON_OS
 */
static void __exit 
ifx_pmon_exit(void)
{
    int ret;

    if ((ret = misc_deregister(&ifx_pmon_miscdevice)) < 0) {
        printk(KERN_ERR "%s: misc_deregister failed with %d\n", __func__, ret);
    }
    ifx_pmon_proc_delete();
    ifx_pmon_reg_init();
}

module_init(ifx_pmon_init);
module_exit(ifx_pmon_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LeiChuanhua <Chuanhua.lei@infineon.com>");
MODULE_DESCRIPTION("IFX Performance Monitor Driver");
MODULE_SUPPORTED_DEVICE ("IFX builtin performance monitor");

