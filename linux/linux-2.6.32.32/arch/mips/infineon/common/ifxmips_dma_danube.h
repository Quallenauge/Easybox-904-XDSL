/**
** FILE NAME    : ifxmips_dma_danube.h
** PROJECT      : IFX UEIP
** MODULES      : Central DMA
** DATE         : 03 June 2009
** AUTHOR       : Reddy Mallikarjuna
** DESCRIPTION  : IFX Danube Central DMA driver header file
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
** 03 June 2009     Reddy Mallikarjuna  Initial UEIP release
*******************************************************************************/

#ifndef _IFXMIPS_DMA_DANUBE_H_
#define _IFXMIPS_DMA_DANUBE_H_

/*!
  \file ifxmips_dma_danube.h
  \ingroup IFX_DMA_CORE
  \brief Header file for IFX Central DMA  driver internal definition for DANUBE platform.
*/

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/*! \enum ifx_dma_chan_dir_t
 \brief DMA Rx/Tx channel
*/
typedef enum {
    IFX_DMA_RX_CH = 0,  /*!< Rx channel */
    IFX_DMA_TX_CH = 1,  /*!< Tx channel */
} ifx_dma_chan_dir_t;

/*! \typedef _dma_chan_map
 \brief The parameter structure is used to dma channel map
*/
typedef struct ifx_dma_chan_map{
    int 				port_num;       /*!< Port number */
    char    			dev_name[16];   /*!< Device Name */
    ifx_dma_chan_dir_t 	dir;            /*!< Direction of the DMA channel */
    int     			pri;            /*!< Class value */
    int     			irq;            /*!< DMA channel irq number(refer to the platform irq.h file) */
    int     			rel_chan_no;    /*!< Relative channel number */
} _dma_chan_map;

/* DANUBE Supported Devices */
#if !defined(ENABLE_RX_DPLUS_PATH) || !ENABLE_RX_DPLUS_PATH
static char dma_device_name[6][20] = {{"PPE"},{"DEU"},{"SPI"},{"SDIO"},{"MCTRL0"},{"MCTRL1"}};
static _dma_chan_map ifx_default_dma_map[20] = {
    /* portnum, device name, channel direction, class value, IRQ number, relative channel number */
    {0, "PPE",      IFX_DMA_RX_CH,  0,  IFX_DMA_CH0_INT,    0},
    {0, "PPE",      IFX_DMA_TX_CH,  0,  IFX_DMA_CH1_INT,    0},
    {0, "PPE",      IFX_DMA_RX_CH,  1,  IFX_DMA_CH2_INT,    1},
    {0, "PPE",      IFX_DMA_TX_CH,  1,  IFX_DMA_CH3_INT,    1},
    {0, "PPE",      IFX_DMA_RX_CH,  2,  IFX_DMA_CH4_INT,    2},
    {0, "PPE",      IFX_DMA_TX_CH,  2,  IFX_DMA_CH5_INT,    2},
    {0, "PPE",      IFX_DMA_RX_CH,  3,  IFX_DMA_CH6_INT,    3},
    {0, "PPE",      IFX_DMA_TX_CH,  3,  IFX_DMA_CH7_INT,    3},
    {1, "DEU",      IFX_DMA_RX_CH,  0,  IFX_DMA_CH8_INT,    0},
    {1, "DEU",      IFX_DMA_TX_CH,  0,  IFX_DMA_CH9_INT,    0},
    {1, "DEU",      IFX_DMA_RX_CH,  1,  IFX_DMA_CH10_INT,   1},
    {1, "DEU",      IFX_DMA_TX_CH,  1,  IFX_DMA_CH11_INT,   1},
    {2, "SPI",      IFX_DMA_RX_CH,  0,  IFX_DMA_CH12_INT,   0},
    {2, "SPI",      IFX_DMA_TX_CH,  0,  IFX_DMA_CH13_INT,   0},
    {3, "SDIO",     IFX_DMA_RX_CH,  0,  IFX_DMA_CH14_INT,   0},
    {3, "SDIO",     IFX_DMA_TX_CH,  0,  IFX_DMA_CH15_INT,   0},
    {4, "MCTRL0",   IFX_DMA_RX_CH,  0,  IFX_DMA_CH16_INT,   0},
    {4, "MCTRL0",   IFX_DMA_TX_CH,  0,  IFX_DMA_CH17_INT,   0},
    {4,	"MCTRL1",   IFX_DMA_RX_CH,  1,  IFX_DMA_CH18_INT,   1},
    {4,	"MCTRL1",   IFX_DMA_TX_CH,  1,  IFX_DMA_CH19_INT,   1},
};
#else
static char dma_device_name[7][20] = {{"PPE"},{"ETH2"},{"DEU"},{"SPI"},{"SDIO"},{"MCTRL0"},{"MCTRL1"}};
static _dma_chan_map ifx_default_dma_map[20] = {
    (0, "PPE",      IFX_DMA_RX_CH,  0,  IFX_DMA_CH0_INT,    0},
    {0, "PPE",      IFX_DMA_TX_CH,  0,  IFX_DMA_CH1_INT,    0},
    {0, "PPE",      IFX_DMA_RX_CH,  1,  IFX_DMA_CH2_INT,    1},
    {0, "PPE",      IFX_DMA_TX_CH,  1,  IFX_DMA_CH3_INT,    1},
    {0, "ETH2",     IFX_DMA_RX_CH,  2,  IFX_DMA_CH4_INT,    2},
    {0, "ETH2",     IFX_DMA_TX_CH,  2,  IFX_DMA_CH5_INT,    2},
    {0, "ETH2",     IFX_DMA_RX_CH,  3,  IFX_DMA_CH6_INT,    3},
    {0, "ETH2",     IFX_DMA_TX_CH,  3,  IFX_DMA_CH7_INT,    3},
    {1, "DEU",      IFX_DMA_RX_CH,  0,  IFX_DMA_CH8_INT,    0},
    {1, "DEU",      IFX_DMA_TX_CH,  0,  IFX_DMA_CH9_INT,    0},
    {1, "DEU",      IFX_DMA_RX_CH,  1,  IFX_DMA_CH10_INT,   1},
    {1, "DEU",      IFX_DMA_TX_CH,  1,  IFX_DMA_CH11_INT,   1},
    {2, "SPI",      IFX_DMA_RX_CH,  0,  IFX_DMA_CH12_INT,   0},
    {2, "SPI",      IFX_DMA_TX_CH,  0,  IFX_DMA_CH13_INT,   0},
    {3, "SDIO",     IFX_DMA_RX_CH,  0,  IFX_DMA_CH14_INT,   0},
    {3, "SDIO",     IFX_DMA_TX_CH,  0,  IFX_DMA_CH15_INT,   0},
    {4, "MCTRL0",   IFX_DMA_RX_CH,  0,  IFX_DMA_CH16_INT,   0},
    {4, "MCTRL0",   IFX_DMA_TX_CH,  0,  IFX_DMA_CH17_INT,   0},
    {4,	"MCTRL1",   IFX_DMA_RX_CH,  1,  IFX_DMA_CH18_INT,   1},
    {4,	"MCTRL1",   IFX_DMA_TX_CH,  1,  IFX_DMA_CH19_INT,   1},
};
#endif /** ENABLE_RX_DPLUS_PATH */

#endif /* _IFXMIPS_DMA_DANUBE_H_ */
