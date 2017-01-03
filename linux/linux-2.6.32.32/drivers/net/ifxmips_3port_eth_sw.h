/**
** FILE NAME    : ifxmips_3port_eth_sw.h
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

#ifndef _IFXMIPS_3PORT_SW_H_
#define _IFXMIPS_3PORT_SW_H_
/*!
  \file ifxmips_3port_eth_sw.h
  \ingroup IFX_ETH_DRV
  \brief Header file for ARx ETH driver internal definitions
*/

#include <asm/ifx/ifx_gpio.h>

static const int eth_gpio_module_id = IFX_GPIO_MODULE_INTERNAL_SWITCH;
extern int eth1_up;
/**  default board related configuration  */
/* MII0 mode */
#define MII0_MODE_SETUP                         RGMII_MODE

#if defined(CONFIG_MII1_RGMII_MODE_100MB)
  #define MII1_MODE_SETUP                       RGMII_MODE_100MB
#elif defined(CONFIG_MII1_RGMII_MODE)
  #define MII1_MODE_SETUP                       RGMII_MODE
#elif defined(CONFIG_MII1_AUTONEG)
  #define MII1_MODE_SETUP                       RGMII_MODE_100MB  //  default setting before auto-negotiate
  #define MII1_GPHY_INT_EXT_INT_NO              5

  #define GPHY_VSC								0
  #define GPHY_LANTIQ_11G						1
  #if MII1_GPHY_INT_EXT_INT_NO == 0
    #define MII1_GPHY_INT                       INT_NUM_IM4_IRL30
    #define GPIO_EXT_INT                        0
  #elif MII1_GPHY_INT_EXT_INT_NO == 1
    #define MII1_GPHY_INT                       INT_NUM_IM3_IRL31
    #define GPIO_EXT_INT                        1
  #elif MII1_GPHY_INT_EXT_INT_NO == 2
    #define MII1_GPHY_INT                       INT_NUM_IM1_IRL26
    #define GPIO_EXT_INT                        2
  #elif MII1_GPHY_INT_EXT_INT_NO == 3
    #define MII1_GPHY_INT                       INT_NUM_IM1_IRL0
    #define GPIO_EXT_INT                        39
  #elif MII1_GPHY_INT_EXT_INT_NO == 4
    #define MII1_GPHY_INT                       INT_NUM_IM1_IRL1
    #define GPIO_EXT_INT                        10
  #elif MII1_GPHY_INT_EXT_INT_NO == 5
    #define MII1_GPHY_INT                       INT_NUM_IM1_IRL2
    #define GPIO_EXT_INT                        9
  #endif
#else
  #error you must specify MII1 link state in current stage
#endif

#ifdef CONFIG_SW_ROUTING_MODE
    #define CONFIG_PMAC_DMA_ENABLE          1   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          1   /*g_dma_pmac*/
    #define INGRESS_DIRECT_FORWARD_ENABLE   1
#else
    #define CONFIG_PMAC_DMA_ENABLE          0   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          0   /*g_dma_pmac*/
    #define INGRESS_DIRECT_FORWARD_ENABLE   0
#endif

#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    #define NUM_ETH_INF                     2
#else
    #define NUM_ETH_INF                     1
#endif
#define CONFIG_IFX_MAC1                     1
#define ENABLE_SWITCH_FLOW_CONTROL          1
/* select 25MHz clock based on design board*/
#define SELECT_25MHZ_CLOCK_FROM_GPIO        0
/** GPIO 42 (P2.10) */
#define GPIO_MDIO                           42
/** GPIO 43 (P2.11) */
#define GPIO_MDC                            43
 /*GPIO 3 (P0.3) */
#define GPIO_CLK_OUT2                       3
/* GPIO 32 (P2.0) */
#define GPIO_RESET_GPHY                     32
/** HW related configuration */
#define PHY0_ADDR                           0x00
#define PHY1_ADDR                           0x11
#define TANTOS_CHIP_ID                      0x2599

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
    u32	res31_27                :5; /*!< reserved bits */
    u32 	SPID                :3; /*!< Source port ID*/
    u32	res23_18                :6; /*!< reserved bits */
    u32     DPID                :2; /*!< Destination Physical Port ID valid only when DIRECT is set*/
    u32	res15_10                :6; /*!< reserved bits */
    u32	QID                     :2; /*!< Queue ID of destination Physical port, valid only when Direct is set*/
    u32	res7_1                  :7; /*!< reserved bits */
    u32      DIRECT             :1; /*!< Direct Forwarding Enable*/
}cpu_ingress_pkt_header_t;

