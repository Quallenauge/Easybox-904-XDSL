/******************************************************************************
**
** FILE NAME    : ifxmips_vr9_clk.c
** PROJECT      : UEIP
** MODULES     	: CGU
**
** DATE         : 19 JUL 2005
** AUTHOR       : Xu Liang
** DESCRIPTION  : Clock Generation Unit (CGU) Driver
** COPYRIGHT    : 	Copyright (c) 2006
**			Infineon Technologies AG
**			Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 19 JUL 2005  Xu Liang        Initiate Version
** 21 AUG 2006  Xu Liang        Work around to fix calculation error for 36M
**                              crystal.
** 23 OCT 2006  Xu Liang        Add GPL header.
** 28 Sept 2007 Teh Kok How	Use kernel interface for 64-bit arithmetics
** 28 May 2009  Huang Xiaogang  The first UEIP release
*******************************************************************************/
/*!
  \file ifxmips_vr9_clk.c
  \ingroup IFX_CGU
  \brief This file contains Clock Generation Unit driver
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/irq.h>
#include <asm/div64.h>
#include <asm/time.h>
#include <asm/r4kcache.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_clk.h>
#include <asm/ifx/common_routines.h>
#ifdef CONFIG_USE_EMULATOR
#include <asm/ifx/vr9/emulation.h>
#endif

#define ENABLE_SET_CLOCK_PROFILING      0

#define BARRIER __asm__ __volatile__(".set noreorder\n\t" \
                                     "nop; nop; nop; nop; nop; nop; nop; nop; nop;\n\t" \
                                     "nop; nop; nop; nop; nop; nop; nop; nop; nop;\n\t" \
                                     ".set reorder\n\t")
/*
 * MPS SRAM Base Address
 */
#define MBX_BASEADDRESS                 0xBF200000
#define MPS_MEM_SEG_DATASIZE            512

/*
 *  Frequency of Clock Direct Feed from The Analog Line Driver Chip
 */
#define BASIC_INPUT_CLOCK_FREQUENCY     36000000

/*
 *  CGU PLL0 Configure Register
 */
#define CGU_PLL0_PLL_BP                 (*IFX_CGU_PLL0_CFG & (1 << 30))
#define CGU_PLL1_FMOD_S                 GET_BITS(*IFX_CGU_PLL0_CFG, 23, 22)
#define CGU_PLL0_PS_2_EN                (*IFX_CGU_PLL0_CFG & (1 << 21))
#define CGU_PLL0_PS_1_EN                (*IFX_CGU_PLL0_CFG & (1 << 20))
#define CGU_PLL0_CFG_PLLN               GET_BITS(*IFX_CGU_PLL0_CFG, 13, 6)
#define CGU_PLL0_CFG_PLLM               GET_BITS(*IFX_CGU_PLL0_CFG, 5, 2)
#define CGU_PLL0_CFG_PLLL               (*IFX_CGU_PLL0_CFG & (1 << 1))
#define CGU_PLL0_CFG_PLL_EN             (*IFX_CGU_PLL0_CFG & (1 << 0))

/*
 *  CGU PLL1 Configure Register
 */
#define CGU_PLL1_PHDIV_EN               (*IFX_CGU_PLL1_CFG & (1 << 31))
#define CGU_PLL1_PLL_BP                 (*IFX_CGU_PLL1_CFG & (1 << 30))
#define CGU_PLL1_CFG_CTEN               (*IFX_CGU_PLL1_CFG & (1 << 29))
#define CGU_PLL1_CFG_DSMSEL             (*IFX_CGU_PLL1_CFG & (1 << 28))
#define CGU_PLL1_CFG_FRAC_EN            (*IFX_CGU_PLL1_CFG & (1 << 27))
#define CGU_PLL1_CFG_PLLK_HI            GET_BITS(*IFX_CGU_PLL1_CFG, 26, 17)
#define CGU_PLL2_CFG_PLL1_K_LO          GET_BITS(*IFX_CGU_PLL2_CFG, 29, 20)
#define CGU_PLL1_CFG_PLLD               GET_BITS(*IFX_CGU_PLL1_CFG, 16, 13)
#define CGU_PLL1_CFG_PLLN               GET_BITS(*IFX_CGU_PLL1_CFG, 12, 6)
#define CGU_PLL1_CFG_PLLM               GET_BITS(*IFX_CGU_PLL1_CFG, 5, 2)
#define CGU_PLL1_CFG_PLLL               (*IFX_CGU_PLL1_CFG & (1 << 1))
#define CGU_PLL1_CFG_PLL_EN             (*IFX_CGU_PLL1_CFG & (1 << 0))

/*
 *  CGU PLL2 Configure/Status Register
 */
#define CGU_PLL2_CFG_PLL_BP             (*IFX_CGU_PLL2_CFG & (1 << 30))
#define CGU_PLL2_CFG_PLL1_K             GET_BITS(*IFX_CGU_PLL2_CFG, 29, 20)
#define CGU_PLL2_CFG_PLLN               GET_BITS(*IFX_CGU_PLL2_CFG, 13, 6)
#define CGU_PLL2_CFG_PLLM               GET_BITS(*IFX_CGU_PLL2_CFG, 5, 2)
#define CGU_PLL2_CFG_PLLL               (*IFX_CGU_PLL2_CFG & (1 << 1))
#define CGU_PLL2_CFG_PLL_EN             (*IFX_CGU_PLL2_CFG & (1 << 0))

/*
 *  CGU Clock Sys Mux Register
 */
#define CGU_SYS_CPU                     GET_BITS(*IFX_CGU_SYS, 7, 4)
#define CGU_SYS_OCP                     GET_BITS(*IFX_CGU_SYS, 2, 0)

/*
 * CGU CGU Clock Frequency Select Register
 */
#define CGU_CLKFSR_FPIM                 (*IFX_CGU_CLKFSR & (1 << 6))
#define CGU_CLKFSR_FPIS                 (*IFX_CGU_CLKFSR & (1 << 5))
#define CGU_CLKFSR_AHBM                 (*IFX_CGU_CLKFSR & (1 << 4))
#define CGU_CLKFSR_AHBS                 (*IFX_CGU_CLKFSR & (1 << 3))
#define CGU_CLKFSR_PPE                  GET_BITS(*IFX_CGU_CLKFSR, 18, 16)
/*
 *  CGU Update Register
 */
