/**
** FILE NAME    : ifx_mmc_wlan.h
** PROJECT      : IFX UEIP
** MODULES      : MMC module for WLAN
** DATE         : 
** AUTHOR       : Ralf Janssen
** DESCRIPTION  : IFX Cross-Platform MMC for WLAN device driver header file
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
** 12 May 2010         Ralf Janssen               Initial release
*******************************************************************************/
/*!
  \file ifx_mmc_wlan.h
  \ingroup IFX_MMC_DRV
  \brief MMC host driver for WLAN SDIO Card
*/

#ifndef __IFX_SDIO_WLAN_H__
#define __IFX_SDIO_WLAN_H__

/*!
  @brief Handle, that is returned by \ref ifx_sdio_open and is passed to all SDIO functions.
 */
typedef uint32_t ifx_sdio_handle_t;

/*!
  @brief Function pointer to callback handler.

  The WLAN driver (or any other driver using this SDIO driver) can use this callback handler
  to get informed, when an asynchronous transaction has been completed.
 */
typedef void (*ifx_sdio_callback_handler_t)(void *context, int status);

/*!
  @brief Structure containing the required configuration and default parameters for the SDIO driver.

  This structure is passed to \ref ifx_sdio_open to configure the SDIO interface with respect to
  clock, function select, block/byte mode, bus width, CRC handling, interrupt path (in-band/out-of-band)
  and which GPIO is used for reset.
 */
typedef struct ifx_sdio_config {
    ifx_sdio_callback_handler_t callback_handler; /*!< Callback handler, used for async operations */
    void *context;              /*!< Context of SDIO device driver,
                                     will be passed to async callback handlers. */
    uint32_t clock;             /*!< SDIO clock freuqency in Hz */

        /* The following parameters are used to pre-define some parameters */
        /* used with almost all SDIO commands (CMD52/CMD53). */
    uint8_t function;           /*!< Default function number in CMD52/53 */
    uint8_t raw;                /*!< Default RAW flag in CMD52 */
    uint8_t block_mode;         /*!< Default block-/byte-mode flag in CMD53 */
    uint8_t opcode;             /*!< Default opcode flag in CMD53 */

    uint16_t block_len;         /*!< @brief Default block length for CMD53 block transfers */
    uint8_t bus_width;          /*!< 1-/4-Bit bus */

        /* The SDIO controller does not generate a CRC in byte mode */
        /* Therefore the driver allows to calculate and append the CRC in SW  */
    uint8_t crc;                /*!< If set, calculate and append CRC for CMD53 byte mode */
    uint8_t irq_oob;            /*!< If set, use Out-Of-Band interrupt via GPIO29 */
    uint8_t rst_gpio;           /*!< GPIO-Number for external reset (255 for none) */
} ifx_sdio_config_t;


/*!
  @brief Structure containing definition for SDIO commands.

  This structure contains information about the command (0..63), the argument of the command
  and the response type (\ref MMC_RSP_TYPES).
 */
typedef struct ifx_sdio_cmd {
    uint32_t op_code;           /*!< SDIO command (0..63) */
    uint32_t args;              /*!< argument to SDIO command */
    uint32_t response_type;     /*!< Response type (\ref MMC_RSP_TYPES) */
    uint32_t response[4];       /*!< Short/Long response from SDIO device */
    int error;                  /*!< return code of \ref ifx_sdio_cmd */
} ifx_sdio_cmd_t;

/*!
  @internal
 */
/*@{*/
/* response types, copied from linux-2.6.26.1/include/linux/mmc/core.h */
#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136     (1 << 1)                /* 136 bit response */
#define MMC_RSP_CRC     (1 << 2)                /* expect valid crc */
#define MMC_RSP_BUSY    (1 << 3)                /* card may send busy */
#define MMC_RSP_OPCODE  (1 << 4)                /* response contains opcode */

/*!
  @defgroup MMC_RSP_TYPES SDIO Response types
  @{
 */
