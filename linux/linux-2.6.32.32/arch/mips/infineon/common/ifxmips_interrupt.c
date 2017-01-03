/******************************************************************************
**
** FILE NAME    : ifxmips_interrupt.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : ICU driver source file
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
** $Date        $Author         $Comment
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



#include <linux/version.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/irq_cpu.h>
#include <asm/traps.h>
#ifdef CONFIG_KGDB
#  include <asm/gdb-stub.h>
#endif

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_si.h>
#include "ifxmips_interrupt.h"


#define IFX_ICU_VER_MAJOR               3
#define IFX_ICU_VER_MID                 0
#define IFX_ICU_VER_MINOR               1

#define MAX_IRQ_PER_IM                  32
/* Every IM has 32 irq pins , up to 5 IM from 0~4 */
#define IRQ2IM(irq)                     (((irq) >> 5) & 0x07)   //  irq / 32
#define IRQ2IMIRQ(irq)                  ((irq) & 0x1F)          //  irq % 32

#undef IFX_ICU_DEBUG
#ifdef IFX_ICU_DEBUG
#  define IFX_ICU_PRINT(fmt, args...)    printk(KERN_INFO "[%s %d]: " fmt, __func__, __LINE__, ##args)
#else
#  define IFX_ICU_PRINT(x...)
#endif


extern void prom_printf(const char * fmt, ...);


static u32 hardware_irq_count[NR_CPUS][6] = {{0}};

static volatile ifx_icu_t ifx_icu_p[] = {
    (volatile ifx_icu_t)IFX_ICU,
#ifdef CONFIG_SMP
    (volatile ifx_icu_t)IFX_ICU1
#endif
};

static volatile ifx_eiu_icu_t bsp_eiu_icu_p = (volatile ifx_eiu_icu_t)IFX_ICU_EIU;

static DEFINE_SPINLOCK(g_icu_lock);


//  to make sure this function is really "inline"ed
//  I use macro rather than inline function
#ifdef CONFIG_MIPS_MT_SMTC
#  define IFX_GET_CPU_ID()          cpu_data[smp_processor_id()].vpe_id
#elif defined(CONFIG_MIPS_MT_SMP)
#  define IFX_GET_CPU_ID()          smp_processor_id()
#else
#  define IFX_GET_CPU_ID()          0
#endif

inline int bsp_get_irq_isr(u32 irq)
{
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    return ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_isr & (1 << IRQ2IMIRQ(irq));
}
EXPORT_SYMBOL_GPL(bsp_get_irq_isr);

inline int bsp_get_irq_ier(u32 irq)
{
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    return ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_ier & (1 << IRQ2IMIRQ(irq));
}
EXPORT_SYMBOL_GPL(bsp_get_irq_ier);

inline void bsp_mask_irq(u32 irq)
{
    unsigned long sys_flags;
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    IFX_ICU_PRINT("irq: %d\n", irq);
    spin_lock_irqsave(&g_icu_lock, sys_flags);
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_ier &= ~(1 << IRQ2IMIRQ(irq));
    spin_unlock_irqrestore(&g_icu_lock, sys_flags);
}
EXPORT_SYMBOL_GPL(bsp_mask_irq);

inline void bsp_ack_irq(u32 irq)
{
    //  clearing IM_ISR is done in unmask (enable) irq as well as this function
    //  double clear of IM_ISR before IRQ unmasked does no harm
    //  for direct interrupt
    //  acknowledge to peripheral is done while IM_IOSR turning into 1
    //  it means before MIPS handle interrupt, acknowledge is done
    //  for indirect interrupt
    //  acknowledge is done by IRQ handle routine
    //  clear of IM_ISR is done just before leaving interrupt service routine

    unsigned long sys_flags;
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    IFX_ICU_PRINT("irq: %d\n", irq);
    spin_lock_irqsave(&g_icu_lock, sys_flags);
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_isr = 1 << IRQ2IMIRQ(irq);   //  clear IM status
    spin_unlock_irqrestore(&g_icu_lock, sys_flags);
}
EXPORT_SYMBOL_GPL(bsp_ack_irq);

/* irq_hwmask is used by the following irq on IFX platforms:
 * 0x8000 : MIPS_CPU_TIMER_IRQ
 * 0x4000 : MIPS_CPU_CPUCTR_IRQ
 * 0x0200 : MIPS_CPU_IPI_IRQ (smtc.c)
 * 0x0100 : MIPS_CPU_RTLX_IRQ (rtlx.c)
 */
