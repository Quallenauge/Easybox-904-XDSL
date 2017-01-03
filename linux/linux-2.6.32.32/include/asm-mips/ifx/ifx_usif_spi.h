/******************************************************************************
**
** FILE NAME    : ifx_usif_spi.h
** PROJECT      : IFX UEIP for VRX200
** MODULES      : USIF for SPI Mode
**
** DATE         : 03 Jun 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : USIF SPI mode for counterpart API
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
** HISTORY
** $Date        $Author         $Comment
** 03 Jun,2009  Lei Chuanhua    Initial version
*******************************************************************************/

#ifndef IFX_USIF_SPI_H
#define IFX_USIF_SPI_H

/*!
  \defgroup IFX_USIF_SPI IFX USIF SPI mode module
  \brief ifx usif spi mode driver module
*/

/*!
  \defgroup IFX_USIF_SPI_DEFINITIONS Defintion and structures
  \ingroup IFX_USIF_SPI
  \brief definitions for ifx usif spi mode driver 
*/

/*!
  \defgroup IFX_USIF_SPI_FUNCTIONS External APIs
  \ingroup IFX_USIF_SPI
  \brief usif spi external driver functions
*/

/*!
  \defgroup IFX_USIF_SPI_INTERNAL Internal functions
  \ingroup IFX_USIF_SPI
  \brief usif spi internal driver functions
*/

/*!
  \file ifx_usif_spi.h
  \brief header file for usif spi driver
*/

/* @{ */
/*!  
 \addtogroup IFX_USIF_SPI_DEFINITIONS
*/
/*! \enum  IFX_USIF_SPI_PRIO_t  
    \brief Set communication priority of USIF_SPI connection
    
    Three priority levels are defined. Low-level priority queue used for applications like FLASH driver 
    (ifx_usif_spi_prio_low). normal priority queue used for applications like display (ifx_usif_spi_prio_mid). 
    High priority queue used for applications like RTP packet transfer (ifx_usif_spi_prio_high).    
 */
typedef enum{
    IFX_USIF_SPI_PRIO_LOW = 0, /*!< Low Priority queue. For FLASH driver, etc. */
    IFX_USIF_SPI_PRIO_MID,     /*!< Normal Priority queue. For LCD, display data, etc. */
    IFX_USIF_SPI_PRIO_HIGH,    /*!< High priority queue. Ror RTP voice, etc. */
    IFX_USIF_SPI_PRIO_ASYNC,   /*!< Tasklet priority (This is the highest supported priority). 
                               For this priority level only the asynchronous API set 
                               of the SSC driver can be called. These funtions stay unblocked and a 
                               callback function is called when the request is processed. This allows 
                               that the APIs are called from tasklet level. The callback function is 
                               always called on tasklet level */
}IFX_USIF_SPI_PRIO_t;

#define IFX_USIF_SPI_PRIO_MAX IFX_USIF_SPI_PRIO_ASYNC


/*! \enum  IFX_USIF_SPI_MODE_t  
    \brief Defines the Ssc hardware mode settings supported

    Because there is no official specification, what exactly SPI is and what not, it is necessary 
    to consult the data sheets of the components one wants to use. Important are the permitted clock 
    frequencies and the type of valid transitions. There are no general rules for transitions where 
    data should be latched. Although not specified by Motorola, in practice four modes are used. 
    These four modes are the combinations of CPOL and CPHA. In table 1, the four modes are listed.If 
    the phase of the clock is zero, i.e. CPHA = 0, data is latched at the rising edge of the clock with 
    CPOL = 0, and at the falling edge of the clock with CPOL = 1. If CPHA = 1, the polarities are reversed. 
    CPOL = 0 means falling edge, CPOL = 1 rising edge.The micro controllers from Motorola allow the polarity 
    and the phase of the clock to be adjusted. A positive polarity results in latching data at the rising 
    edge of the clock. However data is put on the data line already at the falling edge in order to stabilize.
    Most peripherals which can only be slaves, work with this configuration. If it should become necessary 
    to use the other polarity, transitions are reversed.    
  */
