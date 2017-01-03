/******************************************************************************
**
** FILE NAME    : ifxmips_si_ar9.h
** PROJECT      : UEIP
** MODULES      : Serial In Controller
**
** DATE         : 26 Apr 2010
** AUTHOR       : Xu Liang
** DESCRIPTION  : Serial In Controller driver header file for AR9
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date          $Author         $Comment
** Apr 26, 2010   Xu Liang        Init Version
*******************************************************************************/



#ifndef IFXMIPS_SI_AR9_H
#define IFXMIPS_SI_AR9_H



/*  SI Registers Mapping    */
#define IFX_SI_BASE                     0x1E100B00
#define IFX_SI_REG(x)                   ((volatile unsigned int *)KSEG1ADDR(IFX_SI_BASE | (x)))
#define IFX_SI_CPU                      IFX_SI_REG(0xC4)
#define IFX_SI_CON                      IFX_SI_REG(0xC8)
#define IFX_SI_INT_CON                  IFX_SI_REG(0xCC)
#define IFX_SI_INTCR_CON                IFX_SI_REG(0xD0)
#define IFX_SI_D_CON                    IFX_SI_REG(0xD4)

/*  Data Structure  */
static ifx_si_port_t g_si_port_priv = {
    .chipid      = 0x0016C000,
    .chipid_mask = 0x0FFFC000,
    .reg = {
        .cpu    = IFX_SI_CPU,
        .con    = IFX_SI_CON,
        .int_con= IFX_SI_INT_CON,
        .int_clr= IFX_SI_INTCR_CON,
        .dl_con = IFX_SI_D_CON,
    },
    .pin_num    = IFX_SI_MAX_PIN,
    .pin_status = {0},
};



#endif  //  IFXMIPS_SI_AR9_H
