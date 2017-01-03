/******************************************************************************
**
** FILE NAME    : irq.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : common header file
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



#ifndef IFX_IRQ_H
#define IFX_IRQ_H


#include <linux/version.h>
#include <linux/types.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
  #define MIPS_CPU_IRQ_BASE             MIPSCPU_INT_BASE
#endif

/* these vectors are to handle the interrupts from the internal DANUBE
 * interrupt controller. THe INT_NUM values are really just indices into
 * an array and are set up so that we can use the INT_NUM as a shift
 * to calculate a mask value.
 */
#define INT_NUM_IRQ0                    0x00
#define INT_NUM_IM0_IRL0                (INT_NUM_IRQ0 + 0)
#define INT_NUM_IM0_IRL1                (INT_NUM_IRQ0 + 1)
#define INT_NUM_IM0_IRL2                (INT_NUM_IRQ0 + 2)
#define INT_NUM_IM0_IRL3                (INT_NUM_IRQ0 + 3)
#define INT_NUM_IM0_IRL4                (INT_NUM_IRQ0 + 4)
#define INT_NUM_IM0_IRL5                (INT_NUM_IRQ0 + 5)
#define INT_NUM_IM0_IRL6                (INT_NUM_IRQ0 + 6)
#define INT_NUM_IM0_IRL7                (INT_NUM_IRQ0 + 7)
#define INT_NUM_IM0_IRL8                (INT_NUM_IRQ0 + 8)
#define INT_NUM_IM0_IRL9                (INT_NUM_IRQ0 + 9)
#define INT_NUM_IM0_IRL10               (INT_NUM_IRQ0 + 10)
#define INT_NUM_IM0_IRL11               (INT_NUM_IRQ0 + 11)
#define INT_NUM_IM0_IRL12               (INT_NUM_IRQ0 + 12)
#define INT_NUM_IM0_IRL13               (INT_NUM_IRQ0 + 13)
#define INT_NUM_IM0_IRL14               (INT_NUM_IRQ0 + 14)
#define INT_NUM_IM0_IRL15               (INT_NUM_IRQ0 + 15)
#define INT_NUM_IM0_IRL16               (INT_NUM_IRQ0 + 16)
#define INT_NUM_IM0_IRL17               (INT_NUM_IRQ0 + 17)
#define INT_NUM_IM0_IRL18               (INT_NUM_IRQ0 + 18)
#define INT_NUM_IM0_IRL19               (INT_NUM_IRQ0 + 19)
#define INT_NUM_IM0_IRL20               (INT_NUM_IRQ0 + 20)
#define INT_NUM_IM0_IRL21               (INT_NUM_IRQ0 + 21)
#define INT_NUM_IM0_IRL22               (INT_NUM_IRQ0 + 22)
#define INT_NUM_IM0_IRL23               (INT_NUM_IRQ0 + 23)
#define INT_NUM_IM0_IRL24               (INT_NUM_IRQ0 + 24)
#define INT_NUM_IM0_IRL25               (INT_NUM_IRQ0 + 25)
#define INT_NUM_IM0_IRL26               (INT_NUM_IRQ0 + 26)
#define INT_NUM_IM0_IRL27               (INT_NUM_IRQ0 + 27)
#define INT_NUM_IM0_IRL28               (INT_NUM_IRQ0 + 28)
#define INT_NUM_IM0_IRL29               (INT_NUM_IRQ0 + 29)
#define INT_NUM_IM0_IRL30               (INT_NUM_IRQ0 + 30)
#define INT_NUM_IM0_IRL31               (INT_NUM_IRQ0 + 31)

