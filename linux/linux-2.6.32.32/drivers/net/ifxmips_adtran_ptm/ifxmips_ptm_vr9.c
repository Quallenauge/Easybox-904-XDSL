/******************************************************************************
**
** FILE NAME    : ifxmips_ptm_vr9.c
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : PTM driver common source file (core functions)
** COPYRIGHT    :       Copyright (c) 2006
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
** 07 JUL 2009  Xu Liang        Init Version
*******************************************************************************/



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
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <asm/delay.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
//#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
//#include <asm/ifx/ifx_pmu.h>
#ifdef MODULE
  #include <asm/ifx/ifx_rcu.h>
#endif
#include <asm/ifx/ifx_clk.h>
#include "ifxmips_ptm_vdsl.h"
#include "ifxmips_ptm_fw_vr9.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
  #undef  WAN_TX_DESC_NUM_TOTAL
  #define WAN_TX_DESC_NUM_TOTAL             64

 #if !defined(OFFCHIP_BONDING_TEST) || !OFFCHIP_BONDING_TEST
  #define RX_DATA_ADDR(x)                   (unsigned int)CDM_DATA_MEMORY(1, ((x) % 8) * 0x90)      //  x < 64
  #define CPU_TX_DATA_ADDR(x)               (unsigned int)CDM_DATA_MEMORY(1, ((x) % 16 + 8) * 0x90) //  x < 64
  #define QOS_TX_DATA_ADDR(x)               (unsigned int)CDM_DATA_MEMORY(1, ((x) % 16 + 24) * 0x90)//  x < 64
 #else
  #define BASE_DATA_ADDR                    (ppe_base_addr == IFX_PPE_ORG ? 0xA0F00000 : 0xB8160000)
  #define DATA_ADDR(x)                      ((unsigned int)BASE_DATA_ADDR + (x) * 2048)
  #define RX_DATA_ADDR(x)                   DATA_ADDR(x)            //  x < 64
  #define CPU_TX_DATA_ADDR(x)               DATA_ADDR((x) + 64)     //  x < 64
  #define QOS_TX_DATA_ADDR(x)               DATA_ADDR((x) + 128)    //  x < 64
 #endif
#endif



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Hardware Init/Uninit Functions
 */
static inline void init_pmu(void);
static inline void uninit_pmu(void);
static inline void reset_ppe(void);
static inline void init_pdma(void);
static inline void init_mailbox(void);
static inline void init_atm_tc(void);
static inline void clear_share_buffer(void);
#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
  static inline int init_tables(void);
#endif

/*
 *  External Functions
 */
#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
  extern int ifx_ptm_init_sw(unsigned int);
  extern void ifx_ptm_uninit_sw(void);
  extern void ifx_ptm_start_sw(void);
#endif



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
  static int g_wanqos_en = 1;
  static int g_queue_gamma_map[4] = {0xFF, 0x00, 0x00, 0x00};
#endif



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
  unsigned int ifx_vr9_addr_offset   = 0;
  unsigned int ifx_vr9_ppe_base_addr = IFX_PPE_ORG;
  unsigned int ifx_vr9_sw_base_addr  = VR9_SWIP_MACRO_ORG;
  unsigned int ifx_vr9_dma_base_addr = IFX_DMA_ORG;
  unsigned int ifx_vr9_pmu_base_addr = IFX_PMU_ORG;
#endif



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static inline void init_pmu(void)
{
#if 0
    //*PMU_PWDCR &= ~((1 << 29) | (1 << 22) | (1 << 21) | (1 << 19) | (1 << 18));
    PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_SLL01_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_ENABLE);
    //PPE_QSB_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_ENABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_ENABLE);

    SWITCH_PMU_SETUP(IFX_PMU_ENABLE);
    DMA_PMU_SETUP(IFX_PMU_ENABLE);
#else
    //  PPE TOP, PPE EMA/PDMA, PPE TC, PPE SLL
    *IFX_PMU_PWDCR &= ~((1 << 29) | (1 << 22) | (1 << 21) | (1 << 19));
    //  Switch, DPLUS Slave, DPLUS Master
    *IFX_PMU_PWDCR &= ~((1 << 28) | (1 << 24) | (1 << 23));
    //  DMA
    *IFX_PMU_PWDCR &= ~(1 << 5);
#endif
}