void bsp_mask_and_ack_irq(u32 irq)
{
    bsp_mask_irq(irq);
    //  for both indirect & direct interrupt
    //  clear of IM_ISR is done in both ack and unmask (enable) irq
    //  double clear of IM_ISR before IRQ unmasked does no harm
    //  for direct interrupt
    //  acknowledge to peripheral is done while IM_IOSR turning into 1
    //  it means before MIPS handle interrupt, acknowledge is done
    //  for indirect interrupt
    //  acknowledge is done by IRQ handle routine
    //  clear of IM_ISR is done just before leaving interrupt service routine
    bsp_ack_irq(irq);
#ifdef CONFIG_MIPS_MT_SMTC
    if ( (irq_hwmask[irq] & ST0_IM) )
        set_c0_status(irq_hwmask[irq] & ST0_IM);
#endif /* CONFIG_MIPS_MT_SMTC */
}
EXPORT_SYMBOL_GPL(bsp_mask_and_ack_irq);

inline void bsp_enable_irq(u32 irq)
{
    unsigned long sys_flags;
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    spin_lock_irqsave(&g_icu_lock, sys_flags);
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_ier |= (1 << IRQ2IMIRQ(irq));
    spin_unlock_irqrestore(&g_icu_lock, sys_flags);

    irq_desc[irq].status &= ~IRQ_MASKED;
}
EXPORT_SYMBOL(bsp_enable_irq);

inline void bsp_disable_irq(u32 irq)
{
    BUG_ON(irq > INT_NUM_IM4_IRL31);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
  #define IRQ_DELAYED_DISABLE       0
#endif
    if ( !(irq_desc[irq].status & IRQ_DELAYED_DISABLE) ) {
        bsp_mask_irq(irq);
    }
}
EXPORT_SYMBOL(bsp_disable_irq);

inline void bsp_unmask_irq(u32 irq)
{
    unsigned long sys_flags;
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    IFX_ICU_PRINT("irq: %d\n", irq);
    spin_lock_irqsave(&g_icu_lock, sys_flags);
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_isr = 1 << IRQ2IMIRQ(irq);   //  clear IM status
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_ier |= 1 << IRQ2IMIRQ(irq);
    spin_unlock_irqrestore(&g_icu_lock, sys_flags);
}
EXPORT_SYMBOL_GPL(bsp_unmask_irq);

static void bsp_end_irq(u32 irq)
{
    BUG_ON(irq > INT_NUM_IM4_IRL31);
    if ( !(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)) )
        bsp_unmask_irq(irq);
}

static inline int bsp_retrigger_irq(u32 irq)
{
    unsigned long sys_flags;
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    IFX_ICU_PRINT("irq: %d\n", irq);
    spin_lock_irqsave(&g_icu_lock, sys_flags);
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_irsr |= 1 << IRQ2IMIRQ(irq);
    spin_unlock_irqrestore(&g_icu_lock, sys_flags);
    return 0;
}

/* The following two APIs for USIF/SSC to generate fake interrupt
 * It should be protected in the caller's context.
 */
void ifx_icu_irsr_set(u32 irq)
{
    bsp_retrigger_irq(irq);
}
EXPORT_SYMBOL(ifx_icu_irsr_set);

void ifx_icu_irsr_clr(u32 irq)
{
    unsigned long sys_flags;
    int cpu = IFX_GET_CPU_ID();

    BUG_ON(irq > INT_NUM_IM4_IRL31);
    spin_lock_irqsave(&g_icu_lock, sys_flags);
    //  it seems no effect to write ZERO to IRSR
    //  need double check
    ifx_icu_p[cpu]->im[IRQ2IM(irq)].im_irsr &= ~(1 << IRQ2IMIRQ(irq));
    spin_unlock_irqrestore(&g_icu_lock, sys_flags);
}
EXPORT_SYMBOL(ifx_icu_irsr_clr);

/* External Interrupt Controller */
static inline int ifx_irq_to_eiu(u32 irq)
{
    static u32 irq_tbl[] = {
        IFX_EIU_IR0,
        IFX_EIU_IR1,
        IFX_EIU_IR2,
#if defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
        IFX_EIU_IR3,
        IFX_EIU_IR4,
#endif
#if defined(CONFIG_AR9) || defined(CONFIG_VR9)
        IFX_EIU_IR5,
#endif
    };
    int i;

    for ( i = NUM_ENTITY(irq_tbl) - 1; i >= 0 && irq_tbl[i] != irq; i-- );
    return i;
}

static inline void bsp_eiu_enable_irq(u32 irq)
{
    unsigned long sys_flags;
    int i;

    IFX_ICU_PRINT("irq: %d\n", irq);
    if ( bsp_eiu_icu_p->eiu_inen == 0 && (i = ifx_irq_to_eiu(irq)) >= 0 ) {
        spin_lock_irqsave(&g_icu_lock, sys_flags);
        if ( bsp_eiu_icu_p->eiu_inen == 0 ) {
            if ( ifx_gpio_register(IFX_GPIO_MODULE_EXIN) != IFX_SUCCESS )
                printk(KERN_ERR "[%s %d]: failed in config GPIO pin for external interrupt detection unit\n", __func__, __LINE__);
        }
        bsp_eiu_icu_p->eiu_inen |= 1 << i;
        spin_unlock_irqrestore(&g_icu_lock, sys_flags);
    }

    bsp_enable_irq(irq);
}

