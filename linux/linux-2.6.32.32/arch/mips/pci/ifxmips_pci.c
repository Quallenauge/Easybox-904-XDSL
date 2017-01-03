/*************************************************************************
 * 
 * FILE NAME    : ifxmips_pci.c
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
 /*!
  \file ifxmips_pci.c
  \ingroup IFX_PCI
  \brief pci bus driver source file
*/
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <asm/paccess.h>
#include <asm/addrspace.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/irq.h>
#include <asm/ifx/ifx_rcu.h>

#include "ifxmips_pci_reg.h"
#include "ifxmips_pci.h"

#ifdef CONFIG_IFX_PMCU
#include "ifxmips_pci_pm.h"
#endif /* CONFIG_IFX_PMCU */

#define IFX_PCI_VER_MAJOR          1
#define IFX_PCI_VER_MID            2
#define IFX_PCI_VER_MINOR          0

extern u32 max_pfn, max_low_pfn;

extern struct pci_controller ifx_pci_controller;
extern int ifx_pci_bus_status;
extern void __iomem *ifx_pci_cfg_space;
extern u32 pci_config_addr(u8 bus_num, u16 devfn, int where);

/* Used by ifxmips_interrupt.c to suppress bus exception */
int pci_bus_error_flag;

extern u32 ifx_pci_config_read(u32 addr);
extern void ifx_pci_config_write(u32 addr, u32 data);

#ifdef CONFIG_IFX_DUAL_MINI_PCI   
#define IFX_PCI_REQ1 29
#define IFX_PCI_GNT1 30
#endif /* CONFIG_IFX_DUAL_MINI_PCI */

static const int pci_gpio_module_id = IFX_GPIO_MODULE_PCI;

#ifdef CONFIG_IFX_PCI_DANUBE_EBU_LED_RST
#include <asm/ifx/ifx_ebu_led.h>

static inline void 
ifx_pci_dev_reset_init(void)
{
    ifx_ebu_led_enable();
    ifx_ebu_led_set_data(9, 1);
}

static inline void
ifx_pci_dev_reset(void)
{
    ifx_ebu_led_set_data(9, 0);
    mdelay(5);
    ifx_ebu_led_set_data(9, 1);
    mdelay(1);
    ifx_ebu_led_disable();
}
#else
/* GPIO in global view */
/*ctc*/
#define IFX_PCI_RST  21
//#define IFX_PCI_RST  45

