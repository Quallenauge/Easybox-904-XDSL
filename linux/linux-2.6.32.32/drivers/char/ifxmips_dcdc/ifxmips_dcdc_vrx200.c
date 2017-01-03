/******************************************************************************
**
** FILE NAME    : ifx_dcdc_vrx200.c
** PROJECT      : IFX UEIP
** MODULES      : IFX DCDC converter driver
** DATE         : 13 Dec 2010
** AUTHOR       : Sameer Ahmad
** DESCRIPTION  : IFX DCDC Converter driver's Platform specific Callbacks
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
** $Date                $Author                 $Comment
** 13 Dec 2010           Sameer Ahmad
*******************************************************************************/
#include "ifx_dcdc.h"

/* Declaration for the Call back handlers*/
static int ifx_dcdc_vrx200_voltage_set (int voltage_value);
static int ifx_dcdc_vrx200_voltage_get (void);
static int ifx_dcdc_vrx200_DutyCycleMax_set (int Duty_cycle);
static int ifx_dcdc_vrx200_PwmSwitchingFreq_set (int PwmSwitchingFreq);
static int ifx_dcdc_vrx200_NonOvDelay_set (unsigned int Delay_p, unsigned int Delay_n);
static int ifx_dcdc_vrx200_PfmMode_Enable (void);
static int ifx_dcdc_vrx200_PfmMode_Disable (void);
static unsigned int ifx_Voltage_to_DigRef (unsigned int voltage);
static unsigned int ifx_DigRef_to_Voltage (void);

/*Platform dependent callbacks table*/
Ifx_dcdc_platform_callbacks_table Ifx_dcdc_Platform_CallBack_table =  {
       .Ifx_dcdc_voltage_set_cb  = ifx_dcdc_vrx200_voltage_set,
       .Ifx_dcdc_voltage_get_cb  = ifx_dcdc_vrx200_voltage_get,
       .Ifx_dcdc_DutyCycleMax_set_cb =  ifx_dcdc_vrx200_DutyCycleMax_set,
       .Ifx_dcdc_PwmSwitchingFreq_set_cb =  ifx_dcdc_vrx200_PwmSwitchingFreq_set,
       .Ifx_dcdc_NonOvDelay_set_cb =  ifx_dcdc_vrx200_NonOvDelay_set,
       .Ifx_dcdc_PfmModeEnable_cb =  ifx_dcdc_vrx200_PfmMode_Enable,
       .Ifx_dcdc_PfmModeDisable_cb =  ifx_dcdc_vrx200_PfmMode_Disable,
};
/*Pointer which points to the DCDC register file*/
Dcdc_register_map     *DcDcRegisterMAP = (Dcdc_register_map*)
                                          IFX_DCDC_ABSOLUTE_REGISTER_ADDR;
/*Define the table of supported voltage levels
* this should be defined by every platform*/
unsigned int SupportedVoltageLevels[] = {
                                          IFX_DCDC_CORE_VOLTAGE_930,
                                          IFX_DCDC_CORE_VOLTAGE_1000,
                                          IFX_DCDC_CORE_VOLTAGE_1175 };
