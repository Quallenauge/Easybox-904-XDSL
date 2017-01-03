/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_ral.c
   \remarks implement the register access layer.
 *****************************************************************************/

#include "ifx_ethsw_ral.h"

/**
   This is the switch ral layer function for internal switch

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_RAL_Dev_t context.
   \param RegAddr  This parameter is a register address.
   \param value  This value get from register. 

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RAL_Register_DirectRead(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value) 
{
    IFX_PSB6970_RAL_Dev_t *pDev = (IFX_PSB6970_RAL_Dev_t *)pDevCtx;
#ifdef DEBUG_RAL
    IFXOS_PRINT_INT_RAW("\nAR9Read: offset=0x%x, ", RegAddr);
#endif
    RegAddr += pDev->nBaseAddress;
    *value = REG32_ACCESS(RegAddr);
#ifdef DEBUG_RAL
    IFXOS_PRINT_INT_RAW("value=0x%x, addr=0x%x\n", *value, RegAddr);
#endif
    return IFX_SUCCESS;
}

/**
   This is the switch ral layer function for internal switch

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_RAL_Dev_t context.
   \param RegAddr  This parameter is a register address.
   \param value  This value is written to register. 

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RAL_Register_DirectWrite(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value) 
{
    IFX_PSB6970_RAL_Dev_t *pDev = pDevCtx;
#ifdef DEBUG_RAL
    IFXOS_PRINT_INT_RAW("\nAR9Write: offset=0x%x, ", RegAddr);
#endif
    RegAddr += pDev->nBaseAddress;
    REG32_ACCESS(RegAddr) = value;
#ifdef DEBUG_RAL
    IFXOS_PRINT_INT_RAW("value=0x%x, addr=0x%x\n", value, RegAddr);
#endif
    return IFX_SUCCESS;
}

/**
   This is the switch ral layer function for external switch

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_RAL_Dev_t context.
   \param RegAddr  This parameter is a register address.
   \param value  This value get from register. 

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RAL_Register_MDIORead(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value) 
{
    IFX_PSB6970_RAL_Dev_t *pDev = pDevCtx;
    IFX_uint32_t MDIO_CTL_Register;
    IFX_uint32_t regAddr;
    IFX_uint32_t regValue;
	unsigned long __ilockflags;
    MDIO_CTL_Register = (0x01<<IFX_PSB6970_MDIO_MBUSY_SHIFT) + (IFX_PSB6970_MDIO_READ_OPERATION << IFX_PSB6970_MDIO_OP_SHIFT) + (RegAddr);
    MDIO_CTL_Register = (0x0000FFFF & MDIO_CTL_Register);
    local_irq_save(__ilockflags);
    do {
        regValue = REG32_ACCESS(pDev->nBaseAddress);
    } while ( regValue & 0x00008000);
    //while((REG32_ACCESS(pDev->nBaseAddress) & 0x8000));
   
    REG32_ACCESS(pDev->nBaseAddress) = MDIO_CTL_Register;
/*    IFXOS_USecSleep(50); */
//    IFXOS_USecSleep(10);
    do {
		IFXOS_USecSleep(10);
        regValue = REG32_ACCESS(pDev->nBaseAddress);
    } while ( regValue & 0x00008000);
    //while((REG32_ACCESS(pDev->nBaseAddress) & 0x8000));
    regAddr = pDev->nBaseAddress + 4;
    *value = REG32_ACCESS((regAddr));
    local_irq_restore( __ilockflags);
#ifdef DEBUG_RAL
    IFXOS_PRINT_INT_RAW("\nT3GRead: offset=0x%x, read-vlaue=0x%x ", RegAddr, *value);
    IFXOS_PRINT_INT_RAW("Addr=0x%x, MDIO_CTL_Register=0x%x\n", pDev->nBaseAddress, MDIO_CTL_Register);
#endif

    return IFX_SUCCESS;
}

/**
   This is the switch ral layer function for external switch

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_RAL_Dev_t context.
   \param RegAddr  This parameter is a register address.
   \param value  This value is written to register.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RAL_Register_MDIOWrite(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value) 
{
    IFX_PSB6970_RAL_Dev_t *pDev = pDevCtx;
    IFX_uint32_t MDIO_CTL_Register;
    IFX_uint32_t regValue;
	unsigned long __ilockflags;
    MDIO_CTL_Register = (value << IFX_PSB6970_MDIO_WD_SHIFT) + (0x01<<IFX_PSB6970_MDIO_MBUSY_SHIFT) + (IFX_PSB6970_MDIO_WRITE_OPERATION << IFX_PSB6970_MDIO_OP_SHIFT) + (RegAddr);
	
#ifdef DEBUG_RAL
    IFXOS_PRINT_INT_RAW("\nT3GWrite: offset=0x%x, ", RegAddr);
    IFXOS_PRINT_INT_RAW("Addr=0x%x, value=0x%x\n", pDev->nBaseAddress, value);
#endif
    local_irq_save(__ilockflags);
    //while((REG32_ACCESS(pDev->nBaseAddress) & 0x8000));
    do {
        regValue = REG32_ACCESS(pDev->nBaseAddress);
    } while ( regValue & 0x00008000);

    REG32_ACCESS(pDev->nBaseAddress) = MDIO_CTL_Register;

    do {
    	 IFXOS_USecSleep(10);
        regValue = REG32_ACCESS(pDev->nBaseAddress);
    } while ( regValue & 0x00008000);
    //while((REG32_ACCESS(pDev->nBaseAddress) & 0x8000));
	local_irq_restore( __ilockflags);
    return IFX_SUCCESS;
}

/**
   This is the switch ral layer init function 

   \param pInit  This parameter is a pointer to the ral init handle context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_void_t *IFX_PSB6970_RAL_init(IFX_PSB6970_RAL_Init_t *pInit)
{
	IFX_PSB6970_RAL_Dev_t *pDev;
	
	pDev = (IFX_PSB6970_RAL_Dev_t *) IFXOS_BlockAlloc (sizeof (IFX_PSB6970_RAL_Dev_t));
	if(pDev == IFX_NULL) {
        IFXOS_PRINT_INT_RAW("Error : %s memory allocation failed !!\n", __func__);
        return (pDev);
    }
    switch (pInit->eDev) {
    	case IFX_PSB6970_DEV_AR9:
             pDev->register_read = &IFX_PSB6970_RAL_Register_DirectRead;
             pDev->register_write = &IFX_PSB6970_RAL_Register_DirectWrite;
             pDev->nBaseAddress = IFX_PSB6970_AR9_BASEADDR;
             break;
        case IFX_PSB6970_DEV_TANTOS_3G:
             pDev->register_read = &IFX_PSB6970_RAL_Register_MDIORead;
             pDev->register_write = &IFX_PSB6970_RAL_Register_MDIOWrite;
             pDev->nBaseAddress = IFX_PSB6970_TANTOS_3G_BASEADDR;
             break;
        default:
        	pDev->register_read = IFX_NULL;
        	pDev->register_write = IFX_NULL ;
        	pDev->nBaseAddress = (int)IFX_NULL;
        	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML eDev not found) \n", __FILE__, __FUNCTION__, __LINE__);
        	if (pDev) {
        		kfree(pDev);
        		pDev = IFX_NULL;
        		
        	}
//        	return IFX_NULL;
    }
    pDev->eDev = pInit->eDev;
    return pDev;
}