#define CGU_UPDATE_FSC_INT_TYPE         (*IFX_CGU_UPDATE & (1 << 1))
#define CGU_UPDATE_UPD                  (*IFX_CGU_UPDATE & (1 << 0))

/*
 *  CGU Interface Clock Register
 */
#define CGU_IF_CLK_FPI_CLK              GET_BITS(*IFX_CGU_IF_CLK, 28, 25)
#define CGU_IF_CLK_PCI_CLK              GET_BITS(*IFX_CGU_IF_CLK, 24, 20)
#define CGU_IF_CLK_PDA                  (*IFX_CGU_IF_CLK & (1 << 19))
#define CGU_IF_CLK_PCI_B                (*IFX_CGU_IF_CLK & (1 << 18))
#define CGU_IF_CLK_PCIBM                (*IFX_CGU_IF_CLK & (1 << 17))
#define CGU_IF_CLK_PCIS                 (*IFX_CGU_IF_CLK & (1 << 16))
#define CGU_IF_CLK_CLKOD0               GET_BITS(*IFX_CGU_IF_CLK, 15, 14)
#define CGU_IF_CLK_CLKOD1               GET_BITS(*IFX_CGU_IF_CLK, 13, 12)
#define CGU_IF_CLK_CLKOD2               GET_BITS(*IFX_CGU_IF_CLK, 11, 10)
#define CGU_IF_CLK_CLKOD3               GET_BITS(*IFX_CGU_IF_CLK, 9, 8)
#define CGU_IF_CLK_SI_CLK_EN            (*IFX_CGU_IF_CLK & (1 << 7))
#define CGU_IF_CLK_GPHY_SEL             GET_BITS(*IFX_CGU_IF_CLK, 4, 2)
#define CGU_IF_CLK_USB_SEL              GET_BITS(*IFX_CGU_IF_CLK, 1, 0)

/*
 *  CGU DDR Memory Control Register
 */

/*
 *  CGU CT Status Register 1
 */
#define CGU_CT1SR_PDOUT                 GET_BITS(*IFX_CGU_CT1SR, 13, 0)

/*
 *  CGU CT Kval Register
 */
#define CGU_CT_KVAL_PLL1K               GET_BITS(*IFX_CGU_CT_KVAL, 19, 0)

/*
 *  CGU PCM Control Register
 */
#define CGU_PCMCR_INT_SEL               GET_BITS(*IFX_CGU_PCMCR, 29, 28)
#define CGU_PCMCR_DCL_SEL               GET_BITS(*IFX_CGU_PCMCR, 27, 25)
#define CGU_PCMCR_MUXDCL                (*IFX_CGU_MUXDCL & (1 << 22))
#define CGU_PCMCR_MUXFSC                (*IFX_CGU_MUXDCL & (1 << 18))
#define CGU_PCMCR_PCM_SL                (*IFX_CGU_MUXDCL & (1 << 13))
#define CGU_PCMCR_DNTR                  GET_BITS(*IFX_CGU_PCMCR, 12, 11)
#define CGU_PCMCR_NTRS                  (*IFX_CGU_MUXDCL & (1 << 10))
#define CGU_PCMCR_AC97_EN               (*IFX_CGU_MUXDCL & (1 << 9))
#define CGU_PCMCR_CTTMUX                (*IFX_CGU_MUXDCL & (1 << 8))
#define CGU_PCMCR_CT_MUX_SEL            GET_BITS(*IFX_CGU_PCMCR, 7, 6)
#define CGU_PCMCR_FSC_DUTY              (*IFX_CGU_MUXDCL & (1 << 5))
#define CGU_PCMCR_CTM_SEL               (*IFX_CGU_MUXDCL & (1 << 4))

/*
 *  PCI Clock Control Register
 */
#define CGU_PCI_CR_PADSEL               (*IFX_CGU_PCI_CR & (1 << 31))
#define CGU_PCI_CR_RESSEL               (*IFX_CGU_PCI_CR & (1 << 30))
#define CGU_PCI_CR_PCID_H               GET_BITS(*IFX_CGU_PCI_CR, 23, 21)
#define CGU_PCI_CR_PCID_L               GET_BITS(*IFX_CGU_PCI_CR, 20, 18)

/*
 *  Pre-declaration of File Operations
 */
static int cgu_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static int cgu_open(struct inode *, struct file *);
static int cgu_release(struct inode *, struct file *);

/*
 *  Calculate PLL Frequency
 */
static inline u32 get_input_clock(void);
static inline u32 cal_dsm(int, u32, u32);
static inline u32 mash_dsm(int, u32, u32, u32);
static inline u32 ssff_dsm_1(int, u32, u32, u32);
static inline u32 ssff_dsm_2(int, u32, u32, u32, u32);
static inline u32 dsm(int, u32 M, u32, u32, u32, u32, u32);
static inline u32 cgu_get_pll0_fosc(void);
static inline u32 cgu_get_pll1_fosc(void);
static inline u32 cgu_get_pll2_fosc(void);
inline u32 ifx_get_ddr_hz(void);

/*
 * MPS SRAM backup mem pointer
 */
static	char	*argv;

/*
 *  Proc Filesystem
 */
#ifdef CONFIG_PROC_FS
static struct proc_dir_entry* g_gpio_dir = NULL;
static inline void proc_file_create(void);
static inline void proc_file_delete(void);
static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int proc_read_cgu(char *, char **, off_t, int, int *, void *);
static int proc_write_cgu(struct file *, const char *, unsigned long, void *);
#endif
#ifdef CONFIG_IFX_CLOCK_CHANGE
  static inline int stricmp(const char *, const char *);
  static int get_number(char **, int *, int);
  static inline void ignore_space(char **, int *);
#endif

/*
 *  Init Help Functions
 */
static inline int ifx_cgu_version(char *);

#ifdef CONFIG_IFX_CLOCK_CHANGE
  u32 cgu_set_clock(u32, u32, u32);
#endif

static struct file_operations cgu_fops = {
    ioctl:      cgu_ioctl,
    open:       cgu_open,
    release:    cgu_release
};

