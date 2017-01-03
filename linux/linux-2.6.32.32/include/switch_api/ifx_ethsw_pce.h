/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_pce.h
   \remarks implement PCE header structure.
 *****************************************************************************/

#ifndef _IFX_ETHSW_PCE_H_
#define _IFX_ETHSW_PCE_H_

#include "ifx_types.h"
#include "ifx_ethsw_flow.h"
#include "ifx_ethsw_api.h"

/* Definitions */
#ifndef IFX_DEBUG_PCE
   #define IFX_DEBUG_PCE \
      { \
         printk("DEBUG:\n\tFile %s\n\tLine %d\n", __FILE__, __LINE__);\
      }
#endif

#ifndef IFX_RETURN_PCETM
   #define IFX_RETURN_PCETM \
   { \
      printk("ERROR:\n\tFile %s\n\tLine %d\n", __FILE__, __LINE__); \
      return (-1); \
   }
#endif

#ifndef IFX_RETURN_PCE
   #define IFX_RETURN_PCE \
      { \
         printk("ERROR:\n\tFile %s\n\tLine %d\n", __FILE__, __LINE__); \
         return (-1); \
      }
#endif

#define IFX_PCE_PKG_LNG_TBL_SIZE             16
#define IFX_PCE_DASA_MAC_TBL_SIZE            64
#define IFX_PCE_APPL_TBL_SIZE                64
#define IFX_PCE_IP_DASA_MSB_TBL_SIZE         16
#define IFX_PCE_IP_DASA_LSB_TBL_SIZE         64
#define IFX_PCE_PTCL_TBL_SIZE                32
#define IFX_PCE_PPPOE_TBL_SIZE               16
#define IFX_PCE_VLAN_ACT_TBL_SIZE            64

#define IFX_PCE_TBL_SIZE 64

#define IFX_FLOW_PCE_MICROCODE_VALUES 64
#define NUM_OF_PORTS_INCLUDE_CPU_PORT	7

/* Switch API Micro Code V0.3 */
// parser's microcode output field type
enum {
	
	OUT_MAC0 = 0,
	OUT_MAC1,
	OUT_MAC2,
	OUT_MAC3,
	OUT_MAC4,
	OUT_MAC5,
	OUT_ETHTYP,
	OUT_VTAG0,
 	OUT_VTAG1,
	OUT_ITAG0,
	OUT_ITAG1,	/*10 */
	OUT_ITAG2,
	OUT_ITAG3,
	OUT_IP0,
	OUT_IP1,
	OUT_IP2,
	OUT_IP3,
	OUT_SIP0,
	OUT_SIP1,
	OUT_SIP2,
	OUT_SIP3,	/*20*/
	OUT_SIP4,
	OUT_SIP5,
	OUT_SIP6,
	OUT_SIP7,
	OUT_DIP0,
	OUT_DIP1,
	OUT_DIP2,
	OUT_DIP3,
	OUT_DIP4,
	OUT_DIP5,	/*30*/
	OUT_DIP6,
	OUT_DIP7,
	OUT_SESID,
	OUT_PROT,
	OUT_APP0,
	OUT_APP1,
	OUT_IGMP0,
	OUT_IGMP1,
	OUT_IPOFF,	/*39*/
	OUT_NONE =  63
};

// parser's microcode length type
#define INSTR   0
#define IPV6    1
#define LENACCU 2

// parser's microcode flag type
enum {
	FLAG_ITAG =  0,
	FLAG_VLAN,
	FLAG_SNAP,
	FLAG_PPPOE,
	FLAG_IPV6,
	FLAG_IPV6FL,
	FLAG_IPV4,
	FLAG_IGMP,
	FLAG_TU,
	FLAG_HOP,
	FLAG_NN1,	/*10 */
	FLAG_NN2,
	FLAG_END, 
	FLAG_NO,	/*13*/
};

typedef struct {
   unsigned short val_3;
   unsigned short val_2;
   unsigned short val_1;
   unsigned short val_0;
} IFX_FLOW_PCE_MICROCODE_ROW;

typedef IFX_FLOW_PCE_MICROCODE_ROW IFX_FLOW_PCE_MICROCODE[IFX_FLOW_PCE_MICROCODE_VALUES];

