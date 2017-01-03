/******************************************************************************
**
** FILE NAME    : ifx_board.h
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



#ifndef IFX_BOARD_H
#define IFX_BOARD_H



#if defined(CONFIG_DANUBE_REF_BOARD)
#  include "danube/danube_ref_board.h"
#elif defined(CONFIG_AMAZON_SE)
#  include "amazon_se/boards/boards.h"
#elif defined(CONFIG_AR9_REF_BOARD)
#  include "ar9/ar9_ref_board.h"
#elif defined(CONFIG_VR9_REF_BOARD)
#  include "vr9/vr9_ref_board.h"
#elif defined(CONFIG_AR10_REF_BOARD)
#  include "ar10/ar10_ref_board.h"
#else
#  error unknown board
#endif



#endif  //  IFX_BOARD_H

