/******************************************************************************
**
** FILE NAME    : ifxmips_3port_eth_sw.c
** PROJECT      : IFX UEIP
** MODULES      : IFX ethenret device driver
** DATE         : 30 July  2009
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
** 30 July 2009         Reddy Mallikarjuna      Initial UEIP release
*******************************************************************************/

/*!
  \file ifxmips_3port_eth_sw.c
  \ingroup IFX_ETH_DRV
  \brief ETH driver source file for ARx.
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
#if defined(CONFIG_AR9)
#include "ifxmips_3port_sw_reg.h"
#include "ifxmips_3port_eth_sw.h"
#endif /*CONFIG_AR9*/

#define IFX_DRV_MODULE_NAME             "ifxmips_3port_sw"
#define IFX_DRV_MODULE_VERSION          "1.0.7"
static char version[] __devinitdata =
        IFX_DRV_MODULE_NAME ".c:v" IFX_DRV_MODULE_VERSION " \n";

/* length of time before we decide the hardware is borked,
 * and dev->tx_timeout() should be called to fix the problem
 */
#define IFX_TX_TIMEOUT                  (10 * HZ)

#define DMA_TX_BURST_LEN                DMA_BURSTL_4DW
#define DMA_RX_BURST_LEN                DMA_BURSTL_4DW
#define ETH_PKT_BUF_SIZE                1568
/* pointer to the dma device structure */
struct dma_device_info *g_dma_device=NULL;
/*proc file directory*/
static struct proc_dir_entry*   ifx_eth_dir;
/* Pointer to netword devices structure */
static struct net_device *ifx_eth_dev[NUM_ETH_INF ];
/** PMAC header structure */
cpu_egress_pkt_header_t eg_pkt_hdr;
cpu_ingress_pkt_header_t ig_pkt_hdr;
static int g_pmac_dma,g_dma_pmac;
static int eth0_up;
int eth1_up;
static int ifx_eth_open(struct net_device *dev);
static int ifx_eth_close(struct net_device *dev);
/** Transmit packet from Tx Queue to MAC */
static int ifx_start_xmit (struct sk_buff *skb, struct net_device *dev);
/** Hardware specific IOCTL's  */
static int ifx_eth_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
/** Get the network device statistics */
static struct net_device_stats *ifx_get_stats (struct net_device *dev);
/** change MTU values */
static int ifx_change_mtu (struct net_device *dev, int new_mtu);
/** Set mac address*/
static int ifx_set_mac_address(struct net_device *dev, void *p);
/*Ethernet transmit timeout*/
static void ifx_tx_timeout (struct net_device *dev);
/* select the tx dma channel */
static int ifx_select_tx_chan (struct sk_buff *skb, struct net_device *dev);
/*open dma rx channel*/
static void enable_dma_channel(void);
/*close the dma rx channel*/
static void disable_dma_channel(void);
/** Init of the network device */
static int switch_init(struct net_device *dev);
/* Get the ether addr from u-boot */
static unsigned char my_ethaddr[MAX_ADDR_LEN];

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
    int dev_index = (!strcmp(dev->name, "eth0") ? 0 : 1);
    unsigned int  port_status;

    cmd->supported = (SUPPORTED_Autoneg | SUPPORTED_TP);
    cmd->supported |= (SUPPORTED_100baseT_Half |    \
                    SUPPORTED_100baseT_Full |       \
                    SUPPORTED_10baseT_Half |        \
                    SUPPORTED_10baseT_Full |        \
                    SUPPORTED_1000baseT_Half |      \
                    SUPPORTED_1000baseT_Full |      \
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
    if (priv->flags & IFX_FLAG_ADV_1000HALF)
        cmd->advertising |= ADVERTISED_1000baseT_Half;
    if (priv->flags & IFX_FLAG_ADV_1000FULL)
        cmd->advertising |= ADVERTISED_1000baseT_Full;

    cmd->advertising |= ADVERTISED_Pause | ADVERTISED_Asym_Pause;

    port_status = IFX_REG_R32(IFX_AR9_ETH_PS);
    switch( (port_status>> (1+(dev_index*0x8)) ) & 0x3) {
        case 0:
            priv->current_speed=SPEED_10;
            break;
        case 1:
            priv->current_speed=SPEED_100;
            break;
        case 2:
            priv->current_speed=SPEED_1000;
            break;
        default: /* should not be here */
            printk(KERN_ERR "%s[%d]: Port status error(%d)!!!\n", __func__,__LINE__,port_status);
    }
    cmd->speed = priv->current_speed;
    if ( (port_status  >> (3+(dev_index *0x8))) & 0x1)
        priv->full_duplex=DUPLEX_FULL;
    else
        priv->full_duplex=DUPLEX_HALF;
    cmd->duplex = priv->full_duplex;

    if ( (port_status >> (8*dev_index)) & 0x1 )
        cmd->reserved[0]=0x1;  /*link up bit:0*/
    else
        cmd->reserved[0]=0x0;  /*link down */
    if (port_status & 0x1000 )
        cmd->reserved[0] |=0x2; /*flow control enable Bit1:1*/
    else
        cmd->reserved[0] |= 0x0; /*flow control disable*/
    if (dev_index)
        cmd->phy_address = PHY1_ADDR;
    else
        cmd->phy_address = PHY0_ADDR;
    cmd->port = PORT_MII;

    cmd->transceiver = (priv->flags & IFX_FLAG_INTERNAL_PHY) ? XCVR_INTERNAL : XCVR_EXTERNAL;
    return 0;
}

