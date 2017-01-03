/**
** FILE NAME    : ifxmips_mmc.c
** PROJECT      : IFX UEIP
** MODULES      : MMC module 
** DATE         : 08 July 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : IFX Cross-Platform MMC device driver file
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
** $Date                        $Author                 $Comment
** 08 July 2009         Reddy Mallikarjuna              Initial release
*******************************************************************************/
/*!
  \file ifxmips_mmc.c
  \ingroup IFX_MMC_DRV
  \brief MMC host driver for SD Card
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/highmem.h>
#include <linux/mmc/host.h>
#include <linux/mmc/protocol.h>
#include <linux/mmc/mmc.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/cacheflush.h>
#include <asm/div64.h>
#include <asm/io.h>
#include <asm/scatterlist.h>
#include <linux/platform_device.h>

/** Platform specific header files */
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_dma_core.h>
/** Platform specific gpio configurations*/
#if defined(CONFIG_DANUBE)
#include "ifxmips_mmc_danube.h"
#elif defined(CONFIG_AMAZON_SE)
#include "ifxmips_mmc_amazon_se.h"
#elif defined(CONFIG_AR9)
#include "ifxmips_mmc_ar9.h"
#elif defined(CONFIG_VR9)
#include "ifxmips_mmc_vr9.h"
#else
#error Platform is not specified!
#endif

#include "ifxmips_mmc.h"
#include "ifxmips_mmc_reg.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#define MMC_MAJOR                       201 
#define MMC_PKT_BUF_SIZE                512
#define DMA_TX_BURST_LEN                DMA_BURSTL_4DW
#define DMA_RX_BURST_LEN                DMA_BURSTL_4DW
#define IFX_MMC_DRV_MODULE_NAME         "ifxmips_mmc"
#define IFX_DRV_MODULE_VERSION          "1.0.3"
static char version[] __devinitdata =
        IFX_MMC_DRV_MODULE_NAME ".c:v" IFX_DRV_MODULE_VERSION " \n";

#define DRIVER_NAME                     "ifx_mmc_host"
#define SUPPORT_50_MHZ 1
#if defined(SUPPORT_50_MHZ) & SUPPORT_50_MHZ
static unsigned int fmax                = 50000000;
#else
static unsigned int fmax                = 25000000;
#endif
#define ENABLE_TIMER	                1
ifx_sdio_controller_priv_t *ifx_priv    = NULL;
struct mmc_host *ifx_mmc                = NULL;
int tx_data_recovery = 0, rx_data_recovery = 0;
int tx_rec_cnt = 0,rx_rec_cnt = 0, scan_en= 0 ;
#define DBG(host,fmt,args...)	\
	pr_debug("%s: %s: " fmt, ifx_mmc_hostname(host->mmc), __func__ , args)
/*proc file directory*/
static struct proc_dir_entry*   ifx_sdio_dir;
static void ifx_sdio_request(struct mmc_host *mmc, struct mmc_request *mrq);
static void release_platform_dev(struct device * dev) {
    dev->parent = NULL;
}
static struct platform_device platform_dev = {
    .id         = -1,
    .dev        = {
        .release       = release_platform_dev,
    },
};

/**
* Allocates the buffer .
* This function is invoke when DMA callback function to be called 
*   to allocate a new buffer for Rx packets.*/
static unsigned char *mmc_dma_buffer_alloc (int len, int *byte_offset, void **opt)
{
    unsigned char *buffer = NULL;
    *byte_offset = 0;
    buffer = kmalloc (len, GFP_ATOMIC);
    if (buffer == NULL) {
        printk(KERN_ERR "%s[%d]: Buffer allocation failed!!!\n", __func__,__LINE__);
        return NULL;
    }
    return buffer;
}

/**
* Free buffer.
* This function frees a buffer previously allocated by mmc_dma_buffer_alloc. 
* This is handled in ifx_data_recv function
*/
static int mmc_dma_buffer_free (u8 * dataptr, void *opt)
{
    /* buffer is freed once it's receive from DMA (in ifx_data_recv function) */
    return IFX_SUCCESS;
}

