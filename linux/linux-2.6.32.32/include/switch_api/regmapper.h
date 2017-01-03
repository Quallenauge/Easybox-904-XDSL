/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file regmapper.h
   \remarks  switch API header file, for Infineon Ethernet switch drivers
 *****************************************************************************/

#ifndef _REGMAPPER_H
#define _REGMAPPER_H

#include "ifx_types.h"
#include "commonReg.h"

typedef struct
{
   IFX_uint16_t   nCommonBitEnum;
   IFX_uint16_t   nRegOffset;
   IFX_uint8_t    nBitPosition;
   IFX_uint8_t    nBitSize;
}IFX_ETHSW_regMapper_t;
#endif /* #ifndef _REGMAPPER_H */
