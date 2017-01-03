/******************************************************************************
**
** FILE NAME    : ifxmips_pci_common.c
** PROJECT      : IFX UEIP
** MODULES      : PCI subsystem
**
** DATE         : 30 June 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : PCIe Root Complex Driver
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
** HISTORY
** $Version $Date        $Author         $Comment
** 0.0.1    30 Jun,2009  Lei Chuanhua    Initial version
*******************************************************************************/
/*!
  \file ifxmips_pci_common.c
  \ingroup IFX_PCI_COM
  \brief PCI/PCIe bus driver common OS interface file
*/
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/pci.h> 
#include <linux/interrupt.h> 
#include <linux/time.h> 
#include <linux/delay.h> 

#include "ifxmips_pci_common.h"

/** 
 * \fn int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
 * \brief Map a PCI device to the appropriate interrupt line 
 * 
 * \param dev    The Linux PCI device structure for the device to map 
 * \param slot   The slot number for this device on __BUS 0__. Linux 
 *               enumerates through all the bridges and figures out the 
 *               slot on Bus 0 where this device eventually hooks to. 
 * \param pin    The PCI interrupt pin read from the device, then swizzled 
 *               as it goes through each bridge. 
 * \return Interrupt number for the device 
 * \ingroup IFX_PCI_COM_OS
 */ 
int __init
pcibios_map_irq(IFX_PCI_CONST struct pci_dev *dev, u8 slot, u8 pin)
{
    int pos;
    struct pci_dev *pdev = (struct pci_dev *)dev;
    
    /* Check PCIe capability */    
    pos = pci_find_capability(pdev, PCI_CAP_ID_EXP);
    if (pos <= 0) { /* Normal PCI */
    #ifdef CONFIG_IFX_PCI
        return ifx_pci_bios_map_irq(dev, slot, pin);
    #else
        return 0;
    #endif
    }
    else { /* PCIe */
    #ifdef CONFIG_IFX_PCIE
        return ifx_pcie_bios_map_irq(dev, slot, pin);
    #else
        return 0;
    #endif
    }
}

/** 
 * \fn int pcibios_plat_dev_init(struct pci_dev *dev)
 * \brief Called to perform platform specific PCI setup 
 * 
 * \param dev The Linux PCI device structure for the device to map
 * \return  0 OK
 * \ingroup IFX_PCI_COM_OS
 */ 
int 
pcibios_plat_dev_init(struct pci_dev *dev)
{
    int pos;
    
    pos = pci_find_capability(dev, PCI_CAP_ID_EXP);
    if (pos <= 0) {
    #ifdef CONFIG_IFX_PCI     
        ifx_pci_bios_plat_dev_init(dev);
    #else
        return 0;
    #endif
    }
    else {
    #ifdef CONFIG_IFX_PCIE
        ifx_pcie_bios_plat_dev_init(dev);
    #else
        return 0;
    #endif
    }
    return 0;
}