static unsigned int bsp_eiu_startup_irq(u32 irq)
{
    bsp_eiu_enable_irq(irq);

    return 0;
}

static void bsp_eiu_shutdown_irq(u32 irq)
{
    unsigned long sys_flags;
    int i;

    bsp_disable_irq(irq);

    IFX_ICU_PRINT("irq: %d\n", irq);
    i = ifx_irq_to_eiu(irq);
    if ( i >= 0 ) {
        spin_lock_irqsave(&g_icu_lock, sys_flags);
        bsp_eiu_icu_p->eiu_inen &= ~(1 << i);
        if ( bsp_eiu_icu_p->eiu_inen == 0 )
            ifx_gpio_deregister(IFX_GPIO_MODULE_EXIN);
        spin_unlock_irqrestore(&g_icu_lock, sys_flags);
    }
}

static int bsp_eiu_settype(unsigned int irq, unsigned int flow_type)
{
    unsigned long sys_flags;
    int shift;
    int value = -1;

    shift = ifx_irq_to_eiu(irq);
    if ( shift < 0 ) {
        printk(KERN_ERR "[%s %d]: Invalid irq %d\n", __func__, __LINE__, irq);
        return -EINVAL;
    }
    else
        shift <<= 2;

    switch (flow_type) {
        case IRQF_TRIGGER_NONE:
            printk(KERN_ERR "[%s %d]: Assuming IRQ%d level triggered and EIU registers are already configured!\n", __func__, __LINE__, irq);
            set_irq_handler(irq, handle_level_irq);
            break;
        case IRQF_TRIGGER_RISING:
            value = 1;
            set_irq_handler(irq, handle_edge_irq);
            break;
        case IRQF_TRIGGER_FALLING:
            value = 2;
            set_irq_handler(irq, handle_edge_irq);
            break;
        case IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING:
            value = 3;
            set_irq_handler(irq, handle_edge_irq);
            break;
        case IRQF_TRIGGER_HIGH:
            value = 5;
            set_irq_handler(irq, handle_level_irq);
            break;
        case IRQF_TRIGGER_LOW:
            value = 6;
            set_irq_handler(irq, handle_level_irq);
            break;
        default:
            printk(KERN_ERR "[%s %d]: Invalid irq %d trigger type %d\n", __func__, __LINE__, irq, flow_type);
            value = 0;  //  although spec say that this value disable interrupt, this value works as edge trigger on board
            set_irq_handler(irq, handle_level_irq);
            break;
    }
    if ( value >= 0 ) {
        spin_lock_irqsave(&g_icu_lock, sys_flags);
        bsp_eiu_icu_p->eiu_exin_c = (bsp_eiu_icu_p->eiu_exin_c & ~(7 << shift)) | (value << shift);
        spin_unlock_irqrestore(&g_icu_lock, sys_flags);
    }

    return 0;
}

/* Serial In Interrupt */
static inline unsigned int ifx_irq_to_si(u32 irq)
{
    return irq - INT_NUM_SI_EIU_IR0;
}

static inline void bsp_si_mask_irq(u32 irq)
{
#ifdef CONFIG_IFX_SI
    BUG_ON(irq < INT_NUM_SI_EIU_IR0 || irq > INT_NUM_SI_EIU_IR15);
    IFX_ICU_PRINT("irq: %d\n", irq);
    ifx_si_irq_enable(ifx_irq_to_si(irq), 0);
    if ( ifx_si_irq_ier() == 0 )
        bsp_mask_irq(IFX_SI_EIU_IR);
#endif
}

static inline void bsp_si_ack_irq(u32 irq)
{
#ifdef CONFIG_IFX_SI
    BUG_ON(irq < INT_NUM_SI_EIU_IR0 || irq > INT_NUM_SI_EIU_IR15);
    bsp_ack_irq(IFX_SI_EIU_IR);
#endif
}

/* irq_hwmask is used by the following irq on IFX platforms:
 * 0x8000 : MIPS_CPU_TIMER_IRQ
 * 0x4000 : MIPS_CPU_CPUCTR_IRQ
 * 0x0200 : MIPS_CPU_IPI_IRQ (smtc.c)
 * 0x0100 : MIPS_CPU_RTLX_IRQ (rtlx.c)
 */
void bsp_si_mask_and_ack_irq(u32 irq)
{
    bsp_si_mask_irq(irq);
    //  for both indirect & direct interrupt
    //  clear of IM_ISR is done in both ack and unmask (enable) irq
    //  double clear of IM_ISR before IRQ unmasked does no harm
    //  for direct interrupt
    //  acknowledge to peripheral is done while IM_IOSR turning into 1
    //  it means before MIPS handle interrupt, acknowledge is done
    //  for indirect interrupt
    //  acknowledge is done by IRQ handle routine
    //  clear of IM_ISR is done just before leaving interrupt service routine
    bsp_si_ack_irq(irq);
#ifdef CONFIG_MIPS_MT_SMTC
    if ( (irq_hwmask[irq] & ST0_IM) )
        set_c0_status(irq_hwmask[irq] & ST0_IM);
#endif /* CONFIG_MIPS_MT_SMTC */
}

