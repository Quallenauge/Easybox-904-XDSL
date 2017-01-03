/******************************************************************************
**
** FILE NAME    : ifxmips_ssc_pm.h
** PROJECT      : IFX UEIP
** MODULES      : SSC (Synchronous Serial Controller) Power Management
**
** DATE         : 16 Dec 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : SCC Driver Power Managment
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
** 16 Dec,2009   Lei Chuanhua    First UEIP release
*******************************************************************************/
/*!
 \file ifxmips_ssc_pm.h
 \ingroup IFX_SSC    
 \brief header file for SSC Power Management
*/

#ifndef IFXMIPS_SSC_PM_H
#define IFXMIPS_SSC_PM_H

#include "ifxmips_ssc.h"

void ifx_ssc_pmcu_init(struct ifx_ssc_port *port);
void ifx_ssc_pmcu_exit(struct ifx_ssc_port *port);

#endif /* IFXMIPS_SSC_PM_H  */
