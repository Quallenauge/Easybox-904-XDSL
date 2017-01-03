/******************************************************************************
**
** FILE NAME    : ifxmips_ssc.h
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
** 3 Jul, 2009  Lei Chuanhua    First UEIP release
*******************************************************************************/

/*!
 \file ifxmips_ssc.h
 \ingroup IFX_SSC    
 \brief header file for ifx ssc driver internal definition
*/

#ifndef IFXMIPS_SSC_H
#define IFXMIPS_SSC_H
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/completion.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_ssc.h>

#define IFX_SSC_WAKELIST_INIT(queue) \
    init_waitqueue_head(&queue)

#define IFX_SSC_WAIT_EVENT_TIMEOUT(queue, event, flags, timeout) \
    do {                                                         \
        wait_event_interruptible_timeout((queue),                \
            test_bit((event), &(flags)), (timeout));            \
        clear_bit((event), &(flags));                            \
    }while (0)

/* Wait queue has no address symbol in this macro 
 * interruptible_sleep_on() possibly cause lost-wakeup issue
 * wait_event_interruptible is robust for multiple events
 */
#define IFX_SSC_WAIT_EVENT(queue, event, flags)       \
    do {                                              \
        wait_event_interruptible((queue),            \
            test_bit((event), &(flags)));            \
        clear_bit((event), &(flags));                \
    }while (0)

#define IFX_SSC_WAKEUP_EVENT(queue, event, flags)     \
    do {                                              \
        set_bit((event), &(flags));                   \
        wake_up_interruptible(&(queue));              \
    }while (0)

#define IFX_SSC_IRQ_LOCK_INIT(port, _name)        spin_lock_init(&((port)->ssc_irq_lock))                    
#define IFX_SSC_IRQ_LOCK_DESTROY(port)
#define IFX_SSC_IRQ_LOCK(port) do {                 \
    unsigned long __ilockflags;                     \
    spin_lock_irqsave(&((port)->ssc_irq_lock), __ilockflags);
#define IFX_SSC_IRQ_UNLOCK(port)                    \
    spin_unlock_irqrestore(&((port)->ssc_irq_lock), __ilockflags);               \
} while (0)


#define IFX_SSC_SEM_INIT(sem)   \
    init_MUTEX(&(sem))

#define IFX_SSC_SEM_LOCK(sem)   \
    down(&(sem))
        
#define IFX_SSC_SEM_UNLOCK(sem) \
    up(&(sem))

/* Data may run in kernel thread, tasklet at the same time */
#define IFX_SSC_Q_LOCK_INIT(port)      spin_lock_init(&(port)->q_lock)
#define IFX_SSC_Q_LOCK_DESTROY(port)
#define IFX_SSC_Q_LOCK_BH(port)  do {   \
    unsigned long __ilockflags;         \
    spin_lock_irqsave(&((port)->q_lock), __ilockflags);  
#define IFX_SSC_Q_UNLOCK_BH(port)       \
    spin_unlock_irqrestore(&((port)->q_lock), __ilockflags);\
} while (0)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#define IRQF_DISABLED SA_INTERRUPT
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#define CTL_AUTO -2
#else
#define CTL_AUTO CTL_UNNUMBERED
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#define IFX_INIT_CTL_NAME(val)
#define IFX_SET_CTL_NAME(ctl, val)
#else
#define IFX_INIT_CTL_NAME(val) .ctl_name = val,
#define IFX_SET_CTL_NAME(ctl, val) ctl.ctl_name = val
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21)
#define IFX_REGISTER_SYSCTL_TABLE(t) register_sysctl_table(t, 1)
#else
#define IFX_REGISTER_SYSCTL_TABLE(t) register_sysctl_table(t)
#endif

#define IFX_SSC_MAX_PORT_NUM        1   /* assume default value */

#define IFX_SSC_MAX_DEVNAME         16
#define IFX_SSC_MAX_DEVICE          8    /* XXX */

/* Parameters for SSC DMA device */

#define DEFAULT_SSC_TX_CHANNEL_CLASS 3
#define DEFAULT_SSC_RX_CHANNEL_CLASS 0