static inline void uninit_pmu(void)
{
#if 0
    PPE_TOP_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_SLL01_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_DISABLE);
    //PPE_QSB_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_DISABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_DISABLE);

    SWITCH_PMU_SETUP(IFX_PMU_DISABLE);
    DMA_PMU_SETUP(IFX_PMU_ENABLE);
#endif
}

static inline void reset_ppe(void)
{
#ifdef MODULE
    //  reset PPE
    ifx_rcu_rst(IFX_RCU_DOMAIN_DSLDFE, IFX_RCU_MODULE_PTM);
    udelay(1000);
    ifx_rcu_rst(IFX_RCU_DOMAIN_DSLTC, IFX_RCU_MODULE_PTM);
    udelay(1000);
    ifx_rcu_rst(IFX_RCU_DOMAIN_PPE, IFX_RCU_MODULE_PTM);
    udelay(1000);
    *PP32_SRST &= ~0x000303CF;
    udelay(1000);
    *PP32_SRST |= 0x000303CF;
    udelay(1000);
#endif
}

static inline void init_pdma(void)
{
    IFX_REG_W32(0x00000001, PDMA_CFG);
    IFX_REG_W32(0x00082C00, PDMA_RX_CTX_CFG);
    IFX_REG_W32(0x00081B00, PDMA_TX_CTX_CFG);
    IFX_REG_W32(0x02040604, PDMA_RX_MAX_LEN_REG);
    IFX_REG_W32(0x000F003F, PDMA_RX_DELAY_CFG);

    IFX_REG_W32(0x00000011, SAR_MODE_CFG);
    IFX_REG_W32(0x00082A00, SAR_RX_CTX_CFG);
    IFX_REG_W32(0x00082E00, SAR_TX_CTX_CFG);
    IFX_REG_W32(0x00001021, SAR_POLY_CFG_SET0);
    IFX_REG_W32(0x1EDC6F41, SAR_POLY_CFG_SET1);
    IFX_REG_W32(0x04C11DB7, SAR_POLY_CFG_SET2);
    IFX_REG_W32(0x00000F3E, SAR_CRC_SIZE_CFG);

    IFX_REG_W32(0x01001900, SAR_PDMA_RX_CMDBUF_CFG);
    IFX_REG_W32(0x01001A00, SAR_PDMA_TX_CMDBUF_CFG);
}

static inline void init_mailbox(void)
{
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU1_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU1_IER);
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU3_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU3_IER);
}

static inline void init_atm_tc(void)
{
    IFX_REG_W32(0x00010040, SFSM_CFG0);
    IFX_REG_W32(0x00010040, SFSM_CFG1);
    IFX_REG_W32(0x00020000, SFSM_PGCNT0);
    IFX_REG_W32(0x00020000, SFSM_PGCNT1);
    IFX_REG_W32(0x00000000, DREG_AT_IDLE0);
    IFX_REG_W32(0x00000000, DREG_AT_IDLE1);
    IFX_REG_W32(0x00000000, DREG_AR_IDLE0);
    IFX_REG_W32(0x00000000, DREG_AR_IDLE1);
    IFX_REG_W32(0x0000080C, DREG_B0_LADR);
    IFX_REG_W32(0x0000080C, DREG_B1_LADR);

    IFX_REG_W32(0x000001F0, DREG_AR_CFG0);
    IFX_REG_W32(0x000001F0, DREG_AR_CFG1);
    IFX_REG_W32(0x000001E0, DREG_AT_CFG0);
    IFX_REG_W32(0x000001E0, DREG_AT_CFG1);

    /*  clear sync state    */
    //IFX_REG_W32(0, SFSM_STATE0);
    //IFX_REG_W32(0, SFSM_STATE1);

    IFX_REG_W32_MASK(0, 1 << 14, SFSM_CFG0);    //  enable SFSM storing
    IFX_REG_W32_MASK(0, 1 << 14, SFSM_CFG1);

    IFX_REG_W32_MASK(0, 1 << 15, SFSM_CFG0);    //  HW keep the IDLE cells in RTHA buffer
    IFX_REG_W32_MASK(0, 1 << 15, SFSM_CFG1);

    IFX_REG_W32(0xF0D10000, FFSM_IDLE_HEAD_BC0);
    IFX_REG_W32(0xF0D10000, FFSM_IDLE_HEAD_BC1);
    IFX_REG_W32(0x00030028, FFSM_CFG0);         //  Force_idle
    IFX_REG_W32(0x00030028, FFSM_CFG1);
}

