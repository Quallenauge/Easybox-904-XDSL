/**
** FILE NAME    : ifxmips_dma_reg.h
** PROJECT      : IFX UEIP
** MODULES      : Central DMA 
** DATE         : 03 June 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : IFX Cross-Platform Central DMA driver header file
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
** $Date 			$Author			$Comment
** 03 June 2009		Reddy Mallikarjuna		Initial release
*******************************************************************************/
#ifndef _IFXMIPS_DMA_REG_H_
#define _IFXMIPS_DMA_REG_H_
/*!
  \file ifxmips_dma_reg.h
  \ingroup IFX_DMA_CORE
  \brief IFX Central DMA module register definition
*/

/** =============================*/
/* Includes                      */
/** =============================*/

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
/** Register definitions */
#define DMA_BASE_ADDR                   (0xBE104100)
/** Clock control register */
#define IFX_DMA_CLC_REG                 (volatile u32*)(DMA_BASE_ADDR + 0x00)
/** Module ID register */ 
#define IFX_DMA_ID_REG                  (volatile u32*)(DMA_BASE_ADDR + 0x08)
/** DMA control register */
#define IFX_DMA_CTRL_REG                (volatile u32*)(DMA_BASE_ADDR + 0x10)
/** Global Software reset */
#define DMA_CTRL_RST                    0x00000001	
/**Descriptor read back (supported for VR9)*/
#define DMA_CTRL_DRB                    0x00000100  
/** Descriptor read back (supported for VR9) */
#define DMA_CTRL_MBRSTCNT_MASK          0x03FF0000 
/** MUltiple Burst Arbitration (supported for VR9) */
#define DMA_CTRL_MBRSTARB               0x40000000 
/** Packet Arbitration (supported for VR9) */
#define DMA_CTRL_PKTARB                 0x80000000  	
/** channel polling register*/
#define IFX_DMA_CPOLL_REG               (volatile u32*)(DMA_BASE_ADDR + 0x14)
/**Enable Descriptor polling*/ 
#define DMA_CPOLL_EN                    0x80000000	
/**Enable Descriptor polling*/ 
#define DMA_CPOLL_CNT_MASK              0x0000FFF0	
#define DMA_CPOLL_CNT_VAL(val)          ( ((val) & 0xFFF) << 4)
/** Channel Related registers */
/** channel select register */
#define DMA_CS0                         (volatile u32*)(DMA_BASE_ADDR + 0x18 )
#define IFX_DMA_CS_REG                  DMA_CS0
#define DMA_CS_MASK                     0x0000001F
#define DMA_CS_VAL_GET(val)             (((val) & 0x1F))	
#define DMA_CS_VAL_SET(val)             (((val) & 0x1F))	
/** channel control register */
#define DMA_CCTRL0                      (volatile u32*)(DMA_BASE_ADDR + 0x1C )
#define IFX_DMA_CCTRL_REG               DMA_CCTRL0
/** Channel On/ Off */
#define DMA_CCTRL_ON                    0x00000001 	
/** Channel reset */
#define DMA_CCTRL_RST                   0x00000002 	
/** direction of the channel*/
#define DMA_CCTRL_DIR                   0x00000100 	
#if defined(CONFIG_VR9)
/** Class mask value*/
#define DMA_CCTRL_CLASS_MASK            0x00000E00	
#define DMA_CCTRL_CLASS_VAL_SET(val)    (((val) & 0x7) << 9)
#define DMA_CCTRL_CLASS_VAL_GET(val)    ((((val) & DMA_CCTRL_CLASS_MASK) >> 9) & 0x7)
/** Port assignment mask*/
#define DMA_CCTRL_PRTNR_MASK			0x0000F000	
#define DMA_CCTRL_PRYNR_GET(val)    	((((val) & DMA_CCTRL_PRTNR_MASK) >> 12) & 0xF)
#else
/** Class mask value*/
#define DMA_CCTRL_CLASS_MASK			0x00000C00	
#define DMA_CCTRL_CLASS_VAL_SET(val)    (((val) & 0x3) << 9)
#define DMA_CCTRL_CLASS_VAL_GET(val)    ((((val) & DMA_CCTRL_CLASS_MASK) >> 9) & 0x3)
/** Port assignment mask*/
#define DMA_CCTRL_PRTNR_MASK			0x00003800	
#define DMA_CCTRL_PRYNR_GET(val)    	((((val) & DMA_CCTRL_PRTNR_MASK) >> 11) & 0x7)
#endif /*CONFIG_VR9) */
/** channel weight for Tx direction */
#define DMA_CCTRL_TXWGT_MASK			0x00030000	
#define DMA_CCTRL_TXWGT_VAL(val)       	(((val) & 0x3) << 16)
#define DMA_CCTRL_TXWGT_GET(val)        ((((val) & DMA_CCTRL_TXWGT_MASK) >> 16) & 0x3)
/** Packet drop enabled */
#define DMA_CCTRL_PDEN					0x00800000
#define DMA_CCTRL_PDEN_GET(val)			((((val) &DMA_CCTRL_PDEN) >> 23) & 0x1)
/* Peripheral to Peripheral copy enable*/
#define DMA_CCTRL_P2PCPY				0x01000000
#define DMA_CCTRL_P2PCPY_GET(val)       ((((val) & DMA_CCTRL_P2PCPY) >> 24) & 0x1)
/** loopback enable */
#define DMA_CCTRL_LBEN					0x02000000	
/** Loopback channel number mask*/
#define DMA_CCTRL_LBCHNR_MASK			0xFC000000	
#define DMA_CCTRL_LBCHNR_SET(val)		( ((val) & 0x3F) << 26)
#define DMA_CCTRL_LBCHNR_GET(val)		( (((val) & DMA_CCTRL_LBCHNR_MASK) >> 26) & 0x3F)