/** Set the network device settings */
static int ifx_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev);
    unsigned int  reg;
    int dev_index = (!strcmp(dev->name, "eth0") ? 0 : 1);

    if (cmd->autoneg == AUTONEG_ENABLE) {
        /*TODO*/
        return 0;
    } else if ((cmd->speed != SPEED_100 && cmd->speed != SPEED_10 && cmd->speed != SPEED_1000) \
            ||(cmd->duplex != DUPLEX_HALF && cmd->duplex != DUPLEX_FULL)) {
            return -EINVAL;
    }
    spin_lock_irq(&priv->lock);
    if (cmd->autoneg != AUTONEG_ENABLE) {
        reg = IFX_REG_R32(IFX_AR9_ETH_RGMII_CTL);
        reg &= ~RGMII_CTL_SPD_MASK(dev_index);
        reg &= ~RGMII_CTL_DUP_MASK(dev_index);
        priv->flags |= IFX_FLAG_FORCE_LINK;
        priv->flags &= ~(IFX_FLAG_100_BASE_T | IFX_FLAG_1000_BASE_T|IFX_FLAG_FULL_DUPLEX);
        if (cmd->speed == SPEED_100) {
            priv->flags |= IFX_FLAG_100_BASE_T;
            reg |= RGMII_CTL_SPD_100_SET(dev_index);
        } else if(cmd->speed == SPEED_1000) {
            reg |= RGMII_CTL_SPD_1000_SET(dev_index);
            priv->flags |= IFX_FLAG_1000_BASE_T;
        }
        if (cmd->duplex == DUPLEX_FULL) {
            priv->flags |= IFX_FLAG_FULL_DUPLEX;
            reg |= RGMII_CTL_DUP_SET(dev_index, 1);
        }
        IFX_REG_W32(reg, IFX_AR9_ETH_RGMII_CTL);
        if (dev_index) {
            reg = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
            reg |= (PX_CTL_FLP | PX_CTL_BYPASS);
            IFX_REG_W32(reg, IFX_AR9_ETH_P1_CTL);
        } else {
            reg = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
            reg |= (PX_CTL_FLP | PX_CTL_BYPASS);
            IFX_REG_W32(reg, IFX_AR9_ETH_P1_CTL);
        }
    }
    spin_unlock_irq(&priv->lock);
    return 0;
}

/** Reset the device */
static int ifx_nway_reset(struct net_device *dev)
{
     /*TODO*/
#if defined (CONFIG_MII1_AUTONEG) && CONFIG_MII1_AUTONEG
/*Reset GPHY, which is connected to extern GPHY chip*/
    int dev_index = (!strcmp(dev->name, "eth0") ? 0 : 1);
    if (dev_index)
        reset_ext_gphy(GPIO_RESET_GPHY,1);
#endif /* CONFIG_MII1_AUTONEG */
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

/** Turn On RX DMA channels */
static void enable_dma_channel(void)
{
    struct dma_device_info* dma_dev=g_dma_device;
    int i;

    for(i=0; i<dma_dev->max_rx_chan_num; i++) {
        if ( (dma_dev->rx_chan[i])->control==IFX_DMA_CH_ON )
            dma_dev->rx_chan[i]->open(dma_dev->rx_chan[i]);
    }
}

/** Turn Off RX DMA channels */
static void disable_dma_channel()
{
    struct dma_device_info* dma_dev=g_dma_device;
    int i;
    for (i=0; i<dma_dev->max_rx_chan_num; i++)
        dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);
}

