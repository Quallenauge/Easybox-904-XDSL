/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_sflash_ar10.h
  \ingroup IFX_SFLASH
  \brief ifx serial flash driver AR10 specific file
*/

#ifndef IFXMIPS_SFLASH_AR10_H
#define IFXMIPS_SFLASH_AR10_H

#define IFX_SFLASH_MODE                IFX_SSC_MODE_0
#define IFX_SFLASH_PRIORITY            IFX_SSC_PRIO_LOW
#define IFX_SFLASH_FRAGSIZE            268    /* XXX, must be more than 260, page size + cmd + addr */
#define IFX_SFLASH_MAXFIFOSIZE         32

#ifdef CONFIG_USE_EMULATOR
#define IFX_SFLASH_BAUDRATE            10000    /*  10K Hz */
/* CS0 MXIC */
#define IFX_SFLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT0_POS
#else
#define IFX_SFLASH_BAUDRATE            20000000 /*  20 MHz */
#define IFX_SFLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT0_POS
#endif /* CONFIG_USE_EMULATOR */

#endif /* IFXMIPS_SFLASH_AR10_H */