static int cgu_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    struct cgu_clock_rates rates;

    if ( _IOC_TYPE(cmd) != IFX_CGU_IOC_MAGIC
        || _IOC_NR(cmd) >= CGU_IOC_MAXNR )
        return -ENOTTY;

    if ( _IOC_DIR(cmd) & _IOC_READ )
        ret = !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd));
    else if ( _IOC_DIR(cmd) & _IOC_WRITE )
        ret = !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd));
    if ( ret )
        return -EFAULT;

    switch ( cmd )
    {
    case IFX_CGU_GET_CLOCK_RATES:
        /*  Calculate Clock Rates   */
        rates.cpu       = ifx_get_cpu_hz();
        rates.fpi_bus1  = cgu_get_fpi_bus_clock(1);
        rates.fpi_bus2  = cgu_get_fpi_bus_clock(2);
        rates.pp32      = cgu_get_pp32_clock();
        rates.pci       = cgu_get_pci_clock();
        rates.mii0      = cgu_get_ethernet_clock();
        rates.mii1      = cgu_get_ethernet_clock();
        rates.usb       = cgu_get_usb_clock();
        rates.clockout0 = cgu_get_clockout(0);
        rates.clockout1 = cgu_get_clockout(1);
        rates.clockout2 = cgu_get_clockout(2);
        rates.clockout3 = cgu_get_clockout(3);
        /*  Copy to User Space      */
        copy_to_user((char*)arg, (char*)&rates, sizeof(rates));

        ret = 0;
        break;
    case IFX_CGU_IOC_VERSION:
        {
            struct ifx_cgu_ioctl_version version = {
                .major = IFX_CGU_VER_MAJOR,
                .mid   = IFX_CGU_VER_MID,
                .minor = IFX_CGU_VER_MINOR
            };
            ret = copy_to_user((void *)arg, (void *)&version, sizeof(version));
        }
        break;
    default:
        ret = -ENOTTY;
    }

    return ret;
}

static int cgu_open(struct inode *inode, struct file *file)
{
    return IFX_SUCCESS;
}

static int cgu_release(struct inode *inode, struct file *file)
{
    return IFX_SUCCESS;
}

/*
 *  Description:
 *    get input clock frequency according to GPIO config
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of input clock
 */
static inline u32 get_input_clock(void)
{
    return BASIC_INPUT_CLOCK_FREQUENCY;
}

/*
 *  Description:
 *    common routine to calculate PLL frequency
 *  Input:
 *    num --- u32, numerator
 *    den --- u32, denominator
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 cal_dsm(int pll, u32 num, u32 den)
{
    u64 res;

    res = get_input_clock();
    res *= num;
    do_div(res, den);

    return res;
}

/*
 *  Description:
 *    calculate PLL frequency following MASH-DSM
 *  Input:
 *    M   --- u32, denominator coefficient
 *    N   --- u32, numerator integer coefficient
 *    K   --- u32, numerator fraction coefficient
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 mash_dsm(int pll, u32 M, u32 N, u32 K)
{
    u32 num = ((N + 1) << 10) + K;
    u32 den = (M + 1) << 10;

    return cal_dsm(pll, num, den);
}

/*
 *  Description:
 *    calculate PLL frequency following SSFF-DSM (0.25 < fraction < 0.75)
 *  Input:
 *    M   --- u32, denominator coefficient
 *    N   --- u32, numerator integer coefficient
 *    K   --- u32, numerator fraction coefficient
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 ssff_dsm_1(int pll, u32 M, u32 N, u32 K)
{
    u32 num = ((N + 1) << 11) + K + 512;
    u32 den = (M + 1) << 11;

    return cal_dsm(pll, num, den);
}

/*
 *  Description:
 *    calculate PLL frequency following SSFF-DSM
 *    (fraction < 0.125 || fraction > 0.875)
 *  Input:
 *    M   --- u32, denominator coefficient
 *    N   --- u32, numerator integer coefficient
 *    K   --- u32, numerator fraction coefficient
 *  Output:
 *    u32 --- frequency the PLL output
 */
static inline u32 ssff_dsm_2(int pll, u32 M, u32 N, u32 K, u32 modulo)
{
    u32 offset[4] = {512, 2560, 1536, 3584}; //  FIX the order
    u32 num = ((N + 1) << 12) + K + offset[modulo];
    u32 den = (M + 1) << 12;

    return cal_dsm(pll, num, den);
}

/*
 *  Description:
 *    calculate PLL frequency
 *  Input:
 *    M            --- u32, denominator coefficient
 *    N            --- u32, numerator integer coefficient
 *    K            --- u32, numerator fraction coefficient
 *    dsmsel       --- int, 0: MASH-DSM, 1: SSFF-DSM
 *    phase_div_en --- int, 0: 0.25 < fraction < 0.75
 *                          1: fraction < 0.125 || fraction > 0.875
 *  Output:
 *    u32          --- frequency the PLL output
 */
static inline u32 dsm(int pll, u32 M, u32 N, u32 K, u32 dsmsel, u32 phase_div_en, u32 modulo)
{
    if ( !dsmsel )
        return mash_dsm(pll, M, N, K);
    else
    {
        if ( !phase_div_en )
            return ssff_dsm_1(pll, M, N, K);
        else
            return ssff_dsm_2(pll, M, N, K, modulo);
    }
}

/*
 *  Description:
 *    get oscillate frequency of PLL0
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL0 Fosc
 */
static inline u32 cgu_get_pll0_fosc(void)
{
    if (!CGU_PLL0_CFG_PLL_EN)
        return 0;
    else if (CGU_PLL0_PLL_BP)
        return get_input_clock();
    else
    	return cal_dsm(0, 2*(CGU_PLL0_CFG_PLLN + 1), CGU_PLL0_CFG_PLLM + 1);
}

/*
 *  Description:
 *    get oscillate frequency of PLL2
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL2 Fosc
 */
static inline u32 cgu_get_pll2_fosc(void)
{
    if (!CGU_PLL2_CFG_PLL_EN)
        return 0;
    else if (CGU_PLL2_CFG_PLL_BP)
        return get_input_clock();
    else
	return cal_dsm(0, 2*(CGU_PLL2_CFG_PLLN + 1), CGU_PLL2_CFG_PLLM + 1);
}

