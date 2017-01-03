/******************************************************************************
**
** FILE NAME    : ifxmips_ppa_hal_danube_a4.c
** PROJECT      : UEIP
** MODULES      : ATM + MII0 Acceleration Package (Danube PPA A4)
**
** DATE         : 22 SEP 2007
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM + MII0 Driver with Acceleration Firmware (A4)
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 22 SEP 2007  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */
#define VER_FAMILY      0x02        //  bit 0: res
                                    //      1: Danube
                                    //      2: Twinpass
                                    //      3: Amazon-SE
                                    //      4: res
                                    //      5: AR9
                                    //      6: GR9
#define VER_DRTYPE      0x04        //  bit 0: Normal Data Path driver
                                    //      1: Indirect-Fast Path driver
                                    //      2: HAL driver
                                    //      3: Hook driver
                                    //      4: Stack/System Adaption Layer driver
                                    //      5: PPA API driver
#define VER_INTERFACE   0x00        //  bit 0: MII 0
                                    //      1: MII 1
                                    //      2: ATM WAN
                                    //      3: PTM WAN
#define VER_ACCMODE     0x03        //  bit 0: Routing
                                    //      1: Bridging
#define VER_MAJOR       0
#define VER_MID         0
#define VER_MINOR       3



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
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

/*
 *  Chip Specific Head File
 */

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
  #include <net/danube_ppa_stack_al.h>
  #include <net/danube_ppa_ppe_a4_hal.h>
#else
  #include <net/ifx_ppa_api.h>
  #include <net/ifx_ppa_ppe_hal.h>
#endif



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  Compilation Switch
 */

#define ENABLE_DEBUG                            1

#define ENABLE_ASSERT                           1

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
  #define ENABLE_DEBUG_PRINT                    1
  #define DISABLE_INLINE                        1
#endif

#if defined(DISABLE_INLINE) && DISABLE_INLINE
  #define INLINE
#else
  #define INLINE                                inline
#endif

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
  #undef  dbg
  static unsigned int ifx_ppa_hal_dbg_enable = 0;
  #define dbg(format, arg...)                   do { if ( ifx_ppa_hal_dbg_enable  ) printk(KERN_WARNING ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #if !defined(dbg)
    #define dbg(format, arg...)
  #endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
  #define ASSERT(cond, format, arg...)          do { if ( !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #define ASSERT(cond, format, arg...)
#endif

/*
 *  Firmware Constant
 */
#define MAX_BRIDGING_ENTRIES                    128

#define MAX_ROUTING_ENTRIES                     128 //  pure routing mode: 192
#define MAX_WAN_MC_ENTRIES                      32
#define MAX_PPPOE_ENTRIES                       8
#define MAX_MTU_ENTRIES                         8
#define MAX_MAC_ENTRIES                         16
#define MAX_OUTER_VLAN_ENTRIES                  32

#define CPU_TO_WAN_TX_DESC_NUM                  16
#define WAN_TX_DESC_NUM(i)                      24
#define WAN_RX_DESC_NUM(i)                      16
#define DMA_RX_CH1_DESC_NUM                     16
#define DMA_RX_CH2_DESC_NUM                     16
#define DMA_TX_CH2_DESC_NUM                     DMA_RX_CH2_DESC_NUM
#define WLAN_CPU_TX_DESC_NUM                    16
#define PRE_ALLOC_DESC_TOTAL_NUM                (CPU_TO_WAN_TX_DESC_NUM + WAN_TX_DESC_NUM(0) * 8                        \
                                                + WAN_RX_DESC_NUM(0) * 2 + DMA_RX_CH1_DESC_NUM * DMA_RX_CH2_DESC_NUM    \
                                                + DMA_TX_CH2_DESC_NUM + WLAN_CPU_TX_DESC_NUM)

#define IFX_PPA_PORT_NUM                        8

#define MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES        8
#define MAX_SRC_IP_VLAN_MAP_ENTRIES             4
#define MAX_ETHTYPE_VLAN_MAP_ENTRIES            4
#define MAX_VLAN_VLAN_MAP_ENTRIES               8

/*
 *  destlist
 */
//#define IFX_PPA_DEST_LIST_ETH0                0x0001
//#define IFX_PPA_DEST_LIST_ETH1                0x0002
#ifndef IFX_PPA_DEST_LIST_CPU0
  #define IFX_PPA_DEST_LIST_CPU0                0x0004
#endif
//#define IFX_PPA_DEST_LIST_EXT_INT1            0x0008
//#define IFX_PPA_DEST_LIST_EXT_INT2            0x0010
//#define IFX_PPA_DEST_LIST_EXT_INT3            0x0020
//#define IFX_PPA_DEST_LIST_EXT_INT4            0x0040
//#define IFX_PPA_DEST_LIST_EXT_INT5            0x0080
//#define IFX_PPA_DEST_LIST_ATM                 0x0100

/*
 *  port
 */
//#define IFX_PPA_PORT_ETH0                     0x00
//#define IFX_PPA_PORT_ETH1                     0x01
#ifndef IFX_PPA_PORT_ATM
  #define IFX_PPA_PORT_ATM                      0x01
#endif
//#define IFX_PPA_PORT_CPU0                     0x02
//#define IFX_PPA_PORT_CPU1_EXT_IF0             0x05
//#define IFX_PPA_PORT_ANY                      IFX_PPA_PORT_CPU0
//#define IFX_PPA_PORT_NUM                      0x08

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
                                                     ( ( (__sb_addr) >= 0x1000 ) && ( (__sb_addr) <= 0x17FF ) )  ?  CDM_CODE_MEMORY_RAM1_ADDR((__sb_addr) - 0x1000) :   \
                                                     ( ( (__sb_addr) >= 0x2000 ) && ( (__sb_addr) <= 0x23FF ) )  ?  SB_RAM0_ADDR((__sb_addr) - 0x2000) :                \
                                                     ( ( (__sb_addr) >= 0x2400 ) && ( (__sb_addr) <= 0x2BFF ) )  ?  SB_RAM1_ADDR((__sb_addr) - 0x2400) :                \
                                                     ( ( (__sb_addr) >= 0x2C00 ) && ( (__sb_addr) <= 0x35FF ) )  ?  SB_RAM2_ADDR((__sb_addr) - 0x2C00) :                \
                                                     ( ( (__sb_addr) >= 0x3600 ) && ( (__sb_addr) <= 0x39FF ) )  ?  SB_RAM3_ADDR((__sb_addr) - 0x3600) :                \
                                                 0 ) )

#define FW_VER_ID                               ((volatile struct fw_ver_id *)SB_BUFFER(0x2001))

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

#define WRX_QUEUE_CONFIG(i)                     ((volatile struct wrx_queue_config *)       SB_BUFFER(0x35A0 + (i) * 10))   /*  i < 8   */
#define WTX_PORT_CONFIG(i)                      ((volatile struct wtx_port_config *)        SB_BUFFER(0x2A7E + (i)))        /*  i < 2   */
#define WTX_QUEUE_CONFIG(i)                     ((volatile struct wtx_queue_config *)       SB_BUFFER(0x34E0 + (i) * 24))   /*  i < 8   */

#define DSL_WAN_MIB_TABLE                       ((volatile struct dsl_wan_mib_table *)      SB_BUFFER(0x2AA0))
#define DSL_WAN_VC_MIB_TABLE(i)                 ((volatile struct dsl_wan_vc_mib_table *)   SB_BUFFER(0x2AB0 + (i) * 4))    /*  i < 8   */

#define WAN_MIB_TABLE                           DSL_WAN_MIB_TABLE

#define HTU_ENTRY(i)                            ((volatile struct htu_entry *)              SB_BUFFER(0x2650 + (i)))        /*  i < 32  */
#define HTU_MASK(i)                             ((volatile struct htu_mask *)               SB_BUFFER(0x2670 + (i)))        /*  i < 32  */
#define HTU_RESULT(i)                           ((volatile struct htu_result *)             SB_BUFFER(0x2690 + (i)))        /*  i < 32  */

#define ITF_MIB_TBL(i)                          ((volatile struct itf_mib *)                SB_BUFFER(0x3980 + (i) * 16))   /*  i < 8   */

#define BRG_FWD_HIT_STAT_TBL(i)                 SB_BUFFER(0x2A68 + (i))         /*  i < 8   */
#define BRIDGING_FORWARD_TBL(i)                 SB_BUFFER(0x21D0 + (i) * 4)     /*  i < 128 */

#define DEFAULT_ITF_VLAN_MAP(i)                 ((volatile struct default_itf_vlan_map *)   SB_BUFFER(0x2A30 + (i)))        /*  i < 8   */
#define SRC_IP_VLAN_MAP(i)                      ((volatile struct src_ip_vlan_map *)        SB_BUFFER(0x2A50 + (i) * 2))    /*  i < 4   */
#define ETHTYPE_VLAN_MAP(i)                     ((volatile struct ethtype_vlan_map *)       SB_BUFFER(0x2A38 + (i) * 2))    /*  i < 4   */
#define VLAN_VLAN_MAP(i)                        ((volatile struct vlan_vlan_map *)          SB_BUFFER(0x3440 + (i) * 3))    /*  i < 8   */

#define PPPOE_CFG_TBL(i)                        SB_BUFFER(0x2A58 + (i))         /*  i < 8   */
#define MTU_CFG_TBL(i)                          SB_BUFFER(0x2A60 + (i))         /*  i < 8   */
#define ROUT_MAC_CFG_TBL(i)                     SB_BUFFER(0x29E0 + (i) * 2)     /*  i < 16  */

#define ROUT_FWD_HIT_STAT_TBL(i)                SB_BUFFER(0x2A70 + (i))         /*  i < 8   */
#define ROUT_FORWARD_COMPARE_TBL(i)             ((volatile struct rout_forward_compare_tbl *)   SB_BUFFER(0x2050 + (i) * 3))    /*  i < 128 */
#define ROUT_FORWARD_ACTION_TBL(i)              ((volatile struct rout_forward_action_tbl *)    SB_BUFFER(0x23D0 + (i) * 5))    /*  i < 128 */

#define WAN_ROUT_FORWARD_COMPARE_TBL(i)         ROUT_FORWARD_COMPARE_TBL(i)
#define LAN_ROUT_FORWARD_COMPARE_TBL(i)         ROUT_FORWARD_COMPARE_TBL(LAN_ROUT_TBL_CFG->lan_rout_off + (i))
#define WAN_ROUT_FORWARD_ACTION_TBL(i)          ROUT_FORWARD_ACTION_TBL(i)
#define LAN_ROUT_FORWARD_ACTION_TBL(i)          ROUT_FORWARD_ACTION_TBL(LAN_ROUT_TBL_CFG->lan_rout_off + (i))

#define MC_ROUT_FWD_HIT_STAT_TBL(i)             SB_BUFFER(0x2A78 + (i))         /*  i < 2   */
#define WAN_ROUT_MULTICAST_TBL(i)               ((volatile struct wan_rout_multicast_tbl *)     SB_BUFFER(0x2010 + (i) * 2))    /*  i < 32  */
#define WAN_ROUT_MC_OUTER_VLAN_TBL(i)           ((volatile struct wan_rout_mc_outer_vlan_tbl *) SB_BUFFER(0x2958 + (i) / 4))    /*  i < 32  */

#define OUTER_VLAN_TBL(i)                       SB_BUFFER(0x3940 + (i))         /*  i < 32  */

//#define VLAN_PRI_TO_WAN_TX_QID_MAPPING(i)       SB_BUFFER(0x29C0 + (i))         /*  i < 8   */

#define DSL_TX_QUEUE_MAPPING(i)                 SB_BUFFER(0x3458 + (i))         /*  i < 8   */  //  MSB -> LSB = Prio No. 7 - 0

/*
 *  DMA/EMA Descriptor Base Address
 */
#define CPU_TO_WAN_TX_DESC_BASE                 ((volatile struct tx_descriptor *)SB_BUFFER(0x2960))                /*         16 each queue    */
#define WAN_TX_DESC_BASE(i)                     ((volatile struct tx_descriptor *)SB_BUFFER(0x32A0 + (i) * 2 * 24)) /*  i < 8, 24 each queue    */
#define WAN_RX_DESC_BASE(i)                     ((volatile struct rx_descriptor *)SB_BUFFER(0x3070 + (i) * 2 * 16)) /*  i < 2, 16 each queue    */
#define DMA_RX_CH1_DESC_BASE                    ((volatile struct rx_descriptor *)SB_BUFFER(0x3460))                /*         16 each queue    */
#define DMA_RX_CH2_DESC_BASE                    ((volatile struct rx_descriptor *)SB_BUFFER(0x3480))                /*         16 each queue    */
#define DMA_TX_CH2_DESC_BASE                    ((volatile struct tx_descriptor *)SB_BUFFER(0x34A0))                /*         16 each queue    */
#define WLAN_CPU_TX_DESC_BASE                   ((volatile struct tx_descriptor *)SB_BUFFER(0x34C0))                /*         16 each queue    */

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
 *  Register
 */
#define PP32_DBG_CTRL                           PP32_DEBUG_REG_ADDR(0x0000)

#define DBG_CTRL_START_SET(value)               ((value) ? (1 << 0) : 0)
#define DBG_CTRL_STOP_SET(value)                ((value) ? (1 << 1) : 0)
#define DBG_CTRL_STEP_SET(value)                ((value) ? (1 << 2) : 0)

#define PP32_BRK_SRC                            PP32_DEBUG_REG_ADDR(0x0002)

#define PP32_DBG_PC_MIN(i)                      PP32_DEBUG_REG_ADDR(0x0010 + (i))
#define PP32_DBG_PC_MAX(i)                      PP32_DEBUG_REG_ADDR(0x0014 + (i))
#define PP32_DBG_DATA_MIN(i)                    PP32_DEBUG_REG_ADDR(0x0018 + (i))
#define PP32_DBG_DATA_MAX(i)                    PP32_DEBUG_REG_ADDR(0x001A + (i))

/*
 *  Helper Macro
 */
#define NUM_ENTITY(x)                           (sizeof(x) / sizeof(*(x)))
#define BITSIZEOF_UINT32                        (sizeof(uint32_t) * 8)



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
    unsigned int    mac_change_drop     :1;
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
    unsigned int    all_zero2           :32;
    /*  3h  */
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
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Proc File
 */
static INLINE void proc_file_create(void);
static INLINE void proc_file_delete(void);
static int proc_read_mib(char *, char **, off_t, int, int *, void *);
static int proc_write_mib(struct file *, const char *, unsigned long, void *);
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
static INLINE int stricmp(const char *, const char *);
static INLINE int strincmp(const char *, const char *, int);
static INLINE int get_token(char **, char **, int *, int *);
static INLINE int get_number(char **, int *, int);
static INLINE void get_ip_port(char **, int *, unsigned int *);
static INLINE void get_mac(char **, int *, unsigned int *);
static INLINE void ignore_space(char **, int *);
static INLINE int print_wan_route(char *, int, struct rout_forward_compare_tbl *, struct rout_forward_action_tbl *);
static INLINE int print_lan_route(char *, int, struct rout_forward_compare_tbl *, struct rout_forward_action_tbl *);
static INLINE int print_bridge(char *, int, struct brg_forward_tbl *);
static INLINE int print_mc(char *, int, struct wan_rout_multicast_tbl *);
static int ifx_ppa_ppe_hal_init(void);
static void ifx_ppa_ppe_hal_exit(void);

static int32_t ppa_hal_generic_hook(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag);

/*
 * ####################################
 *           Global Variable
 * ####################################
 */