/** Channel descriptor base address register */
#define DMA_CDBA0                       (volatile u32*)(DMA_BASE_ADDR + 0x20 )
#define IFX_DMA_CDBA_REG                DMA_CDBA0
/** Channel descriptor length register */
#define DMA_CDLEN0                      (volatile u32*)(DMA_BASE_ADDR + 0x24 )
#define IFX_DMA_CDLEN_REG               DMA_CDLEN0
#define DMA_CDLEN_MASK					0x000000FF
#define DMA_CDLEN_VAL_SET(val)			(((val) & 0xFF) ) 
#define DMA_CDLEN_VAL_GET(val)			(((val) & 0xFF) ) 

/** Channel interrupt status register */
#define DMA_CIS0                        (volatile u32*)(DMA_BASE_ADDR + 0x28 )
#define IFX_DMA_CIS_REG                 DMA_CIS0
/* Channel Interrupt Status Register  */
/** End of packet interrupt */
#define DMA_CIS_EOP  	 				0x00000002	
/** Descriptor Under-Run Interrupt  */
#define DMA_CIS_DUR 					0x00000004	
/** Descriptor Complete Interrupt  */
#define DMA_CIS_DESCPT 					0x00000008	
/** Channel Off Interrupt  */
#define DMA_CIS_CHOFF   				0x00000010	
/** SAI Read Error Interrupt */
#define DMA_CIS_RDERR 					0x00000020	
/** all interrupts */
#define DMA_CIS_ALL     				( DMA_CIS_EOP		\
										| DMA_CIS_DUR 		\
										| DMA_CIS_DESCPT 	\
										| DMA_CIS_CHOFF 	\
										| DMA_CIS_RDERR	)

/** Channel interrupt enable register */
#define DMA_CIE0                        (volatile u32*)(DMA_BASE_ADDR + 0x2C )
#define IFX_DMA_CIE_REG                 DMA_CIE0

/**  Channel Interrupt Enable Register */
/** End of packet interrupt enable */
#define DMA_CIE_EOP 	 	 			0x00000002	
/** Descriptor Under-Run Interrupt enable  */
#define DMA_CIE_DUR                     0x00000004	
/** Descriptor Complete Interrupt  enable*/
#define DMA_CIE_DESCPT 					0x00000008
/** Channel Off Interrupt enable */
#define DMA_CIE_CHOFF   				0x00000010	
/** SAI Read Error Interrupt enable*/
#define DMA_CIE_RDERR 					0x00000020

