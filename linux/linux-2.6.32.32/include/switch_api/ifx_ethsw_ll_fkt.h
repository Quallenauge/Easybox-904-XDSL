/****************************************************************************

                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _IFX_ETHSW_LL_FKT_H
#define _IFX_ETHSW_LL_FKT_H

#include "ifx_types.h"

/* general declaration fits for all low-level functions. */
typedef IFX_return_t (*IFX_ll_fkt) (IFX_void_t *, IFX_uint32_t);
typedef struct IFX_ETHSW_lowLevelFkts_t IFX_ETHSW_lowLevelFkts_t;

/* Switch API low-level function tables to map all supported IOCTL commands */
struct IFX_ETHSW_lowLevelFkts_t
{
   /* Some device have multiple tables to split the generic switch API features
      and the device specific switch API features. Additional tables, if exist,
      can be found under this next pointer. Every table comes along with a
      different 'nType' parameter to differentiate. */
   IFX_ETHSW_lowLevelFkts_t         *pNext;
   /* IOCTL type of all commands listed in the table. */
   IFX_uint16_t                     nType;
   /* Number of low-level functions listed in the table. */
   IFX_uint32_t                     nNumFkts;
   /* Pointer to the first entry of the ioctl number table. This table is used
      to check if the given ioctl command fits the the found low-level function
      pointer under 'pFkts'. */
   // IFX_uint32_t                     *pIoctlCmds;
   /* Pointer to the first entry of the function table. Table size is given
      by the parameter 'nNumFkts'. */
   IFX_ll_fkt                       *pFkts;
};

#endif /* #ifndef _IFX_ETHSW_LL_FKT_H */
