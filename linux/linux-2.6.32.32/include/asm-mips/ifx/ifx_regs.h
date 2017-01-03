/******************************************************************************
**
** FILE NAME    : ifx_regs.h
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



#ifndef IFX_CHIP_REGS_H
#define IFX_CHIP_REGS_H

#include <asm/io.h>

/*
 *  Register Operation
 */
#define IFX_REG_R32(_r)                    __raw_readl((volatile unsigned int *)(_r))
#define IFX_REG_W32(_v, _r)               __raw_writel((_v), (volatile unsigned int *)(_r))
#define IFX_REG_W32_MASK(_clr, _set, _r)   IFX_REG_W32((IFX_REG_R32((_r)) & ~(_clr)) | (_set), (_r))
#define IFX_REG_R16(_r)                    __raw_readw((_r))
#define IFX_REG_W16(_v, _r)               __raw_writew((_v), (_r))
#define IFX_REG_W16_MASK(_clr, _set, _r)   IFX_REG_W16((IFX_REG_R16((_r)) & ~(_clr)) | (_set), (_r))
#define IFX_REG_R8(_r)                     __raw_readb((_r))
#define IFX_REG_W8(_v, _r)                __raw_writeb((_v), (_r))
#define IFX_REG_W8_MASK(_clr, _set, _r)    IFX_REG_W8((IFX_REG_R8((_r)) & ~(_clr)) | (_set), (_r))

/*
 * Register manipulation macros that expect bit field defines
 * to follow the convention that an _S suffix is appended for
 * a shift count, while the field mask has no suffix. Or can use
 * _M as suffix
 */

/* Shift first, then mask, usually for write operation */
#define SM(_v, _f)  (((_v) << _f##_S) & (_f))

/* Mask first , then shift, usually for read operation */
#define MS(_v, _f)  (((_v) & (_f)) >> _f##_S)

#define IFX_REG_RMW32(_set, _clr, _r)    \
    IFX_REG_W32((IFX_REG_R32((_r)) & ~(_clr)) | (_set), (_r))

