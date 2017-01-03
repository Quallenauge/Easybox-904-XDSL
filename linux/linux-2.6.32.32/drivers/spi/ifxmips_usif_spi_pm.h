/******************************************************************************
**
** FILE NAME    : ifxmips_usif_spi_pm.h
** PROJECT      : IFX UEIP
** MODULES      : USIF SPI Mode Power Management
**
** DATE         : 18 Dec 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : USIF SPI Mode Driver Power Managment
** COPYRIGHT    :       Copyright (c) 2009
**                      Lantiq Deutschland GmbH
**                      Am Campeon 3, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 18 Dec,2009   Lei Chuanhua    First UEIP release
*******************************************************************************/
/*!
 \file ifxmips_usif_spi_pm.h
 \ingroup IFX_USIF_SPI    
 \brief header file for USIF SPI Power Management
*/

#ifndef IFXMIPS_USIF_SPI_PM_H
#define IFXMIPS_USIF_SPI_PM_H

#include "ifxmips_usif_spi.h"

void ifx_usif_spi_pmcu_init(struct ifx_usif_port *port);
void ifx_usif_spi_pmcu_exit(struct ifx_usif_port *port);

#endif /* IFXMIPS_USIF_SPI_PM_H  */


