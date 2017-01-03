/******************************************************************************
**
** FILE NAME    : ifxmips_usif_spi.c
** PROJECT      : IFX UEIP
** MODULES      : USIF for SPI
**
** DATE         : 03 June 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : USIF for SPI Master/Slave
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
** HISTORY
** $Date        $Author         $Comment
** 03 Jun,2009  Lei Chuanhua    Initial version
*******************************************************************************/
/*!
  \file ifxmips_usif_spi.c
  \ingroup IFX_USIF_SPI
  \brief usif spi driver source file
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
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <asm/page.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/sysctl.h>

/* Project header */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>

#include "ifxmips_usif_spi.h"

#ifdef CONFIG_IFX_PMCU
#include "ifxmips_usif_spi_pm.h"
#endif /* CONFIG_IFX_PMCU */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/sched.h>
#define kill_proc(p,s,v)        send_sig(s,find_task_by_vpid(p),0)
#endif

#define IFX_USIF_SPI_VER_MAJOR          1
#define IFX_USIF_SPI_VER_MID            2
#define IFX_USIF_SPI_VER_MINOR          1

#define IFX_USIF_SPI_NAME             "ifx_usif_spi"

#define IFX_USIF_SPI_MAX_BAUD          50000000
#define IFX_USIF_SPI_HALF_BAUD         25000000
#define IFX_USIF_SPI_1MHZ_BAUD         1000000

#ifdef CONFIG_IFX_USIF_SPI_DEBUG
#define INLINE 
enum {
    USIF_MSG_TX_FIFO  = 0x00000001,
    USIF_MSG_TX_DMA   = 0x00000002,
    USIF_MSG_RX_FIFO  = 0x00000004,
    USIF_MSG_RX_DMA   = 0x00000008,
    USIF_MSG_INT      = 0x00000010,  /* Interrupt msg */
    USIF_MSG_CFG      = 0x00000020,
    USIF_MSG_THREAD   = 0x00000040,
    USIF_MSG_TASKLET  = 0x00000080,
    USIF_MSG_DEBUG    = 0x00000100,
    USIF_MSG_ERROR    = 0x00000200,
    USIF_MSG_INIT     = 0x00000400, /* Initialization msg */
    USIF_MSG_QUEUE    = 0x00000800,
    USIF_MSG_LOCK     = 0x00001000,
    USIF_MSG_CALLBACK = 0x00002000,
    USIF_MSG_ANY      = 0xffffffff,   /* anything */
};
static void ifx_usif_spi_debug(struct ifx_usif_port *port, const char *fmt, ...);

#define IFX_USIF_PRINT(_port, _m, _fmt, args...) do { \
    if ((_port)->usif_debug & (_m)) {                 \
        ifx_usif_spi_debug((_port), (_fmt), ##args);      \
    }                                                \
} while (0)

#else
#define INLINE  inline
#define IFX_USIF_PRINT(_port, _m, _fmt, ...)
#endif /* CONFIG_IFX_USIF_SPI_DEBUG */

extern unsigned int ifx_get_usif_hz(void);

static struct ifx_usif_port ifx_usif_spi_port;

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
} ifx_usif_int_wrapper_t;

static ifx_usif_int_wrapper_t ifx_usif_int_wrapper = {
    .request  = request_irq,
    .free     = free_irq,
};

static struct proc_dir_entry *ifx_usif_spi_proc = NULL;
static const char ifx_usif_spi_proc_node_name[] = "driver/ifx_usif_spi";

/**
 * \fn static INLINE void ifx_usif_spi_raise_fake_irq(unsigned int irq)
 * \brief Trigger different schedule procedures according to different context.
 * if caller is already in tasklet, it will be done in caller's tasklet
 *
 * \param   irq       Interrupt pin number 
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static INLINE void 
ifx_usif_spi_raise_fake_irq(unsigned int irq)
{
    unsigned long flags;
    struct ifx_usif_port *port = (struct ifx_usif_port *) &ifx_usif_spi_port;

    IFX_USIF_PRINT(port, USIF_MSG_INT, 
        "%s irq %d triggered \n", __func__, irq);
    spin_lock_irqsave(&port->usif_irq_lock, flags);
    ifx_icu_irsr_set(irq);
    spin_unlock_irqrestore(&port->usif_irq_lock, flags);
}

/**
 * \fn static void ifx_usif_spi_start_tasklet(struct ifx_usif_port *port)
 * \brief Trigger different schedule procedures according to different context.
 * if caller is already in tasklet, it will be done in caller's tasklet
 *
 * \param   port       Pointer to structure #ifx_usif_port  
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static void 
ifx_usif_spi_start_tasklet(struct ifx_usif_port *port)
{
    struct tasklet_struct *ptasklet;

    /* 
     * Calls the internal process to serve the queue. This routine would
     * immediately return in case the USIF hardware is currently used to serve
     * another request. 
     */
    ptasklet = &port->usif_txrxq;
    if (in_irq()) { /* Hardware irq */
        IFX_USIF_PRINT(port, USIF_MSG_INT, "%s hardware irq schedule\n", __func__);
        tasklet_hi_schedule(ptasklet);
    }
    else if (in_softirq()){ /* Softirq or tasklet */
        IFX_USIF_PRINT(port, USIF_MSG_TASKLET, "%s softirq schedule\n", __func__);
        if (tasklet_trylock(ptasklet)) { /* tasklet_trylock for SMP*/
            ptasklet->func(ptasklet->data);
            tasklet_unlock(ptasklet);
        }
        else {
            IFX_USIF_PRINT(port, USIF_MSG_TASKLET, "%s should never happen\n", __func__);
        }
    }
    else { /* Process  context */
        IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s process schedule\n", __func__);
        ifx_usif_spi_raise_fake_irq(port->usif_fake_irq);
    }
}

/**
 * \fn static irqreturn_t ifx_usif_spi_fake_isr (int irq, void *dev_id)
 * \brief Fake interrupt handler
 *
 * \param   irq    fake interrupt irq number
 * \param   dev_id vague type, will be converted to 
 *                 pointer to structure #ifx_usif_port
 * \return  IRQ_HANDLED irq has been handled.
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
ifx_usif_spi_fake_isr (int irq, void *dev_id)
#else
ifx_usif_spi_fake_isr (int irq, void *dev_id, struct pt_regs *regs)
#endif 
{
    struct ifx_usif_port *port = (struct ifx_usif_port *) dev_id;

    IFX_USIF_PRINT(port, USIF_MSG_INT, "%s irq %d served\n", __func__, irq);
    ifx_icu_irsr_clr(irq);
    ifx_usif_spi_start_tasklet(port);
    return IRQ_HANDLED;
}

#ifdef CONFIG_IFX_USIF_SPI_DEBUG
/**
 * \fn static void ifx_usif_spi_debug(struct ifx_usif_port *port, const char *fmt, ...)
 * \brief Debug all kinds of level message
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \param   fmt     debug output format
 * 
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static void
ifx_usif_spi_debug(struct ifx_usif_port *port, const char *fmt, ...)
{
    static char buf[256] = {0};      /* XXX */
    va_list ap;
#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
    static const char *p = "tasklet";
#else
    static const char *p = "kthread";
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap); 

    printk("%s %s: %s\n", p, port->name, buf);
}
#endif /* CONFIG_IFX_USIF_SPI_DEBUG */

/**
 * \fn static INLINE void ifx_usif_spi_wait_tx_finished(struct ifx_usif_port *port)
 * \brief Wait for SPI bus becomes idle, FIFO empty doesn't means spi bus idle.
 *        to start another transaction, need to make sure bus is idle
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static INLINE void 
ifx_usif_spi_wait_tx_finished(struct ifx_usif_port *port)
{
    /* Only TX_FIN can make sure that there is no activity in serial bus */
    while ((IFX_USIF_GET_RAW_INT_STATUS(port) & IFX_USIF_INT_TX_FIN) == 0){
        ;
    }
    IFX_USIF_CLR_INT(IFX_USIF_INT_TX_FIN, port); /* Prepare for the next transaction */
}

/**
 * \fn static INLINE unsigned int ifx_usif_spi_get_kernel_clk(struct ifx_usif_port *port)
 * \brief Get USIF SPI clock speed.
 * Returns the current operating speed of the USIF peripheral, depending on chip
 * specific bus speed and RMC setting in CLC register.
 * 
 * \param   port     Pointer to structure #ifx_usif_port
 * \return  >0       Peripheral speed in HZ
 * \return  0        Error
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static INLINE unsigned int 
ifx_usif_spi_get_kernel_clk(struct ifx_usif_port *port)
{
    /* This function assumes that the CLC register is set with the
     * appropriate value for RMC.
     */
    unsigned int rmc;

    rmc = MS(IFX_USIF_GET_CLOCK_COUNTER(port), IFX_USIF_CLC_CNT_RMC);
    if (rmc == 0) {
        printk(KERN_ERR "%s rmc==0 \n", __func__);
        return 0;
    }
    return (unsigned int)(ifx_get_usif_hz() / rmc);
}

/**
 * \fn static int ifx_usif_spi_stats_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
 * \brief USIF SPI proc file read.
 * This function creates the proc file output for the USIF SPI statistics.
 * 
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_stats_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
    int off = 0;
    ifx_usif_port_t *port;
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_STAT_t *stats;
    
    port = &ifx_usif_spi_port;
    stats = &port->stats;

    off += sprintf(page + off, "Statistics for USIF SPI\n\n");
    off += sprintf(page + off, "USIF%d\n", port->port_idx);
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
    off += sprintf(page + off, "USIF bus status %s\n", port->usif_cs_locked? "locked" : "unlocked");
    off += sprintf(page + off, "\n");

    /* Per device statistics */
    IFX_USIF_SPI_SEM_LOCK(port->dev_sem);
    TAILQ_FOREACH(dev, &port->usif_devq, dev_entry){
        off += sprintf(page + off, "Device %s:\n", dev->dev_name);
        off += sprintf(page + off, "Rx Bytes %d\n", dev->stats.rxBytes);
        off += sprintf(page + off, "Tx Bytes %d\n", dev->stats.txBytes);
        off += sprintf(page + off, "Context errors %d\n", dev->stats.context_err);
        off += sprintf(page + off, "Duplicated qentry errors %d\n", dev->stats.dup_qentries);
        off += sprintf(page + off, "Fragment errors %d\n", dev->stats.frag_err);
        off += sprintf(page + off, "handler errors %d\n", dev->stats.handler_err);
        off += sprintf(page + off, "Enqueue %d\n", dev->stats.enqueue);
        off += sprintf(page + off, "Dequeue %d\n", dev->stats.dequeue);            

    }
    IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem); 
    *eof = 1;
    return off;
}

/**
 * \fn static int ifx_usif_spi_reg_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
 * \brief USIF SPI proc file read.
 * This function creates the proc file output for the USIF SPI register dump.
 * 
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_reg_read(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
    int off = 0;
    ifx_usif_port_t *port;
#define IFX_USIF_SPI_REG_MAX  24
    u32 stats[IFX_USIF_SPI_REG_MAX] = {0};

    port = &ifx_usif_spi_port;

    IFX_USIF_SPI_IRQ_LOCK(port);
    stats[0] = IFX_USIF_GET_ID(port);
    stats[1] = IFX_USIF_GET_SWC_ID(port);
    stats[2] = IFX_USIF_GET_FIFO_ID(port);
    stats[3] = IFX_USIF_GET_CLOCK_COUNTER(port);
    stats[4] = IFX_USIG_GET_CLOCK_STATUS(port);
    stats[5] = IFX_USIF_GET_MODE_CONFIG(port);
    stats[6] = IFX_USIF_GET_PROTOCOL_CONFIG(port);
    stats[7] = IFX_USIF_GET_PROTOCOL_STATUS(port);
    stats[8] = IFX_USIF_GET_CHIPSELECT_CONFIG(port);
    stats[9] = IFX_USIF_GET_FRAC_DIV(port);
    stats[10] = IFX_USIF_GET_BAUDRATE_COUNTER(port);
    stats[11] = IFX_USIF_GET_INTERCHARACTER_TIMEOUT(port);
    stats[12] = IFX_USIF_GET_FIFO_CONFIG(port);
    stats[13] = IFX_USIF_GET_FIFO_STATUS(port);
    stats[14] = IFX_USIF_GET_TXD_SIDEBAND_CONFIG(port);
    stats[15] = IFX_USIF_GET_DPLUS_STATUS(port);
    stats[16] = IFX_USIF_GET_TX_WORD(port);
    if (IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN) {
        stats[17] = IFX_USIF_GET_RX_WORD(port);
    }
    stats[18] = IFX_USIF_GET_RX_MRPS(port);
    stats[19] = IFX_USIF_GET_RAW_INT_STATUS(port);
    stats[20] = IFX_USIF_GET_INT_MASK(port);
    stats[21] = IFX_USIF_GET_INT_STATUS(port);

    IFX_USIF_SPI_IRQ_UNLOCK(port);

    off += sprintf(page + off, "Register dump for USIF SPI\n\n");
    off += sprintf(page + off, "USIF_ID           0x%08x\n", stats[0]);
    off += sprintf(page + off, "USIF_SWCID        0x%08x\n", stats[1]);
    off += sprintf(page + off, "USIF_FIFO_ID      0x%08x\n", stats[2]);

    off += sprintf(page + off, "USIF_CLC_CNT      0x%08x\n", stats[3]);
    off += sprintf(page + off, "USIF_CLC_STAT     0x%08x\n", stats[4]);

    off += sprintf(page + off, "USIF_MODE_CFG     0x%08x\n", stats[5]);
    off += sprintf(page + off, "USIF_PRTC_CFG     0x%08x\n", stats[6]);
    off += sprintf(page + off, "USIF_PRTC_STAT    0x%08x\n", stats[7]);

    off += sprintf(page + off, "USIF_CS_CFG       0x%08x\n", stats[8]);
    off += sprintf(page + off, "USIF_FDIV_CFG     0x%08x\n", stats[9]);
    off += sprintf(page + off, "USIF_BC_CFG       0x%08x\n", stats[10]);
    off += sprintf(page + off, "USIF_ICTMO_CFG    0x%08x\n", stats[11]);
    off += sprintf(page + off, "USIF_FIFO_CFG     0x%08x\n", stats[12]);
    off += sprintf(page + off, "USIF_FIFO_STAT    0x%08x\n", stats[13]);
    off += sprintf(page + off, "USIF_TXD_SB       0x%08x\n", stats[14]);
    off += sprintf(page + off, "USIF_DPLUS_STAT   0x%08x\n", stats[15]);
    off += sprintf(page + off, "USIF_TXD          0x%08x\n", stats[16]);
    off += sprintf(page + off, "USIF_RXD          0x%08x\n", stats[17]);
    off += sprintf(page + off, "USIF_MRPS_CTRL    0x%08x\n", stats[18]);
    off += sprintf(page + off, "USIF_RIS          0x%08x\n", stats[19]);
    off += sprintf(page + off, "USIF_IMSC         0x%08x\n", stats[20]);
    off += sprintf(page + off, "USIF_MIS          0x%08x\n", stats[21]);
    off += sprintf(page + off, "\n");

    *eof = 1;
    return off;
#undef IFX_USIF_SPI_REG_MAX
}


/**
 * \fn static INLINE int ifx_usif_spi_drv_ver(char *buf)
 * \brief Display USPI SPI driver version after initilazation succeeds
 * 
 * \return number of bytes will be printed
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE int 
ifx_usif_spi_drv_ver(char *buf)
{
    return ifx_drv_ver(buf, "USIF-SPI", IFX_USIF_SPI_VER_MAJOR, IFX_USIF_SPI_VER_MID, IFX_USIF_SPI_VER_MINOR);
} 

/**
 * \fn static inline int ifx_usif_spi_proc_version(char *page, char **start, off_t offset, int count, int *eof, void *data)
 * \brief USIF SPI proc version.
 * This function creates the proc file output for the USIF SPI driver.
 * 
 * \param   page     Buffer to write the string to
 * \param   start    not used (Linux internal)
 * \param   offset   not used (Linux internal)
 * \param   count    not used (Linux internal)
 * \param   eof      Set to 1 when all data is stored in buffer
 * \param   data     not used (Linux internal)
 * \return  len      Lenght of data in buffer
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static inline int 
ifx_usif_spi_proc_version(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    /* No sanity check because length is smaller than one page */
    len += ifx_usif_spi_drv_ver(page + len);
    *eof = 1;
    return len;    
}

