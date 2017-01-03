/******************************************************************************
**
** FILE NAME    : ifxmips_deu.c
** PROJECT      : IFX UEIP
** MODULES      : DEU Module for Danube
**
** DATE         : September 8, 2009
** AUTHOR       : Mohammad Firdaus
** DESCRIPTION  : Data Encryption Unit Driver
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
** $Date        $Author             $Comment
** 08,Sept 2009 Mohammad Firdaus    Initial UEIP release
*******************************************************************************/

/*!
  \defgroup IFX_DEU IFX_DEU_DRIVERS
  \ingroup API
  \brief ifx deu driver module
*/

/*!
  \file	ifxmips_deu.c
  \ingroup IFX_DEU
  \brief main deu driver file
*/

/*!
 \defgroup IFX_DEU_FUNCTIONS IFX_DEU_FUNCTIONS
 \ingroup IFX_DEU
 \brief IFX DEU functions
*/

/* Project header */
#include <linux/version.h>
#if defined(CONFIG_MODVERSIONS)
#define MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/crypto.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>       /* Stuff about file systems that we need */
#include <asm/byteorder.h>
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>
#include "ifxmips_deu.h"

#if defined(CONFIG_DANUBE)
#include "ifxmips_deu_danube.h"
#elif defined(CONFIG_AR9)
#include "ifxmips_deu_ar9.h"
#elif defined(CONFIG_VR9) || defined(CONFIG_AR10)
#include "ifxmips_deu_vr9.h"
#else 
#error "Platform unknown!"
#endif /* CONFIG_xxxx */

#ifndef CONFIG_CRYPTO_DEV_PWR_SAVE_MODE
#ifdef CONFIG_CRYPTO_DEV_DMA
int disable_deudma = 0;
#else
int disable_deudma = 1;
#endif /* CONFIG_CRYPTO_DEV_DMA */
#else
#ifdef CONFIG_CRYPTO_DEV_DMA
static int init_dma = 0;
void deu_dma_priv_init(void);
#endif
static u32 deu_power_flag = 0; 
spinlock_t pwr_lock;
void powerup_deu(int crypto);
void powerdown_deu(int crypto);
void aes_chip_init(void);
void des_chip_init(void);
#endif /* CONFIG_CRYPTO_DEV_PWR_SAVE_MODE */

void chip_version(void);

/*! \fn static int __init deu_init (void)
 *  \ingroup IFX_DEU_FUNCTIONS
 *  \brief link all modules that have been selected in kernel config for ifx hw crypto support   
 *  \return ret 
*/  
                               
static int __init deu_init (void)
{
    int ret = -ENOSYS;

#ifdef CONFIG_CRYPTO_DEV_PWR_SAVE_MODE
    unsigned long flag;
    printk("DEU is starting in Power Saving Mode\n");

    PWR_CRTCL_SECT_INIT;
    PWR_CRTCL_SECT_START;
 
    START_DEU_POWER;   
  
    PWR_CRTCL_SECT_END;

    /* Initialize the dma priv members 
     * have to do here otherwise the tests will fail
     * caused by exceptions. Only needed in DMA mode. */
#ifdef CONFIG_CRYPTO_DEV_DMA
    deu_dma_priv_init();
#endif

#else

    START_DEU_POWER;
    printk("Line: %d, function: %s\n", __LINE__, __func__);

#ifdef CONFIG_CRYPTO_DEV_DMA
    deu_dma_init ();
#endif

#endif /* CONFIG_CRYPTO_DEV_PWR_SAVE_MODE */

    
#define IFX_DEU_DRV_VERSION         "2.0.0"
         printk(KERN_INFO "Infineon Technologies DEU driver version %s \n", IFX_DEU_DRV_VERSION);

    FIND_DEU_CHIP_VERSION;

#if defined(CONFIG_CRYPTO_DEV_DES)
    if ((ret = ifxdeu_init_des ())) {
        printk (KERN_ERR "IFX DES initialization failed!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_AES)
    if ((ret = ifxdeu_init_aes ())) {
        printk (KERN_ERR "IFX AES initialization failed!\n");
    }

#endif
#if defined(CONFIG_CRYPTO_DEV_ARC4)
    if ((ret = ifxdeu_init_arc4 ())) {
        printk (KERN_ERR "IFX ARC4 initialization failed!\n");
    }

#endif
#if defined(CONFIG_CRYPTO_DEV_SHA1)
    if ((ret = ifxdeu_init_sha1 ())) {
        printk (KERN_ERR "IFX SHA1 initialization failed!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_MD5)
    if ((ret = ifxdeu_init_md5 ())) {
        printk (KERN_ERR "IFX MD5 initialization failed!\n");
    }

#endif
#if defined(CONFIG_CRYPTO_DEV_SHA1_HMAC)
    if ((ret = ifxdeu_init_sha1_hmac ())) {
        printk (KERN_ERR "IFX SHA1_HMAC initialization failed!\n");
    }
#endif
#if defined(CONFIG_CRYPTO_DEV_MD5_HMAC)
    if ((ret = ifxdeu_init_md5_hmac ())) {
        printk (KERN_ERR "IFX MD5_HMAC initialization failed!\n");
    }
#endif

#if defined(CONFIG_CRYPTO_ASYNC_AES)
   if ((ret = lqdeu_async_aes_init())) {
        printk (KERN_ERR "Lantiq async AES initialization failed!\n");
    }
#endif

#if defined(CONFIG_CRYPTO_ASYNC_DES)
   if ((ret = lqdeu_async_des_init())) {
        printk (KERN_ERR "Lantiq async AES initialization failed!\n");
    }
#endif

    return ret;

}

/*! \fn static void __exit deu_fini (void)
 *  \ingroup IFX_DEU_FUNCTIONS
 *  \brief remove the loaded crypto algorithms   
*/                                 
void __exit deu_fini (void)
{
//#ifdef CONFIG_CRYPTO_DEV_PWR_SAVE_MODE
    #if defined(CONFIG_CRYPTO_DEV_DES)
    ifxdeu_fini_des ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_AES)
    ifxdeu_fini_aes ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_ARC4)
    ifxdeu_fini_arc4 ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_SHA1)
    ifxdeu_fini_sha1 ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_MD5)
    ifxdeu_fini_md5 ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_SHA1_HMAC)
    ifxdeu_fini_sha1_hmac ();
    #endif
    #if defined(CONFIG_CRYPTO_DEV_MD5_HMAC)
    ifxdeu_fini_md5_hmac ();
    #endif
    printk("DEU has exited successfully\n");

    #if defined(CONFIG_CRYPTO_DEV_DMA)
    ifxdeu_fini_dma();
    printk("DMA has deregistered successfully\n");
    #endif

    #if defined(CONFIG_CRYPTO_ASYNC_AES)
    lqdeu_fini_async_aes();
    printk("Async AES has deregistered successfully\n");
    #endif
}


