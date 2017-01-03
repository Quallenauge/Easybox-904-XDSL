/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_vr9_ral.c
   \remarks implement the register access by internal bus.
 *****************************************************************************/

#include <ifx_ethsw_reg_access.h>
#include <ifx_ethsw_vr9_reg_access.h>

/**
* \fn int IFX_Register_init_Switch_Dev(IFX_void_t)
* \brief Init and register the RAL switch device
* \param  none
* \return on Success return data read from register, else return IFX_ERROR
*/

extern IFX_uint32_t ifx_ethsw_ll_DirectAccessRead(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t * value);
extern IFX_return_t ifx_ethsw_ll_DirectAccessWrite(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t value);

IFX_void_t *IFX_FLOW_RAL_init(IFX_FLOW_RAL_Init_t *pInit)
{

  IFX_FLOW_RAL_Dev_t *pDev;

//#ifdef IFXOS_SUPPORT
  pDev = (IFX_FLOW_RAL_Dev_t *) IFXOS_BlockAlloc (sizeof (IFX_FLOW_RAL_Dev_t));
//#else
//  pDev = (IFX_FLOW_RAL_Dev_t *) kmalloc (sizeof (IFX_FLOW_RAL_Dev_t), GFP_KERNEL);
//#endif

  if(pDev == IFX_NULL) {
    IFXOS_PRINT_INT_RAW("Error : %s memory allocation failed !!\n", __func__);
    return IFX_NULL;
  }

  if(pInit->eDev == IFX_FLOW_DEV_INT)
  {
    pDev->register_read = &ifx_ethsw_ll_DirectAccessRead;
    pDev->register_write = &ifx_ethsw_ll_DirectAccessWrite;
    pDev->nBaseAddress = VR9_BASE_ADDRESS;
  }

  pDev->eDev = pInit->eDev;
  
  return pDev;

}

