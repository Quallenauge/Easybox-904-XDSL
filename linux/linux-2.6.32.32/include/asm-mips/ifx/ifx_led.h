/******************************************************************************
**
** FILE NAME    : ifx_led.h
** PROJECT      : UEIP
** MODULES      : LED Driver
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global LED driver header file
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
** 16 JUL 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFX_LED_H
#define IFX_LED_H



/*!
  \defgroup IFX_LED UEIP Project - LED driver module
  \brief UEIP Project - LED driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_LED_API APIs
  \ingroup IFX_LED
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_LED_IOCTL IOCTL Commands
  \ingroup IFX_LED
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_LED_STRUCT Structures
  \ingroup IFX_LED
  \brief Structures used by user application.
 */

/*!
  \file ifx_led.h
  \ingroup IFX_LED
  \brief LED driver header file
 */



#include <linux/leds.h>
#include "asm/ifx/ifx_types.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  LED Events
 */
#define IFX_LED_TRIGGER_POWER_ON        "power_on"
#define IFX_LED_TRIGGER_WARNING         "warning"
#define IFX_LED_TRIGGER_DSL_LINK        "dsl_link"
#define IFX_LED_TRIGGER_DSL_DATA        "dsl_data"
#define IFX_LED_TRIGGER_USB_LINK        "usb_link"
#define IFX_LED_TRIGGER_WAN_STATUS      "wan_status"
#define IFX_LED_TRIGGER_EPHY_LINK       "ephy_link"
#define IFX_LED_TRIGGER_EPHY_SPEED      "ephy_speed"
#define IFX_LED_TRIGGER_FXS1_LINK       "fxs1_link"
#define IFX_LED_TRIGGER_FXS2_LINK       "fxs2_link"
#define IFX_LED_TRIGGER_FXO_ACT         "fxo_act"
#define IFX_LED_TRIGGER_WLAN_READY1     "wlan_ready1"
#define IFX_LED_TRIGGER_WLAN_READY2     "wlan_ready2"
#define IFX_LED_TRIGGER_SD_LINK         "sd_link"



/*
 * ####################################
 *              Data Type
 * ####################################
 */

/*!
  \addtogroup IFX_LED_STRUCT
 */
/*@{*/

/*
 *  LED Device Definition
 */
/*!
  \struct ifx_led_device
  \brief Structure used for LED device.
 */
struct ifx_led_device {
    struct led_classdev     device;             /*!< LED device */
    char                    name[32];           /*!< LED device name */

    char                   *default_trigger;    /*!< default event/trigger of LED device */
    unsigned int            phys_id;            /*!< physical ID of LED device, driver use this ID to communicate with physical LED driver */
    unsigned int            value_on;           /*!< value to turn on LED */
    unsigned int            value_off;          /*!< value to turn off LED */
/*!
  \def IFX_LED_DEVICE_FLAG_PHYS_GPIO
  \brief field "flags" of struct "ifx_led_device" - LED drived by GPIO.
 */
#define IFX_LED_DEVICE_FLAG_PHYS_GPIO   0
/*!
  \def IFX_LED_DEVICE_FLAG_PHYS_LEDC
  \brief field "flags" of struct "ifx_led_device" - LED drived by LED Controller (Serial Out).
 */
#define IFX_LED_DEVICE_FLAG_PHYS_LEDC   1
/*!
  \def IFX_LED_DEVICE_FLAG_PHYS_EBU
  \brief field "flags" of struct "ifx_led_device" - LED drived by EBU.
 */
#define IFX_LED_DEVICE_FLAG_PHYS_EBU    2
#define IFX_LED_DEVICE_FLAG_PHYS_MASK   3
#define IFX_LED_DEVICE_FLAG_CREATED     (1 << 30)
/*!
  \def IFX_LED_DEVICE_FLAG_INVALID
  \brief field "flags" of struct "ifx_led_device" - LED is not in use.
 */
#define IFX_LED_DEVICE_FLAG_INVALID     (1 << 31)
    unsigned int            flags;              /*!< additional flags */
};

