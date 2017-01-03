/******************************************************************************
**
** FILE NAME    : ifxmips_ptm_vr9_sw.c
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : PTM driver common source file (Switch functions)
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
//#include <asm/ifx/ifx_dma_core.h>
#include "ifxmips_ptm_vdsl.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */

//static int dma_int_handler(struct dma_device_info *, int);

/*
 *  Hardware Init/Uninit Functions
 */
static inline int init_dma(void);
static inline void uninit_dma(void);
static inline int init_sw(unsigned int);
static inline void uninit_sw(void);
static inline void sw_download_micro_code(void);



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

//static struct dma_device_info  *g_dma_device = NULL;
static DEFINE_SPINLOCK(g_dma_lock);

/*
 *  Switch PCE Micro Code
 */
static IFX_FLOW_PCE_MICROCODE pce_mc_max_ifx_tag_m = {
    //------------------------------------------------------------------------------------------
    //                value    mask   ns  out_fields   L  type     flags       ipv4_len
    //------------------------------------------------------------------------------------------
    IFX_FLOW_PCE_MC_M(0x88c3, 0xFFFF, 1 , OUT_ITAG0 ,  4, INSTR  , FLAG_ITAG , 0),   // 0 : IFXTAG
    IFX_FLOW_PCE_MC_M(0x8100, 0xFFFF, 2 , OUT_VTAG0 ,  2, INSTR  , FLAG_VLAN , 0),   // 1 : C_VTAG
    IFX_FLOW_PCE_MC_M(0x0800, 0xFFFF, 20, OUT_ETHTYP,  1, INSTR  , FLAG_NO   , 0),   // 2 : ET_IPV4
    IFX_FLOW_PCE_MC_M(0x86DD, 0xFFFF, 21, OUT_ETHTYP,  1, INSTR  , FLAG_NO   , 0),   // 3 : ET_IPV6
    IFX_FLOW_PCE_MC_M(0x8864, 0xFFFF, 17, OUT_ETHTYP,  1, INSTR  , FLAG_NO   , 0),   // 4 : ET_PPPOE_S
    IFX_FLOW_PCE_MC_M(0x8863, 0xFFFF, 16, OUT_ETHTYP,  1, INSTR  , FLAG_NO   , 0),   // 5 : ET_PPPOE_D
    IFX_FLOW_PCE_MC_M(0x88A8, 0xFFFF, 1 , OUT_VTAG0 ,  2, INSTR  , FLAG_VLAN , 0),   // 6 : S_VTAG
    IFX_FLOW_PCE_MC_M(0x8100, 0xFFFF, 1 , OUT_VTAG0 ,  2, INSTR  , FLAG_VLAN , 0),   // 7 : C_VTAG2
    IFX_FLOW_PCE_MC_M(0x0000, 0xF800, 10, OUT_NONE  ,  0, INSTR  , FLAG_NO   , 0),   // 8 : EL_LES_800
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 47, OUT_ETHTYP,  1, INSTR  , FLAG_NO   , 0),   // 9 : ET_OTHER
    IFX_FLOW_PCE_MC_M(0x0600, 0x0600, 49, OUT_ETHTYP,  1, INSTR  , FLAG_NO   , 0),   // 10: EL_GRE_600
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 12, OUT_NONE  ,  1, INSTR  , FLAG_NO   , 0),   // 11: EL_LES_600
    IFX_FLOW_PCE_MC_M(0xAAAA, 0xFFFF, 14, OUT_NONE  ,  1, INSTR  , FLAG_NO   , 0),   // 12: SNAP1
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_NO   , 0),   // 13: NO_SNAP1
    IFX_FLOW_PCE_MC_M(0x0300, 0xFF00, 49, OUT_NONE  ,  0, INSTR  , FLAG_SNAP , 0),   // 14: SNAP2
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_NO   , 0),   // 15: NO_SNAP2
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 18, OUT_NONE  ,  3, INSTR  , FLAG_NO   , 0),   // 16: SESID_IGN
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 18, OUT_DIP7  ,  3, INSTR  , FLAG_PPPOE, 0),   // 17: SESID
    IFX_FLOW_PCE_MC_M(0x0021, 0xFFFF, 20, OUT_NONE  ,  1, INSTR  , FLAG_NO   , 0),   // 18: PPPOE_IP
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_NO   , 0),   // 19: PPPOE_NOIP
    IFX_FLOW_PCE_MC_M(0x4000, 0xF000, 23, OUT_IP0   ,  4, INSTR  , FLAG_IPV4 , 1),   // 20: IPV4_VER
    IFX_FLOW_PCE_MC_M(0x6000, 0xF000, 34, OUT_IP0   ,  3, INSTR  , FLAG_IPV6 , 0),   // 21: IPV6_VER
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_NO   , 0),   // 22: NO_IP
    IFX_FLOW_PCE_MC_M(0x0011, 0x00FF, 27, OUT_IP3   ,  2, INSTR  , FLAG_NO   , 0),   // 23: IPV4_UDP1
    IFX_FLOW_PCE_MC_M(0x0006, 0x00FF, 27, OUT_IP3   ,  2, INSTR  , FLAG_NO   , 0),   // 24: IPV4_TCP
    IFX_FLOW_PCE_MC_M(0x0002, 0x00FF, 31, OUT_IP3   ,  2, INSTR  , FLAG_NO   , 0),   // 25: IPV4_IGMP1
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 29, OUT_IP3   ,  2, INSTR  , FLAG_NO   , 0),   // 26: IPV4_OTH1
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 28, OUT_SIP0  ,  4, INSTR  , FLAG_NO   , 0),   // 27: IPV4_UDP2
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 47, OUT_NONE  ,  0, LENACCU, FLAG_NO   , 0),   // 28: IPV4_UDP3
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 30, OUT_SIP0  ,  4, INSTR  , FLAG_NO   , 0),   // 29: IPV4_OTH2
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_NO   , 0),   // 30: IPV4_OTH3
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 32, OUT_SIP0  ,  4, INSTR  , FLAG_NO   , 0),   // 31: IPV4_IGMP2
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 33, OUT_NONE  ,  0, LENACCU, FLAG_NO   , 0),   // 32: IPV4_IGMP3
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_APP0  ,  4, INSTR  , FLAG_IGMP , 0),   // 33: IPV4_IGMP4
    IFX_FLOW_PCE_MC_M(0x1100, 0xFF00, 46, OUT_PROT  ,  1, INSTR  , FLAG_NO   , 0),   // 34: IPV6_UDP
    IFX_FLOW_PCE_MC_M(0x0600, 0xFF00, 46, OUT_PROT  ,  1, INSTR  , FLAG_NO   , 0),   // 35: IPV6_TCP
    IFX_FLOW_PCE_MC_M(0x0000, 0xFF00, 40, OUT_IP3   , 17, INSTR  , FLAG_HOP  , 0),   // 36: IPV6_HOP
    IFX_FLOW_PCE_MC_M(0x2B00, 0xFF00, 40, OUT_IP3   , 17, INSTR  , FLAG_NN1  , 0),   // 37: IPV6_ROU
    IFX_FLOW_PCE_MC_M(0x3C00, 0xFF00, 40, OUT_IP3   , 17, INSTR  , FLAG_NN2  , 0),   // 38: IPV6_DES
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 48, OUT_PROT  ,  1, INSTR  , FLAG_NO   , 0),   // 39: IPV6_OTH
    IFX_FLOW_PCE_MC_M(0x1100, 0xFF00, 47, OUT_PROT  ,  0, IPV6   , FLAG_NO   , 0),   // 40: NXT_HD_UDP
    IFX_FLOW_PCE_MC_M(0x0600, 0xFF00, 47, OUT_PROT  ,  0, IPV6   , FLAG_NO   , 0),   // 41: NXT_HD_TCP
    IFX_FLOW_PCE_MC_M(0x0000, 0xFF00, 40, OUT_NONE  ,  0, IPV6   , FLAG_HOP  , 0),   // 42: NXT_HD_HOP
    IFX_FLOW_PCE_MC_M(0x2B00, 0xFF00, 40, OUT_NONE  ,  0, IPV6   , FLAG_NN1  , 0),   // 43: NXT_HD_ROU
    IFX_FLOW_PCE_MC_M(0x3C00, 0xFF00, 40, OUT_NONE  ,  0, IPV6   , FLAG_NN2  , 0),   // 44: NXT_HD_DES
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_PROT  ,  1, INSTR  , FLAG_NO   , 0),   // 45: NXT_HD_OTH
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 47, OUT_SIP0  , 16, INSTR  , FLAG_NO   , 0),   // 46: TU_IP
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_APP0  ,  2, INSTR  , FLAG_TU   , 0),   // 47: TU_PORTS
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_SIP0  , 16, INSTR  , FLAG_NO   , 0),   // 48: IPV6_IP
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 49: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 50: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 51: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 52: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 53: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 54: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 55: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 56: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 57: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 58: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 59: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 60: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 61: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0),   // 62: END
    IFX_FLOW_PCE_MC_M(0x0000, 0x0000, 49, OUT_NONE  ,  0, INSTR  , FLAG_END  , 0)    // 63: END
};