static inline void 
ifx_pci_dev_reset_init(void)
{
    /* 
     * PCI_RST: P1.5 used as a general GPIO, instead of PCI_RST gpio.
     * In Danube/AR9, it reset internal PCI core and external PCI device
     * However, in VR9, it only resets external PCI device. Internal core 
     * reset by PCI software reset registers.
     * GPIO21 if used as PCI_RST, software can't control reset time.
     * Since it uses as a general GPIO, ALT should 0, 0.
     */
    ifx_gpio_pin_reserve(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_output_set(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_altsel1_clear(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_PCI_RST, pci_gpio_module_id);
}

static inline void
ifx_pci_dev_reset(void)
{
    /* Reset external PCI device. */
    ifx_gpio_output_clear(IFX_PCI_RST, pci_gpio_module_id);
    smp_wmb();
    mdelay(5);
    ifx_gpio_output_set(IFX_PCI_RST, pci_gpio_module_id);
    smp_wmb();
    mdelay(1);
}
#endif /* CONFIG_IFX_PCI_DANUBE_EBU_LED_RST */

static u32 round_up_to_next_power_of_two(u32 x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

static void
ifx_pci_startup(void)
{
    u32 reg; 

    /* Choose reset from RCU first, then do reset */
    reg = IFX_REG_R32(IFX_CGU_PCI_CR);
    reg |= IFX_PCI_CLK_RESET_FROM_CGU; /* XXX, Should be RCU*/
    IFX_REG_W32(reg, IFX_CGU_PCI_CR);
    
    /* Secondly, RCU reset PCI domain including PCI CLK and PCI bridge */
    ifx_rcu_rst(IFX_RCU_DOMAIN_PCI, IFX_RCU_MODULE_PCI);
    
    reg = IFX_REG_R32(IFX_CGU_IF_CLK);
    reg &= ~ IFX_PCI_CLK_MASK;

    /* 
     * CGU PCI CLK is specific with platforms. Danube has four bits,
     * AR9 and VR9 has five bits. Their definitions are in platform header
     * file
     */
#ifdef CONFIG_IFX_PCI_EXTERNAL_CLK_SRC
    reg &= ~IFX_PCI_INTERNAL_CLK_SRC;
    /* External clk has no need to set clk in register */
#else
    reg |= IFX_PCI_INTERNAL_CLK_SRC;
#ifdef CONFIG_IFX_PCI_INTERNAL_CLK_SRC_60
    /* PCI 62.5 MHz clock */
    reg |= IFX_PCI_60MHZ;
#else
    /* PCI 33.3 MHz clock */
    reg |= IFX_PCI_33MHZ;
#endif /* CONFIG_IFX_PCI_INTERNAL_CLK_SRC_60 */
#endif /* CONFIG_IFX_PCI_EXTERNAL_CLK_SRC */
    IFX_REG_W32(reg, IFX_CGU_IF_CLK);

    reg = IFX_REG_R32(IFX_CGU_PCI_CR);
#ifdef CONFIG_IFX_PCI_EXTERNAL_CLK_SRC
    reg &= ~IFX_PCI_CLK_FROM_CGU;
#else
    reg |= IFX_PCI_CLK_FROM_CGU; 
#endif /* CONFIG_IFX_PCI_EXTERNAL_CLK_SRC */

    reg &= ~IFX_PCI_DELAY_MASK;
    
#if !defined(CONFIG_IFX_PCI_CLOCK_DELAY_NANO_SECONDS) || CONFIG_IFX_PCI_CLOCK_DELAY_NANO_SECONDS < 0 || CONFIG_IFX_PCI_CLOCK_DELAY_NANO_SECONDS > 7 
    #error "please define CONFIG_IFX_PCI_CLOCK_DELAY_NANO_SECONDS properly"
#endif
    reg |= (CONFIG_IFX_PCI_CLOCK_DELAY_NANO_SECONDS << IFX_PCI_DELAY_SHIFT);

#if !defined(CONFIG_IFX_PCI_CLOCK_DELAY_TENTH_NANO_SECOND) || CONFIG_IFX_PCI_CLOCK_DELAY_TENTH_NANO_SECOND < 0 || CONFIG_IFX_PCI_CLOCK_DELAY_TENTH_NANO_SECOND > 5
    #error "Please CONFIG_CONFIG_IFX_PCI_CLOCK_DELAY_TENTH_NANO_SECOND properly"
#endif
    reg |= (CONFIG_IFX_PCI_CLOCK_DELAY_TENTH_NANO_SECOND << 18);

    IFX_REG_W32(reg, IFX_CGU_PCI_CR);
    
    ifx_pci_dev_reset_init();

#ifdef CONFIG_IFX_DUAL_MINI_PCI    
    /* PCI_REQ1: P1.13 ALT 01 */
    ifx_gpio_pin_reserve(IFX_PCI_REQ1, pci_gpio_module_id);
    ifx_gpio_dir_in_set(IFX_PCI_REQ1, pci_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_PCI_REQ1, pci_gpio_module_id);
    ifx_gpio_altsel1_clear(IFX_PCI_REQ1, pci_gpio_module_id);
    
    /* PCI_GNT1: P1.14 ALT 01 */
    ifx_gpio_pin_reserve(IFX_PCI_GNT1, pci_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_PCI_GNT1, pci_gpio_module_id);
    ifx_gpio_altsel0_set(IFX_PCI_GNT1, pci_gpio_module_id);
    ifx_gpio_altsel1_clear(IFX_PCI_GNT1, pci_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_PCI_GNT1, pci_gpio_module_id);
#endif /* CONFIG_IFX_DUAL_MINI_PCI */
    /* Enable auto-switching between PCI and EBU, normal ack */
    reg = IFX_PCI_CLK_CTRL_EBU_PCI_SWITCH_EN | IFX_PCI_CLK_CTRL_FPI_NORMAL_ACK;
    IFX_REG_W32(reg, IFX_PCI_CLK_CTRL);
    /* Configuration mode, i.e. configuration is not done, PCI access has to be retried */
    IFX_REG_CLR_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    smp_wmb();

    /* Enable bus master/IO/MEM access */
    reg = IFX_REG_R32(IFX_PCI_CMD);
    reg |= IFX_PCI_CMD_IO_EN | IFX_PCI_CMD_MEM_EN | IFX_PCI_CMD_MASTER_EN;
    IFX_REG_W32(reg, IFX_PCI_CMD);

    reg = IFX_REG_R32(IFX_PCI_ARB);
#ifdef CONFIG_IFX_DUAL_MINI_PCI
    /* Enable external 4 PCI masters */
    reg &= ~(SM(3, IFX_PCI_ARB_PCI_PORT_ARB));
#else
    /* Enable external 1 PCI masters */
    reg &= ~(SM(1, IFX_PCI_ARB_PCI_PORT_ARB));
#endif
    /* Enable internal arbiter */
    reg |= IFX_PCI_ARB_INTERNAL_EN;

    /* Enable internal PCI master reqest */
    reg &= ~(SM(3, IFX_PCI_ARB_PCI_MASTER_REQ0));

    /* Enable EBU reqest */
    reg &= ~(SM(3, IFX_PCI_ARB_PCI_MASTER_REQ1));
    
    /* Enable all external masters request */
    reg &= ~(SM(3, IFX_PCI_ARB_PCI_MASTER_REQ2));
    IFX_REG_W32(reg, IFX_PCI_ARB);
    smp_wmb();

    /* 
     * FPI ==> PCI MEM address mapping 
     * base: 0xb8000000 == > 0x18000000 
     * size: 8x4M = 32M 
     */
    reg = IFX_PCI_MEM_PHY_BASE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP0);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP1);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP2);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP3);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP4);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP5);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP6);
    reg += 0x400000;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP7);

    /* FPI ==> PCI IO address mapping 
     * base: 0xbAE00000 == > 0xbAE00000
     * size: 2M 
     */
    IFX_REG_W32(IFX_PCI_IO_PHY_BASE, IFX_PCI_FPI_ADDR_MAP11_HIGH);

    /* PCI ==> FPI address mapping 
     * base: 0x0 ==> 0x0 
     * size: 32M 
     */
    /* At least 16M. Otherwise there will be discontiguous memory region. */
    reg = IFX_PCI_BAR_PREFETCH;
    reg |= ((-round_up_to_next_power_of_two((max_low_pfn << PAGE_SHIFT))) & 0x0F000000);
    IFX_REG_W32(reg, IFX_PCI_BAR11MASK);
    
    IFX_REG_W32(0x0, IFX_PCI_ADDR_MAP11);
    IFX_REG_W32(0x0, IFX_PCI_BAR1);
    
