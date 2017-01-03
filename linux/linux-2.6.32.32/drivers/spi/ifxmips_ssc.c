/******************************************************************************
**
** FILE NAME    : ifxmips_ssc.c
** PROJECT      : IFX UEIP
** MODULES      : SSC (Synchronous Serial Controller)
**
** DATE         : 3 July 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : SCC Driver
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
** 3 Jul,2009   Lei Chuanhua    First UEIP release
*******************************************************************************/
/*!
  \file ifxmips_ssc.c
  \ingroup IFX_SSC
  \brief ssc bus driver source file
*/
#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <asm/page.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/delay.h>
#include <asm/system.h>

/* Project header */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>

#include "ifxmips_ssc_reg.h"
#include "ifxmips_ssc.h"

#ifdef CONFIG_IFX_PMCU
#include "ifxmips_ssc_pm.h"
#endif /* CONFIG_IFX_PMCU */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/sched.h>
#define kill_proc(p,s,v)        send_sig(s,find_task_by_vpid(p),0)
#endif

#define IFX_SSC_VER_MAJOR          2
#define IFX_SSC_VER_MID            2
#define IFX_SSC_VER_MINOR          6

#if defined(CONFIG_AMAZON_SE) || defined(CONFIG_AR10)
#undef IFX_SSC_DMA_SUPPORT
#else
#define IFX_SSC_DMA_SUPPORT
#endif /* defined(CONFIG_AMAZON_SE) || defined(CONFIG_AR10) */

#define IFX_SSC_SPLIT_BAUD_RATE   25000000

#ifdef CONFIG_IFX_SPI_DEBUG
#define INLINE 
enum {
    SSC_MSG_TX_FIFO  = 0x00000001,
    SSC_MSG_TX_DMA   = 0x00000002,  
    SSC_MSG_RX_FIFO  = 0x00000004,  
    SSC_MSG_RX_DMA   = 0x00000008,  
    SSC_MSG_INT      = 0x00000010,  /* Interrupt msg */
    SSC_MSG_CFG      = 0x00000020,
    SSC_MSG_THREAD   = 0x00000040,
    SSC_MSG_TASKLET  = 0x00000080,
    SSC_MSG_DEBUG    = 0x00000100,
    SSC_MSG_ERROR    = 0x00000200,
    SSC_MSG_INIT     = 0x00000400, /* Initialization msg */
    SSC_MSG_QUEUE    = 0x00000800,
    SSC_MSG_LOCK     = 0x00001000,
    SSC_MSG_CALLBACK = 0x00002000,
    SSC_MSG_ANY      = 0xffffffff,   /* anything */
};
static void ifx_ssc_debug(struct ifx_ssc_port *port, const char *fmt, ...);

#define IFX_SSC_PRINT(_port, _m, _fmt, args...) do { \
    if ((_port)->ssc_debug & (_m)) {                 \
        ifx_ssc_debug((_port), (_fmt), ##args);      \
    }                                                \
} while (0)

#else
#define INLINE  inline
#define IFX_SSC_PRINT(_port, _m, _fmt, ...)
#endif /* CONFIG_IFX_SPI_DEBUG */

#define IFX_SSC_NAME             "ifx_ssc"

/* This irq number is platform specific, defined in platform header.h */
#define IFX_SSC_FAKE_IRQ_NO      IFX_SSC_TIR

/**
 * This is the per-channel data structure containing pointers, flags
 * and variables for the port.  ifx_ssc_isp is allocated in ifx_ssc_init() 
 * based on the chip version.
 */
static struct ifx_ssc_port *ifx_ssc_isp = NULL;

static struct proc_dir_entry *ifx_ssc_proc;

typedef struct {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
    int (*request) (unsigned int irq, irqreturn_t  (*handler) 
        (int, void *), unsigned long irqflags,
        const char *devname, void *dev_id);
#else
    int (*request) (unsigned int irq, irqreturn_t  (*handler) 
        (int, void *, struct pt_regs *), unsigned long irqflags,
        const char *devname, void *dev_id);
#endif
    void (*free) (unsigned int irq, void *dev_id);
} ifx_ssc_int_wrapper_t;


static ifx_ssc_int_wrapper_t ifx_ssc_int_wrapper = {
    .request  = request_irq,    
    .free     = free_irq,         
};

/**
 * \fn static INLINE void ifx_ssc_raise_fake_irq(unsigned int irq)
 * \brief Set ICU request register bit to generate fake interrupt
 *
 * \param   irq    fake interrupt irq number
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void 
ifx_ssc_raise_fake_irq(unsigned int irq)
{
    unsigned long flags;
    struct ifx_ssc_port *port = (struct ifx_ssc_port *) &ifx_ssc_isp[0];

    IFX_SSC_PRINT(port, SSC_MSG_INT, 
        "%s irq %d triggered \n", __func__, irq);

    spin_lock_irqsave(&port->ssc_irq_lock, flags);
    ifx_icu_irsr_set(irq);
    spin_unlock_irqrestore(&port->ssc_irq_lock, flags);
}

/**
 * \fn static void  ifx_ssc_start_tasklet(struct ifx_ssc_port *port)
 * \brief Trigger different schedule procedures according to different context.
 * if caller is already in tasklet, it will be done in caller's tasklet
 *
 * \param   port       Pointer to structure #ifx_ssc_port  
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void 
ifx_ssc_start_tasklet(struct ifx_ssc_port *port)
{
    struct tasklet_struct *ptasklet;

    /* 
     * Calls the internal process to serve the queue. This routine would
     * immediately return in case the SSC hardware is currently used to serve
     * another request. 
     */
    ptasklet = &port->ssc_txrxq;
    if (in_irq()) { /* Hardware irq */
        IFX_SSC_PRINT(port, SSC_MSG_INT, "%s hardware irq schedule\n", __func__);
        tasklet_hi_schedule(ptasklet);
    }
    else if (in_softirq()) { /* Softirq or tasklet */
        IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s softirq schedule\n", __func__);
        if (tasklet_trylock(ptasklet)) { /* tasklet_trylock for SMP*/
            ptasklet->func(ptasklet->data);
            tasklet_unlock(ptasklet);
        }
        else {
            IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s should never happen\n", __func__);
        }
    }
    else { /* Process  context */
        IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s process schedule\n", __func__);
        ifx_ssc_raise_fake_irq(port->ssc_fake_irq);
    }
}

/**
 * \fn static irqreturn_t ifx_ssc_fake_isr (int irq, void *dev_id)
 * \brief Fake interrupt handler
 *
 * \param   irq    fake interrupt irq number
 * \param   dev_id vague type, will be converted to 
 *                 pointer to structure #ifx_ssc_port
 * \return  IRQ_HANDLED irq has been handled.
 * \ingroup IFX_SSC_INTERNAL
 */
static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
ifx_ssc_fake_isr (int irq, void *dev_id)
#else
ifx_ssc_fake_isr (int irq, void *dev_id, struct pt_regs *regs)
#endif 
{
    struct ifx_ssc_port *port = (struct ifx_ssc_port *) dev_id;

    IFX_SSC_PRINT(port, SSC_MSG_INT, "%s irq %d served\n", __func__, irq);
    ifx_icu_irsr_clr(irq);
    ifx_ssc_start_tasklet(port);
    return IRQ_HANDLED;
}

#ifdef CONFIG_IFX_SPI_DEBUG
/**
 * \fn static void ifx_ssc_debug(struct ifx_ssc_port *port, const char *fmt, ...)
 * \brief Debug all kinds of level message
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   fmt     debug output format
 * 
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static void
ifx_ssc_debug(struct ifx_ssc_port *port, const char *fmt, ...)
{
    static char buf[256] = {0};      /* XXX */
    va_list ap;
#ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
    static const char *p = "tasklet";
#else
    static const char *p = "kthread";
#endif
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap); 

    printk("%s %s: %s\n", p, port->name, buf);
}
#endif /* CONFIG_IFX_SPI_DEBUG */

/**
 * \fn static INLINE void ifx_ssc_wait_finished(struct ifx_ssc_port *port)
 * \brief Wait for SPI bus becomes idle, FIFO empty doesn't means spi bus idle.
 *        to start another transaction, need to make sure bus is idle
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE void 
ifx_ssc_wait_finished(struct ifx_ssc_port *port)
{
    while((IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_BUSY)){
        ; /* Do nothing */
    }
}

/**
 * \fn static INLINE unsigned int ifx_ssc_get_kernel_clk(struct ifx_ssc_port *port)
 * \brief Get SSC clock speed.
 * Returns the current operating speed of the SSC peripheral, depending on chip
 * specific bus speed and RMC setting in CLC register.
 * 
 * \param   port     Pointer to structure #ifx_ssc_port
 * \return  >0       Peripheral speed in HZ
 * \return  0        Error
 * \ingroup IFX_SSC_INTERNAL
 */
static INLINE unsigned int 
ifx_ssc_get_kernel_clk(struct ifx_ssc_port *port)
{
    /* This function assumes that the CLC register is set with the
     * appropriate value for RMC.
     */
    unsigned int rmc;

    rmc = IFX_SSC_GET_CLC_RMC(port);
    if (rmc == 0) {
        printk(KERN_ERR "%s rmc==0 \n", __func__);
        return 0;
    }
    return (ifx_get_fpi_hz() / rmc);
}

/**
 * \fn static int ifx_ssc_stats_proc(char *page, char **start, off_t offset, int count, int *eof, void *data)
 * \brief SSC proc file read.
 * This function creates the proc file output for the SSC driver statistics.
 * 
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_stats_proc(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
    int off = 0;
    int t = 0;
    struct ifx_ssc_port *port;
    struct ifx_ssc_statistics *stats;
    ssc_device_t *dev;
    
    off += sprintf(page + off, "Statistics for IFX Synchronous Serial Controller(SSC)\n\n");
    for (t = 0; t < IFX_SSC_MAX_PORT_NUM; t++) {
        port = &ifx_ssc_isp[t];

        stats = &port->stats;
        
        off += sprintf(page + off, "SSC%d\n",port->port_idx);
        off += sprintf(page + off, "RX overflow errors %d\n", stats->rxOvErr);
        off += sprintf(page + off, "RX underflow errors %d\n", stats->rxUnErr);
        off += sprintf(page + off, "TX overflow errors %d\n", stats->txOvErr);
        off += sprintf(page + off, "TX underflow errors %d\n", stats->txUnErr);
        off += sprintf(page + off, "Abort errors %d\n", stats->abortErr);
        off += sprintf(page + off, "Mode errors %d\n", stats->modeErr);
        off += sprintf(page + off, "RX Bytes %d\n", stats->rxBytes);
        off += sprintf(page + off, "TX Bytes %d\n", stats->txBytes);
        off += sprintf(page + off, "TX FIFO transaction %d\n", stats->txFifo);
        off += sprintf(page + off, "TX DMA transaction %d\n", stats->txDma);
        off += sprintf(page + off, "TX DMA bytes %d\n", stats->txDmaBytes);
        off += sprintf(page + off, "RX FIFO transaction %d\n", stats->rxFifo);
        off += sprintf(page + off, "RX DMA transaction %d\n", stats->rxDma);
        off += sprintf(page + off, "RX DMA bytes %d\n", stats->rxDmaBytes);
        off += sprintf(page + off, "SSC bus status %s\n", port->ssc_cs_locked ? "locked" : "unlocked");
        off += sprintf(page + off, "\n");
        /* Per device statistics */
        IFX_SSC_SEM_LOCK(port->dev_sem);
        TAILQ_FOREACH(dev, &port->ssc_devq, dev_entry){
            off += sprintf(page + off, "Device %s:\n", dev->dev_name);
            off += sprintf(page + off, "Rx Bytes %d\n", dev->stats.rxBytes);
            off += sprintf(page + off, "Tx Bytes %d\n", dev->stats.txBytes);
            off += sprintf(page + off, "Context errors %d\n", dev->stats.context_err);
            off += sprintf(page + off, "Duplicated qentry errors %d\n", dev->stats.dup_qentries);
            off += sprintf(page + off, "Fragment errors %d\n", dev->stats.frag_err);
            off += sprintf(page + off, "Handler errors %d\n", dev->stats.handler_err);
            off += sprintf(page + off, "Enqueue %d\n", dev->stats.enqueue);
            off += sprintf(page + off, "Dequeue %d\n", dev->stats.dequeue);            
        }
        IFX_SSC_SEM_UNLOCK(port->dev_sem); 
    }
    *eof = 1;
    return off;
}

/**
 * \fn static int ifx_ssc_reg_read_proc(char *page, char **start, off_t offset, int count, int *eof, void *data)
 * \brief SSC proc file read.
 * This function creates the proc file output for SSC register dump.
 * 
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_reg_read_proc(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
    int off = 0;
    int t = 0;
#define IFX_SSC_REG_MAX  20
    u32 stats[IFX_SSC_REG_MAX] = {0};
    struct ifx_ssc_port *port;

    off += sprintf(page + off, "Register Dump for IFX Synchronous Serial Controller(SSC)\n\n");
    for (t = 0; t < IFX_SSC_MAX_PORT_NUM; t++) {
        port = &ifx_ssc_isp[t];
        IFX_SSC_IRQ_LOCK(port);
        stats[0] = IFX_SSC_GET_CLC(port);
        stats[1] = IFX_SSC_GET_ID(port);
        stats[2] = IFX_SSC_GET_CON(port);
        stats[3] = IFX_SSC_GET_STATE(port);
        stats[4] = IFX_SSC_GET_TX_WORD(port);
        stats[5] = IFX_SSC_GET_FIFO_STATUS(port);
        stats[6] = IFX_SSC_GET_RX_FIFO_CTRL(port);
        stats[7] = IFX_SSC_GET_TX_FIFO_CTRL(port);
        stats[8] = IFX_SSC_GET_BR(port);
        stats[9] = IFX_SSC_GET_FRAMING_CON(port);
        stats[10] = IFX_SSC_GET_FRAMING_STATUS(port);
        stats[11] = IFX_SSC_GET_GPOCON(port);
        stats[12] = IFX_SSC_GET_GPOSTAT(port);
        stats[13] = IFX_SSC_GET_RXREQ(port);
        stats[14] = IFX_SSC_GET_RXCNT(port);
        stats[15] = IFX_SSC_GET_DMA_CON(port);
        stats[16] = IFX_SSC_GET_IRN_EN(port);
        stats[17] = IFX_SSC_GET_IRN_CR(port);
        stats[18] = IFX_SSC_GET_IRN_ICR(port);
        IFX_SSC_IRQ_UNLOCK(port);
        off += sprintf(page + off, "SSC%d\n", port->port_idx);
        off += sprintf(page + off, "IFX_SSC_CLC     0x%08x\n", stats[0]);
        off += sprintf(page + off, "IFX_SSC_ID      0x%08x\n", stats[1]);
        off += sprintf(page + off, "IFX_SSC_MCON    0x%08x\n", stats[2]);
        off += sprintf(page + off, "IFX_SSC_STATE   0x%08x\n", stats[3]);
        off += sprintf(page + off, "IFX_SSC_TB      0x%08x\n", stats[4]);
        off += sprintf(page + off, "IFX_SSC_FSTAT   0x%08x\n", stats[5]);
        off += sprintf(page + off, "IFX_SSC_RXFCON  0x%08x\n", stats[6]);
        off += sprintf(page + off, "IFX_SSC_TXFCON  0x%08x\n", stats[7]);
        off += sprintf(page + off, "IFX_SSC_BR      0x%08x\n", stats[8]);
        off += sprintf(page + off, "IFX_SSC_SFCON   0x%08x\n", stats[9]);
        off += sprintf(page + off, "IFX_SSC_SFSTAT  0x%08x\n", stats[10]);
        off += sprintf(page + off, "IFX_SSC_GPOCON  0x%08x\n", stats[11]);
        off += sprintf(page + off, "IFX_SSC_GPOSTAT 0x%08x\n", stats[12]);
        off += sprintf(page + off, "IFX_SSC_RXREQ   0x%08x\n", stats[13]);
        off += sprintf(page + off, "IFX_SSC_RXCNT   0x%08x\n", stats[14]);
        off += sprintf(page + off, "IFX_SSC_DMACON  0x%08x\n", stats[15]);
        off += sprintf(page + off, "IFX_SSC_IRN_EN  0x%08x\n", stats[16]);
        off += sprintf(page + off, "IFX_SSC_IRN_CR  0x%08x\n", stats[17]);
        off += sprintf(page + off, "IFX_SSC_IRN_ICR 0x%08x\n", stats[18]);
        off += sprintf(page + off, "\n");
    }
    *eof = 1;
    return off;
#undef IFX_SSC_REG_MAX
}

#ifdef IFX_SSC_DMA_SUPPORT
/**
 * \fn static void ifx_ssc_dma_setup(struct ifx_ssc_port *port, int dir, int enabled)
 * \brief Setup dma direction and enable/disable
 * 
 * \param   port        Pointer to structure #ifx_ssc_port
 * \param   dir         DMA Direction, tx/rx
 * \param   enabled     DMA enable/disable
 * \return  none
 * \ingroup IFX_SSC_INTERNAL  
 */