/** Receive data once get the DMA pesudo interrupt */
static int
ifx_data_recv (struct dma_device_info *dma_dev)
{
    unsigned char *buf = NULL, *buffer;
    unsigned int len, remain;
    unsigned long flags;
	struct ifx_sdio_host *host = ifx_priv->host;
	len = dma_device_read (dma_dev, &buf, NULL);
/*	printk("%s,   len:%d\n", __func__, len);  */
	buffer = ifx_sdio_kmap_atomic(host, &flags) + host->sg_off;
	remain = host->sg_ptr->length - host->sg_off;
	memcpy (buffer, ((unsigned char *) buf) ,len);
	/* Unmap the buffer.*/
	ifx_sdio_kunmap_atomic(host, buffer, &flags);
	host->sg_off += len;
	host->size -= len;
	remain -= len;
	
	if(buf) kfree(buf);
	if (host->size == 0) {
	    MMC_WRITE_REG32(0, MCI_IM1);
	    MMC_WRITE_REG32(MMC_READ_REG32(MCI_IM0) | MCI_IM_DE, MCI_IM0);
	}
	return IFX_SUCCESS;
}

/**
* DMA Pseudo Interrupt handler.
* This function handle the DMA pseudo interrupts to handle the data packets Rx/Tx over DMA channels
* It will handle the following interrupts
*   RCV_INT: DMA receive the packet interrupt,So get from the PPE peripheral
*   TX_BUF_FULL_INT: TX channel descriptors are not availabe, so, stop the transmission 
        and enable the Tx channel interrupt.
*   TRANSMIT_CPT_INT: Tx channel descriptors are availabe and resume the transmission and 
        disable the Tx channel interrupt.
*/
static int mmc_dma_intr_handler (struct dma_device_info *dma_dev, int status)
{
    int i;
    switch (status) {
        case RCV_INT:
            ifx_data_recv (dma_dev);
            break;
        case TX_BUF_FULL_INT:
            for(i = 0; i < dma_dev->max_tx_chan_num; i++) {
                if((dma_dev->tx_chan[i])->control == IFX_DMA_CH_ON) {
                    dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
                }
            }
            break;
        case TRANSMIT_CPT_INT:
            for(i = 0; i< dma_dev->max_tx_chan_num; i++) {
                dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
            }
            break;
    }
    return IFX_SUCCESS;
}

/**
* Register with DMA device driver to handle the MMC port DMA channels.
* This function initializes the passed DMA device structure for usage as MMC DMA device.
*/
static int setup_dma_driver (ifx_sdio_controller_priv_t * priv)
{
    int i = 0, ret;
    priv->dma_device = dma_device_reserve ("SDIO");
    if (!priv->dma_device) {
        printk(KERN_ERR "%s: Reserve with DMA device failed!!!\n", __func__);
        return -ENODEV;
    }
    priv->dma_device->intr_handler          = mmc_dma_intr_handler;
    priv->dma_device->buffer_alloc          = mmc_dma_buffer_alloc;
    priv->dma_device->buffer_free           = mmc_dma_buffer_free;
    priv->dma_device->num_rx_chan           = 1;
    priv->dma_device->num_tx_chan           = 1;      
    priv->dma_device->tx_burst_len          = DMA_TX_BURST_LEN;
    priv->dma_device->rx_burst_len          = DMA_RX_BURST_LEN;
    priv->dma_device->tx_endianness_mode    = IFX_DMA_ENDIAN_TYPE3;
    priv->dma_device->rx_endianness_mode    = IFX_DMA_ENDIAN_TYPE3;
    priv->dma_device->port_tx_weight        = 7;
    
    for (i = 0; i < priv->dma_device->num_rx_chan; i++) {
        priv->dma_device->rx_chan[i]->packet_size               = MMC_PKT_BUF_SIZE;
        priv->dma_device->rx_chan[i]->control                   = IFX_DMA_CH_ON;
        priv->dma_device->rx_chan[i]->desc_len                  = 1;
    }
    
    for (i = 0; i < priv->dma_device->num_tx_chan; i++) {
        priv->dma_device->tx_chan[i]->control = IFX_DMA_CH_ON;
        priv->dma_device->tx_chan[i]->desc_len                  = 1;
    }
    ret = dma_device_register (priv->dma_device);
    if ( ret != IFX_SUCCESS) {
        printk(KERN_ERR "%s: Register with DMA device failed!!!\n", __func__);
        return IFX_ERROR;
    }
    for (i = 0; i < priv->dma_device->num_rx_chan; i++)
        priv->dma_device->rx_chan[i]->open (priv->dma_device->rx_chan[i]);
    return IFX_SUCCESS;
}

/**
* Unregister with DMA device driver.
*/
static int cleanup_dma_driver (ifx_sdio_controller_priv_t * priv)
{
    if (priv->dma_device) {
        dma_device_unregister (priv->dma_device);
        dma_device_release(priv->dma_device);
    }
    return 0;
}

