/******************************************************************************
**
** FILE NAME    : ifxmips_usif_sflash_vr9.h
** PROJECT      : IFX UEIP
** MODULES      : Serial Flash
**
** DATE         : 16 Oct 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : SPI Flash vr9 specific configuration
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
**  03,July,2009 Lei Chuanhua   Initial UEIP release
*******************************************************************************/
/*!
  \file ifxmips_usif_sflash_vr9.h
  \ingroup IFX_USIF_SFLASH
  \brief ifx usif serial flash driver VRX specific file
*/

#ifndef IFXMIPS_USIF_SFLASH_VR9_H
#define IFXMIPS_USIF_SFLASH_VR9_H

#define IFX_USIF_SFLASH_MODE                IFX_USIF_SPI_MODE_0
#define IFX_USIF_SFLASH_PRIORITY            IFX_USIF_SPI_PRIO_LOW
#define IFX_USIF_SFLASH_FRAGSIZE            268    /* XXX, must be more than 260, page size + cmd + addr */
#define IFX_USIF_SFLASH_MAXFIFOSIZE         32

#ifdef CONFIG_USE_EMULATOR
#define IFX_USIF_SFLASH_BAUDRATE            10000    /*  10K Hz */
/* MXIC */
#define IFX_USIF_SFLASH_CS                  IFX_USIF_SPI_CS0 
 /* ATMEL */
//#define IFX_USIF_SFLASH_CS                  IFX_USIF_SPI_CS1 
#else
#define IFX_USIF_SFLASH_BAUDRATE            20000000  /*  20 MHz */
//#define IFX_USIF_SFLASH_CS                  IFX_USIF_SPI_CS1
#define IFX_USIF_SFLASH_CS                  IFX_USIF_SPI_CS2
#endif /* CONFIG_USE_EMULATOR */

#endif /* IFXMIPS_USIF_SFLASH_VR9_H */