typedef enum {
    IFX_USIF_SPI_MODE_0 = 0,  /*!< CPOL=0,CPHA=0 */
    IFX_USIF_SPI_MODE_1,      /*!< CPOL=0,CPHA=1 */
    IFX_USIF_SPI_MODE_2,      /*!< CPOL=1,CPHA=0 */
    IFX_USIF_SPI_MODE_3,      /*!< CPOL=1,CPHA=1 */
    IFX_USIF_SPI_MODE_UNKNOWN,/*!< Unknown SPI mode */
} IFX_USIF_SPI_MODE_t;

/*! \enum  IFX_USIF_SPI_HANDL_TYPE_t  
    \brief Defines the USIF SPI handler type supported
  */
typedef enum {
   IFX_USIF_SPI_HANDL_TYPE_SYNC = 0, /*!< Only SYNC handler which be used by sync application */
   IFX_USIF_SPI_HANDL_TYPE_ASYNC,    /*!< Only ASYNC handler which be used by async application */
}IFX_USIF_SPI_HANDL_TYPE_t;

/*!< \typedef IFX_USIF_SPI_CS_DATA_t
     \brief Definition of device specific data for chip select
 */
typedef int  IFX_USIF_SPI_CS_DATA_t;

/* chip select number */
#define IFX_USIF_SPI_CS0    0
#define IFX_USIF_SPI_CS1    1
#define IFX_USIF_SPI_CS2    2
#define IFX_USIF_SPI_CS3    3
#define IFX_USIF_SPI_CS4    4
#define IFX_USIF_SPI_CS5    5
#define IFX_USIF_SPI_CS6    6
#define IFX_USIF_SPI_CS7    7

enum {
    IFX_USIF_SPI_CS_ON = 0,
    IFX_USIF_SPI_CS_OFF,
};

/*! \typedef IFX_USIF_SPI_CS_CB_t
    \brief Chip Select Callback function type declaration 
    
     csq csq=0, i.e. CS low (active). csq=1, i.e.CS high (idle).
     cs_data This is the device number in case if more than one device is using the same registered driver.
     E.g. two VINETICs, each with it´s own CS.
 */
typedef int  (*IFX_USIF_SPI_CS_CB_t)(u32 csq, IFX_USIF_SPI_CS_DATA_t cs_data);

/*! typedef ifx_usif_spi_async_fkt_cb_t
    \brief Callback definition for asynchronous SSC API calls. This callback is called
     by the SSC driver on tasklet level after the request is completed or exit with
     an error.

    \param handle Funktion handle that provided for callback registration
         during the SSC asynchronous API call.
    \param retvalue This return value describe if the asynchronous request
         identified an error and worked successfully.
*/
typedef void (*ifx_usif_spi_async_fkt_cb_t)(int handle, int retvalue);

/*! typedef IFX_SSC_ASYNC_CALLBACK_t
    \brief Parameter to specify the asynchronous callback. It is called by the SSC
    tasklet after the requested transmission or locking is done.
*/
typedef struct{
    ifx_usif_spi_async_fkt_cb_t  pFunction; /*!< Callback function pointer, called in the SSC tasklet 
                                              when the requested command is executed*/
    int                          functionHandle; /*!< Callback function handle. This parameter is 
                                             transparently given to the callback function without 
                                             any modification by the SSC driver*/
}IFX_USIF_SPI_ASYNC_CALLBACK_t;


/*!  \brief Parameter structure used to configure an USIF_SPI connection "ConnId".
 */
