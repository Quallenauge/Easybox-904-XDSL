/******************************************************************************
**
** FILE NAME    : ifxmips_ppa_datapath_amazon_se_a4.c
** PROJECT      : UEIP
** MODULES      : ATM + MII0 Acceleration Package (Amazon-SE PPA A4)
**
** DATE         : 17 SEP 2007
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM + MII0 Driver with Acceleration Firmware (A4)
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
** 17 SEP 2007  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */
#define VER_FAMILY      0x08        //  bit 0: res
                                    //      1: Danube
                                    //      2: Twinpass
                                    //      3: Amazon-SE
                                    //      4: res
                                    //      5: AR9
                                    //      6: GR9
#define VER_DRTYPE      0x03        //  bit 0: Normal Data Path driver
                                    //      1: Indirect-Fast Path driver
                                    //      2: HAL driver
                                    //      3: Hook driver
                                    //      4: Stack/System Adaption Layer driver
                                    //      5: PPA API driver
#define VER_INTERFACE   0x05        //  bit 0: MII 0
                                    //      1: MII 1
                                    //      2: ATM WAN
                                    //      3: PTM WAN
#define VER_ACCMODE     0x01        //  bit 0: Routing
                                    //      1: Bridging
#define VER_MAJOR       0
#define VER_MID         1
#define VER_MINOR       1



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
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
#include <net/xfrm.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_rcu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_led.h>
#ifdef CONFIG_IFX_ETH_FRAMEWORK
  #include <asm/ifx/ifx_eth_framework.h>
#endif
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_stack_al.h>
#include <net/ifx_ppa_api_directpath.h>
#include "../../ifx_ppa_datapath.h"
#include "ifxmips_ppa_datapath_fw_amazon_se_a4.h"



/*
 * ####################################
 *        OS Versionn Dependent
 * ####################################
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
  #define MOD_INC_USE_COUNT
  #define MOD_DEC_USE_COUNT
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32)
  static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
  {
    skb->tail = skb->data + offset;
  }
#endif



/*
 * ####################################
 *   Parameters to Configure PPE
 * ####################################
 */
static int port_cell_rate_up[2] = {3200, 3200}; /*  Maximum TX cell rate for ports                  */

static int qsb_tau   = 1;                       /*  QSB cell delay variation due to concurrency     */
static int qsb_srvm  = 0x0F;                    /*  QSB scheduler burst length                      */
static int qsb_tstep = 4 ;                      /*  QSB time step, all legal values are 1, 2, 4     */

static int write_descriptor_delay  = 0x20;      /*  Write descriptor delay                          */

static int aal5r_max_packet_size   = 0x0630;    /*  Max frame size for RX                           */
static int aal5r_min_packet_size   = 0x0000;    /*  Min frame size for RX                           */
static int aal5s_max_packet_size   = 0x0630;    /*  Max frame size for TX                           */

static int ethwan = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
  #define MODULE_PARM_ARRAY(a, b)   module_param_array(a, int, NULL, 0)
  #define MODULE_PARM(a, b)         module_param(a, int, 0)
#else
  #define MODULE_PARM_ARRAY(a, b)   MODULE_PARM(a, b)
#endif

MODULE_PARM_ARRAY(port_cell_rate_up, "2-2i");
MODULE_PARM_DESC(port_cell_rate_up, "ATM port upstream rate in cells/s");

MODULE_PARM(qsb_tau,"i");
MODULE_PARM_DESC(qsb_tau, "Cell delay variation. Value must be > 0");
MODULE_PARM(qsb_srvm, "i");
MODULE_PARM_DESC(qsb_srvm, "Maximum burst size");
MODULE_PARM(qsb_tstep, "i");
MODULE_PARM_DESC(qsb_tstep, "n*32 cycles per sbs cycles n=1,2,4");

MODULE_PARM(write_descriptor_delay, "i" );
MODULE_PARM_DESC(write_descriptor_delay, "PPE core clock cycles between descriptor write and effectiveness in external RAM");

MODULE_PARM(aal5r_max_packet_size, "i");
MODULE_PARM_DESC(aal5r_max_packet_size, "Max packet size in byte for downstream AAL5 frames");
MODULE_PARM(aal5r_min_packet_size, "i");
MODULE_PARM_DESC(aal5r_min_packet_size, "Min packet size in byte for downstream AAL5 frames");
MODULE_PARM(aal5s_max_packet_size, "i");
MODULE_PARM_DESC(aal5s_max_packet_size, "Max packet size in byte for upstream AAL5 frames");

MODULE_PARM(ethwan, "i");
MODULE_PARM_DESC(ethwan, "WAN mode, 1 - ETH0 mixed, 0 - DSL WAN.");



/*
 * ####################################
 *              Board Type
 * ####################################
 */

#define BOARD_DANUBE                            0x01
#define BOARD_TWINPATH_E                        0x02
#define BOARD_TWINPATH_VE                       0x03
#define BOARD_AMAZON_SE                         0x11



/*
 * ####################################
 *              Definition
 * ####################################
 */

#define BOARD_CONFIG                            BOARD_AMAZON_SE

#define DEBUG_SKB_SWAP                          0   //  debug skb swap in all queues (ETH/DSL RX/TX path)

#define ENABLE_BURST_WORKAROUND                 1

#define ENABLE_LED_FRAMEWORK                    1

#define ENABLE_NO_DELAY_ATM_SKB_FREE            1

#define ENABLE_USB_WLAN_SUPPORT                 1

#define ENABLE_DIRECTPATH_TX_QUEUE              1
#define ENABLE_DIRECTPATH_TX_QUEUE_SIZE         (1524 * 50)

#define ENABLE_STATS_ON_VCC_BASIS               1

#define ENABLE_DFE_LOOPBACK_TEST                0

#define DISABLE_VBR                             0

#define ENABLE_MY_MEMCPY                        0

#define ENABLE_SKB_SWAP_DBG                     0   //  debug skb swap in DSL TX path

#define ENABLE_DEBUG                            1

#define ENABLE_ASSERT                           1

#define DEBUG_QOS                               1

#define DEBUG_DUMP_SKB                          1

#define DEBUG_DUMP_FLAG_HEADER                  1

#define DEBUG_DUMP_INIT                         1

#define DEBUG_FIRMWARE_TABLES_PROC              1

#define DEBUG_HTU_PROC                          1

#define DEBUG_MEM_PROC                          1

#define DEBUG_PP32_PROC                         1

#define DEBUG_FW_PROC                           1

#define DEBUG_SEND_PROC                         1

#define ENABLE_PROBE_TRANSCEIVER                0

#define ENABLE_HW_FLOWCONTROL                   1

#define PPE_MAILBOX_IGU1_INT                    INT_NUM_IM2_IRL13

#define MY_ETH0_ADDR                            g_my_ethaddr

#if defined(CONFIG_DSL_MEI_CPE_DRV) && !defined(CONFIG_IFXMIPS_DSL_CPE_MEI)
  #define CONFIG_IFXMIPS_DSL_CPE_MEI            1
#endif

//  default board related configuration
#define ENABLE_AMAZON_SE_BOARD                  1
#if defined(CONFIG_RED_MII_MAC_MODE)
  #define MII0_MODE_SETUP                       RED_MII_MODE
#elif defined(CONFIG_EPHY_MODE)
  #define MII0_MODE_SETUP                       EPHY_MODE
#else
  #define MII0_MODE_SETUP                       EPHY_MODE
#endif
#define EXTERNAL_EPHY_CLOCK                     0

#ifndef CONFIG_MTD_SPI_FLASH
  #define RED_MII_MUX_WITH_SPI                  1
#endif

#define SET_CLASS_A_VALUE                       1

#if !defined(CONFIG_NET_HW_FLOWCONTROL)
  #undef  ENABLE_HW_FLOWCONTROL
  #define ENABLE_HW_FLOWCONTROL                 0
#endif

#if defined(ENABLE_STATS_ON_VCC_BASIS) && ENABLE_STATS_ON_VCC_BASIS
  #define UPDATE_VCC_STAT(conn, field, num)     do { g_atm_priv_data.connection[conn].field += (num); } while (0)
#else
  #define UPDATE_VCC_STAT(conn, field, num)
#endif

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

#define err(format, arg...)                     do { if ( (g_dbg_enable & DBG_ENABLE_MASK_ERR) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
  #undef  dbg
  #define dbg(format, arg...)                   do { if ( (g_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT) ) printk(KERN_WARNING __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #if !defined(dbg)
    #define dbg(format, arg...)
  #endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
  #define ASSERT(cond, format, arg...)          do { if ( (g_dbg_enable & DBG_ENABLE_MASK_ASSERT) && !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #define ASSERT(cond, format, arg...)
#endif

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
  #define softq_dbg(bit, format, arg...)        do { if ( (g_softq_dbg_enable & (1 << bit)) ) printk(KERN_DEBUG "%d: " format, bit, ##arg); } while ( 0 )
#endif

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
  #define DUMP_SKB_LEN                          ~0
#endif

#if (defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB)                     \
    || (defined(DEBUG_DUMP_FLAG_HEADER) && DEBUG_DUMP_FLAG_HEADER)  \
    || (defined(DEBUG_DUMP_INIT) && DEBUG_DUMP_INIT)                \
    || (defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT)          \
    || (defined(ENABLE_ASSERT) && ENABLE_ASSERT)
  #define ENABLE_DBG_PROC                       1
#else
  #define ENABLE_DBG_PROC                       0
#endif

/*
 *  Debug Print Mask
 */
#define DBG_ENABLE_MASK_ERR                     (1 << 0)
#define DBG_ENABLE_MASK_DEBUG_PRINT             (1 << 1)
#define DBG_ENABLE_MASK_ASSERT                  (1 << 2)
#define DBG_ENABLE_MASK_DUMP_SKB_RX             (1 << 8)
#define DBG_ENABLE_MASK_DUMP_SKB_TX             (1 << 9)
#define DBG_ENABLE_MASK_DUMP_FLAG_HEADER        (1 << 10)
#define DBG_ENABLE_MASK_DUMP_INIT               (1 << 11)
#define DBG_ENABLE_MASK_DUMP_QOS                (1 << 12)
#define DBG_ENABLE_MASK_ALL                     (DBG_ENABLE_MASK_ERR | DBG_ENABLE_MASK_DEBUG_PRINT | DBG_ENABLE_MASK_ASSERT \
                                                | DBG_ENABLE_MASK_DUMP_SKB_RX | DBG_ENABLE_MASK_DUMP_SKB_TX                 \
                                                | DBG_ENABLE_MASK_DUMP_FLAG_HEADER | DBG_ENABLE_MASK_DUMP_INIT | DBG_ENABLE_MASK_DUMP_QOS)

/*
 *  Mailbox Signal Bit
 */
#define CPU_TO_WAN_TX_SIG                       (1 << 30)
#define WAN_TX_SIG_NUM(i)                       (16 + (i))                  //  i < 8
#define WAN_TX_SIG(i)                           (1 << WAN_TX_SIG_NUM(i))    //  i < 8
#define WAN_RX_SIG(i)                           (1 << (i))                  //  i < 2, 0: AAL5, 1: OAM
#define DMA_TX_CH1_SIG                          (1 << 31)
#define WLAN_CPU_TX_SIG                         (1 << 25)

/*
 *  Eth Mode
 */
#define MII_MODE                                1
#define REV_MII_MODE                            2
#define RED_MII_MODE                            3
#define EPHY_MODE                               4

/*
 *  Constant Definition
 */
#define ETH_WATCHDOG_TIMEOUT                    (10 * HZ)
#define ETOP_MDIO_DELAY                         1
#define IDLE_CYCLE_NUMBER                       30000

#define DMA_PACKET_SIZE                         1568
#define DMA_ALIGNMENT                           16

#define FWCODE_ROUTING_ACC_D2                   0x02
#define FWCODE_BRIDGING_ACC_D3                  0x03
#define FWCODE_ROUTING_BRIDGING_ACC_D4          0x04
#define FWCODE_ROUTING_BRIDGING_ACC_A4          0x14

/*
 *  ATM Cell
 */
#define CELL_SIZE                               ATM_AAL0_SDU

/*
 *  Ethernet Frame Definitions
 */
#define ETH_CRC_LENGTH                          4
#define ETH_MAX_DATA_LENGTH                     ETH_DATA_LEN
#define ETH_MIN_TX_PACKET_LENGTH                ETH_ZLEN

/*
 *  ATM Port, QSB Queue, DMA RX/TX Channel Parameters
 */
#define ATM_PORT_NUMBER                         1
#define ATM_QUEUE_NUMBER                        8
#define ATM_SW_TX_QUEUE_NUMBER                  ATM_QUEUE_NUMBER
#define QSB_QUEUE_NUMBER_BASE                   1

#define DEFAULT_RX_HUNT_BITTH                   4

/*
 *  QSB Queue Scheduling and Shaping Definitions
 */
#define QSB_WFQ_NONUBR_MAX                      0x3f00
#define QSB_WFQ_UBR_BYPASS                      0x3fff
#define QSB_TP_TS_MAX                           65472
#define QSB_TAUS_MAX                            64512
#define QSB_GCR_MIN                             18

/*
 *  QSB Command Set
 */
#define QSB_RAMAC_RW_READ                       0
#define QSB_RAMAC_RW_WRITE                      1

#define QSB_RAMAC_TSEL_QPT                      0x01
#define QSB_RAMAC_TSEL_SCT                      0x02
#define QSB_RAMAC_TSEL_SPT                      0x03
#define QSB_RAMAC_TSEL_VBR                      0x08

#define QSB_RAMAC_LH_LOW                        0
#define QSB_RAMAC_LH_HIGH                       1

#define QSB_QPT_SET_MASK                        0x0
#define QSB_QVPT_SET_MASK                       0x0
#define QSB_SET_SCT_MASK                        0x0
#define QSB_SET_SPT_MASK                        0x0
#define QSB_SET_SPT_SBVALID_MASK                0x7FFFFFFF

#define QSB_SPT_SBV_VALID                       (1 << 31)
#define QSB_SPT_PN_SET(value)                   (((value) & 0x01) ? (1 << 16) : 0)
#define QSB_SPT_INTRATE_SET(value)              SET_BITS(0, 13, 0, value)

/*
 *  OAM Definitions
 */
#define OAM_HTU_ENTRY_NUMBER                    3
#define OAM_F4_SEG_HTU_ENTRY                    0
#define OAM_F4_TOT_HTU_ENTRY                    1
#define OAM_F5_HTU_ENTRY                        2
#define OAM_F4_CELL_ID                          0
#define OAM_F5_CELL_ID                          15

/*
 *  EMA Settings
 */
#define EMA_CMD_BUF_LEN                         0x0010
#define EMA_CMD_BASE_ADDR                       (0x0930 << 2)
#define EMA_DATA_BUF_LEN                        0x0040
#define EMA_DATA_BASE_ADDR                      (0x0C00 << 2)
#define EMA_WRITE_BURST                         0x02
#define EMA_READ_BURST                          0x02

/*
 *  Firmware Settings
 */
#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
  #define ROUT_NUM                              88                          //  Routing + Bridging: Max 88
#else
  #define ROUT_NUM                              95                          //  Routing + Bridging: Max 95
#endif
#define WAN_ROUT_NUM                            (ROUT_NUM / 2)
#define LAN_ROUT_NUM                            (ROUT_NUM - WAN_ROUT_NUM)
#define LAN_ROUT_OFF                            WAN_ROUT_NUM

#define WAN_ROUT_MC_NUM                         32

#define BRIDGING_ENTRY_NUM                      128                         //  Routing + Bridging: Max 128

#define CPU_TO_WAN_TX_DESC_NUM                  16
#define WAN_TX_DESC_NUM(i)                      24
#define WAN_RX_DESC_NUM(i)                      16
#define DMA_RX_CH1_DESC_NUM                     16
#define DMA_RX_CH2_DESC_NUM                     16                          //  allocate in this driver
//#define DMA_TX_CH2_DESC_NUM                     16
#define DMA_TX_CH1_DESC_NUM                     DMA_RX_CH2_DESC_NUM
#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
  #define WLAN_CPU_TX_DESC_NUM                  16
  #define PRE_ALLOC_DESC_TOTAL_NUM              (CPU_TO_WAN_TX_DESC_NUM + WAN_TX_DESC_NUM(0) * 8                        \
                                                + WAN_RX_DESC_NUM(0) * 2 + DMA_RX_CH1_DESC_NUM                          \
                                                + DMA_RX_CH2_DESC_NUM + WLAN_CPU_TX_DESC_NUM)
#else
  #define PRE_ALLOC_DESC_TOTAL_NUM              (CPU_TO_WAN_TX_DESC_NUM + WAN_TX_DESC_NUM(0) * 8                        \
                                                + WAN_RX_DESC_NUM(0) * 2 + DMA_RX_CH1_DESC_NUM                          \
                                                + DMA_RX_CH2_DESC_NUM)
#endif

#define IFX_PPA_PORT_NUM                        8

#define MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES        8
#define MAX_SRC_IP_VLAN_MAP_ENTRIES             4
#define MAX_ETHTYPE_VLAN_MAP_ENTRIES            4
#define MAX_VLAN_VLAN_MAP_ENTRIES               8

/*
 *  Bits Operation
 */
//#define GET_BITS(x, msb, lsb)                   (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
//#define SET_BITS(x, msb, lsb, value)            (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

/*
 *  FPI Configuration Bus Register and Memory Address Mapping
 */
#define AMAZON_SE_PPE                   (KSEG1 | 0x1E180000)
#define PP32_DEBUG_REG_ADDR(x)          ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x0000) << 2)))
#define PPM_INT_REG_ADDR(x)             ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x0030) << 2)))
#define PP32_INTERNAL_RES_ADDR(x)       ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x0040) << 2)))
//#define PPE_CLOCK_CONTROL_ADDR(x)       ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x0100) << 2)))
#define CDM_CODE_MEMORY_RAM0_ADDR(x)    ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x1000) << 2)))
#define CDM_CODE_MEMORY_RAM1_ADDR(x)    ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x2000) << 2)))
#define PPE_REG_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x4000) << 2)))
#define PP32_DATA_MEMORY_RAM1_ADDR(x)   ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x5000) << 2)))
#define PPM_INT_UNIT_ADDR(x)            ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x6000) << 2)))
#define PPM_TIMER0_ADDR(x)              ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x6100) << 2)))
#define PPM_TASK_IND_REG_ADDR(x)        ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x6200) << 2)))
#define PPS_BRK_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x6300) << 2)))
#define PPM_TIMER1_ADDR(x)              ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x6400) << 2)))
//#define SB_RAM0_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x8000) << 2)))
//#define SB_RAM1_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x8400) << 2)))
//#define SB_RAM2_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x8C00) << 2)))
//#define SB_RAM3_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x9600) << 2)))
#define SB_RAM0_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x8200) << 2)))
#define SB_RAM1_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x8C00) << 2)))
//#define SB_RAM2_ADDR(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0x9600) << 2)))
#define QSB_CONF_REG(x)                 ((volatile u32*)(AMAZON_SE_PPE + (((x) + 0xC000) << 2)))

/*
 *  DWORD-Length of Memory Blocks
 */
#define PP32_DEBUG_REG_DWLEN            0x0030
#define PPM_INT_REG_DWLEN               0x0010
#define PP32_INTERNAL_RES_DWLEN         0x00C0
//#define PPE_CLOCK_CONTROL_DWLEN         0x0F00
#define CDM_CODE_MEMORY_RAM0_DWLEN      0x1000
#define CDM_CODE_MEMORY_RAM1_DWLEN      0x0800
#define PPE_REG_DWLEN                   0x1000
#define PP32_DATA_MEMORY_RAM1_DWLEN     0x0800
#define PPM_INT_UNIT_DWLEN              0x0100
#define PPM_TIMER0_DWLEN                0x0100
#define PPM_TASK_IND_REG_DWLEN          0x0100
#define PPS_BRK_DWLEN                   0x0100
#define PPM_TIMER1_DWLEN                0x0100
//#define SB_RAM0_DWLEN                   0x0400
//#define SB_RAM1_DWLEN                   0x0800
//#define SB_RAM2_DWLEN                   0x0A00
//#define SB_RAM3_DWLEN                   0x0400
#define SB_RAM0_DWLEN                   0x0A00
#define SB_RAM1_DWLEN                   0x0A00
//#define SB_RAM2_DWLEN                   0x1000
#define QSB_CONF_REG_DWLEN              0x0100

/*
 *  Host-PPE Communication Data Address Mapping
 */
//  TODO: SB_RAM2, SB_RAM3 is not available in Amazon_SE
#define SB_BUFFER(__sb_addr)    ( (volatile u32 *) ( ( ( (__sb_addr) >= 0x0000 ) && ( (__sb_addr) <= 0x0FFF ) )  ?  PPE_REG_ADDR(__sb_addr) :                           \
                                                     ( ( (__sb_addr) >= 0x1000 ) && ( (__sb_addr) <= 0x17FF ) )  ?  CDM_CODE_MEMORY_RAM1_ADDR((__sb_addr) - 0x1000) :   \
                                                     ( ( (__sb_addr) >= 0x2200 ) && ( (__sb_addr) <= 0x2BFF ) )  ?  SB_RAM0_ADDR((__sb_addr) - 0x2200) :                \
                                                     ( ( (__sb_addr) >= 0x2C00 ) && ( (__sb_addr) <= 0x35FF ) )  ?  SB_RAM1_ADDR((__sb_addr) - 0x2C00) :                \
                                                 0 ) )

#define FW_VER_ID                               ((volatile struct fw_ver_id *)SB_BUFFER(0x2401))

#define CFG_WRX_HTUTS                           SB_BUFFER(0x2A00)   /*  WAN RX HTU Table Size, must be configured before enable PPE firmware.               */
#define CFG_WRDES_DELAY                         SB_BUFFER(0x2A04)   /*  WAN Descriptor Write Delay, must be configured before enable PPE firmware.          */
#define WRX_EMACH_ON                            SB_BUFFER(0x2A05)   /*  WAN RX EMA Channel Enable (0 - 1), must be configured before enable PPE firmware.   */
#define WTX_EMACH_ON                            SB_BUFFER(0x2A06)   /*  WAN TX EMA Channel Enable (0 - 7), must be configured before enable PPE firmware.   */
#define WRX_HUNT_BITTH                          SB_BUFFER(0x2A07)   /*  WAN RX HUNT Threshold, must be between 2 to 8.                                      */

#define ETH_PORTS_CFG                           ((volatile struct eth_ports_cfg *)          SB_BUFFER(0x2A14))
#define LAN_ROUT_TBL_CFG                        ((volatile struct lan_rout_tbl_cfg *)       SB_BUFFER(0x2A16))
#define WAN_ROUT_TBL_CFG                        ((volatile struct wan_rout_tbl_cfg *)       SB_BUFFER(0x2A17))
#define GEN_MODE_CFG                            ((volatile struct gen_mode_cfg *)           SB_BUFFER(0x2A18))
#define BRG_TBL_CFG                             ((volatile struct brg_tbl_cfg*)             SB_BUFFER(0x2A19))
#define CFG_ROUT_MAC_NO                         SB_BUFFER(0x2A1A)
#define TX_QOS_CFG                              SB_BUFFER(0x2A1B)   //  not implemented yet
#define ETH_DEFAULT_DEST_LIST(i)                ((volatile struct eth_default_dest_list *)  SB_BUFFER(0x2A20 + (i)))        /*  i < 8   */

#define WRX_QUEUE_CONFIG(i)                     ((volatile struct wrx_queue_config *)       SB_BUFFER(0x2800 + (i) * 10))   /*  i < 8   */
#define WTX_PORT_CONFIG(i)                      ((volatile struct wtx_port_config *)        SB_BUFFER(0x2A7E + (i)))        /*  i < 2   */
#define WTX_QUEUE_CONFIG(i)                     ((volatile struct wtx_queue_config *)       SB_BUFFER(0x2850 + (i) * 24))   /*  i < 8   */

#define DSL_WAN_MIB_TABLE                       ((volatile struct dsl_wan_mib_table *)      SB_BUFFER(0x2AA0))
#define DSL_WAN_VC_MIB_TABLE(i)                 ((volatile struct dsl_wan_vc_mib_table *)   SB_BUFFER(0x2AB0 + (i) * 4))    /*  i < 8   */

#define WAN_MIB_TABLE                           DSL_WAN_MIB_TABLE

#define HTU_ENTRY(i)                            ((volatile struct htu_entry *)              SB_BUFFER(0x2C40 + (i)))        /*  i < 32  */
#define HTU_MASK(i)                             ((volatile struct htu_mask *)               SB_BUFFER(0x2C4C + (i)))        /*  i < 32  */
#define HTU_RESULT(i)                           ((volatile struct htu_result *)             SB_BUFFER(0x2C58 + (i)))        /*  i < 32  */

#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
  #define ITF_MIB_TBL(i)                        ((volatile struct itf_mib *)                SB_BUFFER(0x25C0 + (i) * 16))   /*  i < 8   */
#else
  #define ITF_MIB_TBL(i)                        ((volatile struct itf_mib *)                SB_BUFFER(0x25E0 + (i) * 16))   /*  i < 8   */
#endif

#define BRG_FWD_HIT_STAT_TBL(i)                 SB_BUFFER(0x2A68 + (i))         /*  i < 8   */
#define BRIDGING_FORWARD_TBL(i)                 SB_BUFFER(0x2DC4 + (i) * 3)     /*  i < 128 */

#define DEFAULT_ITF_VLAN_MAP(i)                 ((volatile struct default_itf_vlan_map *)   SB_BUFFER(0x2A30 + (i)))        /*  i < 8   */
#define SRC_IP_VLAN_MAP(i)                      ((volatile struct src_ip_vlan_map *)        SB_BUFFER(0x2A50 + (i) * 2))    /*  i < 4   */
#define ETHTYPE_VLAN_MAP(i)                     ((volatile struct ethtype_vlan_map *)       SB_BUFFER(0x2A38 + (i) * 2))    /*  i < 4   */
#define VLAN_VLAN_MAP(i)                        ((volatile struct vlan_vlan_map *)          SB_BUFFER(0x2340 + (i) * 3))    /*  i < 8   */

#define PPPOE_CFG_TBL(i)                        SB_BUFFER(0x2A58 + (i))         /*  i < 8   */
#define MTU_CFG_TBL(i)                          SB_BUFFER(0x2A60 + (i))         /*  i < 8   */
#define ROUT_MAC_CFG_TBL(i)                     SB_BUFFER(0x29E0 + (i) * 2)     /*  i < 16  */

#define ROUT_FWD_HIT_STAT_TBL(i)                SB_BUFFER(0x2A70 + (i))         /*  i < 8   */
#define ROUT_FORWARD_COMPARE_TBL(i)             ((volatile struct rout_forward_compare_tbl *)   SB_BUFFER(0x2CA4 + (i) * 3))    /*  i < 88/95 */
#define ROUT_FORWARD_ACTION_TBL(i)              ((volatile struct rout_forward_action_tbl *)    SB_BUFFER(0x2405 + (i) * 5))    /*  i < 88/95 */

#define WAN_ROUT_FORWARD_COMPARE_TBL(i)         ROUT_FORWARD_COMPARE_TBL(i)
#define LAN_ROUT_FORWARD_COMPARE_TBL(i)         ROUT_FORWARD_COMPARE_TBL(LAN_ROUT_TBL_CFG->lan_rout_off + (i))
#define WAN_ROUT_FORWARD_ACTION_TBL(i)          ROUT_FORWARD_ACTION_TBL(i)
#define LAN_ROUT_FORWARD_ACTION_TBL(i)          ROUT_FORWARD_ACTION_TBL(LAN_ROUT_TBL_CFG->lan_rout_off + (i))

#define MC_ROUT_FWD_HIT_STAT_TBL(i)             SB_BUFFER(0x2A78 + (i))         /*  i < 2   */
#define WAN_ROUT_MULTICAST_TBL(i)               ((volatile struct wan_rout_multicast_tbl *)     SB_BUFFER(0x2C64 + (i) * 2))    /*  i < 32  */
#define WAN_ROUT_MC_OUTER_VLAN_TBL(i)           ((volatile struct wan_rout_mc_outer_vlan_tbl *) SB_BUFFER(0x2958 + (i) / 4))    /*  i < 32  */

#define OUTER_VLAN_TBL(i)                       SB_BUFFER(0x27E0 + (i))         /*  i < 32  */

//#define VLAN_PRI_TO_WAN_TX_QID_MAPPING(i)       SB_BUFFER(0x29C0 + (i))         /*  i < 8   */

#define DSL_TX_QUEUE_MAPPING(i)                 SB_BUFFER(0x23F8 + (i))         /*  i < 8   */  //  MSB -> LSB = Prio No. 7 - 0

#define DSLWAN_TX_PKT_CNTx(i)                   SB_BUFFER(0x29B0 + (i)) //  i < 8

/*
 *  DMA/EMA Descriptor Base Address
 */
#define CPU_TO_WAN_TX_DESC_BASE                 ((volatile struct tx_descriptor *)SB_BUFFER(0x2960))                /*         16 each queue    */
#define WAN_TX_DESC_BASE(i)                     ((volatile struct tx_descriptor *)SB_BUFFER(0x2600 + (i) * 2 * 24)) /*  i < 8, 24 each queue    */
#define WAN_RX_DESC_BASE(i)                     ((volatile struct rx_descriptor *)SB_BUFFER(0x2780 + (i) * 2 * 16)) /*  i < 2, 16 each queue    */
#define DMA_RX_CH1_DESC_BASE                    ((volatile struct rx_descriptor *)SB_BUFFER(0x27C0))                /*         16 each queue    */
#define DMA_RX_CH2_DESC_BASE                    ((volatile struct rx_descriptor *)((u32)g_dma_rx_ch2_desc | KSEG1)) /*         16 each queue, allocate in this driver   */
#define DMA_TX_CH1_DESC_BASE                    ((volatile struct tx_descriptor *)DMA_RX_CH2_DESC_BASE)
#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
  #define WLAN_CPU_TX_DESC_BASE                 ((volatile struct tx_descriptor *)SB_BUFFER(0x2320))                /*         16 each queue    */
#endif

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
/*
 *  Firmware Proc
 */
  #define TIR(i)                                ((volatile u32 *)(0xBE198800) + (i))

  #define __CPU_TX_SWAPPER_DROP_PKTS            SB_BUFFER(0x29A8)
  #define __CPU_TX_SWAPPER_DROP_BYTES           SB_BUFFER(0x29A9)
  #define __DSLWAN_FP_SWAPPER_DROP_PKTS         SB_BUFFER(0x29AA)
  #define __DSLWAN_FP_SWAPPER_DROP_BYTES        SB_BUFFER(0x29AB)

  #define __CPU_TXDES_SWAP_RDPTR                SB_BUFFER(0x29AC)
  #define __DSLWAN_FP_RXDES_SWAP_RDPTR          SB_BUFFER(0x29AD)
  #define __DSLWAN_FP_RXDES_DPLUS_WRPTR         SB_BUFFER(0x29AE)

  #define __DSLWAN_TX_PKT_CNT0                  SB_BUFFER(0x29B0)
  #define __DSLWAN_TX_PKT_CNT1                  SB_BUFFER(0x29B1)
  #define __DSLWAN_TX_PKT_CNT2                  SB_BUFFER(0x29B2)
  #define __DSLWAN_TX_PKT_CNT3                  SB_BUFFER(0x29B3)
  #define __DSLWAN_TX_PKT_CNT4                  SB_BUFFER(0x29B4)
  #define __DSLWAN_TX_PKT_CNT5                  SB_BUFFER(0x29B5)
  #define __DSLWAN_TX_PKT_CNT6                  SB_BUFFER(0x29B6)
  #define __DSLWAN_TX_PKT_CNT7                  SB_BUFFER(0x29B7)

  #define __DSLWAN_TXDES_SWAP_PTR0              SB_BUFFER(0x29B8)
  #define __DSLWAN_TXDES_SWAP_PTR1              SB_BUFFER(0x29B9)
  #define __DSLWAN_TXDES_SWAP_PTR2              SB_BUFFER(0x29BA)
  #define __DSLWAN_TXDES_SWAP_PTR3              SB_BUFFER(0x29BB)
  #define __DSLWAN_TXDES_SWAP_PTR4              SB_BUFFER(0x29BC)
  #define __DSLWAN_TXDES_SWAP_PTR5              SB_BUFFER(0x29BD)
  #define __DSLWAN_TXDES_SWAP_PTR6              SB_BUFFER(0x29BE)
  #define __DSLWAN_TXDES_SWAP_PTR7              SB_BUFFER(0x29BF)

  // SB_BUFFER(0x29C0) - SB_BUFFER(0x29C7)
  #define __VLAN_PRI_TO_QID_MAPPING             SB_BUFFER(0x29C0)

  //
  // etx MIB: SB_BUFFER(0x29C8) - SB_BUFFER(0x29CF)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT0       SB_BUFFER(0x29C8)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT1       SB_BUFFER(0x29C9)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT2       SB_BUFFER(0x29CA)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT3       SB_BUFFER(0x29CB)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT4       SB_BUFFER(0x29CC)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT5       SB_BUFFER(0x29CD)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT6       SB_BUFFER(0x29CE)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT7       SB_BUFFER(0x29CF)

  #define __CPU_TO_DSLWAN_TX_PKTS               SB_BUFFER(0x29D0)
  #define __CPU_TO_DSLWAN_TX_BYTES              SB_BUFFER(0x29D1)

  #define ACC_ERX_PID                           SB_BUFFER(0x2B00)
  #define ACC_ERX_PORT_TIMES                    SB_BUFFER(0x2B01)
  #define SLL_ISSUED                            SB_BUFFER(0x2B02)
  #define BMC_ISSUED                            SB_BUFFER(0x2B03)
  #define DPLUS_RX_ON                           SB_BUFFER(0x2BBF)

  #define PRESEARCH_RDPTR                       SB_BUFFER(0x2B06)

  #define SLL_ERX_PID                           SB_BUFFER(0x2B04)
  #define SLL_PKT_CNT                           SB_BUFFER(0x2B08)
  #define SLL_RDPTR                             SB_BUFFER(0x2B0A)

  #define EDIT_PKT_CNT                          SB_BUFFER(0x2B0C)
  #define EDIT_RDPTR                            SB_BUFFER(0x2B0E)

  #define DPLUSRX_PKT_CNT                       SB_BUFFER(0x2B10)
  #define DPLUS_RDPTR                           SB_BUFFER(0x2B12)

  #define SLL_STATE_NULL                        0
  #define SLL_STATE_DA                          1
  #define SLL_STATE_SA                          2
  #define SLL_STATE_DA_BC                       3
  #define SLL_STATE_ROUTER                      4

  // DSL WAN MIB (total) - atm (0x2AA0 - 0x2AAF)
  //---------------------------------------------
  #define __WRX_DROPHTU_CELL                    SB_BUFFER(0x2AA1)
  #define __WRX_DROPDES_PDU                     SB_BUFFER(0x2AA2)
  #define __WRX_CORRECT_PDU                     SB_BUFFER(0x2AA3)
  #define __WRX_ERR_PDU                         SB_BUFFER(0x2AA4)
  #define __WRX_DROPDES_CELL                    SB_BUFFER(0x2AA5)
  #define __WRX_CORRECT_CELL                    SB_BUFFER(0x2AA6)
  #define __WRX_ERR_CELL                        SB_BUFFER(0x2AA7)
  #define __WRX_TOTAL_BYTE                      SB_BUFFER(0x2AA8)
  #define __WTX_TOTAL_PDU                       SB_BUFFER(0x2AAA)
  #define __WTX_TOTAL_CELL                      SB_BUFFER(0x2AAB)
  #define __WTX_TOTAL_BYTE                      SB_BUFFER(0x2AAC)
#endif

/*
 *  Share Buffer Registers
 */
#define SB_MST_PRI0                             PPE_REG_ADDR(0x0300)
#define SB_MST_PRI1                             PPE_REG_ADDR(0x0301)
#define SB_MST_SEL                              PPE_REG_ADDR(0x0304)

/*
 *    ETOP MDIO Registers
 */
#define ETOP_MDIO_CFG                           PPE_REG_ADDR(0x0600)
#define ETOP_MDIO_ACC                           PPE_REG_ADDR(0x0601)
#define ETOP_CFG                                PPE_REG_ADDR(0x0602)
#define ETOP_IG_VLAN_COS                        PPE_REG_ADDR(0x0603)
#define ETOP_IG_DSCP_COSx(x)                    PPE_REG_ADDR(0x0607 - ((x) & 0x03))
#define ETOP_IG_PLEN_CTRL0                      PPE_REG_ADDR(0x0608)
//#define ETOP_IG_PLEN_CTRL1                      PPE_REG_ADDR(0x0609)
#define ETOP_ISR                                PPE_REG_ADDR(0x060A)
#define ETOP_IER                                PPE_REG_ADDR(0x060B)
#define ETOP_VPID                               PPE_REG_ADDR(0x060C)
#define ENET_MAC_CFG(i)                         PPE_REG_ADDR(0x0610 + ((i) ? 0x40 : 0x00))
#define ENETS_DBA(i)                            PPE_REG_ADDR(0x0612 + ((i) ? 0x40 : 0x00))
#define ENETS_CBA(i)                            PPE_REG_ADDR(0x0613 + ((i) ? 0x40 : 0x00))
#define ENETS_CFG(i)                            PPE_REG_ADDR(0x0614 + ((i) ? 0x40 : 0x00))
#define ENETS_PGCNT(i)                          PPE_REG_ADDR(0x0615 + ((i) ? 0x40 : 0x00))
#define ENETS_PKTCNT(i)                         PPE_REG_ADDR(0x0616 + ((i) ? 0x40 : 0x00))
#define ENETS_BUF_CTRL(i)                       PPE_REG_ADDR(0x0617 + ((i) ? 0x40 : 0x00))
#define ENETS_COS_CFG(i)                        PPE_REG_ADDR(0x0618 + ((i) ? 0x40 : 0x00))
#define ENETS_IGDROP(i)                         PPE_REG_ADDR(0x0619 + ((i) ? 0x40 : 0x00))
#define ENETS_IGERR(i)                          PPE_REG_ADDR(0x061A + ((i) ? 0x40 : 0x00))
#define ENETS_MAC_DA0(i)                        PPE_REG_ADDR(0x061B + ((i) ? 0x40 : 0x00))
#define ENETS_MAC_DA1(i)                        PPE_REG_ADDR(0x061C + ((i) ? 0x40 : 0x00))
#define ENETF_DBA(i)                            PPE_REG_ADDR(0x0630 + ((i) ? 0x40 : 0x00))
#define ENETF_CBA(i)                            PPE_REG_ADDR(0x0631 + ((i) ? 0x40 : 0x00))
#define ENETF_CFG(i)                            PPE_REG_ADDR(0x0632 + ((i) ? 0x40 : 0x00))
#define ENETF_PGCNT(i)                          PPE_REG_ADDR(0x0633 + ((i) ? 0x40 : 0x00))
#define ENETF_PKTCNT(i)                         PPE_REG_ADDR(0x0634 + ((i) ? 0x40 : 0x00))
#define ENETF_HFCTRL(i)                         PPE_REG_ADDR(0x0635 + ((i) ? 0x40 : 0x00))
#define ENETF_TXCTRL(i)                         PPE_REG_ADDR(0x0636 + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS0(i)                         PPE_REG_ADDR(0x0638 + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS1(i)                         PPE_REG_ADDR(0x0639 + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS2(i)                         PPE_REG_ADDR(0x063A + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS3(i)                         PPE_REG_ADDR(0x063B + ((i) ? 0x40 : 0x00))
#define ENETF_EGCOL(i)                          PPE_REG_ADDR(0x063C + ((i) ? 0x40 : 0x00))
#define ENETF_EGDROP(i)                         PPE_REG_ADDR(0x063D + ((i) ? 0x40 : 0x00))

/*  ETOP_MDIO_ACCESS MASKS  */
#define EPHY_RA_MASK                            0x80000000
#define EPHY_RW_MASK                            0x40000000  //Mask

#define EPHY_WRITE_MASK                         0x00000000
#define READ_MASK                               0x40000000
#define EPHY_ADDRESS                            0x08
#define EPHY_ADDRESS_SHIFT                      21
#define EPHY_REG_ADDRESS_SHIFT                  16

#define PHY0_ADDR                               0

#define RA_READ_ENABLE                          0x03    //MDIO Request/Ack and Read
#define RA_WRITE_ENABLE                         0x02    //MDIO Req/Ack and Write

#define EPHY_ADDRESS_MASK                       0x1F
#define EPHY_REG_MASK                           0x1F
#define EPHY_DATA_MASK                          0xFFFF

#define EPHY_RESET                              0x8000
#define EPHY_AUTO_NEGOTIATION_ENABLE            0x1000
#define AUTO_NEGOTIATION_COMPLETE               0x20
#define EPHY_RESTART_AUTO_NEGOTIATION           0x200

#define EPHY_MDIO_BASE_CONTROL_REG              0x00    //PHY Control Register
#define EPHY_LINK_SPEED_MASK                    0x2000
#define EPHY_AUTO_NEG_ENABLE_MASK               0x1000
#define EPHY_DUPLEX_MASK                        0x0100

#define EPHY_MDIO_BASE_STATUS_REG               0x01    //PHY status register
#define EPHY_LINK_STATUS_MASK                   0x02

#define EPHY_MDIO_ADVERTISMENT_REG              0x04    //Auto Negotiation Advertisement Register
#define EPHY_MDIO_ADVERT_100_FD                 0x100
#define EPHY_MDIO_ADVERT_100_HD                 0x080
#define EPHY_MDIO_ADVERT_10_FD                  0x040
#define EPHY_MDIO_ADVERT_10_HD                  0x020
#define EPHY_MDIO_BC_NEGOTIATE                  0x0200

#define EPHY_SPECIFIC_STATUS_REG                0x17    //PHY specific status register
#define EPHY_SPECIFIC_STATUS_SPEED_MASK         0x20
#define EPHY_SPECIFIC_STATUS_DUPLEX_MASK        0x40
#define EPHY_SPECIFIC_STATUS_LINK_UP            0x10
#define LINK_UP                                 0x01
#define LINK_DOWN                               0x00

#define EPHY_SET_CLASS_VALUE_REG                0x12


/*
 *  DPlus Registers
 */
#define DPLUS_TXDB                              PPE_REG_ADDR(0x0700)
#define DPLUS_TXCB                              PPE_REG_ADDR(0x0701)
#define DPLUS_TXCFG                             PPE_REG_ADDR(0x0702)
#define DPLUS_TXPGCNT                           PPE_REG_ADDR(0x0703)
#define DPLUS_RXDB                              PPE_REG_ADDR(0x0710)
#define DPLUS_RXCB                              PPE_REG_ADDR(0x0711)
#define DPLUS_RXCFG                             PPE_REG_ADDR(0x0712)
#define DPLUS_RXPGCNT                           PPE_REG_ADDR(0x0713)

/*
 *  EMA Registers
 */
#define EMA_CMDCFG                              PPE_REG_ADDR(0x0A00)
#define EMA_DATACFG                             PPE_REG_ADDR(0x0A01)
#define EMA_CMDCNT                              PPE_REG_ADDR(0x0A02)
#define EMA_DATACNT                             PPE_REG_ADDR(0x0A03)
#define EMA_ISR                                 PPE_REG_ADDR(0x0A04)
#define EMA_IER                                 PPE_REG_ADDR(0x0A05)
#define EMA_CFG                                 PPE_REG_ADDR(0x0A06)
#define EMA_SUBID                               PPE_REG_ADDR(0x0A07)

/*
 *  PP32 Debug Control Register
 */
#define PP32_DBG_CTRL                   PP32_DEBUG_REG_ADDR(0x0000)

//#define DBG_CTRL_START_SET(value)       ((value) ? (1 << 0) : 0)
//#define DBG_CTRL_STOP_SET(value)        ((value) ? (1 << 1) : 0)
//#define DBG_CTRL_STEP_SET(value)        ((value) ? (1 << 2) : 0)
#define DBG_CTRL_RESTART                0
#define DBG_CTRL_STOP                   1

/*
 *  PP32 Registers
 */
//#define PP32_HALT_STAT                          PP32_DEBUG_REG_ADDR(0x0001)
//
//#define PP32_BRK_SRC                            PP32_DEBUG_REG_ADDR(0x0002)
//
//#define PP32_DBG_PC_MIN(i)                      PP32_DEBUG_REG_ADDR(0x0010 + (i))
//#define PP32_DBG_PC_MAX(i)                      PP32_DEBUG_REG_ADDR(0x0014 + (i))
//#define PP32_DBG_DATA_MIN(i)                    PP32_DEBUG_REG_ADDR(0x0018 + (i))
//#define PP32_DBG_DATA_MAX(i)                    PP32_DEBUG_REG_ADDR(0x001A + (i))
//#define PP32_DBG_DATA_VAL(i)                    PP32_DEBUG_REG_ADDR(0x001C + (i))
//
//#define PP32_DBG_CUR_PC                         PP32_DEBUG_REG_ADDR(0x0080)
//
//#define PP32_DBG_TASK_NO                        PP32_DEBUG_REG_ADDR(0x0081)
#define PP32_HALT_STAT                          PP32_DEBUG_REG_ADDR(0x0D00)
#define PP32_BREAKPOINT_REASONS                 PP32_DEBUG_REG_ADDR(0x0A00)

#define PP32_BRK_SRC                            PP32_DEBUG_REG_ADDR(0x0F00)

#define PP32_DBG_PC_MIN(i)                      PP32_DEBUG_REG_ADDR(0x0010 + (i))
#define PP32_DBG_PC_MAX(i)                      PP32_DEBUG_REG_ADDR(0x0014 + (i))
#define PP32_DBG_DATA_MIN(i)                    PP32_DEBUG_REG_ADDR(0x0018 + (i))
#define PP32_DBG_DATA_MAX(i)                    PP32_DEBUG_REG_ADDR(0x001A + (i))
#define PP32_DBG_DATA_VAL(i)                    PP32_DEBUG_REG_ADDR(0x001C + (i))

#define PP32_DBG_CUR_PC                         PP32_DEBUG_REG_ADDR(0x0F80)

#define PP32_DBG_TASK_NO                        PP32_DEBUG_REG_ADDR(0x0F81)

#define PP32_SRST                               PPE_REG_ADDR(0x0020)
#define PP32_CFG                                PPE_REG_ADDR(0x0030)

/*
 *    Code/Data Memory (CDM) Interface Configuration Register
 */
#define CDM_CFG                                 PPE_REG_ADDR(0x0100)

#define CDM_CFG_RAM1_SET(value)                 SET_BITS(0, 3, 2, value)
#define CDM_CFG_RAM0_SET(value)                 ((value) ? (1 << 1) : 0)

/*
 *  ETOP MDIO Configuration Register
 */
#define ETOP_MDIO_CFG_SMRST(value)              ((value) ? (1 << 13) : 0)
#define ETOP_MDIO_CFG_PHYA(i, value)            ((i) ? SET_BITS(0, 12, 8, (value)) : SET_BITS(0, 7, 3, (value)))
#define ETOP_MDIO_CFG_UMM(i, value)             ((value) ? ((i) ? (1 << 2) : (1 << 1)) : 0)

#define ETOP_MDIO_CFG_MASK(i)                   (ETOP_MDIO_CFG_SMRST(1) | ETOP_MDIO_CFG_PHYA(i, 0x1F) | ETOP_MDIO_CFG_UMM(i, 1))

/*
 *  ENet MAC Configuration Register
 */
#define ENET_MAC_CFG_CRC(i)                     (*ENET_MAC_CFG(i) & (0x01 << 11))
#define ENET_MAC_CFG_DUPLEX(i)                  (*ENET_MAC_CFG(i) & (0x01 << 2))
#define ENET_MAC_CFG_SPEED(i)                   (*ENET_MAC_CFG(i) & (0x01 << 1))
#define ENET_MAC_CFG_LINK(i)                    (*ENET_MAC_CFG(i) & 0x01)

#define ENET_MAC_CFG_CRC_OFF(i)                 do { *ENET_MAC_CFG(i) &= ~(1 << 11); } while (0)
#define ENET_MAC_CFG_CRC_ON(i)                  do { *ENET_MAC_CFG(i) |= 1 << 11; } while (0)
#define ENET_MAC_CFG_DUPLEX_HALF(i)             do { *ENET_MAC_CFG(i) &= ~(1 << 2); } while (0)
#define ENET_MAC_CFG_DUPLEX_FULL(i)             do { *ENET_MAC_CFG(i) |= 1 << 2; } while (0)
#define ENET_MAC_CFG_SPEED_10M(i)               do { *ENET_MAC_CFG(i) &= ~(1 << 1); } while (0)
#define ENET_MAC_CFG_SPEED_100M(i)              do { *ENET_MAC_CFG(i) |= 1 << 1; } while (0)
#define ENET_MAC_CFG_LINK_NOT_OK(i)             do { *ENET_MAC_CFG(i) &= ~1; } while (0)
#define ENET_MAC_CFG_LINK_OK(i)                 do { *ENET_MAC_CFG(i) |= 1; } while (0)

/*
 *  ENets Configuration Register
 */
#define ENETS_CFG_VL2_SET                       (1 << 29)
#define ENETS_CFG_VL2_CLEAR                     ~(1 << 29)
#define ENETS_CFG_FTUC_SET                      (1 << 28)
#define ENETS_CFG_FTUC_CLEAR                    ~(1 << 28)
#define ENETS_CFG_DPBC_SET                      (1 << 27)
#define ENETS_CFG_DPBC_CLEAR                    ~(1 << 27)
#define ENETS_CFG_DPMC_SET                      (1 << 26)
#define ENETS_CFG_DPMC_CLEAR                    ~(1 << 26)

/*
 *  ENets Classification Configuration Register
 */
#define ENETS_COS_CFG_VLAN_SET                  (1 << 1)
#define ENETS_COS_CFG_VLAN_CLEAR                ~(1 << 1)
#define ENETS_COS_CFG_DSCP_SET                  (1 << 0)
#define ENETS_COS_CFG_DSCP_CLEAR                ~(1 << 0)

/*
 *  QSB Internal Cell Delay Variation Register
 */
#define QSB_ICDV                        QSB_CONF_REG(0x0007)

#define QSB_ICDV_TAU                    GET_BITS(*QSB_ICDV, 5, 0)

#define QSB_ICDV_TAU_SET(value)         SET_BITS(0, 5, 0, value)

/*
 *  QSB Scheduler Burst Limit Register
 */
#define QSB_SBL                         QSB_CONF_REG(0x0009)

#define QSB_SBL_SBL                     GET_BITS(*QSB_SBL, 3, 0)

#define QSB_SBL_SBL_SET(value)          SET_BITS(0, 3, 0, value)

/*
 *  QSB Configuration Register
 */
#define QSB_CFG                         QSB_CONF_REG(0x000A)

#define QSB_CFG_TSTEPC                  GET_BITS(*QSB_CFG, 1, 0)

#define QSB_CFG_TSTEPC_SET(value)       SET_BITS(0, 1, 0, value)

/*
 *  QSB RAM Transfer Table Register
 */
#define QSB_RTM                         QSB_CONF_REG(0x000B)

#define QSB_RTM_DM                      (*QSB_RTM)

#define QSB_RTM_DM_SET(value)           ((value) & 0xFFFFFFFF)

/*
 *  QSB RAM Transfer Data Register
 */
#define QSB_RTD                         QSB_CONF_REG(0x000C)

#define QSB_RTD_TTV                     (*QSB_RTD)

#define QSB_RTD_TTV_SET(value)          ((value) & 0xFFFFFFFF)

/*
 *  QSB RAM Access Register
 */
#define QSB_RAMAC                       QSB_CONF_REG(0x000D)

#define QSB_RAMAC_RW                    (*QSB_RAMAC & (1 << 31))
#define QSB_RAMAC_TSEL                  GET_BITS(*QSB_RAMAC, 27, 24)
#define QSB_RAMAC_LH                    (*QSB_RAMAC & (1 << 16))
#define QSB_RAMAC_TESEL                 GET_BITS(*QSB_RAMAC, 9, 0)

#define QSB_RAMAC_RW_SET(value)         ((value) ? (1 << 31) : 0)
#define QSB_RAMAC_TSEL_SET(value)       SET_BITS(0, 27, 24, value)
#define QSB_RAMAC_LH_SET(value)         ((value) ? (1 << 16) : 0)
#define QSB_RAMAC_TESEL_SET(value)      SET_BITS(0, 9, 0, value)

/*
 *  Mailbox IGU0 Registers
 */
#define MBOX_IGU0_ISRS                          PPE_REG_ADDR(0x0200)
#define MBOX_IGU0_ISRC                          PPE_REG_ADDR(0x0201)
#define MBOX_IGU0_ISR                           PPE_REG_ADDR(0x0202)
#define MBOX_IGU0_IER                           PPE_REG_ADDR(0x0203)

/*
 *  Mailbox IGU1 Registers
 */
#define MBOX_IGU1_ISRS                          PPE_REG_ADDR(0x0204)
#define MBOX_IGU1_ISRC                          PPE_REG_ADDR(0x0205)
#define MBOX_IGU1_ISR                           PPE_REG_ADDR(0x0206)
#define MBOX_IGU1_IER                           PPE_REG_ADDR(0x0207)

/*
 *  A4 mode related HW register
 */
#define AT_CTRL                         PPE_REG_ADDR(0xD02)
#define AR_CTRL                         PPE_REG_ADDR(0xD08)
#define AT_IDLE0                        PPE_REG_ADDR(0xD28)
#define AT_IDLE1                        PPE_REG_ADDR(0xD29)
#define AR_IDLE0                        PPE_REG_ADDR(0xD74)
#define AR_IDLE1                        PPE_REG_ADDR(0xD75)
#define RFBI_CFG                        PPE_REG_ADDR(0x400)
#define SFSM_DBA0                       PPE_REG_ADDR(0x412)
#define SFSM_DBA1                       PPE_REG_ADDR(0x413)
#define SFSM_CBA0                       PPE_REG_ADDR(0x414)
#define SFSM_CBA1                       PPE_REG_ADDR(0x415)
#define SFSM_CFG0                       PPE_REG_ADDR(0x416)
#define SFSM_CFG1                       PPE_REG_ADDR(0x417)
#define FFSM_DBA0                       PPE_REG_ADDR(0x508)
#define FFSM_DBA1                       PPE_REG_ADDR(0x509)
#define FFSM_CFG0                       PPE_REG_ADDR(0x50A)
#define FFSM_CFG1                       PPE_REG_ADDR(0x50B)
#define FFSM_IDLE_HEAD_BC0              PPE_REG_ADDR(0x50E)
#define FFSM_IDLE_HEAD_BC1              PPE_REG_ADDR(0x50F)

/*
 *  Reset Unit Register
 */
#define AMAZON_SE_RCU                           (KSEG1 + 0x1F203000)
#define RCU_RST_REQ                             ((volatile u32 *)(AMAZON_SE_RCU + 0x0010))

/*
 *  CGU Register
 */
#define AMAZON_SE_CGU                           (KSEG1 + 0x1F103000)
#define CGU_PLL0_CFG                            ((volatile u32 *)(AMAZON_SE_CGU + 0x0004))
#define CGU_PLL1_CFG                            ((volatile u32 *)(AMAZON_SE_CGU + 0x0008))
#define CGU_SYS                                 ((volatile u32 *)(AMAZON_SE_CGU + 0x0010))
#define CGU_IFCCR                               ((volatile u32 *)(AMAZON_SE_CGU + 0x0018))
#define CGU_OSC_CON                             ((volatile u32 *)(AMAZON_SE_CGU + 0x001c))

/*
 *  Power Management Unit Registers
 */
#define AMAZON_SE_PMU                           (KSEG1 + 0x1F102000)
#define PMU_PWDCR                               ((volatile u32 *)(AMAZON_SE_PMU + 0x001C))
#define PMU_SR                                  ((volatile u32 *)(AMAZON_SE_PMU + 0x0020))

/*
 *  GPIO Registers
 */
#define AMAZON_SE_GPIO                          (KSEG1 + 0x1E100B00)
#define GPIO_Pi_OUT(i)                          ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0010))
#define GPIO_Pi_IN(i)                           ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0014))
#define GPIO_Pi_DIR(i)                          ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0018))
#define GPIO_Pi_ALTSEL0(i)                      ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x001C))
#define GPIO_Pi_ALTSEL1(i)                      ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0020))
#define GPIO_Pi_OD(i)                           ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0024))
#define GPIO_Pi_STOFF(i)                        ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0028))
#define GPIO_Pi_PUDSEL(i)                       ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x002C))
#define GPIO_Pi_PUDEN(i)                        ((volatile u32 *)(AMAZON_SE_GPIO + (i) * 0x0030 + 0x0030))

/*
 *  macro for eth framework
 */
#ifndef CONFIG_IFX_ETH_FRAMEWORK
  #define ifx_eth_fw_alloc_netdev(size, ifname, dummy)      alloc_netdev(size, ifname, ether_setup)
  #define ifx_eth_fw_free_netdev(netdev, dummy)             free_netdev(netdev)
  #define ifx_eth_fw_register_netdev(netdev)                register_netdev(netdev)
  #define ifx_eth_fw_unregister_netdev(netdev, dummy)       unregister_netdev(netdev)
  #define ifx_eth_fw_netdev_priv(netdev)                    netdev_priv(netdev)
#endif



/*
 * ####################################
 *              Data Type
 * ####################################
 */

/*
 *  64-bit Data Type
 */
typedef struct {
    unsigned int    h   :32;
    unsigned int    l   :32;
} ppe_u64_t;

/*
 *  PPE ATM Cell Header
 */
#if defined(__BIG_ENDIAN)
  struct uni_cell_header {
    unsigned int    gfc :4;
    unsigned int    vpi :8;
    unsigned int    vci :16;
    unsigned int    pti :3;
    unsigned int    clp :1;
  };
#else
  struct uni_cell_header {
    unsigned int    clp :1;
    unsigned int    pti :3;
    unsigned int    vci :16;
    unsigned int    vpi :8;
    unsigned int    gfc :4;
  };
#endif

/*
 *  Host-PPE Communication Data Structure
 */
#if defined(__BIG_ENDIAN)
  struct fw_ver_id {
    unsigned int    family              :4;
    unsigned int    fwtype              :4;
    unsigned int    interface           :4;
    unsigned int    fwmode              :4;
    unsigned int    major               :8;
    unsigned int    minor               :8;
  };

  struct eth_ports_cfg {
    unsigned int    wan_vlanid_hi       :12;
    unsigned int    wan_vlanid_lo       :12;
    unsigned int    res1                :4;
    unsigned int    eth1_type           :2; //  reserved in A4
    unsigned int    eth0_type           :2;
  };

  struct lan_rout_tbl_cfg {
    unsigned int    lan_rout_num        :9;
    unsigned int    lan_rout_off        :9;
    unsigned int    res1                :4;
    unsigned int    lan_tcpudp_ver_en   :1; //  reserved in A4
    unsigned int    lan_ip_ver_en       :1; //  reserved in A4
    unsigned int    lan_tcpudp_err_drop :1; //  reserved in A4
    unsigned int    lan_rout_drop       :1;
    unsigned int    res2                :6;
  };

  struct wan_rout_tbl_cfg {
    unsigned int    wan_rout_num        :9;
    unsigned int    wan_rout_mc_num     :7;
    unsigned int    res1                :4;
    unsigned int    wan_rout_mc_drop    :1;
    unsigned int    res2                :1;
    unsigned int    wan_tcpdup_ver_en   :1; //  reserved in A4
    unsigned int    wan_ip_ver_en       :1; //  reserved in A4
    unsigned int    wan_tcpudp_err_drop :1; //  reserved in A4
    unsigned int    wan_rout_drop       :1;
    unsigned int    res3                :6;
  };

  struct gen_mode_cfg {
    unsigned int    cpu1_fast_mode      :1; //  reserved in A4
    unsigned int    wan_fast_mode       :1;
    unsigned int    us_early_discard_en :1;
    unsigned int    res1                :2;
    unsigned int    fast_path_wfq       :3;
    unsigned int    dplus_wfq           :8;
    unsigned int    etx_wfq             :8;
    unsigned int    wan_acc_mode        :2;
    unsigned int    lan_acc_mode        :2;
    unsigned int    acc_mode            :2;
    unsigned int    dscp_qos_en         :1;
    unsigned int    res2                :1;
  };

  struct brg_tbl_cfg {
    unsigned int    brg_entry_num       :9;
    unsigned int    res1                :23;
  };

  struct eth_default_dest_list {
    unsigned int    in_etag_en          :2; //  Egress Inner VLAN Tagging Control
    unsigned int    srcip_vlan_en       :1;
    unsigned int    ethtype_vlan_en     :1;
    unsigned int    vlan_vlan_en        :1;
    unsigned int    loop_ctl            :1;
    unsigned int    vlan_en             :1; //  VLAN Aware Enable
    unsigned int    mac_change_drop     :1; //  reserved
    unsigned int    res1                :1;
    unsigned int    port_en             :1; //  Port Based VLAN Enable (i.e.,use default config)
    unsigned int    res2                :3;
    unsigned int    out_itag_vld        :1; //  Ingress Outer VLAN Valid
    unsigned int    out_etag_en         :2; //  Egress Outer VLAN Tagging Control
    unsigned int    unknown_mc_dest_list:8;
    unsigned int    unknown_uc_dest_list:8;
  };

  struct wrx_queue_config {
    /*  0h  */
    unsigned int    res4                :20;
    unsigned int    mpoa_type           :2; //  0: EoA without FCS, 1: EoA with FCS, 2: PPPoA, 3:IPoA
    unsigned int    ip_ver              :1; //  0: IPv4, 1: IPv6
    unsigned int    mpoa_mode           :1; //  0: VCmux, 1: LLC
    unsigned int    res2                :8;
    /*  1h  */
    unsigned int    oversize            :16;
    unsigned int    undersize           :16;
    /*  2h  */
    unsigned int    res1                :16;
    unsigned int    mfs                 :16;
    /*  3h  */
    unsigned int    uumask              :8;
    unsigned int    cpimask             :8;
    unsigned int    uuexp               :8;
    unsigned int    cpiexp              :8;
  };

  struct wtx_port_config {
    unsigned int    res1                :27;
    unsigned int    qid                 :4;
    unsigned int    qsben               :1;
  };

  struct wtx_queue_config {
    /*  0h  */
    unsigned int    uu                  :8;
    unsigned int    cpi                 :8;
    unsigned int    same_vc_qmap        :8; //  e.g., TX Q0, Q2, Q4 is VCID1, config TX Q0, value is binary 00010100. Set all queue in this VC with 1 except this queue.
    unsigned int    res1                :1;
    unsigned int    sbid                :1;
    unsigned int    qsb_vcid            :4; //  Which QSB queue (VCID) does this TX queue map to.
    unsigned int    mpoa_mode           :1; //  0: VCmux, 1: LLC
    unsigned int    qsben               :1; //  reserved in A4
    /*  1h  */
    unsigned int    atm_header          :32;
  };

  struct htu_entry {
    unsigned int    res1        :2;
    unsigned int    pid         :2;
    unsigned int    vpi         :8;
    unsigned int    vci         :16;
    unsigned int    pti         :3;
    unsigned int    vld         :1;
  };

  struct htu_mask {
    unsigned int    set         :2;
    unsigned int    pid_mask    :2;
    unsigned int    vpi_mask    :8;
    unsigned int    vci_mask    :16;
    unsigned int    pti_mask    :3;
    unsigned int    clear       :1;
  };

  struct htu_result {
    unsigned int    res1        :12;
    unsigned int    cellid      :4;
    unsigned int    res2        :5;
    unsigned int    type        :1;
    unsigned int    ven         :1;
    unsigned int    res3        :5;
    unsigned int    qid         :4;
  };

  struct brg_forward_tbl {
    /*  0h  */
    unsigned int    mac_52              :32;
    /*  1h  */
    unsigned int    mac_10              :16;
    unsigned int    all_zero1           :16;
    /*  2h  */
    unsigned int    res1                :16;
    unsigned int    dest_list           :8;
    unsigned int    src_mac_drop        :1;
    unsigned int    itf                 :3;
    unsigned int    res2                :1;
    unsigned int    dslwan_qid          :3;
  };

  struct default_itf_vlan_map {
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8; //  vlan_port_map
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
  };

  struct src_ip_vlan_map {
    /*  0h  */
    unsigned int    src_ip              :32;
    /*  1h  */
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8; //  vlan_port_map
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
  };

  struct ethtype_vlan_map {
    /*  0h  */
    unsigned int    res0                :16;
    unsigned int    ethtype             :16;
    /*  1h  */
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8; //  vlan_port_map
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
  };

  struct vlan_vlan_map {
    /*  0h  */
    unsigned int    res0                :4;
    unsigned int    in_vlan_tag         :12;
    unsigned int    res1                :16;
    /*  1h  */
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8; //  vlan_port_map
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
    /* 2h */
    unsigned int    in_out_etag_ctrl    :32;    //  MSB -> LSB = itf 7 in, itf 7 out, ... , itf 0 in, itf 0 out
  };

  struct rout_forward_compare_tbl {
    /*  0h  */
    unsigned int    src_ip              :32;
    /*  1h  */
    unsigned int    dest_ip             :32;
    /*  2h  */
    unsigned int    src_port            :16;
    unsigned int    dest_port           :16;
  };

  struct rout_forward_action_tbl {
    /*  0h  */
    unsigned int    new_port            :16;
    unsigned int    new_dest_mac54      :16;
    /*  1h  */
    unsigned int    new_dest_mac30      :32;
    /*  2h  */
    unsigned int    new_ip              :32;
    /*  3h  */
    unsigned int    rout_type           :2;
    unsigned int    new_dscp            :6;
    unsigned int    mtu_ix              :3;
    unsigned int    in_vlan_ins         :1; //  Inner VLAN Insertion Enable
    unsigned int    in_vlan_rm          :1; //  Inner VLAN Remove Enable
    unsigned int    new_dscp_en         :1;
    unsigned int    res1                :1;
    unsigned int    protocol            :1; //  0: UDP, 1: TCP
    unsigned int    dest_list           :8;
    unsigned int    pppoe_mode          :1;
    unsigned int    pppoe_ix            :3; //  not valid for WAN entry
    unsigned int    new_src_mac_ix      :4;
    /*  4h  */
    unsigned int    new_in_vci          :16;//  New Inner VLAN Tag to be inserted
    unsigned int    res3                :1;
    unsigned int    out_vlan_ix         :5; //  New Outer VLAN Tag pointed by this field to be inserted
    unsigned int    out_vlan_ins        :1; //  Outer VLAN Insertion Enable
    unsigned int    out_vlan_rm         :1; //  Outer VLAN Remove Enable
    unsigned int    res4                :2;
    unsigned int    mpoa_type           :2; //  not valid for WAN entry
    unsigned int    res5                :1;
    unsigned int    dslwan_qid          :3; //  not valid for WAN entry
  };

  struct wan_rout_multicast_tbl {
    /*  0h  */
    unsigned int    new_in_vci          :16;
    unsigned int    dest_list           :8;
    unsigned int    pppoe_mode          :1;
    unsigned int    new_src_mac_en      :1;
    unsigned int    in_vlan_ins         :1; //  Inner VLAN Insertion Enable
    unsigned int    in_vlan_rm          :1; //  Inner VLAN Remove Enable
    unsigned int    new_src_mac_ix      :4;
    /*  1h  */
    unsigned int    wan_dest_ip         :32;
  };

  struct wan_rout_mc_outer_vlan_tbl {
    unsigned int    res0                :1;
    unsigned int    out_vlan_ix0        :5;
    unsigned int    out_vlan_ins0       :1;
    unsigned int    out_vlan_rm0        :1;
    unsigned int    res1                :1;
    unsigned int    out_vlan_ix1        :5;
    unsigned int    out_vlan_ins1       :1;
    unsigned int    out_vlan_rm1        :1;
    unsigned int    res2                :1;
    unsigned int    out_vlan_ix2        :5;
    unsigned int    out_vlan_ins2       :1;
    unsigned int    out_vlan_rm2        :1;
    unsigned int    res3                :1;
    unsigned int    out_vlan_ix3        :5;
    unsigned int    out_vlan_ins3       :1;
    unsigned int    out_vlan_rm3        :1;
  };
#else
#endif

struct dsl_wan_mib_table {
    u32             res1;
    u32             wrx_drophtu_cell;
    u32             wrx_dropdes_pdu;
    u32             wrx_correct_pdu;
    u32             wrx_err_pdu;
    u32             wrx_dropdes_cell;
    u32             wrx_correct_cell;
    u32             wrx_err_cell;
    u32             wrx_total_byte;
    u32             res2;
    u32             wtx_total_pdu;
    u32             wtx_total_cell;
    u32             wtx_total_byte;
};
#define wan_mib_table                           dsl_wan_mib_table

struct dsl_wan_vc_mib_table {
    u32             vc_rx_pdu;
    u32             vc_rx_bytes;
    u32             vc_tx_pdu;
    u32             vc_tx_bytes;
};

struct itf_mib {
    u32             ig_fast_brg_uni_pkts;
    u32             ig_fast_brg_mul_pkts;
    u32             ig_fast_brg_br_pkts;
    u32             res1;

    u32             ig_fast_rt_uni_udp_pkts;
    u32             ig_fast_rt_uni_tcp_pkts;
    u32             ig_fast_rt_mul_pkts;
    u32             res2;

    u32             ig_fast_rt_bytes;
    u32             ig_fast_brg_bytes;

    u32             ig_cpu_pkts;
    u32             ig_cpu_bytes;

    u32             ig_drop_pkts;
    u32             ig_drop_bytes;

    u32             eg_fast_pkts;
    u32             eg_fast_bytes;
};

/*
 *  Flag Header & Descriptor
 */
#if defined(__BIG_ENDIAN)
  struct flag_header {
    //  0 - 3h
    unsigned int    rout_fwd_vld        :1;
    unsigned int    rout_mc_vld         :1;
    unsigned int    is_brg              :1; //  0: route, 1: bridge
    unsigned int    uc_mc_flag          :1; //  0: uni-cast, 1: multicast
    unsigned int    tcpudp_err          :1; //  reserved in A4
    unsigned int    tcpudp_chk          :1; //  reserved in A4
    unsigned int    is_udp              :1;
    unsigned int    is_tcp              :1;
    unsigned int    mpoa_type           :2; //  0: EoA without FCS, 1: EoA with FCS, 2: PPPoA, 3:IPoA
    unsigned int    ip_offset           :6;
    unsigned int    is_pppoes           :1; //  2h
    unsigned int    res1                :1;
    unsigned int    is_ipv4             :1;
    unsigned int    is_vlan             :2; //  0: nil, 1: single tag, 2: double tag, 3: reserved
    unsigned int    res2                :2;
    unsigned int    rout_index          :9;
    //  4 - 7h
    unsigned int    dest_list           :8;
    unsigned int    aal5_raw            :1; //  0: ETH frame with FCS, 1: AAL5 CPCS-SDU
    unsigned int    src_dir             :1; //  0: LAN, 1: WAN
    unsigned int    acc_done            :1;
    unsigned int    tcp_rst             :1;
    unsigned int    tcp_fin             :1;
    unsigned int    dsl_qid             :3; //  valid for Central DMA RX CH 0, 1, 3 only
    unsigned int    temp_dest_list      :8; //  only for firmware use
    unsigned int    src_itf             :3; //  7h
    unsigned int    pl_byteoff          :5;
  };

  struct wlan_flag_header {
    unsigned int    res1                :1;
    unsigned int    aal5_raw            :1; //  0: ETH frame with FCS, 1: AAL5 CPCS-SDU
    unsigned int    mpoa_type           :2; //  0: EoA without FCS, 1: EoA with FCS, 2: PPPoA, 3:IPoA
    unsigned int    res2                :1;
    unsigned int    dsl_qid             :3; //  not valid for WLAN
    unsigned int    res3                :5;
    unsigned int    src_itf             :3;
    unsigned int    payload_overlap     :16;//  This flag header is 16 bits only. This field overlapps with payload, and is for 32-bit align purpose.
  };

  struct rx_descriptor {
    //  0 - 3h
    unsigned int    own                 :1; //  0: PPE, 1: MIPS, this value set is for DSL PKT RX and DSL OAM RX, it's special case.
    unsigned int    c                   :1;
    unsigned int    sop                 :1;
    unsigned int    eop                 :1;
    unsigned int    res1                :3;
    unsigned int    byteoff             :2;
    unsigned int    res2                :2;
    unsigned int    qid                 :4;
    unsigned int    err                 :1; //  0: no error, 1: error (RA, IL, CRC, USZ, OVZ, MFL, CPI, CPCS-UU)
    unsigned int    datalen             :16;
    //  4 - 7h
    unsigned int    res3                :3;
    unsigned int    dataptr             :29;
  };

  struct tx_descriptor {
    //  0 - 3h
    unsigned int    own                 :1; //  0: MIPS, 1: PPE, for CPU to WAN TX, it's inverse, 0: PPE, 1: MIPS
    unsigned int    c                   :1;
    unsigned int    sop                 :1;
    unsigned int    eop                 :1;
    unsigned int    byteoff             :5;
    unsigned int    mpoa_pt             :1; //  not for WLAN TX, 0: MPoA is dterminated in FW, 1: MPoA is transparent to FW.
    unsigned int    mpoa_type           :2; //  not for WLAN TX, 0: EoA without FCS, 1: reserved, 2: PPPoA, 3: IPoA
    unsigned int    pdu_type            :1; //  not for WLAN TX, 0: AAL5, 1: Non-AAL5 cell
    unsigned int    qid                 :3; //  not for WLAN TX,
    unsigned int    datalen             :16;
    //  4 - 7h
    unsigned int    res1                :3;
    unsigned int    dataptr             :29;
  };
#else
#endif

/*
 *  QSB Queue Parameter Table Entry and Queue VBR Parameter Table Entry
 */
#if defined(__BIG_ENDIAN)
  union qsb_queue_parameter_table {
    struct {
      unsigned int  res1    :1;
      unsigned int  vbr     :1;
      unsigned int  wfqf    :14;
      unsigned int  tp      :16;
    }             bit;
    u32           dword;
  };

  union qsb_queue_vbr_parameter_table {
    struct {
      unsigned int  taus    :16;
      unsigned int  ts      :16;
    }             bit;
    u32           dword;
  };
#else
  union qsb_queue_parameter_table {
    struct {
      unsigned int  tp      :16;
      unsigned int  wfqf    :14;
      unsigned int  vbr     :1;
      unsigned int  res1    :1;
    }             bit;
    u32           dword;
  };

  union qsb_queue_vbr_parameter_table {
    struct {
      unsigned int  ts      :16;
      unsigned int  taus    :16;
    }             bit;
    u32           dword;
  };
#endif  //  defined(__BIG_ENDIAN)

/*
 *  Internal Structure of Devices (ETH/ATM)
 */
struct eth_priv_data {
    struct  net_device_stats        stats;

    u32                             enets_igerr;
    u32                             enets_igdrop;
    u32                             enetf_egcol;
    u32                             enetf_egdrop;

    u32                             rx_packets;
    u32                             rx_bytes;
    u32                             rx_dropped;
    u32                             tx_packets;
    u32                             tx_bytes;
    u32                             tx_errors;
    u32                             tx_dropped;

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
    int                             fc_bit;                 /*  net wakeup callback                     */
#endif

    u32                             mii_mode;
    u32                             mdio_phy_addr;
};

struct atm_port {
    struct atm_dev                 *dev;

    u32                             tx_max_cell_rate;       /*  maximum cell rate                       */
    u32                             tx_used_cell_rate;      /*  currently used cell rate                */
};

struct atm_connection {
    struct atm_vcc                 *vcc;                    /*  opened VCC                              */
#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    struct net_device              *netdev;
#endif
    struct timespec                 access_time;            /*  time when last F4/F5 user cell arrives  */

    int                             prio_queue_map[8];
    u32                             prio_tx_packets[8];

    u32                             rx_packets;
    u32                             rx_bytes;
    u32                             rx_dropped;
    u32                             tx_packets;
    u32                             tx_bytes;
    u32                             tx_errors;
    u32                             tx_dropped;

    u32                             port;                   /*  to which port the connection belongs    */
    u32                             sw_tx_queue_table;      /*  software TX queues used for this        */
                                                            /*  connection                              */
};

struct atm_priv_data {
    struct atm_port                 port[ATM_PORT_NUMBER];
    struct atm_connection           connection[ATM_QUEUE_NUMBER];
    u32                             max_connections;        /*  total connections available             */
    u32                             connection_table;       /*  connection opened status, every bit     */
                                                            /*  stands for one connection as well as    */
                                                            /*  the QSB queue used by this connection   */
    u32                             max_sw_tx_queues;       /*  total software TX queues available      */
    u32                             sw_tx_queue_table;      /*  software TX queue occupations status    */

    ppe_u64_t                       wrx_total_byte;         /*  bit-64 extention of MIB table member    */
    ppe_u64_t                       wtx_total_byte;         /*  bit-64 extention of MIB talbe member    */

    u32                             wrx_pdu;                /*  successfully received AAL5 packet       */
    u32                             wrx_drop_pdu;           /*  AAL5 packet dropped by driver on RX     */
    u32                             wtx_pdu;
    u32                             wtx_err_pdu;            /*  error AAL5 packet                       */
    u32                             wtx_drop_pdu;           /*  AAL5 packet dropped by driver on TX     */

    struct dsl_wan_mib_table        prev_mib;
};



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  ATM Operations
 */
static int ppe_ioctl(struct atm_dev *, unsigned int, void *);
static int ppe_open(struct atm_vcc *);
static void ppe_close(struct atm_vcc *);
static int ppe_send(struct atm_vcc *, struct sk_buff *);
static int ppe_send_oam(struct atm_vcc *, void *, int);
static int ppe_change_qos(struct atm_vcc *, struct atm_qos *, int);

/*
 *  ATM Upper Layer Hook Function
 */
static void mpoa_setup(struct atm_vcc *, int, int);

/*
 *  Network Operations
 */
static void eth_setup(struct net_device *);
static struct net_device_stats *eth_get_stats(struct net_device *);
static int eth_open(struct net_device *);
static int eth_stop(struct net_device *);
static int eth_hard_start_xmit(struct sk_buff *, struct net_device *);
static int eth_set_mac_address(struct net_device *, void *);
static int eth_ioctl(struct net_device *, struct ifreq *, int);
static void eth_tx_timeout(struct net_device *);

/*
 *  Network operations help functions
 */
static INLINE int get_eth_port(struct net_device *);
static INLINE int eth0_xmit(struct sk_buff *, int);
#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
  static void eth_xon(struct net_device *);
#endif
#if defined(ENABLE_PROBE_TRANSCEIVER) && ENABLE_PROBE_TRANSCEIVER
  static INLINE int probe_transceiver(struct net_device *);
#endif

/*
 *  ioctl help functions
 */
static INLINE int ethtool_ioctl(struct net_device *, struct ifreq *);
static INLINE void set_vlan_cos(struct vlan_cos_req *);
static INLINE void set_dscp_cos(struct dscp_cos_req *);
static INLINE u32 __ephy_read_mdio_reg(int, int);
static INLINE int __ephy_write_mdio_reg(int, int, u32);
static INLINE int __ephy_auto_negotiate(int);
static INLINE int set_mac(struct net_device *, u32, u32, u32);

/*
 *  64-bit operation used by MIB calculation
 */
static INLINE void u64_add_u32(ppe_u64_t opt1, u32 opt2,ppe_u64_t *ret);

/*
 *  DSL led flash function
 */
static INLINE void adsl_led_flash(void);

/*
 *  DSL PVC Multiple Queue/Priority Operation
 */
static int sw_tx_queue_add(int);
static int sw_tx_queue_del(int);
static int sw_tx_prio_to_queue(int, int, int, int *);

/*
 *  QSB & HTU setting functions
 */
static void set_qsb(struct atm_vcc *, struct atm_qos *, unsigned int);
static INLINE void set_htu_entry(unsigned int, unsigned int, unsigned int, int);
static INLINE void clear_htu_entry(unsigned int);

/*
 *  QSB & HTU init functions
 */
static INLINE void qsb_global_set(void);
static INLINE void setup_oam_htu_entry(void);
static INLINE void validate_oam_htu_entry(void);
static INLINE void invalidate_oam_htu_entry(void);

/*
 *  look up for connection ID
 */
static INLINE int find_vpi(unsigned int);
static INLINE int find_vpivci(unsigned int, unsigned int);
static INLINE int find_vcc(struct atm_vcc *);

/*
 *  Buffer manage functions
 */
static INLINE struct sk_buff *alloc_skb_rx(void);
static INLINE struct sk_buff *alloc_skb_tx(int);
static struct sk_buff* skb_break_away_from_protocol(struct sk_buff *);
static INLINE int __get_skb_from_dbg_pool(struct sk_buff *, const char *, unsigned int);
#define get_skb_from_dbg_pool(skb)  __get_skb_from_dbg_pool(skb, __FUNCTION__, __LINE__)
static INLINE struct sk_buff *__get_skb_pointer(unsigned int, unsigned int, const char *, unsigned int);
#define get_skb_pointer(dataptr, s) __get_skb_pointer(dataptr, s, __FUNCTION__, __LINE__)
static INLINE void __put_skb_to_dbg_pool(struct sk_buff *, const char *, unsigned int);
#define put_skb_to_dbg_pool(skb)    __put_skb_to_dbg_pool(skb, __FUNCTION__, __LINE__)
static struct sk_buff* atm_alloc_tx(struct atm_vcc *, unsigned int);
static INLINE void atm_free_tx_skb_vcc(struct sk_buff *skb);
static INLINE void atm_free_tx_skb_vcc_delayed(struct sk_buff *skb);

/*
 *  Mailbox handler
 */
static irqreturn_t mailbox_irq_handler(int, void *);
#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
  static void softq_send(int);
#endif

/*
 *  Turn On/Off Dma
 */
static INLINE void turn_on_dma_rx(int);
static INLINE void turn_off_dma_rx(int);

/*
 *  DMA interface functions
 */
static u8 *dma_buffer_alloc(int, int *, void **);
static int dma_buffer_free(u8 *, void *);
static int dma_int_handler(struct dma_device_info *, int);
static INLINE int dma_rx_int_handler(struct dma_device_info *);

/*
 *  Hardware init functions
 */
static INLINE void reset_ppe(void);
static INLINE void init_pmu(void);
static INLINE void init_gpio(int);
static INLINE void configureMiiRxClk(int, int);
static INLINE void configureMiiTxClk(int, int);
static INLINE void configureRMiiRefClk(int, int);
static INLINE void configurePhyClk(int);
static INLINE void init_etop(int);
static INLINE void start_etop(void);
static INLINE void stop_datapath(void);
static INLINE void init_ema(void);
static INLINE void init_mailbox(void);
static INLINE void clear_share_buffer(void);
static INLINE void clear_cdm(void);
static INLINE void board_init(void);
static INLINE void init_hw(void);

/*
 *  PP32 specific functions
 */
static INLINE int pp32_download_code(const u32 *, unsigned int, const u32 *, unsigned int);
static INLINE int pp32_specific_init(int, void *);
static INLINE int pp32_start(int);
static INLINE void pp32_stop(void);

/*
 *  Init & clean-up functions
 */
static INLINE int init_local_variables(void);
static INLINE void clear_local_variables(void);
static INLINE void init_communication_data_structures(int);
static INLINE int alloc_dma(void);
static INLINE void free_dma(void);
#if defined(CONFIG_IFX_PPA_DATAPATH)
  static INLINE void ethaddr_setup(unsigned int, char *);
#endif

/*
 *  DSL Data Led help function
 */
static void dsl_led_polling(unsigned long);

/*
 *  local implement memcpy
 */
#if defined(ENABLE_MY_MEMCPY) && ENABLE_MY_MEMCPY
  static INLINE void *my_memcpy(unsigned char *, const unsigned char *, unsigned int);
#else
  #define my_memcpy             memcpy
#endif

/*
 *  Print Firmware Version ID
 */
static int print_fw_ver(char *, int);
static int print_driver_ver(char *, int, char *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

/*
 *  Proc File
 */
static INLINE void proc_file_create(void);
static INLINE void proc_file_delete(void);
static int proc_read_ver(char *, char **, off_t, int, int *, void *);
static int proc_read_mib(char *, char **, off_t, int, int *, void *);
static int proc_write_mib(struct file *, const char *, unsigned long, void *);
static int proc_read_dsl_mib(char *, char **, off_t, int, int *, void *);
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
  static int proc_read_route(char *, char **, off_t, int, int *, void *);
  static int proc_write_route(struct file *, const char *, unsigned long, void *);
  static int proc_read_bridge(char *, char **, off_t, int, int *, void *);
  static int proc_write_bridge(struct file *, const char *, unsigned long, void *);
  static int proc_read_mc(char *, char **, off_t, int, int *, void *);
  static int proc_read_genconf(char *, char **, off_t, int, int *, void *);
  static int proc_write_genconf(struct file *, const char *, unsigned long, void *);
  static int proc_read_queue(char *, char **, off_t, int, int *, void *);
  static int proc_write_queue(struct file *, const char *, unsigned long, void *);
  static int proc_read_pppoe(char *, char **, off_t, int, int *, void *);
  static int proc_read_mtu(char *, char **, off_t, int, int *, void *);
  static int proc_read_hit(char *, char **, off_t, int, int *, void *);
  static int proc_write_hit(struct file *, const char *, unsigned long, void *);
  static int proc_read_mac(char *, char **, off_t, int, int *, void *);
  static int proc_read_vlan(char *, char **, off_t, int, int *, void *);
  static int proc_read_out_vlan(char *, char **, off_t, int, int *, void *);
#endif
#if defined(DEBUG_HTU_PROC) && DEBUG_HTU_PROC
  static int proc_read_htu(char *, char **, off_t, int, int *, void *);
#endif
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int proc_read_dbg(char *, char **, off_t, int, int *, void *);
  static int proc_write_dbg(struct file *, const char *, unsigned long, void *);
#endif
#if defined(DEBUG_MEM_PROC) && DEBUG_MEM_PROC
  static int proc_write_mem(struct file *, const char *, unsigned long, void *);
#endif
#if defined(DEBUG_PP32_PROC) && DEBUG_PP32_PROC
  static int proc_read_pp32(char *, char **, off_t, int, int *, void *);
  static int proc_write_pp32(struct file *, const char *, unsigned long, void *);
#endif
static int proc_read_burstlen(char *, char **, off_t, int, int *, void *);
static int proc_write_burstlen(struct file *, const char *, unsigned long, void *);
#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
  static int proc_read_fw(char *, char **, off_t, int, int *, void *);
  static int proc_write_fw(struct file *, const char *, unsigned long, void *);
#endif
#if defined(DEBUG_SEND_PROC) && DEBUG_SEND_PROC
  static int proc_read_send(char *, char **, off_t, int, int *, void *);
  static int proc_write_send(struct file *, const char *, unsigned long, void *);
#endif
static int proc_read_prio(char *, char **, off_t, int, int *, void *);
static int proc_write_prio(struct file *, const char *, unsigned long, void *);
#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
  static int proc_read_softq(char *, char **, off_t, int, int *, void *);
  static int proc_write_softq(struct file *, const char *, unsigned long, void *);
#endif

/*
 *  Proc File help functions
 */
static INLINE int proc_buf_copy(char **, int, off_t, int *, const char *, int);
static INLINE int stricmp(const char *, const char *);
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
  static INLINE int strincmp(const char *, const char *, int);
#endif
static INLINE int get_token(char **, char **, int *, int *);
static INLINE int get_number(char **, int *, int);
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
  static INLINE void get_ip_port(char **, int *, unsigned int *);
  static INLINE void get_mac(char **, int *, unsigned int *);
#endif
static INLINE void ignore_space(char **, int *);
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
  static INLINE int print_wan_route(char *, int, struct rout_forward_compare_tbl *, struct rout_forward_action_tbl *);
  static INLINE int print_lan_route(char *, int, struct rout_forward_compare_tbl *, struct rout_forward_action_tbl *);
  static INLINE int print_bridge(char *, int, struct brg_forward_tbl *);
  static INLINE int print_mc(char *, int, struct wan_rout_multicast_tbl *);
#endif

/*
 *  Debug functions
 */
#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
  static INLINE void dump_skb(struct sk_buff *, u32, char *, int, int);
#else
  #define dump_skb(a, b, c, d, e)
#endif
#if defined(DEBUG_DUMP_FLAG_HEADER) && DEBUG_DUMP_FLAG_HEADER
  static INLINE void dump_flag_header(int, struct flag_header *, const char *, int, int);
#else
  #define dump_flag_header(a, b, c, d, e)
#endif
#if defined(DEBUG_DUMP_INIT) && DEBUG_DUMP_INIT
  static INLINE void dump_init(void);
#else
  #define dump_init()
#endif

/*
 *  Directpath Help Functions
 */
#if defined(ENABLE_DIRECTPATH_TX_QUEUE) && ENABLE_DIRECTPATH_TX_QUEUE
  static int send_directpath_tx_queue(void);
#endif

/*
 *  External Functions
 */
extern void (*ppa_hook_mpoa_setup)(struct atm_vcc *, int, int);
#if defined(CONFIG_IFX_OAM) || defined(CONFIG_IFX_OAM_MODULE)
  extern void ifx_push_oam(unsigned char *);
#else
  static inline void ifx_push_oam(unsigned char *dummy) {}
#endif
#if defined(CONFIG_IFXMIPS_DSL_CPE_MEI) || defined(CONFIG_IFXMIPS_DSL_CPE_MEI_MODULE)
 #if !defined(ENABLE_LED_FRAMEWORK) || !ENABLE_LED_FRAMEWORK
  extern int ifx_mei_atm_led_blink(void);
 #endif
  extern int ifx_mei_atm_showtime_check(int *is_showtime, struct port_cell_info *port_cell, void **xdata_addr);
#else
 #if !defined(ENABLE_LED_FRAMEWORK) || !ENABLE_LED_FRAMEWORK
  static inline int ifx_mei_atm_led_blink(void) { return IFX_SUCCESS; }
 #endif
  static inline int ifx_mei_atm_showtime_check(int *is_showtime, struct port_cell_info *port_cell, void **xdata_addr)
  {
    if ( is_showtime != NULL )
        *is_showtime = 0;
    return IFX_SUCCESS;
  }
#endif

/*
 *  External Variables
 */
extern struct sk_buff* (*ifx_atm_alloc_tx)(struct atm_vcc *, unsigned int);
#if defined(CONFIG_IFXMIPS_DSL_CPE_MEI) || defined(CONFIG_IFXMIPS_DSL_CPE_MEI_MODULE)
  extern int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *);
  extern int (*ifx_mei_atm_showtime_exit)(void);
#else
  int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *) = NULL;
  EXPORT_SYMBOL(ifx_mei_atm_showtime_enter);
  int (*ifx_mei_atm_showtime_exit)(void) = NULL;
  EXPORT_SYMBOL(ifx_mei_atm_showtime_exit);
#endif



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

static int g_fwcode = FWCODE_ROUTING_BRIDGING_ACC_A4;

//static u32 g_vlan_queue_map[8] = {0, 1, 2, 3, 4, 5, 6, 7};

static struct semaphore g_sem;  //  lock used by open/close function

static int g_stop_datapath = 0;

static int g_eth_wan_mode = 0;

static unsigned int g_dma_rx_ch2_desc_base = 0;
static struct rx_descriptor *g_dma_rx_ch2_desc;

static u32 g_cpu_to_wan_tx_desc_pos = 0;
static u32 g_wan_rx1_desc_pos = 0;
static struct sk_buff *g_wan_rx1_desc_skb[WAN_RX_DESC_NUM(1)] = {0};
#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
  static u32 g_wlan_cpu_tx_desc_pos = 0;
  static struct sk_buff *g_wlan_cpu_tx_desc_skb[WLAN_CPU_TX_DESC_NUM] = {0};
#endif

static u32 g_mailbox_signal_mask = WAN_RX_SIG(1);

static volatile int             g_f_dma_opened = 0;
static struct dma_device_info  *g_dma_device = NULL;

static struct atm_priv_data g_atm_priv_data;

static struct atmdev_ops g_ppe_atm_ops = {
    owner:      THIS_MODULE,
    open:       ppe_open,
    close:      ppe_close,
    ioctl:      ppe_ioctl,
    send:       ppe_send,
    send_oam:   ppe_send_oam,
    change_qos: ppe_change_qos,
};

static int g_showtime = 0;
static void *g_xdata_addr = NULL;

static u32 g_dsl_wrx_correct_pdu = 0;
static u32 g_dsl_wtx_total_pdu = 0;
static struct timer_list g_dsl_led_polling_timer;
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK && defined(CONFIG_IFX_LED)
  static void *g_data_led_trigger = NULL;
#endif

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
  static unsigned int g_softq_dbg_enable = 0;
  static int g_tx_queue_to_conn[ATM_QUEUE_NUMBER];
  static unsigned int g_tx_queue_upper = 20, g_tx_queue_lower = 16;
  static unsigned int g_enable_softq = 1;
  static unsigned int g_skb_softq_max_length = 200;
  static struct sk_buff_head g_skb_softq[ATM_QUEUE_NUMBER];
  static unsigned int g_tx_queued[ATM_QUEUE_NUMBER] = {0};
#endif

static struct net_device *g_eth_net_dev[1] = {0};

#if !defined(CONFIG_IFX_ETH_FRAMEWORK) && LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static struct net_device_ops g_eth_netdev_ops = {
    .ndo_open            = eth_open,
    .ndo_get_stats       = eth_get_stats,
    .ndo_stop            = eth_stop,
    .ndo_start_xmit      = eth_hard_start_xmit,
    .ndo_set_mac_address = eth_set_mac_address,
    .ndo_do_ioctl        = eth_ioctl,
    .ndo_tx_timeout      = eth_tx_timeout,
};
#endif

static int g_eth_prio_queue_map[1][8];

static u8 g_my_ethaddr[MAX_ADDR_LEN] = {0};

static struct proc_dir_entry *g_eth_proc_dir = NULL;

static int g_dbg_enable = 0;

static unsigned int g_mib_itf = 0x03;

#if defined(ENABLE_SKB_SWAP_DBG) && ENABLE_SKB_SWAP_DBG
  #define g_dbg_skb_num     NUM_ENTITY(g_dbg_skb)
  static u32 g_dbg_skb[2048] = {0};
  static u32 g_dbg_skb_data[g_dbg_skb_num] = {0};
  static int g_dbg_skb_p = 0;
#endif

/*
 *  variable for directpath
 */
static int g_directpath_tx_full = 0;

/*
 *  variable for test
 */
#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
  static struct sk_buff *g_dbg_skb_swap_pool[1024] = {0};
#endif



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

/*
 *  variable for directpath
 */
struct ppe_directpath_data g_ppe_directpath_data[5];




/*
 * ####################################
 *            Local Function
 * ####################################
 */

/*
 *  Description:
 *    Handle all ioctl command. This is the only way, which user level could
 *    use to access PPE driver.
 *  Input:
 *    inode --- struct inode *, file descriptor on drive
 *    file  --- struct file *, file descriptor of virtual file system
 *    cmd   --- unsigned int, device specific commands.
 *    arg   --- unsigned long, pointer to a structure to pass in arguments or
 *              pass out return value.
 *  Output:
 *    int   --- 0:    Success
 *              else: Error Code
 */
static int ppe_ioctl(struct atm_dev *dev, unsigned int cmd, void *arg)
{
    int ret = 0;
    atm_cell_ifEntry_t mib_cell;
    atm_aal5_ifEntry_t mib_aal5;
    atm_aal5_vcc_x_t mib_vcc;
    u32 value;
    int conn;

    if ( _IOC_TYPE(cmd) != PPE_ATM_IOC_MAGIC
        || _IOC_NR(cmd) >= PPE_ATM_IOC_MAXNR )
        return -ENOTTY;

    if ( _IOC_DIR(cmd) & _IOC_READ )
        ret = !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd));
    else if ( _IOC_DIR(cmd) & _IOC_WRITE )
        ret = !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd));
    if ( ret )
        return -EFAULT;

    switch ( cmd )
    {
    case PPE_ATM_MIB_CELL:  /*  cell level  MIB */
        /*  These MIB should be read at ARC side, now put zero only.    */
        mib_cell.ifHCInOctets_h = 0;
        mib_cell.ifHCInOctets_l = 0;
        mib_cell.ifHCOutOctets_h = 0;
        mib_cell.ifHCOutOctets_l = 0;
        mib_cell.ifInErrors = 0;
        mib_cell.ifInUnknownProtos = DSL_WAN_MIB_TABLE->wrx_drophtu_cell;
        mib_cell.ifOutErrors = 0;

        ret = sizeof(mib_cell) - copy_to_user(arg, &mib_cell, sizeof(mib_cell));
        break;
    case PPE_ATM_MIB_AAL5:  /*  AAL5 MIB    */
        value = WAN_MIB_TABLE->wrx_total_byte;
        u64_add_u32(g_atm_priv_data.wrx_total_byte, value - g_atm_priv_data.prev_mib.wrx_total_byte, &g_atm_priv_data.wrx_total_byte);
        g_atm_priv_data.prev_mib.wrx_total_byte = value;
        mib_aal5.ifHCInOctets_h = g_atm_priv_data.wrx_total_byte.h;
        mib_aal5.ifHCInOctets_l = g_atm_priv_data.wrx_total_byte.l;

        value = WAN_MIB_TABLE->wtx_total_byte;
        u64_add_u32(g_atm_priv_data.wtx_total_byte, value - g_atm_priv_data.prev_mib.wtx_total_byte, &g_atm_priv_data.wtx_total_byte);
        g_atm_priv_data.prev_mib.wtx_total_byte = value;
        mib_aal5.ifHCOutOctets_h = g_atm_priv_data.wtx_total_byte.h;
        mib_aal5.ifHCOutOctets_l = g_atm_priv_data.wtx_total_byte.l;

        mib_aal5.ifInUcastPkts  = g_atm_priv_data.wrx_pdu;
        mib_aal5.ifOutUcastPkts = WAN_MIB_TABLE->wtx_total_pdu;
        mib_aal5.ifInErrors     = WAN_MIB_TABLE->wrx_err_pdu;
        mib_aal5.ifInDiscards   = WAN_MIB_TABLE->wrx_dropdes_pdu + g_atm_priv_data.wrx_drop_pdu;
        mib_aal5.ifOutErros     = g_atm_priv_data.wtx_err_pdu;
        mib_aal5.ifOutDiscards  = g_atm_priv_data.wtx_drop_pdu;

        ret = sizeof(mib_aal5) - copy_to_user(arg, &mib_aal5, sizeof(mib_aal5));
        break;
    case PPE_ATM_MIB_VCC:   /*  VCC related MIB */
        copy_from_user(&mib_vcc, arg, sizeof(mib_vcc));
        conn = find_vpivci(mib_vcc.vpi, mib_vcc.vci);
        if ( conn >= 0 )
        {
            mib_vcc.mib_vcc.aal5VccCrcErrors     = 0;   //  no support any more, g_atm_priv_data.connection[conn].aal5_vcc_crc_err;
            mib_vcc.mib_vcc.aal5VccOverSizedSDUs = 0;   //  no support any more, g_atm_priv_data.connection[conn].aal5_vcc_oversize_sdu;
            mib_vcc.mib_vcc.aal5VccSarTimeOuts   = 0;   /*  no timer support    */
            ret = sizeof(mib_vcc) - copy_to_user(arg, &mib_vcc, sizeof(mib_vcc));
        }
        else
            ret = -EINVAL;
        break;
    case PPE_ATM_MAP_PKT_PRIO_TO_Q:
        {
            struct ppe_prio_q_map cmd;

            if ( copy_from_user(&cmd, arg, sizeof(cmd)) )
                return -EFAULT;

            if ( cmd.qid < 0 || cmd.pkt_prio < 0 || cmd.pkt_prio >= 8 )
                return -EINVAL;

            conn = find_vpivci(cmd.vpi, cmd.vci);
            if ( conn < 0 )
                return -EINVAL;

            return sw_tx_prio_to_queue(conn, cmd.pkt_prio, cmd.qid, NULL);
        }
        break;
    case PPE_ATM_TX_Q_OP:
        {
            struct tx_q_op cmd;

            if ( copy_from_user(&cmd, arg, sizeof(cmd)) )
                return -EFAULT;

            if ( !(cmd.flags & PPE_ATM_TX_Q_OP_CHG_MASK) )
                return -EFAULT;

            conn = find_vpivci(cmd.vpi, cmd.vci);
            if ( conn < 0 )
                return -EINVAL;

            if ( (cmd.flags & PPE_ATM_TX_Q_OP_ADD) )
                return sw_tx_queue_add(conn);
            else
                return sw_tx_queue_del(conn);
        }
        break;
    case PPE_ATM_GET_MAP_PKT_PRIO_TO_Q:
        {
            struct ppe_prio_q_map_all cmd;
            u32 sw_tx_queue_table;
            int sw_tx_queue_to_virt_queue_map[ATM_SW_TX_QUEUE_NUMBER] = {0};
            int sw_tx_queue;
            int virt_queue;
            int i;

            if ( copy_from_user(&cmd, arg, sizeof(cmd)) )
                return -EFAULT;

            conn = find_vpivci(cmd.vpi, cmd.vci);
            if ( conn >= 0 )
            {
                for ( sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table, sw_tx_queue = virt_queue = 0;
                      sw_tx_queue_table != 0;
                      sw_tx_queue_table >>= 1, sw_tx_queue++ )
                    if ( (sw_tx_queue_table & 0x01) )
                    {
                        sw_tx_queue_to_virt_queue_map[sw_tx_queue] = virt_queue++;
                    }

                cmd.total_queue_num = virt_queue;
                for ( i = 0; i < 8; i++ )
                {
                    cmd.pkt_prio[i] = i;
                    cmd.qid[i] = sw_tx_queue_to_virt_queue_map[g_atm_priv_data.connection[conn].prio_queue_map[i]];
                }

                ret = sizeof(cmd) - copy_to_user(arg, &cmd, sizeof(cmd));
            }
            else
                ret = -EINVAL;
        }
        break;
    default:
        ret = -ENOIOCTLCMD;
    }

    return ret;
}

static int ppe_open(struct atm_vcc *vcc)
{
    int ret;
    short vpi = vcc->vpi;
    int vci = vcc->vci;
    struct atm_port *port = &g_atm_priv_data.port[(int)vcc->dev->dev_data];
    int sw_tx_queue;
    unsigned int dsl_tx_queue_mapping;
    int conn;
    int f_enable_irq = 0;
    struct wrx_queue_config wrx_queue_config = {0};
    struct wtx_queue_config wtx_queue_config = {0};
    struct uni_cell_header *cell_header;
    int i;

    if ( g_eth_wan_mode )
    {
        err("ETH WAN mode: g_eth_wan_mode = %d", g_eth_wan_mode);
        return -EIO;
    }

    if ( vcc->qos.aal != ATM_AAL5 && vcc->qos.aal != ATM_AAL0 )
        return -EPROTONOSUPPORT;

    down(&g_sem);

    /*  check bandwidth */
    if ( (vcc->qos.txtp.traffic_class == ATM_CBR && vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_used_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_VBR_RT && vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_used_cell_rate))
#if defined(DISABLE_VBR) && DISABLE_VBR
      || (vcc->qos.txtp.traffic_class == ATM_VBR_NRT && vcc->qos.txtp.pcr > (port->tx_max_cell_rate - port->tx_used_cell_rate))
#else
      || (vcc->qos.txtp.traffic_class == ATM_VBR_NRT && vcc->qos.txtp.scr > (port->tx_max_cell_rate - port->tx_used_cell_rate))
#endif
      || (vcc->qos.txtp.traffic_class == ATM_UBR_PLUS && vcc->qos.txtp.min_pcr > (port->tx_max_cell_rate - port->tx_used_cell_rate)) )
    {
        err("exceed TX line rate");
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    /*  check existing vpi,vci  */
    conn = find_vpivci(vpi, vci);
    if ( conn >= 0 )
    {
        err("existing PVC (%d.%d)", (int)vpi, vci);
        ret = -EADDRINUSE;
        goto PPE_OPEN_EXIT;
    }

    /*  check whether it need to enable irq */
    if ( g_atm_priv_data.connection_table == 0 )
        f_enable_irq = 1;

    /*  allocate connection */
    for ( sw_tx_queue = 0; sw_tx_queue < g_atm_priv_data.max_sw_tx_queues; sw_tx_queue++ )
        if ( !(g_atm_priv_data.sw_tx_queue_table & (1 << sw_tx_queue)) )
            break;
    if ( sw_tx_queue == g_atm_priv_data.max_sw_tx_queues )
    {
        err("no free TX queue");
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    /*  allocate connection */
    for ( conn = 0; conn < g_atm_priv_data.max_connections; conn++ )
        if ( !(g_atm_priv_data.connection_table & (1 << conn)) )
        {
            g_atm_priv_data.connection_table |= 1 << conn;
            g_atm_priv_data.sw_tx_queue_table |= 1 << sw_tx_queue;
            g_atm_priv_data.connection[conn].vcc = vcc;
            g_atm_priv_data.connection[conn].port = (u32)vcc->dev->dev_data;
            g_atm_priv_data.connection[conn].sw_tx_queue_table = 1 << sw_tx_queue;
            for ( i = 0, dsl_tx_queue_mapping = 0; i < 8; i++ )
            {
                g_atm_priv_data.connection[conn].prio_queue_map[i] = sw_tx_queue;
                dsl_tx_queue_mapping |= sw_tx_queue << (i << 2);
            }
            *DSL_TX_QUEUE_MAPPING(conn) = dsl_tx_queue_mapping;
#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
            g_tx_queue_to_conn[sw_tx_queue] = conn;
#endif
            break;
        }
    if ( conn == g_atm_priv_data.max_connections )
    {
        err("exceed PVC limit");
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    /*  reserve bandwidth   */
    switch ( vcc->qos.txtp.traffic_class )
    {
    case ATM_CBR:
    case ATM_VBR_RT:
        port->tx_used_cell_rate += vcc->qos.txtp.max_pcr;
        break;
    case ATM_VBR_NRT:
#if defined(DISABLE_VBR) && DISABLE_VBR
        port->tx_used_cell_rate += vcc->qos.txtp.pcr;
#else
        port->tx_used_cell_rate += vcc->qos.txtp.scr;
#endif
        break;
    case ATM_UBR_PLUS:
        port->tx_used_cell_rate += vcc->qos.txtp.min_pcr;
        break;
    }

    /*  setup RX queue cfg and TX queue cfg */
    wrx_queue_config.mpoa_type  = 3;    //  set IPoA as default
    wrx_queue_config.ip_ver     = 0;    //  set IPv4 as default
    wrx_queue_config.mpoa_mode  = 0;    //  set VC-mux as default
    wrx_queue_config.oversize   = aal5r_max_packet_size;
    wrx_queue_config.undersize  = aal5r_min_packet_size;
    wrx_queue_config.mfs        = aal5r_max_packet_size;

    wtx_queue_config.same_vc_qmap   = 0x00;     //  up to now, one VC with multiple TX queue has not been implemented
    wtx_queue_config.sbid           = g_atm_priv_data.connection[conn].port;
    wtx_queue_config.qsb_vcid       = conn + QSB_QUEUE_NUMBER_BASE; //  qsb qid = firmware qid + 1
    wtx_queue_config.mpoa_mode      = 0;        //  set VC-mux as default
    wtx_queue_config.qsben          = 1;        //  reserved in A4, however put 1 for backward compatible

    cell_header = (struct uni_cell_header *)((u32 *)&wtx_queue_config + 1);
    cell_header->clp    = (vcc->atm_options & ATM_ATMOPT_CLP) ? 1 : 0;
    cell_header->pti    = ATM_PTI_US0;
    cell_header->vci    = vci;
    cell_header->vpi    = vpi;
    cell_header->gfc    = 0;

    *WRX_QUEUE_CONFIG(conn) = wrx_queue_config;
    *WTX_QUEUE_CONFIG(sw_tx_queue) = wtx_queue_config;

    /*  set qsb */
    set_qsb(vcc, &vcc->qos, conn);

    /*  update atm_vcc structure    */
    vcc->itf = (int)vcc->dev->dev_data;
    set_bit(ATM_VF_READY, &vcc->flags);

    if ( f_enable_irq )
    {
        ifx_atm_alloc_tx = atm_alloc_tx;
        turn_on_dma_rx(31);

        g_dsl_wrx_correct_pdu = DSL_WAN_MIB_TABLE->wrx_correct_pdu;
        g_dsl_wtx_total_pdu = DSL_WAN_MIB_TABLE->wtx_total_pdu;
        g_dsl_led_polling_timer.expires = jiffies + HZ / 10;    //  100ms
        add_timer(&g_dsl_led_polling_timer);
    }

    /*  set htu entry   */
    set_htu_entry(vpi, vci, conn, vcc->qos.aal == ATM_AAL5 ? 1 : 0);

    dbg("ppe_open(%d.%d): conn = %d", vcc->vpi, vcc->vci, conn);
    up(&g_sem);
    return 0;

PPE_OPEN_EXIT:
    up(&g_sem);
    return ret;
}

static void ppe_close(struct atm_vcc *vcc)
{
    int conn;
    struct atm_port *port;
    struct atm_connection *connection;

    if ( vcc == NULL )
        return;

    down(&g_sem);

    /*  get connection id   */
    conn = find_vcc(vcc);
    if ( conn < 0 )
    {
        err("can't find vcc");
        goto PPE_CLOSE_EXIT;
    }
    connection = &g_atm_priv_data.connection[conn];
    port = &g_atm_priv_data.port[connection->port];

    /*  clear htu   */
    clear_htu_entry(conn);

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    {
        unsigned int sw_tx_queue_table = connection->sw_tx_queue_table;
        int bit;

        while ( (bit = clz(sw_tx_queue_table)) >= 0 )
        {
            ASSERT(bit < 8, "incorrect TX queue");
            g_tx_queue_to_conn[bit] = -1;
            sw_tx_queue_table ^= 1 << bit;
        }
    }
#endif

    /*  release connection  */
    g_atm_priv_data.connection_table &= ~(1 << conn);
    g_atm_priv_data.sw_tx_queue_table &= ~(connection->sw_tx_queue_table);
    memset(connection, 0, sizeof(*connection));

    /*  disable irq */
    if ( g_atm_priv_data.connection_table == 0 )
    {
        del_timer(&g_dsl_led_polling_timer);

        turn_off_dma_rx(31);
        ifx_atm_alloc_tx = NULL;
    }

    /*  release bandwidth   */
    switch ( vcc->qos.txtp.traffic_class )
    {
    case ATM_CBR:
    case ATM_VBR_RT:
        port->tx_used_cell_rate -= vcc->qos.txtp.max_pcr;
        break;
    case ATM_VBR_NRT:
#if defined(DISABLE_VBR) && DISABLE_VBR
        port->tx_used_cell_rate -= vcc->qos.txtp.pcr;
#else
        port->tx_used_cell_rate -= vcc->qos.txtp.scr;
#endif
        break;
    case ATM_UBR_PLUS:
        port->tx_used_cell_rate -= vcc->qos.txtp.min_pcr;
        break;
    }

    /*  idle for a while to let parallel operation finish   */
    udelay(100);

PPE_CLOSE_EXIT:
    up(&g_sem);
}

static int ppe_send(struct atm_vcc *vcc, struct sk_buff *skb)
{
    int ret;
    int conn;
    unsigned long sys_flag;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;
    int byteoff;
    u32 priority;

    if ( vcc == NULL || skb == NULL )
        return -EINVAL;

    if ( skb->len > aal5s_max_packet_size )
        return -EOVERFLOW;

    ATM_SKB(skb)->vcc = vcc;

    if ( g_stop_datapath != 0 )
    {
        atm_free_tx_skb_vcc(skb);
        return -EBUSY;
    }

#if defined(ENABLE_NO_DELAY_ATM_SKB_FREE) && ENABLE_NO_DELAY_ATM_SKB_FREE
    skb_get(skb);
    atm_free_tx_skb_vcc(skb);
    ATM_SKB(skb)->vcc = NULL;
#endif

    conn = find_vcc(vcc);
    if ( conn < 0 )
    {
        err("not find vcc");
        ret = -EINVAL;
        goto FIND_VCC_FAIL;
    }

    if ( !g_showtime )
    {
        err("not in showtime");
        ret = -EIO;
        goto CHECK_SHOWTIME_FAIL;
    }

#if defined(ENABLE_DSL_LOOPBACK_TEST) && ENABLE_DSL_LOOPBACK_TEST
    {
        unsigned char mac[6];
        unsigned char mac_broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        unsigned char ip[4];
        int off = 0;

        dump_skb(skb, DUMP_SKB_LEN, "ppe_send", 0, 1);

        switch ( WRX_QUEUE_CONFIG(conn)->mpoa_type )
        {
        case 0: //  EoA w/o FCS
        case 1: //  EoA w FCS
            if ( memcmp(skb->data + 10, mac_broadcast, 6) == 0 )
            {
                break;
            }
            memcpy(mac, skb->data + 10, 6);
            memcpy(skb->data + 10, skb->data + 16, 6);
            memcpy(skb->data + 16, mac, 6);
            off = 22;
            while ( skb->data[off] == 0x81 && skb->data[off + 1] == 0x00 )  //  VLAN
                off += 4;
            off -= 8;
        case 3: //  IPoA
            off += 8;
            if ( skb->data[off] == 0x08 && skb->data[off + 1] == 0x00 ) //  IP
            {
                off += 14;
                memcpy(ip, skb->data + off, 4);
                memcpy(skb->data + off, skb->data + off + 4, 4);
                memcpy(skb->data + off + 4, ip, 4);
            }
            break;
        }
        dump_skb(skb, DUMP_SKB_LEN, "DSL Loopback", 0, 0);

        g_atm_priv_data.connection[conn].access_time = current_kernel_time();

        adsl_led_flash();

        vcc->push(vcc, skb);

        if ( vcc->stats )
            atomic_inc(&vcc->stats->rx);

        g_atm_priv_data.wrx_pdu++;

        UPDATE_VCC_STAT(conn, rx_packets, 1);
        UPDATE_VCC_STAT(conn, rx_bytes, skb->len);

        return 0;
    }
#endif

    priority = skb->priority;
    if ( priority >= 8 )
        priority = 7;

    /*  reserve space to put pointer in skb */
    byteoff = (u32)skb->data & 15;
    if ( skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff || skb->end - skb->data < 1564 /* 1518 (max ETH frame) + 22 (reserved for firmware) + 10 (AAL5) + 6 (PPPoE) + 4 x 2 (2 VLAN) */ || skb_shared(skb) || skb_cloned(skb) )
    {
        //  this should be few case

        struct sk_buff *new_skb;

        //dbg("skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff");
        ASSERT(skb_headroom(skb) >= sizeof(struct sk_buff *) + byteoff, "skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff");
        ASSERT(skb->end - skb->data >= 1564, "skb->end (%p) - skb->data (%p) = %d < 1518 + 22 + 10 + 6 + 4 x 2", skb->end, skb->data, skb->end - skb->data);

        new_skb = alloc_skb_tx(DMA_PACKET_SIZE);    //  may be used by RX fastpath later
        if ( !new_skb )
        {
            err("no memory");
            ret = -ENOMEM;
            goto ALLOC_SKB_TX_FAIL;
        }
        skb_put(new_skb, skb->len);
        my_memcpy(new_skb->data, skb->data, skb->len);
        atm_free_tx_skb_vcc(skb);
        skb = new_skb;
        byteoff = (u32)skb->data & 15;
#ifndef CONFIG_MIPS_UNCACHED
        /*  write back to physical memory   */
        dma_cache_wback((u32)skb->data, skb->len);
#endif
    }
    else
    {
        *(struct sk_buff **)((u32)skb->data - byteoff - sizeof(struct sk_buff *)) = skb;
#ifndef CONFIG_MIPS_UNCACHED
        /*  write back to physical memory   */
        dma_cache_wback((u32)skb->data - byteoff - sizeof(struct sk_buff *), skb->len + byteoff + sizeof(struct sk_buff *));
#endif
#if !defined(ENABLE_NO_DELAY_ATM_SKB_FREE) || !ENABLE_NO_DELAY_ATM_SKB_FREE
        /*  before putting in TX queue, do something to prevent atm_may_send fail   */
        atomic_sub(skb->truesize, &sk_atm(vcc)->sk_wmem_alloc);
#endif
    }

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    if ( g_enable_softq )
    {
        unsigned long sys_flag;
        int qid = g_atm_priv_data.connection[conn].prio_queue_map[priority];

        if ( skb_queue_len(&g_skb_softq[qid]) != 0 || *DSLWAN_TX_PKT_CNTx(qid) > g_tx_queue_upper )
        {
            if ( skb_queue_len(&g_skb_softq[qid]) < g_skb_softq_max_length )
            {
                skb_queue_tail(&g_skb_softq[qid], skb);
                g_tx_queued[qid]++;
                local_irq_save(sys_flag);
                *MBOX_IGU1_ISRC = WAN_TX_SIG(qid);  //  xuliang: to clear interrupt before it's enabled.
                g_mailbox_signal_mask |= WAN_TX_SIG(qid);
                *MBOX_IGU1_IER = g_mailbox_signal_mask;
                local_irq_restore(sys_flag);

                return 0;
            }
            else
            {
                ret = -EIO;
                err("softq full");
                goto NO_FREE_DESC;
            }
        }
    }
    else if ( g_atm_priv_data.connection[conn].netdev != NULL )
    {
        unsigned long sys_flag;
        int qid = g_atm_priv_data.connection[conn].prio_queue_map[priority];

        if ( *DSLWAN_TX_PKT_CNTx(qid) > g_tx_queue_upper )
        {
            local_irq_save(sys_flag);
            netif_stop_queue(g_atm_priv_data.connection[conn].netdev);
            *MBOX_IGU1_ISRC = WAN_TX_SIG(qid);  //  xuliang: to clear interrupt before it's enabled.
            g_mailbox_signal_mask |= WAN_TX_SIG(qid);
            *MBOX_IGU1_IER = g_mailbox_signal_mask;
            local_irq_restore(sys_flag);
            softq_dbg(0, "+++ xuliang +++: conn = %d, qid = %d, netdev - %s, *MBOX_IGU1_IER = %08x\n", conn, qid, g_atm_priv_data.connection[conn].netdev->name, *MBOX_IGU1_IER);
        }
    }
#endif

    /*  allocate descriptor */
    local_irq_save(sys_flag);
    desc = CPU_TO_WAN_TX_DESC_BASE + g_cpu_to_wan_tx_desc_pos;
    if ( !desc->own )    //  PPE hold
    {
        local_irq_restore(sys_flag);
        ret = -EIO;
        err("PPE hold");
        goto NO_FREE_DESC;
    }
    if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
        g_cpu_to_wan_tx_desc_pos = 0;
    local_irq_restore(sys_flag);

    /*  load descriptor from memory */
    reg_desc = *desc;

#if defined(ENABLE_SKB_SWAP_DBG) && ENABLE_SKB_SWAP_DBG
    {
        int i;
        u32 tmp_skb;

        for ( i = 0; i < g_dbg_skb_num; i++ )
            if ( reg_desc.dataptr == g_dbg_skb_data[i] )
            {
                g_dbg_skb_data[i] = 0;
                break;
            }
        if ( i == g_dbg_skb_num )
            panic("can't find reg_desc.dataptr\n");

        tmp_skb = *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4));
        if ( tmp_skb == g_dbg_skb[i] )
            g_dbg_skb[i] = 0;
        else
            panic("pointer to skb does not match original, g_dbg_skb[%d] = %08X, tmp_skb = %08X, g_dbg_skb[i]->data = %08X, reg_desc.dataptr = %08X\n", i, g_dbg_skb[i], tmp_skb, (u32)((struct sk_buff *)g_dbg_skb[i])->data, reg_desc.dataptr);

        if ( *(volatile u32 *)(KSEG1 | (((u32)skb->data & ~15) - 4)) != (u32)skb )
            panic("pointer to skb in origin is wrong, *(volatile u32 *)(KSEG1 | (((u32)skb->data & ~15) - 4)) = %08X, skb = %08X\n", *(volatile u32 *)(KSEG1 | (((u32)skb->data & ~15) - 4)), (u32)skb);

        g_dbg_skb_data[i] = (u32)skb->data & 0x1FFFFFF0;
        g_dbg_skb[i] = (u32)skb;
    }
#endif

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & 15) == 0, "reg_desc.dataptr (0x%#x) must be 4 DWORDS aligned", reg_desc.dataptr);
    ASSERT(*(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)) > 0x80000000, "pointer to skb is wrong, desc->dataptr = %08X, skb = %08X", reg_desc.dataptr, *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)));
    skb_to_free = get_skb_pointer(reg_desc.dataptr, 0);
    atm_free_tx_skb_vcc_delayed(skb_to_free);

    /*  detach from protocol    */
    skb_to_free = skb;
    skb = skb_break_away_from_protocol(skb);
    dev_kfree_skb_any(skb_to_free);

    put_skb_to_dbg_pool(skb);

    /*  update descriptor   */
    reg_desc.dataptr    = (u32)skb->data & 0x1FFFFFF0;  //  byte address
    reg_desc.byteoff    = byteoff;
    reg_desc.datalen    = skb->len;
    reg_desc.mpoa_pt    = 1;
    reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
    reg_desc.pdu_type   = vcc->qos.aal == ATM_AAL5 ? 0 : 1;
    reg_desc.qid        = g_atm_priv_data.connection[conn].prio_queue_map[priority];
    reg_desc.own        = 0;
    reg_desc.c          = 0;

    /*  update MIB  */
    UPDATE_VCC_STAT(conn, tx_packets, 1);
    UPDATE_VCC_STAT(conn, tx_bytes, skb->len);

    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx);
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_pdu++;

    g_atm_priv_data.connection[conn].prio_tx_packets[priority]++;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send", reg_desc.qid, 1);

    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    adsl_led_flash();

    return 0;

FIND_VCC_FAIL:
    if ( vcc->stats )
    {
        atomic_inc(&vcc->stats->tx_err);
    }
    if ( vcc->qos.aal == ATM_AAL5 )
    {
        g_atm_priv_data.wtx_err_pdu++;
        g_atm_priv_data.wtx_drop_pdu++;
    }
    atm_free_tx_skb_vcc(skb);
    return ret;

CHECK_SHOWTIME_FAIL:
ALLOC_SKB_TX_FAIL:
    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx_err);
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_drop_pdu++;
    UPDATE_VCC_STAT(conn, tx_dropped, 1);
    atm_free_tx_skb_vcc(skb);
    return ret;

NO_FREE_DESC:
    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx_err);
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_drop_pdu++;
    UPDATE_VCC_STAT(conn, tx_dropped, 1);
#if defined(ENABLE_NO_DELAY_ATM_SKB_FREE) && ENABLE_NO_DELAY_ATM_SKB_FREE
    atm_free_tx_skb_vcc(skb);
#else
    atm_free_tx_skb_vcc_delayed(skb);
#endif
    return ret;
}

static int ppe_send_oam(struct atm_vcc *vcc, void *cell, int flags)
{
    int conn;
    struct uni_cell_header *uni_cell_header = (struct uni_cell_header *)cell;
    struct sk_buff *skb;
    unsigned long sys_flag;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;

    if ( ((uni_cell_header->pti == ATM_PTI_SEGF5 || uni_cell_header->pti == ATM_PTI_E2EF5)
        && find_vpivci(uni_cell_header->vpi, uni_cell_header->vci) < 0)
        || ((uni_cell_header->vci == 0x03 || uni_cell_header->vci == 0x04)
        && find_vpi(uni_cell_header->vpi) < 0) )
        return -EINVAL;

    if ( !g_showtime )
    {
        err("not in showtime");
        return -EIO;
    }

    /*  find queue ID   */
    conn = find_vcc(vcc);
    if ( conn < 0 )
    {
        err("OAM not find queue");
        return -EINVAL;
    }

    /*  allocate sk_buff    */
    skb = alloc_skb_tx(DMA_PACKET_SIZE);    //  may be used by RX fastpath later
    if ( skb == NULL )
        return -ENOMEM;

    /*  copy data   */
    skb_put(skb, CELL_SIZE);
    my_memcpy(skb->data, cell, CELL_SIZE);

#ifndef CONFIG_MIPS_UNCACHED
    /*  write back to physical memory   */
    dma_cache_wback((u32)skb->data, skb->len);
#endif

    /*  allocate descriptor */
    local_irq_save(sys_flag);
    desc = CPU_TO_WAN_TX_DESC_BASE + g_cpu_to_wan_tx_desc_pos;
    if ( !desc->own )    //  PPE hold
    {
        local_irq_restore(sys_flag);
        err("OAM not alloc tx connection");
        return -EIO;
    }
    if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
        g_cpu_to_wan_tx_desc_pos = 0;
    local_irq_restore(sys_flag);

    /*  load descriptor from memory */
    reg_desc = *desc;

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & 15) == 0, "reg_desc.dataptr (0x%#x) must be 4 DWORDS aligned", reg_desc.dataptr);
    ASSERT(*(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)) > 0x80000000, "pointer to skb is wrong, desc->dataptr = %08X, skb = %08X", reg_desc.dataptr, *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)));
    skb_to_free = get_skb_pointer(reg_desc.dataptr, 0);
    atm_free_tx_skb_vcc_delayed(skb_to_free);
    put_skb_to_dbg_pool(skb);

    /*  setup descriptor    */
    reg_desc.dataptr    = (u32)skb->data & 0x1FFFFFF0;  //  byte address
    reg_desc.byteoff    = (u32)skb->data & 15;
    reg_desc.datalen    = skb->len;
    reg_desc.mpoa_pt    = 1;
    reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
    reg_desc.pdu_type   = 1;    //  cell
    reg_desc.qid        = g_atm_priv_data.connection[conn].prio_queue_map[7];   //  expect priority '7' is highest priority
    reg_desc.own        = 0;
    reg_desc.c          = 0;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send_oam", reg_desc.qid, 1);

    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    adsl_led_flash();

    return 0;
}

static int ppe_change_qos(struct atm_vcc *vcc, struct atm_qos *qos, int flags)
{
    int conn;

    if ( vcc == NULL || qos == NULL )
        return -EINVAL;

    conn = find_vcc(vcc);
    if ( conn < 0 )
        return -EINVAL;

    set_qsb(vcc, qos, conn);

    return 0;
}

static void mpoa_setup(struct atm_vcc *vcc, int mpoa_type, int f_llc)
{
    int conn;
    u32 sw_tx_queue_table;
    int sw_tx_queue;

    conn = find_vcc(vcc);
    if ( conn < 0 )
        return;

    WRX_QUEUE_CONFIG(conn)->mpoa_type = mpoa_type;
    WRX_QUEUE_CONFIG(conn)->mpoa_mode = f_llc;

    for ( sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table, sw_tx_queue = 0;
          sw_tx_queue_table != 0;
          sw_tx_queue_table >>= 1, sw_tx_queue++ )
        if ( (sw_tx_queue_table & 0x01) )
            WTX_QUEUE_CONFIG(sw_tx_queue)->mpoa_mode = f_llc;

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    if ( mpoa_type == 3 || mpoa_type == 0 ) //  IPoA/EoA - BR2684
    {
        void **brvcc = (void **)vcc->user_back;

        ASSERT(brvcc[0] == (void *)vcc, "improper br2684_vcc structure");
        g_atm_priv_data.connection[conn].netdev = (struct net_device *)brvcc[1];
        dbg("debugging message: net_device of VCC (%d.%d) is got 0x%08x", (int)vcc->vpi, vcc->vci, (unsigned int)g_atm_priv_data.connection[conn].netdev);
    }
#endif
}

static void eth_setup(struct net_device *dev)
{
    u8 *ethaddr = MY_ETH0_ADDR;
    u32 val;
    int i;

#ifndef CONFIG_IFX_ETH_FRAMEWORK
  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    /*  hook network operations */
    dev->get_stats       = eth_get_stats;
    dev->open            = eth_open;
    dev->stop            = eth_stop;
    dev->hard_start_xmit = eth_hard_start_xmit;
    dev->set_mac_address = eth_set_mac_address;
    dev->do_ioctl        = eth_ioctl;
    dev->tx_timeout      = eth_tx_timeout;
  #else
    dev->netdev_ops      = &g_eth_netdev_ops;
  #endif
    dev->watchdog_timeo  = ETH_WATCHDOG_TIMEOUT;
#endif

    /*  read MAC address from the MAC table and put them into device    */
    for ( i = 0, val = 0; i < 6; i++ )
        val += dev->dev_addr[i];
    if ( val == 0 )
    {
        for ( i = 0, val = 0; i < 6; i++ )
            val += ethaddr[i];
        if ( val == 0 )
        {
            /*  ethaddr not set in u-boot   */
            dev->dev_addr[0] = 0x00;
            dev->dev_addr[1] = 0x20;
            dev->dev_addr[2] = 0xda;
            dev->dev_addr[3] = 0x86;
            dev->dev_addr[4] = 0x23;
            dev->dev_addr[5] = 0x74;
        }
        else
        {
            for ( i = 0; i < 6; i++ )
                dev->dev_addr[i] = ethaddr[i];
        }
    }
}

static struct net_device_stats *eth_get_stats(struct net_device *dev)
{
    int port;
    struct eth_priv_data *priv = ifx_eth_fw_netdev_priv(dev);

    port = get_eth_port(dev);

    priv->enets_igerr  += *ENETS_IGERR(port);
    priv->enets_igdrop += *ENETS_IGDROP(port);
    priv->enetf_egcol  += *ENETF_EGCOL(port);
    priv->enetf_egdrop += *ENETF_EGDROP(port);

    priv->stats.rx_packets = priv->rx_packets
                           + ITF_MIB_TBL(0)->ig_fast_rt_uni_udp_pkts
                           + ITF_MIB_TBL(0)->ig_fast_rt_uni_tcp_pkts
                           + ITF_MIB_TBL(0)->ig_fast_rt_mul_pkts
                           + ITF_MIB_TBL(0)->ig_fast_brg_uni_pkts
                           + ITF_MIB_TBL(0)->ig_fast_brg_mul_pkts;
    priv->stats.rx_bytes   = priv->rx_bytes
                           + ITF_MIB_TBL(0)->ig_fast_brg_bytes
                           + ITF_MIB_TBL(0)->ig_fast_rt_bytes;
    priv->stats.rx_errors  = priv->enets_igerr;
    priv->stats.rx_dropped = priv->rx_dropped
                           + priv->enets_igdrop
                           + ITF_MIB_TBL(0)->ig_drop_pkts;

    priv->stats.tx_packets = priv->tx_packets + ITF_MIB_TBL(0)->eg_fast_pkts;
    priv->stats.tx_bytes   = priv->tx_bytes + ITF_MIB_TBL(0)->eg_fast_bytes;
    priv->stats.tx_errors  = priv->tx_errors  + priv->enetf_egcol;
    priv->stats.tx_dropped = priv->tx_dropped + priv->enetf_egdrop;

    return &priv->stats;
}

static int eth_open(struct net_device *dev)
{
    down(&g_sem);

    MOD_INC_USE_COUNT;

#if defined(ENABLE_PROBE_TRANSCEIVER) && ENABLE_PROBE_TRANSCEIVER
    if ( !probe_transceiver(dev) )
    {
        printk("%s cannot work because of hardware problem\n", dev->name);
        MOD_DEC_USE_COUNT;
        up(&g_sem);
        return -1;
    }
#endif

    dbg("%s", dev->name);

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
    if ( (g_eth_priv_data[get_eth_port(dev)].fc_bit = netdev_register_fc(dev, eth_xon)) == 0 )
        printk("Hardware Flow Control register fails\n");
#endif

    turn_on_dma_rx(get_eth_port(dev));

#ifndef CONFIG_IFX_ETH_FRAMEWORK
    netif_start_queue(dev);
#endif

    up(&g_sem);
    return 0;
}

static int eth_stop(struct net_device *dev)
{
#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
    int port;
#endif

    down(&g_sem);

    turn_off_dma_rx(get_eth_port(dev));

#ifndef CONFIG_IFX_ETH_FRAMEWORK
    netif_stop_queue(dev);
#endif

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
    port = get_eth_port(dev);
    if ( g_eth_priv_data[port].fc_bit )
        netdev_unregister_fc(g_eth_priv_data[port].fc_bit);
#endif

    MOD_DEC_USE_COUNT;

    up(&g_sem);
    return 0;
}

static int eth_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int qid;

    if ( skb->priority >= NUM_ENTITY(g_eth_prio_queue_map[0]) )
        qid = g_eth_prio_queue_map[0][NUM_ENTITY(g_eth_prio_queue_map[0]) - 1];
    else
        qid = g_eth_prio_queue_map[0][skb->priority];

    return eth0_xmit(skb, qid);
}

static int eth_set_mac_address(struct net_device *dev, void *p)
{
    struct sockaddr *addr = (struct sockaddr *)p;
#ifdef ROUT_MAC_CFG_TBL
    u32 addr1, addr2;
    int i;
#endif

    dbg("%s: change MAC from %02X:%02X:%02X:%02X:%02X:%02X to %02X:%02X:%02X:%02X:%02X:%02X", dev->name,
        (u32)dev->dev_addr[0] & 0xFF, (u32)dev->dev_addr[1] & 0xFF, (u32)dev->dev_addr[2] & 0xFF, (u32)dev->dev_addr[3] & 0xFF, (u32)dev->dev_addr[4] & 0xFF, (u32)dev->dev_addr[5] & 0xFF,
        (u32)addr->sa_data[0] & 0xFF, (u32)addr->sa_data[1] & 0xFF, (u32)addr->sa_data[2] & 0xFF, (u32)addr->sa_data[3] & 0xFF, (u32)addr->sa_data[4] & 0xFF, (u32)addr->sa_data[5] & 0xFF);

#ifdef ROUT_MAC_CFG_TBL
    addr1 = (((u32)dev->dev_addr[0] & 0xFF) << 24) | (((u32)dev->dev_addr[1] & 0xFF) << 16) | (((u32)dev->dev_addr[2] & 0xFF) << 8) | ((u32)dev->dev_addr[3] & 0xFF);
    addr2 = (((u32)dev->dev_addr[4] & 0xFF) << 24) | (((u32)dev->dev_addr[5] & 0xFF) << 16);
    for ( i = 0; i < 16; i++ )
        if ( ROUT_MAC_CFG_TBL(i)[0] == addr1 && ROUT_MAC_CFG_TBL(i)[1] == addr2 )
        {
            ROUT_MAC_CFG_TBL(i)[0] = (((u32)addr->sa_data[0] & 0xFF) << 24) | (((u32)addr->sa_data[1] & 0xFF) << 16) | (((u32)addr->sa_data[2] & 0xFF) << 8) | ((u32)addr->sa_data[3] & 0xFF);
            ROUT_MAC_CFG_TBL(i)[1] = (((u32)addr->sa_data[4] & 0xFF) << 24) | (((u32)addr->sa_data[5] & 0xFF) << 16);
            break;
        }
#endif

    my_memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

    return 0;
}

static int eth_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    int port;

    port = get_eth_port(dev);
    switch ( cmd )
    {
    case SIOCETHTOOL:
        return ethtool_ioctl(dev, ifr);
    case SET_VLAN_COS:
        {
            struct vlan_cos_req vlan_cos_req;

            if ( copy_from_user(&vlan_cos_req, ifr->ifr_data, sizeof(struct vlan_cos_req)) )
                return -EFAULT;
            set_vlan_cos(&vlan_cos_req);
        }
        break;
    case SET_DSCP_COS:
        {
            struct dscp_cos_req dscp_cos_req;

            if ( copy_from_user(&dscp_cos_req, ifr->ifr_data, sizeof(struct dscp_cos_req)) )
                return -EFAULT;
            set_dscp_cos(&dscp_cos_req);
        }
        break;
    case ENABLE_VLAN_CLASSIFICATION:
        *ENETS_COS_CFG(port) |= ENETS_COS_CFG_VLAN_SET;    break;
    case DISABLE_VLAN_CLASSIFICATION:
        *ENETS_COS_CFG(port) &= ENETS_COS_CFG_VLAN_CLEAR;  break;
    case ENABLE_DSCP_CLASSIFICATION:
        *ENETS_COS_CFG(port) |= ENETS_COS_CFG_DSCP_SET;    break;
    case DISABLE_DSCP_CLASSIFICATION:
        *ENETS_COS_CFG(port) &= ENETS_COS_CFG_DSCP_CLEAR;  break;
    case VLAN_CLASS_FIRST:
        *ENETS_CFG(port) &= ENETS_CFG_FTUC_CLEAR;          break;
    case VLAN_CLASS_SECOND:
        *ENETS_CFG(port) |= ENETS_CFG_VL2_SET;             break;
    case PASS_UNICAST_PACKETS:
        *ENETS_CFG(port) &= ENETS_CFG_FTUC_CLEAR;          break;
    case FILTER_UNICAST_PACKETS:
        *ENETS_CFG(port) |= ENETS_CFG_FTUC_SET;            break;
    case KEEP_BROADCAST_PACKETS:
        *ENETS_CFG(port) &= ENETS_CFG_DPBC_CLEAR;          break;
    case DROP_BROADCAST_PACKETS:
        *ENETS_CFG(port) |= ENETS_CFG_DPBC_SET;            break;
    case KEEP_MULTICAST_PACKETS:
        *ENETS_CFG(port) &= ENETS_CFG_DPMC_CLEAR;          break;
    case DROP_MULTICAST_PACKETS:
        *ENETS_CFG(port) |= ENETS_CFG_DPMC_SET;
    case ETH_MAP_PKT_PRIO_TO_Q:
        {
            struct ppe_prio_q_map cmd;

            if ( copy_from_user(&cmd, ifr->ifr_data, sizeof(cmd)) )
                return -EFAULT;

            if ( cmd.qid < 0 || cmd.qid > 3 || cmd.qid == 2 /* reserved for fast path */
                || cmd.pkt_prio < 0 || cmd.pkt_prio >= NUM_ENTITY(g_eth_prio_queue_map[0]) )
                return -EINVAL;

            g_eth_prio_queue_map[0][cmd.pkt_prio] = cmd.qid;
        }
        break;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
}

static void eth_tx_timeout(struct net_device *dev)
{
    struct eth_priv_data *priv = ifx_eth_fw_netdev_priv(dev);

    priv->tx_errors++;

    g_dma_device->tx_chan[0]->disable_irq(g_dma_device->tx_chan[0]);

    netif_wake_queue(dev);

    return;
}

static INLINE int get_eth_port(struct net_device *dev)
{
    return 0;
}

static INLINE int eth0_xmit(struct sk_buff *skb, int ch)
{
    int len;
    struct eth_priv_data *priv = ifx_eth_fw_netdev_priv(g_eth_net_dev[0]);

    if ( g_stop_datapath != 0 )
        goto ETH_XMIT_DROP;

    len = skb->len <= ETH_MIN_TX_PACKET_LENGTH ? ETH_MIN_TX_PACKET_LENGTH : skb->len;

    ASSERT(ch == 0, "Amazon-SE acceleration package use channel 0 only for CPU path traffic!");
    g_dma_device->current_tx_chan = ch;

    dump_skb(skb, DUMP_SKB_LEN, "eth0_xmit", ch, 1);

    g_eth_net_dev[0]->trans_start = jiffies;

    priv->tx_packets++;
    priv->tx_bytes += len;

    put_skb_to_dbg_pool(skb);
    if ( dma_device_write(g_dma_device,
                          skb->data,
                          len,
                          skb)
         != len )
        goto ETH_XMIT_DROP;

    return 0;

ETH_XMIT_DROP:
    dev_kfree_skb_any(skb);
    priv->tx_dropped++;
    return -1;
}

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
static void eth_xon(struct net_device *dev)
{
    unsigned long sys_flag;
    struct eth_priv_data *priv = ifx_eth_fw_netdev_priv(dev);

    clear_bit(priv->fc_bit, &netdev_fc_xoff);
    //  enable ENET0 store
    local_irq_save(sys_flag);
    *ETOP_CFG |= 1 << 6;
    local_irq_restore(sys_flag);
}
#endif

#if defined(ENABLE_PROBE_TRANSCEIVER) && ENABLE_PROBE_TRANSCEIVER
/*
 *  Description:
 *    Setup ethernet hardware in init process.
 *  Input:
 *    dev --- struct net_device *, device to be setup.
 *  Output:
 *    int --- 0:    Success
 *            else: Error Code (-EIO, link is not OK)
 */
static INLINE int probe_transceiver(struct net_device *dev)
{
    int port;

    port = get_eth_port(dev);

    if ( g_eth_priv_data[port].mii_mode == EPHY_MODE )
        g_eth_priv_data[port].mdio_phy_addr = EPHY_ADDRESS;
    else
    {
        g_eth_priv_data[port].mdio_phy_addr = PHY0_ADDR;

        *ENETS_MAC_DA0(port) = (((u32)dev->dev_addr[0] & 0xFF) << 24) | (((u32)dev->dev_addr[1] & 0xFF) << 16) | (((u32)dev->dev_addr[2] & 0xFF) << 8) | ((u32)dev->dev_addr[3] & 0xFF);
        *ENETS_MAC_DA1(port) = (((u32)dev->dev_addr[4] & 0xFF) << 24) | (((u32)dev->dev_addr[5] & 0xFF) << 16);

        if ( !ENET_MAC_CFG_LINK(port) )
        {
            *ETOP_MDIO_CFG = (*ETOP_MDIO_CFG & ~ETOP_MDIO_CFG_MASK(port))
                             | ETOP_MDIO_CFG_SMRST(port)
                             | ETOP_MDIO_CFG_PHYA(port, port)
                             | ETOP_MDIO_CFG_UMM(port, 1);

            udelay(ETOP_MDIO_DELAY);

            if ( !ENET_MAC_CFG_LINK(port) )
                return -EIO;
        }
    }

    return 0;
}
#endif

/*
 *  Description:
 *    Handle ioctl command SIOCETHTOOL.
 *  Input:
 *    dev --- struct net_device *, device responsing to the command.
 *    ifr --- struct ifreq *, interface request structure to pass parameters
 *            or result.
 *  Output:
 *    int --- 0:    Success
 *            else: Error Code (-EFAULT, -EOPNOTSUPP)
 */
static INLINE int ethtool_ioctl(struct net_device *dev, struct ifreq *ifr)
{
    int ret = 0;
    int port;
    struct ethtool_cmd cmd;
    u32 data;
    struct eth_priv_data *priv = ifx_eth_fw_netdev_priv(dev);

    if ( copy_from_user(&cmd, ifr->ifr_data, sizeof(cmd)) )
        return -EFAULT;

    port = get_eth_port(dev);

    switch ( cmd.cmd )
    {
    case ETHTOOL_GSET:      /*  get hardware information        */
        {
            memset(&cmd, 0, sizeof(cmd));

            cmd.supported   = SUPPORTED_Autoneg | SUPPORTED_TP | SUPPORTED_MII |
                              SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full |
                              SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full;
            cmd.port        = PORT_MII;
            if ( priv->mii_mode == EPHY_MODE )
            {
                cmd.transceiver = XCVR_INTERNAL;
                cmd.phy_address = EPHY_ADDRESS;

                data = __ephy_read_mdio_reg(EPHY_ADDRESS, EPHY_SPECIFIC_STATUS_REG);
                cmd.reserved[0] = (data & EPHY_SPECIFIC_STATUS_LINK_UP) ? LINK_UP : LINK_DOWN;
                cmd.speed       = (data & EPHY_SPECIFIC_STATUS_SPEED_MASK) ? SPEED_100 : SPEED_10;
                cmd.duplex      = (data & EPHY_SPECIFIC_STATUS_DUPLEX_MASK) ? DUPLEX_FULL : DUPLEX_HALF;

                data = __ephy_read_mdio_reg(EPHY_ADDRESS, EPHY_MDIO_BASE_CONTROL_REG);
                cmd.autoneg     = (data & EPHY_AUTO_NEG_ENABLE_MASK) ? AUTONEG_ENABLE : AUTONEG_DISABLE;

                data = __ephy_read_mdio_reg(EPHY_ADDRESS, EPHY_MDIO_ADVERTISMENT_REG);
                if ( (data & EPHY_MDIO_ADVERT_100_FD) )
                    cmd.advertising |=  ADVERTISED_100baseT_Full;
                else
                    cmd.advertising &=  ~ADVERTISED_100baseT_Full;
                if ( (data & EPHY_MDIO_ADVERT_100_HD) )
                    cmd.advertising |=  ADVERTISED_100baseT_Half;
                else
                    cmd.advertising &=  ~ADVERTISED_100baseT_Half;
                if ( (data & EPHY_MDIO_ADVERT_10_FD) )
                    cmd.advertising |=  ADVERTISED_10baseT_Full;
                else
                    cmd.advertising &=  ~ADVERTISED_10baseT_Full;
                if ( (data & EPHY_MDIO_ADVERT_10_HD) )
                    cmd.advertising |=  ADVERTISED_10baseT_Half;
                else
                    cmd.advertising &=  ~ADVERTISED_10baseT_Half;
            }
            else
            {
                cmd.transceiver = XCVR_EXTERNAL;
                cmd.phy_address = priv->mdio_phy_addr;

                cmd.speed       = ENET_MAC_CFG_SPEED(port) ? SPEED_100 : SPEED_10;
                cmd.duplex      = ENET_MAC_CFG_DUPLEX(port) ? DUPLEX_FULL : DUPLEX_HALF;

                if ( (*ETOP_MDIO_CFG & ETOP_MDIO_CFG_UMM(port, 1)) )
                {
                    /*  auto negotiate  */
                    cmd.autoneg = AUTONEG_ENABLE;
                    cmd.advertising |= ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
                                       ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full;
                }
                else
                {
                    cmd.autoneg = AUTONEG_DISABLE;
                    cmd.advertising &= ~(ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
                                         ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full);
                }
            }

            if ( copy_to_user(ifr->ifr_data, &cmd, sizeof(cmd)) )
                return -EFAULT;
        }
        break;
    case ETHTOOL_SSET:      /*  force the speed and duplex mode */
        {
            if ( !capable(CAP_NET_ADMIN) )
                return -EPERM;

            ret = set_mac(dev, cmd.speed, cmd.duplex, cmd.autoneg);
        }
        break;
    case ETHTOOL_GDRVINFO:  /*  get driver information          */
        {
            struct ethtool_drvinfo info;
            char str[32];

            memset(&info, 0, sizeof(info));
            strncpy(info.driver, "AmazonSE Eth Driver (A4)", sizeof(info.driver) - 1);
            sprintf(str, "%d.%d.%d.%d.%d.%d", (int)FW_VER_ID->family, (int)FW_VER_ID->fwtype, (int)FW_VER_ID->interface, (int)FW_VER_ID->fwmode, (int)FW_VER_ID->major, (int)FW_VER_ID->minor);
            strncpy(info.fw_version, str, sizeof(info.fw_version) - 1);
            strncpy(info.bus_info, "N/A", sizeof(info.bus_info) - 1);
            info.regdump_len = 0;
            info.eedump_len = 0;
            info.testinfo_len = 0;
            if ( copy_to_user(ifr->ifr_data, &info, sizeof(info)) )
                return -EFAULT;
        }
        break;
    case ETHTOOL_NWAY_RST:  /*  restart auto negotiation        */
        if ( priv->mii_mode == REV_MII_MODE )
            *ETOP_MDIO_CFG |= ETOP_MDIO_CFG_SMRST(port) | ETOP_MDIO_CFG_UMM(port, 1);
        else if ( priv->mii_mode == EPHY_MODE )
        {
            *ETOP_MDIO_ACC = EPHY_RA_MASK | EPHY_WRITE_MASK | (EPHY_ADDRESS << EPHY_ADDRESS_SHIFT) | (EPHY_MDIO_BASE_CONTROL_REG << EPHY_REG_ADDRESS_SHIFT) | EPHY_RESET;   //0x83e08000;
            __ephy_auto_negotiate(EPHY_ADDRESS);    //0x08
        }
        break;
    default:
        return -EOPNOTSUPP;
    }

    return ret;
}

/*
 *  Description:
 *    Specify ETOP ingress VLAN priority's class of service mapping.
 *  Input:
 *    req --- struct vlan_cos_req *, pass parameters such as priority and class
 *            of service mapping.
 *  Output:
 *    none
 */
static INLINE void set_vlan_cos(struct vlan_cos_req *req)
{
    *ETOP_IG_VLAN_COS = SET_BITS(*ETOP_IG_VLAN_COS, (req->pri << 1) + 1, req->pri << 1, req->cos_value);
}

/*
 *  Description:
 *    Specify ETOP ingress VLAN differential service control protocol's class of
 *    service mapping.
 *  Input:
 *    req --- struct dscp_cos_req *, pass parameters such as differential
 *            service control protocol and class of service mapping.
 *  Output:
 *    none
 */
static INLINE void set_dscp_cos(struct dscp_cos_req *req)
{
    *ETOP_IG_DSCP_COSx(req->dscp >> 4) = SET_BITS(*ETOP_IG_DSCP_COSx(req->dscp >> 4), ((req->dscp & 0x0F) << 1) + 1, (req->dscp & 0x0F) << 1, req->cos_value);
}

static INLINE u32 __ephy_read_mdio_reg(int phy_addr, int phy_reg_num)
{
    u32 phy_reg_val;

    *ETOP_MDIO_ACC = (RA_READ_ENABLE << 30)
                    | ((phy_addr & EPHY_ADDRESS_MASK) << EPHY_ADDRESS_SHIFT)
                    | ((phy_reg_num & EPHY_REG_MASK) << EPHY_REG_ADDRESS_SHIFT);

    while ( *ETOP_MDIO_ACC & 0x80000000 );  //fix me ...

    phy_reg_val = *ETOP_MDIO_ACC & EPHY_DATA_MASK;
    return phy_reg_val;
}

static INLINE int __ephy_write_mdio_reg(int phy_addr, int phy_reg_num, u32 phy_data)
{
    *ETOP_MDIO_ACC = (RA_WRITE_ENABLE << 30)
                    | ((phy_addr & EPHY_ADDRESS_MASK) << EPHY_ADDRESS_SHIFT)
                    | ((phy_reg_num & EPHY_REG_MASK) << EPHY_REG_ADDRESS_SHIFT)
                    | (phy_data & EPHY_DATA_MASK);

    while ( *ETOP_MDIO_ACC & 0x80000000 );  //fix me ..

    return 0;
}

static INLINE int __ephy_auto_negotiate(int phy_addr)
{
    u32 phy_reg_val;

    phy_reg_val = ephy_read_mdio_reg(phy_addr, EPHY_MDIO_BASE_CONTROL_REG);
    ephy_write_mdio_reg(phy_addr,
                        EPHY_MDIO_BASE_CONTROL_REG,
                        phy_reg_val | EPHY_RESTART_AUTO_NEGOTIATION | EPHY_AUTO_NEGOTIATION_ENABLE | EPHY_RESET);

    return 0;
}

static INLINE int set_mac(struct net_device *dev, u32 speed, u32 duplex, u32 autoneg)
{
    int port;
    struct eth_priv_data *priv = ifx_eth_fw_netdev_priv(dev);

    port = get_eth_port(dev);

    if ( autoneg == AUTONEG_ENABLE )
    {
        /*set property and start autonegotiation */
        /*have to set mdio advertisement register and restart autonegotiation */
        /*which is a very rare case, put it to future development if necessary. */
        *ETOP_MDIO_ACC = EPHY_RA_MASK | EPHY_WRITE_MASK | (EPHY_ADDRESS << EPHY_ADDRESS_SHIFT) | (EPHY_MDIO_BASE_CONTROL_REG << EPHY_REG_ADDRESS_SHIFT) | EPHY_RESET;   //0x83e08000;
        __ephy_auto_negotiate(EPHY_ADDRESS);    //0x08
    }
    else    /*autoneg==AUTONEG_DISABLE or -1 */
    {
        /*set property without autonegotiation */
        if ( priv->mii_mode == EPHY_MODE )
        {
            u32 data = 0;
            *ETOP_MDIO_ACC = EPHY_RA_MASK | EPHY_WRITE_MASK | (EPHY_ADDRESS << EPHY_ADDRESS_SHIFT) | (EPHY_MDIO_BASE_CONTROL_REG << EPHY_REG_ADDRESS_SHIFT | EPHY_RESET);

            data = __ephy_read_mdio_reg(0xff, EPHY_MDIO_BASE_CONTROL_REG);

            if ( speed == SPEED_10 )
                data &= ~EPHY_LINK_SPEED_MASK;
            else
                data |= EPHY_LINK_SPEED_MASK;

            if ( duplex == DUPLEX_HALF )
                data &= ~EPHY_DUPLEX_MASK;
            else
                data |= EPHY_DUPLEX_MASK;

            __ephy_write_mdio_reg(EPHY_ADDRESS, EPHY_MDIO_BASE_CONTROL_REG, (data | EPHY_RESET));
        }
        else
        {
            /*  set property without autonegotiation    */
            *ETOP_MDIO_CFG &= ~ETOP_MDIO_CFG_UMM(port, 1);

            /*  set speed   */
            if ( speed == SPEED_10 )
                ENET_MAC_CFG_SPEED_10M(port);
            else if ( speed == SPEED_100 )
                ENET_MAC_CFG_SPEED_100M(port);

            /*  set duplex  */
            if ( duplex == DUPLEX_HALF )
                ENET_MAC_CFG_DUPLEX_HALF(port);
            else if ( duplex == DUPLEX_FULL )
                ENET_MAC_CFG_DUPLEX_FULL(port);

            ENET_MAC_CFG_LINK_OK(port);
        }
    }
    return 0;
}

/*
 *  Description:
 *    Add a 32-bit value to 64-bit value, and put result in a 64-bit variable.
 *  Input:
 *    opt1 --- ppe_u64_t, first operand, a 64-bit unsigned integer value
 *    opt2 --- u32, second operand, a 32-bit unsigned integer value
 *    ret  --- ppe_u64_t, pointer to a variable to hold result
 *  Output:
 *    none
 */
static INLINE void u64_add_u32(ppe_u64_t opt1, u32 opt2,ppe_u64_t *ret)
{
    ret->l = opt1.l + opt2;
    if ( ret->l < opt1.l || ret->l < opt2 )
        ret->h++;
}

static INLINE void adsl_led_flash(void)
{
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK
  #ifdef CONFIG_IFX_LED
    if ( g_data_led_trigger != NULL )
        ifx_led_trigger_activate(g_data_led_trigger);
  #endif
#else
    ifx_mei_atm_led_blink();
#endif
}

//  add one lower priority queue
static int sw_tx_queue_add(int conn)
{
    int ret;
    int new_sw_tx_queue;
    u32 new_sw_tx_queue_table;
    int sw_tx_queue_to_virt_queue_map[ATM_SW_TX_QUEUE_NUMBER] = {0};
    int virt_queue_to_sw_tx_queue_map[8] = {0};
    int num_of_virt_queue;
    u32 sw_tx_queue_table;
    int sw_tx_queue;
    unsigned int dsl_tx_queue_mapping;
    int virt_queue;
    struct wtx_queue_config wtx_queue_config;
    int f_got_wtx_queue_config = 0;
    int i;

    if ( !in_interrupt() )
        down(&g_sem);

    for ( new_sw_tx_queue = 0; new_sw_tx_queue < g_atm_priv_data.max_sw_tx_queues; new_sw_tx_queue++ )
        if ( !(g_atm_priv_data.sw_tx_queue_table & (1 << new_sw_tx_queue)) )
            break;
    if ( new_sw_tx_queue == g_atm_priv_data.max_sw_tx_queues )
    {
        ret = -EINVAL;
        goto SW_TX_QUEUE_ADD_EXIT;
    }

    new_sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table | (1 << new_sw_tx_queue);

    for ( sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table, sw_tx_queue = virt_queue = 0;
          sw_tx_queue_table != 0;
          sw_tx_queue_table >>= 1, sw_tx_queue++ )
        if ( (sw_tx_queue_table & 0x01) )
        {
            if ( !f_got_wtx_queue_config )
            {
                wtx_queue_config = *WTX_QUEUE_CONFIG(sw_tx_queue);
                f_got_wtx_queue_config = 1;
            }

            sw_tx_queue_to_virt_queue_map[sw_tx_queue] = virt_queue++;
            WTX_QUEUE_CONFIG(sw_tx_queue)->same_vc_qmap = new_sw_tx_queue_table & ~(1 << sw_tx_queue);
        }

    wtx_queue_config.same_vc_qmap = new_sw_tx_queue_table & ~(1 << new_sw_tx_queue);
    *WTX_QUEUE_CONFIG(new_sw_tx_queue) = wtx_queue_config;

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    g_tx_queue_to_conn[new_sw_tx_queue] = conn;
#endif

    for ( num_of_virt_queue = 0, sw_tx_queue_table = new_sw_tx_queue_table, sw_tx_queue = 0;
          num_of_virt_queue < NUM_ENTITY(virt_queue_to_sw_tx_queue_map) && sw_tx_queue_table != 0;
          sw_tx_queue_table >>= 1, sw_tx_queue++ )
        if ( (sw_tx_queue_table & 0x01) )
            virt_queue_to_sw_tx_queue_map[num_of_virt_queue++] = sw_tx_queue;

    g_atm_priv_data.sw_tx_queue_table |= 1 << new_sw_tx_queue;
    g_atm_priv_data.connection[conn].sw_tx_queue_table = new_sw_tx_queue_table;

    for ( i = 0, dsl_tx_queue_mapping = 0; i < 8; i++ )
    {
        sw_tx_queue = g_atm_priv_data.connection[conn].prio_queue_map[i];
        virt_queue = sw_tx_queue_to_virt_queue_map[sw_tx_queue];
        sw_tx_queue = virt_queue_to_sw_tx_queue_map[virt_queue];
        g_atm_priv_data.connection[conn].prio_queue_map[i] = sw_tx_queue;
        dsl_tx_queue_mapping |= sw_tx_queue << (i << 2);
    }
    *DSL_TX_QUEUE_MAPPING(conn) = dsl_tx_queue_mapping;

    ret = 0;

SW_TX_QUEUE_ADD_EXIT:

    if ( !in_interrupt() )
        up(&g_sem);

    return ret;
}

//  delete lowest priority queue
static int sw_tx_queue_del(int conn)
{
    int ret;
    int rem_sw_tx_queue;
    u32 new_sw_tx_queue_table;
    u32 sw_tx_queue_table;
    int sw_tx_queue;
    int i;

    if ( !in_interrupt() )
        down(&g_sem);

    for ( sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table, rem_sw_tx_queue = g_atm_priv_data.max_sw_tx_queues - 1;
          (sw_tx_queue_table & (1 << (g_atm_priv_data.max_sw_tx_queues - 1))) == 0;
          sw_tx_queue_table <<= 1, rem_sw_tx_queue-- );

    if ( g_atm_priv_data.connection[conn].sw_tx_queue_table == (1 << rem_sw_tx_queue) )
    {
        ret = -EIO;
        goto SW_TX_QUEUE_DEL_EXIT;
    }

    new_sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table & ~(1 << rem_sw_tx_queue);

    for ( sw_tx_queue_table = new_sw_tx_queue_table, sw_tx_queue = g_atm_priv_data.max_sw_tx_queues - 1;
          (sw_tx_queue_table & (1 << (g_atm_priv_data.max_sw_tx_queues - 1))) == 0;
          sw_tx_queue_table <<= 1, sw_tx_queue-- );

    for ( i = 0; i < 8; i++ )
        if ( g_atm_priv_data.connection[conn].prio_queue_map[i] == rem_sw_tx_queue )
        {
            g_atm_priv_data.connection[conn].prio_queue_map[i] = sw_tx_queue;
            *DSL_TX_QUEUE_MAPPING(conn) = (*DSL_TX_QUEUE_MAPPING(conn) & ~(0x0F << (i << 2))) | (sw_tx_queue << (i << 2));
        }

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    g_tx_queue_to_conn[rem_sw_tx_queue] = -1;
#endif

    g_atm_priv_data.sw_tx_queue_table &= ~(1 << rem_sw_tx_queue);
    g_atm_priv_data.connection[conn].sw_tx_queue_table = new_sw_tx_queue_table;

    for ( sw_tx_queue_table = new_sw_tx_queue_table, sw_tx_queue = 0;
          sw_tx_queue_table != 0;
          sw_tx_queue_table >>= 1, sw_tx_queue++ )
        if ( (sw_tx_queue_table & 0x01) )
            WTX_QUEUE_CONFIG(sw_tx_queue)->same_vc_qmap = new_sw_tx_queue_table & ~(1 << sw_tx_queue);

    ret = 0;

SW_TX_QUEUE_DEL_EXIT:

    if ( !in_interrupt() )
        up(&g_sem);

    return ret;
}

static int sw_tx_prio_to_queue(int conn, int prio, int queue, int *p_num_of_virt_queue)
{
    int virt_queue_to_sw_tx_queue_map[8] = {0};
    int num_of_virt_queue;
    u32 sw_tx_queue_table;
    int sw_tx_queue;

    if ( conn < 0 )
        return -EINVAL;

    if ( !in_interrupt() )
        down(&g_sem);

    for ( num_of_virt_queue = 0, sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table, sw_tx_queue = 0;
          num_of_virt_queue < NUM_ENTITY(virt_queue_to_sw_tx_queue_map) && sw_tx_queue_table != 0;
          sw_tx_queue_table >>= 1, sw_tx_queue++ )
        if ( (sw_tx_queue_table & 0x01) )
            virt_queue_to_sw_tx_queue_map[num_of_virt_queue++] = sw_tx_queue;

    if ( p_num_of_virt_queue != NULL )
        *p_num_of_virt_queue = num_of_virt_queue;

    if ( prio < 0 || queue < 0 || queue >= num_of_virt_queue )
    {
        if ( !in_interrupt() )
            up(&g_sem);
        return -EINVAL;
    }

    g_atm_priv_data.connection[conn].prio_queue_map[prio] = virt_queue_to_sw_tx_queue_map[queue];
    *DSL_TX_QUEUE_MAPPING(conn) = (*DSL_TX_QUEUE_MAPPING(conn) & ~(0x0F << (prio << 2))) | (virt_queue_to_sw_tx_queue_map[queue] << (prio << 2));

    if ( !in_interrupt() )
        up(&g_sem);

    return 0;
}

/*
 *  Description:
 *    Setup QSB queue.
 *  Input:
 *    vcc        --- struct atm_vcc *, structure of an opened connection
 *    qos        --- struct atm_qos *, QoS parameter of the connection
 *    connection --- unsigned int, QSB queue ID, which is same as connection ID
 *  Output:
 *    none
 */
static void set_qsb(struct atm_vcc *vcc, struct atm_qos *qos, unsigned int connection)
{
    u32 qsb_clk = ifx_get_fpi_hz();
    union qsb_queue_parameter_table qsb_queue_parameter_table = {{0}};
    union qsb_queue_vbr_parameter_table qsb_queue_vbr_parameter_table = {{0}};
    u32 tmp;

    connection += QSB_QUEUE_NUMBER_BASE;    //  qsb qid = firmware qid + 1

#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
    {
        static char *str_traffic_class[9] = {
            "ATM_NONE",
            "ATM_UBR",
            "ATM_CBR",
            "ATM_VBR",
            "ATM_ABR",
            "ATM_ANYCLASS",
            "ATM_VBR_RT",
            "ATM_UBR_PLUS",
            "ATM_MAX_PCR"
        };

        unsigned char traffic_class = qos->txtp.traffic_class;
        int max_pcr = qos->txtp.max_pcr;
        int pcr = qos->txtp.pcr;
        int min_pcr = qos->txtp.min_pcr;
  #if !defined(DISABLE_VBR) || !DISABLE_VBR
        int scr = qos->txtp.scr;
        int mbs = qos->txtp.mbs;
        int cdv = qos->txtp.cdv;
  #endif

        printk("TX QoS\n");

        printk("  traffic class : ");
        if ( traffic_class == (unsigned char)ATM_MAX_PCR )
            printk("ATM_MAX_PCR\n");
        else if ( traffic_class > ATM_UBR_PLUS )
            printk("Unknown Class\n");
        else
            printk("%s\n", str_traffic_class[traffic_class]);

        printk("  max pcr       : %d\n", max_pcr);
        printk("  desired pcr   : %d\n", pcr);
        printk("  min pcr       : %d\n", min_pcr);

  #if !defined(DISABLE_VBR) || !DISABLE_VBR
        printk("  sustained rate: %d\n", scr);
        printk("  max burst size: %d\n", mbs);
        printk("  cell delay var: %d\n", cdv);
  #endif
    }
#endif    //  defined(DEBUG_QOS) && DEBUG_QOS

    /*
     *  Peak Cell Rate (PCR) Limiter
     */
    if ( qos->txtp.max_pcr == 0 )
        qsb_queue_parameter_table.bit.tp = 0;   /*  disable PCR limiter */
    else
    {
        /*  peak cell rate would be slightly lower than requested [maximum_rate / pcr = (qsb_clock / 8) * (time_step / 4) / pcr] */
        tmp = ((qsb_clk * qsb_tstep) >> 5) / qos->txtp.max_pcr + 1;
        /*  check if overflow takes place   */
        qsb_queue_parameter_table.bit.tp = tmp > QSB_TP_TS_MAX ? QSB_TP_TS_MAX : tmp;
    }

    //  A funny issue. Create two PVCs, one UBR and one UBR with max_pcr.
    //  Send packets to these two PVCs at same time, it trigger strange behavior.
    //  In A1, RAM from 0x80000000 to 0x0x8007FFFF was corrupted with fixed pattern 0x00000000 0x40000000.
    //  In A4, PPE firmware keep emiting unknown cell and do not respond to driver.
    //  To work around, create UBR always with max_pcr.
    //  If user want to create UBR without max_pcr, we give a default one larger than line-rate.
    if ( qos->txtp.traffic_class == ATM_UBR && qsb_queue_parameter_table.bit.tp == 0 )
    {
        int port = g_atm_priv_data.connection[connection - QSB_QUEUE_NUMBER_BASE].port;
        unsigned int max_pcr = g_atm_priv_data.port[port].tx_max_cell_rate + 1000;

        tmp = ((qsb_clk * qsb_tstep) >> 5) / max_pcr + 1;
        if ( tmp > QSB_TP_TS_MAX )
            tmp = QSB_TP_TS_MAX;
        else if ( tmp < 1 )
            tmp = 1;
        qsb_queue_parameter_table.bit.tp = tmp;
    }

    /*
     *  Weighted Fair Queueing Factor (WFQF)
     */
    switch ( qos->txtp.traffic_class )
    {
    case ATM_CBR:
    case ATM_VBR_RT:
        /*  real time queue gets weighted fair queueing bypass  */
        qsb_queue_parameter_table.bit.wfqf = 0;
        break;
    case ATM_VBR_NRT:
    case ATM_UBR_PLUS:
        /*  WFQF calculation here is based on virtual cell rates, to reduce granularity for high rates  */
        /*  WFQF is maximum cell rate / garenteed cell rate                                             */
        /*  wfqf = qsb_minimum_cell_rate * QSB_WFQ_NONUBR_MAX / requested_minimum_peak_cell_rate        */
        if ( qos->txtp.min_pcr == 0 )
            qsb_queue_parameter_table.bit.wfqf = QSB_WFQ_NONUBR_MAX;
        else
        {
            tmp = QSB_GCR_MIN * QSB_WFQ_NONUBR_MAX / qos->txtp.min_pcr;
            if ( tmp == 0 )
                qsb_queue_parameter_table.bit.wfqf = 1;
            else if ( tmp > QSB_WFQ_NONUBR_MAX )
                qsb_queue_parameter_table.bit.wfqf = QSB_WFQ_NONUBR_MAX;
            else
                qsb_queue_parameter_table.bit.wfqf = tmp;
        }
        break;
    default:
    case ATM_UBR:
        qsb_queue_parameter_table.bit.wfqf = QSB_WFQ_UBR_BYPASS;
    }

    /*
     *  Sustained Cell Rate (SCR) Leaky Bucket Shaper VBR.0/VBR.1
     */
    if ( qos->txtp.traffic_class == ATM_VBR_RT || qos->txtp.traffic_class == ATM_VBR_NRT )
    {
#if defined(DISABLE_VBR) && DISABLE_VBR
        /*  disable shaper  */
        qsb_queue_vbr_parameter_table.bit.taus = 0;
        qsb_queue_vbr_parameter_table.bit.ts = 0;
#else
        if ( qos->txtp.scr == 0 )
        {
            /*  disable shaper  */
            qsb_queue_vbr_parameter_table.bit.taus = 0;
            qsb_queue_vbr_parameter_table.bit.ts = 0;
        }
        else
        {
            /*  Cell Loss Priority  (CLP)   */
            if ( (vcc->atm_options & ATM_ATMOPT_CLP) )
                /*  CLP1    */
                qsb_queue_parameter_table.bit.vbr = 1;
            else
                /*  CLP0    */
                qsb_queue_parameter_table.bit.vbr = 0;
            /*  Rate Shaper Parameter (TS) and Burst Tolerance Parameter for SCR (tauS) */
            tmp = ((qsb_clk * qsb_tstep) >> 5) / qos->txtp.scr + 1;
            qsb_queue_vbr_parameter_table.bit.ts = tmp > QSB_TP_TS_MAX ? QSB_TP_TS_MAX : tmp;
            tmp = (qos->txtp.mbs - 1) * (qsb_queue_vbr_parameter_table.bit.ts - qsb_queue_parameter_table.bit.tp) / 64;
            if ( tmp == 0 )
                qsb_queue_vbr_parameter_table.bit.taus = 1;
            else if ( tmp > QSB_TAUS_MAX )
                qsb_queue_vbr_parameter_table.bit.taus = QSB_TAUS_MAX;
            else
                qsb_queue_vbr_parameter_table.bit.taus = tmp;
        }
#endif
    }
    else
    {
        qsb_queue_vbr_parameter_table.bit.taus = 0;
        qsb_queue_vbr_parameter_table.bit.ts = 0;
    }

    /*  Queue Parameter Table (QPT) */
    *QSB_RTM   = QSB_RTM_DM_SET(QSB_QPT_SET_MASK);
    *QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_parameter_table.dword);
    *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_QPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(connection);
#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
        printk("QPT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (u32)QSB_RTM, *QSB_RTM, (u32)QSB_RTD, *QSB_RTD, (u32)QSB_RAMAC, *QSB_RAMAC);
#endif
    /*  Queue VBR Paramter Table (QVPT) */
    *QSB_RTM   = QSB_RTM_DM_SET(QSB_QVPT_SET_MASK);
    *QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_vbr_parameter_table.dword);
    *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_VBR) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(connection);
#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
        printk("QVPT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (u32)QSB_RTM, *QSB_RTM, (u32)QSB_RTD, *QSB_RTD, (u32)QSB_RAMAC, *QSB_RAMAC);
#endif

#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
    {
        printk("set_qsb\n");
        printk("  qsb_clk = %lu\n", (unsigned long)qsb_clk);
        printk("  qsb_queue_parameter_table.bit.tp       = %d\n", (int)qsb_queue_parameter_table.bit.tp);
        printk("  qsb_queue_parameter_table.bit.wfqf     = %d (0x%08X)\n", (int)qsb_queue_parameter_table.bit.wfqf, (int)qsb_queue_parameter_table.bit.wfqf);
        printk("  qsb_queue_parameter_table.bit.vbr      = %d\n", (int)qsb_queue_parameter_table.bit.vbr);
        printk("  qsb_queue_parameter_table.dword        = 0x%08X\n", (int)qsb_queue_parameter_table.dword);
        printk("  qsb_queue_vbr_parameter_table.bit.ts   = %d\n", (int)qsb_queue_vbr_parameter_table.bit.ts);
        printk("  qsb_queue_vbr_parameter_table.bit.taus = %d\n", (int)qsb_queue_vbr_parameter_table.bit.taus);
        printk("  qsb_queue_vbr_parameter_table.dword    = 0x%08X\n", (int)qsb_queue_vbr_parameter_table.dword);
    }
#endif
}

/*
 *  Description:
 *    Add one entry to HTU table.
 *  Input:
 *    vpi        --- unsigned int, virtual path ID
 *    vci        --- unsigned int, virtual channel ID
 *    connection --- unsigned int, connection ID
 *    aal5       --- int, 0 means AAL0, else means AAL5
 *  Output:
 *    none
 */
static INLINE void set_htu_entry(unsigned int vpi, unsigned int vci, unsigned int conn, int aal5)
{
    struct htu_entry htu_entry = {  res1:       0x00,
                                    pid:        g_atm_priv_data.connection[conn].port & 0x01,
                                    vpi:        vpi,
                                    vci:        vci,
                                    pti:        0x00,
                                    vld:        0x01};

    struct htu_mask htu_mask = {    set:        0x03,
                                    pid_mask:   0x02,
                                    vpi_mask:   0x00,
                                    vci_mask:   0x0000,
                                    pti_mask:   0x03,   //  0xx, user data
                                    clear:      0x00};

    struct htu_result htu_result = {res1:       0x00,
                                    cellid:     conn,
                                    res2:       0x00,
                                    type:       aal5 ? 0x00 : 0x01,
                                    ven:        0x01,
                                    res3:       0x00,
                                    qid:        conn};

#if 0
    htu_entry.vld     = 1;
    htu_mask.pid_mask = 0x03;
    htu_mask.vpi_mask = 0xFF;
    htu_mask.vci_mask = 0xFFFF;
    htu_mask.pti_mask = 0x7;
    htu_result.cellid = 1;
    htu_result.type   = 0;
    htu_result.ven    = 0;
    htu_result.qid    = conn;
#endif
    *HTU_RESULT(conn + OAM_HTU_ENTRY_NUMBER) = htu_result;
    *HTU_MASK(conn + OAM_HTU_ENTRY_NUMBER)   = htu_mask;
    *HTU_ENTRY(conn + OAM_HTU_ENTRY_NUMBER)  = htu_entry;

//    printk("Config HTU (%d) for QID %d\n", conn + OAM_HTU_ENTRY_NUMBER, conn);
//    printk("  HTU_RESULT = 0x%08X\n", *(u32*)HTU_RESULT(conn + OAM_HTU_ENTRY_NUMBER));
//    printk("  HTU_MASK   = 0x%08X\n", *(u32*)HTU_MASK(conn + OAM_HTU_ENTRY_NUMBER));
//    printk("  HTU_ENTRY  = 0x%08X\n", *(u32*)HTU_ENTRY(conn + OAM_HTU_ENTRY_NUMBER));
}

/*
 *  Description:
 *    Remove one entry from HTU table.
 *  Input:
 *    conn --- unsigned int, connection ID
 *  Output:
 *    none
 */
static INLINE void clear_htu_entry(unsigned int conn)
{
    HTU_ENTRY(conn + OAM_HTU_ENTRY_NUMBER)->vld = 0;
}

/*
 *  Description:
 *    Setup QSB.
 *  Input:
 *    none
 *  Output:
 *    none
 */
static INLINE void qsb_global_set(void)
{
    u32 qsb_clk = ifx_get_fpi_hz();
    int i;
    u32 tmp1, tmp2, tmp3;

    *QSB_ICDV = QSB_ICDV_TAU_SET(qsb_tau);
    *QSB_SBL  = QSB_SBL_SBL_SET(qsb_srvm);
    *QSB_CFG  = QSB_CFG_TSTEPC_SET(qsb_tstep >> 1);
#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
    {
        printk("qsb_clk = %u\n", qsb_clk);
        printk("QSB_ICDV (%08X) = %d (%d), QSB_SBL (%08X) = %d (%d), QSB_CFG (%08X) = %d (%d)\n", (u32)QSB_ICDV, *QSB_ICDV, QSB_ICDV_TAU_SET(qsb_tau), (u32)QSB_SBL, *QSB_SBL, QSB_SBL_SBL_SET(qsb_srvm), (u32)QSB_CFG, *QSB_CFG, QSB_CFG_TSTEPC_SET(qsb_tstep >> 1));
    }
#endif

    /*
     *  set SCT and SPT per port
     */
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( g_atm_priv_data.port[i].tx_max_cell_rate != 0 )
        {
            tmp1 = ((qsb_clk * qsb_tstep) >> 1) / g_atm_priv_data.port[i].tx_max_cell_rate;
            tmp2 = tmp1 >> 6;                   /*  integer value of Tsb    */
            tmp3 = (tmp1 & ((1 << 6) - 1)) + 1; /*  fractional part of Tsb  */
            /*  carry over to integer part (?)  */
            if ( tmp3 == (1 << 6) )
            {
                tmp3 = 0;
                tmp2++;
            }
            if ( tmp2 == 0 )
                tmp2 = tmp3 = 1;
            /*  1. set mask                                 */
            /*  2. write value to data transfer register    */
            /*  3. start the tranfer                        */
            /*  SCT (FracRate)  */
            *QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SCT_MASK);
            *QSB_RTD   = QSB_RTD_TTV_SET(tmp3);
            *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SCT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
#if defined(DEBUG_QOS) && DEBUG_QOS
            if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
                printk("SCT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (u32)QSB_RTM, *QSB_RTM, (u32)QSB_RTD, *QSB_RTD, (u32)QSB_RAMAC, *QSB_RAMAC);
#endif
            /*  SPT (SBV + PN + IntRage)    */
            *QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SPT_MASK);
            *QSB_RTD   = QSB_RTD_TTV_SET(QSB_SPT_SBV_VALID | QSB_SPT_PN_SET(i & 0x01) | QSB_SPT_INTRATE_SET(tmp2));
            *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
#if defined(DEBUG_QOS) && DEBUG_QOS
            if ( (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
                printk("SPT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (u32)QSB_RTM, *QSB_RTM, (u32)QSB_RTD, *QSB_RTD, (u32)QSB_RAMAC, *QSB_RAMAC);
#endif
        }
}

static INLINE void setup_oam_htu_entry(void)
{
    struct htu_entry htu_entry = {0};
    struct htu_result htu_result = {0};
    struct htu_mask htu_mask = {    set:        0x03,
                                    pid_mask:   0x00,
                                    vpi_mask:   0x00,
                                    vci_mask:   0x00,
                                    pti_mask:   0x00,
                                    clear:      0x00};
    int i;

    /*
     *  HTU Tables
     */
    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
    {
        htu_result.qid = (unsigned int)i;

        *HTU_ENTRY(i + OAM_HTU_ENTRY_NUMBER)  = htu_entry;
        *HTU_MASK(i + OAM_HTU_ENTRY_NUMBER)   = htu_mask;
        *HTU_RESULT(i + OAM_HTU_ENTRY_NUMBER) = htu_result;
    }
    /*  OAM HTU Entry   */
    htu_entry.vci     = 0x03;
    htu_mask.pid_mask = 0x03;
    htu_mask.vpi_mask = 0xFF;
    htu_mask.vci_mask = 0x0000;
    htu_mask.pti_mask = 0x07;
    htu_result.cellid = 0;  //  need confirm
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = 0;  //  need confirm
    *HTU_RESULT(OAM_F4_SEG_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F4_SEG_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)  = htu_entry;
    htu_entry.vci     = 0x04;
    htu_result.cellid = 0;  //  need confirm
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = 0;  //  need confirm
    *HTU_RESULT(OAM_F4_TOT_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F4_TOT_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)  = htu_entry;
    htu_entry.vci     = 0x00;
    htu_entry.pti     = 0x04;
    htu_mask.vci_mask = 0xFFFF;
    htu_mask.pti_mask = 0x01;
    htu_result.cellid = 0;  //  need confirm
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = 0;  //  need confirm
    *HTU_RESULT(OAM_F5_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F5_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F5_HTU_ENTRY)  = htu_entry;
}

/*
 *  Description:
 *    Add HTU entries to capture OAM cell.
 *  Input:
 *    none
 *  Output:
 *    none
 */
static INLINE void validate_oam_htu_entry(void)
{
    HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 1;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 1;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 1;
}

/*
 *  Description:
 *    Remove HTU entries which are used to capture OAM cell.
 *  Input:
 *    none
 *  Output:
 *    none
 */
static INLINE void invalidate_oam_htu_entry(void)
{
    HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 0;
    /*  idle for a while to finish running HTU search   */
    udelay(100);
}

/*
 *  Description:
 *    Loop up for connection ID with virtual path ID.
 *  Input:
 *    vpi --- unsigned int, virtual path ID
 *  Output:
 *    int --- negative value: failed
 *            else          : connection ID
 */
static INLINE int find_vpi(unsigned int vpi)
{
    int i;
    struct atm_connection *connection = g_atm_priv_data.connection;

    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
        if ( (g_atm_priv_data.connection_table & (1 << i))
            && connection[i].vcc != NULL
            && vpi == connection[i].vcc->vpi )
            return i;
    return -1;
}

/*
 *  Description:
 *    Loop up for connection ID with virtual path ID and virtual channel ID.
 *  Input:
 *    vpi --- unsigned int, virtual path ID
 *    vci --- unsigned int, virtual channel ID
 *  Output:
 *    int --- negative value: failed
 *            else          : connection ID
 */
static INLINE int find_vpivci(unsigned int vpi, unsigned int vci)
{
    int i;
    struct atm_connection *connection = g_atm_priv_data.connection;

    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
        if ( (g_atm_priv_data.connection_table & (1 << i))
            && connection[i].vcc != NULL
            && vpi == connection[i].vcc->vpi
            && vci == connection[i].vcc->vci )
            return i;
    return -1;
}

/*
 *  Description:
 *    Loop up for connection ID with atm_vcc structure.
 *  Input:
 *    vcc --- struct atm_vcc *, atm_vcc structure of opened connection
 *  Output:
 *    int --- negative value: failed
 *            else          : connection ID
 */
static INLINE int find_vcc(struct atm_vcc *vcc)
{
    int i;
    struct atm_connection *connection = g_atm_priv_data.connection;

    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
        if ( (g_atm_priv_data.connection_table & (1 << i))
            && connection[i].vcc == vcc )
            return i;
    return -1;
}

/*
 *  Description:
 *    Allocate a sk_buff for RX path using. The size is maximum packet size
 *    plus maximum overhead size.
 *  Input:
 *    none
 *  Output:
 *    sk_buff* --- 0:    Failed
 *                 else: Pointer to sk_buff
 */
static INLINE struct sk_buff *alloc_skb_rx(void)
{
    return alloc_skb_tx(DMA_PACKET_SIZE);
}

static INLINE struct sk_buff *alloc_skb_tx(int len)
{
    struct sk_buff *skb;

    skb = dev_alloc_skb(len + 32);
    if ( skb )
    {
        ATM_SKB(skb)->vcc = NULL;
        skb_reserve(skb, (~((u32)skb->data + 15) & 15) + 16);
        ASSERT(((u32)skb->data & 15) == 0, "skb->data (%#x) is not 4 DWORDS aligned", (u32)skb->data);
        *((u32 *)skb->data - 1) = (u32)skb;
#ifndef CONFIG_MIPS_UNCACHED
        dma_cache_wback((u32)skb->data - sizeof(u32), sizeof(u32));
#endif
    }

    return skb;
}

static struct sk_buff* skb_break_away_from_protocol(struct sk_buff *skb)
{
    struct sk_buff *new_skb;

    if ( skb_shared(skb) ) {
        new_skb = skb_clone(skb, GFP_ATOMIC);
        if ( new_skb == NULL )
            return NULL;
    }
    else
        new_skb = skb_get(skb);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    dst_release(new_skb->dst);
    new_skb->dst = NULL;
#else
    skb_dst_drop(new_skb);
#endif
#ifdef CONFIG_XFRM
    secpath_put(new_skb->sp);
    new_skb->sp = NULL;
#endif
#if defined(CONFIG_NETFILTER) || LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    nf_conntrack_put(new_skb->nfct);
    new_skb->nfct = NULL;
  #if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
    nf_conntrack_put_reasm(new_skb->nfct_reasm);
    new_skb->nfct_reasm = NULL;
  #endif
  #ifdef CONFIG_BRIDGE_NETFILTER
    nf_bridge_put(new_skb->nf_bridge);
    new_skb->nf_bridge = NULL;
  #endif
#endif

    return new_skb;
}

static INLINE int __get_skb_from_dbg_pool(struct sk_buff *skb, const char *func_name, unsigned int line_num)
{
    int ret = 0;
#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
    int i;

    for ( i = 0; i < NUM_ENTITY(g_dbg_skb_swap_pool) && g_dbg_skb_swap_pool[i] != skb; i++ );
    if ( i == NUM_ENTITY(g_dbg_skb_swap_pool) || (unsigned int)skb < KSEG0 )
    {
        err("%s:%d: skb (0x%08x) not in g_dbg_skb_swap_pool", func_name, line_num, (unsigned int)skb);
        ret = -EIO;
    }
    else
        g_dbg_skb_swap_pool[i] = NULL;
#endif
    return ret;
}

static INLINE struct sk_buff *__get_skb_pointer(unsigned int dataptr, unsigned int shift, const char *func_name, unsigned int line_num)
{
    unsigned int shifted_dataptr;
    unsigned int skb_dataptr;
    struct sk_buff *skb;

    //  usually, CPE memory is less than 256M bytes
    //  so NULL means invalid pointer
    if ( dataptr == 0 ) {
        dbg("dataptr is 0, it's supposed to be invalid pointer");
        return NULL;
    }

    shifted_dataptr = shift ? (dataptr << 2) : dataptr;

    skb_dataptr = (shifted_dataptr - 4) | KSEG1;
    skb = *(struct sk_buff **)skb_dataptr;

    if ( __get_skb_from_dbg_pool(skb, func_name, line_num) )
    {
        err("__get_skb_from_dbg_pool fail - dataptr = 0x%08x, shift = %d, skb_dataptr = 0x%08x, skb = 0x%08x", dataptr, shift, skb_dataptr, (unsigned int)skb);
    }

    ASSERT((unsigned int)skb >= KSEG0, "%s:%d: invalid skb - skb = %#08x, dataptr = %#08x", func_name, line_num, (unsigned int)skb, dataptr);
    ASSERT((((unsigned int)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1))) | KSEG1) == (shifted_dataptr | KSEG1), "%s:%d: invalid skb - skb = %#08x, skb->data = %#08x, dataptr = %#08x", func_name, line_num, (unsigned int)skb, (unsigned int)skb->data, dataptr);

    return skb;
}

static INLINE void __put_skb_to_dbg_pool(struct sk_buff *skb, const char *func_name, unsigned int line_num)
{
#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
    int i;

    for ( i = 0; i < NUM_ENTITY(g_dbg_skb_swap_pool) && g_dbg_skb_swap_pool[i] != NULL; i++ );
    if ( i == NUM_ENTITY(g_dbg_skb_swap_pool) )
    {
        err("%s:%d: g_dbg_skb_swap_pool overrun", func_name, line_num);
    }
    else
        g_dbg_skb_swap_pool[i] = skb;
#endif
}

static struct sk_buff* atm_alloc_tx(struct atm_vcc *vcc, unsigned int size)
{
    struct sk_buff *skb;

    /*  send buffer overflow    */
    if ( atomic_read(&sk_atm(vcc)->sk_wmem_alloc) && !atm_may_send(vcc, size) )
    {
        err("send buffer overflow");
        return NULL;
    }

    skb = alloc_skb_tx(size);
    if ( skb == NULL )
    {
        err("sk buffer is used up");
        return NULL;
    }

    atomic_add(skb->truesize, &sk_atm(vcc)->sk_wmem_alloc);

    return skb;
}

static INLINE void atm_free_tx_skb_vcc(struct sk_buff *skb)
{
    struct atm_vcc* vcc;

    ASSERT((u32)skb > 0x80000000, "atm_free_tx_skb_vcc: skb = %08X", (u32)skb);

    vcc = ATM_SKB(skb)->vcc;

    if ( vcc != NULL && vcc->pop != NULL )
    {
        ASSERT(atomic_read(&skb->users) != 0, "atm_free_tx_skb_vcc(vcc->pop): skb->users == 0, skb = %08X", (u32)skb);
        vcc->pop(vcc, skb);
    }
    else
    {
        ASSERT(atomic_read(&skb->users) != 0, "atm_free_tx_skb_vcc(dev_kfree_skb_any): skb->users == 0, skb = %08X", (u32)skb);
        dev_kfree_skb_any(skb);
    }
}

static INLINE void atm_free_tx_skb_vcc_delayed(struct sk_buff *skb)
{
#if !defined(ENABLE_NO_DELAY_ATM_SKB_FREE) || !ENABLE_NO_DELAY_ATM_SKB_FREE
    struct atm_vcc* vcc;

    ASSERT((u32)skb > 0x80000000, "atm_free_tx_skb_vcc: skb = %08X", (u32)skb);

    vcc = ATM_SKB(skb)->vcc;

    if ( (unsigned int)vcc >= KSEG0 && (unsigned int)vcc->pop >= KSEG0 && (unsigned int)vcc->dev >= KSEG0 && find_vcc(vcc) >= 0 )
    {
        ASSERT(atomic_read(&skb->users) != 0, "atm_free_tx_skb_vcc(vcc->pop): skb->users == 0, skb = %08X", (u32)skb);
        atomic_add(skb->truesize, &sk_atm(vcc)->sk_wmem_alloc);
        vcc->pop(vcc, skb);
        return;
    }
#endif

    //  Else
    ASSERT(atomic_read(&skb->users) != 0, "atm_free_tx_skb_vcc(dev_kfree_skb_any): skb->users == 0, skb = %08X", (u32)skb);
    dev_kfree_skb_any(skb);
}

/*
 *  Description:
 *    Handle IRQ of mailbox and despatch to relative handler.
 *  Input:
 *    irq    --- int, IRQ number
 *    dev_id --- void *, argument passed when registering IRQ handler
 *    regs   --- struct pt_regs *, registers' value before jumping into handler
 *  Output:
 *    none
 */
static irqreturn_t mailbox_irq_handler(int irq, void *dev_id)
{
    u32 mailbox_signal = 0;

    mailbox_signal = *MBOX_IGU1_ISR & g_mailbox_signal_mask;
    if ( !mailbox_signal )
        return IRQ_HANDLED;
    *MBOX_IGU1_ISRC = mailbox_signal;

    if ( (mailbox_signal & DMA_TX_CH1_SIG) )
    {
        const int upper_limit = 1000;
        int i = 0;

        while ( DMA_TX_CH1_DESC_BASE->own && i++ < upper_limit );

        if ( DMA_TX_CH1_DESC_BASE->own )
        {
            err("The Central DMA TX Channel 1 Descriptor is still not updated after %d times check!\n", upper_limit);
        }
        else
        {
            g_mailbox_signal_mask &= ~DMA_TX_CH1_SIG;
            g_dma_device->tx_chan[1]->open(g_dma_device->tx_chan[1]);
            *MBOX_IGU1_IER = g_mailbox_signal_mask;
        }
    }

    if ( (mailbox_signal & WAN_RX_SIG(1)) )  //  OAM
    {
        volatile struct rx_descriptor *desc;
        struct sk_buff *skb;
        struct uni_cell_header *header;
        int conn;
        struct atm_vcc *vcc;

        do
        {
            while ( 1 )
            {
                desc = WAN_RX_DESC_BASE(1) + g_wan_rx1_desc_pos;
                if ( !desc->own )   //  PPE hold
                    break;
                skb = g_wan_rx1_desc_skb[g_wan_rx1_desc_pos];
                if ( ++g_wan_rx1_desc_pos == WAN_RX_DESC_NUM(1) )
                    g_wan_rx1_desc_pos = 0;

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
                skb->tail = skb->data + CELL_SIZE;
                skb->len = CELL_SIZE;
                dump_skb(skb, DUMP_SKB_LEN, "mailbox_irq_handler - OAM", 0, 0);
#endif

                header = (struct uni_cell_header *)skb->data;
                if ( header->pti == ATM_PTI_SEGF5 || header->pti == ATM_PTI_E2EF5 )
                    conn = find_vpivci(header->vpi, header->vci);
                else if ( header->vci == 0x03 || header->vci == 0x04 )
                    conn = find_vpi(header->vpi);
                else
                    conn = -1;

                if ( conn >= 0 && (vcc = g_atm_priv_data.connection[conn].vcc) != NULL )
                {
                    g_atm_priv_data.connection[conn].access_time = current_kernel_time();

                    if ( vcc->push_oam != NULL )
                        vcc->push_oam(vcc, skb->data);
                    else
                        ifx_push_oam(skb->data);
                }
                else
                    err("OAM cell dropped due to wrong qid/conn");

                desc->c = 0;
                desc->own = 0;
            }
            *MBOX_IGU1_ISRC = WAN_RX_SIG(1);
        } while ( desc->own );   //  check after clean, if MIPS hold, repeat
    }

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    if ( (mailbox_signal & (0xFF << 16)) )
    {
        unsigned mask = (mailbox_signal >> 16) & 0xFF;
        int qid;
        int conn;

        softq_dbg(1, "+++ xuliang +++: mailbox_signal = %08x, mask = %08x\n", mailbox_signal, mask);
        while ( (qid = clz(mask)) >= 0 )
        {
            softq_dbg(2, "qid = %d, *DSLWAN_TX_PKT_CNTx(%d) = %d\n", qid, qid, *DSLWAN_TX_PKT_CNTx(qid));
            if ( *DSLWAN_TX_PKT_CNTx(qid) <= g_tx_queue_lower )
            {
                if ( g_enable_softq )
                    softq_send(qid);
                else
                {
                    g_mailbox_signal_mask &= ~WAN_TX_SIG(qid);
                    *MBOX_IGU1_IER = g_mailbox_signal_mask;
                    conn = g_tx_queue_to_conn[qid];
                    softq_dbg(3, "*DSLWAN_TX_PKT_CNTx(%d) = %d, conn = %d\n", qid, *DSLWAN_TX_PKT_CNTx(qid), conn);
                    if ( g_atm_priv_data.connection[conn].netdev != NULL )
                    {
                        softq_dbg(4, "g_atm_priv_data.connection[%d].netdev - %s\n", conn, g_atm_priv_data.connection[conn].netdev->name);
                        netif_wake_queue(g_atm_priv_data.connection[conn].netdev);
                    }
                    else
                    {
                        err("+++ xuliang +++: netdev should not be NULL\n");
                    }
                }
            }
            mask ^= 1 << qid;
        }
        softq_dbg(5, "+++ xuliang +++: end\n");
    }
#endif

    if ( (mailbox_signal & WLAN_CPU_TX_SIG) )
    {
#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
        int i;
#endif

#if defined(ENABLE_DIRECTPATH_TX_QUEUE) && ENABLE_DIRECTPATH_TX_QUEUE
        if ( send_directpath_tx_queue() != 0 )  //  not finish
            return IRQ_HANDLED;
#endif

        g_directpath_tx_full = 0;

        g_mailbox_signal_mask &= ~WLAN_CPU_TX_SIG;
        *MBOX_IGU1_IER = g_mailbox_signal_mask;

#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
        for ( i = 0; i < NUM_ENTITY(g_ppe_directpath_data); i++ )
            if ( g_ppe_directpath_data[i].callback.start_tx_fn )
                g_ppe_directpath_data[i].callback.start_tx_fn(g_ppe_directpath_data[i].netif);  //  resume device
#endif
    }

    return IRQ_HANDLED;
}

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
static void softq_send(int qid)
{
    int conn = g_tx_queue_to_conn[qid];
    struct atm_vcc *vcc = g_atm_priv_data.connection[conn].vcc;
    struct sk_buff *skb;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;

    while ( *DSLWAN_TX_PKT_CNTx(qid) <= g_tx_queue_upper && (skb = skb_dequeue(&g_skb_softq[qid])) != NULL )
    {
        /*  allocate descriptor */
        desc = CPU_TO_WAN_TX_DESC_BASE + g_cpu_to_wan_tx_desc_pos;
        if ( !desc->own )   //  PPE hold
            goto SOFTQ_SEND_NO_FREE_DESC;
        if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
            g_cpu_to_wan_tx_desc_pos = 0;

        /*  load descriptor from memory */
        reg_desc = *desc;

        /*  free previous skb   */
        ASSERT((reg_desc.dataptr & 15) == 0, "reg_desc.dataptr (0x%#x) must be 4 DWORDS aligned", reg_desc.dataptr);
        ASSERT(*(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)) > 0x80000000, "pointer to skb is wrong, desc->dataptr = %08X, skb = %08X", reg_desc.dataptr, *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)));
        skb_to_free = get_skb_pointer(reg_desc.dataptr, 0);
        dev_kfree_skb_any(skb_to_free);
        put_skb_to_dbg_pool(skb);

        /*  update descriptor   */
        reg_desc.dataptr    = (u32)skb->data & 0x1FFFFFF0;  //  byte address
        reg_desc.byteoff    = (u32)skb->data & 0x0F;
        reg_desc.datalen    = skb->len;
        reg_desc.mpoa_pt    = 1;
        reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
        reg_desc.pdu_type   = vcc->qos.aal == ATM_AAL5 ? 0 : 1;
        reg_desc.qid        = qid;
        reg_desc.own        = 0;
        reg_desc.c          = 0;

        /*  write discriptor to memory and write back cache */
        *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
        *(volatile u32 *)desc = *(u32 *)&reg_desc;

        /*  update MIB  */
        UPDATE_VCC_STAT(conn, tx_packets, 1);
        UPDATE_VCC_STAT(conn, tx_bytes, skb->len);

        if ( vcc->stats )
            atomic_inc(&vcc->stats->tx);
        if ( vcc->qos.aal == ATM_AAL5 )
            g_atm_priv_data.wtx_pdu++;

        g_atm_priv_data.connection[conn].prio_tx_packets[skb->priority >= 8 ? 7 : skb->priority]++;

        adsl_led_flash();
    }

    if ( skb_queue_len(&g_skb_softq[qid]) == 0 ) //  no more pending packet
    {
        g_mailbox_signal_mask &= ~WAN_TX_SIG(qid);
        *MBOX_IGU1_IER = g_mailbox_signal_mask;
    }

    return;

SOFTQ_SEND_NO_FREE_DESC:
    if ( vcc->stats )
        atomic_inc(&vcc->stats->rx_drop);
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_drop_pdu++;
    UPDATE_VCC_STAT(conn, rx_dropped, 1);
    dev_kfree_skb_any(skb);
    return;
}
#endif

static INLINE void turn_on_dma_rx(int mask)
{
    unsigned long sys_flag;
    int i;

    local_irq_save(sys_flag);
    ASSERT((u32)g_dma_device >= 0x80000000, "g_dma_device = 0x%08X", (u32)g_dma_device);
    for ( i = 0; i < g_dma_device->max_rx_chan_num; i++ )
    {
        ASSERT((u32)g_dma_device->rx_chan[i] >= 0x80000000, "g_dma_device->rx_chan[%d] = 0x%08X", i, (u32)g_dma_device->rx_chan[i]);
        ASSERT(g_dma_device->rx_chan[i]->control == IFX_DMA_CH_ON, "g_dma_device->rx_chan[%d]->control = %d", i, g_dma_device->rx_chan[i]->control);

        if ( g_dma_device->rx_chan[i]->control == IFX_DMA_CH_ON )
        {
            ASSERT((u32)g_dma_device->rx_chan[i]->open >= 0x80000000, "g_dma_device->rx_chan[%d]->open = 0x%08X", i, (u32)g_dma_device->rx_chan[i]->open);

            //  channel 1, 2 is for fast path
            if ( i == 1 || i == 2 )
                g_dma_device->rx_chan[i]->dir = 1;
            g_dma_device->rx_chan[i]->open(g_dma_device->rx_chan[i]);
            if ( i == 1 || i == 2 )
                g_dma_device->rx_chan[i]->dir = -1;
        }
    }
    g_f_dma_opened |= 1 << mask;
    __sync();
    local_irq_restore(sys_flag);
}

static INLINE void turn_off_dma_rx(int mask)
{
    unsigned long sys_flag;
    int i;

    local_irq_save(sys_flag);
    g_f_dma_opened &= ~(1 << mask);
    __sync();
    if ( !g_f_dma_opened )
    {
        for ( i = 0; i < g_dma_device->max_rx_chan_num; i++ )
            g_dma_device->rx_chan[i]->close(g_dma_device->rx_chan[i]);
    }
    local_irq_restore(sys_flag);
}

static u8 *dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
    u8 *buf;
    struct sk_buff *skb;

    skb = alloc_skb_rx();
    if ( !skb )
        return NULL;

    put_skb_to_dbg_pool(skb);

    buf = (u8 *)skb->data;
    *(u32 *)opt = (u32)skb;
    *byte_offset = 0;
    return buf;
}

static int dma_buffer_free(u8 *dataptr, void *opt)
{
    //  ! be careful
    //    this method makes kernel crash when free this DMA device
    //    since the pointers used by fastpast which is supposed not visible to CPU
    //    mix up the pointers so that "opt" is not align with "dataptr".

//    ASSERT(*(void **)((((u32)dataptr | KSEG1) & ~0x0F) - 4) == opt, "address is not match: dataptr = %08X, opt = %08X, *(void **)((((u32)dataptr | KSEG1) & ~0x0F) - 4) = %08X", (u32)dataptr, (u32)opt, *(u32 *)((((u32)dataptr | KSEG1) & ~0x0F) - 4));

    if ( opt )
    {
        get_skb_from_dbg_pool((struct sk_buff *)opt);
        dev_kfree_skb_any((struct sk_buff *)opt);
    }

    return 0;
}

static int dma_int_handler(struct dma_device_info *dma_dev, int status)
{
    int ret = 0;
    int i;

    switch ( status )
    {
    case RCV_INT:
        ret = dma_rx_int_handler(dma_dev);
        break;
    case TX_BUF_FULL_INT:
        dbg("eth0 TX buffer full!");
        netif_stop_queue(g_eth_net_dev[0]);
        for( i = 0; i < g_dma_device->max_tx_chan_num; i++ )
        {
            if ( i >= 1 )
                continue;

            if ( g_dma_device->tx_chan[i]->control == IFX_DMA_CH_ON )
                g_dma_device->tx_chan[i]->enable_irq(g_dma_device->tx_chan[i]);
        }
        break;
    case TRANSMIT_CPT_INT:
        dbg("eth0 TX buffer released!");
        for( i = 0; i < g_dma_device->max_tx_chan_num; i++ )
        {
            if ( i >= 1 )
                continue;

            g_dma_device->tx_chan[i]->disable_irq(g_dma_device->tx_chan[i]);
        }
        netif_wake_queue(g_eth_net_dev[0]);
        break;
    default:
        err("unkown DMA interrupt event - %d", status);
    }

    return ret;
}

static INLINE int dma_rx_int_handler(struct dma_device_info *dma_dev)
{
    struct sk_buff *skb = NULL;
    u8 *buf;
    int len;
    struct flag_header *header;
//    u32 header_val;
    u32 off;
    int conn;
    struct atm_vcc *vcc;
    struct eth_priv_data *priv = NULL;

    len = dma_device_read(dma_dev, &buf, (void **)&skb);
    if ( !skb )
        return 0;
    get_skb_from_dbg_pool(skb);

    ASSERT(buf != NULL, "skb = %08x, buf = %08x", (unsigned int)skb, (unsigned int)buf);
    ASSERT((u32)skb >= 0x80000000, "can not restore skb pointer (ch %d) --- skb = 0x%08X", dma_dev->current_rx_chan, (u32)skb);
    ASSERT(skb->data == buf && skb->tail == buf, "skb->data (%08X) or skb->tail (%08X) is corrupted, buf is %08x", (u32)skb->data, (u32)skb->tail, (u32)buf);
    ASSERT(len <= (u32)skb->end - (u32)skb->data, "pakcet is too large: %d", len);
    ASSERT(*(struct sk_buff **)((((u32)buf | KSEG1) & ~0x0F) - 4) == skb, "address is not match: skb = %08X, *(struct sk_buff **)((((u32)buf | KSEG1) & ~0x0F) - 4) = %08X", (u32)skb, *(u32 *)((((u32)buf | KSEG1) & ~0x0F) - 4));

    dump_flag_header(g_fwcode, (struct flag_header *)skb->data, (char *)__FUNCTION__, dma_dev->current_rx_chan, 0);

    header = (struct flag_header *)skb->data;

    //  pl_byteoff
    off = header->pl_byteoff;

    len -= sizeof(struct flag_header) + off + ETH_CRC_LENGTH;
    ASSERT(len >= 60, "pakcet is too small: %d", len);

    skb->data += sizeof(struct flag_header) + off;
    skb->len  =  len;
    skb_set_tail_pointer(skb, len);

    dump_skb(skb, DUMP_SKB_LEN, "dma_rx_int_handler raw data", dma_dev->current_rx_chan, 0);

    //  implement indirect fast path
    if ( header->acc_done && header->dest_list )    //  acc_done == 1 && dest_list != 0
    {
        //  Current implementation send packet to highest priority dest only (0 -> 7, 0 is highest)
        //  2 is CPU, so we ignore it here
        if ( (header->dest_list & (1 << 3)) )       //  3 - USB/WLAN, for performance concern, handle this queue first
        {
            int if_id = 0;

            if ( (g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
            {
                //  fill MAC info into skb, in case the driver hooked to direct path need such info
                skb->dev = g_ppe_directpath_data[if_id].netif;
                skb->protocol = eth_type_trans(skb, skb->dev);
                skb_push(skb, ETH_HLEN);

                //  no RX interface information, indirect xmit
                g_ppe_directpath_data[if_id].rx_fn_txif_pkt++;
                g_ppe_directpath_data[if_id].callback.rx_fn(NULL, g_ppe_directpath_data[if_id].netif, skb, skb->len);
                return 0;
            }
            dev_kfree_skb_any(skb);
            return 0;
        }
        else if ( (header->dest_list & (1 << 0)) )  //  0 - eth0
        {
            eth0_xmit(skb, 0);
            return 0;
        }
        else if ( (header->dest_list & (1 << 1)) )  //  1 - DSL
        {
            unsigned long sys_flag;
            volatile struct tx_descriptor *desc;
            struct tx_descriptor reg_desc;
            struct sk_buff *skb_to_free;
            int byteoff;

            byteoff = (u32)skb->data & 15;
            *(struct sk_buff **)((u32)skb->data - byteoff - sizeof(struct sk_buff *)) = skb;
#ifndef CONFIG_MIPS_UNCACHED
            /*  write back to physical memory   */
            dma_cache_wback((u32)skb->data - byteoff - sizeof(struct sk_buff *), sizeof(struct sk_buff *));
#endif

            /*  allocate descriptor */
            local_irq_save(sys_flag);
            desc = CPU_TO_WAN_TX_DESC_BASE + g_cpu_to_wan_tx_desc_pos;
            if ( !desc->own )   //  PPE hold
            {
                local_irq_restore(sys_flag);
                err("PPE hold");
                dev_kfree_skb_any(skb);
                return 0;
            }
            if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
                g_cpu_to_wan_tx_desc_pos = 0;
            local_irq_restore(sys_flag);

            /*  load descriptor from memory */
            reg_desc = *desc;

            /*  free previous skb   */
            ASSERT((reg_desc.dataptr & 15) == 0, "reg_desc.dataptr (0x%#x) must be 4 DWORDS aligned", reg_desc.dataptr);
            ASSERT(*(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)) > 0x80000000, "pointer to skb is wrong, desc->dataptr = %08X, skb = %08X", reg_desc.dataptr, *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)));
            skb_to_free = get_skb_pointer(reg_desc.dataptr, 0);
            atm_free_tx_skb_vcc_delayed(skb_to_free);
            put_skb_to_dbg_pool(skb);

            /*  update descriptor   */
            reg_desc.dataptr    = (u32)skb->data & 0x1FFFFFF0;  //  byte address
            reg_desc.byteoff    = byteoff;
            reg_desc.datalen    = skb->len;
            reg_desc.mpoa_pt    = 0;    //  firmware apply MPoA
            reg_desc.mpoa_type  = header->mpoa_type;
            reg_desc.pdu_type   = 0;    //  AAL5
            reg_desc.qid        = header->dsl_qid;
            reg_desc.own        = 0;
            reg_desc.c          = 0;

#if 0
            /*  update MIB  */
            UPDATE_VCC_STAT(conn, tx_packets, 1);
            UPDATE_VCC_STAT(conn, tx_bytes, skb->len);
#endif

            dump_skb(skb, DUMP_SKB_LEN, "ppe_send (indirect path)", reg_desc.qid, 1);

            /*  write discriptor to memory and write back cache */
            *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
            *(volatile u32 *)desc = *(u32 *)&reg_desc;

            adsl_led_flash();

            return 0;
        }
        else
        {
            for ( off = 4; off < 7; off++ )
            {
                if ( (header->dest_list & (1 << off)) ) //  4...7
                {
                    int if_id = off - 3;

                    if ( (g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
                    {
                        //  fill MAC info into skb, in case the driver hooked to direct path need such info
                        skb->dev = g_ppe_directpath_data[if_id].netif;
                        skb->protocol = eth_type_trans(skb, skb->dev);
                        skb_push(skb, ETH_HLEN);

                        //  no RX interface information, indirect xmit
                        g_ppe_directpath_data[if_id].rx_fn_txif_pkt++;
                        g_ppe_directpath_data[if_id].callback.rx_fn(NULL, g_ppe_directpath_data[if_id].netif, skb, skb->len);
                        return 0;
                    }
                }
            }
            dev_kfree_skb_any(skb);
            return 0;
        }
    }

    off = header->src_itf;
    switch ( off )
    {
    case 0: //  MII0
        priv = ifx_eth_fw_netdev_priv(g_eth_net_dev[0]);
        if ( netif_running(g_eth_net_dev[0]) )
        {
            skb->dev = g_eth_net_dev[0];
            skb->protocol = eth_type_trans(skb, g_eth_net_dev[0]);

            if ( netif_rx(skb) == NET_RX_DROP )
            {
#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
                if ( priv->fc_bit && !test_and_set_bit(priv->fc_bit, &netdev_fc_xoff) )
                    //  disable ENET0 store
                    *ETOP_CFG &= ~(1 << 6);
#endif
                priv->rx_dropped++;
            }
            else
            {
                priv->rx_packets++;
                priv->rx_bytes += skb->len;
            }

            return 0;
        }
        else
            priv->rx_dropped++;
        break;
    case 1: //  ATM
        conn = header->dsl_qid;
        if ( (vcc = g_atm_priv_data.connection[conn].vcc) != NULL )
        {
            if ( atm_charge(vcc, skb->truesize) )
            {
                ASSERT(vcc->qos.aal == ATM_AAL5 || header->aal5_raw, "ETH packet comes in from ATM port non-AAL5 VC");

                ATM_SKB(skb)->vcc = vcc;

                if ( !header->aal5_raw )
                {
                    /*  ETH packet, need recover ATM encapsulation  */
                    if ( WRX_QUEUE_CONFIG(conn)->mpoa_mode )
                    {
                        /*  LLC */
                        switch ( header->mpoa_type )
                        {
                        case 0: //  EoA w/o FCS
                            ASSERT(skb_headroom(skb) >= 10, "not enough skb headroom (LLC EoA w/o FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (LLC EoA w/o FCS)");
                            skb_push(skb, 10);
                            ((u32 *)skb->data)[0] = 0xAAAA0300;
                            ((u32 *)skb->data)[1] = 0x80C20007;
                            ((u16 *)skb->data)[4] = 0x0000;
                            break;
                        case 1: //  EoA w FCS
                            ASSERT(skb_headroom(skb) >= 10, "not enough skb headroom (LLC EoA w FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (LLC EoA w FCS)");
                            skb_push(skb, 10);
                            skb_put(skb, ETH_CRC_LENGTH);
                            ((u32 *)skb->data)[0] = 0xAAAA0300;
                            ((u32 *)skb->data)[1] = 0x80C20001;
                            ((u16 *)skb->data)[4] = 0x0000;
                            break;
                        case 2: //  PPPoA
                            ASSERT(skb_headroom(skb) >= 6, "not enough skb headroom (LLC PPPoA)");
                            skb->data += header->ip_offset - 6;
                            skb->len  -= header->ip_offset - 6;
                            ASSERT(((u32)skb->data & 0x03) == 0, "not aligned data pointer (LLC PPPoA)");
                            ((u32 *)skb->data)[0] = 0xFEFE03CF;
                            ((u16 *)skb->data)[2] = header->is_ipv4 ? 0x0021 : 0x0057;
                            break;
                        case 3: //  IPoA
                            ASSERT(skb_headroom(skb) >= 8, "not enough skb headroom (LLC IPoA)");
                            skb->data += header->ip_offset - 8;
                            skb->len  -= header->ip_offset - 8;
                            ASSERT(((u32)skb->data & 0x03) == 0, "not aligned data pointer (LLC IPoA)");
                            ((u32 *)skb->data)[0] = 0xAAAA0300;
                            ((u32 *)skb->data)[1] = header->is_ipv4 ? 0x00000800 : 0x000086DD;
                            break;
                        }
                    }
                    else
                    {
                        /*  VC-mux  */
                        switch ( header->mpoa_type )
                        {
                        case 0: //  EoA w/o FCS
                            ASSERT(skb_headroom(skb) >= 2, "not enough skb headroom (VC-mux EoA w/o FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (VC-mux EoA w/o FCS)");
                            skb_push(skb, 2);
                            *(u16 *)skb->data = 0x0000;
                            break;
                        case 1: //  EoA w FCS
                            ASSERT(skb_headroom(skb) >= 2, "not enough skb headroom (VC-mux EoA w FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (VC-mux EoA w FCS)");
                            skb_push(skb, 2);
                            skb_put(skb, ETH_CRC_LENGTH);
                            *(u16 *)skb->data = 0x0000;
                            break;
                        case 2: //  PPPoA
                            ASSERT(skb_headroom(skb) >= 2, "not enough skb headroom (VC-mux PPPoA)");
                            skb->data += header->ip_offset - 2;
                            skb->len  -= header->ip_offset - 2;
                            ASSERT(((u32)skb->data & 0x03) == 0, "not aligned data pointer (VC-mux PPPoA)");
                            if ( header->is_ipv4 )
                                *(u16 *)skb->data = 0x0021; //  refer to pppoatm.c, 0xC021 is used here for Link Control Protocol
                            else
                                *(u16 *)skb->data = 0x0057;
                            break;
                        case 3: //  IPoA
                            skb->data += header->ip_offset;
                            skb->len  -= header->ip_offset;
                            break;
                        }
                    }
                    dump_skb(skb, DUMP_SKB_LEN, "dma_rx_int_handler AAL5 encapsulated", dma_dev->current_rx_chan, 0);
                }

                g_atm_priv_data.connection[conn].access_time = current_kernel_time();

                adsl_led_flash();

                vcc->push(vcc, skb);

                if ( vcc->stats )
                    atomic_inc(&vcc->stats->rx);

                if ( vcc->qos.aal == ATM_AAL5 )
                    g_atm_priv_data.wrx_pdu++;

                UPDATE_VCC_STAT(conn, rx_packets, 1);
                UPDATE_VCC_STAT(conn, rx_bytes, skb->len);

                return 0;
            }
            else
            {
                err("inactive qid %d", conn);

                if ( vcc->stats )
                    atomic_inc(&vcc->stats->rx_drop);

                if ( vcc->qos.aal == ATM_AAL5 )
                    g_atm_priv_data.wrx_drop_pdu++;

                UPDATE_VCC_STAT(conn, rx_dropped, 1);
            }
        }
        break;
    case 3: //  USB/WLAN
    default:    //  other interface receive
        if ( (g_ppe_directpath_data[off - 3].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
        {
            skb->dev = g_ppe_directpath_data[off - 3].netif;
            //  no TX interface information, device push
            g_ppe_directpath_data[off - 3].rx_fn_rxif_pkt++;
            g_ppe_directpath_data[off - 3].callback.rx_fn(g_ppe_directpath_data[off - 3].netif, NULL, skb, skb->len);
            return 0;
        }
    }

    dev_kfree_skb_any(skb);
    return 0;
}

static INLINE void reset_ppe(void)
{
#ifdef MODULE
    //  reset PPE
    unsigned int etop_cfg;
    unsigned int etop_mdio_cfg;
    unsigned int etop_ig_plen_ctrl;
    unsigned int enet_mac_cfg;

    etop_cfg            = *ETOP_CFG;
    etop_mdio_cfg       = *ETOP_MDIO_CFG;
    etop_ig_plen_ctrl   = *ETOP_IG_PLEN_CTRL0;
    enet_mac_cfg        = *ENET_MAC_CFG(0);

    *ETOP_CFG = (*ETOP_CFG & ~0x03C0) | 0x0001;

    //  reset PPE
    ifx_rcu_rst(IFX_RCU_DOMAIN_PPE, IFX_RCU_MODULE_PPA);

    *ETOP_MDIO_CFG     = etop_mdio_cfg;
    *ETOP_IG_PLEN_CTRL0= etop_ig_plen_ctrl;
    *ENET_MAC_CFG(0)   = enet_mac_cfg;
    *ETOP_CFG          = etop_cfg;
#endif
}

static INLINE void init_pmu(void)
{
    //  PMU_PWDCR - bit
    //   5 - DMA, should be enabled in DMA driver
    //   9 - DSL DFE/AFE
    //  13 - PPE
    //  15 - AHB
    PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_SLL01_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_QSB_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_ENABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_DPLUS_PMU_SETUP(IFX_PMU_ENABLE);
#if MII0_MODE_SETUP == EPHY_MODE
    //   7 - E-phye
    EPHY_PMU_SETUP(IFX_PMU_DISABLE);
#endif

    udelay(100);
}

static INLINE void init_gpio(int mii_mode)
{
    switch ( mii_mode )
    {
    case REV_MII_MODE:
        //  GPIO 0 (TXD0), 4(TXD1), 12 (RXDV), 13(TXEN)
        *GPIO_Pi_ALTSEL0(0) = (*GPIO_Pi_ALTSEL0(0) & 0xCFFF) | 0x0010;
        *GPIO_Pi_ALTSEL1(0) = (*GPIO_Pi_ALTSEL1(0) & 0xFFFE) | 0x3011;
        *GPIO_Pi_OD(0)      =  *GPIO_Pi_OD(0) | 0x3011;
        *GPIO_Pi_DIR(0)     = (*GPIO_Pi_DIR(0) & ~0x3011) | (0x1000);
        //  GPIO 19 (RXD3), 20(RXD2), 23 (RXD1), 25 (RXD0), 24 (MDIO), 27 (MDC), 28(TXD3), 29 (TXD2)
        *GPIO_Pi_ALTSEL0(1) = (*GPIO_Pi_ALTSEL0(1) & 0xCD66) | 0x0900;
        *GPIO_Pi_ALTSEL1(1) = (*GPIO_Pi_ALTSEL1(1) & 0xFFFF) | 0x3B98;
        *GPIO_Pi_OD(1)      =  *GPIO_Pi_OD(1) | 0x0998;
        *GPIO_Pi_DIR(1)     = (*GPIO_Pi_DIR(1) & ~0x0200) | 0x3998;
        //  GPIO 21 (CRS), 22(RX Clk), 26 (TX Clk)
        *GPIO_Pi_ALTSEL0(1) = (*GPIO_Pi_ALTSEL0(1) & 0xFB9F);
        *GPIO_Pi_ALTSEL1(1) = (*GPIO_Pi_ALTSEL1(1) & 0xFFFF) | 0x0460;
        *GPIO_Pi_DIR(1)     =  *GPIO_Pi_DIR(1) & ~0x0220;
        dbg("GPIO setting is done for REV MII");
        break;

    case RED_MII_MODE:
        //  GPIO 0 (TXD0), 4(TXD1), 13(TXEN)
        *GPIO_Pi_ALTSEL0(0) = (*GPIO_Pi_ALTSEL0(0) & 0xDFFE) | 0x0010;
        *GPIO_Pi_ALTSEL1(0) = (*GPIO_Pi_ALTSEL1(0) & 0xFFFF) | 0x2011;
        *GPIO_Pi_OD(0)      =  *GPIO_Pi_OD(0) | 0x2011;
        //  GPIO24 (MDIO), 27 (MDC)
        *GPIO_Pi_ALTSEL0(1) = (*GPIO_Pi_ALTSEL0(1) & 0xFFFF) | 0x0900;
        *GPIO_Pi_ALTSEL1(1) = (*GPIO_Pi_ALTSEL1(1) & 0xFFFF) | 0x0900;
        *GPIO_Pi_DIR(1)     =  *GPIO_Pi_DIR(1) | 0x0900;

#if defined(RED_MII_MUX_WITH_SPI) && RED_MII_MUX_WITH_SPI
        //  GPIO 7(RXDV), 8(RXD1), 9(RXD0), 10(Ref Clock)
        *GPIO_Pi_ALTSEL0(0) =  *GPIO_Pi_ALTSEL0(0) & 0xF87F;
        *GPIO_Pi_ALTSEL1(0) = (*GPIO_Pi_ALTSEL1(0) & 0xFFFF) | 0x0780;
        *GPIO_Pi_OD(0)      =  *GPIO_Pi_OD(0) | 0x0400;
        *GPIO_Pi_DIR(0)     =  *GPIO_Pi_DIR(0) & 0xFC7F;
        dbg("SPI Inactive");
#else
        //  GPIO 21, 22, 23, 25
        *GPIO_Pi_ALTSEL0(1) =  *GPIO_Pi_ALTSEL0(1) & 0xFD1F;
        *GPIO_Pi_ALTSEL1(1) = (*GPIO_Pi_ALTSEL1(1) & 0xFFFF) | 0x02E0;
        *GPIO_Pi_OD(1)      =  *GPIO_Pi_OD(1) | 0x0040;
        *GPIO_Pi_DIR(1)     =  *GPIO_Pi_DIR(1) & 0xFD5F;
        dbg("FLASH Inactive");
#endif
    }
}

static INLINE void configureMiiRxClk(int external, int loopback)
{
    if ( external )
        //  Clock is from External (From PAD)
        *CGU_IFCCR &= ~(0x03 << 28);
    else
    {
        //Clock is from  CGU
        if ( !loopback )
            *CGU_IFCCR |= 0x03 << 28;
        else
            *CGU_IFCCR = (*CGU_IFCCR | (0x01 << 28)) & ~(0x01 << 29);
    }
}


static INLINE void configureMiiTxClk(int external, int loopback)
{
    if ( external )
        //  Clock is from External (From PAD)
        *CGU_IFCCR &= ~(0x03 << 26);
    else
    {
        //Clock is from  CGU
        if ( !loopback )
            *CGU_IFCCR |= 0x03 << 26;
        else
            *CGU_IFCCR = (*CGU_IFCCR | (0x01 << 26)) & ~(0x01 << 27);
    }
}

static INLINE void configureRMiiRefClk(int external, int loopback)
{
    if ( external )
        //  Clock is from External (From PAD)
        *CGU_IFCCR &= ~(0x03 << 24);
    else
    {
        //Clock is from  CGU
        if ( !loopback )
            *CGU_IFCCR |= 0x03 << 24;
        else
            *CGU_IFCCR = (*CGU_IFCCR | (0x01 << 24)) & ~(0x01 << 25);
    }
}

static INLINE void configurePhyClk(int enable)
{
    if (enable)
    {
        //Clock from External
        *CGU_IFCCR &= ~(0x01 << 4);
        *CGU_OSC_CON |= 0x18;
        dbg("External Clock\n");
    }
    else
    {
        //Clock from CGU
        *CGU_IFCCR |= 0x01 << 4;
        *CGU_OSC_CON &= ~0x18;
        dbg("Internal Clock\n");
    }
}

static INLINE void init_etop(int mii_mode)
{
    u32 etop_cfg_clear, etop_cfg_set;
    volatile int i;

    /*  reset ETOP  */
    *PP32_DBG_CTRL = DBG_CTRL_RESTART;  //  before init we run pp32 and issue reset to workaround unproper pp32 internal register values
    for ( i = 0; i < 10; i++ );
    *PP32_SRST &= ~(1 << 8);            //  Reset EMA before reseting PPE to avoid uncertain EMA state
    for ( i = 0; i < 10; i++ );
    *RCU_RST_REQ |= 1 << 8;             //  Reset PPE module (ETOP)
    udelay(100);

    //  disable eth0
    etop_cfg_set = 1 << 0;
    etop_cfg_clear = (1 << 6) | (1 << 8);
    *ETOP_CFG = (*ETOP_CFG | etop_cfg_set) & ~etop_cfg_clear;

    switch ( mii_mode )
    {
    case MII_MODE:
        configureMiiRxClk(1, 0);
        configureMiiTxClk(1, 0);
        dbg("MII_MODE selected");
        break;
    case REV_MII_MODE:
        configureMiiRxClk(0, 0);
        configureMiiTxClk(0, 0);
        *ETOP_CFG &= ~0x00004000;
        *CGU_OSC_CON &= ~0x18;
        *ETOP_CFG |= 0x00000002;
        dbg("REV_MII_MODE selected");
        break;
    case RED_MII_MODE:
        configureRMiiRefClk(0, 0);
        *ETOP_CFG = (*ETOP_CFG & ~0x00004000) | 0x00001000;
        *CGU_OSC_CON &= ~0x18;
        dbg("RED_MII_MODE selected");
        break;
    case EPHY_MODE:
        configurePhyClk(EXTERNAL_EPHY_CLOCK);
        *ETOP_CFG |= 0x00004000;
#if defined(SET_CLASS_A_VALUE) && SET_CLASS_A_VALUE
        ephy_write_mdio_reg(EPHY_ADDRESS, EPHY_SET_CLASS_VALUE_REG, 0xC020);
#endif
        dbg("EPHY_MODE selected");
        break;
    default:
        err("Unknown MII mode - %d", mii_mode);
    }

    //  set packet length
    *ETOP_IG_PLEN_CTRL0 = 0x004005DC;

    *ETOP_MDIO_CFG    = 0x0000;

    *ENET_MAC_CFG(0)  = 0x0807;

    *ENETS_DBA(0)     = 0x0F44;
    *ENETS_CBA(0)     = 0x15A4;
    *ENETS_CFG(0)     = 0x00585032;
    *ENETS_PGCNT(0)   = 0x00020000;
    *ENETS_PKTCNT(0)  = 0x0200;

    *ENETF_DBA(0)     = 0x1264;
    *ENETF_CBA(0)     = 0x15D6;
    *ENETF_CFG(0)     = 0x700D;
    *ENETF_PGCNT(0)   = 0x0000;
    *ENETF_PKTCNT(0)  = 0x0000;

    *DPLUS_TXCFG      = 0x000D;
    *DPLUS_TXDB       = 0x1264;
    *DPLUS_TXCB       = 0x15D6;
    *DPLUS_TXPGCNT    = 0x0000;
    *DPLUS_TXCFG      = 0xF00D;

    *DPLUS_RXCFG      = 0x5042;
    *DPLUS_RXPGCNT    = 0x00040000;

    //  enable MII0
//    *ETOP_CFG = (*ETOP_CFG & ~(1 << 0)) | ((1 << 6) | (1 << 8));
}

static INLINE void start_etop(void)
{
    // enable MII0
    *ETOP_CFG = (*ETOP_CFG & ~(1 << 0)) | ((1 << 6) | (1 << 8));
#if MII0_MODE_SETUP == EPHY_MODE
    EPHY_PMU_SETUP(IFX_PMU_ENABLE);
#endif
}

static INLINE void stop_datapath(void)
{
    unsigned long org_jiffies;
    int i;

    g_stop_datapath = 1;    //  going to be unloaded

    //  disable ENET0 store to stop ingress traffic
    *ETOP_CFG &= ~(1 << 6);

    //  turn on DMA RX channels
    turn_on_dma_rx(31);

    //  clean ingress datapath
    org_jiffies = jiffies;
    while ( ((*ENETS_PGCNT(0) & 0xFF) != 0 || (*ENETS_PKTCNT(0) & 0xFF) != 0 || (*DPLUS_RXPGCNT & 0xFF) != 0) && jiffies - org_jiffies < HZ / 10 + 1 )
        schedule();
    if ( jiffies - org_jiffies > HZ / 10 )
        err("Can not clear ENETS_PGCNT/ENETS_PKTCNT/DPLUS_RXPGCNT");

    //  turn off DMA RX channels (including loopback RX channels)
    while ( (i = clz(g_f_dma_opened)) >= 0 )
        turn_off_dma_rx(i);

    //  turn off DMA Loopback TX channels
    g_dma_device->tx_chan[1]->close(g_dma_device->tx_chan[1]);

    //  stop ENET0 fetch and MII
    *ETOP_CFG = (*ETOP_CFG & ~(1 << 8)) | (1 << 0);
}

static INLINE void init_ema(void)
{
    *EMA_CMDCFG  = (EMA_CMD_BUF_LEN << 16) | (EMA_CMD_BASE_ADDR >> 2);
    *EMA_DATACFG = (EMA_DATA_BUF_LEN << 16) | (EMA_DATA_BASE_ADDR >> 2);
    *EMA_IER     = 0x000000FF;
    *EMA_CFG     = EMA_READ_BURST | (EMA_WRITE_BURST << 2);
}

static INLINE void init_mailbox(void)
{
    *MBOX_IGU1_ISRC = 0xFFFFFFFF;
    *MBOX_IGU1_IER  = 0x00000000;   //  Don't need to enable RX interrupt, EMA driver handle RX path.
    dbg("MBOX_IGU1_IER = 0x%08X", *MBOX_IGU1_IER);
}

static INLINE void clear_share_buffer(void)
{
    volatile u32 *p = SB_RAM0_ADDR(0);
    unsigned int i;

    //  write all zeros only
    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN; i++ )
        *p++ = 0;

    //  Configure share buffer master selection
    //*SB_MST_SEL |= 0x03;
    *SB_MST_PRI0 = 1;
    *SB_MST_PRI1 = 1;
}

static INLINE void clear_cdm(void)
{
    volatile u32 *dest;
    int i;

    //  Both blocks are set to code memory
    *CDM_CFG = CDM_CFG_RAM1_SET(0x01) | CDM_CFG_RAM0_SET(0x00);
    for ( i = 0; i < 1000; i++ );

    dest = CDM_CODE_MEMORY_RAM0_ADDR(0);
    for ( i = 0; i < CDM_CODE_MEMORY_RAM0_DWLEN + CDM_CODE_MEMORY_RAM1_DWLEN; i++ )
        *dest++ = 0;
}

static INLINE void board_init(void)
{
}

static INLINE void init_dsl_hw(void)
{
    *SFSM_DBA0          = 0x0208;
//  *SFSM_DBA1          = 0x0800;
    *SFSM_CBA0          = 0x03E4;
//  *SFSM_CBA1          = 0x0921;
#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
    *SFSM_CFG0          = 0x14014;
#else
    *SFSM_CFG0          = 0x14018;
#endif
    *SFSM_CFG1          = 0x00000;
    *FFSM_DBA0          = 0x0358;
//  *FFSM_DBA1          = 0x0932;
//  *FFSM_CFG0          = 0x3000A;
    *FFSM_CFG0          = 0x1000A;
//  *FFSM_CFG1          = 0x3000C;
//  *FFSM_IDLE_HEAD_BC0 = 0xF0D10000;
    *FFSM_IDLE_HEAD_BC0 = 0x00000001;
//  *FFSM_IDLE_HEAD_BC1 = 0xF0D10000;
//  *EMA_DATACFG        = 0x1000B00;
}

static INLINE void init_hw(void)
{
    //  reset etop is done in init_etop

    init_pmu();

    reset_ppe();

    init_gpio(MII0_MODE_SETUP);

    init_etop(MII0_MODE_SETUP);

    init_dsl_hw();

    init_ema();

    init_mailbox();

    clear_share_buffer();

    clear_cdm();

    board_init();
}

/*
 *  Description:
 *    Download PPE firmware binary code.
 *  Input:
 *    src       --- u32 *, binary code buffer
 *    dword_len --- unsigned int, binary code length in DWORD (32-bit)
 *  Output:
 *    int       --- 0:    Success
 *                  else: Error Code
 */
static INLINE int pp32_download_code(const u32 *code_src, unsigned int code_dword_len, const u32 *data_src, unsigned int data_dword_len)
{
    u32 reg_old_value;
    volatile u32 *dest;

    if ( code_src == 0 || ((unsigned long)code_src & 0x03) != 0
        || data_src == 0 || ((unsigned long)data_src & 0x03) )
        return -EINVAL;

    /*  save the old value of CDM_CFG and set PPE code memory to FPI bus access mode    */
    reg_old_value = *CDM_CFG;
    if ( code_dword_len <= 4096 )
        *CDM_CFG = CDM_CFG_RAM1_SET(0x00) | CDM_CFG_RAM0_SET(0x00);
    else
        *CDM_CFG = CDM_CFG_RAM1_SET(0x01) | CDM_CFG_RAM0_SET(0x00);

    /*  copy code   */
    dest = CDM_CODE_MEMORY_RAM0_ADDR(0);
    while ( code_dword_len-- > 0 )
        *dest++ = *code_src++;

    /*  copy data   */
    dest = PP32_DATA_MEMORY_RAM1_ADDR(0);
    while ( data_dword_len-- > 0 )
        *dest++ = *data_src++;

    /*  restore old configuration   */
//    *CDM_CFG = reg_old_value;

    return 0;
}

/*
 *  Description:
 *    Do PP32 specific initialization.
 *  Input:
 *    data --- void *, specific parameter passed in.
 *  Output:
 *    int  --- 0:    Success
 *             else: Error Code
 */
static INLINE int pp32_specific_init(int fwcode, void *data)
{
    return 0;
}

/*
 *  Description:
 *    Initialize and start up PP32.
 *  Input:
 *    none
 *  Output:
 *    int  --- 0:    Success
 *             else: Error Code
 */
static INLINE int pp32_start(int fwcode)
{
    int ret;

    /*  download firmware   */
    ret = pp32_download_code(firmware_binary_code, sizeof(firmware_binary_code) / sizeof(*firmware_binary_code), firmware_binary_data, sizeof(firmware_binary_data) / sizeof(*firmware_binary_data));
    if ( ret )
        return ret;

    /*  firmware specific initialization    */
    ret = pp32_specific_init(g_fwcode, NULL);
    if ( ret )
        return ret;

    /*  run PP32    */
    *PP32_DBG_CTRL = DBG_CTRL_RESTART;
    /*  idle for a while to let PP32 init itself    */
    udelay(100);

    return 0;
}

/*
 *  Description:
 *    Halt PP32.
 *  Input:
 *    none
 *  Output:
 *    none
 */
static INLINE void pp32_stop(void)
{
    /*  halt PP32   */
    *PP32_DBG_CTRL = DBG_CTRL_STOP;
}

static INLINE int init_local_variables(void)
{
    int i, j;

    /*  initialize semaphore used by open and close */
    sema_init(&g_sem, 1);

    if ( ethwan == 1 )
        g_eth_wan_mode = 2;

    memset(&g_atm_priv_data, 0, sizeof(g_atm_priv_data));
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        g_atm_priv_data.port[i].tx_max_cell_rate    = (u32)port_cell_rate_up[i];
    }
    g_atm_priv_data.max_connections = ATM_QUEUE_NUMBER;
    g_atm_priv_data.max_sw_tx_queues = ATM_SW_TX_QUEUE_NUMBER;

//    memset(g_eth_priv_data, 0, sizeof(g_eth_priv_data));

    for ( i = 0; i < 1; i++ )
    {
        int tx_q[] = {0};
        int max_packet_priority = NUM_ENTITY(g_eth_prio_queue_map[i]);  //  assume only 8 levels are used in Linux
        int tx_num_q = NUM_ENTITY(tx_q);
        int priorities_per_q = (max_packet_priority + tx_num_q - 1) / tx_num_q;

        for ( j = 0; j < NUM_ENTITY(g_eth_prio_queue_map[i]); j++ )
            g_eth_prio_queue_map[i][j] = tx_q[j / priorities_per_q];
    }

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    for ( i = 0; i < NUM_ENTITY(g_tx_queue_to_conn); i++ )
        g_tx_queue_to_conn[i] = -1;
    for ( i = 0; i < NUM_ENTITY(g_skb_softq); i++ )
        skb_queue_head_init(&g_skb_softq[i]);
#endif

    return 0;
}

static INLINE void clear_local_variables(void)
{
#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    struct sk_buff *skb;
    int i;

    for ( i = 0; i < NUM_ENTITY(g_skb_softq); i++ )
    {
        while ( (skb = skb_dequeue(&g_skb_softq[i])) != NULL )
            dev_kfree_skb_any(skb);
    }
#endif
}

static INLINE void init_communication_data_structures(int fwcode)
{
    struct eth_ports_cfg eth_ports_cfg = {0};
    struct lan_rout_tbl_cfg lan_rout_tbl_cfg = {0};
    struct wan_rout_tbl_cfg wan_rout_tbl_cfg = {0};
    struct gen_mode_cfg gen_mode_cfg = {0};
    struct brg_tbl_cfg brg_tbl_cfg = {0};
    struct eth_default_dest_list eth_default_dest_list = {0};
    struct wrx_queue_config wrx_queue_config = {0};
    struct wtx_port_config wtx_port_config = {0};
    struct wtx_queue_config wtx_queue_config = {0};
    int i;

    *CDM_CFG = CDM_CFG_RAM1_SET(0x00) | CDM_CFG_RAM0_SET(0x00);

    udelay(10);

    *CFG_WRX_HTUTS      = g_atm_priv_data.max_connections + OAM_HTU_ENTRY_NUMBER;
    *CFG_WRDES_DELAY    = write_descriptor_delay;
    *WRX_EMACH_ON       = 0x03;
    *WTX_EMACH_ON       = 0xFF;
    *WRX_HUNT_BITTH     = DEFAULT_RX_HUNT_BITTH;

    eth_ports_cfg.wan_vlanid_hi = 0;
    eth_ports_cfg.wan_vlanid_lo = 0;
    eth_ports_cfg.eth0_type = g_eth_wan_mode;   //  2 - ETH0 mixed, 0 - DSL WAN
    *ETH_PORTS_CFG = eth_ports_cfg;

    lan_rout_tbl_cfg.lan_rout_num   = LAN_ROUT_NUM;
    lan_rout_tbl_cfg.lan_rout_off   = LAN_ROUT_OFF;
    lan_rout_tbl_cfg.lan_rout_drop  = 0;
    *LAN_ROUT_TBL_CFG = lan_rout_tbl_cfg;

    wan_rout_tbl_cfg.wan_rout_num       = WAN_ROUT_NUM;
    wan_rout_tbl_cfg.wan_rout_mc_num    = WAN_ROUT_MC_NUM;
    wan_rout_tbl_cfg.wan_rout_mc_drop   = 0;
    wan_rout_tbl_cfg.wan_rout_drop      = 0;
    *WAN_ROUT_TBL_CFG = wan_rout_tbl_cfg;

    gen_mode_cfg.wan_fast_mode          = 1;
    gen_mode_cfg.us_early_discard_en    = 1;
    gen_mode_cfg.fast_path_wfq          = 0x01;
    gen_mode_cfg.dplus_wfq              = 0x05;
    gen_mode_cfg.etx_wfq                = 0x10;
    gen_mode_cfg.wan_acc_mode           = 2;
    gen_mode_cfg.lan_acc_mode           = 2;
    gen_mode_cfg.acc_mode               = 3;
    *GEN_MODE_CFG = gen_mode_cfg;

    brg_tbl_cfg.brg_entry_num   = BRIDGING_ENTRY_NUM;
    *BRG_TBL_CFG = brg_tbl_cfg;

    *CFG_ROUT_MAC_NO = 0;

    eth_default_dest_list.in_etag_en            = 0;
    eth_default_dest_list.srcip_vlan_en         = 0;
    eth_default_dest_list.ethtype_vlan_en       = 0;
    eth_default_dest_list.vlan_vlan_en          = 0;
    eth_default_dest_list.loop_ctl              = 0;
    eth_default_dest_list.vlan_en               = 0;
    eth_default_dest_list.mac_change_drop       = 0;
    eth_default_dest_list.port_en               = 0;
    eth_default_dest_list.out_itag_vld          = 1;
    eth_default_dest_list.out_etag_en           = 0;
    eth_default_dest_list.unknown_mc_dest_list  = 4;    //  CPU
    eth_default_dest_list.unknown_uc_dest_list  = 4;    //  CPU
    for ( i = 0; i < 8; i++ )
        *ETH_DEFAULT_DEST_LIST(i) = eth_default_dest_list;

    //  fake setting, when open ATM VC, it will be set with the real value
    wrx_queue_config.mpoa_type  = 3;    //  IPoA
    wrx_queue_config.ip_ver     = 0;
    wrx_queue_config.mpoa_mode  = 0;    //  VC mux
    wrx_queue_config.oversize   = DMA_PACKET_SIZE;
    wrx_queue_config.undersize  = 0;
    wrx_queue_config.mfs        = DMA_PACKET_SIZE;
    wrx_queue_config.uumask     = 0xFF;
    wrx_queue_config.cpimask    = 0xFF;
    wrx_queue_config.uuexp      = 0;
    wrx_queue_config.cpiexp     = 0;
    for ( i = 0; i < 8; i++ )
        *WRX_QUEUE_CONFIG(i) = wrx_queue_config;

    wtx_port_config.qid     = 0;
    wtx_port_config.qsben   = 1;
    for ( i = 0; i < 2; i++ )
        *WTX_PORT_CONFIG(i) = wtx_port_config;

    //  fake setting, when open ATM VC, it will be set with the real value
    wtx_queue_config.uu             = 0;
    wtx_queue_config.cpi            = 0;
    wtx_queue_config.same_vc_qmap   = 0;
    wtx_queue_config.sbid           = 0;
    wtx_queue_config.mpoa_mode      = 0;    //  VC mux
    wtx_queue_config.qsben          = 1;
    wtx_queue_config.atm_header     = 0;
    for ( i = 0; i < 8; i++ )
    {
        wtx_queue_config.qsb_vcid   = i + QSB_QUEUE_NUMBER_BASE;
        *WTX_QUEUE_CONFIG(i) = wtx_queue_config;
    }

    *MTU_CFG_TBL(0) = DMA_PACKET_SIZE;          //  for ATM
    for ( i = 1; i < 8; i++ )
        *MTU_CFG_TBL(i) = ETH_MAX_DATA_LENGTH;  //  for ETH

    BRIDGING_FORWARD_TBL(0)[2] = 0x00000400;
    BRIDGING_FORWARD_TBL(0)[1] = 0xFFFF0000;
    BRIDGING_FORWARD_TBL(0)[0] = 0xFFFFFFFF;
    for ( i = 1; i < BRIDGING_ENTRY_NUM; i++ )
    {
        BRIDGING_FORWARD_TBL(i)[0] = 0xFFFFFFFF;
        BRIDGING_FORWARD_TBL(i)[1] = 0xFFFFFFFF;
        BRIDGING_FORWARD_TBL(i)[2] = 0xFFFFFFFF;
    }

//    for ( i = 0; i < 8; i++ )
//        *VLAN_PRI_TO_WAN_TX_QID_MAPPING(i) = g_vlan_queue_map[i];

    setup_oam_htu_entry();
}

static INLINE int alloc_dma(void)
{
    int ret;
    struct sk_buff *skb_pool[PRE_ALLOC_DESC_TOTAL_NUM];
    struct sk_buff **pskb;
    volatile u32 *p;
    int i, j;

    for ( i = 0; i < PRE_ALLOC_DESC_TOTAL_NUM; i++ )
    {
        skb_pool[i] = alloc_skb_rx();
        if ( !skb_pool[i] )
        {
            err("alloc_skb_rx fail");
            ret = -ENOMEM;
            goto ALLOC_SKB_FAIL;
        }
#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
        g_dbg_skb_swap_pool[i] = skb_pool[i];
#endif
    }

    g_dma_rx_ch2_desc_base = (unsigned int)kzalloc(DMA_RX_CH2_DESC_NUM * sizeof(*g_dma_rx_ch2_desc) + 32, GFP_ATOMIC);
    g_dma_rx_ch2_desc = (struct rx_descriptor *)((g_dma_rx_ch2_desc_base + 31) & ~31);
#ifndef CONFIG_MIPS_UNCACHED
    dma_cache_wback_inv((unsigned long)g_dma_rx_ch2_desc, DMA_RX_CH2_DESC_NUM * sizeof(g_dma_rx_ch2_desc));
#endif

    g_dma_device = dma_device_reserve("PPE");
    if ( !g_dma_device )
    {
        ret = -EIO;
        goto RESERVE_DMA_FAIL;
    }

    g_dma_device->buffer_alloc    = dma_buffer_alloc;
    g_dma_device->buffer_free     = dma_buffer_free;
    g_dma_device->intr_handler    = dma_int_handler;
    g_dma_device->max_rx_chan_num = 4;
    g_dma_device->max_tx_chan_num = 2;
    g_dma_device->tx_burst_len    = 4;
    g_dma_device->rx_burst_len    = 4;

    for ( i = 0; i < g_dma_device->max_rx_chan_num; i++ )
    {
        g_dma_device->rx_chan[i]->packet_size = DMA_PACKET_SIZE;
        g_dma_device->rx_chan[i]->control     = IFX_DMA_CH_ON;
        //g_dma_device->rx_chan[i]->channel_packet_drop_enable    = 1;
    }
    g_dma_device->rx_chan[1]->desc_base = (int)DMA_RX_CH1_DESC_BASE;
    g_dma_device->rx_chan[1]->desc_len  = DMA_RX_CH1_DESC_NUM;
    g_dma_device->rx_chan[2]->desc_base = (int)DMA_RX_CH2_DESC_BASE;
    g_dma_device->rx_chan[2]->desc_len  = DMA_RX_CH2_DESC_NUM;
    g_dma_device->rx_chan[1]->req_irq_to_free   = g_dma_device->rx_chan[1]->irq;
    g_dma_device->rx_chan[2]->req_irq_to_free   = g_dma_device->rx_chan[2]->irq;


    for ( i = 0; i < g_dma_device->max_tx_chan_num; i++ )
    {
        g_dma_device->tx_chan[i]->control     = IFX_DMA_CH_ON;
    }
    g_dma_device->tx_chan[1]->desc_base = (int)DMA_TX_CH1_DESC_BASE;
    g_dma_device->tx_chan[1]->desc_len  = DMA_TX_CH1_DESC_NUM;
    g_dma_device->tx_chan[1]->global_buffer_len = DMA_PACKET_SIZE;
    g_dma_device->tx_chan[1]->peri_to_peri      = 1;

    g_dma_device->port_packet_drop_enable = 1;

    if ( dma_device_register(g_dma_device) != IFX_SUCCESS )
    {
        err("failed in \"dma_device_register\"");
        ret = -EIO;
        goto DMA_DEVICE_REGISTER_FAIL;
    }

    pskb = skb_pool;

    p = (volatile u32 *)CPU_TO_WAN_TX_DESC_BASE;
    for ( i = 0; i < CPU_TO_WAN_TX_DESC_NUM; i++ )
    {
#if defined(ENABLE_SKB_SWAP_DBG) && ENABLE_SKB_SWAP_DBG
        g_dbg_skb_data[g_dbg_skb_p] = (u32)(*pskb)->data & 0x1FFFFFF0;
        g_dbg_skb[g_dbg_skb_p] = (u32)*pskb;
        g_dbg_skb_p++;
#endif

        ASSERT(((u32)(*pskb)->data & 15) == 0, "CPU to WAN TX data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    for ( j = 0; j < 8; j++ )
    {
        p = (volatile u32 *)WAN_TX_DESC_BASE(j);
        for ( i = 0; i < WAN_TX_DESC_NUM(j); i++ )
        {
#if defined(ENABLE_SKB_SWAP_DBG) && ENABLE_SKB_SWAP_DBG
            g_dbg_skb_data[g_dbg_skb_p] = (u32)(*pskb)->data & 0x1FFFFFF0;
            g_dbg_skb[g_dbg_skb_p] = (u32)*pskb;
            g_dbg_skb_p++;
#endif

            ASSERT(((u32)(*pskb)->data & 15) == 0, "WAN (ATM) TX data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
            *p++ = 0x30000000;
            *p++ = ((u32)(*pskb++)->data & 0x1FFFFFF0) >> 2;
        }
    }

    p = (volatile u32 *)WAN_RX_DESC_BASE(0);
    for ( i = 0; i < WAN_RX_DESC_NUM(0); i++ )
    {
        ASSERT(((u32)(*pskb)->data & 3) == 0, "WAN (ATM) RX channel 0 (AAL) data pointer (0x%#x) must be DWORD aligned", (u32)(*pskb)->data);
        *p++ = 0x30000000 | DMA_PACKET_SIZE;
        *p++ = ((u32)(*pskb++)->data & 0x1FFFFFFC) >> 2;
    }

    p = (volatile u32 *)WAN_RX_DESC_BASE(1);
    for ( i = 0; i < WAN_RX_DESC_NUM(1); i++ )
    {
        ASSERT(((u32)(*pskb)->data & 3) == 0, "WAN (ATM) RX channel 0 (OAM) data pointer (0x%#x) must be DWORD aligned", (u32)(*pskb)->data);
        g_wan_rx1_desc_skb[i] = *pskb;
        *p++ = 0x30000000 | DMA_PACKET_SIZE;
        *p++ = ((u32)(*pskb++)->data & 0x1FFFFFFC) >> 2;
    }

    p = (volatile u32 *)DMA_RX_CH1_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH1_DESC_NUM; i++ )
    {
#if defined(ENABLE_SKB_SWAP_DBG) && ENABLE_SKB_SWAP_DBG
        g_dbg_skb_data[g_dbg_skb_p] = (u32)(*pskb)->data & 0x1FFFFFF0;
        g_dbg_skb[g_dbg_skb_p] = (u32)*pskb;
        g_dbg_skb_p++;
#endif

        ASSERT(((u32)(*pskb)->data & 15) == 0, "DMA RX channel 1 data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0x80000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    p = (volatile u32 *)DMA_RX_CH2_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH2_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 15) == 0, "DMA RX channel 2 data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
    p = (volatile u32 *)WLAN_CPU_TX_DESC_BASE;
    for ( i = 0; i < WLAN_CPU_TX_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 3) == 0, "WLAN CPU TX data pointer (0x%#x) must be DWORD aligned", (u32)(*pskb)->data);
        g_wlan_cpu_tx_desc_skb[i] = *pskb;
        *p++ = 0x30000000;
        *p++ = ((u32)(*pskb++)->data & 0x1FFFFFFC) >> 2;
    }
#endif

    g_f_dma_opened = 0;
    g_mailbox_signal_mask |= DMA_TX_CH1_SIG;

    return 0;

DMA_DEVICE_REGISTER_FAIL:
    dma_device_release(g_dma_device);
    g_dma_device = NULL;
RESERVE_DMA_FAIL:
    i = PRE_ALLOC_DESC_TOTAL_NUM;
ALLOC_SKB_FAIL:
    while ( i-- )
        dev_kfree_skb_any(skb_pool[i]);
    return ret;
}

static INLINE void free_dma(void)
{
    volatile struct tx_descriptor *p;
    struct sk_buff *skb_to_free;
    int i, j;

    p = (volatile struct tx_descriptor *)CPU_TO_WAN_TX_DESC_BASE;
    for ( i = 0; i < CPU_TO_WAN_TX_DESC_NUM; i++ )
    {
        skb_to_free = get_skb_pointer(p->dataptr, 0);
        atm_free_tx_skb_vcc_delayed(skb_to_free);
        p++;
    }

    for ( j = 0; j < 8; j++ )
    {
        p = (volatile struct tx_descriptor *)WAN_TX_DESC_BASE(j);
        for ( i = 0; i < WAN_TX_DESC_NUM(j); i++ )
        {
            skb_to_free = get_skb_pointer(p->dataptr, 2);
            atm_free_tx_skb_vcc_delayed(skb_to_free);
            p++;
        }
    }

    p = (volatile struct tx_descriptor *)WAN_RX_DESC_BASE(0);
    for ( i = 0; i < WAN_RX_DESC_NUM(0); i++ )
    {
        skb_to_free = get_skb_pointer(p->dataptr, 2);
        dev_kfree_skb_any(skb_to_free);
        p++;
    }

    for ( i = 0; i < WAN_RX_DESC_NUM(1); i++ )
    {
        skb_to_free = g_wan_rx1_desc_skb[i];
        get_skb_from_dbg_pool(skb_to_free);
        dev_kfree_skb_any(skb_to_free);
    }

    p = (volatile struct tx_descriptor *)DMA_RX_CH1_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH1_DESC_NUM; i++ )
    {
        skb_to_free = get_skb_pointer(p->dataptr, 0);
        atm_free_tx_skb_vcc_delayed(skb_to_free);
        p++;
    }

    p = (volatile struct tx_descriptor *)DMA_RX_CH2_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH2_DESC_NUM; i++ )
    {
        skb_to_free = get_skb_pointer(p->dataptr, 0);
        dev_kfree_skb_any(skb_to_free);
        p++;
    }

#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
    for ( i = 0; i < WLAN_CPU_TX_DESC_NUM; i++ )
    {
        skb_to_free = g_wlan_cpu_tx_desc_skb[i];
        get_skb_from_dbg_pool(skb_to_free);
        dev_kfree_skb_any(skb_to_free);
    }
#endif

    dma_device_unregister(g_dma_device);
    dma_device_release(g_dma_device);
    g_dma_device = NULL;

    if ( g_dma_rx_ch2_desc_base != 0 )
    {
        kfree((void *)g_dma_rx_ch2_desc_base);
        g_dma_rx_ch2_desc_base = 0;
    }

#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
    for ( i = 0; i < NUM_ENTITY(g_dbg_skb_swap_pool); i++ )
        if ( g_dbg_skb_swap_pool[i] != NULL )
        {
            err("skb swap pool is not clean: %d - %08x\n", i, (unsigned int)g_dbg_skb_swap_pool[i]);
        }
#endif
}

#if defined(CONFIG_IFX_PPA_DATAPATH)
static INLINE void ethaddr_setup(unsigned int port, char *line)
{
    u8 *p;
    char *ep;
    int i;

    p = MY_ETH0_ADDR;
    memset(p, 0, MAX_ADDR_LEN * sizeof(*p));
    for ( i = 0; i < 6; i++ )
    {
        p[i] = line ? simple_strtoul(line, &ep, 16) : 0;
        if ( line )
            line = *ep ? ep + 1 : ep;
    }
    dbg("eth%d mac address %02X-%02X-%02X-%02X-%02X-%02X",
        port ? 1 : 0,
        (u32)p[0] & 0xFF, (u32)p[1] & 0xFF, (u32)p[2] & 0xFF,
        (u32)p[3] & 0xFF, (u32)p[4] & 0xFF, (u32)p[5] & 0xFF);
}
#endif

static void dsl_led_polling(unsigned long arg)
{
    if ( DSL_WAN_MIB_TABLE->wrx_correct_pdu != g_dsl_wrx_correct_pdu || DSL_WAN_MIB_TABLE->wtx_total_pdu != g_dsl_wtx_total_pdu )
    {
        g_dsl_wrx_correct_pdu = DSL_WAN_MIB_TABLE->wrx_correct_pdu;
        g_dsl_wtx_total_pdu = DSL_WAN_MIB_TABLE->wtx_total_pdu;

        adsl_led_flash();
    }

    if ( ifx_atm_alloc_tx != NULL )
    {
        g_dsl_led_polling_timer.expires = jiffies + HZ / 10;    //  100ms
        add_timer(&g_dsl_led_polling_timer);
    }
}

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

    return d;
}
#endif

static int print_fw_ver(char *buf, int buf_len)
{
    static char * fw_ver_family_str[] = {
        "reserved - 0",
        "Danube",
        "Twinpass",
        "Amazon-SE",
        "reserved - 4",
        "AR9",
        "GR9"
    };
    static char * fw_ver_type_str[] = {
        "reserved - 0",
        "Standard",
        "Acceleration",
        "VDSL2 Bonding"
    };
    static char * fw_ver_interface_str[] = {
        "MII0",
        "MII0 + MII1",
        "MII0 + ATM",
        "MII0 + PTM",
        "MII0/1 + ATM",
        "MII0/1 + PTM"
    };
    static char * fw_ver_mode_str[] = {
        "reserved - 0",
        "reserved - 1",
        "reserved - 2",
        "Routing",
        "reserved - 4",
        "Bridging",
        "Bridging + IPv4 Routing",
        "Bridging + IPv4/6 Routing"
    };

    int len = 0;

    len += snprintf(buf + len, buf_len - len, "PPE firmware info:\n");
    len += snprintf(buf + len, buf_len - len, "  Version ID: %d.%d.%d.%d.%d.%d\n", (int)FW_VER_ID->family, (int)FW_VER_ID->fwtype, (int)FW_VER_ID->interface, (int)FW_VER_ID->fwmode, (int)FW_VER_ID->major, (int)FW_VER_ID->minor);
    if ( FW_VER_ID->family > NUM_ENTITY(fw_ver_family_str) )
        len += snprintf(buf + len, buf_len - len, "  Family    : reserved - %d\n", (int)FW_VER_ID->family);
    else
        len += snprintf(buf + len, buf_len - len, "  Family    : %s\n", fw_ver_family_str[FW_VER_ID->family]);
    if ( FW_VER_ID->fwtype > NUM_ENTITY(fw_ver_type_str) )
        len += snprintf(buf + len, buf_len - len, "  FW Type   : reserved - %d\n", (int)FW_VER_ID->fwtype);
    else
        len += snprintf(buf + len, buf_len - len, "  FW Type   : %s\n", fw_ver_type_str[FW_VER_ID->fwtype]);
    if ( FW_VER_ID->interface > NUM_ENTITY(fw_ver_interface_str) )
        len += snprintf(buf + len, buf_len - len, "  Interface : reserved - %d\n", (int)FW_VER_ID->interface);
    else
        len += snprintf(buf + len, buf_len - len, "  Interface : %s\n", fw_ver_interface_str[FW_VER_ID->interface]);
    if ( FW_VER_ID->fwmode > NUM_ENTITY(fw_ver_mode_str) )
        len += snprintf(buf + len, buf_len - len, "  Mode      : reserved - %d\n", (int)FW_VER_ID->fwmode);
    else
        len += snprintf(buf + len, buf_len - len, "  Mode      : %s\n", fw_ver_mode_str[FW_VER_ID->fwmode]);
    len += snprintf(buf + len, buf_len - len, "  Release   : %d.%d\n", (int)FW_VER_ID->major, (int)FW_VER_ID->minor);

    return len;
}

static int print_driver_ver(char *buf, int buf_len, char *title, unsigned int family, unsigned int type, unsigned int itf, unsigned int mode, unsigned int major, unsigned int mid, unsigned int minor)
{
    static char * dr_ver_family_str[] = {
        NULL,
        "Danube",
        "Twinpass",
        "Amazon-SE",
        NULL,
        "AR9",
        "GR9"
    };
    static char * dr_ver_type_str[] = {
        "Normal Data Path",
        "Indirect-Fast Path",
        "HAL",
        "Hook",
        "OS Adatpion Layer",
        "PPA API"
    };
    static char * dr_ver_interface_str[] = {
        "MII0",
        "MII1",
        "ATM",
        "PTM"
    };
    static char * dr_ver_accmode_str[] = {
        "Routing",
        "Bridging",
    };

    int len = 0;
    unsigned int bit;
    int i, j;

    len += snprintf(buf + len, buf_len - len, "%s:\n", title);
    len += snprintf(buf + len, buf_len - len, "  Version ID: %d.%d.%d.%d.%d.%d.%d\n", family, type, itf, mode, major, mid, minor);
    len += snprintf(buf + len, buf_len - len, "  Family    : ");
    for ( bit = family, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_family_str); bit >>= 1, i++ )
        if ( (bit & 0x01) && dr_ver_family_str[i] != NULL )
        {
            if ( j )
                len += snprintf(buf + len, buf_len - len, " | %s", dr_ver_family_str[i]);
            else
                len += snprintf(buf + len, buf_len - len, dr_ver_family_str[i]);
            j++;
        }
    if ( j )
        len += snprintf(buf + len, buf_len - len, "\n");
    else
        len += snprintf(buf + len, buf_len - len, "N/A\n");
    len += snprintf(buf + len, buf_len - len, "  DR Type   : ");
    for ( bit = type, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_type_str); bit >>= 1, i++ )
        if ( (bit & 0x01) && dr_ver_type_str[i] != NULL )
        {
            if ( j )
                len += snprintf(buf + len, buf_len - len, " | %s", dr_ver_type_str[i]);
            else
                len += snprintf(buf + len, buf_len - len, dr_ver_type_str[i]);
            j++;
        }
    if ( j )
        len += snprintf(buf + len, buf_len - len, "\n");
    else
        len += snprintf(buf + len, buf_len - len, "N/A\n");
    len += snprintf(buf + len, buf_len - len, "  Interface : ");
    for ( bit = itf, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_interface_str); bit >>= 1, i++ )
        if ( (bit & 0x01) && dr_ver_interface_str[i] != NULL )
        {
            if ( j )
                len += snprintf(buf + len, buf_len - len, " | %s", dr_ver_interface_str[i]);
            else
                len += snprintf(buf + len, buf_len - len, dr_ver_interface_str[i]);
            j++;
        }
    if ( j )
        len += snprintf(buf + len, buf_len - len, "\n");
    else
        len += snprintf(buf + len, buf_len - len, "N/A\n");
    len += snprintf(buf + len, buf_len - len, "  Mode      : ");
    for ( bit = mode, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_accmode_str); bit >>= 1, i++ )
        if ( (bit & 0x01) && dr_ver_accmode_str[i] != NULL )
        {
            if ( j )
                len += snprintf(buf + len, buf_len - len, " | %s", dr_ver_accmode_str[i]);
            else
                len += snprintf(buf + len, buf_len - len, dr_ver_accmode_str[i]);
            j++;
        }
    if ( j )
        len += snprintf(buf + len, buf_len - len, "\n");
    else
        len += snprintf(buf + len, buf_len - len, "N/A\n");
    len += snprintf(buf + len, buf_len - len, "  Release   : %d.%d.%d\n", major, mid, minor);

    return len;
}

static INLINE void proc_file_create(void)
{
    struct proc_dir_entry *res;

    g_eth_proc_dir = proc_mkdir("eth", NULL);

    create_proc_read_entry("ver",
                           0,
                           g_eth_proc_dir,
                           proc_read_ver,
                           NULL);

    res = create_proc_entry("mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_mib;
        res->write_proc = proc_write_mib;
    }

    create_proc_read_entry("dslmib",
                           0,
                           g_eth_proc_dir,
                           proc_read_dsl_mib,
                           NULL);

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC

    res = create_proc_entry("route",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_route;
        res->write_proc = proc_write_route;
    }

    res = create_proc_entry("bridge",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_bridge;
        res->write_proc = proc_write_bridge;
    }

    create_proc_read_entry("mc",
                           0,
                           g_eth_proc_dir,
                           proc_read_mc,
                           NULL);

    res = create_proc_entry("genconf",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_genconf;
        res->write_proc = proc_write_genconf;
    }

    res = create_proc_entry("queue",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_queue;
        res->write_proc = proc_write_queue;
    }

    create_proc_read_entry("pppoe",
                           0,
                           g_eth_proc_dir,
                           proc_read_pppoe,
                           NULL);

    create_proc_read_entry("mtu",
                           0,
                           g_eth_proc_dir,
                           proc_read_mtu,
                           NULL);

    res = create_proc_entry("hit",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_hit;
        res->write_proc = proc_write_hit;
    }

    create_proc_read_entry("mac",
                           0,
                           g_eth_proc_dir,
                           proc_read_mac,
                           NULL);

    create_proc_read_entry("vlan",
                           0,
                           g_eth_proc_dir,
                           proc_read_vlan,
                           NULL);

    create_proc_read_entry("out_vlan",
                           0,
                           g_eth_proc_dir,
                           proc_read_out_vlan,
                           NULL);

#endif

#if defined(DEBUG_HTU_PROC) && DEBUG_HTU_PROC
    create_proc_read_entry("htu",
                           0,
                           g_eth_proc_dir,
                           proc_read_htu,
                           NULL);
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    res = create_proc_entry("dbg",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_dbg;
        res->write_proc = proc_write_dbg;
    }
#endif

#if defined(DEBUG_MEM_PROC) && DEBUG_MEM_PROC
    res = create_proc_entry("mem",
                            0,
                            g_eth_proc_dir);
    if ( res )
        res->write_proc = proc_write_mem;
#endif

#if defined(DEBUG_PP32_PROC) && DEBUG_PP32_PROC
    res = create_proc_entry("pp32",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_pp32;
        res->write_proc = proc_write_pp32;
    }
#endif

    res = create_proc_entry("burstlen",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_burstlen;
        res->write_proc = proc_write_burstlen;
    }

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
    res = create_proc_entry("fw",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_fw;
        res->write_proc = proc_write_fw;
    }
#endif

#if defined(DEBUG_SEND_PROC) && DEBUG_SEND_PROC
    res = create_proc_entry("send",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_send;
        res->write_proc = proc_write_send;
    }
#endif

    res = create_proc_entry("prio",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_prio;
        res->write_proc = proc_write_prio;
    }

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    res = create_proc_entry("softq",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_softq;
        res->write_proc = proc_write_softq;
    }
#endif
}

static INLINE void proc_file_delete(void)
{
#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND
    remove_proc_entry("softq",
                      g_eth_proc_dir);
#endif

    remove_proc_entry("prio",
                      g_eth_proc_dir);

#if defined(DEBUG_SEND_PROC) && DEBUG_SEND_PROC
    remove_proc_entry("send",
                      g_eth_proc_dir);
#endif

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
    remove_proc_entry("fw",
                      g_eth_proc_dir);
#endif

    remove_proc_entry("burstlen",
                      g_eth_proc_dir);

#if defined(DEBUG_PP32_PROC) && DEBUG_PP32_PROC
    remove_proc_entry("pp32",
                      g_eth_proc_dir);
#endif

#if defined(DEBUG_MEM_PROC) && DEBUG_MEM_PROC
    remove_proc_entry("mem",
                      g_eth_proc_dir);
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    remove_proc_entry("dbg",
                      g_eth_proc_dir);
#endif

#if defined(DEBUG_HTU_PROC) && DEBUG_HTU_PROC
    remove_proc_entry("htu",
                      g_eth_proc_dir);
#endif

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC

    remove_proc_entry("out_vlan",
                      g_eth_proc_dir);

    remove_proc_entry("vlan",
                      g_eth_proc_dir);

    remove_proc_entry("mac",
                      g_eth_proc_dir);

    remove_proc_entry("hit",
                      g_eth_proc_dir);

    remove_proc_entry("mtu",
                      g_eth_proc_dir);

    remove_proc_entry("pppoe",
                      g_eth_proc_dir);

    remove_proc_entry("queue",
                      g_eth_proc_dir);

    remove_proc_entry("genconf",
                      g_eth_proc_dir);

    remove_proc_entry("mc",
                      g_eth_proc_dir);

    remove_proc_entry("bridge",
                      g_eth_proc_dir);

    remove_proc_entry("route",
                      g_eth_proc_dir);

#endif

    remove_proc_entry("dslmib",
                      g_eth_proc_dir);

    remove_proc_entry("mib",
                      g_eth_proc_dir);

    remove_proc_entry("ver",
                      g_eth_proc_dir);

    remove_proc_entry("eth", NULL);
}

static int proc_read_ver(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += print_driver_ver(page + len, count - len, "PPE datapath driver info", VER_FAMILY, VER_DRTYPE, VER_INTERFACE, VER_ACCMODE, VER_MAJOR, VER_MID, VER_MINOR);
    len += print_fw_ver(page + len, count - len);

    *eof = 1;

    return len;
}

static int proc_read_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    struct eth_priv_data *priv;
    char *itf_name[] = {"eth0", "atm", "cpu", "ext_int1", "ext_int2", "ext_int3", "ext_int4", "ext_int5"};
    char *row_name[] = {"ig_fast_brg_uni_pkts", "ig_fast_brg_mul_pkts", "ig_fast_brg_br_pkts", NULL, "ig_fast_rt_uni_udp_pkts", "ig_fast_rt_uni_tcp_pkts", "ig_fast_rt_mul_pkts", NULL, "ig_fast_rt_bytes", "ig_fast_brg_bytes", "ig_cpu_pkts", "ig_cpu_bytes", "ig_drop_pkts", "ig_drop_bytes", "eg_fast_pkts", "eg_fast_bytes"};
    int i, j, k, h;

    MOD_INC_USE_COUNT;

    eth_get_stats(g_eth_net_dev[0]);
    priv = ifx_eth_fw_netdev_priv(g_eth_net_dev[0]);

    len += sprintf(page + len, "Ethernet\n");

    len += sprintf(page + len,         "  Hardware (eth0)\n");
    len += sprintf(page + len,         "    enets_igerr:  %u\n", priv->enets_igerr);
    len += sprintf(page + len,         "    enets_igdrop: %u\n", priv->enets_igdrop);
    len += sprintf(page + len,         "    enetf_egcol:  %u\n", priv->enetf_egcol);
    len += sprintf(page + len,         "    enetf_egdrop: %u\n", priv->enetf_egdrop);

    len += sprintf(page + len, "  Firmware  (");
    for ( i = h = 0; i < NUM_ENTITY(itf_name); i++ )
        if ( (g_mib_itf & (1 << i)) )
            len += sprintf(page + len, h++ == 0 ? "%s" : ", %s", itf_name[i]);
    len += sprintf(page + len, ")\n");
    for ( i = k = 0; i < NUM_ENTITY(row_name); i++ )
        if ( row_name[i] != NULL && k < (j = strlen(row_name[i])) )
            k = j;
    k += 6;
    for ( i = 0; i < NUM_ENTITY(row_name); i++ )
    {
        if ( row_name[i] == NULL )
            continue;

        len += (j = sprintf(page + len, "    %s: ", row_name[i]));
        for ( j = k - j; j > 0; j-- )
            page[len++] = ' ';
        for ( j = h = 0; j < 8; j++ )
            if ( (g_mib_itf & (1 << j)) )
                len += sprintf(page + len, h++ == 0 ? "%10u" : ", %10u", ((volatile unsigned int *)ITF_MIB_TBL(j))[i]);
        len += sprintf(page + len, "\n");
    }

    len += sprintf(page + len, "  Driver (eth0, atm)\n");
    len += sprintf(page + len, "    rx_packets: %10u, %10u\n",         priv->rx_packets,    g_atm_priv_data.wrx_pdu);
    len += sprintf(page + len, "    rx_bytes:   %10u,        N/A\n",   priv->rx_bytes);
    len += sprintf(page + len, "    rx_dropped: %10u, %10u\n",         priv->rx_dropped,    g_atm_priv_data.wrx_drop_pdu);
    len += sprintf(page + len, "    tx_packets: %10u, %10u\n",         priv->tx_packets,    g_atm_priv_data.wtx_pdu);
    len += sprintf(page + len, "    tx_bytes:   %10u,        N/A\n",   priv->tx_bytes);
    len += sprintf(page + len, "    tx_errors:  %10u, %10u\n",         priv->tx_errors,     g_atm_priv_data.wtx_err_pdu);
    len += sprintf(page + len, "    tx_dropped: %10u, %10u\n",         priv->tx_dropped,    g_atm_priv_data.wtx_drop_pdu);

    MOD_DEC_USE_COUNT;

    *eof = 1;

    ASSERT(len <= 4096, "proc read buffer overflow");
    return len;
}

static int proc_write_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *p1, *p2;
    int len;
    int colon;
    char local_buf[1024];

    struct eth_priv_data *priv;
    unsigned int mib_itf = 0;
    char *itf_name[] = {"eth0", "atm", "cpu", "ext_int1", "ext_int2", "ext_int3", "ext_int4", "ext_int5"};
    int i;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    p2 = NULL;
    colon = 1;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "pvc") == 0 )
        {
            u32 vpi, vci;
            int conn;

            ignore_space(&p2, &len);
            vpi = get_number(&p2, &len, 0);
            ignore_space(&p2, &len);
            vci = get_number(&p2, &len, 0);

            conn = find_vpivci(vpi, vci);

            printk("vpi = %u, vci = %u, conn = %d\n", vpi, vci, conn);

            if ( conn >= 0 )
            {
                printk("rx_packets  = %u\n", g_atm_priv_data.connection[conn].rx_packets);
                printk("rx_bytes    = %u\n", g_atm_priv_data.connection[conn].rx_bytes);
                printk("rx_dropped  = %u\n", g_atm_priv_data.connection[conn].rx_dropped);
                printk("tx_packets  = %u\n", g_atm_priv_data.connection[conn].tx_packets);
                printk("tx_bytes    = %u\n", g_atm_priv_data.connection[conn].tx_bytes);
                printk("tx_errors   = %u\n", g_atm_priv_data.connection[conn].tx_errors);
                printk("tx_dropped  = %u\n", g_atm_priv_data.connection[conn].tx_dropped);
                printk("prio_tx_packets:");
                for ( i = 0; i < 8; i++ )
                    printk(" %u", g_atm_priv_data.connection[conn].prio_tx_packets[i]);
                printk("\n");
            }
        }
        else if ( stricmp(p1, "on") == 0 || stricmp(p1, "enable") == 0 )
            mib_itf = 0x40000000;
        else if ( stricmp(p1, "off") == 0 || stricmp(p1, "disable") == 0 )
            mib_itf = 0x20000000;
        else if ( stricmp(p1, "clear") == 0 || stricmp(p1, "clean") == 0 )
            mib_itf = 0x80000000;
        else if ( (mib_itf & 0xE0000000) )
        {
            if ( stricmp(p1, "all") == 0 )
                mib_itf |= 0xFF;
            else if ( strlen(p1) == 1 && *p1 >= '0' && *p1 <= '7' )
                mib_itf |= 1 << (*p1 - '0');
            else
            {
                for ( i = 0; i < NUM_ENTITY(itf_name); i++ )
                    if ( stricmp(p1, itf_name[i]) == 0 )
                    {
                        mib_itf |= 1 << i;
                        break;
                    }
            }
        }

        p1 = p2;
        colon = 1;
    }

    if ( (mib_itf & 0x40000000) )
        g_mib_itf |= mib_itf & 0xFF;
    else if ( (mib_itf & 0x20000000) )
        g_mib_itf &= ~(mib_itf & 0xFF);
    else if ( (mib_itf & 0x80000000) )
    {
        if ( mib_itf == 0x80000000 )
            mib_itf = 0x800000FF;
        if ( (mib_itf & 0x01) )
        {
            eth_get_stats(g_eth_net_dev[0]);
            priv = ifx_eth_fw_netdev_priv(g_eth_net_dev[0]);
            memset(priv, 0, (u32)&priv->tx_dropped - (u32)priv + sizeof(u32));
        }
        if ( (mib_itf & 0x02) )
        {
            memset(&g_atm_priv_data.wrx_total_byte, 0, (u32)&g_atm_priv_data.prev_mib - (u32)&g_atm_priv_data.wrx_total_byte);
            for ( i = 0; i < ATM_QUEUE_NUMBER; i++ )
                memset(&g_atm_priv_data.connection[i].rx_packets, 0, (u32)&g_atm_priv_data.connection[i].port - (u32)&g_atm_priv_data.connection[i].rx_packets);
            memset((void *)DSL_WAN_MIB_TABLE, 0, sizeof(struct dsl_wan_mib_table));
            for ( i = 0; i < ATM_QUEUE_NUMBER; i++ )
            {
                volatile struct dsl_wan_vc_mib_table *pdsl_mib = DSL_WAN_VC_MIB_TABLE(i);
                memset((void *)pdsl_mib, 0, sizeof(struct dsl_wan_vc_mib_table));
            }
        }
        for ( i = 0; i < 8; i++ )
            if ( (mib_itf & (1 << i)) )
            {
                void *ptmp = (void *)ITF_MIB_TBL(i);
                memset(ptmp, 0, sizeof(struct itf_mib));
            }
    }
    else
    {
        printk("echo <on|off|clear> <all|eth0|atm|cpu|ext_int?> > /proc/eth/mib\n");
        printk("echo pvc <vpi>.<vci> > /proc/eth/mib\n");
    }

    return count;
}

static int proc_read_dsl_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + len,         "DSL WAN MIB\n");
    len += sprintf(page + len,         "  wrx_drophtu_cell: %u\n", DSL_WAN_MIB_TABLE->wrx_drophtu_cell);
    len += sprintf(page + len,         "  wrx_dropdes_pdu:  %u\n", DSL_WAN_MIB_TABLE->wrx_dropdes_pdu);
    len += sprintf(page + len,         "  wrx_correct_pdu:  %u\n", DSL_WAN_MIB_TABLE->wrx_correct_pdu);
    len += sprintf(page + len,         "  wrx_err_pdu:      %u\n", DSL_WAN_MIB_TABLE->wrx_err_pdu);
    len += sprintf(page + len,         "  wrx_dropdes_cell: %u\n", DSL_WAN_MIB_TABLE->wrx_dropdes_cell);
    len += sprintf(page + len,         "  wrx_correct_cell: %u\n", DSL_WAN_MIB_TABLE->wrx_correct_cell);
    len += sprintf(page + len,         "  wrx_err_cell:     %u\n", DSL_WAN_MIB_TABLE->wrx_err_cell);
    len += sprintf(page + len,         "  wrx_total_byte:   %u\n", DSL_WAN_MIB_TABLE->wrx_total_byte);
    len += sprintf(page + len,         "  wtx_total_pdu:    %u\n", DSL_WAN_MIB_TABLE->wtx_total_pdu);
    len += sprintf(page + len,         "  wtx_total_cell:   %u\n", DSL_WAN_MIB_TABLE->wtx_total_cell);
    len += sprintf(page + len,         "  wtx_total_byte:   %u\n", DSL_WAN_MIB_TABLE->wtx_total_byte);

    *eof = 1;

    return len;
}
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC

static int proc_read_route(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    struct rout_forward_compare_tbl *pcompare;
    struct rout_forward_action_tbl *pwaction;
    struct rout_forward_action_tbl *plaction;
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Wan Routing Table\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_ROUTE_OVERRUN_END;

    pcompare = (struct rout_forward_compare_tbl *)WAN_ROUT_FORWARD_COMPARE_TBL(0);
    pwaction = (struct rout_forward_action_tbl *)WAN_ROUT_FORWARD_ACTION_TBL(0);
    for ( i = 0; i < WAN_ROUT_TBL_CFG->wan_rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_wan_route(str, i, pcompare, pwaction);
            if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        pwaction++;
    }

    llen = sprintf(str, "Lan Routing Table\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_ROUTE_OVERRUN_END;

    pcompare = (struct rout_forward_compare_tbl *)LAN_ROUT_FORWARD_COMPARE_TBL(0);
    plaction = (struct rout_forward_action_tbl *)LAN_ROUT_FORWARD_ACTION_TBL(0);
    for ( i = 0; i < LAN_ROUT_TBL_CFG->lan_rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_lan_route(str, i, pcompare, plaction);
            if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        plaction++;
    }

    *eof = 1;

PROC_READ_ROUTE_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_write_route(struct file *file, const char *buf, unsigned long count, void *data)
{
    static const char *command[] = {
        "add",      //  0
        "del",      //  1
        "LAN",      //  2
        "WAN",      //  3
        "new",      //  4
        "src",      //  5
        "dest",     //  6
        "MAC",      //  7
        "route",    //  8
        "type",     //  9
        "DSCP",     //  10
        "MTU",      //  11
        "index",    //  12
        "VLAN",     //  13
        "insert",   //  14
        "remove",   //  15
        "list",     //  16
        "PPPoE",    //  17
        "mode",     //  18
        "ch",       //  19
        "id",       //  20
        "delete",   //  21
        "disable",  //  22
        "enable",   //  23
        "transparent",  //  24
        "termination",  //  25
        "NULL",     //  26
        "IPv4",     //  27
        "NAT",      //  28
        "NAPT",     //  29
        "entry",    //  30
        "help",     //  31
    };

    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};
    static const int dest_list_strlen[] = {4, 4, 4, 8, 8, 8, 8, 8};

    int state;              //  1: new,
    int prev_cmd;
    int operation;          //  1: add, 2: delete
    int type;               //  1: LAN, 2: WAN, 0: auto detect
    int entry;
    struct rout_forward_compare_tbl compare_tbl;
    struct rout_forward_action_tbl action_tbl;
    unsigned int val[6];
    char local_buf[1024];
    int len;
    char *p1, *p2;
    int colon;
    int i, j;
    u32 mask;
    u32 bit;
    u32 *pu1, *pu2;

    MOD_INC_USE_COUNT;

    len = sizeof(local_buf) < count ? sizeof(local_buf) : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    state = 0;
    prev_cmd = 0;
    operation = 0;
    type = 0;
    entry = -1;

    memset(&compare_tbl, 0, sizeof(compare_tbl));
    memset(&action_tbl, 0, sizeof(action_tbl));

    p1 = local_buf;
    colon = 1;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        for ( i = 0; i < sizeof(command) / sizeof(*command); i++ )
            if ( stricmp(p1, command[i]) == 0 )
            {
                switch ( i )
                {
                case 0:
                    if ( !state && !operation )
                    {
                        operation = 1;
//                      printk("add\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 1:
                case 21:
                    if ( !state && !operation )
                    {
                        operation = 2;
//                      printk("delete\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 2:
                    if ( !state && !type )
                    {
                        type = 1;
//                      printk("lan\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 3:
                    if ( !state && !type )
                    {
                        type = 2;
//                      printk("wan\n");
                    }
                    state = prev_cmd = 0;
                    break;
                case 4:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 4;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 5:
                    if ( state == 1 )
                    {
                        if ( prev_cmd == 4 )
                        {
                            //  new src
                            if ( !type )
                                type = 1;

                            get_ip_port(&p2, &len, val);
//                          printk("new src: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                            action_tbl.new_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            action_tbl.new_port = val[4];
                        }
                        else
                            state = 0;
                    }
                    if ( state == 0 )
                    {
                        //  src
                        get_ip_port(&p2, &len, val);
//                      printk("src: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                        compare_tbl.src_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                        compare_tbl.src_port = val[4];
                    }
                    state = prev_cmd = 0;
                    break;
                case 6:
                    if ( state == 1 )
                    {
                        if ( prev_cmd == 4 )
                        {
                            //  new dest
                            if ( !type )
                                type = 2;

                            get_ip_port(&p2, &len, val);
//                          printk("new dest: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                            action_tbl.new_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            action_tbl.new_port = val[4];
                        }
                        else
                            state = 0;
                    }
                    if ( state == 0 )
                    {
                        if ( !colon )
                        {
                            int llen;

                            llen = len;
                            p1 = p2;
                            while ( llen && *p1 <= ' ' )
                            {
                                llen--;
                                p1++;
                            }
                            if ( llen && (*p1 == ':' || (*p1 >= '0' && *p1 <= '9')) )
                                colon = 1;
                        }
                        if ( colon )
                        {
                            //  dest
                            get_ip_port(&p2, &len, val);
//                          printk("dest: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                            compare_tbl.dest_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            compare_tbl.dest_port = val[4];
                        }
                        else
                        {
                            state = 1;
                            prev_cmd = 6;
                            break;
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 7:
                    if ( state == 1 && prev_cmd == 4 )
                    {
                        //  new MAC
                        get_mac(&p2, &len, val);
//                      printk("new MAC: %02X.%02X.%02X.%02X:%02X:%02X\n", val[0], val[1], val[2], val[3], val[4], val[5]);
                        action_tbl.new_dest_mac54 = (val[0] << 8) | val[1];
                        action_tbl.new_dest_mac30 = (val[2] << 24) | (val[3] << 16) | (val[4] << 8) | val[5];
                    }
                    state = prev_cmd = 0;
                    break;
                case 8:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 8;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 9:
                    if ( state == 1 && prev_cmd == 8 )
                    {
                        state = 2;
                        prev_cmd = 9;
                        ignore_space(&p2, &len);
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 10:
                    if ( state == 1 && prev_cmd == 4 )
                    {
                        ignore_space(&p2, &len);
                        if ( len && *p2 >= '0' && *p2 <= '9' )
                        {
                            //  new DSCP
                            val[0] = get_number(&p2, &len, 0);
//                          printk("new DSCP: %d\n", val[0]);
                            if ( !action_tbl.new_dscp_en )
                            {
                                action_tbl.new_dscp_en = 1;
                                action_tbl.new_dscp = val[0];
                            }
                        }
                        else if ( (len == 8 || (len > 8 && (p2[8] <= ' ' || p2[8] == ','))) && strincmp(p2, "original", 8) == 0 )
                        {
                            p2 += 8;
                            len -= 8;
                            //  new DSCP original
//                          printk("new DSCP: original\n");
                            //  the reset value is 0, so don't need to do anything
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 11:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 11;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 12:
                    if ( state == 1 )
                    {
                        if ( prev_cmd == 11 )
                        {
                            //  MTU index
                            ignore_space(&p2, &len);
                            val[0] = get_number(&p2, &len, 0);
//                          printk("MTU index: %d\n", val[0]);
                            action_tbl.mtu_ix = val[0];
                        }
                        else if ( prev_cmd == 13 )
                        {
                            //  VLAN insert enable
                            //  VLAN index
                            ignore_space(&p2, &len);
                            val[0] = get_number(&p2, &len, 0);
//                          printk("VLAN insert: enable, index %d\n", val[0]);
                            if ( !action_tbl.in_vlan_ins )
                            {
                                action_tbl.in_vlan_ins = 1;
                                action_tbl.new_in_vci = val[0];
                            }
                        }
                        else if ( prev_cmd == 17 )
                        {
                            //  PPPoE index
                            ignore_space(&p2, &len);
                            val[0] = get_number(&p2, &len, 0);
//                          printk("PPPoE index: %d\n", val[0]);
                            action_tbl.pppoe_ix = val[0];
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 13:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 13;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 14:
                    if ( state == 1 && prev_cmd == 13 )
                    {
                        state = 2;
                        prev_cmd = 14;
                        ignore_space(&p2, &len);
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 15:
                    if ( state == 1 && prev_cmd == 13 )
                    {
                        state = 2;
                        prev_cmd = 15;
                        ignore_space(&p2, &len);
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 16:
                    if ( state == 1 && prev_cmd == 6 )
                    {
                        mask = 0;
                        do
                        {
                            ignore_space(&p2, &len);
                            if ( !len )
                                break;
                            for ( j = 0, bit = 1; j < sizeof(dest_list) / sizeof(*dest_list); j++, bit <<= 1 )
                                if ( (len == dest_list_strlen[j] || (len > dest_list_strlen[j] && (p2[dest_list_strlen[j]] <= ' ' || p2[dest_list_strlen[j]] == ','))) && strincmp(p2, dest_list[j], dest_list_strlen[j]) == 0 )
                                {
                                    p2 += dest_list_strlen[j];
                                    len -= dest_list_strlen[j];
                                    mask |= bit;
                                    break;
                                }
                        } while ( j < sizeof(dest_list) / sizeof(*dest_list) );
//                      if ( mask )
//                      {
//                          //  dest list
//                          printk("dest list:");
//                          for ( j = 0, bit = 1; j < sizeof(dest_list) / sizeof(*dest_list); j++, bit <<= 1 )
//                              if ( (mask & bit) )
//                              {
//                                  printk(" %s", dest_list[j]);
//                              }
//                          printk("\n");
//                      }
//                      else
//                          printk("dest list: none\n");
                        action_tbl.dest_list = mask;
                    }
                    state = prev_cmd = 0;
                    break;
                case 17:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 17;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 18:
                    if ( state == 1 && prev_cmd == 17 )
                    {
                        state = 2;
                        prev_cmd = 18;
                        ignore_space(&p2, &len);
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 19:
                    if ( state == 1 && prev_cmd == 6 )
                    {
                        state = 2;
                        prev_cmd = 19;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 20:
                    if ( state == 2 && prev_cmd == 19 )
                    {
                        //  dest ch id
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
//                      printk("dest ch id: %d\n", val[0]);
                        //action_tbl.dest_chid = val[0];
                    }
                    state = prev_cmd = 0;
                    break;
                case 22:
                case 23:
                    if ( state == 2 )
                    {
                        if ( prev_cmd == 14 )
                        {
                            //  VLAN insert
//                          printk("VLAN insert: %s (%d)", command[i], i - 22);
                            if ( (i - 22) )
                            {
                                ignore_space(&p2, &len);
                                if ( len > 5 && (p2[5] <= ' ' || p2[5] == ':') && strincmp(p2, "index", 5) == 0 )
                                {
                                    p2 += 6;
                                    len -= 6;
                                    //  VLAN index
                                    ignore_space(&p2, &len);
                                    val[0] = get_number(&p2, &len, 0);
//                                  printk(", index %d", val[0]);
                                    if ( !action_tbl.in_vlan_ins )
                                    {
                                        action_tbl.in_vlan_ins = 1;
                                        //action_tbl.vlan_ix = val[0];
                                        action_tbl.new_in_vci = val[0];
                                    }
                                }
                            }
//                          printk("\n");
                        }
                        else if ( prev_cmd == 15 )
                        {
                            //  VLAN remove
//                          printk("VLAN remove: %s (%d)\n", command[i], i - 22);
                            if ( (i - 22) && !action_tbl.in_vlan_rm )
                                action_tbl.in_vlan_rm = 1;
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 24:
                case 25:
                    if ( state == 2 && prev_cmd == 18 )
                    {
                        //  PPPoE mode
//                      printk("PPPoE mode: %s (%d)\n", command[i], i - 24);
                        action_tbl.pppoe_mode = i - 24;
                    }
                    state = prev_cmd = 0;
                    break;
                case 26:
                case 27:
                case 28:
                case 29:
                    if ( state == 2 && prev_cmd == 9 )
                    {
                        //  route type
//                      printk("route type: %s (%d)\n", command[i], i - 26);
                        action_tbl.rout_type = i - 26;
                    }
                    state = prev_cmd = 0;
                    break;
                case 30:
                    if ( !state )
                    {
                        if ( entry < 0 )
                        {
                            ignore_space(&p2, &len);
                            if ( len && *p2 >= '0' && *p2 <= '9' )
                            {
                                entry = get_number(&p2, &len, 0);
                                //  entry
//                              printk("entry: %d\n", entry);
                            }
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 31:
                    printk("add\n");
                    printk("  LAN/WAN entry ???\n");
                    printk("    compare\n");
                    printk("      src:  ???.???.???.???:????\n");
                    printk("      dest: ???.???.???.???:????\n");
                    printk("    action\n");
                    printk("      new src/dest:???.???.???.???:????\n");
                    printk("      new MAC:     ??:??:??:??:??:?? (HEX)\n");
                    printk("      route type:  NULL/IPv4/NAT/NAPT\n");
                    printk("      new DSCP:    original/??\n");
                    printk("      MTU index:   ??\n");
                    printk("      VLAN insert: disable/enable, index ??\n");
                    printk("      VLAN remove: disable/enable\n");
                    printk("      dest list:   ETH0/ETH1/CPU0/EXT_INT?\n");
                    printk("      PPPoE mode:  transparent/termination\n");
                    printk("      PPPoE index: ??\n");
                    printk("      dest ch id:  ??\n");
                    printk("\n");
                    printk("delete\n");
                    printk("  LAN/WAN entry ???\n");
                    printk("    compare\n");
                    printk("      src:  ???.???.???.???:????\n");
                    printk("      dest: ???.???.???.???:????\n");
                    printk("\n");
                default:
                    state = prev_cmd = 0;
                }

                break;
            }

        if ( i == sizeof(command) / sizeof(*command) )
            state = prev_cmd = 0;

        p1 = p2;
        colon = 1;
    }

    if ( operation == 2 )
    {
        //  delete
        pu1 = (u32*)&compare_tbl;
        pu2 = NULL;
        if ( entry < 0 )
        {
            //  search the entry number
            if ( *pu1 && pu1[1] )
            {
                if ( (!type || type == 1) )
                {
                    //  LAN
//                    for ( entry = 0; entry < LAN_ROUT_NUM; entry++ )
                    for ( entry = 0; entry < LAN_ROUT_TBL_CFG->lan_rout_num; entry++ )
                    {
                        pu2 = (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(entry);
                        if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                            break;
                    }
//                    if ( entry == LAN_ROUT_NUM )
                    if ( entry == LAN_ROUT_TBL_CFG->lan_rout_num )
                        pu2 = NULL;
                }
                if ( (!type && !pu2) || type == 2 )
                {
                    //  WAN
//                    for ( entry = 0; entry < WAN_ROUT_NUM; entry++ )
                    for ( entry = 0; entry < WAN_ROUT_TBL_CFG->wan_rout_num; entry++ )
                    {
                        pu2 = (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(entry);
                        if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                            break;
                    }
//                    if ( entry == WAN_ROUT_NUM )
                    if ( entry == WAN_ROUT_TBL_CFG->wan_rout_num )
                        pu2 = NULL;
                }
            }
        }
        else
        {
            if ( *pu1 && pu1[1] )
            {
                //  check compare
//                if ( (!type || type == 1) && entry < LAN_ROUT_NUM )
                if ( (!type || type == 1) && entry < LAN_ROUT_TBL_CFG->lan_rout_num )
                {
                    //  LAN
                    pu2 = (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(entry);
                    if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                }
//                if ( ((!type && !pu2) || type == 2) && entry < WAN_ROUT_NUM )
                if ( ((!type && !pu2) || type == 2) && entry < WAN_ROUT_TBL_CFG->wan_rout_num )
                {
                    //  WAN
                    pu2 = (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(entry);
                    if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                        pu2 = NULL;
                }
            }
            else if ( !*pu1 && !pu1[1] )
            {
//                if ( type == 1 && entry < LAN_ROUT_NUM )
                if ( type == 1 && entry < LAN_ROUT_TBL_CFG->lan_rout_num )
                    pu2 = (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(entry);
//                else if ( type == 2 && entry < WAN_ROUT_NUM )
                else if ( type == 2 && entry < WAN_ROUT_TBL_CFG->wan_rout_num )
                    pu2 = (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(entry);
            }
        }
        if ( pu2 )
        {
            *pu2 = 0;
            *(pu2 + 1) = 0;
            *(pu2 + 2) = 0;
        }
    }
    else if ( operation == 1 && type && *(u32*)&compare_tbl && *((u32*)&compare_tbl + 1) )
    {
        pu2 = NULL;
        if ( entry < 0 )
        {
            int max_entry;

            //  add
            pu1 = (u32*)&compare_tbl;
            pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
//            max_entry = type == 1 ? LAN_ROUT_NUM : WAN_ROUT_NUM;
            max_entry = type == 1 ? LAN_ROUT_TBL_CFG->lan_rout_num : WAN_ROUT_TBL_CFG->wan_rout_num;
            for ( entry = 0; entry < max_entry; entry++, pu2 += 4 )
                if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                    break;
            if ( entry == max_entry )
            {
                pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
                for ( entry = 0; entry < max_entry; entry++, pu2 += 4 )
                    if ( !*pu2 && !pu2[1] )
                        break;
                if ( entry == max_entry )
                    entry = -1;
            }
        }
        else
        {
            int max_entry;
            int tmp_entry;

            //  replace
            pu1 = (u32*)&compare_tbl;
            pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
//            max_entry = type == 1 ? LAN_ROUT_NUM : WAN_ROUT_NUM;
            max_entry = type == 1 ? LAN_ROUT_TBL_CFG->lan_rout_num : WAN_ROUT_TBL_CFG->wan_rout_num;
            for ( tmp_entry = 0; tmp_entry < max_entry; tmp_entry++, pu2 += 4 )
                if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                    break;
            if ( tmp_entry < max_entry )
                entry = tmp_entry;
//            else if ( !(type == 1 && entry < LAN_ROUT_NUM) && !(type == 2 && entry < WAN_ROUT_NUM) )
            else if ( !(type == 1 && entry < LAN_ROUT_TBL_CFG->lan_rout_num) && !(type == 2 && entry < WAN_ROUT_TBL_CFG->wan_rout_num) )
                entry = -1;
        }

        if ( entry >= 0 )
        {
            //  add or replace
            if ( type == 1 )
            {
                pu1 = (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(entry);
                pu2 = (u32*)LAN_ROUT_FORWARD_ACTION_TBL(entry);
            }
            else
            {
                pu1 = (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(entry);
                pu2 = (u32*)WAN_ROUT_FORWARD_ACTION_TBL(entry);
            }
            my_memcpy((unsigned char *)pu1, (unsigned char *)&compare_tbl, sizeof(compare_tbl));
            my_memcpy((unsigned char *)pu2, (unsigned char *)&action_tbl, sizeof(action_tbl));
        }
    }

    MOD_DEC_USE_COUNT;

    return count;
}

static int proc_read_bridge(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    struct brg_forward_tbl *paction;
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Bridging Table\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_BRIDGE_OVERRUN_END;

    paction = (struct brg_forward_tbl *)BRIDGING_FORWARD_TBL(0);
    for ( i = 0; i < BRG_TBL_CFG->brg_entry_num; i++ )
    {
        if ( !(*((u32*)paction + 1) & 0xFFFF) )
        {
            llen = print_bridge(str, i, paction);
            if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                goto PROC_READ_BRIDGE_OVERRUN_END;
        }

        paction++;
    }

    *eof = 1;

PROC_READ_BRIDGE_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_write_bridge(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

static int proc_read_mc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    volatile struct wan_rout_multicast_tbl *paction;
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Multicast Table\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_MC_OVERRUN_END;

    paction = WAN_ROUT_MULTICAST_TBL(0);
    for ( i = 0; i < WAN_ROUT_MC_NUM; i++ )
    {
        if ( paction->wan_dest_ip )
        {
            llen = print_mc(str, i, (struct wan_rout_multicast_tbl *)paction);
            if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                goto PROC_READ_MC_OVERRUN_END;
        }

        paction++;
    }

    *eof = 1;

PROC_READ_MC_OVERRUN_END:
    return len - off;
}

static int proc_read_genconf(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[2048];
    int llen = 0;
    int i;
    unsigned long bit;
    char *ppst[3] = {0};

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen += sprintf(str + llen, "CFG_WRX_HTUTS    (0x%08X): %d\n", (u32)CFG_WRX_HTUTS, *CFG_WRX_HTUTS);
    llen += sprintf(str + llen, "CFG_WRDES_DELAY  (0x%08X): %d\n", (u32)CFG_WRDES_DELAY, *CFG_WRDES_DELAY);
    llen += sprintf(str + llen, "WRX_EMACH_ON     (0x%08X): AAL - %s, OAM - %s\n", (u32)WRX_EMACH_ON, (*WRX_EMACH_ON & 0x01) ? "on" : "off",
                                                                                                      (*WRX_EMACH_ON & 0x02) ? "on" : "off");
    llen += sprintf(str + llen, "WTX_EMACH_ON     (0x%08X): ", (u32)WTX_EMACH_ON);
    for ( i = 0, bit = 1; i < 8; i++, bit <<= 1 )
    {
        llen += sprintf(str + llen, " %d - %s", i, (*WTX_EMACH_ON & bit) ? "on " : "off");
        if ( i == 3 )
            llen += sprintf(str + llen, "\n                               ");
        else if ( i == 7 )
            llen += sprintf(str + llen, "\n");
    }
    llen += sprintf(str + llen, "WRX_HUNT_BITTH   (0x%08X): %d\n", (u32)WRX_HUNT_BITTH, *WRX_HUNT_BITTH);

    switch ( ETH_PORTS_CFG->eth0_type )
    {
    case 0: ppst[0] = "LAN"; break;
    case 1: ppst[0] = "WAN"; break;
    case 2: ppst[0] = "Mix"; break;
    default: ppst[0] = "Unkown";
    }
    llen += sprintf(str + llen, "ETH_PORTS_CFG    (0x%08X): WAN VLAN ID Hi - %d, WAN VLAN ID Lo - %d, ETH0 Type - %s\n", (u32)ETH_PORTS_CFG, ETH_PORTS_CFG->wan_vlanid_hi, ETH_PORTS_CFG->wan_vlanid_lo, ppst[0]);

    llen += sprintf(str + llen, "LAN_ROUT_TBL_CFG (0x%08X): num - %d, off - %d, no hit drop - %s\n",
                                                            (u32)LAN_ROUT_TBL_CFG,
                                                            LAN_ROUT_TBL_CFG->lan_rout_num,
                                                            LAN_ROUT_TBL_CFG->lan_rout_off,
                                                            LAN_ROUT_TBL_CFG->lan_rout_drop  ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "WAN_ROUT_TBL_CFG (0x%08X): num - %d, MC num - %d, MC drop - %s, no hit drop - %s\n",
                                                            (u32)WAN_ROUT_TBL_CFG,
                                                            WAN_ROUT_TBL_CFG->wan_rout_num,
                                                            WAN_ROUT_TBL_CFG->wan_rout_mc_num,
                                                            WAN_ROUT_TBL_CFG->wan_rout_mc_drop ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->wan_rout_drop  ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "BRG_TBL_CFG      (0x%08X): num - %d\n",
                                                            (u32)BRG_TBL_CFG,
                                                            BRG_TBL_CFG->brg_entry_num
                                                            );

    switch ( GEN_MODE_CFG->wan_acc_mode )
    {
    case 0: ppst[0] = "null"; break;
    case 2: ppst[0] = "enable"; break;
    default: ppst[0] = "unknown";
    }
    switch ( GEN_MODE_CFG->lan_acc_mode )
    {
    case 0: ppst[1] = "null"; break;
    case 2: ppst[1] = "enable"; break;
    default: ppst[1] = "unknown";
    }
    switch ( GEN_MODE_CFG->acc_mode )
    {
    case 0: ppst[2] = "unknown"; break;
    case 1: ppst[2] = "bridging"; break;
    case 2: ppst[2] = "routing"; break;
    case 3: ppst[2] = "bridging + routing"; break;
    }
    llen += sprintf(str + llen, "GEN_MODE_CFG     (0x%08X): WAN fast mode - %s, U/S early discard - %s\n"
                                "                               fast path wfq - %d, DPLUS wfq - %d, ETX wfq - %d,\n"
                                "                               WAN acc mode - %s, LAN acc mode - %s\n"
                                "                               acc mode - %s\n",
                                                            (u32)GEN_MODE_CFG,
                                                            GEN_MODE_CFG->wan_fast_mode ? "direct" : "indirect",
                                                            GEN_MODE_CFG->us_early_discard_en ? "enable" : "disable",
                                                            GEN_MODE_CFG->fast_path_wfq,
                                                            GEN_MODE_CFG->dplus_wfq,
                                                            GEN_MODE_CFG->etx_wfq,
                                                            ppst[0],
                                                            ppst[1],
                                                            ppst[2]
                                                            );

#if 0
    llen += sprintf(str + llen, "BRG_VLAN_TBL     (0x%08X): eth0 - vlan %s, %d - %d, %d - %d\n"
                                "                               eth1 - vlan %s, %d - %d, %d - %d\n",
                                                            (u32)SWCFG_IDEN_VLAN_CFG(0),
                                                            SWCFG_IDEN_VLAN_CFG(0)->enable ? "on " : "off",
                                                            SWCFG_IDEN_VLAN_CFG(0)->vlan_base0,
                                                            SWCFG_IDEN_VLAN_CFG(0)->vlan_base0 + SWCFG_IDEN_VLAN_CFG(0)->vlan_range0,
                                                            SWCFG_IDEN_VLAN_CFG(0)->vlan_base1,
                                                            SWCFG_IDEN_VLAN_CFG(0)->vlan_base1 + SWCFG_IDEN_VLAN_CFG(0)->vlan_range1,
                                                            SWCFG_IDEN_VLAN_CFG(1)->enable ? "on " : "off",
                                                            SWCFG_IDEN_VLAN_CFG(1)->vlan_base0,
                                                            SWCFG_IDEN_VLAN_CFG(1)->vlan_base0 + SWCFG_IDEN_VLAN_CFG(1)->vlan_range0,
                                                            SWCFG_IDEN_VLAN_CFG(1)->vlan_base1,
                                                            SWCFG_IDEN_VLAN_CFG(1)->vlan_base1 + SWCFG_IDEN_VLAN_CFG(1)->vlan_range1
                                                            );
#endif

    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_GENCONF_OVERRUN_END;

    *eof = 1;

PROC_READ_GENCONF_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_write_genconf(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    int f_wan_hi = 0;

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

    if ( strincmp(p, "wan hi ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        f_wan_hi = 1;
    }
    else if ( strincmp(p, "wan high ", 9) == 0 )
    {
        p += 9;
        rlen -= 9;
        f_wan_hi = 1;
    }
    else if ( strincmp(p, "wan lo ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        f_wan_hi = -1;
    }
    else if ( strincmp(p, "wan low ", 8) == 0 )
    {
        p += 8;
        rlen -= 8;
        f_wan_hi = -1;
    }
    else if ( strincmp(p, "eth0 type ", 10) == 0 )
    {
        p += 10;
        rlen -= 10;
        if ( stricmp(p, "lan") == 0 )
            ETH_PORTS_CFG->eth0_type = 0;
        else if ( stricmp(p, "wan") == 0 )
            ETH_PORTS_CFG->eth0_type = 1;
        else if ( stricmp(p, "mix") == 0 )
            ETH_PORTS_CFG->eth0_type = 2;
    }
    else if ( stricmp(p, "direct") == 0 )
    {
        GEN_MODE_CFG->wan_fast_mode = 1;
    }
    else if ( stricmp(p, "indirect") == 0 )
    {
        GEN_MODE_CFG->wan_fast_mode = 0;
    }

    if ( f_wan_hi )
    {
        int num;

        num = get_number(&p, &rlen, 0);
        if ( f_wan_hi > 0 )
            ETH_PORTS_CFG->wan_vlanid_hi = num;
        else
            ETH_PORTS_CFG->wan_vlanid_lo = num;
    }

    MOD_DEC_USE_COUNT;

    return count;
}

static int proc_read_queue(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *mpoa_type_str[] = {"EoA w/o FCS", "EoA w FCS", "PPPoA", "IPoA"};

    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    struct wrx_queue_config rx;
    struct wtx_queue_config tx;
    char qmap_str[64];
    char qmap_flag;
    int qmap_str_len;
    int i, k;
    unsigned int bit;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "RX Queue Config (0x%08X):\n", (u32)WRX_QUEUE_CONFIG(0));
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_QUEUE_OVERRUN_END;

    for ( i = 0; i < 8; i++ )
    {
        rx = *WRX_QUEUE_CONFIG(i);
        llen  = sprintf(str       , "  %d: MPoA type - %s, MPoA mode - %s, IP version %d\n", i, mpoa_type_str[rx.mpoa_type], rx.mpoa_mode ? "LLC" : "VC mux", rx.ip_ver ? 6 : 4);
        llen += sprintf(str + llen,  "     Oversize - %d, Undersize - %d, Max Frame size - %d\n", rx.oversize, rx.undersize, rx.mfs);
        llen += sprintf(str + llen,  "     uu mask - 0x%02X, cpi mask - 0x%02X, uu exp - 0x%02X, cpi exp - 0x%02X\n", rx.uumask, rx.cpimask, rx.uuexp, rx.cpiexp);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_QUEUE_OVERRUN_END;
    }

    llen = sprintf(str, "TX Queue Config (0x%08X):\n", (u32)WTX_QUEUE_CONFIG(0));
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_QUEUE_OVERRUN_END;

    for ( i = 0; i < 8; i++ )
    {
        tx = *WTX_QUEUE_CONFIG(i);
        qmap_flag = 0;
        qmap_str_len = 0;
        for ( k = 0, bit = 1; k < 8; k++, bit <<= 1 )
            if ( (tx.same_vc_qmap & bit) )
            {
                if ( qmap_flag++ )
                    qmap_str_len += sprintf(qmap_str + qmap_str_len, ", ");
                qmap_str_len += sprintf(qmap_str + qmap_str_len, "%d", k);
            }
        llen  = sprintf(str       , "  %d: uu - 0x%02X, cpi - 0x%02X, same VC queue map - %s\n", i, tx.uu, tx.cpi, qmap_flag ? qmap_str : "null");
        llen += sprintf(str + llen,  "     bearer channel - %d, QSB ID - %d, MPoA mode - %s\n", tx.sbid, tx.qsb_vcid, tx.mpoa_mode ? "LLC" : "VC mux");
        llen += sprintf(str + llen,  "     ATM header - 0x%08X\n", tx.atm_header);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_QUEUE_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_QUEUE_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_write_queue(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

static int proc_read_pppoe(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[512];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "PPPoE Table (0x%08X) - Session ID:\n", (u32)PPPOE_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *PPPOE_CFG_TBL(i));
        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_PPPOE_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_PPPOE_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_read_mtu(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "MTU Table (0x%08X):\n", (u32)MTU_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *MTU_CFG_TBL(i));
        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_MTU_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_MTU_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_read_hit(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;
    int n;
    unsigned long bit;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "Unicast Routing Hit Table (0x%08X):\n", (u32)ROUT_FWD_HIT_STAT_TBL(0));
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "             1 2 3 4 5 6 7 8 9 10\n");
    pstr += llen;
    len += llen;

    n = 1;
    for ( i = 0; i < 8; i++ )
        for ( bit = 0x80000000; bit; bit >>= 1 )
        {
            if ( n % 10 == 1 )
                llen = sprintf(str, "  %3d - %3d:", n, n + 9);

            llen += sprintf(str + llen, " %d", (*ROUT_FWD_HIT_STAT_TBL(i) & bit) ? 1 : 0);

            if ( n++ % 10 == 0 )
            {
                llen += sprintf(str + llen, "\n");

                if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_HIT_OVERRUN_END;
    }

    llen = sprintf(str, "Multicast Routing Hit Table (0x%08X):\n", (u32)MC_ROUT_FWD_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_HIT_OVERRUN_END;

    n = 1;
    for ( i = 0; i < 1; i++ )
        for ( bit = 0x80000000; bit; bit >>= 1 )
        {
            if ( n % 10 == 1 )
                llen = sprintf(str, "  %3d - %3d:", n, n + 9);

            llen += sprintf(str + llen, " %d", (*MC_ROUT_FWD_HIT_STAT_TBL(i) & bit) ? 1 : 0);

            if ( n++ % 10 == 0 )
            {
                llen += sprintf(str + llen, "\n");

                if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_HIT_OVERRUN_END;
    }

    llen = sprintf(str, "Bridging Hit Table (0x%08X):\n", (u32)BRG_FWD_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_HIT_OVERRUN_END;

    n = 1;
    for ( i = 0; i < 8; i++ )
        for ( bit = 0x80000000; bit; bit >>= 1 )
        {
            if ( n % 10 == 1 )
                llen = sprintf(str, "  %3d - %3d:", n, n + 9);

            llen += sprintf(str + llen, " %d", (*BRG_FWD_HIT_STAT_TBL(i) & bit) ? 1 : 0);

            if ( n++ % 10 == 0 )
            {
                llen += sprintf(str + llen, "\n");

                if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_HIT_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_HIT_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_write_hit(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;
    int i;

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

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clean") == 0 )
        for ( i = 0; i < 12; i++ )
            *ROUT_FWD_HIT_STAT_TBL(i) = 0;

    MOD_DEC_USE_COUNT;

    return count;
}

static int proc_read_mac(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;
    unsigned int mac52, mac10;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "MAC Table:\n");
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  CFG_ROUT_MAC_NO  (0x%08X): %d\n", (u32)CFG_ROUT_MAC_NO, (u32)*CFG_ROUT_MAC_NO);
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  ROUT_MAC_CFG_TBL (0x%08X)\n", (u32)ROUT_MAC_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 16; i++ )
    {
        mac52 = *ROUT_MAC_CFG_TBL(i);
        mac10 = *(ROUT_MAC_CFG_TBL(i) + 1);

        llen = sprintf(str, "    %2d: %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1, mac52 >> 24, (mac52 >> 16) & 0xFF, (mac52 >> 8) & 0xFF, mac52 & 0xFF, mac10 >> 24, (mac10 >> 16) & 0xFF);
        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_MAC_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_MAC_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_read_vlan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *etag_en_str[] = {
        "unmodified",
        "remove",
        "insert",
        "replace"
    };

    struct eth_default_dest_list itf_cfg;
    volatile struct eth_default_dest_list *p_itf_cfg;
    struct default_itf_vlan_map def_map;
    volatile struct default_itf_vlan_map *p_def_map;
    struct src_ip_vlan_map ip_map;
    volatile struct src_ip_vlan_map *p_ip_map;
    struct ethtype_vlan_map ethtype_map;
    volatile struct ethtype_vlan_map *p_ethtype_map;
    struct vlan_vlan_map vlan_map;
    volatile struct vlan_vlan_map *p_vlan_map;
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Interface VLAN Config:\n");
    if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
        goto PROC_READ_VLAN_OVERRUN_END;

    p_itf_cfg = ETH_DEFAULT_DEST_LIST(0);
    for ( i = 0; i < IFX_PPA_PORT_NUM; i++, p_itf_cfg++ )
    {
        itf_cfg = *p_itf_cfg;

        llen = sprintf(str,         "  %d (0x%08X):\n", i, (u32)p_itf_cfg);
        llen += sprintf(str + llen, "    egress inner vlan - %s, src ip - %s, ethtype - %s, vlan tag - %s\n",
                                         etag_en_str[itf_cfg.in_etag_en],
                                         itf_cfg.srcip_vlan_en ? " on" : "off",
                                         itf_cfg.ethtype_vlan_en ? " on" : "off",
                                         itf_cfg.vlan_vlan_en ? " on" : "off");
        llen += sprintf(str + llen, "    loopback - %s, vlan aware - %s, mac change drop - %s\n",
                                         itf_cfg.loop_ctl ? "enable" : "disable",
                                         itf_cfg.vlan_en ? " on" : "off",
                                         itf_cfg.mac_change_drop ? " drop" : "forward");
        llen += sprintf(str + llen, "    port en - %s, ingress outer vlan - %s, egress outer vlan - %s\n",
                                         itf_cfg.port_en ? "enable" : "disable",
                                         itf_cfg.out_itag_vld ? " enable" : "disable",
                                         etag_en_str[itf_cfg.out_etag_en]);
        llen += sprintf(str + llen, "    unknown multicast - 0x%02X, unknown uni-cast - 0x%02X\n",
                                         itf_cfg.unknown_mc_dest_list,
                                         itf_cfg.unknown_uc_dest_list);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_def_map = DEFAULT_ITF_VLAN_MAP(0);
    for ( i = 0; i < MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES; i++, p_def_map++ )
    {
        def_map = *p_def_map;

        llen = sprintf(str, "  %d (0x%08X): inner vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_def_map, def_map.new_in_vci, def_map.vlan_member, def_map.dest_qos, def_map.outer_vlan_ix);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_ip_map = SRC_IP_VLAN_MAP(0);
    for ( i = 0; i < MAX_SRC_IP_VLAN_MAP_ENTRIES; i++, p_ip_map++ )
    {
        ip_map = *p_ip_map;

        llen = sprintf(str, "  %d (0x%08X): src ip - %d.%d.%d.%d, vci - 0x%04X, map - 0x%02X, outer vlan index - %d, dest qos - %d\n", i, (u32)p_ip_map, ip_map.src_ip >> 24, (ip_map.src_ip >> 16) & 0xFF, (ip_map.src_ip >> 8) & 0xFF, ip_map.src_ip & 0xFF, ip_map.new_in_vci, ip_map.vlan_member, ip_map.dest_qos, ip_map.outer_vlan_ix);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_ethtype_map = ETHTYPE_VLAN_MAP(0);
    for ( i = 0; i < MAX_ETHTYPE_VLAN_MAP_ENTRIES; i++, p_ethtype_map++ )
    {
        ethtype_map = *p_ethtype_map;

        llen = sprintf(str, "  %d (0x%08X): ethtype - 0x%04X, vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_ethtype_map, ethtype_map.ethtype, ethtype_map.new_in_vci, ethtype_map.vlan_member, ethtype_map.dest_qos, ethtype_map.outer_vlan_ix);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_vlan_map = VLAN_VLAN_MAP(0);
    for ( i = 0; i < MAX_VLAN_VLAN_MAP_ENTRIES; i++, p_vlan_map++ )
    {
        vlan_map = *p_vlan_map;

        llen = sprintf(str, "  %d (0x%08X): vlan tag - 0x%04X, vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_vlan_map, vlan_map.in_vlan_tag, vlan_map.new_in_vci, vlan_map.vlan_member, vlan_map.dest_qos, vlan_map.outer_vlan_ix);
        llen += sprintf(str + llen, "                  in/out etag ctrl - 0x%08x\n", vlan_map.in_out_etag_ctrl);

        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_VLAN_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

static int proc_read_out_vlan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "Outer VLAN Table (0x%08X):\n", (u32)OUTER_VLAN_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 32; i++ )
    {
        llen = sprintf(str, "  %d: 0x%08X\n", i, *OUTER_VLAN_TBL(i));
        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_OUT_VLAN_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_OUT_VLAN_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}

#endif

#if defined(DEBUG_HTU_PROC) && DEBUG_HTU_PROC
static INLINE int print_htu(char *buf, int i)
{
    int len = 0;

    if ( HTU_ENTRY(i)->vld )
    {
        len += sprintf(buf + len, "%2d. valid\n", i);
        len += sprintf(buf + len,  "    entry  0x%08x - pid %01x vpi %02x vci %04x pti %01x\n", *(u32*)HTU_ENTRY(i), HTU_ENTRY(i)->pid, HTU_ENTRY(i)->vpi, HTU_ENTRY(i)->vci, HTU_ENTRY(i)->pti);
        len += sprintf(buf + len,  "    mask   0x%08x - pid %01x vpi %02x vci %04x pti %01x\n", *(u32*)HTU_MASK(i), HTU_MASK(i)->pid_mask, HTU_MASK(i)->vpi_mask, HTU_MASK(i)->vci_mask, HTU_MASK(i)->pti_mask);
        len += sprintf(buf + len,  "    result 0x%08x - type: %s, qid: %d", *(u32*)HTU_RESULT(i), HTU_RESULT(i)->type ? "cell" : "AAL5", HTU_RESULT(i)->qid);
        if ( HTU_RESULT(i)->type )
            len += sprintf(buf + len,  ", cell id: %d, verification: %s", HTU_RESULT(i)->cellid, HTU_RESULT(i)->ven ? "on" : "off");
        len += sprintf(buf + len,  "\n");
    }
    else
        len += sprintf(buf + len, "%2d. invalid\n", i);

    return len;
}

static int proc_read_htu(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *pstr;
    char str[512];
    int llen;

    int htuts = *CFG_WRX_HTUTS;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    __sync();

    for ( i = 0; i < htuts; i++ )
    {
        llen = print_htu(str, i);
        if ( proc_buf_copy(&pstr, count, off, &len, str, llen) )
            goto PROC_READ_HTU_OVERRUN_END;
    }

    *eof = 1;

PROC_READ_HTU_OVERRUN_END:
    MOD_DEC_USE_COUNT;
    return len - off;
}
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    MOD_INC_USE_COUNT;

    len += sprintf(page + len, "error print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ERR)              ? "enabled" : "disabled");
    len += sprintf(page + len, "debug print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT)      ? "enabled" : "disabled");
    len += sprintf(page + len, "assert           - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ASSERT)           ? "enabled" : "disabled");
    len += sprintf(page + len, "dump rx skb      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_RX)      ? "enabled" : "disabled");
    len += sprintf(page + len, "dump tx skb      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_TX)      ? "enabled" : "disabled");
    len += sprintf(page + len, "dump flag header - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_FLAG_HEADER) ? "enabled" : "disabled");
    len += sprintf(page + len, "dump init        - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_INIT)        ? "enabled" : "disabled");
    len += sprintf(page + len, "dump qos         - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS)         ? "enabled" : "disabled");

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_dbg(struct file *file, const char *buf, unsigned long count, void *data)
{
    static const char *dbg_enable_mask_str[] = {
        " error print",
        " err",
        " debug print",
        " dbg",
        " assert",
        " assert",
        " dump rx skb",
        " rx",
        " dump tx skb",
        " tx",
        " dump flag header",
        " header",
        " dump init",
        " init",
        " dump qos",
        " qos",
        " all"
    };
    static const int dbg_enable_mask_str_len[] = {
        12, 4,
        12, 4,
        7,  7,
        12, 3,
        12, 3,
        17, 7,
        10, 5,
        9,  4,
        4
    };
    u32 dbg_enable_mask[] = {
        DBG_ENABLE_MASK_ERR,
        DBG_ENABLE_MASK_DEBUG_PRINT,
        DBG_ENABLE_MASK_ASSERT,
        DBG_ENABLE_MASK_DUMP_SKB_RX,
        DBG_ENABLE_MASK_DUMP_SKB_TX,
        DBG_ENABLE_MASK_DUMP_FLAG_HEADER,
        DBG_ENABLE_MASK_DUMP_INIT,
        DBG_ENABLE_MASK_DUMP_QOS,
        DBG_ENABLE_MASK_ALL
    };

    char str[2048];
    char *p;

    int len, rlen;

    int f_enable = 0;
    int i;

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

    if ( strincmp(p, "enable", 6) == 0 )
    {
        p += 6;
        f_enable = 1;
    }
    else if ( strincmp(p, "disable", 7) == 0 )
    {
        p += 7;
        f_enable = -1;
    }
    else if ( strincmp(p, "help", 4) == 0 || *p == '?' )
    {
        printk("echo <enable/disable> [err/dbg/assert/rx/tx/header/init/qos/all] > /proc/eth/dbg\n");
    }

    if ( f_enable )
    {
        if ( *p == 0 )
        {
            if ( f_enable > 0 )
                g_dbg_enable |= DBG_ENABLE_MASK_ALL;
            else
                g_dbg_enable &= ~DBG_ENABLE_MASK_ALL;
        }
        else
        {
            do
            {
                for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ )
                    if ( strincmp(p, dbg_enable_mask_str[i], dbg_enable_mask_str_len[i]) == 0 )
                    {
                        if ( f_enable > 0 )
                            g_dbg_enable |= dbg_enable_mask[i >> 1];
                        else
                            g_dbg_enable &= ~dbg_enable_mask[i >> 1];
                        p += dbg_enable_mask_str_len[i];
                        break;
                    }
            } while ( i < NUM_ENTITY(dbg_enable_mask_str) );
        }
    }

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

#define PP32_REG_ADDR_BEGIN     0x0
#define PP32_REG_ADDR_END       0x0FFF
#define PP32_SB_ADDR_BEGIN      0x2200
#define PP32_SB_ADDR_END        0x35FF

static inline unsigned long sb_addr_to_fpi_addr_convert(unsigned long sb_addr)
{
    if ( sb_addr <= PP32_SB_ADDR_END )
        sb_addr = (unsigned long)SB_BUFFER(sb_addr);

    return sb_addr;
}

#if defined(DEBUG_MEM_PROC) && DEBUG_MEM_PROC
static int proc_write_mem(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *p1, *p2;
    int len;
    int colon;
    unsigned long *p;
    char local_buf[1024];
    int i, n, l;

    MOD_INC_USE_COUNT;

    len = sizeof(local_buf) < count ? sizeof(local_buf) : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    colon = 1;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "w") == 0 || stricmp(p1, "write") == 0 || stricmp(p1, "r") == 0 || stricmp(p1, "read") == 0 )
            break;

        p1 = p2;
        colon = 1;
    }

    if ( *p1 == 'w' )
    {
        ignore_space(&p2, &len);
        p = (unsigned long *)get_number(&p2, &len, 1);
        p = (unsigned long *)sb_addr_to_fpi_addr_convert((unsigned long)p);
        if ( (u32)p >= KSEG0 )
            while ( 1 )
            {
                ignore_space(&p2, &len);
                if ( !len || !((*p2 >= '0' && *p2 <= '9') || (*p2 >= 'a' && *p2 <= 'f') || (*p2 >= 'A' && *p2 <= 'F')) )
                    break;

                *p++ = (u32)get_number(&p2, &len, 1);
            }
    }
    else if ( *p1 == 'r' )
    {
        ignore_space(&p2, &len);
        p = (unsigned long *)get_number(&p2, &len, 1);
        p = (unsigned long *)sb_addr_to_fpi_addr_convert((unsigned long)p);
        if ( (u32)p >= KSEG0 )
        {
            ignore_space(&p2, &len);
            n = (int)get_number(&p2, &len, 0);
            if ( n )
            {
                n += (l = ((int)p >> 2) & 0x03);
                p = (unsigned long *)((u32)p & ~0x0F);
                for ( i = 0; i < n; i++ )
                {
                    if ( (i & 0x03) == 0 )
                        printk("%08X:", (u32)p);
                    if ( i < l )
                        printk("         ");
                    else
                        printk(" %08X", (u32)*p);
                    p++;
                    if ( (i & 0x03) == 0x03 )
                        printk("\n");
                }
                if ( (n & 0x03) != 0x00 )
                    printk("\n");
            }
        }
    }

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

#if defined(DEBUG_PP32_PROC) && DEBUG_PP32_PROC
static int proc_read_pp32(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *halt_stat[] = {
        "reset",
        "break in line",
        "stop",
        "step",
        "code",
        "data0",
        "data1"
    };
    static const char *brk_src_data[] = {
        "off",
        "read",
        "write",
        "read/write",
        "write_equal",
        "N/A",
        "N/A",
        "N/A"
    };
    static const char *brk_src_code[] = {
        "off",
        "on"
    };

    int len = 0;
    int i;
    int k;
    unsigned long bit;

    MOD_INC_USE_COUNT;

    len += sprintf(page + len, "Task No %d, PC %04x\n", *PP32_DBG_TASK_NO & 0x03, *PP32_DBG_CUR_PC & 0xFFFF);

    if ( !(*PP32_HALT_STAT & 0x01) )
        len += sprintf(page + len, "  Halt State: Running\n");
    else
    {
        len += sprintf(page + len, "  Halt State: Stopped");
        k = 0;
        for ( bit = 2, i = 0; bit <= (1 << 7); bit <<= 1, i++ )
            if ( (*PP32_HALT_STAT & bit) )
            {
                if ( !k )
                {
                    len += sprintf(page + len, ", ");
                    k++;
                }
                else
                    len += sprintf(page + len, " | ");
                len += sprintf(page + len, halt_stat[i]);
            }

        len += sprintf(page + len, "\n");
    }

    len += sprintf(page + len, "  Break Src:  data1 - %s, data0 - %s, pc3 - %s, pc2 - %s, pc1 - %s, pc0 - %s\n",
                                                    brk_src_data[(*PP32_BRK_SRC >> 11) & 0x07], brk_src_data[(*PP32_BRK_SRC >> 8) & 0x07], brk_src_code[(*PP32_BRK_SRC >> 3) & 0x01], brk_src_code[(*PP32_BRK_SRC >> 2) & 0x01], brk_src_code[(*PP32_BRK_SRC >> 1) & 0x01], brk_src_code[*PP32_BRK_SRC & 0x01]);

    for ( i = 0; i < 4; i++ )
        len += sprintf(page + len, "    pc%d:      %04x - %04x\n", i, *PP32_DBG_PC_MIN(i), *PP32_DBG_PC_MAX(i));

    for ( i = 0; i < 2; i++ )
        len += sprintf(page + len, "    data%d:    %04x - %04x (%08x)\n", i, *PP32_DBG_DATA_MIN(i), *PP32_DBG_DATA_MAX(i), *PP32_DBG_DATA_VAL(i));

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
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

    if ( stricmp(str, "start") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_RESTART;
    else if ( stricmp(str, "stop") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_STOP;
//    else if ( stricmp(str, "step") == 0 )
//        *PP32_DBG_CTRL = DBG_CTRL_STEP_SET(1);
    else
    {
        printk("echo \"<command>\" > /proc/eth/etop\n");
        printk("  command:\n");
        printk("    start - run pp32\n");
        printk("    stop  - stop pp32\n");
//        printk("    step  - run pp32 with one step only\n");
        printk("    help  - print this screen\n");
    }

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

static int proc_read_burstlen(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    u32 dma_ps;
    u32 dma_pctrl;
    unsigned long sys_flag;

    MOD_INC_USE_COUNT;

    local_irq_save(sys_flag);
    dma_ps = *(volatile u32 *)0xBE104140;
    *(volatile u32 *)0xBE104140 = 0;
    dma_pctrl = *(volatile u32 *)0xBE104144;
    *(volatile u32 *)0xBE104140 = dma_ps;
    local_irq_restore(sys_flag);

    len += sprintf(page + len, "DMA-PPE burst length: Rx %d, Tx %d\n", 1 << ((dma_pctrl >> 2) & 0x03), 1 << ((dma_pctrl >> 4) & 0x03));

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_burstlen(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;

    int len, rlen;

    int burstlen;
    u32 dma_ps;
    unsigned long sys_flag;

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

    if ( rlen == 1 )
    {
        burstlen = 0;
        switch ( *p )
        {
        case '2':
            burstlen = 0x14;   break;
        case '4':
            burstlen = 0x28;   break;
        case '8':
        default:
            printk("DMA-PPE doesn't not support %d word burst length!\n", (int)*p);
        }
        if ( burstlen )
        {
            local_irq_save(sys_flag);
            dma_ps = *(volatile u32 *)0xBE104140;
            *(volatile u32 *)0xBE104140 = 0;
            *(volatile u32 *)0xBE104144 = (*(volatile u32 *)0xBE104144 & ~0x3C) | burstlen;
            *(volatile u32 *)0xBE104140 = dma_ps;
            local_irq_restore(sys_flag);
        }
    }

    MOD_DEC_USE_COUNT;

    return count;
}

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
static int proc_read_fw(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    u32 sll_state;
    u32 tir4;
    u32 tir4_sll_state;
    u32 tir4_dplus_rx_on;

    MOD_INC_USE_COUNT;

    len += sprintf(page + len, "Firmware\n");
    len += sprintf(page + len, "  ACC_ERX_PID        = %08X\n", *ACC_ERX_PID);
    len += sprintf(page + len, "  ACC_ERX_PORT_TIMES = %08X\n", *ACC_ERX_PORT_TIMES);

    sll_state = *SLL_ISSUED;
    tir4 = *TIR(4);
    tir4_sll_state = (tir4 >> 21) & 7;
    tir4_dplus_rx_on = (tir4 >> 20) & 1;

    len += sprintf(page + len, "  SLL_ISSUED         = %d [%s], tir4.sll_state = %d [%s]\n",
                        sll_state,
                        sll_state == SLL_STATE_NULL     ? "NULL"   :
                        sll_state == SLL_STATE_DA       ? "BRG_DA" :
                        sll_state == SLL_STATE_SA       ? "BRG_SA" :
                        sll_state == SLL_STATE_ROUTER   ? "ROUT" :
                                                          "Error!",
                        tir4_sll_state,
                        tir4_sll_state == SLL_STATE_NULL    ? "NULL"   :
                        tir4_sll_state == SLL_STATE_DA      ? "BRG_DA" :
                        tir4_sll_state == SLL_STATE_SA      ? "BRG_SA" :
                        tir4_sll_state == SLL_STATE_ROUTER  ? "ROUT" :
                                                              "Error!"  );

    len += sprintf(page + len, "  DPLUS_RX_ON        = %d, tir4.dplus_rx_on = %d\n",
            *DPLUS_RX_ON, tir4_dplus_rx_on);


    len += sprintf(page + len, "  BMC_ISSUED         = %08X\n", *BMC_ISSUED);

    len += sprintf(page + len, "  PRESEARCH_RDPTR    = %08X, %08X\n",
            *PRESEARCH_RDPTR, *(PRESEARCH_RDPTR + 1));

    len += sprintf(page + len, "  SLL_ERX_PID        = %08X\n", *SLL_ERX_PID);

    len += sprintf(page + len, "  SLL_PKT_CNT        = %08X, %08X\n",
            *SLL_PKT_CNT, *(SLL_PKT_CNT + 1));

    len += sprintf(page + len, "  SLL_RDPTR          = %08X, %08X\n",
            *SLL_RDPTR, *(SLL_RDPTR + 1));


    len += sprintf(page + len, "  EDIT_PKT_CNT       = %08X, %08X\n",
            *EDIT_PKT_CNT, *(EDIT_PKT_CNT + 1));
    len += sprintf(page + len, "  EDIT_RDPTR         = %08X, %08X\n",
            *EDIT_RDPTR, *(EDIT_RDPTR + 1));

    len += sprintf(page + len, "  DPLUSRX_PKT_CNT    = %08X, %08X\n",
            *DPLUSRX_PKT_CNT, *(DPLUSRX_PKT_CNT + 1));
    len += sprintf(page + len, "  DPLUS_RDPTR        = %08X, %08X\n",
            *DPLUS_RDPTR, *(DPLUS_RDPTR + 1));

    //len += sprintf(page + len, "  POSTSEARCH_RDPTR   = %08X, %08X\n",

    len += sprintf(page + len, "\n\nQoS:\n");
    len += sprintf(page + len, "  DSLWAN_TX_SWAP_RDPTR = %04X, %04X, %04X, %04X, %04X, %04X, %04X, %04X\n",
                                            * __DSLWAN_TXDES_SWAP_PTR0, * __DSLWAN_TXDES_SWAP_PTR1,
                                            * __DSLWAN_TXDES_SWAP_PTR2, * __DSLWAN_TXDES_SWAP_PTR3,
                                            * __DSLWAN_TXDES_SWAP_PTR4, * __DSLWAN_TXDES_SWAP_PTR5,
                                            * __DSLWAN_TXDES_SWAP_PTR6, * __DSLWAN_TXDES_SWAP_PTR7 );

    len += sprintf(page + len, "  DSLWAN_TX_PKT_CNT    = %04X, %04X, %04X, %04X, %04X, %04X, %04X, %04X\n",
                                            * __DSLWAN_TX_PKT_CNT0, * __DSLWAN_TX_PKT_CNT1,
                                            * __DSLWAN_TX_PKT_CNT2, * __DSLWAN_TX_PKT_CNT3,
                                            * __DSLWAN_TX_PKT_CNT4, * __DSLWAN_TX_PKT_CNT5,
                                            * __DSLWAN_TX_PKT_CNT6, * __DSLWAN_TX_PKT_CNT7);

    len += sprintf(page + len, "  QOSD_DPLUS_RDPTR     = %04X\n",
                                            * __DSLWAN_FP_RXDES_SWAP_RDPTR);

    len += sprintf(page + len, "  QOSD_CPUTX_RDPTR     = %04X\n",
                                            * __CPU_TXDES_SWAP_RDPTR);

    len += sprintf(page + len, "  DPLUS_RXDES_RDPTR    = %04X\n",
                                            * __DSLWAN_FP_RXDES_DPLUS_WRPTR);

    len += sprintf(page + len, "\nQoS Mib:\n");
    len += sprintf(page + len, "  cputx_pkts:             %u\n",   *__CPU_TO_DSLWAN_TX_PKTS);
    len += sprintf(page + len, "  cputx_bytes:            %u\n",   *__CPU_TO_DSLWAN_TX_BYTES);
    len += sprintf(page + len, "  cputx_drop_pkts:        %u\n",   *__CPU_TX_SWAPPER_DROP_PKTS);
    len += sprintf(page + len, "  cputx_drop_bytess:      %u\n",   *__CPU_TX_SWAPPER_DROP_BYTES);

    len += sprintf(page + len, "  dslwan_fp_drop_pkts:    %u\n",   *__DSLWAN_FP_SWAPPER_DROP_PKTS );
    len += sprintf(page + len, "  dslwan_fp_drop_bytes:   %u\n",   *__DSLWAN_FP_SWAPPER_DROP_BYTES );

    len += sprintf(page + len, "  dslwan_tx_qf_drop_pkts: %u (%u, %u, %u, %u, %u, %u, %u, %u)\n",
                 *__DSLWAN_TX_THRES_DROP_PKT_CNT0 +  *__DSLWAN_TX_THRES_DROP_PKT_CNT1 +  *__DSLWAN_TX_THRES_DROP_PKT_CNT2 +  *__DSLWAN_TX_THRES_DROP_PKT_CNT3 +
                 *__DSLWAN_TX_THRES_DROP_PKT_CNT4 +  *__DSLWAN_TX_THRES_DROP_PKT_CNT5 +  *__DSLWAN_TX_THRES_DROP_PKT_CNT6 +  *__DSLWAN_TX_THRES_DROP_PKT_CNT7,
                 * __DSLWAN_TX_THRES_DROP_PKT_CNT0,  *__DSLWAN_TX_THRES_DROP_PKT_CNT1,  *__DSLWAN_TX_THRES_DROP_PKT_CNT2,  *__DSLWAN_TX_THRES_DROP_PKT_CNT3,
                 * __DSLWAN_TX_THRES_DROP_PKT_CNT4,  *__DSLWAN_TX_THRES_DROP_PKT_CNT5,  *__DSLWAN_TX_THRES_DROP_PKT_CNT6,  *__DSLWAN_TX_THRES_DROP_PKT_CNT7);

    len += sprintf(page + len, "\nDSL WAN MIB\n");
    len += sprintf(page + len, "  RX cell: drophtu %u, dropdes %u, err %u, correct %u\n", *__WRX_DROPHTU_CELL, *__WRX_DROPDES_CELL, *__WRX_ERR_CELL, *__WRX_CORRECT_CELL);
    len += sprintf(page + len, "  RX PDU:  dropdes %u, err %u, correct %u\n", *__WRX_DROPDES_PDU, *__WRX_ERR_PDU, *__WRX_CORRECT_PDU);
    len += sprintf(page + len, "  TX cell: total %u\n", *__WTX_TOTAL_CELL);
    len += sprintf(page + len, "  TX PDU:  total %u\n", *__WTX_TOTAL_PDU);

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_fw(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
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

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clean") == 0 )
    {
        * __DSLWAN_TX_THRES_DROP_PKT_CNT0 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT1 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT2 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT3 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT4 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT5 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT6 = 0;
        * __DSLWAN_TX_THRES_DROP_PKT_CNT7 = 0;

        * __CPU_TO_DSLWAN_TX_PKTS  = 0;
        * __CPU_TO_DSLWAN_TX_BYTES = 0;


        * __CPU_TX_SWAPPER_DROP_PKTS    = 0;
        * __CPU_TX_SWAPPER_DROP_BYTES   = 0;
        * __DSLWAN_FP_SWAPPER_DROP_PKTS = 0;
        * __DSLWAN_FP_SWAPPER_DROP_PKTS = 0;
    }

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

#if defined(DEBUG_SEND_PROC) && DEBUG_SEND_PROC
static int ppe_send_cell(int conn, void *cell)
{
    struct sk_buff *skb;
    unsigned long sys_flag;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;

    /*  allocate sk_buff    */
    skb = alloc_skb_tx(CELL_SIZE);
    if ( skb == NULL )
        return -ENOMEM;
    ATM_SKB(skb)->vcc = NULL;

    /*  copy data   */
    skb_put(skb, CELL_SIZE);
    my_memcpy(skb->data, cell, CELL_SIZE);

#ifndef CONFIG_MIPS_UNCACHED
    /*  write back to physical memory   */
    dma_cache_wback((u32)skb->data, skb->len);
#endif

    /*  allocate descriptor */
    local_irq_save(sys_flag);
    desc = CPU_TO_WAN_TX_DESC_BASE + g_cpu_to_wan_tx_desc_pos;
    if ( !desc->own )    //  PPE hold
    {
        local_irq_restore(sys_flag);
        err("OAM not alloc tx connection");
        return -EIO;
    }
    if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
        g_cpu_to_wan_tx_desc_pos = 0;
    local_irq_restore(sys_flag);

    /*  load descriptor from memory */
    reg_desc = *desc;

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & 15) == 0, "reg_desc.dataptr (0x%#x) must be 4 DWORDS aligned", reg_desc.dataptr);
    ASSERT(*(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)) > 0x80000000, "pointer to skb is wrong, desc->dataptr = %08X, skb = %08X", reg_desc.dataptr, *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)));
    skb_to_free = get_skb_pointer(reg_desc.dataptr, 0);
    atm_free_tx_skb_vcc_delayed(skb_to_free);
    put_skb_to_dbg_pool(skb);

    /*  setup descriptor    */
    reg_desc.dataptr    = (u32)skb->data & 0x1FFFFFF0;  //  byte address
    reg_desc.byteoff    = (u32)skb->data & 15;
    reg_desc.datalen    = skb->len;
    reg_desc.mpoa_pt    = 1;
    reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
    reg_desc.pdu_type   = 1;    //  cell
    reg_desc.qid        = conn;
    reg_desc.own        = 0;
    reg_desc.c          = 0;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send_oam", 0, 1);

    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    adsl_led_flash();

    return 0;
}

static int proc_read_send(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    MOD_INC_USE_COUNT;

    len += sprintf(page + len, "echo \"[<qid> qid] [<pvc> vpi.vci] <dword0> <dword1> ...... <dword12>\" > /proc/eth/send\n");

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len;
}

static int proc_write_send(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;

    int len, rlen;
    int conn = -1;
    int dword_counter;
    unsigned int cell[16];
    int ret;
    int vpi = 0, vci = 0;
    int f_enable_irq = 0;
    int f_continue_send = 0;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        return 0;
    }

    if ( strincmp(p, "qid ", 4) == 0 )
    {
        p += 4;
        rlen -= 4;
        ignore_space(&p, &rlen);

        conn = get_number(&p, &rlen, 0);
    }
    else if ( strincmp(p, "pvc ", 4) == 0 )
    {
        p += 4;
        rlen -= 4;
        ignore_space(&p, &rlen);

        vpi = get_number(&p, &rlen, 0);
        if ( rlen && *p == '.' )
        {
            p++;
            rlen--;
        }
        else
            ignore_space(&p, &rlen);
        vci = get_number(&p, &rlen, 0);
        conn = find_vpivci(vpi, vci);
        if ( conn < 0 )
            printk("no such pvc %d.%d\n", vpi, vci);
    }

    if ( !f_continue_send )
    {
        if ( conn >= 0 && conn <= 14 )
        {
            for ( dword_counter = 0; dword_counter < sizeof(cell) / sizeof(cell[0]); dword_counter++ )
            {
                ignore_space(&p, &rlen);
                if ( rlen <= 0 )
                    break;
                cell[dword_counter] = get_number(&p, &rlen, 1);
            }
            if ( dword_counter < 13 )
                printk("not enough data to compose cell\n");
            else
            {
                if ( !g_atm_priv_data.connection[conn].vcc )
                {
                    struct wrx_queue_config wrx_queue_config = {0};
                    struct wtx_queue_config wtx_queue_config = {0};
                    struct uni_cell_header *cell_header;

                    printk("need open queue by self\n");
                    /*  setup RX queue cfg and TX queue cfg */
                    wrx_queue_config.mpoa_type  = 3;    //  set IPoA as default
                    wrx_queue_config.ip_ver     = 0;    //  set IPv4 as default
                    wrx_queue_config.mpoa_mode  = 0;    //  set VC-mux as default
                    wrx_queue_config.oversize   = aal5r_max_packet_size;
                    wrx_queue_config.undersize  = aal5r_min_packet_size;
                    wrx_queue_config.mfs        = aal5r_max_packet_size;
                    wtx_queue_config.same_vc_qmap   = 0x00;     //  up to now, one VC with multiple TX queue has not been implemented
                    wtx_queue_config.sbid           = g_atm_priv_data.connection[conn].port;
                    wtx_queue_config.qsb_vcid       = conn + QSB_QUEUE_NUMBER_BASE; //  qsb qid = firmware qid + 1
                    wtx_queue_config.mpoa_mode      = 0;        //  set VC-mux as default
                    wtx_queue_config.qsben          = 1;        //  reserved in A4, however put 1 for backward compatible
                    cell_header = (struct uni_cell_header *)((u32 *)&wtx_queue_config + 1);
                    cell_header->clp    = 0;
                    cell_header->pti    = ATM_PTI_US0;
                    cell_header->vci    = 1;
                    cell_header->vpi    = 4;
                    cell_header->gfc    = 0;
                    *WRX_QUEUE_CONFIG(conn) = wrx_queue_config;
                    *WTX_QUEUE_CONFIG(conn) = wtx_queue_config;
                    if ( g_atm_priv_data.connection_table == 0 )
                    {
                        f_enable_irq = 1;
                        turn_on_dma_rx(31);
                    }
                }

                if ( dword_counter > 13 )
                    printk("too much data, last %d dwords ignored\n", dword_counter - 13);
                if ( (ret = ppe_send_cell(conn, cell)) )
                    printk("ppe_send_cell(%d, %08x) return %d\n", conn, (unsigned int)cell, ret);

                if ( f_enable_irq )
                {
                    volatile int j;
                    for ( j = 0; j < 100000; j++ );
                    turn_off_dma_rx(31);
                }
            }
        }
        else
            printk("the queue (%d) is wrong\n", conn);
    }

    return count;
}
#endif

static int proc_read_prio(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i, j;

    len += sprintf(page + len,        "Priority to Queue Map:\n");
    len += sprintf(page + len,        "  prio\t\t:  0  1  2  3  4  5  6  7\n");
    for ( i = 0; i < 1; i++ )
    {
        len += sprintf(page + len,    "  eth%d\t\t:", i);
        for ( j = 0; j < NUM_ENTITY(g_eth_prio_queue_map[i]); j++ )
            len += sprintf(page + len,"  %d", g_eth_prio_queue_map[i][j]);
        len += sprintf(page + len,    "\n");
    }
    for ( i = 0; i < ATM_QUEUE_NUMBER; i++ )
    {
        if ( g_atm_priv_data.connection[i].vcc != NULL )
        {
            int sw_tx_queue_to_virt_queue_map[ATM_SW_TX_QUEUE_NUMBER] = {0};
            u32 sw_tx_queue_table;
            int sw_tx_queue;
            int virt_queue;

            for ( sw_tx_queue_table = g_atm_priv_data.connection[i].sw_tx_queue_table, sw_tx_queue = virt_queue = 0;
                  sw_tx_queue_table != 0;
                  sw_tx_queue_table >>= 1, sw_tx_queue++ )
                if ( (sw_tx_queue_table & 0x01) )
                    sw_tx_queue_to_virt_queue_map[sw_tx_queue] = virt_queue++;

            len += sprintf(page + len,"  pvc %d.%d\t:", (int)g_atm_priv_data.connection[i].vcc->vpi, (int)g_atm_priv_data.connection[i].vcc->vci);
            for ( j = 0; j < 8; j++ )
                len += sprintf(page + len," %2d", sw_tx_queue_to_virt_queue_map[g_atm_priv_data.connection[i].prio_queue_map[j]]);
            len += sprintf(page + len,    "\n");
            len += sprintf(page + len,"      phys qid\t:");
            for ( j = 0; j < 8; j++ )
                len += sprintf(page + len," %2d", g_atm_priv_data.connection[i].prio_queue_map[j]);
            len += sprintf(page + len,    "\n");
        }
    }

    *eof = 1;

    return len;
}

static int proc_write_prio(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 1;
    unsigned int port = ~0;
    int conn = -1;
    unsigned int vpi, vci;
    int prio = -1;
    int queue = -1;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "help") == 0 )
        {
            printk("echo <eth0/pvc vpi.vci> prio xx queue xx [prio xx queue xx] > /proc/eth/prio\n");
            printk("echo pvc vpi.vci <add/del> > /proc/eth/prio\n");
            break;
        }
        else if ( stricmp(p1, "eth0") == 0 )
        {
            port = 0;
            prio = queue = -1;
            printk("port = 0\n");
        }
        //else if ( stricmp(p1, "eth1") == 0 )
        //{
        //    port = 1;
        //    prio = queue = -1;
        //    printk("port = 1\n");
        //}
        else if ( stricmp(p1, "pvc") == 0 )
        {
            ignore_space(&p2, &len);
            vpi = get_number(&p2, &len, 0);
            ignore_space(&p2, &len);
            vci = get_number(&p2, &len, 0);
            conn = find_vpivci(vpi, vci);
            printk("vpi = %u, vci = %u, conn = %d\n", vpi, vci, conn);
            prio = queue = -1;
        }
        else if ( port != ~0 || conn >= 0 )
        {
            if ( stricmp(p1, "p") == 0 || stricmp(p1, "prio") == 0 )
            {
                ignore_space(&p2, &len);
                prio = get_number(&p2, &len, 0);
                printk("prio = %d\n", prio);
                if ( port >= 0 && port <= 1 && prio >= 0 && prio < NUM_ENTITY(g_eth_prio_queue_map[port]) )
                {
                    if ( queue >= 0 )
                        g_eth_prio_queue_map[port][prio] = queue;
                }
                else if ( conn >= 0 && prio >= 0 && prio < 8 )
                {
                    sw_tx_prio_to_queue(conn, prio, queue, NULL);
                }
                else
                    printk("prio (%d) is out of range 0 - %d\n", prio, conn >= 0 ? 7 : NUM_ENTITY(g_eth_prio_queue_map[port]) - 1);
            }
            else if ( stricmp(p1, "q") == 0 || stricmp(p1, "queue") == 0 )
            {
                ignore_space(&p2, &len);
                queue = get_number(&p2, &len, 0);
                printk("queue = %d\n", queue);
                if ( port >= 0 && port <= 1 )
                {
                    if ( queue >= 0 && queue <= 3 )
                    {
                        if ( prio >= 0 )
                            g_eth_prio_queue_map[port][prio] = queue;
                    }
                    else
                        printk("queue (%d) is out of range 0 - 3\n", queue);
                }
                else if ( conn >= 0 )
                {
                    int num_of_virt_queue;

                    if ( sw_tx_prio_to_queue(conn, prio, queue, &num_of_virt_queue) != 0 )
                        printk("queue (%d) is out of range 0 - %d\n", queue, num_of_virt_queue - 1);
                }
            }
            else if ( conn >= 0 && stricmp(p1, "add") == 0 )
            {
                int ret = sw_tx_queue_add(conn);

                if ( ret == 0 )
                    printk("add tx queue successfully\n");
                else
                    printk("failed in adding tx queue: %d\n", ret);
            }
            else if ( conn >= 0 && (stricmp(p1, "del") == 0 || stricmp(p1, "rem") == 0) )
            {
                int ret = sw_tx_queue_del(conn);

                if ( ret == 0 )
                    printk("remove tx queue successfully\n");
                else
                    printk("failed in removing tx queue: %d\n", ret);
            }
            else
                printk("unknown command (%s)\n", p1);
        }
        else
            printk("unknown command (%s)\n", p1);

        p1 = p2;
        colon = 1;
    }

    return count;
}

#if defined(ENABLE_BURST_WORKAROUND) && ENABLE_BURST_WORKAROUND

static int proc_read_softq(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i;

    len += sprintf(page + len, "g_softq_dbg_enable = 0x%08X\n", g_softq_dbg_enable);
    len += sprintf(page + len, "g_tx_queue_to_conn[%d] = ", NUM_ENTITY(g_tx_queue_to_conn));
    for ( i = 0; i < NUM_ENTITY(g_tx_queue_to_conn); i++ )
        len += sprintf(page + len, i == 0 ? "%d" : ", %d", g_tx_queue_to_conn[i]);
    len += sprintf(page + len, "\n");
    len += sprintf(page + len, "g_tx_queue_upper = %d, g_tx_queue_lower = %d\n", g_tx_queue_upper, g_tx_queue_lower);
    len += sprintf(page + len, "g_enable_softq = %d\n", g_enable_softq);
    len += sprintf(page + len, "g_skb_softq_max_length = %d\n", g_skb_softq_max_length);
    len += sprintf(page + len, "g_skb_softq[%d] = ", NUM_ENTITY(g_skb_softq));
    for ( i = 0; i < NUM_ENTITY(g_skb_softq); i++ )
        len += sprintf(page + len, i == 0 ? "%d" : ", %d", skb_queue_len(&g_skb_softq[i]));
    len += sprintf(page + len, "\n");
    len += sprintf(page + len, "g_tx_queued[%d] = ", NUM_ENTITY(g_tx_queued));
    for ( i = 0; i < NUM_ENTITY(g_tx_queued); i++ )
        len += sprintf(page + len, i == 0 ? "%d" : ", %d", g_tx_queued[i]);
    len += sprintf(page + len, "\n");


    *eof = 1;

    return len;
}

static int proc_write_softq(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 1;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "help") == 0 )
        {
            printk("echo g_softq_dbg_enable <mask of debug message>   > /proc/eth/softq\n");
            printk("echo g_tx_queue_upper <threshold to suspend send> > /proc/eth/softq\n");
            printk("echo g_tx_queue_lower <threshold to resume send>  > /proc/eth/softq\n");
            break;
        }
        else if ( stricmp(p1, "g_softq_dbg_enable") == 0 )
        {
            ignore_space(&p2, &len);
            g_softq_dbg_enable = get_number(&p2, &len, 1);
        }
        else if ( stricmp(p1, "g_tx_queue_upper") == 0 )
        {
            ignore_space(&p2, &len);
            g_tx_queue_upper = get_number(&p2, &len, 0);
        }
        else if ( stricmp(p1, "g_tx_queue_lower") == 0 )
        {
            ignore_space(&p2, &len);
            g_tx_queue_lower = get_number(&p2, &len, 0);
        }
        else if ( stricmp(p1, "g_enable_softq") == 0 )
        {
            ignore_space(&p2, &len);
            g_enable_softq = get_number(&p2, &len, 0);
        }
        else if ( stricmp(p1, "g_skb_softq_max_length") == 0 )
        {
            ignore_space(&p2, &len);
            g_skb_softq_max_length = get_number(&p2, &len, 0);
        }
        else if ( stricmp(p1, "clear") == 0 || stricmp(p1, "clean") == 0 )
            memset(g_tx_queued, 0, sizeof(g_tx_queued));

        p1 = p2;
        colon = 1;
    }

    return count;
}

#endif

static INLINE int proc_buf_copy(char **pbuf, int size, off_t off, int *ppos, const char *str, int len)
{
    if ( *ppos <= off && *ppos + len > off )
    {
        my_memcpy(*pbuf, str + off - *ppos, *ppos + len - off);
        *pbuf += *ppos + len - off;
    }
    else if ( *ppos > off )
    {
        my_memcpy(*pbuf, str, len);
        *pbuf += len;
    }
    *ppos += len;
    return *ppos >= off + size;
}

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

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
static INLINE int strincmp(const char *p1, const char *p2, int n)
{
    int c1 = 0, c2;

    while ( n && *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
        n--;
    }

    return n ? *p1 - *p2 : c1;
}
#endif

static INLINE int get_token(char **p1, char **p2, int *len, int *colon)
{
    int tlen = 0;

    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z') || (**p1 >= '0' && **p1<= '9')) )
    {
        (*p1)++;
        (*len)--;
    }
    if ( !*len )
        return 0;

    if ( *colon )
    {
        *colon = 0;
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            if ( **p2 == ':' )
            {
                *colon = 1;
                break;
            }
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }
    else
    {
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }

    return tlen;
}

static INLINE int get_number(char **p, int *len, int is_hex)
{
    int ret = 0;
    int n = 0;

    if ( (*p)[0] == '0' && (*p)[1] == 'x' )
    {
        is_hex = 1;
        (*p) += 2;
        (*len) -= 2;
    }

    if ( is_hex )
    {
        while ( *len && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F')) )
        {
            if ( **p >= '0' && **p <= '9' )
                n = **p - '0';
            else if ( **p >= 'a' && **p <= 'f' )
               n = **p - 'a' + 10;
            else if ( **p >= 'A' && **p <= 'F' )
                n = **p - 'A' + 10;
            ret = (ret << 4) | n;
            (*p)++;
            (*len)--;
        }
    }
    else
    {
        while ( *len && **p >= '0' && **p <= '9' )
        {
            n = **p - '0';
            ret = ret * 10 + n;
            (*p)++;
            (*len)--;
        }
    }

    return ret;
}

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
static INLINE void get_ip_port(char **p, int *len, unsigned int *val)
{
    int i;

    memset(val, 0, sizeof(*val) * 5);

    for ( i = 0; i < 5; i++ )
    {
        ignore_space(p, len);
        if ( !*len )
            break;
        val[i] = get_number(p, len, 0);
    }
}

static INLINE void get_mac(char **p, int *len, unsigned int *val)
{
    int i;

    memset(val, 0, sizeof(*val) * 6);

    for ( i = 0; i < 6; i++ )
    {
        ignore_space(p, len);
        if ( !*len )
            break;
        val[i] = get_number(p, len, 1);
    }
}
#endif

static INLINE void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
static INLINE int print_wan_route(char *buf, int i, struct rout_forward_compare_tbl *pcompare, struct rout_forward_action_tbl *pwaction)
{
    static const char *dest_list[] = {"ETH0", "ATM", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d\n", i);
    len += sprintf(buf + len,          "    compare (0x%08X)\n", (u32)pcompare);
    len += sprintf(buf + len,          "      src:  %d.%d.%d.%d:%d\n", pcompare->src_ip >> 24,  (pcompare->src_ip >> 16) & 0xFF,  (pcompare->src_ip >> 8) & 0xFF,  pcompare->src_ip & 0xFF, pcompare->src_port);
    len += sprintf(buf + len,          "      dest: %d.%d.%d.%d:%d\n", pcompare->dest_ip >> 24, (pcompare->dest_ip >> 16) & 0xFF, (pcompare->dest_ip >> 8) & 0xFF, pcompare->dest_ip & 0xFF, pcompare->dest_port);
    len += sprintf(buf + len,          "    action  (0x%08X)\n", (u32)pwaction);
    len += sprintf(buf + len,          "      new dest:    %d.%d.%d.%d:%d\n", pwaction->new_ip >> 24, (pwaction->new_ip >> 16) & 0xFF, (pwaction->new_ip >> 8) & 0xFF, pwaction->new_ip & 0xFF, pwaction->new_port);
    len += sprintf(buf + len,          "      new MAC :    %02X:%02X:%02X:%02X:%02X:%02X\n", (pwaction->new_dest_mac54 >> 8) & 0xFF, pwaction->new_dest_mac54 & 0xFF, pwaction->new_dest_mac30 >> 24, (pwaction->new_dest_mac30 >> 16) & 0xFF, (pwaction->new_dest_mac30 >> 8) & 0xFF, pwaction->new_dest_mac30 & 0xFF);
    switch ( pwaction->rout_type )
    {
    case 1:  len += sprintf(buf + len, "      route type:  IPv4\n"); break;
    case 2:  len += sprintf(buf + len, "      route type:  NAT\n");  break;
    case 3:  len += sprintf(buf + len, "      route type:  NAPT\n"); break;
    default: len += sprintf(buf + len, "      route type:  NULL\n");
    }
    if ( pwaction->new_dscp_en )
        len += sprintf(buf + len,      "      new DSCP:    %d\n", pwaction->new_dscp);
    else
        len += sprintf(buf + len,      "      new DSCP:    original (not modified)\n");
    len += sprintf(buf + len,          "      MTU index:   %d\n", pwaction->mtu_ix);
    if ( pwaction->in_vlan_ins )
        len += sprintf(buf + len,      "      VLAN insert: enable, VCI 0x%04x\n", pwaction->new_in_vci);
    else
        len += sprintf(buf + len,      "      VLAN insert: disable\n");
    len += sprintf(buf + len,          "      VLAN remove: %s\n", pwaction->in_vlan_rm ? "enable" : "disable");
    if ( !pwaction->dest_list )
        len += sprintf(buf + len,      "      dest list:   none\n");
    else
    {
        len += sprintf(buf + len,      "      dest list:   ");
        for ( bit = 1, j = k = 0; bit < 1 << 8; bit <<= 1, j++ )
            if ( (pwaction->dest_list & bit) )
            {
                if ( k )
                    len += sprintf(buf + len, ", ");
                len += sprintf(buf + len, dest_list[j]);
                k = 1;
            }
        len += sprintf(buf + len, "\n");
    }
    if ( pwaction->pppoe_mode )
        len += sprintf(buf + len,      "      PPPoE mode:  termination\n");
    else
        len += sprintf(buf + len,      "      PPPoE mode:  transparent\n");
    len += sprintf(buf + len,          "      new src MAC index: %d\n", pwaction->new_src_mac_ix);
    if ( pwaction->out_vlan_ins )
        len += sprintf(buf + len,      "      outer VLAN insert: enable, VCI index %d\n", pwaction->out_vlan_ix);
    else
        len += sprintf(buf + len,      "      outer VLAN insert: disable\n");
    len += sprintf(buf + len,          "      outer VLAN remove: %s\n", pwaction->out_vlan_rm ? "enable" : "disable");

    return len;
}

static INLINE int print_lan_route(char *buf, int i, struct rout_forward_compare_tbl *pcompare, struct rout_forward_action_tbl *plaction)
{
    static const char *dest_list[] = {"ETH0", "ATM", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};
    static const char *mpoa_type[] = {"EoA w/o FCS", "EoA w FCS", "PPPoA", "IPoA"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d\n", i);
    len += sprintf(buf + len,          "    compare (0x%08X)\n", (u32)pcompare);
    len += sprintf(buf + len,          "      src:  %d.%d.%d.%d:%d\n", pcompare->src_ip >> 24,  (pcompare->src_ip >> 16) & 0xFF,  (pcompare->src_ip >> 8) & 0xFF,  pcompare->src_ip & 0xFF, pcompare->src_port);
    len += sprintf(buf + len,          "      dest: %d.%d.%d.%d:%d\n", pcompare->dest_ip >> 24, (pcompare->dest_ip >> 16) & 0xFF, (pcompare->dest_ip >> 8) & 0xFF, pcompare->dest_ip & 0xFF, pcompare->dest_port);
    len += sprintf(buf + len,          "    action  (0x%08X)\n", (u32)plaction);
    len += sprintf(buf + len,          "      new src:     %d.%d.%d.%d:%d\n", plaction->new_ip >> 24, (plaction->new_ip >> 16) & 0xFF, (plaction->new_ip >> 8) & 0xFF, plaction->new_ip & 0xFF, plaction->new_port);
    len += sprintf(buf + len,          "      new MAC:     %02X:%02X:%02X:%02X:%02X:%02X\n", (plaction->new_dest_mac54 >> 8) & 0xFF, plaction->new_dest_mac54 & 0xFF, plaction->new_dest_mac30 >> 24, (plaction->new_dest_mac30 >> 16) & 0xFF, (plaction->new_dest_mac30 >> 8) & 0xFF, plaction->new_dest_mac30 & 0xFF);
    switch ( plaction->rout_type )
    {
    case 1:  len += sprintf(buf + len, "      route type:  IPv4\n"); break;
    case 2:  len += sprintf(buf + len, "      route type:  NAT\n");  break;
    case 3:  len += sprintf(buf + len, "      route type:  NAPT\n"); break;
    default: len += sprintf(buf + len, "      route type:  NULL\n");
    }
    if ( plaction->new_dscp_en )
        len += sprintf(buf + len,      "      new DSCP:    %d\n", plaction->new_dscp);
    else
        len += sprintf(buf + len,      "      new DSCP:    original (not modified)\n");
    len += sprintf(buf + len,          "      MTU index:   %d\n", plaction->mtu_ix);
    if ( plaction->in_vlan_ins )
        len += sprintf(buf + len,      "      VLAN insert: enable, VCI 0x%04x\n", plaction->new_in_vci);
    else
        len += sprintf(buf + len,      "      VLAN insert: disable\n");
    len += sprintf(buf + len,          "      VLAN remove: %s\n", plaction->in_vlan_rm ? "enable" : "disable");
    if ( !plaction->dest_list )
        len += sprintf(buf + len,      "      dest list:   none\n");
    else
    {
        len += sprintf(buf + len,      "      dest list:   ");
        for ( bit = 1, j = k = 0; bit < 1 << 8; bit <<= 1, j++ )
            if ( (plaction->dest_list & bit) )
            {
                if ( k )
                    len += sprintf(buf + len, ", ");
                len += sprintf(buf + len, dest_list[j]);
                k = 1;
            }
        len += sprintf(buf + len, "\n");
    }
    if ( plaction->pppoe_mode )
        len += sprintf(buf + len,      "      PPPoE mode:  termination\n");
    else
        len += sprintf(buf + len,      "      PPPoE mode:  transparent\n");
    len += sprintf(buf + len,          "      PPPoE index: %d\n", plaction->pppoe_ix);
    len += sprintf(buf + len,          "      new src MAC index: %d\n", plaction->new_src_mac_ix);
    if ( plaction->out_vlan_ins )
        len += sprintf(buf + len,      "      outer VLAN insert: enable, VCI index %d\n", plaction->out_vlan_ix);
    else
        len += sprintf(buf + len,      "      outer VLAN insert: disable\n");
    len += sprintf(buf + len,          "      outer VLAN remove: %s\n", plaction->out_vlan_rm ? "enable" : "disable");
    len += sprintf(buf + len,          "      MPoA type:         %s\n", mpoa_type[plaction->mpoa_type]);
    len += sprintf(buf + len,          "      DSLWAN QID:        %d\n", plaction->dslwan_qid);

    return len;
}

static INLINE int print_bridge(char *buf, int i, struct brg_forward_tbl *paction)
{
    static const char *dest_list[] = {"ETH0", "ATM", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d (0x%08X)\n", i, (u32)paction);
    len += sprintf(buf + len,          "    compare\n");
    len += sprintf(buf + len,          "      mac:          %02X:%02X:%02X:%02X:%02X:%02X\n", paction->mac_52 >> 24, (paction->mac_52 >> 16) & 0xFF, (paction->mac_52 >> 8) & 0xFF, paction->mac_52 & 0xFF, (paction->mac_10 >> 8) & 0xFF, paction->mac_10 & 0xFF);
    len += sprintf(buf + len,          "    action\n");
    if ( !paction->dest_list )
        len += sprintf(buf + len,      "      dest list:    none\n");
    else
    {
        len += sprintf(buf + len,      "      dest list:    ");
        for ( bit = 1, j = k = 0; j < 8; bit <<= 1, j++ )
            if ( (paction->dest_list & bit) )
            {
                if ( k )
                    len += sprintf(buf + len, ", ");
                len += sprintf(buf + len, dest_list[j]);
                k = 1;
            }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len,          "      src mac drop: %s\n", paction->src_mac_drop ? "enable" : "disable");
    switch ( paction->itf )
    {
    case 0x00:
        len += sprintf(buf + len,      "      port:         ETH0\n");
        break;
    case 0x01:
        len += sprintf(buf + len,      "      port:         ATM\n");
        len += sprintf(buf + len,      "      DSLWAN QID:   %d\n", paction->dslwan_qid);
        break;
    case 0x02:
        len += sprintf(buf + len,      "      port:         CPU0\n");
        break;
    case 0x03:
        len += sprintf(buf + len,      "      port:         CPU0 EXT IF 0\n");
        break;
    default:
        len += sprintf(buf + len,      "      port:         Unknown\n");
    }

    return len;
}

static INLINE int print_mc(char *buf, int i, struct wan_rout_multicast_tbl *paction)
{
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "res", "ATM"};

    struct wan_rout_mc_outer_vlan_tbl vlan_tbl;
    int len = 0;
    u32 bit;
    int j, k;

    vlan_tbl = *WAN_ROUT_MC_OUTER_VLAN_TBL(i);
    ((unsigned char *)&vlan_tbl)[0] = ((unsigned char *)&vlan_tbl)[i & 0x03];

    len += sprintf(buf + len,          "  entry %d\n", i);
    len += sprintf(buf + len,          "    compare\n");
    len += sprintf(buf + len,          "      wan_dest_ip:  %d.%d.%d.%d\n", (paction->wan_dest_ip >> 24) & 0xFF,  (paction->wan_dest_ip >> 16) & 0xFF,  (paction->wan_dest_ip >> 8) & 0xFF,  paction->wan_dest_ip & 0xFF);
    len += sprintf(buf + len,          "    action  (0x%08X)\n", (u32)paction);
    if ( paction->in_vlan_ins )
        len += sprintf(buf + len,      "      VLAN insert:  enable, VCI 0x%04x\n", paction->new_in_vci);
    else
        len += sprintf(buf + len,      "      VLAN insert:  disable\n");
    len += sprintf(buf + len,          "      VLAN remove:  %s\n", paction->in_vlan_rm ? "enable" : "disable");
    if ( !paction->dest_list )
        len += sprintf(buf + len,      "      dest list:    none\n");
    else
    {
        len += sprintf(buf + len,      "      dest list:   ");
        for ( bit = 1, j = k = 0; bit < 1 << 8; bit <<= 1, j++ )
            if ( (paction->dest_list & bit) )
            {
                if ( k )
                    len += sprintf(buf + len, ", ");
                len += sprintf(buf + len, dest_list[j]);
                k = 1;
            }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len,          "      PPPoE mode:   %s\n", paction->pppoe_mode ? "termination" : "transparent");
    if ( paction->new_src_mac_en )
        len += sprintf(buf + len,      "      new src MAC index: %d\n", paction->new_src_mac_ix);
    else
        len += sprintf(buf + len,      "      new src MAC index: disabled\n");
    if ( vlan_tbl.out_vlan_ins0 )
        len += sprintf(buf + len,      "      outer VLAN insert: enable, index %d\n", vlan_tbl.out_vlan_ix0);
    else
        len += sprintf(buf + len,      "      outer VLAN insert: disable\n");
    len += sprintf(buf + len,          "      outer VLAN remove: %s\n", vlan_tbl.out_vlan_rm0 ? "enable" : "disable");

    return len;
}
#endif

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
static INLINE void dump_skb(struct sk_buff *skb, u32 len, char *title, int ch, int is_tx)
{
    int i;

    if ( !(g_dbg_enable & (is_tx ? DBG_ENABLE_MASK_DUMP_SKB_TX : DBG_ENABLE_MASK_DUMP_SKB_RX)) )
        return;

    if ( skb->len < len )
        len = skb->len;

    if ( len > DMA_PACKET_SIZE )
    {
        printk("too big data length: skb = %08x, skb->data = %08x, skb->len = %d\n", (u32)skb, (u32)skb->data, skb->len);
        return;
    }

    if ( ch >= 0 )
        printk("%s (ch %d)\n", title, ch);
    else
        printk("%s\n", title);
    printk("  skb->data = %08X, skb->tail = %08X, skb->len = %d\n", (u32)skb->data, (u32)skb->tail, (int)skb->len);
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
}
#endif

#if defined(DEBUG_DUMP_FLAG_HEADER) && DEBUG_DUMP_FLAG_HEADER
static INLINE void dump_flag_header(int fwcode, struct flag_header *header, const char *title, int ch, int is_wlan)
{
    static char * mpoa_type_str[] = {
        "EoA w/o FCS",
        "EoA w FCS",
        "PPPoA",
        "IPoA"
    };
    static char * is_vlan_str[] = {
        "nil",
        "single tag",
        "double tag",
        "reserved"
    };

    if ( !(g_dbg_enable & DBG_ENABLE_MASK_DUMP_FLAG_HEADER) )
        return;

    if ( ch >= 0 )
        printk("%s (ch %d)\n", title, ch);
    else
        printk("%s\n", title);

    switch ( fwcode )
    {
    case FWCODE_ROUTING_BRIDGING_ACC_A4:
        if ( is_wlan )
        {
            struct wlan_flag_header * p = (struct wlan_flag_header *)header;

            printk("  aal5_raw       = %Xh (%s)\n", (u32)p->aal5_raw, p->aal5_raw ? "AAL5" : "ETH");
            printk("  mpoa_type      = %Xh (%s)\n", (u32)p->mpoa_type, mpoa_type_str[p->mpoa_type]);
            printk("  dsl_qid        = %Xh\n", (u32)p->dsl_qid);
            printk("  src_itf        = %Xh\n", (u32)header->src_itf);
        }
        else
        {
            printk("  rout_fwd_vld   = %Xh\n", (u32)header->rout_fwd_vld);
            printk("  rout_mc_vld    = %Xh\n", (u32)header->rout_mc_vld);
            printk("  is_brg         = %Xh (%s)\n", (u32)header->is_brg, header->is_brg ? "bridging" : "routing");
            printk("  uc_mc_flag     = %Xh (%s)\n", (u32)header->uc_mc_flag, header->uc_mc_flag ? "multicast" : "uni-cast");
            printk("  is_udp         = %Xh\n", (u32)header->is_udp);
            printk("  is_tcp         = %Xh\n", (u32)header->is_tcp);
            printk("  mpoa_type      = %Xh (%s)\n", (u32)header->mpoa_type, mpoa_type_str[header->mpoa_type]);
            printk("  ip_offset      = %Xh\n", (u32)header->ip_offset);
            printk("  is_pppoes      = %Xh\n", (u32)header->is_pppoes);
            printk("  is_ipv4        = %Xh\n", (u32)header->is_ipv4);
            printk("  is_vlan        = %Xh (%s)\n", (u32)header->is_vlan, is_vlan_str[header->is_vlan]);
            printk("  rout_index     = %Xh\n", (u32)header->rout_index);
            printk("  dest_list      = %Xh\n", (u32)header->dest_list);
            printk("  aal5_raw       = %Xh (%s)\n", (u32)header->aal5_raw, header->aal5_raw ? "AAL5" : "ETH");
            printk("  src_dir        = %Xh (from %s side)\n", (u32)header->src_dir, header->src_dir ? "WAN" : "LAN");
            printk("  acc_done       = %Xh\n", (u32)header->acc_done);
            printk("  tcp_rst        = %Xh\n", (u32)header->tcp_rst);
            printk("  tcp_fin        = %Xh\n", (u32)header->tcp_fin);
            printk("  dsl_qid        = %Xh\n", (u32)header->dsl_qid);
            printk("  temp_dest_list = %Xh\n", (u32)header->temp_dest_list);
            printk("  src_itf        = %Xh\n", (u32)header->src_itf);
            printk("  pl_byteoff     = %Xh\n", (u32)header->pl_byteoff);
        }
        break;
    }
}
#endif

#if defined(DEBUG_DUMP_INIT) && DEBUG_DUMP_INIT
static INLINE void dump_init(void)
{
    int i;

    if ( !(g_dbg_enable & DBG_ENABLE_MASK_DUMP_INIT) )
        return;

    printk("Share Buffer Conf:\n");
    printk("  SB_MST_PRI0(%08X) = 0x%08X\n", (u32)SB_MST_PRI0, *SB_MST_PRI0);
    printk("  SB_MST_PRI1(%08X) = 0x%08X\n", (u32)SB_MST_PRI1, *SB_MST_PRI1);

    printk("ETOP:\n");
    printk("  ETOP_MDIO_CFG        = 0x%08X\n", *ETOP_MDIO_CFG);
    printk("  ETOP_MDIO_ACC        = 0x%08X\n", *ETOP_MDIO_ACC);
    printk("  ETOP_CFG             = 0x%08X\n", *ETOP_CFG);
    printk("  ETOP_IG_VLAN_COS     = 0x%08X\n", *ETOP_IG_VLAN_COS);
    printk("  ETOP_IG_DSCP_COSx(0) = 0x%08X\n", *ETOP_IG_DSCP_COSx(0));
    printk("  ETOP_IG_DSCP_COSx(1) = 0x%08X\n", *ETOP_IG_DSCP_COSx(1));
    printk("  ETOP_IG_DSCP_COSx(2) = 0x%08X\n", *ETOP_IG_DSCP_COSx(2));
    printk("  ETOP_IG_DSCP_COSx(3) = 0x%08X\n", *ETOP_IG_DSCP_COSx(3));
    printk("  ETOP_IG_PLEN_CTRL0   = 0x%08X\n", *ETOP_IG_PLEN_CTRL0);
    printk("  ETOP_ISR             = 0x%08X\n", *ETOP_ISR);
    printk("  ETOP_IER             = 0x%08X\n", *ETOP_IER);
    printk("  ETOP_VPID            = 0x%08X\n", *ETOP_VPID);

    for ( i = 0; i < 2; i++ )
    {
        printk("ENET%d:\n", i);
        printk("  ENET_MAC_CFG(%d)      = 0x%08X\n", i, *ENET_MAC_CFG(i));
        printk("  ENETS_DBA(%d)         = 0x%08X\n", i, *ENETS_DBA(i));
        printk("  ENETS_CBA(%d)         = 0x%08X\n", i, *ENETS_CBA(i));
        printk("  ENETS_CFG(%d)         = 0x%08X\n", i, *ENETS_CFG(i));
        printk("  ENETS_PGCNT(%d)       = 0x%08X\n", i, *ENETS_PGCNT(i));
        printk("  ENETS_PKTCNT(%d)      = 0x%08X\n", i, *ENETS_PKTCNT(i));
        printk("  ENETS_BUF_CTRL(%d)    = 0x%08X\n", i, *ENETS_BUF_CTRL(i));
        printk("  ENETS_COS_CFG(%d)     = 0x%08X\n", i, *ENETS_COS_CFG(i));
        printk("  ENETS_IGDROP(%d)      = 0x%08X\n", i, *ENETS_IGDROP(i));
        printk("  ENETS_IGERR(%d)       = 0x%08X\n", i, *ENETS_IGERR(i));
        printk("  ENETS_MAC_DA0(%d)     = 0x%08X\n", i, *ENETS_MAC_DA0(i));
        printk("  ENETS_MAC_DA1(%d)     = 0x%08X\n", i, *ENETS_MAC_DA1(i));
        printk("  ENETF_DBA(%d)         = 0x%08X\n", i, *ENETF_DBA(i));
        printk("  ENETF_CBA(%d)         = 0x%08X\n", i, *ENETF_CBA(i));
        printk("  ENETF_CFG(%d)         = 0x%08X\n", i, *ENETF_CFG(i));
        printk("  ENETF_PGCNT(%d)       = 0x%08X\n", i, *ENETF_PGCNT(i));
        printk("  ENETF_PKTCNT(%d)      = 0x%08X\n", i, *ENETF_PKTCNT(i));
        printk("  ENETF_HFCTRL(%d)      = 0x%08X\n", i, *ENETF_HFCTRL(i));
        printk("  ENETF_TXCTRL(%d)      = 0x%08X\n", i, *ENETF_TXCTRL(i));
        printk("  ENETF_VLCOS0(%d)      = 0x%08X\n", i, *ENETF_VLCOS0(i));
        printk("  ENETF_VLCOS1(%d)      = 0x%08X\n", i, *ENETF_VLCOS1(i));
        printk("  ENETF_VLCOS2(%d)      = 0x%08X\n", i, *ENETF_VLCOS2(i));
        printk("  ENETF_VLCOS3(%d)      = 0x%08X\n", i, *ENETF_VLCOS3(i));
        printk("  ENETF_EGCOL(%d)       = 0x%08X\n", i, *ENETF_EGCOL(i));
        printk("  ENETF_EGDROP(%d)      = 0x%08X\n", i, *ENETF_EGDROP(i));
    }

    printk("DPLUS:\n");
    printk("  DPLUS_TXDB           = 0x%08X\n", *DPLUS_TXDB);
    printk("  DPLUS_TXCB           = 0x%08X\n", *DPLUS_TXCB);
    printk("  DPLUS_TXCFG          = 0x%08X\n", *DPLUS_TXCFG);
    printk("  DPLUS_TXPGCNT        = 0x%08X\n", *DPLUS_TXPGCNT);
    printk("  DPLUS_RXDB           = 0x%08X\n", *DPLUS_RXDB);
    printk("  DPLUS_RXCB           = 0x%08X\n", *DPLUS_RXCB);
    printk("  DPLUS_RXCFG          = 0x%08X\n", *DPLUS_RXCFG);
    printk("  DPLUS_RXPGCNT        = 0x%08X\n", *DPLUS_RXPGCNT);

    printk("Communication:\n");
    printk("  FW_VER_ID(%08X)  = 0x%08X\n", (u32)FW_VER_ID, *(u32*)FW_VER_ID);
}
#endif

#if defined(ENABLE_DIRECTPATH_TX_QUEUE) && ENABLE_DIRECTPATH_TX_QUEUE
static int send_directpath_tx_queue(void)
{
    unsigned long sys_flag;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;
    struct sk_buff *skb;
    int i;

    local_irq_save(sys_flag);
    for ( i = 0; i < NUM_ENTITY(g_ppe_directpath_data); i++ )
    {
        while ( g_ppe_directpath_data[i].skb_list )
        {
            desc = WLAN_CPU_TX_DESC_BASE + g_wlan_cpu_tx_desc_pos;
            if ( desc->own )    //  PPE hold
            {
                local_irq_restore(sys_flag);
                return -1;
            }

            //  remove skb from list
            skb = g_ppe_directpath_data[i].skb_list;
            if ( skb->prev == skb )
                skb->prev = skb->next = g_ppe_directpath_data[i].skb_list = NULL;
            else
            {
                g_ppe_directpath_data[i].skb_list = skb->next;
                g_ppe_directpath_data[i].skb_list->prev = skb->prev;
                skb->prev->next = skb->next;
                skb->prev = skb->next = NULL;

                g_ppe_directpath_data[i].skb_list_size -= skb->len;
            }

            //  prepare to free previous skb
            skb_to_free = g_wlan_cpu_tx_desc_skb[g_wlan_cpu_tx_desc_pos];
            get_skb_from_dbg_pool(skb_to_free);
            g_wlan_cpu_tx_desc_skb[g_wlan_cpu_tx_desc_pos] = skb;
            put_skb_to_dbg_pool(skb);
            //  move to next descriptor
            if ( ++g_wlan_cpu_tx_desc_pos == WLAN_CPU_TX_DESC_NUM )
                g_wlan_cpu_tx_desc_pos = 0;

            /*  free previous skb   */
            dev_kfree_skb_any(skb_to_free);

            /*  load descriptor from memory */
            reg_desc = *desc;

            /*  update descriptor   */
            reg_desc.dataptr    = ((u32)skb->data & 0x1FFFFFFC) >> 2;   //  dword address
            reg_desc.byteoff    = ((u32)skb->data & 3);                 //  should be 0
            reg_desc.datalen    = skb->len <= ETH_MIN_TX_PACKET_LENGTH + 2 ? ETH_MIN_TX_PACKET_LENGTH + 6 : skb->len + 6;   //  to complement 4 bytes of CRC to the PPE firmware, 2 bytes are inband header
            reg_desc.own        = 1;
            reg_desc.c          = 0;

  #ifndef CONFIG_MIPS_UNCACHED
            /*  write back to physical memory   */
            dma_cache_wback((u32)skb->data, skb->len);
  #endif

            /*  write discriptor to memory and write back cache */
            *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
            *(volatile u32 *)desc = *(u32 *)&reg_desc;

            if ( g_ppe_directpath_data[i].skb_list_size <= (ENABLE_DIRECTPATH_TX_QUEUE_SIZE * 3 / 4)
                && g_ppe_directpath_data[i].callback.start_tx_fn )
                g_ppe_directpath_data[i].callback.start_tx_fn(g_ppe_directpath_data[i].netif);
        }
    }
    local_irq_restore(sys_flag);

    return 0;
}
#endif



/*
 * ####################################
 *           Global Function
 * ####################################
 */

int32_t ppa_datapath_generic_hook(PPA_GENERIC_DATAPATH_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
    dbg("ppa_datapath_generic_hook cmd 0x%x\n", cmd);
    switch ( cmd )
    {
    case PPA_GENERIC_DATAPATH_TSET:
        return IFX_SUCCESS;

    case PPA_GENERIC_DATAPATH_ADDR_TO_FPI_ADDR:
        {
            PPA_FPI_ADDR *addr = (PPA_FPI_ADDR *) buffer;
            addr->addr_fpi = sb_addr_to_fpi_addr_convert(addr->addr_orig);
            return IFX_SUCCESS;
        }

    default:
        dbg("ppa_datapath_generic_hook not support cmd 0x%x\n", cmd );
        return -1;
    }

    return -1;
}

static int atm_showtime_enter(struct port_cell_info *port_cell, void *xdata_addr)
{
    int i, j;

    ASSERT(port_cell != NULL, "port_cell is NULL");
    ASSERT(xdata_addr != NULL, "xdata_addr is NULL");

    for ( j = 0; j < ATM_PORT_NUMBER && j < port_cell->port_num; j++ )
        if ( port_cell->tx_link_rate[j] > 0 )
            break;
    for ( i = 0; i < ATM_PORT_NUMBER && i < port_cell->port_num; i++ )
        g_atm_priv_data.port[i].tx_max_cell_rate = port_cell->tx_link_rate[i] > 0 ? port_cell->tx_link_rate[i] : port_cell->tx_link_rate[j];

    qsb_global_set();

    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
        if ( (g_atm_priv_data.connection_table & (1 << i)) && g_atm_priv_data.connection[i].vcc != NULL )
            set_qsb(g_atm_priv_data.connection[i].vcc, &g_atm_priv_data.connection[i].vcc->qos, i);

    //  TODO: ReTX set xdata_addr
    g_xdata_addr = xdata_addr;

    g_showtime = 1;

    dbg("enter showtime, cell rate: 0 - %d, xdata addr: 0x%08x", g_atm_priv_data.port[0].tx_max_cell_rate, (unsigned int)g_xdata_addr);

    return IFX_SUCCESS;
}

static int atm_showtime_exit(void)
{
    if ( !g_showtime )
        return IFX_ERROR;

    g_showtime = 0;

    //  TODO: ReTX clean state
    g_xdata_addr = NULL;

    dbg("leave showtime");

    return IFX_SUCCESS;
}

int get_dslwan_qid_with_vcc(struct atm_vcc *vcc)
{
    int conn = find_vcc(vcc);
    u32 sw_tx_queue_table;
    int sw_tx_queue;

    if ( conn >= 0 )
    {
        for ( sw_tx_queue_table = g_atm_priv_data.connection[conn].sw_tx_queue_table, sw_tx_queue = 0;
              sw_tx_queue_table != 0;
              sw_tx_queue_table >>= 1, sw_tx_queue++ )
            if ( (sw_tx_queue_table & 0x01) )
                break;

        conn = (conn << 8) | sw_tx_queue;
    }

    return conn;
}

int get_netif_qid_with_pkt(struct sk_buff *skb, void *arg, int is_atm_vcc)
{
    if ( is_atm_vcc )
    {
        int conn = find_vcc((struct atm_vcc *)arg);

        if ( conn < 0 )
            return -1;

        return (conn << 8) | g_atm_priv_data.connection[conn].prio_queue_map[skb->priority > 7 ? 7 : skb->priority];
    }
    else
        return 0;
}


#if defined(ENABLE_USB_WLAN_SUPPORT) && ENABLE_USB_WLAN_SUPPORT
int ppe_directpath_send(uint32_t if_id, struct sk_buff *skb, int32_t len, uint32_t flags)
{
    //  Careful, no any safety check here.
    //  Parameters must be correct.
    unsigned long sys_flag;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;
    int byteoff;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_directpath_send - org", 0, 1);

    /*  reserve space to put pointer in skb */
    byteoff = (u32)skb->data & 3;
    if ( byteoff != 2 || skb_headroom(skb) < byteoff )
    {
        //  this should be few case
        struct sk_buff *new_skb;

        new_skb = alloc_skb_tx(DMA_PACKET_SIZE);    //  not necessary to be that large
        if ( !new_skb )
        {
            err("no memory");
            goto ALLOC_SKB_TX_FAIL;
        }
        skb_reserve(new_skb, 2);    //  for WLAN TX inband header

        my_memcpy(new_skb->data, skb->data, skb->len);

        dev_kfree_skb_any(skb);
        skb = new_skb;
    }

    //  put WLAN TX inband header (2-byte, Set SRC_ITF bits)
    skb_push(skb, 2);
    *(u16*)skb->data = 0x0000 | (if_id & 0x07);

 #if defined(ENABLE_DIRECTPATH_TX_QUEUE) && ENABLE_DIRECTPATH_TX_QUEUE
    if ( g_directpath_tx_full )
    {
        uint32_t off = if_id - 3;

        if ( g_ppe_directpath_data[off].skb_list_size + skb->len > ENABLE_DIRECTPATH_TX_QUEUE_SIZE )
        {
            if ( g_ppe_directpath_data[off].callback.stop_tx_fn )
                g_ppe_directpath_data[off].callback.stop_tx_fn(g_ppe_directpath_data[off].netif);

            goto NO_FREE_DESC;
        }

        local_irq_save(sys_flag);
        g_ppe_directpath_data[off].skb_list_size += skb->len;
        if ( g_ppe_directpath_data[off].skb_list != NULL )
        {
            g_ppe_directpath_data[off].skb_list->prev->next = skb;
            skb->next = g_ppe_directpath_data[off].skb_list;
            skb->prev = g_ppe_directpath_data[off].skb_list->prev;
            g_ppe_directpath_data[off].skb_list->prev = skb;
        }
        else
        {
            g_ppe_directpath_data[off].skb_list = skb;
            skb->prev = skb->next = skb;
        }
        local_irq_restore(sys_flag);
        return 0;
    }
 #endif

    /*  allocate descriptor */
    local_irq_save(sys_flag);
    desc = WLAN_CPU_TX_DESC_BASE + g_wlan_cpu_tx_desc_pos;
    if ( desc->own )    //  PPE hold
    {
        local_irq_restore(sys_flag);
        err("PPE hold");
        goto NO_FREE_DESC;
    }
    //  prepare to free previous skb
    skb_to_free = g_wlan_cpu_tx_desc_skb[g_wlan_cpu_tx_desc_pos];
    get_skb_from_dbg_pool(skb_to_free);
    g_wlan_cpu_tx_desc_skb[g_wlan_cpu_tx_desc_pos] = skb;
    put_skb_to_dbg_pool(skb);
    //  move to next descriptor
    if ( ++g_wlan_cpu_tx_desc_pos == WLAN_CPU_TX_DESC_NUM )
        g_wlan_cpu_tx_desc_pos = 0;
    //  flow control
    if ( !g_directpath_tx_full && WLAN_CPU_TX_DESC_BASE[g_wlan_cpu_tx_desc_pos].own )
    {
#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
        int i;
#endif

        g_directpath_tx_full = 1;

#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
        for ( i = 0; i < NUM_ENTITY(g_ppe_directpath_data); i++ )
            if ( g_ppe_directpath_data[i].callback.stop_tx_fn )
                g_ppe_directpath_data[i].callback.stop_tx_fn(g_ppe_directpath_data[i].netif);
#endif

        *MBOX_IGU1_ISRC = WLAN_CPU_TX_SIG;
        *MBOX_IGU1_IER |= WLAN_CPU_TX_SIG;
    }
    local_irq_restore(sys_flag);

    /*  free previous skb   */
    dev_kfree_skb_any(skb_to_free);

    /*  load descriptor from memory */
    reg_desc = *desc;

    /*  update descriptor   */
    reg_desc.dataptr    = ((u32)skb->data & 0x1FFFFFFC) >> 2;   //  dword address
    reg_desc.byteoff    = ((u32)skb->data & 3);                 //  should be 0
    reg_desc.datalen    = skb->len <= ETH_MIN_TX_PACKET_LENGTH + 2 ? ETH_MIN_TX_PACKET_LENGTH + 6 : skb->len + 6;   //  to complement 4 bytes of CRC to the PPE firmware, 2 bytes are inband header
    reg_desc.own        = 1;
    reg_desc.c          = 0;

    /* TODO: Increment tx stats */

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send_pkt_to_ppe - with head", 0, 1);

 #ifndef CONFIG_MIPS_UNCACHED
    /*  write back to physical memory   */
    dma_cache_wback((u32)skb->data, skb->len);
 #endif

    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    return 0;

ALLOC_SKB_TX_FAIL:
NO_FREE_DESC:
    dev_kfree_skb_any(skb); /* count these Tx errors */
    return -1;
}
#else
int ppe_directpath_send(uint32_t if_id, struct sk_buff *skb, int32_t len, uint32_t flags)
{
    dev_kfree_skb_any(skb); /* count these Tx errors */
    return -1;
}
#endif

int ppe_directpath_rx_stop(uint32_t if_id, uint32_t flags)
{
    return 0;
}

int ppe_directpath_rx_start(uint32_t if_id, uint32_t flags)
{
    return 0;
}

int ppe_clk_change(unsigned int arg, unsigned int flags)
{
    int i;

    qsb_global_set();

    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
        if ( (g_atm_priv_data.connection_table & (1 << i)) && g_atm_priv_data.connection[i].vcc != NULL )
            set_qsb(g_atm_priv_data.connection[i].vcc, &g_atm_priv_data.connection[i].vcc->qos, i);

    return 0;
}

u32 ephy_read_mdio_reg(int phy_addr, int phy_reg_num)
{
    return __ephy_read_mdio_reg(phy_addr, phy_reg_num);
}
EXPORT_SYMBOL(ephy_read_mdio_reg);

int ephy_write_mdio_reg(int phy_addr, int phy_reg_num, u32 phy_data)
{
    return __ephy_write_mdio_reg(phy_addr, phy_reg_num, phy_data);
}
EXPORT_SYMBOL(ephy_write_mdio_reg);

int ephy_auto_negotiate(int phy_addr)
{
    return __ephy_auto_negotiate(phy_addr);
}
EXPORT_SYMBOL(ephy_auto_negotiate);



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

static int __init ppe_eth_init(void)
{
    int ret;
    int i;
    char buf[512];
#ifdef CONFIG_IFX_ETH_FRAMEWORK
    struct ifx_eth_fw_netdev_ops eth_ops = {
        .get_stats      = eth_get_stats,
        .open           = eth_open,
        .stop           = eth_stop,
        .start_xmit     = eth_hard_start_xmit,
        .set_mac_address= eth_set_mac_address,
        .do_ioctl       = eth_ioctl,
        .tx_timeout     = eth_tx_timeout,
        .watchdog_timeo = ETH_WATCHDOG_TIMEOUT,
    };
    struct eth_priv_data *priv;
#endif

    printk("Loading A4 (ATM+MII0) driver ...... ");

#if IFX_PPA_DP_DBG_PARAM_ENABLE
    if( ifx_ppa_drv_dp_dbg_param_enable == 1 )
    {
        ethwan = ifx_ppa_drv_dp_dbg_param_ethwan;
    }
#endif  //IFX_PPA_DP_DBG_PARAM_ENABLE

    init_local_variables();

    init_hw();

    init_communication_data_structures(g_fwcode);

    ret = alloc_dma();
    if ( ret )
        goto ALLOC_DMA_FAIL;

    ppa_hook_mpoa_setup = mpoa_setup;

    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        g_atm_priv_data.port[i].dev = atm_dev_register("ifx_atm", &g_ppe_atm_ops, -1, 0UL);
        if ( !g_atm_priv_data.port[i].dev )
        {
            ret = -EIO;
            goto ATM_DEV_REGISTER_FAIL;
        }
        else
        {
            g_atm_priv_data.port[i].dev->ci_range.vpi_bits = 8;
            g_atm_priv_data.port[i].dev->ci_range.vci_bits = 16;
            g_atm_priv_data.port[i].dev->link_rate = g_atm_priv_data.port[i].tx_max_cell_rate;
            g_atm_priv_data.port[i].dev->dev_data = (void*)i;
        }
    }

    g_eth_net_dev[0] = ifx_eth_fw_alloc_netdev(sizeof(struct eth_priv_data), "eth0", &eth_ops);
    if ( g_eth_net_dev[0] == NULL )
    {
        ret = -ENOMEM;
        goto ALLOC_NETDEV_FAIL;
    }
    eth_setup(g_eth_net_dev[0]);
    ret = ifx_eth_fw_register_netdev(g_eth_net_dev[0]);
    if ( ret )
        goto REGISTER_NETDEV_FAIL;
#ifdef CONFIG_IFX_ETH_FRAMEWORK
    priv = ifx_eth_fw_netdev_priv(g_eth_net_dev[0]);
    priv->rx_packets = priv->stats.rx_packets;
    priv->rx_bytes   = priv->stats.rx_bytes;
    priv->rx_dropped = priv->stats.rx_dropped;
    priv->tx_packets = priv->stats.tx_packets;
    priv->tx_bytes   = priv->stats.tx_bytes;
    priv->tx_errors  = priv->stats.tx_errors;
    priv->tx_dropped = priv->stats.tx_dropped;
#endif

    ret = request_irq(PPE_MAILBOX_IGU1_INT, mailbox_irq_handler, IRQF_DISABLED, "a4_mailbox_isr", NULL);
    if ( ret )
    {
        if ( ret == -EBUSY )
            err("IRQ may be occupied by other PPE driver, please reconfig to disable it.\n");
        goto REQUEST_IRQ_FAIL;
    }

    /*
     *  init variable for directpath
     */
    memset(g_ppe_directpath_data, 0, sizeof(g_ppe_directpath_data));

    dump_init();

    pp32_start(g_fwcode);
    if ( ret )
        goto PP32_START_FAIL;

    *MBOX_IGU1_IER = g_mailbox_signal_mask;

    qsb_global_set();
    validate_oam_htu_entry();

    start_etop();

    //  init timer for Data Led
    setup_timer(&g_dsl_led_polling_timer, dsl_led_polling, 0);
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK && defined(CONFIG_IFX_LED)
    ifx_led_trigger_register("dsl_data", &g_data_led_trigger);
#endif

    /*  create proc file    */
    proc_file_create();

    ifx_mei_atm_showtime_enter = atm_showtime_enter;
    ifx_mei_atm_showtime_exit  = atm_showtime_exit;

    //Fix warning message ---start
    ifx_ppa_drv_get_dslwan_qid_with_vcc_hook = get_dslwan_qid_with_vcc;
    ifx_ppa_drv_get_netif_qid_with_pkt_hook = get_netif_qid_with_pkt;
    ifx_ppa_drv_ppe_clk_change_hook = ppe_clk_change;


#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
    ifx_ppa_drv_g_ppe_directpath_data = g_ppe_directpath_data;
    ifx_ppa_drv_directpath_send_hook = ppe_directpath_send;
    ifx_ppa_drv_directpath_rx_start_hook = ppe_directpath_rx_start;
    ifx_ppa_drv_directpath_rx_stop_hook = ppe_directpath_rx_stop;
#endif //CONFIG_IFX_PPA_API_DIRECTPATH
    ifx_ppa_drv_datapath_generic_hook = ppa_datapath_generic_hook;
    //Fix warning message ---end

    printk("Succeeded!\n");
    print_driver_ver(buf, sizeof(buf), "PPE datapath driver info", VER_FAMILY, VER_DRTYPE, VER_INTERFACE, VER_ACCMODE, VER_MAJOR, VER_MID, VER_MINOR);
    printk(buf);
    print_fw_ver(buf, sizeof(buf));
    printk(buf);

    return 0;

PP32_START_FAIL:
    free_irq(PPE_MAILBOX_IGU1_INT, NULL);
REQUEST_IRQ_FAIL:
    ifx_eth_fw_unregister_netdev(g_eth_net_dev[0], 0);
REGISTER_NETDEV_FAIL:
    ifx_eth_fw_free_netdev(g_eth_net_dev[0], 0);
    g_eth_net_dev[0] = NULL;
ALLOC_NETDEV_FAIL:
ATM_DEV_REGISTER_FAIL:
    while ( i-- )
        atm_dev_deregister(g_atm_priv_data.port[i].dev);
    ppa_hook_mpoa_setup = NULL;
    free_dma();
ALLOC_DMA_FAIL:
    clear_local_variables();
    return ret;
}

static void __exit ppe_eth_exit(void)
{
    int i;

   //Fix warning message ---start
    ifx_ppa_drv_get_dslwan_qid_with_vcc_hook = NULL;
    ifx_ppa_drv_get_netif_qid_with_pkt_hook = NULL;
    ifx_ppa_drv_ppe_clk_change_hook = NULL;

#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
    ifx_ppa_drv_g_ppe_directpath_data = NULL;
    ifx_ppa_drv_directpath_send_hook = NULL;
    ifx_ppa_drv_directpath_rx_start_hook = NULL;
    ifx_ppa_drv_directpath_rx_stop_hook = NULL;
#endif //CONFIG_IFX_PPA_API_DIRECTPATH
   ifx_ppa_drv_datapath_generic_hook = NULL;
    //Fix warning message ---end

    ifx_mei_atm_showtime_enter = NULL;
    ifx_mei_atm_showtime_exit  = NULL;

    proc_file_delete();

#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK && defined(CONFIG_IFX_LED)
    ifx_led_trigger_deregister(g_data_led_trigger);
    g_data_led_trigger = NULL;
#endif

    stop_datapath();

    invalidate_oam_htu_entry();

    pp32_stop();

    free_irq(PPE_MAILBOX_IGU1_INT, NULL);

    ifx_eth_fw_unregister_netdev(g_eth_net_dev[0], 0);
    ifx_eth_fw_free_netdev(g_eth_net_dev[0], 0);
    g_eth_net_dev[0] = NULL;

    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        atm_dev_deregister(g_atm_priv_data.port[i].dev);

    ppa_hook_mpoa_setup = NULL;

    free_dma();

    clear_local_variables();
}

#if defined(CONFIG_IFX_PPA_DATAPATH)
static int __init eth0addr_setup(char *line)
{
    ethaddr_setup(0, line);

    return 0;
}

static int __init wan_mode_setup(char *line)
{
    if ( strcmp(line, "1") == 0 )
        ethwan = 1;

    return 0;
}
#endif



module_init(ppe_eth_init);
module_exit(ppe_eth_exit);
#if defined(CONFIG_IFX_PPA_DATAPATH)
  __setup("ethaddr=", eth0addr_setup);
  __setup("ethwan=", wan_mode_setup);
#endif



MODULE_LICENSE("GPL");