#define INT_NUM_IM1_IRL0                (INT_NUM_IRQ0 + 32)
#define INT_NUM_IM1_IRL1                (INT_NUM_IM1_IRL0 + 1)
#define INT_NUM_IM1_IRL2                (INT_NUM_IM1_IRL0 + 2)
#define INT_NUM_IM1_IRL3                (INT_NUM_IM1_IRL0 + 3)
#define INT_NUM_IM1_IRL4                (INT_NUM_IM1_IRL0 + 4)
#define INT_NUM_IM1_IRL5                (INT_NUM_IM1_IRL0 + 5)
#define INT_NUM_IM1_IRL6                (INT_NUM_IM1_IRL0 + 6)
#define INT_NUM_IM1_IRL7                (INT_NUM_IM1_IRL0 + 7)
#define INT_NUM_IM1_IRL8                (INT_NUM_IM1_IRL0 + 8)
#define INT_NUM_IM1_IRL9                (INT_NUM_IM1_IRL0 + 9)
#define INT_NUM_IM1_IRL10               (INT_NUM_IM1_IRL0 + 10)
#define INT_NUM_IM1_IRL11               (INT_NUM_IM1_IRL0 + 11)
#define INT_NUM_IM1_IRL12               (INT_NUM_IM1_IRL0 + 12)
#define INT_NUM_IM1_IRL13               (INT_NUM_IM1_IRL0 + 13)
#define INT_NUM_IM1_IRL14               (INT_NUM_IM1_IRL0 + 14)
#define INT_NUM_IM1_IRL15               (INT_NUM_IM1_IRL0 + 15)
#define INT_NUM_IM1_IRL16               (INT_NUM_IM1_IRL0 + 16)
#define INT_NUM_IM1_IRL17               (INT_NUM_IM1_IRL0 + 17)
#define INT_NUM_IM1_IRL18               (INT_NUM_IM1_IRL0 + 18)
#define INT_NUM_IM1_IRL19               (INT_NUM_IM1_IRL0 + 19)
#define INT_NUM_IM1_IRL20               (INT_NUM_IM1_IRL0 + 20)
#define INT_NUM_IM1_IRL21               (INT_NUM_IM1_IRL0 + 21)
#define INT_NUM_IM1_IRL22               (INT_NUM_IM1_IRL0 + 22)
#define INT_NUM_IM1_IRL23               (INT_NUM_IM1_IRL0 + 23)
#define INT_NUM_IM1_IRL24               (INT_NUM_IM1_IRL0 + 24)
#define INT_NUM_IM1_IRL25               (INT_NUM_IM1_IRL0 + 25)
#define INT_NUM_IM1_IRL26               (INT_NUM_IM1_IRL0 + 26)
#define INT_NUM_IM1_IRL27               (INT_NUM_IM1_IRL0 + 27)
#define INT_NUM_IM1_IRL28               (INT_NUM_IM1_IRL0 + 28)
#define INT_NUM_IM1_IRL29               (INT_NUM_IM1_IRL0 + 29)
#define INT_NUM_IM1_IRL30               (INT_NUM_IM1_IRL0 + 30)
#define INT_NUM_IM1_IRL31               (INT_NUM_IM1_IRL0 + 31)

