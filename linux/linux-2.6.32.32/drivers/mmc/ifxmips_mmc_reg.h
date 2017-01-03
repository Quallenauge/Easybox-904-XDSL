/**
** FILE NAME    : ifxmips_mmc_reg.h
** PROJECT      : IFX UEIP
** MODULES      : MMC module 
** DATE         : 08 July 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : IFX Cross-Platform MMC driver register header file
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
** 08 July 2009         Reddy Mallikarjuna              Initial release
*******************************************************************************/
#ifndef _IFXMIPS_MMCREG_H_
#define _IFXMIPS_MMCREG_H_
/*!
  \file ifxmips_mmc_reg.h
  \ingroup IFX_MMC_DRV
  \brief IFX MMC module register definition
*/

#include <asm/ifx/ifx_regs.h>

#define BIT0            (1<<0)
#define BIT1            (1<<1)
#define BIT2            (1<<2)
#define BIT3            (1<<3)
#define BIT4            (1<<4)
#define BIT5            (1<<5)
#define BIT6            (1<<6)
#define BIT7            (1<<7)
#define BIT8            (1<<8)
#define BIT9            (1<<9)
#define BIT10           (1<<10)
#define BIT11           (1<<11)
#define BIT12           (1<<12)
#define BIT13           (1<<13)
#define BIT14           (1<<14)
#define BIT15           (1<<15)
#define BIT16           (1<<16)
#define BIT17           (1<<17)
#define BIT18           (1<<18)
#define BIT19           (1<<19)
#define BIT20           (1<<20)
#define BIT21           (1<<21)
#define BIT22           (1<<22)
#define BIT23           (1<<23)
#define BIT24           (1<<24)
#define BIT25           (1<<25)
#define BIT26           (1<<26)
#define BIT27           (1<<27)
#define BIT28           (1<<28)
#define BIT29           (1<<29)
#define BIT30           (1<<30)
#define BIT31           (1<<31)

#define MMC_WRITE_REG32(data,addr)         IFX_REG_W32((data), (volatile unsigned  int *)addr)
#define MMC_READ_REG32(addr)               IFX_REG_R32((volatile unsigned  int *)addr)

/*MMC module registers address space */
#define IFX_MMC_BASE_ADDR       (0xBE102000)
/** power control register */
#define MCI_PWR                 (IFX_MMC_BASE_ADDR+ 0x0000)
#define MCI_PWR_OFF             0x0     /*Power is switched off */
#define MCI_PWR_UP              0x2     /*Power up on command*/
#define MCI_PWR_ON              0x3     /*Power always on*/
/** clock control register */
#define MCI_CLK                 (IFX_MMC_BASE_ADDR	+ 0x0004)
#define MCI_CLK_DIV_MASK        0x000000FF
#define MCI_CLK_DIV_SET(arg)    ( (arg) & MCI_CLK_DIV_MASK)
#define MCI_CLK_DIV_GET(arg)    ( (arg) & MCI_CLK_DIV_MASK)
#define MCI_CLK_ENABLE          BIT8    /*Enable MCI clock */
#define MCI_CLK_PWRSAVE         BIT9    /*Disable MCI clock output when bus is idle */
#define MCI_CLK_BYPASS          BIT10   /*Enable bypass of clock divide logic */
#define MCI_WIDE_BUS            BIT11   /*wide bus mode. MCIDAT3:0 are used */

