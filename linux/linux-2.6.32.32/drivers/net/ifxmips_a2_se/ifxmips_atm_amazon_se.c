/******************************************************************************
**
** FILE NAME    : ifxmips_atm_amazon_se.c
** PROJECT      : UEIP
** MODULES      : ATM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM driver common source file (core functions)
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
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_rcu.h>
#include "ifxmips_atm_core.h"
#include "ifxmips_atm_fw_amazon_se.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  EMA Settings
 */
#define EMA_CMD_BUF_LEN      0x0040
#define EMA_CMD_BASE_ADDR    (0x00001580 << 2)
#define EMA_DATA_BUF_LEN     0x0100
#define EMA_DATA_BASE_ADDR   (0x00000B00 << 2)
#define EMA_WRITE_BURST      0x2
#define EMA_READ_BURST       0x2



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
static inline void init_gpio(int);
static inline void init_etop(int);
static inline void init_ema(void);
static inline void start_etop(void);
static inline void init_mailbox(void);
static inline void init_atm_tc(void);
static inline void clear_share_buffer(void);



/*
 * ####################################
 *            Local Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static inline void init_pmu(void)
{
    //*(unsigned long *)0xBF10201C &= ~((1 << 15) | (1 << 13) | (1 << 9));
    //PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_SLL01_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_ENABLE);
    //PPE_QSB_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_ENABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_ENABLE);
#if MII0_MODE_SETUP == EPHY_MODE
    EPHY_PMU_SETUP(IFX_PMU_ENABLE);
#endif
}

static inline void uninit_pmu(void)
{
    PPE_SLL01_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_TC_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_EMA_PMU_SETUP(IFX_PMU_DISABLE);
    //PPE_QSB_PMU_SETUP(IFX_PMU_DISABLE);
    PPE_TPE_PMU_SETUP(IFX_PMU_DISABLE);
    DSL_DFE_PMU_SETUP(IFX_PMU_DISABLE);
    //PPE_TOP_PMU_SETUP(IFX_PMU_DISABLE);
}

static inline void reset_ppe(void)
{
#ifdef MODULE
    unsigned int etop_cfg;
    unsigned int etop_mdio_cfg;
    unsigned int etop_ig_plen_ctrl;
    unsigned int enet_mac_cfg;

    etop_cfg            = *IFX_PP32_ETOP_CFG;
    etop_mdio_cfg       = *IFX_PP32_ETOP_MDIO_CFG;
    etop_ig_plen_ctrl   = *IFX_PP32_ETOP_IG_PLEN_CTRL;
    enet_mac_cfg        = *IFX_PP32_ENET_MAC_CFG;

    *IFX_PP32_ETOP_CFG  = (*IFX_PP32_ETOP_CFG & ~0x03C0) | 0x0001;

    //  reset PPE
    ifx_rcu_rst(IFX_RCU_DOMAIN_PPE, IFX_RCU_MODULE_ATM);

    *IFX_PP32_ETOP_MDIO_CFG     = etop_mdio_cfg;
    *IFX_PP32_ETOP_IG_PLEN_CTRL = etop_ig_plen_ctrl;
    *IFX_PP32_ENET_MAC_CFG      = enet_mac_cfg;
    *IFX_PP32_ETOP_CFG          = etop_cfg;
#endif
}

static inline void init_gpio(int mii_mode)
{
    if ( mii_mode != RED_MII_MODE )
        return;

#define eth_gpio_module_id          IFX_GPIO_MODULE_INTERNAL_SWITCH

/** change the GPIO pins based on HW changes*/
/** IFX_RMIITXD0       GPIO 0 */
#define IFX_RMIITXD0                0
/** IFX_RMIITXD1       GPIO 4 */
#define IFX_RMIITXD1                4
/** IFX_TXEN     GPIO 13*/
#define IFX_TXEN                    13
/** IFX_MDC       GPIO 27 */
#define IFX_MDC                     27
/** IFX_MDIO       GPIO 24 */
#define IFX_MDIO                    24
/** IFX_RMIIRXD0  P0.9/P1.9  GPIO 9/25 */
#define IFX_RMIIRXD0                25
/** IFX_RMIIRXD1  P0.8/P1.7  GPIO 8/23 */
#define IFX_RMIIRXD1                23
/*IFX_RXDV  P0.7/P1.5  GPIO 7/21 */
#define IFX_RXDV                    21
/** IFX_REFCLK  P0.10/P1.6  GPIO 10/22 */
#define IFX_REFCLK                  22

 /* TXD0 : P0.0 ALT 10*/
    ifx_gpio_pin_reserve(IFX_RMIITXD0, eth_gpio_module_id);
    //ifx_gpio_dir_out_set(IFX_RMIITXD0, eth_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_RMIITXD0, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_RMIITXD0, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_RMIITXD0, eth_gpio_module_id);
 /* TXD1 : P0.4 ALT 11*/
    ifx_gpio_pin_reserve(IFX_RMIITXD1, eth_gpio_module_id);
   // ifx_gpio_dir_out_set(IFX_RMIITXD1, eth_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_RMIITXD1, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_RMIITXD1, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_RMIITXD1, eth_gpio_module_id);
 /* TXEN : P0.13 ALT 10 */
    ifx_gpio_pin_reserve(IFX_TXEN, eth_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_TXEN, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_TXEN, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_TXEN, eth_gpio_module_id);
