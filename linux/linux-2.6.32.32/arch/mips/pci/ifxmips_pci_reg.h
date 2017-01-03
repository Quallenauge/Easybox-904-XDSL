/*************************************************************************
 * 
 * FILE NAME    : ifxmips_pci_reg.h
 * PROJECT      : IFX UEIP
 * MODULES      : PCI
 *
 * DATE         : 29 June 2009
 * AUTHOR       : Lei Chuanhua
 *
 * DESCRIPTION  : PCI Host Controller Driver
 * COPYRIGHT    :       Copyright (c) 2009
 *                      Infineon Technologies AG
 *                      Am Campeon 1-12, 85579 Neubiberg, Germany
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * HISTORY
 * $Version $Date      $Author     $Comment
 * 1.0      29 Jun     Lei Chuanhua First UEIP release
 *************************************************************************/
/*!
  \file ifxmips_pci_reg.h
  \ingroup IFX_PCI
  \brief pci bus driver register definition
*/
#ifndef IFXMIPS_PCI_REG_H
#define IFXMIPS_PCI_REG_H

#define IFX_PCI_CTRL_BASE                       (KSEG1 | 0x1E105400)
#define IFX_PCI_CFG_PHY_BASE                    0x17000000
#define IFX_PCI_CFG_BASE                        (KSEG1 | 0x17000000)
#define IFX_PCI_CFG_SIZE                        0x01000000

#define IFX_PCI_MEM_BASE                        (KSEG1 | 0x18000000)
#define IFX_PCI_MEM_PHY_BASE                    0x18000000
#define IFX_PCI_MEM_SIZE                        0x02000000

#define IFX_PCI_IO_BASE                         (KSEG1 | 0x1AE00000)
#define IFX_PCI_IO_PHY_BASE                     0x1AE00000
#define IFX_PCI_IO_SIZE                         0x00200000

/* PCI Host Controller Register Mapping */
#define IFX_PCI_CLK_CTRL                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0000)
#define IFX_PCI_CLK_CTRL_FPI_CLK_DIS            0x00000001
#define IFX_PCI_CLK_CTRL_EBU_PCI_SWITCH_EN      0x00000002
#define IFX_PCI_CLK_CTRL_FIXED_ARB_EN           0x00000004
#define IFX_PCI_CLK_CTRL_FPI_NORMAL_ACK         0x00000008

#define IFX_PCI_ID                              (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0004)
#define IFX_PCI_SFT_RST                         (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0010)
#define IFX_PCI_SFT_RST_REQ                     0x00000001
#define IFX_PCI_SFT_RST_ACKING                  0x00000002

#define IFX_PCI_PCI_FPI_ERR_ADDR                (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0014)
#define IFX_PCI_FPI_PCI_ERR_ADDR                (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0018)
#define IFX_PCI_FPI_ERR_TAG                     (volatile u32*)(IFX_PCI_CTRL_BASE + 0x001C)
#define IFX_PCI_IRR                             (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0020)
#define IFX_PCI_IRA                             (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0024)
#define IFX_PCI_IRM                             (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0028)
#define IFX_PCI_IR_TIMEOUT                      0x00010000
#define IFX_PCI_SWAP                            (volatile u32*)(IFX_PCI_CTRL_BASE + 0x002C)
#define IFX_PCI_SWAP_RX                         0x00000001
#define IFX_PCI_SWAP_TX                         0x00000002

#define IFX_PCI_MOD                             (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0030)
#define IFX_PCI_MOD_ONE_IRQ_INTA                0x00000000
#define IFX_PCI_MOD_TWO_IRQ_INTA_OR_INTB        0x00000001
#define IFX_PCI_MOD_TWO_IRQ_INTA_AND_INTB       0x00000003
#define IFX_PCI_MOD_READ_BURST_THRESHOLD        0x00000700
#define IFX_PCI_MOD_READ_BURST_THRESHOLD_S      8
#define IFX_PCI_MOD_CFG_OK                      0x01000000

#define IFX_PCI_MOD_RD_RESP_16CLK_RULE          0x10000000

#define IFX_PCI_DV_ID                           (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0034)
#define IFX_PCI_SUBSYS_ID                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0038)
#define IFX_PCI_PM                              (volatile u32*)(IFX_PCI_CTRL_BASE + 0x003C)
#define IFX_PCI_CLASS_CODE1                     (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0040)
#define IFX_PCI_BAR11MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0044)
#define IFX_PCI_BAR12MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0048)
#define IFX_PCI_BAR13MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x004C)
#define IFX_PCI_BAR14MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0050)
#define IFX_PCI_BAR15MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0054)
#define IFX_PCI_BAR16MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0058)

