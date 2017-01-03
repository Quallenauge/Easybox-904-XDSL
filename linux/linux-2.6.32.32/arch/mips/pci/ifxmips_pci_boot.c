
/****************************************************************************
                              Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kallsyms.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_rcu.h>

#include "ifxmips_pci_reg.h"
#include "ifxmips_pci_dev_fw.h"
#include "ifxmips_pci_boot.h"

#define IFX_PCI_BOOT_VER_MAJOR          1
#define IFX_PCI_BOOT_VER_MID            0
#define IFX_PCI_BOOT_VER_MINOR          2

/* GPIO in global view */
#define IFX_PCI_RST  21

static const int pci_gpio_module_id = IFX_GPIO_MODULE_PCI;

static void
pci_disable_external_pci(void)
{
    IFX_REG_RMW32_FILED(IFX_PCI_ARB_PCI_MASTER_REQ2, 3, IFX_PCI_ARB);
    smp_wmb();
    /* make sure EBUSY is low && Frame Ird is high) */
    while ((IFX_REG_R32(IFX_PCI_ARB) & 
        (IFX_PCI_ARB_PCI_NOT_READY | IFX_PCI_ARB_PCI_NO_FRM | IFX_PCI_ARB_EBU_IDLE)) 
        != (IFX_PCI_ARB_PCI_NOT_READY | IFX_PCI_ARB_PCI_NO_FRM));
}

static void
pci_enable_external_pci(void)
{
    /* Clear means enabling all external masters request */
    IFX_REG_CLR_BIT(IFX_PCI_ARB_PCI_MASTER_REQ2, IFX_PCI_ARB);
    smp_wmb();
}

static void 
pci_core_rst(void) 
{
    u32 reg;
    unsigned long flags;
     
    local_irq_save(flags);
    /* Ack the interrupt */
    IFX_REG_W32(IFX_REG_R32(IFX_PCI_IRA), IFX_PCI_IRA);        

    /* Disable external masters */
    pci_disable_external_pci();
    
    /* PCI core reset start */
    reg = IFX_REG_R32(IFX_PCI_SFT_RST);
    reg |= IFX_PCI_SFT_RST_REQ;
    IFX_REG_W32(reg, IFX_PCI_SFT_RST);

    udelay(2);
    /* Wait for PCI core reset to be finished */
    while ((IFX_REG_R32(IFX_PCI_SFT_RST) & IFX_PCI_SFT_RST_ACKING));

    /* Out of reset to normal operation */
    reg = IFX_REG_R32(IFX_PCI_SFT_RST);
    reg &= ~IFX_PCI_SFT_RST_REQ;
    IFX_REG_W32(reg, IFX_PCI_SFT_RST);    

    udelay(1);
    /* Enable external masters */
    pci_enable_external_pci();
    local_irq_restore(flags);
}

static irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
pci_core_int_isr(int irq, void *dev_id)
#else
pci_core_int_isr(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    /* Only care about Timeout interrupt */
    if ((IFX_REG_R32(IFX_PCI_IRR) & IFX_PCI_IR_TIMEOUT) == IFX_PCI_IR_TIMEOUT) {
        printk(KERN_ERR "%s: PCI timeout occurred\n", __func__); 
        pci_core_rst();
    } 
    return IRQ_HANDLED;
}

static void 
pci_ir_irq_init(void)
{
    int ret;
 
    /* Clear the interrupt first */
    IFX_REG_W32(IFX_PCI_IR_TIMEOUT, IFX_PCI_IRA);

    ret = request_irq(IFX_PCI_IR, pci_core_int_isr,
                         IRQF_DISABLED, "pci_boot_ir", NULL);
    if (ret) {
        printk("%s:request irq %d failed with %d \n", __func__, IFX_PCI_IR, ret);
        return;
    }
    /* Unmask Timeout interrupt */
    IFX_REG_W32(IFX_PCI_IR_TIMEOUT, IFX_PCI_IRM);
}

