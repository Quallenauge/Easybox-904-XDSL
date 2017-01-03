/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pce.c
   \remarks implement SWITCH API PCE Table micro code
 *****************************************************************************/

/* Implement the PCE Table Read/Write Function */
#include "ifx_ethsw_pce.h"
#include "ifx_switch_ll.h"
#include "VR9_switch.h"
#include "VR9_top.h"

#define PCE_TABLE_DEBUG	0

#define	PCE_ASSERT(t)	if ((t)) { 	\
		IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (" # t ")\n", __FILE__, __FUNCTION__, __LINE__);	\
		return (-1); }
// macro to define a microcode row
#define IFX_FLOW_PCE_MC_M(val, msk, ns, out, len, type, flags, ipv4_len) \
    { val, msk, (ns<<10 | out<<4 | len>>1), (len&1)<<15 | type<<13 | flags<<9 | ipv4_len<<8 }
/* Micro code version V2_11 (extension for parsing IPv6 in PPPoE) */
IFX_FLOW_PCE_MICROCODE pce_mc_max_ifx_tag_m = {
    //------------------------------------------------------------------------------------------
    //                value    mask   ns  out_fields   L  type     flags       ipv4_len
    //------------------------------------------------------------------------------------------
	IFX_FLOW_PCE_MC_M(0x88c3 , 0xFFFF , 1 , OUT_ITAG0 , 4 , INSTR , FLAG_ITAG , 0),
	IFX_FLOW_PCE_MC_M(0x8100 , 0xFFFF , 2 , OUT_VTAG0 , 2 , INSTR , FLAG_VLAN , 0),
	IFX_FLOW_PCE_MC_M(0x88A8 , 0xFFFF , 1 , OUT_VTAG0 , 2 , INSTR , FLAG_VLAN , 0),
	IFX_FLOW_PCE_MC_M(0x8100 , 0xFFFF , 1 , OUT_VTAG0 , 2 , INSTR , FLAG_VLAN , 0),
	IFX_FLOW_PCE_MC_M(0x8864 , 0xFFFF , 17 , OUT_ETHTYP , 1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0800 , 0xFFFF , 21 , OUT_ETHTYP ,	1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x86DD , 0xFFFF , 22 , OUT_ETHTYP ,	1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x8863 , 0xFFFF , 16 , OUT_ETHTYP , 1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0xF800 , 10 , OUT_NONE , 0 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 38 , OUT_ETHTYP ,	1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0600 , 0x0600 , 38 , OUT_ETHTYP ,	1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 12 , OUT_NONE , 1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0xAAAA , 0xFFFF , 14 , OUT_NONE , 1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0300 , 0xFF00 , 39 , OUT_NONE ,		0 , INSTR , FLAG_SNAP , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_DIP7 ,		3 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 18 , OUT_DIP7 , 3 , INSTR , FLAG_PPPOE , 0),
	IFX_FLOW_PCE_MC_M(0x0021 , 0xFFFF , 21 , OUT_NONE ,		1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0057 , 0xFFFF , 22 , OUT_NONE ,		1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x4000 , 0xF000 , 24 , OUT_IP0 ,		4 , INSTR , FLAG_IPV4 , 1),
	IFX_FLOW_PCE_MC_M(0x6000 , 0xF000 , 27 , OUT_IP0 ,		3 , INSTR , FLAG_IPV6 , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 25 , OUT_IP3 ,		2 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 26 , OUT_SIP0 ,		4 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 38 , OUT_NONE ,		0 , LENACCU , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x1100 , 0xFF00 , 37 , OUT_PROT ,		1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0600 , 0xFF00 , 37 , OUT_PROT ,		1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0xFF00 , 33 , OUT_IP3 ,		17, INSTR , FLAG_HOP , 0),
	IFX_FLOW_PCE_MC_M(0x2B00 , 0xFF00 , 33 , OUT_IP3 ,		17, INSTR , FLAG_NN1 , 0),
	IFX_FLOW_PCE_MC_M(0x3C00 , 0xFF00 , 33 , OUT_IP3 ,		17, INSTR , FLAG_NN2 , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 37 , OUT_PROT ,		1 , INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0xFF00 , 33 , OUT_NONE ,		0 , IPV6 ,  FLAG_HOP , 0),
	IFX_FLOW_PCE_MC_M(0x2B00 , 0xFF00 , 33 , OUT_NONE ,		0 , IPV6 ,  FLAG_NN1 , 0),
	IFX_FLOW_PCE_MC_M(0x3C00 , 0xFF00 , 33 , OUT_NONE ,		0 , IPV6 ,  FLAG_NN2 , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 38 , OUT_PROT ,		1 , IPV6 ,  FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 38 , OUT_SIP0 ,		16, INSTR , FLAG_NO , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_APP0 ,		4 , INSTR , FLAG_IGMP , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
	IFX_FLOW_PCE_MC_M(0x0000 , 0x0000 , 39 , OUT_NONE ,		0 , INSTR , FLAG_END , 0),
};

/* Static Function Declaration */
static IFX_int32_t tbl_write(void *pTblStart,IFX_uint16_t *pRefCnt,	\
			void *pPar, IFX_uint32_t TblEntrySize,IFX_uint32_t TblEntryNum)
{
	IFX_int32_t i;
	/* search if the entry is already available and can be re-used */
	for (i = 0; i < TblEntryNum; i++) {
		if (pRefCnt[i] > 0) {
			/* entry is used, check if the entry content fits */
			if (memcmp(((char*)pTblStart) + i * TblEntrySize, pPar, (IFX_uint8_t)TblEntrySize) == 0) {
				/* content is the same, increment reference counter and return the index*/
				pRefCnt[i]++;
				return i;
			}
		}
	}
   /* find an empty entry and add information */
	for (i = 0; i < TblEntryNum; i++) {
		if (pRefCnt[i] == 0) {
         memcpy(((char*)pTblStart) + i * TblEntrySize, pPar, (IFX_uint8_t)TblEntrySize);
         pRefCnt[i]++;
			return i;
		}
      }
   /* table is full, return an error */
   IFX_RETURN_PCETM;
}

static IFX_int32_t tbl_idx_delete(IFX_uint16_t *pRefCnt,IFX_uint32_t index,IFX_uint32_t TblSize)
{
	PCE_ASSERT(index >= TblSize);
   if (pRefCnt[index] > 0)
      pRefCnt[index]--;
	return IFX_SUCCESS;
}

/* Packet Length Table write */
IFX_int32_t ifx_pce_tm_pkg_lng_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_PKG_LNG_TBL_t *pPar)
{
   IFX_void_t *pDevCtx =IFX_NULL;
   IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
   IFX_int_t table_index;

	table_index = tbl_write(pTmHandle->pkg_lng_tbl, pTmHandle->pkg_lng_tbl_cnt,	\
					pPar, sizeof(IFX_PCE_PKG_LNG_TBL_t), IFX_PCE_PKG_LNG_TBL_SIZE);
	if (table_index < 0) return(table_index);
   pcetable.table = IFX_ETHSW_PCE_PACKET_INDEX;
   pcetable.table_index = table_index;
   pcetable.key[0] =pPar->pkg_lng;
   pcetable.mask = pPar->pkg_lng_rng;
   pcetable.valid = 1;
   ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return table_index;
}

