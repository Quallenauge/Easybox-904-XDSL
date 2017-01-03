/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_usif_spi_ar10.h
  \ingroup IFX_USIF_SPI
  \brief USIF SPI driver AR10 specific file
*/
#ifndef IFXMIPS_USIF_SPI_AR10_H
#define IFXMIPS_USIF_SPI_AR10_H

#include <asm/ifx/ifx_pmu.h>

static inline void ifx_usif_spi_pm_enable(void)
{
    /* Activate USIF SPI */
    USIF_PMU_SETUP(IFX_PMU_ENABLE);
}


static inline void ifx_usif_spi_pm_disable(void) 
{
    USIF_PMU_SETUP(IFX_PMU_DISABLE);
}
#endif /* IFXMIPS_USIF_SPI_AR10_H */