/** End of Packet interrupt mask */
#define IFX_DMA_CIE_EOP_MASK			0x2
/** Descriptor under run interrupt mask */
#define IFX_DMA_CIE_DUR_MASK            0x4
/** Descriptor complete interrupt mask */
#define IFX_DMA_CIE_DUR_DESCPT_MASK     0x8
/** Dma channel off interrupt mask */
#define IFX_DMA_CIE_CHOFF_MSK           0x10
/** SAI Read error interrupt  mask*/
#define IFX_DMA_CIE_RDERR_MSK			0x20
/** enable all interrupts */
#define DMA_CIE_ALL                     (DMA_CIE_EOP 		\
										| DMA_CIE_DUR 		\
										| DMA_CIE_DESCPT 	\
										| DMA_CIE_CHOFF		\
										| DMA_CIE_RDERR	)
/** Channel Global buffer length register (not supported for Danube)*/
#define IFX_DMA_CGBL_REG                (volatile u32*)(DMA_BASE_ADDR + 0x30)
#define DMA_CGBL_MASK					0x0000FFFF
#define DMA_CGBL_SET_VAL(val)       	(((val) & 0xFFFF))
#define DMA_CGBL_GET_VAL(val)       	(((val) & 0xFFFF) )
/** DMA Current Descriptor Pointer Register (Supports only VR9)*/
#define IFX_DMA_CDPTNRD_REG             (volatile u32*)(DMA_BASE_ADDR + 0x34)
/** Port Related registers */
/** Port select registers */
#define DMA_PS0                         (volatile u32*)(DMA_BASE_ADDR + 0x40 )
#define IFX_DMA_PS_REG                  DMA_PS0
/** Port select mask*/
#define DMA_PS_PS_MASK					0x00000007	
#define DMA_PS_VAL_SET(val)   			(((val) & 0x7) )
#define DMA_PS_VAL_GET(val)   			(((val) & 0x7) )
#define DMA_PCTRL0                      (volatile u32*)(DMA_BASE_ADDR + 0x44 )
#define IFX_DMA_PCTRL_REG               DMA_PCTRL0
/** Port Control bit mask*/
/** General purpose control, only used to memcopy port*/
#define DMA_PCTRL_GPC 					0x00010000	
#define DMA_PCTRL_GPC_VAL_SET(val)      (((val) & 0x1) << 16)
#define DMA_PCTRL_GPC_VAL_GET(val)      (((val) & DMA_PCTRL_GPC) >> 16)

/** Port Endianness for Tranmit Direction  */
#define DMA_PCTRL_TXENDI_SET_VAL(val)   (((val) & 0x3) << 10)
/** Port Endianness for Receive Direction  */
#define DMA_PCTRL_RXENDI_SET_VAL(val)   (((val) & 0x3) << 8)
/** Port Burst Length for Transmit Direction */
#define DMA_PCTRL_TXBL_SET_VAL(val)     (((val) & 0x3) << 4)
/* Port Burst Length for Receive Direction */
#define DMA_PCTRL_RXBL_SET_VAL(val)     (((val) & 0x3) << 2)
/* Port Weight for Transmit Direction  */
#define DMA_PCTRL_TXWGT_SET_VAL(val)    (((val) & 0x7) << 12)
/* Port packet drop enabled*/
#define DMA_PCTRL_PDEN_SET_VAL(val)     (((val) & 0x1) << 6)

/** Interrupt Related registers */
/** Interrupt node enable register */
#define IFX_DMA_IRNEN_REG               (volatile u32*)(DMA_BASE_ADDR + 0xf4)
/** Interrupt node control register */
#define IFX_DMA_IRNCR_REG               (volatile u32*)(DMA_BASE_ADDR + 0xf8)
/** Interrupt capture register */
#define IFX_DMA_IRNICR_REG              (volatile u32*)(DMA_BASE_ADDR + 0xfc)

/** DMA descriptor bitfields */
/** Descriptor owner bit indication*/
/** Descriptor is owned by DMA */ 
#define IFX_DMA_OWN                     1
/** Descriptor is owned by CPU */
#define IFX_CPU_OWN                     0
/** owner bit belongs to CPU*/
#define IFX_DMA_DESC_OWN_CPU            0x0
/** owner bit belongs to DMA */
#define IFX_DMA_DESC_OWN_DMA            0x80000000
/** Descriprot Complete */
#define IFX_DMA_DESC_CPT_SET            0x40000000
/** Start of Packet */
#define IFX_DMA_DESC_SOP_SET            0x20000000
/** End of Packet */
#define IFX_DMA_DESC_EOP_SET            0x10000000

