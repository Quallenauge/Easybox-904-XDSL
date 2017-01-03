/******************************************************************************
**
** FILE NAME    : ifxmips_ptm_common.h
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : PTM driver header file (common definitions)
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
** $Date        $Author         $Comment
** 17 JUN 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFXMIPS_PTM_GPIO_VR9_H
#define IFXMIPS_PTM_GPIO_VR9_H



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  GPIO Register
 */
#define IFX_GPIO                                (KSEG1 | 0x1E100B00)

#define IFX_GPIO_P0_OUT                         ((volatile u32 *)(IFX_GPIO + 0x0010))



#endif  //  IFXMIPS_PTM_GPIO_VR9_H
