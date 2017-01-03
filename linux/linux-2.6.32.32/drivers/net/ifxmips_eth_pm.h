/**
** FILE NAME    : ifxmips_eth_pm.h
** PROJECT      : IFX UEIP
** MODULES      : ETH module Power Management
** DATE         : 22 Dec 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : ETH PM driver header file
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
** $Date                        $Author                         $Comment
** 22 Dec 2009                  Reddy Mallikarjuna              Initial release
*******************************************************************************/
#ifndef _IFXMIPS_ETH_PM_H_
#define _IFXMIPS_ETH_PM_H_

/*!
 \file ifxmips_eth_pm.h
 \ingroup IFX_ETH_DRV    
 \brief Header file for ETH Power Management
*/

void ifx_eth_pmcu_init(void);
void ifx_eth_pmcu_exit(void);

#endif /* _IFXMIPS_ETH_PM_H_  */
