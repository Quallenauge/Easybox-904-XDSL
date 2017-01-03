/******************************************************************************
**
** FILE NAME    : ifxmips_usif_spi.h
** PROJECT      : IFX UEIP for VRX200
** MODULES      : USIF for SPI
**
** DATE         : 21 Aug 2008
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : USIF for SPI Master/Slave
** COPYRIGHT    :       Copyright (c) 2008 ~ 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
** HISTORY
** $Date         $Author         $Comment
** 21 Aug, 2008  Lei Chuanhua    Initial version
** 20 Jun, 2009  Lei Chuanhua    Code review modification
*******************************************************************************/

#ifndef IFXMIPS_USIF_SPI_H
#define IFXMIPS_USIF_SPI_H

/*!
 \file ifxmips_usif_spi.h
 \ingroup IFX_USIF_SPI    
 \brief header file for ifx usif spi driver internal definition
*/

#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/completion.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif
#include <asm/ifx/ifx_usif_spi.h>
#include "ifxmips_usif_reg.h"

#define IFX_USIF_SPI_WAKELIST_INIT(queue) \
    init_waitqueue_head(&queue)

#define IFX_USIF_SPI_WAIT_EVENT_TIMEOUT(info, event, timeout)    \
    do {                                                         \
        wait_event_interruptible_timeout((info)->dma_wait,       \
            test_bit((event), &(info)->event_flags), (timeout)); \
        clear_bit((event), &(info)->event_flags);                \
    }while (0)

/* Wait queue has no address symbol in this macro 
 * interruptible_sleep_on() possibly cause lost-wakeup issue
 * wait_event_interruptible is robust for multiple events
 */
#define IFX_USIF_SPI_WAIT_EVENT(queue, event, flags)  \
    do {                                              \
        wait_event_interruptible((queue),            \
            test_bit((event), &(flags)));            \
        clear_bit((event), &(flags));                \
    }while (0)

#define IFX_USIF_SPI_WAKEUP_EVENT(queue, event, flags) \
    do {                                               \
        set_bit((event), &(flags));                    \
        wake_up_interruptible(&(queue));               \
    }while (0)


#define IFX_USIF_SPI_IRQ_LOCK_INIT(port, _name)  spin_lock_init(&((port)->usif_irq_lock))

#define IFX_USIF_SPI_IRQ_LOCK_DESTROY(port)
#define IFX_USIF_SPI_IRQ_LOCK(port) do {            \
    unsigned long __ilockflags;                     \
    spin_lock_irqsave(&((port)->usif_irq_lock), __ilockflags);
#define IFX_USIF_SPI_IRQ_UNLOCK(port)               \
    spin_unlock_irqrestore(&((port)->usif_irq_lock), __ilockflags);               \
} while (0)


#define IFX_USIF_SPI_SEM_INIT(sem)   \
    init_MUTEX(&(sem))

#define IFX_USIF_SPI_SEM_LOCK(sem)   \
    down(&(sem))
        
#define IFX_USIF_SPI_SEM_UNLOCK(sem) \
    up(&(sem))

/* Data may run in kernel thread, tasklet at the same time */
#define IFX_USIF_SPI_Q_LOCK_INIT(port)      spin_lock_init(&(port)->q_lock)
#define IFX_USIF_SPI_Q_LOCK_DESTROY(port)
#define IFX_USIF_SPI_Q_LOCK_BH(port)  do {   \
    unsigned long __ilockflags;         \
    spin_lock_irqsave(&((port)->q_lock), __ilockflags);  
#define IFX_USIF_SPI_Q_UNLOCK_BH(port)       \
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

#define IFX_USIF_SPI_MAX_PORT_NUM    1  /* assume default value */

#define IFX_USIF_SPI_MAX_DEVNAME     16
#define IFX_USIF_SPI_MAX_DEVICE      8    /* XXX */