static uint32_t g_routing_entry_occupation[(MAX_ROUTING_ENTRIES + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32 + 1] = {0};
static uint32_t *g_lan_routing_entry_occupation = g_routing_entry_occupation;
static uint32_t g_lan_routing_entries = 0;
static uint32_t g_lan_routing_entry_off = 0;
static PPA_LOCK g_lan_routing_lock;
static uint32_t g_wan_routing_entries = 0;
static PPA_LOCK g_wan_routing_lock;

static uint32_t g_wan_mc_entry_occupation[(MAX_WAN_MC_ENTRIES + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32] = {0};
static uint32_t g_wan_mc_entries = 0;
static PPA_LOCK g_wan_mc_lock;

static uint32_t g_bridging_entry_occupation[(MAX_BRIDGING_ENTRIES + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32] = {0};
static uint32_t g_bridging_entries = 0;
static PPA_LOCK g_bridging_lock;

static uint32_t g_pppoe_entry_counter[MAX_PPPOE_ENTRIES] = {0};
static PPA_LOCK g_pppoe_lock;

static uint32_t g_mtu_entry_counter[MAX_MTU_ENTRIES] = {0};
static PPA_LOCK g_mtu_lock;

static uint32_t g_mac_entry_counter[MAX_MAC_ENTRIES] = {0};
static PPA_LOCK g_mac_lock;
static uint32_t g_mac_entries_counter = 0;

static uint32_t g_outer_vlan_entry_counter[MAX_OUTER_VLAN_ENTRIES] = {0};
static PPA_LOCK g_outer_vlan_lock;

#if defined(ENABLE_CLEVER_BRG_VLAN_SUPPORT) && ENABLE_CLEVER_BRG_VLAN_SUPPORT
  static uint32_t g_default_itf_vlan_map_occupation = 0;
  static uint32_t g_ip_vlan_map_counter[MAX_SRC_IP_VLAN_MAP_ENTRIES] = {0};
  static uint32_t g_ethtype_vlan_map_counter[MAX_ETHTYPE_VLAN_MAP_ENTRIES] = {0};
  static uint32_t g_vlan_vlan_map_counter[MAX_VLAN_VLAN_MAP_ENTRIES] = {0};
  static uint32_t g_itf_cfg_ig_counter[IFX_PPA_PORT_NUM] = {0};
  static uint32_t g_itf_cfg_eg_counter[IFX_PPA_PORT_NUM] = {0};
#else
  static uint32_t g_default_itf_vlan_map_occupation = 0;
  static uint32_t g_ip_vlan_map_occupation = 0;
  static uint32_t g_ethtype_vlan_map_occupation = 0;
  static uint32_t g_vlan_vlan_map_occupation = 0;
#endif
static PPA_LOCK g_itf_cfg_lock;

/*
 *  PROC
 */
static int g_ppa_proc_dir_flag = 0;
static struct proc_dir_entry *g_ppa_proc_dir = NULL;
static int g_ppa_ppe_hal_proc_dir_flag = 0;
static struct proc_dir_entry *g_ppa_ppe_hal_proc_dir = NULL;
static struct proc_dir_entry *g_ppa_ppe_a4_hal_proc_dir = NULL;

#if !defined(CONFIG_DANUBE_PPA_A4_API) && !defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
  static unsigned int g_sys_flag_stack[16];
  static unsigned int g_sys_flag_pos = 0;
#endif


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
extern struct proc_dir_entry proc_root;
#endif

/*
 * ####################################
 *           Extern Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static INLINE void __ppa_disable_int(void)
{
#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    ppa_disable_int();
#else
    unsigned int sys_flag;

    sys_flag = ppa_disable_int();
    if ( g_sys_flag_pos < NUM_ENTITY(g_sys_flag_stack) )
        g_sys_flag_stack[g_sys_flag_pos] = sys_flag;
    g_sys_flag_pos++;
#endif
}

static INLINE void __ppa_enable_int(void)
{
#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    ppa_enable_int();
#else
    if ( g_sys_flag_pos > 0 )
    {
        if ( --g_sys_flag_pos < NUM_ENTITY(g_sys_flag_stack) )
            ppa_enable_int(g_sys_flag_stack[g_sys_flag_pos]);
    }
#endif
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

    for ( res = g_ppa_proc_dir->subdir; res; res = res->next )
        if ( res->namelen == 3
            && res->name[0] == 'h'
            && res->name[1] == 'a'
            && res->name[2] == 'l' ) //  "hal"
        {
            g_ppa_ppe_hal_proc_dir = res;
            break;
        }
    if ( !res )
    {
        g_ppa_ppe_hal_proc_dir = proc_mkdir("hal", g_ppa_proc_dir);
        g_ppa_ppe_hal_proc_dir_flag = 1;
    }

    g_ppa_ppe_a4_hal_proc_dir = proc_mkdir("a4", g_ppa_ppe_hal_proc_dir);

    res = create_proc_read_entry("mib",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_mib,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mib;

    res = create_proc_read_entry("route",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_route,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_route;

    res = create_proc_read_entry("bridge",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_bridge,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_bridge;

    res = create_proc_read_entry("mc",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_mc,
                                  NULL);

    res = create_proc_read_entry("genconf",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_genconf,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_genconf;

    res = create_proc_read_entry("queue",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_queue,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_queue;

    res = create_proc_read_entry("pppoe",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_pppoe,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_pppoe;

    res = create_proc_read_entry("mtu",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_mtu,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mtu;

    res = create_proc_read_entry("hit",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_hit,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_hit;

    res = create_proc_read_entry("mac",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_mac,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_mac;

    res = create_proc_read_entry("vlan",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_vlan,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_vlan;

    res = create_proc_read_entry("out_vlan",
                                  0,
                                  g_ppa_ppe_a4_hal_proc_dir,
                                  proc_read_out_vlan,
                                  NULL);
    if ( res )
        res->write_proc = proc_write_out_vlan;
}

static INLINE void proc_file_delete(void)
{
    remove_proc_entry("out_vlan",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("vlan",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("mac",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("hit",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("mtu",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("pppoe",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("queue",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("genconf",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("mc",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("bridge",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("route",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("mib",
                      g_ppa_ppe_a4_hal_proc_dir);

    remove_proc_entry("a4",
                      g_ppa_ppe_hal_proc_dir);

    if ( g_ppa_ppe_hal_proc_dir_flag )
        remove_proc_entry("hal",
                          g_ppa_proc_dir);

    if ( g_ppa_proc_dir_flag )
        remove_proc_entry("ppa", NULL);
}

static int proc_read_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *strtitle[8] = {
        "ETH0", "ATM", "CPU0", NULL, NULL, NULL, NULL, NULL
    };
    int len = 0;
    int i;

    for ( i = 0; i < sizeof(strtitle) / sizeof(*strtitle); i++ )
    {
        if ( !strtitle[i] )
            continue;

        len += sprintf(page + off + len,     "%s\n", strtitle[i]);
        len += sprintf(page + off + len,     "    ig_fast_brg_uni_pkts:    %u\n", ITF_MIB_TBL(i)->ig_fast_brg_uni_pkts);
        len += sprintf(page + off + len,     "    ig_fast_brg_mul_pkts:    %u\n", ITF_MIB_TBL(i)->ig_fast_brg_mul_pkts);
        len += sprintf(page + off + len,     "    ig_fast_brg_br_pkts:     %u\n", ITF_MIB_TBL(i)->ig_fast_brg_br_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_uni_udp_pkts: %u\n", ITF_MIB_TBL(i)->ig_fast_rt_uni_udp_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_uni_tcp_pkts: %u\n", ITF_MIB_TBL(i)->ig_fast_rt_uni_tcp_pkts);
        len += sprintf(page + off + len,     "    ig_fast_rt_mul_pkts:     %u\n", ITF_MIB_TBL(i)->ig_fast_rt_mul_pkts);
        len += sprintf(page + off + len,     "    ig_fast_brg_bytes:       %u\n", ITF_MIB_TBL(i)->ig_fast_brg_bytes);
        len += sprintf(page + off + len,     "    ig_fast_rt_bytes:        %u\n", ITF_MIB_TBL(i)->ig_fast_rt_bytes);
        len += sprintf(page + off + len,     "    ig_cpu_pkts:             %u\n", ITF_MIB_TBL(i)->ig_cpu_pkts);
        len += sprintf(page + off + len,     "    ig_cpu_bytes:            %u\n", ITF_MIB_TBL(i)->ig_cpu_bytes);
        len += sprintf(page + off + len,     "    ig_drop_pkts:            %u\n", ITF_MIB_TBL(i)->ig_drop_pkts);
        len += sprintf(page + off + len,     "    ig_drop_bytes:           %u\n", ITF_MIB_TBL(i)->ig_drop_bytes);
        len += sprintf(page + off + len,     "    eg_fast_pkts:            %u\n", ITF_MIB_TBL(i)->eg_fast_pkts);
        len += sprintf(page + off + len,     "    eg_fast_bytes:           %u\n", ITF_MIB_TBL(i)->eg_fast_bytes);
    }

    *eof = 1;

    return len;
}

static int proc_write_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;
    u32 eth_clear;

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

    eth_clear = 0;
    if ( stricmp(p, "clear") == 0 || stricmp(p, "clear all") == 0
        || stricmp(p, "clean") == 0 || stricmp(p, "clean all") == 0 )
        eth_clear = 0xFF;
    else if ( stricmp(p, "clear eth0") == 0 || stricmp(p, "clear 0") == 0
            || stricmp(p, "clean eth0") == 0 || stricmp(p, "clean 0") == 0 )
        eth_clear = 0x01;
    else if ( stricmp(p, "clear atm") == 0 || stricmp(p, "clear 1") == 0
            || stricmp(p, "clean atm") == 0 || stricmp(p, "clean 1") == 0 )
        eth_clear = 0x02;
    else if ( stricmp(p, "clear cpu0") == 0 || stricmp(p, "clear 2") == 0
            || stricmp(p, "clean cpu0") == 0 || stricmp(p, "clean 2") == 0 )
        eth_clear = 0x04;
    else if ( stricmp(p, "clear cpu1") == 0 || stricmp(p, "clear 5") == 0
            || stricmp(p, "clean cpu1") == 0 || stricmp(p, "clean 5") == 0 )
        eth_clear = 0x20;

    if ( (eth_clear & 0x01) )
        ppa_memset((void *)ITF_MIB_TBL(0), 0, sizeof(struct itf_mib));
    if ( (eth_clear & 0x02) )
        ppa_memset((void *)ITF_MIB_TBL(1), 0, sizeof(struct itf_mib));
    if ( (eth_clear & 0x04) )
        ppa_memset((void *)ITF_MIB_TBL(2), 0, sizeof(struct itf_mib));
    if ( (eth_clear & 0x20) )
        ppa_memset((void *)ITF_MIB_TBL(5), 0, sizeof(struct itf_mib));

    return count;
}

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

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Wan Routing Table\n");
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

    pcompare = (struct rout_forward_compare_tbl *)WAN_ROUT_FORWARD_COMPARE_TBL(0);
    pwaction = (struct rout_forward_action_tbl *)WAN_ROUT_FORWARD_ACTION_TBL(0);
    for ( i = 0; i < WAN_ROUT_TBL_CFG->wan_rout_num; i++ )
    {
        if ( *(u32*)pcompare && *((u32*)pcompare + 1) )
        {
            llen = print_wan_route(str, i, pcompare, pwaction);
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
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        pwaction++;
    }

    llen = sprintf(str, "Lan Routing Table\n");
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
                goto PROC_READ_ROUTE_OVERRUN_END;
        }

        pcompare++;
        plaction++;
    }

    *eof = 1;

    return len - off;

PROC_READ_ROUTE_OVERRUN_END:
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
            ppa_memcpy((unsigned char *)pu1, (unsigned char *)&compare_tbl, sizeof(compare_tbl));
            ppa_memcpy((unsigned char *)pu2, (unsigned char *)&action_tbl, sizeof(action_tbl));
        }
    }

    return count;
}

static int proc_read_bridge(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    struct brg_forward_tbl *paction;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

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

    return len - off;

PROC_READ_BRIDGE_OVERRUN_END:
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
        memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off )
    {
        memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;

    paction = WAN_ROUT_MULTICAST_TBL(0);
    for ( i = 0; i < MAX_WAN_MC_ENTRIES; i++ )
    {
        if ( paction->wan_dest_ip )
        {
            llen = print_mc(str, i, (struct wan_rout_multicast_tbl *)paction);
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
    char *ppst[3] = {0};

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
        goto PROC_READ_GENCONF_OVERRUN_END;

    *eof = 1;

    return len - off;

PROC_READ_GENCONF_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_genconf(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    int f_wan_hi = 0;

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

    if ( f_wan_hi )
    {
        int num;

        num = get_number(&p, &rlen, 0);
        if ( f_wan_hi > 0 )
            ETH_PORTS_CFG->wan_vlanid_hi = num;
        else
            ETH_PORTS_CFG->wan_vlanid_lo = num;
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

    llen = sprintf(pstr, "RX Queue Config (0x%08X):\n", (u32)WRX_QUEUE_CONFIG(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        rx = *WRX_QUEUE_CONFIG(i);
        llen  = sprintf(str       , "  %d: MPoA type - %s, MPoA mode - %s, IP version %d\n", i, mpoa_type_str[rx.mpoa_type], rx.mpoa_mode ? "LLC" : "VC mux", rx.ip_ver ? 6 : 4);
        llen += sprintf(str + llen,  "     Oversize - %d, Undersize - %d, Max Frame size - %d\n", rx.oversize, rx.undersize, rx.mfs);
        llen += sprintf(str + llen,  "     uu mask - 0x%02X, cpi mask - 0x%02X, uu exp - 0x%02X, cpi exp - 0x%02X\n", rx.uumask, rx.cpimask, rx.uuexp, rx.cpiexp);

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
            goto PROC_READ_QUEUE_OVERRUN_END;
    }

    llen = sprintf(str, "TX Queue Config (0x%08X):\n", (u32)WTX_QUEUE_CONFIG(0));
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

static int proc_read_pppoe(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "PPPoE Table (0x%08X) - Session ID:\n", (u32)PPPOE_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *PPPOE_CFG_TBL(i));
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
            goto PROC_READ_PPPOE_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_PPPOE_OVERRUN_END:
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
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "MTU Table (0x%08X):\n", (u32)MTU_CFG_TBL(0));
    pstr += llen;
    len += llen;

    for ( i = 0; i < 8; i++ )
    {
        llen = sprintf(str, "  %d: %d\n", i, *MTU_CFG_TBL(i));
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
            goto PROC_READ_MTU_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_MTU_OVERRUN_END:
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
    char str[2048];
    int llen;
    int i;
    int n;
    unsigned long bit;

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
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }

    llen = sprintf(str, "Multicast Routing Hit Table (0x%08X):\n", (u32)MC_ROUT_FWD_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
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
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }

    llen = sprintf(str, "Bridging Hit Table (0x%08X):\n", (u32)BRG_FWD_HIT_STAT_TBL(0));
    llen += sprintf(str + llen, "             1 2 3 4 5 6 7 8 9 10\n");
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
                    goto PROC_READ_HIT_OVERRUN_END;
            }
        }

    if ( n % 10 != 0 )
    {
        llen += sprintf(str + llen, "\n");

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
            goto PROC_READ_HIT_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_HIT_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_hit(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;
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

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clean") == 0 )
        for ( i = 0; i < 12; i++ )
            *ROUT_FWD_HIT_STAT_TBL(i) = 0;

    return count;
}

static int proc_read_mac(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;
    unsigned int mac52, mac10;

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

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
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
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i;

    pstr = *start = page;

    __sync();

    llen = sprintf(str, "Interface VLAN Config:\n");
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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_def_map = DEFAULT_ITF_VLAN_MAP(0);
    for ( i = 0; i < MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES; i++, p_def_map++ )
    {
        def_map = *p_def_map;

        llen = sprintf(str, "  %d (0x%08X): inner vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_def_map, def_map.new_in_vci, def_map.vlan_member, def_map.dest_qos, def_map.outer_vlan_ix);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_ip_map = SRC_IP_VLAN_MAP(0);
    for ( i = 0; i < MAX_SRC_IP_VLAN_MAP_ENTRIES; i++, p_ip_map++ )
    {
        ip_map = *p_ip_map;

        llen = sprintf(str, "  %d (0x%08X): src ip - %d.%d.%d.%d, vci - 0x%04X, map - 0x%02X, outer vlan index - %d, dest qos - %d\n", i, (u32)p_ip_map, ip_map.src_ip >> 24, (ip_map.src_ip >> 16) & 0xFF, (ip_map.src_ip >> 8) & 0xFF, ip_map.src_ip & 0xFF, ip_map.new_in_vci, ip_map.vlan_member, ip_map.dest_qos, ip_map.outer_vlan_ix);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    p_ethtype_map = ETHTYPE_VLAN_MAP(0);
    for ( i = 0; i < MAX_ETHTYPE_VLAN_MAP_ENTRIES; i++, p_ethtype_map++ )
    {
        ethtype_map = *p_ethtype_map;

        llen = sprintf(str, "  %d (0x%08X): ethtype - 0x%04X, vci - 0x%04X, map - 0x%02X, dest qos - %d, outer vlan index - %d\n", i, (u32)p_ethtype_map, ethtype_map.ethtype, ethtype_map.new_in_vci, ethtype_map.vlan_member, ethtype_map.dest_qos, ethtype_map.outer_vlan_ix);

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
            goto PROC_READ_VLAN_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_VLAN_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_vlan(struct file *file, const char *buf, unsigned long count, void *data)
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

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_out_vlan(struct file *file, const char *buf, unsigned long count, void *data)
{
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

static INLINE void get_ip_port(char **p, int *len, unsigned int *val)
{
    int i;

    ppa_memset(val, 0, sizeof(*val) * 5);

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

    ppa_memset(val, 0, sizeof(*val) * PPA_ETH_ALEN);

    for ( i = 0; i < PPA_ETH_ALEN; i++ )
    {
        ignore_space(p, len);
        if ( !*len )
            break;
        val[i] = get_number(p, len, 1);
    }
}

static INLINE void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}

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
    len += sprintf(buf + len,          "      DSLWAN QID:        %d (conn %d)\n", plaction->dslwan_qid & 0xFF, (plaction->dslwan_qid >> 8) & 0xFF);

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
        len += sprintf(buf + len,      "      DSLWAN QID:   %d (conn %d)\n", paction->dslwan_qid & 0xFF, (paction->dslwan_qid >> 8) & 0xFF);
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



/*
 * ####################################
 *           Global Function
 * ####################################
 */

void get_ppe_hal_id(uint32_t *p_family,
                    uint32_t *p_type,
                    uint32_t *p_if,
                    uint32_t *p_mode,
                    uint32_t *p_major,
                    uint32_t *p_mid,
                    uint32_t *p_minor)
{
    if ( p_family )
        *p_family = VER_FAMILY;

    if ( p_type )
        *p_type = VER_DRTYPE;

    if ( p_if )
        *p_if = VER_INTERFACE;

    if ( p_mode )
        *p_mode = VER_ACCMODE;

    if ( p_major )
        *p_major = VER_MAJOR;

    if ( p_mid )
        *p_mid = VER_MID;

    if ( p_minor )
        *p_minor = VER_MINOR;
}

uint32_t get_firmware_id(uint32_t pp32_index,
                     uint32_t *p_family,
                     uint32_t *p_type,
                     uint32_t *p_if,
                     uint32_t *p_mode,
                     uint32_t *p_major,
                     uint32_t *p_minor)
{
    register struct fw_ver_id id;

    if( pp32_index != 0 ) return IFX_FAILURE;
    id = *FW_VER_ID;

    if ( p_family )
        *p_family = id.family;

    if ( p_type )
        *p_type = id.fwtype;

    if ( p_if )
        *p_if = id.interface;

    if ( p_mode )
        *p_mode = id.fwmode;

    if ( p_major )
        *p_major = id.major;

    if ( p_minor )
        *p_minor = id.minor;
    return IFX_SUCCESS;
}

uint32_t get_number_of_phys_port(void)
{
    return 8;
}

void get_phys_port_info(uint32_t port,
                        uint32_t *p_flags,
                        PPA_IFNAME ifname[PPA_IF_NAME_SIZE])
{
#if defined(CONFIG_IFX_PPA)
    char *str_ifname[] = {
        "eth0",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    };
    uint32_t flags[] = {
        ETH_PORTS_CFG->eth0_type == 2 ? IFX_PPA_PHYS_PORT_FLAGS_MODE_ETH_MIX_VALID : IFX_PPA_PHYS_PORT_FLAGS_MODE_ETH_LAN_VALID,
        ETH_PORTS_CFG->eth0_type == 0 ? IFX_PPA_PHYS_PORT_FLAGS_MODE_ATM_WAN_VALID : 0,
        IFX_PPA_PHYS_PORT_FLAGS_MODE_CPU_VALID,
        IFX_PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID | IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU0,
        IFX_PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID | IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU0,
        IFX_PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID | IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU1,
        IFX_PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID | IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU1,
        IFX_PPA_PHYS_PORT_FLAGS_MODE_EXT_LAN_VALID | IFX_PPA_PHYS_PORT_FLAGS_EXT_CPU1
    };

    if ( port >= sizeof(str_ifname) / sizeof(*str_ifname) )
    {
        if ( p_flags )
            *p_flags = 0;
        if ( ifname )
            *ifname = 0;
        return;
    }

    if ( p_flags )
    {
        *p_flags = flags[port];
        if ( ETH_DEFAULT_DEST_LIST(port)->out_itag_vld )
            *p_flags |= IFX_PPA_PHYS_PORT_FLAGS_OUTER_VLAN;
    }
    if ( ifname )
        strcpy(ifname, str_ifname[port]);
#endif
}

void get_max_route_entries(uint32_t *p_entry,
                           uint32_t *p_mc_entry)
{
    if ( p_entry )
        *p_entry = MAX_ROUTING_ENTRIES;

    if ( p_mc_entry )
        *p_mc_entry = MAX_WAN_MC_ENTRIES;
}

void get_max_bridging_entries(uint32_t *p_entry)
{
    if ( p_entry )
        *p_entry = MAX_BRIDGING_ENTRIES;
}

void set_wan_vlan_id(uint32_t vlan_id)
{
    ETH_PORTS_CFG->wan_vlanid_lo = vlan_id & ((1 << 12) - 1);
    ETH_PORTS_CFG->wan_vlanid_hi = (vlan_id >> 16) & ((1 << 12) - 1);
}

uint32_t get_wan_vlan_id(void)
{
    return (ETH_PORTS_CFG->wan_vlanid_hi << 16) | ETH_PORTS_CFG->wan_vlanid_lo;
}

//  if_type:
//    bit 0: LAN
//    bit 1: WAN
void set_if_type(uint32_t if_type,
                 uint32_t if_no)
{
    uint8_t if_type_template[4] = {2, 0, 1, 2}; //  0: LAN, 1: WAN, 2: MIX (new spec)

    if ( if_no == 0 )
        ETH_PORTS_CFG->eth0_type = if_type_template[if_type];
    else if ( if_no == 1 )
        ETH_PORTS_CFG->eth1_type = if_type_template[if_type];
}

uint32_t get_if_type(uint32_t if_no)
{
    uint32_t if_type_template[4] = {IFX_PPA_IF_TYPE_LAN, IFX_PPA_IF_TYPE_WAN, IFX_PPA_IF_TYPE_MIX, IFX_PPA_IF_NOT_FOUND};

    if ( if_no == 0 )
        return if_type_template[ETH_PORTS_CFG->eth0_type];
    else if ( if_no == 1 )
        return if_type_template[ETH_PORTS_CFG->eth1_type];
    else
        return IFX_PPA_IF_NOT_FOUND;
}

//  flags
//    bit 0: entry_num is valid
//    bit 1: mc_entry_num is valid
//    bit 2: f_ip_verify is valid
//    bit 3: f_tcpudp_verify is valid
//    bit 4: f_tcpudp_err_drop is valid
//    bit 5: f_drop_on_no_hit is valid
//    bit 6: f_mc_drop_on_no_hit is valid
void set_route_cfg(uint32_t f_is_lan,
                   uint32_t entry_num,
                   uint32_t mc_entry_num,
                   uint32_t f_ip_verify,
                   uint32_t f_tcpudp_verify,
                   uint32_t f_tcpudp_err_drop,
                   uint32_t f_drop_on_no_hit,
                   uint32_t f_mc_drop_on_no_hit,
                   uint32_t flags)
{
    //  LAN route config is only a little different
    struct wan_rout_tbl_cfg cfg;
    uint32_t *p_occupation;

    if ( entry_num < 1 )
        entry_num = 1;

    if ( mc_entry_num < 1 )
        mc_entry_num = 1;

    cfg = f_is_lan ? *(struct wan_rout_tbl_cfg *)LAN_ROUT_TBL_CFG : *WAN_ROUT_TBL_CFG;

    if ( (flags & IFX_PPA_SET_ROUTE_CFG_ENTRY_NUM) )
    {
        cfg.wan_rout_num = entry_num;

        if ( f_is_lan )
        {
            g_lan_routing_entries = entry_num;
            ((struct lan_rout_tbl_cfg *)&cfg)->lan_rout_off = g_lan_routing_entry_off = MAX_ROUTING_ENTRIES - entry_num;    //  in LAN cfg, this field is offset

            g_lan_routing_entry_occupation = g_routing_entry_occupation + (g_lan_routing_entry_off + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32;
            p_occupation = g_lan_routing_entry_occupation;
        }
        else
        {
            g_wan_routing_entries = entry_num;

            p_occupation = g_routing_entry_occupation;
        }

        ppa_memset(p_occupation, 0, (entry_num + BITSIZEOF_UINT32 - 1)  / BITSIZEOF_UINT32 * sizeof(uint32_t));
        if ( entry_num % BITSIZEOF_UINT32 )
        {
            p_occupation += entry_num / BITSIZEOF_UINT32;
            *p_occupation = ~0 ^ ((1 << (entry_num % BITSIZEOF_UINT32)) - 1);
        }
    }

    if ( !f_is_lan && (flags & IFX_PPA_SET_ROUTE_CFG_MC_ENTRY_NUM) )
    {
        g_wan_mc_entries = mc_entry_num;
        cfg.wan_rout_mc_num = mc_entry_num;

        if ( mc_entry_num % BITSIZEOF_UINT32 )
            g_wan_mc_entry_occupation[mc_entry_num / BITSIZEOF_UINT32] = ~0 ^ ((1 << (mc_entry_num % BITSIZEOF_UINT32)) - 1);
    }

    if ( (flags & IFX_PPA_SET_ROUTE_CFG_IP_VERIFY) )
        cfg.wan_ip_ver_en = f_ip_verify ? 1 : 0;

    if ( (flags & IFX_PPA_SET_ROUTE_CFG_TCPUDP_VERIFY) )
        cfg.wan_tcpdup_ver_en = f_tcpudp_verify ? 1 : 0;

    if ( (flags & IFX_PPA_SET_ROUTE_CFG_TCPUDP_ERR_DROP) )
        cfg.wan_tcpudp_err_drop = f_tcpudp_err_drop ? 1 : 0;

    if ( (flags & IFX_PPA_SET_ROUTE_CFG_DROP_ON_NOT_HIT) )
        cfg.wan_rout_drop = f_drop_on_no_hit ? 1 : 0;

    if ( !f_is_lan && (flags & IFX_PPA_SET_ROUTE_CFG_MC_DROP_ON_NOT_HIT) )
        cfg.wan_rout_mc_drop = f_mc_drop_on_no_hit ? 1 : 0;

    if ( f_is_lan )
        *(volatile struct wan_rout_tbl_cfg *)LAN_ROUT_TBL_CFG = cfg;
    else
        *WAN_ROUT_TBL_CFG = cfg;
}

void set_bridging_cfg(uint32_t entry_num,
                      uint32_t br_to_src_port_mask, uint32_t br_to_src_port_en,
                      uint32_t f_dest_vlan_en,
                      uint32_t f_src_vlan_en,
                      uint32_t f_mac_change_drop,
                      uint32_t flags)
{
    struct brg_tbl_cfg cfg;

    if ( entry_num < 1 )
        entry_num = 1;

    cfg = *BRG_TBL_CFG;

    if ( (flags & IFX_PPA_SET_BRIDGING_CFG_ENTRY_NUM) )
    {
        cfg.brg_entry_num = entry_num;
        g_bridging_entries = entry_num;

        ppa_memset(g_bridging_entry_occupation, 0, sizeof(g_bridging_entry_occupation));
        if ( entry_num % BITSIZEOF_UINT32 )
            g_bridging_entry_occupation[entry_num / BITSIZEOF_UINT32] = ~0 ^ ((1 << (entry_num % BITSIZEOF_UINT32)) - 1);
    }

//    if ( (flags & IFX_PPA_SET_BRIDGING_CFG_BR_TO_SRC_PORT_EN) )
//        cfg.br_to_src_port_en = (cfg.br_to_src_port_en & ~br_to_src_port_mask) | (br_to_src_port_en & br_to_src_port_mask);

//    if ( (flags & IFX_PPA_SET_BRIDGING_CFG_DEST_VLAN_EN) )
//        cfg.dest_vlan_en = f_dest_vlan_en ? 1 : 0;

//    if ( (flags & IFX_PPA_SET_BRIDGING_CFG_SRC_VLAN_EN) )
//        cfg.src_vlan_en = f_src_vlan_en ? 1 : 0;

//    if ( (flags & IFX_PPA_SET_BRIDGING_CFG_MAC_CHANGE_DROP) )
//        cfg.mac_change_drop = f_mac_change_drop ? 1 : 0;

    *BRG_TBL_CFG = cfg;
}

void set_fast_mode(uint32_t mode,
                   uint32_t flags)
{
    if ( (flags & IFX_PPA_SET_FAST_MODE_CPU1) )
        GEN_MODE_CFG->cpu1_fast_mode = (mode & IFX_PPA_SET_FAST_MODE_CPU1_DIRECT) ? 1 : 0;

    if ( (flags & IFX_PPA_SET_FAST_MODE_ETH1) )
        GEN_MODE_CFG->wan_fast_mode = (mode & IFX_PPA_SET_FAST_MODE_ETH1_DIRECT) ? 1 : 0;

//    reconfig_dma_channel(GEN_MODE_CFG->cpu1_fast_mode && GEN_MODE_CFG->wan_fast_mode);
}

void set_if_wfq(uint32_t if_wfq,
                uint32_t if_no)
{
    if ( if_no == 1 )
        GEN_MODE_CFG->etx_wfq = if_wfq;
}

void set_dplus_wfq(uint32_t wfq)
{
    GEN_MODE_CFG->dplus_wfq = wfq;
}

void set_fastpath_wfq(uint32_t wfq)
{
    GEN_MODE_CFG->fast_path_wfq = wfq;
}

void get_acc_mode(uint32_t f_is_lan,
                  uint32_t *p_acc_mode)
{
    if ( p_acc_mode )
        *p_acc_mode = f_is_lan ? GEN_MODE_CFG->lan_acc_mode : GEN_MODE_CFG->wan_acc_mode;
}

//  acc_mode:
//    0: no acceleration
//    2: routing acceleration
void set_acc_mode(uint32_t f_is_lan,
                  uint32_t acc_mode)
{
    if ( f_is_lan )
        GEN_MODE_CFG->lan_acc_mode = acc_mode;
    else
        GEN_MODE_CFG->wan_acc_mode = acc_mode;
}

void set_default_dest_list(uint32_t uc_dest_list,
                           uint32_t mc_dest_list,
                           uint32_t if_no)
{
    struct eth_default_dest_list cfg;

    ppa_lock_get(&g_itf_cfg_lock);
    cfg = *ETH_DEFAULT_DEST_LIST(if_no);
    cfg.unknown_mc_dest_list = mc_dest_list;
    cfg.unknown_uc_dest_list = uc_dest_list;
    *ETH_DEFAULT_DEST_LIST(if_no) = cfg;
    ppa_lock_release(&g_itf_cfg_lock);
}

#if defined(ENABLE_CLEVER_BRG_VLAN_SUPPORT) && ENABLE_CLEVER_BRG_VLAN_SUPPORT
int add_brg_vlan_support(uint32_t ig_if_no,
                         uint32_t ig_cond_type,
                         uint32_t ig_criteria,  //  ig_cond_type - 0: bypass, 1: src IP, 2: ETH type, 3, VLAN tag
                         uint32_t ig_vlan_rm_en,
                         uint32_t eg_if_no,
                         uint32_t eg_vlan_ins_en,
                         uint32_t eg_new_vci)
{
    struct itf_cfg ig_cfg, eg_cfg;
    int32_t entry, empty_entry;
    int f_found;

    ppa_lock_get(&g_itf_cfg_lock);

    ig_cfg = *ETH_DEFAULT_DEST_LIST(ig_if_no);

    switch ( ig_cond_type )
    {
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP:
        f_found = 0;
        empty_entry = -1;
        for ( entry = MAX_SRC_IP_VLAN_MAP_ENTRIES - 1; entry >= 0; entry-- )
            if ( !g_ip_vlan_map_counter[entry] )
                empty_entry = entry;
            else if ( ig_criteria == IP_VLAN_MAP(entry)->src_ip )
            {
                f_found = 1;
                break;
            }
        if ( !f_found )
        {
            if ( empty_entry < 0 )
                //  no entry
                goto ADD_BRG_VLAN_SUPPORT_ERR;
            else
            {
                if ( eg_vlan_ins_en )
                    IP_VLAN_MAP(empty_entry)->new_vci = eg_new_vci;
                IP_VLAN_MAP(empty_entry)->vlan_port_map |= 1 << eg_if_no;
                IP_VLAN_MAP(empty_entry)->src_ip = ig_criteria;
            }
            g_ip_vlan_map_counter[empty_entry]++;
        }
        else
        {
            IP_VLAN_MAP(entry)->vlan_port_map |= 1 << eg_if_no;
            g_ip_vlan_map_counter[entry]++;
        }
        ig_cfg.src_ip_vlan_en = 1;
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE:
        f_found = 0;
        empty_entry = -1;
        for ( entry = MAX_ETHTYPE_VLAN_MAP_ENTRIES - 1; entry >= 0; entry-- )
            if ( !g_ethtype_vlan_map_counter[entry] )
                empty_entry = entry;
            else if ( ig_criteria == ETHTYPE_VLAN_MAP(entry)->ethtype )
            {
                f_found = 1;
                break;
            }
        if ( !f_found )
        {
            if ( empty_entry < 0 )
                //  no entry
                goto ADD_BRG_VLAN_SUPPORT_ERR;
            else
            {
                if ( eg_vlan_ins_en )
                    ETHTYPE_VLAN_MAP(empty_entry)->new_vci = eg_new_vci;
                ETHTYPE_VLAN_MAP(empty_entry)->vlan_port_map |= 1 << eg_if_no;
                ETHTYPE_VLAN_MAP(empty_entry)->ethtype = ig_criteria;
            }
            g_ethtype_vlan_map_counter[empty_entry]++;
        }
        else
        {
            ETHTYPE_VLAN_MAP(entry)->vlan_port_map |= 1 << eg_if_no;
            g_ethtype_vlan_map_counter[entry]++;
        }
        ig_cfg.ethtype_vlan_en = 1;
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN:
        f_found = 0;
        empty_entry = -1;
        for ( entry = MAX_VLAN_VLAN_MAP_ENTRIES - 1; entry >= 0; entry-- )
            if ( !g_vlan_vlan_map_counter[entry] )
                empty_entry = entry;
            else if ( ig_criteria == VLAN_VLAN_MAP(entry)->in_vlan_tag )
            {
                f_found = 1;
                break;
            }
        if ( !f_found )
        {
            if ( empty_entry < 0 )
                //  no entry
                goto ADD_BRG_VLAN_SUPPORT_ERR;
            else
            {
                if ( eg_vlan_ins_en )
                    VLAN_VLAN_MAP(empty_entry)->new_vci = eg_new_vci;
                VLAN_VLAN_MAP(empty_entry)->vlan_port_map |= 1 << eg_if_no;
                VLAN_VLAN_MAP(empty_entry)->in_vlan_tag = ig_criteria;
            }
            g_vlan_vlan_map_counter[empty_entry]++;
        }
        else
        {
            VLAN_VLAN_MAP(entry)->vlan_port_map |= 1 << eg_if_no;
            g_vlan_vlan_map_counter[entry]++;
        }
        ig_cfg.vlan_tag_vlan_en = 1;
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF:
    default:
        if ( (g_default_itf_vlan_map_occupation & (1 << ig_if_no)) )
            goto ADD_BRG_VLAN_SUPPORT_ERR;
        g_default_itf_vlan_map_occupation |= 1 << ig_if_no;
        if ( eg_vlan_ins_en )
            DEFAULT_ITF_VLAN_MAP(ig_if_no)->new_vci = eg_new_vci;
        DEFAULT_ITF_VLAN_MAP(ig_if_no)->vlan_port_map |= 1 << eg_if_no;
    }

    if ( ig_if_no == eg_if_no )
    {
        if ( eg_vlan_ins_en )
            ig_cfg.port_eg_vlan_ins = 1;
        if ( ig_vlan_rm_en )
            ig_cfg.port_eg_vlan_rm = 1;
        ig_cfg.same_port_en = 1;
    }
    else
    {
        eg_cfg = *ETH_DEFAULT_DEST_LIST(eg_if_no);
        if ( eg_vlan_ins_en )
            eg_cfg.port_eg_vlan_ins = 1;
        if ( ig_vlan_rm_en )
            eg_cfg.port_eg_vlan_rm = 1;
        *ETH_DEFAULT_DEST_LIST(eg_if_no) = eg_cfg;
        g_itf_cfg_eg_counter[eg_if_no]++;
    }

    ig_cfg.port_vlan_aware = 1;
    *ETH_DEFAULT_DEST_LIST(ig_if_no) = ig_cfg;
    g_itf_cfg_ig_counter[ig_if_no]++;

    ppa_lock_release(&g_itf_cfg_lock);
    return 0;

ADD_BRG_VLAN_SUPPORT_ERR:
    ppa_lock_release(&g_itf_cfg_lock);
    return -1;
}
#else

void set_bridge_if_vlan_config(uint32_t if_no,
                               uint32_t f_eg_vlan_insert,
                               uint32_t f_eg_vlan_remove,
                               uint32_t f_ig_vlan_aware,
                               uint32_t f_ig_src_ip_based,
                               uint32_t f_ig_eth_type_based,
                               uint32_t f_ig_vlanid_based,
                               uint32_t f_ig_port_based,
                               uint32_t f_eg_out_vlan_insert,
                               uint32_t f_eg_out_vlan_remove,
                               uint32_t f_ig_out_vlan_aware /* reserved */)
{
    struct eth_default_dest_list cfg;
    unsigned int in_etag_en, out_etag_en;

    in_etag_en = (f_eg_vlan_insert ? 2 : 0) | (f_eg_vlan_remove ? 1 : 0);
    out_etag_en = (f_eg_out_vlan_insert ? 2 : 0) | (f_eg_out_vlan_remove ? 1 : 0);

    ppa_lock_get(&g_itf_cfg_lock);
    cfg = *ETH_DEFAULT_DEST_LIST(if_no);
    cfg.in_etag_en          = in_etag_en;
    cfg.srcip_vlan_en       = f_ig_src_ip_based ? 1 : 0;
    cfg.ethtype_vlan_en     = f_ig_eth_type_based ? 1 : 0;
    cfg.vlan_vlan_en        = f_ig_vlanid_based ? 1 : 0;
    cfg.vlan_en             = f_ig_vlan_aware ? 1 : 0;
    cfg.port_en             = f_ig_port_based ? 1 : 0;
    //cfg.out_itag_vld        = f_ig_out_vlan_aware ? 1 : 0;
    cfg.out_etag_en         = out_etag_en;
    *ETH_DEFAULT_DEST_LIST(if_no) = cfg;
    ppa_lock_release(&g_itf_cfg_lock);
}

//void set_bridge_if_outer_vlan_config(uint32_t if_no,
//                                     uint32_t f_ig_outer_vlan_en,
//                                     uint32_t f_eg_outer_vlan_insert,
//                                     uint32_t outer_vlan_ix,
//                                     uint32_t f_eg_outer_vlan_remove)
//{
//    struct eth_default_dest_list cfg;
//    unsigned int out_etag_en;
//
//    out_etag_en = (f_eg_outer_vlan_insert ? 2 : 0) | (f_eg_outer_vlan_remove ? 1 : 0);
//
//    ppa_lock_get(&g_itf_cfg_lock);
//    cfg = *ETH_DEFAULT_DEST_LIST(if_no);
//    cfg.out_itag_vld    = f_ig_outer_vlan_en ? 1 : 0;
//    cfg.out_etag_en     = out_etag_en;
//    *ETH_DEFAULT_DEST_LIST(if_no) = cfg;
//    if ( f_eg_outer_vlan_insert )
//        DEFAULT_ITF_VLAN_MAP(if_no)->outer_vlan_ix = outer_vlan_ix;
//    ppa_lock_release(&g_itf_cfg_lock);
//}

void get_bridge_if_vlan_config(uint32_t if_no,
                               uint32_t *f_eg_vlan_insert,
                               uint32_t *f_eg_vlan_remove,
                               uint32_t *f_ig_vlan_aware,
                               uint32_t *f_ig_src_ip_based,
                               uint32_t *f_ig_eth_type_based,
                               uint32_t *f_ig_vlanid_based,
                               uint32_t *f_ig_port_based,
                               uint32_t *f_eg_out_vlan_insert,
                               uint32_t *f_eg_out_vlan_remove,
                               uint32_t *f_ig_out_vlan_aware /* reserved */)
{
    struct eth_default_dest_list cfg;

    ppa_lock_get(&g_itf_cfg_lock);
    cfg = *ETH_DEFAULT_DEST_LIST(if_no);
    ppa_lock_release(&g_itf_cfg_lock);

    *f_eg_vlan_insert       = (cfg.in_etag_en & 2) ? 1 : 0;
    *f_eg_vlan_remove       = (cfg.in_etag_en & 1) ? 1 : 0;
    *f_ig_vlan_aware        = cfg.vlan_en;
    *f_ig_src_ip_based      = cfg.srcip_vlan_en;
    *f_ig_eth_type_based    = cfg.ethtype_vlan_en;
    *f_ig_vlanid_based      = cfg.vlan_vlan_en;
    *f_ig_port_based        = cfg.port_en;
    *f_eg_out_vlan_insert   = (cfg.out_etag_en & 2) ? 1 : 0;
    *f_eg_out_vlan_remove   = (cfg.out_etag_en & 1) ? 1 : 0;
    //*f_ig_out_vlan_aware    = cfg.out_itag_vld;
}

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
int32_t add_vlan_map(uint32_t ig_criteria_type,
                     uint32_t ig_criteria,
                     uint32_t new_in_vci,
                     uint32_t vlan_port_map)
#else
int32_t add_vlan_map(uint32_t ig_criteria_type,
                     uint32_t ig_criteria,
                     uint32_t new_in_vci,
                     uint32_t dest_qos,
                     uint32_t outer_vlan_ix,
                     uint32_t in_out_etag_ctrl,
                     uint32_t vlan_port_map)
#endif
{
    int32_t entry, empty_entry = -1;
    uint32_t bit;
    int32_t max_entry;
    uint32_t *p_occupation;
    uint32_t base;
    uint32_t size;
    volatile struct src_ip_vlan_map *p_table;
#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    uint32_t dest_qos = 0;
    uint32_t outer_vlan_ix = 0;
    uint32_t in_out_etag_ctrl = 0;
#endif

    dbg("add_vlan_map:%d %d %d %x", ig_criteria_type, ig_criteria, new_in_vci, vlan_port_map);
    switch ( ig_criteria_type )
    {
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF:
        if ( ig_criteria >= MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES )
            return -1;
        ppa_lock_get(&g_itf_cfg_lock);
        if ( (g_default_itf_vlan_map_occupation & (1 << ig_criteria)) )
        {
            ppa_lock_release(&g_itf_cfg_lock);
            return -1;
        }
        else
        {
            g_default_itf_vlan_map_occupation |= 1 << ig_criteria;
            DEFAULT_ITF_VLAN_MAP(ig_criteria)->new_in_vci   = new_in_vci;
            DEFAULT_ITF_VLAN_MAP(ig_criteria)->vlan_member  = vlan_port_map;
            DEFAULT_ITF_VLAN_MAP(ig_criteria)->outer_vlan_ix = outer_vlan_ix;
            DEFAULT_ITF_VLAN_MAP(ig_criteria)->dest_qos = dest_qos;
            ppa_lock_release(&g_itf_cfg_lock);
            return 0;
        }
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP:
        p_occupation = &g_ip_vlan_map_occupation;
        max_entry = MAX_SRC_IP_VLAN_MAP_ENTRIES;
        //p_table = SRC_IP_VLAN_MAP(0);
        base = (uint32_t)SRC_IP_VLAN_MAP(0);
        size = sizeof(*SRC_IP_VLAN_MAP(0));
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE:
        p_occupation = &g_ethtype_vlan_map_occupation;
        max_entry = MAX_ETHTYPE_VLAN_MAP_ENTRIES;
        //p_table = (volatile struct src_ip_vlan_map *)ETHTYPE_VLAN_MAP(0);
        base = (uint32_t)ETHTYPE_VLAN_MAP(0);
        size = sizeof(*ETHTYPE_VLAN_MAP(0));
        ig_criteria &= 0xFFFF;  //  ethtype
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN:
        p_occupation = &g_vlan_vlan_map_occupation;
        max_entry = MAX_VLAN_VLAN_MAP_ENTRIES;
        //p_table = (volatile struct src_ip_vlan_map *)VLAN_VLAN_MAP(0);
        base = (uint32_t)VLAN_VLAN_MAP(0);
        size = sizeof(*VLAN_VLAN_MAP(0));
        ig_criteria = (ig_criteria & 0x0FFF) << 16; //  in_vlan_tag
        break;
    default:
        return -1;
    }

    ppa_lock_get(&g_itf_cfg_lock);

    //  find existing entry and empty entry
    for ( entry = max_entry - 1, bit = 1 << entry; entry >= 0; entry--, bit >>= 1 )
    {
        if ( !(*p_occupation & bit) )
            empty_entry = entry;
        else
        {
            p_table = (volatile struct src_ip_vlan_map *)(base + size * entry);
            if ( p_table->src_ip == ig_criteria )
            {
                ppa_lock_release(&g_itf_cfg_lock);
                return -1;
            }
        }
        }

    //  no empty entry
    if ( empty_entry < 0 )
    {
        ppa_lock_release(&g_itf_cfg_lock);
        return -1;
    }

    *p_occupation |= 1 << empty_entry;

    p_table = (volatile struct src_ip_vlan_map *)(base + size * empty_entry);
    p_table->src_ip         = ig_criteria;
    p_table->new_in_vci     = new_in_vci;
    p_table->dest_qos       = dest_qos;
    p_table->outer_vlan_ix  = outer_vlan_ix;
    if ( ig_criteria_type == IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN )
        ((volatile struct vlan_vlan_map *)p_table)->in_out_etag_ctrl = in_out_etag_ctrl;
    p_table->vlan_member    = vlan_port_map;

    ppa_lock_release(&g_itf_cfg_lock);

    return 0;
}

void del_vlan_map(uint32_t ig_criteria_type,
                  uint32_t ig_criteria)
{
    int32_t entry;
    uint32_t bit;
    int32_t max_entry;
    uint32_t *p_occupation;
    uint32_t base;
    uint32_t size;
    volatile struct src_ip_vlan_map *p_table;

    dbg("del_vlan_map:%d\n", ig_criteria_type);
    switch ( ig_criteria_type )
    {
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF:
        ppa_lock_get(&g_itf_cfg_lock);
        DEFAULT_ITF_VLAN_MAP(ig_criteria)->vlan_member  = 0;
        DEFAULT_ITF_VLAN_MAP(ig_criteria)->new_in_vci   = 0;
        g_default_itf_vlan_map_occupation &= ~(1 << ig_criteria);
        ppa_lock_release(&g_itf_cfg_lock);
        return;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP:
        p_occupation = &g_ip_vlan_map_occupation;
        max_entry = MAX_SRC_IP_VLAN_MAP_ENTRIES;
        //p_table = SRC_IP_VLAN_MAP(0);
        base = (uint32_t)SRC_IP_VLAN_MAP(0);
        size = sizeof(*SRC_IP_VLAN_MAP(0));
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE:
        p_occupation = &g_ethtype_vlan_map_occupation;
        max_entry = MAX_ETHTYPE_VLAN_MAP_ENTRIES;
        //p_table = (volatile struct src_ip_vlan_map *)ETHTYPE_VLAN_MAP(0);
        base = (uint32_t)ETHTYPE_VLAN_MAP(0);
        size = sizeof(*ETHTYPE_VLAN_MAP(0));
        ig_criteria &= 0xFFFF;  //  ethtype
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN:
        p_occupation = &g_vlan_vlan_map_occupation;
        max_entry = MAX_VLAN_VLAN_MAP_ENTRIES;
        //p_table = (volatile struct src_ip_vlan_map *)VLAN_VLAN_MAP(0);
        base = (uint32_t)VLAN_VLAN_MAP(0);
        size = sizeof(*VLAN_VLAN_MAP(0));
        ig_criteria = (ig_criteria & 0x0FFF) << 16; //  in_vlan_tag
        break;
    default:
        return;
    }

    ppa_lock_get(&g_itf_cfg_lock);

    for ( entry = 0, bit = 1 << entry; entry < max_entry; entry++, bit <<= 1 )
    {
        p_table = (volatile struct src_ip_vlan_map *)(base + size * entry);
        if ( (*p_occupation & bit) && p_table->src_ip == ig_criteria )
        {
            p_table->src_ip         = 0;
            //  modified by Lance, prevent dropping packet
            //p_table->vlan_member    = 0;
            p_table->vlan_member    = 0xFF;
            p_table->new_in_vci     = 0;
            p_table->dest_qos       = 0;
            p_table->outer_vlan_ix  = 0;
            if ( ig_criteria_type == IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN )
                ((volatile struct vlan_vlan_map *)p_table)->in_out_etag_ctrl = 0;
            *p_occupation &= ~(1 << entry);
            break;
        }
    }

    ppa_lock_release(&g_itf_cfg_lock);
}

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
int32_t get_vlan_map(uint32_t ig_criteria_type,
                     uint32_t entry,
                     uint32_t *ig_criteria,
                     uint32_t *new_in_vci,
                     uint32_t *vlan_port_map)
#else
int32_t get_vlan_map(uint32_t ig_criteria_type,
                     uint32_t entry,
                     uint32_t *ig_criteria,
                     uint32_t *new_in_vci,
                     uint32_t *dest_qos,
                     uint32_t *outer_vlan_ix,
                     uint32_t *in_out_etag_ctrl,
                     uint32_t *vlan_port_map)
#endif
{
    int ret = -1;   //  -1: stop, 0: empty entry, 1: get entry
#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    uint32_t dest_qos[1];
    uint32_t outer_vlan_ix[1];
    uint32_t in_out_etag_ctrl[1];
#endif

    ppa_lock_get(&g_itf_cfg_lock);
    switch ( ig_criteria_type )
    {
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF:
        if ( entry < MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES )
        {
            if ( (g_default_itf_vlan_map_occupation & (1 << entry)) )
            {
                *ig_criteria    = entry;  //sgh
                *new_in_vci     = DEFAULT_ITF_VLAN_MAP(entry)->new_in_vci;
                *dest_qos       = DEFAULT_ITF_VLAN_MAP(entry)->dest_qos;
                *outer_vlan_ix  = DEFAULT_ITF_VLAN_MAP(entry)->outer_vlan_ix;
                *vlan_port_map  = DEFAULT_ITF_VLAN_MAP(entry)->vlan_member;
                ret = 1;
            }
            else
                ret = 0;
        }
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP:
        if ( entry < MAX_SRC_IP_VLAN_MAP_ENTRIES )
        {
            if ( (g_ip_vlan_map_occupation & (1 << entry)) )
            {
                *ig_criteria    = SRC_IP_VLAN_MAP(entry)->src_ip;
                *new_in_vci     = SRC_IP_VLAN_MAP(entry)->new_in_vci;
                *dest_qos       = SRC_IP_VLAN_MAP(entry)->dest_qos;
                *outer_vlan_ix  = SRC_IP_VLAN_MAP(entry)->outer_vlan_ix;
                *vlan_port_map  = SRC_IP_VLAN_MAP(entry)->vlan_member;
                ret = 1;
            }
            else
               ret = 0;
        }
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE:
        if ( entry < MAX_ETHTYPE_VLAN_MAP_ENTRIES )
        {
            if ( (g_ethtype_vlan_map_occupation & (1 << entry)) )
            {
                *ig_criteria    = ETHTYPE_VLAN_MAP(entry)->ethtype;
                *new_in_vci     = ETHTYPE_VLAN_MAP(entry)->new_in_vci;
                *dest_qos       = ETHTYPE_VLAN_MAP(entry)->dest_qos;
                *outer_vlan_ix  = ETHTYPE_VLAN_MAP(entry)->outer_vlan_ix;
                *vlan_port_map  = ETHTYPE_VLAN_MAP(entry)->vlan_member;
                ret = 1;
            }
            else
                ret = 0;
        }
        break;
    case IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN:
        if ( entry < MAX_VLAN_VLAN_MAP_ENTRIES )
        {
            if ( (g_vlan_vlan_map_occupation & (1 << entry)) )
            {
                *ig_criteria      = VLAN_VLAN_MAP(entry)->in_vlan_tag;
                *new_in_vci       = VLAN_VLAN_MAP(entry)->new_in_vci;
                *dest_qos         = VLAN_VLAN_MAP(entry)->dest_qos;
                *outer_vlan_ix    = VLAN_VLAN_MAP(entry)->outer_vlan_ix;
                *in_out_etag_ctrl = VLAN_VLAN_MAP(entry)->in_out_etag_ctrl;
                *vlan_port_map    = VLAN_VLAN_MAP(entry)->vlan_member;
                ret = 1;
            }
            else
                ret = 0;
        }
    }
    ppa_lock_release(&g_itf_cfg_lock);

    return ret;
}

void del_all_vlan_map(void)
{
    int i;
    volatile struct src_ip_vlan_map *p_table;
    volatile struct default_itf_vlan_map *p_default_table;
    volatile struct vlan_vlan_map *p_vlan_table;

    ppa_lock_get(&g_itf_cfg_lock);

    g_default_itf_vlan_map_occupation = 0;
    g_ip_vlan_map_occupation = 0;
    g_ethtype_vlan_map_occupation = 0;
    g_vlan_vlan_map_occupation = 0;

    p_default_table = DEFAULT_ITF_VLAN_MAP(0);
    for(i = 0; i < MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES;i++)
    {
        p_default_table[i].new_in_vci = 0;
        p_default_table[i].vlan_member = 0xFF;
        p_default_table[i].dest_qos = 0;
        p_default_table[i].outer_vlan_ix = 0;
    }
    p_table = SRC_IP_VLAN_MAP(0);

    for(i = 0; i < MAX_SRC_IP_VLAN_MAP_ENTRIES;i++)
    {
        p_table[i].src_ip       = 0;
        p_table[i].vlan_member  = 0xFF;
        p_table[i].new_in_vci   = 0;
        p_table[i].dest_qos     = 0;
        p_table[i].outer_vlan_ix = 0;
    }
    p_table = (volatile struct src_ip_vlan_map *)ETHTYPE_VLAN_MAP(0);

    for(i = 0; i < MAX_ETHTYPE_VLAN_MAP_ENTRIES;i++)
    {
        p_table[i].src_ip       = 0;
        p_table[i].vlan_member  = 0xFF;
        p_table[i].new_in_vci   = 0;
        p_table[i].dest_qos     = 0;
        p_table[i].outer_vlan_ix = 0;
    }

    p_vlan_table = (volatile struct vlan_vlan_map *)VLAN_VLAN_MAP(0);

    for(i = 0; i < MAX_VLAN_VLAN_MAP_ENTRIES;i++)
    {
        p_vlan_table[i].in_vlan_tag = 0;
        p_vlan_table[i].vlan_member = 0xFF;
        p_vlan_table[i].new_in_vci  = 0;
        p_vlan_table[i].dest_qos    = 0;
        p_vlan_table[i].outer_vlan_ix    = 0;
        p_vlan_table[i].in_out_etag_ctrl = 0;
    }

    //clean up interface config
    for ( i = 0; i < 8; i++ )
        *(volatile u32 *)ETH_DEFAULT_DEST_LIST(i) = 0x00040404;
    ppa_lock_release(&g_itf_cfg_lock);
}

#endif

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
int32_t add_routing_entry(uint32_t f_is_lan,
                          uint32_t src_ip,
                          uint32_t src_port,
                          uint32_t dst_ip,
                          uint32_t dst_port,
                          uint32_t route_type,
                          uint32_t new_ip,
                          uint32_t new_port,
                          uint8_t  new_mac[PPA_ETH_ALEN],
                          uint32_t new_src_mac_ix,
                          uint32_t mtu_ix,
                          uint32_t f_new_dscp_enable,
                          uint32_t new_dscp,
                          uint32_t f_vlan_ins_enable,
                          uint32_t new_vci,
                          uint32_t f_vlan_rm_enable,
                          uint32_t pppoe_mode,
                          uint32_t pppoe_ix,
                          uint32_t f_out_vlan_ins_enable,
                          uint32_t out_vlan_ix,
                          uint32_t f_out_vlan_rm_enable,
                          uint32_t dslwan_qid,
                          uint32_t dest_list,
                          uint32_t *p_entry)
#else
int32_t add_routing_entry(uint32_t f_is_lan,
                          uint32_t src_ip,
                          uint32_t src_port,
                          uint32_t dst_ip,
                          uint32_t dst_port,
                          uint32_t f_is_tcp,    //  1: TCP, 0: UDP
                          uint32_t route_type,
                          uint32_t new_ip,
                          uint32_t new_port,
                          uint8_t  new_mac[PPA_ETH_ALEN],
                          uint32_t new_src_mac_ix,
                          uint32_t mtu_ix,
                          uint32_t f_new_dscp_enable,
                          uint32_t new_dscp,
                          uint32_t f_vlan_ins_enable,
                          uint32_t new_vci,
                          uint32_t f_vlan_rm_enable,
                          uint32_t pppoe_mode,
                          uint32_t pppoe_ix,
                          uint32_t f_out_vlan_ins_enable,
                          uint32_t out_vlan_ix,
                          uint32_t f_out_vlan_rm_enable,
                          uint32_t dslwan_qid,
                          uint32_t dest_list,
                          uint32_t *p_entry)
#endif
{
    PPA_LOCK *p_lock;
    uint32_t entry;
    struct rout_forward_action_tbl action = {0};
    struct rout_forward_compare_tbl compare;
    uint32_t *p_occupation;
    uint32_t entries;
    uint32_t i;
    uint32_t bit;

#if 0
    printk("add_routing_entry: \n");
    printk("  f_is_lan              = %d\n", f_is_lan);
    printk("  src_ip                = %d.%d.%d.%d\n", src_ip >> 24, (src_ip >> 16) & 0xFF, (src_ip >> 8) & 0xFF, src_ip & 0xFF);
    printk("  src_port              = %d\n", src_port);
    printk("  dst_ip                = %d.%d.%d.%d\n", dst_ip >> 24, (dst_ip >> 16) & 0xFF, (dst_ip >> 8) & 0xFF, dst_ip & 0xFF);
    printk("  dst_port              = %d\n", dst_port);
  #if !defined(CONFIG_DANUBE_PPA_A4_API) && !defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    printk("  f_is_tcp              = %d\n", f_is_tcp);
  #endif
    printk("  route_type            = %d\n", route_type);
    printk("  new_ip                = %d.%d.%d.%d\n", new_ip >> 24, (new_ip >> 16) & 0xFF, (new_ip >> 8) & 0xFF, new_ip & 0xFF);
    printk("  new_port              = %d\n", new_port);
    printk("  new_mac               = %02x:%02x:%02x:%02x:%02x:%02x\n", (uint32_t)new_mac[0], (uint32_t)new_mac[1], (uint32_t)new_mac[2], (uint32_t)new_mac[3], (uint32_t)new_mac[4], (uint32_t)new_mac[5]);
    printk("  new_src_mac_ix        = %d (%02x:%02x:%02x:%02x:%02x:%02x)\n", new_src_mac_ix, ROUT_MAC_CFG_TBL(new_src_mac_ix)[0] >> 24, (ROUT_MAC_CFG_TBL(new_src_mac_ix)[0] >> 16) & 0xFF, (ROUT_MAC_CFG_TBL(new_src_mac_ix)[0] >> 8) & 0xFF, ROUT_MAC_CFG_TBL(new_src_mac_ix)[0] & 0xFF, (ROUT_MAC_CFG_TBL(new_src_mac_ix)[1] >> 24) & 0xFF, (ROUT_MAC_CFG_TBL(new_src_mac_ix)[1] >> 16) & 0xFF);
    printk("  mtu_ix                = %d (%d)\n", mtu_ix, *MTU_CFG_TBL(mtu_ix));
    printk("  f_new_dscp_enable     = %d\n", f_new_dscp_enable);
    printk("  new_dscp              = %d\n", new_dscp);
    printk("  f_vlan_ins_enable     = %d\n", f_vlan_ins_enable);
    printk("  new_vci               = %04x\n", new_vci);
    printk("  f_vlan_rm_enable      = %d\n", f_vlan_rm_enable);
    printk("  pppoe_mode            = %d\n", pppoe_mode);
    if ( f_is_lan )
        printk("  pppoe_ix              = %d (%d)\n", pppoe_ix, *PPPOE_CFG_TBL(pppoe_ix));
    else
        printk("  pppoe_ix              = %d\n", pppoe_ix);
    printk("  f_out_vlan_ins_enable = %d\n", f_out_vlan_ins_enable);
    printk("  out_vlan_ix           = %04x\n", out_vlan_ix);
    printk("  f_out_vlan_rm_enable  = %d\n", f_out_vlan_rm_enable);
    if ( f_is_lan )
        printk("  dslwan_qid            = %d (conn %d)\n", dslwan_qid & 0xFF, (dslwan_qid >> 8) & 0xFF);
    printk("  dest_list             = %02X\n", dest_list);
    printk("  p_entry               = %08X\n", (uint32_t)p_entry);
#endif

    if ( f_is_lan )
    {
        p_lock = &g_lan_routing_lock;
        p_occupation = g_lan_routing_entry_occupation;
        entries = g_lan_routing_entries;
        entry = g_lan_routing_entry_off;
    }
    else
    {
        p_lock = &g_wan_routing_lock;
        p_occupation = g_routing_entry_occupation;
        entries = g_wan_routing_entries;
        entry = 0;
    }

    entries = (entries + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32;

    ppa_lock_get(p_lock);

    for ( i = 0; i < entries; i++ )
        if ( p_occupation[i] != ~0 )
            goto ADD_ROUTING_ENTRY_GOON;
    //  no empty entry
    ppa_lock_release(p_lock);
    return IFX_EAGAIN;

ADD_ROUTING_ENTRY_GOON:
    entry += i * BITSIZEOF_UINT32;
    bit = 1;
    while ( (p_occupation[i] & bit) )
    {
        bit <<= 1;
        entry++;
    }
    p_occupation[i] |= bit;

    action.new_port           = new_port;
    action.new_dest_mac54     = (((uint32_t)new_mac[0] & 0xFF) << 8) | ((uint32_t)new_mac[1] & 0xFF);
    action.new_dest_mac30     = (((uint32_t)new_mac[2] & 0xFF) << 24) | (((uint32_t)new_mac[3] & 0xFF) << 16) | (((uint32_t)new_mac[4] & 0xFF) << 8) | ((uint32_t)new_mac[5] & 0xFF);
    action.new_ip             = new_ip;
    action.rout_type          = route_type;
    action.new_dscp           = new_dscp;
    action.mtu_ix             = mtu_ix < MAX_MTU_ENTRIES ? mtu_ix : 0;
    action.in_vlan_ins        = f_vlan_ins_enable ? 1 : 0;
    action.in_vlan_rm         = f_vlan_rm_enable ? 1 : 0;
    action.new_dscp_en        = f_new_dscp_enable ? 1 : 0;
#if !defined(CONFIG_DANUBE_PPA_A4_API) && !defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    action.protocol           = f_is_tcp ? 1 : 0;
#endif
    action.dest_list          = IFX_PPA_DEST_LIST_CPU0; //  disable action first by pass packet to CPU0
    action.pppoe_mode         = pppoe_mode ? 1 : 0;
    if ( f_is_lan && pppoe_mode )
        action.pppoe_ix       = pppoe_ix < MAX_PPPOE_ENTRIES ? pppoe_ix : 0;
    action.new_src_mac_ix     = new_src_mac_ix < MAX_MAC_ENTRIES ? new_src_mac_ix : 0;
    action.new_in_vci         = f_vlan_ins_enable ? new_vci : 0;
    action.out_vlan_ix        = f_out_vlan_ins_enable ? out_vlan_ix : 0;
    action.out_vlan_ins       = f_out_vlan_ins_enable ? 1 : 0;
    action.out_vlan_rm        = f_out_vlan_rm_enable ? 1 : 0;
    if ( f_is_lan )
    {
        action.mpoa_type      = WRX_QUEUE_CONFIG((dslwan_qid >> 8) & 0xFF)->mpoa_type;
        action.dslwan_qid     = dslwan_qid & 0xFF;
    }

    compare.src_ip            = src_ip;
    compare.dest_ip           = dst_ip;
    compare.src_port          = src_port;
    compare.dest_port         = dst_port;

    *WAN_ROUT_FORWARD_ACTION_TBL(entry) = action;
    *WAN_ROUT_FORWARD_COMPARE_TBL(entry) = compare;

    //  before enable this entry, clear hit status
    __ppa_disable_int();
    *ROUT_FWD_HIT_STAT_TBL(entry >> 5) &= ~(1 << (BITSIZEOF_UINT32 - 1 - (entry & 0x1F)));
    __ppa_enable_int();

    WAN_ROUT_FORWARD_ACTION_TBL(entry)->dest_list = dest_list;  //  enable this entry finally

    ppa_lock_release(p_lock);

    ASSERT(p_entry != NULL, "p_entry == NULL");
    *p_entry = entry;

    return IFX_SUCCESS;
}

void del_routing_entry(uint32_t entry)
{
    PPA_LOCK *p_lock;
    uint32_t entry_bit;
    volatile uint32_t *p;

    if ( entry >= g_lan_routing_entry_off )
    {
        //  LAN
        if ( entry >= g_lan_routing_entry_off + g_lan_routing_entries )
            return;
        p_lock = &g_lan_routing_lock;
        entry_bit = entry + ((BITSIZEOF_UINT32 - (g_lan_routing_entry_off & (BITSIZEOF_UINT32 - 1))) & (BITSIZEOF_UINT32 - 1));
    }
    else
    {
        //  WAN
        if ( entry >= g_wan_routing_entries )
            return;
        p_lock = &g_wan_routing_lock;
        entry_bit = entry;
    }

    ppa_lock_get(p_lock);

    WAN_ROUT_FORWARD_ACTION_TBL(entry)->dest_list = IFX_PPA_DEST_LIST_CPU0;

    p = (volatile uint32_t *)WAN_ROUT_FORWARD_COMPARE_TBL(entry);
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;

    g_routing_entry_occupation[entry_bit >> 5] &= ~(1 << (entry_bit & 0x1F));

    ppa_lock_release(p_lock);
}

int32_t update_routing_entry(uint32_t entry,
                             uint32_t route_type,
                             uint32_t new_ip,
                             uint32_t new_port,
                             uint8_t  new_mac[PPA_ETH_ALEN],
                             uint32_t new_src_mac_ix,
                             uint32_t mtu_ix,
                             uint32_t f_new_dscp_enable,
                             uint32_t new_dscp,
                             uint32_t f_vlan_ins_enable,
                             uint32_t new_vci,
                             uint32_t f_vlan_rm_enable,
                             uint32_t pppoe_mode,
                             uint32_t pppoe_ix,
                             uint32_t f_out_vlan_ins_enable,
                             uint32_t out_vlan_ix,
                             uint32_t f_out_vlan_rm_enable,
                             uint32_t dslwan_qid,
                             uint32_t dest_list,
                             uint32_t flags)
{
    PPA_LOCK *p_lock;
    struct rout_forward_action_tbl action;
    uint32_t i;

    i = entry;

    if ( entry >= g_lan_routing_entry_off )
    {
        //  LAN
        p_lock = &g_lan_routing_lock;
        i += (BITSIZEOF_UINT32 - (g_lan_routing_entry_off & (BITSIZEOF_UINT32 - 1))) & (BITSIZEOF_UINT32 - 1);
        //i = (entry >> 5);
    }
    else
    {
        //  WAN
        p_lock = &g_wan_routing_lock;
    }

    ppa_lock_get(p_lock);

    if ( !(g_routing_entry_occupation[i >> 5] & (1 << (i & 0x1F))) )
    //if ( !(g_routing_entry_occupation[i] & (1 << (entry & 0x1F))) )
    {
        ppa_lock_release(p_lock);
        return -1;
    }

    action = *WAN_ROUT_FORWARD_ACTION_TBL(entry);

    //  disable this entry
    WAN_ROUT_FORWARD_ACTION_TBL(entry)->dest_list = IFX_PPA_DEST_LIST_CPU0;

    //  if dest_chid is not update, keep it
    if ( !(flags & IFX_PPA_UPDATE_ROUTING_ENTRY_DEST_LIST) )
        dest_list = action.dest_list;
    action.dest_list = IFX_PPA_DEST_LIST_CPU0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_PORT) )
        action.new_port           = new_port;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_MAC) )
    {
        action.new_dest_mac54     = (((uint32_t)new_mac[0] & 0xFF) << 8) | ((uint32_t)new_mac[1] & 0xFF);
        action.new_dest_mac30     = (((uint32_t)new_mac[2] & 0xFF) << 24) | (((uint32_t)new_mac[3] & 0xFF) << 16) | (((uint32_t)new_mac[4] & 0xFF) << 8) | ((uint32_t)new_mac[5] & 0xFF);
    }

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_IP) )
        action.new_ip             = new_ip;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_ROUTE_TYPE) )
        action.rout_type          = route_type;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_DSCP) )
        action.new_dscp           = new_dscp;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_MTU_IX) )
        action.mtu_ix             = mtu_ix < MAX_MTU_ENTRIES ? mtu_ix : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_VLAN_INS_EN) )
        action.in_vlan_ins        = f_vlan_ins_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_VLAN_RM_EN) )
        action.in_vlan_rm         = f_vlan_rm_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_DSCP_EN) )
        action.new_dscp_en        = f_new_dscp_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_PPPOE_MODE) )
        action.pppoe_mode         = pppoe_mode ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_INS_EN) )
        action.out_vlan_ins       = f_out_vlan_ins_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_RM_EN) )
        action.out_vlan_rm        = f_out_vlan_rm_enable ? 1 : 0;

    if ( entry >= g_lan_routing_entry_off && (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_PPPOE_IX) )
        action.pppoe_ix           = action.pppoe_mode && pppoe_ix < MAX_PPPOE_ENTRIES ? pppoe_ix : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_SRC_MAC_IX) )
        action.new_src_mac_ix     = new_src_mac_ix < MAX_MAC_ENTRIES ? new_src_mac_ix : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_VCI) )
        action.new_in_vci         = action.in_vlan_ins ? new_vci : 0;

    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_IX) )
        action.out_vlan_ix        = action.out_vlan_ins ? out_vlan_ix : 0;