#define INT_NUM_IM2_IRL0                (INT_NUM_IRQ0 + 64)
#define INT_NUM_IM2_IRL1                (INT_NUM_IM2_IRL0 + 1)
#define INT_NUM_IM2_IRL2                (INT_NUM_IM2_IRL0 + 2)
#define INT_NUM_IM2_IRL3                (INT_NUM_IM2_IRL0 + 3)
#define INT_NUM_IM2_IRL4                (INT_NUM_IM2_IRL0 + 4)
#define INT_NUM_IM2_IRL5                (INT_NUM_IM2_IRL0 + 5)
#define INT_NUM_IM2_IRL6                (INT_NUM_IM2_IRL0 + 6)
#define INT_NUM_IM2_IRL7                (INT_NUM_IM2_IRL0 + 7)
#define INT_NUM_IM2_IRL8                (INT_NUM_IM2_IRL0 + 8)
#define INT_NUM_IM2_IRL9                (INT_NUM_IM2_IRL0 + 9)
#define INT_NUM_IM2_IRL10               (INT_NUM_IM2_IRL0 + 10)
#define INT_NUM_IM2_IRL11               (INT_NUM_IM2_IRL0 + 11)
#define INT_NUM_IM2_IRL12               (INT_NUM_IM2_IRL0 + 12)
#define INT_NUM_IM2_IRL13               (INT_NUM_IM2_IRL0 + 13)
#define INT_NUM_IM2_IRL14               (INT_NUM_IM2_IRL0 + 14)
#define INT_NUM_IM2_IRL15               (INT_NUM_IM2_IRL0 + 15)
#define INT_NUM_IM2_IRL16               (INT_NUM_IM2_IRL0 + 16)
#define INT_NUM_IM2_IRL17               (INT_NUM_IM2_IRL0 + 17)
#define INT_NUM_IM2_IRL18               (INT_NUM_IM2_IRL0 + 18)
#define INT_NUM_IM2_IRL19               (INT_NUM_IM2_IRL0 + 19)
#define INT_NUM_IM2_IRL20               (INT_NUM_IM2_IRL0 + 20)
#define INT_NUM_IM2_IRL21               (INT_NUM_IM2_IRL0 + 21)
#define INT_NUM_IM2_IRL22               (INT_NUM_IM2_IRL0 + 22)
#define INT_NUM_IM2_IRL23               (INT_NUM_IM2_IRL0 + 23)
#define INT_NUM_IM2_IRL24               (INT_NUM_IM2_IRL0 + 24)
#define INT_NUM_IM2_IRL25               (INT_NUM_IM2_IRL0 + 25)
#define INT_NUM_IM2_IRL26               (INT_NUM_IM2_IRL0 + 26)
#define INT_NUM_IM2_IRL27               (INT_NUM_IM2_IRL0 + 27)
#define INT_NUM_IM2_IRL28               (INT_NUM_IM2_IRL0 + 28)
#define INT_NUM_IM2_IRL29               (INT_NUM_IM2_IRL0 + 29)
#define INT_NUM_IM2_IRL30               (INT_NUM_IM2_IRL0 + 30)
#define INT_NUM_IM2_IRL31               (INT_NUM_IM2_IRL0 + 31)

#define INT_NUM_IM3_IRL0                (INT_NUM_IRQ0 + 96)
#define INT_NUM_IM3_IRL1                (INT_NUM_IM3_IRL0 + 1)
#define INT_NUM_IM3_IRL2                (INT_NUM_IM3_IRL0 + 2)
#define INT_NUM_IM3_IRL3                (INT_NUM_IM3_IRL0 + 3)
#define INT_NUM_IM3_IRL4                (INT_NUM_IM3_IRL0 + 4)
#define INT_NUM_IM3_IRL5                (INT_NUM_IM3_IRL0 + 5)
#define INT_NUM_IM3_IRL6                (INT_NUM_IM3_IRL0 + 6)
#define INT_NUM_IM3_IRL7                (INT_NUM_IM3_IRL0 + 7)
#define INT_NUM_IM3_IRL8                (INT_NUM_IM3_IRL0 + 8)
#define INT_NUM_IM3_IRL9                (INT_NUM_IM3_IRL0 + 9)
#define INT_NUM_IM3_IRL10               (INT_NUM_IM3_IRL0 + 10)
#define INT_NUM_IM3_IRL11               (INT_NUM_IM3_IRL0 + 11)
#define INT_NUM_IM3_IRL12               (INT_NUM_IM3_IRL0 + 12)
#define INT_NUM_IM3_IRL13               (INT_NUM_IM3_IRL0 + 13)
#define INT_NUM_IM3_IRL14               (INT_NUM_IM3_IRL0 + 14)
#define INT_NUM_IM3_IRL15               (INT_NUM_IM3_IRL0 + 15)
#define INT_NUM_IM3_IRL16               (INT_NUM_IM3_IRL0 + 16)
#define INT_NUM_IM3_IRL17               (INT_NUM_IM3_IRL0 + 17)
#define INT_NUM_IM3_IRL18               (INT_NUM_IM3_IRL0 + 18)
#define INT_NUM_IM3_IRL19               (INT_NUM_IM3_IRL0 + 19)
#define INT_NUM_IM3_IRL20               (INT_NUM_IM3_IRL0 + 20)
#define INT_NUM_IM3_IRL21               (INT_NUM_IM3_IRL0 + 21)
#define INT_NUM_IM3_IRL22               (INT_NUM_IM3_IRL0 + 22)
#define INT_NUM_IM3_IRL23               (INT_NUM_IM3_IRL0 + 23)
#define INT_NUM_IM3_IRL24               (INT_NUM_IM3_IRL0 + 24)
#define INT_NUM_IM3_IRL25               (INT_NUM_IM3_IRL0 + 25)
#define INT_NUM_IM3_IRL26               (INT_NUM_IM3_IRL0 + 26)
#define INT_NUM_IM3_IRL27               (INT_NUM_IM3_IRL0 + 27)
#define INT_NUM_IM3_IRL28               (INT_NUM_IM3_IRL0 + 28)
#define INT_NUM_IM3_IRL29               (INT_NUM_IM3_IRL0 + 29)
#define INT_NUM_IM3_IRL30               (INT_NUM_IM3_IRL0 + 30)
#define INT_NUM_IM3_IRL31               (INT_NUM_IM3_IRL0 + 31)

