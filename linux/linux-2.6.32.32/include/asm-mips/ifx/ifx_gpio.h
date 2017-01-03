/******************************************************************************
**
** FILE NAME    : ifx_gpio.h
** PROJECT      : UEIP
** MODULES      : GPIO
**
** DATE         : 3 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global GPIO driver header file
** COPYRIGHT    :       Copyright (c) 2006
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
** 03 JUL 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFX_GPIO_H
#define IFX_GPIO_H



/*!
  \defgroup IFX_GPIO UEIP Project - GPIO driver module
  \brief UEIP Project - GPIO driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_GPIO_API APIs
  \ingroup IFX_GPIO
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_GPIO_IOCTL IOCTL Commands
  \ingroup IFX_GPIO
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_GPIO_STRUCT Structures
  \ingroup IFX_GPIO
  \brief Structures used by user application.
 */

/*!
  \file ifx_gpio.h
  \ingroup IFX_GPIO
  \brief GPIO driver header file
 */



/*
 * ####################################
 *              Definition
 * ####################################
 */

#define IFX_GPIO_PIN_NUMBER_PER_PORT    16
#define IFX_GPIO_PIN_ID(port, pin)      ((port) * IFX_GPIO_PIN_NUMBER_PER_PORT + (pin))
#define IFX_GPIO_PIN_ID_TO_PORT(pin_id) (pin_id >> 4)
#define IFX_GPIO_PIN_ID_TO_PIN(pin_id)  (pin_id & 0x0F)

enum {
    IFX_GPIO_PIN_AVAILABLE  = 0,
    IFX_GPIO_MODULE_MIN     = IFX_GPIO_PIN_AVAILABLE,
    //  following are module_id used by component while registering
    IFX_GPIO_MODULE_TEST,
    IFX_GPIO_MODULE_MEI,
    IFX_GPIO_MODULE_DSL_NTR,
    IFX_GPIO_MODULE_SSC,
    IFX_GPIO_MODULE_ASC0,
    IFX_GPIO_MODULE_SDIO,
    IFX_GPIO_MODULE_LEDC,
    IFX_GPIO_MODULE_USB,
    IFX_GPIO_MODULE_INTERNAL_SWITCH,
    IFX_GPIO_MODULE_PCI,
    IFX_GPIO_MODULE_PCIE,
    IFX_GPIO_MODULE_NAND,
    IFX_GPIO_MODULE_PPA,
    IFX_GPIO_MODULE_TAPI_VMMC,
    IFX_GPIO_MODULE_TAPI_DEMO,
    IFX_GPIO_MODULE_TAPI_FXO,
    IFX_GPIO_MODULE_TAPI_DXT,
    IFX_GPIO_MODULE_TAPI_VCPE,
    IFX_GPIO_MODULE_VINAX,
    IFX_GPIO_MODULE_USIF_UART,
    IFX_GPIO_MODULE_USIF_SPI,
    IFX_GPIO_MODULE_SPI_FLASH,
    IFX_GPIO_MODULE_SPI_EEPROM,
    IFX_GPIO_MODULE_USIF_SPI_SFLASH,
    IFX_GPIO_MODULE_LED,
    IFX_GPIO_MODULE_EBU_LED,
    IFX_GPIO_MODULE_EXIN,   //  external interrupt detection unit
    IFX_GPIO_MODULE_PAGE,   //  page button gpio
    IFX_GPIO_MODULE_DECT,   //  DECT
    IFX_GPIO_MODULE_SI,
    IFX_GPIO_MODULE_BUTTON, // ctc
    IFX_GPIO_MODULE_LCD, // ctc
    IFX_GPIO_MODULE_TP, // ctc
    IFX_GPIO_MODULE_SIPO, // ctc
    //  add more component in the future
    IFX_GPIO_MODULE_MAX,
    //  flag to init module during system bootup no matter whether this module is loaded or not
    IFX_GPIO_MODULE_EARLY_REGISTER = 0x08000000,    //  this bit flag trigger pin init during system bootup
};

