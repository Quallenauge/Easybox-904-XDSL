/******************************************************************************
**
** FILE NAME    : ifxmips_gptu.c
** PROJECT      : IFX UEIP
** MODULES      : GPTU
**
** DATE         : 28 May 2009
** AUTHOR       : Huang Xiaogang
** DESCRIPTION  : IFX Cross-Platform General Purpose Timer Counter module driver
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
** 28 May 2009  Huang Xiaogang The first UEIP release
*******************************************************************************/
/*!
    \file ifxmips_gptu.c
    \ingroup IFX_GPTU
    \brief This file contains General Purpose Timer Counter driver
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/irq.h>
#include <asm/div64.h>
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/model.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gptu.h>
#include <asm/ifx/ifx_clk.h>

#ifdef CONFIG_IFX_PMCU
#include "ifxmips_gptu_pm.h"
#endif /* CONFIG_IFX_PMCU */

#define ifxmips_r32(reg)                        __raw_readl(reg)
#define ifxmips_w32(val, reg)                   __raw_writel(val, reg)
#define ifxmips_w32_mask(clear, set, reg)       ifxmips_w32((ifxmips_r32(reg) & ~clear) | set, reg)

#define MAX_NUM_OF_32BIT_TIMER_BLOCKS	6


#ifdef TIMER1A
#define FIRST_TIMER			TIMER1A
#else
#define FIRST_TIMER			2
#endif

/*
 *  GPTC divider is set or not.
 */
#define GPTU_CLC_RMC_IS_SET		0

/*
 * GPTC driver DEBUG on/off
 */
//#define GPTU_DEBUG

/*
 *  Timer Interrupt (IRQ)
 */
/*  Must be adjusted when ICU driver is available */
#if defined(CONFIG_AMAZON_SE)
#define TIMER_INTERRUPT                 (INT_NUM_IM2_IRL0 + 25)
#else
#define TIMER_INTERRUPT                 (INT_NUM_IM3_IRL0 + 22)
#endif

/*
 *  GPTU Register Mapping
 */
#define IFXMIPS_GPTU			(KSEG1 + 0x1E100A00)
#define IFXMIPS_GPTU_IRNEN		((volatile u32 *)(IFXMIPS_GPTU + 0x00F4))
#define IFXMIPS_GPTU_IRNICR		((volatile u32 *)(IFXMIPS_GPTU + 0x00F8))
#define IFXMIPS_GPTU_IRNCR		((volatile u32 *)(IFXMIPS_GPTU + 0x00FC))

/*
 *  Clock Control Register
 */
#define IFXMIPS_GPTU_CLC                ((volatile u32 *)(IFXMIPS_GPTU + 0x0000))

#define GPTU_CLC_SMC			GET_BITS(*IFXMIPS_GPTU_CLC, 23, 16)
#define GPTU_CLC_RMC			GET_BITS(*IFXMIPS_GPTU_CLC, 15, 8)
#define GPTU_CLC_FSOE			(*IFXMIPS_GPTU_CLC & (1 << 5))
#define GPTU_CLC_EDIS			(*IFXMIPS_GPTU_CLC & (1 << 3))
#define GPTU_CLC_SPEN			(*IFXMIPS_GPTU_CLC & (1 << 2))
#define GPTU_CLC_DISS			(*IFXMIPS_GPTU_CLC & (1 << 1))
#define GPTU_CLC_DISR			(*IFXMIPS_GPTU_CLC & (1 << 0))

#define GPTU_CLC_SMC_SET(value)		SET_BITS(0, 23, 16, (value))
#define GPTU_CLC_RMC_SET(value)		SET_BITS(0, 15, 8, (value))
#define GPTU_CLC_FSOE_SET(value)	((value) ? (1 << 5) : 0)
#define GPTU_CLC_SBWE_SET(value)	((value) ? (1 << 4) : 0)
#define GPTU_CLC_EDIS_SET(value)	((value) ? (1 << 3) : 0)
#define GPTU_CLC_SPEN_SET(value)	((value) ? (1 << 2) : 0)
#define GPTU_CLC_DISR_SET(value)	((value) ? (1 << 0) : 0)

/*
 *  ID Register
 */
#define IFXMIPS_GPTU_ID                 ((volatile u32 *)(IFXMIPS_GPTU + 0x0008))

