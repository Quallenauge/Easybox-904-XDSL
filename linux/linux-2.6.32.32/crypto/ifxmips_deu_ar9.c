/******************************************************************************
**
** FILE NAME    : ifxmips_deu_ar9.c
** PROJECT      : IFX UEIP
** MODULES      : DEU Module for AR9
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
  \file	ifxmips_deu_ar9.c
  \brief ifx deu board specific driver file for ar9
*/

/*! 
 \defgroup BOARD_SPECIFIC_FUNCTIONS IFX_BOARD_SPECIFIC_FUNCTIONS 
 \ingroup IFX_DEU
 \brief board specific functions
*/

/* Project header files */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <asm/io.h> //dma_cache_inv
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_regs.h>

#include "ifxmips_deu_dma.h"
#include "ifxmips_deu_ar9.h"

/* Function decleration */
void aes_chip_init (void);
void des_chip_init (void);
int deu_dma_init (void);
u32 endian_swap(u32 input);
u32* memory_alignment(const u8 *arg, u32 *buff_alloc, int in_out, int nbytes);
void aes_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes);
void des_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes);
void deu_dma_priv_init(void);
void __exit ifxdeu_fini_dma(void);

#define DES_3DES_START  IFX_DES_CON
#define AES_START       IFX_AES_CON
#define CLC_START	IFX_DEU_CLK

/* Variables */
#ifdef CONFIG_CRYPTO_DEV_DMA
u32 *aes_buff_in = NULL;
u32 *aes_buff_out = NULL;
u32 *des_buff_in = NULL;
u32 *des_buff_out = NULL;
_ifx_deu_device ifx_deu[1];
#endif /* CONFIG_CRYPTO_DEV_DMA */

u8 *g_dma_page_ptr = NULL;
u8 *g_dma_block = NULL;
u8 *g_dma_block2 = NULL;

deu_drv_priv_t deu_dma_priv;

#ifdef CONFIG_CRYPTO_DEV_DMA

void deu_dma_priv_init(void)
{
    DEU_WAKELIST_INIT(deu_dma_priv.deu_thread_wait);
    deu_dma_priv.deu_event_flags = 0;
    deu_dma_priv.des_event_flags = 0;
    deu_dma_priv.aes_event_flags = 0;
    deu_dma_priv.event_src = 0;
    deu_dma_priv.deu_rx_buf = NULL;
    deu_dma_priv.deu_rx_len = 0;
}


/*! \fn int deu_dma_init (void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief Initialize DMA for DEU usage. DMA specific registers are 
 *         intialized here, including a pointer to the device, memory 
 *  	    space for the device and DEU-DMA descriptors  
 *  \return -1: fail, 0: SUCCESS
*/

int deu_dma_init (void)
{
    volatile struct deu_dma_t *dma = (struct deu_dma_t *) IFX_DEU_DMA_CON;
    struct dma_device_info *dma_device = NULL;
    int i = 0;

    struct dma_device_info *deu_dma_device_ptr;

    // get one free page and share between g_dma_block and g_dma_block2
    printk("PAGE_SIZE = %ld\n", PAGE_SIZE);
    g_dma_page_ptr = (u8 *)__get_free_page(GFP_KERNEL); // need 16-byte alignment memory block
    g_dma_block = g_dma_page_ptr; // need 16-byte alignment memory block
    g_dma_block2 = (u8 *)(g_dma_page_ptr + (PAGE_SIZE >> 1)); // need 16-byte alignment memory block

#ifndef CONFIG_CRYPTO_DEV_PWR_SAVE_MODE   
    deu_dma_priv_init();
#endif

    deu_dma_device_ptr = dma_device_reserve ("DEU");
    if (!deu_dma_device_ptr) {
        printk ("DEU: reserve DMA fail!\n");
        return -1;
    }
    ifx_deu[0].dma_device = deu_dma_device_ptr;

    dma_device = deu_dma_device_ptr;
    dma_device->priv = &deu_dma_priv;
    dma_device->buffer_alloc = &deu_dma_buffer_alloc;
    dma_device->buffer_free = &deu_dma_buffer_free;
    dma_device->intr_handler = &deu_dma_intr_handler;

    dma_device->tx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_device->rx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_device->port_num = 1;
    dma_device->tx_burst_len = 2;
    dma_device->rx_burst_len = 2;
    dma_device->max_rx_chan_num = 1;
    dma_device->max_tx_chan_num = 1;
    dma_device->port_packet_drop_enable = 0;

    for (i = 0; i < dma_device->max_rx_chan_num; i++) {
        dma_device->rx_chan[i]->packet_size = DEU_MAX_PACKET_SIZE;
        dma_device->rx_chan[i]->desc_len = 1;
        dma_device->rx_chan[i]->control = IFX_DMA_CH_ON;
        dma_device->rx_chan[i]->byte_offset = 0;
    }

    for (i = 0; i < dma_device->max_tx_chan_num; i++) {
        dma_device->tx_chan[i]->control = IFX_DMA_CH_ON;
        dma_device->tx_chan[i]->desc_len = 1;
    }

    dma_device->current_tx_chan = 0;
    dma_device->current_rx_chan = 0;
   
    i = dma_device_register (dma_device);
    for (i = 0; i < dma_device->max_rx_chan_num; i++) {
        (dma_device->rx_chan[i])->open (dma_device->rx_chan[i]);
#if defined(CONFIG_CRYPTO_DEV_POLL_DMA)
        (dma_device->rx_chan[i])->disable_irq(dma_device->rx_chan[i]);
#endif
    } 

    dma->controlr.BS = 0;
    dma->controlr.RXCLS = 0;
    dma->controlr.EN = 1;

    return 0;
}