#define IFX_GPIO_DECLARE_MODULE_NAME(var)   \
    char *var[] = {                         \
        "Available",                        \
        "TEST",                             \
        "MEI",                              \
        "DSL-NTR",                          \
        "SSC",                              \
        "ASC0",                             \
        "SDIO",                             \
        "LEDC",                             \
        "USB",                              \
        "INT_SW",                           \
        "PCI",                              \
        "PCIE",                             \
        "NAND",                             \
        "PPA",                              \
        "TAPI-VMMC",                        \
        "TAPI-DEMO",                        \
        "TAPI-FXO",                         \
        "TAPI-DXT",                         \
        "TAPI-VCPE",                        \
        "VINAX",                            \
        "USIF-UART",                        \
        "USIF-SPI",                         \
        "SFLASH",                           \
        "EEPROM",                           \
        "USIF-SFLASH",                      \
        "LED",                              \
        "EBU_LED",                          \
        "EXIN",                             \
        "PAGE_BUTTON" /*ctc*/,              \
        "DECT",                             \
        "SERIAL_IN",                        \
        "BUTTON" /*ctc*/,                   \
        "LCD" /*ctc*/,                   \
        "TP" /*ctc*/,                   \
        "SIPO" /*ctc*/,                   \
    }



/*
 * ####################################
 *                IOCTL
 * ####################################
 */

/*!
  \addtogroup IFX_GPIO_STRUCT
 */
/*@{*/

/*!
  \struct ifx_gpio_ioctl_parm
  \brief Structure used for single pin configuration.

  User application use this structure to call single pin configuration IOCTL commands:
  IFX_GPIO_IOC_OD, IFX_GPIO_IOC_PUDSEL, IFX_GPIO_IOC_PUDEN, IFX_GPIO_IOC_STOFF,
  IFX_GPIO_IOC_DIR, IFX_GPIO_IOC_OUTPUT, IFX_GPIO_IOC_INPUT, IFX_GPIO_IOC_ALTSEL0,
  IFX_GPIO_IOC_ALTSEL1. "port", "pin" and "module" are input. Driver takes input
  "port" and "pin" to identify which pin to configure. "module" is module ID so that
  driver can do sanity check to avoid improper pin usage (e.g. two module share one pin).
  "value" could be either input or output.
 */
struct ifx_gpio_ioctl_parm {
    int port;          /*!< input, GPIO port number 0 ~ 3           */
    int pin;           /*!< input, GPIO pin number 0 ~ 15           */
    int value;         /*!< input/output, value to be set (or get)  */
    int module;        /*!< input, module id of this operation      */
};

/*!
  \struct ifx_gpio_ioctl_version
  \brief Structure used for query of driver version.
 */
struct ifx_gpio_ioctl_version {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver */
    unsigned int    minor;  /*!< output, minor number of driver */
};

/*!
  \struct ifx_gpio_ioctl_pin_reserve
  \brief Structure used to reserve pin.
 */
struct ifx_gpio_ioctl_pin_reserve {
    int             pin;        /*!< input, pin ID */
    int             module_id;  /*!< input, module ID */
};

/*!
  \struct ifx_gpio_ioctl_pin_config
  \brief Structure used for single pin configuration (IFX_GPIO_IOC_PIN_CONFIG).

  User application use this structure to call pin configuration IOCTL command - IFX_GPIO_IOC_PIN_CONFIG.
 */
struct ifx_gpio_ioctl_pin_config {
    int             module_id;  /*!< input, module ID */
    int             pin;        /*!< input, pin ID */
#define IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET        (1 << 0)    /*!< config flag, enable Open Drain */
#define IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR      (1 << 1)    /*!< config flag, disable Open Drain */
#define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET    (1 << 2)    /*!< config flag, set Pull-Up */
#define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_CLEAR  (1 << 3)    /*!< config flag, set Pull-Down */
#define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET     (1 << 4)    /*!< config flag, enable Pull-Up/Down */
#define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_CLEAR   (1 << 5)    /*!< config flag, disable Pull-Up/Down */
#define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_SET     (1 << 6)    /*!< config flag, enable Schmitt Trigger */
#define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_CLEAR   (1 << 7)    /*!< config flag, disable Schmitt Trigger */
#define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT       (1 << 8)    /*!< config flag, configure GPIO pin as output */
#define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN        (1 << 9)    /*!< config flag, configure GPIO pin as input */
#define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET    (1 << 10)   /*!< config flag, output 1 */
#define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_CLEAR  (1 << 11)   /*!< config flag, output 0 */
#define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET   (1 << 12)   /*!< config flag, set Alternative Select 0 with value 1 */
#define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR (1 << 13)   /*!< config flag, set Alternative Select 0 with value 0 */
#define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET   (1 << 14)   /*!< config flag, set Alternative Select 1 with value 1 */
#define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR (1 << 15)   /*!< config flag, set Alternative Select 1 with value 0 */
    unsigned int    config;     /*!< input, config flags */
};