#if !defined(CONFIG_DANUBE_PPA_A4_API) && !defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    if ( (flags & IFX_PPA_UPDATE_ROUTING_ENTRY_DEST_QID) )
        action.dslwan_qid         = dslwan_qid & 0xFF;
#endif

    *WAN_ROUT_FORWARD_ACTION_TBL(entry) = action;

    WAN_ROUT_FORWARD_ACTION_TBL(entry)->dest_list = dest_list;

    ppa_lock_release(p_lock);

    return 0;
}

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
int32_t add_wan_mc_entry(uint32_t dest_ip_compare,
                         uint32_t f_vlan_ins_enable,
                         uint32_t new_vci,
                         uint32_t f_vlan_rm_enable,
                         uint32_t f_src_mac_enable,
                         uint32_t src_mac_ix,
                         uint32_t pppoe_mode,
                         uint32_t f_out_vlan_ins_enable,
                         uint32_t out_vlan_ix,
                         uint32_t f_out_vlan_rm_enable,
                         uint32_t dest_list,
                         uint32_t *p_entry)
#else
int32_t add_wan_mc_entry(uint32_t dest_ip_compare,
                         uint32_t f_vlan_ins_enable,
                         uint32_t new_vci,
                         uint32_t f_vlan_rm_enable,
                         uint32_t f_src_mac_enable,
                         uint32_t src_mac_ix,
                         uint32_t pppoe_mode,
                         uint32_t f_out_vlan_ins_enable,
                         uint32_t out_vlan_ix,
                         uint32_t f_out_vlan_rm_enable,
                         uint32_t f_new_dscp_en,
                         uint32_t new_dscp,
                         uint32_t dest_qid,
                         uint32_t dest_list,
                         uint32_t route_type,   //  this field is not available in firmware
                         uint32_t *p_entry)
