/******************************************************************************
**
** FILE NAME    : ifxmips_exin.c
** PROJECT      : CPE
** MODULES      : EXIN
**
** DATE         : 11 AUG 2011
** AUTHOR       : Andreas Schmidt
** DESCRIPTION  : External Interrupt (EXIN) Driver
** COPYRIGHT    :       Copyright (c) 2011
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
** 11 AUG 2011  A. Schmidt      Initiate Version
*******************************************************************************/


#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_interrupt.h"

#if 1
#define DEBUG(args...) do {;} while(0)
#else
#define DEBUG   printk
#endif

struct exin_dev_s {
	u32               exin_num;
	u32               exin_mode;
	u32               exin_open;
	u32               exin_irq;
	wait_queue_head_t exin_readq;
};


static struct exin_dev_s exin_dev[] =
{
	{ 0, 0, 0, IFX_EIU_IR0 },
	{ 1, 0, 0, IFX_EIU_IR1 },
	{ 2, 0, 0, IFX_EIU_IR2 },
#if defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
	{ 3, 0, 0, IFX_EIU_IR3 },
	{ 4, 0, 0, IFX_EIU_IR4 },
#endif
#if defined(CONFIG_AR9) || defined(CONFIG_VR9)
	{ 5, 0, 0, IFX_EIU_IR5 },
#endif
};
#define EXIN_COUNT (sizeof(exin_dev)/sizeof(struct exin_dev_s))

static int flag = 0;
static int maj = 244;
static char msg[] = "0\n\0";	/* For returning EXIN input level */
static char *msg_ptr;


static volatile ifx_eiu_icu_t bsp_eiu_icu_p = (volatile ifx_eiu_icu_t)IFX_ICU_EIU;


static irqreturn_t ifx_exin_int_handler(int irq, void *dev_id)
{
    struct exin_dev_s *dev = (struct exin_dev_s*) dev_id;

    if (dev->exin_mode & 0x4) {
        /* level triggered interrupt; prevent interrupt flood by disbabling it */
        disable_irq(irq);
    }
    DEBUG("process %i (%s) awakening the readers...\n",
            current->pid, current->comm);
    flag = 1;
    wake_up_interruptible(&(dev->exin_readq));

    return IRQ_HANDLED;
}


static int ifx_exin_open(struct inode *inode, struct file * filp)
{
    int num = MINOR(inode->i_rdev);
    int ret;

    DEBUG("ifx_exin_open is invoked with minor=%d\n", num);

    if (num >= EXIN_COUNT) 
        return -1;

    filp->f_pos=0;
    filp->private_data = (void*) &exin_dev[num];
    msg_ptr = msg;

    if (exin_dev[num].exin_open++ == 0) {
        ret = request_irq(exin_dev[num].exin_irq, ifx_exin_int_handler, IRQF_SHARED, "exin", &exin_dev[num]);
        if (ret < 0) {
            printk("Unable to register interrupt %d for the External Interrupts\n", exin_dev[num].exin_irq);
        }
//        bsp_eiu_icu_p->eiu_inc  |= (1 << num); /* clear pending interrupt */
        bsp_eiu_icu_p->eiu_inen |= (1 << num); /* enable interrupt */
    }


    return 0;
}

static int ifx_exin_close(struct inode *inode, struct file *filp)
{
    int num = MINOR(inode->i_rdev);

    DEBUG("ifx_exin_close is invoked with minor=%d\n", num);

    if (num >= EXIN_COUNT) 
        return -1;

    bsp_eiu_icu_p->eiu_inen &= ~(1 << num);

    if (--exin_dev[num].exin_open == 0) {
        free_irq(exin_dev[num].exin_irq, &exin_dev[num]);
    }

    return 0;
}
//extern void bsp_ack_irq(u32 irq);
//extern void ifx_icu_irsr_clr(u32 irq);
static ssize_t ifx_exin_read(struct file *filp, char *ubuf, size_t len, loff_t *off)
{
    struct exin_dev_s *dev = (struct exin_dev_s*) (filp->private_data);
    u32 num = dev->exin_num;
 
    if (dev->exin_mode != 0) {
        DEBUG("process %i (%s) going to sleep\n",
            current->pid, current->comm);
        wait_event_interruptible(dev->exin_readq, flag != 0);
        flag = 0;
        DEBUG("awoken %i (%s)\n", current->pid, current->comm);
    }
    else if (*msg_ptr != 0) {
        /* Return input level of the EXIN pin. This is done checking the interrupt 
           capture register for High level, without enabling the interrupt. This 
           way we avoid to check the (platform dependant) GPIO pin                */

//        bsp_eiu_icu_p->eiu_inic &= ~(1 << num); /* clear pending interrupt */
        //bsp_eiu_icu_p->eiu_inen &= ~(1 << num); /* disable interrupt */
        disable_irq(dev->exin_irq);
        bsp_eiu_icu_p->eiu_exin_c |= (5 << (4*num));
        if (bsp_eiu_icu_p->eiu_inic & (1 << num)) {
            *msg_ptr++ = '1';
        }
        else {
            *msg_ptr++ = '0';
        }
        *msg_ptr++ = '\n';
        bsp_eiu_icu_p->eiu_exin_c &= ~(0x7 << (4*num));

        /* clear pending interrupt */
        //bsp_ack_irq(dev->exin_irq); - not working as expected...
        //ifx_icu_irsr_clr(dev->exin_irq); - not working as expected...

        enable_irq(dev->exin_irq);
        copy_to_user(ubuf, msg, 2);

        return 2;
    }
    return 0; /* EOF */
}

static ssize_t ifx_exin_write(struct file *filp, const char *ubuf, size_t len, loff_t *off)
{
    struct exin_dev_s *dev = (struct exin_dev_s*) (filp->private_data);
    char buf[2] = {0};
    u32 mode, num;

    copy_from_user(buf, ubuf, 1);
    DEBUG("ifx_exin_write is invoked and the buf is %s\n", buf);
    
    mode = buf[0] - '0';
    if (mode > 0x7)
        return -1;

    dev->exin_mode = mode;
    num = dev->exin_num;
    bsp_eiu_icu_p->eiu_exin_c = (bsp_eiu_icu_p->eiu_exin_c & ~(0x7 << (4*num)))
                                 | (mode << (4*num));
    return len;
}

static struct file_operations exin_fops = {
        owner:          THIS_MODULE,
        read:           ifx_exin_read,    /* read */
        write:          ifx_exin_write,   /* write */
        open:           ifx_exin_open,    /* open */
        release:        ifx_exin_close,   /* release */
};


int __init ifx_exin_init(void)
{
    int i, ret;

    if ((ret = register_chrdev(maj, "ifx_exin", &exin_fops)) < 0){
            printk("Unable to register major %d for the External Interrupts\n", maj);
    }
    for ( i = 0; i < EXIN_COUNT; i++ ) {
        exin_dev[i].exin_num = i;
        init_waitqueue_head(&exin_dev[i].exin_readq);
    }

    return 0;
}

void __exit ifx_exin_cleanup_module(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    int ret;
    if((ret = unregister_chrdev(maj, "ifx_exin")) < 0) {
     	printk("Unable to un-register major %d for the External Interrupts\n", maj);
    }
#else
    unregister_chrdev(maj, "ifx_exin");
#endif
}

module_exit(ifx_exin_cleanup_module);
module_init(ifx_exin_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andreas Schmidt");
MODULE_DESCRIPTION("IFX external interrupt driver");
