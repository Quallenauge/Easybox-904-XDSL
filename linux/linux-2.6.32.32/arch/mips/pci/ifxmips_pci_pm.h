/******************************************************************************
**
** FILE NAME    : ifxmips_pci_pm.h
** PROJECT      : IFX UEIP
** MODULES      : PCI Host Controller Driver
**
** DATE         : 21 Dec 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : PCI Host Controller Driver Power Managment
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
** 21 Dec,2009   Lei Chuanhua    First UEIP release
*******************************************************************************/
/*!
 \file ifxmips_pci_pm.h
 \ingroup IFX_PCI 
 \brief header file for PCI Host controller Power Management
*/

#ifndef IFXMIPS_PCI_PM_H
#define IFXMIPS_PCI_PM_H

void ifx_pci_pmcu_init(void);
void ifx_pci_pmcu_exit(void);

#endif /* IFXMIPS_PCI_PM_H  */