#define GPTU_ID_ID			GET_BITS(*IFXMIPS_GPTU_ID, 15, 8)
#define GPTU_ID_CFG			GET_BITS(*IFXMIPS_GPTU_ID, 7, 5)
#define GPTU_ID_REV			GET_BITS(*IFXMIPS_GPTU_ID, 4, 0)

/*
 *  Control Register of Timer/Counter nX
 *    n is the index of block (1 based index)
 *    X is either A or B
 */
#define IFXMIPS_GPTU_CON(n, X)          ((volatile u32 *)(IFXMIPS_GPTU + 0x0010 + ((X) * 4) + ((n) - 1) * 0x0020))      /* X must be either A or B */
#define IFXMIPS_GPTU_RUN(n, X)          ((volatile u32 *)(IFXMIPS_GPTU + 0x0018 + ((X) * 4) + ((n) - 1) * 0x0020))      /* X must be either A or B */
#define IFXMIPS_GPTU_RELOAD(n, X)       ((volatile u32 *)(IFXMIPS_GPTU + 0x0020 + ((X) * 4) + ((n) - 1) * 0x0020))      /* X must be either A or B */
#define IFXMIPS_GPTU_COUNT(n, X)        ((volatile u32 *)(IFXMIPS_GPTU + 0x0028 + ((X) * 4) + ((n) - 1) * 0x0020))      /* X must be either A or B */

#define GPTU_CON_SRC_EG(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 10))
#define GPTU_CON_SRC_EXT(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 9))
#define GPTU_CON_SYNC(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 8))
#define GPTU_CON_EDGE(n, X)		GET_BITS(*IFXMIPS_GPTU_CON(n, X), 7, 6)
#define GPTU_CON_INV(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 5))
#define GPTU_CON_EXT(n, X)		(*IFXMIPS_GPTU_CON(n, A) & (1 << 4))	/* Timer/Counter B does not have this bit */
#define GPTU_CON_STP(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 3))
#define GPTU_CON_CNT(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 2))
#define GPTU_CON_DIR(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 1))
#define GPTU_CON_EN(n, X)		(*IFXMIPS_GPTU_CON(n, X) & (1 << 0))

#define GPTU_CON_SRC_EG_SET(value)	((value) ? 0 : (1 << 10))
#define GPTU_CON_SRC_EXT_SET(value)	((value) ? (1 << 9) : 0)
#define GPTU_CON_SYNC_SET(value)	((value) ? (1 << 8) : 0)
#define GPTU_CON_EDGE_SET(value)	SET_BITS(0, 7, 6, (value))
#define GPTU_CON_INV_SET(value)		((value) ? (1 << 5) : 0)
#define GPTU_CON_EXT_SET(value)		((value) ? (1 << 4) : 0)
#define GPTU_CON_STP_SET(value)		((value) ? (1 << 3) : 0)
#define GPTU_CON_CNT_SET(value)		((value) ? (1 << 2) : 0)
#define GPTU_CON_DIR_SET(value)		((value) ? (1 << 1) : 0)

#define GPTU_RUN_RL_SET(value)		((value) ? (1 << 2) : 0)
#define GPTU_RUN_CEN_SET(value)		((value) ? (1 << 1) : 0)
#define GPTU_RUN_SEN_SET(value)		((value) ? (1 << 0) : 0)

#define GPTU_IRNEN_TC_SET(n, X, value)	((value) ? (1 << (((n) - 1) * 2 + (X))) : 0)
#define GPTU_IRNCR_TC_SET(n, X, value)	((value) ? (1 << (((n) - 1) * 2 + (X))) : 0)

#define TIMER_FLAG_MASK_SIZE(x)		(x & 0x0001)
#define TIMER_FLAG_MASK_TYPE(x)		(x & 0x0002)
#define TIMER_FLAG_MASK_STOP(x)		(x & 0x0004)
#define TIMER_FLAG_MASK_DIR(x)		(x & 0x0008)
#define TIMER_FLAG_NONE_EDGE		0x0000
#define TIMER_FLAG_MASK_EDGE(x)		(x & 0x0030)
#define TIMER_FLAG_REAL			0x0000
#define TIMER_FLAG_INVERT		0x0040
#define TIMER_FLAG_MASK_INVERT(x)	(x & 0x0040)
#define TIMER_FLAG_MASK_TRIGGER(x)	(x & 0x0070)
#define TIMER_FLAG_MASK_SYNC(x)		(x & 0x0080)
#define TIMER_FLAG_CALLBACK_IN_HB	0x0200
#define TIMER_FLAG_MASK_HANDLE(x)	(x & 0x0300)
#define TIMER_FLAG_MASK_SRC(x)		(x & 0x1000)