static void 
ifx_ssc_dma_setup(struct ifx_ssc_port *port, int dir, int enabled)
{
    u32 reg = 0;
    
    IFX_SSC_IRQ_LOCK(port);
    reg = IFX_SSC_GET_DMA_CON(port);
    if (dir == IFX_SSC_DIR_RX)  {/* RX */
        if (enabled) {
            reg |= IFX_SSC_DMACON_RXON;
        }
        else {
            reg &= ~IFX_SSC_DMACON_RXON;
        }
    }
    else {
        if (enabled) {
            reg |= IFX_SSC_DMACON_TXON;
        }
        else {
            reg &= ~IFX_SSC_DMACON_TXON;
        }
    }
    IFX_SSC_SET_DMA_CON(reg, port);
    IFX_SSC_IRQ_UNLOCK(port);    
}

/**
 * \fn static INLINE void ifx_ssc_dma_irq_finished(struct ifx_ssc_port *port)
 * \brief DMA<TX/RX> interrupt received, this function calls to reschedule or wake up sleep 
 *        kernel thread
 * 
 * \param   port        Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void
ifx_ssc_dma_irq_finished(struct ifx_ssc_port *port)
{
    /* 
     * Reset the flag that we are waiting for the DMA to complete 
     * This flag should be reset before the following stuff, otherwise
     * start_tasklet will stop
     */
    atomic_set(&port->dma_wait_state, 0);
    smp_wmb();
#ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
    ifx_ssc_start_tasklet(port);
#else
    IFX_SSC_WAKEUP_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT, port->event_flags);
#endif 
}

/**
 * \fn static int ifx_ssc_dma_int_handler( struct dma_device_info* dma_dev, int status)
 * \brief Pseudo Interrupt handler for DMA.
 * This function processes DMA interrupts notified to the SSC device driver.
 * It is installed at the DMA core as interrupt handler for the SSC DMA device 
 * and handles the following DMA interrupts:
 * - In case of a DMA receive interrupt the received data is passed to the 
 *   upper layer.
 * 
 * \param   dma_dev pointer to DMA device structure
 * \param   status  type of interrupt being notified (RCV_INT: DMA receive 
 *                  interrupt, TX_BUF_FULL_INT: transmit buffer full interrupt, 
 *                  TRANSMIT_CPT_INT: transmission complete interrupt)
 * \return  OK      In case of successful data reception from DMA
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_dma_int_handler(struct dma_device_info* dma_dev, int status)
{
    int i;
    struct ifx_ssc_port *port;

    port = (struct ifx_ssc_port *) dma_dev->priv;
    IFX_SSC_PRINT(port, SSC_MSG_INT, "Interrupt status %d\n", status);
    switch(status) {
        case RCV_INT:
            IFX_SSC_PRINT(port, SSC_MSG_INT, "RCV_INT\n");
            ifx_ssc_dma_setup(port, IFX_SSC_DIR_RX, IFX_SSC_DMA_DISABLE);

            /* 
             * NB, we have to disable SPI DMA Rx channel every time, just it is not enough to
             * clear the central DMA global dma status variable. Because we should clear the
             * DMA channel interrupt status bit at the same time. However, if we don't reset
             * DMA descriptor because upper layer application may not take the data yet. Therefore,
             * DMA tasklet will continue to raise pseudo interrupt. In order to reach the above two
             * goals, it is the most simple to close the channel. It will be openned again when
             * some data need to receive.
             */
            (dma_dev->rx_chan[dma_dev->current_rx_chan])->close(dma_dev->rx_chan[dma_dev->current_rx_chan]);

            /*
             * HW WAR, if packet length is indivisible by 4, last 1~3 bytes,
             * special swapping and memcpy involved.
             */
            {
                char *p;
                int i, j, k; 

                i = port->actual_rx_len >> 2;
                j = port->actual_rx_len & 0x3;

                if (j != 0) { /* Hit last 1~3 byte case */
                    p = port->dma_rxbuf + (i << 2);
                    for (k = 0; k < j; k++) {
                        p[k] = p[4 - j + k];
                    }
                    memcpy((char *)port->rxbuf_ptr, port->dma_rxbuf, port->actual_rx_len);

                    /* Reset these for FIFO usage */
                    port->rxbuf_ptr = NULL;
                    port->actual_rx_len = 0;
                }
            }
            ifx_ssc_dma_irq_finished(port);
            break;
            
        case TX_BUF_FULL_INT:
            for(i = 0; i < dma_dev->max_tx_chan_num; i++) {
                if((dma_dev->tx_chan[i])->control == IFX_DMA_CH_ON) {
                    dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
                }
            }
            IFX_SSC_PRINT(port, SSC_MSG_INT, "TX_BUF_FULL_INT\n");
            break;
            
        case TRANSMIT_CPT_INT:
            for(i = 0; i< dma_dev->max_tx_chan_num; i++) {
                dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
            }
            IFX_SSC_PRINT(port, SSC_MSG_INT, "TRANSMIT_CPT_INT\n");

            /* XXX, where is the best place because of HW limitation 
             * NB, all cases should wait because it just indicates DMA has finished 
             * transfered data to SSC FIFO, but the data could be still in SSC FIFO.
             */
            ifx_ssc_wait_finished(port);
            ifx_ssc_dma_irq_finished(port);
            break;
            
        default:
            printk(KERN_ERR "%s unknow interrupt %d\n", __func__, status);
            break;
    }
    return 0;
}

/**
 * \fn static u8* ifx_ssc_dma_buffer_alloc(int len, int* byte_offset, void** opt)
 * \brief Allocates buffer for SSC DMA.
 * This function is installed as DMA callback function to be called when the DMA
 * needs to allocate a new buffer.
 * 
 * \param   len          Length of packet
 * \param   byte_offset Pointer to byte offset
 * \param   opt        unused  
 * \return  NULL         In case of buffer allocation fails
 * \return  buffer       Pointer to allocated memory
 * \ingroup IFX_SSC_INTERNAL 
 */
static u8* 
ifx_ssc_dma_buffer_alloc(int len, int* byte_offset, void** opt)
{
    return NULL;
}

/**
 * \fn static int ifx_ssc_dma_buffer_free(u8* dataptr,void* opt)
 * \brief Free DMA buffer.
 * This function frees a buffer previously allocated by the DMA. 
 * 
 * \param   dataptr Pointer to data buffer
 * \param   opt     unused  
 * \return  0        OK
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_dma_buffer_free(u8* dataptr,void* opt)
{
    /* SPI will be in charge of memory free if necessary */
    return 0;
} 

/**
 * \fn static int ifx_ssc_init_dma_device(int line, struct dma_device_info *dma_dev)
 * \brief Initialize SSC DMA device.
 * This function initializes the passed DMA device structure for usage as 
 * SSC DMA device.
 * 
 * \param   line     SSC device (0)
 * \param   dma_dev Pointer to dma device structure to be initialized
 * \return  0        OK
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_init_dma_device(int line, struct dma_device_info *dma_dev)
{
    int i;

    dma_dev->priv = &ifx_ssc_isp[line];
    dma_dev->num_tx_chan  = DEFAULT_SSC_TX_CHANNEL_NUM;
    dma_dev->num_rx_chan  = DEFAULT_SSC_RX_CHANNEL_NUM; 
    dma_dev->tx_burst_len = DEFAULT_SSC_TX_BURST_LEN;
    dma_dev->rx_burst_len = DEFAULT_SSC_RX_BURST_LEN;
    dma_dev->port_tx_weight = 1;
    dma_dev->tx_endianness_mode = IFX_DMA_ENDIAN_TYPE0;
    dma_dev->rx_endianness_mode = IFX_DMA_ENDIAN_TYPE0;
    dma_dev->port_packet_drop_enable = 0;
    for(i = 0; i < dma_dev->num_tx_chan; i++){
        dma_dev->tx_chan[i]->desc_len = DEFAULT_SSC_TX_CHANNEL_DESCR_NUM;  
        dma_dev->tx_chan[i]->control = IFX_DMA_CH_ON;
        dma_dev->tx_chan[i]->packet_size = DEFAULT_SSC_FRAGMENT_SIZE;

    }
    for(i = 0; i < dma_dev->num_rx_chan; i++){
        dma_dev->rx_chan[i]->desc_len = DEFAULT_SSC_RX_CHANNEL_DESCR_NUM;
        dma_dev->rx_chan[i]->packet_size = DEFAULT_SSC_FRAGMENT_SIZE;
        dma_dev->rx_chan[i]->control = IFX_DMA_CH_ON;
        dma_dev->rx_chan[i]->byte_offset= 0;
    }
    dma_dev->current_tx_chan = 0;
    dma_dev->current_rx_chan = 0;
    /* 
     * set DMA handler functions for rx-interrupts, 
     * buffer allocation and release
     */
    dma_dev->intr_handler = ifx_ssc_dma_int_handler;
    dma_dev->buffer_alloc = ifx_ssc_dma_buffer_alloc;
    dma_dev->buffer_free  = ifx_ssc_dma_buffer_free;
    dma_device_register(dma_dev); 
    return 0;
}
#endif /* IFX_SSC_DMA_SUPPORT */

/**
 * \fn static INLINE void ifx_ssc_gpio_init(void)
 * \brief Reserve and initialize GPIO for SSC .
 *
 * \return none
 * \ingroup IFX_SSC_INTERNAL  
 */
static INLINE void 
ifx_ssc_gpio_init(void)
{
    ifx_gpio_register(IFX_GPIO_MODULE_SSC);
}

/**
 * \fn static INLINE void ifx_ssc_gpio_release(void)
 * \brief Release reserverd gpio resource so that other module could use it.
 * \return none
 * 
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void 
ifx_ssc_gpio_release(void)
{
    ifx_gpio_deregister(IFX_GPIO_MODULE_SSC);
}

/**
 * \fn static INLINE int ifx_ssc_rxtx_mode_set(struct ifx_ssc_port *port, unsigned int val)
 * \brief Rx/Tx mode set.
 * Set the transmission mode while SSC is idle
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   val     Rx/Tx mode
 * \return  0       OK
 * \return  -EINVAL Invalid parameters supplied 
 * \return  -EBUSY  Transmission or reception ongoing
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE int 
ifx_ssc_rxtx_mode_set(struct ifx_ssc_port *port, unsigned int val)
{
    u32 reg;

    if (!(port) || (val & ~(IFX_SSC_MODE_MASK))) {
        return -EINVAL;
    }

    /* check BUSY and RXCNT */
    if ((IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_BUSY)) {
        printk(KERN_ERR "%s state busy\n", __func__);
        return -EBUSY; 
    }
    if (IFX_SSC_RX_TO_RECEIVED(port)) {
        printk(KERN_ERR "%s rx todo busy\n", __func__);
        return -EBUSY;
    }
    IFX_SSC_IRQ_LOCK(port);
    reg = IFX_SSC_GET_CON(port);
    reg &= ~(IFX_SSC_CON_RX_OFF | IFX_SSC_CON_TX_OFF);
    reg |= val;
    IFX_SSC_SET_CON(reg, port);
    port->opts.modeRxTx = val;
    IFX_SSC_IRQ_UNLOCK(port);
    return 0;
} 

/**
 * \fn static int ifx_ssc_sethwopts(struct ifx_ssc_port *port)
 * \brief SSC set hardware options.
 * This routine intializes the SSC appropriately depending on slave/master and 
 * full-/half-duplex mode. It assumes that the SSC is disabled and the fifo's 
 * and buffers are flushed later on.
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  0       OK
 * \return  -EINVAL Invalid hardware options supplied
 * \ingroup IFX_SSC_INTERNAL 
 */
static int
ifx_ssc_sethwopts(struct ifx_ssc_port *port)
{
    unsigned long bits;
    u32 reg;
    struct ifx_ssc_hwopts *opts = &port->opts;

    /* sanity checks */
    if ((opts->dataWidth < IFX_SSC_MIN_DATA_WIDTH) ||
        (opts->dataWidth > IFX_SSC_MAX_DATA_WIDTH)) {
        printk(KERN_ERR "%s: sanity check failed\n", __func__);
        return -EINVAL;
    }
    bits = IFX_SSC_ENCODE_DATA_WIDTH(opts->dataWidth);
    bits |= IFX_SSC_CON_ENABLE_BYTE_VALID; /* TXB+2, TXB+3 trigger FPI Addr */

    if (opts->rxOvErrDetect)
        bits |= IFX_SSC_CON_RX_OFL_CHECK;
    if (opts->rxUndErrDetect)
        bits |= IFX_SSC_CON_RX_UFL_CHECK;
    if (opts->txOvErrDetect)
        bits |= IFX_SSC_CON_TX_OFL_CHECK;
    if (opts->txUndErrDetect)
        bits |= IFX_SSC_CON_TX_UFL_CHECK;
    if (opts->loopBack)
        bits |= IFX_SSC_CON_LOOPBACK_MODE;
    if (opts->echoMode)
        bits |= IFX_SSC_CON_ECHO_MODE_ON;
    if (opts->headingControl)
        bits |= IFX_SSC_CON_MSB_FIRST;
    if (opts->clockPhase)
        bits |= IFX_SSC_CON_PH;
    if (opts->clockPolarity)
        bits |= IFX_SSC_CON_PO;
    switch (opts->modeRxTx) {
        case IFX_SSC_MODE_TX:
            bits |= IFX_SSC_CON_RX_OFF;
            break;
        case IFX_SSC_MODE_RX:
            bits |= IFX_SSC_CON_TX_OFF;
            break;
    }
    IFX_SSC_IRQ_LOCK(port);
    IFX_SSC_SET_CON(bits, port);

    reg = (port->opts.gpoCs << IFX_SSC_GPOCON_ISCSB0_POS) |
            (port->opts.gpoInv << IFX_SSC_GPOCON_INVOUT0_POS);
    
    IFX_SSC_SET_GPOCON(reg, port);

    reg = port->opts.gpoCs << IFX_SSC_WHBGPOSTAT_SETOUT0_POS;
    
    IFX_SSC_SET_FGPO(reg, port);

    if (opts->masterSelect) {
        reg = IFX_SSC_WHBSTATE_MASTER_MODE;
    }
    else {
        reg = IFX_SSC_WHBSTATE_SLAVE_MODE; 
    }
    IFX_SSC_SET_WHBSTATE(reg, port);
    port->tx_fifo_size = IFX_SSC_TX_FIFO_SIZE(port);
    port->rx_fifo_size = IFX_SSC_RX_FIFO_SIZE(port);
    IFX_SSC_IRQ_UNLOCK(port); 
    /* set up the port pins */
    ifx_ssc_gpio_init();
    return 0;
}

/**
 * \fn int ifx_ssc_cs_low(u32 pin)
 * \brief Chip select enable.
 * This function sets the given chip select for SSC0 to low.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_ssc_cs_low(u32 pin)
{
    int ret;
    u32 reg;

    struct ifx_ssc_port *port = &ifx_ssc_isp[0];
    
    if (pin > IFX_SSC_MAX_GPO_OUT) {
        ret = -EINVAL;
    }
    else {
        reg = (1 << (pin + IFX_SSC_WHBGPOSTAT_CLROUT0_POS));
        IFX_SSC_SET_FGPO(reg, port);
        smp_wmb(); 
        ret = 0;
    }
    return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_low);

/**
 * \fn int ifx_ssc_cs_high(u32 pin)
 * \brief Chip select disable.
 * This function sets the given chip select for SSC0 to high.
 *
 * \param   pin      Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_ssc_cs_high(u32 pin)
{
    int ret;
    u32 reg;
    
    struct ifx_ssc_port *port = &ifx_ssc_isp[0];

    if (pin > IFX_SSC_MAX_GPO_OUT) {
        ret = -EINVAL;
    }
    else {
        udelay(1); /* XXX, at least half of cycle of baudrate delay is needed */
        reg = 1 << (pin + IFX_SSC_WHBGPOSTAT_SETOUT0_POS);
        IFX_SSC_SET_FGPO(reg, port);
        smp_wmb(); 
        ret = 0;
    }
    return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_high);

/**
 * \fn static void ifx_ssc_start_rxfifo(struct ifx_ssc_port *port)
 * \brief Start FIFO data reception.
 * This function processes received data. It will read data from the FIFO
 *
 * \param   port     Pointer to structure #ifx_ssc_port
 * \return  none
 * Description:
 * In Tx/Rx mode, to void memory copy, where rx data starts must be determined
 * To this end, rx_start_position will trace rx data starting position. It involves 
 * several special cases
 * 1) If txsize is divisable by 4, all tx data will be skipped.
 * 2) If txsize is not divisable by 4,including less than 4 bytes. The remaining 1~3 bytes
 *    have to do swap.
 * \ingroup IFX_SSC_INTERNAL 
 */