/* Tx/Rx mode, FDF, HDF 2 bits */
#define IFX_USIF_SPI_MODE_OFF         0
#define IFX_USIF_SPI_MODE_RX          0x1
#define IFX_USIF_SPI_MODE_TX          0x2
#define IFX_USIF_SPI_MODE_RXTX        0x3
#define IFX_USIF_SPI_MODE_RXTX_MASK   (IFX_USIF_SPI_MODE_RX | IFX_USIF_SPI_MODE_TX)

/* Heading Control */
#define IFX_USIF_SPI_LSB_FIRST            0
#define IFX_USIF_SPI_MSB_FIRST            1

#define IFX_USIF_SPI_CS_MAX               7 /* Actually it is max cs index */

/* Parameters for USIF_SPI DMA device */
#define DEFAULT_USIF_SPI_TX_CHANNEL_CLASS   0
#define DEFAULT_USIF_SPI_RX_CHANNEL_CLASS   0

#define DEFAULT_USIF_SPI_TX_BURST_LEN       2 /* 2 words, 4 words, 8 words */
#define DEFAULT_USIF_SPI_RX_BURST_LEN       2 /* 2 words, 4 words, 8 words */

#define DEFAULT_USIF_SPI_TX_CHANNEL_NUM      1
#define DEFAULT_USIF_SPI_RX_CHANNEL_NUM      1

#define DEFAULT_USIF_SPI_TX_CHANNEL_DESC_NUM 1  
#define DEFAULT_USIF_SPI_RX_CHANNEL_DESC_NUM 1

/* Default parameter */
#define IFX_USIF_SPI_DEF_SYNC           1  /* Synchronous */
#define IFX_USIF_SPI_DEF_MASTERSLAVE    1  /* Master */
#define IFX_USIF_SPI_DEF_FRMCTL         0  /* Disable framing control */
#define IFX_USIF_SPI_DEF_LOOP_BACK      0  /* Disable loopback */
#define IFX_USIF_SPI_DEF_ECHO_MODE      0  /* Disable echo mode */
#define IFX_USIF_SPI_DEF_TX_INVERT      0  /* Don't invert */
#define IFX_USIF_SPI_DEF_RX_INVERT      0  /* Don't invert */
#define IFX_USIF_SPI_DEF_SHIFT_CLK      0  /* No shift clock */
#define IFX_USIF_SPI_DEF_CLK_PO         0  /* Idle Low */
#define IFX_USIF_SPI_DEF_CLK_PH         1  /* 0: shift on leading edge, latch on trailling edge, 1, otherwise*/
#define IFX_USIF_SPI_DEF_TX_IDLE_STATE  0  /* Low */
#define IFX_USIF_SPI_DEF_MODE_RXTX      IFX_USIF_SPI_MODE_RXTX /* TX/RX modes */
#define IFX_USIF_SPI_DEF_CHAR_LEN       IFX_USIF_PRTC_CFG_CLEN8
#define IFX_USIF_SPI_DEF_HDR_CTRL       IFX_USIF_SPI_MSB_FIRST
#define IFX_USIF_SPI_DEF_WAKEUP         0

#ifdef CONFIG_USE_EMULATOR
#define IFX_USIF_SPI_DEF_BAUDRATE        10000
#else
#define IFX_USIF_SPI_DEF_BAUDRATE        2000000
#endif
#define IFX_USIF_SPI_DEF_RMC             0x01 /* To use it up to max value */
#define IFX_USIF_SPI_DEF_ORMC            0x01

/*!  
 \addtogroup IFX_USIF_SPI_DEFINITIONS
*/
/* @{ */

/*! \def DEFAULT_USIF_SPI_FRAGMENT_SIZE 
    \brief a maximum fragment size parameter is defined for all connections. 
    
    This threshold is given in number of bytes. It has to be configured for each connection 
    and be less or equal. If the size of the packet is greater than this threshold the application 
    has to take care of the fragmentation of the packet. The default global value is given with 1056 bytes. 
    This is the recommended value to use, but each connection is free configure this parameter to a lesser 
    value individually
  */
#define DEFAULT_USIF_SPI_FRAGMENT_SIZE    1056