#ifdef CONFIG_IFX_PCI_HW_SWAP 
    /* both TX and RX endian swap are enabled */
    reg = IFX_PCI_SWAP_RX | IFX_PCI_SWAP_TX;
    IFX_REG_W32(reg, IFX_PCI_SWAP);
    smp_wmb();
#endif
    reg = IFX_REG_R32(IFX_PCI_BAR12MASK);
    reg |= IFX_PCI_BAR_DECODING_EN;
    IFX_REG_W32(reg, IFX_PCI_BAR12MASK);
    
    reg = IFX_REG_R32(IFX_PCI_BAR13MASK);
    reg |= IFX_PCI_BAR_DECODING_EN;
    IFX_REG_W32(reg, IFX_PCI_BAR13MASK);
    
    reg = IFX_REG_R32(IFX_PCI_BAR14MASK);
    reg |= IFX_PCI_BAR_DECODING_EN;
    IFX_REG_W32(reg, IFX_PCI_BAR14MASK);
    
    reg = IFX_REG_R32(IFX_PCI_BAR15MASK);
    reg |= IFX_PCI_BAR_DECODING_EN;
    IFX_REG_W32(reg, IFX_PCI_BAR15MASK);
    
    reg = IFX_REG_R32(IFX_PCI_BAR16MASK);
    reg |= IFX_PCI_BAR_DECODING_EN;
    IFX_REG_W32(reg, IFX_PCI_BAR16MASK); 

    /* Use 4 dw burse length, XXX 8 dw will cause PCI timeout */
    reg = SM(IFX_PCI_FPI_BURST_LEN4, IFX_PCI_FPI_RD_BURST_LEN) |
          SM(IFX_PCI_FPI_BURST_LEN4, IFX_PCI_FPI_WR_BURST_LEN);
    IFX_REG_W32(reg, IFX_PCI_FPI_BURST_LENGTH);

    /* Configuration OK. */
    IFX_REG_SET_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    smp_wmb();
    mdelay(1);
    
    ifx_pci_dev_reset();
}

 /* Brief:  disable external pci aribtor request
  * Details:
  *    blocking call, i.e. only return when there is no external PCI bus activities
  */
