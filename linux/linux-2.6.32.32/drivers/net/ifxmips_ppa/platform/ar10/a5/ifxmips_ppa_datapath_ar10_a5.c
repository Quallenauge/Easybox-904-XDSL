/******************************************************************************
**
** FILE NAME    : ifxmips_ppa_datapath_ar10_a5.c
** PROJECT      : UEIP
** MODULES      : ATM + MII0/1 Acceleration Package (AR10 PPA A5)
**
** DATE         : 23 MAR 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM + MII0/1 Driver with Acceleration Firmware (A5)
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
** 23 MAR 2008  Xu Liang        Initiate Version
*******************************************************************************/
/*
 * ####################################
 *      Version No.
 * ####################################
 */
#define VER_FAMILY      0x80    //  bit 0: res
                                //      1: Danube
                                //      2: Twinpass
                                //      3: Amazon-SE
                                //      4: res
                                //      5: AR9
                                //      6: VR9
                                //      7: AR10
#define VER_DRTYPE      0x03    //  bit 0: Normal Data Path driver
                                //      1: Indirect-Fast Path driver
                                //      2: HAL driver
                                //      3: Hook driver
                                //      4: Stack/System Adaption Layer driver
                                //      5: PPA API driver
#define VER_INTERFACE   0x07    //      bit 0: MII 0
                                //      1: MII 1
                                //      2: ATM WAN
                                //      3: PTM WAN
#define VER_ACCMODE     0x00    //  bit 0: Routing
                                //      1: Bridging
#define VER_MAJOR       0
#define VER_MID         0
#define VER_MINOR       4

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
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/atmdev.h>
#include <linux/init.h>
#include <linux/etherdevice.h>  /*  eth_type_trans  */
#include <linux/ethtool.h>      /*  ethtool_cmd     */
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/errno.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_rcu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_led.h>
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_clk.h>
#include <switch_api/ifx_ethsw_kernel_api.h>
#include <switch_api/ifx_ethsw_api.h>
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_stack_al.h>
#include <net/ifx_ppa_api_directpath.h>
#include "../../ifx_ppa_datapath.h"
#include "ifxmips_ppa_datapath_fw_ar10_a5.h"
#include "ifxmips_ppa_hal_ar10_a5.h"
#include "ifxmips_ppa_proc_ar10_a5.h"
#include "ifxmips_ppa_proc_ar10_a5.c"

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
static int wanitf = ~0;

static int ipv6_acc_en = 1;

static int wanqos_en = 0;

#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_SIZE)
  static int directpath_tx_queue_size = 1524 * 50;
#endif
#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_PKTS)
  static int directpath_tx_queue_pkts = 50;
#endif

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

MODULE_PARM(write_descriptor_delay, "i");
MODULE_PARM_DESC(write_descriptor_delay, "PPE core clock cycles between descriptor write and effectiveness in external RAM");

MODULE_PARM(aal5r_max_packet_size, "i");
MODULE_PARM_DESC(aal5r_max_packet_size, "Max packet size in byte for downstream AAL5 frames");
MODULE_PARM(aal5r_min_packet_size, "i");
MODULE_PARM_DESC(aal5r_min_packet_size, "Min packet size in byte for downstream AAL5 frames");
MODULE_PARM(aal5s_max_packet_size, "i");
MODULE_PARM_DESC(aal5s_max_packet_size, "Max packet size in byte for upstream AAL5 frames");

MODULE_PARM(ethwan, "i");
MODULE_PARM_DESC(ethwan, "WAN mode, 2 - ETH WAN, 1 - ETH mixed, 0 - DSL WAN.");

MODULE_PARM(wanitf, "i");
MODULE_PARM_DESC(wanitf, "WAN interfaces, bit 0 - ETH0, 1 - ETH1, 2 - reserved for CPU0, 3/4/5 - DirectPath, 6/7 - DSL");

MODULE_PARM(ipv6_acc_en, "i");
MODULE_PARM_DESC(ipv6_acc_en, "IPv6 support, 1 - enabled, 0 - disabled.");

MODULE_PARM(wanqos_en, "i");
MODULE_PARM_DESC(wanqos_en, "WAN QoS support, 1 - enabled, 0 - disabled.");

#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_SIZE)
  MODULE_PARM(directpath_tx_queue_size, "i");
  MODULE_PARM_DESC(directpath_tx_queue_size, "directpath TX queue size in total buffer size");
#endif
#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_PKTS)
  MODULE_PARM(directpath_tx_queue_pkts, "i");
  MODULE_PARM_DESC(directpath_tx_queue_pkts, "directpath TX queue size in total packet number");
#endif



/*
 * ####################################
 *              Board Type
 * ####################################
 */

#define BOARD_AR10_REFERENCE                     0x01


/* ###################################
  *             emulation
  * ###################################
  */

//ENABLE_LED_FRAMEWORK 0

/*
 * ####################################
 *              Definition
 * ####################################
 */

#define BOARD_CONFIG                            BOARD_AR10_REFERENCE

#define DEBUG_SKB_SWAP                          0

#define ENABLE_RX_PROFILE                       0

#define ENABLE_LED_FRAMEWORK                    0

#define ENABLE_NO_DELAY_ATM_SKB_FREE            1

#define ENABLE_CONFIGURABLE_DSL_VLAN            1

#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_SIZE)
  #define ENABLE_DIRECTPATH_TX_QUEUE            1
  #define ENABLE_DIRECTPATH_TX_QUEUE_SIZE       directpath_tx_queue_size
#elif defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_PKTS)
  #define ENABLE_DIRECTPATH_TX_QUEUE            1
  #define ENABLE_DIRECTPATH_TX_QUEUE_PKTS       directpath_tx_queue_pkts
#endif

#define ENABLE_SWITCH_FLOW_CONTROL              1

#define ENABLE_STATS_ON_VCC_BASIS               1

#define ENABLE_DSL_LOOPBACK_TEST                0   //  driver level loopback

#define ENABLE_DFE_LOOPBACK_TEST                1   //  DSL firmware level loopback

#define DISABLE_VBR                             0

#define ENABLE_MY_MEMCPY                        0

#define ENABLE_FW_MODULE_TEST                   0

#define ENABLE_DEBUG                            1

#define ENABLE_ASSERT                           1

#define DEBUG_QOS                               1

#define DEBUG_DUMP_SKB                          1

#define DEBUG_DUMP_FLAG_HEADER                  1

#define DEBUG_DUMP_INIT                         0

#define DEBUG_FIRMWARE_TABLES_PROC              1

#define DEBUG_HTU_PROC                          1

#define DEBUG_MEM_PROC                          1

#define DEBUG_PP32_PROC                         1

#define DEBUG_FW_PROC                           1

#define DEBUG_SEND_PROC                         1

#define DEBUG_MIRROR_PROC                       1

#define PPE_MAILBOX_IGU1_INT                    INT_NUM_IM2_IRL24

#define MY_ETH0_ADDR                            g_my_ethaddr

#if defined(CONFIG_DSL_MEI_CPE_DRV) && !defined(CONFIG_IFXMIPS_DSL_CPE_MEI)
  #define CONFIG_IFXMIPS_DSL_CPE_MEI            1
#endif
#define AR10_EMULATION                          1

//  specific board related configuration
#if defined (BOARD_CONFIG)
  #if BOARD_CONFIG == BOARD_AR10_REFERENCE
  #endif
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
//#define DMA_TX_CH2_SIG                          (1 << 31)
#define CPU_TO_ETH_WAN_TX_SIG                   (1 << 17)
#define CPU_TO_DSL_WAN_TX_SIG                   (1 << 16)
#define CPU_TO_WAN_SWAP_SIG                     (1 << 16)   //  ETH WAN QoS Mode only
#define WAN_RX_SIG(i)                           (1 << (i))  //  0: AAL5, 1: OAM

/*
 *  Eth Mode
 */
#define RGMII_MODE                              0
#define MII_MODE                                1
#define REV_MII_MODE                            2
#define RED_MII_MODE_IC                         3   //  Input clock
#define RGMII_MODE_100MB                        4
#define TURBO_REV_MII_MODE                      6   //  Turbo Rev Mii mode
#define RED_MII_MODE_OC                         7   //  Output clock
#define RGMII_MODE_10MB                         8

/*
 *  GPIO Related
 */
#define IFX_PPA_GPIO_OP(op, pin)                op((pin), IFX_GPIO_MODULE_PPA)
#define IFX_SWITCH_PIN_RESERVE(pin)             IFX_PPA_GPIO_OP(ifx_gpio_pin_reserve, pin)
#define IFX_SWITCH_DIR_OUT(pin)                 IFX_PPA_GPIO_OP(ifx_gpio_dir_out_set, pin)
#define IFX_SWITCH_DIR_IN(pin)                  IFX_PPA_GPIO_OP(ifx_gpio_dir_in_set, pin)
#define IFX_SWITCH_OUTPUT_SET(pin)              IFX_PPA_GPIO_OP(ifx_gpio_output_set, pin)
#define IFX_SWITCH_OUTPUT_CLR(pin)              IFX_PPA_GPIO_OP(ifx_gpio_output_clear, pin)
#define IFX_SWITCH_ALTSEL0_SET(pin)             IFX_PPA_GPIO_OP(ifx_gpio_altsel0_set, pin)
#define IFX_SWITCH_ALTSEL0_CLR(pin)             IFX_PPA_GPIO_OP(ifx_gpio_altsel0_clear, pin)
#define IFX_SWITCH_OD_SET(pin)                  IFX_PPA_GPIO_OP(ifx_gpio_open_drain_set, pin)
#define IFX_SWITCH_OD_CLR(pin)                  IFX_PPA_GPIO_OP(ifx_gpio_open_drain_clear, pin)
#define IFX_SWITCH_ALTSEL1_SET(pin)             IFX_PPA_GPIO_OP(ifx_gpio_altsel1_set, pin)
#define IFX_SWITCH_ALTSEL1_CLR(pin)             IFX_PPA_GPIO_OP(ifx_gpio_altsel1_clear, pin)
#define IFX_SWITCH_PUDSEL_SET(pin)              IFX_PPA_GPIO_OP(ifx_gpio_pudsel_set, pin)
#define IFX_SWITCH_PUDEN_SET(pin)               IFX_PPA_GPIO_OP(ifx_gpio_puden_set, pin)
#define SWITCH_MDIO                             42  //  P2.10
#define SWITCH_MDC                              43  //  P2.11
#define MII0_COL                                32  //  P2.0
#define MII0_CRS                                33  //  P2.1
#define MII0_TXERR                              40  //  P2.8
#define MII0_RXERR                              41  //  P2.9
#define MII1_COL                                44  //  P2.12
#define MII1_CRS                                47  //  P2.15
#define MII1_TXERR                              45  //  P2.13
#define MII1_RXERR                              46  //  P2.14
#define CLOCK_OUT2                              3   //  P0.3
#define RESET_GPHY                              32  //  P2.0

/*
 *  Constant Definition
 */
#define ETH_WATCHDOG_TIMEOUT                    (10 * HZ)
#define ETOP_MDIO_DELAY                         1
#define IDLE_CYCLE_NUMBER                       30000

#define DMA_PACKET_SIZE                         1568
#define DMA_ALIGNMENT                           32

#define FWCODE_ROUTING_ACC_D2                   0x02
#define FWCODE_BRIDGING_ACC_D3                  0x03
#define FWCODE_ROUTING_BRIDGING_ACC_D4          0x04
#define FWCODE_ROUTING_BRIDGING_ACC_A4          0x14
#define FWCODE_ROUTING_ACC_D5                   0x05
#define FWCODE_ROUTING_ACC_A5                   0x15

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
#define ATM_PORT_NUMBER                         2
#define ATM_QUEUE_NUMBER                        15
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
#define EMA_CMD_BASE_ADDR                       (0x1710 << 2)
#define EMA_DATA_BUF_LEN                        0x0040
#define EMA_DATA_BASE_ADDR                      (0x16d0 << 2)
#define EMA_WRITE_BURST                         0x02
#define EMA_READ_BURST                          0x02

/*
 *  Firmware Settings
 */

#define CPU_TO_WAN_TX_DESC_NUM                  32  //  WAN CPU TX
#define DSL_WAN_TX_DESC_NUM(i)                  32  //  WAN QoS TX, i < 15
#define ETH_WAN_TX_DESC_NUM(i)                  __ETH_WAN_TX_QUEUE_LEN  //  ETH WAN QoS TX, i < __ETH_WAN_TX_QUEUE_NUM
#define CPU_TO_WAN_SWAP_DESC_NUM                32
#define WAN_TX_DESC_NUM_TOTAL                   (DSL_WAN_TX_DESC_NUM(0) * 15)   //  DSL WAN TX desc overlap ETH WAN TX desc + ETH WAN Swap desc
#define WAN_RX_DESC_NUM(i)                      32  //  DSL RX PKT/OAM, i < 2, 0 PKT, 1 OAM
#define DMA_TX_CH1_DESC_NUM                     (g_eth_wan_mode ? (g_eth_wan_mode == 3 /* ETH WAN */ && g_wanqos_en ? 32 : DMA_RX_CH1_DESC_NUM) : WAN_RX_DESC_NUM(0))
#define DMA_RX_CH1_DESC_NUM                     32  //  DSL/MII1 WAN mode fast path
#define DMA_RX_CH2_DESC_NUM                     32  //  Switch/MII0/MII1 LAN mode fast path
#define DMA_TX_CH0_DESC_NUM                     DMA_RX_CH2_DESC_NUM //  DMA_TX_CH0_DESC_NUM is used to replace DMA_TX_CH2_DESC_NUM

#define IFX_PPA_PORT_NUM                        8

/*
 *  Bits Operation
 */
//#define GET_BITS(x, msb, lsb)                   (((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
//#define SET_BITS(x, msb, lsb, value)            (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

/*
 *  External Tantos Switch Related
 */
#define AR10_SWIP_MACRO                          (KSEG1 | 0x1E108000)
#define AR10_SWIP_MACRO_REG(off)                 ((volatile u32*)(AR10_SWIP_MACRO + (off) * 4))
#define AR10_SWIP_TOP                            (AR10_SWIP_MACRO | (0x0C40 * 4))
#define AR10_SWIP_TOP_REG(off)                   ((volatile u32*)(AR10_SWIP_TOP + (off) * 4))
#define ETHSW_SWRES_REG                         AR10_SWIP_MACRO_REG(0x00)
#define ETHSW_CLK_REG                           AR10_SWIP_MACRO_REG(0x01)
#define ETHSW_BM_RAM_VAL_3_REG                  AR10_SWIP_MACRO_REG(0x40)
#define ETHSW_BM_RAM_VAL_2_REG                  AR10_SWIP_MACRO_REG(0x41)
#define ETHSW_BM_RAM_VAL_1_REG                  AR10_SWIP_MACRO_REG(0x42)
#define ETHSW_BM_RAM_VAL_0_REG                  AR10_SWIP_MACRO_REG(0x43)
#define ETHSW_BM_RAM_ADDR_REG                   AR10_SWIP_MACRO_REG(0x44)
#define ETHSW_BM_RAM_CTRL_REG                   AR10_SWIP_MACRO_REG(0x45)
//  Buffer manager per port registrs
#define ETHSW_BM_PCFG_REG(port)                 AR10_SWIP_MACRO_REG(0x80 + (port) * 2)   //  port < 7
#define ETHSW_BM_RMON_CTRL_REG(port)            AR10_SWIP_MACRO_REG(0x81 + (port) * 2)   //  port < 7
#define PCE_PMAP_REG(reg)                       AR10_SWIP_MACRO_REG(0x453 + (reg) - 1)   //  1 <= reg <= 3
//  Parser & Classification Engine
#define PCE_TBL_KEY(n)                          AR10_SWIP_MACRO_REG(0x440 + 7 - (n))                 //  n < 7
#define PCE_TBL_MASK                            AR10_SWIP_MACRO_REG(0x448)
#define PCE_TBL_VAL(n)                          AR10_SWIP_MACRO_REG(0x449 + 4 - (n))                 //  n < 4;
#define PCE_TBL_ADDR                            AR10_SWIP_MACRO_REG(0x44E)
#define PCE_TBL_CTRL                            AR10_SWIP_MACRO_REG(0x44F)
#define PCE_TBL_STAT                            AR10_SWIP_MACRO_REG(0x450)
#define PCE_GCTRL_REG(reg)                      AR10_SWIP_MACRO_REG(0x456 + (reg))
#define PCE_PCTRL_REG(port, reg)                AR10_SWIP_MACRO_REG(0x480 + (port) * 0xA + (reg))    //  port < 12, reg < 4
//  MAC Frame Length Register
#define MAC_FLEN_REG                            AR10_SWIP_MACRO_REG(0x8C5)
//  MAC Port Status Register
#define MAC_PSTAT_REG(port)                     AR10_SWIP_MACRO_REG(0x900 + (port) * 0xC)//  port < 7
//  MAC Control Register 0
#define MAC_CTRL_REG(port, reg)                 AR10_SWIP_MACRO_REG(0x903 + (port) * 0xC + (reg))    //  port < 7, reg < 7
//  Ethernet Switch Fetch DMA Port Control, Controls per-port functions of the Fetch DMA
#define FDMA_PCTRL_REG(port)                    AR10_SWIP_MACRO_REG(0xA80 + (port) * 6)  //  port < 7
//  Ethernet Switch Store DMA Port Control, Controls per-ingress-port functions of the Store DMA
#define SDMA_PCTRL_REG(port)                    AR10_SWIP_MACRO_REG(0xBC0 + (port) * 6)  //  port < 7
//  Global Control Register 0
#define GLOB_CTRL_REG                           AR10_SWIP_TOP_REG(0x00)
//  MDIO Control Register
#define MDIO_CTRL_REG                           AR10_SWIP_TOP_REG(0x08)
//  MDIO Read Data Register
#define MDIO_READ_REG                           AR10_SWIP_TOP_REG(0x09)
//  MDIO Write Data Register
#define MDIO_WRITE_REG                          AR10_SWIP_TOP_REG(0x0A)
//  MDC Clock Configuration Register 0
#define MDC_CFG_0_REG                           AR10_SWIP_TOP_REG(0x0B)
//  MDC Clock Configuration Register 1
#define MDC_CFG_1_REG                           AR10_SWIP_TOP_REG(0x0C)
//  PHY Address Register PORT 5~0
#define PHY_ADDR_REG(port)                      AR10_SWIP_TOP_REG(0x15 - (port))     //  port < 6
//  PHY MDIO Polling Status per PORT
#define MDIO_STAT_REG(port)                     AR10_SWIP_TOP_REG(0x16 + (port))     //  port < 6
//  xMII Control Registers
//  xMII Port 0 Configuration register
#define MII_CFG_REG(port)                       AR10_SWIP_TOP_REG(0x36 + (port) * 2) //  port < 6
//  Configuration of Clock Delay for Port 0 (used for RGMII mode only)
#define MII_PCDU_REG(port)                      AR10_SWIP_TOP_REG(0x37 + (port) * 2) //  port < 6
//  PMAC Header Control Register
#define PMAC_HD_CTL_REG                         AR10_SWIP_TOP_REG(0x82)
//  PMAC Type/Length register
#define PMAC_TL_REG                             AR10_SWIP_TOP_REG(0x83)
//  PMAC Source Address Register
#define PMAC_SA1_REG                            AR10_SWIP_TOP_REG(0x84)
#define PMAC_SA2_REG                            AR10_SWIP_TOP_REG(0x85)
#define PMAC_SA3_REG                            AR10_SWIP_TOP_REG(0x86)
//  PMAC Destination Address Register
#define PMAC_DA1_REG                            AR10_SWIP_TOP_REG(0x87)
#define PMAC_DA2_REG                            AR10_SWIP_TOP_REG(0x88)
#define PMAC_DA3_REG                            AR10_SWIP_TOP_REG(0x89)
//  PMAC VLAN register
#define PMAC_VLAN_REG                           AR10_SWIP_TOP_REG(0x8A)
//  PMAC Inter Packet Gap in RX Direction
#define PMAC_RX_IPG_REG                         AR10_SWIP_TOP_REG(0x8B)
//  PMAC Special Tag Ethertype
#define PMAC_ST_ETYPE_REG                       AR10_SWIP_TOP_REG(0x8C)
//  PMAC Ethernet WAN Group
#define PMAC_EWAN_REG                           AR10_SWIP_TOP_REG(0x8D)

/*
 *  DWORD-Length of Memory Blocks
 */
#define PP32_DEBUG_REG_DWLEN                    0x0030
#define PPM_INT_REG_DWLEN                       0x0010
#define PP32_INTERNAL_RES_DWLEN                 0x00C0
#define PPE_CLOCK_CONTROL_DWLEN                 0x0F00
#define CDM_CODE_MEMORY_RAM0_DWLEN              0x1000
#define CDM_CODE_MEMORY_RAM1_DWLEN              0x1000
#define PPE_REG_DWLEN                           0x1000
#define PP32_DATA_MEMORY_RAM1_DWLEN             CDM_CODE_MEMORY_RAM1_DWLEN
#define PPM_INT_UNIT_DWLEN                      0x0100
#define PPM_TIMER0_DWLEN                        0x0100
#define PPM_TASK_IND_REG_DWLEN                  0x0100
#define PPS_BRK_DWLEN                           0x0100
#define PPM_TIMER1_DWLEN                        0x0100
#define SB_RAM0_DWLEN                           0x0800
#define SB_RAM1_DWLEN                           0x0800
#define SB_RAM2_DWLEN                           0x0800
#define SB_RAM3_DWLEN                           0x0800
#define SB_RAM4_DWLEN                           0x0C00
#define QSB_CONF_REG_DWLEN                      0x0100


/*
 *  DMA/EMA Descriptor Base Address
 */
#define CPU_TO_WAN_TX_DESC_BASE                 ((volatile struct tx_descriptor *)SB_BUFFER(0x3C00))                /*         32 each queue    */
#define DSL_WAN_TX_DESC_BASE(i)                 ((volatile struct tx_descriptor *)SB_BUFFER(0x3C40 + (i) * 2 * 32)) /*  i < 15,32 each queue    */
#define __ETH_WAN_TX_QUEUE_NUM                  g_wanqos_en
#define __ETH_WAN_TX_QUEUE_LEN                  (448 / __ETH_WAN_TX_QUEUE_NUM < 256 ? (448 / __ETH_WAN_TX_QUEUE_NUM) : 255)
#define __ETH_WAN_TX_DESC_BASE(i)               (0x3C80 + (i) * 2 * __ETH_WAN_TX_QUEUE_LEN)
#define ETH_WAN_TX_DESC_BASE(i)                 ((volatile struct tx_descriptor *)SB_BUFFER(__ETH_WAN_TX_DESC_BASE(i))) /*  i < __ETH_WAN_TX_QUEUE_NUM, __ETH_WAN_TX_QUEUE_LEN each queue    */
#define CPU_TO_WAN_SWAP_DESC_BASE               ((volatile struct tx_descriptor *)SB_BUFFER(0x3C40))                /*         32 each queue    */
#define WAN_RX_DESC_BASE(i)                     ((volatile struct rx_descriptor *)SB_BUFFER(0x3B80 + (i) * 2 * 32)) /*  i < 2, 32 each queue    */
#define DMA_TX_CH1_DESC_BASE                    (g_eth_wan_mode ? (g_eth_wan_mode == 3 /* ETH WAN */ && g_wanqos_en ? ((volatile struct rx_descriptor *)SB_BUFFER(0x3AC0)): DMA_RX_CH1_DESC_BASE) : WAN_RX_DESC_BASE(0))
#define DMA_RX_CH1_DESC_BASE                    ((volatile struct rx_descriptor *)SB_BUFFER(0x3B40))                /*         32 each queue    */
#define DMA_RX_CH2_DESC_BASE                    ((volatile struct rx_descriptor *)SB_BUFFER(0x3B00))                /*         32 each queue    */
#define DMA_TX_CH0_DESC_BASE                    DMA_RX_CH2_DESC_BASE

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
/*
 *  Firmware Proc
 */
//  need check with hejun
  #define TIR(i)                                ((volatile u32 *)(0xBE198800) + (i))

  #define __CPU_TX_SWAPPER_DROP_PKTS            SB_BUFFER(0x32C0)
  #define __CPU_TX_SWAPPER_DROP_BYTES           SB_BUFFER(0x32C1)
  #define __DSLWAN_FP_SWAPPER_DROP_PKTS         SB_BUFFER(0x32C2)
  #define __DSLWAN_FP_SWAPPER_DROP_BYTES        SB_BUFFER(0x32C3)

  #define __CPU_TXDES_SWAP_RDPTR                SB_BUFFER(0x32C4)
  #define __DSLWAN_FP_RXDES_SWAP_RDPTR          SB_BUFFER(0x32C5)
  #define __DSLWAN_FP_RXDES_DPLUS_WRPTR         SB_BUFFER(0x32C6)

  #define __DSLWAN_TX_PKT_CNT(i)                SB_BUFFER(0x32D0+(i))   //  i < 8

  #define __DSLWAN_TXDES_SWAP_PTR(i)            SB_BUFFER(0x32E0+(i))   //  i < 8

#if 0
  // SB_BUFFER(0x29C0) - SB_BUFFER(0x29C7)
  #define __VLAN_PRI_TO_QID_MAPPING             SB_BUFFER(0x32E0)
#endif

  //
  // etx MIB: SB_BUFFER(0x32F0) - SB_BUFFER(0x32FF)
  #define __DSLWAN_TX_THRES_DROP_PKT_CNT(i)     SB_BUFFER(0x2D80+(i))   //  i < 8

  #define __CPU_TO_DSLWAN_TX_PKTS               SB_BUFFER(0x32C8)
  #define __CPU_TO_DSLWAN_TX_BYTES              SB_BUFFER(0x32C9)

  #define ACC_ERX_PID                           SB_BUFFER(0x2B00)
  #define ACC_ERX_PORT_TIMES                    SB_BUFFER(0x2B01)
  #define SLL_ISSUED                            SB_BUFFER(0x2B02)
  #define BMC_ISSUED                            SB_BUFFER(0x2B03)
  #define DPLUS_RX_ON                           SB_BUFFER(0x3283)
  #define ISR_IS                                SB_BUFFER(0x3284)

  #define __ENETS_PGNUM                         20
  #define __ERX_CBUF_BASE                       SB_BUFFER(0x343C)
  #define __ERX_DBUF_BASE                       SB_BUFFER(0x3450)

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

  #define PRE_DPLUS_PTR                         SB_BUFFER(0x3281)
  #define DPLUS_PTR                             SB_BUFFER(0x3282)
  #define DPLUS_CNT                             SB_BUFFER(0x3280)
#endif

/*
 *  Share Buffer Registers
 */
#define SB_MST_PRI0                             PPE_REG_ADDR(0x0300)
#define SB_MST_PRI1                             PPE_REG_ADDR(0x0301)
//#define SB_MST_SEL                              PPE_REG_ADDR(0x0304)

/*
 *    ETOP MDIO Registers
 */
//#define ETOP_MDIO_CFG                           PPE_REG_ADDR(0x0600)
//#define ETOP_MDIO_ACC                           PPE_REG_ADDR(0x0601)
//#define ETOP_CFG                                PPE_REG_ADDR(0x0602)
//#define ETOP_IG_VLAN_COS                        PPE_REG_ADDR(0x0603)
//#define ETOP_IG_DSCP_COSx(x)                    PPE_REG_ADDR(0x0607 - ((x) & 0x03))
//#define ETOP_IG_PLEN_CTRL0                      PPE_REG_ADDR(0x0608)
//#define ETOP_ISR                                PPE_REG_ADDR(0x060A)
//#define ETOP_IER                                PPE_REG_ADDR(0x060B)
//#define ETOP_VPID                               PPE_REG_ADDR(0x060C)
//#define ENET_MAC_CFG(i)                         PPE_REG_ADDR(0x0610 + ((i) ? 0x40 : 0x00))
//#define ENETS_DBA(i)                            PPE_REG_ADDR(0x0612 + ((i) ? 0x40 : 0x00))
//#define ENETS_CBA(i)                            PPE_REG_ADDR(0x0613 + ((i) ? 0x40 : 0x00))
//#define ENETS_CFG(i)                            PPE_REG_ADDR(0x0614 + ((i) ? 0x40 : 0x00))
//#define ENETS_PGCNT(i)                          PPE_REG_ADDR(0x0615 + ((i) ? 0x40 : 0x00))
//#define ENETS_PKTCNT(i)                         PPE_REG_ADDR(0x0616 + ((i) ? 0x40 : 0x00))
//#define ENETS_BUF_CTRL(i)                       PPE_REG_ADDR(0x0617 + ((i) ? 0x40 : 0x00))
//#define ENETS_COS_CFG(i)                        PPE_REG_ADDR(0x0618 + ((i) ? 0x40 : 0x00))
//#define ENETS_IGDROP(i)                         PPE_REG_ADDR(0x0619 + ((i) ? 0x40 : 0x00))
//#define ENETS_IGERR(i)                          PPE_REG_ADDR(0x061A + ((i) ? 0x40 : 0x00))
//#define ENETS_MAC_DA0(i)                        PPE_REG_ADDR(0x061B + ((i) ? 0x40 : 0x00))
//#define ENETS_MAC_DA1(i)                        PPE_REG_ADDR(0x061C + ((i) ? 0x40 : 0x00))
//#define ENETF_DBA(i)                            PPE_REG_ADDR(0x0630 + ((i) ? 0x40 : 0x00))
//#define ENETF_CBA(i)                            PPE_REG_ADDR(0x0631 + ((i) ? 0x40 : 0x00))
//#define ENETF_CFG(i)                            PPE_REG_ADDR(0x0632 + ((i) ? 0x40 : 0x00))
//#define ENETF_PGCNT(i)                          PPE_REG_ADDR(0x0633 + ((i) ? 0x40 : 0x00))
//#define ENETF_PKTCNT(i)                         PPE_REG_ADDR(0x0634 + ((i) ? 0x40 : 0x00))
//#define ENETF_HFCTRL(i)                         PPE_REG_ADDR(0x0635 + ((i) ? 0x40 : 0x00))
//#define ENETF_TXCTRL(i)                         PPE_REG_ADDR(0x0636 + ((i) ? 0x40 : 0x00))
//#define ENETF_VLCOS0(i)                         PPE_REG_ADDR(0x0638 + ((i) ? 0x40 : 0x00))
//#define ENETF_VLCOS1(i)                         PPE_REG_ADDR(0x0639 + ((i) ? 0x40 : 0x00))
//#define ENETF_VLCOS2(i)                         PPE_REG_ADDR(0x063A + ((i) ? 0x40 : 0x00))
//#define ENETF_VLCOS3(i)                         PPE_REG_ADDR(0x063B + ((i) ? 0x40 : 0x00))
//#define ENETF_EGCOL(i)                          PPE_REG_ADDR(0x063C + ((i) ? 0x40 : 0x00))
//#define ENETF_EGDROP(i)                         PPE_REG_ADDR(0x063D + ((i) ? 0x40 : 0x00))

/*
 *  DPlus Registers
 */
#define DM_RXDB                                 PPE_REG_ADDR(0x0612)
#define DM_RXCB                                 PPE_REG_ADDR(0x0613)
#define DM_RXCFG                                PPE_REG_ADDR(0x0614)
#define DM_RXPGCNT                              PPE_REG_ADDR(0x0615)
#define DM_RXPKTCNT                             PPE_REG_ADDR(0x0616)
#define DS_RXDB                                 PPE_REG_ADDR(0x0710)
#define DS_RXCB                                 PPE_REG_ADDR(0x0711)
#define DS_RXCFG                                PPE_REG_ADDR(0x0712)
#define DS_RXPGCNT                              PPE_REG_ADDR(0x0713)

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
#define PP32_DBG_CTRL                           PP32_DEBUG_REG_ADDR(0x0000)

#define DBG_CTRL_RESTART                        0
#define DBG_CTRL_STOP                           1

/*
 *  PP32 Registers
 */

//  Danube
//
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

// Amazon-S

#define PP32_CTRL_CMD                           PP32_DEBUG_REG_ADDR(0x0B00)
  #define PP32_CTRL_CMD_RESTART                 (1 << 0)
  #define PP32_CTRL_CMD_STOP                    (1 << 1)
  #define PP32_CTRL_CMD_STEP                    (1 << 2)
  #define PP32_CTRL_CMD_BREAKOUT                (1 << 3)

#define PP32_CTRL_OPT                           PP32_DEBUG_REG_ADDR(0x0C00)
  #define PP32_CTRL_OPT_BREAKOUT_ON_STOP_ON     (3 << 0)
  #define PP32_CTRL_OPT_BREAKOUT_ON_STOP_OFF    (2 << 0)
  #define PP32_CTRL_OPT_BREAKOUT_ON_BREAKIN_ON  (3 << 2)
  #define PP32_CTRL_OPT_BREAKOUT_ON_BREAKIN_OFF (2 << 2)
  #define PP32_CTRL_OPT_STOP_ON_BREAKIN_ON      (3 << 4)
  #define PP32_CTRL_OPT_STOP_ON_BREAKIN_OFF     (2 << 4)
  #define PP32_CTRL_OPT_STOP_ON_BREAKPOINT_ON   (3 << 6)
  #define PP32_CTRL_OPT_STOP_ON_BREAKPOINT_OFF  (2 << 6)
  #define PP32_CTRL_OPT_BREAKOUT_ON_STOP        (*PP32_CTRL_OPT & (1 << 0))
  #define PP32_CTRL_OPT_BREAKOUT_ON_BREAKIN     (*PP32_CTRL_OPT & (1 << 2))
  #define PP32_CTRL_OPT_STOP_ON_BREAKIN         (*PP32_CTRL_OPT & (1 << 4))
  #define PP32_CTRL_OPT_STOP_ON_BREAKPOINT      (*PP32_CTRL_OPT & (1 << 6))

#define PP32_BRK_PC(i)                          PP32_DEBUG_REG_ADDR(0x0900 + (i) * 2)
#define PP32_BRK_PC_MASK(i)                     PP32_DEBUG_REG_ADDR(0x0901 + (i) * 2)
#define PP32_BRK_DATA_ADDR(i)                   PP32_DEBUG_REG_ADDR(0x0904 + (i) * 2)
#define PP32_BRK_DATA_ADDR_MASK(i)              PP32_DEBUG_REG_ADDR(0x0905 + (i) * 2)
#define PP32_BRK_DATA_VALUE_RD(i)               PP32_DEBUG_REG_ADDR(0x0908 + (i) * 2)
#define PP32_BRK_DATA_VALUE_RD_MASK(i)          PP32_DEBUG_REG_ADDR(0x0909 + (i) * 2)
#define PP32_BRK_DATA_VALUE_WR(i)               PP32_DEBUG_REG_ADDR(0x090C + (i) * 2)
#define PP32_BRK_DATA_VALUE_WR_MASK(i)          PP32_DEBUG_REG_ADDR(0x090D + (i) * 2)
  #define PP32_BRK_CONTEXT_MASK(i)              (1 << (i))
  #define PP32_BRK_CONTEXT_MASK_EN              (1 << 4)
  #define PP32_BRK_COMPARE_GREATER_EQUAL        (1 << 5)    //  valid for break data value rd/wr only
  #define PP32_BRK_COMPARE_LOWER_EQUAL          (1 << 6)
  #define PP32_BRK_COMPARE_EN                   (1 << 7)

#define PP32_BRK_TRIG                           PP32_DEBUG_REG_ADDR(0x0F00)
  #define PP32_BRK_GRPi_PCn_ON(i, n)            ((3 << ((n) * 2)) << ((i) * 16))
  #define PP32_BRK_GRPi_PCn_OFF(i, n)           ((2 << ((n) * 2)) << ((i) * 16))
  #define PP32_BRK_GRPi_DATA_ADDRn_ON(i, n)     ((3 << ((n) * 2 + 4)) << ((i) * 16))
  #define PP32_BRK_GRPi_DATA_ADDRn_OFF(i, n)    ((2 << ((n) * 2 + 4)) << ((i) * 16))
  #define PP32_BRK_GRPi_DATA_VALUE_RDn_ON(i, n) ((3 << ((n) * 2 + 8)) << ((i) * 16))
  #define PP32_BRK_GRPi_DATA_VALUE_RDn_OFF(i, n)((2 << ((n) * 2 + 8)) << ((i) * 16))
  #define PP32_BRK_GRPi_DATA_VALUE_WRn_ON(i, n) ((3 << ((n) * 2 + 12)) << ((i) * 16))
  #define PP32_BRK_GRPi_DATA_VALUE_WRn_OFF(i, n)((2 << ((n) * 2 + 12)) << ((i) * 16))
  #define PP32_BRK_GRPi_PCn(i, n)               (*PP32_BRK_TRIG & ((1 << ((n))) << ((i) * 8)))
  #define PP32_BRK_GRPi_DATA_ADDRn(i, n)        (*PP32_BRK_TRIG & ((1 << ((n) + 2)) << ((i) * 8)))
  #define PP32_BRK_GRPi_DATA_VALUE_RDn(i, n)    (*PP32_BRK_TRIG & ((1 << ((n) + 4)) << ((i) * 8)))
  #define PP32_BRK_GRPi_DATA_VALUE_WRn(i, n)    (*PP32_BRK_TRIG & ((1 << ((n) + 6)) << ((i) * 8)))