#define MMC_CLK_400K            400000
#define MMC_BUS_1BITS           1
#define MMC_BUS_4BITS           4
#define MMC_ERR_STARTBIT        6
/** Set frequency*/
#define MMC_SET_FREQENCY        0x01 
/** Set bus width*/
#define MMC_SET_BUS_WIDTH       0x02 
/** Argument register */
#define MCI_ARG                 (IFX_MMC_BASE_ADDR	+ 0x0008)
/** Command register */
#define MCI_CMD                 (IFX_MMC_BASE_ADDR	+ 0x000C)
#define MCI_CMD_INDEX_MASK      0x0000003F
#define MCI_CMD_INDEX_SET(arg)  ( (arg) & MC_CMD_INDEX_MASK) 
#define MCI_CMD_INDEX_GET(arg)  ( (arg) & MC_CMD_INDEX_MASK) 
#define MCI_CMD_RSP             BIT6    /*CPSM waits for a response */
#define MCI_CMD_LRSP            BIT7    /*CPSM receives a 136-bit long response */
#define MCI_CMD_INT             BIT8    /*CPSM disables command timer and waits for interrupt */
#define MCI_CPSM_INTERRUPT      BIT8    /*command interrupt enabled */
#define MCI_CMD_PEN             BIT9    /* CPSM waits for CMDpPend before it starts sending a command*/
#define MCI_CPSM_PENDING        BIT9    /* command pending enabled  */
#define MCI_CMD_EN              BIT10   /* Configures the CPSM */
#define MCI_CPSM_ENABLE         BIT10   /* command enabled */
#define MCI_CMD_NO_RSP          0
#define MCI_CMD_SHORT_RSP       (MCI_CMD_RSP)   /* short response flag */
#define MCI_CMD_LONG_RSP        (MCI_CMD_RSP | MCI_CMD_LRSP)    /*long response flag */

/** Response command register */
#define MCI_REPCMD              (IFX_MMC_BASE_ADDR	+ 0x0010)
#define MCI_RESPCMD_INDEX_MASK  0x0000003F
#define MCI_RESPCMD_INDEX_GET(arg)  ( (arg) & MCI_RESPCMD_INDEX_MASK)
/** Response 0 register */
#define MCI_REP0                (IFX_MMC_BASE_ADDR	+ 0x0014)
/** Response 1 register */
#define MCI_REP1                (IFX_MMC_BASE_ADDR	+ 0x0018)
/** Response 2 register */
#define MCI_REP2                (IFX_MMC_BASE_ADDR	+ 0x001C)
/** Response 3 register */
#define MCI_REP3                (IFX_MMC_BASE_ADDR	+ 0x0020)
/** Data timer register */
#define MCI_DTIM                (IFX_MMC_BASE_ADDR	+ 0x0024)
/** Data length register */
#define MCI_DLGTH               (IFX_MMC_BASE_ADDR	+ 0x0028)
#define MCI_DLGTH_MASK          0x0000FFFF
#define MCI_DLGTH_DL_GET(arg)   ( (arg) & MCI_DLGTH_MASK)
#define MCI_DLGTH_DL_SET(arg)   ( (arg) & MCI_DLGTH_MASK)

/** Data control register */
#define MCI_DCTRL               (IFX_MMC_BASE_ADDR	+ 0x002C)
/** Enables/Disables the data transfer */
#define MCI_DCTRL_EN            BIT0  

/** Configures the direction.(0B -->from controller to card,  1B-->from card to controller.*/
#define MCI_DCTRL_DIR            BIT1    

/** Controls the transfer mode.(0B --> block data transfer, 1B-->stream data transfer)*/
#define MCI_DCTRL_M             BIT2
/** Configures the DMA.(0B-->function is disabled, 1B->function is enabled)*/
#define MCI_DCTRL_DMA           BIT3 
#define MCI_DCTRL_BS(arg)       ( ((arg) & 0xF) << 4)
/** Data Counter Register (Returns the remaining data)*/
#define MCI_DCNT                (IFX_MMC_BASE_ADDR	+ 0x0030)
#define MCI_DCNT_GET(arg)       ( (arg) & 0xFFFF )
/** Status Register */
#define MCI_STAT                (IFX_MMC_BASE_ADDR	+ 0x0034)
/** Receive Data Available (Data available in receive FIFO)*/
#define MCI_STAT_RXDA           BIT21 
/** Transmit Data Available (Data available in transmit FIFO)*/
#define MCI_STAT_TXDA           BIT20
/*Receive FIFO Empty (Returns the Receive FIFO status) */
#define MCI_STAT_RXFE           BIT19
/** Transmit FIFO Empty (Returns the Receive FIFO status) */
#define MCI_STAT_TXFE           BIT18 
/** Receive FIFO Full (Returns the Receive FIFO status) */
#define MCI_STAT_RXFF           BIT17
/** Transmit FIFO Full (Returns the Transmit FIFO status)*/
#define MCI_STAT_TXFF           BIT16
/** Receive FIFO half full (Returns the FIFO status) */
#define MCI_STAT_RXHF           BIT15
/** Transmit FIFO half empty (Returns the FIFO status) */
#define MCI_STAT_TXHF           BIT14
/** Receive Active (Data receive in progress) */
#define MCI_STAT_RXA            BIT13
/** Transmit Active (Data transmit in progress) */
#define MCI_STAT_TXA            BIT12
/** Command Active (Command transfer in progress) */
#define MCI_STAT_CMDA           BIT11
/** Data Block End (Data block sent /received) */
#define MCI_STAT_DBE            BIT10
/** Start Bit Error (Start bit not detected on all data signals in wide bus mode) */
#define MCI_STAT_SBE            BIT9
/** Data End (i.e the data counter is zero) */
#define MCI_STAT_DE             BIT8
/** Command Sent (Command was sent. No response required )*/
#define MCI_STAT_CS             BIT7
/* Command Response End (Command response received. CRC check passed ) */
#define MCI_STAT_CRE            BIT6
/** Receive Overrun (Receive FIFO overrun error) */
#define MCI_STAT_RO             BIT5
/* Transmit Underrun (Transmit FIFO underrun error) */
#define MCI_STAT_TU             BIT4
/** Data Time-out (Data time-out indication) */
#define MCI_STAT_DTO            BIT3
/** Command Time-out (Command time-out indication) */
#define MCI_STAT_CTO            BIT2
/** Data CRC Fail (Data block send/received CRC check failed) */
#define MCI_STAT_DCF            BIT1
/** Command CRC Fail (Command response received, CRC check failed) */
#define MCI_STAT_CCF            BIT0

