/******************************************************************************
**
** FILE NAME    : ifxmips_eth.c
** PROJECT      : IFX UEIP
** MODULES      : IFX ethenret device driver
** DATE         : 24 July  2009
** AUTHOR       : Reddy Mallikarjuna
** DESCRIPTION  : IFX Cross-Platform ethernet device driver
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
** $Date                $Author                 $Comment
** 24 July 2009         Reddy Mallikarjuna      Initial UEIP release
*******************************************************************************/

/*!
  \file ifxmips_eth.c
  \ingroup IFX_ETH_DRV
  \brief ETH driver source file for Danube/Amazon-SE.
*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/moduleparam.h>
#include <linux/types.h>  /* size_t */
#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h> /* eth_type_trans */
#include <asm/delay.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_dma_core.h>
#include <asm/ifx/ifx_eth_framework.h>

#include "ifxmips_eth.h"
#include "ifxmips_etop_reg.h"
#if defined(CONFIG_AMAZON_SE)
#include "ifxmips_eth_gpio.h"
#endif /*CONFIG_AMAZON_SE*/

#define IFX_DRV_MODULE_NAME             "ifx_ethernet"
#define IFX_DRV_MODULE_VERSION          "1.0.5"
static char version[] __devinitdata =
        IFX_DRV_MODULE_NAME".c:v" IFX_DRV_MODULE_VERSION " \n";

/* length of time before we decide the hardware is borked,
 * and dev->tx_timeout() should be called to fix the problem
 */
#define IFX_TX_TIMEOUT                  (10 * HZ)
#define NUM_ETH_INF                     1
/* Burst length 8 will not support */
#define DMA_TX_BURST_LEN                DMA_BURSTL_4DW
#define DMA_RX_BURST_LEN                DMA_BURSTL_4DW
#define ETH_PKT_BUF_SIZE                1536

static int interface_mode;
#if defined(CONFIG_REV_MII_MAC_MODE)
#define INTERFACE_MODE                  REV_MII_MAC_MODE
#elif defined(CONFIG_RED_MII_MAC_MODE)
#define INTERFACE_MODE                  RED_MII_MAC_MODE
#elif defined(CONFIG_EPHY_MODE)
#define INTERFACE_MODE                  MII_PHY_MODE
#endif
/* pointer to the dma device structure */
struct dma_device_info *g_dma_device=NULL;
/*proc file directory*/
static struct proc_dir_entry*   ifx_eth_dir;
/* Pointer to netword devices structure */
static struct net_device *ifx_eth_dev[NUM_ETH_INF ];

static int ifx_eth_open(struct net_device *dev);
static int ifx_eth_close(struct net_device *dev);
static int ifx_start_xmit (struct sk_buff *skb, struct net_device *dev);
static int ifx_eth_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
static struct net_device_stats *ifx_get_stats (struct net_device *dev);
static int ifx_change_mtu (struct net_device *dev, int new_mtu);
/*switch set mac address*/
static int ifx_set_mac_address(struct net_device *dev, void *p);
/*Ethernet transmit timeout*/
static void ifx_tx_timeout (struct net_device *dev);
/* select the tx dma channel */
static int ifx_select_tx_chan (struct sk_buff *skb, struct net_device *dev);
/* Turn ON dma rx channel*/
static void open_rx_dma(void);
/* Turn off the dma rx channel*/
static void close_rx_dma(void);
/** Init of the network device */
static int switch_init(struct net_device *dev);
/* Get the ether addr from u-boot */
static unsigned char my_ethaddr[MAX_ADDR_LEN];
/*************************************************************************************************/
/** Get the driver information */
static void ifx_get_drvinfo (struct net_device *dev, struct ethtool_drvinfo *info)
{
     /* driver driver short name (Max 32 characters) */
    strcpy (info->driver, IFX_DRV_MODULE_NAME);
    /* driver version (Max 32 characters) */
    strcpy (info->version, IFX_DRV_MODULE_VERSION);
}

/** Get the network device settings  */
static int ifx_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev);
    unsigned int reg_val;

    cmd->supported = (SUPPORTED_Autoneg | SUPPORTED_TP);
    cmd->supported |= (SUPPORTED_100baseT_Half |    \
                    SUPPORTED_100baseT_Full |       \
                    SUPPORTED_10baseT_Half |        \
                    SUPPORTED_10baseT_Full |        \
                    SUPPORTED_MII);

    cmd->advertising = 0;
    if (priv->flags & IFX_FLAG_ADV_10HALF)
        cmd->advertising |= ADVERTISED_10baseT_Half;
    if (priv->flags & IFX_FLAG_ADV_10FULL)
        cmd->advertising |= ADVERTISED_10baseT_Full;
    if (priv->flags & IFX_FLAG_ADV_100HALF)
        cmd->advertising |= ADVERTISED_100baseT_Half;
    if (priv->flags & IFX_FLAG_ADV_100FULL)
        cmd->advertising |= ADVERTISED_100baseT_Full;
    cmd->advertising |= ADVERTISED_Pause | ADVERTISED_Asym_Pause;
    reg_val = ETOP_READ_REG32(ENET0_MAC_CFG_REG);
    if (MAC_CFG_SPEED_GET(reg_val) )
        cmd->speed =  SPEED_100;
    else
        cmd->speed =  SPEED_10;

    priv->current_speed = cmd->speed;

    if(MAC_CFG_DUPLEX_GET(reg_val))
        cmd->duplex = DUPLEX_FULL ;
    else
        cmd->duplex = DUPLEX_HALF;
    priv->full_duplex = cmd->duplex;
    cmd->port = PORT_MII;
    cmd->phy_address = priv->phy_addr;
    cmd->transceiver = (priv->flags & IFX_FLAG_INTERNAL_PHY) ? XCVR_INTERNAL : XCVR_EXTERNAL;
    reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
    if (reg_val & MDIO_CFG_UMM0 )
        cmd->autoneg = AUTONEG_ENABLE;
    else
        cmd->autoneg = AUTONEG_DISABLE ;
    if (cmd->autoneg == AUTONEG_ENABLE)
        cmd->advertising |= ADVERTISED_Autoneg;

    return 0;
}