/** Provides the address of the configured/fetched lookup table. */
typedef enum
{
	/** Parser microcode table */
	IFX_ETHSW_PCE_PARS_INDEX		= 0x00,
	IFX_ETHSW_PCE_ACTVLAN_INDEX		= 0x01,
	IFX_ETHSW_PCE_VLANMAP_INDEX		= 0x02,
	IFX_ETHSW_PCE_PPPOE_INDEX		= 0x03,
	IFX_ETHSW_PCE_PROTOCOL_INDEX	= 0x04,
	IFX_ETHSW_PCE_APPLICATION_INDEX	= 0x05,
	IFX_ETHSW_PCE_IP_DASA_MSB_INDEX	= 0x06,
	IFX_ETHSW_PCE_IP_DASA_LSB_INDEX	= 0x07,
	IFX_ETHSW_PCE_PACKET_INDEX		= 0x08,
	IFX_ETHSW_PCE_PCP_INDEX			= 0x09,
	IFX_ETHSW_PCE_DSCP_INDEX		= 0x0A,
	IFX_ETHSW_PCE_MAC_BRIDGE_INDEX	= 0x0B,
	IFX_ETHSW_PCE_MAC_DASA_INDEX	= 0x0C,
	IFX_ETHSW_PCE_MULTICAST_SW_INDEX= 0x0D,
	IFX_ETHSW_PCE_MULTICAST_HW_INDEX= 0x0E,
	IFX_ETHSW_PCE_TFLOW_INDEX		= 0x0F,
	IFX_ETHSW_PCE_REMARKING_INDEX	= 0x10,
	IFX_ETHSW_PCE_QUEUE_MAP_INDEX	= 0x11,
	IFX_ETHSW_PCE_METER_INS_0_INDEX	= 0x12,
	IFX_ETHSW_PCE_METER_INS_1_INDEX	= 0x13
}LOOKUP_TABLE_ADDRESS_t;


/** Description */
typedef enum
{
  /**  */
  IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_ADRD = 0,
  IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_ADWR = 1,
  IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_KSRD = 2,
  IFX_ETHSW_LOOKUP_TABLE_ACCESS_OP_MODE_KSWR = 3
}LOOKUP_TABLE_ACCESS_OPERATION_MODE_t;

typedef struct
{
	/* key values */
	IFX_uint16_t key[8];
	/* mask nipples valid for the keys */
	IFX_uint16_t mask;
	/* values */
	IFX_uint16_t val[5];
	/* choose the related table */
	IFX_uint16_t table;
	/* address index/offset inside the table (zero based couting) */
	IFX_uint16_t table_index;
	/* type: mask (0) or range (1) */
	IFX_uint16_t type:1;
	/* entry is valid (1) or invalid (0) */
	IFX_uint16_t valid:1;
	/* entry belongs to group in case it is not zero */
	IFX_uint16_t group:4;
}IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t;


typedef struct
{
   /* Packet length */
   IFX_uint16_t              pkg_lng;
   /* Packet length range, in number of bytes */
   IFX_uint16_t              pkg_lng_rng;
}IFX_PCE_PKG_LNG_TBL_t;

typedef struct
{
   /* MAC Address */
   IFX_uint8_t               mac[6];
   /* MAC Address Mask */
   IFX_uint16_t              mac_mask;
}IFX_PCE_DASA_MAC_TBL_t;

typedef struct
{
   /* Application data */
   IFX_uint16_t              appl_data;
   /* Mode = mask: Application data mask
      Mode = range: Application data range */
   IFX_uint16_t              mask_range;
}IFX_PCE_APPL_TBL_t;

typedef struct
{
   /* IP address byte 15:8 */
   IFX_uint8_t               ip_msb[8];
   /* Nibble mask */
   IFX_uint16_t              mask;
}IFX_PCE_IP_DASA_MSB_TBL_t;

typedef struct
{
   /* IP address byte 7:0 */
   IFX_uint8_t               ip_lsb[8];
   /* Nibble mask */
   IFX_uint16_t              mask;
}IFX_PCE_IP_DASA_LSB_TBL_t;

