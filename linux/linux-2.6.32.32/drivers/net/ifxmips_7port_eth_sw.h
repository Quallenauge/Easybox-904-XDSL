/**
** FILE NAME    : ifxmips_7port_eth_sw.h
** PROJECT      : IFX UEIP
** MODULES      : ETH
** DATE         : 24 July 2009
** AUTHOR       : Reddy Mallikarjuna
** DESCRIPTION  : IFX ETH driver header file
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
** $Date            $Author         $Comment
** 24 July 2009     Reddy Mallikarjuna  Initial release
*******************************************************************************/

#ifndef _IFXMIPS_7PORT_ETH_SW_H_
#define _IFXMIPS_7PORT_ETH_SW_H_

/*!
  \file ifxmips_7port_eth_sw.h
  \ingroup IFX_ETH_DRV
  \brief Header file for VRx ETH driver internal definitions
*/

#include <asm/ifx/ifx_gpio.h>

static const int eth_gpio_module_id = IFX_GPIO_MODULE_INTERNAL_SWITCH;

/**  default board related configuration  */
#if defined(CONFIG_MII0_PORT_ENABLED)
#define CONFIG_MAC0                         1
#else
#define CONFIG_MAC0                         0
#endif

#if defined(CONFIG_MII1_PORT_ENABLED)
#define CONFIG_MAC1                         1
#else
#define CONFIG_MAC1                         0
#endif

#if defined(CONFIG_MII2_PORT_ENABLED)
#define CONFIG_MAC2                         1
#else
#define CONFIG_MAC2                         0
#endif

#if defined(CONFIG_MII3_PORT_ENABLED)
#define CONFIG_MAC3                         1
#else
#define CONFIG_MAC3                         0
#endif

#if defined(CONFIG_MII4_PORT_ENABLED)
#define CONFIG_MAC4                         1
#else
#define CONFIG_MAC4                         0
#endif

#if defined(CONFIG_MII5_PORT_ENABLED)
#define CONFIG_MAC5                         1
#else
#define CONFIG_MAC5                         0
#endif

#if defined(CONFIG_MII0_RGMII_MAC_MODE)
#define MII0_MODE_SETUP                 RGMII_MODE
#elif defined(CONFIG_MII0_RMII_MAC_MODE)
#define MII0_MODE_SETUP                 RMII_MAC_MODE
#elif defined(CONFIG_MII0_RMII_PHY_MODE)
#define MII0_MODE_SETUP                 RMII_PHY_MODE
#elif defined(CONFIG_MII0_MII_MAC_MODE)
#define MII0_MODE_SETUP                 MII_MAC_MODE
#elif defined(CONFIG_MII0_MII_PHY_MODE)
#define MII0_MODE_SETUP                 MII_PHY_MODE
#endif

#if defined(CONFIG_MII1_RGMII_MAC_MODE)
#define MII1_MODE_SETUP                 RGMII_MODE
#elif defined(CONFIG_MII1_RMII_MAC_MODE)
#define MII1_MODE_SETUP                 RMII_MAC_MODE
#elif defined(CONFIG_MII1_RMII_PHY_MODE)
#define MII1_MODE_SETUP                 RMII_PHY_MODE
#elif defined(CONFIG_MII1_MII_MAC_MODE)
#define MII1_MODE_SETUP                 MII_MAC_MODE
#elif defined(CONFIG_MII1_MII_PHY_MODE)
#define MII1_MODE_SETUP                 MII_PHY_MODE
#endif

#if defined(CONFIG_MII2_GMII_MODE)
#define MII2_MODE_SETUP                 GMII_MAC_MODE
#elif defined(CONFIG_MII2_MII_MAC_MODE)
#define MII2_MODE_SETUP                 MII_MAC_MODE
#endif

#if defined(CONFIG_MII3_MII_MAC_MODE)
#define MII3_MODE_SETUP                 MII_MAC_MODE
#endif