#define DEFAULT_SSC_TX_BURST_LEN   2 /* 2 words, 4 words, 8 words */
#define DEFAULT_SSC_RX_BURST_LEN   2 /* 2 words, 4 words, 8 words */

#define DEFAULT_SSC_TX_CHANNEL_NUM 1
#define DEFAULT_SSC_RX_CHANNEL_NUM 1

#define DEFAULT_SSC_TX_CHANNEL_DESCR_NUM 1  
#define DEFAULT_SSC_RX_CHANNEL_DESCR_NUM 1
/*!  
 \addtogroup IFX_SSC_DEFINITIONS
*/
/* @{ */

/*! \def DEFAULT_SSC_FRAGMENT_SIZE 
    \brief a maximum fragment size parameter is defined for all connections. 
    
    This threshold is given in number of bytes. It has to be configured for each connection 
    and be less or equal. If the size of the packet is greater than this threshold the application 
    has to take care of the fragmentation of the packet. The default global value is given with 1056 bytes. 
    This is the recommended value to use, but each connection is free configure this parameter to a lesser 
    value individually
  */
#define DEFAULT_SSC_FRAGMENT_SIZE    1056

/*! \def IFX_SSC_MAX_FRAGSIZE 
    \brief maximum fragment size in theory. 
  */
#define IFX_SSC_MAX_FRAGSIZE         (32 * 1024)

/*! \def IFX_SSC_MAX_FRAGSIZE 
    \brief Mimimum fragment size. Smaller than this size has no meaning . 
  */
#define IFX_SSC_MIN_FRAGSIZE         32

/*! \def DEFAULT_SSC_FIFO_THRESHOULD 
    \brief For enabling the SSC driver to choose between FIFO and DMA mode in HDX communication 
    a threshold parameter is set for all connections
    
    This threshold is given in number of bytes. If the size of the packet is lesser than this 
    threshold the FIFO mode is used, else the DMA mode. The global default value of 148 bytes is 
    recommended. This value has to be optimized/tuned accoring to application
  */
#define DEFAULT_SSC_FIFO_THRESHOULD  148

/*! \def IFX_SSC_FIFO_MAX_THRESHOULD 
    \brief Maximum FIFO/DMA threshould
  */
#define IFX_SSC_FIFO_MAX_THRESHOULD  512

/*! \def IFX_SSC_FIFO_MIN_THRESHOULD 
    \brief Mimimum FIFO/DMA threshould. 
    \note  This thresould must be more than DMA burst length 
  */
#define IFX_SSC_FIFO_MIN_THRESHOULD  32 

/*!
  \brief SSC Statistics.
  Symbolic constants to be used in SSC routines 
 */
struct ifx_ssc_statistics {
    u32 abortErr;    /*!< abort error */
    u32 modeErr;     /*!< master/slave mode error */
    u32 txOvErr;     /*!< TX Overflow error */
    u32 txUnErr;     /*!< TX Underrun error */
    u32 rxOvErr;     /*!< RX Overflow error */
    u32 rxUnErr;     /*!< RX Underrun error */
    u32 rxBytes;     /*!< Received bytes */
    u32 txBytes;     /*!< Transmitted bytes */
    u32 txFifo;      /*!< Transmit transactions in FIFO */
    u32 txDma;       /*!< Transmit transactions in DMA */
    u32 txDmaBytes;  /*!< Transmit bytes in DMA mode */
    u32 rxFifo;      /*!< Receive transactions in FIFO */
    u32 rxDma;       /*!< Receive transactions in FIFO */
    u32 rxDmaBytes;  /*!< Receive bytes in DMA mode */
};


/*!
  \brief SSC Hardware Options.
 */