/* Packet Length Table delete */
IFX_int32_t ifx_pce_tm_pkg_lng_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_PKG_LNG_TBL_SIZE);
	if (pTmHandle->pkg_lng_tbl_cnt[index] > 0 )
		pTmHandle->pkg_lng_tbl_cnt[index]--;
		
	if (pTmHandle->pkg_lng_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->pkg_lng_tbl) + (index * sizeof(IFX_PCE_PKG_LNG_TBL_t)) ),	\
		 0, sizeof(IFX_PCE_PKG_LNG_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_PACKET_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

/* Packet Length Table read */
IFX_int32_t ifx_pce_tm_pkg_lng_tbl_read(IFX_PCE_HANDLE_t *pTmHandle,IFX_int32_t index,IFX_PCE_PKG_LNG_TBL_t *pPar)
{
	if (index < IFX_PCE_PKG_LNG_TBL_SIZE) {
      memcpy(pPar, &pTmHandle->pkg_lng_tbl[index],sizeof(IFX_PCE_PKG_LNG_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}

/* MAC DA/SA Table index write */
IFX_int32_t ifx_pce_tm_dasa_mac_tbl_write(IFX_PCE_HANDLE_t *pTmHandle,IFX_PCE_DASA_MAC_TBL_t *pPar)
{
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_int_t table_index;
	
	table_index = tbl_write(pTmHandle->dasa_mac_tbl, pTmHandle->dasa_mac_tbl_cnt, pPar, \
					sizeof(IFX_PCE_DASA_MAC_TBL_t), IFX_PCE_DASA_MAC_TBL_SIZE);
	if (table_index < 0) return(table_index);
   pcetable.table = IFX_ETHSW_PCE_MAC_DASA_INDEX;
   pcetable.table_index = table_index;
   pcetable.key[0] =(pPar->mac[4] << 8 | pPar->mac[5]);
   pcetable.key[1] =(pPar->mac[2] << 8 | pPar->mac[3]);
   pcetable.key[2] =(pPar->mac[0] << 8 | pPar->mac[1]);
   pcetable.mask = pPar->mac_mask;
   pcetable.valid = 1;
   ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return table_index;
}

/* MAC DA/SA Table delete */
IFX_int32_t ifx_pce_tm_dasa_mac_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_DASA_MAC_TBL_SIZE);
	if (pTmHandle->dasa_mac_tbl_cnt[index] > 0 )
		pTmHandle->dasa_mac_tbl_cnt[index]--;
		
	if (pTmHandle->dasa_mac_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->dasa_mac_tbl) + (index * sizeof(IFX_PCE_DASA_MAC_TBL_t)) ),	\
		0, sizeof(IFX_PCE_DASA_MAC_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_MAC_DASA_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   }
	return IFX_SUCCESS;
}

/* MAC DA/SA Table Read */
IFX_int32_t ifx_pce_tm_dasa_mac_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index,\
		IFX_PCE_DASA_MAC_TBL_t *pPar)
{
	if (index < IFX_PCE_DASA_MAC_TBL_SIZE) {
   /* Copy mac address */
   memcpy(pPar, &pTmHandle->dasa_mac_tbl[index], sizeof(IFX_PCE_DASA_MAC_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}

/* Application Table write */
IFX_int32_t ifx_pce_tm_appl_tbl_write(IFX_PCE_HANDLE_t *pTmHandle,IFX_PCE_APPL_TBL_t *pPar)
{
   IFX_void_t *pDevCtx =IFX_NULL;
   IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
   IFX_int_t table_index;

	table_index =  tbl_write(pTmHandle->appl_tbl, pTmHandle->appl_tbl_cnt, pPar,	\
				sizeof(IFX_PCE_APPL_TBL_t), IFX_PCE_APPL_TBL_SIZE);
	if (table_index < 0) return(table_index);
   pcetable.table = IFX_ETHSW_PCE_APPLICATION_INDEX;
   pcetable.table_index = table_index;
   pcetable.key[0] =pPar->appl_data;
   pcetable.mask = pPar->mask_range;
   pcetable.valid = 1;
   ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return table_index;
}

/* Application Table Delete */
IFX_int32_t ifx_pce_tm_appl_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_APPL_TBL_SIZE);
	if (pTmHandle->appl_tbl_cnt[index] > 0 )
		pTmHandle->appl_tbl_cnt[index]--;

	if (pTmHandle->appl_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->appl_tbl) + (index * sizeof(IFX_PCE_APPL_TBL_t)) ),	\
		0, sizeof(IFX_PCE_APPL_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_APPLICATION_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

/* Application Table Read */
IFX_int32_t ifx_pce_tm_appl_tbl_read(IFX_PCE_HANDLE_t *pTmHandle,IFX_int32_t index,IFX_PCE_APPL_TBL_t *pPar)
{
	if (index < IFX_PCE_APPL_TBL_SIZE) {
   /* Copy Application Table */
   memcpy(pPar, &pTmHandle->appl_tbl[index], sizeof(IFX_PCE_APPL_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}
/* IP DA/SA msb Table write */
IFX_int32_t ifx_pce_tm_ip_dasa_msb_tbl_write(IFX_PCE_HANDLE_t *pTmHandle,IFX_PCE_IP_DASA_MSB_TBL_t *pPar)
{
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_int_t table_index, i;

	table_index = tbl_write(pTmHandle->ip_dasa_msb_tbl,pTmHandle->ip_dasa_msb_tbl_cnt, pPar,	\
		sizeof(IFX_PCE_IP_DASA_MSB_TBL_t), IFX_PCE_IP_DASA_MSB_TBL_SIZE);
	if (table_index < 0) return(table_index);
	pcetable.table = IFX_ETHSW_PCE_IP_DASA_MSB_INDEX;
	pcetable.table_index = table_index;
	for ( i =0 ; i < 4 ; i++ )
		pcetable.key[i] =((pPar->ip_msb[((i*2)+1)] << 8) |pPar->ip_msb[(i*2)]);
   pcetable.mask = pPar->mask;
   pcetable.valid = 1;
   ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return table_index;
}

/* IP DA/SA msb Table delete */
IFX_int32_t ifx_pce_tm_ip_dasa_msb_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_IP_DASA_MSB_TBL_SIZE);
	if (pTmHandle->ip_dasa_msb_tbl_cnt[index] > 0 )
		pTmHandle->ip_dasa_msb_tbl_cnt[index]--;

	if (pTmHandle->ip_dasa_msb_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->ip_dasa_msb_tbl) + (index * sizeof(IFX_PCE_IP_DASA_MSB_TBL_t)) ),	\
		 0, sizeof(IFX_PCE_IP_DASA_MSB_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_IP_DASA_MSB_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

/* IP DA/SA msb Table read */
IFX_int32_t ifx_pce_tm_ip_dasa_msb_tbl_read(IFX_PCE_HANDLE_t *pTmHandle,IFX_int32_t index,	\
			IFX_PCE_IP_DASA_MSB_TBL_t *pPar)
{
	if (index < IFX_PCE_IP_DASA_MSB_TBL_SIZE) {
   /* Copy mac address */
   memcpy(pPar, &pTmHandle->ip_dasa_msb_tbl[index], sizeof(IFX_PCE_IP_DASA_MSB_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}

/* IP DA/SA lsb Table Write */
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_IP_DASA_LSB_TBL_t *pPar)
{
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_int_t table_index, i;

	table_index = tbl_write(pTmHandle->ip_dasa_lsb_tbl, pTmHandle->ip_dasa_lsb_tbl_cnt, pPar,	\
		 sizeof(IFX_PCE_IP_DASA_LSB_TBL_t), IFX_PCE_IP_DASA_LSB_TBL_SIZE);
	if (table_index < 0) return(table_index);
	pcetable.table = IFX_ETHSW_PCE_IP_DASA_LSB_INDEX;
	pcetable.table_index = table_index;
	for ( i =0 ; i < 4 ; i++ )
		pcetable.key[i] =((pPar->ip_lsb[((i*2)+1)] << 8) |pPar->ip_lsb[(i*2)]);
   pcetable.mask = pPar->mask;
   pcetable.valid = 1;
   ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return table_index;
}

/* IP DA/SA lsb Table delete */
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_IP_DASA_LSB_TBL_SIZE);
	if (pTmHandle->ip_dasa_lsb_tbl_cnt[index] > 0 )
		pTmHandle->ip_dasa_lsb_tbl_cnt[index]--;

	if (pTmHandle->ip_dasa_lsb_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->ip_dasa_lsb_tbl) + (index * sizeof(IFX_PCE_IP_DASA_LSB_TBL_t)) ),	\
		0, sizeof(IFX_PCE_IP_DASA_LSB_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_IP_DASA_LSB_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.key[3] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

/* IP DA/SA lsb Table index delete */
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint32_t index)
{
	return tbl_idx_delete(pTmHandle->ip_dasa_lsb_tbl_cnt, index,IFX_PCE_IP_DASA_LSB_TBL_SIZE);
}

/* IP DA/SA lsb Table read */
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index,	\
		IFX_PCE_IP_DASA_LSB_TBL_t *pPar)
{
	if (index < IFX_PCE_IP_DASA_LSB_TBL_SIZE) {
   /* Copy IP address */
   memcpy(pPar, &pTmHandle->ip_dasa_lsb_tbl[index], sizeof(IFX_PCE_IP_DASA_LSB_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}

/* Protocal Table write */
IFX_int32_t ifx_pce_tm_ptcl_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_PTCL_TBL_t *pPar)
{
  IFX_void_t *pDevCtx =IFX_NULL;
  IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
  IFX_int_t table_index;

	table_index = tbl_write(pTmHandle->ptcl_tbl,pTmHandle->ptcl_tbl_cnt, pPar,	\
		 sizeof(IFX_PCE_PTCL_TBL_t),IFX_PCE_PTCL_TBL_SIZE);
	if (table_index < 0) return(table_index);

   pcetable.table = IFX_ETHSW_PCE_PROTOCOL_INDEX;
   pcetable.table_index = table_index;
   /* Should We need to check the entertype ? */
   pcetable.key[0] =pPar->key.ethertype;
   pcetable.mask = pPar->mask.ethertype_mask;
   pcetable.valid = 1;
   ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return table_index;
}
/* Get the vlan flow table index*/
IFX_int32_t get_ifx_pce_tm_vlan_act_tbl_index(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint8_t index)
{
	PCE_ASSERT(index >= IFX_PCE_VLAN_ACT_TBL_SIZE);
	if (pTmHandle->vlan_act_tbl_cnt[index] == 0 )
		return IFX_SUCCESS;
	else
		return IFX_ERROR;
}
/* Protocal Table delete */
IFX_int32_t ifx_pce_tm_ptcl_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_PTCL_TBL_SIZE);
	if (pTmHandle->ptcl_tbl_cnt[index] > 0 )
		pTmHandle->ptcl_tbl_cnt[index]--;
		
	if (pTmHandle->ptcl_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->ptcl_tbl) + (index * sizeof(IFX_PCE_PTCL_TBL_t)) ),	\
		0, sizeof(IFX_PCE_PTCL_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_PROTOCOL_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.key[3] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

/* Protocal Table Read */
IFX_int32_t ifx_pce_tm_ptcl_tbl_read(IFX_PCE_HANDLE_t *pTmHandle,IFX_int32_t index,IFX_PCE_PTCL_TBL_t *pPar)
{
	if (index < IFX_PCE_PTCL_TBL_SIZE) {
   /* Copy mac address */
   memcpy(pPar, &pTmHandle->ptcl_tbl[index], sizeof(IFX_PCE_PTCL_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}
/* PPPoE Table Write */
IFX_int32_t ifx_pce_tm_pppoe_tbl_write(IFX_PCE_HANDLE_t *pTmHandle,IFX_PCE_PPPOE_TBL_t *pPar)
{
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_int_t table_index;

	table_index = tbl_write(pTmHandle->pppoe_tbl,pTmHandle->pppoe_tbl_cnt, pPar,
		sizeof(IFX_PCE_PPPOE_TBL_t), IFX_PCE_PPPOE_TBL_SIZE);
	if (table_index < 0) return table_index; 
	pcetable.table = IFX_ETHSW_PCE_PPPOE_INDEX;
	pcetable.table_index = table_index;
   pcetable.key[0] =pPar->sess_id;
	pcetable.valid = 1;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return table_index;
}

/* PPPoE Table Delete */
IFX_int32_t ifx_pce_tm_pppoe_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	PCE_ASSERT(index >= IFX_PCE_PPPOE_TBL_SIZE);
	if (pTmHandle->pppoe_tbl_cnt[index] > 0 )
		pTmHandle->pppoe_tbl_cnt[index]--;

	if (pTmHandle->pppoe_tbl_cnt[index] == 0) {
		memset( (((char*)pTmHandle->pppoe_tbl) + (index * sizeof(IFX_PCE_PPPOE_TBL_t)) ),	\
		0, sizeof(IFX_PCE_PPPOE_TBL_t));
		pcetable.table = IFX_ETHSW_PCE_PPPOE_INDEX;
		pcetable.table_index = index;
		pcetable.key[0] = 0;
		pcetable.key[1] = 0;
		pcetable.key[2] = 0;
		pcetable.key[3] = 0;
		pcetable.mask = 0;
		pcetable.valid = 0;
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	}
	return IFX_SUCCESS;
}

/* PPPoE Table Read */
IFX_int32_t ifx_pce_tm_pppoe_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index,IFX_PCE_PPPOE_TBL_t *pPar)
{
	if (index < IFX_PCE_PPPOE_TBL_SIZE) {
	/* Copy VLAN Action Table*/
		memcpy(pPar, &pTmHandle->pppoe_tbl[index], sizeof(IFX_PCE_PPPOE_TBL_t));
		return IFX_SUCCESS;
	}
	return IFX_ERROR;
}

/* VLAN Table Delete */
IFX_int32_t ifx_pce_tm_vlan_act_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint32_t index)
{
	PCE_ASSERT(index >= IFX_PCE_VLAN_ACT_TBL_SIZE);
	if (pTmHandle->vlan_act_tbl_cnt[index] > 0 )
		pTmHandle->vlan_act_tbl_cnt[index]--;
	
	return IFX_SUCCESS;
}

IFX_int32_t ifx_pce_tm_find_vlan_id_index (IFX_PCE_HANDLE_t *pTmHandle,IFX_uint16_t vid ) 
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable_vlan;
	IFX_int32_t	table_index, vid_index = 0x7F;
	IFX_void_t *pDevCtx =IFX_NULL;
	
	/* Read out the real VLAN action table and  find valid vlan id*/
	for (table_index = 0; table_index < IFX_PCE_VLAN_ACT_TBL_SIZE ; table_index++) {
		pcetable_vlan.table = IFX_ETHSW_PCE_ACTVLAN_INDEX;
		pcetable_vlan.table_index = table_index; /*index of the VLAN ID configuration */
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable_vlan);
		if ( pcetable_vlan.valid == IFX_TRUE ) {
			if ( pcetable_vlan.key[0] == vid ) {
				vid_index = table_index;
				pTmHandle->vlan_act_tbl_cnt[table_index]++;
				break;
			}
		}
	}
	return (vid_index);
}


/* PCE Table Init routine */
IFX_int32_t ifx_pce_table_init(IFX_PCE_t *pPCEHandle)
{
	IFX_int32_t i;

	if (pPCEHandle == IFX_NULL)
		return IFX_ERROR;
	memset(&pPCEHandle->pce_sub_tbl, 0, sizeof(IFX_PCE_HANDLE_t));
	memset(&pPCEHandle->pce_tbl, 0, sizeof(IFX_PCE_TBL_t));
	memset(&pPCEHandle->pce_act, 0, sizeof(IFX_FLOW_PCE_action_t));
	for (i = 0;i < IFX_PCE_TBL_SIZE ;i++) {
		pPCEHandle->pce_tbl_used[i] = 0;
	}
	return IFX_SUCCESS;
}

/* PCE Table Micro Code Init routine */
IFX_return_t IFX_VR9_Switch_PCE_Micro_Code_Int()
{
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_uint8_t i,j;
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t tbl_entry;

	/* Disable all physical port - FDMA_PCTRL.EN & SDMA_PCTRL.PEN */
	for (j =0; j < NUM_OF_PORTS_INCLUDE_CPU_PORT;j++) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_FDMA_PCTRL_EN_OFFSET + (j * 0x6)),	\
                               VR9_FDMA_PCTRL_EN_SHIFT, VR9_FDMA_PCTRL_EN_SIZE, 0);
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_SDMA_PCTRL_PEN_OFFSET + (j * 0x6)),	\
                               VR9_SDMA_PCTRL_PEN_SHIFT, VR9_SDMA_PCTRL_PEN_SIZE, 0);
   }
	/* Set GLOB_CTRL.SE */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_GLOB_CTRL_SE_OFFSET + 0xC40),	\
                                 VR9_GLOB_CTRL_SE_SHIFT, VR9_GLOB_CTRL_SE_SIZE, 1);

	/* Download the microcode to the hardware */
	for (i = 0; i < IFX_FLOW_PCE_MICROCODE_VALUES; i++) {
		memset(&tbl_entry, 0, sizeof(tbl_entry));
		tbl_entry.val[3] = pce_mc_max_ifx_tag_m[i].val_3;
		tbl_entry.val[2] = pce_mc_max_ifx_tag_m[i].val_2;
		tbl_entry.val[1] = pce_mc_max_ifx_tag_m[i].val_1;
		tbl_entry.val[0] = pce_mc_max_ifx_tag_m[i].val_0;
		tbl_entry.table_index = i;
		tbl_entry.table = 0; // PARSER
		ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &tbl_entry);
	}

	/* PCE parser microcode is valid */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_GCTRL_0_MC_VALID_OFFSET,	\
         VR9_PCE_GCTRL_0_MC_VALID_SHIFT, VR9_PCE_GCTRL_0_MC_VALID_SIZE, 0x1);

	/* UTP: SMS01358977 - Default unknown Multicast/Unicast*/
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PMAP_2_DMCPMAP_OFFSET,	\
		VR9_PCE_PMAP_2_DMCPMAP_SHIFT, VR9_PCE_PMAP_2_DMCPMAP_SIZE, 0x7F);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_PMAP_3_UUCMAP_OFFSET,	\
		VR9_PCE_PMAP_3_UUCMAP_SHIFT, VR9_PCE_PMAP_3_UUCMAP_SIZE, 0x7F);

	/* Overwrite the default configuration needed by Switch */
	/* RMON Counter Enable for all physical ports */
	for (j = 0; j < NUM_OF_PORTS_INCLUDE_CPU_PORT; j++) {
		ifx_ethsw_ll_DirectAccessWrite(pDevCtx, (VR9_BM_PCFG_CNTEN_OFFSET + (j * 0x2)),	\
			VR9_BM_PCFG_CNTEN_SHIFT, VR9_BM_PCFG_CNTEN_SIZE, 1);
	}
   
	/* The Threshold bit is set to local threshold by default */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_BM_QUEUE_GCTRL_GL_MOD_OFFSET,	\
		VR9_BM_QUEUE_GCTRL_GL_MOD_SHIFT, VR9_BM_QUEUE_GCTRL_GL_MOD_SIZE, 0);
	return IFX_SUCCESS;
}