/**
 * \fn static int ifx_usif_spi_proc_create(void)
 * \brief Create USIF SPI proc/drivers node
 * 
 * \return  -EFAULT Failed to create proc node
 * \return  0       Create proc node successfully
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_proc_create(void)
{
    int ret = 0;
    struct proc_dir_entry *entry;
    
    ifx_usif_spi_proc = proc_mkdir(ifx_usif_spi_proc_node_name, NULL);
    if (ifx_usif_spi_proc == NULL) {
        printk(KERN_ERR "%s:  Unable to create proc entry for %s\n", 
            __func__, ifx_usif_spi_proc_node_name);
        ret = -EFAULT;
        goto done;
    }
    entry = create_proc_read_entry("version",
                            0,
                            ifx_usif_spi_proc,
                            ifx_usif_spi_proc_version,
                            NULL);
    if (entry == NULL) {
        ret = -EFAULT;
        goto err1;
    }

    entry = create_proc_read_entry("stats",
                            0,
                            ifx_usif_spi_proc,
                            ifx_usif_spi_stats_read,
                            NULL);
    if (entry == NULL) {
        ret = -EFAULT;
        goto err2;
    }
    entry = create_proc_read_entry("reg",
                            0,
                            ifx_usif_spi_proc,
                            ifx_usif_spi_reg_read,
                            NULL);
    if (entry == NULL) {
        ret = -EFAULT;
        goto err3;
    }    
    return ret;
    
err3:
    remove_proc_entry("stats", ifx_usif_spi_proc);
err2:
    remove_proc_entry("version", ifx_usif_spi_proc);
err1:
    remove_proc_entry(ifx_usif_spi_proc_node_name, NULL);
done:
    return ret;
}

/**
 * \fn static void ifx_usif_spi_proc_delete(void)
 * \brief Delete USIF SPI proc/drivers node
 * 
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void 
ifx_usif_spi_proc_delete(void)
{
    remove_proc_entry("version", ifx_usif_spi_proc);

    remove_proc_entry("stats", ifx_usif_spi_proc);
    
    remove_proc_entry("reg", ifx_usif_spi_proc);

    remove_proc_entry(ifx_usif_spi_proc_node_name, NULL);
}

/**
 * \fn static void ifx_usif_spi_dma_setup(struct ifx_usif_port *port, int dir, int enabled)
 *
 * \brief Setup dma direction and enable/disable
 * 
 * \param   port        Pointer to structure #ifx_usif_port
 * \param   dir         DMA Direction, tx/rx
 * \param   enabled     DMA enable/disable
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void 
ifx_usif_spi_dma_setup(struct ifx_usif_port *port, int dir, int enabled)
{
    u32 reg = 0;
    IFX_USIF_SPI_CONFIGURE_t  *usif_cfg;
    ifx_usif_device_t *current_dev = port->current_dev;
    struct dma_device_info *dma_dev = port->dma_dev;
    static const u32 dplus_def_setting = IFX_USIF_DPLUS_CTRL_TX_DIS \
                | IFX_USIF_DPLUS_CTRL_RX_MASK | IFX_USIF_DPLUS_CTRL_SET_MASK;

    usif_cfg = &current_dev->conn_id;
    
    reg = dplus_def_setting;
    
    if (dir == IFX_USIF_SPI_DIR_RX) {/* RX */
        if (enabled) {
            /* CSX<0~2> mapping to RX Class, this class must go to D+ interface */
            reg &= ~(1 << (usif_cfg->cs_data + IFX_USIF_DPLUS_CTRL_RX_MASK_OFFSET));
            /* Change USIF SPI RX DMA channel class according to CSX<0~2> */
            dma_device_setup_class_val(dma_dev->rx_chan[dma_dev->current_rx_chan], usif_cfg->cs_data);
        }
        else { /* BPI interface */
            reg |= (1 << (usif_cfg->cs_data + IFX_USIF_DPLUS_CTRL_RX_MASK_OFFSET));
            /* Change USIF SPI RX DMA channel class according to CSX<0~2> */
            dma_device_setup_class_val(dma_dev->rx_chan[dma_dev->current_rx_chan], 0);
        }
    }
    else { /* TX */
        if (enabled) {
            reg &= ~IFX_USIF_DPLUS_CTRL_TX_DIS;
            reg |= IFX_USIF_DPLUS_CTRL_TX_EN;
        }
        else {
            reg &= ~IFX_USIF_DPLUS_CTRL_TX_EN;
            reg |= IFX_USIF_DPLUS_CTRL_TX_DIS;
        }
        /*
         * XXX, auto chipselect, tx class information 
         * At present, auto chipselection disabled,
         * TX Class will always use CLASS0
         */
    } 
    IFX_USIF_SPI_IRQ_LOCK(port);
    IFX_USIF_CONFIG_DPLUS_CONTROL(reg, port);
    IFX_USIF_SPI_IRQ_UNLOCK(port);
    /* Ensure D+ TX activity finished */
    if ((dir == IFX_USIF_SPI_DIR_TX) && (enabled == IFX_USIF_SPI_DMA_DISABLE)) {
        while ((IFX_USIF_GET_DPLUS_STATUS(port) & IFX_USIF_DPLUS_STAT_TX_ACT)) {
            ;
        }
    }
}

/**
 * \fn static INLINE void ifx_usif_spi_dma_irq_finished(struct ifx_usif_port *port)
 * \brief DMA<TX/RX> interrupt received, this function calls to reschedule or wake up sleep 
 * kernel thread
 * 
 * \param   port        Pointer to structure #ifx_usif_port
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void
ifx_usif_spi_dma_irq_finished(struct ifx_usif_port *port)
{
    /* 
     * Reset the flag that we are waiting for the DMA to complete 
     * This flag should be reset before the following stuff, otherwise
     * start_tasklet will stop
     */
    atomic_set(&port->dma_wait_state, 0);
    smp_wmb();
#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
    ifx_usif_spi_start_tasklet(port);
#else
    IFX_USIF_SPI_WAKEUP_EVENT(port->usif_thread_wait, IFX_USIF_SPI_THREAD_EVENT, port->event_flags);
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */
}

/**
 * \fn static int ifx_usif_spi_dma_int_handler(struct dma_device_info* dma_dev, int status)
 *
 * \brief Pseudo Interrupt handler for DMA.
 * This function processes DMA interrupts notified to the USIF device driver.
 * It is installed at the DMA core as interrupt handler for the USIF SPI DMA device 
 * and handles the following DMA interrupts:
 * - In case of a DMA receive interrupt the received data is passed to the 
 *   upper layer.
 * 
 * \param   dma_dev pointer to DMA device structure
 * \param   status  type of interrupt being notified (RCV_INT: DMA receive 
 *                  interrupt, TX_BUF_FULL_INT: transmit buffer full interrupt, 
 *                  TRANSMIT_CPT_INT: transmission complete interrupt)
 * \return  OK      In case of successful data reception from DMA
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_dma_int_handler(struct dma_device_info* dma_dev, int status)
{
    int i;
    struct ifx_usif_port *port;

    port = (struct ifx_usif_port *) dma_dev->priv;
    IFX_USIF_PRINT(port, USIF_MSG_INT, "Interrupt status %d\n", status);
    switch(status) {
        case RCV_INT: 
            IFX_USIF_PRINT(port, USIF_MSG_INT, "RCV_INT\n");
            ifx_usif_spi_dma_setup(port, IFX_USIF_SPI_DIR_RX, IFX_USIF_SPI_DMA_DISABLE);

            /* Disable USIF-SPI DMA RX channel */
            (dma_dev->rx_chan[dma_dev->current_rx_chan])->close(dma_dev->rx_chan[dma_dev->current_rx_chan]);

            /*
             * HW WAR, if packet length is indivisible by 4 or DMA burst length, the 
             * hardware will burst the data to DRAM, so it is possible to overwrite the
             * other data. Currently, only 4 bytes are considered.
             * XXX, to handle burst length case
             */
            { 
                int i = port->rx_len & 0x3;
                if (i != 0) {
                    memcpy((char *)port->rxbuf_ptr, port->dma_rxbuf, port->rx_len);
                    port->rxbuf_ptr = NULL;
                    port->rx_len = 0;
                }
            }

            ifx_usif_spi_dma_irq_finished(port);
            break;
            
        case TX_BUF_FULL_INT:
            for(i = 0; i < dma_dev->max_tx_chan_num; i++) {
                if((dma_dev->tx_chan[i])->control == IFX_DMA_CH_ON) {
                    dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
                }
            }
            IFX_USIF_PRINT(port, USIF_MSG_INT, "TX_BUF_FULL_INT\n");
            break;
            
        case TRANSMIT_CPT_INT:
            for(i = 0; i< dma_dev->max_tx_chan_num; i++) {
                dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
            }
            IFX_USIF_PRINT(port, USIF_MSG_INT, "TRANSMIT_CPT_INT\n");

            /*
             * NB, Make sure data has been sent out. TX_FIN interrupt
             * may take longer time to receive. Polling is better here
             */
            ifx_usif_spi_wait_tx_finished(port);
            
            ifx_usif_spi_dma_irq_finished(port);
            break;
            
        default:
            printk(KERN_ERR "%s unknow interrupt %d\n", __func__, status);
            break;
    }
    return 0;
}

/**
 * \fn static u8* ifx_usif_spi_dma_buffer_alloc(int len, int* byte_offset, void** opt)
 * \brief Allocates buffer for USIF SPI DMA.
 * This function is installed as DMA callback function to be called when the DMA
 * needs to allocate a new buffer.
 * 
 * \param   len          Length of packet
 * \param   *byte_offset Pointer to byte offset
 * \param   **opt        unused  
 * \return  NULL         In case of buffer allocation fails
 * \return  buffer       Pointer to allocated memory
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static u8* 
ifx_usif_spi_dma_buffer_alloc(int len, int *byte_offset, void** opt)
{
    return NULL;
}

/**
 * \fn static int ifx_usif_spi_dma_buffer_free(u8 *dataptr, void *opt)
 * \brief Free DMA buffer.
 * This function frees a buffer previously allocated by the DMA. 
 * 
 * \param   *dataptr Pointer to data buffer
 * \param   *opt     unused  
 * \return  0        OK
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_dma_buffer_free(u8 *dataptr, void *opt)
{
    /* SPI will be in charge of memory free if necessary */
    return 0;
} 

/**
 * \fn static int ifx_usif_spi_init_dma_device(struct dma_device_info *dma_dev)
 * \brief Initialize USIF SPI DMA device.
 * This function initializes the passed DMA device structure for usage as 
 * USIF SPI DMA device.
 * 
 * \param   *dma_dev Pointer to dma device structure to be initialized
 * \return  0        OK
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_init_dma_device(struct dma_device_info *dma_dev)
{
    int i;

    dma_dev->priv = &ifx_usif_spi_port;
    dma_dev->num_tx_chan  = DEFAULT_USIF_SPI_TX_CHANNEL_NUM;
    dma_dev->num_rx_chan  = DEFAULT_USIF_SPI_RX_CHANNEL_NUM; 
    dma_dev->tx_burst_len = DEFAULT_USIF_SPI_TX_BURST_LEN;
    dma_dev->rx_burst_len = DEFAULT_USIF_SPI_RX_BURST_LEN;
    dma_dev->port_tx_weight = 1;
    /* Combined with FIFO TX/RX SWAP */
    dma_dev->tx_endianness_mode = IFX_DMA_ENDIAN_TYPE3; 
    dma_dev->rx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_dev->port_packet_drop_enable = 0;
    for(i = 0; i < dma_dev->num_tx_chan; i++){
        dma_dev->tx_chan[i]->desc_len = DEFAULT_USIF_SPI_TX_CHANNEL_DESC_NUM;
        dma_dev->tx_chan[i]->control = IFX_DMA_CH_ON;
        dma_dev->tx_chan[i]->packet_size = DEFAULT_USIF_SPI_FRAGMENT_SIZE;
    }
    for(i = 0; i < dma_dev->num_rx_chan; i++){
        dma_dev->rx_chan[i]->desc_len = DEFAULT_USIF_SPI_RX_CHANNEL_DESC_NUM;
        dma_dev->rx_chan[i]->packet_size = DEFAULT_USIF_SPI_FRAGMENT_SIZE;
        dma_dev->rx_chan[i]->control = IFX_DMA_CH_ON;
        dma_dev->rx_chan[i]->byte_offset = 0;
    }
    dma_dev->current_tx_chan = 0;
    dma_dev->current_rx_chan = 0;
    /* 
     * set DMA handler functions for rx-interrupts, 
     * buffer allocation and release
     */
    dma_dev->intr_handler = ifx_usif_spi_dma_int_handler;
    dma_dev->buffer_alloc = ifx_usif_spi_dma_buffer_alloc;
    dma_dev->buffer_free  = ifx_usif_spi_dma_buffer_free;
    dma_device_register(dma_dev); 
    return 0;
}

/**
 * \fn static INLINE void ifx_usif_spi_gpio_init(void)
 * \brief Reserve and initialize GPIO for USIF-SPI.
 *
 * Initialize MUX settings to enable SPI interface
 * \ingroup IFX_USIF_SPI_INTERNAL  
 */
static INLINE void 
ifx_usif_spi_gpio_init(void)
{
    ifx_gpio_register(IFX_GPIO_MODULE_USIF_SPI);
}

/**
 * \fn static INLINE void ifx_usif_spi_gpio_release(void)
 * \brief GPIO release.
 *
 * Release reserverd gpio resource so that other module could use it
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void 
ifx_usif_spi_gpio_release(void)
{
    ifx_gpio_deregister(IFX_GPIO_MODULE_USIF_SPI);
}

/**
 * \fn static INLINE void ifx_usif_spi_clc_cfg_mode(struct ifx_usif_port *port)
 * \brief enter configuration mode.
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * 
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void
ifx_usif_spi_clc_cfg_mode(struct ifx_usif_port *port)
{
    int i;
#define IFX_USIF_SPI_CLC_CFG_TIMEOUT  10
    /* 
     * WAR, read back to make sure the previous write/read to take effect
     * before we enter config mode.
     * Please note, must be a config register, instead of status register
     * Unfortunately, it will not work on emulation platform
     */  
#ifndef CONFIG_USE_EMULATOR
    IFX_USIF_GET_MODE_CONFIG(port);