/** Set the network device settings */
static int ifx_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev);
    unsigned int val = 0, reg_val;

    if (cmd->autoneg == AUTONEG_ENABLE) {
        /* Danube & ASE do not support gigabit. */
        if (cmd->advertising & (ADVERTISED_1000baseT_Half |ADVERTISED_1000baseT_Full))
            return -EINVAL;
    } else if ((cmd->speed != SPEED_100 && cmd->speed != SPEED_10) \
            ||(cmd->duplex != DUPLEX_HALF && cmd->duplex != DUPLEX_FULL)) {
            return -EINVAL;
    }
    spin_lock_irq(&priv->lock);
    if (cmd->autoneg == AUTONEG_ENABLE) {
        reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
        if (reg_val & MDIO_CFG_UMM0) {
            reg_val |= ( MDIO_CFG_SMRST);
            ETOP_WRITE_REG32(reg_val, ETOP_MDIO_CFG_REG);
        }
    } else {
        priv->flags |= IFX_FLAG_FORCE_LINK;
        priv->flags &= ~(IFX_FLAG_100_BASE_T | IFX_FLAG_FULL_DUPLEX);
        if (cmd->speed == SPEED_100)
            priv->flags |= IFX_FLAG_100_BASE_T;
        if (cmd->duplex == DUPLEX_FULL)
            priv->flags |= IFX_FLAG_FULL_DUPLEX;
        if (interface_mode != MII_PHY_MODE) {
            reg_val = ETOP_READ_REG32(ENET0_MAC_CFG_REG);
            if (priv->flags & IFX_FLAG_100_BASE_T)
                val |= MAC_CFG_SPEED_SET(1);
            if (priv->flags & IFX_FLAG_FULL_DUPLEX)
                val |= MAC_CFG_DUPLEX_SET(1);
            if ( priv->flags & IFX_FLAG_FORCE_LINK )
                val |= MAC_CFG_LINK(1);
            reg_val &= ~(0x7);
            reg_val |= val;
            ETOP_WRITE_REG32(reg_val, ENET0_MAC_CFG_REG);
        } else {
        /*TODO*/
            printk("%s: TODO for MII_PHY_MODE\n",__func__);
        }
    }
    spin_unlock_irq(&priv->lock);

    return 0;
}

/** Reset the device */
static int ifx_nway_reset(struct net_device *dev)
{
    IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev);
    unsigned int  reg_val;
    spin_lock_irq(&priv->lock);
    reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
    if (reg_val & MDIO_CFG_UMM0) {
        reg_val |= ( MDIO_CFG_SMRST);
        ETOP_WRITE_REG32(reg_val, ETOP_MDIO_CFG_REG);
    }
    spin_unlock_irq(&priv->lock);
    return 0;
}

/** Structure of the ether tool operation  */
static const struct ethtool_ops ifx_ethtool_ops = {
        .get_drvinfo            = ifx_get_drvinfo,
        .get_settings           = ifx_get_settings,
        .set_settings           = ifx_set_settings,
        .nway_reset             = ifx_nway_reset,
        .get_link               = ethtool_op_get_link,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        .get_perm_addr          = ethtool_op_get_perm_addr,
#endif
};

#ifndef MODULE
/* Get the ether addr from u-boot */
static int __init ethaddr_setup(char *line)
{
    char *ep;
    int i;
    memset(my_ethaddr, 0, MAX_ADDR_LEN);
    /* there should really be routines to do this stuff */
    for (i = 0; i < 6; i++)	{
        my_ethaddr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
        if (line)
            line = (*ep) ? ep+1 : ep;
    }
    printk("mac address %2x-%2x-%2x-%2x-%2x-%2x \n" \
        ,my_ethaddr[0]  \
        ,my_ethaddr[1]  \
        ,my_ethaddr[2]  \
        ,my_ethaddr[3]  \
        ,my_ethaddr[4]  \
        ,my_ethaddr[5]);
    return 0;
}
__setup("ethaddr=", ethaddr_setup);
#endif

/** Turn ON RX DMA channels */
static void open_rx_dma(void)
{
    struct dma_device_info* dma_dev=g_dma_device;
    int i;

    for(i=0; i<dma_dev->max_rx_chan_num; i++) {
        if ( (dma_dev->rx_chan[i])->control==IFX_DMA_CH_ON )
            dma_dev->rx_chan[i]->open(dma_dev->rx_chan[i]);
    }
}