/*
 *  Description:
 *    get oscillate frequency of PLL1
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PLL1 Fosc
 */
static inline u32 cgu_get_pll1_fosc(void)
{
    if ( !CGU_PLL1_CFG_PLL_EN )
        return 0;
    else if ( CGU_PLL1_PLL_BP )
        return get_input_clock();
    else
    {
        if ( !CGU_PLL1_CFG_FRAC_EN )
            return dsm(1, CGU_PLL1_CFG_PLLM, CGU_PLL1_CFG_PLLN, 0, 0, 0, 0);
        else
            return dsm(1, CGU_PLL1_CFG_PLLM, CGU_PLL1_CFG_PLLN, CGU_PLL1_CFG_PLLK_HI, CGU_PLL1_CFG_DSMSEL, CGU_PLL1_PHDIV_EN, CGU_PLL1_FMOD_S);
    }
}

/*
 *  Description:
 *    get frequency of DDR
 *  Output:
 *    u32 --- frequency of DDR
 */
inline u32 ifx_get_ddr_hz(void)
{
    switch (CGU_SYS_OCP){
    case 0: return ifx_get_cpu_hz(); 		//OCP ratio = 1
    case 2: return ifx_get_cpu_hz() >> 1; 	//OCP ratio = 2
    case 3: return (ifx_get_cpu_hz() * 2) / 5;  //OCP ratio = 2.5
    case 4: return ifx_get_cpu_hz() / 3;	//OCP ratio = 3
    default: return ifx_get_cpu_hz() >> 2;
    }
}
EXPORT_SYMBOL(ifx_get_ddr_hz);

/*
 *  Description:
 *    get frequency of FPI bus,
 *    On VR9 fpi has the same speed as DDR.
 *  Input:
 *    fpi --- int, 1: FPI bus 1 (Slave), 2: FPI bus 2 (Master)
 *  Output:
 *    u32 --- frequency of FPI bus
 */
u32 cgu_get_fpi_bus_clock(int fpi)
{
    if(fpi == 2)
    	return (CGU_CLKFSR_FPIM >> 6) ? (ifx_get_ddr_hz() >> 1) : ifx_get_ddr_hz();
    else if(fpi == 1)
	return (CGU_CLKFSR_FPIS >> 5) ? (ifx_get_ddr_hz() >> 1) : ifx_get_ddr_hz();	
    else {
	printk("Invalid fpi number\n");
	return IFX_ERROR;
    }
}

/*
 *  Description:
 *    get frequency of PP32 processor
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PP32 processor
 */
u32 cgu_get_pp32_clock(void)
{
    switch (CGU_CLKFSR_PPE){
    case 0: return CLOCK_500M;
    case 1: return cgu_get_pll2_fosc() >> 1;
    case 2: return cgu_get_pll2_fosc() / 3;
    default: return cgu_get_pll2_fosc() >> 1;
    }
}

/*
 *  Description:
 *    get frequency of PCI bus
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of PCI bus
 */
u32 cgu_get_pci_clock(void)
{
    switch(CGU_IF_CLK_PCI_CLK){
    case (0x7): return (cgu_get_pll0_fosc() >> 1) / (7 + 1);
    case (0xe): return (cgu_get_pll0_fosc() >> 1) / (14 + 1);
    default: return (cgu_get_pll0_fosc() >> 1) / (7 + 1);
    }
}

/*
 *  Description:
 *    get frequency of ethernet module (MII)
 *  Input:
 *    mii --- int, 0: mii0, 1: mii1
 *  Output:
 *    u32 --- frequency of ethernet module
 */
u32 cgu_get_ethernet_clock(void)
{
    return 50000000;
}

/*
 *  Description:
 *    get frequency of USB
 *  Input:
 *    none
 *  Output:
 *    u32 --- frequency of USB
 */
u32 cgu_get_usb_clock(void)
{
    switch ( CGU_IF_CLK_USB_SEL )
    {
    case 0:
    case 3:
        return (get_input_clock() + 1) / 3;
    case 1:
        return get_input_clock();
    }
    return 0;
}

/*
 *  Description:
 *    get frequency of CLK_OUT pin
 *  Input:
 *    clkout --- int, clock out pin number
 *  Output:
 *    u32    --- frequency of CLK_OUT pin
 */
u32 cgu_get_clockout(int clkout)
{
  #if 0
    u32 fosc1 = cgu_get_pll1_fosc();

    if ( clkout > 3 || clkout < 0 )
        return 0;

    switch ( ((u32)clkout << 2) | GET_BITS(*IFX_CGU_IF_CLK, 15 - clkout * 2, 14 - clkout * 2) )
    {
    case 0: /*  32.768KHz   */
    case 15:
        return (fosc1 + 6000) / 12000;
    case 1: /*  1.536MHz    */
        return (fosc1 + 128) / 256;
    case 2: /*  2.5MHz      */
        return (fosc2 + 60) / 120;
    case 3: /*  12MHz       */
    case 5:
    case 12:
        return (fosc2 + 12) / 25;
    case 4: /*  40MHz       */
        return (cgu_get_pll2_fps(2) + 3) / 6;
    case 6: /*  24MHz       */
        return (cgu_get_pll2_fps(2) + 5) / 10;
    case 7: /*  48MHz       */
        return (cgu_get_pll2_fps(2) + 2) / 5;
    case 8: /*  25MHz       */
    case 14:
        return (fosc2 + 6) / 12;
    case 9: /*  50MHz       */
    case 13:
        return (fosc2 + 3) / 6;
    case 10:/*  30MHz       */
        return (fosc2 + 5) / 10;
    case 11:/*  60MHz       */
        return (fosc2 + 2) / 5;
    }
  #endif

    return 0;
}
#ifdef CONFIG_PROC_FS
static inline void proc_file_create(void)
{
    struct proc_dir_entry *res;

    g_gpio_dir = proc_mkdir("driver/ifx_cgu", NULL);

    create_proc_read_entry("version",
                            0,
                            g_gpio_dir,
                            proc_read_version,
                            NULL);

    res = create_proc_entry("clk_setting",
                            0,
                            g_gpio_dir);
    if ( res )
    {
        res->read_proc  = proc_read_cgu;
        res->write_proc = proc_write_cgu;
    }
}