void
ifx_disable_external_pci(void)
{
    
    IFX_REG_RMW32_FILED(IFX_PCI_ARB_PCI_MASTER_REQ2, 3, IFX_PCI_ARB);
    smp_wmb();
    /* make sure EBUSY is low && Frame Ird is high) */
    while ((IFX_REG_R32(IFX_PCI_ARB) & 
        (IFX_PCI_ARB_PCI_NOT_READY | IFX_PCI_ARB_PCI_NO_FRM | IFX_PCI_ARB_EBU_IDLE)) 
        != (IFX_PCI_ARB_PCI_NOT_READY | IFX_PCI_ARB_PCI_NO_FRM));
}

/* Brief:  enable external pci aribtor request
 * Details:
 *     non-blocking call
 */
void
ifx_enable_external_pci(void)
{
    /* Clear means enabling all external masters request */
    IFX_REG_CLR_BIT(IFX_PCI_ARB_PCI_MASTER_REQ2, IFX_PCI_ARB);
    smp_wmb();
}

#ifdef CONFIG_DANUBE_EBU_PCI_SW_ARBITOR
void 
ifx_enable_ebu(void)
{
    u32 reg;

    /* Delay before enabling ebu ??? */

    /* Disable int/ext pci request */
    reg = IFX_REG_R32(IFX_PCI_ARB);
    reg &= ~(IFX_PCI_ARB_PCI_MASTER_REQ1);
    reg |= (IFX_PCI_ARB_PCI_MASTER_REQ0 | IFX_PCI_ARB_PCI_MASTER_REQ2);
    IFX_REG_W32(reg, IFX_PCI_ARB);
    
    /* Poll for pci bus idle */
    reg = IFX_REG_R32(IFX_PCI_ARB);
    while ((reg & IFX_PCI_ART_PCI_IDLE) != IFX_PCI_ART_PCI_IDLE) {
        reg = IFX_REG_R32(IFX_PCI_ARB);
    };

    /* EBU only, Arbitor fix*/
    IFX_REG_W32(0, IFX_PCI_CLK_CTRL);

    /* 
     * Unmask CFRAME_MASK changing PCI's Config Space via internal path 
     * might need to change to external path 
     */
    /* Start configuration, one burst read is allowed */
    reg = IFX_REG_R32(IFX_PCI_MOD);
    reg &= ~IFX_PCI_MOD_CFG_OK;
    reg |= IFX_PCI_MOD_TWO_IRQ_INTA_AND_INTB | SM(1, IFX_PCI_MOD_READ_BURST_THRESHOLD);
    IFX_REG_W32(reg, IFX_PCI_MOD);

    reg = IFX_REG_R32(IFX_PCI_CARDBUS_FRAME_MASK);
    reg &= ~IFX_PCI_CARDBUS_FRAME_MASK_EN;
    IFX_REG_W32(reg, IFX_PCI_CARDBUS_FRAME_MASK);
    
    IFX_REG_SET_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);

    reg = IFX_REG_R32(IFX_PCI_CARDBUS_FRAME_MASK);
}
EXPORT_SYMBOL(ifx_enable_ebu);