static inline void bsp_si_unmask_irq(u32 irq)
{
#ifdef CONFIG_IFX_SI
    unsigned int ier;

    BUG_ON(irq < INT_NUM_SI_EIU_IR0 || irq > INT_NUM_SI_EIU_IR15);
    IFX_ICU_PRINT("irq: %d\n", irq);
    ier = ifx_si_irq_ier();
    ifx_si_irq_enable(ifx_irq_to_si(irq), 1);
    if ( ier == 0 )
        bsp_unmask_irq(IFX_SI_EIU_IR);
#endif
}

static void bsp_si_end_irq(u32 irq)
{
    BUG_ON(irq < INT_NUM_SI_EIU_IR0 || irq > INT_NUM_SI_EIU_IR15);
    if ( !(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)) )
        bsp_si_unmask_irq(irq);
}

static int bsp_si_settype(unsigned int irq, unsigned int flow_type)
{
#ifdef CONFIG_IFX_SI
    int bit;

    bit = ifx_irq_to_si(irq);
    if ( bit >= 16 ) {
        printk(KERN_ERR "[%s %d]: Invalid irq %d\n", __func__, __LINE__, irq);
        return -EINVAL;
    }

    switch (flow_type) {
        case IRQF_TRIGGER_NONE:
            printk(KERN_ERR "[%s %d]: Assuming IRQ%d level triggered and EIU registers are already configured!\n", __func__, __LINE__, irq);
            set_irq_handler(irq, handle_level_irq);
            break;
        case IRQF_TRIGGER_HIGH:
            ifx_si_irq_active_high(bit, 1);
            set_irq_handler(irq, handle_level_irq);
            break;
        case IRQF_TRIGGER_LOW:
            ifx_si_irq_active_high(bit, 0);
            set_irq_handler(irq, handle_level_irq);
            break;
        default:
            printk(KERN_ERR "[%s %d]: Invalid irq %d trigger type %d\n", __func__, __LINE__, irq, flow_type);
            ifx_si_irq_active_high(bit, 1);
            set_irq_handler(irq, handle_level_irq);
            break;
    }

    return 0;
#else
    return -EIO;
#endif
}
/* Serial In Interrupt End */

/* Timer Interrupt Controller */
static inline void mask_mips_timer_irq(unsigned int irq)
{
#ifdef CONFIG_MIPS_MT_SMTC
    unsigned int vpflags = dvpe();
#endif
    clear_c0_status(STATUSF_IP7);
    irq_disable_hazard();
#ifdef CONFIG_MIPS_MT_SMTC
    evpe(vpflags);
#endif
}

static inline void unmask_mips_timer_irq(unsigned int irq)
{
#ifdef CONFIG_MIPS_MT_SMTC
    unsigned int vpflags = dvpe();
#endif
    set_c0_status(STATUSF_IP7);
    irq_enable_hazard();
#ifdef CONFIG_MIPS_MT_SMTC
    evpe(vpflags);
#endif
}

static inline void ack_mips_timer_irq(unsigned int irq)
{
    //  done in "c0_timer_ack"
}

static inline void mask_and_ack_mips_timer_irq(unsigned int irq)
{
#ifdef CONFIG_MIPS_MT_SMTC
    unsigned int vpflags = dvpe();
#endif
    clear_c0_status(STATUSF_IP7);
    irq_disable_hazard();
#ifdef CONFIG_MIPS_MT_SMTC
    if ( (irq_hwmask[irq] & ST0_IM) )
        set_c0_status(irq_hwmask[irq] & ST0_IM);
    evpe(vpflags);
#endif
}

static inline int retrigger_mips_timer_irq(u32 irq)
{
#ifdef CONFIG_MIPS_MT_SMTC
    unsigned int vpflags = dvpe();
#endif
    set_c0_cause(CAUSEF_IP7);
#ifdef CONFIG_MIPS_MT_SMTC
    evpe(vpflags);
#endif
    return 0;
}
/* End of Time Interrupt functions */

static struct irq_chip bsp_timer_irq_type = {
    .name     = "TIMER",
    .startup  = NULL,
    .shutdown = NULL,
    .enable   = NULL,
    .disable  = NULL,
    .ack      = ack_mips_timer_irq,
    .mask     = mask_mips_timer_irq,
    .mask_ack = mask_and_ack_mips_timer_irq,
    .unmask   = unmask_mips_timer_irq,
    .eoi      = unmask_mips_timer_irq,
    .end      = unmask_mips_timer_irq,
    .retrigger= retrigger_mips_timer_irq,
#ifdef CONFIG_MIPS_MT_SMTC_IRQAFF
    .set_affinity   = plat_set_irq_affinity,
#endif
};