static inline void proc_file_delete(void)
{
    remove_proc_entry("clk_setting", g_gpio_dir);
    remove_proc_entry("driver/ifx_cgu", NULL);
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_cgu_version(buf + len);

    if ( offset >= len ) {
        *start = buf;
        *eof = 1;
        return 0;
    }
    *start = buf + offset;
    if ( (len -= offset) > count )
        return count;
    *eof = 1;
    return len;
}

static int proc_read_cgu(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "pll0 N = %d, M = %d, PLL_BP = %d, PLL_EN = %d\n",
                CGU_PLL0_CFG_PLLN, CGU_PLL0_CFG_PLLM, CGU_PLL0_PLL_BP >> 30, CGU_PLL0_CFG_PLL_EN);
    len += sprintf(page + off + len, "pll1 N = %d, M = %d, K_hi = %d, K_lo = %d, phdiven = %d, dsmsel = %d, modulo = %d PLL_BP = %d PLL_EN = %d\n",
                CGU_PLL1_CFG_PLLN, CGU_PLL1_CFG_PLLM, CGU_PLL1_CFG_PLLK_HI, CGU_PLL2_CFG_PLL1_K_LO,
                CGU_PLL1_PHDIV_EN >> 31, CGU_PLL1_CFG_DSMSEL >> 28, CGU_PLL1_FMOD_S, CGU_PLL1_PLL_BP >> 30, CGU_PLL1_CFG_PLL_EN);
    len += sprintf(page + off + len, "pll2 N = %d, M = %d, PLL_BP = %d, PLL_EN = %d\n",
                CGU_PLL2_CFG_PLLN, CGU_PLL2_CFG_PLLM, CGU_PLL2_CFG_PLL_BP >> 30, CGU_PLL2_CFG_PLL_EN);
    len += sprintf(page + off + len, "pll0_fosc    = %d\n", cgu_get_pll0_fosc());
    len += sprintf(page + off + len, "pll1_fosc    = %d\n", cgu_get_pll1_fosc());
    len += sprintf(page + off + len, "pll2_fosc    = %d\n", cgu_get_pll2_fosc());
    len += sprintf(page + off + len, "cpu clock    = %d\n", ifx_get_cpu_hz());
    len += sprintf(page + off + len, "DDR clock    = %d\n", ifx_get_ddr_hz());
    len += sprintf(page + off + len, "FPI bus 1    = %d\n", ifx_get_fpi_hz());
    len += sprintf(page + off + len, "FPI bus 2    = %d\n", ifx_get_fpi_hz());
    len += sprintf(page + off + len, "PP32 clock   = %d\n", cgu_get_pp32_clock());
    len += sprintf(page + off + len, "PCI clock    = %d\n", cgu_get_pci_clock());
    len += sprintf(page + off + len, "Ethernet MII0= %d\n", cgu_get_ethernet_clock());
    len += sprintf(page + off + len, "Ethernet MII1= %d\n", cgu_get_ethernet_clock());
    len += sprintf(page + off + len, "USB clock    = %d\n", cgu_get_usb_clock());
    len += sprintf(page + off + len, "Clockout0    = %d\n", cgu_get_clockout(0));
    len += sprintf(page + off + len, "Clockout1    = %d\n", cgu_get_clockout(1));
    len += sprintf(page + off + len, "Clockout2    = %d\n", cgu_get_clockout(2));
    len += sprintf(page + off + len, "Clockout3    = %d\n", cgu_get_clockout(3));

    *eof = 1;

    return len;
}

static int proc_write_cgu(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        return 0;
    }

#ifdef CONFIG_IFX_CLOCK_CHANGE
    if ( stricmp(p, "help") == 0 )
        printk("echo <cpu clk> <ddr clk> <fpi clk> > /proc/driver/ifx_cgu/clk_setting\n");
    else
    {
        int cpu_clk = 0, ddr_clk = 0, fpi_clk = 0;

        cpu_clk = get_number(&p, &rlen, 0);
        ignore_space(&p, &rlen);
        if ( rlen > 0 )
        {
            ddr_clk = get_number(&p, &rlen, 0);
            ignore_space(&p, &rlen);
            if ( rlen > 0 )
                fpi_clk = get_number(&p, &rlen, 0);
        }

        if ( !fpi_clk )
            fpi_clk = ddr_clk;

        switch ( cpu_clk )
        {
        case 600:
        case 500:
        case 393:
        case 333:
        case 125:
            if ( cgu_set_clock(cpu_clk, ddr_clk, fpi_clk) )
                printk("fail in function \"cgu_set_clock\"\n");
            break;
        default:
            printk("do not support cpu_clk = %d\n", cpu_clk);
        }
    }
#endif

    return count;
}
#endif //CONFIG_PROC_FS

#ifdef CONFIG_IFX_CLOCK_CHANGE
static inline int stricmp(const char *p1, const char *p2)
{
    int c1, c2;

    while ( *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
    }

    return *p1 - *p2;
}

static int get_number(char **p, int *len, int is_hex)
{
    int ret = 0;
    int n = 0;

    if ( (*p)[0] == '0' && (*p)[1] == 'x' )
    {
        is_hex = 1;
        (*p) += 2;
        (*len) -= 2;
    }

    if ( is_hex )
    {
        while ( *len && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F')) )
        {
            if ( **p >= '0' && **p <= '9' )
                n = **p - '0';
            else if ( **p >= 'a' && **p <= 'f' )
               n = **p - 'a' + 10;
            else if ( **p >= 'A' && **p <= 'F' )
                n = **p - 'A' + 10;
            ret = (ret << 4) | n;
            (*p)++;
            (*len)--;
        }
    }
    else
    {
        while ( *len && **p >= '0' && **p <= '9' )
        {
            n = **p - '0';
            ret = ret * 10 + n;
            (*p)++;
            (*len)--;
        }
    }

    return ret;
}

static inline void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}
#endif //CONFIG_IFX_CLOCK_CHANGE

