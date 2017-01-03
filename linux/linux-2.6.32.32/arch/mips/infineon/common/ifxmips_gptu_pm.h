/******************************************************************************
**
** FILE NAME    : ifxmips_gptu_pm.h
** PROJECT      : IFX UEIP
** MODULES      : GPTU Power Management
**
** DATE         : 16 Dec 2009
** AUTHOR       : Huang Xiaogang
** DESCRIPTION  : GPTU Driver Power Managment
** COPYRIGHT    :       Copyright (c) 2009
**                      Lantiq Deutschland GmbH
**                      Am Campeon 3, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 16 Dec,2009   Huang Xiaogang First UEIP release
*******************************************************************************/
/*!
 \file ifxmips_gptu_pm.h
 \ingroup IFX_GPTU    
 \brief header file for  Power Management
*/

#ifndef IFXMIPS_GPTU_PM_H
#define IFXMIPS_GPTU_PM_H

void ifx_gptu_pmcu_init(void);
void ifx_gptu_pmcu_exit(void);

#endif /* IFXMIPS_GPTU_PM_H  */
