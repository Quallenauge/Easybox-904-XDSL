/******************************************************************************
**
** FILE NAME    : ifxmips_eeprom.c
** PROJECT      : IFX UEIP
** MODULES      : EEPROM
**
** DESCRIPTION  : X25040 EEPROM Driver
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
** $Version $Date          $Author        $Comment
** 0.0.1    06 July,2009  Lei Chuanhua    First UEIP release
*******************************************************************************/

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/major.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <asm/uaccess.h>

/* Project header */
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_ssc.h>

#define IFX_EEPROM_VER_MAJOR        1
#define IFX_EEPROM_VER_MID          1
#define IFX_EEPROM_VER_MINOR        1

#define IFX_EEPROM_EMSG(fmt,args...) printk("%s:" fmt, __func__, ##args)
//#define IFX_EEPROM_DBG
#ifdef IFX_EEPROM_DBG
#define IFX_EEPROM_DMSG(fmt,args...) printk("%s:" fmt, __func__, ##args)
#define INLINE 
#else
#define IFX_EEPROM_DMSG(fmt,args...)
#define INLINE inline 
#endif

static IFX_SSC_HANDLE *eeprom_handler;

/* allow the user to set the major device number 60 ~ 63 for experimental */
static int maj = 60;

/* commands for EEPROM, x25160, x25140 */
#define IFX_EEPROM_WREN         ((u8)0x06)
#define IFX_EEPROM_WRDI         ((u8)0x04)
#define IFX_EEPROM_RDSR         ((u8)0x05)
#define IFX_EEPROM_WRSR         ((u8)0x01)
#define IFX_EEPROM_READ         ((u8)0x03)
#define IFX_EEPROM_WRITE        ((u8)0x02)
#define IFX_EEPROM_PAGE_SIZE          4
#define IFX_EEPROM_SIZE               512
#define IFX_EEPROM_DETECT_COUNTER     100000

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,52))
MODULE_PARM (maj, "i");
#else
#include <linux/moduleparam.h>
module_param(maj, int, 0600);
#endif 
MODULE_PARM_DESC (maj, "EEPROM major device number");

static int
ifx_eeprom_rdsr (char *status)
{
    int ret = 0;
    u8 cmd = IFX_EEPROM_RDSR;

    if ((ret = ifx_sscTxRx(eeprom_handler,(char *)&cmd, sizeof(u8), status, sizeof(u8))) != 2){
        IFX_EEPROM_EMSG("line %d ifx_sscTxRx fails %d\n", __LINE__, ret);
        return -1;
    }
    return 0;
}

static INLINE int
ifx_eeprom_wip_over(void)
{
    int ret = 0;
    u8 status;
    int count = 0;

    while (1) {
        ret = ifx_eeprom_rdsr(&status);
        IFX_EEPROM_DMSG("status %x \n", status);
        if (ret) {
            IFX_EEPROM_EMSG("read back status fails %d\n", ret);
            break;
        }
        else if ((status & 1) == 0) {
            break;
        }
        if (++count > IFX_EEPROM_DETECT_COUNTER) {
            IFX_EEPROM_EMSG("Detect counter out of range\n");
            ret = -1;
            break;
        }
    }
    return ret;
}

static int
ifx_eeprom_wren(void)
{
    int ret = 0;
    u8 cmd = IFX_EEPROM_WREN;

    if ((ret = ifx_sscTx(eeprom_handler,(char *)&cmd, sizeof(u8))) != 1){
        IFX_EEPROM_EMSG("line %d ifx_sscTx fails %d\n", __LINE__, ret);
        return ret;
    }
    ifx_eeprom_wip_over();
    return 0; 
}

static int
ifx_eeprom_wrsr(void)
{
    int ret = 0;
    u8 cmd[2];

    cmd[0] = IFX_EEPROM_WRSR;
    cmd[1] = 0;
    
    if ((ret = ifx_eeprom_wren())) {
        IFX_EEPROM_EMSG ("ifx_eeprom_wren fails\n");
        return ret;
    }
    if ((ret = ifx_sscTx(eeprom_handler,(char *)&cmd, sizeof(cmd))) != 2){
        IFX_EEPROM_EMSG("line %d ifx_sscTx fails %d\n", __LINE__, ret);
        return ret;
    }
    ifx_eeprom_wip_over();
    return 0;
}

static int
eeprom_read(u32 addr, unsigned char *buf, u32 len)
{
    int ret = 0;
    u8 write_buf[2];
    u32 eff = 0;
    int total = 0;

    while (1) {
        ifx_eeprom_wip_over ();
        eff = IFX_EEPROM_PAGE_SIZE - (addr % IFX_EEPROM_PAGE_SIZE);
        eff = (eff < len) ? eff : len;
        write_buf[0] = IFX_EEPROM_READ | ((u8) ((addr & 0x100) >> 5));
        write_buf[1] = (addr & 0xff);
        if ((ret = ifx_sscTxRx(eeprom_handler, write_buf, 2, buf, eff)) != (eff + 2)) {
            IFX_EEPROM_EMSG("ifx_sscTxRx fails %d\n", ret);
            goto eeprom_read_err_out;
        }
        buf += eff;
        len -= eff;
        addr += eff;
        total += eff;
        if (len <= 0) {
            break;
        }
    }
eeprom_read_err_out:
    return total;
}

