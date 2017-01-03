/******************************************************************************
**
** FILE NAME    : ifxmips_ppa_datapath_danube_d4.c
** PROJECT      : UEIP
** MODULES      : MII0/1 Acceleration Package (Danube/Twinpass PPA D4)
**
** DATE         : 12 SEP 2006
** AUTHOR       : Xu Liang
** DESCRIPTION  : ETH Driver (MII0 & MII1) with Acceleration Firmware (D4)
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
** 12 SEP 2006  Xu Liang        Initiate Version
** 26 OCT 2006  Xu Liang        Add GPL header.
** 22 JUN 2009  Xu Liang        Feature Set 2.1
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */
#define VER_FAMILY      0x04        //  bit 0: res
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
#define VER_INTERFACE   0x03        //  bit 0: MII 0
                                    //      1: MII 1
                                    //      2: ATM WAN
                                    //      3: PTM WAN
#define VER_ACCMODE     0x00        //  bit 0: Routing
                                    //      1: Bridging
#define VER_MAJOR       0
#define VER_MID         0
#define VER_MINOR       2



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
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_rcu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_dma_core.h>
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_stack_al.h>
#include <net/ifx_ppa_api_directpath.h>
#include "../../ifx_ppa_datapath.h"
#include "ifxmips_ppa_datapath_fw_danube_d4.h"



/*
 * ####################################
 *        OS Versionn Dependent
 * ####################################
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
  #define MOD_INC_USE_COUNT
  #define MOD_DEC_USE_COUNT
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
  #define MODULE_PARM_ARRAY(a, b)   module_param_array(a, int, NULL, 0)
  #define MODULE_PARM(a, b)         module_param(a, int, 0)
#else
  #define MODULE_PARM_ARRAY(a, b)   MODULE_PARM(a, b)
#endif



/*
 * ####################################
 *   Parameters to Configure PPE
 * ####################################
 */

static int ethwan = 2;

#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_SIZE)
  static int directpath_tx_queue_size = 1524 * 50;
#endif
#if defined(CONFIG_IFX_PPA_DIRECTPATH_TX_QUEUE_PKTS)
  static int directpath_tx_queue_pkts = 50;
#endif

MODULE_PARM(ethwan, "i");
MODULE_PARM_DESC(ethwan, "WAN mode, 2 - ETH1 WAN, 1 - ETH0 mixed.");

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

#define BOARD_DANUBE                            0x01
#define BOARD_TWINPATH_E                        0x02
#define BOARD_TWINPATH_VE                       0x03



/*
 * ####################################
 *              Definition
 * ####################################
 */

#define BOARD_CONFIG                            BOARD_TWINPATH_E

#define ENABLE_MY_MEMCPY                        0

#ifndef CONFIG_IFX_PPA_D4_DUALCORE
  #define ENABLE_SINGLE_CORE_EMUL               1
#endif

#define ENABLE_DEBUG                            1

#define ENABLE_ASSERT                           1

#define DEBUG_DUMP_SKB                          1

#define DEBUG_DUMP_FLAG_HEADER                  1

#define DEBUG_DUMP_INIT                         0

#define DEBUG_FIRMWARE_TABLES_PROC              1

#define DEBUG_MEM_PROC                          1

#define DEBUG_PP32_PROC                         1

#define DEBUG_FW_PROC                           1

#define ENABLE_PROBE_TRANSCEIVER                0

#define ENABLE_HW_FLOWCONTROL                   1

#define PPE_MAILBOX_IGU1_INT                    INT_NUM_IM2_IRL24

#define MY_ETH0_ADDR                            g_my_ethaddr

#define MY_ETH1_ADDR                            (g_my_ethaddr + MAX_ADDR_LEN)

//  default board related configuration
#define ENABLE_DANUBE_BOARD                     1
#define MII0_MODE_SETUP                         REV_MII_MODE
#define MII1_MODE_SETUP                         MII_MODE
#define ENABLE_MII0_TX_CLK_INVERSION            0
#define ENABLE_MII1_TX_CLK_INVERSION            1

//  specific board related configuration
#if defined (BOARD_CONFIG)
  #if BOARD_CONFIG == BOARD_TWINPATH_E || BOARD_CONFIG == BOARD_TWINPATH_VE
    #undef  ENABLE_DANUBE_BOARD
    #define ENABLE_TWINPATH_E_BOARD             1

    #undef  MII1_MODE_SETUP
    #define MII1_MODE_SETUP                     REV_MII_MODE

    #undef  ENABLE_MII1_TX_CLK_INVERSION
    #define ENABLE_MII1_TX_CLK_INVERSION        0
  #endif
#endif

#if !defined(CONFIG_NET_HW_FLOWCONTROL)
  #undef  ENABLE_HW_FLOWCONTROL
  #define ENABLE_HW_FLOWCONTROL                 0
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
    || (defined(ENABLE_ETH_ASSERT) && ENABLE_ETH_ASSERT)
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
#define DBG_ENABLE_MASK_ALL                     (DBG_ENABLE_MASK_ERR | DBG_ENABLE_MASK_DEBUG_PRINT | DBG_ENABLE_MASK_ASSERT \
                                                | DBG_ENABLE_MASK_DUMP_SKB_RX | DBG_ENABLE_MASK_DUMP_SKB_TX                 \
                                                | DBG_ENABLE_MASK_DUMP_FLAG_HEADER | DBG_ENABLE_MASK_DUMP_INIT)

/*
 *  Mailbox Signal Bit
 */
//#define CPU_TO_WAN_TX_SIG                       (1 << 30)
#define WAN_TX_SIG                              (1 << 16)
//#define WAN_RX_SIG(i)                           (1 << (i))
#define DMA_TX_CH2_SIG                          (1 << 0)
//#define WLAN_CPU_TX_SIG                         (1 << 25)

/*
 *  Eth Mode
 */
#define MII_MODE                                1
#define REV_MII_MODE                            2

/*
 *  Default MII1 Hardware Configuration
 */
#define CDM_CFG_DEFAULT                         0x00000000
#define SB_MST_SEL_DEFAULT                      0x00000003
#define ENETS1_CFG_DEFAULT                      (0x00007037 | (RX_HEAD_MAC_ADDR_ALIGNMENT << 18))
#define ENETF1_CFG_DEFAULT                      0x00007010
#define ENETS1_PGCNT_DEFAULT                    0x00020000
#define ENETS1_PKTCNT_DEFAULT                   0x00000200
#define ENETF1_PGCNT_DEFAULT                    0x00020000
#define ENETF1_PKTCNT_DEFAULT                   0x00000200
#define ENETS1_COS_CFG_DEFAULT                  0x00000002  // This enables multiple DMA channels when packets with VLAN is injected. COS mapping is through ETOP_IG_VLAN_COS; It is already preconfigured.
#define ENETS1_DBA_DEFAULT                      0x00000400
#define ENETS1_CBA_DEFAULT                      0x00000AE0
#define ENETF1_DBA_DEFAULT                      0x00001600
#define ENETF1_CBA_DEFAULT                      0x00001800

/*
 *  Constant Definition
 */
#define ETH_WATCHDOG_TIMEOUT                    (10 * HZ)
//#define ETOP_MDIO_PHY1_ADDR                     1
#define ETOP_MDIO_DELAY                         1
#define IDLE_CYCLE_NUMBER                       30000
#define ETH1_TX_TOTAL_CHANNEL_USED              MAX_TX_EMA_CHANNEL_NUMBER

#define DMA_PACKET_SIZE                         1568
#define DMA_ALIGNMENT                           4

#define FWCODE_ROUTING_ACC_D2                   0x02
#define FWCODE_BRIDGING_ACC_D3                  0x03
#define FWCODE_ROUTING_BRIDGING_ACC_D4          0x04
#define FWCODE_ROUTING_BRIDGING_ACC_A4          0x14
#define FWCODE_ROUTING_ACC_D5                   0x05
#define FWCODE_ROUTING_ACC_A5                   0x15

/*
 *  EMA TX Channel Parameters
 */
#define MAX_TX_EMA_CHANNEL_NUMBER               4
#define EMA_ALIGNMENT                           4

/*
 *  Ethernet Frame Definitions
 */
#define ETH_MAC_HEADER_LENGTH                   ETH_HLEN
#define ETH_CRC_LENGTH                          4
#define ETH_MIN_FRAME_LENGTH                    (ETH_ZLEN + ETH_CRC_LENGTH)
#define ETH_MAX_FRAME_LENGTH                    (ETH_FRAME_LEN + ETH_CRC_LENGTH)
#define ETH_MAX_DATA_LENGTH                     ETH_DATA_LEN

/*
 *  TX Frame Definitions
 */
#define MAX_TX_PACKET_ALIGN_BYTES               (EMA_ALIGNMENT - 1)
#define MAX_TX_PACKET_PADDING_BYTES             (EMA_ALIGNMENT - 1)
#define MIN_TX_PACKET_LENGTH                    ETH_ZLEN

/*
 *  EMA Settings
 */
#define EMA_CMD_BUF_LEN                         0x0010
#define EMA_CMD_BASE_ADDR                       (0x1960 << 2)
#define EMA_DATA_BUF_LEN                        0x0100
#define EMA_DATA_BASE_ADDR                      (0x1480 << 2)
#define EMA_WRITE_BURST                         0x02
#define EMA_READ_BURST                          0x02

/*
 *  Firmware Settings
 */
#define WAN_ROUT_NUM                            128
#define LAN_ROUT_NUM                            (256 - WAN_ROUT_NUM)
#define LAN_ROUT_OFF                            WAN_ROUT_NUM

#define WAN_ROUT_MC_NUM                         32

#define BRIDGING_ENTRY_NUM                      256

#define DMA_RX_CH2_DESC_NUM                     24
#define DMA_TX_CH2_DESC_NUM                     DMA_RX_CH2_DESC_NUM
#define DMA_RX_CH1_DESC_NUM                     24
#define ETH1_TX_DESC_NUM                        DMA_RX_CH1_DESC_NUM
#define DMA_RX_CH3_DESC_NUM                     24
#define WLAN_CPU_TX_DESC_NUM                    2
#define PRE_ALLOC_DESC_TOTAL_NUM                (DMA_RX_CH2_DESC_NUM + DMA_TX_CH2_DESC_NUM                          \
                                                + DMA_RX_CH1_DESC_NUM + ETH1_TX_DESC_NUM * (1 + ETH1_TX_TOTAL_CHANNEL_USED))

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
#define DANUBE_PPE                              (KSEG1 | 0x1E180000)
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
#define SB_BUFFER(__sb_addr)    ( (volatile u32 *) ( ( ( (__sb_addr) >= 0x0000 ) && ( (__sb_addr) <= 0x0FFF ) )  ?  PPE_REG_ADDR(__sb_addr) :                           \
                                                     ( ( (__sb_addr) >= 0x1000 ) && ( (__sb_addr) <= 0x17FF ) )  ?  PP32_DATA_MEMORY_RAM1_ADDR((__sb_addr) - 0x1000) :  \
                                                     ( ( (__sb_addr) >= 0x2000 ) && ( (__sb_addr) <= 0x23FF ) )  ?  SB_RAM0_ADDR((__sb_addr) - 0x2000) :                \
                                                     ( ( (__sb_addr) >= 0x2400 ) && ( (__sb_addr) <= 0x2BFF ) )  ?  SB_RAM1_ADDR((__sb_addr) - 0x2400) :                \
                                                     ( ( (__sb_addr) >= 0x2C00 ) && ( (__sb_addr) <= 0x35FF ) )  ?  SB_RAM2_ADDR((__sb_addr) - 0x2C00) :                \
                                                     ( ( (__sb_addr) >= 0x3600 ) && ( (__sb_addr) <= 0x39FF ) )  ?  SB_RAM3_ADDR((__sb_addr) - 0x3600) :                \
                                                 0 ) )

#define FW_VER_ID                               ((volatile struct fw_ver_id *)SB_BUFFER(0x2001))

#define IP_TCPUDP_VERIFICATION_ENABLED          SB_BUFFER(0x10CC)

#define ETX1_DMACH_ON                           SB_BUFFER(0x110E)   //  reserved in new firmware 1.00 and above, in A4, it's base + 0x0006
#define ETH_PORTS_CFG                           ((volatile struct eth_ports_cfg *)          SB_BUFFER(0x1114))
#define LAN_ROUT_TBL_CFG                        ((volatile struct lan_rout_tbl_cfg *)       SB_BUFFER(0x1116))
#define WAN_ROUT_TBL_CFG                        ((volatile struct wan_rout_tbl_cfg *)       SB_BUFFER(0x1117))
#define GEN_MODE_CFG                            ((volatile struct gen_mode_cfg *)           SB_BUFFER(0x1118))
#define BRG_TBL_CFG                             ((volatile struct brg_tbl_cfg *)            SB_BUFFER(0x1119))
#define CFG_ROUT_MAC_NO                         SB_BUFFER(0x111A)
#define TX_QOS_CFG                              SB_BUFFER(0x111B)   //  not implemented yet
#define BRG_VLAN_CFG(i)                         ((volatile struct brg_vlan_cfg *)           SB_BUFFER(0x1170 + (i)))    /*  i < 8   */
#define BRG_RT_ID_TBL(i)                        ((volatile struct brg_rt_id_tbl *)          SB_BUFFER(0x1780 + (i)))    /*  i < 8   */

#define ETH1_TX_MIB_TBL                         ((volatile struct eth1_tx_mib_tbl *)        SB_BUFFER(0x1130))

#define ITF_MIB_TBL(i)                          ((volatile struct itf_mib *)                SB_BUFFER(0x3980 + (i) * 16))   /*  i < 8   */

#define BRG_FWD_HIT_STAT_TBL(i)                 SB_BUFFER(0x11B0 + (i))         /*  i < 8   */
#define BRIDGING_FORWARD_TBL(i)                 SB_BUFFER(0x2350 + (i) * 4)     /*  i < 256 */

#define DEFAULT_ITF_VLAN_MAP(i)                 ((volatile struct default_itf_vlan_map *)   SB_BUFFER(0x1128 + (i)))        /*  i < 8   */
#define SRC_IP_VLAN_MAP(i)                      ((volatile struct src_ip_vlan_map *)        SB_BUFFER(0x1178 + (i) * 2))    /*  i < 4   */
#define ETHTYPE_VLAN_MAP(i)                     ((volatile struct ethtype_vlan_map *)       SB_BUFFER(0x1138 + (i) * 2))    /*  i < 4   */
#define VLAN_VLAN_MAP(i)                        ((volatile struct vlan_vlan_map *)          SB_BUFFER(0x1788 + (i) * 3))    /*  i < 8   */

#define PPPOE_CFG_TBL(i)                        SB_BUFFER(0x1180 + (i))         /*  i < 8   */
#define MTU_CFG_TBL(i)                          SB_BUFFER(0x1188 + (i))         /*  i < 8   */
#define ROUTER_MAC_CFG_TBL(i)                   SB_BUFFER(0x1190 + (i) * 2)     /*  i < 16  */

#define ROUT_FWD_HIT_STAT_TBL(i)                SB_BUFFER(0x11B8 + (i))
#define ROUT_FORWARD_COMPARE_TBL(i)             ((volatile struct rout_forward_compare_tbl *)   SB_BUFFER(0x2050 + (i) * 3))    /*  i < 256 */
#define ROUT_FORWARD_ACTION_TBL(i)              ((volatile struct rout_forward_action_tbl *)    SB_BUFFER(0x11C0 + (i) * 5))    /*  i < 256 */

#define WAN_ROUT_FORWARD_COMPARE_TBL(i)         ROUT_FORWARD_COMPARE_TBL(i)
#define LAN_ROUT_FORWARD_COMPARE_TBL(i)         ROUT_FORWARD_COMPARE_TBL(LAN_ROUT_TBL_CFG->lan_rout_off + (i))
#define WAN_ROUT_FORWARD_ACTION_TBL(i)          ROUT_FORWARD_ACTION_TBL(i)
#define LAN_ROUT_FORWARD_ACTION_TBL(i)          ROUT_FORWARD_ACTION_TBL(LAN_ROUT_TBL_CFG->lan_rout_off + (i))

#define MC_ROUT_FWD_HIT_STAT_TBL(i)             SB_BUFFER(0x364C + (i))
#define WAN_ROUT_MULTICAST_TBL(i)               ((volatile struct wan_rout_multicast_tbl *)     SB_BUFFER(0x2010 + (i) * 2))    /*  i < 32  */
#define WAN_ROUT_MC_OUTER_VLAN_TBL(i)           ((volatile struct wan_rout_mc_outer_vlan_tbl *) SB_BUFFER(0x17E0 + (i) / 4))    /*  i < 32  */

#define OUTER_VLAN_TBL(i)                       SB_BUFFER(0x17C0 + (i))         /*  i < 32  */

/*
 *  DMA/EMA Descriptor Base Address
 */
#define DMA_RX_CH1_DESC_BASE                    SB_BUFFER(0x16C0)
#define DMA_RX_CH2_DESC_BASE                    SB_BUFFER(0x16F0)
#define DMA_RX_CH3_DESC_BASE                    SB_BUFFER(0x1720)
#define DMA_TX_CH2_DESC_BASE                    SB_BUFFER(0x1750)
#define EMA_TX_CH_DESC_BASE(i)                  SB_BUFFER(0x3800 + (i) * 2 * ETH1_TX_DESC_NUM)  /*  i < 4   */
#define CPU_TO_ETH1_TX_DESC_BASE                ((volatile struct tx_descriptor *)SB_BUFFER(0x38C0))
#define WLAN_CPU_TX_DESC_BASE                   SB_BUFFER(0x3950)

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
/*
 *  Firmware Proc
 */

  //  4 MIB counter, records the nubmer of packets(bytes) dropped
  //  by QoS dispatcher. Should appear in /proc/mib
  #define __CPU_TX_SWAPPER_DROP_PKTS      SB_RAM3_ADDR(0x0308)
  #define __CPU_TX_SWAPPER_DROP_BYTES     SB_RAM3_ADDR(0x0309)
  #define __ETH1_FP_SWAPPER_DROP_PKTS     SB_RAM3_ADDR(0x030A)
  #define __ETH1_FP_SWAPPER_DROP_BYTES    SB_RAM3_ADDR(0x030B)

  //  firmware context, should appear in /proc/fw
  #define __ETH1_TXDES_SWAP_PTR0          SB_RAM3_ADDR(0x030C)
  #define __ETH1_TXDES_SWAP_PTR1          SB_RAM3_ADDR(0x030D)
  #define __ETH1_TXDES_SWAP_PTR2          SB_RAM3_ADDR(0x030E)
  #define __ETH1_TXDES_SWAP_PTR3          SB_RAM3_ADDR(0x030F)

  //  firmware context, should appear in /proc/fw
  #define __ETH1_TX_PKT_CNT0              SB_RAM3_ADDR(0x0310)
  #define __ETH1_TX_PKT_CNT1              SB_RAM3_ADDR(0x0311)
  #define __ETH1_TX_PKT_CNT2              SB_RAM3_ADDR(0x0312)
  #define __ETH1_TX_PKT_CNT3              SB_RAM3_ADDR(0x0313)

  //  firmware context, should appear in /proc/fw
  #define __CPU_TXDES_SWAP_RDPTR          SB_RAM3_ADDR(0x0314)
  #define __ETH1_FP_RXDES_SWAP_RDPTR      SB_RAM3_ADDR(0x0315)
  #define __ETH1_FP_RXDES_DPLUS_WRPTR     SB_RAM3_ADDR(0x0316)

  //  mib counters, should appear in /proc/mib
  #define __CPU_TO_ETH1_TX_PKTS           SB_RAM3_ADDR(0x0318)
  #define __CPU_TO_ETH1_TX_BYTES          SB_RAM3_ADDR(0x0319)

  //  vlan priority to qid mapping configuration, priority [0-7] --> qid [0-3]
  //  0x0320-0x0327
  #define __VLAN_PRI_TO_QID_MAPPING       SB_RAM3_ADDR(0x0320)

  //  mib counters, should appear in /proc/mib
  #define __ETH1_TX_CH0_TOTAL_PDU         SB_RAM3_ADDR(0x0328)
  #define __ETH1_TX_CH0_TOTAL_BYTES       SB_RAM3_ADDR(0x0329)

  #define __ETH1_TX_CH1_TOTAL_PDU         SB_RAM3_ADDR(0x032A)
  #define __ETH1_TX_CH1_TOTAL_BYTES       SB_RAM3_ADDR(0x032B)

  #define __ETH1_TX_CH2_TOTAL_PDU         SB_RAM3_ADDR(0x032C)
  #define __ETH1_TX_CH2_TOTAL_BYTES       SB_RAM3_ADDR(0x032D)

  #define __ETH1_TX_CH3_TOTAL_PDU         SB_RAM3_ADDR(0x032E)
  #define __ETH1_TX_CH3_TOTAL_BYTES       SB_RAM3_ADDR(0x032F)

  //  mib counters, should appear in /proc/mib
  // etx MIB:
  #define __ETH1_TX_THRES_DROP_PKT_CNT0   SB_RAM3_ADDR(0x0330)
  #define __ETH1_TX_THRES_DROP_PKT_CNT1   SB_RAM3_ADDR(0x0331)
  #define __ETH1_TX_THRES_DROP_PKT_CNT2   SB_RAM3_ADDR(0x0332)
  #define __ETH1_TX_THRES_DROP_PKT_CNT3   SB_RAM3_ADDR(0x0333)