/*! \def IFX_USIF_SPI_MAX_FRAGSIZE 
    \brief maximum fragment size in theory. 
  */
#define IFX_USIF_SPI_MAX_FRAGSIZE             (32 * 1024)

/*! \def IFX_USIF_SPI_MIN_FRAGSIZE 
    \brief Mimimum fragment size. Smaller than this size has no meaning . 
  */
#define IFX_USIF_SPI_MIN_FRAGSIZE             32

/*! \def DEFAULT_USIF_SPI_FIFO_THRESHOULD 
    \brief For enabling the USIF_SPI driver to choose between FIFO and DMA mode in HDX communication 
    a threshold parameter is set for all connections
    
    This threshold is given in number of bytes. If the size of the packet is lesser than this 
    threshold the FIFO mode is used, else the DMA mode. The global default value of 148 bytes is 
    recommended. This value has to be optimized/tuned accoring to application
  */
#define DEFAULT_USIF_SPI_FIFO_THRESHOULD  148

/*! \def IFX_USIF_SPI_FIFO_MAX_THRESHOULD 
    \brief Maximum FIFO/DMA threshould
  */
#define IFX_USIF_SPI_FIFO_MAX_THRESHOULD      600

/*! \def IFX_USIF_SPI_FIFO_MIN_THRESHOULD 
    \brief Mimimum FIFO/DMA threshould. 
    \note  This thresould must be more than DMA burst length 
  */
#define IFX_USIF_SPI_FIFO_MIN_THRESHOULD      32

/**
 * TX FIFO burst size is set to 2 words, update accordingly 
 * when 'IFX_USIF_SPI_TXFIFO_SETUP' changed 
 * NB, Must be the same as DMA burst length
 */
#define IFX_USIF_SPI_TX_BUSRT_SIZE            0x02
#define IFX_USIF_SPI_TX_BUSRT_SIZE_BYTES      0x08  /* 8 bytes */

/**
 * TX FIFO is byte aligned, so one word contains 4 characters, 
 *   update accordingly when 'IFX_USIF_SPI_TXFIFO_SETUP' changed
 */
#define IFX_USIF_SPI_TX_FIFO_CHARS_PER_WORD   0x04

/**
 * RX FIFO burst size is set to 2 words, update accordingly 
 * when 'IFX_USIF_SPI_RXFIFO_SETUP' changed
 * NB, Must be the same as DMA burst length
 */
#define IFX_USIF_SPI_RX_BUSRT_SIZE            0x02
#define IFX_USIF_SPI_RX_BUSRT_SIZE_BYTES      0x08  /* 8 bytes */
/**
 * RX FIFO is byte aligned, so one word contains 4 characters, 
 *   update accordingly when 'IFX_USIF_SPI_RXFIFO_SETUP' changed
 */
#define IFX_USIF_SPI_RX_FIFO_CHARS_PER_WORD   0x04

/**
 * TX FIFO is setup as flow control mode, with byte alignemnet and 
 *  busrt size as 2 words 
 */
#define IFX_USIF_SPI_TXFIFO_SETUP           \
    (/*IFX_USIF_FIFO_CFG_TXFC |*/ SM(IFX_USIF_TXFIFO_ALIGN_BYTE, IFX_USIF_FIFO_CFG_RXFA) \
    | SM(IFX_USIF_TXFIFO_BURST_WORD2, IFX_USIF_FIFO_CFG_TXBS))

/* 
 * RX FIFO is setup as flow control mode, with byte alignemnet and 
 * busrt size as 2 words 
 */
#define IFX_USIF_SPI_RXFIFO_SETUP            \
    (IFX_USIF_FIFO_CFG_RXFC | SM(IFX_USIF_RXFIFO_ALIGN_BYTE, IFX_USIF_FIFO_CFG_RXFA) \
    | SM(IFX_USIF_RXFIFO_BURST_WORD2, IFX_USIF_FIFO_CFG_RXBS))