IFX_return_t ifx_ethsw_xwayflow_pce_table_write(IFX_void_t *pDevCtx, IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t *pData)
{
    IFX_uint32_t value;
    IFX_uint16_t udata;

	do {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
			VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, &value);
	} while (value); /* check if hardware is ready (PCE_TBL_CTRL.BAS) */
	/* prepare the table access: PCE_TBL_ADDR.ADDR = pData->table_index */
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_ADDR_ADDR_OFFSET,	\
		VR9_PCE_TBL_ADDR_ADDR_SHIFT, VR9_PCE_TBL_ADDR_ADDR_SIZE, pData->table_index);
    udata = pData->table;
	// PCE_TBL_CTRL.ADDR = pData->table
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_ADDR_OFFSET,	\
        VR9_PCE_TBL_CTRL_ADDR_SHIFT, VR9_PCE_TBL_CTRL_ADDR_SIZE, udata);
  // PCE_TBL_CTRL.OPMOD = ADWR //(address-based write)
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_OPMOD_OFFSET,	\
        VR9_PCE_TBL_CTRL_OPMOD_SHIFT, VR9_PCE_TBL_CTRL_OPMOD_SIZE, IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_ADWR);
  /* Need to change the code style -3M */
  // PCE_TBL_KEY_'X'.KEY'X' = pData->key['X'] //done for all key values
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_7_KEY7_OFFSET,	\
    VR9_PCE_TBL_KEY_7_KEY7_SHIFT, VR9_PCE_TBL_KEY_7_KEY7_SIZE, pData->key[7]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_6_KEY6_OFFSET,	\
    VR9_PCE_TBL_KEY_6_KEY6_SHIFT, VR9_PCE_TBL_KEY_6_KEY6_SIZE, pData->key[6]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_5_KEY5_OFFSET,	\
		VR9_PCE_TBL_KEY_5_KEY5_SHIFT, VR9_PCE_TBL_KEY_5_KEY5_SIZE, pData->key[5]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_4_KEY4_OFFSET,	\
		VR9_PCE_TBL_KEY_4_KEY4_SHIFT, VR9_PCE_TBL_KEY_4_KEY4_SIZE, pData->key[4]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_3_KEY3_OFFSET,	\
		VR9_PCE_TBL_KEY_3_KEY3_SHIFT, VR9_PCE_TBL_KEY_3_KEY3_SIZE, pData->key[3]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_2_KEY2_OFFSET,	\
		VR9_PCE_TBL_KEY_2_KEY2_SHIFT, VR9_PCE_TBL_KEY_2_KEY2_SIZE, pData->key[2]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_1_KEY1_OFFSET,	\
		VR9_PCE_TBL_KEY_1_KEY1_SHIFT, VR9_PCE_TBL_KEY_1_KEY1_SIZE, pData->key[1]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_0_KEY0_OFFSET,	\
		VR9_PCE_TBL_KEY_0_KEY0_SHIFT, VR9_PCE_TBL_KEY_0_KEY0_SIZE, pData->key[0]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_MASK_0_MASK0_OFFSET,	\
		VR9_PCE_TBL_MASK_0_MASK0_SHIFT, VR9_PCE_TBL_MASK_0_MASK0_SIZE, pData->mask);
	// PCE_TBL_VAL_'X' = pData->val['X'] //done for all values
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_4_VAL4_OFFSET,	\
		VR9_PCE_TBL_VAL_4_VAL4_SHIFT, VR9_PCE_TBL_VAL_4_VAL4_SIZE, pData->val[4]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_3_VAL3_OFFSET,	\
		VR9_PCE_TBL_VAL_3_VAL3_SHIFT, VR9_PCE_TBL_VAL_3_VAL3_SIZE, pData->val[3]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_2_VAL2_OFFSET,	\
		VR9_PCE_TBL_VAL_2_VAL2_SHIFT, VR9_PCE_TBL_VAL_2_VAL2_SIZE, pData->val[2]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_1_VAL1_OFFSET,	\
		VR9_PCE_TBL_VAL_1_VAL1_SHIFT, VR9_PCE_TBL_VAL_1_VAL1_SIZE, pData->val[1]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_0_VAL0_OFFSET,	\
		VR9_PCE_TBL_VAL_0_VAL0_SHIFT, VR9_PCE_TBL_VAL_0_VAL0_SIZE, pData->val[0]);

#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
	IFXOS_PRINT_INT_RAW("pData->key[0] = %x\n", pData->key[0]);
	IFXOS_PRINT_INT_RAW("pData->key[1] = %x\n", pData->key[1]);
	IFXOS_PRINT_INT_RAW("pData->key[2] = %x\n", pData->key[2]);
	IFXOS_PRINT_INT_RAW("pData->key[3] = %x\n", pData->key[3]);
	IFXOS_PRINT_INT_RAW("pData->key[4] = %x\n", pData->key[4]);
	IFXOS_PRINT_INT_RAW("pData->key[5] = %x\n", pData->key[5]);
	IFXOS_PRINT_INT_RAW("pData->key[6] = %x\n", pData->key[6]);
	IFXOS_PRINT_INT_RAW("pData->key[7] = %x\n", pData->key[7]);
	IFXOS_PRINT_INT_RAW("pData->val[0] = %x\n", pData->val[0]);
	IFXOS_PRINT_INT_RAW("pData->val[1] = %x\n", pData->val[1]);
	IFXOS_PRINT_INT_RAW("pData->val[2] = %x\n", pData->val[2]);
	IFXOS_PRINT_INT_RAW("pData->val[3] = %x\n", pData->val[3]);
	IFXOS_PRINT_INT_RAW("pData->val[4] = %x\n", pData->val[4]);
#endif /*PCE_TABLE_DEBUG */

	// PCE_TBL_CTRL.TYPE = pData->type
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_TYPE_OFFSET,	\
		VR9_PCE_TBL_CTRL_TYPE_SHIFT, VR9_PCE_TBL_CTRL_TYPE_SIZE, pData->type);

	// PCE_TBL_CTRL.VLD = pData->valid
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_VLD_OFFSET,	\
		VR9_PCE_TBL_CTRL_VLD_SHIFT, VR9_PCE_TBL_CTRL_VLD_SIZE, pData->valid);

	// PCE_TBL_CTRL.GMAP'X' = pData->group'X' //done for all groups
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_GMAP_OFFSET,	\
		VR9_PCE_TBL_CTRL_GMAP_SHIFT, VR9_PCE_TBL_CTRL_GMAP_SIZE, pData->group);

	// start the table access:
	// PCE_TBL_CTRL.BAS = BUSY //(start the write access)
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
		VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, IFX_TRUE);

	do {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
			 VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, &value);
	} while (value != 0); // check if hardware is ready
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,0, 15, &value);
//    IFXOS_PRINT_INT_RAW("DEBUG: The 0x44F data = [0x%x]\n",value);

    return IFX_SUCCESS;
}

IFX_return_t ifx_ethsw_xwayflow_pce_table_read(IFX_void_t *pDevCtx, IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t *pData)
{
	IFX_uint32_t value;
	
	/* check if ready: */
	// PCE_TBL_CTRL.BAS
	do {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
			VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, &value);
	} while (value != 0); // check if hardware is ready
	// prepare the table access:
	// PCE_TBL_ADDR.ADDR = pData->table_index
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_ADDR_ADDR_OFFSET,	\
		VR9_PCE_TBL_ADDR_ADDR_SHIFT, VR9_PCE_TBL_ADDR_ADDR_SIZE, pData->table_index);
	// PCE_TBL_CTRL.ADDR = pData->table
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_ADDR_OFFSET,	\
		VR9_PCE_TBL_CTRL_ADDR_SHIFT, VR9_PCE_TBL_CTRL_ADDR_SIZE, pData->table);
  // PCE_TBL_CTRL.OPMOD = ADRD //(address-based read)
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_OPMOD_OFFSET,	\
        VR9_PCE_TBL_CTRL_OPMOD_SHIFT, VR9_PCE_TBL_CTRL_OPMOD_SIZE, IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_ADRD);
  // start the table access:
  // PCE_TBL_CTRL.BAS = BUSY //(start the read access)
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
    VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, IFX_TRUE);
	//check if ready:
	/*do {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_STAT_TBUSY_OFFSET,	\
			VR9_PCE_TBL_STAT_TBUSY_SHIFT, VR9_PCE_TBL_STAT_TBUSY_SIZE, &value);
	} while (value != 0); // check if hardware is ready
    */
    do {
    	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
    		VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, &value);
    } while (value != 0); // check if hardware is ready
  // evaluate the results:
  // pData->key['X'] = PCE_TBL_KEY_'X'.KEY'X' //done for all key values
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_7_KEY7_OFFSET,	\
    VR9_PCE_TBL_KEY_7_KEY7_SHIFT, VR9_PCE_TBL_KEY_7_KEY7_SIZE, &value);
    pData->key[7] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_6_KEY6_OFFSET,	\
    VR9_PCE_TBL_KEY_6_KEY6_SHIFT, VR9_PCE_TBL_KEY_6_KEY6_SIZE, &value);
    pData->key[6] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_5_KEY5_OFFSET,	\
    VR9_PCE_TBL_KEY_5_KEY5_SHIFT, VR9_PCE_TBL_KEY_5_KEY5_SIZE, &value);
    pData->key[5] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_4_KEY4_OFFSET,	\
    VR9_PCE_TBL_KEY_4_KEY4_SHIFT, VR9_PCE_TBL_KEY_4_KEY4_SIZE, &value);
    pData->key[4] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_3_KEY3_OFFSET,	\
    VR9_PCE_TBL_KEY_3_KEY3_SHIFT, VR9_PCE_TBL_KEY_3_KEY3_SIZE, &value);
    pData->key[3] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_2_KEY2_OFFSET,	\
    VR9_PCE_TBL_KEY_2_KEY2_SHIFT, VR9_PCE_TBL_KEY_2_KEY2_SIZE, &value);
    pData->key[2] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_1_KEY1_OFFSET,	\
    VR9_PCE_TBL_KEY_1_KEY1_SHIFT, VR9_PCE_TBL_KEY_1_KEY1_SIZE, &value);
    pData->key[1] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_KEY_0_KEY0_OFFSET,	\
    VR9_PCE_TBL_KEY_0_KEY0_SHIFT, VR9_PCE_TBL_KEY_0_KEY0_SIZE, &value);
    pData->key[0] = value;
  // pData->val['X'] = PCE_TBL_VAL_'X' //done for all values
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_VAL_4_VAL4_OFFSET,	\
    VR9_PCE_TBL_VAL_4_VAL4_SHIFT, VR9_PCE_TBL_VAL_4_VAL4_SIZE, &value);
    pData->val[4] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_VAL_3_VAL3_OFFSET,	\
    VR9_PCE_TBL_VAL_3_VAL3_SHIFT, VR9_PCE_TBL_VAL_3_VAL3_SIZE, &value);
    pData->val[3] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_VAL_2_VAL2_OFFSET,	\
    VR9_PCE_TBL_VAL_2_VAL2_SHIFT, VR9_PCE_TBL_VAL_2_VAL2_SIZE, &value);
    pData->val[2] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_VAL_1_VAL1_OFFSET,	\
    VR9_PCE_TBL_VAL_1_VAL1_SHIFT, VR9_PCE_TBL_VAL_1_VAL1_SIZE, &value);
    pData->val[1] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_VAL_0_VAL0_OFFSET,	\
    VR9_PCE_TBL_VAL_0_VAL0_SHIFT, VR9_PCE_TBL_VAL_0_VAL0_SIZE, &value);
    pData->val[0] = value;
    ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_MASK_0_MASK0_OFFSET,	\
    VR9_PCE_TBL_MASK_0_MASK0_SHIFT, VR9_PCE_TBL_MASK_0_MASK0_SIZE, &value);
    pData->mask = value;
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
  IFXOS_PRINT_INT_RAW("pData->key[0] = %x\n", pData->key[0]);
  IFXOS_PRINT_INT_RAW("pData->key[1] = %x\n", pData->key[1]);
  IFXOS_PRINT_INT_RAW("pData->key[2] = %x\n", pData->key[2]);
  IFXOS_PRINT_INT_RAW("pData->key[3] = %x\n", pData->key[3]);
  IFXOS_PRINT_INT_RAW("pData->key[4] = %x\n", pData->key[4]);
  IFXOS_PRINT_INT_RAW("pData->key[5] = %x\n", pData->key[5]);
  IFXOS_PRINT_INT_RAW("pData->key[6] = %x\n", pData->key[6]);
  IFXOS_PRINT_INT_RAW("pData->key[7] = %x\n", pData->key[7]);
  IFXOS_PRINT_INT_RAW("pData->val[0] = %x\n", pData->val[0]);
  IFXOS_PRINT_INT_RAW("pData->val[1] = %x\n", pData->val[1]);
  IFXOS_PRINT_INT_RAW("pData->val[2] = %x\n", pData->val[2]);
  IFXOS_PRINT_INT_RAW("pData->val[3] = %x\n", pData->val[3]);
  IFXOS_PRINT_INT_RAW("pData->val[4] = %x\n", pData->val[4]);