#define IFX_REG_RMW32_FILED(_f, _v, _r) \
    IFX_REG_W32(\
        (IFX_REG_R32((_r)) &~ (_f)) | (((_v) << (_f##_S)) & (_f)), (_r))

#define IFX_REG_SET_BIT(_f, _r) \
    IFX_REG_W32((IFX_REG_R32((_r)) &~ (_f)) | (_f), (_r))

#define IFX_REG_CLR_BIT(_f, _r) \
    IFX_REG_W32(IFX_REG_R32((_r)) &~ (_f), (_r))

#define IFX_REG_IS_BIT_SET(_f, _r) \
    ((IFX_REG_R32((_r)) & (_f)) != 0)

/*
 *  Bits Operation
 */
#define GET_BITS(x, msb, lsb)               \
    (((x) >> (lsb)) & ((1 << ((msb) + 1 - (lsb))) - 1))
#define SET_BITS(x, msb, lsb, value)        \
    (((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | (((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

#if defined(CONFIG_DANUBE)
#include "danube/danube.h"
#elif defined(CONFIG_AMAZON_SE)
#include "amazon_se/amazon_se.h"
#elif defined(CONFIG_AR9)
#include "ar9/ar9.h"
#elif defined(CONFIG_VR9)
#include "vr9/vr9.h"
#elif defined(CONFIG_AR10)
#include "ar10/ar10.h"
#else
#error unknown chip
#endif
#include "ifx_board.h"

/*
 *  Chip ID
 */
typedef struct {
#define IFX_FAMILY_UNKNOWN              0
#define IFX_FAMILY_DANUBE               1
#define IFX_FAMILY_TWINPASS             IFX_FAMILY_DANUBE
#define IFX_FAMILY_AMAZON_SE            2
#define IFX_FAMILY_ASE                  IFX_FAMILY_AMAZON_SE
#define IFX_FAMILY_AR9                  3
#define IFX_FAMILY_xRX100               IFX_FAMILY_AR9
#define IFX_FAMILY_VR9                  4
#define IFX_FAMILY_xRX200               IFX_FAMILY_VR9
    unsigned int        family_id;
#define IFX_FAMILY_DANUBE_A1x           1
#define IFX_FAMILY_TWINPASS_A1x         IFX_FAMILY_DANUBE_A1x
#define IFX_FAMILY_AMAZON_SE_A1x        1
#define IFX_FAMILY_ASE_A1x              IFX_FAMILY_AMAZON_SE_A1x
#define IFX_FAMILY_AR9_A1x              1
#define IFX_FAMILY_xRX100_A1x           IFX_FAMILY_AR9_A1x
#define IFX_FAMILY_VR9_A1x              1
#define IFX_FAMILY_xRX200_A1x           IFX_FAMILY_VR9_A1x
#define IFX_FAMILY_xRX200_A2x           2
    unsigned int        family_ver;
#define IFX_PARTNUM_DANUBE              0x00EB
#define IFX_PARTNUM_DANUBE_S            0x00ED
#define IFX_PARTNUM_AMAZON_S	        0x00EF
#define IFX_PARTNUM_VINAX_VE            0x013C
#define IFX_PARTNUM_VINAX_E             0x0151
#define IFX_PARTNUM_AMAZON_SE_50601     0x0152
#define IFX_PARTNUM_AMAZON_SE_50600     0x0153
#define IFX_PARTNUM_ARX188              0x016C
#define IFX_PARTNUM_ARX168              0x016D
#define IFX_PARTNUM_GRX188              0x0170
#define IFX_PARTNUM_GRX168              0x0171
#define IFX_PARTNUM_VRX288_A1x          0x01C0
#define IFX_PARTNUM_VRX282_A1x          0x01C1
#define IFX_PARTNUM_VRX268_A1x          0x01C2
#define IFX_PARTNUM_GRX288_A1x          0x01C9
#define IFX_PARTNUM_GRX268_A1x          0x01C8
#define IFX_PARTNUM_VRX288_A2x          0x000B
#define IFX_PARTNUM_VRX282_A2x          0x000E
#define IFX_PARTNUM_VRX268_A2x          0x000C
#define IFX_PARTNUM_GRX288_A2x          0x000D
    unsigned int        part_number;
    unsigned int        chip_version;
    unsigned int        manufacturer_id;
} ifx_chipid_t;
static inline void ifx_get_chipid(ifx_chipid_t *p_chipid)
{
    unsigned int chipid;

    if ( p_chipid == NULL )
        return;

    chipid = IFX_REG_R32(IFX_MPS_CHIPID);
    p_chipid->part_number     = GET_BITS(chipid, 27, 12);
    p_chipid->chip_version    = GET_BITS(chipid, 30, 28);
    p_chipid->manufacturer_id = GET_BITS(chipid, 11, 1);

    switch ( p_chipid->part_number ) {
        case IFX_PARTNUM_DANUBE:
        case IFX_PARTNUM_DANUBE_S:
        case IFX_PARTNUM_AMAZON_S:
            p_chipid->family_id  = IFX_FAMILY_DANUBE;
            p_chipid->family_ver = IFX_FAMILY_DANUBE_A1x;
            break;
        case IFX_PARTNUM_VINAX_VE:
        case IFX_PARTNUM_VINAX_E:
            p_chipid->family_id  = IFX_FAMILY_TWINPASS;
            p_chipid->family_ver = IFX_FAMILY_DANUBE_A1x;
            break;
        case IFX_PARTNUM_AMAZON_SE_50601:
        case IFX_PARTNUM_AMAZON_SE_50600:
            p_chipid->family_id  = IFX_FAMILY_AMAZON_SE;
            p_chipid->family_ver = IFX_FAMILY_AMAZON_SE_A1x;
            break;
        case IFX_PARTNUM_ARX188:
        case IFX_PARTNUM_ARX168:
        case IFX_PARTNUM_GRX188:
        case IFX_PARTNUM_GRX168:
            p_chipid->family_id  = IFX_FAMILY_xRX100;
            p_chipid->family_ver = IFX_FAMILY_xRX100_A1x;
            break;
        case IFX_PARTNUM_VRX288_A1x:
        case IFX_PARTNUM_VRX282_A1x:
        case IFX_PARTNUM_VRX268_A1x:
        case IFX_PARTNUM_GRX288_A1x:
        case IFX_PARTNUM_GRX268_A1x:
            p_chipid->family_id  = IFX_FAMILY_xRX200;
            p_chipid->family_ver = IFX_FAMILY_xRX200_A1x;
            break;
        case IFX_PARTNUM_VRX288_A2x:
        case IFX_PARTNUM_VRX282_A2x:
        case IFX_PARTNUM_VRX268_A2x:
        case IFX_PARTNUM_GRX288_A2x:
            p_chipid->family_id  = IFX_FAMILY_xRX200;
            p_chipid->family_ver = IFX_FAMILY_xRX200_A2x;
            break;
        default:
            p_chipid->family_id = IFX_FAMILY_UNKNOWN;
    }
}

/*
 *  Clock
 */
#define CLOCK_25M                           25000000
#define CLOCK_48M                           48000000
#define CLOCK_60M                           60000000
#define CLOCK_62_5M                         62500000
#define CLOCK_83M                           83333333
#define CLOCK_83_5M                         83500000
#define CLOCK_98_304M                       98304000
#define CLOCK_111M                          111111111
#define CLOCK_125M                          125000000
#define CLOCK_133M                          133333333
#define CLOCK_150M                          150000000
#define CLOCK_166M                          166666666
#define CLOCK_167M                          166666667
#define CLOCK_200M                          200000000
#define CLOCK_196_608M                      196608000
#define CLOCK_222M                          222222222
#define CLOCK_250M                          250000000
#define CLOCK_266M                          266666666
#define CLOCK_300M                          300000000
#define CLOCK_333M                          333333333
#define CLOCK_393M                          393215332
#define CLOCK_500M                          500000000
#define CLOCK_600M                          600000000
#define CLOCK_1000M                        1000000000

#endif  /* IFX_CHIP_REGS_H */