/*! \typedef  cpu_egress_pkt_header_t
* \brief Pseudo MAC(PMAC) cpu egress packet header structure (8-bytes)

    This structure is used internal purpose
*/
typedef struct
{
    u32 ipOffset                :8; /*!< IP offset*/
    u32 destPortMap             :8; /*!< destination port map*/
    u32 srcPortID               :3; /*!< Source port id*/
    u32 inputTag                :1; /*!< input tag flag, packet is received with VALN tag or not*/
    u32 managementPacket        :1; /*!< management flag, packet is classifield as managemnt packet*/
    u32 outputTag               :1; /*!< packet is transmitted with VLAN tag to CPU */
    u32 queueID                 :2; /*!< Queue ID*/
    u32 crcGenerated            :1; /*!< packet include CRC*/
    u32 crcErr                  :1; /*!< crc error packet*/
    u32 arpRarpPacket           :1; /*!< ARP/RARP packet*/
    u32 PPPoEPacket             :1; /*!< PPPoE packet*/
    u32 iPv6Packet              :1; /*!< IPv6 packet*/
    u32 res                     :1; /*!< reserved */
    u32 iPv4Packet              :1; /*!< IPv4 packet*/
    u32 mirrored                :1; /*!< Packet was mirrored*/
    u32 res_31_16               :16;    /*!< reserved */
    u32 PKTLEN_8                :8; /*!< packet length Lower 8 bits*/
    u32 res_7_3                 :5; /*!< reserved bits 7~3*/
    u32 PKTLEN_3                :3; /*!< packet length higher 3 bits*/
}cpu_egress_pkt_header_t;
/* @} */

void inline ifx_mdelay( int delay){
	int i;
	for ( i=delay; i > 0; i--)
		udelay(1000);
}
#if defined(SELECT_25MHZ_CLOCK_FROM_GPIO) && SELECT_25MHZ_CLOCK_FROM_GPIO
/** Select the 25MHz clock for GPHY using the GPIO */
static void select_25MhzClock(int gpio,int en)
{
    unsigned int reg;
    reg = IFX_REG_R32(IFX_CGU_IF_CLK);
     /*Select Clock_Out2 @25MHz */
    reg &= ~(3<<10);
    IFX_REG_W32( reg, IFX_CGU_IF_CLK);
    if ( en ) {
        ifx_gpio_pin_reserve(gpio, eth_gpio_module_id);
        ifx_gpio_dir_out_set(gpio, eth_gpio_module_id);
        ifx_gpio_altsel0_set(gpio, eth_gpio_module_id);
        ifx_gpio_altsel1_clear(gpio, eth_gpio_module_id);
        ifx_gpio_open_drain_set(gpio, eth_gpio_module_id);
    } else {
        ifx_gpio_pin_free(gpio, eth_gpio_module_id);
    }
}
#endif /*SELECT_25MHZ_CLOCK_FROM_GPIO */
/*Reset of the Extern GPHY */
static void reset_ext_gphy(int gpio, int en)
{
    if (en) {
        ifx_gpio_pin_reserve(gpio, eth_gpio_module_id);
        ifx_gpio_altsel0_clear(gpio, eth_gpio_module_id);
        ifx_gpio_altsel1_clear(gpio, eth_gpio_module_id);
        ifx_gpio_dir_out_set(gpio, eth_gpio_module_id);
        ifx_gpio_open_drain_set(gpio, eth_gpio_module_id);
        ifx_gpio_output_set(gpio, eth_gpio_module_id);
        ifx_mdelay(100);
        ifx_gpio_output_clear(gpio, eth_gpio_module_id);
        ifx_mdelay(100);
        ifx_gpio_output_set(gpio, eth_gpio_module_id);
        ifx_mdelay(100);
    } else {
        ifx_gpio_pin_free(gpio, eth_gpio_module_id);
    }
}
/**Reserve & setting of GPIO pins for MDIO/MDC */
static void reserve_gpiopins_for_mdio(int en)
{
    if (en ) {
        ifx_gpio_pin_reserve(GPIO_MDIO, eth_gpio_module_id);
        ifx_gpio_dir_out_set(GPIO_MDIO, eth_gpio_module_id);
        ifx_gpio_altsel0_set(GPIO_MDIO, eth_gpio_module_id);
        ifx_gpio_altsel1_clear(GPIO_MDIO, eth_gpio_module_id);
        ifx_gpio_open_drain_set(GPIO_MDIO, eth_gpio_module_id);
        
        ifx_gpio_pin_reserve(GPIO_MDC, eth_gpio_module_id);
        ifx_gpio_dir_out_set(GPIO_MDC, eth_gpio_module_id);
        ifx_gpio_altsel0_set(GPIO_MDC, eth_gpio_module_id);
        ifx_gpio_altsel1_clear(GPIO_MDC, eth_gpio_module_id);
        ifx_gpio_open_drain_set(GPIO_MDC, eth_gpio_module_id);
    } else {
        ifx_gpio_pin_free(GPIO_MDIO, eth_gpio_module_id);
        ifx_gpio_pin_free(GPIO_MDC, eth_gpio_module_id);
    }
}

