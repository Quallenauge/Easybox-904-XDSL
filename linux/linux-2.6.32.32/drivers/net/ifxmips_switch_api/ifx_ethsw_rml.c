/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_rml.c
   \remarks implement the register mapping layer.
 *****************************************************************************/

#include "ifx_ethsw_rml.h"

extern IFX_ETHSW_regMapper_t regMapper_TANTOS_3G[];
extern IFX_ETHSW_regMapper_t regMapper_AR9[];
extern char regMapper_TANTOS_3G_Names[];

/**
   This is the switch rml layer function to get register

   \param pDevCtx  This parameter is a pointer to the rml device context.
   \param RegAddr  This parameter is a register address.
   \param value  This pointer to the value that getting from hardware register.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RML_RegisterGet(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * pvalue) 
{
    IFX_PSB6970_RML_Dev_t *pDev = pDevCtx;
    pDev->register_read(pDev->pRegAccessHandle, RegAddr, pvalue);

#ifdef DEBUG_RML
    IFXOS_PRINT_INT_RAW("\n%s[%d]: offset=0x%x, value=0x%x\n",__func__,__LINE__, RegAddr, *pvalue);
#endif

    return IFX_SUCCESS;
}

/**
   This is the switch rml layer function to set register

   \param pDevCtx  This parameter is a pointer to the rml device context.
   \param RegAddr  This parameter is a register address.
   \param value  This parameter is the value that want to be set.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_PSB6970_RML_RegisterSet(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value) 
{
    IFX_PSB6970_RML_Dev_t *pDev = pDevCtx;
#ifdef DEBUG_RML
    IFXOS_PRINT_INT_RAW("\n%s[%d]: offset=0x%x, value=0x%x\n",__func__,__LINE__, RegAddr, value);
#endif
    pDev->register_write(pDev->pRegAccessHandle, RegAddr, value);
    return IFX_SUCCESS;
}

/**
   This is the switch rml layer function to read the register value by using common bit.

   \param pCxtHandle  This parameter is a pointer to the rml device context.
   \param commonbit This parameter is a register mapper selector.
   \param portIdx This parameter is a port index
   \param value This value get from hardware register.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/

IFX_return_t IFX_PSB6970_RML_Read(IFX_void_t* pCxtHandle, 
                                    IFX_ETHSW_regMapperSelector_t commonbit, 
                                    IFX_uint32_t portIdx, 
                                    IFX_uint32_t *pvalue)
{
    IFX_PSB6970_RML_Dev_t *pDev = (IFX_PSB6970_RML_Dev_t*)pCxtHandle;
    IFX_ETHSW_regMapper_t *tableHandle = pDev->tableHandle;
    IFX_uint32_t tableIdx = ((IFX_uint32_t)commonbit) + portIdx;
    IFX_uint32_t readRegValue, regAddr, mask;

#ifdef DEBUG_RML
    IFXOS_PRINT_INT_RAW("\n%s[%d]: commonbit = %d ",__func__,__LINE__, commonbit);
#endif

    /* The selected common bit is maybe not supported. */
    if (tableHandle[tableIdx].nBitSize == 0) {
       *pvalue = 0;
       return IFX_SUCCESS;
    }

    /* Check that the selected common bit and the port index do not exceed the 
    maximum supported table size. */
    if (tableIdx > COMMON_BIT_LATEST)
        return IFX_ERROR;

    /* The selected common bit does not fit to the table configuration. */
    if (tableHandle[tableIdx].nCommonBitEnum != commonbit)
    {
        IFXOS_PRINT_INT_RAW("\n%s[%d]: nCommonBitEnum = %d, commonbit = %d\n",
                        __func__,__LINE__, tableHandle[tableIdx].nCommonBitEnum, commonbit);
    
        return IFX_ERROR;
    }

    /* Add the general base address to the register hardware offset to get
    the absolute register address. */
    regAddr = tableHandle[tableIdx].nRegOffset;

    /* Call the Register Access Layer to perform the read operation. Here the callback function pointer are used. */
    if (pDev->register_read(pDev->pRegAccessHandle, regAddr , &readRegValue) != IFX_SUCCESS)
            return IFX_ERROR;
    
#ifdef DEBUG_RML
    IFXOS_PRINT_INT_RAW("\n%s[%d]: readRegValue = 0x%x\n",__func__,__LINE__, readRegValue);
#endif

    /* Prepare the mask for filtering */
    if (tableHandle[tableIdx].nBitSize == 32)
        mask = 0xFFFFFFFF;
    else
        mask = (1 << tableHandle[tableIdx].nBitSize) - 1;

    /* Exract the bit field by usage of masking and bit shifting */
    readRegValue = readRegValue >> tableHandle[tableIdx].nBitPosition;
        
    *pvalue = (readRegValue & mask);
#ifdef DEBUG_RML
    IFXOS_PRINT_INT_RAW("\n%s[%d]: Return Value = 0x%x, mask=0x%x, readRegValue=0x%x\n",__func__,__LINE__, *pvalue, mask, readRegValue);
#endif

#ifdef DEBUG
    IFXOS_PRINT_INT_RAW("DEBUG> %s[%d]: commonbit=%d, offset=%x, pos=%d, size=%d, value=%d\n",__func__,__LINE__, 
                        commonbit, 
                        regAddr, 
                        tableHandle[tableIdx].nBitPosition,
                        tableHandle[tableIdx].nBitSize,
                        *pvalue
                        );
#endif

    return IFX_SUCCESS;
}