typedef struct{
    IFX_USIF_SPI_MODE_t  spi_mode; /*!< Defines the hardware setup mode of the USIF_SPI */
    IFX_USIF_SPI_PRIO_t  spi_prio; /*!< The communication priority of USIF_SPI connection.
                                   Three priority levels are defined. Low-level priority queue used for applications 
                                   like FLASH driver (ifx_usif_spi_prio_low). Normal priority queue used for applications 
                                   like display (ifx_usif_spi_prio_mid). High priority queue used for applications like RTP 
                                   packet transfer (ifx_usif_spi_prio_high). The here defined USIF_SPI priority corresponds to 
                                   the priority the USIF_SPI driver application and should be set just for information when 
                                   the driver is registered by the application. 
                               */
    int             baudrate; /*!< Baudrate used for the ConnId.
                                 This parameter can be later be modified by a call of ifx_usif_spiSetBaud 
                               */
    int             fragSize; /*!< All transmitted and received packets should be fragmented in this fragment 
                                 size. Size given in Bytes. A maximum of 1024 Bytes is allowed. If the client 
                                 uses a bigger values here, USIF_SPI returns with error. The client has to take 
                                 care of the fragmentation 
                               */
    int             maxFIFOSize; /*!< Maximum packet size in FIFO mode.
                                   All transmitted and received packets are transmitted in DMA mode if the packet 
                                   size is greater than this value. A value of 148 is recommended at first. 
                                   Size given in Bytes
                                  */
    IFX_USIF_SPI_CS_CB_t csset_cb;   /*!< Function Callback called by USIF_SPI driver when it starts/stops to receive or transmit */
    IFX_USIF_SPI_CS_DATA_t cs_data;    /*!< Parameter used for the function call of "csSet_cb". */
} IFX_USIF_SPI_CONFIGURE_t;

/*! \typedef IFX_USIF_SPI_HANDLE_t
    \brief Definition of the connection handle
    
     as it is used by the client kernel module that use the USIF_SPI driver
     Inside of the USIF_SPI driver, this handle is mapped to an internal structure that contains the connection specific 
     parameter (e.g. Baudrate, Chipselect Callbacks, etc.).
 */
typedef void * IFX_USIF_SPI_HANDLE_t;
/* @} */

extern int ifx_usif_spi_cs_low(u32 pin);
extern int ifx_usif_spi_cs_high(u32 pin);
extern int ifx_usif_spiLock(IFX_USIF_SPI_HANDLE_t handler);
extern int ifx_usif_spiUnlock(IFX_USIF_SPI_HANDLE_t handler);
extern int ifx_usif_spiSetBaud(IFX_USIF_SPI_HANDLE_t handler, unsigned int baud);
extern int ifx_usif_spiTxRx(IFX_USIF_SPI_HANDLE_t handler, char* txbuf, 
    u32 txlen,char* rxbuf, u32 rxlen);
extern int ifx_usif_spiRx(IFX_USIF_SPI_HANDLE_t handler, char *rxbuf, u32 rxlen);
extern int ifx_usif_spiTx(IFX_USIF_SPI_HANDLE_t handler, char *txbuf, u32 txlen);
extern IFX_USIF_SPI_HANDLE_t ifx_usif_spiAllocConnection (char *dev_name, IFX_USIF_SPI_CONFIGURE_t *connid);
extern int ifx_usif_spiFreeConnection (IFX_USIF_SPI_HANDLE_t handler);
extern int ifx_usif_spiAsyncTxRx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
        char *txbuf, int txsize, char *rxbuf, int rxsize);
extern int ifx_usif_spiAsyncTx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
    char *txbuf, int txsize);
extern int ifx_usif_spiAsyncRx(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback,
    char *rxbuf, int rxsize);

extern int ifx_usif_spiAsyncLock(IFX_USIF_SPI_HANDLE_t handler, IFX_USIF_SPI_ASYNC_CALLBACK_t *pCallback);
extern int ifx_usif_spiAsyncUnLock(IFX_USIF_SPI_HANDLE_t handler);

#endif /* IFX_USIF_SPI_H */