void 
ifx_disable_ebu(void)
{
    u32 reg;
    
    /* Delay before enabling ebu ??? */

    /* Restore EBU and PCI auto switch */
    IFX_REG_W32(IFX_PCI_CLK_CTRL_EBU_PCI_SWITCH_EN, IFX_PCI_CLK_CTRL);
    /* 
     * unmask CFRAME_MASK changing PCI's Config Space via internal path
     * might need to change to external path 
     */
    reg = IFX_REG_R32(IFX_PCI_MOD);
    
    /* Start configuration, one burst read is allowed */
    reg &= ~IFX_PCI_MOD_CFG_OK;
    reg |= IFX_PCI_MOD_TWO_IRQ_INTA_AND_INTB | SM(1, IFX_PCI_MOD_READ_BURST_THRESHOLD);
    IFX_REG_W32(reg, IFX_PCI_MOD);

    reg = IFX_REG_R32(IFX_PCI_CARDBUS_FRAME_MASK);
    reg |= IFX_PCI_CARDBUS_FRAME_MASK_EN;

    IFX_REG_W32(reg, IFX_PCI_CARDBUS_FRAME_MASK);
    IFX_REG_SET_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    
    /* Enable int/ext pci request */
    reg = IFX_REG_R32(IFX_PCI_ARB);
    reg &= ~(IFX_PCI_ARB_PCI_MASTER_REQ0 | IFX_PCI_ARB_PCI_MASTER_REQ2);
    reg |= IFX_PCI_ARB_PCI_MASTER_REQ1;
    IFX_REG_W32(reg, IFX_PCI_ARB);
}
EXPORT_SYMBOL(ifx_disable_ebu);

#endif /* CONFIG_DANUBE_EBU_PCI_SW_ARBITOR */

static void __devinit 
pcibios_fixup_resources(struct pci_dev *dev)
{
    struct pci_controller* hose = (struct pci_controller *)dev->sysdata;
    int i;
    unsigned long offset;

    if (!hose) {
        printk(KERN_ERR "No hose for PCI dev %s!\n", pci_name(dev));
        return;
    }
    
    for (i = 0; i < DEVICE_COUNT_RESOURCE; i++) {
        struct resource *res = dev->resource + i;

        if (!res->flags)
            continue;
        if (res->end == 0xffffffff) {
            printk(KERN_INFO "PCI:%s Resource %d [%016llx-%016llx] is unassigned\n",
                pci_name(dev), i, (u64)res->start, (u64)res->end);
            res->end -= res->start;
            res->start = 0;
            res->flags |= IORESOURCE_UNSET;
            continue;
        }
        
        offset = 0;
        if (res->flags & IORESOURCE_MEM) {
            offset = hose->mem_offset;
        } 
        else if (res->flags & IORESOURCE_IO) {
            offset = hose->io_offset;
        }
        if (offset != 0) {
            res->start += offset;
            res->end += offset;
            printk(KERN_INFO "Fixup res %d (%lx) of dev %s: %llx -> %llx\n",
                i, res->flags, pci_name(dev),
                (u64)res->start - offset, (u64)res->start);
        }
    }

    IFX_PCI_PRINT("[%s %s %d]: %s\n", __FILE__, __func__, __LINE__, pci_name(dev));

    /* Enable I/O, MEM, Bus Master, Special Cycles SERR, Fast back-to-back */
    pci_write_config_word(dev, PCI_COMMAND, 
        PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER |
        PCI_COMMAND_SPECIAL | PCI_COMMAND_SERR | PCI_COMMAND_FAST_BACK);
}
DECLARE_PCI_FIXUP_EARLY(PCI_ANY_ID, PCI_ANY_ID, pcibios_fixup_resources);

