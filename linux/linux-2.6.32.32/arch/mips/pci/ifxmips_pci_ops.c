/******************************************************************************
 *
 * FILE NAME    : ifxmips_pci_ops.c
 * PROJECT      : IFX UEIP
 * MODULES      : PCI
 *
 * DATE         : 29 June 2009
 * AUTHOR       : Lei Chuanhua
 *
 * DESCRIPTION  : PCI Basic Read/Write Operation
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
 * $Version $Date         $Author      $Comment
 * 1.0      29 Jun, 2009  Lei Chuanhua First UEIP release 
*******************************************************************************/
/*!
  \file ifxmips_pci_ops.c
  \ingroup IFX_PCI
  \brief pci bus driver common operation source file
*/
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/cache.h>
#include <asm/mipsmtregs.h>
#include <asm/paccess.h>
#include <asm/pci.h>
#include <asm/io.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>

#include "ifxmips_pci_reg.h"
#include "ifxmips_pci.h"

static DEFINE_SPINLOCK(ifx_pci_lock);
int ifx_pci_bus_status = 0;

volatile void __iomem *ifx_pci_cfg_space;

struct resource ifx_pci_io_resource = {
    .name   = "PCI I/O resources",
    .start  = IFX_PCI_IO_PHY_BASE,
    .end    = IFX_PCI_IO_PHY_BASE + IFX_PCI_IO_SIZE,
    .flags  = IORESOURCE_IO,
};

static struct resource ifx_pci_mem_resource = {
    .name   = "PCI Memory resources",
    .start  = IFX_PCI_MEM_PHY_BASE,
    .end    = IFX_PCI_MEM_PHY_BASE + IFX_PCI_MEM_SIZE,
    .flags  = IORESOURCE_MEM,
};

/*
 * Address for type 0 config is as follows:
 * AD:
 *  1:0 00 indicates type zero transaction
 *  7:2    indicates the target config dword
 *  10:8   indicates the target function within the physical device
 *  31:11  are reserved (and most probably used to connect idsels)
 */
inline u32
pci_config_addr(u8 bus_num, u16 devfn, int where)
{
    u32 addr;

    if (!bus_num) {
        /* type 0 */
        addr = ((PCI_SLOT(devfn) & 0x1F) << 11) | ((PCI_FUNC(devfn) & 0x7) << 8) | ((where &0xFF) & ~3);
    } 
    else {
        /* type 1 */
        addr = (bus_num << 16) | ((PCI_SLOT(devfn) & 0x1F) << 11) |
                ((PCI_FUNC(devfn) & 0x7) << 8) | ((where & 0xFF) & ~3) | 1;
    }

    return addr;
}

/*
 * Read/write 32-bit values in config space.
 */
inline u32
ifx_pci_config_read (u32 addr)
{
    return *(volatile u32 *)((u32)ifx_pci_cfg_space + addr);
}

inline void
ifx_pci_config_write (u32 addr, u32 data)
{
    *(volatile u32 *)((u32)ifx_pci_cfg_space + addr) = data;
}

/*
 * Some checks before doing config cycles:
 * In PCI Device Mode, hide everything on bus 0 except the LDT host
 * bridge.  Otherwise, access is controlled by bridge MasterEn bits.
 */
static int 
ifx_pci_can_access(struct pci_bus *bus, int devfn, int where)
{
    u32 devno;

    if (!(ifx_pci_bus_status & PCI_BUS_ENABLED))
        return 0;

    if (bus->number == 0) {
        devno = PCI_SLOT(devfn);
        if (get_dbe(devno, (volatile u32*) ((u32)ifx_pci_cfg_space + pci_config_addr(bus->number, devfn, where)))) {
            return 0;
        } 
        else {
            return 1;
        }
    } 
    else {
        return 1;
    }
}

static inline void
ifx_pci_clear_master_abort(struct pci_bus *bus)
{
    u32 temp;
    
    /* Clear possible Master abort, use PCI access to clear it */
    temp = ifx_pci_config_read(pci_config_addr(bus->number, 0, 4));

#ifdef CONFIG_IFX_PCI_HW_SWAP
    temp = le32_to_cpu(temp);
#endif
    ifx_pci_config_write(pci_config_addr(bus->number, 0x68, 4), temp);
}

/** 
 * \fn static int ifx_pcibios_read(struct pci_bus *bus, unsigned int devfn,
                 int where, int size, u32 * val)
 * \brief Read a value from configuration space 
 * 
 * \param[in] bus    Pointer to pci bus
 * \param[in] devfn  PCI device function number
 * \param[in] where  PCI register number 
 * \param[in] size   Register read size
 * \param[out] val    Pointer to return value
 * \return PCIBIOS_BAD_REGISTER_NUMBER Invalid register number
 * \return PCIBIOS_DEVICE_NOT_FOUND    PCI device not found
 * \return PCIBIOS_SUCCESSFUL          OK
 * \ingroup IFX_PCI_OS
 */ 
