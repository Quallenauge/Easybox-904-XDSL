/******************************************************************************
**
** FILE NAME    : ifxmips_ssc_reg.h
** PROJECT      : IFX UEIP
** MODULES      : SSC (Synchronous Serial Controller)
**
** DATE         : 31 Aug 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : SCC Driver
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
** 31 Aug, 2009  Lei Chuanhua    First UEIP release
*******************************************************************************/

#ifndef IFXMIPS_SSC_REG_H
#define IFXMIPS_SSC_REG_H

/*!
 \file ifxmips_ssc_reg.h
 \ingroup IFX_SSC    
 \brief ifx ssc module register definition
*/
#define IFX_SSC_PHY_BASE              (0x1E100800)
#define IFX_SSC_SIZE                   256

/* Default values for SSC configuration */
#define IFX_SSC_DEF_IDLE_DATA       1  /* enable */
#define IFX_SSC_DEF_BYTE_VALID_CTL  1  /* enable */
#define IFX_SSC_DEF_DATA_WIDTH      32 /* bits */
#define IFX_SSC_DEF_ABRT_ERR_DETECT 0  /* disable */
#define IFX_SSC_DEF_RO_ERR_DETECT   1  /* enable */
#define IFX_SSC_DEF_RU_ERR_DETECT   0  /* disable */
#define IFX_SSC_DEF_TO_ERR_DETECT   0  /* disable */
#define IFX_SSC_DEF_TU_ERR_DETECT   0  /* disable */
#define IFX_SSC_DEF_LOOP_BACK       0  /* disable */
#define IFX_SSC_DEF_ECHO_MODE       0  /* disable */
#define IFX_SSC_DEF_CLOCK_POLARITY  0  /* low */
#define IFX_SSC_DEF_CLOCK_PHASE     1  /* 0: shift on leading edge, latch on trailling edge, 1, otherwise */
#define IFX_SSC_DEF_HEADING_CONTROL IFX_SSC_MSB_FIRST
#define IFX_SSC_DEF_MODE_RXTX       IFX_SSC_MODE_RXTX
// other values
#define IFX_SSC_DEF_MASTERSLAVE     IFX_SSC_MASTER_MODE /* master */
#ifdef CONFIG_USE_EMULATOR
#define IFX_SSC_DEF_BAUDRATE        10000
#else
#define IFX_SSC_DEF_BAUDRATE        2000000
#endif
#define IFX_SSC_DEF_RMC             0x01 /* To use it up to max value */

#define IFX_SSC_DEF_TXFIFO_FL       4   /* Tunnable, Smaller Trigger level means small polling time 
                                         * Max trigger level is equal to FIFO size - DMA burst length
                                         */
#define IFX_SSC_DEF_RXFIFO_FL       4   /* DMA case, RX info must be even number */

#define IFX_SSC_DEF_GPO_CS      0x3 /* no chip select */
#define IFX_SSC_DEF_GPO_INV     0   /* no chip select */
#define IFX_SSC_DEF_SFCON       0   /* no serial framing */
#define IFX_SSC_DEF_IRNEN       (IFX_SSC_T_BIT | IFX_SSC_R_BIT | IFX_SSC_E_BIT)

/* Macros to configure SSC hardware */
/* headingControl: */
#define IFX_SSC_LSB_FIRST            0
#define IFX_SSC_MSB_FIRST            1
/* dataWidth: */
#define IFX_SSC_MIN_DATA_WIDTH       2
#define IFX_SSC_MAX_DATA_WIDTH       32
/* master/slave mode select */
#define IFX_SSC_MASTER_MODE          1
#define IFX_SSC_SLAVE_MODE           0
/* rx/tx mode */
#define IFX_SSC_MODE_RXTX        0
#define IFX_SSC_MODE_RX          1
#define IFX_SSC_MODE_TX          2
#define IFX_SSC_MODE_OFF         3
#define IFX_SSC_MODE_MASK        IFX_SSC_MODE_RX | IFX_SSC_MODE_TX

/* GPO values */
#define IFX_SSC_MAX_GPO_OUT      7

#define IFX_SSC_RXREQ_BLOCK_SIZE     32768