#define PP32_CPU_STATUS                         PP32_DEBUG_REG_ADDR(0x0D00)
#define PP32_HALT_STAT                          PP32_CPU_STATUS
#define PP32_DBG_CUR_PC                         PP32_CPU_STATUS
  #define PP32_CPU_USER_STOPPED                 (*PP32_CPU_STATUS & (1 << 0))
  #define PP32_CPU_USER_BREAKIN_RCV             (*PP32_CPU_STATUS & (1 << 1))
  #define PP32_CPU_USER_BREAKPOINT_MET          (*PP32_CPU_STATUS & (1 << 2))
  #define PP32_CPU_CUR_PC                       (*PP32_CPU_STATUS >> 16)

#define PP32_BREAKPOINT_REASONS                 PP32_DEBUG_REG_ADDR(0x0A00)
  #define PP32_BRK_PC_MET(i)                    (*PP32_BREAKPOINT_REASONS & (1 << (i)))
  #define PP32_BRK_DATA_ADDR_MET(i)             (*PP32_BREAKPOINT_REASONS & (1 << ((i) + 2)))
  #define PP32_BRK_DATA_VALUE_RD_MET(i)         (*PP32_BREAKPOINT_REASONS & (1 << ((i) + 4)))
  #define PP32_BRK_DATA_VALUE_WR_MET(i)         (*PP32_BREAKPOINT_REASONS & (1 << ((i) + 6)))
  #define PP32_BRK_DATA_VALUE_RD_LO_EQ(i)       (*PP32_BREAKPOINT_REASONS & (1 << ((i) * 2 + 8)))
  #define PP32_BRK_DATA_VALUE_RD_GT_EQ(i)       (*PP32_BREAKPOINT_REASONS & (1 << ((i) * 2 + 9)))
  #define PP32_BRK_DATA_VALUE_WR_LO_EQ(i)       (*PP32_BREAKPOINT_REASONS & (1 << ((i) * 2 + 12)))
  #define PP32_BRK_DATA_VALUE_WR_GT_EQ(i)       (*PP32_BREAKPOINT_REASONS & (1 << ((i) * 2 + 13)))
  #define PP32_BRK_CUR_CONTEXT                  ((*PP32_BREAKPOINT_REASONS >> 16) & 0x03)

#define PP32_GP_REG_BASE                        PP32_DEBUG_REG_ADDR(0x0E00)
#define PP32_GP_CONTEXTi_REGn(i, n)             PP32_DEBUG_REG_ADDR(0x0E00 + (i) * 16 + (n))

/*
 *    Code/Data Memory (CDM) Interface Configuration Register
 */
#define CDM_CFG                                 PPE_REG_ADDR(0x0100)

#define CDM_CFG_RAM1_SET(value)                 SET_BITS(0, 3, 2, value)
#define CDM_CFG_RAM0_SET(value)                 ((value) ? (1 << 1) : 0)

/*
 *  ETOP MDIO Configuration Register
 */
//#define ETOP_MDIO_CFG_SMRST(value)              ((value) ? (1 << 13) : 0)
//#define ETOP_MDIO_CFG_PHYA(i, value)            ((i) ? SET_BITS(0, 12, 8, (value)) : SET_BITS(0, 7, 3, (value)))
//#define ETOP_MDIO_CFG_UMM(i, value)             ((value) ? ((i) ? (1 << 2) : (1 << 1)) : 0)
//
//#define ETOP_MDIO_CFG_MASK(i)                   (ETOP_MDIO_CFG_SMRST(1) | ETOP_MDIO_CFG_PHYA(i, 0x1F) | ETOP_MDIO_CFG_UMM(i, 1))

/*
 *  ENet MAC Configuration Register
 */
//#define ENET_MAC_CFG_CRC(i)                     (*ENET_MAC_CFG(i) & (0x01 << 11))
//#define ENET_MAC_CFG_DUPLEX(i)                  (*ENET_MAC_CFG(i) & (0x01 << 2))
//#define ENET_MAC_CFG_SPEED(i)                   (*ENET_MAC_CFG(i) & (0x01 << 1))
//#define ENET_MAC_CFG_LINK(i)                    (*ENET_MAC_CFG(i) & 0x01)
//
//#define ENET_MAC_CFG_CRC_OFF(i)                 do { *ENET_MAC_CFG(i) &= ~(1 << 11); } while (0)
//#define ENET_MAC_CFG_CRC_ON(i)                  do { *ENET_MAC_CFG(i) |= 1 << 11; } while (0)
//#define ENET_MAC_CFG_DUPLEX_HALF(i)             do { *ENET_MAC_CFG(i) &= ~(1 << 2); } while (0)
//#define ENET_MAC_CFG_DUPLEX_FULL(i)             do { *ENET_MAC_CFG(i) |= 1 << 2; } while (0)
//#define ENET_MAC_CFG_SPEED_10M(i)               do { *ENET_MAC_CFG(i) &= ~(1 << 1); } while (0)
//#define ENET_MAC_CFG_SPEED_100M(i)              do { *ENET_MAC_CFG(i) |= 1 << 1; } while (0)
//#define ENET_MAC_CFG_LINK_NOT_OK(i)             do { *ENET_MAC_CFG(i) &= ~1; } while (0)
//#define ENET_MAC_CFG_LINK_OK(i)                 do { *ENET_MAC_CFG(i) |= 1; } while (0)

/*
 *  ENets Configuration Register
 */
//#define ENETS_CFG_VL2_SET                       (1 << 29)
//#define ENETS_CFG_VL2_CLEAR                     ~(1 << 29)
//#define ENETS_CFG_FTUC_SET                      (1 << 28)
//#define ENETS_CFG_FTUC_CLEAR                    ~(1 << 28)
//#define ENETS_CFG_DPBC_SET                      (1 << 27)
//#define ENETS_CFG_DPBC_CLEAR                    ~(1 << 27)
//#define ENETS_CFG_DPMC_SET                      (1 << 26)
//#define ENETS_CFG_DPMC_CLEAR                    ~(1 << 26)

/*
 *  ENets Classification Configuration Register
 */
//#define ENETS_COS_CFG_VLAN_SET                  (1 << 1)
//#define ENETS_COS_CFG_VLAN_CLEAR                ~(1 << 1)
//#define ENETS_COS_CFG_DSCP_SET                  (1 << 0)
//#define ENETS_COS_CFG_DSCP_CLEAR                ~(1 << 0)

/*
 *  QSB Internal Cell Delay Variation Register
 */
#define QSB_ICDV                                QSB_CONF_REG(0x0007)

#define QSB_ICDV_TAU                            GET_BITS(*QSB_ICDV, 5, 0)

#define QSB_ICDV_TAU_SET(value)                 SET_BITS(0, 5, 0, value)

/*
 *  QSB Scheduler Burst Limit Register
 */
#define QSB_SBL                                 QSB_CONF_REG(0x0009)

#define QSB_SBL_SBL                             GET_BITS(*QSB_SBL, 3, 0)

#define QSB_SBL_SBL_SET(value)                  SET_BITS(0, 3, 0, value)

/*
 *  QSB Configuration Register
 */
#define QSB_CFG                                 QSB_CONF_REG(0x000A)

#define QSB_CFG_TSTEPC                          GET_BITS(*QSB_CFG, 1, 0)

#define QSB_CFG_TSTEPC_SET(value)               SET_BITS(0, 1, 0, value)

/*
 *  QSB RAM Transfer Table Register
 */
#define QSB_RTM                                 QSB_CONF_REG(0x000B)

#define QSB_RTM_DM                              (*QSB_RTM)

#define QSB_RTM_DM_SET(value)                   ((value) & 0xFFFFFFFF)

/*
 *  QSB RAM Transfer Data Register
 */
#define QSB_RTD                                 QSB_CONF_REG(0x000C)

#define QSB_RTD_TTV                             (*QSB_RTD)

#define QSB_RTD_TTV_SET(value)                  ((value) & 0xFFFFFFFF)

/*
 *  QSB RAM Access Register
 */
#define QSB_RAMAC                               QSB_CONF_REG(0x000D)

#define QSB_RAMAC_RW                            (*QSB_RAMAC & (1 << 31))
#define QSB_RAMAC_TSEL                          GET_BITS(*QSB_RAMAC, 27, 24)
#define QSB_RAMAC_LH                            (*QSB_RAMAC & (1 << 16))
#define QSB_RAMAC_TESEL                         GET_BITS(*QSB_RAMAC, 9, 0)

#define QSB_RAMAC_RW_SET(value)                 ((value) ? (1 << 31) : 0)
#define QSB_RAMAC_TSEL_SET(value)               SET_BITS(0, 27, 24, value)
#define QSB_RAMAC_LH_SET(value)                 ((value) ? (1 << 16) : 0)
#define QSB_RAMAC_TESEL_SET(value)              SET_BITS(0, 9, 0, value)

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
 *  DSL registers
 */
#define AT_CTRL                                 PPE_REG_ADDR(0xD02)
#define AR_CTRL                                 PPE_REG_ADDR(0xD08)
#define AT_IDLE0                                PPE_REG_ADDR(0xD28)
#define AT_IDLE1                                PPE_REG_ADDR(0xD29)
#define AR_IDLE0                                PPE_REG_ADDR(0xD74)
#define AR_IDLE1                                PPE_REG_ADDR(0xD75)
#define RFBI_CFG                                PPE_REG_ADDR(0x400)
#define SFSM_DBA0                               PPE_REG_ADDR(0x412)
#define SFSM_DBA1                               PPE_REG_ADDR(0x413)
#define SFSM_CBA0                               PPE_REG_ADDR(0x414)
#define SFSM_CBA1                               PPE_REG_ADDR(0x415)
#define SFSM_CFG0                               PPE_REG_ADDR(0x416)
#define SFSM_CFG1                               PPE_REG_ADDR(0x417)
#define FFSM_DBA0                               PPE_REG_ADDR(0x508)
#define FFSM_DBA1                               PPE_REG_ADDR(0x509)
#define FFSM_CFG0                               PPE_REG_ADDR(0x50A)
#define FFSM_CFG1                               PPE_REG_ADDR(0x50B)
#define FFSM_IDLE_HEAD_BC0                      PPE_REG_ADDR(0x50E)
#define FFSM_IDLE_HEAD_BC1                      PPE_REG_ADDR(0x50F)

/*
 *  Reset Registers
 */
#define AMAZON_S_RCU_BASE_ADDR                  (KSEG1 | 0x1F203000)
#define AMAZON_S_RCU_RST_REQ                    ((volatile u32*)(AMAZON_S_RCU_BASE_ADDR + 0x0010))
#define AMAZON_S_RCU_RST_STAT                   ((volatile u32*)(AMAZON_S_RCU_BASE_ADDR + 0x0014))
#define AMAZON_S_USB_CFG                        ((volatile u32*)(AMAZON_S_RCU_BASE_ADDR + 0x0018))
#define AMAZON_S_RCU_PPE_CONF                   ((volatile u32*)(AMAZON_S_RCU_BASE_ADDR + 0x002C))

/*
 *  Power Management Unit Registers
 */
//#define AMAZON_S_PMU                            (KSEG1 + 0x1F102000)
//#define PMU_PWDCR                               ((volatile u32 *)(AMAZON_S_PMU + 0x001C))
//#define PMU_SR                                  ((volatile u32 *)(AMAZON_S_PMU + 0x0020))

/*
 *  GPIO Registers
 */
//#define AMAZON_S_GPIO                           (KSEG1 + 0x1E100B00)
//#define GPIO_Pi_OUT(i)                          ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0010))
//#define GPIO_Pi_IN(i)                           ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0014))
//#define GPIO_Pi_DIR(i)                          ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0018))
//#define GPIO_Pi_ALTSEL0(i)                      ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x001C))
//#define GPIO_Pi_ALTSEL1(i)                      ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0020))
//#define GPIO_Pi_OD(i)                           ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0024))
//#define GPIO_Pi_STOFF(i)                        ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0028))
//#define GPIO_Pi_PUDSEL(i)                       ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x002C))
//#define GPIO_Pi_PUDEN(i)                        ((volatile u32 *)(AMAZON_S_GPIO + (i) * 0x0030 + 0x0030))

/*
 *  DMA Registers
 */
#define AMAZON_S_DMA                            (KSEG1 | 0x1E104100)
#define AMAZON_S_DMA_BASE                       AMAZON_S_DMA
#define AMAZON_S_DMA_CLC                        (volatile u32*)(AMAZON_S_DMA_BASE + 0x00)
#define AMAZON_S_DMA_ID                         (volatile u32*)(AMAZON_S_DMA_BASE + 0x08)
#define AMAZON_S_DMA_CTRL                       (volatile u32*)(AMAZON_S_DMA_BASE + 0x10)
#define AMAZON_S_DMA_CPOLL                      (volatile u32*)(AMAZON_S_DMA_BASE + 0x14)
#define AMAZON_S_DMA_CS(i)                      (volatile u32*)(AMAZON_S_DMA_BASE + 0x18 + 0x38 * (i))
#define AMAZON_S_DMA_CCTRL(i)                   (volatile u32*)(AMAZON_S_DMA_BASE + 0x1C + 0x38 * (i))
#define AMAZON_S_DMA_CDBA(i)                    (volatile u32*)(AMAZON_S_DMA_BASE + 0x20 + 0x38 * (i))
#define AMAZON_S_DMA_CDLEN(i)                   (volatile u32*)(AMAZON_S_DMA_BASE + 0x24 + 0x38 * (i))
#define AMAZON_S_DMA_CIS(i)                     (volatile u32*)(AMAZON_S_DMA_BASE + 0x28 + 0x38 * (i))
#define AMAZON_S_DMA_CIE(i)                     (volatile u32*)(AMAZON_S_DMA_BASE + 0x2C + 0x38 * (i))
#define AMAZON_S_CGBL                           (volatile u32*)(AMAZON_S_DMA_BASE + 0x30)
#define AMAZON_S_DMA_PS(i)                      (volatile u32*)(AMAZON_S_DMA_BASE + 0x40 + 0x30 * (i))
#define AMAZON_S_DMA_PCTRL(i)                   (volatile u32*)(AMAZON_S_DMA_BASE + 0x44 + 0x30 * (i))
#define AMAZON_S_DMA_IRNEN                      (volatile u32*)(AMAZON_S_DMA_BASE + 0xf4)
#define AMAZON_S_DMA_IRNCR                      (volatile u32*)(AMAZON_S_DMA_BASE + 0xf8)
#define AMAZON_S_DMA_IRNICR                     (volatile u32*)(AMAZON_S_DMA_BASE + 0xfc)

/*
 *  External Interrupt Registers
 */
#define AMAZON_S_EIU_BASE                       (KSEG1 | 0x1F101000)
#define AMAZON_S_EIU_EXIN_C                     (volatile u32*)(AMAZON_S_EIU_BASE + 0x00)
#define AMAZON_S_EIU_INIC                       (volatile u32*)(AMAZON_S_EIU_BASE + 0x04)
#define AMAZON_S_EIU_INC                        (volatile u32*)(AMAZON_S_EIU_BASE + 0x08)
#define AMAZON_S_EIU_INEN                       (volatile u32*)(AMAZON_S_EIU_BASE + 0x0C)
#define AMAZON_S_EIU_YIELDEN0                   (volatile u32*)(AMAZON_S_EIU_BASE + 0x10)
#define AMAZON_S_EIU_YIELDEN1                   (volatile u32*)(AMAZON_S_EIU_BASE + 0x14)
#define AMAZON_S_EIU_YIELDEN2                   (volatile u32*)(AMAZON_S_EIU_BASE + 0x18)
#define AMAZON_S_EIU_NMI_CR                     (volatile u32*)(AMAZON_S_EIU_BASE + 0xF0)
#define AMAZON_S_EIU_NMI_SR                     (volatile u32*)(AMAZON_S_EIU_BASE + 0xF4)


/*
 *  Emulation
 */


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
 *  Switch Header, Flag Header & Descriptor
 */
#if defined(__BIG_ENDIAN)

#if 0
  struct sw_eg_pkt_header {
    unsigned int    spid                :8; //  bit 26-24
    unsigned int    dpid                :8; //  bit 17-16
    unsigned int    qid                 :8; //  bit 9-8
    unsigned int    direct              :8; //  bit 0
  };
#else
  struct sw_eg_pkt_header {
    //  byte 0
    unsigned int    res1                :5;
    unsigned int    spid                :3;
    //  byte 1
    unsigned int    crc_gen_dis         :1; //  0: enable CRC generation, 1: disable CRC generation
    unsigned int    res2                :4;
    unsigned int    dpid                :3;
    //  byte 2
    unsigned int    port_map_en         :1; //  0: ignore Dest Eth Port Map, 1: use Dest Eth Port Map (field port_map)
    unsigned int    port_map_sel        :1; //  0: field port_map is Dest Port Mask, 1: field port_map is Dest Port Map
    unsigned int    lrn_dis             :1; //  0/1: enable/disable source MAC address learning
    unsigned int    class_en            :1; //  0/1: disable/enable Target Traffic Class (field class)
    unsigned int    class               :4; //  Target Traffic Class
    //  byte 3
    unsigned int    res3                :1;
    unsigned int    port_map            :6;
    unsigned int    dpid_en             :1; //  0/1: disable/enable field dpid
  };
#endif

  struct flag_header {
    //  0 - 3h
    unsigned int    ipv4_mc_pkt         :1; //  IPv4 multicast packet?
    unsigned int    res0                :1;
    unsigned int    proc_type           :1; //  0: routing, 1: bridging
    unsigned int    res1                :1;
    unsigned int    tcpudp_err          :1; //  reserved in A4
    unsigned int    tcpudp_chk          :1; //  reserved in A4
    unsigned int    is_udp              :1;
    unsigned int    is_tcp              :1;
    unsigned int    res2                :2;
    unsigned int    ip_offset           :6;
    unsigned int    is_pppoes           :1; //  2h
    unsigned int    is_ipv6             :1;
    unsigned int    is_ipv4             :1;
    unsigned int    is_vlan             :2; //  0: nil, 1: single tag, 2: double tag, 3: reserved
    unsigned int    rout_index          :11;

    //  4 - 7h
    unsigned int    dest_list           :8;
    unsigned int    src_itf             :3; //  7h
    unsigned int    tcp_rstfin          :1; //  7h
    unsigned int    dslwan_qid          :4; //  7h
    unsigned int    temp_dest_list      :8; //  only for firmware use
    unsigned int    src_dir             :1; //  0: LAN, 1: WAN
    unsigned int    acc_done            :1;
    unsigned int    pl_byteoff          :6;
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
    unsigned int    qid                 :4; //  not for WLAN TX, careful, in DSL it's 4 bits, in ETH it's 3 bits
    unsigned int    datalen             :15;//  careful, in DSL it's 15 bits, in ETH it's 16 bits
    //  4 - 7h
    unsigned int    small               :1;
    unsigned int    res1                :2;
    unsigned int    dataptr             :29;
  };

  struct eth_tx_descriptor {
    //  0 - 3h
    unsigned int    own                 :1; //  0: MIPS, 1: PPE
    unsigned int    c                   :1;
    unsigned int    sop                 :1;
    unsigned int    eop                 :1;
    unsigned int    byteoff             :5;
    unsigned int    qid                 :4;
    unsigned int    res1                :3;
    unsigned int    datalen             :16;
    //  4 - 7h
    unsigned int    small               :1;
    unsigned int    res2                :2;
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
    u32                             rx_errors;
    u32                             rx_dropped;
    u32                             tx_packets;
    u32                             tx_bytes;
    u32                             tx_errors;
    u32                             tx_dropped;

    u32                             dev_id;

};

struct atm_port {
    struct atm_dev                 *dev;

    u32                             tx_max_cell_rate;       /*  maximum cell rate                       */
    u32                             tx_used_cell_rate;      /*  currently used cell rate                */
};

struct atm_connection {
    struct atm_vcc                 *vcc;                    /*  opened VCC                              */
    struct timespec                 access_time;            /*  time when last F4/F5 user cell arrives  */

    int                             prio_queue_map[8];
    u32                             prio_tx_packets[8];

    u32                             rx_packets;
    u32                             rx_bytes;
    u32                             rx_errors;
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
static void eth_init(struct net_device *);
static struct net_device_stats *eth_get_stats(struct net_device *);
static int eth_open(struct net_device *);
static int eth_stop(struct net_device *);
static int eth_hard_start_xmit(struct sk_buff *, struct net_device *);
static int eth_qos_hard_start_xmit(struct sk_buff *, struct net_device *);
static int eth_set_mac_address(struct net_device *, void *);
static int eth_ioctl(struct net_device *, struct ifreq *, int);
//static int eth_change_mtu(struct net_device *, int);
static void eth_tx_timeout(struct net_device *);

/*
 *  Network operations help functions
 */
static INLINE int get_eth_port(struct net_device *);
static INLINE int eth_xmit(struct sk_buff *, unsigned int, int, int, int);

/*
 *  ioctl help functions
 */
static INLINE int ethtool_ioctl(struct net_device *, struct ifreq *);
#if 0   //  TODO
static INLINE void set_vlan_cos(struct vlan_cos_req *);
static INLINE void set_dscp_cos(struct dscp_cos_req *);
#endif

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
static INLINE struct sk_buff *__get_skb_pointer(unsigned int, const char *, unsigned int);
#define get_skb_pointer(dataptr)    __get_skb_pointer(dataptr, __FUNCTION__, __LINE__)
static INLINE void __put_skb_to_dbg_pool(struct sk_buff *, const char *, unsigned int);
#define put_skb_to_dbg_pool(skb)    __put_skb_to_dbg_pool(skb, __FUNCTION__, __LINE__)
static struct sk_buff* atm_alloc_tx(struct atm_vcc *, unsigned int);
static INLINE void atm_free_tx_skb_vcc(struct sk_buff *skb);
static INLINE void atm_free_tx_skb_vcc_delayed(struct sk_buff *skb);

/*
 *  Mailbox handler
 */
static irqreturn_t mailbox_irq_handler(int, void *);

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
static INLINE void start_cpu_port(void);
static INLINE void init_internal_tantos_qos_setting(void);
static INLINE void init_internal_tantos(void);
static INLINE void init_dplus(void);
static INLINE void init_ema(void);
static INLINE void init_mailbox(void);
static INLINE void clear_share_buffer(void);
static INLINE void clear_cdm(void);
static INLINE void board_init(void);
static INLINE void init_dsl_hw(void);
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
 *  Print Firmware/Driver Version ID
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
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
  static int proc_write_route(struct file *, const char *, unsigned long, void *);
  static int proc_read_queue(char *, char **, off_t, int, int *, void *);
  static int proc_write_queue(struct file *, const char *, unsigned long, void *);
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
static INLINE unsigned int sw_get_rmon_counter(int, int);
static INLINE void sw_clr_rmon_counter(int);
static INLINE int read_port_counters(int, char *);
static int port0_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port1_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port2_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port3_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port4_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port5_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port6_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port_counters_proc_write(struct file *, const char *, unsigned long, void *);
//static int proc_read_mactable(char *, char **, off_t, int, int *, void *);
//static int proc_write_mactable(struct file *, const char *, unsigned long, void *);
static int proc_read_directforwarding(char *, char **, off_t, int, int *, void *);
static int proc_write_directforwarding(struct file *, const char *, unsigned long, void *);
static int proc_read_clk(char *, char **, off_t, int, int *, void *);
static int proc_write_clk(struct file *, const char *, unsigned long, void *);
static int proc_read_flowcontrol(char *, char **, off_t, int, int *, void *);
static int proc_write_flowcontrol(struct file *, const char *, unsigned long, void *);
static int proc_read_prio(char *, char **, off_t, int, int *, void *);
static int proc_write_prio(struct file *, const char *, unsigned long, void *);
static int proc_read_ewan(char *, char **, off_t, int, int *, void *);
static int proc_write_ewan(struct file *, const char *, unsigned long, void *);

#ifdef CONFIG_IFX_PPA_QOS
  static int proc_read_qos(char *, char **, off_t, int, int *, void *);
#endif
#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
  static int proc_read_dsl_vlan(char *, char **, off_t, int, int *, void *);
  static int proc_write_dsl_vlan(struct file *, const char *, unsigned long, void *);
  static INLINE int ppe_set_vlan(int, unsigned short);
  static INLINE int sw_set_vlan_bypass(int, unsigned int, int);
#endif
#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
  static int proc_read_mirror(char *, char **, off_t, int, int *, void *);
  static int proc_write_mirror(struct file *, const char *, unsigned long, void *);
#endif
static int32_t ppa_datapath_generic_hook(PPA_GENERIC_DATAPATH_HOOK_CMD cmd, void *buffer, uint32_t flag);

/*
 *  Proc File help functions
 */
#if 0
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
  static INLINE int print_route(char *, int, int, struct rout_forward_compare_tbl *, struct rout_forward_action_tbl *);
#endif
#endif
/*
 *  Debug functions
 */
#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
  static INLINE void dump_skb(struct sk_buff *, u32, char *, int, int, int, int);
#else
  #define dump_skb(a, b, c, d, e, f)
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
 *  Local MAC Address Tracking Functions
 */
static INLINE void register_netdev_event_handler(void);
static INLINE void unregister_netdev_event_handler(void);
static int netdev_event_handler(struct notifier_block *, unsigned long, void *);

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

static int g_fwcode = FWCODE_ROUTING_ACC_A5;

//static spinlock_t g_d5_lock;

static struct semaphore g_sem;  //  lock used by open/close function

static int g_eth_wan_mode = 0;
static unsigned int g_wan_itf = 3 << 6;

static int g_ipv6_acc_en = 1;
static unsigned int g_ipv4_lan_collision_rout_fwdc_table_base;
static unsigned int g_ipv4_wan_collision_rout_fwda_table_base;

static int g_wanqos_en = 0;

static int g_cpu_to_wan_tx_desc_pos = 0;
static int g_cpu_to_wan_swap_desc_pos = 0;
static int g_wan_rx1_desc_pos = 0;
struct sk_buff *g_wan_rx1_desc_skb[WAN_RX_DESC_NUM(1)] = {0};

static u32 g_mailbox_signal_mask;

static int                      g_f_dma_opened = 0;
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

#ifdef CONFIG_IFXMIPS_MEI_FW_LOOPBACK
  static int g_showtime = 1;
#else
  static int g_showtime = 0;
#endif
static void *g_xdata_addr = NULL;

static u32 g_dsl_wrx_correct_pdu = 0;
static u32 g_dsl_wtx_total_pdu = 0;
static struct timer_list g_dsl_led_polling_timer;
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK
  static void *g_data_led_trigger = NULL;
#endif

//static struct eth_priv_data g_eth_priv_data[2];

static struct net_device *g_eth_net_dev[2] = {0};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static struct net_device_ops eth_netdev_ops[2] = {
    {
    .ndo_open               = eth_open,
    .ndo_stop               = eth_stop,
    .ndo_get_stats          = eth_get_stats,
    .ndo_set_mac_address    = eth_set_mac_address,
    .ndo_tx_timeout         = eth_tx_timeout,
    .ndo_do_ioctl           = eth_ioctl,
    },
    {
    .ndo_open               = eth_open,
    .ndo_stop               = eth_stop,
    .ndo_get_stats          = eth_get_stats,
    .ndo_set_mac_address    = eth_set_mac_address,
    .ndo_tx_timeout         = eth_tx_timeout,
    .ndo_do_ioctl           = eth_ioctl,
    },
};
#endif

static int g_eth_prio_queue_map[2][8];

static u8 g_my_ethaddr[MAX_ADDR_LEN * 2] = {0};

static struct proc_dir_entry *g_eth_proc_dir = NULL;

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int g_dbg_enable = 0;
#endif

//  1:switch forward packets between port0/1 and cpu port directly w/o processing
//  0:pkts go through normal path and are processed by switch central function
static int g_ingress_direct_forward_enable = 0;
static int g_egress_direct_forward_enable = 1;

static int g_pmac_ewan = 0;

/*
 *  variable for directpath
 */
static int g_directpath_dma_full = 0;

#if !defined(DISABLE_LOCAL_MAC_ADD_TO_SW) || !DISABLE_LOCAL_MAC_ADD_TO_SW
  static struct notifier_block g_netdev_event_handler_nb = {0};
#endif

#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
  static unsigned short g_dsl_vlan_qid_map[ATM_QUEUE_NUMBER * 2] = {0};
#endif

#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
  static struct sk_buff *g_dbg_skb_swap_pool[1024] = {0};
#endif

#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
  static struct net_device *g_mirror_netdev = NULL;
#endif



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

/*
 *  variable for directpath
 */
struct ppe_directpath_data g_ppe_directpath_data[3];    //  port 0 - eth0, 1 - eth1, 2 - cpu, 6 - dsl (PPPoA/IPoA), 7 - dsl (EoA)
EXPORT_SYMBOL(g_ppe_directpath_data);

#if defined(ENABLE_DFE_LOOPBACK_TEST) && ENABLE_DFE_LOOPBACK_TEST
extern void dsl_powerup(void);
extern void wait_ppe_sync_state(u32 state);
#endif

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
        value = DSL_WAN_MIB_TABLE->wrx_total_byte;
        u64_add_u32(g_atm_priv_data.wrx_total_byte, value - g_atm_priv_data.prev_mib.wrx_total_byte, &g_atm_priv_data.wrx_total_byte);
        g_atm_priv_data.prev_mib.wrx_total_byte = value;
        mib_aal5.ifHCInOctets_h = g_atm_priv_data.wrx_total_byte.h;
        mib_aal5.ifHCInOctets_l = g_atm_priv_data.wrx_total_byte.l;

        value = DSL_WAN_MIB_TABLE->wtx_total_byte;
        u64_add_u32(g_atm_priv_data.wtx_total_byte, value - g_atm_priv_data.prev_mib.wtx_total_byte, &g_atm_priv_data.wtx_total_byte);
        g_atm_priv_data.prev_mib.wtx_total_byte = value;
        mib_aal5.ifHCOutOctets_h = g_atm_priv_data.wtx_total_byte.h;
        mib_aal5.ifHCOutOctets_l = g_atm_priv_data.wtx_total_byte.l;

        mib_aal5.ifInUcastPkts  = g_atm_priv_data.wrx_pdu;
        mib_aal5.ifOutUcastPkts = DSL_WAN_MIB_TABLE->wtx_total_pdu;
        mib_aal5.ifInErrors     = DSL_WAN_MIB_TABLE->wrx_err_pdu;
        mib_aal5.ifInDiscards   = DSL_WAN_MIB_TABLE->wrx_dropdes_pdu + g_atm_priv_data.wrx_drop_pdu;
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
    int   vci = vcc->vci;
    struct atm_port *port = &g_atm_priv_data.port[(int)vcc->dev->dev_data];
    int sw_tx_queue;
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

    /*  allocate software TX queue  */
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
            for ( i = 0; i < 8; i++ )
                g_atm_priv_data.connection[conn].prio_queue_map[i] = sw_tx_queue;
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
    wrx_queue_config.new_vlan   = 0x0FF0 | conn;    //  use vlan to differentiate PVCs
    wrx_queue_config.vlan_ins   = 1;    //  use vlan to differentiate PVCs, please use outer VLAN in routing table, and do remember to configure switch to handle bridging case
    wrx_queue_config.mpoa_type  = 3;    //  set IPoA as default
    wrx_queue_config.ip_ver     = 0;    //  set IPv4 as default
    wrx_queue_config.mpoa_mode  = 0;    //  set VC-mux as default
    wrx_queue_config.oversize   = aal5r_max_packet_size;
    wrx_queue_config.undersize  = aal5r_min_packet_size;
    wrx_queue_config.mfs        = aal5r_max_packet_size;

    wtx_queue_config.same_vc_qmap   = 0x00;     //  only one TX queue is assigned now, use ioctl to add other TX queues
    wtx_queue_config.sbid           = g_atm_priv_data.connection[conn].port;
    wtx_queue_config.qsb_vcid       = conn + QSB_QUEUE_NUMBER_BASE; //  qsb qid = firmware qid + 1
    wtx_queue_config.mpoa_mode      = 0;        //  set VC-mux as default
    wtx_queue_config.qsben          = 1;        //  reserved in A4, however put 1 for backward compatible

    cell_header = (struct uni_cell_header *)((u32 *)&wtx_queue_config + 2);
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
    int i;

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
    for ( i = 0; i < IDLE_CYCLE_NUMBER; i++ );

#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
    for ( i = 0; i < NUM_ENTITY(g_dsl_vlan_qid_map); i += 2 )
        if ( g_dsl_vlan_qid_map[i] == WRX_QUEUE_CONFIG(conn)->new_vlan )
        {
            g_dsl_vlan_qid_map[i] = 0;
            break;
        }
#endif

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

#if (defined(ENABLE_DSL_LOOPBACK_TEST) && ENABLE_DSL_LOOPBACK_TEST) || (defined(ENABLE_DFE_LOOPBACK_TEST) && ENABLE_DFE_LOOPBACK_TEST)
    {
        unsigned char mac[6];
        unsigned char mac_broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        unsigned char ip[4];
        int off = 0;

        dump_skb(skb, DUMP_SKB_LEN, "ppe_send - before swap", 7, 0, 1, 0);

        switch ( WRX_QUEUE_CONFIG(conn)->mpoa_type )
        {
        case 0: //  EoA w/o FCS
        case 1: //  EoA w FCS
            if ( memcmp(skb->data + 10, mac_broadcast, 6) == 0 )
                break;
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
  #if (defined(ENABLE_DSL_LOOPBACK_TEST) && ENABLE_DSL_LOOPBACK_TEST)
        dump_skb(skb, DUMP_SKB_LEN, "DSL Loopback", (WRX_QUEUE_CONFIG(conn)->mpoa_type & 0x02) ? 7 : 6, 0, 0, 0);

        g_atm_priv_data.connection[conn].access_time = current_kernel_time();

        adsl_led_flash();

        vcc->push(vcc, skb);

        if ( vcc->stats )
            atomic_inc(&vcc->stats->rx);

        g_atm_priv_data.wrx_pdu++;

        UPDATE_VCC_STAT(conn, rx_packets, 1);
        UPDATE_VCC_STAT(conn, rx_bytes, skb->len);

        return 0;
  #endif
    }
#endif

    priority = skb->priority;
    if ( priority >= 8 )
        priority = 7;

    /*  reserve space to put pointer in skb */
    byteoff = (u32)skb->data & (DMA_ALIGNMENT - 1);
    if ( skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff || skb->end - skb->data < 1564 /* 1518 (max ETH frame) + 22 (reserved for firmware) + 10 (AAL5) + 6 (PPPoE) + 4 x 2 (2 VLAN) */ || skb_shared(skb) || skb_cloned(skb) )
    {
        //  this should be few case

        struct sk_buff *new_skb;

        //dbg("skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff");
        ASSERT(skb_headroom(skb) >= sizeof(struct sk_buff *) + byteoff, "skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff");
        ASSERT(skb->end - skb->data >= 1564, "skb->end - skb->data < 1518 + 22 + 10 + 6 + 4 x 2");

        new_skb = alloc_skb_tx(skb->len < DMA_PACKET_SIZE ? DMA_PACKET_SIZE : skb->len);    //  may be used by RX fastpath later
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
        byteoff = (u32)skb->data & (DMA_ALIGNMENT - 1);
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

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & (DMA_ALIGNMENT - 1)) == 0, "reg_desc.dataptr (0x%#x) must be 8 DWORDS aligned", reg_desc.dataptr);
    skb_to_free = get_skb_pointer(reg_desc.dataptr);
    atm_free_tx_skb_vcc_delayed(skb_to_free);
    put_skb_to_dbg_pool(skb);

#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
    if ( g_mirror_netdev != NULL )
    {
        if ( (WRX_QUEUE_CONFIG(conn)->mpoa_type & 0x02) == 0 )  //  handle EoA only, TODO: IPoA/PPPoA
        {
            struct sk_buff *new_skb = skb_clone(skb, GFP_ATOMIC);

            if ( new_skb != NULL )
            {
                if ( WRX_QUEUE_CONFIG(conn)->mpoa_mode )    //  LLC
                    skb_pull(new_skb, 10);  //  remove EoA header
                else                                        //  VC mux
                    skb_pull(new_skb, 2);   //  remove EoA header
                if ( WRX_QUEUE_CONFIG(conn)->mpoa_type == 1 )   //  remove FCS
                    skb_trim(new_skb, new_skb->len - 4);
                new_skb->dev = g_mirror_netdev;
                dev_queue_xmit(new_skb);
            }
        }
    }
#endif