static struct irq_chip bsp_irq_type = {
    .name       = "IFX_ICU",
    .startup    = NULL,
    .shutdown   = NULL,
    .enable     = bsp_enable_irq,
    .disable    = bsp_disable_irq,
    .ack        = bsp_ack_irq,
    .mask       = bsp_mask_irq,
    .mask_ack   = bsp_mask_and_ack_irq,
    .unmask     = bsp_unmask_irq,
    .eoi        = bsp_unmask_irq,
    .end        = bsp_end_irq,
    .retrigger  = bsp_retrigger_irq,
#ifdef CONFIG_MIPS_MT_SMTC_IRQAFF
    .set_affinity   = plat_set_irq_affinity,
#endif
};

static struct irq_chip bsp_eiu_irq_type = {
    .name       = "IFX_EIC",
    .set_type   = bsp_eiu_settype,
    .startup    = bsp_eiu_startup_irq,
    .shutdown   = bsp_eiu_shutdown_irq,
    .enable     = bsp_enable_irq,
    .disable    = bsp_disable_irq,
    .ack        = bsp_ack_irq,
    .mask       = bsp_mask_irq,
    .mask_ack   = bsp_mask_and_ack_irq,
    .unmask     = bsp_unmask_irq,
    .eoi        = bsp_unmask_irq,
    .end        = bsp_end_irq,
    .retrigger  = bsp_retrigger_irq,
#ifdef CONFIG_MIPS_MT_SMTC_IRQAFF
    .set_affinity   = plat_set_irq_affinity,
#endif /* CONFIG_MIPS_MT_SMTC_IRQAFF */
};

static struct irq_chip bsp_si_irq_type = {
    .name       = "IFX_SI",
    .set_type   = bsp_si_settype,
    .ack        = bsp_si_ack_irq,
    .mask       = bsp_si_mask_irq,
    .mask_ack   = bsp_si_mask_and_ack_irq,
    .unmask     = bsp_si_unmask_irq,
    .eoi        = bsp_si_unmask_irq,
    .end        = bsp_si_end_irq,
#ifdef CONFIG_MIPS_MT_SMTC_IRQAFF
    .set_affinity   = plat_set_irq_affinity,
#endif /* CONFIG_MIPS_MT_SMTC_IRQAFF */
};

static irqreturn_t perf_irq_handle(int irq, void *dev_id);
static struct irqaction perfirq = {
    .handler = perf_irq_handle,
    .flags   = IRQF_PERCPU,
    .name    = "perf_ctr",
    .dev_id   = "perf_ctr",
};
extern void enable_perf_counter(void);
extern void disable_perf_counter(void);
extern void clear_perf_counter(void);

static inline void ifx_enable_perf_counter(void)
{
#ifdef CONFIG_PERFORMANCE_COUNTER
    if (!cpu_wait)
        enable_perf_counter();
#endif
}


#ifdef CONFIG_IFX_SI
  #define IFX_DO_IRQ_DISPATCH(__irq)  do                    \
  {                                                         \
    if ( __irq == IFX_SI_EIU_IR ) {                         \
        si_eiu_irqdispatch();                               \
    } else                                                  \
        do_IRQ((int)__irq);                                 \
  } while ( 0 )
#else
  #define IFX_DO_IRQ_DISPATCH(__irq)  do                    \
  {                                                         \
    do_IRQ((int)__irq);                                     \
  } while ( 0 )
#endif

//  to make sure this function is really "inline"ed
//  I use macro rather than inline function
#define IFX_HW_IRQDISPATCH(im)      do                      \
{                                                           \
    u32 __irq;                                              \
    int __cpu = IFX_GET_CPU_ID();                           \
                                                            \
    ifx_enable_perf_counter();                              \
                                                            \
    __irq = (ifx_icu_p[__cpu]->ivec >> ((im) * 6)) & 0x3F;  \
                                                            \
    if ( !__irq )                                           \
        break;                                              \
    BUG_ON(__irq > 32);                                     \
                                                            \
    __irq = ((im) << 5) + __irq - 1;                        \
                                                            \
    if ( __irq == IFX_EBU_IR ) {                            \
        /* clear EBU interrupt */                           \
        *IFX_EBU_PCC_ISTAT |= 0x10;                         \
    }                                                       \
                                                            \
    IFX_ICU_PRINT("irq: %d\n", __irq);                      \
    hardware_irq_count[__cpu][im]++;                        \
    IFX_DO_IRQ_DISPATCH(__irq);                             \
} while ( 0 )

#ifdef CONFIG_IFX_SI
/* Cascaded interrupts from serial input */
static inline void si_eiu_irqdispatch(void)
{
    int bit = clz(ifx_si_irq_isr());

    if ( bit >= 0 )
        do_IRQ(INT_NUM_SI_EIU_IR0 + bit);
}
#endif

/* Cascaded interrupts from IM0 */
static void hw0_irqdispatch(void)
{
    IFX_HW_IRQDISPATCH(0);
}