/** Turn off the RX DMA channels */
static void close_rx_dma()
{
    struct dma_device_info* dma_dev=g_dma_device;
    int i;

    for (i=0; i<dma_dev->max_rx_chan_num; i++)
        dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);
}

/** open the network device interface*/
static int ifx_eth_open(struct net_device *dev)
{
    open_rx_dma();
//    netif_start_queue(dev);
    return IFX_SUCCESS;
}

/** Close the network device interface*/
static int ifx_eth_close(struct net_device *dev)
{
    close_rx_dma();
//    netif_stop_queue(dev);
    return IFX_SUCCESS;
}

/** Send the packet to netwrok rx queue, used by  switch_hw_receive function */
static void ifx_eth_rx(struct net_device *dev, int len,struct sk_buff* skb)
{
    IFX_switch_priv_t *priv             = ifx_eth_fw_netdev_priv(dev);

    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
#ifdef CONFIG_NAPI_ENABLED
    netif_receive_skb(skb);
#else
    netif_rx(skb);
#endif
    priv->stats.rx_packets++;
    priv->stats.rx_bytes+=len;
}

/* This function is called in dma intr handler (DMA RCV_INT interrupt).
* This function get the data from the DMA device.  */
static void switch_hw_receive(struct dma_device_info* dma_dev)
{
    unsigned char* buf=NULL;
    int len=0 ;
    struct sk_buff *skb=NULL;
    struct net_device *dev;

    len = dma_device_read(dma_dev,&buf,(void**)&skb);
    if ((len >= 0x600) || (len < 64) ) {
        printk(KERN_ERR "%s[%d]: Packet is too large/small (%d)!!!\n", __func__,__LINE__,len);
        goto rx_err_exit;
    }
    if (skb == NULL  ) {
        printk(KERN_ERR "%s[%d]: Can't restore the Packet !!!\n", __func__,__LINE__);
        goto rx_err_exit;
    }
    /* remove CRC */
    len -= 4;
    if (len > (skb->end -skb->tail)) {
        printk(KERN_ERR "%s[%d]: len:%d end:%p tail:%p Err!!!\n", __func__,__LINE__,(len+4), skb->end, skb->tail);
        goto rx_err_exit;
    }
    skb_put(skb,len);
    dev= ifx_eth_dev[0];
    skb->dev = dev;
    ifx_eth_rx(dev,len,skb);
    return ;
rx_err_exit:
    if (skb)
        dev_kfree_skb_any(skb);
    return ;
}

/* Get the network device stats information */
static struct net_device_stats *ifx_get_stats (struct net_device *dev)
{
    IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev);
    return &priv->stats;
}

/* Trasmit timeout */
static void ifx_tx_timeout(struct net_device *dev)
{
    IFX_switch_priv_t *priv		= ifx_eth_fw_netdev_priv(dev);
    struct dma_device_info* dma_dev = g_dma_device;
    int i;
    printk(KERN_ERR "%s: transmit timed out, disable the dma channel irq\n", dev->name);
    priv->stats.tx_errors++;
    for (i=0; i<dma_dev->num_tx_chan; i++) {
        dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
    }
    netif_wake_queue(dev);
}

/** set mac address, used by ifx_set_mac_address function */
/*
static void __ifx_set_mac_addr (unsigned char *data)
{
    u32 val;
    val  = ((u32) data[2]) << 0;
    val |= ((u32) data[3]) << 8;
    val |= ((u32) data[4]) << 16;
    val |= ((u32) data[5]) << 24;
    ETOP_WRITE_REG32(val, ENETS0_MAC_DA0_REG);
    val  = ((u32) data[0]) << 16;
    val |= ((u32) data[1]) << 24;
    ETOP_WRITE_REG32(val, ENETS0_MAC_DA1_REG);
}
*/

/* Set the MAC address */
static int ifx_set_mac_address (struct net_device *dev, void *p)
{
/*
    IFX_switch_priv_t *priv		= ifx_eth_fw_netdev_priv(dev);
    */

    struct sockaddr *addr = p;
    if (netif_running(dev))
        return -EBUSY;
    if (!is_valid_ether_addr(addr->sa_data))
        return -EINVAL;
    memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
/*
    spin_lock_irq(&priv->lock);
    if (!(dev->flags & IFF_PROMISC)) {
        __ifx_set_mac_addr(dev->dev_addr);
    }
    spin_unlock_irq(&priv->lock);
*/
    return 0;
}

/** Change the MTU value of the netwrok device interfaces */
static int ifx_change_mtu (struct net_device *dev, int new_mtu)
{
    if(new_mtu < ETH_ZLEN || new_mtu > ETH_DATA_LEN)
        return -EINVAL;
    dev->mtu = new_mtu;
    return 0;
}

/** select the DMA channel numbers, Please refer to the dma_setup_init function */
static int ifx_select_tx_chan (struct sk_buff *skb, struct net_device *dev)
{
    int chan_no;
/*TODO: select the channel based on some criteria*/
    chan_no = 0;
    return chan_no;
}