/*! \fn u32 *memory_alignment(const u8 *arg, u32 *buffer_alloc, int in_buff, int nbytes)  
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief  Not used for AR9
 *  \param arg Pointer to the input / output memory address
 *  \param buffer_alloc A pointer to the buffer 
 *  \param in_buff Input (if == 1) or Output (if == 0) buffer 
 *  \param nbytes Number of bytes of data   
*/

u32 *memory_alignment(const u8 *arg, u32 *buffer_alloc, int in_buff, int nbytes) 
{
   return (u32 *) arg;
}

/*! \fn void aes_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes) 
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief copy the DMA data to the memory address space for AES 
 *  \param outcopy Not used
 *  \param out_dma A pointer to the memory address that stores the DMA data
 *  \param out_arg The pointer to the memory address that needs to be copied to]
 *  \param nbytes Number of bytes of data   
*/

void aes_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes) 
{
    memcpy (out_arg, out_dma, nbytes);       
}

/*! \fn void des_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes) 
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief copy the DMA data to the memory address space for DES 
 *  \param outcopy Not used
 *  \param out_dma A pointer to the memory address that stores the DMA data
 *  \param out_arg The pointer to the memory address that needs to be copied to]
 *  \param nbytes Number of bytes of data   
*/

void des_dma_memory_copy(u32 *outcopy, u32 *out_dma, u8 *out_arg, int nbytes) 
{
    memcpy (out_arg, out_dma, nbytes);       
}

/*! \fn __exit ifxdeu_fini_dma(void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief unregister dma devices after exit
*/  

void __exit ifxdeu_fini_dma(void)
{
    if (g_dma_page_ptr)
        free_page((u32) g_dma_page_ptr);
    dma_device_release(ifx_deu[0].dma_device);
    dma_device_unregister(ifx_deu[0].dma_device); 

}

#endif /* CONFIG_CRYPTO_DEV_DMA */

/*! \fn u32 endian_swap(u32 input) 
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief Swap data given to the function 
 *  \param input   Data input to be swapped
 *  \return either the swapped data or the input data depending on whether it is in DMA mode or FPI mode
*/
u32 endian_swap(u32 input)
{
#ifdef CONFIG_CRYPTO_DEV_DMA_AR9
    u8 *ptr = (u8 *)&input; 
    return ((ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0]);     
#else
    return input;
#endif
}

/*! \fn	u32 input_swap(u32 input)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief Not used  
 *  \return input
*/

u32 input_swap(u32 input)
{
    return input;
}

/*! \fn void aes_chip_init (void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief initialize AES hardware   
*/

void aes_chip_init (void)
{
   volatile struct aes_t *aes = (struct aes_t *) AES_START;

   aes->controlr.SM = 1;
#ifndef CONFIG_CRYPTO_DEV_DMA_AR9
   aes->controlr.ARS = 1;
#else
   aes->controlr.NDC = 1; /* to write to ENDI */
   asm("sync"); 
   aes->controlr.ENDI = 0;
   asm("sync");
   aes->controlr.ARS = 0; // 0 for dma
   asm("sync");
#endif

}

/*! \fn void des_chip_init (void)
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief initialize DES hardware
*/         
                        
void des_chip_init (void)
{
    volatile struct des_t *des = (struct des_t *) DES_3DES_START;

#ifndef CONFIG_CRYPTO_DEV_DMA_AR9
    // start crypto engine with write to ILR
    des->controlr.SM = 1;
    asm("sync");
    des->controlr.ARS = 1;

#else
    des->controlr.SM = 1;
    des->controlr.NDC = 1;
    asm("sync");
    des->controlr.ENDI = 0;
    asm("sync");
    des->controlr.ARS = 0; // 0 for dma

#endif
}

/*! \fn void chip_version(void) 
 *  \ingroup BOARD_SPECIFIC_FUNCTIONS
 *  \brief not used!
*/     

void chip_version(void) 
{
    return;
}

