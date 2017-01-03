/******************************************************************************
**
** FILE NAME    : ifxmips_interrupt.h
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : ICU driver header file
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



#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <linux/types.h>

#if defined(CONFIG_DANUBE) || defined(CONFIG_AMAZON_SE)
#  define ICU_OFFSET_OF_IVEC    12
#elif defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
#  define ICU_OFFSET_OF_IVEC    2
#else
#  error  ICU_OFFSET_OF_IVEC is not defined (no platform info).
#endif

#if defined(CONFIG_DANUBE) || defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_AR10)
#  define ICU_IM_NUM            5
#elif defined(CONFIG_AMAZON_SE)
#  define ICU_IM_NUM            4
#else
#  error  ICU_IM_NUM is not defined (no platform info).
#endif

struct ifx_icu_im {
    volatile u32                im_isr;     /*  0x00    */
    volatile u32                reserved0;  /*  0x04    */
    volatile u32                im_ier;     /*  0x08    */
    volatile u32                reserved1;  /*  0x0C    */
    volatile u32                im_iosr;    /*  0x10    */
    volatile u32                reserved2;  /*  0x14    */
    volatile u32                im_irsr;    /*  0x18    */
    volatile u32                reserved3;  /*  0x1C    */
    volatile u32                im_imr;     /*  0x20    */
    volatile u32                reserved4;  /*  0x24    */
};

struct ifx_icu {
    volatile struct ifx_icu_im  im[5];
    volatile u32                reserved[ICU_OFFSET_OF_IVEC];
    volatile u32                ivec;
};

struct ifx_eiu_icu {
    volatile u32                eiu_exin_c; /*  0x00    */
    volatile u32                eiu_inic;   /*  0x04    */
    volatile u32                eiu_inc;    /*  0x08    */
    volatile u32                eiu_inen;   /*  0x0C    */
};

typedef struct ifx_icu         *ifx_icu_t;
typedef struct ifx_eiu_icu     *ifx_eiu_icu_t;


#endif  //  INTERRUPT_H