static void
ifx_ssc_start_rxfifo(struct ifx_ssc_port *port)
{
    int fifo_fill_lev, bytes_in_buf, i, j;
    unsigned long tmp_val;
    unsigned int rx_valid_cnt;
    unsigned long tx_dummy = 0;
    ifx_ssc_rxd_t rxd_data;

    i = port->actual_tx_len & 0x3;
    /* Number of words waiting in the RX FIFO */
    fifo_fill_lev = IFX_SSC_RX_FIFO_FILL_LEVEL(port);
    // Note: There are always 32 bits in a fifo-entry except for the last 
    // word of a contigous transfer block and except for not in rx-only 
    // mode and CON.ENBV set. But for this case it should be a convention 
    // in software which helps:
    // In tx or rx/tx mode all transfers from the buffer to the FIFO are 
    // 32-bit wide, except for the last three bytes, which could be a 
    // combination of 16- and 8-bit access.
    // => The whole block is received as 32-bit words as a contigous stream, 
    // even if there was a gap in tx which has the fifo run out of data! 
    // Just the last fifo entry *may* be partially filled (0, 1, 2 or 3 bytes)!

    bytes_in_buf = port->txrx_len;
    /* transfer with 32 bits per entry */
    while ((bytes_in_buf >= 4) && (fifo_fill_lev > 0)) {
        /* Skip the first tx parts which are divisible by 4  */
        if (port->tx_counter_for_rx > 0) {
            tx_dummy = IFX_SSC_GET_RX_WORD(port);
            port->tx_counter_for_rx--;
            port->rx_start_position += 4;
        }
        else if (i != 0) { /* last 1 ~ 3 byte belongs to rx */
            tmp_val = IFX_SSC_GET_RX_WORD(port);
            for (j = 0; j < 4 - i;  j++) {
                *port->rxbuf_ptr = (tmp_val >> (8 * ( 4 - i - j - 1))) & 0xff;
                port->rxbuf_ptr++;
            }
            port->rx_start_position += i;
        }
        else {
            *(u32 *) port->rxbuf_ptr = IFX_SSC_GET_RX_WORD(port);
            port->rxbuf_ptr += 4;
        }
        fifo_fill_lev--;
        bytes_in_buf -= 4;
        port->txrx_len -= 4;
    }

    /* Now do the rest as mentioned in STATE.RXBV */
    while ((bytes_in_buf > 0) && (fifo_fill_lev > 0)) {
        /* 
         * With high baudrate, RXVB always report 4 bytes for incomplete word
         * For 3 bytes 
         */
        if ((port->baudrate > IFX_SSC_SPLIT_BAUD_RATE) && (bytes_in_buf == 3)) {
            rxd_data.rxd_word = IFX_SSC_GET_RX_WORD(port);
            for (i = 0; i < 2; i++) {
                if (port->rx_start_position >= port->actual_tx_len) {
                    *port->rxbuf_ptr = rxd_data.rxd_byte.byte[2 + i];
                    port->rxbuf_ptr++;
                }
                else {
                    port->rx_start_position++;
                }
                bytes_in_buf--;
                port->txrx_len--;
            }
            rxd_data.rxd_word = IFX_SSC_GET_RX_WORD(port);
            if (port->rx_start_position >= port->actual_tx_len) {
                *port->rxbuf_ptr = rxd_data.rxd_byte.byte[3];
                port->rxbuf_ptr++;
            }
            else {
                port->rx_start_position++;
            }
            bytes_in_buf--;
            port->txrx_len--;
            break;
        }
        else {
            rx_valid_cnt = IFX_SSC_RX_VALID_BYTES(port);
            if (rx_valid_cnt == 0)
                break;
            if (rx_valid_cnt > bytes_in_buf) {
                // ### TO DO: warning message: not block aligned data, other data 
                // in this entry will be lost
                rx_valid_cnt = bytes_in_buf;
            }
            tmp_val = IFX_SSC_GET_RX_WORD(port);
            for (i = 0; i < rx_valid_cnt; i++) {
                if (port->rx_start_position >= port->actual_tx_len) {
                    *port->rxbuf_ptr = (tmp_val >> (8 * (rx_valid_cnt - i - 1))) & 0xff;
                    port->rxbuf_ptr++;
                }
                else {
                    port->rx_start_position++;
                }
                bytes_in_buf--;
                port->txrx_len--;
            }
        }    
    }
    if ((port->opts.modeRxTx == IFX_SSC_MODE_RX) &&
        IFX_SSC_RX_TO_RECEIVED(port) == 0) {
        u32 reg;
        
        if (port->txrx_len < IFX_SSC_RXREQ_BLOCK_SIZE) {
            reg = SM(port->txrx_len, IFX_SSC_RXREQ_RXCOUNT);
        }
        else {
            reg = SM(IFX_SSC_RXREQ_BLOCK_SIZE, IFX_SSC_RXREQ_RXCOUNT);
        }
        IFX_SSC_SET_RXREQ(reg, port);
    }
}

/**
 * \fn static void ifx_ssc_start_txfifo(struct ifx_ssc_port *port)
 * \brief Start FIFO data transmision.
 * This function copies remaining data in the transmit buffer into the FIFO 
 *
 * \param   port     Pointer to structure #ifx_ssc_port
 * \return  none
 * Description:
 * If txsize is not equal to zero, ssc driver will generate dummy data according
 * to different cases.
 * If txsize is equal to zero, just send dummy data whose length is equal to 
 * rxsize for clock generation.
 * \ingroup IFX_SSC_INTERNAL 
 */
static void
ifx_ssc_start_txfifo(struct ifx_ssc_port *port)
{
    int fifo_space, fill, i, j;
    u32 tx_dummy = 0;

    j = port->actual_tx_len & 0x3;

    fifo_space = port->tx_fifo_size - IFX_SSC_TX_FIFO_FILL_LEVEL(port);
    if (fifo_space == 0) {
        return;
    }

    fill = port->txbuf_end - port->txbuf_ptr;
    if (fill > fifo_space * 4)
        fill = fifo_space * 4;

    for (i = 0; i < fill / 4; i++) {
        if (port->tx_counter_for_tx > 0) {
            /* at first 32 bit access */
            IFX_SSC_TX_WORD(*(u32 *) port->txbuf_ptr, port);
            port->tx_counter_for_tx--;
            port->tx_end_position += 4;
            port->txbuf_ptr += 4;
        }
        else if (j != 0) { /* 1~3 bytes, tmp buffer to generate dummy data */
            int k;
            u8 tbuf[4] = {0};
            /* XXX, Seperate transmission doesn't work; combine data and dummy into 
             * into one 32bit data.
             */
            for (k = 0; k < j; k++) {
                tbuf[k] = *(u8*)port->txbuf_ptr;
                port->txbuf_ptr ++;
            }
            IFX_SSC_TX_WORD(*(u32 *) tbuf, port);
            port->txbuf_ptr += 4 - j; /* Totally move 4 bytes of address */
            port->tx_end_position += j;
        } 
        else {
            port->txbuf_ptr += 4;
            IFX_SSC_TX_WORD(tx_dummy, port);
        }
    }

    fifo_space -= fill / 4;
    fill &= 0x3;
    if ((fifo_space > 0) && (fill > 1)) {
        if (port->tx_end_position < port->actual_tx_len) {
            if ((port->actual_tx_len - port->tx_end_position) >= 2) {
                /* trailing 16 bit access */
                IFX_SSC_TX_HALFWORD(*(u16 *) port->txbuf_ptr, port);
                port->tx_end_position += 2;
            }
            else {
                u8 tbuf[2] = {0};
                /* XXX, combine into 16 bit data */
                tbuf[0] = *(u8 *) port->txbuf_ptr;
                IFX_SSC_TX_HALFWORD(*(u16*)tbuf, port);
                port->tx_end_position += 1;
            }
        }
        else {
            IFX_SSC_TX_HALFWORD((u16)tx_dummy, port); 
        }
        port->txbuf_ptr += 2;
        fill -= 2;
        fifo_space--; 
    }

    if ((fifo_space > 0) && (fill > 0)) {
        if (port->tx_end_position < port->actual_tx_len) {
            /* trailing 8 bit access */
            IFX_SSC_TX_BYTE(*(u8 *) port->txbuf_ptr, port);
            port->tx_end_position += 1;
        }
        else {
            IFX_SSC_TX_BYTE((u8)tx_dummy, port);
        }
        port->txbuf_ptr++;
        fifo_space--;
        fill -= 1;
    }
}

/**
 * \fn static ssize_t ifx_ssc_rxfifo_prepare(struct ifx_ssc_port *port, char *rxbuf, size_t rxsize)
 * \brief SSC Rx FIFO operation preparation.
 *
 * \param   port    Pointer to structure ## struct ifx_ssc_port
 * \param   rxbuf   Pointer to receive buffer
 * \param   rxsize  Length of receive buffer
 * \return  >0      Number of bytes received
 * \return  -EFAULT Invalid txrx mode
 * \return  -EIO    RX over flow
 * \ingroup IFX_SSC_INTERNAL 
 */
static ssize_t
ifx_ssc_rxfifo_prepare(struct ifx_ssc_port *port, char *rxbuf, size_t rxsize)
{
    unsigned long flags;
    ssize_t ret_val;

    if (port->opts.modeRxTx == IFX_SSC_MODE_TX) {
        return -EFAULT;
    }
    
    spin_lock_irqsave(&port->ssc_irq_lock, flags);
    port->rxbuf_ptr = rxbuf;
    port->rxbuf_end = rxbuf + rxsize;
    port->rx_start_position = 0; /* Every transaction, initialized once */
    port->tx_end_position = 0;
    /* TXRX in poll mode, rxsize and txsize determines when the transaction will end */
    while (port->txrx_len > 0) {
        if (port->txbuf_ptr < port->txbuf_end) {
            ifx_ssc_start_txfifo(port);
        }
        ifx_ssc_start_rxfifo(port);

        /* Check if any fatal error occurred */
        if ((IFX_SSC_GET_STATE(port) & (IFX_SSC_STATE_RX_OFL))) {
            spin_unlock_irqrestore(&port->ssc_irq_lock, flags);
            printk(KERN_ERR "%s RX OverFlow, please reduce baudrate\n", __func__);
            return -EIO; 
        }        
    }
    ret_val = port->rxbuf_ptr - rxbuf;
    spin_unlock_irqrestore(&port->ssc_irq_lock, flags);
    
    return (ret_val);
}

/**
 * \fn static ssize_t ifx_ssc_txfifo_prepare (struct ifx_ssc_port *port, const char *txbuf, size_t txsize)
 * \brief SSC Tx FIFO operation preparation.
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   txbuf  Pointer to receive buffer
 * \param   txsize  Length of receive buffer
 * \return  >0      Number of bytes received
 * \return  -EFAULT Invalid txrx mode 
 * \ingroup IFX_SSC_INTERNAL 
 */
static ssize_t
ifx_ssc_txfifo_prepare (struct ifx_ssc_port *port, const char *txbuf, size_t txsize)
{
    if (port->opts.modeRxTx == IFX_SSC_MODE_RX) {
        return -EFAULT;
    }

    IFX_SSC_IRQ_LOCK(port);
    port->txbuf_ptr = (char *)txbuf;
    port->txbuf_end = (char *)txbuf + txsize;

    /* Start the transmission */
    if (port->opts.modeRxTx == IFX_SSC_MODE_TX) {
        ifx_ssc_start_txfifo(port);
    }
    IFX_SSC_IRQ_UNLOCK(port);
    return txsize;
}

/**
 * \fn static int ifx_ssc_set_baudrate (struct ifx_ssc_port *port, unsigned int baudrate)
 * \brief SSC set baudrate.
 * Sets the baudrate of the corresponding port according to the passed
 * rate after reading out the current module speed.
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \param   baudrate  Desired baudrate
 * \return  0       OK
 * \return  -EINVAL Could not retrieve system clock or invalid baudrate setting
 * \ingroup IFX_SSC_INTERNAL 
 */
static int
ifx_ssc_set_baudrate (struct ifx_ssc_port *port, unsigned int baudrate)
{
    unsigned int ifx_ssc_clock;
    unsigned int br;
    int enabled;
    
    ifx_ssc_clock = ifx_ssc_get_kernel_clk(port);
    if (ifx_ssc_clock == 0) {
        return -EINVAL;
    }

    /* Baud rate or kernel clock change needs to calculate the new baud rate */
    if ((port->prev_baudrate == baudrate) && (port->prev_ssc_clk == ifx_ssc_clock)) 
        return 0;
#ifdef CONFIG_VR9
    if (ifx_is_vr9_a21_chip()) {
        /* VR9 A21 high baudrate support */
        u32 reg;
        
        if (baudrate > IFX_SSC_SPLIT_BAUD_RATE) {
            reg = IFX_REG_R32(IFX_GPHY1_CFG);
            reg &= ~IFX_SSC_HIGH_BAUD_DELAY_MASK;
            reg |= IFX_SSC_HIGH_BAUD_DELAY_THREE_CLOCK;
            IFX_REG_W32(reg, IFX_GPHY1_CFG);
        }
        else {
            reg = IFX_REG_R32(IFX_GPHY1_CFG);
            reg &= ~IFX_SSC_HIGH_BAUD_DELAY_MASK;
            IFX_REG_W32(reg, IFX_GPHY1_CFG);
        }
    }
#endif /* CONFIG_VR9 */

    /* Compute divider */
    br = (((ifx_ssc_clock >> 1) + baudrate / 2) / baudrate) - 1;
    if (br > 0xffff ||((br == 0) &&
        (IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_MASTER))) {
        printk(KERN_ERR "%s: illegal baudrate %u br %d\n", __func__, baudrate, br);
        return -EINVAL;
    }    
    IFX_SSC_IRQ_LOCK(port);
    /* Have to disable the SSC to set the baudrate */
    enabled = (IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_ENABLED) ? 1: 0;

    IFX_SSC_CONFIG_MODE(port);

    IFX_SSC_SET_BR(br, port);
    smp_wmb();
    if (enabled) {
        IFX_SSC_RUN_MODE(port);
    }
    IFX_SSC_IRQ_UNLOCK(port);
    port->prev_baudrate = baudrate;
    port->baudrate = baudrate;
    port->prev_ssc_clk = ifx_ssc_clock;
    return 0;
}

/**
 * \fn static int ifx_ssc_hwinit (struct ifx_ssc_port *port)
 * \brief SSC hardware initialization.
 * Initializes the SSC port hardware with the desired baudrate and transmission
 * options.
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  0       OK
 * \return  -EINVAL Error during initialization
 * \ingroup IFX_SSC_INTERNAL 
 */
static int
ifx_ssc_hwinit (struct ifx_ssc_port *port)
{
    u32 reg;
    int enabled;

    /* have to disable the SSC */
    enabled = (IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_ENABLED) ? 1: 0;

    IFX_SSC_CONFIG_MODE(port);
    if (ifx_ssc_sethwopts(port) < 0) {
        printk(KERN_ERR "%s: setting the hardware options failed\n", __func__);
        return -EINVAL;
    }

    if (ifx_ssc_set_baudrate(port, port->baudrate) < 0) {
        printk(KERN_ERR "%s: setting the baud rate failed\n", __func__);
        return -EINVAL;
    }
    IFX_SSC_IRQ_LOCK(port);
    
    /* TX FIFO filling level, half of FIFO size */
    reg = SM((port->tx_fifo_size >> 1), IFX_SSC_XFCON_ITL) |
           IFX_SSC_XFCON_FIFO_FLUSH  | IFX_SSC_XFCON_FIFO_ENABLE;
    IFX_SSC_TX_FIFO_CTRL(reg, port);
    
    /* RX FIFO filling level, half of FIFO size */
#ifdef IFX_SSC_DMA_SUPPORT
    /* Change to DMA RX trigger level to 2 to trigger DMA earlier */
    reg = SM( 2 /*(port->rx_fifo_size - 2)*/, IFX_SSC_XFCON_ITL);
#else
    reg = SM((port->rx_fifo_size >> 1), IFX_SSC_XFCON_ITL);
#endif
    reg |= IFX_SSC_XFCON_FIFO_FLUSH  | IFX_SSC_XFCON_FIFO_ENABLE;
    IFX_SSC_RX_FIFO_CTRL(reg, port);
    if (enabled) {
        IFX_SSC_RUN_MODE(port);
    }
    IFX_SSC_IRQ_UNLOCK(port);
    return 0;
}

/** 
 * \fn static int ifx_ssc_txrx_fifo(struct ifx_ssc_port *port, char *txbuf, u32 txsize, char *rxbuf, u32 rxsize)
 * \brief Called to transmit/receive to/from SSC in one step using FIFO mode.  
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \param   rxbuf     Pointer to store the received data packet
 * \param   rxsize     Amount of Bytes to receive.
 * \return  >= 0       Number of bytes received (if rxbuf != 0) or transmitted
 * \return  < 0        error number 
 * \ingroup IFX_SSC_INTERNAL 
 */