/* Clock Control Register*/
#define IFX_SSC_CLC                     0x00000000
/* CLC register bits */
#define IFX_SSC_CLC_EN                  0x00000000
#define IFX_SSC_CLC_DIS                 0x00000001
#define IFX_SSC_CLC_DIS_STATUS          0x00000002
#define IFX_SSC_CLC_SUSPEND_EN          0x00000004
#define IFX_SSC_CLC_EXT_CLOCK_DIS       0x00000008
#define IFX_SSC_CLC_WR_PROTECT_DIS      0x00000010
#define IFX_SSC_CLC_FAST_CLOCK_SHUT_OFF 0x00000020
#define IFX_SSC_CLC_RMC                 0x0000FF00
#define IFX_SSC_CLC_RMC_S               8
#define IFX_SSC_CLC_SMC                 0x00FF0000
#define IFX_SSC_CLC_SMC_S               16

/* Port Input Select Register*/
#define IFX_SSC_PISEL                   0x00000004

#define IFX_SSC_PISEL_MASTER_IN_A       0x0000
#define IFX_SSC_PISEL_MASTER_IN_B       0x0001
#define IFX_SSC_PISEL_SLAVE_IN_A        0x0000
#define IFX_SSC_PISEL_SLAVE_IN_B        0x0002
#define IFX_SSC_PISEL_CLOCK_IN_A        0x0000
#define IFX_SSC_PISEL_CLOCK_IN_B        0x0004

/* Identification Register */
#define IFX_SSC_ID                      0x00000008

#define IFX_SSC_ID_REV                  0x0000001F
#define IFX_SSC_ID_REV_S                0
#define IFX_SSC_ID_CFG_DMA              0x00000020
#define IFX_SSC_ID_MODULE_ID            0x0000FF00
#define IFX_SSC_ID_MODULE_ID_S          8
#define IFX_SSC_ID_RXFS                 0x003F0000
#define IFX_SSC_ID_RXFS_S               16
#define IFX_SSC_ID_TXFS                 0x3F000000
#define IFX_SSC_ID_TXFS_S               24

#define IFX_SSC_ID_MODULE_ID_VAL        0x45

/* address of the Control Register of the SSC */
#define IFX_SSC_CON                         0x00000010

/* IFX_SSC_CON register */
#ifdef CONFIG_VR9
#define IFX_SSC_CON_RX_SWAP                 0x04000000
#define IFX_SSC_CON_TX_SWAP                 0x02000000
#endif /* CONFIG_VR9 */

#define IFX_SSC_CON_TXENDI                  0x04000000
#define IFX_SSC_CON_RXENDI                  0x02000000
#define IFX_SSC_CON_ECHO_MODE_ON            0x01000000
#define IFX_SSC_CON_IDLE_HIGH               0x00800000
#define IFX_SSC_CON_ENABLE_BYTE_VALID       0x00400000

#define IFX_SSC_CON_DATA_WIDTH              0x001F0000
#define IFX_SSC_CON_DATA_WIDTH_S            16

#define IFX_SSC_CON_LATCH_CLK_DELAY         0x00002000
#define IFX_SSC_CON_RX_UFL_CHECK            0x00001000
#define IFX_SSC_CON_TX_UFL_CHECK            0x00000800
#define IFX_SSC_CON_ABORT_ERR_CHECK         0x00000400
#define IFX_SSC_CON_RX_OFL_CHECK            0x00000200
#define IFX_SSC_CON_TX_OFL_CHECK            0x00000100

#define IFX_SSC_CON_LOOPBACK_MODE           0x00000080
#define IFX_SSC_CON_PO                      0x00000040
#define IFX_SSC_CON_PH                      0x00000020
#define IFX_SSC_CON_MSB_FIRST               0x00000010
#define IFX_SSC_CON_ENABLE_CS_SLAVE         0x00000008
#define IFX_SSC_CON_INVERT_CS_HIGH_ACTIVE   0x00000004
#define IFX_SSC_CON_RX_OFF                  0x00000002
#define IFX_SSC_CON_TX_OFF                  0x00000001

#define IFX_SSC_ENCODE_DATA_WIDTH(width) (SM(((width) - 1), IFX_SSC_CON_DATA_WIDTH))

/* Status Register */
#define IFX_SSC_STATE                      0x00000014