#endif
{
    uint32_t entry;
    struct wan_rout_multicast_tbl cfg = {0};
    struct wan_rout_mc_outer_vlan_tbl out_cfg = {0};
    uint32_t entries;
    uint32_t i;
    uint32_t bit;

    /*
     *  find empty entry
     */

    entries = (g_wan_mc_entries + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32;

    ppa_lock_get(&g_wan_mc_lock);

    for ( i = 0; i < entries; i++ )
        if ( g_wan_mc_entry_occupation[i] != ~0 )
            goto ADD_WAN_MC_ENTRY_GOON;
    //  no empty entry
    ppa_lock_release(&g_wan_mc_lock);
    return -1;

ADD_WAN_MC_ENTRY_GOON:
    entry = i * BITSIZEOF_UINT32;
    bit = 1;
    while ( (g_wan_mc_entry_occupation[i] & bit) )
    {
        bit <<= 1;
        entry++;
    }
    g_wan_mc_entry_occupation[i] |= bit;

    cfg.new_in_vci     = f_vlan_ins_enable ? new_vci : 0;
    cfg.dest_list      = IFX_PPA_DEST_LIST_CPU0;
    cfg.pppoe_mode     = pppoe_mode ? 1 : 0;
    cfg.new_src_mac_en = f_src_mac_enable ? 1 : 0;
    cfg.in_vlan_ins    = f_vlan_ins_enable ? 1 : 0;
    cfg.in_vlan_rm     = f_vlan_rm_enable ? 1 : 0;
    cfg.new_src_mac_ix = f_src_mac_enable ? src_mac_ix : 0;
    cfg.wan_dest_ip    = dest_ip_compare;

    *WAN_ROUT_MULTICAST_TBL(entry) = cfg;

    out_cfg.out_vlan_ins0    = f_out_vlan_ins_enable ? 1 : 0;
    if ( f_out_vlan_ins_enable )
        out_cfg.out_vlan_ix0 = out_vlan_ix;
    out_cfg.out_vlan_rm0     = f_out_vlan_rm_enable ? 1 : 0;

    *((volatile u8 *)WAN_ROUT_MC_OUTER_VLAN_TBL(entry) + (entry & 0x03)) = *(u8 *)&out_cfg;

    __ppa_disable_int();
    *MC_ROUT_FWD_HIT_STAT_TBL(entry >> 5) &= ~(1 << (BITSIZEOF_UINT32 - 1 - (entry & 0x1F)));
    __ppa_enable_int();

    WAN_ROUT_MULTICAST_TBL(entry)->dest_list = dest_list;

    ppa_lock_release(&g_wan_mc_lock);

    ASSERT(p_entry != NULL, "p_entry == NULL");
    *p_entry = entry;

    return 0;
}

void del_wan_mc_entry(uint32_t entry)
{
    if ( entry < g_wan_mc_entries )
    {
        volatile uint32_t *p;

        ppa_lock_get(&g_wan_mc_lock);

        WAN_ROUT_MULTICAST_TBL(entry)->dest_list = IFX_PPA_DEST_LIST_CPU0;  //  disable entry

        p = (volatile uint32_t *)WAN_ROUT_MULTICAST_TBL(entry);
        p[1] = 0;
        p[0] = 0;

        g_wan_mc_entry_occupation[entry >> 5] &= ~(1 << (entry & 0x1F));

        ppa_lock_release(&g_wan_mc_lock);
    }
}

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
int32_t update_wan_mc_entry(uint32_t entry,
                            uint32_t f_vlan_ins_enable,
                            uint32_t new_vci,
                            uint32_t f_vlan_rm_enable,
                            uint32_t f_src_mac_enable,
                            uint32_t src_mac_ix,
                            uint32_t pppoe_mode,
                            uint32_t dest_list,
                            uint32_t dest_chid,
                            uint32_t flags)
#else
int32_t update_wan_mc_entry(uint32_t entry,
                            uint32_t f_vlan_ins_enable,
                            uint32_t new_vci,
                            uint32_t f_vlan_rm_enable,
                            uint32_t f_src_mac_enable,
                            uint32_t src_mac_ix,
                            uint32_t pppoe_mode,
                            uint32_t f_out_vlan_ins_enable,
                            uint32_t out_vlan_ix,
                            uint32_t f_out_vlan_rm_enable,
                            uint32_t f_new_dscp_en,
                            uint32_t new_dscp,
                            uint32_t dest_qid,
                            uint32_t dest_list,
                            uint32_t flags)
#endif
{
    struct wan_rout_multicast_tbl cfg;
    struct wan_rout_mc_outer_vlan_tbl out_cfg;

    if ( entry >= g_wan_mc_entries )
        return -1;

    ppa_lock_get(&g_wan_mc_lock);

    if ( !(g_wan_mc_entry_occupation[entry >> 5] & (1 << (entry & 0x1F))) )
    {
        ppa_lock_release(&g_wan_mc_lock);
        return -1;
    }

    cfg = *WAN_ROUT_MULTICAST_TBL(entry);

    //  if not update, keep it
    if ( !(flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_DEST_LIST) )
        dest_list = cfg.dest_list;
    cfg.dest_list = IFX_PPA_DEST_LIST_CPU0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_VLAN_INS_EN) )
        cfg.in_vlan_ins    = f_vlan_ins_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_NEW_VCI) )
        cfg.new_in_vci     = cfg.in_vlan_ins ? new_vci : 0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_VLAN_RM_EN) )
        cfg.in_vlan_rm     = f_vlan_rm_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_SRC_MAC_EN) )
        cfg.new_src_mac_en = f_src_mac_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_SRC_MAC_IX) )
        cfg.new_src_mac_ix = cfg.new_src_mac_en ? src_mac_ix : 0;