struct ifx_ssc_hwopts {
    unsigned int abortErrDetect:1;  /*!< Abort Error detection (in slave mode) */
    unsigned int rxOvErrDetect:1;   /*!< Receive Overflow Error detection */
    unsigned int rxUndErrDetect:1;  /*!< Receive Underflow Error detection */
    unsigned int txOvErrDetect:1;   /*!< Transmit Overflow Error detection */
    unsigned int txUndErrDetect:1;  /*!< Transmit Underflow Error detection */
    unsigned int echoMode:1;        /*!< Echo mode */
    unsigned int loopBack:1;        /*!< Loopback mode */
    unsigned int idleValue:1;       /*!< Idle value */
    unsigned int clockPolarity:1;   /*!< Idle clock is high or low */
    unsigned int clockPhase:1;      /*!< Tx on trailing or leading edge */
    unsigned int headingControl:1;  /*!< LSB first or MSB first */
    unsigned int dataWidth:6;       /*!< from 2 up to 32 bits */
    unsigned int masterSelect:1;    /*!< Master or Slave mode */
    unsigned int modeRxTx:2;        /*!< rx/tx mode */
    unsigned int gpoCs:8;           /*!< choose outputs to use for chip select */
    unsigned int gpoInv:8;          /*!< invert GPO outputs */
};

#define IFX_SSC_IS_MASTER(p) ((p)->opts.masterSelect == SSC_MASTER_MODE)


struct ssc_device;


/*! typedef IFX_SSC_QUEUE_t
    \brief queue structure for sync or async API
*/
typedef struct IFX_SSC_QUEUE {
    struct ssc_device *dev;         /*!< Back pointer to SSC client device */
    int               request_lock; /*!< Indicates this queue locked or not */
    atomic_t          isqueued;     /*!< Only one entry is allowed per device 
                                        shared between tasklet and kernel thread
                                    */
    char              *txbuf;       /*!< Transmission buffer snapshot */
    int               txsize;       /*!< Transmission size snaphot */
    char              *rxbuf;       /*!< Reception buffer snapshot */
    int               rxsize;       /*!< Receptin size snapshot */
    int               exchange_bytes; /*!< Transmit/Received bytes for callback function */
    IFX_SSC_HANDL_TYPE_t       handle_type; /*!< Sync/Async handle type */
    IFX_SSC_ASYNC_CALLBACK_t   callback;    /*!< Async API callback function */
    TAILQ_ENTRY(IFX_SSC_QUEUE) q_next;      /*!< Used if it works as queue header */
}IFX_SSC_QUEUE_t;
/* @} */
enum {
    IFX_SSC_DIR_RX = 0,
    IFX_SSC_DIR_TX,
};

enum {
    IFX_SSC_DMA_DISABLE = 0,
    IFX_SSC_DMA_ENABLE,      
};

/*!
  \brief SSC bus device statistics
 */
struct ifx_ssc_device_stats {
    u32 rxBytes;      /*!< Received bytes */
    u32 txBytes;      /*!< Transmitted bytes */
    u32 dup_qentries; /*!< Duplicated qentry error */
    u32 context_err;  /*!< Context error */
    u32 frag_err;     /*!< Fragment error */
    u32 handler_err;  /*!< Handler error */
    u32 enqueue;      /*!< enqueue times */
    u32 dequeue;      /*!< dequeue times */
    /* More ... */
};

/*! typedef ssc_device_t
    \brief ssc device structure defintion
*/
typedef struct ssc_device {
    char                            dev_name[IFX_SSC_MAX_DEVNAME]; 
    struct ifx_ssc_port             *port;     /* back pointer */
    TAILQ_ENTRY(ssc_device)         dev_entry;
    IFX_SSC_CONFIGURE_t             conn_id;
    struct ifx_ssc_device_stats     stats;
#ifdef CONFIG_SYSCTL
    struct ctl_table_header         *ssc_sysctl_header;
    struct ctl_table                *ssc_sysctls;
#endif  /* CONFIG_SYSCTL */
    IFX_SSC_QUEUE_t                 queue;
    IFX_SSC_PRIO_t                  dev_prio;

    /* Kernel thread based per device <ssc client> */
    wait_queue_head_t               dev_thread_wait;  
    volatile  long                  event_flags;
#define IFX_SSC_DEV_THREAD_EVENT            1
}ssc_device_t;

