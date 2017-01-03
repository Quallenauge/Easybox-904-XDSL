/******************************************************************************
**
** FILE NAME    : ifx_pmon.h
** PROJECT      : IFX UEIP
** MODULES      : PMON
**
** DATE         : 21 July 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX Performance Monitor
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
** 21 July 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/

/**
  \defgroup IFX_PMON Performance Monitor Interface Module
  \brief ifx pmon driver module
*/

/*!
  \defgroup IFX_PMON_DEFINITIONS Defintions and structures
  \ingroup IFX_PMON
  \brief definitions for ifx pmon driver 
*/

/*!
  \defgroup IFX_PMON_IOCTL User API IOCTL
  \ingroup IFX_PMON
  \brief IOCTL Commands used by user application.
 */

/*!
  \file ifx_pmon.h
  \ingroup IFX_PMON
  \brief ifx pmon driver header file for APIs
*/
#ifndef IFX_PMON_H
#define IFX_PMON_H
/*!
  \addtogroup IFX_PMON_DEFINITIONS
 */
/* @{ */
#ifdef CONFIG_VR9
/*! \enum  IFX_PMON_EVENT  
    \brief External PMON Event defintion 
 */
enum IFX_PMON_EVENT {
    IFX_PMON_EVENT_NONE     = 0, /*!< No input (default) */
#define IFX_PMON_EVENT_MIN     IFX_PMON_EVENT_NONE            
    IFX_PMON_EVENT_DDR_READ,  /*!< DDR read commands, independent of length */
    IFX_PMON_EVENT_DDR_WRITE, /*!< DDR write commands, independent of length */
    IFX_PMON_EVENT_DDR_MASK_WRITE, /*!< DDR masked write commands */
    IFX_PMON_EVENT_DDR_ONE_WORD_64BIT_READ,/*!< Single-word 64-bit read commands */
    IFX_PMON_EVENT_DDR_TWO_WORD_64BIT_READ,/*!< 2-beat burst 64-bit read commands */
    IFX_PMON_EVENT_DDR_FOUR_WORD_64BIT_READ,/*!< 4-word 64-bit read commands */
    IFX_PMON_EVENT_DDR_EIGHT_WORD_64BIT_READ,/*!< 8-word 64-bit read commands */
    IFX_PMON_EVENT_DDR_ONE_WORD_64BIT_WRITE,/*!< Single 64-bit write commands */
    IFX_PMON_EVENT_DDR_TWO_WORD_64BIT_WRITE, /*!< 2-word 64-bit write commands */
    IFX_PMON_EVENT_DDR_FOUR_WORD_64BIT_WRITE,/*!< 4-word 64-bit write commands */
    IFX_PMON_EVENT_DDR_EIGHT_WORD_64BIT_WIRTE,/*!< 8-word 64-bit write commands */
    IFX_PMON_EVENT_AHB_READ_CYCLES, /*!< DMA 32-bit receive block counter */
    IFX_PMON_EVENT_AHB_READ_CPT,    /*!< DMA 32-bit transmit block counter */
    IFX_PMON_EVENT_AHB_WRITE_CYCLES,/*!< AHB read completed */
    IFX_PMON_EVENT_AHB_WRITE_CPT,   /*!< AHB write completed */
    IFX_PMON_EVENT_DMA_RX_BLOCK_CNT,/*!< AHB total read cycles */
    IFX_PMON_EVENT_DMA_TX_BLOCK_CNT,/*!< AHB total write cycles */
#define IFX_PMON_EVENT_MAX   IFX_PMON_EVENT_DMA_TX_BLOCK_CNT
};
#elif defined (CONFIG_AR10)
#define IFX_PMON_EVENT_NONE         0
#define IFX_PMON_BIU0_READ_EVENT    0x04 /*!< Bus interface Unit0 read */
#define IFX_PMON_BIU0_WRITE_EVENT   0x05 /*!< Bus interface Unit0 write */
#define IFX_PMON_BIU1_READ_EVENT    0x06 /*!< Bus interface Unit1 read */
#define IFX_PMON_BIU1_WRITE_EVENT   0x07 /*!< Bus interface Unit1 write */

#define IFX_PMON_EVENT_MIN          IFX_PMON_BIU0_READ_EVENT

/* DMA */
#define IFX_PMON_DMA_READ_EVENT     0x0C /*!< DMA read */
#define IFX_PMON_DMA_WRITE_EVENT    0x0D /*!< DMA write */
#define IFX_PMON_DMA_RX_EVENT       0x0E /*!< DMA payload rx */
#define IFX_PMON_DMA_TX_EVENT       0x0F /*!< DMA payload tx */

/* PPE, GPHY, ethernet */
#define IFX_PMON_FPI1S_READ_EVENT   0x14 /*!< FPI1 Slave bus read */
#define IFX_PMON_FPI1S_WRITE_EVENT  0x15 /*!< FPI1 Slave bus write */