#if !defined(CONFIG_DANUBE_PPA_A4_API) && !defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_PPPOE_MODE) )
        cfg.pppoe_mode     = pppoe_mode ? 1 : 0;
#endif

    *(u8 *)&out_cfg = *((volatile u8 *)WAN_ROUT_MC_OUTER_VLAN_TBL(entry) + (entry & 0x03));

#if !defined(CONFIG_DANUBE_PPA_A4_API) && !defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_OUT_VLAN_INS_EN) )
        out_cfg.out_vlan_ins0 = f_out_vlan_ins_enable ? 1 : 0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_OUT_VLAN_IX) )
        out_cfg.out_vlan_ix0 = out_cfg.out_vlan_ins0 ? out_vlan_ix : 0;

    if ( (flags & IFX_PPA_UPDATE_WAN_MC_ENTRY_OUT_VLAN_RM_EN) )
        out_cfg.out_vlan_rm0 = f_out_vlan_rm_enable ? 1 : 0;
#endif

     *((volatile u8 *)WAN_ROUT_MC_OUTER_VLAN_TBL(entry) + (entry & 0x03)) = *(u8 *)&out_cfg;

    *WAN_ROUT_MULTICAST_TBL(entry) = cfg;

    WAN_ROUT_MULTICAST_TBL(entry)->dest_list = dest_list;

    ppa_lock_release(&g_wan_mc_lock);

    return 0;
}