/** Descriptor byte offset */
#define IFX_DMA_DESC_BYTE_OFFSET        23
/** Receive Descriptor byte offset */
#define IFX_DMA_RX_DESC_BYTE_OFFSET(val)    ((val &0x3) << IFX_DMA_DESC_BYTE_OFFSET)
/** Transmit Descriptor byte offset */
#define IFX_DMA_TX_DESC_BYTE_OFFSET(val)    ((val &0x1f) << IFX_DMA_DESC_BYTE_OFFSET)


#define DMA_IRQ_SPIN_LOCK_INIT(x)           spin_lock_init(&((x)->irq_lock))                    
#define DMA_IRQ_SPIN_LOCK_DESTROY(x)
#define DMA_IRQ_SPIN_LOCK(x) do {                   \
    unsigned long __ilockflags;                     \
    spin_lock_irqsave(&((x)->irq_lock), __ilockflags);
#define DMA_IRQ_SPIN_UNLOCK(x)                      \
    spin_unlock_irqrestore(&((x)->irq_lock), __ilockflags); \
} while (0)

#define DMA_IRQ_LOCK(X) do {                \
    unsigned long __ilockflags;             \
    local_irq_save(__ilockflags);
#define DMA_IRQ_UNLOCK(X)                   \
    local_irq_restore( __ilockflags);       \
} while (0)

#define IFX_DMA_WRREG_PROT(chan,addr,value) \
    do {                                    \
        unsigned int tcs;                   \
        DMA_IRQ_LOCK((chan));               \
        tcs = IFX_REG_R32(IFX_DMA_CS_REG);  \
	IFX_REG_W32(chan, IFX_DMA_CS_REG);	    \
        *addr = value;                      \
	IFX_REG_W32(tcs, IFX_DMA_CS_REG);	    \
        DMA_IRQ_UNLOCK((chan));             \
    } while ( 0 )

#define IFX_DMA_RDREG_PROT(chan,addr,value) \
    do {                                    \
        unsigned int tcs;                   \
        DMA_IRQ_LOCK((chan));               \
        tcs = IFX_REG_R32(IFX_DMA_CS_REG);  \
	IFX_REG_W32(chan, IFX_DMA_CS_REG);      \
        value = *addr;                      \
	IFX_REG_W32(tcs, IFX_DMA_CS_REG);       \
        DMA_IRQ_UNLOCK((chan));             \
    } while ( 0 )

#define IFX_DMA_ORREG_PROT(chan,addr,value) \
    do {                                    \
        unsigned int tcs;                   \
        DMA_IRQ_LOCK((chan));               \
        tcs = IFX_REG_R32(IFX_DMA_CS_REG);  \
	IFX_REG_W32(chan, IFX_DMA_CS_REG);	    \
        *addr |= value;                     \
	IFX_REG_W32(tcs, IFX_DMA_CS_REG);	    \
        DMA_IRQ_UNLOCK((chan));             \
    } while ( 0 )

#define IFX_DMA_ANDREG_PROT(chan,addr,value)\
    do {                                    \
        u32 tcs;                            \
        DMA_IRQ_LOCK((chan));               \
        tcs = IFX_REG_R32(IFX_DMA_CS_REG);  \
	IFX_REG_W32(chan, IFX_DMA_CS_REG);      \
        *addr &= value;                     \
	IFX_REG_W32(tcs, IFX_DMA_CS_REG);       \
         DMA_IRQ_UNLOCK((chan));            \
    } while ( 0 )