static inline void
pci_dev_reset(void)
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
    ifx_gpio_altsel0_clear(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_altsel1_clear(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_open_drain_set(IFX_PCI_RST, pci_gpio_module_id);
    ifx_gpio_dir_out_set(IFX_PCI_RST, pci_gpio_module_id);
    mdelay(1);
    /* Reset external PCI device. */
    ifx_gpio_output_clear(IFX_PCI_RST, pci_gpio_module_id);
    smp_wmb();
    mdelay(5);
    ifx_gpio_output_set(IFX_PCI_RST, pci_gpio_module_id);
    smp_wmb();
    mdelay(1);
}

static void pci_host_por_out(void)
{
    u32 reg;

    reg = IFX_REG_R32(IFX_PCI_BOOT_POR_OUT);
    reg |= IFX_PCI_BOOT_PORT_RST;
    IFX_REG_W32(reg, IFX_PCI_BOOT_POR_OUT);
    mdelay(5);
}

static void pci_host_init(void)
{
    u32 reg;
    u32 devaddr;

    /* Choose reset from RCU first, then do reset */
    reg = IFX_REG_R32(IFX_CGU_PCI_CR);
    reg |= IFX_PCI_CLK_RESET_FROM_CGU; /* XXX, Should be RCU*/
    IFX_REG_W32(reg, IFX_CGU_PCI_CR);
    
    /* Secondly, RCU reset PCI domain including PCI CLK and PCI bridge */
    ifx_rcu_rst(IFX_RCU_DOMAIN_PCI, IFX_RCU_MODULE_PCI);

    reg = IFX_REG_R32(IFX_CGU_IF_CLK);
    reg &= ~ IFX_PCI_CLK_MASK;
    reg |= IFX_PCI_INTERNAL_CLK_SRC;    /* Host output Clock */
    
    reg |= IFX_PCI_CLK; 

    IFX_REG_W32(reg, IFX_CGU_IF_CLK);

    reg = IFX_REG_R32(IFX_CGU_PCI_CR);
     /* Internal Clock after PADs */
    reg |= IFX_PCI_CLK_FROM_CGU;
    reg &= ~IFX_PCI_DELAY_MASK;
    reg |= (6 << IFX_PCI_DELAY_SHIFT); /* 6ns delay */
    IFX_REG_W32(reg, IFX_CGU_PCI_CR);
    
    /* Enable auto-switching between PCI and EBU, normal ack */
    reg = IFX_PCI_CLK_CTRL_EBU_PCI_SWITCH_EN | IFX_PCI_CLK_CTRL_FPI_NORMAL_ACK;
    IFX_REG_W32(reg, IFX_PCI_CLK_CTRL);
    udelay(1);

    /* Configuration mode, i.e. configuration is not done, PCI access has to be retried */
    IFX_REG_CLR_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    smp_wmb();

    reg = IFX_REG_R32(IFX_PCI_ARB);

    /* Enable external 1 PCI master, ie, PCI device */
    reg &= ~(SM(1, IFX_PCI_ARB_PCI_PORT_ARB));
    
    /* Enable internal PCI master reqest */
    reg &= ~(SM(3, IFX_PCI_ARB_PCI_MASTER_REQ0));

    /* 
     * Enable EBU reqest for NOR and NAND boot on SMB and u-boot
     * needs to put MB in reset through POR_OUT <GPIO 15>
     * For SPI boot, we can disable it. 
     */
    reg |= (SM(3, IFX_PCI_ARB_PCI_MASTER_REQ1));
    //reg &= ~(SM(3, IFX_PCI_ARB_PCI_MASTER_REQ1));
    
    /* Enable all external masters request */
    reg &= ~(SM(3, IFX_PCI_ARB_PCI_MASTER_REQ2));

    /* Enable internal arbiter, only host has arbiter */
    reg |= IFX_PCI_ARB_INTERNAL_EN;
    
    IFX_REG_W32(reg, IFX_PCI_ARB);
    smp_wmb();

    /* PCI host outbound memory window configuration to access pci device register bank */ 
    reg = IFX_PCI_BOOT_FPI_BASE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP0);
    reg += IFX_PCI_BOOT_BLOCK_SIZE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP1);
    reg += IFX_PCI_BOOT_BLOCK_SIZE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP2);
    reg += IFX_PCI_BOOT_BLOCK_SIZE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP3);
    reg = IFX_PCI_BOOT_SRAM_BASE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP4);
    reg += IFX_PCI_BOOT_BLOCK_SIZE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP5);
    reg += IFX_PCI_BOOT_BLOCK_SIZE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP6);
    reg += IFX_PCI_BOOT_BLOCK_SIZE;
    IFX_REG_W32(reg, IFX_PCI_FPI_ADDR_MAP7);

    /* PCI host inbound memory window configuration for pci device to access host ddr */