#define IFX_USIF_SPI_FIFO_SETUP  (IFX_USIF_SPI_TXFIFO_SETUP |  IFX_USIF_SPI_RXFIFO_SETUP)

/*!
  \brief USIF_SPI Statistics.
  Symbolic constants to be used in USIF_SPI routines 
 */
typedef struct ifx_usif_spi_statistics {
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
} IFX_USIF_SPI_STAT_t;

/*!
  \brief USIF_SPI Hardware Options.
 */
struct ifx_usif_hwopts {
    unsigned int sync:1;            /*!< Synchronous or Asynchronous */
    unsigned int master:1;          /*!< Master or slave for sync mode */
    unsigned int frmctl:1;          /*!< Framing control enable or disable */
    unsigned int loopBack:1;        /*!< Loopback mode */
    unsigned int echoMode:1;        /*!< Echo mode */
    unsigned int txInvert:1;        /*!< Transmissin line invert */
    unsigned int rxInvert:1;        /*!< Receive line invert */
    unsigned int shiftClk:1;        /*!< Shift clock force */
    unsigned int clkPO:1;           /*!< Shift clock polarity configuration */
    unsigned int clkPH:1;           /*!< Shift clock phase configuration*/
    unsigned int txIdleValue:1;     /*!< Tx idle state */
    unsigned int modeRxTx:2;        /*!< Rx/tx mode */
    unsigned int charLen:5;         /*!< Character length */
    unsigned int hdrCtrl:1;         /*!< Heading of data bits */
    unsigned int wakeUp:1;          /*!< Wake-up bit detection */
};

struct ifx_usif_device;

/*! typedef IFX_USIF_QUEUE
    \brief queue structure for sync or async API
*/
typedef struct IFX_USIF_QUEUE {
    struct ifx_usif_device *dev;    /*!< Back pointer to SSC client device */
    int               request_lock; /*!< Indicates this queue locked or not */
    atomic_t          isqueued;     /*!< Only one entry is allowed per device 
                                        shared between tasklet and kernel thread
                                    */
    char              *txbuf;       /*!< Transmission buffer snapshot */
    int               txsize;       /*!< Transmission size snaphot */
    char              *rxbuf;       /*!< Reception buffer snapshot */
    int               rxsize;       /*!< Receptin size snapshot */
    int               exchange_bytes; /*!< Transmit/Received bytes for callback function */
    IFX_USIF_SPI_HANDL_TYPE_t       handle_type; /*!< Sync/Async handle type */
    IFX_USIF_SPI_ASYNC_CALLBACK_t   callback;    /*!< Async API callback function */
    TAILQ_ENTRY(IFX_USIF_QUEUE) q_next;      /*!< Used if it works as queue header */
}IFX_USIF_QUEUE_t;


enum {
    IFX_USIF_SPI_DIR_RX = 0,
    IFX_USIF_SPI_DIR_TX,
};

enum {
    IFX_USIF_SPI_DMA_DISABLE = 0,
    IFX_USIF_SPI_DMA_ENABLE,      
};
/*!
  \brief USIF SPI bus device statistics
 */
struct usif_device_stats {
    u32 rxBytes;      /*!< Received bytes */
    u32 txBytes;      /*!< Transmitted bytes */
    u32 dup_qentries; /*!< Duplicated qentry error */
    u32 context_err;  /*!< Context error */
    u32 frag_err;     /*!< Fragment error */
    u32 handler_err;  /*!< Handler error */
    u32 enqueue;      /*!< Enqueue times */
    u32 dequeue;      /*!< Dequeue times */
    /* More ... */
};

