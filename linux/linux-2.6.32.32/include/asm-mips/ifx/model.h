/******************************************************************************
**
** FILE NAME    : model.h
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



#ifndef IFX_MODEL_H
#define IFX_MODEL_H



#if defined(CONFIG_DANUBE)
#  include "danube/model.h"
#elif defined(CONFIG_AMAZON_SE)
#  include "amazon_se/model.h"
#elif defined(CONFIG_AR9)
#  include "ar9/model.h"
#elif defined(CONFIG_VR9)
#  include "vr9/model.h"
#elif defined(CONFIG_AR10)
#  include "ar10/model.h"
#else
#  error unknown chip
#endif



#endif  //  IFX_MODEL_H