#if defined(CONFIG_IFX_PPE_E5_OFFCHIP_BONDING)
    reg = 0x0f000008; /* BAR1 MASK, 16MB, prefetchable */
    IFX_REG_W32(reg, IFX_PCI_BAR11MASK);
    /* DDR from 16~32MMB */
    IFX_REG_W32(IFX_PCI_INBOUND_WINDOW_ONE_16MB_DDR_BASE, IFX_PCI_ADDR_MAP11);
#else
    reg = 0x0fe00008; /* BAR1 MASK, 2MB, prefetchable */
    IFX_REG_W32(reg, IFX_PCI_BAR11MASK);
    /* DDR from 14~16:MB */
    IFX_REG_W32(IFX_PCI_INBOUND_WINDOW_ONE_14MB_DDR_BASE, IFX_PCI_ADDR_MAP11);
#endif
    /* BAR1 value, it should cover PCI device bus address */
    IFX_REG_W32(IFX_PCI_INBOUND_WINDOW_ONE_BASE, IFX_PCI_BAR1);

    /* PCI host inbound memory window configuration for pci device to access host register bank */
#if defined(CONFIG_IFX_PPE_E5_OFFCHIP_BONDING)
    reg = 0x0fc00000; /* BAR2 MASK, 4MB, non-prefetchable */
    IFX_REG_W32(reg, IFX_PCI_BAR12MASK);
#else
    reg = 0x0f000000; /* BAR2 MASK, 16MB, non-prefetchable */
    IFX_REG_W32(reg, IFX_PCI_BAR12MASK);
#endif
    reg = IFX_PCI_INBOUND_WINDOW_TWO_SRAM_BASE;
    reg |= IFX_PCI_ADDR_MAP_SUPERVISOR;
    IFX_REG_W32(reg, IFX_PCI_ADDR_MAP12); /* Register bank, supervisor mode */
    /* BAR2 value, it should cover PCI device bus address */
    IFX_REG_W32(IFX_PCI_INBOUND_WINDOW_TWO_BASE, IFX_PCI_BAR2);
#if defined(CONFIG_IFX_PPE_E5_OFFCHIP_BONDING)
    /* PCI host inbound memory window configuration for pci device to access host PPE share buffer */
    reg = 0x0fc00000; /* BAR3 MASK, 4MB, non-prefetchable */
    IFX_REG_W32(reg, IFX_PCI_BAR13MASK);
    reg = IFX_PCI_INBOUND_WINDOW_THREE_FPI_BASE;
    reg |= IFX_PCI_ADDR_MAP_SUPERVISOR;
    IFX_REG_W32(reg, IFX_PCI_ADDR_MAP13); /* PPE share buffer, supervisor mode */
    /* BAR3 value, it should cover PCI device bus address */
    IFX_REG_W32(IFX_PCI_INBOUND_WINDOW_THREE_BASE, IFX_PCI_BAR3);