/** Transmit packet over DMA, which comes from the Tx Queue */
static int ifx_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    IFX_switch_priv_t *priv		= ifx_eth_fw_netdev_priv(dev);
    int len , rc = NETDEV_TX_OK;
    char *data;
    struct dma_device_info* dma_dev=g_dma_device;

    len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
    data = skb->data;
    priv->skb = skb;
    dev->trans_start = jiffies;
    /*select the tx channel*/
    dma_dev->current_tx_chan = ifx_select_tx_chan (skb, dev);
    if (dma_device_write(dma_dev, data, len, skb) != len ) {
        if (skb)
            dev_kfree_skb_any(skb);
        priv->stats.tx_errors++;
        priv->stats.tx_dropped++;
  /*      rc = NETDEV_TX_BUSY;  */
    } else {
        priv->stats.tx_packets++;
        priv->stats.tx_bytes+=len;
    }
    return rc;
}

/** Enable VLAN Class oF Service */
static void set_vlan_cos(_vlan_cos_req* vlan_cos_req)
{
    unsigned int pri = vlan_cos_req->pri;
    unsigned int cos_value = vlan_cos_req->cos_value;
    unsigned int reg_val = ETOP_READ_REG32(ETOP_IG_VLAN_COS_REG);
    reg_val &= ~(3 << (pri * 2));
    reg_val |= ( cos_value << (pri * 2) );
    ETOP_WRITE_REG32(reg_val, ETOP_IG_VLAN_COS_REG);

}

/** Enable DSCP Class Of Service */
static void set_dscp_cos(_dscp_cos_req* dscp_cos_req)
{
    int dscp = dscp_cos_req->dscp;
    int cos_value = dscp_cos_req->cos_value;
    unsigned int reg_val = ETOP_READ_REG32(ETOP_IG_DSCP_COS0_REG -(dscp/16*4));
    reg_val &= ~(3 << (dscp%16*2));
    reg_val |= (cos_value << (dscp%16*2));
    ETOP_WRITE_REG32(reg_val, (ETOP_IG_DSCP_COS0_REG - (dscp/16*4)));

}

/** Platform specific IOCTL's handler */
static int ifx_eth_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
  /*  IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev); */
    _vlan_cos_req etop_vlan_cos_req;
    _dscp_cos_req etop_dscp_cos_req;
    unsigned int reg_val;
    int err = -EINVAL;
    if (!netif_running(dev))
        goto out;
    switch(cmd){
        case SET_VLAN_COS:
            copy_from_user(&etop_vlan_cos_req, (_vlan_cos_req*)ifr->ifr_data,sizeof(_vlan_cos_req));
            set_vlan_cos(&etop_vlan_cos_req);
            break;
        case SET_DSCP_COS:
            copy_from_user(&etop_dscp_cos_req, (_vlan_cos_req*)ifr->ifr_data,sizeof(_dscp_cos_req));
            set_dscp_cos(&etop_dscp_cos_req);
            break;
        case ENABLE_VLAN_COS:
            reg_val = ETOP_READ_REG32(ENETS0_COS_CFG_REG);
            reg_val |= COS_CFG_VLAN;
            ETOP_WRITE_REG32(reg_val, ENETS0_COS_CFG_REG);
            break;
        case DISABLE_VLAN_COS:
            reg_val = ETOP_READ_REG32(ENETS0_COS_CFG_REG);
            reg_val &= ~COS_CFG_VLAN;
            ETOP_WRITE_REG32(reg_val, ENETS0_COS_CFG_REG);
            break;
        case ENABLE_DSCP_COS:
            reg_val = ETOP_READ_REG32(ENETS0_COS_CFG_REG);
            reg_val |= COS_CFG_DSCP;
            ETOP_WRITE_REG32(reg_val, ENETS0_COS_CFG_REG);
            break;
        case DISABLE_DSCP_COS:
            reg_val = ETOP_READ_REG32(ENETS0_COS_CFG_REG);
            reg_val &= ~COS_CFG_DSCP;
            ETOP_WRITE_REG32(reg_val, ENETS0_COS_CFG_REG);
            break;
        case VLAN_CLASS_FIRST:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val |= ENETS0_CFG_VL2;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case VLAN_CLASS_SECOND:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val &= ~ENETS0_CFG_VL2;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case PASS_UNICAST_PACKETS:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val &= ~ENETS0_CFG_FTUC;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case FILTER_UNICAST_PACKETS:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val |= ENETS0_CFG_FTUC;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case KEEP_BROADCAST_PACKETS:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val &= ~ENETS0_CFG_DPBC;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case DROP_BROADCAST_PACKETS:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val |= ENETS0_CFG_DPBC;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case KEEP_MULTICAST_PACKETS:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val &= ~ENETS0_CFG_DPMC;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
        case DROP_MULTICAST_PACKETS:
            reg_val = ETOP_READ_REG32(ENETS0_CFG_REG);
            reg_val |= ENETS0_CFG_DPMC;
            ETOP_WRITE_REG32(reg_val, ENETS0_CFG_REG);
            break;
    }
    err = 0;
out:
    return err;
}