struct timer_dev_timer {
	unsigned int f_irq_on;
	unsigned int irq;
	unsigned int flag;
	unsigned long arg1;
	unsigned long arg2;
};

struct timer_dev {
	struct mutex gptu_mutex;
	unsigned int number_of_timers;
	unsigned int occupation;
	unsigned int f_gptu_on;
	struct timer_dev_timer timer[MAX_NUM_OF_32BIT_TIMER_BLOCKS * 2];
};

static int ifx_gptu_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static int ifx_gptu_open(struct inode *, struct file *);
static int ifx_gptu_release(struct inode *, struct file *);

//extern unsigned int cgu_get_fpi_bus_clock(int fpi);

static struct file_operations gptu_fops = {
	.owner = THIS_MODULE,
	.ioctl = ifx_gptu_ioctl,
	.open = ifx_gptu_open,
	.release = ifx_gptu_release
};

static struct miscdevice gptu_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "gptu",
	.fops = &gptu_fops,
};

static struct timer_dev timer_dev;

/**
 * Handle timer interrupt
 */
static irqreturn_t timer_irq_handler(int irq, void *p)
{
	unsigned int timer;
	unsigned int flag;
	struct timer_dev_timer *dev_timer = (struct timer_dev_timer *)p;

	timer = irq - TIMER_INTERRUPT;
	if (timer < timer_dev.number_of_timers
		&& dev_timer == &timer_dev.timer[timer]) {
		/*  Clear interrupt.    */
		ifxmips_w32(1 << timer, IFXMIPS_GPTU_IRNCR);

		/*  Call user hanler or signal. */
		flag = dev_timer->flag;
		if (!(timer & 0x01)
			|| TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT) {
			/* 16-bit timer or timer A of 32-bit timer  */
			switch (TIMER_FLAG_MASK_HANDLE(flag)) {
			case TIMER_FLAG_CALLBACK_IN_IRQ:
			case TIMER_FLAG_CALLBACK_IN_HB:
				if (dev_timer->arg1)
					(*(timer_callback)dev_timer->arg1)(dev_timer->arg2);
				break;
			case TIMER_FLAG_SIGNAL:
				send_sig((int)dev_timer->arg2, (struct task_struct *)dev_timer->arg1, 0);
				break;
			}
		}
	}
	return IRQ_HANDLED;
}

static inline void ifx_gptu_enable(void)
{
    GPTC_PMU_SETUP(IFX_PMU_ENABLE);

	/*  Set divider as 1, disable write protection for SPEN, enable module. */
	*IFXMIPS_GPTU_CLC =
		GPTU_CLC_SMC_SET(0x00) |
		GPTU_CLC_RMC_SET(0x01) |
		GPTU_CLC_FSOE_SET(0) |
		GPTU_CLC_SBWE_SET(1) |
		GPTU_CLC_EDIS_SET(0) |
		GPTU_CLC_SPEN_SET(0) |
		GPTU_CLC_DISR_SET(0);
}

static inline void ifx_gptu_disable(void)
{
	ifxmips_w32(0x00, IFXMIPS_GPTU_IRNEN);
	ifxmips_w32(0xfff, IFXMIPS_GPTU_IRNCR);

	/*  Set divider as 0, enable write protection for SPEN, disable module. */
	*IFXMIPS_GPTU_CLC =
		GPTU_CLC_SMC_SET(0x00) |
		GPTU_CLC_RMC_SET(0x00) |
		GPTU_CLC_FSOE_SET(0) |
		GPTU_CLC_SBWE_SET(0) |
		GPTU_CLC_EDIS_SET(0) |
		GPTU_CLC_SPEN_SET(0) |
		GPTU_CLC_DISR_SET(1);

//	ifxmips_pmu_disable(IFXMIPS_PMU_PWDCR_GPT);
}

/*!
  \fn 		int ifx_gptu_timer_request((unsigned int timer, unsigned int flag,
        		unsigned long value, unsigned long arg1, unsigned long arg2)
  \brief 	allocate a proper timer for other module
  \param 	timer 	index of timer/counter, if zero, a timer will be allocated
 		   		automatically
  \param 	flag 	configuration flag
  \param 	value 	reload value
  \param 	arg1 	callback function's pointer, if any
  \param 	arg2 	parameters for callback function, if any
  \ingroup 	IFX_GPTU_API
 */
