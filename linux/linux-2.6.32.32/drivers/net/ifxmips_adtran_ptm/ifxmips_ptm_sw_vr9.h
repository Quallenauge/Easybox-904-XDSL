/******************************************************************************
**
** FILE NAME    : ifxmips_ptm_common.h
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : PTM driver header file (common definitions)
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
** 17 JUN 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFXMIPS_PTM_SW_VR9_H
#define IFXMIPS_PTM_SW_VR9_H



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  Internal Tantos Switch Register
 */
#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
  extern unsigned int ifx_vr9_sw_base_addr;
  #define VR9_SWIP_MACRO                KSEG1ADDR(ifx_vr9_sw_base_addr)
  #define VR9_SWIP_MACRO_ORG            0x1E108000
#else
  #define VR9_SWIP_MACRO                KSEG1ADDR(0x1E108000)
#endif
#define VR9_SWIP_MACRO_REG(off)         ((volatile u32*)(VR9_SWIP_MACRO + (off) * 4))
#define VR9_SWIP_TOP                    (VR9_SWIP_MACRO | (0x0C40 * 4))
#define VR9_SWIP_TOP_REG(off)           ((volatile u32*)(VR9_SWIP_TOP + (off) * 4))
#define ETHSW_SWRES_REG                 VR9_SWIP_MACRO_REG(0x00)
#define ETHSW_CLK_REG                   VR9_SWIP_MACRO_REG(0x01)
#define ETHSW_BM_RAM_VAL_3_REG          VR9_SWIP_MACRO_REG(0x40)
#define ETHSW_BM_RAM_VAL_2_REG          VR9_SWIP_MACRO_REG(0x41)
#define ETHSW_BM_RAM_VAL_1_REG          VR9_SWIP_MACRO_REG(0x42)
#define ETHSW_BM_RAM_VAL_0_REG          VR9_SWIP_MACRO_REG(0x43)
#define ETHSW_BM_RAM_ADDR_REG           VR9_SWIP_MACRO_REG(0x44)
#define ETHSW_BM_RAM_CTRL_REG           VR9_SWIP_MACRO_REG(0x45)
//  Buffer manager per port registrs
#define ETHSW_BM_PCFG_REG(port)         VR9_SWIP_MACRO_REG(0x80 + (port) * 2)   //  port < 7
#define ETHSW_BM_RMON_CTRL_REG(port)    VR9_SWIP_MACRO_REG(0x81 + (port) * 2)   //  port < 7
#define PCE_PMAP_REG(reg)               VR9_SWIP_MACRO_REG(0x453 + (reg) - 1)   //  1 <= reg <= 3
//  Parser & Classification Engine
#define PCE_TBL_KEY(n)                  VR9_SWIP_MACRO_REG(0x440 + 7 - (n))                 //  n < 7
#define PCE_TBL_MASK                    VR9_SWIP_MACRO_REG(0x448)
#define PCE_TBL_VAL(n)                  VR9_SWIP_MACRO_REG(0x449 + 4 - (n))                 //  n < 4;
#define PCE_TBL_ADDR                    VR9_SWIP_MACRO_REG(0x44E)
#define PCE_TBL_CTRL                    VR9_SWIP_MACRO_REG(0x44F)
#define PCE_TBL_STAT                    VR9_SWIP_MACRO_REG(0x450)
#define PCE_PMAP_REG(reg)               VR9_SWIP_MACRO_REG(0x453 + (reg) - 1)               //  reg >= 1 && reg <= 3
#define PCE_GCTRL_REG(reg)              VR9_SWIP_MACRO_REG(0x456 + (reg))
#define PCE_PCTRL_REG(port, reg)        VR9_SWIP_MACRO_REG(0x480 + (port) * 0xA + (reg))    //  port < 12, reg < 4
//  MAC Frame Length Register
#define MAC_FLEN_REG                    VR9_SWIP_MACRO_REG(0x8C5)
//  MAC Port Status Register
#define MAC_PSTAT_REG(port)             VR9_SWIP_MACRO_REG(0x900 + (port) * 0xC)//  port < 7
//  MAC Control Register 0
#define MAC_CTRL_REG(port, reg)         VR9_SWIP_MACRO_REG(0x903 + (port) * 0xC + (reg))    //  port < 7, reg < 7
//  Ethernet Switch Fetch DMA Port Control, Controls per-port functions of the Fetch DMA
#define FDMA_PCTRL_REG(port)            VR9_SWIP_MACRO_REG(0xA80 + (port) * 6)  //  port < 7
//  Ethernet Switch Store DMA Port Control, Controls per-port functions of the Store DMA
#define SDMA_PCTRL_REG(port)            VR9_SWIP_MACRO_REG(0xBC0 + (port) * 6)  //  port < 7
//  Global Control Register 0
#define GLOB_CTRL_REG                   VR9_SWIP_TOP_REG(0x00)
//  MDIO Control Register
#define MDIO_CTRL_REG                   VR9_SWIP_TOP_REG(0x08)
//  MDIO Read Data Register
#define MDIO_READ_REG                   VR9_SWIP_TOP_REG(0x09)
//  MDIO Write Data Register
#define MDIO_WRITE_REG                  VR9_SWIP_TOP_REG(0x0A)
//  MDC Clock Configuration Register 0
#define MDC_CFG_0_REG                   VR9_SWIP_TOP_REG(0x0B)
//  MDC Clock Configuration Register 1
#define MDC_CFG_1_REG                   VR9_SWIP_TOP_REG(0x0C)
//  PHY Address Register PORT 5~0
#define PHY_ADDR_REG(port)              VR9_SWIP_TOP_REG(0x15 - (port))     //  port < 6
//  PHY MDIO Polling Status per PORT
#define MDIO_STAT_REG(port)             VR9_SWIP_TOP_REG(0x16 + (port))     //  port < 6
//  xMII Control Registers
//  xMII Port 0 Configuration register
#define MII_CFG_REG(port)               VR9_SWIP_TOP_REG(0x36 + (port) * 2) //  port < 6
//  Configuration of Clock Delay for Port 0 (used for RGMII mode only)
#define MII_PCDU_REG(port)              VR9_SWIP_TOP_REG(0x37 + (port) * 2) //  port < 6
//  PMAC Header Control Register
#define PMAC_HD_CTL_REG                 VR9_SWIP_TOP_REG(0x82)
//  PMAC Type/Length register
#define PMAC_TL_REG                     VR9_SWIP_TOP_REG(0x83)
//  PMAC Source Address Register
#define PMAC_SA1_REG                    VR9_SWIP_TOP_REG(0x84)
#define PMAC_SA2_REG                    VR9_SWIP_TOP_REG(0x85)
#define PMAC_SA3_REG                    VR9_SWIP_TOP_REG(0x86)
//  PMAC Destination Address Register
#define PMAC_DA1_REG                    VR9_SWIP_TOP_REG(0x87)
#define PMAC_DA2_REG                    VR9_SWIP_TOP_REG(0x88)
#define PMAC_DA3_REG                    VR9_SWIP_TOP_REG(0x89)
//  PMAC VLAN register
#define PMAC_VLAN_REG                   VR9_SWIP_TOP_REG(0x8A)
//  PMAC Inter Packet Gap in RX Direction
#define PMAC_RX_IPG_REG                 VR9_SWIP_TOP_REG(0x8B)
//  PMAC Special Tag Ethertype
#define PMAC_ST_ETYPE_REG               VR9_SWIP_TOP_REG(0x8C)
//  PMAC Ethernet WAN Group
#define PMAC_EWAN_REG                   VR9_SWIP_TOP_REG(0x8D)