static int
ifx_ssc_txrx_fifo(struct ifx_ssc_port *port, char *txbuf, u32 txsize, char *rxbuf, u32 rxsize)
{
    int ret = 0;
    int eff_size = 0;

    if (txbuf == NULL || txsize == 0) {
        if (rxbuf != NULL && rxsize != 0) {
             eff_size = rxsize;
        }
    }
    else if (rxbuf == NULL || rxsize == 0) {
        if (txbuf != NULL && txsize != 0) {
             eff_size = txsize;
        }
    }
    else {
         eff_size = txsize + rxsize;
    }

    port->actual_tx_len = txsize;
    port->actual_rx_len = rxsize;
    port->tx_counter_for_rx = txsize >> 2;
    port->tx_counter_for_tx = txsize >> 2;
    port->txrx_len = eff_size;  
    /* Both tx/rx transmitted or received the txrx_len for full duplex mode */
    ret = ifx_ssc_txfifo_prepare(port, txbuf, eff_size);
    if ( ret != eff_size ){
        printk(KERN_ERR "ifx_ssc_txfifo_prepare return %d\n", ret); 
        goto txrx_exit;
    }

    ret = ifx_ssc_rxfifo_prepare(port, rxbuf, eff_size);
    if (ret != rxsize){
        printk(KERN_ERR "ifx_ssc_rxfifo_prepare return %d\n", ret); 
        goto txrx_exit;
    }
    ret = eff_size;
txrx_exit:
    if (ret < 0) {
        printk(KERN_ERR "%s failed\n", __func__);
    }
    return ret;
}

/**
 * \fn static INLINE int ifx_ssc_txfifo(struct ifx_ssc_port *port, char *txbuf, u32 txsize)
 * \brief Called to transmit data to SSC using FIFO mode, wrapped from ifx_ssc_txrx_fifo.
 * \param   port       Pointer to structure #ifx_ssc_port  
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number 
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE int 
ifx_ssc_txfifo(struct ifx_ssc_port *port, char *txbuf, u32 txsize)
{
    return ifx_ssc_txrx_fifo(port, txbuf, txsize, NULL, 0);
}

/**
 * \fn static INLINE int ifx_ssc_rxfifo(struct ifx_ssc_port *port, char *rxbuf, u32 rxsize)
 * \brief   Called to receive from SSC using FIFO mode, wrapped from ifx_ssc_txrx_fifo
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   rxbuf     Pointer to store the received data packet
 * \param   rxsize     Amount of Bytes to receive.
 * \return  >= 0       Number of bytes received
 * \return  < 0        error number 
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE int 
ifx_ssc_rxfifo(struct ifx_ssc_port *port, char *rxbuf, u32 rxsize)
{
    return ifx_ssc_txrx_fifo(port, NULL, 0, rxbuf, rxsize);
}

/**
 * \fn static INLINE int ifx_ssc_set_spi_mode(ssc_device_t *dev)
 * \brief SSC set ssc_ mode 
 * Sets the spi mode of the corresponding device. SSC mode is per device 
 *      parameter. It is initialized during registeration
 *
 * \param   dev    Pointer to device
 * \return  0       OK
 * \return  -EBUSY  could not set ssc mode because the system is busy
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE  int 
ifx_ssc_set_spi_mode(ssc_device_t *dev)
{
    u32 reg;
    int val = IFX_SSC_CON_PH;

    IFX_SSC_CONFIGURE_t *ssc_cfg;
    struct ifx_ssc_port *port;

    ssc_cfg = &dev->conn_id;
    port = dev->port;

    if (port->prev_ssc_mode == ssc_cfg->ssc_mode) 
        return 0;

    if ((IFX_SSC_GET_STATE(port) & IFX_SSC_STATE_BUSY)
        || IFX_SSC_RX_TO_RECEIVED(port) > 0) {
        printk(KERN_ERR "%s failed to set spi mode\n", __func__);
        return -EBUSY;
    }
    switch(ssc_cfg->ssc_mode) {
        case IFX_SSC_MODE_0:
            val = IFX_SSC_CON_PH;
            break;
            
        case IFX_SSC_MODE_1:
            val = 0;
            break;
            
        case IFX_SSC_MODE_2:
            val = IFX_SSC_CON_PO | IFX_SSC_CON_PH;
            break;
            
        case IFX_SSC_MODE_3:
            val = IFX_SSC_CON_PO;
            break;
            
        default:
            break;
    }
    IFX_SSC_IRQ_LOCK(port);

    IFX_SSC_CONFIG_MODE(port);

    reg = IFX_SSC_GET_CON(port);
    reg &= ~(IFX_SSC_CON_PO | IFX_SSC_CON_PH);
    reg |= (val);
    smp_wmb();
    IFX_SSC_SET_CON(reg, port);
    smp_wmb();
    IFX_SSC_RUN_MODE(port);
    IFX_SSC_IRQ_UNLOCK(port);
    port->prev_ssc_mode = ssc_cfg->ssc_mode;
    return 0;
}

/**
 * \fn static INLINE void ifx_ssc_txrx_setup( struct ifx_ssc_port *port)
 * \brief SSC set txrx direction if using FIFO mode.
 * To use FIFO for transmission and reception. Firstly, rx/tx mode muse be set
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void 
ifx_ssc_txrx_setup( struct ifx_ssc_port *port)
{
    u32 reg;
    
    /* Disable DMA for TX and RX */
    IFX_SSC_IRQ_LOCK(port);
    reg = IFX_SSC_GET_DMA_CON(port);
    reg &= ~(IFX_SSC_DMACON_TXON | IFX_SSC_DMACON_RXON);
    IFX_SSC_SET_DMA_CON(reg, port);
    IFX_SSC_IRQ_UNLOCK(port);
    if (port->opts.modeRxTx != IFX_SSC_MODE_RXTX) {
        ifx_ssc_rxtx_mode_set(port, IFX_SSC_MODE_RXTX);
    }
}

#ifdef IFX_SSC_DMA_SUPPORT
/**
 * \fn static INLINE void ifx_ssc_tx_setup( struct ifx_ssc_port *port)
 * \brief SSC set Tx mode 
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void 
ifx_ssc_tx_setup( struct ifx_ssc_port *port)
{
    if (port->opts.modeRxTx != IFX_SSC_MODE_TX) {
        ifx_ssc_rxtx_mode_set(port, IFX_SSC_MODE_TX);
    }
}

/**
 * \fn static INLINE void ifx_ssc_rx_setup( struct ifx_ssc_port *port)
 * \brief SSC set Rx mode 
 *
 * \param   port    Pointer to structure #ifx_ssc_port
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void 
ifx_ssc_rx_setup( struct ifx_ssc_port *port)
{
    if (port->opts.modeRxTx != IFX_SSC_MODE_RX) {
        ifx_ssc_rxtx_mode_set(port, IFX_SSC_MODE_RX);
    }
}
#endif /* IFX_SSC_DMA_SUPPORT */

/**
 * \fn static void ifx_ssc_enqueue(IFX_SSC_QUEUE_t *queue)
 * \brief SSC add queue entry to priority queue
 *
 * \param   queue    Pointer to structure #IFX_SSC_QUEUE_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static void 
ifx_ssc_enqueue(IFX_SSC_QUEUE_t *queue)
{
    ssc_device_t     *dev;
    struct ifx_ssc_port *port;
    
    IFX_KASSERT((queue != NULL), ("%s should never happen\n", __func__));
    dev = queue->dev;
    port = dev->port;
    IFX_SSC_PRINT(port, SSC_MSG_QUEUE, "%s dev %s prio %d enqueued\n",
        __func__, dev->dev_name, dev->dev_prio);
    IFX_SSC_Q_LOCK_BH(port);
    if (dev->dev_prio == IFX_SSC_PRIO_ASYNC) {
        TAILQ_INSERT_TAIL(&port->ssc_asyncq, queue, q_next);
    }
    else {
        TAILQ_INSERT_TAIL(&port->ssc_syncq[dev->dev_prio], queue, q_next);
    }
    dev->stats.enqueue++;
    IFX_SSC_Q_UNLOCK_BH(port);
}

/**
 * \fn static void ifx_ssc_dequeue(IFX_SSC_QUEUE_t *queue) 
 * \brief SSC remove queue entry from priority queue
 *
 * \param   queue    Pointer to structure #IFX_SSC_QUEUE_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static void
ifx_ssc_dequeue(IFX_SSC_QUEUE_t *queue) 
{
    ssc_device_t     *dev;
    struct ifx_ssc_port *port;
    
    IFX_KASSERT((queue != NULL), ("%s should never happen\n", __func__));
    dev = queue->dev;
    port = dev->port;
    IFX_SSC_PRINT(port, SSC_MSG_QUEUE, "%s dev %s prio %d dequeued\n",
        __func__, dev->dev_name, dev->dev_prio);
    IFX_SSC_Q_LOCK_BH(port);
    if (dev->dev_prio == IFX_SSC_PRIO_ASYNC) {
        if (!TAILQ_EMPTY(&port->ssc_asyncq)) {
            TAILQ_REMOVE(&port->ssc_asyncq, queue, q_next);
        }
    }
    else {
        if (!TAILQ_EMPTY(&port->ssc_syncq[dev->dev_prio])) {
            TAILQ_REMOVE(&port->ssc_syncq[dev->dev_prio], queue, q_next);
        }
    }
    dev->stats.dequeue++;
    IFX_SSC_Q_UNLOCK_BH(port);
}

/**
 * \fn static void ifx_ssc_cs_lock(ssc_device_t *dev)
 * \brief SSC chip select function, set spi mode, baudrate, call registered 
 *     device-specific cs set function.
 *
 * \param   dev    Pointer to structure #ssc_device_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static void 
ifx_ssc_cs_lock(ssc_device_t *dev)
{
    IFX_SSC_CONFIGURE_t *ssc_cfg;
    struct ifx_ssc_port *port;
    
    port = dev->port;
    if (port->ssc_cs_locked == IFX_TRUE) {
        printk(KERN_ERR "Fatal error: %s locked already before\n", __func__);
        return;
    }    
    IFX_SSC_PRINT(port, SSC_MSG_LOCK, "%s %s enter\n", __func__, dev->dev_name);

    port->ssc_cs_locked = IFX_TRUE;
    ssc_cfg = &dev->conn_id;
    ifx_ssc_set_spi_mode(dev);
    ifx_ssc_set_baudrate(port, ssc_cfg->baudrate);
    if (ssc_cfg->csset_cb != NULL) {
        ssc_cfg->csset_cb(IFX_SSC_CS_ON, ssc_cfg->cs_data);
    }
}

/**
 * \fn static INLINE void ifx_ssc_cs_unlock(ssc_device_t *dev)
 * \brief SSC chip un select function,  call registered device-specific cs reset function.
 *
 * \param   dev    Pointer to structure #ssc_device_t
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void
ifx_ssc_cs_unlock(ssc_device_t *dev)
{
    IFX_SSC_CONFIGURE_t *ssc_cfg;
    struct ifx_ssc_port *port;
    
    port = dev->port;
    if (port->ssc_cs_locked == IFX_FALSE) {
        printk(KERN_ERR "Fatal error: %s unlocked already before\n", __func__);
        return;
    }       
    ssc_cfg = &dev->conn_id;
    if(ssc_cfg->csset_cb != NULL) {
        ssc_cfg->csset_cb(IFX_SSC_CS_OFF, ssc_cfg->cs_data);
    }
    port->ssc_cs_locked = IFX_FALSE;
    IFX_SSC_PRINT(port, SSC_MSG_LOCK, "%s %s exit\n", __func__, dev->dev_name);
}

/**
 *\fn int ifx_sscLock(IFX_SSC_HANDLE handler)
 *\brief  Called to lock and reserve the whole SSC interface 
 * for the given 'handler'
 *
 * The chipselect, belonging to this SSC session is already 
 * activated. This means the chipselect callback is called. 
 * After complete data transmission and reception, ifx_sscUnLock 
 * has to be called to release the SSC interface again for 
 * other clients.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \return  0          Locked succesfully
 * \return  -1         Failed to lock the ssc bus
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscLock(IFX_SSC_HANDLE handler)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg;
    struct ifx_ssc_port *port;
    IFX_SSC_QUEUE_t     *pqueue;

    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    pqueue = &dev->queue;
    ssc_cfg = &dev->conn_id;
    port = dev->port;
    
    if (port->lock_qentry == pqueue) {
        /* We hold the lock already -> nothing to request here! */
        return (-1);
    }
 
    /* 
     * Check if the queue entry of the ConnId is already queued with a request 
     * but this request is not served yet. Every ConnId can only queue up one 
     * request at one time. 
     */
    if (atomic_read(&pqueue->isqueued) == 1) {
        return (-1);
    }

   /* Initialize unused variable */
    pqueue->txbuf          = NULL;
    pqueue->txsize         = 0;
    pqueue->rxbuf          = NULL;
    pqueue->rxsize         = 0;
    
    /* Place a lock request in the queue */
    pqueue->request_lock = IFX_TRUE;

    atomic_set(&pqueue->isqueued, 1);
 
    /* Add queue entry to priority queue */
    ifx_ssc_enqueue(pqueue);
 
    /* 
     * If no async support is avaiable, trigger the SSC kernel thread and
     * wait pending till job is done. 
     */
#ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
    IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s raise fake interrupt\n", __func__);
    ifx_ssc_start_tasklet(port);
#else
    IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s wake up ssc kernel thread\n", __func__);
    /* Kick off SSC kernel thread */
    IFX_SSC_WAKEUP_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT, port->event_flags);
#endif
    /* Wait till wake up from SSC kernel thread */
    IFX_SSC_WAIT_EVENT(dev->dev_thread_wait, IFX_SSC_DEV_THREAD_EVENT, dev->event_flags); 
    IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s wakeup received from ssc kernel thread\n", __func__);
    return 0; 
}
EXPORT_SYMBOL(ifx_sscLock);

/**
 *\fn int ifx_sscUnlock(IFX_SSC_HANDLE handler)
 * \brief  This function releases the SSC lock that was placed before by calling ifx_SscLock.
 *
 * This function also inactivate the chipselect signal, which was set in ifx_SscLock.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \return  0          UnLocked succesfully
 * \return  -1         Failed to unlock the ssc bus
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscUnlock(IFX_SSC_HANDLE handler)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg = NULL;
    struct ifx_ssc_port *port = NULL;
    IFX_SSC_QUEUE_t     *pqueue;
    
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    pqueue = &dev->queue;
    ssc_cfg = &dev->conn_id;
    port = dev->port;
    
    if (port->lock_qentry != &dev->queue) {
        /* We do not hold the lock, therefore we can not release it! */
        return -1;
    }

   /* Just forget about the lock, then the SSC driver would just take it
    * as a normel queue entry 
    */    
    ifx_ssc_cs_unlock(dev);
    pqueue->txbuf          = NULL;
    pqueue->txsize         = 0;
    pqueue->rxbuf          = NULL;
    pqueue->rxsize         = 0;
    port->lock_qentry = NULL;
    return 0;
}
EXPORT_SYMBOL(ifx_sscUnlock);

/**
 * \fn int ifx_sscSetBaud(IFX_SSC_HANDLE handler, unsigned int baud)
 * \brief Configures the Baudrate of a given connection.
 *
 * The baudrate can also be change multiple times 
 * for a single connection. The baudrate change 
 * will take place for the next call of ifx_SscTx, 
 * ifx_SscRx or ifx_SscTxRx.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   baud       Baudrate to configure. This value can be rounded
 *                     during the calculation of the SSC clock divider
 *
 * \return  = 0        OK
 * \return  < 0        error number 
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscSetBaud(IFX_SSC_HANDLE handler, unsigned int baud)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg;

    /* Sanity check */
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    ssc_cfg = &dev->conn_id;
    /* XXX, protection because of being used in other places */
    ssc_cfg->baudrate = baud;
    return 0;
}
EXPORT_SYMBOL(ifx_sscSetBaud);

#ifdef IFX_SSC_DMA_SUPPORT
/**
 * \fn static int ifx_ssc_txdma(struct ifx_ssc_port *port, char *txbuf, int txsize)
 * \brief Called to transmit the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   txbuf      Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number
 * \ingroup IFX_SSC_INTERNAL 
 */