    /*  update descriptor   */
    reg_desc.dataptr    = (u32)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1));  //  byte address
    reg_desc.byteoff    = byteoff;
    reg_desc.datalen    = skb->len;
    reg_desc.mpoa_pt    = 1;
    reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
    reg_desc.pdu_type   = vcc->qos.aal == ATM_AAL5 ? 0 : 1;
    reg_desc.qid        = g_atm_priv_data.connection[conn].prio_queue_map[priority];
    reg_desc.own        = 0;
    reg_desc.c          = 0;
    reg_desc.sop = reg_desc.eop = 1;

    /*  update MIB  */
    UPDATE_VCC_STAT(conn, tx_packets, 1);
    UPDATE_VCC_STAT(conn, tx_bytes, skb->len);

    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx);
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_pdu++;

    g_atm_priv_data.connection[conn].prio_tx_packets[priority]++;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send", 7, reg_desc.qid, 1, 0);

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
    ASSERT((reg_desc.dataptr & (DMA_ALIGNMENT - 1)) == 0, "reg_desc.dataptr (0x%#x) must be 8 DWORDS aligned", reg_desc.dataptr);
    skb_to_free = get_skb_pointer(reg_desc.dataptr);
    atm_free_tx_skb_vcc_delayed(skb_to_free);
    put_skb_to_dbg_pool(skb);

    /*  setup descriptor    */
    reg_desc.dataptr    = (u32)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1));  //  byte address
    reg_desc.byteoff    = (u32)skb->data & (DMA_ALIGNMENT - 1);
    reg_desc.datalen    = skb->len;
    reg_desc.mpoa_pt    = 1;
    reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
    reg_desc.pdu_type   = 1;    //  cell
    reg_desc.qid        = g_atm_priv_data.connection[conn].prio_queue_map[7];   //  expect priority '7' is highest priority
    reg_desc.own        = 0;
    reg_desc.c          = 0;
    reg_desc.sop = reg_desc.eop = 1;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send_oam", 7, reg_desc.qid, 1, 0);

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
}

static void eth_init(struct net_device *dev)
{
    int port;
    u8 *ethaddr;
    u32 val;
    int i;
    struct eth_priv_data *priv = netdev_priv(dev);

    for(port = 0; port < NUM_ENTITY(g_eth_net_dev) && g_eth_net_dev[port] != NULL; port ++);

    if(port >= NUM_ENTITY(g_eth_net_dev)){
        printk("Error, Already Created enough ethernet devices!\n");
        return;
    }

    ethaddr = MY_ETH0_ADDR + port * MAX_ADDR_LEN;

    ether_setup(dev);   /*  assign some members */

    /*  hook network operations */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    dev->get_stats           = eth_get_stats;
    dev->open                = eth_open;
    dev->stop                = eth_stop;
    if ( g_eth_wan_mode == 3 && g_wanqos_en && (g_wan_itf & (1 << port)) )
        dev->hard_start_xmit = eth_qos_hard_start_xmit;
    else
        dev->hard_start_xmit = eth_hard_start_xmit;
    dev->set_mac_address     = eth_set_mac_address;
    dev->do_ioctl            = eth_ioctl;
//    dev->change_mtu          = eth_change_mtu;
    dev->tx_timeout          = eth_tx_timeout;
    dev->watchdog_timeo      = ETH_WATCHDOG_TIMEOUT;
//    dev->priv                = g_eth_priv_data + port;

    SET_MODULE_OWNER(dev);
#else
    dev->watchdog_timeo = ETH_WATCHDOG_TIMEOUT;
    if(g_eth_wan_mode == 3 && g_wanqos_en && (g_wan_itf & (1 << port))){
        eth_netdev_ops[port].ndo_start_xmit = eth_qos_hard_start_xmit;
    }else{
        eth_netdev_ops[port].ndo_start_xmit = eth_hard_start_xmit;
    }
    dev->netdev_ops = &eth_netdev_ops[port];
#endif
    priv->dev_id = port;

    /*  read MAC address from the MAC table and put them into device    */
    val = 0;
    for ( i = 0; i < 6; i++ )
        val += ethaddr[i];
    if ( val == 0 )
    {
        /*  ethaddr not set in u-boot   */
        dev->dev_addr[0] = 0x00;
        dev->dev_addr[1] = 0x20;
        dev->dev_addr[2] = 0xda;
        dev->dev_addr[3] = 0x86;
        dev->dev_addr[4] = 0x23;
        dev->dev_addr[5] = 0x74 + port;
    }
    else
    {
        for ( i = 0; i < 6; i++ )
            dev->dev_addr[i] = ethaddr[i];
    }

    return ;   //  Qi Ming set 1 here in Switch driver
}

static struct net_device_stats *eth_get_stats(struct net_device *dev)
{
    int port;
    struct eth_priv_data *priv = netdev_priv(dev);

    port = get_eth_port(dev);
    if ( port < 0 )
        return NULL;

    priv->stats.rx_packets  = priv->rx_packets
                            + ITF_MIB_TBL(port)->ig_fast_brg_pkts
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_udp_pkts
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_tcp_pkts
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_mc_pkts
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv6_udp_pkts
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv6_tcp_pkts;
    priv->stats.rx_bytes    = priv->rx_bytes
                            + ITF_MIB_TBL(port)->ig_fast_brg_bytes
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_bytes
                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv6_bytes;
    priv->stats.rx_errors   = priv->rx_errors
                            + ITF_MIB_TBL(port)->ig_drop_pkts;
    priv->stats.rx_dropped  = priv->rx_dropped
                            + ITF_MIB_TBL(port)->ig_drop_pkts;

    priv->stats.tx_packets  = priv->tx_packets + ITF_MIB_TBL(port)->eg_fast_pkts;
    priv->stats.tx_bytes    = priv->tx_bytes;
    priv->stats.tx_errors   = priv->tx_errors;
    priv->stats.tx_dropped  = priv->tx_dropped;

    return &priv->stats;



#if 0
    int port;
    struct eth_priv_data *priv = get_devpriv(dev);

    port = get_eth_port(dev);
    if ( port < 0 )
        return NULL;

    g_eth_priv_data[port].stats.rx_packets  = g_eth_priv_data[port].rx_packets
                                            + ITF_MIB_TBL(port)->ig_fast_brg_pkts
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_udp_pkts
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_tcp_pkts
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_mc_pkts
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv6_udp_pkts
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv6_tcp_pkts;
    g_eth_priv_data[port].stats.rx_bytes    = g_eth_priv_data[port].rx_bytes
                                            + ITF_MIB_TBL(port)->ig_fast_brg_bytes
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv4_bytes
                                            + ITF_MIB_TBL(port)->ig_fast_rt_ipv6_bytes;
    g_eth_priv_data[port].stats.rx_errors   = g_eth_priv_data[port].rx_errors
                                            + ITF_MIB_TBL(port)->ig_drop_pkts;
    g_eth_priv_data[port].stats.rx_dropped  = g_eth_priv_data[port].rx_dropped
                                            + ITF_MIB_TBL(port)->ig_drop_pkts;

    g_eth_priv_data[port].stats.tx_packets  = g_eth_priv_data[port].tx_packets + ITF_MIB_TBL(port)->eg_fast_pkts;
    g_eth_priv_data[port].stats.tx_bytes    = g_eth_priv_data[port].tx_bytes;
    g_eth_priv_data[port].stats.tx_errors   = g_eth_priv_data[port].tx_errors;
    g_eth_priv_data[port].stats.tx_dropped  = g_eth_priv_data[port].tx_dropped;

    return &g_eth_priv_data[port].stats;
#endif
}

static int eth_open(struct net_device *dev)
{
    int port;

    port = get_eth_port(dev);
    if ( port < 0 )
        return -ENODEV;

    down(&g_sem);

    dbg("%s", dev->name);

    turn_on_dma_rx(port);

    netif_start_queue(dev);

    up(&g_sem);
    return 0;
}

static int eth_stop(struct net_device *dev)
{
    int port;

    port = get_eth_port(dev);
    if ( port < 0 )
        return -ENODEV;

    down(&g_sem);

    turn_off_dma_rx(port);

    netif_stop_queue(dev);

    up(&g_sem);
    return 0;
}

static int eth_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int port;
    int qid;

    port = get_eth_port(dev);
    if ( port < 0 )
        return -ENODEV;

    if ( skb->priority >= NUM_ENTITY(g_eth_prio_queue_map[port]) )
        qid = g_eth_prio_queue_map[port][NUM_ENTITY(g_eth_prio_queue_map[port]) - 1];
    else
        qid = g_eth_prio_queue_map[port][skb->priority];

    eth_xmit(skb, port, 2, 2, qid); //  spid - CPU, qid - taken from prio_queue_map

    return 0;
}

static int eth_qos_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int port;
    int qid;
    unsigned long sys_flag;
    volatile struct eth_tx_descriptor *desc;
    struct eth_tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;
    int byteoff;
    int len;
    struct sw_eg_pkt_header pkth = {0};
    struct eth_priv_data *priv = NULL;

    port = get_eth_port(dev);
    if ( port < 0 || port >= NUM_ENTITY(g_eth_net_dev) ){
        printk("%s: Improper ethernet device\n", __FUNCTION__);
        return -ENODEV;
    }

    priv = netdev_priv(dev);

    if ( skb->priority >= NUM_ENTITY(g_eth_prio_queue_map[port]) )
        qid = g_eth_prio_queue_map[port][NUM_ENTITY(g_eth_prio_queue_map[port]) - 1];
    else
        qid = g_eth_prio_queue_map[port][skb->priority];

    pkth.spid           = 2;    //  CPU
    pkth.dpid           = port; //  eth0/eth1
    pkth.lrn_dis        = 0;
    pkth.class_en       = 1;
    pkth.class          = qid;
    if ( pkth.dpid < 2 )
        pkth.dpid_en    = g_egress_direct_forward_enable;

    len = skb->len <= ETH_MIN_TX_PACKET_LENGTH ? ETH_MIN_TX_PACKET_LENGTH : skb->len;

    dump_skb(skb, DUMP_SKB_LEN, "eth_qos_hard_start_xmit", port, qid, 1, 0);

    /*  reserve space to put pointer in skb */
    byteoff = (((unsigned int)skb->data - sizeof(pkth)) & (DMA_ALIGNMENT - 1)) + sizeof(pkth);
    //if ( skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff || skb->end - skb->data < 1564 /* 1518 (max ETH frame) + 22 (reserved for firmware) + 10 (AAL5) + 6 (PPPoE) + 4 x 2 (2 VLAN) */ || skb_shared(skb) || skb_cloned(skb) )
    if ( skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff || skb_shared(skb) || skb_cloned(skb) )
    {
        struct sk_buff *new_skb;

        ASSERT(skb_headroom(skb) >= sizeof(struct sk_buff *) + byteoff, "skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff");
        new_skb = alloc_skb_tx(skb->len < DMA_PACKET_SIZE ? DMA_PACKET_SIZE : skb->len);    //  may be used by RX fastpath later
        if ( !new_skb )
        {
            err("no memory");
            goto ALLOC_SKB_TX_FAIL;
        }
        skb_put(new_skb, sizeof(pkth) + skb->len);
        my_memcpy(new_skb->data, &pkth, sizeof(pkth));
        my_memcpy(new_skb->data + sizeof(pkth), skb->data, skb->len);
        dev_kfree_skb_any(skb);
        skb = new_skb;
        byteoff = (u32)skb->data & (DMA_ALIGNMENT - 1);
#ifndef CONFIG_MIPS_UNCACHED
        /*  write back to physical memory   */
        dma_cache_wback((u32)skb->data, skb->len);
#endif
    }
    else
    {
        skb_push(skb, sizeof(pkth));
        byteoff -= sizeof(pkth);
        my_memcpy(skb->data, &pkth, sizeof(pkth));
        *(struct sk_buff **)((u32)skb->data - byteoff - sizeof(struct sk_buff *)) = skb;
#ifndef CONFIG_MIPS_UNCACHED
        /*  write back to physical memory   */
        dma_cache_wback((u32)skb->data - byteoff - sizeof(struct sk_buff *), skb->len + byteoff + sizeof(struct sk_buff *));
#endif
    }

    /*  allocate descriptor */
    local_irq_save(sys_flag);
    desc = (volatile struct eth_tx_descriptor *)(CPU_TO_WAN_TX_DESC_BASE + g_cpu_to_wan_tx_desc_pos);
    if ( desc->own )    //  PPE hold
    {
        local_irq_restore(sys_flag);
        err("PPE hold");
        goto NO_FREE_DESC;
    }
    if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
        g_cpu_to_wan_tx_desc_pos = 0;
    local_irq_restore(sys_flag);

    /*  load descriptor from memory */
    reg_desc = *desc;

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & (DMA_ALIGNMENT - 1)) == 0, "reg_desc.dataptr (0x%#x) must be 8 DWORDS aligned", reg_desc.dataptr);
    skb_to_free = get_skb_pointer(reg_desc.dataptr);
    dev_kfree_skb_any(skb_to_free);
    put_skb_to_dbg_pool(skb);

#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
    if ( g_mirror_netdev != NULL )
    {
        struct sk_buff *new_skb = skb_clone(skb, GFP_ATOMIC);

        if ( new_skb != NULL )
        {
            skb_pull(new_skb, sizeof(pkth));
            new_skb->dev = g_mirror_netdev;
            dev_queue_xmit(new_skb);
        }
    }
#endif

    /*  update descriptor   */
    reg_desc.small      = (unsigned int)skb->end - (unsigned int)skb->data < DMA_PACKET_SIZE ? 1 : 0;
    reg_desc.dataptr    = (u32)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1));  //  byte address
    reg_desc.byteoff    = byteoff;
    reg_desc.datalen    = len + sizeof(pkth);
    reg_desc.qid        = qid;
    reg_desc.own        = 1;
    reg_desc.c          = 0;
    reg_desc.sop = reg_desc.eop = 1;

    /*  update MIB  */

    dev->trans_start = jiffies;
    priv->tx_packets++;
    priv->tx_bytes += len;


    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    return 0;

NO_FREE_DESC:
ALLOC_SKB_TX_FAIL:
    dev_kfree_skb_any(skb);

    priv->tx_errors++;
    priv->tx_dropped++;

    return 0;
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
    if ( port < 0 )
        return -ENODEV;

    switch ( cmd )
    {
    case SIOCETHTOOL:
        return ethtool_ioctl(dev, ifr);
    case SET_VLAN_COS:
        {
            struct vlan_cos_req vlan_cos_req;

            if ( copy_from_user(&vlan_cos_req, ifr->ifr_data, sizeof(struct vlan_cos_req)) )
                return -EFAULT;
#if 0   //  TODO
            set_vlan_cos(&vlan_cos_req);
#endif
        }
        break;
    case SET_DSCP_COS:
        {
            struct dscp_cos_req dscp_cos_req;

            if ( copy_from_user(&dscp_cos_req, ifr->ifr_data, sizeof(struct dscp_cos_req)) )
                return -EFAULT;
#if 0   //  TODO
            set_dscp_cos(&dscp_cos_req);
#endif
        }
        break;
#if 0   //  TODO
    case ENABLE_VLAN_CLASSIFICATION:
        *ENETS_COS_CFG(port) |= ENETS_COS_CFG_VLAN_SET;     break;
    case DISABLE_VLAN_CLASSIFICATION:
        *ENETS_COS_CFG(port) &= ENETS_COS_CFG_VLAN_CLEAR;   break;
    case ENABLE_DSCP_CLASSIFICATION:
        *ENETS_COS_CFG(port) |= ENETS_COS_CFG_DSCP_SET;     break;
    case DISABLE_DSCP_CLASSIFICATION:
        *ENETS_COS_CFG(port) &= ENETS_COS_CFG_DSCP_CLEAR;   break;
    case VLAN_CLASS_FIRST:
        *ENETS_CFG(port) &= ENETS_CFG_FTUC_CLEAR;           break;
    case VLAN_CLASS_SECOND:
        *ENETS_CFG(port) |= ENETS_CFG_VL2_SET;              break;
    case PASS_UNICAST_PACKETS:
        *ENETS_CFG(port) &= ENETS_CFG_FTUC_CLEAR;           break;
    case FILTER_UNICAST_PACKETS:
        *ENETS_CFG(port) |= ENETS_CFG_FTUC_SET;             break;
    case KEEP_BROADCAST_PACKETS:
        *ENETS_CFG(port) &= ENETS_CFG_DPBC_CLEAR;           break;
    case DROP_BROADCAST_PACKETS:
        *ENETS_CFG(port) |= ENETS_CFG_DPBC_SET;             break;
    case KEEP_MULTICAST_PACKETS:
        *ENETS_CFG(port) &= ENETS_CFG_DPMC_CLEAR;           break;
    case DROP_MULTICAST_PACKETS:
        *ENETS_CFG(port) |= ENETS_CFG_DPMC_SET;             break;
#endif
    case ETH_MAP_PKT_PRIO_TO_Q:
        {
            struct ppe_prio_q_map cmd;

            if ( copy_from_user(&cmd, ifr->ifr_data, sizeof(cmd)) )
                return -EFAULT;

            if ( cmd.pkt_prio < 0 || cmd.pkt_prio >= NUM_ENTITY(g_eth_prio_queue_map[port]) )
                return -EINVAL;

            if ( cmd.qid < 0 )
                return -EINVAL;
            if ( cmd.qid >= ((g_wan_itf & (1 << port)) && (g_eth_wan_mode == 3) && g_wanqos_en ? g_wanqos_en : 4) )
                return -EINVAL;

            g_eth_prio_queue_map[port][cmd.pkt_prio] = cmd.qid;
        }
        break;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
}

//static int eth_change_mtu(struct net_device *dev, int new_mtu)
//{
//    printk(KERN_ERR __FILE__ ":%d:%s: not implemented\n", __LINE__, __FUNCTION__);
//
//    return 0;
//}

static void eth_tx_timeout(struct net_device *dev)
{
    unsigned long sys_flag;
    struct eth_priv_data *priv = netdev_priv(dev);
    //int port;

    //TODO:must restart the TX channels

    //port = get_eth_port(dev);
    priv->tx_errors++;

    local_irq_save(sys_flag);
    g_dma_device->tx_chan[2]->disable_irq(g_dma_device->tx_chan[2]);
    netif_wake_queue(g_eth_net_dev[0]);
    if ( !g_wanqos_en )
        netif_wake_queue(g_eth_net_dev[1]);
    local_irq_restore(sys_flag);

    return;
}

static INLINE int get_eth_port(struct net_device *dev)
{
    return dev == g_eth_net_dev[0] ? 0 : (dev == g_eth_net_dev[1] ? 1 : -1);
}

static INLINE int eth_xmit(struct sk_buff *skb, unsigned int port, int ch, int spid, int prio)
{
    struct sw_eg_pkt_header pkth = {0};
    int len;
    struct eth_priv_data *priv = NULL;

    len = skb->len <= ETH_MIN_TX_PACKET_LENGTH ? ETH_MIN_TX_PACKET_LENGTH : skb->len;

#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
    if ( g_mirror_netdev != NULL && (g_wan_itf & (1 << port)) )
    {
        struct sk_buff *new_skb = skb_clone(skb, GFP_ATOMIC);

        if ( new_skb != NULL )
        {
            new_skb->dev = g_mirror_netdev;
            dev_queue_xmit(new_skb);
        }
    }
#endif

    g_dma_device->current_tx_chan = ch;

    dump_skb(skb, DUMP_SKB_LEN, "eth_xmit - raw data", port, ch, 1, 0);

    if ( skb_headroom(skb) < sizeof(struct sw_eg_pkt_header) )
    {
        struct sk_buff *new_skb;

        new_skb = alloc_skb_tx(skb->len);
        if ( !new_skb )
            goto ETH_XMIT_DROP;
        else
        {
            skb_put(new_skb, skb->len + sizeof(struct sw_eg_pkt_header));
            memcpy(new_skb->data + sizeof(struct sw_eg_pkt_header), skb->data, skb->len);
            dev_kfree_skb_any(skb);
            skb = new_skb;
        }
    }
    else
        skb_push(skb, sizeof(struct sw_eg_pkt_header));
    len += sizeof(struct sw_eg_pkt_header);

    pkth.spid           = spid;
    pkth.dpid           = port;
    pkth.lrn_dis        = 0;
    pkth.class_en       = 1;
    pkth.class          = prio;
    if ( pkth.dpid < 2 )
        pkth.dpid_en    = g_egress_direct_forward_enable;

    memcpy(skb->data, &pkth, sizeof(struct sw_eg_pkt_header));

    if ( port < NUM_ENTITY(g_eth_net_dev))
    {
        g_eth_net_dev[port]->trans_start = jiffies;
        priv = netdev_priv(g_eth_net_dev[port]);
        priv->tx_packets++;
        priv->tx_bytes += len;
    }

    if ( dma_device_write(g_dma_device,
                          skb->data,
                          len,
                          skb)
         != len )
        goto ETH_XMIT_DROP;

    return 0;

ETH_XMIT_DROP:
    dev_kfree_skb_any(skb);
    if ( port < NUM_ENTITY(g_eth_net_dev) )
    {
        priv = netdev_priv(g_eth_net_dev[port]);
        priv->tx_errors++;
        priv->tx_dropped++;
    }
    return -1;
}

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
    int port;
    struct ethtool_cmd cmd;

    if ( copy_from_user(&cmd, ifr->ifr_data, sizeof(cmd)) )
        return -EFAULT;

    port = get_eth_port(dev);

    switch ( cmd.cmd )
    {
    case ETHTOOL_GSET:      /*  get hardware information        */
        {
            memset(&cmd, 0, sizeof(cmd));

#if 0   //  TODO
            cmd.supported   = SUPPORTED_Autoneg | SUPPORTED_TP | SUPPORTED_MII |
                              SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full |
                              SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full;
            cmd.port        = PORT_MII;
            cmd.transceiver = XCVR_EXTERNAL;
            cmd.phy_address = port;
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
#endif

            if ( copy_to_user(ifr->ifr_data, &cmd, sizeof(cmd)) )
                return -EFAULT;
        }
        break;
    case ETHTOOL_SSET:      /*  force the speed and duplex mode */
        {
            if ( !capable(CAP_NET_ADMIN) )
                return -EPERM;

            if ( cmd.autoneg == AUTONEG_ENABLE )
            {
                /*  set property and start autonegotiation                                  */
                /*  have to set mdio advertisement register and restart autonegotiation     */
                /*  which is a very rare case, put it to future development if necessary.   */
            }
            else
            {
#if 0   //  TODO
                /*  set property without autonegotiation    */
                *ETOP_MDIO_CFG &= ~ETOP_MDIO_CFG_UMM(port, 1);

                /*  set speed   */
                if ( cmd.speed == SPEED_10 )
                    ENET_MAC_CFG_SPEED_10M(port);
                else if ( cmd.speed == SPEED_100 )
                    ENET_MAC_CFG_SPEED_100M(port);

                /*  set duplex  */
                if ( cmd.duplex == DUPLEX_HALF )
                    ENET_MAC_CFG_DUPLEX_HALF(port);
                else if ( cmd.duplex == DUPLEX_FULL )
                    ENET_MAC_CFG_DUPLEX_FULL(port);

                ENET_MAC_CFG_LINK_OK(port);
#endif
            }
        }
        break;
    case ETHTOOL_GDRVINFO:  /*  get driver information          */
        {
            struct ethtool_drvinfo info;
            char str[32];

            memset(&info, 0, sizeof(info));
            strncpy(info.driver, "Danube Eth Driver (A4)", sizeof(info.driver) - 1);
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
#if 0   //  TODO
        *ETOP_MDIO_CFG |= ETOP_MDIO_CFG_SMRST(port) | ETOP_MDIO_CFG_UMM(port, 1);
#endif
        break;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
}

#if 0   //  TODO
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
#endif

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
    if ( g_data_led_trigger != NULL )
        ifx_led_trigger_activate(g_data_led_trigger);
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

    for ( num_of_virt_queue = 0, sw_tx_queue_table = new_sw_tx_queue_table, sw_tx_queue = 0;
          num_of_virt_queue < NUM_ENTITY(virt_queue_to_sw_tx_queue_map) && sw_tx_queue_table != 0;
          sw_tx_queue_table >>= 1, sw_tx_queue++ )
        if ( (sw_tx_queue_table & 0x01) )
            virt_queue_to_sw_tx_queue_map[num_of_virt_queue++] = sw_tx_queue;

    g_atm_priv_data.sw_tx_queue_table |= 1 << new_sw_tx_queue;
    g_atm_priv_data.connection[conn].sw_tx_queue_table = new_sw_tx_queue_table;

    for ( i = 0; i < 8; i++ )
    {
        sw_tx_queue = g_atm_priv_data.connection[conn].prio_queue_map[i];
        virt_queue = sw_tx_queue_to_virt_queue_map[sw_tx_queue];
        sw_tx_queue = virt_queue_to_sw_tx_queue_map[virt_queue];
        g_atm_priv_data.connection[conn].prio_queue_map[i] = sw_tx_queue;
    }

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
            g_atm_priv_data.connection[conn].prio_queue_map[i] = sw_tx_queue;

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
    register int i;

    HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 0;
    /*  idle for a while to finish running HTU search   */
    for ( i = 0; i < IDLE_CYCLE_NUMBER; i++ );
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

    skb = dev_alloc_skb(len + DMA_ALIGNMENT * 2);
    if ( skb )
    {
        ATM_SKB(skb)->vcc = NULL;
        skb_reserve(skb, (~((u32)skb->data + (DMA_ALIGNMENT - 1)) & (DMA_ALIGNMENT - 1)) + DMA_ALIGNMENT);
        ASSERT(((u32)skb->data & (DMA_ALIGNMENT - 1)) == 0, "skb->data (%#x) is not 8 DWORDS aligned", (u32)skb->data);
        *((u32 *)skb->data - 1) = (u32)skb;
#ifndef CONFIG_MIPS_UNCACHED
        dma_cache_wback((u32)skb->data - sizeof(u32), sizeof(u32));
#endif
    }

    return skb;
}

static INLINE struct sk_buff *__get_skb_pointer(unsigned int dataptr, const char *func_name, unsigned int line_num)
{
    unsigned int skb_dataptr;
    struct sk_buff *skb;

    //  usually, CPE memory is less than 256M bytes
    //  so NULL means invalid pointer
    if ( dataptr == 0 ) {
        dbg("dataptr is 0, it's supposed to be invalid pointer");
        return NULL;
    }

    skb_dataptr = (dataptr - 4) | KSEG1;
    skb = *(struct sk_buff **)skb_dataptr;

#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
    {
        int i;

        for ( i = 0; i < NUM_ENTITY(g_dbg_skb_swap_pool) && g_dbg_skb_swap_pool[i] != skb; i++ );
        if ( i == NUM_ENTITY(g_dbg_skb_swap_pool) || (unsigned int)skb < KSEG0 )
        {
            err("%s:%d: skb (0x%08x) not in g_dbg_skb_swap_pool", func_name, line_num, (unsigned int)skb);
        }
        else
            g_dbg_skb_swap_pool[i] = NULL;
    }
#endif

    ASSERT((unsigned int)skb >= KSEG0, "%s:%d: invalid skb - skb = %#08x, dataptr = %#08x", func_name, line_num, (unsigned int)skb, dataptr);
    ASSERT((((unsigned int)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1))) | KSEG1) == (dataptr | KSEG1), "%s:%d: invalid skb - skb = %#08x, skb->data = %#08x, dataptr = %#08x", func_name, line_num, (unsigned int)skb, (unsigned int)skb->data, dataptr);

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

    skb = alloc_skb_tx(size < DMA_PACKET_SIZE ? DMA_PACKET_SIZE : size);
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
    int i;

    ASSERT((u32)skb > 0x80000000, "atm_free_tx_skb_vcc: skb = %08X", (u32)skb);

    vcc = ATM_SKB(skb)->vcc;

    if ( (unsigned int)vcc >= KSEG0 && (unsigned int)vcc->pop >= KSEG0 )
    {
        for ( i = 0; i < ATM_PORT_NUMBER; i++ )
            if ( vcc->dev == g_atm_priv_data.port[i].dev )
                break;

        if ( i < ATM_PORT_NUMBER && find_vcc(vcc) >= 0 )
        {
            ASSERT(atomic_read(&skb->users) != 0, "atm_free_tx_skb_vcc(vcc->pop): skb->users == 0, skb = %08X", (u32)skb);
            atomic_add(skb->truesize, &sk_atm(vcc)->sk_wmem_alloc);
            vcc->pop(vcc, skb);
            return;
        }
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

//  xuliang: legacy code, may be re-enabled due to bad performance when DMA TX channel is enabled without valid descriptor
//    if ( (mailbox_signal & DMA_TX_CH2_SIG) )
//    {
//        g_mailbox_signal_mask &= ~DMA_TX_CH2_SIG;
//        g_dma_device->tx_chan[0]->open(g_dma_device->tx_chan[0]);
//        g_dma_device->tx_chan[1]->open(g_dma_device->tx_chan[1]);
//        *MBOX_IGU1_IER = g_mailbox_signal_mask;
//    }

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
                if ( desc->own )    //  PPE hold
                    break;
                skb = g_wan_rx1_desc_skb[g_wan_rx1_desc_pos];
                if ( ++g_wan_rx1_desc_pos == WAN_RX_DESC_NUM(1) )
                    g_wan_rx1_desc_pos = 0;

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
                skb->tail = skb->data + CELL_SIZE;
                skb->len = CELL_SIZE;
                dump_skb(skb, DUMP_SKB_LEN, "mailbox_irq_handler - OAM", 7, 0, 0, 0);
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
                desc->own = 1;
            }
            *MBOX_IGU1_ISRC = WAN_RX_SIG(1);
        } while ( !desc->own );   //  check after clean, if MIPS hold, repeat
    }

    if ( (mailbox_signal & CPU_TO_WAN_SWAP_SIG) )
    {
        struct sk_buff *new_skb;
        volatile struct tx_descriptor *desc = &CPU_TO_WAN_SWAP_DESC_BASE[g_cpu_to_wan_swap_desc_pos];
        struct tx_descriptor reg_desc = {0};
        struct sk_buff *skb_to_free;

        while ( desc->own == 0 )
        {
            new_skb = alloc_skb_rx();
            if ( new_skb == NULL )
                break;

#ifndef CONFIG_MIPS_UNCACHED
            /*  invalidate cache    */
            dma_cache_inv((unsigned long)new_skb->data, DMA_PACKET_SIZE);
#endif

            if ( ++g_cpu_to_wan_swap_desc_pos == CPU_TO_WAN_SWAP_DESC_NUM )
                g_cpu_to_wan_swap_desc_pos = 0;

            /*  free previous skb   */
            skb_to_free = get_skb_pointer(desc->dataptr);
            if ( skb_to_free != NULL )
                dev_kfree_skb_any(skb_to_free);
            put_skb_to_dbg_pool(new_skb);

            /*  update descriptor   */
            reg_desc.dataptr = (unsigned int)new_skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1));
            reg_desc.own     = 1;

            /*  write discriptor to memory  */
            *((volatile unsigned int *)desc + 1) = *((unsigned int *)&reg_desc + 1);
            wmb();
            *(volatile unsigned int *)desc = *(unsigned int *)&reg_desc;

            desc = &CPU_TO_WAN_SWAP_DESC_BASE[g_cpu_to_wan_swap_desc_pos];
        }
    }

    return IRQ_HANDLED;
}

static INLINE void turn_on_dma_rx(int mask)
{
    int i;

    if ( !g_f_dma_opened )
    {
        ASSERT((u32)g_dma_device >= 0x80000000, "g_dma_device = 0x%08X", (u32)g_dma_device);

        for ( i = 0; i < g_dma_device->max_rx_chan_num; i++ )
        {
            ASSERT((u32)g_dma_device->rx_chan[i] >= 0x80000000, "g_dma_device->rx_chan[%d] = 0x%08X", i, (u32)g_dma_device->rx_chan[i]);
            ASSERT(g_dma_device->rx_chan[i]->control == IFX_DMA_CH_ON, "g_dma_device->rx_chan[i]->control = %d", g_dma_device->rx_chan[i]->control);

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
    }
    g_f_dma_opened |= 1 << mask;
}

static INLINE void turn_off_dma_rx(int mask)
{
    int i;

    g_f_dma_opened &= ~(1 << mask);
    if ( !g_f_dma_opened )
        for ( i = 0; i < g_dma_device->max_rx_chan_num; i++ )
            g_dma_device->rx_chan[i]->close(g_dma_device->rx_chan[i]);
}

static u8 *dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
    u8 *buf;
    struct sk_buff *skb;

    skb = alloc_skb_rx();
    if ( !skb )
        return NULL;

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
        dev_kfree_skb_any((struct sk_buff *)opt);
    else if ( dataptr )
        kfree(dataptr);

    return 0;
}

#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
  static unsigned int count_buf_a[512];
  static unsigned int count_buf_a_pos = 0;
  unsigned int count_rx_a_start, count_rx_a_end;
  static unsigned int count_buf_b[512];
  static unsigned int count_buf_b_pos = 0;
  unsigned int count_rx_b_start, count_rx_b_end;
#endif