static int 
ifx_pcibios_read(struct pci_bus *bus, unsigned int devfn,
                 int where, int size, u32 * val)
{
    u32 data = 0;
    int err = PCIBIOS_SUCCESSFUL;
#ifdef CONFIG_IFX_PCI_HW_SWAP
    u32 devno = PCI_SLOT(devfn);
#endif /* CONFIG_IFX_PCI_HW_SWAP */
    
    if (unlikely(size != 1 && size != 2 && size != 4)){
        err = PCIBIOS_BAD_REGISTER_NUMBER;
        goto out;
    }

    /* Make sure the address is aligned to natural boundary */
    if (unlikely(((size - 1) & where))) {
        err = PCIBIOS_BAD_REGISTER_NUMBER;
        goto out;
    }


    IFX_PCI_PRINT("%s: %02x:%02x.%01x/%02x:%01d\n", __func__, bus->number,
        PCI_SLOT(devfn), PCI_FUNC(devfn), where, size); 

    if (!ifx_pci_can_access(bus, devfn, where)) {
        err =  PCIBIOS_DEVICE_NOT_FOUND;
        goto out;
    }
    
    PCI_IRQ_LOCK(ifx_pci_lock);    
    data = ifx_pci_config_read(pci_config_addr(bus->number, devfn, where));

    /* Prevent lxdb-1-1 optimization which causes system crash  */   
    __sync();  

#ifdef CONFIG_IFX_PCI_HW_SWAP
    if (devno != 0) {
        data = le32_to_cpu(data);
    }
#endif
    switch (size) {
    case 1:
        *val = (data >> ((where & 3) << 3)) & 0xffU;
        break;
    case 2:
        *val = (data >> ((where & 3) << 3)) & 0xffffU;
        break;
    case 4:
        *val = data;
        break;
    }
    PCI_IRQ_UNLOCK(ifx_pci_lock);      
    ifx_pci_clear_master_abort(bus);
out:
    return err;
}

/** 
 * \fn static int ifx_pcibios_write(struct pci_bus *bus, unsigned int devfn,
                  int where, int size, u32 val)
 * \brief Write a value to PCI configuration space 
 * 
 * \param[in] bus    Pointer to pci bus
 * \param[in] devfn  PCI device function number
 * \param[in] where  PCI register number 
 * \param[in] size   The register size to be written
 * \param[in] val    The valule to be written
 * \return PCIBIOS_BAD_REGISTER_NUMBER Invalid register number
 * \return PCIBIOS_DEVICE_NOT_FOUND    PCI device not found
 * \return PCIBIOS_SUCCESSFUL          OK
 * \ingroup IFX_PCI_OS
 */ 
static int 
ifx_pcibios_write(struct pci_bus *bus, unsigned int devfn,
                  int where, int size, u32 val)
{
    u32 cfgaddr = pci_config_addr(bus->number, devfn, where);
    u32 data = 0, shift;
#ifdef CONFIG_IFX_PCI_HW_SWAP      
    u32 devno = PCI_SLOT(devfn);
#endif /* CONFIG_IFX_PCI_HW_SWAP */
    int err = PCIBIOS_SUCCESSFUL;

    /* Make sure the address is aligned to natural boundary */
    if (unlikely(((size - 1) & where))) {
        err = PCIBIOS_BAD_REGISTER_NUMBER;
        goto out;
    }

    if (!ifx_pci_can_access(bus, devfn, where)){
        err = PCIBIOS_DEVICE_NOT_FOUND;
        goto out;
    }

    IFX_PCI_PRINT("%s: %02x:%02x.%01x/%02x:%01d value=%08x\n", __func__, 
        bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, val);

    PCI_IRQ_LOCK(ifx_pci_lock); 
    data = ifx_pci_config_read(cfgaddr);
#ifdef CONFIG_IFX_PCI_HW_SWAP
    if (devno != 0) {
        data = le32_to_cpu(data);
    }
#endif
    switch (size) {
    case 1:
        shift = (where & 3) << 3;
        data &= ~(0xffU << shift);
        data |= ((val & 0xffU) << shift);
        break;
    case 2:
        shift = (where & 3) << 3;
        data &= ~(0xffffU << shift);
        data |= ((val & 0xffffU) << shift);
        break;
    case 4:
        data = val;
        break;
    }
#ifdef CONFIG_IFX_PCI_HW_SWAP
    if (devno != 0) {
        data = cpu_to_le32(data);
    }
#endif
    ifx_pci_config_write(cfgaddr, data);
    PCI_IRQ_UNLOCK(ifx_pci_lock);
    
    ifx_pci_clear_master_abort(bus);
out:
    return err;
}

static struct pci_ops ifx_pci_ops = {
    .read  = ifx_pcibios_read,
    .write = ifx_pcibios_write,
};

struct pci_controller ifx_pci_controller = {
    .pci_ops        = &ifx_pci_ops,
    .mem_resource   = &ifx_pci_mem_resource,
    .io_resource    = &ifx_pci_io_resource,
};