/** 
 * \fn int ifx_pci_bios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
 * \brief Map a PCI device to the appropriate interrupt line 
 * 
 * \param[in] dev    The Linux PCI device structure for the device to map 
 * \param[in] slot   The slot number for this device on __BUS 0__. Linux 
 *               enumerates through all the bridges and figures out the 
 *               slot on Bus 0 where this device eventually hooks to. 
 * \param[in] pin    The PCI interrupt pin read from the device, then swizzled 
 *               as it goes through each bridge. 
 * \return Interrupt number for the device
 * \ingroup IFX_PCI_OS
 */
int 
ifx_pci_bios_map_irq(IFX_PCI_CONST struct pci_dev *dev, u8 slot, u8 pin)
{
    int irq = -1;
        
    IFX_PCI_PRINT("%s dev %s slot %d pin %d \n", __func__, pci_name(dev), slot, pin);
    switch (pin) {
    case 0:
        break;
    case 1:
        IFX_PCI_PRINT("%s dev %s: interrupt pin 1\n", __func__, pci_name(dev));
        /*
         * PCI_INTA--shared with EBU
         * falling edge level triggered:0x4, low level:0xc, rising edge:0x2
         */
        IFX_REG_W32(IFX_EBU_PCC_CON_IREQ_LOW_LEVEL_DETECT, IFX_EBU_PCC_CON);
        /* enable interrupt only */
        IFX_REG_W32(IFX_EBU_PCC_IEN_PCI_EN, IFX_EBU_PCC_IEN);
        irq = INT_NUM_IM0_IRL22;
        break;
    case 2:
    case 3:
        break;
    default:
        printk(KERN_WARNING "WARNING: %s dev %s: invalid interrupt pin %d\n", __func__, pci_name(dev), pin);
        break;
    }
    return irq;
}

/** 
 * \fn int ifx_pci_bios_plat_dev_init(struct pci_dev *dev)
 * \brief Called to perform platform specific PCI setup 
 * 
 * \param[in] dev The Linux PCI device structure for the device to map
 * \return OK
 * \ingroup IFX_PCI_OS
 */ 
int 
ifx_pci_bios_plat_dev_init(struct pci_dev *dev)
{
    return 0;
}

static void inline 
ifx_pci_core_rst(void) 
{
    u32 reg;
    unsigned long flags;
    volatile int i;
     
    local_irq_save(flags);
    /* Ack the interrupt */
    IFX_REG_W32(IFX_REG_R32(IFX_PCI_IRA), IFX_PCI_IRA);        

    /* Disable external masters */
    ifx_disable_external_pci();
    
    /* PCI core reset start */
    reg = IFX_REG_R32(IFX_PCI_SFT_RST);
    reg |= IFX_PCI_SFT_RST_REQ;
    IFX_REG_W32(reg, IFX_PCI_SFT_RST);

    for (i = 0; i < 100; i++);
    /* Wait for PCI core reset to be finished */
    while ((IFX_REG_R32(IFX_PCI_SFT_RST) & IFX_PCI_SFT_RST_ACKING));

    /* Out of reset to normal operation */
    reg = IFX_REG_R32(IFX_PCI_SFT_RST);
    reg &= ~IFX_PCI_SFT_RST_REQ;
    IFX_REG_W32(reg, IFX_PCI_SFT_RST);    

    for (i = 0; i < 50; i++);
    /* Enable external masters */
    ifx_enable_external_pci();
    local_irq_restore(flags);
}

static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
ifx_pci_core_int_isr(int irq, void *dev_id)
#else
ifx_pci_core_int_isr(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    /* Only care about Timeout interrupt */
    if ((IFX_REG_R32(IFX_PCI_IRR) & IFX_PCI_IR_TIMEOUT) == IFX_PCI_IR_TIMEOUT) {
        printk(KERN_ERR "%s: PCI timeout occurred\n", __func__); 
        ifx_pci_core_rst();
    } 
    return IRQ_HANDLED;
}

static void 
ifx_pci_ir_irq_init(void)
{
    int ret;
 
    /* Clear the interrupt first */
    IFX_REG_W32(IFX_PCI_IR_TIMEOUT, IFX_PCI_IRA);

    ret = request_irq(IFX_PCI_IR, ifx_pci_core_int_isr,
                         IRQF_DISABLED, "ifx_pci_ir", NULL);
    if (ret) {
        printk("%s:request irq %d failed with %d \n", __func__, IFX_PCI_IR, ret);
        return;
    }
    /* Unmask Timeout interrupt */
    IFX_REG_W32(IFX_PCI_IR_TIMEOUT, IFX_PCI_IRM);
}