#if defined(CONFIG_MII4_GMII_MODE)
#define MII4_MODE_SETUP                 GMII_MAC_MODE
#elif defined(CONFIG_MII4_MII_MAC_MODE)
#define MII4_MODE_SETUP                 MII_MAC_MODE
#endif

#if defined(CONFIG_MII5_RGMII_MODE)
#define MII5_MODE_SETUP                 RGMII_MODE
#elif defined(CONFIG_MII5_MII_MAC_MODE)
#define MII5_MODE_SETUP                 MII_PHY_MODE
#endif


#ifdef CONFIG_SW_ROUTING_MODE
    #define CONFIG_PMAC_DMA_ENABLE          1   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          1   /*g_dma_pmac*/
#else
    #define CONFIG_PMAC_DMA_ENABLE          0   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          0   /*g_dma_pmac*/
#endif

#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    #define NUM_ETH_INF                     2
#else
    #define NUM_ETH_INF                     1
#endif

/*!
  \addtogroup IFX_ETH_DRV_STRUCTURE
 */

/*@{*/

/*! \typedef  cpu_ingress_pkt_header_t
* \brief Pseudo MAC(PMAC) cpu ingress packet header structure(4-bytes)

    This structure is used internal purpose
*/
typedef struct
{
    u32 resv_7_3            :5; /*!< Reserved bits */
    u32 SPID                :3; /*!< Source Logical Port ID */
    u32 CRCGEN_DIS          :1; /*!< CRC Generation Disable */
    u32 resv_14_11          :4;  /*!< Reserved bits */
    u32 DPID                :3; /*!< Destination Group and Port ID */
    u32 Port_Map_EN         :1; /*!< Destination Ethernet Port Map Enable */
    u32 Port_Map_Sel        :1; /*!< Destination Ethernet Port Map or Mask */
    u32 LRN_DIS             :1; /*!< Source MAC Address Learning Disable */
    u32 CLASS_EN            :1; /*!< Destination Class Enable */
    u32 Ig_Tr_CLASS         :4; /*!< Target Traffic Class */
    u32 resv_31             :1; /*!< Reserved bit*/
    u32 Dest_Port_Map       :6; /*!< Destination Ethernet Port Map */
    u32 DPID_EN             :1; /*!< Destination Group ID Enable */
} cpu_ingress_pkt_header_t;

/*! \typedef  cpu_egress_pkt_header_t
* \brief Pseudo MAC(PMAC) cpu egress packet header structure (8-bytes)

    This structure is used internal purpose
*/
typedef struct
{
    u32 b0_resv_7_6         :2;  /*!< Reserved bits*/
    u32 IPOFF               :6;  /*!< IP Offset */
    u32 PORTMAP             :8; /*!< Destination Group and Port Map */
    u32 SLPID               :3; /*!< Source Logical Port ID */
    u32 b2_reserved_4_3     :2; /*!< Reserved bits*/
    u32 IS_TAG              :1; /*!< Is Tagged */
    u32 b2_reserved_1_0     :2; /*!< Reserved bits*/
    u32 b3_reserved_7_4     :4; /*!< Reserved bits*/
    u32 PPPoES              :1; /*!< PPPoE Session Packet */
    u32 IPv6                :1; /*!< IPv6 Packet */
    u32 IPv4                :1; /*!< IPv4 Packet */
    u32 MRR                 :1; /*!< Mirrored */
    u32 b4_resv_7_6         :2; /*!< Reserved bits*/
    u32 PKT_LEN_HI          :6; /* Packet Length High Bits */
    u32 PKT_LEN_LO          :8; /* Packet Length Low Bits */
    u32 b6_resv_7_0         :8; /*!< Reserved bits*/
    u32 b7_resv_7           :1; /*!< Reserved bits*/
    u32 SPPID               :3; /*!< Source Physical Port ID */
    u32 Eg_Tr_CLASS         :4; /*!< Traffic Class */
} cpu_egress_pkt_header_t;
/* @} */