#define MMC_RSP_NONE    (0)     /*!< No response expected  */
#define MMC_RSP_R1      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE) /*!< Response R1 expected */
#define MMC_RSP_R1B     (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY) /*!< Response R1B expected */
#define MMC_RSP_R2      (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC) /*!< Response R2 expected */
#define MMC_RSP_R3      (MMC_RSP_PRESENT) /*!< Response R3 expected */
#define MMC_RSP_R4      (MMC_RSP_PRESENT) /*!< Response R4 expected */
#define MMC_RSP_R5      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE) /*!< Response R5 expected */
#define MMC_RSP_R6      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE) /*!< Response R6 expected */
#define MMC_RSP_R7      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE) /*!< Response R7 expected */
/* @} */


/*
 * Exported functions
 */

/* open SDIO interface and set configuration */
/*!
 * @brief
 *      This function opens the SDIO interface.
 *
 *      It initializes all registers and activates the module clock.\n
 *      The second parameter allows to configure the operational mode.
 * @param[in]
 *      dev_index       Device index starting with 0 for the first SDIO device.
 * @param[in]
 *      cfg             Requested configuration of the SDIO interface.
 * @return
 *      < 0             In case of errors (see errno.h for error codes).\n
 *      >=0             Device handle that is used to call SDIO functions.
 */
ifx_sdio_handle_t ifx_sdio_open(int dev_index, ifx_sdio_config_t *cfg);


/* close SDIO interface and release resources */
/*!
 * @brief
 *      This function closes the SDIO interface.
 *
 *      It clears all registers and deactivates the module clock.
 * @param[in]
 *      sh              Device handle returned by ifx_sdio_open
 * @return
 *      < 0             In case of errors (see errno.h for error codes).\n
 *      0               If no error ocurred.
 */
int ifx_sdio_close(ifx_sdio_handle_t sh);


/* configure various settings of the driver */
/*!
 * @brief
 *      Provide ioctl style function for configuration purposes.
 *
 *
 * @param[in]
 *      sh              Device handle returned by \ref ifx_sdio_open
 * @param[in]
 *      command         Command that should be executed (\ref IFX_SDIO_IOCTL_COMMANDS).\n
 * @param[in, out]
 *      param           Parameter to command. The meaning is command specific
 *
 * @return
 *      < 0             In case of errors (see errno.h for error codes).\n
 *      0               If no error ocurred.
 */
int ifx_sdio_ioctl(ifx_sdio_handle_t sh, uint32_t command, uint32_t param);

/* Request SDIO device interrupt line from kernel and enable interrupt in SDIO core */
int ifx_sdio_request_irq(ifx_sdio_handle_t sh, irq_handler_t handler,
                         unsigned long flags, const char *dev_name,
                         void *dev_id);

/* Disable interrupt in SDIO core and free interrupt line. */
int ifx_sdio_free_irq (ifx_sdio_handle_t sh, void *dev_id);

/* This functions enables the SDIO device interrupt. */
int ifx_sdio_enable_irq (ifx_sdio_handle_t sh);

/* This function disables the SDIO device interrupt. */
int ifx_sdio_disable_irq (ifx_sdio_handle_t sh);

/* send a command to the SDIO device and wait for response */
int ifx_sdio_cmd(ifx_sdio_handle_t sh, ifx_sdio_cmd_t *cmd);

/* send a command to the SDIO device and wait for response,
   provide all parameters directly */
int ifx_sdio_cmd_ext(ifx_sdio_handle_t sh, uint8_t op_code, uint32_t args,
                     uint32_t response_type, uint32_t *response);

/* write one byte via CMD52 to SDIO device */
int ifx_sdio_write_byte(ifx_sdio_handle_t sh, uint32_t address,
                        uint8_t in, uint8_t *out);

/* write one byte via CMD52 to SDIO device, but provide all parameter */
int ifx_sdio_write_byte_ext(ifx_sdio_handle_t sh, uint32_t address,
                            uint8_t function, uint8_t raw,
                            uint8_t in, uint8_t *out);

/* Write arbitrary number of bytes to device.
   Return, when all data has been transferred. */
int ifx_sdio_write_sync(ifx_sdio_handle_t sh, uint32_t address,
                        void *data, uint32_t count);

/* Write arbitrary number of bytes to device, but provide all parameter.
   Return, when all data has been transferred. */
int ifx_sdio_write_sync_ext(ifx_sdio_handle_t sh, uint32_t address,
                            void *data, uint32_t count,
                            uint8_t function, uint8_t block_len_pow, uint8_t opcode);

