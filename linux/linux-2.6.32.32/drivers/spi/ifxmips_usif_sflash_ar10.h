/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_usif_sflash_ar10.h
  \ingroup IFX_USIF_SFLASH
  \brief ifx usif serial flash driver AR10 specific file
*/

#ifndef IFXMIPS_USIF_SFLASH_AR10_H
#define IFXMIPS_USIF_SFLASH_AR10_H

#define IFX_USIF_SFLASH_MODE                IFX_USIF_SPI_MODE_0
#define IFX_USIF_SFLASH_PRIORITY            IFX_USIF_SPI_PRIO_LOW
#define IFX_USIF_SFLASH_FRAGSIZE            268    /* XXX, must be more than 260, page size + cmd + addr */
#define IFX_USIF_SFLASH_MAXFIFOSIZE         32

#ifdef CONFIG_USE_EMULATOR
#define IFX_USIF_SFLASH_BAUDRATE            10000    /*  10K Hz */
/* MXIC */
#define IFX_USIF_SFLASH_CS                  IFX_USIF_SPI_CS0 
#else
#define IFX_USIF_SFLASH_BAUDRATE            20000000  /*  20 MHz */
#define IFX_USIF_SFLASH_CS                  IFX_USIF_SPI_CS0
#endif /* CONFIG_USE_EMULATOR */

#endif /* IFXMIPS_USIF_SFLASH_AR10_H */