int ifx_gptu_timer_request(unsigned int timer, unsigned int flag,
	unsigned long value, unsigned long arg1, unsigned long arg2)
{
	int ret = 0;
	unsigned int con_reg, irnen_reg;
	int n, X;

	if (timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;
#ifdef GPTU_DEBUG
	printk(KERN_INFO "request_timer(%d, 0x%08X, %lu)...",
		timer, flag, value);
#endif
	if (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT)
		value &= 0xFFFF;
	else
		timer &= ~0x01;

	mutex_lock(&timer_dev.gptu_mutex);

	/*
	 *  Allocate timer.
	 */
	if (timer < FIRST_TIMER) {
		unsigned int mask;
		unsigned int shift;
		/* This takes care of TIMER1B which is the only choice for Voice TAPI system */
		unsigned int offset = TIMER2A;

		/*
		 *  Pick up a free timer.
		 */
		if (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT) {
			mask = 1 << offset;
			shift = 1;
		} else {
			mask = 3 << offset;
			shift = 2;
		}
		for (timer = offset;
		     timer < offset + timer_dev.number_of_timers;
		     timer += shift, mask <<= shift)
			if (!(timer_dev.occupation & mask)) {
				timer_dev.occupation |= mask;
				break;
			}
		if (timer >= offset + timer_dev.number_of_timers) {
#ifdef GPTU_DEBUG
			printk("failed![%d]\n", __LINE__);
#endif
			mutex_unlock(&timer_dev.gptu_mutex);
			return -EINVAL;
		} else
			ret = timer;
	} else {
		register unsigned int mask;

		/*
		 *  Check if the requested timer is free.
		 */
		mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
		if ((timer_dev.occupation & mask)) {
#ifdef GPTU_DEBUG
			printk("failed![%d] mask %#x, timer_dev.occupation %#x\n",
				__LINE__, mask, timer_dev.occupation);
#endif
			mutex_unlock(&timer_dev.gptu_mutex);
			return -EBUSY;
		} else {
			timer_dev.occupation |= mask;
			ret = 0;
		}
	}

	/*
	 *  Prepare control register value.
	 */
	switch (TIMER_FLAG_MASK_EDGE(flag)) {
	default:
	case TIMER_FLAG_NONE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x00);
		break;
	case TIMER_FLAG_RISE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x01);
		break;
	case TIMER_FLAG_FALL_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x02);
		break;
	case TIMER_FLAG_ANY_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x03);
		break;
	}
	if (TIMER_FLAG_MASK_TYPE(flag) == TIMER_FLAG_TIMER)
		con_reg |=
			TIMER_FLAG_MASK_SRC(flag) ==
			TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EXT_SET(1) :
			GPTU_CON_SRC_EXT_SET(0);
	else
		con_reg |=
			TIMER_FLAG_MASK_SRC(flag) ==
			TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EG_SET(1) :
			GPTU_CON_SRC_EG_SET(0);
	con_reg |=
		TIMER_FLAG_MASK_SYNC(flag) ==
		TIMER_FLAG_UNSYNC ? GPTU_CON_SYNC_SET(0) :
		GPTU_CON_SYNC_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_INVERT(flag) ==
		TIMER_FLAG_REAL ? GPTU_CON_INV_SET(0) : GPTU_CON_INV_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_SIZE(flag) ==
		TIMER_FLAG_16BIT ? GPTU_CON_EXT_SET(0) :
		GPTU_CON_EXT_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_STOP(flag) ==
		TIMER_FLAG_ONCE ? GPTU_CON_STP_SET(1) : GPTU_CON_STP_SET(0);
	con_reg |=
		TIMER_FLAG_MASK_TYPE(flag) ==
		TIMER_FLAG_TIMER ? GPTU_CON_CNT_SET(0) :
		GPTU_CON_CNT_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_DIR(flag) ==
		TIMER_FLAG_UP ? GPTU_CON_DIR_SET(1) : GPTU_CON_DIR_SET(0);

	/*
	 *  Fill up running data.
	 */
	timer_dev.timer[timer - FIRST_TIMER].flag = flag;
	timer_dev.timer[timer - FIRST_TIMER].arg1 = arg1;
	timer_dev.timer[timer - FIRST_TIMER].arg2 = arg2;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer_dev.timer[timer - FIRST_TIMER + 1].flag = flag;

	/*
	 *  Enable GPTU module.
	 */
	if (!timer_dev.f_gptu_on) {
		ifx_gptu_enable();
		timer_dev.f_gptu_on = 1;
	}

	/*
	 *  Enable IRQ.
	 */
	if (TIMER_FLAG_MASK_HANDLE(flag) != TIMER_FLAG_NO_HANDLE) {
		if (TIMER_FLAG_MASK_HANDLE(flag) == TIMER_FLAG_SIGNAL) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
			timer_dev.timer[timer - FIRST_TIMER].arg1 =
				(unsigned long) find_task_by_pid((int) arg1);
#else
			read_lock(&tasklist_lock);
			timer_dev.timer[timer - FIRST_TIMER].arg1 =
				(unsigned long) find_task_by_vpid((pid_t) arg1);
			read_unlock(&tasklist_lock);
#endif
		}

		irnen_reg = 1 << (timer - FIRST_TIMER);

		if (TIMER_FLAG_MASK_HANDLE(flag) == TIMER_FLAG_SIGNAL
		    || (TIMER_FLAG_MASK_HANDLE(flag) ==
			TIMER_FLAG_CALLBACK_IN_IRQ
			&& timer_dev.timer[timer - FIRST_TIMER].arg1)) {
			enable_irq(timer_dev.timer[timer - FIRST_TIMER].irq);
			timer_dev.timer[timer - FIRST_TIMER].f_irq_on = 1;
		}
	} else
		irnen_reg = 0;

	/*
	 *  Write config register, reload value and enable interrupt.
	 */
	n = timer >> 1;
	X = timer & 0x01;
	*IFXMIPS_GPTU_CON(n, X) = con_reg;
	*IFXMIPS_GPTU_RELOAD(n, X) = value;
	/* printk("reload value = %d\n", (u32)value); */
	*IFXMIPS_GPTU_IRNEN |= irnen_reg;

	mutex_unlock(&timer_dev.gptu_mutex);