#define IFX_SSC_STATE_RX_EOM               0x80000000
#define IFX_SSC_STATE_RX_BYTE_VALID        0x70000000
#define IFX_SSC_STATE_RX_BYTE_VALID_S      28
#define IFX_SSC_DECODE_RX_BYTE_VALID(con_state) (MS((con_state), IFX_SSC_STATE_RX_BYTE_VALID))

#define IFX_SSC_STATE_TX_EOM               0x08000000
#define IFX_SSC_STATE_TX_BYTE_VALID        0x07000000
#define IFX_SSC_STATE_TX_BYTE_VALID_S      24
#define IFX_SSC_DECODE_TX_BYTE_VALID(con_state) (MS((con_state), IFX_SSC_STATE_TX_BYTE_VALID))

#define IFX_SSC_STATE_BIT_COUNT            0x001F0000
#define IFX_SSC_STATE_BIT_COUNT_S          16
#define IFX_SSC_DECODE_DATA_WIDTH(con_state) (MS((con_state), IFX_SSC_STATE_BIT_COUNT) + 1)
#define IFX_SSC_STATE_BUSY                 0x00002000
#define IFX_SSC_STATE_RX_UFL               0x00001000
#define IFX_SSC_STATE_TX_UFL               0x00000800
#define IFX_SSC_STATE_ABORT_ERR            0x00000400
#define IFX_SSC_STATE_RX_OFL               0x00000200
#define IFX_SSC_STATE_TX_OFL               0x00000100
#define IFX_SSC_STATE_MODE_ERR             0x00000080
#define IFX_SSC_STATE_SLAVE_SELECTED       0x00000004
#define IFX_SSC_STATE_MASTER               0x00000002
#define IFX_SSC_STATE_ENABLED              0x00000001

/* Write Hardware Modified Control Register Bits  */
#define IFX_SSC_WHBSTATE                    0x00000018
#define IFX_SSC_WHBSTATE_DISABLE_SSC        0x00000001
#define IFX_SSC_WHBSTATE_CONFIGURATION_MODE 0x00000001
#define IFX_SSC_WHBSTATE_ENABLE_SSC         0x00000002
#define IFX_SSC_WHBSTATE_RUN_MODE           0x00000002
#define IFX_SSC_WHBSTATE_SLAVE_MODE         0x00000004
#define IFX_SSC_WHBSTATE_MASTER_MODE        0x00000008
#define IFX_SSC_WHBSTATE_CLR_RX_UFL_ERROR   0x00000010
#define IFX_SSC_WHBSTATE_SET_RX_UFL_ERROR   0x00000020
#define IFX_SSC_WHBSTATE_CLR_MODE_ERROR     0x00000040
#define IFX_SSC_WHBSTATE_SET_MODE_ERROR     0x00000080
#define IFX_SSC_WHBSTATE_CLR_TX_OFL_ERROR   0x00000100
#define IFX_SSC_WHBSTATE_CLR_RX_OFL_ERROR   0x00000200
#define IFX_SSC_WHBSTATE_CLR_ABORT_ERROR    0x00000400
#define IFX_SSC_WHBSTATE_CLR_TX_UFL_ERROR   0x00000800
#define IFX_SSC_WHBSTATE_SET_TX_OFL_ERROR   0x00001000
#define IFX_SSC_WHBSTATE_SET_RX_OFL_ERROR   0x00002000
#define IFX_SSC_WHBSTATE_SET_ABORT_ERROR    0x00004000
#define IFX_SSC_WHBSTATE_SET_TX_UFL_ERROR   0x00008000
#define IFX_SSC_WHBSTATE_CLR_ALL_ERROR      0x00000F50
#define IFX_SSC_WHBSTATE_SET_ALL_ERROR      0x0000F0A0

/* Baudrate Timer Reload Register */
#define IFX_SSC_BR                          0x00000040
#define IFX_SSC_BR_BAUDRATE                 0xFFFF
#define IFX_SSC_BR_BAUDRATE_S               0

/* Baudrate Timer Status Register */
#define IFX_SSC_BRSTAT                      0x00000044
#define IFX_SSC_BRSTAT_BAUDTIMER            0xFFFF
#define IFX_SSC_BRSTAT_BAUDTIMER_S          0

