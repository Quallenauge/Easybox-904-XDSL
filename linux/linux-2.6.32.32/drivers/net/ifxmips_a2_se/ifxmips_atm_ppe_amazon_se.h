/******************************************************************************
**
** FILE NAME    : ifxmips_atm_ppe_amazon_se.h
** PROJECT      : UEIP
** MODULES     	: ATM (ADSL)
**
** DATE         : 1 AUG 2005
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM Driver (PPE Registers)
** COPYRIGHT    : 	Copyright (c) 2006
**			Infineon Technologies AG
**			Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
**  4 AUG 2005  Xu Liang        Initiate Version
** 23 OCT 2006  Xu Liang        Add GPL header.
**  9 JAN 2007  Xu Liang        First version got from Anand (IC designer)
*******************************************************************************/



#ifndef IFXMIPS_ATM_PPE_AMAZON_SE_H
#define IFXMIPS_ATM_PPE_AMAZON_SE_H



/*
 *  FPI Configuration Bus Register and Memory Address Mapping
 */
#define IFX_PPE                         (KSEG1 | 0x1E180000)
#define PP32_DEBUG_REG_ADDR(i, x)       ((volatile unsigned int*)(IFX_PPE + (((x) + 0x0000) << 2)))
#define PPM_INT_REG_ADDR(i, x)          ((volatile unsigned int*)(IFX_PPE + (((x) + 0x0030) << 2)))
#define PP32_INTERNAL_RES_ADDR(i, x)    ((volatile unsigned int*)(IFX_PPE + (((x) + 0x0040) << 2)))
#define CDM_CODE_MEMORY(i, x)           ((volatile unsigned int*)(IFX_PPE + (((x) + 0x1000) << 2)))
#define PPE_REG_ADDR(x)                 ((volatile unsigned int*)(IFX_PPE + (((x) + 0x4000) << 2)))
#define CDM_DATA_MEMORY(i, x)           ((volatile unsigned int*)(IFX_PPE + (((x) + 0x5000) << 2)))
#define PPM_INT_UNIT_ADDR(x)            ((volatile unsigned int*)(IFX_PPE + (((x) + 0x6000) << 2)))
#define PPM_TIMER0_ADDR(x)              ((volatile unsigned int*)(IFX_PPE + (((x) + 0x6100) << 2)))
#define PPM_TASK_IND_REG_ADDR(x)        ((volatile unsigned int*)(IFX_PPE + (((x) + 0x6200) << 2)))
#define PPS_BRK_ADDR(x)                 ((volatile unsigned int*)(IFX_PPE + (((x) + 0x6300) << 2)))
#define PPM_TIMER1_ADDR(x)              ((volatile unsigned int*)(IFX_PPE + (((x) + 0x6400) << 2)))
#define SB_RAM0_ADDR(x)                 ((volatile unsigned int*)(IFX_PPE + (((x) + 0x8200) << 2)))
#define SB_RAM1_ADDR(x)                 ((volatile unsigned int*)(IFX_PPE + (((x) + 0x8C00) << 2)))
#define QSB_CONF_REG_ADDR(x)            ((volatile unsigned int*)(IFX_PPE + (((x) + 0xC000) << 2)))

/*
 *  DWORD-Length of Memory Blocks
 */
#define PP32_DEBUG_REG_DWLEN            0x0030
#define PPM_INT_REG_DWLEN               0x0010
#define PP32_INTERNAL_RES_DWLEN         0x00C0
#define CDM_CODE_MEMORYn_DWLEN(n)       ((n) == 0 ? 0x1000 : 0x0800)
#define PPE_REG_DWLEN                   0x1000
#define CDM_DATA_MEMORY_DWLEN           CDM_CODE_MEMORYn_DWLEN(1)
#define PPM_INT_UNIT_DWLEN              0x0100
#define PPM_TIMER0_DWLEN                0x0100
#define PPM_TASK_IND_REG_DWLEN          0x0100
#define PPS_BRK_DWLEN                   0x0100
#define PPM_TIMER1_DWLEN                0x0100
#define SB_RAM0_DWLEN                   0x0A00
#define SB_RAM1_DWLEN                   0x0A00
#define QSB_CONF_REG_DWLEN              0x0100

/*
 *  PP32 to FPI Address Mapping
 */
#define SB_BUFFER(__sb_addr)            ((volatile unsigned int *)((((__sb_addr) >= 0x2200) && ((__sb_addr) <= 0x2BFF)) ? SB_RAM0_ADDR((__sb_addr) - 0x2200) :   \
                                                                   (((__sb_addr) >= 0x2C00) && ((__sb_addr) <= 0x35FF)) ? SB_RAM1_ADDR((__sb_addr) - 0x2C00) :   \
                                                                0))