#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG  
static void config_gphy_gpio(int en)
{
    if ( en ) {
        ifx_gpio_pin_reserve(GPIO_EXT_INT, eth_gpio_module_id);
#if ( (GPIO_EXT_INT == 0) || (GPIO_EXT_INT == 1) )
        ifx_gpio_altsel0_set(GPIO_EXT_INT, eth_gpio_module_id);
        ifx_gpio_altsel1_clear(GPIO_EXT_INT, eth_gpio_module_id);
#elif ((GPIO_EXT_INT == 2) || (GPIO_EXT_INT == 39) )
        ifx_gpio_altsel0_clear(GPIO_EXT_INT, eth_gpio_module_id);
        ifx_gpio_altsel1_set(GPIO_EXT_INT, eth_gpio_module_id);
#elif ((GPIO_EXT_INT == 10) || (GPIO_EXT_INT == 9) )
        ifx_gpio_altsel0_set(GPIO_EXT_INT, eth_gpio_module_id);
        ifx_gpio_altsel1_set(GPIO_EXT_INT, eth_gpio_module_id);
#else
    #error ERROR EXT INT NO
#endif
        ifx_gpio_dir_in_set(GPIO_EXT_INT, eth_gpio_module_id);
    } else {
        ifx_gpio_pin_free(GPIO_EXT_INT, eth_gpio_module_id);
    }
}
#endif /** CONFIG_MII1_AUTONEG */

static u16 ar9_smi_reg_read(u16 reg_addr)
{
    unsigned int reg;
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
    reg = MDIO_CTL_OP_READ | ( reg_addr & 0x3FF) | MDIO_CTL_MBUSY;
    IFX_REG_W32( reg, IFX_AR9_ETH_MDIO_CTL);
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
    return(IFX_REG_R32(IFX_AR9_ETH_MDIO_DATA) & MDIO_CTL_RD_MASK);
}

static void ar9_smi_reg_write(u16 reg_addr, u16 data)
{
    
    unsigned int reg;
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
    reg = MDIO_CTL_WD(data) | MDIO_CTL_OP_WRITE | (reg_addr & 0x3FF) | MDIO_CTL_MBUSY;
    IFX_REG_W32( reg, IFX_AR9_ETH_MDIO_CTL);
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
    return ;
}
#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG 
static void ar9_switch_write_mdio(unsigned int uPhyAddr, unsigned int uRegAddr,unsigned int data )
{
    unsigned int reg;
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
  
    reg = (MDIO_CTL_WD(data) | MDIO_CTL_OP_WRITE        \
        | MDIO_CTL_MBUSY | MDIO_CTL_PHYAD_SET(uPhyAddr) \
        | MDIO_CTL_REGAD_SET(uRegAddr));
    IFX_REG_W32( reg, IFX_AR9_ETH_MDIO_CTL);
    udelay(100);
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
        udelay(10);
    }
    return ; 
}
#endif /** CONFIG_MII1_AUTONEG */

