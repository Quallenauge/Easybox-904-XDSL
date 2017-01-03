/******************************************************************************
**
** FILE NAME    : ifxmips_sflash_ar9.h
** PROJECT      : IFX UEIP
** MODULES      : Serial Flash
**
** DATE         : 03 July 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : SPI Flash AR9 specific configuration
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
  \file ifxmips_sflash_ar9.h
  \ingroup IFX_SFLASH
  \brief ifx serial flash driver ARX specific file
*/
#ifndef IFXMIPS_SFLASH_AR9_H
#define IFXMIPS_SFLASH_AR9_H

#define IFX_SFLASH_MODE                IFX_SSC_MODE_0
#define IFX_SFLASH_PRIORITY            IFX_SSC_PRIO_LOW
#define IFX_SFLASH_FRAGSIZE            268    /* XXX, must be more than 260, page size + cmd + addr */
#define IFX_SFLASH_MAXFIFOSIZE         268    /* Always use FIFO */
#define IFX_SFLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT3_POS

#define IFX_SFLASH_BAUDRATE            20000000      /*  20 MHz */

#endif /* IFXMIPS_SFLASH_AR9_H */