void inline ifx_mdelay( int delay){
	int i;
	for ( i=delay; i > 0; i--)
		udelay(1000);
}

void vr9_switch_write_mdio(unsigned int phyAddr, unsigned int regAddr,unsigned int data )
{
    unsigned int reg;
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }
    reg = MDIO_READ_WDATA(data);
    SW_WRITE_REG32( reg, MDIO_WRITE_REG);
    reg = ( MDIO_CTRL_OP_WR | MDIO_CTRL_PHYAD_SET(phyAddr) | MDIO_CTRL_REGAD(regAddr) );
    reg |= MDIO_CTRL_MBUSY;
    SW_WRITE_REG32( reg, MDIO_CTRL_REG);
    udelay(100);
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
        udelay(10);
    }
    return ;
}

unsigned short vr9_switch_read_mdio(unsigned int phyAddr, unsigned int regAddr )
{
    unsigned int reg;
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }

    reg = ( MDIO_CTRL_OP_RD | MDIO_CTRL_PHYAD_SET(phyAddr) | MDIO_CTRL_REGAD(regAddr) );
    reg |= MDIO_CTRL_MBUSY;

    SW_WRITE_REG32( reg, MDIO_CTRL_REG);
    reg = SW_READ_REG32(MDIO_CTRL_REG);
    while (reg & MDIO_CTRL_MBUSY ) {
        reg = SW_READ_REG32(MDIO_CTRL_REG);
    }

    reg = SW_READ_REG32(MDIO_READ_REG);
    return (MDIO_READ_RDATA(reg));
}


