/**
** FILE NAME    : ifxmips_3port_sw_reg.h
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

#ifndef _IFXMIPS_3PORT_REG_H_
#define _IFXMIPS_3PORT_REG_H_ 
/*!
  \file ifxmips_3port_sw_reg.h
  \ingroup IFX_ETH_DRV
  \brief IFX Eth module register definition for ARx platforms
*/

/************************************************************************/
/*   Module       :  AR9  3-port Ethernet Switch register address and bits*/
/************************************************************************/
#define IFX_AR9_ETH_REG_BASE        (0xBE108000)
/*Port status */
#define IFX_AR9_ETH_PS			(volatile unsigned int*)(IFX_AR9_ETH_REG_BASE + 0x000)
#define PS_P0LS                 0x00000001	/*Port 0 link status */ 
#define PS_P0SS                 0x00000002	/*Port 0 speed status (0=10Mbps, 1=100Mbps*/ 
#define PS_P0SHS                0x00000004	/*Port 0 speed high status (1=1000Mbps)*/ 
#define PS_P0DS                 0x00000008	/*Port 0 duplex status (0= Half Duplex, 1= Full Duplex)*/ 
#define PS_P0FCS                0x00000010	/*Port 0 flow control(0= disable, 1= enable)*/ 
#define PS_P1LS                 0x00000100	/*Port 1 link status */ 
#define PS_P1SS                 0x00000200	/*Port 1 speed status (0=10Mbps, 1=100Mbps*/ 
#define PS_P1SHS                0x00000400	/*Port 1 speed high status (1=1000Mbps)*/ 
#define PS_P1DS                 0x00000800	/*Port 1 duplex status (0= Half Duplex, 1= Full Duplex)*/ 
#define PS_P1FCS                0x00001000	/*Port 1 flow control(0= disable, 1= enable)*/ 

#define IFX_AR9_ETH_P0_CTL      (volatile unsigned int*)(IFX_AR9_ETH_REG_BASE + 0x004)
#define IFX_AR9_ETH_P1_CTL      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x008)
#define IFX_AR9_ETH_P2_CTL      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x00C)
#define PX_CTL_BYPASS           0x00000001	/*Bypass mode (0=rule of VLAN tag member, 1=same format as receive ) */
#define PX_CTL_DSV8021X         0x00000002	/*Drop scheme for violation 802.1x (0=drop the packet, 1=don't care) */ 
#define PX_CTL_REDIR            0x00002000	/*Port redirect (0=disable, 1= enable)) */
#define PX_CTL_LD               0x00004000	/*Learning disable (1=disable, 0= enable)) */
#define PX_CTL_AD               0x00008000	/*Aging disable (1=disable, 0= enable)) */
#define PX_CTL_FLD              0x00020000	/*force mac link down (0=disable, 1= enable)) */
#define PX_CTL_FLP              0x00040000	/*force mac link up (0=disable, 1= enable)) */
#define PX_CTL_DFWD             0x00080000	/* Port ingress direct forwarding (0=disable, 1=enable) */
#define PX_CTL_DMDIO            0x00400000	/* Disable MDIO auto polling (0=disable, 1=enable) */
#define PX_CTL_TPE              0x01000000	/* Ether type Priority enable(0=disable, 1=enable) */
#define PX_CTL_IPVLAN           0x02000000	/* IP over VLAN Priority enable(0=disable, 1=enable) */
#define PX_CTL_SPE              0x04000000	/* Service Priority enable(0=disable, 1=enable) */
#define PX_CTL_VPE              0x08000000	/* VLAN Priority enable(0=disable, 1=enable) */
#define PX_CTL_IPOVTU           0x10000000	/* IP over TCP/UDP Priority enable(0=disable, 1=enable) */
#define PX_CTL_TCPE             0x20000000	/* TCP/UDP Priority enable(0=disable, 1=enable) */

#define IFX_AR9_ETH_GCTL0       (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x068) 
#define GCTL0_MPL_MASK          0x00000300   
#define GCTL0_MPL_1522          0x00000000   /*Max packet length 1522 Bytes */
#define GCTL0_MPL_1518          0x00000100   /*Max packet length 1518 Bytes */
#define GCTL0_MPL_1536          0x00000200   /*Max packet length 1536 Bytes */
#define GCTL0_LPE               0x20000000   /*Virtual ports over cpu physical port enable (0=disable, 1=enable) */
#define GCTL0_ICRCCD            0x40000000   /*Crc check enabled (0=disable, 1=enable) */
#define GCTL0_SE                0x80000000   /*Switch Enable (0=disable, 1=enable) */