/*
 * ####################################
 *           Global Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

#if 0
static int dma_int_handler(struct dma_device_info *dma_dev, int status)
{
    int ret = 0;
    //uint32_t sys_flag;

    switch ( status ) {
    case RCV_INT:
        break;
    case TX_BUF_FULL_INT:
        break;
    case TRANSMIT_CPT_INT:
        break;
    default:
        break;
    }

    return ret;
}
#endif

static inline int init_dma(void)
{
#if 0

    int i;

    g_dma_device = dma_device_reserve("PPE");
    if ( !g_dma_device )
        return IFX_ERROR;

    //g_dma_device->buffer_alloc    = dma_buffer_alloc;
    //g_dma_device->buffer_free     = dma_buffer_free;
    g_dma_device->intr_handler    = dma_int_handler;
    g_dma_device->tx_burst_len    = 8;
    g_dma_device->rx_burst_len    = 8;

    for ( i = 0; i < g_dma_device->max_rx_chan_num; i++ ) {
        g_dma_device->rx_chan[i]->packet_size = 0x90;
        g_dma_device->rx_chan[i]->control     = IFX_DMA_CH_OFF;
    }

    g_dma_device->rx_chan[0]->control   = IFX_DMA_CH_ON;
    g_dma_device->rx_chan[0]->desc_base = (int)FASTPATH_TO_WAN_TX_DESC_BASE;
    g_dma_device->rx_chan[0]->desc_len  = FASTPATH_TO_WAN_TX_DESC_NUM;

    for ( i = 0; i < g_dma_device->max_tx_chan_num; i++ ) {
        g_dma_device->tx_chan[i]->control     = IFX_DMA_CH_OFF;
    }
    g_dma_device->tx_chan[0]->control   = IFX_DMA_CH_ON;
    g_dma_device->tx_chan[0]->desc_base = (int)WAN_RX_DESC_BASE;
    g_dma_device->tx_chan[0]->desc_len  = WAN_RX_DESC_NUM;
    g_dma_device->tx_chan[0]->global_buffer_len = 0x90;
    g_dma_device->tx_chan[0]->peri_to_peri      = 1;

    if ( dma_device_register(g_dma_device) != IFX_SUCCESS ) {
        dma_device_release(g_dma_device);
        return IFX_ERROR;
    }
#else

    unsigned long sys_flag;
    unsigned int ppe_base_addr = ifx_vr9_ppe_base_addr;

    spin_lock_irqsave(&g_dma_lock, sys_flag);
    ifx_vr9_ppe_base_addr = IFX_PPE_ORG;

    *IFX_DMA_PS(0) = 0;
    *IFX_DMA_PCTRL(0) = (*IFX_DMA_PCTRL(0) & ~0x3C) | (3 << 4) | (3 << 2);  //  Tx/Rx burst length 8 (DWORD)
    *IFX_DMA_PCTRL(0) |= 0x0F00;    //  endiness

    *IFX_DMA_CS(0) = 0;
    *IFX_DMA_CCTRL(0) = 0x00010000;
    *IFX_DMA_CDBA(0)  = CPHYSADDR((int)FASTPATH_TO_WAN_TX_DESC_BASE);   //  0x1E221600
    *IFX_DMA_CDLEN(0) = FASTPATH_TO_WAN_TX_DESC_NUM;                    //  0x40
    *IFX_DMA_CGBL     = 0x90;

    *IFX_DMA_CS(0) = 1;
    *IFX_DMA_CCTRL(0) = 0x01010100;
    *IFX_DMA_CDBA(0)  = CPHYSADDR((int)WAN_RX_DESC_BASE);               //  0x1E221800
    *IFX_DMA_CDLEN(0) = WAN_RX_DESC_NUM;                                //  0x40
    *IFX_DMA_CGBL     = 0x90;

    ifx_vr9_ppe_base_addr = ppe_base_addr;
    spin_unlock_irqrestore(&g_dma_lock, sys_flag);

#endif

    return IFX_SUCCESS;
}

static inline void uninit_dma(void)
{
#if 0
    g_dma_device->rx_chan[0]->close(g_dma_device->rx_chan[0]);
    g_dma_device->tx_chan[0]->close(g_dma_device->tx_chan[0]);
    dma_device_release(g_dma_device);
    g_dma_device = NULL;
#else
    unsigned long sys_flag;

    spin_lock_irqsave(&g_dma_lock, sys_flag);
    *IFX_DMA_CS(0) = 0;
    *IFX_DMA_CCTRL(0) &= ~1;
    *IFX_DMA_CS(0) = 1;
    *IFX_DMA_CCTRL(0) &= ~1;
    spin_unlock_irqrestore(&g_dma_lock, sys_flag);
#endif
}

static inline int init_sw(unsigned int lan_port)
{
    int i;

    *GLOB_CTRL_REG = 1 << 15;   //  enable Switch

    for ( i = 0; i < 7; i++ )
        sw_clr_rmon_counter(i);

    //  disable FCS check on LAN port
    *SDMA_PCTRL_REG(lan_port) |= 1 << 5;

    //  disable FCS generation and padding on LAN port
    *MAC_CTRL_REG(lan_port, 0) = 0;

    //  enable CRC generation (from DMA to PMAC)
    //  enable CRC removal (from PMAC to DMA)
    //  disable CRC check (from DMA to PMAC)
    *PMAC_HD_CTL_REG = 0x414;

    sw_download_micro_code();

    //  forward "lan_port" packets to CPU port
    while ( (*PCE_TBL_CTRL & (1 << 15)) );
    *PCE_TBL_ADDR   = 0x0000;
    *PCE_TBL_KEY(7) = 0xFFFF;
    *PCE_TBL_KEY(6) = 0x7F0F;
    *PCE_TBL_KEY(5) = 0xFFFF;
    *PCE_TBL_KEY(4) = 0xFFFF;
    *PCE_TBL_KEY(3) = 0xFFFF;
    *PCE_TBL_KEY(2) = 0xFFFF;
    *PCE_TBL_KEY(1) = 0xFFFF;
    *PCE_TBL_KEY(0) = 0xFF00 | lan_port;
    *PCE_TBL_MASK   = 0x0000;
    *PCE_TBL_VAL(4) = 0x000C;
    *PCE_TBL_VAL(3) = 0xFF1F;
    *PCE_TBL_VAL(2) = 0x0000;
    *PCE_TBL_VAL(1) = 0x0040;
    *PCE_TBL_VAL(0) = 0x0001;
    *PCE_TBL_CTRL   = 0x902F;

    //  forward CPU port packets to "lan_port"
    while ( (*PCE_TBL_CTRL & (1 << 15)) );
    *PCE_TBL_ADDR   = 0x0001;
    *PCE_TBL_KEY(7) = 0xFFFF;
    *PCE_TBL_KEY(6) = 0x7F0F;
    *PCE_TBL_KEY(5) = 0xFFFF;
    *PCE_TBL_KEY(4) = 0xFFFF;
    *PCE_TBL_KEY(3) = 0xFFFF;
    *PCE_TBL_KEY(2) = 0xFFFF;
    *PCE_TBL_KEY(1) = 0xFFFF;
    *PCE_TBL_KEY(0) = 0xFF06;
    *PCE_TBL_MASK   = 0x0000;
    *PCE_TBL_VAL(4) = 0x000C;
    *PCE_TBL_VAL(3) = 0xFF1F;
    *PCE_TBL_VAL(2) = 0x0000;
    *PCE_TBL_VAL(1) = 1 << lan_port;
    *PCE_TBL_VAL(0) = 0x0001;
    *PCE_TBL_CTRL   = 0x902F;
    while ( (*PCE_TBL_CTRL & (1 << 15)) );

    //  class value 0 to "lan_port" ingress packet
    *PCE_PCTRL_REG(lan_port, 2) = 0x0000;

    return IFX_SUCCESS;
}

static inline void uninit_sw(void)
{
}

static inline void sw_download_micro_code(void)
{
    int i;

    for ( i = 0; i < IFX_FLOW_PCE_MICROCODE_VALUES; i++ ) {
        while ( (*PCE_TBL_CTRL & (1 << 15)) );
        *PCE_TBL_ADDR   = i;
        *PCE_TBL_VAL(3) = pce_mc_max_ifx_tag_m[i].val_3;
        *PCE_TBL_VAL(2) = pce_mc_max_ifx_tag_m[i].val_2;
        *PCE_TBL_VAL(1) = pce_mc_max_ifx_tag_m[i].val_1;
        *PCE_TBL_VAL(0) = pce_mc_max_ifx_tag_m[i].val_0;
        *PCE_TBL_CTRL   = 0x8020;
    }
    while ( (*PCE_TBL_CTRL & (1 << 15)) );
    *PCE_GCTRL_REG(0) = 0x0008; //  PCE parser microcode is valid

    //  traffic flow table entry 3 - time stamp action enable
    while ( (*PCE_TBL_CTRL & (1 << 15)) );
    *PCE_TBL_ADDR   = 0x0003;
    *PCE_TBL_KEY(7) = 0xFFFF;
    *PCE_TBL_KEY(6) = 0x7F0F;
    *PCE_TBL_KEY(5) = 0xFFFF;
    *PCE_TBL_KEY(4) = 0xFFFF;
    *PCE_TBL_KEY(3) = 0xFFFF;
    *PCE_TBL_KEY(2) = 0xFFFF;
    *PCE_TBL_KEY(1) = 0xFFFF;
    *PCE_TBL_KEY(0) = 0xFF0F;
    *PCE_TBL_MASK   = 0x0000;
    *PCE_TBL_VAL(4) = 0x0000;
    *PCE_TBL_VAL(3) = 0x0000;
    *PCE_TBL_VAL(2) = 0x0000;
    *PCE_TBL_VAL(1) = 0x0000;
    *PCE_TBL_VAL(0) = 0x0040;
    *PCE_TBL_CTRL   = 0x902F;
    while ( (*PCE_TBL_CTRL & (1 << 15)) );

    *PCE_PMAP_REG(2) = 0x007F;  //  default multicast port map
    *PCE_PMAP_REG(3) = 0x007F;  //  default unknown unicast port map
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

int ifx_ptm_init_sw(unsigned int lan_port)
{
    int ret;

    ret = init_dma();
    if ( ret != IFX_SUCCESS )
        goto INIT_DMA_FAIL;

    ret = init_sw(lan_port);
    if ( ret != IFX_SUCCESS )
        goto INIT_SW_FAIL;

    return IFX_SUCCESS;

INIT_SW_FAIL:
    uninit_dma();
INIT_DMA_FAIL:
    return ret;
}

void ifx_ptm_uninit_sw(void)
{
    uninit_sw();

    uninit_dma();
}

void ifx_ptm_start_sw(void)
{
#if 0
    g_dma_device->rx_chan[0]->open(g_dma_device->rx_chan[0]);
    g_dma_device->tx_chan[0]->open(g_dma_device->tx_chan[0]);
#else
    unsigned long sys_flag;

    spin_lock_irqsave(&g_dma_lock, sys_flag);
    *IFX_DMA_CS(0) = 0;
    *IFX_DMA_CCTRL(0) |= 1;
    *IFX_DMA_CS(0) = 1;
    *IFX_DMA_CCTRL(0) |= 1;
    spin_unlock_irqrestore(&g_dma_lock, sys_flag);
#endif
}