/* Cascaded interrupts from IM1 */
static void hw1_irqdispatch(void)
{
    IFX_HW_IRQDISPATCH(1);
}

/* Cascaded interrupts from IM2 */
static void hw2_irqdispatch(void)
{
    IFX_HW_IRQDISPATCH(2);
}

/* Cascaded interrupts from IM3 */
static void hw3_irqdispatch(void)
{
    IFX_HW_IRQDISPATCH(3);
}

/* Cascaded interrupts from IM4 */
static void hw4_irqdispatch(void)
{
    IFX_HW_IRQDISPATCH(4);
}

#ifdef CONFIG_PERFORMANCE_COUNTER
extern unsigned long last_c0_count;
extern unsigned long instructions_per_second;  /* cpu instructions completed per second */
extern unsigned long perf_cpu_utilization_count;
extern unsigned long perf_cycles, delta_c0_count;
extern unsigned long long perf_cpu_utilization, _perf_cpu_utilization;  /* cpu utilization in promille */
#endif

/* Cascaded interrupts from IM5 */
static void hw5_irqdispatch (void)
{
    int cpu = IFX_GET_CPU_ID();
#ifdef CONFIG_PERFORMANCE_COUNTER
    unsigned long c0_count;

    if ( !cpu_wait )
        enable_perf_counter();        /* we are working -> enable performance counter */
    /* With CPU Freq @ 333 MHz: */
    /* c0_count is incremented with cpu_clk/get_counter_resolution(), i.e. each 6 ns */
    /* perf_cycle is incremented with cpu_clk, i.e. each 3 ns */
    /* each second, we calculate the cpu utilization */
    if ( ++perf_cpu_utilization_count >= HZ ) {
        c0_count = read_c0_count();
        perf_cycles = read_c0_perfcntr0();  /* cycles */
        instructions_per_second = read_c0_perfcntr1(); /* instructions completed */
        clear_perf_counter();

        delta_c0_count = c0_count - last_c0_count;
        last_c0_count = c0_count;
    }
#endif
    hardware_irq_count[cpu][5]++;
    do_IRQ(MIPS_CPU_TIMER_IRQ);
}

#ifdef CONFIG_MIPS_MT_SMTC_IRQAFF
/* IRQ affinity hook */
void plat_set_irq_affinity(unsigned int irq, cpumask_t affinity)
{
     cpumask_t tmask = affinity;
     int cpu = 0;
     void smtc_set_irq_affinity(unsigned int irq, cpumask_t aff);

    /*
     * On the legacy Malta development board, all I/O interrupts
     * are routed through the 8259 and combined in a single signal
     * to the CPU daughterboard, and on the CoreFPGA2/3 34K models,
     * that signal is brought to IP2 of both VPEs. To avoid racing
     * concurrent interrupt service events, IP2 is enabled only on
     * one VPE, by convention VPE0.  So long as no bits are ever
     * cleared in the affinity mask, there will never be any
     * interrupt forwarding.  But as soon as a program or operator
     * sets affinity for one of the related IRQs, we need to make
     * sure that we don't ever try to forward across the VPE boundry,
     * at least not until we engineer a system where the interrupt
     * _ack() or _end() function can somehow know that it corresponds
     * to an interrupt taken on another VPE, and perform the appropriate
     * restoration of Status.IM state using MFTR/MTTR instead of the
     * normal local behavior. We also ensure that no attempt will
     * be made to forward to an offline "CPU".
     */

     for_each_cpu_mask(cpu, affinity) {
        if ( (cpu_data[cpu].vpe_id != 0) || !cpu_online(cpu) ) {
            cpu_clear(cpu, tmask);
        }
     }
     irq_desc[irq].affinity = tmask;

     if ( cpus_empty(tmask) ) {
         /*
          * We could restore a default mask here, but the
          * runtime code can anyway deal with the null set
          */
          printk(KERN_WARNING  "IRQ affinity leaves no legal CPU for IRQ %d\n", irq);
      }
     /* Do any generic SMTC IRQ affinity setup */
     smtc_set_irq_affinity(irq, tmask);
}
#endif /* CONFIG_MIPS_MT_SMTC_IRQAFF */

extern int (*perf_irq)(void);

static irqreturn_t perf_irq_handle(int irq, void *dev_id)
{
    return perf_irq();
}

#ifdef CONFIG_IFX_PCI
extern int pci_bus_error_flag;
#endif

int bus_error_handler(struct pt_regs *regs, int is_fixup)
{
#ifdef CONFIG_IFX_PCI
    if ( pci_bus_error_flag ) {
        return MIPS_BE_DISCARD;
    }
#endif
    printk(KERN_INFO "Bus Error!\n");
    if (is_fixup) {
        return MIPS_BE_FIXUP;
    }
    else {
        return MIPS_BE_FATAL;
    }
}

/*
 *  Version of ffs that only looks at bits 12..15.
 */
