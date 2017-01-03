/*************************************************************************
 * 
 * FILE NAME    : ifxmips_pci.h
 * PROJECT      : IFX UEIP
 * MODULES      : PCI
 *
 * DATE         : 29 June 2009
 * AUTHOR       : Lei Chuanhua
 *
 * DESCRIPTION  : PCI Host Controller Driver
 * COPYRIGHT    :       Copyright (c) 2009
 *                      Infineon Technologies AG
 *                      Am Campeon 1-12, 85579 Neubiberg, Germany
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 * HISTORY
 * $Version $Date      $Author     $Comment
 * 1.0      29 Jun     Lei Chuanhua First UEIP release
 *************************************************************************/

#ifndef IFXMIPS_PCI_H
#define IFXMIPS_PCI_H
#include <linux/version.h>
#include "ifxmips_pci_common.h"

/*!
 \defgroup IFX_PCI  PCI  bus driver module   
 \brief  PCI IP module support all CPEs 
*/

/*!
 \defgroup IFX_PCI_OS OS APIs
 \ingroup IFX_PCI
 \brief PCI bus driver OS interface functions
*/

/*!
 \file ifxmips_pci.h
 \ingroup IFX_PCI  
 \brief header file for PCI module common header file
*/

#define PCI_BRIDGE_DEVICE       0
#define PCI_BUS_ENABLED         1

//#define IFX_PCI_DBG
#if defined(IFX_PCI_DBG)
#define IFX_PCI_PRINT(_fmt, args...) printk(_fmt, ##args)
#else
#define IFX_PCI_PRINT(_m, _fmt, args...)   \
    do {} while(0)
#endif


#define PCI_IRQ_LOCK(lock) do {             \
    unsigned long flags;                     \
    spin_lock_irqsave(&(lock), flags);
#define PCI_IRQ_UNLOCK(lock)                \
    spin_unlock_irqrestore(&(lock), flags);  \
} while (0)


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#define IRQF_DISABLED SA_INTERRUPT
#endif

#endif /* IFXMIPS_PCI_H */