/*
 *  LED Trigger Atrribute
 */
/*!
  \struct ifx_led_trigger_attrib
  \brief Structure used for LED default behavior (attibutes).
 */
struct ifx_led_trigger_attrib {
    unsigned int            delay_on;           /*!< delay of LED on (in millisecond) */
    unsigned int            delay_off;          /*!< delay of LED off (in millisecond) */
    unsigned int            timeout;            /*!< stop blinking/on after timeout (in millisecond) */
    unsigned int            def_value;          /*!< default state of LED (on/off) */
/*!
  \def IFX_LED_TRIGGER_ATTRIB_DELAY_ON
  \brief field "flags" of struct "ifx_led_trigger_attrib" - delay_on is valid.
 */
#define IFX_LED_TRIGGER_ATTRIB_DELAY_ON     (1 << 0)
/*!
  \def IFX_LED_TRIGGER_ATTRIB_DELAY_OFF
  \brief field "flags" of struct "ifx_led_trigger_attrib" - delay_off is valid.
 */
#define IFX_LED_TRIGGER_ATTRIB_DELAY_OFF    (1 << 1)
/*!
  \def IFX_LED_TRIGGER_ATTRIB_TIMEOUT
  \brief field "flags" of struct "ifx_led_trigger_attrib" - timeout is valid.
 */
#define IFX_LED_TRIGGER_ATTRIB_TIMEOUT      (1 << 2)
/*!
  \def IFX_LED_TRIGGER_ATTRIB_DEF_VALUE
  \brief field "flags" of struct "ifx_led_trigger_attrib" - def_value is valid.
 */
#define IFX_LED_TRIGGER_ATTRIB_DEF_VALUE    (1 << 3)
    unsigned int            flags;              /*!< additional flags */
};

/*@}*/



/*
 * ####################################
 *                IOCTL
 * ####################################
 */

/*!
  \addtogroup IFX_LED_STRUCT
 */
/*@{*/

/*!
  \struct ifx_led_ioctl_version
  \brief Structure used for query of driver version.
 */
struct ifx_led_ioctl_version {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver */
    unsigned int    minor;  /*!< output, minor number of driver */
};

/*!
  \struct ifx_led_ioctl_trigger
  \brief Structure used for trigger operations.
 */
struct ifx_led_ioctl_trigger {
    char            name[32];               /*!< input, name of trigger */
    void           *handler;                /*!< input/output, handler of trigger */
    struct ifx_led_trigger_attrib   attrib; /*!< input, behavior of LED */
};

/*@}*/


/*!
  \addtogroup IFX_LED_IOCTL
 */
/*@{*/
#define IFX_LED_IOC_MAGIC               0xed
/*!
  \def IFX_LED_IOC_VERSION
  \brief LED IOCTL Command - Get version number of driver.

   This command uses structure "ifx_led_ioctl_version" as parameter to get version number of driver.
 */
#define IFX_LED_IOC_VERSION             _IOR( IFX_LED_IOC_MAGIC, 0, struct ifx_led_ioctl_version)
/*!
  \def IFX_LED_IOC_TRIGGER_REGISTER
  \brief LED IOCTL Command - Register a LED trigger (source of event).

   This command uses structure "ifx_led_ioctl_trigger" as parameter to register a LED trigger (source of event).
   User need provide valid "name" before call this IOCTL.
   The return value is hold in "handler", and it should be used for other IOCTL commands.
 */
#define IFX_LED_IOC_TRIGGER_REGISTER    _IOWR(IFX_LED_IOC_MAGIC, 1, struct ifx_led_ioctl_trigger)
/*!
  \def IFX_LED_IOC_TRIGGER_DEREGISTER
  \brief LED IOCTL Command - Deregister a LED trigger (source of event).

   This command uses structure "ifx_led_ioctl_trigger" as parameter to deregister a LED trigger (source of event).
   User need provide valid "handler" which is got from "IFX_LED_IOC_TRIGGER_REGISTER".
   After this command, the "handler" is destroyed and can not be used anymore.
 */
