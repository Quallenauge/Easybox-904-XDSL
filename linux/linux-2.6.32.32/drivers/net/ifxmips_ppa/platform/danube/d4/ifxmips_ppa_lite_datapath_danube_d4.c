/******************************************************************************
**
** FILE NAME    : ifxmips_ppa_lite_datapath_danube_d4.c
** PROJECT      : UEIP
** MODULES      : MII0/1 Acceleration Package (Danube/Twinpass PPA D4)
**
** DATE         : 24 MAR 2007
** AUTHOR       : Xu Liang
** DESCRIPTION  : CPU1 ETH Driver with Acceleration Firmware (D4)
** COPYRIGHT    :   Copyright (c) 2006
**          Infineon Technologies AG
**          Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 24 MAR 2007  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/atmdev.h>
#include <linux/init.h>
#include <linux/etherdevice.h>  /*  eth_type_trans  */
#include <linux/ethtool.h>      /*  ethtool_cmd     */
#include <linux/if_ether.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/io.h>
#include <linux/errno.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_ppa_lite_datapath_danube_d4.h"



/*
 * ####################################
 *        OS Versionn Dependent
 * ####################################
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
  #define MOD_INC_USE_COUNT
  #define MOD_DEC_USE_COUNT
#endif



/*
 * ####################################
 *              Board Type
 * ####################################
 */
#define BOARD_DANUBE                            1
#define BOARD_TWINPATH_E                        2



/*
 * ####################################
 *              Definition
 * ####################################
 */

#define YIELD_TYPE                              1   //  1: yield, 2: schedule, 3: xuliang_yield, otherwise: none
#if YIELD_TYPE == 1
  #define my_yield                              yield
#elif YIELD_TYPE == 2
  #define my_yield                              schedule
#elif YIELD_TYPE == 3
  #define my_yield                              xuliang_yield
  extern void xuliang_yield(void);              //  in kernel/sched.c
#else
  #define my_yield()
#endif

#define ENABLE_MY_MEMCPY                        0

#ifndef CONFIG_IFX_PPA_D4_DUALCORE
  #define ENABLE_SINGLE_CORE_EMUL               1
#endif
#define ENABLE_TEMP_TEST_PROC                   1
#define ENABLE_SKB_BALANCE_COUNT                0

#define BOARD_CONFIG                            BOARD_TWINPATH_E

#define ENABLE_DEBUG                            1

#define ENABLE_ASSERT                           1

#define DEBUG_DUMP_SKB_RX                       1

#define DEBUG_DUMP_SKB_TX                       1

#define PPE_MAILBOX_IGU0_INT                    INT_NUM_IM2_IRL23

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
  #define ENABLE_DEBUG_PRINT                    1
  #define DISABLE_INLINE                        1
#else
  #define ENABLE_DEBUG_PRINT                    0
  #define DISABLE_INLINE                        0
#endif

#if !defined(DISABLE_INLINE) || !DISABLE_INLINE
  #define INLINE                                inline
#else
  #define INLINE
#endif

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
  #undef  dbg
  #define dbg(format, arg...)                   do { if ( g_dbg_enable ) printk(KERN_WARNING __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #if !defined(dbg)
    #define dbg(format, arg...)
  #endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
  #define ASSERT(cond, format, arg...)          do { if ( g_dbg_enable && !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #define ASSERT(cond, format, arg...)
#endif

#undef  err
#define err(format, arg...)                     printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg)

#if (defined(DEBUG_DUMP_SKB_RX) && DEBUG_DUMP_SKB_RX) || (defined(DEBUG_DUMP_SKB_TX) && DEBUG_DUMP_SKB_TX)
  #define DEBUG_DUMP_SKB                        1
#else
  #define DEBUG_DUMP_SKB                        0
#endif

#define DUMP_SKB_LEN                            ~0

#if (defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB)                     \
    || (defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT)          \
    || (defined(ENABLE_ASSERT) && ENABLE_ASSERT)
  #define ENABLE_DBG_PROC                       1
#else
  #define ENABLE_DBG_PROC                       0
#endif

/*
 *  Firmware Settings
 */
#define DMA_RX_CH3_DESC_NUM                     24

#define TX_DESC_NUM                             (DMA_RX_CH3_DESC_NUM * 2)

/*
 *  Constants
 */
#define TASKLET_MAX_RX_CHECK_LOOP               25
#define DMA_PACKET_SIZE                         1568
#define MIN_TX_PACKET_LENGTH                    ETH_ZLEN
#define RX_TAIL_CRC_LENGTH                      4
#define CPU1_PACKET_RESERVE                     22
#define CPU1_INTERFACE_ID_OFFSET                21
#define CPU1_INTERFACE_ID                       5

/*
 *  Bits Operation
 */
//#define GET_BITS(x, msb, lsb)                   (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
//#define SET_BITS(x, msb, lsb, value)            (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

/*
 *  FPI Configuration Bus Register and Memory Address Mapping
 */
#define DANUBE_PPE                              (KSEG1 + 0x1E180000)
#define PP32_DEBUG_REG_ADDR(x)                  ((volatile u32*)(DANUBE_PPE + (((x) + 0x0000) << 2)))
#define PPM_INT_REG_ADDR(x)                     ((volatile u32*)(DANUBE_PPE + (((x) + 0x0030) << 2)))
#define PP32_INTERNAL_RES_ADDR(x)               ((volatile u32*)(DANUBE_PPE + (((x) + 0x0040) << 2)))
#define PPE_CLOCK_CONTROL_ADDR(x)               ((volatile u32*)(DANUBE_PPE + (((x) + 0x0100) << 2)))
#define CDM_CODE_MEMORY_RAM0_ADDR(x)            ((volatile u32*)(DANUBE_PPE + (((x) + 0x1000) << 2)))
#define CDM_CODE_MEMORY_RAM1_ADDR(x)            ((volatile u32*)(DANUBE_PPE + (((x) + 0x2000) << 2)))
#define PPE_REG_ADDR(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0x4000) << 2)))
#define PP32_DATA_MEMORY_RAM1_ADDR(x)           ((volatile u32*)(DANUBE_PPE + (((x) + 0x5000) << 2)))
#define PPM_INT_UNIT_ADDR(x)                    ((volatile u32*)(DANUBE_PPE + (((x) + 0x6000) << 2)))
#define PPM_TIMER0_ADDR(x)                      ((volatile u32*)(DANUBE_PPE + (((x) + 0x6100) << 2)))
#define PPM_TASK_IND_REG_ADDR(x)                ((volatile u32*)(DANUBE_PPE + (((x) + 0x6200) << 2)))
#define PPS_BRK_ADDR(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0x6300) << 2)))
#define PPM_TIMER1_ADDR(x)                      ((volatile u32*)(DANUBE_PPE + (((x) + 0x6400) << 2)))
#define SB_RAM0_ADDR(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0x8000) << 2)))
#define SB_RAM1_ADDR(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0x8400) << 2)))
#define SB_RAM2_ADDR(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0x8C00) << 2)))
#define SB_RAM3_ADDR(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0x9600) << 2)))
#define QSB_CONF_REG(x)                         ((volatile u32*)(DANUBE_PPE + (((x) + 0xC000) << 2)))

/*
 *  DWORD-Length of Memory Blocks
 */