unsigned short ar9_switch_read_mdio(unsigned int uPhyAddr, unsigned int uRegAddr )
{
    unsigned int reg;
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
    reg = MDIO_CTL_OP_READ | MDIO_CTL_MBUSY \
        | MDIO_CTL_PHYAD_SET(uPhyAddr) | MDIO_CTL_REGAD_SET(uRegAddr);
    IFX_REG_W32( reg, IFX_AR9_ETH_MDIO_CTL);
    reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    while (reg & MDIO_CTL_MBUSY ) {
        reg = IFX_REG_R32(IFX_AR9_ETH_MDIO_CTL);
    }
    return(IFX_REG_R32(IFX_AR9_ETH_MDIO_DATA) & MDIO_CTL_RD_MASK);
}

/* Extern tantos switch init */
static void tantos_switch_init(void)
{
    u16 reg;
    reserve_gpiopins_for_mdio(1);
    reg = (unsigned short)(ar9_smi_reg_read(0x101));
    printk(" External Switch ID :0x%08x\n",reg);
    if (reg == TANTOS_CHIP_ID) {
        ar9_smi_reg_write(0xa1,0x0004); /*PORT 5 */
        udelay(500);
        reg = (unsigned short)(ar9_smi_reg_read(0xa1));
        if(reg != 0x4) 
            printk("%s[%d]   Error!!!!\n", __FUNCTION__,__LINE__);
        ar9_smi_reg_write(0xc1,0x0004); /*PORT 6*/
        udelay(500);
        reg = (unsigned short)(ar9_smi_reg_read(0xc1));
        if(reg != 0x4) 
            printk("%s[%d]   Error!!!!\n", __FUNCTION__,__LINE__);
            
#if   defined( ENABLE_SWITCH_FLOW_CONTROL ) && ENABLE_SWITCH_FLOW_CONTROL
        ar9_smi_reg_write(0xf5,0x0bbb); /*Enable flow control */
        udelay(500);
        reg = (unsigned short)(ar9_smi_reg_read(0xf5));
        if(reg != 0x0bbb) 
            printk("%s[%d]   Error!!!!\n", __FUNCTION__,__LINE__);
#else
        ar9_smi_reg_write(0xf5,0x0aaa); /*Disable flow control */
        udelay(500);
        reg = (unsigned short)(ar9_smi_reg_read(0xf5));
        if(reg != 0x0aaa) 
            printk("%s[%d]   Error!!!!\n", __FUNCTION__,__LINE__);
#endif
/*Set Bypass mode */
        /*P0 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0x03));
        ar9_smi_reg_write(0x03, (reg| 0x80));
        /*P1 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0x23));
        ar9_smi_reg_write(0x23, (reg| 0x80));
        /*P2 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0x43));
        ar9_smi_reg_write(0x43, (reg| 0x80));
        /*P3 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0x63));
        ar9_smi_reg_write(0x63, (reg| 0x80));
        /*P4 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0x83));
        ar9_smi_reg_write(0x83, (reg| 0x80));
        /*P5 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0xa3));
        ar9_smi_reg_write(0xa3, (reg| 0x80));
        /*P6 portbase vlan map reg */
        reg = (unsigned short)(ar9_smi_reg_read(0xc3));
        ar9_smi_reg_write(0xc3, (reg| 0x80));
        
    }
    return ;
}
/** 3 port switch HW init */
void switch_hw_init(int mode, int port)
{
    unsigned int reg;
    /* Initilize the externl Tanto switch */
    if ( port == 0)
        tantos_switch_init();
        
    reg = IFX_REG_R32(IFX_RCU_PPE_CONF);
    if ( port == 0) {
        reg &= 0xFFFF81FF;
        /*DLL bypass for SWITCH Rx port 1.75ns delay */
         reg |= (0x5 << 9);
    }
    else if ( port == 1) {
        reg &= 0xFFE07FFF;
         /* Tx-->1.75ns, Rx-->1.75ns */
        reg |= (0x20 << 15);
    }
    IFX_REG_W32( reg, IFX_RCU_PPE_CONF);  
    
    reg = IFX_REG_R32(IFX_AR9_ETH_RGMII_CTL);
    if ( port == 0) {
        reg &= ~(RGMII_CTL_P0TDLY_MASK | RGMII_CTL_P0RDLY_MASK |RGMII_CTL_P0IS_MASK \
                | RGMII_CTL_P0FRQ |RGMII_CTL_P0CKIO );
        /*Set RGMI0 Rx clock delay DLL 1.75nSec*/
        reg |= RGMII_CTL_P0RDLY_1_75;
        if ( mode == RGMII_MODE) {
        reg |= (RGMII_CTL_P0IS_RGMII | RGMII_CTL_P0SPD_1000);
        } else if (RGMII_MODE_100MB ) {
            reg |= (RGMII_CTL_P0IS_RGMII | RGMII_CTL_P0SPD_100);
        }
    }
    else if (port == 1) {
        reg &= ~(RGMII_CTL_P1TDLY_MASK | RGMII_CTL_P1RDLY_MASK |RGMII_CTL_P1IS_MASK \
                   | RGMII_CTL_P1FRQ |RGMII_CTL_P1CKIO  );
        if ( mode == RGMII_MODE) {
        reg |= (RGMII_CTL_P1IS_RGMII | RGMII_CTL_P1SPD_1000);
        } else if (RGMII_MODE_100MB ) {
            reg |= (RGMII_CTL_P1IS_RGMII | RGMII_CTL_P1SPD_100);
        }
    }
   
#if   defined( ENABLE_SWITCH_FLOW_CONTROL ) && ENABLE_SWITCH_FLOW_CONTROL
    reg |= RGMII_CTL_FCE_SET(port,1);
#else
    reg |= RGMII_CTL_FCE_SET(port,0);
#endif
    IFX_REG_W32( reg, IFX_AR9_ETH_RGMII_CTL);
    
    if (port)
        reg = IFX_REG_R32(IFX_AR9_ETH_P1_CTL );
    else
        reg = IFX_REG_R32(IFX_AR9_ETH_P0_CTL );
#if defined(INGRESS_DIRECT_FORWARD_ENABLE) && INGRESS_DIRECT_FORWARD_ENABLE
    reg |= PX_CTL_DFWD;
#else
    reg &= ~PX_CTL_DFWD;
#endif
    reg |= PX_CTL_BYPASS;
    /* Link is up during the interface up */
    /*reg |= PX_CTL_FLP; */
    reg |= (PX_CTL_IPVLAN | PX_CTL_SPE | PX_CTL_VPE);
    if (port)
        IFX_REG_W32( reg, IFX_AR9_ETH_P1_CTL);
    else
       IFX_REG_W32( reg, IFX_AR9_ETH_P0_CTL);

}

