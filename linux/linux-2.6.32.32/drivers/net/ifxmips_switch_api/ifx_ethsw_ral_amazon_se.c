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
#ifdef AMAZON_SE_TANTOS0G_GPIO
extern int ifx_sw_read(unsigned int addr, unsigned int *dat);
extern int ifx_sw_write(unsigned int addr, unsigned int dat);
#endif

/**
   This is the switch ral layer function for external switch

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_RAL_Dev_t context.
   \param RegAddr  This parameter is a register address.
   \param value  This value get from register. 

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_AMAZON_SE_RAL_Register_MDIORead(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t * value) 
{
#ifdef AMAZON_SE_TANTOS0G_GPIO
    ifx_sw_read(RegAddr, value);
    IFXOS_USecSleep(500);
    return IFX_SUCCESS;
#else
    IFXOS_PRINT_INT_RAW("RAL layer was not supported!\n");
    return IFX_ERROR;
#endif
#if 0
    IFX_PSB6970_RAL_Dev_t *pDev = pDevCtx;
    IFX_uint32_t MDIO_CTL_Register;
    IFX_uint32_t regValue;
    MDIO_CTL_Register = (0x01<<IFX_AMAZON_SE_MDIO_RA_SHIFT) + 
                        (0x01 << IFX_AMAZON_SE_MDIO_RW_SHIFT) + 
                        (RegAddr<<IFX_AMAZON_SE_MDIO_REGADDR_SHIFT);

    REG32_ACCESS(pDev->nBaseAddress) = MDIO_CTL_Register;
    IFXOS_USecSleep(500);
    do {
        regValue = REG32_ACCESS(pDev->nBaseAddress);
    } while ( regValue & 0x80000000);

    regValue = REG32_ACCESS(pDev->nBaseAddress);
    *value = 0x0000FFFF & regValue;

    return IFX_SUCCESS;
#endif
}

/**
   This is the switch ral layer function for external switch

   \param pDevCtx  This parameter is a pointer to the IFX_PSB6970_RAL_Dev_t context.
   \param RegAddr  This parameter is a register address.
   \param value  This value is written to register.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_AMAZON_SE_RAL_Register_MDIOWrite(IFX_void_t *pDevCtx , IFX_int32_t RegAddr, IFX_uint32_t value) 
{
#ifdef AMAZON_SE_TANTOS0G_GPIO
    ifx_sw_write(RegAddr, value);
    IFXOS_USecSleep(500);
    return IFX_SUCCESS;
#else
    IFXOS_PRINT_INT_RAW("RAL layer was not supported!\n");
    return IFX_ERROR;
#endif
#if 0
    IFX_PSB6970_RAL_Dev_t *pDev = pDevCtx;
    IFX_uint32_t MDIO_CTL_Register;

    MDIO_CTL_Register = (0x01<<IFX_AMAZON_SE_MDIO_RA_SHIFT) + 
                        (RegAddr<<IFX_AMAZON_SE_MDIO_REGADDR_SHIFT) +
                        (value) ;
    MDIO_CTL_Register &= ~(0x01 << IFX_AMAZON_SE_MDIO_RW_SHIFT);

    REG32_ACCESS(pDev->nBaseAddress) = MDIO_CTL_Register;
    IFXOS_USecSleep(500);

    return IFX_SUCCESS;
#endif
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
        return IFX_NULL;
    }

    pDev->register_read = &IFX_AMAZON_SE_RAL_Register_MDIORead;
    pDev->register_write = &IFX_AMAZON_SE_RAL_Register_MDIOWrite;
    pDev->nBaseAddress = IFX_AMAZON_SE_PPE_BASEADDR;

    pDev->eDev = pInit->eDev;
    return pDev;
}