/*!
  \struct ifx_gpio_ioctl_module_query
  \brief Structure used for query of port.

  User application use this structure to call IOCTL command - IFX_GPIO_IOC_MODULE_QUERY.
  Driver will check the GPIO port and return all pins' status of this port.
 */
struct ifx_gpio_ioctl_module_query {
    int             module_id;  /*!< input, ID of port (0 ~ 3) */
    unsigned int    pin_num;    /*!< input, number of pins in this port */
    unsigned int    pin[IFX_GPIO_PIN_NUMBER_PER_PORT];  /*!< input, status of each pin (module using this pin) */
};

/*!
  \struct ifx_gpio_ioctl_pin_query
  \brief Structure used for query of pin.

  User application use this structure to call IOCTL command - IFX_GPIO_IOC_PIN_CONFIG.
  Driver will check the module ID using this pin. If this pin is configured as input,
  the input value will be stored in member "input".
 */
struct ifx_gpio_ioctl_pin_query {
    int             pin;        /*!< input, pin ID to query */
    int             module_id;  /*!< output, module ID using this pin, ZERO if the pin is not reserved */
    int             input;      /*!< output, input value if this pin is configured as input */
};

/*@}*/


/*!
  \addtogroup IFX_GPIO_IOCTL
 */
/*@{*/
#define IFX_GPIO_IOC_MAGIC              0xbf
//  backward compatible
/*!
  \def IFX_GPIO_IOC_OD
  \brief GPIO IOCTL Command - Configure Open Drain for given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to configure Open Drain for given pin.
 */
#define IFX_GPIO_IOC_OD                 _IOW( IFX_GPIO_IOC_MAGIC,  0, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_PUDSEL
  \brief GPIO IOCTL Command - Configure Pull-Up/Down resistor for given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to configure Pull-Up/Down resistor for given pin.
 */
#define IFX_GPIO_IOC_PUDSEL             _IOW( IFX_GPIO_IOC_MAGIC,  1, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_PUDEN
  \brief GPIO IOCTL Command - Enable/Disable Pull-Up/Down feature for given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to enable/disable Pull-Up/Down feature for given pin.
 */
#define IFX_GPIO_IOC_PUDEN              _IOW( IFX_GPIO_IOC_MAGIC,  2, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_STOFF
  \brief GPIO IOCTL Command - Enable/Disable Pull-Up/Down feature for given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to enable/disable Pull-Up/Down feature for given pin.
 */
#define IFX_GPIO_IOC_STOFF              _IOW( IFX_GPIO_IOC_MAGIC,  3, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_DIR
  \brief GPIO IOCTL Command - Configure given pin to be input or output.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to configure given pin to be input or output.
 */
#define IFX_GPIO_IOC_DIR                _IOW( IFX_GPIO_IOC_MAGIC,  4, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_OUTPUT
  \brief GPIO IOCTL Command - Configure given pin to output 0 or 1.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to configure given pin to output 0 or 1.
 */
#define IFX_GPIO_IOC_OUTPUT             _IOW( IFX_GPIO_IOC_MAGIC,  5, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_INPUT
  \brief GPIO IOCTL Command - Collect input from given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to collect input from given pin.
 */
#define IFX_GPIO_IOC_INPUT              _IOWR(IFX_GPIO_IOC_MAGIC,  6, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_ALTSEL0
  \brief GPIO IOCTL Command - Configure Alternative Select 0 for given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to configure Alternative Select 0 for given pin.
 */
#define IFX_GPIO_IOC_ALTSEL0            _IOW( IFX_GPIO_IOC_MAGIC,  7, struct ifx_gpio_ioctl_parm)
/*!
  \def IFX_GPIO_IOC_ALTSEL1
  \brief GPIO IOCTL Command - Configure Alternative Select 1 for given pin.

   This command uses structure "ifx_gpio_ioctl_parm" as parameter to configure Alternative Select 1 for given pin.
 */
#define IFX_GPIO_IOC_ALTSEL1            _IOW( IFX_GPIO_IOC_MAGIC,  8, struct ifx_gpio_ioctl_parm)
//  UEIP IOCTL commands
/*!
  \def IFX_GPIO_IOC_VERSION
  \brief GPIO IOCTL Command - Get version number of driver.

   This command uses structure "ifx_gpio_ioctl_version" as parameter to get version number of driver.
 */