/**
* DMA Pseudo Interrupt handler.
* This function handle the DMA pseudo interrupts to handle the data packets Rx/Tx over DMA channels
* It will handle the following interrupts
*   RCV_INT: DMA receive the packet interrupt,So get from the PPE peripheral
*   TX_BUF_FULL_INT: TX channel descriptors are not availabe, so, stop the transmission
        and enable the Tx channel interrupt.
*   TRANSMIT_CPT_INT: Tx channel descriptors are availabe and resume the transmission and
        disable the Tx channel interrupt.
*/
int dma_intr_handler(struct dma_device_info* dma_dev,int status)
{
    struct net_device* dev;
    int i;
    switch(status) {
        case RCV_INT:
            switch_hw_receive(dma_dev);
            break;
        case TX_BUF_FULL_INT:
            for(i=0; i < NUM_ETH_INF ; i++){
                dev = ifx_eth_dev[i];
                netif_stop_queue(dev);
            }
            for(i=0;i<dma_dev->num_tx_chan;i++) {
                if((dma_dev->tx_chan[i])->control==IFX_DMA_CH_ON)
                    dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
                }
            break;
        case TRANSMIT_CPT_INT:
            for(i=0;i<dma_dev->num_tx_chan;i++) {
                dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
            }
            for(i=0; i < NUM_ETH_INF ; i++){
                dev = ifx_eth_dev[i];
                netif_wake_queue(dev);
            }
            break;
    }
    return IFX_SUCCESS;
}

/**
* Allocates the buffer for ethernet packet.
* This function is invoke when DMA callback function to be called
*   to allocate a new buffer for Rx packets.*/
u8* etop_dma_buffer_alloc(int len, int* byte_offset,void** opt)
{
    unsigned char *buffer=NULL;
    struct sk_buff *skb=NULL;
    *byte_offset=2; /* for reserving 2 bytes in skb buffer, so, set offset 2 bytes infront of data pointer */
    skb = dev_alloc_skb(ETH_PKT_BUF_SIZE);
    if (skb == NULL) {
        printk(KERN_ERR "%s[%d]: Buffer allocation failed!!!\n", __func__,__LINE__);
        return NULL;
    }
    buffer=(u8*)(skb->data);
    skb_reserve(skb, 2);
    *(int*)opt=(int)skb;
    return buffer;
}

/* Free skb buffer
* This function frees a buffer previously allocated by the DMA buffer
*   alloc callback function. */
int etop_dma_buffer_free(u8* dataptr,void* opt)
{
    struct sk_buff *skb=NULL;
    if(opt==NULL){
        if(dataptr)
            kfree(dataptr);
    }else {
        skb=(struct sk_buff*)opt;
        if(skb)
            dev_kfree_skb_any(skb);
    }
    return IFX_SUCCESS;
}

#ifdef  CONFIG_NAPI_ENABLED
/** This function scheduled from upper layer when the NAPI is enabled*/
//static int switch_poll(struct net_device *poll_dev, int *budget)
//{
//    int work_to_do, work_done, ret;
//    struct dma_device_info* dma_dev=g_dma_device;
//    work_to_do = min(*budget, poll_dev->quota);
//    work_done = 0;
//    ret = dma_device_poll(dma_dev, work_to_do, &work_done);
//    *budget -= work_done;
//    poll_dev->quota -= work_done;
//    return ret;
//}
static ifx_eth_fw_poll_ret_t switch_poll(struct net_device *poll_dev, int work_to_do, int *work_done)
{
    int ret;

    ret = dma_device_poll(g_dma_device, work_to_do, work_done);
    return ret == 0 ? IFX_ETH_FW_POLL_COMPLETE : IFX_ETH_FW_POLL_CONTINUE;
}

static void eth_activate_poll(struct dma_device_info* dma_dev)
{
    struct net_device *dev;
    int i;
    for(i=0; i < NUM_ETH_INF ; i++) {
        dev = ifx_eth_dev[i];
//        if ( netif_rx_schedule_prep(dev) )
//        __netif_rx_schedule(dev);
        ifx_eth_fw_poll_schedule(dev);
    }
}

static void eth_inactivate_poll(struct dma_device_info* dma_dev)
{
    struct net_device *dev;
    int i;
    for(i=0; i < NUM_ETH_INF ; i++) {
        dev = ifx_eth_dev[i];
//        if(netif_running(dev) )
//            netif_rx_complete(dev);
        ifx_eth_fw_poll_complete(dev);
    }
}
#endif

/** Unregister with DMA device core driver */
static void dma_setup_uninit(void)
{
    struct dma_device_info* dma_dev=g_dma_device;
    if( dma_dev ) {
        dma_device_unregister(dma_dev);
        dma_device_release(dma_dev);
    }
}