/** 
* This function is called once the processing of mmmc command/data processing is done.
* This will callback to the block device driver
*/
static void
ifx_sdio_request_end(struct ifx_sdio_host *host, struct mmc_request *mrq)
{
    MMC_WRITE_REG32(0, MCI_CMD);
    BUG_ON(host->data);
    host->mrq = NULL;
    host->cmd = NULL;
    if (mrq->data)
        mrq->data->bytes_xfered = host->data_xfered;
    MMC_WRITE_REG32(0x0 , SDIO_DMACON);
    /* Need to drop the host lock here; ifx_mmc_request_done may call back into the driver...*/
    spin_unlock(&host->lock);
    ifx_mmc_request_done(host->mmc, mrq);
    spin_lock(&host->lock);
}

/** 
* Stop the data receive 
* Once the data processing is done, then clear the MMc data control register and inetrrupts
*/
static void ifx_sdio_stop_data(struct ifx_sdio_host *host)
{
    MMC_WRITE_REG32(0, MCI_DCTRL);
    MMC_WRITE_REG32(0, MCI_IM1);
    host->data = NULL;
}

/** 
* Start the data receive.
* This function is called once request is come fom the upper layer driver
*/
static void ifx_sdio_start_data(struct ifx_sdio_host *host, struct mmc_data *data)
{
    unsigned int datactrl, timeout, irqmask=0;
    unsigned long long clks;
    int blksz_bits, write_flag;
    
    DBG(host, "blksz %04x blks %04x flags %08x\n",	    data->blksz, data->blocks, data->flags);
    host->data = data;
/*    host->size = data->blksz;   */
    host->size = data->blocks*data->blksz; /*for multiblock mode */ 
    host->data_xfered = 0;
    
    ifx_sdio_init_sg(host, data);
    clks = (unsigned long long)data->timeout_ns * host->cclk;
    do_div(clks, 1000000000UL);
    
    timeout = data->timeout_clks + (unsigned int)clks;
    MMC_WRITE_REG32(timeout, MCI_DTIM);
    MMC_WRITE_REG32(host->size, MCI_DLGTH);
    
    blksz_bits = ffs(data->blksz) - 1;
    BUG_ON(1 << blksz_bits != data->blksz);
    datactrl = MCI_DCTRL_EN | blksz_bits << 4;
    if (data->flags & MMC_DATA_READ) {
        datactrl |= MCI_DCTRL_DIR;
		/*
		 * If we have less than a FIFOSIZE of bytes to transfer,
		 * trigger a PIO interrupt as soon as any data is available.
		 */
        write_flag=0;
        datactrl |=  (1<< 3); /*enable dma mode*/ 
        MMC_WRITE_REG32(0x1 , SDIO_DMACON);
    } else {
		/*
		 * We don't actually need to include "FIFO empty" here
		 * since its implicit in "FIFO half empty".
		 */
        irqmask = MCI_IM_TXHF;
        write_flag= 1;
        MMC_WRITE_REG32(0x2 , SDIO_DMACON);
    }
    MMC_WRITE_REG32(datactrl, MCI_DCTRL);
    MMC_WRITE_REG32(MMC_READ_REG32(MCI_IM0) & ~MCI_IM_DE, MCI_IM0);
    if(write_flag)	{
        write_flag=0;
        MMC_WRITE_REG32(irqmask, MCI_IM1);
    }
}

/** 
* Start the command processing
*/
static void
ifx_sdio_start_command(struct ifx_sdio_host *host, struct mmc_command *cmd, u32 c)
{
    DBG(host, "op %02x arg %08x flags %08x\n",    cmd->opcode, cmd->arg, cmd->flags);
    
    if (MMC_READ_REG32(MCI_CMD) & MCI_CPSM_ENABLE) {
        MMC_WRITE_REG32(0, MCI_CMD);
        udelay(1);
    }
    c |= cmd->opcode | MCI_CPSM_ENABLE;
    if (cmd->flags & MMC_RSP_PRESENT) {
        if (cmd->flags & MMC_RSP_136)
            c |= MCI_CMD_LONG_RSP;
        c |= MCI_CMD_SHORT_RSP;
    }
    if (/*interrupt*/0)
        c |= MCI_CPSM_INTERRUPT;
        
    host->cmd = cmd;
  /*  printk("%s[%d] cmd: 0x%08x, arg:0x%08x\n",__func__,__LINE__,c,cmd->arg); */
    MMC_WRITE_REG32(cmd->arg, MCI_ARG);
    MMC_WRITE_REG32(c, MCI_CMD);
}