#define IFX_PCI_BAR_PREFETCH                    0x00000008
#define IFX_PCI_BAR_LD                          0x0FFFFFF0
#define IFX_PCI_BAR_LD_S                        4
#define IFX_PCI_BAR_DECODING_EN                 0x80000000

#define IFX_PCI_CIS_PT1                         (volatile u32*)(IFX_PCI_CTRL_BASE + 0x005C)
#define IFX_PCI_SUBSYS_ID1                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0060)
#define IFX_PCI_ADDR_MAP11                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0064)
#define IFX_PCI_ADDR_MAP12                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0068)
#define IFX_PCI_ADDR_MAP13                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x006C)
#define IFX_PCI_ADDR_MAP14                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0070)
#define IFX_PCI_ADDR_MAP15                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0074)
#define IFX_PCI_ADDR_MAP16                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0078)
#define IFX_PCI_ADDR_MAP_SUPERVISOR             0x00000001

#define IFX_PCI_FPI_SEG_EN                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x007C)
#define IFX_PCI_ARB                             (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0080)
#define IFX_PCI_ARB_INTERNAL_EN                 0x00000001
#define IFX_PCI_ARB_FIXED_EN                    0x00000002
#define IFX_PCI_ARB_MASTER_REQ0                 0x0000000C
#define IFX_PCI_ARB_MASTER_REQ0_S               2
#define IFX_PCI_ARB_MASTER_REQ1                 0x00000030
#define IFX_PCI_ARB_MASTER_REQ1_S               4
#define IFX_PCI_ARB_MASTER_REQ2                 0x000000C0
#define IFX_PCI_ARB_MASTER_REQ2_S               6
#define IFX_PCI_ARB_PCI_MASTER_REQ0             0x00000300
#define IFX_PCI_ARB_PCI_MASTER_REQ0_S           8
#define IFX_PCI_ARB_PCI_MASTER_REQ1             0x00000C00
#define IFX_PCI_ARB_PCI_MASTER_REQ1_S           10
#define IFX_PCI_ARB_PCI_MASTER_REQ2             0x00003000
#define IFX_PCI_ARB_PCI_MASTER_REQ2_S           12
#define IFX_PCI_ARB_PCI_PORT_ARB                0x000F0000
#define IFX_PCI_ARB_PCI_PORT_ARB_S              16
#define IFX_PCI_ARB_PCI_NOT_READY               0x00100000
#define IFX_PCI_ARB_PCI_NO_FRM                  0x00200000
#define IFX_PCI_ARB_EBU_IDLE                    0x00400000
#define IFX_PCI_ART_PCI_IDLE                    (IFX_PCI_ARB_PCI_NOT_READY | IFX_PCI_ARB_PCI_NO_FRM)

#define IFX_PCI_BAR21MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0084)
#define IFX_PCI_BAR22MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0088)
#define IFX_PCI_BAR23MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x008C)
#define IFX_PCI_BAR24MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0090)
#define IFX_PCI_BAR25MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0094)
#define IFX_PCI_BAR26MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x0098)
#define IFX_PCI_CIS_PT2                         (volatile u32*)(IFX_PCI_CTRL_BASE + 0x009C)
#define IFX_PCI_SUBSYS_ID2                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00A0)
#define IFX_PCI_ADDR_MAP21                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00A4)
#define IFX_PCI_ADDR_MAP22                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00A8)
#define IFX_PCI_ADDR_MAP23                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00AC)
#define IFX_PCI_ADDR_MAP24                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00B0)
#define IFX_PCI_ADDR_MAP25                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00B4)
#define IFX_PCI_ADDR_MAP26                      (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00B8)
#define IFX_PCI_ADDR_MASK                       (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00BC)
#define IFX_PCI_FPI_ADDR_MAP0                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00C0)
#define IFX_PCI_FPI_ADDR_MAP1                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00C4)
#define IFX_PCI_FPI_ADDR_MAP2                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00C8)
#define IFX_PCI_FPI_ADDR_MAP3                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00CC)
#define IFX_PCI_FPI_ADDR_MAP4                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00D0)
#define IFX_PCI_FPI_ADDR_MAP5                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00D4)
#define IFX_PCI_FPI_ADDR_MAP6                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00D8)
#define IFX_PCI_FPI_ADDR_MAP7                   (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00DC)
#define IFX_PCI_FPI_ADDR_MAP11_LOW              (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00E0)
#define IFX_PCI_FPI_ADDR_MAP11_HIGH             (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00E4)
#define IFX_PCI_FPI_BURST_LENGTH                (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00E8)
#define IFX_PCI_FPI_RD_BURST_LEN                0x00000003
#define IFX_PCI_FPI_RD_BURST_LEN_S              0
#define IFX_PCI_FPI_WR_BURST_LEN                0x00000300
#define IFX_PCI_FPI_WR_BURST_LEN_S              8

