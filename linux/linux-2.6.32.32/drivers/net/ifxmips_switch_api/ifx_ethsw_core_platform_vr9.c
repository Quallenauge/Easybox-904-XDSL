/****************************************************************************
                               Copyright  2009
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************

   \file ifx_ethsw_core_platform_vr9.c
   \remarks implement VR9 platform specify routine
 *****************************************************************************/

#include <asm/ifx/ifx_rcu.h>
//#include "ifx_ethsw_core_platform.h"
#include <linux/kernel.h> /* printk() */
#include <ifx_ethsw_flow_core.h>
#include <ifx_ethsw_pce.h>
#include <ifx_ethsw_flow_ll.h>
#include <ifx_ethsw_core_platform.h>
IFX_int32_t platform_device_reset_trigger(IFX_void_t)
{
	unsigned int reg;

	reg = ifx_rcu_rst_req_read();
	/* Reset Switch RD21 */
	reg |= ( 1 << 21 );
	ifx_rcu_rst_req_write(reg, reg);
	return IFX_SUCCESS;
}

IFX_int32_t platform_device_reset_release(IFX_void_t)
{
	/* Empty */
	return IFX_SUCCESS;
}

IFX_int32_t platform_device_init(IFX_void_t *pDevCtx)
{
	IFX_uint32_t PHY_ID, Reg_value, chip_id;
/*	IFX_FLOW_switchDev_t *pEthSWDevHandle = (IFX_FLOW_switchDev_t*)pDevCtx; */

	/* Read out the PHY ID and program to switch register */
	PHY_ID = GSWIP_REG_ACCESS(GFMDIO_ADD);

	/* PHY 2 */
	Reg_value = GSWIP_REG_ACCESS(PHY_ADDR_2);
	Reg_value &= ~(0x1F);
	Reg_value |= (PHY_ID & 0x1F);
	GSWIP_REG_ACCESS(PHY_ADDR_2) = Reg_value ;

	/* PHY 3 */
	Reg_value = GSWIP_REG_ACCESS(PHY_ADDR_3);
	Reg_value &= ~(0x1F);
	Reg_value |= ((PHY_ID >> 5) & 0x1F);
	GSWIP_REG_ACCESS(PHY_ADDR_3) = Reg_value ;

	/* PHY 4 */
	Reg_value = GSWIP_REG_ACCESS(PHY_ADDR_4);
	Reg_value &= ~(0x1F);
	Reg_value |= ((PHY_ID >> 10) & 0x1F);
	GSWIP_REG_ACCESS(PHY_ADDR_4) = Reg_value ;
#if defined(CONFIG_FE_MODE)
	/* PHY 5 */
	Reg_value = GSWIP_REG_ACCESS(PHY_ADDR_5);
	Reg_value &= ~(0x1F);
	Reg_value |= ((PHY_ID  >> 15) & 0x1F);
	GSWIP_REG_ACCESS(PHY_ADDR_5) = Reg_value ;
#endif

	PHY_ID = GSWIP_REG_ACCESS(MPS_CHIPID);
	chip_id = ( (PHY_ID & 0xFFF000) >> 12);
	switch ( ( chip_id ) ) {
		case 0x1c0:
		case 0x1c1:
		case 0x1c2:
		case 0x1c8:
		case 0x1c9:
			/* Set Port 6 RBUF_BYPASS mode */
			Reg_value = GSWIP_REG_ACCESS(MAC_PDI_CTRL_6);	
			GSWIP_REG_ACCESS(MAC_PDI_CTRL_6) = Reg_value | ( 1 << 6);
			/* Set Port 6 RBUF_BYPASS mode */
			Reg_value = GSWIP_REG_ACCESS(PMAC_RX_IPG);
			GSWIP_REG_ACCESS(PMAC_RX_IPG) = Reg_value | 0xb;
			break;
	}
	return IFX_SUCCESS;
}