#endif

    reg = SM(IFX_PCI_FPI_BURST_LEN8, IFX_PCI_FPI_RD_BURST_LEN) |
          SM(IFX_PCI_FPI_BURST_LEN8, IFX_PCI_FPI_WR_BURST_LEN);
    IFX_REG_W32(reg, IFX_PCI_FPI_BURST_LENGTH);
    
    /* Enable bus master/IO/MEM access */
    reg = IFX_REG_R32(IFX_PCI_CMD);
    reg |= IFX_PCI_CMD_IO_EN | IFX_PCI_CMD_MEM_EN | IFX_PCI_CMD_MASTER_EN;
    IFX_REG_W32(reg, IFX_PCI_CMD);

    /* Configuration OK. */
    IFX_REG_SET_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    smp_wmb();
    mdelay(1);

    /* Get PCI POR <whole chip> out of reset*/
    pci_host_por_out();
    
    /* Reset PCI device here, IMO */
    pci_dev_reset();

    /* PCI device BAR configuration */
    devaddr = IFX_PCI_CFG_BASE + (IFX_PCI_BOOT_DEV_IDSEL - 16) * 0x800; /* 0xb7006800 */
    IFX_REG_W32(IFX_PCI_BOOT_DEV_BAR2_BASE, (volatile u32*)(devaddr + 0x14));

    /* PCI device IO/memory/bus master enabled */
    reg = IFX_REG_R32((volatile u32*)(devaddr + 0x04));
    reg |= IFX_PCI_CMD_IO_EN | IFX_PCI_CMD_MEM_EN | IFX_PCI_CMD_MASTER_EN;
    IFX_REG_W32(reg, (volatile u32*)(devaddr + 0x04));
    udelay(10);
}

static int pci_host_load_dev_fw(void)
{
	int i;
    u32 reg;
    u32 devaddr;
    
    /* Load miniFW from host to PCI device */
    devaddr = IFX_PCI_DEV_PPE_ADDR; /* Device PPE share buffer address 0x1e220000 */
    for (i = 0; i < ARRAY_SIZE(pci_dev_bm_fw); i++) {
        IFX_REG_W32(pci_dev_bm_fw[i], (volatile u32*)(devaddr + (i << 2)));
    }
 
    /* Read back and check if download is successful */
    for (i = 0; i < ARRAY_SIZE(pci_dev_bm_fw); i++) {
        if (IFX_REG_R32((volatile u32*)(devaddr + (i << 2))) != pci_dev_bm_fw[i]) {
            printk(KERN_ERR "%s miniloader download check failed!!!!!\n", __func__);
            return -1;
        }
    }

    IFX_REG_W32(IFX_PCI_DEV_PPE_LOCAL_ADDR, IFX_PCI_BOOT_DEV_MPS_ENTRY);
    reg = IFX_REG_R32(IFX_PCI_BOOT_DEV_RDY);
    reg |= IFX_PCI_BOOT_READY;
    IFX_REG_W32(reg, IFX_PCI_BOOT_DEV_RDY);

    /* Disable arbiter to improve throughput */ 
    IFX_REG_CLR_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    reg = IFX_REG_R32(IFX_PCI_ARB);
    reg |= IFX_PCI_ARB_PCI_PORT_ARB;
    reg &= ~IFX_PCI_ARB_INTERNAL_EN;
    IFX_REG_W32(reg, IFX_PCI_ARB);
    IFX_REG_SET_BIT(IFX_PCI_MOD_CFG_OK, IFX_PCI_MOD);
    smp_wmb();   
   
    /* Wait for PCI device to be reconfigured */
    mdelay(100);
    /* PCI device should be up and running, wait for PCI device to identify it is ready */
    while ((IFX_REG_R32(IFX_PCI_BOOT_DEV_RDY) & IFX_PCI_BOOT_READY) != 0) {
        ;
    }
    return 0;
}

static __init int pci_boot_init(void)
{
    char ver_str[128] = {0};
    
    pci_host_init();
    if (pci_host_load_dev_fw() != 0) {
        return -1;
    }
    pci_ir_irq_init();
    ifx_drv_ver(ver_str, "PCI Bonding Boot", 
        IFX_PCI_BOOT_VER_MAJOR, IFX_PCI_BOOT_VER_MID, IFX_PCI_BOOT_VER_MINOR);
    printk(KERN_INFO "%s", ver_str);
    return 0;
}

arch_initcall(pci_boot_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lei Chuanhua, chuanhua.lei@lantiq.com");
MODULE_SUPPORTED_DEVICE("Lantiq VRX PCI bonding devices");
MODULE_DESCRIPTION("Lantiq VRX PCI bonding boot up driver");