/* WLAN BB/MAC, CPU DMA */
#define IFX_PMON_AHB1S_READ_EVENT   0x18 /*!< AHB1 Slave bus read */
#define IFX_PMON_AHB1S_WRITE_EVENT  0x19 /*!< AHB1 Slave bus write */

/* USB, DSL */
#define IFX_PMON_AHB2S_READ_EVENT   0x1C /*!< AHB2 Slave bus read */
#define IFX_PMON_AHB2S_WRITE_EVENT  0x1D /*!< AHB2 Slave bus write */

/* PCIe  */
#define IFX_PMON_AHB4S_READ_EVENT   0x20 /*!< AHB4 Slave bus read */
#define IFX_PMON_AHB4S_WRITE_EVENT  0x21 /*!< AHB4 Slave bus Write */

/* DDR  */
#define IFX_PMON_DDR_READ_EVENT     0x40 /*!< DDR single read */
#define IFX_PMON_DDR_WRITE_EVENT    0x41 /*!< DDR single write */
#define IFX_PMON_DDR_CMD_QUEUE_ALMOST_FULL  0x43 /*!< DDR Command Queue Almost Full */
#define IFX_PMON_DDR_CKE_STAT               0x44 /*!< DDR CKE status */
#define IFX_PMON_DDR_REFRESH_IN_PROGRESS    0x45 /*!< DDR Refresh in progress */
#define IFX_PMON_DDR_CONTROLLER_BUSY        0x46 /*!< DDR Controller Busy */
#define IFX_PMON_DDR_CMD_QUEUE_FULL         0x47 /*!< DDR Command Queue Full */

/* SRAM  */
#define IFX_PMON_SRAM_READ_EVENT    0x50 /*!< SRAM read */
#define IFX_PMON_SRAM_WRITE_EVENT   0x51 /*!< SRAM write */

/* FPI2M  */
#define IFX_PMON_FPI2M_READ_EVENT   0x58 /*!< FPI2 master read */
#define IFX_PMON_FPI2M_WRITE_EVENT  0x59 /*!< FPI2 master write */

/* FPI3M  */
#define IFX_PMON_FPI3M_READ_EVENT   0x5C /*!< FPI3 master read */
#define IFX_PMON_FPI3M_WRITE_EVENT  0x5D /*!< FPI3 master write */

/* AHB3M  */
#define IFX_PMON_AHB3M_READ_EVENT   0x60 /*!< AHB3 master read */
#define IFX_PMON_AHB3M_WRITE_EVENT  0x61 /*!< AHB3 master write */

#define IFX_PMON_EVENT_MAX   IFX_PMON_AHB3M_WRITE_EVENT
#else
#error "platform not supported"
#endif


#define IFX_PMON_XTC_COUNTER0  0 

#define IFX_PMON_XTC_COUNTER1  1 

#define IFX_PMON_MAX_PERF_CNT_PER_TC 2

/* @} */

/*! \enum  IFX_MIPS_TC  
    \brief Multithread Index 
 */
enum IFX_MIPS_TC{
    IFX_MIPS_TC0 = 0, /*!< MT TC 0 */
    IFX_MIPS_TC1,     /*!< MT TC 1 */
    IFX_MIPS_TC2,     /*!< MT TC 2 */ 
    IFX_MIPS_TC3,     /*!< MT TC 3 */
};

/*!
  \addtogroup IFX_PMON_IOCTL
 */
/* @{ */
/*!
    \brief Structure describing pmon version
 */
struct ifx_pmon_ioctl_version {
    unsigned int    major;   /*!< Version Major number */
    unsigned int    mid;     /*!< Version Mid number */
    unsigned int    minor;   /*!< Version Minor number */
};

/*!
    \brief Structure describing pmon event
 */
struct ifx_pmon_ioctl_event {
    unsigned int    pmon_event;   /*!< PMON external even id */
    unsigned int    counter;     /*!< CPU performance counter 0 or 1 */
    unsigned int    tc;           /*!< MT TC index 0~3*/
};

/*! 
 * \def IFX_PMON_IOC_MAGIC
 * \brief PMON IOCTL Magic number
 */
#define IFX_PMON_IOC_MAGIC               0xef
/*! 
 * \def IFX_PMON_IOC_VERSION
 * \brief PMON IOCTL to get version number
 */
#define IFX_PMON_IOC_VERSION             _IOR( IFX_PMON_IOC_MAGIC, 0, struct ifx_pmon_ioctl_version)
/*! 
 * \def IFX_PMON_IOC_EVENT
 * \brief PMON IOCTL to configure external event and CPU performance counter 0/1
 */
#define IFX_PMON_IOC_EVENT               _IOWR(IFX_PMON_IOC_MAGIC, 1, struct ifx_pmon_ioctl_event)
/*! 
 * \def IFX_PMON_IOC_DISABLE
 * \brief PMON IOCTL to disable PMON module
 */

#define IFX_PMON_IOC_DISABLE             _IOWR(IFX_PMON_IOC_MAGIC, 2, int)
/* @} */

#endif /* IFX_PMON_H */