/* Transmitter Buffer Register */
#define IFX_SSC_TB                          0x00000020
#define IFX_SSC_TB_DATA                     0xFFFFFFFF
#define IFX_SSC_TB_DATA_S                   0

/* Reciver Buffer Register */
#define IFX_SSC_RB                          0x00000024
#define IFX_SSC_RB_DATA                     0xFFFFFFFF
#define IFX_SSC_RB_DATA_S                   0

/* Receive FIFO Control Register */
#define IFX_SSC_RXFCON                      0x00000030

/* Transmit FIFO Control Register */
#define IFX_SSC_TXFCON                      0x00000034

#define IFX_SSC_XFCON_FIFO_ENABLE           0x00000001
#define IFX_SSC_XFCON_FIFO_FLUSH            0x00000002
#define IFX_SSC_XFCON_ITL                   0x00003F00
#define IFX_SSC_XFCON_ITL_S                 8

/* FIFO Status Register */
#define IFX_SSC_FSTAT                       0x00000038
#define IFX_SSC_FSTAT_RX_WORDS              0x0000003F
#define IFX_SSC_FSTAT_RX_WORDS_S            0
#define IFX_SSC_FSTAT_TX_WORDS              0x00003F00
#define IFX_SSC_FSTAT_TX_WORDS_S            8

/* Data Frame Control register */
#define IFX_SSC_SFCON                          0x00000060
#define IFX_SSC_SFCON_SF_ENABLE                0x00000001
#define IFX_SSC_SFCON_FIR_ENABLE_BEFORE_PAUSE  0x00000004
#define IFX_SSC_SFCON_FIR_ENABLE_AFTER_PAUSE   0x00000008
#define IFX_SSC_SFCON_DATA_LENGTH              0x0000FFF0
#define IFX_SSC_SFCON_DATA_LENGTH_S            4
#define IFX_SSC_SFCON_PAUSE_DATA               0x00030000
#define IFX_SSC_SFCON_PAUSE_DATA_S             16
#define IFX_SSC_SFCON_PAUSE_DATA_0             0x00000000
#define IFX_SSC_SFCON_PAUSE_DATA_1             0x00010000
#define IFX_SSC_SFCON_PAUSE_DATA_IDLE          0x00020000
#define IFX_SSC_SFCON_PAUSE_CLOCK              0x000C0000
#define IFX_SSC_SFCON_PAUSE_CLOCK_S            18
#define IFX_SSC_SFCON_PAUSE_CLOCK_0            0x00000000
#define IFX_SSC_SFCON_PAUSE_CLOCK_1            0x00040000
#define IFX_SSC_SFCON_PAUSE_CLOCK_IDLE         0x00080000
#define IFX_SSC_SFCON_PAUSE_CLOCK_RUN          0x000C0000
#define IFX_SSC_SFCON_STOP_AFTER_PAUSE         0x00100000
#define IFX_SSC_SFCON_CONTINUE_AFTER_PAUSE     0x00000000
#define IFX_SSC_SFCON_PAUSE_LENGTH             0xFFC00000
#define IFX_SSC_SFCON_PAUSE_LENGTH_S           22
#define IFX_SSC_SFCON_DATA_LENGTH_MAX          4096
#define IFX_SSC_SFCON_PAUSE_LENGTH_MAX         1024

#define IFX_SSC_SFCON_EXTRACT_DATA_LENGTH(sfcon)  (MS((sfcon), IFX_SSC_SFCON_DATA_LENGTH))
#define IFX_SSC_SFCON_EXTRACT_PAUSE_LENGTH(sfcon) (MS((sfcon), IFX_SSC_SFCON_PAUSE_LENGTH))
#define IFX_SSC_SFCON_SET_DATA_LENGTH(value)      (SM((value), IFX_SSC_SFCON_DATA_LENGTH))
#define IFX_SSC_SFCON_SET_PAUSE_LENGTH(value)     (SM((value), IFX_SSC_SFCON_PAUSE_LENGTH))

