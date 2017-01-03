/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1998, 1999, 2003 by Ralf Baechle
 */
#ifndef _ASM_TIMEX_H
#define _ASM_TIMEX_H

#ifdef __KERNEL__

#include <asm/mipsregs.h>

/*
 * This is the clock rate of the i8253 PIT.  A MIPS system may not have
 * a PIT by the symbol is used all over the kernel including some APIs.
 * So keeping it defined to the number for the PIT is the only sane thing
 * for now.
 */
#define CLOCK_TICK_RATE 1193182

/*
 * Standard way to access the cycle counter.
 * Currently only used on SMP for scheduling.
 *
 * Only the low 32 bits are available as a continuously counting entity.
 * But this only means we'll force a reschedule every 8 seconds or so,
 * which isn't an evil thing.
 *
 * We know that all SMP capable CPUs have cycle counters.
 *
 ************************************************************************************
 * Patch by Mathieu Desnoyers
 ************************************************************************************
 * partly reverts commit efb9ca08b5a2374b29938cdcab417ce4feb14b54. Selects
 * HAVE_GET_CYCLES_32 only on CPUs where it is safe to use it.
 *
 * Currently consider the "_WORKAROUND" cases for 4000 and 4400 to be unsafe, but
 * should probably add other sub-architecture to the blacklist.
 *
 * Do not define HAVE_GET_CYCLES because MIPS does not provide 64-bit tsc (only
 * 32-bits).
 ***************
 *
 * Mathieu Desnoyers <mathieu.desnoyers@polymtl>
 * HAVE_GET_CYCLES makes sure that this case is handled properly :
 *
 * Ralf Baechle <ralf [at] linux-mips> :
 * This avoids us executing an mfc0 c0_count instruction on processors which
 * don't have but also on certain R4000 and R4400 versions where reading from
 * the count register just in the very moment when its value equals c0_compare
 * will result in the timer interrupt getting lost.
 *
 *   Reply:
 *      The usual workaround for this processor bug is to check if the value of
 *     the c0_count and c0_compare registers are close. Clone, not identical to
 *     allow for the time skew in the pipeline. If they are close, then
 *     execute the timer interrupt handler. See also the R4000/R4400 errata.
 *
 *     Ralf
 *
 *       Reply:
 *         Hrm, this sounds fragile. What happens if the instrumentation is added
 *         in a irq-disabled code path ? We would be executing the timer interrupt
 *         handler at this site synchronously when it should be deferred until the
 *         irq off section is done. That would therefore behave like there is an
 *         interrupt occuring in an irq off section.
 *
 *         OTOH, if we detect that get_cycles() is called with irqs off and decide
 *         not to execute the irq handler, then the handler that would be expected
 *         to be executed when the irq off section ends will simply be discarded
 *         and a timer interrupt will be lost.
 *
 *         Or is there some way to raise the interrupt line from the CPU so it
 *         behaves like a standard asynchronous interrupt ?
 *
 *         The other way around would be to add a check in local_irq_enable() to
 *         verify that such work is pending, but it might have a significant
 *         performance impact.
 *
 *         Mathieu
 *
 */

typedef unsigned int cycles_t;

#ifdef CONFIG_VR9
  #define HAVE_GET_CYCLES_32
#endif

#ifdef HAVE_GET_CYCLES_32

static inline cycles_t get_cycles(void)
{
	return read_c0_count();
}

static inline void get_cycles_barrier(void)
{
}

static inline cycles_t get_cycles_rate(void)
{
	return CLOCK_TICK_RATE;
}

#else  // not define HAVE_GET_CYCLES_32

static inline cycles_t get_cycles(void)
{
  #error Check if the CPU can not support CP0_counter
	return 0;
}
#endif


#endif /* __KERNEL__ */

#endif /*  _ASM_TIMEX_H */

/*
 * If we need to config HAVE_GET_CYCLES_32 from menuconfig, add this patch to Kconfig.
 *
 ************************************************************************************
 * Patch by Mathieu Desnoyers
 ************************************************************************************
 *--- linux.trees.git.orig/arch/mips/Kconfig	2008-11-07 00:06:06.000000000 -0500
 *+++ linux.trees.git/arch/mips/Kconfig	2008-11-07 00:10:10.000000000 -0500
 *@@ -1611,6 +1611,10 @@ config CPU_R4000_WORKAROUNDS
 * config CPU_R4400_WORKAROUNDS
 * 	bool
 *
 *+config HAVE_GET_CYCLES_32
 *+	def_bool y
 *+	depends on !CPU_R4400_WORKAROUNDS
 *+
 * #
 * # Use the generic interrupt handling code in kernel/irq/:
 * #
 *
 */