static int
ifx_ssc_txdma(struct ifx_ssc_port *port, char *txbuf, int txsize)
{
    int i, j;
    int retval = 0;
    int k, m;
    char *p;
    char *pbuf;
    struct dma_device_info* dma_dev;

    ifx_ssc_dma_setup(port, IFX_SSC_DIR_TX, IFX_SSC_DMA_ENABLE);

    /*
     * HW WAR, last 1~3 byte must do swapping, but the wanted bytes could be 
     * out of orignal tx buffer, so pre-allocated buffer used, and what's 
     * more, memcpy involved.
     * XXX, actually, potential issues include data length which is a multiple of DMA
     * burst length.
     */
    i = txsize >> 2;  /* divided by 4 */
    j = txsize & 0x3; /* modulo */
    if (j != 0) { /* Hit last 1~3 byte case */
        memcpy(port->dma_txbuf, txbuf, txsize);
        p = port->dma_txbuf + (i << 2);
        for (m = 0, k = j - 1; k >= 0; k--, m++) {
            p[4 - m - 1] = p[k]; /* Possibly out of range if no memory copy */
        } 
        pbuf = port->dma_txbuf;
    }
    else {
        pbuf = txbuf;
    }

    dma_dev = port->dma_dev;

    /** 
     * Set a flag that we are waiting for the DMA to complete. This flag
     * will be reseted again in the DMA interrupt. 
     * NB, it must be ahead of the following stuff, because once descriptor
     * is prepared, interrupt may come back immediately
     */
    atomic_set(&port->dma_wait_state, 1);
    
    /**
     * Run in tasklet or kernel thread, DMA tasklet may run the same function
     * Lock must be used.
     */
    IFX_SSC_IRQ_LOCK(port);
    retval = dma_device_write(dma_dev, pbuf, txsize, NULL);  
    IFX_SSC_IRQ_UNLOCK(port);
    IFX_KASSERT(retval == txsize, ("%s retval %d != txsize %d\n", 
        __func__, retval, txsize));

    return retval;
}

/**
 * \fn static int ifx_ssc_rxdma(struct ifx_ssc_port *port, char *rxbuf, int rxsize)
 * \brief Called to receive the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_ssc_port
 * \param   rxbuf      Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes received
 * \return   < 0       error number
 * \ingroup IFX_SSC_INTERNAL 
 */
static int
ifx_ssc_rxdma(struct ifx_ssc_port *port, char *rxbuf, int rxsize)
{
    u32 reg;
    char *pbuf;
    int   dma_rxsize;
    struct dma_device_info* dma_dev;
    dma_dev = port->dma_dev;

    /* 
     * Backup original buffer, so that later we can find it in dma handler
     * Borrow two variables from FIFO usage 
     */
    port->rxbuf_ptr = rxbuf;
    port->actual_rx_len = rxsize;

    /*
     * HW WAR, last 1~3 byte must do swapping, but the wanted bytes could be 
     * out of orignal rx buffer, so pre-allocated buffer used, and what's 
     * more, memcpy involved.
     * XXX, actually, potential issues include data length which is a multiple of DMA
     * burst length.     
     */
    if ((rxsize & 0x3)){ /* Can't be divisible by 4 */
        pbuf = port->dma_rxbuf;
        dma_rxsize = (rxsize & ~0x3) + 4; /* Round up one dword to make sure enough space */
        IFX_KASSERT(dma_rxsize <= DEFAULT_SSC_FRAGMENT_SIZE, ("%s fragment %d out of range\n",
            __func__, dma_rxsize));
    }
    else {
        pbuf = rxbuf;
        dma_rxsize = rxsize;
    }
    
    /* NB, DMA descriptoer must be setup before request counter */
    dma_device_desc_setup(dma_dev, pbuf, dma_rxsize);

    ifx_ssc_dma_setup(port, IFX_SSC_DIR_RX, IFX_SSC_DMA_ENABLE);

    /*
     * NB, Enable SPI DMA channel every time. There are two reasons as follow,
     * 1. In synchronous DMA application, there is no pre-allocation memory for
     *    SPI bus driver. When it registers with central DMA driver, it will not
     *    open RX channel for reception. So at least, we need to open channel once
     *    before we use it.
     * 2. We have to close this channel after we receive the packet since we have only
     *    one descriptor, and we can't clear the descriptor. This will be explained 
     *    further in close channel part.
     */
    (dma_dev->rx_chan[dma_dev->current_rx_chan])->open(dma_dev->rx_chan[dma_dev->current_rx_chan]);

    /* Set a flag that we are waiting for the DMA to complete. This flag
     * will be reseted again in the DMA interrupt. 
     */
    atomic_set(&port->dma_wait_state, 1);
    
    /* 
     * Set up request counter after DMA setting is ready,
     * Otherwise, receive overrun will happen.
     */
    IFX_SSC_IRQ_LOCK(port);
    if (rxsize < IFX_SSC_RXREQ_BLOCK_SIZE) {
        reg = SM(rxsize, IFX_SSC_RXREQ_RXCOUNT);
        /* At least, rxsize will cause some issues, maybe 
         * rxsize smaller than DMA burst len.
         */
    }
    else {
        reg = SM(IFX_SSC_RXREQ_BLOCK_SIZE, IFX_SSC_RXREQ_RXCOUNT);

    }
    smp_mb();
    IFX_SSC_SET_RXREQ(reg, port);
    
    IFX_SSC_IRQ_UNLOCK(port);

    return rxsize;
}
#endif /* IFX_SSC_DMA_SUPPORT*/

/**
 *\fn int ifx_sscTxRx (IFX_SSC_HANDLE handler, char *txbuf, u32 txsize, char *rxbuf, u32 rxsize)
 *\brief Called to transmit/receive to/from SSC in one step.
 
 * This means that the data transmission and reception is done in parallel.
 * No DMA is possible here. The SSC driver sets the chipselect when the 
 * data transmission starts and resets it when the transmission is 
 * completed. The transmit and receive buffer memory allocation and 
 * de-allocation is done by the SSC client.
 *
 * \param   handler   Handle of the connection where to make the 
 *                    configuration on
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize    Amount of Bytes to transmit
 * \param   rxbuf     Pointer to store the received data packet
 * \param   rxsize    Amount of Bytes to receive.
 * \return  >= 0      Number of bytes received (if rxbuf != 0) or transmitted
 * \return  < 0       error number 
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscTxRx(IFX_SSC_HANDLE handler, char *txbuf, u32 txsize, char *rxbuf, u32 rxsize)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg;
    IFX_SSC_QUEUE_t     *pqueue;
    struct ifx_ssc_port *port;
    struct ifx_ssc_device_stats *stats;

    /* Sanity check */
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    IFX_KASSERT(!((rxbuf == NULL) && (rxsize == 0) 
        && (txbuf == NULL)  && (txsize == 0)),  
        ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    stats = &dev->stats;
    port = dev->port;
    
    if (in_interrupt()) {
        stats->context_err++;
        printk(KERN_ERR "%s can't be called in interupt context< irq, softirq, tasklet>\n", __func__);
        return 0;
    }
    ssc_cfg = &dev->conn_id;
    pqueue = &dev->queue;

    if (txsize > ssc_cfg->fragSize || rxsize > ssc_cfg->fragSize) {
        stats->frag_err++;
        printk(KERN_ERR "%s Device driver must do its own fragmentation tx %d rx %d > %d\n", 
            __func__, txsize, rxsize, ssc_cfg->fragSize); 
        return 0;
    }
    
   /*
    * Ensure that only asynchronous SSC Handles could enqueue a 
    * synchronous request. The parameter 'handle_type' is set during the 
    * ConnId allocation process. 
    */
    if (pqueue->handle_type != IFX_SSC_HANDL_TYPE_SYNC) {
        stats->handler_err++;
        printk(KERN_ERR "%s must use sync handler\n", __func__);
        return 0;
    }

    /* 
     * Check if the queue entry of the ConnId is already queued with a request 
     * but this request is not served yet. Every ConnId can only queue up one 
     * request at one time. 
     */
    if (atomic_read(&pqueue->isqueued) == 1) {
        stats->dup_qentries++;
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s same queue has queued more than once\n", __func__);
        return 0;
    }
    /* Add pointer and sizes to the queue entry of this SSC handle. */
    pqueue->txbuf          = txbuf;
    pqueue->txsize         = txsize;
    pqueue->rxbuf          = rxbuf;
    pqueue->rxsize         = rxsize;
    atomic_set(&pqueue->isqueued, 1);
    pqueue->exchange_bytes = 0;
    pqueue->request_lock   = IFX_FALSE;
    memset(&pqueue->callback, 0, sizeof (IFX_SSC_ASYNC_CALLBACK_t));

    /* Add queue entry to priority queue */
    ifx_ssc_enqueue(pqueue);
    
    /* If no async support is avaiable, trigger the SSC kernel thread and
     * wait pending till job is done. 
     */
#ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
    IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s raise fake interrupt\n", __func__);
    ifx_ssc_start_tasklet(port);
#else
    IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s wake up ssc kernel thread\n", __func__);
    /* Kick off SSC kernel thread */
    IFX_SSC_WAKEUP_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT, port->event_flags);
#endif
    /* Wait till wakeup from SSC kernel thread */
    IFX_SSC_WAIT_EVENT(dev->dev_thread_wait, IFX_SSC_DEV_THREAD_EVENT, dev->event_flags);
    IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s wakeup event received from ssc kernel thread\n", __func__);

    /* Reset queue pointer */
    pqueue->txbuf = NULL;
    pqueue->rxbuf = NULL;
    return pqueue->txsize + pqueue->rxsize;
}
EXPORT_SYMBOL(ifx_sscTxRx);

/**
 *\fn int ifx_sscTx(IFX_SSC_HANDLE handler, char *txbuf, u32 txsize)
 *\brief Called to transmit the data.
 
 * transmission starts and resets it when the transmission
 * the transmit buffer is done by the SSC client.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number 
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscTx(IFX_SSC_HANDLE handler, char *txbuf, u32 txsize)
{
    return ifx_sscTxRx(handler, txbuf, txsize, NULL, 0);
}

EXPORT_SYMBOL(ifx_sscTx);

/**
 *\fn int ifx_sscRx(IFX_SSC_HANDLE handler, char *rxbuf, u32 rxsize)
 *\brief Called to receive the data.

 * The SSC driver sets the chipselect when the data reception starts and 
 * resets it when the reception is completed. The memory allocation and 
 * de-allocation of the receive buffer is done by the SSC client.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   rxbuf     Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes received
 * \return  < 0        error number 
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscRx(IFX_SSC_HANDLE handler, char *rxbuf, u32 rxsize)
{
    return ifx_sscTxRx(handler, NULL, 0, rxbuf, rxsize);
}
EXPORT_SYMBOL (ifx_sscRx);

/**
 * \fn static int ifx_ssc_serve_qentry(struct ifx_ssc_port *port)
 * \brief Called to serve every queue entry and it is a common function for
 * SSC kernel thread and tasklet 
 *
 * \param   port       Pointer to structure #ifx_ssc_port  
 * \return  1          Continue to loop this function until return 0
 * \return  0         Immediately exit this function. For kernel thread,
 *                     it will sleep, for tasklet, it will exit dynamically
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_serve_qentry(struct ifx_ssc_port *port)
{
    IFX_SSC_QUEUE_t  *qentry = NULL;
    ssc_device_t     *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg = NULL;
    struct dma_device_info *dma_dev;    
    
    /*
     * Don't serve the current or next queue entry in case we are currently 
     * waiting for the DMA interrupt to report the transmission completion. 
     */
    if (atomic_read(&port->dma_wait_state) == 1) {
        /* 
         * DMA interrupt will spawn tasklet or wake up kernel thread 
         * in order to continue. 
         */
        return 0;
    }
 
    /* Identify the queue entry to serve */
    if (port->serve_qentry != NULL) {
        /* Continues serving the queue that was served before */
        qentry = port->serve_qentry;
    }
    else if (port->lock_qentry != NULL) {
        /* 
         * If one queue holds the lock, only serve this one element and
         * ignore all others. 
         */
        qentry = port->lock_qentry;
        port->serve_qentry = qentry;
    }
    else {
        IFX_SSC_Q_LOCK_BH(port);
        if (!TAILQ_EMPTY(&port->ssc_asyncq)) {
            qentry = TAILQ_FIRST(&port->ssc_asyncq);
        }
        else {
            int  i;
            /* Choose the highest queue entry first */
            for (i = IFX_SSC_PRIO_HIGH; i >= IFX_SSC_PRIO_LOW; i--) {
                if (!TAILQ_EMPTY(&port->ssc_syncq[i])){
                    qentry = TAILQ_FIRST(&port->ssc_syncq[i]);
                    break;
                }
            }
        }
        IFX_SSC_Q_UNLOCK_BH(port);
        
        /* Remember that we are working on this queue now */
        port->serve_qentry = qentry;
        
        /* 
         * NB, Don't remove the qentry here since we may never come here once callers called
         * sequence. It may cause enqueue/dequeue mismatch
         */
    }
 
    /* No queue found that should be served */
    if (qentry == NULL) {
        return 0;
    }
 
    /* Get connection handle */
    dev = qentry->dev;
    ssc_cfg = &dev->conn_id;
    dma_dev = port->dma_dev;
    if (qentry->txbuf != NULL) {
    #ifdef IFX_SSC_DMA_SUPPORT   
        int tx_dma_aligned = ((((u32)qentry->txbuf) & ((dma_dev->tx_burst_len << 2) - 1)) == 0) ? 1 : 0;
    #endif /* IFX_SSC_DMA_SUPPORT */
        if ((port->ssc_cs_locked == IFX_FALSE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect set callback of the SSC-Handle */
            ifx_ssc_cs_lock(dev);
        }
   #ifdef IFX_SSC_DMA_SUPPORT     
        /* If buffer not aligned on DMA burst length, fall back to FIFO */
        if ((qentry->txsize > ssc_cfg->maxFIFOSize) && (tx_dma_aligned == 1)) {
            IFX_SSC_PRINT(port, SSC_MSG_TX_DMA, "%s TX DMA enter\n", __func__);
            ifx_ssc_tx_setup(port);
            ifx_ssc_txdma(port, qentry->txbuf, qentry->txsize);
            port->stats.txDma++;
            /* 
             * Reset the data pointer, because this data are done on the
             * SSC hardware. 
             */
            qentry->txbuf = NULL;
            /* Count the number of transmitted bytes for this queue entry */
            qentry->exchange_bytes = qentry->txsize;
            dev->stats.txBytes += qentry->txsize;
            port->stats.txBytes += qentry->txsize;
            port->stats.txDmaBytes += qentry->txsize;
            return 0;
        }
        else 
    #endif /* IFX_SSC_DMA_SUPPORT */
        {
            ifx_ssc_txrx_setup(port);
            ifx_ssc_txfifo(port, qentry->txbuf, qentry->txsize);
            port->stats.txFifo++;
            IFX_SSC_PRINT(port, SSC_MSG_TX_FIFO, "%s TX FIFO enter\n", __func__);

            /* 
             * Reset the data pointer, because this data are done on the
             * SSC hardware. 
             */
            qentry->txbuf = NULL;
            /* Count the number of transmitted bytes for this queue entry */
            qentry->exchange_bytes = qentry->txsize;
            dev->stats.txBytes += qentry->txsize;
            port->stats.txBytes += qentry->txsize;
            /* NB, Make sure data has been sent out */
            ifx_ssc_wait_finished(port);
            return 1;
        }
    }
    else if (qentry->rxbuf != NULL) {
    #ifdef IFX_SSC_DMA_SUPPORT  
        int rx_dma_aligned = ((((u32) qentry->rxbuf) & ((dma_dev->rx_burst_len << 2) - 1)) == 0) ? 1 : 0;
    #endif /* IFX_SSC_DMA_SUPPORT */
        if ((port->ssc_cs_locked == IFX_FALSE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect set callback of the SSC-Handle */
            ifx_ssc_cs_lock(dev);
        }
    #ifdef IFX_SSC_DMA_SUPPORT   
        /* If buffer not aligned on DMA burst length, fall back to FIFO */
        if ((qentry->rxsize > ssc_cfg->maxFIFOSize) && (rx_dma_aligned == 1)) {
            IFX_SSC_PRINT(port, SSC_MSG_RX_DMA, "%s RX DMA enter\n", __func__);
            ifx_ssc_rx_setup(port);
            ifx_ssc_rxdma(port, qentry->rxbuf, qentry->rxsize);
            port->stats.rxDma++;
          
            /* Reset the data pointer, because this data are done on the
             * SSC hardware.
             */
            qentry->rxbuf = NULL;
            /* Count the number of transmitted bytes for this queue entry */
            qentry->exchange_bytes += qentry->rxsize;
            dev->stats.rxBytes += qentry->rxsize;
            port->stats.rxBytes += qentry->rxsize;
            port->stats.rxDmaBytes += qentry->rxsize;
            return 0;
        }
        else 
    #endif /* IFX_SSC_DMA_SUPPORT */
        {
            ifx_ssc_txrx_setup(port);
            ifx_ssc_rxfifo(port, qentry->rxbuf, qentry->rxsize);
            port->stats.rxFifo++;
            IFX_SSC_PRINT(port, SSC_MSG_RX_FIFO, "%s RX FIFO enter\n", __func__);

            /* Reset the data pointer, because this data are done on the
             * SSC hardware. 
             */
            qentry->rxbuf = NULL;
            /* Count the number of recevied bytes for this queue entry */
            qentry->exchange_bytes += qentry->rxsize;
            dev->stats.rxBytes += qentry->rxsize;
            port->stats.rxBytes += qentry->rxsize;
            /* NB, Make sure data has been sent out */
            ifx_ssc_wait_finished(port);
            return 1;
        }
    }
    else if (qentry->request_lock) {
        /* A lock request found */
        IFX_SSC_PRINT(port, SSC_MSG_LOCK, "%s request lock enter\n", __func__);
        port->lock_qentry = qentry;
        qentry->request_lock = IFX_FALSE;
        
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        /* 
         * Reset the flag in the queue element that this one is queued with a 
         * request to be served. 
         */
        atomic_set(&qentry->isqueued, 0);

        /* syncAPI has the last watchdog branch but nested callback has to dequeue it */
        if (qentry->handle_type == IFX_SSC_HANDL_TYPE_ASYNC) {
            ifx_ssc_dequeue(qentry);
        }

        /* Call the Chipselect set callback of the SSC-Handle */
        ifx_ssc_cs_lock(dev);
  
        if (qentry->callback.pFunction) {
            /* 
             * Store the callback parameter local to cleanup the queue entry before 
             * calling the callback. 
             */
            IFX_SSC_ASYNC_CALLBACK_t callback = qentry->callback;
            
            qentry->callback.pFunction = NULL;
            IFX_SSC_PRINT(port, SSC_MSG_CALLBACK, "%s line%d request callback\n", __func__, __LINE__);
         
            callback.pFunction(callback.functionHandle, 0);
        }
        return 1;
    }
    else if (qentry->callback.pFunction != NULL) {
        /* 
         * Store the callback parameter local to cleanup the queue entry before 
         * calling the callback. 
         */
        IFX_SSC_ASYNC_CALLBACK_t callback = qentry->callback;
  
        qentry->callback.pFunction = NULL;
        
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        /* 
         * Reset the flag in the queue element that this one is queued with a 
         * request to be served. 
         */
        atomic_set(&qentry->isqueued, 0);
        
        ifx_ssc_dequeue(qentry);
        
        if ((port->ssc_cs_locked == IFX_TRUE) && (port->lock_qentry == NULL)) {            
            /* Call the Chipselect reset callback of the SSC-Handle */
            ifx_ssc_cs_unlock(dev);
        }

        IFX_SSC_PRINT(port, SSC_MSG_CALLBACK, "%s line%d direct callback\n", __func__, __LINE__);
      
        /* Callback to call */
        callback.pFunction(callback.functionHandle, 
                qentry->exchange_bytes);
        return 1;
    }
    else if (qentry->handle_type == IFX_SSC_HANDL_TYPE_SYNC) {
        if ((port->ssc_cs_locked == IFX_TRUE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect reset callback of the SSC-Handle */
            ifx_ssc_cs_unlock(dev);
        }

        /* Wake up the pending thread */
        IFX_SSC_WAKEUP_EVENT(dev->dev_thread_wait, IFX_SSC_DEV_THREAD_EVENT, dev->event_flags);
        IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s wakeup SSC client kernel thread\n", __func__);

        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        /* 
         * Reset the flag in the queue element that this one is queued with a 
         * request to be served. 
         */
        atomic_set(&qentry->isqueued, 0);
        
        ifx_ssc_dequeue(qentry);
        /* syncAPI has no callback, we have to differentiate it */
        if (port->ssc_cs_locked == IFX_TRUE) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else if ((qentry->handle_type == IFX_SSC_HANDL_TYPE_ASYNC) && (port->ssc_cs_locked == IFX_TRUE)) {
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        return 0; /* Don't let it continue, but must restart tasklet in somewhere else */
    }
    return 0;
}

#ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
/**
 * \fn static INLINE void ifx_ssc_tasklet_serve_queue(struct ifx_ssc_port *port)
 * \brief Called to serve every queue entry in tasklet
 *
 * \param   port       Pointer to structure #ifx_ssc_port  
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void 
ifx_ssc_tasklet_serve_queue(struct ifx_ssc_port *port)
{
    /* 
     * Serve queue entries till no queue entry anymore to serve, we wait for
     * DMA or the lock entry is not in the queue. 
     */
    while (ifx_ssc_serve_qentry(port));
}

/**
 * \fn static INLINE void ifx_ssc_tasklet(unsigned long arg)
 * \brief SSC tasklet implementation
 *
 * \param   arg       cast to structure #ifx_ssc_port  
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void 
ifx_ssc_tasklet(unsigned long arg)
{
    struct ifx_ssc_port *port = (struct ifx_ssc_port *)arg;

    IFX_SSC_PRINT(port, SSC_MSG_TASKLET, "%s enter\n", __func__);
    ifx_ssc_tasklet_serve_queue(port);
}

/**
 * \fn static INLINE void ifx_ssc_tasklet_init(struct ifx_ssc_port *port)
 * \brief SSC tasklet initialization
 *
 * \param   port       Pointer to structure #ifx_ssc_port  
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE void
ifx_ssc_tasklet_init(struct ifx_ssc_port *port)
{
    tasklet_init(&port->ssc_txrxq, ifx_ssc_tasklet, (unsigned long)port);
}

#else
#define IFX_SSC_THREAD_OPTIONS   (CLONE_FS | CLONE_FILES | CLONE_SIGHAND)

/**
 * \fn static int ifx_ssc_kthread(void *arg)
 * \brief SSC kernel thread implementation function
 *
 * \param   arg       cast to structure #ifx_ssc_port  
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static int 
ifx_ssc_kthread(void *arg)
{
    struct ifx_ssc_port *port = (struct ifx_ssc_port *)arg;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
    struct task_struct *kthread = current;
#endif

    /* do LINUX specific setup */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))
    daemonize();
    reparent_to_init();

    /* 
     * lock the kernel. A new kernel thread starts without
     * the big kernel lock, regardless of the lock state
     * of the creator (the lock level is *not* inheritated)
     */
    lock_kernel();

    /* Don't care about any signals. */
    siginitsetinv(&current->blocked, 0);

    /* set name of this process */
    strcpy(kthread->comm, port->name);

    /* let others run */
    unlock_kernel();