/**
* \fn int IFX_ethsw_RML_Write()
* \brief Access read low level register mapper layer
* \param  RML_Dev_t* pCxtHandle
* \param  IFX_ETHSW_regMapperSelector_t commonbit
* \param  IFX_uint32_t portIdx
* \param  IFX_uint32_t value
* \return on Success return data read from register, else return ETHSW_statusErr
*/
IFX_return_t IFX_PSB6970_RML_Write(IFX_void_t *pCxtHandle, 
                            IFX_ETHSW_regMapperSelector_t commonbit,
                            IFX_uint32_t portIdx,
                            IFX_uint32_t value)
{
    IFX_PSB6970_RML_Dev_t * pDev = (IFX_PSB6970_RML_Dev_t*)pCxtHandle;
    IFX_ETHSW_regMapper_t *tableHandle = pDev->tableHandle;
    IFX_uint32_t tableIdx = ((IFX_uint32_t)commonbit) + portIdx;
    IFX_uint32_t readRegValue, regAddr, mask;
    IFX_uint32_t writtenRegValue;
    
#ifdef DEBUG_RML
    IFXOS_PRINT_INT_RAW("\n%s[%d]: commonbit = %d\n",__func__,__LINE__, commonbit);
#endif

    // check nCommonBitEnum -> no action! 
    if ( tableHandle[tableIdx].nCommonBitEnum == 0xFFFF )
        return IFX_SUCCESS;

    /* Check that the selected common bit and the port index do not exceed the 
    maximum supported table size. */
    if (tableIdx > COMMON_BIT_LATEST)
        return IFX_ERROR;
    
    /* The selected common bit does not fit to the table configuration. */
    if (tableHandle[tableIdx].nCommonBitEnum != commonbit)
    {
#ifdef DEBUG_RML
        IFXOS_PRINT_INT_RAW("\n%s[%d]: commonbit = %d, nCommonBitEnum=%d\n",__func__,__LINE__, commonbit, tableHandle[tableIdx].nCommonBitEnum);
#endif
        return IFX_ERROR;
    }
    
    regAddr = tableHandle[tableIdx].nRegOffset;

    /* First read the current register value */
    if (pDev->register_read(pDev->pRegAccessHandle, regAddr , &readRegValue) != IFX_SUCCESS)
        return IFX_ERROR;

    /* Prepare the mask for filtering */
    if (tableHandle[tableIdx].nBitSize == 32)
        mask = 0xFFFFFFFF;
    else
        mask = (1 << tableHandle[tableIdx].nBitSize) - 1;
    mask = mask << tableHandle[tableIdx].nBitPosition;

    /* Shift the value to right place as given by the hardware register bit 
    field and mask out unneeded bits. */
    //value = (value << tableHandle[tableIdx].nBitPosition) & mask;
    writtenRegValue = (value << tableHandle[tableIdx].nBitPosition) & mask;

    /* Mask out the bit field from the read register and place in the new value.*/
    //value = (readRegValue & ~mask) | value;
    writtenRegValue = (readRegValue & ~mask) | writtenRegValue;

    /* Write back the modified register configuration. */
    //if (pDev->register_write(pDev->pRegAccessHandle, regAddr , value) != IFX_SUCCESS)
    if (pDev->register_write(pDev->pRegAccessHandle, regAddr , writtenRegValue) != IFX_SUCCESS)
        return IFX_ERROR;
#ifdef DEBUG
    IFXOS_PRINT_INT_RAW("DEBUG> %s[%d]: cmb=%d, off=%x, pos=%d, sz=%d, wrVal=%d, rdReg=%x, wrReg=%x\n",__func__,__LINE__, 
                        commonbit, 
                        regAddr, 
                        tableHandle[tableIdx].nBitPosition,
                        tableHandle[tableIdx].nBitSize,
                        value,
                        readRegValue,
                        writtenRegValue
                        );
#endif

    return IFX_SUCCESS;

}

/**
   This is the switch rml layer init function

   \param pCxtHandle  This parameter is a pointer to the init handle context.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_void_t *IFX_PSB6970_RML_init(IFX_PSB6970_RML_Init_t *pInit)
{
	IFX_PSB6970_RML_Dev_t *pDev;
	
	pDev = (IFX_PSB6970_RML_Dev_t*) IFXOS_BlockAlloc (sizeof (IFX_PSB6970_RML_Dev_t));
	if(pDev == IFX_NULL) {
		IFXOS_PRINT_INT_RAW("Error : %s memory allocation failed !!\n", __func__);
		return pDev;
	}
	
	pDev->register_write = pInit->register_write;
    pDev->register_read = pInit->register_read;
    pDev->pRegAccessHandle = pInit->pRegAccessHandle;
    pDev->nBaseAddress = pInit->nBaseAddress;

    switch (pInit->eDev) {
    	case IFX_PSB6970_DEV_AR9:
             pDev->tableHandle = regMapper_AR9;
             break;
        case IFX_PSB6970_DEV_TANTOS_3G:
        case IFX_PSB6970_DEV_TANTOS_0G:
             pDev->tableHandle = regMapper_TANTOS_3G;
             break;
        default:
        	pDev->tableHandle = IFX_NULL;
        	IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (RML eDev not found) \n", __FILE__, __FUNCTION__, __LINE__);
        	if (pDev) {
        		kfree(pDev);
        		pDev = IFX_NULL;
        		
        	}
//        	return IFX_NULL;
    }
    return (pDev);
}