typedef struct
{
   union {
      /* Ethertype */
      IFX_uint16_t              ethertype;
      struct {
         /* Protocol */
         IFX_uint16_t              protocol:8;
         /* Protocol Flags */
         IFX_uint16_t              protocol_flags:8;
      }prot;
   }key;
   union {
      /* Ethertype Mask */
      IFX_uint16_t              ethertype_mask;
      struct {
         /* reserved */
         IFX_uint16_t              res:12;
         /* Protocol Mask */
         IFX_uint16_t              protocol_mask:2;
         /* Protocol Flag Mask */
         IFX_uint16_t              protocol_flag_mask:2;
      }prot;
   }mask;
}IFX_PCE_PTCL_TBL_t;

typedef struct
{
   /* PPPoE session ID */
   IFX_uint16_t              sess_id;
}IFX_PCE_PPPOE_TBL_t;
#if 0
typedef struct
{
   /* VLAN ID */
   IFX_uint16_t              vid;
   /* reserved */
   IFX_uint16_t              res:9;
   /* Replacement Flag */
   IFX_uint16_t              repl_flg:1;
   /* FID */
   IFX_uint16_t              fid:6;
}IFX_PCE_VLAN_ACT_TBL_t;
#endif 
typedef struct
{
   /* DSCP value */
   IFX_uint16_t                             dscp:7;
   /* PCP value */
   IFX_uint16_t                             pcp:4;
   /* Packet length */
   IFX_uint16_t                             pkt_lng_idx:5;
   /* Destination MAC address */
   IFX_uint16_t                             dst_mac_addr_idx:8;
   /* Source MAC address */
   IFX_uint16_t                             src_mac_addr_idx:8;
   /* Destination Application field */
   IFX_uint16_t                             dst_appl_fld_idx:8;
   /* Source Application field */
   IFX_uint16_t                             src_appl_fld_idx:8;
   /* DIP MSB */
   IFX_uint16_t                             dip_msb_idx:8;
   /* DIP LSB */
   IFX_uint16_t                             dip_lsb_idx:8;
   /* SIP MSB */
   IFX_uint16_t                             sip_msb_idx:8;
   /* SIP LSB */
   IFX_uint16_t                             sip_lsb_idx:8;
   /* IP protocol */
   IFX_uint16_t                             ip_prot_idx:8;
   /* Ethertype */
   IFX_uint16_t                             ethertype_idx:8;
   /* PPPoE */
   IFX_uint16_t                             pppoe_idx:5;
   /* VLAN */
   IFX_uint16_t                             vlan_idx:7;
   /* Port ID */
   IFX_uint16_t                             port_id:8;
}IFX_PCE_TBL_t;

typedef struct
{
   /* table reference counter */
   IFX_uint16_t               pkg_lng_tbl_cnt[IFX_PCE_PKG_LNG_TBL_SIZE];
   IFX_uint16_t               dasa_mac_tbl_cnt[IFX_PCE_DASA_MAC_TBL_SIZE];
   IFX_uint16_t               appl_tbl_cnt[IFX_PCE_APPL_TBL_SIZE];
   IFX_uint16_t               ip_dasa_msb_tbl_cnt[IFX_PCE_IP_DASA_MSB_TBL_SIZE];
   IFX_uint16_t               ip_dasa_lsb_tbl_cnt[IFX_PCE_IP_DASA_LSB_TBL_SIZE];
   IFX_uint16_t               ptcl_tbl_cnt[IFX_PCE_PTCL_TBL_SIZE];
   IFX_uint16_t               pppoe_tbl_cnt[IFX_PCE_PPPOE_TBL_SIZE];
   IFX_uint16_t               vlan_act_tbl_cnt[IFX_PCE_VLAN_ACT_TBL_SIZE];

   /* cached tables */
   IFX_PCE_PKG_LNG_TBL_t     pkg_lng_tbl[IFX_PCE_PKG_LNG_TBL_SIZE];
   IFX_PCE_DASA_MAC_TBL_t    dasa_mac_tbl[IFX_PCE_DASA_MAC_TBL_SIZE];
   IFX_PCE_APPL_TBL_t        appl_tbl[IFX_PCE_APPL_TBL_SIZE];
   IFX_PCE_IP_DASA_MSB_TBL_t      ip_dasa_msb_tbl[IFX_PCE_IP_DASA_MSB_TBL_SIZE];
   IFX_PCE_IP_DASA_LSB_TBL_t      ip_dasa_lsb_tbl[IFX_PCE_IP_DASA_LSB_TBL_SIZE];
   IFX_PCE_PTCL_TBL_t        ptcl_tbl[IFX_PCE_PTCL_TBL_SIZE];
   IFX_PCE_PPPOE_TBL_t       pppoe_tbl[IFX_PCE_PPPOE_TBL_SIZE];
 //  IFX_PCE_VLAN_ACT_TBL_t    vlan_act_tbl[IFX_PCE_VLAN_ACT_TBL_SIZE];

}IFX_PCE_HANDLE_t;