/* Data Frame Status register */
#define IFX_SSC_SFSTAT                      0x00000064
#define IFX_SSC_SFSTAT_IN_DATA              0x00000001
#define IFX_SSC_SFSTAT_IN_PAUSE             0x00000002
#define IFX_SSC_SFSTAT_DATA_COUNT           0x0000FFF0
#define IFX_SSC_SFSTAT_DATA_COUNT_S         4
#define IFX_SSC_SFSTAT_PAUSE_COUNT          0xFFF00000
#define IFX_SSC_SFSTAT_PAUSE_COUNT_S        20

#define IFX_SSC_SFSTAT_EXTRACT_DATA_COUNT(sfstat)  (MS((sfstat), IFX_SSC_SFSTAT_DATA_COUNT))
#define IFX_SSC_SFSTAT_EXTRACT_PAUSE_COUNT(sfstat) (MS((sfstat), IFX_SSC_SFSTAT_PAUSE_COUNT))

/* General Purpose Output Control register */
#define IFX_SSC_GPOCON                      0x00000070
#define IFX_SSC_GPOCON_INVOUT0_POS      0
#define IFX_SSC_GPOCON_INV_OUT0         0x00000001
#define IFX_SSC_GPOCON_TRUE_OUT0        0x00000000
#define IFX_SSC_GPOCON_INVOUT1_POS      1
#define IFX_SSC_GPOCON_INV_OUT1         0x00000002
#define IFX_SSC_GPOCON_TRUE_OUT1        0x00000000
#define IFX_SSC_GPOCON_INVOUT2_POS      2
#define IFX_SSC_GPOCON_INV_OUT2         0x00000003
#define IFX_SSC_GPOCON_TRUE_OUT2        0x00000000
#define IFX_SSC_GPOCON_INVOUT3_POS      3
#define IFX_SSC_GPOCON_INV_OUT3         0x00000008
#define IFX_SSC_GPOCON_TRUE_OUT3        0x00000000
#define IFX_SSC_GPOCON_INVOUT4_POS      4
#define IFX_SSC_GPOCON_INV_OUT4         0x00000010
#define IFX_SSC_GPOCON_TRUE_OUT4        0x00000000
#define IFX_SSC_GPOCON_INVOUT5_POS      5
#define IFX_SSC_GPOCON_INV_OUT5         0x00000020
#define IFX_SSC_GPOCON_TRUE_OUT5        0x00000000
#define IFX_SSC_GPOCON_INVOUT6_POS      6
#define IFX_SSC_GPOCON_INV_OUT6         0x00000040
#define IFX_SSC_GPOCON_TRUE_OUT6        0x00000000
#define IFX_SSC_GPOCON_INVOUT7_POS      7
#define IFX_SSC_GPOCON_INV_OUT7         0x00000080
#define IFX_SSC_GPOCON_TRUE_OUT7        0x00000000
#define IFX_SSC_GPOCON_INV_OUT_ALL      0x000000FF
#define IFX_SSC_GPOCON_TRUE_OUT_ALL     0x00000000

#define IFX_SSC_GPOCON_ISCSB0_POS       8
#define IFX_SSC_GPOCON_IS_CSB0          0x00000100
#define IFX_SSC_GPOCON_IS_GPO0          0x00000000
#define IFX_SSC_GPOCON_ISCSB1_POS       9
#define IFX_SSC_GPOCON_IS_CSB1          0x00000200
#define IFX_SSC_GPOCON_IS_GPO1          0x00000000
#define IFX_SSC_GPOCON_ISCSB2_POS       10
#define IFX_SSC_GPOCON_IS_CSB2          0x00000400
#define IFX_SSC_GPOCON_IS_GPO2          0x00000000
#define IFX_SSC_GPOCON_ISCSB3_POS       11
#define IFX_SSC_GPOCON_IS_CSB3          0x00000800
#define IFX_SSC_GPOCON_IS_GPO3          0x00000000
#define IFX_SSC_GPOCON_ISCSB4_POS       12
#define IFX_SSC_GPOCON_IS_CSB4          0x00001000
#define IFX_SSC_GPOCON_IS_GPO4          0x00000000
#define IFX_SSC_GPOCON_ISCSB5_POS       13
#define IFX_SSC_GPOCON_IS_CSB5          0x00002000
#define IFX_SSC_GPOCON_IS_GPO5          0x00000000
#define IFX_SSC_GPOCON_ISCSB6_POS       14
#define IFX_SSC_GPOCON_IS_CSB6          0x00004000
#define IFX_SSC_GPOCON_IS_GPO6          0x00000000
#define IFX_SSC_GPOCON_ISCSB7_POS       15
#define IFX_SSC_GPOCON_IS_CSB7          0x00008000
#define IFX_SSC_GPOCON_IS_GPO7          0x00000000
#define IFX_SSC_GPOCON_IS_CSB_ALL       0x0000FF00
#define IFX_SSC_GPOCON_IS_GPO_ALL       0x00000000