/* Write arbitrary number of bytes to device.
   Return immediately after starting the transfer. */
int ifx_sdio_write_async(ifx_sdio_handle_t sh, uint32_t address,
                         void *data, uint32_t count);

/* Write arbitrary number of bytes to device, but provide all parameter
   Return immediately after starting the transfer. */
int ifx_sdio_write_async_ext(ifx_sdio_handle_t sh, uint32_t address,
                             void *data, uint32_t count,
                             uint8_t function, uint8_t block_len_pow, uint8_t opcode);

/* Read a single byte via CMD52. */
int ifx_sdio_read_byte(ifx_sdio_handle_t sh, uint32_t address, uint8_t *pdata);

/* Read a single byte via CMD52, but provide all paramter. */
int ifx_sdio_read_byte_ext(ifx_sdio_handle_t sh, uint32_t address,
                           uint8_t function, uint8_t *pdata);

/* Read arbitrary number of bytes from device.
   Return, when all data has been transferred. */
int ifx_sdio_read_sync (ifx_sdio_handle_t sh, uint32_t address,
                        void *data, uint32_t count);

/* Read arbitrary number of bytes from device, but provide all parameter.
   Return, when all data has been transferred. */
int ifx_sdio_read_sync_ext (ifx_sdio_handle_t sh, uint32_t address,
                            void *data, uint32_t count,
                            uint8_t function, uint8_t block_len_pow, uint8_t opcode);

/* Read arbitrary number of bytes from device.
   Return immediately after starting the transfer. */
int ifx_sdio_read_async (ifx_sdio_handle_t sh, uint32_t address,
                         void *data, uint32_t count);

/* Read arbitrary number of bytes from device, but provide all parameter.
   Return immediately after starting the transfer. */
int ifx_sdio_read_async_ext(ifx_sdio_handle_t sh, uint32_t address,
                            void *data, uint32_t count,
                            uint8_t function, uint8_t block_len_pow, uint8_t opcode);



int ifx_sdio_enumerate(ifx_sdio_handle_t sh);

int ifx_sdio_poll_async_state(ifx_sdio_handle_t sh);


/*
 * IOCTL commands
 */
/*!
 @defgroup IFX_SDIO_IOCTL_COMMANDS IFX SDIO ioctl commands
 @{

        This section lists the available ioctl commands of the SDIO driver.
        Also the meaning of the \ref ifx_sdio_ioctl parameter
        is described per command.
*/

/*!
        IFX_SDIO_IOCTL_SET_CLK sets the clock frequency of SDIO interface in Hz.

        @param The requested SDIO clock in HZ is specified as parameter in \ref ifx_sdio_ioctl.
        The actual frequency is rounded to the next valid value less or equal to the requested one.
*/
#define IFX_SDIO_IOCTL_SET_CLK          1

/*!
        IFX_SDIO_IOCTL_SET_BUS_WIDTH select the bus width of the SDIO interface, when using CMD53.

        @param Bus width selection as defined by \ref IFX_SDIO_IOCTL_PARAM_BUS_WIDTH.
 */
#define IFX_SDIO_IOCTL_SET_BUS_WIDTH    2

/*!
        IFX_SDIO_IOCTL_SET_BLOCK_LENGTH sets the block length for CMD53 block-transactions.

        @param Block length in bytes. The block length must be a power of 2.
 */
#define IFX_SDIO_IOCTL_SET_BLOCK_LENGTH 3

/*!
        IFX_SDIO_IOCTL_SET_FUNCTION modifies the by default selected function withtin the SDIO device.
        This feature has not been implemented, yet.

        @param Function number (0..7) of SDIO device.
 */
#define IFX_SDIO_IOCTL_SET_FUNCTION     4
/* @} */

/*!
  @defgroup IFX_SDIO_IOCTL_PARAM_BUS_WIDTH ioctl parameter for command \ref IFX_SDIO_IOCTL_SET_BUS_WIDTH
  @{
 */
#define IFX_SDIO_IOCTL_BUS_WIDTH_1      0 /*!< Set Bus width to 1-Bit */
#define IFX_SDIO_IOCTL_BUS_WIDTH_4      1 /*!< Set Bus width to 4-Bit */
/* @} */

#endif /* __IFX_SDIO_WLAN_H__ */
