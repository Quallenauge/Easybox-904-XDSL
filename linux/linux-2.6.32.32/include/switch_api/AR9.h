/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************/

#ifndef _AR9_H
#define _AR9_H
/* -------------------------------------------------------------------------- */
/* Register: 'Port Status Register' */
/* Bit: 'P1FCS' */
/* Description: 'Port 1 Flow Control Status' */
#define AR9_PS_REG_P1FCS_OFFSET                           0x0000
#define AR9_PS_REG_P1FCS_SHIFT                            12
#define AR9_PS_REG_P1FCS_SIZE                             1
/* Bit: 'P1DS' */
/* Description: 'Port 1 Duplex Status' */
#define AR9_PS_REG_P1DS_OFFSET                            0x0000
#define AR9_PS_REG_P1DS_SHIFT                             11
#define AR9_PS_REG_P1DS_SIZE                              1
/* Bit: 'P1SHS' */
/* Description: 'Port 1 Speed High Status' */
#define AR9_PS_REG_P1SHS_OFFSET                           0x0000
#define AR9_PS_REG_P1SHS_SHIFT                            10
#define AR9_PS_REG_P1SHS_SIZE                             1
/* Bit: 'P1SS' */
/* Description: 'Port 1 Speed Status' */
#define AR9_PS_REG_P1SS_OFFSET                            0x0000
#define AR9_PS_REG_P1SS_SHIFT                             9
#define AR9_PS_REG_P1SS_SIZE                              1
/* Bit: 'P1LS' */
/* Description: 'Port 1 Link Status' */
#define AR9_PS_REG_P1LS_OFFSET                            0x0000
#define AR9_PS_REG_P1LS_SHIFT                             8
#define AR9_PS_REG_P1LS_SIZE                              1
/* Bit: 'P0FCS' */
/* Description: 'Port 0 Flow Control Status' */
#define AR9_PS_REG_P0FCS_OFFSET                           0x0000
#define AR9_PS_REG_P0FCS_SHIFT                            4
#define AR9_PS_REG_P0FCS_SIZE                             1
/* Bit: 'P0DS' */
/* Description: 'Port 0 Duplex Status' */
#define AR9_PS_REG_P0DS_OFFSET                            0x0000
#define AR9_PS_REG_P0DS_SHIFT                             3
#define AR9_PS_REG_P0DS_SIZE                              1
/* Bit: 'P0SHS' */
/* Description: 'Port 0 Speed High Status' */
#define AR9_PS_REG_P0SHS_OFFSET                           0x0000
#define AR9_PS_REG_P0SHS_SHIFT                            2
#define AR9_PS_REG_P0SHS_SIZE                             1
/* Bit: 'P0SS' */
/* Description: 'Port 0 Speed Status' */
#define AR9_PS_REG_P0SS_OFFSET                            0x0000
#define AR9_PS_REG_P0SS_SHIFT                             1
#define AR9_PS_REG_P0SS_SIZE                              1
/* Bit: 'P0LS' */
/* Description: 'Port 0 Link Status' */
#define AR9_PS_REG_P0LS_OFFSET                            0x0000
#define AR9_PS_REG_P0LS_SHIFT                             0
#define AR9_PS_REG_P0LS_SIZE                              1
/* -------------------------------------------------------------------------- */
/* Register: 'P0 Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define AR9_P0_CTL_REG_SPS_OFFSET                         0x0004
#define AR9_P0_CTL_REG_SPS_SHIFT                          30
#define AR9_P0_CTL_REG_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define AR9_P0_CTL_REG_TCPE_OFFSET                        0x0004
#define AR9_P0_CTL_REG_TCPE_SHIFT                         29
#define AR9_P0_CTL_REG_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define AR9_P0_CTL_REG_IPOVTU_OFFSET                      0x0004
#define AR9_P0_CTL_REG_IPOVTU_SHIFT                       28
#define AR9_P0_CTL_REG_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define AR9_P0_CTL_REG_VPE_OFFSET                         0x0004
#define AR9_P0_CTL_REG_VPE_SHIFT                          27
#define AR9_P0_CTL_REG_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define AR9_P0_CTL_REG_SPE_OFFSET                         0x0004
#define AR9_P0_CTL_REG_SPE_SHIFT                          26
#define AR9_P0_CTL_REG_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define AR9_P0_CTL_REG_IPVLAN_OFFSET                      0x0004
#define AR9_P0_CTL_REG_IPVLAN_SHIFT                       25
#define AR9_P0_CTL_REG_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define AR9_P0_CTL_REG_TPE_OFFSET                         0x0004
#define AR9_P0_CTL_REG_TPE_SHIFT                          24
#define AR9_P0_CTL_REG_TPE_SIZE                           1
/* Bit: 'DFWD' */
/* Description: 'Port Ingress Direct Forwarding' */
#define AR9_P0_CTL_REG_DFWD_OFFSET                        0x0004
#define AR9_P0_CTL_REG_DFWD_SHIFT                         19
#define AR9_P0_CTL_REG_DFWD_SIZE                          1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define AR9_P0_CTL_REG_FLP_OFFSET                         0x0004
#define AR9_P0_CTL_REG_FLP_SHIFT                          18
#define AR9_P0_CTL_REG_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define AR9_P0_CTL_REG_FLD_OFFSET                         0x0004
#define AR9_P0_CTL_REG_FLD_SHIFT                          17
#define AR9_P0_CTL_REG_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define AR9_P0_CTL_REG_RMWFQ_OFFSET                       0x0004
#define AR9_P0_CTL_REG_RMWFQ_SHIFT                        16
#define AR9_P0_CTL_REG_RMWFQ_SIZE                         1
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define AR9_P0_CTL_REG_AD_OFFSET                          0x0004
#define AR9_P0_CTL_REG_AD_SHIFT                           15
#define AR9_P0_CTL_REG_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define AR9_P0_CTL_REG_LD_OFFSET                          0x0004
#define AR9_P0_CTL_REG_LD_SHIFT                           14
#define AR9_P0_CTL_REG_LD_SIZE                            1
/* Bit: 'REDIR' */
/* Description: 'Port Redirect Option' */
#define AR9_P0_CTL_REG_REDIR_OFFSET                       0x0004
#define AR9_P0_CTL_REG_REDIR_SHIFT                        13
#define AR9_P0_CTL_REG_REDIR_SIZE                         1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define AR9_P0_CTL_REG_MNA024_OFFSET                      0x0004
#define AR9_P0_CTL_REG_MNA024_SHIFT                       8
#define AR9_P0_CTL_REG_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define AR9_P0_CTL_REG_PPPOEP_OFFSET                      0x0004
#define AR9_P0_CTL_REG_PPPOEP_SHIFT                       7
#define AR9_P0_CTL_REG_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define AR9_P0_CTL_REG_PM_OFFSET                          0x0004
#define AR9_P0_CTL_REG_PM_SHIFT                           6
#define AR9_P0_CTL_REG_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define AR9_P0_CTL_REG_IPMO_OFFSET                        0x0004
#define AR9_P0_CTL_REG_IPMO_SHIFT                         4
#define AR9_P0_CTL_REG_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define AR9_P0_CTL_REG_PAS_OFFSET                         0x0004
#define AR9_P0_CTL_REG_PAS_SHIFT                          2
#define AR9_P0_CTL_REG_PAS_SIZE                           2
/* Bit: 'DSV8021x' */
/* Description: 'Drop Scheme for violation 802.1x' */
#define AR9_P0_CTL_REG_DSV8021X_OFFSET                    0x0004
#define AR9_P0_CTL_REG_DSV8021X_SHIFT                     1
#define AR9_P0_CTL_REG_DSV8021X_SIZE                      1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define AR9_P0_CTL_REG_BYPASS_OFFSET                      0x0004
#define AR9_P0_CTL_REG_BYPASS_SHIFT                       0
#define AR9_P0_CTL_REG_BYPASS_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'P1 Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define AR9_P1_CTL_REG_SPS_OFFSET                         0x0008
#define AR9_P1_CTL_REG_SPS_SHIFT                          30
#define AR9_P1_CTL_REG_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define AR9_P1_CTL_REG_TCPE_OFFSET                        0x0008
#define AR9_P1_CTL_REG_TCPE_SHIFT                         29
#define AR9_P1_CTL_REG_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define AR9_P1_CTL_REG_IPOVTU_OFFSET                      0x0008
#define AR9_P1_CTL_REG_IPOVTU_SHIFT                       28
#define AR9_P1_CTL_REG_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define AR9_P1_CTL_REG_VPE_OFFSET                         0x0008
#define AR9_P1_CTL_REG_VPE_SHIFT                          27
#define AR9_P1_CTL_REG_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define AR9_P1_CTL_REG_SPE_OFFSET                         0x0008
#define AR9_P1_CTL_REG_SPE_SHIFT                          26
#define AR9_P1_CTL_REG_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define AR9_P1_CTL_REG_IPVLAN_OFFSET                      0x0008
#define AR9_P1_CTL_REG_IPVLAN_SHIFT                       25
#define AR9_P1_CTL_REG_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define AR9_P1_CTL_REG_TPE_OFFSET                         0x0008
#define AR9_P1_CTL_REG_TPE_SHIFT                          24
#define AR9_P1_CTL_REG_TPE_SIZE                           1
/* Bit: 'DFWD' */
/* Description: 'Port Ingress Direct Forwarding' */
#define AR9_P1_CTL_REG_DFWD_OFFSET                        0x0008
#define AR9_P1_CTL_REG_DFWD_SHIFT                         19
#define AR9_P1_CTL_REG_DFWD_SIZE                          1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define AR9_P1_CTL_REG_FLP_OFFSET                         0x0008
#define AR9_P1_CTL_REG_FLP_SHIFT                          18
#define AR9_P1_CTL_REG_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define AR9_P1_CTL_REG_FLD_OFFSET                         0x0008
#define AR9_P1_CTL_REG_FLD_SHIFT                          17
#define AR9_P1_CTL_REG_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define AR9_P1_CTL_REG_RMWFQ_OFFSET                       0x0008
#define AR9_P1_CTL_REG_RMWFQ_SHIFT                        16
#define AR9_P1_CTL_REG_RMWFQ_SIZE                         1
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define AR9_P1_CTL_REG_AD_OFFSET                          0x0008
#define AR9_P1_CTL_REG_AD_SHIFT                           15
#define AR9_P1_CTL_REG_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define AR9_P1_CTL_REG_LD_OFFSET                          0x0008
#define AR9_P1_CTL_REG_LD_SHIFT                           14
#define AR9_P1_CTL_REG_LD_SIZE                            1
/* Bit: 'REDIR' */
/* Description: 'Port Redirect Option' */
#define AR9_P1_CTL_REG_REDIR_OFFSET                       0x0008
#define AR9_P1_CTL_REG_REDIR_SHIFT                        13
#define AR9_P1_CTL_REG_REDIR_SIZE                         1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define AR9_P1_CTL_REG_MNA024_OFFSET                      0x0008
#define AR9_P1_CTL_REG_MNA024_SHIFT                       8
#define AR9_P1_CTL_REG_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define AR9_P1_CTL_REG_PPPOEP_OFFSET                      0x0008
#define AR9_P1_CTL_REG_PPPOEP_SHIFT                       7
#define AR9_P1_CTL_REG_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define AR9_P1_CTL_REG_PM_OFFSET                          0x0008
#define AR9_P1_CTL_REG_PM_SHIFT                           6
#define AR9_P1_CTL_REG_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define AR9_P1_CTL_REG_IPMO_OFFSET                        0x0008
#define AR9_P1_CTL_REG_IPMO_SHIFT                         4
#define AR9_P1_CTL_REG_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define AR9_P1_CTL_REG_PAS_OFFSET                         0x0008
#define AR9_P1_CTL_REG_PAS_SHIFT                          2
#define AR9_P1_CTL_REG_PAS_SIZE                           2
/* Bit: 'DSV8021x' */
/* Description: 'Drop Scheme for violation 802.1x' */
#define AR9_P1_CTL_REG_DSV8021X_OFFSET                    0x0008
#define AR9_P1_CTL_REG_DSV8021X_SHIFT                     1
#define AR9_P1_CTL_REG_DSV8021X_SIZE                      1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define AR9_P1_CTL_REG_BYPASS_OFFSET                      0x0008
#define AR9_P1_CTL_REG_BYPASS_SHIFT                       0
#define AR9_P1_CTL_REG_BYPASS_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'P2 Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define AR9_P2_CTL_REG_SPS_OFFSET                         0x000C
#define AR9_P2_CTL_REG_SPS_SHIFT                          30
#define AR9_P2_CTL_REG_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define AR9_P2_CTL_REG_TCPE_OFFSET                        0x000C
#define AR9_P2_CTL_REG_TCPE_SHIFT                         29
#define AR9_P2_CTL_REG_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define AR9_P2_CTL_REG_IPOVTU_OFFSET                      0x000C
#define AR9_P2_CTL_REG_IPOVTU_SHIFT                       28
#define AR9_P2_CTL_REG_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define AR9_P2_CTL_REG_VPE_OFFSET                         0x000C
#define AR9_P2_CTL_REG_VPE_SHIFT                          27
#define AR9_P2_CTL_REG_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define AR9_P2_CTL_REG_SPE_OFFSET                         0x000C
#define AR9_P2_CTL_REG_SPE_SHIFT                          26
#define AR9_P2_CTL_REG_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define AR9_P2_CTL_REG_IPVLAN_OFFSET                      0x000C
#define AR9_P2_CTL_REG_IPVLAN_SHIFT                       25
#define AR9_P2_CTL_REG_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define AR9_P2_CTL_REG_TPE_OFFSET                         0x000C
#define AR9_P2_CTL_REG_TPE_SHIFT                          24
#define AR9_P2_CTL_REG_TPE_SIZE                           1
/* Bit: 'DFWD' */
/* Description: 'Port Ingress Direct Forwarding' */
#define AR9_P2_CTL_REG_DFWD_OFFSET                        0x000C
#define AR9_P2_CTL_REG_DFWD_SHIFT                         19
#define AR9_P2_CTL_REG_DFWD_SIZE                          1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define AR9_P2_CTL_REG_FLP_OFFSET                         0x000C
#define AR9_P2_CTL_REG_FLP_SHIFT                          18
#define AR9_P2_CTL_REG_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define AR9_P2_CTL_REG_FLD_OFFSET                         0x000C
#define AR9_P2_CTL_REG_FLD_SHIFT                          17
#define AR9_P2_CTL_REG_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define AR9_P2_CTL_REG_RMWFQ_OFFSET                       0x000C
#define AR9_P2_CTL_REG_RMWFQ_SHIFT                        16
#define AR9_P2_CTL_REG_RMWFQ_SIZE                         1
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define AR9_P2_CTL_REG_AD_OFFSET                          0x000C
#define AR9_P2_CTL_REG_AD_SHIFT                           15
#define AR9_P2_CTL_REG_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define AR9_P2_CTL_REG_LD_OFFSET                          0x000C
#define AR9_P2_CTL_REG_LD_SHIFT                           14
#define AR9_P2_CTL_REG_LD_SIZE                            1
/* Bit: 'REDIR' */
/* Description: 'Port Redirect Option' */
#define AR9_P2_CTL_REG_REDIR_OFFSET                       0x000C
#define AR9_P2_CTL_REG_REDIR_SHIFT                        13
#define AR9_P2_CTL_REG_REDIR_SIZE                         1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define AR9_P2_CTL_REG_MNA024_OFFSET                      0x000C
#define AR9_P2_CTL_REG_MNA024_SHIFT                       8
#define AR9_P2_CTL_REG_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define AR9_P2_CTL_REG_PPPOEP_OFFSET                      0x000C
#define AR9_P2_CTL_REG_PPPOEP_SHIFT                       7
#define AR9_P2_CTL_REG_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define AR9_P2_CTL_REG_PM_OFFSET                          0x000C
#define AR9_P2_CTL_REG_PM_SHIFT                           6
#define AR9_P2_CTL_REG_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define AR9_P2_CTL_REG_IPMO_OFFSET                        0x000C
#define AR9_P2_CTL_REG_IPMO_SHIFT                         4
#define AR9_P2_CTL_REG_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define AR9_P2_CTL_REG_PAS_OFFSET                         0x000C
#define AR9_P2_CTL_REG_PAS_SHIFT                          2
#define AR9_P2_CTL_REG_PAS_SIZE                           2
/* Bit: 'DSV8021x' */
/* Description: 'Drop Scheme for violation 802.1x' */
#define AR9_P2_CTL_REG_DSV8021X_OFFSET                    0x000C
#define AR9_P2_CTL_REG_DSV8021X_SHIFT                     1
#define AR9_P2_CTL_REG_DSV8021X_SIZE                      1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define AR9_P2_CTL_REG_BYPASS_OFFSET                      0x000C
#define AR9_P2_CTL_REG_BYPASS_SHIFT                       0
#define AR9_P2_CTL_REG_BYPASS_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 VLAN Control Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define AR9_P0_VLAN_REG_DFID_OFFSET                       0x0010
#define AR9_P0_VLAN_REG_DFID_SHIFT                        30
#define AR9_P0_VLAN_REG_DFID_SIZE                         2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define AR9_P0_VLAN_REG_TBVE_OFFSET                       0x0010
#define AR9_P0_VLAN_REG_TBVE_SHIFT                        29
#define AR9_P0_VLAN_REG_TBVE_SIZE                         1
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define AR9_P0_VLAN_REG_IFNTE_OFFSET                      0x0010
#define AR9_P0_VLAN_REG_IFNTE_SHIFT                       28
#define AR9_P0_VLAN_REG_IFNTE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define AR9_P0_VLAN_REG_VC_OFFSET                         0x0010
#define AR9_P0_VLAN_REG_VC_SHIFT                          27
#define AR9_P0_VLAN_REG_VC_SIZE                           1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define AR9_P0_VLAN_REG_VSD_OFFSET                        0x0010
#define AR9_P0_VLAN_REG_VSD_SHIFT                         26
#define AR9_P0_VLAN_REG_VSD_SIZE                          1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define AR9_P0_VLAN_REG_AOVTP_OFFSET                      0x0010
#define AR9_P0_VLAN_REG_AOVTP_SHIFT                       25
#define AR9_P0_VLAN_REG_AOVTP_SIZE                        1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define AR9_P0_VLAN_REG_VMCE_OFFSET                       0x0010
#define AR9_P0_VLAN_REG_VMCE_SHIFT                        24
#define AR9_P0_VLAN_REG_VMCE_SIZE                         1
/* Bit: 'DVPM' */
/* Description: 'Default VLAN Port Map' */
#define AR9_P0_VLAN_REG_DVPM_OFFSET                       0x0010
#define AR9_P0_VLAN_REG_DVPM_SHIFT                        16
#define AR9_P0_VLAN_REG_DVPM_SIZE                         8
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define AR9_P0_VLAN_REG_PP_OFFSET                         0x0010
#define AR9_P0_VLAN_REG_PP_SHIFT                          14
#define AR9_P0_VLAN_REG_PP_SIZE                           2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define AR9_P0_VLAN_REG_PPE_OFFSET                        0x0010
#define AR9_P0_VLAN_REG_PPE_SHIFT                         13
#define AR9_P0_VLAN_REG_PPE_SIZE                          1
/* Bit: 'PVTAGMP' */
/* Description: 'Port base VLAN tag member for Port 0' */
#define AR9_P0_VLAN_REG_PVTAGMP_OFFSET                    0x0010
#define AR9_P0_VLAN_REG_PVTAGMP_SHIFT                     12
#define AR9_P0_VLAN_REG_PVTAGMP_SIZE                      1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define AR9_P0_VLAN_REG_PVID_OFFSET                       0x0010
#define AR9_P0_VLAN_REG_PVID_SHIFT                        0
#define AR9_P0_VLAN_REG_PVID_SIZE                         12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 VLAN Control Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define AR9_P1_VLAN_REG_DFID_OFFSET                       0x0014
#define AR9_P1_VLAN_REG_DFID_SHIFT                        30
#define AR9_P1_VLAN_REG_DFID_SIZE                         2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define AR9_P1_VLAN_REG_TBVE_OFFSET                       0x0014
#define AR9_P1_VLAN_REG_TBVE_SHIFT                        29
#define AR9_P1_VLAN_REG_TBVE_SIZE                         1
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define AR9_P1_VLAN_REG_IFNTE_OFFSET                      0x0014
#define AR9_P1_VLAN_REG_IFNTE_SHIFT                       28
#define AR9_P1_VLAN_REG_IFNTE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define AR9_P1_VLAN_REG_VC_OFFSET                         0x0014
#define AR9_P1_VLAN_REG_VC_SHIFT                          27
#define AR9_P1_VLAN_REG_VC_SIZE                           1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define AR9_P1_VLAN_REG_VSD_OFFSET                        0x0014
#define AR9_P1_VLAN_REG_VSD_SHIFT                         26
#define AR9_P1_VLAN_REG_VSD_SIZE                          1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define AR9_P1_VLAN_REG_AOVTP_OFFSET                      0x0014
#define AR9_P1_VLAN_REG_AOVTP_SHIFT                       25
#define AR9_P1_VLAN_REG_AOVTP_SIZE                        1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define AR9_P1_VLAN_REG_VMCE_OFFSET                       0x0014
#define AR9_P1_VLAN_REG_VMCE_SHIFT                        24
#define AR9_P1_VLAN_REG_VMCE_SIZE                         1
/* Bit: 'DVPM' */
/* Description: 'Default VLAN Port Map' */
#define AR9_P1_VLAN_REG_DVPM_OFFSET                       0x0014
#define AR9_P1_VLAN_REG_DVPM_SHIFT                        16
#define AR9_P1_VLAN_REG_DVPM_SIZE                         8
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define AR9_P1_VLAN_REG_PP_OFFSET                         0x0014
#define AR9_P1_VLAN_REG_PP_SHIFT                          14
#define AR9_P1_VLAN_REG_PP_SIZE                           2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define AR9_P1_VLAN_REG_PPE_OFFSET                        0x0014
#define AR9_P1_VLAN_REG_PPE_SHIFT                         13
#define AR9_P1_VLAN_REG_PPE_SIZE                          1
/* Bit: 'PVTAGMP' */
/* Description: 'Port base VLAN tag member for Port 0' */
#define AR9_P1_VLAN_REG_PVTAGMP_OFFSET                    0x0014
#define AR9_P1_VLAN_REG_PVTAGMP_SHIFT                     12
#define AR9_P1_VLAN_REG_PVTAGMP_SIZE                      1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define AR9_P1_VLAN_REG_PVID_OFFSET                       0x0014
#define AR9_P1_VLAN_REG_PVID_SHIFT                        0
#define AR9_P1_VLAN_REG_PVID_SIZE                         12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 VLAN Control Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define AR9_P2_VLAN_REG_DFID_OFFSET                       0x0018
#define AR9_P2_VLAN_REG_DFID_SHIFT                        30
#define AR9_P2_VLAN_REG_DFID_SIZE                         2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define AR9_P2_VLAN_REG_TBVE_OFFSET                       0x0018
#define AR9_P2_VLAN_REG_TBVE_SHIFT                        29
#define AR9_P2_VLAN_REG_TBVE_SIZE                         1
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define AR9_P2_VLAN_REG_IFNTE_OFFSET                      0x0018
#define AR9_P2_VLAN_REG_IFNTE_SHIFT                       28
#define AR9_P2_VLAN_REG_IFNTE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define AR9_P2_VLAN_REG_VC_OFFSET                         0x0018
#define AR9_P2_VLAN_REG_VC_SHIFT                          27
#define AR9_P2_VLAN_REG_VC_SIZE                           1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define AR9_P2_VLAN_REG_VSD_OFFSET                        0x0018
#define AR9_P2_VLAN_REG_VSD_SHIFT                         26
#define AR9_P2_VLAN_REG_VSD_SIZE                          1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define AR9_P2_VLAN_REG_AOVTP_OFFSET                      0x0018
#define AR9_P2_VLAN_REG_AOVTP_SHIFT                       25
#define AR9_P2_VLAN_REG_AOVTP_SIZE                        1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define AR9_P2_VLAN_REG_VMCE_OFFSET                       0x0018
#define AR9_P2_VLAN_REG_VMCE_SHIFT                        24
#define AR9_P2_VLAN_REG_VMCE_SIZE                         1
/* Bit: 'DVPM' */
/* Description: 'Default VLAN Port Map' */
#define AR9_P2_VLAN_REG_DVPM_OFFSET                       0x0018
#define AR9_P2_VLAN_REG_DVPM_SHIFT                        16
#define AR9_P2_VLAN_REG_DVPM_SIZE                         8
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define AR9_P2_VLAN_REG_PP_OFFSET                         0x0018
#define AR9_P2_VLAN_REG_PP_SHIFT                          14
#define AR9_P2_VLAN_REG_PP_SIZE                           2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define AR9_P2_VLAN_REG_PPE_OFFSET                        0x0018
#define AR9_P2_VLAN_REG_PPE_SHIFT                         13
#define AR9_P2_VLAN_REG_PPE_SIZE                          1
/* Bit: 'PVTAGMP' */
/* Description: 'Port base VLAN tag member for Port 0' */
#define AR9_P2_VLAN_REG_PVTAGMP_OFFSET                    0x0018
#define AR9_P2_VLAN_REG_PVTAGMP_SHIFT                     12
#define AR9_P2_VLAN_REG_PVTAGMP_SIZE                      1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define AR9_P2_VLAN_REG_PVID_OFFSET                       0x0018
#define AR9_P2_VLAN_REG_PVID_SHIFT                        0
#define AR9_P2_VLAN_REG_PVID_SIZE                         12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Ingress Control Register' */
/* Bit: 'P0ITT' */
/* Description: 'Port 0 Ingress/Egress Timer Tick T selection' */
#define AR9_P0_INCTL_REG_P0ITT_OFFSET                     0x0020
#define AR9_P0_INCTL_REG_P0ITT_SHIFT                      11
#define AR9_P0_INCTL_REG_P0ITT_SIZE                       2
/* Bit: 'P0ITR' */
/* Description: 'Port 0 Ingress Token R' */
#define AR9_P0_INCTL_REG_P0ITR_OFFSET                     0x0020
#define AR9_P0_INCTL_REG_P0ITR_SHIFT                      0
#define AR9_P0_INCTL_REG_P0ITR_SIZE                       11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Ingress Control Register' */
/* Bit: 'P1ITT' */
/* Description: 'Port 1 Ingress/Egress Timer Tick T selection' */
#define AR9_P1_INCTL_REG_P1ITT_OFFSET                     0x0024
#define AR9_P1_INCTL_REG_P1ITT_SHIFT                      11
#define AR9_P1_INCTL_REG_P1ITT_SIZE                       2
/* Bit: 'P1ITR' */
/* Description: 'Port 1 Ingress Token R' */
#define AR9_P1_INCTL_REG_P1ITR_OFFSET                     0x0024
#define AR9_P1_INCTL_REG_P1ITR_SHIFT                      0
#define AR9_P1_INCTL_REG_P1ITR_SIZE                       11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Ingress Control Register' */
/* Bit: 'P2ITT' */
/* Description: 'Port 2 Ingress/Egress Timer Tick T selection' */
#define AR9_P2_INCTL_REG_P2ITT_OFFSET                     0x0028
#define AR9_P2_INCTL_REG_P2ITT_SHIFT                      11
#define AR9_P2_INCTL_REG_P2ITT_SIZE                       2
/* Bit: 'P2ITR' */
/* Description: 'Port 2 Ingress Token R' */
#define AR9_P2_INCTL_REG_P2ITR_OFFSET                     0x0028
#define AR9_P2_INCTL_REG_P2ITR_SHIFT                      0
#define AR9_P2_INCTL_REG_P2ITR_SIZE                       11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for Strict Q32 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define AR9_P0_ECS_Q32_REG_P0SPQ3TR_OFFSET                0x0030
#define AR9_P0_ECS_Q32_REG_P0SPQ3TR_SHIFT                 16
#define AR9_P0_ECS_Q32_REG_P0SPQ3TR_SIZE                  11
/* Bit: 'P0SPQ2TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q2' */
#define AR9_P0_ECS_Q32_REG_P0SPQ2TR_OFFSET                0x0030
#define AR9_P0_ECS_Q32_REG_P0SPQ2TR_SHIFT                 0
#define AR9_P0_ECS_Q32_REG_P0SPQ2TR_SIZE                  11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for Strict Q32 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define AR9_P1_ECS_Q32_REG_P0SPQ3TR_OFFSET                0x0040
#define AR9_P1_ECS_Q32_REG_P0SPQ3TR_SHIFT                 16
#define AR9_P1_ECS_Q32_REG_P0SPQ3TR_SIZE                  11
/* Bit: 'P0SPQ2TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q2' */
#define AR9_P1_ECS_Q32_REG_P0SPQ2TR_OFFSET                0x0040
#define AR9_P1_ECS_Q32_REG_P0SPQ2TR_SHIFT                 0
#define AR9_P1_ECS_Q32_REG_P0SPQ2TR_SIZE                  11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for Strict Q32 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define AR9_P2_ECS_Q32_REG_P0SPQ3TR_OFFSET                0x0050
#define AR9_P2_ECS_Q32_REG_P0SPQ3TR_SHIFT                 16
#define AR9_P2_ECS_Q32_REG_P0SPQ3TR_SIZE                  11
/* Bit: 'P0SPQ2TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q2' */
#define AR9_P2_ECS_Q32_REG_P0SPQ2TR_OFFSET                0x0050
#define AR9_P2_ECS_Q32_REG_P0SPQ2TR_SHIFT                 0
#define AR9_P2_ECS_Q32_REG_P0SPQ2TR_SIZE                  11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for Strict Q10 Register' */
/* Bit: 'P0SPQ1TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q1' */
#define AR9_P0_ECS_Q10_REG_P0SPQ1TR_OFFSET                0x0034
#define AR9_P0_ECS_Q10_REG_P0SPQ1TR_SHIFT                 16
#define AR9_P0_ECS_Q10_REG_P0SPQ1TR_SIZE                  11
/* Bit: 'P0SPQ0TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q0' */
#define AR9_P0_ECS_Q10_REG_P0SPQ0TR_OFFSET                0x0034
#define AR9_P0_ECS_Q10_REG_P0SPQ0TR_SHIFT                 0
#define AR9_P0_ECS_Q10_REG_P0SPQ0TR_SIZE                  11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for Strict Q10 Register' */
/* Bit: 'P0SPQ1TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q1' */
#define AR9_P1_ECS_Q10_REG_P0SPQ1TR_OFFSET                0x0044
#define AR9_P1_ECS_Q10_REG_P0SPQ1TR_SHIFT                 16
#define AR9_P1_ECS_Q10_REG_P0SPQ1TR_SIZE                  11
/* Bit: 'P0SPQ0TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q0' */
#define AR9_P1_ECS_Q10_REG_P0SPQ0TR_OFFSET                0x0044
#define AR9_P1_ECS_Q10_REG_P0SPQ0TR_SHIFT                 0
#define AR9_P1_ECS_Q10_REG_P0SPQ0TR_SIZE                  11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for Strict Q10 Register' */
/* Bit: 'P0SPQ1TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q1' */
#define AR9_P2_ECS_Q10_REG_P0SPQ1TR_OFFSET                0x0054
#define AR9_P2_ECS_Q10_REG_P0SPQ1TR_SHIFT                 16
#define AR9_P2_ECS_Q10_REG_P0SPQ1TR_SIZE                  11
/* Bit: 'P0SPQ0TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q0' */
#define AR9_P2_ECS_Q10_REG_P0SPQ0TR_OFFSET                0x0054
#define AR9_P2_ECS_Q10_REG_P0SPQ0TR_SHIFT                 0
#define AR9_P2_ECS_Q10_REG_P0SPQ0TR_SIZE                  11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for WFQ Q32 Register' */
/* Bit: 'P0WQ3TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q3' */
#define AR9_P0_ECW_Q32_REG_P0WQ3TR_OFFSET                 0x0038
#define AR9_P0_ECW_Q32_REG_P0WQ3TR_SHIFT                  16
#define AR9_P0_ECW_Q32_REG_P0WQ3TR_SIZE                   11
/* Bit: 'P0WQ2TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q2' */
#define AR9_P0_ECW_Q32_REG_P0WQ2TR_OFFSET                 0x0038
#define AR9_P0_ECW_Q32_REG_P0WQ2TR_SHIFT                  0
#define AR9_P0_ECW_Q32_REG_P0WQ2TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for WFQ Q32 Register' */
/* Bit: 'P1WQ3TR' */
/* Description: 'Port 1 Egress Token R for WFQ Q3' */
#define AR9_P1_ECW_Q32_REG_P1WQ3TR_OFFSET                 0x0048
#define AR9_P1_ECW_Q32_REG_P1WQ3TR_SHIFT                  16
#define AR9_P1_ECW_Q32_REG_P1WQ3TR_SIZE                   11
/* Bit: 'P1WQ2TR' */
/* Description: 'Port 1 Egress Token R for WFQ Q2' */
#define AR9_P1_ECW_Q32_REG_P1WQ2TR_OFFSET                 0x0048
#define AR9_P1_ECW_Q32_REG_P1WQ2TR_SHIFT                  0
#define AR9_P1_ECW_Q32_REG_P1WQ2TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for WFQ Q32 Register' */
/* Bit: 'P2WQ3TR' */
/* Description: 'Port 2 Egress Token R for WFQ Q3' */
#define AR9_P2_ECW_Q32_REG_P2WQ3TR_OFFSET                 0x0058
#define AR9_P2_ECW_Q32_REG_P2WQ3TR_SHIFT                  16
#define AR9_P2_ECW_Q32_REG_P2WQ3TR_SIZE                   11
/* Bit: 'P2WQ2TR' */
/* Description: 'Port 2 Egress Token R for WFQ Q2' */
#define AR9_P2_ECW_Q32_REG_P2WQ2TR_OFFSET                 0x0058
#define AR9_P2_ECW_Q32_REG_P2WQ2TR_SHIFT                  0
#define AR9_P2_ECW_Q32_REG_P2WQ2TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for WFQ Q10 Register' */
/* Bit: 'P0WQ1TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q1' */
#define AR9_P0_ECW_Q10_REG_P0WQ1TR_OFFSET                 0x003C
#define AR9_P0_ECW_Q10_REG_P0WQ1TR_SHIFT                  16
#define AR9_P0_ECW_Q10_REG_P0WQ1TR_SIZE                   11
/* Bit: 'P0WQ0TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q0' */
#define AR9_P0_ECW_Q10_REG_P0WQ0TR_OFFSET                 0x003C
#define AR9_P0_ECW_Q10_REG_P0WQ0TR_SHIFT                  0
#define AR9_P0_ECW_Q10_REG_P0WQ0TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for WFQ Q10 Register' */
/* Bit: 'P1WQ1TR' */
/* Description: 'Port 1 Egress Token R for WFQ Q1' */
#define AR9_P1_ECW_Q10_REG_P1WQ1TR_OFFSET                 0x004C
#define AR9_P1_ECW_Q10_REG_P1WQ1TR_SHIFT                  16
#define AR9_P1_ECW_Q10_REG_P1WQ1TR_SIZE                   11
/* Bit: 'P1WQ0TR' */
/* Description: 'Port 1 Egress Token R for WFQ Q0' */
#define AR9_P1_ECW_Q10_REG_P1WQ0TR_OFFSET                 0x004C
#define AR9_P1_ECW_Q10_REG_P1WQ0TR_SHIFT                  0
#define AR9_P1_ECW_Q10_REG_P1WQ0TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for WFQ Q10 Register' */
/* Bit: 'P2WQ1TR' */
/* Description: 'Port 2 Egress Token R for WFQ Q1' */
#define AR9_P2_ECW_Q10_REG_P2WQ1TR_OFFSET                 0x005C
#define AR9_P2_ECW_Q10_REG_P2WQ1TR_SHIFT                  16
#define AR9_P2_ECW_Q10_REG_P2WQ1TR_SIZE                   11
/* Bit: 'P2WQ0TR' */
/* Description: 'Port 2 Egress Token R for WFQ Q0' */
#define AR9_P2_ECW_Q10_REG_P2WQ0TR_OFFSET                 0x005C
#define AR9_P2_ECW_Q10_REG_P2WQ0TR_SHIFT                  0
#define AR9_P2_ECW_Q10_REG_P2WQ0TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Interrupt Enable Register' */
/* Bit: 'DBFIE' */
/* Description: 'Data Buffer is Full Interrupt Enable' */
#define AR9_INT_ENA_REG_DBFIE_OFFSET                      0x0060
#define AR9_INT_ENA_REG_DBFIE_SHIFT                       7
#define AR9_INT_ENA_REG_DBFIE_SIZE                        1
/* Bit: 'DBNFIE' */
/* Description: 'Data Buffer is nearly Full Interrupt Enable' */
#define AR9_INT_ENA_REG_DBNFIE_OFFSET                     0x0060
#define AR9_INT_ENA_REG_DBNFIE_SHIFT                      6
#define AR9_INT_ENA_REG_DBNFIE_SIZE                       1
/* Bit: 'LTFIE' */
/* Description: 'Learning Table Full Interrupt Enable' */
#define AR9_INT_ENA_REG_LTFIE_OFFSET                      0x0060
#define AR9_INT_ENA_REG_LTFIE_SHIFT                       5
#define AR9_INT_ENA_REG_LTFIE_SIZE                        1
/* Bit: 'LTADIE' */
/* Description: 'Leaning Table Access Done Interrupt Enable' */
#define AR9_INT_ENA_REG_LTADIE_OFFSET                     0x0060
#define AR9_INT_ENA_REG_LTADIE_SHIFT                      4
#define AR9_INT_ENA_REG_LTADIE_SIZE                       1
/* Bit: 'PSVIE' */
/* Description: 'Port Security Violation Interrupt Enable' */
#define AR9_INT_ENA_REG_PSVIE_OFFSET                      0x0060
#define AR9_INT_ENA_REG_PSVIE_SHIFT                       1
#define AR9_INT_ENA_REG_PSVIE_SIZE                        3
/* Bit: 'PSCIE' */
/* Description: 'Port Status Change Interrupt Enable' */
#define AR9_INT_ENA_REG_PSCIE_OFFSET                      0x0060
#define AR9_INT_ENA_REG_PSCIE_SHIFT                       0
#define AR9_INT_ENA_REG_PSCIE_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'Interrupt Status Register' */
/* Bit: 'DBF' */
/* Description: 'Data Buffer is Full' */
#define AR9_INT_ST_REG_DBF_OFFSET                         0x0064
#define AR9_INT_ST_REG_DBF_SHIFT                          7
#define AR9_INT_ST_REG_DBF_SIZE                           1
/* Bit: 'DBNF' */
/* Description: 'Data Buffer is nearly Full' */
#define AR9_INT_ST_REG_DBNF_OFFSET                        0x0064
#define AR9_INT_ST_REG_DBNF_SHIFT                         6
#define AR9_INT_ST_REG_DBNF_SIZE                          1
/* Bit: 'LTF' */
/* Description: 'Learning Table Full' */
#define AR9_INT_ST_REG_LTF_OFFSET                         0x0064
#define AR9_INT_ST_REG_LTF_SHIFT                          5
#define AR9_INT_ST_REG_LTF_SIZE                           1
/* Bit: 'LTAD' */
/* Description: 'Leaning Table Access Done' */
#define AR9_INT_ST_REG_LTAD_OFFSET                        0x0064
#define AR9_INT_ST_REG_LTAD_SHIFT                         4
#define AR9_INT_ST_REG_LTAD_SIZE                          1
/* Bit: 'PSV' */
/* Description: 'Port Security Violation' */
#define AR9_INT_ST_REG_PSV_OFFSET                         0x0064
#define AR9_INT_ST_REG_PSV_SHIFT                          1
#define AR9_INT_ST_REG_PSV_SIZE                           3
/* Bit: 'PSC' */
/* Description: 'Port Status Change' */
#define AR9_INT_ST_REG_PSC_OFFSET                         0x0064
#define AR9_INT_ST_REG_PSC_SHIFT                          0
#define AR9_INT_ST_REG_PSC_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Switch Global Control Register 0' */
/* Bit: 'SE' */
/* Description: 'Switch Enable' */
#define AR9_SW_GCTL0_REG_SE_OFFSET                        0x0068
#define AR9_SW_GCTL0_REG_SE_SHIFT                         31
#define AR9_SW_GCTL0_REG_SE_SIZE                          1
/* Bit: 'ICRCCD' */
/* Description: 'CRC Check Disable' */
#define AR9_SW_GCTL0_REG_ICRCCD_OFFSET                    0x0068
#define AR9_SW_GCTL0_REG_ICRCCD_SHIFT                     30
#define AR9_SW_GCTL0_REG_ICRCCD_SIZE                      1
/* Bit: 'LPE' */
/* Description: 'Virtual Ports Over CPU Physical Port Enable' */
#define AR9_SW_GCTL0_REG_LPE_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_LPE_SHIFT                        29
#define AR9_SW_GCTL0_REG_LPE_SIZE                         1
/* Bit: 'RVID0' */
/* Description: 'Replace VID0' */
#define AR9_SW_GCTL0_REG_RVID0_OFFSET                     0x0068
#define AR9_SW_GCTL0_REG_RVID0_SHIFT                      28
#define AR9_SW_GCTL0_REG_RVID0_SIZE                       1
/* Bit: 'RVID1' */
/* Description: 'Replace VID1' */
#define AR9_SW_GCTL0_REG_RVID1_OFFSET                     0x0068
#define AR9_SW_GCTL0_REG_RVID1_SHIFT                      27
#define AR9_SW_GCTL0_REG_RVID1_SIZE                       1
/* Bit: 'RVIDFFF' */
/* Description: 'Replace VIDFFF' */
#define AR9_SW_GCTL0_REG_RVIDFFF_OFFSET                   0x0068
#define AR9_SW_GCTL0_REG_RVIDFFF_SHIFT                    26
#define AR9_SW_GCTL0_REG_RVIDFFF_SIZE                     1
/* Bit: 'PCR' */
/* Description: 'Priority Change Rule' */
#define AR9_SW_GCTL0_REG_PCR_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_PCR_SHIFT                        25
#define AR9_SW_GCTL0_REG_PCR_SIZE                         1
/* Bit: 'PCE' */
/* Description: 'Priority Change Enable' */
#define AR9_SW_GCTL0_REG_PCE_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_PCE_SHIFT                        24
#define AR9_SW_GCTL0_REG_PCE_SIZE                         1
/* Bit: 'TSIPGE' */
/* Description: 'Transmit Short IPG Enable' */
#define AR9_SW_GCTL0_REG_TSIPGE_OFFSET                    0x0068
#define AR9_SW_GCTL0_REG_TSIPGE_SHIFT                     23
#define AR9_SW_GCTL0_REG_TSIPGE_SIZE                      1
/* Bit: 'PHYBA' */
/* Description: 'PHY Base Address' */
#define AR9_SW_GCTL0_REG_PHYBA_OFFSET                     0x0068
#define AR9_SW_GCTL0_REG_PHYBA_SHIFT                      22
#define AR9_SW_GCTL0_REG_PHYBA_SIZE                       1
/* Bit: 'DPWECH' */
/* Description: 'Drop Packet When Excessive Collision Happen' */
#define AR9_SW_GCTL0_REG_DPWECH_OFFSET                    0x0068
#define AR9_SW_GCTL0_REG_DPWECH_SHIFT                     21
#define AR9_SW_GCTL0_REG_DPWECH_SIZE                      1
/* Bit: 'ATS' */
/* Description: 'Aging Timer Select' */
#define AR9_SW_GCTL0_REG_ATS_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_ATS_SHIFT                        18
#define AR9_SW_GCTL0_REG_ATS_SIZE                         3
/* Bit: 'MCA' */
/* Description: 'Mirror CRC Also' */
#define AR9_SW_GCTL0_REG_MCA_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_MCA_SHIFT                        17
#define AR9_SW_GCTL0_REG_MCA_SIZE                         1
/* Bit: 'MRA' */
/* Description: 'Mirror RXER Also' */
#define AR9_SW_GCTL0_REG_MRA_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_MRA_SHIFT                        16
#define AR9_SW_GCTL0_REG_MRA_SIZE                         1
/* Bit: 'MPA' */
/* Description: 'Mirror PAUSE Also' */
#define AR9_SW_GCTL0_REG_MPA_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_MPA_SHIFT                        15
#define AR9_SW_GCTL0_REG_MPA_SIZE                         1
/* Bit: 'MLA' */
/* Description: 'Mirror Long Also' */
#define AR9_SW_GCTL0_REG_MLA_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_MLA_SHIFT                        14
#define AR9_SW_GCTL0_REG_MLA_SIZE                         1
/* Bit: 'MSA' */
/* Description: 'Mirror Short Also' */
#define AR9_SW_GCTL0_REG_MSA_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_MSA_SHIFT                        13
#define AR9_SW_GCTL0_REG_MSA_SIZE                         1
/* Bit: 'SNIFFPN' */
/* Description: 'Sniffer port number' */
#define AR9_SW_GCTL0_REG_SNIFFPN_OFFSET                   0x0068
#define AR9_SW_GCTL0_REG_SNIFFPN_SHIFT                    10
#define AR9_SW_GCTL0_REG_SNIFFPN_SIZE                     3
/* Bit: 'MPL' */
/* Description: 'Max Packet Length (MAXPKTLEN)' */
#define AR9_SW_GCTL0_REG_MPL_OFFSET                       0x0068
#define AR9_SW_GCTL0_REG_MPL_SHIFT                        8
#define AR9_SW_GCTL0_REG_MPL_SIZE                         2
/* Bit: 'DMQ3' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q3)' */
#define AR9_SW_GCTL0_REG_DMQ3_OFFSET                      0x0068
#define AR9_SW_GCTL0_REG_DMQ3_SHIFT                       6
#define AR9_SW_GCTL0_REG_DMQ3_SIZE                        2
/* Bit: 'DMQ2' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q2)' */
#define AR9_SW_GCTL0_REG_DMQ2_OFFSET                      0x0068
#define AR9_SW_GCTL0_REG_DMQ2_SHIFT                       4
#define AR9_SW_GCTL0_REG_DMQ2_SIZE                        2
/* Bit: 'DMQ1' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q1)' */
#define AR9_SW_GCTL0_REG_DMQ1_OFFSET                      0x0068
#define AR9_SW_GCTL0_REG_DMQ1_SHIFT                       2
#define AR9_SW_GCTL0_REG_DMQ1_SIZE                        2
/* Bit: 'DMQ0' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q0)' */
#define AR9_SW_GCTL0_REG_DMQ0_OFFSET                      0x0068
#define AR9_SW_GCTL0_REG_DMQ0_SHIFT                       0
#define AR9_SW_GCTL0_REG_DMQ0_SIZE                        2
/* -------------------------------------------------------------------------- */
/* Register: 'Switch Global Control Register 1' */
/* Bit: 'BISTDN' */
/* Description: 'BIST Done' */
#define AR9_SW_GCTL1_REG_BISTDN_OFFSET                    0x006C
#define AR9_SW_GCTL1_REG_BISTDN_SHIFT                     27
#define AR9_SW_GCTL1_REG_BISTDN_SIZE                      1
/* Bit: 'EDSTX' */
/* Description: 'Enable drop scheme of TX and RX' */
#define AR9_SW_GCTL1_REG_EDSTX_OFFSET                     0x006C
#define AR9_SW_GCTL1_REG_EDSTX_SHIFT                      26
#define AR9_SW_GCTL1_REG_EDSTX_SIZE                       1
/* Bit: 'CTTX' */
/* Description: 'Congestion threshold for TX queue' */
#define AR9_SW_GCTL1_REG_CTTX_OFFSET                      0x006C
#define AR9_SW_GCTL1_REG_CTTX_SHIFT                       24
#define AR9_SW_GCTL1_REG_CTTX_SIZE                        2
/* Bit: 'IJT' */
/* Description: 'Input Jam Threshold' */
#define AR9_SW_GCTL1_REG_IJT_OFFSET                       0x006C
#define AR9_SW_GCTL1_REG_IJT_SHIFT                        21
#define AR9_SW_GCTL1_REG_IJT_SIZE                         3
/* Bit: 'DIVS' */
/* Description: 'Do not Identify VLAN after SNAP' */
#define AR9_SW_GCTL1_REG_DIVS_OFFSET                      0x006C
#define AR9_SW_GCTL1_REG_DIVS_SHIFT                       20
#define AR9_SW_GCTL1_REG_DIVS_SIZE                        1
/* Bit: 'DII6P' */
/* Description: 'Do not Identify IPV6 in PPPOE' */
#define AR9_SW_GCTL1_REG_DII6P_OFFSET                     0x006C
#define AR9_SW_GCTL1_REG_DII6P_SHIFT                      19
#define AR9_SW_GCTL1_REG_DII6P_SIZE                       1
/* Bit: 'DIIPS' */
/* Description: 'Do not Identify IP in PPPOE after SNAP' */
#define AR9_SW_GCTL1_REG_DIIPS_OFFSET                     0x006C
#define AR9_SW_GCTL1_REG_DIIPS_SHIFT                      18
#define AR9_SW_GCTL1_REG_DIIPS_SIZE                       1
/* Bit: 'DIE' */
/* Description: 'Do not Identify IPV6' */
#define AR9_SW_GCTL1_REG_DIE_OFFSET                       0x006C
#define AR9_SW_GCTL1_REG_DIE_SHIFT                        17
#define AR9_SW_GCTL1_REG_DIE_SIZE                         1
/* Bit: 'DIIP' */
/* Description: 'Do not Identify IPv4 in PPPOE' */
#define AR9_SW_GCTL1_REG_DIIP_OFFSET                      0x006C
#define AR9_SW_GCTL1_REG_DIIP_SHIFT                       16
#define AR9_SW_GCTL1_REG_DIIP_SIZE                        1
/* Bit: 'DIS' */
/* Description: 'Do not Identify SNAP' */
#define AR9_SW_GCTL1_REG_DIS_OFFSET                       0x006C
#define AR9_SW_GCTL1_REG_DIS_SHIFT                        15
#define AR9_SW_GCTL1_REG_DIS_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'Default Portmap Register' */
/* Bit: 'UP' */
/* Description: 'Unknown Unicast Portmap' */
#define AR9_DF_PORTMAP_REG_UP_OFFSET                      0x002C
#define AR9_DF_PORTMAP_REG_UP_SHIFT                       24
#define AR9_DF_PORTMAP_REG_UP_SIZE                        8
/* Bit: 'BP' */
/* Description: 'Broadcast Portmap' */
#define AR9_DF_PORTMAP_REG_BP_OFFSET                      0x002C
#define AR9_DF_PORTMAP_REG_BP_SHIFT                       16
#define AR9_DF_PORTMAP_REG_BP_SIZE                        8
/* Bit: 'MP' */
/* Description: 'Unknown Multicast Portmap' */
#define AR9_DF_PORTMAP_REG_MP_OFFSET                      0x002C
#define AR9_DF_PORTMAP_REG_MP_SHIFT                       8
#define AR9_DF_PORTMAP_REG_MP_SIZE                        8
/* Bit: 'RP' */
/* Description: 'Reserve Portmap' */
#define AR9_DF_PORTMAP_REG_RP_OFFSET                      0x002C
#define AR9_DF_PORTMAP_REG_RP_SHIFT                       0
#define AR9_DF_PORTMAP_REG_RP_SIZE                        8
/* -------------------------------------------------------------------------- */
/* Register: 'ARP/RARP Register' */
/* Bit: 'MACA' */
/* Description: 'MAC Control Action' */
#define AR9_ARP_REG_MACA_OFFSET                           0x0070
#define AR9_ARP_REG_MACA_SHIFT                            14
#define AR9_ARP_REG_MACA_SIZE                             2
/* Bit: 'UPT' */
/* Description: 'Unicast packet Treated as Cross_VLAN packet' */
#define AR9_ARP_REG_UPT_OFFSET                            0x0070
#define AR9_ARP_REG_UPT_SHIFT                             13
#define AR9_ARP_REG_UPT_SIZE                              1
/* Bit: 'RPT' */
/* Description: 'RARP Packet Treated as Cross_VLAN Packet' */
#define AR9_ARP_REG_RPT_OFFSET                            0x0070
#define AR9_ARP_REG_RPT_SHIFT                             12
#define AR9_ARP_REG_RPT_SIZE                              1
/* Bit: 'RAPA' */
/* Description: 'RARP/ARP Packet Action' */
#define AR9_ARP_REG_RAPA_OFFSET                           0x0070
#define AR9_ARP_REG_RAPA_SHIFT                            10
#define AR9_ARP_REG_RAPA_SIZE                             2
/* Bit: 'RAPPE' */
/* Description: 'RARP/ARP Packet Priority Enable' */
#define AR9_ARP_REG_RAPPE_OFFSET                          0x0070
#define AR9_ARP_REG_RAPPE_SHIFT                           9
#define AR9_ARP_REG_RAPPE_SIZE                            1
/* Bit: 'RAPP' */
/* Description: 'RARP/ARP Packet Priority' */
#define AR9_ARP_REG_RAPP_OFFSET                           0x0070
#define AR9_ARP_REG_RAPP_SHIFT                            7
#define AR9_ARP_REG_RAPP_SIZE                             2
/* Bit: 'RAPOTH' */
/* Description: 'RARP/ARP Packet Output Tag Handle' */
#define AR9_ARP_REG_RAPOTH_OFFSET                         0x0070
#define AR9_ARP_REG_RAPOTH_SHIFT                          5
#define AR9_ARP_REG_RAPOTH_SIZE                           2
/* Bit: 'APT' */
/* Description: 'ARP Packet Treated as Cross _ VLAN Packet' */
#define AR9_ARP_REG_APT_OFFSET                            0x0070
#define AR9_ARP_REG_APT_SHIFT                             4
#define AR9_ARP_REG_APT_SIZE                              1
/* Bit: 'RAPTM' */
/* Description: 'RARP/ARP Packet Treated as Management Packet' */
#define AR9_ARP_REG_RAPTM_OFFSET                          0x0070
#define AR9_ARP_REG_RAPTM_SHIFT                           3
#define AR9_ARP_REG_RAPTM_SIZE                            1
/* Bit: 'TAPTS' */
/* Description: 'RARP/ARP Packet Treated as Span Packet' */
#define AR9_ARP_REG_TAPTS_OFFSET                          0x0070
#define AR9_ARP_REG_TAPTS_SHIFT                           2
#define AR9_ARP_REG_TAPTS_SIZE                            1
/* Bit: 'TAP' */
/* Description: 'Trap ARP Packet' */
#define AR9_ARP_REG_TAP_OFFSET                            0x0070
#define AR9_ARP_REG_TAP_SHIFT                             1
#define AR9_ARP_REG_TAP_SIZE                              1
/* Bit: 'TRP' */
/* Description: 'Trap RARP Packet' */
#define AR9_ARP_REG_TRP_OFFSET                            0x0070
#define AR9_ARP_REG_TRP_SHIFT                             0
#define AR9_ARP_REG_TRP_SIZE                              1
/* -------------------------------------------------------------------------- */
/* Register: 'Storm control Register' */
/* Bit: 'STORM_10_TH' */
/* Description: '10M Threshold' */
#define AR9_STRM_CTL_REG_STORM_10_TH_OFFSET               0x0074
#define AR9_STRM_CTL_REG_STORM_10_TH_SHIFT                16
#define AR9_STRM_CTL_REG_STORM_10_TH_SIZE                 13
/* Bit: 'STORM_B' */
/* Description: 'Storm Enable for Broadcast Packets' */
#define AR9_STRM_CTL_REG_STORM_B_OFFSET                   0x0074
#define AR9_STRM_CTL_REG_STORM_B_SHIFT                    15
#define AR9_STRM_CTL_REG_STORM_B_SIZE                     1
/* Bit: 'STORM_M' */
/* Description: 'Storm Enable for Multicast Packets' */
#define AR9_STRM_CTL_REG_STORM_M_OFFSET                   0x0074
#define AR9_STRM_CTL_REG_STORM_M_SHIFT                    14
#define AR9_STRM_CTL_REG_STORM_M_SIZE                     1
/* Bit: 'STORM_U' */
/* Description: 'Storm Enable for Un-learned Unicast Packets' */
#define AR9_STRM_CTL_REG_STORM_U_OFFSET                   0x0074
#define AR9_STRM_CTL_REG_STORM_U_SHIFT                    13
#define AR9_STRM_CTL_REG_STORM_U_SIZE                     1
/* Bit: 'STORM_100_TH' */
/* Description: '100M Threshold' */
#define AR9_STRM_CTL_REG_STORM_100_TH_OFFSET              0x0074
#define AR9_STRM_CTL_REG_STORM_100_TH_SHIFT               0
#define AR9_STRM_CTL_REG_STORM_100_TH_SIZE                13
/* -------------------------------------------------------------------------- */
/* Register: 'RGMII/GMII Port Control Register' */
/* Bit: 'MCS' */
/* Description: 'Management Clock Select' */
#define AR9_RGMII_CTL_REG_MCS_OFFSET                      0x0078
#define AR9_RGMII_CTL_REG_MCS_SHIFT                       24
#define AR9_RGMII_CTL_REG_MCS_SIZE                        8
/* Bit: 'P1CKIO' */
/* Description: 'Interface Clk PAD I/O Select' */
#define AR9_RGMII_CTL_REG_P1CKIO_OFFSET                   0x0078
#define AR9_RGMII_CTL_REG_P1CKIO_SHIFT                    23
#define AR9_RGMII_CTL_REG_P1CKIO_SIZE                     1
/* Bit: 'P1Feq' */
/* Description: 'Interface Reverse MII Clk Frequency' */
#define AR9_RGMII_CTL_REG_P1FEQ_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P1FEQ_SHIFT                     22
#define AR9_RGMII_CTL_REG_P1FEQ_SIZE                      1
/* Bit: 'P0CKIO' */
/* Description: 'Interface Clk PAD I/O Select' */
#define AR9_RGMII_CTL_REG_P0CKIO_OFFSET                   0x0078
#define AR9_RGMII_CTL_REG_P0CKIO_SHIFT                    21
#define AR9_RGMII_CTL_REG_P0CKIO_SIZE                     1
/* Bit: 'P0Feq' */
/* Description: 'Interface Reverse MII Clk Frequency' */
#define AR9_RGMII_CTL_REG_P0FEQ_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P0FEQ_SHIFT                     20
#define AR9_RGMII_CTL_REG_P0FEQ_SIZE                      1
/* Bit: 'P1IS' */
/* Description: 'Interface Selection' */
#define AR9_RGMII_CTL_REG_P1IS_OFFSET                     0x0078
#define AR9_RGMII_CTL_REG_P1IS_SHIFT                      18
#define AR9_RGMII_CTL_REG_P1IS_SIZE                       2
/* Bit: 'P1RDLY' */
/* Description: 'Port 1 RGMII Rx Clock Delay' */
#define AR9_RGMII_CTL_REG_P1RDLY_OFFSET                   0x0078
#define AR9_RGMII_CTL_REG_P1RDLY_SHIFT                    16
#define AR9_RGMII_CTL_REG_P1RDLY_SIZE                     2
/* Bit: 'P1TDLY' */
/* Description: 'Port 1 RGMII Tx Clock Delay' */
#define AR9_RGMII_CTL_REG_P1TDLY_OFFSET                   0x0078
#define AR9_RGMII_CTL_REG_P1TDLY_SHIFT                    14
#define AR9_RGMII_CTL_REG_P1TDLY_SIZE                     2
/* Bit: 'P1SPD' */
/* Description: 'Port 1 Speed' */
#define AR9_RGMII_CTL_REG_P1SPD_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P1SPD_SHIFT                     12
#define AR9_RGMII_CTL_REG_P1SPD_SIZE                      2
/* Bit: 'P1DUP' */
/* Description: 'Port 1 Duplex mode' */
#define AR9_RGMII_CTL_REG_P1DUP_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P1DUP_SHIFT                     11
#define AR9_RGMII_CTL_REG_P1DUP_SIZE                      1
/* Bit: 'P1FCE' */
/* Description: 'Port 1 Flow Control Enable' */
#define AR9_RGMII_CTL_REG_P1FCE_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P1FCE_SHIFT                     10
#define AR9_RGMII_CTL_REG_P1FCE_SIZE                      1
/* Bit: 'P0IS' */
/* Description: 'Interface Selection' */
#define AR9_RGMII_CTL_REG_P0IS_OFFSET                     0x0078
#define AR9_RGMII_CTL_REG_P0IS_SHIFT                      8
#define AR9_RGMII_CTL_REG_P0IS_SIZE                       2
/* Bit: 'P0RDLY' */
/* Description: 'Port 0 RGMII Rx Clock Delay' */
#define AR9_RGMII_CTL_REG_P0RDLY_OFFSET                   0x0078
#define AR9_RGMII_CTL_REG_P0RDLY_SHIFT                    6
#define AR9_RGMII_CTL_REG_P0RDLY_SIZE                     2
/* Bit: 'P0TDLY' */
/* Description: 'Port 0 RGMII Tx Clock Delay' */
#define AR9_RGMII_CTL_REG_P0TDLY_OFFSET                   0x0078
#define AR9_RGMII_CTL_REG_P0TDLY_SHIFT                    4
#define AR9_RGMII_CTL_REG_P0TDLY_SIZE                     2
/* Bit: 'P0SPD' */
/* Description: 'Port 0 Speed' */
#define AR9_RGMII_CTL_REG_P0SPD_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P0SPD_SHIFT                     2
#define AR9_RGMII_CTL_REG_P0SPD_SIZE                      2
/* Bit: 'P0DUP' */
/* Description: 'Port 0 Duplex mode' */
#define AR9_RGMII_CTL_REG_P0DUP_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P0DUP_SHIFT                     1
#define AR9_RGMII_CTL_REG_P0DUP_SIZE                      1
/* Bit: 'P0FCE' */
/* Description: 'Port 0 Flow Control Enable' */
#define AR9_RGMII_CTL_REG_P0FCE_OFFSET                    0x0078
#define AR9_RGMII_CTL_REG_P0FCE_SHIFT                     0
#define AR9_RGMII_CTL_REG_P0FCE_SIZE                      1
/* -------------------------------------------------------------------------- */
/* Register: '802.1p Priority Map Register' */
/* Bit: '1PPQ7' */
/* Description: 'Priority Queue 7' */
#define AR9_1P_PRT_REG_1PPQ7_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ7_SHIFT                        14
#define AR9_1P_PRT_REG_1PPQ7_SIZE                         2
/* Bit: '1PPQ6' */
/* Description: 'Priority Queue 6' */
#define AR9_1P_PRT_REG_1PPQ6_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ6_SHIFT                        12
#define AR9_1P_PRT_REG_1PPQ6_SIZE                         2
/* Bit: '1PPQ5' */
/* Description: 'Priority Queue 5' */
#define AR9_1P_PRT_REG_1PPQ5_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ5_SHIFT                        10
#define AR9_1P_PRT_REG_1PPQ5_SIZE                         2
/* Bit: '1PPQ4' */
/* Description: 'Priority Queue 4' */
#define AR9_1P_PRT_REG_1PPQ4_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ4_SHIFT                        8
#define AR9_1P_PRT_REG_1PPQ4_SIZE                         2
/* Bit: '1PPQ3' */
/* Description: 'Priority Queue 3' */
#define AR9_1P_PRT_REG_1PPQ3_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ3_SHIFT                        6
#define AR9_1P_PRT_REG_1PPQ3_SIZE                         2
/* Bit: '1PPQ2' */
/* Description: 'Priority Queue 2' */
#define AR9_1P_PRT_REG_1PPQ2_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ2_SHIFT                        4
#define AR9_1P_PRT_REG_1PPQ2_SIZE                         2
/* Bit: '1PPQ1' */
/* Description: 'Priority Queue 1' */
#define AR9_1P_PRT_REG_1PPQ1_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ1_SHIFT                        2
#define AR9_1P_PRT_REG_1PPQ1_SIZE                         2
/* Bit: '1PPQ0' */
/* Description: 'Priority Queue 0' */
#define AR9_1P_PRT_REG_1PPQ0_OFFSET                       0x007C
#define AR9_1P_PRT_REG_1PPQ0_SHIFT                        0
#define AR9_1P_PRT_REG_1PPQ0_SIZE                         2
/* -------------------------------------------------------------------------- */
/* Register: 'Ingress Policing Flow Control On Watermark' */
/* Bit: 'F' */
/* Description: 'Ingress Policing Flow Control On Watermark' */
#define AR9_PAUSE_ON_WM_F_OFFSET                          0x0080
#define AR9_PAUSE_ON_WM_F_SHIFT                           0
#define AR9_PAUSE_ON_WM_F_SIZE                            18
/* -------------------------------------------------------------------------- */
/* Register: 'Ingress Policing Flow Control Off Watermark' */
/* Bit: 'B' */
/* Description: 'Ingress Policing Flow Control Off Watermark' */
#define AR9_PAUSE_OFF_WM_B_OFFSET                         0x0084
#define AR9_PAUSE_OFF_WM_B_SHIFT                          0
#define AR9_PAUSE_OFF_WM_B_SIZE                           18
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer Threshold Register' */
/* Bit: 'PUO2' */
/* Description: 'Port 2 Based Flow Control Off Watermark Offset' */
#define AR9_BF_TH_REG_PUO2_OFFSET                         0x0088
#define AR9_BF_TH_REG_PUO2_SHIFT                          30
#define AR9_BF_TH_REG_PUO2_SIZE                           2
/* Bit: 'PUO1' */
/* Description: 'Port 1 Based Flow Control Off Watermark Offset' */
#define AR9_BF_TH_REG_PUO1_OFFSET                         0x0088
#define AR9_BF_TH_REG_PUO1_SHIFT                          28
#define AR9_BF_TH_REG_PUO1_SIZE                           2
/* Bit: 'PUO0' */
/* Description: 'Port 0 Based Flow Control Off Watermark Offset' */
#define AR9_BF_TH_REG_PUO0_OFFSET                         0x0088
#define AR9_BF_TH_REG_PUO0_SHIFT                          26
#define AR9_BF_TH_REG_PUO0_SIZE                           2
/* Bit: 'PFO2' */
/* Description: 'Port 2 Based Flow Control On Watermark Offset' */
#define AR9_BF_TH_REG_PFO2_OFFSET                         0x0088
#define AR9_BF_TH_REG_PFO2_SHIFT                          22
#define AR9_BF_TH_REG_PFO2_SIZE                           2
/* Bit: 'PFO1' */
/* Description: 'Port 1 Based Flow Control On Watermark Offset' */
#define AR9_BF_TH_REG_PFO1_OFFSET                         0x0088
#define AR9_BF_TH_REG_PFO1_SHIFT                          20
#define AR9_BF_TH_REG_PFO1_SIZE                           2
/* Bit: 'PFO0' */
/* Description: 'Port 0 Based Flow Control On Watermark Offset' */
#define AR9_BF_TH_REG_PFO0_OFFSET                         0x0088
#define AR9_BF_TH_REG_PFO0_SHIFT                          18
#define AR9_BF_TH_REG_PFO0_SIZE                           2
/* Bit: 'TLA' */
/* Description: 'Global Flow Control Stop Watermark Selection' */
#define AR9_BF_TH_REG_TLA_OFFSET                          0x0088
#define AR9_BF_TH_REG_TLA_SHIFT                           13
#define AR9_BF_TH_REG_TLA_SIZE                            1
/* Bit: 'THA' */
/* Description: 'Global Flow Control On Watermark Selection' */
#define AR9_BF_TH_REG_THA_OFFSET                          0x0088
#define AR9_BF_TH_REG_THA_SHIFT                           12
#define AR9_BF_TH_REG_THA_SIZE                            1
/* Bit: 'TLO' */
/* Description: 'Global Flow Control Off Watermark Offset' */
#define AR9_BF_TH_REG_TLO_OFFSET                          0x0088
#define AR9_BF_TH_REG_TLO_SHIFT                           10
#define AR9_BF_TH_REG_TLO_SIZE                            2
/* Bit: 'THO' */
/* Description: 'Global Flow Control On Watermark Offset' */
#define AR9_BF_TH_REG_THO_OFFSET                          0x0088
#define AR9_BF_TH_REG_THO_SHIFT                           8
#define AR9_BF_TH_REG_THO_SIZE                            2
/* Bit: 'PUA' */
/* Description: 'Port Based Flow Control Off Watermark Selection ' */
#define AR9_BF_TH_REG_PUA_OFFSET                          0x0088
#define AR9_BF_TH_REG_PUA_SHIFT                           5
#define AR9_BF_TH_REG_PUA_SIZE                            3
/* Bit: 'PFA' */
/* Description: 'Port Based Flow Control on Watermark Selection' */
#define AR9_BF_TH_REG_PFA_OFFSET                          0x0088
#define AR9_BF_TH_REG_PFA_SHIFT                           1
#define AR9_BF_TH_REG_PFA_SIZE                            3
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Header Control Register' */
/* Bit: 'RES' */
/* Description: 'Reserved' */
#define AR9_PMAC_HD_CTL_RES_OFFSET                        0x008C
#define AR9_PMAC_HD_CTL_RES_SHIFT                         23
#define AR9_PMAC_HD_CTL_RES_SIZE                          9
/* Bit: 'RXSH' */
/* Description: 'Status Header for Packets from DMA to PMAC' */
#define AR9_PMAC_HD_CTL_RXSH_OFFSET                       0x008C
#define AR9_PMAC_HD_CTL_RXSH_SHIFT                        22
#define AR9_PMAC_HD_CTL_RXSH_SIZE                         1
/* Bit: 'RL2' */
/* Description: 'Remove Layer-2 Header from Packets Going from PMAC
to DMA' */
#define AR9_PMAC_HD_CTL_RL2_OFFSET                        0x008C
#define AR9_PMAC_HD_CTL_RL2_SHIFT                         21
#define AR9_PMAC_HD_CTL_RL2_SIZE                          1
/* Bit: 'RC' */
/* Description: 'Remove CRC from Packets Going from PMAC to DMA' */
#define AR9_PMAC_HD_CTL_RC_OFFSET                         0x008C
#define AR9_PMAC_HD_CTL_RC_SHIFT                          20
#define AR9_PMAC_HD_CTL_RC_SIZE                           1
/* Bit: 'AS' */
/* Description: 'Status Header for Packets from PMAC to DMA' */
#define AR9_PMAC_HD_CTL_AS_OFFSET                         0x008C
#define AR9_PMAC_HD_CTL_AS_SHIFT                          19
#define AR9_PMAC_HD_CTL_AS_SIZE                           1
/* Bit: 'AC' */
/* Description: 'Add CRC for packets from DMA to PMAC' */
#define AR9_PMAC_HD_CTL_AC_OFFSET                         0x008C
#define AR9_PMAC_HD_CTL_AC_SHIFT                          18
#define AR9_PMAC_HD_CTL_AC_SIZE                           1
/* Bit: 'TYPE_LEN' */
/* Description: 'Contains the length/type value to the added to packets
from DMA to PMAC' */
#define AR9_PMAC_HD_CTL_TYPE_LEN_OFFSET                   0x008C
#define AR9_PMAC_HD_CTL_TYPE_LEN_SHIFT                    2
#define AR9_PMAC_HD_CTL_TYPE_LEN_SIZE                     16
/* Bit: 'TAG' */
/* Description: 'Add TAG to Packets from DMA to PMAC' */
#define AR9_PMAC_HD_CTL_TAG_OFFSET                        0x008C
#define AR9_PMAC_HD_CTL_TAG_SHIFT                         1
#define AR9_PMAC_HD_CTL_TAG_SIZE                          1
/* Bit: 'ADD' */
/* Description: 'ADD Header to Packets from DMA to PMAC' */
#define AR9_PMAC_HD_CTL_ADD_OFFSET                        0x008C
#define AR9_PMAC_HD_CTL_ADD_SHIFT                         0
#define AR9_PMAC_HD_CTL_ADD_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Source Address Register 1' */
/* Bit: 'SA_47_32' */
/* Description: 'Source Address to be inserted as a part of the Ethernet
header.' */
#define AR9_PMAC_SA1_SA_47_32_OFFSET                      0x0090
#define AR9_PMAC_SA1_SA_47_32_SHIFT                       0
#define AR9_PMAC_SA1_SA_47_32_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Source Address Register 2' */
/* Bit: 'SA_31_0' */
/* Description: 'Source Address' */
#define AR9_PMAC_SA2_SA_31_0_OFFSET                       0x0094
#define AR9_PMAC_SA2_SA_31_0_SHIFT                        0
#define AR9_PMAC_SA2_SA_31_0_SIZE                         32
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Destination Address Register 1' */
/* Bit: 'DA_47_32' */
/* Description: 'Destination Address' */
#define AR9_PMAC_DA1_DA_47_32_OFFSET                      0x0098
#define AR9_PMAC_DA1_DA_47_32_SHIFT                       0
#define AR9_PMAC_DA1_DA_47_32_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Destination Address Register 2' */
/* Bit: 'DA_31_0' */
/* Description: 'Destination Address to be inserted as a part of
the Ethernet header.' */
#define AR9_PMAC_DA2_DA_31_0_OFFSET                       0x009C
#define AR9_PMAC_DA2_DA_31_0_SHIFT                        0
#define AR9_PMAC_DA2_DA_31_0_SIZE                         32
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC VLAN Register' */
/* Bit: 'PRI' */
/* Description: 'Priority to be inserted as a part of VLAN tag' */
#define AR9_PMAC_VLAN_PRI_OFFSET                          0x00A0
#define AR9_PMAC_VLAN_PRI_SHIFT                           13
#define AR9_PMAC_VLAN_PRI_SIZE                            3
/* Bit: 'CFI' */
/* Description: 'CFI bit to be inserted as a part of VLAN tag' */
#define AR9_PMAC_VLAN_CFI_OFFSET                          0x00A0
#define AR9_PMAC_VLAN_CFI_SHIFT                           12
#define AR9_PMAC_VLAN_CFI_SIZE                            1
/* Bit: 'VLAN ID' */
/* Description: 'VLAN ID to be inserted as a part of VLAN tag' */
#define AR9_PMAC_VLAN_VLAN_ID_OFFSET                      0x00A0
#define AR9_PMAC_VLAN_VLAN_ID_SHIFT                       0
#define AR9_PMAC_VLAN_VLAN_ID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC TX IPG Counter Register' */
/* Bit: 'IPG_CNT' */
/* Description: 'IPG Counter' */
#define AR9_PMAC_TX_IPG_IPG_CNT_OFFSET                    0x00A4
#define AR9_PMAC_TX_IPG_IPG_CNT_SHIFT                     0
#define AR9_PMAC_TX_IPG_IPG_CNT_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC RX IPG Counter Register' */
/* Bit: 'IDIS_REQ_WM' */
/* Description: 'Disable RX FIFO Request Watermark' */
#define AR9_PMAC_RX_IPG_IDIS_REQ_WM_OFFSET                0x00A8
#define AR9_PMAC_RX_IPG_IDIS_REQ_WM_SHIFT                 8
#define AR9_PMAC_RX_IPG_IDIS_REQ_WM_SIZE                  1
/* Bit: 'IREQ_WM' */
/* Description: 'RX FIFO Request Watermark' */
#define AR9_PMAC_RX_IPG_IREQ_WM_OFFSET                    0x00A8
#define AR9_PMAC_RX_IPG_IREQ_WM_SHIFT                     4
#define AR9_PMAC_RX_IPG_IREQ_WM_SIZE                      4
/* Bit: 'IPG_CNT' */
/* Description: 'IPG Counter' */
#define AR9_PMAC_RX_IPG_IPG_CNT_OFFSET                    0x00A8
#define AR9_PMAC_RX_IPG_IPG_CNT_SHIFT                     0
#define AR9_PMAC_RX_IPG_IPG_CNT_SIZE                      4
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 0 Register' */
/* Bit: 'ADDR31_0' */
/* Description: 'Address [31:0]' */
#define AR9_ADR_TB_CTL0_REG_ADDR31_0_OFFSET               0x00AC
#define AR9_ADR_TB_CTL0_REG_ADDR31_0_SHIFT                0
#define AR9_ADR_TB_CTL0_REG_ADDR31_0_SIZE                 32
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 1 Register' */
/* Bit: 'PMAP' */
/* Description: 'Port Map' */
#define AR9_ADR_TB_CTL1_REG_PMAP_OFFSET                   0x00B0
#define AR9_ADR_TB_CTL1_REG_PMAP_SHIFT                    20
#define AR9_ADR_TB_CTL1_REG_PMAP_SIZE                     8
/* Bit: 'FID' */
/* Description: 'FID group' */
#define AR9_ADR_TB_CTL1_REG_FID_OFFSET                    0x00B0
#define AR9_ADR_TB_CTL1_REG_FID_SHIFT                     16
#define AR9_ADR_TB_CTL1_REG_FID_SIZE                      2
/* Bit: 'ADDR47_32' */
/* Description: 'Address [47:32]' */
#define AR9_ADR_TB_CTL1_REG_ADDR47_32_OFFSET              0x00B0
#define AR9_ADR_TB_CTL1_REG_ADDR47_32_SHIFT               0
#define AR9_ADR_TB_CTL1_REG_ADDR47_32_SIZE                16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 2 Register' */
/* Bit: 'IFCE' */
/* Description: 'Find Within Current HASH' */
#define AR9_ADR_TB_CTL2_REG_IFCE_OFFSET                   0x00B4
#define AR9_ADR_TB_CTL2_REG_IFCE_SHIFT                    23
#define AR9_ADR_TB_CTL2_REG_IFCE_SIZE                     1
/* Bit: 'CMD' */
/* Description: 'Command' */
#define AR9_ADR_TB_CTL2_REG_CMD_OFFSET                    0x00B4
#define AR9_ADR_TB_CTL2_REG_CMD_SHIFT                     20
#define AR9_ADR_TB_CTL2_REG_CMD_SIZE                      3
/* Bit: 'AC' */
/* Description: 'Access Control' */
#define AR9_ADR_TB_CTL2_REG_AC_OFFSET                     0x00B4
#define AR9_ADR_TB_CTL2_REG_AC_SHIFT                      16
#define AR9_ADR_TB_CTL2_REG_AC_SIZE                       4
/* Bit: 'INFOT' */
/* Description: 'Info Type: Static address' */
#define AR9_ADR_TB_CTL2_REG_INFOT_OFFSET                  0x00B4
#define AR9_ADR_TB_CTL2_REG_INFOT_SHIFT                   12
#define AR9_ADR_TB_CTL2_REG_INFOT_SIZE                    1
/* Bit: 'ITAT' */
/* Description: 'Info_Ctrl/Age Timer' */
#define AR9_ADR_TB_CTL2_REG_ITAT_OFFSET                   0x00B4
#define AR9_ADR_TB_CTL2_REG_ITAT_SHIFT                    0
#define AR9_ADR_TB_CTL2_REG_ITAT_SIZE                     11
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 0 Register' */
/* Bit: 'ADDRS31_0' */
/* Description: 'Address [31:0]' */
#define AR9_ADR_TB_ST0_REG_ADDRS31_0_OFFSET               0x00B8
#define AR9_ADR_TB_ST0_REG_ADDRS31_0_SHIFT                0
#define AR9_ADR_TB_ST0_REG_ADDRS31_0_SIZE                 32
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 1 Register' */
/* Bit: 'PMAPS' */
/* Description: 'Port Map' */
#define AR9_ADR_TB_ST1_REG_PMAPS_OFFSET                   0x00BC
#define AR9_ADR_TB_ST1_REG_PMAPS_SHIFT                    20
#define AR9_ADR_TB_ST1_REG_PMAPS_SIZE                     8
/* Bit: 'FIDS' */
/* Description: 'FID group' */
#define AR9_ADR_TB_ST1_REG_FIDS_OFFSET                    0x00BC
#define AR9_ADR_TB_ST1_REG_FIDS_SHIFT                     16
#define AR9_ADR_TB_ST1_REG_FIDS_SIZE                      2
/* Bit: 'ADDRS47_32' */
/* Description: 'Address [47:32]' */
#define AR9_ADR_TB_ST1_REG_ADDRS47_32_OFFSET              0x00BC
#define AR9_ADR_TB_ST1_REG_ADDRS47_32_SHIFT               0
#define AR9_ADR_TB_ST1_REG_ADDRS47_32_SIZE                16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 2 Register' */
/* Bit: 'BUSY' */
/* Description: 'Busy: Access Logic is Busy' */
#define AR9_ADR_TB_ST2_REG_BUSY_OFFSET                    0x00C0
#define AR9_ADR_TB_ST2_REG_BUSY_SHIFT                     31
#define AR9_ADR_TB_ST2_REG_BUSY_SIZE                      1
/* Bit: 'RSLT' */
/* Description: 'Result' */
#define AR9_ADR_TB_ST2_REG_RSLT_OFFSET                    0x00C0
#define AR9_ADR_TB_ST2_REG_RSLT_SHIFT                     28
#define AR9_ADR_TB_ST2_REG_RSLT_SIZE                      3
/* Bit: 'CMD' */
/* Description: 'Command' */
#define AR9_ADR_TB_ST2_REG_CMD_OFFSET                     0x00C0
#define AR9_ADR_TB_ST2_REG_CMD_SHIFT                      20
#define AR9_ADR_TB_ST2_REG_CMD_SIZE                       3
/* Bit: 'AC' */
/* Description: 'Access Control' */
#define AR9_ADR_TB_ST2_REG_AC_OFFSET                      0x00C0
#define AR9_ADR_TB_ST2_REG_AC_SHIFT                       16
#define AR9_ADR_TB_ST2_REG_AC_SIZE                        4
/* Bit: 'BAD' */
/* Description: 'Bad Status' */
#define AR9_ADR_TB_ST2_REG_BAD_OFFSET                     0x00C0
#define AR9_ADR_TB_ST2_REG_BAD_SHIFT                      14
#define AR9_ADR_TB_ST2_REG_BAD_SIZE                       1
/* Bit: 'OCP' */
/* Description: 'Occupy Status' */
#define AR9_ADR_TB_ST2_REG_OCP_OFFSET                     0x00C0
#define AR9_ADR_TB_ST2_REG_OCP_SHIFT                      13
#define AR9_ADR_TB_ST2_REG_OCP_SIZE                       1
/* Bit: 'INFOTS' */
/* Description: 'Info Type: Static address' */
#define AR9_ADR_TB_ST2_REG_INFOTS_OFFSET                  0x00C0
#define AR9_ADR_TB_ST2_REG_INFOTS_SHIFT                   12
#define AR9_ADR_TB_ST2_REG_INFOTS_SIZE                    1
/* Bit: 'ITATS' */
/* Description: 'Info_Ctrl/Age Timer Status' */
#define AR9_ADR_TB_ST2_REG_ITATS_OFFSET                   0x00C0
#define AR9_ADR_TB_ST2_REG_ITATS_SHIFT                    0
#define AR9_ADR_TB_ST2_REG_ITATS_SIZE                     11
/* -------------------------------------------------------------------------- */
/* Register: 'RMON Counter Control Register' */
/* Bit: 'BAS' */
/* Description: 'Busy/Access Start' */
#define AR9_RMON_CTL_REG_BAS_OFFSET                       0x00C4
#define AR9_RMON_CTL_REG_BAS_SHIFT                        11
#define AR9_RMON_CTL_REG_BAS_SIZE                         1
/* Bit: 'CAC' */
/* Description: 'Command for access counter' */
#define AR9_RMON_CTL_REG_CAC_OFFSET                       0x00C4
#define AR9_RMON_CTL_REG_CAC_SHIFT                        9
#define AR9_RMON_CTL_REG_CAC_SIZE                         2
/* Bit: 'PORTC' */
/* Description: 'Port' */
#define AR9_RMON_CTL_REG_PORTC_OFFSET                     0x00C4
#define AR9_RMON_CTL_REG_PORTC_SHIFT                      6
#define AR9_RMON_CTL_REG_PORTC_SIZE                       3
/* Bit: 'OFFSET' */
/* Description: 'Counter Offset' */
#define AR9_RMON_CTL_REG_OFFSET_OFFSET                    0x00C4
#define AR9_RMON_CTL_REG_OFFSET_SHIFT                     0
#define AR9_RMON_CTL_REG_OFFSET_SIZE                      6
/* -------------------------------------------------------------------------- */
/* Register: 'RMON Counter Status Register' */
/* Bit: 'COUNTER' */
/* Description: 'RMON Counter field 31:0 or counter field 63:32' */
#define AR9_RMON_ST_REG_COUNTER_OFFSET                    0x00C8
#define AR9_RMON_ST_REG_COUNTER_SHIFT                     0
#define AR9_RMON_ST_REG_COUNTER_SIZE                      32
/* -------------------------------------------------------------------------- */
/* Register: 'MDIO Indirect Access Control' */
/* Bit: 'WD' */
/* Description: 'The Write Data to the MDIO register' */
#define AR9_MDIO_CTL_REG_WD_OFFSET                        0x00CC
#define AR9_MDIO_CTL_REG_WD_SHIFT                         16
#define AR9_MDIO_CTL_REG_WD_SIZE                          16
/* Bit: 'MBUSY' */
/* Description: 'Busy state' */
#define AR9_MDIO_CTL_REG_MBUSY_OFFSET                     0x00CC
#define AR9_MDIO_CTL_REG_MBUSY_SHIFT                      15
#define AR9_MDIO_CTL_REG_MBUSY_SIZE                       1
/* Bit: 'OP' */
/* Description: 'Operation Code' */
#define AR9_MDIO_CTL_REG_OP_OFFSET                        0x00CC
#define AR9_MDIO_CTL_REG_OP_SHIFT                         10
#define AR9_MDIO_CTL_REG_OP_SIZE                          2
/* Bit: 'PHYAD' */
/* Description: 'PHY Address' */
#define AR9_MDIO_CTL_REG_PHYAD_OFFSET                     0x00CC
#define AR9_MDIO_CTL_REG_PHYAD_SHIFT                      5
#define AR9_MDIO_CTL_REG_PHYAD_SIZE                       5
/* Bit: 'REGAD' */
/* Description: 'Register Address' */
#define AR9_MDIO_CTL_REG_REGAD_OFFSET                     0x00CC
#define AR9_MDIO_CTL_REG_REGAD_SHIFT                      0
#define AR9_MDIO_CTL_REG_REGAD_SIZE                       5
/* -------------------------------------------------------------------------- */
/* Register: 'MDIO Indirect Read Data' */
/* Bit: 'RD' */
/* Description: 'The Read Data' */
#define AR9_MDIO_DATA_REG_RD_OFFSET                       0x00D0
#define AR9_MDIO_DATA_REG_RD_SHIFT                        0
#define AR9_MDIO_DATA_REG_RD_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter Action' */
/* Bit: 'QATF7' */
/* Description: 'Destination Queue for Type Filter 7' */
#define AR9_TP_FLT_ACT_REG_QATF7_OFFSET                   0x00D4
#define AR9_TP_FLT_ACT_REG_QATF7_SHIFT                    30
#define AR9_TP_FLT_ACT_REG_QATF7_SIZE                     2
/* Bit: 'QATF6' */
/* Description: 'Destination Queue for Type Filter 6' */
#define AR9_TP_FLT_ACT_REG_QATF6_OFFSET                   0x00D4
#define AR9_TP_FLT_ACT_REG_QATF6_SHIFT                    28
#define AR9_TP_FLT_ACT_REG_QATF6_SIZE                     2
/* Bit: 'QTF5' */
/* Description: 'Destination Queue for Type Filter 5' */
#define AR9_TP_FLT_ACT_REG_QTF5_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_QTF5_SHIFT                     26
#define AR9_TP_FLT_ACT_REG_QTF5_SIZE                      2
/* Bit: 'QTF4' */
/* Description: 'Destination Queue for Type Filter 4' */
#define AR9_TP_FLT_ACT_REG_QTF4_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_QTF4_SHIFT                     24
#define AR9_TP_FLT_ACT_REG_QTF4_SIZE                      2
/* Bit: 'QTF3' */
/* Description: 'Destination Queue for Type Filter 3' */
#define AR9_TP_FLT_ACT_REG_QTF3_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_QTF3_SHIFT                     22
#define AR9_TP_FLT_ACT_REG_QTF3_SIZE                      2
/* Bit: 'QTF2' */
/* Description: 'Destination Queue for Type Filter 2' */
#define AR9_TP_FLT_ACT_REG_QTF2_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_QTF2_SHIFT                     20
#define AR9_TP_FLT_ACT_REG_QTF2_SIZE                      2
/* Bit: 'QTF1' */
/* Description: 'Destination Queue for Type Filter 1' */
#define AR9_TP_FLT_ACT_REG_QTF1_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_QTF1_SHIFT                     18
#define AR9_TP_FLT_ACT_REG_QTF1_SIZE                      2
/* Bit: 'QTF0' */
/* Description: 'Destination Queue for Type Filter 0' */
#define AR9_TP_FLT_ACT_REG_QTF0_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_QTF0_SHIFT                     16
#define AR9_TP_FLT_ACT_REG_QTF0_SIZE                      2
/* Bit: 'ATF7' */
/* Description: 'Action for Type Filter 7' */
#define AR9_TP_FLT_ACT_REG_ATF7_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF7_SHIFT                     14
#define AR9_TP_FLT_ACT_REG_ATF7_SIZE                      2
/* Bit: 'ATF6' */
/* Description: 'Action for Type Filter 6' */
#define AR9_TP_FLT_ACT_REG_ATF6_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF6_SHIFT                     12
#define AR9_TP_FLT_ACT_REG_ATF6_SIZE                      2
/* Bit: 'ATF5' */
/* Description: 'Action for Type Filter 5' */
#define AR9_TP_FLT_ACT_REG_ATF5_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF5_SHIFT                     10
#define AR9_TP_FLT_ACT_REG_ATF5_SIZE                      2
/* Bit: 'ATF4' */
/* Description: 'Action for Type Filter 4' */
#define AR9_TP_FLT_ACT_REG_ATF4_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF4_SHIFT                     8
#define AR9_TP_FLT_ACT_REG_ATF4_SIZE                      2
/* Bit: 'ATF3' */
/* Description: 'Action for Type Filter 3' */
#define AR9_TP_FLT_ACT_REG_ATF3_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF3_SHIFT                     6
#define AR9_TP_FLT_ACT_REG_ATF3_SIZE                      2
/* Bit: 'ATF2' */
/* Description: 'Action for Type Filter 2' */
#define AR9_TP_FLT_ACT_REG_ATF2_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF2_SHIFT                     4
#define AR9_TP_FLT_ACT_REG_ATF2_SIZE                      2
/* Bit: 'ATF1' */
/* Description: 'Action for Type Filter 1' */
#define AR9_TP_FLT_ACT_REG_ATF1_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF1_SHIFT                     2
#define AR9_TP_FLT_ACT_REG_ATF1_SIZE                      2
/* Bit: 'ATF0' */
/* Description: 'Action for Type Filter 0' */
#define AR9_TP_FLT_ACT_REG_ATF0_OFFSET                    0x00D4
#define AR9_TP_FLT_ACT_REG_ATF0_SHIFT                     0
#define AR9_TP_FLT_ACT_REG_ATF0_SIZE                      2
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter Action' */
/* Bit: 'APF7' */
/* Description: 'Action for Protocol Filter 7' */
#define AR9_PRTCL_FLT_ACT_REG_APF7_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF7_SHIFT                  14
#define AR9_PRTCL_FLT_ACT_REG_APF7_SIZE                   2
/* Bit: 'APF6' */
/* Description: 'Action for Protocol Filter 6' */
#define AR9_PRTCL_FLT_ACT_REG_APF6_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF6_SHIFT                  12
#define AR9_PRTCL_FLT_ACT_REG_APF6_SIZE                   2
/* Bit: 'APF5' */
/* Description: 'Action for Protocol Filter 5' */
#define AR9_PRTCL_FLT_ACT_REG_APF5_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF5_SHIFT                  10
#define AR9_PRTCL_FLT_ACT_REG_APF5_SIZE                   2
/* Bit: 'APF4' */
/* Description: 'Action for Protocol Filter 4' */
#define AR9_PRTCL_FLT_ACT_REG_APF4_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF4_SHIFT                  8
#define AR9_PRTCL_FLT_ACT_REG_APF4_SIZE                   2
/* Bit: 'APF3' */
/* Description: 'Action for Protocol Filter 3' */
#define AR9_PRTCL_FLT_ACT_REG_APF3_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF3_SHIFT                  6
#define AR9_PRTCL_FLT_ACT_REG_APF3_SIZE                   2
/* Bit: 'APF2' */
/* Description: 'Action for Protocol Filter 2' */
#define AR9_PRTCL_FLT_ACT_REG_APF2_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF2_SHIFT                  4
#define AR9_PRTCL_FLT_ACT_REG_APF2_SIZE                   2
/* Bit: 'APF1' */
/* Description: 'Action for Protocol Filter 1' */
#define AR9_PRTCL_FLT_ACT_REG_APF1_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF1_SHIFT                  2
#define AR9_PRTCL_FLT_ACT_REG_APF1_SIZE                   2
/* Bit: 'APF0' */
/* Description: 'Action for Protocol Filter 0' */
#define AR9_PRTCL_FLT_ACT_REG_APF0_OFFSET                 0x00D8
#define AR9_PRTCL_FLT_ACT_REG_APF0_SHIFT                  0
#define AR9_PRTCL_FLT_ACT_REG_APF0_SIZE                   2
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 0' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT0_REG_M_OFFSET                        0x0100
#define AR9_VLAN_FLT0_REG_M_SHIFT                         24
#define AR9_VLAN_FLT0_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT0_REG_FID_OFFSET                      0x0100
#define AR9_VLAN_FLT0_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT0_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT0_REG_TM_OFFSET                       0x0100
#define AR9_VLAN_FLT0_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT0_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT0_REG_VV_OFFSET                       0x0100
#define AR9_VLAN_FLT0_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT0_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT0_REG_VP_OFFSET                       0x0100
#define AR9_VLAN_FLT0_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT0_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT0_REG_VID_OFFSET                      0x0100
#define AR9_VLAN_FLT0_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT0_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 1' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT1_REG_M_OFFSET                        0x0104
#define AR9_VLAN_FLT1_REG_M_SHIFT                         24
#define AR9_VLAN_FLT1_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT1_REG_FID_OFFSET                      0x0104
#define AR9_VLAN_FLT1_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT1_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT1_REG_TM_OFFSET                       0x0104
#define AR9_VLAN_FLT1_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT1_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT1_REG_VV_OFFSET                       0x0104
#define AR9_VLAN_FLT1_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT1_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT1_REG_VP_OFFSET                       0x0104
#define AR9_VLAN_FLT1_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT1_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT1_REG_VID_OFFSET                      0x0104
#define AR9_VLAN_FLT1_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT1_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 2' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT2_REG_M_OFFSET                        0x0108
#define AR9_VLAN_FLT2_REG_M_SHIFT                         24
#define AR9_VLAN_FLT2_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT2_REG_FID_OFFSET                      0x0108
#define AR9_VLAN_FLT2_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT2_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT2_REG_TM_OFFSET                       0x0108
#define AR9_VLAN_FLT2_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT2_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT2_REG_VV_OFFSET                       0x0108
#define AR9_VLAN_FLT2_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT2_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT2_REG_VP_OFFSET                       0x0108
#define AR9_VLAN_FLT2_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT2_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT2_REG_VID_OFFSET                      0x0108
#define AR9_VLAN_FLT2_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT2_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 3' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT3_REG_M_OFFSET                        0x010C
#define AR9_VLAN_FLT3_REG_M_SHIFT                         24
#define AR9_VLAN_FLT3_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT3_REG_FID_OFFSET                      0x010C
#define AR9_VLAN_FLT3_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT3_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT3_REG_TM_OFFSET                       0x010C
#define AR9_VLAN_FLT3_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT3_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT3_REG_VV_OFFSET                       0x010C
#define AR9_VLAN_FLT3_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT3_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT3_REG_VP_OFFSET                       0x010C
#define AR9_VLAN_FLT3_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT3_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT3_REG_VID_OFFSET                      0x010C
#define AR9_VLAN_FLT3_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT3_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 4' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT4_REG_M_OFFSET                        0x0110
#define AR9_VLAN_FLT4_REG_M_SHIFT                         24
#define AR9_VLAN_FLT4_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT4_REG_FID_OFFSET                      0x0110
#define AR9_VLAN_FLT4_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT4_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT4_REG_TM_OFFSET                       0x0110
#define AR9_VLAN_FLT4_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT4_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT4_REG_VV_OFFSET                       0x0110
#define AR9_VLAN_FLT4_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT4_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT4_REG_VP_OFFSET                       0x0110
#define AR9_VLAN_FLT4_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT4_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT4_REG_VID_OFFSET                      0x0110
#define AR9_VLAN_FLT4_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT4_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 5' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT5_REG_M_OFFSET                        0x0114
#define AR9_VLAN_FLT5_REG_M_SHIFT                         24
#define AR9_VLAN_FLT5_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT5_REG_FID_OFFSET                      0x0114
#define AR9_VLAN_FLT5_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT5_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT5_REG_TM_OFFSET                       0x0114
#define AR9_VLAN_FLT5_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT5_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT5_REG_VV_OFFSET                       0x0114
#define AR9_VLAN_FLT5_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT5_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT5_REG_VP_OFFSET                       0x0114
#define AR9_VLAN_FLT5_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT5_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT5_REG_VID_OFFSET                      0x0114
#define AR9_VLAN_FLT5_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT5_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 6' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT6_REG_M_OFFSET                        0x0118
#define AR9_VLAN_FLT6_REG_M_SHIFT                         24
#define AR9_VLAN_FLT6_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT6_REG_FID_OFFSET                      0x0118
#define AR9_VLAN_FLT6_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT6_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT6_REG_TM_OFFSET                       0x0118
#define AR9_VLAN_FLT6_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT6_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT6_REG_VV_OFFSET                       0x0118
#define AR9_VLAN_FLT6_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT6_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT6_REG_VP_OFFSET                       0x0118
#define AR9_VLAN_FLT6_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT6_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT6_REG_VID_OFFSET                      0x0118
#define AR9_VLAN_FLT6_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT6_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 7' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT7_REG_M_OFFSET                        0x011C
#define AR9_VLAN_FLT7_REG_M_SHIFT                         24
#define AR9_VLAN_FLT7_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT7_REG_FID_OFFSET                      0x011C
#define AR9_VLAN_FLT7_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT7_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT7_REG_TM_OFFSET                       0x011C
#define AR9_VLAN_FLT7_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT7_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT7_REG_VV_OFFSET                       0x011C
#define AR9_VLAN_FLT7_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT7_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT7_REG_VP_OFFSET                       0x011C
#define AR9_VLAN_FLT7_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT7_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT7_REG_VID_OFFSET                      0x011C
#define AR9_VLAN_FLT7_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT7_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 8' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT8_REG_M_OFFSET                        0x0120
#define AR9_VLAN_FLT8_REG_M_SHIFT                         24
#define AR9_VLAN_FLT8_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT8_REG_FID_OFFSET                      0x0120
#define AR9_VLAN_FLT8_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT8_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT8_REG_TM_OFFSET                       0x0120
#define AR9_VLAN_FLT8_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT8_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT8_REG_VV_OFFSET                       0x0120
#define AR9_VLAN_FLT8_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT8_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT8_REG_VP_OFFSET                       0x0120
#define AR9_VLAN_FLT8_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT8_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT8_REG_VID_OFFSET                      0x0120
#define AR9_VLAN_FLT8_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT8_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 9' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT9_REG_M_OFFSET                        0x0124
#define AR9_VLAN_FLT9_REG_M_SHIFT                         24
#define AR9_VLAN_FLT9_REG_M_SIZE                          8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT9_REG_FID_OFFSET                      0x0124
#define AR9_VLAN_FLT9_REG_FID_SHIFT                       22
#define AR9_VLAN_FLT9_REG_FID_SIZE                        2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT9_REG_TM_OFFSET                       0x0124
#define AR9_VLAN_FLT9_REG_TM_SHIFT                        19
#define AR9_VLAN_FLT9_REG_TM_SIZE                         3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT9_REG_VV_OFFSET                       0x0124
#define AR9_VLAN_FLT9_REG_VV_SHIFT                        15
#define AR9_VLAN_FLT9_REG_VV_SIZE                         1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT9_REG_VP_OFFSET                       0x0124
#define AR9_VLAN_FLT9_REG_VP_SHIFT                        12
#define AR9_VLAN_FLT9_REG_VP_SIZE                         3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT9_REG_VID_OFFSET                      0x0124
#define AR9_VLAN_FLT9_REG_VID_SHIFT                       0
#define AR9_VLAN_FLT9_REG_VID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 10' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT10_REG_M_OFFSET                       0x0128
#define AR9_VLAN_FLT10_REG_M_SHIFT                        24
#define AR9_VLAN_FLT10_REG_M_SIZE                         8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT10_REG_FID_OFFSET                     0x0128
#define AR9_VLAN_FLT10_REG_FID_SHIFT                      22
#define AR9_VLAN_FLT10_REG_FID_SIZE                       2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT10_REG_TM_OFFSET                      0x0128
#define AR9_VLAN_FLT10_REG_TM_SHIFT                       19
#define AR9_VLAN_FLT10_REG_TM_SIZE                        3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT10_REG_VV_OFFSET                      0x0128
#define AR9_VLAN_FLT10_REG_VV_SHIFT                       15
#define AR9_VLAN_FLT10_REG_VV_SIZE                        1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT10_REG_VP_OFFSET                      0x0128
#define AR9_VLAN_FLT10_REG_VP_SHIFT                       12
#define AR9_VLAN_FLT10_REG_VP_SIZE                        3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT10_REG_VID_OFFSET                     0x0128
#define AR9_VLAN_FLT10_REG_VID_SHIFT                      0
#define AR9_VLAN_FLT10_REG_VID_SIZE                       12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 11' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT11_REG_M_OFFSET                       0x012C
#define AR9_VLAN_FLT11_REG_M_SHIFT                        24
#define AR9_VLAN_FLT11_REG_M_SIZE                         8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT11_REG_FID_OFFSET                     0x012C
#define AR9_VLAN_FLT11_REG_FID_SHIFT                      22
#define AR9_VLAN_FLT11_REG_FID_SIZE                       2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT11_REG_TM_OFFSET                      0x012C
#define AR9_VLAN_FLT11_REG_TM_SHIFT                       19
#define AR9_VLAN_FLT11_REG_TM_SIZE                        3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT11_REG_VV_OFFSET                      0x012C
#define AR9_VLAN_FLT11_REG_VV_SHIFT                       15
#define AR9_VLAN_FLT11_REG_VV_SIZE                        1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT11_REG_VP_OFFSET                      0x012C
#define AR9_VLAN_FLT11_REG_VP_SHIFT                       12
#define AR9_VLAN_FLT11_REG_VP_SIZE                        3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT11_REG_VID_OFFSET                     0x012C
#define AR9_VLAN_FLT11_REG_VID_SHIFT                      0
#define AR9_VLAN_FLT11_REG_VID_SIZE                       12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 12' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT12_REG_M_OFFSET                       0x0130
#define AR9_VLAN_FLT12_REG_M_SHIFT                        24
#define AR9_VLAN_FLT12_REG_M_SIZE                         8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT12_REG_FID_OFFSET                     0x0130
#define AR9_VLAN_FLT12_REG_FID_SHIFT                      22
#define AR9_VLAN_FLT12_REG_FID_SIZE                       2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT12_REG_TM_OFFSET                      0x0130
#define AR9_VLAN_FLT12_REG_TM_SHIFT                       19
#define AR9_VLAN_FLT12_REG_TM_SIZE                        3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT12_REG_VV_OFFSET                      0x0130
#define AR9_VLAN_FLT12_REG_VV_SHIFT                       15
#define AR9_VLAN_FLT12_REG_VV_SIZE                        1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT12_REG_VP_OFFSET                      0x0130
#define AR9_VLAN_FLT12_REG_VP_SHIFT                       12
#define AR9_VLAN_FLT12_REG_VP_SIZE                        3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT12_REG_VID_OFFSET                     0x0130
#define AR9_VLAN_FLT12_REG_VID_SHIFT                      0
#define AR9_VLAN_FLT12_REG_VID_SIZE                       12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 13' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT13_REG_M_OFFSET                       0x0134
#define AR9_VLAN_FLT13_REG_M_SHIFT                        24
#define AR9_VLAN_FLT13_REG_M_SIZE                         8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT13_REG_FID_OFFSET                     0x0134
#define AR9_VLAN_FLT13_REG_FID_SHIFT                      22
#define AR9_VLAN_FLT13_REG_FID_SIZE                       2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT13_REG_TM_OFFSET                      0x0134
#define AR9_VLAN_FLT13_REG_TM_SHIFT                       19
#define AR9_VLAN_FLT13_REG_TM_SIZE                        3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT13_REG_VV_OFFSET                      0x0134
#define AR9_VLAN_FLT13_REG_VV_SHIFT                       15
#define AR9_VLAN_FLT13_REG_VV_SIZE                        1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT13_REG_VP_OFFSET                      0x0134
#define AR9_VLAN_FLT13_REG_VP_SHIFT                       12
#define AR9_VLAN_FLT13_REG_VP_SIZE                        3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT13_REG_VID_OFFSET                     0x0134
#define AR9_VLAN_FLT13_REG_VID_SHIFT                      0
#define AR9_VLAN_FLT13_REG_VID_SIZE                       12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 14' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT14_REG_M_OFFSET                       0x0138
#define AR9_VLAN_FLT14_REG_M_SHIFT                        24
#define AR9_VLAN_FLT14_REG_M_SIZE                         8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT14_REG_FID_OFFSET                     0x0138
#define AR9_VLAN_FLT14_REG_FID_SHIFT                      22
#define AR9_VLAN_FLT14_REG_FID_SIZE                       2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT14_REG_TM_OFFSET                      0x0138
#define AR9_VLAN_FLT14_REG_TM_SHIFT                       19
#define AR9_VLAN_FLT14_REG_TM_SIZE                        3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT14_REG_VV_OFFSET                      0x0138
#define AR9_VLAN_FLT14_REG_VV_SHIFT                       15
#define AR9_VLAN_FLT14_REG_VV_SIZE                        1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT14_REG_VP_OFFSET                      0x0138
#define AR9_VLAN_FLT14_REG_VP_SHIFT                       12
#define AR9_VLAN_FLT14_REG_VP_SIZE                        3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT14_REG_VID_OFFSET                     0x0138
#define AR9_VLAN_FLT14_REG_VID_SHIFT                      0
#define AR9_VLAN_FLT14_REG_VID_SIZE                       12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 15' */
/* Bit: 'M' */
/* Description: 'Member' */
#define AR9_VLAN_FLT15_REG_M_OFFSET                       0x013C
#define AR9_VLAN_FLT15_REG_M_SHIFT                        24
#define AR9_VLAN_FLT15_REG_M_SIZE                         8
/* Bit: 'FID' */
/* Description: 'FID' */
#define AR9_VLAN_FLT15_REG_FID_OFFSET                     0x013C
#define AR9_VLAN_FLT15_REG_FID_SHIFT                      22
#define AR9_VLAN_FLT15_REG_FID_SIZE                       2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define AR9_VLAN_FLT15_REG_TM_OFFSET                      0x013C
#define AR9_VLAN_FLT15_REG_TM_SHIFT                       19
#define AR9_VLAN_FLT15_REG_TM_SIZE                        3
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define AR9_VLAN_FLT15_REG_VV_OFFSET                      0x013C
#define AR9_VLAN_FLT15_REG_VV_SHIFT                       15
#define AR9_VLAN_FLT15_REG_VV_SIZE                        1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define AR9_VLAN_FLT15_REG_VP_OFFSET                      0x013C
#define AR9_VLAN_FLT15_REG_VP_SHIFT                       12
#define AR9_VLAN_FLT15_REG_VP_SIZE                        3
/* Bit: 'VID' */
/* Description: 'VID' */
#define AR9_VLAN_FLT15_REG_VID_OFFSET                     0x013C
#define AR9_VLAN_FLT15_REG_VID_SHIFT                      0
#define AR9_VLAN_FLT15_REG_VID_SIZE                       12
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 10' */
/* Bit: 'VCET1' */
/* Description: 'Value 1 Compared with Ether-Type' */
#define AR9_TP_FLT10_REG_VCET1_OFFSET                     0x0140
#define AR9_TP_FLT10_REG_VCET1_SHIFT                      16
#define AR9_TP_FLT10_REG_VCET1_SIZE                       16
/* Bit: 'VCET0' */
/* Description: 'Value 0 Compared with Ether-Type' */
#define AR9_TP_FLT10_REG_VCET0_OFFSET                     0x0140
#define AR9_TP_FLT10_REG_VCET0_SHIFT                      0
#define AR9_TP_FLT10_REG_VCET0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 32' */
/* Bit: 'VCET1' */
/* Description: 'Value 1 Compared with Ether-Type' */
#define AR9_TP_FLT32_REG_VCET1_OFFSET                     0x0144
#define AR9_TP_FLT32_REG_VCET1_SHIFT                      16
#define AR9_TP_FLT32_REG_VCET1_SIZE                       16
/* Bit: 'VCET0' */
/* Description: 'Value 0 Compared with Ether-Type' */
#define AR9_TP_FLT32_REG_VCET0_OFFSET                     0x0144
#define AR9_TP_FLT32_REG_VCET0_SHIFT                      0
#define AR9_TP_FLT32_REG_VCET0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 54' */
/* Bit: 'VCET1' */
/* Description: 'Value 1 Compared with Ether-Type' */
#define AR9_TP_FLT54_REG_VCET1_OFFSET                     0x0148
#define AR9_TP_FLT54_REG_VCET1_SHIFT                      16
#define AR9_TP_FLT54_REG_VCET1_SIZE                       16
/* Bit: 'VCET0' */
/* Description: 'Value 0 Compared with Ether-Type' */
#define AR9_TP_FLT54_REG_VCET0_OFFSET                     0x0148
#define AR9_TP_FLT54_REG_VCET0_SHIFT                      0
#define AR9_TP_FLT54_REG_VCET0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 76' */
/* Bit: 'VCET1' */
/* Description: 'Value 1 Compared with Ether-Type' */
#define AR9_TP_FLT76_REG_VCET1_OFFSET                     0x014C
#define AR9_TP_FLT76_REG_VCET1_SHIFT                      16
#define AR9_TP_FLT76_REG_VCET1_SIZE                       16
/* Bit: 'VCET0' */
/* Description: 'Value 0 Compared with Ether-Type' */
#define AR9_TP_FLT76_REG_VCET0_OFFSET                     0x014C
#define AR9_TP_FLT76_REG_VCET0_SHIFT                      0
#define AR9_TP_FLT76_REG_VCET0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 0' */
/* Bit: 'PQF' */
/* Description: 'Priority Queue F' */
#define AR9_DFSRV_MAP0_REG_PQF_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQF_SHIFT                      30
#define AR9_DFSRV_MAP0_REG_PQF_SIZE                       2
/* Bit: 'PQE' */
/* Description: 'Priority Queue E' */
#define AR9_DFSRV_MAP0_REG_PQE_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQE_SHIFT                      28
#define AR9_DFSRV_MAP0_REG_PQE_SIZE                       2
/* Bit: 'PQD' */
/* Description: 'Priority Queue D' */
#define AR9_DFSRV_MAP0_REG_PQD_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQD_SHIFT                      26
#define AR9_DFSRV_MAP0_REG_PQD_SIZE                       2
/* Bit: 'PQC' */
/* Description: 'Priority Queue C' */
#define AR9_DFSRV_MAP0_REG_PQC_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQC_SHIFT                      24
#define AR9_DFSRV_MAP0_REG_PQC_SIZE                       2
/* Bit: 'PQB' */
/* Description: 'Priority Queue B' */
#define AR9_DFSRV_MAP0_REG_PQB_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQB_SHIFT                      22
#define AR9_DFSRV_MAP0_REG_PQB_SIZE                       2
/* Bit: 'PQA' */
/* Description: 'Priority Queue A' */
#define AR9_DFSRV_MAP0_REG_PQA_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQA_SHIFT                      20
#define AR9_DFSRV_MAP0_REG_PQA_SIZE                       2
/* Bit: 'PQ9' */
/* Description: 'Priority Queue 9' */
#define AR9_DFSRV_MAP0_REG_PQ9_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ9_SHIFT                      18
#define AR9_DFSRV_MAP0_REG_PQ9_SIZE                       2
/* Bit: 'PQ8' */
/* Description: 'Priority Queue 8' */
#define AR9_DFSRV_MAP0_REG_PQ8_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ8_SHIFT                      16
#define AR9_DFSRV_MAP0_REG_PQ8_SIZE                       2
/* Bit: 'PQ7' */
/* Description: 'Priority Queue 7' */
#define AR9_DFSRV_MAP0_REG_PQ7_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ7_SHIFT                      14
#define AR9_DFSRV_MAP0_REG_PQ7_SIZE                       2
/* Bit: 'PQ6' */
/* Description: 'Priority Queue 6' */
#define AR9_DFSRV_MAP0_REG_PQ6_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ6_SHIFT                      12
#define AR9_DFSRV_MAP0_REG_PQ6_SIZE                       2
/* Bit: 'PQ5' */
/* Description: 'Priority Queue 5' */
#define AR9_DFSRV_MAP0_REG_PQ5_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ5_SHIFT                      10
#define AR9_DFSRV_MAP0_REG_PQ5_SIZE                       2
/* Bit: 'PQ4' */
/* Description: 'Priority Queue 4' */
#define AR9_DFSRV_MAP0_REG_PQ4_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ4_SHIFT                      8
#define AR9_DFSRV_MAP0_REG_PQ4_SIZE                       2
/* Bit: 'PQ3' */
/* Description: 'Priority Queue 3' */
#define AR9_DFSRV_MAP0_REG_PQ3_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ3_SHIFT                      6
#define AR9_DFSRV_MAP0_REG_PQ3_SIZE                       2
/* Bit: 'PQ2' */
/* Description: 'Priority Queue 2' */
#define AR9_DFSRV_MAP0_REG_PQ2_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ2_SHIFT                      4
#define AR9_DFSRV_MAP0_REG_PQ2_SIZE                       2
/* Bit: 'PQ1' */
/* Description: 'Priority Queue 1' */
#define AR9_DFSRV_MAP0_REG_PQ1_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ1_SHIFT                      2
#define AR9_DFSRV_MAP0_REG_PQ1_SIZE                       2
/* Bit: 'PQ0' */
/* Description: 'Priority Queue 0' */
#define AR9_DFSRV_MAP0_REG_PQ0_OFFSET                     0x0150
#define AR9_DFSRV_MAP0_REG_PQ0_SHIFT                      0
#define AR9_DFSRV_MAP0_REG_PQ0_SIZE                       2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 1' */
/* Bit: 'PQ1F' */
/* Description: 'Priority Queue 1F' */
#define AR9_DFSRV_MAP1_REG_PQ1F_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ1F_SHIFT                     30
#define AR9_DFSRV_MAP1_REG_PQ1F_SIZE                      2
/* Bit: 'PQ1E' */
/* Description: 'Priority Queue 1E' */
#define AR9_DFSRV_MAP1_REG_PQ1E_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ1E_SHIFT                     28
#define AR9_DFSRV_MAP1_REG_PQ1E_SIZE                      2
/* Bit: 'PQ1D' */
/* Description: 'Priority Queue 1D' */
#define AR9_DFSRV_MAP1_REG_PQ1D_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ1D_SHIFT                     26
#define AR9_DFSRV_MAP1_REG_PQ1D_SIZE                      2
/* Bit: 'PQ1C' */
/* Description: 'Priority Queue 1C' */
#define AR9_DFSRV_MAP1_REG_PQ1C_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ1C_SHIFT                     24
#define AR9_DFSRV_MAP1_REG_PQ1C_SIZE                      2
/* Bit: 'PQ1B' */
/* Description: 'Priority Queue 1B' */
#define AR9_DFSRV_MAP1_REG_PQ1B_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ1B_SHIFT                     22
#define AR9_DFSRV_MAP1_REG_PQ1B_SIZE                      2
/* Bit: 'PQ1A' */
/* Description: 'Priority Queue 1A' */
#define AR9_DFSRV_MAP1_REG_PQ1A_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ1A_SHIFT                     20
#define AR9_DFSRV_MAP1_REG_PQ1A_SIZE                      2
/* Bit: 'PQ19' */
/* Description: 'Priority Queue 19' */
#define AR9_DFSRV_MAP1_REG_PQ19_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ19_SHIFT                     18
#define AR9_DFSRV_MAP1_REG_PQ19_SIZE                      2
/* Bit: 'PQ18' */
/* Description: 'Priority Queue 18' */
#define AR9_DFSRV_MAP1_REG_PQ18_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ18_SHIFT                     16
#define AR9_DFSRV_MAP1_REG_PQ18_SIZE                      2
/* Bit: 'PQ17' */
/* Description: 'Priority Queue 17' */
#define AR9_DFSRV_MAP1_REG_PQ17_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ17_SHIFT                     14
#define AR9_DFSRV_MAP1_REG_PQ17_SIZE                      2
/* Bit: 'PQ16' */
/* Description: 'Priority Queue 16' */
#define AR9_DFSRV_MAP1_REG_PQ16_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ16_SHIFT                     12
#define AR9_DFSRV_MAP1_REG_PQ16_SIZE                      2
/* Bit: 'PQ15' */
/* Description: 'Priority Queue 15' */
#define AR9_DFSRV_MAP1_REG_PQ15_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ15_SHIFT                     10
#define AR9_DFSRV_MAP1_REG_PQ15_SIZE                      2
/* Bit: 'PQ14' */
/* Description: 'Priority Queue 14' */
#define AR9_DFSRV_MAP1_REG_PQ14_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ14_SHIFT                     8
#define AR9_DFSRV_MAP1_REG_PQ14_SIZE                      2
/* Bit: 'PQ13' */
/* Description: 'Priority Queue 13' */
#define AR9_DFSRV_MAP1_REG_PQ13_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ13_SHIFT                     6
#define AR9_DFSRV_MAP1_REG_PQ13_SIZE                      2
/* Bit: 'PQ12' */
/* Description: 'Priority Queue 12' */
#define AR9_DFSRV_MAP1_REG_PQ12_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ12_SHIFT                     4
#define AR9_DFSRV_MAP1_REG_PQ12_SIZE                      2
/* Bit: 'PQ11' */
/* Description: 'Priority Queue 11' */
#define AR9_DFSRV_MAP1_REG_PQ11_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ11_SHIFT                     2
#define AR9_DFSRV_MAP1_REG_PQ11_SIZE                      2
/* Bit: 'PQ10' */
/* Description: 'Priority Queue 10' */
#define AR9_DFSRV_MAP1_REG_PQ10_OFFSET                    0x0154
#define AR9_DFSRV_MAP1_REG_PQ10_SHIFT                     0
#define AR9_DFSRV_MAP1_REG_PQ10_SIZE                      2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 2' */
/* Bit: 'PQ2F' */
/* Description: 'Priority Queue 2F' */
#define AR9_DFSRV_MAP2_REG_PQ2F_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ2F_SHIFT                     30
#define AR9_DFSRV_MAP2_REG_PQ2F_SIZE                      2
/* Bit: 'PQ2E' */
/* Description: 'Priority Queue 2E' */
#define AR9_DFSRV_MAP2_REG_PQ2E_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ2E_SHIFT                     28
#define AR9_DFSRV_MAP2_REG_PQ2E_SIZE                      2
/* Bit: 'PQ2D' */
/* Description: 'Priority Queue 2D' */
#define AR9_DFSRV_MAP2_REG_PQ2D_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ2D_SHIFT                     26
#define AR9_DFSRV_MAP2_REG_PQ2D_SIZE                      2
/* Bit: 'PQ2C' */
/* Description: 'Priority Queue 2C' */
#define AR9_DFSRV_MAP2_REG_PQ2C_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ2C_SHIFT                     24
#define AR9_DFSRV_MAP2_REG_PQ2C_SIZE                      2
/* Bit: 'PQ2B' */
/* Description: 'Priority Queue 2B' */
#define AR9_DFSRV_MAP2_REG_PQ2B_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ2B_SHIFT                     22
#define AR9_DFSRV_MAP2_REG_PQ2B_SIZE                      2
/* Bit: 'PQ2A' */
/* Description: 'Priority Queue 2A' */
#define AR9_DFSRV_MAP2_REG_PQ2A_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ2A_SHIFT                     20
#define AR9_DFSRV_MAP2_REG_PQ2A_SIZE                      2
/* Bit: 'PQ29' */
/* Description: 'Priority Queue 29' */
#define AR9_DFSRV_MAP2_REG_PQ29_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ29_SHIFT                     18
#define AR9_DFSRV_MAP2_REG_PQ29_SIZE                      2
/* Bit: 'PQ28' */
/* Description: 'Priority Queue 28' */
#define AR9_DFSRV_MAP2_REG_PQ28_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ28_SHIFT                     16
#define AR9_DFSRV_MAP2_REG_PQ28_SIZE                      2
/* Bit: 'PQ27' */
/* Description: 'Priority Queue 27' */
#define AR9_DFSRV_MAP2_REG_PQ27_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ27_SHIFT                     14
#define AR9_DFSRV_MAP2_REG_PQ27_SIZE                      2
/* Bit: 'PQ26' */
/* Description: 'Priority Queue 26' */
#define AR9_DFSRV_MAP2_REG_PQ26_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ26_SHIFT                     12
#define AR9_DFSRV_MAP2_REG_PQ26_SIZE                      2
/* Bit: 'PQ25' */
/* Description: 'Priority Queue 25' */
#define AR9_DFSRV_MAP2_REG_PQ25_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ25_SHIFT                     10
#define AR9_DFSRV_MAP2_REG_PQ25_SIZE                      2
/* Bit: 'PQ24' */
/* Description: 'Priority Queue 24' */
#define AR9_DFSRV_MAP2_REG_PQ24_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ24_SHIFT                     8
#define AR9_DFSRV_MAP2_REG_PQ24_SIZE                      2
/* Bit: 'PQ23' */
/* Description: 'Priority Queue 23' */
#define AR9_DFSRV_MAP2_REG_PQ23_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ23_SHIFT                     6
#define AR9_DFSRV_MAP2_REG_PQ23_SIZE                      2
/* Bit: 'PQ22' */
/* Description: 'Priority Queue 22' */
#define AR9_DFSRV_MAP2_REG_PQ22_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ22_SHIFT                     4
#define AR9_DFSRV_MAP2_REG_PQ22_SIZE                      2
/* Bit: 'PQ21' */
/* Description: 'Priority Queue 21' */
#define AR9_DFSRV_MAP2_REG_PQ21_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ21_SHIFT                     2
#define AR9_DFSRV_MAP2_REG_PQ21_SIZE                      2
/* Bit: 'PQ20' */
/* Description: 'Priority Queue 20' */
#define AR9_DFSRV_MAP2_REG_PQ20_OFFSET                    0x0158
#define AR9_DFSRV_MAP2_REG_PQ20_SHIFT                     0
#define AR9_DFSRV_MAP2_REG_PQ20_SIZE                      2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 3' */
/* Bit: 'PQ3F' */
/* Description: 'Priority Queue 3F' */
#define AR9_DFSRV_MAP3_REG_PQ3F_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ3F_SHIFT                     30
#define AR9_DFSRV_MAP3_REG_PQ3F_SIZE                      2
/* Bit: 'PQ3E' */
/* Description: 'Priority Queue 3E' */
#define AR9_DFSRV_MAP3_REG_PQ3E_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ3E_SHIFT                     28
#define AR9_DFSRV_MAP3_REG_PQ3E_SIZE                      2
/* Bit: 'PQ3D' */
/* Description: 'Priority Queue 3D' */
#define AR9_DFSRV_MAP3_REG_PQ3D_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ3D_SHIFT                     26
#define AR9_DFSRV_MAP3_REG_PQ3D_SIZE                      2
/* Bit: 'PQ3C' */
/* Description: 'Priority Queue 3C' */
#define AR9_DFSRV_MAP3_REG_PQ3C_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ3C_SHIFT                     24
#define AR9_DFSRV_MAP3_REG_PQ3C_SIZE                      2
/* Bit: 'PQ3B' */
/* Description: 'Priority Queue 3B' */
#define AR9_DFSRV_MAP3_REG_PQ3B_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ3B_SHIFT                     22
#define AR9_DFSRV_MAP3_REG_PQ3B_SIZE                      2
/* Bit: 'PQ3A' */
/* Description: 'Priority Queue 3A' */
#define AR9_DFSRV_MAP3_REG_PQ3A_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ3A_SHIFT                     20
#define AR9_DFSRV_MAP3_REG_PQ3A_SIZE                      2
/* Bit: 'PQ39' */
/* Description: 'Priority Queue 39' */
#define AR9_DFSRV_MAP3_REG_PQ39_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ39_SHIFT                     18
#define AR9_DFSRV_MAP3_REG_PQ39_SIZE                      2
/* Bit: 'PQ38' */
/* Description: 'Priority Queue 38' */
#define AR9_DFSRV_MAP3_REG_PQ38_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ38_SHIFT                     16
#define AR9_DFSRV_MAP3_REG_PQ38_SIZE                      2
/* Bit: 'PQ37' */
/* Description: 'Priority Queue 37' */
#define AR9_DFSRV_MAP3_REG_PQ37_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ37_SHIFT                     14
#define AR9_DFSRV_MAP3_REG_PQ37_SIZE                      2
/* Bit: 'PQ36' */
/* Description: 'Priority Queue 36' */
#define AR9_DFSRV_MAP3_REG_PQ36_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ36_SHIFT                     12
#define AR9_DFSRV_MAP3_REG_PQ36_SIZE                      2
/* Bit: 'PQ35' */
/* Description: 'Priority Queue 35' */
#define AR9_DFSRV_MAP3_REG_PQ35_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ35_SHIFT                     10
#define AR9_DFSRV_MAP3_REG_PQ35_SIZE                      2
/* Bit: 'PQ34' */
/* Description: 'Priority Queue 34' */
#define AR9_DFSRV_MAP3_REG_PQ34_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ34_SHIFT                     8
#define AR9_DFSRV_MAP3_REG_PQ34_SIZE                      2
/* Bit: 'PQ33' */
/* Description: 'Priority Queue 33' */
#define AR9_DFSRV_MAP3_REG_PQ33_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ33_SHIFT                     6
#define AR9_DFSRV_MAP3_REG_PQ33_SIZE                      2
/* Bit: 'PQ32' */
/* Description: 'Priority Queue 32' */
#define AR9_DFSRV_MAP3_REG_PQ32_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ32_SHIFT                     4
#define AR9_DFSRV_MAP3_REG_PQ32_SIZE                      2
/* Bit: 'PQ31' */
/* Description: 'Priority Queue 31' */
#define AR9_DFSRV_MAP3_REG_PQ31_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ31_SHIFT                     2
#define AR9_DFSRV_MAP3_REG_PQ31_SIZE                      2
/* Bit: 'PQ30' */
/* Description: 'Priority Queue 30' */
#define AR9_DFSRV_MAP3_REG_PQ30_OFFSET                    0x015C
#define AR9_DFSRV_MAP3_REG_PQ30_SHIFT                     0
#define AR9_DFSRV_MAP3_REG_PQ30_SIZE                      2
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 0' */
/* Bit: 'ATUF0' */
/* Description: 'Action for TCP/UDP Port Filter 0' */
#define AR9_TCP_PF0_REG_ATUF0_OFFSET                      0x0160
#define AR9_TCP_PF0_REG_ATUF0_SHIFT                       28
#define AR9_TCP_PF0_REG_ATUF0_SIZE                        2
/* Bit: 'TUPF0' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 0' */
#define AR9_TCP_PF0_REG_TUPF0_OFFSET                      0x0160
#define AR9_TCP_PF0_REG_TUPF0_SHIFT                       26
#define AR9_TCP_PF0_REG_TUPF0_SIZE                        2
/* Bit: 'COMP0' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF0_REG_COMP0_OFFSET                      0x0160
#define AR9_TCP_PF0_REG_COMP0_SHIFT                       24
#define AR9_TCP_PF0_REG_COMP0_SIZE                        2
/* Bit: 'PRANGE0' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF0_REG_PRANGE0_OFFSET                    0x0160
#define AR9_TCP_PF0_REG_PRANGE0_SHIFT                     16
#define AR9_TCP_PF0_REG_PRANGE0_SIZE                      8
/* Bit: 'BASEPT0' */
/* Description: 'Base Port number 0' */
#define AR9_TCP_PF0_REG_BASEPT0_OFFSET                    0x0160
#define AR9_TCP_PF0_REG_BASEPT0_SHIFT                     0
#define AR9_TCP_PF0_REG_BASEPT0_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 1' */
/* Bit: 'ATUF1' */
/* Description: 'Action for TCP/UDP Port Filter 1' */
#define AR9_TCP_PF1_REG_ATUF1_OFFSET                      0x0164
#define AR9_TCP_PF1_REG_ATUF1_SHIFT                       28
#define AR9_TCP_PF1_REG_ATUF1_SIZE                        2
/* Bit: 'TUPF1' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 1' */
#define AR9_TCP_PF1_REG_TUPF1_OFFSET                      0x0164
#define AR9_TCP_PF1_REG_TUPF1_SHIFT                       26
#define AR9_TCP_PF1_REG_TUPF1_SIZE                        2
/* Bit: 'COMP1' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF1_REG_COMP1_OFFSET                      0x0164
#define AR9_TCP_PF1_REG_COMP1_SHIFT                       24
#define AR9_TCP_PF1_REG_COMP1_SIZE                        2
/* Bit: 'PRANGE1' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF1_REG_PRANGE1_OFFSET                    0x0164
#define AR9_TCP_PF1_REG_PRANGE1_SHIFT                     16
#define AR9_TCP_PF1_REG_PRANGE1_SIZE                      8
/* Bit: 'BASEPT1' */
/* Description: 'Base Port number 1' */
#define AR9_TCP_PF1_REG_BASEPT1_OFFSET                    0x0164
#define AR9_TCP_PF1_REG_BASEPT1_SHIFT                     0
#define AR9_TCP_PF1_REG_BASEPT1_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 2' */
/* Bit: 'ATUF2' */
/* Description: 'Action for TCP/UDP Port Filter 2' */
#define AR9_TCP_PF2_REG_ATUF2_OFFSET                      0x0168
#define AR9_TCP_PF2_REG_ATUF2_SHIFT                       28
#define AR9_TCP_PF2_REG_ATUF2_SIZE                        2
/* Bit: 'TUPF2' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 2' */
#define AR9_TCP_PF2_REG_TUPF2_OFFSET                      0x0168
#define AR9_TCP_PF2_REG_TUPF2_SHIFT                       26
#define AR9_TCP_PF2_REG_TUPF2_SIZE                        2
/* Bit: 'COMP2' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF2_REG_COMP2_OFFSET                      0x0168
#define AR9_TCP_PF2_REG_COMP2_SHIFT                       24
#define AR9_TCP_PF2_REG_COMP2_SIZE                        2
/* Bit: 'PRANGE2' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF2_REG_PRANGE2_OFFSET                    0x0168
#define AR9_TCP_PF2_REG_PRANGE2_SHIFT                     16
#define AR9_TCP_PF2_REG_PRANGE2_SIZE                      8
/* Bit: 'BASEPT2' */
/* Description: 'Base Port number 2' */
#define AR9_TCP_PF2_REG_BASEPT2_OFFSET                    0x0168
#define AR9_TCP_PF2_REG_BASEPT2_SHIFT                     0
#define AR9_TCP_PF2_REG_BASEPT2_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 3' */
/* Bit: 'ATUF3' */
/* Description: 'Action for TCP/UDP Port Filter 3' */
#define AR9_TCP_PF3_REG_ATUF3_OFFSET                      0x016C
#define AR9_TCP_PF3_REG_ATUF3_SHIFT                       28
#define AR9_TCP_PF3_REG_ATUF3_SIZE                        2
/* Bit: 'TUPF3' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 3' */
#define AR9_TCP_PF3_REG_TUPF3_OFFSET                      0x016C
#define AR9_TCP_PF3_REG_TUPF3_SHIFT                       26
#define AR9_TCP_PF3_REG_TUPF3_SIZE                        2
/* Bit: 'COMP3' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF3_REG_COMP3_OFFSET                      0x016C
#define AR9_TCP_PF3_REG_COMP3_SHIFT                       24
#define AR9_TCP_PF3_REG_COMP3_SIZE                        2
/* Bit: 'PRANGE3' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF3_REG_PRANGE3_OFFSET                    0x016C
#define AR9_TCP_PF3_REG_PRANGE3_SHIFT                     16
#define AR9_TCP_PF3_REG_PRANGE3_SIZE                      8
/* Bit: 'BASEPT3' */
/* Description: 'Base Port number 3' */
#define AR9_TCP_PF3_REG_BASEPT3_OFFSET                    0x016C
#define AR9_TCP_PF3_REG_BASEPT3_SHIFT                     0
#define AR9_TCP_PF3_REG_BASEPT3_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 4' */
/* Bit: 'ATUF4' */
/* Description: 'Action for TCP/UDP Port Filter 4' */
#define AR9_TCP_PF4_REG_ATUF4_OFFSET                      0x0170
#define AR9_TCP_PF4_REG_ATUF4_SHIFT                       28
#define AR9_TCP_PF4_REG_ATUF4_SIZE                        2
/* Bit: 'TUPF4' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 4' */
#define AR9_TCP_PF4_REG_TUPF4_OFFSET                      0x0170
#define AR9_TCP_PF4_REG_TUPF4_SHIFT                       26
#define AR9_TCP_PF4_REG_TUPF4_SIZE                        2
/* Bit: 'COMP4' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF4_REG_COMP4_OFFSET                      0x0170
#define AR9_TCP_PF4_REG_COMP4_SHIFT                       24
#define AR9_TCP_PF4_REG_COMP4_SIZE                        2
/* Bit: 'PRANGE4' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF4_REG_PRANGE4_OFFSET                    0x0170
#define AR9_TCP_PF4_REG_PRANGE4_SHIFT                     16
#define AR9_TCP_PF4_REG_PRANGE4_SIZE                      8
/* Bit: 'BASEPT4' */
/* Description: 'Base Port number 4' */
#define AR9_TCP_PF4_REG_BASEPT4_OFFSET                    0x0170
#define AR9_TCP_PF4_REG_BASEPT4_SHIFT                     0
#define AR9_TCP_PF4_REG_BASEPT4_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 5' */
/* Bit: 'ATUF5' */
/* Description: 'Action for TCP/UDP Port Filter 5' */
#define AR9_TCP_PF5_REG_ATUF5_OFFSET                      0x0174
#define AR9_TCP_PF5_REG_ATUF5_SHIFT                       28
#define AR9_TCP_PF5_REG_ATUF5_SIZE                        2
/* Bit: 'TUPF5' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 5' */
#define AR9_TCP_PF5_REG_TUPF5_OFFSET                      0x0174
#define AR9_TCP_PF5_REG_TUPF5_SHIFT                       26
#define AR9_TCP_PF5_REG_TUPF5_SIZE                        2
/* Bit: 'COMP5' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF5_REG_COMP5_OFFSET                      0x0174
#define AR9_TCP_PF5_REG_COMP5_SHIFT                       24
#define AR9_TCP_PF5_REG_COMP5_SIZE                        2
/* Bit: 'PRANGE5' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF5_REG_PRANGE5_OFFSET                    0x0174
#define AR9_TCP_PF5_REG_PRANGE5_SHIFT                     16
#define AR9_TCP_PF5_REG_PRANGE5_SIZE                      8
/* Bit: 'BASEPT5' */
/* Description: 'Base Port number 5' */
#define AR9_TCP_PF5_REG_BASEPT5_OFFSET                    0x0174
#define AR9_TCP_PF5_REG_BASEPT5_SHIFT                     0
#define AR9_TCP_PF5_REG_BASEPT5_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 6' */
/* Bit: 'ATUF6' */
/* Description: 'Action for TCP/UDP Port Filter 6' */
#define AR9_TCP_PF6_REG_ATUF6_OFFSET                      0x0178
#define AR9_TCP_PF6_REG_ATUF6_SHIFT                       28
#define AR9_TCP_PF6_REG_ATUF6_SIZE                        2
/* Bit: 'TUPF6' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 6' */
#define AR9_TCP_PF6_REG_TUPF6_OFFSET                      0x0178
#define AR9_TCP_PF6_REG_TUPF6_SHIFT                       26
#define AR9_TCP_PF6_REG_TUPF6_SIZE                        2
/* Bit: 'COMP6' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF6_REG_COMP6_OFFSET                      0x0178
#define AR9_TCP_PF6_REG_COMP6_SHIFT                       24
#define AR9_TCP_PF6_REG_COMP6_SIZE                        2
/* Bit: 'PRANGE6' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF6_REG_PRANGE6_OFFSET                    0x0178
#define AR9_TCP_PF6_REG_PRANGE6_SHIFT                     16
#define AR9_TCP_PF6_REG_PRANGE6_SIZE                      8
/* Bit: 'BASEPT6' */
/* Description: 'Base Port number 6' */
#define AR9_TCP_PF6_REG_BASEPT6_OFFSET                    0x0178
#define AR9_TCP_PF6_REG_BASEPT6_SHIFT                     0
#define AR9_TCP_PF6_REG_BASEPT6_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 7' */
/* Bit: 'ATUF7' */
/* Description: 'Action for TCP/UDP Port Filter 7' */
#define AR9_TCP_PF7_REG_ATUF7_OFFSET                      0x017C
#define AR9_TCP_PF7_REG_ATUF7_SHIFT                       28
#define AR9_TCP_PF7_REG_ATUF7_SIZE                        2
/* Bit: 'TUPF7' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 7' */
#define AR9_TCP_PF7_REG_TUPF7_OFFSET                      0x017C
#define AR9_TCP_PF7_REG_TUPF7_SHIFT                       26
#define AR9_TCP_PF7_REG_TUPF7_SIZE                        2
/* Bit: 'COMP7' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define AR9_TCP_PF7_REG_COMP7_OFFSET                      0x017C
#define AR9_TCP_PF7_REG_COMP7_SHIFT                       24
#define AR9_TCP_PF7_REG_COMP7_SIZE                        2
/* Bit: 'PRANGE7' */
/* Description: 'Port Range in TCP/UDP' */
#define AR9_TCP_PF7_REG_PRANGE7_OFFSET                    0x017C
#define AR9_TCP_PF7_REG_PRANGE7_SHIFT                     16
#define AR9_TCP_PF7_REG_PRANGE7_SIZE                      8
/* Bit: 'BASEPT7' */
/* Description: 'Base Port number 7' */
#define AR9_TCP_PF7_REG_BASEPT7_OFFSET                    0x017C
#define AR9_TCP_PF7_REG_BASEPT7_SHIFT                     0
#define AR9_TCP_PF7_REG_BASEPT7_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'Reserved DA(0180C2000003~0180C2000000) control register' */
/* Bit: 'RA03_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_03_00_REG_RA03_VALID_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA03_VALID_SHIFT                 31
#define AR9_RA_03_00_REG_RA03_VALID_SIZE                  1
/* Bit: 'RA03_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_03_00_REG_RA03_SPAN_OFFSET                 0x0180
#define AR9_RA_03_00_REG_RA03_SPAN_SHIFT                  30
#define AR9_RA_03_00_REG_RA03_SPAN_SIZE                   1
/* Bit: 'RA03_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_03_00_REG_RA03_MG_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA03_MG_SHIFT                    29
#define AR9_RA_03_00_REG_RA03_MG_SIZE                     1
/* Bit: 'RA03_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_03_00_REG_RA03_CV_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA03_CV_SHIFT                    28
#define AR9_RA_03_00_REG_RA03_CV_SIZE                     1
/* Bit: 'RA03_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_03_00_REG_RA03_TXTAG_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA03_TXTAG_SHIFT                 26
#define AR9_RA_03_00_REG_RA03_TXTAG_SIZE                  2
/* Bit: 'RA03_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_03_00_REG_RA03_ACT_OFFSET                  0x0180
#define AR9_RA_03_00_REG_RA03_ACT_SHIFT                   24
#define AR9_RA_03_00_REG_RA03_ACT_SIZE                    2
/* Bit: 'RA02_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_03_00_REG_RA02_VALID_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA02_VALID_SHIFT                 23
#define AR9_RA_03_00_REG_RA02_VALID_SIZE                  1
/* Bit: 'RA02_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_03_00_REG_RA02_SPAN_OFFSET                 0x0180
#define AR9_RA_03_00_REG_RA02_SPAN_SHIFT                  22
#define AR9_RA_03_00_REG_RA02_SPAN_SIZE                   1
/* Bit: 'RA02_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_03_00_REG_RA02_MG_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA02_MG_SHIFT                    21
#define AR9_RA_03_00_REG_RA02_MG_SIZE                     1
/* Bit: 'RA02_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_03_00_REG_RA02_CV_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA02_CV_SHIFT                    20
#define AR9_RA_03_00_REG_RA02_CV_SIZE                     1
/* Bit: 'RA02_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_03_00_REG_RA02_TXTAG_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA02_TXTAG_SHIFT                 18
#define AR9_RA_03_00_REG_RA02_TXTAG_SIZE                  2
/* Bit: 'RA02_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_03_00_REG_RA02_ACT_OFFSET                  0x0180
#define AR9_RA_03_00_REG_RA02_ACT_SHIFT                   16
#define AR9_RA_03_00_REG_RA02_ACT_SIZE                    2
/* Bit: 'RA01_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_03_00_REG_RA01_VALID_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA01_VALID_SHIFT                 15
#define AR9_RA_03_00_REG_RA01_VALID_SIZE                  1
/* Bit: 'RA01_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_03_00_REG_RA01_SPAN_OFFSET                 0x0180
#define AR9_RA_03_00_REG_RA01_SPAN_SHIFT                  14
#define AR9_RA_03_00_REG_RA01_SPAN_SIZE                   1
/* Bit: 'RA01_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_03_00_REG_RA01_MG_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA01_MG_SHIFT                    13
#define AR9_RA_03_00_REG_RA01_MG_SIZE                     1
/* Bit: 'RA01_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_03_00_REG_RA01_CV_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA01_CV_SHIFT                    12
#define AR9_RA_03_00_REG_RA01_CV_SIZE                     1
/* Bit: 'RA01_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_03_00_REG_RA01_TXTAG_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA01_TXTAG_SHIFT                 10
#define AR9_RA_03_00_REG_RA01_TXTAG_SIZE                  2
/* Bit: 'RA01_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_03_00_REG_RA01_ACT_OFFSET                  0x0180
#define AR9_RA_03_00_REG_RA01_ACT_SHIFT                   8
#define AR9_RA_03_00_REG_RA01_ACT_SIZE                    2
/* Bit: 'RA00_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_03_00_REG_RA00_VALID_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA00_VALID_SHIFT                 7
#define AR9_RA_03_00_REG_RA00_VALID_SIZE                  1
/* Bit: 'RA00_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_03_00_REG_RA00_SPAN_OFFSET                 0x0180
#define AR9_RA_03_00_REG_RA00_SPAN_SHIFT                  6
#define AR9_RA_03_00_REG_RA00_SPAN_SIZE                   1
/* Bit: 'RA00_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_03_00_REG_RA00_MG_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA00_MG_SHIFT                    5
#define AR9_RA_03_00_REG_RA00_MG_SIZE                     1
/* Bit: 'RA00_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_03_00_REG_RA00_CV_OFFSET                   0x0180
#define AR9_RA_03_00_REG_RA00_CV_SHIFT                    4
#define AR9_RA_03_00_REG_RA00_CV_SIZE                     1
/* Bit: 'RA00_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_03_00_REG_RA00_TXTAG_OFFSET                0x0180
#define AR9_RA_03_00_REG_RA00_TXTAG_SHIFT                 2
#define AR9_RA_03_00_REG_RA00_TXTAG_SIZE                  2
/* Bit: 'RA00_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_03_00_REG_RA00_ACT_OFFSET                  0x0180
#define AR9_RA_03_00_REG_RA00_ACT_SHIFT                   0
#define AR9_RA_03_00_REG_RA00_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000007~0180C2000004' */
/* Bit: 'RA13_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_07_04_REG_RA13_VALID_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA13_VALID_SHIFT                 31
#define AR9_RA_07_04_REG_RA13_VALID_SIZE                  1
/* Bit: 'RA13_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_07_04_REG_RA13_SPAN_OFFSET                 0x0184
#define AR9_RA_07_04_REG_RA13_SPAN_SHIFT                  30
#define AR9_RA_07_04_REG_RA13_SPAN_SIZE                   1
/* Bit: 'RA13_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_07_04_REG_RA13_MG_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA13_MG_SHIFT                    29
#define AR9_RA_07_04_REG_RA13_MG_SIZE                     1
/* Bit: 'RA13_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_07_04_REG_RA13_CV_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA13_CV_SHIFT                    28
#define AR9_RA_07_04_REG_RA13_CV_SIZE                     1
/* Bit: 'RA13_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_07_04_REG_RA13_TXTAG_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA13_TXTAG_SHIFT                 26
#define AR9_RA_07_04_REG_RA13_TXTAG_SIZE                  2
/* Bit: 'RA13_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_07_04_REG_RA13_ACT_OFFSET                  0x0184
#define AR9_RA_07_04_REG_RA13_ACT_SHIFT                   24
#define AR9_RA_07_04_REG_RA13_ACT_SIZE                    2
/* Bit: 'RA12_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_07_04_REG_RA12_VALID_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA12_VALID_SHIFT                 23
#define AR9_RA_07_04_REG_RA12_VALID_SIZE                  1
/* Bit: 'RA12_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_07_04_REG_RA12_SPAN_OFFSET                 0x0184
#define AR9_RA_07_04_REG_RA12_SPAN_SHIFT                  22
#define AR9_RA_07_04_REG_RA12_SPAN_SIZE                   1
/* Bit: 'RA12_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_07_04_REG_RA12_MG_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA12_MG_SHIFT                    21
#define AR9_RA_07_04_REG_RA12_MG_SIZE                     1
/* Bit: 'RA12_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_07_04_REG_RA12_CV_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA12_CV_SHIFT                    20
#define AR9_RA_07_04_REG_RA12_CV_SIZE                     1
/* Bit: 'RA12_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_07_04_REG_RA12_TXTAG_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA12_TXTAG_SHIFT                 18
#define AR9_RA_07_04_REG_RA12_TXTAG_SIZE                  2
/* Bit: 'RA12_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_07_04_REG_RA12_ACT_OFFSET                  0x0184
#define AR9_RA_07_04_REG_RA12_ACT_SHIFT                   16
#define AR9_RA_07_04_REG_RA12_ACT_SIZE                    2
/* Bit: 'RA11_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_07_04_REG_RA11_VALID_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA11_VALID_SHIFT                 15
#define AR9_RA_07_04_REG_RA11_VALID_SIZE                  1
/* Bit: 'RA11_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_07_04_REG_RA11_SPAN_OFFSET                 0x0184
#define AR9_RA_07_04_REG_RA11_SPAN_SHIFT                  14
#define AR9_RA_07_04_REG_RA11_SPAN_SIZE                   1
/* Bit: 'RA11_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_07_04_REG_RA11_MG_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA11_MG_SHIFT                    13
#define AR9_RA_07_04_REG_RA11_MG_SIZE                     1
/* Bit: 'RA11_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_07_04_REG_RA11_CV_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA11_CV_SHIFT                    12
#define AR9_RA_07_04_REG_RA11_CV_SIZE                     1
/* Bit: 'RA11_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_07_04_REG_RA11_TXTAG_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA11_TXTAG_SHIFT                 10
#define AR9_RA_07_04_REG_RA11_TXTAG_SIZE                  2
/* Bit: 'RA11_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_07_04_REG_RA11_ACT_OFFSET                  0x0184
#define AR9_RA_07_04_REG_RA11_ACT_SHIFT                   8
#define AR9_RA_07_04_REG_RA11_ACT_SIZE                    2
/* Bit: 'RA10_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_07_04_REG_RA10_VALID_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA10_VALID_SHIFT                 7
#define AR9_RA_07_04_REG_RA10_VALID_SIZE                  1
/* Bit: 'RA10_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_07_04_REG_RA10_SPAN_OFFSET                 0x0184
#define AR9_RA_07_04_REG_RA10_SPAN_SHIFT                  6
#define AR9_RA_07_04_REG_RA10_SPAN_SIZE                   1
/* Bit: 'RA10_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_07_04_REG_RA10_MG_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA10_MG_SHIFT                    5
#define AR9_RA_07_04_REG_RA10_MG_SIZE                     1
/* Bit: 'RA10_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_07_04_REG_RA10_CV_OFFSET                   0x0184
#define AR9_RA_07_04_REG_RA10_CV_SHIFT                    4
#define AR9_RA_07_04_REG_RA10_CV_SIZE                     1
/* Bit: 'RA10_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_07_04_REG_RA10_TXTAG_OFFSET                0x0184
#define AR9_RA_07_04_REG_RA10_TXTAG_SHIFT                 2
#define AR9_RA_07_04_REG_RA10_TXTAG_SIZE                  2
/* Bit: 'RA10_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_07_04_REG_RA10_ACT_OFFSET                  0x0184
#define AR9_RA_07_04_REG_RA10_ACT_SHIFT                   0
#define AR9_RA_07_04_REG_RA10_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200000B~0180C2000008' */
/* Bit: 'RA23_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_0B_08_REG_RA23_VALID_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA23_VALID_SHIFT                 31
#define AR9_RA_0B_08_REG_RA23_VALID_SIZE                  1
/* Bit: 'RA23_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_0B_08_REG_RA23_SPAN_OFFSET                 0x0188
#define AR9_RA_0B_08_REG_RA23_SPAN_SHIFT                  30
#define AR9_RA_0B_08_REG_RA23_SPAN_SIZE                   1
/* Bit: 'RA23_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_0B_08_REG_RA23_MG_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA23_MG_SHIFT                    29
#define AR9_RA_0B_08_REG_RA23_MG_SIZE                     1
/* Bit: 'RA23_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_0B_08_REG_RA23_CV_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA23_CV_SHIFT                    28
#define AR9_RA_0B_08_REG_RA23_CV_SIZE                     1
/* Bit: 'RA23_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_0B_08_REG_RA23_TXTAG_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA23_TXTAG_SHIFT                 26
#define AR9_RA_0B_08_REG_RA23_TXTAG_SIZE                  2
/* Bit: 'RA23_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_0B_08_REG_RA23_ACT_OFFSET                  0x0188
#define AR9_RA_0B_08_REG_RA23_ACT_SHIFT                   24
#define AR9_RA_0B_08_REG_RA23_ACT_SIZE                    2
/* Bit: 'RA22_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_0B_08_REG_RA22_VALID_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA22_VALID_SHIFT                 23
#define AR9_RA_0B_08_REG_RA22_VALID_SIZE                  1
/* Bit: 'RA22_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_0B_08_REG_RA22_SPAN_OFFSET                 0x0188
#define AR9_RA_0B_08_REG_RA22_SPAN_SHIFT                  22
#define AR9_RA_0B_08_REG_RA22_SPAN_SIZE                   1
/* Bit: 'RA22_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_0B_08_REG_RA22_MG_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA22_MG_SHIFT                    21
#define AR9_RA_0B_08_REG_RA22_MG_SIZE                     1
/* Bit: 'RA22_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_0B_08_REG_RA22_CV_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA22_CV_SHIFT                    20
#define AR9_RA_0B_08_REG_RA22_CV_SIZE                     1
/* Bit: 'RA22_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_0B_08_REG_RA22_TXTAG_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA22_TXTAG_SHIFT                 18
#define AR9_RA_0B_08_REG_RA22_TXTAG_SIZE                  2
/* Bit: 'RA22_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_0B_08_REG_RA22_ACT_OFFSET                  0x0188
#define AR9_RA_0B_08_REG_RA22_ACT_SHIFT                   16
#define AR9_RA_0B_08_REG_RA22_ACT_SIZE                    2
/* Bit: 'RA21_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_0B_08_REG_RA21_VALID_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA21_VALID_SHIFT                 15
#define AR9_RA_0B_08_REG_RA21_VALID_SIZE                  1
/* Bit: 'RA21_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_0B_08_REG_RA21_SPAN_OFFSET                 0x0188
#define AR9_RA_0B_08_REG_RA21_SPAN_SHIFT                  14
#define AR9_RA_0B_08_REG_RA21_SPAN_SIZE                   1
/* Bit: 'RA21_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_0B_08_REG_RA21_MG_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA21_MG_SHIFT                    13
#define AR9_RA_0B_08_REG_RA21_MG_SIZE                     1
/* Bit: 'RA21_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_0B_08_REG_RA21_CV_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA21_CV_SHIFT                    12
#define AR9_RA_0B_08_REG_RA21_CV_SIZE                     1
/* Bit: 'RA21_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_0B_08_REG_RA21_TXTAG_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA21_TXTAG_SHIFT                 10
#define AR9_RA_0B_08_REG_RA21_TXTAG_SIZE                  2
/* Bit: 'RA21_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_0B_08_REG_RA21_ACT_OFFSET                  0x0188
#define AR9_RA_0B_08_REG_RA21_ACT_SHIFT                   8
#define AR9_RA_0B_08_REG_RA21_ACT_SIZE                    2
/* Bit: 'RA20_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_0B_08_REG_RA20_VALID_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA20_VALID_SHIFT                 7
#define AR9_RA_0B_08_REG_RA20_VALID_SIZE                  1
/* Bit: 'RA20_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_0B_08_REG_RA20_SPAN_OFFSET                 0x0188
#define AR9_RA_0B_08_REG_RA20_SPAN_SHIFT                  6
#define AR9_RA_0B_08_REG_RA20_SPAN_SIZE                   1
/* Bit: 'RA20_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_0B_08_REG_RA20_MG_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA20_MG_SHIFT                    5
#define AR9_RA_0B_08_REG_RA20_MG_SIZE                     1
/* Bit: 'RA20_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_0B_08_REG_RA20_CV_OFFSET                   0x0188
#define AR9_RA_0B_08_REG_RA20_CV_SHIFT                    4
#define AR9_RA_0B_08_REG_RA20_CV_SIZE                     1
/* Bit: 'RA20_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_0B_08_REG_RA20_TXTAG_OFFSET                0x0188
#define AR9_RA_0B_08_REG_RA20_TXTAG_SHIFT                 2
#define AR9_RA_0B_08_REG_RA20_TXTAG_SIZE                  2
/* Bit: 'RA20_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_0B_08_REG_RA20_ACT_OFFSET                  0x0188
#define AR9_RA_0B_08_REG_RA20_ACT_SHIFT                   0
#define AR9_RA_0B_08_REG_RA20_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200000F~0180C200000C' */
/* Bit: 'RA33_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_0F_0C_REG_RA33_VALID_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA33_VALID_SHIFT                 31
#define AR9_RA_0F_0C_REG_RA33_VALID_SIZE                  1
/* Bit: 'RA33_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_0F_0C_REG_RA33_SPAN_OFFSET                 0x018C
#define AR9_RA_0F_0C_REG_RA33_SPAN_SHIFT                  30
#define AR9_RA_0F_0C_REG_RA33_SPAN_SIZE                   1
/* Bit: 'RA33_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_0F_0C_REG_RA33_MG_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA33_MG_SHIFT                    29
#define AR9_RA_0F_0C_REG_RA33_MG_SIZE                     1
/* Bit: 'RA33_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_0F_0C_REG_RA33_CV_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA33_CV_SHIFT                    28
#define AR9_RA_0F_0C_REG_RA33_CV_SIZE                     1
/* Bit: 'RA33_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_0F_0C_REG_RA33_TXTAG_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA33_TXTAG_SHIFT                 26
#define AR9_RA_0F_0C_REG_RA33_TXTAG_SIZE                  2
/* Bit: 'RA33_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_0F_0C_REG_RA33_ACT_OFFSET                  0x018C
#define AR9_RA_0F_0C_REG_RA33_ACT_SHIFT                   24
#define AR9_RA_0F_0C_REG_RA33_ACT_SIZE                    2
/* Bit: 'RA32_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_0F_0C_REG_RA32_VALID_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA32_VALID_SHIFT                 23
#define AR9_RA_0F_0C_REG_RA32_VALID_SIZE                  1
/* Bit: 'RA32_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_0F_0C_REG_RA32_SPAN_OFFSET                 0x018C
#define AR9_RA_0F_0C_REG_RA32_SPAN_SHIFT                  22
#define AR9_RA_0F_0C_REG_RA32_SPAN_SIZE                   1
/* Bit: 'RA32_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_0F_0C_REG_RA32_MG_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA32_MG_SHIFT                    21
#define AR9_RA_0F_0C_REG_RA32_MG_SIZE                     1
/* Bit: 'RA32_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_0F_0C_REG_RA32_CV_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA32_CV_SHIFT                    20
#define AR9_RA_0F_0C_REG_RA32_CV_SIZE                     1
/* Bit: 'RA32_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_0F_0C_REG_RA32_TXTAG_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA32_TXTAG_SHIFT                 18
#define AR9_RA_0F_0C_REG_RA32_TXTAG_SIZE                  2
/* Bit: 'RA32_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_0F_0C_REG_RA32_ACT_OFFSET                  0x018C
#define AR9_RA_0F_0C_REG_RA32_ACT_SHIFT                   16
#define AR9_RA_0F_0C_REG_RA32_ACT_SIZE                    2
/* Bit: 'RA31_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_0F_0C_REG_RA31_VALID_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA31_VALID_SHIFT                 15
#define AR9_RA_0F_0C_REG_RA31_VALID_SIZE                  1
/* Bit: 'RA31_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_0F_0C_REG_RA31_SPAN_OFFSET                 0x018C
#define AR9_RA_0F_0C_REG_RA31_SPAN_SHIFT                  14
#define AR9_RA_0F_0C_REG_RA31_SPAN_SIZE                   1
/* Bit: 'RA31_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_0F_0C_REG_RA31_MG_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA31_MG_SHIFT                    13
#define AR9_RA_0F_0C_REG_RA31_MG_SIZE                     1
/* Bit: 'RA31_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_0F_0C_REG_RA31_CV_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA31_CV_SHIFT                    12
#define AR9_RA_0F_0C_REG_RA31_CV_SIZE                     1
/* Bit: 'RA31_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_0F_0C_REG_RA31_TXTAG_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA31_TXTAG_SHIFT                 10
#define AR9_RA_0F_0C_REG_RA31_TXTAG_SIZE                  2
/* Bit: 'RA31_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_0F_0C_REG_RA31_ACT_OFFSET                  0x018C
#define AR9_RA_0F_0C_REG_RA31_ACT_SHIFT                   8
#define AR9_RA_0F_0C_REG_RA31_ACT_SIZE                    2
/* Bit: 'RA30_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_0F_0C_REG_RA30_VALID_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA30_VALID_SHIFT                 7
#define AR9_RA_0F_0C_REG_RA30_VALID_SIZE                  1
/* Bit: 'RA30_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_0F_0C_REG_RA30_SPAN_OFFSET                 0x018C
#define AR9_RA_0F_0C_REG_RA30_SPAN_SHIFT                  6
#define AR9_RA_0F_0C_REG_RA30_SPAN_SIZE                   1
/* Bit: 'RA30_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_0F_0C_REG_RA30_MG_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA30_MG_SHIFT                    5
#define AR9_RA_0F_0C_REG_RA30_MG_SIZE                     1
/* Bit: 'RA30_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_0F_0C_REG_RA30_CV_OFFSET                   0x018C
#define AR9_RA_0F_0C_REG_RA30_CV_SHIFT                    4
#define AR9_RA_0F_0C_REG_RA30_CV_SIZE                     1
/* Bit: 'RA30_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_0F_0C_REG_RA30_TXTAG_OFFSET                0x018C
#define AR9_RA_0F_0C_REG_RA30_TXTAG_SHIFT                 2
#define AR9_RA_0F_0C_REG_RA30_TXTAG_SIZE                  2
/* Bit: 'RA30_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_0F_0C_REG_RA30_ACT_OFFSET                  0x018C
#define AR9_RA_0F_0C_REG_RA30_ACT_SHIFT                   0
#define AR9_RA_0F_0C_REG_RA30_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000013~0180C2000010' */
/* Bit: 'RA43_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_13_10_REG_RA43_VALID_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA43_VALID_SHIFT                 31
#define AR9_RA_13_10_REG_RA43_VALID_SIZE                  1
/* Bit: 'RA43_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_13_10_REG_RA43_SPAN_OFFSET                 0x0190
#define AR9_RA_13_10_REG_RA43_SPAN_SHIFT                  30
#define AR9_RA_13_10_REG_RA43_SPAN_SIZE                   1
/* Bit: 'RA43_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_13_10_REG_RA43_MG_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA43_MG_SHIFT                    29
#define AR9_RA_13_10_REG_RA43_MG_SIZE                     1
/* Bit: 'RA43_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_13_10_REG_RA43_CV_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA43_CV_SHIFT                    28
#define AR9_RA_13_10_REG_RA43_CV_SIZE                     1
/* Bit: 'RA43_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_13_10_REG_RA43_TXTAG_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA43_TXTAG_SHIFT                 26
#define AR9_RA_13_10_REG_RA43_TXTAG_SIZE                  2
/* Bit: 'RA43_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_13_10_REG_RA43_ACT_OFFSET                  0x0190
#define AR9_RA_13_10_REG_RA43_ACT_SHIFT                   24
#define AR9_RA_13_10_REG_RA43_ACT_SIZE                    2
/* Bit: 'RA42_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_13_10_REG_RA42_VALID_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA42_VALID_SHIFT                 23
#define AR9_RA_13_10_REG_RA42_VALID_SIZE                  1
/* Bit: 'RA42_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_13_10_REG_RA42_SPAN_OFFSET                 0x0190
#define AR9_RA_13_10_REG_RA42_SPAN_SHIFT                  22
#define AR9_RA_13_10_REG_RA42_SPAN_SIZE                   1
/* Bit: 'RA42_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_13_10_REG_RA42_MG_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA42_MG_SHIFT                    21
#define AR9_RA_13_10_REG_RA42_MG_SIZE                     1
/* Bit: 'RA42_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_13_10_REG_RA42_CV_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA42_CV_SHIFT                    20
#define AR9_RA_13_10_REG_RA42_CV_SIZE                     1
/* Bit: 'RA42_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_13_10_REG_RA42_TXTAG_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA42_TXTAG_SHIFT                 18
#define AR9_RA_13_10_REG_RA42_TXTAG_SIZE                  2
/* Bit: 'RA42_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_13_10_REG_RA42_ACT_OFFSET                  0x0190
#define AR9_RA_13_10_REG_RA42_ACT_SHIFT                   16
#define AR9_RA_13_10_REG_RA42_ACT_SIZE                    2
/* Bit: 'RA41_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_13_10_REG_RA41_VALID_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA41_VALID_SHIFT                 15
#define AR9_RA_13_10_REG_RA41_VALID_SIZE                  1
/* Bit: 'RA41_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_13_10_REG_RA41_SPAN_OFFSET                 0x0190
#define AR9_RA_13_10_REG_RA41_SPAN_SHIFT                  14
#define AR9_RA_13_10_REG_RA41_SPAN_SIZE                   1
/* Bit: 'RA41_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_13_10_REG_RA41_MG_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA41_MG_SHIFT                    13
#define AR9_RA_13_10_REG_RA41_MG_SIZE                     1
/* Bit: 'RA41_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_13_10_REG_RA41_CV_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA41_CV_SHIFT                    12
#define AR9_RA_13_10_REG_RA41_CV_SIZE                     1
/* Bit: 'RA41_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_13_10_REG_RA41_TXTAG_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA41_TXTAG_SHIFT                 10
#define AR9_RA_13_10_REG_RA41_TXTAG_SIZE                  2
/* Bit: 'RA41_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_13_10_REG_RA41_ACT_OFFSET                  0x0190
#define AR9_RA_13_10_REG_RA41_ACT_SHIFT                   8
#define AR9_RA_13_10_REG_RA41_ACT_SIZE                    2
/* Bit: 'RA40_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_13_10_REG_RA40_VALID_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA40_VALID_SHIFT                 7
#define AR9_RA_13_10_REG_RA40_VALID_SIZE                  1
/* Bit: 'RA40_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_13_10_REG_RA40_SPAN_OFFSET                 0x0190
#define AR9_RA_13_10_REG_RA40_SPAN_SHIFT                  6
#define AR9_RA_13_10_REG_RA40_SPAN_SIZE                   1
/* Bit: 'RA40_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_13_10_REG_RA40_MG_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA40_MG_SHIFT                    5
#define AR9_RA_13_10_REG_RA40_MG_SIZE                     1
/* Bit: 'RA40_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_13_10_REG_RA40_CV_OFFSET                   0x0190
#define AR9_RA_13_10_REG_RA40_CV_SHIFT                    4
#define AR9_RA_13_10_REG_RA40_CV_SIZE                     1
/* Bit: 'RA40_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_13_10_REG_RA40_TXTAG_OFFSET                0x0190
#define AR9_RA_13_10_REG_RA40_TXTAG_SHIFT                 2
#define AR9_RA_13_10_REG_RA40_TXTAG_SIZE                  2
/* Bit: 'RA40_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_13_10_REG_RA40_ACT_OFFSET                  0x0190
#define AR9_RA_13_10_REG_RA40_ACT_SHIFT                   0
#define AR9_RA_13_10_REG_RA40_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000017~0180C2000014' */
/* Bit: 'RA53_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_17_14_REG_RA53_VALID_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA53_VALID_SHIFT                 31
#define AR9_RA_17_14_REG_RA53_VALID_SIZE                  1
/* Bit: 'RA53_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_17_14_REG_RA53_SPAN_OFFSET                 0x0194
#define AR9_RA_17_14_REG_RA53_SPAN_SHIFT                  30
#define AR9_RA_17_14_REG_RA53_SPAN_SIZE                   1
/* Bit: 'RA53_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_17_14_REG_RA53_MG_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA53_MG_SHIFT                    29
#define AR9_RA_17_14_REG_RA53_MG_SIZE                     1
/* Bit: 'RA53_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_17_14_REG_RA53_CV_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA53_CV_SHIFT                    28
#define AR9_RA_17_14_REG_RA53_CV_SIZE                     1
/* Bit: 'RA53_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_17_14_REG_RA53_TXTAG_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA53_TXTAG_SHIFT                 26
#define AR9_RA_17_14_REG_RA53_TXTAG_SIZE                  2
/* Bit: 'RA53_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_17_14_REG_RA53_ACT_OFFSET                  0x0194
#define AR9_RA_17_14_REG_RA53_ACT_SHIFT                   24
#define AR9_RA_17_14_REG_RA53_ACT_SIZE                    2
/* Bit: 'RA52_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_17_14_REG_RA52_VALID_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA52_VALID_SHIFT                 23
#define AR9_RA_17_14_REG_RA52_VALID_SIZE                  1
/* Bit: 'RA52_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_17_14_REG_RA52_SPAN_OFFSET                 0x0194
#define AR9_RA_17_14_REG_RA52_SPAN_SHIFT                  22
#define AR9_RA_17_14_REG_RA52_SPAN_SIZE                   1
/* Bit: 'RA52_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_17_14_REG_RA52_MG_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA52_MG_SHIFT                    21
#define AR9_RA_17_14_REG_RA52_MG_SIZE                     1
/* Bit: 'RA52_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_17_14_REG_RA52_CV_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA52_CV_SHIFT                    20
#define AR9_RA_17_14_REG_RA52_CV_SIZE                     1
/* Bit: 'RA52_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_17_14_REG_RA52_TXTAG_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA52_TXTAG_SHIFT                 18
#define AR9_RA_17_14_REG_RA52_TXTAG_SIZE                  2
/* Bit: 'RA52_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_17_14_REG_RA52_ACT_OFFSET                  0x0194
#define AR9_RA_17_14_REG_RA52_ACT_SHIFT                   16
#define AR9_RA_17_14_REG_RA52_ACT_SIZE                    2
/* Bit: 'RA51_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_17_14_REG_RA51_VALID_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA51_VALID_SHIFT                 15
#define AR9_RA_17_14_REG_RA51_VALID_SIZE                  1
/* Bit: 'RA51_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_17_14_REG_RA51_SPAN_OFFSET                 0x0194
#define AR9_RA_17_14_REG_RA51_SPAN_SHIFT                  14
#define AR9_RA_17_14_REG_RA51_SPAN_SIZE                   1
/* Bit: 'RA51_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_17_14_REG_RA51_MG_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA51_MG_SHIFT                    13
#define AR9_RA_17_14_REG_RA51_MG_SIZE                     1
/* Bit: 'RA51_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_17_14_REG_RA51_CV_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA51_CV_SHIFT                    12
#define AR9_RA_17_14_REG_RA51_CV_SIZE                     1
/* Bit: 'RA51_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_17_14_REG_RA51_TXTAG_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA51_TXTAG_SHIFT                 10
#define AR9_RA_17_14_REG_RA51_TXTAG_SIZE                  2
/* Bit: 'RA51_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_17_14_REG_RA51_ACT_OFFSET                  0x0194
#define AR9_RA_17_14_REG_RA51_ACT_SHIFT                   8
#define AR9_RA_17_14_REG_RA51_ACT_SIZE                    2
/* Bit: 'RA50_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_17_14_REG_RA50_VALID_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA50_VALID_SHIFT                 7
#define AR9_RA_17_14_REG_RA50_VALID_SIZE                  1
/* Bit: 'RA50_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_17_14_REG_RA50_SPAN_OFFSET                 0x0194
#define AR9_RA_17_14_REG_RA50_SPAN_SHIFT                  6
#define AR9_RA_17_14_REG_RA50_SPAN_SIZE                   1
/* Bit: 'RA50_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_17_14_REG_RA50_MG_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA50_MG_SHIFT                    5
#define AR9_RA_17_14_REG_RA50_MG_SIZE                     1
/* Bit: 'RA50_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_17_14_REG_RA50_CV_OFFSET                   0x0194
#define AR9_RA_17_14_REG_RA50_CV_SHIFT                    4
#define AR9_RA_17_14_REG_RA50_CV_SIZE                     1
/* Bit: 'RA50_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_17_14_REG_RA50_TXTAG_OFFSET                0x0194
#define AR9_RA_17_14_REG_RA50_TXTAG_SHIFT                 2
#define AR9_RA_17_14_REG_RA50_TXTAG_SIZE                  2
/* Bit: 'RA50_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_17_14_REG_RA50_ACT_OFFSET                  0x0194
#define AR9_RA_17_14_REG_RA50_ACT_SHIFT                   0
#define AR9_RA_17_14_REG_RA50_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200001B~0180C2000018' */
/* Bit: 'RA63_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_1B_18_REG_RA63_VALID_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA63_VALID_SHIFT                 31
#define AR9_RA_1B_18_REG_RA63_VALID_SIZE                  1
/* Bit: 'RA63_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_1B_18_REG_RA63_SPAN_OFFSET                 0x0198
#define AR9_RA_1B_18_REG_RA63_SPAN_SHIFT                  30
#define AR9_RA_1B_18_REG_RA63_SPAN_SIZE                   1
/* Bit: 'RA63_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_1B_18_REG_RA63_MG_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA63_MG_SHIFT                    29
#define AR9_RA_1B_18_REG_RA63_MG_SIZE                     1
/* Bit: 'RA63_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_1B_18_REG_RA63_CV_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA63_CV_SHIFT                    28
#define AR9_RA_1B_18_REG_RA63_CV_SIZE                     1
/* Bit: 'RA63_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_1B_18_REG_RA63_TXTAG_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA63_TXTAG_SHIFT                 26
#define AR9_RA_1B_18_REG_RA63_TXTAG_SIZE                  2
/* Bit: 'RA63_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_1B_18_REG_RA63_ACT_OFFSET                  0x0198
#define AR9_RA_1B_18_REG_RA63_ACT_SHIFT                   24
#define AR9_RA_1B_18_REG_RA63_ACT_SIZE                    2
/* Bit: 'RA62_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_1B_18_REG_RA62_VALID_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA62_VALID_SHIFT                 23
#define AR9_RA_1B_18_REG_RA62_VALID_SIZE                  1
/* Bit: 'RA62_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_1B_18_REG_RA62_SPAN_OFFSET                 0x0198
#define AR9_RA_1B_18_REG_RA62_SPAN_SHIFT                  22
#define AR9_RA_1B_18_REG_RA62_SPAN_SIZE                   1
/* Bit: 'RA62_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_1B_18_REG_RA62_MG_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA62_MG_SHIFT                    21
#define AR9_RA_1B_18_REG_RA62_MG_SIZE                     1
/* Bit: 'RA62_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_1B_18_REG_RA62_CV_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA62_CV_SHIFT                    20
#define AR9_RA_1B_18_REG_RA62_CV_SIZE                     1
/* Bit: 'RA62_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_1B_18_REG_RA62_TXTAG_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA62_TXTAG_SHIFT                 18
#define AR9_RA_1B_18_REG_RA62_TXTAG_SIZE                  2
/* Bit: 'RA62_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_1B_18_REG_RA62_ACT_OFFSET                  0x0198
#define AR9_RA_1B_18_REG_RA62_ACT_SHIFT                   16
#define AR9_RA_1B_18_REG_RA62_ACT_SIZE                    2
/* Bit: 'RA61_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_1B_18_REG_RA61_VALID_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA61_VALID_SHIFT                 15
#define AR9_RA_1B_18_REG_RA61_VALID_SIZE                  1
/* Bit: 'RA61_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_1B_18_REG_RA61_SPAN_OFFSET                 0x0198
#define AR9_RA_1B_18_REG_RA61_SPAN_SHIFT                  14
#define AR9_RA_1B_18_REG_RA61_SPAN_SIZE                   1
/* Bit: 'RA61_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_1B_18_REG_RA61_MG_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA61_MG_SHIFT                    13
#define AR9_RA_1B_18_REG_RA61_MG_SIZE                     1
/* Bit: 'RA61_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_1B_18_REG_RA61_CV_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA61_CV_SHIFT                    12
#define AR9_RA_1B_18_REG_RA61_CV_SIZE                     1
/* Bit: 'RA61_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_1B_18_REG_RA61_TXTAG_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA61_TXTAG_SHIFT                 10
#define AR9_RA_1B_18_REG_RA61_TXTAG_SIZE                  2
/* Bit: 'RA61_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_1B_18_REG_RA61_ACT_OFFSET                  0x0198
#define AR9_RA_1B_18_REG_RA61_ACT_SHIFT                   8
#define AR9_RA_1B_18_REG_RA61_ACT_SIZE                    2
/* Bit: 'RA60_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_1B_18_REG_RA60_VALID_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA60_VALID_SHIFT                 7
#define AR9_RA_1B_18_REG_RA60_VALID_SIZE                  1
/* Bit: 'RA60_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_1B_18_REG_RA60_SPAN_OFFSET                 0x0198
#define AR9_RA_1B_18_REG_RA60_SPAN_SHIFT                  6
#define AR9_RA_1B_18_REG_RA60_SPAN_SIZE                   1
/* Bit: 'RA60_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_1B_18_REG_RA60_MG_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA60_MG_SHIFT                    5
#define AR9_RA_1B_18_REG_RA60_MG_SIZE                     1
/* Bit: 'RA60_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_1B_18_REG_RA60_CV_OFFSET                   0x0198
#define AR9_RA_1B_18_REG_RA60_CV_SHIFT                    4
#define AR9_RA_1B_18_REG_RA60_CV_SIZE                     1
/* Bit: 'RA60_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_1B_18_REG_RA60_TXTAG_OFFSET                0x0198
#define AR9_RA_1B_18_REG_RA60_TXTAG_SHIFT                 2
#define AR9_RA_1B_18_REG_RA60_TXTAG_SIZE                  2
/* Bit: 'RA60_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_1B_18_REG_RA60_ACT_OFFSET                  0x0198
#define AR9_RA_1B_18_REG_RA60_ACT_SHIFT                   0
#define AR9_RA_1B_18_REG_RA60_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200001F~0180C200001C' */
/* Bit: 'RA73_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_1F_1C_REG_RA73_VALID_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA73_VALID_SHIFT                 31
#define AR9_RA_1F_1C_REG_RA73_VALID_SIZE                  1
/* Bit: 'RA73_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_1F_1C_REG_RA73_SPAN_OFFSET                 0x019C
#define AR9_RA_1F_1C_REG_RA73_SPAN_SHIFT                  30
#define AR9_RA_1F_1C_REG_RA73_SPAN_SIZE                   1
/* Bit: 'RA73_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_1F_1C_REG_RA73_MG_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA73_MG_SHIFT                    29
#define AR9_RA_1F_1C_REG_RA73_MG_SIZE                     1
/* Bit: 'RA73_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_1F_1C_REG_RA73_CV_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA73_CV_SHIFT                    28
#define AR9_RA_1F_1C_REG_RA73_CV_SIZE                     1
/* Bit: 'RA73_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_1F_1C_REG_RA73_TXTAG_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA73_TXTAG_SHIFT                 26
#define AR9_RA_1F_1C_REG_RA73_TXTAG_SIZE                  2
/* Bit: 'RA73_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_1F_1C_REG_RA73_ACT_OFFSET                  0x019C
#define AR9_RA_1F_1C_REG_RA73_ACT_SHIFT                   24
#define AR9_RA_1F_1C_REG_RA73_ACT_SIZE                    2
/* Bit: 'RA72_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_1F_1C_REG_RA72_VALID_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA72_VALID_SHIFT                 23
#define AR9_RA_1F_1C_REG_RA72_VALID_SIZE                  1
/* Bit: 'RA72_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_1F_1C_REG_RA72_SPAN_OFFSET                 0x019C
#define AR9_RA_1F_1C_REG_RA72_SPAN_SHIFT                  22
#define AR9_RA_1F_1C_REG_RA72_SPAN_SIZE                   1
/* Bit: 'RA72_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_1F_1C_REG_RA72_MG_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA72_MG_SHIFT                    21
#define AR9_RA_1F_1C_REG_RA72_MG_SIZE                     1
/* Bit: 'RA72_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_1F_1C_REG_RA72_CV_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA72_CV_SHIFT                    20
#define AR9_RA_1F_1C_REG_RA72_CV_SIZE                     1
/* Bit: 'RA72_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_1F_1C_REG_RA72_TXTAG_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA72_TXTAG_SHIFT                 18
#define AR9_RA_1F_1C_REG_RA72_TXTAG_SIZE                  2
/* Bit: 'RA72_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_1F_1C_REG_RA72_ACT_OFFSET                  0x019C
#define AR9_RA_1F_1C_REG_RA72_ACT_SHIFT                   16
#define AR9_RA_1F_1C_REG_RA72_ACT_SIZE                    2
/* Bit: 'RA71_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_1F_1C_REG_RA71_VALID_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA71_VALID_SHIFT                 15
#define AR9_RA_1F_1C_REG_RA71_VALID_SIZE                  1
/* Bit: 'RA71_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_1F_1C_REG_RA71_SPAN_OFFSET                 0x019C
#define AR9_RA_1F_1C_REG_RA71_SPAN_SHIFT                  14
#define AR9_RA_1F_1C_REG_RA71_SPAN_SIZE                   1
/* Bit: 'RA71_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_1F_1C_REG_RA71_MG_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA71_MG_SHIFT                    13
#define AR9_RA_1F_1C_REG_RA71_MG_SIZE                     1
/* Bit: 'RA71_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_1F_1C_REG_RA71_CV_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA71_CV_SHIFT                    12
#define AR9_RA_1F_1C_REG_RA71_CV_SIZE                     1
/* Bit: 'RA71_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_1F_1C_REG_RA71_TXTAG_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA71_TXTAG_SHIFT                 10
#define AR9_RA_1F_1C_REG_RA71_TXTAG_SIZE                  2
/* Bit: 'RA71_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_1F_1C_REG_RA71_ACT_OFFSET                  0x019C
#define AR9_RA_1F_1C_REG_RA71_ACT_SHIFT                   8
#define AR9_RA_1F_1C_REG_RA71_ACT_SIZE                    2
/* Bit: 'RA70_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_1F_1C_REG_RA70_VALID_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA70_VALID_SHIFT                 7
#define AR9_RA_1F_1C_REG_RA70_VALID_SIZE                  1
/* Bit: 'RA70_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_1F_1C_REG_RA70_SPAN_OFFSET                 0x019C
#define AR9_RA_1F_1C_REG_RA70_SPAN_SHIFT                  6
#define AR9_RA_1F_1C_REG_RA70_SPAN_SIZE                   1
/* Bit: 'RA70_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_1F_1C_REG_RA70_MG_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA70_MG_SHIFT                    5
#define AR9_RA_1F_1C_REG_RA70_MG_SIZE                     1
/* Bit: 'RA70_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_1F_1C_REG_RA70_CV_OFFSET                   0x019C
#define AR9_RA_1F_1C_REG_RA70_CV_SHIFT                    4
#define AR9_RA_1F_1C_REG_RA70_CV_SIZE                     1
/* Bit: 'RA70_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_1F_1C_REG_RA70_TXTAG_OFFSET                0x019C
#define AR9_RA_1F_1C_REG_RA70_TXTAG_SHIFT                 2
#define AR9_RA_1F_1C_REG_RA70_TXTAG_SIZE                  2
/* Bit: 'RA70_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_1F_1C_REG_RA70_ACT_OFFSET                  0x019C
#define AR9_RA_1F_1C_REG_RA70_ACT_SHIFT                   0
#define AR9_RA_1F_1C_REG_RA70_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000023~0180C2000020' */
/* Bit: 'RA83_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_23_20_REG_RA83_VALID_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA83_VALID_SHIFT                 31
#define AR9_RA_23_20_REG_RA83_VALID_SIZE                  1
/* Bit: 'RA83_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_23_20_REG_RA83_SPAN_OFFSET                 0x01A0
#define AR9_RA_23_20_REG_RA83_SPAN_SHIFT                  30
#define AR9_RA_23_20_REG_RA83_SPAN_SIZE                   1
/* Bit: 'RA83_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_23_20_REG_RA83_MG_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA83_MG_SHIFT                    29
#define AR9_RA_23_20_REG_RA83_MG_SIZE                     1
/* Bit: 'RA83_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_23_20_REG_RA83_CV_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA83_CV_SHIFT                    28
#define AR9_RA_23_20_REG_RA83_CV_SIZE                     1
/* Bit: 'RA83_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_23_20_REG_RA83_TXTAG_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA83_TXTAG_SHIFT                 26
#define AR9_RA_23_20_REG_RA83_TXTAG_SIZE                  2
/* Bit: 'RA83_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_23_20_REG_RA83_ACT_OFFSET                  0x01A0
#define AR9_RA_23_20_REG_RA83_ACT_SHIFT                   24
#define AR9_RA_23_20_REG_RA83_ACT_SIZE                    2
/* Bit: 'RA82_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_23_20_REG_RA82_VALID_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA82_VALID_SHIFT                 23
#define AR9_RA_23_20_REG_RA82_VALID_SIZE                  1
/* Bit: 'RA82_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_23_20_REG_RA82_SPAN_OFFSET                 0x01A0
#define AR9_RA_23_20_REG_RA82_SPAN_SHIFT                  22
#define AR9_RA_23_20_REG_RA82_SPAN_SIZE                   1
/* Bit: 'RA82_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_23_20_REG_RA82_MG_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA82_MG_SHIFT                    21
#define AR9_RA_23_20_REG_RA82_MG_SIZE                     1
/* Bit: 'RA82_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_23_20_REG_RA82_CV_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA82_CV_SHIFT                    20
#define AR9_RA_23_20_REG_RA82_CV_SIZE                     1
/* Bit: 'RA82_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_23_20_REG_RA82_TXTAG_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA82_TXTAG_SHIFT                 18
#define AR9_RA_23_20_REG_RA82_TXTAG_SIZE                  2
/* Bit: 'RA82_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_23_20_REG_RA82_ACT_OFFSET                  0x01A0
#define AR9_RA_23_20_REG_RA82_ACT_SHIFT                   16
#define AR9_RA_23_20_REG_RA82_ACT_SIZE                    2
/* Bit: 'RA81_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_23_20_REG_RA81_VALID_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA81_VALID_SHIFT                 15
#define AR9_RA_23_20_REG_RA81_VALID_SIZE                  1
/* Bit: 'RA81_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_23_20_REG_RA81_SPAN_OFFSET                 0x01A0
#define AR9_RA_23_20_REG_RA81_SPAN_SHIFT                  14
#define AR9_RA_23_20_REG_RA81_SPAN_SIZE                   1
/* Bit: 'RA81_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_23_20_REG_RA81_MG_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA81_MG_SHIFT                    13
#define AR9_RA_23_20_REG_RA81_MG_SIZE                     1
/* Bit: 'RA81_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_23_20_REG_RA81_CV_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA81_CV_SHIFT                    12
#define AR9_RA_23_20_REG_RA81_CV_SIZE                     1
/* Bit: 'RA81_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_23_20_REG_RA81_TXTAG_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA81_TXTAG_SHIFT                 10
#define AR9_RA_23_20_REG_RA81_TXTAG_SIZE                  2
/* Bit: 'RA81_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_23_20_REG_RA81_ACT_OFFSET                  0x01A0
#define AR9_RA_23_20_REG_RA81_ACT_SHIFT                   8
#define AR9_RA_23_20_REG_RA81_ACT_SIZE                    2
/* Bit: 'RA80_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_23_20_REG_RA80_VALID_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA80_VALID_SHIFT                 7
#define AR9_RA_23_20_REG_RA80_VALID_SIZE                  1
/* Bit: 'RA80_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_23_20_REG_RA80_SPAN_OFFSET                 0x01A0
#define AR9_RA_23_20_REG_RA80_SPAN_SHIFT                  6
#define AR9_RA_23_20_REG_RA80_SPAN_SIZE                   1
/* Bit: 'RA80_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_23_20_REG_RA80_MG_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA80_MG_SHIFT                    5
#define AR9_RA_23_20_REG_RA80_MG_SIZE                     1
/* Bit: 'RA80_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_23_20_REG_RA80_CV_OFFSET                   0x01A0
#define AR9_RA_23_20_REG_RA80_CV_SHIFT                    4
#define AR9_RA_23_20_REG_RA80_CV_SIZE                     1
/* Bit: 'RA80_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_23_20_REG_RA80_TXTAG_OFFSET                0x01A0
#define AR9_RA_23_20_REG_RA80_TXTAG_SHIFT                 2
#define AR9_RA_23_20_REG_RA80_TXTAG_SIZE                  2
/* Bit: 'RA80_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_23_20_REG_RA80_ACT_OFFSET                  0x01A0
#define AR9_RA_23_20_REG_RA80_ACT_SHIFT                   0
#define AR9_RA_23_20_REG_RA80_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000027~0180C2000024' */
/* Bit: 'RA93_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_27_24_REG_RA93_VALID_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA93_VALID_SHIFT                 31
#define AR9_RA_27_24_REG_RA93_VALID_SIZE                  1
/* Bit: 'RA93_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_27_24_REG_RA93_SPAN_OFFSET                 0x01A4
#define AR9_RA_27_24_REG_RA93_SPAN_SHIFT                  30
#define AR9_RA_27_24_REG_RA93_SPAN_SIZE                   1
/* Bit: 'RA93_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_27_24_REG_RA93_MG_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA93_MG_SHIFT                    29
#define AR9_RA_27_24_REG_RA93_MG_SIZE                     1
/* Bit: 'RA93_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_27_24_REG_RA93_CV_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA93_CV_SHIFT                    28
#define AR9_RA_27_24_REG_RA93_CV_SIZE                     1
/* Bit: 'RA93_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_27_24_REG_RA93_TXTAG_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA93_TXTAG_SHIFT                 26
#define AR9_RA_27_24_REG_RA93_TXTAG_SIZE                  2
/* Bit: 'RA93_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_27_24_REG_RA93_ACT_OFFSET                  0x01A4
#define AR9_RA_27_24_REG_RA93_ACT_SHIFT                   24
#define AR9_RA_27_24_REG_RA93_ACT_SIZE                    2
/* Bit: 'RA92_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_27_24_REG_RA92_VALID_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA92_VALID_SHIFT                 23
#define AR9_RA_27_24_REG_RA92_VALID_SIZE                  1
/* Bit: 'RA92_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_27_24_REG_RA92_SPAN_OFFSET                 0x01A4
#define AR9_RA_27_24_REG_RA92_SPAN_SHIFT                  22
#define AR9_RA_27_24_REG_RA92_SPAN_SIZE                   1
/* Bit: 'RA92_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_27_24_REG_RA92_MG_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA92_MG_SHIFT                    21
#define AR9_RA_27_24_REG_RA92_MG_SIZE                     1
/* Bit: 'RA92_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_27_24_REG_RA92_CV_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA92_CV_SHIFT                    20
#define AR9_RA_27_24_REG_RA92_CV_SIZE                     1
/* Bit: 'RA92_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_27_24_REG_RA92_TXTAG_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA92_TXTAG_SHIFT                 18
#define AR9_RA_27_24_REG_RA92_TXTAG_SIZE                  2
/* Bit: 'RA92_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_27_24_REG_RA92_ACT_OFFSET                  0x01A4
#define AR9_RA_27_24_REG_RA92_ACT_SHIFT                   16
#define AR9_RA_27_24_REG_RA92_ACT_SIZE                    2
/* Bit: 'RA91_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_27_24_REG_RA91_VALID_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA91_VALID_SHIFT                 15
#define AR9_RA_27_24_REG_RA91_VALID_SIZE                  1
/* Bit: 'RA91_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_27_24_REG_RA91_SPAN_OFFSET                 0x01A4
#define AR9_RA_27_24_REG_RA91_SPAN_SHIFT                  14
#define AR9_RA_27_24_REG_RA91_SPAN_SIZE                   1
/* Bit: 'RA91_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_27_24_REG_RA91_MG_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA91_MG_SHIFT                    13
#define AR9_RA_27_24_REG_RA91_MG_SIZE                     1
/* Bit: 'RA91_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_27_24_REG_RA91_CV_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA91_CV_SHIFT                    12
#define AR9_RA_27_24_REG_RA91_CV_SIZE                     1
/* Bit: 'RA91_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_27_24_REG_RA91_TXTAG_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA91_TXTAG_SHIFT                 10
#define AR9_RA_27_24_REG_RA91_TXTAG_SIZE                  2
/* Bit: 'RA91_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_27_24_REG_RA91_ACT_OFFSET                  0x01A4
#define AR9_RA_27_24_REG_RA91_ACT_SHIFT                   8
#define AR9_RA_27_24_REG_RA91_ACT_SIZE                    2
/* Bit: 'RA90_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_27_24_REG_RA90_VALID_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA90_VALID_SHIFT                 7
#define AR9_RA_27_24_REG_RA90_VALID_SIZE                  1
/* Bit: 'RA90_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_27_24_REG_RA90_SPAN_OFFSET                 0x01A4
#define AR9_RA_27_24_REG_RA90_SPAN_SHIFT                  6
#define AR9_RA_27_24_REG_RA90_SPAN_SIZE                   1
/* Bit: 'RA90_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_27_24_REG_RA90_MG_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA90_MG_SHIFT                    5
#define AR9_RA_27_24_REG_RA90_MG_SIZE                     1
/* Bit: 'RA90_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_27_24_REG_RA90_CV_OFFSET                   0x01A4
#define AR9_RA_27_24_REG_RA90_CV_SHIFT                    4
#define AR9_RA_27_24_REG_RA90_CV_SIZE                     1
/* Bit: 'RA90_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_27_24_REG_RA90_TXTAG_OFFSET                0x01A4
#define AR9_RA_27_24_REG_RA90_TXTAG_SHIFT                 2
#define AR9_RA_27_24_REG_RA90_TXTAG_SIZE                  2
/* Bit: 'RA90_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_27_24_REG_RA90_ACT_OFFSET                  0x01A4
#define AR9_RA_27_24_REG_RA90_ACT_SHIFT                   0
#define AR9_RA_27_24_REG_RA90_ACT_SIZE                    2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200002B~0180C2000028' */
/* Bit: 'RA103_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_2B_28_REG_RA103_VALID_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA103_VALID_SHIFT                31
#define AR9_RA_2B_28_REG_RA103_VALID_SIZE                 1
/* Bit: 'RA103_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_2B_28_REG_RA103_SPAN_OFFSET                0x01A8
#define AR9_RA_2B_28_REG_RA103_SPAN_SHIFT                 30
#define AR9_RA_2B_28_REG_RA103_SPAN_SIZE                  1
/* Bit: 'RA103_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_2B_28_REG_RA103_MG_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA103_MG_SHIFT                   29
#define AR9_RA_2B_28_REG_RA103_MG_SIZE                    1
/* Bit: 'RA103_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_2B_28_REG_RA103_CV_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA103_CV_SHIFT                   28
#define AR9_RA_2B_28_REG_RA103_CV_SIZE                    1
/* Bit: 'RA103_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_2B_28_REG_RA103_TXTAG_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA103_TXTAG_SHIFT                26
#define AR9_RA_2B_28_REG_RA103_TXTAG_SIZE                 2
/* Bit: 'RA103_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_2B_28_REG_RA103_ACT_OFFSET                 0x01A8
#define AR9_RA_2B_28_REG_RA103_ACT_SHIFT                  24
#define AR9_RA_2B_28_REG_RA103_ACT_SIZE                   2
/* Bit: 'RA102_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_2B_28_REG_RA102_VALID_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA102_VALID_SHIFT                23
#define AR9_RA_2B_28_REG_RA102_VALID_SIZE                 1
/* Bit: 'RA102_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_2B_28_REG_RA102_SPAN_OFFSET                0x01A8
#define AR9_RA_2B_28_REG_RA102_SPAN_SHIFT                 22
#define AR9_RA_2B_28_REG_RA102_SPAN_SIZE                  1
/* Bit: 'RA102_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_2B_28_REG_RA102_MG_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA102_MG_SHIFT                   21
#define AR9_RA_2B_28_REG_RA102_MG_SIZE                    1
/* Bit: 'RA102_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_2B_28_REG_RA102_CV_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA102_CV_SHIFT                   20
#define AR9_RA_2B_28_REG_RA102_CV_SIZE                    1
/* Bit: 'RA102_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_2B_28_REG_RA102_TXTAG_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA102_TXTAG_SHIFT                18
#define AR9_RA_2B_28_REG_RA102_TXTAG_SIZE                 2
/* Bit: 'RA102_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_2B_28_REG_RA102_ACT_OFFSET                 0x01A8
#define AR9_RA_2B_28_REG_RA102_ACT_SHIFT                  16
#define AR9_RA_2B_28_REG_RA102_ACT_SIZE                   2
/* Bit: 'RA101_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_2B_28_REG_RA101_VALID_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA101_VALID_SHIFT                15
#define AR9_RA_2B_28_REG_RA101_VALID_SIZE                 1
/* Bit: 'RA101_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_2B_28_REG_RA101_SPAN_OFFSET                0x01A8
#define AR9_RA_2B_28_REG_RA101_SPAN_SHIFT                 14
#define AR9_RA_2B_28_REG_RA101_SPAN_SIZE                  1
/* Bit: 'RA101_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_2B_28_REG_RA101_MG_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA101_MG_SHIFT                   13
#define AR9_RA_2B_28_REG_RA101_MG_SIZE                    1
/* Bit: 'RA101_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_2B_28_REG_RA101_CV_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA101_CV_SHIFT                   12
#define AR9_RA_2B_28_REG_RA101_CV_SIZE                    1
/* Bit: 'RA101_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_2B_28_REG_RA101_TXTAG_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA101_TXTAG_SHIFT                10
#define AR9_RA_2B_28_REG_RA101_TXTAG_SIZE                 2
/* Bit: 'RA101_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_2B_28_REG_RA101_ACT_OFFSET                 0x01A8
#define AR9_RA_2B_28_REG_RA101_ACT_SHIFT                  8
#define AR9_RA_2B_28_REG_RA101_ACT_SIZE                   2
/* Bit: 'RA100_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_2B_28_REG_RA100_VALID_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA100_VALID_SHIFT                7
#define AR9_RA_2B_28_REG_RA100_VALID_SIZE                 1
/* Bit: 'RA100_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_2B_28_REG_RA100_SPAN_OFFSET                0x01A8
#define AR9_RA_2B_28_REG_RA100_SPAN_SHIFT                 6
#define AR9_RA_2B_28_REG_RA100_SPAN_SIZE                  1
/* Bit: 'RA100_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_2B_28_REG_RA100_MG_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA100_MG_SHIFT                   5
#define AR9_RA_2B_28_REG_RA100_MG_SIZE                    1
/* Bit: 'RA100_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_2B_28_REG_RA100_CV_OFFSET                  0x01A8
#define AR9_RA_2B_28_REG_RA100_CV_SHIFT                   4
#define AR9_RA_2B_28_REG_RA100_CV_SIZE                    1
/* Bit: 'RA100_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_2B_28_REG_RA100_TXTAG_OFFSET               0x01A8
#define AR9_RA_2B_28_REG_RA100_TXTAG_SHIFT                2
#define AR9_RA_2B_28_REG_RA100_TXTAG_SIZE                 2
/* Bit: 'RA100_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_2B_28_REG_RA100_ACT_OFFSET                 0x01A8
#define AR9_RA_2B_28_REG_RA100_ACT_SHIFT                  0
#define AR9_RA_2B_28_REG_RA100_ACT_SIZE                   2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200002F~0180C200002C' */
/* Bit: 'RA113_VALID' */
/* Description: 'Valid bit for 0180C2000003' */
#define AR9_RA_2F_2C_REG_RA113_VALID_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA113_VALID_SHIFT                31
#define AR9_RA_2F_2C_REG_RA113_VALID_SIZE                 1
/* Bit: 'RA113_SPAN' */
/* Description: 'Span bit for 0180C2000003' */
#define AR9_RA_2F_2C_REG_RA113_SPAN_OFFSET                0x01AC
#define AR9_RA_2F_2C_REG_RA113_SPAN_SHIFT                 30
#define AR9_RA_2F_2C_REG_RA113_SPAN_SIZE                  1
/* Bit: 'RA113_MG' */
/* Description: 'Management bit for 0180C2000003' */
#define AR9_RA_2F_2C_REG_RA113_MG_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA113_MG_SHIFT                   29
#define AR9_RA_2F_2C_REG_RA113_MG_SIZE                    1
/* Bit: 'RA113_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000003' */
#define AR9_RA_2F_2C_REG_RA113_CV_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA113_CV_SHIFT                   28
#define AR9_RA_2F_2C_REG_RA113_CV_SIZE                    1
/* Bit: 'RA113_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000003' */
#define AR9_RA_2F_2C_REG_RA113_TXTAG_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA113_TXTAG_SHIFT                26
#define AR9_RA_2F_2C_REG_RA113_TXTAG_SIZE                 2
/* Bit: 'RA113_ACT' */
/* Description: 'Action bit for 0180C2000003' */
#define AR9_RA_2F_2C_REG_RA113_ACT_OFFSET                 0x01AC
#define AR9_RA_2F_2C_REG_RA113_ACT_SHIFT                  24
#define AR9_RA_2F_2C_REG_RA113_ACT_SIZE                   2
/* Bit: 'RA112_VALID' */
/* Description: 'Valid bit for 0180C2000002' */
#define AR9_RA_2F_2C_REG_RA112_VALID_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA112_VALID_SHIFT                23
#define AR9_RA_2F_2C_REG_RA112_VALID_SIZE                 1
/* Bit: 'RA112_SPAN' */
/* Description: 'Span bit for 0180C2000002' */
#define AR9_RA_2F_2C_REG_RA112_SPAN_OFFSET                0x01AC
#define AR9_RA_2F_2C_REG_RA112_SPAN_SHIFT                 22
#define AR9_RA_2F_2C_REG_RA112_SPAN_SIZE                  1
/* Bit: 'RA112_MG' */
/* Description: 'Management bit for 0180C2000002' */
#define AR9_RA_2F_2C_REG_RA112_MG_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA112_MG_SHIFT                   21
#define AR9_RA_2F_2C_REG_RA112_MG_SIZE                    1
/* Bit: 'RA112_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000002' */
#define AR9_RA_2F_2C_REG_RA112_CV_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA112_CV_SHIFT                   20
#define AR9_RA_2F_2C_REG_RA112_CV_SIZE                    1
/* Bit: 'RA112_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000002' */
#define AR9_RA_2F_2C_REG_RA112_TXTAG_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA112_TXTAG_SHIFT                18
#define AR9_RA_2F_2C_REG_RA112_TXTAG_SIZE                 2
/* Bit: 'RA112_ACT' */
/* Description: 'Action bit for 0180C2000002' */
#define AR9_RA_2F_2C_REG_RA112_ACT_OFFSET                 0x01AC
#define AR9_RA_2F_2C_REG_RA112_ACT_SHIFT                  16
#define AR9_RA_2F_2C_REG_RA112_ACT_SIZE                   2
/* Bit: 'RA111_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define AR9_RA_2F_2C_REG_RA111_VALID_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA111_VALID_SHIFT                15
#define AR9_RA_2F_2C_REG_RA111_VALID_SIZE                 1
/* Bit: 'RA111_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define AR9_RA_2F_2C_REG_RA111_SPAN_OFFSET                0x01AC
#define AR9_RA_2F_2C_REG_RA111_SPAN_SHIFT                 14
#define AR9_RA_2F_2C_REG_RA111_SPAN_SIZE                  1
/* Bit: 'RA111_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define AR9_RA_2F_2C_REG_RA111_MG_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA111_MG_SHIFT                   13
#define AR9_RA_2F_2C_REG_RA111_MG_SIZE                    1
/* Bit: 'RA111_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define AR9_RA_2F_2C_REG_RA111_CV_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA111_CV_SHIFT                   12
#define AR9_RA_2F_2C_REG_RA111_CV_SIZE                    1
/* Bit: 'RA111_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define AR9_RA_2F_2C_REG_RA111_TXTAG_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA111_TXTAG_SHIFT                10
#define AR9_RA_2F_2C_REG_RA111_TXTAG_SIZE                 2
/* Bit: 'RA111_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define AR9_RA_2F_2C_REG_RA111_ACT_OFFSET                 0x01AC
#define AR9_RA_2F_2C_REG_RA111_ACT_SHIFT                  8
#define AR9_RA_2F_2C_REG_RA111_ACT_SIZE                   2
/* Bit: 'RA110_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define AR9_RA_2F_2C_REG_RA110_VALID_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA110_VALID_SHIFT                7
#define AR9_RA_2F_2C_REG_RA110_VALID_SIZE                 1
/* Bit: 'RA110_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define AR9_RA_2F_2C_REG_RA110_SPAN_OFFSET                0x01AC
#define AR9_RA_2F_2C_REG_RA110_SPAN_SHIFT                 6
#define AR9_RA_2F_2C_REG_RA110_SPAN_SIZE                  1
/* Bit: 'RA110_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define AR9_RA_2F_2C_REG_RA110_MG_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA110_MG_SHIFT                   5
#define AR9_RA_2F_2C_REG_RA110_MG_SIZE                    1
/* Bit: 'RA110_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define AR9_RA_2F_2C_REG_RA110_CV_OFFSET                  0x01AC
#define AR9_RA_2F_2C_REG_RA110_CV_SHIFT                   4
#define AR9_RA_2F_2C_REG_RA110_CV_SIZE                    1
/* Bit: 'RA110_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define AR9_RA_2F_2C_REG_RA110_TXTAG_OFFSET               0x01AC
#define AR9_RA_2F_2C_REG_RA110_TXTAG_SHIFT                2
#define AR9_RA_2F_2C_REG_RA110_TXTAG_SIZE                 2
/* Bit: 'RA110_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define AR9_RA_2F_2C_REG_RA110_ACT_OFFSET                 0x01AC
#define AR9_RA_2F_2C_REG_RA110_ACT_SHIFT                  0
#define AR9_RA_2F_2C_REG_RA110_ACT_SIZE                   2
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter 0' */
/* Bit: 'PFR3' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F0_REG_PFR3_OFFSET                      0x01B0
#define AR9_PRTCL_F0_REG_PFR3_SHIFT                       24
#define AR9_PRTCL_F0_REG_PFR3_SIZE                        8
/* Bit: 'PFR2' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F0_REG_PFR2_OFFSET                      0x01B0
#define AR9_PRTCL_F0_REG_PFR2_SHIFT                       16
#define AR9_PRTCL_F0_REG_PFR2_SIZE                        8
/* Bit: 'PFR1' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F0_REG_PFR1_OFFSET                      0x01B0
#define AR9_PRTCL_F0_REG_PFR1_SHIFT                       8
#define AR9_PRTCL_F0_REG_PFR1_SIZE                        8
/* Bit: 'PFR0' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F0_REG_PFR0_OFFSET                      0x01B0
#define AR9_PRTCL_F0_REG_PFR0_SHIFT                       0
#define AR9_PRTCL_F0_REG_PFR0_SIZE                        8
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter 1' */
/* Bit: 'PFR3' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F1_REG_PFR3_OFFSET                      0x01B4
#define AR9_PRTCL_F1_REG_PFR3_SHIFT                       24
#define AR9_PRTCL_F1_REG_PFR3_SIZE                        8
/* Bit: 'PFR2' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F1_REG_PFR2_OFFSET                      0x01B4
#define AR9_PRTCL_F1_REG_PFR2_SHIFT                       16
#define AR9_PRTCL_F1_REG_PFR2_SIZE                        8
/* Bit: 'PFR1' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F1_REG_PFR1_OFFSET                      0x01B4
#define AR9_PRTCL_F1_REG_PFR1_SHIFT                       8
#define AR9_PRTCL_F1_REG_PFR1_SIZE                        8
/* Bit: 'PFR0' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define AR9_PRTCL_F1_REG_PFR0_OFFSET                      0x01B4
#define AR9_PRTCL_F1_REG_PFR0_SHIFT                       0
#define AR9_PRTCL_F1_REG_PFR0_SIZE                        8
/* -------------------------------------------------------------------------- */
#endif /* #ifndef _AR9_H */