#endif /*PCE_TABLE_DEBUG */
  // pData->type = PCE_TBL_CTRL.TYPE
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_TYPE_OFFSET,	\
    VR9_PCE_TBL_CTRL_TYPE_SHIFT, VR9_PCE_TBL_CTRL_TYPE_SIZE, &value);
    pData->type = value;
  // pData->valid = PCE_TBL_CTRL.VLD
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_VLD_OFFSET,	\
    VR9_PCE_TBL_CTRL_VLD_SHIFT, VR9_PCE_TBL_CTRL_VLD_SIZE, &value);
    pData->valid = value;
  // pData->group'X' = PCE_TBL_CTRL.GMAP'X' //done for all groups
	ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_GMAP_OFFSET,	\
    VR9_PCE_TBL_CTRL_GMAP_SHIFT, VR9_PCE_TBL_CTRL_GMAP_SIZE, &value);
    pData->group = value;
    return IFX_SUCCESS;
}

IFX_return_t ifx_ethsw_xwayflow_pce_table_cam_write(IFX_void_t *pDevCtx, IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t *pData)
{
    IFX_uint32_t value;

	/* check if ready: */
	do {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET, \
             VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, &value);
	} while (value != 0); /*  check if hardware is ready */
	
	/* prepare the table access: */
	/* PCE_TBL_ADDR.ADDR = pData->table_index 
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_ADDR_ADDR_OFFSET,
			VR9_PCE_TBL_ADDR_ADDR_SHIFT, VR9_PCE_TBL_ADDR_ADDR_SIZE, pData->table_index);
	PCE_TBL_CTRL.ADDR = pData->table
	*/
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_ADDR_OFFSET,	\
			VR9_PCE_TBL_CTRL_ADDR_SHIFT, VR9_PCE_TBL_CTRL_ADDR_SIZE, pData->table);
	/*  PCE_TBL_CTRL.OPMOD = KSWR */ //(Key-based search write access)
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_OPMOD_OFFSET,	\
        VR9_PCE_TBL_CTRL_OPMOD_SHIFT, VR9_PCE_TBL_CTRL_OPMOD_SIZE, IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_KSWR);
	// PCE_TBL_KEY_'X'.KEY'X' = pData->key['X'] //done for all key values
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_7_KEY7_OFFSET,	\
			VR9_PCE_TBL_KEY_7_KEY7_SHIFT, VR9_PCE_TBL_KEY_7_KEY7_SIZE, pData->key[7]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_6_KEY6_OFFSET,	\
			VR9_PCE_TBL_KEY_6_KEY6_SHIFT, VR9_PCE_TBL_KEY_6_KEY6_SIZE, pData->key[6]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_5_KEY5_OFFSET,	\
			VR9_PCE_TBL_KEY_5_KEY5_SHIFT, VR9_PCE_TBL_KEY_5_KEY5_SIZE, pData->key[5]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_4_KEY4_OFFSET,	\
			VR9_PCE_TBL_KEY_4_KEY4_SHIFT, VR9_PCE_TBL_KEY_4_KEY4_SIZE, pData->key[4]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_3_KEY3_OFFSET,	\
			VR9_PCE_TBL_KEY_3_KEY3_SHIFT, VR9_PCE_TBL_KEY_3_KEY3_SIZE, pData->key[3]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_2_KEY2_OFFSET,	\
			VR9_PCE_TBL_KEY_2_KEY2_SHIFT, VR9_PCE_TBL_KEY_2_KEY2_SIZE, pData->key[2]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_1_KEY1_OFFSET,	\
			VR9_PCE_TBL_KEY_1_KEY1_SHIFT, VR9_PCE_TBL_KEY_1_KEY1_SIZE, pData->key[1]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_KEY_0_KEY0_OFFSET,	\
			VR9_PCE_TBL_KEY_0_KEY0_SHIFT, VR9_PCE_TBL_KEY_0_KEY0_SIZE, pData->key[0]);
			
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_MASK_0_MASK0_OFFSET,	\
			VR9_PCE_TBL_MASK_0_MASK0_SHIFT, VR9_PCE_TBL_MASK_0_MASK0_SIZE, pData->mask);
	// PCE_TBL_VAL_'X' = pData->val['X'] //done for all values
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_4_VAL4_OFFSET,	\
			VR9_PCE_TBL_VAL_4_VAL4_SHIFT, VR9_PCE_TBL_VAL_4_VAL4_SIZE, pData->val[4]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_3_VAL3_OFFSET,	\
			VR9_PCE_TBL_VAL_3_VAL3_SHIFT, VR9_PCE_TBL_VAL_3_VAL3_SIZE, pData->val[3]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_2_VAL2_OFFSET,	\
			VR9_PCE_TBL_VAL_2_VAL2_SHIFT, VR9_PCE_TBL_VAL_2_VAL2_SIZE, pData->val[2]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_1_VAL1_OFFSET,	\
			VR9_PCE_TBL_VAL_1_VAL1_SHIFT, VR9_PCE_TBL_VAL_1_VAL1_SIZE, pData->val[1]);
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_VAL_0_VAL0_OFFSET,	\
			VR9_PCE_TBL_VAL_0_VAL0_SHIFT, VR9_PCE_TBL_VAL_0_VAL0_SIZE, pData->val[0]);
	// PCE_TBL_CTRL.TYPE = pData->type
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_TYPE_OFFSET,	\
			VR9_PCE_TBL_CTRL_TYPE_SHIFT, VR9_PCE_TBL_CTRL_TYPE_SIZE, pData->type);
	// PCE_TBL_CTRL.VLD = pData->valid
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_VLD_OFFSET,	\
			VR9_PCE_TBL_CTRL_VLD_SHIFT, VR9_PCE_TBL_CTRL_VLD_SIZE, pData->valid);
	
	// PCE_TBL_CTRL.GMAP'X' = pData->group'X' //done for all groups
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_GMAP_OFFSET,	\
			VR9_PCE_TBL_CTRL_GMAP_SHIFT, VR9_PCE_TBL_CTRL_GMAP_SIZE, pData->group);
	// start the table access:
	// PCE_TBL_CTRL.BAS = BUSY //(start the write access)
	ifx_ethsw_ll_DirectAccessWrite(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET,	\
			VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, IFX_TRUE);
	do {
		ifx_ethsw_ll_DirectAccessRead(pDevCtx, VR9_PCE_TBL_CTRL_BAS_OFFSET, \
             VR9_PCE_TBL_CTRL_BAS_SHIFT, VR9_PCE_TBL_CTRL_BAS_SIZE, &value);
	} while (value != 0); /*  check if hardware is ready */
	return IFX_SUCCESS;
}

IFX_int32_t ifx_pce_action_delete(IFX_PCE_t *pHandle, IFX_uint32_t index)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_void_t *pDevCtx =IFX_NULL;
	PCE_ASSERT(index >= IFX_PCE_TBL_SIZE);
	memset(&pHandle->pce_act[index], 0, sizeof(IFX_FLOW_PCE_action_t));
	/* REMOVE RULE ACTION FROM HARDWARE DEVICE */
	pcetable.table_index = index;    /*index of the Traffic Flow Table index configuration */
	pcetable.table = IFX_ETHSW_PCE_TFLOW_INDEX;
	ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable);
	if (pcetable.valid == IFX_TRUE) {
		if ( ( (pcetable.val[0] >> 1) & 0x1 ) == IFX_TRUE) {
			IFX_uint32_t index =(pcetable.val[2] & 0x3F) ;
			if (pHandle->pce_sub_tbl.vlan_act_tbl_cnt[index] > 0)
				pHandle->pce_sub_tbl.vlan_act_tbl_cnt[index]--;
		}
	}
	pcetable.valid = 0;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
	return IFX_SUCCESS;
}

IFX_int32_t ifx_pce_pattern_delete(IFX_PCE_t *pHandle, IFX_uint32_t index)
{
	IFX_PCE_TBL_t  *pFet;

	PCE_ASSERT(index >= IFX_PCE_TBL_SIZE);
	/* Check if an entry is currently programmed and remove that one. */
	if (pHandle->pce_tbl_used[index] == 0)
		return IFX_SUCCESS;
	pFet = &(pHandle->pce_tbl[index]);
#define IFX_PCE_TM_IDX_DELETE(x, y, z) if (x != y) \
	if (0 != z( &pHandle->pce_sub_tbl, y)) IFX_RETURN_PCE;

	/* Packet length */
	IFX_PCE_TM_IDX_DELETE(0x1F, pFet->pkt_lng_idx, ifx_pce_tm_pkg_lng_tbl_delete)
	/* Destination MAC address */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->dst_mac_addr_idx,ifx_pce_tm_dasa_mac_tbl_delete)
	/* Source MAC address */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->src_mac_addr_idx,ifx_pce_tm_dasa_mac_tbl_delete)
	/* Destination Application field */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->dst_appl_fld_idx, ifx_pce_tm_appl_tbl_delete)
	/* Source Application field */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->src_appl_fld_idx, ifx_pce_tm_appl_tbl_delete)
	/* DIP MSB */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->dip_msb_idx, ifx_pce_tm_ip_dasa_msb_tbl_delete)
	/* DIP LSB */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->dip_lsb_idx, ifx_pce_tm_ip_dasa_lsb_tbl_delete)
	/* SIP MSB */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->sip_msb_idx, ifx_pce_tm_ip_dasa_msb_tbl_delete)
	/* SIP LSB */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->sip_lsb_idx, ifx_pce_tm_ip_dasa_lsb_tbl_delete)
	/* IP protocol */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->ip_prot_idx, ifx_pce_tm_ptcl_tbl_delete)
	/* Ethertype */
	IFX_PCE_TM_IDX_DELETE(0xFF, pFet->ethertype_idx, ifx_pce_tm_ptcl_tbl_delete)
	/* PPPoE */
	IFX_PCE_TM_IDX_DELETE(0x1F, pFet->pppoe_idx, ifx_pce_tm_pppoe_tbl_delete)
   /* VLAN */
	IFX_PCE_TM_IDX_DELETE(0x7F, pFet->vlan_idx, ifx_pce_tm_vlan_act_tbl_delete)
	/* reset the flag that this rule line is used */
	pHandle->pce_tbl_used[index] = 0;
	/* reset the rule line */
	memset(pFet, 0xFF, sizeof(IFX_PCE_TBL_t));
	/* WRITE DOWN THE NEW LINE TO THE HARDWARE */
	/* also remove the action for this rule */
	return ifx_pce_action_delete(pHandle, index);
}