/*! 
 * \fn static int __init ifx_pci_init(void)
 * \brief  Initialize the IFX PCI host controller, register with PCI 
 *         bus subsystem.
 *
 * \return -ENOMEM configuration/io space mapping failed.
 * \return -EIO    pci bus not initialized
 * \return  0      OK
 * \ingroup IFX_PCI_OS
 */ 
static int __init
ifx_pci_init(void)
{
    u32 cmdreg;
    void __iomem *io_map_base;
    char ver_str[128] = {0};
    
    pci_bus_error_flag = 1;

    ifx_pci_startup();
    ifx_pci_cfg_space = ioremap_nocache(IFX_PCI_CFG_PHY_BASE, IFX_PCI_CFG_SIZE);
    if (ifx_pci_cfg_space == NULL) {
        printk(KERN_ERR "%s configuration space ioremap failed\n", __func__);
        return -ENOMEM;
    }

    IFX_PCI_PRINT("[%s %s %d]: ifx_pci_cfg_space %p\n", __FILE__, __func__, __LINE__, ifx_pci_cfg_space);

    /* Otherwise, warning will pop up */
    io_map_base = ioremap(IFX_PCI_IO_PHY_BASE, IFX_PCI_IO_SIZE);
    if (io_map_base == NULL) {
        iounmap(ifx_pci_cfg_space);
        IFX_PCI_PRINT("%s io space ioremap failed\n", __func__);
        return -ENOMEM;
    }
    ifx_pci_controller.io_map_base = (unsigned long)io_map_base;

    cmdreg = ifx_pci_config_read(pci_config_addr(0, PCI_DEVFN(PCI_BRIDGE_DEVICE, 0), PCI_COMMAND));
    if (!(cmdreg & PCI_COMMAND_MASTER)) {
        printk(KERN_INFO "PCI: Skipping PCI probe. Bus is not initialized.\n");
        iounmap(ifx_pci_cfg_space);
        return -EIO;
    }
    ifx_pci_bus_status |= PCI_BUS_ENABLED;

    /* Turn on ExpMemEn */
    cmdreg = ifx_pci_config_read(pci_config_addr(0, PCI_DEVFN(PCI_BRIDGE_DEVICE, 0), 0x40));
    ifx_pci_config_write(pci_config_addr(0, PCI_DEVFN(PCI_BRIDGE_DEVICE, 0), 0x40),
                        cmdreg | 0x10);
    cmdreg = ifx_pci_config_read(pci_config_addr(0, PCI_DEVFN(PCI_BRIDGE_DEVICE, 0), 0x40));

    /* Enable normal FPI bus exception after we configured everything */
    IFX_REG_CLR_BIT(IFX_PCI_CLK_CTRL_FPI_NORMAL_ACK, IFX_PCI_CLK_CTRL);

    IFX_PCI_PRINT("[%s %s %d]: mem_resource @%p, io_resource @%p\n", __FILE__, __func__, __LINE__, 
                &ifx_pci_controller.mem_resource, &ifx_pci_controller.io_resource);

    register_pci_controller(&ifx_pci_controller);

    ifx_pci_ir_irq_init();

#ifdef CONFIG_IFX_PMCU
    ifx_pci_pmcu_init();
#endif /* CONFIG_IFX_PMCU */

    ifx_drv_ver(ver_str, "PCI host controller", IFX_PCI_VER_MAJOR, IFX_PCI_VER_MID, IFX_PCI_VER_MINOR);
    printk(KERN_INFO "%s", ver_str);
    return 0;
}

arch_initcall(ifx_pci_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Chuanhua, chuanhua.lei@infineon.com");
MODULE_SUPPORTED_DEVICE("Infineon builtin PCI module for Danube AR9 and VR9");
MODULE_DESCRIPTION("Infineon builtin PCI host controller driver");

