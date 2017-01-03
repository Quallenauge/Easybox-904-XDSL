/******************************************************************************
**
** FILE NAME    : ifx_ledc.h
** PROJECT      : UEIP
** MODULES      : LED Controller (Serial Out)
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global LED Controller driver header file
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

#ifndef IFX_LEDC_H
#define IFX_LEDC_H



/*!
  \defgroup IFX_LEDC UEIP Project - LED Controller sub-driver module
  \brief UEIP Project - LED Controller (Serial Out) sub-driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_LEDC_API APIs
  \ingroup IFX_LEDC
  \brief APIs used by other drivers/modules.
 */

/*!
  \defgroup IFX_LEDC_IOCTL IOCTL Commands
  \ingroup IFX_LEDC
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_LEDC_STRUCT Structures
  \ingroup IFX_LEDC
  \brief Structures used by user application.
 */

/*!
  \file ifx_ledc.h
  \ingroup IFX_LEDC
  \brief LED Controller (Serial Out) sub-driver header file
 */



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*!
  \addtogroup IFX_LEDC_STRUCT
 */
/*@{*/

/*
 *  Definition of Operation MASK
 */
/*!
  \def IFX_LEDC_CFG_OP_UPDATE_SOURCE
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select LED data source.
 */
#define IFX_LEDC_CFG_OP_UPDATE_SOURCE       0x0001
/*!
  \def IFX_LEDC_CFG_OP_BLINK
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - enable/disable blink.
 */
#define IFX_LEDC_CFG_OP_BLINK               0x0002
/*!
  \def IFX_LEDC_CFG_OP_UPDATE_CLOCK
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select update clock source.
 */
#define IFX_LEDC_CFG_OP_UPDATE_CLOCK        0x0004
/*!
  \def IFX_LEDC_CFG_OP_STORE_MODE
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select clock store mode or single pulse store mode.
 */
#define IFX_LEDC_CFG_OP_STORE_MODE          0x0008
/*!
  \def IFX_LEDC_CFG_OP_SHIFT_CLOCK
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select shift clock source.
 */
#define IFX_LEDC_CFG_OP_SHIFT_CLOCK         0x0010
/*!
  \def IFX_LEDC_CFG_OP_DATA_OFFSET
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select cycles (0 ~ 3) to be inserted before data transmition.
 */
#define IFX_LEDC_CFG_OP_DATA_OFFSET         0x0020
/*!
  \def IFX_LEDC_CFG_OP_NUMBER_OF_LED
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select number of LEDs (8, 16, 24).
 */
#define IFX_LEDC_CFG_OP_NUMBER_OF_LED       0x0040
/*!
  \def IFX_LEDC_CFG_OP_DATA
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - LED on/off.
 */
#define IFX_LEDC_CFG_OP_DATA                0x0080
/*!
  \def IFX_LEDC_CFG_OP_MIPS0_ACCESS
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select owner of LED (mips0/mips1).
 */
#define IFX_LEDC_CFG_OP_MIPS0_ACCESS        0x0100
/*!
  \def IFX_LEDC_CFG_OP_DATA_CLOCK_EDGE
  \brief field "operation_mask" of struct "ifx_ledc_config_param" - select data clock effective edge (rising/falling).
 */
#define IFX_LEDC_CFG_OP_DATA_CLOCK_EDGE     0x0200

/*
 *  Constant for Some Operations
 */
/*!
  \def IFX_LED_CON1_UPDATE_SRC_SOFTWARE
  \brief field "update_clock" of struct "ifx_ledc_config_param" - update LED by software trigger.
 */
#define IFX_LED_CON1_UPDATE_SRC_SOFTWARE    0
#define LED_CON1_UPDATE_SRC_SOFTWARE        IFX_LED_CON1_UPDATE_SRC_SOFTWARE
/*!
  \def IFX_LED_CON1_UPDATE_SRC_GPT
  \brief field "update_clock" of struct "ifx_ledc_config_param" - update LED by GPT timer.
 */
#define IFX_LED_CON1_UPDATE_SRC_GPT         1
#define LED_CON1_UPDATE_SRC_GPT             IFX_LED_CON1_UPDATE_SRC_GPT
/*!
  \def IFX_LED_CON1_UPDATE_SRC_FPI
  \brief field "update_clock" of struct "ifx_ledc_config_param" - update LED by FPI clock.
 */