/* MDIO : P1.8 ALT 11*/
    ifx_gpio_pin_reserve(IFX_MDIO, eth_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_MDIO, eth_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MDIO, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MDIO, eth_gpio_module_id);
/* MDC : P1.11 ALT 11*/
    ifx_gpio_pin_reserve(IFX_MDC, eth_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_MDC, eth_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_MDC, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_MDC, eth_gpio_module_id);
/* RXD0 : P0.9/P1.9 ALT 10*/
    ifx_gpio_pin_reserve(IFX_RMIIRXD0, eth_gpio_module_id);
    ifx_gpio_dir_in_set(IFX_RMIIRXD0, eth_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_RMIIRXD0, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_RMIIRXD0, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_RMIIRXD0, eth_gpio_module_id);
 /* RXD1 : P0.8/P1.7 ALT 10*/
    ifx_gpio_pin_reserve(IFX_RMIIRXD1, eth_gpio_module_id);
    ifx_gpio_dir_in_set(IFX_RMIIRXD1, eth_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_RMIIRXD1, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_RMIIRXD1, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_RMIIRXD1, eth_gpio_module_id);
 /* RXDV : P0.7/P1.5 ALT 10 */
    ifx_gpio_pin_reserve(IFX_RXDV, eth_gpio_module_id);
    ifx_gpio_dir_in_set(IFX_RXDV, eth_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_RXDV, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_RXDV, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_RXDV, eth_gpio_module_id);
/* REFCLK : P0.10/P1.6 ALT 10 */
    ifx_gpio_pin_reserve(IFX_REFCLK, eth_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_REFCLK, eth_gpio_module_id);
    ifx_gpio_altsel1_set(IFX_REFCLK, eth_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_REFCLK, eth_gpio_module_id);
}

static inline void configureMiiRxClk(int external, int loopback)
{
    if ( external )
        //  Clock is from External (From PAD)
        *IFX_CGU_IF_CLK &= ~(0x03 << 28);
    else
    {
        //Clock is from  CGU
        if ( !loopback )
            *IFX_CGU_IF_CLK |= 0x03 << 28;
        else
            *IFX_CGU_IF_CLK = (*IFX_CGU_IF_CLK | (0x01 << 28)) & ~(0x01 << 29);
    }
}


static inline void configureMiiTxClk(int external, int loopback)
{
    if ( external )
        //  Clock is from External (From PAD)
        *IFX_CGU_IF_CLK &= ~(0x03 << 26);
    else
    {
        //Clock is from  CGU
        if ( !loopback )
            *IFX_CGU_IF_CLK |= 0x03 << 26;
        else
            *IFX_CGU_IF_CLK = (*IFX_CGU_IF_CLK | (0x01 << 26)) & ~(0x01 << 27);
    }
}

static inline void configureRMiiRefClk(int external, int loopback)
{
    if ( external )
        //  Clock is from External (From PAD)
        *IFX_CGU_IF_CLK &= ~(0x03 << 24);
    else
    {
        //Clock is from  CGU
        if ( !loopback )
            *IFX_CGU_IF_CLK |= 0x03 << 24;
        else
            *IFX_CGU_IF_CLK = (*IFX_CGU_IF_CLK | (0x01 << 24)) & ~(0x01 << 25);
    }
}

static inline void configurePhyClk(int enable)
{
    if (enable)
    {
        //Clock from External
        *IFX_CGU_IF_CLK &= ~(0x01 << 4);
        *IFX_CGU_OSC_CON |= 0x18;
        dbg("External Clock\n");
    }
    else
    {
        //Clock from CGU
        *IFX_CGU_IF_CLK |= 0x01 << 4;
        *IFX_CGU_OSC_CON &= ~0x18;
        dbg("Internal Clock\n");
    }
}