#define INT_NUM_IM4_IRL0                (INT_NUM_IRQ0 + 128)
#define INT_NUM_IM4_IRL1                (INT_NUM_IM4_IRL0 + 1)
#define INT_NUM_IM4_IRL2                (INT_NUM_IM4_IRL0 + 2)
#define INT_NUM_IM4_IRL3                (INT_NUM_IM4_IRL0 + 3)
#define INT_NUM_IM4_IRL4                (INT_NUM_IM4_IRL0 + 4)
#define INT_NUM_IM4_IRL5                (INT_NUM_IM4_IRL0 + 5)
#define INT_NUM_IM4_IRL6                (INT_NUM_IM4_IRL0 + 6)
#define INT_NUM_IM4_IRL7                (INT_NUM_IM4_IRL0 + 7)
#define INT_NUM_IM4_IRL8                (INT_NUM_IM4_IRL0 + 8)
#define INT_NUM_IM4_IRL9                (INT_NUM_IM4_IRL0 + 9)
#define INT_NUM_IM4_IRL10               (INT_NUM_IM4_IRL0 + 10)
#define INT_NUM_IM4_IRL11               (INT_NUM_IM4_IRL0 + 11)
#define INT_NUM_IM4_IRL12               (INT_NUM_IM4_IRL0 + 12)
#define INT_NUM_IM4_IRL13               (INT_NUM_IM4_IRL0 + 13)
#define INT_NUM_IM4_IRL14               (INT_NUM_IM4_IRL0 + 14)
#define INT_NUM_IM4_IRL15               (INT_NUM_IM4_IRL0 + 15)
#define INT_NUM_IM4_IRL16               (INT_NUM_IM4_IRL0 + 16)
#define INT_NUM_IM4_IRL17               (INT_NUM_IM4_IRL0 + 17)
#define INT_NUM_IM4_IRL18               (INT_NUM_IM4_IRL0 + 18)
#define INT_NUM_IM4_IRL19               (INT_NUM_IM4_IRL0 + 19)
#define INT_NUM_IM4_IRL20               (INT_NUM_IM4_IRL0 + 20)
#define INT_NUM_IM4_IRL21               (INT_NUM_IM4_IRL0 + 21)
#define INT_NUM_IM4_IRL22               (INT_NUM_IM4_IRL0 + 22)
#define INT_NUM_IM4_IRL23               (INT_NUM_IM4_IRL0 + 23)
#define INT_NUM_IM4_IRL24               (INT_NUM_IM4_IRL0 + 24)
#define INT_NUM_IM4_IRL25               (INT_NUM_IM4_IRL0 + 25)
#define INT_NUM_IM4_IRL26               (INT_NUM_IM4_IRL0 + 26)
#define INT_NUM_IM4_IRL27               (INT_NUM_IM4_IRL0 + 27)
#define INT_NUM_IM4_IRL28               (INT_NUM_IM4_IRL0 + 28)
#define INT_NUM_IM4_IRL29               (INT_NUM_IM4_IRL0 + 29)
#define INT_NUM_IM4_IRL30               (INT_NUM_IM4_IRL0 + 30)
#define INT_NUM_IM4_IRL31               (INT_NUM_IM4_IRL0 + 31)