#define IFX_LED_CON1_UPDATE_SRC_FPI         2
#define LED_CON1_UPDATE_SRC_FPI             IFX_LED_CON1_UPDATE_SRC_FPI


#define IFX_LED_CON1_GROUP2                 (1 << 2)
#define IFX_LED_CON1_GROUP1                 (1 << 1)
#define IFX_LED_CON1_GROUP0                 (1 << 0)
#define LED_CON1_GROUP2                     IFX_LED_CON1_GROUP2
#define LED_CON1_GROUP1                     IFX_LED_CON1_GROUP1
#define LED_CON1_GROUP0                     IFX_LED_CON1_GROUP0

/*!
  \def IFX_LED_EXT_SRC_DSL_LED0
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of DSL LED0
 */
#define IFX_LED_EXT_SRC_DSL_LED0            0
/*!
  \def IFX_LED_EXT_SRC_DSL_LED1
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of DSL LED1
 */
#define IFX_LED_EXT_SRC_DSL_LED1            1
/*!
  \def IFX_LED_EXT_SRC_GPHY1_LED0
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY1 LED0
 */
#define IFX_LED_EXT_SRC_GPHY1_LED0          2
/*!
  \def IFX_LED_EXT_SRC_GPHY1_LED1
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY1 LED1
 */
#define IFX_LED_EXT_SRC_GPHY1_LED1          3
/*!
  \def IFX_LED_EXT_SRC_GPHY1_LED2
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY1 LED2
 */
#define IFX_LED_EXT_SRC_GPHY1_LED2          4
/*!
  \def IFX_LED_EXT_SRC_GPHY0_LED0
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY0 LED0
 */
#define IFX_LED_EXT_SRC_GPHY0_LED0          5
/*!
  \def IFX_LED_EXT_SRC_GPHY0_LED1
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY0 LED1
 */
#define IFX_LED_EXT_SRC_GPHY0_LED1          6
/*!
  \def IFX_LED_EXT_SRC_GPHY0_LED2
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY0 LED2
 */
#define IFX_LED_EXT_SRC_GPHY0_LED2          7
/*!
  \def IFX_LED_EXT_SRC_EPHY_LINK_LED
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of Ephy Link LED
 */
#define IFX_LED_EXT_SRC_EPHY_LINK_LED       IFX_LED_EXT_SRC_GPHY1_LED0
/*!
  \def IFX_LED_EXT_SRC_EPHY_SPEED_LED
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of Ephy Speed LED
 */
#define IFX_LED_EXT_SRC_EPHY_SPEED_LED      IFX_LED_EXT_SRC_GPHY1_LED1
/*!
  \def IFX_LED_EXT_SRC_EPHY_COLLISION_LED
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of Ephy Collision LED
 */
#define IFX_LED_EXT_SRC_EPHY_COLLISION_LED  IFX_LED_EXT_SRC_GPHY1_LED2
/*!
  \def IFX_LED_EXT_SRC_EPHY_DUPLEX_LED
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of Ephy Duplex LED
 */
#define IFX_LED_EXT_SRC_EPHY_DUPLEX_LED     IFX_LED_EXT_SRC_GPHY0_LED0
/*!
  \def IFX_LED_EXT_SRC_GPHY2_LED0
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY2 LED0
 */
#define IFX_LED_EXT_SRC_GPHY2_LED0          8
/*!
  \def IFX_LED_EXT_SRC_GPHY2_LED1
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY2 LED1
 */
#define IFX_LED_EXT_SRC_GPHY2_LED1          9
/*!
  \def IFX_LED_EXT_SRC_GPHY2_LED2
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of GPHY2 LED2
 */
#define IFX_LED_EXT_SRC_GPHY2_LED2          10
/*!
  \def IFX_LED_EXT_SRC_WLAN_SB_LQ
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of WLAN Link Quality Single Band LED
 */
#define IFX_LED_EXT_SRC_WLAN_SB_LQ          11
/*!
  \def IFX_LED_EXT_SRC_WLAN_DB_LQ
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of WLAN Link Quality Dual Band LED
 */
#define IFX_LED_EXT_SRC_WLAN_DB_LQ          12
/*!
  \def IFX_LED_EXT_SRC_WLAN_SB_LA
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of WLAN Link/Activity Single Band LED
 */
#define IFX_LED_EXT_SRC_WLAN_SB_LA          13
/*!
  \def IFX_LED_EXT_SRC_WLAN_DB_LA
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of WLAN Link/Activity Dual Band LED
 */