static inline int __ephy_write_mdio_reg(int phy_addr, int phy_reg_num, u32 phy_data)
{
    *ETOP_MDIO_ACC = (RA_WRITE_ENABLE << 30)
                    | ((phy_addr & EPHY_ADDRESS_MASK) << EPHY_ADDRESS_SHIFT)
                    | ((phy_reg_num & EPHY_REG_MASK) << EPHY_REG_ADDRESS_SHIFT)
                    | (phy_data & EPHY_DATA_MASK);

    while ( *ETOP_MDIO_ACC & 0x80000000 );  //fix me ..

    return 0;
}

static int ephy_write_mdio_reg(int phy_addr, int phy_reg_num, u32 phy_data)
{
    return __ephy_write_mdio_reg(phy_addr, phy_reg_num, phy_data);
}

static inline void init_etop(int mii_mode)
{
    u32 etop_cfg_clear, etop_cfg_set;

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
        *IFX_CGU_OSC_CON &= ~0x18;
        *ETOP_CFG |= 0x00000002;
        dbg("REV_MII_MODE selected");
        break;
    case RED_MII_MODE:
        configureRMiiRefClk(0, 0);
        *ETOP_CFG = (*ETOP_CFG & ~0x00004000) | 0x00001000;
        *IFX_CGU_OSC_CON &= ~0x18;
        dbg("RED_MII_MODE selected");
        break;
    case EPHY_MODE:
#define EXTERNAL_EPHY_CLOCK                     0
        configurePhyClk(EXTERNAL_EPHY_CLOCK);
        *ETOP_CFG |= 0x00004000;
        ephy_write_mdio_reg(EPHY_ADDRESS, EPHY_SET_CLASS_VALUE_REG, 0xC020);
        dbg("EPHY_MODE selected");
        break;
    default:
        err("Unknown MII mode - %d", mii_mode);
    }

    //  set packet length
    *ETOP_IG_PLEN_CTRL0 = 0x004005DC;

    *ETOP_MDIO_CFG    = 0x0000;

    *ENET_MAC_CFG(0)  = 0x0807;

    *ENETS_DBA(0)     = 0x1260;
    *ENETS_CBA(0)     = 0x0DA0;
    *ENETS_CFG(0)     = (*ENETS_CFG(0) & ~0x3FF) | 0x019 | 0x3000 | 0x80000;
    *ENETS_PGCNT(0)   = 0x00020000;
    *ENETS_PKTCNT(0)  = 0x0200;

    *ENETF_DBA(0)     = 0x0C00;
    *ENETF_CBA(0)     = 0x0DC0;
    *ENETF_CFG(0)     = (*ENETF_CFG(0) & ~0xFF) | 0x0D;
    *ENETF_PGCNT(0)   = 0x00020000;
    *ENETF_PKTCNT(0)  = 0x0200;

    *DPLUS_TXCFG      = 0x000D; //default 0xf010
//    *DPLUS_TXDB       = 0x1264;
//    *DPLUS_TXCB       = 0x15D6;
//    *DPLUS_TXPGCNT    = 0x0000;
//    *DPLUS_TXCFG      = 0xF00D;
//
    *DPLUS_RXCFG      = 0x5030;  //default : 0xd030
//    *DPLUS_RXPGCNT    = 0x00040000;

    //  enable MII0
//    *ETOP_CFG = (*ETOP_CFG & ~(1 << 0)) | ((1 << 6) | (1 << 8));
}

static inline void start_etop(void)
{
    // enable MII0
    *ETOP_CFG = (*ETOP_CFG & ~(1 << 0)) | ((1 << 6) | (1 << 8));
}

static inline void init_ema(void)
{
    IFX_REG_W32((EMA_CMD_BUF_LEN << 16) | (EMA_CMD_BASE_ADDR >> 2), EMA_CMDCFG);
    IFX_REG_W32((EMA_DATA_BUF_LEN << 16) | (EMA_DATA_BASE_ADDR >> 2), EMA_DATACFG);
    IFX_REG_W32(0x000000FF, EMA_IER);
    IFX_REG_W32(EMA_READ_BURST | (EMA_WRITE_BURST << 2), EMA_CFG);
}

static inline void init_mailbox(void)
{
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU0_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU0_IER);
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU1_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU1_IER);
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU3_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU3_IER);
    IFX_REG_W32(0xFFFFFFFF, MBOX_IGU4_ISRC);
    IFX_REG_W32(0x00000000, MBOX_IGU4_IER);
}