#ifdef GPTU_DEBUG
	printk("successful!\n");
#endif
	return ret;
}
EXPORT_SYMBOL(ifx_gptu_timer_request);

/*!
  \fn 		int ifx_gptu_timer_free(unsigned int timer)
  \brief 	free timer
  \param 	timer 	index of timer/counter
  \ingroup  	IFX_GPTU_API
 */
int ifx_gptu_timer_free(unsigned int timer)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	if (GPTU_CON_EN(n, X))
		*IFXMIPS_GPTU_RUN(n, X) = GPTU_RUN_CEN_SET(1);

	*IFXMIPS_GPTU_IRNEN &= ~GPTU_IRNEN_TC_SET(n, X, 1);
	*IFXMIPS_GPTU_IRNCR |= GPTU_IRNCR_TC_SET(n, X, 1);

	if (timer_dev.timer[timer - FIRST_TIMER].f_irq_on) {
		disable_irq(timer_dev.timer[timer - FIRST_TIMER].irq);
		timer_dev.timer[timer - FIRST_TIMER].f_irq_on = 0;
	}

	timer_dev.occupation &= ~mask;
	if (!timer_dev.occupation && timer_dev.f_gptu_on) {
		ifx_gptu_disable();
		timer_dev.f_gptu_on = 0;
	}

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(ifx_gptu_timer_free);

/*!
  \fn 		int ifx_gptu_timer_start(unsigned int timer, int is_resume)
  \brief 	start one registered timer
  \param 	timer 		index of timer/counter
  \param 	is_resume 	0 indicates restart, non-zero resume
  \ingroup      IFX_GPTU_API
 */
int ifx_gptu_timer_start(unsigned int timer, int is_resume)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) ==
	TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	*IFXMIPS_GPTU_RUN(n, X) = GPTU_RUN_RL_SET(!is_resume) | GPTU_RUN_SEN_SET(1);

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(ifx_gptu_timer_start);

/*!
  \fn		int ifx_gptu_timer_stop(unsigned int timer)
  \brief 	stop one registered timer
  \param 	timer 	index of timer/counter
  \ingroup      IFX_GPTU_API
 */
int ifx_gptu_timer_stop(unsigned int timer)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER
	    || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	*IFXMIPS_GPTU_RUN(n, X) = GPTU_RUN_CEN_SET(1);

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(ifx_gptu_timer_stop);

