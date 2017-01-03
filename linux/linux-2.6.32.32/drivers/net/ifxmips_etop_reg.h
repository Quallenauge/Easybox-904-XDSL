/**
** FILE NAME    : ifxmips_etop_reg.h
** PROJECT      : IFX UEIP
** MODULES      : ETH module 
** DATE         : 23 July 2009
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
** 23 July 2009         Reddy Mallikarjuna              Initial release
*******************************************************************************/

#ifndef _IFXMIPS_ETOP_REG_H_
#define _IFXMIPS_ETOP_REG_H_

/*!
  \file ifxmips_etop_reg.h
  \ingroup IFX_ETH_DRV
  \brief IFX Eth module register definition for Amazon-SE/DANUBE platforms
*/
/** ==========================  */
/* Include files                */
/** =========================== */
#include <asm/ifx/ifx_regs.h>

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#define ETOP_WRITE_REG32(data,addr)         IFX_REG_W32((data), (volatile unsigned  int *)addr)
#define ETOP_READ_REG32(addr)               IFX_REG_R32((volatile unsigned  int *)addr)

/** ETOP register address and bits*/
#define ETOP_PPE32_BASE_ADDR                (0xBE180000)
/** PP32 Debug Control register */
#define PPE32_DBB_CTRL_REG                  (ETOP_PPE32_BASE_ADDR)
/** Start of PP32(0B-->writing ‘0’ has no effect, 1B-->starts PP32;  */
#define DBG_CTRL_START                      0x00000001
/** Stop of PP32 (0B--> no effect, 1B-->stops PP32; */
#define DBG_CTRL_STOP                       0x00000002
/** PP32 data mem reg base address*/
#define DATA_MEM_MAP_REG_BASE               (ETOP_PPE32_BASE_ADDR + (0x4000 * 4))
/** ETOP MDIO Configuration */
#define ETOP_MDIO_CFG_REG                   (DATA_MEM_MAP_REG_BASE + (0x0600 * 4))
/**  EMAC 0 Use MDIO Mode. Configures the MDIO auto-sensing mode. */
#define MDIO_CFG_UMM0                       0x00000002
/**  EMAC 1 Use MDIO Mode. Configures the MDIO auto-sensing mode. */
#define MDIO_CFG_UMM1                       0x00000004
/** Configures the MDIO PHY address. ( Used for MDIO auto-detection state machine only) */
#define MDIO_CFG_PHYA0_SET(arg)             ((arg & 0x1F) << 0x3 )
#define MDIO_CFG_PHYA1_SET(arg)             ((arg & 0x1F) << 0x8 )
#define MDIO_CFG_PHYA0_GET(arg)             ((arg >> 0x3) & 0x1F)
#define MDIO_CFG_PHYA1_GET(arg)             ((arg >> 0x8) & 0x1F) 
/** Resets the MDIO Auto-detection state machine.(bit is self clearing)*/
#define MDIO_CFG_SMRST                      0x00002000
/** ETOP MDIO Access */
#define ETOP_MDIO_ACC_REG                   (DATA_MEM_MAP_REG_BASE + (0x0601 * 4))
/* 16 bit data to be read from/written to selected register */
#define MDIO_ACC_PHYD(arg)                  ((arg) & 0xFFFF)
/** Address of register accessed within selected PHY. */
#define MDIO_ACC_REGA_SET(arg)              ((arg&0x1F) << 16)
#define MDIO_ACC_REGA_GET(arg)              ((arg >> 16) &0x1F)
/** Access PHY Address. */
#define MDIO_ACC_PHYA_SET(arg)              ((arg&0x1F) << 21)
#define MDIO_ACC_PHYA_GET(arg)              ((arg >> 31) &0x1F)
/** Read/Write (1B RD, Read Access, 0B WR, Write access)*/
#define MDIO_ACC_RD                         (0x1 << 30)
#define MDIO_ACC_WR                         (0x0 << 30)
/** Request/Acknowledge CPU writes 1 to make a new access request.
MDIO hardware resets RA bit once access is completed.*/
#define MDIO_ACC_RA                         0x80000000
/** ETOP CFG Configuration Register*/
#define ETOP_CFG_REG                        (DATA_MEM_MAP_REG_BASE + (0x0602 * 4))
/* ENET 0 MII/Reverse MII Off (specify the MII/Reverse MII working mode or disable mode.
* 0B WORKING, interface is in working mode;
* 1B DISABLE, interface is in tristate mode */
#define CFG_OFF0                            0x00000001
/** ENET 0 MII/Reverse MII Mode (0B --> MII mode, 1B --> Reverse MII mode) */
#define CFG_REMII0                          0x00000002
/** ENET 0 TURBO Mode (0B --> Normal MII/Reverse MII mode, 
* 1B --> Turbo MII/Reverse MII mode, the MII clock is doubled)*/
#define CFG_TURBO0                          0x00000004
/* ENET 1 MII/Reverse MII Off (specify the MII/Reverse MII working mode or disable mode.*
* 0B WORKING, interface is in working mode;
* 1B DISABLE, interface is in tristate mode */
#define CFG_OFF1                            0x00000008
/** ENET 1 MII/Reverse MII Mode (0B --> MII mode, 1B --> Reverse MII mode) */
#define CFG_REMII1                          0x00000010
/** ENET 1 TURBO Mode (0B --> Normal MII/Reverse MII mode,
* 1B --> Turbo MII/Reverse MII mode, the MII clock is doubled)*/
#define CFG_TURBO1                          0x00000020
/** ENET 0 Store Enable (0B Disable, ENET 0 does not store the incoming bytes to buffer;
* 1B Enable, ENET0 store the incoming bytes to buffer) */
#define CFG_SEN0                            0x00000040
/** ENET 1 Store Enable (0B Disable, ENET 1 does not store the incoming bytes to buffer;
* 1B Enable, ENET1 store the incoming bytes to buffer) */
#define CFG_SEN1                            0x00000080
/** ENET 0 Fetch Enable (0B Disable, ENET 0 does not fetch bytes from buffer,
* 1B Enable, ENET 0 fetches bytes from buffer) */
#define CFG_FEN0                            0x00000100
/** ENET 1 Fetch Enable (0B Disable, ENET 1 does not fetch bytes from buffer,
* 1B Enable, ENET 1 fetches bytes from buffer) */
#define CFG_FEN1                            0x00000200
/** ENET 0 TX CLK Inversion (0B Disable, ENET 0 TX CLK is not inverted before being used internally,
* 1B Enable, ENET 0 TX CLK is inverted before being used internally) */
#define CFG_TCKINV0                         0x00000400
/** ENET 1 TX CLK Inversion (0B Disable, ENET 1 TX CLK is not inverted before being used internally,
* 1B Enable, ENET 1 TX CLK is inverted before being used internally) */
#define CFG_TCKINV1                         0x00000800
/* ENET 0 Reduced MII Mode (not Supported Danube) 
* 0B interface is in not in RMII mode;
* 1B RMII, interface is in Reduced MII mode; */
#define CFG_RMII0                           0x00001000
/*This field is used to configure ENET 0 RX CLK Inversion. 
* 0B Disable, ENET 0 RX CLK is not inverted before being used internally;
* 1B Enable, ENET 0 RX CLK is inverted before being used internally; */
#define CFG_RXCKINV0                        0x00002000
/* ENET PHY Connection Enable (0B Disable, ENET 0 to internal PHY connection is not there;
* 1B Enable, ENET 0 to internal PHY connection is enabled; */
#define CFG_INT_PHY                         0x00004000
/* ENET 0 RMII CLK Source (0B--> RMII CLK is from CGU, 1B -->RMII CLK is from external */
#define CFG_RMIIC_S                         0x00008000
/**ETOP IG VLAN priority CoS Mapping register*/
#define ETOP_IG_VLAN_COS_REG                (DATA_MEM_MAP_REG_BASE + (0x0603 * 4))
#define ETOP_IG_DSCP_COS3_REG               (DATA_MEM_MAP_REG_BASE + (0x0604 * 4))
#define ETOP_IG_DSCP_COS2_REG               (DATA_MEM_MAP_REG_BASE + (0x0605 * 4))
#define ETOP_IG_DSCP_COS1_REG               (DATA_MEM_MAP_REG_BASE + (0x0606 * 4))
#define ETOP_IG_DSCP_COS0_REG               (DATA_MEM_MAP_REG_BASE + (0x0607 * 4))
#define ETOP_IG_PLEN_CTRL_REG               (DATA_MEM_MAP_REG_BASE + (0x0608 * 4))
/** ENETS Oversize Frame Length Threshold 
* The length of MAC client data greater than this value is considered as oversize frame. 
* (DA, SA, TYPE/Length Field, Tag control field and CRC are not included) */
#define IG_PLEN_CTRL_OVER(arg)              ((arg & 0x3FFF))
/** ENETS Undersize Frame Length Threshold */
#define IG_PLEN_CTRL_UNDER_SET(arg)         ((arg & 0x7F) << 16 )
#define IG_PLEN_CTRL_UNDER_GET(arg)         ((arg >> 16) & 0x7F)
/** ETOP VLAN PROT ID Register*/
#define ETOP_VPID_REG                       (DATA_MEM_MAP_REG_BASE + (0x060C * 4))
#define ETOP_ISR_REG                        (DATA_MEM_MAP_REG_BASE + (0x060A * 4))
#define ETOP_IER_REG                        (DATA_MEM_MAP_REG_BASE + (0x060B * 4))
/** ENET0 MAC Configuration Register*/
#define ENET0_MAC_CFG_REG                    (DATA_MEM_MAP_REG_BASE + (0x0610 * 4))
/** Link Status (0B --> link is not ok, 1B-->link is ok */
#define MAC_CFG_LINK_MASK                   0x00000001
#define MAC_CFG_LINK(arg)                   (arg & 0x1)
/** Link Speed (0B --> set to 10 Mbit/s,  1B --> set to 100 Mbit/s.) */
#define MAC_CFG_SPEED_MASK                   0x00000002
#define MAC_CFG_SPEED_GET(arg)              ((arg >> 1) & 0x1)
#define MAC_CFG_SPEED_SET(arg)              ((arg & 1) << 1)
/** Link Duplex Mode (0B --> half duplex mode, 1B-->full duplex mode) */
#define MAC_CFG_DUPLEX_MASK                 0x00000004
#define MAC_CFG_DUPLEX_GET(arg)             ((arg >> 2) & 0x1)
#define MAC_CFG_DUPLEX_SET(arg)             ((arg & 1) << 2)
/** Egress Pause Enable (0B --> no pause frame is generated, 1B --> pause frame can be generated) */
#define MAC_CFG_EPAUS_GET(arg)              ((arg >> 3) & 0x1)
#define MAC_CFG_EPAUS_SET(arg)              ((arg & 1) << 3)
/** Ingress Pause Enable (0B --> received pause frame can not be identified,
* 1B --> received pause frame can be identified and control the egress flow) */
#define MAC_CFG_IPAUS_GET(arg)              ((arg >> 4) & 0x1)
#define MAC_CFG_IPAUS_SET(arg)              ((arg & 1) << 4)
/** CRC Generation (0B --> no CRC is generated and inserted,
* 1B-->CRC is generated and inserted for every frame) */
#define MAC_CFG_CGEN_GET(arg)               ((arg >> 11) & 0x1)
#define MAC_CFG_CGEN_SET(arg)               ((arg & 1) << 11)
/** Backpressure Control (0B --> no backpressure need to be asserted,
* 1B -->backpressure need to be asserted) */
#define MAC_CFG_BP_GET(arg)                 ((arg >> 12) & 0x1)
#define MAC_CFG_BP_SET(arg)                 ((arg & 1) << 12)
/** ENETS0 Ingress Configuration Register */
#define ENETS0_CFG_REG                      (DATA_MEM_MAP_REG_BASE + (0x0614 * 4))
/** ENETS Pause Frame Forwarding Enable */
#define ENETS0_CFG_PAUSEFWD                 0x80000000
/** ENETS Packet Sanity Check Disable */
#define ENETS0_CFG_CHECKDIS                 0x40000000
/** ENETS VLAN Selection */
#define ENETS0_CFG_VL2                      0x20000000
/** ENETS Filter Unicast Packets */
#define ENETS0_CFG_FTUC                     0x10000000
/** ENETS DROP Broadcast Packets */
#define ENETS0_CFG_DPBC                     0x08000000
/** ENETS DROP Multicast Packets */
#define ENETS0_CFG_DPMC                     0x04000000
#define ENETS0_PGCNT_REG                    (DATA_MEM_MAP_REG_BASE + (0x0615 * 4))
#define ENETS0_PKTCNT_REG                   (DATA_MEM_MAP_REG_BASE + (0x0616 * 4))
#define ENETS0_BUF_CTRL_REG                 (DATA_MEM_MAP_REG_BASE + (0x0617 * 4))
#define ENETS0_COS_CFG_REG                  (DATA_MEM_MAP_REG_BASE + (0x0618 * 4))
#define COS_CFG_VLAN                        0x00000002
#define COS_CFG_DSCP                        0x00000001
#define ENETS0_IGDROP_REG                   (DATA_MEM_MAP_REG_BASE + (0x0619 * 4))
#define ENETS0_IGERR_REG                    (DATA_MEM_MAP_REG_BASE + (0x061A * 4))
#define ENETS0_MAC_DA0_REG                  (DATA_MEM_MAP_REG_BASE + (0x061B * 4))
#define ENETS0_MAC_DA1_REG                  (DATA_MEM_MAP_REG_BASE + (0x061C * 4))
#define ENETF0_CFG_REG                      (DATA_MEM_MAP_REG_BASE + (0x0632 * 4))
#define ENETF0_PGCNT_REG                    (DATA_MEM_MAP_REG_BASE + (0x0633 * 4))
#define ENETF0_PKTCNT_REG                   (DATA_MEM_MAP_REG_BASE + (0x0634 * 4))
#define ENETF0_HFCTRL_REG                   (DATA_MEM_MAP_REG_BASE + (0x0635 * 4))
#define ENETF_TXCTRL_REG                    (DATA_MEM_MAP_REG_BASE + (0x0636 * 4))
#define ENETF0_VLCOS0_REG                   (DATA_MEM_MAP_REG_BASE + (0x0638 * 4))
#define ENETF0_VLCOS1_REG                   (DATA_MEM_MAP_REG_BASE + (0x0639 * 4))
#define ENETF0_VLCOS2_REG                   (DATA_MEM_MAP_REG_BASE + (0x063A * 4))
#define ENETF0_VLCOS3_REG                   (DATA_MEM_MAP_REG_BASE + (0x063B * 4))
#define ENETF0_EGERR_REG                    (DATA_MEM_MAP_REG_BASE + (0x063C * 4))
#define ENETF0_EGDROP_REG                   (DATA_MEM_MAP_REG_BASE + (0x063D * 4))

#endif /*_IFXMIPS_ETOP_REG_H_ */