/**
** Default Transmit/Receive endiannes type 
** B0_B1_B2_B3 No byte switching 
*/
#define IFX_DMA_DEFAULT_TX_ENDIANNESS       IFX_DMA_ENDIAN_TYPE3
#define IFX_DMA_DEFAULT_RX_ENDIANNESS       IFX_DMA_ENDIAN_TYPE3
/** channel weight value*/
#define IFX_DMA_CH_DEFAULT_WEIGHT           100	
/** Default Port Transmit weight value */
#define IFX_DMA_TX_PORT_DEFAULT_WEIGHT      1
/** Default Port Transmit weight value */
#define IFX_DMA_TX_CHAN_DEFAULT_WEIGHT      1
/** Default packet drop enabled for port */
#define IFX_DMA_DEF_PORT_BASED_PKT_DROP_EN  IFX_DISABLE  
/** Default packet drop enabled for channel   */
#define IFX_DMA_DEF_CHAN_BASED_PKT_DROP_EN  IFX_DISABLE
/** Default Transmit burst length  */
#define IFX_DMA_PORT_DEFAULT_TX_BURST_LEN   IFX_DMA_BURSTL_2
/** Default Receive burst length  */
#define IFX_DMA_PORT_DEFAULT_RX_BURST_LEN   IFX_DMA_BURSTL_2

/** default enabled interrupts */
#define DMA_CIE_DEFAULT                     ( DMA_CIE_DESCPT    \
                                            | DMA_CIE_EOP )
/** disable all interrupts */
#define DMA_CIE_DISABLE_ALL                 0 
/** default channel polling interval value*/
#define DMA_DEFAULT_POLL_VALUE              4

#ifdef CONFIG_CPU_LITTLE_ENDIAN
typedef struct rx_desc {
    volatile unsigned int Data_Pointer;                 /* Descriptor data pointer */
    union {
        struct {
            volatile unsigned int data_length   :16;    /* descriptor  data lenght */
            volatile unsigned int reserved16_22 :7;
            volatile unsigned int byte_offset   :2;     /* Byte offset field */
            volatile unsigned int reserve_25_27 :3;
            volatile unsigned int EoP           :1;     /*End of Packet flag */
            volatile unsigned int SoP           :1;     /* Start of the Packet flag */
            volatile unsigned int C             :1;     /*Descripter complete flag */
            volatile unsigned int OWN           :1;     /*Owner bit indication */
        } field;
        volatile unsigned int word;
    } status;
}_rx_desc;
typedef struct tx_desc {
    volatile unsigned int Data_Pointer;                 /* Descriptor data pointer */
    union {
        struct {
            volatile unsigned int data_length   :16;    /* descriptor  data lenght */
            volatile unsigned int reserved      :7;
            volatile unsigned int byte_offset   :5;     /* Byte offset field */
            volatile unsigned int EoP           :1;     /*End of Packet flag */
            volatile unsigned int SoP           :1;     /* Start of the Packet flag */
            volatile unsigned int C             :1;     /*Descripter complete flag */
            volatile unsigned int OWN           :1;     /*Owner bit indication */
        }field;
        volatile unsigned int word;
    } field;
}_tx_desc;
#else 
typedef struct rx_desc {
    union {
        struct {
            volatile unsigned int OWN           :1;     /*Owner bit indication */
            volatile unsigned int C             :1;
            volatile unsigned int SoP           :1;     /* Start of the Packet flag */
            volatile unsigned int EoP           :1;     /*End of Packet flag */
            volatile unsigned int reserve_25_27 :3;
            volatile unsigned int byte_offset   :2;     /* Byte offset field */
            volatile unsigned int reserve16_22  :7;     /*Descripter complete flag */
            volatile unsigned int data_length   :16;    /* descriptor  data lenght */
        }field;
        volatile unsigned int word;
    } status;
    volatile unsigned int Data_Pointer;                 /* Descriptor data pointer */
} _rx_desc;

typedef struct tx_desc {
    union {
        struct {
            volatile unsigned int OWN           :1;     /*Owner bit indication */
            volatile unsigned int C             :1;
            volatile unsigned int SoP           :1;     /* Start of the Packet flag */
            volatile unsigned int EoP           :1;     /*End of Packet flag */
            volatile unsigned int byte_offset   :5;     /* Byte offset field */
            volatile unsigned int reserved      :7;     /*Descripter complete flag */
            volatile unsigned int data_length   :16;    /* descriptor  data lenght */
        } field;
        volatile unsigned int word;
    } status;
    volatile unsigned int Data_Pointer;             /* Descriptor data pointer */
}_tx_desc;

#endif    /*CONFIG_CPU_LITTLE_ENDIAN  */
#endif /* _IFXMIPS_DMA_REG_H_ */
