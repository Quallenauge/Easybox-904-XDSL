/**
** FILE NAME    : ifxmips_mmc_wlan.c
** PROJECT      : IFX UEIP
** MODULES      : MMC WLAN module 
** DATE         : 08 July 2009
** AUTHOR       : Ralf Janssen
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
** $Date                $Author                 $Comment
** 01 AUG 2008                Ralf Janssen           Initial release
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/autoconf.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

#include <asm/irq.h>
#include <asm/ifx/ifx_mmc_wlan.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_dma_core.h>
#include "ifxmips_mmc_reg.h"
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

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheops.h>
#include <asm/mipsregs.h>
#include <asm/atomic.h>
#include <asm/bitops.h>

#include <linux/mmc/mmc.h>
#include <linux/delay.h>
/* #include <linux/trace.h> */


MODULE_DESCRIPTION ("IFX sdio controller module");
MODULE_AUTHOR ("Ralf Janssen");
MODULE_LICENSE("GPL");

/*
  Driver version number
 */
#define IFX_SDIO_VERSION "0.0.8"
/* const */ char *ifx_sdio_driver = "IFX SDIO Driver";

#define MCI_TCR (IFX_MMC_BASE_ADDR + 0x0100)

#define IFX_SDIO_USE_DMA 1

#define IFX_SDIO_USE_TIMER

#define IFX_PMU_SDIO_SHIFT                      2
#define IFX_MULTICHUNK_GATHER 1

#define IFX_SDIO_ASSERT(t)      if (!(t)) { printk("ERROR: %s:%s:%d (" # t ")\n", __FILE__, __FUNCTION__, __LINE__); /* BUG_ON(1); */ \
                                            {volatile int wait = 1; while(wait){}}}

/*
  Debug defines and macros
 */
//#define IFX_SDIO_DEBUG
#undef IFX_SDIO_DEBUG
#ifdef IFX_SDIO_DEBUG
#define IFX_SDIO_DMSG(fmt, args...) printk( KERN_INFO  "%s: " fmt,__FUNCTION__, ## args)
#else
#define IFX_SDIO_DMSG(fmt, args...)
#endif
#define IFX_SDIO_EMSG(fmt, args...) printk( KERN_ERR  "%s: " fmt,__FUNCTION__, ## args)
#define IFX_SDIO_FUNC_DMSG(fmt, args...)

#define MAX_SDIO_HOSTS 1

#define IFX_SDIO_PROLOG(_sh, _cfg)                                              \
    ifx_int_sdio_config_t *_cfg;                                                \
                                                                                \
    if (_sh >= MAX_SDIO_HOSTS)                                                  \
        return -ENODEV;                                                         \
                                                                                \
    _cfg = &intcfg[_sh];                                                        \
                                                                                \
    if (_cfg->state != SDIO_IDLE) {                                             \
        IFX_SDIO_EMSG("_cfg->state(%d) != SDIO_IDLE\n", _cfg->state);           \
        return -EBUSY;                                                          \
    }

#define IFX_SDIO_PROLOG_NO_STATE_CHECK(_sh, _cfg)                               \
    ifx_int_sdio_config_t *_cfg;                                                \
                                                                                \
    if (_sh >= MAX_SDIO_HOSTS)                                                  \
        return -ENODEV;                                                         \
                                                                                \
    _cfg = &intcfg[_sh];
 

static uint32_t max_wait_guard_time = 0;
#define WAIT_GUARD_TIME(_cfg)                                                           \
do {                                                                                    \
    uint32_t wgt_start = read_c0_count();                                               \
    uint32_t wgt_time;                                                                  \
    if ((jiffies - (_cfg)->write_delay_jiffies) <= 1) {                                 \
/*         uint32_t write_delay_ticks = (_cfg)->write_delay_ns * 1000 / 7519 + 1;    */       \
/*         while ((read_c0_count() - (_cfg)->write_delay_c0count) < write_delay_ticks) { } */ \
        while ((read_c0_count() - (_cfg)->write_delay_c0count) < (_cfg)->write_delay_ticks) { } \
    }                                                                                   \
    wgt_time = read_c0_count() - wgt_start;                                             \
    if (wgt_time > max_wait_guard_time)                                                 \
        max_wait_guard_time = wgt_time;                                                 \
} while (0)

#define SAVE_DEFAULT_CONFIG(_cfg)                                               \
do {                                                                            \
    (_cfg)->saved_default_config.cmd53_arg_write = (_cfg)->cmd53_arg_write;     \
    (_cfg)->saved_default_config.cmd53_arg_read  = (_cfg)->cmd53_arg_read;      \
    (_cfg)->saved_default_config.block_len_pow   = (_cfg)->block_len_pow;       \
    (_cfg)->saved_default_config.ecfg_block_len  = (_cfg)->ecfg.block_len;      \
    (_cfg)->saved_default_config.ecfg_block_mode = (_cfg)->ecfg.block_mode;     \
} while (0)

#define RESTORE_DEFAULT_CONFIG(_cfg)                                            \
do {                                                                            \
    (_cfg)->cmd53_arg_write = (_cfg)->saved_default_config.cmd53_arg_write;     \
    (_cfg)->cmd53_arg_read  = (_cfg)->saved_default_config.cmd53_arg_read;      \
    (_cfg)->block_len_pow   = (_cfg)->saved_default_config.block_len_pow;       \
    (_cfg)->ecfg.block_len  = (_cfg)->saved_default_config.ecfg_block_len;      \
    (_cfg)->ecfg.block_mode = (_cfg)->saved_default_config.ecfg_block_mode;     \
} while (0)

/*
 * Structure definitions
 */
typedef struct {
    uint32_t len;
    void *data;
} chunk_t;

typedef struct _ifx_platform_config {
        /* interrupt definition */
    int irq0;
    int irq1;
    int dev_irq;                /* SDIO device irq */
    int rst_gpio;               /* GPIO pin used for reset */
        /* gpio for SDIO definition */
    int gpio_cmd;
    int gpio_clk;
    int gpio_data0;
    int gpio_data1;
    int gpio_data2;
    int gpio_data3;
    int (*ifx_sdio_gpio_configure)(struct _ifx_platform_config *platform);
    void (*ifx_sdio_gpio_release)(struct _ifx_platform_config *platform);
    int (*ifx_sdio_gpio_ext_reset)(struct _ifx_platform_config *platform, int active);
    void (*sdio_ack_irq)(struct _ifx_platform_config *platform, int ack_irq0, int ack_irq1);
} ifx_platform_config_t;

typedef struct {
    uint32_t cmd53_arg_write;
    uint32_t cmd53_arg_read;
    uint32_t block_len_pow;
    uint16_t ecfg_block_len;
    uint8_t ecfg_block_mode;
} saved_default_config_t;

typedef enum {
    SDIO_CLOSED = 0,
    SDIO_SHUTDOWN,              /* 1 */
    SDIO_IDLE,                  /* 2 */
    SDIO_CMD,                   /* 3 - generic CMD */
    SDIO_TX_SYNC_CMD,           /* 4 */
    SDIO_TX_SYNC_DATA,          /* 5 */
    SDIO_TX_ASYNC_CMD,          /* 6 */
    SDIO_TX_ASYNC_DATA,         /* 7 */
    SDIO_RX_SYNC_CMD,           /* 8 */
    SDIO_RX_SYNC_DATA,          /* 9 */
    SDIO_RX_ASYNC_CMD,          /* 10 */
    SDIO_RX_ASYNC_DATA          /* 11 */
} sdio_state_t;

typedef struct _dma_config {
    struct dma_device_info *dma_device;
    wait_queue_head_t dma_wait_queue; /* sync operations using DMA use it */
    uint32_t dma_done;
    uint32_t dma_status;
    void *data;                 /* pointer to data buffer provided by app. */
    uint32_t count;             /* number of bytes to transmit/receive */
    uint8_t rx_dma_opened;      /* flag, whether RX DMA has already been opened */
    chunk_t* next_chunk;        /* Next chunk to be transmitted */
    uint32_t start_c0count;     /* timeout value for tx/rx async transmit (No data forwarded to FIFO, no DTO, no RO) */
    uint32_t timeout;           /* timeout in c0count for tx/rx async */
} dma_config_t;

typedef struct _sdio_statistics {
    uint32_t cmd_count;         /* number of commands sent */
    uint32_t tx_sync;           /* number of tx_sync transfers */
    uint32_t tx_sync_bytes;     /* number of bytes transferred via tx sync */
    uint32_t rx_sync;           /* number of rx_sync transfers */
    uint32_t rx_sync_bytes;     /* number of bytes transferred via rx sync */
    uint32_t tx_async;          /* number of tx_async transfers */
    uint32_t tx_async_bytes;    /* number of bytes transferred via tx async */
    uint32_t rx_async;          /* number of rx_async transfers */
    uint32_t rx_async_bytes;    /* number of bytes transferred via rx async */
    uint32_t tx_err_tu;         /* transmitter underrun */
    uint32_t tx_err_dto;        /* data timeout (MCI_STAT.DTO) */
    uint32_t tx_err_dto_sw;     /* data timeout (detected by SW) */
    uint32_t tx_err_timer_exp;  /* timer expired, i.e. TU/DTO w/o error interrupt */
    uint32_t rx_err_ro;         /* receiver overflow error */
    uint32_t rx_err_ro_sw;      /* receiver overflow error (detected by SW) */
    uint32_t rx_err_ro_recovered; /* receiver overflow recovery */
    uint32_t rx_err_timer_exp;  /* timer expired, i.e. RO w/o error interrupt */
    uint32_t err_irq_dma_first; /* DMA irq was raised before SDIO interrupt */
    uint32_t err_irq_sdio_first; /* SDIO interrupt was raised before DMA interrupt */
    uint32_t process_tx_poll_sum; /* how many times in avg. did we call process_tx_data */
    uint32_t process_tx_poll_count;
    uint32_t process_tx_poll_max; /* max. number of calls to process_tx_data */
    uint32_t process_tx_c0_sum;
    uint32_t process_tx_c0_count;
    uint32_t process_tx_c0_max;
} sdio_statistics_t;


/* internal configuration */
typedef struct _ifx_int_sdio_config {
    ifx_sdio_config_t ecfg;   /* configuration, defined by external driver */
    ifx_platform_config_t platform;
    saved_default_config_t saved_default_config;
    sdio_state_t state;
    sdio_statistics_t stat;
    uint32_t index;
    uint32_t mclk;              /* SDIO module clock */
    uint32_t mclk_max;          /* Maximum mclk possible (100 MHz) */
    uint32_t clock;             /* interface clock */
    uint32_t block_len_pow;     /* power to 2 of block length */
    uint32_t cmd52_arg_write;   /* pre-calculated argument for CMD52 write */
    uint32_t cmd52_arg_read;    /* pre-calculated argument for CMD52 reead */
    uint32_t cmd53_arg_write;   /* pre-calculated argument for CMD53 write*/
    uint32_t cmd53_arg_read;    /* pre-calculated argument for CMD53 read */
    uint32_t write_delay_ns;    /* time to transmit 16-Bit with SDIO clock */
    uint32_t write_delay_ticks; /* time to transmit 16-Bit with SDIO clock in c0_clock ticks */
    uint32_t write_delay_jiffies; /* guard time start in jiffies */
    uint32_t write_delay_c0count; /* guard time start in c0_count */
#ifdef CONFIG_PROC_FS
    struct proc_entry_struct *proc_entries;
    struct proc_dir_entry *proc_entry_dir;
#endif
    dma_config_t dma;           /* DMA related settings */
#ifdef IFX_SDIO_USE_TIMER
    struct timer_list timer;    /* used for error recovery w/o err interrupt */
#endif
} ifx_int_sdio_config_t;


/*
 * Variables
 */
ifx_int_sdio_config_t intcfg[MAX_SDIO_HOSTS];

/*
 * Function prototypes
 */
static int ifx_sdio_configure(ifx_int_sdio_config_t *cfg);
static int ifx_sdio_release(ifx_int_sdio_config_t *cfg);
static int ifx_sdio_set_clock(ifx_int_sdio_config_t *cfg, uint32_t clock);
static int ifx_sdio_set_bus_width(ifx_int_sdio_config_t *cfg, uint32_t bus_width);
static int ifx_sdio_ioctl_internal(ifx_int_sdio_config_t *cfg, uint32_t command, uint32_t param);
#ifdef IFX_SDIO_USE_DMA
static int setup_dma_driver (ifx_int_sdio_config_t *cfg);
static int cleanup_dma_driver (ifx_int_sdio_config_t *cfg);
static int dma_tx_channel_reset(struct dma_device_info *dma_dev);
static int dma_rx_channel_reset(ifx_int_sdio_config_t *cfg);
static int sdio_dma_intr_handler (struct dma_device_info *dma_dev, int status);
static u8 *sdio_dma_buffer_alloc (int len, int *byte_offset, void **opt);
static int sdio_dma_buffer_free (uint8_t * dataptr, void *opt);
static int ifx_data_recv (ifx_int_sdio_config_t *cfg);
static int ifx_sdio_dma_tx_complete (ifx_int_sdio_config_t *cfg);
static int _ifx_sdio_poll_async_state(ifx_int_sdio_config_t *cfg);
#endif /* #ifdef IFX_SDIO_USE_DMA */
static int _ifx_sdio_read_async(ifx_int_sdio_config_t *cfg, uint32_t address,
                                void *data, uint32_t count);
static int _ifx_sdio_write_async(ifx_int_sdio_config_t *cfg, uint32_t address,
                                 void *data, uint32_t count);
static int _ifx_sdio_tx_recovery(ifx_int_sdio_config_t *cfg);
static int _ifx_sdio_process_tx_data(ifx_int_sdio_config_t *cfg);
static int _ifx_sdio_process_rx_data(ifx_int_sdio_config_t *cfg);

static int ifx_sdio_fill_platform(ifx_int_sdio_config_t *icfg);

/* imported functions */
int dma_device_write_sdio (struct dma_device_info *dma_dev, u32 *chunkdataptr, void *opt);

/*
  PROC interface settings and function prototypes
 */
