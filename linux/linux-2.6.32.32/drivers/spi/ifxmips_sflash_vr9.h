/******************************************************************************
**
** FILE NAME    : ifxmips_sflash_vr9.h
** PROJECT      : IFX UEIP
** MODULES      : Serial Flash
**
** DATE         : 03 July 2009
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
  \file ifxmips_sflash_vr9.h
  \ingroup IFX_SFLASH
  \brief ifx serial flash driver VRX specific file
*/

#ifndef IFXMIPS_SFLASH_VR9_H
#define IFXMIPS_SFLASH_VR9_H

#define IFX_SFLASH_MODE                IFX_SSC_MODE_0
#define IFX_SFLASH_PRIORITY            IFX_SSC_PRIO_LOW
#define IFX_SFLASH_FRAGSIZE            268    /* XXX, must be more than 260, page size + cmd + addr */
#define IFX_SFLASH_MAXFIFOSIZE         268    /* Always FIFO mode */

#ifdef CONFIG_USE_EMULATOR
#define IFX_SFLASH_BAUDRATE            10000    /*  10K Hz */
/* CS0 MXIC */
//#define IFX_SFLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT0_POS
/* CS1 ATMEL */
#define IFX_SFLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT1_POS
#else
#define IFX_SFLASH_BAUDRATE            20000000 /*  20 MHz */
#define IFX_SFLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT3_POS
#endif /* CONFIG_USE_EMULATOR */

#endif /* IFXMIPS_SFLASH_VR9_H */