/** Default Portmap register */
#define IFX_AR9_ETH_DF_PORTMAP_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x02C)
/** Unicast Portmap */
#define DF_PORTMAP_UP(port)             (port << (24+port))
/** Broadcast Portmap */
#define DF_PORTMAP_BP(port)             (port << (16+port))
/** Multicast Portmap */
#define DF_PORTMAP_MP(port)             (port << (8+port))
/** Reserve Portmap */
#define DF_PORTMAP_RP(port)             (port << (0+port))
/** Port Egress Control for Strict Q32/Q10 */
#define IFX_AR9_ETH_P0_ECS_Q32_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x30)
#define IFX_AR9_ETH_P0_ECS_Q10_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x34)
#define IFX_AR9_ETH_P0_ECW_Q32_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x38)
#define IFX_AR9_ETH_P0_ECW_Q10_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x3C)

#define IFX_AR9_ETH_P1_ECS_Q32_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x40)
#define IFX_AR9_ETH_P1_ECS_Q10_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x44)
#define IFX_AR9_ETH_P1_ECW_Q32_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x48)
#define IFX_AR9_ETH_P1_ECW_Q10_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x4C)

#define IFX_AR9_ETH_P2_ECS_Q32_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x50)
#define IFX_AR9_ETH_P2_ECS_Q10_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x54)
#define IFX_AR9_ETH_P2_ECW_Q32_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x58)
#define IFX_AR9_ETH_P2_ECW_Q10_REG      (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x5C)


#define IFX_AR9_ETH_RGMII_CTL           (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x078)
#define RGMII_CTL_FCE_SET(port, arg)    ( (arg & 0x1) << (port * 10))
#define RGMII_CTL_FCE_GET(port, arg)    ( (arg  >> (port * 10) ) & 0x1)

#define RGMII_CTL_DUP_MASK(port)        ( 0x1 << ((port * 10)+1))
#define RGMII_CTL_DUP_SET(port, arg)    ( (arg & 0x1) << ((port * 10)+1))
#define RGMII_CTL_DUP_GET(port, arg)    ( (arg  >> ((port * 10)+1) ) & 0x1)

#define RGMII_CTL_SPD_MASK(port)        ( (0xC) << ((port * 10)+2))
#define RGMII_CTL_SPD_100_SET(port)     ( (0x1) << ((port * 10)+2))
#define RGMII_CTL_SPD_1000_SET(port)    ( (0x2) << ((port * 10)+2))

#define RGMII_CTL_P0FCE         0x00000001	/*Port 0 flow control enable (0=disable, 1=enable) */
#define RGMII_CTL_P0DUP         0x00000002	/*Port 0 duplex mode (0=half duplex, 1=full duplex) */
#define RGMII_CTL_P0SPD_MASK    0x0000000C
#define RGMII_CTL_P0SPD_10      0x00000000	/*Port 0 speed 10Mbps */
#define RGMII_CTL_P0SPD_100     0x00000004	/*Port 0 speed 100Mbps */
#define RGMII_CTL_P0SPD_1000    0x00000008	/*Port 0 speed 1000Mbps */
#define RGMII_CTL_P0TDLY_MASK   0x00000030	/*Port 0 RGMII Tx clock delay */
#define RGMII_CTL_P0TDLY_0      0x00000000	/*Port 0 RGMII Tx clock delay 0ns */
#define RGMII_CTL_P0TDLY_1_5    0x00000010	/*Port 0 RGMII Tx clock delay 1.5ns */
#define RGMII_CTL_P0TDLY_1_75   0x00000020	/*Port 0 RGMII Tx clock delay 1.75ns */
#define RGMII_CTL_P0TDLY_2      0x00000030	/*Port 0 RGMII Tx clock delay 2ns */
#define RGMII_CTL_P0RDLY_MASK   0x000000C0	/*Port 0 RGMII Rx clock delay */
#define RGMII_CTL_P0RDLY_0      0x00000000	/*Port 0 RGMII Rx clock delay 0ns */
#define RGMII_CTL_P0RDLY_1_5    0x00000040	/*Port 0 RGMII Rx clock delay 1.5ns */
#define RGMII_CTL_P0RDLY_1_75   0x00000080	/*Port 0 RGMII Rx clock delay 1.75ns */
#define RGMII_CTL_P0RDLY_2      0x000000C0	/*Port 0 RGMII Rx clock delay 2ns */
#define RGMII_CTL_P0IS_MASK     0x00000300	/*Port 0 interface selection*/
#define RGMII_CTL_P0IS_RGMII    0x00000000	/*Port 0 interface selection  RGMII mode*/
#define RGMII_CTL_P0IS_MII      0x00000100	/*Port 0 interface selection  MII mode*/
#define RGMII_CTL_P0IS_REVMII   0x00000200	/*Port 0 interface selection  Rev MII mode*/
#define RGMII_CTL_P0IS_REDMII   0x00000300	/*Port 0 interface selection  Reduced MII mode*/