static int dma_int_handler(struct dma_device_info *dma_dev, int status)
{
    int ret = 0;
    unsigned long sys_flag;
#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
    int i;
#endif
#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
    unsigned int count_rx_start = 0, count_rx_end = 0;
    static unsigned int count_buf[512];
    static unsigned int count_buf_pos = 0;
#endif

    switch ( status )
    {
    case RCV_INT:
#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
        count_rx_a_start = count_rx_a_end = 0;
        count_rx_b_start = count_rx_b_end = 0;
        local_irq_save(sys_flag);
        count_rx_start = read_c0_count();
#endif
        ret = dma_rx_int_handler(dma_dev);
#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
        count_rx_end = read_c0_count();
        local_irq_restore(sys_flag);
        //printk("%s: rx %d cycles\n", __func__, count_rx_end - count_rx_start);
        count_buf[count_buf_pos++] = count_rx_end - count_rx_start;
        if ( count_buf_pos >= NUM_ENTITY(count_buf) )
        {
            for ( count_buf_pos = 0, count_rx_end = 0; count_buf_pos < NUM_ENTITY(count_buf); count_buf_pos++ )
                count_rx_end += count_buf[count_buf_pos];
            count_rx_end = (count_rx_end + NUM_ENTITY(count_buf) / 2) / NUM_ENTITY(count_buf);
            printk("%s: average rx (over %d packets) %d cycles\n", __func__, NUM_ENTITY(count_buf), count_rx_end);

            count_buf_pos = 0;
        }

        if ( count_rx_a_start != count_rx_a_end )
        {
            count_buf_a[count_buf_a_pos++] = count_rx_a_end - count_rx_a_start;
            if ( count_buf_a_pos >= NUM_ENTITY(count_buf_a) )
            {
                for ( count_buf_a_pos = 0, count_rx_a_end = 0; count_buf_a_pos < NUM_ENTITY(count_buf_a); count_buf_a_pos++ )
                    count_rx_a_end += count_buf_a[count_buf_a_pos];
                count_rx_a_end = (count_rx_a_end + NUM_ENTITY(count_buf_a) / 2) / NUM_ENTITY(count_buf_a);
                printk("%s: average rx decap (over %d packets) %d cycles\n", __func__, NUM_ENTITY(count_buf_a), count_rx_a_end);

                count_buf_a_pos = 0;
            }
        }
        if ( count_rx_b_start != count_rx_b_end )
        {
            count_buf_b[count_buf_b_pos++] = count_rx_b_end - count_rx_b_start;
            if ( count_buf_b_pos >= NUM_ENTITY(count_buf_b) )
            {
                for ( count_buf_b_pos = 0, count_rx_b_end = 0; count_buf_b_pos < NUM_ENTITY(count_buf_b); count_buf_b_pos++ )
                    count_rx_b_end += count_buf_b[count_buf_b_pos];
                count_rx_b_end = (count_rx_b_end + NUM_ENTITY(count_buf_b) / 2) / NUM_ENTITY(count_buf_b);
                printk("%s: average rx push (over %d packets) %d cycles\n", __func__, NUM_ENTITY(count_buf_b), count_rx_b_end);

                count_buf_b_pos = 0;
            }
        }
#endif
        break;
    case TX_BUF_FULL_INT:
        if ( g_dma_device->current_tx_chan == 2 )
        {
            dbg("eth0/1 TX buffer full!");
            local_irq_save(sys_flag);
            if ( !g_wanqos_en )
            {
                if ( g_eth_net_dev[0]->trans_start < g_eth_net_dev[1]->trans_start )
                {
                    g_eth_net_dev[1]->trans_start = jiffies;
                    g_eth_net_dev[0]->trans_start = jiffies + 1;
                }
                else
                {
                    g_eth_net_dev[0]->trans_start = jiffies;
                    g_eth_net_dev[1]->trans_start = jiffies + 1;
                }
                netif_stop_queue(g_eth_net_dev[1]);
            }
            netif_stop_queue(g_eth_net_dev[0]);
            if ( g_dma_device->tx_chan[2]->control == IFX_DMA_CH_ON )
                g_dma_device->tx_chan[2]->enable_irq(g_dma_device->tx_chan[2]);
            local_irq_restore(sys_flag);
        }
        else if ( g_dma_device->current_tx_chan == 3 )
        {
            dbg("directpath TX (CPU->PPE) buffer full!");
            local_irq_save(sys_flag);
            if ( !g_directpath_dma_full )
            {
                g_directpath_dma_full = 1;
                if ( g_dma_device->tx_chan[3]->control == IFX_DMA_CH_ON )
                    g_dma_device->tx_chan[3]->enable_irq(g_dma_device->tx_chan[3]);
#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
                for ( i = 0; i < NUM_ENTITY(g_ppe_directpath_data); i++ )
                    if ( (g_ppe_directpath_data[i].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) && g_ppe_directpath_data[i].callback.stop_tx_fn )
                        g_ppe_directpath_data[i].callback.stop_tx_fn(g_ppe_directpath_data[i].netif);
#endif
            }
            local_irq_restore(sys_flag);
        }
        else
        {
            err("incorrect DMA TX channel: %d (0 - 1 is reserved for fast path)", g_dma_device->current_tx_chan);
        }
        break;
    case TRANSMIT_CPT_INT:
        if ( g_dma_device->current_tx_chan == 2 )
        {
            dbg("eth0/1 TX buffer released!");
            local_irq_save(sys_flag);
            g_dma_device->tx_chan[2]->disable_irq(g_dma_device->tx_chan[2]);
            netif_wake_queue(g_eth_net_dev[0]);
            if ( !g_wanqos_en )
                netif_wake_queue(g_eth_net_dev[1]);
            local_irq_restore(sys_flag);
        }
        else if ( g_dma_device->current_tx_chan == 3 )
        {
            dbg("directpath TX (CPU->PPE) buffer released");
            local_irq_save(sys_flag);
            if ( g_directpath_dma_full )
            {
                g_directpath_dma_full = 0;
                g_dma_device->tx_chan[3]->disable_irq(g_dma_device->tx_chan[3]);
#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
                for ( i = 0; i < NUM_ENTITY(g_ppe_directpath_data); i++ )
                    if ( (g_ppe_directpath_data[i].flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) && g_ppe_directpath_data[i].callback.start_tx_fn )
                        g_ppe_directpath_data[i].callback.start_tx_fn(g_ppe_directpath_data[i].netif);
#endif
            }
            local_irq_restore(sys_flag);
#if defined(ENABLE_DIRECTPATH_TX_QUEUE) && ENABLE_DIRECTPATH_TX_QUEUE
            send_directpath_tx_queue();
#endif
        }
        else
        {
            err("incorrect DMA TX channel: %d (0 - 1 is reserved for fast path)", g_dma_device->current_tx_chan);
        }
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
    u32 header_val;
    u32 off;
    int conn;
    struct atm_vcc *vcc;
    struct eth_priv_data *priv = NULL;

    len = dma_device_read(dma_dev, &buf, (void **)&skb);
    if ( !skb )
        return 0;

    ASSERT(buf != NULL, "skb = %08x, buf = %08x", (unsigned int)skb, (unsigned int)buf);
    ASSERT((u32)skb >= 0x80000000, "can not restore skb pointer (ch %d) --- skb = 0x%08X", dma_dev->current_rx_chan, (u32)skb);
    ASSERT(skb->data == buf && skb->tail == buf, "skb->data (%08X) or skb->tail (%08X) is corrupted", (u32)skb->data, (u32)skb->tail);
    ASSERT(len <= (u32)skb->end - (u32)skb->data, "pakcet is too large: %d", len);
    ASSERT(*(struct sk_buff **)((((u32)buf | KSEG1) & ~0x1F) - 4) == skb, "address is not match: skb = %08X, *(struct sk_buff **)((((u32)buf | KSEG1) & ~0x0F) - 4) = %08X", (u32)skb, *(u32 *)((((u32)buf | KSEG1) & ~0x1F) - 4));

    dump_flag_header(g_fwcode, (struct flag_header *)skb->data, __FUNCTION__, dma_dev->current_rx_chan, 0);

    header = (struct flag_header *)skb->data;
    header_val = *((u32 *)skb->data + 1);

    //  pl_byteoff
    off = header_val & ((1 << 6) - 1);

    len -= sizeof(struct flag_header) + off;

    skb->data += sizeof(struct flag_header) + off;
    skb->tail =  skb->data + len;
    skb->len  =  len;

    dump_skb(skb, DUMP_SKB_LEN, "dma_rx_int_handler raw data", (header_val >> 21) & 0x07, dma_dev->current_rx_chan, 0, 0);

    //  implement indirect fast path
    if ( (header_val & (1 << 6)) && (header_val & 0xFF000000) )    //  acc_done == 1 && dest_list != 0
    {
        //  Current implementation send packet to highest priority dest only (0 -> 7, 0 is highest)
        //  2 is CPU, so we ignore it here
        if ( (header_val & (1 << 24)) )         //  0 - eth0
        {
            eth_xmit(skb, 0, 2, 2, header->dslwan_qid);
            return 0;
        }
        else if ( (header_val & (1 << 25)) )    //  1 - eth1
        {
            eth_xmit(skb, 1, 2, 2, header->dslwan_qid);
            return 0;
        }
        else if ( (header_val & (1 << 31)) )    //  7 - DSL, 6 - res
        {
            unsigned long sys_flag;
            volatile struct tx_descriptor *desc;
            struct tx_descriptor reg_desc;
            struct sk_buff *skb_to_free;
            int byteoff;

            conn = (header_val >> 16) & 0x07;

#if defined(ENABLE_DFE_LOOPBACK_TEST_INDIRECT) && ENABLE_DFE_LOOPBACK_TEST_INDIRECT
            {
                unsigned char *p = (unsigned char *)skb->data;

                if ( WRX_QUEUE_CONFIG(conn)->mpoa_type < 2 )    //  EoA
                {
                    unsigned char mac[6];

                    //  mac
                    memcpy(mac, p, 6);
                    memcpy(p, p + 6, 6);
                    memcpy(p + 6, mac, 6);
  #ifndef CONFIG_MIPS_UNCACHED
                    dma_cache_wback((unsigned long)p, 12);
  #endif
                }
                if ( header->mpoa_type != 2 )   //  EoA or IPoA
                {
                    unsigned char ip[4];
                    unsigned char port[2];

                    //  IP
                    p += header->ip_offset + 8;
                    memcpy(ip, p + 4, 4);
                    memcpy(p + 4, p + 8, 4);
                    memcpy(p + 8, ip, 4);

                    //  port
                    if ( p[1] == 0x06 || p[1] == 0x11 ) //  TCP or UDP
                    {
                        memcpy(port, p + 12, 2);
                        memcpy(p + 12, p + 14, 2);
                        memcpy(p + 14, port, 2);
                    }
  #ifndef CONFIG_MIPS_UNCACHED
                    dma_cache_wback((unsigned long)p, 22);
  #endif
                }
            }
#endif

            byteoff = (u32)skb->data & (DMA_ALIGNMENT - 1);
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
            ASSERT((reg_desc.dataptr & (DMA_ALIGNMENT - 1)) == 0, "reg_desc.dataptr (0x%#x) must be 8 DWORDS aligned", reg_desc.dataptr);
            skb_to_free = get_skb_pointer(reg_desc.dataptr);
            atm_free_tx_skb_vcc_delayed(skb_to_free);
            put_skb_to_dbg_pool(skb);

            /*  update descriptor   */
            reg_desc.dataptr    = (u32)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1));  //  byte address
            reg_desc.byteoff    = byteoff;
            reg_desc.datalen    = skb->len;
            reg_desc.mpoa_pt    = 0;    //  firmware apply MPoA
            reg_desc.mpoa_type  = WRX_QUEUE_CONFIG(conn)->mpoa_type;
            reg_desc.pdu_type   = 0;    //  AAL5
            reg_desc.qid        = conn;
            reg_desc.own        = 0;
            reg_desc.c          = 0;
            reg_desc.sop = reg_desc.eop = 1;

#if 0
            /*  update MIB  */
            UPDATE_VCC_STAT(conn, tx_packets, 1);
            UPDATE_VCC_STAT(conn, tx_bytes, skb->len);
#endif

            dump_skb(skb, DUMP_SKB_LEN, "ppe_send", 7, 0, 1, 0);

            /*  write discriptor to memory and write back cache */
            *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
            *(volatile u32 *)desc = *(u32 *)&reg_desc;

            adsl_led_flash();

            return 0;
        }
        else
        {
            //  fill MAC info into skb, in case the driver hooked to direct path need such info
            skb->dev = g_eth_net_dev[0];
            skb->protocol = eth_type_trans(skb, g_eth_net_dev[0]);
            skb_push(skb, ETH_HLEN);

            for ( off = 27; off < 30; off++ )
            {
                if ( (header_val & (1 << off)) )    //  3...5
                {
                    int if_id = off - 27;

                    if ( (g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
                    {
                        int i;

                        //  detect ATM port and remove the tag having PVC info
                        if ( header->src_itf == 7 && header->is_udp == 0 && header->is_tcp == 0
                            && skb->data[12] == 0x81 && skb->data[13] == 0x00
                            && (skb->data[14] & 0x0F) == 0x0F && (skb->data[15] & 0xF0) == 0xF0 )
                        {
                            //  remove outer VLAN tag
                            for ( i = 11; i >= 0; i-- )
                                skb->data[i + 4] = skb->data[i];
                            skb_pull(skb, 4);
                        }

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

    off = (header_val >> 21) & 0x07;
    switch ( off )
    {
    case 0: //  MII0
    case 1: //  MII1
        priv = netdev_priv(g_eth_net_dev[off]);
        if ( netif_running(g_eth_net_dev[off]) )
        {
            //  do not need to remove CRC
            //skb->tail -= ETH_CRC_LENGTH;
            //skb->len  -= ETH_CRC_LENGTH;

            {
                skb->dev = g_eth_net_dev[off];
                skb->protocol = eth_type_trans(skb, g_eth_net_dev[off]);
            }

            if ( netif_rx(skb) == NET_RX_DROP )
            {
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
    case 6: //  ATM (IPoA, PPPoA)
    case 7: //  ATM (EoA)
        if ( off == 6 )
            conn = skb->data[11] & 0x3F;    //  QID: 6 bits
        else
        {
            int i;

            ASSERT(skb->data[12] == 0x81 && skb->data[13] == 0x00, "EoA traffic: no outer VLAN for PVCs differentiation");  //  outer VLAN
#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
            if ( skb->data[14] != 0x0F || (skb->data[15] & 0xF0) != 0xF0 )
            {
                unsigned short vlan_id = ((unsigned short)skb->data[14] << 8) | (unsigned short)skb->data[15];
                for ( i = 0; i < NUM_ENTITY(g_dsl_vlan_qid_map); i += 2 )
                    if ( g_dsl_vlan_qid_map[i] == vlan_id )
                    {
                        skb->data[15] = (unsigned char)g_dsl_vlan_qid_map[i + 1];
                        break;
                    }
                ASSERT(i < NUM_ENTITY(g_dsl_vlan_qid_map), "search DSL VLAN table fail");
            }
#endif
            conn = skb->data[15] & 0x0F;
            //  remove outer VLAN tag
            for ( i = 11; i >= 0; i-- )
                skb->data[i + 4] = skb->data[i];
            skb_pull(skb, 4);
        }
        if ( conn < 0 || conn >= ATM_QUEUE_NUMBER )
        {
            dbg("DMA channel %d, ATM conn (%d) is out of range 0 - %d", off, conn, ATM_QUEUE_NUMBER - 1);
            break;
        }
        if ( (vcc = g_atm_priv_data.connection[conn].vcc) != NULL )
        {
            if ( atm_charge(vcc, skb->truesize) )
            {
                //err("reminder: how to handle raw packet without flag aal5_raw?");
                //ASSERT(vcc->qos.aal == ATM_AAL5 || (header_val & (1 << 23)), "ETH packet comes in from ATM port non-AAL5 VC");

                ATM_SKB(skb)->vcc = vcc;

                //if ( (header_val & (1 << 23)) )
                //{
                //    /*  AAL 5 raw packet    */
                //    //skb->tail -= ETH_CRC_LENGTH;
                //    //skb->len  -= ETH_CRC_LENGTH;
                //}
                //else
                {
#if defined(ENABLE_DFE_LOOPBACK_TEST) && ENABLE_DFE_LOOPBACK_TEST
  #if 0 //  to avoid switch learning problem, this code is moved to send path
                    //  swap MAC and IP
                    switch ( WRX_QUEUE_CONFIG(conn)->mpoa_type )
                    {
                    case 0: //  EoA w/o FCS
                    case 1: //  EoA w FCS
                        {
                            unsigned char *pmac;
                            unsigned char *pip;
                            unsigned char *pport;
                            unsigned char mac[6];
                            unsigned char ip[6];
                            unsigned char port[2];

                            pmac = skb->data;
                            memcpy(mac, pmac, 6);
                            memcpy(pmac, pmac + 6, 6);
                            memcpy(pmac + 6, mac, 6);

                            pip = pmac + 12;
                            while ( pip[0] == 0x81 && pip[1] == 0x00 )  //  VLAN tag
                                pip += 4;
                            if ( pip[0] == 0x08 && pip[1] == 0x00 )     //  IP
                            {
                                pport = pip + 11;

                                pip += 14;
                                //  swap IP bring in IP checksum error
                                memcpy(ip, pip, 4);
                                memcpy(pip, pip + 4, 4);
                                memcpy(pip + 4, ip, 4);

                                if ( pport[0] == 0x06 || pport[0] == 0x11 ) //  TCP or UDP
                                {
                                    pport = pip + 8;
                                    memcpy(port, pport, 2);
                                    memcpy(pport, pport + 2, 2);
                                    memcpy(pport + 2, port, 2);
                                }
                            }
                        }
                        break;
                    case 3: //  IPoA
                        {
                            unsigned char *pip;
                            unsigned char *pport;
                            unsigned char ip[4];
                            unsigned char port[2];

                            pip = skb->data + header->ip_offset + 12;
                            memcpy(ip, pip, 4);
                            memcpy(pip, pip + 4, 4);
                            memcpy(pip + 4, ip, 4);

                            pport = pip - 3;
                            if ( pport[0] == 0x06 || pport[0] == 0x11 ) //  TCP or UDP
                            {
                                pport = pip + 8;
                                memcpy(port, pport, 2);
                                memcpy(pport, pport + 2, 2);
                                memcpy(pport + 2, port, 2);
                            }
                        }
                        break;
                    }
  #endif
#endif
#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
                    count_rx_a_start = read_c0_count();
#endif
                    /*  ETH packet, need recover ATM encapsulation  */
                    if ( WRX_QUEUE_CONFIG(conn)->mpoa_mode )
                    {
                        unsigned int proto_type;

                        /*  LLC */
                        switch ( WRX_QUEUE_CONFIG(conn)->mpoa_type )
                        {
                        case 0: //  EoA w/o FCS
                            ASSERT(skb_headroom(skb) >= 10, "not enough skb headroom (LLC EoA w/o FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (LLC EoA w/o FCS)");
                            skb->data -= 10;
                            skb->len  += 10;
                            //skb->tail -= ETH_CRC_LENGTH;
                            //skb->len  += 10 - ETH_CRC_LENGTH;
                            ((u32 *)skb->data)[0] = 0xAAAA0300;
                            ((u32 *)skb->data)[1] = 0x80C20007;
                            ((u16 *)skb->data)[4] = 0x0000;
                            break;
                        case 1: //  EoA w FCS
                            ASSERT(skb_headroom(skb) >= 10, "not enough skb headroom (LLC EoA w FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (LLC EoA w FCS)");
                            skb->data -= 10;
                            skb->len  += 10;
                            ((u32 *)skb->data)[0] = 0xAAAA0300;
                            ((u32 *)skb->data)[1] = 0x80C20001;
                            ((u16 *)skb->data)[4] = 0x0000;
                            break;
                        case 2: //  PPPoA
                            switch ( (proto_type = ntohs(*(u16 *)(skb->data + 12))) )
                            {
                            case 0x0800: proto_type = 0x0021; break;
                            case 0x86DD: proto_type = 0x0057; break;
                            }
                            ASSERT(header->ip_offset - 6 >= 0 || skb_headroom(skb) >= 6 - header->ip_offset, "not enough skb headroom (LLC PPPoA)");
                            skb->data += 0x0E - 6;
                            skb->len  -= 0x0E - 6;
                            //skb->tail -= ETH_CRC_LENGTH;
                            //skb->len  -= 0x0E - 6 + ETH_CRC_LENGTH;
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (LLC PPPoA)");
                            ((u32 *)skb->data)[0] = 0xFEFE03CF;
                            ((u16 *)skb->data)[2] = (u16)proto_type;
                            break;
                        case 3: //  IPoA
                            ASSERT(header->ip_offset - 8 >= 0 || skb_headroom(skb) >= 8 - header->ip_offset, "not enough skb headroom (LLC IPoA)");
                            skb->data += header->ip_offset - 8;
                            skb->len  -= header->ip_offset - 8;
                            //skb->tail -= ETH_CRC_LENGTH;
                            //skb->len  -= header->ip_offset - 8 + ETH_CRC_LENGTH;
                            ASSERT(((u32)skb->data & 0x03) == 0, "not aligned data pointer (LLC IPoA)");
                            ((u32 *)skb->data)[0] = 0xAAAA0300;
                            //((u32 *)skb->data)[1] = header->is_ipv4 ? 0x00000800 : 0x000086DD;
                            ((u16 *)skb->data)[2] = 0x0000;
                            break;
                        }
                    }
                    else
                    {
                        unsigned int proto_type;

                        /*  VC-mux  */
                        switch ( WRX_QUEUE_CONFIG(conn)->mpoa_type )
                        {
                        case 0: //  EoA w/o FCS
                            ASSERT(skb_headroom(skb) >= 2, "not enough skb headroom (VC-mux EoA w/o FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (VC-mux EoA w/o FCS)");
                            skb->data -= 2;
                            skb->len  += 2;
                            //skb->tail -= ETH_CRC_LENGTH;
                            //skb->len  += 2 - ETH_CRC_LENGTH;
                            *(u16 *)skb->data = 0x0000;
                            break;
                        case 1: //  EoA w FCS
                            ASSERT(skb_headroom(skb) >= 2, "not enough skb headroom (VC-mux EoA w FCS)");
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (VC-mux EoA w FCS)");
                            skb->data -= 2;
                            skb->len  += 2;
                            *(u16 *)skb->data = 0x0000;
                            break;
                        case 2: //  PPPoA
                            switch ( (proto_type = ntohs(*(u16 *)(skb->data + 12))) )
                            {
                            case 0x0800: proto_type = 0x0021; break;
                            case 0x86DD: proto_type = 0x0057; break;
                            }
                            ASSERT(header->ip_offset - 2 >= 0 || skb_headroom(skb) >= 2 - header->ip_offset, "not enough skb headroom (VC-mux PPPoA)");
                            skb->data += 0x0E - 2;
                            skb->len  -= 0x0E - 2;
                            //skb->tail -= ETH_CRC_LENGTH;
                            //skb->len  -= 0x0E - 2 + ETH_CRC_LENGTH;
                            ASSERT(((u32)skb->data & 0x03) == 2, "not aligned data pointer (VC-mux PPPoA)");
                            *(u16 *)skb->data = (u16)proto_type;
                            break;
                        case 3: //  IPoA
                            skb->data += header->ip_offset;
                            skb->len  -= header->ip_offset;
                            //skb->tail -= ETH_CRC_LENGTH;
                            //skb->len  -= header->ip_offset + ETH_CRC_LENGTH;
                            break;
                        }
                    }
                    dump_skb(skb, DUMP_SKB_LEN, "dma_rx_int_handler AAL5 encapsulated", off, dma_dev->current_rx_chan, 0, 0);
#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
                    count_rx_a_end = read_c0_count();
#endif
                }

                g_atm_priv_data.connection[conn].access_time = current_kernel_time();

                adsl_led_flash();

#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
                count_rx_b_start = read_c0_count();
#endif
                vcc->push(vcc, skb);
#if defined(ENABLE_RX_PROFILE) && ENABLE_RX_PROFILE
                count_rx_b_end = read_c0_count();
#endif

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
                dbg("inactive qid %d", conn);

                if ( vcc->stats )
                    atomic_inc(&vcc->stats->rx_drop);

                if ( vcc->qos.aal == ATM_AAL5 )
                    g_atm_priv_data.wrx_drop_pdu++;

                UPDATE_VCC_STAT(conn, rx_dropped, 1);
            }
        }
        break;
    case 3:     //  WLAN
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

#if AR10_EMULATION

enum{
    MII_PHY_MODE = 0,
    MII_MAC_MODE,
    RMII_PHY_MODE,
    RMII_MAC_MODE,
    RGMII_ONLY_MODE,
};

static u8 xMii_interface_mode[6] = {
					RGMII_ONLY_MODE,
					RGMII_ONLY_MODE,
					RGMII_ONLY_MODE,
					MII_MAC_MODE,
					RGMII_ONLY_MODE,
					RGMII_ONLY_MODE,
};

//for emulation only: in venus, there is not real phy connection. It's mac to mac connection.
//So must force link up and set the link rate
static INLINE void arx_ethsw_port_enable(int port)
{
    if(port < 0 || port > 5)
        return;

    *PHY_ADDR_REG(port) &= ~(0x1f<<11);
    if (xMii_interface_mode[port] == RGMII_ONLY_MODE)
		*PHY_ADDR_REG(port) |= (0x2<<11); // 1G data rate
	*PHY_ADDR_REG(port) |= (1<<13);// Link Up

    return;
}
#endif


static INLINE void start_cpu_port(void)
{
    int i;

    *DS_RXCFG |= 0x00008000;    //  enable DSRX
    *DM_RXCFG |= 0x80000000;    //  enable DMRX

    *FDMA_PCTRL_REG(6) |= 0x01; //  enable port

    for ( i = 0; i < 6; i++ )
        IFX_REG_W32_MASK(0, 1, SDMA_PCTRL_REG(i));  //  start port 0 - 5
}

static INLINE void init_internal_tantos_qos_setting(void)
{
}

static INLINE void init_internal_tantos(void)
{
    int i;

    *GLOB_CTRL_REG = 1 << 15;   //  enable Switch

    for ( i = 0; i < 7; i++ )
        sw_clr_rmon_counter(i);

    *FDMA_PCTRL_REG(6) = (*FDMA_PCTRL_REG(6) | 0x02) & ~0x01;   //  insert special tag and disable port

    *PMAC_HD_CTL_REG = 0x01DC;  //  PMAC Head

    for ( i = 6; i < 12; i++ )
        *PCE_PCTRL_REG(i, 0) |= 1 << 11;    //  ingress special tag

    *MAC_FLEN_REG = 1518 + 8 + 4 * 2;   //  MAC frame + 8-byte special tag + 4-byte VLAN tag * 2
    *MAC_CTRL_REG(6, 2) |= 1 << 3;      //  enable jumbo frame

    for(i = 0; i < 7; i ++){
        *MAC_CTRL_REG(i,0) |= 1 << 7;   //set  FCS generation Enable for each port
    }

#if 0
    for ( i = 0; i < 12; i++ )
        *PCE_PCTRL_REG(i, 0) |= 1 << 5;     //  VLAN transparent mode

    //  VLAN active table
    *PCE_TBL_KEY(0) = 0x0055;
    *PCE_TBL_VAL(0) = 0x0000;
    *PCE_TBL_ADDR   = 0x0000;
    *PCE_TBL_CTRL   = 0x9021;

    //  VLAN mapping table
    *PCE_TBL_VAL(0) = 0x0055;
    *PCE_TBL_VAL(1) = 0x0FFF;
    *PCE_TBL_VAL(2) = 0x0000;
    *PCE_TBL_ADDR   = 0x0000;
    *PCE_TBL_CTRL   = 0x9022;
#endif
#if 0
    *PCE_GCTRL_REG(0) |= 1 << 14;           //  VLAN-aware Switching

    for ( i = 0; i < 12; i++ )
        *PCE_PCTRL_REG(i, 5) |= 1 << 0;     //  VLAN unmatch rule    TO BE TESTED
#endif

#if 0
    //  xuliang: MFE test
    for ( i = 0; i < 6; i++ )
        *PCE_PCTRL_REG(i, 3)  |= 1 << 11;     //  enable egress re-direct to monitoring port for physical port 0-5
    *PCE_PMAP_REG(1) = 0x40;                //  monitoring
#else
    *PCE_PMAP_REG(1) = 0x7F;                //  monitoring
#endif
    //*PCE_PMAP_REG(2) = 0x7F & ~g_pmac_ewan; //  broadcast and unknown multicast
    *PCE_PMAP_REG(2) = 0x7F;                //  broadcast and unknown multicast
    //*PCE_PMAP_REG(3) = 0x7F & ~g_pmac_ewan; //  unknown uni-cast
    *PCE_PMAP_REG(3) = 0x7F;                //  unknown uni-cast

    *PMAC_EWAN_REG = g_pmac_ewan;

    *PMAC_RX_IPG_REG = 0x8F;

#if AR10_EMULATION
    //venus emulation code, disable phy automatic polling function
    *MDC_CFG_0_REG = 0;
#endif

    init_internal_tantos_qos_setting();
}

static INLINE void reset_ppe(void)
{
#ifdef MODULE
    //  reset PPE
    ifx_rcu_rst(IFX_RCU_DOMAIN_PPE, IFX_RCU_MODULE_PPA);
#endif
}

static INLINE void init_pmu(void)
{
    //   5 - DMA, should be enabled in DMA driver
    //   9 - DSL DFE/AFE
    //  15 - AHB
    //  21 - PPE TC
    //  22 - PPE EMA
    //  23 - PPE DPLUS
    //  28 - SWITCH

    //  code as reference
    //*PMU_PWDCR &= ~((1 << 28) | (1 << 23) | (1 << 22) | (1 << 21) | (1 << 15) | (1 << 9) | (1 << 5));
    *AMAZON_S_CGU_SYS &= ~(0x00030000); //set PPE clk to 250Mhz ; 300Mhz is not supported in DSL Mode

    PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_SLL01_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_QSB_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_ENABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_DPLUS_PMU_SETUP(IFX_PMU_ENABLE);
    SWITCH_PMU_SETUP(IFX_PMU_ENABLE);
}

static INLINE void init_dplus(void)
{
    *DM_RXCFG &= ~0x80000000;   //  disable DMRX
    *DS_RXCFG &= ~0x00008000;   //  disable DSRX
    *DM_RXPGCNT = 0x00020000;   //  clear page pointer & counter
    *DS_RXPGCNT = 0x00040000;   //  clear page pointer & counter
//    *DS_RXPGCNT = 0x00000000;   //  clear page pointer & counter

    *DM_RXDB    = 0x1450;
    *DM_RXCB    = 0x143C;
//    *DM_RXPGCNT = 0x00020000;
    *DM_RXPKTCNT= 0x00000200;
    *DM_RXCFG   = 0x00587014;
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
    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN + SB_RAM2_DWLEN + SB_RAM3_DWLEN + SB_RAM4_DWLEN; i++ )
        *p++ = 0;

    //  Configure share buffer master selection
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
     int i;

    *SFSM_CFG0          = 0x00010014;
    for (i=0;i<10;i++);

    *SFSM_DBA0          = 0x1800;
    *SFSM_CBA0          = 0x1918;
    for (i=0;i<10;i++);

    *SFSM_CFG0          = 0x00014014;
    *FFSM_DBA0          = 0x17AC;
    *FFSM_CFG0          = 0x00010006;

    *SFSM_CFG1          = 0x00010014;
    for (i=0;i<10;i++);

    *SFSM_DBA1          = 0x192C;
    *SFSM_CBA1          = 0x1A44;
    *FFSM_DBA1          = 0x1758;
    *FFSM_CFG1          = 0x00010006;

    //*FFSM_IDLE_HEAD_BC0 = 0x00000001;
}


static INLINE void setup_ppe_conf(void)
{
    // enable PPE and MIPS access to DFE memory
    //*AMAZON_S_RCU_PPE_CONF = *AMAZON_S_RCU_PPE_CONF | 0xC0000000;
}

static INLINE void init_hw(void)
{
    setup_ppe_conf();

    init_pmu();

    init_internal_tantos();

    init_dplus();

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
    volatile u32 *dest;
    int i;

    if ( code_src == 0 || ((unsigned long)code_src & 0x03) != 0
        || data_src == 0 || ((unsigned long)data_src & 0x03) != 0
        || (code_dword_len > 0x1000 && data_dword_len > 0) )
        return -EINVAL;

    /*  set PPE code memory to FPI bus access mode  */
    if ( code_dword_len <= 0x1000 )
        *CDM_CFG = CDM_CFG_RAM1_SET(0x00) | CDM_CFG_RAM0_SET(0x00);
    else
        *CDM_CFG = CDM_CFG_RAM1_SET(0x01) | CDM_CFG_RAM0_SET(0x00);
    for ( i = 0; i < 1000; i++ );

    dbg("code_dword_len = 0x%X, data_dword_len = 0x%X", code_dword_len, data_dword_len);

    /*  copy code   */
    dest = CDM_CODE_MEMORY_RAM0_ADDR(0);
    while ( code_dword_len-- > 0 )
        *dest++ = *code_src++;

    /*  copy data   */
    dest = PP32_DATA_MEMORY_RAM1_ADDR(0);
    while ( data_dword_len-- > 0 )
        *dest++ = *data_src++;

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
    ret = pp32_specific_init(fwcode, NULL);
    if ( ret )
        return ret;

    /*  run PP32    */
    *PP32_DBG_CTRL = DBG_CTRL_RESTART;
    /*  idle for a while to let PP32 init itself    */
    udelay(200);

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
    else if ( ethwan == 2 )
        g_eth_wan_mode = 3;

    //printk("ethwan=%d, g_eth_wan_mode=%d\n", ethwan, g_eth_wan_mode);

    if ( wanitf == ~0 || wanitf == 0)
    {
        switch ( g_eth_wan_mode )
        {
        case 0: /*  DSL WAN     */  g_wan_itf = 3 << 6; break;
        case 2: /*  Mixed Mode  */  g_wan_itf = 1 << 0; break;
        case 3: /*  ETH WAN     */  g_wan_itf = 1 << 1; break;
        }
    }
    else
    {
        g_wan_itf = wanitf;
        switch ( g_eth_wan_mode )
        {
        case 0: /*  DSL WAN     */
                g_wan_itf &= 0xF8;
                g_wan_itf |= 3 << 6;
                break;   //  ETH0/1 can not be WAN
        case 2: /*  Mixed Mode  */
				g_wan_itf &= 0x03; //  only ETH0/1 support mixed mode
				if(g_wan_itf == 0x03 || g_wan_itf == 0){ //both ETH0/1 in mixed mode, reset to eth0 mixed mode
					g_wan_itf = 1;
				}
				break;
        case 3: /*  ETH WAN     */
				g_wan_itf &= 0x3B; //  DSL disabled in ETH WAN mode
				if((g_wan_itf & 0x03) == 0x03){ //both ETH0/1 in WAN mode, remove eth0 wan setting
					g_wan_itf &= ~0x01;
				}
                if((g_wan_itf & 0x03) == 0){
                    g_wan_itf |= 1 << 1;
                }
				break;
        }


    }

    //printk("g_wan_itf = %x\n", g_wan_itf);

    g_ipv6_acc_en = ipv6_acc_en ? 1 : 0;
    g_ipv4_lan_collision_rout_fwdc_table_base = __IPV4_LAN_COLLISION_ROUT_FWDC_TABLE_BASE;
    g_ipv4_wan_collision_rout_fwda_table_base = __IPV4_WAN_COLLISION_ROUT_FWDA_TABLE_BASE;

    g_wanqos_en = g_eth_wan_mode == 3 && (g_wan_itf & 0x03) != 0x03 && wanqos_en ? wanqos_en : 0;

    g_mailbox_signal_mask = g_wanqos_en ? CPU_TO_WAN_SWAP_SIG : WAN_RX_SIG(1);

    memset(&g_atm_priv_data, 0, sizeof(g_atm_priv_data));
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        g_atm_priv_data.port[i].tx_max_cell_rate    = (u32)port_cell_rate_up[i];
    }
    g_atm_priv_data.max_connections = ATM_QUEUE_NUMBER;
    g_atm_priv_data.max_sw_tx_queues = ATM_SW_TX_QUEUE_NUMBER;

    //memset(g_eth_priv_data, 0, sizeof(g_eth_priv_data));
    //printk("start setting the queue map\n");

    for ( i = 0; i < 2; i++ )
    {
        int max_packet_priority = NUM_ENTITY(g_eth_prio_queue_map[i]);  //  assume only 8 levels are used in Linux
        int tx_num_q;
        int q_step, q_accum, p_step;

        tx_num_q = (g_wan_itf & (1 << i)) && g_wanqos_en ? __ETH_WAN_TX_QUEUE_NUM : 4;
        q_step = tx_num_q - 1;
        p_step = max_packet_priority - 1;
        for ( j = 0, q_accum = 0; j < max_packet_priority; j++, q_accum += q_step )
            g_eth_prio_queue_map[i][j] = (q_accum + (p_step >> 1)) / p_step;

        if ( (g_wan_itf & (1 << i)) && g_wanqos_en )
            for ( j = 0; j < max_packet_priority; j++ )
                g_eth_prio_queue_map[i][j] = q_step - g_eth_prio_queue_map[i][j];
    }

    return 0;
}

static INLINE void clear_local_variables(void)
{
}

#if defined(ENABLE_FW_MODULE_TEST) && ENABLE_FW_MODULE_TEST

#include "cfg_arrays_a5.h"

unsigned int ppe_fw_mode        = 0; // 0: normal mode, 1: mix mode
unsigned int ppe_wan_hash_algo  = 0; // 0: using algo 0, 1: using algo 1
unsigned int acc_proto      = 0; // 0: UDP, 1:TCP

void setup_acc_action_tbl(void)
{
    unsigned int idx;

    if (acc_proto == 0) {
        // clear bit16 of dw3 of each action entry
        unsigned long udp_mask = ~ (1 << 16);

        // setup for Hash entry
        idx = 3;
        while (idx < sizeof(lan_uc_act_tbl_normal_mode_cfg)/sizeof(unsigned long)) {
            lan_uc_act_tbl_normal_mode_cfg[idx] &= udp_mask;
            lan_uc_act_tbl_mixed_mode_cfg[idx] &= udp_mask;

            wan_uc_act_tbl_alo_0_cfg[idx] &= udp_mask;
            wan_uc_act_tbl_alo_1_cfg[idx] &= udp_mask;
            idx += 6;
        }

        // setup for Collsion entry
        idx = 3;
        while (idx < sizeof(lan_uc_col_act_tbl_normal_mode_cfg)/sizeof(unsigned long)) {
            lan_uc_col_act_tbl_normal_mode_cfg[idx] &= udp_mask;
            lan_uc_col_act_tbl_mixed_mode_cfg[idx] &= udp_mask;
            wan_uc_col_act_tbl_cfg[idx] &= udp_mask;
            idx += 6;
        }
    }
    else {
        // set bit16 of dw3 of each action entry
        unsigned long tcp_mask =  (1 << 16);

        // setup for Hash entry
        idx = 3;
        while (idx < sizeof(lan_uc_act_tbl_normal_mode_cfg)/sizeof(unsigned long)) {
            lan_uc_act_tbl_normal_mode_cfg[idx] |= tcp_mask;
            lan_uc_act_tbl_mixed_mode_cfg[idx] |= tcp_mask;

            wan_uc_act_tbl_alo_0_cfg[idx] |= tcp_mask;
            wan_uc_act_tbl_alo_1_cfg[idx] |= tcp_mask;
            idx += 6;
        }

        // setup for Collsion entry
        idx = 3;
        while (idx < sizeof(lan_uc_col_act_tbl_normal_mode_cfg)/sizeof(unsigned long)) {
            lan_uc_col_act_tbl_normal_mode_cfg[idx] |= tcp_mask;
            lan_uc_col_act_tbl_mixed_mode_cfg[idx] |= tcp_mask;
            wan_uc_col_act_tbl_cfg[idx] |= tcp_mask;
            idx += 6;
        }
    }
}

void init_acc_tables(void)
{
    setup_acc_action_tbl();

    // init MAC table
    memcpy((void *)ROUT_MAC_CFG_TBL(0), rout_mac_cfg, sizeof(rout_mac_cfg));

    // PPPoE table
    memcpy((void *)PPPOE_CFG_TBL(0), pppoe_cfg, sizeof(pppoe_cfg));

    // Outer VLAN Config
    memcpy((void *)OUTER_VLAN_TBL(0), outer_vlan_cfg, sizeof(outer_vlan_cfg));

    // lan uc_cmp_tbl_cfg (Hash) and collision
    memcpy((void *)ROUT_LAN_HASH_CMP_TBL(0), lan_uc_cmp_tbl_cfg, sizeof(lan_uc_cmp_tbl_cfg));
    memcpy((void *)ROUT_LAN_COLL_CMP_TBL(0), lan_uc_col_cmp_tbl_cfg, sizeof(lan_uc_col_cmp_tbl_cfg));

    // lan action
    if(ppe_fw_mode == 0) {
        // normal mode
        memcpy((void *)ROUT_LAN_HASH_ACT_TBL(0), lan_uc_act_tbl_normal_mode_cfg, sizeof(lan_uc_act_tbl_normal_mode_cfg));
        memcpy((void *)ROUT_LAN_COLL_ACT_TBL(0), lan_uc_col_act_tbl_normal_mode_cfg, sizeof(lan_uc_col_act_tbl_normal_mode_cfg));
    }
    else {
        // mix mode
        memcpy((void *)ROUT_LAN_HASH_ACT_TBL(0), lan_uc_act_tbl_mixed_mode_cfg, sizeof(lan_uc_act_tbl_mixed_mode_cfg));
        memcpy((void *)ROUT_LAN_COLL_ACT_TBL(0), lan_uc_col_act_tbl_mixed_mode_cfg, sizeof(lan_uc_col_act_tbl_mixed_mode_cfg));
    }

    // wan hash cmp anc act table
    if ( ppe_wan_hash_algo == 0) {
        // WAN algo 0
        memcpy((void *)ROUT_WAN_HASH_CMP_TBL(0), wan_uc_cmp_tbl_alo_0_cfg, sizeof(wan_uc_cmp_tbl_alo_0_cfg));
        memcpy((void *)ROUT_WAN_HASH_ACT_TBL(0), wan_uc_act_tbl_alo_0_cfg, sizeof(wan_uc_act_tbl_alo_0_cfg));
    }
    else {
        // WAN algo 1
        memcpy((void *)ROUT_WAN_HASH_CMP_TBL(0), wan_uc_cmp_tbl_alo_1_cfg, sizeof(wan_uc_cmp_tbl_alo_1_cfg));
        memcpy((void *)ROUT_WAN_HASH_ACT_TBL(0), wan_uc_act_tbl_alo_1_cfg, sizeof(wan_uc_act_tbl_alo_1_cfg));
    }

    // wan collision cmp and act table
    memcpy((void *)ROUT_WAN_COLL_CMP_TBL(0), wan_uc_col_cmp_tbl_cfg, sizeof(wan_uc_col_cmp_tbl_cfg));
    memcpy((void *)ROUT_WAN_COLL_ACT_TBL(0), wan_uc_col_act_tbl_cfg, sizeof(wan_uc_col_act_tbl_cfg));

    // wan multicast cmp and act table
    memcpy((void *)ROUT_WAN_MC_CMP_TBL(0), wan_mc_cmp_tbl_cfg, sizeof(wan_mc_cmp_tbl_cfg));
    memcpy((void *)ROUT_WAN_MC_ACT_TBL(0), wan_mc_act_tbl_cfg, sizeof(wan_mc_act_tbl_cfg));
}

#endif  //  #if defined(ENABLE_FW_MODULE_TEST) && ENABLE_FW_MODULE_TEST

static INLINE void init_communication_data_structures(int fwcode)
{
    struct eth_ports_cfg eth_ports_cfg = {0};
    struct rout_tbl_cfg lan_rout_tbl_cfg = {0};
    struct rout_tbl_cfg wan_rout_tbl_cfg = {0};
    struct gen_mode_cfg1 gen_mode_cfg1 = {0};
    struct wrx_queue_config wrx_queue_config = {0};
    struct wtx_port_config wtx_port_config = {0};
    struct wtx_queue_config wtx_queue_config = {0};
    int i;

    *CDM_CFG = CDM_CFG_RAM1_SET(0x00) | CDM_CFG_RAM0_SET(0x00);

    for ( i = 0; i < 1000; i++ );

    *PSEUDO_IPv4_BASE_ADDR = 0xFFFFFF00;

    *CFG_WRX_HTUTS      = g_atm_priv_data.max_connections + OAM_HTU_ENTRY_NUMBER;
    *CFG_WRDES_DELAY    = write_descriptor_delay;
    *WRX_EMACH_ON       = 0x03;
    *WTX_EMACH_ON       = 0x7FFF;
    *WRX_HUNT_BITTH     = DEFAULT_RX_HUNT_BITTH;

    eth_ports_cfg.wan_vlanid_hi = 0;
    eth_ports_cfg.wan_vlanid_lo = 0;
    eth_ports_cfg.eth0_type     = 0;    //  lan
    *ETH_PORTS_CFG = eth_ports_cfg;

    lan_rout_tbl_cfg.rout_num   = MAX_COLLISION_ROUTING_ENTRIES;
    lan_rout_tbl_cfg.ttl_en     = 1;
    lan_rout_tbl_cfg.rout_drop  = 0;
    *LAN_ROUT_TBL_CFG = lan_rout_tbl_cfg;

    wan_rout_tbl_cfg.rout_num           = MAX_COLLISION_ROUTING_ENTRIES;
    wan_rout_tbl_cfg.wan_rout_mc_num    = MAX_WAN_MC_ENTRIES;
    wan_rout_tbl_cfg.ttl_en             = 1;
    wan_rout_tbl_cfg.wan_rout_mc_drop   = 0;
    wan_rout_tbl_cfg.rout_drop          = 0;
    *WAN_ROUT_TBL_CFG = wan_rout_tbl_cfg;

    gen_mode_cfg1.app2_indirect         = 1;    //  0: means DMA RX ch 3 is dedicated for CPU1 process
    gen_mode_cfg1.us_indirect           = 0;
    gen_mode_cfg1.us_early_discard_en   = 1;
#ifdef CONFIG_IFX_PPA_MFE
    gen_mode_cfg1.classification_num    = MAX_CLASSIFICATION_ENTRIES;
#else
    gen_mode_cfg1.classification_num    = 0;
#endif
    gen_mode_cfg1.ipv6_rout_num         = g_ipv6_acc_en ? 2 : 2;
    gen_mode_cfg1.session_ctl_en        = 0;
    gen_mode_cfg1.wan_hash_alg          = 0;
#ifdef CONFIG_IFX_PPA_MFE
    gen_mode_cfg1.brg_class_en          = 1;
#else
    gen_mode_cfg1.brg_class_en          = 0;
#endif
    gen_mode_cfg1.ipv4_mc_acc_mode      = 0;
    gen_mode_cfg1.ipv6_acc_en           = g_ipv6_acc_en;
    gen_mode_cfg1.wan_acc_en            = 1;
    gen_mode_cfg1.lan_acc_en            = 1;
    gen_mode_cfg1.sw_iso_mode           = 1;
    gen_mode_cfg1.sys_cfg               = g_eth_wan_mode;   //  3 - ETH1 WAN, 2 - ETH0 mixed, 0 - DSL WAN
    *GEN_MODE_CFG1 = gen_mode_cfg1;
    GEN_MODE_CFG2->itf_outer_vlan_vld   = 1 << 7;   //  DSL EoA port use outer VLAN for PVCs differentiation
    //GEN_MODE_CFG2->itf_outer_vlan_vld  |= 0xFF;     //  Turn on outer VLAN for all interfaces;

    //  fake setting, when open ATM VC, it will be set with the real value
    wrx_queue_config.new_vlan   = 0;
    wrx_queue_config.vlan_ins   = 0;
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
    for ( i = 0; i < 16; i++ )
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
    for ( i = 0; i < 15; i++ )
    {
        wtx_queue_config.qsb_vcid   = i + QSB_QUEUE_NUMBER_BASE;
        *WTX_QUEUE_CONFIG(i) = wtx_queue_config;
    }

    //*MTU_CFG_TBL(0) = DMA_PACKET_SIZE;          //  for ATM
    *MTU_CFG_TBL(0) = ETH_MAX_DATA_LENGTH;
    for ( i = 1; i < 8; i++ )
        *MTU_CFG_TBL(i) = ETH_MAX_DATA_LENGTH;  //  for ETH

    if ( g_eth_wan_mode == 0 )
        setup_oam_htu_entry();

    if ( g_wanqos_en )
    {
        struct wtx_qos_q_desc_cfg wtx_qos_q_desc_cfg = {0};
        struct tx_qos_cfg tx_qos_cfg = {0};
        struct cfg_std_data_len cfg_std_data_len = {0};

        for ( i = 0; i < __ETH_WAN_TX_QUEUE_NUM; i++ )
        {
            wtx_qos_q_desc_cfg.length = ETH_WAN_TX_DESC_NUM(i);
            wtx_qos_q_desc_cfg.addr   = __ETH_WAN_TX_DESC_BASE(i);
            *WTX_QOS_Q_DESC_CFG(i) = wtx_qos_q_desc_cfg;
        }

        tx_qos_cfg.time_tick    = cgu_get_pp32_clock() / 62500; //  16 * (cgu_get_pp32_clock() / 1000000)
        tx_qos_cfg.eth1_eg_qnum = __ETH_WAN_TX_QUEUE_NUM;
        tx_qos_cfg.eth1_qss     = 1;
        tx_qos_cfg.eth1_burst_chk = 1;
        *TX_QOS_CFG = tx_qos_cfg;

        cfg_std_data_len.byte_off = 0;
        cfg_std_data_len.data_len = DMA_PACKET_SIZE;

        *CFG_STD_DATA_LEN = cfg_std_data_len;
    }

    *CFG_WAN_PORTMAP    = g_wan_itf;
    *CFG_MIXED_PORTMAP  = g_eth_wan_mode == 2 /* Mixed Mode */ ? g_wan_itf : 0;

#if defined(ENABLE_FW_MODULE_TEST) && ENABLE_FW_MODULE_TEST
    init_acc_tables();
#endif
}

static INLINE int alloc_dma(void)
{
    int ret;
    unsigned int pre_alloc_desc_total_num;
    struct sk_buff **skb_pool;
    struct sk_buff **pskb;
    volatile u32 *p;
    int i;

    pre_alloc_desc_total_num = DMA_RX_CH1_DESC_NUM + DMA_RX_CH2_DESC_NUM;
    if ( g_eth_wan_mode == 0 )  //  DSL WAN
        pre_alloc_desc_total_num += CPU_TO_WAN_TX_DESC_NUM + WAN_TX_DESC_NUM_TOTAL + WAN_RX_DESC_NUM(0) * 2;
    else if ( g_wanqos_en )
        pre_alloc_desc_total_num += CPU_TO_WAN_TX_DESC_NUM + WAN_TX_DESC_NUM_TOTAL + DMA_TX_CH1_DESC_NUM;

    skb_pool = (struct sk_buff **)kmalloc(pre_alloc_desc_total_num * sizeof(*skb_pool), GFP_KERNEL);
    if ( skb_pool == NULL )
    {
        ret = -ENOMEM;
        goto ALLOC_SKB_POOL_FAIL;
    }

    for ( i = 0; i < pre_alloc_desc_total_num; i++ )
    {
        skb_pool[i] = alloc_skb_rx();
        if ( !skb_pool[i] )
        {
            ret = -ENOMEM;
            goto ALLOC_SKB_FAIL;
        }
#ifndef CONFIG_MIPS_UNCACHED
        /*  invalidate cache    */
        dma_cache_inv((unsigned long)skb_pool[i]->data, DMA_PACKET_SIZE);
#endif
#if defined(DEBUG_SKB_SWAP) && DEBUG_SKB_SWAP
        g_dbg_skb_swap_pool[i] = skb_pool[i];
#endif
    }

    g_dma_device = dma_device_reserve("PPE");
    if ( !g_dma_device )
    {
        ret = -EIO;
        goto RESERVE_DMA_FAIL;
    }

    g_dma_device->buffer_alloc    = dma_buffer_alloc;
    g_dma_device->buffer_free     = dma_buffer_free;
    g_dma_device->intr_handler    = dma_int_handler;
//    g_dma_device->max_rx_chan_num = 4;
//    g_dma_device->max_tx_chan_num = 4;
    g_dma_device->tx_burst_len    = 8;
    g_dma_device->rx_burst_len    = 8;
    //*AMAZON_S_SW_PMAC_RX_IPG = (*AMAZON_S_SW_PMAC_RX_IPG & ~0xF0) | 0x0130; //  increase RX FIFO Request Watermark, 0x01?0 means maximum watermark

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
    g_dma_device->tx_chan[0]->desc_base = (int)DMA_TX_CH0_DESC_BASE;
    g_dma_device->tx_chan[0]->desc_len  = DMA_TX_CH0_DESC_NUM;
    g_dma_device->tx_chan[1]->desc_base = (int)DMA_TX_CH1_DESC_BASE;
    g_dma_device->tx_chan[1]->desc_len  = DMA_TX_CH1_DESC_NUM;
    g_dma_device->tx_chan[0]->global_buffer_len = DMA_PACKET_SIZE;
    g_dma_device->tx_chan[0]->peri_to_peri      = 1;
    g_dma_device->tx_chan[1]->global_buffer_len = DMA_PACKET_SIZE;
    g_dma_device->tx_chan[1]->peri_to_peri      = 1;

    g_dma_device->port_packet_drop_enable = 1;

    if ( dma_device_register(g_dma_device) != IFX_SUCCESS )
    {
        printk("failed in \"dma_device_register\"");
        ret = -EIO;
        goto DMA_DEVICE_REGISTER_FAIL;
    }

    pskb = skb_pool;

    p = (volatile u32 *)CPU_TO_WAN_TX_DESC_BASE;
    for ( i = 0; i < CPU_TO_WAN_TX_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & (DMA_ALIGNMENT - 1)) == 0, "CPU to WAN TX data pointer (0x%#x) must be 8 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = g_wanqos_en ? 0x30000000 : 0xB0000000;
        if ( g_eth_wan_mode == 0 || g_wanqos_en )
            *p++ = (u32)(*pskb++)->data & 0x1FFFFFE0;
        else
            *p++ = 0;
    }

    p = (volatile u32 *)DSL_WAN_TX_DESC_BASE(0);
    for ( i = 0; i < WAN_TX_DESC_NUM_TOTAL; i++ )
    {
        ASSERT(((u32)(*pskb)->data & (DMA_ALIGNMENT - 1)) == 0, "WAN (ATM/ETH) TX data pointer (0x%#x) must be 8 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0x30000000;
        if ( g_eth_wan_mode == 0 )
            *p++ = ((u32)(*pskb++)->data & 0x1FFFFFE0) >> 2;
        else if ( g_wanqos_en )
            *p++ = (u32)(*pskb++)->data & 0x1FFFFFE0;
        else
            *p++ = 0;
    }

    if ( g_eth_wan_mode == 0 )
    {
        p = (volatile u32 *)WAN_RX_DESC_BASE(0);
        for ( i = 0; i < WAN_RX_DESC_NUM(0); i++ )
        {
            ASSERT(((u32)(*pskb)->data & (DMA_ALIGNMENT - 1)) == 0, "WAN (ATM) RX channel 0 (AAL) data pointer (0x%#x) must be 8 DWORDS aligned", (u32)(*pskb)->data);
            *p++ = 0xB0000000 | DMA_PACKET_SIZE;
            *p++ = ((u32)(*pskb++)->data & 0x1FFFFFE0);
        }

        p = (volatile u32 *)WAN_RX_DESC_BASE(1);
        for ( i = 0; i < WAN_RX_DESC_NUM(1); i++ )
        {
            ASSERT(((u32)(*pskb)->data & 3) == 0, "WAN (ATM) RX channel 1 (OAM) data pointer (0x%#x) must be DWORD aligned", (u32)(*pskb)->data);
            g_wan_rx1_desc_skb[i] = *pskb;
            *p++ = 0xB0000000 | DMA_PACKET_SIZE;
            *p++ = ((u32)(*pskb++)->data & 0x1FFFFFFC) >> 2;
        }
    }

    p = (volatile u32 *)DMA_RX_CH1_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH1_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & (DMA_ALIGNMENT - 1)) == 0, "DMA RX channel 1 data pointer (0x%#x) must be 8 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0x80000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFE0;
    }

    p = (volatile u32 *)DMA_RX_CH2_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH2_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & (DMA_ALIGNMENT - 1)) == 0, "DMA RX channel 2 data pointer (0x%#x) must be 8 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFE0;
    }

    if ( g_wanqos_en )
    {
        p = (volatile u32 *)DMA_TX_CH1_DESC_BASE;
        for ( i = 0; i < DMA_TX_CH1_DESC_NUM; i++ )
        {
            ASSERT(((u32)(*pskb)->data & (DMA_ALIGNMENT - 1)) == 0, "DMA TX channel 1 data pointer (0x%#x) must be 8 DWORDS aligned", (u32)(*pskb)->data);
            *p++ = 0xB0000000;
            *p++ = (u32)(*pskb++)->data & 0x1FFFFFE0;
        }
    }

    g_f_dma_opened = 0;
//    g_mailbox_signal_mask |= DMA_TX_CH2_SIG;

    kfree(skb_pool);

    return 0;

DMA_DEVICE_REGISTER_FAIL:
    dma_device_release(g_dma_device);
    g_dma_device = NULL;
RESERVE_DMA_FAIL:
    i = pre_alloc_desc_total_num;
ALLOC_SKB_FAIL:
    while ( i-- )
        dev_kfree_skb_any(skb_pool[i]);
    kfree(skb_pool);
ALLOC_SKB_POOL_FAIL:
    return ret;
}

static INLINE void free_dma(void)
{
}

#if defined(CONFIG_IFX_PPA_DATAPATH)
static INLINE void ethaddr_setup(unsigned int port, char *line)
{
    u8 *p;
    char *ep;
    int i;

    p = MY_ETH0_ADDR + (port ? MAX_ADDR_LEN : 0);
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
        "AR10",
        "GR10"
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
        "VR9",
        "AR10",
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

    res = create_proc_read_entry("ver",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_ver,
                                  NULL);

    res = create_proc_read_entry("mib",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_mib,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mib;

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC

    res = create_proc_read_entry("route",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_route,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_route;

    res = create_proc_read_entry("mc",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_mc,
                                  NULL);

    res = create_proc_read_entry("genconf",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_genconf,
                                  NULL);
    if ( res )
        res->write_proc = ifx_ppa_drv_proc_write_genconf;

    res = create_proc_read_entry("queue",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_queue,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_queue;

    res = create_proc_read_entry("pppoe",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_pppoe,
                                  NULL);
    if ( res )
        res->write_proc = ifx_ppa_drv_proc_write_pppoe;

    res = create_proc_read_entry("mtu",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_mtu,
                                  NULL);
    if ( res )
        res->write_proc = ifx_ppa_drv_proc_write_mtu;

    res = create_proc_read_entry("hit",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_hit,
                                  NULL);
    if ( res )
        res->write_proc = ifx_ppa_drv_proc_write_hit;

    res = create_proc_read_entry("mac",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_mac,
                                  NULL);
    if ( res )
        res->write_proc = ifx_ppa_drv_proc_write_mac;

    res = create_proc_read_entry("out_vlan",
                                  0,
                                  g_eth_proc_dir,
                                  ifx_ppa_drv_proc_read_out_vlan,
                                  NULL);
    if ( res )
        res->write_proc = ifx_ppa_drv_proc_write_out_vlan;

#endif

#if defined(DEBUG_HTU_PROC) && DEBUG_HTU_PROC
    res = create_proc_read_entry("htu",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_htu,
                                  NULL);
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    res = create_proc_read_entry("dbg",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_dbg,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_dbg;
#endif

#if defined(DEBUG_MEM_PROC) && DEBUG_MEM_PROC
    res = create_proc_read_entry("mem",
                                  0,
                                  g_eth_proc_dir,
                                  NULL,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mem;
#endif

#if defined(DEBUG_PP32_PROC) && DEBUG_PP32_PROC
    res = create_proc_read_entry("pp32",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_pp32,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_pp32;
#endif

    res = create_proc_read_entry("burstlen",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_burstlen,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_burstlen;

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
    res = create_proc_read_entry("fw",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_fw,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_fw;
#endif

#if defined(DEBUG_SEND_PROC) && DEBUG_SEND_PROC
    res = create_proc_read_entry("send",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_send,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_send;
#endif

    res = create_proc_entry("port0mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port0_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port1mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port1_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port2mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port2_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port3mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port3_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port4mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port4_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port5mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port5_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port6mib",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = port6_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }
#if 0
    res = create_proc_read_entry("mactable",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_mactable,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mactable;
#endif
    res = create_proc_read_entry("direct_forwarding",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_directforwarding,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_directforwarding;

    res = create_proc_read_entry("clk",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_clk,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_clk;

    res = create_proc_read_entry("flowcontrol",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_flowcontrol,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_flowcontrol;

    res = create_proc_entry("prio",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_prio;
        res->write_proc = proc_write_prio;
    }

    if ( g_ipv6_acc_en )
    {
        res = create_proc_entry("ipv6_ip",
                                0,
                                g_eth_proc_dir);
        if ( res )
            res->read_proc  = ifx_ppa_drv_proc_read_ipv6_ip;
    }

    res = create_proc_entry("ewan",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_ewan;
        res->write_proc = proc_write_ewan;
    }
#ifdef CONFIG_IFX_PPA_QOS
    if ( g_wanqos_en )
    {
        res = create_proc_entry("qos",
                                0,
                                g_eth_proc_dir);
        if ( res )
            res->read_proc  = proc_read_qos;
    }
#endif

#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
    res = create_proc_entry("dsl_vlan",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_dsl_vlan;
        res->write_proc = proc_write_dsl_vlan;
    }
#endif

#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
    res = create_proc_entry("mirror",
                            0,
                            g_eth_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_mirror;
        res->write_proc = proc_write_mirror;
    }
#endif
}

static INLINE void proc_file_delete(void)
{
#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC
    remove_proc_entry("mirror",
                      g_eth_proc_dir);
#endif

#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
    remove_proc_entry("dsl_vlan",
                      g_eth_proc_dir);
#endif

#ifdef CONFIG_IFX_PPA_QOS
    if ( g_wanqos_en )
    {
        remove_proc_entry("qos",
                          g_eth_proc_dir);
    }
#endif

    if ( g_ipv6_acc_en )
    {
        remove_proc_entry("ipv6_ip",
                          g_eth_proc_dir);
    }

    remove_proc_entry("prio",
                      g_eth_proc_dir);

    remove_proc_entry("flowcontrol",
                      g_eth_proc_dir);

    remove_proc_entry("clk",
                      g_eth_proc_dir);

    remove_proc_entry("direct_forwarding",
                      g_eth_proc_dir);

//    remove_proc_entry("mactable",
//                      g_eth_proc_dir);

    remove_proc_entry("port6mib",
                      g_eth_proc_dir);

    remove_proc_entry("port5mib",
                      g_eth_proc_dir);

    remove_proc_entry("port4mib",
                      g_eth_proc_dir);

    remove_proc_entry("port3mib",
                      g_eth_proc_dir);
    remove_proc_entry("port2mib",
                      g_eth_proc_dir);

    remove_proc_entry("port1mib",
                      g_eth_proc_dir);

    remove_proc_entry("port0mib",
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

    remove_proc_entry("route",
                      g_eth_proc_dir);

#endif

    remove_proc_entry("mib",
                      g_eth_proc_dir);

    remove_proc_entry("ver",
                      g_eth_proc_dir);

    remove_proc_entry("eth", NULL);
}

static int proc_read_ver(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += print_driver_ver(page + off + len, count - len, "PPE datapath driver info", VER_FAMILY, VER_DRTYPE, VER_INTERFACE, VER_ACCMODE, VER_MAJOR, VER_MID, VER_MINOR);
    len += print_fw_ver(page + off + len, count - len);

    *eof = 1;

    return len;
}

static int proc_read_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    struct eth_priv_data *priv0, *priv1;
    //int i;

    eth_get_stats(g_eth_net_dev[0]);
    eth_get_stats(g_eth_net_dev[1]);
    priv0 = netdev_priv(g_eth_net_dev[0]);
    priv1 = netdev_priv(g_eth_net_dev[1]);
    len += sprintf(page + off + len,    "Firmware  (ETH0, ETH1, ATM - IPoA/PPPoA, ATM - EoA)\n");
    len += sprintf(page + off + len,    "  ig_fast_brg_pkts:          %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_fast_brg_pkts,           ITF_MIB_TBL(1)->ig_fast_brg_pkts,           ITF_MIB_TBL(6)->ig_fast_brg_pkts,           ITF_MIB_TBL(7)->ig_fast_brg_pkts);
    len += sprintf(page + off + len,    "  ig_fast_brg_bytes:         %10u, %10u, %10u, %10u\n\n",   ITF_MIB_TBL(0)->ig_fast_brg_bytes,          ITF_MIB_TBL(1)->ig_fast_brg_bytes,          ITF_MIB_TBL(6)->ig_fast_brg_bytes,          ITF_MIB_TBL(7)->ig_fast_brg_bytes);

    len += sprintf(page + off + len,    "  ig_fast_rt_ipv4_udp_pkts:  %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_fast_rt_ipv4_udp_pkts,   ITF_MIB_TBL(1)->ig_fast_rt_ipv4_udp_pkts,   ITF_MIB_TBL(6)->ig_fast_rt_ipv4_udp_pkts,   ITF_MIB_TBL(7)->ig_fast_rt_ipv4_udp_pkts);
    len += sprintf(page + off + len,    "  ig_fast_rt_ipv4_tcp_pkts:  %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_fast_rt_ipv4_tcp_pkts,   ITF_MIB_TBL(1)->ig_fast_rt_ipv4_tcp_pkts,   ITF_MIB_TBL(6)->ig_fast_rt_ipv4_tcp_pkts,   ITF_MIB_TBL(7)->ig_fast_rt_ipv4_tcp_pkts);
    len += sprintf(page + off + len,    "  ig_fast_rt_ipv4_mc_pkts:   %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_fast_rt_ipv4_mc_pkts,    ITF_MIB_TBL(1)->ig_fast_rt_ipv4_mc_pkts,    ITF_MIB_TBL(6)->ig_fast_rt_ipv4_mc_pkts,    ITF_MIB_TBL(7)->ig_fast_rt_ipv4_mc_pkts);
    len += sprintf(page + off + len,    "  ig_fast_rt_ipv4_bytes:     %10u, %10u, %10u, %10u\n\n",   ITF_MIB_TBL(0)->ig_fast_rt_ipv4_bytes,      ITF_MIB_TBL(1)->ig_fast_rt_ipv4_bytes,      ITF_MIB_TBL(6)->ig_fast_rt_ipv4_bytes,      ITF_MIB_TBL(7)->ig_fast_rt_ipv4_bytes);

    len += sprintf(page + off + len,    "  ig_fast_rt_ipv6_udp_pkts:  %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_fast_rt_ipv6_udp_pkts,   ITF_MIB_TBL(1)->ig_fast_rt_ipv6_udp_pkts,   ITF_MIB_TBL(6)->ig_fast_rt_ipv6_udp_pkts,   ITF_MIB_TBL(7)->ig_fast_rt_ipv6_udp_pkts);
    len += sprintf(page + off + len,    "  ig_fast_rt_ipv6_tcp_pkts:  %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_fast_rt_ipv6_tcp_pkts,   ITF_MIB_TBL(1)->ig_fast_rt_ipv6_tcp_pkts,   ITF_MIB_TBL(6)->ig_fast_rt_ipv6_tcp_pkts,   ITF_MIB_TBL(7)->ig_fast_rt_ipv6_tcp_pkts);
    len += sprintf(page + off + len,    "  ig_fast_rt_ipv6_bytes:     %10u, %10u, %10u, %10u\n\n",   ITF_MIB_TBL(0)->ig_fast_rt_ipv6_bytes,      ITF_MIB_TBL(1)->ig_fast_rt_ipv6_bytes,      ITF_MIB_TBL(6)->ig_fast_rt_ipv6_bytes,      ITF_MIB_TBL(7)->ig_fast_rt_ipv6_bytes);


    len += sprintf(page + off + len,    "  ig_cpu_pkts:               %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_cpu_pkts,                ITF_MIB_TBL(1)->ig_cpu_pkts,                ITF_MIB_TBL(6)->ig_cpu_pkts,                ITF_MIB_TBL(7)->ig_cpu_pkts);
    len += sprintf(page + off + len,    "  ig_cpu_bytes:              %10u, %10u, %10u, %10u\n\n",   ITF_MIB_TBL(0)->ig_cpu_bytes,               ITF_MIB_TBL(1)->ig_cpu_bytes,               ITF_MIB_TBL(6)->ig_cpu_bytes,               ITF_MIB_TBL(7)->ig_cpu_bytes);

    len += sprintf(page + off + len,    "  ig_drop_pkts:              %10u, %10u, %10u, %10u\n",     ITF_MIB_TBL(0)->ig_drop_pkts,               ITF_MIB_TBL(1)->ig_drop_pkts,               ITF_MIB_TBL(6)->ig_drop_pkts,               ITF_MIB_TBL(7)->ig_drop_pkts);
    len += sprintf(page + off + len,    "  ig_drop_bytes:             %10u, %10u, %10u, %10u\n\n",   ITF_MIB_TBL(0)->ig_drop_bytes,              ITF_MIB_TBL(1)->ig_drop_bytes,              ITF_MIB_TBL(6)->ig_drop_bytes,              ITF_MIB_TBL(7)->ig_drop_bytes);

    len += sprintf(page + off + len,    "  eg_fast_pkts:              %10u, %10u, %10u, %10u\n\n",   ITF_MIB_TBL(0)->eg_fast_pkts,               ITF_MIB_TBL(1)->eg_fast_pkts,               ITF_MIB_TBL(6)->eg_fast_pkts,               ITF_MIB_TBL(7)->eg_fast_pkts);

    len += sprintf(page + off + len,    "Driver (ETH0, ETH1, ATM)\n");
    len += sprintf(page + off + len,    "  rx_packets: %10u, %10u, %10u\n",         priv0->rx_packets,  priv1->rx_packets,  g_atm_priv_data.wrx_pdu);
    len += sprintf(page + off + len,    "  rx_bytes:   %10u, %10u,        N/A\n",   priv0->rx_bytes,    priv1->rx_bytes);
    len += sprintf(page + off + len,    "  rx_errors:  %10u, %10u,        N/A\n",   priv0->rx_errors,   priv1->rx_errors);
    len += sprintf(page + off + len,    "  rx_dropped: %10u, %10u, %10u\n",         priv0->rx_dropped,  priv1->rx_dropped,  g_atm_priv_data.wrx_drop_pdu);
    len += sprintf(page + off + len,    "  tx_packets: %10u, %10u, %10u\n",         priv0->tx_packets,  priv1->tx_packets, g_atm_priv_data.wtx_pdu);
    len += sprintf(page + off + len,    "  tx_bytes:   %10u, %10u,        N/A\n",   priv0->tx_bytes,    priv1->tx_bytes);
    len += sprintf(page + off + len,    "  tx_errors:  %10u, %10u, %10u\n",         priv0->tx_errors,   priv1->tx_errors,   g_atm_priv_data.wtx_err_pdu);
    len += sprintf(page + off + len,    "  tx_dropped: %10u, %10u, %10u\n",         priv0->tx_dropped,  priv1->tx_dropped,  g_atm_priv_data.wtx_drop_pdu);

    len += sprintf(page + off + len,    "DSL WAN MIB\n");
    len += sprintf(page + off + len,    "  wrx_drophtu_cell: %10u\n", DSL_WAN_MIB_TABLE->wrx_drophtu_cell);
    len += sprintf(page + off + len,    "  wrx_dropdes_pdu:  %10u\n", DSL_WAN_MIB_TABLE->wrx_dropdes_pdu);
    len += sprintf(page + off + len,    "  wrx_correct_pdu:  %10u\n", DSL_WAN_MIB_TABLE->wrx_correct_pdu);
    len += sprintf(page + off + len,    "  wrx_err_pdu:      %10u\n", DSL_WAN_MIB_TABLE->wrx_err_pdu);
    len += sprintf(page + off + len,    "  wrx_dropdes_cell: %10u\n", DSL_WAN_MIB_TABLE->wrx_dropdes_cell);
    len += sprintf(page + off + len,    "  wrx_correct_cell: %10u\n", DSL_WAN_MIB_TABLE->wrx_correct_cell);
    len += sprintf(page + off + len,    "  wrx_err_cell:     %10u\n", DSL_WAN_MIB_TABLE->wrx_err_cell);
    len += sprintf(page + off + len,    "  wrx_total_byte:   %10u\n", DSL_WAN_MIB_TABLE->wrx_total_byte);
    len += sprintf(page + off + len,    "  wtx_total_pdu:    %10u\n", DSL_WAN_MIB_TABLE->wtx_total_pdu);
    len += sprintf(page + off + len,    "  wtx_total_cell:   %10u\n", DSL_WAN_MIB_TABLE->wtx_total_cell);
    len += sprintf(page + off + len,    "  wtx_total_byte:   %10u\n", DSL_WAN_MIB_TABLE->wtx_total_byte);

#ifdef TEST_DSL_MIB  //note, for proc memory overflow, cannot add any info here.
    if( ethwan== 0 )
    {
        len += sprintf(page + off + len,    "DSL Queue DROP MIB\n");
        for(i=0; i<ATM_QUEUE_NUMBER; i++ )
        {
            len += sprintf(page + off + len,    "  queue%02d: %10u", i, DSL_QUEUE_TX_DROP_MIB_TABLE(i)->pdu);
            if( (i%4) == 3 ) len += sprintf(page + off + len,    "\n");
        }
        len += sprintf(page + off + len,    "\n");
    }
#endif
//    len += sprintf(page + off + len,    "DSL WAN VC MIB (2, 6, 9, 12)\n");
//    len += sprintf(page + off + len,    "  vc_rx_pdu:   %10u, %10u, %10u, %10u\n", DSL_WAN_VC_MIB_TABLE(2)->vc_rx_pdu,      DSL_WAN_VC_MIB_TABLE(6)->vc_rx_pdu,      DSL_WAN_VC_MIB_TABLE(9)->vc_rx_pdu,      DSL_WAN_VC_MIB_TABLE(12)->vc_rx_pdu);
//    len += sprintf(page + off + len,    "  vc_rx_bytes: %10u, %10u, %10u, %10u\n", DSL_WAN_VC_MIB_TABLE(2)->vc_rx_bytes,    DSL_WAN_VC_MIB_TABLE(6)->vc_rx_bytes,    DSL_WAN_VC_MIB_TABLE(9)->vc_rx_bytes,    DSL_WAN_VC_MIB_TABLE(12)->vc_rx_bytes);
//    len += sprintf(page + off + len,    "  vc_tx_pdu:   %10u, %10u, %10u, %10u\n", DSL_WAN_VC_MIB_TABLE(2)->vc_tx_pdu,      DSL_WAN_VC_MIB_TABLE(6)->vc_tx_pdu,      DSL_WAN_VC_MIB_TABLE(9)->vc_tx_pdu,      DSL_WAN_VC_MIB_TABLE(12)->vc_tx_pdu);
//    len += sprintf(page + off + len,    "  vc_tx_bytes: %10u, %10u, %10u, %10u\n", DSL_WAN_VC_MIB_TABLE(2)->vc_tx_bytes,    DSL_WAN_VC_MIB_TABLE(6)->vc_tx_bytes,    DSL_WAN_VC_MIB_TABLE(9)->vc_tx_bytes,    DSL_WAN_VC_MIB_TABLE(12)->vc_tx_bytes);

    *eof = 1;

    return len;
}

static int proc_write_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;
    u32 eth_clear;
    int i;
    struct eth_priv_data *priv = NULL;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

    eth_clear = 0;
    if ( stricmp(p, "clear") == 0 || stricmp(p, "clear all") == 0
        || stricmp(p, "clean") == 0 || stricmp(p, "clean all") == 0 )
        eth_clear = 3 | (3 << 6);
    else if ( stricmp(p, "clear eth0") == 0 || stricmp(p, "clear 0") == 0
            || stricmp(p, "clean eth0") == 0 || stricmp(p, "clean 0") == 0 )
        eth_clear = 1;
    else if ( stricmp(p, "clear eth1") == 0 || stricmp(p, "clear 1") == 0
            || stricmp(p, "clean eth1") == 0 || stricmp(p, "clean 1") == 0 )
        eth_clear = 2;
    else if ( stricmp(p, "clear atm") == 0 || stricmp(p, "clear 6") == 0 || stricmp(p, "clear 7") == 0
            || stricmp(p, "clean atm") == 0 || stricmp(p, "clean 6") == 0 || stricmp(p, "clean 7") == 0 )
        eth_clear = 3 << 6;
    else if ( stricmp(p, "queue") == 0 )
    {
        for ( i = 0; i < 16; i++ )
        {
            printk("Qid %2d: rx_pdu = %10u, rx_bytes = %10u, tx_pdu = %10u, tx_bytes = %10u\n", i,
                    DSL_QUEUE_RX_MIB_TABLE(i)->pdu, DSL_QUEUE_RX_MIB_TABLE(i)->bytes,
                    DSL_QUEUE_TX_MIB_TABLE(i)->pdu, DSL_QUEUE_TX_MIB_TABLE(i)->bytes);
        }
    }
    else if ( strincmp(p, "pvc ", 4) == 0 )
    {
        u32 vpi, vci;
        int conn;

        p += 4;
        rlen -= 4;
        ignore_space(&p, &rlen);
        vpi = get_number(&p, &rlen, 0);
        ignore_space(&p, &rlen);
        vci = get_number(&p, &rlen, 0);

        conn = find_vpivci(vpi, vci);

        printk("vpi = %u, vci = %u, conn = %d\n", vpi, vci, conn);

        if ( conn >= 0 )
        {
            printk("rx_packets  = %u\n", g_atm_priv_data.connection[conn].rx_packets);
            printk("rx_bytes    = %u\n", g_atm_priv_data.connection[conn].rx_bytes);
            printk("rx_errors   = %u\n", g_atm_priv_data.connection[conn].rx_errors);
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

    if ( (eth_clear & 1) )
    {
        eth_get_stats(g_eth_net_dev[0]);
        priv = netdev_priv(g_eth_net_dev[0]);
        //memset(&g_eth_priv_data[0], 0, (u32)&g_eth_priv_data[0].tx_dropped - (u32)&g_eth_priv_data[0] + sizeof(u32));
        memset(priv, 0, (u32)&priv->tx_dropped - (u32)priv + sizeof(u32));
        memset((void *)ITF_MIB_TBL(0), 0, sizeof(struct itf_mib));
    }
    if ( (eth_clear & 2) )
    {
        eth_get_stats(g_eth_net_dev[1]);
        priv = netdev_priv(g_eth_net_dev[1]);
        //memset(&g_eth_priv_data[1], 0, (u32)&g_eth_priv_data[1].tx_dropped - (u32)&g_eth_priv_data[1] + sizeof(u32));
        memset(priv, 0, (u32)&priv->tx_dropped - (u32)priv + sizeof(u32));
        memset((void *)ITF_MIB_TBL(1), 0, sizeof(struct itf_mib));
    }
    if ( (eth_clear & (3 << 6)) )
    {
        memset(&g_atm_priv_data.wrx_total_byte, 0, (u32)&g_atm_priv_data.prev_mib - (u32)&g_atm_priv_data.wrx_total_byte);
        for ( i = 0; i < ATM_QUEUE_NUMBER; i++ )
            memset(&g_atm_priv_data.connection[i].rx_packets, 0, (u32)&g_atm_priv_data.connection[i].port - (u32)&g_atm_priv_data.connection[i].rx_packets);
        memset((void *)ITF_MIB_TBL(6), 0, sizeof(struct itf_mib));
        memset((void *)ITF_MIB_TBL(7), 0, sizeof(struct itf_mib));
        memset((void *)DSL_WAN_MIB_TABLE, 0, sizeof(struct dsl_wan_mib_table));
//        for ( i = 0; i < 15; i++ )
//            memset((void *)DSL_WAN_VC_MIB_TABLE(i), 0, sizeof(struct dsl_wan_vc_mib_table));
        memset((void *)DSL_QUEUE_RX_MIB_TABLE(0), 0, sizeof(*DSL_QUEUE_RX_MIB_TABLE(0)) * 2);
        memset((void *)DSL_QUEUE_TX_MIB_TABLE(0), 0, sizeof(*DSL_QUEUE_TX_MIB_TABLE(0)) * 15);
        for ( i = 0; i < ATM_QUEUE_NUMBER; i++ )
        {
            memset(g_atm_priv_data.connection[i].prio_tx_packets, 0, 8 * sizeof(u32));
            if( ethwan== 0 )
                memset((void *)DSL_QUEUE_TX_DROP_MIB_TABLE(i), 0, sizeof(DSL_QUEUE_TX_DROP_MIB_TABLE(i)));
        }
    }

    return count;
}

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC

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
        "tcp",      //  31
        "help",     //  32
        "vci",      //  33
        "yes",      //  34
        "no",       //  35
        "qid",      //  36
        "outer",    //  37
        "IP",       //  38
        "IPv6",     //  39
    };

    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "ATM"};
    static const int dest_list_strlen[] = {4, 4, 4, 8, 8, 8, 8, 3};

    int state;              //  1: new,
    int prev_cmd;
    int operation;          //  1: add, 2: delete
    int type;               //  1: LAN, 2: WAN, 0: auto detect
    int entry;
    struct rout_forward_compare_tbl compare_tbl;
    struct rout_forward_action_tbl action_tbl;
    uint32_t ipv6_src_ip[4] = {0}, ipv6_dst_ip[4] = {0};
    int is_ipv6 = 0;
    unsigned int val[20];
    char local_buf[1024];
    int len;
    char *p1, *p2;
    int colon;
    int i, j;
    u32 mask;
    u32 bit;
    u32 *pu1, *pu2, *pu3;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
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

                            //  check for "new src mac index"
                            ignore_space(&p2, &len);
                            if ( strincmp(p2, "mac ", 4) == 0 )
                            {
                                state = 2;
                                prev_cmd = 5;
                                break;
                            }
                            else
                            {
                                get_ip_port(&p2, &len, val);
//                              printk("new src: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                                action_tbl.new_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                                action_tbl.new_port = val[4];
                            }
                        }
                        else
                            state = 0;
                    }
                    if ( state == 0 )
                    {
                        //  src
                        get_ip_port(&p2, &len, val);
//                      printk("src: %d.%d.%d.%d:%d\n", val[0], val[1], val[2], val[3], val[4]);
                        if ( val[5] == 4 )
                        {
                            if ( is_ipv6 )
                                ipv6_src_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            else
                                compare_tbl.src_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                        }
                        else
                        {
                            is_ipv6 = 1;
                            ipv6_src_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            ipv6_src_ip[1] = (val[6] << 24) | (val[7] << 16) | (val[8] << 8) | val[9];
                            ipv6_src_ip[2] = (val[10] << 24) | (val[11] << 16) | (val[12] << 8) | val[13];
                            ipv6_src_ip[3] = (val[14] << 24) | (val[15] << 16) | (val[16] << 8) | val[17];
                        }
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
                            if ( val[5] == 4 )
                            {
                                if ( is_ipv6 )
                                    ipv6_dst_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                                else
                                    compare_tbl.dest_ip = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                            }
                            else
                            {
                                is_ipv6 = 1;
                                ipv6_dst_ip[0] = (val[0] << 24) | (val[1] << 16) | (val[2] << 8) | val[3];
                                ipv6_dst_ip[1] = (val[6] << 24) | (val[7] << 16) | (val[8] << 8) | val[9];
                                ipv6_dst_ip[2] = (val[10] << 24) | (val[11] << 16) | (val[12] << 8) | val[13];
                                ipv6_dst_ip[3] = (val[14] << 24) | (val[15] << 16) | (val[16] << 8) | val[17];
                            }
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
                    else if ( state == 2 && prev_cmd == 5 )
                    {
                        state = 3;
                        prev_cmd = 7;
                        break;
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
                    else if ( state == 3 && prev_cmd == 7 )
                    {
                        //  new src mac index
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
                        action_tbl.new_src_mac_ix = val[0];
                    }
                    else if ( state == 2 && prev_cmd == 13 )
                    {
                        //  outer VLAN enable
                        //  outer VLAN index
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
//                      printk("outer VLAN insert: enable, index %d\n", val[0]);
                        if ( !action_tbl.out_vlan_ins )
                        {
                            action_tbl.out_vlan_ins = 1;
                            action_tbl.out_vlan_ix = val[0];
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
                    else if ( state == 1 && prev_cmd == 37 )
                    {
                        state = 2;
                        prev_cmd = 13;
                        printk("outer vlan\n");
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 14:
                    if ( (state == 1 || state == 2) && prev_cmd == 13 )
                    {
                        state++;
                        prev_cmd = 14;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 15:
                    if ( (state == 1 || state == 2) && prev_cmd == 13 )
                    {
                        state++;
                        prev_cmd = 15;
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
                                else if ( len > 3 && (p2[3] <= ' ' || p2[3] == ':') && strincmp(p2, "vci", 3) == 0 )
                                {
                                    p2 += 4;
                                    len -= 4;
                                    //  New VCI
                                    ignore_space(&p2, &len);
                                    val[0] = get_number(&p2, &len, 1);
//                                  printk(", vci 0x%04X", val[0]);
                                    if ( !action_tbl.in_vlan_ins )
                                    {
                                        action_tbl.in_vlan_ins = 1;
                                        action_tbl.new_in_vci = val[0];
                                    }
                                }
                            }
                            else
                            {
                                action_tbl.in_vlan_ins = 0;
                                action_tbl.new_in_vci = 0;
                            }
//                          printk("\n");
                        }
                        else if ( prev_cmd == 15 )
                        {
                            //  VLAN remove
//                          printk("VLAN remove: %s (%d)\n", command[i], i - 22);
                            action_tbl.in_vlan_rm = i - 22;
                        }
                    }
                    else if ( state == 3 )
                    {
                        if ( prev_cmd == 14 )
                        {
                            //  outer vlan insert
//                          printk("outer VLAN insert: %s (%d)", command[i], i - 22);
                            if ( (i - 22) )
                            {
                                ignore_space(&p2, &len);
                                if ( len > 5 && (p2[5] <= ' ' || p2[5] == ':') && strincmp(p2, "index", 5) == 0 )
                                {
                                    p2 += 6;
                                    len -= 6;
                                    //  outer VLAN index
                                    ignore_space(&p2, &len);
                                    val[0] = get_number(&p2, &len, 0);
//                                  printk(", index %d", val[0]);
                                    if ( !action_tbl.out_vlan_ins )
                                    {
                                        action_tbl.out_vlan_ins = 1;
                                        action_tbl.out_vlan_ix = val[0];
                                    }
                                }
                            }
                            else
                            {
                                action_tbl.out_vlan_ins = 0;
                                action_tbl.out_vlan_ix = 0;
                            }
//                          printk("\n");
                        }
                        else if ( prev_cmd == 15 )
                        {
                            //  outer VLAN remove
//                          printk("outer VLAN remove: %s (%d)\n", command[i], i - 22);
                            action_tbl.out_vlan_rm = i - 22;
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
                case 38:
                case 39:
                    i = 27;
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
                case 31:  // if this flag is not presented, it's UDP by default
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 31;
                        action_tbl.protocol = 1;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
                case 32:
                    printk("add\n");
                    printk("  LAN/WAN entry ???\n");
                    printk("    compare\n");
                    printk("      src:  ???.???.???.???:????\n");
                    printk("      dest: ???.???.???.???:????\n");
                    printk("    action\n");
                    printk("      new src/dest:      ???.???.???.???:????\n");
                    printk("      new MAC:           ??:??:??:??:??:?? (HEX)\n");
                    printk("      route type:        NULL/IP/NAT/NAPT\n");
                    printk("      new DSCP:          original/??\n");
                    printk("      MTU index:         ??\n");
                    printk("      VLAN insert:       disable/enable, VCI ???? (HEX)\n");
                    printk("      VLAN remove:       disable/enable\n");
                    printk("      dest list:         ETH0/ETH1/CPU0/EXT_INT?\n");
                    printk("      PPPoE mode:        transparent/termination\n");
                    printk("      PPPoE index:       ??\n");
                    printk("      new src MAC index: ??\n");
                    printk("      tcp:               yes/no\n");
                    printk("      dest qid:          ??\n");
                    printk("\n");
                    printk("delete\n");
                    printk("  LAN/WAN entry ???\n");
                    printk("    compare\n");
                    printk("      src:  ???.???.???.???:????\n");
                    printk("      dest: ???.???.???.???:????\n");
                    printk("\n");
                    state = prev_cmd = 0;
                    break;
                case 33:
                    if ( state == 1 && prev_cmd == 13 )
                    {
                        //  vlan vci
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 1);
                        if ( !action_tbl.in_vlan_ins )
                        {
                            action_tbl.in_vlan_ins = 1;
                            action_tbl.new_in_vci = val[0];
                        }
                    }
                    state = prev_cmd = 0;
                    break;
                case 34:
                    if ( state == 1 && prev_cmd == 31 )
                        //  tcp yes
                        action_tbl.protocol = 1;
                    state = prev_cmd = 0;
                    break;
                case 35:
                    if ( state == 1 && prev_cmd == 31 )
                        //  tcp no
                        action_tbl.protocol = 0;
                    state = prev_cmd = 0;
                    break;
                case 36:
                    if ( state == 1 && prev_cmd == 6 )
                    {
                        //  dest qid
                        ignore_space(&p2, &len);
                        val[0] = get_number(&p2, &len, 0);
                        action_tbl.dest_qid = val[0];
                    }
                    state = prev_cmd = 0;
                    break;
                case 37:
                    if ( !state )
                    {
                        state = 1;
                        prev_cmd = 37;
                    }
                    else
                        state = prev_cmd = 0;
                    break;
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
        u32 is_lan = 0;

        //  delete
        pu1 = (u32*)&compare_tbl;
        pu2 = NULL;
        pu3 = NULL;
        if ( entry < 0 )
        {
            //  search the entry number
            if ( *pu1 && pu1[1] )
            {
                if ( (!type || type == 1) )
                {
                    //  LAN
                    for ( entry = 0; entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK; entry++ )
                    {
                        pu2 = (u32*)ROUT_LAN_HASH_CMP_TBL(entry);
                        if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                        {
                            pu3 = (u32*)ROUT_LAN_HASH_ACT_TBL(entry);
                            break;
                        }
                    }
                    if ( entry == MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        for ( entry = 0; entry < LAN_ROUT_TBL_CFG->rout_num; entry++ )
                        {
                            pu2 = (u32*)ROUT_LAN_COLL_CMP_TBL(entry);
                            if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                            {
                                pu3 = (u32*)ROUT_LAN_COLL_ACT_TBL(entry);
                                break;
                            }
                        }
                        if ( entry == LAN_ROUT_TBL_CFG->rout_num )
                            pu2 = NULL;
                        else
                            entry += MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK;
                    }
                    if ( pu3 != NULL )
                        is_lan = 1;
                }
                if ( (!type && !pu2) || type == 2 )
                {
                    //  WAN
                    for ( entry = 0; entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK; entry++ )
                    {
                        pu2 = (u32*)ROUT_WAN_HASH_CMP_TBL(entry);
                        if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                        {
                            pu3 = (u32*)ROUT_WAN_HASH_ACT_TBL(entry);
                            break;
                        }
                    }
                    if ( entry == MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        for ( entry = 0; entry < WAN_ROUT_TBL_CFG->rout_num; entry++ )
                        {
                            pu2 = (u32*)ROUT_WAN_COLL_CMP_TBL(entry);
                            if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                            {
                                pu3 = (u32*)ROUT_WAN_COLL_ACT_TBL(entry);
                                break;
                            }
                        }
                        if ( entry == WAN_ROUT_TBL_CFG->rout_num )
                            pu2 = NULL;
                        else
                            entry += MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK;
                    }
                    if ( pu3 != NULL )
                        is_lan = 0;
                }
            }
        }
        else
        {
            if ( *pu1 && pu1[1] )
            {
                pu3 = NULL;
                //  check compare
                if ( !type || type == 1 )
                {
                    //  LAN
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_LAN_HASH_CMP_TBL(entry);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_LAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + LAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_LAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_LAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 1;
                }
                if ( !type || type == 2 )
                {
                    //  WAN
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_WAN_HASH_CMP_TBL(entry);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_WAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + WAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_WAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        if ( *pu2 != *pu1 || pu2[1] != pu1[1] || pu2[2] != pu1[2] )
                            pu2 = NULL;
                        else
                            pu3 = (u32*)ROUT_WAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 0;
                }
            }
            else if ( !*pu1 && !pu1[1] )
            {
                if ( type == 1 )
                {
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_LAN_HASH_CMP_TBL(entry);
                        pu3 = (u32*)ROUT_LAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + LAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_LAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        pu3 = (u32*)ROUT_LAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 1;
                }
                else if ( type == 2 )
                {
                    if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK )
                    {
                        pu2 = (u32*)ROUT_WAN_HASH_CMP_TBL(entry);
                        pu3 = (u32*)ROUT_WAN_HASH_ACT_TBL(entry);
                    }
                    else if ( entry < MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK + WAN_ROUT_TBL_CFG->rout_num )
                    {
                        pu2 = (u32*)ROUT_WAN_COLL_CMP_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                        pu3 = (u32*)ROUT_WAN_COLL_ACT_TBL(entry - MAX_ROUTING_ENTRIES_PER_HASH_BLOCK * MAX_HASH_BLOCK);
                    }
                    if ( pu3 != NULL )
                        is_lan = 0;
                }
            }
        }
#if 0
        if ( pu2 )
        {
            *pu2 = 0;
            *(pu2 + 1) = 0;
            *(pu2 + 2) = 0;
        }
        if ( pu3 )
        {
            *pu3 = 0;
            *(pu3 + 1) = 0;
            *(pu3 + 2) = 0;
            *(pu3 + 3) = 0;
            *(pu3 + 4) = 0;
            *(pu3 + 5) = 0;
        }
#else
        if ( pu2 && pu3 )
        {
            printk("verify only, no firmware table operation\n");

            is_lan <<= 31;
            printk("del_routing_entry(%d - %08x)\n", entry, (u32)entry | is_lan);
        }
#endif
    }
    else if ( operation == 1 && type && ((!is_ipv6 && *(u32*)&compare_tbl && *((u32*)&compare_tbl + 1)) || (is_ipv6 && *((u32*)&compare_tbl + 2))) )
    {
#if 0   //  TODO
        pu2 = NULL;
        if ( entry < 0 )
        {
            int max_entry;

            //  add
            pu1 = (u32*)&compare_tbl;
            pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
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
#else
        char str_to_print[1024];

        printk("verify only, no firmware table operation\n");

        if ( is_ipv6 )
        {
            printk("ipv6 src ip:  %d.%d.%d.%d", ipv6_src_ip[0] >> 24, (ipv6_src_ip[0] >> 16) & 0xFF, (ipv6_src_ip[0] >> 8) & 0xFF, ipv6_src_ip[0] & 0xFF);
            printk(".%d.%d.%d.%d", ipv6_src_ip[1] >> 24, (ipv6_src_ip[1] >> 16) & 0xFF, (ipv6_src_ip[1] >> 8) & 0xFF, ipv6_src_ip[1] & 0xFF);
            printk(".%d.%d.%d.%d", ipv6_src_ip[2] >> 24, (ipv6_src_ip[2] >> 16) & 0xFF, (ipv6_src_ip[2] >> 8) & 0xFF, ipv6_src_ip[2] & 0xFF);
            printk(".%d.%d.%d.%d", ipv6_src_ip[3] >> 24, (ipv6_src_ip[3] >> 16) & 0xFF, (ipv6_src_ip[3] >> 8) & 0xFF, ipv6_src_ip[3] & 0xFF);
            printk("\n");
            printk("ipv6 dest ip: %d.%d.%d.%d", ipv6_dst_ip[0] >> 24, (ipv6_dst_ip[0] >> 16) & 0xFF, (ipv6_dst_ip[0] >> 8) & 0xFF, ipv6_dst_ip[0] & 0xFF);
            printk(".%d.%d.%d.%d", ipv6_dst_ip[1] >> 24, (ipv6_dst_ip[1] >> 16) & 0xFF, (ipv6_dst_ip[1] >> 8) & 0xFF, ipv6_dst_ip[1] & 0xFF);
            printk(".%d.%d.%d.%d", ipv6_dst_ip[2] >> 24, (ipv6_dst_ip[2] >> 16) & 0xFF, (ipv6_dst_ip[2] >> 8) & 0xFF, ipv6_dst_ip[2] & 0xFF);
            printk(".%d.%d.%d.%d", ipv6_dst_ip[3] >> 24, (ipv6_dst_ip[3] >> 16) & 0xFF, (ipv6_dst_ip[3] >> 8) & 0xFF, ipv6_dst_ip[3] & 0xFF);
            printk("\n");
        }
        print_route(str_to_print, 0, type == 1 ? 1 : 0, &compare_tbl, &action_tbl);
        printk(str_to_print);
#endif
    }

    return count;
}

static int proc_read_queue(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *mpoa_type_str[] = {"EoA w/o FCS", "EoA w FCS", "PPPoA", "IPoA"};

    int len = 0;
    int len_max = off + count;
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

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "RX Queue Config (0x%08X):\n", (u32)WRX_QUEUE_CONFIG(0));
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_QUEUE_OVERRUN_END;

    for ( i = 0; i < 16; i++ )
    {
        rx = *WRX_QUEUE_CONFIG(i);
        llen  = sprintf(str       ,  "  %d: MPoA type - %s, MPoA mode - %s, IP version %d\n", i, mpoa_type_str[rx.mpoa_type], rx.mpoa_mode ? "LLC" : "VC mux", rx.ip_ver ? 6 : 4);
        llen += sprintf(str + llen,  "     Oversize - %d, Undersize - %d, Max Frame size - %d\n", rx.oversize, rx.undersize, rx.mfs);
        llen += sprintf(str + llen,  "     uu mask - 0x%02X, cpi mask - 0x%02X, uu exp - 0x%02X, cpi exp - 0x%02X\n", rx.uumask, rx.cpimask, rx.uuexp, rx.cpiexp);
        if ( rx.vlan_ins )
            llen += sprintf(str + llen, "     new_vlan = 0x%08X\n", rx.new_vlan);

        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_QUEUE_OVERRUN_END;
    }

    llen = sprintf(str, "TX Queue Config (0x%08X):\n", (u32)WTX_QUEUE_CONFIG(0));
    if ( len <= off && len + llen > off )
    {
        my_memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        my_memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_QUEUE_OVERRUN_END;

    for ( i = 0; i < 15; i++ )
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

        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_QUEUE_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_QUEUE_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_queue(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
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
    int len_max = off + count;
    char *pstr;
    char str[1024];
    int llen;

    int htuts = *CFG_WRX_HTUTS;
    int i;

    pstr = *start = page;

    __sync();

    for ( i = 0; i < htuts; i++ )
    {
        llen = print_htu(str, i);
        if ( len <= off && len + llen > off )
        {
            my_memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            my_memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_HTU_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_HTU_OVERRUN_END:

    return len - llen - off;
}
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "error print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ERR)              ? "enabled" : "disabled");
    len += sprintf(page + off + len, "debug print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "assert           - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ASSERT)           ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump rx skb      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_RX)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump tx skb      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_TX)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump flag header - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_FLAG_HEADER) ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump init        - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_INIT)        ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump qos         - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS)         ? "enabled" : "disabled");
  #if defined(ENABLE_FW_MODULE_TEST) && ENABLE_FW_MODULE_TEST
    len += sprintf(page + off + len, "FW Mode          - DSL WAN\n");
    len += sprintf(page + off + len, "WAN Hash Algo    - %s\n", (ppe_wan_hash_algo)   ? "Algo 1"   : "Algo 0");
    len += sprintf(page + off + len, "ACC Protocol     - %s\n", (acc_proto)        ? "TCP" : "UDP");
    len += sprintf(page + off + len, "MPoA Type        - EoA1\n");
  #endif

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

  #if defined(ENABLE_FW_MODULE_TEST) && ENABLE_FW_MODULE_TEST
    if ( strincmp(p, "set_acc_udp", 11) == 0 ) {
    if (acc_proto != 0) {
        acc_proto = 0;
        init_acc_tables();
    }
    }
    else if ( strincmp(p, "set_acc_tcp", 11) == 0 ) {
    if (acc_proto != 1) {
        acc_proto = 1;
        init_acc_tables();
    }
    }
    else if ( strincmp(p, "set_mix_mode", 12) == 0 ) {
    if (ppe_fw_mode != 1) {
        ppe_fw_mode = 1;
        init_acc_tables();
    }
    }
    else if ( strincmp(p, "set_normal_mode", 15) == 0 ) {
    if (ppe_fw_mode != 0) {
        ppe_fw_mode = 0;
        init_acc_tables();
    }
    }
    else if ( strincmp(p, "set_algo_0", 10) == 0 ) {
    if (ppe_wan_hash_algo != 0) {
        ppe_wan_hash_algo = 0;
        init_acc_tables();
    }
    }
    else if ( strincmp(p, "set_algo_1", 10) == 0 ) {
    if (ppe_wan_hash_algo != 1) {
        ppe_wan_hash_algo = 1;
        init_acc_tables();
    }
    }
    else
  #endif
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
  #if defined(ENABLE_FW_MODULE_TEST) && ENABLE_FW_MODULE_TEST
        printk("echo set_mix_mode/set_normal_mode/set_algo_0/set_algo_1\n");
        printk("echo set_acc_udp/set_acc_tcp\n");
  #endif
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

    return count;
}
#endif

#define PP32_REG_ADDR_BEGIN     0x0
#define PP32_REG_ADDR_END       0x1FFF
#define PP32_SB_ADDR_BEGIN      0x2000
#define PP32_SB_ADDR_END        0x4BFF

static inline unsigned long sb_addr_to_fpi_addr_convert(unsigned long sb_addr)
{
    if ( sb_addr <= PP32_SB_ADDR_END) {
        return (unsigned long ) SB_BUFFER(sb_addr);
    }
    else {
        return sb_addr;
    }
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
    int is_switch = 0;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
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
        if ( p2[0] == 's' && p2[1] == 'w' && (p2[2] == ' ' || p2[2] == '\t') )
        {
            unsigned long temp;

            is_switch = 1;
            p2 += 3;
            len -= 3;
            ignore_space(&p2, &len);
            temp = get_number(&p2, &len, 1);
            p = (unsigned long *)AR10_SWIP_MACRO_REG(temp);
        }
        else
        {
            p = (unsigned long *)get_number(&p2, &len, 1);
            p = (unsigned long *)sb_addr_to_fpi_addr_convert( (unsigned long) p);
        }

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
        p = (unsigned long *)sb_addr_to_fpi_addr_convert( (unsigned long) p);

        if ( (u32)p >= KSEG0 )
        {
            ignore_space(&p2, &len);
            n = (int)get_number(&p2, &len, 0);
            if ( n )
            {
                char str[32] = {0};
                char *pch = str;
                int k;
                u32 data;
                char c;

                n += (l = ((int)p >> 2) & 0x03);
                p = (unsigned long *)((u32)p & ~0x0F);
                for ( i = 0; i < n; i++ )
                {
                    if ( (i & 0x03) == 0 )
                    {
                        printk("%08X:", (u32)p);
                        pch = str;
                    }
                    if ( i < l )
                    {
                        printk("         ");
                        sprintf(pch, "    ");
                    }
                    else
                    {
                        data = (u32)*p;
                        printk(" %08X", data);
                        for ( k = 0; k < 4; k++ )
                        {
                            c = ((char*)&data)[k];
                            pch[k] = c < ' ' ? '.' : c;
                        }
                    }
                    p++;
                    pch += 4;
                    if ( (i & 0x03) == 0x03 )
                    {
                        pch[0] = 0;
                        printk(" ; %s\n", str);
                    }
                }
                if ( (n & 0x03) != 0x00 )
                {
                    for ( k = 4 - (n & 0x03); k > 0; k-- )
                        printk("         ");
                    pch[0] = 0;
                    printk(" ; %s\n", str);
                }
            }
        }
    }

    return count;
}
#endif

#if defined(DEBUG_PP32_PROC) && DEBUG_PP32_PROC
static int proc_read_pp32(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *stron = " on";
    static const char *stroff = "off";

    int len = 0;
    int cur_context;
    int f_stopped = 0;
    char str[256];
    char strlength;
    int i, j;

    if ( PP32_CPU_USER_STOPPED || PP32_CPU_USER_BREAKIN_RCV || PP32_CPU_USER_BREAKPOINT_MET )
    {
        strlength = 0;
        if ( PP32_CPU_USER_STOPPED )
            strlength += sprintf(str + strlength, "stopped");
        if ( PP32_CPU_USER_BREAKPOINT_MET )
            strlength += sprintf(str + strlength, strlength ? " | breakpoint" : "breakpoint");
        if ( PP32_CPU_USER_BREAKIN_RCV )
            strlength += sprintf(str + strlength, strlength ? " | breakin" : "breakin");
        f_stopped = 1;
    }
    else if ( PP32_CPU_CUR_PC == PP32_CPU_CUR_PC )
    {
        sprintf(str, "stopped");
        f_stopped = 1;
    }
    else
        sprintf(str, "running");
    cur_context = PP32_BRK_CUR_CONTEXT;
    len += sprintf(page + off + len, "Context: %d, PC: 0x%04x, %s\n", cur_context, PP32_CPU_CUR_PC, str);

    if ( PP32_CPU_USER_BREAKPOINT_MET )
    {
        strlength = 0;
        if ( PP32_BRK_PC_MET(0) )
            strlength += sprintf(str + strlength, "pc0");
        if ( PP32_BRK_PC_MET(1) )
            strlength += sprintf(str + strlength, strlength ? " | pc1" : "pc1");
        if ( PP32_BRK_DATA_ADDR_MET(0) )
            strlength += sprintf(str + strlength, strlength ? " | daddr0" : "daddr0");
        if ( PP32_BRK_DATA_ADDR_MET(1) )
            strlength += sprintf(str + strlength, strlength ? " | daddr1" : "daddr1");
        if ( PP32_BRK_DATA_VALUE_RD_MET(0) )
        {
            strlength += sprintf(str + strlength, strlength ? " | rdval0" : "rdval0");
            if ( PP32_BRK_DATA_VALUE_RD_LO_EQ(0) )
            {
                if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(0) )
                    strlength += sprintf(str + strlength, " ==");
                else
                    strlength += sprintf(str + strlength, " <=");
            }
            else if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(0) )
                strlength += sprintf(str + strlength, " >=");
        }
        if ( PP32_BRK_DATA_VALUE_RD_MET(1) )
        {
            strlength += sprintf(str + strlength, strlength ? " | rdval1" : "rdval1");
            if ( PP32_BRK_DATA_VALUE_RD_LO_EQ(1) )
            {
                if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(1) )
                    strlength += sprintf(str + strlength, " ==");
                else
                    strlength += sprintf(str + strlength, " <=");
            }
            else if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(1) )
                strlength += sprintf(str + strlength, " >=");
        }
        if ( PP32_BRK_DATA_VALUE_WR_MET(0) )
        {
            strlength += sprintf(str + strlength, strlength ? " | wtval0" : "wtval0");
            if ( PP32_BRK_DATA_VALUE_WR_LO_EQ(0) )
            {
                if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(0) )
                    strlength += sprintf(str + strlength, " ==");
                else
                    strlength += sprintf(str + strlength, " <=");
            }
            else if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(0) )
                strlength += sprintf(str + strlength, " >=");
        }
        if ( PP32_BRK_DATA_VALUE_WR_MET(1) )
        {
            strlength += sprintf(str + strlength, strlength ? " | wtval1" : "wtval1");
            if ( PP32_BRK_DATA_VALUE_WR_LO_EQ(1) )
            {
                if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(1) )
                    strlength += sprintf(str + strlength, " ==");
                else
                    strlength += sprintf(str + strlength, " <=");
            }
            else if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(1) )
                strlength += sprintf(str + strlength, " >=");
        }
        len += sprintf(page + off + len, "break reason: %s\n", str);
    }

    if ( f_stopped )
    {
        len += sprintf(page + off + len, "General Purpose Register (Context %d):\n", cur_context);
        for ( i = 0; i < 4; i++ )
        {
            for ( j = 0; j < 4; j++ )
                len += sprintf(page + off + len, "   %2d: %08x", i + j * 4, *PP32_GP_CONTEXTi_REGn(cur_context, i + j * 4));
            len += sprintf(page + off + len, "\n");
        }
    }

    len += sprintf(page + off + len, "break out on: break in - %s, stop - %s\n",
                                        PP32_CTRL_OPT_BREAKOUT_ON_BREAKIN ? stron : stroff,
                                        PP32_CTRL_OPT_BREAKOUT_ON_STOP ? stron : stroff);
    len += sprintf(page + off + len, "     stop on: break in - %s, break point - %s\n",
                                        PP32_CTRL_OPT_STOP_ON_BREAKIN ? stron : stroff,
                                        PP32_CTRL_OPT_STOP_ON_BREAKPOINT ? stron : stroff);
    len += sprintf(page + off + len, "breakpoint:\n");
    len += sprintf(page + off + len, "     pc0: 0x%08x, %s\n", *PP32_BRK_PC(0), PP32_BRK_GRPi_PCn(0, 0) ? "group 0" : "off");
    len += sprintf(page + off + len, "     pc1: 0x%08x, %s\n", *PP32_BRK_PC(1), PP32_BRK_GRPi_PCn(1, 1) ? "group 1" : "off");
    len += sprintf(page + off + len, "  daddr0: 0x%08x, %s\n", *PP32_BRK_DATA_ADDR(0), PP32_BRK_GRPi_DATA_ADDRn(0, 0) ? "group 0" : "off");
    len += sprintf(page + off + len, "  daddr1: 0x%08x, %s\n", *PP32_BRK_DATA_ADDR(1), PP32_BRK_GRPi_DATA_ADDRn(1, 1) ? "group 1" : "off");
    len += sprintf(page + off + len, "  rdval0: 0x%08x\n", *PP32_BRK_DATA_VALUE_RD(0));
    len += sprintf(page + off + len, "  rdval1: 0x%08x\n", *PP32_BRK_DATA_VALUE_RD(1));
    len += sprintf(page + off + len, "  wrval0: 0x%08x\n", *PP32_BRK_DATA_VALUE_WR(0));
    len += sprintf(page + off + len, "  wrval1: 0x%08x\n", *PP32_BRK_DATA_VALUE_WR(1));

    *eof = 1;

    return len;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    u32 addr;

    int len, rlen;

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

    if ( stricmp(p, "start") == 0 )
        *PP32_CTRL_CMD = PP32_CTRL_CMD_RESTART;
    else if ( stricmp(p, "stop") == 0 )
        *PP32_CTRL_CMD = PP32_CTRL_CMD_STOP;
    else if ( stricmp(p, "step") == 0 )
        *PP32_CTRL_CMD = PP32_CTRL_CMD_STEP;
    else if ( strincmp(p, "pc0 ", 4) == 0 )
    {
        p += 4;
        rlen -= 4;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG = PP32_BRK_GRPi_PCn_OFF(0, 0);
            *PP32_BRK_PC_MASK(0) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_PC(0) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_PC(0) = addr;
            *PP32_BRK_PC_MASK(0) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG = PP32_BRK_GRPi_PCn_ON(0, 0);
        }
    }
    else if ( strincmp(p, "pc1 ", 4) == 0 )
    {
        p += 4;
        rlen -= 4;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG = PP32_BRK_GRPi_PCn_OFF(1, 1);
            *PP32_BRK_PC_MASK(1) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_PC(1) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_PC(1) = addr;
            *PP32_BRK_PC_MASK(1) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG = PP32_BRK_GRPi_PCn_ON(1, 1);
        }
    }
    else if ( strincmp(p, "daddr0 ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG = PP32_BRK_GRPi_DATA_ADDRn_OFF(0, 0);
            *PP32_BRK_DATA_ADDR_MASK(0) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_DATA_ADDR(0) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_DATA_ADDR(0) = addr;
            *PP32_BRK_DATA_ADDR_MASK(0) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG = PP32_BRK_GRPi_DATA_ADDRn_ON(0, 0);
        }
    }
    else if ( strincmp(p, "daddr1 ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG = PP32_BRK_GRPi_DATA_ADDRn_OFF(1, 1);
            *PP32_BRK_DATA_ADDR_MASK(1) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_DATA_ADDR(1) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_DATA_ADDR(1) = addr;
            *PP32_BRK_DATA_ADDR_MASK(1) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG = PP32_BRK_GRPi_DATA_ADDRn_ON(1, 1);
        }
    }
    else
    {

        printk("echo \"<command>\" > /proc/eth/pp32\n");
        printk("  command:\n");
        printk("    start  - run pp32\n");
        printk("    stop   - stop pp32\n");
        printk("    step   - run pp32 with one step only\n");
        printk("    pc0    - pc0 <addr>/off, set break point PC0\n");
        printk("    pc1    - pc1 <addr>/off, set break point PC1\n");
        printk("    daddr0 - daddr0 <addr>/off, set break point data address 0\n");
        printk("    daddr0 - daddr1 <addr>/off, set break point data address 1\n");
        printk("    help   - print this screen\n");
    }

    if ( *PP32_BRK_TRIG )
        *PP32_CTRL_OPT = PP32_CTRL_OPT_STOP_ON_BREAKPOINT_ON;
    else
        *PP32_CTRL_OPT = PP32_CTRL_OPT_STOP_ON_BREAKPOINT_OFF;

    return count;
}
#endif

static int proc_read_burstlen(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    u32 dma_ps;
    u32 dma_pctrl;
    unsigned long sys_flag;

    local_irq_save(sys_flag);
    dma_ps = *(volatile u32 *)0xBE104140;
    *(volatile u32 *)0xBE104140 = 0;
    dma_pctrl = *(volatile u32 *)0xBE104144;
    *(volatile u32 *)0xBE104140 = dma_ps;
    local_irq_restore(sys_flag);

    len += sprintf(page + off + len, "DMA-PPE burst length: Rx %d, Tx %d\n", 1 << ((dma_pctrl >> 2) & 0x03), 1 << ((dma_pctrl >> 4) & 0x03));

    *eof = 1;

    return len;
}

static int proc_write_burstlen(struct file *file, const char *buf, unsigned long count, void *data)
{
#if 0
    char str[2048];
    char *p;

    int len, rlen;

    int burstlen;
    u32 dma_ps;
    u32 sys_flag;

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
            burstlen = 0x3C;   break;
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

    return count;
#else
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 0;
    int mask = 0x3C;    //  rx: 0x0C, tx: 0x30, all: 0x3C
    int burstlen = 0;
    int burstlen_mask = 0;
    int f_help = 0;
    u32 dma_ps;
    unsigned long sys_flag;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "rx") == 0 )
            burstlen_mask |= (mask = 0x0C);
        else if ( stricmp(p1, "tx") == 0 )
            burstlen_mask |= (mask = 0x30);
        else if ( stricmp(p1, "all") == 0 )
            burstlen_mask |= (mask = 0x3C);
        else if ( strcmp(p1, "2") == 0 )
            burstlen = 0x14 & mask;
        else if ( strcmp(p1, "4") == 0 )
            burstlen = 0x28 & mask;
        else if ( strcmp(p1, "8") == 0 )
            burstlen = 0x3C & mask;
        else
        {
            f_help = 1;
            break;
        }

        p1 = p2;
    }

    if ( !burstlen_mask && burstlen )
        burstlen_mask = 0x3C;

    if ( !burstlen_mask || !burstlen )
        f_help = 1;

    if ( !f_help )
    {
        local_irq_save(sys_flag);
        dma_ps = *(volatile u32 *)0xBE104140;
        *(volatile u32 *)0xBE104140 = 0;
        *(volatile u32 *)0xBE104144 = (*(volatile u32 *)0xBE104144 & ~burstlen_mask) | burstlen;
        *(volatile u32 *)0xBE104140 = dma_ps;
        local_irq_restore(sys_flag);
    }
    else
    {
        printk("echo [rx/tx/all] <2/4/8> > /proc/eth/burstlen\n");
    }

    return count;
#endif
}

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
static int proc_read_fw(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

//    u32 sll_state;
    u32 tir4;
    u32 tir4_sll_state;
    u32 tir4_dplus_rx_on;

    len += sprintf(page + off + len, "Firmware\n");
//    len += sprintf(page + off + len, "  ACC_ERX_PID        = %08X\n", *ACC_ERX_PID);
//    len += sprintf(page + off + len, "  ACC_ERX_PORT_TIMES = %08X\n", *ACC_ERX_PORT_TIMES);

//    sll_state = *SLL_ISSUED;
    tir4 = *TIR(4);
    tir4_sll_state = (tir4 >> 21) & 7;
    tir4_dplus_rx_on = (tir4 >> 20) & 1;

    /*
    len += sprintf(page + off + len, "  SLL_ISSUED         = %d [%s], tir4.sll_state = %d [%s]\n",
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

    len += sprintf(page + off + len, "  DPLUS_RX_ON        = %d, tir4.dplus_rx_on = %d\n",
            *DPLUS_RX_ON, tir4_dplus_rx_on);


    len += sprintf(page + off + len, "  BMC_ISSUED         = %08X\n", *BMC_ISSUED);

    len += sprintf(page + off + len, "  PRESEARCH_RDPTR    = %08X, %08X\n",
            *PRESEARCH_RDPTR, *(PRESEARCH_RDPTR + 1));

    len += sprintf(page + off + len, "  SLL_ERX_PID        = %08X\n", *SLL_ERX_PID);

    len += sprintf(page + off + len, "  SLL_PKT_CNT        = %08X, %08X\n",
            *SLL_PKT_CNT, *(SLL_PKT_CNT + 1));

    len += sprintf(page + off + len, "  SLL_RDPTR          = %08X, %08X\n",
            *SLL_RDPTR, *(SLL_RDPTR + 1));


    len += sprintf(page + off + len, "  EDIT_PKT_CNT       = %08X, %08X\n",
            *EDIT_PKT_CNT, *(EDIT_PKT_CNT + 1));
    len += sprintf(page + off + len, "  EDIT_RDPTR         = %08X, %08X\n",
            *EDIT_RDPTR, *(EDIT_RDPTR + 1));

    len += sprintf(page + off + len, "  DPLUSRX_PKT_CNT    = %08X, %08X\n",
            *DPLUSRX_PKT_CNT, *(DPLUSRX_PKT_CNT + 1));
    len += sprintf(page + off + len, "  DPLUS_RDPTR        = %08X, %08X\n",
            *DPLUS_RDPTR, *(DPLUS_RDPTR + 1));

    len += sprintf(page + off + len, "  POSTSEARCH_RDPTR   = %08X, %08X\n", */

    len += sprintf(page + off + len, "\nQoS:\n");
    len += sprintf(page + off + len, "  DSLWAN_TX_SWAP_RDPTR = %04X, %04X, %04X, %04X, %04X, %04X, %04X, %04X\n %04X, %04X, %04X, %04X, %04X, %04X, %04X\n",
                                            * __DSLWAN_TXDES_SWAP_PTR(0), * __DSLWAN_TXDES_SWAP_PTR(1),
                                            * __DSLWAN_TXDES_SWAP_PTR(2), * __DSLWAN_TXDES_SWAP_PTR(3),
                                            * __DSLWAN_TXDES_SWAP_PTR(4), * __DSLWAN_TXDES_SWAP_PTR(5),
                                            * __DSLWAN_TXDES_SWAP_PTR(6), * __DSLWAN_TXDES_SWAP_PTR(7),
                                           * __DSLWAN_TXDES_SWAP_PTR(8), * __DSLWAN_TXDES_SWAP_PTR(9),
                                            * __DSLWAN_TXDES_SWAP_PTR(10), * __DSLWAN_TXDES_SWAP_PTR(11),
                                            * __DSLWAN_TXDES_SWAP_PTR(12), * __DSLWAN_TXDES_SWAP_PTR(13),
                                            * __DSLWAN_TXDES_SWAP_PTR(14));

    len += sprintf(page + off + len, "  DSLWAN_TX_PKT_CNT    = %04X, %04X, %04X, %04X, %04X, %04X, %04X, %04X\n %04X, %04X, %04X, %04X, %04X, %04X, %04X\n ",
                                            * __DSLWAN_TX_PKT_CNT(0), * __DSLWAN_TX_PKT_CNT(1),
                                            * __DSLWAN_TX_PKT_CNT(2), * __DSLWAN_TX_PKT_CNT(3),
                                            * __DSLWAN_TX_PKT_CNT(4), * __DSLWAN_TX_PKT_CNT(5),
                                            * __DSLWAN_TX_PKT_CNT(6), * __DSLWAN_TX_PKT_CNT(7),
                                            * __DSLWAN_TX_PKT_CNT(8), * __DSLWAN_TX_PKT_CNT(9),
                                            * __DSLWAN_TX_PKT_CNT(10), * __DSLWAN_TX_PKT_CNT(11),
                                            * __DSLWAN_TX_PKT_CNT(12), * __DSLWAN_TX_PKT_CNT(13),
                                            * __DSLWAN_TX_PKT_CNT(14));


    len += sprintf(page + off + len, "  ENETS PGNUM             = %d\n",  __ENETS_PGNUM);
    len += sprintf(page + off + len, "  ENETS DBUF BASE ADDR     = %08X\n",  (unsigned int)__ERX_DBUF_BASE);
    len += sprintf(page + off + len, "  ENETS CBUF BASE ADDR     = %08X\n",  (unsigned int)__ERX_CBUF_BASE);

    len += sprintf(page + off + len, "  QOSD_DPLUS_RDPTR     = %04X\n",
                                            * __DSLWAN_FP_RXDES_SWAP_RDPTR);

    len += sprintf(page + off + len, "  QOSD_CPUTX_RDPTR     = %04X\n",
                                            * __CPU_TXDES_SWAP_RDPTR);

    len += sprintf(page + off + len, "  DPLUS_RXDES_RDPTR    = %04X\n",
                                            * __DSLWAN_FP_RXDES_DPLUS_WRPTR);


    len += sprintf(page + off + len, "  pre_dplus_ptr    = %04X\n", *PRE_DPLUS_PTR);

    len += sprintf(page + off + len, "  pre_dplus_cnt    = %04X\n", (*DM_RXPKTCNT) & 0xff);

    len += sprintf(page + off + len, "  dplus_ptr        = %04X\n", *DPLUS_PTR);

    len += sprintf(page + off + len, "  dplus_cnt        = %04X\n", *DPLUS_CNT);

    len += sprintf(page + off + len, "  DPLUS_RX_ON      = %d\n",  *DPLUS_RX_ON);

    len += sprintf(page + off + len, "  ISR_IS           = %08X\n ",  *ISR_IS);

    len += sprintf(page + off + len, "\nQoS Mib:\n");
    len += sprintf(page + off + len, "  cputx_pkts:             %u\n",   *__CPU_TO_DSLWAN_TX_PKTS);
    len += sprintf(page + off + len, "  cputx_bytes:            %u\n",   *__CPU_TO_DSLWAN_TX_BYTES);
    len += sprintf(page + off + len, "  cputx_drop_pkts:        %u\n",   *__CPU_TX_SWAPPER_DROP_PKTS);
    len += sprintf(page + off + len, "  cputx_drop_bytess:      %u\n",   *__CPU_TX_SWAPPER_DROP_BYTES);

    len += sprintf(page + off + len, "  dslwan_fp_drop_pkts:    %u\n",   *__DSLWAN_FP_SWAPPER_DROP_PKTS );
    len += sprintf(page + off + len, "  dslwan_fp_drop_bytes:   %u\n",   *__DSLWAN_FP_SWAPPER_DROP_BYTES );

    len += sprintf(page + off + len, "  dslwan_tx_qf_drop_pkts:  (%u, %u, %u, %u, %u, %u, %u, %u ,%u, %u, %u, %u, %u, %u, %u)\n",
                 *__DSLWAN_TX_THRES_DROP_PKT_CNT(0),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(1),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(2) ,  *__DSLWAN_TX_THRES_DROP_PKT_CNT(3) ,
                 *__DSLWAN_TX_THRES_DROP_PKT_CNT(4),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(5),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(6) ,  *__DSLWAN_TX_THRES_DROP_PKT_CNT(7),
                 *__DSLWAN_TX_THRES_DROP_PKT_CNT(8),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(9),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(10),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(11),
                 *__DSLWAN_TX_THRES_DROP_PKT_CNT(12),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(13),  *__DSLWAN_TX_THRES_DROP_PKT_CNT(14));

    *eof = 1;

    return len;
}

static int proc_write_fw(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen, i;

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

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clean") == 0 )
    {
       for( i=0;i<15;i++)
               * __DSLWAN_TX_THRES_DROP_PKT_CNT(i) = 0;

        * __CPU_TO_DSLWAN_TX_PKTS  = 0;
        * __CPU_TO_DSLWAN_TX_BYTES = 0;


        * __CPU_TX_SWAPPER_DROP_PKTS    = 0;
        * __CPU_TX_SWAPPER_DROP_BYTES   = 0;
        * __DSLWAN_FP_SWAPPER_DROP_PKTS = 0;
        * __DSLWAN_FP_SWAPPER_DROP_PKTS = 0;
    }

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
    skb = alloc_skb_tx(DMA_PACKET_SIZE);
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
        err("Cell not alloc tx connection");
        return -EIO;
    }
    if ( ++g_cpu_to_wan_tx_desc_pos == CPU_TO_WAN_TX_DESC_NUM )
        g_cpu_to_wan_tx_desc_pos = 0;
    local_irq_restore(sys_flag);

    /*  load descriptor from memory */
    reg_desc = *desc;

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & (DMA_ALIGNMENT - 1)) == 0, "reg_desc.dataptr (0x%#x) must be 8 DWORDS aligned", reg_desc.dataptr);
    skb_to_free = get_skb_pointer(reg_desc.dataptr);
    atm_free_tx_skb_vcc_delayed(skb_to_free);
    put_skb_to_dbg_pool(skb);

    /*  setup descriptor    */
    reg_desc.dataptr    = (u32)skb->data & (0x1FFFFFFF ^ (DMA_ALIGNMENT - 1));  //  byte address
    reg_desc.byteoff    = (u32)skb->data & (DMA_ALIGNMENT - 1);
    reg_desc.datalen    = skb->len;
    reg_desc.mpoa_pt    = 1;
    reg_desc.mpoa_type  = 0;    //  this flag should be ignored by firmware
    reg_desc.pdu_type   = 1;    //  cell
    reg_desc.qid        = conn;
    reg_desc.own        = 0;
    reg_desc.c          = 0;
    reg_desc.sop = reg_desc.eop = 1;

    dump_skb(skb, DUMP_SKB_LEN, "ppe_send_cell", 7, 0, 1, 0);

    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    adsl_led_flash();

    return 0;
}

static int proc_read_send(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "echo \"[<qid> qid] [<pvc> vpi.vci] <dword0> <dword1> ...... <dword12>\" > /proc/eth/send\n");

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

#define RX_TOTAL_PCKNT                  0x1F
#define RX_UNICAST_PCKNT                0x23
#define RX_MULTICAST_PCKNT              0x22
#define RX_CRC_ERR_PCKNT                0x21
#define RX_UNDERSIZE_GOOD_PCKNT         0x1D
#define RX_OVER_SIZE_GOOD_PCKNT         0x1B
#define RX_UNDERSIZE_ERR_PCKNT          0x1E
#define RX_GOOD_PAUSE_PCKNT             0x20
#define RX_OVER_SIZE_ERR_PCKNT          0x1C
#define RX_ALLIGN_ERR_PCKNT             0x1A
#define RX_FILTERED_PCKNT               0x19
#define RX_DISCARD_CONGESTION_PCKNT     0x11

#define RX_SIZE_64B_PCKNT               0x12
#define RX_SIZE_65_127B_PCKNT           0x13
#define RX_SIZE_128_255B_PCKNT          0x14
#define RX_SIZE_256_511B_PCKNT          0x15
#define RX_SIZE_512_1023B_PCKNT         0x16
#define RX_SIZE_1024B_MAX_PCKNT         0x17

#define TX_TOTAL_PCKNT                  0x0C
#define TX_UNICAST_PCKNT                0x06
#define TX_MULTICAST_PCKNT              0x07

#define TX_SINGLE_COLLISION_CNT         0x08
#define TX_MULTIPLE_COLLISION_CNT       0x09
#define TX_LATE_COLLISION_CNT           0x0A
#define TX_EXCESSIVE_COLLISION_CNT      0x0B
#define TX_PAUSE_PCKNT                  0x0D

#define TX_SIZE_64B_PCKNT               0x00
#define TX_SIZE_65_127B_PCKNT           0x01
#define TX_SIZE_128_255B_PCKNT          0x02
#define TX_SIZE_256_511B_PCKNT          0x03
#define TX_SIZE_512_1023B_PCKNT         0x04
#define TX_SIZE_1024B_MAX_PCKNT         0x05


#define TX_DROP_PCKNT                   0x10
#define RX_DROP_PCKNT                   0x18

#define RX_GOOD_BYTE_CNT_LOW            0x24
#define RX_GOOD_BYTE_CNT_HIGH           0x25

#define RX_BAD_BYTE_CNT_LOW             0x26
#define RX_BAD_BYTE_CNT_HIGH            0x27

#define TX_GOOD_BYTE_CNT_LOW            0x0E
#define TX_GOOD_BYTE_CNT_HIGH           0x0F

static INLINE unsigned int sw_get_rmon_counter(int port, int addr)
{
    *ETHSW_BM_RAM_ADDR_REG = addr;
    *ETHSW_BM_RAM_CTRL_REG  = 0x8000 | port;
    while ( (*ETHSW_BM_RAM_CTRL_REG & 0x8000) );

    return (*ETHSW_BM_RAM_VAL_1_REG << 16) | (*ETHSW_BM_RAM_VAL_0_REG & 0xFFFF);
}

static INLINE void sw_clr_rmon_counter(int port)
{
    int i;

    if ( port >= 0 && port < 7 )
    {
        *ETHSW_BM_PCFG_REG(port) = 0;
        *ETHSW_BM_RMON_CTRL_REG(port) = 3;
        for ( i = 1000; (*ETHSW_BM_RMON_CTRL_REG(port) & 3) != 0 && i > 0; i-- );
        if ( i == 0 )
            *ETHSW_BM_RMON_CTRL_REG(port) = 0;
        *ETHSW_BM_PCFG_REG(port) = 1;
    }
}

static INLINE int read_port_counters(int port, char *buf)
{
    int len = 0;
    unsigned int counter;
    unsigned long long  byteCnt;

    len += sprintf(buf+len, "\n\tPort [%d] counters\n\n", port);

    len += sprintf(buf+len, "Rx Total PckCnt     :");
    counter = sw_get_rmon_counter(port, RX_TOTAL_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx Unicast PckCnt   :");
    counter = sw_get_rmon_counter(port, RX_UNICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx Multicast PckCnt :");
    counter = sw_get_rmon_counter(port, RX_MULTICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx Total PckCnt     :");
    counter = sw_get_rmon_counter(port, TX_TOTAL_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx Unicase PckCnt   :");
    counter = sw_get_rmon_counter(port, TX_UNICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx Multicase PckCnt :");
    counter = sw_get_rmon_counter(port, TX_MULTICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Rx 64B PckCnt       :");
    counter = sw_get_rmon_counter(port, RX_SIZE_64B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [65-127B] PckCnt :");
    counter = sw_get_rmon_counter(port, RX_SIZE_65_127B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [128~255B] PckCnt:");
    counter = sw_get_rmon_counter(port, RX_SIZE_128_255B_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);
    len += sprintf(buf+len, "Rx [256~511B] PckCnt:");
    counter = sw_get_rmon_counter(port, RX_SIZE_256_511B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [512~1023B]PckCnt:");
    counter = sw_get_rmon_counter(port, RX_SIZE_512_1023B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [ >1024B] PckCnt :");
    counter = sw_get_rmon_counter(port, RX_SIZE_1024B_MAX_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx [64B] PckCnt     :");
    counter = sw_get_rmon_counter(port, TX_SIZE_64B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [65~127B] PckCnt :");
    counter = sw_get_rmon_counter(port, TX_SIZE_65_127B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [128~255B] PckCnt:");
    counter = sw_get_rmon_counter(port, TX_SIZE_128_255B_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);
    len += sprintf(buf+len, "Tx [256~511B] PckCnt:");
    counter = sw_get_rmon_counter(port, TX_SIZE_256_511B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [512~1023B]PckCnt:");
    counter = sw_get_rmon_counter(port, TX_SIZE_512_1023B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [>1024B] PckCnt  :");
    counter = sw_get_rmon_counter(port, TX_SIZE_1024B_MAX_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Rx CRC err PckCnt   :");
    counter = sw_get_rmon_counter(port, RX_CRC_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Unsize err PCkCnt:");
    counter = sw_get_rmon_counter(port, RX_UNDERSIZE_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Ovsize err PckCnt:");
    counter = sw_get_rmon_counter(port, RX_OVER_SIZE_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\n",counter );

    len += sprintf(buf+len, "Rx UnsizeGood PckCnt:");
    counter = sw_get_rmon_counter(port, RX_UNDERSIZE_GOOD_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx OvsizeGood PckCnt:");
    counter = sw_get_rmon_counter(port, RX_OVER_SIZE_GOOD_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Good Pause PckCnt:");
    counter = sw_get_rmon_counter(port, RX_GOOD_PAUSE_PCKNT);
    len += sprintf(buf+len, "0x%08x\n",counter );

    len += sprintf(buf+len, "Rx Align err PckCnt :");
    counter = sw_get_rmon_counter(port, RX_ALLIGN_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx filterd errPckCnt:");
    counter = sw_get_rmon_counter(port, RX_FILTERED_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Tx Single col Cnt   :");
    counter = sw_get_rmon_counter(port, TX_SINGLE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx Multiple col Cnt :");
    counter = sw_get_rmon_counter(port, TX_MULTIPLE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Tx Late col  Cnt    :");
    counter = sw_get_rmon_counter(port,TX_LATE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Tx Excessive col Cnt:");
    counter = sw_get_rmon_counter(port, TX_EXCESSIVE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx  Pause Cnt       :");
    counter = sw_get_rmon_counter(port, TX_PAUSE_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);


    len += sprintf(buf+len, "Tx Drop PckCnt      :");
    counter =sw_get_rmon_counter(port, TX_DROP_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Drop PckCnt      :");
    counter = sw_get_rmon_counter(port, RX_DROP_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Rx Good Byte Cnt    :");
    byteCnt = sw_get_rmon_counter(port, RX_GOOD_BYTE_CNT_HIGH);
    byteCnt <<= 32;
    byteCnt += sw_get_rmon_counter(port, RX_GOOD_BYTE_CNT_LOW);
    len += sprintf(buf+len, "0x%llx \t", byteCnt);

    len += sprintf(buf+len, "Tx Good Byte Cnt    :");
    byteCnt = sw_get_rmon_counter(port, TX_GOOD_BYTE_CNT_HIGH);
    byteCnt <<= 32;
    byteCnt += sw_get_rmon_counter(port, TX_GOOD_BYTE_CNT_LOW);
    len += sprintf(buf+len, "0x%llx \n", byteCnt);

    len += sprintf(buf+len, "Rx Bad Byte Cnt     :");
    byteCnt = sw_get_rmon_counter(port, RX_BAD_BYTE_CNT_HIGH);
    byteCnt <<= 32;
    byteCnt += sw_get_rmon_counter(port, RX_BAD_BYTE_CNT_LOW);
    len += sprintf(buf+len, "0x%llx \t", byteCnt);

    len += sprintf(buf+len, "Rx Discard Cong Cnt :");
    byteCnt = sw_get_rmon_counter(port, RX_DISCARD_CONGESTION_PCKNT);
    len += sprintf(buf+len, "0x%llx \n", byteCnt);

    return len;
}

static int port0_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(0, buf + len);

    *eof = 1;

    return len;
}

static int port1_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(1, buf + len);

    *eof = 1;

    return len;
}

static int port2_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(2, buf + len);

    *eof = 1;

    return len;
}

static int port3_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(3, buf + len);

    *eof = 1;

    return len;
}

static int port4_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(4, buf + len);

    *eof = 1;

    return len;
}

static int port5_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(5, buf + len);

    *eof = 1;

    return len;
}

static int port6_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(6, buf + len);

    *eof = 1;

    return len;
}

static int port_counters_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    int port;

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

    if ( stricmp(p, "clean") == 0 || stricmp(p, "clear") == 0 )
    {
        strcpy(str, file->f_dentry->d_iname);
        port = str[4] - '0';
        str[4] = ' ';
        if ( stricmp(str, "port mib") == 0 && port >= 0 && port < 7 )
            sw_clr_rmon_counter(port);
        else
            printk("unknown entry - %s\n", file->f_dentry->d_iname);
    }
    else if ( stricmp(p, "clean all") == 0 || stricmp(p, "clear all") == 0 )
    {
        for ( port = 0; port < 7; port++ )
            sw_clr_rmon_counter(port);
    }

    return count;
}

static int proc_read_directforwarding(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "ingress direct forwarding - %s\n", g_ingress_direct_forward_enable ? "enable" : "disable");
    len += sprintf(page + off + len, "egress direct forwarding  - %s\n", g_egress_direct_forward_enable ? "enable" : "disable");

    *eof = 1;

    return len;
}

static int proc_write_directforwarding(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

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

    if ( stricmp(p, "ingress enable") == 0 )
    {
        if ( !g_ingress_direct_forward_enable )
        {
            g_ingress_direct_forward_enable = 1;
//            *AMAZON_S_SW_P0_CTL |= (1 << 19);  //  enable port ingress direct forwarding
//            *AMAZON_S_SW_P1_CTL |= (1 << 19);  //  enable port ingress direct forwarding
            //egress is enabled in eth_xmit
        }
    }
    else if ( stricmp(p, "egress enable") == 0 )
    {
        if ( !g_egress_direct_forward_enable )
        {
            g_egress_direct_forward_enable = 1;
            //egress is enabled in eth_xmit
        }
    }
    else if ( stricmp(p, "ingress disable") == 0 )
    {
        if ( g_ingress_direct_forward_enable )
        {
            g_ingress_direct_forward_enable = 0;
//            *AMAZON_S_SW_P0_CTL &= ~(1 << 19);  //  disable port ingress direct forwarding
//            *AMAZON_S_SW_P1_CTL &= ~(1 << 19);  //  disable port ingress direct forwarding
        }
    }
    else if ( stricmp(p, "egress disable") == 0 )
    {
        if ( g_egress_direct_forward_enable )
        {
            g_egress_direct_forward_enable = 0;
            //egress is enabled in eth_xmit
        }
    }
    else
    {
        printk("echo <ingress/egress> <enable/disable> > /proc/eth/direct_forwarding\n");
        printk("enable  : pure routing configuration, switch forward packets between port0/1 and cpu port directly w/o learning\n");
        printk("disable : bridging/mix configuration, switch learn MAC address and make decision on which port packet forward to\n");
    }

    return count;
}

static int proc_read_clk(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, (*AMAZON_S_CGU_SYS & (1 << 7)) ? "PPE clock - 250M\n" : "PPE clock - 300M\n");

    *eof = 1;

    return len;
}

static int proc_write_clk(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

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

    if ( stricmp(p, "250M") == 0 || stricmp(p, "250") == 0 )
    {
        *AMAZON_S_CGU_SYS |= 1 << 7;
        *AMAZON_S_CGU_UPDATE = 1;
    }
    else if ( stricmp(p, "300M") == 0 || stricmp(p, "300") == 0 )
    {
        *AMAZON_S_CGU_SYS &= ~(1 << 7);
        *AMAZON_S_CGU_UPDATE = 1;
    }
    else
    {
        printk("echo <250M/300M> > /proc/eth/pure_routing\n");
        printk("  250M - PPE clock 250MHz\n");
        printk("  300M - PPE clock 300MHz\n");
    }

    return count;
}

static int proc_read_flowcontrol(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
#if 0
    u16 value;

    value = smi_reg_read(0x00f5);
    if ( value == 0x0BBB )
        len += sprintf(page + off + len, "external tantos flow control: enable\n");
    else if ( value == 0x0AAA )
        len += sprintf(page + off + len, "external tantos flow control: disable\n");
    else
        len += sprintf(page + off + len, "external tantos flow control: value = 0x%04x\n", (u32)value);

    len += sprintf(page + off + len, (*AMAZON_S_SW_RGMII_CTL & 1) ? "MII0 flow control: enable\n" : "MII0 flow control: disable\n");
    len += sprintf(page + off + len, (*AMAZON_S_SW_RGMII_CTL & (1 << 10)) ? "MII1 flow control: enable\n" : "MII0 flow control: disable\n");
#endif

    *eof = 1;

    return len;
}

static int proc_write_flowcontrol(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 0;
    int f_enable = 0;
    int flag = 0;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "enable") == 0 )
            f_enable = 1;
        else if ( stricmp(p1, "disable") == 0 )
            f_enable = -1;
        else if ( stricmp(p1, "help") == 0 || strcmp(p1, "?") == 0 )
        {
            f_enable = 0;
            flag = 0;
            break;
        }
        else if ( f_enable )
        {
            if ( stricmp(p1, "sw") == 0 || stricmp(p1, "tantos") == 0 || stricmp(p1, "external") == 0 || stricmp(p1, "ext") == 0 )
                flag = (flag & ~3) | (f_enable > 0 ? 3 : 2);
            else if ( stricmp(p1, "eth0") == 0 || stricmp(p1, "mii0") == 0 )
                flag = (flag & ~(3 << 2)) | ((f_enable > 0 ? 3 : 2) << 2);
            else if ( stricmp(p1, "eth1") == 0 || stricmp(p1, "mii1") == 0 )
                flag = (flag & ~(3 << 4)) | ((f_enable > 0 ? 3 : 2) << 4);
        }

        p1 = p2;
    }

    if ( !flag && f_enable )
        flag = f_enable > 0 ? 0x3F : 0x2A;

#if 0
    if ( (flag & 3) )
    {
        if ( (flag & 1) )
            smi_reg_write(0x00f5, 0x0bbb);
        else
            smi_reg_write(0x00f5, 0x0aaa);
    }
    if ( (flag & (3 << 2)) )
    {
        if ( (flag & (1 << 2)) )
            *AMAZON_S_SW_RGMII_CTL |= 1;
        else
            *AMAZON_S_SW_RGMII_CTL &= ~1;
    }
    if ( (flag & (3 << 4)) )
    {
        if ( (flag & (1 << 4)) )
            *AMAZON_S_SW_RGMII_CTL |= 1 << 10;
        else
            *AMAZON_S_SW_RGMII_CTL &= ~(1 << 10);
    }
#endif

    if ( !flag )
        printk("echo <enable/disable> [sw/mii0/mii1] > /proc/eth/flowcontrol\n");

    return count;
}

static int proc_read_prio(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i, j;

    len += sprintf(page + off + len,        "Priority to Queue Map:\n");
    len += sprintf(page + off + len,        "  prio\t\t:  0  1  2  3  4  5  6  7\n");
    for ( i = 0; i < 2; i++ )
    {
        len += sprintf(page + off + len,    "  eth%d\t\t:", i);
        for ( j = 0; j < NUM_ENTITY(g_eth_prio_queue_map[i]); j++ )
            len += sprintf(page + off + len,"  %d", g_eth_prio_queue_map[i][j]);
        len += sprintf(page + off + len,    "\n");
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

            len += sprintf(page + off + len,"  pvc %d.%d \t:", (int)g_atm_priv_data.connection[i].vcc->vpi, (int)g_atm_priv_data.connection[i].vcc->vci);
            for ( j = 0; j < 8; j++ )
                len += sprintf(page + off + len," %2d", sw_tx_queue_to_virt_queue_map[g_atm_priv_data.connection[i].prio_queue_map[j]]);
            len += sprintf(page + off + len,    "\n");
            len += sprintf(page + off + len,"      phys qid\t:");
            for ( j = 0; j < 8; j++ )
                len += sprintf(page + off + len," %2d", g_atm_priv_data.connection[i].prio_queue_map[j]);
            len += sprintf(page + off + len,    "\n");
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
            printk("echo <eth0/eth1/pvc vpi.vci> prio xx queue xx [prio xx queue xx] > /proc/eth/prio\n");
            printk("echo pvc vpi.vci <add/del> > /proc/eth/prio\n");
            break;
        }
        else if ( stricmp(p1, "eth0") == 0 )
        {
            port = 0;
            prio = queue = -1;
            printk("port = 0\n");
        }
        else if ( stricmp(p1, "eth1") == 0 )
        {
            port = 1;
            prio = queue = -1;
            printk("port = 1\n");
        }
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
                    if ( queue >= 0 && queue < ((g_wan_itf & (1 << port)) && g_wanqos_en ? g_wanqos_en : 4) )
                    {
                        if ( prio >= 0 )
                            g_eth_prio_queue_map[port][prio] = queue;
                    }
                    else
                        printk("queue (%d) is out of range 0 - %d\n", queue, (g_wan_itf & (1 << port)) && g_wanqos_en ? g_wanqos_en - 1 : 3);
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

static int proc_read_ewan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i, j;
    unsigned int bit;

    g_pmac_ewan = *PMAC_EWAN_REG;

    len += sprintf(page + off + len, "EWAN: ");
    for ( i = j = 0, bit = 1; i < 7; i++, bit <<= 1 )
        if ( (g_pmac_ewan & bit) ) {
            len += sprintf(page + off + len, (const char *)(j ? " | P%d" : "P%d"), i);
            j++;
        }
    len += sprintf(page + off + len, (const char *)(j ? "\n" : "null\n"));

    *eof = 1;

    return len;
}

static int proc_write_ewan(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 1;

    unsigned int got_ports = 0;
    unsigned int ports = 0;
    char *ports_name[] = {"p0", "p1", "p2", "p3", "p4", "p5"};
    int i;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "help") == 0 )
        {
            printk("echo <p0|p1|p2|p3|p4|p5> > /proc/eth/ewan\n");
            break;
        }
        else
        {
            for ( i = 0; i < NUM_ENTITY(ports_name); i++ )
                if ( stricmp(p1, ports_name[i]) == 0 )
                {
                    ports |= 1 << i;
                    got_ports++;
                    break;
                }
        }

        p1 = p2;
        colon = 1;
    }

    if ( got_ports )
        *PMAC_EWAN_REG = g_pmac_ewan = ports;

    return count;
}
#ifdef CONFIG_IFX_PPA_QOS
static int proc_read_qos(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[200];
    int llen;
    int i;
    struct eth_wan_mib_table qos_queue_mib;
    volatile struct tx_qos_cfg tx_qos_cfg = *TX_QOS_CFG;
    volatile struct wtx_qos_q_desc_cfg qos_q_desc_cfg;

    pstr = *start = page;

    __sync();

    if( GEN_MODE_CFG->sys_cfg != 3) return 0; //QOS Note: Different condition with different mode/platfrom

    llen = sprintf(pstr, "\n  qos         : %s\n  wfq         : %s\n  Rate shaping: %s\n\n",
                    tx_qos_cfg.eth1_qss ?"enabled":"disabled",
                    tx_qos_cfg.wfq_en?"enabled":"disabled",
                    tx_qos_cfg.shape_en ?"enabled":"disabled");
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  Ticks  =%u,    overhd    =%u,       qnum=%u  @%p\n", (u32)tx_qos_cfg.time_tick, (u32)tx_qos_cfg.overhd_bytes, (u32)tx_qos_cfg.eth1_eg_qnum, TX_QOS_CFG );
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  PPE clk=%u MHz, basic tick=%u\n",(u32)cgu_get_pp32_clock() / 1000000, TX_QOS_CFG->time_tick / (cgu_get_pp32_clock() / 1000000));
    pstr += llen;
    len += llen;

    if ( tx_qos_cfg.eth1_eg_qnum )
    {
        llen = sprintf(pstr, "\n  MIB : rx_pkt/rx_bytes         tx_pkt/tx_bytes       cpu_small_drop/cpu_drop  fast_small_drop/fast_drop_cnt\n");
        pstr += llen;
        len += llen;
        for ( i = 0; i < 8; i++ )
        {
            qos_queue_mib = *ETH_WAN_TX_MIB_TABLE(i);

            llen = sprintf(str, "    %2u: %010u/%010u  %010u/%010u  %010u/%010u  %010u/%010u  @0x%p\n", i,
                qos_queue_mib.wrx_total_pdu, qos_queue_mib.wrx_total_bytes,
                qos_queue_mib.wtx_total_pdu, qos_queue_mib.wtx_total_bytes,
                qos_queue_mib.wtx_cpu_drop_small_pdu, qos_queue_mib.wtx_cpu_drop_pdu,
                qos_queue_mib.wtx_fast_drop_small_pdu, qos_queue_mib.wtx_fast_drop_pdu,
                ETH_WAN_TX_MIB_TABLE(i));

            if ( len <= off && len + llen > off )
            {
                ppa_memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                ppa_memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_MAC_OVERRUN_END;
        }


        //QOS queue descriptor
        llen = sprintf(pstr, "\n  Desc: threshold  num    base_addr  rd_ptr   wr_ptr\n");
        pstr += llen;
        len += llen;
        for ( i = 0; i < 8; i++ )
        {
            qos_q_desc_cfg = *WTX_QOS_Q_DESC_CFG(i);

            llen = sprintf(pstr, "    %2u: 0x%02x       0x%02x   0x%04x     0x%04x   0x%04x  @0x%p\n", i,
                qos_q_desc_cfg.threshold,
                qos_q_desc_cfg.length,
                qos_q_desc_cfg.addr,
                qos_q_desc_cfg.rd_ptr,
                qos_q_desc_cfg.wr_ptr,
                WTX_QOS_Q_DESC_CFG(i) );

            pstr += llen;
            len += llen;
        }
    }

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    return len - llen - off;
}
#endif

#if defined(ENABLE_CONFIGURABLE_DSL_VLAN) && ENABLE_CONFIGURABLE_DSL_VLAN
static int proc_read_dsl_vlan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i;

    len += sprintf(page + off + len,     "PVC to DSL VLAN map:\n");
    for ( i = 0; i < ATM_QUEUE_NUMBER; i++ )
    {
        if ( HTU_ENTRY(i + OAM_HTU_ENTRY_NUMBER)->vld == 0 )
        {
            len += sprintf(page + off + len, "  %2d: invalid\n", i);
            continue;
        }

        len += sprintf(page + off + len, "  %2d: PVC %u.%u\tVLAN 0x%04X\n", i, (unsigned int)g_atm_priv_data.connection[i].vcc->vpi, (unsigned int)g_atm_priv_data.connection[i].vcc->vci, (unsigned int)WRX_QUEUE_CONFIG(i)->new_vlan);
    }
    len += sprintf(page + off + len,     "DSL VLAN to PVC map:\n");
    for ( i = 0; i < NUM_ENTITY(g_dsl_vlan_qid_map); i += 2 )
    {
        if ( g_dsl_vlan_qid_map[i] == 0 )
        {
            len += sprintf(page + off + len, "  %2d: invalid\n", i);
            continue;
        }
        len += sprintf(page + off + len, "  %2d: VLAN 0x%04X\tQID %d\n", i >> 1, (unsigned int)g_dsl_vlan_qid_map[i], (unsigned int)g_dsl_vlan_qid_map[i + 1]);
    }

    *eof = 1;

    return len;
}

static int proc_write_dsl_vlan(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 1;
    int conn = -1;
    unsigned int vpi, vci;
    unsigned int vlan_tag = ~0;
    int eth0 = 0, eth1 = 0;
    int bypass = 0, enable = 0;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "help") == 0 )
        {
            printk("echo <pvc vpi.vci> <vlan xx> [bypass [enable|disbable]] > /proc/eth/dsl_vlan\n");
            printk("  vlan   - 16-bit vlan tag, including PCP, CFI, and VID.\n");
            printk("  bypass - traffic from ATM to MII0 is done by internal switch, no CPU involvement.\n");
            conn = -1;
            break;
        }
        else if ( bypass == 1 )
        {
            if ( stricmp(p1, "enable") == 0 )
                enable = 1;
            else if ( stricmp(p1, "disable") == 0 )
                enable = -1;
            else if ( stricmp(p1, "eth0") == 0 )
            {
                if ( eth0 == 0 )
                    eth0 = enable == 0 ? 1 : enable;
            }
            else if ( stricmp(p1, "eth1") == 0 )
            {
                if ( eth1 == 0 )
                    eth1 = enable == 0 ? 1 : enable;
            }
            else
            {
                printk("unknown command (%s)\n", p1);
                conn = -1;
                break;
            }
        }
        else
        {
            if ( bypass == 1 && eth0 == 0 )
                eth0 = enable >= 0 ? 1 : -1;

            bypass = 0;
            enable = 0;

            if ( stricmp(p1, "pvc") == 0 )
            {
                ignore_space(&p2, &len);
                vpi = get_number(&p2, &len, 0);
                ignore_space(&p2, &len);
                vci = get_number(&p2, &len, 0);
                conn = find_vpivci(vpi, vci);
                printk("vpi = %u, vci = %u, conn = %d\n", vpi, vci, conn);
            }
            else if ( vlan_tag == ~0 && stricmp(p1, "vlan") == 0 )
            {
                ignore_space(&p2, &len);
                vlan_tag = get_number(&p2, &len, 0) & 0xFFFF;
                printk("VLAN tag = 0x%04X\n", vlan_tag);
            }
            else if ( stricmp(p1, "bypass") == 0 )
                bypass = 1;
            else
            {
                printk("unknown command (%s)\n", p1);
                conn = -1;
                break;
            }
        }

        p1 = p2;
        colon = 1;
    }
    if ( bypass == 1 && eth0 == 0 )
        eth0 = enable >= 0 ? 1 : -1;

    if ( conn >= 0 && vlan_tag != ~0 )
    {
        if ( ppe_set_vlan(conn, (unsigned short)vlan_tag) != 0 )
            conn = -1;
    }
    if ( conn >= 0 )
    {
        vlan_tag = (unsigned int)WRX_QUEUE_CONFIG(conn)->new_vlan;
        if ( eth0 != 0 )
            sw_set_vlan_bypass(0, vlan_tag, eth0 < 0 ? 1 : 0);
        if ( eth1 != 0 )
            sw_set_vlan_bypass(1, vlan_tag, eth0 < 0 ? 1 : 0);
    }

    return count;
}

static INLINE int ppe_set_vlan(int conn, unsigned short vlan_tag)
{
    int candidate = -1;
    int i;

    for ( i = 0; i < NUM_ENTITY(g_dsl_vlan_qid_map); i += 2 )
    {
        if ( candidate < 0 && g_dsl_vlan_qid_map[i] == 0 )
            candidate = i;
        else if ( g_dsl_vlan_qid_map[i] == (unsigned short)vlan_tag )
        {
            if ( conn != g_dsl_vlan_qid_map[i + 1] )
            {
                err("VLAN tag 0x%04X is in use", vlan_tag);
                return -1;  //  quit (error), VLAN tag is used by other PVC
            }
            return 0;       //  quit (success), VLAN tag is assigned already
        }
    }
    //  candidate must be valid, because size of g_dsl_vlan_qid_map is equal to max number of PVCs
    g_dsl_vlan_qid_map[candidate + 1] = conn;
    g_dsl_vlan_qid_map[candidate] = vlan_tag;
    WRX_QUEUE_CONFIG(conn)->new_vlan = vlan_tag;

    return 0;
}

static INLINE int sw_set_vlan_bypass(int itf, unsigned int vlan_tag, int is_del)
{
    if ( itf == 1 )
    {
        err("does not support eth1 VLAN bypass!");
        return -1;
    }
#if 0 // TODO: VR9 internal switch config
    if ( is_del == 0 )
    {
        *AMAZON_S_SW_DF_PORTMAP |= 0x01010101;  //  *AMAZON_S_SW_DF_PORTMAP = 0x05050505;
        *AMAZON_S_SW_P2_VLAN    = (*AMAZON_S_SW_P2_VLAN & ~0x00010000) | 0x20000000;    //  *AMAZON_S_SW_P2_VLAN    = 0x24FE0001;
        *AMAZON_S_SW_VLAN_FLT0  = 0x01088000 | (vlan_tag & 0x0FFF);
        *AMAZON_S_SW_P2_CTL     &= ~0x0C;       //  *AMAZON_S_SW_P2_CTL     = 0x0E040003;
        //*AMAZON_S_SW_P0_CTL     = 0x0E44000C;
        //*AMAZON_S_SW_P0_VLAN    = 0x14FF0001;
    }
    else
    {
        *AMAZON_S_SW_P2_CTL     |= 0x0C;        //  *AMAZON_S_SW_P2_CTL     = 0x0E04000F;
        *AMAZON_S_SW_VLAN_FLT0  = 0x00000000;
        *AMAZON_S_SW_P2_VLAN    = (*AMAZON_S_SW_P2_VLAN | 0x00010000) & ~0x20000000;    //  *AMAZON_S_SW_P2_VLAN    = 0x04FF0001;
        *AMAZON_S_SW_DF_PORTMAP &= ~0x01010101; //  //  *AMAZON_S_SW_DF_PORTMAP = 0x04040404;
    }
#endif

    return 0;
}
#endif

#if defined(DEBUG_MIRROR_PROC) && DEBUG_MIRROR_PROC

static int proc_read_mirror(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char devname[IFNAMSIZ + 1] = {0};

    if ( g_mirror_netdev == NULL )
        strcpy(devname, "NONE");
    else
        strncpy(devname, g_mirror_netdev->name, IFNAMSIZ);

    len += sprintf(page + off + len, "mirror: %s\n", devname);

    *eof = 1;

    return len;
}

static int proc_write_mirror(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return count;

    if ( g_mirror_netdev != NULL )
    {
        dev_put(g_mirror_netdev);
        g_mirror_netdev = NULL;
    }
    if ( stricmp(p, "none") == 0 )
        printk("disable mirror\n");
    else
    {
  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        g_mirror_netdev = dev_get_by_name(p);
  #else
        g_mirror_netdev = dev_get_by_name(&init_net, p);
  #endif
        if ( g_mirror_netdev != NULL )
            printk("mirror: %s\n", p);
        else
            printk("mirror: none, can't find device \"%s\"\n", p);
    }

    return count;
}

#endif

#if 0
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
    unsigned int tmp_val[17] = {0};
    int is_ipv6 = 0;

    memset(val, 0, sizeof(*val) * 6);

    for ( i = 0; i < 4; i++ )
    {
        ignore_space(p, len);
        if ( !*len )
            break;
        val[i] = get_number(p, len, 0);
    }

    if ( **p == '.' )
    {
        is_ipv6 = 1;
        for ( i = 0; i < 16 - 4; i++ )
        {
            ignore_space(p, len);
            if ( !*len )
                break;
            tmp_val[i] = get_number(p, len, 0);
        }
    }

    ignore_space(p, len);
    if ( *len )
        val[4] = get_number(p, len, 0);

    if ( is_ipv6 )
    {
        val[5] = 6;
        for ( i = 0; i < 16 - 4; i++ )
            val[i + 6] = tmp_val[i];
    }
    else
        val[5] = 4;
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
static INLINE int print_route(char *buf, int i, int f_is_lan, struct rout_forward_compare_tbl *pcompare, struct rout_forward_action_tbl *pwaction)
{
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "res", "ATM"};
    static const char *mpoa_type[] = {"EoA without FCS", "EoA with FCS", "PPPoA", "IPoA"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d\n", i);
    len += sprintf(buf + len,          "    compare (0x%08X)\n", (u32)pcompare);
    len += sprintf(buf + len,          "      src:  %d.%d.%d.%d:%d\n", pcompare->src_ip >> 24,  (pcompare->src_ip >> 16) & 0xFF,  (pcompare->src_ip >> 8) & 0xFF,  pcompare->src_ip & 0xFF, pcompare->src_port);
    len += sprintf(buf + len,          "      dest: %d.%d.%d.%d:%d\n", pcompare->dest_ip >> 24, (pcompare->dest_ip >> 16) & 0xFF, (pcompare->dest_ip >> 8) & 0xFF, pcompare->dest_ip & 0xFF, pcompare->dest_port);
    len += sprintf(buf + len,          "    action  (0x%08X)\n", (u32)pwaction);
    len += sprintf(buf + len,          "      new %s    %d.%d.%d.%d:%d\n", f_is_lan ? "src :" : "dest:", pwaction->new_ip >> 24, (pwaction->new_ip >> 16) & 0xFF, (pwaction->new_ip >> 8) & 0xFF, pwaction->new_ip & 0xFF, pwaction->new_port);
    len += sprintf(buf + len,          "      new MAC :    %02X:%02X:%02X:%02X:%02X:%02X\n", (pwaction->new_dest_mac54 >> 8) & 0xFF, pwaction->new_dest_mac54 & 0xFF, pwaction->new_dest_mac30 >> 24, (pwaction->new_dest_mac30 >> 16) & 0xFF, (pwaction->new_dest_mac30 >> 8) & 0xFF, pwaction->new_dest_mac30 & 0xFF);
    switch ( pwaction->rout_type )
    {
    case 1:  len += sprintf(buf + len, "      route type:  IP\n");   break;
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
    {
        len += sprintf(buf + len,      "      PPPoE mode:  termination\n");
        if ( f_is_lan )
            len += sprintf(buf + len,  "      PPPoE index: %d\n", pwaction->pppoe_ix);
    }
    else
        len += sprintf(buf + len,      "      PPPoE mode:  transparent\n");
    len += sprintf(buf + len,          "      new src MAC index: %d\n", pwaction->new_src_mac_ix);
    if ( pwaction->out_vlan_ins )
        len += sprintf(buf + len,      "      outer VLAN insert: enable, index %d\n", pwaction->out_vlan_ix);
    else
        len += sprintf(buf + len,      "      outer VLAN insert: disable\n");
    if ( f_is_lan || GEN_MODE_CFG1->sys_cfg != 0 /* not DSL WAN mode */ )
        len += sprintf(buf + len,      "      outer VLAN remove: %s\n", pwaction->out_vlan_rm ? "enable" : "disable");
    else
        len += sprintf(buf + len,      "      outer VLAN remove: %s (forced to be enable for ATM PVC differentiation)\n", pwaction->out_vlan_rm ? "enable" : "disable");
    len += sprintf(buf + len,          "      tcp:         %s\n", pwaction->protocol ? "yes" : "no (UDP)");
    len += sprintf(buf + len,          "      entry valid: %s\n", pwaction->entry_vld ? "yes" : "no");
    if ( f_is_lan )
    {
        len += sprintf(buf + len,      "      mpoa type:   %s\n", mpoa_type[pwaction->mpoa_type]);
        len += sprintf(buf + len,      "      dslwan qid:  %d\n", pwaction->dest_qid);
    }
    else
        len += sprintf(buf + len,      "      dslwan qid:  %d (conn %d)\n", pwaction->dest_qid & 0xFF, (pwaction->dest_qid >> 8) & 0xFF);

    return len;
}

#endif

#endif // if 0

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
static INLINE void dump_skb(struct sk_buff *skb, u32 len, char *title, int port, int ch, int is_tx, int is_force)
{
    int i;

    if ( !is_force && !(g_dbg_enable & (is_tx ? DBG_ENABLE_MASK_DUMP_SKB_TX : DBG_ENABLE_MASK_DUMP_SKB_RX)) )
        return;

    if ( skb->len < len )
        len = skb->len;

    if ( len > DMA_PACKET_SIZE )
    {
        printk("too big data length: skb = %08x, skb->data = %08x, skb->len = %d\n", (u32)skb, (u32)skb->data, skb->len);
        return;
    }

    if ( ch >= 0 )
        printk("%s (port %d, ch %d)\n", title, port, ch);
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
    case FWCODE_ROUTING_ACC_A5:
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
            printk("  ipv4_mc_pkt    = %Xh (%s)\n", (u32)header->ipv4_mc_pkt, header->ipv4_mc_pkt ? "multicast" : "uni-cast");
            printk("  proc_type      = %Xh (%s)\n", (u32)header->proc_type, header->proc_type ? "bridging" : "routing");
            printk("  tcpudp_err     = %Xh\n", (u32)header->tcpudp_err);
            printk("  tcpudp_chk     = %Xh\n", (u32)header->tcpudp_chk);
            printk("  is_udp         = %Xh\n", (u32)header->is_udp);
            printk("  is_tcp         = %Xh\n", (u32)header->is_tcp);
            printk("  ip_offset      = %Xh\n", (u32)header->ip_offset);
            printk("  is_pppoes      = %Xh\n", (u32)header->is_pppoes);
            printk("  is_ipv6        = %Xh\n", (u32)header->is_ipv6);
            printk("  is_ipv4        = %Xh\n", (u32)header->is_ipv4);
            printk("  is_vlan        = %Xh (%s)\n", (u32)header->is_vlan, is_vlan_str[header->is_vlan]);
            printk("  rout_index     = %Xh\n", (u32)header->rout_index);

            printk("  dest_list      = %Xh\n", (u32)header->dest_list);
            printk("  src_itf        = %Xh\n", (u32)header->src_itf);
            printk("  tcp_rstfin     = %Xh\n", (u32)header->tcp_rstfin);
            printk("  dslwan_qid     = %Xh\n", (u32)header->dslwan_qid);
            printk("  temp_dest_list = %Xh\n", (u32)header->temp_dest_list);
            printk("  src_dir        = %Xh (from %s side)\n", (u32)header->src_dir, header->src_dir ? "WAN" : "LAN");
            printk("  acc_done       = %Xh\n", (u32)header->acc_done);
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
    printk("  SB_MST_SEL(%08X) = 0x%08X\n", (u32)SB_MST_SEL, *SB_MST_SEL);

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
    struct sk_buff *skb;
    int len;
    int i;

    for ( i = 0; !g_directpath_dma_full && i < NUM_ENTITY(g_ppe_directpath_data); i++ )
    {
        while ( !g_directpath_dma_full && g_ppe_directpath_data[i].skb_list )
        {
            local_irq_save(sys_flag);

            skb = skb_get(g_ppe_directpath_data[i].skb_list);

            len = skb->len;
            if ( len < ETH_MIN_TX_PACKET_LENGTH + sizeof(struct sw_eg_pkt_header) )
                len = ETH_MIN_TX_PACKET_LENGTH + sizeof(struct sw_eg_pkt_header);

            if ( dma_device_write(g_dma_device, skb->data, len, skb) == len )
            {
                //  remove skb from list
                if ( skb->prev == skb )
                    skb->prev = skb->next = g_ppe_directpath_data[i].skb_list = NULL;
                else
                {
                    g_ppe_directpath_data[i].skb_list = skb->next;
                    g_ppe_directpath_data[i].skb_list->prev = skb->prev;
                    skb->prev->next = skb->next;
                    skb->prev = skb->next = NULL;
                }
                g_ppe_directpath_data[i].skb_list_len--;
                g_ppe_directpath_data[i].skb_list_size -= skb->len;
            }
            else
            {
                local_irq_restore(sys_flag);
                dev_kfree_skb_any(skb);
                return -1;
            }

            local_irq_restore(sys_flag);

            dev_kfree_skb_any(skb);

            if ( g_ppe_directpath_data[i].callback.start_tx_fn
  #if defined(ENABLE_DIRECTPATH_TX_QUEUE_SIZE)
                 && g_ppe_directpath_data[i].skb_list_size <= (ENABLE_DIRECTPATH_TX_QUEUE_SIZE * 3 / 4)
  #else
                 && g_ppe_directpath_data[i].skb_list_len <= (ENABLE_DIRECTPATH_TX_QUEUE_PKTS * 3 / 4)
  #endif
               )
                g_ppe_directpath_data[i].callback.start_tx_fn(g_ppe_directpath_data[i].netif);
        }
    }

    return 0;
}
#endif

static INLINE void register_netdev_event_handler(void)
{
    g_netdev_event_handler_nb.notifier_call = netdev_event_handler;
    register_netdevice_notifier(&g_netdev_event_handler_nb);
}

static INLINE void unregister_netdev_event_handler(void)
{
    unregister_netdevice_notifier(&g_netdev_event_handler_nb);
}

#ifdef CONFIG_IFX_ETHSW_API
static int netdev_event_handler(struct notifier_block *nb, unsigned long event, void *netdev)
{
    struct net_device *netif = (struct net_device *)netdev;
    IFX_ETHSW_HANDLE handle;
    union ifx_sw_param x;

#if 0
    if(event == NETDEV_UP){
        printk("NETDEV_UP\n");
    }else if(event == NETDEV_DOWN){
        printk("NETDEV_DOWN\n");
    }
    return NOTIFY_DONE;
#endif

    if ( netif->type != ARPHRD_ETHER )
        return NOTIFY_DONE;

    if ( (netif->flags & IFF_POINTOPOINT) ) //  ppp interface
        return NOTIFY_DONE;

    switch ( event )
    {
    case NETDEV_UP:
        handle = ifx_ethsw_kopen("/dev/switch_api/0");
        memset(&x.MAC_tableAdd, 0x00, sizeof(x.MAC_tableAdd));
        x.MAC_tableAdd.nFId = 0;
        x.MAC_tableAdd.nPortId = 6; //  CPU port
        //if ( netif->name[0] == 'p' && netif->name[1] == 't' && netif->name[2] == 'm' )  //  ptm
        //    x.MAC_tableAdd.nPortId = 11;
        //else if ( netif->name[0] == 'n' && netif->name[1] == 'a' && netif->name[2] == 's' ) //  atm
        //    x.MAC_tableAdd.nPortId = 11;
        //else if ( netif->name[0] == 'e' && netif->name[1] == 't' && netif->name[2] == 'h' && netif->name[3] == '1' )    //  eth1
        //    x.MAC_tableAdd.nPortId = ;
        //else
        //{
        //    int i;
        //
        //    for ( i = 0; i < NUM_ENTITY(g_ppe_directpath_data); i++ )
        //    {
        //        if ( netif == g_ppe_directpath_data[i].netif )
        //            x.MAC_tableAdd.nPortId = 7 + i;
        //    }
        //}
        x.MAC_tableAdd.nAgeTimer = 3;
        x.MAC_tableAdd.bStaticEntry = 1;
        x.MAC_tableAdd.nMAC[0] = netif->dev_addr[0];
        x.MAC_tableAdd.nMAC[1] = netif->dev_addr[1];
        x.MAC_tableAdd.nMAC[2] = netif->dev_addr[2];
        x.MAC_tableAdd.nMAC[3] = netif->dev_addr[3];
        x.MAC_tableAdd.nMAC[4] = netif->dev_addr[4];
        x.MAC_tableAdd.nMAC[5] = netif->dev_addr[5];
        ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_ADD, (unsigned int)&x.MAC_tableAdd);
        ifx_ethsw_kclose(handle);
        return NOTIFY_OK;
    case NETDEV_DOWN:
        handle = ifx_ethsw_kopen("/dev/switch_api/0");
        memset(&x.MAC_tableRemove, 0x00, sizeof(x.MAC_tableRemove));
        x.MAC_tableRemove.nFId = 0;
        x.MAC_tableRemove.nMAC[0] = netif->dev_addr[0];
        x.MAC_tableRemove.nMAC[1] = netif->dev_addr[1];
        x.MAC_tableRemove.nMAC[2] = netif->dev_addr[2];
        x.MAC_tableRemove.nMAC[3] = netif->dev_addr[3];
        x.MAC_tableRemove.nMAC[4] = netif->dev_addr[4];
        x.MAC_tableRemove.nMAC[5] = netif->dev_addr[5];
        ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE, (unsigned int)&x.MAC_tableRemove);
        ifx_ethsw_kclose(handle);
        return NOTIFY_OK;
    }

    return NOTIFY_DONE;
}
#else
static int netdev_event_handler(struct notifier_block *nb, unsigned long event, void *netdev)
{
    return NOTIFY_DONE;
}
#endif



/*
 * ####################################
 *           Global Function
 * ####################################
 */

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

    dbg("enter showtime, cell rate: 0 - %d, 1 - %d, xdata addr: 0x%08x", g_atm_priv_data.port[0].tx_max_cell_rate, g_atm_priv_data.port[1].tx_max_cell_rate, (unsigned int)g_xdata_addr);

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
#ifdef CONFIG_IFX_ETHSW_API
static int mac_entry_setting(unsigned char *mac, uint32_t fid, uint32_t portid, uint32_t agetime, uint32_t st_entry , uint32_t action)
{
	IFX_ETHSW_HANDLE handle;
	union ifx_sw_param x;
	int ret = IFX_SUCCESS;

	if(!mac)
		return IFX_ERROR;

	switch(action)
	{
		case IFX_ETHSW_MAC_TABLE_ENTRY_ADD:
			handle = ifx_ethsw_kopen("/dev/switch/0");
        	memset(&x.MAC_tableAdd, 0x00, sizeof(x.MAC_tableAdd));
			x.MAC_tableAdd.nFId = fid;
			x.MAC_tableAdd.nPortId = portid;
			x.MAC_tableAdd.nAgeTimer = agetime;
			x.MAC_tableAdd.bStaticEntry = st_entry;
			x.MAC_tableAdd.nMAC[0] = mac[0];
			x.MAC_tableAdd.nMAC[1] = mac[1];
			x.MAC_tableAdd.nMAC[2] = mac[2];
			x.MAC_tableAdd.nMAC[3] = mac[3];
			x.MAC_tableAdd.nMAC[4] = mac[4];
			x.MAC_tableAdd.nMAC[5] = mac[5];
			ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_ADD, (unsigned int)&x.MAC_tableAdd);
			ifx_ethsw_kclose(handle);
			break;

		case IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE:
			handle = ifx_ethsw_kopen("/dev/switch/0");
			memset(&x.MAC_tableRemove, 0x00, sizeof(x.MAC_tableRemove));
			x.MAC_tableRemove.nFId = fid;
			x.MAC_tableRemove.nMAC[0] = mac[0];
			x.MAC_tableRemove.nMAC[1] = mac[1];
			x.MAC_tableRemove.nMAC[2] = mac[2];
			x.MAC_tableRemove.nMAC[3] = mac[3];
			x.MAC_tableRemove.nMAC[4] = mac[4];
			x.MAC_tableRemove.nMAC[5] = mac[5];
			ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE, (unsigned int)&x.MAC_tableRemove);
			ifx_ethsw_kclose(handle);
			break;

		default:
			break;
	}

	return ret;
}
#else
static int mac_entry_setting(unsigned char *mac, uint32_t fid, uint32_t portid, uint32_t agetime, uint32_t st_entry , uint32_t action)
{
    return IFX_SUCCESS;
}
#endif

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
    {
        int port = get_eth_port((struct net_device *)arg);
        if ( port >= 0 )
            return g_eth_prio_queue_map[port][skb->priority > 7 ? 7 : skb->priority];
    }

    return 0;
}


int ppe_directpath_send(uint32_t if_id, struct sk_buff *skb, int32_t len, uint32_t flags)
{
    uint32_t off;

    //  Careful, no any safety check here.
    //  Parameters must be correct.

    off = if_id - 3;

    g_ppe_directpath_data[off].tx_pkt++;

#if !defined(ENABLE_DIRECTPATH_TX_QUEUE) || !ENABLE_DIRECTPATH_TX_QUEUE
    if ( eth_xmit(skb, if_id /* dummy */, 3, if_id /* spid */, 0 /* lowest priority */) )
    {
        g_ppe_directpath_data[off].tx_pkt_dropped++;
        return -1;
    }
    else
        return 0;
#else
    if ( !g_directpath_dma_full && !g_ppe_directpath_data[if_id - 3].skb_list )
    {
        if ( eth_xmit(skb, if_id /* dummy */, 3, if_id /* spid */, 0 /* lowest priority */) )
        {
            g_ppe_directpath_data[off].tx_pkt_dropped++;
            return -1;
        }
        else
            return 0;
    }
    else
    {
        if (
  #if defined(ENABLE_DIRECTPATH_TX_QUEUE_SIZE)
             g_ppe_directpath_data[off].skb_list_size + skb->len + sizeof(struct sw_eg_pkt_header) <= ENABLE_DIRECTPATH_TX_QUEUE_SIZE
  #else
             g_ppe_directpath_data[off].skb_list_len  < ENABLE_DIRECTPATH_TX_QUEUE_PKTS
  #endif
           )
        {
            struct sw_eg_pkt_header pkth = {spid:   if_id,  //  CPU/app0/app1/app2
                                            dpid:   2,      //  eth0/eth1/app0/app1/app2
                                            qid:    0,      //  0 is lowest priority, 3 is highest priority
                                            direct: 0};     //  disable direct forward
            unsigned long sys_flag;

            if ( skb_headroom(skb) < sizeof(struct sw_eg_pkt_header) )
            {
                struct sk_buff *new_skb;

                new_skb = alloc_skb_tx(skb->len);
                if ( !new_skb )
                    goto PPE_DIRECTPATH_SEND_DROP;
                else
                {
                    skb_put(new_skb, skb->len + sizeof(struct sw_eg_pkt_header));
                    memcpy(new_skb->data + sizeof(struct sw_eg_pkt_header), skb->data, skb->len);
                    dev_kfree_skb_any(skb);
                    skb = new_skb;
                }
            }
            else
                skb_push(skb, sizeof(struct sw_eg_pkt_header));
            memcpy(skb->data, &pkth, sizeof(struct sw_eg_pkt_header));

            local_irq_save(sys_flag);
            g_ppe_directpath_data[off].skb_list_len++;
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

            g_ppe_directpath_data[off].tx_pkt_queued++;

            return 0;
        }
        else if ( g_ppe_directpath_data[off].callback.stop_tx_fn )
            g_ppe_directpath_data[off].callback.stop_tx_fn(g_ppe_directpath_data[off].netif);

PPE_DIRECTPATH_SEND_DROP:
        dev_kfree_skb_any(skb);
        g_ppe_directpath_data[off].tx_pkt_dropped++;
        return -1;
    }
#endif
}


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

    TX_QOS_CFG->time_tick = cgu_get_pp32_clock() / 62500;

    qsb_global_set();

    for ( i = 0; i < g_atm_priv_data.max_connections; i++ )
        if ( (g_atm_priv_data.connection_table & (1 << i)) && g_atm_priv_data.connection[i].vcc != NULL )
            set_qsb(g_atm_priv_data.connection[i].vcc, &g_atm_priv_data.connection[i].vcc->qos, i);

    return 0;
}


/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

static int __init amazon_s_eth_init(void)
{
    int ret;
    int port;
    int i;
    char buf[512];
    struct port_cell_info port_cell = {0};
    int showtime = 0;
    void *xdata_addr = NULL;
    char ifname[IFNAMSIZ];

    printk("Loading A5 (MII0/1 + ATM) driver ...... \n");
#if IFX_PPA_DP_DBG_PARAM_ENABLE
    if( ifx_ppa_drv_dp_dbg_param_enable == 1 )
    {
        ethwan = ifx_ppa_drv_dp_dbg_param_ethwan;
        wanitf = ifx_ppa_drv_dp_dbg_param_wanitf;
        ipv6_acc_en= ifx_ppa_drv_dp_dbg_param_ipv6_acc_en;
        wanqos_en= ifx_ppa_drv_dp_dbg_param_wanqos_en;
    }
#endif  //IFX_PPA_DP_DBG_PARAM_ENABLE
    reset_ppe();

    init_local_variables();

    init_hw();

    init_communication_data_structures(g_fwcode);

    ret = alloc_dma();
    if ( ret )
        goto ALLOC_DMA_FAIL;

    ppa_hook_mpoa_setup = mpoa_setup;

    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
    {
        g_atm_priv_data.port[i].dev = atm_dev_register("amazon_s_atm", &g_ppe_atm_ops, -1, 0UL);
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

    for ( port = 0; port < 2; port++ )
    {
        sprintf(ifname, "eth%d", port);
        g_eth_net_dev[port] = alloc_netdev(sizeof(struct eth_priv_data), ifname, eth_init);
        if(g_eth_net_dev[port]){
           ret = register_netdev(g_eth_net_dev[port]);
           if(ret){
                printk("Register Ethernet device %s fail\n", ifname);
                free_netdev(g_eth_net_dev[port]);
                g_eth_net_dev[port] = NULL;
                goto REGISTER_NETDEV_FAIL;
           }
        }else{
            printk("alloc ethernet device error\n");
            goto REGISTER_NETDEV_FAIL;
        }
    }

    ret = request_irq(PPE_MAILBOX_IGU1_INT, mailbox_irq_handler, IRQF_DISABLED, "a5_mailbox_isr", NULL);
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

    ret = pp32_start(g_fwcode);
    if ( ret )
        goto PP32_START_FAIL;

    *MBOX_IGU1_IER = g_mailbox_signal_mask;

    qsb_global_set();
    if ( g_eth_wan_mode == 0 )
        validate_oam_htu_entry();

    start_cpu_port();

#if AR10_EMULATION
    //emulation, only enable port 1-4, port0 & port5 is quite different
    for(port = 1; port < 5; port ++){
        arx_ethsw_port_enable(port);
    }
#endif

    //  turn on DMA TX channels (0, 1) for fast path
    g_dma_device->tx_chan[0]->open(g_dma_device->tx_chan[0]);
    g_dma_device->tx_chan[1]->open(g_dma_device->tx_chan[1]);

#if defined(ENABLE_DFE_LOOPBACK_TEST) && ENABLE_DFE_LOOPBACK_TEST
    //init dfe
    dsl_powerup();
    for(i =0; i < 10000; i ++);
    wait_ppe_sync_state(0);
    for(i =0; i < 10000; i ++);
    wait_ppe_sync_state(1);
    {
        struct port_cell_info info = {0};
        info.port_num = 2;
        info.tx_link_rate[0] = 3000;
        atm_showtime_enter(&info,NULL);
    }

#endif

    ifx_mei_atm_showtime_check(&showtime, &port_cell, &xdata_addr);
    if ( showtime )
        atm_showtime_enter(&port_cell, xdata_addr);

    //  init timer for Data Led
    setup_timer(&g_dsl_led_polling_timer, dsl_led_polling, 0);
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK
    ifx_led_trigger_register("dsl_data", &g_data_led_trigger);
#endif
    register_netdev_event_handler();

    /*  create proc file    */
    //ifx_ppa_drv_proc_init(g_ipv6_acc_en);
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
    ifx_ppa_drv_datapath_mac_entry_setting = mac_entry_setting;
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
REGISTER_NETDEV_FAIL:
    while ( port --){
        if(g_eth_net_dev[port]){
            unregister_netdev(g_eth_net_dev[port]);
            free_netdev(g_eth_net_dev[port]);
            g_eth_net_dev[port] = NULL;
        }
    }
ATM_DEV_REGISTER_FAIL:
    while ( i-- )
        atm_dev_deregister(g_atm_priv_data.port[i].dev);
    free_dma();
ALLOC_DMA_FAIL:
    return ret;
}

static void __exit amazon_s_eth_exit(void)
{
    int i;

    ifx_mei_atm_showtime_enter = NULL;
    ifx_mei_atm_showtime_exit  = NULL;

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
    ifx_ppa_drv_datapath_mac_entry_setting = NULL;
    //Fix warning message ---end
    proc_file_delete();

    unregister_netdev_event_handler();
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK
    ifx_led_trigger_deregister(g_data_led_trigger);
    g_data_led_trigger = NULL;
#endif

    invalidate_oam_htu_entry();

    pp32_stop();

    free_irq(PPE_MAILBOX_IGU1_INT, NULL);

    //unregister_netdev(&g_eth_net_dev[0]);
    for(i = 0; i < NUM_ENTITY(g_eth_net_dev); i ++){
        if(g_eth_net_dev[i]){
            free_netdev(g_eth_net_dev[i]);
            unregister_netdev(g_eth_net_dev[i]);
            g_eth_net_dev[i] = NULL;
        }
    }

    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        atm_dev_deregister(g_atm_priv_data.port[i].dev);

    ppa_hook_mpoa_setup = NULL;

    free_dma();

    clear_local_variables();
}

#if defined(CONFIG_IFX_PPA_DATAPATH)
static int __init amazon_s_eth0addr_setup(char *line)
{
    ethaddr_setup(0, line);

    return 0;
}

static int __init amazon_s_eth1addr_setup(char *line)
{
    ethaddr_setup(1, line);

    return 0;
}

static int __init amazon_s_wan_mode_setup(char *line)
{
    if ( strcmp(line, "1") == 0 )
        ethwan = 1;
    else if ( strcmp(line, "2") == 0 )
        ethwan = 2;

    return 0;
}

static int __init wanitf_setup(char *line)
{
    wanitf = simple_strtoul(line, NULL, 0);

    if ( wanitf > 0xFF )
        wanitf = ~0;

    return 0;
}

static int __init ipv6_acc_en_setup(char *line)
{
    if ( strcmp(line, "0") == 0 )
        ipv6_acc_en = 0;
    else
        ipv6_acc_en = 1;

    return 0;
}

static int __init wanqos_en_setup(char *line)
{
    wanqos_en = simple_strtoul(line, NULL, 0);

    if ( wanqos_en > 8 )
        wanqos_en = 0;

    return 0;
}

 #if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_SIZE)
static int __init directpath_tx_queue_size_setup(char *line)
{
    unsigned long i;

    i = simple_strtoul(line, NULL, 0);
    directpath_tx_queue_size = (int)i;

    return 0;
}
 #endif

 #if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_PKTS)
static int __init directpath_tx_queue_pkts_setup(char *line)
{
    unsigned long i;

    i = simple_strtoul(line, NULL, 0);
    directpath_tx_queue_pkts = (int)i;

    return 0;
}
 #endif
#endif

int32_t ppa_datapath_generic_hook(PPA_GENERIC_DATAPATH_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
    dbg("ppa_datapath_generic_hook cmd 0x%x\n", cmd );
    switch (cmd)  {
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
        return IFX_FAILURE;
    }

    return IFX_FAILURE;

}
module_init(amazon_s_eth_init);
module_exit(amazon_s_eth_exit);
#if defined(CONFIG_IFX_PPA_DATAPATH)
  __setup("ethaddr=", amazon_s_eth0addr_setup);
  __setup("eth1addr=", amazon_s_eth1addr_setup);
  __setup("ethwan=", amazon_s_wan_mode_setup);
  __setup("wanitf=", wanitf_setup);
  __setup("ipv6_acc_en=", ipv6_acc_en_setup);
  __setup("wanqos_en=", wanqos_en_setup);
 #if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_SIZE)
  __setup("directpath_tx_queue_size=", directpath_tx_queue_size_setup);
 #endif
 #if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_PKTS)
  __setup("directpath_tx_queue_pkts=", directpath_tx_queue_pkts_setup);
 #endif
#endif