#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG
/* mii1 gphy status structure */
struct mii1_gphy_status {
    int link;  /*link status, up/down */
    int speed;  /* preset link speed, 10/100/1000*/
    int duplex;  /*duplex status, FD/HD*/
};
static struct mii1_gphy_status gphy_status;

/*Used for Interrupt Handler */
static irqreturn_t mii1_gphy_VSC_int_handler (int irq, void *dev_id)
{
    unsigned short  status, linkReg, speedReg;
    unsigned int reg_rgmii, reg_port1;
    struct mii1_gphy_status *phy_status  = (struct mii1_gphy_status *)dev_id;
    /*read data from phy register to clear phy interrupts */
    /*printk("%s: Got interrupt\n",__func__);*/
    
    status=ar9_switch_read_mdio(0x11,0x1a); 
    /*interrupt status bit */
    if(status & 0x8000) { 
        /*cause the interrupt due to link status, speed & FDX */
        if( (status & 0x7000)) {
            linkReg=ar9_switch_read_mdio(0x11,0x01);
            /*Link on PHY is down */
            reg_port1 = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
            if((linkReg&0x04)==0) {
               
                reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                /*Port1 force link down */
                reg_port1 |= PX_CTL_FLD;
                phy_status->link = 0x0; /*LINK DOWN */
            } else {
                speedReg=ar9_switch_read_mdio(0x11,0x1c);
                 reg_rgmii = IFX_REG_R32(IFX_AR9_ETH_RGMII_CTL);
                if(speedReg & 0x20) {
                    reg_rgmii |= RGMII_CTL_DUP_SET(1,1);
                    phy_status->duplex = DUPLEX_FULL;
                }else {
                    reg_rgmii |= RGMII_CTL_DUP_SET(1,1);
                    phy_status->duplex = DUPLEX_HALF;
                }
                switch ((speedReg & 0x18) >> 3 ) {
                    case 0 :  /*speed 10BASE-T */
                        reg_rgmii &= ~RGMII_CTL_P1SPD_MASK;
                        reg_rgmii |= RGMII_CTL_P1SPD_10;
                        reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                        /*Port1 force link up*/
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
                        if(eth1_up)
                            reg_port1 |= PX_CTL_FLP;
#else
                        reg_port1 |= PX_CTL_FLP;
#endif 
                        phy_status->speed = SPEED_10;
                        phy_status->link = 0x1; /*LINK UP */
                        break;
                    case 1:	  /*speed 100BASE-TX*/
                        reg_rgmii &= ~RGMII_CTL_P1SPD_MASK;
                        reg_rgmii |= RGMII_CTL_P1SPD_100;
                        reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                        /*Port1 force link up*/
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
                        if(eth1_up)
                            reg_port1 |= PX_CTL_FLP;
#else
                        reg_port1 |= PX_CTL_FLP;
#endif 
                        phy_status->speed = SPEED_100;
                        phy_status->link = 0x1; /*LINK UP */
                        break;
                    case 2:	  /*speed 1000BASE-T*/
                        reg_rgmii &= ~RGMII_CTL_P1SPD_MASK;
                        reg_rgmii |= RGMII_CTL_P1SPD_1000;
                        reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                        /*Port1 force link up*/
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
                        if(eth1_up)
                            reg_port1 |= PX_CTL_FLP;
#else
                        reg_port1 |= PX_CTL_FLP;
#endif 
                        phy_status->speed = SPEED_1000;
                        phy_status->link = 0x1; /*LINK UP */
                        break;
                }
                IFX_REG_W32( reg_rgmii, IFX_AR9_ETH_RGMII_CTL);
            }
            IFX_REG_W32( reg_port1, IFX_AR9_ETH_P1_CTL);
        }
    }
    return IRQ_HANDLED;
}