int32_t get_dest_ip_from_wan_mc_entry(uint32_t entry,
                                      uint32_t *p_ip)
{
    if ( entry >= g_wan_mc_entries )
        return -1;

    ASSERT(p_ip != NULL, "p_ip == NULL");
    *p_ip = WAN_ROUT_MULTICAST_TBL(entry)->wan_dest_ip;

    return 0;
}

int32_t add_bridging_entry(uint32_t port,
                           uint8_t  mac[PPA_ETH_ALEN],
                           uint32_t f_src_mac_drop,
                           uint32_t dslwan_qid,
                           uint32_t dest_list,
                           uint32_t *p_entry)
{
    uint32_t entry;
    struct brg_forward_tbl action = {0};
    uint32_t entries;
    uint32_t i;
    uint32_t bit;

    if(port > get_number_of_phys_port()){//don't add to switch ??Should add to CPU port?
        return IFX_EINVAL;
    }
    entries = (g_bridging_entries + BITSIZEOF_UINT32 - 1) / BITSIZEOF_UINT32;

    ppa_lock_get(&g_bridging_lock);

    for ( i = 0; i < entries; i++ )
        if ( g_bridging_entry_occupation[i] != ~0 )
            goto ADD_BRIDGING_ENTRY_GOON;
    //  no empty entry
    ppa_lock_release(&g_bridging_lock);
    return -1;

ADD_BRIDGING_ENTRY_GOON:
    entry = i * BITSIZEOF_UINT32;
    bit = 1;
    while ( (g_bridging_entry_occupation[i] & bit) )
    {
        bit <<= 1;
        entry++;
    }
    g_bridging_entry_occupation[i] |= bit;

    action.itf              = port;
    action.mac_52           = (((uint32_t)mac[0] & 0xFF) << 24) | (((uint32_t)mac[1] & 0xFF) << 16) | (((uint32_t)mac[2] & 0xFF) << 8) | ((uint32_t)mac[3] & 0xFF);
    action.mac_10           = (((uint32_t)mac[4] & 0xFF) << 8) | ((uint32_t)mac[5] & 0xFF);
    action.src_mac_drop     = f_src_mac_drop;
    if ( port == IFX_PPA_PORT_ATM )
        action.dslwan_qid   = (dslwan_qid >> 8) & 0xFF; //  VCID
    action.dest_list        = dest_list;

    __ppa_disable_int();
    *BRG_FWD_HIT_STAT_TBL(entry >> 5) &= ~(1 << (BITSIZEOF_UINT32 - 1 - (entry & 0x1F)));
    __ppa_enable_int();

    ((uint32_t *)BRIDGING_FORWARD_TBL(entry))[3] = ((uint32_t *)&action)[3];
    ((uint32_t *)BRIDGING_FORWARD_TBL(entry))[1] = ((uint32_t *)&action)[1];
    ((uint32_t *)BRIDGING_FORWARD_TBL(entry))[0] = ((uint32_t *)&action)[0];
    ((uint32_t *)BRIDGING_FORWARD_TBL(entry))[2] = 0;

    ppa_lock_release(&g_bridging_lock);

    if ( p_entry )
        *p_entry = entry;

    return 0;
}

void del_bridging_entry(uint32_t entry)
{
    volatile uint32_t *p;

    if ( entry >= g_bridging_entries )
        return;

    ppa_lock_get(&g_bridging_lock);

    p = (volatile uint32_t *)BRIDGING_FORWARD_TBL(entry);
    p[2] = 0xFFFFFFFF;
    p[0] = 0xFFFFFFFF;
    p[1] = 0xFFFFFFFF;
    p[3] = 0xFFFFFFFF;

    g_bridging_entry_occupation[entry >> 5] &= ~(1 << (entry & 0x1F));

    ppa_lock_release(&g_bridging_lock);
}

int32_t add_pppoe_entry(uint32_t session_id,
                        uint32_t *p_entry)
{
    uint32_t entry, empty_entry = MAX_PPPOE_ENTRIES;

    session_id &= 0x0003FFFF;

    ppa_lock_get(&g_pppoe_lock);

    for ( entry = 0; entry < MAX_PPPOE_ENTRIES; entry++ )
        if ( !g_pppoe_entry_counter[entry] )
            empty_entry = entry;
        else if ( *PPPOE_CFG_TBL(entry) == session_id )
            goto ADD_PPPOE_ENTRY_GOON;

    //  no empty entry
    if ( empty_entry >= MAX_PPPOE_ENTRIES )
    {
        ppa_lock_release(&g_pppoe_lock);
        return -1;
    }

    entry = empty_entry;

    *PPPOE_CFG_TBL(entry) = session_id;

ADD_PPPOE_ENTRY_GOON:
    g_pppoe_entry_counter[entry]++;

    ppa_lock_release(&g_pppoe_lock);

    ASSERT(p_entry != NULL, "p_entry == NULL");
    *p_entry = entry;

    return 0;
}

void del_pppoe_entry(uint32_t entry)
{
    if ( entry < MAX_PPPOE_ENTRIES )
    {
        ppa_lock_get(&g_pppoe_lock);
        if ( g_pppoe_entry_counter[entry] && !--g_pppoe_entry_counter[entry] )
            *PPPOE_CFG_TBL(entry) = 0;
        ppa_lock_release(&g_pppoe_lock);
    }
}

int32_t get_pppoe_entry(uint32_t entry,
                        uint32_t *p_session_id)
{
    if ( entry >= MAX_PPPOE_ENTRIES )
        return -1;

    ppa_lock_get(&g_pppoe_lock);

    if ( !g_pppoe_entry_counter[entry] )
    {
        ppa_lock_release(&g_pppoe_lock);
        return -1;
    }

    ASSERT(p_session_id != NULL, "p_session_id == NULL");
    *p_session_id = *PPPOE_CFG_TBL(entry);

    ppa_lock_release(&g_pppoe_lock);

    return 0;
}

int32_t add_mtu_entry(uint32_t mtu_size,
                      uint32_t *p_entry)
{
    uint32_t entry, empty_entry = MAX_MTU_ENTRIES;

    mtu_size &= 0x0003FFFF;

    ppa_lock_get(&g_mtu_lock);

    //  find existing entry and empty entry
    for ( entry = 0; entry < MAX_MTU_ENTRIES; entry++ )
        if ( !g_mtu_entry_counter[entry] )
            empty_entry = entry;
        else if ( *MTU_CFG_TBL(entry) == mtu_size )
            goto ADD_MTU_ENTRY_GOON;

    //  no empty entry
    if ( empty_entry >= MAX_MTU_ENTRIES )
    {
        ppa_lock_release(&g_mtu_lock);
        return -1;
    }

    entry = empty_entry;

    *MTU_CFG_TBL(entry) = mtu_size;

ADD_MTU_ENTRY_GOON:
    g_mtu_entry_counter[entry]++;

    ppa_lock_release(&g_mtu_lock);

    ASSERT(p_entry != NULL, "p_entry == NULL");
    *p_entry = entry;

    return 0;
}

void del_mtu_entry(uint32_t entry)
{
    if ( entry < MAX_MTU_ENTRIES )
    {
        ppa_lock_get(&g_mtu_lock);
        if ( g_mtu_entry_counter[entry] && !--g_mtu_entry_counter[entry] )
            *MTU_CFG_TBL(entry) = 0;
        ppa_lock_release(&g_mtu_lock);
    }
}

int32_t get_mtu_entry(uint32_t entry,
                      uint32_t *p_mtu_size)
{
    if ( entry >= MAX_MTU_ENTRIES )
        return -1;

    ppa_lock_get(&g_mtu_lock);

    if ( !g_mtu_entry_counter[entry] )
    {
        ppa_lock_release(&g_mtu_lock);
        return -1;
    }

    ASSERT(p_mtu_size != NULL, "p_mtu_size == NULL");
    *p_mtu_size = *MTU_CFG_TBL(entry);

    ppa_lock_release(&g_mtu_lock);

    return 0;
}

/*!
  \fn void uint32_t get_routing_entry_bytes(uint32_t entry)
  \ingroup AMAZON_S_PPA_PPE_A5_HAL_COMPILE_PARAMS
  \brief get one routing entry's byte counter
  \param entry  entry number got from function call "add_routing_entry"
  \return byte counter value
 */
uint32_t get_routing_entry_bytes(uint32_t entry)
{
    //  no available in A4/D4
    return 0;
}

int32_t add_mac_entry(uint8_t mac[PPA_ETH_ALEN],
                      uint32_t *p_entry)
{
    int32_t entry, empty_entry = -1;
    uint32_t mac52 = (((uint32_t)mac[0] & 0xFF) << 24) | (((uint32_t)mac[1] & 0xFF) << 16) | (((uint32_t)mac[2] & 0xFF) << 8) | ((uint32_t)mac[3] & 0xFF);
    uint32_t mac10 = (((uint32_t)mac[4] & 0xFF) << 24) | (((uint32_t)mac[5] & 0xFF) << 16);

    ppa_lock_get(&g_mac_lock);

    //  find existing entry and empty entry
    for ( entry = MAX_MAC_ENTRIES - 1; entry >= 0; entry-- )
        if ( !g_mac_entry_counter[entry] )
            empty_entry = entry;
        else if ( ROUT_MAC_CFG_TBL(entry)[0] == mac52 && ROUT_MAC_CFG_TBL(entry)[1] == mac10 )
            goto ADD_MAC_ENTRY_GOON;

    //  no empty entry
    if ( empty_entry < 0 )
    {
        ppa_lock_release(&g_mac_lock);
        return -1;
    }

    entry = empty_entry;

    ROUT_MAC_CFG_TBL(entry)[0] = mac52;
    ROUT_MAC_CFG_TBL(entry)[1] = mac10;

    if ( !g_mac_entries_counter++ )
        (*CFG_ROUT_MAC_NO) = 2;
    else
        (*CFG_ROUT_MAC_NO)++;

ADD_MAC_ENTRY_GOON:
    g_mac_entry_counter[entry]++;

    ppa_lock_release(&g_mac_lock);

    ASSERT(p_entry != NULL, "p_entry == NULL");
    *p_entry = entry;

    return 0;
}

void del_mac_entry(uint32_t entry)
{
    if ( entry < MAX_MAC_ENTRIES )
    {
        ppa_lock_get(&g_mac_lock);
        if ( g_mac_entry_counter[entry] && !--g_mac_entry_counter[entry] )
        {
            ROUT_MAC_CFG_TBL(entry)[0] = 0;
            ROUT_MAC_CFG_TBL(entry)[1] = 0;

            //  TODO: wait for more accurate implement
            //  if ( !--g_mac_entries_counter )
            //      (*CFG_ROUT_MAC_NO) = 0;
            //  else
            //      (*CFG_ROUT_MAC_NO)--;
        }
        ppa_lock_release(&g_mac_lock);
    }
}

int32_t get_mac_entry(uint32_t entry,
                      uint8_t mac[PPA_ETH_ALEN])
{
    if ( entry >= MAX_MAC_ENTRIES )
        return -1;

    ppa_lock_get(&g_mac_lock);

    if ( !g_mac_entry_counter[entry] )
    {
        ppa_lock_release(&g_mac_lock);
        return -1;
    }

    ASSERT(mac != NULL, "mac == NULL");
    ppa_memcpy(mac, (void *)ROUT_MAC_CFG_TBL(entry), PPA_ETH_ALEN);

    ppa_lock_release(&g_mac_lock);

    return 0;
}

int32_t add_outer_vlan_entry(uint32_t new_tag,
                             uint32_t *p_entry)
{
    uint32_t entry, empty_entry = MAX_OUTER_VLAN_ENTRIES;

    ppa_lock_get(&g_outer_vlan_lock);

    for ( entry = 0; entry < MAX_OUTER_VLAN_ENTRIES; entry++ )
        if ( !g_outer_vlan_entry_counter[entry] )
            empty_entry = entry;
        else if ( *OUTER_VLAN_TBL(entry) == new_tag )
            goto ADD_OUTER_VLAN_ENTRY_GOON;

    //  no empty entry
    if ( empty_entry >= MAX_OUTER_VLAN_ENTRIES )
    {
        ppa_lock_release(&g_outer_vlan_lock);
        return -1;
    }

    entry = empty_entry;

    *OUTER_VLAN_TBL(entry) = new_tag;

ADD_OUTER_VLAN_ENTRY_GOON:
    g_outer_vlan_entry_counter[entry]++;

    ppa_lock_release(&g_outer_vlan_lock);

    ASSERT(p_entry != NULL, "p_entry == NULL");
    *p_entry = entry;

    return 0;
}