/*
 *  MIB Counters
 */
#define RX_TOTAL_PCKNT                  0x1F
#define RX_UNICAST_PCKNT                0x23
#define RX_MULTICAST_PCKNT              0x22
#define RX_CRC_ERR_PCKNT                0x21
#define RX_UNDERSIZE_GOOD_PCKNT         0x1D
#define RX_OVER_SIZE_GOOD_PCKNT         0x1B
#define RX_UNDERSIZE_ERR_PCKNT          0x1E
#define RX_GOOD_PAUSE_PCKNT             0x20
#define RX_OVER_SIZE_ERR_PCKNT          0x1C
#define RX_ALLIGN_ERR_PCKNT             0x1A
#define RX_FILTERED_PCKNT               0x19
#define RX_DISCARD_CONGESTION_PCKNT     0x11

#define RX_SIZE_64B_PCKNT               0x12
#define RX_SIZE_65_127B_PCKNT           0x13
#define RX_SIZE_128_255B_PCKNT          0x14
#define RX_SIZE_256_511B_PCKNT          0x15
#define RX_SIZE_512_1023B_PCKNT         0x16
#define RX_SIZE_1024B_MAX_PCKNT         0x17

#define TX_TOTAL_PCKNT                  0x0C
#define TX_UNICAST_PCKNT                0x06
#define TX_MULTICAST_PCKNT              0x07

#define TX_SINGLE_COLLISION_CNT         0x08
#define TX_MULTIPLE_COLLISION_CNT       0x09
#define TX_LATE_COLLISION_CNT           0x0A
#define TX_EXCESSIVE_COLLISION_CNT      0x0B
#define TX_PAUSE_PCKNT                  0x0D