#ifdef CONFIG_IFX_CLOCK_CHANGE
 #if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
  static unsigned int g_extra_counter_start, g_extra_counter_end;
 #endif
extern void ifx_update_asc_clock_settings(void);

//static void dummy_func(void) {}

extern unsigned long cycles_per_jiffy;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
extern struct clocksource clocksource_mips;
#else
static struct clocksource clocksource_mips;
#endif

#if 0
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static u64 clocksource_max_deferment(struct clocksource *cs)
{
	u64 max_nsecs, max_cycles;

	/*
	 * Calculate the maximum number of cycles that we can pass to the
	 * cyc2ns function without overflowing a 64-bit signed result. The
	 * maximum number of cycles is equal to ULLONG_MAX/cs->mult which
	 * is equivalent to the below.
	 * max_cycles < (2^63)/cs->mult
	 * max_cycles < 2^(log2((2^63)/cs->mult))
	 * max_cycles < 2^(log2(2^63) - log2(cs->mult))
	 * max_cycles < 2^(63 - log2(cs->mult))
	 * max_cycles < 1 << (63 - log2(cs->mult))
	 * Please note that we add 1 to the result of the log2 to account for
	 * any rounding errors, ensure the above inequality is satisfied and
	 * no overflow will occur.
	 */
	max_cycles = 1ULL << (63 - (ilog2(cs->mult) + 1));

	/*
	 * The actual maximum number of cycles we can defer the clocksource is
	 * determined by the minimum of max_cycles and cs->mask.
	 */
	max_cycles = min_t(u64, max_cycles, (u64) cs->mask);
	max_nsecs = clocksource_cyc2ns(max_cycles, cs->mult, cs->shift);

	/*
	 * To ensure that the clocksource does not wrap whilst we are idle,
	 * limit the time the clocksource can be deferred by 12.5%. Please
	 * note a margin of 12.5% is used because this can be computed with
	 * a shift, versus say 10% which would require division.
	 */
	return max_nsecs - (max_nsecs >> 5);
}
#endif

static inline void update_sysclock(u32 cgu_sys)
{
    int i;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    u64 temp;
#endif

    switch ( *IFX_CGU_SYS & (0x0f << 4) )
    {
        case (0 << 4): i = CLOCK_600M; break;
        case (1 << 4): i = CLOCK_500M; break;
        case (2 << 4): i = CLOCK_393M; break; //cgu_get_pll1_fosc();
        case (3 << 4): i = CLOCK_333M; break;
        case (4 << 4): i = CLOCK_125M; break;
        case (5 << 4): i = CLOCK_125M; break;
        case (6 << 4): i = CLOCK_196_608M; break;
        case (7 << 4): i = CLOCK_166M; break;
        case (8 << 4): i = CLOCK_125M; break;
        case (9 << 4): i = CLOCK_125M; break;
        default:       i = CLOCK_393M;
    }

    if ( i )
    {
        mips_hpt_frequency = (i + 1) / 2; /*half of the CPU frequency*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        cycles_per_jiffy = (i + HZ) / (HZ * 2);
#endif

        /* Calclate a somewhat reasonable rating value */
            clocksource_mips.rating = 200 + mips_hpt_frequency / 10000000;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
            /* Find a shift value */
            for ( i = 32; i > 0; i-- )
            {
                    temp = (u64)NSEC_PER_SEC << i;
                    do_div(temp, mips_hpt_frequency);
                    if ( (temp >> 32) == 0 )
                            break;
            }
        clocksource_mips.shift = i;
        clocksource_mips.mult = (u32)temp;

        clocksource_mips.error = 0;
        clocksource_mips.xtime_nsec = 0;
        clocksource_calculate_interval(&clocksource_mips, tick_nsec);
#else
//        clocksource_set_clock(&clocksource_mips, mips_hpt_frequency);
//        clocksource_mips.max_idle_ns = clocksource_max_deferment(&clocksource_mips);
#endif
    }
}
#if 0 
static void dump_tc(int t)
{
        unsigned long val;

        settc(t);
        printk(KERN_DEBUG "VPE loader: TC index %d targtc %ld "
               "TCStatus 0x%lx halt 0x%lx\n",
               t, read_c0_vpecontrol() & VPECONTROL_TARGTC,
               read_tc_c0_tcstatus(), read_tc_c0_tchalt());

        printk(KERN_DEBUG " tcrestart 0x%lx\n", read_tc_c0_tcrestart());
        printk(KERN_DEBUG " tcbind 0x%lx\n", read_tc_c0_tcbind());

        val = read_c0_vpeconf0();
        printk(KERN_DEBUG " VPEConf0 0x%lx MVP %ld\n", val,
               (val & VPECONF0_MVP) >> VPECONF0_MVP_SHIFT);

        printk(KERN_DEBUG " c0 status 0x%lx\n", read_vpe_c0_status());
        printk(KERN_DEBUG " c0 cause 0x%lx\n", read_vpe_c0_cause());

        printk(KERN_DEBUG " c0 badvaddr 0x%lx\n", read_vpe_c0_badvaddr());
        printk(KERN_DEBUG " c0 epc 0x%lx\n", read_vpe_c0_epc());
}
void cgu_dump_vpe(int t)
{
        settc(t);

        printk(KERN_DEBUG "VPEControl 0x%lx\n", read_vpe_c0_vpecontrol());
        printk(KERN_DEBUG "VPEConf0 0x%lx\n", read_vpe_c0_vpeconf0());

        dump_tc(t);
}

#endif

//extern void ifx_fill_icache(void *addr, u32 size);
extern void ifx_fill_lock_icache(void *addr, u32 size);
extern void ifx_unlock_icache(void *addr, u32 size);