/**
* Handle completion of data transfers.
*/
static void
ifx_sdio_data_irq(struct ifx_sdio_host *host, struct mmc_data *data,
	      unsigned int status)
{
    if (status & MCI_STAT_DBE) {
        host->data_xfered += data->blksz;
    }
    if (status & (MCI_STAT_DCF|MCI_STAT_DTO|MCI_STAT_TU|MCI_STAT_RO)) {
        if (status & MCI_STAT_DCF) {
            data->error = MMC_ERR_BADCRC;
            /*printk("%s[%d] Data crc err(Status:0x%08x)!!!\n",__FUNCTION__,__LINE__,status); */
        } else if (status & MCI_STAT_DTO) {
            data->error = MMC_ERR_TIMEOUT;
            /*printk("%s[%d] Data time out err(Status:0x%08x)!!!\n",__FUNCTION__,__LINE__,status);*/
        } else if (status & (MCI_STAT_TU)) {
            data->error = MMC_ERR_FIFO;
            tx_data_recovery = 1;
            /*printk("%s[%d] Data fifo TU err(Status: 0x%08x)!!!\n",__FUNCTION__,__LINE__,status);*/
        } else if (status & (MCI_STAT_RO)) {
            data->error = MMC_ERR_FIFO;
           /* rx_data_recovery = 1;*/
            /*printk("%s[%d] Data fifo RO err(Status:0x%08x)!!!\n",__FUNCTION__,__LINE__,status);*/
        } else if (status & MCI_STAT_SBE  ) {
            data->error = MMC_ERR_STARTBIT;  
            /*printk("%s[%d] Data fifo err(Status:0x%08x)!!!\n",__FUNCTION__,__LINE__,status);*/
        }
        status |= MCI_STAT_DE;
        /* We hit an error condition.  Ensure that any data
		 * partially written to a page is properly coherent.
		 */
		if (host->sg_len && data->flags & MMC_DATA_READ)
		    flush_dcache_page(host->sg_ptr->page);
		}
		if (status & MCI_STAT_DE) {
		    ifx_sdio_stop_data(host);
		    
		if (!data->stop) {
		    ifx_sdio_request_end(host, data->mrq);
		} else {
		    ifx_sdio_start_command(host, data->stop, 0);
		}
	}
}

/**
* Handle completion of command and data transfers.
*/
static void
ifx_sdio_cmd_irq(struct ifx_sdio_host *host, struct mmc_command *cmd,
	     unsigned int status)
{
    host->cmd = NULL;
    
    cmd->resp[0] = MMC_READ_REG32(MCI_REP0);
    cmd->resp[1] = MMC_READ_REG32(MCI_REP1);
    cmd->resp[2] = MMC_READ_REG32(MCI_REP2);
    cmd->resp[3] = MMC_READ_REG32(MCI_REP3);
    
    if (status & MCI_STAT_CTO) {
        cmd->error = MMC_ERR_TIMEOUT;
    } else if (status & MCI_STAT_CCF && cmd->flags & MMC_RSP_CRC) {
        cmd->error = MMC_ERR_BADCRC;
    }
    if (!cmd->data || cmd->error != MMC_ERR_NONE) {
        if (host->data)
            ifx_sdio_stop_data(host);
        ifx_sdio_request_end(host, cmd->mrq);
    } else if (!(cmd->data->flags & MMC_DATA_READ)) {
        ifx_sdio_start_data(host, cmd->data);
    }
}

/**
* Data transfer IRQ handler.It will handle in IM1 interrupt line
*/
static irqreturn_t ifx_sdio_pio_irq(int irq, void *dev_id)
{
    unsigned int status, remain, len;
    struct ifx_sdio_host *host = dev_id;
    struct dma_device_info *dma_dev = ifx_priv->dma_device;
    unsigned long flags;
    char *buffer;
    
    status = MMC_READ_REG32(MCI_STAT);
    DBG(host, "irq1 %08x\n", status);
    /* once we get the ready for data transmit, then disable all interrupts on MASK1 */
    MMC_WRITE_REG32(0, MCI_IM1);
    
    /* Map the current scatter buffer.*/
    buffer = ifx_sdio_kmap_atomic(host, &flags) + host->sg_off;
    remain = host->sg_ptr->length - host->sg_off;
    len = 0;
    
    len = dma_device_write(dma_dev, buffer, host->size, NULL);
    if(len  != host->size) {
        printk("%s[%d] dma device full!!!  len:0x%08x....\n",__FUNCTION__,__LINE__,len);
    }
	/* Unmap the buffer.*/
	ifx_sdio_kunmap_atomic(host, buffer, &flags);
	host->sg_off += len;
	host->size -= len;
	remain -= len;
	
	if(!remain )
	    flush_dcache_page(host->sg_ptr->page);
	/*
	 * If we run out of data, disable the data IRQs; this
	 * prevents a race where the FIFO becomes empty before
	 * the chip itself has disabled the data path, and
	 * stops us racing with our data end IRQ.
	 */
    if (host->size == 0) {
        MMC_WRITE_REG32(MMC_READ_REG32(MCI_IM0) | MCI_IM_DE, MCI_IM0);
    }
    return IRQ_HANDLED;
}