#define RGMII_CTL_P1FCE         0x00000400	/*Port 1 flow control enable (0=disable, 1=enable) */
#define RGMII_CTL_P1DUP         0x00000800	/*Port 1 duplex mode (0=half duplex, 1=full duplex) */
#define RGMII_CTL_P1SPD_MASK    0x00003000
#define RGMII_CTL_P1SPD_10      0x00000000	/*Port 1 speed 10Mbps */
#define RGMII_CTL_P1SPD_100     0x00001000	/*Port 1 speed 100Mbps */
#define RGMII_CTL_P1SPD_1000    0x00002000	/*Port 1 speed 1000Mbps */
#define RGMII_CTL_P1TDLY_MASK   0x0000C000	/*Port 1 RGMII Tx clock delay */
#define RGMII_CTL_P1TDLY_0      0x00000000	/*Port 1 RGMII Tx clock delay 0ns */
#define RGMII_CTL_P1TDLY_1_5    0x00004000	/*Port 1 RGMII Tx clock delay 1.5ns */
#define RGMII_CTL_P1TDLY_1_75   0x00008000	/*Port 1 RGMII Tx clock delay 1.75ns */
#define RGMII_CTL_P1TDLY_2      0x0000C000	/*Port 1 RGMII Tx clock delay 2ns */
#define RGMII_CTL_P1RDLY_MASK   0x00030000	/*Port 1 RGMII Rx clock delay */
#define RGMII_CTL_P1RDLY_0      0x00000000	/*Port 1 RGMII Rx clock delay 0ns */
#define RGMII_CTL_P1RDLY_1_5    0x00010000	/*Port 1 RGMII Rx clock delay 1.5ns */
#define RGMII_CTL_P1RDLY_1_75   0x00020000	/*Port 1 RGMII Rx clock delay 1.75ns */
#define RGMII_CTL_P1RDLY_2      0x00030000	/*Port 1 RGMII Rx clock delay 2ns */
#define RGMII_CTL_P1IS_MASK     0x000C0000	/*Port 1 interface selection*/
#define RGMII_CTL_P1IS_RGMII    0x00000000	/*Port 1 interface selection  RGMII mode*/
#define RGMII_CTL_P1IS_MII      0x00040000	/*Port 1 interface selection  MII mode*/
#define RGMII_CTL_P1IS_REVMII   0x00080000	/*Port 1 interface selection  Rev MII mode*/
#define RGMII_CTL_P1IS_REDMII   0x000C0000	/*Port 1 interface selection  Reduced MII mode*/

#define RGMII_CTL_P0FRQ         0x00100000	/*Port 0 interface Rev MII clock frquency ( 0=25MHz, 1=50MHz)*/
#define RGMII_CTL_P0CKIO        0x00200000	/*Port 0 interface Clock PAD I/O select ( 0=Input , 1=Output)*/
#define RGMII_CTL_P1FRQ         0x00400000	/*Port 1 interface Rev MII clock frquency ( 0=25MHz, 1=50MHz)*/
#define RGMII_CTL_P1CKIO        0x00800000	/*Port 1 interface Clock PAD I/O select ( 0=Input , 1=Output)*/
#define RGMII_CTL_MCS_MASK      0xFF000000	/* MDC clcok select, 25MHz/((MCS+1)*2)*/

