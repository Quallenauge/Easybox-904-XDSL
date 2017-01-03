/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
/*!
  \file ifxmips_pcie_vr9.h
  \ingroup IFX_PCIE
  \brief PCIe RC driver vr9 specific file
*/

#ifndef IFXMIPS_PCIE_VR9_H
#define IFXMIPS_PCIE_VR9_H

#include <linux/types.h>
#include <linux/delay.h>

/* Project header file */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>

/*ctc*/
#define IFX_PCIE_GPIO_RESET  38
//#define IFX_PCIE_GPIO_RESET  19

static const int ifx_pcie_gpio_module_id = IFX_GPIO_MODULE_PCIE;

static inline void pcie_ep_gpio_rst_init(int pcie_port)
{
    ifx_gpio_pin_reserve(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
    ifx_gpio_output_set(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
    ifx_gpio_altsel0_clear(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
    ifx_gpio_altsel1_clear(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
}

static inline void pcie_ahb_pmu_setup(void) 
{
    /* Enable AHB bus master/slave */
    AHBM_PMU_SETUP(IFX_PMU_ENABLE);
    AHBS_PMU_SETUP(IFX_PMU_ENABLE);
}

static inline void pcie_rcu_endian_setup(int pcie_port)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_RCU_AHB_ENDIAN);
#ifdef CONFIG_IFX_PCIE_HW_SWAP
    reg |= IFX_RCU_AHB_BE_PCIE_M;
    reg |= IFX_RCU_AHB_BE_PCIE_S;
    reg &= ~IFX_RCU_AHB_BE_XBAR_M;
#else 
    reg |= IFX_RCU_AHB_BE_PCIE_M;
    reg &= ~IFX_RCU_AHB_BE_PCIE_S;
    reg &= ~IFX_RCU_AHB_BE_XBAR_M;
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
    IFX_REG_W32(reg, IFX_RCU_AHB_ENDIAN);
    IFX_PCIE_PRINT(PCIE_MSG_REG, "%s IFX_RCU_AHB_ENDIAN: 0x%08x\n", __func__, IFX_REG_R32(IFX_RCU_AHB_ENDIAN));
}

static inline void pcie_phy_pmu_enable(int pcie_port)
{
    PCIE_PHY_PMU_SETUP(IFX_PMU_ENABLE);
}

static inline void pcie_phy_pmu_disable(int pcie_port)
{
    PCIE_PHY_PMU_SETUP(IFX_PMU_DISABLE);
}

static inline void pcie_pdi_big_endian(int pcie_port)
{
    u32 reg;

    /* SRAM2PDI endianness control. */
    reg = IFX_REG_R32(IFX_RCU_AHB_ENDIAN);
    /* Config AHB->PCIe and PDI endianness */
    reg |= IFX_RCU_AHB_BE_PCIE_PDI;
    IFX_REG_W32(reg, IFX_RCU_AHB_ENDIAN);
}

static inline void pcie_pdi_pmu_enable(int pcie_port)
{
    /* Enable PDI to access PCIe PHY register */
    PDI_PMU_SETUP(IFX_PMU_ENABLE);
}

static inline void pcie_core_rst_assert(int pcie_port)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_RCU_RST_REQ);

    /* Reset PCIe PHY & Core, bit 22, bit 26 may be affected if write it directly  */
    reg |= 0x00400000;
    IFX_REG_W32(reg, IFX_RCU_RST_REQ);
}

static inline void pcie_core_rst_deassert(int pcie_port)
{
    u32 reg;

    /* Make sure one micro-second delay */
    udelay(1);

    /* Reset PCIe PHY & Core, bit 22 */
    reg = IFX_REG_R32(IFX_RCU_RST_REQ);
    reg &= ~0x00400000;
    IFX_REG_W32(reg, IFX_RCU_RST_REQ);
}

static inline void pcie_phy_rst_assert(int pcie_port)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_RCU_RST_REQ);
    reg |= 0x00001000; /* Bit 12 */
    IFX_REG_W32(reg, IFX_RCU_RST_REQ);
}

static inline void pcie_phy_rst_deassert(int pcie_port)
{
    u32 reg;

    /* Make sure one micro-second delay */
    udelay(1);

    reg = IFX_REG_R32(IFX_RCU_RST_REQ);
    reg &= ~0x00001000; /* Bit 12 */
    IFX_REG_W32(reg, IFX_RCU_RST_REQ);
}

static inline void pcie_device_rst_assert(int pcie_port)
{
    ifx_gpio_output_clear(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
}

static inline void pcie_device_rst_deassert(int pcie_port)
{
    mdelay(100);
    ifx_gpio_output_set(IFX_PCIE_GPIO_RESET, ifx_pcie_gpio_module_id);
}

static inline void pcie_core_pmu_setup(int pcie_port)
{
    /* PCIe Core controller enabled */
    PCIE_CTRL_PMU_SETUP(IFX_PMU_ENABLE);

    /* Enable PCIe L0 Clock */
    PCIE_L0_CLK_PMU_SETUP(IFX_PMU_ENABLE);
}

static inline void pcie_msi_init(int pcie_port)
{
    pcie_msi_pic_init(pcie_port);
    MSI_PMU_SETUP(IFX_PMU_ENABLE);
}

static inline u32
ifx_pcie_bus_nr_deduct(u32 bus_number, int pcie_port)
{
    u32 tbus_number = bus_number;

#ifdef CONFIG_IFX_PCI
    if (pcibios_host_nr() > 1) {
        tbus_number -= pcibios_1st_host_bus_nr();
    }
#endif /* CONFIG_IFX_PCI */
    return tbus_number;
}

static inline u32
ifx_pcie_bus_enum_hack(struct pci_bus *bus, u32 devfn, int where, u32 value, int pcie_port, int read)
{
    struct pci_dev *pdev;
    u32 tvalue = value;

    /* Sanity check */
    pdev = pci_get_slot(bus, devfn);
    if (pdev == NULL) {
        return tvalue;
    }

    /* Only care about PCI bridge */
    if (pdev->hdr_type != PCI_HEADER_TYPE_BRIDGE) {
        return tvalue;
    }

    if (read) { /* Read hack */
    #ifdef CONFIG_IFX_PCI
        if (pcibios_host_nr() > 1) {
            tvalue = ifx_pcie_bus_enum_read_hack(where, tvalue);
        }
    #endif /* CONFIG_IFX_PCI */  
    }
    else { /* Write hack */
    #ifdef CONFIG_IFX_PCI    
        if (pcibios_host_nr() > 1) {
            tvalue = ifx_pcie_bus_enum_write_hack(where, tvalue);
        }
    #endif
    }
    return tvalue;
}

#endif /* IFXMIPS_PCIE_VR9_H */