static inline void init_atm_tc(void)
{
    IFX_REG_W32(0x0000,     DREG_AT_CTRL);
    IFX_REG_W32(0x0000,     DREG_AR_CTRL);
    IFX_REG_W32(0x0,        DREG_AT_IDLE0);
    IFX_REG_W32(0x0,        DREG_AT_IDLE1);
    IFX_REG_W32(0x0,        DREG_AR_IDLE0);
    IFX_REG_W32(0x0,        DREG_AR_IDLE1);
    IFX_REG_W32(0x40,       RFBI_CFG);
    IFX_REG_W32(0x0700,     SFSM_DBA0);
    IFX_REG_W32(0x0818,     SFSM_DBA1);
    IFX_REG_W32(0x0930,     SFSM_CBA0);
    IFX_REG_W32(0x0944,     SFSM_CBA1);
    IFX_REG_W32(0x14014,    SFSM_CFG0);
    IFX_REG_W32(0x14014,    SFSM_CFG1);
    IFX_REG_W32(0x0958,     FFSM_DBA0);
    IFX_REG_W32(0x09AC,     FFSM_DBA1);
    IFX_REG_W32(0x10006,    FFSM_CFG0);
    IFX_REG_W32(0x10006,    FFSM_CFG1);
    IFX_REG_W32(0x00000001, FFSM_IDLE_HEAD_BC0);
    IFX_REG_W32(0x00000001, FFSM_IDLE_HEAD_BC1);
}

static inline void clear_share_buffer(void)
{
    volatile u32 *p = SB_RAM0_ADDR(0);
    unsigned int i;

    for ( i = 0; i < SB_RAM0_DWLEN + SB_RAM1_DWLEN; i++ )
        IFX_REG_W32(0, p++);
}

/*
 *  Description:
 *    Download PPE firmware binary code.
 *  Input:
 *    src       --- u32 *, binary code buffer
 *    dword_len --- unsigned int, binary code length in DWORD (32-bit)
 *  Output:
 *    int       --- IFX_SUCCESS:    Success
 *                  else:           Error Code
 */
static inline int pp32_download_code(u32 *code_src, unsigned int code_dword_len, u32 *data_src, unsigned int data_dword_len)
{
    volatile u32 *dest;

    if ( code_src == 0 || ((unsigned long)code_src & 0x03) != 0
        || data_src == 0 || ((unsigned long)data_src & 0x03) != 0 )
        return IFX_ERROR;

    if ( code_dword_len <= CDM_CODE_MEMORYn_DWLEN(0) )
        IFX_REG_W32(0x00, CDM_CFG);
    else
        IFX_REG_W32(0x04, CDM_CFG);

    /*  copy code   */
    dest = CDM_CODE_MEMORY(0, 0);
    while ( code_dword_len-- > 0 )
        IFX_REG_W32(*code_src++, dest++);

    /*  copy data   */
    dest = CDM_DATA_MEMORY(0, 0);
    while ( data_dword_len-- > 0 )
        IFX_REG_W32(*data_src++, dest++);

    return IFX_SUCCESS;
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

extern void ifx_atm_get_fw_ver(unsigned int *major, unsigned int *minor)
{
    ASSERT(major != NULL, "pointer is NULL");
    ASSERT(minor != NULL, "pointer is NULL");

#ifdef VER_IN_FIRMWARE
    *major = FW_VER_ID->major;
    *minor = FW_VER_ID->minor;
#else
    *major = ATM_FW_VER_MAJOR;
    *minor = ATM_FW_VER_MINOR;
#endif
}

void ifx_atm_init_chip(void)
{
    init_pmu();

    reset_ppe();

    init_gpio(MII0_MODE_SETUP);

    init_etop(MII0_MODE_SETUP);

    init_ema();

    init_mailbox();

    init_atm_tc();

    clear_share_buffer();
}

void ifx_atm_uninit_chip(void)
{
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
    int ret;

    /*  download firmware   */
    ret = pp32_download_code(firmware_binary_code, sizeof(firmware_binary_code) / sizeof(*firmware_binary_code), firmware_binary_data, sizeof(firmware_binary_data) / sizeof(*firmware_binary_data));
    if ( ret != IFX_SUCCESS )
        return ret;

    /*  run PP32    */
    IFX_REG_W32(DBG_CTRL_RESTART, PP32_DBG_CTRL);

    /*  idle for a while to let PP32 init itself    */
    udelay(10);

    //  start etop after pp32 run
    start_etop();

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
    /*  halt PP32   */
    IFX_REG_W32(DBG_CTRL_STOP, PP32_DBG_CTRL);
}