#endif

/*
 *  Share Buffer Registers
 */
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
#define PP32_DBG_CTRL                           PP32_DEBUG_REG_ADDR(0x0000)

#define DBG_CTRL_START_SET(value)               ((value) ? (1 << 0) : 0)
#define DBG_CTRL_STOP_SET(value)                ((value) ? (1 << 1) : 0)
#define DBG_CTRL_STEP_SET(value)                ((value) ? (1 << 2) : 0)

/*
 *  PP32 Registers
 */
#define PP32_HALT_STAT                          PP32_DEBUG_REG_ADDR(0x0001)

#define PP32_BRK_SRC                            PP32_DEBUG_REG_ADDR(0x0002)

#define PP32_DBG_PC_MIN(i)                      PP32_DEBUG_REG_ADDR(0x0010 + (i))
#define PP32_DBG_PC_MAX(i)                      PP32_DEBUG_REG_ADDR(0x0014 + (i))
#define PP32_DBG_DATA_MIN(i)                    PP32_DEBUG_REG_ADDR(0x0018 + (i))
#define PP32_DBG_DATA_MAX(i)                    PP32_DEBUG_REG_ADDR(0x001A + (i))
#define PP32_DBG_DATA_VAL(i)                    PP32_DEBUG_REG_ADDR(0x001C + (i))

#define PP32_DBG_CUR_PC                         PP32_DEBUG_REG_ADDR(0x0080)

#define PP32_DBG_TASK_NO                        PP32_DEBUG_REG_ADDR(0x0081)

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
 *  Power Management Unit Registers
 */
#define DANUBE_PMU                              (KSEG1 + 0x1F102000)
#define PMU_PWDCR                               ((volatile u32 *)(DANUBE_PMU + 0x001C))
#define PMU_SR                                  ((volatile u32 *)(DANUBE_PMU + 0x0020))

/*
 *  PP32 Reset
 */
#define PP32_SRST                               PPE_REG_ADDR(0x0020)

/*
 *  helper macro
 */
#define NUM_ENTITY(x)                           (sizeof(x) / sizeof(*(x)))



/*
 * ####################################
 *              Data Type
 * ####################################
 */

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
    unsigned int    eth1_type           :2; //  reserved in new firmware 1.00 and above
    unsigned int    eth0_type           :2;
  };

  struct lan_rout_tbl_cfg {
    unsigned int    lan_rout_num        :9;
    unsigned int    lan_rout_off        :9;
    unsigned int    res1                :4;
    unsigned int    lan_tcpudp_ver_en   :1; //  reserved in new firmware 1.00 and above
    unsigned int    lan_ip_ver_en       :1; //  reserved in new firmware 1.00 and above
    unsigned int    lan_tcpudp_err_drop :1; //  reserved in new firmware 1.00 and above
    unsigned int    lan_rout_drop       :1; //  reserved in new firmware 1.00 and above
    unsigned int    res2                :6;
  };

  struct wan_rout_tbl_cfg {
    unsigned int    wan_rout_num        :9;
    unsigned int    wan_rout_mc_num     :7;
    unsigned int    res1                :4;
    unsigned int    wan_rout_mc_drop    :1;
    unsigned int    res2                :1;
    unsigned int    wan_tcpdup_ver_en   :1; //  reserved in new firmware 1.00 and above
    unsigned int    wan_ip_ver_en       :1; //  reserved in new firmware 1.00 and above
    unsigned int    wan_tcpudp_err_drop :1; //  reserved in new firmware 1.00 and above
    unsigned int    wan_rout_drop       :1; //  reserved in new firmware 1.00 and above
    unsigned int    res3                :6;
  };

  struct gen_mode_cfg {
    unsigned int    cpu1_fast_mode      :1; //  reserved
    unsigned int    wan_fast_mode       :1;
    unsigned int    us_early_discard_en :1;
    unsigned int    res1                :2;
    unsigned int    fast_path_wfq       :3; //  reserved in new firmware 1.00 and above
    unsigned int    dplus_wfq           :8; //  reserved in new firmware 1.00 and above
    unsigned int    etx_wfq             :8; //  reserved in new firmware 1.00 and above
    unsigned int    wan_acc_mode        :2;
    unsigned int    lan_acc_mode        :2;
    unsigned int    acc_mode            :2;
    unsigned int    res2                :1;
    unsigned int    dscp_qos_en         :1;
  };

  struct brg_tbl_cfg {
    unsigned int    brg_entry_num       :9;
    unsigned int    res1                :7;
    unsigned int    br_to_src_port_en   :8; //  reserved in new firmware 1.00 and above
    unsigned int    res2                :2;
    unsigned int    dest_vlan_en        :1; //  reserved in new firmware 1.00 and above
    unsigned int    src_vlan_en         :1; //  reserved in new firmware 1.00 and above
    unsigned int    res3                :3;
    unsigned int    mac_change_drop     :1; //  reserved in new firmware 1.00 and above
  };

  struct brg_vlan_cfg {
    unsigned int    in_etag_ctrl        :2; //  0: unmodified, 1: remove, 2: insertion, 3: replace
    unsigned int    src_ip_vlan_en      :1; //  0: no ip_2_vlan lookup, 1: ip_2_vlan lookup
    unsigned int    ethtype_vlan_en     :1; //  0: no ethtype_2_vlan lookup, 1:  ethtype_2_vlan lookup
    unsigned int    vlan_tag_vlan_en    :1; //  0: no vlan_2_vlan lookup, 1:  vlan_2_vlan lookup
    unsigned int    loop_ctl            :1; //  1: packet received from one interface is allowed to be forwarded to this interface
    unsigned int    vlan_en             :1; //  0: no vlan editing, 1: vlan editing
    unsigned int    mac_change_drop     :1; //  reserved in new firmware 1.00 and above
    unsigned int    res1                :1;
    unsigned int    port_en             :1; //  Port Based VLAN Enable, 0: no default table, 1: search for default table
    unsigned int    res2                :3;
    unsigned int    out_itag_vld        :1; //  Ingress Outer VLAN Valid
    unsigned int    out_etag_ctrl       :2; //  Egress Outer VLAN Tagging Control
    unsigned int    unknown_mc_dest_list:8;
    unsigned int    unknown_uc_dest_list:8;
  };

  struct brg_rt_id_tbl {
    unsigned int    brg_vlanid_en       :1;
    unsigned int    brg_vlanid_range2   :3;
    unsigned int    brg_vlanid_base2    :12;
    unsigned int    res0                :1;
    unsigned int    brg_vlanid_range1   :3;
    unsigned int    brg_vlanid_base1    :12;
  };

  struct brg_forward_tbl {
    //  0h
    unsigned int    mac_52              :32;
    //  4h
    unsigned int    mac_10              :16;
    unsigned int    vci                 :16;    //  reserved, all zero
    //  8h
    unsigned int    all_zero            :32;
    //  Ch
    unsigned int    new_vci             :16;    //  reserved, all zero
    unsigned int    dest_list           :8;
    unsigned int    src_mac_drop        :1;
    unsigned int    itf                 :3;
    unsigned int    res                 :2;
    unsigned int    dslwan_vcid         :2;     //  WAN QID for D4
  };

  // 8 entries
  struct default_itf_vlan_map {
    //  0h
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8;
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
  };

  // 4 entries
  struct src_ip_vlan_map {
    //  0h
    unsigned int    src_ip              :32;
    //  4h
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8;
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
  };

  // 4 entries
  struct ethtype_vlan_map {
    //  0h
    unsigned int    res0                :16;
    unsigned int    ethtype             :16;
    //  4h
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8;
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
  };

  // 8 entries
  struct vlan_vlan_map {
    //  0h
    unsigned int    res0                :4;
    unsigned int    in_vlan_tag         :12;
    unsigned int    res1                :16;
    //  4h
    unsigned int    new_in_vci          :16;
    unsigned int    vlan_member         :8; //  vlan_port_map
    unsigned int    dest_qos            :3;
    unsigned int    outer_vlan_ix       :5;
    /* 2h */
    unsigned int    in_out_etag_ctrl    :32;    //  MSB -> LSB = itf 7 in, itf 7 out, ... , itf 0 in, itf 0 out
  };

  struct rout_forward_compare_tbl {
    //  0h
    unsigned int    src_ip              :32;
    //  4h
    unsigned int    dest_ip             :32;
    //  8h
    unsigned int    src_port            :16;
    unsigned int    dest_port           :16;
  };

  struct rout_forward_action_tbl {
    //  0h
    unsigned int    new_port            :16;
    unsigned int    new_dest_mac54      :16;
    //  4h
    unsigned int    new_dest_mac30      :32;
    //  8h
    unsigned int    new_ip              :32;
    //  Ch
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
    //  10h
    unsigned int    new_in_vci          :16;
    unsigned int    res2                :1;
    unsigned int    out_vlan_ix         :5; //  New Outer VLAN Tag pointed by this field to be inserted
    unsigned int    out_vlan_ins        :1; //  Outer VLAN Insertion Enable
    unsigned int    out_vlan_rm         :1; //  Outer VLAN Remove Enable
    unsigned int    res4                :2;
    unsigned int    mpoa_type           :2; //  reserved in D4
    unsigned int    res5                :1;
    unsigned int    dest_qid            :3; //  not valid for WAN entry
  };

  struct wan_rout_multicast_tbl {
    //  0h
    unsigned int    new_in_vci          :16;
    unsigned int    dest_list           :8;
    unsigned int    pppoe_mode          :1;
    unsigned int    new_src_mac_en      :1;
    unsigned int    in_vlan_ins         :1;
    unsigned int    in_vlan_rm          :1;
    unsigned int    new_src_mac_ix      :4;
    //  4h
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

struct eth1_tx_mib_tbl {
  unsigned long   etx_total_pdu;
  unsigned long   etx_total_bytes;
};

struct itf_mib {
    unsigned long   ig_fast_brg_uni_pkts;
    unsigned long   ig_fast_brg_mul_pkts;
    unsigned long   ig_fast_brg_br_pkts;
    unsigned long   res1;

    unsigned long   ig_fast_rt_uni_udp_pkts;
    unsigned long   ig_fast_rt_uni_tcp_pkts;
    unsigned long   ig_fast_rt_mul_pkts;
    unsigned long   res2;

    unsigned long   ig_fast_rt_bytes;
    unsigned long   ig_fast_brg_bytes;

    unsigned long   ig_cpu_pkts;
    unsigned long   ig_cpu_bytes;

    unsigned long   ig_drop_pkts;
    unsigned long   ig_drop_bytes;

    unsigned long   eg_fast_pkts;
    unsigned long   eg_fast_bytes;
};

/*
 *  Flag Header & Descriptor
 */
#if defined(__BIG_ENDIAN)
  //  flag_header must have same fields as bridging_flag_header in 4-7h
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

  struct tx_descriptor {
    //  0 - 3h
    unsigned int    own         :1;
    unsigned int    c           :1;
    unsigned int    sop         :1;
    unsigned int    eop         :1;
    unsigned int    byteoff     :5;
    unsigned int    res1        :4;
    unsigned int    qid         :3;
    unsigned int    datalen     :16;
    //  4 - 7h
    unsigned int    res2        :4;
    unsigned int    dataptr     :28;
  };
#endif

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

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
    int                             fc_bit;                 /*  net wakeup callback                     */
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    unsigned char                   dev_addr[MAX_ADDR_LEN];
#endif
};



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Network Operations
 */
static void eth_init(struct net_device *);
static struct net_device_stats *eth_get_stats(struct net_device *);
static int eth_open(struct net_device *);
static int eth_stop(struct net_device *);
static int eth0_hard_start_xmit(struct sk_buff *, struct net_device *);
static int eth1_hard_start_xmit(struct sk_buff *, struct net_device *);
static int eth_set_mac_address(struct net_device *, void *);
static int eth_ioctl(struct net_device *, struct ifreq *, int);
//static int eth_change_mtu(struct net_device *, int);
static void eth0_tx_timeout(struct net_device *);
static void eth1_tx_timeout(struct net_device *);

/*
 *  Network operations help functions
 */
static INLINE int get_eth_port(struct net_device *);
static INLINE int eth1_alloc_tx_desc(int, int *);
static INLINE int eth0_xmit(struct sk_buff *, int);
static INLINE int eth1_xmit(struct sk_buff *, int);
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

/*
 *  Buffer manage functions
 */
static INLINE struct sk_buff *alloc_skb_rx(void);
static INLINE struct sk_buff *alloc_skb_tx(unsigned int);

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
static INLINE void init_gpio(int);
static INLINE void init_etop(int, int, int, int);
static INLINE void start_etop(void);
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
#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC
  static int proc_read_route(char *, char **, off_t, int, int *, void *);
  static int proc_write_route(struct file *, const char *, unsigned long, void *);
  static int proc_read_bridge(char *, char **, off_t, int, int *, void *);
  static int proc_write_bridge(struct file *, const char *, unsigned long, void *);
  static int proc_read_mc(char *, char **, off_t, int, int *, void *);
  static int proc_read_genconf(char *, char **, off_t, int, int *, void *);
  static int proc_write_genconf(struct file *, const char *, unsigned long, void *);
  static int proc_read_pppoe(char *, char **, off_t, int, int *, void *);
  static int proc_write_pppoe(struct file *, const char *, unsigned long, void *);
  static int proc_read_mtu(char *, char **, off_t, int, int *, void *);
  static int proc_write_mtu(struct file *, const char *, unsigned long, void *);
  static int proc_read_hit(char *, char **, off_t, int, int *, void *);
  static int proc_write_hit(struct file *, const char *, unsigned long, void *);
  static int proc_read_mac(char *, char **, off_t, int, int *, void *);
  static int proc_write_mac(struct file *, const char *, unsigned long, void *);
  static int proc_read_vlan(char *, char **, off_t, int, int *, void *);
  static int proc_write_vlan(struct file *, const char *, unsigned long, void *);
  static int proc_read_out_vlan(char *, char **, off_t, int, int *, void *);
  static int proc_write_out_vlan(struct file *, const char *, unsigned long, void *);
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
#endif
static int proc_read_prio(char *, char **, off_t, int, int *, void *);
static int proc_write_prio(struct file *, const char *, unsigned long, void *);

static int32_t ppa_datapath_generic_hook(PPA_GENERIC_DATAPATH_HOOK_CMD cmd, void *buffer, uint32_t flag);
/*
 *  Proc File help functions
 */
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
//#if defined(DEBUG_DMA_PROC) && DEBUG_DMA_PROC
//  static INLINE int print_dma_desc(char *, int, int, int);
//#endif
//#if defined(DEBUG_DMA_PROC_UPSTREAM) && DEBUG_DMA_PROC_UPSTREAM
//  static INLINE int print_ema_desc(char *, int, int, int);
//#endif

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
 *  External Variables
 */
//extern int (*indirect_xmit[8])(uint32_t, struct sk_buff *);
//extern int (*device_push[8])(uint32_t, struct sk_buff *);
//extern uint32_t device_push_dev[8];



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

static int g_fwcode = FWCODE_ROUTING_BRIDGING_ACC_D4;

static int g_eth_wan_mode = 0;

//static uint32_t g_vlan_queue_map[8] = {3, 3, 3, 3, 3, 3, 3, 3};

//static struct sk_buff         **eth1_skb_pointers_addr;
//static struct eth_dev_tx_ch     eth1_tx_ch[ETH1_TX_TOTAL_CHANNEL_USED];
//
//#if defined(ENABLE_FW_TX_QOS) && ENABLE_FW_TX_QOS
//  static struct sk_buff       **cpu_to_eth1_skb_pointers_addr;
//  static struct eth_dev_tx_ch   cpu_to_eth1_tx_ch;
//#endif

static unsigned int g_eth1_tx_desc_pos = 0;

static u32 g_mailbox_signal_mask = 0;

static int                      g_f_dma_opened;
static struct dma_device_info  *g_dma_device = NULL;
//static struct sk_buff          *dma_ch2_skb_pointers[DMA_TX_CH2_DESC_NUM];
//static u32                      dma_ch2_tx_not_run;

//static struct eth_priv_data g_eth_priv_data[2];

static struct net_device *g_eth_net_dev[2] = {0};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static struct net_device_ops eth_netdev_ops[2] = {
    {
    .ndo_open                   = eth_open,
    .ndo_stop                   = eth_stop,
    .ndo_set_mac_address        = eth_set_mac_address,
    .ndo_do_ioctl               = eth_ioctl,
    .ndo_tx_timeout             = eth0_tx_timeout,
    .ndo_get_stats              = eth_get_stats,
    .ndo_start_xmit             = eth0_hard_start_xmit,
    },
    {
    .ndo_open                   = eth_open,
    .ndo_stop                   = eth_stop,
    .ndo_set_mac_address        = eth_set_mac_address,
    .ndo_do_ioctl               = eth_ioctl,
    .ndo_tx_timeout             = eth1_tx_timeout,
    .ndo_get_stats              = eth_get_stats,
    .ndo_start_xmit             = eth1_hard_start_xmit,
    },
};
#endif


static int g_eth_prio_queue_map[2][8];

static u8 g_my_ethaddr[MAX_ADDR_LEN * 2] = {0};

static unsigned char **g_cpu1_skb_data = NULL;  //  DMA_RX_CH3_DESC_NUM + 1, the last one is init (uint32_t)flag

static struct proc_dir_entry *g_eth_proc_dir;

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static u32 g_dbg_enable = 0;
  static u32 is_test_mode;
#endif

/*
 *  variable for directpath
 */
//static int g_directpath_tx_full = 0;



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

static void eth_init(struct net_device *dev)
{
    int port = -1;
    u8 *ethaddr;
    u32 val;
    int i;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    struct eth_priv_data *priv = netdev_priv(dev);
#endif

    for(i = 0; i < NUM_ENTITY(g_eth_net_dev); i ++)
    {
        if(!g_eth_net_dev[i]){
            port = i;
            ethaddr = i ? MY_ETH1_ADDR : MY_ETH0_ADDR;
            break;
        }
    }
    if(port < 0){
        printk("All eth netdevice slot are occupied\n");
        return;
    }

    ether_setup(dev);   /*  assign some members */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    /*  hook network operations */
    dev->get_stats       = eth_get_stats;
    dev->open            = eth_open;
    dev->stop            = eth_stop;
    dev->hard_start_xmit = port ? eth1_hard_start_xmit : eth0_hard_start_xmit;
    dev->set_mac_address = eth_set_mac_address;
    dev->do_ioctl        = eth_ioctl;
//    dev->change_mtu      = eth_change_mtu;
    dev->tx_timeout      = port ? eth1_tx_timeout : eth0_tx_timeout;
    dev->watchdog_timeo  = ETH_WATCHDOG_TIMEOUT;

    SET_MODULE_OWNER(dev);
#else
    dev->watchdog_timeo = ETH_WATCHDOG_TIMEOUT;
    dev->netdev_ops = &eth_netdev_ops[port];
    dev->dev_addr = priv->dev_addr;
#endif
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

    priv->enets_igerr  += *ENETS_IGERR(port);
    priv->enets_igdrop += *ENETS_IGDROP(port);
    priv->enetf_egcol  += *ENETF_EGCOL(port);
    priv->enetf_egdrop += *ENETF_EGDROP(port);

    priv->stats.rx_packets = priv->rx_packets
                                             + ITF_MIB_TBL(port)->ig_fast_rt_uni_udp_pkts
                                             + ITF_MIB_TBL(port)->ig_fast_rt_uni_tcp_pkts
                                             + ITF_MIB_TBL(port)->ig_fast_rt_mul_pkts
                                             + ITF_MIB_TBL(port)->ig_fast_brg_uni_pkts
                                             + ITF_MIB_TBL(port)->ig_fast_brg_mul_pkts;
    priv->stats.rx_bytes   = priv->rx_bytes
                                             + ITF_MIB_TBL(port)->ig_fast_brg_bytes
                                             + ITF_MIB_TBL(port)->ig_fast_rt_bytes;
    priv->stats.rx_errors  = priv->rx_errors
                                             + priv->enets_igerr;
    priv->stats.rx_dropped = priv->rx_dropped
                                             + priv->enets_igdrop
                                             + ITF_MIB_TBL(port)->ig_drop_pkts;

    priv->stats.tx_packets = priv->tx_packets + ITF_MIB_TBL(port)->eg_fast_pkts;
    priv->stats.tx_bytes   = priv->tx_bytes + ITF_MIB_TBL(port)->eg_fast_bytes;
    priv->stats.tx_errors  = priv->tx_errors  + priv->enetf_egcol;
    priv->stats.tx_dropped = priv->tx_dropped + priv->enetf_egdrop;

    return &priv->stats;
}