#define IFX_AR9_ETH_1P_PRT      (volatile unsigned int*)(IFX_AR9_ETH_REG_BASE + 0x07C)
#define P_PRT_1PPQ0_MASK        0x00000003	/*Priority Queue 0 */
#define P_PRT_1PPQ0_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 0 */
#define P_PRT_1PPQ0_Q1			0x00000001	/*Swithc Queue 1 --> Priority Queue 0 */
#define P_PRT_1PPQ0_Q2			0x00000002	/*Switch Queue 2 --> Priority Queue 0 */
#define P_PRT_1PPQ0_Q3			0x00000003	/*Switch Queue 3 --> Priority Queue 0 */

#define P_PRT_1PPQ1_MASK		0x0000000C	/*Priority Queue 1 */
#define P_PRT_1PPQ1_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 1 */
#define P_PRT_1PPQ1_Q1			0x00000004	/*Switch Queue 1 --> Priority Queue 1 */
#define P_PRT_1PPQ1_Q2			0x00000008	/*Switch Queue 2 --> Priority Queue 1 */
#define P_PRT_1PPQ1_Q3			0x0000000C	/*Switch Queue 3 --> Priority Queue 1 */

#define P_PRT_1PPQ2_MASK		0x00000030	/*Priority Queue 2 */
#define P_PRT_1PPQ2_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 2 */
#define P_PRT_1PPQ2_Q1			0x00000010	/*Switch Queue 1 --> Priority Queue 2 */
#define P_PRT_1PPQ2_Q2			0x00000020	/*Switch Queue 2 --> Priority Queue 2 */
#define P_PRT_1PPQ2_Q3			0x00000030	/*Switch Queue 3 --> Priority Queue 2 */

#define P_PRT_1PPQ3_MASK		0x000000C0	/*Priority Queue 3 */
#define P_PRT_1PPQ3_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 3 */
#define P_PRT_1PPQ3_Q1			0x00000040	/*Switch Queue 1 --> Priority Queue 3 */
#define P_PRT_1PPQ3_Q2			0x00000080	/*Switch Queue 2 --> Priority Queue 3 */
#define P_PRT_1PPQ3_Q3			0x000000C0	/*Switch Queue 3 --> Priority Queue 3 */

#define P_PRT_1PPQ4_MASK		0x00000300	/*Priority Queue 4 */
#define P_PRT_1PPQ4_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 4 */
#define P_PRT_1PPQ4_Q1			0x00000100	/*Switch Queue 1 --> Priority Queue 4 */
#define P_PRT_1PPQ4_Q2			0x00000200	/*Switch Queue 2 --> Priority Queue 4 */
#define P_PRT_1PPQ4_Q3			0x00000300	/*Switch Queue 3 --> Priority Queue 4 */

#define P_PRT_1PPQ5_MASK		0x00000C00	/*Priority Queue 5 */
#define P_PRT_1PPQ5_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 5 */
#define P_PRT_1PPQ5_Q1			0x00000400	/*Switch Queue 1 --> Priority Queue 5 */
#define P_PRT_1PPQ5_Q2			0x00000800	/*Switch Queue 2 --> Priority Queue 5 */
#define P_PRT_1PPQ5_Q3			0x00000C00	/*Switch Queue 3 --> Priority Queue 5 */

#define P_PRT_1PPQ6_MASK		0x00003000	/*Priority Queue 6 */
#define P_PRT_1PPQ6_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 6 */
#define P_PRT_1PPQ6_Q1			0x00000100	/*Switch Queue 1 --> Priority Queue 6 */
#define P_PRT_1PPQ6_Q2			0x00000200	/*Switch Queue 2 --> Priority Queue 6 */
#define P_PRT_1PPQ6_Q3			0x00000300	/*Switch Queue 3 --> Priority Queue 6 */

#define P_PRT_1PPQ7_MASK		0x0000C000	/*Priority Queue 7 */
#define P_PRT_1PPQ7_Q0			0x00000000	/*Switch Queue 0 --> Priority Queue 7 */
#define P_PRT_1PPQ7_Q1			0x00004000	/*Switch Queue 1 --> Priority Queue 7 */
#define P_PRT_1PPQ7_Q2			0x00008000	/*Switch Queue 2 --> Priority Queue 7 */
#define P_PRT_1PPQ7_Q3			0x0000C000	/*Switch Queue 3 --> Priority Queue 7 */

