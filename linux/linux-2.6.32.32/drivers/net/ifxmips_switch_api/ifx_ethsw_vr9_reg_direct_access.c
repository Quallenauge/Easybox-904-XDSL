/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_reg_direct_access.c
   \remarks implement the register access by internal bus.
 *****************************************************************************/

#include <ifx_ethsw_reg_access.h>
#include <ifx_ethsw_vr9_reg_access.h>


/**
* \fn int ifx_switch_hw_ll_direct_access_read()
* \brief Access read low level hardware register via internal register
* \param  IFX_ETHSW_regMapper_t *RegAccessHandle
* \param  IFX_int32_t RegAddr
* \param  IFX_uint32_t * value
* \return on Success return data read from register, else return ETHSW_statusErr
*/
IFX_uint32_t ifx_ethsw_ll_DirectAccessRead(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t * value) 
{
		IFX_uint32_t regValue, regAddr, mask;
		
		/* Prepare the register address */
		regAddr = VR9_BASE_ADDRESS + (Offset * 4);
		
//		printk("regAddr(Read) = 0x%x\n",regAddr);
		/* Read the Whole 32bit register */
		regValue = VR9_REG32_ACCESS(regAddr);

		/* Prepare the mask	*/
		mask = (1 << Size) - 1 ;

		/* Bit shifting to the exract bit field */
		regValue = (regValue >> Shift);
		
		*value = (regValue & mask);

//		printk(" >> ifx_switch_hw_ll_direct_access_read => value [%x]\n",*value);
	
		return IFX_SUCCESS;
}

/**
* \fn int ifx_switch_hw_ll_direct_access_write()
* \brief Access read low level hardware register via internal register
* \param  IFX_ETHSW_regMapper_t *RegAccessHandle
* \param  IFX_int32_t RegAddr
* \param  IFX_uint32_t value
* \return on Success return OK else return ETHSW_statusErr
*/
IFX_return_t ifx_ethsw_ll_DirectAccessWrite(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t value) 
{
		IFX_uint32_t regValue, regAddr, mask;
		
		/* Prepare the register address */
		regAddr = VR9_BASE_ADDRESS + (Offset * 4);
		
//		printk("regAddr(Write) = 0x%x\n",regAddr);
		/* Read the Whole 32bit register */
		regValue = VR9_REG32_ACCESS(regAddr);

		/* Prepare the mask	*/
		mask = (1 << Size) - 1 ;
		mask = (mask << Shift);
		
		/* Shift the value to the right place and mask the rest of the bit*/
		value = ( value << Shift ) & mask;
		
		/*  Mask out the bit field from the read register and place in the new value */
		value = ( regValue & ~mask ) | value ;
		
//		printk("write value = 0x%x\n", value);
		/* Write into register */
		VR9_REG32_ACCESS(regAddr) = value ;
//	*((volatile IFX_uint32_t *)(RegAddr)) = value;
// 	printk(" >> ifx_switch_hw_ll_direct_access_read\n");
		
	return IFX_SUCCESS;

}