static int
eeprom_write(u32 addr, unsigned char *buf, u32 len)
{
    int ret = 0;
    u32 eff = 0;
    int start = 0;
    int total = 0;
    static u8 write_buf[IFX_EEPROM_SIZE] = {0}; /* XXX */

    while (1) {
        ifx_eeprom_wip_over();
        if ((ret = ifx_eeprom_wren())) {
            IFX_EEPROM_EMSG("ifx_eeprom_wren fails\n");
            return ret;
        }
        write_buf[0] = IFX_EEPROM_WRITE | ((u8) ((addr & 0x100) >> 5));
        write_buf[1] = (addr & 0xff);
        start = 2;
        
        eff = IFX_EEPROM_PAGE_SIZE - (addr % IFX_EEPROM_PAGE_SIZE);
        eff = (eff < len) ? eff : len;
        memcpy(write_buf + start, buf, eff);
        
        total = start + eff;
        if ((ret = ifx_sscTx(eeprom_handler, write_buf, total)) != total) {
            IFX_EEPROM_EMSG("ifx_sscTx fails %d\n", ret);
            return ret;
        }
        buf += eff;
        len -= eff;
        addr += eff;
        if (len <= 0)
            break;
    }
    return 0;
}

static INLINE int
ifx_eeprom_open (struct inode *inode, struct file *filp)
{
    filp->f_pos = 0;
    return 0;
}

static INLINE int
ifx_eeprom_close (struct inode *inode, struct file *filp)
{
    return 0;
}

static INLINE int
ifx_eeprom_ioctl (struct inode *inode, struct file *filp, unsigned int cmd,
             unsigned long data)
{
    return 0;
}

ssize_t
ifx_eeprom_kread (char *buf, size_t len, u32 addr)
{
    int ret = 0;

    if ((addr + len) > IFX_EEPROM_SIZE) {
        IFX_EEPROM_EMSG("invalid len\n");
        addr = 0;
        len = IFX_EEPROM_SIZE / 2;
    }

    if ((ret = ifx_eeprom_wrsr())) {
        IFX_EEPROM_EMSG("EEPROM reset fails\n");
        goto read_err_out;
    }

    if ((ret = eeprom_read (addr, buf, len)) != len) {
        IFX_EEPROM_EMSG("eeprom read fails\n");
        goto read_err_out;
    } 
read_err_out:
    return ret;
}
EXPORT_SYMBOL(ifx_eeprom_kread);

static ssize_t
ifx_eeprom_read (struct file *filp, char *ubuf, size_t len, loff_t * off)
{
    int ret = 0;
    u8 ssc_rx_buf[IFX_EEPROM_SIZE];
    
    if (*off >= IFX_EEPROM_SIZE)
        return 0;
    if (*off + len > IFX_EEPROM_SIZE)
        len = IFX_EEPROM_SIZE - *off;
    if (len == 0)
       return 0;
    if ((ret = ifx_eeprom_kread (ssc_rx_buf, len, *off)) < 0) {
        IFX_EEPROM_EMSG("read fails, err=%x\n", ret);
        return ret;
    }
    if (copy_to_user ((void *) ubuf, ssc_rx_buf, ret) != 0) {
        ret = -EFAULT;
    }
    *off += len;
    return len;
}

ssize_t
ifx_eeprom_kwrite (char *buf, size_t len, u32 addr)
{
    int ret = 0;

    if ((ret = ifx_eeprom_wrsr())) {
        IFX_EEPROM_EMSG("EEPROM reset fails\n");
        goto write_err_out;
    }
    if ((ret = eeprom_write(addr, buf, len))) {
        IFX_EEPROM_EMSG("eeprom write fails\n");
        goto write_err_out;
    }
write_err_out:
    return ret;
}
EXPORT_SYMBOL(ifx_eeprom_kwrite);

static ssize_t
ifx_eeprom_write (struct file *filp, const char *ubuf, size_t len,
             loff_t * off)
{
    int ret = 0;
    unsigned char ssc_tx_buf[IFX_EEPROM_SIZE];
    
    if (*off >= IFX_EEPROM_SIZE)
        return 0;
    if ((len + *off) > IFX_EEPROM_SIZE)
        len = IFX_EEPROM_SIZE - *off;

    if ((ret = copy_from_user (ssc_tx_buf, ubuf, len))) {
        return EFAULT;
    }
    ret = ifx_eeprom_kwrite (ssc_tx_buf, len, *off);
    if (ret > 0) {
        *off = ret;
    }
    return ret;
}