/*!
  \fn		int ifx_gptu_counter_flags_reset(u32 timer, u32 flags)
  \brief	reset counter flags
  \param 	timer index of timer/counter
  \param 	flags configuration flag
  \ingroup      IFX_GPTU_API
 */
int ifx_gptu_counter_flags_reset(u32 timer, u32 flags)
{
	unsigned int oflag;
	unsigned int mask, con_reg;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	oflag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(oflag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(oflag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	switch (TIMER_FLAG_MASK_EDGE(flags)) {
	default:
	case TIMER_FLAG_NONE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x00);
		break;
	case TIMER_FLAG_RISE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x01);
		break;
	case TIMER_FLAG_FALL_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x02);
		break;
	case TIMER_FLAG_ANY_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x03);
		break;
	}
	if (TIMER_FLAG_MASK_TYPE(flags) == TIMER_FLAG_TIMER)
		con_reg |= TIMER_FLAG_MASK_SRC(flags) == TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EXT_SET(1) : GPTU_CON_SRC_EXT_SET(0);
	else
		con_reg |= TIMER_FLAG_MASK_SRC(flags) == TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EG_SET(1) : GPTU_CON_SRC_EG_SET(0);
	con_reg |= TIMER_FLAG_MASK_SYNC(flags) == TIMER_FLAG_UNSYNC ? GPTU_CON_SYNC_SET(0) : GPTU_CON_SYNC_SET(1);
	con_reg |= TIMER_FLAG_MASK_INVERT(flags) == TIMER_FLAG_REAL ? GPTU_CON_INV_SET(0) : GPTU_CON_INV_SET(1);
	con_reg |= TIMER_FLAG_MASK_SIZE(flags) == TIMER_FLAG_16BIT ? GPTU_CON_EXT_SET(0) : GPTU_CON_EXT_SET(1);
	con_reg |= TIMER_FLAG_MASK_STOP(flags) == TIMER_FLAG_ONCE ? GPTU_CON_STP_SET(1) : GPTU_CON_STP_SET(0);
	con_reg |= TIMER_FLAG_MASK_TYPE(flags) == TIMER_FLAG_TIMER ? GPTU_CON_CNT_SET(0) : GPTU_CON_CNT_SET(1);
	con_reg |= TIMER_FLAG_MASK_DIR(flags) == TIMER_FLAG_UP ? GPTU_CON_DIR_SET(1) : GPTU_CON_DIR_SET(0);

	timer_dev.timer[timer - FIRST_TIMER].flag = flags;
	if (TIMER_FLAG_MASK_SIZE(flags) != TIMER_FLAG_16BIT)
		timer_dev.timer[timer - FIRST_TIMER + 1].flag = flags;

	n = timer >> 1;
	X = timer & 0x01;

	*IFXMIPS_GPTU_CON(n, X) = con_reg;
	smp_wmb();
	printk(KERN_INFO "[%s]: counter%d oflags %#x, nflags %#x, GPTU_CON %#x\n", __func__, timer, oflag, flags, *IFXMIPS_GPTU_CON(n, X));
	mutex_unlock(&timer_dev.gptu_mutex);
	return 0;
}
EXPORT_SYMBOL(ifx_gptu_counter_flags_reset);

/*!
  \fn 	 	int ifx_gptu_countvalue_get(unsigned int timer, unsigned long *value)
  \brief 	get count value of particular assigned timer
  \param 	timer index of timer/counter
  \param	value: pointer of value
  \ingroup      IFX_GPTU_API
 */
int ifx_gptu_countvalue_get(unsigned int timer, unsigned long *value)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER
	    || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	*value = *IFXMIPS_GPTU_COUNT(n, X);

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(ifx_gptu_countvalue_get);

/*!
  \fn		u32 ifx_gptu_divider_cal(unsigned long freq)
  \brief 	get the count value of particular assigned timer
  \param	freq	expected frequency
  \ingroup      IFX_GPTU_API
 */
u32 ifx_gptu_divider_cal(unsigned long freq)
{
	u64 module_freq;
#if defined(CONFIG_DANUBE) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
	u64 fpi = cgu_get_fpi_bus_clock(2); //FIXME
#else
        u64 fpi = cgu_get_fpi_bus_clock();
#endif
	u32 clock_divider = 1;
	module_freq = fpi * 1000;
	do_div(module_freq, clock_divider * freq);
	return module_freq;
}
EXPORT_SYMBOL(ifx_gptu_divider_cal);