/** Register with DMA device core driver */
static int dma_setup_init(void)
{
    int i, ret;

    g_dma_device=dma_device_reserve("PPE");
    if(!g_dma_device) {
        printk(KERN_ERR "%s[%d]: Reserved with DMA core driver failed!!!\n", __func__,__LINE__);
        return -ENODEV;
    }
    g_dma_device->buffer_alloc              = &etop_dma_buffer_alloc;
    g_dma_device->buffer_free               = &etop_dma_buffer_free;
    g_dma_device->intr_handler              = &dma_intr_handler;
    g_dma_device->num_rx_chan               = 4;
/* one channel is enough for one network device(eth0) */
    g_dma_device->num_tx_chan               = 1;
    g_dma_device->tx_burst_len              = DMA_TX_BURST_LEN;
    g_dma_device->rx_burst_len              = DMA_RX_BURST_LEN;
    g_dma_device->tx_endianness_mode        = IFX_DMA_ENDIAN_TYPE3;
    g_dma_device->rx_endianness_mode        = IFX_DMA_ENDIAN_TYPE3;
    /*g_dma_device->port_packet_drop_enable   = IFX_DISABLE; */
    for (i = 0; i < g_dma_device->num_rx_chan; i++) {
        g_dma_device->rx_chan[i]->packet_size       =  ETH_PKT_BUF_SIZE;
        g_dma_device->rx_chan[i]->control           = IFX_DMA_CH_ON;
    }
    for (i = 0; i < g_dma_device->num_tx_chan; i++) {
        if ( i == 0 ) /* TODO: Used only one DMA Tx channel */
            g_dma_device->tx_chan[i]->control       = IFX_DMA_CH_ON;
        else
            g_dma_device->tx_chan[i]->control       = IFX_DMA_CH_OFF;
    }
#ifdef CONFIG_NAPI_ENABLED
    g_dma_device->activate_poll     = eth_activate_poll;
    g_dma_device->inactivate_poll   = eth_inactivate_poll;
#endif
    ret = dma_device_register (g_dma_device);
    if ( ret != IFX_SUCCESS)
        printk(KERN_ERR "%s[%d]: Register with DMA core driver Failed!!!\n", __func__,__LINE__);

    return ret;
}

/** Used only for Amazon-SE platform*/
#if defined(CONFIG_AMAZON_SE)
/** select EPHY clock */
static void setEphyClk(void)
{
    unsigned int reg_val;
    reg_val = ETOP_READ_REG32(IFX_CGU_IF_CLK);
    reg_val |= (0x1 << 4);
    ETOP_WRITE_REG32(reg_val, IFX_CGU_IF_CLK);
}

/** select Red Mii reference clock */
static void setRedMiiRefClk(void)
{
    unsigned int reg_val;
    reg_val = ETOP_READ_REG32(IFX_CGU_IF_CLK);
    reg_val |= (0x3 << 24);
    ETOP_WRITE_REG32(reg_val, IFX_CGU_IF_CLK);
    reg_val = ETOP_READ_REG32(IFX_CGU_OSC_CON);
    reg_val &= ~(0x18);
    ETOP_WRITE_REG32(reg_val, IFX_CGU_OSC_CON);
}
#endif /*CONFIG_AMAZON_SE*/

/** Hardware initilization */
void etop_hw_init(int mode)
{
    unsigned int reg_val;
#if defined(CONFIG_AMAZON_SE)
    int i;
    /** Turnoff the PPE */
    ETOP_WRITE_REG32(0, ETOP_PPE32_BASE_ADDR);
    /*Reset PPE modue */
    reg_val = ETOP_READ_REG32(IFX_RCU_RST_REQ);
    reg_val |= 0x100;
    ETOP_WRITE_REG32(reg_val, IFX_RCU_RST_REQ);
	for(i=0;i<100;i++) udelay(100);
#endif  /*CONFIG_AMAZON_SE*/
    /* Enable PPE module from PMU*/
    PPE_TPE_PMU_SETUP(IFX_PMU_MODULE_PPE_TPE);
    /* Enable EENET0 module from PMU */
    PPE_ENET0_PMU_SETUP(IFX_PMU_MODULE_PPE_ENET0);
    switch(mode) {
        case REV_MII_MAC_MODE:
            reg_val = ETOP_READ_REG32(ETOP_CFG_REG);
             /*set interface is in off mode */
            reg_val |= (CFG_OFF0);
            ETOP_WRITE_REG32(reg_val, ETOP_CFG_REG);
            /*Configure rev MII mode */
            reg_val |= (CFG_REMII0);
            /*Disable turbo mode */
            reg_val &= ~(CFG_TURBO0);
            /*set ENET1 interface is in off mode (Danube only)*/
            reg_val |= CFG_OFF1;
            /*set ENET0 interface is in working mode */
            reg_val &= ~(CFG_OFF0);
            ETOP_WRITE_REG32(reg_val, ETOP_CFG_REG);
            reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
            /*Disable Auto sending mode */
            reg_val &= ~(MDIO_CFG_UMM0|MDIO_CFG_UMM1);
            ETOP_WRITE_REG32(reg_val, ETOP_MDIO_CFG_REG);
            printk("Selected REV MII MAC mode \n");
            break;
#if defined(CONFIG_AMAZON_SE)
/*Danube does not supported */
        case RED_MII_MAC_MODE:
            reg_val = ETOP_READ_REG32(ETOP_CFG_REG);
             /*set interface is in off mode */
            reg_val |= (CFG_OFF0);
            ETOP_WRITE_REG32(reg_val, ETOP_CFG_REG);
            setRedMiiRefClk();
            amazon_se_eth_gpio_configure(1);
            reg_val &= ~(CFG_INT_PHY);
            /* enable Reduced MII mode */
            reg_val |= (CFG_RMII0);
            /*set ENET0 interface is in working mode */
            reg_val &= ~(CFG_OFF0);
            ETOP_WRITE_REG32(reg_val, ETOP_CFG_REG);
            reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
            /*Disable Auto sensing mode */
            reg_val &= ~(MDIO_CFG_UMM0);
            ETOP_WRITE_REG32(reg_val, ETOP_MDIO_CFG_REG);
            printk("Selected RED MII MAC mode \n");
            break;
        case MII_PHY_MODE:
            /* Enable EPHY module from PMU */
            EPHY_PMU_SETUP(IFX_PMU_MODULE_EPHY);
             reg_val = ETOP_READ_REG32(ETOP_CFG_REG);
             /*set interface is in off mode */
            reg_val |= (CFG_OFF0);
            ETOP_WRITE_REG32(reg_val, ETOP_CFG_REG);
            /*clock from CGU */
            setEphyClk();
            reg_val = ETOP_READ_REG32(ETOP_CFG_REG);
             /*set interface is in working mode */
            reg_val &= ~(CFG_OFF0);
            reg_val |= (CFG_INT_PHY);
            ETOP_WRITE_REG32(reg_val, ETOP_CFG_REG);
            reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
            /*Disable Auto sensing mode */
            reg_val &= ~(MDIO_CFG_UMM0);
            ETOP_WRITE_REG32(reg_val, ETOP_MDIO_CFG_REG);
            /*Due to the bug in HW, need a delay*/
            udelay(100);
            /*0x8 PHY address, 0x12 Class A phy register address, 0cc020 is data to write in Class A phy reg */
            reg_val = MDIO_ACC_PHYA_SET(0x8)| MDIO_ACC_REGA_SET(0x12) |MDIO_ACC_PHYD(0xC020) | MDIO_ACC_RA;
            ETOP_WRITE_REG32(reg_val, ETOP_MDIO_ACC_REG);
            udelay(125);
            do {
                reg_val = ETOP_READ_REG32(ETOP_MDIO_ACC_REG);
            }while ( reg_val & MDIO_ACC_RA ) ;
            /*Reenable Auto sensing mode */
            reg_val = ETOP_READ_REG32(ETOP_MDIO_CFG_REG);
            reg_val |= (MDIO_CFG_UMM0);
            ETOP_WRITE_REG32(reg_val, ETOP_MDIO_CFG_REG);
            printk("Selected EPHY mode \n");
            break;
#endif  /* CONFIG_AMAZON_SE*/
    }

    reg_val = ETOP_READ_REG32(ENET0_MAC_CFG_REG);
    /*Enable CRC generation*/
    reg_val |= MAC_CFG_CGEN_SET(1);
    ETOP_WRITE_REG32(reg_val, ENET0_MAC_CFG_REG);
    reg_val = 0;
    /*Set frame over/under size frame length*/
    reg_val = (IG_PLEN_CTRL_OVER(IFX_MAX_MTU)|IG_PLEN_CTRL_UNDER_SET(IFX_MIN_MTU));
    ETOP_WRITE_REG32(reg_val, ETOP_IG_PLEN_CTRL_REG);

}