#define IFX_LED_EXT_SRC_WLAN_DB_LA          14
/*!
  \def IFX_LED_EXT_SRC_WLAN_SECU0
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of WLAN Security LED0 (Dual Color)
 */
#define IFX_LED_EXT_SRC_WLAN_SECU0          15
/*!
  \def IFX_LED_EXT_SRC_WLAN_SECU1
  \brief field "source" and "source_mask" of struct "ifx_ledc_config_param" - index of WLAN Security LED1 (Dual Color)
 */
#define IFX_LED_EXT_SRC_WLAN_SECU1          16

/*@}*/



/*
 * ####################################
 *                IOCTL
 * ####################################
 */

/*!
  \addtogroup IFX_LEDC_STRUCT
 */
/*@{*/

/*!
  \struct ifx_ledc_ioctl_version
  \brief Structure used for query of driver version.
 */
struct ifx_ledc_ioctl_version {
    unsigned int    major;  /*!< output, major number of driver */
    unsigned int    mid;    /*!< output, mid number of driver */
    unsigned int    minor;  /*!< output, minor number of driver */
};

/*!
  \struct ifx_ledc_config_param
  \brief Structure used for configure LED Controller (Serial Out).
 */
struct ifx_ledc_config_param {
    unsigned long   operation_mask;         /*!< input, Select operations to be performed */
    unsigned long   source_mask;            /*!< input, LED to change update source (LEDC or Ext - ADSL, PHY) */
    unsigned long   source;                 /*!< input, Corresponding update source (LEDC or Ext - ADSL, PHY), 0 - LEDC, 1 - EXT */
    unsigned long   blink_mask;             /*!< input, LEDs to set blink mode */
    unsigned long   blink;                  /*!< input, Set to blink mode or normal mode, 0 - blinking, 1 - non-blinking */
    unsigned long   update_clock;           /*!< input, Select the source of update clock, 0 - Software, 1 - GPT, 2 - FPI */
    unsigned long   fpid;                   /*!< input, If FPI is the source of update clock, set the divider */
                                            /*!< input, else if GPT is the source, set the frequency (unit 1/10Hz)  */
    unsigned long   store_mode;             /*!< input, Set clock mode or single pulse mode for store signal, 0 - single store, 1 - shift clock store */
    unsigned long   fpis;                   /*!< input, FPI is the source of shift clock, set the divider */
    unsigned long   data_offset;            /*!< input, Set cycles to be inserted before data is transmitted (0 - 3) */
    unsigned long   number_of_enabled_led;  /*!< input, Total number of LED to be enabled (0, 8, 16, 24) */
    unsigned long   data_mask;              /*!< input, LEDs to set value */
    unsigned long   data;                   /*!< input, Corresponding value, 0 - low output (off), 1 - high output (on) */
    unsigned long   mips0_access_mask;      /*!< input, LEDs to set access right (not valid for Amazon-SE) */
    unsigned long   mips0_access;           /*!< input, 1: the corresponding data is output from MIPS0, 0: MIPS1 */
    unsigned long   f_data_clock_on_rising; /*!< input, 1: data clock on rising edge, 0: data clock on falling edge */
};

/*@}*/

/*!
  \addtogroup IFX_LEDC_IOCTL
 */
/*@{*/
#define IFX_LEDC_IOC_MAGIC              'k'
/*!
  \def IFX_LEDC_IOC_VERSION
  \brief LED Controller IOCTL Command - Get driver version number.

   This command uses struct "ifx_ledc_ioctl_version" as parameter to LED Controller driver version number.
 */
#define IFX_LEDC_IOC_VERSION            _IOR(IFX_LEDC_IOC_MAGIC,  0, struct ifx_ledc_ioctl_version)
/*!
  \def IFX_LEDC_IOC_SET_CONFIG
  \brief LED Controller IOCTL Command - Config LED Controller (Serial Out).

   This command uses struct "ifx_ledc_config_param" as parameter to configure LED Controller (Serial Out).
 */
#define IFX_LEDC_IOC_SET_CONFIG         _IOW(IFX_LEDC_IOC_MAGIC,  1, struct ifx_ledc_config_param)
/*@}*/



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
  int ifx_ledc_set_blink(unsigned int led, unsigned int blink);
  int ifx_ledc_set_data(unsigned int led, unsigned int data);
  int ifx_ledc_config(struct ifx_ledc_config_param *param);
#endif



#endif  //  IFX_LEDC_H