void port_init(int port, int mode)
{
    unsigned int cfg_reg, phy_reg, mac_ctrl_0;

    if (port < 0 || port > 5 ) {
        printk("%s[%d]: Err!!! (invalid port number: %d)\n",__func__,__LINE__,port);
        return ;
    }


    cfg_reg = SW_READ_REG32(MII_CFG_0_REG + (port * 8));
    cfg_reg |= MII_CFG_RES; /* reset the port */
    SW_WRITE_REG32( cfg_reg, (MII_CFG_0_REG+(port*8)) );
    udelay(100);

    switch (port) {
        case 0:
            cfg_reg = SW_READ_REG32(MII_CFG_0_REG);
            phy_reg = SW_READ_REG32(PHY_ADDR_0);
            mac_ctrl_0 = SW_READ_REG32(MAC_0_CTRL_0);
            mac_ctrl_0 &= ~(MAC_CTRL_0_FCON_MASK | MAC_CTRL_0_FDUP_MASK | MAC_CTRL_0_GMII_MASK);
            phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
            cfg_reg &= ~(MII_CFG_MIIRATE_MASK | MII_CFG_MIIMODE_MASK );
            if (mode == RGMII_MODE ) {
                cfg_reg |= ( MII_CFG_MIIMODE_RGMII | MII_CFG_MIIRATE_125MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_1000;
            } else if (mode == RGMII_MODE_100MB) {
                cfg_reg |= ( MII_CFG_MIIMODE_RGMII | MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else if (mode == RED_MII_MAC_MODE) {
                cfg_reg |= ( MII_CFG_MIIMODE_RMIIM | MII_CFG_MIIRATE_50MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else if (mode == MII_MAC_MODE) {
                cfg_reg |= ( MII_CFG_MIIMODE_MIIM | MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else {
                printk("%s[%d]: Err!!! (Unknow interface mode)\n",__func__,__LINE__);
            }
            mac_ctrl_0 |= (MAC_CTRL_0_FDUP_EN | MAC_CTRL_0_FCON_RXTX );
            phy_reg |= PHY_ADDR_LINKST_UP;
            SW_WRITE_REG32( cfg_reg, MII_CFG_0_REG );
            SW_WRITE_REG32( mac_ctrl_0, MAC_0_CTRL_0 );
            SW_WRITE_REG32( phy_reg, PHY_ADDR_0 );
            break;
        case 1:
            cfg_reg = SW_READ_REG32(MII_CFG_1_REG);
            phy_reg = SW_READ_REG32(PHY_ADDR_1);
            mac_ctrl_0 = SW_READ_REG32(MAC_1_CTRL_0);
            mac_ctrl_0 &= ~(MAC_CTRL_0_FCON_MASK | MAC_CTRL_0_FDUP_MASK | MAC_CTRL_0_GMII_MASK);
            phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
            cfg_reg &= ~(MII_CFG_MIIRATE_MASK | MII_CFG_MIIMODE_MASK );
            if (mode == RGMII_MODE ) {
                cfg_reg |= ( MII_CFG_MIIMODE_RGMII | MII_CFG_MIIRATE_125MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_1000;
            } else if (mode == RGMII_MODE_100MB) {
                cfg_reg |= ( MII_CFG_MIIMODE_RGMII | MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else if (mode == RED_MII_MAC_MODE) {
                cfg_reg |= ( MII_CFG_MIIMODE_RMIIM | MII_CFG_MIIRATE_50MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else if (mode == MII_MAC_MODE) {
                cfg_reg |= ( MII_CFG_MIIMODE_MIIM | MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else {
                printk("%s[%d]: Err!!! (Unknow interface mode)\n",__func__,__LINE__);
            }
            mac_ctrl_0 |= (MAC_CTRL_0_FDUP_EN | MAC_CTRL_0_FCON_RXTX );
            phy_reg |= PHY_ADDR_LINKST_UP;
            SW_WRITE_REG32( cfg_reg, MII_CFG_1_REG );
            SW_WRITE_REG32( mac_ctrl_0, MAC_1_CTRL_0 );
            SW_WRITE_REG32( phy_reg, PHY_ADDR_1 );
            break;
        case 2:
            cfg_reg = SW_READ_REG32(MII_CFG_2_REG);
            phy_reg = SW_READ_REG32(PHY_ADDR_2);
            mac_ctrl_0 = SW_READ_REG32(MAC_2_CTRL_0);
            mac_ctrl_0 &= ~(MAC_CTRL_0_FCON_MASK | MAC_CTRL_0_FDUP_MASK | MAC_CTRL_0_GMII_MASK);
            phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
            cfg_reg &= ~(MII_CFG_MIIRATE_MASK | MII_CFG_MIIMODE_MASK );
            if (mode == GMII_MAC_MODE ) {
                cfg_reg |= ( MII_CFG_MIIRATE_125MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_1000;
            } else if (mode == MII_MAC_MODE) {
                cfg_reg |= (  MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else {
                printk("%s[%d]: Err!!! (Unknow interface mode)\n",__func__,__LINE__);
            }
            mac_ctrl_0 |= (MAC_CTRL_0_FDUP_EN | MAC_CTRL_0_FCON_RXTX );
            phy_reg |= PHY_ADDR_LINKST_UP;
            phy_reg |=  PHY_ADDR_FDUP_EN;
            SW_WRITE_REG32( cfg_reg, MII_CFG_2_REG );
            SW_WRITE_REG32( mac_ctrl_0, MAC_2_CTRL_0 );
            SW_WRITE_REG32( phy_reg, PHY_ADDR_2 );
            break;
        case 3:
            cfg_reg = SW_READ_REG32(MII_CFG_3_REG);
            phy_reg = SW_READ_REG32(PHY_ADDR_3);
            mac_ctrl_0 = SW_READ_REG32(MAC_3_CTRL_0);
            mac_ctrl_0 &= ~(MAC_CTRL_0_FCON_MASK | MAC_CTRL_0_FDUP_MASK | MAC_CTRL_0_GMII_MASK);
            phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
            cfg_reg &= ~(MII_CFG_MIIRATE_MASK | MII_CFG_MIIMODE_MASK );
            if (mode == MII_MAC_MODE) {
                cfg_reg |= (  MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else {
                printk("%s[%d]: Err!!! (Unknow interface mode)\n",__func__,__LINE__);
            }
            mac_ctrl_0 |= (MAC_CTRL_0_FDUP_EN | MAC_CTRL_0_FCON_RXTX );
            phy_reg |= PHY_ADDR_LINKST_UP;
            phy_reg |=  PHY_ADDR_FDUP_EN;
            SW_WRITE_REG32( cfg_reg, MII_CFG_3_REG );
            SW_WRITE_REG32( mac_ctrl_0, MAC_3_CTRL_0 );
            SW_WRITE_REG32( phy_reg, PHY_ADDR_3 );
            break;
        case 4:
            cfg_reg = SW_READ_REG32(MII_CFG_4_REG);
            phy_reg = SW_READ_REG32(PHY_ADDR_4);
            mac_ctrl_0 = SW_READ_REG32(MAC_4_CTRL_0);
            mac_ctrl_0 &= ~(MAC_CTRL_0_FCON_MASK | MAC_CTRL_0_FDUP_MASK | MAC_CTRL_0_GMII_MASK);
            phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
            cfg_reg &= ~(MII_CFG_MIIRATE_MASK | MII_CFG_MIIMODE_MASK );
            if (mode == GMII_MAC_MODE ) {
                cfg_reg |= ( MII_CFG_MIIRATE_125MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_1000;
            } else if (mode == MII_MAC_MODE) {
                cfg_reg |= (  MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else {
               printk("%s[%d]: Err!!! (Unknow interface mode)\n",__func__,__LINE__);
            }
            mac_ctrl_0 |= (MAC_CTRL_0_FDUP_EN | MAC_CTRL_0_FCON_RXTX );
            phy_reg |= PHY_ADDR_LINKST_UP;
            phy_reg |=  PHY_ADDR_FDUP_EN;
            SW_WRITE_REG32( cfg_reg, MII_CFG_4_REG );
            SW_WRITE_REG32( mac_ctrl_0, MAC_4_CTRL_0 );
            SW_WRITE_REG32( phy_reg, PHY_ADDR_4 );
            break;
        case 5:
            cfg_reg = SW_READ_REG32(MII_CFG_5_REG);
            phy_reg = SW_READ_REG32(PHY_ADDR_5);
            mac_ctrl_0 = SW_READ_REG32(MAC_5_CTRL_0);
            mac_ctrl_0 &= ~(MAC_CTRL_0_FCON_MASK | MAC_CTRL_0_FDUP_MASK | MAC_CTRL_0_GMII_MASK);
            phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
            cfg_reg &= ~(MII_CFG_MIIRATE_MASK | MII_CFG_MIIMODE_MASK );
            if (mode == RGMII_MODE ) {
                cfg_reg |= ( MII_CFG_MIIMODE_RGMII | MII_CFG_MIIRATE_125MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_1000;
            } else if (mode == RGMII_MODE_100MB) {
                cfg_reg |= ( MII_CFG_MIIMODE_RGMII | MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_RGMII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else if (mode == RED_MII_MAC_MODE) {
                cfg_reg |= ( MII_CFG_MIIMODE_RMIIM | MII_CFG_MIIRATE_50MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else if (mode == MII_MAC_MODE) {
                cfg_reg |= ( MII_CFG_MIIMODE_MIIM | MII_CFG_MIIRATE_25MHZ | MII_CFG_EN);
                mac_ctrl_0 |= MAC_CTRL_0_GMII_MII;
                phy_reg |= PHY_ADDR_SPEED_100;
            } else {
                printk("%s[%d]: Err!!! (Unknow interface mode)\n",__func__,__LINE__);
            }
            mac_ctrl_0 |= (MAC_CTRL_0_FDUP_EN | MAC_CTRL_0_FCON_RXTX );
            phy_reg |= PHY_ADDR_LINKST_UP;
            SW_WRITE_REG32( cfg_reg, MII_CFG_5_REG );
            SW_WRITE_REG32( mac_ctrl_0, MAC_5_CTRL_0 );
            SW_WRITE_REG32( phy_reg, PHY_ADDR_5 );
            break;
    }
}

int vr9_7port_sw_hw_init(void)
{
    unsigned int reg;
//    SWITCH_PMU_SETUP(IFX_PMU_ENABLE);  /*BUG in RTL silver0.0 */
    PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_DPLUSS_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_DPLUSM_PMU_SETUP (IFX_PMU_ENABLE);
    SWITCH_PMU_SETUP(IFX_PMU_ENABLE);

    /**Enable Switch  */
    SW_WRITE_REG32 ( (SW_READ_REG32(GLOB_CTRL_REG) | GLOB_CTRL_SE), GLOB_CTRL_REG) ;

    /** Disable MDIO auto polling mode for all ports */
    /*SW_WRITE_REG32 ( 0x0, MDC_CFG_0_REG) ; */
#if defined( CONFIG_MAC0) && CONFIG_MAC0
    port_init(0, MII0_MODE_SETUP);
#endif /* CONFIG_MAC0*/
#if defined( CONFIG_MAC1) && CONFIG_MAC1
    port_init(1, MII1_MODE_SETUP);
#endif /* CONFIG_MAC1*/
#if defined( CONFIG_MAC2) && CONFIG_MAC2
    port_init(2, MII2_MODE_SETUP);
#endif /* CONFIG_MAC2*/
#if defined( CONFIG_MAC3) && CONFIG_MAC3
    port_init(3, MII3_MODE_SETUP);
#endif /* CONFIG_MAC3*/
#if defined( CONFIG_MAC4) && CONFIG_MAC4
    port_init(4, MII4_MODE_SETUP);
#endif /* CONFIG_MAC4*/
#if defined( CONFIG_MAC5) && CONFIG_MAC5
    port_init(5, MII5_MODE_SETUP);
#endif /* CONFIG_MAC5*/
#if 1
    {
	int i;
	for ( i =0; i < 7; i++) {
                /*       enable counters  */
                SW_WRITE_REG32(0x1, (ETHSW_BM_PCFG_REG + (i*8)));
        }
    }
#endif
    /* Replace default IPG value from 0x85 to ox8B */
    SW_WRITE_REG32 (0x8B, PMAC_RX_IPG_REG) ;

    reg = SW_READ_REG32(PMAC_HD_CTL_REG );
    reg |= PMAC_HD_CTL_AC;
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    SW_WRITE_REG32 ( (SW_READ_REG32(FDMA_PCTRL_PORT6) | FDMA_PCTRL_STEN), FDMA_PCTRL_PORT6) ;
    reg |= PMAC_HD_CTL_AS;
#else
    reg &= ~PMAC_HD_CTL_AS;
#endif
#if defined(CONFIG_DMA_PMAC_ENABLE) && CONFIG_DMA_PMAC_ENABLE
    reg |= PMAC_HD_CTL_RXSH;
#else
    reg &= ~PMAC_HD_CTL_RXSH;
#endif
    SW_WRITE_REG32( reg, PMAC_HD_CTL_REG);

   for ( reg = 0; reg < 7; reg++ ) {
        SW_WRITE_REG32((SW_READ_REG32(FDMA_PCTRL_REG(reg)) | 0x01), FDMA_PCTRL_REG(reg));
        SW_WRITE_REG32((SW_READ_REG32(SDMA_PCTRL_REG(reg)) | 0x01), SDMA_PCTRL_REG(reg));
    }
    return 0;
}

#endif /*_IFXMIPS_7PORT_ETH_SW_H_  */
