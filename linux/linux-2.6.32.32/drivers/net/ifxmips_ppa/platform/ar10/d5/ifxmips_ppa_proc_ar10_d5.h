#ifndef __IFX_PPA_PROC_V5_H__
#define __IFX_PPA_PROC_V5_H__


/*******************************************************************************
**
** FILE NAME    : ifx_ppa_proc_v5.h
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 4 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA PPE Firmware Hardware/Firmware Adaption Layer Header File
*** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 04 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/

/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Proc File
 */

int ifx_ppa_drv_proc_read_hal_mib(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_hal_mib(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_route(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_hal_route(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_mc(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_read_genconf(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_genconf(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_pppoe(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_pppoe(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_mtu(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_mtu(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_hit(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_hit(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_mac(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_mac(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_out_vlan(char *, char **, off_t, int, int *, void *);
int ifx_ppa_drv_proc_write_out_vlan(struct file *, const char *, unsigned long, void *);
int ifx_ppa_drv_proc_read_ipv6_ip(char *, char **, off_t, int, int *, void *);
extern int32_t (*ifx_ppa_drv_add_ipv6_routing_entry_fn)(uint32_t f_is_lan,
                               uint32_t src_ip[4],
                               uint32_t src_port,
                               uint32_t dst_ip[4],
                               uint32_t dst_port,
                               uint32_t f_is_tcp,    //  1: TCP, 0: UDP
                               uint32_t route_type,
                               uint32_t new_ip,
                               uint32_t new_port,
                               uint8_t  new_mac[PPA_ETH_ALEN],
                               uint32_t new_src_mac_ix,
                               uint32_t mtu_ix,
                               uint32_t f_new_dscp_enable,
                               uint32_t new_dscp,
                               uint32_t f_vlan_ins_enable,
                               uint32_t new_vci,
                               uint32_t f_vlan_rm_enable,
                               uint32_t pppoe_mode,
                               uint32_t pppoe_ix,
                               uint32_t f_out_vlan_ins_enable,
                               uint32_t out_vlan_ix,
                               uint32_t f_out_vlan_rm_enable,
                               uint32_t dslwan_qid,
                               uint32_t dest_list,
                               uint32_t *p_entry);


extern int32_t (*ifx_ppa_drv_add_routing_entry_fn)(uint32_t f_is_lan,
                          uint32_t src_ip,
                          uint32_t src_port,
                          uint32_t dst_ip,
                          uint32_t dst_port,
                          uint32_t f_is_tcp,    //  1: TCP, 0: UDP
                          uint32_t route_type,
                          uint32_t new_ip,
                          uint32_t new_port,
                          uint8_t  new_mac[PPA_ETH_ALEN],
                          uint32_t new_src_mac_ix,
                          uint32_t mtu_ix,
                          uint32_t f_new_dscp_enable,
                          uint32_t new_dscp,
                          uint32_t f_vlan_ins_enable,
                          uint32_t new_vci,
                          uint32_t f_vlan_rm_enable,
                          uint32_t pppoe_mode,
                          uint32_t pppoe_ix,
                          uint32_t f_out_vlan_ins_enable,
                          uint32_t out_vlan_ix,
                          uint32_t f_out_vlan_rm_enable,
                          uint32_t dslwan_qid,
                          uint32_t dest_list,
                          uint32_t *p_entry);


extern void (*ifx_ppa_drv_del_routing_entry_fn)(uint32_t entry);
#endif
