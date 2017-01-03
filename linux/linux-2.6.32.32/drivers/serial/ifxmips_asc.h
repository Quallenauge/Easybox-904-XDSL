/******************************************************************************
**
** FILE NAME    : ifxmips_asc.h
** PROJECT      : IFX UEIP
** MODULES      : ASC (UART)
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global IFX ASC (UART) driver header file
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
** $Date        $Author         $Comment
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



#ifndef IFX_ASC_H
#define IFX_ASC_H



/* channel operating modes */
#define ASCOPT_CSIZE                    0x00000003
#define ASCOPT_CS7                      0x00000001
#define ASCOPT_CS8                      0x00000002
#define ASCOPT_PARENB                   0x00000004
#define ASCOPT_STOPB                    0x00000008
#define ASCOPT_PARODD                   0x00000010
#define ASCOPT_CREAD                    0x00000020

#define ASC_OPTIONS                     (ASCOPT_CREAD | ASCOPT_CS8)

/* ASC input select (0 or 1) */
#define CONSOLE_TTY                     0

#define IFX_ASC_TXFIFO_FL               1
#define IFX_ASC_RXFIFO_FL               1
#define IFX_ASC_TXFIFO_FULL             16

/* interrupt lines masks for the ASC device interrupts*/
/* change these macroses if it's necessary */
#define IFX_ASC_IRQ_LINE_ALL            0x000000FF      /* all IRQs */
#define IFX_ASC_IRQ_LINE_MASK_ALL       0x00000000      /* disable/mask all IRQs */

#define IFX_ASC_IRQ_LINE_TIR            0x00000001      /* Tx Int */
#define IFX_ASC_IRQ_LINE_RIR            0x00000002      /* Rx Int */
#define IFX_ASC_IRQ_LINE_EIR            0x00000004      /* Error Int */
#define IFX_ASC_IRQ_LINE_TBIR           0x00000008      /* Tx Buffer Int */
#define IFX_ASC_IRQ_LINE_ABSTIR         0x00000010      /* Autobaud Start Int */
#define IFX_ASC_IRQ_LINE_ABDETIP        0x00000020      /* Autobaud Detection Int */
#define IFX_ASC_IRQ_LINE_MSIR           0x00000040      /* Modem Status Int */
#define IFX_ASC_IRQ_LINE_SFCIR          0x00000080      /* Software Flow Control Int */

/* ID register's bits and bitfields */
#define ASCID_TX32                      0x80000000
#define ASCID_TXFS_MASK                 0x3F000000
#define ASCID_TXFS_OFF                  24
#define ASCID_RX32                      0x00800000
#define ASCID_RXFS_MASK                 0x003F0000
#define ASCID_RXFS_OFF                  16
#define ASCID_ID_MASK                   0x0000FF00
#define ASCID_ID_OFF                    8
#define ASCID_AB                        0x00000080
#define ASCID_AIDA                      0x00000040
#define ASCID_DMA                       0x00000020
#define ASCID_REV_MASK                  0x0000001F
#define ASCID_REV_OFF                   0

/* CLC register's bits and bitfields */
#define ASCCLC_DISR                     0x00000001
#define ASCCLC_DISS                     0x00000002
#define ASCCLC_RMCMASK                  0x0000FF00
#define ASCCLC_RMCOFFSET                8

/* CON register's bits and bitfields */
#define ASCMCON_MODEMASK                0x0000000f
#define ASCMCON_M_8ASYNC                0x0
#define ASCMCON_M_8IRDA                 0x1
#define ASCMCON_M_7ASYNC                0x2
#define ASCMCON_M_7IRDA                 0x3
#define ASCMCON_WLSMASK                 0x0000000c
#define ASCMCON_WLSOFFSET               2
#define ASCMCON_WLS_8BIT                0x0
#define ASCMCON_WLS_7BIT                0x1
#define ASCMCON_PEN                     0x00000010
#define ASCMCON_ODD                     0x00000020
#define ASCMCON_SP                      0x00000040
#define ASCMCON_STP                     0x00000080
#define ASCMCON_BRS                     0x00000100
#define ASCMCON_FDE                     0x00000200
#define ASCMCON_ERRCLK                  0x00000400
#define ASCMCON_EMMASK                  0x00001800
#define ASCMCON_EMOFFSET                11
#define ASCMCON_EM_ECHO_OFF             0x0
#define ASCMCON_EM_ECHO_AB              0x1
#define ASCMCON_EM_ECHO_ON              0x2
#define ASCMCON_LB                      0x00002000
#define ASCMCON_ACO                     0x00004000
#define ASCMCON_R                       0x00008000
#define ASCMCON_PAL                     0x00010000
#define ASCMCON_FEN                     0x00020000
#define ASCMCON_RUEN                    0x00040000
#define ASCMCON_ROEN                    0x00080000
#define ASCMCON_TOEN                    0x00100000
#define ASCMCON_BEN                     0x00200000
#define ASCMCON_TXINV                   0x01000000
#define ASCMCON_RXINV                   0x02000000
#define ASCMCON_TXMSB                   0x04000000
#define ASCMCON_RXMSB                   0x08000000