#define PP32_DEBUG_REG_DWLEN                    0x0030
#define PPM_INT_REG_DWLEN                       0x0010
#define PP32_INTERNAL_RES_DWLEN                 0x00C0
#define PPE_CLOCK_CONTROL_DWLEN                 0x0F00
#define CDM_CODE_MEMORY_RAM0_DWLEN              0x1000
#define CDM_CODE_MEMORY_RAM1_DWLEN              0x0800
#define PPE_REG_DWLEN                           0x1000
#define PP32_DATA_MEMORY_RAM1_DWLEN             CDM_CODE_MEMORY_RAM1_DWLEN
#define PPM_INT_UNIT_DWLEN                      0x0100
#define PPM_TIMER0_DWLEN                        0x0100
#define PPM_TASK_IND_REG_DWLEN                  0x0100
#define PPS_BRK_DWLEN                           0x0100
#define PPM_TIMER1_DWLEN                        0x0100
#define SB_RAM0_DWLEN                           0x0400
#define SB_RAM1_DWLEN                           0x0800
#define SB_RAM2_DWLEN                           0x0A00
#define SB_RAM3_DWLEN                           0x0400
#define QSB_CONF_REG_DWLEN                      0x0100

/*
 *  Host-PPE Communication Data Address Mapping
 */
#define DMA_RX_CH3_DESC_BASE                    PP32_DATA_MEMORY_RAM1_ADDR(0x0720)
#define CPU1_TX_DESC(x)                         ((volatile struct tx_desc *)SB_RAM3_ADDR(0x0350 + (x) * 2))
#define CPU1_TX_DATA_PAGE(x)                    SB_RAM2_ADDR(0x06A0 + (x) * 25 * 16)    //  CPU1 TX in driver is equivalent to CPU1 RX in firmware

/*
 *  Mailbox IGU0 Registers
 */
#define MBOX_IGU0_ISRS                          PPE_REG_ADDR(0x0200)
#define MBOX_IGU0_ISRC                          PPE_REG_ADDR(0x0201)
#define MBOX_IGU0_ISR                           PPE_REG_ADDR(0x0202)
#define MBOX_IGU0_IER                           PPE_REG_ADDR(0x0203)



/*
 * ####################################
 *              Data Type
 * ####################################
 */

#if defined(__BIG_ENDIAN)
  struct rx_desc {
    u32 OWN                     :1;
    u32 C                       :1;
    u32 SoP                     :1;
    u32 EoP                     :1;
    u32 Burst_length_offset     :3;
    u32 byte_offset             :2;
    u32 reserve                 :7;
    u32 data_length             :16;

    u32 data_pointer            :32;
  };

  struct tx_desc{
    //  0 - 3h
    u32 OWN                     :1;
    u32 C                       :1;
    u32 SoP                     :1;
    u32 EoP                     :1;
    u32 Burst_length_offset     :3;
    u32 byte_offset             :2;
    u32 reserve                 :7;
    u32 data_length             :16;

    //  4 - 7h
    u32 data_pointer            :32;
  };

  struct flag_header {
    //  0 - 3h
    unsigned int    rout_fwd_vld:1;
    unsigned int    rout_mc_vld :1;
    unsigned int    brg_rt_flag :1; //  0: route, 1: bridge
    unsigned int    uc_mc_flag  :1; //  0: uni-cast, 1: multicast
    unsigned int    tcpudp_err  :1;
    unsigned int    tcpudp_chk  :1;
    unsigned int    is_udp      :1;
    unsigned int    is_tcp      :1;
    unsigned int    res2        :2; //  1h
    unsigned int    ip_offset   :6;
    unsigned int    is_pppoes   :1; //  2h
    unsigned int    res3        :1;
    unsigned int    is_ipv4     :1;
    unsigned int    is_vlan     :2;
    unsigned int    res4        :2;
    unsigned int    rout_index  :9;
    //  4 - 7h
    unsigned int    dest_list   :8;
    unsigned int    res5        :1; //  5h
    unsigned int    src_dir     :1;
    unsigned int    acc_done    :1;
    unsigned int    tcp_rst     :1;
    unsigned int    tcp_fin     :1;
    unsigned int    res6        :3;
    unsigned int    temp_dest_list  :8;
    unsigned int    src_itf     :3; //  5h
    unsigned int    pl_byteoff  :5;
  };
#endif

struct cpu1_mib {
    u32 rx_pkts;
    u32 rx_bytes;
    u32 rx_drop_pkts;
    u32 rx_drop_bytes;
    u32 tx_pkts;
    u32 tx_bytes;
    u32 tx_drop_pkts;
    u32 tx_drop_bytes;
    u32 xmit_pkts;
    u32 xmit_bytes;
};



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Sync with CPU0
 */
#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
  extern void  ifx_mps_ppa_init_sync(u32 address);    //  cpu 1 tell the init address to cpu0
#else
  static void  ifx_mps_ppa_init_sync(u32 address) { ((u32 *)address)[DMA_RX_CH3_DESC_NUM] = 1; dma_cache_wback_inv(address, (DMA_RX_CH3_DESC_NUM + 1) * sizeof(u32)); }
#endif
//extern u32 ifx_mps_ppa_get_init_status(void);       //  return the ppa init address of cpu1, called by cpu0, if 0 -> not initialized yet

/*
 *  RX functions
 */
static irqreturn_t mailbox_irq_handler(int irq, void *);
static void do_cpu1_rx_tasklet(unsigned long);
void ppa_lite_stop_rx(void);
void ppa_lite_start_rx(void);

/*
 *  TX functions
 */
int cpu1_hard_start_xmit(struct sk_buff *, struct net_device *);
static int cpu1_tx_d(void *);

/*
 *  Proc File
 */
static INLINE void proc_file_create(void);
static INLINE void proc_file_delete(void);
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int proc_read_dbg(char *, char **, off_t, int, int *, void *);
  static int proc_write_dbg(struct file *, const char *, unsigned long, void *);
#endif
#if defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC
  static int proc_read_test(char *, char **, off_t, int, int *, void *);
  static int proc_write_test(struct file *, const char *, unsigned long, void *);
#endif
static int proc_read_mib(char *, char **, off_t, int, int *, void *);
static int proc_write_mib(struct file *, const char *, unsigned long, void *);
#if (defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC) || (defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC)
  static INLINE int stricmp(const char *, const char *);
#endif

/*
 *  local implement memcpy
 */
#if defined(ENABLE_MY_MEMCPY) && ENABLE_MY_MEMCPY
  static INLINE void *my_memcpy(unsigned char *, const unsigned char *, unsigned int);
#else
  #define my_memcpy             memcpy
#endif

/*
 *  Debug functions
 */
static INLINE void dump_skb(struct sk_buff *, u32, char *, int);

/*
 *  External Variables
 */
#if 0
  extern void (*ppa_lite_tx_full)(void);
  extern void (*ppa_lite_tx_free)(void);
#else
  void (*ppa_lite_tx_full)(void) = NULL;
  void (*ppa_lite_tx_free)(void) = NULL;
#endif

//extern void mask_and_ack_danube_irq(unsigned int);
//extern void enable_danube_irq(unsigned int);



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int g_dbg_enable = 0;
#endif

#if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
  static u32 g_skb_balance_count = 0;
#endif

#if defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC
  static int g_f_mac_test = 0;
  static unsigned char g_mac_compare[12] = {0};
#endif

static volatile int g_f_tx_full = 0;

static unsigned int g_cpu1_tx_data_page_set = 0;

static struct semaphore g_tx_count_sem;
static struct sk_buff *g_tx_head = NULL, *g_tx_tail = NULL;

static volatile int g_f_rx_on = 1;

static DECLARE_TASKLET(g_rx_tasklet, do_cpu1_rx_tasklet, 0);
static int g_in_rx_tasklet = 0;

static unsigned int g_rx_cur = 0;
static struct sk_buff *g_rx_skb[DMA_RX_CH3_DESC_NUM] = {0};
#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
  static unsigned char *g_rx_skb_data[DMA_RX_CH3_DESC_NUM + 1] = {0};   //  the last one used as (uint32_t)flag