#else
    daemonize(port->name);
#endif
    /* Request delivery of SIGKILL */
    allow_signal(SIGKILL);

    while(1) {
        /* Die if I receive SIGKILL */
        if (signal_pending(current)) {
            break;
        }

        /* Serve queue entries till no queue entry anymore to serve, we wait for
         * DMA or the lock entry is not in the queue.
         */
        while (ifx_ssc_serve_qentry(port));
  
        /* Wait for DMA interrupt or sync queue to wakes us up */
        IFX_SSC_WAIT_EVENT(port->ssc_thread_wait, IFX_SSC_THREAD_EVENT, port->event_flags);
        IFX_SSC_PRINT(port, SSC_MSG_THREAD, "%s DMA or sync queue event received\n", __func__);
    }
    complete_and_exit(&port->thread_done, 0);
    return 0;
}

/**
 * \fn static INLINE int ifx_ssc_thread_init(struct ifx_ssc_port *port)
 * \brief SSC kernel thread initialization
 *
 * \param   port       Pointer to structure #ifx_ssc_port  
 * \return  none
 * \ingroup IFX_SSC_INTERNAL 
 */
static INLINE int 
ifx_ssc_thread_init(struct ifx_ssc_port *port)
{
    init_completion(&port->thread_done);
    port->ssc_pid = kernel_thread(ifx_ssc_kthread, (void*) port,
        IFX_SSC_THREAD_OPTIONS);
    IFX_SSC_PRINT(port, SSC_MSG_INIT, "%s pid %d\n", __func__, port->ssc_pid);
    return 0;
}
#endif /* CONFIG_IFX_SPI_ASYNCHRONOUS */

/** 
 * \fn int ifx_sscAsyncTxRx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
 *       char *txbuf, int txsize, char *rxbuf, int rxsize)
 * \brief Transmit/receive to/from SSC in one step. It performs the data transmission and 
 *  then the data reception.

 * The SSC driver sets the chipselect when the data transmission starts and
 * resets it when the transmission is completed.
 * This routine is called to start an asynchronous data transmission. 
 * The provided callback routine is called after the transmission is done. 
 *
 * \param   handler    Handle of the connection where to make the configuration on
 * \param   pCallback  Function callback that is called after the request is
 *                     performed.
 * \param   txbuf      Pointer to the data packet to transmit.
 * \param   txsize     Amount of Bytes to transmit.
 * \param   rxbuf      Pointer to store the received data packet.
 * \param   rxsize     Amount of Bytes to receive.

 * \return Returns the amount of bytes that are transmitted in case of
 *  successful transmission. In case of an error the function returns (-1).
 *
 * \remarks This function activates the chipselect before data transmission and
 *  deactivates it after transmission (when function returns). This is done with
 *  the callback function. The chipselect is not modified in case
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscAsyncTxRx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
        char *txbuf, int txsize, char *rxbuf, int rxsize)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg;
    IFX_SSC_QUEUE_t     *pqueue;
    struct ifx_ssc_port *port;
    struct ifx_ssc_device_stats *stats;

   /* Sanity check */
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    IFX_KASSERT(!((rxbuf == NULL) && (rxsize == 0) 
        && (txbuf == NULL)  && (txsize == 0)),
        ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    stats = &dev->stats;
    port = dev->port;

    if (!in_interrupt()) {
        stats->context_err++;
        printk(KERN_ERR "%s must be called in interrupt context<irq, softirq, tasklet>\n", __func__);
        return -1;
    }

    ssc_cfg = &dev->conn_id;
    pqueue = &dev->queue;
    
    if (txsize > ssc_cfg->fragSize || rxsize > ssc_cfg->fragSize) {
        stats->frag_err++;
        printk(KERN_ERR "%s Device driver must do its own fragmentation tx %d rx %d > %d\n", 
            __func__, txsize, rxsize, ssc_cfg->fragSize); 
        return -1;
    }

    /* 
     * Ensure that only asynchronous SSC Handles could enqueue an 
     * asynchronous request. The parameter 'sscHandleType' is set during the 
     * ConnId allocation process. 
     */
    if (pqueue->handle_type != IFX_SSC_HANDL_TYPE_ASYNC) {
        stats->handler_err++;
        printk(KERN_ERR "%s must use async handler\n", __func__);
        return -1;
    }
    
    /* 
     * Check if the queue entry of the ConnId is already queued with a request 
     * but this request is not served yet. Every ConnId can only queue up one 
     * request at the time. 
     */
    if (atomic_read(&pqueue->isqueued) == 1) {
        stats->dup_qentries++;
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s same queue has queued more than once\n", __func__);
        return (-1);
    }
    
    /* Add pointer and sizes to the queue entry of this SSC handle. */
    pqueue->txbuf          = txbuf;
    pqueue->txsize         = txsize;
    pqueue->rxbuf          = rxbuf;
    pqueue->rxsize         = rxsize;
    if (pCallback != NULL) {
        pqueue->callback   = *pCallback;
    }
    pqueue->request_lock   = IFX_FALSE;
    pqueue->exchange_bytes = 0;
    atomic_set(&pqueue->isqueued, 1);
 
    /* Add queue entry to priority synchronous queue */
    ifx_ssc_enqueue(pqueue);

    /* 
     * Trigger schedule or tasklet or fake interrupt according to different 
     * cases.
     */ 
    ifx_ssc_start_tasklet(port);
    return 0;
}
EXPORT_SYMBOL(ifx_sscAsyncTxRx);

/** 
 * \fn int ifx_sscAsyncTx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
 *       char *txbuf, int txsize)
 * \brief transmit the data, located at "txbuf". The "txsize" amount of bytes is 
 *  transmitted over SSC. 

 * The SSC driver sets the chipselect when the data transmission starts and
 * resets it when the transmission is completed.
 * This routine is called to start an asynchronous data transmission. 
 * The provided callback routine is called after the transmission is done. 
 *
 * \param   handler    Handle of the connection where to make the configuration on
 * \param   pCallback  Function callback that is called after the request is
 *                     performed.
 * \param   txbuf      Pointer to the data packet to transmit.
 * \param   txsize     Amount of Bytes to transmit.

 * \return Returns the amount of bytes that are transmitted in case of
 *  successful transmission. In case of an error the function returns (-1).
 *
 * \remarks This function activates the chipselect before data transmission and
 *  deactivates it after transmission (when function returns). This is done with
 *  the callback function. The chipselect is not modified in case
 * \ingroup IFX_SSC_FUNCTIONS
 */
int
ifx_sscAsyncTx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
    char *txbuf, int txsize)
{
    return ifx_sscAsyncTxRx(handler, pCallback, txbuf, txsize, NULL, 0);
}
EXPORT_SYMBOL(ifx_sscAsyncTx);

/** 
 * \fn int ifx_sscAsyncRx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
 *       char *rxbuf, int rxsize)
 * \brief Receive from SSC. The received data are stored at "rxbuf". The "rxsize" 
 *  describes the amount of bytes to receive from SSC.

 * The SSC driver sets the chipselect when the data reception starts and
 * resets it when the reception is completed.
 *
 * \param   handler    Handle of the connection where to make the configuration on
 * \param   pCallback  Function callback that is called after the request is
 *                     performed.
 * \param   rxbuf      Pointer to store the received data packet.
 * \param   rxsize     Amount of Bytes to receive.

 * \return  Returns the amount of bytes that are received in case of successful
   transmission. In case of an error the function returns (-1). (-1).
 *
 * \remarks This function activates the chipselect before data transmission and
 *  deactivates it after transmission (when function returns). This is done with
 *  the callback function. The chipselect is not modified in case
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscAsyncRx(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback,
    char *rxbuf, int rxsize)
{
    return ifx_sscAsyncTxRx(handler, pCallback,  NULL, 0, rxbuf, rxsize);
}
EXPORT_SYMBOL(ifx_sscAsyncRx);

/** 
 * \fn int ifx_sscAsyncLock(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback)
 *
 * \brief This function locks and reserves the whole SSC interface for the given
    'handler'.

 *  The chipselect, belonging to this SSC session is already
 *  activated. This means the chipselect callback is called.
 *  After complete data transmission and reception, 
 *  This routine is called to get the lock on tasklet level. 
 *  The provided callback routine is called after the lock is set. 
 *
 * \param handler Handle of the connection.
 * \param pCallback Function callback that is called after the request is
          performed.
 *
 * \return Return (0) in case of success, otherwise (-1) in case of errors.
 *
 * \remarks This function actives the SSC chipselect of this 'handler' by calling
 * the callback function. 
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscAsyncLock(IFX_SSC_HANDLE handler, IFX_SSC_ASYNC_CALLBACK_t *pCallback)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg;
    struct ifx_ssc_port *port;
    IFX_SSC_QUEUE_t     *pqueue;

    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));
    IFX_KASSERT((pCallback != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    pqueue = &dev->queue;
    ssc_cfg = &dev->conn_id;
    port = dev->port;
    
    if (port->lock_qentry == pqueue) {
        /* We hold the lock already -> nothing to request here! */
        return (-1);
    }
 
    /*
     * Check if the queue entry of the ConnId is already queued with a request 
     * but this request is not served yet. Every ConnId can only queue up one 
     * request at the time. 
     */
    if (atomic_read(&pqueue->isqueued) == 1) {
        return (-1);
    }
    pqueue->txbuf          = NULL;
    pqueue->txsize         = 0;
    pqueue->rxbuf          = NULL;
    pqueue->rxsize         = 0;
    /* Place a lock request in the queue */
    pqueue->request_lock = IFX_TRUE;
    pqueue->callback = *pCallback;
    pqueue->exchange_bytes = 0;
    atomic_set(&pqueue->isqueued, 1);
    
    /* Add queue entry to priority synchronous queue */
    ifx_ssc_enqueue(pqueue);
 
    /* 
     * Calls the internal process to serve the queue. This routine would
     * immediately return in case  the SSC hardware is currently used to serve
     * another request. 
     */
    ifx_ssc_start_tasklet(port);
    return 0;
}
EXPORT_SYMBOL(ifx_sscAsyncLock);

/** 
 * \fn int ifx_sscAsyncUnLock(IFX_SSC_HANDLE handler)
 * \brief This function releases the SSC lock that was placed before by calling
 * \ref ifx_sscAsyncLock. This function also inactivate the chipselect signal, which
 *  was set in \ref ifx_sscAsyncLock.

 * \param handler Handle of the connection.
 *
 * \return Return (0) in case of success, otherwise (-1) in case of errors.
 * \ingroup IFX_SSC_FUNCTIONS
 */
