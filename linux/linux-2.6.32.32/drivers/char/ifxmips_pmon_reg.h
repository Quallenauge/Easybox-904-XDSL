/******************************************************************************
**
** FILE NAME    : ifxmips_pmon_reg.h
** PROJECT      : IFX UEIP
** MODULES      : PMON
**
** DATE         : 21 July 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX Performance Monitor
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
** 21 July 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/
/*!
  \file ifxmips_pmon_reg.h
  \ingroup IFX_PMON
  \brief ifx pmon driver register definition
*/
#ifndef IFXMIPS_PMON_REG_H
#define IFXMIPS_PMON_REG_H

#ifdef CONFIG_VR9
#include "ifxmips_pmon_vr9.h"
#elif defined (CONFIG_AR10)
#include "ifxmips_pmon_ar10.h"
#else
#error "No platform defined"
#endif

#endif /* IFXMIPS_PMON_REG_H */