#define IFX_AR9_ETH_PMAC_HD_CTL (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x08C)   
#define PMAC_HD_CTL_ADD         0x00000001	/* Add ethernet header to packets from DMA to PMAC */
#define PMAC_HD_CTL_TAG         0x00000002	/* Add VLAN Tag to packets from DMA to PMAC */
#define PMAC_HD_CTL_TYPE_LEN_MASK   0x0003FFFC	/* Contains the Length/Type value added to packest from DMA to PMAC */
#define PMAC_HD_CTL_AC          0x00040000	/* Add CRC for packest from DMA to PMAC */
#define PMAC_HD_CTL_AS          0x00080000	/* Add status header for packest from PMAC to DMA */
#define PMAC_HD_CTL_RC          0x00100000	/* Remove CRC from packest going from PMAC to DMA */
#define PMAC_HD_CTL_RL2         0x00200000	/* Remove Layer-2 Header from packest going from PMAC to DMA */
#define PMAC_HD_CTL_RXSH        0x00400000	/* Add status header for packest from DMA to PMAC */

#define IFX_AR9_ETH_PMAC_SA1    (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x090)
#define IFX_AR9_ETH_PMAC_SA2    (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x094)
#define IFX_AR9_ETH_PMAC_DA1    (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x098)
#define IFX_AR9_ETH_PMAC_DA2    (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x09C)

#define IFX_AR9_ETH_PMAC_TX_IPG (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x0A4)
#define IFX_AR9_ETH_PMAC_RX_IPG (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x0A8)
#define PMAC_RX_IPG_IPG_CNT_MASK    0x0000000F 	/*IPG counter value */
#define PMAC_RX_IPG_IREQ_WM_MASK    0x000000F0 	/* Rx FIFO request watermark value */
#define PMAC_RX_IPG_IDIS_RWQ_WM     0x00000100 	/* Disable RX FIFO request watermark */

#define IFX_AR9_ETH_RMON_CTL    (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x0C4)
#define RMON_CTL_OFFSET_MASK    0x0000003F	/*Counter offset */
#define RMON_CTL_PORTC_MASK     0x000001C0	/*Port number */
#define RMON_CTL_CAC_MASK       0x00000600	/*Command for access counters*/
#define RMON_CTL_CAC_INDIR      0x00000000	/*Indirect Read counters */
#define RMON_CTL_CAC_PORT_CNT   0x00000200	/*Get port counters (offset 0~237)*/
#define RMON_CTL_CAC_PORT_RST   0x00000400	/*Reset port counters*/
#define RMON_CTL_CAC_RST        0x00000400	/*Reset all counters */
#define RMON_CTL_BUSY           0x00000800	/*Busy/Access state*/

#define IFX_AR9_ETH_RMON_ST     (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x0C8)
#define IFX_AR9_ETH_MDIO_CTL    (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x0CC)   
#define MDIO_CTL_REGAD_MASK     0x0000001F	/*Register address */
#define MDIO_CTL_REGAD_SET(arg) ((arg & 0x1F))
#define MDIO_CTL_PHYAD_MASK     0x000003E0	/*PHY address */
#define MDIO_CTL_PHYAD_SET(arg) ((arg & 0x1FF) << 5)
#define MDIO_CTL_OP_MASK        0x00000C00	/* Operation code*/
#define MDIO_CTL_OP_WRITE       0x00000400	/* Write command*/
#define MDIO_CTL_OP_READ        0x00000800	/* Read command*/
#define MDIO_CTL_MBUSY          0x00008000	/* Busy state */
#define MDIO_CTL_WD_MASK        0xFFFF0000	
#define MDIO_CTL_WD_OFFSET      16	
#define MDIO_CTL_WD(val)        ( ( val << MDIO_CTL_WD_OFFSET) & MDIO_CTL_WD_MASK)

#define IFX_AR9_ETH_MDIO_DATA   (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x0D0)
#define MDIO_CTL_RD_MASK        0x0000FFFF	

#define IFX_AR9_ETH_DFSRV_MAP0  (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x150)
#define IFX_AR9_ETH_DFSRV_MAP1  (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x154)
#define IFX_AR9_ETH_DFSRV_MAP2  (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x158)
#define IFX_AR9_ETH_DFSRV_MAP3  (volatile unsigned int*) (IFX_AR9_ETH_REG_BASE + 0x15C)

#endif /*_IFXMIPS_3PORT_REG_H_  */