u32 cgu_set_clock(u32 cpu_clk, u32 ddr_clk, u32 fpi_clk)
{
    void *start, *end;
    unsigned long sys_flag;
    unsigned long dmt_flag;
    unsigned int vpe_flag;
    int i;
    u32 cgu_sys = 0;
    u32 cgu_sys_templ[] = {
	500, 200, 0x13,
	500, 250, 0x12,
	333, 167, 0x32,
    393, 196, 0x22,
    125, 125, 0x80,
    };
#if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
    u32 count_start, count_end;
#endif 

    for ( i = 0; i < sizeof(cgu_sys_templ) / sizeof(*cgu_sys_templ); i += 3 )
        if ( cpu_clk == cgu_sys_templ[i + 0] && ddr_clk == cgu_sys_templ[i + 1]
            /* && (cgu_sys_templ[i + 2] & 0x18) == (*IFX_CGU_SYS & 0x18) */ )
        {
            cgu_sys = cgu_sys_templ[i + 2];
            break;
        }
    if ( i >= sizeof(cgu_sys_templ) / sizeof(*cgu_sys_templ) )
        return -EINVAL;

    /* test purpose only -> time measurement */
//  *(volatile u32 *)0xbe100b70 ^= 0x8000;   /* GPIO_47 pin toggle */

    local_irq_save(sys_flag);

    dmt_flag = dmt();
    vpe_flag = dvpe();

    /* Put MVPE's into 'configuration state' */
    set_c0_mvpcontrol(MVPCONTROL_VPC);

    settc(1);
    if ((read_tc_c0_tcstatus() & TCSTATUS_A) || !(read_tc_c0_tchalt() & TCHALT_H)) {
    	write_tc_c0_tchalt(TCHALT_H);
    	instruction_hazard();
    }

    /* take system out of configuration state */
    clear_c0_mvpcontrol(MVPCONTROL_VPC);

 #if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
    g_extra_counter_start = g_extra_counter_end = 0;
    count_start = read_c0_count();
 #endif
//    ehb();

    *IFX_CGU_SYS = cgu_sys;

        /* Cache the reset code of this function */
        __asm__ __volatile__ (
                "       .set    push                            \n"
                "       .set    mips3                           \n"
                "       la      %0,startpoint                   \n"
                "       la      %1,endpoint                     \n"
                "       .set    pop                             \n"
                : "=r" (start), "=r" (end)
                :
        );

//	printk("start = %p, end = %p", start, end);
    memcpy((u8*)argv, (u8*)MBX_BASEADDRESS, MPS_MEM_SEG_DATASIZE);
    memcpy((u8*)MBX_BASEADDRESS, (u8*)start, (end - start));

    /* save cgu_sys value to MPS memory for later use */
    *(volatile u32 *)MBX_BASEADDRESS = cgu_sys;

    /* jump to MPS memory; skip first 4 byte because of cgu_sys value */
    __asm__("jr     %0"::"r"(MBX_BASEADDRESS+4));

//        for (iptr = (void *)((unsigned int)start & ~(L1_CACHE_BYTES - 1));
//             iptr < end; iptr += L1_CACHE_BYTES)
//                cache_op(Fill, iptr);

    __asm__ __volatile__ (
            "startpoint:                                    \n"
    );

//    *IFX_CGU_SYS = cgu_sys;
//    __sync();

    BARRIER;
    *(volatile u32 *)0xbf401070 |= 0x1;   //Put DDR into self Refresh mode
    BARRIER;
    
    /* wait until DDR controller acknowledge self refresh */
    while ( (*(volatile u32 *)0xbf400400 & 0x2) == 0 ); // check CKE bit

    /*  Stop the DDR SDRAM controller by writing a 0 to the START parameter. */
    *(volatile u32 *)0xbf401070 &= ~(1<<8);


#if 0 
    __asm__ __volatile__ (
                ".set push\n\t"
//                ".set noreorder\n\t"
                ".set mips32r2\n\t"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop\n\t"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop\n\t"
                "la   $8, 0xbf103020\n\t" /* IFX_CGU_UPDATE */
                "sd   %0, 0($8)\n\t"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop\n\t"
                "nop; nop; nop; nop; nop; nop; nop; nop; nop\n\t"
                ".set pop"
                : /* output */
                : /* input */    "r"(0x01)
                : /* modifies */ "$8" );

#else

    BARRIER;
    *IFX_CGU_UPDATE = 0x01;
    BARRIER;
#endif

    /* this wait is absolutely necessary, otherwise system becomes instable !
       CGU_SYS is split into a master register and a shadow register.
       write -> master-register; read <- shadow-register.
       Takeover from master into shadow triggered by IFX_CGU_UPDATE.
       Therefore we have to wait until cgu_sys value is transfered into shadow register.
    */
//    while ( *IFX_CGU_SYS != cgu_sys );
    while ( *IFX_CGU_SYS != *(volatile u32 *)MBX_BASEADDRESS );

    /*  restart the DDR SDRAM controller by writing a 1 to the START parameter. 
        This forces the DLL to lock to the new frequency. */
    *(volatile u32 *)0xbf401070 |= 0x100;

 #if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
        g_extra_counter_start = read_c0_count();
 #endif
    while ( (*(volatile u32 *)0xbf4012f0 & 0x1) == 0 );    //check for DLL relock
    while ( (*(volatile u32 *)0xbf401300 & 0x1) == 0 );    //check for DLL relock

 #if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
        g_extra_counter_end = read_c0_count();
 #endif
    *(volatile u32 *)0xbf401070 &= ~(1<<0); //Put DDR out of self Refresh mode
//    *(volatile u32 *)0xbf401060 |= 0x1;     //Power-up Selfrefresh exit

    __asm__("jr     %0"::"r"( end + 16 ));

    __asm__ __volatile__ (
            "endpoint:                                      \n"
    );

 #if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
    count_end = read_c0_count();
 #endif

    BARRIER;
    memcpy((u8*)MBX_BASEADDRESS, (u8*)argv, MPS_MEM_SEG_DATASIZE);

    /* Put MVPE's into 'configuration state' */
    set_c0_mvpcontrol(MVPCONTROL_VPC);

    settc(1);

    if ((read_tc_c0_tcstatus() & TCSTATUS_A) || !(read_tc_c0_tchalt() & TCHALT_H)) {
//	printk("TCSTATUS_A = 0x%p\n", (read_tc_c0_tcstatus() & TCSTATUS_A));
    	write_tc_c0_tchalt(read_tc_c0_tchalt() & ~TCHALT_H);
    	instruction_hazard();
    }

    /* take system out of configuration state */
    clear_c0_mvpcontrol(MVPCONTROL_VPC);

    evpe(vpe_flag);
    emt(dmt_flag);

    /* test purpose only -> time measurement */
//  *(volatile u32 *)0xbe100b70 ^= 0x8000;   /* GPIO_47 pin toggle */
    local_irq_restore(sys_flag);

    ifx_update_asc_clock_settings();

    update_sysclock(cgu_sys);

//    local_irq_restore(sys_flag);

 #if defined(ENABLE_SET_CLOCK_PROFILING) && ENABLE_SET_CLOCK_PROFILING
    printk("Succeeded in switching to CPU %d, DDR %d, FPI %d, clock cycle %u. extra cycle %u\n",
        cpu_clk, ddr_clk, fpi_clk, count_end - count_start, g_extra_counter_end - g_extra_counter_start);
 #else
    printk("Succeeded in switching to CPU %d, DDR %d, FPI %d\n", cpu_clk, ddr_clk, fpi_clk);
 #endif

    return 0;
}
#endif //CONFIG_IFX_CLOCK_CHANGE