/* STATE register's bits and bitfields */
#define ASCSTATE_REN                    0x00000001
#define ASCSTATE_PE                     0x00010000
#define ASCSTATE_FE                     0x00020000
#define ASCSTATE_RUE                    0x00040000
#define ASCSTATE_ROE                    0x00080000
#define ASCSTATE_TOE                    0x00100000
#define ASCSTATE_BE                     0x00200000
#define ASCSTATE_TXBVMASK               0x07000000
#define ASCSTATE_TXBVOFFSET             24
#define ASCSTATE_TXEOM                  0x08000000
#define ASCSTATE_RXBVMASK               0x70000000
#define ASCSTATE_RXBVOFFSET             28
#define ASCSTATE_RXEOM                  0x80000000
#define ASCSTATE_ANY                    (ASCSTATE_PE|ASCSTATE_FE|ASCSTATE_ROE)

/* WHBSTATE register's bits and bitfields */
#define ASCWHBSTATE_CLRREN              0x00000001
#define ASCWHBSTATE_SETREN              0x00000002
#define ASCWHBSTATE_CLRPE               0x00000004
#define ASCWHBSTATE_CLRFE               0x00000008
#define ASCWHBSTATE_CLRRUE              0x00000010
#define ASCWHBSTATE_CLRROE              0x00000020
#define ASCWHBSTATE_CLRTOE              0x00000040
#define ASCWHBSTATE_CLRBE               0x00000080
#define ASCWHBSTATE_SETPE               0x00000100
#define ASCWHBSTATE_SETFE               0x00000200
#define ASCWHBSTATE_SETRUE              0x00000400
#define ASCWHBSTATE_SETROE              0x00000800
#define ASCWHBSTATE_SETTOE              0x00001000
#define ASCWHBSTATE_SETBE               0x00002000

/* ABCON register's bits and bitfields */
#define ASCABCON_ABEN                   0x0001
#define ASCABCON_AUREN                  0x0002
#define ASCABCON_ABSTEN                 0x0004
#define ASCABCON_ABDETEN                0x0008
#define ASCABCON_FCDETEN                0x0010

/* FDV register mask, offset and bitfields*/
#define ASCFDV_VALUE_MASK               0x000001FF

/* WHBABCON register's bits and bitfields */
#define ASCWHBABCON_CLRABEN             0x0001
#define ASCWHBABCON_SETABEN             0x0002

/* ABSTAT register's bits and bitfields */
#define ASCABSTAT_FCSDET                0x0001
#define ASCABSTAT_FCCDET                0x0002
#define ASCABSTAT_SCSDET                0x0004
#define ASCABSTAT_SCCDET                0x0008
#define ASCABSTAT_DETWAIT               0x0010

/* WHBABSTAT register's bits and bitfields */
#define ASCWHBABSTAT_CLRFCSDET          0x0001
#define ASCWHBABSTAT_SETFCSDET          0x0002
#define ASCWHBABSTAT_CLRFCCDET          0x0004
#define ASCWHBABSTAT_SETFCCDET          0x0008
#define ASCWHBABSTAT_CLRSCSDET          0x0010
#define ASCWHBABSTAT_SETSCSDET          0x0020
#define ASCWHBABSTAT_CLRSCCDET          0x0040
#define ASCWHBABSTAT_SETSCCDET          0x0080
#define ASCWHBABSTAT_CLRDETWAIT         0x0100
#define ASCWHBABSTAT_SETDETWAIT         0x0200

/* TXFCON register's bits and bitfields */
#define ASCTXFCON_TXFEN                 0x0001
#define ASCTXFCON_TXFFLU                0x0002
#define ASCTXFCON_TXFITLMASK            0x3F00
#define ASCTXFCON_TXFITLOFF             8

/* RXFCON register's bits and bitfields */
#define ASCRXFCON_RXFEN                 0x0001
#define ASCRXFCON_RXFFLU                0x0002
#define ASCRXFCON_RXFITLMASK            0x3F00
#define ASCRXFCON_RXFITLOFF             8