#endif /* CONFIG_USE_EMULATOR */
    IFX_USIF_CLC_CONFIG_MODE(port);
    for (i = 0; i < IFX_USIF_SPI_CLC_CFG_TIMEOUT; i++) {
        if ((IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_CFG_UPDATE_DONE) 
            == IFX_USIF_CLC_STAT_CFG_UPDATE_DONE) {
            break;
        }
    }
    if (i >= IFX_USIF_SPI_CLC_CFG_TIMEOUT) {
        printk(KERN_ERR "%s timeout\n", __func__);
    }

#undef IFX_USIF_SPI_CLC_CFG_TIMEOUT
}

/**
 * \fn static INLINE void ifx_usif_spi_clc_run_mode(struct ifx_usif_port *port)
 * \brief enter running mode.
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * 
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void
ifx_usif_spi_clc_run_mode(struct ifx_usif_port *port)
{
    int i;
#define IFX_USIF_SPI_CLC_RUN_TIMEOUT  10

    /* 
     * WAR, dummy read back to make sure the previous write/read to take effect
     * before we enter run mode.
     * Please note, must be a config register, instead of status register.
     * Unfortunately, it will not work on emulation platform
     */
#ifndef CONFIG_USE_EMULATOR
    IFX_USIF_GET_MODE_CONFIG(port);
#endif /* CONFIG_USE_EMULATOR */
    IFX_USIF_CLC_RUN_MODE(port);
    for (i = 0; i < IFX_USIF_SPI_CLC_RUN_TIMEOUT; i++) {
        if ((IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN_UPDATE_DONE)
            == IFX_USIF_CLC_STAT_RUN_UPDATE_DONE) {
            break;
        }
    }
    if (i >= IFX_USIF_SPI_CLC_RUN_TIMEOUT) {
        printk(KERN_ERR "%s timeout\n", __func__);
    }
    /* WAR, for lower baudrate, it still takes some time to make configuration to take effect */
    if (port->baudrate < 1000000) {
        udelay(1000000 / port->baudrate);
    }
    
#undef IFX_USIF_SPI_CLC_RUN_TIMEOUT
}

/**
 * \fn static INLINE int ifx_usif_spi_rxtx_mode_set(struct ifx_usif_port *port, unsigned int val)
 * \brief Rx/Tx mode set.
 * Set the transmission mode while USIF SPI is idle
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \param   val     Rx/Tx mode
 * \return  0       OK
 * \return  -EINVAL Invalid parameters supplied 
 * \return  -EBUSY  Transmission or reception ongoing
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE int 
ifx_usif_spi_rxtx_mode_set(struct ifx_usif_port *port, unsigned int val)
{
    u32 bits = 0;
    u32 reg;
    int enabled = 0;
    int force_clk;
    
    if (unlikely(port == NULL) 
        || unlikely((val & (IFX_USIF_SPI_MODE_RXTX_MASK)) == IFX_USIF_SPI_MODE_OFF)) {
        return -EINVAL;
    }

    /* Check if there is activity in USIF bus */
    if (((IFX_USIF_GET_FIFO_STATUS(port) & IFX_USIF_FIFO_STAT_TXRX_FFS_MASK) != 0x00)){
        printk(KERN_ERR "%s is busy\n", __func__);
        return -EBUSY;
    }

    switch (val) {
        case IFX_USIF_SPI_MODE_RX:
            bits = IFX_USIF_MODE_CFG_RX_EN;
            force_clk = 1; /* Force shift clock for RX only */
            break;
            
        case IFX_USIF_SPI_MODE_TX:
            bits = IFX_USIF_MODE_CFG_TX_EN;
            force_clk = 0;
            break;
            
        case IFX_USIF_SPI_MODE_RXTX: /* Fall through */
        default:
            bits = IFX_USIF_MODE_CFG_TXRX_EN;
            force_clk = 0;
            break;
    }
    
    IFX_USIF_SPI_IRQ_LOCK(port);
    enabled = IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN;
    if (enabled) {
        ifx_usif_spi_clc_cfg_mode(port);
    }
    reg = IFX_USIF_GET_MODE_CONFIG(port);
    reg &= ~ IFX_USIF_MODE_CFG_TXRX_MASK;

    reg |= bits;

    if (force_clk == 1) {
        reg |= IFX_USIF_MODE_CFG_SC_FRC;
    }
    else {
        reg &= ~ IFX_USIF_MODE_CFG_SC_FRC;
    }
    IFX_USIF_SET_MODE_CONFIG(reg, port);
    if (enabled) {
        ifx_usif_spi_clc_run_mode(port);
    }
    port->opts.modeRxTx = val;
    IFX_USIF_SPI_IRQ_UNLOCK(port);
    return 0;
}

/**
 * \fn static int ifx_usif_spi_sethwopts(struct ifx_usif_port *port)
 * \brief USIF SPI set hardware options.
 * This routine intializes the USIF SPI appropriately depending on slave/master and 
 * full-/half-duplex mode. It assumes that the USIF SPI is disabled and the fifo's 
 * and buffers are flushed later on.
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \return  0       OK
 * \return  -EINVAL Invalid hardware options supplied
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int
ifx_usif_spi_sethwopts(struct ifx_usif_port *port)
{
    u32 reg;
    u32 mode = 0;
    u32 prtc = 0;
    struct ifx_usif_hwopts *opts = &port->opts;

    if (opts->sync) {
        mode |= IFX_USIF_MODE_CFG_SYNC;
    }

    if (opts->master) {
        mode |= IFX_USIF_MODE_CFG_MA;
    }

    if (opts->frmctl) {
        mode |= IFX_USIF_MODE_CFG_FC_EN;
    }
    
    if (opts->loopBack) {
        mode |= IFX_USIF_MODE_CFG_LB;
    }
    
    if (opts->echoMode) {
        mode |= IFX_USIF_MODE_CFG_ECH;
    }

    if (opts->txInvert) {
        mode |= IFX_USIF_MODE_CFG_TX_INV;
    }

    if (opts->rxInvert) {
        mode |= IFX_USIF_MODE_CFG_RX_INV;
    }

    if (opts->shiftClk) {
        mode |= IFX_USIF_MODE_CFG_SC_FRC;
    }

    if (opts->clkPO) {
        mode |= IFX_USIF_MODE_CFG_SC_POL;
    }
    
    if (opts->clkPH) {
        mode |= IFX_USIF_MODE_CFG_SC_PH;
    }
    
    if (opts->txIdleValue) {
        mode |= IFX_USIF_MODE_CFG_TX_IDLE;
    }
    
    switch (opts->modeRxTx) {
        case IFX_USIF_SPI_MODE_TX:
            mode |= IFX_USIF_MODE_CFG_TX_EN;
            break;
        case IFX_USIF_SPI_MODE_RX:
            mode |= IFX_USIF_MODE_CFG_RX_EN;
            break;
        case IFX_USIF_SPI_MODE_RXTX:
            mode |= IFX_USIF_MODE_CFG_TXRX_EN;
            break;
        default:
            mode |= IFX_USIF_MODE_CFG_TXRX_EN;
            break;
    }

    switch (opts->charLen) {
        case IFX_USIF_PRTC_CFG_CLEN8:
            prtc |= IFX_USIF_PRTC_CFG_CLEN8;
            break;
        case IFX_USIF_PRTC_CFG_CLEN16:
            prtc |= IFX_USIF_PRTC_CFG_CLEN16;
            break;
        case IFX_USIF_PRTC_CFG_CLEN32:
            prtc |= IFX_USIF_PRTC_CFG_CLEN32;
            break;
        default:
            prtc |= IFX_USIF_PRTC_CFG_CLEN32;
            break;
    }

    if (opts->hdrCtrl) {
        prtc |= IFX_USIF_PRTC_CFG_HD_MSB;
    }

    if (opts->wakeUp) {
        prtc |= IFX_USIF_PRTC_CFG_WK_UP;
    }
    
    IFX_USIF_SPI_IRQ_LOCK(port);

    /* Mode configration register, master, sync, mode 0, tx/rx enable  */
    IFX_USIF_SET_MODE_CONFIG(mode, port);

    /* Protocol configuration register, 4 bytes per character, MSB first */
    IFX_USIF_SET_PROTOCOL_CONFIG(prtc, port);

    /* Default CS, no auto chip select. Initial value as high */
    reg = IFX_USIF_CS_CFG_EN;
    reg |= IFX_USIF_CS_CFG_CSX_DEFAULT;
    IFX_USIF_SET_CHIPSELECT_CONFIG(reg, port);

    /* Class 0 only, disable D+ TX */
    IFX_USIF_CONFIG_DPLUS_CONTROL(IFX_USIF_DPLUS_CTRL_TX_DIS |
        IFX_USIF_DPLUS_CTRL_RX_MASK | IFX_USIF_DPLUS_CTRL_SET_MASK, port);

    /* TX Class information */
    IFX_USIF_CONFIG_TXD_SIDEBAND(SM(IFX_USIF_TXD_CLASS0, IFX_USIF_TXD_SB_TX_CLS), port);

    IFX_USIF_SPI_IRQ_UNLOCK(port);
    return 0;
}

/**
 * \fn int ifx_usif_spi_cs_low(u32 cs)
 * \brief Chip select enable.
 * This function sets the given chip select for USIF SPI to low.
 *
 * \param   cs       Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_USIF_SPI_FUNCTIONS 
 */
int 
ifx_usif_spi_cs_low(u32 cs)
{
    int ret;
    u32 reg;
    int enabled;
    struct ifx_usif_port *port = &ifx_usif_spi_port;
    
    if (cs > IFX_USIF_SPI_CS_MAX) {
        ret = -EINVAL;
    }
    else {
        /* Check if there is activity in USIF bus */
        if (((IFX_USIF_GET_FIFO_STATUS(port) & IFX_USIF_FIFO_STAT_TXRX_FFS_MASK) != 0x00)){
            printk(KERN_ERR "%s is busy\n", __func__);
            return -EBUSY;
        } 
        enabled = IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN;
        if (enabled) {
            ifx_usif_spi_clc_cfg_mode(port);
        } 
        reg = IFX_USIF_GET_CHIPSELECT_CONFIG(port);
        /* Chipboudary CSO = CSO XOR CSOINV, CSOINV is initialized to 0xff */
        reg |= (1 << (cs + IFX_USIF_CS_OFFSET));
        IFX_USIF_SET_CHIPSELECT_CONFIG(reg, port);
   
        if (enabled) {
            ifx_usif_spi_clc_run_mode(port);
        }
        ret = 0;
    }
    return ret;
}
EXPORT_SYMBOL(ifx_usif_spi_cs_low);

/**
 * \fn int ifx_usif_spi_cs_high(u32 cs)
 * \brief Chip select disable.
 * This function sets the given chip select for USIF SPI to high.
 *
 * \param   cs       Selected CS pin
 * \return  0        OK
 * \return  -EINVAL  Invalid GPIO pin provided
 * \ingroup IFX_USIF_SPI_FUNCTIONS 
 */
int 
ifx_usif_spi_cs_high(u32 cs)
{
    int ret;
    int enabled;
    u32 reg;
    struct ifx_usif_port *port = &ifx_usif_spi_port;

    if (cs > IFX_USIF_SPI_CS_MAX) {
        ret = -EINVAL;
    }
    else {
        /* Check if there is activity in USIF bus */
        if (((IFX_USIF_GET_FIFO_STATUS(port) & IFX_USIF_FIFO_STAT_TXRX_FFS_MASK) != 0x00)){
            printk(KERN_ERR "%s is busy\n", __func__);
            return -EBUSY;
        }
        enabled = IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN;
        if (enabled) {
            ifx_usif_spi_clc_cfg_mode(port);
        }        
        
        reg = IFX_USIF_GET_CHIPSELECT_CONFIG(port);
         /* Chipboudary CSO = CSO XOR CSOINV, CSOINV is initialized to 0xff */
        reg &= ~(1 << (cs + IFX_USIF_CS_OFFSET));
        IFX_USIF_SET_CHIPSELECT_CONFIG(reg, port); 

        if (enabled) {
            ifx_usif_spi_clc_run_mode(port);
        }
        ret = 0;
    }
    return ret;
}
EXPORT_SYMBOL(ifx_usif_spi_cs_high);

/**
 * \fn static void ifx_usif_spi_start_txfifo(struct ifx_usif_port *port, char *txbuf, u32 txsize)
 * \brief Start FIFO data transmisssion .
 * This function copies remaining data in the transmit buffer into the FIFO 
 *
 * \param   port     Pointer to structure #ifx_usif_port
 * \param   txbuf    Pointer to the data packet to transmit
 * \param   txsize   Amount of Bytes to transmit
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void
ifx_usif_spi_start_txfifo(struct ifx_usif_port *port, char *txbuf, u32 txsize)
{
    int i;
    char *p;
    u32 eff_bytes, eff_word;
    u32 tx_cnt;
    u32 *ptxbuf;
    IFX_USIF_TXD_t txd_data;
    int total = txsize;

    while (1) {

        /* Wait for TX FIFO empty */
        while (IFX_USIF_GET_TX_FILL_FIFO(port) != 0x00){
            ;
        }
        
        /* Optimized the frequest two cases */
        if (total <= IFX_USIF_SPI_TX_FIFO_CHARS_PER_WORD) { /* Last one incomlete or complete word */
            eff_bytes = total;
            for (i = 0, p = txbuf; i < eff_bytes; i++, p++) {
                txd_data.txd_byte.byte[i] = *(u8 *)p;
            }
            IFX_USIF_PUT_TX_LAST_WORD(((eff_bytes == IFX_USIF_SPI_TX_FIFO_CHARS_PER_WORD)? \
                IFX_USIF_TXD_BE4: eff_bytes), txd_data.txd_word, port);
        }
        else if (total > port->tx_fifo_size_bytes) { /* More than fifo bytes */
            eff_bytes = port->tx_fifo_size_bytes;
            eff_word = port->tx_fifo_size_words;
            for (i = 0, ptxbuf = (u32 *)txbuf; i < eff_word; i++, ptxbuf++) {
                IFX_USIF_PUT_TX_WORD(*ptxbuf, port);
            }
        }
        else { /* 5 ~ 32 bytes */
            eff_bytes = total; 
            eff_word = (eff_bytes >> 2) + ((eff_bytes & 0x3) > 0 ? 1 : 0);
            
            /* Transmit the complete word */
            for (i = 0, ptxbuf = (u32 *)txbuf; i < eff_word - 1; i++, ptxbuf++) {
                IFX_USIF_PUT_TX_WORD(*ptxbuf, port);
            }
            tx_cnt = (eff_bytes & 0x3);
            /* Still one complete word */
            if (tx_cnt == 0) {
                tx_cnt = IFX_USIF_SPI_TX_FIFO_CHARS_PER_WORD;  /* 4 bytes for one complete word*/
            }

            /* Transmit the last complete or incomplete word */
            for (i = 0, p = (char *)ptxbuf; i < tx_cnt; i++, p++) {
                txd_data.txd_byte.byte[i] = *(u8 *)p;
            }
            IFX_USIF_PUT_TX_LAST_WORD((tx_cnt == IFX_USIF_SPI_TX_FIFO_CHARS_PER_WORD? \
                IFX_USIF_TXD_BE4: tx_cnt), txd_data.txd_word, port);
        }
       
        txbuf += eff_bytes;
        total -= eff_bytes;
        if (total == 0){
            break;
        }
    }
}