/**
* Handle completion of command and data transfers.
*/
static irqreturn_t ifx_sdio_irq(int irq, void *dev_id)
{
    struct ifx_sdio_host *host = dev_id;
    unsigned int status;
    int ret = 0;
    
    spin_lock(&host->lock);
    do {
        struct mmc_command *cmd;
        struct mmc_data *data;
        status = MMC_READ_REG32(MCI_STAT);
        status &= MMC_READ_REG32(MCI_IM0);
        MMC_WRITE_REG32(status, MCI_CL);
        
        DBG(host, "irq0 %08x\n", status);
        
        data = host->data;
        if (status & (MCI_STAT_DCF|MCI_STAT_DTO|MCI_STAT_TU|
			      MCI_STAT_RO|MCI_STAT_DE|MCI_STAT_DBE) && data)
		    ifx_sdio_data_irq(host, data, status);
		    
        cmd = host->cmd;
        if (status & (MCI_STAT_CCF|MCI_STAT_CTO|MCI_STAT_CS|MCI_STAT_CRE) && cmd)
            ifx_sdio_cmd_irq(host, cmd, status);
            
        ret = 1;
    } while (status);
    
    spin_unlock(&host->lock);
    return IRQ_RETVAL(ret);
}

/** Reset the DMA channel and reprogramme it */
static void dma_channel_reset ( struct dma_device_info *dma_dev, int dir)
{
    _dma_channel_info *pCh;
    if (dir == 1) {
        pCh = dma_dev->tx_chan[dma_dev->current_tx_chan];
        pCh->reset(pCh);
    } else if ( dir == 0 ) {
        pCh = dma_dev->rx_chan[dma_dev->current_rx_chan];
        pCh->reset(pCh);
    }
}
/** 
* Request from upper layer to to send data/receive data over MMC bus
*/
static void ifx_sdio_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
    struct ifx_sdio_host *host = ifx_mmc_priv(mmc);
    ifx_priv->host = host;		
    /* printk("%s[%d] ......\n",__func__,__LINE__);  */
    WARN_ON(host->mrq != NULL);
    
    spin_lock_irq(&host->lock);
    host->mrq = mrq;
    if(tx_data_recovery  )  {
        dma_channel_reset ( ifx_priv->dma_device, 1);
        tx_data_recovery = 0;
        tx_rec_cnt++;
    }
    if (rx_data_recovery ) {
        dma_channel_reset ( ifx_priv->dma_device, 0);
        rx_data_recovery = 0;
        rx_rec_cnt++;
    }
    if (mrq->data && mrq->data->flags & MMC_DATA_READ) {
        ifx_sdio_start_data(host, mrq->data);
    }
    ifx_sdio_start_command(host, mrq->cmd, 0);
    spin_unlock_irq(&host->lock);
}

/** 
* Request from upper layer to to configure the mmc host
*/
static void ifx_sdio_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
    struct ifx_sdio_host *host = ifx_mmc_priv(mmc);  
    u32 clk = 0,  bus_width;
    
    /*	printk("%s[%d] host: 0x%08x\n",__func__,__LINE__,host); */
    if (ios->clock) {
        if (ios->clock >= host->mclk) {
            clk = MCI_CLK_BYPASS;
            host->cclk = host->mclk;
                        ifx_priv->current_speed = host->cclk;
        } else {
            clk = host->mclk / (2 * ios->clock) - 1;
            if (clk > 256)
                clk = 255;
            host->cclk = host->mclk / (2 * (clk + 1));
            ifx_priv->current_speed = host->cclk;
        }
        clk |= MCI_CLK_ENABLE;
        MMC_WRITE_REG32(clk, MCI_CLK);
    }
	if(ios->bus_width == MMC_BUS_WIDTH_1) {
	    bus_width = MMC_READ_REG32(MCI_CLK);
		bus_width &=~(MCI_WIDE_BUS);
		MMC_WRITE_REG32(bus_width, MCI_CLK);
    }
    else if(ios->bus_width == MMC_BUS_WIDTH_4) {
        bus_width = MMC_READ_REG32(MCI_CLK);
        bus_width |= (MCI_WIDE_BUS);
        MMC_WRITE_REG32(bus_width, MCI_CLK);
    }
}