static irqreturn_t mii1_gphy_11G_int_handler (int irq, void *dev_id)
{
    unsigned short  status, linkReg, speedReg;
    unsigned int reg_rgmii, reg_port1;
    struct mii1_gphy_status *phy_status  = (struct mii1_gphy_status *)dev_id;
    /*read data from phy register to clear phy interrupts */
    /*printk("%s: Got interrupt\n",__func__);*/
    
    status=ar9_switch_read_mdio(0x11,0x1a); 
    /*cause the interrupt due to link status, speed & FDX */
        if( (status & 0x0007)) {
            linkReg=ar9_switch_read_mdio(0x11,0x01);
            /*Link on PHY is down */
            reg_port1 = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
            if((linkReg&0x04)==0) {
               
                reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                /*Port1 force link down */
                reg_port1 |= PX_CTL_FLD;
                phy_status->link = 0x0; /*LINK DOWN */
            } else {
                speedReg=ar9_switch_read_mdio(0x11,0x18);
                 reg_rgmii = IFX_REG_R32(IFX_AR9_ETH_RGMII_CTL);
                if(speedReg & 0x08) {
                    reg_rgmii |= RGMII_CTL_DUP_SET(1,1);
                    phy_status->duplex = DUPLEX_FULL;
                }else {
                    reg_rgmii |= RGMII_CTL_DUP_SET(1,1);
                    phy_status->duplex = DUPLEX_HALF;
                }
                switch ((speedReg & 0x18) >> 3 ) {
                    case 0 :  /*speed 10BASE-T */
                        reg_rgmii &= ~RGMII_CTL_P1SPD_MASK;
                        reg_rgmii |= RGMII_CTL_P1SPD_10;
                        reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                        /*Port1 force link up*/
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
                        if(eth1_up)
                            reg_port1 |= PX_CTL_FLP;
#else
                        reg_port1 |= PX_CTL_FLP;
#endif 
                        phy_status->speed = SPEED_10;
                        phy_status->link = 0x1; /*LINK UP */
                        break;
                    case 1:	  /*speed 100BASE-TX*/
                        reg_rgmii &= ~RGMII_CTL_P1SPD_MASK;
                        reg_rgmii |= RGMII_CTL_P1SPD_100;
                        reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                        /*Port1 force link up*/
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
                        if(eth1_up)
                            reg_port1 |= PX_CTL_FLP;
#else
                        reg_port1 |= PX_CTL_FLP;
#endif 
                        phy_status->speed = SPEED_100;
                        phy_status->link = 0x1; /*LINK UP */
                        break;
                    case 2:	  /*speed 1000BASE-T*/
                        reg_rgmii &= ~RGMII_CTL_P1SPD_MASK;
                        reg_rgmii |= RGMII_CTL_P1SPD_1000;
                        reg_port1 &= ~(PX_CTL_FLD|PX_CTL_FLP);
                        /*Port1 force link up*/
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
                        if(eth1_up)
                            reg_port1 |= PX_CTL_FLP;
#else
                        reg_port1 |= PX_CTL_FLP;
#endif 
                        phy_status->speed = SPEED_1000;
                        phy_status->link = 0x1; /*LINK UP */
                        break;
                }
                IFX_REG_W32( reg_rgmii, IFX_AR9_ETH_RGMII_CTL);
            }
            IFX_REG_W32( reg_port1, IFX_AR9_ETH_P1_CTL);
        }
    
    return IRQ_HANDLED;
}