/**
 * \fn static void ifx_usif_spi_start_rxfifo(struct ifx_usif_port *port, char *rxbuf, u32 rxsize)
 * \brief Start FIFO data reception.
 * This function processes received data. It will read data from the FIFO and 
 * copy it to the receive buffer. 
 *
 * \param   port     Pointer to structure #ifx_usif_port
 * \param   rxbuf    Pointer to store the received data packet
 * \param   rxsize   Amount of Bytes to receive.
 * XXX, RME or calculate the length from the total length
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void
ifx_usif_spi_start_rxfifo(struct ifx_usif_port *port, char *rxbuf, u32 rxsize)
{
    int i;
    char *p;
    u32 eff_bytes, eff_word;
    int rx_cnt;
    u32 *prxbuf;
    IFX_USIF_RXD_t rxd_data;
    int total = rxsize;
    
    while(1){
        /* In this case, no need to check last complete or incomplete word */
        if (total >= port->rx_fifo_size_bytes) {
            eff_bytes = port->rx_fifo_size_bytes;
            eff_word = port->rx_fifo_size_words;
            
            IFX_USIF_CONFIG_RX_MRPS(SM(eff_bytes, IFX_USIF_MRPS_CTRL_MRPS), port);
            /* Wait for data whose length is equal to FIFO size */
            while (IFX_USIF_GET_RX_FILL_FIFO(port) != eff_word){
                ;
            }
            for (i = 0, prxbuf = (u32 *)rxbuf; i < eff_word; i++, prxbuf++) {
                *prxbuf = IFX_USIF_GET_RX_WORD(port);
            }
        }
        else {
            eff_bytes = total;
            eff_word = (eff_bytes >> 2) + ((eff_bytes & 0x3) > 0 ? 1 : 0);

            IFX_USIF_CONFIG_RX_MRPS(SM(eff_bytes, IFX_USIF_MRPS_CTRL_MRPS), port);
            while (IFX_USIF_GET_RX_FILL_FIFO(port) != eff_word){
                ;
            }
            /* Receive complete word */
            for (i = 0, prxbuf = (u32 *)rxbuf; i < eff_word - 1; i++, prxbuf++) {
                *prxbuf = IFX_USIF_GET_RX_WORD(port);
            }

            /* 
             * SPI Master know how many bytes to be received 
             * XXX, a better way to use RME and DPLUS status 
             * register, but it involves interrupt handling.
             */
            rx_cnt = total - ((eff_word - 1) << 2);
            /* Last complete word or in complete word */
            rxd_data.rxd_word = IFX_USIF_GET_RX_WORD(port);
            for (i = 0, p = (char *)prxbuf; i < rx_cnt; i++, p++) {
                *(u8 *)p = rxd_data.rxd_byte.byte[i];
            }
        }
        rxbuf += eff_bytes;
        total -= eff_bytes;
        if (total == 0) {
            break;
        }
    }
}

/**
 * \fn static int ifx_usif_spi_set_baudrate(struct ifx_usif_port *port, unsigned int baudrate)
 * \brief USIF SPI set baudrate.
 * Sets the baudrate of the corresponding port according to the passed
 * rate after reading out the current module speed.
 *
 * \param   port      Pointer to structure #ifx_usif_port
 * \param   baudrate  Desired baudrate
 * \return  0         OK
 * \return  -EINVAL   Could not retrieve system clock or invalid baudrate setting
 * \return  -EBUSY    Serial bus still busy
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int
ifx_usif_spi_set_baudrate(struct ifx_usif_port *port, unsigned int baudrate)
{
    int enabled;
    unsigned int spi_clock;
    unsigned int bc = 0;

    if (port->prev_baudrate == baudrate) 
        return 0;

    if ((baudrate > IFX_USIF_SPI_HALF_BAUD) && (baudrate < IFX_USIF_SPI_MAX_BAUD)) {
        printk(KERN_ERR "%s: clock < %d ~ %d >not supported  \n", __func__, 
            IFX_USIF_SPI_HALF_BAUD, IFX_USIF_SPI_MAX_BAUD);
        return -EINVAL;
    }
    
    spi_clock = ifx_usif_spi_get_kernel_clk(port);
    if (spi_clock == 0) {
        return -EINVAL;
    }
    
    if ((baudrate != IFX_USIF_SPI_MAX_BAUD) && (spi_clock >> 2) < baudrate) {
        printk(KERN_ERR "%s: illegal baudrate %u \n", __func__, baudrate);
        return -EINVAL;
    }

    if (baudrate != IFX_USIF_SPI_MAX_BAUD) {
        /*
         * Compute the baud counter 
         * bc = fkernel/(4 * fbaud) -1 ; not so accurate
         * Only half kernel clock is different. 
         */
        bc = (((spi_clock >> 2) + baudrate / 2) / baudrate) - 1;
    }
    if (bc > 0x1fff ||((bc == 0) &&
         ((IFX_USIF_GET_MODE_CONFIG(port) & IFX_USIF_MODE_CFG_MA) == 0))) {
        printk(KERN_ERR "%s: illegal baudrate %u br %d\n", __func__, baudrate, bc);
        return -EINVAL;
    }
        
    /* Check if there is activity in USIF bus */
    if ((IFX_USIF_GET_FIFO_STATUS(port) & IFX_USIF_FIFO_STAT_TXRX_FFS_MASK) != 0x00){
        printk(KERN_ERR "%s is busy\n", __func__);
        return -EBUSY;
    }
    port->baudrate = baudrate;
    IFX_USIF_SPI_IRQ_LOCK(port);
    /* Has to be in cfg mode */
    enabled = IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN;
    if (enabled) {
        ifx_usif_spi_clc_cfg_mode(port);
    }

    /* NB, SPI mode should not use fraction divider */
    IFX_USIF_FRAC_DIV_CONFIG(0, port);

    /* Half kernel clock, SCDIV = 1, BCRV = 0, fraction divider must not be used */
    if (baudrate == IFX_USIF_SPI_MAX_BAUD) {
        bc = 0;
        bc |= IFX_USIF_BC_CFG_SCDIV_HALF;
    }
    IFX_USIF_BAUDRATE_COUNTER_CONFIG(bc, port);
    
    if (enabled) {
        ifx_usif_spi_clc_run_mode(port);
    }
    IFX_USIF_SPI_IRQ_UNLOCK(port);
    
    port->prev_baudrate = baudrate;
    return 0;
}

/**
 * \fn static int ifx_usif_spi_hwinit(struct ifx_usif_port *port)
 * \brief USIF SPI hardware initialization.
 * Initializes the USIF SPI port hardware with the desired baudrate and transmission
 * options.
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \return  0       OK
 * \return  -EINVAL Error during initialization
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int
ifx_usif_spi_hwinit(struct ifx_usif_port *port)
{
    u32 reg =  0;

    ifx_usif_spi_gpio_init();

    /* Enable USIF /AHBS */
    ifx_usif_spi_pm_enable();

   /* RMC first */
    IFX_USIF_SET_CLOCK_COUNTER(SM(IFX_USIF_SPI_DEF_RMC, IFX_USIF_CLC_CNT_RMC)| 
        SM(IFX_USIF_SPI_DEF_ORMC, IFX_USIF_CLC_CNT_ORMC), port);
    
    /* Switch to USIF configuration mode, module enabled onece  */
    reg = IFX_USIF_CLC_MOD_EN;
    IFX_USIF_SET_RUN_CONTROL(reg, port);

    ifx_usif_spi_clc_cfg_mode(port); 
     
    /* Disable all interrupts. No interrupts at all except DMA */
    IFX_USIF_SET_INT_MASK(0, port);
    IFX_USIF_CLR_INT(IFX_USIF_INT_ALL, port);

    if (ifx_usif_spi_sethwopts(port) < 0) {
        printk(KERN_ERR "%s: setting the hardware options failed\n", __func__);
        return -EINVAL;
    }

    if (ifx_usif_spi_set_baudrate(port, port->baudrate) < 0) {
        printk(KERN_ERR "%s: setting the baud rate failed\n", __func__);
        return -EINVAL;
    }

    IFX_USIF_SPI_IRQ_LOCK(port);

    reg = IFX_USIF_SPI_FIFO_SETUP;

    /* 
     * Enable TX swap and RX swap, must be set so that it is compatible with
     * old SSC module.
     */
    reg |= IFX_USIF_FIFO_CFG_TX_SWAP | IFX_USIF_FIFO_CFG_RX_SWAP;
    /* Configure the TX/RX FIFO */
    IFX_USIF_SET_FIFO_CONFIG(reg, port);
 
    /* Switch to USIF working mode */
    ifx_usif_spi_clc_run_mode(port);
 
    IFX_USIF_SPI_IRQ_UNLOCK(port);

    /* FIFO information for later use */    
    port->tx_fifo_size_words = IFX_USIF_GET_TX_FIFO_SIZE(port);
    port->tx_fifo_size_bytes = (port->tx_fifo_size_words << 2);
    port->rx_fifo_size_words = IFX_USIF_GET_RX_FIFO_SIZE(port);
    port->rx_fifo_size_bytes = (port->rx_fifo_size_words << 2);
    return 0;
#undef IFX_USIF_CLC_MAX_CNT
}

/**
 * \fn static INLINE int ifx_usif_spi_txfifo(struct ifx_usif_port *port, char *txbuf, u32 txsize)
 * \brief Called to transmit data to USIF SPI using FIFO mode .
 * \param   port       Pointer to structure #ifx_usif_port  
 * \param   txbuf      Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 *          < 0        error number
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE int 
ifx_usif_spi_txfifo(struct ifx_usif_port *port, char *txbuf, u32 txsize)
{
    IFX_KASSERT(port->opts.modeRxTx == IFX_USIF_SPI_MODE_TX, ("%s invalid txrx mode\n", __func__));

    ifx_usif_spi_dma_setup(port, IFX_USIF_SPI_DIR_TX, IFX_USIF_SPI_DMA_DISABLE);

    IFX_USIF_SPI_IRQ_LOCK(port);
    /* Start the transmission */
    ifx_usif_spi_start_txfifo(port, txbuf, txsize);
    IFX_USIF_SPI_IRQ_UNLOCK(port);
    
    return txsize;
}

/**
 * \fn static INLINE int ifx_usif_spi_rxfifo(struct ifx_usif_port *port, char *rxbuf, u32 rxsize)
 * \brief Called to receive from USIF SPI using FIFO mode .
 * \param   port       Pointer to structure #ifx_usif_port
 * \param   rxbuf      Pointer to store the received data packet
 * \param   rxsize     Amount of Bytes to receive.
 * \return  >= 0       Number of bytes received
 *          < 0        error number 
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE int 
ifx_usif_spi_rxfifo(struct ifx_usif_port *port, char *rxbuf, u32 rxsize)
{
    IFX_KASSERT(port->opts.modeRxTx == IFX_USIF_SPI_MODE_RX, ("%s invalid txrx mode\n", __func__));

    ifx_usif_spi_dma_setup(port, IFX_USIF_SPI_DIR_RX, IFX_USIF_SPI_DMA_DISABLE);            

    IFX_USIF_SPI_IRQ_LOCK(port);
    ifx_usif_spi_start_rxfifo(port, rxbuf, rxsize);
    IFX_USIF_SPI_IRQ_UNLOCK(port); 
    
    return rxsize;
}

/**
 * \fn static INLINE int ifx_usif_spi_set_spi_mode(ifx_usif_device_t *dev)
 * \brief Sets the spi mode of the corresponding device. USIF SPI mode is per device 
 *      parameter. It is initialized during registeration
 *
 * \param   dev     Pointer to device
 * \return  0       OK
 * \return  -EBUSY  could not set usif spi mode because serial bus is busy
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE int 
ifx_usif_spi_set_spi_mode(ifx_usif_device_t *dev)
{
    u32 reg;
    int val = 0;
    int enabled = 0;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    struct ifx_usif_port *port;

    usif_cfg = &dev->conn_id;
    port = dev->port;

    if (port->prev_spi_mode == usif_cfg->spi_mode) {
        return 0;
    }

    /* Check if there is activity in USIF bus */
    if ((IFX_USIF_GET_FIFO_STATUS(port) & IFX_USIF_FIFO_STAT_TXRX_FFS_MASK) != 0x00) {
        printk(KERN_ERR "%s is busy\n", __func__);
        return -EBUSY;
    }
    
    switch(usif_cfg->spi_mode) {
        case IFX_USIF_SPI_MODE_0:
            val = IFX_USIF_MODE_CFG_SC_PH;
            break;
        case IFX_USIF_SPI_MODE_1:
            val = 0;
            break;
        case IFX_USIF_SPI_MODE_2:
            val = IFX_USIF_MODE_CFG_SC_POL | IFX_USIF_MODE_CFG_SC_PH;
            break;
        case IFX_USIF_SPI_MODE_3:
            val = IFX_USIF_MODE_CFG_SC_POL;
            break;
        default: /* Mode 0*/
            val = IFX_USIF_MODE_CFG_SC_PH; 
            break;
    }
    IFX_USIF_SPI_IRQ_LOCK(port);
    
    enabled = IFX_USIG_GET_CLOCK_STATUS(port) & IFX_USIF_CLC_STAT_RUN;
    if (enabled) {
        ifx_usif_spi_clc_cfg_mode(port);
    }  
    reg = IFX_USIF_GET_MODE_CONFIG(port);

    reg &= ~IFX_USIF_MODE_CFG_SC_MODE_MASK;

    reg |= val;
    IFX_USIF_SET_MODE_CONFIG(reg, port);

    if (enabled) {
        ifx_usif_spi_clc_run_mode(port);
    }
    IFX_USIF_SPI_IRQ_UNLOCK(port);
    port->prev_spi_mode = usif_cfg->spi_mode;
    return 0;
}

