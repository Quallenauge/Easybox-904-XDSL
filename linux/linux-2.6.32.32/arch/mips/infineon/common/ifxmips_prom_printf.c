/******************************************************************************
**
** FILE NAME    : ifxmips_prom_printf.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : common source file
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
#include <linux/module.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>

void prom_printf(const char * fmt, ...)
{
    static char buf[1024];

    va_list args;
    int l;
    char *p, *buf_end;

    /* Low level, brute force, not SMP safe... */
    va_start(args, fmt);
    l = vsprintf(buf, fmt, args); /* hopefully i < sizeof(buf) */
    va_end(args);
    buf_end = buf + l;

#ifdef CONFIG_IFX_ASC_CONSOLE_ASC0
#define IFX_CONSOLE_ASC_FSTAT     IFX_ASC0_FSTAT
#define IFX_CONSOLE_ASC_TBUF      IFX_ASC0_TBUF
#else
#define IFX_CONSOLE_ASC_FSTAT     IFX_ASC1_FSTAT
#define IFX_CONSOLE_ASC_TBUF      IFX_ASC1_TBUF
#endif
#define ASCFSTAT_TXFFLMASK          0x3F00
    for ( p = buf; p < buf_end; p++ ) {
        /* Wait for FIFO to empty */
        while ( (*IFX_CONSOLE_ASC_FSTAT & ASCFSTAT_TXFFLMASK) != 0x00 );
        /* Crude cr/nl handling is better than none */
        if ( *p == '\n' )
            *IFX_CONSOLE_ASC_TBUF = '\r';
        *IFX_CONSOLE_ASC_TBUF = *p;
    }
}
EXPORT_SYMBOL(prom_printf);