/** init of the network device */
static int switch_init(struct net_device *dev)
{
    u64 retval;
    static int macVal=0;
    int i;
    /*printk("%s up\n",dev->name); */
//    dev->open            = ifx_eth_open;
//    dev->stop            = ifx_eth_close;
//    dev->hard_start_xmit = ifx_start_xmit;
//    dev->do_ioctl        = ifx_eth_ioctl;
//    dev->get_stats       = ifx_get_stats;
//    dev->change_mtu      = ifx_change_mtu;
//    dev->set_mac_address = ifx_set_mac_address;
//    dev->tx_timeout      = ifx_tx_timeout;
//    dev->watchdog_timeo  = IFX_TX_TIMEOUT;
//#ifdef CONFIG_NAPI_ENABLED
//    dev->poll            = switch_poll;
//    dev->weight          = 64;
//#endif

    SET_ETHTOOL_OPS(dev, &ifx_ethtool_ops);

    for ( i = 0, retval = 0; i < 6; i++ )
        retval += dev->dev_addr[i];
    if ( retval == 0 ) {
        /*read the mac address from the mac table and put them into the mac table.*/
      	for (i = 0, retval = 0; i < 6; i++) {
    		retval +=my_ethaddr[i];
    	}
    	/* if ethaddr not set in u-boot, then use default one */
    	if (retval == 0) {
    	    dev->dev_addr[0] = 0x00;
    		dev->dev_addr[1] = 0x20;
    		dev->dev_addr[2] = 0xda;
    		dev->dev_addr[3] = 0x86;
    		dev->dev_addr[4] = 0x23;
    		dev->dev_addr[5] = 0x74 + macVal;
    	} else {
    	    for (i = 0; i < 6; i++) {
    	        dev->dev_addr[i] = my_ethaddr[i];
    	    }
    	    dev->dev_addr[5] += + macVal ;
    	}
    	macVal++;
    }
    return 0;
}
/** Driver version info */
static inline int eth_drv_ver(char *buf)
{
    return sprintf(buf, "IFX Eth driver, version %s,(c)2009 Infineon Technologies AG\n", version);
}

/** Displays the version of ETH module via proc file */
static int eth_proc_version(IFX_char_t *buf, IFX_char_t **start, off_t offset,
                         int count, int *eof, void *data)
{
    int len = 0;
    /* No sanity check cos length is smaller than one page */
    len += eth_drv_ver(buf + len);
    *eof = 1;
    return len;
}