static int index=0 ;
/** open the network device interface*/
static int ifx_eth_open(struct net_device *dev)
{
    unsigned int reg_val;
    if(index == 0) {
        enable_dma_channel();
    }
    index++;
    if(strcmp(dev->name, "eth0") == 0) {
        reg_val = IFX_REG_R32(IFX_AR9_ETH_P0_CTL);
        reg_val &= ~(PX_CTL_FLD);
        reg_val |= PX_CTL_FLP;
        IFX_REG_W32(reg_val, IFX_AR9_ETH_P0_CTL);
        eth0_up = 1;
    }
    if (g_pmac_dma) {
        if(strcmp(dev->name, "eth1") == 0) {
            reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
            reg_val &= ~(PX_CTL_FLD);
            reg_val |= PX_CTL_FLP;
            IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
            eth1_up = 1;
        }
    } else {
        reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
        reg_val &= ~(PX_CTL_FLD);
        reg_val |= PX_CTL_FLP;
        IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
    }
//    netif_start_queue(dev);
    return IFX_SUCCESS;
}

/** Close the network device interface*/
static int ifx_eth_close(struct net_device *dev)
{
    unsigned int reg_val;
    if(index) index--;
	if(index == 0){
	    disable_dma_channel();
	}
	if(strcmp(dev->name, "eth0") == 0) {
        reg_val = IFX_REG_R32(IFX_AR9_ETH_P0_CTL);
        reg_val &= ~(PX_CTL_FLP);
        reg_val |= PX_CTL_FLD;
        IFX_REG_W32(reg_val, IFX_AR9_ETH_P0_CTL);
        eth0_up = 0;
    }
    if (g_pmac_dma) {
        if(strcmp(dev->name, "eth1") == 0) {
            reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
            reg_val &= ~(PX_CTL_FLP);
            reg_val |= PX_CTL_FLD;
            IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
            eth1_up = 0;
        }
    } else {
        reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
        reg_val &= ~(PX_CTL_FLP);
        reg_val |= PX_CTL_FLD;
        IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
    }
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

/**
* This function is called in dma intr handler (DMA RCV_INT interrupt).
* This function get the data from the DMA device.
*   if the packet is valid then it will send to upper layer based on  criteria.
*       The switch CPU port PMAC status header is enabled, then remove the header and
*           look from which port the packet comes and send to relative network device.
        if PMAC status header is not enabled, then send the packets eth0 interafce
*/
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
    if(g_pmac_dma){
        int sourcePortId;;
        len -= 8;  /*Remove PMAC to DMA header */
        skb_pull(skb,8);
        eg_pkt_hdr = * ((cpu_egress_pkt_header_t *) (buf+2));
        sourcePortId =(eg_pkt_hdr.srcPortID) & 0x7 ;
        if(sourcePortId < NUM_ETH_INF) {
            switch(sourcePortId) {
                case 0:
                    dev = ifx_eth_dev[0];
                    break;
                case 1:
                    dev = ifx_eth_dev[1];
                    break;
                case 2:
                    dev = ifx_eth_dev[2];
                    break;
                case 3:
                    dev = ifx_eth_dev[3];
                    break;
                case 4:
                    dev = ifx_eth_dev[4];
                    break;
                case 5:
                    dev = ifx_eth_dev[5];
                    break;
                default:
                goto rx_err_exit;
            }
        } else {
            goto rx_err_exit;
        }
    } else
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
    IFX_REG_W32(val, IFX_AR9_ETH_PMAC_SA2);
    val  = ((u32) data[0]) << 16;
    val |= ((u32) data[1]) << 24;
    IFX_REG_W32(val, IFX_AR9_ETH_PMAC_SA1);
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
    /** TODO: is it required to set MAC address in the HW registers?*/
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

/* Select the DMA channel numbers, refer dma_setup_init function */
static int ifx_select_tx_chan (struct sk_buff *skb, struct net_device *dev)
{
    int chan_no;
/*TODO: select the channel based on  criteria*/
    int dev_index = (!strcmp(dev->name, "eth0") ? 0 : 1);
    if (dev_index)
        chan_no = 1;
    else
        chan_no = 0;
    return chan_no;
}

/**
* Transmit packet over DMA, which comes from the Tx Queue
* Note: Tx packet based on the interface queue.
*       if packet comes from eth0, then sendout the packet over Tx DMA channel 0
*       if packet comes from eth1, then sendout the packet over Tx DMA channel 1
* refer the function "ifx_select_tx_chan" selection of dma channels
* if switch CPU port PMAC status header is enabled, then set the status header
*   based on criteria and push the status header infront of header.
* if head room is not availabe for status header(4-bytes), reallocate the head room
*   and push status header  infront of the header
*/
static int ifx_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    IFX_switch_priv_t *priv		= ifx_eth_fw_netdev_priv(dev);
    int len , rc = NETDEV_TX_OK;
    char *data;
    struct dma_device_info* dma_dev=g_dma_device;

    len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;

    if(g_dma_pmac) {
        memset((void *) &ig_pkt_hdr, 0, sizeof (ig_pkt_hdr));
        /*if DIRECT is set 1, then packet is directly forward to the physical port based on DPID (no learning),
		if DIRECT is set 0, then switch will learn and decide forward the packet to which physical port*/

#ifdef CONFIG_SW_ROUTING_MODE
        ig_pkt_hdr.DIRECT= 1;
#else
        ig_pkt_hdr.DIRECT= 0;
#endif
        if(!(strcmp(dev->name, "eth0"))) {
            ig_pkt_hdr.SPID= 2;  /*CPU port */
            ig_pkt_hdr.DPID= 0;  /*Packet send through MII0 interface, valid only when DIRECT is set 1 */
        } else if(!(strcmp(dev->name, "eth1"))) {
            ig_pkt_hdr.SPID= 2;  /*CPU port */
            ig_pkt_hdr.DPID= 1; /* Send packet through  MII1 interface, valid only when DIRECT is set 1 */
        } else {
            /*if eth1 interface is not there, then send through eth0 */
            ig_pkt_hdr.SPID= 2;
            ig_pkt_hdr.DPID= 0;  /*Packet send through MII0 interface, valid only when DIRECT is set 1 */
        }
        if(skb_headroom(skb)>=4) {
            skb_push(skb,4);
            memcpy(skb->data, (void*)&ig_pkt_hdr, 4);
            len+=4;
        } else {
            struct sk_buff *tmp = skb;
            skb = skb_realloc_headroom(tmp, 4);
            if(tmp)
                dev_kfree_skb_any(tmp);
            if (skb ==  NULL) {
                printk(KERN_ERR "%s[%d]: skb_realloc_headroom failed!!!\n", __func__,__LINE__);
                return -ENOMEM;
            }
            skb_push(skb, 4);
            memcpy(skb->data, &ig_pkt_hdr, 4);
            len += 4;
        }
    }

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
    unsigned int reg_val = IFX_REG_R32(IFX_AR9_ETH_1P_PRT);
    reg_val &= ~(3 << (pri * 2));
    reg_val |= ( cos_value << (pri * 2) );
    IFX_REG_W32(reg_val, IFX_AR9_ETH_1P_PRT);
}