typedef struct
{
   /* Parameter for the sub-tables */
   IFX_PCE_HANDLE_t                         pce_sub_tbl;

   IFX_PCE_TBL_t                            pce_tbl[IFX_PCE_TBL_SIZE]; 
   IFX_FLOW_PCE_action_t                    pce_act[IFX_PCE_TBL_SIZE]; 

   /* set if the entry is used */
   IFX_uint8_t                              pce_tbl_used[IFX_PCE_TBL_SIZE];
}IFX_PCE_t;

/* Function Declaration */
IFX_int32_t ifx_pce_table_init(IFX_PCE_t *pPCEHandle);

IFX_int32_t ifx_pce_tm_pkg_lng_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_PKG_LNG_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_pkg_lng_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_pkg_lng_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_PKG_LNG_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_dasa_mac_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_DASA_MAC_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_dasa_mac_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_dasa_mac_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_DASA_MAC_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_appl_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_APPL_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_appl_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_appl_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_APPL_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ip_dasa_msb_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_IP_DASA_MSB_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ip_dasa_msb_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_ip_dasa_msb_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_IP_DASA_MSB_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_IP_DASA_LSB_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_idx_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_IP_DASA_LSB_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ip_dasa_lsb_tbl_index_search(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_IP_DASA_LSB_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ptcl_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_PTCL_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_ptcl_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_ptcl_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_PTCL_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_pppoe_tbl_write(IFX_PCE_HANDLE_t *pTmHandle, IFX_PCE_PPPOE_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_pppoe_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_pppoe_tbl_read(IFX_PCE_HANDLE_t *pTmHandle, IFX_int32_t index, IFX_PCE_PPPOE_TBL_t *pPar);
IFX_int32_t ifx_pce_tm_vlan_act_tbl_delete(IFX_PCE_HANDLE_t *pTmHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_tm_print_tables(IFX_PCE_HANDLE_t *pTmHandle);
IFX_int32_t get_ifx_pce_tm_vlan_act_tbl_index(IFX_PCE_HANDLE_t *pTmHandle,IFX_uint8_t index);
IFX_int32_t ifx_pce_pattern_delete(IFX_PCE_t *pHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_action_delete(IFX_PCE_t *pHandle, IFX_uint32_t index);
IFX_int32_t ifx_pce_rule_read(IFX_PCE_t *pHandle, IFX_FLOW_PCE_rule_t *pPar);
IFX_int32_t ifx_pce_rule_write(IFX_PCE_t *pHandle, IFX_FLOW_PCE_rule_t *pPar);

IFX_return_t ifx_ethsw_xwayflow_pce_table_write(IFX_void_t *pDevCtx, IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t *pData);
IFX_return_t ifx_ethsw_xwayflow_pce_table_read(IFX_void_t *pDevCtx, IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t *pData);
IFX_return_t ifx_ethsw_xwayflow_pce_table_cam_write(IFX_void_t *pDevCtx, IFX_ETHSW_XWAYFLOW_PCE_TABLE_ENTRY_t *pData);

extern IFX_uint32_t ifx_ethsw_ll_DirectAccessRead(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t * value);
extern IFX_return_t ifx_ethsw_ll_DirectAccessWrite(IFX_void_t *pDevCtx, IFX_int16_t Offset, IFX_int16_t Shift, IFX_int16_t Size, IFX_uint32_t value);

/* Micro Code Load */
IFX_return_t IFX_VR9_Switch_PCE_Micro_Code_Int(IFX_void_t);

/* Debug Funtion */
IFX_int32_t ifx_pce_print_tables(IFX_PCE_t *pHandle);
#endif /* _IFX_ETHSW_PCE_H_ */