/*
 *  PP32 Debug Control Register
 */
#define PP32_DBG_CTRL                   PP32_DEBUG_REG_ADDR(0, 0x0000)

#define DBG_CTRL_RESTART                0
#define DBG_CTRL_STOP                   1

#define PP32_HALT_STAT                  PP32_DEBUG_REG_ADDR(0, 0x0D00)
#define PP32_BREAKPOINT_REASONS         PP32_DEBUG_REG_ADDR(0, 0x0A00)

#define PP32_BRK_SRC                    PP32_DEBUG_REG_ADDR(0, 0x0F00)

#define PP32_DBG_CUR_PC                 PP32_DEBUG_REG_ADDR(0, 0x0F80)

#define PP32_DBG_TASK_NO                PP32_DEBUG_REG_ADDR(0, 0x0F81)

/*
 *  Share Buffer
 */
#define SB_MST_PRI0                     PPE_REG_ADDR(0x0300)
#define SB_MST_PRI1                     PPE_REG_ADDR(0x0301)

/*
 *  EMA Registers
 */
#define EMA_CMDCFG                      PPE_REG_ADDR(0x0A00)
#define EMA_DATACFG                     PPE_REG_ADDR(0x0A01)
#define EMA_CMDCNT                      PPE_REG_ADDR(0x0A02)
#define EMA_DATACNT                     PPE_REG_ADDR(0x0A03)
#define EMA_ISR                         PPE_REG_ADDR(0x0A04)
#define EMA_IER                         PPE_REG_ADDR(0x0A05)
#define EMA_CFG                         PPE_REG_ADDR(0x0A06)
#define EMA_SUBID                       PPE_REG_ADDR(0x0A07)

#define EMA_ALIGNMENT                   4

/*
 *  Mailbox IGU0 Interrupt
 */
#define ETH_MAILBOX_IGU0_INT            INT_NUM_IM2_IRL12

/*
 *  Mailbox IGU1 Interrupt
 */
#define PPE_MAILBOX_IGU1_INT            INT_NUM_IM2_IRL13

/*
 *  DPlus Registers
 */
#define DPLUS_TXDB                              PPE_REG_ADDR(0x0700)
#define DPLUS_TXCB                              PPE_REG_ADDR(0x0701)
#define DPLUS_TXCFG                             PPE_REG_ADDR(0x0702)
#define DPLUS_TXPGCNT                           PPE_REG_ADDR(0x0703)
#define DPLUS_RXDB                              PPE_REG_ADDR(0x0710)
#define DPLUS_RXCB                              PPE_REG_ADDR(0x0711)
#define DPLUS_RXCFG                             PPE_REG_ADDR(0x0712)
#define DPLUS_RXPGCNT                           PPE_REG_ADDR(0x0713)

/*
 *    ETOP MDIO Registers
 */