/** Enable DSCP Class Of Service */
static void set_dscp_cos(_dscp_cos_req* dscp_cos_req)
{
    int dscp = dscp_cos_req->dscp;
    int cos_value = dscp_cos_req->cos_value;
    unsigned int reg_val = IFX_REG_R32(IFX_AR9_ETH_DFSRV_MAP0 +(dscp/16*4));
    reg_val &= ~(3 << (dscp%16*2));
    reg_val |= (cos_value << (dscp%16*2));
    IFX_REG_W32(reg_val, (IFX_AR9_ETH_DFSRV_MAP0 + (dscp/16*4)));
}

/** Platform specific IOCTL's handler */
static int ifx_eth_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
  /*  IFX_switch_priv_t *priv = ifx_eth_fw_netdev_priv(dev); */
    _vlan_cos_req etop_vlan_cos_req;
    _dscp_cos_req etop_dscp_cos_req;
    unsigned int reg_val;
    int err = -EINVAL;
    int dev_index = !(strcmp(dev->name, "eth0")) ? 0 : 1;
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
			if (dev_index) {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
			    reg_val |= PX_CTL_VPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
			} else {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P0_CTL);
			    reg_val |= PX_CTL_VPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P0_CTL);
			}
            break;
        case DISABLE_VLAN_COS:
            if (dev_index) {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
			    reg_val &= ~PX_CTL_VPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
			} else {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P0_CTL);
			    reg_val &= ~PX_CTL_VPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P0_CTL);
			}
            break;

        case ENABLE_DSCP_COS:
            if (dev_index) {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
			    reg_val |= PX_CTL_SPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
			} else {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P0_CTL);
			    reg_val |= PX_CTL_SPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P0_CTL);
			}
            break;
        case DISABLE_DSCP_COS:
           if (dev_index) {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P1_CTL);
			    reg_val &= ~PX_CTL_SPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P1_CTL);
			} else {
			    reg_val = IFX_REG_R32(IFX_AR9_ETH_P0_CTL);
			    reg_val &= ~PX_CTL_SPE;
			    IFX_REG_W32(reg_val, IFX_AR9_ETH_P0_CTL);
			}
            break;
        case PASS_UNICAST_PACKETS:
            reg_val = IFX_REG_R32(IFX_AR9_ETH_DF_PORTMAP_REG);
            reg_val |= DF_PORTMAP_UP(dev_index);
            IFX_REG_W32(reg_val, IFX_AR9_ETH_DF_PORTMAP_REG);
            break;
        case FILTER_UNICAST_PACKETS:
            reg_val = IFX_REG_R32(IFX_AR9_ETH_DF_PORTMAP_REG);
            reg_val &= ~DF_PORTMAP_UP(dev_index);
            IFX_REG_W32(reg_val, IFX_AR9_ETH_DF_PORTMAP_REG);
            break;
        case KEEP_BROADCAST_PACKETS:
            reg_val = IFX_REG_R32(IFX_AR9_ETH_DF_PORTMAP_REG);
            reg_val |= DF_PORTMAP_BP(dev_index);
            IFX_REG_W32(reg_val, IFX_AR9_ETH_DF_PORTMAP_REG);
            break;
        case DROP_BROADCAST_PACKETS:
            reg_val = IFX_REG_R32(IFX_AR9_ETH_DF_PORTMAP_REG);
            reg_val &= ~DF_PORTMAP_BP(dev_index);
            IFX_REG_W32(reg_val, IFX_AR9_ETH_DF_PORTMAP_REG);
            break;
        case KEEP_MULTICAST_PACKETS:
            reg_val = IFX_REG_R32(IFX_AR9_ETH_DF_PORTMAP_REG);
            reg_val |= DF_PORTMAP_MP(dev_index);
            IFX_REG_W32(reg_val, IFX_AR9_ETH_DF_PORTMAP_REG);
            break;
        case DROP_MULTICAST_PACKETS:
             reg_val = IFX_REG_R32(IFX_AR9_ETH_DF_PORTMAP_REG);
            reg_val &= ~DF_PORTMAP_MP(dev_index);
            IFX_REG_W32(reg_val, IFX_AR9_ETH_DF_PORTMAP_REG);
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
int sw_dma_intr_handler(struct dma_device_info* dma_dev,int status)
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
unsigned char* sw_dma_buffer_alloc(int len, int* byte_offset,void** opt)
{
    unsigned char *buffer=NULL;
    struct sk_buff *skb=NULL;
    int offset = 0;
    int dma_burst_len = g_dma_device->rx_burst_len <<2;
    *byte_offset = 2; /* for reserving 2 bytes in skb buffer, so, set offset 2 bytes infront of data pointer */
    skb = dev_alloc_skb(ETH_PKT_BUF_SIZE+dma_burst_len);
    if (skb == NULL) {
        printk(KERN_ERR "%s[%d]: Buffer allocation failed!!!\n", __func__,__LINE__);
        return NULL;
    }
    if(likely(skb)) {
        if(((u32)skb->data & (dma_burst_len -1)) != 0) {
            offset = ~((u32)skb->data+(dma_burst_len -1)) & (dma_burst_len -1);
        }
        if(offset != 0 ) {
            buffer = (unsigned char *)(skb->data+offset);
            skb_reserve(skb, offset);
        } else {
            buffer = (unsigned char*)(skb->data);
        }
        skb_reserve(skb, 2);
        *(int*)opt=(int)skb;
    }
    return buffer;
}