/*! typedef ifx_usif_device_t
    \brief USIF SPI device structure defintion
*/
typedef struct ifx_usif_device {
    char                            dev_name[IFX_USIF_SPI_MAX_DEVNAME]; 
    struct ifx_usif_port            *port;     /* Back pointer */
    TAILQ_ENTRY(ifx_usif_device)    dev_entry;
    IFX_USIF_SPI_CONFIGURE_t        conn_id;
    struct usif_device_stats        stats;
#ifdef CONFIG_SYSCTL
    struct ctl_table_header         *usif_sysctl_header;
    struct ctl_table                *usif_sysctls;
#endif  /* CONFIG_SYSCTL */
    IFX_USIF_QUEUE_t                 queue;
    IFX_USIF_SPI_PRIO_t                  dev_prio;

    /* Kernel thread based per device <ssc client> */
    wait_queue_head_t               dev_thread_wait;  
    volatile  long                  event_flags;
#define IFX_USIF_SPI_DEV_THREAD_EVENT            1
}ifx_usif_device_t;

typedef TAILQ_HEAD( ,ifx_usif_device)    usif_devq_t;
typedef TAILQ_HEAD(, IFX_USIF_QUEUE) usif_dataq_t;

/*! typedef ifx_usif_port_t
    \brief USIF SPI logical port definition
*/
typedef struct ifx_usif_port {
    unsigned char __iomem	       *membase;  /* Virtual */
    unsigned long                  mapbase;   /* Physical */
    struct ifx_usif_hwopts         opts;
    IFX_USIF_SPI_STAT_t            stats;
    u32                            usif_debug;   /* debug msg flags */
    u32                            tx_fifo_size_words;
    u32                            tx_fifo_size_bytes;
    u32                            rx_fifo_size_words;
    u32                            rx_fifo_size_bytes;
    /* USIF_SPI port index, actually, it means how many spi ports we have. */
    int                       port_idx;
    usif_devq_t               usif_devq;
    ifx_usif_device_t         *current_dev;  /* Current calling dev in the port */
    int                       usif_ndevs;
    char                      name[IFX_USIF_SPI_MAX_DEVNAME];
    struct semaphore          dev_sem;
    unsigned int prev_baudrate;
    unsigned int baudrate;
    /* Pointers to rx position */
    volatile char *rxbuf_ptr;
    int            rx_len;

    volatile  long         event_flags;
    struct dma_device_info *dma_dev;  /* DMA device structure */
    IFX_USIF_SPI_MODE_t    prev_spi_mode;  
    int                    usif_frag_size;   /* Possible configuration change, void malloc
                                         * big memory in reception
                                         */
    IFX_USIF_QUEUE_t      *lock_qentry; /* XXX, protection? */
    IFX_USIF_QUEUE_t      *serve_qentry;/* Only in its own tasklet or kernel thread */
    usif_dataq_t          usif_asyncq;   /* Async queue header */
    usif_dataq_t          usif_syncq[IFX_USIF_SPI_PRIO_MAX]; /* Sync queue header */
    spinlock_t            q_lock;        /* on three queues */
    int                   usif_thread;
    int                   usif_fake_irq;
    spinlock_t            usif_irq_lock;
    atomic_t              dma_wait_state;
    wait_queue_head_t     usif_thread_wait;
    struct completion     thread_done;    
#define IFX_USIF_SPI_THREAD_EVENT       1
                                         
    pid_t                 usif_pid;
    int                   usif_cs_locked;  /* Done in the same tasklet or thread only */
    struct tasklet_struct usif_txrxq;
    /* HW WAR DMA buffer */
    char                 *dma_txbuf;      /* Aligned buffer */
    char                 *dma_orig_txbuf; /* Original buffer */
    char                 *dma_rxbuf;      /* Aligned buffer */
    char                 *dma_orig_rxbuf; /* Original buffer */
#ifdef CONFIG_SYSCTL
    struct ctl_table_header    *port_sysctl_header;
    struct ctl_table           *port_sysctls;
#endif  /* CONFIG_SYSCTL */
}ifx_usif_port_t;
/* @} */

#ifdef CONFIG_VR9
#include "ifxmips_usif_spi_vr9.h"
#elif defined CONFIG_AR10
#include "ifxmips_usif_spi_ar10.h"
#else
#error "USIF platform not defined!!!!"
#endif 
#endif /* IFXMIPS_USIF_SPI_H */

