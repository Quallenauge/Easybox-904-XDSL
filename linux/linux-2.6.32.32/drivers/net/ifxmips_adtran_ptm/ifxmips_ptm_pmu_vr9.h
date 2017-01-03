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

#ifndef IFXMIPS_PTM_PMU_VR9_H
#define IFXMIPS_PTM_PMU_VR9_H



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  PMU Register
 */
extern unsigned int ifx_vr9_pmu_base_addr;
#define IFX_PMU                                 KSEG1ADDR(ifx_vr9_pmu_base_addr)
#define IFX_PMU_ORG                             0x1F102000

/* PMU Power Down Control Register */
#define IFX_PMU_PWDCR                           ((volatile u32*)(IFX_PMU + 0x001C))
#define IFX_PMU_SR                              ((volatile u32*)(IFX_PMU + 0x0020))

/* The following is new for VR9 and future platform */
#define IFX_PMU_PWDCR1                          ((volatile u32*)(IFX_PMU + 0x0024))
#define IFX_PMU_SR1                             ((volatile u32*)(IFX_PMU + 0x0028))



#endif  //  IFXMIPS_PTM_PMU_VR9_H
