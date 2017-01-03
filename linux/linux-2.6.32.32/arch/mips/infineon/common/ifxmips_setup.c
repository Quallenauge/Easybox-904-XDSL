/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * HISTORY
 * $Version $Date      $Author     $Comment
 * 2.0      07/05/09    Xu Liang
 * 1.0      19/07/07    Teh Kok How
 */



#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/version.h>

#include <asm/time.h>
#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>



extern void prom_printf(const char *, ...);
extern unsigned int ifx_get_cpu_hz(void);
extern void ifx_reboot_setup(void);
#ifdef CONFIG_KGDB
  extern int kgdb_serial_init(void);
#endif



#ifdef CONFIG_MIPS_APSP_KSPD
  unsigned long cpu_khz;
#endif



void __init bus_error_init(void)
{
    /* nothing */
}

static inline long get_counter_resolution(void)
{
#if defined(CONFIG_DANUBE) || defined(CONFIG_AR9) || defined(CONFIG_VR9)
    volatile long res;
    __asm__ __volatile__(
        ".set   push\n"
        ".set   noreorder\n"
        ".set   mips32r2\n"
        "rdhwr  %0, $3\n"
        "ehb\n"
        ".set pop\n"
     :  "=&r" (res)
     : /* no input */
     : "memory");
     instruction_hazard();
     return res;
#else
    return 2;
#endif
}

void __init plat_time_init(void)
{
    u32 resolution = get_counter_resolution();

    mips_hpt_frequency = ifx_get_cpu_hz() / resolution;

#ifdef CONFIG_USE_EMULATOR
    //  we do some hacking here to give illusion we have a faster counter
    //  frequency so that the time interrupt happends less frequently
    mips_hpt_frequency *= 25;
#endif

    prom_printf("mips_hpt_frequency = %d, counter_resolution = %d\n", mips_hpt_frequency, resolution);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    //  timer interrupt is wired to IP7
    //  no matter what value is read by read_c0_intctl()
    cp0_compare_irq = 7;
#endif

#ifdef CONFIG_MIPS_APSP_KSPD
    cpu_khz = ifx_get_cpu_hz() / 1000;
#endif
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
/*
 * THe CPU counter for System timer, set to HZ
 * GPTU Timer 6 for high resolution timer, set to hr_time_resolution
 * Also misuse this routine to print out the CPU type and clock.
 */
void __init plat_timer_setup(struct irqaction *irq)
{
    /* cpu counter for timer interrupts */
    setup_irq(MIPS_CPU_TIMER_IRQ, irq);
}
#else
unsigned int __cpuinit get_c0_compare_int(void)
{
  return MIPS_CPU_TIMER_IRQ;
}
#endif

void __init plat_mem_setup(void)
{
    ifx_reboot_setup();
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    board_time_init = plat_time_init;
#endif

#ifdef CONFIG_KGDB
    kgdb_serial_init();
    prom_printf("\n===>Please connect GDB to console tty0\n");
#endif
}