static int eth_open(struct net_device *dev)
{
    MOD_INC_USE_COUNT;

#if defined(ENABLE_PROBE_TRANSCEIVER) && ENABLE_PROBE_TRANSCEIVER
    if ( !probe_transceiver(dev) )
    {
        printk("%s cannot work because of hardware problem\n", dev->name);
        MOD_DEC_USE_COUNT;
        return -1;
    }
#endif
    dbg("%s", dev->name);

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
    if ( (g_eth_priv_data[get_eth_port(dev)].fc_bit = netdev_register_fc(dev, eth_xon)) == 0 )
        printk("Hardware Flow Control register fails\n");
#endif

    turn_on_dma_rx(get_eth_port(dev));

    netif_start_queue(dev);

    return 0;
}

static int eth_stop(struct net_device *dev)
{
#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
//    int port;
    struct eth_priv_data *priv = netdev_priv(dev);
#endif

    turn_off_dma_rx(get_eth_port(dev));

    netif_stop_queue(dev);

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
//    port = get_eth_port(dev);
    if ( priv->fc_bit )
        netdev_unregister_fc(priv->fc_bit);
#endif

    MOD_DEC_USE_COUNT;

    return 0;
}

static int eth0_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int qid;

    if ( skb->priority >= NUM_ENTITY(g_eth_prio_queue_map[0]) )
        qid = g_eth_prio_queue_map[0][NUM_ENTITY(g_eth_prio_queue_map[0]) - 1];
    else
        qid = g_eth_prio_queue_map[0][skb->priority];

    return eth0_xmit(skb, qid);
}

static int eth1_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    int qid;

    if ( skb->priority >= NUM_ENTITY(g_eth_prio_queue_map[1]) )
        qid = g_eth_prio_queue_map[1][NUM_ENTITY(g_eth_prio_queue_map[1]) - 1];
    else
        qid = g_eth_prio_queue_map[1][skb->priority];

    return eth1_xmit(skb, qid);
}