/**
 * \fn static INLINE void ifx_usif_spi_tx_setup(struct ifx_usif_port *port)
 * \brief USIF SPI set TX mode 
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void 
ifx_usif_spi_tx_setup(struct ifx_usif_port *port)
{
    if (port->opts.modeRxTx != IFX_USIF_SPI_MODE_TX) {
        ifx_usif_spi_rxtx_mode_set(port, IFX_USIF_SPI_MODE_TX);
    }
}

/**
 * \fn static INLINE void ifx_usif_spi_rx_setup(struct ifx_usif_port *port)
 * \brief  USIF SPI set RX mode 
 *
 * \param   port    Pointer to structure #ifx_usif_port
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void 
ifx_usif_spi_rx_setup(struct ifx_usif_port *port)
{
    if (port->opts.modeRxTx != IFX_USIF_SPI_MODE_RX) {
        ifx_usif_spi_rxtx_mode_set(port, IFX_USIF_SPI_MODE_RX);
    }
}

/**
 * \fn static void ifx_usif_spi_enqueue(IFX_USIF_QUEUE_t *queue)
 * \brief USIF add queue entry to priority queue
 *
 * \param   queue    Pointer to structure #IFX_USIF_QUEUE_t
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void 
ifx_usif_spi_enqueue(IFX_USIF_QUEUE_t *queue)
{
    ifx_usif_device_t     *dev;
    struct ifx_usif_port *port;
    
    IFX_KASSERT((queue != NULL), ("%s should never happen\n", __func__));
    dev = queue->dev;
    port = dev->port;
    IFX_USIF_PRINT(port, USIF_MSG_QUEUE, "%s dev %s prio %d enqueued\n",
        __func__, dev->dev_name, dev->dev_prio);
    
    IFX_USIF_SPI_Q_LOCK_BH(port);
    if (dev->dev_prio == IFX_USIF_SPI_PRIO_ASYNC) {
        TAILQ_INSERT_TAIL(&port->usif_asyncq, queue, q_next);
    }
    else {
        TAILQ_INSERT_TAIL(&port->usif_syncq[dev->dev_prio], queue, q_next);
    }
    dev->stats.enqueue++;
    IFX_USIF_SPI_Q_UNLOCK_BH(port);
}

/**
 * \fn static void ifx_usif_spi_dequeue(IFX_USIF_QUEUE_t *queue) 
 * \brief USIF remove queue entry from priority queue
 *
 * \param   queue    Pointer to structure #IFX_USIF_QUEUE_t
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void
ifx_usif_spi_dequeue(IFX_USIF_QUEUE_t *queue) 
{
    ifx_usif_device_t     *dev;
    struct ifx_usif_port *port;
    
    IFX_KASSERT((queue != NULL), ("%s should never happen\n", __func__));
    dev = queue->dev;
    port = dev->port;
    IFX_USIF_PRINT(port, USIF_MSG_QUEUE, "%s dev %s prio %d dequeued\n",
        __func__, dev->dev_name, dev->dev_prio);   
    
    IFX_USIF_SPI_Q_LOCK_BH(port);
    if (dev->dev_prio == IFX_USIF_SPI_PRIO_ASYNC) {
        if (!TAILQ_EMPTY(&port->usif_asyncq)) { /* Sanity check */
            TAILQ_REMOVE(&port->usif_asyncq, queue, q_next);
        }
    }
    else {
        if (!TAILQ_EMPTY(&port->usif_syncq[dev->dev_prio])) { /* Sanity check */
            TAILQ_REMOVE(&port->usif_syncq[dev->dev_prio], queue, q_next);
        }
    }
    dev->stats.dequeue++;
    IFX_USIF_SPI_Q_UNLOCK_BH(port);
}

/**
 * \fn static void ifx_usif_spi_cs_lock(ifx_usif_device_t *dev)
 * \brief USIF chip select function, set spi mode, baudrate, call registered 
 *     device-specific cs set function.
 *
 * \param   dev    Pointer to structure #ifx_usif_device_t
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static void 
ifx_usif_spi_cs_lock(ifx_usif_device_t *dev)
{
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    struct ifx_usif_port *port;

    port = dev->port;
    if (port->usif_cs_locked == IFX_TRUE) {
        printk(KERN_ERR "Fatal error: %s locked already before\n", __func__);
        return;
    }
    IFX_USIF_PRINT(dev->port, USIF_MSG_LOCK, "%s %s enter\n", __func__, dev->dev_name);

    port->usif_cs_locked = IFX_TRUE;

    usif_cfg = &dev->conn_id;
    ifx_usif_spi_set_spi_mode(dev);
    ifx_usif_spi_set_baudrate(port, usif_cfg->baudrate);
    if (usif_cfg->csset_cb != NULL) {
        usif_cfg->csset_cb(IFX_USIF_SPI_CS_ON, usif_cfg->cs_data);
    }
}

/**
 * \fn static INLINE void ifx_usif_spi_cs_unlock(ifx_usif_device_t *dev)
 * \brief USIF chip unselect function,  call registered device-specific cs reset function.
 *
 * \param   dev    Pointer to structure #ifx_usif_device_t
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void
ifx_usif_spi_cs_unlock(ifx_usif_device_t *dev)
{
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    struct ifx_usif_port *port;

    port = dev->port;
    if (port->usif_cs_locked == IFX_FALSE) {
        printk(KERN_ERR "Fatal error: %s unlocked already before\n", __func__);
        return;
    }
    usif_cfg = &dev->conn_id;
    if(usif_cfg->csset_cb != NULL) {
        usif_cfg->csset_cb(IFX_USIF_SPI_CS_OFF, usif_cfg->cs_data);
    }
    port->usif_cs_locked = IFX_FALSE;
    IFX_USIF_PRINT(port, USIF_MSG_LOCK, "%s %s exit\n", __func__, dev->dev_name);
}

/**
 *\fn int ifx_usif_spiLock(IFX_USIF_SPI_HANDLE_t handler)
 *\brief  Called to lock and reserve the whole USIF SPI interface 
 * for the given 'handler'
 *
 * The chipselect, belonging to this USIF SPI session is already 
 * activated. This means the chipselect callback is called. 
 * After complete data transmission and reception, ifx_usif_spiUnLock 
 * has to be called to release the USIF SPI interface again for 
 * other clients.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \return  = 0
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiLock(IFX_USIF_SPI_HANDLE_t handler)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    struct ifx_usif_port *port;
    IFX_USIF_QUEUE_t     *pqueue;

    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    pqueue = &dev->queue;
    usif_cfg = &dev->conn_id;
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
    ifx_usif_spi_enqueue(pqueue);
 
    /* 
     * If no async support is avaiable, trigger the USIF kernel thread and
     * wait pending till job is done. 
     */
#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
    IFX_USIF_PRINT(port, USIF_MSG_TASKLET, "%s raise fake interrupt\n", __func__);
    ifx_usif_spi_start_tasklet(port);
#else
    IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s wake up usif kernel thread\n", __func__);
    /* Kick off USIF kernel thread */
    IFX_USIF_SPI_WAKEUP_EVENT(port->usif_thread_wait, IFX_USIF_SPI_THREAD_EVENT, port->event_flags);
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */
    /* Wait till wake up from USIF kernel thread */
    IFX_USIF_SPI_WAIT_EVENT(dev->dev_thread_wait, IFX_USIF_SPI_DEV_THREAD_EVENT, dev->event_flags); 
    IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s wakeup received from usif kernel thread\n", __func__);
    return 0;
}
EXPORT_SYMBOL(ifx_usif_spiLock);

/**
 *\fn int ifx_usif_spiUnlock(IFX_USIF_SPI_HANDLE_t handler)
 * \brief  This function releases the USIF SPI lock that was placed before by calling ifx_SscLock.
 *
 * This function also inactivate the chipselect signal, which was set in ifx_SscLock.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \return  = 0
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiUnlock(IFX_USIF_SPI_HANDLE_t handler)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg = NULL;
    struct ifx_usif_port *port = NULL;
    IFX_USIF_QUEUE_t     *pqueue;
    
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    pqueue = &dev->queue;    
    usif_cfg = &dev->conn_id;
    port = dev->port;
    
    if (port->lock_qentry != &dev->queue) {
        /* We do not hold the lock, therefore we can not release it! */
        return -1;
    }

   /* Just forget about the lock, then the USIF driver would just take it
    * as a normel queue entry 
    */    
    ifx_usif_spi_cs_unlock(dev);
    
    port->lock_qentry = NULL;
    pqueue->txbuf          = NULL;
    pqueue->txsize         = 0;
    pqueue->rxbuf          = NULL;
    pqueue->rxsize         = 0;
    return 0;
}
EXPORT_SYMBOL(ifx_usif_spiUnlock);

/**
 * \fn int ifx_usif_spiSetBaud(IFX_USIF_SPI_HANDLE_t handler, unsigned int baud)
 * \brief Configures the Baudrate of a given connection.
 *
 * The baudrate can also be change multiple times 
 * for a single connection. The baudrate change 
 * will take place for the next call of ifx_usif_spiTx, 
 * ifx_usif_spiRx or ifx_usif_spiTxRx.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   baud       Baudrate to configure. This value can be rounded
 *                     during the calculation of the USIF SPI clock divider
 *
 * \return  = 0        OK
 *          < 0        error number 
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiSetBaud(IFX_USIF_SPI_HANDLE_t handler, unsigned int baud)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;

    /* Sanity check */
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    usif_cfg = &dev->conn_id;
    /* XXX, protection because of being used in other places */
    usif_cfg->baudrate = baud;
    return 0;
}
EXPORT_SYMBOL(ifx_usif_spiSetBaud);

/**
 * \fn static int ifx_usif_spi_txdma(struct ifx_usif_port *port, char *txbuf, int txsize)
 * \brief Called to transmit the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_usif_port
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 *          < 0        error number
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int
ifx_usif_spi_txdma(struct ifx_usif_port *port, char *txbuf, int txsize)
{    
    int retval = 0;
    struct dma_device_info* dma_dev;
    
    dma_dev = port->dma_dev;
    
    /** 
     * Set a flag that we are waiting for the DMA to complete. This flag
     * will be reseted again in the DMA interrupt. 
     * NB, it must be ahead of the following stuff, because once descriptor
     * is prepared, interrupt may come back immediately
     */
    atomic_set(&port->dma_wait_state, 1);

    ifx_usif_spi_dma_setup(port, IFX_USIF_SPI_DIR_TX, IFX_USIF_SPI_DMA_ENABLE);
    
    /**
     * Run in tasklet or kernel thread, DMA tasklet may run the same function
     * Lock must be used.
     * Although descriptor length field requires multiple of dma burst length,
     * The real size should be set up, otherwise, more clocks than expected will
     * be transmitted to the serial line. 
     */
    IFX_USIF_SPI_IRQ_LOCK(port);
    retval = dma_device_write(dma_dev, txbuf, txsize, NULL);
    IFX_USIF_SPI_IRQ_UNLOCK(port);
    IFX_KASSERT(retval == txsize, ("%s retval %d != txsize %d\n", 
        __func__, retval, txsize));

    return retval;
}

/**
 * \fn static int ifx_usif_spi_rxdma(struct ifx_usif_port *port, char *rxbuf, int rxsize)
 * \brief Called to receive the data using DMA mode .
 *
 * \param   port       Pointer to structure #ifx_usif_port
 * \param   rxbuf      Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes received
 *          < 0        error number
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int
ifx_usif_spi_rxdma(struct ifx_usif_port *port, char *rxbuf, int rxsize)
{
    char *pbuf;
    int   dma_rxsize;
    struct dma_device_info* dma_dev;
    dma_dev = port->dma_dev;

    /* Backup original buffer, so that later we can find it in dma handler */
    port->rxbuf_ptr = rxbuf;
    port->rx_len = rxsize;

    /*
     * HW WAR, DMA always burst the data. This means at least it will burst word
     * If last 1~3 bytes hit, round up to word, but it is out of original buffer.
     * memory copy will be involved.
     * XXX, a safe way is to round up to DMA burst length.
     */
    if ((rxsize & 0x3)){ /* Can't be divisible by 4 */
        pbuf = port->dma_rxbuf;
        dma_rxsize = (rxsize & ~0x3) + 4; /* Round up one dword to make sure enough space */
        IFX_KASSERT(dma_rxsize <= DEFAULT_USIF_SPI_FRAGMENT_SIZE, ("%s fragment %d out of range\n",
            __func__, dma_rxsize));
    }
    else {
        pbuf = rxbuf;
        dma_rxsize = rxsize;
    }
    /* Disable interrupt */
    IFX_USIF_SET_INT_MASK(0, port);    
    /* NB, DMA descriptoer must be setup before MRPS */
    dma_device_desc_setup(dma_dev, pbuf, dma_rxsize);

    /* D+ RX always enabled, RX Class MASK */
    ifx_usif_spi_dma_setup(port, IFX_USIF_SPI_DIR_RX, IFX_USIF_SPI_DMA_ENABLE);

    /* Enable SPI DMA channel */
    (dma_dev->rx_chan[dma_dev->current_rx_chan])->open(dma_dev->rx_chan[dma_dev->current_rx_chan]);

    /* 
     * Set a flag that we are waiting for the DMA to complete. This flag
     * will be reseted again in the DMA interrupt. 
     */
    atomic_set(&port->dma_wait_state, 1);

    if (rxsize > IFX_USIF_MRPS_MAX) {
        rxsize = IFX_USIF_MRPS_MAX;
    }
    /* This will trigger the clock and shift in data */
    smp_mb();
    IFX_USIF_CONFIG_RX_MRPS(SM(rxsize, IFX_USIF_MRPS_CTRL_MRPS), port);
    return rxsize;
}

/**
 *\fn int ifx_usif_spiTxRx (IFX_USIF_SPI_HANDLE_t handler, char *txbuf, u32 txsize, char *rxbuf, u32 rxsize)
 *\brief Called to transmit/receive to/from USIF SPI in one step.
 
 * This means that the data transmission and reception is done in parallel.
 * No DMA is possible here. The USIF SPI driver sets the chipselect when the 
 * data transmission starts and resets it when the transmission is 
 * completed. The transmit and receive buffer memory allocation and 
 * de-allocation is done by the USIF SPI client.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   txbuf     Pointer to the data packet to transmit
 * \param   txsize    Amount of Bytes to transmit
 * \param   rxbuf     Pointer to store the received data packet
 * \param   rxsize    Amount of Bytes to receive.
 * \return  >= 0      Number of bytes received (if rx_buf != 0) or transmitted
 * \return  < 0       error number 
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int
ifx_usif_spiTxRx(IFX_USIF_SPI_HANDLE_t handler, char *txbuf, u32 txsize, char *rxbuf, u32 rxsize)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    IFX_USIF_QUEUE_t     *pqueue;
    struct ifx_usif_port *port;
    struct usif_device_stats *stats;

    /* Sanity check */
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    IFX_KASSERT(!((rxbuf == NULL) && (rxsize == 0) 
        && (txbuf == NULL)  && (txsize == 0)),  
        ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    stats = &dev->stats;
    port = dev->port;
    
    if (in_interrupt()) {
        stats->context_err++;
        printk(KERN_ERR "%s can't be called in interupt context< irq, softirq, tasklet>\n", __func__);
        return 0;
    }
    usif_cfg = &dev->conn_id;
    pqueue = &dev->queue;

    if ((txsize > usif_cfg->fragSize) || (rxsize > usif_cfg->fragSize)) {
        stats->frag_err++;
        printk(KERN_ERR "%s Device driver must do its own fragmentation tx %d, rx %d > %d\n", 
            __func__, txsize, rxsize, usif_cfg->fragSize); 
        return 0;
    }
    
   /*
    * Ensure that only asynchronous USIF Handles could enqueue a 
    * synchronous request. The parameter 'handle_type' is set during the 
    * ConnId allocation process. 
    */
    if (pqueue->handle_type != IFX_USIF_SPI_HANDL_TYPE_SYNC) {
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
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s same queue has queued more than once\n", __func__);
        return 0;
    }
    /* Add pointer and sizes to the queue entry of this USIF handle. */
    pqueue->txbuf          = txbuf;
    pqueue->txsize         = txsize;
    pqueue->rxbuf          = rxbuf;
    pqueue->rxsize         = rxsize;
    atomic_set(&pqueue->isqueued, 1);
    pqueue->exchange_bytes = 0;
    pqueue->request_lock   = IFX_FALSE;
    memset(&pqueue->callback, 0, sizeof (IFX_USIF_SPI_ASYNC_CALLBACK_t));

    /* Add queue entry to priority queue */
    ifx_usif_spi_enqueue(pqueue);
    
    /* If no async support is avaiable, trigger the USIF kernel thread and
     * wait pending till job is done. 
     */
#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
    IFX_USIF_PRINT(port, USIF_MSG_TASKLET, "%s raise fake interrupt\n", __func__);
    ifx_usif_spi_start_tasklet(port);
#else
    IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s wake up usif kernel thread\n", __func__);
    /* Kick off USIF kernel thread */
    IFX_USIF_SPI_WAKEUP_EVENT(port->usif_thread_wait, IFX_USIF_SPI_THREAD_EVENT, port->event_flags);
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */
    /* Wait till wakeup from USIF kernel thread */
    IFX_USIF_SPI_WAIT_EVENT(dev->dev_thread_wait, IFX_USIF_SPI_DEV_THREAD_EVENT, dev->event_flags);
    IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s wakeup event received from usif kernel thread\n", __func__);

    /* Reset queue pointer */
    pqueue->txbuf = NULL;
    pqueue->rxbuf = NULL;
    return pqueue->txsize + pqueue->rxsize;
}
EXPORT_SYMBOL(ifx_usif_spiTxRx);