/*==============================================================================
*  Function:        ifx_dcdc_dump_regs
*  Description :    Debug routine to dump the DCDC converter registers
*  Params:
*  Return Value:
*===============================================================================
*/
void ifx_dcdc_dump_regs(void)
{
/*Dump DCDC converter registers*/

    IFX_DCDC_PRINT("\nB0_Coeh Register \t%x",DcDcRegisterMAP->B0_Coeh);
    IFX_DCDC_PRINT("\nB0_Coel Register \t%x",DcDcRegisterMAP->B0_Coel);
    IFX_DCDC_PRINT("\nB1_Coeh Register \t%x",DcDcRegisterMAP->B1_Coeh);
    IFX_DCDC_PRINT("\nB1_Coel Register \t%x",DcDcRegisterMAP->B1_Coel);
    IFX_DCDC_PRINT("\nB2_Coeh Register \t%x",DcDcRegisterMAP->B2_Coeh);
    IFX_DCDC_PRINT("\nB2_Coel Register \t%x",DcDcRegisterMAP->B2_Coel);
    IFX_DCDC_PRINT("\nClk_Set0 Register \t%x",DcDcRegisterMAP->Clk_Set0.byte);
    IFX_DCDC_PRINT("\nClk_Set1 Register \t%x",DcDcRegisterMAP->Clk_Set1.byte);
    IFX_DCDC_PRINT("\nPWM_Confh Register \t%x",DcDcRegisterMAP->PWM_Confh);
    IFX_DCDC_PRINT("\nPWM_Confl Register \t%x",DcDcRegisterMAP->PWM_Confl);
    IFX_DCDC_PRINT("\nBias_Vreg0 Register \t%x",DcDcRegisterMAP->Bias_Vreg0.byte);
    IFX_DCDC_PRINT("\nBias_Vreg1 Register \t%x",DcDcRegisterMAP->Bias_Vreg1);
    IFX_DCDC_PRINT("\nAdc_Gen0 Register \t%x",DcDcRegisterMAP->Adc_Gen0.byte);
    IFX_DCDC_PRINT("\nAdc_Gen1 Register \t%x",DcDcRegisterMAP->Adc_Gen1.byte);
    IFX_DCDC_PRINT("\nAdc_Con0 Register \t%x",DcDcRegisterMAP->Adc_Con0.byte);
    IFX_DCDC_PRINT("\nAdc_Con1 Register \t%x",DcDcRegisterMAP->Adc_Con1.byte);
    IFX_DCDC_PRINT("\nOsc_Conf Register \t%x",DcDcRegisterMAP->Osc_Conf.byte);
    IFX_DCDC_PRINT("\nOsc_Sta Register \t%x",DcDcRegisterMAP->Osc_Sta.byte);
}
/*==============================================================================
*  Function:        ifx_dcdc_vrx200_voltage_set
*  Description :    function to change the Voltage on VRX200 platform
*  Params:
*      voltage_value :  voltage value to be set
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
static int ifx_dcdc_vrx200_voltage_set (int voltage_value)
{
    unsigned int dig_ref = 0;

     dig_ref = ifx_Voltage_to_DigRef (voltage_value);
     DcDcRegisterMAP->Bias_Vreg1 = dig_ref;

     IFX_DCDC_PRINT("\n value of dig ref reg %x expected %x", DcDcRegisterMAP->Bias_Vreg1, dig_ref);
     return IFX_SUCCESS;
}

/*==============================================================================
*  Function:        ifx_dcdc_vrx200_voltage_get
*  Description :    function to get the Voltage on VRX200 platform
*  Params:
*  Return Value:    Returns current voltage or IFX_ERROR
*===============================================================================
*/
static int ifx_dcdc_vrx200_voltage_get(void)
{
    return ifx_DigRef_to_Voltage ();
}
/*==============================================================================
*  Function:        ifx_dcdc_vrx200_DutyCycleMax_set
*  Description :    function to change the DutyCycleMax on VRX200 platform
*  Params:
*      Duty_cycle : duty cycle  value to be set
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
static int ifx_dcdc_vrx200_DutyCycleMax_set (int Duty_cycle)
{
     DcDcRegisterMAP->Duty_Cycle_max_sat = Duty_cycle;
     return IFX_SUCCESS;
}
/*==============================================================================
*  Function:        ifx_dcdc_vrx200_PwmSwitchingFreq_set
*  Description :    function to change the DutyCycleMax on VRX200 platform
*  Params:
*      PwmSwitchingFreq : PwmSwitchingFreq  value to be set
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
/*NOTE: THE IMPLEMENTATION OF FOLLOWING CB HANDLER IS CURRENTLY IN TODO LIST*/
static int ifx_dcdc_vrx200_PwmSwitchingFreq_set (int PwmSwitchingFreq)
{
     return IFX_SUCCESS;
}
/*==============================================================================
*  Function:        ifx_dcdc_vrx200_NonOvDelay_set
*  Description :    function to change the NonOvDelay on VRX200 platform
*  Params:
*      NonOvDelay : NonOvDelay  value to be set
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
static int ifx_dcdc_vrx200_NonOvDelay_set (unsigned int Delay_p,
                                           unsigned int Delay_n)
{
     DcDcRegisterMAP->Non_OV_Delay.NonOv_Delay.Delay_P = Delay_p;
     DcDcRegisterMAP->Non_OV_Delay.NonOv_Delay.Delay_N = Delay_n;
     return IFX_SUCCESS;
}
/*==============================================================================
*  Function:        ifx_dcdc_vrx200_PfmMode_Enable
*  Description :    function to enable the PFM mode on VRX200 platform
*  Params:
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
static int ifx_dcdc_vrx200_PfmMode_Enable ()
{
    DcDcRegisterMAP->Adc_Gen0.Adc_reg0.pfm_mode = 0x1;
    return IFX_SUCCESS;
}
/*==============================================================================
*  Function:        ifx_dcdc_vrx200_PfmMode_Disable
*  Description :    function to Disable the PFM mode on VRX200 platform
*  Params:
*  Return Value:    IFX_SUCCESS or IFX_ERROR
*===============================================================================
*/
static int ifx_dcdc_vrx200_PfmMode_Disable ()
{
     DcDcRegisterMAP->Adc_Gen0.Adc_reg0.pfm_mode = 0x0;
     return IFX_SUCCESS;
}

unsigned int ifx_Voltage_to_DigRef (unsigned int voltage)
{

    if (voltage > IFX_DIGREF_DEFAULT_VALUE)
        return (IFX_DIGREF_DEFAULT_BITMASK + ((voltage - IFX_DIGREF_DEFAULT_VALUE)/8));
    else
        return (IFX_DIGREF_DEFAULT_BITMASK - ((IFX_DIGREF_DEFAULT_VALUE - voltage)/8));
}

unsigned int ifx_DigRef_to_Voltage ()
{

    return (DcDcRegisterMAP->Bias_Vreg1 * 8);
}