static inline unsigned int irq_ffs(unsigned int pending)
{
#if defined(CONFIG_CPU_MIPS32) || defined(CONFIG_CPU_MIPS64)
    return clz(pending) - CAUSEB_IP;
#else
    unsigned int a0 = 7;
    unsigned int t0;

    t0 = pending & 0xf000;
    t0 = t0 < 1;
    t0 = t0 << 2;
    a0 = a0 - t0;
    pending = pending << t0;

    t0 = pending & 0xc000;
    t0 = t0 < 1;
    t0 = t0 << 1;
    a0 = a0 - t0;
    pending = pending << t0;

    t0 = pending & 0x8000;
    t0 = t0 < 1;
    //t0 = t0 << 2;
    a0 = a0 - t0;
    //pending = pending << t0;

    return a0;
#endif
}

/*
 * IRQs on the Malta board look basically (barring software IRQs which we
 * don't use at all and all external interrupt sources are combined together
 * on hardware interrupt 0 (MIPS IRQ 2)) like:
 *
 *      MIPS IRQ        Source
 *      --------        ------
 *             0        Software0
 *             1        Software1
 *             2        Hardware0
 *             3        Hardware1
 *             4        Hardware2
 *             5        Hardware3
 *             6        Hardware4
 *             7        R4k timer(what we use)
 *
 * We handle the IRQ according to _our_ priority which is:
 *
 * Highest ----     R4k Timer
 * Lowest  ----     Combined hardware interrupt
 *
 * then we just return, if multiple IRQs are pending then we will just take
 * another exception, big deal.
 */
asmlinkage void plat_irq_dispatch(void)
{
    unsigned int pending = read_c0_cause() & read_c0_status() & ST0_IM;
    int irq;

    if ( !pending )
        return;

    irq = irq_ffs(pending);
    prom_printf("[%s %d]: irq %d\n", __func__, __LINE__, irq);
    switch (irq) {
#ifdef CONFIG_SMP
        case 0:
        case 1:
            do_IRQ(MIPSCPU_INT_BASE + irq); //  SMP/SMTC only
            break;
#endif
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            IFX_HW_IRQDISPATCH(irq - 2);
            break;
        case 7:
            hw5_irqdispatch();
            break;
        default:
            spurious_interrupt();
            break;
        }
}

static void print_interrupt_vector(u32 irq)
{
#ifdef IFX_ICU_DEBUG
    u32 ebase = read_c0_ebase();
    extern char except_vec_vi, except_vec_vi_lui;
    extern char except_vec_vi_ori;
    volatile u32 *w, handler;
    const int lui_offset = &except_vec_vi_lui - &except_vec_vi;
    const int ori_offset = &except_vec_vi_ori - &except_vec_vi;

    w = (u32 *)((u8*)(ebase + 0x200 + irq*0x200) + lui_offset);
    handler = *w & 0xFFFF;
    handler <<= 16;
    w = (u32 *)((u8*)(ebase + 0x200 + irq*0x200) + ori_offset);
    handler |= (*w & 0xFFFF);
    printk(KERN_INFO "%#x: %#x: %#x\n", ebase + 0x200 + irq*0x200, *(u32*)(ebase + 0x200 + irq*0x200), handler);
#endif /* IFX_ICU_DEBUG */
}