#define TX_SIZE_64B_PCKNT               0x00
#define TX_SIZE_65_127B_PCKNT           0x01
#define TX_SIZE_128_255B_PCKNT          0x02
#define TX_SIZE_256_511B_PCKNT          0x03
#define TX_SIZE_512_1023B_PCKNT         0x04
#define TX_SIZE_1024B_MAX_PCKNT         0x05


#define TX_DROP_PCKNT                   0x10
#define RX_DROP_PCKNT                   0x18

#define RX_GOOD_BYTE_CNT_LOW            0x24
#define RX_GOOD_BYTE_CNT_HIGH           0x25

#define RX_BAD_BYTE_CNT_LOW             0x26
#define RX_BAD_BYTE_CNT_HIGH            0x27

#define TX_GOOD_BYTE_CNT_LOW            0x0E
#define TX_GOOD_BYTE_CNT_HIGH           0x0F

static inline unsigned int sw_get_rmon_counter(int port, int addr)
{
    *ETHSW_BM_RAM_ADDR_REG = addr;
    *ETHSW_BM_RAM_CTRL_REG  = 0x8000 | port;
    while ( (*ETHSW_BM_RAM_CTRL_REG & 0x8000) );

    return (*ETHSW_BM_RAM_VAL_1_REG << 16) | (*ETHSW_BM_RAM_VAL_0_REG & 0xFFFF);
}

static inline void sw_clr_rmon_counter(int port)
{
    int i;

    if ( port >= 0 && port < 7 )
    {
        *ETHSW_BM_PCFG_REG(port) = 0;
        *ETHSW_BM_RMON_CTRL_REG(port) = 3;
        for ( i = 1000; (*ETHSW_BM_RMON_CTRL_REG(port) & 3) != 0 && i > 0; i-- );
        if ( i == 0 )
            *ETHSW_BM_RMON_CTRL_REG(port) = 0;
        *ETHSW_BM_PCFG_REG(port) = 1;
    }
}

/*
 *  Switch PCE Micro Code
 */
/* Switch API Micro Code V0.3 */
// parser's microcode output field type
#define OUT_MAC0   0
#define OUT_MAC1   1
#define OUT_MAC2   2
#define OUT_MAC3   3
#define OUT_MAC4   4
#define OUT_MAC5   5
#define OUT_ETHTYP 6
#define OUT_VTAG0  7
#define OUT_VTAG1  8
#define OUT_ITAG0  9
#define OUT_ITAG1  10
#define OUT_ITAG2  11
#define OUT_ITAG3  12
#define OUT_IP0    13
#define OUT_IP1    14
#define OUT_IP2    15
#define OUT_IP3    16
#define OUT_SIP0   17
#define OUT_SIP1   18
#define OUT_SIP2   19
#define OUT_SIP3   20
#define OUT_SIP4   21
#define OUT_SIP5   22
#define OUT_SIP6   23
#define OUT_SIP7   24
#define OUT_DIP0   25
#define OUT_DIP1   26
#define OUT_DIP2   27
#define OUT_DIP3   28
#define OUT_DIP4   29
#define OUT_DIP5   30
#define OUT_DIP6   31
#define OUT_DIP7   32
#define OUT_SESID  33
#define OUT_PROT   34
#define OUT_APP0   35
#define OUT_APP1   36
#define OUT_IGMP0  37
#define OUT_IGMP1  38
#define OUT_IPOFF  39
#define OUT_NONE   63

// parser's microcode length type
#define INSTR   0
#define IPV6    1
#define LENACCU 2

// parser's microcode flag type
#define FLAG_ITAG   0
#define FLAG_VLAN   1
#define FLAG_SNAP   2
#define FLAG_PPPOE  3
#define FLAG_IPV6   4
#define FLAG_IPV6FL 5
#define FLAG_IPV4   6
#define FLAG_IGMP   7
#define FLAG_TU     8
#define FLAG_HOP    9
#define FLAG_NN1    10
#define FLAG_NN2    11
#define FLAG_END    12
#define FLAG_NO     13

// macro to define a microcode row
#define IFX_FLOW_PCE_MC_M(val, msk, ns, out, len, type, flags, ipv4_len) \
    { val, msk, ns<<10 | out<<4 | len>>1, (len&1)<<15 | type<<13 | flags<<9 | ipv4_len<<8 }

#define IFX_FLOW_PCE_MICROCODE_VALUES 64

typedef struct {
   unsigned short val_3;
   unsigned short val_2;
   unsigned short val_1;
   unsigned short val_0;
} IFX_FLOW_PCE_MICROCODE_ROW;

typedef IFX_FLOW_PCE_MICROCODE_ROW IFX_FLOW_PCE_MICROCODE[IFX_FLOW_PCE_MICROCODE_VALUES];



#endif  //  IFXMIPS_PTM_SW_VR9_H