/**
 *\fn int ifx_usif_spiTx(IFX_USIF_SPI_HANDLE_t handler, char *txbuf, u32 txsize)
 *\brief Called to transmit the data.
 
 * transmission starts and resets it when the transmission
 * the transmit buffer is done by the USIF SPI client.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   txbuf      Pointer to the data packet to transmit
 * \param   txsize     Amount of Bytes to transmit
 * \return  >= 0       Number of bytes transmitted
 * \return  < 0        error number 
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int
ifx_usif_spiTx(IFX_USIF_SPI_HANDLE_t handler, char *txbuf, u32 txsize)
{
    return ifx_usif_spiTxRx(handler, txbuf, txsize, NULL, 0);
}
EXPORT_SYMBOL(ifx_usif_spiTx);

/**
 *\fn int ifx_usif_spiRx(IFX_USIF_SPI_HANDLE_t handler, char *rxbuf, u32 rxsize)
 *\brief Called to receive the data.

 * The USIF SPI driver sets the chipselect when the data reception starts and 
 * resets it when the reception is completed. The memory allocation and 
 * de-allocation of the receive buffer is done by the USIF SPI client.
 *
 * \param   handler    Handle of the connection where to make the 
 *                     configuration on
 * \param   rxbuf      Pointer to the data packet to be received
 * \param   rxsize     Amount of Bytes to be received
 * \return  >= 0       Number of bytes received
 * \return  < 0        error number 
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int
ifx_usif_spiRx(IFX_USIF_SPI_HANDLE_t handler, char *rxbuf, u32 rxsize)
{
    return ifx_usif_spiTxRx(handler, NULL, 0, rxbuf, rxsize);
}
EXPORT_SYMBOL(ifx_usif_spiRx);

/**
 * \fn static int ifx_usif_spi_serve_qentry(struct ifx_usif_port *port)
 * \brief Called to serve every queue entry and it is a common function for
 * USIF kernel thread and tasklet 
 *
 * \param   port       Pointer to structure #ifx_usif_port  
 * \return  1          Continue to loop this function until return 0
 * \return  0          Immediately exit this function. For kernel thread,
 *                     it will sleep, for tasklet, it will exit dynamically
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_serve_qentry(struct ifx_usif_port *port)
{
    IFX_USIF_QUEUE_t  *qentry = NULL;
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg = NULL;
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
        IFX_USIF_SPI_Q_LOCK_BH(port);
        if (!TAILQ_EMPTY(&port->usif_asyncq)) {
            qentry = TAILQ_FIRST(&port->usif_asyncq);
        }
        else {
            int  i;
            
            /* Choose the highest queue entry first */
            for (i = IFX_USIF_SPI_PRIO_HIGH; i >= IFX_USIF_SPI_PRIO_LOW; i--) {
                if (!TAILQ_EMPTY(&port->usif_syncq[i])){
                    qentry = TAILQ_FIRST(&port->usif_syncq[i]);
                    break;
                }
            }
        }
        IFX_USIF_SPI_Q_UNLOCK_BH(port);
        
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
    usif_cfg = &dev->conn_id;
    dma_dev = port->dma_dev;
    /* This will pass cs to port for class information per queue entry */
    port->current_dev = dev;
    if (qentry->txbuf != NULL) {
        int tx_dma_aligned = ((((u32)qentry->txbuf) & ((dma_dev->tx_burst_len << 2) - 1)) == 0) ? 1 : 0;

        if ((port->usif_cs_locked == IFX_FALSE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect set callback of the USIF-Handle */
            ifx_usif_spi_cs_lock(dev);
        }

        ifx_usif_spi_tx_setup(port);
        /* If buffer not aligned on DMA burst length, fall back to FIFO */
        if ((qentry->txsize > usif_cfg->maxFIFOSize) && (tx_dma_aligned == 1)) {
            IFX_USIF_PRINT(port, USIF_MSG_TX_DMA, "%s TX DMA enter\n", __func__);
            ifx_usif_spi_txdma(port, qentry->txbuf, qentry->txsize);
            port->stats.txDma++;
   
            /* 
             * Reset the data pointer, because this data are done on the
             * USIF hardware. 
             */
            qentry->txbuf = NULL;
            /* Count the number of transmitted bytes for this queue entry */
            qentry->exchange_bytes = qentry->txsize;
            dev->stats.txBytes += qentry->txsize;
            port->stats.txBytes += qentry->txsize;
            port->stats.txDmaBytes += qentry->txsize;
            return 0;
        }
        else {
            ifx_usif_spi_txfifo(port, qentry->txbuf, qentry->txsize);
            port->stats.txFifo++;
            IFX_USIF_PRINT(port, USIF_MSG_TX_FIFO, "%s TX FIFO enter\n", __func__);

            /* 
             * Reset the data pointer, because this data are done on the
             * USIF hardware. 
             */
            qentry->txbuf = NULL;
            /* Count the number of transmitted bytes for this queue entry */
            qentry->exchange_bytes = qentry->txsize;
            dev->stats.txBytes += qentry->txsize;
            port->stats.txBytes += qentry->txsize;
            /* 
             * NB, Make sure data has been sent out 
             * XXX, remove polling by checking TX_FIN
             */
            ifx_usif_spi_wait_tx_finished(port);
            return 1;
        }
    }
    else if (qentry->rxbuf != NULL) {
        int rx_dma_aligned = ((((u32) qentry->rxbuf) & ((dma_dev->rx_burst_len << 2) - 1)) == 0) ? 1 : 0;

        if ((port->usif_cs_locked == IFX_FALSE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect set callback of the USIF-Handle */
            ifx_usif_spi_cs_lock(dev);       
        }

        ifx_usif_spi_rx_setup(port);
        /* If buffer not aligned on DMA burst length, fall back to FIFO */
        if ((qentry->rxsize > usif_cfg->maxFIFOSize) && (rx_dma_aligned == 1)) {
            IFX_USIF_PRINT(port, USIF_MSG_RX_DMA, "%s RX DMA enter\n", __func__);
            ifx_usif_spi_rxdma(port, qentry->rxbuf, qentry->rxsize);
            port->stats.rxDma++;
          
            /* 
             * Reset the data pointer, because this data are done on the
             * USIF hardware. 
             */
            qentry->rxbuf = NULL;
            /* Count the number of transmitted bytes for this queue entry */
            qentry->exchange_bytes += qentry->rxsize;
            dev->stats.rxBytes += qentry->rxsize;
            port->stats.rxBytes += qentry->rxsize;
            port->stats.rxDmaBytes += qentry->rxsize;
            return 0;
        }
        else {
            ifx_usif_spi_rxfifo(port, qentry->rxbuf, qentry->rxsize);
            port->stats.rxFifo++;
            IFX_USIF_PRINT(port, USIF_MSG_RX_FIFO, "%s RX FIFO enter\n", __func__);

            /* 
             * Reset the data pointer, because this data are done on the
             * USIF hardware. 
             */
            qentry->rxbuf = NULL;
            /* Count the number of recevied bytes for this queue entry */
            qentry->exchange_bytes += qentry->rxsize;
            dev->stats.rxBytes += qentry->rxsize;
            port->stats.rxBytes += qentry->rxsize;
            return 1;
        }
    }
    else if (qentry->request_lock) {
        /* A lock request found */
        IFX_USIF_PRINT(port, USIF_MSG_LOCK, "%s request lock enter\n", __func__);
        port->lock_qentry = qentry;
        qentry->request_lock = IFX_FALSE;
        
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;

        /* Reset current device */
        port->current_dev = NULL;
        /* 
         * Reset the flag in the queue element that this one is queued with a 
         * request to be served. 
         */
        atomic_set(&qentry->isqueued, 0);
        
        /* syncAPI has the last watchdog branch but nested callback has to dequeue it */
        if (qentry->handle_type == IFX_USIF_SPI_HANDL_TYPE_ASYNC) {
            ifx_usif_spi_dequeue(qentry);
        }
        /* Call the Chipselect set callback of the USIF-Handle */
        ifx_usif_spi_cs_lock(dev);        
  
        if (qentry->callback.pFunction) {
            /* 
             * Store the callback parameter local to cleanup the queue entry before 
             * calling the callback. 
             */
            IFX_USIF_SPI_ASYNC_CALLBACK_t callback = qentry->callback;
            
            qentry->callback.pFunction = NULL;
            IFX_USIF_PRINT(port, USIF_MSG_CALLBACK, "%s line%d request callback\n", __func__, __LINE__);
            /* Callback to call */
            callback.pFunction(callback.functionHandle, 0);
        }
        return 1;
    }
    else if (qentry->callback.pFunction != NULL) {
        /* 
         * Store the callback parameter local to cleanup the queue entry before 
         * calling the callback. 
         */
        IFX_USIF_SPI_ASYNC_CALLBACK_t callback = qentry->callback;
  
        qentry->callback.pFunction = NULL;
        
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        
        /* Reset current device */
        port->current_dev = NULL;
        /* 
         * Reset the flag in the queue element that this one is queued with a 
         * request to be served. 
         */
        atomic_set(&qentry->isqueued, 0); 
        ifx_usif_spi_dequeue(qentry);
        
        if ((port->usif_cs_locked == IFX_TRUE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect reset callback of the USIF-Handle */
            ifx_usif_spi_cs_unlock(dev);
        }

        IFX_USIF_PRINT(port, USIF_MSG_CALLBACK, "%s line%d direct callback\n", __func__, __LINE__);
      
        /* Callback to call */
        callback.pFunction(callback.functionHandle, 
                qentry->exchange_bytes);
        
        return 1;
    }
    else if (qentry->handle_type == IFX_USIF_SPI_HANDL_TYPE_SYNC) {
        if ((port->usif_cs_locked == IFX_TRUE) && (port->lock_qentry == NULL)) {
            /* Call the Chipselect reset callback of the USIF-Handle */
            ifx_usif_spi_cs_unlock(dev);
        }
  
        /* Wake up the pending thread */
        IFX_USIF_SPI_WAKEUP_EVENT(dev->dev_thread_wait, IFX_USIF_SPI_DEV_THREAD_EVENT, dev->event_flags);
        IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s wakeup USIF client kernel thread\n", __func__);
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        /* Reset current device */
        port->current_dev = NULL;
        /* 
         * Reset the flag in the queue element that this one is queued with a 
         * request to be served. 
         */
        atomic_set(&qentry->isqueued, 0);
        
        ifx_usif_spi_dequeue(qentry);
        /* syncAPI has no callback, we have to differentiate it */
        if (port->usif_cs_locked == IFX_TRUE) { 
            return 0;
        }
        else {
            return 1;
        }
    }
    else if ((qentry->handle_type == IFX_USIF_SPI_HANDL_TYPE_ASYNC) && (port->usif_cs_locked == IFX_TRUE)) {
        /* Serving the current queue entry is done */
        port->serve_qentry = NULL;
        return 0; /* Don't let it continue, but must restart tasklet in somewhere else */
    }    
    return 0;
}

#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
/**
 * \fn static INLINE void ifx_usif_spi_tasklet_serve_queue(struct ifx_usif_port *port)
 * \brief Called to serve every queue entry in tasklet
 *
 * \param   port       Pointer to structure #ifx_usif_port  
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void 
ifx_usif_spi_tasklet_serve_queue(struct ifx_usif_port *port)
{
    /* 
     * Serve queue entries till no queue entry anymore to serve, we wait for
     * DMA or the lock entry is not in the queue. 
     */
    while (ifx_usif_spi_serve_qentry(port));
}

/**
 * \fn static INLINE void ifx_usif_spi_tasklet(unsigned long arg)
 * \brief USIF tasklet implementation
 *
 * \param   arg       cast to structure #ifx_usif_port  
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void 
ifx_usif_spi_tasklet(unsigned long arg)
{
    struct ifx_usif_port *port = (struct ifx_usif_port *)arg;

    IFX_USIF_PRINT(port, USIF_MSG_TASKLET, "%s enter\n", __func__);
    ifx_usif_spi_tasklet_serve_queue(port);
}

/**
 * \fn static INLINE void ifx_usif_spi_tasklet_init(struct ifx_usif_port *port)
 * \brief USIF tasklet initialization
 *
 * \param   port       Pointer to structure #ifx_usif_port  
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE void
ifx_usif_spi_tasklet_init(struct ifx_usif_port *port)
{
    tasklet_init(&port->usif_txrxq, ifx_usif_spi_tasklet, (unsigned long)port);
}

#else
#define IFX_USIF_THREAD_OPTIONS   (CLONE_FS | CLONE_FILES | CLONE_SIGHAND)

/**
 * \fn static int ifx_usif_spi_kthread(void *arg)
 * \brief USIF kernel thread implementation function
 *
 * \param   arg       cast to structure #ifx_usif_port  
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static int 
ifx_usif_spi_kthread(void *arg)
{
    struct ifx_usif_port *port = (struct ifx_usif_port *)arg;
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
        while (ifx_usif_spi_serve_qentry(port));
  
        /* Wait for DMA interrupt or sync queue to wakes us up */
        IFX_USIF_SPI_WAIT_EVENT(port->usif_thread_wait, IFX_USIF_SPI_THREAD_EVENT, port->event_flags);
        IFX_USIF_PRINT(port, USIF_MSG_THREAD, "%s DMA or sync queue event received\n", __func__);
    }
    complete_and_exit(&port->thread_done, 0);
    return 0;
}

/**
 * \fn static INLINE int ifx_usif_spi_thread_init(struct ifx_usif_port *port)
 * \brief USIF kernel thread initialization
 *
 * \param   port       Pointer to structure #ifx_usif_port  
 * \return  none
 * \ingroup IFX_USIF_SPI_INTERNAL 
 */
static INLINE int 
ifx_usif_spi_thread_init(struct ifx_usif_port *port)
{
    init_completion(&port->thread_done);
    port->usif_pid = kernel_thread(ifx_usif_spi_kthread, (void*) port,
        IFX_USIF_THREAD_OPTIONS);
    IFX_USIF_PRINT(port, USIF_MSG_INIT, "%s pid %d\n", __func__, port->usif_pid);
    return 0;
}
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */

/** 
 * \fn int ifx_usif_spiAsyncTxRx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
 *       char *txbuf, int txsize, char *rxbuf, int rxsize)
 * \brief Transmit/receive to/from USIF in one step. It performs the data transmission and 
 *  then the data reception.

 * The USIF driver sets the chipselect when the data transmission starts and
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
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiAsyncTxRx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
        char *txbuf, int txsize, char *rxbuf, int rxsize)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    IFX_USIF_QUEUE_t     *pqueue;
    struct ifx_usif_port *port;
    struct usif_device_stats *stats;

   /* Sanity check */
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    IFX_KASSERT(!((rxbuf == NULL) && (rxsize == 0) 
        && (txbuf == NULL)  && (txsize == 0)),  
        ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    stats = &dev->stats;
    port = dev->port;

    if (!in_interrupt()) {
        stats->context_err++;
        printk(KERN_ERR "%s must be called in interrupt context<irq, softirq, tasklet>\n", __func__);
        return -1;
    }

    usif_cfg = &dev->conn_id;
    pqueue = &dev->queue;
    
    if ((txsize > usif_cfg->fragSize) || (rxsize > usif_cfg->fragSize)) {
        stats->frag_err++;
        printk(KERN_ERR "%s Device driver must do its own fragmentation\n", __func__); 
        return -1;
    }

    /* 
     * Ensure that only asynchronous USIF Handles could enqueue an 
     * asynchronous request. The parameter 'usifHandleType' is set during the 
     * ConnId allocation process. 
     */
    if (pqueue->handle_type != IFX_USIF_SPI_HANDL_TYPE_ASYNC) {
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
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s same queue has queued more than once\n", __func__);
        return (-1);
    }
    
    /* Add pointer and sizes to the queue entry of this USIF handle. */
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
    ifx_usif_spi_enqueue(pqueue);
    /* 
     * Trigger schedule or tasklet or fake interrupt according to different 
     * cases.
     */ 
    ifx_usif_spi_start_tasklet(port);
    return 0;
}
EXPORT_SYMBOL(ifx_usif_spiAsyncTxRx);

/** 
 * \fn int ifx_usif_spiAsyncTx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
 *       char *txbuf, int txsize)
 * \brief transmit the data, located at "txbuf". The "txsize" amount of bytes is 
 *  transmitted over USIF. 

 * The USIF driver sets the chipselect when the data transmission starts and
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
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int
ifx_usif_spiAsyncTx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
    char *txbuf, int txsize)
{
    return ifx_usif_spiAsyncTxRx(handler, pCallback, txbuf, txsize, NULL, 0);
}
EXPORT_SYMBOL(ifx_usif_spiAsyncTx);

/** 
 * \fn int ifx_usif_spiAsyncRx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
 *       char *rxbuf, int rxsize)
 * \brief Receive from USIF. The received data are stored at "rxbuf". The "rxsize" 
 *  describes the amount of bytes to receive from USIF.

 * The USIF driver sets the chipselect when the data reception starts and
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
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiAsyncRx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
    char *rxbuf, int rxsize)
{
    return ifx_usif_spiAsyncTxRx(handler, pCallback,  NULL, 0, rxbuf, rxsize);
}
EXPORT_SYMBOL(ifx_usif_spiAsyncRx);

/** 
 * \fn int ifx_usif_spiAsyncLock(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback)
 *
 * \brief This function locks and reserves the whole USIF interface for the given
    'handler'.

 *  The chipselect, belonging to this USIF session is already
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
 * \remarks This function actives the USIF chipselect of this 'handler' by calling
 * the callback function. 
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiAsyncLock(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg;
    struct ifx_usif_port *port;
    IFX_USIF_QUEUE_t     *pqueue;

    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));
    IFX_KASSERT((pCallback != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    pqueue = &dev->queue;
    usif_cfg = &dev->conn_id;
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

    /* Initialize unused variable */
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
    ifx_usif_spi_enqueue(pqueue);
 
    /* 
     * Calls the internal process to serve the queue. This routine would
     * immediately return in case  the USIF hardware is currently used to serve
     * another request. 
     */
    ifx_usif_spi_start_tasklet(port);
    return 0;
}
EXPORT_SYMBOL(ifx_usif_spiAsyncLock);
/** 
 * \fn int ifx_usif_spiAsyncUnLock(IFX_USIF_SPI_HANDLE_t handler)
 * \brief This function releases the USIF lock that was placed before by calling
 * \ref ifx_usif_spiAsyncLock. This function also inactivate the chipselect signal, which
 *  was set in \ref ifx_usif_spiAsyncLock.

 * \param handler Handle of the connection.
 *
 * \return Return (0) in case of success, otherwise (-1) in case of errors.
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int 
ifx_usif_spiAsyncUnLock(IFX_USIF_SPI_HANDLE_t handler)
{
    ifx_usif_device_t *dev;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg = NULL;
    struct ifx_usif_port *port = NULL;
    IFX_USIF_QUEUE_t     *pqueue;
    
    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    dev = (ifx_usif_device_t *)handler;
    pqueue = &dev->queue;
    usif_cfg = &dev->conn_id;
    port = dev->port;
    
    if (port->lock_qentry != pqueue) {
        /* We do not hold the lock, therefore we can not release it! */
        return -1;
    }

   /* Just forget about the lock, then the USIF driver would just take it
    * as a normel queue entry 
    */ 
    ifx_usif_spi_cs_unlock(dev);
   
    /* Reset to the default value */
    pqueue->txbuf          = NULL;
    pqueue->txsize         = 0;
    pqueue->rxbuf          = NULL;
    pqueue->rxsize         = 0;
    pqueue->exchange_bytes = 0;
    memset(&pqueue->callback, 0, sizeof (IFX_USIF_SPI_ASYNC_CALLBACK_t));
    
    port->lock_qentry = NULL;

    /* Restart tasklet since we are async release SPI bus */
    ifx_usif_spi_start_tasklet(port);    
    return 0;
}
EXPORT_SYMBOL(ifx_usif_spiAsyncUnLock);

#ifdef CONFIG_SYSCTL
/*
 * Deal with the sysctl handler api changing.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8)
#define	USIF_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(ctl_table *ctl, int write, struct file *filp, \
	  void __user *buffer, size_t *lenp)
#define	USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, filp, buffer, lenp)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
#define	USIF_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(ctl_table *ctl, int write, struct file *filp, \
	  void __user *buffer, size_t *lenp, loff_t *ppos)
#define	USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, filp, buffer, lenp, ppos)
#else /* Linux 2.6.32+ */
#define	USIF_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
	f(ctl_table *ctl, int write, \
	  void __user *buffer, size_t *lenp, loff_t *ppos)
#define	USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
	proc_dointvec(ctl, write, buffer, lenp, ppos)
#endif

enum {
    USIF_SPI_PRIV_FRAGMENT_SIZE  = 1,
    USIF_SPI_PRIV_FIFO_SIZE      = 2,
    USIF_SPI_PRIV_BAUDRATE       = 3,
    USIF_SPI_PRIV_MODE           = 4,
};

static int
USIF_SYSCTL_DECL(usif_sysctl_private, ctl, write, filp, buffer, lenp, ppos)
{
    ifx_usif_device_t *dev = ctl->extra1;
    IFX_USIF_SPI_CONFIGURE_t *usif_cfg = &dev->conn_id;
    struct ifx_usif_port *port;
    u32 val;
    int ret;

    port = dev->port;
    ctl->data = &val;
    ctl->maxlen = sizeof(val);
    if (write) {
        ret = USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,lenp, ppos);
        if (ret == 0) {
            switch ((long)ctl->extra2) {
                case USIF_SPI_PRIV_FRAGMENT_SIZE:
                    if (val < IFX_USIF_SPI_MIN_FRAGSIZE || val > IFX_USIF_SPI_MAX_FRAGSIZE)
                        return -EINVAL;
                    usif_cfg->fragSize = val;
                    port->usif_frag_size = val;
                    break;

                case USIF_SPI_PRIV_FIFO_SIZE:
                    if (val < IFX_USIF_SPI_FIFO_MIN_THRESHOULD || val > IFX_USIF_SPI_FIFO_MAX_THRESHOULD)
                        return -EINVAL;
                    usif_cfg->maxFIFOSize = val;
                    break;

                case USIF_SPI_PRIV_BAUDRATE:
                    /* XXX, sanity check */
                    usif_cfg->baudrate = val;
                    break;

                case USIF_SPI_PRIV_MODE:
                    ret = -EINVAL;
                    break;

                default:
                    return -EINVAL;
            }
        }
    } 
    else {
        switch ((long)ctl->extra2) {
            case USIF_SPI_PRIV_FRAGMENT_SIZE:
                val = usif_cfg->fragSize;
                break;

            case USIF_SPI_PRIV_FIFO_SIZE:
                val = usif_cfg->maxFIFOSize;
                break;

            case USIF_SPI_PRIV_BAUDRATE:
                val = usif_cfg->baudrate;
                break;

            case USIF_SPI_PRIV_MODE:
                val = usif_cfg->spi_mode;
                break;

            default:
                return -EINVAL;
        }
        ret = USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos);
    }
    return ret;
}

static const 
ctl_table usif_sysctl_template[] = {
    /* NB: must be last entry before NULL */
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "fragment_size",
      .mode     = 0644,
      .proc_handler = usif_sysctl_private,
      .extra2   = (void *)USIF_SPI_PRIV_FRAGMENT_SIZE,
    },
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "fifosize",
      .mode     = 0644,
      .proc_handler = usif_sysctl_private,
      .extra2   = (void *)USIF_SPI_PRIV_FIFO_SIZE,
    },
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "baudrate",
      .mode     = 0644,
      .proc_handler = usif_sysctl_private,
      .extra2   = (void *)USIF_SPI_PRIV_BAUDRATE,
    },
    { IFX_INIT_CTL_NAME(CTL_AUTO)
      .procname = "spimode",
      .mode     = 0644,
      .proc_handler = usif_sysctl_private,
      .extra2   = (void *)USIF_SPI_PRIV_MODE,
    },
    { 0 }
};

static void
ifx_usif_spi_sysctl_attach(ifx_usif_device_t *dev)
{
    int i, space;

    space = 5 * sizeof(struct ctl_table) + sizeof(usif_sysctl_template);
    dev->usif_sysctls = kmalloc(space, GFP_KERNEL);
    if (dev->usif_sysctls == NULL) {
        printk("%s: no memory for sysctl table!\n", __func__);
        return;
    }

    /* setup the table */
    memset(dev->usif_sysctls, 0, space);
    IFX_SET_CTL_NAME(dev->usif_sysctls[0], CTL_DEV);
    dev->usif_sysctls[0].procname = "dev";
    dev->usif_sysctls[0].mode = 0555;
    dev->usif_sysctls[0].child = &dev->usif_sysctls[2];
    /* [1] is NULL terminator */
    IFX_SET_CTL_NAME(dev->usif_sysctls[2], CTL_AUTO);
    dev->usif_sysctls[2].procname = dev->dev_name;
    dev->usif_sysctls[2].mode = 0555;
    dev->usif_sysctls[2].child = &dev->usif_sysctls[4];
    /* [3] is NULL terminator */
    /* copy in pre-defined data */
    memcpy(&dev->usif_sysctls[4], usif_sysctl_template,
        sizeof(usif_sysctl_template));

    /* add in dynamic data references */
    for (i = 4; dev->usif_sysctls[i].procname; i++) {
        if (dev->usif_sysctls[i].extra1 == NULL) {
            dev->usif_sysctls[i].extra1 = dev;
        }
    }

    /* tack on back-pointer to parent device */
    dev->usif_sysctls[i-1].data = dev->dev_name;

    /* and register everything */
    dev->usif_sysctl_header = IFX_REGISTER_SYSCTL_TABLE(dev->usif_sysctls);
    if (!dev->usif_sysctl_header) {
        printk("%s: failed to register sysctls!\n", dev->dev_name);
        kfree(dev->usif_sysctls);
        dev->usif_sysctls = NULL;
    }
}

static void
ifx_usif_spi_sysctl_detach(ifx_usif_device_t *dev)
{
    if (dev->usif_sysctl_header) {
        unregister_sysctl_table(dev->usif_sysctl_header);
        dev->usif_sysctl_header = NULL;
    }
    if (dev->usif_sysctls) {
        kfree(dev->usif_sysctls);
        dev->usif_sysctls = NULL;
    }
}

/* USIF Driver itself proc support for debug and future configuration */
enum {
    USIF_SPI_PRIV_DEBUG     = 1,
};

static int
USIF_SYSCTL_DECL(port_sysctl_private, ctl, write, filp, buffer, lenp, ppos)
{
    struct ifx_usif_port *port = ctl->extra1;
    u32 val;
    int ret;

    ctl->data = &val;
    ctl->maxlen = sizeof(val);
    if (write) {
        ret = USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer,lenp, ppos);
        if (ret == 0) {
            switch ((long)ctl->extra2) {
                case USIF_SPI_PRIV_DEBUG:
                    port->usif_debug = val;
                    break;

                default:
                    return -EINVAL;
            }
        }
    } 
    else {
        switch ((long)ctl->extra2) {
            case USIF_SPI_PRIV_DEBUG:
                val = port->usif_debug;
                break;

            default:
                return -EINVAL;
        }
        ret = USIF_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos);
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
      .extra2   = (void *)USIF_SPI_PRIV_DEBUG,
    },     
    { 0 }
};