/** Interrupt mask clear register */
#define MCI_CL                  (IFX_MMC_BASE_ADDR	+ 0x0038)
/** Data Block End Clear (Data block sent /received) */
#define MCI_CL_DBEC             BIT10
/** Start Bit Error Clear (Start bit not detected on all data signals in wide bus mode) */
#define MCI_CL_SBEC             BIT9
/** Data End Clear (Data end indication means that the data counter is zero) */
#define MCI_CL_DEC              BIT8
/** Command Sent Clear (Command was sent. No response required) */
#define MCI_CL_CSC              BIT7
/** Command Response End Clear (Command response received. CRC check passed) */
#define MCI_CL_CREC             BIT6
/** Receive Overrun Clear (Receive FIFO overrun error) */
#define MCI_CL_ROC              BIT5
/** Transmit Underrun Clear (Transmit FIFO underrun error) */
#define MCI_CL_TUC              BIT4
/** Data Time-out Clear (Data time-out indication) */
#define MCI_CL_DTOC             BIT3
/** Command Time-out Clear (Command time-out indication) */
#define MCI_CL_CTOC             BIT2
/* Data CRC Fail Clear (Data block send/received CRC check failed) */
#define MCI_CL_DCFC             BIT1
/** Command CRC Fail Clear (Command response received, CRC check failed) */
#define MCI_CL_CCFC             BIT0

/** Interrupt Mask Register 0 */
#define MCI_IM0                 (IFX_MMC_BASE_ADDR	+ 0x003C)
/** Interrupt Mask Register 0 */
#define MCI_IM1                 (IFX_MMC_BASE_ADDR	+ 0x0040)
/** Mask Receive Data Available (Data available in receive FIFO)*/
#define MCI_IM_RXDA             BIT21 
/** Mask Transmit Data Available (Data available in transmit FIFO)*/
#define MCI_IM_TXDA             BIT20
/** Mask Receive FIFO Empty (Returns the Receive FIFO status) */
#define MCI_IM_RXFE             BIT19
/** Mask Transmit FIFO Empty (Returns the Receive FIFO status) */
#define MCI_IM_TXFE             BIT18 
/** Mask Receive FIFO Full (Returns the Receive FIFO status) */
#define MCI_IM_RXFF             BIT17
/** Mask Transmit FIFO Full (Returns the Transmit FIFO status)*/
#define MCI_IM_TXFF             BIT16
/** Mask Receive FIFO half full (Returns the FIFO status) */
#define MCI_IM_RXHF             BIT15
/** Mask Transmit FIFO half empty (Returns the FIFO status) */
#define MCI_IM_TXHF             BIT14
/** Mask Receive Active (Data receive in progress) */
#define MCI_IM_RXA              BIT13
/** Mask Transmit Active (Data transmit in progress) */
#define MCI_IM_TXA              BIT12
/** Mask Command Active (Command transfer in progress) */
#define MCI_IM_CMDA             BIT11
/** Mask Data Block End (Data block sent /received) */
#define MCI_IM_DBE              BIT10
/** Mask Start Bit Error (Start bit not detected on all data signals in wide bus mode) */
#define MCI_IM_SBE              BIT9
/** Mask Data End (i.e the data counter is zero) */
#define MCI_IM_DE               BIT8
/** Mask Command Sent (Command was sent. No response required )*/
#define MCI_IM_CS               BIT7
/** Mask Command Response End (Command response received. CRC check passed ) */
#define MCI_IM_CRE              BIT6
/** Mask Receive Overrun (Receive FIFO overrun error) */
#define MCI_IM_RO               BIT5
/** Mask Transmit Underrun (Transmit FIFO underrun error) */
#define MCI_IM_TU               BIT4
/** Mask Data Time-out (Data time-out indication) */
#define MCI_IM_DTO              BIT3
/** Mask Command Time-out (Command time-out indication) */
#define MCI_IM_CTO              BIT2
/** Mask Data CRC Fail (Data block send/received CRC check failed) */
#define MCI_IM_DCF              BIT1
/** Mask Command CRC Fail (Command response received, CRC check failed) */
#define MCI_IM_CCF              BIT0

