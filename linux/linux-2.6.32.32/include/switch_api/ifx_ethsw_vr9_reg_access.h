/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_reg_access.h"
   \remarks 
 *****************************************************************************/
#ifndef _IFX_ETHSW_REG_ACCESS_H
#define _IFX_ETHSW_REG_ACCESS_H

#include "ifx_types.h"

IFX_int32_t ifx_ethsw_ll_DirectAccessRead
(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t * value);
IFX_return_t ifx_ethsw_ll_DirectAccessWrite
(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t value);

#endif    /* _IFX_ETHSW_REG_ACCESS.H */