/* Free skb buffer
* This function frees a buffer previously allocated by the DMA buffer
*   alloc callback function. */
int sw_dma_buffer_free(unsigned char* dataptr,void* opt)
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

#ifdef CONFIG_NAPI_ENABLED
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
    g_dma_device->buffer_alloc              =&sw_dma_buffer_alloc;
    g_dma_device->buffer_free               =&sw_dma_buffer_free;
    g_dma_device->intr_handler              =&sw_dma_intr_handler;
    g_dma_device->num_rx_chan               = 4;
    g_dma_device->num_tx_chan               = 2;
    g_dma_device->tx_burst_len              = DMA_TX_BURST_LEN;
    g_dma_device->rx_burst_len              = DMA_RX_BURST_LEN;
    g_dma_device->tx_endianness_mode        = IFX_DMA_ENDIAN_TYPE3;
    g_dma_device->rx_endianness_mode        = IFX_DMA_ENDIAN_TYPE3;
    /*g_dma_device->port_packet_drop_enable   = IFX_DISABLE;*/

    for (i = 0; i < g_dma_device->num_rx_chan; i++) {
        g_dma_device->rx_chan[i]->packet_size =  ETH_PKT_BUF_SIZE;
        g_dma_device->rx_chan[i]->control = IFX_DMA_CH_ON;
    }
    for (i = 0; i < g_dma_device->num_tx_chan; i++) {
        /* TODO: Modify with application requirement
        * eth0 -->DMA Tx channel 0, eth1 --> DMA Tx channel 1
        */
        if ( (i == 0) || (i == 1))
            g_dma_device->tx_chan[i]->control = IFX_DMA_CH_ON;
        else
            g_dma_device->tx_chan[i]->control = IFX_DMA_CH_OFF;
    }
