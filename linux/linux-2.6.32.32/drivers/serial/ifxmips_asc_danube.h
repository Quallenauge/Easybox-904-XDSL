/******************************************************************************
**
** FILE NAME    : ifxmips_asc_danube.h
** PROJECT      : IFX UEIP
** MODULES      : ASC (UART)
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global IFX ASC (UART) driver header file for Danube
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



#ifndef	IFX_ASC_DANUBE_H
#define	IFX_ASC_DANUBE_H



#define UART_NR                         2

#if defined(CONFIG_SERIAL_IFX_ASC_CONSOLE)
#  if defined(CONFIG_IFX_ASC_CONSOLE_ASC0)
#    define IFX_ASC_CONSOLE_INDEX       0
#  else
#    define IFX_ASC_CONSOLE_INDEX       1
#  endif
#endif



static ifx_asc_port_priv_t ifx_asc_port_priv[UART_NR] = {
    {(ifx_asc_reg_t*)IFX_ASC0, 32, IFX_ASC0_TIR, IFX_ASC0_TBIR, IFX_ASC0_RIR, IFX_ASC0_EIR, "asc0_tx", "asc0_rx", "asc0_err", 0, 0, 0, 0, 0, 0},
    {(ifx_asc_reg_t*)IFX_ASC1,  8, IFX_ASC1_TIR, IFX_ASC1_TBIR, IFX_ASC1_RIR, IFX_ASC1_EIR, "asc1_tx", "asc1_rx", "asc1_err", 0, 0, 0, 0, 0, 0}
};



#endif	//	IFX_ASC_DANUBE_H