#else
  unsigned char *g_rx_skb_data[DMA_RX_CH3_DESC_NUM + 1] = {0};
  EXPORT_SYMBOL(g_rx_skb_data);
#endif

#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
  //extern int (*mps_netif_receive_skb_fn)(struct sk_buff *skb);
  int (*g_netif_rx)(struct sk_buff *) = NULL;
#else
  static int g_loop = 0;
  static int mps_netif_receive_skb(struct sk_buff *skb)
  {
    if ( !g_loop )
    {
  #if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
        g_skb_balance_count--;
  #endif
        dev_kfree_skb_any(skb);
    }
    else
        cpu1_hard_start_xmit(skb, NULL);
    return 0;
  }
  static int (*g_netif_rx)(struct sk_buff *) = mps_netif_receive_skb;
#endif

struct cpu1_mib g_cpu1_mib = {0};

/*
 *  PROC
 */
static int g_ppa_proc_dir_flag = 0;
static struct proc_dir_entry *g_ppa_proc_dir = NULL;
static struct proc_dir_entry *g_ppa_lite_proc_dir = NULL;



/*
* ####################################
 *           Global Variable
 * ####################################
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    extern struct proc_dir_entry proc_root;
#endif



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static irqreturn_t mailbox_irq_handler(int irq, void *dev_id)
{
    if ( g_in_rx_tasklet++ )
        return IRQ_HANDLED;

    *MBOX_IGU0_IER = 0x00;
    //mask_and_ack_danube_irq(PPE_MAILBOX_IGU0_INT);

    //err("*MBOX_IGU0_ISR = %08x, *MBOX_IGU0_IER = %08x", *MBOX_IGU0_ISR, *MBOX_IGU0_IER);

    tasklet_schedule(&g_rx_tasklet);

    return IRQ_HANDLED;
}

static void do_cpu1_rx_tasklet(unsigned long arg)
{
    unsigned long sys_flag;
    volatile struct rx_desc *pdesc = (volatile struct rx_desc *)DMA_RX_CH3_DESC_BASE;
    struct rx_desc desc;
    struct sk_buff *skb, *new_skb;
    u32 off;
    int len;
    int i;

    for ( i = 0; i < TASKLET_MAX_RX_CHECK_LOOP; i++ )
    {
        local_irq_save(sys_flag);
        if ( !g_f_rx_on )
        {
            //  stop RX
            g_in_rx_tasklet = 0;
            local_irq_restore(sys_flag);
            return;
        }
        local_irq_restore(sys_flag);

        if ( pdesc[g_rx_cur].OWN )
        {
            //  no available descriptor
            //err("quit tasklet: *MBOX_IGU0_ISR = %08x", *MBOX_IGU0_ISR);
            *MBOX_IGU0_ISRC = *MBOX_IGU0_ISR;
            if ( pdesc[g_rx_cur].OWN )
            {
                //  make sure no available descriptor coming in critical area
                local_irq_save(sys_flag);
                g_in_rx_tasklet = 0;
                if ( g_f_rx_on )
                {
                    //enable_danube_irq(PPE_MAILBOX_IGU0_INT);
                    *MBOX_IGU0_IER = 0x01;
                }
                local_irq_restore(sys_flag);
                return;
            }
        }

        desc = pdesc[g_rx_cur];

        ASSERT(desc.data_length - RX_TAIL_CRC_LENGTH - CPU1_PACKET_RESERVE <= 1536, "desc.data_length = %d", desc.data_length);

        if ( g_netif_rx )
        {
            new_skb = dev_alloc_skb(DMA_PACKET_SIZE + 16);
#if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
            if ( new_skb )
                g_skb_balance_count++;
#endif

            if ( new_skb )
            {
                skb = g_rx_skb[g_rx_cur];
                //off = ((struct flag_header *)skb->data)->pl_byteoff;
                //len = desc.data_length - sizeof(struct flag_header) - off - RX_TAIL_CRC_LENGTH;
                //skb_reserve(skb, sizeof(struct flag_header) + off);
                //skb_put(skb, len);
                off = *((u32 *)skb->data + 1) & ((1 << 5) - 1);
                len = desc.data_length - sizeof(struct flag_header) - off - RX_TAIL_CRC_LENGTH;
                skb->data += sizeof(struct flag_header) + off;
                skb->tail = skb->data + len;
                skb->len  = len;

                dump_skb(skb, DUMP_SKB_LEN, "RX", 0);

                g_cpu1_mib.rx_pkts++;
                g_cpu1_mib.rx_bytes += skb->len;

#if defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC
                if ( g_f_mac_test )
                {
                    int i;

                    for ( i = 0; i < 12; i++ )
                        if ( g_mac_compare[i] != skb->data[i] )
                        {
                            printk("data mac error: "
                                   "%02x:%02x:%02x:%02x:%02x:%02x "
                                   "%02x:%02x:%02x:%02x:%02x:%02x\n",
                                   skb->data[0],
                                   skb->data[1],
                                   skb->data[2],
                                   skb->data[3],
                                   skb->data[4],
                                   skb->data[5],
                                   skb->data[6],
                                   skb->data[7],
                                   skb->data[8],
                                   skb->data[9],
                                   skb->data[10],
                                   skb->data[11]);
                            break;
                        }
                }
#endif
                g_netif_rx(skb);

                if ( ((u32)new_skb->data & 0x0F) != 0 )
                    skb_reserve(new_skb, ~((u32)new_skb->data + 0x0F) & 0x0F);
#ifndef CONFIG_MIPS_UNCACHED
  #if 1
                dma_cache_inv((unsigned long)new_skb->data, DMA_PACKET_SIZE);
  #else
                dma_cache_inv((unsigned long)new_skb->data, 80);
  #endif
#endif
                g_rx_skb[g_rx_cur] = new_skb;

                desc.data_pointer = (u32)new_skb->data & 0x1FFFFFFF;
            }
            else
            {
                g_cpu1_mib.rx_drop_pkts++;
                g_cpu1_mib.rx_drop_bytes += desc.data_length - RX_TAIL_CRC_LENGTH - CPU1_PACKET_RESERVE;
            }
        }
        else
        {
            g_cpu1_mib.rx_drop_pkts++;
            g_cpu1_mib.rx_drop_bytes += desc.data_length - RX_TAIL_CRC_LENGTH - CPU1_PACKET_RESERVE;
        }

        desc.data_length = DMA_PACKET_SIZE;
        desc.OWN         = 1;
        desc.C           = 0;

        *((volatile u32 *)&pdesc[g_rx_cur] + 1) = *((u32 *)&desc + 1);
        *(volatile u32 *)&pdesc[g_rx_cur]       = *(u32 *)&desc;

        if ( ++g_rx_cur == DMA_RX_CH3_DESC_NUM )
            g_rx_cur = 0;
    }

    tasklet_schedule(&g_rx_tasklet);
}

void ppa_lite_stop_rx(void)
{
    g_f_rx_on = 0;
}

void ppa_lite_start_rx(void)
{
    unsigned long sys_flag;

    local_irq_save(sys_flag);
    if ( !g_f_rx_on )
    {
        g_f_rx_on = 1;
        if ( !g_in_rx_tasklet )
        {
            //enable_danube_irq(PPE_MAILBOX_IGU0_INT);
            *MBOX_IGU0_IER = 0x01;
        }
    }
    local_irq_restore(sys_flag);
}

int cpu1_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    unsigned long sys_flags;
    volatile unsigned int counter;

    dump_skb(skb, DUMP_SKB_LEN, "TX", 1);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    counter = (unsigned int)g_tx_count_sem.count.counter;
#else
    counter = g_tx_count_sem.count;
#endif

    if ( g_f_tx_full )
    {
        dbg("overflow");
        g_cpu1_mib.tx_drop_pkts++;
        g_cpu1_mib.tx_drop_bytes += skb->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH : skb->len;
        dev_kfree_skb_any(skb);
        return 0;
    }
    else if ( counter >= TX_DESC_NUM )
    {
        local_irq_save(sys_flags);
        g_f_tx_full = 1;

        if ( ppa_lite_tx_full )
            ppa_lite_tx_full();
        local_irq_restore(sys_flags);
    }

    g_cpu1_mib.tx_pkts++;
    g_cpu1_mib.tx_bytes += skb->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH : skb->len;

    ASSERT(skb->next == NULL, "skb->next != NULL");
    ASSERT(skb->prev == NULL, "skb->prev != NULL");
    //ASSERT(skb->list == NULL, "skb->list != NULL");
    ASSERT(!in_irq(), "in_irq");

    local_irq_save(sys_flags);
    if ( g_tx_tail )
    {
        g_tx_tail->next = skb;
        skb->prev = g_tx_tail;
        g_tx_tail = skb;
    }
    else
        g_tx_head = g_tx_tail = skb;
    local_irq_restore(sys_flags);

    up(&g_tx_count_sem);

    return 0;
}

static int cpu1_tx_d(void *dummy)
{
    unsigned long sys_flags;
    struct sk_buff *skb;
    uint8_t *p;
    struct tx_desc tx_desc;
    volatile unsigned int counter;

    //  init part
    //set_user_nice(current, 10);
    //__set_current_state(TASK_INTERRUPTIBLE);
    //mb();

    for ( ; ; )
    {
        //  while ( !CPU1_TX_DESC(g_cpu1_tx_data_page_set)->OWN || !CPU1_TX_DESC(g_cpu1_tx_data_page_set)->C )
        while ( !CPU1_TX_DESC(g_cpu1_tx_data_page_set)->OWN )
            my_yield();

        ASSERT(!in_irq(), "in_irq");

        down(&g_tx_count_sem);  //  inside: __set_current_state(TASK_RUNNING)

        local_irq_save(sys_flags);
        ASSERT(g_tx_head != NULL, "g_tx_head == NULL");
        skb = g_tx_head;
        g_tx_head = g_tx_head->next;
        if ( g_tx_head )
            g_tx_head->prev = NULL;
        else
            g_tx_tail = NULL;
        local_irq_restore(sys_flags);

        skb->next = skb->prev = NULL;

        p = (uint8_t *)CPU1_TX_DATA_PAGE(g_cpu1_tx_data_page_set);
        my_memcpy(p + CPU1_PACKET_RESERVE, skb->data, skb->len);
#if 0
        if ( memcmp(p + CPU1_PACKET_RESERVE, skb->data, skb->len) != 0 )
            printk("system memcmp failed\n");
        {
            int i;

            for ( i = 0; i < skb->len; i++ )
                if ( p[CPU1_PACKET_RESERVE + i] != skb->data[i] )
                {
                    printk("my memcmp failed\n");
                    break;
                }
        }
#endif

        ((uint8_t *)CPU1_TX_DATA_PAGE(g_cpu1_tx_data_page_set))[CPU1_INTERFACE_ID_OFFSET] = CPU1_INTERFACE_ID;

        tx_desc = *CPU1_TX_DESC(g_cpu1_tx_data_page_set);
        //tx_desc.data_length = skb->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH + RX_TAIL_CRC_LENGTH : skb->len + RX_TAIL_CRC_LENGTH;
        tx_desc.data_length = (skb->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH + RX_TAIL_CRC_LENGTH : skb->len + RX_TAIL_CRC_LENGTH) + RX_TAIL_CRC_LENGTH;
        tx_desc.C = 0;
        tx_desc.OWN = 0;
        //printk("tx_desc.data_length = %d\n", tx_desc.data_length);
        //*CPU1_TX_DESC(g_cpu1_tx_data_page_set) = tx_desc;
        *((u32*)CPU1_TX_DESC(g_cpu1_tx_data_page_set) + 1) = tx_desc.data_pointer;
        *(u32*)CPU1_TX_DESC(g_cpu1_tx_data_page_set) = *(u32*)&tx_desc;

#if 0
        dump_skb(skb, DUMP_SKB_LEN, "TX hardware", 1);
#else
  #if defined(DEBUG_DUMP_SKB_TX) && DEBUG_DUMP_SKB_TX
        if ( g_dbg_enable )
        {
            u32 len;
            char title[] = "TX hardware";
            int i;

            len = tx_desc.data_length;

            if ( len > 1536 )
                printk("too big data length: tx_desc.data_length = %d\n", tx_desc.data_length);
            else
            {
                printk("%s\n", title);
                printk("  tx_desc.data_length = %d\n", tx_desc.data_length);
                for ( i = 1; i <= len; i++ )
                {
                    if ( i % 16 == 1 )
                        printk("  %4d:", i - 1);
                    printk(" %02X", (int)(*(p + CPU1_PACKET_RESERVE + i - 1) & 0xFF));
                    if ( i % 16 == 0 )
                        printk("\n");
                }
                if ( (i - 1) % 16 != 0 )
                    printk("\n");
            }
        }
  #endif
#endif

        g_cpu1_mib.xmit_pkts++;
        g_cpu1_mib.xmit_bytes += skb->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH : skb->len;

#if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
        g_skb_balance_count--;
#endif
        dev_kfree_skb_any(skb);

        g_cpu1_tx_data_page_set ^= 0x01;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        counter = (unsigned int)g_tx_count_sem.count.counter;
#else
        counter = g_tx_count_sem.count;
#endif
        if ( g_f_tx_full && counter < TX_DESC_NUM - DMA_RX_CH3_DESC_NUM / 2 )
        {
            local_irq_save(sys_flags);
            g_f_tx_full = 0;
            if ( ppa_lite_tx_free )
                ppa_lite_tx_free();
            local_irq_restore(sys_flags);
        }
    }

    return 0;
}

static INLINE void proc_file_create(void)
{
    struct proc_dir_entry *res;

    for ( res = proc_root.subdir; res; res = res->next )
        if ( res->namelen == 3
            && res->name[0] == 'p'
            && res->name[1] == 'p'
            && res->name[2] == 'a' ) //  "ppa"
        {
            g_ppa_proc_dir = res;
            break;
        }
    if ( !res )
    {
        g_ppa_proc_dir = proc_mkdir("ppa", NULL);
        g_ppa_proc_dir_flag = 1;
    }

    g_ppa_lite_proc_dir = proc_mkdir("lite", g_ppa_proc_dir);

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    res = create_proc_read_entry("dbg",
                                  0,
                                  g_ppa_lite_proc_dir,
                                  proc_read_dbg,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_dbg;
#endif

#if defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC
    res = create_proc_read_entry("test",
                                  0,
                                  g_ppa_lite_proc_dir,
                                  proc_read_test,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_test;
#endif

    res = create_proc_read_entry("mib",
                                  0,
                                  g_ppa_lite_proc_dir,
                                  proc_read_mib,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mib;
}

static INLINE void proc_file_delete(void)
{
    remove_proc_entry("mib",
                      g_ppa_lite_proc_dir);

#if defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC
    remove_proc_entry("test",
                      g_ppa_lite_proc_dir);
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    remove_proc_entry("dbg",
                      g_ppa_lite_proc_dir);
#endif

    remove_proc_entry("lite",
                      g_ppa_proc_dir);

    if ( g_ppa_proc_dir_flag )
    {
        remove_proc_entry("ppa", NULL);
        g_ppa_proc_dir = NULL;
        g_ppa_proc_dir_flag = 0;
    }
}

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    MOD_INC_USE_COUNT;

    if ( g_dbg_enable )
        len += sprintf(page + off + len, "debug enabled (g_dbg_enable = %08X)\n", g_dbg_enable);
    else
        len += sprintf(page + off + len, "debug disabled\n");

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_dbg(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[64];
    char *p;

    int len, rlen;

    MOD_INC_USE_COUNT;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        MOD_DEC_USE_COUNT;
        return 0;
    }

    if ( stricmp(str, "enable") == 0 )
        g_dbg_enable = 1;
    else
        g_dbg_enable = 0;

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

#if defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC
static int proc_read_test(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    MOD_INC_USE_COUNT;

    len += sprintf(page + off + len, "xmit - send a packet\n");
  #if defined(ENABLE_SINGLE_CORE_EMUL) && ENABLE_SINGLE_CORE_EMUL
    len += sprintf(page + off + len, "g_loop = %d\n", g_loop);
  #endif
  #if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
    len += sprintf(page + off + len, "g_skb_balance_count = %d\n", g_skb_balance_count);
  #endif
    len += sprintf(page + off + len, "g_f_rx_on               = %d\n", g_f_rx_on);
    len += sprintf(page + off + len, "g_f_tx_full             = %d\n", g_f_tx_full);
    len += sprintf(page + off + len, "g_cpu1_tx_data_page_set = %u\n", g_cpu1_tx_data_page_set);
    len += sprintf(page + off + len, "g_in_rx_tasklet         = %d\n", g_in_rx_tasklet);
    len += sprintf(page + off + len, "g_rx_cur                = %u\n", g_rx_cur);
    len += sprintf(page + off + len, "g_netif_rx              = %08x\n", (u32)g_netif_rx);
    len += sprintf(page + off + len, "ppa_lite_tx_full        = %08x\n", (u32)ppa_lite_tx_full);
    len += sprintf(page + off + len, "ppa_lite_tx_free        = %08x\n", (u32)ppa_lite_tx_free);
    if ( g_f_mac_test )
    {
        len += sprintf(page + off + len,
                       "g_f_mac_test on"
                       " %02x:%02x:%02x:%02x:%02x:%02x ->"
                       " %02x:%02x:%02x:%02x:%02x:%02x\n",
                       g_mac_compare[6],
                       g_mac_compare[7],
                       g_mac_compare[8],
                       g_mac_compare[9],
                       g_mac_compare[10],
                       g_mac_compare[11],
                       g_mac_compare[0],
                       g_mac_compare[1],
                       g_mac_compare[2],
                       g_mac_compare[3],
                       g_mac_compare[4],
                       g_mac_compare[5]);
    }
    else
        len += sprintf(page + off + len, "g_f_mac_test off\n");

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_test(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[64];
    char *p;
    volatile unsigned int counter = 0;

    int len, rlen;

    int flag;

    if ( in_irq() )
        printk("Warning: we are in irq\n");

    MOD_INC_USE_COUNT;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        MOD_DEC_USE_COUNT;
        return 0;
    }

    if ( str[2] == ' ' || str[2] == '\t' )
    {
        str[2] = 0;
        flag = 0;
    }
    else if ( str[3] == ' ' || str[3] == '\t' )
    {
        str[3] = 0;
        flag = 0;
    }
    else if ( str[4] == ' ' || str[4] == '\t' )
    {
        str[4] = 0;
        flag = 0;
    }
    else if ( str[6] == ' ' || str[6] == '\t' )
    {
        str[6] = 0;
        flag = 0;
    }
    else
        flag = 1;

    if ( stricmp(str, "xmit") == 0 )
    {
        struct sk_buff *skb;

        if ( !flag )
        {
            int i;

            for ( i = 5; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
        }

        for ( ; flag; flag-- )
        {
            skb = dev_alloc_skb(64);
  #if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
            if ( skb )
                g_skb_balance_count++;
  #endif
            if ( !skb )
                err("dev_alloc_skb -> NULL");
            else
            {
  #if 0
                const unsigned char data[] = {
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x16,
                    0x41, 0xad, 0x73, 0xfc, 0x08, 0x06, 0x00, 0x01,
                    0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x19,
                    0x41, 0xaf, 0x73, 0xf1, 0xac, 0x14, 0x50, 0x53,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 0x14,
                    0x50, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00,
                };
  #else
                const unsigned char data[] = {
                    0x00, 0x00, 0x00, 0x33, 0x03, 0x01, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x41, 0x08, 0x00, 0x45, 0x00,
                    0x00, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x40, 0x11,
                    0xa4, 0x68, 0x0a, 0x0a, 0x0a, 0x64, 0xc0, 0xa8,
                    0x01, 0x01, 0x13, 0x88, 0x17, 0x70, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00,
                };
  #endif

                skb_reserve(skb, 2);
                skb_put(skb, sizeof(data));

                my_memcpy(skb->data, data, sizeof(data));

                cpu1_hard_start_xmit(skb, NULL);
            }
        }
    }
    else if ( stricmp(str, "mac") == 0 )
    {
        if ( flag )
        {
            //  turn off
            g_f_mac_test = 0;
        }
        else if ( rlen > 38 )
        {
            int i;

            for ( i = 4; i <= 38; i++ )
                if ( str[i] >= '0' && str[i] <= '9' )
                    str[i] -= '0';
                else if ( str[i] >= 'a' && str[i] <= 'f' )
                    str[i] -= 'a' - 10;
                else if ( str[i] >= 'A' && str[i] <= 'F' )
                    str[i] -= 'A' - 10;

            g_mac_compare[0x00] = str[4] << 4 | str[5];
            g_mac_compare[0x01] = str[7] << 4 | str[8];
            g_mac_compare[0x02] = str[10] << 4 | str[11];
            g_mac_compare[0x03] = str[13] << 4 | str[14];
            g_mac_compare[0x04] = str[16] << 4 | str[17];
            g_mac_compare[0x05] = str[19] << 4 | str[20];
            g_mac_compare[0x06] = str[22] << 4 | str[23];
            g_mac_compare[0x07] = str[25] << 4 | str[26];
            g_mac_compare[0x08] = str[28] << 4 | str[29];
            g_mac_compare[0x09] = str[31] << 4 | str[32];
            g_mac_compare[0x0A] = str[34] << 4 | str[35];
            g_mac_compare[0x0B] = str[37] << 4 | str[38];

            g_f_mac_test = 1;
        }
    }
    else if ( stricmp(str, "memcpy") == 0 )
    {
  #if 0
        unsigned char buffer[256] = {0};
        unsigned char *buf = (unsigned char *)(((u32)buffer + 0x0F) & ~0x0F);
        unsigned char *buf_phy;
        volatile unsigned char *p = (volatile uint8_t *)CPU1_TX_DATA_PAGE(0) + CPU1_PACKET_RESERVE;
        struct sk_buff *skb = dev_alloc_skb(1536);
        int i;

    #if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
        if ( skb )
            g_skb_balance_count++;
    #endif

        buf = (unsigned char *)(((u32)skb->data + 0x1F) & ~0x1F);
        dma_cache_inv((unsigned long)skb->head, (u32)skb->end - (u32)skb->head);
        printk("skb->head = %08x\n", (u32)skb->head);

        if ( !flag )
        {
            for ( i = 7; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
            buf += flag;
        }

        buf_phy = (unsigned char *)((u32)buf | KSEG1);
        for ( i = 0; i < 89; i++ )
            buf_phy[i] = i + 1;

        memcpy((void *)p, buf, 89);

        for ( i = 0; i < 89; i++ )
            if ( p[i] != buf[i] )
            {
                printk("--- not match ---\n");
                break;
            }
        if ( i == 89 )
            printk("+++ match +++\n");

        printk("dest - %08x, src - %08x\n", (u32)p, (u32)buf);
        for ( i = 0; i < 92; i++ )
        {
            flag = i & 0x07;

            if ( !flag )
                printk("  %4d: %02x", i, (unsigned int)p[i]);
            else
            {
                printk(" %02x", (unsigned int)p[i]);
                if ( flag == 7 )
                    printk("\n");
            }
        }
        if ( flag != 7 )
            printk("\n");

  #if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
        g_skb_balance_count--;
  #endif
        dev_kfree_skb_any(skb);
  #else
        int loop = 1, size = 1024;
        struct sk_buff *skb[512] = {0};
        unsigned long start, end;
        int i, j;

        if ( !flag )
        {
            for ( i = 7; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
            loop = flag;

            if ( str[i] )
            {
                flag = 0;
                for ( i++; str[i]; i++ )
                {
                    if ( str[i] >= '0' && str[i] <= '9' )
                        flag = flag * 10 + str[i] - '0';
                    else
                        break;
                }
                if ( flag < 1518 )
                    size = flag;
            }
        }

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
        {
            skb[i] = dev_alloc_skb(1536);
            if ( !skb[i] )
            {
                printk("failed in allocating skb\n");

                while ( --i >= 0 )
                    dev_kfree_skb_any(skb[i]);
                MOD_DEC_USE_COUNT;
                return count;
            }
            for ( j = 0; j < 1536; j++ )
                skb[i]->data[j] = (unsigned char)(j & 0xFF);
        }

        printk("loop %d times with %dbytes data", loop, size);
        if ( loop * size >= 1024 * 1024 )
            printk(" - around %dM", loop * size / (1024 * 1024));
        else if ( loop * size >= 1024 )
            printk(" - around %dK", loop * size / 1024);
        printk("\n");

        start = jiffies;
        for ( i = 0; i < loop; i++ )
            my_memcpy((unsigned char *)CPU1_TX_DATA_PAGE(i & 0x01) + CPU1_PACKET_RESERVE, skb[i % (sizeof(skb) / sizeof(*skb))]->data + CPU1_PACKET_RESERVE, size);
        end = jiffies;

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
            dev_kfree_skb_any(skb[i]);

        if ( start <= end )
            printk("it takes %lumsec\n", (end - start) * 1000 / HZ);
        else
            printk("it takes start %lu end %lu, overflow\n", start, end);
  #endif
    }
    else if ( stricmp(str, "tx") == 0 )
    {
        unsigned long time_jiffies = HZ;
        int size = 1024;
        unsigned long end;
        struct tx_desc tx_desc;
        int count = 0;
        int i;

        if ( !flag )
        {
            for ( i = 3; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
            time_jiffies = (unsigned long)flag * HZ;

            if ( str[i] )
            {
                flag = 0;
                for ( i++; str[i]; i++ )
                {
                    if ( str[i] >= '0' && str[i] <= '9' )
                        flag = flag * 10 + str[i] - '0';
                    else
                        break;
                }
                if ( flag < 1518 )
                    size = flag;
            }
        }

        printk("tx: going to test %lu seconds with %dbytes length packet\n", (time_jiffies + HZ / 2) / HZ, size);

        ((uint8_t *)CPU1_TX_DATA_PAGE(0))[CPU1_INTERFACE_ID_OFFSET] = 5;
        ((uint8_t *)CPU1_TX_DATA_PAGE(1))[CPU1_INTERFACE_ID_OFFSET] = 5;
        end = jiffies + time_jiffies;
        while ( jiffies < end )
        {
            tx_desc = *CPU1_TX_DESC(g_cpu1_tx_data_page_set);
            if ( !tx_desc.OWN )
            {
                my_yield();
                continue;
            }
            tx_desc.data_length = size + 8;
            tx_desc.C = 0;
            tx_desc.OWN = 0;
            *CPU1_TX_DESC(g_cpu1_tx_data_page_set) = tx_desc;
            g_cpu1_tx_data_page_set ^= 0x01;
            count++;
        }
        printk("sent %d packets at %dbytes length\n", count, size);
    }
    else if ( stricmp(str, "txc") == 0 )
    {
        unsigned long time_jiffies = HZ;
        struct sk_buff *skb[512] = {0};
        int size = 1024;
        unsigned long end;
        struct tx_desc tx_desc;
        int count = 0;
        int i, j;

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
        {
            skb[i] = dev_alloc_skb(1536);
            if ( !skb[i] )
            {
                printk("failed in allocating skb\n");

                while ( --i >= 0 )
                    dev_kfree_skb_any(skb[i]);
                MOD_DEC_USE_COUNT;
                return count;
            }
            for ( j = 0; j < 1536; j++ )
                skb[i]->data[j] = (unsigned char)(j & 0xFF);
        }

        if ( !flag )
        {
            for ( i = 4; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
            time_jiffies = (unsigned long)flag * HZ;

            if ( str[i] )
            {
                flag = 0;
                for ( i++; str[i]; i++ )
                {
                    if ( str[i] >= '0' && str[i] <= '9' )
                        flag = flag * 10 + str[i] - '0';
                    else
                        break;
                }
                if ( flag < 1518 )
                    size = flag;
            }
        }

        printk("txc: going to test %lu seconds with %dbytes length packet\n", (time_jiffies + HZ / 2) / HZ, size);

        end = jiffies + time_jiffies;
        while ( jiffies < end )
        {
            tx_desc = *CPU1_TX_DESC(g_cpu1_tx_data_page_set);
            if ( !tx_desc.OWN )
            {
                my_yield();
                continue;
            }

            ((uint8_t *)CPU1_TX_DATA_PAGE(g_cpu1_tx_data_page_set))[CPU1_INTERFACE_ID_OFFSET] = 5;
            my_memcpy((unsigned char *)CPU1_TX_DATA_PAGE(g_cpu1_tx_data_page_set) + CPU1_PACKET_RESERVE, skb[count % (sizeof(skb) / sizeof(*skb))]->data + CPU1_PACKET_RESERVE, size);

            tx_desc.data_length = size + 8;
            tx_desc.C = 0;
            tx_desc.OWN = 0;
            *CPU1_TX_DESC(g_cpu1_tx_data_page_set) = tx_desc;
            g_cpu1_tx_data_page_set ^= 0x01;
            count++;
        }

        printk("sent %d packets at %dbytes length\n", count, size);

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
            dev_kfree_skb_any(skb[i]);
    }
    else if ( stricmp(str, "txr") == 0 )
    {
        unsigned long time_jiffies = HZ;
        struct sk_buff *skb[64] = {0};
        int size = 1024;
        unsigned long end;
        int count = 0;
        int i;

        if ( !flag )
        {
            for ( i = 4; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
            time_jiffies = (unsigned long)flag * HZ;

            if ( str[i] )
            {
                flag = 0;
                for ( i++; str[i]; i++ )
                {
                    if ( str[i] >= '0' && str[i] <= '9' )
                        flag = flag * 10 + str[i] - '0';
                    else
                        break;
                }
                if ( flag < 1518 )
                    size = flag;
            }
        }

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
        {
            skb[i] = dev_alloc_skb(1536);
            if ( !skb[i] )
            {
                printk("failed in allocating skb\n");

                while ( --i >= 0 )
                    dev_kfree_skb_any(skb[i]);
                MOD_DEC_USE_COUNT;
                return count;
            }
            skb_reserve(skb[i], CPU1_PACKET_RESERVE);
            skb_put(skb[i], size);
        }

        printk("txr: going to test %lu seconds with %dbytes length packet (skb[%d])\n", (time_jiffies + HZ / 2) / HZ, size, sizeof(skb) / sizeof(*skb));

        end = jiffies + time_jiffies;
        while ( jiffies < end )
        {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
            counter = (unsigned int)g_tx_count_sem.count.counter;
#else
            counter = g_tx_count_sem.count;
#endif

            if (counter >= TX_DESC_NUM )
            {
                my_yield();
                continue;
            }

            i = count % (sizeof(skb) / sizeof(*skb));
            cpu1_hard_start_xmit(skb[i], NULL);
            skb[i] = dev_alloc_skb(1536);
            if ( !skb[i] )
            {
                printk("failed in allocating skb\n");

                while ( --i >= 0 )
                    dev_kfree_skb_any(skb[i]);
                MOD_DEC_USE_COUNT;
                return count;
            }
            skb_reserve(skb[i], CPU1_PACKET_RESERVE);
            skb_put(skb[i], size);

            count++;
        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        counter = (unsigned int)g_tx_count_sem.count.counter;
#else
        counter = g_tx_count_sem.count;
#endif

        count -= counter;

        printk("sent %d packets at %dbytes length\n", count, size);

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
            dev_kfree_skb_any(skb[i]);
    }
    else if ( stricmp(str, "txe") == 0 )
    {
        unsigned long time_jiffies = HZ;
        struct sk_buff *skb[64] = {0};
        int size = 1024;
        unsigned long end;
        struct tx_desc tx_desc;
        int count = 0;
        int i;

        if ( !flag )
        {
            for ( i = 4; str[i]; i++ )
            {
                if ( str[i] >= '0' && str[i] <= '9' )
                    flag = flag * 10 + str[i] - '0';
                else
                    break;
            }
            time_jiffies = (unsigned long)flag * HZ;

            if ( str[i] )
            {
                flag = 0;
                for ( i++; str[i]; i++ )
                {
                    if ( str[i] >= '0' && str[i] <= '9' )
                        flag = flag * 10 + str[i] - '0';
                    else
                        break;
                }
                if ( flag < 1518 )
                    size = flag;
            }
        }

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
        {
            skb[i] = dev_alloc_skb(1536);
            if ( !skb[i] )
            {
                printk("failed in allocating skb\n");

                while ( --i >= 0 )
                    dev_kfree_skb_any(skb[i]);
                MOD_DEC_USE_COUNT;
                return count;
            }
            skb_reserve(skb[i], CPU1_PACKET_RESERVE);
            skb_put(skb[i], size);
        }

        printk("txe: going to test %lu seconds with %dbytes length packet (skb[%d])\n", (time_jiffies + HZ / 2) / HZ, size, sizeof(skb) / sizeof(*skb));

        end = jiffies + time_jiffies;
        while ( jiffies < end )
        {
            tx_desc = *CPU1_TX_DESC(g_cpu1_tx_data_page_set);
            if ( !tx_desc.OWN )
            {
                my_yield();
                continue;
            }

            i = count % (sizeof(skb) / sizeof(*skb));

            my_memcpy((uint8_t *)CPU1_TX_DATA_PAGE(g_cpu1_tx_data_page_set) + CPU1_PACKET_RESERVE, skb[i]->data, skb[i]->len);
            ((uint8_t *)CPU1_TX_DATA_PAGE(g_cpu1_tx_data_page_set))[CPU1_INTERFACE_ID_OFFSET] = CPU1_INTERFACE_ID;
            tx_desc.data_length = (skb[i]->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH + RX_TAIL_CRC_LENGTH : skb[i]->len + RX_TAIL_CRC_LENGTH) + RX_TAIL_CRC_LENGTH;
            tx_desc.C = 0;
            tx_desc.OWN = 0;
            *CPU1_TX_DESC(g_cpu1_tx_data_page_set) = tx_desc;
            g_cpu1_mib.xmit_pkts++;
            g_cpu1_mib.xmit_bytes += skb[i]->len <= MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH : skb[i]->len;
            dev_kfree_skb_any(skb[i]);
            g_cpu1_tx_data_page_set ^= 0x01;

            skb[i] = dev_alloc_skb(1536);
            if ( !skb[i] )
            {
                printk("failed in allocating skb\n");

                while ( --i >= 0 )
                    dev_kfree_skb_any(skb[i]);
                MOD_DEC_USE_COUNT;
                return count;
            }
            skb_reserve(skb[i], CPU1_PACKET_RESERVE);
            skb_put(skb[i], size);

            count++;
        }
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        counter = (unsigned int)g_tx_count_sem.count.counter;
#else
        counter = g_tx_count_sem.count;
#endif

        count -= counter;

        printk("sent %d packets at %dbytes length\n", count, size);

        for ( i = 0; i < sizeof(skb) / sizeof(*skb); i++ )
            dev_kfree_skb_any(skb[i]);
    }
    else
    {
        if ( !flag )
            str[4] = ' ';

  #if defined(ENABLE_SINGLE_CORE_EMUL) && ENABLE_SINGLE_CORE_EMUL
        if ( stricmp(str, "loop on") == 0 )
        {
            g_loop = 1;
            ppa_lite_tx_full = ppa_lite_stop_rx;
            ppa_lite_tx_free = ppa_lite_start_rx;
        }
        else if ( stricmp(str, "loop off") == 0 )
        {
            ppa_lite_tx_full = NULL;
            ppa_lite_tx_free = NULL;
            g_loop = 0;
        }
  #endif
    }

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

static int proc_read_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    MOD_INC_USE_COUNT;

    len += sprintf(page + off + len,     "CPU1 MIB\n");
    len += sprintf(page + off + len,     "  rx_pkts:        %u\n", g_cpu1_mib.rx_pkts);
    len += sprintf(page + off + len,     "  rx_bytes:       %u\n", g_cpu1_mib.rx_bytes);
    len += sprintf(page + off + len,     "  rx_drop_pkts:   %u\n", g_cpu1_mib.rx_drop_pkts);
    len += sprintf(page + off + len,     "  rx_drop_bytes:  %u\n", g_cpu1_mib.rx_drop_bytes);
    len += sprintf(page + off + len,     "  tx_pkts:        %u\n", g_cpu1_mib.tx_pkts);
    len += sprintf(page + off + len,     "  tx_bytes:       %u\n", g_cpu1_mib.tx_bytes);
    len += sprintf(page + off + len,     "  tx_drop_pkts:   %u\n", g_cpu1_mib.tx_drop_pkts);
    len += sprintf(page + off + len,     "  tx_drop_bytes:  %u\n", g_cpu1_mib.tx_drop_bytes);
    len += sprintf(page + off + len,     "  xmit_pkts:      %u\n", g_cpu1_mib.xmit_pkts);
    len += sprintf(page + off + len,     "  xmit_bytes:     %u\n", g_cpu1_mib.xmit_bytes);

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[64];
    char *p;
    int len, rlen;
    u32 eth_clear;

    MOD_INC_USE_COUNT;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        MOD_DEC_USE_COUNT;
        return 0;
    }

    eth_clear = 0;
    if ( stricmp(p, "clear") == 0 || stricmp(p, "clear all") == 0
        || stricmp(p, "clean") == 0 || stricmp(p, "clean all") == 0 )
        memset(&g_cpu1_mib, 0, sizeof(g_cpu1_mib));

    MOD_DEC_USE_COUNT;

    return count;
}

#if (defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC) || (defined(ENABLE_TEMP_TEST_PROC) && ENABLE_TEMP_TEST_PROC)
static INLINE int stricmp(const char *p1, const char *p2)
{
    int c1, c2;

    while ( *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
    }

    return *p1 - *p2;
}
#endif

#if defined(ENABLE_MY_MEMCPY) && ENABLE_MY_MEMCPY
static INLINE void *my_memcpy(unsigned char *dest, const unsigned char *src, unsigned int count)
{
    char *d = (char *)dest, *s = (char *)src;

    if (count >= 32) {
        int i = 8 - (((unsigned long) d) & 0x7);

        if (i != 8)
            while (i-- && count--) {
                *d++ = *s++;
            }

        if (((((unsigned long) d) & 0x7) == 0) &&
                ((((unsigned long) s) & 0x7) == 0)) {
            while (count >= 32) {
                unsigned long long t1, t2, t3, t4;
                t1 = *(unsigned long long *) (s);
                t2 = *(unsigned long long *) (s + 8);
                t3 = *(unsigned long long *) (s + 16);
                t4 = *(unsigned long long *) (s + 24);
                *(unsigned long long *) (d) = t1;
                *(unsigned long long *) (d + 8) = t2;
                *(unsigned long long *) (d + 16) = t3;
                *(unsigned long long *) (d + 24) = t4;
                d += 32;
                s += 32;
                count -= 32;
            }
            while (count >= 8) {
                *(unsigned long long *) d =
                                            *(unsigned long long *) s;
                d += 8;
                s += 8;
                count -= 8;
            }
        }

        if (((((unsigned long) d) & 0x3) == 0) &&
                ((((unsigned long) s) & 0x3) == 0)) {
            while (count >= 4) {
                *(unsigned long *) d = *(unsigned long *) s;
                d += 4;
                s += 4;
                count -= 4;
            }
        }

        if (((((unsigned long) d) & 0x1) == 0) &&
                ((((unsigned long) s) & 0x1) == 0)) {
            while (count >= 2) {
                *(unsigned short *) d = *(unsigned short *) s;
                d += 2;
                s += 2;
                count -= 2;
            }
        }
    }

    while (count--) {
        *d++ = *s++;
    }

//    return d;
}
#endif

static INLINE void dump_skb(struct sk_buff *skb, u32 len, char *title, int is_tx)
{
#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
    int i;

  #if !defined(DEBUG_DUMP_SKB_RX) || !DEBUG_DUMP_SKB_RX
    if ( !is_tx )
        return;
  #endif

  #if !defined(DEBUG_DUMP_SKB_TX) || !DEBUG_DUMP_SKB_TX
    if ( is_tx )
        return;
  #endif

    if ( !g_dbg_enable )
        return;

    if ( skb->len < len )
        len = skb->len;

    if ( len > 1536 )
    {
        printk("too big data length: skb = %08x, skb->data = %08x, skb->len = %d\n", (u32)skb, (u32)skb->data, skb->len);
        return;
    }

    printk("%s\n", title);
    printk("  skb->data = %08X, skb->head = %08x, skb->tail = %08X, skb->len = %d\n", (u32)skb->data, (u32)skb->head, (u32)skb->tail, (int)skb->len);
    for ( i = 1; i <= len; i++ )
    {
        if ( i % 16 == 1 )
            printk("  %4d:", i - 1);
        printk(" %02X", (int)(*((char*)skb->data + i - 1) & 0xFF));
        if ( i % 16 == 0 )
            printk("\n");
    }
    if ( (i - 1) % 16 != 0 )
        printk("\n");
#endif
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

int __init danube_ppa_lite_eth_d4_init(void)
{
    struct tx_desc tx_desc = {
        OWN:                    1,
        C:                      1,
        SoP:                    1,
        SoP:                    1,
        Burst_length_offset:    CPU1_PACKET_RESERVE / 4,
        byte_offset:            CPU1_PACKET_RESERVE & 0x03,
        reserve:                0,
        data_length:            0,
        data_pointer:           0
        };
    int i;

    printk("Loading D4 Lite driver ...... ");

    for ( i = 0; i < DMA_RX_CH3_DESC_NUM; i++ )
    {
        g_rx_skb[i] = dev_alloc_skb(DMA_PACKET_SIZE + 16);
#if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
        if ( g_rx_skb[i] )
            g_skb_balance_count++;
#endif
        if ( !g_rx_skb[i] )
        {
            err("failed in allocating sk_buff");
            goto DEV_ALLOC_SKB_FAILED;
        }
        if ( ((u32)g_rx_skb[i]->data & 0x0F) != 0 )
            skb_reserve(g_rx_skb[i], ~((u32)g_rx_skb[i]->data + 0x0F) & 0x0F);
#ifndef CONFIG_MIPS_UNCACHED
        dma_cache_inv((unsigned long)g_rx_skb[i]->data, DMA_PACKET_SIZE);
#endif
        g_rx_skb_data[i] = g_rx_skb[i]->data;
    }

#ifndef CONFIG_MIPS_UNCACHED
    dma_cache_wback_inv((unsigned long)g_rx_skb_data, sizeof(g_rx_skb_data));
#endif
    ifx_mps_ppa_init_sync((u32)g_rx_skb_data);
    while ( 1 )
    {
#ifndef CONFIG_MIPS_UNCACHED
        dma_cache_inv((unsigned long)&g_rx_skb_data[DMA_RX_CH3_DESC_NUM], sizeof(g_rx_skb_data[DMA_RX_CH3_DESC_NUM]));
#endif
        if ( g_rx_skb_data[DMA_RX_CH3_DESC_NUM] )
            break;
        schedule();
    }

    ((uint8_t *)CPU1_TX_DATA_PAGE(0))[CPU1_INTERFACE_ID_OFFSET] = CPU1_INTERFACE_ID;
    tx_desc.data_pointer = (uint32_t)CPU1_TX_DATA_PAGE(0);
    *CPU1_TX_DESC(0) = tx_desc;

    ((uint8_t *)CPU1_TX_DATA_PAGE(1))[CPU1_INTERFACE_ID_OFFSET] = CPU1_INTERFACE_ID;
    tx_desc.data_pointer = (uint32_t)CPU1_TX_DATA_PAGE(1);
    *CPU1_TX_DESC(1) = tx_desc;

    //  hook netif_rx
//#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
//    g_netif_rx = mps_netif_receive_skb_fn;
//#else
//    g_netif_rx = mps_netif_receive_skb;
//#endif

    *MBOX_IGU0_ISRC = 0xFFFFFFFF;
    *MBOX_IGU0_IER = 0x01;
    if ( request_irq(PPE_MAILBOX_IGU0_INT, mailbox_irq_handler, IRQF_DISABLED, "cpu1_ema_isr", NULL) )
    {
        err("failed in requesting irq");
        goto REQUEST_IRQ_FAILED;
    }

    sema_init(&g_tx_count_sem, 0);
    if ( kernel_thread(cpu1_tx_d, NULL, CLONE_FS | CLONE_FILES /* | CLONE_SIGNAL */) < 0 )
    {
        err("failed in creating TX thread");
        goto KERNEL_THREAD_FAILED;
    }

    proc_file_create();

    printk("Succeeded!\n");

    return 0;

KERNEL_THREAD_FAILED:
    free_irq(PPE_MAILBOX_IGU0_INT, NULL);
REQUEST_IRQ_FAILED:
//    i = DMA_RX_CH3_DESC_NUM;
DEV_ALLOC_SKB_FAILED:
    while ( i-- )
    {
#if defined(ENABLE_SKB_BALANCE_COUNT) && ENABLE_SKB_BALANCE_COUNT
        g_skb_balance_count--;
#endif
        dev_kfree_skb_any(g_rx_skb[i]);
    }
    return -1;
}

void __exit danube_ppa_lite_eth_d4_exit(void)
{
    proc_file_delete();
}

module_init(danube_ppa_lite_eth_d4_init);
module_exit(danube_ppa_lite_eth_d4_exit);

EXPORT_SYMBOL(cpu1_hard_start_xmit);
#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
  EXPORT_SYMBOL(g_netif_rx);
#endif
EXPORT_SYMBOL(ppa_lite_stop_rx);
EXPORT_SYMBOL(ppa_lite_start_rx);
EXPORT_SYMBOL(ppa_lite_tx_full);
EXPORT_SYMBOL(ppa_lite_tx_free);