int ifx_gptu_timer_set(unsigned int timer, unsigned int freq, int is_cyclic,
	int is_ext_src, unsigned int handle_flag, unsigned long arg1,
	unsigned long arg2)
{
	unsigned long divider;
	unsigned int flag;

	divider = ifx_gptu_divider_cal(freq);
	if (divider == 0)
		return -EINVAL;
	flag = ((divider & ~0xFFFF) ? TIMER_FLAG_32BIT : TIMER_FLAG_16BIT)
		| (is_cyclic ? TIMER_FLAG_CYCLIC : TIMER_FLAG_ONCE)
		| (is_ext_src ? TIMER_FLAG_EXT_SRC : TIMER_FLAG_INT_SRC)
		| TIMER_FLAG_TIMER | TIMER_FLAG_DOWN
		| TIMER_FLAG_MASK_HANDLE(handle_flag);

	printk(KERN_INFO "ifx_gptu_timer_set(%d, %d), divider = %lu\n",
		timer, freq, divider);
	return ifx_gptu_timer_request(timer, flag, divider, arg1, arg2);
}
EXPORT_SYMBOL(ifx_gptu_timer_set);

int ifx_gptu_counter_set(unsigned int timer, unsigned int flag, u32 reload,
	unsigned long arg1, unsigned long arg2)
{
	printk(KERN_INFO "ifx_gptu_counter_set(%d, %#x, %d)\n", timer, flag, reload);
	return ifx_gptu_timer_request(timer, flag, reload, arg1, arg2);
}
EXPORT_SYMBOL(ifx_gptu_counter_set);

/*!
  \fn 		static int ifx_gptu_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
        		unsigned long arg)
  \brief 	standard operation to handle ioctl function call
  \param 	inode 	linux inode
  \param 	file 	linux file handler
  \param 	cmd 	ioctl command parameter
  \param 	arg 	structure gptu_ioctl_param
  \ingroup      IFX_GPTU_API
 */
static int ifx_gptu_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
	unsigned long arg)
{
	int ret;
	struct gptu_ioctl_param param;

	if (!access_ok(VERIFY_READ, arg, sizeof(struct gptu_ioctl_param)))
		return -EFAULT;

	copy_from_user(&param, (void *) arg, sizeof(param));

	if ((((cmd == IFX_GPTU_REQUEST_TIMER || cmd == IFX_GPTU_SET_TIMER
	       || cmd == IFX_GPTU_SET_COUNTER) && param.timer < 2)
	     || cmd == IFX_GPTU_GET_COUNT_VALUE || cmd == IFX_GPTU_CALCULATE_DIVIDER)
	    && !access_ok(VERIFY_WRITE, arg,
			   sizeof(struct gptu_ioctl_param)))
		return -EFAULT;

	switch (cmd) {
	case IFX_GPTU_REQUEST_TIMER:
		ret = ifx_gptu_timer_request(param.timer, param.flag, param.value,
				     (unsigned long) param.pid,
				     (unsigned long) param.sig);
		if (ret > 0) {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      timer, &ret, sizeof(&ret));
			ret = 0;
		}
		break;
	case IFX_GPTU_FREE_TIMER:
		ret = ifx_gptu_timer_free(param.timer);
		break;
	case IFX_GPTU_START_TIMER:
		ret = ifx_gptu_timer_start(param.timer, param.flag);
		break;
	case IFX_GPTU_STOP_TIMER:
		ret = ifx_gptu_timer_stop(param.timer);
		break;
	case IFX_GPTU_GET_COUNT_VALUE:
		ret = ifx_gptu_countvalue_get(param.timer, &param.value);
		if (!ret)
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      value, &param.value,
				      sizeof(param.value));
		break;
	case IFX_GPTU_CALCULATE_DIVIDER:
		param.value = ifx_gptu_divider_cal(param.value);
		if (param.value == 0)
			ret = -EINVAL;
		else {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      value, &param.value,
				      sizeof(param.value));
			ret = 0;
		}
		break;
	case IFX_GPTU_SET_TIMER:
		ret = ifx_gptu_timer_set(param.timer, param.value,
				 TIMER_FLAG_MASK_STOP(param.flag) !=
				 TIMER_FLAG_ONCE ? 1 : 0,
				 TIMER_FLAG_MASK_SRC(param.flag) ==
				 TIMER_FLAG_EXT_SRC ? 1 : 0,
				 TIMER_FLAG_MASK_HANDLE(param.flag) ==
				 TIMER_FLAG_SIGNAL ? TIMER_FLAG_SIGNAL :
				 TIMER_FLAG_NO_HANDLE,
				 (unsigned long) param.pid,
				 (unsigned long) param.sig);
		if (ret > 0) {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      timer, &ret, sizeof(&ret));
			ret = 0;
		}
		break;
	case IFX_GPTU_SET_COUNTER:
		ifx_gptu_counter_set(param.timer, param.flag, param.value, 0, 0);
		if (ret > 0) {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      timer, &ret, sizeof(&ret));
			ret = 0;
		}
		break;
        case IFX_GPTU_VERSION:
	{
            struct ifx_gptu_ioctl_version version = {
                .major = IFX_GPTU_VER_MAJOR,
                .mid   = IFX_GPTU_VER_MID,
                .minor = IFX_GPTU_VER_MINOR
            };
            ret = copy_to_user((void *)arg, (void *)&version, sizeof(version));
        	break;
	}
	default:
		ret = -ENOTTY;
	}
	return ret;
}