enum {
    IFX_PCI_FPI_BURST_LEN1 = 0,
    IFX_PCI_FPI_BURST_LEN2,
    IFX_PCI_FPI_BURST_LEN4,
    IFX_PCI_FPI_BURST_LEN8,
};

#define IFX_PCI_PCI_SET_SERR                    (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00EC)
#define IFX_PCI_DMA_FPI_ST_ADDR                 (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00F0)
#define IFX_PCI_DMA_PCI_ST_ADDR                 (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00F4)
#define IFX_PCI_DMA_TRAN_CNT                    (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00F8)
#define IFX_PCI_DMA_TRAN_CTL                    (volatile u32*)(IFX_PCI_CTRL_BASE + 0x00FC)

/* PCI configuration space register mapping */
#define IFX_PCI_CS_DEV_VEN_ID_REG               (volatile u32*)(IFX_PCI_CFG_BASE + 0x0000)
#define IFX_PCI_CMD                             (volatile u32*)(IFX_PCI_CFG_BASE + 0x0004)
#define IFX_PCI_CMD_IO_EN                       0x00000001
#define IFX_PCI_CMD_MEM_EN                      0x00000002
#define IFX_PCI_CMD_MASTER_EN                   0x00000004
#define IFX_PCI_CMD_SPECIAL_CYCL_EN             0x00000008
#define IFX_PCI_CMD_MEM_WR_INVALIDATE_EN        0x00000010
#define IFX_PCI_CMD_PARITY_ERR_EN               0x00000040
#define IFX_PCI_CMD_SYS_ERR_EN                  0x00000100
#define IFX_PCI_CMD_FAST_B2B_EN                 0x00000200

#define IFX_PCI_CL_CODE_REVID                   (volatile u32*)(IFX_PCI_CFG_BASE + 0x0008)
#define IFX_PCI_BST_HT_LT_CLS                   (volatile u32*)(IFX_PCI_CFG_BASE + 0x000C)
#define IFX_PCI_BAR1                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0010)
#define IFX_PCI_BAR2                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0014)
#define IFX_PCI_BAR3                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0018)
#define IFX_PCI_BAR4                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x001C)
#define IFX_PCI_BAR5                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0020)
#define IFX_PCI_BAR6                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0024)
#define IFX_PCI_CARDBUS_CIS_PT                  (volatile u32*)(IFX_PCI_CFG_BASE + 0x0028)
#define IFX_PCI_SUBSYS_VEN_ID                   (volatile u32*)(IFX_PCI_CFG_BASE + 0x002C)
#define IFX_PCI_EXROM_BAS_ADDR                  (volatile u32*)(IFX_PCI_CFG_BASE + 0x0030)
#define IFX_PCI_RES1                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0034)
#define IFX_PCI_RES2                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0038)
#define IFX_PCI_LAT_GNT_INTR                    (volatile u32*)(IFX_PCI_CFG_BASE + 0x003C)
#define IFX_PCI_PM_PT_CPID                      (volatile u32*)(IFX_PCI_CFG_BASE + 0x0040)
#define IFX_PCI_DAT_PMCSR_PM                    (volatile u32*)(IFX_PCI_CFG_BASE + 0x0044)
#define IFX_PCI_RES3                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x0048)
#define IFX_PCI_RES4                            (volatile u32*)(IFX_PCI_CFG_BASE + 0x004C)
#define IFX_PCI_ERR_ADDR_PCI_FPI                (volatile u32*)(IFX_PCI_CFG_BASE + 0x0050)
#define IFX_PCI_ERR_ADDR_FPI_PCI                (volatile u32*)(IFX_PCI_CFG_BASE + 0x0054)
#define IFX_PCI_ERR_TAG_FPI_PCI                 (volatile u32*)(IFX_PCI_CFG_BASE + 0x0058)
#define IFX_PCI_PC_ARB                          (volatile u32*)(IFX_PCI_CFG_BASE + 0x005C)
#define IFX_PCI_FPI_PCI_INT_STS                 (volatile u32*)(IFX_PCI_CFG_BASE + 0x0060)
#define IFX_PCI_FPI_PCI_INT_ACK                 (volatile u32*)(IFX_PCI_CFG_BASE + 0x0064)
#define IFX_PCI_FPI_PCI_INT_MASK                (volatile u32*)(IFX_PCI_CFG_BASE + 0x0068)
#define IFX_PCI_CARDBUS_FRAME_MASK              (volatile u32*)(IFX_PCI_CFG_BASE + 0x006C)

#define IFX_PCI_CARDBUS_FRAME_MASK_EN           0x00000001
#endif /* IFXMIPS_PCI_REG_H */