/** create proc for debug  info, ifx_eth_module_init */
static int eth_proc_create(void)
{
    /* procfs */
    ifx_eth_dir = proc_mkdir ("driver/ifx_eth", NULL);
    if (ifx_eth_dir == NULL) {
        printk(KERN_ERR "%s: Create proc directory (/driver/ifx_eth) failed!!!\n", __func__);
        return IFX_ERROR;
    }
    create_proc_read_entry("version", 0, ifx_eth_dir, \
                    eth_proc_version,  NULL);
    return IFX_SUCCESS;
}

/** remove of the proc entries, ifx_eth_module_exit */
static void eth_proc_delete(void)
{

    remove_proc_entry("version", ifx_eth_dir);
    remove_proc_entry("driver/ifx_eth",  NULL);
}

/** init the Ethernet module */
int ifx_eth_module_init (void)
{
    struct ifx_eth_fw_netdev_ops ops = {
        .init           = switch_init,
        .get_stats      = ifx_get_stats,
        .open           = ifx_eth_open,
        .stop           = ifx_eth_close,
        .start_xmit     = ifx_start_xmit,
        .set_mac_address= ifx_set_mac_address,
        .change_mtu     = ifx_change_mtu,
        .do_ioctl       = ifx_eth_ioctl,
        .tx_timeout     = ifx_tx_timeout,
        .watchdog_timeo	= IFX_TX_TIMEOUT,
#ifdef  CONFIG_NAPI_ENABLED
        .poll           = switch_poll,
        .weight         = 25,
#endif
    };
    int  i,  err;
    IFX_switch_priv_t* priv;
    char ver_str[128] = {0};
    interface_mode = INTERFACE_MODE;
    /** Register with DM core driver */
    dma_setup_init();
    /** HW init */
    etop_hw_init(interface_mode);

    for (i=0; i< NUM_ETH_INF ; i++) {
        //ifx_eth_dev[i] = alloc_etherdev(sizeof(IFX_switch_priv_t));
        char name[16];
        sprintf(name, "eth%d", i);
        ifx_eth_dev[i] = ifx_eth_fw_alloc_netdev(sizeof(IFX_switch_priv_t), name, &ops);
        if (!ifx_eth_dev[i]) {
            printk(KERN_ERR "%s[%d]: no memory for ifx_eth_dev!!!\n", __func__,__LINE__);
            err = -ENOMEM;
            goto err_out_free_res;
        }
//        ifx_eth_dev[i]->init=switch_init;
        priv = ifx_eth_fw_netdev_priv(ifx_eth_dev[i]);
        if (interface_mode == MII_PHY_MODE) {
            priv->phy_addr = 0x08; /*Internal PHY address */
        } else {
            priv->phy_addr = 0x00;
        }

        /* By default, advertise supported  speed/duplex settings. */
        priv->flags |= (IFX_FLAG_ADV_10HALF     \
                        | IFX_FLAG_ADV_10FULL   \
                        | IFX_FLAG_ADV_100HALF  \
                        | IFX_FLAG_ADV_100FULL);

    /* By default, auto-negotiate PAUSE. */
        priv->flags |= IFX_FLAG_PAUSE_AUTO;
        spin_lock_init(&priv->lock);
        //err = register_netdev(ifx_eth_dev[i]);
        err = ifx_eth_fw_register_netdev(ifx_eth_dev[i]);
        if ( err ) {
            printk(KERN_ERR "%s[%d]: Register with network device failed!!!\n", __func__,__LINE__);
            goto err_out_free_res;
        }
    }

    if (eth_proc_create() == IFX_ERROR )
        goto err_out_free_res;
    /* Print the driver version info */
    eth_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    return  0;

err_out_free_res:
    /*Unregister with DMA core driver */
    dma_setup_uninit();
    /* unregister the network devices */
    for (i=0; i< NUM_ETH_INF ; i++) {
        if(ifx_eth_dev[i])
//            free_netdev(ifx_eth_dev[i]);
            ifx_eth_fw_free_netdev(ifx_eth_dev[i], i);
    }
    return err;
}

/** Clean up ETH moule */
void  ifx_eth_module_exit (void)
{
    int i;

#if defined (CONFIG_RED_MII_MAC_MODE) && defined(CONFIG_AMAZON_SE)
    /* free the GPIO's */
    amazon_se_eth_gpio_configure(0);
#endif
    /* unregister the network devices */
    for (i=0; i< NUM_ETH_INF ; i++) {
        //unregister_netdev(ifx_eth_dev[i]);
        //free_netdev(ifx_eth_dev[i]);
        ifx_eth_fw_unregister_netdev(ifx_eth_dev[i], i);
        ifx_eth_fw_free_netdev(ifx_eth_dev[i], i);
    }
    /*Unregister with DMA core driver */
    dma_setup_uninit();
    /** remove of the proc entries */
    eth_proc_delete();
}

module_init(ifx_eth_module_init);
module_exit(ifx_eth_module_exit);

MODULE_AUTHOR("Reddy Mallikarjuna");
MODULE_DESCRIPTION("IFX ethernet driver (Supported Danube/Amazon-SE)");
MODULE_LICENSE("GPL");
MODULE_VERSION(IFX_DRV_MODULE_VERSION);