#define INT_NUM_SI_EIU_IR0              (INT_NUM_IRQ0 + 160)
#define INT_NUM_SI_EIU_IR1              (INT_NUM_SI_EIU_IR0 + 1)
#define INT_NUM_SI_EIU_IR2              (INT_NUM_SI_EIU_IR0 + 2)
#define INT_NUM_SI_EIU_IR3              (INT_NUM_SI_EIU_IR0 + 3)
#define INT_NUM_SI_EIU_IR4              (INT_NUM_SI_EIU_IR0 + 4)
#define INT_NUM_SI_EIU_IR5              (INT_NUM_SI_EIU_IR0 + 5)
#define INT_NUM_SI_EIU_IR6              (INT_NUM_SI_EIU_IR0 + 6)
#define INT_NUM_SI_EIU_IR7              (INT_NUM_SI_EIU_IR0 + 7)
#define INT_NUM_SI_EIU_IR8              (INT_NUM_SI_EIU_IR0 + 8)
#define INT_NUM_SI_EIU_IR9              (INT_NUM_SI_EIU_IR0 + 9)
#define INT_NUM_SI_EIU_IR10             (INT_NUM_SI_EIU_IR0 + 10)
#define INT_NUM_SI_EIU_IR11             (INT_NUM_SI_EIU_IR0 + 11)
#define INT_NUM_SI_EIU_IR12             (INT_NUM_SI_EIU_IR0 + 12)
#define INT_NUM_SI_EIU_IR13             (INT_NUM_SI_EIU_IR0 + 13)
#define INT_NUM_SI_EIU_IR14             (INT_NUM_SI_EIU_IR0 + 14)
#define INT_NUM_SI_EIU_IR15             (INT_NUM_SI_EIU_IR0 + 15)

#define MIPS_CPU_CPUCTR_IRQ             INT_NUM_IM4_IRL31
#define MIPS_CPU_TIMER_IRQ              (INT_NUM_SI_EIU_IR15 + 1)
#define MIPSCPU_INT_BASE                (MIPS_CPU_TIMER_IRQ + 1)

#ifdef CONFIG_MIPS_MT
  #define NR_IRQS                       (MIPSCPU_INT_BASE + 8)  /* Largest number of ints of all machines plus timer + 2 software IRQs + 6 hardware IRQs */
#else
  #define NR_IRQS                       (MIPSCPU_INT_BASE)      /* Largest number of ints of all machines plus timer  */
#endif

#ifdef CONFIG_MIPS_MT_SMTC
#define MIPSCPU_INT_CPUCTR            7
#endif


#if defined(CONFIG_DANUBE)
#include "danube/irq.h"
#elif defined(CONFIG_AMAZON_SE)
#include "amazon_se/irq.h"
#elif defined(CONFIG_AR9)
#include "ar9/irq.h"
#elif defined(CONFIG_VR9)
#include "vr9/irq.h"
#elif defined(CONFIG_AR10)
#include "ar10/irq.h"
#else
#error unknown chip
#endif


extern void bsp_mask_and_ack_irq(u32 irq_nr);
extern void bsp_enable_irq(u32 irq_nr);
extern void bsp_disable_irq(u32 irq_nr);
extern void ifx_icu_irsr_set(u32 irq);
extern void ifx_icu_irsr_clr(u32 irq);


#endif  /*  IFX_IRQ_H */