#define IFX_GPIO_IOC_VERSION            _IOR( IFX_GPIO_IOC_MAGIC, 10, struct ifx_gpio_ioctl_version)
/*!
  \def IFX_GPIO_IOC_REGISTER
  \brief GPIO IOCTL Command - Register module.

   This command uses "arg" as parameter (module_id) to register module.
 */
#define IFX_GPIO_IOC_REGISTER           _IO(  IFX_GPIO_IOC_MAGIC, 11)
/*!
  \def IFX_GPIO_IOC_DEREGISTER
  \brief GPIO IOCTL Command - Deregister module.

   This command uses "arg" as parameter (module_id) to deregister module.
 */
#define IFX_GPIO_IOC_DEREGISTER         _IO(  IFX_GPIO_IOC_MAGIC, 12)
/*!
  \def IFX_GPIO_IOC_PIN_RESERVE
  \brief GPIO IOCTL Command - Reserve pin.

   This command uses struct "ifx_gpio_ioctl_pin_reserve" as parameter to reserve pin.
 */
#define IFX_GPIO_IOC_PIN_RESERVE        _IOW( IFX_GPIO_IOC_MAGIC, 13, struct ifx_gpio_ioctl_pin_reserve)
/*!
  \def IFX_GPIO_IOC_PIN_FREE
  \brief GPIO IOCTL Command - Free pin.

   This command uses struct "ifx_gpio_ioctl_pin_reserve" as parameter to free pin.
 */
#define IFX_GPIO_IOC_PIN_FREE           _IOW( IFX_GPIO_IOC_MAGIC, 14, struct ifx_gpio_ioctl_pin_reserve)
/*!
  \def IFX_GPIO_IOC_PIN_CONFIG
  \brief GPIO IOCTL Command - Configure pin.

   This command uses struct "ifx_gpio_ioctl_pin_config" as parameter to configure pin.
 */
#define IFX_GPIO_IOC_PIN_CONFIG         _IOR( IFX_GPIO_IOC_MAGIC, 15, struct ifx_gpio_ioctl_pin_config)
/*!
  \def IFX_GPIO_IOC_MODULE_QUERY
  \brief GPIO IOCTL Command - Get pin information of given module ID.

   This command uses struct "ifx_gpio_ioctl_module_query" as parameter to get pin information of given module ID.
 */
#define IFX_GPIO_IOC_MODULE_QUERY       _IOWR(IFX_GPIO_IOC_MAGIC, 16, struct ifx_gpio_ioctl_module_query)
/*!
  \def IFX_GPIO_IOC_PIN_QUERY
  \brief GPIO IOCTL Command - Get pin information of given pin ID.

   This command uses struct "ifx_gpio_ioctl_module_query" as parameter to get pin information of given pin ID.
 */
#define IFX_GPIO_IOC_PIN_QUERY          _IOWR(IFX_GPIO_IOC_MAGIC, 17, struct ifx_gpio_ioctl_pin_query)
/*@}*/



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
  /*
   *    Module Level Functions
   */
  int ifx_gpio_register(int module_id);
  int ifx_gpio_deregister(int module_id);
  /*
   *    Pin Operation Functions
   */
  int ifx_gpio_output_set(int pin, int module_id);
  int ifx_gpio_output_clear(int pin, int module_id);
  int ifx_gpio_input_get(int pin, int module_id, int *input);
  /*
   *    Pin Operation Functions (Backward Compatible)
   */
  int ifx_gpio_pin_reserve(int pin, int module_id);
  int ifx_gpio_pin_free(int pin, int module_id);
  int ifx_gpio_open_drain_set(int pin, int module_id);
  int ifx_gpio_open_drain_clear(int pin, int module_id);
  int ifx_gpio_pudsel_set(int pin, int module_id);
  int ifx_gpio_pudsel_clear(int pin, int module_id);
  int ifx_gpio_puden_set(int pin, int module_id);
  int ifx_gpio_puden_clear(int pin, int module_id);
  int ifx_gpio_stoff_set(int pin, int module_id);
  int ifx_gpio_stoff_clear(int pin, int module_id);
  int ifx_gpio_dir_out_set(int pin, int module_id);
  int ifx_gpio_dir_in_set(int pin, int module_id);
  int ifx_gpio_altsel0_set(int pin, int module_id);
  int ifx_gpio_altsel0_clear(int pin, int module_id);
  int ifx_gpio_altsel1_set(int pin, int module_id);
  int ifx_gpio_altsel1_clear(int pin, int module_id);
#endif  //  __KERNEL__



#endif  //  IFX_GPIO_H