#define ETOP_MDIO_CFG                           PPE_REG_ADDR(0x0600)
#define ETOP_MDIO_ACC                           PPE_REG_ADDR(0x0601)
#define ETOP_CFG                                PPE_REG_ADDR(0x0602)
#define ETOP_IG_VLAN_COS                        PPE_REG_ADDR(0x0603)
#define ETOP_IG_DSCP_COSx(x)                    PPE_REG_ADDR(0x0607 - ((x) & 0x03))
#define ETOP_IG_PLEN_CTRL0                      PPE_REG_ADDR(0x0608)
//#define ETOP_IG_PLEN_CTRL1                      PPE_REG_ADDR(0x0609)
#define ETOP_ISR                                PPE_REG_ADDR(0x060A)
#define ETOP_IER                                PPE_REG_ADDR(0x060B)
#define ETOP_VPID                               PPE_REG_ADDR(0x060C)
#define ENET_MAC_CFG(i)                         PPE_REG_ADDR(0x0610 + ((i) ? 0x40 : 0x00))
#define ENETS_DBA(i)                            PPE_REG_ADDR(0x0612 + ((i) ? 0x40 : 0x00))
#define ENETS_CBA(i)                            PPE_REG_ADDR(0x0613 + ((i) ? 0x40 : 0x00))
#define ENETS_CFG(i)                            PPE_REG_ADDR(0x0614 + ((i) ? 0x40 : 0x00))
#define ENETS_PGCNT(i)                          PPE_REG_ADDR(0x0615 + ((i) ? 0x40 : 0x00))
#define ENETS_PKTCNT(i)                         PPE_REG_ADDR(0x0616 + ((i) ? 0x40 : 0x00))
#define ENETS_BUF_CTRL(i)                       PPE_REG_ADDR(0x0617 + ((i) ? 0x40 : 0x00))
#define ENETS_COS_CFG(i)                        PPE_REG_ADDR(0x0618 + ((i) ? 0x40 : 0x00))
#define ENETS_IGDROP(i)                         PPE_REG_ADDR(0x0619 + ((i) ? 0x40 : 0x00))
#define ENETS_IGERR(i)                          PPE_REG_ADDR(0x061A + ((i) ? 0x40 : 0x00))
#define ENETS_MAC_DA0(i)                        PPE_REG_ADDR(0x061B + ((i) ? 0x40 : 0x00))
#define ENETS_MAC_DA1(i)                        PPE_REG_ADDR(0x061C + ((i) ? 0x40 : 0x00))
#define ENETF_DBA(i)                            PPE_REG_ADDR(0x0630 + ((i) ? 0x40 : 0x00))
#define ENETF_CBA(i)                            PPE_REG_ADDR(0x0631 + ((i) ? 0x40 : 0x00))
#define ENETF_CFG(i)                            PPE_REG_ADDR(0x0632 + ((i) ? 0x40 : 0x00))
#define ENETF_PGCNT(i)                          PPE_REG_ADDR(0x0633 + ((i) ? 0x40 : 0x00))
#define ENETF_PKTCNT(i)                         PPE_REG_ADDR(0x0634 + ((i) ? 0x40 : 0x00))
#define ENETF_HFCTRL(i)                         PPE_REG_ADDR(0x0635 + ((i) ? 0x40 : 0x00))
#define ENETF_TXCTRL(i)                         PPE_REG_ADDR(0x0636 + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS0(i)                         PPE_REG_ADDR(0x0638 + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS1(i)                         PPE_REG_ADDR(0x0639 + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS2(i)                         PPE_REG_ADDR(0x063A + ((i) ? 0x40 : 0x00))
#define ENETF_VLCOS3(i)                         PPE_REG_ADDR(0x063B + ((i) ? 0x40 : 0x00))
#define ENETF_EGCOL(i)                          PPE_REG_ADDR(0x063C + ((i) ? 0x40 : 0x00))
#define ENETF_EGDROP(i)                         PPE_REG_ADDR(0x063D + ((i) ? 0x40 : 0x00))

/*  ETOP_MDIO_ACCESS MASKS  */
#define EPHY_RA_MASK                            0x80000000
#define EPHY_RW_MASK                            0x40000000  //Mask

#define EPHY_WRITE_MASK                         0x00000000
#define READ_MASK                               0x40000000
#define EPHY_ADDRESS                            0x08
#define EPHY_ADDRESS_SHIFT                      21
#define EPHY_REG_ADDRESS_SHIFT                  16

#define PHY0_ADDR                               0

#define RA_READ_ENABLE                          0x03    //MDIO Request/Ack and Read
#define RA_WRITE_ENABLE                         0x02    //MDIO Req/Ack and Write

#define EPHY_ADDRESS_MASK                       0x1F
#define EPHY_REG_MASK                           0x1F
#define EPHY_DATA_MASK                          0xFFFF

#define EPHY_RESET                              0x8000
#define EPHY_AUTO_NEGOTIATION_ENABLE            0x1000
#define AUTO_NEGOTIATION_COMPLETE               0x20
#define EPHY_RESTART_AUTO_NEGOTIATION           0x200

#define EPHY_MDIO_BASE_CONTROL_REG              0x00    //PHY Control Register
#define EPHY_LINK_SPEED_MASK                    0x2000
#define EPHY_AUTO_NEG_ENABLE_MASK               0x1000
#define EPHY_DUPLEX_MASK                        0x0100

#define EPHY_MDIO_BASE_STATUS_REG               0x01    //PHY status register
#define EPHY_LINK_STATUS_MASK                   0x02

#define EPHY_MDIO_ADVERTISMENT_REG              0x04    //Auto Negotiation Advertisement Register
#define EPHY_MDIO_ADVERT_100_FD                 0x100
#define EPHY_MDIO_ADVERT_100_HD                 0x080
#define EPHY_MDIO_ADVERT_10_FD                  0x040
#define EPHY_MDIO_ADVERT_10_HD                  0x020
#define EPHY_MDIO_BC_NEGOTIATE                  0x0200

#define EPHY_SPECIFIC_STATUS_REG                0x17    //PHY specific status register
#define EPHY_SPECIFIC_STATUS_SPEED_MASK         0x20
#define EPHY_SPECIFIC_STATUS_DUPLEX_MASK        0x40
#define EPHY_SPECIFIC_STATUS_LINK_UP            0x10
#define LINK_UP                                 0x01
#define LINK_DOWN                               0x00

#define EPHY_SET_CLASS_VALUE_REG                0x12



#endif  //  IFXMIPS_ATM_PPE_AMAZON_SE_H