int 
ifx_sscAsyncUnLock(IFX_SSC_HANDLE handler)
{
    ssc_device_t *dev;
    IFX_SSC_CONFIGURE_t *ssc_cfg = NULL;
    struct ifx_ssc_port *port = NULL;
    IFX_SSC_QUEUE_t     *pqueue;
    
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ssc_device_t *)handler;
    ssc_cfg = &dev->conn_id;
    pqueue  = &dev->queue;    
    port = dev->port;
    
    if (port->lock_qentry != pqueue) {
        /* We do not hold the lock, therefore we can not release it! */
        return -1;
    }    

    /* Just forget about the lock, then the SSC driver would just take it
     * as a normel queue entry 
     */ 
    ifx_ssc_cs_unlock(dev);
    /* XXX, Possible race condition if called in in_irq() */
    pqueue->txbuf          = NULL;
    pqueue->txsize         = 0;
    pqueue->rxbuf          = NULL;
    pqueue->rxsize         = 0;
    pqueue->exchange_bytes = 0;
    memset(&pqueue->callback, 0, sizeof (IFX_SSC_ASYNC_CALLBACK_t));
    port->lock_qentry = NULL;

    /* Restart tasklet since we are async release SPI bus */
    ifx_ssc_start_tasklet(port);
    return 0;
}
EXPORT_SYMBOL(ifx_sscAsyncUnLock);

#ifdef CONFIG_SYSCTL
/*
 * Deal with the sysctl handler api changing.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8)
#define	SSC_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(ctl_table *ctl, int write, struct file *filp, \
	  void __user *buffer, size_t *lenp)
#define	SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, filp, buffer, lenp)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
#define	SSC_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(ctl_table *ctl, int write, struct file *filp, \
	  void __user *buffer, size_t *lenp, loff_t *ppos)
#define	SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, filp, buffer, lenp, ppos)
#else /* Linux 2.6.32+ */
#define	SSC_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(ctl_table *ctl, int write, \
	  void __user *buffer, size_t *lenp, loff_t *ppos)
#define	SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, buffer, lenp, ppos)
#endif

/* SSC Client driver proc entry for parameter configuration */
enum {
    IFX_SSC_PRIV_FRAGMENT_SIZE  = 1,
    IFX_SSC_PRIV_FIFO_SIZE      = 2,
    IFX_SSC_PRIV_BAUDRATE       = 3,
    IFX_SSC_PRIV_MODE           = 4,
};

static int
SSC_SYSCTL_DECL(ssc_sysctl_private, ctl, write, filp, buffer, lenp, ppos)
{
    ssc_device_t *dev = ctl->extra1;
    IFX_SSC_CONFIGURE_t *ssc_cfg = &dev->conn_id;
    struct ifx_ssc_port *port;
    u32 val;
    int ret;

    port = dev->port;
    ctl->data = &val;
    ctl->maxlen = sizeof(val);
    if (write) {
        ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,lenp, ppos);
        if (ret == 0) {
            switch ((long)ctl->extra2) {
                case IFX_SSC_PRIV_FRAGMENT_SIZE:
                    if (val < IFX_SSC_MIN_FRAGSIZE || val > IFX_SSC_MAX_FRAGSIZE)
                        return -EINVAL;
                    ssc_cfg->fragSize = val;
                    port->ssc_fragSize = val;
                    break;

                case IFX_SSC_PRIV_FIFO_SIZE:
                    if (val < IFX_SSC_FIFO_MIN_THRESHOULD || val > IFX_SSC_FIFO_MAX_THRESHOULD)
                        return -EINVAL;
                    ssc_cfg->maxFIFOSize = val;
                    break;

                case IFX_SSC_PRIV_BAUDRATE:
                    /* XXX, sanity check */
                    ssc_cfg->baudrate = val;
                    break;

                case IFX_SSC_PRIV_MODE:
                    ret = -EINVAL;
                    break;
                    
                default:
                    return -EINVAL;
            }
        }
    } 
    else {
        switch ((long)ctl->extra2) {
            case IFX_SSC_PRIV_FRAGMENT_SIZE:
                val = ssc_cfg->fragSize;
                break;

            case IFX_SSC_PRIV_FIFO_SIZE:
                val = ssc_cfg->maxFIFOSize;
                break;

            case IFX_SSC_PRIV_BAUDRATE:
                val = ssc_cfg->baudrate;
                break;

            case IFX_SSC_PRIV_MODE:
                val = ssc_cfg->ssc_mode;
                break;

            default:
                return -EINVAL;
        }
        ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos);
    }
    return ret;
}

static const 
ctl_table ssc_sysctl_template[] = {
    /* NB: must be last entry before NULL */
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "fragment_size",
      .mode     = 0644,
      .proc_handler = ssc_sysctl_private,
      .extra2   = (void *)IFX_SSC_PRIV_FRAGMENT_SIZE,
    },
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "fifosize",
      .mode     = 0644,
      .proc_handler = ssc_sysctl_private,
      .extra2   = (void *)IFX_SSC_PRIV_FIFO_SIZE,
    },
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "baudrate",
      .mode     = 0644,
      .proc_handler = ssc_sysctl_private,
      .extra2   = (void *)IFX_SSC_PRIV_BAUDRATE,
    },
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "spimode",
      .mode     = 0644,
      .proc_handler = ssc_sysctl_private,
      .extra2   = (void *)IFX_SSC_PRIV_MODE,
    },
    { 0 }
};

static void
ifx_ssc_sysctl_attach(ssc_device_t *dev)
{
    int i, space;

    space = 5 * sizeof(struct ctl_table) + sizeof(ssc_sysctl_template);
    dev->ssc_sysctls = kmalloc(space, GFP_KERNEL);
    if (dev->ssc_sysctls == NULL) {
        printk("%s: no memory for sysctl table!\n", __func__);
        return;
    }

    /* setup the table */
    memset(dev->ssc_sysctls, 0, space);
    IFX_SET_CTL_NAME(dev->ssc_sysctls[0], CTL_DEV);
    dev->ssc_sysctls[0].procname = "dev";
    dev->ssc_sysctls[0].mode = 0555;
    dev->ssc_sysctls[0].child = &dev->ssc_sysctls[2];
    /* [1] is NULL terminator */
    IFX_SET_CTL_NAME(dev->ssc_sysctls[2], CTL_AUTO);
    dev->ssc_sysctls[2].procname = dev->dev_name;
    dev->ssc_sysctls[2].mode = 0555;
    dev->ssc_sysctls[2].child = &dev->ssc_sysctls[4];
    /* [3] is NULL terminator */
    /* copy in pre-defined data */
    memcpy(&dev->ssc_sysctls[4], ssc_sysctl_template,
        sizeof(ssc_sysctl_template));

    /* add in dynamic data references */
    for (i = 4; dev->ssc_sysctls[i].procname; i++){
        if (dev->ssc_sysctls[i].extra1 == NULL) {
            dev->ssc_sysctls[i].extra1 = dev;
        }
    }

    /* tack on back-pointer to parent device */
    dev->ssc_sysctls[i - 1].data = dev->dev_name;

    /* and register everything */
    dev->ssc_sysctl_header = IFX_REGISTER_SYSCTL_TABLE(dev->ssc_sysctls);
    if (dev->ssc_sysctl_header == NULL ) {
        printk("%s: failed to register sysctls!\n", dev->dev_name);
        kfree(dev->ssc_sysctls);
        dev->ssc_sysctls = NULL;
    }
}

static void
ifx_ssc_sysctl_detach(ssc_device_t *dev)
{
    if (dev->ssc_sysctl_header != NULL) {
        unregister_sysctl_table(dev->ssc_sysctl_header);
        dev->ssc_sysctl_header = NULL;
    }
    if (dev->ssc_sysctls != NULL) {
        kfree(dev->ssc_sysctls);
        dev->ssc_sysctls = NULL;
    }
}

/* SSC Driver itself proc support for debug and future configuration */
enum {
    IFX_SSC_PRIV_DEBUG          = 1,
};

static int
SSC_SYSCTL_DECL(port_sysctl_private, ctl, write, filp, buffer, lenp, ppos)
{
    struct ifx_ssc_port *port = ctl->extra1;
    u32 val;
    int ret;

    ctl->data = &val;
    ctl->maxlen = sizeof(val);
    if (write) {
        ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,lenp, ppos);
        if (ret == 0) {
            switch ((long)ctl->extra2) {
                case IFX_SSC_PRIV_DEBUG:
                    port->ssc_debug = val;
                    break;

                default:
                    return -EINVAL;
            }
        }
    } 
    else {
        switch ((long)ctl->extra2) {
            case IFX_SSC_PRIV_DEBUG:
                val = port->ssc_debug;
                break;

            default:
                return -EINVAL;
        }
        ret = SSC_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos);
    }
    return ret;
}

static const 
ctl_table port_sysctl_template[] = {
    /* NB: must be last entry before NULL */
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "debug",
      .mode     = 0644,
      .proc_handler = port_sysctl_private,
      .extra2   = (void *)IFX_SSC_PRIV_DEBUG,
    },
    { 0 }
};

static void
ifx_ssc_port_sysctl_attach(struct ifx_ssc_port *port)
{
    int i, space;

    space = 5 * sizeof(struct ctl_table) + sizeof(port_sysctl_template);
    port->port_sysctls = kmalloc(space, GFP_KERNEL);
    if (port->port_sysctls == NULL) {
        printk("%s: no memory for sysctl table!\n", __func__);
        return;
    }

    /* setup the table */
    memset(port->port_sysctls, 0, space);
    IFX_SET_CTL_NAME(port->port_sysctls[0], CTL_DEV);
    port->port_sysctls[0].procname = "dev";
    port->port_sysctls[0].mode = 0555;
    port->port_sysctls[0].child = &port->port_sysctls[2];
    /* [1] is NULL terminator */
    IFX_SET_CTL_NAME(port->port_sysctls[2], CTL_AUTO);
    port->port_sysctls[2].procname = port->name;
    port->port_sysctls[2].mode = 0555;
    port->port_sysctls[2].child = &port->port_sysctls[4];
    /* [3] is NULL terminator */
    /* copy in pre-defined data */
    memcpy(&port->port_sysctls[4], port_sysctl_template,
        sizeof(port_sysctl_template));

    /* add in dynamic data references */
    for (i = 4; port->port_sysctls[i].procname; i++){
        if (port->port_sysctls[i].extra1 == NULL) {
            port->port_sysctls[i].extra1 = port;
        }
    }

    /* tack on back-pointer to parent device */
    port->port_sysctls[i - 1].data = port->name; 

    /* and register everything */
    port->port_sysctl_header = IFX_REGISTER_SYSCTL_TABLE(port->port_sysctls);
    if (port->port_sysctl_header == NULL ) {
        printk("%s: failed to register sysctls!\n", port->name);
        kfree(port->port_sysctls);
        port->port_sysctls = NULL;
    }
}

static void
ifx_ssc_port_sysctl_detach(struct ifx_ssc_port *port)
{
    if (port->port_sysctl_header != NULL) {
        unregister_sysctl_table(port->port_sysctl_header);
        port->port_sysctl_header = NULL;
    }
    if (port->port_sysctls != NULL) {
        kfree(port->port_sysctls);
        port->port_sysctls = NULL;
    }
}
#endif  /* CONFIG_SYSCTL */

/**
 *\fn IFX_SSC_HANDLE ifx_sscAllocConnection (char *dev_name, IFX_SSC_CONFIGURE_t *connid)
 *\brief Allocate and create a Connection ID "ConnId"
 *
 * Allocate and create a Connection ID "ConnId" to communicate over SSC.
 * This ConnId is needed for all remaining SSC driver API calls. This 
 * ConnId is a handle that helps the SSC driver to find the configuration 
 * that belongs to the connection. ConnId specific parameters are e.g. 
 * Baudrate, Priority, Chipselect Callback, etc.
 *
 * \param   dev_name    unique name for this connection. If null, will alloc
 *                      one unique name automatically
 * \param   connid      Connectin id 
 * \return  a handle "IFX_SSC_HANDLE" in case the allocation was successful. 
 *          In case of an error, the return handle is zero (NULL).
 * \ingroup IFX_SSC_FUNCTIONS
 */
IFX_SSC_HANDLE 
ifx_sscAllocConnection (char *dev_name, IFX_SSC_CONFIGURE_t *connid)
{
    struct ifx_ssc_port *port;
    ssc_device_t *p;
    ssc_device_t *q;
    IFX_SSC_QUEUE_t *queue;
    char buf[IFX_SSC_MAX_DEVNAME] = {0};
    char *pName;

    /* Sanity check first! */
    if (ifx_ssc_isp == NULL) {
        printk("%s ssc driver must be loaded first!\n", __func__);
        return NULL;
    }

    port = &ifx_ssc_isp[0]; /* XXX */

    if (port->ssc_ndevs >= IFX_SSC_MAX_DEVICE) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, 
            "%s device number out of range\n", __func__);
        return NULL;
    }

    if (connid == NULL) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR,
            "%s must provide connection portrmation!\n", __func__);
        return NULL;
    }

    if ((connid->ssc_mode < IFX_SSC_MODE_0) 
        || (connid->ssc_mode > IFX_SSC_MODE_3)) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR,
            "%s invalid spi mode <%d~%d>!\n", __func__, IFX_SSC_MODE_0, IFX_SSC_MODE_3);
        return NULL;
    }

    if (connid->ssc_prio < IFX_SSC_PRIO_LOW 
        || (connid->ssc_prio > IFX_SSC_PRIO_MAX)) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, 
            "%s invalid priority <%d~%d>!\n", __func__, IFX_SSC_PRIO_LOW, IFX_SSC_PRIO_MAX);
    }
    

    if (connid->csset_cb == NULL) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s must provide cs function\n", __func__);
        return NULL;
    }

    if (connid->fragSize < IFX_SSC_MIN_FRAGSIZE 
        || connid->fragSize > IFX_SSC_MAX_FRAGSIZE) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s invalid fragment size <%d~%d>!\n", __func__,
            IFX_SSC_MIN_FRAGSIZE, IFX_SSC_MAX_FRAGSIZE);
        return NULL;
    }

    if (connid->maxFIFOSize < IFX_SSC_FIFO_MIN_THRESHOULD 
        || connid->maxFIFOSize > IFX_SSC_FIFO_MAX_THRESHOULD) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s invalid fifo size <%d~%d>!\n", __func__,
            IFX_SSC_FIFO_MIN_THRESHOULD, IFX_SSC_FIFO_MAX_THRESHOULD);
        return NULL;
    }
    /* If no name specified, will assign one name for identification */
    if (dev_name == NULL) {
        sprintf(buf, "ssc%d", port->ssc_ndevs);
        pName = buf;
    }
    else {
        if (strlen(dev_name) > (IFX_SSC_MAX_DEVNAME - 1) ) {
            IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s device name is too long\n", __func__);
            return NULL;
        }
        pName = dev_name;
    }

    p = (ssc_device_t *)kmalloc(sizeof (ssc_device_t), GFP_KERNEL);
    if (p == NULL) {
        IFX_SSC_PRINT(port, SSC_MSG_ERROR,"%s failed to allocate memory\n", __func__);
        return NULL;
    }
    memset(p, 0, sizeof (ssc_device_t));

    IFX_SSC_SEM_LOCK(port->dev_sem);
    TAILQ_FOREACH(q, &port->ssc_devq, dev_entry) {
        if (strcmp(q->dev_name, pName) == 0) {
            kfree(p);
            IFX_SSC_SEM_UNLOCK(port->dev_sem);
            IFX_SSC_PRINT(port, SSC_MSG_ERROR, "%s device registered already!\n", __func__);
            return NULL;
        }
    }
    IFX_SSC_SEM_UNLOCK(port->dev_sem);

    /* Follow net device driver name rule */
    memcpy(p->dev_name, pName, IFX_SSC_MAX_DEVNAME);
    memcpy((char *)&p->conn_id, (char *)connid, sizeof (IFX_SSC_CONFIGURE_t));

    queue = &p->queue;
    /* Queue handler type converted from priority */
    if (connid->ssc_prio == IFX_SSC_PRIO_ASYNC) {
        queue->handle_type = IFX_SSC_HANDL_TYPE_ASYNC;
    }
    else {
        queue->handle_type = IFX_SSC_HANDL_TYPE_SYNC;
    }
    /* Back pointer to later usage */
    queue->dev = p;
    atomic_set(&queue->isqueued, 0);
    queue->request_lock = IFX_FALSE;
    /* 
     * Just for fast access, priority based on device, instead of packet 
     * Still keep per packet priority there for future change.
     */
    p->dev_prio = connid->ssc_prio;
    
    IFX_SSC_WAKELIST_INIT(p->dev_thread_wait);

    p->port = port; /* back pointer to port for easy reference later */
    port->ssc_ndevs++;
#ifdef CONFIG_SYSCTL
    ifx_ssc_sysctl_attach(p);