static int ifx_gptu_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ifx_gptu_release(struct inode *inode, struct file *file)
{
	return 0;
}

/*!
  \fn int __init ifx_gptu_init(void)
  \brief This function is called when the ifxmips_gptu driver is installed
  \param void
  \return     0 or -1
  \ingroup      IFX_GPTU_API
*/
int __init ifx_gptu_init(void)
{
	int ret;
	unsigned int i;

	ifxmips_w32(0, IFXMIPS_GPTU_IRNEN);
	ifxmips_w32(0xfff, IFXMIPS_GPTU_IRNCR);

	memset(&timer_dev, 0, sizeof(timer_dev));
	mutex_init(&timer_dev.gptu_mutex);

	ifx_gptu_enable();
	timer_dev.number_of_timers = /*GPTU_ID_CFG*/3 * 2;
	ifx_gptu_disable();
	if (timer_dev.number_of_timers > MAX_NUM_OF_32BIT_TIMER_BLOCKS * 2)
		timer_dev.number_of_timers = MAX_NUM_OF_32BIT_TIMER_BLOCKS * 2;
	printk(KERN_INFO "gptu: totally %d 16-bit timers/counters\n", timer_dev.number_of_timers);

	ret = misc_register(&gptu_miscdev);
	if (ret) {
		printk(KERN_ERR "gptu: can't misc_register, get error %d\n", -ret);
		return ret;
	} else {
		printk(KERN_INFO "gptu: misc_register on minor %d\n", gptu_miscdev.minor);
	}

	for (i = 0; i < timer_dev.number_of_timers; i++) {
		ret = request_irq(TIMER_INTERRUPT + i, timer_irq_handler, IRQF_TIMER, gptu_miscdev.name, &timer_dev.timer[i]);
		if (ret) {
			for (; i >= 0; i--)
				free_irq(TIMER_INTERRUPT + i, &timer_dev.timer[i]);
			misc_deregister(&gptu_miscdev);
			printk(KERN_ERR "gptu: failed in requesting irq (%d), get error %d\n", i, -ret);
			return ret;
		} else {
			timer_dev.timer[i].irq = TIMER_INTERRUPT + i;
			disable_irq(timer_dev.timer[i].irq);
			printk(KERN_INFO "gptu: succeeded to request irq %d\n", timer_dev.timer[i].irq);
		}
	}

#ifdef CONFIG_IFX_PMCU
//	ifx_gptu_pmcu_init(); debundle with pmcu module temporally
#endif /* CONFIG_IFX_PMCU */

	return 0;
}

/**
 * This function is called when the ifxmips_gptu driver is removed from kernel
 * with the rmmod command.
 *
 */
void __exit ifx_gptu_exit(void)
{
	unsigned int i;

	for (i = 0; i < timer_dev.number_of_timers; i++) {
		if (timer_dev.timer[i].f_irq_on)
			disable_irq(timer_dev.timer[i].irq);
		free_irq(timer_dev.timer[i].irq, &timer_dev.timer[i]);
	}
	ifx_gptu_disable();
	misc_deregister(&gptu_miscdev);
#ifdef CONFIG_IFX_PMCU
//	ifx_gptu_pmcu_exit(); debundle with pmcu module temporally
#endif /* CONFIG_IFX_PMCU */
}

module_init(ifx_gptu_init);
module_exit(ifx_gptu_exit);