#ifndef CONFIG_CRYPTO_DEV_PWR_SAVE_MODE
int disable_multiblock = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
module_param(disable_multiblock,int,0);

#else
//MODULE_PARM (disable_multiblock, "i");
MODULE_PARM_DESC (disable_multiblock,
          "Disable encryption of whole multiblock buffers.");
#endif

#else 

/*! \fn void powerup_deu(int crypto)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief to power up the chosen cryptography module
 *  \param crypto AES_INIT, DES_INIT, etc. are the cryptographic algorithms chosen
 *  \return void
*/

void powerup_deu(int crypto)
{
    u32 temp;
    unsigned long flag;
    PWR_CRTCL_SECT_START;
    temp = 0;

#ifdef CONFIG_CRYPTO_DEV_DMA
    if (!init_dma)
    {
        deu_dma_init();
        init_dma = 1;
    }
#endif

    temp = 1 << crypto;

    switch (crypto)
    {
#if  defined(CONFIG_CRYPTO_DEV_AES) || defined(CONFIG_ASYNC_AES)
        case AES_INIT:
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            aes_chip_init();
            break;
#endif
#if defined(CONFIG_CRYPTO_DEV_DES) || defined(CONFIG_ASYNC_DES)
        case DES_INIT:
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            des_chip_init();
            break;
#endif
#ifdef CONFIG_CRYPTO_DEV_SHA1
        case SHA1_INIT:
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            break;
#endif
#ifdef CONFIG_CRYPTO_DEV_MD5
        case MD5_INIT:
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            break;
#endif
#ifdef CONFIG_CRYPTO_DEV_ARC4
        case ARC4_INIT:
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            break;
                
#endif
#ifdef CONFIG_CRYPTO_DEV_MD5_HMAC
        case MD5_HMAC_INIT:  
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            break;
#endif
#ifdef CONFIG_CRYPTO_DEV_SHA1_HMAC
       case SHA1_HMAC_INIT:
            if (!(deu_power_flag & temp))
            {
                START_DEU_POWER;
                deu_power_flag |= temp;
            }
            break;
#endif
        default:
            printk("Error finding initialization crypto\n");
            break;
    }

    PWR_CRTCL_SECT_END;
}

/*! \fn void powerdown_deu(int crypto)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief to power down the deu module totally if there are no other DEU encrypt/decrypt/hash
           processes still in progress. If there is, do not power down the module.
 *  \param crypto defines the cryptographic algo to module to be switched off
 *  \return void
*/

void powerdown_deu(int crypto)
{

    u32 temp;
    unsigned long flag;
    PWR_CRTCL_SECT_START;
    
    temp = 0;
    
    temp = 1 << crypto;
    if ((deu_power_flag &= ~temp))
    { 
        printk("DEU power down not permitted. DEU modules still in use! \n");
        PWR_CRTCL_SECT_END;
        return;
    }
     
    STOP_DEU_POWER;    
    //printk("DEU Module is turned OFF.\n");
    PWR_CRTCL_SECT_END;
}
#endif /* CONFIG_CRYPTO_DEV_PWR_SAVE_MODE */

module_init (deu_init);
module_exit (deu_fini);

MODULE_DESCRIPTION ("Infineon DEU crypto engine support.");
MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Mohammad Firdaus");

