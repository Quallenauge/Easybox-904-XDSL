/******************************************************************************
**
** FILE NAME    : ifx_ebu_led.h
** PROJECT      : UEIP
** MODULES      : EBU to Control LEDs
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global EBU LED Controller driver header file
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

#ifndef IFX_EBU_LED_H
#define IFX_EBU_LED_H



/*!
  \defgroup IFX_LEDEBU UEIP Project - LED EBU sub-driver module
  \brief UEIP Project - LED EBU sub-driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_LEDEBU_API APIs
  \ingroup IFX_LEDEBU
  \brief APIs used by other drivers/modules.
 */

/*!
  \file ifx_ebu_led.h
  \ingroup IFX_LEDEBU
  \brief LED EBU sub-driver header file
 */



/*
 * ####################################
 *                IOCTL
 * ####################################
 */



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
 #ifdef CONFIG_IFX_EBU_LED
  int ifx_ebu_led_set_data(unsigned int led, unsigned int data);
  void ifx_ebu_led_enable(void);
  void ifx_ebu_led_disable(void);
 #else
  #define ifx_ebu_led_set_data(led, data)
  #define ifx_ebu_led_enable()
  #define ifx_ebu_led_disable()
 #endif
#endif



#endif  //  IFX_EBU_LED_H