static int eth_set_mac_address(struct net_device *dev, void *p)
{
    struct sockaddr *addr = (struct sockaddr *)p;
#ifdef ROUTER_MAC_CFG_TBL
    u32 addr1, addr2;
    int i;
#endif

    printk("%s: change MAC from %02X:%02X:%02X:%02X:%02X:%02X to %02X:%02X:%02X:%02X:%02X:%02X\n", dev->name,
        (u32)dev->dev_addr[0] & 0xFF, (u32)dev->dev_addr[1] & 0xFF, (u32)dev->dev_addr[2] & 0xFF, (u32)dev->dev_addr[3] & 0xFF, (u32)dev->dev_addr[4] & 0xFF, (u32)dev->dev_addr[5] & 0xFF,
        (u32)addr->sa_data[0] & 0xFF, (u32)addr->sa_data[1] & 0xFF, (u32)addr->sa_data[2] & 0xFF, (u32)addr->sa_data[3] & 0xFF, (u32)addr->sa_data[4] & 0xFF, (u32)addr->sa_data[5] & 0xFF);

#ifdef ROUTER_MAC_CFG_TBL
    addr1 = (((u32)dev->dev_addr[0] & 0xFF) << 24) | (((u32)dev->dev_addr[1] & 0xFF) << 16) | (((u32)dev->dev_addr[2] & 0xFF) << 8) | ((u32)dev->dev_addr[3] & 0xFF);
    addr2 = (((u32)dev->dev_addr[4] & 0xFF) << 24) | (((u32)dev->dev_addr[5] & 0xFF) << 16);
    for ( i = 0; i < 16; i++ )
        if ( ROUTER_MAC_CFG_TBL(i)[0] == addr1 && ROUTER_MAC_CFG_TBL(i)[1] == addr2 )
        {
            ROUTER_MAC_CFG_TBL(i)[0] = (((u32)addr->sa_data[0] & 0xFF) << 24) | (((u32)addr->sa_data[1] & 0xFF) << 16) | (((u32)addr->sa_data[2] & 0xFF) << 8) | ((u32)addr->sa_data[3] & 0xFF);
            ROUTER_MAC_CFG_TBL(i)[1] = (((u32)addr->sa_data[4] & 0xFF) << 24) | (((u32)addr->sa_data[5] & 0xFF) << 16);
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
        GEN_MODE_CFG->us_early_discard_en = 1;
        *ENETS_COS_CFG(port) |= ENETS_COS_CFG_VLAN_SET;     break;
    case DISABLE_VLAN_CLASSIFICATION:
        GEN_MODE_CFG->us_early_discard_en = 0;
        *ENETS_COS_CFG(port) &= ENETS_COS_CFG_VLAN_CLEAR;   break;
    case ENABLE_DSCP_CLASSIFICATION:
        GEN_MODE_CFG->us_early_discard_en = 1;
        //*ENETS_COS_CFG(port) |= ENETS_COS_CFG_DSCP_SET;     break;
        GEN_MODE_CFG->dscp_qos_en = 1;                      break;
    case DISABLE_DSCP_CLASSIFICATION:
        GEN_MODE_CFG->us_early_discard_en = 0;
        //*ENETS_COS_CFG(port) &= ENETS_COS_CFG_DSCP_CLEAR;   break;
        GEN_MODE_CFG->dscp_qos_en = 0;                      break;
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
        *ENETS_CFG(port) |= ENETS_CFG_DPMC_SET;
    case ETH_MAP_PKT_PRIO_TO_Q:
        {
            struct ppe_prio_q_map cmd;

            if ( copy_from_user(&cmd, ifr->ifr_data, sizeof(cmd)) )
                return -EFAULT;

            if ( cmd.qid < 0 || cmd.qid > 3 || cmd.qid == 2 /* reserved for fast path */
                || cmd.pkt_prio < 0 || cmd.pkt_prio >= NUM_ENTITY(g_eth_prio_queue_map[0]) )
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

static void eth0_tx_timeout(struct net_device *dev)
{
    int i;
    struct eth_priv_data *priv = netdev_priv(dev);
    //TODO:must restart the TX channels

    priv->tx_errors++;

    for ( i = 0; i < g_dma_device->max_tx_chan_num; i++ )
    {
        if ( i == 2 )   //  TX ch 2 reserved for fast path
            continue;

        g_dma_device->tx_chan[i]->disable_irq(g_dma_device->tx_chan[i]);
    }

    netif_wake_queue(dev);

    return;
}

static void eth1_tx_timeout(struct net_device *dev)
{
    unsigned long sys_flag;

    ASSERT(dev == g_eth_net_dev[1], "dev != &g_eth_net_dev[1]");

    //  must restart TX channel (pending)

    /*  disable TX irq, release skb when sending new packet */
    local_irq_save(sys_flag);
    g_mailbox_signal_mask &= ~WAN_TX_SIG;
    *MBOX_IGU1_IER = g_mailbox_signal_mask;
    local_irq_restore(sys_flag);

    /*  wake up TX queue    */
    netif_wake_queue(dev);

    return;
}

static INLINE int get_eth_port(struct net_device *dev)
{
    return dev == g_eth_net_dev[0] ? 0 : 1;
}

/*
 *  Description:
 *    Allocate a TX descriptor for DMA channel.
 *  Input:
 *    ch     --- int, connection ID
 *    f_full --- int *, a pointer to get descriptor full flag
 *               1: full, 0: not full
 *  Output:
 *    int    --- negative value: descriptor is used up.
 *               else:           index of descriptor relative to the first one
 *                               of this channel.
 */
static INLINE int eth1_alloc_tx_desc(int ch, int *f_full)
{
    int desc_pos;

    ASSERT(f_full, "pointer \"f_full\" must be valid!");
    *f_full = 1;

    if ( CPU_TO_ETH1_TX_DESC_BASE[g_eth1_tx_desc_pos].own )     //  hold by MIPS
    {
        desc_pos = (int)g_eth1_tx_desc_pos;

        if ( ++g_eth1_tx_desc_pos == ETH1_TX_DESC_NUM )
            g_eth1_tx_desc_pos = 0;

        if ( CPU_TO_ETH1_TX_DESC_BASE[g_eth1_tx_desc_pos].own ) //  hold by MIPS
            *f_full = 0;
    }
    else
        desc_pos = -1;

    return desc_pos;
}

static INLINE int eth0_xmit(struct sk_buff *skb, int ch)
{
    int len;
    struct eth_priv_data *priv;

    len = skb->len < MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH : skb->len;

    g_dma_device->current_tx_chan = ch;

    dump_skb(skb, DUMP_SKB_LEN, "eth0_xmit", ch, 1);

    g_eth_net_dev[0]->trans_start = jiffies;
    priv = netdev_priv(g_eth_net_dev[0]);

    priv->tx_packets++;
    priv->tx_bytes += len;

    if ( dma_device_write(g_dma_device,
                          skb->data,
                          len,
                          skb)
         != len )
    {
        dev_kfree_skb_any(skb);
        priv->tx_errors++;
        priv->tx_dropped++;
    }

    return 0;
}

static INLINE int eth1_xmit(struct sk_buff *skb, int ch)
{
    unsigned long sys_flag;
    int f_full;
    int desc_pos;
    volatile struct tx_descriptor *desc;
    struct tx_descriptor reg_desc;
    struct sk_buff *skb_to_free;
    u32 byteoff;
    struct eth_priv_data *priv;

    g_eth_net_dev[1]->trans_start = jiffies;
    priv = netdev_priv(g_eth_net_dev[1]);
    priv->tx_packets++;
    priv->tx_bytes += skb->len;

    //  allocate descriptor
    local_irq_save(sys_flag);
    desc_pos = eth1_alloc_tx_desc(ch, &f_full);
    if ( f_full )
    {
        *MBOX_IGU1_ISRC = WAN_TX_SIG;
        netif_stop_queue(g_eth_net_dev[1]);
        g_mailbox_signal_mask |= WAN_TX_SIG;
        *MBOX_IGU1_IER = g_mailbox_signal_mask;
    }
    local_irq_restore(sys_flag);

    if ( desc_pos < 0 )
        goto ETH1_XMIT_FAIL;

    /*  reserve space to put pointer in skb */
    byteoff = (u32)skb->data & 15;
    if ( skb_headroom(skb) < sizeof(struct sk_buff *) + byteoff
        || skb->end - skb->data < 1554 /* 1518 (max ETH frame) + 22 (reserved for firmware) + 6 (PPPoE) + 4 x 2 (2 VLAN) */
        || skb_shared(skb)
        || skb_cloned(skb) )
    {
        struct sk_buff *new_skb;

        new_skb = alloc_skb_tx(DMA_PACKET_SIZE);    //  may be used by RX fastpath later
        if ( !new_skb )
        {
            err("no memory");
            goto ETH1_XMIT_FAIL;
        }
        skb_put(new_skb, skb->len);
        my_memcpy(new_skb->data, skb->data, skb->len);
        dev_kfree_skb_any(skb);
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
    }

    /*  load descriptor from memory */
    desc = CPU_TO_ETH1_TX_DESC_BASE + desc_pos;
    reg_desc = *desc;

    /*  free previous skb   */
    ASSERT((reg_desc.dataptr & 15) == 0, "reg_desc.dataptr (%#08x) must be 4 DWORDS aligned", reg_desc.dataptr);
    ASSERT(*(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)) > 0x80000000, "pointer to skb is wrong, desc->dataptr = %08X, skb = %08X", reg_desc.dataptr, *(volatile u32 *)(KSEG1 | (reg_desc.dataptr - 4)));
    skb_to_free = *(struct sk_buff **)(KSEG1 | (reg_desc.dataptr - 4));
    ASSERT((reg_desc.dataptr & 0x1FFFFFF0) == ((u32)skb_to_free->data & 0x1FFFFFF0), "skb_to_free = %#08x, data = %#08x, reg_desc.dataptr = %#08x", (u32)skb_to_free, (u32)skb_to_free->data, (u32)reg_desc.dataptr);
    dev_kfree_skb_any(skb_to_free);

    /*  update descriptor   */
    reg_desc.dataptr = (u32)skb->data & 0x1FFFFFF0;
    reg_desc.byteoff = byteoff;
    reg_desc.datalen = skb->len < MIN_TX_PACKET_LENGTH ? MIN_TX_PACKET_LENGTH + ETH_CRC_LENGTH : skb->len + ETH_CRC_LENGTH; //  if packet length is less than 60, pad it to 60 bytes
    reg_desc.qid     = ch;
    reg_desc.own     = 0;
    reg_desc.c       = 0;

    dump_skb(skb, DUMP_SKB_LEN, "eth1_xmit", ch, 1);

    /*  write discriptor to memory and write back cache */
    *((volatile u32 *)desc + 1) = *((u32 *)&reg_desc + 1);
    *(volatile u32 *)desc = *(u32 *)&reg_desc;

    return 0;

ETH1_XMIT_FAIL:
    priv->tx_dropped++;
    dev_kfree_skb_any(skb);
    return 0;
}

#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
static void eth_xon(struct net_device *dev)
{
    struct eth_priv_data *priv = netdev_priv(dev);
    clear_bit(priv->.fc_bit, &netdev_fc_xoff);
    if ( netif_running(g_eth_net_dev[0]) || netif_running(g_eth_net_dev[1]) )
    {
        if ( g_dma_device->rx_chan[0]->control == IFX_DMA_CH_ON )
                g_dma_device->rx_chan[0]->open(g_dma_device->rx_chan[0]);
    }
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

    *ENETS_MAC_DA0(port) = (dev->dev_addr[0] << 24) | (dev->dev_addr[1] << 16) | (dev->dev_addr[2] << 8) | dev->dev_addr[3];
    *ENETS_MAC_DA1(port) = (dev->dev_addr[4] << 24) | (dev->dev_addr[3] << 16);

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
            }
        }
        break;
    case ETHTOOL_GDRVINFO:  /*  get driver information          */
        {
            struct ethtool_drvinfo info;
            char str[32];

            memset(&info, 0, sizeof(info));
            strncpy(info.driver, "Danube Eth Driver (D4)", sizeof(info.driver) - 1);
            sprintf(str, "%d.%d.%d", FW_VER_ID->family, FW_VER_ID->major, FW_VER_ID->minor);
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
        *ETOP_MDIO_CFG |= ETOP_MDIO_CFG_SMRST(port) | ETOP_MDIO_CFG_UMM(port, 1);
        break;
    default:
        return -EOPNOTSUPP;
    }

    return 0;
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
    //*VLAN_PRI_TO_QID_MAPPING(req->pri) = req->cos_value & 0x03;
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

/*
 *  Description:
 *    Allocate a sk_buff for TX path using.
 *  Input:
 *    size     --- unsigned int, size of the buffer
 *  Output:
 *    sk_buff* --- 0:    Failed
 *                 else: Pointer to sk_buff
 */
static INLINE struct sk_buff *alloc_skb_tx(unsigned int size)
{
    struct sk_buff *skb;

    skb = dev_alloc_skb(size + 32);
    if ( skb )
    {
        skb_reserve(skb, (~((u32)skb->data + 15) & 15) + 16);
        ASSERT(((u32)skb->data & 15) == 0, "skb->data (%#x) is not 4 DWORDS aligned", (u32)skb->data);
        *((u32 *)skb->data - 1) = (u32)skb;
#ifndef CONFIG_MIPS_UNCACHED
        dma_cache_wback((u32)skb->data - sizeof(u32), sizeof(u32));
#endif
    }
    return skb;
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
    u32 mailbox_signal;

    mailbox_signal = *MBOX_IGU1_ISR & g_mailbox_signal_mask;
    if ( !mailbox_signal )
        return IRQ_HANDLED;
    *MBOX_IGU1_ISRC = mailbox_signal;

    if ( (mailbox_signal & DMA_TX_CH2_SIG) )
    {
        const int upper_limit = 1000;
        int i = 0;

        while ( !(*DMA_TX_CH2_DESC_BASE & 0x80000000) && i++ < upper_limit );

        if ( !(*DMA_TX_CH2_DESC_BASE & 0x80000000) )
        {
            err("The Central DMA TX Channel 2 Descriptor is still not updated after %d times check!\n", upper_limit);
        }
        else
        {
            g_mailbox_signal_mask &= ~DMA_TX_CH2_SIG;
            g_dma_device->tx_chan[2]->open(g_dma_device->tx_chan[2]);
            *MBOX_IGU1_IER = g_mailbox_signal_mask;
        }
    }

    if ( (mailbox_signal & WAN_TX_SIG) )
    {
        g_mailbox_signal_mask &= ~WAN_TX_SIG;
        *MBOX_IGU1_IER = g_mailbox_signal_mask;

        netif_wake_queue(g_eth_net_dev[1]);
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

                //  channel 1, 2, 3 is for fast path
                if ( i == 1 || i == 2 || i == 3 )
                    g_dma_device->rx_chan[i]->dir = 1;
                g_dma_device->rx_chan[i]->open(g_dma_device->rx_chan[i]);
                if ( i == 1 || i == 2 || i == 3 )
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
    if ( opt )
        dev_kfree_skb_any((struct sk_buff *)opt);
    else if ( dataptr )
        kfree(dataptr);

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
            if ( i == 2 )
                continue;

            if ( g_dma_device->tx_chan[i]->control == IFX_DMA_CH_ON )
                g_dma_device->tx_chan[i]->enable_irq(g_dma_device->tx_chan[i]);
        }
        break;
    case TRANSMIT_CPT_INT:
        dbg("eth0 TX buffer released!");
        for( i = 0; i < g_dma_device->max_tx_chan_num; i++ )
        {
            if ( i == 2 )
                continue;

            g_dma_device->tx_chan[i]->disable_irq(g_dma_device->tx_chan[i]);
        }
        netif_wake_queue(g_eth_net_dev[0]);
        break;
    default:
        dbg("unkown DMA interrupt event - %d", status);
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
    struct eth_priv_data *priv = NULL;

    len = dma_device_read(dma_dev, &buf, (void **)&skb);
    if ( !skb )
        return 0;

    ASSERT(buf != NULL, "skb = %08x, buf = %08x", (unsigned int)skb, (unsigned int)buf);
    ASSERT((u32)skb >= 0x80000000, "can not restore skb pointer (ch %d) --- skb = 0x%08X", dma_dev->current_rx_chan, (u32)skb);
    ASSERT(skb->data == buf && skb->tail == buf, "skb->data (%08X) or skb->tail (%08X) is corrupted", (u32)skb->data, (u32)skb->tail);
    ASSERT(len <= (u32)skb->end - (u32)skb->data, "pakcet is too large: %d", len);
    ASSERT(*(struct sk_buff **)((((u32)buf | KSEG1) & ~0x0F) - 4) == skb, "address is not match: skb = %08X, *(struct sk_buff **)((((u32)buf | KSEG1) & ~0x0F) - 4) = %08X", (u32)skb, *(u32 *)((((u32)buf | KSEG1) & ~0x0F) - 4));

    //  work around due to some memory problem
    //skb->data = skb->tail = buf;

    dump_flag_header(g_fwcode, (struct flag_header *)skb->data, __FUNCTION__, dma_dev->current_rx_chan, 0);

    header = (struct flag_header *)skb->data;
    header_val = *((u32 *)skb->data + 1);

    //  pl_byteoff
    off = header_val & ((1 << 5) - 1);

    len -= sizeof(struct flag_header) + off + ETH_CRC_LENGTH;
    ASSERT(len >= 60, "pakcet is too small: %d", len);

    skb->data += sizeof(struct flag_header) + off;
    skb->tail = skb->data + len;
    skb->len  = len;

    dump_skb(skb, DUMP_SKB_LEN, "dma_rx_int_handler", dma_dev->current_rx_chan, 0);

    //  implement indirect fast path
    if ( (header_val & (1 << 21)) && (header_val & 0xFF000000) )    //  acc_done == 1 && dest_list != 0
    {
        if ( (header_val & (1 << 25)) )         //  1 - eth1
        {
            eth1_xmit(skb, 3);
            return 0;
        }
        else if ( (header_val & (1 << 24)) )    //  0 - eth0
        {
            eth0_xmit(skb, 3);
            return 0;
        }
        else
        {
            //  fill MAC info into skb, in case the driver hooked to direct path need such info
            skb->dev = g_eth_net_dev[0];
            skb->protocol = eth_type_trans(skb, g_eth_net_dev[0]);
            skb_push(skb, ETH_HLEN);

            for ( off = 27; off < 29; off++ )
            {
                if ( (header_val & (1 << off)) )    //  3...4
                {
                    int if_id = off - 27;

                    if ( (g_ppe_directpath_data[if_id].flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
                    {
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

    off = (header_val >> 5) & 0x07;
    switch ( off )
    {
    case 0: //  MII0
    case 1: //  MII1
        priv = netdev_priv(g_eth_net_dev[off]);
        if ( netif_running(g_eth_net_dev[off]) )
        {
            skb->dev = g_eth_net_dev[off];
            skb->protocol = eth_type_trans(skb, g_eth_net_dev[off]);

            if ( netif_rx(skb) == NET_RX_DROP )
            {
#if defined(ENABLE_HW_FLOWCONTROL) && ENABLE_HW_FLOWCONTROL
                if ( priv->fc_bit && !test_and_set_bit(priv->fc_bit, &netdev_fc_xoff) )
                    g_dma_device->rx_chan[0]->close(g_dma_device->rx_chan[0]);
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

static inline void reset_ppe(void)
{
#ifdef MODULE
    //unsigned int etop_cfg;
    //unsigned int etop_mdio_cfg;
    //unsigned int etop_ig_plen_ctrl;
    //unsigned int enet_mac_cfg;
    //
    //etop_cfg            = *IFX_PP32_ETOP_CFG;
    //etop_mdio_cfg       = *IFX_PP32_ETOP_MDIO_CFG;
    //etop_ig_plen_ctrl   = *IFX_PP32_ETOP_IG_PLEN_CTRL;
    //enet_mac_cfg        = *IFX_PP32_ENET_MAC_CFG;
    //
    //*IFX_PP32_ETOP_CFG &= ~0x03C0;

    //  reset PPE
    ifx_rcu_rst(IFX_RCU_DOMAIN_PPE, IFX_RCU_MODULE_PPA);

    //*IFX_PP32_ETOP_MDIO_CFG     = etop_mdio_cfg;
    //*IFX_PP32_ETOP_IG_PLEN_CTRL = etop_ig_plen_ctrl;
    //*IFX_PP32_ENET_MAC_CFG      = enet_mac_cfg;
    //*IFX_PP32_ETOP_CFG          = etop_cfg;
#endif
}

static INLINE void init_pmu(void)
{
    //   5 - DMA, should be enabled in DMA driver
    //  12 - GPTC, ? question mark, why GPTC is enabled
    //  13 - PPE
    //  15 - AHB
    *PMU_PWDCR &= ~((1 << 15) | (1 << 13) | (1 << 12) | (1 << 5));
}

static INLINE void init_gpio(int mii1_mode)
{
    *(unsigned long *)0xBE100B1C = (*(unsigned long *)0xBE100B1C & 0x00009859) | 0x00006786;
    *(unsigned long *)0xBE100B20 = (*(unsigned long *)0xBE100B20 & 0x00009859) | 0x000067a6;
    *(unsigned long *)0xBE100B4C = (*(unsigned long *)0xBE100B4C & 0x00000fa7) | 0x0000e058;
    *(unsigned long *)0xBE100B50 = (*(unsigned long *)0xBE100B50 & 0x00000fa7) | 0x0000f058;

    if ( mii1_mode == MII_MODE )
    {
        *(unsigned long *)0xBE100B18 = (*(unsigned long *)0xBE100B18 & 0x00009859) | 0x00006020;
        *(unsigned long *)0xBE100B48 = (*(unsigned long *)0xBE100B48 & 0x00000fa7) | 0x00007000;
        *(unsigned long *)0xBE100B24 = *(unsigned long *)0xBE100B24 | 0x00006020;
        *(unsigned long *)0xBE100B54 = *(unsigned long *)0xBE100B54 | 0x00007000;
        dbg("MII1 = MII_MODE");
    }
    else
    {
        *(unsigned long *)0xBE100B18 = (*(unsigned long *)0xBE100B18 & 0x00009859) | 0x00000786;
        *(unsigned long *)0xBE100B48 = (*(unsigned long *)0xBE100B48 & 0x00000fa7) | 0x00008058;
        *(unsigned long *)0xBE100B24 = *(unsigned long *)0xBE100B24 | 0x00000786;
        *(unsigned long *)0xBE100B54 = *(unsigned long *)0xBE100B54 | 0x00000058;
        dbg("MII1 = REV_MII_MODE");
    }

    dbg("DANUBE_GPIO_P0_ALTSEL0(0xBE100B1C) = 0x%08X", *(unsigned int *)0xBE100B1C);
    dbg("DANUBE_GPIO_P0_ALTSEL1(0xBE100B20) = 0x%08X", *(unsigned int *)0xBE100B20);
    dbg("DANUBE_GPIO_P1_ALTSEL0(0xBE100B4C) = 0x%08X", *(unsigned int *)0xBE100B4C);
    dbg("DANUBE_GPIO_P1_ALTSEL1(0xBE100B50) = 0x%08X", *(unsigned int *)0xBE100B50);
    dbg("DANUBE_GPIO_P0_DIR(0xBE100B18)     = 0x%08X", *(unsigned int *)0xBE100B18);
    dbg("DANUBE_GPIO_P1_DIR(0xBE100B48)     = 0x%08X", *(unsigned int *)0xBE100B48);
    dbg("DANUBE_GPIO_P0_OD(0xBE100B24)      = 0x%08X", *(unsigned int *)0xBE100B24);
    dbg("DANUBE_GPIO_P1_OD(0xBE100B54)      = 0x%08X", *(unsigned int *)0xBE100B54);
}

static INLINE void init_etop(int mii0_mode, int mii1_mode, int mii0_iclk, int mii1_iclk)
{
    u32 etop_cfg_clear, etop_cfg_set;
    //int i;

    //  reset ETOP
    *((volatile u32 *)(0xBF203000 + 0x0010)) |= (1 << 8);
    //for ( i = 0; i < 0x1000; i++ );
    udelay(10);

    //  disable both eth0 and eth1
    etop_cfg_set = (1 << 0) | (1 << 3);
    etop_cfg_clear = (1 << 6) | (1 << 8) | (1 << 7) | (1 << 9);

    //  set MII or revMII mode
    if ( mii0_mode == REV_MII_MODE )
        etop_cfg_set |= 1 << 1;
    else
        etop_cfg_clear |= 1 << 1;
    if ( mii1_mode == REV_MII_MODE )
        etop_cfg_set |= 1 << 4;
    else
        etop_cfg_clear |= 1 << 4;

    //  invert TX clock if necessary
    if ( mii0_iclk )
        etop_cfg_set |= 1 << 10;
    else
        etop_cfg_clear |= 1 << 10;
    if ( mii1_iclk )
        etop_cfg_set |= 1 << 11;
    else
        etop_cfg_clear |= 1 << 11;

    *ETOP_CFG = (*ETOP_CFG | etop_cfg_set) & ~etop_cfg_clear;

    //  set packet length
//    *ETOP_IG_PLEN_CTRL0 = 0x004005EE; //  without PPPoE
    *ETOP_IG_PLEN_CTRL0 = 0x004005F6;   //  with PPPoE support

    *ETOP_MDIO_CFG      = 0x00;

    //*ENET_MAC_CFG(0)    = 0x0807;
    *ENET_MAC_CFG(0)    = 0x081F;
    *ENETS_BUF_CTRL(0)  = 0x1619;

    *ENETS_DBA(0)       = 0x0C00;
    *ENETS_CBA(0)       = 0x10B0;
    *ENETS_CFG(0)       = 0x0058504B;
    *ENETS_PGCNT(0)     = 0x00020000;
    *ENETS_PKTCNT(0)    = 0x0200;

    *ENETF_DBA(0)       = 0x1100;
    *ENETF_CBA(0)       = 0x15C0;
    *ENETF_CFG(0)       = 0x700D;

    //*ENET_MAC_CFG(1)    = 0x0807;
    *ENET_MAC_CFG(1)    = 0x081F;
    *ENETS_BUF_CTRL(1)  = 0x1619;

    *ENETS_DBA(1)       = 0x0750;
    *ENETS_CBA(1)       = 0x1600;
    *ENETS_CFG(1)       = 0x0058504B;
    *ENETS_PGCNT(1)     = ENETS1_PGCNT_DEFAULT;
    *ENETS_PKTCNT(1)    = ENETS1_PKTCNT_DEFAULT;
    *ENETS_COS_CFG(1)   = ENETS1_COS_CFG_DEFAULT;

    *ENETF_DBA(1)       = 0x1650;
    *ENETF_CBA(1)       = 0x17F0;
    *ENETF_CFG(1)       = 0x700D;
    *ENETF_PGCNT(1)     = ENETF1_PGCNT_DEFAULT;
    *ENETF_PKTCNT(1)    = ENETF1_PKTCNT_DEFAULT;

    //  tx page is 13, start transmit threshold is 10
    *ENETF_TXCTRL(0)    = 0x0A;
    *ENETF_TXCTRL(1)    = 0x0A;

    *DPLUS_TXCFG        = 0x000D;
    *DPLUS_TXDB         = 0x1100;
    *DPLUS_TXCB         = 0x15C0;
    *DPLUS_TXCFG        = 0xF00D;

    *DPLUS_RXCFG        = 0x504B;
    *DPLUS_RXPGCNT      = 0x00040000;

    //  enable both eth0 and eth1
//    *ETOP_CFG = (*ETOP_CFG & ~((1 << 0) | (1 << 3))) | ((1 << 6) | (1 << 8) | (1 << 7) | (1 << 9));

#if 0
    {
        u32 sys_flag;
        u32 dma_ps;

        //  burst length is set to 4 by default
        local_irq_save(sys_flag);
        dma_ps = *(volatile u32 *)0xBE104140;
        *(volatile u32 *)0xBE104140 = 0;
        *(volatile u32 *)0xBE104144 = (*(volatile u32 *)0xBE104144 & ~0x3C) | 0x28;
        *(volatile u32 *)0xBE104140 = dma_ps;
        local_irq_restore(sys_flag);
    }
#endif
}

static INLINE void start_etop(void)
{
    u32 mii_mode;
    int i;

    // flipover MII mode of MII0
    mii_mode = *ETOP_CFG & (1 << 1);
    *ETOP_CFG ^= 1 << 1;

    //  reset ENETS0
    *PP32_SRST &= ~(1 << 10);
    for ( i = 0; i < 0x10; i++ );
    *PP32_SRST |= 1 << 10;
    for ( i = 0; i < 0x1000; i++ );

    //  enable both eth0 and eth1
    //*ETOP_CFG = (*ETOP_CFG & ~((1 << 0) | (1 << 3))) | ((1 << 6) | (1 << 8) | (1 << 7) | (1 << 9));
    *ETOP_CFG |= (1 << 6) | (1 << 8) | (1 << 7) | (1 << 9);
    for ( i = 0; i < 0x1000; i++ );
    *ETOP_CFG &= ~((1 << 0) | (1 << 3));
    *ETOP_CFG = (*ETOP_CFG & ~(1 << 1)) | mii_mode;
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
    *MBOX_IGU1_IER  = 0x00000000;   //  Don't need to enable RX interrupt, DMA driver handle RX path.
    *MBOX_IGU0_ISRC = 0xFFFFFFFF;
    *MBOX_IGU0_IER  = 0x01;
}

static INLINE void clear_share_buffer(void)
{
    volatile u32 *p = SB_RAM0_ADDR(0);
    unsigned int i;

    /*  write all zeros only    */
    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN + SB_RAM2_DWLEN + SB_RAM3_DWLEN; i++ )
        *p++ = 0;

    //  Configure share buffer master selection
    *SB_MST_SEL |= 0x03;
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
#ifdef CONFIG_ADM6996_SUPPORT_SAMURAI_TWINPASS
    extern int ifx_sw_vlan_add(int, int, int);
    extern int ifx_sw_vlan_del(int, int);

    int i, j;

    for(i = 0; i< 6; i++)
        for(j = 0; j<6; j++)
            ifx_sw_vlan_del(i, j);

    ifx_sw_vlan_add(1, 1, 0);
    ifx_sw_vlan_add(2, 2, 0);
    ifx_sw_vlan_add(4, 4, 0);
    ifx_sw_vlan_add(2, 1, 0);
    ifx_sw_vlan_add(4, 1, 0);
    ifx_sw_vlan_add(1, 2, 0);
    ifx_sw_vlan_add(2, 2, 0);
    ifx_sw_vlan_add(4, 2, 0);
    ifx_sw_vlan_add(1, 4, 0);
    ifx_sw_vlan_add(2, 4, 0);
    ifx_sw_vlan_add(4, 4, 0);

    ifx_sw_vlan_add(3, 3, 1);
    ifx_sw_vlan_add(5, 5, 1);
    ifx_sw_vlan_add(3, 5, 1);
    ifx_sw_vlan_add(5, 3, 1);
#endif
}

static INLINE void init_hw(void)
{
    init_pmu();

    init_gpio(MII1_MODE_SETUP);

    init_etop(MII0_MODE_SETUP, MII1_MODE_SETUP, ENABLE_MII0_TX_CLK_INVERSION, ENABLE_MII1_TX_CLK_INVERSION);

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
    int i;

    /*  download firmware   */
    switch ( fwcode )
    {
    case FWCODE_ROUTING_BRIDGING_ACC_D4:
        ret = pp32_download_code(firmware_binary_code, sizeof(firmware_binary_code) / sizeof(*firmware_binary_code), firmware_binary_data, sizeof(firmware_binary_data) / sizeof(*firmware_binary_data));
        break;

    default:
        ret = -1;
    }
    if ( ret )
        return ret;

    /*  firmware specific initialization    */
    ret = pp32_specific_init(fwcode, NULL);
    if ( ret )
        return ret;

    /*  run PP32    */
    *PP32_DBG_CTRL = DBG_CTRL_START_SET(1);
    /*  idle for a while to let PP32 init itself    */
    for ( i = 0; i < IDLE_CYCLE_NUMBER; i++ );

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
    *PP32_DBG_CTRL = DBG_CTRL_STOP_SET(1);
}

static INLINE int init_local_variables(void)
{
    int i, j;

    if ( ethwan == 1 )
        g_eth_wan_mode = 2;
    else if ( ethwan == 2 )
        g_eth_wan_mode = 0;

//    memset(g_eth_priv_data, 0, sizeof(g_eth_priv_data));

    i = 0;
    {
        int tx_q[] = {3, 1, 0};
        int max_packet_priority = NUM_ENTITY(g_eth_prio_queue_map[i]);  //  assume only 8 levels are used in Linux
        int tx_num_q = NUM_ENTITY(tx_q);
        int priorities_per_q = (max_packet_priority + tx_num_q - 1) / tx_num_q;

        for ( j = 0; j < NUM_ENTITY(g_eth_prio_queue_map[i]); j++ )
            g_eth_prio_queue_map[i][j] = tx_q[j / priorities_per_q];
    }

    i = 1;
    {
        int tx_q[] = {3, 2, 1, 0};
        int max_packet_priority = NUM_ENTITY(g_eth_prio_queue_map[i]);  //  assume only 8 levels are used in Linux
        int tx_num_q = NUM_ENTITY(tx_q);
        int priorities_per_q = (max_packet_priority + tx_num_q - 1) / tx_num_q;

        for ( j = 0; j < NUM_ENTITY(g_eth_prio_queue_map[i]); j++ )
            g_eth_prio_queue_map[i][j] = tx_q[j / priorities_per_q];
    }

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    is_test_mode = 0;
#endif

    return 0;
}

static INLINE void clear_local_variables(void)
{
}

static INLINE void init_communication_data_structures(int fwcode)
{
#if defined(ENABLE_FW_TX_QOS) && ENABLE_FW_TX_QOS
    u32 etop_ig_vlan_cos;
#endif
    int i;

    *CDM_CFG = CDM_CFG_RAM1_SET(0x00) | CDM_CFG_RAM0_SET(0x00);

    for ( i = 0; i < 1000; i++ );

    *(u32*)ETX1_DMACH_ON    = (1 << ETH1_TX_TOTAL_CHANNEL_USED) - 1;
    //*(u32*)ETH_PORTS_CFG    = 0x00000004;
    *(u32*)LAN_ROUT_TBL_CFG = 0x40200380;
    *(u32*)WAN_ROUT_TBL_CFG = 0x40200380;
    *(u32*)BRG_TBL_CFG      = 0x80000000;
    *(u32*)GEN_MODE_CFG     = 0xE50110A0;
    *CFG_ROUT_MAC_NO        = 0x00000000;   //  0: bridging only, N: N-1 mac entries

    ETH_PORTS_CFG->wan_vlanid_hi = 15;
    ETH_PORTS_CFG->wan_vlanid_lo = 15;
    ETH_PORTS_CFG->eth0_type     = g_eth_wan_mode;  //  2 - ETH0 mixed, 0 - ETH1 WAN
    ETH_PORTS_CFG->eth1_type     = 1;   //  backward compatible

    for ( i = 0; i < 8; i++ )
        *(volatile u32 *)BRG_VLAN_CFG(i) = 0x00040404;

    for ( i = 0; i < 8; i++ )
        *(volatile u32 *)BRG_RT_ID_TBL(i) = 0;

    for ( i = 0; i < 8; i++ )
        *MTU_CFG_TBL(i) = ETH_MAX_DATA_LENGTH;

    BRIDGING_FORWARD_TBL(0)[3] = 0x00000420;
    BRIDGING_FORWARD_TBL(0)[2] = 0x00000000;
    BRIDGING_FORWARD_TBL(0)[1] = 0xFFFF0000;
    BRIDGING_FORWARD_TBL(0)[0] = 0xFFFFFFFF;
    for ( i = 1; i < BRIDGING_ENTRY_NUM; i++ )
    {
        BRIDGING_FORWARD_TBL(i)[0] = 0xFFFFFFFF;
        BRIDGING_FORWARD_TBL(i)[1] = 0xFFFFFFFF;
        BRIDGING_FORWARD_TBL(i)[2] = 0xFFFFFFFF;
        BRIDGING_FORWARD_TBL(i)[3] = 0xFFFFFFFF;
    }
}

static INLINE int alloc_dma(void)
{
    struct sk_buff *skb_pool[PRE_ALLOC_DESC_TOTAL_NUM];
    struct sk_buff **pskb;
    volatile u32 *p;
    int i, j;

    for ( i = 0; i < PRE_ALLOC_DESC_TOTAL_NUM; i++ )
    {
        skb_pool[i] = alloc_skb_rx();
        if ( !skb_pool[i] )
            goto ALLOC_SKB_FAIL;
    }

    g_dma_device = dma_device_reserve("PPE");
    if ( !g_dma_device )
        goto ALLOC_SKB_FAIL;

    g_dma_device->buffer_alloc    = dma_buffer_alloc;
    g_dma_device->buffer_free     = dma_buffer_free;
    g_dma_device->intr_handler    = dma_int_handler;
    g_dma_device->max_rx_chan_num = 4;
    g_dma_device->max_tx_chan_num = 4;
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
    g_dma_device->rx_chan[3]->desc_base = (int)DMA_RX_CH3_DESC_BASE;
    g_dma_device->rx_chan[3]->desc_len  = DMA_RX_CH3_DESC_NUM;
    g_dma_device->rx_chan[1]->req_irq_to_free   = g_dma_device->rx_chan[1]->irq;
    g_dma_device->rx_chan[2]->req_irq_to_free   = g_dma_device->rx_chan[2]->irq;
    g_dma_device->rx_chan[3]->req_irq_to_free   = g_dma_device->rx_chan[3]->irq;

    for ( i = 0; i < g_dma_device->max_tx_chan_num; i++ )
    {
        g_dma_device->tx_chan[i]->control     = IFX_DMA_CH_ON;
    }
    g_dma_device->tx_chan[2]->desc_base = (int)DMA_TX_CH2_DESC_BASE;
    g_dma_device->tx_chan[2]->desc_len  = DMA_TX_CH2_DESC_NUM;
    //g_dma_device->tx_chan[2]->global_buffer_len = DMA_PACKET_SIZE;
    //g_dma_device->tx_chan[2]->peri_to_peri      = 1;

    g_dma_device->port_packet_drop_enable = 1;

    if ( dma_device_register(g_dma_device) != IFX_SUCCESS )
    {
        err("failed in \"dma_device_register\"");
        goto DMA_DEVICE_REGISTER_FAIL;
    }

    pskb = skb_pool;

    p = DMA_RX_CH2_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH2_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 15) == 0, "DMA RX channel 2 data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    p = DMA_TX_CH2_DESC_BASE;
    for ( i = 0; i < DMA_TX_CH2_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 15) == 0, "DMA TX channel 2 data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0x30000000;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    p = DMA_RX_CH1_DESC_BASE;
    for ( i = 0; i < DMA_RX_CH1_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 15) == 0, "DMA RX channel 1 data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    p = (volatile u32 *)CPU_TO_ETH1_TX_DESC_BASE;
    for ( i = 0; i < ETH1_TX_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 15) == 0, "ETH1 CPU TX channel data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    for ( j = 0; j < ETH1_TX_TOTAL_CHANNEL_USED; j++ )
    {
        p = EMA_TX_CH_DESC_BASE(j);
        for ( i = 0; i < ETH1_TX_DESC_NUM; i++ )
        {
            ASSERT(((u32)(*pskb)->data & 15) == 0, "EMA channel %d data pointer (0x%#x) must be 4 DWORDS aligned", j, (u32)(*pskb)->data);
            *p++ = 0x30000000;
            *p++ = ((u32)(*pskb++)->data & 0x1FFFFFF0) >> 2;
        }
    }

#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
    {
        extern u32 ifx_mps_ppa_get_init_status(void);
        do
        {
            g_cpu1_skb_data = (unsigned char **)ifx_mps_ppa_get_init_status();
        } while ( !g_cpu1_skb_data );
        g_cpu1_skb_data = (unsigned char **)((uint32_t)g_cpu1_skb_data | KSEG1);
    }
#else
    {
        extern unsigned char *g_rx_skb_data[];
        g_cpu1_skb_data = g_rx_skb_data;
    }
#endif

    p = DMA_RX_CH3_DESC_BASE;
    pskb = (struct sk_buff **)g_cpu1_skb_data;
    for ( i = 0; i < DMA_RX_CH3_DESC_NUM; i++ )
    {
        ASSERT(((u32)(*pskb)->data & 15) == 0, "DMA RX channel 3 data pointer (0x%#x) must be 4 DWORDS aligned", (u32)(*pskb)->data);
        *p++ = 0xB0000000 | DMA_PACKET_SIZE;
        *p++ = (u32)(*pskb++)->data & 0x1FFFFFF0;
    }

    g_f_dma_opened = 0;
    g_mailbox_signal_mask |= DMA_TX_CH2_SIG;

    return 0;

DMA_DEVICE_REGISTER_FAIL:
    dma_device_release(g_dma_device);
    g_dma_device = NULL;
ALLOC_SKB_FAIL:
    while ( i-- )
        dev_kfree_skb_any(skb_pool[i]);
    return -ENOMEM;
}

static INLINE void free_dma(void)
{
    if ( g_dma_device )
    {
        dma_device_unregister(g_dma_device);
        dma_device_release(g_dma_device);
        g_dma_device = NULL;

        //  TODO
    }
}

#if defined(CONFIG_IFX_PPA_DATAPATH)
static INLINE void ethaddr_setup(unsigned int port, char *line)
{
    u8 *p;
    char *ep;
    int i;

    p = port ? MY_ETH1_ADDR : MY_ETH0_ADDR;
    memset(p, 0, MAX_ADDR_LEN * sizeof(*p));
    for ( i = 0; i < 6; i++ )
   {
        p[i] = line ? simple_strtoul(line, &ep, 16) : 0;
        if ( line )
            line = *ep ? ep + 1 : ep;
    }
    dbg("eth%d mac address %02X-%02X-%02X-%02X-%02X-%02X\n",
        port ? 1 : 0,
        p[0], p[1], p[2],
        p[3], p[4], p[5]);
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
                                  proc_read_route,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_route;

    res = create_proc_read_entry("bridge",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_bridge,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_bridge;

    res = create_proc_read_entry("mc",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_mc,
                                  NULL);

    res = create_proc_read_entry("genconf",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_genconf,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_genconf;

    res = create_proc_read_entry("pppoe",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_pppoe,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_pppoe;

    res = create_proc_read_entry("mtu",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_mtu,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mtu;

    res = create_proc_read_entry("hit",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_hit,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_hit;

    res = create_proc_read_entry("mac",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_mac,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mac;

    res = create_proc_read_entry("vlan",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_vlan,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_vlan;

    res = create_proc_read_entry("out_vlan",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_out_vlan,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_out_vlan;

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
#endif

    res = create_proc_read_entry("prio",
                                  0,
                                  g_eth_proc_dir,
                                  proc_read_prio,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_prio;
}

static INLINE void proc_file_delete(void)
{
    remove_proc_entry("prio",
                      g_eth_proc_dir);

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

    remove_proc_entry("genconf",
                      g_eth_proc_dir);

    remove_proc_entry("mc",
                      g_eth_proc_dir);

    remove_proc_entry("bridge",
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

    len += print_driver_ver(page + len, count - len, "PPE datapath driver info", VER_FAMILY, VER_DRTYPE, VER_INTERFACE, VER_ACCMODE, VER_MAJOR, VER_MID, VER_MINOR);
    len += print_fw_ver(page + len, count - len);

    *eof = 1;

    return len - off;
}

static int proc_read_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i;
    struct eth_priv_data *priv;

    MOD_INC_USE_COUNT;

    for ( i = 0; i < 2; i++ )
    {
        eth_get_stats(g_eth_net_dev[i]);
        priv = netdev_priv(g_eth_net_dev[i]);

        len += sprintf(page + len,         "Port %d\n", i);
        len += sprintf(page + len,         "  Hardware\n");
        len += sprintf(page + len,         "    enets_igerr:  %u\n", priv->enets_igerr);
        len += sprintf(page + len,         "    enets_igdrop: %u\n", priv->enets_igdrop);
        len += sprintf(page + len,         "    enetf_egcol:  %u\n", priv->enetf_egcol);
        len += sprintf(page + len,         "    enetf_egdrop: %u\n", priv->enetf_egdrop);
        len += sprintf(page + len,         "  Firmware\n");
        len += sprintf(page + len,         "    ig_fast_brg_uni_pkts:    %lu\n", ITF_MIB_TBL(i)->ig_fast_brg_uni_pkts);
        len += sprintf(page + len,         "    ig_fast_brg_mul_pkts:    %lu\n", ITF_MIB_TBL(i)->ig_fast_brg_mul_pkts);
        len += sprintf(page + len,         "    ig_fast_brg_br_pkts:     %lu\n", ITF_MIB_TBL(i)->ig_fast_brg_br_pkts);
        len += sprintf(page + len,         "    ig_fast_rt_uni_udp_pkts: %lu\n", ITF_MIB_TBL(i)->ig_fast_rt_uni_udp_pkts);
        len += sprintf(page + len,         "    ig_fast_rt_uni_tcp_pkts: %lu\n", ITF_MIB_TBL(i)->ig_fast_rt_uni_tcp_pkts);
        len += sprintf(page + len,         "    ig_fast_rt_mul_pkts:     %lu\n", ITF_MIB_TBL(i)->ig_fast_rt_mul_pkts);
        len += sprintf(page + len,         "    ig_fast_brg_bytes:       %lu\n", ITF_MIB_TBL(i)->ig_fast_brg_bytes);
        len += sprintf(page + len,         "    ig_fast_rt_bytes:        %lu\n", ITF_MIB_TBL(i)->ig_fast_rt_bytes);
        len += sprintf(page + len,         "    ig_cpu_pkts:             %lu\n", ITF_MIB_TBL(i)->ig_cpu_pkts);
        len += sprintf(page + len,         "    ig_cpu_bytes:            %lu\n", ITF_MIB_TBL(i)->ig_cpu_bytes);
        len += sprintf(page + len,         "    ig_drop_pkts:            %lu\n", ITF_MIB_TBL(i)->ig_drop_pkts);
        len += sprintf(page + len,         "    ig_drop_bytes:           %lu\n", ITF_MIB_TBL(i)->ig_drop_bytes);
        len += sprintf(page + len,         "    eg_fast_pkts:            %lu\n", ITF_MIB_TBL(i)->eg_fast_pkts);
        len += sprintf(page + len,         "    eg_fast_bytes:           %lu\n", ITF_MIB_TBL(i)->eg_fast_bytes);
        len += sprintf(page + len,         "  Driver\n");
        len += sprintf(page + len,         "    rx_packets: %u\n", priv->rx_packets);
        len += sprintf(page + len,         "    rx_bytes:   %u\n", priv->rx_bytes);
        len += sprintf(page + len,         "    rx_errors:  %u\n", priv->rx_errors);
        len += sprintf(page + len,         "    rx_dropped: %u\n", priv->rx_dropped);
        len += sprintf(page + len,         "    tx_packets: %u\n", priv->tx_packets);
        len += sprintf(page + len,         "    tx_bytes:   %u\n", priv->tx_bytes);
        len += sprintf(page + len,         "    tx_errors:  %u\n", priv->tx_errors);
        len += sprintf(page + len,         "    tx_dropped: %u\n", priv->tx_dropped);
        len += sprintf(page + len,         "  Total\n");
        len += sprintf(page + len,         "    rx_packets: %lu\n", priv->stats.rx_packets);
        len += sprintf(page + len,         "    rx_bytes:   %lu\n", priv->stats.rx_bytes);
        len += sprintf(page + len,         "    rx_errors:  %lu\n", priv->stats.rx_errors);
        len += sprintf(page + len,         "    rx_dropped: %lu\n", priv->stats.rx_dropped);
        len += sprintf(page + len,         "    tx_packets: %lu\n", priv->stats.tx_packets);
        len += sprintf(page + len,         "    tx_bytes:   %lu\n", priv->stats.tx_bytes);
        len += sprintf(page + len,         "    tx_errors:  %lu\n", priv->stats.tx_errors);
        len += sprintf(page + len,         "    tx_dropped: %lu\n", priv->stats.tx_dropped);
    }

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC
    len += sprintf(page + len, "QoS Mib:\n");
    len += sprintf(page + len, "    cputx_pkts:          %u\n",   *__CPU_TO_ETH1_TX_PKTS);
    len += sprintf(page + len, "    cputx_bytes:         %u\n",   *__CPU_TO_ETH1_TX_BYTES);
    len += sprintf(page + len, "    cputx_drop_pkts:     %u\n",   *__CPU_TX_SWAPPER_DROP_PKTS);
    len += sprintf(page + len, "    cputx_drop_bytess:   %u\n",   *__CPU_TX_SWAPPER_DROP_BYTES);

    len += sprintf(page + len, "    eth1_fp_pkts:        %lu\n",  ITF_MIB_TBL(1)->eg_fast_pkts );
    len += sprintf(page + len, "    eth1_fp_bytes:       %lu\n",  ITF_MIB_TBL(1)->eg_fast_bytes );
    len += sprintf(page + len, "    eth1_fp_drop_pkts:   %u\n",   *__ETH1_FP_SWAPPER_DROP_PKTS );
    len += sprintf(page + len, "    eth1_fp_drop_bytess: %u\n",   *__ETH1_FP_SWAPPER_DROP_BYTES );

    len += sprintf(page + len, "    eth1_tx_pkts:        %lu (%u, %u, %u, %u)\n",
                                                                        ETH1_TX_MIB_TBL->etx_total_pdu,
                                                                        * __ETH1_TX_CH0_TOTAL_PDU,
                                                                        * __ETH1_TX_CH1_TOTAL_PDU,
                                                                        * __ETH1_TX_CH2_TOTAL_PDU,
                                                                        * __ETH1_TX_CH3_TOTAL_PDU);

    len += sprintf(page + len, "    eth1_tx_bytes:       %lu (%u, %u, %u, %u)\n",
                                                                        ETH1_TX_MIB_TBL->etx_total_bytes,
                                                                        * __ETH1_TX_CH0_TOTAL_BYTES,
                                                                        * __ETH1_TX_CH1_TOTAL_BYTES,
                                                                        * __ETH1_TX_CH2_TOTAL_BYTES,
                                                                        * __ETH1_TX_CH3_TOTAL_BYTES );

    len += sprintf(page + len, "    eth1_tx_qf_drop_pkts:%u (%u, %u, %u, %u)\n",
                                                                        *__ETH1_TX_THRES_DROP_PKT_CNT0 +  *__ETH1_TX_THRES_DROP_PKT_CNT1 +  *__ETH1_TX_THRES_DROP_PKT_CNT2 +  *__ETH1_TX_THRES_DROP_PKT_CNT3,
                                                                        * __ETH1_TX_THRES_DROP_PKT_CNT0,
                                                                        * __ETH1_TX_THRES_DROP_PKT_CNT1,
                                                                        * __ETH1_TX_THRES_DROP_PKT_CNT2,
                                                                        * __ETH1_TX_THRES_DROP_PKT_CNT3);
#endif

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len - off;
}

static int proc_write_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[64];
    char *p;
    int len, rlen;
    u32 eth_clear;
    struct eth_priv_data *priv = NULL;

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
        eth_clear = 3;
    else if ( stricmp(p, "clear eth0") == 0 || stricmp(p, "clear 0") == 0
            || stricmp(p, "clean eth0") == 0 || stricmp(p, "clean 0") == 0 )
        eth_clear = 1;
    else if ( stricmp(p, "clear eth1") == 0 || stricmp(p, "clear 1") == 0
            || stricmp(p, "clean eth1") == 0 || stricmp(p, "clean 1") == 0 )
        eth_clear = 2;

    if ( (eth_clear & 1) )
    {
        eth_get_stats(g_eth_net_dev[0]);
        priv = netdev_priv(g_eth_net_dev[0]);
        memset(priv, 0, (u32)&priv->tx_dropped - (u32)priv + sizeof(u32));
        memset((void *)ITF_MIB_TBL(0), 0, sizeof(struct itf_mib));
    }
    if ( (eth_clear & 2) )
    {
        eth_get_stats(g_eth_net_dev[1]);
        priv = netdev_priv(g_eth_net_dev[1]);
        memset(priv, 0, (u32)&priv->tx_dropped - (u32)(priv) + sizeof(u32));
        memset((void *)ITF_MIB_TBL(1), 0, sizeof(struct itf_mib));
    }

#if 0
    ETH1_TX_MIB_TBL->etx_total_pdu   = 0;
    ETH1_TX_MIB_TBL->etx_total_bytes = 0;

    * __CPU_TX_SWAPPER_DROP_PKTS    = 0;
    * __CPU_TX_SWAPPER_DROP_BYTES   = 0;
    * __ETH1_FP_SWAPPER_DROP_PKTS   = 0;
    * __ETH1_FP_SWAPPER_DROP_BYTES  = 0;

    * __CPU_TO_ETH1_TX_PKTS     = 0;
    * __CPU_TO_ETH1_TX_BYTES    = 0;

    * __CPU_TO_ETH1_TX_PKTS     = 0;
    * __CPU_TO_ETH1_TX_BYTES    = 0;

    * __ETH1_TX_CH0_TOTAL_PDU   = 0;
    * __ETH1_TX_CH0_TOTAL_BYTES = 0;

    * __ETH1_TX_CH1_TOTAL_PDU   = 0;
    * __ETH1_TX_CH1_TOTAL_BYTES = 0;

    * __ETH1_TX_CH2_TOTAL_PDU   = 0;
    * __ETH1_TX_CH2_TOTAL_BYTES = 0;

    * __ETH1_TX_CH3_TOTAL_PDU   = 0;
    * __ETH1_TX_CH3_TOTAL_BYTES = 0;

    * __ETH1_TX_THRES_DROP_PKT_CNT0 = 0;
    * __ETH1_TX_THRES_DROP_PKT_CNT1 = 0;
    * __ETH1_TX_THRES_DROP_PKT_CNT2 = 0;
    * __ETH1_TX_THRES_DROP_PKT_CNT3 = 0;
#endif

    MOD_DEC_USE_COUNT;

    return count;
}

#if defined(DEBUG_FIRMWARE_TABLES_PROC) && DEBUG_FIRMWARE_TABLES_PROC

static int proc_read_route(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    struct rout_forward_compare_tbl *pcompare;
    struct rout_forward_action_tbl *pwaction;
    struct rout_forward_action_tbl *plaction;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    __sync();

    pstr = *start = page;

    llen = sprintf(str, "Wan Routing Table\n");
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

    pcompare = (struct rout_forward_compare_tbl *)WAN_ROUT_FORWARD_COMPARE_TBL(0);
    pwaction = (struct rout_forward_action_tbl *)WAN_ROUT_FORWARD_ACTION_TBL(0);
    for ( i = 0; i < WAN_ROUT_TBL_CFG->wan_rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_wan_route(str, i, pcompare, pwaction);
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
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        pwaction++;
    }

    llen = sprintf(str, "Lan Routing Table\n");
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
        goto PROC_READ_ROUTE_OVERRUN_END;

    pcompare = (struct rout_forward_compare_tbl *)LAN_ROUT_FORWARD_COMPARE_TBL(0);
    plaction = (struct rout_forward_action_tbl *)LAN_ROUT_FORWARD_ACTION_TBL(0);
    for ( i = 0; i < LAN_ROUT_TBL_CFG->lan_rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_lan_route(str, i, pcompare, plaction);
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
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        plaction++;
    }

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_ROUTE_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
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
            for ( entry = 0; entry < max_entry; entry++, pu2 += 3 )
                if ( *pu2 == *pu1 && pu2[1] == pu1[1] && pu2[2] == pu1[2] )
                    break;
            if ( entry == max_entry )
            {
                pu2 = type == 1 ? (u32*)LAN_ROUT_FORWARD_COMPARE_TBL(0) : (u32*)WAN_ROUT_FORWARD_COMPARE_TBL(0);
                for ( entry = 0; entry < max_entry; entry++, pu2 += 3 )
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
            for ( tmp_entry = 0; tmp_entry < max_entry; tmp_entry++, pu2 += 3 )
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
    int len_max = off + count;
    char *pstr;
    char str[512];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen = sprintf(str, "Bridging Table\n");
    if ( len <= off && len + llen > off )
    {
        memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;

    paction = (struct brg_forward_tbl *)BRIDGING_FORWARD_TBL(0);
    for ( i = 0; i < BRG_TBL_CFG->brg_entry_num; i++ )
    {
        if ( !*((u32*)paction + 2) )
        {
            llen = print_bridge(str, i, paction);
            if ( len <= off && len + llen > off )
            {
                memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_BRIDGE_OVERRUN_END;
        }

        paction++;
    }

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_BRIDGE_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_bridge(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

static int proc_read_mc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    volatile struct wan_rout_multicast_tbl *paction;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Multicast Table\n");
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

    paction = WAN_ROUT_MULTICAST_TBL(0);
    for ( i = 0; i < WAN_ROUT_MC_NUM; i++ )
    {
        if ( paction->wan_dest_ip )
        {
            llen = print_mc(str, i, (struct wan_rout_multicast_tbl *)paction);
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
                goto PROC_READ_MC_OVERRUN_END;
        }

        paction++;
    }

    *eof = 1;

    return len - off;

PROC_READ_MC_OVERRUN_END:
    return len - llen - off;
}

static int proc_read_genconf(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen = 0;
    int i;
    unsigned long bit;
    char *ppst[2];

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen += sprintf(str,        "ETX1_DMACH_ON    (0x%08X): ", (u32)ETX1_DMACH_ON);
    for ( i = 0, bit = 1; i < 4; i++, bit <<= 1 )
        llen += sprintf(str + llen, " %d - %s", i + 1, (*ETX1_DMACH_ON & bit) ? "on " : "off");
    llen += sprintf(str + llen, "\n");

    switch ( ETH_PORTS_CFG->eth1_type )
    {
    case 0: ppst[0] = "LAN"; break;
    case 1: ppst[0] = "WAN"; break;
    case 2: ppst[0] = "Mix"; break;
    default: ppst[0] = "Unkown";
    }
    switch ( ETH_PORTS_CFG->eth0_type )
    {
    case 0: ppst[1] = "LAN"; break;
    case 1: ppst[1] = "WAN"; break;
    case 2: ppst[1] = "Mix"; break;
    default: ppst[1] = "Unkown";
    }
    llen += sprintf(str + llen, "ETH_PORTS_CFG    (0x%08X): WAN VLAN ID Hi - %d, WAN VLAN ID Lo - %d, ETH1 Type - %s, ETH0 Type - %s\n", (u32)ETH_PORTS_CFG, ETH_PORTS_CFG->wan_vlanid_hi, ETH_PORTS_CFG->wan_vlanid_lo, ppst[0], ppst[1]);

    llen += sprintf(str + llen, "LAN_ROUT_TBL_CFG (0x%08X): num - %d, off - %d, TCP/UDP ver - %s, IP ver - %s,\n"
                                "                               TCP/UDP err drop - %s, no hit drop - %s\n",
                                                            (u32)LAN_ROUT_TBL_CFG,
                                                            LAN_ROUT_TBL_CFG->lan_rout_num,
                                                            LAN_ROUT_TBL_CFG->lan_rout_off,
                                                            LAN_ROUT_TBL_CFG->lan_tcpudp_ver_en ? "on" : "off",
                                                            LAN_ROUT_TBL_CFG->lan_ip_ver_en ? "on" : "off",
                                                            LAN_ROUT_TBL_CFG->lan_tcpudp_err_drop ? "on" : "off",
                                                            LAN_ROUT_TBL_CFG->lan_rout_drop  ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "WAN_ROUT_TBL_CFG (0x%08X): num - %d, MC num - %d, MC drop - %s, TCP/UDP ver - %s,\n"
                                "                               IP ver - %s, TCP/UDP err drop - %s, no hit drop - %s\n",
                                                            (u32)WAN_ROUT_TBL_CFG,
                                                            WAN_ROUT_TBL_CFG->wan_rout_num,
                                                            WAN_ROUT_TBL_CFG->wan_rout_mc_num,
                                                            WAN_ROUT_TBL_CFG->wan_rout_mc_drop ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->wan_tcpdup_ver_en ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->wan_ip_ver_en ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->wan_tcpudp_err_drop ? "on" : "off",
                                                            WAN_ROUT_TBL_CFG->wan_rout_drop  ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "BRG_TBL_CFG      (0x%08X): num - %d, destine to src port - %s, dest vlan - %s\n"
                                "                               src vlan - %s, mac change drop - %s\n",
                                                            (u32)BRG_TBL_CFG,
                                                            BRG_TBL_CFG->brg_entry_num,
                                                            BRG_TBL_CFG->br_to_src_port_en ? "on" : "off",
                                                            BRG_TBL_CFG->dest_vlan_en ? "on" : "off",
                                                            BRG_TBL_CFG->src_vlan_en ? "on" : "off",
                                                            BRG_TBL_CFG->mac_change_drop ? "on" : "off"
                                                            );

    switch ( GEN_MODE_CFG->wan_acc_mode )
    {
    case 0: ppst[0] = "null"; break;
    case 2: ppst[0] = "routing"; break;
    default: ppst[0] = "unknown";
    }
    switch ( GEN_MODE_CFG->lan_acc_mode )
    {
    case 0: ppst[1] = "null"; break;
    case 2: ppst[1] = "routing"; break;
    default: ppst[1] = "unknown";
    }
    llen += sprintf(str + llen, "GEN_MODE_CFG     (0x%08X): CPU1 fast mode - %s, ETH1 fast mode - %s,\n"
                                "                               fast path wfq - %d, DPLUS wfq - %d, ETX wfq - %d,\n"
                                "                               WAN mode - %s, LAN mode - %s\n"
                                "                               ETH1 TX QoS - %s, DSCP CoS - %s\n",
                                                            (u32)GEN_MODE_CFG,
                                                            GEN_MODE_CFG->cpu1_fast_mode ? "direct" : "indirect",
                                                            GEN_MODE_CFG->wan_fast_mode ? "direct" : "indirect",
                                                            GEN_MODE_CFG->fast_path_wfq,
                                                            GEN_MODE_CFG->dplus_wfq,
                                                            GEN_MODE_CFG->etx_wfq,
                                                            ppst[0],
                                                            ppst[1],
                                                            GEN_MODE_CFG->us_early_discard_en ? "on" : "off",
                                                            GEN_MODE_CFG->dscp_qos_en ? "on" : "off"
                                                            );

    llen += sprintf(str + llen, "BRG_RT_ID_TBL     (0x%08X): eth0 - vlan %s, %d - %d, %d - %d\n"
                                "                                eth1 - vlan %s, %d - %d, %d - %d\n",
                                                            (u32)BRG_RT_ID_TBL(0),
                                                            BRG_RT_ID_TBL(0)->brg_vlanid_en ? "on " : "off",
                                                            BRG_RT_ID_TBL(0)->brg_vlanid_base1,
                                                            BRG_RT_ID_TBL(0)->brg_vlanid_base1 + BRG_RT_ID_TBL(0)->brg_vlanid_range1,
                                                            BRG_RT_ID_TBL(0)->brg_vlanid_base2,
                                                            BRG_RT_ID_TBL(0)->brg_vlanid_base2 + BRG_RT_ID_TBL(0)->brg_vlanid_range2,
                                                            BRG_RT_ID_TBL(1)->brg_vlanid_en ? "on " : "off",
                                                            BRG_RT_ID_TBL(1)->brg_vlanid_base1,
                                                            BRG_RT_ID_TBL(1)->brg_vlanid_base1 + BRG_RT_ID_TBL(1)->brg_vlanid_range1,
                                                            BRG_RT_ID_TBL(1)->brg_vlanid_base2,
                                                            BRG_RT_ID_TBL(1)->brg_vlanid_base2 + BRG_RT_ID_TBL(1)->brg_vlanid_range2
                                                            );

    llen += sprintf(str + llen, "CFG_ROUT_MAC_NO            (0x%08X): 0x%08X\n", (u32)CFG_ROUT_MAC_NO, (u32)*CFG_ROUT_MAC_NO);
    llen += sprintf(str + llen, "IP_TCPUDP_VERIFICATION_ENABLED (0x%08X): 0x%08X\n", (u32)IP_TCPUDP_VERIFICATION_ENABLED, (u32)*IP_TCPUDP_VERIFICATION_ENABLED);

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
        goto PROC_READ_GENCONF_OVERRUN_END;

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_GENCONF_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_genconf(struct file *file, const char *buf, unsigned long count, void *data)
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

    if ( stricmp(p, "tcpver on") == 0 || stricmp(p, "tcpver enable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_tcpudp_ver_en = 1;
        WAN_ROUT_TBL_CFG->wan_tcpdup_ver_en = 1;
    }
    else if ( stricmp(p, "tcpver off") == 0 || stricmp(p, "tcpver disable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_tcpudp_ver_en = 0;
        WAN_ROUT_TBL_CFG->wan_tcpdup_ver_en = 0;
    }
    else if ( stricmp(p, "ipver on") == 0 || stricmp(p, "ipver enable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_ip_ver_en = 1;
        WAN_ROUT_TBL_CFG->wan_ip_ver_en = 1;
    }
    else if ( stricmp(p, "ipver off") == 0 || stricmp(p, "ipver disable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_ip_ver_en = 0;
        WAN_ROUT_TBL_CFG->wan_ip_ver_en = 0;
    }
    else if ( stricmp(p, "errdrop on") == 0 || stricmp(p, "errdrop enable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_tcpudp_err_drop = 1;
        WAN_ROUT_TBL_CFG->wan_tcpudp_err_drop = 1;
    }
    else if ( stricmp(p, "errdrop off") == 0 || stricmp(p, "errdrop disable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_tcpudp_err_drop = 0;
        WAN_ROUT_TBL_CFG->wan_tcpudp_err_drop = 0;
    }
    else if ( stricmp(p, "nohitdrop on") == 0 || stricmp(p, "nohitdrop enable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_rout_drop = 1;
        WAN_ROUT_TBL_CFG->wan_rout_drop = 1;
    }
    else if ( stricmp(p, "nohitdrop off") == 0 || stricmp(p, "nohitdrop disable") == 0 )
    {
        LAN_ROUT_TBL_CFG->lan_rout_drop = 0;
        WAN_ROUT_TBL_CFG->wan_rout_drop = 0;
    }

    MOD_DEC_USE_COUNT;

    return count;
}

static int proc_read_pppoe(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[512];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen = sprintf(pstr, "PPPoE Table (0x%08X) - Session ID:\n", (u32)PPPOE_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *PPPOE_CFG_TBL(i));
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
            goto PROC_READ_PPPOE_OVERRUN_END;
    }

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_PPPOE_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_pppoe(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

static int proc_read_mtu(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[512];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen = sprintf(pstr, "MTU Table (0x%08X):\n", (u32)MTU_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *MTU_CFG_TBL(i));
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
            goto PROC_READ_MTU_OVERRUN_END;
    }

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_MTU_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_mtu(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

static int proc_read_hit(char *page, char **start, off_t off, int count, int *eof, void *data)
{
   int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[1024];
    int llen;
    int i;
    int n;
    unsigned long bit;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen = sprintf(pstr, "Unicast Hit Table:\n");
    pstr += llen;
    len += llen;

#if 0
    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %3d - %3d:", i * 32 + 1, (i + 1) * 32);

        for ( bit = 1; bit; bit <<= 1 )
            llen += sprintf(str + llen, " %d", (*ROUT_FWD_HIT_STAT_TBL(i) & bit) ? 1 : 0);

        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }
#else
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
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }

#endif

    llen = sprintf(str, "Multicast Hit Table (0x%08X):\n", (u32)MC_ROUT_FWD_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
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
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }

    llen = sprintf(str, "Bridge Hit Table (0x%08X):\n", (u32)BRG_FWD_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
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
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_HIT_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_hit(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[64];
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
    int len_max = off + count;
    char *pstr;
    char str[512];
    int llen;
    int i;
    unsigned int mac52, mac10;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen = sprintf(pstr, "MAC Table:\n");
    pstr += llen;
    len += llen;

#if 0

    llen = sprintf(pstr, "  WAN (0x%08X)\n", (u32)WAN_ROUT_MAC_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        mac52 = *WAN_ROUT_MAC_CFG_TBL(i);
        mac10 = *(WAN_ROUT_MAC_CFG_TBL(i) + 1);

        llen = sprintf(str, "    %d: %02X:%02X:%02X:%02X:%02X:%02X\n", i, mac52 >> 24, (mac52 >> 16) & 0xFF, (mac52 >> 8) & 0xFF, mac52 & 0xFF, mac10 >> 24, (mac10 >> 16) & 0xFF);
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
            goto PROC_READ_MAC_OVERRUN_END;
    }

    llen = sprintf(str, "  LAN (0x%08X)\n", (u32)LAN_ROUT_MAC_CFG_TBL(0));
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
        goto PROC_READ_MAC_OVERRUN_END;

    for ( i = 0; i < 8; i++ )
    {
        mac52 = *LAN_ROUT_MAC_CFG_TBL(i);
        mac10 = *(LAN_ROUT_MAC_CFG_TBL(i) + 1);

        llen = sprintf(str, "    %d: %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1, mac52 >> 24, (mac52 >> 16) & 0xFF, (mac52 >> 8) & 0xFF, mac52 & 0xFF, mac10 >> 24, (mac10 >> 16) & 0xFF);
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
            goto PROC_READ_MAC_OVERRUN_END;
    }

#else

    llen = sprintf(pstr, "  CFG_ROUT_MAC_NO(0x%08X): %d\n", (u32)CFG_ROUT_MAC_NO, (u32)*CFG_ROUT_MAC_NO);
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  ROUTER_MAC_CFG_TBL (0x%08X)\n", (u32)ROUTER_MAC_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 16; i++ )
    {
        mac52 = *ROUTER_MAC_CFG_TBL(i);
        mac10 = *(ROUTER_MAC_CFG_TBL(i) + 1);

        llen = sprintf(str, "    %2d: %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1, mac52 >> 24, (mac52 >> 16) & 0xFF, (mac52 >> 8) & 0xFF, mac52 & 0xFF, mac10 >> 24, (mac10 >> 16) & 0xFF);
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
            goto PROC_READ_MAC_OVERRUN_END;
    }

#endif

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_mac(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

static int proc_read_vlan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *etag_en_str[] = {
        "unmodified",
        "remove",
        "insert",
        "replace"
    };

    struct brg_vlan_cfg itf_cfg;
    volatile struct brg_vlan_cfg *p_itf_cfg;
    struct default_itf_vlan_map def_map;
    volatile struct default_itf_vlan_map *p_def_map;
    struct src_ip_vlan_map ip_map;
    volatile struct src_ip_vlan_map *p_ip_map;
    struct ethtype_vlan_map ethtype_map;
    volatile struct ethtype_vlan_map *p_ethtype_map;
    struct vlan_vlan_map vlan_map;
    volatile struct vlan_vlan_map *p_vlan_map;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[512];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

    llen = sprintf(str, "Interface VLAN Config:\n");
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
        goto PROC_READ_VLAN_OVERRUN_END;

    p_itf_cfg = BRG_VLAN_CFG(0);
    for ( i = 0; i < IFX_PPA_PORT_NUM; i++, p_itf_cfg++ )
    {
        itf_cfg = *p_itf_cfg;

        llen = sprintf(str,         "  %d (0x%08X):\n", i, (u32)p_itf_cfg);
        llen += sprintf(str + llen, "    egress inner vlan - %s, src ip - %s, ethtype - %s, vlan tag - %s\n",
                                         etag_en_str[itf_cfg.in_etag_ctrl],
                                         itf_cfg.src_ip_vlan_en ? " on" : "off",
                                         itf_cfg.ethtype_vlan_en ? " on" : "off",
                                         itf_cfg.vlan_tag_vlan_en ? " on" : "off");
        llen += sprintf(str + llen, "    loopback - %s, vlan aware - %s, mac change drop - %s\n",
                                         itf_cfg.loop_ctl ? "enable" : "disable",
                                         itf_cfg.vlan_en ? " on" : "off",
                                         itf_cfg.mac_change_drop ? " drop" : "forward");
        llen += sprintf(str + llen, "    port en - %s, ingress outer vlan - %s, egress outer vlan - %s\n",
                                         itf_cfg.port_en ? "enable" : "disable",
                                         itf_cfg.out_itag_vld ? " enable" : "disable",
                                         etag_en_str[itf_cfg.out_etag_ctrl]);
        llen += sprintf(str + llen, "    unknown multicast - 0x%02X, unknown uni-cast - 0x%02X\n",
                                         itf_cfg.unknown_mc_dest_list,
                                         itf_cfg.unknown_uc_dest_list);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_def_map = DEFAULT_ITF_VLAN_MAP(0);
    for ( i = 0; i < MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES; i++, p_def_map++ )
    {
        def_map = *p_def_map;

        llen = sprintf(str, "  %d (0x%08X): inner vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_def_map, def_map.new_in_vci, def_map.vlan_member, def_map.dest_qos, def_map.outer_vlan_ix);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_ip_map = SRC_IP_VLAN_MAP(0);
    for ( i = 0; i < MAX_SRC_IP_VLAN_MAP_ENTRIES; i++, p_ip_map++ )
    {
        ip_map = *p_ip_map;

        llen = sprintf(str, "  %d (0x%08X): src ip - %d.%d.%d.%d, vci - 0x%04X, map - 0x%02X, outer vlan index - %d, dest qos - %d\n", i, (u32)p_ip_map, ip_map.src_ip >> 24, (ip_map.src_ip >> 16) & 0xFF, (ip_map.src_ip >> 8) & 0xFF, ip_map.src_ip & 0xFF, ip_map.new_in_vci, ip_map.vlan_member, ip_map.dest_qos, ip_map.outer_vlan_ix);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_ethtype_map = ETHTYPE_VLAN_MAP(0);
    for ( i = 0; i < MAX_ETHTYPE_VLAN_MAP_ENTRIES; i++, p_ethtype_map++ )
    {
        ethtype_map = *p_ethtype_map;

        llen = sprintf(str, "  %d (0x%08X): ethtype - 0x%04X, vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_ethtype_map, ethtype_map.ethtype, ethtype_map.new_in_vci, ethtype_map.vlan_member, ethtype_map.dest_qos, ethtype_map.outer_vlan_ix);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_vlan_map = VLAN_VLAN_MAP(0);
    for ( i = 0; i < MAX_VLAN_VLAN_MAP_ENTRIES; i++, p_vlan_map++ )
    {
        vlan_map = *p_vlan_map;

        llen = sprintf(str, "  %d (0x%08X): vlan tag - 0x%04X, vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_vlan_map, vlan_map.in_vlan_tag, vlan_map.new_in_vci, vlan_map.vlan_member, vlan_map.dest_qos, vlan_map.outer_vlan_ix);
        llen += sprintf(str + llen, "                  in/out etag ctrl - 0x%08x\n", vlan_map.in_out_etag_ctrl);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_VLAN_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_vlan(struct file *file, const char *buf, unsigned long count, void *data)
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

    MOD_DEC_USE_COUNT;

    return count;
}

static int proc_read_out_vlan(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "Outer VLAN Table (0x%08X):\n", (u32)OUTER_VLAN_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 32; i++ )
    {
        llen = sprintf(str, "  %d: 0x%08X\n", i, *OUTER_VLAN_TBL(i));
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
            goto PROC_READ_MAC_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_out_vlan(struct file *file, const char *buf, unsigned long count, void *data)
{
    return count;
}

#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC

static unsigned int etop_cfg;
static unsigned int dplus_txcfg;
static unsigned int dplus_rxcfg;
static unsigned int dplus_txpgcnt;
static unsigned int dplus_rxpgcnt;

static unsigned int enets_pgcnt[2];
static unsigned int enets_pktcnt[2];
static unsigned int enetf_pgcnt[2];
static unsigned int enetf_pktcnt[2];

static unsigned int enets_dba[2];
static unsigned int enets_cba[2];
static unsigned int enetf_dba[2];
static unsigned int enetf_cba[2];

static unsigned int enets_cfg[2];
static unsigned int enetf_cfg[2];

static unsigned int enet_mac_cfg[2];

static void switch_to_test_mode(void)
{
    int i;

    // 1
    etop_cfg    = * ETOP_CFG;
    * ETOP_CFG    = 0x12;

    for (i = 0 ; i < 100000; ++i);

    // 2
    dplus_txcfg = * DPLUS_TXCFG;
    * DPLUS_TXCFG = 0X700D;

    // 3
    dplus_rxcfg = * DPLUS_RXCFG;
    * DPLUS_RXCFG = 0X504B;

    // 4 stop PP32
    *PP32_DBG_CTRL = DBG_CTRL_STOP_SET(1);

    // 5, 6
    dplus_txpgcnt  = * DPLUS_TXPGCNT;
    dplus_rxpgcnt  = * DPLUS_RXPGCNT;

    * DPLUS_TXPGCNT = 0x40000;
    * DPLUS_RXPGCNT = 0x40000;

    // 7
    enets_pgcnt[0]  = * ENETS_PGCNT(0);
    enets_pgcnt[1]  = * ENETS_PGCNT(1);
    enets_pktcnt[0] = * ENETS_PKTCNT(0);
    enets_pktcnt[1] = * ENETS_PKTCNT(1);

    enetf_pgcnt[0]  = * ENETF_PGCNT(0);
    enetf_pgcnt[1]  = * ENETF_PGCNT(1);
    enetf_pktcnt[0] = * ENETF_PKTCNT(0);
    enetf_pktcnt[1] = * ENETF_PKTCNT(1);

    * ENETS_PGCNT(0)  = 0x60000;
    * ENETS_PGCNT(1)  = 0x60000;
    * ENETS_PKTCNT(0) = 0x600;
    * ENETS_PKTCNT(1) = 0x600;

    * ENETF_PGCNT(0)  = 0x60000;
    * ENETF_PGCNT(1)  = 0x60000;
    * ENETF_PKTCNT(0) = 0x600;
    * ENETF_PKTCNT(1) = 0x600;

    // 8
    enets_dba[0]  = * ENETS_DBA(0);
    enets_dba[1]  = * ENETS_DBA(1);
    enets_cba[0]  = * ENETS_CBA(0);
    enets_cba[1]  = * ENETS_CBA(1);

    enetf_dba[0]  = * ENETF_DBA(0);
    enetf_dba[1]  = * ENETF_DBA(1);
    enetf_cba[0]  = * ENETF_CBA(0);
    enetf_cba[1]  = * ENETF_CBA(1);

    * ENETS_DBA(1) = * ENETF_DBA(0) = 0x950;
    * ENETS_DBA(0) = * ENETF_DBA(1) = 0x750;

    * ENETS_CBA(1) = * ENETF_CBA(0) = 0xB50;
    * ENETS_CBA(0) = * ENETF_CBA(1) = 0xB70;

    // 9
    enets_cfg[0] = * ENETS_CFG(0);
    enets_cfg[1] = * ENETS_CFG(1);
    enetf_cfg[0] = * ENETF_CFG(0);
    enetf_cfg[1] = * ENETF_CFG(1);

    * ENETS_CFG(0) = 0X5020;
    * ENETS_CFG(1) = 0X5020;
    * ENETF_CFG(0) = 0X5020;
    * ENETF_CFG(1) = 0X5020;


    // 10
    enet_mac_cfg[0] = * ENET_MAC_CFG(0);
    enet_mac_cfg[1] = * ENET_MAC_CFG(1);

    * ENET_MAC_CFG(0) = 7;
    * ENET_MAC_CFG(1) = 7;

    // 11
    * ETOP_CFG    = 0x3D2;
}

static void switch_to_work_mode(void)
{
    int i;

    // 1
    * ETOP_CFG    = 0x12;

    for (i = 0 ; i < 100000; ++i);

    // 5, 6
    * DPLUS_TXPGCNT = dplus_txpgcnt;
    * DPLUS_RXPGCNT = dplus_rxpgcnt;


    // 7
    * ENETS_PGCNT(0)  = enets_pgcnt[0] ;
    * ENETS_PGCNT(1)  = enets_pgcnt[1] ;
    * ENETS_PKTCNT(0) = enets_pktcnt[0];
    * ENETS_PKTCNT(1) = enets_pktcnt[1];

    * ENETF_PGCNT(0)  = enetf_pgcnt[0] ;
    * ENETF_PGCNT(1)  = enetf_pgcnt[1] ;
    * ENETF_PKTCNT(0) = enetf_pktcnt[0];
    * ENETF_PKTCNT(1) = enetf_pktcnt[1];

    // 8
    * ENETS_DBA(0)  = enets_dba[0];
    * ENETS_DBA(1)  = enets_dba[1];
    * ENETS_CBA(0)  = enets_cba[0];
    * ENETS_CBA(1)  = enets_cba[1];

    * ENETF_DBA(0)  = enetf_dba[0];
    * ENETF_DBA(1)  = enetf_dba[1];
    * ENETF_CBA(0)  = enetf_cba[0];
    * ENETF_CBA(1)  = enetf_cba[1];

    // 9
    * ENETS_CFG(0) = enets_cfg[0];
    * ENETS_CFG(1) = enets_cfg[1];
    * ENETF_CFG(0) = enetf_cfg[0];
    * ENETF_CFG(1) = enetf_cfg[1];

    // 10
    * ENET_MAC_CFG(0) = enet_mac_cfg[0];
    * ENET_MAC_CFG(1) = enet_mac_cfg[1];

    // 11

    // 2
    * DPLUS_TXCFG = dplus_txcfg ;
    * DPLUS_RXCFG = dplus_rxcfg;

    * ETOP_CFG = etop_cfg;

    for (i = 0 ; i < 100000; ++i);

    *PP32_DBG_CTRL = DBG_CTRL_START_SET(1);
}

static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + len, "error print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ERR)              ? "enabled" : "disabled");
    len += sprintf(page + len, "debug print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT)      ? "enabled" : "disabled");
    len += sprintf(page + len, "assert           - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ASSERT)           ? "enabled" : "disabled");
    len += sprintf(page + len, "dump rx skb      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_RX)      ? "enabled" : "disabled");
    len += sprintf(page + len, "dump tx skb      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_TX)      ? "enabled" : "disabled");
    len += sprintf(page + len, "dump flag header - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_FLAG_HEADER) ? "enabled" : "disabled");
    len += sprintf(page + len, "dump init        - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DUMP_INIT)        ? "enabled" : "disabled");

    *eof = 1;

    return len - off;
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
        printk("echo <enable/disable> [err/dbg/assert/rx/tx/header/init/all] > /proc/eth/dbg\n");
    }
    else if ( stricmp(str, "test_mode") == 0 ) {
        if (! is_test_mode) {
            switch_to_test_mode();
            is_test_mode = 1;
        }
    }
    else if ( stricmp(str, "work_mode") == 0 ) {
        if (is_test_mode) {
            switch_to_work_mode();
            is_test_mode = 0;
        }
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

static int proc_read_prio(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i, j;

    MOD_INC_USE_COUNT;

    len += sprintf(page + len,        "Priority to Queue Map:\n");
    len += sprintf(page + len,        "  prio     :  0  1  2  3  4  5  6  7\n");
    for ( i = 0; i < 2; i++ )
    {
        len += sprintf(page + len,    "  eth%d     :", i);
        for ( j = 0; j < NUM_ENTITY(g_eth_prio_queue_map[i]); j++ )
            len += sprintf(page + len,"  %d", g_eth_prio_queue_map[i][j]);
        len += sprintf(page + len,    "\n");
    }

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len - off;
}

static int proc_write_prio(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 1;
    unsigned int port = ~0;
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
            printk("echo <eth0/eth1> prio xx queue xx [prio xx queue xx] > /proc/eth/prio\n");
            break;
        }
        else if ( stricmp(p1, "eth0") == 0 )
        {
            port = 0;
            prio = queue = -1;
            dbg("port = 0");
        }
        else if ( stricmp(p1, "eth1") == 0 )
        {
            port = 1;
            prio = queue = -1;
            dbg("port = 1");
        }
        else if ( stricmp(p1, "pvc") == 0 )
        {
            err("atm is not valid");
            prio = queue = -1;
        }
        else if ( port != ~0 )
        {
            if ( stricmp(p1, "p") == 0 || stricmp(p1, "prio") == 0 )
            {
                ignore_space(&p2, &len);
                prio = get_number(&p2, &len, 0);
                dbg("prio = %d", prio);
                if ( port >= 0 && port <= 1 && prio >= 0 && prio < NUM_ENTITY(g_eth_prio_queue_map[port]) )
                {
                    if ( queue >= 0 )
                        g_eth_prio_queue_map[port][prio] = queue;
                }
                else
                {
                    err("prio (%d) is out of range 0 - %d", prio, NUM_ENTITY(g_eth_prio_queue_map[port]) - 1);
                }
            }
            else if ( stricmp(p1, "q") == 0 || stricmp(p1, "queue") == 0 )
            {
                ignore_space(&p2, &len);
                queue = get_number(&p2, &len, 0);
                dbg("queue = %d", queue);
                if ( port >= 0 && port <= 1 && queue >= 0 && queue <= 3 )
                {
                    if ( prio >= 0 )
                        g_eth_prio_queue_map[port][prio] = queue;
                }
                else
                {
                    err("queue (%d) is out of range 0 - 3", queue);
                }
            }
            else
            {
                err("unknown command (%s)", p1);
            }
        }
        else
        {
            err("unknown command (%s)", p1);
        }

        p1 = p2;
        colon = 1;
    }

    return count;
}

#if defined(DEBUG_FW_PROC) && DEBUG_FW_PROC

#define ACC_ERX_PID             PP32_DATA_MEMORY_RAM1_ADDR(0x0080)
#define ACC_ERX_PORT_TIMES      PP32_DATA_MEMORY_RAM1_ADDR(0x0081)
#define SLL_ISSUED              PP32_DATA_MEMORY_RAM1_ADDR(0x0082)
#define BMC_ISSUED              PP32_DATA_MEMORY_RAM1_ADDR(0x0083)

#define PRESEARCH_RDPTR         PP32_DATA_MEMORY_RAM1_ADDR(0x0088)

#define SLL_ERX_PID             PP32_DATA_MEMORY_RAM1_ADDR(0x0084)
#define SLL_PKT_CNT             PP32_DATA_MEMORY_RAM1_ADDR(0x0090)
#define SLL_RDPTR               PP32_DATA_MEMORY_RAM1_ADDR(0x0094)


#define EDIT_PKT_CNT            PP32_DATA_MEMORY_RAM1_ADDR(0x0098)
#define EDIT_RDPTR              PP32_DATA_MEMORY_RAM1_ADDR(0x009C)

#define DPLUSRX_PKT_CNT         PP32_DATA_MEMORY_RAM1_ADDR(0x00A0)
#define DPLUS_RDPTR             PP32_DATA_MEMORY_RAM1_ADDR(0x00A4)

#define SLL_STATE_NULL      0
#define SLL_STATE_DA        1
#define SLL_STATE_SA        2
#define SLL_STATE_DA_BC     3
#define SLL_STATE_ROUTER    4

static int proc_read_fw(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    MOD_INC_USE_COUNT;

    len += sprintf(page + len, "Firmware\n");
    len += sprintf(page + len, "  ACC_ERX_PID        = %08X\n", *ACC_ERX_PID);
    len += sprintf(page + len, "  ACC_ERX_PORT_TIMES = %08X\n", *ACC_ERX_PORT_TIMES);
    len += sprintf(page + len, "  SLL_ISSUED         = %08X [%s]\n", *SLL_ISSUED,
                        *SLL_ISSUED == SLL_STATE_NULL   ? "NULL"   :
                        *SLL_ISSUED == SLL_STATE_DA     ? "BRG_DA" :
                        *SLL_ISSUED == SLL_STATE_SA     ? "BRG_SA" :
                        *SLL_ISSUED == SLL_STATE_ROUTER ? "ROUT" :
                                                          "Error!");
    len += sprintf(page + len, "  BMC_ISSUED         = %08X\n", *BMC_ISSUED);

    len += sprintf(page + len, "  PRESEARCH_RDPTR    = %08X, %08X, %08X, %08X\n",
            *PRESEARCH_RDPTR, *(PRESEARCH_RDPTR + 1), * (PRESEARCH_RDPTR + 2), * (PRESEARCH_RDPTR + 3));

    len += sprintf(page + len, "  SLL_ERX_PID        = %08X\n", *SLL_ERX_PID);

    len += sprintf(page + len, "  SLL_PKT_CNT        = %08X, %08X, %08X, %08X\n",
            *SLL_PKT_CNT, *(SLL_PKT_CNT + 1), * (SLL_PKT_CNT + 2), * (SLL_PKT_CNT + 3));

    len += sprintf(page + len, "  SLL_RDPTR          = %08X, %08X, %08X, %08X\n",
            *SLL_RDPTR, *(SLL_RDPTR + 1), * (SLL_RDPTR + 2), * (SLL_RDPTR + 3));


    len += sprintf(page + len, "  EDIT_PKT_CNT       = %08X, %08X, %08X, %08X\n",
            *EDIT_PKT_CNT, *(EDIT_PKT_CNT + 1), * (EDIT_PKT_CNT + 2), * (EDIT_PKT_CNT + 3));
    len += sprintf(page + len, "  EDIT_RDPTR         = %08X, %08X, %08X, %08X\n",
            *EDIT_RDPTR, *(EDIT_RDPTR + 1), * (EDIT_RDPTR + 2), * (EDIT_RDPTR + 3));

    len += sprintf(page + len, "  DPLUSRX_PKT_CNT    = %08X, %08X, %08X, %08X\n",
            *DPLUSRX_PKT_CNT, *(DPLUSRX_PKT_CNT + 1), * (DPLUSRX_PKT_CNT + 2), * (DPLUSRX_PKT_CNT + 3));
    len += sprintf(page + len, "  DPLUS_RDPTR        = %08X, %08X, %08X, %08X\n",
            *DPLUS_RDPTR, *(DPLUS_RDPTR + 1), * (DPLUS_RDPTR + 2), * (DPLUS_RDPTR + 3));

    //len += sprintf(page + len, "  POSTSEARCH_RDPTR   = %08X, %08X\n",

  #if defined(ENABLE_FW_TX_QOS) && ENABLE_FW_TX_QOS
    len += sprintf(page + len, " QoS");
    len += sprintf(page + len, "  ETH1_TX_SWAP_RDPTR = %08X, %08X, %08X, %08X\n",
                                            * __ETH1_TXDES_SWAP_PTR0, * __ETH1_TXDES_SWAP_PTR1,
                                            * __ETH1_TXDES_SWAP_PTR2, * __ETH1_TXDES_SWAP_PTR3);

    len += sprintf(page + len, "  ETH1_TX_PKT_CNT    = %08X, %08X, %08X, %08X\n",
                                            * __ETH1_TX_PKT_CNT0, * __ETH1_TX_PKT_CNT1,
                                            * __ETH1_TX_PKT_CNT2, * __ETH1_TX_PKT_CNT3);

    len += sprintf(page + len, "  QOSD_DPLUS_RDPTR   = %08X\n",
                                            * __ETH1_FP_RXDES_SWAP_RDPTR);

    len += sprintf(page + len, "  QOSD_CPUTX_RDPTR   = %08X\n",
                                            * __CPU_TXDES_SWAP_RDPTR);

    len += sprintf(page + len, "  DPLUS_RXDES_RDPTR  = %08X\n",
                                            * __ETH1_FP_RXDES_DPLUS_WRPTR);
  #endif

    MOD_DEC_USE_COUNT;

    *eof = 1;

    return len - off;
}
#endif

#define PP32_REG_ADDR_BEGIN     0x0
#define PP32_REG_ADDR_END       0x1FFF
#define PP32_SB_ADDR_BEGIN      0x2000
#define PP32_SB_ADDR_END        0xFFFF

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
                char str[32] = {0};
                char *pch = str;
                int k;
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
                        printk(" %08X", (u32)*p);
                        for ( k = 0; k < 4; k++ )
                        {
                            c = ((char*)p)[k];
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

    MOD_DEC_USE_COUNT;

    return count;
}
#endif

#if defined(DEBUG_DMA_PROC) && DEBUG_DMA_PROC
static int proc_read_dma(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[512];
    int llen;
    int i;

    MOD_INC_USE_COUNT;

    pstr = *start = page;

  #if defined(DEBUG_DMA_PROC_UPSTREAM) && DEBUG_DMA_PROC_UPSTREAM
    if ( (dbg_dma_enable & 0x01) )
    {
        llen = sprintf(str, "DMA (eth0 -> eth1)\n");
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
            goto PROC_READ_DMA_OVERRUN_END;

        llen = sprintf(str, "  rx\n");
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
            goto PROC_READ_DMA_OVERRUN_END;

        for ( i = 0; i < DMA_RX_CH1_DESC_NUM; i++ )
        {
            llen = print_dma_desc(str, 1, i, 1);
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
                goto PROC_READ_DMA_OVERRUN_END;
        }

        llen = sprintf(str, "  tx\n");
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
            goto PROC_READ_DMA_OVERRUN_END;

        for ( i = 0; i < ETH1_TX_DESC_NUM; i++ )
        {
            llen = print_ema_desc(str, 1, i, 0);
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
                goto PROC_READ_DMA_OVERRUN_END;
        }
    }
  #endif

  #if defined(DEBUG_DMA_PROC_DOWNSTREAM) && DEBUG_DMA_PROC_DOWNSTREAM
    if ( (dbg_dma_enable & 0x02) )
    {
        llen = sprintf(str, "DMA (eth1 -> eth0)\n");
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
            goto PROC_READ_DMA_OVERRUN_END;

        llen = sprintf(str, "  rx\n");
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
            goto PROC_READ_DMA_OVERRUN_END;

        for ( i = 0; i < DMA_RX_CH2_DESC_NUM; i++ )
        {
            llen = print_dma_desc(str, 2, i, 1);
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
                goto PROC_READ_DMA_OVERRUN_END;
        }

        llen = sprintf(str, "  tx\n");
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
            goto PROC_READ_DMA_OVERRUN_END;

        for ( i = 0; i < DMA_TX_CH2_DESC_NUM; i++ )
        {
            llen = print_dma_desc(str, 2, i, 0);
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
                goto PROC_READ_DMA_OVERRUN_END;
        }
    }
  #endif

    *eof = 1;

    MOD_DEC_USE_COUNT;

    return len - off;

PROC_READ_DMA_OVERRUN_END:
    MOD_DEC_USE_COUNT;

    return len - llen - off;
}

static int proc_write_dma(struct file *file, const char *buf, unsigned long count, void *data)
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
    {
        if ( !dbg_dma_enable )
            dbg_dma_enable = 0x03;
    }
    else if ( stricmp(str, "disable") == 0 )
        dbg_dma_enable = 0x00;
    else if ( stricmp(str, "enable up") == 0 || stricmp(str, "enable upstream") == 0 )
        dbg_dma_enable |= 0x01;
    else if ( stricmp(str, "disable up") == 0 || stricmp(str, "disable upstream") == 0 )
        dbg_dma_enable &= ~0x01;
    else if ( stricmp(str, "enable down") == 0 || stricmp(str, "enable downstream") == 0 )
        dbg_dma_enable |= 0x02;
    else if ( stricmp(str, "disable down") == 0 || stricmp(str, "disable downstream") == 0 )
        dbg_dma_enable &= ~0x02;
    else
    {
        printk("echo \"<command>\" > /proc/eth/dma\n");
        printk("  command:\n");
        printk("    enable [up/down]  - enable up/down stream dma dump\n");
        printk("    disable [up/down] - disable up/down stream dma dump\n");
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

    return len - off;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
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

    if ( stricmp(str, "start") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_START_SET(1);
    else if ( stricmp(str, "stop") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_STOP_SET(1);
    else if ( stricmp(str, "step") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_STEP_SET(1);
    else
    {
        printk("echo \"<command>\" > /proc/eth/etop\n");
        printk("  command:\n");
        printk("    start - run pp32\n");
        printk("    stop  - stop pp32\n");
        printk("    step  - run pp32 with one step only\n");
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

    return len - off;
}

static int proc_write_burstlen(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[64];
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

    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z')) )
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
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};

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
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};

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
    len += sprintf(buf + len,          "      Dest QID:          %d\n", plaction->dest_qid);

    return len;
}

static INLINE int print_bridge(char *buf, int i, struct brg_forward_tbl *paction)
{
    static const char *dest_list[] = {"ETH0", "ETH1", "CPU0", "EXT_INT1", "EXT_INT2", "EXT_INT3", "EXT_INT4", "EXT_INT5"};

    int len = 0;
    u32 bit;
    int j, k;

    len += sprintf(buf + len,          "  entry %d (0x%08X)\n", i, (u32)paction);
    len += sprintf(buf + len,          "    compare\n");
    len += sprintf(buf + len,          "      mac:          %02X:%02X:%02X:%02X:%02X:%02X\n", paction->mac_52 >> 24, (paction->mac_52 >> 16) & 0xFF, (paction->mac_52 >> 8) & 0xFF, paction->mac_52 & 0xFF, (paction->mac_10 >> 8) & 0xFF, paction->mac_10 & 0xFF);
    len += sprintf(buf + len,          "      vci:          0x%04X\n", paction->vci);
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
    switch ( paction->itf )
    {
    case 0x00:
        len += sprintf(buf + len,      "      port:         ETH0\n");
        break;
    case 0x01:
        len += sprintf(buf + len,      "      port:         ETH1\n");
        break;
    case 0x02:
        len += sprintf(buf + len,      "      port:         CPU0\n");
        break;
    case 0x05:
        len += sprintf(buf + len,      "      port:         CPU1 EXT IF 0\n");
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

#if defined(DEBUG_DMA_PROC) && DEBUG_DMA_PROC
static INLINE int print_dma_desc(char *buf, int channel, int desc, int is_rx)
{
    volatile u32 *pdesc = NULL;
    volatile _tx_desc *ptx;
    int len = 0;

    if ( is_rx )
    {
        if ( channel == 1 )
            pdesc = DMA_RX_CH1_DESC_BASE;
        else if ( channel == 2 )
            pdesc = DMA_RX_CH2_DESC_BASE;
    }
    else
    {
        if ( channel == 2 )
            pdesc = DMA_TX_CH2_DESC_BASE;
    }
    if ( !pdesc )
        return 0;

    ptx = (volatile _tx_desc *)&pdesc[desc * 2];
    len += sprintf(buf + len, "    %2d (%08X). %08X %08X, own %d, c %d, sop %d, eop %d, off %d, addr %08X, len %d\n",
                                        desc, (u32)(pdesc + desc * 2), pdesc[desc * 2], pdesc[desc * 2 + 1],
                                        ptx->status.field.OWN, ptx->status.field.C, ptx->status.field.SoP, ptx->status.field.EoP,
                                        ptx->status.field.byte_offset, (u32)ptx->Data_Pointer | KSEG1, ptx->status.field.data_length);
//    len += sprintf(buf + len, "    %2d. %08X %08X\n", desc, pdesc[desc * 2], pdesc[desc * 2 + 1]);
//    len += sprintf(buf + len,  "        own %d, c %d, sop %d, eop %d\n", ptx->status.field.OWN, ptx->status.field.C, ptx->status.field.SoP, ptx->status.field.EoP);
//    len += sprintf(buf + len,  "        off %d, addr %08X, len %d\n", ptx->status.field.byte_offset, (u32)ptx->Data_Pointer | KSEG1, ptx->status.field.data_length);

    return len;
}
#endif

#if defined(DEBUG_DMA_PROC_UPSTREAM) && DEBUG_DMA_PROC_UPSTREAM
static INLINE int print_ema_desc(char *buf, int channel, int desc, int is_rx)
{
    volatile u32 *pdesc = NULL;
    volatile struct tx_descriptor *ptx;
    int len = 0;

    if ( !is_rx )
    {
        pdesc = EMA_TX_CH_DESC_BASE(channel);
    }
    if ( !pdesc )
        return 0;

    ptx = (volatile struct tx_descriptor *)&pdesc[desc * 2];
    len += sprintf(buf + len, "    %2d (%08X). %08X %08X, own %d, c %d, sop %d, eop %d, off %d, addr %08X, len %d\n",
                                        desc, (u32)(pdesc + desc * 2), pdesc[desc * 2], pdesc[desc * 2 + 1],
                                        ptx->own, ptx->c, ptx->sop, ptx->eop,
                                        ptx->byteoff, ((u32)ptx->dataptr << 2) | KSEG1, ptx->datalen);
//    len += sprintf(buf + len, "    %2d. %08X %08X\n", desc, pdesc[desc * 2], pdesc[desc * 2 + 1]);
//    len += sprintf(buf + len,  "        own %d, c %d, sop %d, eop %d\n", ptx->own, ptx->c, ptx->sop, ptx->eop);
//    len += sprintf(buf + len,  "        off %d, addr %08X, len %d\n", ptx->byteoff, ((u32)ptx->dataptr << 2) | KSEG1, ptx->datalen);

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
    case FWCODE_ROUTING_BRIDGING_ACC_D4:
        if ( is_wlan )
        {
            //  TODO
        }
        else
        {
            printk("  rout_fwd_vld   = %Xh\n", (u32)header->rout_fwd_vld);
            printk("  rout_mc_vld    = %Xh\n", (u32)header->rout_mc_vld);
            printk("  brg_rt_flag    = %Xh (%s)\n", (u32)header->brg_rt_flag, header->brg_rt_flag ? "bridging" : "routing");
            printk("  uc_mc_flag     = %Xh (%s)\n", (u32)header->uc_mc_flag, header->uc_mc_flag ? "multicast" : "uni-cast");
            printk("  tcpudp_err     = %Xh\n", (u32)header->tcpudp_err);
            printk("  tcpudp_chk     = %Xh\n", (u32)header->tcpudp_chk);
            printk("  is_udp         = %Xh\n", (u32)header->is_udp);
            printk("  is_tcp         = %Xh\n", (u32)header->is_tcp);
            printk("  ip_offset      = %Xh\n", (u32)header->ip_offset);
            printk("  is_pppoes      = %Xh\n", (u32)header->is_pppoes);
            printk("  is_ipv4        = %Xh\n", (u32)header->is_ipv4);
            printk("  is_vlan        = %Xh (%s)\n", (u32)header->is_vlan, is_vlan_str[header->is_vlan]);
            printk("  rout_index     = %Xh\n", (u32)header->rout_index);
            printk("  dest_list      = %Xh\n", (u32)header->dest_list);
            printk("  src_dir        = %Xh (from %s side)\n", (u32)header->src_dir, header->src_dir ? "WAN" : "LAN");
            printk("  acc_done       = %Xh\n", (u32)header->acc_done);
            printk("  tcp_rst        = %Xh\n", (u32)header->tcp_rst);
            printk("  tcp_fin        = %Xh\n", (u32)header->tcp_fin);
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

    if ( !dbg_enable )
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
    printk("  General Configuration(%08X):\n", (u32)ETX1_DMACH_ON);
    printk("    ETX1_DMACH_ON      = 0x%08X\n", *(u32*)ETX1_DMACH_ON);
    printk("    ETH_PORTS_CFG      = 0x%08X\n", *(u32*)ETH_PORTS_CFG);
    printk("    LAN_ROUT_TBL_CFG   = 0x%08X\n", *(u32*)LAN_ROUT_TBL_CFG);
    printk("    WAN_ROUT_TBL_CFG   = 0x%08X\n", *(u32*)WAN_ROUT_TBL_CFG);
    printk("    GEN_MODE_CFG       = 0x%08X\n", *(u32*)GEN_MODE_CFG);
  #if 0
    printk("  VLAN_CFG_TBL(%08X):\n", (u32)VLAN_CFG_TBL(0));
    for ( i = 0; i < 8; i++ )
        printk("    Entry(%d)           = 0x%08X\n", i, *(u32*)VLAN_CFG_TBL(i));
  #endif
    printk("  PPPOE_CFG_TBL(%08X):\n", (u32)PPPOE_CFG_TBL(0));
    for ( i = 0; i < 8; i++ )
        printk("    Entry(%d)           = 0x%08X\n", i, *(u32*)PPPOE_CFG_TBL(i));
    printk("  MTU_CFG_TBL(%08X):\n", (u32)MTU_CFG_TBL(0));
    for ( i = 0; i < 8; i++ )
        printk("    Entry(%d)           = 0x%08X\n", i, *(u32*)MTU_CFG_TBL(i));
    printk("  ROUT_FWD_HIT_STAT_TBL(%08X):\n", (u32)ROUT_FWD_HIT_STAT_TBL(0));
    for ( i = 0; i < 8; i++ )
        printk("    DWORD(%d)           = 0x%08X\n", i, *(u32*)ROUT_FWD_HIT_STAT_TBL(i));
  #if 0
    printk("  WAN_ROUT_MAC_CFG_TBL(%08X):\n", (u32)WAN_ROUT_MAC_CFG_TBL(0));
    for ( i = 0; i < 8; i++ )
    {
        printk("    Entry(%d)           = 0x%08X\n", i, *(u32*)WAN_ROUT_MAC_CFG_TBL(i));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_MAC_CFG_TBL(i) + 1));
    }
  #else
    printk("  ROUTER_MAC_CFG_TBL(%08X):\n", (u32)ROUTER_MAC_CFG_TBL(0));
    for ( i = 0; i < 8; i++ )
    {
        printk("    Entry(%d)           = 0x%08X\n", i, *(u32*)ROUTER_MAC_CFG_TBL(i));
        printk("                       = 0x%08X\n", *((u32*)ROUTER_MAC_CFG_TBL(i) + 1));
    }
  #endif
    printk("  WAN_ROUT_FORWARD_COMPARE_TBL(%08X) & WAN_ROUT_FORWARD_ACTION_TBL(%08X):\n", (u32)WAN_ROUT_FORWARD_COMPARE_TBL(0), (u32)WAN_ROUT_FORWARD_ACTION_TBL(0));
    for ( i = 0; i < 5; i++ )
    {
        int k;

        switch ( i )
        {
        case 2:  k = 128; break;
        case 3:  k = 192; break;
        default: k = i;
        }

        printk("    Entry(%d)\n", k);
        printk("      Compare          = 0x%08X\n", *(u32*)WAN_ROUT_FORWARD_COMPARE_TBL(k));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_FORWARD_COMPARE_TBL(k) + 1));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_FORWARD_COMPARE_TBL(k) + 2));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_FORWARD_COMPARE_TBL(k) + 3));
        printk("      Action           = 0x%08X\n", *(u32*)WAN_ROUT_FORWARD_ACTION_TBL(k));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_FORWARD_ACTION_TBL(k) + 1));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_FORWARD_ACTION_TBL(k) + 2));
        printk("                       = 0x%08X\n", *((u32*)WAN_ROUT_FORWARD_ACTION_TBL(k) + 3));
    }

    printk("  LAN_ROUT_FORWARD_COMPARE_TBL(%08X) & LAN_ROUT_FORWARD_ACTION_TBL(%08X):\n", (u32)LAN_ROUT_FORWARD_COMPARE_TBL(0), (u32)LAN_ROUT_FORWARD_ACTION_TBL(0));
    for ( i = 0; i < 5; i++ )
    {
        int k;

        switch ( i )
        {
        case 2:  k = 128; break;
        case 3:  k = 192; break;
        default: k = i;
        }

        printk("    Entry(%d)\n", k);
        printk("      Compare          = 0x%08X\n", *(u32*)LAN_ROUT_FORWARD_COMPARE_TBL(k));
        printk("                       = 0x%08X\n", *((u32*)LAN_ROUT_FORWARD_COMPARE_TBL(k) + 1));
        printk("                       = 0x%08X\n", *((u32*)LAN_ROUT_FORWARD_COMPARE_TBL(k) + 2));
        printk("                       = 0x%08X\n", *((u32*)LAN_ROUT_FORWARD_COMPARE_TBL(k) + 3));
        printk("      Action           = 0x%08X\n", *(u32*)LAN_ROUT_FORWARD_ACTION_TBL(k));
        printk("                       = 0x%08X\n", *((u32*)LAN_ROUT_FORWARD_ACTION_TBL(k) + 1));
        printk("                       = 0x%08X\n", *((u32*)LAN_ROUT_FORWARD_ACTION_TBL(k) + 2));
        printk("                       = 0x%08X\n", *((u32*)LAN_ROUT_FORWARD_ACTION_TBL(k) + 3));
    }
}
#endif

#if 0
static void force_dump_skb(struct sk_buff *skb, u32 len, char *title, int ch)
{
    int i;

    if ( skb->len < len )
        len = skb->len;

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

static void force_dump_flag_header(struct flag_header *header, char *title, int ch)
{
    if ( ch >= 0 )
        printk("%s (ch %d)\n", title, ch);
    else
        printk("%s\n", title);
    printk("  rout_fwd_vld = %Xh\n", (u32)header->rout_fwd_vld);
    printk("  rout_mc_vld  = %Xh\n", (u32)header->rout_mc_vld);
    printk("  tcpudp_err   = %Xh\n", (u32)header->tcpudp_err);
    printk("  tcpudp_chk   = %Xh\n", (u32)header->tcpudp_chk);
    printk("  is_udp       = %Xh\n", (u32)header->is_udp);
    printk("  is_tcp       = %Xh\n", (u32)header->is_tcp);
    printk("  ip_offset    = %Xh\n", (u32)header->ip_offset);
    printk("  is_pppoes    = %Xh\n", (u32)header->is_pppoes);
    printk("  is_ipv4      = %Xh\n", (u32)header->is_ipv4);
    printk("  is_vlan      = %Xh\n", (u32)header->is_vlan);
    printk("  rout_index   = %Xh\n", (u32)header->rout_index);
    printk("  dest_list    = %Xh\n", (u32)header->dest_list);
    printk("  src_itf      = %Xh\n", (u32)header->src_itf);
    printk("  src_dir      = %Xh\n", (u32)header->src_dir);
    printk("  acc_done     = %Xh\n", (u32)header->acc_done);
    printk("  tcp_fin      = %Xh\n", (u32)header->tcp_fin);
    printk("  pl_byteoff   = %Xh\n", (u32)header->pl_byteoff);
}
#endif



/*
 * ####################################
 *           Global Function
 * ####################################
 */

int get_dslwan_qid_with_vcc(struct atm_vcc *vcc)
{
    return 0;
}

int get_netif_qid_with_pkt(struct sk_buff *skb, void *arg, int is_atm_vcc)
{
    if ( is_atm_vcc )
        return 0;
    else
    {
        int port = get_eth_port((struct net_device *)arg);

        return g_eth_prio_queue_map[port][skb->priority > 7 ? 7 : skb->priority];
    }
}

// winder 20070214 add.
void danube_set_tmii_mode(int port)
{
    if (port == 0)
    {
        *ETOP_CFG |= 1 << 2;
        printk("%s: Set MII0 Turbo\n\r", __FUNCTION__);
    }
    else if (port == 1)
    {
        *ETOP_CFG |= 1 << 5;
        printk("%s: Set MII1 Turbo\n\r", __FUNCTION__);
    }
    else
    printk("%s:Wrong port number!!\n", __FUNCTION__); // should not be here.
}

void danube_set_normal_mode(int port)
{
    if (port == 0)
    {
        *ETOP_CFG &= ~(1 << 2);
        printk("%s: Set MII0 Normal\n\r", __FUNCTION__);
    }
    else if (port == 1)
    {
        *ETOP_CFG &= ~(1 << 5);
        printk("%s: Set MII1 Normal\n\r", __FUNCTION__);
    }
    else
    printk("%s:Wrong port number!!\n", __FUNCTION__); // should not be here.
}

int ppe_directpath_send(uint32_t if_id, struct sk_buff *skb, int32_t len, uint32_t flags)
{
    //  no WLAN to ETH0/1 support, push back immediately
    if_id -= 3;
    skb->dev = g_ppe_directpath_data[if_id].netif;
    //  no TX interface information, device push
    g_ppe_directpath_data[if_id].rx_fn_rxif_pkt++;
    g_ppe_directpath_data[if_id].callback.rx_fn(g_ppe_directpath_data[if_id].netif, NULL, skb, skb->len);
    return 0;
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
    return 0;
}



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

int __init danube_eth_init(void)
{
    int ret;
    int i;
    char buf[512];

    printk("Loading D4 (MII0/1) driver... ");

#if IFX_PPA_DP_DBG_PARAM_ENABLE
    if( ifx_ppa_drv_dp_dbg_param_enable == 1 )
    {
        ethwan = ifx_ppa_drv_dp_dbg_param_ethwan;
    }
#endif  //IFX_PPA_DP_DBG_PARAM_ENABLE

    reset_ppe();

    ret = init_local_variables();
    if ( ret )
        goto INIT_LOCAL_VARIABLES_FAIL;

    init_hw();

    init_communication_data_structures(g_fwcode);

    //  same function as "dbg_disable_dma_copy_port"
    //  SB_BUFFER(0x3950) -> ETH 1 TX (pages) - Data
    //  PPE FW play role as CPU and set owner bit 1 means it's inited for memory copy port as RX
    WLAN_CPU_TX_DESC_BASE[0] = 0xBB000000;
    WLAN_CPU_TX_DESC_BASE[1] = (uint32_t)SB_BUFFER(0x32A0);
    WLAN_CPU_TX_DESC_BASE[2] = 0xBB000000;
    WLAN_CPU_TX_DESC_BASE[3] = (uint32_t)SB_BUFFER(0x3430);
    //  set page data (src_itf = 5)
    ((uint8_t *)SB_BUFFER(0x32A0))[21] = 5;
    ((uint8_t *)SB_BUFFER(0x3430))[21] = 5;

    ret = alloc_dma();
    if ( ret )
        goto ALLOC_DMA_FAIL;

    //  create device
    for ( i = 0; i < 2; i++ )
    {
        g_eth_net_dev[i] = alloc_netdev(sizeof(struct eth_priv_data), "eth%d", eth_init);
        if(!g_eth_net_dev[i]){
            printk("Cannot alloc eth netdevice\n");
            goto REGISTER_NETDEV_FAIL;
        }
        ret = register_netdev(g_eth_net_dev[i]);
        if ( ret ){
            free_netdev(g_eth_net_dev[i]);
            g_eth_net_dev[i] = NULL;
            goto REGISTER_NETDEV_FAIL;
        }
    }

    ret = request_irq(PPE_MAILBOX_IGU1_INT, mailbox_irq_handler, IRQF_DISABLED, "d4_mailbox_isr", NULL);
    if ( ret )
        goto REQUEST_IRQ_PPE_MAILBOX_IGU1_INT_FAIL;

    memset(g_ppe_directpath_data, 0, sizeof(g_ppe_directpath_data));

    dump_init();

    ret = pp32_start(g_fwcode);
    if ( ret )
        goto PP32_START_FAIL;

    //  careful, PPE firmware may set some registers, recover them here
    *MBOX_IGU1_IER = g_mailbox_signal_mask;
    *MBOX_IGU0_ISRC = 0xFFFFFFFF;
    *MBOX_IGU0_IER = 0x01;

#if !defined(ENABLE_SINGLE_CORE_EMUL) || !ENABLE_SINGLE_CORE_EMUL
    g_cpu1_skb_data[DMA_RX_CH3_DESC_NUM] = (unsigned char *)1;
#endif

    //  everything is ready, enable hardware to take package
    start_etop();

    //  create proc file
    proc_file_create();

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
REQUEST_IRQ_PPE_MAILBOX_IGU1_INT_FAIL:
REGISTER_NETDEV_FAIL:
    while ( i-- ){
        unregister_netdev(g_eth_net_dev[i]);
        free_netdev(g_eth_net_dev[i]);
        g_eth_net_dev[i] = NULL;
    }
    free_dma();
ALLOC_DMA_FAIL:
    clear_local_variables();
INIT_LOCAL_VARIABLES_FAIL:
    printk("init failed!\n");
    return ret;
}

void __exit danube_eth_exit(void)
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

    proc_file_delete();

    pp32_stop();

    free_dma();

    free_irq(PPE_MAILBOX_IGU1_INT, NULL);

    for ( i = 0; i < 2; i++ ){
        unregister_netdev(g_eth_net_dev[i]);
        free_netdev(g_eth_net_dev[i]);
        g_eth_net_dev[i] = NULL;
    }

    clear_local_variables();
}

#if defined(CONFIG_IFX_PPA_DATAPATH)
static int __init danube_eth0addr_setup(char *line)
{
    ethaddr_setup(0, line);

    return 0;
}

static int __init danube_eth1addr_setup(char *line)
{
    ethaddr_setup(1, line);

    return 0;
}

static int __init danube_d4_wan_mode_setup(char *line)
{
    if ( strcmp(line, "1") == 0 )
        ethwan = 1;
    else if ( strcmp(line, "2") == 0 )
        ethwan = 2;

    return 0;
}
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
        return -1;
    }

    return -1;

}



module_init(danube_eth_init);
module_exit(danube_eth_exit);
#if defined(CONFIG_IFX_PPA_DATAPATH)
  __setup("ethaddr=", danube_eth0addr_setup);
  __setup("eth1addr=", danube_eth1addr_setup);
  __setup("ethwan=", danube_d4_wan_mode_setup);
#endif



MODULE_LICENSE("GPL");