#ifdef CONFIG_PROC_FS
static int ifx_sdio_read_proc_general(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_ecfg(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_platform(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_dma(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_statistics(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_all(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_cccr(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_fbr1(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_cis0(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_cis1(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_register(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc_timeout(char *buffer, ifx_int_sdio_config_t *cfg);
static int ifx_sdio_read_proc(char *page, char **start, off_t off,
                              int count, int *eof, void *data);
static int ifx_sdio_write_proc(struct file *file, const char *buffer,
                               unsigned long count, void *data);
static int ifx_sdio_proc_add_interface(ifx_int_sdio_config_t *cfg);
static int ifx_sdio_proc_remove_interface(ifx_int_sdio_config_t *cfg);
static int ifx_sdio_register_procfs(void);
static int ifx_sdio_unregister_procfs(void);

#define IFX_SDIO_PROC_DIRNAME     "ifx_sdio"
static struct proc_dir_entry *ifx_sdio_dir;
static struct proc_dir_entry *entry;

typedef int (*read_proc_fct_t)(char *buffer, ifx_int_sdio_config_t *cfg);
typedef int (*write_proc_fct_t)(char *buffer, int len, ifx_int_sdio_config_t *cfg);

static struct {
    char *name;
    read_proc_fct_t read_proc_fct;
    write_proc_fct_t write_proc_fct;
} read_proc_entries[] = {
    { "general", ifx_sdio_read_proc_general, NULL },
    { "ecfg", ifx_sdio_read_proc_ecfg, NULL },
    { "platform", ifx_sdio_read_proc_platform, NULL },
    { "dma", ifx_sdio_read_proc_dma, NULL },
    { "statistics", ifx_sdio_read_proc_statistics, NULL },
    { "all", ifx_sdio_read_proc_all, NULL },
    { "cccr", ifx_sdio_read_proc_cccr, NULL },
    { "fbr1", ifx_sdio_read_proc_fbr1, NULL },
    { "cis0", ifx_sdio_read_proc_cis0, NULL },
    { "cis1", ifx_sdio_read_proc_cis1, NULL },
    { "register", ifx_sdio_read_proc_register, NULL },
    { "timeout", ifx_sdio_read_proc_timeout, NULL }
};
#define NR_READ_PROC_ENTRIES (sizeof(read_proc_entries)/sizeof(read_proc_entries[0]))

static struct proc_entry_struct {
    ifx_int_sdio_config_t *cfg;
    read_proc_fct_t read_proc_fct;
    write_proc_fct_t write_proc_fct;
    struct proc_dir_entry *proc_entry;
} proc_entries[MAX_SDIO_HOSTS][NR_READ_PROC_ENTRIES];
#endif /* CONFIG_PROC_FS */

/*
 * Local helper functions
 */

/* function to fill the instruction cache with code beginning at "from" and */
/* ending at "to" address. */
/* Transferring data via SDIO at 25 MHz is very time critical. */
/* Every 8*40ns=320ns a new dword has to be read from  the FIFO. */
/* Therefore it is required to avoid TLB/cache misses while receiving data. */
static inline void fill_icache(void *from, void *to)
{
    unsigned long lfrom = (unsigned long)from;
    unsigned long lto   = (unsigned long)to;

    lfrom &= ~15;                /* set address to beginning of cache line */
    lto   &= ~15;

    for (; lfrom <= lto; lfrom += 16)
    {
        __asm__ __volatile__ (
            ".set noreorder\n\t"
            ".set mips3\n\t"
            "cache\t%1, 0(%0)\n\t"
            ".set\tmips0\n\t"
            ".set\treorder\n\t"
            :
            : "r" (lfrom), "i" (Fill));
    }
}

/* Prepare constant parts of cmd52/53 argument */
static inline uint32_t ifx_sdio_prep_cmd52_write(uint8_t function, uint8_t raw)
{
    uint32_t cmd52_write;

    cmd52_write =
        0x80000000 |            /* R/W Write */
        function << 28 |        /* Function number */
        raw << 27;              /* RAW read-after-write */
    
    return cmd52_write;
}

static inline uint32_t ifx_sdio_prep_cmd52_read(uint8_t function)
{
    uint32_t cmd52_read;

    cmd52_read =
        0x00000000 |            /* R/W Read */
        function << 28;         /* Function number */
    
    return cmd52_read;
}

static inline uint32_t ifx_sdio_prep_cmd53_write(uint8_t function, uint8_t block,
                                                 uint8_t opcode)
{
    uint32_t cmd53_write;
    
    cmd53_write =
        0x80000000 |            /* R/W Write */
        function << 28 |        /* Function number */
        block << 27 |           /* byte/block mode */
        opcode << 26;           /* fixed/incrementing address */
    
    return cmd53_write;
}

static inline uint32_t ifx_sdio_prep_cmd53_read(uint8_t function, uint8_t block,
                                                uint8_t opcode)
{
    uint32_t cmd53_read;
    
    cmd53_read =
        0x00000000 |            /* R/W Read */
        function << 28 |        /* Function number */
        block << 27 |           /* byte/block mode */
        opcode << 26;           /* fixed/incrementing address */
    
    return cmd53_read;
}

static void ifx_sdio_prep_cmds(ifx_int_sdio_config_t *icfg)
{
    uint8_t function   = icfg->ecfg.function;
    uint8_t raw        = icfg->ecfg.raw;
    uint8_t block_mode = icfg->ecfg.block_mode;
    uint8_t opcode     = icfg->ecfg.opcode;
    
    icfg->cmd52_arg_write = ifx_sdio_prep_cmd52_write(function, raw);
    icfg->cmd52_arg_read  = ifx_sdio_prep_cmd52_read(function);
    icfg->cmd53_arg_write = ifx_sdio_prep_cmd53_write(function, block_mode, opcode);
    icfg->cmd53_arg_read  = ifx_sdio_prep_cmd53_read(function, block_mode, opcode);
}

/* Calculate cmd52/53 argument */
static inline uint32_t ifx_sdio_calc_cmd52_write(uint32_t pre, uint32_t addr,
                                                 uint8_t data)
{
    uint32_t cmd52_write;

    cmd52_write =
        pre |                   /* Pre-Calculated part of argument */
        addr << 9 |             /* Address to write */
        data;                   /* Data to write */
    
    return cmd52_write;
}

static inline uint32_t ifx_sdio_calc_cmd52_read(uint32_t pre, uint32_t addr)
{
    uint32_t cmd52_read;

    cmd52_read =
        pre |                   /* Pre-Calculated part of argument */
        addr << 9;              /* Address to read */
    
    return cmd52_read;
}

static inline uint32_t ifx_sdio_calc_cmd53_write(uint32_t pre, uint32_t addr,
                                                 uint32_t count)
{
    uint32_t cmd53_write;
    
    cmd53_write =
        pre |                   /* Pre-Calculated part of argument */
        addr << 9 |             /* Address to write */
        count;                  /* byte/block count */
    
    return cmd53_write;
}

static inline uint32_t ifx_sdio_calc_cmd53_read(uint32_t pre, uint32_t addr,
                                                uint32_t count)
{
    uint32_t cmd53_read;
    
    cmd53_read =
        pre |                   /* Pre-Calculated part of argument */
        addr << 9 |             /* Address to write */
        count;                  /* byte/block count */
    
    return cmd53_read;
}

/*
 * Internally used  functions
 */

/* convert MCI_STAT error bits to errno numbers*/
static int mcistat2errno(uint32_t mci_stat)
{
    int ret;
    
    if (mci_stat & (MCI_STAT_DCF | MCI_STAT_SBE)) /* DataCRC or StartByte Error */
        ret = -EILSEQ;
    else if (mci_stat & MCI_STAT_DTO) /* data timeout */
        ret = -ETIMEDOUT;
    else if (mci_stat & (MCI_STAT_RO | MCI_STAT_TU)) /* rx overflow; tx underrun */
        ret = -EIO;
    else            /* data block end is ok */
        ret = 0;

    return ret;
}

/* internally used, no error checking performed */
static int _ifx_sdio_cmd(ifx_int_sdio_config_t *cfg,
                         uint32_t op_code, uint32_t args,
                         uint32_t resp_type, uint32_t *response)
{
    int ret = 0;
    uint32_t sd_cmd = 0;
    uint32_t stat;
    
    MMC_WRITE_REG32(args, MCI_ARG);

    sd_cmd = op_code;

    if (resp_type & MMC_RSP_PRESENT)
        sd_cmd |= MCI_CMD_RSP;

    if (resp_type & MMC_RSP_136)
        sd_cmd |= MCI_CMD_LRSP;

    MMC_WRITE_REG32(0x3ff, MCI_CL);
    
    sd_cmd |= MCI_CMD_EN;
    MMC_WRITE_REG32(sd_cmd, MCI_CMD);

        /* TODO: Use interrupts, for lower speed accesses */
        /* But focus is on WLAN adapter access, with critical timing */
    fill_icache(&&cmd_start, &&cmd_end);
  cmd_start:
    for (;;) {
        stat = MMC_READ_REG32(MCI_STAT);

        if (stat & MCI_STAT_CS) { /* command sent, no response */
            IFX_SDIO_ASSERT(resp_type == MMC_RSP_NONE);
            break;
        } /* if (stat & MCI_STAT_CS) */

        if (stat & MCI_STAT_CRE) { /* command response end */
            IFX_SDIO_ASSERT(resp_type != MMC_RSP_NONE);

            if (resp_type & MMC_RSP_OPCODE) { /* check returned opcode */
                if (op_code != MMC_READ_REG32(MCI_REPCMD)) {
                    ret = -EILSEQ;
                    break;
                }
            } /* if (resp_type & MMC_RSP_OPCODE) */
            
            response[0] = MMC_READ_REG32(MCI_REP0);
            if (resp_type & MMC_RSP_136) {
                response[1] = MMC_READ_REG32(MCI_REP1);
                response[2] = MMC_READ_REG32(MCI_REP2);
                response[3] = MMC_READ_REG32(MCI_REP3);
            } /* if (resp_type & MMC_RSP_136) */
            
            break;
        } /* if (stat & MCI_STAT_CRE) */

        if (stat & MCI_STAT_CTO) { /* command time out */
            ret = -ETIMEDOUT;
            break;
        } /* if (stat & MCI_STAT_CTO) */

        if (stat & MCI_STAT_CCF) { /* command crc fail */
            ret = -EILSEQ;
            break;
        } /* if (stat & MCI_STAT_CCF) */
    } /* for (;;) */

    MMC_WRITE_REG32(0x7ff, MCI_CL);      /* clear all status bits */

    cfg->stat.cmd_count++;

    return ret;
  cmd_end:;
}

/* Read arbitrary number of bytes from device.
   Return, when all data has been transferred. */
/* used internally */
static int _ifx_sdio_read_sync (ifx_int_sdio_config_t *cfg, uint32_t address,
                                void *data, uint32_t count)
{
    int ret = 0;
    uint32_t cmd53_arg;
    uint32_t response[4];
    uint32_t cmd53_count;
    int try = 0;
    uint32_t dctrl = MCI_DCTRL_DIR | MCI_DCTRL_EN; /* card to host; enable */
    uint32_t mci_stat = 0;
    uint32_t mci_fc = 0;
    uint32_t mci_dcnt;
    uint32_t count_dw;
    uint32_t *pData = data;
    unsigned long  flags;
    int n;

    if (cfg->ecfg.block_mode) {
        cmd53_count = count / cfg->ecfg.block_len;
        dctrl |= (cfg->block_len_pow << 4); /* block length */
        
            /* count must be multiple of block length */        
        IFX_SDIO_ASSERT((cmd53_count * cfg->ecfg.block_len) == count);
    }
    else {
        IFX_SDIO_ASSERT(count <= 512);
        IFX_SDIO_ASSERT(count > 0);
        cmd53_count = (count % 512);
        dctrl |= MCI_DCTRL_M;   /* Byte mode */
    }

    cmd53_arg = ifx_sdio_calc_cmd53_read(cfg->cmd53_arg_read, address, cmd53_count);

    local_irq_save(flags);
    fill_icache(&&read_sync_start, &&read_sync_end);
    WAIT_GUARD_TIME(cfg);
    do {
        cfg->state = SDIO_RX_SYNC_CMD;

        MMC_WRITE_REG32(0xfffff, MCI_DTIM);
        MMC_WRITE_REG32(count, MCI_DLGTH);
        MMC_WRITE_REG32(dctrl, MCI_DCTRL);
        
read_sync_start:
        ret = _ifx_sdio_cmd(cfg, 53, cmd53_arg, MMC_RSP_R5, response);
        if (ret) {
            MMC_WRITE_REG32(dctrl & ~(MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL); /* clear byte mode, enable */
            continue;
        }

        cfg->state = SDIO_RX_SYNC_DATA;

        count_dw = (count+3) >> 2; /* number of dwords to read */
        do {
            mci_stat = MMC_READ_REG32(MCI_STAT);
            mci_fc   = MMC_READ_REG32(MCI_FC);
            mci_dcnt = MMC_READ_REG32(MCI_DCNT);

            if (mci_stat & MCI_STAT_RXDA) { /* Receive data available */
                n = count_dw - mci_fc; /* number of dwords to read */
                while (n && count_dw) {
                    *pData++ = MMC_READ_REG32(MCI_DF0);
                    n--;
                    count_dw--;
                }
            }
read_sync_end:
            
            if (mci_stat & (MCI_STAT_DTO /* | MCI_STAT_DBE */ | MCI_STAT_DCF | MCI_STAT_SBE | MCI_STAT_RO))
            {
                printk("%s:%d Error: mci_stat=0x%08x, data=%p, count=%d, count_dw=%d\n",
                       __FUNCTION__, __LINE__, mci_stat, data, count, count_dw);
                if (mci_stat & (MCI_STAT_DCF | MCI_STAT_SBE)) /* DataCRC or StartByte Error */
                    ret = -EILSEQ;
                else if (mci_stat & MCI_STAT_DTO) /* data timeout */
                    ret = -ETIMEDOUT;
                else if (mci_stat & MCI_STAT_RO) /* receiver overflow */
                    ret = -EIO;
/*                 else            /\* data block end is ok *\/ */
/*                     ret = 0; */
            }
        } while((count_dw != 0) && (ret == 0));

        MMC_WRITE_REG32(dctrl & ~(MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL); /* clear byte mode, enable */

    } while ((ret != 0) && (++try < 1 /* 3 */)); /* make at most 3 retries (--RJ-- No retries)*/
    
    MMC_WRITE_REG32(0x7ff, MCI_CL);      /* clear all status bits */
    cfg->state = SDIO_IDLE;

    local_irq_restore(flags);

    cfg->stat.rx_sync++;
    cfg->stat.rx_sync_bytes += count;
    
    return ret;
}

/* Write arbitrary number of bytes to device.
   Return, when all data has been transferred. */

static int _ifx_sdio_write_sync(ifx_int_sdio_config_t *cfg, uint32_t address,
                               void *data, uint32_t count)
{
    int ret = 0;
    uint32_t cmd53_arg;
    uint32_t response[4];
    uint32_t cmd53_count;
    int try = 0;
    uint32_t dctrl = MCI_DCTRL_EN; /* (host to card); enable */
    uint32_t mci_stat = 0;
    uint16_t *pData = data;
    uint16_t *pDataEnd;
    unsigned long  flags;
    uint32_t error_mask;
    

    uint32_t total_count = count;
    chunk_t single_chunk = { count, data }; /* Used, if we only have a single data block */
    chunk_t *chunk = &single_chunk;

#if 0                           /* test multiple chunks per transaction */
    uint32_t test_chunks[1 + 3*2]; /* three chunks */
    {
        if (count > 80) {
            test_chunks[0] = count;
            test_chunks[1] = 16;
            test_chunks[2] = (uint32_t)(data+0);
            test_chunks[3] = 64;
            test_chunks[4] = (uint32_t)(data+16);
            test_chunks[5] = count - 16 - 64;
            test_chunks[6] = (uint32_t)(data + 16 + 64);
            count = 0;
            total_count = 0;
            data = &test_chunks[0];
        }
    }
#endif

    if (total_count == 0) {
            /* transaction consists of multiple chunks */
            /* Format of data: */
            /*   total length of transaction */
            /*   length of first chunk */
            /*   data address of first chunk */
            /*   length, addr of second chunk */
        total_count = *(uint32_t*)data;
        chunk = data + sizeof(uint32_t);
        pData = chunk->data;
    }

    if (cfg->ecfg.block_mode) {
        cmd53_count = total_count / cfg->ecfg.block_len;
        dctrl |= (cfg->block_len_pow << 4); /* block length */
        error_mask = (MCI_STAT_DTO /* | MCI_STAT_DBE */ | MCI_STAT_DCF | MCI_STAT_SBE | MCI_STAT_TU);
        
            /* total_count must be multiple of block length */        
        IFX_SDIO_ASSERT((cmd53_count * cfg->ecfg.block_len) == total_count);
    }
    else {
        IFX_SDIO_ASSERT(total_count <= 512);
        IFX_SDIO_ASSERT(total_count > 0);
        cmd53_count = (total_count % 512);
        dctrl |= MCI_DCTRL_M;   /* Byte mode */
        error_mask = (MCI_STAT_DTO /* | MCI_STAT_DBE */ /* | MCI_STAT_DCF */ | MCI_STAT_SBE | MCI_STAT_TU);
    }

    cmd53_arg = ifx_sdio_calc_cmd53_write(cfg->cmd53_arg_write, address, cmd53_count);

    local_irq_save(flags);
    fill_icache(&&write_sync_start, &&write_sync_end);
    WAIT_GUARD_TIME(cfg);
    do {
        cfg->state = SDIO_TX_SYNC_CMD;

        MMC_WRITE_REG32(0xfffff, MCI_DTIM);
        MMC_WRITE_REG32(total_count, MCI_DLGTH);
/*         MMC_WRITE_REG32(dctrl, MCI_DCTRL); */
        
        ret = _ifx_sdio_cmd(cfg, 53, cmd53_arg, MMC_RSP_R5, response);
        if (ret) {
            MMC_WRITE_REG32(dctrl & ~(MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL); /* clear enable */
            printk("_ifx_sdio_cmd return value is %d\n", ret);
            continue;
        }

        cfg->state = SDIO_TX_SYNC_DATA;

        pData = chunk->data;
        pDataEnd = (uint16_t*)((uint32_t)pData + chunk->len);

        ret = 0;
        count = 0;
#ifdef TRANSMITTER_UNDERRUN_WORKAROUND
        if (total_count == 4) {
                /* copy first up to 64 bytes to FIFO */
            MMC_WRITE_REG32(0x02, MCI_TCR);   /* Set Test mode */
            do {
                register uint32_t val;
                val  = (pData[0] << 16) + (pData[1]);
/*                 printk("    Write %08X to FIFO (1)\n", val); */
                do{ *((volatile u32*)(MCI_DF0)) = (u32)(val); /* mb(); */} while (0);
                pData += 2;     /* 2 16-Bit Words */
                count += 4;
                if (pData >= pDataEnd){
                    chunk++;
                    pData = chunk->data;
                    pDataEnd = (uint16_t*)((uint32_t)pData + chunk->len);
                }
            } while ((count < total_count) && (count < 64));
            MMC_WRITE_REG32(dctrl, MCI_DCTRL);
            MMC_WRITE_REG32(0x00, MCI_TCR);   /* Release Test mode */
        }
#else
            /* cache data (max. 64 bytes) */
        {
            chunk_t *cchunk = chunk;
            uint32_t ccount = 0;
            int c;
            uint32_t vdata;
            do {
                for (c = 0; c < cchunk->len;c++)
                    vdata = ((volatile unsigned char *)(cchunk->data))[c];
                ccount += cchunk->len;
                cchunk ++;
            } while ((ccount < total_count) && (ccount < 64));
        }
        MMC_WRITE_REG32(dctrl, MCI_DCTRL);
#endif /* TRANSMITTER_UNDERRUN_WORKAROUND */
        while((count < total_count) && (ret == 0)) {
            register uint32_t val;
            mci_stat = MMC_READ_REG32(MCI_STAT);
write_sync_start:
                /* Transmit FIFO not full and still data to send */
            while (!(mci_stat & MCI_STAT_TXFF)) {
                if (pData < pDataEnd) {
                    val  = (pData[0] << 16) + (pData[1]);
/*                     printk("    Write %08X to FIFO (2)\n", val); */
                    do{ *((volatile u32*)(MCI_DF0)) = (u32)(val); /* mb(); */} while (0);
/*                     MMC_WRITE_REG32(val, MCI_DF0); */
                    pData += 2;     /* 2 16-Bit Words */
                    count += 4;
                } else if (count < total_count) {
                    chunk++;
                    pData = chunk->data;
                    pDataEnd = (uint16_t*)((uint32_t)pData + chunk->len);
                } else {
                    break;
                }
                mci_stat = MMC_READ_REG32(MCI_STAT);
            }
write_sync_end:
            
            if (mci_stat & error_mask)
            {
                printk("%s:%d Error: mci_stat=0x%08x, data=%p, total_count=%d, count=%d, pData=%p, pDataEnd=%p\n",
                       __FUNCTION__, __LINE__, mci_stat, data, total_count, count, pData, pDataEnd);
                ret = mcistat2errno(mci_stat);
            }
        }

        while (mci_stat & MCI_STAT_TXA) /* wait for transmitter to finish transaction */
            mci_stat = MMC_READ_REG32(MCI_STAT);
        
        MMC_WRITE_REG32(dctrl & ~(MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL); /* clear enable, byte mode */
        MMC_WRITE_REG32(0x7ff, MCI_CL);      /* clear all status bits */

    } while ((ret != 0) && (++try < 1)); /* make at most 3 retries (--RJ-- No Retries)*/

    cfg->state = SDIO_IDLE;
    
    local_irq_restore(flags);
    
        /* byte mode does not send CRC, therefore we have to wait 16 clock cycles */
        /* before the next command is allowed */
    if (!cfg->ecfg.block_mode && !cfg->ecfg.crc) {
        cfg->write_delay_jiffies = jiffies;
        cfg->write_delay_c0count = read_c0_count();
    }

    cfg->stat.tx_sync++;
    cfg->stat.tx_sync_bytes += count;
    
    return ret;
}

static int _ifx_sdio_read_async(ifx_int_sdio_config_t *cfg, uint32_t address,
                                void *data, uint32_t count)
{
    int ret = 0;
    uint32_t cmd53_arg;
    uint32_t response[4];
    uint32_t cmd53_count;
    uint32_t dctrl = MCI_DCTRL_DMA | MCI_DCTRL_DIR | MCI_DCTRL_EN; /* DMA; card to host; enable */
    uint32_t sdio_dmacon;
    int i;

    if (cfg->ecfg.block_mode) {
        cmd53_count = count / cfg->ecfg.block_len;
        dctrl |= (cfg->block_len_pow << 4); /* block length */
        
            /* count must be multiple of block length */        
        IFX_SDIO_ASSERT((cmd53_count * cfg->ecfg.block_len) == count);
    }
    else {
        IFX_SDIO_ASSERT(count <= 512);
        IFX_SDIO_ASSERT(count > 0);
        cmd53_count = (count % 512);
        dctrl |= MCI_DCTRL_M;   /* Byte mode */
    }

    cmd53_arg = ifx_sdio_calc_cmd53_read(cfg->cmd53_arg_read, address, cmd53_count);
    
    sdio_dmacon = MMC_READ_REG32(SDIO_DMACON);

    WAIT_GUARD_TIME(cfg);

    cfg->state = SDIO_RX_ASYNC_CMD;
    cfg->dma.data = data;
    cfg->dma.count = count;

    
    MMC_WRITE_REG32(sdio_dmacon | SDIO_DMACON_RXON, SDIO_DMACON);

        /* provide data buffer to DMA */
        /* TODO: Do proper checking of buffer for burst alignment */
    dma_device_desc_setup(cfg->dma.dma_device, data, count);
    if (!cfg->dma.rx_dma_opened) {
        for (i = 0; i < cfg->dma.dma_device->num_rx_chan; i++) {
            cfg->dma.dma_device->rx_chan[i]->open (cfg->dma.dma_device->rx_chan[i]);
            cfg->dma.dma_device->rx_chan[i]->disable_irq(cfg->dma.dma_device->rx_chan[i]);
        }
        cfg->dma.rx_dma_opened = 1;
    }
        
    MMC_WRITE_REG32(0x7ff, MCI_CL); /* clear all interrupts */
    MMC_WRITE_REG32(0x0fffffff, MCI_DTIM); /* 10s timeout @ 25 MHz */
    MMC_WRITE_REG32(count, MCI_DLGTH);
    MMC_WRITE_REG32(dctrl, MCI_DCTRL);

    ret = _ifx_sdio_cmd(cfg, 53, cmd53_arg, MMC_RSP_R5, response);
    if (ret) {
        MMC_WRITE_REG32(dctrl & ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_DIR | MCI_DCTRL_M), MCI_DCTRL); /* clear enable */
        MMC_WRITE_REG32(sdio_dmacon, SDIO_DMACON);
        printk("_ifx_sdio_cmd return value is %d\n", ret);
        cfg->state = SDIO_IDLE;
            /* TODO: error recovery for DMA (unused buffer assigned to desccr) */
    }
    else {
        cfg->state = SDIO_RX_ASYNC_DATA;
        if (cfg->ecfg.callback_handler != 0) { /* only enable DMA interrupt, if a callback handler is defined */
            cfg->dma.timeout = 1000000;   /* 1000000 * 7.5 ns = 7500 us / TODO: calc timeout properly acc. data rate, buffer length */
            cfg->dma.start_c0count = read_c0_count();;
#ifdef IFX_SDIO_USE_TIMER
            mod_timer(&cfg->timer, jiffies + 2); /* TODO calc timeout value acc. to data rate */
#endif
            cfg->dma.dma_device->rx_chan[0]->enable_irq(cfg->dma.dma_device->rx_chan[0]);
            MMC_WRITE_REG32(MCI_IM_DCF | MCI_IM_DTO | MCI_IM_RO, MCI_IM0);
        }

        ret = -EINPROGRESS;
    }

    cfg->stat.rx_async++;
    cfg->stat.rx_async_bytes += count;

    return ret;
}

static int _ifx_sdio_write_async(ifx_int_sdio_config_t *cfg, uint32_t address,
                                 void *data, uint32_t count)
{
    int ret = 0;
    unsigned long flags;
    uint32_t cmd53_arg;
    uint32_t response[4];
    uint32_t cmd53_count;
    uint32_t dctrl = MCI_DCTRL_DMA | MCI_DCTRL_EN; /* DMA; (host to card); enable */
    uint32_t sdio_dmacon;

    uint32_t total_count = count;
    uint32_t single_chunk[] = { count, count, (uint32_t)data }; /* Used, if we only have a single data block */
    chunk_t *chunk = (chunk_t *)(&single_chunk[1]);
    uint32_t fifo_prefill = 64; /* 64 bytes are prefilled in FIFO, before DMA starts */

    if (total_count == 0) {
            /* transaction consists of multiple chunks */
            /* Format of data: */
            /*   total length of transaction */
            /*   length of first chunk */
            /*   data address of first chunk */
            /*   length, addr of second chunk */
            /*   Must end with a NULL as next data value */
        total_count = *(uint32_t*)data;
        chunk = data + sizeof(uint32_t);
    }

    if (cfg->ecfg.block_mode) {
        cmd53_count = total_count / cfg->ecfg.block_len;
        dctrl |= (cfg->block_len_pow << 4); /* block length */
        
            /* count must be multiple of block length */        
        IFX_SDIO_ASSERT((cmd53_count * cfg->ecfg.block_len) == total_count);
    }
    else {
        IFX_SDIO_ASSERT(total_count <= 512);
        IFX_SDIO_ASSERT(total_count > 0);
        cmd53_count = (total_count % 512);
        dctrl |= MCI_DCTRL_M;   /* Byte mode */
    }

    cmd53_arg = ifx_sdio_calc_cmd53_write(cfg->cmd53_arg_write, address, cmd53_count);
    
    sdio_dmacon = MMC_READ_REG32(SDIO_DMACON);

    MMC_WRITE_REG32(sdio_dmacon | SDIO_DMACON_TXON, SDIO_DMACON);

    WAIT_GUARD_TIME(cfg);       /* TODO: Check, if wait is necessary */

    cfg->state = SDIO_TX_ASYNC_CMD;

    
/*     MMC_WRITE_REG32(0x0fffffff, MCI_DTIM); /\* 10s timeout @ 25 MHz *\/ */
    MMC_WRITE_REG32(total_count * 10, MCI_DTIM);
    MMC_WRITE_REG32(total_count, MCI_DLGTH);
/*     MMC_WRITE_REG32(dctrl, MCI_DCTRL); */
/*     MMC_WRITE_REG32(sdio_dmacon | SDIO_DMACON_TXON, SDIO_DMACON); */

        /* Give all data chunks to DMA, before giving command */
        /* Otherwise the dma_device_write function might take too long */
        /* to add packets to the descriptor list, because of cache flushing */
        /* provide sdio context to dma driver only for last data chunk */
        /* to indicate end of transaction */
/*     dma_device_write(cfg->dma.dma_device, chunk->data + 64, chunk->len - 64, cfg); */

    {
        if (chunk[0].len > fifo_prefill) {
            chunk[-1].data -= fifo_prefill;
            chunk[0].len -= fifo_prefill;
            chunk[0].data += fifo_prefill;
            dma_device_write_sdio(cfg->dma.dma_device, (void*)chunk - sizeof(uint32_t), cfg);
            chunk[-1].data += fifo_prefill;
            chunk[0].len += fifo_prefill;
            chunk[0].data -= fifo_prefill;
        }
        else {
            fifo_prefill = 0;   /* lets try witout pre filling of the FIFO */
            printk("%s: First chunk too small (%d)!", __FUNCTION__, chunk[0].len);
            dma_device_write_sdio(cfg->dma.dma_device, (void*)chunk - sizeof(uint32_t), cfg);
        }
    }
        
    local_irq_save(flags);
    ret = _ifx_sdio_cmd(cfg, 53, cmd53_arg, MMC_RSP_R5, response);
    if (ret) {
        MMC_WRITE_REG32(dctrl & ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL); /* clear enable */
        MMC_WRITE_REG32(sdio_dmacon, SDIO_DMACON);
        printk("_ifx_sdio_cmd return value is %d\n", ret);
        cfg->state = SDIO_IDLE;
        IFX_SDIO_ASSERT(0);     /* TODO: error recovery for DMA */
    }
    else {
        cfg->dma.timeout = 1000000;   /* 1000000 * 7.5 ns = 7500 us / TODO: calc timeout properly acc. data rate, buffer length */
        cfg->dma.start_c0count = read_c0_count();;
#ifdef IFX_SDIO_USE_TIMER
        mod_timer(&cfg->timer, jiffies + 2); /* TODO calc timeout value acc. to data rate */
#endif
        
        if (fifo_prefill) {
            MMC_WRITE_REG32(0x02, MCI_TCR);   /* Set Test mode */

                /* preload the FIFO, before starting SDIO data path */
            memcpy((void*)MCI_DF0, chunk[0].data, fifo_prefill);

            cfg->state = SDIO_TX_ASYNC_DATA;
            MMC_WRITE_REG32(dctrl, MCI_DCTRL);
            MMC_WRITE_REG32(0x00, MCI_TCR);   /* Disable Test mode */
        }
        else {
            cfg->state = SDIO_TX_ASYNC_DATA;
            MMC_WRITE_REG32(dctrl, MCI_DCTRL);
        }
        if (cfg->ecfg.callback_handler != 0) { /* interrupt only makes sense, if a callback handler is registered */
            cfg->dma.dma_device->tx_chan[0]->enable_irq(cfg->dma.dma_device->tx_chan[0]);
                /* unmask SDIO interrupts */
            MMC_WRITE_REG32(MCI_IM_DTO | MCI_IM_TU, MCI_IM0);
        }

        ret = -EINPROGRESS;
    }
    local_irq_restore(flags);

    cfg->stat.tx_async++;
    cfg->stat.tx_async_bytes += total_count;

    return ret;
}

int ifx_sdio_get_state(ifx_int_sdio_config_t *cfg)
{
     return cfg->state;
}

/* recovery for transmitter underrun (TU) and data time out (DTO)*/
static int _ifx_sdio_tx_recovery(ifx_int_sdio_config_t *cfg)
{
    uint32_t mci_stat    = MMC_READ_REG32(MCI_STAT);
    uint32_t mci_dcnt    = MMC_READ_REG32(MCI_DCNT);
    uint32_t mci_dctrl   = MMC_READ_REG32(MCI_DCTRL);
    int dcnt_loop;
    
    if (mci_stat & MCI_STAT_TU)
        cfg->stat.tx_err_tu++;
    else
        cfg->stat.tx_err_dto++;

    dma_tx_channel_reset(cfg->dma.dma_device);

        /* TU recovery for XWAY WAVE100 Family */
        /* Each started multiblock transfer must be completed by the host. */
        /* Im case of a transmitter underrun, the XWAY WAVE will receive the */
        /* current block with trailing 0xff values; which will also cause a */
        /* CRC check failure. */
        /* After that the device expects the remaining blocks to be sent by */
        /* the host, before accepting a new CMD53. */
        /* For this the MCI_DCNT register is read to determine the number of */
        /* outstanding blocks. This number of blocks are transferred by means of */
        /* the data path state machine (DPSM). The length register is set to one */
        /* block before enabling the DPSM. Then a single DWORD is written to the FIFO */
        /* This will cause the DPSM to start the block transmission, which will */
        /* again fail with a Transmitter Underrun, but is a complete block for the device. */
        /* These step are repeated for each missing block. This way the device will get */
        /* the given number of data blocks, which all have an invalid CRC, starting with */
        /* the block, that suffered the first TU.  */
        /* calc number of missing blocks */
    dcnt_loop = mci_dcnt / cfg->ecfg.block_len;
                
        /* Wait some time to be sure, that the current block
           has been received completely by device */
    ndelay(cfg->ecfg.block_len * 80); /* TODO: (fixme) assuming 25 MHz (80ns) */
    udelay(7000);

    printk("TU/DTO (%d): mci_stat=%08X, dcnt_loop=%d, mci_dcnt=0x%04X, tx_async=%d\n",
           __LINE__, mci_stat, dcnt_loop, mci_dcnt, cfg->stat.tx_async);
    if (dcnt_loop > 0) {
        volatile int wait = 0;
        while (wait) { }
    }
                
    while (dcnt_loop--) {
                
            /* set data state machine to idle */
        MMC_WRITE_REG32(mci_dctrl &
               ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
        MMC_WRITE_REG32(MCI_STAT_TU, MCI_CL); /* clear TU indication */

            /* pretend to send one block */
        MMC_WRITE_REG32(cfg->ecfg.block_len, MCI_DLGTH);
        MMC_WRITE_REG32(0x0fffffff, MCI_DTIM); /* 10s timeout @ 25 MHz */
            /* Enable DPSM to start transferring one block */
        ndelay(200); /* Wait 5 SDIO clock cycles TODO: valid for 25 MHz */
        MMC_WRITE_REG32((mci_dctrl & ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M)) |
               (MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
                    
            /* write one dword to FIFO, so that DPSM starts working */
        MMC_WRITE_REG32(0xDEADBEEF, MCI_DF0) ;

            /* Wait some time to be sure, that the current block
               has been received completely by the device */
        ndelay(cfg->ecfg.block_len * 80); /* TODO: (fixme) assuming 25 MHz (80ns) */
        udelay(20); /* TODO: find out correct time to wait */
    } 
/*                 IFX_SDIO_ASSERT(mci_dcnt < cfg->ecfg.block_len); */
    printk("TU/DTO (%d): mci_stat=%08X, dcnt_loop=%d\n", __LINE__, MMC_READ_REG32(MCI_STAT), dcnt_loop);

    return 0;
}

int _ifx_sdio_process_tx_data(ifx_int_sdio_config_t *cfg)
{
    uint32_t mci_dctrl;
    uint32_t mci_stat;
    uint32_t mci_dcnt;
    uint32_t sdio_dmacon;
    int ret;
    
    mci_dctrl = MMC_READ_REG32(MCI_DCTRL);
    mci_stat = MMC_READ_REG32(MCI_STAT);
    mci_dcnt = MMC_READ_REG32(MCI_DCNT);

    ret = mcistat2errno(mci_stat);

    if ((cfg->dma.start_c0count) &&
        ((read_c0_count() - cfg->dma.start_c0count) >= cfg->dma.timeout) &&
        (mci_stat & MCI_STAT_TXA)) {
        printk("%s:%d: no progress mci_stat=%08X, mci_dcnt=0x%04X #######\n",
               __FUNCTION__, __LINE__, mci_stat, mci_dcnt);
        mci_stat |= MCI_STAT_DTO;
        cfg->dma.start_c0count = 0;
        ret = -ETIMEDOUT;
        cfg->stat.tx_err_dto_sw++;
    }
            
    if (mci_stat & (MCI_STAT_TU | MCI_STAT_DTO)) {
        _ifx_sdio_tx_recovery(cfg);
    }

    if (((mci_stat & MCI_STAT_TXA) != 0) && (ret == 0)) { /* still not finished */
        ret = -EINPROGRESS;
    }
    else {
        sdio_dmacon = MMC_READ_REG32(SDIO_DMACON);
    
        MMC_WRITE_REG32(mci_dctrl &
               ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
        MMC_WRITE_REG32(sdio_dmacon & ~SDIO_DMACON_TXON, SDIO_DMACON);
        cfg->state = SDIO_IDLE;
        cfg->dma.start_c0count = 0;
        if (ret) {
                /* TODO: Recover DMA in case of error */
            printk("%s: TX Error mci_stat = %08X, status = %d, mci_dcnt=%d\n",
                   __func__, mci_stat, ret, mci_dcnt);

            if ((ret == -EIO) || /* TU */
                (ret == -ETIMEDOUT)) { /* Data Time Out */
/*                         dma_tx_channel_reset(cfg->dma.dma_device); */
                    /* TODO: Proper error reporting to wlan driver */
                ret = 0; /* Pretend to have transmitted correctly */
            }
        }
        MMC_WRITE_REG32(0x7ff, MCI_CL);      /* clear all status bits */
        cfg->write_delay_jiffies = jiffies;
        cfg->write_delay_c0count = read_c0_count();
    }
    
    return ret;
}

int _ifx_sdio_process_rx_data(ifx_int_sdio_config_t *cfg)
{
    uint32_t mci_stat    = MMC_READ_REG32(MCI_STAT);
    uint32_t sdio_dmacon = MMC_READ_REG32(SDIO_DMACON);
    uint32_t mci_dcnt    = MMC_READ_REG32(MCI_DCNT);
    uint32_t mci_dctrl   = MMC_READ_REG32(MCI_DCTRL);    
    
    int ret = mcistat2errno(mci_stat);
    int data_len = 0;
    unsigned char *data;
    int opt = -1;
    
    if ((cfg->dma.start_c0count) &&
        ((read_c0_count() - cfg->dma.start_c0count) >= cfg->dma.timeout) /* && */
/*         (mci_stat & MCI_STAT_RXA) */) {
        printk("%s:%d: no progress mci_stat=%08X, mci_dcnt=0x%04X #######\n",
               __FUNCTION__, __LINE__, mci_stat, mci_dcnt);
        mci_stat |= MCI_STAT_RO;
        cfg->dma.start_c0count = 0;
        ret = -EIO;
        cfg->stat.rx_err_ro_sw++;
    }
            
    if (ret) {
        if (ret == -EIO) { /* RO */
            cfg->stat.rx_err_ro++;
                /* wait until the read transaction has been terminated */
/*                     do { mci_stat = MMC_READ_REG32(MCI_STAT); } while (mci_stat & MCI_STAT_RXA); */
            ndelay(mci_dcnt * 80); /* TODO: (fixme) assuming 25 MHz SDIO clock */
            mci_dctrl = MMC_READ_REG32(MCI_DCTRL);
            MMC_WRITE_REG32(mci_dctrl &
                   ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
        }
    }
    
    if ((ret == 0) && ((mci_stat & MCI_STAT_RXA) != 0)) { /* still not finished */
        ret = -EINPROGRESS;
    }
    else {
        MMC_WRITE_REG32(0x7ff, MCI_CL);      /* clear all status bits */
        if ((ret == 0) || (ret == -EIO)) {      /* no SDIO error occurred or RO error (ignored) */
            data_len = dma_device_read(cfg->dma.dma_device, &data, (void**)&opt);
/*                     printk("%s: data_len=%d, opt=%d\n", __func__, data_len, opt); */
            if ((data_len <= 0) && (opt == -1) && (ret == 0)) { /* no buffer available yet (data in FIFO ?)*/
                ret = -EINPROGRESS;
            }
            else {
                mci_dctrl = MMC_READ_REG32(MCI_DCTRL);
                sdio_dmacon = MMC_READ_REG32(SDIO_DMACON);
                
                MMC_WRITE_REG32(mci_dctrl &
                       ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
                MMC_WRITE_REG32(sdio_dmacon & ~SDIO_DMACON_RXON, SDIO_DMACON);
                cfg->state = SDIO_IDLE;
                cfg->dma.start_c0count = 0;
/*                 if (data_len > 0) /\* we received data from dma_device_read *\/ */
/*                     ret = 0; */
            }
            if (ret == -EIO) { /* Receiver Overflow */
                MMC_WRITE_REG32(mci_dctrl &
                       ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
                MMC_WRITE_REG32(sdio_dmacon & ~SDIO_DMACON_RXON, SDIO_DMACON);
                
                dma_rx_channel_reset(cfg);
                cfg->platform.sdio_ack_irq(&cfg->platform, 1, 0); /* acknowledge IM0 interrupts */
                        
                cfg->state = SDIO_IDLE;
                cfg->dma.start_c0count = 0;
                
                cfg->stat.rx_err_ro_recovered++;
            }
        }
        else {
                /* TODO: Recover DMA in case of error */
            printk("%s: RX Error mci_stat = %08X, status = %d\n",
                   __func__, mci_stat, ret);

            ndelay(mci_dcnt * 80); /* TODO: (fixme) assuming 25 MHz SDIO clock */
            
            mci_dctrl = MMC_READ_REG32(MCI_DCTRL);
            sdio_dmacon = MMC_READ_REG32(SDIO_DMACON);
            
            MMC_WRITE_REG32(mci_dctrl &
                   ~(MCI_DCTRL_DMA | MCI_DCTRL_EN | MCI_DCTRL_M), MCI_DCTRL);
            MMC_WRITE_REG32(sdio_dmacon & ~SDIO_DMACON_RXON, SDIO_DMACON);
            cfg->state = SDIO_IDLE;
            cfg->dma.start_c0count = 0;
        }
    }
    return ret;
}

int _ifx_sdio_poll_async_state(ifx_int_sdio_config_t *cfg) __attribute__((no_instrument_function));
int _ifx_sdio_poll_async_state(ifx_int_sdio_config_t *cfg)
{
    int ret = -EINPROGRESS;
    int mci_stat = 0;
    uint32_t mci_dcnt = 0;
    
    mci_dcnt = MMC_READ_REG32(MCI_DCNT);
    mci_stat = MMC_READ_REG32(MCI_STAT);
    ret = mcistat2errno(mci_stat);
    
    switch (cfg->state) {
        case SDIO_TX_ASYNC_DATA:
            ret = _ifx_sdio_process_tx_data(cfg);
            break;
        case SDIO_RX_ASYNC_DATA:
            ret = _ifx_sdio_process_rx_data(cfg);
            break;
        case SDIO_IDLE:
            ret=0;
            break;
        default:
            ret = -EIO;
            break;
    }

/*     printk("%s: ret = %d\n", __func__, ret); */
    return ret;
}


/* This interrupt handler is called only in case of errors */
/* and when callback handler is registered */
/* Otherwise SDIO errors are handled in poll_async_state */
irqreturn_t _ifx_sdio_int_handler(int irq, void *dev_instance)
{
    uint32_t mci_stat;
    uint32_t mci_im0;
    int handled = 1;
    ifx_int_sdio_config_t *cfg = (ifx_int_sdio_config_t *)dev_instance;

#if 0
    struct dma_device_info *dma_dev;
    _dma_channel_info *pCh;

    dma_dev = cfg->dma.dma_device;
    pCh = dma_dev->rx_chan[dma_dev->current_rx_chan];

    if (test_and_set_bit(0, &pCh->sync_per_irq)) {
            /* DMA interupt is already running, we do recovery that way */
        cfg->stat.err_irq_dma_first++;
        return IRQ_RETVAL(handled);
    }
    cfg->stat.err_irq_sdio_first++;
#endif /* #if 0 */

#ifdef IFX_SDIO_USE_TIMER
    del_timer(&cfg->timer);
#endif

    mci_stat = MMC_READ_REG32(MCI_STAT);
    
        /* Mask all interrupts */
    MMC_WRITE_REG32(0, MCI_IM0);

/*     printk("SDIO error interrupt: mci_stat=%08X ******************\n", mci_stat); */

    switch (cfg->state)
    {
        case SDIO_TX_ASYNC_DATA:
            ifx_sdio_dma_tx_complete(cfg);
            break;
        case SDIO_RX_ASYNC_DATA:
            ifx_data_recv(cfg);
            break;
        default:
            /* If IM0 has no interrupt enabled, then we probable 
            encountered an error and were called by DMA first, without
            clearing the SDIO interrupt indication in ICU.
            Thus don't print any error message on the console. */
            mci_im0 = MMC_READ_REG32(MCI_IM0);
            if (mci_im0 != 0)   /* Any interrupt enabled ?*/
                IFX_SDIO_EMSG("Unhandled interrupt mci_stat=%08X\n", mci_stat);
            break;
    }

#if 0
    clear_bit(0, &pCh->sync_per_irq);
#endif /* #if 0 */

    return IRQ_RETVAL(handled);    
}

static void _ifx_sdio_timer_fct(unsigned long _cfg)
{
    ifx_int_sdio_config_t *cfg = (ifx_int_sdio_config_t *)_cfg;

    if ((cfg->dma.start_c0count) &&
        ((read_c0_count() - cfg->dma.start_c0count) >= cfg->dma.timeout)) {

        printk("Timer interrupt, start_c0count=%u, c0count=%u (diff %d), timeout=%u\n",
               cfg->dma.start_c0count, read_c0_count(),
               read_c0_count() - cfg->dma.start_c0count, cfg->dma.timeout);
        switch (cfg->state) {
            case SDIO_TX_ASYNC_DATA:
                cfg->stat.tx_err_timer_exp++;
                break;
            case SDIO_RX_ASYNC_DATA:
                cfg->stat.rx_err_timer_exp++;
                break;
            default:
                printk("%s: Timer interrupt without transmitting data\n", __func__);
                return;
        }
        _ifx_sdio_int_handler(0 /* irq_no; not used */, cfg);
    }
    else {
        printk("Timer interrupt, but no timeout start_c0count=%d, c0count=%d (diff=%d), timeout=%d\n",
               cfg->dma.start_c0count, read_c0_count(),
               read_c0_count() - cfg->dma.start_c0count, cfg->dma.timeout);
    }
}


/*
 * Exported functions
 */

/* open SDIO interface and set configuration */
ifx_sdio_handle_t ifx_sdio_open(int dev_index, ifx_sdio_config_t *ecfg)
{
    ifx_int_sdio_config_t *cfg;
    int sh = dev_index;
    int ret, i, ones;
    uint32_t block_len_pow = 0;
    uint32_t sdio_imc;
    
    if (sh >= MAX_SDIO_HOSTS)
        return -ENODEV;

    cfg = &intcfg[sh];

    if (cfg->state != SDIO_CLOSED)
        return -EBUSY;

        /* Do some sanity checks on parameters in ecfg */
    if ((ecfg->clock      > 50000000) ||
        (ecfg->function   > 7)        ||
        (ecfg->raw        > 1)        ||
        (ecfg->block_mode > 1)        ||
        (ecfg->opcode     > 1)        ||
        (ecfg->block_len  > 2048)     ||
        (ecfg->bus_width  > 1)        ||
        (ecfg->crc        > 1)) {
        return -EINVAL;
    }

    if (ecfg->crc != 0) {
        printk("CRC calculation not yet supported !\n");
        ecfg->crc = 0;
    }

        /* ecfg->block_len must be power of 2 */
        /* count number of set bits, has to be one */
    ones = 0;
    for (i = 0; i < sizeof(ecfg->block_len) * 8; i++)
    {
        if (ecfg->block_len & (1 << i)) {
            ones++;
            /* pow(2, i) is block_len, used to program hardware */
            block_len_pow = i;
        }
    }
    if (ones != 1)
        return -EINVAL;

        /* clear old config */
    memset(&intcfg[sh], 0, sizeof(intcfg[0]));

    memcpy(&cfg->ecfg, ecfg, sizeof(cfg->ecfg));
    cfg->block_len_pow = block_len_pow;

        /* Prepare constant parts of cmd52/53 argument */
    ifx_sdio_prep_cmds(cfg);

        /* Fill-in platform specific configuration */
    ret = ifx_sdio_fill_platform(cfg);
    if (ret)
        goto fail;

        /* reserve and configure GPIOs */
    ret = cfg->platform.ifx_sdio_gpio_configure(&cfg->platform);
    if (ret)
        goto fail;

        /* Allocate SDIO resource, initialize interface */
    ret = ifx_sdio_configure(cfg);
    if (ret)
        goto fail_configure;
    
    cfg->state = SDIO_IDLE;
    cfg->index = dev_index;

#if 1
   /**
      \todo check IRQ disabled or shared
   */
    ret = request_irq(cfg->platform.irq0, _ifx_sdio_int_handler, IRQF_DISABLED, "SDIO_IM0", cfg);
#else
    ret = request_irq(cfg->platform.irq0, _ifx_sdio_int_handler, IRQF_SHARED, "SDIO_IM0", cfg);
#endif /* #if 1 */

    if (ret)
        goto fail_request_irq;
    sdio_imc = MMC_READ_REG32(SDIO_IMC);
    sdio_imc |= SDIO_IMC_INTR0;
    MMC_WRITE_REG32(sdio_imc, SDIO_IMC);

#ifdef IFX_SDIO_USE_DMA
        /* initialize DMA */
    ret = setup_dma_driver(cfg);
    if (ret)
        goto fail_setup_dma;
#endif
        /* add proc entry for this instance */
    ifx_sdio_proc_add_interface(cfg);

#ifdef IFX_SDIO_USE_TIMER
        /* initialize timer */
    setup_timer(&cfg->timer, _ifx_sdio_timer_fct, (unsigned long)cfg);
#endif

/*     MOD_INC_USE_COUNT; */
    
    return sh;

   fail_setup_dma:
      free_irq(cfg->platform.irq0, cfg);
  fail_request_irq:
    ifx_sdio_release(cfg);
  fail_configure:
    cfg->platform.ifx_sdio_gpio_release(&cfg->platform);
  fail:
    return ret;
}

/* close SDIO interface and release resources */
int ifx_sdio_close(ifx_sdio_handle_t sh)
{
    uint32_t sdio_imc;
    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */

    cfg->state = SDIO_SHUTDOWN;

        /* remove proc entry for this instance */
    ifx_sdio_proc_remove_interface(cfg);

#ifdef IFX_SDIO_USE_DMA
        /* shudown DMA */
    cleanup_dma_driver(cfg);
#endif

    
    sdio_imc = MMC_READ_REG32(SDIO_IMC);
    sdio_imc &= ~SDIO_IMC_INTR0;
    MMC_WRITE_REG32(sdio_imc, SDIO_IMC);    
    free_irq(cfg->platform.irq0, cfg);
    
        /* Release SDIO resource, shutdown interfacee */
    ifx_sdio_release(cfg);

        /* release GPIOs */
    cfg->platform.ifx_sdio_gpio_release(&cfg->platform);

    cfg->state = SDIO_CLOSED;
    
/*     MOD_DEC_USE_COUNT; */
    
    return 0;
}

/* configure various settings of the driver */
int ifx_sdio_ioctl(ifx_sdio_handle_t sh, uint32_t command, uint32_t param)
{
    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    
    return ifx_sdio_ioctl_internal(cfg, command, param);
}

/* Request SDIO device interrupt line from kernel and enable interrupt in SDIO core */
int ifx_sdio_request_irq(ifx_sdio_handle_t sh, irq_handler_t handler,
                         unsigned long flags, const char *dev_name,
                         void *dev_id)
{
    uint32_t sdio_imc;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    
        /* Only allow request_irq, when the interrupt is disabled */
    sdio_imc = MMC_READ_REG32(SDIO_IMC);
    if (sdio_imc & SDIO_IMC_SDIO)
        return -EBUSY;

    sdio_imc |= SDIO_IMC_SDIO;
    MMC_WRITE_REG32(sdio_imc, SDIO_IMC);

    return request_irq(cfg->platform.dev_irq, handler, flags, dev_name, dev_id);
}

/* Disable interrupt in SDIO core and free interrupt line. */
int ifx_sdio_free_irq (ifx_sdio_handle_t sh, void *dev_id)
{
    uint32_t sdio_imc;

    IFX_SDIO_PROLOG_NO_STATE_CHECK(sh, cfg); /* get cfg structure */
    
    sdio_imc = MMC_READ_REG32(SDIO_IMC);
    
    sdio_imc &= ~SDIO_IMC_SDIO;
    MMC_WRITE_REG32(sdio_imc, SDIO_IMC);

    free_irq(cfg->platform.dev_irq, dev_id);

    return 0;
}

/* This functions enables the SDIO device interrupt. */
int ifx_sdio_enable_irq (ifx_sdio_handle_t sh)
{
    IFX_SDIO_PROLOG_NO_STATE_CHECK(sh, cfg); /* get cfg structure and make sanity checks */

    enable_irq(cfg->platform.dev_irq);

    return 0;
}

/* This function disables the SDIO device interrupt. */
int ifx_sdio_disable_irq (ifx_sdio_handle_t sh)
{
    IFX_SDIO_PROLOG_NO_STATE_CHECK(sh, cfg); /* get cfg structure and make sanity checks */

    disable_irq(cfg->platform.dev_irq);
    
    return 0;
}

/* send a command to the SDIO device and wait for response */
int ifx_sdio_cmd(ifx_sdio_handle_t sh, ifx_sdio_cmd_t *cmd)
{
    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */

    IFX_SDIO_FUNC_DMSG("op_code=%d, args=0x%08x\n", cmd->op_code, cmd->args);

    cfg->state = SDIO_CMD;
    cmd->error = _ifx_sdio_cmd(cfg, cmd->op_code, cmd->args,
                               cmd->response_type, cmd->response);
    cfg->state = SDIO_IDLE;

    return cmd->error;
}

/* send a command to the SDIO device and wait for response,
   provide all parameters directly */
int ifx_sdio_cmd_ext(ifx_sdio_handle_t sh, uint8_t op_code, uint32_t args,
                     uint32_t response_type, uint32_t *response)
{
    int error;
    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */

    IFX_SDIO_FUNC_DMSG("op_code=%d, args=0x%08x\n", op_code, args);

    cfg->state = SDIO_CMD;
    error = _ifx_sdio_cmd(cfg, op_code, args, response_type, response);
    cfg->state = SDIO_IDLE;

    return error;
}


/* write one byte via CMD52 to SDIO device */
int ifx_sdio_write_byte(ifx_sdio_handle_t sh, uint32_t address,
                        uint8_t in, uint8_t *out)
{
    int ret = 0;
    uint32_t response[4];

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, in=0x%02x\n", address, in);

    cfg->state = SDIO_CMD;
    ret = _ifx_sdio_cmd(cfg, 52,
                        cfg->cmd52_arg_write | (address << 9) | in,
                        MMC_RSP_R5,
                        response);
    cfg->state = SDIO_IDLE;

    IFX_SDIO_FUNC_DMSG("out = 0x%02x\n", response[0] & 0xff);

    if (out && (ret == 0)) {
        *out = response[0] & 0xff;
    }

    return ret;
}

/* write one byte via CMD52 to SDIO device, but provide all parameter */
int ifx_sdio_write_byte_ext(ifx_sdio_handle_t sh, uint32_t address,
                            uint8_t function, uint8_t raw,
                            uint8_t in, uint8_t *out)
{
    int ret = 0;
    uint32_t response[4];
    uint32_t cmd52_arg, cmd52_pre;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT(function <= 7);
    IFX_SDIO_ASSERT(raw <= 1);

    cfg->state = SDIO_CMD;

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, function=%d, raw=%d, in=0x%02x\n", address, function, raw, in);

    cmd52_pre = ifx_sdio_prep_cmd52_write(function, raw);
    cmd52_arg = ifx_sdio_calc_cmd52_write(cmd52_pre, address, in);
    
    ret = _ifx_sdio_cmd(cfg, 52, cmd52_arg, MMC_RSP_R5, response);
    cfg->state = SDIO_IDLE;

    IFX_SDIO_FUNC_DMSG("out=0x%02x\n", response[0]);

    if (out && (ret == 0)) {
        *out = response[0] & 0xff;
    }

    return ret;
}

int ifx_sdio_write_sync (ifx_sdio_handle_t sh, uint32_t address,
                        void *data, uint32_t count)
{
    int ret = 0;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT((((uint32_t)data) & 1) == 0); /* need word aligned buffer */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, count=0x%04x\n", address, count);

    ret = _ifx_sdio_write_sync(cfg, address, data, count);
    
    return ret;
}

/* Write arbitrary number of bytes to device, but provide all parameter.
   Return, when all data has been transferred. */
int ifx_sdio_write_sync_ext(ifx_sdio_handle_t sh, uint32_t address,
                            void *data, uint32_t count,
                            uint8_t function, uint8_t block_len_pow, uint8_t opcode)
{
    int ret = -ENODEV;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT((((uint32_t)data) & 1) == 0); /* need word aligned buffer */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, count=0x%04x, function=%d, blp=%d, opcode=%d\n",
                  address, count, function, block_len_pow, opcode);

    if ((function > 7) || (block_len_pow > 11) || (opcode > 1)) {
        return -EINVAL;
    }

    SAVE_DEFAULT_CONFIG(cfg);

    if (block_len_pow > 0) {
        cfg->cmd53_arg_write = ifx_sdio_prep_cmd53_write(function, 1, opcode);
        cfg->block_len_pow  = block_len_pow;
        cfg->ecfg.block_len = 1 << block_len_pow;
        cfg->ecfg.block_mode = 1;
    }
    else {
        cfg->cmd53_arg_write = ifx_sdio_prep_cmd53_write(function, 0, opcode);
        cfg->ecfg.block_mode = 0;
    }

    ret = _ifx_sdio_write_sync(cfg, address, data, count);

    RESTORE_DEFAULT_CONFIG(cfg);

    return ret;
}

extern char *ifx_sdio_driver;

/* Write arbitrary number of bytes to device.
   Return immediately after starting the transfer. */
int ifx_sdio_write_async (ifx_sdio_handle_t sh, uint32_t address,
                          void *data, uint32_t count)
{
    int ret = 0;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT((((uint32_t)data) & 3) == 0); /* need dword aligned buffer */
/*     IFX_SDIO_ASSERT(count >  64); /\* data needs to be bigger than FIFO (because of TU workaround) *\/ */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, count=0x%04x\n", address, count);

    ret = _ifx_sdio_write_async(cfg, address, data, count);
    
    return ret;
}

int ifx_sdio_poll_async_state(ifx_sdio_handle_t sh) __attribute__((no_instrument_function));
int ifx_sdio_poll_async_state(ifx_sdio_handle_t sh)
{
    IFX_SDIO_PROLOG_NO_STATE_CHECK(sh, cfg); /* get cfg structure */

    return _ifx_sdio_poll_async_state(cfg);
}

/* Write arbitrary number of bytes to device., but provide all parameter
   Return immediately after starting the transfer. */
int ifx_sdio_write_async_ext(ifx_sdio_handle_t sh, uint32_t address,
                             void *data, uint32_t count,
                             uint8_t function, uint8_t block_len_pow, uint8_t opcode)
{
    return -ENODEV;
}

/* Read a single byte via CMD52. */
int ifx_sdio_read_byte(ifx_sdio_handle_t sh, uint32_t address, uint8_t *pdata)
{
    int ret = 0;
    uint32_t response[4];

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x\n", address);

    cfg->state = SDIO_CMD;
    ret = _ifx_sdio_cmd(cfg, 52,
                        cfg->cmd52_arg_read | (address << 9),
                        MMC_RSP_R5,
                        response);
    cfg->state = SDIO_IDLE;
    
    IFX_SDIO_FUNC_DMSG("out = 0x%02x\n", response[0] & 0xff);

    if (ret == 0) {
        *pdata = response[0] & 0xff;
    }
    
    return ret;
}

/* Read a single byte via CMD52, but provide all paramter. */
int ifx_sdio_read_byte_ext(ifx_sdio_handle_t sh, uint32_t address,
                           uint8_t function, uint8_t *pdata)
{
    int ret = 0;
    uint32_t response[4];
    uint32_t cmd52_pre, cmd52_arg;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT(function <= 7);

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, function=%d\n", address, function);

    cfg->state = SDIO_CMD;

    cmd52_pre = ifx_sdio_prep_cmd52_read(function);
    cmd52_arg = ifx_sdio_calc_cmd52_read(cmd52_pre, address);
        
    ret = _ifx_sdio_cmd(cfg, 52, cmd52_arg, MMC_RSP_R5,response);
    cfg->state = SDIO_IDLE;

    if (ret == 0) {
        *pdata = response[0] & 0xff;
    }
    
    return ret;
}

/* Read arbitrary number of bytes from device.
   Return, when all data has been transferred. */
int ifx_sdio_read_sync (ifx_sdio_handle_t sh, uint32_t address,
                        void *data, uint32_t count)
{
    int ret = 0;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT(count != 0);
    IFX_SDIO_ASSERT((((uint32_t)data) & 3) == 0); /* need dword aligned buffer */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, count=0x%04x\n", address, count);

    ret = _ifx_sdio_read_sync(cfg, address, data, count);
    
    return ret;
}

/* Read arbitrary number of bytes from device, but provide all parameter.
   Return, when all data has been transferred. */
int ifx_sdio_read_sync_ext (ifx_sdio_handle_t sh, uint32_t address,
                            void *data, uint32_t count,
                            uint8_t function, uint8_t block_len_pow, uint8_t opcode)
{
    int ret = -ENODEV;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT(count != 0);
    IFX_SDIO_ASSERT((((uint32_t)data) & 3) == 0); /* need dword aligned buffer */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, count=0x%04x, function=%d, blp=%d, opcode=%d\n",
                  address, count, function, block_len_pow, opcode);

    if ((function > 7) || (block_len_pow > 11) || (opcode > 1)) {
        return -EINVAL;
    }

    SAVE_DEFAULT_CONFIG(cfg);

    if (block_len_pow > 0) {
        cfg->cmd53_arg_read = ifx_sdio_prep_cmd53_read(function, 1, opcode);
        cfg->block_len_pow  = block_len_pow;
        cfg->ecfg.block_len = 1 << block_len_pow;
        cfg->ecfg.block_mode = 1;
    }
    else {
        cfg->cmd53_arg_read = ifx_sdio_prep_cmd53_read(function, 0, opcode);
        cfg->ecfg.block_mode = 0;
    }

    ret = _ifx_sdio_read_sync(cfg, address, data, count);

    RESTORE_DEFAULT_CONFIG(cfg);

    return ret;
}

/* Read arbitrary number of bytes from device.
   Return immediately after starting the transfer. */
int ifx_sdio_read_async (ifx_sdio_handle_t sh, uint32_t address,
                         void *data, uint32_t count)
{
    int ret = 0;

    IFX_SDIO_PROLOG(sh, cfg); /* get cfg structure and make sanity checks */
    IFX_SDIO_ASSERT(count != 0);
    IFX_SDIO_ASSERT((((uint32_t)data) & 3) == 0); /* need dword aligned buffer */

    IFX_SDIO_FUNC_DMSG("addr=0x%08x, count=0x%04x\n", address, count);

    ret = _ifx_sdio_read_async(cfg, address, data, count);

    return ret;
}


/* Read arbitrary number of bytes from device, but provide all parameter.
   Return immediately after starting the transfer. */
int ifx_sdio_read_async_ext(ifx_sdio_handle_t sh, uint32_t address,
                            void *data, uint32_t count,
                            uint8_t function, uint8_t block_len_pow, uint8_t opcode)
{
    return -ENODEV;
}

/*
  PROC interface related functions
 */

#ifdef CONFIG_PROC_FS
static int ifx_sdio_register_procfs(void)
{
    printk("%s\n\n", __FUNCTION__);

    ifx_sdio_dir = proc_mkdir (IFX_SDIO_PROC_DIRNAME, NULL);

    if (ifx_sdio_dir == NULL)
    {
        printk (KERN_ERR ": can't create /proc/"
                IFX_SDIO_PROC_DIRNAME "\n\n");
        return (-ENOMEM);
    }

    entry = create_proc_entry ("entry",
                               S_IWUSR | S_IRUSR | S_IRGRP |
                               S_IROTH, ifx_sdio_dir);
    if (entry) {
        entry->read_proc = ifx_sdio_read_proc;
        entry->write_proc = ifx_sdio_write_proc;
    }
    else {
        printk (KERN_ERR
                ": can't create /proc/"
                IFX_SDIO_PROC_DIRNAME "/%s\n\n",
                "entry");
        return (-ENOMEM);
    }

    return 0;
}

static int ifx_sdio_unregister_procfs(void)
{
    printk("%s\n\n", __FUNCTION__);

    remove_proc_entry ("entry", ifx_sdio_dir);

    remove_proc_entry (IFX_SDIO_PROC_DIRNAME, &proc_root);

    return 0;
}

static int ifx_sdio_read_proc_general(char *buffer, ifx_int_sdio_config_t *cfg)
{
    return sprintf(buffer,
                   "general\n"
                   "\tindex = %d\n"
                   "\tstate = %d\n"
                   "\tmclk  = %d\n"
                   "\tmclk_max  = %d\n"
                   "\tclock = %d\n"
                   "\tblock_len_pow   = %d\n"
                   "\tcmd52_arg_write = 0x%08x\n"
                   "\tcmd52_arg_read  = 0x%08x\n"
                   "\tcmd53_arg_write = 0x%08x\n"
                   "\tcmd53_arg_read  = 0x%08x\n"
                   "\twrite_delay_ns  = %d\n"
                   "\twrite_delay_ticks   = %d\n"
                   "\twrite_delay_jiffies = %d\n"
                   "\twrite_delay_c0count = %d\n",
                   cfg->index, cfg->state, cfg->mclk, cfg->mclk_max,
                   cfg->clock, cfg->block_len_pow,
                   cfg->cmd52_arg_write, cfg->cmd52_arg_read,
                   cfg->cmd53_arg_write, cfg->cmd53_arg_read,
                   cfg->write_delay_ns, cfg->write_delay_ticks,
                   cfg->write_delay_jiffies,
                   cfg->write_delay_c0count);
}

static int ifx_sdio_read_proc_ecfg(char *buffer, ifx_int_sdio_config_t *cfg)
{
    ifx_sdio_config_t *ecfg = &cfg->ecfg;
    return sprintf(buffer,
                   "ecfg\n"
                   "\tcallback   = %p\n"
                   "\tcontext    = %p\n"
                   "\tclock      = %d\n"
                   "\tfunction   = %d\n"
                   "\traw        = %d\n"
                   "\tblock_mode = %d\n"
                   "\topcode     = %d\n"
                   "\tblock_len  = %d\n"
                   "\tcrc        = %d\n"
                   "\tirq_oob    = %d\n"
                   "\trst_gpio   = %d\n",
                   ecfg->callback_handler,
                   ecfg->context, ecfg->clock, ecfg->function, ecfg->raw,
                   ecfg->block_mode, ecfg->opcode, ecfg->block_len, ecfg->crc,
                   ecfg->irq_oob, ecfg->rst_gpio);
}

static int ifx_sdio_read_proc_platform(char *buffer, ifx_int_sdio_config_t *cfg)
{
    ifx_platform_config_t *platform = &cfg->platform;
    return sprintf(buffer,
                   "platform\n"
                   "\tirq0       = %d\n"
                   "\tirq1       = %d\n"
                   "\tdev_irq    = %d\n"
                   "\trst_gpio   = %d\n"
                   "\tgpio_cmd   = %d\n"
                   "\tgpio_clk   = %d\n"
                   "\tgpio_data0 = %d\n"
                   "\tgpio_data1 = %d\n"
                   "\tgpio_data2 = %d\n"
                   "\tgpio_data3 = %d\n",
                   platform->irq0, platform->irq1, platform->dev_irq,
                   platform->rst_gpio,
                   platform->gpio_cmd, platform->gpio_clk,
                   platform->gpio_data0, platform->gpio_data1,
                   platform->gpio_data2, platform->gpio_data3);
}

static int ifx_sdio_read_proc_dma(char *buffer, ifx_int_sdio_config_t *cfg)
{
    return sprintf(buffer,
                   "dma\n"
                   "\tdma_device    = %p\n"
                   "\tdma_done      = %d\n"
                   "\tdma_status    = %d\n"
                   "\tdata          = %p\n"
                   "\tcount         = %d\n"
                   "\trx_dma_opened = %d\n"
                   "\tstart_c0count = %d\n"
                   "\ttimeout       = %d\n",
                   cfg->dma.dma_device,
                   cfg->dma.dma_done, cfg->dma.dma_status,
                   cfg->dma.data, cfg->dma.count,
                   cfg->dma.rx_dma_opened,
                   cfg->dma.start_c0count, cfg->dma.timeout);
}

static int ifx_sdio_read_proc_statistics(char *buffer, ifx_int_sdio_config_t *cfg)
{
    int len;
    len = sprintf(buffer,
                   "statistics\n"
                   "\tcmd_count     = %d\n"
                   "\ttx_sync       = %d\n"
                   "\ttx_sync_byte  = %d\n"
                   "\trx_sync       = %d\n"
                   "\trx_sync_byte  = %d\n"
                   "\ttx_async      = %d\n"
                   "\ttx_async_byte = %d\n"
                   "\trx_async      = %d\n"
                   "\trx_async_byte = %d\n"
                   "\ttx_err_tu     = %d\n"
                   "\ttx_err_dto    = %d\n"
                   "\ttx_err_dtosw  = %d\n"
                   "\ttx_err_timer_exp = %d\n"
                   "\trx_err_ro        = %d\n"
                   "\trx_err_ro_recovered = %d\n"
                   "\trx_err_timer_exp    = %d\n"
                   "\terr_irq_dma_first   = %d\n"
                   "\terr_irq_sdio_first  = %d\n"
                   "\tmax_wait_guard_time = %d\n"

                   "\tprocess_tx_poll_sum = %d / %d\n"
                   "\tprocess_tx_poll_count= %d\n"
                   "\tprocess_tx_poll_max = %d\n"

                   "\tprocess_tx_c0_sum   = %d / %d\n"
                   "\tprocess_tx_c0_count = %d\n"
                   "\tprocess_tx_c0_max   = %d\n"
                   ,
                   cfg->stat.cmd_count, cfg->stat.tx_sync,
                   cfg->stat.tx_sync_bytes, cfg->stat.rx_sync,
                   cfg->stat.rx_sync_bytes, cfg->stat.tx_async,
                   cfg->stat.tx_async_bytes, cfg->stat.rx_async,
                   cfg->stat.rx_async_bytes, cfg->stat.tx_err_tu,
                   cfg->stat.tx_err_dto, cfg->stat.tx_err_dto_sw,
                   cfg->stat.tx_err_timer_exp,
                   cfg->stat.rx_err_ro, cfg->stat.rx_err_ro_recovered,
                   cfg->stat.rx_err_timer_exp,
                   cfg->stat.err_irq_dma_first, cfg->stat.err_irq_sdio_first,
                   max_wait_guard_time,

                   cfg->stat.process_tx_poll_sum,
                   cfg->stat.process_tx_poll_sum / ((cfg->stat.process_tx_poll_count != 0) ? cfg->stat.process_tx_poll_count : 1),
                   cfg->stat.process_tx_poll_count, cfg->stat.process_tx_poll_max,
                   
                   cfg->stat.process_tx_c0_sum,
                   cfg->stat.process_tx_c0_sum / ((cfg->stat.process_tx_c0_count != 0) ? cfg->stat.process_tx_c0_count : 1),
                   cfg->stat.process_tx_c0_count, cfg->stat.process_tx_c0_max);
#ifdef DEBUG_CH6_DMA_INT
    {
        extern int cis_ch6[16];
        int i;
        for (i = 0; i < 16; i++)
            len += sprintf(buffer + len,
                           "cis_ch6[%d]=%d\n", i, cis_ch6[i]);
        len += sprintf(buffer + len, "cis_ch6[EOP] = %d\n",
                       cis_ch6[2] + cis_ch6[3] + cis_ch6[6] + cis_ch6[7] + 
                               cis_ch6[10] + cis_ch6[11] + cis_ch6[14] + cis_ch6[15]);
        len += sprintf(buffer + len, "cis_ch6[DUR] = %d\n",
                       cis_ch6[4] + cis_ch6[5] + cis_ch6[6] + cis_ch6[7] + 
                               cis_ch6[12] + cis_ch6[13] + cis_ch6[14] + cis_ch6[15]);
        len += sprintf(buffer + len, "cis_ch6[CPT] = %d\n",
                       cis_ch6[8] + cis_ch6[9] + cis_ch6[10] + cis_ch6[11] + 
                               cis_ch6[12] + cis_ch6[13] + cis_ch6[14] + cis_ch6[15]);
    }
#endif /* DEBUG_CH6_DMA_INT */
    return len;
}
                       
static int ifx_sdio_read_proc_all(char *buffer, ifx_int_sdio_config_t *cfg)
{
    int len = 0;

    len += sprintf(buffer + len, "IFX SDIO index %d\n", cfg->index);
    len += ifx_sdio_read_proc_general(buffer + len, cfg);
    len += ifx_sdio_read_proc_ecfg(buffer + len, cfg);
    len += ifx_sdio_read_proc_platform(buffer + len, cfg);
    len += ifx_sdio_read_proc_dma(buffer + len, cfg);
    len += ifx_sdio_read_proc_statistics(buffer + len, cfg);

    return len;
}

static int ifx_sdio_read_proc_dump8(char *buffer, ifx_int_sdio_config_t *cfg, int fct, int addr, int length)
{
    int len = 0, ret = 0;
    int i;
    uint8_t val;

    for (i = 0; i < length; i++)
    {
        if ((i % 16) == 0)
            len += sprintf(buffer + len, "%04x: ", addr + i);
        ret = ifx_sdio_read_byte_ext(cfg->index, addr + i, fct, &val);
        if (ret)
            return ret;
        len += sprintf(buffer + len, "%02X ", val);
        if ((i % 16) == 15)
            len += sprintf(buffer + len, "\n");
    }
    if ((i % 16) != 15)
        len += sprintf(buffer + len, "\n");

    return len;
}

static int ifx_sdio_read_proc_cccr(char *buffer, ifx_int_sdio_config_t *cfg)
{
    int ret;

    ret = ifx_sdio_read_proc_dump8(buffer, cfg, 0 /*fct*/, 0 /*addr*/, 256 /*len*/);
    if (ret < 0)
        ret = sprintf(buffer, "Error reading CCCR: errno=%d\n", ret);
    
    return ret;
}

static int ifx_sdio_read_proc_fbr1(char *buffer, ifx_int_sdio_config_t *cfg)
{
    int ret;
    int fbr = 1;

    ret = ifx_sdio_read_proc_dump8(buffer, cfg, 0 /*fct*/, 0x100 * fbr /*addr*/, 256 /*len*/);
    if (ret < 0)
        ret = sprintf(buffer, "Error reading FBR%d: errno=%d\n", fbr, ret);
    
    return ret;
}

static int ifx_sdio_read_proc_dump_cis_tuple(char *buffer, ifx_int_sdio_config_t *cfg, int tuple_addr, int *tpl_length)
{
    int len = 0, ret = 0;
    uint8_t tpl_code, tpl_link, tpl_data;
    int i;

    *tpl_length = 0;
    
    len += sprintf(buffer + len, "Tuple address %06X\n", tuple_addr);
    if (tuple_addr == 0)
        return len;
    
    ret = ifx_sdio_read_byte_ext(cfg->index, tuple_addr, 0, &tpl_code);
    if (ret < 0)
        return ret;
    len += sprintf(buffer + len, "  TPL_CODE: %02X\n", tpl_code);

    if (tpl_code == 0xff) {
        *tpl_length = 0;
    } else {
        ret = ifx_sdio_read_byte_ext(cfg->index, tuple_addr + 1, 0, &tpl_link);
        if (ret < 0)
            return ret;
        len += sprintf(buffer + len, "  TPL_LINK: %02X\n", tpl_link);
        
        for (i = 0; i < tpl_link; i++) {
            ret = ifx_sdio_read_byte_ext(cfg->index, tuple_addr + 2 + i, 0, &tpl_data);
            if (ret < 0)
                return ret;
            
            if ((i % 16) == 0)
                len += sprintf(buffer + len, "  TPL_DATA: ");
            len += sprintf(buffer + len, "%02X ", tpl_data);
            if ((i % 16) == 15)
                len += sprintf(buffer + len, "\n");
        }
        if ((i % 16) != 15)
            len += sprintf(buffer + len, "\n");
        *tpl_length = tpl_link + 2;
    }

    return len;
    
}

static int ifx_sdio_read_proc_dump_cis(char *buffer, ifx_int_sdio_config_t *cfg, int cis)
{
    int len = 0;
    int cis_addr, cis_addr0, cis_addr1, cis_addr2;
    int ret = 0;
    uint8_t val;
    int tuple_addr = 0, tuple_length = 0;

    ret = ifx_sdio_read_byte_ext(cfg->index, cis * 0x100 + 0x09, 0, &val);
    if (ret < 0)
        return ret;
    cis_addr0 = val;            /* LSB of CIS address */

    ret = ifx_sdio_read_byte_ext(cfg->index, cis * 0x100 + 0x0a, 0, &val);
    if (ret < 0)
        return ret;
    cis_addr1 = val;

    ret = ifx_sdio_read_byte_ext(cfg->index, cis * 0x100 + 0x0b, 0, &val);
    if (ret < 0)
        return ret;
    cis_addr2 = val;            /* MSB of CIS address */

    cis_addr = (cis_addr2 << 16) + (cis_addr1 << 8) + cis_addr0;


    if (cis == 0)               /* Common CIS */
        len += sprintf(buffer + len, "CIS (common)\n");
    else
        len += sprintf(buffer + len, "CIS Function %d\n", cis);

    tuple_addr = cis_addr;
    do {
        ret = ifx_sdio_read_proc_dump_cis_tuple(buffer + len, cfg, tuple_addr,&tuple_length);
        if (ret < 0)
            return ret;
        len += ret;
        
        tuple_addr += tuple_length;
        
    } while (tuple_length != 0);

    return len;
}

static int ifx_sdio_read_proc_cis0(char *buffer, ifx_int_sdio_config_t *cfg)
{
    return ifx_sdio_read_proc_dump_cis(buffer, cfg, 0);
}

static int ifx_sdio_read_proc_cis1(char *buffer, ifx_int_sdio_config_t *cfg)
{
    return ifx_sdio_read_proc_dump_cis(buffer, cfg, 1);
}

static int ifx_sdio_read_proc_register(char *buffer, ifx_int_sdio_config_t *cfg)
{
    int len = 0;
    int addr = 0;

    len += sprintf(buffer + len, "MCI register");
    for (addr = 0xbe102000; addr < 0xbe10204c; addr += 4)
    {
        if ((addr % 16) == 0)
            len += sprintf(buffer + len, "\n%08X: ", addr);
        len += sprintf(buffer + len, "%08X ", *(volatile unsigned int *)addr);
    }

    len += sprintf(buffer + len, "\n\nSDIO register");
    for (addr = 0xbe103000; addr < 0xbe103024; addr += 4)
    {
        if ((addr % 16) == 0)
            len += sprintf(buffer + len, "\n%08X: ", addr);
        len += sprintf(buffer + len, "%08X ", *(volatile unsigned int *)addr);
    }
    len += sprintf(buffer + len, "\n");

    return len;
}

static int ifx_sdio_read_proc_timeout(char *buffer, ifx_int_sdio_config_t *cfg)
{
    int len = 0;

    len += sprintf(buffer + len, "Triggering Timeout function\n");
    _ifx_sdio_timer_fct((unsigned long)cfg);
    return len;
}

static int ifx_sdio_read_proc(char *page, char **start, off_t off,
                       int count, int *eof, void *data)
{
    struct proc_entry_struct *pes = (struct proc_entry_struct *)data;
    int len = 0;

    if (off) {
        *eof = 1;
        return 0;
    }

    if (pes) {
        if (pes->read_proc_fct)
            len += pes->read_proc_fct(page, pes->cfg);
        else
            len += sprintf(page + len, "Read not defined for this proc file!\n");
    }

    *eof = 1;
    return len;
}

static int ifx_sdio_write_proc(struct file *file, const char *buffer,
                        unsigned long count, void *data)
{
    return count;
}

static int ifx_sdio_proc_add_interface(ifx_int_sdio_config_t *cfg)
{
    char *name = "0";
    int ret = 0;
    int i;
    struct proc_dir_entry *entry;
    struct proc_entry_struct *pes;

    name[0] = cfg->index + '0'; /* create name for this interface */

    cfg->proc_entry_dir = proc_mkdir(name, ifx_sdio_dir);

    if (cfg->proc_entry_dir == 0) {
        printk("Can't create /proc/%s/%s\n", IFX_SDIO_PROC_DIRNAME, name);
        return -ENOMEM;
    }

    for (i = 0; i < NR_READ_PROC_ENTRIES; i++) {
        pes = &proc_entries[cfg->index][i];
        entry = create_proc_entry(read_proc_entries[i].name,
                                  S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH,
                                  cfg->proc_entry_dir);
        if (entry) {
            entry->read_proc = ifx_sdio_read_proc;
            entry->write_proc = ifx_sdio_write_proc;
            entry->data = pes;
            
            pes->proc_entry = entry;
            pes->cfg = cfg;;
            pes->read_proc_fct  = read_proc_entries[i].read_proc_fct;
            pes->write_proc_fct = read_proc_entries[i].write_proc_fct;

        }
        else {
            pes->proc_entry = NULL;
            printk("Can't create /proc/%s/%s/%s\n",
                   IFX_SDIO_PROC_DIRNAME, name, read_proc_entries[i].name);
        }
    }
    
    return ret;
}

static int ifx_sdio_proc_remove_interface(ifx_int_sdio_config_t *cfg)
{
    char *name = "0";
    int i;
    struct proc_entry_struct *pes;

    name[0] = cfg->index + '0'; /* create name for this interface */
    pes = proc_entries[cfg->index];

    for (i = 0; i < NR_READ_PROC_ENTRIES; i++) {
        entry = pes[i].proc_entry;
        pes[i].proc_entry = 0;
        if (entry)
            remove_proc_entry(read_proc_entries[i].name, cfg->proc_entry_dir);
    }
    remove_proc_entry(name, ifx_sdio_dir);
    return 0;
}
#endif /* #ifdef CONFIG_PROC_FS */

static int ifx_sdio_init_module (void)
{
    uint32_t sdio_id = 0;

    printk ("IFX SDIO Controller driver version:%s\n", IFX_SDIO_VERSION);
    printk ("Date: %s, Time: %s", __DATE__, __TIME__);

    sdio_id = MMC_READ_REG32(SDIO_ID);
    printk ("\nIFX SDIO Controller HW ID:%08X\n", sdio_id);
    
    if (sdio_id != 0xF041C030) {
        printk("IFX SDIO Controller not found!!\n");
        return -ENODEV;
    }

        /* clear all configurations */
    memset(intcfg, 0, sizeof(intcfg));

#ifdef CONFIG_PROC_FS
    ifx_sdio_register_procfs();
#endif

    return 0;
}

static void ifx_sdio_exit_module (void)
{
    printk("Module ifx_sdio_controller exit\n");
    
#ifdef CONFIG_PROC_FS
    ifx_sdio_unregister_procfs();
#endif //CONFIG_PROC_FS
    
}

static int ifx_sdio_configure(ifx_int_sdio_config_t *cfg)
{
   /* power on SDIO module */
	SDIO_PMU_SETUP(IFX_PMU_ENABLE);
/*
   *(IFX_PMU_PWDCR) &= ~(1 << IFX_PMU_SDIO_SHIFT);
*/
   /* enable module clock */
    MMC_WRITE_REG32 (0x400, SDIO_CLC);   /* 100MHz / 4 = 25MHz  */
/*     MMC_WRITE_REG32 (0x600, SDIO_CLC);   /\* 100MHz / 6 = 16MHz  *\/ */
/*     MMC_WRITE_REG32 (0x300, SDIO_CLC);   /\* 100MHz / 3 = 33MHz  (beyond spec ?)*\/ */
/*     MMC_WRITE_REG32 (0x200, SDIO_CLC);   /\* 100MHz / 2 = 50MHz  (beyond spec ?)*\/ */
    cfg->mclk = 25000000;       /* 25 MHz */
    cfg->mclk_max = 100000000;
    
    /* select SDIO clock (400 kHz for enumeration) */
    ifx_sdio_set_clock(cfg, 0);
    mdelay (1);
    ifx_sdio_set_clock(cfg, 400000);
    
        /* Power On for external device */
    MMC_WRITE_REG32(MCI_PWR_ON, MCI_PWR);
    
        /* Select bus width */
    ifx_sdio_set_bus_width(cfg, cfg->ecfg.bus_width);
    
    MMC_WRITE_REG32(SDIO_CTRL_SDIOEN, SDIO_CTRL);
    
    return 0;
}

static int ifx_sdio_release(ifx_int_sdio_config_t *cfg)
{
        /* Power Off for external device */
    MMC_WRITE_REG32 (MCI_PWR_OFF, MCI_PWR); 

        /* switch off SDIO clock */
    ifx_sdio_ioctl_internal(cfg, IFX_SDIO_IOCTL_SET_CLK, 0);


   /* power down SDIO module */
	SDIO_PMU_SETUP(IFX_PMU_DISABLE);
/*
   *(IFX_PMU_PWDCR) |= (1 << IFX_PMU_SDIO_SHIFT);
*/

    return  0;
}

static int ifx_sdio_set_clock(ifx_int_sdio_config_t *cfg, uint32_t clock)
{
    uint32_t reg = 0;
    uint32_t div;
    uint32_t div_sdio_clc;

    div = MCI_CLK_BYPASS;
    div_sdio_clc = 1;
    cfg->mclk = cfg->mclk_max;

    reg = MMC_READ_REG32(MCI_CLK);
    reg &= ~(MCI_CLK_BYPASS | 0xFF); /* BYPASS, CLK_DIV */
    if (clock == 0) {
        reg &= ~(MCI_CLK_ENABLE);
        MMC_WRITE_REG32(reg, MCI_CLK);
        cfg->clock = 0;
        cfg->write_delay_ns = 0; /* no guard time */
        cfg->write_delay_ticks = 0;
    }
    else {
        if (clock >= cfg->mclk) {
            if (clock > cfg->mclk)
                IFX_SDIO_EMSG("Error! Clock is too high. clock = %d, "
                              "mclk=%d\n", clock, cfg->mclk);
        }
        else {
            div_sdio_clc = (cfg->mclk_max / clock);
            
            if ((cfg->mclk_max / div_sdio_clc) > clock)
                div_sdio_clc++;

            while (div_sdio_clc > 15) { /* SDIO_CLC.RMC has range 0..15 (4 Bits) */
                if (div == MCI_CLK_BYPASS)
                    div = 0;
                else
                    div++;
                div_sdio_clc = (cfg->mclk_max / (2*(div+1))) / clock;
                if (((cfg->mclk_max / div_sdio_clc) / (2*(div+1))) > clock)
                    div_sdio_clc++;
            }
        }
        cfg->mclk = cfg->mclk_max / div_sdio_clc;

        IFX_SDIO_DMSG ("div=%d,sdio_clc.rmc=%d,mclk=%d\n", div, div_sdio_clc, cfg->mclk);
        div_sdio_clc <<= 8;     /* Divider is Bit[11:8] */
        
        reg |= div;
        reg |= MCI_CLK_ENABLE;

        MMC_WRITE_REG32 (div_sdio_clc, SDIO_CLC);
        MMC_WRITE_REG32 (reg, MCI_CLK);
        if (div == MCI_CLK_BYPASS)
            cfg->clock = cfg->mclk;
        else
            cfg->clock = cfg->mclk / ((div + 1) * 2);
        IFX_SDIO_DMSG ("current_speed = %08X\n", cfg->clock);
        IFX_SDIO_DMSG ("\nMCI_CLK=%08X\n", MMC_READ_REG32 (MCI_CLK));

            /* we need to wait 16 (CRC) + 10 (Idle, CRC Status, busy) clock cycles after the SDIO core */
            /* signals write completed (CRC is omitted by core). Save 16 clocks, if CRC is appended by SW */
        if (!cfg->ecfg.crc)
            cfg->write_delay_ns = (16+10) * (1000000000L / cfg->clock + 1);
        else
            cfg->write_delay_ns = (10) * (1000000000L / cfg->clock + 1);
            /* TODO: Calculate delay w.r.t. system clock */
            /* currently a timer tick is 1/133MHz, i.e. 7.519 ns */
        cfg->write_delay_ticks = cfg->write_delay_ns * 1000 / 7519 + 1;   
        IFX_SDIO_DMSG ("cfg->write_delay_ns    = %d\n", cfg->write_delay_ns);
        IFX_SDIO_DMSG ("cfg->write_delay_ticks = %d\n", cfg->write_delay_ticks);
    }

    return 0;
}

static int ifx_sdio_set_bus_width(ifx_int_sdio_config_t *cfg, uint32_t bus_width)
{
    uint32_t mci_clk;

    mci_clk = MMC_READ_REG32(MCI_CLK);
    mci_clk &= ~MCI_WIDE_BUS;

    if (bus_width == IFX_SDIO_IOCTL_BUS_WIDTH_4)
        mci_clk |= MCI_WIDE_BUS;
    MMC_WRITE_REG32(mci_clk, MCI_CLK);
    
    return 0;
}

static int ifx_sdio_set_block_length(ifx_int_sdio_config_t *cfg, uint32_t length)
{
    int ret;
    uint32_t cmd52_pre;
    uint32_t cmd52_arg;
    uint32_t response[4];

    cmd52_pre = ifx_sdio_prep_cmd52_write(0 /* function */, 1 /* raw */);
    cmd52_arg = ifx_sdio_calc_cmd52_write(cmd52_pre, 0x110, (length % 0x100));
    IFX_SDIO_DMSG ("setting block size to 0x%04x (low byte)\n", length);
    ret = _ifx_sdio_cmd(cfg, 52,
                        cmd52_arg,
                        MMC_RSP_R5,
                        response);
    IFX_SDIO_DMSG ("returned value is 0x%02X\n", response[0]);
    if (ret < 0)
        return ret;
    
    cmd52_arg = ifx_sdio_calc_cmd52_write(cmd52_pre, 0x111, (length / 0x100));
    IFX_SDIO_DMSG ("setting block size to 0x%04x (high byte)\n", length);
    ret = _ifx_sdio_cmd(cfg, 52,
                        cmd52_arg,
                        MMC_RSP_R5,
                        response);
    IFX_SDIO_DMSG ("returned value is 0x%02X\n", response[0]);

    return ret;
}

static int ifx_sdio_ioctl_internal(ifx_int_sdio_config_t *cfg, uint32_t command, uint32_t param)
{
    int ret = 0;
    switch (command) {
        case IFX_SDIO_IOCTL_SET_CLK:
            ret = ifx_sdio_set_clock(cfg, param);
            break;
        case IFX_SDIO_IOCTL_SET_BUS_WIDTH:
            ret = ifx_sdio_set_bus_width(cfg, param);
            break;
        case IFX_SDIO_IOCTL_SET_BLOCK_LENGTH:
            ret = ifx_sdio_set_block_length(cfg, param);
            break;
        case IFX_SDIO_IOCTL_SET_FUNCTION:
            break;
        default:
            printk("%s: invalid command (%08x)\n", __func__, command);
            ret = -EINVAL;
    }
    return ret;
}


/*
  DMA related functions
*/
#ifdef IFX_SDIO_USE_DMA
static int setup_dma_driver (ifx_int_sdio_config_t *cfg)
{
    int i = 0;
    int ret;

    init_waitqueue_head(&cfg->dma.dma_wait_queue);
    
    cfg->dma.dma_device = dma_device_reserve ("SDIO");
    
    if (!cfg->dma.dma_device)
      return -EBUSY;
    cfg->dma.dma_device->intr_handler = sdio_dma_intr_handler;
    cfg->dma.dma_device->buffer_alloc = sdio_dma_buffer_alloc;
    cfg->dma.dma_device->buffer_free = sdio_dma_buffer_free;
    cfg->dma.dma_device->num_rx_chan = 1;       /*turn on all the receive channels */
    cfg->dma.dma_device->num_tx_chan = 1;       

    cfg->dma.dma_device->tx_burst_len = DMA_BURSTL_4DW;
    cfg->dma.dma_device->rx_burst_len = DMA_BURSTL_4DW;
	
    cfg->dma.dma_device->tx_endianness_mode  =IFX_DMA_ENDIAN_TYPE0;
    cfg->dma.dma_device->rx_endianness_mode  =IFX_DMA_ENDIAN_TYPE0;
	
    cfg->dma.dma_device->port_tx_weight = 7;


    for (i = 0; i < cfg->dma.dma_device->num_rx_chan; i++) {
        cfg->dma.dma_device->rx_chan[i]->packet_size = 0; /* MAX_RECEIVE_LENGTH -- not used for SDIO */
        cfg->dma.dma_device->rx_chan[i]->control = IFX_DMA_CH_ON;
        cfg->dma.dma_device->rx_chan[i]->desc_handle= 1;
/*         cfg->dma.dma_device->rx_chan[i]->desc_len= 1; */
    }

   ret = dma_device_register (cfg->dma.dma_device);
	if ( ret != IFX_SUCCESS)
	        printk(KERN_ERR "%s[%d]: Register with DMA core driver Failed!!!\n", __func__,__LINE__);

    return ret;
    
}

static int cleanup_dma_driver (ifx_int_sdio_config_t *cfg)
{
    if (cfg == NULL)
        return -EINVAL;
    
    if (cfg->dma.dma_device) {
        dma_device_unregister (cfg->dma.dma_device);
        dma_device_release(cfg->dma.dma_device);
        cfg->dma.dma_device = NULL;
    }
    return 0;
}

static int dma_tx_channel_reset(struct dma_device_info *dma_dev)
{
    _dma_channel_info *pCh;
	
    pCh = dma_dev->tx_chan[dma_dev->current_tx_chan];
    pCh->reset(pCh);

    return 0;
}


static int dma_rx_channel_reset(ifx_int_sdio_config_t *cfg)
{
    _dma_channel_info *pCh;
   struct dma_device_info *dma_dev = cfg->dma.dma_device;

    pCh = dma_dev->rx_chan[dma_dev->current_rx_chan];
    pCh->reset(pCh);
    
    cfg->dma.rx_dma_opened = 0;

    return 0;
}


static int sdio_dma_intr_handler (struct dma_device_info *dma_dev, int status)
{
    ifx_int_sdio_config_t *cfg = &intcfg[0]; /* TODO: Get handle from DMA driver */
    int errno;
    
/*     printk("TRACE %s %s %d dma_dev=0x%08x status=%d\n",__FILE__,__FUNCTION__,__LINE__,(uint32_t)dma_dev,status); */

//    IFX_SDIO_FUNC_DMSG ("got dma interrupt (status=%d)!\n", status);

#ifdef IFX_SDIO_USE_TIMER
    del_timer(&cfg->timer);
#endif

    errno = mcistat2errno(MMC_READ_REG32(MCI_STAT));
   /* If there is an error indication in the SDIO status, we will be called by SDI int */
   /* To avoid race condition, don't do anything from DMA interrupt context */
    if (errno == 0) {
        switch (status) {
        case RCV_INT:
/*             IFX_SDIO_EMSG ("SDIO RX_CPT_INT\n"); */
            ifx_data_recv (cfg);
            break;
        case TRANSMIT_CPT_INT:
/*             IFX_SDIO_EMSG ("SDIO TX_CPT_INT\n"); */
            ifx_sdio_dma_tx_complete(cfg);
            break;
        case TX_BUF_FULL_INT:
            IFX_SDIO_EMSG ("SDIO TX_BUF_FULL_INT\n");
            break;
        }
    }
    return 0;
}

static u8 *sdio_dma_buffer_alloc (int len, int *byte_offset, void **opt)
{
    IFX_SDIO_FUNC_DMSG("entered\n");

        /* buffer will be provided, when data is read via SDIO */
    *byte_offset = 0;
    *opt = 0;
    return 0;
}

static int sdio_dma_buffer_free (uint8_t * dataptr, void *opt)
{
    IFX_SDIO_FUNC_DMSG("entered\n");
    return 0;
}

static int ifx_data_recv (ifx_int_sdio_config_t *cfg)
{
    struct dma_device_info *dma_dev;
    int status = 0;
    uint8_t *buffer;
    int len;

    static unsigned long running = 0;     /* set atomically, if fct is running (detect re-entrancy) */

    if (test_and_set_bit(0, &running)) {
            /* double entry */
        IFX_SDIO_EMSG("function called recursively.");
        IFX_SDIO_ASSERT(0);
    }

    dma_dev = cfg->dma.dma_device;

    if ((cfg->state != SDIO_RX_ASYNC_DATA) &&
        (cfg->state != SDIO_RX_ASYNC_CMD)) {
        static volatile int wait = 1;
        IFX_SDIO_EMSG("cfg->state=%d\n", cfg->state);
        while (wait) { }
    }

        /* Mask all interrupts */
    MMC_WRITE_REG32(0, MCI_IM0);
    cfg->dma.dma_device->rx_chan[0]->disable_irq(cfg->dma.dma_device->rx_chan[0]);

        /* try to evaluate SDIO status (MCI_STAT) and DMA status */
        /* In case of error, the DMA channel will be reset and */
        /* dma_device_read must not be called */
    do {
        status = _ifx_sdio_process_rx_data(cfg);
    } while (status == -EINPROGRESS);

    if (status == 0) {
        len = dma_device_read(cfg->dma.dma_device, &buffer, NULL);

/*         MMC_WRITE_REG32(0x43f, MCI_CMD); /\* CMD63 as sync in SDIO analyzer *\/ */
        if (cfg->ecfg.callback_handler)
            cfg->ecfg.callback_handler(cfg->ecfg.context, cfg->state);
    }
    else {
/*         MMC_WRITE_REG32(0x43f, MCI_CMD); /\* CMD63 as sync in SDIO analyzer *\/ */
        if (cfg->ecfg.callback_handler)
            cfg->ecfg.callback_handler(cfg->ecfg.context, status);
    }
    
    if (!cfg->ecfg.block_mode && !cfg->ecfg.crc) {
        cfg->write_delay_jiffies = jiffies;
        cfg->write_delay_c0count = read_c0_count();
    }

    clear_bit(0, &running);

    return status;
}

static int ifx_sdio_dma_tx_complete(ifx_int_sdio_config_t *cfg)
{
    struct dma_device_info *dma_dev;
    int status = 0;
    int proc_count = 0;         /* how often did we call process_tx_data */
    uint32_t proc_c0;

    static unsigned long running = 0;     /* set atomically, if fct is running (detect re-entrancy) */

    if (test_and_set_bit(0, &running)) {
            /* double entry */
        IFX_SDIO_EMSG("function called recursively.");
        IFX_SDIO_ASSERT(0);
    }

    IFX_SDIO_ASSERT(cfg->state == SDIO_TX_ASYNC_DATA);

    dma_dev = cfg->dma.dma_device;

        /* Mask all interrupts */
    MMC_WRITE_REG32(0, MCI_IM0);
    cfg->dma.dma_device->tx_chan[0]->disable_irq(cfg->dma.dma_device->tx_chan[0]);


    /* wait for transmitter to become inactive */
    /* or when there was an error */
    proc_c0 = read_c0_count();
    do {
        status = _ifx_sdio_process_tx_data(cfg);
        proc_count++;
    } while (status == -EINPROGRESS);
    proc_c0 = read_c0_count() - proc_c0;
    
    cfg->stat.process_tx_poll_sum += proc_count;
    cfg->stat.process_tx_poll_count++;
    if (proc_count > cfg->stat.process_tx_poll_max)
        cfg->stat.process_tx_poll_max = proc_count;

    cfg->stat.process_tx_c0_sum += proc_c0;
    cfg->stat.process_tx_c0_count++;
    if (proc_c0 > cfg->stat.process_tx_c0_max)
        cfg->stat.process_tx_c0_max = proc_c0;

     
/*     MMC_WRITE_REG32(0x43f, MCI_CMD); /\* CMD63 as sync in SDIO analyzer *\/ */
    if (cfg->ecfg.callback_handler)
        cfg->ecfg.callback_handler(cfg->ecfg.context, status == 0 ? SDIO_TX_ASYNC_DATA : status);

    clear_bit(0, &running);
    
    return status;
}
#endif /* IFX_SDIO_USE_DMA */

/*
  Platform specififc functions
 */
static int ifx_gpio_configure (ifx_platform_config_t *platform);
static void ifx_gpio_release (ifx_platform_config_t *platform);
static int ifx_gpio_ext_reset(ifx_platform_config_t *platform, int active);
static int ifx_reserve_and_configure_gpio(int pin, int dir,
                                          int altsel0, int altsel1,
                                          int pud, int st, int od);
static void ifx_sdio_ack_irq(struct _ifx_platform_config *platform,
                             int ack_irq0, int ack_irq1);

static int ifx_sdio_fill_platform(ifx_int_sdio_config_t *icfg)
{
    ifx_platform_config_t *pcfg = &icfg->platform;
    ifx_sdio_config_t *ecfg = &icfg->ecfg;
    
    /* interrupt definition */
    pcfg->irq0 = IFX_MMC_CONTROLLER_INTR0_IRQ;
    pcfg->irq1 = IFX_MMC_CONTROLLER_INTR1_IRQ;

    if (!ecfg->irq_oob)               /* In-Band Interrupt via SDIO DAT[1] */
        pcfg->dev_irq = IFX_MMC_CONTROLLER_SDIO_I_IRQ;
    else
        pcfg->dev_irq = IFX_EIU_IR1; /* External Interrupt 1 EIU_IR1 */

    pcfg->rst_gpio = ecfg->rst_gpio;
    
        /* gpio for SDIO definition */
    pcfg->gpio_cmd   = IFX_MCLCMD;         /* GPIO 1.3 */
    pcfg->gpio_clk   = IFX_MCLCLK;         /* GPIO 0.15 */
    pcfg->gpio_data0 = IFX_MCLDATA0;       /* GPIO 0.12 */
    pcfg->gpio_data1 = IFX_MCLDATA1;       /* GPIO 1.10 */
    pcfg->gpio_data2 = IFX_MCLDATA2;       /* GPIO 1.12 */
    pcfg->gpio_data3 = IFX_MCLDATA3;       /* GPIO 1.4 */

    pcfg->ifx_sdio_gpio_configure   = ifx_gpio_configure;
    pcfg->ifx_sdio_gpio_release     = ifx_gpio_release;
    pcfg->ifx_sdio_gpio_ext_reset   = ifx_gpio_ext_reset;
    pcfg->sdio_ack_irq              = ifx_sdio_ack_irq;

    return 0;
}
/*
  reserve and configure a gpio port
  Parameter:
        port    port number (0..1)
        pin     pin number (0..15) within port
        dir     direction selector (0: input; 1: output)
        altsel0 alternate setting 0
        altsel1 alternate setting 1
        pud     pull-up/-down selector (0: disable, 1: pull-up, 2: pull-down)
        st      Schmitt-Trigger (0: disable; 1: enable)
        od      open-drain output (0: enable; 1: disable)
  Return: 0, when everything is fine
          All other values indicate a problem,
          please see port.c file for details on return codes
 */
static int ifx_reserve_and_configure_gpio(int pin, int dir,
                                          int altsel0, int altsel1,
                                          int pud, int st, int od)
{
    int ret;
    int module_id = IFX_GPIO_MODULE_SDIO;

    ret = ifx_gpio_pin_reserve(pin, module_id);
    if (ret)
        goto fail;
    
        /* Direction */
    ret = (dir) ?
        ifx_gpio_dir_out_set(pin, module_id) :
        ifx_gpio_dir_in_set(pin, module_id);
    if (ret)
        goto fail;

        /* altsel0 */
    ret = (altsel0) ?
        ifx_gpio_altsel0_set(pin, module_id) :
        ifx_gpio_altsel0_clear(pin, module_id);
    if (ret)
        goto fail;

        /* altsel1 */
    ret = (altsel1) ?
        ifx_gpio_altsel1_set(pin, module_id) :
        ifx_gpio_altsel1_clear(pin, module_id);
    if (ret)
        goto fail;

        /* pull-up/-down */
    if (pud == 0)
        ret = ifx_gpio_puden_clear(pin, module_id);
    else
    {
        ret = ifx_gpio_puden_set(pin, module_id);
        if (ret)
            goto fail;
        if (pud == 1)
            ret = ifx_gpio_pudsel_set(pin, module_id);
        else
            ret = ifx_gpio_pudsel_clear(pin, module_id);
    }
    if (ret)
        goto fail;

        /* schmitt-trigger */
    ret = (st) ?
        ifx_gpio_stoff_clear(pin, module_id) :
        ifx_gpio_stoff_set(pin, module_id);
    if (ret)
        goto fail;

        /* open-drain output */
    ret = (od) ?
        ifx_gpio_open_drain_set(pin, module_id) :
        ifx_gpio_open_drain_clear(pin, module_id);

  fail:
    return ret;
            
}

/*
  This function initializes the GPIO pins for use as SDIO interface
  Parameter: none
  Return: 0, when everything is fine
          All other values indicate a problem,
          please see port.c file for details on return codes
 */

static int ifx_gpio_configure (ifx_platform_config_t *platform)
{
    int module_id = IFX_GPIO_MODULE_SDIO;
    int ret;
    
#if defined (CONFIG_AMAZON_SE)
   amazon_se_mmc_gpio_configure();
#elif defined (CONFIG_DANUBE)
   danube_mmc_gpio_configure();
#elif defined (CONFIG_AR9)
   ar9_mmc_gpio_configure();
#elif defined(CONFIG_VR9)
   vr9_mmc_gpio_configure();
#endif /* #if defined (CONFIG_AMAZON_SE) */

#if defined (CONFIG_AMAZON_SE)
    if (platform->dev_irq == IFX_EIU_IR1)
    {
            /////////////////////////EXIN1/////////////////////////////
            //GPIO29 (P1.13): DIR=0,ALT0=1,ALT1=1,OUT=1, OD=1
        IFX_SDIO_DMSG ("using GPIO 29 as EXIN1.\n");
        ret = ifx_reserve_and_configure_gpio(29, 0, 1, 1, 0, 0, 1);
        if (ret)
            goto fail_exin1_reserve;

        (*IFX_ICU_EIU_EXIN_C) |= 0x060; /*Bit 6:4, b'110->low level*/
        /* enable interrupt only */
        (*IFX_ICU_EIU_INEN) |= 0x02; /*Bit 1 , 1/0-enable/disable*/
    }
#elif defined (CONFIG_DANUBE)
   #error "ifx_gpio_configure for danube"
#elif defined (CONFIG_AR9)
   #error "ifx_gpio_configure for ar9"
#elif defined(CONFIG_VR9)
   #error "ifx_gpio_configure for vr9"
#endif /* #if defined (CONFIG_AMAZON_SE) */

    if (platform->rst_gpio < 32) {
            /////////////////////////Reset/////////////////////////////
        IFX_SDIO_DMSG ("using GPIO %d as reset output.\n", platform->rst_gpio);
        ret = ifx_reserve_and_configure_gpio(platform->rst_gpio,
                                             1, 0, 0, 1, 0, 1);
        if (ret)
            goto fail_reset_reserve;
        
    }
    return 0;
    
  fail_reset_reserve:
    if (platform->dev_irq == IFX_EIU_IR1)
       ifx_gpio_pin_free(19, module_id);
  fail_exin1_reserve:
      ifx_gpio_pin_free(IFX_MCLDATA3, module_id);
  fail_mcldata3_reserve:
      ifx_gpio_pin_free(IFX_MCLDATA2, module_id);
  fail_mcldata2_reserve:
      ifx_gpio_pin_free(IFX_MCLDATA1, module_id);
  fail_mcldata1_reserve:
      ifx_gpio_pin_free(IFX_MCLDATA0, module_id);
  fail_mcldata0_reserve:
      ifx_gpio_pin_free(IFX_MCLCLK, module_id);
  fail_mclcmd_reserve:
      ifx_gpio_pin_free(IFX_MCLCMD, module_id);
  fail_mclclk_reserve:
    IFX_SDIO_EMSG("GPIO configuration failed, ret=%d\n", ret);
    printk("GPIO configuration failed, ret=%d\n", ret);

    return ret;
}

/*
  Release all reserved gpio's
 */
static void ifx_gpio_release (ifx_platform_config_t *platform)
{
    int module_id = (int)IFX_GPIO_MODULE_SDIO;

#if defined (CONFIG_AMAZON_SE)
    if (platform->dev_irq == IFX_EIU_IR1)
        ifx_gpio_pin_free(29, module_id); /* EXIN1 */
#elif defined (CONFIG_DANUBE)
   #error "ifx_gpio_release for danube"
#elif defined (CONFIG_AR9)
   #error "ifx_gpio_release for ar9"
#elif defined(CONFIG_VR9)
   #error "ifx_gpio_release for vr9"
#endif /* #if defined (CONFIG_AMAZON_SE) */

    if (platform->rst_gpio < 32)
        ifx_gpio_pin_free(platform->rst_gpio, module_id);

    ifx_gpio_pin_free(IFX_MCLDATA3, module_id); /* MCI_DATA3 */
    ifx_gpio_pin_free(IFX_MCLDATA2, module_id); /* MCI_DATA2 */
    ifx_gpio_pin_free(IFX_MCLDATA1, module_id); /* MCI_DATA1 */
    ifx_gpio_pin_free(IFX_MCLDATA0, module_id); /* MCI_DATA0 */
    ifx_gpio_pin_free(IFX_MCLCMD, module_id);   /* MCI_CMD */
    ifx_gpio_pin_free(IFX_MCLCLK, module_id);   /* MCI_CLK */
}

static int ifx_gpio_ext_reset(ifx_platform_config_t *platform, int active)
{
    int module_id = (int)IFX_GPIO_MODULE_SDIO;

    IFX_SDIO_ASSERT((active ==0) || (active == 1));

    if (active)
        ifx_gpio_output_clear(platform->rst_gpio, module_id);
    else
        ifx_gpio_output_set(platform->rst_gpio, module_id);

    return 0;
}

static void _amazon_se_sdio_ack_irq(int irq_nr)
{
    /* copied from interrupt.c */
    /* TODO: make generic funtion in interrupt.c */
    
    /* have to access the correct register here */
    if (irq_nr <= INT_NUM_IM0_IRL31 && irq_nr >= INT_NUM_IM0_IRL0)
    {
        /* access IM0 */
        /* ack */
        *IFX_ICU_IM0_ISR = IFX_ICU_IM0_IR(irq_nr);
    }else if (irq_nr <= INT_NUM_IM1_IRL31 && irq_nr >= INT_NUM_IM1_IRL0)
    {
        /* access IM1 */
        /* ack */
        *IFX_ICU_IM1_ISR = IFX_ICU_IM1_IR(irq_nr - 32);
    }else if (irq_nr <= INT_NUM_IM2_IRL31 && irq_nr >= INT_NUM_IM2_IRL0)
    {
        /* access IM2 */
        /* ack */
        *IFX_ICU_IM2_ISR = IFX_ICU_IM2_IR(irq_nr - 64);
    }else if (irq_nr <= INT_NUM_IM3_IRL31 && irq_nr >= INT_NUM_IM3_IRL0)
    {
        /* access IM3 */
        /* ack */
        *IFX_ICU_IM3_ISR = IFX_ICU_IM3_IR(irq_nr - 96);
    }
}

static void ifx_sdio_ack_irq(struct _ifx_platform_config *platform, int ack_irq0, int ack_irq1)
{
#if defined (CONFIG_AMAZON_SE)
    if (ack_irq0)
        _amazon_se_sdio_ack_irq(platform->irq0);
    if (ack_irq1)
        _amazon_se_sdio_ack_irq(platform->irq1);
#elif defined (CONFIG_DANUBE)
   #error "ifx_sdio_ack_irq for danube"
#elif defined (CONFIG_AR9)
   #error "ifx_sdio_ack_irq for ar9"
#elif defined(CONFIG_VR9)
   #error "ifx_sdio_ack_irq for vr9"
#endif /* #if defined (CONFIG_AMAZON_SE) */
}

int ifx_sdio_enumerate(ifx_sdio_handle_t sh)
{
    unsigned int reply;
    int ret;
    uint32_t response[4];

    IFX_SDIO_PROLOG(sh, cfg);

    if (cfg->ecfg.rst_gpio != 255) {    /* external reset defined */
            /* activate reset for 1 ms */
         IFX_SDIO_DMSG ("Set reset active (low)\n");
        cfg->platform.ifx_sdio_gpio_ext_reset(&cfg->platform, 1);
        udelay(1000);
        cfg->platform.ifx_sdio_gpio_ext_reset(&cfg->platform, 0);
        IFX_SDIO_DMSG ("Set reset inactive (high)\n");
        udelay(1000);
    }

/* command 0 */
    ret = ifx_sdio_cmd_ext(sh, 0 /* SD_IO_GO_IDLE_STATE */, 0, MMC_RSP_NONE, response);
    if(ret)return ret;

/* command 5 */
    IFX_SDIO_DMSG ("issue command 5 - expect CRC error (ok)\n");

    ret = ifx_sdio_cmd_ext(sh, 5 /* SDIO_CMD5 */, 0 /* VDD_VOLTAGE_WINDOW */,MMC_RSP_R4, response);
    if(ret && ret != -EILSEQ) {
       IFX_SDIO_EMSG ("commnd 5 Ret value: %d\n\n", ret);
       return ret;
    }
    IFX_SDIO_DMSG ("The OCR value returned is 0x%0x\n",response[0]);
    IFX_SDIO_DMSG ("issue command 5 - expect CRC error (ok)\n");

    ret = ifx_sdio_cmd_ext(sh, 5 /* SDIO_CMD5 */, 0 /* VDD_VOLTAGE_WINDOW */,MMC_RSP_R4, response);
    if(ret && ret != -EILSEQ) {
       IFX_SDIO_EMSG ("commnd 5 Ret value: %d\n\n", ret);
       return ret;
    }
    IFX_SDIO_DMSG ("The OCR value returned is 0x%0x\n",response[0]);
    reply = 0xff8000;
    IFX_SDIO_DMSG ("The OCR value sent is 0x%0x\n",reply);

    ret = ifx_sdio_cmd_ext(sh, 5 /* SDIO_CMD5 */, reply,MMC_RSP_R4, response);        
    if(ret && ret != -EILSEQ) {
       IFX_SDIO_EMSG ("commnd 5 Ret value: %d\n\n", ret);
       return ret;
    }
    IFX_SDIO_DMSG ("The OCR value returned is 0x%0x\n",response[0]);

/* command SD_IO_SEND_RELATIVE_ADDR */
    IFX_SDIO_DMSG ("issue command SD_IO_SEND_RELATIVE_ADDR\n");
    ret = ifx_sdio_cmd_ext(sh, 3 /* SD_IO_SEND_RELATIVE_ADDR */, 0,MMC_RSP_R6, response);        
    if(ret) {
       IFX_SDIO_EMSG ("commnd send relative addr Ret value: %d\n\n", ret);
       return ret;
    }
    IFX_SDIO_DMSG ("longval is %0x\n",response[0]);
#if 1 
/* command SD_IO_SELECT_CARD */
    IFX_SDIO_DMSG ("issue command SD_IO_SELECT_CARD\n");
    ret = ifx_sdio_cmd_ext(sh, 7 /* SD_IO_SELECT_CARD */,response[0] /* longval */,MMC_RSP_R6, response);        
    if(ret) {
       IFX_SDIO_EMSG ("commnd send IO select card ret value: %d\n\n", ret);
       return ret;
    }
    IFX_SDIO_DMSG ("RCAVALUE: %08X\n\n",response[0]);
#if 1 
    IFX_SDIO_DMSG ("initialize CCCR\n");
    reply = 2;
    ret = ifx_sdio_write_byte_ext(sh,
                                  2, /* CCCR_IO_ENABLE */
                                  0, /* function */
                                  0, /* raw */
                                  2, /* value */
                                  NULL);

        


    if(ret)return ret;
    IFX_SDIO_DMSG ("issue command CCCR_BUS_INTERFACE_CONTOROL\n");
    ret = ifx_sdio_write_byte_ext(sh,
                                  7, /* CCCR_BUS_INTERFACE_CONTOROL */
                                  0, /* function */
                                  0, /* raw */
                                  (cfg->ecfg.bus_width == IFX_SDIO_IOCTL_BUS_WIDTH_4) ? 2 : 0, /* value */
                                  NULL);

#endif /* #if 1 */
#endif /* #if 1 */
    {
            /* TODO: Do this in WLAN 1752 driver */
        uint8_t out;
        IFX_SDIO_DMSG ("setting CCCR register 0xF5 to 0x00 (enable CRC reporting)\n");
        ret = ifx_sdio_write_byte_ext(sh,
                                      0xF5, /* I/O block size */
                                      0, /* function */
                                      1, /* raw */
                                      0, /* value */
                                      &out);
        IFX_SDIO_DMSG ("returned value is 0x%02X\n", out);
    }

    IFX_SDIO_DMSG ("exiting %s\n",__FUNCTION__);

    ifx_sdio_set_clock(cfg, cfg->ecfg.clock);
        
    return ret;
}


module_init (ifx_sdio_init_module);
module_exit (ifx_sdio_exit_module);

EXPORT_SYMBOL(ifx_sdio_open);
EXPORT_SYMBOL(ifx_sdio_close);
EXPORT_SYMBOL(ifx_sdio_ioctl);
EXPORT_SYMBOL(ifx_sdio_request_irq);
EXPORT_SYMBOL(ifx_sdio_free_irq);
EXPORT_SYMBOL(ifx_sdio_enable_irq);
EXPORT_SYMBOL(ifx_sdio_disable_irq);
EXPORT_SYMBOL(ifx_sdio_cmd);
EXPORT_SYMBOL(ifx_sdio_cmd_ext);
EXPORT_SYMBOL(ifx_sdio_write_byte);
EXPORT_SYMBOL(ifx_sdio_write_byte_ext);
EXPORT_SYMBOL(ifx_sdio_write_sync);
EXPORT_SYMBOL(ifx_sdio_write_sync_ext);
EXPORT_SYMBOL(ifx_sdio_write_async);
EXPORT_SYMBOL(ifx_sdio_write_async_ext);
EXPORT_SYMBOL(ifx_sdio_read_byte);
EXPORT_SYMBOL(ifx_sdio_read_byte_ext);
EXPORT_SYMBOL(ifx_sdio_read_sync);
EXPORT_SYMBOL(ifx_sdio_read_sync_ext);
EXPORT_SYMBOL(ifx_sdio_read_async);
EXPORT_SYMBOL(ifx_sdio_read_async_ext);
EXPORT_SYMBOL(ifx_sdio_enumerate);
EXPORT_SYMBOL(ifx_sdio_poll_async_state);
EXPORT_SYMBOL(ifx_sdio_get_state);