static const struct mmc_host_ops ifx_sdio_ops = {
	.request	= ifx_sdio_request,
	.set_ios	= ifx_sdio_set_ios,
};

#if ENABLE_TIMER 
static void ifx_sdio_card_check_status(unsigned long data)
{
    struct ifx_sdio_host *host = (struct ifx_sdio_host *)data;
/*    
    unsigned int status ;
    status = MMC_READ_REG32(MCI_STAT);
    if(status) {
*/
    if(scan_en) {
	/*	printk("%s[%d] : status:0x%08x\n",__FUNCTION__,__LINE__,status); */
	    ifx_mmc_detect_change(host->mmc, 0);
	    scan_en= 0;
	}
/*	host->prev_stat = status; */
	mod_timer(&host->timer, jiffies + HZ);
}
#endif

/** Driver version info */
static inline int mmc_host_drv_ver(char *buf)
{
    return sprintf(buf, "IFX MMC host(SD card) driver, version %s,(c)2009 Infineon Technologies AG\n", version);
}
/** Host GPIo infoinfo */
static inline int mmc_host_gpio(char *buf)
{
    return sprintf(buf, "CMD:\t%d\nCLK:\t%d\nDATA0:\t%d\nDATA1:\t%d\nDATA2:\t%d\nDATA3:\t%d\n", \
    IFX_MCLCMD, IFX_MCLCLK,IFX_MCLDATA0,IFX_MCLDATA1,IFX_MCLDATA2, IFX_MCLDATA3);
}

/** Host bust mode infoinfo */
static inline int mmc_host_busmode(char *buf)
{
    unsigned int reg;
    reg = MMC_READ_REG32(MCI_CLK);
    if (reg & MCI_WIDE_BUS) {
        return sprintf(buf, " Bus Mode: 4 Bits\n");
    } else {
        return sprintf(buf, " Bus Mode: 1 Bits\n");
    }
}

/** Host bust mode infoinfo */
static inline int mmc_host_frequency(char *buf)
{
    return sprintf(buf, "Current Host frequncy:\t%d\n", ifx_priv->current_speed);
}

/** Displays the host information (GPIO's, Bus width & frequncy */
static int mmc_host_proc_info(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
    int len = 0;
    /* No sanity check cos length is smaller than one page */
    len += mmc_host_drv_ver(buf + len);
    len += mmc_host_gpio(buf+len);
    len += mmc_host_busmode(buf+len);
    len += mmc_host_frequency(buf+len);
    len += sprintf(buf+len, "Recovery counter: :\ttx : %d, rx: %d\n", tx_rec_cnt,rx_rec_cnt);
    *eof = 1;
    return len;
}

/** Enable card scan via proc file */
static int mmc_host_scan_restart(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
    int len = 0;
    len += sprintf(buf+len, "Re-Scan the SD-card enabled: \n");
    scan_en= 1;
    *eof = 1;
    return len;    
}
/** Displays the version of DMA module via proc file */
static int mmc_host_proc_version(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
    int len = 0;
    /* No sanity check cos length is smaller than one page */
    len += mmc_host_drv_ver(buf + len);
    *eof = 1;
    return len;    
}
 
/** create proc for debug  info, \used ifx_mmc_module_init */
static int mmc_host_proc_create(void)
{
    /* procfs */
    ifx_sdio_dir = proc_mkdir ("driver/ifx_mmc", NULL);
    if (ifx_sdio_dir == NULL) {
        printk(KERN_ERR "%s: Create proc directory (/driver/ifx_mmc) failed!!!\n", __func__);
        return IFX_ERROR;
    }
    create_proc_read_entry("version", 0, ifx_sdio_dir, \
                    mmc_host_proc_version,  NULL);
    create_proc_read_entry("host_info", 0, ifx_sdio_dir, \
                    mmc_host_proc_info,  NULL);
    create_proc_read_entry("probecard", 0, ifx_sdio_dir, \
                    mmc_host_scan_restart,  NULL);
    return IFX_SUCCESS;
}

/** remove of the proc entries, \used ifx_mmc_module_exit */
static void mmc_host_proc_delete(void)
{
    
    remove_proc_entry("version", ifx_sdio_dir);
    remove_proc_entry("host_info", ifx_sdio_dir);
    remove_proc_entry("probecard", ifx_sdio_dir);
    remove_proc_entry("driver/ifx_mmc",  NULL);
}

