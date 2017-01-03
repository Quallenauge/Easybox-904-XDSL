/**
** FILE NAME    : ifxmips_eth.h
** PROJECT      : IFX UEIP
** MODULES      : ETH module 
** DATE         : 24 July 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : IFX Cross-Platform ETH driver header file
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
** $Date                        $Author                 $Comment
** 24 July 2009         Reddy Mallikarjuna              Initial release
*******************************************************************************/

#ifndef _IFXMIPS_ETH_H_
#define _IFXMIPS_ETH_H_

/*!
  \defgroup IFX_ETH_DRV UEIP Project - ETH driver
  \brief UEIP Project - ETH driver module, supports IFX CPE platforms(Danube/ASE/ARx/VRx).
*/

/*!
  \defgroup IFX_ETH_DRV_API Externl APIs
  \ingroup IFX_ETH_DRV
  \brief Externl APIs for other modules.
*/

/*!
  \defgroup IFX_ETH_DRV_STRUCTURE Driver Structures
  \ingroup IFX_ETH_DRV
  \brief Definitions/Structures of ETH module.
*/

/*!
  \file ifxmips_eth.h
  \ingroup IFX_ETH_DRV
  \brief Header file for IFX ETH driver internal definitions
*/

/* Interface mode */
/** MII/Rev MII */
/* MII MAC mode*/
#define MII_MAC_MODE                0x0001
/* MII internl PHY mode */
#define MII_PHY_MODE                0x0002
#define EPHY_MODE                   MII_PHY_MODE
/* Reverse MII MAC mode */
#define REV_MII_MAC_MODE            0x0004
/** Turbo Rev Mii MAC mode */
#define TURBO_REV_MAC_MII_MODE      0x0008
/** Reduced MII mode */
#define RED_MII_PHY_MODE            0x0010
#define RMII_PHY_MODE               RED_MII_PHY_MODE
/** Reduce MII MAC mode with Input ref clock */
#define RED_MII_MAC_MODE            0x0020
#define RMII_MAC_MODE               RED_MII_MAC_MODE
/** Reduce MII MAC mode with Output Ref clock */
#define RED_MII_MAC_MODE_OC         0x0040
#define RED_MII_MODE                RMII_MAC_MODE
/** RGMII MAC mode */
#define RGMII_MODE                  0x0100
/* RGMII MAC with 100MB mode */
#define RGMII_MODE_100MB            0x0200
/*GMII MAC mode */
#define GMII_MAC_MODE               0x0400

/* hardware minimum and maximum for a single frame's data payload */
/* Minimum MTU value */
#define IFX_MIN_MTU                 64
/*Max MTU value*/
#define IFX_MAX_MTU                 1500
/* Eth Packet buffer size to allocate for DMA descriptor*/
#define IFXRX_PKT_BUF_SZ           (1536 +  64)
#define IFX_TX_PKT_BUF_SZ           (IFX_MAX_MTU + ETH_HLEN + 8)

/** ioctl commands*/
/* Set VLAN CoS value */
#define SET_VLAN_COS                SIOCDEVPRIVATE
/* Set DSCP CoS value*/
#define SET_DSCP_COS                SIOCDEVPRIVATE+1
/* Enable VLAN CoS */
#define ENABLE_VLAN_COS             SIOCDEVPRIVATE+2
/* Disable VLAN CoS */
#define DISABLE_VLAN_COS            SIOCDEVPRIVATE+3
/* Select first VLAN Class */
#define VLAN_CLASS_FIRST            SIOCDEVPRIVATE+4
/* Select second VLAN class if device supports*/
#define VLAN_CLASS_SECOND           SIOCDEVPRIVATE+5
/* Enable DSCP CoS */
#define ENABLE_DSCP_COS             SIOCDEVPRIVATE+6
/*Disable DSCP CoS */
#define DISABLE_DSCP_COS            SIOCDEVPRIVATE+7
/* Enable Unicast packet */
#define PASS_UNICAST_PACKETS        SIOCDEVPRIVATE+8
/* Disable Unicase packet */
#define FILTER_UNICAST_PACKETS      SIOCDEVPRIVATE+9
/* Enable broadcast packet */
#define KEEP_BROADCAST_PACKETS      SIOCDEVPRIVATE+10
/* Disable broadcast packet */
#define DROP_BROADCAST_PACKETS      SIOCDEVPRIVATE+11
/* Enable Multicast packet */
#define KEEP_MULTICAST_PACKETS      SIOCDEVPRIVATE+12
/* Disable Multicast packet */
#define DROP_MULTICAST_PACKETS      SIOCDEVPRIVATE+13

/*!
  \addtogroup IFX_ETH_DRV_STRUCTURE
 */
/*@{*/

/*! \typedef  _vlan_cos_req
* \brief VLAN CoS structure

    This structure is used to set the VLAN CoS values
*/
typedef struct vlan_cos_req {
    int                 port; /*!< port number*/
    int                 pri;  /*!< priority */
    int                 cos_value;  /*!< class of service value*/
}_vlan_cos_req;

/*! \typedef  _dscp_cos_req
* \brief DSCP CoS structure

    This structure is used to set the DSCP CoS values
*/
typedef struct dscp_cos_req {
    int                 dscp;   /*!< dscp number*/
    int                 cos_value;  /*!< class of service value*/
}_dscp_cos_req;

/*! \typedef  IFX_switch_priv_t
* \brief Ethernet private data structure

    This structure is used internal purpose
*/
typedef struct switch_priv {
    struct net_device_stats     stats; /*!< network device interface Statistics */
    struct dma_device_info      *dma_device; /*!< structure of dma device information */
    struct sk_buff              *skb; /*!< skb buffer structure*/
    spinlock_t                  lock;	/*!< spin lock */
    int                         phy_addr; /*!< interface mdio phy address*/
    int                         current_speed; /*!< interface current speed*/
    int                         full_duplex; /*!< duplex mode*/
    int                         current_duplex; /*!< current interface duplex mode*/
    void __iomem                *base_addr; /*!< Base address */
    unsigned int                flags;  /*!< flags */
#define IFX_FLAG_PAUSE_AUTO         0x00000001
#define IFX_FLAG_FULL_DUPLEX        0x00000002
#define IFX_FLAG_10_BASE_T          0x00000010
#define IFX_FLAG_100_BASE_T         0x00000020
#define IFX_FLAG_1000_BASE_T        0x00000040
#define IFX_FLAG_TX_PAUSE           0x00000100
#define IFX_FLAG_RX_PAUSE           0x00000200
#define IFX_FLAG_FORCE_LINK         0x00000400
#define IFX_FLAG_ADV_10HALF         0x00001000
#define IFX_FLAG_ADV_10FULL         0x00002000
#define IFX_FLAG_ADV_100HALF        0x00004000
#define IFX_FLAG_ADV_100FULL        0x08008000
#define IFX_FLAG_ADV_1000HALF       0x00010000
#define IFX_FLAG_ADV_1000FULL       0x00020000
#define IFX_FLAG_INTERNAL_PHY       0x00100000
}IFX_switch_priv_t;
/* @} */
#endif /* _IFXMIPS_ETH_H_*/