static loff_t
ifx_eeprom_llseek (struct file * filp, loff_t off, int whence)
{
    loff_t newpos;
    
    switch (whence) {
        case 0:     /*SEEK_SET */
            newpos = off;
            break;
        case 1:     /*SEEK_CUR */
            newpos = filp->f_pos + off;
            break;
        default:
            return -EINVAL;
    }
    if (newpos < 0)
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}


static struct 
file_operations ifx_eeprom_fops = {
    .owner   = THIS_MODULE,
    .llseek  = ifx_eeprom_llseek,
    .read    = ifx_eeprom_read,
    .write   = ifx_eeprom_write,
    .ioctl   = ifx_eeprom_ioctl,
    .open    = ifx_eeprom_open, 
    .release = ifx_eeprom_close,
};

#define IFX_EEPROM_MODE                IFX_SSC_MODE_0
#define IFX_EEPROM_PRIORITY            IFX_SSC_PRIO_LOW
#define IFX_EEPROM_FRAGSIZE            512
#define IFX_EEPROM_MAXFIFOSIZE         32
#define IFX_EEPROM_CS                  IFX_SSC_WHBGPOSTAT_OUT0_POS
#define IFX_EEPROM_NAME                "ifx_eeprom"
#ifdef  CONFIG_USE_EMULATOR
#define IFX_EEPROM_BAUDRATE            10000                       /*  10KHz */
#else
#define IFX_EEPROM_BAUDRATE            1000000                      /*  1 MHz */
#endif

static INLINE int 
ifx_eeprom_cs_handler(u32 csq, IFX_CS_DATA cs_data)
{
    if (csq == IFX_SSC_CS_ON) { /* Low active */
        return ifx_ssc_cs_low(cs_data);
    }
    else {
        return ifx_ssc_cs_high(cs_data);
    }
}

static INLINE IFX_SSC_HANDLE
ifx_eeprom_register(char *dev_name) 
{
    IFX_SSC_CONFIGURE_t ssc_cfg = {0};
    
    ssc_cfg.baudrate     = IFX_EEPROM_BAUDRATE;
    ssc_cfg.csset_cb     = ifx_eeprom_cs_handler;
    ssc_cfg.cs_data      = IFX_EEPROM_CS;
    ssc_cfg.fragSize     = IFX_EEPROM_FRAGSIZE;
    ssc_cfg.maxFIFOSize  = IFX_EEPROM_MAXFIFOSIZE;
    ssc_cfg.ssc_mode     = IFX_EEPROM_MODE;
    ssc_cfg.ssc_prio     = IFX_EEPROM_PRIORITY;
    return ifx_sscAllocConnection(dev_name, &ssc_cfg);
}

static INLINE void 
ifx_eeprom_gpio_init(void)
{
    ifx_gpio_register(IFX_GPIO_MODULE_SPI_EEPROM);
}

static INLINE void 
ifx_eeprom_gpio_release(void)
{   
    ifx_gpio_deregister(IFX_GPIO_MODULE_SPI_EEPROM);
}

static int __init
ifx_eeprom_init (void)
{
    int ret = 0;
    char ver_str[128] = {0};
    
    if ((ret = register_chrdev (maj, "eeprom", &ifx_eeprom_fops)) < 0) {
        printk ("Unable to register major %d for the Infineon Amazon EEPROM\n", maj);
        if (maj == 0) {
           goto errout;
        }
        else {
            maj = 0;
            if ((ret = register_chrdev (maj, "ssc",  &ifx_eeprom_fops)) < 0) {
                printk ("Unable to register major 0 for the Infineon Amazon EEPROM\n");
                goto errout;
            }
       }
    }
    if (maj == 0)
        maj = ret;
    eeprom_handler = ifx_eeprom_register(IFX_EEPROM_NAME);
    if (eeprom_handler == NULL) {
        printk("%s failed to register eeprom\n", __func__);
        unregister_chrdev (maj, "eeprom");
        return -ENOMEM;
    }
    ifx_eeprom_gpio_init();

    ifx_drv_ver(ver_str, "SPI EERPOM", IFX_EEPROM_VER_MAJOR, IFX_EEPROM_VER_MID, IFX_EEPROM_VER_MINOR);
    printk(KERN_INFO "%s", ver_str);
errout:
    return ret;
}

static void __exit
ifx_eeprom_exit (void)
{
    /* New kernel has no return value */
    unregister_chrdev(maj, "eeprom");

    ifx_sscFreeConnection(eeprom_handler); 
    ifx_eeprom_gpio_release();
}
module_init(ifx_eeprom_init);
module_exit(ifx_eeprom_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Chuanhua, Chuanhua.lei@infineon.com");
MODULE_DESCRIPTION("IFX EEPROM driver");
MODULE_SUPPORTED_DEVICE("ifx_eeprom");

#ifndef MODULE
static int __init
ifx_eeprom_set_maj (char *str)
{
    maj = simple_strtol (str, NULL, 0);
    return 1;
}
__setup ("eeprom_maj=", ifx_eeprom_set_maj);
#endif /* !MODULE */