/* General Purpose Output Status register */
#define IFX_SSC_GPOSTAT              0x00000074

#define IFX_SSC_GPOSTAT_OUT0            0x00000001
#define IFX_SSC_GPOSTAT_OUT1            0x00000002
#define IFX_SSC_GPOSTAT_OUT2            0x00000004
#define IFX_SSC_GPOSTAT_OUT3            0x00000008
#define IFX_SSC_GPOSTAT_OUT4            0x00000010
#define IFX_SSC_GPOSTAT_OUT5            0x00000020
#define IFX_SSC_GPOSTAT_OUT6            0x00000040
#define IFX_SSC_GPOSTAT_OUT7            0x00000080
#define IFX_SSC_GPOSTAT_OUT_ALL         0x000000FF

/* Force GPO Status register */
#define IFX_SSC_WHBGPOSTAT              0x00000078
#define IFX_SSC_WHBGPOSTAT_CLROUT0_POS  0
#define IFX_SSC_WHBGPOSTAT_CLR_OUT0     0x00000001
#define IFX_SSC_WHBGPOSTAT_CLROUT1_POS  1
#define IFX_SSC_WHBGPOSTAT_CLR_OUT1     0x00000002
#define IFX_SSC_WHBGPOSTAT_CLROUT2_POS  2
#define IFX_SSC_WHBGPOSTAT_CLR_OUT2     0x00000004
#define IFX_SSC_WHBGPOSTAT_CLROUT3_POS  3
#define IFX_SSC_WHBGPOSTAT_CLR_OUT3     0x00000008
#define IFX_SSC_WHBGPOSTAT_CLROUT4_POS  4
#define IFX_SSC_WHBGPOSTAT_CLR_OUT4     0x00000010
#define IFX_SSC_WHBGPOSTAT_CLROUT5_POS  5
#define IFX_SSC_WHBGPOSTAT_CLR_OUT5     0x00000020
#define IFX_SSC_WHBGPOSTAT_CLROUT6_POS  6
#define IFX_SSC_WHBGPOSTAT_CLR_OUT6     0x00000040
#define IFX_SSC_WHBGPOSTAT_CLROUT7_POS  7
#define IFX_SSC_WHBGPOSTAT_CLR_OUT7     0x00000080
#define IFX_SSC_WHBGPOSTAT_CLR_OUT_ALL  0x000000FF

#define IFX_SSC_WHBGPOSTAT_SETOUT0_POS  8
#define IFX_SSC_WHBGPOSTAT_SET_OUT0     0x00000100
#define IFX_SSC_WHBGPOSTAT_SETOUT1_POS  9
#define IFX_SSC_WHBGPOSTAT_SET_OUT1     0x00000200
#define IFX_SSC_WHBGPOSTAT_SETOUT2_POS  10
#define IFX_SSC_WHBGPOSTAT_SET_OUT2     0x00000400
#define IFX_SSC_WHBGPOSTAT_SETOUT3_POS  11
#define IFX_SSC_WHBGPOSTAT_SET_OUT3     0x00000800
#define IFX_SSC_WHBGPOSTAT_SETOUT4_POS  12
#define IFX_SSC_WHBGPOSTAT_SET_OUT4     0x00001000
#define IFX_SSC_WHBGPOSTAT_SETOUT5_POS  13
#define IFX_SSC_WHBGPOSTAT_SET_OUT5     0x00002000
#define IFX_SSC_WHBGPOSTAT_SETOUT6_POS  14
#define IFX_SSC_WHBGPOSTAT_SET_OUT6     0x00004000
#define IFX_SSC_WHBGPOSTAT_SETOUT7_POS  15
#define IFX_SSC_WHBGPOSTAT_SET_OUT7     0x00008000
#define IFX_SSC_WHBGPOSTAT_SET_OUT_ALL  0x0000FF00