/* Main Platform Interrupt Controller Initialization function */
void __init arch_init_irq(void)
{
    int i, j;
    char ver_str[128] = {0};

    IFX_ICU_PRINT("\n");

    board_be_handler = &bus_error_handler;

#ifdef CONFIG_MIPS_MT
    if ( !cpu_has_veic ) {
  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        mips_cpu_irq_init(MIPSCPU_INT_BASE);
  #else
        mips_cpu_irq_init();
  #endif
    }
#endif
    /* mask all interrupt sources */
    for ( i = 0; i < NUM_ENTITY(ifx_icu_p); i++ ) {
        for ( j = 0; j < ICU_IM_NUM; j++ ) {
            ifx_icu_p[i]->im[j].im_ier = 0;

            //  clear pending interrupts
            ifx_icu_p[i]->im[j].im_isr = ~0;
        }
    }

   /* Setting up Vectored Interrupt Table
    * Vectored Interrupt Mode is used when the following conditions are met:
    * CONFIG3[VInt] = 1
    * CONFIG3[Veic] = 0
    * INTCTL[VS]   != 0
    * CAUSE[IV]     = 1
    * STATUS[BEV]   = 0
    * A vector number is generated by the interrupt control logic. This number is combined with IntCtlVS to create
    * interrupt offset, which is added to 0x200 to create exception vector offset.
    * Vector Number 0 and 1 = Software interrupt 0 and 1
    * Vector Number 2 to 7  = Hardware interrupt 0 to 5
    */
    /* Now safe to set the exception vector. */
    if ( cpu_has_vint ) {
        IFX_ICU_PRINT("Installing  interrupt vectors...\n");
        set_vi_handler(2, hw0_irqdispatch);
        set_vi_handler(3, hw1_irqdispatch);
        set_vi_handler(4, hw2_irqdispatch);
        set_vi_handler(5, hw3_irqdispatch);
        set_vi_handler(6, hw4_irqdispatch);
        set_vi_handler(7, hw5_irqdispatch);
    }
//#ifdef CONFIG_SMP
//    printk(KERN_INFO "SRSConf0 %#x: SRS1: %#x, SRS2 %#x, SRS3 %#x\n", read_c0_srsconf0(), SRSCONF0_SRS1, SRSCONF0_SRS2, SRSCONF0_SRS3);
//#endif
    IFX_ICU_PRINT("CONFIG  = %#08x\n", read_c0_config());
    IFX_ICU_PRINT("CONFIG1 = %#08x\n", read_c0_config1());
    IFX_ICU_PRINT("CONFIG2 = %#08x\n", read_c0_config2());
    IFX_ICU_PRINT("CONFIG3 = %#08x\n", read_c0_config3());
    IFX_ICU_PRINT("STATUS  = %#08x\n", read_c0_status());
    IFX_ICU_PRINT("CAUSE   = %#08x\n", read_c0_cause());
    IFX_ICU_PRINT("INTCTL  = %#08x\n", read_c0_intctl());
    IFX_ICU_PRINT("SRSCTL  = %#08x\n", read_c0_srsctl());
    IFX_ICU_PRINT("SRSMAP  = %#08x\n", read_c0_srsmap());
    IFX_ICU_PRINT("EBASE   = %#08x\n", read_c0_ebase());

    for ( i = INT_NUM_IRQ0; i <= INT_NUM_IM4_IRL31; i++ ) {
        /* External interrupt */
        if ( i == IFX_EIU_IR0 || i == IFX_EIU_IR1 || i == IFX_EIU_IR2 )
            set_irq_chip_and_handler(i, &bsp_eiu_irq_type, handle_level_irq);
#if defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
        else if ( i == IFX_EIU_IR3 || i == IFX_EIU_IR4 )
            set_irq_chip_and_handler(i, &bsp_eiu_irq_type, handle_level_irq);
#endif
#if defined(CONFIG_AR9) || defined(CONFIG_VR9)
        else if ( i == IFX_EIU_IR5 )
            set_irq_chip_and_handler(i, &bsp_eiu_irq_type, handle_level_irq);
#endif
        else
            set_irq_chip_and_handler(i, &bsp_irq_type, handle_level_irq);
    }
    for ( i = INT_NUM_SI_EIU_IR0; i <= INT_NUM_SI_EIU_IR15; i++ ) {
        set_irq_chip_and_handler(i, &bsp_si_irq_type, handle_level_irq);
    }
    /* This entry is for timer interrupt */
    /* /proc/irq/ needs irq_desc[].handler to create an entry for every interrupt */
#ifdef CONFIG_SMP
    set_irq_chip_and_handler(MIPS_CPU_TIMER_IRQ, &bsp_timer_irq_type, handle_percpu_irq);
#else
    set_irq_chip_and_handler(MIPS_CPU_TIMER_IRQ, &bsp_timer_irq_type, handle_level_irq);
#endif
#if 0
    /* Mask IE of Performance Counter Control registers first.
     * Applications should enable it if it needs to handle the counter overflow interrupt.
     * Pecostat: IE
     */
    write_c0_perfctrl0(read_c0_perfctrl0() & ~CEB_IE);
    write_c0_perfctrl1(read_c0_perfctrl1() & ~CEB_IE);
    write_c0_perfctrl2(read_c0_perfctrl2() & ~CEB_IE);
    write_c0_perfctrl3(read_c0_perfctrl3() & ~CEB_IE);
#endif
#ifdef CONFIG_MIPS_MT_SMTC
    setup_irq_smtc(MIPS_CPU_CPUCTR_IRQ, &perfirq, 0x4000);
#else
    setup_irq(MIPS_CPU_CPUCTR_IRQ, &perfirq);
#endif
#if !defined(CONFIG_MIPS_MT_SMP) && !defined(CONFIG_MIPS_MT_SMTC)
    set_c0_status(IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
#else
    set_c0_status(IE_SW0 | IE_SW1 | IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5);
#endif

#ifdef CONFIG_KGDB
    set_debug_traps();
    breakpoint();
#endif

    ifx_drv_ver(ver_str, "ICU", IFX_ICU_VER_MAJOR, IFX_ICU_VER_MID, IFX_ICU_VER_MINOR);
    printk(KERN_INFO "%s", ver_str);

    for ( i = 0; i < 8; i++ ) {
        print_interrupt_vector(i);
    }
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32) && defined(CONFIG_MIPS_MT_SMP)
  int __cpuinitdata gic_present = 0;
#endif


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xu Liang");
MODULE_DESCRIPTION ("IFX CPE Interrupt Controller Driver");
MODULE_SUPPORTED_DEVICE ("IFX CPE ICU module");
