/****************************************************************************
                              Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/

#ifndef IFXMIPS_PCI_BOOT_H
#define IFXMIPS_PCI_BOOT_H

/* Bonding daughter board IDSEL 30, family board IDSEL 29 */
#define IFX_PCI_BOOT_DEV_IDSEL                   30
/* 33 MHz or 60 MHz */
#define IFX_PCI_CLK  IFX_PCI_60MHZ
/* 
 * FPI ==> PCI MEM address mapping 
 * base: 0xb8000000 == > 0x1e000001  
 * size: 8x4M = 32M 
 */
#define IFX_PCI_BOOT_FPI_BASE                    0x1E000001
#define IFX_PCI_BOOT_SRAM_BASE                   0x1F000000
#define IFX_PCI_BOOT_BLOCK_SIZE                  0x00400000

/* The following three window base must match PCI device outbound window configuration */
#define IFX_PCI_INBOUND_WINDOW_ONE_BASE          0xB8000000
#define IFX_PCI_INBOUND_WINDOW_TWO_BASE          0xB9000000
#define IFX_PCI_INBOUND_WINDOW_THREE_BASE        0xB9400000

#define IFX_PCI_INBOUND_WINDOW_ONE_16MB_DDR_BASE 0x01000000
#define IFX_PCI_INBOUND_WINDOW_ONE_14MB_DDR_BASE 0x0e000000

#define IFX_PCI_INBOUND_WINDOW_TWO_SRAM_BASE     0x1f000000
#define IFX_PCI_INBOUND_WINDOW_THREE_FPI_BASE    0x1e000000

#define IFX_PCI_BOOT_DEV_BAR2_BASE               0x10000000

#define IFX_PCI_DEV_PPE_ADDR                     0xb8220000 /* 0x1e220000 */
#define IFX_PCI_DEV_PPE_LOCAL_ADDR               0xbe220000
/* 
 * Access PCI device MPS register 0xbf2001c0 which is equal to 0x1f2001c0
 * ->host side  0xb92001c0 as the miniloader entry address 0xbe220000
 */
#define IFX_PCI_BOOT_DEV_MPS_ENTRY               (volatile u32*)(0xb92001c0)

/* 
 * Program PCI device PCI_RDY bit so that bootrom can start miniloader
 * PCI device RDY is in 0xbf203028 which mapped into host side as 0xb9203028 
 */
#define IFX_PCI_BOOT_DEV_RDY                     (volatile u32*)(0xb9203028)
#define IFX_PCI_BOOT_READY                       0x00000001

#define IFX_PCI_BOOT_POR_OUT                     (volatile u32*)(0xbe100b10)
#define IFX_PCI_BOOT_PORT_RST                    0x00008000
    
#endif /* IFXMIPS_PCI_BOOT_H */