/* Receive Request Register */
#define IFX_SSC_RXREQ                   0x00000080
#define IFX_SSC_RXREQ_RXCOUNT           0x0000FFFF
#define IFX_SSC_RXREQ_RXCOUNT_S         0

/* Receive Count Register */
#define IFX_SSC_RXCNT                   0x00000084

#define IFX_SSC_RXCNT_TODO              0x0000FFFF
#define IFX_SSC_RXCNT_TODO_S            0

/* DMA Configuration Register */
#define IFX_SSC_DMACON                  0x000000EC

#define IFX_SSC_DMACON_RXON             0x00000001
#define IFX_SSC_DMACON_TXON             0x00000002
#define IFX_SSC_DMACON_DMAON            0x00000003
#define IFX_SSC_DMACON_CLASS            0x0000000C
#define IFX_SSC_DMACON_CLASS_S          2

/* Interrupt Node Enable Register */
#define IFX_SSC_IRN_EN                 0xF4

/* Interrupt Node Interrupt Capture Register */
#define IFX_SSC_IRN_CR                 0xF8

/* Interrupt Node Control Register */
#define IFX_SSC_IRN_ICR                0xFC

#define IFX_SSC_NUM_IRQ                4

#ifdef CONFIG_DANUBE
#define IFX_SSC_R_BIT       0x00000001  /* Data sheet is wrong about R/T definition */
#define IFX_SSC_T_BIT       0x00000002
#else
#define IFX_SSC_T_BIT       0x00000001  /* Data sheet is wrong about R/T definition */
#define IFX_SSC_R_BIT       0x00000002
#endif
#define IFX_SSC_E_BIT       0x00000004
#define IFX_SSC_F_BIT       0x00000008

/* Generic register operation can't meet the requirement */

#ifdef	CONFIG_CPU_LITTLE_ENDIAN
#define IFX_SSC_REG_W16(_v, _r) ((*(volatile u16 *)(_r)) = (_v))
#define IFX_SSC_REG_W8(_v, _r)  ((*(volatile u8 *)(_r)) = (_v))
#else 
#define IFX_SSC_REG_W16(_v, _r)  ((*(volatile u16 *)((_r) + 2 )) = (_v))
#define IFX_SSC_REG_W8(_v, _r)   ((*(volatile u8 *)((_r) + 3 )) = (_v))
#endif /* CONFIG_CPU_LITTLE_ENDIAN */

/* Get or Set CLC configuration */
#define IFX_SSC_GET_CLC(_p)      IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_CLC))
#define IFX_SSC_GET_CLC_RMC(_p)  MS(IFX_SSC_GET_CLC((_p)), IFX_SSC_CLC_RMC)
#define IFX_SSC_SET_CLC(_v, _p)  IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_CLC))

/* Get ID */
#define IFX_SSC_GET_ID(_p)       IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_ID))
#define IFX_SSC_TX_FIFO_SIZE(_p) MS(IFX_SSC_GET_ID((_p)), IFX_SSC_ID_TXFS)
#define IFX_SSC_RX_FIFO_SIZE(_p) MS(IFX_SSC_GET_ID((_p)), IFX_SSC_ID_RXFS)

/* Get or Set CON configuration */
#define IFX_SSC_GET_CON(_p)      IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_CON))
#define IFX_SSC_SET_CON(_v, _p)  IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_CON))

/* Get SSC Status */
#define IFX_SSC_GET_STATE(_p)          IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_STATE))
#define IFX_SSC_RX_VALID_BYTES(_p)     MS(IFX_SSC_GET_STATE((_p)), IFX_SSC_STATE_RX_BYTE_VALID)

/* Set hardware modified control register */
#define IFX_SSC_SET_WHBSTATE(_v, _p)   IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_WHBSTATE))
#define IFX_SSC_CONFIG_MODE(_p)        IFX_SSC_SET_WHBSTATE(IFX_SSC_WHBSTATE_CONFIGURATION_MODE, (_p)) 
#define IFX_SSC_RUN_MODE(_p)           IFX_SSC_SET_WHBSTATE(IFX_SSC_WHBSTATE_RUN_MODE, (_p)) 