#define MCI_IRQENABLE           \
        ( MCI_IM_CCF|MCI_IM_DCF \
        |MCI_IM_CTO| MCI_IM_DTO \
        |MCI_IM_TU|MCI_IM_RO    \
        |MCI_IM_CRE|MCI_IM_CS   \
        |MCI_IM_DBE | MCI_IM_SBE)

/** Secure Digital Memory Card Select */
#define MCI_SDMCS               (IFX_MMC_BASE_ADDR	+ 0x0044)
#define MCI_SDMCS_MASK          0x0000000F
#define MC_SDMCS_GET(arg)       ( (arg)&MCI_SDMCS_MASK)
#define MC_SDMCS_SET(arg)       ( (arg)&MCI_SDMCS_MASK)

/** FIFO Counter Register */
#define MCI_FC                  (IFX_MMC_BASE_ADDR	+ 0x0048)
#define MCI_FC_MASK             0x0000FFFF
#define MC_FC_GET(arg)          ( (arg)&MCI_FC_MASK)
#define MC_FC_SET(arg)          ( (arg)&MCI_FC_MASK)

/** Data FIFO Registers*/
#define MCI_DF0                 (IFX_MMC_BASE_ADDR	+ 0x0080)
#define MCI_DF1                 (IFX_MMC_BASE_ADDR	+ 0x0084)
#define MCI_DF2                 (IFX_MMC_BASE_ADDR	+ 0x0088)
#define MCI_DF3                 (IFX_MMC_BASE_ADDR	+ 0x008C)
#define MCI_DF4                 (IFX_MMC_BASE_ADDR	+ 0x0090)
#define MCI_DF5                 (IFX_MMC_BASE_ADDR	+ 0x0094)
#define MCI_DF6                 (IFX_MMC_BASE_ADDR	+ 0x0098)
#define MCI_DF7                 (IFX_MMC_BASE_ADDR	+ 0x009C)
#define MCI_DF8                 (IFX_MMC_BASE_ADDR	+ 0x00A0)
#define MCI_DF9                 (IFX_MMC_BASE_ADDR	+ 0x00A4)
#define MCI_DF10                (IFX_MMC_BASE_ADDR	+ 0x00A8)
#define MCI_DF11                (IFX_MMC_BASE_ADDR	+ 0x00AC)
#define MCI_DF12                (IFX_MMC_BASE_ADDR	+ 0x00B0)
#define MCI_DF13                (IFX_MMC_BASE_ADDR	+ 0x00B4)
#define MCI_DF14                (IFX_MMC_BASE_ADDR	+ 0x00B8)
#define MCI_DF15                (IFX_MMC_BASE_ADDR	+ 0x00BC)
/** Peripheral Identification Register */
#define MCI_PID0                (IFX_MMC_BASE_ADDR	+ 0x0FE0)
#define MCI_PID1                (IFX_MMC_BASE_ADDR	+ 0x0FE4)
#define MCI_PID2                (IFX_MMC_BASE_ADDR	+ 0x0FE8)
#define MCI_PID3                (IFX_MMC_BASE_ADDR	+ 0x0FEC)
/** MCI Core Identification Register */
#define MCI_PCID0               (IFX_MMC_BASE_ADDR	+ 0x0FF0)
#define MCI_PCID1               (IFX_MMC_BASE_ADDR	+ 0x0FF4)
#define MCI_PCID2               (IFX_MMC_BASE_ADDR	+ 0x0FF8)
#define MCI_PCID3               (IFX_MMC_BASE_ADDR	+ 0x0FFC)
/** SDIO Clock Control Register */
#define SDIO_CLC                (IFX_MMC_BASE_ADDR	+ 0x1000)
/** Clock Divider for Sleep Mode */
#define SDIO_CLC_SMC_MASK       0x00FF0000
#define SDIO_CLC_SMC_SET(arg)   ( ((arg)& 0xFF) << 16 )
#define SDIO_CLC_SMC_GET(arg)   ( ((arg)& SDIO_CLC_SMS_MASK) >> 16 )
/** Clock Divider for Normal Run Mode*/
#define SDIO_CLC_RMC_MASK       0x00000F00
#define SDIO_CLC_RMC_SET(arg)   ( ((arg)& 0xF) << 8 )
#define SDIO_CLC_RMC_GET(arg)   ( ((arg)& SDIO_CLC_RMS_MASK) >> 8)
/** Fast Shut-Off Enable Bit */
#define SDIO_CLC_FSOE           BIT5
/** Suspend Bit Write Enable for OCDS */
#define SDIO_CLC_SBWE           BIT4
/** External Request Disable */
#define SDIO_CLC_EDIS           BIT3
/** Suspend Enable Bit for OCDS  */
#define SDIO_CLC_SPEN           BIT2
/** Disable Status Bit */
#define SDIO_CLC_DISS_GET(arg)  ( ((arg) & 0x2) >> 1)
/** Disable Request Bit */
#define SDIO_CLC_DISR_SET(arg)  ( ((arg) & 0x1) )
/** SDIO Identification Register (0xF041C030)*/
#define SDIO_ID                 (IFX_MMC_BASE_ADDR	+ 0x1004)
/** SDIO Control Register */
#define SDIO_CTRL               (IFX_MMC_BASE_ADDR	+ 0x1008)
/** Read Wait Control (0B-->Read Wait signalling is stopped by releasing MCIDAT[2],
*   1B-->Read Wait is signalled in the next interrupt period, by pulling MCIDAT[2] low.
* Note: This only applies to interrupt periods within multiple block data transfers. */
#define SDIO_CTRL_RDWT          BIT4
/** SDIO External Direct Path Enable (0B-->SDIO functionality is enabled from GPIO,
*   1B-->SDIO functionality is enabled from external direct pin)*/
#define SDIO_CTRL_SDIO_EXT      BIT1
/** SDIO Enable (0B-->SDIO functionality disabled. Pure SD-Card functionality available,
*   1B-->SDIO functionality is enabled. SDIO IRQ and Read Wait is used)*/
#define SDIO_CTRL_SDIOEN        BIT0
/** SDIO Interrupt Mask Control Register */
#define SDIO_IMC                (IFX_MMC_BASE_ADDR	+ 0x1010)      
/** SDIO Interrupt Mask (1B-->SDIO interrupt is enabled,0B->SDIO interrupt is disabled)*/
#define SDIO_IMC_SDIO           BIT2
/** INTR1 Interrupt Mask (1B-->INTR1 interrupt is enabled,0B-->INTR1 interrupt is disabled)*/
#define SDIO_IMC_INTR1          BIT1
/** INTR0 Interrupt Mask (1B-->INTR0 interrupt is enabled (from PL180),
* 0B-->INTR0 interrupt is disabled (from PL180))*/
#define SDIO_IMC_INTR0          BIT0
/** SDIO Raw Interrupt Status Register */
#define SDIO_RIS                (IFX_MMC_BASE_ADDR	+ 0x100C)       
/** SDIO Interrupt Status (0B-->no SDIO interrupt,1B-->SDIO interrupt)*/
#define SDIO_RIS_SDIO_GET(arg)  ( ((arg) >> 2) & 0x1)
/** INTR1 Interrupt Status (0B-->no SDIO INTR1 interrupt,1B-->SDIO INTR1 interrupt)*/
#define SDIO_RIS_INTR1_GET(arg) ( (arg) >> 1) & 0x1)
/** INTR0 Interrupt Status (0B-->no SDIO INTR0 interrupt,1B-->SDIO INTR0 interrupt)*/
#define SDIO_RIS_INTR0_GET(arg) ( (arg) & 0x1 )
/** SDIO Masked Interrupt Status Register */
#define SDIO_MIS                (IFX_MMC_BASE_ADDR	+ 0x1014)
/** SDIO Masked Interrupt Status (0B-->No SDIO interrupt, 1B--> SDIO interrupt)*/
#define SDIO_MIS_SDIO_GET(arg)  ( ((arg) >> 2) & 0x1)
/** INTR1 Masked Interrupt Status (0B-->No SDIO INTR1 interrupt,1B-->SDIO INTR1 interrupt)*/
#define SDIO_MIS_INTR1_GET(arg) ( (arg) >> 1) & 0x1)
/** INTR0 Masked Interrupt Status (0B -->No SDIO INTR0 interrupt, 1B-->SDIO INTR0 interrupt)*/
#define SDIO_MIS_INTR0_GET(arg) ( (arg) & 0x1 )
/** SDIO Interrupt Clear Register */
#define SDIO_ICR                (IFX_MMC_BASE_ADDR	+ 0x1018)
/** SDIO Interrupt Clear (0B--> No Action,1B -->Clears SDIO interrupt)*/
#define SDIO_ICR_SDIO_SET(arg)  ( ((arg) & 0x1 ) << 0x2) 
/** INTR1 Interrupt Clear (0B-->No Action,1B-->Clears SDIO INTR1 interrupt)*/
#define SDIO_ICR_INTR1_SET(arg)  ( ((arg) & 0x1 ) << 0x1) 
/** INTR0 Interrupt Clear (0B--> No Action,1B -->Clears SDIO INTR0 interrupt)*/
#define SDIO_ICR_INTR0_SET(arg)  ( ((arg) & 0x1 ) ) 
/** SDIO Interrupt Set Register */
#define SDIO_ISR                (IFX_MMC_BASE_ADDR	+ 0x101C)
/*SDIO Interrupt Set (0B--> No Action,1B-->Sets SDIO interrupt)*/
#define SDIO_ISR_SDIO_SET(arg)  ( ((arg) & 0x1 ) << 0x2) 
/** INTR1 Interrupt Set (0B--> No Action,1B-->Sets SDIO INTR1 interrupt)*/
#define SDIO_ISR_INTR1_SET(arg)  ( ((arg) & 0x1 ) << 0x1) 
/** INTR0 Interrupt Set (0B--> No Action,1B--> Sets SDIO INTR0 interrupt)*/
#define SDIO_ISR_INTR0_SET(arg)  ( ((arg) & 0x1 ) ) 
/** SDIO DMA Control Register */
#define SDIO_DMACON             (IFX_MMC_BASE_ADDR	+ 0x1020)
/** brief DMA Tx path on */
#define SDIO_DMACON_TXON	BIT1
/** brief DMA Rx path on */
#define SDIO_DMACON_RXON	BIT0

/** Receive Class
* Programmable class specifier to be sent to the DMA controller with every received word.
*/
#define SDIO_DMACON_RXCLS_MASK      0x0000000C
#define SDIO_DMACON_RXCLS_GET(arg)  ( ((arg)>>2) & 0x3 )
#define SDIO_DMACON_RXCLS_SET(arg)  ( ((arg) & 0x3) << 0x2)

/** Enables/Disables the transmit path.(0B-->Transmit Path DMA is off,1B -->Transmit Path DMA is on)*/
#define SDIO_DMACON_TXON_SET(arg)   ( ((arg) & 0x1 ) << 0x1 ) 
#define SDIO_DMACON_TXON_GET(arg)   ( ((arg) >> 0x1) & 0x1 ) 
/** Enables/Disables the receive path (0B-->Receive Path DMA is off,1B-->Receive Path DMA is on)*/
#define SDIO_DMACON_RXON_SET(arg)   ( (arg) & 0x1 )  
#define SDIO_DMACON_RXON_GET(arg)   ( (arg)  & 0x1 ) 

#endif /*_IFXMIPS_MMCREG_H_ */
