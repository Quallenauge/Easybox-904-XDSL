/******************************************************************************
**
** FILE NAME    : ifxmips_usif_uart_pm.h
** PROJECT      : IFX UEIP
** MODULES      : USIF UART Mode Power Management
**
** DATE         : 18 Dec 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : USIF UART Mode Driver Power Managment
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
 \file ifxmips_usif_uart_pm.h
 \ingroup IFX_USIF_UART    
 \brief header file for USIF UART Power Management
*/

#ifndef IFXMIPS_USIF_UART_PM_H
#define IFXMIPS_USIF_UART_PM_H

void ifx_usif_uart_pmcu_init(IFX_USIF_UART_PORT_t *port);
void ifx_usif_uart_pmcu_exit(IFX_USIF_UART_PORT_t *port);

#endif /* IFXMIPS_USIF_UART_PM_H  */