#ifdef CONFIG_NAPI_ENABLED
    g_dma_device->activate_poll = eth_activate_poll;
    g_dma_device->inactivate_poll = eth_inactivate_poll;
#endif
    ret = dma_device_register (g_dma_device);
    if ( ret != IFX_SUCCESS)
        printk(KERN_ERR "%s[%d]: Register with DMA core driver Failed!!!\n", __func__,__LINE__);

    return ret;
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
      	for (i = 0; i < 6; i++) {
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
    return sprintf(buf, "IFX 3 Port Switch Eth driver, version %s,(c)2009 Infineon Technologies AG\n", version);
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

/** Init Ethernet module */
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
    unsigned int reg;
    char ver_str[128] = {0};
    IFX_switch_priv_t* priv;
    g_pmac_dma  = 0;
    g_dma_pmac  = 0;
    eth0_up     = 0;
    eth1_up     = 0;
    /** Register with DM core driver */
    dma_setup_init();
#if defined(CONFIG_AR9)
    /* HW init of the Switch */
    ar9_3port_sw_hw_init();
#endif /* CONFIG_AR9*/
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
        priv->phy_addr = 0x00;
        /* By default, advertise supported  speed/duplex settings. */
        priv->flags |= (IFX_FLAG_ADV_10HALF         \
                        | IFX_FLAG_ADV_10FULL       \
                        | IFX_FLAG_ADV_100HALF      \
                        | IFX_FLAG_ADV_100FULL      \
                        | IFX_FLAG_ADV_1000HALF     \
                        | IFX_FLAG_ADV_1000FULL)    \
                        ;
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
    reg = IFX_REG_R32(IFX_AR9_ETH_PMAC_HD_CTL );
    if (reg  & PMAC_HD_CTL_AS )
        g_pmac_dma = 1;
    if (reg  & PMAC_HD_CTL_RXSH )
        g_dma_pmac = 1;

    if (eth_proc_create() == IFX_ERROR )
        goto err_out_free_res;
    /* Print the driver version info */
    eth_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    return  0;
err_out_free_res:
    /*Unregister with DMA core driver */
    dma_setup_uninit();
    for (i=0; i< NUM_ETH_INF ; i++) {
        /* unregister the network devices */
        if(ifx_eth_dev[i])
//            free_netdev(ifx_eth_dev[i]);
            ifx_eth_fw_free_netdev(ifx_eth_dev[i], i);
    }
    return err;
}


/**
* 3 port ethernet module cleanup
* Unregister with dma core driver and also network devices
*/
void  ifx_eth_module_exit (void)
{
    int i;
#if defined(CONFIG_AR9)
    /* Un init of the GPIO's and free_irq*/
    ar9_3port_sw_hw_uninit();
#endif /* CONFIG_AR9*/
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
MODULE_DESCRIPTION("IFX ethernet driver (Supported AR9)");
MODULE_LICENSE("GPL");
MODULE_VERSION(IFX_DRV_MODULE_VERSION);