void del_outer_vlan_entry(uint32_t entry)
{
    if ( entry < MAX_OUTER_VLAN_ENTRIES )
    {
        ppa_lock_get(&g_outer_vlan_lock);
        if ( g_outer_vlan_entry_counter[entry] && !--g_outer_vlan_entry_counter[entry] )
            *OUTER_VLAN_TBL(entry) = 0;
        ppa_lock_release(&g_outer_vlan_lock);
    }
}

int32_t get_outer_vlan_entry(uint32_t entry,
                             uint32_t *p_outer_vlan_tag)
{
    if ( entry >= MAX_OUTER_VLAN_ENTRIES )
        return -1;

    ppa_lock_get(&g_outer_vlan_lock);

    if ( !g_outer_vlan_entry_counter[entry] )
    {
        ppa_lock_release(&g_outer_vlan_lock);
        return -1;
    }

    ASSERT(p_outer_vlan_tag != NULL, "p_outer_vlan_tag == NULL");
    *p_outer_vlan_tag = *OUTER_VLAN_TBL(entry);

    ppa_lock_release(&g_outer_vlan_lock);

    return 0;
}

#if defined(CONFIG_DANUBE_PPA_A4_API) || defined(CONFIG_DANUBE_PPA_A4_API_MODULE)
void get_itf_mib(void *p)
{
}
#else
void get_itf_mib(uint32_t itf, struct ppe_itf_mib *p)
{
    if ( p != NULL && itf < 8 )
        ppa_memcpy(p, (void *)ITF_MIB_TBL(itf), sizeof(*p));
}
#endif

#if 0
void get_lan_rx_mib(uint32_t *p_fast_tcp_pkts,
                    uint32_t *p_fast_udp_pkts,
                    uint32_t *p_fast_drop_tcp_pkts,
                    uint32_t *p_fast_drop_udp_pkts,
                    uint32_t *p_drop_pkts)
{
    if ( p_fast_tcp_pkts )
        *p_fast_tcp_pkts = ITF_MIB_TBL(1)->ig_fast_rt_uni_tcp_pkts;

    if ( p_fast_udp_pkts )
        *p_fast_udp_pkts = ITF_MIB_TBL(1)->ig_fast_rt_uni_udp_pkts;

    if ( p_fast_drop_tcp_pkts )
        *p_fast_drop_tcp_pkts = ITF_MIB_TBL(1)->ig_drop_pkts;

    if ( p_fast_drop_udp_pkts )
        *p_fast_drop_udp_pkts = ITF_MIB_TBL(1)->ig_drop_pkts;

    if ( p_drop_pkts )
        *p_drop_pkts = ITF_MIB_TBL(1)->ig_drop_pkts;
}

void get_wan_rx_mib(uint32_t *p_fast_uc_tcp_pkts,
                    uint32_t *p_fast_uc_udp_pkts,
                    uint32_t *p_fast_mc_tcp_pkts,
                    uint32_t *p_fast_mc_udp_pkts,
                    uint32_t *p_fast_drop_tcp_pkts,
                    uint32_t *p_fast_drop_udp_pkts,
                    uint32_t *p_drop_pkts)
{
    if ( p_fast_uc_tcp_pkts )
        *p_fast_uc_tcp_pkts = ITF_MIB_TBL(0)->ig_fast_rt_uni_tcp_pkts;

    if ( p_fast_uc_udp_pkts )
        *p_fast_uc_udp_pkts = ITF_MIB_TBL(0)->ig_fast_rt_uni_udp_pkts;

    if ( p_fast_mc_tcp_pkts )
        *p_fast_mc_tcp_pkts = ITF_MIB_TBL(0)->ig_fast_rt_mul_pkts;

    if ( p_fast_mc_udp_pkts )
        *p_fast_mc_udp_pkts = ITF_MIB_TBL(0)->ig_fast_rt_mul_pkts;

    if ( p_fast_drop_tcp_pkts )
        *p_fast_drop_tcp_pkts = ITF_MIB_TBL(0)->ig_drop_pkts;

    if ( p_fast_drop_udp_pkts )
        *p_fast_drop_udp_pkts = ITF_MIB_TBL(0)->ig_drop_pkts;

    if ( p_drop_pkts )
        *p_drop_pkts = ITF_MIB_TBL(0)->ig_drop_pkts;
}
#endif

uint32_t test_and_clear_hit_stat(uint32_t entry)
{
    if ( entry < MAX_ROUTING_ENTRIES )
    {
        uint32_t ret;
        uint32_t bit;

        bit = 1 << (BITSIZEOF_UINT32 - 1 - (entry & 0x1F));
        __ppa_disable_int();
        ret = *ROUT_FWD_HIT_STAT_TBL(entry >> 5);
        *ROUT_FWD_HIT_STAT_TBL(entry >> 5) = ret & ~bit;
        __ppa_enable_int();
        ret &= bit;

        return ret;
    }
    else
        return 0;
}

uint32_t test_and_clear_hit_stat_batch(uint32_t entry)
{
    if ( entry < MAX_ROUTING_ENTRIES )
    {
        uint32_t block = entry >> 5;
        uint32_t ret = *ROUT_FWD_HIT_STAT_TBL(block);

        *ROUT_FWD_HIT_STAT_TBL(block) = 0;

        return ret;
    }
    else
        return 0;
}

uint32_t test_and_clear_mc_hit_stat(uint32_t entry)
{
    if ( entry < MAX_WAN_MC_ENTRIES )
    {
        uint32_t ret;
        uint32_t bit;

        bit = 1 << (BITSIZEOF_UINT32 - 1 - (entry & 0x1F));
        __ppa_disable_int();
        ret = *MC_ROUT_FWD_HIT_STAT_TBL(entry >> 5);
        *MC_ROUT_FWD_HIT_STAT_TBL(entry >> 5) = ret & ~bit;
        __ppa_enable_int();
        ret &= bit;

        return ret;
    }
    else
        return 0;
}

uint32_t test_and_clear_mc_hit_stat_batch(uint32_t entry)
{
    if ( entry < MAX_WAN_MC_ENTRIES )
    {
        uint32_t block = entry >> 5;
        uint32_t ret = *MC_ROUT_FWD_HIT_STAT_TBL(block);

        *MC_ROUT_FWD_HIT_STAT_TBL(block) = 0;

        return ret;
    }
    else
        return 0;
}

uint32_t test_and_clear_bridging_hit_stat(uint32_t entry)
{
    if ( entry < MAX_BRIDGING_ENTRIES )
    {
        uint32_t ret;
        uint32_t bit;

        bit = 1 << (BITSIZEOF_UINT32 - 1 - (entry & 0x1F));
        __ppa_disable_int();
        ret = *BRG_FWD_HIT_STAT_TBL(entry >> 5);
        *BRG_FWD_HIT_STAT_TBL(entry >> 5) = ret & ~bit;
        __ppa_enable_int();
        ret &= bit;

        return ret;
    }
    else
        return 0;
}

uint32_t test_and_clear_bridging_hit_stat_batch(uint32_t entry)
{
    if ( entry < MAX_BRIDGING_ENTRIES )
    {
        uint32_t block = entry >> 5;
        uint32_t ret = *BRG_FWD_HIT_STAT_TBL(block);

        *BRG_FWD_HIT_STAT_TBL(block) = 0;

        return ret;
    }
    else
        return 0;
}

#if 0
void set_mac_table(uint8_t lan_mac[PPA_ETH_ALEN],
                   uint8_t wan_mac[PPA_ETH_ALEN])
{
    uint32_t i;

    if ( lan_mac )
        for ( i = 0; i < MAX_LAN_MAC_ENTRIES; i++ )
        {
            *(volatile uint32_t *)LAN_ROUT_MAC_CFG_TBL(i) = *(uint32_t *)lan_mac;
            *((volatile uint16_t *)LAN_ROUT_MAC_CFG_TBL(i) + 2) = *((uint16_t *)lan_mac + 2);
        }

    if ( wan_mac )
        for ( i = 0; i < MAX_WAN_MAC_ENTRIES; i++ )
        {
            *(volatile uint32_t *)WAN_ROUT_MAC_CFG_TBL(i) = *(uint32_t *)wan_mac;
            *((volatile uint16_t *)WAN_ROUT_MAC_CFG_TBL(i) + 2) = *((uint16_t *)wan_mac + 2);
        }
}

void get_mac_table(uint8_t lan_mac[PPA_ETH_ALEN],
                   uint8_t wan_mac[PPA_ETH_ALEN])
{
    if ( lan_mac )
    {
        *(uint32_t *)lan_mac        = *(volatile uint32_t *)LAN_ROUT_MAC_CFG_TBL(0);
        *((uint16_t *)lan_mac + 2)  = *((volatile uint16_t *)LAN_ROUT_MAC_CFG_TBL(0) + 2);
    }

    if ( wan_mac )
    {
        *(uint32_t *)wan_mac        = *(volatile uint32_t *)WAN_ROUT_MAC_CFG_TBL(0);
        *((uint16_t *)wan_mac + 2)  = *((volatile uint16_t *)WAN_ROUT_MAC_CFG_TBL(0) + 2);
    }
}
#endif

int32_t get_max_vfilter_entries(uint32_t vfilter_type)
{

    if( vfilter_type == IFX_PPA_BRG_VLAN_IG_COND_TYPE_DEF )

        return MAX_DEFAULT_ITF_VLAN_MAP_ENTRIES;

    else if( vfilter_type == IFX_PPA_BRG_VLAN_IG_COND_TYPE_SRC_IP )

        return MAX_SRC_IP_VLAN_MAP_ENTRIES;

    else if( vfilter_type == IFX_PPA_BRG_VLAN_IG_COND_TYPE_ETH_TYPE )

        return MAX_ETHTYPE_VLAN_MAP_ENTRIES;

    else if( vfilter_type == IFX_PPA_BRG_VLAN_IG_COND_TYPE_VLAN)

        return MAX_VLAN_VLAN_MAP_ENTRIES;

    else return 0;

}

static INLINE uint32_t ifx_ppa_drv_get_phys_port_num(void)
{
        return 4 ;//IFX_PPA_PORT_NUM;
}