static void
ifx_usif_spi_port_sysctl_attach(struct ifx_usif_port *port)
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
ifx_usif_spi_port_sysctl_detach(struct ifx_usif_port *port)
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
 *\fn IFX_USIF_SPI_HANDLE_t ifx_usif_spiAllocConnection(char *dev_name, IFX_USIF_SPI_CONFIGURE_t *connid)
 *\brief Allocate and create a Connection ID "ConnId"
 *
 * Allocate and create a Connection ID "ConnId" to communicate over USIF SPI.
 * This ConnId is needed for all remaining USIF SPI driver API calls. This 
 * ConnId is a handle that helps the USIF SPI driver to find the configuration 
 * that belongs to the connection. ConnId specific parameters are e.g. 
 * Baudrate, Priority, Chipselect Callback, etc.
 *
 * \param   dev_name    unique name for this connection. If null, will alloc
 *                      one unique name automatically
 * \param   connid      Connectin id 
 * \return  a handle "IFX_USIF_SPI_HANDLE_t" in case the allocation was successful. 
 *          In case of an error, the return handle is zero (NULL).
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
IFX_USIF_SPI_HANDLE_t 
ifx_usif_spiAllocConnection(char *dev_name, IFX_USIF_SPI_CONFIGURE_t *connid)
{
    struct ifx_usif_port *port;
    ifx_usif_device_t *p;
    ifx_usif_device_t *q;
    IFX_USIF_QUEUE_t *queue;
    char buf[IFX_USIF_SPI_MAX_DEVNAME] = {0};
    char *pname;

    port = &ifx_usif_spi_port;

    if (port->usif_ndevs >= IFX_USIF_SPI_MAX_DEVICE) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, 
            "%s device number out of range\n", __func__);
        return NULL;
    }

    if (connid == NULL) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR,
            "%s must provide connection portrmation!\n", __func__);
        return NULL;
    }

    if ((connid->spi_mode < IFX_USIF_SPI_MODE_0) 
        || (connid->spi_mode > IFX_USIF_SPI_MODE_3)) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR,
            "%s invalid spi mode <%d~%d>!\n", __func__, IFX_USIF_SPI_MODE_0, IFX_USIF_SPI_MODE_3);
        return NULL;
    }

    if (connid->spi_prio < IFX_USIF_SPI_PRIO_LOW 
        || (connid->spi_prio > IFX_USIF_SPI_PRIO_MAX)) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, 
            "%s invalid priority <%d~%d>!\n", __func__, IFX_USIF_SPI_PRIO_LOW, IFX_USIF_SPI_PRIO_MAX);
    }
    
    if (connid->csset_cb == NULL) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s must provide cs function\n", __func__);
        return NULL;
    }

    if (connid->fragSize < IFX_USIF_SPI_MIN_FRAGSIZE 
        || connid->fragSize > IFX_USIF_SPI_MAX_FRAGSIZE) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s invalid fragment size <%d~%d>!\n", __func__,
            IFX_USIF_SPI_MIN_FRAGSIZE, IFX_USIF_SPI_MAX_FRAGSIZE);
        return NULL;
    }

    if (connid->maxFIFOSize < IFX_USIF_SPI_FIFO_MIN_THRESHOULD 
        || connid->maxFIFOSize > IFX_USIF_SPI_FIFO_MAX_THRESHOULD) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s invalid fifo size <%d~%d>!\n", __func__,
            IFX_USIF_SPI_FIFO_MIN_THRESHOULD, IFX_USIF_SPI_FIFO_MAX_THRESHOULD);
        return NULL;
    }
    /* If no name specified, will assign one name for identification */
    if (dev_name == NULL) {
        sprintf(buf, "usif%d", port->usif_ndevs);
        pname = buf;
    }
    else {
        if (strlen(dev_name) > (IFX_USIF_SPI_MAX_DEVNAME - 1) ) {
            IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s device name is too long\n", __func__);
            return NULL;
        }
        pname = dev_name;
    }

    p = kmalloc(sizeof(ifx_usif_device_t), GFP_KERNEL);
    if (p == NULL) {
        IFX_USIF_PRINT(port, USIF_MSG_ERROR,"%s failed to allocate memory\n", __func__);
        return NULL;
    }
    memset(p, 0, sizeof (ifx_usif_device_t));

    IFX_USIF_SPI_SEM_LOCK(port->dev_sem);
    TAILQ_FOREACH(q, &port->usif_devq, dev_entry) {
        if (strcmp(q->dev_name, pname) == 0) {
            kfree(p);
            IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);
            IFX_USIF_PRINT(port, USIF_MSG_ERROR, "%s device registered already!\n", __func__);
            return NULL;
        }
    }
    IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);

    /* Follow net device driver name rule */
    memcpy(p->dev_name, pname, IFX_USIF_SPI_MAX_DEVNAME);
    memcpy((char *)&p->conn_id, (char *)connid, sizeof (IFX_USIF_SPI_CONFIGURE_t));

    queue = &p->queue;
    /* Queue handler type converted from priority */
    if (connid->spi_prio == IFX_USIF_SPI_PRIO_ASYNC) {
        queue->handle_type = IFX_USIF_SPI_HANDL_TYPE_ASYNC;
    }
    else {
        queue->handle_type = IFX_USIF_SPI_HANDL_TYPE_SYNC;
    }
    /* Back pointer to later usage */
    queue->dev = p;
    atomic_set(&queue->isqueued, 0);
    queue->request_lock = IFX_FALSE;
    /* 
     * Just for fast access, priority based on device, instead of packet 
     * Still keep per packet priority there for future change.
     */
    p->dev_prio = connid->spi_prio;
    
    IFX_USIF_SPI_WAKELIST_INIT(p->dev_thread_wait);

    p->port = port; /* back pointer to port for easy reference later */
    port->usif_ndevs++;
#ifdef CONFIG_SYSCTL
    ifx_usif_spi_sysctl_attach(p);
#endif /* CONFIG_SYSCTL */
    IFX_USIF_SPI_SEM_LOCK(port->dev_sem);
    TAILQ_INSERT_TAIL(&port->usif_devq, p, dev_entry);
    IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);

    IFX_USIF_PRINT(port, USIF_MSG_INIT, 
        "%s: device %s register sucessfully!\n", __func__, p->dev_name);
    return (IFX_USIF_SPI_HANDLE_t)p;
}
EXPORT_SYMBOL(ifx_usif_spiAllocConnection);

/*!
 *\fn int ifx_usif_spiFreeConnection(IFX_USIF_SPI_HANDLE_t handler)
 *\brief Release usif spi connnection
 * 
 * Release a ConnId handle that was allocated by the function ifx_SscAllocConnection
 * before. An allocated ConnId has to be released by the client driver module 
 * when the USIF SPI driver is not used anymore. Note that all allocated ConnId's should 
 * be released before the USIF SPI driver is unloaded from the kernel.
 * 
 * \param   handler    ConnId handle allocated by ifx_SscAllocConnection
 * \returns (0) in case of success, otherwise (-1) in case of errors.
 * \ingroup IFX_USIF_SPI_FUNCTIONS
 */
int  
ifx_usif_spiFreeConnection(IFX_USIF_SPI_HANDLE_t handler)
{
    ifx_usif_device_t *p;
    struct ifx_usif_port *port;
    ifx_usif_device_t *q, *next;

    IFX_KASSERT((handler != NULL), ("%s Invalid parameter\n", __func__));

    p = (ifx_usif_device_t *)handler;
    port = p->port;
    IFX_USIF_SPI_SEM_LOCK(port->dev_sem);
    TAILQ_FOREACH_SAFE(q, &port->usif_devq, dev_entry, next) {
        if (strcmp(q->dev_name, p->dev_name) ==  0) {
            TAILQ_REMOVE(&port->usif_devq, q, dev_entry);
        #ifdef CONFIG_SYSCTL
            ifx_usif_spi_sysctl_detach(q);
        #endif /* CONFIG_SYSCTL */
            kfree(q);
            port->usif_ndevs--;
            IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);
            IFX_USIF_PRINT(port, USIF_MSG_INIT, "%s device %s unregistered\n", __func__, p->dev_name);
            return 0;
        }
    }
    IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);
    return -1;
}
EXPORT_SYMBOL(ifx_usif_spiFreeConnection);

/**
 * \fn static int __init ifx_usif_spi_init (void)
 * \brief USIF-SPI module Initialization.
 *
 * \return -ENOMEM  Failed to allocate memory
 *         -EBUSY   Failed to iomap register space
 *         0        OK
 * \ingroup IFX_USIF_SPI_INTERNAL  
 */
static int __init
ifx_usif_spi_init(void)
{
    struct ifx_usif_port *port;
    int i;
    int ret_val = -ENOMEM;
    char ver_str[128] = {0};
    static int ifx_usif_spi_initialized = 0; 

    if (ifx_usif_spi_initialized == 1) {
        return 0;
    }
    else {
        ifx_usif_spi_initialized = 1;
    }
 
    memset(&ifx_usif_spi_port, 0, sizeof (struct ifx_usif_port));

    /* Set default values in ifx_usif_port */
    port = &ifx_usif_spi_port;

    port->port_idx = 0;
    
    /* default values for the HwOpts */
    port->opts.sync     = IFX_USIF_SPI_DEF_SYNC;
    port->opts.master   = IFX_USIF_SPI_DEF_MASTERSLAVE;
    port->opts.frmctl   = IFX_USIF_SPI_DEF_FRMCTL;
    port->opts.loopBack = IFX_USIF_SPI_DEF_LOOP_BACK;
    port->opts.echoMode = IFX_USIF_SPI_DEF_ECHO_MODE;
    port->opts.txInvert = IFX_USIF_SPI_DEF_TX_INVERT;
    port->opts.rxInvert = IFX_USIF_SPI_DEF_RX_INVERT;
    port->opts.shiftClk = IFX_USIF_SPI_DEF_SHIFT_CLK;
    port->opts.clkPO    = IFX_USIF_SPI_DEF_CLK_PO;
    port->opts.clkPH    = IFX_USIF_SPI_DEF_CLK_PH;
    port->opts.txIdleValue = IFX_USIF_SPI_DEF_TX_IDLE_STATE;
    port->opts.modeRxTx = IFX_USIF_SPI_DEF_MODE_RXTX;

    port->opts.charLen  = IFX_USIF_SPI_DEF_CHAR_LEN;
    port->opts.hdrCtrl  = IFX_USIF_SPI_DEF_HDR_CTRL;
    port->opts.wakeUp   = IFX_USIF_SPI_DEF_WAKEUP;

    port->baudrate      = IFX_USIF_SPI_DEF_BAUDRATE;
    port->prev_baudrate = 0;
    port->prev_spi_mode = IFX_USIF_SPI_MODE_UNKNOWN;
    port->usif_ndevs    = 0;
    port->usif_frag_size = DEFAULT_USIF_SPI_FRAGMENT_SIZE;
 
    /* Values specific to USIF SPI */
    port->mapbase = IFX_USIF_PHY_BASE;
    port->membase = ioremap_nocache(port->mapbase, IFX_USIF_SIZE);
    if (!port->membase) {
        printk(KERN_ERR "%s: Failed during io remap\n", __func__);
        ret_val = -EBUSY;
        goto errout1;
    }
    IFX_USIF_PRINT(port, USIF_MSG_INIT, "%s: mapbase 0x%08lx membase 0x%08x\n", 
        __func__, port->mapbase, (u32)port->membase);

    /* Register with DMA engine */
    port->dma_dev = dma_device_reserve("USIF");
    if (port->dma_dev == NULL) {
        printk(KERN_ERR "%s: Failed to reserve dma device!\n", __func__);
        goto errout2;
    }
    
    ifx_usif_spi_init_dma_device(port->dma_dev);
    strcpy(port->name, IFX_USIF_SPI_NAME);
    port->usif_cs_locked = IFX_FALSE;
    port->lock_qentry   = NULL;
    port->serve_qentry  = NULL;    
#ifdef CONFIG_IFX_USIF_SPI_DEBUG
    port->usif_debug = USIF_MSG_INIT | USIF_MSG_ERROR;
#else
    port->usif_debug = 0;
#endif /* CONFIG_IFX_USIF_SPI_DEBUG */

    atomic_set(&port->dma_wait_state, 0);

    /* Either USIF tasklet or USIF kernel thread support, not both */
#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
    ifx_usif_spi_tasklet_init(port);
#else
    ifx_usif_spi_thread_init(port);
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */

    /* The following buffer allocation for HW WAR, last 1~3 bytes in DMA 
     * It will make sure buffer will align on dma burst length
     */    
    port->dma_orig_txbuf = kmalloc(DEFAULT_USIF_SPI_FRAGMENT_SIZE \
        + ((port->dma_dev->tx_burst_len << 2) - 1), GFP_KERNEL);
    if (port->dma_orig_txbuf == NULL) {
        printk(KERN_ERR "%s: no memory for dma_orig_txbuf\n", __func__);
        goto errout3;
    }
    port->dma_txbuf = (char *)(((u32)( port->dma_orig_txbuf +   \
        ((port->dma_dev->tx_burst_len << 2) - 1)))               \
         & ~((port->dma_dev->tx_burst_len << 2) - 1));

    
    port->dma_orig_rxbuf = kmalloc(DEFAULT_USIF_SPI_FRAGMENT_SIZE \
        + ((port->dma_dev->rx_burst_len << 2) - 1), GFP_KERNEL);
    if (port->dma_orig_rxbuf == NULL) {
        printk(KERN_ERR "%s: no memory for dma_orig_rxbuf\n", __func__);
        goto errout4;
    }
    port->dma_rxbuf = (char *)(((u32)( port->dma_orig_rxbuf +  \
        ((port->dma_dev->rx_burst_len << 2) - 1)))              \
         & ~((port->dma_dev->rx_burst_len << 2) - 1));

    TAILQ_INIT(&port->usif_devq);
    TAILQ_INIT(&port->usif_asyncq);
    
    for (i = 0; i < IFX_USIF_SPI_PRIO_MAX; i++) {
        TAILQ_INIT(&port->usif_syncq[i]);
    }
    IFX_USIF_SPI_Q_LOCK_INIT(port);
    IFX_USIF_SPI_SEM_INIT(port->dev_sem);
    IFX_USIF_SPI_WAKELIST_INIT(port->usif_thread_wait);
    IFX_USIF_SPI_IRQ_LOCK_INIT(port, "ifx_usif_spi_lock");
    
    port->usif_fake_irq = IFX_USIF_TX_INT;                 
    ret_val = ifx_usif_int_wrapper.request(port->usif_fake_irq, ifx_usif_spi_fake_isr,
                  IRQF_DISABLED, "ifx_usif_tx", port);
    if (ret_val) {
        printk(KERN_ERR "%s: unable to get irq %d\n", __func__, port->usif_fake_irq);
        goto errout5;
    }   
    if (ifx_usif_spi_hwinit(port) < 0) {
        printk ("%s: hardware init failed for USIF SPI\n", __func__);
        goto errout5;
    }
#ifdef CONFIG_SYSCTL
    ifx_usif_spi_port_sysctl_attach(port);
#endif /* CONFIG_SYSCTL */

#ifdef CONFIG_IFX_PMCU
    ifx_usif_spi_pmcu_init(port);
#endif /* CONFIG_IFX_PMCU */
    ifx_usif_spi_proc_create();
    ifx_usif_spi_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    return 0;
    
errout5:
    ifx_usif_int_wrapper.free(port->usif_fake_irq, port);
errout4:
    kfree(port->dma_orig_rxbuf);
errout3:
    kfree(port->dma_orig_txbuf);
errout2:
    dma_device_unregister(port->dma_dev);
    dma_device_release(port->dma_dev);    
errout1:
    return (ret_val);
}

/**
 * \fn static void __exit ifx_usif_spi_exit (void)
 * \brief USIF SPI Module Cleanup.
 *
 * Upon removal of the USIF-SPI module this function will free all allocated 
 * resources and unregister devices. 
 * \return none
 * \ingroup IFX_USIF_SPI_INTERNAL
 */
static void __exit
ifx_usif_spi_exit(void)
{
    struct dma_device_info *dma_dev;
    struct ifx_usif_port *port = &ifx_usif_spi_port;
    
    /* Exit running mode */
    ifx_usif_spi_clc_cfg_mode(port);

    IFX_USIF_SPI_SEM_LOCK(port->dev_sem);
    if (!TAILQ_EMPTY(&port->usif_devq)) {
        IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);
        printk(KERN_ERR "%s USIF devices still attached, please release them first\n", __func__);
        return;
    }
    IFX_USIF_SPI_SEM_UNLOCK(port->dev_sem);
     
    ifx_usif_int_wrapper.free(port->usif_fake_irq, port);
#ifdef CONFIG_IFX_USIF_SPI_ASYNCHRONOUS
    tasklet_kill(&port->usif_txrxq);
#else
    kill_proc(port->usif_pid, SIGKILL, 1);
    wait_for_completion(&port->thread_done);
#endif /* CONFIG_IFX_USIF_SPI_ASYNCHRONOUS */
    IFX_USIF_SPI_IRQ_LOCK_DESTROY(port);
    IFX_USIF_SPI_Q_LOCK_DESTROY(port);
    dma_dev = port->dma_dev;
    if (dma_dev != NULL) {
        dma_device_unregister(dma_dev);
        dma_device_release(dma_dev);
    }
#ifdef CONFIG_SYSCTL
    ifx_usif_spi_port_sysctl_detach(port);
#endif /* CONFIG_SYSCTL */

#ifdef CONFIG_IFX_PMCU
    ifx_usif_spi_pmcu_exit(port);
#endif /* CONFIG_IFX_PMCU */
    kfree(port->dma_orig_rxbuf);
    kfree(port->dma_orig_txbuf);
    ifx_usif_spi_proc_delete();
    iounmap(port->membase);
    ifx_usif_spi_pm_disable();
    ifx_usif_spi_gpio_release();
}

module_init(ifx_usif_spi_init);
module_exit(ifx_usif_spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Chuanhua, Chuanhua.Lei@infineon.com");
MODULE_DESCRIPTION("IFX USIF SPI driver for VRX200 reference board");
MODULE_SUPPORTED_DEVICE ("VRX200 builtin USIF-SPI module");