typedef TAILQ_HEAD( ,ssc_device) ssc_devq_t;

typedef TAILQ_HEAD(, IFX_SSC_QUEUE) ssc_dataq_t;

/* Structure for transmit word */
typedef union ifx_ssc_txd {
    struct {
        u8 byte[4];
    }txd_byte;
    u32 txd_word;
}ifx_ssc_txd_t;

/* Structure for receive word */
typedef union ifx_ssc_rxd {
    struct {
        unsigned char byte[4];
    }rxd_byte;
    unsigned int rxd_word;
}ifx_ssc_rxd_t;
/*!
  \brief SSC Hardware logical port
 */
struct ifx_ssc_port {
    unsigned char __iomem	  *membase;  /* Virtual */
    unsigned long             mapbase;   /* Physical */
    struct ifx_ssc_hwopts     opts;
    struct ifx_ssc_statistics stats;
    u32          ssc_debug;   /* debug msg flags */
    /* SSC port index, actually, it means how many spi bus we have. */
    int          port_idx;
    ssc_devq_t   ssc_devq;

    /* Device < ssc client> queue only happens in process context */
    struct semaphore dev_sem;
    int          ssc_ndevs;
    char         name[IFX_SSC_MAX_DEVNAME];
    unsigned int prev_ssc_clk;
    unsigned int prev_baudrate;
    unsigned int baudrate;
    /* buffer and pointers to the rx/tx position FIFO */
    char          *rxbuf_end; /* buffer end pointer for RX */
    volatile char *rxbuf_ptr; /* buffer write pointer for RX */
    char          *txbuf_end; /* buffer end pointer for TX */
    volatile char *txbuf_ptr; /* buffer read pointer for TX */

    /* All these counters used to remove FIFO memory copy */
    int            actual_rx_len;     /* Real Rx data length for debugging */
    int            actual_tx_len;     /* Real Tx data length, passed between functions */
    int            txrx_len;          /* Total Tx/Rx length */
    int            rx_start_position; /* Trace where rx data starts */
    int            tx_counter_for_rx; /* Tx counter used for tracing rx real data pos */
    int            tx_end_position;   /* Trace where tx data ends */
    int            tx_counter_for_tx; /* Tx counter used for tracing tx real data ends */


    volatile  long         event_flags;
    struct dma_device_info *dma_dev;  /* DMA device structure */
    IFX_SSC_MODE_t    prev_ssc_mode;  
    int               ssc_fragSize;   /* Possible configuration change, void malloc
                                       * big memory in reception
                                       */

    IFX_SSC_QUEUE_t      *lock_qentry; /* XXX, protection? */
    IFX_SSC_QUEUE_t      *serve_qentry;/* Only in its own tasklet or kernel thread */
    ssc_dataq_t          ssc_asyncq;   /* Async queue header */
    ssc_dataq_t          ssc_syncq[IFX_SSC_PRIO_MAX]; /* Sync queue header */
    spinlock_t           q_lock;        /* on three queues */
    int                  ssc_thread;
    int                  ssc_fake_irq;
    spinlock_t           ssc_irq_lock;
    atomic_t             dma_wait_state;
    wait_queue_head_t    ssc_thread_wait;
    struct completion    thread_done;
#define IFX_SSC_THREAD_EVENT       1

    pid_t                ssc_pid;
    int                  ssc_cs_locked;  /* Done in the same tasklet or thread only */
    struct tasklet_struct ssc_txrxq;
    /* HW WAR DMA buffer */
    char                 *dma_txbuf;      /* Aligned buffer */
    char                 *dma_orig_txbuf; /* Original buffer */
    char                 *dma_rxbuf;      /* Aligned buffer */
    char                 *dma_orig_rxbuf; /* Original buffer */
#ifdef CONFIG_SYSCTL
    struct ctl_table_header    *port_sysctl_header;
    struct ctl_table           *port_sysctls;
#endif  /* CONFIG_SYSCTL */
    u32                  tx_fifo_size;
    u32                  rx_fifo_size;
};


#endif /* IFXMIPS_SSC_H */