static inline void clear_share_buffer(void)
{
    volatile u32 *p;
    unsigned int i;

    p = SB_RAM0_ADDR(0);
    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN + SB_RAM2_DWLEN + SB_RAM3_DWLEN; i++ )
        IFX_REG_W32(0, p++);

    p = SB_RAM6_ADDR(0);
    for ( i = 0; i < SB_RAM6_DWLEN; i++ )
        IFX_REG_W32(0, p++);
}

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
static inline int init_tables(void)
{
    struct cfg_std_data_len cfg_std_data_len = {0};
    struct tx_qos_cfg tx_qos_cfg = {0};
    struct psave_cfg psave_cfg = {0};
    struct eg_bwctrl_cfg eg_bwctrl_cfg = {0};
    struct test_mode test_mode = {0};
    struct rx_bc_cfg rx_bc_cfg = {0};
    struct tx_bc_cfg tx_bc_cfg = {0};
    struct gpio_mode gpio_mode = {0};
    struct gpio_wm_cfg gpio_wm_cfg = {0};
    struct rx_gamma_itf_cfg rx_gamma_itf_cfg = {0};
    struct tx_gamma_itf_cfg tx_gamma_itf_cfg = {0};
    struct wtx_qos_q_desc_cfg wtx_qos_q_desc_cfg = {0};
    struct rx_descriptor rx_desc = {0};
    struct tx_descriptor tx_desc = {0};
    unsigned int ppe_base_addr = ifx_vr9_ppe_base_addr;
    int i;

    cfg_std_data_len.byte_off = RX_HEAD_MAC_ADDR_ALIGNMENT; //  this field replaces byte_off in rx descriptor of VDSL ingress
    cfg_std_data_len.data_len = 1600;
    *CFG_STD_DATA_LEN = cfg_std_data_len;

    tx_qos_cfg.time_tick    = cgu_get_pp32_clock() / 62500; //  16 * (cgu_get_pp32_clock() / 1000000)
    tx_qos_cfg.overhd_bytes = 0;
    tx_qos_cfg.eth1_eg_qnum = __ETH_WAN_TX_QUEUE_NUM;
    tx_qos_cfg.eth1_burst_chk = 1;
    tx_qos_cfg.eth1_qss     = 0;
    tx_qos_cfg.shape_en     = 0;    //  disable
    tx_qos_cfg.wfq_en       = 0;    //  strict priority
    *TX_QOS_CFG = tx_qos_cfg;

    psave_cfg.start_state   = 0;
    psave_cfg.sleep_en      = 1;    //  enable sleep mode
    *PSAVE_CFG = psave_cfg;

    eg_bwctrl_cfg.fdesc_wm  = 16;
    eg_bwctrl_cfg.class_len = 128;
    *EG_BWCTRL_CFG = eg_bwctrl_cfg;

    *GPIO_ADDR = (unsigned int)IFX_GPIO_P0_OUT;

    gpio_mode.gipo_bit_bc1 = 2;
    gpio_mode.gipo_bit_bc0 = 1;
    gpio_mode.gipo_bc1_en  = 0;
    gpio_mode.gipo_bc0_en  = 1;
    *GPIO_MODE = gpio_mode;

    gpio_wm_cfg.stop_wm_bc1  = 2;
    gpio_wm_cfg.start_wm_bc1 = 4;
    gpio_wm_cfg.stop_wm_bc0  = 2;
    gpio_wm_cfg.start_wm_bc0 = 4;
    *GPIO_WM_CFG = gpio_wm_cfg;

    test_mode.mib_clear_mode    = 0;
    test_mode.test_mode         = 0;
    *TEST_MODE = test_mode;

    rx_bc_cfg.local_state   = 0;
    rx_bc_cfg.remote_state  = 0;
    rx_bc_cfg.to_false_th   = 7;
    rx_bc_cfg.to_looking_th = 3;
    *RX_BC_CFG(0) = rx_bc_cfg;
    *RX_BC_CFG(1) = rx_bc_cfg;

    tx_bc_cfg.fill_wm   = 2;
    tx_bc_cfg.uflw_wm   = 2;
    *TX_BC_CFG(0) = tx_bc_cfg;
    *TX_BC_CFG(1) = tx_bc_cfg;

    rx_gamma_itf_cfg.receive_state      = 0;
    rx_gamma_itf_cfg.rx_min_len         = 0;
    rx_gamma_itf_cfg.rx_pad_en          = 0;
    rx_gamma_itf_cfg.rx_eth_fcs_ver_dis = 1;
    rx_gamma_itf_cfg.rx_rm_eth_fcs      = 0;
    rx_gamma_itf_cfg.rx_tc_crc_ver_dis  = 0;
    rx_gamma_itf_cfg.rx_tc_crc_size     = 1;
    rx_gamma_itf_cfg.rx_eth_fcs_result  = 0xC704DD7B;
    rx_gamma_itf_cfg.rx_tc_crc_result   = 0x1D0F1D0F;
    rx_gamma_itf_cfg.rx_crc_cfg         = 0x2502;
    rx_gamma_itf_cfg.rx_eth_fcs_init_value  = 0xFFFFFFFF;
    rx_gamma_itf_cfg.rx_tc_crc_init_value   = 0x0000FFFF;
    rx_gamma_itf_cfg.rx_max_len_sel     = 0;
    rx_gamma_itf_cfg.rx_edit_num2       = 0;
    rx_gamma_itf_cfg.rx_edit_pos2       = 0;
    rx_gamma_itf_cfg.rx_edit_type2      = 0;
    rx_gamma_itf_cfg.rx_edit_en2        = 0;
    rx_gamma_itf_cfg.rx_edit_num1       = 0;
    rx_gamma_itf_cfg.rx_edit_pos1       = 0;
    rx_gamma_itf_cfg.rx_edit_type1      = 0;
    rx_gamma_itf_cfg.rx_edit_en1        = 0;
    rx_gamma_itf_cfg.rx_inserted_bytes_1l   = 0;
    rx_gamma_itf_cfg.rx_inserted_bytes_1h   = 0;
    rx_gamma_itf_cfg.rx_inserted_bytes_2l   = 0;
    rx_gamma_itf_cfg.rx_inserted_bytes_2h   = 0;
    rx_gamma_itf_cfg.rx_len_adj         = -2;
    for ( i = 0; i < 4; i++ )
        *RX_GAMMA_ITF_CFG(i) = rx_gamma_itf_cfg;

    tx_gamma_itf_cfg.tx_len_adj         = 2;
    tx_gamma_itf_cfg.tx_crc_off_adj     = 6;
    tx_gamma_itf_cfg.tx_min_len         = 0;
    tx_gamma_itf_cfg.tx_eth_fcs_gen_dis = 1;
    tx_gamma_itf_cfg.tx_tc_crc_size     = 1;
    tx_gamma_itf_cfg.tx_crc_cfg         = 0x2F02;
    tx_gamma_itf_cfg.tx_eth_fcs_init_value  = 0xFFFFFFFF;
    tx_gamma_itf_cfg.tx_tc_crc_init_value   = 0x0000FFFF;
#ifdef CO_VERIFICATION
    tx_gamma_itf_cfg.tx_len_adj         = 6;
    tx_gamma_itf_cfg.tx_eth_fcs_gen_dis = 0;
    tx_gamma_itf_cfg.tx_crc_cfg         = 0x2F00;
#endif
    for ( i = 0; i < NUM_ENTITY(g_queue_gamma_map); i++ ) {
        tx_gamma_itf_cfg.queue_mapping = g_queue_gamma_map[i];
        *TX_GAMMA_ITF_CFG(i) = tx_gamma_itf_cfg;
    }

    for ( i = 0; i < __ETH_WAN_TX_QUEUE_NUM; i++ ) {
        wtx_qos_q_desc_cfg.length = WAN_TX_DESC_NUM;
        wtx_qos_q_desc_cfg.addr   = __ETH_WAN_TX_DESC_BASE(i);
        *WTX_QOS_Q_DESC_CFG(i) = wtx_qos_q_desc_cfg;
    }

    //  default TX queue QoS config is all ZERO

    //  TX Ctrl K Table
    IFX_REG_W32(0x90111293, TX_CTRL_K_TABLE(0));
    IFX_REG_W32(0x14959617, TX_CTRL_K_TABLE(1));
    IFX_REG_W32(0x18999A1B, TX_CTRL_K_TABLE(2));
    IFX_REG_W32(0x9C1D1E9F, TX_CTRL_K_TABLE(3));
    IFX_REG_W32(0xA02122A3, TX_CTRL_K_TABLE(4));
    IFX_REG_W32(0x24A5A627, TX_CTRL_K_TABLE(5));
    IFX_REG_W32(0x28A9AA2B, TX_CTRL_K_TABLE(6));
    IFX_REG_W32(0xAC2D2EAF, TX_CTRL_K_TABLE(7));
    IFX_REG_W32(0x30B1B233, TX_CTRL_K_TABLE(8));
    IFX_REG_W32(0xB43536B7, TX_CTRL_K_TABLE(9));
    IFX_REG_W32(0xB8393ABB, TX_CTRL_K_TABLE(10));
    IFX_REG_W32(0x3CBDBE3F, TX_CTRL_K_TABLE(11));
    IFX_REG_W32(0xC04142C3, TX_CTRL_K_TABLE(12));
    IFX_REG_W32(0x44C5C647, TX_CTRL_K_TABLE(13));
    IFX_REG_W32(0x48C9CA4B, TX_CTRL_K_TABLE(14));
    IFX_REG_W32(0xCC4D4ECF, TX_CTRL_K_TABLE(15));

    //  init RX descriptor
    rx_desc.own     = 1;
    rx_desc.c       = 0;
    rx_desc.sop     = 1;
    rx_desc.eop     = 1;
    rx_desc.byteoff = RX_HEAD_MAC_ADDR_ALIGNMENT;
    rx_desc.datalen = RX_MAX_BUFFER_SIZE - RX_HEAD_MAC_ADDR_ALIGNMENT;
    for ( i = 0; i < WAN_RX_DESC_NUM; i++ ) {
        ifx_vr9_ppe_base_addr = IFX_PPE_ORG;
        rx_desc.dataptr = RX_DATA_ADDR(i) & 0x1FFFFFFF;
        ifx_vr9_ppe_base_addr = ppe_base_addr;
        WAN_RX_DESC_BASE[i] = rx_desc;
    }

    //  init TX descriptor
    tx_desc.own     = 0;
    tx_desc.c       = 0;
    tx_desc.sop     = 1;
    tx_desc.eop     = 1;
    tx_desc.byteoff = 0;
    tx_desc.qid     = 0;
    tx_desc.datalen = 0;
    tx_desc.small   = 0;
    for ( i = 0; i < CPU_TO_WAN_TX_DESC_NUM; i++ ) {
        CPU_TO_WAN_TX_DESC_BASE[i] = tx_desc;
    }
    for ( i = 0; i < WAN_TX_DESC_NUM_TOTAL; i++ ) {
        ifx_vr9_ppe_base_addr = IFX_PPE_ORG;
        tx_desc.dataptr = QOS_TX_DATA_ADDR(i) & 0x1FFFFFFF;
        ifx_vr9_ppe_base_addr = ppe_base_addr;
        WAN_TX_DESC_BASE(0)[i] = tx_desc;
    }

    //  init Swap descriptor
    for ( i = 0; i < WAN_SWAP_DESC_NUM; i++ )
        WAN_SWAP_DESC_BASE[i] = tx_desc;

    //  init fastpath TX descriptor
    tx_desc.own     = 1;
    for ( i = 0; i < FASTPATH_TO_WAN_TX_DESC_NUM; i++ ) {
        ifx_vr9_ppe_base_addr = IFX_PPE_ORG;
        tx_desc.dataptr = CPU_TX_DATA_ADDR(i) & 0x1FFFFFFF;
        ifx_vr9_ppe_base_addr = ppe_base_addr;
        FASTPATH_TO_WAN_TX_DESC_BASE[i] = tx_desc;
    }

    return IFX_SUCCESS;
}
#endif