#endif /* CONFIG_SYSCTL */
    IFX_SSC_SEM_LOCK(port->dev_sem);
    TAILQ_INSERT_TAIL(&port->ssc_devq, p, dev_entry);
    IFX_SSC_SEM_UNLOCK(port->dev_sem);

    /* Make sure very device CS in default state on registration */
    if(connid->csset_cb != NULL) {
        connid->csset_cb(IFX_SSC_CS_OFF, connid->cs_data);
    }
    IFX_SSC_PRINT(port, SSC_MSG_INIT, 
        "%s: device %s register sucessfully!\n", __func__, p->dev_name);
    return (IFX_SSC_HANDLE)p;
}
EXPORT_SYMBOL(ifx_sscAllocConnection);

/**
 *\fn int ifx_sscFreeConnection (IFX_SSC_HANDLE handler)
 *\brief Release ssc connnection
 * 
 * Release a ConnId handle that was allocated by the function ifx_SscAllocConnection
 * before. An allocated ConnId has to be released by the client driver module 
 * when the SSC driver is not used anymore. Note that all allocated ConnId's should 
 * be released before the SSC driver is unloaded from the kernel.
 * 
 * \param   handler    ConnId handle allocated by ifx_SscAllocConnection
 * \returns (0) in case of success, otherwise (-1) in case of errors.
 * \ingroup IFX_SSC_FUNCTIONS
 */
int  
ifx_sscFreeConnection (IFX_SSC_HANDLE handler)
{
    ssc_device_t *p;
    struct ifx_ssc_port *port;  
    ssc_device_t *q, *next;

    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    p = (ssc_device_t *)handler;
    port = p->port;
    IFX_SSC_SEM_LOCK(port->dev_sem);
    TAILQ_FOREACH_SAFE(q, &port->ssc_devq, dev_entry, next) {
        if (strcmp(q->dev_name, p->dev_name) ==  0) {
            TAILQ_REMOVE(&port->ssc_devq, q, dev_entry);
        #ifdef CONFIG_SYSCTL
            ifx_ssc_sysctl_detach(q);
        #endif /* CONFIG_SYSCTL */
            kfree(q);
            port->ssc_ndevs--;
            IFX_SSC_SEM_UNLOCK(port->dev_sem);
            IFX_SSC_PRINT(port, SSC_MSG_INIT, "%s device %s unregistered\n", __func__, p->dev_name);
            return 0;
        }
    }
    IFX_SSC_SEM_UNLOCK(port->dev_sem);
    return -1;
}
EXPORT_SYMBOL(ifx_sscFreeConnection);

/**
 * \fn static inline int ifx_ssc_drv_ver(char *buf)
 * \brief Display SSC driver version after initilazation succeeds
 * 
 * \return number of bytes will be printed
 * \ingroup IFX_SSC_INTERNAL 
 */
static inline int 
ifx_ssc_drv_ver(char *buf)
{
    return ifx_drv_ver(buf, "SSC", IFX_SSC_VER_MAJOR, IFX_SSC_VER_MID, IFX_SSC_VER_MINOR);
}

/**
 * \fn  static inline int ifx_ssc_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
 *
 * \brief  This function is used read the proc entries used by SSC.
 *         Here, the proc entry is used to read version number
 *
 * \param  buf   Proc buffer
 * \param  start  start of the proc entry
 * \param  offset   if set to zero, do not proceed to print proc data
 * \param  count  Maximum proc print size
 * \param  eof    end of the proc entry
 * \param  data   unused
 *
 * \return len Lenght of data in buffer
 *
 * \ingroup IFX_SSC_INTERNAL 
 */
static inline int 
ifx_ssc_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    /* No sanity check cos length is smaller than one page */
    len += ifx_ssc_drv_ver(buf + len);
    *eof = 1;
    return len;    
} 


/**
 * \fn static void ifx_ssc_proc_create(void)
 * \brief Create ssc proc directory and file when module initialized.
 *
 * \return  none
 * \ingroup IFX_SSC_INTERNAL  
 */
static void 
ifx_ssc_proc_create(void)
{
    ifx_ssc_proc = proc_mkdir("driver/ifx_ssc", NULL);

    create_proc_read_entry("version",
                            0,
                            ifx_ssc_proc,
                            ifx_ssc_proc_version,
                            NULL);

    create_proc_read_entry("stats",
                            0,
                            ifx_ssc_proc,
                            ifx_ssc_stats_proc,
                            NULL);
    create_proc_read_entry("reg",
                            0,
                            ifx_ssc_proc,
                            ifx_ssc_reg_read_proc,
                            NULL);
}

/**
 * \fn static void ifx_ssc_proc_delete(void)
 * \brief Delete ssc proc directory and file.
 *
 * \return  none
 * \ingroup IFX_SSC_INTERNAL  
 */
static void 
ifx_ssc_proc_delete(void)
{
    remove_proc_entry("version", ifx_ssc_proc);

    remove_proc_entry("stats", ifx_ssc_proc);
    
    remove_proc_entry("reg", ifx_ssc_proc);

    remove_proc_entry("driver/ifx_ssc", NULL);
}

/**
 * \fn static int __init ifx_ssc_init (void)
 * \brief SSC module Initialization.
 *
 * \return -ENOMEM  Failed to allocate memory
 * \return -EBUSY   Failed to iomap register space
 * \return 0        OK
 * \ingroup IFX_SSC_INTERNAL  
 */
static int __init
ifx_ssc_init (void)
{
    u32 reg;
    struct ifx_ssc_port *port;
    int i, j, nbytes;
    int ret_val = -ENOMEM;
    char ver_str[128] = {0};
    static int ifx_ssc_initialized = 0;

    if (ifx_ssc_initialized == 1) {
        return 0;
    }
    else {
        ifx_ssc_initialized = 1;
    }

    nbytes = IFX_SSC_MAX_PORT_NUM * sizeof(struct ifx_ssc_port);
    ifx_ssc_isp = (struct ifx_ssc_port *) kmalloc(nbytes, GFP_KERNEL);
    if (ifx_ssc_isp == NULL) {
        printk(KERN_ERR "%s: no memory for isp\n", __func__);
        return (ret_val);
    }
    memset(ifx_ssc_isp, 0, nbytes);

    /* set default values in ifx_ssc_port */
    for (i = 0; i < IFX_SSC_MAX_PORT_NUM; i++) {
        port = &ifx_ssc_isp[i];
        port->port_idx = i;
        /* default values for the HwOpts */
        port->opts.abortErrDetect = IFX_SSC_DEF_ABRT_ERR_DETECT;
        port->opts.rxOvErrDetect = IFX_SSC_DEF_RO_ERR_DETECT;
        port->opts.rxUndErrDetect = IFX_SSC_DEF_RU_ERR_DETECT;
        port->opts.txOvErrDetect = IFX_SSC_DEF_TO_ERR_DETECT;
        port->opts.txUndErrDetect = IFX_SSC_DEF_TU_ERR_DETECT;
        port->opts.loopBack = IFX_SSC_DEF_LOOP_BACK;
        port->opts.echoMode = IFX_SSC_DEF_ECHO_MODE;
        port->opts.idleValue = IFX_SSC_DEF_IDLE_DATA;
        port->opts.clockPolarity = IFX_SSC_DEF_CLOCK_POLARITY;
        port->opts.clockPhase = IFX_SSC_DEF_CLOCK_PHASE;
        port->opts.headingControl = IFX_SSC_DEF_HEADING_CONTROL;
        port->opts.dataWidth = IFX_SSC_DEF_DATA_WIDTH;
        port->opts.modeRxTx = IFX_SSC_DEF_MODE_RXTX;
        port->opts.gpoCs = IFX_SSC_DEF_GPO_CS;
        port->opts.gpoInv = IFX_SSC_DEF_GPO_INV;
        port->opts.masterSelect = IFX_SSC_DEF_MASTERSLAVE;
        port->prev_ssc_clk = 0;
        port->baudrate = IFX_SSC_DEF_BAUDRATE;
        port->prev_baudrate = 0;
        port->prev_ssc_mode = IFX_SSC_MODE_UNKNOWN;
        port->ssc_ndevs = 0;
        port->ssc_fragSize = DEFAULT_SSC_FRAGMENT_SIZE;
        /* values specific to SSC */
        port->mapbase = IFX_SSC_PHY_BASE;  
        port->membase = ioremap_nocache(port->mapbase, IFX_SSC_SIZE);
        if (!port->membase) {
            printk(KERN_ERR "%s: Failed during io remap\n", __func__);
            ret_val = -EBUSY;
            goto errout1;
        }
    #ifdef IFX_SSC_DMA_SUPPORT       
        /* Register with DMA engine */
        port->dma_dev = dma_device_reserve("SPI");
        if (port->dma_dev == NULL) {
            printk(KERN_ERR "%s: Failed to reserve dma device!\n", __func__);
            goto errout2;
        }
        ifx_ssc_init_dma_device(i, port->dma_dev);
    #endif /* IFX_SSC_DMA_SUPPORT*/

        strcpy(port->name, IFX_SSC_NAME);
        port->ssc_cs_locked = IFX_FALSE;
        port->lock_qentry   = NULL;
        port->serve_qentry  = NULL;
    #ifdef CONFIG_IFX_SPI_DEBUG
        port->ssc_debug = SSC_MSG_ERROR;
    #else
        port->ssc_debug = 0;
    #endif /* CONFIG_IFX_SPI_DEBUG */
        atomic_set(&port->dma_wait_state, 0);
        /* Either SSC tasklet or SSC kernel thread support, not both */
    #ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
        ifx_ssc_tasklet_init(port);
    #else
        ifx_ssc_thread_init(port);
    #endif /* CONFIG_IFX_SPI_ASYNCHRONOUS */

    #ifdef IFX_SSC_DMA_SUPPORT 
        /* The following buffer allocation for HW WAR, last 1~3 bytes in DMA 
         * It will make sure buffer will align on dma burst length
         */    
        port->dma_orig_txbuf = kmalloc(DEFAULT_SSC_FRAGMENT_SIZE \
            + ((port->dma_dev->tx_burst_len << 2) - 1), GFP_KERNEL);
        if (port->dma_orig_txbuf == NULL) {
            printk(KERN_ERR "%s: no memory for dma_orig_txbuf\n", __func__);
            goto errout3;
        }
        port->dma_txbuf = (char *)(((u32)( port->dma_orig_txbuf +   \
            ((port->dma_dev->tx_burst_len << 2) - 1)))               \
             & ~((port->dma_dev->tx_burst_len << 2) - 1));

    
        port->dma_orig_rxbuf = kmalloc(DEFAULT_SSC_FRAGMENT_SIZE \
            + ((port->dma_dev->rx_burst_len << 2) - 1), GFP_KERNEL);
        if (port->dma_orig_rxbuf == NULL) {
            printk(KERN_ERR "%s: no memory for dma_orig_rxbuf\n", __func__);
            goto errout4;
        }
        port->dma_rxbuf = (char *)(((u32)( port->dma_orig_rxbuf +  \
            ((port->dma_dev->rx_burst_len << 2) - 1)))              \
             & ~((port->dma_dev->rx_burst_len << 2) - 1));
    #endif /* IFX_SSC_DMA_SUPPORT*/

        /* Queue initialization */
        TAILQ_INIT(&port->ssc_devq);
        TAILQ_INIT(&port->ssc_asyncq);
        for (j = 0; j < IFX_SSC_PRIO_MAX; j++) {
            TAILQ_INIT(&port->ssc_syncq[j]);
        }
        IFX_SSC_Q_LOCK_INIT(port);
        IFX_SSC_SEM_INIT(port->dev_sem);
        IFX_SSC_WAKELIST_INIT(port->ssc_thread_wait);
        IFX_SSC_IRQ_LOCK_INIT(port, "ifx_ssc_lock");
        
        /* Activate SSC */
        SPI_PMU_SETUP(IFX_PMU_ENABLE);
      
        reg = IFX_SSC_GET_CLC(port);
        reg |= SM(IFX_SSC_DEF_RMC, IFX_SSC_CLC_RMC);
        reg &= ~IFX_SSC_CLC_DIS;
        IFX_SSC_SET_CLC(reg, port);
        
        port->ssc_fake_irq = IFX_SSC_FAKE_IRQ_NO;
        ret_val = ifx_ssc_int_wrapper.request(port->ssc_fake_irq, ifx_ssc_fake_isr,
                      IRQF_DISABLED, "ifx_ssc_tx", port);
        if (ret_val) {
            printk(KERN_ERR "%s: unable to get irq %d\n", __func__, port->ssc_fake_irq);
            goto errout5;
        }   
        /* Disable SSC module level real hardware interrupts */
        IFX_SSC_SET_IRN_EN(0, port);
        
        /* init serial framing register */
        IFX_SSC_SET_FRAMING_CON(IFX_SSC_DEF_SFCON, port);
        
        if (ifx_ssc_hwinit(port) < 0) {
            ifx_ssc_gpio_release();
            printk(KERN_ERR "%s: hardware init failed for port %d\n", __func__, i);
            goto errout6;
        }
    #ifdef CONFIG_SYSCTL
        ifx_ssc_port_sysctl_attach(port);
    #endif /* CONFIG_SYSCTL */
    
    #ifdef CONFIG_IFX_PMCU
        ifx_ssc_pmcu_init(port);
    #endif /* CONFIG_IFX_PMCU */
    }
    ifx_ssc_proc_create();
    ifx_ssc_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    return 0;

errout6:
    ifx_ssc_int_wrapper.free(port->ssc_fake_irq, port);
errout5:
#ifdef IFX_SSC_DMA_SUPPORT
    kfree(port->dma_orig_rxbuf);
errout4:
    kfree(port->dma_orig_txbuf);
errout3:
    dma_device_unregister(port->dma_dev);
    dma_device_release(port->dma_dev);
errout2:
#endif /* IFX_SSC_DMA_SUPPORT */
    iounmap(port->membase);
errout1:
    kfree(ifx_ssc_isp);
    return (ret_val);
}

/**
 * \fn static void __exit ifx_ssc_exit (void)
 * \brief SSC Module Cleanup.
 *
 * Upon removal of the SSC module this function will free all allocated 
 * resources and unregister devices. 
 * \return none
 * \ingroup IFX_SSC_INTERNAL
 */
static void __exit
ifx_ssc_exit (void)
{
    int i;
    struct ifx_ssc_port *port;
#ifdef IFX_SSC_DMA_SUPPORT
    struct dma_device_info *dma_dev;
#endif       
    /* free up any allocated memory */
    for (i = 0; i < IFX_SSC_MAX_PORT_NUM; i++) {
        port = &ifx_ssc_isp[i];
        /* Disable the SSC */
        IFX_SSC_CONFIG_MODE(port);
        
        IFX_SSC_SEM_LOCK(port->dev_sem);
        if (!TAILQ_EMPTY(&port->ssc_devq)) {
            IFX_SSC_SEM_UNLOCK(port->dev_sem);
            printk(KERN_ERR "%s SSC devices still attached, please release them first\n", __func__);
            return;
        }
        IFX_SSC_SEM_UNLOCK(port->dev_sem);
        ifx_ssc_int_wrapper.free(port->ssc_fake_irq, port);
    #ifdef CONFIG_IFX_SPI_ASYNCHRONOUS
        tasklet_kill(&port->ssc_txrxq);
    #else
        kill_proc(port->ssc_pid, SIGKILL, 1);
        wait_for_completion(&port->thread_done);
    #endif /* CONFIG_IFX_SPI_ASYNCHRONOUS */
        IFX_SSC_IRQ_LOCK_DESTROY(port);
        IFX_SSC_Q_LOCK_DESTROY(port);
    #ifdef IFX_SSC_DMA_SUPPORT 
        dma_dev = port->dma_dev;
        if (dma_dev != NULL) {
            dma_device_unregister(dma_dev);
            dma_device_release(dma_dev);
        }
    #endif
    #ifdef CONFIG_SYSCTL
        ifx_ssc_port_sysctl_detach(port);
    #endif /* CONFIG_SYSCTL */

    #ifdef CONFIG_IFX_PMCU
        ifx_ssc_pmcu_exit(port);
    #endif /* CONFIG_IFX_PMCU */
    
    #ifdef IFX_SSC_DMA_SUPPORT
        kfree(port->dma_orig_rxbuf);
        kfree(port->dma_orig_txbuf);
    #endif 
        iounmap(port->membase);
    }
    ifx_ssc_gpio_release();
    kfree(ifx_ssc_isp);
    ifx_ssc_proc_delete();
    SPI_PMU_SETUP(IFX_PMU_DISABLE);
}

module_init (ifx_ssc_init);
module_exit (ifx_ssc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Chuanhua, chuanhua.lei@infineon.com");
MODULE_DESCRIPTION("IFX SSC driver");
MODULE_SUPPORTED_DEVICE("IFX SSC IP module");