/* FSTAT register's bits and bitfields */
#define ASCFSTAT_RXFFLMASK              0x003F
#define ASCFSTAT_TXFFLMASK              0x3F00
#define ASCFSTAT_TXFFLOFF               8
#define ASCFSTAT_RXFREEMASK             0x003F0000
#define ASCFSTAT_TXFREEMASK             0x3F000000
#define ASCFSTAT_TXFREEOFF              24

typedef struct {        /* ifx_asc_reg_t */
    volatile unsigned long  asc_clc;                            /*0x0000*/
    volatile unsigned long  asc_pisel;                          /*0x0004*/
    volatile unsigned long  asc_id;                             /*0x0008*/
    volatile unsigned long  asc_rsvd1[1];   /* for mapping */   /*0x000C*/
    volatile unsigned long  asc_mcon;                           /*0x0010*/
    volatile unsigned long  asc_state;                          /*0x0014*/
    volatile unsigned long  asc_whbstate;                       /*0x0018*/
    volatile unsigned long  asc_rsvd2[1];   /* for mapping */   /*0x001C*/
    volatile unsigned long  asc_tbuf;                           /*0x0020*/
    volatile unsigned long  asc_rbuf;                           /*0x0024*/
    volatile unsigned long  asc_rsvd3[2];   /* for mapping */   /*0x0028*/
    volatile unsigned long  asc_abcon;                          /*0x0030*/
    volatile unsigned long  asc_abstat;     /* not used */      /*0x0034*/
    volatile unsigned long  asc_whbabcon;                       /*0x0038*/
    volatile unsigned long  asc_whbabstat;  /* not used */      /*0x003C*/
    volatile unsigned long  asc_rxfcon;                         /*0x0040*/
    volatile unsigned long  asc_txfcon;                         /*0x0044*/
    volatile unsigned long  asc_fstat;                          /*0x0048*/
    volatile unsigned long  asc_rsvd4[1];   /* for mapping */   /*0x004C*/
    volatile unsigned long  asc_bg;                             /*0x0050*/
    volatile unsigned long  asc_bg_timer;                       /*0x0054*/
    volatile unsigned long  asc_fdv;                            /*0x0058*/
    volatile unsigned long  asc_pmw;                            /*0x005C*/
    volatile unsigned long  asc_modcon;                         /*0x0060*/
    volatile unsigned long  asc_modstat;                        /*0x0064*/
    volatile unsigned long  asc_rsvd5[2];   /* for mapping */   /*0x0068*/
    volatile unsigned long  asc_sfcc;                           /*0x0070*/
    volatile unsigned long  asc_rsvd6[3];   /* for mapping */   /*0x0074*/
    volatile unsigned long  asc_eomcon;                         /*0x0080*/
    volatile unsigned long  asc_rsvd7[26];   /* for mapping */  /*0x0084*/
    volatile unsigned long  asc_dmacon;                         /*0x00EC*/
    volatile unsigned long  asc_rsvd8[1];   /* for mapping */   /*0x00F0*/
    volatile unsigned long  asc_irnen;                          /*0x00F4*/
    volatile unsigned long  asc_irncr;                          /*0x00F8*/
    volatile unsigned long  asc_irnicr;                         /*0x00FC*/
} ifx_asc_reg_t;

typedef struct {                /* ifx_asc_port_priv_t */
    ifx_asc_reg_t  *base;       /* Port base address */
    u8              portwidth;  /* 8 or 32 */
    u8              tir;        /* TX interrupt */
    u8              tbir;       /* TX buffer interrupt */
    u8              rir;        /* RX interrupt */
    u8              eir;        /* ERROR interrupt */
    char            *tx_irq_name;
    char            *rx_irq_name;
    char            *err_irq_name;
    u32             tx_irq_on;
    u32             rx_bytes;
    u32             rx_parity_error;
    u32             rx_frame_error;
    u32             rx_overrun_error;
    u32             tx_bytes;
    u32             baudrate;
    spinlock_t      lock;
} ifx_asc_port_priv_t;



#if defined(CONFIG_DANUBE)
#  include "ifxmips_asc_danube.h"
#elif defined(CONFIG_AMAZON_SE)
#  include "ifxmips_asc_amazon_se.h"
#elif defined(CONFIG_AR9)
#  include "ifxmips_asc_ar9.h"
#elif defined(CONFIG_VR9)
#  include "ifxmips_asc_vr9.h"
#elif defined(CONFIG_AR10)
#  include "ifxmips_asc_ar10.h"
#else
#  error Platform is not specified!
#endif



#endif  // IFX_ASC_H
