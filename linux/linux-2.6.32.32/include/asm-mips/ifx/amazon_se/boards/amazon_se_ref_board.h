/******************************************************************************
**
** FILE NAME    : amazon_se_ref_board.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : header file for Amazon-SE
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



#ifndef AMAZON_SE_REF_BOARD_H
#define AMAZON_SE_REF_BOARD_H
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */


#if defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE)
#define IFX_MTD_SPI_PART_NB               3
#define IFX_SPI_FLASH_MAX                 6
#endif /* defined(CONFIG_IFX_SPI_FLASH) || defined (CONFIG_IFX_SPI_FLASH_MODULE) */

#if defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)
	#if 1 // Control VBus through LED abstract layer
		#define IFX_LEDGPIO_USB_VBUS            IFX_GPIO_PIN_ID(1, 7)
	#else // Control VBus through GPIO driver directly
		#define IFX_GPIO_USB_VBUS               IFX_GPIO_PIN_ID(1, 7)
	#endif
#endif

#if   (defined(CONFIG_USB_HOST_IFX) || defined(CONFIG_USB_HOST_IFX_MODULE)) && defined(CONFIG_USB_HOST_IFX_LED)
//	#define IFX_LEDGPIO_USB_LED            IFX_GPIO_PIN_ID(1, 11)
#elif (defined(CONFIG_USB_GADGET_IFX) || defined(CONFIG_USB_GADGET_IFX_MODULE)) && defined(CONFIG_USB_GADGET_IFX_LED)
//	#define IFX_LEDGPIO_USB_LED            IFX_GPIO_PIN_ID(1, 11)
#endif



#endif  /* AMAZON_SE_REF_BOARD_H */