/* probe the SD card */
static int ifx_sdio_probe(struct device *dev)
{
    struct mmc_host *mmc = ifx_mmc;
    int ret;
    
    mmc = ifx_mmc_alloc_host(mmc, dev);
    if (!mmc) {
		printk(KERN_ERR "%s: mmc alloc host failed!!!\n", __func__);
		ret = -ENOMEM;
		return ret;
	}
	dev_set_drvdata(dev, mmc);
	ifx_mmc_add_host(mmc);
/*	printk("%s[%d] End\n",__FUNCTION__,__LINE__); */
	return 0;
}

static int ifx_sdio_remove(struct device *dev)
{
    struct mmc_host *mmc =  dev_get_drvdata(dev);
    
    /*printk("%s[%d] ....\n",__FUNCTION__,__LINE__); */
    dev_set_drvdata(dev, NULL);
    
    if (mmc) {
#if ENABLE_TIMER
        struct ifx_sdio_host *host = ifx_mmc_priv(mmc);
        del_timer_sync(&host->timer);
#endif
        ifx_mmc_remove_host(mmc);
        MMC_WRITE_REG32(0, MCI_IM0);
        MMC_WRITE_REG32(0, MCI_IM1);
        MMC_WRITE_REG32(0, MCI_CMD);
        MMC_WRITE_REG32(0, MCI_DCTRL);
        ifx_mmc_free_host(mmc);
    }
    return 0;
}

static struct device_driver ifx_sdio_driver= {
        .name           = DRIVER_NAME,
        .bus            = &platform_bus_type,
        .probe          = ifx_sdio_probe,
        .remove         = ifx_sdio_remove,
};