static inline int ifx_cgu_version(char *buf)
{
    return ifx_drv_ver(buf, "CGU", IFX_CGU_VER_MAJOR, IFX_CGU_VER_MID, IFX_CGU_VER_MINOR);
}

static int __init cgu_init(void)
{
    int ret;
    char ver_str[128] = {0};

    ret = register_chrdev(IFX_CGU_MAJOR, "ifx_cgu", &cgu_fops);
    if ( ret != 0 ) {
        printk(KERN_ERR "Can not register CGU device - %d\n", ret);
        return ret;
    }

#ifdef CONFIG_PROC_FS
    proc_file_create();
#endif
    
    /* malloc MPS backup mem */
    argv = kmalloc(MPS_MEM_SEG_DATASIZE, GFP_KERNEL);

    ifx_cgu_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    /* test purpose only -> time measurement */
//  *(volatile u32 *)0xbe100b7c &= ~0x8000;   // GPIO_47 alternate sel0
//  *(volatile u32 *)0xbe100b80 &= ~0x8000;   // GPIO_47 alternate sel1
//  *(volatile u32 *)0xbe100b8c |= 0x8000;    // GPIO_47 pull up
//  *(volatile u32 *)0xbe100b90 |= 0x8000;    // GPIO_47 pull up enable
//  *(volatile u32 *)0xbe100b78 |= 0x8000;    // GPIO_47 direction
//
    return IFX_SUCCESS;
}

static void __exit cgu_exit(void)
{

#ifdef CONFIG_PROC_FS
    proc_file_delete();
#endif

    /* free MPS backup mem */
    kfree(argv);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    if ( unregister_chrdev(IFX_CGU_MAJOR, "ifx_cgu") ) {
        printk(KERN_ERR "Can not unregister CGU device.");
        return;
    }
#else
    unregister_chrdev(IFX_CGU_MAJOR, "ifx_cgu");
#endif
}

/*!
  \fn       unsigned int ifx_get_cpu_hz(void)
  \brief    Get CPU speed
  \return   CPU clock hz
  \ingroup  IFX_CGU_API
 */
unsigned int ifx_get_cpu_hz(void)
{
#ifdef CONFIG_USE_EMULATOR
    return EMULATOR_CPU_SPEED;
#else
    switch ( *IFX_CGU_SYS & (0x0f << 4) )
    {
        case (0 << 4): return CLOCK_600M;
        case (1 << 4): return CLOCK_500M;
        case (2 << 4): return CLOCK_393M; //cgu_get_pll1_fosc();
        case (3 << 4): return CLOCK_333M;
        case (4 << 4): return CLOCK_125M;
        case (5 << 4): return CLOCK_125M;
        case (6 << 4): return CLOCK_196_608M;
        case (7 << 4): return CLOCK_166M;
        case (8 << 4): return CLOCK_125M;
        case (9 << 4): return CLOCK_125M;
        default:       return CLOCK_393M;
    }
#endif
}
EXPORT_SYMBOL(ifx_get_cpu_hz);

/*!
  \fn       unsigned int ifx_get_fpi_hz(void)
  \brief    Get FPI bus speed
  \return   FPI bus clock hz
  \ingroup  IFX_CGU_API
 */
unsigned int ifx_get_fpi_hz(void)
{
#ifdef CONFIG_USE_EMULATOR
    return EMULATOR_CPU_SPEED >> 1;
#else
    switch (CGU_IF_CLK_FPI_CLK)
    {
        case (0):  return CLOCK_62_5M;
        case (1):  return CLOCK_62_5M;
        case (2):  return CLOCK_83_5M;
        case (3):  return CLOCK_125M;
        case (4):  return CLOCK_125M;
        case (5):  return CLOCK_125M;
        case (6):  return CLOCK_167M;
        case (7):  return CLOCK_200M;
        case (8):  return CLOCK_250M;
        case (9):  return CLOCK_300M;
        case (10): return CLOCK_62_5M;
        case (11): return CLOCK_98_304M;
        case (12): return CLOCK_150M;
        case (13): return CLOCK_196_608M;
        default:   return CLOCK_98_304M;
    }
#endif
}
EXPORT_SYMBOL(ifx_get_fpi_hz);

/*!
  \fn       unsigned int ifx_get_usif_hz(void)
  \brief    Get USIF module clock hz
  \return   USIF module clock hz
  \ingroup  IFX_CGU_API
 */
unsigned int ifx_get_usif_hz(void)
{
#ifdef CONFIG_USE_EMULATOR
    return (PLL0_CLK_SPEED / 10);
#else
    return (cgu_get_pll0_fosc() / 10);
#endif
}
EXPORT_SYMBOL(ifx_get_usif_hz);

EXPORT_SYMBOL(cgu_get_fpi_bus_clock);
EXPORT_SYMBOL(cgu_get_pp32_clock);
EXPORT_SYMBOL(cgu_get_pci_clock);
EXPORT_SYMBOL(cgu_get_ethernet_clock);
EXPORT_SYMBOL(cgu_get_usb_clock);
EXPORT_SYMBOL(cgu_get_clockout);
#ifdef CONFIG_IFX_CLOCK_CHANGE
  EXPORT_SYMBOL(cgu_set_clock);
#endif

module_init(cgu_init);
module_exit(cgu_exit);