IFX_int32_t ifx_pce_rule_read(IFX_PCE_t *pHandle, IFX_FLOW_PCE_rule_t *pPar)
{
	IFX_uint32_t   i, idx = pPar->pattern.nIndex;
	IFX_PCE_TBL_t  *pFet;

	PCE_ASSERT(idx >= IFX_PCE_TBL_SIZE);
	/* initialize to zero the structure before writing the parameters */
	memset(pPar, 0, sizeof(IFX_FLOW_PCE_rule_t));
	pPar->pattern.nIndex = idx;
	if (pHandle->pce_tbl_used[idx] == 0) {
		return IFX_SUCCESS;
	} else {
		pPar->pattern.bEnable = IFX_TRUE;
	}
	pFet = &(pHandle->pce_tbl[idx]);
	/* Port ID */
	if (pFet->port_id == 0xFF)
		pPar->pattern.nPortId = 0;
	else
		pPar->pattern.nPortId = (IFX_uint8_t)pFet->port_id;
	/* Port ID is assigned then the Port ID is Enable */
	if (pFet->port_id != 0xFF)
		pPar->pattern.bPortIdEnable = IFX_TRUE;
	/* todo: check how pPar->bDscp should be used or is needed */
	/* DSCP value */
	if (pFet->dscp == 0x7F)
		pPar->pattern.nDSCP = 0;
	else
		pPar->pattern.nDSCP = (IFX_uint8_t)pFet->dscp;
	/* DSCP is assigned and Enable */
	if (pFet->dscp != 0x7F)
		pPar->pattern.bDSCP_Enable = IFX_TRUE;
	/* todo: check how pPar->bPcp should be used or is needed */
	/* PCP value */
	if (pFet->pcp == 0xF)
		pPar->pattern.nPCP = 0;
	else
		pPar->pattern.nPCP = (IFX_uint8_t)pFet->pcp;
	/* PCP is assigned and Enable */
	if (pFet->pcp != 0xF)
		pPar->pattern.bPCP_Enable = IFX_TRUE;
	if (pFet->pkt_lng_idx != 0x1F) {
		IFX_PCE_PKG_LNG_TBL_t pkg_lng_tbl;
		/* Packet length used */
		pPar->pattern.bPktLngEnable = IFX_TRUE;
		if (IFX_SUCCESS != ifx_pce_tm_pkg_lng_tbl_read(&pHandle->pce_sub_tbl,pFet->pkt_lng_idx,&pkg_lng_tbl))
			IFX_RETURN_PCE;
		/* Packet length */
		pPar->pattern.nPktLng = pkg_lng_tbl.pkg_lng;
		/* Packet length Range */
		pPar->pattern.nPktLngRange = pkg_lng_tbl.pkg_lng_rng;
	}
	if (pFet->dst_mac_addr_idx != 0xFF) {
		IFX_PCE_DASA_MAC_TBL_t dasa_mac_tbl;
    	if (IFX_SUCCESS!= ifx_pce_tm_dasa_mac_tbl_read(&pHandle->pce_sub_tbl,pFet->dst_mac_addr_idx,&dasa_mac_tbl))
    		IFX_RETURN_PCE;
    	/* Destination MAC address used */
    	pPar->pattern.bMAC_DstEnable = IFX_TRUE;
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
     	 IFXOS_PRINT_INT_RAW(" DestMAC = %02x:%02x:%02x:%02x:%02x:%02x | idx = %d\n",
        	 dasa_mac_tbl.mac[0], dasa_mac_tbl.mac[1], dasa_mac_tbl.mac[2], dasa_mac_tbl.mac[3],
        	 dasa_mac_tbl.mac[4], dasa_mac_tbl.mac[5], pFet->dst_mac_addr_idx );
#endif /*PCE_TABLE_DEBUG */
		/* Destination MAC address */
		for (i = 0; i < 6; i++)
			pPar->pattern.nMAC_Dst[i] = dasa_mac_tbl.mac[i];
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
      IFXOS_PRINT_INT_RAW(" DestMAC = %02x:%02x:%02x:%02x:%02x:%02x \n",
        	 pPar->pattern.nMAC_Dst[0], pPar->pattern.nMAC_Dst[1], pPar->pattern.nMAC_Dst[2], pPar->pattern.nMAC_Dst[3],
        	 pPar->pattern.nMAC_Dst[4], pPar->pattern.nMAC_Dst[5]);
#endif /*PCE_TABLE_DEBUG */
      /* Destination MAC address mask */
		pPar->pattern.nMAC_DstMask = dasa_mac_tbl.mac_mask;
	}
	if (pFet->src_mac_addr_idx != 0xFF) {
		IFX_PCE_DASA_MAC_TBL_t dasa_mac_tbl;
		if (IFX_SUCCESS != ifx_pce_tm_dasa_mac_tbl_read(&pHandle->pce_sub_tbl,pFet->src_mac_addr_idx,&dasa_mac_tbl))
			IFX_RETURN_PCE;
		/* Destination MAC address used */
		pPar->pattern.bMAC_SrcEnable = IFX_TRUE;
		/* Destination MAC address */
		for (i = 0; i < 6; i++)
			pPar->pattern.nMAC_Src[i] = dasa_mac_tbl.mac[i];
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
		IFXOS_PRINT_INT_RAW(" DestMAC = %02x:%02x:%02x:%02x:%02x:%02x \n",
        	 pPar->pattern.nMAC_Src[0], pPar->pattern.nMAC_Src[1], pPar->pattern.nMAC_Src[2], pPar->pattern.nMAC_Src[3],
        	 pPar->pattern.nMAC_Src[4], pPar->pattern.nMAC_Src[5]);
#endif /*PCE_TABLE_DEBUG */
		/* Destination MAC address mask */
		pPar->pattern.nMAC_SrcMask = dasa_mac_tbl.mac_mask;
	}
	if (pFet->dst_appl_fld_idx != 0xFF) {
		IFX_PCE_APPL_TBL_t appl_tbl;
		if (IFX_SUCCESS != ifx_pce_tm_appl_tbl_read(&pHandle->pce_sub_tbl,pFet->dst_appl_fld_idx,&appl_tbl))
			IFX_RETURN_PCE;
		/* Destination Application used */
		pPar->pattern.bAppDataMSB_Enable = IFX_TRUE;
		/* Destination Application field */
		pPar->pattern.nAppDataMSB = appl_tbl.appl_data;
		/* Destination Application mask/range used */
		pPar->pattern.bAppMaskRangeMSB_Select = IFX_TRUE;
		/* Destination Application mask/range */
		pPar->pattern.nAppMaskRangeMSB = appl_tbl.mask_range;
	}
	if (pFet->src_appl_fld_idx != 0xFF) {
		IFX_PCE_APPL_TBL_t appl_tbl;
		if (IFX_SUCCESS != ifx_pce_tm_appl_tbl_read(&pHandle->pce_sub_tbl,pFet->src_appl_fld_idx,&appl_tbl))
			IFX_RETURN_PCE;
		/* Source Application used */
		pPar->pattern.bAppDataLSB_Enable = IFX_TRUE;
		/* Source Application field */
		pPar->pattern.nAppDataLSB = appl_tbl.appl_data;
		/* Destination Application mask/range used */
		pPar->pattern.bAppMaskRangeLSB_Select = IFX_TRUE;
		/* Source Application mask/range */
		pPar->pattern.nAppMaskRangeLSB = appl_tbl.mask_range;
	}

	if ( (pFet->dip_msb_idx != 0xFF) && (pFet->dip_lsb_idx != 0xFF) ) { /*for IPv6  */
		IFX_PCE_IP_DASA_MSB_TBL_t dasa_tbl;
		IFX_PCE_IP_DASA_LSB_TBL_t dasa_tbl_lsb;
		/* DIP MSB used */
		pPar->pattern.eDstIP_Select = IFX_FLOW_PCE_IP_V6;
		if (IFX_SUCCESS!= ifx_pce_tm_ip_dasa_msb_tbl_read(&pHandle->pce_sub_tbl, pFet->dip_msb_idx,&dasa_tbl))
			IFX_RETURN_PCE;
		for (i = 0; i < 4; i++) {
			pPar->pattern.nDstIP.nIPv6[i] = ( (dasa_tbl.ip_msb[(i*2)] << 8) | dasa_tbl.ip_msb[(i*2)+1] );
		}
		/* DIP MSB Nibble Mask */
		pPar->pattern.nDstIP_Mask = dasa_tbl.mask;
		
		/* DIP LSB used */
	/*	pPar->pattern.eDstIP_Select = IFX_FLOW_PCE_IP_V6; */
		if (IFX_SUCCESS != ifx_pce_tm_ip_dasa_lsb_tbl_read(&pHandle->pce_sub_tbl,pFet->dip_lsb_idx,&dasa_tbl_lsb))
			IFX_RETURN_PCE;
		for (i = 0; i < 4; i++) {
			pPar->pattern.nDstIP.nIPv6[i+4] = (dasa_tbl_lsb.ip_lsb[(i*2)] << 8) | dasa_tbl_lsb.ip_lsb[((i*2)+1)];
		}
		/* DIP LSB Nibble Mask */
		pPar->pattern.nDstIP_Mask = dasa_tbl_lsb.mask;
	} else if (pFet->dip_lsb_idx != 0xFF) {  /*For IPv4 only */
		IFX_PCE_IP_DASA_LSB_TBL_t dasa_tbl;
		/* DIP LSB used */
		pPar->pattern.eDstIP_Select = IFX_FLOW_PCE_IP_V4;
		if (IFX_SUCCESS != ifx_pce_tm_ip_dasa_lsb_tbl_read(&pHandle->pce_sub_tbl,pFet->dip_lsb_idx,&dasa_tbl))
			IFX_RETURN_PCE;
		/* DIP LSB */
		pPar->pattern.nDstIP.nIPv4 = (dasa_tbl.ip_lsb[0] | (dasa_tbl.ip_lsb[1] << 8) \
									 | (dasa_tbl.ip_lsb[2] << 16) | (dasa_tbl.ip_lsb[3] << 24));
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
		IFXOS_PRINT_INT_RAW(" DstIP = %03d.%03d.%03d.%03d \n",
        	 pPar->pattern.nDstIP.nIPv4 & 0xFF, (pPar->pattern.nDstIP.nIPv4 >> 8 ) & 0xFF,
        	 (pPar->pattern.nDstIP.nIPv4 >> 16 ) & 0xFF, (pPar->pattern.nDstIP.nIPv4 >> 24 ) & 0xFF);
#endif /* PCE_TABLE_DEBUG */
      /* DIP LSB Nibble Mask */
		pPar->pattern.nDstIP_Mask = dasa_tbl.mask;
	}
	if ( (pFet->sip_msb_idx != 0xFF) && (pFet->sip_lsb_idx != 0xFF) ) { /* for IPv6 */
		IFX_PCE_IP_DASA_MSB_TBL_t dasa_tbl;
		IFX_PCE_IP_DASA_LSB_TBL_t dasa_tbl_lsb;
		/* SIP MSB used */
		pPar->pattern.eSrcIP_Select = IFX_FLOW_PCE_IP_V6;
		if (IFX_SUCCESS != ifx_pce_tm_ip_dasa_msb_tbl_read(&pHandle->pce_sub_tbl,pFet->sip_msb_idx,&dasa_tbl))
			IFX_RETURN_PCE;
		/* SIP MSB */
		for (i = 0; i < 4; i++) {
			pPar->pattern.nSrcIP.nIPv6[i] = (dasa_tbl.ip_msb[(i*2)] << 8) | dasa_tbl.ip_msb[(i*2)+1];
		}
		/* SIP MSB Nibble Mask */
		pPar->pattern.nSrcIP_Mask = dasa_tbl.mask;
		/* SIP LSB used */
		pPar->pattern.eSrcIP_Select = IFX_FLOW_PCE_IP_V4;  
		if (IFX_SUCCESS != ifx_pce_tm_ip_dasa_lsb_tbl_read(&pHandle->pce_sub_tbl, pFet->sip_lsb_idx, &dasa_tbl_lsb))
			IFX_RETURN_PCE;
		for (i = 0; i < 4; i++) {
			pPar->pattern.nSrcIP.nIPv6[i+4] = (dasa_tbl_lsb.ip_lsb[(i*2)] << 8) | dasa_tbl_lsb.ip_lsb[((i*2)+1)];
		}
		/* SIP LSB Nibble Mask */
		pPar->pattern.nSrcIP_Mask = dasa_tbl_lsb.mask;
	} else if (pFet->sip_lsb_idx != 0xFF) { /* for IPv4 only */
		IFX_PCE_IP_DASA_LSB_TBL_t dasa_tbl;
		/* SIP LSB used */
		pPar->pattern.eSrcIP_Select = IFX_FLOW_PCE_IP_V4;
		if (IFX_SUCCESS != ifx_pce_tm_ip_dasa_lsb_tbl_read(&pHandle->pce_sub_tbl, pFet->sip_lsb_idx, &dasa_tbl))
			IFX_RETURN_PCE;
		/* SIP LSB */
		pPar->pattern.nSrcIP.nIPv4 = (dasa_tbl.ip_lsb[0] | (dasa_tbl.ip_lsb[1] << 8) \
									| (dasa_tbl.ip_lsb[2] << 16)| (dasa_tbl.ip_lsb[3] << 24));
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
      IFXOS_PRINT_INT_RAW(" SrcIP = %03d.%03d.%03d.%03d \n",
        	 pPar->pattern.nSrcIP.nIPv4 & 0xFF, (pPar->pattern.nSrcIP.nIPv4 >> 8 ) & 0xFF,
        	 (pPar->pattern.nSrcIP.nIPv4 >> 16 ) & 0xFF, (pPar->pattern.nSrcIP.nIPv4 >> 24 ) & 0xFF);
#endif /* PCE_TABLE_DEBUG */
      /* SIP LSB Nibble Mask */
      pPar->pattern.nSrcIP_Mask = dasa_tbl.mask;
   }
	if (pFet->ethertype_idx != 0xFF) {
		IFX_PCE_PTCL_TBL_t   pctl_tbl;
		/* Ethertype used */
		pPar->pattern.bEtherTypeEnable = IFX_TRUE;
		if (IFX_SUCCESS != ifx_pce_tm_ptcl_tbl_read(&pHandle->pce_sub_tbl, pFet->ethertype_idx, &pctl_tbl))
			IFX_RETURN_PCE;
		/* Ethertype */
		pPar->pattern.nEtherType = pctl_tbl.key.ethertype;
		/* Ethertype Mask */
		pPar->pattern.nEtherTypeMask = pctl_tbl.mask.ethertype_mask;
	}
	if (pFet->ip_prot_idx != 0xFF) {
		IFX_PCE_PTCL_TBL_t   pctl_tbl;
		/* IP protocol used */
		pPar->pattern.bProtocolEnable = IFX_TRUE;
		if (IFX_SUCCESS!= ifx_pce_tm_ptcl_tbl_read(&pHandle->pce_sub_tbl, pFet->ip_prot_idx, &pctl_tbl))
			IFX_RETURN_PCE;
		/* IP protocol */
		pPar->pattern.nProtocol = (IFX_uint8_t)pctl_tbl.key.prot.protocol;
		/* IP protocol Mask */
		pPar->pattern.nProtocolMask = (IFX_uint8_t)pctl_tbl.mask.prot.protocol_mask;
	}
	if (pFet->pppoe_idx != 0x1F) {
		IFX_PCE_PPPOE_TBL_t pppoe_tbl;
		/* PPPoE used */
		pPar->pattern.bSessionIdEnable = IFX_TRUE;
		if (IFX_SUCCESS!= ifx_pce_tm_pppoe_tbl_read(&pHandle->pce_sub_tbl, pFet->pppoe_idx, &pppoe_tbl))
			IFX_RETURN_PCE;
		/* PPPoE */
		pPar->pattern.nSessionId = pppoe_tbl.sess_id;
	}
	if (pFet->vlan_idx != 0x7F) {
		IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable_vlan;
		IFX_void_t *pDevCtx =IFX_NULL;
		/* VLAN used */
		pPar->pattern.bVid = IFX_TRUE;
		pcetable_vlan.table = IFX_ETHSW_PCE_ACTVLAN_INDEX;
		pcetable_vlan.table_index = pFet->vlan_idx; /*index of the VLAN ID configuration */
		/* pcetable.key[0] = pPar->nVId; */
		ifx_ethsw_xwayflow_pce_table_read(pDevCtx, &pcetable_vlan);
      if ( pcetable_vlan.valid == IFX_TRUE )
         pPar->pattern.nVid = pcetable_vlan.key[0] & 0xFF;
	}
	/* Copy whole action table into action structure */
	memcpy(&pPar->action, &pHandle->pce_act[idx], sizeof(IFX_FLOW_PCE_action_t));
	return IFX_SUCCESS;
}