/** Init of the MMC module*/
static int __init ifx_mmc_module_init(void)
{
    unsigned int sdio_id = 0;
    struct ifx_sdio_host *host = NULL;
    int retval=0;
    char ver_str[128] = {0};
    tx_data_recovery = 0;
    rx_data_recovery = 0;
    tx_rec_cnt = 0;
    rx_rec_cnt = 0;
    scan_en= 0;
    
    sdio_id = MMC_READ_REG32(SDIO_ID);
    if (sdio_id != 0xF041C030) {
        printk(KERN_ERR "%s[%d]: IFX MMC Controller not found!!!\n", __func__,__LINE__);
        return -ENODEV;
	}
	/* power on SDIO module */
	SDIO_PMU_SETUP(IFX_PMU_ENABLE);
	/* configure the GPIO settingings */
#if defined(CONFIG_DANUBE)
    danube_mmc_gpio_configure ();
#elif defined(CONFIG_AMAZON_SE)
    amazon_se_mmc_gpio_configure ();
#elif defined(CONFIG_AR9)
    ar9_mmc_gpio_configure ();
#elif defined(CONFIG_VR9)
    vr9_mmc_gpio_configure ();
#endif 
    ifx_priv =  kmalloc (sizeof (ifx_sdio_controller_priv_t), GFP_ATOMIC);
    if (ifx_priv == NULL) {
        printk(KERN_ERR "%s[%d]: Memory allocation failed!!!\n", __func__,__LINE__);
        return -ENOMEM;
    }
    ifx_mmc = kmalloc(sizeof(struct mmc_host) + sizeof(struct ifx_sdio_host), GFP_KERNEL);
    if (ifx_mmc == NULL) {
        printk(KERN_ERR "%s[%d]: Memory allocation failed!!!\n", __func__,__LINE__);
        if( ifx_priv) 
            kfree(ifx_priv);
        return -ENOMEM;
    }
    memset (ifx_priv, 0, sizeof (ifx_sdio_controller_priv_t));
    memset (ifx_mmc, 0, (sizeof (struct mmc_host)+sizeof(struct ifx_sdio_host)));
    
    retval = setup_dma_driver (ifx_priv);
    if (retval != IFX_SUCCESS)
        goto free_cdev_mem;
    
    /* SDIO clock 100MHz / 2 = 50MHz */
#if defined(SUPPORT_50_MHZ) & SUPPORT_50_MHZ
    MMC_WRITE_REG32(0x200, SDIO_CLC);
#else
    /* SDIO clock 100MHz / 4 = 25MHz */
    MMC_WRITE_REG32(0x400, SDIO_CLC);
#endif
    MMC_WRITE_REG32(MCI_PWR_ON , MCI_PWR);
    MMC_WRITE_REG32(SDIO_CTRL_SDIOEN , SDIO_CTRL);
    
    retval = mmc_host_proc_create();
    if (retval == IFX_ERROR ) 
        goto free_cdev_mem;
    
    host = ifx_mmc_priv(ifx_mmc);
    host->mclk = fmax; 
    host->mmc = ifx_mmc;
    host->base = (u32*)IFX_MMC_BASE_ADDR;  
    ifx_mmc->ops = &ifx_sdio_ops;
    ifx_mmc->f_min = 400000;
    ifx_mmc->f_max = host->mclk;
    ifx_mmc->ocr_avail = (  MMC_VDD_32_33  |   MMC_VDD_33_34 );
    ifx_mmc->caps = MMC_CAP_4_BIT_DATA;
    ifx_mmc->max_hw_segs = 1;
    ifx_mmc->max_phys_segs = 1;	
    ifx_mmc->max_sectors = 127;
    /* Set the maximum segment size.  */
    ifx_mmc->max_seg_size =4096;
    spin_lock_init(&host->lock);
    MMC_WRITE_REG32(0, MCI_IM0);
    MMC_WRITE_REG32(0, MCI_IM1);
    MMC_WRITE_REG32(0xfff, MCI_CL);
    retval = request_irq(IFX_MMC_CONTROLLER_INTR0_IRQ, ifx_sdio_irq, IRQF_DISABLED, DRIVER_NAME " (cmd)", host);
    if (retval) {
        printk(KERN_ERR "%s[%d]: Request IRQ failed!!!\n", __func__,__LINE__);
        goto free_proc_mem;
    }
    retval = request_irq(IFX_MMC_CONTROLLER_INTR1_IRQ, ifx_sdio_pio_irq, IRQF_DISABLED, DRIVER_NAME " (pio)", host);
    if (retval) {
        printk(KERN_ERR "%s[%d]: Request IRQ failed!!!\n", __func__,__LINE__);
        goto free_irq0;
	}
	MMC_WRITE_REG32(MCI_IRQENABLE, MCI_IM0);
	
#if ENABLE_TIMER 
    init_timer(&host->timer);
    host->timer.data = (unsigned long)host;
    host->timer.function = ifx_sdio_card_check_status;
    host->timer.expires = jiffies + HZ;
    add_timer(&host->timer);
#endif
    MMC_WRITE_REG32(SDIO_IMC_INTR0| SDIO_IMC_INTR1, SDIO_IMC);
    retval = driver_register(&ifx_sdio_driver);
    if (retval < 0) {
        printk(KERN_ERR "%s[%d]: Device Register failed !!!\n", __func__,__LINE__);
        printk(KERN_ERR "%s[%d] retval=%d\n", __func__, __LINE__,retval);
		goto timer_del;
	}
	platform_dev.name = DRIVER_NAME;
	retval = platform_device_register(&platform_dev);
	if(retval < 0) {
	    printk(KERN_ERR "%s[%d] retval=%d\n", __func__, __LINE__,retval);
	    goto del_reg;
	}
/*	printk("%s[%d] End\n",__FUNCTION__,__LINE__); */
/* Print the driver version info */                      
    mmc_host_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
	return 0;
del_reg:
    driver_unregister(&ifx_sdio_driver);
timer_del:
#if ENABLE_TIMER 
    del_timer_sync(&host->timer);
#endif
free_irq0:
    free_irq(IFX_MMC_CONTROLLER_INTR0_IRQ, host);
free_proc_mem:
    mmc_host_proc_delete();
free_cdev_mem:
    if(ifx_priv)
        cleanup_dma_driver (ifx_priv);
    if(ifx_mmc)
        kfree (ifx_mmc);
    return -1;
}

/** Clean up MMC moule */
static void __exit ifx_mmc_module_exit(void)
{
    driver_unregister(&ifx_sdio_driver);
    platform_device_unregister(&platform_dev);
    if(ifx_priv)
        cleanup_dma_driver (ifx_priv);    
    MMC_WRITE_REG32(0, MCI_IM0);
    MMC_WRITE_REG32(0, MCI_IM1);
    MMC_WRITE_REG32(0, MCI_CMD);
    MMC_WRITE_REG32(0, MCI_DCTRL);
     /** remove of the proc entries */
    mmc_host_proc_delete();
    free_irq(IFX_MMC_CONTROLLER_INTR0_IRQ, NULL);
    free_irq(IFX_MMC_CONTROLLER_INTR1_IRQ, NULL);

}

module_init(ifx_mmc_module_init);
module_exit(ifx_mmc_module_exit);
module_param(fmax, uint, 0444);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Reddy.Mallikarjun@infineon.com");
MODULE_VERSION(IFX_DRV_MODULE_VERSION);
MODULE_DESCRIPTION("IFX CPE MMC host device driver ");
MODULE_SUPPORTED_DEVICE ("IFX DSL CPE Devices (Danube, Amazon-SE, AR9 & VR9 )");