/* Set Baud Rate Time */
#define IFX_SSC_SET_BR(_v, _p)         IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_BR))
#define IFX_SSC_GET_BR(_p)             IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_BR))

/* TX Buffer Get/Set */
#define IFX_SSC_TX_WORD(_v, _p)        IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_TB))
#define IFX_SSC_GET_TX_WORD(_p)        IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_TB))
#define IFX_SSC_TX_HALFWORD(_v, _p)    IFX_SSC_REG_W16((_v), ((_p)->membase + IFX_SSC_TB))
#define IFX_SSC_TX_BYTE(_v, _p)        IFX_SSC_REG_W8((_v),  ((_p)->membase + IFX_SSC_TB))

/* RX buffer Get */
#define IFX_SSC_GET_RX_WORD(_p)        IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_RB))

/* RX FIFO Control */
#define IFX_SSC_RX_FIFO_CTRL(_v, _p)   IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_RXFCON))
#define IFX_SSC_GET_RX_FIFO_CTRL(_p)   IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_RXFCON))

/* TX FIFO Control */
#define IFX_SSC_TX_FIFO_CTRL(_v, _p)   IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_TXFCON))
#define IFX_SSC_GET_TX_FIFO_CTRL(_p)   IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_TXFCON))

/* FIFO status */
#define IFX_SSC_GET_FIFO_STATUS(_p)    IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_FSTAT))
#define IFX_SSC_RX_FIFO_FILL_LEVEL(_p) MS(IFX_SSC_GET_FIFO_STATUS((_p)), IFX_SSC_FSTAT_RX_WORDS)
#define IFX_SSC_TX_FIFO_FILL_LEVEL(_p) MS(IFX_SSC_GET_FIFO_STATUS((_p)), IFX_SSC_FSTAT_TX_WORDS)

/* Serial Framing Control */
#define IFX_SSC_GET_FRAMING_CON(_p)     IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_SFCON))
#define IFX_SSC_SET_FRAMING_CON(_v, _p) IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_SFCON))

/* Get Serial Framing Status */
#define IFX_SSC_GET_FRAMING_STATUS(_p)  IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_SFSTAT))

/* General Purpose IO control */
#define IFX_SSC_GET_GPOCON(_p)          IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_GPOCON))
#define IFX_SSC_SET_GPOCON(_v, _p)      IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_GPOCON))

/* General Purpose IO status */
#define IFX_SSC_GET_GPOSTAT(_p)         IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_GPOSTAT))

/* GPIO Force Output */
#define IFX_SSC_SET_FGPO(_v, _p)        IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_WHBGPOSTAT))

/* RX Request */
#define IFX_SSC_SET_RXREQ(_v, _p)       IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_RXREQ))
#define IFX_SSC_GET_RXREQ(_p)           IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_RXREQ))

/* Receive Reuest */
#define IFX_SSC_GET_RXREQ(_p)           IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_RXREQ))

/* Receive counter */
#define IFX_SSC_GET_RXCNT(_p)           IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_RXCNT))
#define IFX_SSC_RX_TO_RECEIVED(_p)      MS(IFX_SSC_GET_RXCNT(_p), IFX_SSC_RXCNT_TODO)

/* DMA Configuration */
#define IFX_SSC_GET_DMA_CON(_p)         IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_DMACON))
#define IFX_SSC_SET_DMA_CON(_v, _p)     IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_DMACON))

/* Interrupt Enable register set/get */
#define IFX_SSC_GET_IRN_EN(_p)          IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_IRN_EN))
#define IFX_SSC_SET_IRN_EN(_v, _p)      IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_IRN_EN))

/* Interrupt capture register */
#define IFX_SSC_GET_IRN_CR(_p)          IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_IRN_CR))
#define IFX_SSC_SET_IRN_CR(_v, _p)      IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_IRN_CR))

/* Interrupt Control Register */
#define IFX_SSC_GET_IRN_ICR(_p)          IFX_REG_R32((volatile u32 *)((_p)->membase + IFX_SSC_IRN_ICR))
#define IFX_SSC_SET_IRN_ICR(_v, _p)      IFX_REG_W32((_v), (volatile u32 *)((_p)->membase + IFX_SSC_IRN_ICR))

#endif /* IFXMIPS_SSC_REG_H */