#define IFX_LED_IOC_TRIGGER_DEREGISTER  _IOW( IFX_LED_IOC_MAGIC, 2, struct ifx_led_ioctl_trigger)
/*!
  \def IFX_LED_IOC_TRIGGER_ACTIVATE
  \brief LED IOCTL Command - send LED trigger (event).

   This command uses structure "ifx_led_ioctl_trigger" as parameter to send a LED trigger (event).
   User uses this command to give a notice on LED event, and LED driver will drive LED according to attributes.
   User need provide valid "handler" which is got from "IFX_LED_IOC_TRIGGER_REGISTER".
 */
#define IFX_LED_IOC_TRIGGER_ACTIVATE    _IOW( IFX_LED_IOC_MAGIC, 3, struct ifx_led_ioctl_trigger)
/*!
  \def IFX_LED_IOC_TRIGGER_DEACTIVATE
  \brief LED IOCTL Command - reset LED to default status.

   This command uses structure "ifx_led_ioctl_trigger" as parameter to reset LED to default status.
   User uses this command to reset LED to default status (e.g. LED off).
   User need provide valid "handler" which is got from "IFX_LED_IOC_TRIGGER_REGISTER".
 */
#define IFX_LED_IOC_TRIGGER_DEACTIVATE  _IOW( IFX_LED_IOC_MAGIC, 4, struct ifx_led_ioctl_trigger)
/*!
  \def IFX_LED_IOC_TRIGGER_SET_ATTRIB
  \brief LED IOCTL Command - define LED behavior.

   This command uses structure "ifx_led_ioctl_trigger" as parameter to define LED behavior.
   User need provide valid "handler" which is got from "IFX_LED_IOC_TRIGGER_REGISTER".
 */
#define IFX_LED_IOC_TRIGGER_SET_ATTRIB  _IOW( IFX_LED_IOC_MAGIC, 5, struct ifx_led_ioctl_trigger)
/*@}*/



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
  int ifx_led_trigger_register(const char *trigger, void **pphandler);
  void ifx_led_trigger_deregister(void *phandler);
  void ifx_led_trigger_activate(void *phandler);
  void ifx_led_trigger_deactivate(void *phandler);
  void ifx_led_trigger_set_attrib(void *phandler, struct ifx_led_trigger_attrib *attrib);

  //    Use LED for GPIO Output operation.
  //      Register LED trigger.
  static inline int ifx_led_gpio_output_register(const char *trigger, void **pphandler)
  {
    int ret;
    struct ifx_led_trigger_attrib attrib = {0};

    attrib.flags = IFX_LED_TRIGGER_ATTRIB_DELAY_ON | IFX_LED_TRIGGER_ATTRIB_DELAY_OFF | IFX_LED_TRIGGER_ATTRIB_TIMEOUT | IFX_LED_TRIGGER_ATTRIB_DEF_VALUE;

    ret = ifx_led_trigger_register(trigger, pphandler);
    if ( ret == IFX_SUCCESS && *pphandler != NULL )
        ifx_led_trigger_set_attrib(*pphandler, &attrib);

    return ret;
  }

  //    Use LED for GPIO Output operation.
  //      Deregister LED trigger.
  static inline void ifx_led_gpio_output_deregister(void *phandler)
  {
    ifx_led_trigger_deregister(phandler);
  }

  //    Use LED for GPIO Output operation.
  //      Output value.
  static inline void ifx_led_gpio_output_value(void *phandler, unsigned int value)
  {
    if ( value != 0 )
        ifx_led_trigger_activate(phandler);
    else
        ifx_led_trigger_deactivate(phandler);
  }
#endif



#endif  //  IFX_LED_H
