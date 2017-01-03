/******************************************************************************
**
** FILE NAME    : ar10_ref_board.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : header file for AR10 
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



#ifndef AR10_REF_BOARD_H
#define AR10_REF_BOARD_H
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */

#if defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) \
    || defined(CONFIG_IFX_USIF_SPI_FLASH) || defined (CONFIG_IFX_USIF_SPI_FLASH_MODULE)
#define IFX_MTD_SPI_PART_NB               3
#define IFX_SPI_FLASH_MAX                 8
#endif /* defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) */

#if defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)
 #if 1
  #define IFX_GPIO_USB_VBUS               IFX_GPIO_PIN_ID(2, 1)
 #else
  #define IFX_GPIO_USB_VBUS1              IFX_GPIO_PIN_ID(1, 13)
  #define IFX_GPIO_USB_VBUS2              IFX_GPIO_PIN_ID(3, 0)
 #endif
#endif


#endif  /* AR10_REF_BOARD_H */