static int32_t ppa_hal_generic_hook(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
    dbg("ppa_hal_generic_hook cmd 0x%x\n", cmd );
    switch (cmd)  {

    case PPA_GENERIC_WAN_INFO:
        {
            PPA_WAN_INFO *wan_info = (PPA_WAN_INFO *)buffer;
            wan_info->mixed = ETH_PORTS_CFG->eth0_type;
            //note, for a4/d4/e4, it does not support wan_port_map, so we directly hardcode the value here as defined in A5/D5/E5
            if( wan_info->mixed )
                wan_info->wan_port_map = 0x1;
            else
                wan_info->wan_port_map = 0x80;
        }
        return IFX_SUCCESS;

    case PPA_GENERIC_HAL_GET_PORT_MIB:
        {
            int i=0;
            int num;
            PPA_PORT_MIB *mib = (PPA_PORT_MIB*) buffer;

            num = NUM_ENTITY(mib->mib_info) > ifx_ppa_drv_get_phys_port_num() ? ifx_ppa_drv_get_phys_port_num():NUM_ENTITY(mib->mib_info) ;
            for(i=0; i<num; i++)
            {
                mib->mib_info[i].ig_fast_brg_pkts = ITF_MIB_TBL(i)->ig_fast_brg_uni_pkts + ITF_MIB_TBL(i)->ig_fast_brg_mul_pkts + ITF_MIB_TBL(i)->ig_fast_brg_br_pkts;
                mib->mib_info[i].ig_fast_brg_bytes = ITF_MIB_TBL(i)->ig_fast_brg_bytes;
                mib->mib_info[i].ig_fast_rt_ipv4_udp_pkts = ITF_MIB_TBL(i)->ig_fast_rt_uni_udp_pkts;
                mib->mib_info[i].ig_fast_rt_ipv4_tcp_pkts = ITF_MIB_TBL(i)->ig_fast_rt_uni_tcp_pkts;
                mib->mib_info[i].ig_fast_rt_ipv4_mc_pkts = ITF_MIB_TBL(i)->ig_fast_rt_mul_pkts;
                mib->mib_info[i].ig_fast_rt_ipv4_bytes = ITF_MIB_TBL(i)->ig_fast_rt_bytes;
                mib->mib_info[i].ig_fast_rt_ipv6_udp_pkts = 0;
                mib->mib_info[i].ig_fast_rt_ipv6_tcp_pkts = 0;
                mib->mib_info[i].ig_fast_rt_ipv6_bytes = 0;
                mib->mib_info[i].ig_cpu_pkts = ITF_MIB_TBL(i)->ig_cpu_pkts;
                mib->mib_info[i].ig_cpu_bytes = ITF_MIB_TBL(i)->ig_cpu_bytes;
                mib->mib_info[i].ig_drop_pkts = ITF_MIB_TBL(i)->ig_drop_pkts;
                mib->mib_info[i].ig_drop_bytes = ITF_MIB_TBL(i)->ig_drop_bytes;
                mib->mib_info[i].eg_fast_pkts = ITF_MIB_TBL(i)->eg_fast_pkts;

                if( i == 0 )
                {
                    mib->mib_info[i].port_flag = PPA_PORT_MODE_ETH;
                    dbg("port[%d] flag: eth port\n", i);
                }
                else if( i == 1 ) //for ase A4/E4 and danube a4
                {
                    mib->mib_info[i].port_flag = PPA_PORT_MODE_DSL;
                    dbg("port[%d] flag: DSL port\n", i);
                }
                else if( i == 2 )  // 2 is CPU port
                {
                   mib->mib_info[i].port_flag = PPA_PORT_MODE_CPU;
                   dbg("port[%d] flag: CPU port\n", i);
                }
                else
                {
                    mib->mib_info[i].port_flag = PPA_PORT_MODE_EXT;
                    dbg("port[%d] flag: extension port\n", i);
                }
            }
            mib->port_num = num;
            dbg("port_num=%d\n", mib->port_num);
            return IFX_SUCCESS;
        }


    case PPA_GENERIC_HAL_CLEAR_PORT_MIB:
        {
            int i;
            for(i = 0; i < ifx_ppa_drv_get_phys_port_num(); i ++)
            {
                ppa_memset( (void *)(ITF_MIB_TBL(i)), 0, sizeof(struct itf_mib));
            }
            return IFX_SUCCESS;
        }


    case PPA_GENERIC_HAL_SET_DEBUG:
        {
            ifx_ppa_hal_dbg_enable = *(unsigned int *) buffer;
            dbg("Set ifx_ppa_hal_dbg_enable to 0x%x\n", ifx_ppa_hal_dbg_enable );
        }
        return IFX_SUCCESS;

    case PPA_GENERIC_HAL_GET_FEATURE_LIST:
        {
            PPA_FEATURE_INFO *feature = (PPA_FEATURE_INFO *) buffer;

            feature->ipv6_en = 0 ;
            feature->qos_en= 0;
            return IFX_SUCCESS;
        }

    case PPA_GENERIC_HAL_GET_MAX_ENTRIES:
        {
            PPA_MAX_ENTRY_INFO *entry=(PPA_MAX_ENTRY_INFO *)buffer;

            entry->max_lan_entries = MAX_ROUTING_ENTRIES/2;
            entry->max_wan_entries = MAX_ROUTING_ENTRIES/2;
            entry->max_mc_entries = MAX_WAN_MC_ENTRIES;
            entry->max_bridging_entries = MAX_BRIDGING_ENTRIES;
            entry->max_ipv6_addr_entries = 0;
            entry->max_fw_queue= 0;

            return IFX_SUCCESS;
        }


//Fix warning message when exports API from different PPE FW Driver--begin
   case PPA_GENERIC_HAL_GET_HAL_VERSION:
         {
            PPA_VERSION *v=(PPA_VERSION *)buffer;
            get_ppe_hal_id( &v->family, &v->type,&v->itf, &v->mode, &v->major, &v->mid, &v->minor );
            return IFX_SUCCESS;
         }

   case PPA_GENERIC_HAL_GET_PPE_FW_VERSION:
         {
            PPA_VERSION *v=(PPA_VERSION *)buffer;
            return get_firmware_id(v->index, &v->family, &v->type,&v->itf, &v->mode, &v->major, &v->minor );
         }


   case PPA_GENERIC_HAL_GET_PHYS_PORT_NUM:
         {
            PPE_COUNT_CFG *count=(PPE_COUNT_CFG *)buffer;
            count->num = get_number_of_phys_port();
            return IFX_SUCCESS;
         }

   case PPA_GENERIC_HAL_GET_PHYS_PORT_INFO:
    {
        PPE_IFINFO *info = (PPE_IFINFO *) buffer;
        get_phys_port_info(info->port, &info->if_flags, info->ifname);
        return IFX_SUCCESS;

    }

   case PPA_GENERIC_HAL_SET_MIX_WAN_VLAN_ID:
       {
           PPE_WAN_VID_RANGE *range_vlan=(PPE_WAN_VID_RANGE *)buffer;
           set_wan_vlan_id(range_vlan->vid);
           return IFX_SUCCESS;
       }

   case PPA_GENERIC_HAL_GET_MIX_WAN_VLAN_ID:
       {
           PPE_WAN_VID_RANGE *range_vlan=(PPE_WAN_VID_RANGE *)buffer;
           range_vlan->vid = get_wan_vlan_id();
           return IFX_SUCCESS;
       }

   case PPA_GENERIC_HAL_SET_ROUT_CFG:
       {
           PPE_ROUTING_CFG *cfg=(PPE_ROUTING_CFG *)buffer;

           set_route_cfg(cfg->f_is_lan, cfg->entry_num, cfg->mc_entry_num, cfg->f_ip_verify, cfg->f_tcpudp_verify,
                                    cfg->f_tcpudp_err_drop, cfg->f_drop_on_no_hit, cfg->f_mc_drop_on_no_hit, cfg->flags);
           return IFX_SUCCESS;
       }

   case PPA_GENERIC_HAL_SET_BRDG_CFG:
       {
           PPE_BRDG_CFG *cfg=(PPE_BRDG_CFG *)buffer;

           set_bridging_cfg( cfg->entry_num,
                       cfg->br_to_src_port_mask,  cfg->br_to_src_port_en,
                       cfg->f_dest_vlan_en,
                       cfg->f_src_vlan_en,
                       cfg->f_mac_change_drop,
                       cfg->flags);

           return IFX_SUCCESS;
       }

    case PPA_GENERIC_HAL_SET_FAST_MODE_CFG:
       {
           PPE_FAST_MODE_CFG *cfg=(PPE_FAST_MODE_CFG *)buffer;

            set_fast_mode( cfg->mode, cfg->flags);

           return IFX_SUCCESS;
       }

    case PPA_GENERIC_HAL_SET_DEST_LIST:
    {
             PPE_DEST_LIST *dst =(PPE_DEST_LIST *)buffer;
            set_default_dest_list(dst->uc_dest_list, dst->mc_dest_list, dst->if_no );
            return IFX_SUCCESS;
     }

    case PPA_GENERIC_HAL_SET_ACC_ENABLE:
       {
           PPE_ACC_ENABLE *cfg=(PPE_ACC_ENABLE *)buffer;

            set_acc_mode( cfg->f_is_lan, cfg->f_enable);

           return IFX_SUCCESS;
       }

    case PPA_GENERIC_HAL_GET_ACC_ENABLE:
       {
           PPE_ACC_ENABLE *cfg=(PPE_ACC_ENABLE *)buffer;

            get_acc_mode( cfg->f_is_lan, &cfg->f_enable);

           return IFX_SUCCESS;
       }


    case PPA_GENERIC_HAL_GET_BRDG_VLAN_CFG:
       {
           PPE_BRDG_VLAN_CFG *cfg=(PPE_BRDG_VLAN_CFG *)buffer;

            get_bridge_if_vlan_config( cfg->if_no,
                                                        &cfg->f_eg_vlan_insert,
                                                        &cfg->f_eg_vlan_remove,
                                                        &cfg->f_ig_vlan_aware,
                                                        &cfg->f_ig_src_ip_based,
                                                        &cfg->f_ig_eth_type_based,
                                                        &cfg->f_ig_vlanid_based,
                                                        &cfg->f_ig_port_based,
                                                        &cfg->f_eg_out_vlan_insert,
                                                        &cfg->f_eg_out_vlan_remove,
                                                        &cfg->f_ig_out_vlan_aware );

           return IFX_SUCCESS;
       }

    case PPA_GENERIC_HAL_SET_BRDG_VLAN_CFG:
       {
            PPE_BRDG_VLAN_CFG *cfg=(PPE_BRDG_VLAN_CFG *)buffer;

            set_bridge_if_vlan_config( cfg->if_no,
                                                        cfg->f_eg_vlan_insert,
                                                        cfg->f_eg_vlan_remove,
                                                        cfg->f_ig_vlan_aware,
                                                        cfg->f_ig_src_ip_based,
                                                        cfg->f_ig_eth_type_based,
                                                        cfg->f_ig_vlanid_based,
                                                        cfg->f_ig_port_based,
                                                        cfg->f_eg_out_vlan_insert,
                                                        cfg->f_eg_out_vlan_remove,
                                                        cfg->f_ig_out_vlan_aware );
           return IFX_SUCCESS;
       }

    case PPA_GENERIC_HAL_ADD_BRDG_VLAN_FITLER:
       {
           PPE_BRDG_VLAN_FILTER_MAP *filter=(PPE_BRDG_VLAN_FILTER_MAP *)buffer;

            return add_vlan_map( filter->ig_criteria_type,
                                     filter->ig_criteria,
                                     filter->new_vci,
                                     filter->dest_qos,
                                     filter->out_vlan_info.vlan_entry,
                                     filter->in_out_etag_ctrl,
                                     filter->vlan_port_map);
       }


    case PPA_GENERIC_HAL_DEL_BRDG_VLAN_FITLER:
       {
           PPE_BRDG_VLAN_FILTER_MAP *filter=(PPE_BRDG_VLAN_FILTER_MAP *)buffer;

            del_vlan_map( filter->ig_criteria_type, filter->ig_criteria);
            return IFX_SUCCESS;
       }

    case PPA_GENERIC_HAL_GET_BRDG_VLAN_FITLER:
       {
           PPE_BRDG_VLAN_FILTER_MAP *filter=(PPE_BRDG_VLAN_FILTER_MAP *)buffer;

            return get_vlan_map( filter->ig_criteria_type,
                                     filter->entry,
                                     &filter->ig_criteria,
                                     &filter->new_vci,
                                     &filter->dest_qos,
                                     &filter->out_vlan_info.vlan_entry,
                                     &filter->in_out_etag_ctrl,
                                     &filter->vlan_port_map);
       }

     case PPA_GENERIC_HAL_DEL_BRDG_VLAN_ALL_FITLER_MAP:
       {
            del_all_vlan_map( );
            return IFX_SUCCESS;
       }
     case PPA_GENERIC_HAL_GET_MAX_VFILTER_ENTRY_NUM: //get the maxumum entry for vlan filter
        {
            PPE_VFILTER_COUNT_CFG *vlan_count=(PPE_VFILTER_COUNT_CFG *)buffer;
            vlan_count->num = get_max_vfilter_entries( vlan_count->vfitler_type );
            return IFX_SUCCESS;
        }
     case PPA_GENERIC_HAL_GET_IPV6_FLAG:
        {
            return IFX_FAILURE;
        }
     case PPA_GENERIC_HAL_ADD_ROUTE_ENTRY:
       {
           PPE_ROUTING_INFO *route=(PPE_ROUTING_INFO *)buffer;

            if( !route->src_ip.f_ipv6 && !route->dst_ip.f_ipv6 && !route->new_ip.f_ipv6) //all are IPV4
            {  //need to check dslite later ????
                return add_routing_entry(route->f_is_lan,  route->src_ip.ip.ip, route->src_port, route->dst_ip.ip.ip, route->dst_port,
                          route->f_is_tcp, route->route_type, route->new_ip.ip.ip, route->new_port,
                          route->new_dst_mac, route->src_mac.mac_ix, route->mtu_info.mtu_ix, route->f_new_dscp_enable, route->new_dscp,
                          route->f_vlan_ins_enable, route->new_vci, route->f_vlan_rm_enable, route->pppoe_mode, route->pppoe_info.pppoe_ix,
                          route->f_out_vlan_ins_enable, route->out_vlan_info.vlan_entry, route->f_out_vlan_rm_enable,
                          route->dslwan_qid,  route->dest_list, &route->entry );
            }
            else if( route->src_ip.f_ipv6 && route->dst_ip.f_ipv6 ) //both are IPV6
            { //late need to check 6rd ???
                return IFX_FAILURE;
            }
            else
            {
                dbg("ip type not match at all:%d/%d/%d(src_ip/dst_ip/new_ip))\n", route->src_ip.f_ipv6, route->dst_ip.f_ipv6, route->new_ip.f_ipv6);
                return IFX_FAILURE;
            }
        }

    case PPA_GENERIC_HAL_DEL_ROUTE_ENTRY:
       {
           PPE_ROUTING_INFO *route=(PPE_ROUTING_INFO *)buffer;
           del_routing_entry( route->entry );
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_UPDATE_ROUTE_ENTRY:
       {
           PPE_ROUTING_INFO *route=(PPE_ROUTING_INFO *)buffer;
           return update_routing_entry( route->entry,  route->route_type, route->new_ip.ip.ip, route->new_port,
                             route->new_dst_mac, route->src_mac.mac_ix, route->mtu_info.mtu_ix,  route->f_new_dscp_enable, route->new_dscp,
                             route->f_vlan_ins_enable, route->new_vci, route->f_vlan_rm_enable, route->pppoe_mode, route->pppoe_info.pppoe_ix,
                             route->f_out_vlan_ins_enable, route->out_vlan_info.vlan_entry, route->f_out_vlan_rm_enable, route->dslwan_qid,
                             route->dest_list, route->update_flags);
        }
   case PPA_GENERIC_HAL_ADD_MC_ENTRY:
       {
           PPE_MC_INFO *mc = (PPE_MC_INFO *)buffer;

           return add_wan_mc_entry(mc->dest_ip_compare, mc->f_vlan_ins_enable, mc->new_vci, mc->f_vlan_rm_enable, mc->f_src_mac_enable,
                                                        mc->src_mac_ix, mc->pppoe_mode, mc->f_out_vlan_ins_enable, mc->out_vlan_info.vlan_entry,  mc->f_out_vlan_rm_enable,
                                                        mc->f_new_dscp_enable, mc->new_dscp, mc->dest_qid, mc->dest_list, mc->route_type,
                                                        &mc->p_entry);
        }
   case PPA_GENERIC_HAL_DEL_MC_ENTRY:
       {
           PPE_MC_INFO *mc = (PPE_MC_INFO *)buffer;

           del_wan_mc_entry(mc->p_entry);
           return IFX_SUCCESS;
        }
   case PPA_GENERIC_HAL_UPDATE_MC_ENTRY:
       {
           PPE_MC_INFO *mc = (PPE_MC_INFO *)buffer;

           return update_wan_mc_entry(mc->p_entry, mc->f_vlan_ins_enable, mc->new_vci, mc->f_vlan_rm_enable, mc->f_src_mac_enable,
                                                            mc->src_mac_ix, mc->pppoe_mode, mc->f_out_vlan_ins_enable, mc->out_vlan_info.vlan_entry, mc->f_out_vlan_rm_enable,
                                                            mc->f_new_dscp_enable, mc->new_dscp, mc->dest_qid, mc->dest_list, mc->update_flags);
       }

   case PPA_GENERIC_HAL_ADD_BR_MAC_BRIDGING_ENTRY:
       {
           PPE_BR_MAC_INFO *br_mac = (PPE_BR_MAC_INFO *)buffer;

           return add_bridging_entry(br_mac->port, br_mac->mac, br_mac->f_src_mac_drop, br_mac->dslwan_qid, br_mac->dest_list, &br_mac->p_entry);
        }
   case PPA_GENERIC_HAL_DEL_BR_MAC_BRIDGING_ENTRY:
       {
           PPE_BR_MAC_INFO *br_mac = (PPE_BR_MAC_INFO *)buffer;

           del_bridging_entry(br_mac->p_entry);
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_ADD_PPPOE_ENTRY:
       {
           PPE_PPPOE_INFO *pppoe_info=(PPE_PPPOE_INFO *)buffer;
           return add_pppoe_entry( pppoe_info->pppoe_session_id, &pppoe_info->pppoe_ix);
        }
    case PPA_GENERIC_HAL_DEL_PPPOE_ENTRY:
       {
           PPE_PPPOE_INFO *pppoe_info=(PPE_PPPOE_INFO *)buffer;
           del_pppoe_entry(  pppoe_info->pppoe_ix);
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_GET_PPPOE_ENTRY:
       {
           PPE_PPPOE_INFO *pppoe_info=(PPE_PPPOE_INFO *)buffer;
           return get_pppoe_entry(  pppoe_info->pppoe_ix, &pppoe_info->pppoe_session_id);
        }

  case PPA_GENERIC_HAL_ADD_MTU_ENTRY:
       {
           PPE_MTU_INFO *mtu_info=(PPE_MTU_INFO *)buffer;
           return add_mtu_entry( mtu_info->mtu, &mtu_info->mtu_ix);
        }
    case PPA_GENERIC_HAL_DEL_MTU_ENTRY:
       {
           PPE_MTU_INFO *mtu_info=(PPE_MTU_INFO *)buffer;
           del_mtu_entry( mtu_info->mtu_ix);
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_GET_MTU_ENTRY:
       {
           PPE_MTU_INFO *mtu_info=(PPE_MTU_INFO *)buffer;
           return get_mtu_entry( mtu_info->mtu_ix, &mtu_info->mtu);
        }
    case PPA_GENERIC_HAL_GET_ROUTE_ACC_BYTES:
        {
           PPE_ROUTING_INFO *route=(PPE_ROUTING_INFO *)buffer;
           route->bytes = get_routing_entry_bytes( route->entry);
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_ADD_MAC_ENTRY:
       {
           PPE_ROUTE_MAC_INFO *mac_info=(PPE_ROUTE_MAC_INFO *)buffer;
           return add_mac_entry( mac_info->mac, &mac_info->mac_ix);
        }
    case PPA_GENERIC_HAL_DEL_MAC_ENTRY:
       {
           PPE_ROUTE_MAC_INFO *mac_info=(PPE_ROUTE_MAC_INFO *)buffer;
           del_mac_entry( mac_info->mac_ix);
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_GET_MAC_ENTRY:
       {
           PPE_ROUTE_MAC_INFO *mac_info=(PPE_ROUTE_MAC_INFO *)buffer;
           return get_mac_entry( mac_info->mac_ix, mac_info->mac);
        }
    case PPA_GENERIC_HAL_ADD_OUT_VLAN_ENTRY:
       {
           PPE_OUT_VLAN_INFO *vlan=(PPE_OUT_VLAN_INFO *)buffer;
           return add_outer_vlan_entry( vlan->vlan_id, &vlan->vlan_entry);
        }
    case PPA_GENERIC_HAL_DEL_OUT_VLAN_ENTRY:
       {
           PPE_OUT_VLAN_INFO *vlan=(PPE_OUT_VLAN_INFO *)buffer;
           del_outer_vlan_entry( vlan->vlan_entry);
           return IFX_SUCCESS;
        }
    case PPA_GENERIC_HAL_GET_OUT_VLAN_ENTRY:
       {
           PPE_OUT_VLAN_INFO *vlan=(PPE_OUT_VLAN_INFO *)buffer;
           return get_outer_vlan_entry( vlan->vlan_entry, &vlan->vlan_id);
        }
    case PPA_GENERIC_HAL_GET_ITF_MIB:
       {
           PPE_ITF_MIB_INFO *mib=(PPE_ITF_MIB_INFO *)buffer;
           get_itf_mib( mib->itf, &mib->mib);
           return IFX_SUCCESS;
        }
     case PPA_GENERIC_HAL_TEST_CLEAR_ROUTE_HIT_STAT:  //check whether a routing entry is hit or not
        {
            PPE_ROUTING_INFO *route=(PPE_ROUTING_INFO *)buffer;
            route->f_hit = test_and_clear_hit_stat( route->entry);
            return IFX_SUCCESS;
        }
     case PPA_GENERIC_HAL_TEST_CLEAR_BR_HIT_STAT:  //check whether a bridge mac entry is hit or not
        {
            PPE_BR_MAC_INFO *br_mac=(PPE_BR_MAC_INFO *)buffer;
            br_mac->f_hit = test_and_clear_bridging_hit_stat( br_mac->p_entry);
            return IFX_SUCCESS;
        }
     case PPA_GENERIC_HAL_INIT: //init HAL
        {
            return ifx_ppa_ppe_hal_init();
        }
     case PPA_GENERIC_HAL_EXIT: //EXIT HAL
        {
            ifx_ppa_ppe_hal_exit();
            return IFX_SUCCESS;
        }



//Fix warning message when exports API from different PPE FW Driver--End

    default:
        dbg("ppa_hal_generic_hook not support cmd 0x%x\n", cmd );
        return IFX_FAILURE;
    }

    return IFX_FAILURE;

}



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

static INLINE void hal_init(void)
{
    proc_file_create();

    ppa_lock_init(&g_lan_routing_lock);
    ppa_lock_init(&g_wan_routing_lock);
    ppa_lock_init(&g_wan_mc_lock);
    ppa_lock_init(&g_bridging_lock);
    ppa_lock_init(&g_pppoe_lock);
    ppa_lock_init(&g_mtu_lock);
    ppa_lock_init(&g_mac_lock);
    ppa_lock_init(&g_outer_vlan_lock);
    ppa_lock_init(&g_itf_cfg_lock);
}

static INLINE void hal_exit(void)
{
    proc_file_delete();

    ppa_lock_destroy(&g_lan_routing_lock);
    ppa_lock_destroy(&g_wan_routing_lock);
    ppa_lock_destroy(&g_wan_mc_lock);
    ppa_lock_destroy(&g_bridging_lock);
    ppa_lock_destroy(&g_pppoe_lock);
    ppa_lock_destroy(&g_mtu_lock);
    ppa_lock_destroy(&g_mac_lock);
    ppa_lock_destroy(&g_outer_vlan_lock);
    ppa_lock_destroy(&g_itf_cfg_lock);
}

#if defined(CONFIG_DANUBE_PPA_A4_API)

static int __init ifx_ppa_ppe_a4_hal_init(void)
{
    hal_init();

    return 0;
}

static void __exit ifx_ppa_ppe_a4_hal_exit(void)
{
    hal_exit();
}

module_init(ifx_ppa_ppe_a4_hal_init);
module_exit(ifx_ppa_ppe_a4_hal_exit);

#elif defined(CONFIG_DANUBE_PPA_A4_API_MODULE)

void ifx_ppa_ppe_a4_hal_init(void)
{
    hal_init();
}

void ifx_ppa_ppe_a4_hal_exit(void)
{
    hal_exit();
}

#else

int ifx_ppa_ppe_hal_init(void)
{
    hal_init();

    return IFX_SUCCESS;
}

void ifx_ppa_ppe_hal_exit(void)
{
    hal_exit();
}

#endif

MODULE_LICENSE("IFX");

static int __init ppe_hal_init(void)
{
	ifx_ppa_drv_hal_generic_hook = ppa_hal_generic_hook;
	return 0;
}
static void __exit ppe_hal_exit(void)
{
	ifx_ppa_drv_hal_generic_hook = NULL;
}
module_init(ppe_hal_init);
module_exit(ppe_hal_exit);
