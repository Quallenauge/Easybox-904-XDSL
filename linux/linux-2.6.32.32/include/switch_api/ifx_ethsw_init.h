/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_init.h
   \remarks Generic switch API header file, for Infineon Ethernet switch
            drivers
 *****************************************************************************/
#ifndef _IFX_ETHSW_INIT_H_
#define _IFX_ETHSW_INIT_H_

#include "ifx_ethsw_linux.h"
#include "ifx_ethsw_PSB6970_core.h"
#include "ifx_ethsw_flow_core.h"
#include "ifx_ethsw_ral.h"
#include "ifx_ethsw_rml.h"
#include "ifx_ethsw_reg_access.h"

typedef struct {
	IFX_uint8_t             			minorNum;
    IFX_void_t              			*pCoreDev;
} IFX_ETHSW_coreHandle_t;

#endif    /* _IFX_ETHSW_INIT_H_ */