/*
 *  Description:
 *    Download PPE firmware binary code.
 *  Input:
 *    pp32      --- int, which pp32 core
 *    src       --- u32 *, binary code buffer
 *    dword_len --- unsigned int, binary code length in DWORD (32-bit)
 *  Output:
 *    int       --- IFX_SUCCESS:    Success
 *                  else:           Error Code
 */
static inline int pp32_download_code(int pp32, u32 *code_src, unsigned int code_dword_len, u32 *data_src, unsigned int data_dword_len)
{
#if !defined(ENABLE_ADTRAN) || !ENABLE_ADTRAN
    unsigned int clr, set;
#endif
    volatile u32 *dest;

    if ( code_src == 0 || ((unsigned long)code_src & 0x03) != 0
        || data_src == 0 || ((unsigned long)data_src & 0x03) != 0 )
        return IFX_ERROR;

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
    if ( pp32 != 0 )    //  E1 firmware must run on PPM0
        return IFX_ERROR;

    if ( code_dword_len > CDM_CODE_MEMORYn_DWLEN(0) )
        return IFX_ERROR;

    IFX_REG_W32(0xFC, CDM_CFG); //  RAM1/2/3 is PPM1 data memory
#else
    clr = pp32 ? 0xF0 : 0x0F;
    if ( code_dword_len <= CDM_CODE_MEMORYn_DWLEN(0) )
        set = pp32 ? (3 << 6): (2 << 2);
    else
        set = 0x00;
    IFX_REG_W32_MASK(clr, set, CDM_CFG);
#endif

    /*  copy code   */
    dest = CDM_CODE_MEMORY(pp32, 0);
    while ( code_dword_len-- > 0 )
        IFX_REG_W32(*code_src++, dest++);

    /*  copy data   */
    dest = CDM_DATA_MEMORY(pp32, 0);
    while ( data_dword_len-- > 0 )
        IFX_REG_W32(*data_src++, dest++);

    return IFX_SUCCESS;
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

void ifx_ptm_get_fw_ver(unsigned int *major, unsigned int *minor)
{
    ASSERT(major != NULL, "pointer is NULL");
    ASSERT(minor != NULL, "pointer is NULL");

    *major = FW_VER_ID->major;
    *minor = FW_VER_ID->minor;
}

void ifx_ptm_set_base_address(unsigned int ppe_base)
{
    if ( ppe_base ) {
        ppe_base = CPHYSADDR(ppe_base);
        ifx_vr9_addr_offset = ppe_base - IFX_PPE_ORG;
        ifx_vr9_ppe_base_addr = ppe_base;
        ifx_vr9_sw_base_addr  = ifx_vr9_addr_offset + VR9_SWIP_MACRO_ORG;
        ifx_vr9_dma_base_addr = ifx_vr9_addr_offset + IFX_DMA_ORG;
        ifx_vr9_pmu_base_addr = ifx_vr9_addr_offset + IFX_PMU_ORG;
    }
    else {
        ifx_vr9_addr_offset   = 0;
        ifx_vr9_ppe_base_addr = IFX_PPE_ORG;
        ifx_vr9_sw_base_addr  = ifx_vr9_addr_offset + VR9_SWIP_MACRO_ORG;
        ifx_vr9_dma_base_addr = ifx_vr9_addr_offset + IFX_DMA_ORG;
        ifx_vr9_pmu_base_addr = ifx_vr9_addr_offset + IFX_PMU_ORG;
    }
}

unsigned int ifx_ptm_get_base_address(void)
{
    return ifx_vr9_ppe_base_addr;
}

int __init ifx_ptm_init_chip(unsigned int lan_port)
{
    init_pmu();

    reset_ppe();

    init_pdma();

    init_mailbox();

    init_atm_tc();

    clear_share_buffer();

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
    if ( ifx_ptm_init_sw(lan_port) != IFX_SUCCESS )
        return IFX_ERROR;

    init_tables();

    ifx_ptm_start_sw();
#endif

    return IFX_SUCCESS;
}

void __exit ifx_ptm_uninit_chip(void)
{
    ifx_ptm_uninit_sw();

    uninit_pmu();
}

/*
 *  Description:
 *    Initialize and start up PP32.
 *  Input:
 *    none
 *  Output:
 *    int  --- IFX_SUCCESS: Success
 *             else:        Error Code
 */
int ifx_pp32_start(int pp32)
{
    unsigned int mask = 1 << (pp32 << 4);
    int ret;

    /*  download firmware   */
    ret = pp32_download_code(pp32, firmware_binary_code, sizeof(firmware_binary_code) / sizeof(*firmware_binary_code), firmware_binary_data, sizeof(firmware_binary_data) / sizeof(*firmware_binary_data));
    if ( ret != IFX_SUCCESS )
        return ret;

    /*  run PP32    */
    IFX_REG_W32_MASK(mask, 0, PP32_FREEZE);

    /*  idle for a while to let PP32 init itself    */
    udelay(10);

    return IFX_SUCCESS;
}

/*
 *  Description:
 *    Halt PP32.
 *  Input:
 *    none
 *  Output:
 *    none
 */
void ifx_pp32_stop(int pp32)
{
    unsigned int mask = 1 << (pp32 << 4);

    /*  halt PP32   */
    IFX_REG_W32_MASK(0, mask, PP32_FREEZE);
}