IFX_int32_t ifx_pce_rule_write(IFX_PCE_t *pHandle, IFX_FLOW_PCE_rule_t *pPar)
{
	IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t pcetable;
	IFX_uint32_t   idx = pPar->pattern.nIndex;
	IFX_void_t *pDevCtx =IFX_NULL;
	IFX_PCE_TBL_t  *pFet;
	IFX_FLOW_PCE_action_t *pAct;
	IFX_FLOW_PCE_rule_t	key_current;
	IFX_int32_t	tab_index,i;

	PCE_ASSERT(idx >= IFX_PCE_TBL_SIZE);

	if (pPar->pattern.bEnable == IFX_FALSE) {
		/* Entry to delete. */
		return ifx_pce_pattern_delete(pHandle, idx);
	}
	 /* first read the current value context from the table to compare if
	 the new configuration is the same. */
	 key_current.pattern.nIndex = idx;
	 /* This key is already programmed on the hardware, no need to update
	 the current hardware programming. */
	 if (ifx_pce_rule_read(pHandle, &key_current) != IFX_SUCCESS)	
	 	IFX_RETURN_PCE;
	 if (!memcmp(&key_current, pPar, sizeof(IFX_FLOW_PCE_rule_t))) {
	 	return 0;
	}

	memset(&pcetable, 0, sizeof(IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t));
	/* Delete the old entry before adding the new one. */
	if (pHandle->pce_tbl_used[idx] != 0) {
		if (ifx_pce_pattern_delete(pHandle, idx) != 0) 
			IFX_RETURN_PCE;
	}
	/* Mark the entry as taken already and then program it. */
	pHandle->pce_tbl_used[idx] = 1;
	/* Now convert the parameter and add to the table. */
	pFet = &(pHandle->pce_tbl[idx]);
	/* Port ID */
	if (pPar->pattern.bPortIdEnable == 1)
		pFet->port_id = pPar->pattern.nPortId;
	else
		pFet->port_id = 0xFF;
	pcetable.key[0] = pFet->port_id;
	/* DSCP value used */
	if (pPar->pattern.bDSCP_Enable == IFX_TRUE) {
		pFet->dscp = pPar->pattern.nDSCP;
	} else {
		pFet->dscp = 0x7F;
	}
	pcetable.key[6] |= (pFet->dscp << 8);
	/* PCP value used */
	if (pPar->pattern.bPCP_Enable == IFX_TRUE) {
		/* PCP value */
		pFet->pcp = pPar->pattern.nPCP;
	} else {
		pFet->pcp = 0xF;
	}
	pcetable.key[6] |=  pFet->pcp;
	/* Packet length used */
	if (pPar->pattern.bPktLngEnable == IFX_TRUE) {
		IFX_PCE_PKG_LNG_TBL_t   pkg_lng;
		pkg_lng.pkg_lng = pPar->pattern.nPktLng; /* Packet length */
		pkg_lng.pkg_lng_rng = pPar->pattern.nPktLngRange; /* Packet length range, in number of bytes */
		tab_index = ifx_pce_tm_pkg_lng_tbl_write(&pHandle->pce_sub_tbl,&pkg_lng);
		if (tab_index < 0) 
			return(tab_index);
		pFet->pkt_lng_idx = tab_index;
	} else {
		pFet->pkt_lng_idx =  0x1F;
	}
	pcetable.key[7] = pFet->pkt_lng_idx;
	/* Destination MAC address used */
	if (pPar->pattern.bMAC_DstEnable == IFX_TRUE) {
		IFX_PCE_DASA_MAC_TBL_t  dasa_mac_tbl;
		for (i = 0; i < 6; i++)
			dasa_mac_tbl.mac[i] = pPar->pattern.nMAC_Dst[i]; /* Destination MAC address */
		dasa_mac_tbl.mac_mask = pPar->pattern.nMAC_DstMask; /* Destination MAC address mask */
		tab_index = ifx_pce_tm_dasa_mac_tbl_write(&pHandle->pce_sub_tbl,&dasa_mac_tbl);
		if (tab_index < 0) 
			return(tab_index);
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
      IFXOS_PRINT_INT_RAW(" DestMAC = %02x:%02x:%02x:%02x:%02x:%02x | tab_index=%d\n",
        	 dasa_mac_tbl.mac[0], dasa_mac_tbl.mac[1], dasa_mac_tbl.mac[2], dasa_mac_tbl.mac[3],
        	 dasa_mac_tbl.mac[4], dasa_mac_tbl.mac[5],tab_index);
#endif /* PCE_TABLE_DEBUG */
      pFet->dst_mac_addr_idx = tab_index;
	} else {
   	  pFet->dst_mac_addr_idx = 0xFF;
	}
	pcetable.key[5] |=  (pFet->dst_mac_addr_idx << 8);
	/* Source MAC address used */
	if (pPar->pattern.bMAC_SrcEnable == IFX_TRUE) {
		IFX_PCE_DASA_MAC_TBL_t  dasa_mac_tbl;
		for (i = 0; i < 6; i++)
			dasa_mac_tbl.mac[i] = pPar->pattern.nMAC_Src[i];	/* Source MAC address */
		/* Source MAC address mask */
		dasa_mac_tbl.mac_mask = pPar->pattern.nMAC_SrcMask;
		tab_index = ifx_pce_tm_dasa_mac_tbl_write(&pHandle->pce_sub_tbl,&dasa_mac_tbl);
		if (tab_index < 0)
			return(tab_index);
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
		IFXOS_PRINT_INT_RAW(" SrcMAC = %02x:%02x:%02x:%02x:%02x:%02x \n",
        	 dasa_mac_tbl.mac[0], dasa_mac_tbl.mac[1], dasa_mac_tbl.mac[2], dasa_mac_tbl.mac[3],
        	 dasa_mac_tbl.mac[4], dasa_mac_tbl.mac[5]);
#endif /* PCE_TABLE_DEBUG */
		pFet->src_mac_addr_idx = tab_index;
	}  else {
		pFet->src_mac_addr_idx = 0xFF;
	}
	pcetable.key[5] |= pFet->src_mac_addr_idx;
	/* Destination Application used */
	if (pPar->pattern.bAppDataMSB_Enable == IFX_TRUE) {
		IFX_PCE_APPL_TBL_t   appl_tbl;
		appl_tbl.appl_data = pPar->pattern.nAppDataMSB; /* Destination Application field */
		appl_tbl.mask_range = pPar->pattern.nAppMaskRangeMSB;  /* Destination Application mask/range */
		tab_index = ifx_pce_tm_appl_tbl_write(&pHandle->pce_sub_tbl, &appl_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->dst_appl_fld_idx = tab_index;
	} else {
		pFet->dst_appl_fld_idx = 0xFF;
	}
	pcetable.key[4] |= pFet->dst_appl_fld_idx;
	/* Source Application field used */
	if (pPar->pattern.bAppDataLSB_Enable == IFX_TRUE) { 
		IFX_PCE_APPL_TBL_t   appl_tbl;
		appl_tbl.appl_data = pPar->pattern.nAppDataLSB; /* Source Application field */
		appl_tbl.mask_range = pPar->pattern.nAppMaskRangeLSB;  /* Source Application mask/range */
		tab_index = ifx_pce_tm_appl_tbl_write(&pHandle->pce_sub_tbl, &appl_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->src_appl_fld_idx = tab_index;
	} else {
		pFet->src_appl_fld_idx = 0xFF;
	}
	pcetable.key[4] |= (pFet->src_appl_fld_idx << 8);

	/* DIP MSB used */
	if (pPar->pattern.eDstIP_Select == /*2*/IFX_FLOW_PCE_IP_V6 ) {
		IFX_PCE_IP_DASA_MSB_TBL_t  dasa_msb_tbl;
		IFX_PCE_IP_DASA_LSB_TBL_t  dasa_lsb_tbl;
		for (i = 0; i < 4; i++) {
			dasa_msb_tbl.ip_msb[(i*2)]		= (pPar->pattern.nDstIP.nIPv6[i] & 0xFF);
			dasa_msb_tbl.ip_msb[(i*2)+1]	= ((pPar->pattern.nDstIP.nIPv6[i] >> 8) & 0xFF);
		}
		dasa_msb_tbl.mask = pPar->pattern.nDstIP_Mask;/* DIP MSB Nibble Mask */
		tab_index = ifx_pce_tm_ip_dasa_msb_tbl_write(&pHandle->pce_sub_tbl,&dasa_msb_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->dip_msb_idx = tab_index;
		
		for (i = 0; i < 4; i++) {
			dasa_lsb_tbl.ip_lsb[(i*2)]		= (pPar->pattern.nDstIP.nIPv6[i+4] & 0xFF);
			dasa_lsb_tbl.ip_lsb[(i*2)+1]	= ((pPar->pattern.nDstIP.nIPv6[i+4] >> 8) & 0xFF);
		}
		dasa_lsb_tbl.mask = pPar->pattern.nDstIP_Mask; /* DIP LSB Nibble Mask */
		tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pHandle->pce_sub_tbl,&dasa_lsb_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->dip_lsb_idx = (IFX_uint16_t)tab_index;
	} else {
		pFet->dip_msb_idx = 0xFF;
		pFet->dip_lsb_idx = 0xFF;
	}
	pcetable.key[3] |= (pFet->dip_msb_idx << 8);
	/* DIP LSB used */
	if (pPar->pattern.eDstIP_Select == IFX_FLOW_PCE_IP_V4) {
		IFX_PCE_IP_DASA_LSB_TBL_t  dasa_lsb_tbl;
		/* DIP LSB */
		dasa_lsb_tbl.ip_lsb[0] = ( pPar->pattern.nDstIP.nIPv4 & 0xFF);
		dasa_lsb_tbl.ip_lsb[1] = ( (pPar->pattern.nDstIP.nIPv4 >> 8 )  & 0xFF );
		dasa_lsb_tbl.ip_lsb[2] = ((pPar->pattern.nDstIP.nIPv4 >> 16 ) & 0xFF );
		dasa_lsb_tbl.ip_lsb[3] = ((pPar->pattern.nDstIP.nIPv4 >> 24 ) & 0xFF);
		dasa_lsb_tbl.mask = pPar->pattern.nDstIP_Mask; /* DIP LSB Nibble Mask */
		tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pHandle->pce_sub_tbl,&dasa_lsb_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->dip_lsb_idx = (IFX_uint16_t)tab_index;
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
      IFXOS_PRINT_INT_RAW(" DstIP = %03d.%03d.%03d.%03d \n",
        	 dasa_lsb_tbl.ip_lsb[3], dasa_lsb_tbl.ip_lsb[2], dasa_lsb_tbl.ip_lsb[1], dasa_lsb_tbl.ip_lsb[0]);
#endif /* PCE_TABLE_DEBUG */
	} else {
		pFet->dip_lsb_idx = 0xFF;
	}
	pcetable.key[3] |= pFet->dip_lsb_idx;
		/* SIP MSB used */
	if (pPar->pattern.eSrcIP_Select == IFX_FLOW_PCE_IP_V6) {
		IFX_PCE_IP_DASA_MSB_TBL_t  dasa_msb_tbl;
		IFX_PCE_IP_DASA_LSB_TBL_t  dasa_lsb_tbl;
		for (i = 0; i < 4; i++) {
			dasa_msb_tbl.ip_msb[(i*2)]		= (pPar->pattern.nSrcIP.nIPv6[i] & 0xFF);
			dasa_msb_tbl.ip_msb[(i*2)+1]	= ((pPar->pattern.nSrcIP.nIPv6[i] >> 8) & 0xFF);
		}
		/* SIP MSB Nibble Mask */
		dasa_msb_tbl.mask = pPar->pattern.nSrcIP_Mask;
		tab_index = ifx_pce_tm_ip_dasa_msb_tbl_write(&pHandle->pce_sub_tbl, &dasa_msb_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->sip_msb_idx = (IFX_uint16_t)tab_index;

		for (i = 0; i < 4; i++) {
			dasa_lsb_tbl.ip_lsb[(i*2)]		= (pPar->pattern.nSrcIP.nIPv6[i+4] & 0xFF);
			dasa_lsb_tbl.ip_lsb[(i*2)+1]	= ((pPar->pattern.nSrcIP.nIPv6[i+4] >> 8) & 0xFF);
		}
		/* SIP LSB Nibble Mask */
		dasa_lsb_tbl.mask = pPar->pattern.nSrcIP_Mask;
		tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pHandle->pce_sub_tbl,&dasa_lsb_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->sip_lsb_idx = (IFX_uint16_t)tab_index;
	} else {
		pFet->sip_msb_idx = 0xFF;
		pFet->sip_lsb_idx = 0xFF;
	}
	pcetable.key[2] |= (pFet->sip_msb_idx << 8);
	/* SIP LSB used */
	if (pPar->pattern.eSrcIP_Select == IFX_FLOW_PCE_IP_V4) {
		IFX_PCE_IP_DASA_LSB_TBL_t  dasa_lsb_tbl;
		dasa_lsb_tbl.ip_lsb[0] = pPar->pattern.nSrcIP.nIPv4 & 0xFF;
		dasa_lsb_tbl.ip_lsb[1] = (pPar->pattern.nSrcIP.nIPv4 >> 8 )  & 0xFF;
		dasa_lsb_tbl.ip_lsb[2] = (pPar->pattern.nSrcIP.nIPv4 >> 16 ) & 0xFF;
		dasa_lsb_tbl.ip_lsb[3] = (pPar->pattern.nSrcIP.nIPv4 >> 24 ) & 0xFF;
		/* SIP LSB Nibble Mask */
		dasa_lsb_tbl.mask = pPar->pattern.nSrcIP_Mask;
		tab_index = ifx_pce_tm_ip_dasa_lsb_tbl_write(&pHandle->pce_sub_tbl,&dasa_lsb_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->sip_lsb_idx = (IFX_uint16_t)tab_index;
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
      IFXOS_PRINT_INT_RAW(" DstIP = %03d.%03d.%03d.%03d \n",
        	 dasa_lsb_tbl.ip_lsb[3], dasa_lsb_tbl.ip_lsb[2], dasa_lsb_tbl.ip_lsb[1], dasa_lsb_tbl.ip_lsb[0]);
 #endif /* PCE_TABLE_DEBUG */
	} else {
		pFet->sip_lsb_idx = 0xFF;
	}
	pcetable.key[2] |= pFet->sip_lsb_idx;
	/* Ethertype used */
	if (pPar->pattern.bEtherTypeEnable == IFX_TRUE) {
		IFX_PCE_PTCL_TBL_t   pctl_tbl;
		pctl_tbl.key.ethertype = pPar->pattern.nEtherType;
		pctl_tbl.mask.ethertype_mask = pPar->pattern.nEtherTypeMask;
		tab_index = ifx_pce_tm_ptcl_tbl_write(&pHandle->pce_sub_tbl, &pctl_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->ethertype_idx = (IFX_uint16_t)tab_index;
	} else {
		pFet->ethertype_idx = 0xFF;
	}
	pcetable.key[1] |= pFet->ethertype_idx;
	/* IP protocol used */
	if (pPar->pattern.bProtocolEnable == IFX_TRUE) {
		IFX_PCE_PTCL_TBL_t   pctl_tbl;
		pctl_tbl.key.prot.protocol = pPar->pattern.nProtocol;
		pctl_tbl.mask.prot.protocol_mask = pPar->pattern.nProtocolMask;
		/* todo: define API parameter for the flags. */
		pctl_tbl.key.prot.protocol_flags = 0xFF;
		/* todo: define API parameter for the flags. */
		pctl_tbl.mask.prot.protocol_flag_mask = 0x3;
		tab_index = ifx_pce_tm_ptcl_tbl_write(&pHandle->pce_sub_tbl, &pctl_tbl);
		if (tab_index < 0) 
			return(tab_index);
		pFet->ip_prot_idx = (IFX_uint16_t)tab_index;
	} else {
		pFet->ip_prot_idx = 0xFF;
	}
	pcetable.key[1] |= (pFet->ip_prot_idx << 8);
	/* PPPoE used */
	if (pPar->pattern.bSessionIdEnable == IFX_TRUE) {
		IFX_PCE_PPPOE_TBL_t  pppoe_tbl;
		pppoe_tbl.sess_id = pPar->pattern.nSessionId;
		tab_index = ifx_pce_tm_pppoe_tbl_write( &pHandle->pce_sub_tbl, (IFX_PCE_PPPOE_TBL_t *)pPar);
		if (tab_index < 0) 
			return(tab_index);
		pFet->pppoe_idx = (IFX_uint16_t)tab_index;
	} else {
   	  pFet->pppoe_idx = 0x1F;
	}
	pcetable.key[7] |= (pFet->pppoe_idx << 8);
	/* VLAN used */
	tab_index = 0x7F;
	if (pPar->pattern.bVid == IFX_TRUE) {
		tab_index = ifx_pce_tm_find_vlan_id_index(&pHandle->pce_sub_tbl,pPar->pattern.nVid );
	}
	pFet->vlan_idx = (IFX_uint16_t)tab_index;
	pcetable.key[0] &= ~(0xFF << 8);
	pcetable.key[0] |= (pFet->vlan_idx << 8);

	pAct = &(pHandle->pce_act[idx]);
	/* take over the rule action */
	// memcpy(&pHandle->pce_act[idx], pPar->action, sizeof(IFX_FLOW_PCE_action_t));
	memcpy(pAct, &pPar->action, sizeof(IFX_FLOW_PCE_action_t));
	/* now programm the cached memory configuration to the hardware!! */
	/* prepare the action table data */
#if defined(PCE_TABLE_DEBUG) && PCE_TABLE_DEBUG
	IFXOS_PRINT_INT_RAW("pAct->ePortMapAction = %d\n",pAct->ePortMapAction);
	IFXOS_PRINT_INT_RAW("pAct->eVLAN_Action = %d\n",pAct->eVLAN_Action);
	IFXOS_PRINT_INT_RAW("pAct->eTrafficClassAction = %d\n",pAct->eTrafficClassAction);
	IFXOS_PRINT_INT_RAW("pAct->bRemarkClass = %d\n",pAct->bRemarkClass);
	IFXOS_PRINT_INT_RAW("pAct->eVLAN_CrossAction = %d\n",pAct->eVLAN_CrossAction);
	IFXOS_PRINT_INT_RAW("pAct->eCrossStateAction = %d\n",pAct->eCrossStateAction);
	IFXOS_PRINT_INT_RAW("pAct->eCritFrameAction = %d\n",pAct->eCritFrameAction );
	IFXOS_PRINT_INT_RAW("pAct->eTimestampAction = %d\n",pAct->eTimestampAction);
	IFXOS_PRINT_INT_RAW("pAct->eIrqAction = %d\n",pAct->eIrqAction);
	IFXOS_PRINT_INT_RAW("pAct->eLearningAction = %d\n",pAct->eLearningAction);
	IFXOS_PRINT_INT_RAW("pAct->eTimestampAction = %d\n",pAct->eTimestampAction);
	IFXOS_PRINT_INT_RAW("pAct->bRMON_Action = %d\n",pAct->bRMON_Action);
	IFXOS_PRINT_INT_RAW("pAct->nTrafficClassAlternate = %d\n",pAct->nTrafficClassAlternate);
	IFXOS_PRINT_INT_RAW("pAct->nForwardPortMap = %d\n",pAct->nForwardPortMap);
	IFXOS_PRINT_INT_RAW("pAct->nMeterId = %d\n",pAct->nMeterId);
	IFXOS_PRINT_INT_RAW("pAct->eMeterAction = %d\n",pAct->eMeterAction);
	IFXOS_PRINT_INT_RAW("pAct->bRemarkDSCP = %d\n",pAct->bRemarkDSCP);
	IFXOS_PRINT_INT_RAW("pAct->bRemarkDSCP = %d\n",pAct->bRemarkDSCP);
#endif /* PCE_TABLE_DEBUG */
	/* Fill the Action parameter into the pcetable.val[x] */
	/*	Action "Forwarding" Group. Port map action enable. This port
        forwarding configuration is ignored in case the action "IGMP Snooping"
        is enabled via the parameter 'nSnoopingTypeAction'. */
	if (pAct->ePortMapAction != IFX_FLOW_PCE_ACTION_PORTMAP_DISABLE) {
		if (pAct->eSnoopingTypeAction == IFX_FLOW_PCE_ACTION_IGMP_SNOOP_DISABLE) {
			pcetable.val[0] = 1;
			pcetable.val[4] |= (0x3 << 2);
			switch (pAct->ePortMapAction ) {
				case IFX_FLOW_PCE_ACTION_PORTMAP_REGULAR:
					pcetable.val[4] &= ~(0x3 << 2);
					break;
				case IFX_FLOW_PCE_ACTION_PORTMAP_DISCARD:
					pcetable.val[1] = 0;
					break;
				case IFX_FLOW_PCE_ACTION_PORTMAP_CPU:
					pcetable.val[1] = 0x40; /* Port 6 */
//					pcetable.val[1] = pAct->nForwardPortMap & 0xFFFF; //0x40; /* Port 6 */
					pcetable.val[4] &= ~(0x3 << 2);
					pcetable.val[4] |= (0x1 << 2);
					break;
				case IFX_FLOW_PCE_ACTION_PORTMAP_ALTERNATIVE:
					pcetable.val[1] = pAct->nForwardPortMap & 0xFFFF;
					break;
			}
		} else {
			switch (pAct->eSnoopingTypeAction) {
				case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REPORT:
				case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_LEAVE:
					pcetable.val[0] = 1;
					pcetable.val[4] &= ~(0x3 << 2);
					/* Forwarding Action enabled. Select Multicast Router Port-Map */
					pcetable.val[4] |= (0x1 << 2);   /* Multicast router portmap */
					break;
				default:
					pcetable.val[0] = 1;
					pcetable.val[4] &= ~(0x3 << 2);
					pcetable.val[4] |= (0x0 << 2);  /* default port map */
					break;
				}
		}
	} else {
		pcetable.val[0] = 0;
		pcetable.val[1] = 0xFFFF;
		pcetable.val[4] &= ~(0x3 << 2);
	}
  /* Action "Flow ID". This flow table action and the 'bFlowID_Action' action can be used exclusively. */
	if (pAct->bFlowID_Action != IFX_FALSE) {
		if (pAct->ePortMapAction == IFX_FLOW_PCE_ACTION_PORTMAP_DISABLE) {
			pcetable.val[4] |= (0x1 << 4);  /* enable Flow ID action */
			pcetable.val[1] = pAct->nFlowID & 0xFFFF;
		} else {
			IFXOS_PRINT_INT_RAW("ERROR: %s:%s:%d (PortMap & FlowID can be used exclusively)\n", __FILE__, __FUNCTION__, __LINE__);
			return(IFX_ERROR);
		}
	}

	tab_index = 0x7F;
	/** Action "VLAN" Group. VLAN action enable */
	pcetable.val[2] = 0;
	pcetable.val[0] &= ~(1 << 13 );  /* for IFX_FLOW_PCE_ACTION_VLAN_REGULAR  also*/
	if (pAct->eVLAN_Action != IFX_FLOW_PCE_ACTION_VLAN_DISABLE) {
		pcetable.val[0] |= ( 1 << 1 );
		if (pAct->eVLAN_Action == IFX_FLOW_PCE_ACTION_VLAN_ALTERNATIVE) {
			tab_index = ifx_pce_tm_find_vlan_id_index(&pHandle->pce_sub_tbl,pAct->nVLAN_Id );
			if (tab_index != 0x7F ) {
				pcetable.val[2] = (tab_index & 0xFF);
				pcetable.val[2] |= ((pAct->nVLAN_Id & 0xFF) << 8);
				pcetable.val[0] |= ( 1 << 13 ); /*Table action enabled */
			}	
		}
	} 
	/** Action "Traffic Class" Group. Traffic class action enable */
	if (pAct->eTrafficClassAction != IFX_FLOW_PCE_ACTION_TRAFFIC_CLASS_DISABLE) {
      pcetable.val[0] |= ( 1 << 2 );
		switch ( pAct->eTrafficClassAction ) {
			case IFX_FLOW_PCE_ACTION_TRAFFIC_CLASS_REGULAR:
				pcetable.val[0] &= ~(1 << 14);
				break;
			case IFX_FLOW_PCE_ACTION_TRAFFIC_CLASS_ALTERNATIVE:
				pcetable.val[0] |= (1 << 14);
				pcetable.val[3] &= ~(0xF << 8);
				pcetable.val[3] |= (pAct->nTrafficClassAlternate & 0xF) << 8;
				break;
		}
	} else {
		pcetable.val[0] &= ~((1 << 2) | (1 << 14));
		pcetable.val[3] |= (0xF << 8);
	}
	/** Action "Remarking" Group. Remarking action enable */
	if (pAct->bRemarkAction != IFX_FALSE)
		pcetable.val[0] |= (1 << 3);
	else
		pcetable.val[0] &= ~(1 << 3);
	/** Action "Cross VLAN" Group. Cross VLAN action enable */
	if (pAct->eVLAN_CrossAction != IFX_FLOW_PCE_ACTION_CROSS_VLAN_DISABLE) {
		pcetable.val[0] |= ( 1 << 4 );
		if (pAct->eVLAN_CrossAction == IFX_FLOW_PCE_ACTION_CROSS_VLAN_REGULAR)
			pcetable.val[3] &= ~(0 << 15);
		else
			pcetable.val[3] |= (1 << 15);
	} else { 
		pcetable.val[0] &= ~ (1 << 4);
		pcetable.val[3] &= ~(0 << 15);
	}
	/** Action "Cross State" Group. Cross state action control and enable */
	if (pAct->eCrossStateAction != IFX_FLOW_PCE_ACTION_CROSS_STATE_DISABLE) {
		pcetable.val[0] |= ( 1 << 5 );
		if (pAct->eCrossStateAction == IFX_FLOW_PCE_ACTION_CROSS_STATE_CROSS)
			pcetable.val[4] |= ( 1 << 13 );
		else
			pcetable.val[4] &= ~(1 << 13 );
	} else { 
		pcetable.val[4] &= ~(1 << 13 );
		pcetable.val[0] &= ~(1 << 5 );
	}
	/** Action "Critical Frames" Group. Critical Frame action control and enable */
	if (pAct->eCritFrameAction != IFX_FLOW_PCE_ACTION_CRITICAL_FRAME_DISABLE) {
		pcetable.val[0] |= ( 1 << 6 );
		if (pAct->eCritFrameAction == IFX_FLOW_PCE_ACTION_CRITICAL_FRAME_CRITICAL)
			pcetable.val[4] |= ( 1 << 14 );
		else
			pcetable.val[4] &= ~(1 << 14 );
	} else { 
		pcetable.val[0] &= ~(1 << 6 );
		pcetable.val[4] &= ~(1 << 14 );
	}
	/** Action "Timestamp" Group. Time stamp action control and enable */
	if (pAct->eTimestampAction != IFX_FLOW_PCE_ACTION_TIMESTAMP_DISABLE) {
		pcetable.val[0] |= ( 1 << 7 );
		if (pAct->eTimestampAction == IFX_FLOW_PCE_ACTION_TIMESTAMP_STORED)
			pcetable.val[4] |= ( 1 << 15 );
		else
			pcetable.val[4] &= ~(1 << 15 );
	} else {
		pcetable.val[0] &= ~(1 << 7 );
		pcetable.val[4] &= ~(1 << 15 );
	}
	/** Action "Interrupt" Group. Interrupt action generate and enable */
	if (pAct->eIrqAction != IFX_FLOW_PCE_ACTION_IRQ_DISABLE) {
		pcetable.val[0] |= ( 1 << 8 );
		if ( pAct->eIrqAction == IFX_FLOW_PCE_ACTION_IRQ_EVENT)
			pcetable.val[0] |= ( 1 << 15 );
		else
			pcetable.val[0] &= ~(1 << 15 );
	} else {
		pcetable.val[0] &= ~((1 << 8) | (1 << 15));
	}
	/** Action "Learning" Group. Learning action control and enable */
	if (pAct->eLearningAction != IFX_FLOW_PCE_ACTION_LEARNING_DISABLE) {
		pcetable.val[0] |= ( 1 << 9 );
		/* Todo: Learning Rule need to be check */
		switch (pAct->eLearningAction) {
			case IFX_FLOW_PCE_ACTION_LEARNING_REGULAR:
				pcetable.val[4] &= ~0x3;
				break;
			case IFX_FLOW_PCE_ACTION_LEARNING_FORCE_NOT:
				pcetable.val[4] = (pcetable.val[4] & ~0x3) | 0x2;
				break;
			case IFX_FLOW_PCE_ACTION_LEARNING_FORCE:
				pcetable.val[4] |= 0x3;
				break;
		}
	} else {
		pcetable.val[0] &= ~(1 << 9 );
		pcetable.val[4] &= ~0x3;
	}
	/** Action "IGMP Snooping" Group. */
	if (pAct->eSnoopingTypeAction != IFX_FLOW_PCE_ACTION_IGMP_SNOOP_DISABLE) {
		pcetable.val[0] |= ( 1 << 10 );
		pcetable.val[4] &= ~( 0x7 << 5 );
		switch (pAct->eSnoopingTypeAction) {
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REGULAR:
				pcetable.val[4] |=( 0 << 5 );
				break;
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_REPORT:
				pcetable.val[4] |=( 1 << 5 );
				break;
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_LEAVE:
				pcetable.val[4] |=( 2 << 5 );
				break;
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_AD:
				pcetable.val[4] |=( 3 << 5 );
				break;
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY:
				pcetable.val[4] |=( 4 << 5 );
				break;
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP:
				pcetable.val[4] |=( 5 << 5 );
				break;
			case IFX_FLOW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER:
				pcetable.val[4] |=( 6 << 5 );
				break;
		}
	} else {
		pcetable.val[0] &= ~(1 << 10 );
		pcetable.val[4] &= ~(0x7 << 5);
	}
	/** Action "Meter" Group. Meter action control and enable. */
	if (pAct->eMeterAction != IFX_FLOW_PCE_ACTION_METER_DISABLE) {
		pcetable.val[0] |= ( 1 << 11 );
		pcetable.val[3] = (pAct->nMeterId & 0x1F);
		switch(pAct->eMeterAction) {
			case IFX_FLOW_PCE_ACTION_METER_1:
				pcetable.val[3] |= 1 << 6;
				break;
			case IFX_FLOW_PCE_ACTION_METER_2:
				pcetable.val[3] |= 2 << 6;
				break;
			case IFX_FLOW_PCE_ACTION_METER_1_2:
				pcetable.val[3] |= 3 << 6;
				break;
		}
	} else {
		pcetable.val[0] &= ~(1 << 11 );
		pcetable.val[3] |= 0x1F;
	}
   /** Action "RMON" Group. RMON action enable */
	if (pAct->bRMON_Action != IFX_FALSE) {
		pcetable.val[0] |= ( 1 << 12 );
		pcetable.val[4] &= ~(0x1F << 8);
//		pcetable.val[4] |= ((pAct->nRMON_Id << 8);
		if (pAct->nRMON_Id < 24  )  {/*RMON_ID will support from 1 to 24 */
			pcetable.val[4] |= ((pAct->nRMON_Id +1) << 8);
		} else  {
			IFXOS_PRINT_INT_RAW("ERROR: RMON_ID: 0x%08x, %s:%s:%d \n",pAct->nRMON_Id, __FILE__, __FUNCTION__, __LINE__);
			return(IFX_ERROR);
		}
		
	} else {
		pcetable.val[0] &= ~(1 << 12 );
		pcetable.val[4] &=	~(0x1F << 8);
	}
	pcetable.val[3] |= (0x7 << 12);
	if(pAct->bRemarkDSCP == IFX_TRUE)
		pcetable.val[3] &= ~(1 << 12);
	if(pAct->bRemarkClass == IFX_TRUE)
		pcetable.val[3] &= ~(1 << 13);
	if(pAct->bRemarkPCP == IFX_TRUE)
		pcetable.val[3] &= ~(1 << 14);

	pcetable.table_index = idx;    /*index of the Traffic Flow Table index configuration */
	pcetable.table = IFX_ETHSW_PCE_TFLOW_INDEX;
	pcetable.valid = 1;
	ifx_ethsw_xwayflow_pce_table_write(pDevCtx, &pcetable);
   return IFX_SUCCESS;
}