#endif /*CONFIG_MII1_AUTONEG*/

/* Init of the 3 port switch */
void ar9_3port_sw_hw_uninit(void)
{
#if defined( CONFIG_IFX_MAC1) && CONFIG_IFX_MAC1
#if defined(SELECT_25MHZ_CLOCK_FROM_GPIO) && SELECT_25MHZ_CLOCK_FROM_GPIO
    select_25MhzClock(GPIO_CLK_OUT2,0);
#endif /* SELECT_25MHZ_CLOCK_FROM_GPIO */
    /*GPIO P2.0*/
    reset_ext_gphy(GPIO_RESET_GPHY,0);
#endif /** CONFIG_IFX_MAC1 */

#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG   
    config_gphy_gpio(0);
    free_irq(MII1_GPHY_INT, (void*)&gphy_status);
    reserve_gpiopins_for_mdio(0);
#endif /** CONFIG_MII1_AUTONEG */

}

/* Init of the 3 port switch */
int ar9_3port_sw_hw_init(void)
{
#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG   
    unsigned int id,g_id = 0;
#endif
    unsigned int reg;
    int result = 0;
    
    PPE_TOP_PMU_SETUP(IFX_PMU_ENABLE);
    PPE_DPLUS_PMU_SETUP(IFX_PMU_ENABLE);
    SWITCH_PMU_SETUP(IFX_PMU_ENABLE);
    /**Enable Switch  */
    IFX_REG_W32 ( (IFX_REG_R32(IFX_AR9_ETH_GCTL0) | GCTL0_SE), IFX_AR9_ETH_GCTL0) ;
    
    /** Disable MDIO auto polling mode */
    IFX_REG_W32 ( (IFX_REG_R32(IFX_AR9_ETH_P0_CTL) | PX_CTL_DMDIO), IFX_AR9_ETH_P0_CTL) ;
    switch_hw_init(MII0_MODE_SETUP,0);
#if defined( CONFIG_IFX_MAC1) && CONFIG_IFX_MAC1
#if defined(SELECT_25MHZ_CLOCK_FROM_GPIO) && SELECT_25MHZ_CLOCK_FROM_GPIO
    select_25MhzClock(GPIO_CLK_OUT2,1);
#endif /* SELECT_25MHZ_CLOCK_FROM_GPIO */
    switch_hw_init(MII1_MODE_SETUP,1);
    /*Reset GPHY*/
    /*GPIO P2.0*/
    reset_ext_gphy(GPIO_RESET_GPHY,1);
#endif /** CONFIG_IFX_MAC1 */
    reg = IFX_REG_R32(IFX_AR9_ETH_GCTL0 );
    reg &= ~GCTL0_MPL_MASK;
    reg |= GCTL0_MPL_1522;
    IFX_REG_W32( reg, IFX_AR9_ETH_GCTL0);
    
    reg = IFX_REG_R32(IFX_AR9_ETH_P2_CTL );
    reg |= PX_CTL_BYPASS;
    reg |= PX_CTL_FLP;
    reg |= (PX_CTL_IPVLAN | PX_CTL_SPE | PX_CTL_VPE);
    IFX_REG_W32( reg, IFX_AR9_ETH_P2_CTL);
    reg = IFX_REG_R32(IFX_AR9_ETH_PMAC_HD_CTL );
    reg |= PMAC_HD_CTL_AC;
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    reg |= PMAC_HD_CTL_AS;
#else
    reg &= ~PMAC_HD_CTL_AS;
#endif
#if defined(CONFIG_DMA_PMAC_ENABLE) && CONFIG_DMA_PMAC_ENABLE
    reg |= PMAC_HD_CTL_RXSH;
#else
    reg &= ~PMAC_HD_CTL_RXSH;
#endif
    IFX_REG_W32( reg, IFX_AR9_ETH_PMAC_HD_CTL);
    
/** Due to traffic halt when burst length 8, replace default IPG value with 0x3B*/
     IFX_REG_W32( 0x3B, IFX_AR9_ETH_PMAC_RX_IPG);
    /*Queue Mapping */
    IFX_REG_W32( 0xFA41, IFX_AR9_ETH_1P_PRT);
    IFX_REG_W32( 0x11100000, IFX_AR9_ETH_DFSRV_MAP0);
    IFX_REG_W32( 0x22211111, IFX_AR9_ETH_DFSRV_MAP1);
    IFX_REG_W32( 0x30222222, IFX_AR9_ETH_DFSRV_MAP2);
    IFX_REG_W32( 0x00030003, IFX_AR9_ETH_DFSRV_MAP3);
    
/** Scheduling Scheme
**  Strict Priority scheduling is used for all ports and Rate limit is disabled*/
    IFX_REG_W32( 0x03E803E8, IFX_AR9_ETH_P0_ECS_Q32_REG);
    IFX_REG_W32( 0x03E803E8, IFX_AR9_ETH_P0_ECS_Q10_REG);
    IFX_REG_W32( 0x03E803E8, IFX_AR9_ETH_P1_ECS_Q32_REG);
    IFX_REG_W32( 0x03E803E8, IFX_AR9_ETH_P1_ECS_Q10_REG);
    IFX_REG_W32( 0x03E803E8, IFX_AR9_ETH_P2_ECS_Q32_REG);
    IFX_REG_W32( 0x03E803E8, IFX_AR9_ETH_P2_ECS_Q10_REG);
    
    IFX_REG_W32( 0, IFX_AR9_ETH_P0_ECW_Q32_REG);
    IFX_REG_W32( 0, IFX_AR9_ETH_P0_ECW_Q10_REG);
    IFX_REG_W32( 0, IFX_AR9_ETH_P1_ECW_Q32_REG);
    IFX_REG_W32( 0, IFX_AR9_ETH_P1_ECW_Q10_REG);
    IFX_REG_W32( 0, IFX_AR9_ETH_P2_ECW_Q32_REG);
    IFX_REG_W32( 0, IFX_AR9_ETH_P2_ECW_Q10_REG);

#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG   
    config_gphy_gpio(1);
    
	id = ar9_switch_read_mdio(0x11, 0x3);
	switch ((id >> 4) & 0xf)
	{
		case 2:
    			ar9_switch_write_mdio(0x11, 0x19, 0xf000);
				g_id = GPHY_VSC;
				printk("\n GPHY_VSC ");
				break;
		case 13:
				ar9_switch_write_mdio(0x11, 0x19, 0x0007);
				g_id = GPHY_LANTIQ_11G;
				printk("\n GPHY_LANTIQ ");
				break;
		default:
				printk("\n unknown ID!");
	}
	if (g_id == GPHY_VSC)	
    	result = request_irq(MII1_GPHY_INT, mii1_gphy_VSC_int_handler, IRQF_DISABLED, "ifx_mii1_gphy", (void*)&gphy_status);
    else
		result = request_irq(MII1_GPHY_INT, mii1_gphy_11G_int_handler, IRQF_DISABLED, "ifx_mii1_gphy", (void*)&gphy_status);
    if ( result ){
        printk("*error, cannot get ext int3 irq!\n");
        return -EFAULT;
    }
    /*Falling edge */
    reg = IFX_REG_R32(IFX_ICU_EIU_EXIN_C );
    reg |= (0x2 << (MII1_GPHY_INT_EXT_INT_NO * 4) );
    IFX_REG_W32( reg, IFX_ICU_EIU_EXIN_C);
    reg = IFX_REG_R32(IFX_ICU_EIU_INEN );
    /*Enable External interrupt*/
    reg |= (0x1 << (MII1_GPHY_INT_EXT_INT_NO ) );
    IFX_REG_W32( reg, IFX_ICU_EIU_INEN);
#endif /** CONFIG_MII1_AUTONEG */
    return result;
}


#endif /*_IFXMIPS_3PORT_SW_H_  */

