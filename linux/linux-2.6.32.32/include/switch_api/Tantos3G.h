/******************************************************************************

                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef _TANTOS3G_H
#define _TANTOS3G_H
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Status Register' */
/* Bit: 'P0FCS' */
/* Description: 'Port 0 Flow Control Status' */
#define TANTOS_3G_P0S_P0FCS_OFFSET                        0x00
#define TANTOS_3G_P0S_P0FCS_SHIFT                         4
#define TANTOS_3G_P0S_P0FCS_SIZE                          1
/* Bit: 'P0DS' */
/* Description: 'Port 0 Duplex Status' */
#define TANTOS_3G_P0S_P0DS_OFFSET                         0x00
#define TANTOS_3G_P0S_P0DS_SHIFT                          3
#define TANTOS_3G_P0S_P0DS_SIZE                           1
/* Bit: 'P0SHS' */
/* Description: 'Port 0 Speed High Status' */
#define TANTOS_3G_P0S_P0SHS_OFFSET                        0x00
#define TANTOS_3G_P0S_P0SHS_SHIFT                         2
#define TANTOS_3G_P0S_P0SHS_SIZE                          1
/* Bit: 'P0SS' */
/* Description: 'Port 0 Speed Status' */
#define TANTOS_3G_P0S_P0SS_OFFSET                         0x00
#define TANTOS_3G_P0S_P0SS_SHIFT                          1
#define TANTOS_3G_P0S_P0SS_SIZE                           1
/* Bit: 'P0LS' */
/* Description: 'Port 0 Link Status' */
#define TANTOS_3G_P0S_P0LS_OFFSET                         0x00
#define TANTOS_3G_P0S_P0LS_SHIFT                          0
#define TANTOS_3G_P0S_P0LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Status Register' */
/* Bit: 'P1FCS' */
/* Description: 'Port 1 Flow Control Status' */
#define TANTOS_3G_P1S_P1FCS_OFFSET                        0x20
#define TANTOS_3G_P1S_P1FCS_SHIFT                         4
#define TANTOS_3G_P1S_P1FCS_SIZE                          1
/* Bit: 'P1DS' */
/* Description: 'Port 1 Duplex Status' */
#define TANTOS_3G_P1S_P1DS_OFFSET                         0x20
#define TANTOS_3G_P1S_P1DS_SHIFT                          3
#define TANTOS_3G_P1S_P1DS_SIZE                           1
/* Bit: 'P1SHS' */
/* Description: 'Port 1 Speed High Status' */
#define TANTOS_3G_P1S_P1SHS_OFFSET                        0x20
#define TANTOS_3G_P1S_P1SHS_SHIFT                         2
#define TANTOS_3G_P1S_P1SHS_SIZE                          1
/* Bit: 'P1SS' */
/* Description: 'Port 1 Speed Status' */
#define TANTOS_3G_P1S_P1SS_OFFSET                         0x20
#define TANTOS_3G_P1S_P1SS_SHIFT                          1
#define TANTOS_3G_P1S_P1SS_SIZE                           1
/* Bit: 'P1LS' */
/* Description: 'Port 1 Link Status' */
#define TANTOS_3G_P1S_P1LS_OFFSET                         0x20
#define TANTOS_3G_P1S_P1LS_SHIFT                          0
#define TANTOS_3G_P1S_P1LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Status Register' */
/* Bit: 'P2FCS' */
/* Description: 'Port 2 Flow Control Status' */
#define TANTOS_3G_P2S_P2FCS_OFFSET                        0x40
#define TANTOS_3G_P2S_P2FCS_SHIFT                         4
#define TANTOS_3G_P2S_P2FCS_SIZE                          1
/* Bit: 'P2DS' */
/* Description: 'Port 2 Duplex Status' */
#define TANTOS_3G_P2S_P2DS_OFFSET                         0x40
#define TANTOS_3G_P2S_P2DS_SHIFT                          3
#define TANTOS_3G_P2S_P2DS_SIZE                           1
/* Bit: 'P2SHS' */
/* Description: 'Port 2 Speed High Status' */
#define TANTOS_3G_P2S_P2SHS_OFFSET                        0x40
#define TANTOS_3G_P2S_P2SHS_SHIFT                         2
#define TANTOS_3G_P2S_P2SHS_SIZE                          1
/* Bit: 'P2SS' */
/* Description: 'Port 2 Speed Status' */
#define TANTOS_3G_P2S_P2SS_OFFSET                         0x40
#define TANTOS_3G_P2S_P2SS_SHIFT                          1
#define TANTOS_3G_P2S_P2SS_SIZE                           1
/* Bit: 'P2LS' */
/* Description: 'Port 2 Link Status' */
#define TANTOS_3G_P2S_P2LS_OFFSET                         0x40
#define TANTOS_3G_P2S_P2LS_SHIFT                          0
#define TANTOS_3G_P2S_P2LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Status Register' */
/* Bit: 'P3FCS' */
/* Description: 'Port 3 Flow Control Status' */
#define TANTOS_3G_P3S_P3FCS_OFFSET                        0x60
#define TANTOS_3G_P3S_P3FCS_SHIFT                         4
#define TANTOS_3G_P3S_P3FCS_SIZE                          1
/* Bit: 'P3DS' */
/* Description: 'Port 3 Duplex Status' */
#define TANTOS_3G_P3S_P3DS_OFFSET                         0x60
#define TANTOS_3G_P3S_P3DS_SHIFT                          3
#define TANTOS_3G_P3S_P3DS_SIZE                           1
/* Bit: 'P3SHS' */
/* Description: 'Port 3 Speed High Status' */
#define TANTOS_3G_P3S_P3SHS_OFFSET                        0x60
#define TANTOS_3G_P3S_P3SHS_SHIFT                         2
#define TANTOS_3G_P3S_P3SHS_SIZE                          1
/* Bit: 'P3SS' */
/* Description: 'Port 3 Speed Status' */
#define TANTOS_3G_P3S_P3SS_OFFSET                         0x60
#define TANTOS_3G_P3S_P3SS_SHIFT                          1
#define TANTOS_3G_P3S_P3SS_SIZE                           1
/* Bit: 'P3LS' */
/* Description: 'Port 3 Link Status' */
#define TANTOS_3G_P3S_P3LS_OFFSET                         0x60
#define TANTOS_3G_P3S_P3LS_SHIFT                          0
#define TANTOS_3G_P3S_P3LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Status Register' */
/* Bit: 'P4FCS' */
/* Description: 'Port 4 Flow Control Status' */
#define TANTOS_3G_P4S_P4FCS_OFFSET                        0x80
#define TANTOS_3G_P4S_P4FCS_SHIFT                         4
#define TANTOS_3G_P4S_P4FCS_SIZE                          1
/* Bit: 'P4DS' */
/* Description: 'Port 4 Duplex Status' */
#define TANTOS_3G_P4S_P4DS_OFFSET                         0x80
#define TANTOS_3G_P4S_P4DS_SHIFT                          3
#define TANTOS_3G_P4S_P4DS_SIZE                           1
/* Bit: 'P4SHS' */
/* Description: 'Port 4 Speed High Status' */
#define TANTOS_3G_P4S_P4SHS_OFFSET                        0x80
#define TANTOS_3G_P4S_P4SHS_SHIFT                         2
#define TANTOS_3G_P4S_P4SHS_SIZE                          1
/* Bit: 'P4SS' */
/* Description: 'Port 4 Speed Status' */
#define TANTOS_3G_P4S_P4SS_OFFSET                         0x80
#define TANTOS_3G_P4S_P4SS_SHIFT                          1
#define TANTOS_3G_P4S_P4SS_SIZE                           1
/* Bit: 'P4LS' */
/* Description: 'Port 4 Link Status' */
#define TANTOS_3G_P4S_P4LS_OFFSET                         0x80
#define TANTOS_3G_P4S_P4LS_SHIFT                          0
#define TANTOS_3G_P4S_P4LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Status Register' */
/* Bit: 'P5FCS' */
/* Description: 'Port 5 Flow Control Status' */
#define TANTOS_3G_P5S_P5FCS_OFFSET                        0xA0
#define TANTOS_3G_P5S_P5FCS_SHIFT                         4
#define TANTOS_3G_P5S_P5FCS_SIZE                          1
/* Bit: 'P5DS' */
/* Description: 'Port 5 Duplex Status' */
#define TANTOS_3G_P5S_P5DS_OFFSET                         0xA0
#define TANTOS_3G_P5S_P5DS_SHIFT                          3
#define TANTOS_3G_P5S_P5DS_SIZE                           1
/* Bit: 'P5SHS' */
/* Description: 'Port 5 Speed High Status' */
#define TANTOS_3G_P5S_P5SHS_OFFSET                        0xA0
#define TANTOS_3G_P5S_P5SHS_SHIFT                         2
#define TANTOS_3G_P5S_P5SHS_SIZE                          1
/* Bit: 'P5SS' */
/* Description: 'Port 5 Speed Status' */
#define TANTOS_3G_P5S_P5SS_OFFSET                         0xA0
#define TANTOS_3G_P5S_P5SS_SHIFT                          1
#define TANTOS_3G_P5S_P5SS_SIZE                           1
/* Bit: 'P5LS' */
/* Description: 'Port 5 Link Status' */
#define TANTOS_3G_P5S_P5LS_OFFSET                         0xA0
#define TANTOS_3G_P5S_P5LS_SHIFT                          0
#define TANTOS_3G_P5S_P5LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Status Register' */
/* Bit: 'P6FCS' */
/* Description: 'Port 6 Flow Control Status' */
#define TANTOS_3G_P6S_P6FCS_OFFSET                        0xC0
#define TANTOS_3G_P6S_P6FCS_SHIFT                         4
#define TANTOS_3G_P6S_P6FCS_SIZE                          1
/* Bit: 'P6DS' */
/* Description: 'Port 6 Duplex Status' */
#define TANTOS_3G_P6S_P6DS_OFFSET                         0xC0
#define TANTOS_3G_P6S_P6DS_SHIFT                          3
#define TANTOS_3G_P6S_P6DS_SIZE                           1
/* Bit: 'P6SHS' */
/* Description: 'Port 6 Speed High Status' */
#define TANTOS_3G_P6S_P6SHS_OFFSET                        0xC0
#define TANTOS_3G_P6S_P6SHS_SHIFT                         2
#define TANTOS_3G_P6S_P6SHS_SIZE                          1
/* Bit: 'P6SS' */
/* Description: 'Port 6 Speed Status' */
#define TANTOS_3G_P6S_P6SS_OFFSET                         0xC0
#define TANTOS_3G_P6S_P6SS_SHIFT                          1
#define TANTOS_3G_P6S_P6SS_SIZE                           1
/* Bit: 'P6LS' */
/* Description: 'Port 6 Link Status' */
#define TANTOS_3G_P6S_P6LS_OFFSET                         0xC0
#define TANTOS_3G_P6S_P6LS_SHIFT                          0
#define TANTOS_3G_P6S_P6LS_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'P0 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P0BC_SPS_OFFSET                         0x01
#define TANTOS_3G_P0BC_SPS_SHIFT                          14
#define TANTOS_3G_P0BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P0BC_TCPE_OFFSET                        0x01
#define TANTOS_3G_P0BC_TCPE_SHIFT                         13
#define TANTOS_3G_P0BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P0BC_IPOVTU_OFFSET                      0x01
#define TANTOS_3G_P0BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P0BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P0BC_VPE_OFFSET                         0x01
#define TANTOS_3G_P0BC_VPE_SHIFT                          11
#define TANTOS_3G_P0BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P0BC_SPE_OFFSET                         0x01
#define TANTOS_3G_P0BC_SPE_SHIFT                          10
#define TANTOS_3G_P0BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P0BC_IPVLAN_OFFSET                      0x01
#define TANTOS_3G_P0BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P0BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P0BC_TPE_OFFSET                         0x01
#define TANTOS_3G_P0BC_TPE_SHIFT                          8
#define TANTOS_3G_P0BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P0BC_FLP_OFFSET                         0x01
#define TANTOS_3G_P0BC_FLP_SHIFT                          2
#define TANTOS_3G_P0BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P0BC_FLD_OFFSET                         0x01
#define TANTOS_3G_P0BC_FLD_SHIFT                          1
#define TANTOS_3G_P0BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P0BC_RMWFQ_OFFSET                       0x01
#define TANTOS_3G_P0BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P0BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P1 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P1BC_SPS_OFFSET                         0x21
#define TANTOS_3G_P1BC_SPS_SHIFT                          14
#define TANTOS_3G_P1BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P1BC_TCPE_OFFSET                        0x21
#define TANTOS_3G_P1BC_TCPE_SHIFT                         13
#define TANTOS_3G_P1BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P1BC_IPOVTU_OFFSET                      0x21
#define TANTOS_3G_P1BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P1BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P1BC_VPE_OFFSET                         0x21
#define TANTOS_3G_P1BC_VPE_SHIFT                          11
#define TANTOS_3G_P1BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P1BC_SPE_OFFSET                         0x21
#define TANTOS_3G_P1BC_SPE_SHIFT                          10
#define TANTOS_3G_P1BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P1BC_IPVLAN_OFFSET                      0x21
#define TANTOS_3G_P1BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P1BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P1BC_TPE_OFFSET                         0x21
#define TANTOS_3G_P1BC_TPE_SHIFT                          8
#define TANTOS_3G_P1BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P1BC_FLP_OFFSET                         0x21
#define TANTOS_3G_P1BC_FLP_SHIFT                          2
#define TANTOS_3G_P1BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P1BC_FLD_OFFSET                         0x21
#define TANTOS_3G_P1BC_FLD_SHIFT                          1
#define TANTOS_3G_P1BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P1BC_RMWFQ_OFFSET                       0x21
#define TANTOS_3G_P1BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P1BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P2 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P2BC_SPS_OFFSET                         0x41
#define TANTOS_3G_P2BC_SPS_SHIFT                          14
#define TANTOS_3G_P2BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P2BC_TCPE_OFFSET                        0x41
#define TANTOS_3G_P2BC_TCPE_SHIFT                         13
#define TANTOS_3G_P2BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P2BC_IPOVTU_OFFSET                      0x41
#define TANTOS_3G_P2BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P2BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P2BC_VPE_OFFSET                         0x41
#define TANTOS_3G_P2BC_VPE_SHIFT                          11
#define TANTOS_3G_P2BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P2BC_SPE_OFFSET                         0x41
#define TANTOS_3G_P2BC_SPE_SHIFT                          10
#define TANTOS_3G_P2BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P2BC_IPVLAN_OFFSET                      0x41
#define TANTOS_3G_P2BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P2BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P2BC_TPE_OFFSET                         0x41
#define TANTOS_3G_P2BC_TPE_SHIFT                          8
#define TANTOS_3G_P2BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P2BC_FLP_OFFSET                         0x41
#define TANTOS_3G_P2BC_FLP_SHIFT                          2
#define TANTOS_3G_P2BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P2BC_FLD_OFFSET                         0x41
#define TANTOS_3G_P2BC_FLD_SHIFT                          1
#define TANTOS_3G_P2BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P2BC_RMWFQ_OFFSET                       0x41
#define TANTOS_3G_P2BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P2BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P3 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P3BC_SPS_OFFSET                         0x61
#define TANTOS_3G_P3BC_SPS_SHIFT                          14
#define TANTOS_3G_P3BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P3BC_TCPE_OFFSET                        0x61
#define TANTOS_3G_P3BC_TCPE_SHIFT                         13
#define TANTOS_3G_P3BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P3BC_IPOVTU_OFFSET                      0x61
#define TANTOS_3G_P3BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P3BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P3BC_VPE_OFFSET                         0x61
#define TANTOS_3G_P3BC_VPE_SHIFT                          11
#define TANTOS_3G_P3BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P3BC_SPE_OFFSET                         0x61
#define TANTOS_3G_P3BC_SPE_SHIFT                          10
#define TANTOS_3G_P3BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P3BC_IPVLAN_OFFSET                      0x61
#define TANTOS_3G_P3BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P3BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P3BC_TPE_OFFSET                         0x61
#define TANTOS_3G_P3BC_TPE_SHIFT                          8
#define TANTOS_3G_P3BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P3BC_FLP_OFFSET                         0x61
#define TANTOS_3G_P3BC_FLP_SHIFT                          2
#define TANTOS_3G_P3BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P3BC_FLD_OFFSET                         0x61
#define TANTOS_3G_P3BC_FLD_SHIFT                          1
#define TANTOS_3G_P3BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P3BC_RMWFQ_OFFSET                       0x61
#define TANTOS_3G_P3BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P3BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P4 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P4BC_SPS_OFFSET                         0x81
#define TANTOS_3G_P4BC_SPS_SHIFT                          14
#define TANTOS_3G_P4BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P4BC_TCPE_OFFSET                        0x81
#define TANTOS_3G_P4BC_TCPE_SHIFT                         13
#define TANTOS_3G_P4BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P4BC_IPOVTU_OFFSET                      0x81
#define TANTOS_3G_P4BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P4BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P4BC_VPE_OFFSET                         0x81
#define TANTOS_3G_P4BC_VPE_SHIFT                          11
#define TANTOS_3G_P4BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P4BC_SPE_OFFSET                         0x81
#define TANTOS_3G_P4BC_SPE_SHIFT                          10
#define TANTOS_3G_P4BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P4BC_IPVLAN_OFFSET                      0x81
#define TANTOS_3G_P4BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P4BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P4BC_TPE_OFFSET                         0x81
#define TANTOS_3G_P4BC_TPE_SHIFT                          8
#define TANTOS_3G_P4BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P4BC_FLP_OFFSET                         0x81
#define TANTOS_3G_P4BC_FLP_SHIFT                          2
#define TANTOS_3G_P4BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P4BC_FLD_OFFSET                         0x81
#define TANTOS_3G_P4BC_FLD_SHIFT                          1
#define TANTOS_3G_P4BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P4BC_RMWFQ_OFFSET                       0x81
#define TANTOS_3G_P4BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P4BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P5 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P5BC_SPS_OFFSET                         0xA1
#define TANTOS_3G_P5BC_SPS_SHIFT                          14
#define TANTOS_3G_P5BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P5BC_TCPE_OFFSET                        0xA1
#define TANTOS_3G_P5BC_TCPE_SHIFT                         13
#define TANTOS_3G_P5BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P5BC_IPOVTU_OFFSET                      0xA1
#define TANTOS_3G_P5BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P5BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P5BC_VPE_OFFSET                         0xA1
#define TANTOS_3G_P5BC_VPE_SHIFT                          11
#define TANTOS_3G_P5BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P5BC_SPE_OFFSET                         0xA1
#define TANTOS_3G_P5BC_SPE_SHIFT                          10
#define TANTOS_3G_P5BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P5BC_IPVLAN_OFFSET                      0xA1
#define TANTOS_3G_P5BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P5BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P5BC_TPE_OFFSET                         0xA1
#define TANTOS_3G_P5BC_TPE_SHIFT                          8
#define TANTOS_3G_P5BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P5BC_FLP_OFFSET                         0xA1
#define TANTOS_3G_P5BC_FLP_SHIFT                          2
#define TANTOS_3G_P5BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P5BC_FLD_OFFSET                         0xA1
#define TANTOS_3G_P5BC_FLD_SHIFT                          1
#define TANTOS_3G_P5BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P5BC_RMWFQ_OFFSET                       0xA1
#define TANTOS_3G_P5BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P5BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P6 Basic Control Register' */
/* Bit: 'SPS' */
/* Description: 'STP/RSTP port state' */
#define TANTOS_3G_P6BC_SPS_OFFSET                         0xC1
#define TANTOS_3G_P6BC_SPS_SHIFT                          14
#define TANTOS_3G_P6BC_SPS_SIZE                           2
/* Bit: 'TCPE' */
/* Description: 'TCP/UDP PRIEN' */
#define TANTOS_3G_P6BC_TCPE_OFFSET                        0xC1
#define TANTOS_3G_P6BC_TCPE_SHIFT                         13
#define TANTOS_3G_P6BC_TCPE_SIZE                          1
/* Bit: 'IPOVTU' */
/* Description: ' IP over TCP/UDP' */
#define TANTOS_3G_P6BC_IPOVTU_OFFSET                      0xC1
#define TANTOS_3G_P6BC_IPOVTU_SHIFT                       12
#define TANTOS_3G_P6BC_IPOVTU_SIZE                        1
/* Bit: 'VPE' */
/* Description: 'VLAN Priority Enable' */
#define TANTOS_3G_P6BC_VPE_OFFSET                         0xC1
#define TANTOS_3G_P6BC_VPE_SHIFT                          11
#define TANTOS_3G_P6BC_VPE_SIZE                           1
/* Bit: 'SPE' */
/* Description: 'Service Priority Enable' */
#define TANTOS_3G_P6BC_SPE_OFFSET                         0xC1
#define TANTOS_3G_P6BC_SPE_SHIFT                          10
#define TANTOS_3G_P6BC_SPE_SIZE                           1
/* Bit: 'IPVLAN' */
/* Description: 'IP over VLAN PRI' */
#define TANTOS_3G_P6BC_IPVLAN_OFFSET                      0xC1
#define TANTOS_3G_P6BC_IPVLAN_SHIFT                       9
#define TANTOS_3G_P6BC_IPVLAN_SIZE                        1
/* Bit: 'TPE' */
/* Description: 'Ether Type Priority Enable' */
#define TANTOS_3G_P6BC_TPE_OFFSET                         0xC1
#define TANTOS_3G_P6BC_TPE_SHIFT                          8
#define TANTOS_3G_P6BC_TPE_SIZE                           1
/* Bit: 'FLP' */
/* Description: 'Force Link Up' */
#define TANTOS_3G_P6BC_FLP_OFFSET                         0xC1
#define TANTOS_3G_P6BC_FLP_SHIFT                          2
#define TANTOS_3G_P6BC_FLP_SIZE                           1
/* Bit: 'FLD' */
/* Description: 'Force Link Down' */
#define TANTOS_3G_P6BC_FLD_OFFSET                         0xC1
#define TANTOS_3G_P6BC_FLD_SHIFT                          1
#define TANTOS_3G_P6BC_FLD_SIZE                           1
/* Bit: 'RMWFQ' */
/* Description: 'Ratio Mode for WFQ' */
#define TANTOS_3G_P6BC_RMWFQ_OFFSET                       0xC1
#define TANTOS_3G_P6BC_RMWFQ_SHIFT                        0
#define TANTOS_3G_P6BC_RMWFQ_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P0 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P0EC_AD_OFFSET                          0x02
#define TANTOS_3G_P0EC_AD_SHIFT                           15
#define TANTOS_3G_P0EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P0EC_LD_OFFSET                          0x02
#define TANTOS_3G_P0EC_LD_SHIFT                           14
#define TANTOS_3G_P0EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P0EC_IMTE_OFFSET                        0x02
#define TANTOS_3G_P0EC_IMTE_SHIFT                         13
#define TANTOS_3G_P0EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P0EC_MNA024_OFFSET                      0x02
#define TANTOS_3G_P0EC_MNA024_SHIFT                       8
#define TANTOS_3G_P0EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P0EC_PPPOEP_OFFSET                      0x02
#define TANTOS_3G_P0EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P0EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P0EC_PM_OFFSET                          0x02
#define TANTOS_3G_P0EC_PM_SHIFT                           6
#define TANTOS_3G_P0EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P0EC_IPMO_OFFSET                        0x02
#define TANTOS_3G_P0EC_IPMO_SHIFT                         4
#define TANTOS_3G_P0EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P0EC_PAS_OFFSET                         0x02
#define TANTOS_3G_P0EC_PAS_SHIFT                          2
#define TANTOS_3G_P0EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P0EC_IFNTE_OFFSET                       0x02
#define TANTOS_3G_P0EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P0EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P1 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P1EC_AD_OFFSET                          0x22
#define TANTOS_3G_P1EC_AD_SHIFT                           15
#define TANTOS_3G_P1EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P1EC_LD_OFFSET                          0x22
#define TANTOS_3G_P1EC_LD_SHIFT                           14
#define TANTOS_3G_P1EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P1EC_IMTE_OFFSET                        0x22
#define TANTOS_3G_P1EC_IMTE_SHIFT                         13
#define TANTOS_3G_P1EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P1EC_MNA024_OFFSET                      0x22
#define TANTOS_3G_P1EC_MNA024_SHIFT                       8
#define TANTOS_3G_P1EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P1EC_PPPOEP_OFFSET                      0x22
#define TANTOS_3G_P1EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P1EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P1EC_PM_OFFSET                          0x22
#define TANTOS_3G_P1EC_PM_SHIFT                           6
#define TANTOS_3G_P1EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P1EC_IPMO_OFFSET                        0x22
#define TANTOS_3G_P1EC_IPMO_SHIFT                         4
#define TANTOS_3G_P1EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P1EC_PAS_OFFSET                         0x22
#define TANTOS_3G_P1EC_PAS_SHIFT                          2
#define TANTOS_3G_P1EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P1EC_IFNTE_OFFSET                       0x22
#define TANTOS_3G_P1EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P1EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P2 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P2EC_AD_OFFSET                          0x42
#define TANTOS_3G_P2EC_AD_SHIFT                           15
#define TANTOS_3G_P2EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P2EC_LD_OFFSET                          0x42
#define TANTOS_3G_P2EC_LD_SHIFT                           14
#define TANTOS_3G_P2EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P2EC_IMTE_OFFSET                        0x42
#define TANTOS_3G_P2EC_IMTE_SHIFT                         13
#define TANTOS_3G_P2EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P2EC_MNA024_OFFSET                      0x42
#define TANTOS_3G_P2EC_MNA024_SHIFT                       8
#define TANTOS_3G_P2EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P2EC_PPPOEP_OFFSET                      0x42
#define TANTOS_3G_P2EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P2EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P2EC_PM_OFFSET                          0x42
#define TANTOS_3G_P2EC_PM_SHIFT                           6
#define TANTOS_3G_P2EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P2EC_IPMO_OFFSET                        0x42
#define TANTOS_3G_P2EC_IPMO_SHIFT                         4
#define TANTOS_3G_P2EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P2EC_PAS_OFFSET                         0x42
#define TANTOS_3G_P2EC_PAS_SHIFT                          2
#define TANTOS_3G_P2EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P2EC_IFNTE_OFFSET                       0x42
#define TANTOS_3G_P2EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P2EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P3 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P3EC_AD_OFFSET                          0x62
#define TANTOS_3G_P3EC_AD_SHIFT                           15
#define TANTOS_3G_P3EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P3EC_LD_OFFSET                          0x62
#define TANTOS_3G_P3EC_LD_SHIFT                           14
#define TANTOS_3G_P3EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P3EC_IMTE_OFFSET                        0x62
#define TANTOS_3G_P3EC_IMTE_SHIFT                         13
#define TANTOS_3G_P3EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P3EC_MNA024_OFFSET                      0x62
#define TANTOS_3G_P3EC_MNA024_SHIFT                       8
#define TANTOS_3G_P3EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P3EC_PPPOEP_OFFSET                      0x62
#define TANTOS_3G_P3EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P3EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P3EC_PM_OFFSET                          0x62
#define TANTOS_3G_P3EC_PM_SHIFT                           6
#define TANTOS_3G_P3EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P3EC_IPMO_OFFSET                        0x62
#define TANTOS_3G_P3EC_IPMO_SHIFT                         4
#define TANTOS_3G_P3EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P3EC_PAS_OFFSET                         0x62
#define TANTOS_3G_P3EC_PAS_SHIFT                          2
#define TANTOS_3G_P3EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P3EC_IFNTE_OFFSET                       0x62
#define TANTOS_3G_P3EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P3EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P4 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P4EC_AD_OFFSET                          0x82
#define TANTOS_3G_P4EC_AD_SHIFT                           15
#define TANTOS_3G_P4EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P4EC_LD_OFFSET                          0x82
#define TANTOS_3G_P4EC_LD_SHIFT                           14
#define TANTOS_3G_P4EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P4EC_IMTE_OFFSET                        0x82
#define TANTOS_3G_P4EC_IMTE_SHIFT                         13
#define TANTOS_3G_P4EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P4EC_MNA024_OFFSET                      0x82
#define TANTOS_3G_P4EC_MNA024_SHIFT                       8
#define TANTOS_3G_P4EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P4EC_PPPOEP_OFFSET                      0x82
#define TANTOS_3G_P4EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P4EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P4EC_PM_OFFSET                          0x82
#define TANTOS_3G_P4EC_PM_SHIFT                           6
#define TANTOS_3G_P4EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P4EC_IPMO_OFFSET                        0x82
#define TANTOS_3G_P4EC_IPMO_SHIFT                         4
#define TANTOS_3G_P4EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P4EC_PAS_OFFSET                         0x82
#define TANTOS_3G_P4EC_PAS_SHIFT                          2
#define TANTOS_3G_P4EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P4EC_IFNTE_OFFSET                       0x82
#define TANTOS_3G_P4EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P4EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P5 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P5EC_AD_OFFSET                          0xA2
#define TANTOS_3G_P5EC_AD_SHIFT                           15
#define TANTOS_3G_P5EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P5EC_LD_OFFSET                          0xA2
#define TANTOS_3G_P5EC_LD_SHIFT                           14
#define TANTOS_3G_P5EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P5EC_IMTE_OFFSET                        0xA2
#define TANTOS_3G_P5EC_IMTE_SHIFT                         13
#define TANTOS_3G_P5EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P5EC_MNA024_OFFSET                      0xA2
#define TANTOS_3G_P5EC_MNA024_SHIFT                       8
#define TANTOS_3G_P5EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P5EC_PPPOEP_OFFSET                      0xA2
#define TANTOS_3G_P5EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P5EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P5EC_PM_OFFSET                          0xA2
#define TANTOS_3G_P5EC_PM_SHIFT                           6
#define TANTOS_3G_P5EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P5EC_IPMO_OFFSET                        0xA2
#define TANTOS_3G_P5EC_IPMO_SHIFT                         4
#define TANTOS_3G_P5EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P5EC_PAS_OFFSET                         0xA2
#define TANTOS_3G_P5EC_PAS_SHIFT                          2
#define TANTOS_3G_P5EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P5EC_IFNTE_OFFSET                       0xA2
#define TANTOS_3G_P5EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P5EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'P6 Extended Control Register' */
/* Bit: 'AD' */
/* Description: 'Aging Disable' */
#define TANTOS_3G_P6EC_AD_OFFSET                          0xC2
#define TANTOS_3G_P6EC_AD_SHIFT                           15
#define TANTOS_3G_P6EC_AD_SIZE                            1
/* Bit: 'LD' */
/* Description: 'Learning Disable' */
#define TANTOS_3G_P6EC_LD_OFFSET                          0xC2
#define TANTOS_3G_P6EC_LD_SHIFT                           14
#define TANTOS_3G_P6EC_LD_SIZE                            1
/* Bit: 'IMTE' */
/* Description: 'IGMP/MLD Trap Enable' */
#define TANTOS_3G_P6EC_IMTE_OFFSET                        0xC2
#define TANTOS_3G_P6EC_IMTE_SHIFT                         13
#define TANTOS_3G_P6EC_IMTE_SIZE                          1
/* Bit: 'MNA024' */
/* Description: 'Maximum Number of Addresses' */
#define TANTOS_3G_P6EC_MNA024_OFFSET                      0xC2
#define TANTOS_3G_P6EC_MNA024_SHIFT                       8
#define TANTOS_3G_P6EC_MNA024_SIZE                        5
/* Bit: 'PPPOEP' */
/* Description: 'PPPOE Port Only' */
#define TANTOS_3G_P6EC_PPPOEP_OFFSET                      0xC2
#define TANTOS_3G_P6EC_PPPOEP_SHIFT                       7
#define TANTOS_3G_P6EC_PPPOEP_SIZE                        1
/* Bit: 'PM' */
/* Description: 'PPPOE Manage' */
#define TANTOS_3G_P6EC_PM_OFFSET                          0xC2
#define TANTOS_3G_P6EC_PM_SHIFT                           6
#define TANTOS_3G_P6EC_PM_SIZE                            1
/* Bit: 'IPMO' */
/* Description: 'Port Mirror Option' */
#define TANTOS_3G_P6EC_IPMO_OFFSET                        0xC2
#define TANTOS_3G_P6EC_IPMO_SHIFT                         4
#define TANTOS_3G_P6EC_IPMO_SIZE                          2
/* Bit: 'PAS' */
/* Description: '802.1x Port Authorized state' */
#define TANTOS_3G_P6EC_PAS_OFFSET                         0xC2
#define TANTOS_3G_P6EC_PAS_SHIFT                          2
#define TANTOS_3G_P6EC_PAS_SIZE                           2
/* Bit: 'IFNTE' */
/* Description: 'Input Force No TAG Enable' */
#define TANTOS_3G_P6EC_IFNTE_OFFSET                       0xC2
#define TANTOS_3G_P6EC_IFNTE_SHIFT                        1
#define TANTOS_3G_P6EC_IFNTE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P0PBVM_DFID_OFFSET                      0x03
#define TANTOS_3G_P0PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P0PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P0PBVM_TBVE_OFFSET                      0x03
#define TANTOS_3G_P0PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P0PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P0PBVM_VC_OFFSET                        0x03
#define TANTOS_3G_P0PBVM_VC_SHIFT                         11
#define TANTOS_3G_P0PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P0PBVM_VSD_OFFSET                       0x03
#define TANTOS_3G_P0PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P0PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P0PBVM_AOVTP_OFFSET                     0x03
#define TANTOS_3G_P0PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P0PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P0PBVM_VMCE_OFFSET                      0x03
#define TANTOS_3G_P0PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P0PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P0PBVM_BYPASS_OFFSET                    0x03
#define TANTOS_3G_P0PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P0PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P0PBVM_DVPM_OFFSET                      0x03
#define TANTOS_3G_P0PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P0PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P1PBVM_DFID_OFFSET                      0x23
#define TANTOS_3G_P1PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P1PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P1PBVM_TBVE_OFFSET                      0x23
#define TANTOS_3G_P1PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P1PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P1PBVM_VC_OFFSET                        0x23
#define TANTOS_3G_P1PBVM_VC_SHIFT                         11
#define TANTOS_3G_P1PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P1PBVM_VSD_OFFSET                       0x23
#define TANTOS_3G_P1PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P1PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P1PBVM_AOVTP_OFFSET                     0x23
#define TANTOS_3G_P1PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P1PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P1PBVM_VMCE_OFFSET                      0x23
#define TANTOS_3G_P1PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P1PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P1PBVM_BYPASS_OFFSET                    0x23
#define TANTOS_3G_P1PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P1PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P1PBVM_DVPM_OFFSET                      0x23
#define TANTOS_3G_P1PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P1PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P2PBVM_DFID_OFFSET                      0x43
#define TANTOS_3G_P2PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P2PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P2PBVM_TBVE_OFFSET                      0x43
#define TANTOS_3G_P2PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P2PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P2PBVM_VC_OFFSET                        0x43
#define TANTOS_3G_P2PBVM_VC_SHIFT                         11
#define TANTOS_3G_P2PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P2PBVM_VSD_OFFSET                       0x43
#define TANTOS_3G_P2PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P2PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P2PBVM_AOVTP_OFFSET                     0x43
#define TANTOS_3G_P2PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P2PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P2PBVM_VMCE_OFFSET                      0x43
#define TANTOS_3G_P2PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P2PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P2PBVM_BYPASS_OFFSET                    0x43
#define TANTOS_3G_P2PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P2PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P2PBVM_DVPM_OFFSET                      0x43
#define TANTOS_3G_P2PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P2PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P3PBVM_DFID_OFFSET                      0x63
#define TANTOS_3G_P3PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P3PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P3PBVM_TBVE_OFFSET                      0x63
#define TANTOS_3G_P3PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P3PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P3PBVM_VC_OFFSET                        0x63
#define TANTOS_3G_P3PBVM_VC_SHIFT                         11
#define TANTOS_3G_P3PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P3PBVM_VSD_OFFSET                       0x63
#define TANTOS_3G_P3PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P3PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P3PBVM_AOVTP_OFFSET                     0x63
#define TANTOS_3G_P3PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P3PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P3PBVM_VMCE_OFFSET                      0x63
#define TANTOS_3G_P3PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P3PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P3PBVM_BYPASS_OFFSET                    0x63
#define TANTOS_3G_P3PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P3PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P3PBVM_DVPM_OFFSET                      0x63
#define TANTOS_3G_P3PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P3PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P4PBVM_DFID_OFFSET                      0x83
#define TANTOS_3G_P4PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P4PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P4PBVM_TBVE_OFFSET                      0x83
#define TANTOS_3G_P4PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P4PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P4PBVM_VC_OFFSET                        0x83
#define TANTOS_3G_P4PBVM_VC_SHIFT                         11
#define TANTOS_3G_P4PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P4PBVM_VSD_OFFSET                       0x83
#define TANTOS_3G_P4PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P4PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P4PBVM_AOVTP_OFFSET                     0x83
#define TANTOS_3G_P4PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P4PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P4PBVM_VMCE_OFFSET                      0x83
#define TANTOS_3G_P4PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P4PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P4PBVM_BYPASS_OFFSET                    0x83
#define TANTOS_3G_P4PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P4PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P4PBVM_DVPM_OFFSET                      0x83
#define TANTOS_3G_P4PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P4PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P5PBVM_DFID_OFFSET                      0xA3
#define TANTOS_3G_P5PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P5PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P5PBVM_TBVE_OFFSET                      0xA3
#define TANTOS_3G_P5PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P5PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P5PBVM_VC_OFFSET                        0xA3
#define TANTOS_3G_P5PBVM_VC_SHIFT                         11
#define TANTOS_3G_P5PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P5PBVM_VSD_OFFSET                       0xA3
#define TANTOS_3G_P5PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P5PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P5PBVM_AOVTP_OFFSET                     0xA3
#define TANTOS_3G_P5PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P5PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P5PBVM_VMCE_OFFSET                      0xA3
#define TANTOS_3G_P5PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P5PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P5PBVM_BYPASS_OFFSET                    0xA3
#define TANTOS_3G_P5PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P5PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P5PBVM_DVPM_OFFSET                      0xA3
#define TANTOS_3G_P5PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P5PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Port Base VLAN Map Register' */
/* Bit: 'DFID' */
/* Description: 'Default FID' */
#define TANTOS_3G_P6PBVM_DFID_OFFSET                      0xC3
#define TANTOS_3G_P6PBVM_DFID_SHIFT                       14
#define TANTOS_3G_P6PBVM_DFID_SIZE                        2
/* Bit: 'TBVE' */
/* Description: 'Tagged Base VLAN Enable' */
#define TANTOS_3G_P6PBVM_TBVE_OFFSET                      0xC3
#define TANTOS_3G_P6PBVM_TBVE_SHIFT                       13
#define TANTOS_3G_P6PBVM_TBVE_SIZE                        1
/* Bit: 'VC' */
/* Description: 'VID Check with the VID table' */
#define TANTOS_3G_P6PBVM_VC_OFFSET                        0xC3
#define TANTOS_3G_P6PBVM_VC_SHIFT                         11
#define TANTOS_3G_P6PBVM_VC_SIZE                          1
/* Bit: 'VSD' */
/* Description: 'VLAN Security Disable' */
#define TANTOS_3G_P6PBVM_VSD_OFFSET                       0xC3
#define TANTOS_3G_P6PBVM_VSD_SHIFT                        10
#define TANTOS_3G_P6PBVM_VSD_SIZE                         1
/* Bit: 'AOVTP' */
/* Description: 'Admit Only VLAN_Tagged Packet' */
#define TANTOS_3G_P6PBVM_AOVTP_OFFSET                     0xC3
#define TANTOS_3G_P6PBVM_AOVTP_SHIFT                      9
#define TANTOS_3G_P6PBVM_AOVTP_SIZE                       1
/* Bit: 'VMCE' */
/* Description: 'VLAN Member Check Enable' */
#define TANTOS_3G_P6PBVM_VMCE_OFFSET                      0xC3
#define TANTOS_3G_P6PBVM_VMCE_SHIFT                       8
#define TANTOS_3G_P6PBVM_VMCE_SIZE                        1
/* Bit: 'BYPASS' */
/* Description: 'ByPass Mode for Output' */
#define TANTOS_3G_P6PBVM_BYPASS_OFFSET                    0xC3
#define TANTOS_3G_P6PBVM_BYPASS_SHIFT                     7
#define TANTOS_3G_P6PBVM_BYPASS_SIZE                      1
/* Bit: 'DVPM' */
/* Description: 'Port Based VLAN Port Map' */
#define TANTOS_3G_P6PBVM_DVPM_OFFSET                      0xC3
#define TANTOS_3G_P6PBVM_DVPM_SHIFT                       0
#define TANTOS_3G_P6PBVM_DVPM_SIZE                        7
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P0DVID_PP_OFFSET                        0x04
#define TANTOS_3G_P0DVID_PP_SHIFT                         14
#define TANTOS_3G_P0DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P0DVID_PPE_OFFSET                       0x04
#define TANTOS_3G_P0DVID_PPE_SHIFT                        13
#define TANTOS_3G_P0DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P0DVID_PVTAGMP_OFFSET                   0x04
#define TANTOS_3G_P0DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P0DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P0DVID_PVID_OFFSET                      0x04
#define TANTOS_3G_P0DVID_PVID_SHIFT                       0
#define TANTOS_3G_P0DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P1DVID_PP_OFFSET                        0x24
#define TANTOS_3G_P1DVID_PP_SHIFT                         14
#define TANTOS_3G_P1DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P1DVID_PPE_OFFSET                       0x24
#define TANTOS_3G_P1DVID_PPE_SHIFT                        13
#define TANTOS_3G_P1DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P1DVID_PVTAGMP_OFFSET                   0x24
#define TANTOS_3G_P1DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P1DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P1DVID_PVID_OFFSET                      0x24
#define TANTOS_3G_P1DVID_PVID_SHIFT                       0
#define TANTOS_3G_P1DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P2DVID_PP_OFFSET                        0x44
#define TANTOS_3G_P2DVID_PP_SHIFT                         14
#define TANTOS_3G_P2DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P2DVID_PPE_OFFSET                       0x44
#define TANTOS_3G_P2DVID_PPE_SHIFT                        13
#define TANTOS_3G_P2DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P2DVID_PVTAGMP_OFFSET                   0x44
#define TANTOS_3G_P2DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P2DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P2DVID_PVID_OFFSET                      0x44
#define TANTOS_3G_P2DVID_PVID_SHIFT                       0
#define TANTOS_3G_P2DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P3DVID_PP_OFFSET                        0x64
#define TANTOS_3G_P3DVID_PP_SHIFT                         14
#define TANTOS_3G_P3DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P3DVID_PPE_OFFSET                       0x64
#define TANTOS_3G_P3DVID_PPE_SHIFT                        13
#define TANTOS_3G_P3DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P3DVID_PVTAGMP_OFFSET                   0x64
#define TANTOS_3G_P3DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P3DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P3DVID_PVID_OFFSET                      0x64
#define TANTOS_3G_P3DVID_PVID_SHIFT                       0
#define TANTOS_3G_P3DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P4DVID_PP_OFFSET                        0x84
#define TANTOS_3G_P4DVID_PP_SHIFT                         14
#define TANTOS_3G_P4DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P4DVID_PPE_OFFSET                       0x84
#define TANTOS_3G_P4DVID_PPE_SHIFT                        13
#define TANTOS_3G_P4DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P4DVID_PVTAGMP_OFFSET                   0x84
#define TANTOS_3G_P4DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P4DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P4DVID_PVID_OFFSET                      0x84
#define TANTOS_3G_P4DVID_PVID_SHIFT                       0
#define TANTOS_3G_P4DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P5DVID_PP_OFFSET                        0xA4
#define TANTOS_3G_P5DVID_PP_SHIFT                         14
#define TANTOS_3G_P5DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P5DVID_PPE_OFFSET                       0xA4
#define TANTOS_3G_P5DVID_PPE_SHIFT                        13
#define TANTOS_3G_P5DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P5DVID_PVTAGMP_OFFSET                   0xA4
#define TANTOS_3G_P5DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P5DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P5DVID_PVID_OFFSET                      0xA4
#define TANTOS_3G_P5DVID_PVID_SHIFT                       0
#define TANTOS_3G_P5DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Default VLAN ID & Priority Register' */
/* Bit: 'PP' */
/* Description: 'Port Priority' */
#define TANTOS_3G_P6DVID_PP_OFFSET                        0xC4
#define TANTOS_3G_P6DVID_PP_SHIFT                         14
#define TANTOS_3G_P6DVID_PP_SIZE                          2
/* Bit: 'PPE' */
/* Description: 'Port Priority Enable' */
#define TANTOS_3G_P6DVID_PPE_OFFSET                       0xC4
#define TANTOS_3G_P6DVID_PPE_SHIFT                        13
#define TANTOS_3G_P6DVID_PPE_SIZE                         1
/* Bit: 'PVTAGMP' */
/* Description: 'Portbase VLAN tag member for Port 0' */
#define TANTOS_3G_P6DVID_PVTAGMP_OFFSET                   0xC4
#define TANTOS_3G_P6DVID_PVTAGMP_SHIFT                    12
#define TANTOS_3G_P6DVID_PVTAGMP_SIZE                     1
/* Bit: 'PVID' */
/* Description: 'PVID' */
#define TANTOS_3G_P6DVID_PVID_OFFSET                      0xC4
#define TANTOS_3G_P6DVID_PVID_SHIFT                       0
#define TANTOS_3G_P6DVID_PVID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P0ECSQ3_P0SPQ3TR_OFFSET                 0x05
#define TANTOS_3G_P0ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P0ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P1ECSQ3_P0SPQ3TR_OFFSET                 0x25
#define TANTOS_3G_P1ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P1ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P2ECSQ3_P0SPQ3TR_OFFSET                 0x45
#define TANTOS_3G_P2ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P2ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P3ECSQ3_P0SPQ3TR_OFFSET                 0x65
#define TANTOS_3G_P3ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P4ECSQ3_P0SPQ3TR_OFFSET                 0x85
#define TANTOS_3G_P4ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P5ECSQ3_P0SPQ3TR_OFFSET                 0xA5
#define TANTOS_3G_P5ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for Strict Q3 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P6ECSQ3_P0SPQ3TR_OFFSET                 0xC5
#define TANTOS_3G_P6ECSQ3_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECSQ3_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P0ECSQ2_P0SPQ3TR_OFFSET                 0x06
#define TANTOS_3G_P0ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P0ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P1ECSQ2_P0SPQ3TR_OFFSET                 0x26
#define TANTOS_3G_P1ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P1ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P2ECSQ2_P0SPQ3TR_OFFSET                 0x46
#define TANTOS_3G_P2ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P2ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P3ECSQ2_P0SPQ3TR_OFFSET                 0x66
#define TANTOS_3G_P3ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P4ECSQ2_P0SPQ3TR_OFFSET                 0x86
#define TANTOS_3G_P4ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P5ECSQ2_P0SPQ3TR_OFFSET                 0xA6
#define TANTOS_3G_P5ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for Strict Q2 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P6ECSQ2_P0SPQ3TR_OFFSET                 0xC6
#define TANTOS_3G_P6ECSQ2_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECSQ2_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P0ECSQ1_P0SPQ3TR_OFFSET                 0x07
#define TANTOS_3G_P0ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P0ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P1ECSQ1_P0SPQ3TR_OFFSET                 0x27
#define TANTOS_3G_P1ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P1ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P2ECSQ1_P0SPQ3TR_OFFSET                 0x47
#define TANTOS_3G_P2ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P2ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P3ECSQ1_P0SPQ3TR_OFFSET                 0x67
#define TANTOS_3G_P3ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P4ECSQ1_P0SPQ3TR_OFFSET                 0x87
#define TANTOS_3G_P4ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P5ECSQ1_P0SPQ3TR_OFFSET                 0xA7
#define TANTOS_3G_P5ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for Strict Q1 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P6ECSQ1_P0SPQ3TR_OFFSET                 0xC7
#define TANTOS_3G_P6ECSQ1_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECSQ1_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P0ECSQ0_P0SPQ3TR_OFFSET                 0x08
#define TANTOS_3G_P0ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P0ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P1ECSQ0_P0SPQ3TR_OFFSET                 0x28
#define TANTOS_3G_P1ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P1ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P2ECSQ0_P0SPQ3TR_OFFSET                 0x48
#define TANTOS_3G_P2ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P2ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P3ECSQ0_P0SPQ3TR_OFFSET                 0x68
#define TANTOS_3G_P3ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P4ECSQ0_P0SPQ3TR_OFFSET                 0x88
#define TANTOS_3G_P4ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P5ECSQ0_P0SPQ3TR_OFFSET                 0xA8
#define TANTOS_3G_P5ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for Strict Q0 Register' */
/* Bit: 'P0SPQ3TR' */
/* Description: 'Port 0 Egress Token R for Strict Priority Q3' */
#define TANTOS_3G_P6ECSQ0_P0SPQ3TR_OFFSET                 0xC8
#define TANTOS_3G_P6ECSQ0_P0SPQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECSQ0_P0SPQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for WFQ Q3 Register' */
/* Bit: 'P0WQ3TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P0ECWQ3_P0WQ3TR_OFFSET                  0x09
#define TANTOS_3G_P0ECWQ3_P0WQ3TR_SHIFT                   0
#define TANTOS_3G_P0ECWQ3_P0WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for WFQ Q3 Register' */
/* Bit: 'P1WQ3TR' */
/* Description: 'Port 1 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P1ECWQ3_P1WQ3TR_OFFSET                  0x29
#define TANTOS_3G_P1ECWQ3_P1WQ3TR_SHIFT                   0
#define TANTOS_3G_P1ECWQ3_P1WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for WFQ Q3 Register' */
/* Bit: 'P2WQ3TR' */
/* Description: 'Port 2 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P2ECWQ3_P2WQ3TR_OFFSET                  0x49
#define TANTOS_3G_P2ECWQ3_P2WQ3TR_SHIFT                   0
#define TANTOS_3G_P2ECWQ3_P2WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for WFQ Q3 Register' */
/* Bit: 'P3WQ3TR' */
/* Description: 'Port 3 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P3ECWQ3_P3WQ3TR_OFFSET                  0x69
#define TANTOS_3G_P3ECWQ3_P3WQ3TR_SHIFT                   0
#define TANTOS_3G_P3ECWQ3_P3WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for WFQ Q3 Register' */
/* Bit: 'P4WQ3TR' */
/* Description: 'Port 4 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P4ECWQ3_P4WQ3TR_OFFSET                  0x89
#define TANTOS_3G_P4ECWQ3_P4WQ3TR_SHIFT                   0
#define TANTOS_3G_P4ECWQ3_P4WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for WFQ Q3 Register' */
/* Bit: 'P5WQ3TR' */
/* Description: 'Port 5 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P5ECWQ3_P5WQ3TR_OFFSET                  0xA9
#define TANTOS_3G_P5ECWQ3_P5WQ3TR_SHIFT                   0
#define TANTOS_3G_P5ECWQ3_P5WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for WFQ Q3 Register' */
/* Bit: 'P6WQ3TR' */
/* Description: 'Port 6 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P6ECWQ3_P6WQ3TR_OFFSET                  0xC9
#define TANTOS_3G_P6ECWQ3_P6WQ3TR_SHIFT                   0
#define TANTOS_3G_P6ECWQ3_P6WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for WFQ Q2 Register' */
/* Bit: 'P0WQ3TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P0ECWQ2_P0WQ3TR_OFFSET                  0x0A
#define TANTOS_3G_P0ECWQ2_P0WQ3TR_SHIFT                   0
#define TANTOS_3G_P0ECWQ2_P0WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for WFQ Q2 Register' */
/* Bit: 'P8WQ3TR' */
/* Description: 'Port 8 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P1ECWQ2_P8WQ3TR_OFFSET                  0x2A
#define TANTOS_3G_P1ECWQ2_P8WQ3TR_SHIFT                   0
#define TANTOS_3G_P1ECWQ2_P8WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for WFQ Q2 Register' */
/* Bit: 'P9WQ3TR' */
/* Description: 'Port 9 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P2ECWQ2_P9WQ3TR_OFFSET                  0x4A
#define TANTOS_3G_P2ECWQ2_P9WQ3TR_SHIFT                   0
#define TANTOS_3G_P2ECWQ2_P9WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for WFQ Q2 Register' */
/* Bit: 'P10WQ3TR' */
/* Description: 'Port 10 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P3ECWQ2_P10WQ3TR_OFFSET                 0x6A
#define TANTOS_3G_P3ECWQ2_P10WQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECWQ2_P10WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for WFQ Q2 Register' */
/* Bit: 'P11WQ3TR' */
/* Description: 'Port 11 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P4ECWQ2_P11WQ3TR_OFFSET                 0x8A
#define TANTOS_3G_P4ECWQ2_P11WQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECWQ2_P11WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for WFQ Q2 Register' */
/* Bit: 'P12WQ3TR' */
/* Description: 'Port 12 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P5ECWQ2_P12WQ3TR_OFFSET                 0xAA
#define TANTOS_3G_P5ECWQ2_P12WQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECWQ2_P12WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for WFQ Q2 Register' */
/* Bit: 'P13WQ3TR' */
/* Description: 'Port 13 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P6ECWQ2_P13WQ3TR_OFFSET                 0xCA
#define TANTOS_3G_P6ECWQ2_P13WQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECWQ2_P13WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for WFQ Q1 Register' */
/* Bit: 'P0WQ3TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P0ECWQ1_P0WQ3TR_OFFSET                  0x0B
#define TANTOS_3G_P0ECWQ1_P0WQ3TR_SHIFT                   0
#define TANTOS_3G_P0ECWQ1_P0WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for WFQ Q1 Register' */
/* Bit: 'P15WQ3TR' */
/* Description: 'Port 15 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P1ECWQ1_P15WQ3TR_OFFSET                 0x2B
#define TANTOS_3G_P1ECWQ1_P15WQ3TR_SHIFT                  0
#define TANTOS_3G_P1ECWQ1_P15WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for WFQ Q1 Register' */
/* Bit: 'P16WQ3TR' */
/* Description: 'Port 16 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P2ECWQ1_P16WQ3TR_OFFSET                 0x4B
#define TANTOS_3G_P2ECWQ1_P16WQ3TR_SHIFT                  0
#define TANTOS_3G_P2ECWQ1_P16WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for WFQ Q1 Register' */
/* Bit: 'P17WQ3TR' */
/* Description: 'Port 17 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P3ECWQ1_P17WQ3TR_OFFSET                 0x6B
#define TANTOS_3G_P3ECWQ1_P17WQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECWQ1_P17WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for WFQ Q1 Register' */
/* Bit: 'P18WQ3TR' */
/* Description: 'Port 18 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P4ECWQ1_P18WQ3TR_OFFSET                 0x8B
#define TANTOS_3G_P4ECWQ1_P18WQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECWQ1_P18WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for WFQ Q1 Register' */
/* Bit: 'P19WQ3TR' */
/* Description: 'Port 19 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P5ECWQ1_P19WQ3TR_OFFSET                 0xAB
#define TANTOS_3G_P5ECWQ1_P19WQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECWQ1_P19WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for WFQ Q1 Register' */
/* Bit: 'P20WQ3TR' */
/* Description: 'Port 20 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P6ECWQ1_P20WQ3TR_OFFSET                 0xCB
#define TANTOS_3G_P6ECWQ1_P20WQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECWQ1_P20WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Egress Control for WFQ Q0 Register' */
/* Bit: 'P0WQ3TR' */
/* Description: 'Port 0 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P0ECWQ0_P0WQ3TR_OFFSET                  0x0C
#define TANTOS_3G_P0ECWQ0_P0WQ3TR_SHIFT                   0
#define TANTOS_3G_P0ECWQ0_P0WQ3TR_SIZE                    11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Egress Control for WFQ Q0 Register' */
/* Bit: 'P22WQ3TR' */
/* Description: 'Port 22 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P1ECWQ0_P22WQ3TR_OFFSET                 0x2C
#define TANTOS_3G_P1ECWQ0_P22WQ3TR_SHIFT                  0
#define TANTOS_3G_P1ECWQ0_P22WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Egress Control for WFQ Q0 Register' */
/* Bit: 'P23WQ3TR' */
/* Description: 'Port 23 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P2ECWQ0_P23WQ3TR_OFFSET                 0x4C
#define TANTOS_3G_P2ECWQ0_P23WQ3TR_SHIFT                  0
#define TANTOS_3G_P2ECWQ0_P23WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Egress Control for WFQ Q0 Register' */
/* Bit: 'P24WQ3TR' */
/* Description: 'Port 24 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P3ECWQ0_P24WQ3TR_OFFSET                 0x6C
#define TANTOS_3G_P3ECWQ0_P24WQ3TR_SHIFT                  0
#define TANTOS_3G_P3ECWQ0_P24WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Egress Control for WFQ Q0 Register' */
/* Bit: 'P25WQ3TR' */
/* Description: 'Port 25 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P4ECWQ0_P25WQ3TR_OFFSET                 0x8C
#define TANTOS_3G_P4ECWQ0_P25WQ3TR_SHIFT                  0
#define TANTOS_3G_P4ECWQ0_P25WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Egress Control for WFQ Q0 Register' */
/* Bit: 'P26WQ3TR' */
/* Description: 'Port 26 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P5ECWQ0_P26WQ3TR_OFFSET                 0xAC
#define TANTOS_3G_P5ECWQ0_P26WQ3TR_SHIFT                  0
#define TANTOS_3G_P5ECWQ0_P26WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Egress Control for WFQ Q0 Register' */
/* Bit: 'P27WQ3TR' */
/* Description: 'Port 27 Egress Token R for WFQ Q3' */
#define TANTOS_3G_P6ECWQ0_P27WQ3TR_OFFSET                 0xCC
#define TANTOS_3G_P6ECWQ0_P27WQ3TR_SHIFT                  0
#define TANTOS_3G_P6ECWQ0_P27WQ3TR_SIZE                   11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 0 Ingress Control Register' */
/* Bit: 'P0ITT' */
/* Description: 'Port 0 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P0ICR_P0ITT_OFFSET                      0x0D
#define TANTOS_3G_P0ICR_P0ITT_SHIFT                       11
#define TANTOS_3G_P0ICR_P0ITT_SIZE                        2
/* Bit: 'P0ITR' */
/* Description: 'Port 0 Ingress Token R' */
#define TANTOS_3G_P0ICR_P0ITR_OFFSET                      0x0D
#define TANTOS_3G_P0ICR_P0ITR_SHIFT                       0
#define TANTOS_3G_P0ICR_P0ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 1 Ingress Control Register' */
/* Bit: 'P1ITT' */
/* Description: 'Port 1 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P1ICR_P1ITT_OFFSET                      0x2D
#define TANTOS_3G_P1ICR_P1ITT_SHIFT                       11
#define TANTOS_3G_P1ICR_P1ITT_SIZE                        2
/* Bit: 'P1ITR' */
/* Description: 'Port 1 Ingress Token R' */
#define TANTOS_3G_P1ICR_P1ITR_OFFSET                      0x2D
#define TANTOS_3G_P1ICR_P1ITR_SHIFT                       0
#define TANTOS_3G_P1ICR_P1ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 2 Ingress Control Register' */
/* Bit: 'P2ITT' */
/* Description: 'Port 2 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P2ICR_P2ITT_OFFSET                      0x4D
#define TANTOS_3G_P2ICR_P2ITT_SHIFT                       11
#define TANTOS_3G_P2ICR_P2ITT_SIZE                        2
/* Bit: 'P2ITR' */
/* Description: 'Port 2 Ingress Token R' */
#define TANTOS_3G_P2ICR_P2ITR_OFFSET                      0x4D
#define TANTOS_3G_P2ICR_P2ITR_SHIFT                       0
#define TANTOS_3G_P2ICR_P2ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 3 Ingress Control Register' */
/* Bit: 'P3ITT' */
/* Description: 'Port 3 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P3ICR_P3ITT_OFFSET                      0x6D
#define TANTOS_3G_P3ICR_P3ITT_SHIFT                       11
#define TANTOS_3G_P3ICR_P3ITT_SIZE                        2
/* Bit: 'P3ITR' */
/* Description: 'Port 3 Ingress Token R' */
#define TANTOS_3G_P3ICR_P3ITR_OFFSET                      0x6D
#define TANTOS_3G_P3ICR_P3ITR_SHIFT                       0
#define TANTOS_3G_P3ICR_P3ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 4 Ingress Control Register' */
/* Bit: 'P4ITT' */
/* Description: 'Port 4 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P4ICR_P4ITT_OFFSET                      0x8D
#define TANTOS_3G_P4ICR_P4ITT_SHIFT                       11
#define TANTOS_3G_P4ICR_P4ITT_SIZE                        2
/* Bit: 'P4ITR' */
/* Description: 'Port 4 Ingress Token R' */
#define TANTOS_3G_P4ICR_P4ITR_OFFSET                      0x8D
#define TANTOS_3G_P4ICR_P4ITR_SHIFT                       0
#define TANTOS_3G_P4ICR_P4ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 5 Ingress Control Register' */
/* Bit: 'P5ITT' */
/* Description: 'Port 5 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P5ICR_P5ITT_OFFSET                      0xAD
#define TANTOS_3G_P5ICR_P5ITT_SHIFT                       11
#define TANTOS_3G_P5ICR_P5ITT_SIZE                        2
/* Bit: 'P5ITR' */
/* Description: 'Port 5 Ingress Token R' */
#define TANTOS_3G_P5ICR_P5ITR_OFFSET                      0xAD
#define TANTOS_3G_P5ICR_P5ITR_SHIFT                       0
#define TANTOS_3G_P5ICR_P5ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Port 6 Ingress Control Register' */
/* Bit: 'P6ITT' */
/* Description: 'Port 6 Ingress/Egress Timer Tick T selection' */
#define TANTOS_3G_P6ICR_P6ITT_OFFSET                      0xCD
#define TANTOS_3G_P6ICR_P6ITT_SHIFT                       11
#define TANTOS_3G_P6ICR_P6ITT_SIZE                        2
/* Bit: 'P6ITR' */
/* Description: 'Port 6 Ingress Token R' */
#define TANTOS_3G_P6ICR_P6ITR_OFFSET                      0xCD
#define TANTOS_3G_P6ICR_P6ITR_SHIFT                       0
#define TANTOS_3G_P6ICR_P6ITR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 0 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF0L_VV_OFFSET                          0x10
#define TANTOS_3G_VF0L_VV_SHIFT                           15
#define TANTOS_3G_VF0L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF0L_VP_OFFSET                          0x10
#define TANTOS_3G_VF0L_VP_SHIFT                           12
#define TANTOS_3G_VF0L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF0L_VID_OFFSET                         0x10
#define TANTOS_3G_VF0L_VID_SHIFT                          0
#define TANTOS_3G_VF0L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 1 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF1L_VV_OFFSET                          0x12
#define TANTOS_3G_VF1L_VV_SHIFT                           15
#define TANTOS_3G_VF1L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF1L_VP_OFFSET                          0x12
#define TANTOS_3G_VF1L_VP_SHIFT                           12
#define TANTOS_3G_VF1L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF1L_VID_OFFSET                         0x12
#define TANTOS_3G_VF1L_VID_SHIFT                          0
#define TANTOS_3G_VF1L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 2 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF2L_VV_OFFSET                          0x14
#define TANTOS_3G_VF2L_VV_SHIFT                           15
#define TANTOS_3G_VF2L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF2L_VP_OFFSET                          0x14
#define TANTOS_3G_VF2L_VP_SHIFT                           12
#define TANTOS_3G_VF2L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF2L_VID_OFFSET                         0x14
#define TANTOS_3G_VF2L_VID_SHIFT                          0
#define TANTOS_3G_VF2L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 3Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF3L_VV_OFFSET                          0x16
#define TANTOS_3G_VF3L_VV_SHIFT                           15
#define TANTOS_3G_VF3L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF3L_VP_OFFSET                          0x16
#define TANTOS_3G_VF3L_VP_SHIFT                           12
#define TANTOS_3G_VF3L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF3L_VID_OFFSET                         0x16
#define TANTOS_3G_VF3L_VID_SHIFT                          0
#define TANTOS_3G_VF3L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 4 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF4L_VV_OFFSET                          0x18
#define TANTOS_3G_VF4L_VV_SHIFT                           15
#define TANTOS_3G_VF4L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF4L_VP_OFFSET                          0x18
#define TANTOS_3G_VF4L_VP_SHIFT                           12
#define TANTOS_3G_VF4L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF4L_VID_OFFSET                         0x18
#define TANTOS_3G_VF4L_VID_SHIFT                          0
#define TANTOS_3G_VF4L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 5 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF5L_VV_OFFSET                          0x1A
#define TANTOS_3G_VF5L_VV_SHIFT                           15
#define TANTOS_3G_VF5L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF5L_VP_OFFSET                          0x1A
#define TANTOS_3G_VF5L_VP_SHIFT                           12
#define TANTOS_3G_VF5L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF5L_VID_OFFSET                         0x1A
#define TANTOS_3G_VF5L_VID_SHIFT                          0
#define TANTOS_3G_VF5L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 6 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF6L_VV_OFFSET                          0x1C
#define TANTOS_3G_VF6L_VV_SHIFT                           15
#define TANTOS_3G_VF6L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF6L_VP_OFFSET                          0x1C
#define TANTOS_3G_VF6L_VP_SHIFT                           12
#define TANTOS_3G_VF6L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF6L_VID_OFFSET                         0x1C
#define TANTOS_3G_VF6L_VID_SHIFT                          0
#define TANTOS_3G_VF6L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 7 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF7L_VV_OFFSET                          0x1E
#define TANTOS_3G_VF7L_VV_SHIFT                           15
#define TANTOS_3G_VF7L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF7L_VP_OFFSET                          0x1E
#define TANTOS_3G_VF7L_VP_SHIFT                           12
#define TANTOS_3G_VF7L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF7L_VID_OFFSET                         0x1E
#define TANTOS_3G_VF7L_VID_SHIFT                          0
#define TANTOS_3G_VF7L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 8 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF8L_VV_OFFSET                          0x30
#define TANTOS_3G_VF8L_VV_SHIFT                           15
#define TANTOS_3G_VF8L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF8L_VP_OFFSET                          0x30
#define TANTOS_3G_VF8L_VP_SHIFT                           12
#define TANTOS_3G_VF8L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF8L_VID_OFFSET                         0x30
#define TANTOS_3G_VF8L_VID_SHIFT                          0
#define TANTOS_3G_VF8L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 9 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF9L_VV_OFFSET                          0x32
#define TANTOS_3G_VF9L_VV_SHIFT                           15
#define TANTOS_3G_VF9L_VV_SIZE                            1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF9L_VP_OFFSET                          0x32
#define TANTOS_3G_VF9L_VP_SHIFT                           12
#define TANTOS_3G_VF9L_VP_SIZE                            3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF9L_VID_OFFSET                         0x32
#define TANTOS_3G_VF9L_VID_SHIFT                          0
#define TANTOS_3G_VF9L_VID_SIZE                           12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 10 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF10L_VV_OFFSET                         0x34
#define TANTOS_3G_VF10L_VV_SHIFT                          15
#define TANTOS_3G_VF10L_VV_SIZE                           1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF10L_VP_OFFSET                         0x34
#define TANTOS_3G_VF10L_VP_SHIFT                          12
#define TANTOS_3G_VF10L_VP_SIZE                           3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF10L_VID_OFFSET                        0x34
#define TANTOS_3G_VF10L_VID_SHIFT                         0
#define TANTOS_3G_VF10L_VID_SIZE                          12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 11 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF11L_VV_OFFSET                         0x36
#define TANTOS_3G_VF11L_VV_SHIFT                          15
#define TANTOS_3G_VF11L_VV_SIZE                           1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF11L_VP_OFFSET                         0x36
#define TANTOS_3G_VF11L_VP_SHIFT                          12
#define TANTOS_3G_VF11L_VP_SIZE                           3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF11L_VID_OFFSET                        0x36
#define TANTOS_3G_VF11L_VID_SHIFT                         0
#define TANTOS_3G_VF11L_VID_SIZE                          12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 12 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF12L_VV_OFFSET                         0x38
#define TANTOS_3G_VF12L_VV_SHIFT                          15
#define TANTOS_3G_VF12L_VV_SIZE                           1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF12L_VP_OFFSET                         0x38
#define TANTOS_3G_VF12L_VP_SHIFT                          12
#define TANTOS_3G_VF12L_VP_SIZE                           3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF12L_VID_OFFSET                        0x38
#define TANTOS_3G_VF12L_VID_SHIFT                         0
#define TANTOS_3G_VF12L_VID_SIZE                          12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 13 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF13L_VV_OFFSET                         0x3A
#define TANTOS_3G_VF13L_VV_SHIFT                          15
#define TANTOS_3G_VF13L_VV_SIZE                           1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF13L_VP_OFFSET                         0x3A
#define TANTOS_3G_VF13L_VP_SHIFT                          12
#define TANTOS_3G_VF13L_VP_SIZE                           3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF13L_VID_OFFSET                        0x3A
#define TANTOS_3G_VF13L_VID_SHIFT                         0
#define TANTOS_3G_VF13L_VID_SIZE                          12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 14 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF14L_VV_OFFSET                         0x3C
#define TANTOS_3G_VF14L_VV_SHIFT                          15
#define TANTOS_3G_VF14L_VV_SIZE                           1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF14L_VP_OFFSET                         0x3C
#define TANTOS_3G_VF14L_VP_SHIFT                          12
#define TANTOS_3G_VF14L_VP_SIZE                           3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF14L_VID_OFFSET                        0x3C
#define TANTOS_3G_VF14L_VID_SHIFT                         0
#define TANTOS_3G_VF14L_VID_SIZE                          12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 15 Low' */
/* Bit: 'VV' */
/* Description: 'VLAN_Valid' */
#define TANTOS_3G_VF15L_VV_OFFSET                         0x3E
#define TANTOS_3G_VF15L_VV_SHIFT                          15
#define TANTOS_3G_VF15L_VV_SIZE                           1
/* Bit: 'VP' */
/* Description: 'VLAN PRI' */
#define TANTOS_3G_VF15L_VP_OFFSET                         0x3E
#define TANTOS_3G_VF15L_VP_SHIFT                          12
#define TANTOS_3G_VF15L_VP_SIZE                           3
/* Bit: 'VID' */
/* Description: 'VID' */
#define TANTOS_3G_VF15L_VID_OFFSET                        0x3E
#define TANTOS_3G_VF15L_VID_SHIFT                         0
#define TANTOS_3G_VF15L_VID_SIZE                          12
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 0 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF0H_FID_OFFSET                         0x11
#define TANTOS_3G_VF0H_FID_SHIFT                          14
#define TANTOS_3G_VF0H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF0H_TM_OFFSET                          0x11
#define TANTOS_3G_VF0H_TM_SHIFT                           7
#define TANTOS_3G_VF0H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF0H_M_OFFSET                           0x11
#define TANTOS_3G_VF0H_M_SHIFT                            0
#define TANTOS_3G_VF0H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 1 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF1H_FID_OFFSET                         0x13
#define TANTOS_3G_VF1H_FID_SHIFT                          14
#define TANTOS_3G_VF1H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF1H_TM_OFFSET                          0x13
#define TANTOS_3G_VF1H_TM_SHIFT                           7
#define TANTOS_3G_VF1H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF1H_M_OFFSET                           0x13
#define TANTOS_3G_VF1H_M_SHIFT                            0
#define TANTOS_3G_VF1H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 2 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF2H_FID_OFFSET                         0x15
#define TANTOS_3G_VF2H_FID_SHIFT                          14
#define TANTOS_3G_VF2H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF2H_TM_OFFSET                          0x15
#define TANTOS_3G_VF2H_TM_SHIFT                           7
#define TANTOS_3G_VF2H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF2H_M_OFFSET                           0x15
#define TANTOS_3G_VF2H_M_SHIFT                            0
#define TANTOS_3G_VF2H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 3 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF3H_FID_OFFSET                         0x17
#define TANTOS_3G_VF3H_FID_SHIFT                          14
#define TANTOS_3G_VF3H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF3H_TM_OFFSET                          0x17
#define TANTOS_3G_VF3H_TM_SHIFT                           7
#define TANTOS_3G_VF3H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF3H_M_OFFSET                           0x17
#define TANTOS_3G_VF3H_M_SHIFT                            0
#define TANTOS_3G_VF3H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 4 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF4H_FID_OFFSET                         0x19
#define TANTOS_3G_VF4H_FID_SHIFT                          14
#define TANTOS_3G_VF4H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF4H_TM_OFFSET                          0x19
#define TANTOS_3G_VF4H_TM_SHIFT                           7
#define TANTOS_3G_VF4H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF4H_M_OFFSET                           0x19
#define TANTOS_3G_VF4H_M_SHIFT                            0
#define TANTOS_3G_VF4H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 5 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF5H_FID_OFFSET                         0x1B
#define TANTOS_3G_VF5H_FID_SHIFT                          14
#define TANTOS_3G_VF5H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF5H_TM_OFFSET                          0x1B
#define TANTOS_3G_VF5H_TM_SHIFT                           7
#define TANTOS_3G_VF5H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF5H_M_OFFSET                           0x1B
#define TANTOS_3G_VF5H_M_SHIFT                            0
#define TANTOS_3G_VF5H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 6 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF6H_FID_OFFSET                         0x1D
#define TANTOS_3G_VF6H_FID_SHIFT                          14
#define TANTOS_3G_VF6H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF6H_TM_OFFSET                          0x1D
#define TANTOS_3G_VF6H_TM_SHIFT                           7
#define TANTOS_3G_VF6H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF6H_M_OFFSET                           0x1D
#define TANTOS_3G_VF6H_M_SHIFT                            0
#define TANTOS_3G_VF6H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 7 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF7H_FID_OFFSET                         0x1F
#define TANTOS_3G_VF7H_FID_SHIFT                          14
#define TANTOS_3G_VF7H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF7H_TM_OFFSET                          0x1F
#define TANTOS_3G_VF7H_TM_SHIFT                           7
#define TANTOS_3G_VF7H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF7H_M_OFFSET                           0x1F
#define TANTOS_3G_VF7H_M_SHIFT                            0
#define TANTOS_3G_VF7H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 8 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF8H_FID_OFFSET                         0x31
#define TANTOS_3G_VF8H_FID_SHIFT                          14
#define TANTOS_3G_VF8H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF8H_TM_OFFSET                          0x31
#define TANTOS_3G_VF8H_TM_SHIFT                           7
#define TANTOS_3G_VF8H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF8H_M_OFFSET                           0x31
#define TANTOS_3G_VF8H_M_SHIFT                            0
#define TANTOS_3G_VF8H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 9 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF9H_FID_OFFSET                         0x33
#define TANTOS_3G_VF9H_FID_SHIFT                          14
#define TANTOS_3G_VF9H_FID_SIZE                           2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF9H_TM_OFFSET                          0x33
#define TANTOS_3G_VF9H_TM_SHIFT                           7
#define TANTOS_3G_VF9H_TM_SIZE                            7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF9H_M_OFFSET                           0x33
#define TANTOS_3G_VF9H_M_SHIFT                            0
#define TANTOS_3G_VF9H_M_SIZE                             7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 10 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF10H_FID_OFFSET                        0x35
#define TANTOS_3G_VF10H_FID_SHIFT                         14
#define TANTOS_3G_VF10H_FID_SIZE                          2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF10H_TM_OFFSET                         0x35
#define TANTOS_3G_VF10H_TM_SHIFT                          7
#define TANTOS_3G_VF10H_TM_SIZE                           7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF10H_M_OFFSET                          0x35
#define TANTOS_3G_VF10H_M_SHIFT                           0
#define TANTOS_3G_VF10H_M_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 11 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF11H_FID_OFFSET                        0x37
#define TANTOS_3G_VF11H_FID_SHIFT                         14
#define TANTOS_3G_VF11H_FID_SIZE                          2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF11H_TM_OFFSET                         0x37
#define TANTOS_3G_VF11H_TM_SHIFT                          7
#define TANTOS_3G_VF11H_TM_SIZE                           7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF11H_M_OFFSET                          0x37
#define TANTOS_3G_VF11H_M_SHIFT                           0
#define TANTOS_3G_VF11H_M_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 12 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF12H_FID_OFFSET                        0x39
#define TANTOS_3G_VF12H_FID_SHIFT                         14
#define TANTOS_3G_VF12H_FID_SIZE                          2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF12H_TM_OFFSET                         0x39
#define TANTOS_3G_VF12H_TM_SHIFT                          7
#define TANTOS_3G_VF12H_TM_SIZE                           7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF12H_M_OFFSET                          0x39
#define TANTOS_3G_VF12H_M_SHIFT                           0
#define TANTOS_3G_VF12H_M_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 13 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF13H_FID_OFFSET                        0x3B
#define TANTOS_3G_VF13H_FID_SHIFT                         14
#define TANTOS_3G_VF13H_FID_SIZE                          2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF13H_TM_OFFSET                         0x3B
#define TANTOS_3G_VF13H_TM_SHIFT                          7
#define TANTOS_3G_VF13H_TM_SIZE                           7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF13H_M_OFFSET                          0x3B
#define TANTOS_3G_VF13H_M_SHIFT                           0
#define TANTOS_3G_VF13H_M_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 14 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF14H_FID_OFFSET                        0x3D
#define TANTOS_3G_VF14H_FID_SHIFT                         14
#define TANTOS_3G_VF14H_FID_SIZE                          2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF14H_TM_OFFSET                         0x3D
#define TANTOS_3G_VF14H_TM_SHIFT                          7
#define TANTOS_3G_VF14H_TM_SIZE                           7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF14H_M_OFFSET                          0x3D
#define TANTOS_3G_VF14H_M_SHIFT                           0
#define TANTOS_3G_VF14H_M_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Filter 15 High' */
/* Bit: 'FID' */
/* Description: 'FID' */
#define TANTOS_3G_VF15H_FID_OFFSET                        0x3F
#define TANTOS_3G_VF15H_FID_SHIFT                         14
#define TANTOS_3G_VF15H_FID_SIZE                          2
/* Bit: 'TM' */
/* Description: 'Tagged Member' */
#define TANTOS_3G_VF15H_TM_OFFSET                         0x3F
#define TANTOS_3G_VF15H_TM_SHIFT                          7
#define TANTOS_3G_VF15H_TM_SIZE                           7
/* Bit: 'M' */
/* Description: 'Member' */
#define TANTOS_3G_VF15H_M_OFFSET                          0x3F
#define TANTOS_3G_VF15H_M_SHIFT                           0
#define TANTOS_3G_VF15H_M_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 0' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF0_VCET_OFFSET                         0x50
#define TANTOS_3G_TF0_VCET_SHIFT                          0
#define TANTOS_3G_TF0_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 1' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF1_VCET_OFFSET                         0x51
#define TANTOS_3G_TF1_VCET_SHIFT                          0
#define TANTOS_3G_TF1_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 2' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF2_VCET_OFFSET                         0x52
#define TANTOS_3G_TF2_VCET_SHIFT                          0
#define TANTOS_3G_TF2_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 3' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF3_VCET_OFFSET                         0x53
#define TANTOS_3G_TF3_VCET_SHIFT                          0
#define TANTOS_3G_TF3_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 4' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF4_VCET_OFFSET                         0x54
#define TANTOS_3G_TF4_VCET_SHIFT                          0
#define TANTOS_3G_TF4_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 5' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF5_VCET_OFFSET                         0x55
#define TANTOS_3G_TF5_VCET_SHIFT                          0
#define TANTOS_3G_TF5_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 6' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF6_VCET_OFFSET                         0x56
#define TANTOS_3G_TF6_VCET_SHIFT                          0
#define TANTOS_3G_TF6_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter 7' */
/* Bit: 'VCET' */
/* Description: 'Value Compared with Ether-Type' */
#define TANTOS_3G_TF7_VCET_OFFSET                         0x57
#define TANTOS_3G_TF7_VCET_SHIFT                          0
#define TANTOS_3G_TF7_VCET_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServ Mapping 0' */
/* Bit: 'PQ7' */
/* Description: 'Priority Queue 7' */
#define TANTOS_3G_DM0_PQ7_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ7_SHIFT                           14
#define TANTOS_3G_DM0_PQ7_SIZE                            2
/* Bit: 'PQ6' */
/* Description: 'Priority Queue 6' */
#define TANTOS_3G_DM0_PQ6_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ6_SHIFT                           12
#define TANTOS_3G_DM0_PQ6_SIZE                            2
/* Bit: 'PQ5' */
/* Description: 'Priority Queue 5' */
#define TANTOS_3G_DM0_PQ5_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ5_SHIFT                           10
#define TANTOS_3G_DM0_PQ5_SIZE                            2
/* Bit: 'PQ4' */
/* Description: 'Priority Queue 4' */
#define TANTOS_3G_DM0_PQ4_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ4_SHIFT                           8
#define TANTOS_3G_DM0_PQ4_SIZE                            2
/* Bit: 'PQ3' */
/* Description: 'Priority Queue 3' */
#define TANTOS_3G_DM0_PQ3_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ3_SHIFT                           6
#define TANTOS_3G_DM0_PQ3_SIZE                            2
/* Bit: 'PQ2' */
/* Description: 'Priority Queue 2' */
#define TANTOS_3G_DM0_PQ2_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ2_SHIFT                           4
#define TANTOS_3G_DM0_PQ2_SIZE                            2
/* Bit: 'PQ1' */
/* Description: 'Priority Queue 1' */
#define TANTOS_3G_DM0_PQ1_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ1_SHIFT                           2
#define TANTOS_3G_DM0_PQ1_SIZE                            2
/* Bit: 'PQ0' */
/* Description: 'Priority Queue 0' */
#define TANTOS_3G_DM0_PQ0_OFFSET                          0x58
#define TANTOS_3G_DM0_PQ0_SHIFT                           0
#define TANTOS_3G_DM0_PQ0_SIZE                            2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 1' */
/* Bit: 'PQF' */
/* Description: 'Priority Queue F' */
#define TANTOS_3G_DM1_PQF_OFFSET                          0x59
#define TANTOS_3G_DM1_PQF_SHIFT                           14
#define TANTOS_3G_DM1_PQF_SIZE                            2
/* Bit: 'PQE' */
/* Description: 'Priority Queue E' */
#define TANTOS_3G_DM1_PQE_OFFSET                          0x59
#define TANTOS_3G_DM1_PQE_SHIFT                           12
#define TANTOS_3G_DM1_PQE_SIZE                            2
/* Bit: 'PQD' */
/* Description: 'Priority Queue D' */
#define TANTOS_3G_DM1_PQD_OFFSET                          0x59
#define TANTOS_3G_DM1_PQD_SHIFT                           10
#define TANTOS_3G_DM1_PQD_SIZE                            2
/* Bit: 'PQC' */
/* Description: 'Priority Queue C' */
#define TANTOS_3G_DM1_PQC_OFFSET                          0x59
#define TANTOS_3G_DM1_PQC_SHIFT                           8
#define TANTOS_3G_DM1_PQC_SIZE                            2
/* Bit: 'PQB' */
/* Description: 'Priority Queue B' */
#define TANTOS_3G_DM1_PQB_OFFSET                          0x59
#define TANTOS_3G_DM1_PQB_SHIFT                           6
#define TANTOS_3G_DM1_PQB_SIZE                            2
/* Bit: 'PQA' */
/* Description: 'Priority Queue A' */
#define TANTOS_3G_DM1_PQA_OFFSET                          0x59
#define TANTOS_3G_DM1_PQA_SHIFT                           4
#define TANTOS_3G_DM1_PQA_SIZE                            2
/* Bit: 'PQ9' */
/* Description: 'Priority Queue 9' */
#define TANTOS_3G_DM1_PQ9_OFFSET                          0x59
#define TANTOS_3G_DM1_PQ9_SHIFT                           2
#define TANTOS_3G_DM1_PQ9_SIZE                            2
/* Bit: 'PQ8' */
/* Description: 'Priority Queue 8' */
#define TANTOS_3G_DM1_PQ8_OFFSET                          0x59
#define TANTOS_3G_DM1_PQ8_SHIFT                           0
#define TANTOS_3G_DM1_PQ8_SIZE                            2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 2' */
/* Bit: 'PQ17' */
/* Description: 'Priority Queue 17' */
#define TANTOS_3G_DM2_PQ17_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ17_SHIFT                          14
#define TANTOS_3G_DM2_PQ17_SIZE                           2
/* Bit: 'PQ16' */
/* Description: 'Priority Queue 16' */
#define TANTOS_3G_DM2_PQ16_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ16_SHIFT                          12
#define TANTOS_3G_DM2_PQ16_SIZE                           2
/* Bit: 'PQ15' */
/* Description: 'Priority Queue 15' */
#define TANTOS_3G_DM2_PQ15_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ15_SHIFT                          10
#define TANTOS_3G_DM2_PQ15_SIZE                           2
/* Bit: 'PQ14' */
/* Description: 'Priority Queue 14' */
#define TANTOS_3G_DM2_PQ14_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ14_SHIFT                          8
#define TANTOS_3G_DM2_PQ14_SIZE                           2
/* Bit: 'PQ13' */
/* Description: 'Priority Queue 13' */
#define TANTOS_3G_DM2_PQ13_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ13_SHIFT                          6
#define TANTOS_3G_DM2_PQ13_SIZE                           2
/* Bit: 'PQ12' */
/* Description: 'Priority Queue 12' */
#define TANTOS_3G_DM2_PQ12_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ12_SHIFT                          4
#define TANTOS_3G_DM2_PQ12_SIZE                           2
/* Bit: 'PQ11' */
/* Description: 'Priority Queue 11' */
#define TANTOS_3G_DM2_PQ11_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ11_SHIFT                          2
#define TANTOS_3G_DM2_PQ11_SIZE                           2
/* Bit: 'PQ10' */
/* Description: 'Priority Queue 10' */
#define TANTOS_3G_DM2_PQ10_OFFSET                         0x5A
#define TANTOS_3G_DM2_PQ10_SHIFT                          0
#define TANTOS_3G_DM2_PQ10_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 3' */
/* Bit: 'PQ1F' */
/* Description: 'Priority Queue 1F' */
#define TANTOS_3G_DM3_PQ1F_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ1F_SHIFT                          14
#define TANTOS_3G_DM3_PQ1F_SIZE                           2
/* Bit: 'PQ1E' */
/* Description: 'Priority Queue 1E' */
#define TANTOS_3G_DM3_PQ1E_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ1E_SHIFT                          12
#define TANTOS_3G_DM3_PQ1E_SIZE                           2
/* Bit: 'PQ1D' */
/* Description: 'Priority Queue 1D' */
#define TANTOS_3G_DM3_PQ1D_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ1D_SHIFT                          10
#define TANTOS_3G_DM3_PQ1D_SIZE                           2
/* Bit: 'PQ1C' */
/* Description: 'Priority Queue 1C' */
#define TANTOS_3G_DM3_PQ1C_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ1C_SHIFT                          8
#define TANTOS_3G_DM3_PQ1C_SIZE                           2
/* Bit: 'PQ1B' */
/* Description: 'Priority Queue 1B' */
#define TANTOS_3G_DM3_PQ1B_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ1B_SHIFT                          6
#define TANTOS_3G_DM3_PQ1B_SIZE                           2
/* Bit: 'PQ1A' */
/* Description: 'Priority Queue 1A' */
#define TANTOS_3G_DM3_PQ1A_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ1A_SHIFT                          4
#define TANTOS_3G_DM3_PQ1A_SIZE                           2
/* Bit: 'PQ19' */
/* Description: 'Priority Queue 19' */
#define TANTOS_3G_DM3_PQ19_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ19_SHIFT                          2
#define TANTOS_3G_DM3_PQ19_SIZE                           2
/* Bit: 'PQ18' */
/* Description: 'Priority Queue 18' */
#define TANTOS_3G_DM3_PQ18_OFFSET                         0x5B
#define TANTOS_3G_DM3_PQ18_SHIFT                          0
#define TANTOS_3G_DM3_PQ18_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 4' */
/* Bit: 'PQ27' */
/* Description: 'Priority Queue 27' */
#define TANTOS_3G_DM4_PQ27_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ27_SHIFT                          14
#define TANTOS_3G_DM4_PQ27_SIZE                           2
/* Bit: 'PQ26' */
/* Description: 'Priority Queue 26' */
#define TANTOS_3G_DM4_PQ26_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ26_SHIFT                          12
#define TANTOS_3G_DM4_PQ26_SIZE                           2
/* Bit: 'PQ25' */
/* Description: 'Priority Queue 25' */
#define TANTOS_3G_DM4_PQ25_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ25_SHIFT                          10
#define TANTOS_3G_DM4_PQ25_SIZE                           2
/* Bit: 'PQ24' */
/* Description: 'Priority Queue 24' */
#define TANTOS_3G_DM4_PQ24_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ24_SHIFT                          8
#define TANTOS_3G_DM4_PQ24_SIZE                           2
/* Bit: 'PQ23' */
/* Description: 'Priority Queue 23' */
#define TANTOS_3G_DM4_PQ23_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ23_SHIFT                          6
#define TANTOS_3G_DM4_PQ23_SIZE                           2
/* Bit: 'PQ22' */
/* Description: 'Priority Queue 22' */
#define TANTOS_3G_DM4_PQ22_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ22_SHIFT                          4
#define TANTOS_3G_DM4_PQ22_SIZE                           2
/* Bit: 'PQ21' */
/* Description: 'Priority Queue 21' */
#define TANTOS_3G_DM4_PQ21_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ21_SHIFT                          2
#define TANTOS_3G_DM4_PQ21_SIZE                           2
/* Bit: 'PQ20' */
/* Description: 'Priority Queue 20' */
#define TANTOS_3G_DM4_PQ20_OFFSET                         0x5C
#define TANTOS_3G_DM4_PQ20_SHIFT                          0
#define TANTOS_3G_DM4_PQ20_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 5' */
/* Bit: 'PQ2F' */
/* Description: 'Priority Queue 2F' */
#define TANTOS_3G_DM5_PQ2F_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ2F_SHIFT                          14
#define TANTOS_3G_DM5_PQ2F_SIZE                           2
/* Bit: 'PQ2E' */
/* Description: 'Priority Queue 2E' */
#define TANTOS_3G_DM5_PQ2E_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ2E_SHIFT                          12
#define TANTOS_3G_DM5_PQ2E_SIZE                           2
/* Bit: 'PQ2D' */
/* Description: 'Priority Queue 2D' */
#define TANTOS_3G_DM5_PQ2D_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ2D_SHIFT                          10
#define TANTOS_3G_DM5_PQ2D_SIZE                           2
/* Bit: 'PQ2C' */
/* Description: 'Priority Queue 2C' */
#define TANTOS_3G_DM5_PQ2C_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ2C_SHIFT                          8
#define TANTOS_3G_DM5_PQ2C_SIZE                           2
/* Bit: 'PQ2B' */
/* Description: 'Priority Queue 2B' */
#define TANTOS_3G_DM5_PQ2B_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ2B_SHIFT                          6
#define TANTOS_3G_DM5_PQ2B_SIZE                           2
/* Bit: 'PQ2A' */
/* Description: 'Priority Queue 2A' */
#define TANTOS_3G_DM5_PQ2A_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ2A_SHIFT                          4
#define TANTOS_3G_DM5_PQ2A_SIZE                           2
/* Bit: 'PQ29' */
/* Description: 'Priority Queue 29' */
#define TANTOS_3G_DM5_PQ29_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ29_SHIFT                          2
#define TANTOS_3G_DM5_PQ29_SIZE                           2
/* Bit: 'PQ28' */
/* Description: 'Priority Queue 28' */
#define TANTOS_3G_DM5_PQ28_OFFSET                         0x5D
#define TANTOS_3G_DM5_PQ28_SHIFT                          0
#define TANTOS_3G_DM5_PQ28_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 6' */
/* Bit: 'PQ37' */
/* Description: 'Priority Queue 37' */
#define TANTOS_3G_DM6_PQ37_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ37_SHIFT                          14
#define TANTOS_3G_DM6_PQ37_SIZE                           2
/* Bit: 'PQ36' */
/* Description: 'Priority Queue 36' */
#define TANTOS_3G_DM6_PQ36_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ36_SHIFT                          12
#define TANTOS_3G_DM6_PQ36_SIZE                           2
/* Bit: 'PQ35' */
/* Description: 'Priority Queue 35' */
#define TANTOS_3G_DM6_PQ35_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ35_SHIFT                          10
#define TANTOS_3G_DM6_PQ35_SIZE                           2
/* Bit: 'PQ34' */
/* Description: 'Priority Queue 34' */
#define TANTOS_3G_DM6_PQ34_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ34_SHIFT                          8
#define TANTOS_3G_DM6_PQ34_SIZE                           2
/* Bit: 'PQ33' */
/* Description: 'Priority Queue 33' */
#define TANTOS_3G_DM6_PQ33_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ33_SHIFT                          6
#define TANTOS_3G_DM6_PQ33_SIZE                           2
/* Bit: 'PQ32' */
/* Description: 'Priority Queue 32' */
#define TANTOS_3G_DM6_PQ32_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ32_SHIFT                          4
#define TANTOS_3G_DM6_PQ32_SIZE                           2
/* Bit: 'PQ31' */
/* Description: 'Priority Queue 31' */
#define TANTOS_3G_DM6_PQ31_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ31_SHIFT                          2
#define TANTOS_3G_DM6_PQ31_SIZE                           2
/* Bit: 'PQ30' */
/* Description: 'Priority Queue 30' */
#define TANTOS_3G_DM6_PQ30_OFFSET                         0x5E
#define TANTOS_3G_DM6_PQ30_SHIFT                          0
#define TANTOS_3G_DM6_PQ30_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'DiffServMapping 7' */
/* Bit: 'PQ3F' */
/* Description: 'Priority Queue 3F' */
#define TANTOS_3G_DM7_PQ3F_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ3F_SHIFT                          14
#define TANTOS_3G_DM7_PQ3F_SIZE                           2
/* Bit: 'PQ3E' */
/* Description: 'Priority Queue 3E' */
#define TANTOS_3G_DM7_PQ3E_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ3E_SHIFT                          12
#define TANTOS_3G_DM7_PQ3E_SIZE                           2
/* Bit: 'PQ3D' */
/* Description: 'Priority Queue 3D' */
#define TANTOS_3G_DM7_PQ3D_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ3D_SHIFT                          10
#define TANTOS_3G_DM7_PQ3D_SIZE                           2
/* Bit: 'PQ3C' */
/* Description: 'Priority Queue 3C' */
#define TANTOS_3G_DM7_PQ3C_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ3C_SHIFT                          8
#define TANTOS_3G_DM7_PQ3C_SIZE                           2
/* Bit: 'PQ3B' */
/* Description: 'Priority Queue 3B' */
#define TANTOS_3G_DM7_PQ3B_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ3B_SHIFT                          6
#define TANTOS_3G_DM7_PQ3B_SIZE                           2
/* Bit: 'PQ3A' */
/* Description: 'Priority Queue 3A' */
#define TANTOS_3G_DM7_PQ3A_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ3A_SHIFT                          4
#define TANTOS_3G_DM7_PQ3A_SIZE                           2
/* Bit: 'PQ39' */
/* Description: 'Priority Queue 39' */
#define TANTOS_3G_DM7_PQ39_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ39_SHIFT                          2
#define TANTOS_3G_DM7_PQ39_SIZE                           2
/* Bit: 'PQ38' */
/* Description: 'Priority Queue 38' */
#define TANTOS_3G_DM7_PQ38_OFFSET                         0x5F
#define TANTOS_3G_DM7_PQ38_SHIFT                          0
#define TANTOS_3G_DM7_PQ38_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 0' */
/* Bit: 'BASEPT0' */
/* Description: 'Base Port number 0' */
#define TANTOS_3G_TUPF0_BASEPT0_OFFSET                    0x70
#define TANTOS_3G_TUPF0_BASEPT0_SHIFT                     0
#define TANTOS_3G_TUPF0_BASEPT0_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 1' */
/* Bit: 'BASEPT1' */
/* Description: 'Base Port number 1' */
#define TANTOS_3G_TUPF1_BASEPT1_OFFSET                    0x72
#define TANTOS_3G_TUPF1_BASEPT1_SHIFT                     0
#define TANTOS_3G_TUPF1_BASEPT1_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 2' */
/* Bit: 'BASEPT2' */
/* Description: 'Base Port number 2' */
#define TANTOS_3G_TUPF2_BASEPT2_OFFSET                    0x74
#define TANTOS_3G_TUPF2_BASEPT2_SHIFT                     0
#define TANTOS_3G_TUPF2_BASEPT2_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 3' */
/* Bit: 'BASEPT3' */
/* Description: 'Base Port number 3' */
#define TANTOS_3G_TUPF3_BASEPT3_OFFSET                    0x76
#define TANTOS_3G_TUPF3_BASEPT3_SHIFT                     0
#define TANTOS_3G_TUPF3_BASEPT3_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 4' */
/* Bit: 'BASEPT4' */
/* Description: 'Base Port number 4' */
#define TANTOS_3G_TUPF4_BASEPT4_OFFSET                    0x78
#define TANTOS_3G_TUPF4_BASEPT4_SHIFT                     0
#define TANTOS_3G_TUPF4_BASEPT4_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 5' */
/* Bit: 'BASEPT5' */
/* Description: 'Base Port number 5' */
#define TANTOS_3G_TUPF5_BASEPT5_OFFSET                    0x7A
#define TANTOS_3G_TUPF5_BASEPT5_SHIFT                     0
#define TANTOS_3G_TUPF5_BASEPT5_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 6' */
/* Bit: 'BASEPT6' */
/* Description: 'Base Port number 6' */
#define TANTOS_3G_TUPF6_BASEPT6_OFFSET                    0x7C
#define TANTOS_3G_TUPF6_BASEPT6_SHIFT                     0
#define TANTOS_3G_TUPF6_BASEPT6_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Filter 7' */
/* Bit: 'BASEPT7' */
/* Description: 'Base Port number 7' */
#define TANTOS_3G_TUPF7_BASEPT7_OFFSET                    0x7E
#define TANTOS_3G_TUPF7_BASEPT7_SHIFT                     0
#define TANTOS_3G_TUPF7_BASEPT7_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 0' */
/* Bit: 'ATUF0' */
/* Description: 'Action for TCP/UDP Port Filter 0' */
#define TANTOS_3G_TUPR0_ATUF0_OFFSET                      0x71
#define TANTOS_3G_TUPR0_ATUF0_SHIFT                       12
#define TANTOS_3G_TUPR0_ATUF0_SIZE                        2
/* Bit: 'TUPF0' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 0' */
#define TANTOS_3G_TUPR0_TUPF0_OFFSET                      0x71
#define TANTOS_3G_TUPR0_TUPF0_SHIFT                       10
#define TANTOS_3G_TUPR0_TUPF0_SIZE                        2
/* Bit: 'COMP0' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR0_COMP0_OFFSET                      0x71
#define TANTOS_3G_TUPR0_COMP0_SHIFT                       8
#define TANTOS_3G_TUPR0_COMP0_SIZE                        2
/* Bit: 'PRANGE0' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR0_PRANGE0_OFFSET                    0x71
#define TANTOS_3G_TUPR0_PRANGE0_SHIFT                     0
#define TANTOS_3G_TUPR0_PRANGE0_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 1' */
/* Bit: 'ATUF1' */
/* Description: 'Action for TCP/UDP Port Filter 1' */
#define TANTOS_3G_TUPR1_ATUF1_OFFSET                      0x73
#define TANTOS_3G_TUPR1_ATUF1_SHIFT                       12
#define TANTOS_3G_TUPR1_ATUF1_SIZE                        2
/* Bit: 'TUPF1' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 1' */
#define TANTOS_3G_TUPR1_TUPF1_OFFSET                      0x73
#define TANTOS_3G_TUPR1_TUPF1_SHIFT                       10
#define TANTOS_3G_TUPR1_TUPF1_SIZE                        2
/* Bit: 'COMP1' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR1_COMP1_OFFSET                      0x73
#define TANTOS_3G_TUPR1_COMP1_SHIFT                       8
#define TANTOS_3G_TUPR1_COMP1_SIZE                        2
/* Bit: 'PRANGE1' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR1_PRANGE1_OFFSET                    0x73
#define TANTOS_3G_TUPR1_PRANGE1_SHIFT                     0
#define TANTOS_3G_TUPR1_PRANGE1_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 2' */
/* Bit: 'ATUF2' */
/* Description: 'Action for TCP/UDP Port Filter 2' */
#define TANTOS_3G_TUPR2_ATUF2_OFFSET                      0x75
#define TANTOS_3G_TUPR2_ATUF2_SHIFT                       12
#define TANTOS_3G_TUPR2_ATUF2_SIZE                        2
/* Bit: 'TUPF2' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 2' */
#define TANTOS_3G_TUPR2_TUPF2_OFFSET                      0x75
#define TANTOS_3G_TUPR2_TUPF2_SHIFT                       10
#define TANTOS_3G_TUPR2_TUPF2_SIZE                        2
/* Bit: 'COMP2' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR2_COMP2_OFFSET                      0x75
#define TANTOS_3G_TUPR2_COMP2_SHIFT                       8
#define TANTOS_3G_TUPR2_COMP2_SIZE                        2
/* Bit: 'PRANGE2' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR2_PRANGE2_OFFSET                    0x75
#define TANTOS_3G_TUPR2_PRANGE2_SHIFT                     0
#define TANTOS_3G_TUPR2_PRANGE2_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 3' */
/* Bit: 'ATUF3' */
/* Description: 'Action for TCP/UDP Port Filter 3' */
#define TANTOS_3G_TUPR3_ATUF3_OFFSET                      0x77
#define TANTOS_3G_TUPR3_ATUF3_SHIFT                       12
#define TANTOS_3G_TUPR3_ATUF3_SIZE                        2
/* Bit: 'TUPF3' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 3' */
#define TANTOS_3G_TUPR3_TUPF3_OFFSET                      0x77
#define TANTOS_3G_TUPR3_TUPF3_SHIFT                       10
#define TANTOS_3G_TUPR3_TUPF3_SIZE                        2
/* Bit: 'COMP3' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR3_COMP3_OFFSET                      0x77
#define TANTOS_3G_TUPR3_COMP3_SHIFT                       8
#define TANTOS_3G_TUPR3_COMP3_SIZE                        2
/* Bit: 'PRANGE3' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR3_PRANGE3_OFFSET                    0x77
#define TANTOS_3G_TUPR3_PRANGE3_SHIFT                     0
#define TANTOS_3G_TUPR3_PRANGE3_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 4' */
/* Bit: 'ATUF4' */
/* Description: 'Action for TCP/UDP Port Filter 4' */
#define TANTOS_3G_TUPR4_ATUF4_OFFSET                      0x79
#define TANTOS_3G_TUPR4_ATUF4_SHIFT                       12
#define TANTOS_3G_TUPR4_ATUF4_SIZE                        2
/* Bit: 'TUPF4' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 4' */
#define TANTOS_3G_TUPR4_TUPF4_OFFSET                      0x79
#define TANTOS_3G_TUPR4_TUPF4_SHIFT                       10
#define TANTOS_3G_TUPR4_TUPF4_SIZE                        2
/* Bit: 'COMP4' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR4_COMP4_OFFSET                      0x79
#define TANTOS_3G_TUPR4_COMP4_SHIFT                       8
#define TANTOS_3G_TUPR4_COMP4_SIZE                        2
/* Bit: 'PRANGE4' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR4_PRANGE4_OFFSET                    0x79
#define TANTOS_3G_TUPR4_PRANGE4_SHIFT                     0
#define TANTOS_3G_TUPR4_PRANGE4_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 5' */
/* Bit: 'ATUF5' */
/* Description: 'Action for TCP/UDP Port Filter 5' */
#define TANTOS_3G_TUPR5_ATUF5_OFFSET                      0x7B
#define TANTOS_3G_TUPR5_ATUF5_SHIFT                       12
#define TANTOS_3G_TUPR5_ATUF5_SIZE                        2
/* Bit: 'TUPF5' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 5' */
#define TANTOS_3G_TUPR5_TUPF5_OFFSET                      0x7B
#define TANTOS_3G_TUPR5_TUPF5_SHIFT                       10
#define TANTOS_3G_TUPR5_TUPF5_SIZE                        2
/* Bit: 'COMP5' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR5_COMP5_OFFSET                      0x7B
#define TANTOS_3G_TUPR5_COMP5_SHIFT                       8
#define TANTOS_3G_TUPR5_COMP5_SIZE                        2
/* Bit: 'PRANGE5' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR5_PRANGE5_OFFSET                    0x7B
#define TANTOS_3G_TUPR5_PRANGE5_SHIFT                     0
#define TANTOS_3G_TUPR5_PRANGE5_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 6' */
/* Bit: 'ATUF6' */
/* Description: 'Action for TCP/UDP Port Filter 6' */
#define TANTOS_3G_TUPR6_ATUF6_OFFSET                      0x7D
#define TANTOS_3G_TUPR6_ATUF6_SHIFT                       12
#define TANTOS_3G_TUPR6_ATUF6_SIZE                        2
/* Bit: 'TUPF6' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 6' */
#define TANTOS_3G_TUPR6_TUPF6_OFFSET                      0x7D
#define TANTOS_3G_TUPR6_TUPF6_SHIFT                       10
#define TANTOS_3G_TUPR6_TUPF6_SIZE                        2
/* Bit: 'COMP6' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR6_COMP6_OFFSET                      0x7D
#define TANTOS_3G_TUPR6_COMP6_SHIFT                       8
#define TANTOS_3G_TUPR6_COMP6_SIZE                        2
/* Bit: 'PRANGE6' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR6_PRANGE6_OFFSET                    0x7D
#define TANTOS_3G_TUPR6_PRANGE6_SHIFT                     0
#define TANTOS_3G_TUPR6_PRANGE6_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'TCP/UDP Port Range 7' */
/* Bit: 'ATUF7' */
/* Description: 'Action for TCP/UDP Port Filter 7' */
#define TANTOS_3G_TUPR7_ATUF7_OFFSET                      0x7F
#define TANTOS_3G_TUPR7_ATUF7_SHIFT                       12
#define TANTOS_3G_TUPR7_ATUF7_SIZE                        2
/* Bit: 'TUPF7' */
/* Description: 'TCP/UDP PRI for TCP/UDP Port Filter 7' */
#define TANTOS_3G_TUPR7_TUPF7_OFFSET                      0x7F
#define TANTOS_3G_TUPR7_TUPF7_SHIFT                       10
#define TANTOS_3G_TUPR7_TUPF7_SIZE                        2
/* Bit: 'COMP7' */
/* Description: 'Compare TCP/UDP Source Port or Destination Port' */
#define TANTOS_3G_TUPR7_COMP7_OFFSET                      0x7F
#define TANTOS_3G_TUPR7_COMP7_SHIFT                       8
#define TANTOS_3G_TUPR7_COMP7_SIZE                        2
/* Bit: 'PRANGE7' */
/* Description: 'Port Range in TCP/UDP' */
#define TANTOS_3G_TUPR7_PRANGE7_OFFSET                    0x7F
#define TANTOS_3G_TUPR7_PRANGE7_SHIFT                     0
#define TANTOS_3G_TUPR7_PRANGE7_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'Reserved DA(0180C2000001~0180C2000000) control register' */
/* Bit: 'RA01_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_01_00_RA01_VALID_OFFSET              0x90
#define TANTOS_3G_RA_01_00_RA01_VALID_SHIFT               15
#define TANTOS_3G_RA_01_00_RA01_VALID_SIZE                1
/* Bit: 'RA01_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_01_00_RA01_SPAN_OFFSET               0x90
#define TANTOS_3G_RA_01_00_RA01_SPAN_SHIFT                14
#define TANTOS_3G_RA_01_00_RA01_SPAN_SIZE                 1
/* Bit: 'RA01_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_01_00_RA01_MG_OFFSET                 0x90
#define TANTOS_3G_RA_01_00_RA01_MG_SHIFT                  13
#define TANTOS_3G_RA_01_00_RA01_MG_SIZE                   1
/* Bit: 'RA01_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_01_00_RA01_CV_OFFSET                 0x90
#define TANTOS_3G_RA_01_00_RA01_CV_SHIFT                  12
#define TANTOS_3G_RA_01_00_RA01_CV_SIZE                   1
/* Bit: 'RA01_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_01_00_RA01_TXTAG_OFFSET              0x90
#define TANTOS_3G_RA_01_00_RA01_TXTAG_SHIFT               10
#define TANTOS_3G_RA_01_00_RA01_TXTAG_SIZE                2
/* Bit: 'RA01_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_01_00_RA01_ACT_OFFSET                0x90
#define TANTOS_3G_RA_01_00_RA01_ACT_SHIFT                 8
#define TANTOS_3G_RA_01_00_RA01_ACT_SIZE                  2
/* Bit: 'RA00_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_01_00_RA00_VALID_OFFSET              0x90
#define TANTOS_3G_RA_01_00_RA00_VALID_SHIFT               7
#define TANTOS_3G_RA_01_00_RA00_VALID_SIZE                1
/* Bit: 'RA00_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_01_00_RA00_SPAN_OFFSET               0x90
#define TANTOS_3G_RA_01_00_RA00_SPAN_SHIFT                6
#define TANTOS_3G_RA_01_00_RA00_SPAN_SIZE                 1
/* Bit: 'RA00_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_01_00_RA00_MG_OFFSET                 0x90
#define TANTOS_3G_RA_01_00_RA00_MG_SHIFT                  5
#define TANTOS_3G_RA_01_00_RA00_MG_SIZE                   1
/* Bit: 'RA00_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_01_00_RA00_CV_OFFSET                 0x90
#define TANTOS_3G_RA_01_00_RA00_CV_SHIFT                  4
#define TANTOS_3G_RA_01_00_RA00_CV_SIZE                   1
/* Bit: 'RA00_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_01_00_RA00_TXTAG_OFFSET              0x90
#define TANTOS_3G_RA_01_00_RA00_TXTAG_SHIFT               2
#define TANTOS_3G_RA_01_00_RA00_TXTAG_SIZE                2
/* Bit: 'RA00_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_01_00_RA00_ACT_OFFSET                0x90
#define TANTOS_3G_RA_01_00_RA00_ACT_SHIFT                 0
#define TANTOS_3G_RA_01_00_RA00_ACT_SIZE                  2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000003~0180C2000002' */
/* Bit: 'RA23_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_03_02_RA23_VALID_OFFSET              0x91
#define TANTOS_3G_RA_03_02_RA23_VALID_SHIFT               15
#define TANTOS_3G_RA_03_02_RA23_VALID_SIZE                1
/* Bit: 'RA23_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_03_02_RA23_SPAN_OFFSET               0x91
#define TANTOS_3G_RA_03_02_RA23_SPAN_SHIFT                14
#define TANTOS_3G_RA_03_02_RA23_SPAN_SIZE                 1
/* Bit: 'RA23_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_03_02_RA23_MG_OFFSET                 0x91
#define TANTOS_3G_RA_03_02_RA23_MG_SHIFT                  13
#define TANTOS_3G_RA_03_02_RA23_MG_SIZE                   1
/* Bit: 'RA23_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_03_02_RA23_CV_OFFSET                 0x91
#define TANTOS_3G_RA_03_02_RA23_CV_SHIFT                  12
#define TANTOS_3G_RA_03_02_RA23_CV_SIZE                   1
/* Bit: 'RA23_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_03_02_RA23_TXTAG_OFFSET              0x91
#define TANTOS_3G_RA_03_02_RA23_TXTAG_SHIFT               10
#define TANTOS_3G_RA_03_02_RA23_TXTAG_SIZE                2
/* Bit: 'RA23_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_03_02_RA23_ACT_OFFSET                0x91
#define TANTOS_3G_RA_03_02_RA23_ACT_SHIFT                 8
#define TANTOS_3G_RA_03_02_RA23_ACT_SIZE                  2
/* Bit: 'RA20_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_03_02_RA20_VALID_OFFSET              0x91
#define TANTOS_3G_RA_03_02_RA20_VALID_SHIFT               7
#define TANTOS_3G_RA_03_02_RA20_VALID_SIZE                1
/* Bit: 'RA20_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_03_02_RA20_SPAN_OFFSET               0x91
#define TANTOS_3G_RA_03_02_RA20_SPAN_SHIFT                6
#define TANTOS_3G_RA_03_02_RA20_SPAN_SIZE                 1
/* Bit: 'RA20_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_03_02_RA20_MG_OFFSET                 0x91
#define TANTOS_3G_RA_03_02_RA20_MG_SHIFT                  5
#define TANTOS_3G_RA_03_02_RA20_MG_SIZE                   1
/* Bit: 'RA20_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_03_02_RA20_CV_OFFSET                 0x91
#define TANTOS_3G_RA_03_02_RA20_CV_SHIFT                  4
#define TANTOS_3G_RA_03_02_RA20_CV_SIZE                   1
/* Bit: 'RA20_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_03_02_RA20_TXTAG_OFFSET              0x91
#define TANTOS_3G_RA_03_02_RA20_TXTAG_SHIFT               2
#define TANTOS_3G_RA_03_02_RA20_TXTAG_SIZE                2
/* Bit: 'RA20_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_03_02_RA20_ACT_OFFSET                0x91
#define TANTOS_3G_RA_03_02_RA20_ACT_SHIFT                 0
#define TANTOS_3G_RA_03_02_RA20_ACT_SIZE                  2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000005~0180C2000004' */
/* Bit: 'RA45_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_05_04_RA45_VALID_OFFSET              0x92
#define TANTOS_3G_RA_05_04_RA45_VALID_SHIFT               15
#define TANTOS_3G_RA_05_04_RA45_VALID_SIZE                1
/* Bit: 'RA45_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_05_04_RA45_SPAN_OFFSET               0x92
#define TANTOS_3G_RA_05_04_RA45_SPAN_SHIFT                14
#define TANTOS_3G_RA_05_04_RA45_SPAN_SIZE                 1
/* Bit: 'RA45_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_05_04_RA45_MG_OFFSET                 0x92
#define TANTOS_3G_RA_05_04_RA45_MG_SHIFT                  13
#define TANTOS_3G_RA_05_04_RA45_MG_SIZE                   1
/* Bit: 'RA45_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_05_04_RA45_CV_OFFSET                 0x92
#define TANTOS_3G_RA_05_04_RA45_CV_SHIFT                  12
#define TANTOS_3G_RA_05_04_RA45_CV_SIZE                   1
/* Bit: 'RA45_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_05_04_RA45_TXTAG_OFFSET              0x92
#define TANTOS_3G_RA_05_04_RA45_TXTAG_SHIFT               10
#define TANTOS_3G_RA_05_04_RA45_TXTAG_SIZE                2
/* Bit: 'RA45_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_05_04_RA45_ACT_OFFSET                0x92
#define TANTOS_3G_RA_05_04_RA45_ACT_SHIFT                 8
#define TANTOS_3G_RA_05_04_RA45_ACT_SIZE                  2
/* Bit: 'RA40_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_05_04_RA40_VALID_OFFSET              0x92
#define TANTOS_3G_RA_05_04_RA40_VALID_SHIFT               7
#define TANTOS_3G_RA_05_04_RA40_VALID_SIZE                1
/* Bit: 'RA40_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_05_04_RA40_SPAN_OFFSET               0x92
#define TANTOS_3G_RA_05_04_RA40_SPAN_SHIFT                6
#define TANTOS_3G_RA_05_04_RA40_SPAN_SIZE                 1
/* Bit: 'RA40_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_05_04_RA40_MG_OFFSET                 0x92
#define TANTOS_3G_RA_05_04_RA40_MG_SHIFT                  5
#define TANTOS_3G_RA_05_04_RA40_MG_SIZE                   1
/* Bit: 'RA40_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_05_04_RA40_CV_OFFSET                 0x92
#define TANTOS_3G_RA_05_04_RA40_CV_SHIFT                  4
#define TANTOS_3G_RA_05_04_RA40_CV_SIZE                   1
/* Bit: 'RA40_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_05_04_RA40_TXTAG_OFFSET              0x92
#define TANTOS_3G_RA_05_04_RA40_TXTAG_SHIFT               2
#define TANTOS_3G_RA_05_04_RA40_TXTAG_SIZE                2
/* Bit: 'RA40_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_05_04_RA40_ACT_OFFSET                0x92
#define TANTOS_3G_RA_05_04_RA40_ACT_SHIFT                 0
#define TANTOS_3G_RA_05_04_RA40_ACT_SIZE                  2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000007~0180C2000006' */
/* Bit: 'RA67_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_07_06_RA67_VALID_OFFSET              0x93
#define TANTOS_3G_RA_07_06_RA67_VALID_SHIFT               15
#define TANTOS_3G_RA_07_06_RA67_VALID_SIZE                1
/* Bit: 'RA67_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_07_06_RA67_SPAN_OFFSET               0x93
#define TANTOS_3G_RA_07_06_RA67_SPAN_SHIFT                14
#define TANTOS_3G_RA_07_06_RA67_SPAN_SIZE                 1
/* Bit: 'RA67_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_07_06_RA67_MG_OFFSET                 0x93
#define TANTOS_3G_RA_07_06_RA67_MG_SHIFT                  13
#define TANTOS_3G_RA_07_06_RA67_MG_SIZE                   1
/* Bit: 'RA67_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_07_06_RA67_CV_OFFSET                 0x93
#define TANTOS_3G_RA_07_06_RA67_CV_SHIFT                  12
#define TANTOS_3G_RA_07_06_RA67_CV_SIZE                   1
/* Bit: 'RA67_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_07_06_RA67_TXTAG_OFFSET              0x93
#define TANTOS_3G_RA_07_06_RA67_TXTAG_SHIFT               10
#define TANTOS_3G_RA_07_06_RA67_TXTAG_SIZE                2
/* Bit: 'RA67_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_07_06_RA67_ACT_OFFSET                0x93
#define TANTOS_3G_RA_07_06_RA67_ACT_SHIFT                 8
#define TANTOS_3G_RA_07_06_RA67_ACT_SIZE                  2
/* Bit: 'RA60_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_07_06_RA60_VALID_OFFSET              0x93
#define TANTOS_3G_RA_07_06_RA60_VALID_SHIFT               7
#define TANTOS_3G_RA_07_06_RA60_VALID_SIZE                1
/* Bit: 'RA60_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_07_06_RA60_SPAN_OFFSET               0x93
#define TANTOS_3G_RA_07_06_RA60_SPAN_SHIFT                6
#define TANTOS_3G_RA_07_06_RA60_SPAN_SIZE                 1
/* Bit: 'RA60_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_07_06_RA60_MG_OFFSET                 0x93
#define TANTOS_3G_RA_07_06_RA60_MG_SHIFT                  5
#define TANTOS_3G_RA_07_06_RA60_MG_SIZE                   1
/* Bit: 'RA60_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_07_06_RA60_CV_OFFSET                 0x93
#define TANTOS_3G_RA_07_06_RA60_CV_SHIFT                  4
#define TANTOS_3G_RA_07_06_RA60_CV_SIZE                   1
/* Bit: 'RA60_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_07_06_RA60_TXTAG_OFFSET              0x93
#define TANTOS_3G_RA_07_06_RA60_TXTAG_SHIFT               2
#define TANTOS_3G_RA_07_06_RA60_TXTAG_SIZE                2
/* Bit: 'RA60_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_07_06_RA60_ACT_OFFSET                0x93
#define TANTOS_3G_RA_07_06_RA60_ACT_SHIFT                 0
#define TANTOS_3G_RA_07_06_RA60_ACT_SIZE                  2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000009~0180C2000008' */
/* Bit: 'RA89_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_09_08_RA89_VALID_OFFSET              0x94
#define TANTOS_3G_RA_09_08_RA89_VALID_SHIFT               15
#define TANTOS_3G_RA_09_08_RA89_VALID_SIZE                1
/* Bit: 'RA89_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_09_08_RA89_SPAN_OFFSET               0x94
#define TANTOS_3G_RA_09_08_RA89_SPAN_SHIFT                14
#define TANTOS_3G_RA_09_08_RA89_SPAN_SIZE                 1
/* Bit: 'RA89_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_09_08_RA89_MG_OFFSET                 0x94
#define TANTOS_3G_RA_09_08_RA89_MG_SHIFT                  13
#define TANTOS_3G_RA_09_08_RA89_MG_SIZE                   1
/* Bit: 'RA89_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_09_08_RA89_CV_OFFSET                 0x94
#define TANTOS_3G_RA_09_08_RA89_CV_SHIFT                  12
#define TANTOS_3G_RA_09_08_RA89_CV_SIZE                   1
/* Bit: 'RA89_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_09_08_RA89_TXTAG_OFFSET              0x94
#define TANTOS_3G_RA_09_08_RA89_TXTAG_SHIFT               10
#define TANTOS_3G_RA_09_08_RA89_TXTAG_SIZE                2
/* Bit: 'RA89_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_09_08_RA89_ACT_OFFSET                0x94
#define TANTOS_3G_RA_09_08_RA89_ACT_SHIFT                 8
#define TANTOS_3G_RA_09_08_RA89_ACT_SIZE                  2
/* Bit: 'RA80_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_09_08_RA80_VALID_OFFSET              0x94
#define TANTOS_3G_RA_09_08_RA80_VALID_SHIFT               7
#define TANTOS_3G_RA_09_08_RA80_VALID_SIZE                1
/* Bit: 'RA80_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_09_08_RA80_SPAN_OFFSET               0x94
#define TANTOS_3G_RA_09_08_RA80_SPAN_SHIFT                6
#define TANTOS_3G_RA_09_08_RA80_SPAN_SIZE                 1
/* Bit: 'RA80_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_09_08_RA80_MG_OFFSET                 0x94
#define TANTOS_3G_RA_09_08_RA80_MG_SHIFT                  5
#define TANTOS_3G_RA_09_08_RA80_MG_SIZE                   1
/* Bit: 'RA80_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_09_08_RA80_CV_OFFSET                 0x94
#define TANTOS_3G_RA_09_08_RA80_CV_SHIFT                  4
#define TANTOS_3G_RA_09_08_RA80_CV_SIZE                   1
/* Bit: 'RA80_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_09_08_RA80_TXTAG_OFFSET              0x94
#define TANTOS_3G_RA_09_08_RA80_TXTAG_SHIFT               2
#define TANTOS_3G_RA_09_08_RA80_TXTAG_SIZE                2
/* Bit: 'RA80_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_09_08_RA80_ACT_OFFSET                0x94
#define TANTOS_3G_RA_09_08_RA80_ACT_SHIFT                 0
#define TANTOS_3G_RA_09_08_RA80_ACT_SIZE                  2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200000B~0180C200000A' */
/* Bit: 'RA1101_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_0B_0A_RA1101_VALID_OFFSET            0x95
#define TANTOS_3G_RA_0B_0A_RA1101_VALID_SHIFT             15
#define TANTOS_3G_RA_0B_0A_RA1101_VALID_SIZE              1
/* Bit: 'RA1101_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_0B_0A_RA1101_SPAN_OFFSET             0x95
#define TANTOS_3G_RA_0B_0A_RA1101_SPAN_SHIFT              14
#define TANTOS_3G_RA_0B_0A_RA1101_SPAN_SIZE               1
/* Bit: 'RA1101_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_0B_0A_RA1101_MG_OFFSET               0x95
#define TANTOS_3G_RA_0B_0A_RA1101_MG_SHIFT                13
#define TANTOS_3G_RA_0B_0A_RA1101_MG_SIZE                 1
/* Bit: 'RA1101_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_0B_0A_RA1101_CV_OFFSET               0x95
#define TANTOS_3G_RA_0B_0A_RA1101_CV_SHIFT                12
#define TANTOS_3G_RA_0B_0A_RA1101_CV_SIZE                 1
/* Bit: 'RA1101_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_0B_0A_RA1101_TXTAG_OFFSET            0x95
#define TANTOS_3G_RA_0B_0A_RA1101_TXTAG_SHIFT             10
#define TANTOS_3G_RA_0B_0A_RA1101_TXTAG_SIZE              2
/* Bit: 'RA1101_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_0B_0A_RA1101_ACT_OFFSET              0x95
#define TANTOS_3G_RA_0B_0A_RA1101_ACT_SHIFT               8
#define TANTOS_3G_RA_0B_0A_RA1101_ACT_SIZE                2
/* Bit: 'RA1100_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_0B_0A_RA1100_VALID_OFFSET            0x95
#define TANTOS_3G_RA_0B_0A_RA1100_VALID_SHIFT             7
#define TANTOS_3G_RA_0B_0A_RA1100_VALID_SIZE              1
/* Bit: 'RA1100_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_0B_0A_RA1100_SPAN_OFFSET             0x95
#define TANTOS_3G_RA_0B_0A_RA1100_SPAN_SHIFT              6
#define TANTOS_3G_RA_0B_0A_RA1100_SPAN_SIZE               1
/* Bit: 'RA1100_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_0B_0A_RA1100_MG_OFFSET               0x95
#define TANTOS_3G_RA_0B_0A_RA1100_MG_SHIFT                5
#define TANTOS_3G_RA_0B_0A_RA1100_MG_SIZE                 1
/* Bit: 'RA1100_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_0B_0A_RA1100_CV_OFFSET               0x95
#define TANTOS_3G_RA_0B_0A_RA1100_CV_SHIFT                4
#define TANTOS_3G_RA_0B_0A_RA1100_CV_SIZE                 1
/* Bit: 'RA1100_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_0B_0A_RA1100_TXTAG_OFFSET            0x95
#define TANTOS_3G_RA_0B_0A_RA1100_TXTAG_SHIFT             2
#define TANTOS_3G_RA_0B_0A_RA1100_TXTAG_SIZE              2
/* Bit: 'RA1100_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_0B_0A_RA1100_ACT_OFFSET              0x95
#define TANTOS_3G_RA_0B_0A_RA1100_ACT_SHIFT               0
#define TANTOS_3G_RA_0B_0A_RA1100_ACT_SIZE                2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200000D~0180C200000C' */
/* Bit: 'RA1321_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_0D_0C_RA1321_VALID_OFFSET            0x96
#define TANTOS_3G_RA_0D_0C_RA1321_VALID_SHIFT             15
#define TANTOS_3G_RA_0D_0C_RA1321_VALID_SIZE              1
/* Bit: 'RA1321_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_0D_0C_RA1321_SPAN_OFFSET             0x96
#define TANTOS_3G_RA_0D_0C_RA1321_SPAN_SHIFT              14
#define TANTOS_3G_RA_0D_0C_RA1321_SPAN_SIZE               1
/* Bit: 'RA1321_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_0D_0C_RA1321_MG_OFFSET               0x96
#define TANTOS_3G_RA_0D_0C_RA1321_MG_SHIFT                13
#define TANTOS_3G_RA_0D_0C_RA1321_MG_SIZE                 1
/* Bit: 'RA1321_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_0D_0C_RA1321_CV_OFFSET               0x96
#define TANTOS_3G_RA_0D_0C_RA1321_CV_SHIFT                12
#define TANTOS_3G_RA_0D_0C_RA1321_CV_SIZE                 1
/* Bit: 'RA1321_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_0D_0C_RA1321_TXTAG_OFFSET            0x96
#define TANTOS_3G_RA_0D_0C_RA1321_TXTAG_SHIFT             10
#define TANTOS_3G_RA_0D_0C_RA1321_TXTAG_SIZE              2
/* Bit: 'RA1321_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_0D_0C_RA1321_ACT_OFFSET              0x96
#define TANTOS_3G_RA_0D_0C_RA1321_ACT_SHIFT               8
#define TANTOS_3G_RA_0D_0C_RA1321_ACT_SIZE                2
/* Bit: 'RA1320_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_0D_0C_RA1320_VALID_OFFSET            0x96
#define TANTOS_3G_RA_0D_0C_RA1320_VALID_SHIFT             7
#define TANTOS_3G_RA_0D_0C_RA1320_VALID_SIZE              1
/* Bit: 'RA1320_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_0D_0C_RA1320_SPAN_OFFSET             0x96
#define TANTOS_3G_RA_0D_0C_RA1320_SPAN_SHIFT              6
#define TANTOS_3G_RA_0D_0C_RA1320_SPAN_SIZE               1
/* Bit: 'RA1320_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_0D_0C_RA1320_MG_OFFSET               0x96
#define TANTOS_3G_RA_0D_0C_RA1320_MG_SHIFT                5
#define TANTOS_3G_RA_0D_0C_RA1320_MG_SIZE                 1
/* Bit: 'RA1320_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_0D_0C_RA1320_CV_OFFSET               0x96
#define TANTOS_3G_RA_0D_0C_RA1320_CV_SHIFT                4
#define TANTOS_3G_RA_0D_0C_RA1320_CV_SIZE                 1
/* Bit: 'RA1320_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_0D_0C_RA1320_TXTAG_OFFSET            0x96
#define TANTOS_3G_RA_0D_0C_RA1320_TXTAG_SHIFT             2
#define TANTOS_3G_RA_0D_0C_RA1320_TXTAG_SIZE              2
/* Bit: 'RA1320_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_0D_0C_RA1320_ACT_OFFSET              0x96
#define TANTOS_3G_RA_0D_0C_RA1320_ACT_SHIFT               0
#define TANTOS_3G_RA_0D_0C_RA1320_ACT_SIZE                2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200000F~0180C200000E' */
/* Bit: 'RA1541_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_0F_0E_RA1541_VALID_OFFSET            0x97
#define TANTOS_3G_RA_0F_0E_RA1541_VALID_SHIFT             15
#define TANTOS_3G_RA_0F_0E_RA1541_VALID_SIZE              1
/* Bit: 'RA1541_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_0F_0E_RA1541_SPAN_OFFSET             0x97
#define TANTOS_3G_RA_0F_0E_RA1541_SPAN_SHIFT              14
#define TANTOS_3G_RA_0F_0E_RA1541_SPAN_SIZE               1
/* Bit: 'RA1541_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_0F_0E_RA1541_MG_OFFSET               0x97
#define TANTOS_3G_RA_0F_0E_RA1541_MG_SHIFT                13
#define TANTOS_3G_RA_0F_0E_RA1541_MG_SIZE                 1
/* Bit: 'RA1541_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_0F_0E_RA1541_CV_OFFSET               0x97
#define TANTOS_3G_RA_0F_0E_RA1541_CV_SHIFT                12
#define TANTOS_3G_RA_0F_0E_RA1541_CV_SIZE                 1
/* Bit: 'RA1541_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_0F_0E_RA1541_TXTAG_OFFSET            0x97
#define TANTOS_3G_RA_0F_0E_RA1541_TXTAG_SHIFT             10
#define TANTOS_3G_RA_0F_0E_RA1541_TXTAG_SIZE              2
/* Bit: 'RA1541_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_0F_0E_RA1541_ACT_OFFSET              0x97
#define TANTOS_3G_RA_0F_0E_RA1541_ACT_SHIFT               8
#define TANTOS_3G_RA_0F_0E_RA1541_ACT_SIZE                2
/* Bit: 'RA1540_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_0F_0E_RA1540_VALID_OFFSET            0x97
#define TANTOS_3G_RA_0F_0E_RA1540_VALID_SHIFT             7
#define TANTOS_3G_RA_0F_0E_RA1540_VALID_SIZE              1
/* Bit: 'RA1540_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_0F_0E_RA1540_SPAN_OFFSET             0x97
#define TANTOS_3G_RA_0F_0E_RA1540_SPAN_SHIFT              6
#define TANTOS_3G_RA_0F_0E_RA1540_SPAN_SIZE               1
/* Bit: 'RA1540_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_0F_0E_RA1540_MG_OFFSET               0x97
#define TANTOS_3G_RA_0F_0E_RA1540_MG_SHIFT                5
#define TANTOS_3G_RA_0F_0E_RA1540_MG_SIZE                 1
/* Bit: 'RA1540_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_0F_0E_RA1540_CV_OFFSET               0x97
#define TANTOS_3G_RA_0F_0E_RA1540_CV_SHIFT                4
#define TANTOS_3G_RA_0F_0E_RA1540_CV_SIZE                 1
/* Bit: 'RA1540_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_0F_0E_RA1540_TXTAG_OFFSET            0x97
#define TANTOS_3G_RA_0F_0E_RA1540_TXTAG_SHIFT             2
#define TANTOS_3G_RA_0F_0E_RA1540_TXTAG_SIZE              2
/* Bit: 'RA1540_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_0F_0E_RA1540_ACT_OFFSET              0x97
#define TANTOS_3G_RA_0F_0E_RA1540_ACT_SHIFT               0
#define TANTOS_3G_RA_0F_0E_RA1540_ACT_SIZE                2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000011~0180C2000010' */
/* Bit: 'RA1761_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_11_10_RA1761_VALID_OFFSET            0x98
#define TANTOS_3G_RA_11_10_RA1761_VALID_SHIFT             15
#define TANTOS_3G_RA_11_10_RA1761_VALID_SIZE              1
/* Bit: 'RA1761_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_11_10_RA1761_SPAN_OFFSET             0x98
#define TANTOS_3G_RA_11_10_RA1761_SPAN_SHIFT              14
#define TANTOS_3G_RA_11_10_RA1761_SPAN_SIZE               1
/* Bit: 'RA1761_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_11_10_RA1761_MG_OFFSET               0x98
#define TANTOS_3G_RA_11_10_RA1761_MG_SHIFT                13
#define TANTOS_3G_RA_11_10_RA1761_MG_SIZE                 1
/* Bit: 'RA1761_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_11_10_RA1761_CV_OFFSET               0x98
#define TANTOS_3G_RA_11_10_RA1761_CV_SHIFT                12
#define TANTOS_3G_RA_11_10_RA1761_CV_SIZE                 1
/* Bit: 'RA1761_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_11_10_RA1761_TXTAG_OFFSET            0x98
#define TANTOS_3G_RA_11_10_RA1761_TXTAG_SHIFT             10
#define TANTOS_3G_RA_11_10_RA1761_TXTAG_SIZE              2
/* Bit: 'RA1761_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_11_10_RA1761_ACT_OFFSET              0x98
#define TANTOS_3G_RA_11_10_RA1761_ACT_SHIFT               8
#define TANTOS_3G_RA_11_10_RA1761_ACT_SIZE                2
/* Bit: 'RA1760_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_11_10_RA1760_VALID_OFFSET            0x98
#define TANTOS_3G_RA_11_10_RA1760_VALID_SHIFT             7
#define TANTOS_3G_RA_11_10_RA1760_VALID_SIZE              1
/* Bit: 'RA1760_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_11_10_RA1760_SPAN_OFFSET             0x98
#define TANTOS_3G_RA_11_10_RA1760_SPAN_SHIFT              6
#define TANTOS_3G_RA_11_10_RA1760_SPAN_SIZE               1
/* Bit: 'RA1760_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_11_10_RA1760_MG_OFFSET               0x98
#define TANTOS_3G_RA_11_10_RA1760_MG_SHIFT                5
#define TANTOS_3G_RA_11_10_RA1760_MG_SIZE                 1
/* Bit: 'RA1760_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_11_10_RA1760_CV_OFFSET               0x98
#define TANTOS_3G_RA_11_10_RA1760_CV_SHIFT                4
#define TANTOS_3G_RA_11_10_RA1760_CV_SIZE                 1
/* Bit: 'RA1760_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_11_10_RA1760_TXTAG_OFFSET            0x98
#define TANTOS_3G_RA_11_10_RA1760_TXTAG_SHIFT             2
#define TANTOS_3G_RA_11_10_RA1760_TXTAG_SIZE              2
/* Bit: 'RA1760_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_11_10_RA1760_ACT_OFFSET              0x98
#define TANTOS_3G_RA_11_10_RA1760_ACT_SHIFT               0
#define TANTOS_3G_RA_11_10_RA1760_ACT_SIZE                2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000013~0180C2000012' */
/* Bit: 'RA1981_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_13_12_RA1981_VALID_OFFSET            0x99
#define TANTOS_3G_RA_13_12_RA1981_VALID_SHIFT             15
#define TANTOS_3G_RA_13_12_RA1981_VALID_SIZE              1
/* Bit: 'RA1981_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_13_12_RA1981_SPAN_OFFSET             0x99
#define TANTOS_3G_RA_13_12_RA1981_SPAN_SHIFT              14
#define TANTOS_3G_RA_13_12_RA1981_SPAN_SIZE               1
/* Bit: 'RA1981_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_13_12_RA1981_MG_OFFSET               0x99
#define TANTOS_3G_RA_13_12_RA1981_MG_SHIFT                13
#define TANTOS_3G_RA_13_12_RA1981_MG_SIZE                 1
/* Bit: 'RA1981_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_13_12_RA1981_CV_OFFSET               0x99
#define TANTOS_3G_RA_13_12_RA1981_CV_SHIFT                12
#define TANTOS_3G_RA_13_12_RA1981_CV_SIZE                 1
/* Bit: 'RA1981_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_13_12_RA1981_TXTAG_OFFSET            0x99
#define TANTOS_3G_RA_13_12_RA1981_TXTAG_SHIFT             10
#define TANTOS_3G_RA_13_12_RA1981_TXTAG_SIZE              2
/* Bit: 'RA1981_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_13_12_RA1981_ACT_OFFSET              0x99
#define TANTOS_3G_RA_13_12_RA1981_ACT_SHIFT               8
#define TANTOS_3G_RA_13_12_RA1981_ACT_SIZE                2
/* Bit: 'RA1980_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_13_12_RA1980_VALID_OFFSET            0x99
#define TANTOS_3G_RA_13_12_RA1980_VALID_SHIFT             7
#define TANTOS_3G_RA_13_12_RA1980_VALID_SIZE              1
/* Bit: 'RA1980_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_13_12_RA1980_SPAN_OFFSET             0x99
#define TANTOS_3G_RA_13_12_RA1980_SPAN_SHIFT              6
#define TANTOS_3G_RA_13_12_RA1980_SPAN_SIZE               1
/* Bit: 'RA1980_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_13_12_RA1980_MG_OFFSET               0x99
#define TANTOS_3G_RA_13_12_RA1980_MG_SHIFT                5
#define TANTOS_3G_RA_13_12_RA1980_MG_SIZE                 1
/* Bit: 'RA1980_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_13_12_RA1980_CV_OFFSET               0x99
#define TANTOS_3G_RA_13_12_RA1980_CV_SHIFT                4
#define TANTOS_3G_RA_13_12_RA1980_CV_SIZE                 1
/* Bit: 'RA1980_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_13_12_RA1980_TXTAG_OFFSET            0x99
#define TANTOS_3G_RA_13_12_RA1980_TXTAG_SHIFT             2
#define TANTOS_3G_RA_13_12_RA1980_TXTAG_SIZE              2
/* Bit: 'RA1980_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_13_12_RA1980_ACT_OFFSET              0x99
#define TANTOS_3G_RA_13_12_RA1980_ACT_SHIFT               0
#define TANTOS_3G_RA_13_12_RA1980_ACT_SIZE                2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000015~0180C2000014' */
/* Bit: 'RA2021_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_15_14_RA2021_VALID_OFFSET            0x9A
#define TANTOS_3G_RA_15_14_RA2021_VALID_SHIFT             15
#define TANTOS_3G_RA_15_14_RA2021_VALID_SIZE              1
/* Bit: 'RA2021_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_15_14_RA2021_SPAN_OFFSET             0x9A
#define TANTOS_3G_RA_15_14_RA2021_SPAN_SHIFT              14
#define TANTOS_3G_RA_15_14_RA2021_SPAN_SIZE               1
/* Bit: 'RA2021_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_15_14_RA2021_MG_OFFSET               0x9A
#define TANTOS_3G_RA_15_14_RA2021_MG_SHIFT                13
#define TANTOS_3G_RA_15_14_RA2021_MG_SIZE                 1
/* Bit: 'RA2021_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_15_14_RA2021_CV_OFFSET               0x9A
#define TANTOS_3G_RA_15_14_RA2021_CV_SHIFT                12
#define TANTOS_3G_RA_15_14_RA2021_CV_SIZE                 1
/* Bit: 'RA2021_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_15_14_RA2021_TXTAG_OFFSET            0x9A
#define TANTOS_3G_RA_15_14_RA2021_TXTAG_SHIFT             10
#define TANTOS_3G_RA_15_14_RA2021_TXTAG_SIZE              2
/* Bit: 'RA2021_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_15_14_RA2021_ACT_OFFSET              0x9A
#define TANTOS_3G_RA_15_14_RA2021_ACT_SHIFT               8
#define TANTOS_3G_RA_15_14_RA2021_ACT_SIZE                2
/* Bit: 'RA200_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_15_14_RA200_VALID_OFFSET             0x9A
#define TANTOS_3G_RA_15_14_RA200_VALID_SHIFT              7
#define TANTOS_3G_RA_15_14_RA200_VALID_SIZE               1
/* Bit: 'RA200_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_15_14_RA200_SPAN_OFFSET              0x9A
#define TANTOS_3G_RA_15_14_RA200_SPAN_SHIFT               6
#define TANTOS_3G_RA_15_14_RA200_SPAN_SIZE                1
/* Bit: 'RA200_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_15_14_RA200_MG_OFFSET                0x9A
#define TANTOS_3G_RA_15_14_RA200_MG_SHIFT                 5
#define TANTOS_3G_RA_15_14_RA200_MG_SIZE                  1
/* Bit: 'RA200_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_15_14_RA200_CV_OFFSET                0x9A
#define TANTOS_3G_RA_15_14_RA200_CV_SHIFT                 4
#define TANTOS_3G_RA_15_14_RA200_CV_SIZE                  1
/* Bit: 'RA200_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_15_14_RA200_TXTAG_OFFSET             0x9A
#define TANTOS_3G_RA_15_14_RA200_TXTAG_SHIFT              2
#define TANTOS_3G_RA_15_14_RA200_TXTAG_SIZE               2
/* Bit: 'RA200_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_15_14_RA200_ACT_OFFSET               0x9A
#define TANTOS_3G_RA_15_14_RA200_ACT_SHIFT                0
#define TANTOS_3G_RA_15_14_RA200_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000017~0180C2000016' */
/* Bit: 'RA2223_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_17_16_RA2223_VALID_OFFSET            0x9B
#define TANTOS_3G_RA_17_16_RA2223_VALID_SHIFT             15
#define TANTOS_3G_RA_17_16_RA2223_VALID_SIZE              1
/* Bit: 'RA2223_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_17_16_RA2223_SPAN_OFFSET             0x9B
#define TANTOS_3G_RA_17_16_RA2223_SPAN_SHIFT              14
#define TANTOS_3G_RA_17_16_RA2223_SPAN_SIZE               1
/* Bit: 'RA2223_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_17_16_RA2223_MG_OFFSET               0x9B
#define TANTOS_3G_RA_17_16_RA2223_MG_SHIFT                13
#define TANTOS_3G_RA_17_16_RA2223_MG_SIZE                 1
/* Bit: 'RA2223_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_17_16_RA2223_CV_OFFSET               0x9B
#define TANTOS_3G_RA_17_16_RA2223_CV_SHIFT                12
#define TANTOS_3G_RA_17_16_RA2223_CV_SIZE                 1
/* Bit: 'RA2223_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_17_16_RA2223_TXTAG_OFFSET            0x9B
#define TANTOS_3G_RA_17_16_RA2223_TXTAG_SHIFT             10
#define TANTOS_3G_RA_17_16_RA2223_TXTAG_SIZE              2
/* Bit: 'RA2223_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_17_16_RA2223_ACT_OFFSET              0x9B
#define TANTOS_3G_RA_17_16_RA2223_ACT_SHIFT               8
#define TANTOS_3G_RA_17_16_RA2223_ACT_SIZE                2
/* Bit: 'RA220_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_17_16_RA220_VALID_OFFSET             0x9B
#define TANTOS_3G_RA_17_16_RA220_VALID_SHIFT              7
#define TANTOS_3G_RA_17_16_RA220_VALID_SIZE               1
/* Bit: 'RA220_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_17_16_RA220_SPAN_OFFSET              0x9B
#define TANTOS_3G_RA_17_16_RA220_SPAN_SHIFT               6
#define TANTOS_3G_RA_17_16_RA220_SPAN_SIZE                1
/* Bit: 'RA220_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_17_16_RA220_MG_OFFSET                0x9B
#define TANTOS_3G_RA_17_16_RA220_MG_SHIFT                 5
#define TANTOS_3G_RA_17_16_RA220_MG_SIZE                  1
/* Bit: 'RA220_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_17_16_RA220_CV_OFFSET                0x9B
#define TANTOS_3G_RA_17_16_RA220_CV_SHIFT                 4
#define TANTOS_3G_RA_17_16_RA220_CV_SIZE                  1
/* Bit: 'RA220_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_17_16_RA220_TXTAG_OFFSET             0x9B
#define TANTOS_3G_RA_17_16_RA220_TXTAG_SHIFT              2
#define TANTOS_3G_RA_17_16_RA220_TXTAG_SIZE               2
/* Bit: 'RA220_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_17_16_RA220_ACT_OFFSET               0x9B
#define TANTOS_3G_RA_17_16_RA220_ACT_SHIFT                0
#define TANTOS_3G_RA_17_16_RA220_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000019~0180C2000018' */
/* Bit: 'RA2425_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_19_18_RA2425_VALID_OFFSET            0x9C
#define TANTOS_3G_RA_19_18_RA2425_VALID_SHIFT             15
#define TANTOS_3G_RA_19_18_RA2425_VALID_SIZE              1
/* Bit: 'RA2425_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_19_18_RA2425_SPAN_OFFSET             0x9C
#define TANTOS_3G_RA_19_18_RA2425_SPAN_SHIFT              14
#define TANTOS_3G_RA_19_18_RA2425_SPAN_SIZE               1
/* Bit: 'RA2425_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_19_18_RA2425_MG_OFFSET               0x9C
#define TANTOS_3G_RA_19_18_RA2425_MG_SHIFT                13
#define TANTOS_3G_RA_19_18_RA2425_MG_SIZE                 1
/* Bit: 'RA2425_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_19_18_RA2425_CV_OFFSET               0x9C
#define TANTOS_3G_RA_19_18_RA2425_CV_SHIFT                12
#define TANTOS_3G_RA_19_18_RA2425_CV_SIZE                 1
/* Bit: 'RA2425_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_19_18_RA2425_TXTAG_OFFSET            0x9C
#define TANTOS_3G_RA_19_18_RA2425_TXTAG_SHIFT             10
#define TANTOS_3G_RA_19_18_RA2425_TXTAG_SIZE              2
/* Bit: 'RA2425_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_19_18_RA2425_ACT_OFFSET              0x9C
#define TANTOS_3G_RA_19_18_RA2425_ACT_SHIFT               8
#define TANTOS_3G_RA_19_18_RA2425_ACT_SIZE                2
/* Bit: 'RA240_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_19_18_RA240_VALID_OFFSET             0x9C
#define TANTOS_3G_RA_19_18_RA240_VALID_SHIFT              7
#define TANTOS_3G_RA_19_18_RA240_VALID_SIZE               1
/* Bit: 'RA240_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_19_18_RA240_SPAN_OFFSET              0x9C
#define TANTOS_3G_RA_19_18_RA240_SPAN_SHIFT               6
#define TANTOS_3G_RA_19_18_RA240_SPAN_SIZE                1
/* Bit: 'RA240_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_19_18_RA240_MG_OFFSET                0x9C
#define TANTOS_3G_RA_19_18_RA240_MG_SHIFT                 5
#define TANTOS_3G_RA_19_18_RA240_MG_SIZE                  1
/* Bit: 'RA240_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_19_18_RA240_CV_OFFSET                0x9C
#define TANTOS_3G_RA_19_18_RA240_CV_SHIFT                 4
#define TANTOS_3G_RA_19_18_RA240_CV_SIZE                  1
/* Bit: 'RA240_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_19_18_RA240_TXTAG_OFFSET             0x9C
#define TANTOS_3G_RA_19_18_RA240_TXTAG_SHIFT              2
#define TANTOS_3G_RA_19_18_RA240_TXTAG_SIZE               2
/* Bit: 'RA240_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_19_18_RA240_ACT_OFFSET               0x9C
#define TANTOS_3G_RA_19_18_RA240_ACT_SHIFT                0
#define TANTOS_3G_RA_19_18_RA240_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200001B~0180C200001A' */
/* Bit: 'RA2627_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_1B_1A_RA2627_VALID_OFFSET            0x9D
#define TANTOS_3G_RA_1B_1A_RA2627_VALID_SHIFT             15
#define TANTOS_3G_RA_1B_1A_RA2627_VALID_SIZE              1
/* Bit: 'RA2627_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_1B_1A_RA2627_SPAN_OFFSET             0x9D
#define TANTOS_3G_RA_1B_1A_RA2627_SPAN_SHIFT              14
#define TANTOS_3G_RA_1B_1A_RA2627_SPAN_SIZE               1
/* Bit: 'RA2627_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_1B_1A_RA2627_MG_OFFSET               0x9D
#define TANTOS_3G_RA_1B_1A_RA2627_MG_SHIFT                13
#define TANTOS_3G_RA_1B_1A_RA2627_MG_SIZE                 1
/* Bit: 'RA2627_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_1B_1A_RA2627_CV_OFFSET               0x9D
#define TANTOS_3G_RA_1B_1A_RA2627_CV_SHIFT                12
#define TANTOS_3G_RA_1B_1A_RA2627_CV_SIZE                 1
/* Bit: 'RA2627_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_1B_1A_RA2627_TXTAG_OFFSET            0x9D
#define TANTOS_3G_RA_1B_1A_RA2627_TXTAG_SHIFT             10
#define TANTOS_3G_RA_1B_1A_RA2627_TXTAG_SIZE              2
/* Bit: 'RA2627_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_1B_1A_RA2627_ACT_OFFSET              0x9D
#define TANTOS_3G_RA_1B_1A_RA2627_ACT_SHIFT               8
#define TANTOS_3G_RA_1B_1A_RA2627_ACT_SIZE                2
/* Bit: 'RA260_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_1B_1A_RA260_VALID_OFFSET             0x9D
#define TANTOS_3G_RA_1B_1A_RA260_VALID_SHIFT              7
#define TANTOS_3G_RA_1B_1A_RA260_VALID_SIZE               1
/* Bit: 'RA260_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_1B_1A_RA260_SPAN_OFFSET              0x9D
#define TANTOS_3G_RA_1B_1A_RA260_SPAN_SHIFT               6
#define TANTOS_3G_RA_1B_1A_RA260_SPAN_SIZE                1
/* Bit: 'RA260_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_1B_1A_RA260_MG_OFFSET                0x9D
#define TANTOS_3G_RA_1B_1A_RA260_MG_SHIFT                 5
#define TANTOS_3G_RA_1B_1A_RA260_MG_SIZE                  1
/* Bit: 'RA260_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_1B_1A_RA260_CV_OFFSET                0x9D
#define TANTOS_3G_RA_1B_1A_RA260_CV_SHIFT                 4
#define TANTOS_3G_RA_1B_1A_RA260_CV_SIZE                  1
/* Bit: 'RA260_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_1B_1A_RA260_TXTAG_OFFSET             0x9D
#define TANTOS_3G_RA_1B_1A_RA260_TXTAG_SHIFT              2
#define TANTOS_3G_RA_1B_1A_RA260_TXTAG_SIZE               2
/* Bit: 'RA260_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_1B_1A_RA260_ACT_OFFSET               0x9D
#define TANTOS_3G_RA_1B_1A_RA260_ACT_SHIFT                0
#define TANTOS_3G_RA_1B_1A_RA260_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200001D~0180C200001C' */
/* Bit: 'RA2829_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_1D_1C_RA2829_VALID_OFFSET            0x9E
#define TANTOS_3G_RA_1D_1C_RA2829_VALID_SHIFT             15
#define TANTOS_3G_RA_1D_1C_RA2829_VALID_SIZE              1
/* Bit: 'RA2829_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_1D_1C_RA2829_SPAN_OFFSET             0x9E
#define TANTOS_3G_RA_1D_1C_RA2829_SPAN_SHIFT              14
#define TANTOS_3G_RA_1D_1C_RA2829_SPAN_SIZE               1
/* Bit: 'RA2829_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_1D_1C_RA2829_MG_OFFSET               0x9E
#define TANTOS_3G_RA_1D_1C_RA2829_MG_SHIFT                13
#define TANTOS_3G_RA_1D_1C_RA2829_MG_SIZE                 1
/* Bit: 'RA2829_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_1D_1C_RA2829_CV_OFFSET               0x9E
#define TANTOS_3G_RA_1D_1C_RA2829_CV_SHIFT                12
#define TANTOS_3G_RA_1D_1C_RA2829_CV_SIZE                 1
/* Bit: 'RA2829_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_1D_1C_RA2829_TXTAG_OFFSET            0x9E
#define TANTOS_3G_RA_1D_1C_RA2829_TXTAG_SHIFT             10
#define TANTOS_3G_RA_1D_1C_RA2829_TXTAG_SIZE              2
/* Bit: 'RA2829_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_1D_1C_RA2829_ACT_OFFSET              0x9E
#define TANTOS_3G_RA_1D_1C_RA2829_ACT_SHIFT               8
#define TANTOS_3G_RA_1D_1C_RA2829_ACT_SIZE                2
/* Bit: 'RA280_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_1D_1C_RA280_VALID_OFFSET             0x9E
#define TANTOS_3G_RA_1D_1C_RA280_VALID_SHIFT              7
#define TANTOS_3G_RA_1D_1C_RA280_VALID_SIZE               1
/* Bit: 'RA280_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_1D_1C_RA280_SPAN_OFFSET              0x9E
#define TANTOS_3G_RA_1D_1C_RA280_SPAN_SHIFT               6
#define TANTOS_3G_RA_1D_1C_RA280_SPAN_SIZE                1
/* Bit: 'RA280_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_1D_1C_RA280_MG_OFFSET                0x9E
#define TANTOS_3G_RA_1D_1C_RA280_MG_SHIFT                 5
#define TANTOS_3G_RA_1D_1C_RA280_MG_SIZE                  1
/* Bit: 'RA280_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_1D_1C_RA280_CV_OFFSET                0x9E
#define TANTOS_3G_RA_1D_1C_RA280_CV_SHIFT                 4
#define TANTOS_3G_RA_1D_1C_RA280_CV_SIZE                  1
/* Bit: 'RA280_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_1D_1C_RA280_TXTAG_OFFSET             0x9E
#define TANTOS_3G_RA_1D_1C_RA280_TXTAG_SHIFT              2
#define TANTOS_3G_RA_1D_1C_RA280_TXTAG_SIZE               2
/* Bit: 'RA280_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_1D_1C_RA280_ACT_OFFSET               0x9E
#define TANTOS_3G_RA_1D_1C_RA280_ACT_SHIFT                0
#define TANTOS_3G_RA_1D_1C_RA280_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200001F~0180C200001E' */
/* Bit: 'RA3031_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_1F_1E_RA3031_VALID_OFFSET            0x9F
#define TANTOS_3G_RA_1F_1E_RA3031_VALID_SHIFT             15
#define TANTOS_3G_RA_1F_1E_RA3031_VALID_SIZE              1
/* Bit: 'RA3031_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_1F_1E_RA3031_SPAN_OFFSET             0x9F
#define TANTOS_3G_RA_1F_1E_RA3031_SPAN_SHIFT              14
#define TANTOS_3G_RA_1F_1E_RA3031_SPAN_SIZE               1
/* Bit: 'RA3031_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_1F_1E_RA3031_MG_OFFSET               0x9F
#define TANTOS_3G_RA_1F_1E_RA3031_MG_SHIFT                13
#define TANTOS_3G_RA_1F_1E_RA3031_MG_SIZE                 1
/* Bit: 'RA3031_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_1F_1E_RA3031_CV_OFFSET               0x9F
#define TANTOS_3G_RA_1F_1E_RA3031_CV_SHIFT                12
#define TANTOS_3G_RA_1F_1E_RA3031_CV_SIZE                 1
/* Bit: 'RA3031_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_1F_1E_RA3031_TXTAG_OFFSET            0x9F
#define TANTOS_3G_RA_1F_1E_RA3031_TXTAG_SHIFT             10
#define TANTOS_3G_RA_1F_1E_RA3031_TXTAG_SIZE              2
/* Bit: 'RA3031_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_1F_1E_RA3031_ACT_OFFSET              0x9F
#define TANTOS_3G_RA_1F_1E_RA3031_ACT_SHIFT               8
#define TANTOS_3G_RA_1F_1E_RA3031_ACT_SIZE                2
/* Bit: 'RA300_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_1F_1E_RA300_VALID_OFFSET             0x9F
#define TANTOS_3G_RA_1F_1E_RA300_VALID_SHIFT              7
#define TANTOS_3G_RA_1F_1E_RA300_VALID_SIZE               1
/* Bit: 'RA300_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_1F_1E_RA300_SPAN_OFFSET              0x9F
#define TANTOS_3G_RA_1F_1E_RA300_SPAN_SHIFT               6
#define TANTOS_3G_RA_1F_1E_RA300_SPAN_SIZE                1
/* Bit: 'RA300_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_1F_1E_RA300_MG_OFFSET                0x9F
#define TANTOS_3G_RA_1F_1E_RA300_MG_SHIFT                 5
#define TANTOS_3G_RA_1F_1E_RA300_MG_SIZE                  1
/* Bit: 'RA300_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_1F_1E_RA300_CV_OFFSET                0x9F
#define TANTOS_3G_RA_1F_1E_RA300_CV_SHIFT                 4
#define TANTOS_3G_RA_1F_1E_RA300_CV_SIZE                  1
/* Bit: 'RA300_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_1F_1E_RA300_TXTAG_OFFSET             0x9F
#define TANTOS_3G_RA_1F_1E_RA300_TXTAG_SHIFT              2
#define TANTOS_3G_RA_1F_1E_RA300_TXTAG_SIZE               2
/* Bit: 'RA300_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_1F_1E_RA300_ACT_OFFSET               0x9F
#define TANTOS_3G_RA_1F_1E_RA300_ACT_SHIFT                0
#define TANTOS_3G_RA_1F_1E_RA300_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000021~0180C2000020' */
/* Bit: 'RA3233_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_21_20_RA3233_VALID_OFFSET            0xB0
#define TANTOS_3G_RA_21_20_RA3233_VALID_SHIFT             15
#define TANTOS_3G_RA_21_20_RA3233_VALID_SIZE              1
/* Bit: 'RA3233_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_21_20_RA3233_SPAN_OFFSET             0xB0
#define TANTOS_3G_RA_21_20_RA3233_SPAN_SHIFT              14
#define TANTOS_3G_RA_21_20_RA3233_SPAN_SIZE               1
/* Bit: 'RA3233_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_21_20_RA3233_MG_OFFSET               0xB0
#define TANTOS_3G_RA_21_20_RA3233_MG_SHIFT                13
#define TANTOS_3G_RA_21_20_RA3233_MG_SIZE                 1
/* Bit: 'RA3233_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_21_20_RA3233_CV_OFFSET               0xB0
#define TANTOS_3G_RA_21_20_RA3233_CV_SHIFT                12
#define TANTOS_3G_RA_21_20_RA3233_CV_SIZE                 1
/* Bit: 'RA3233_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_21_20_RA3233_TXTAG_OFFSET            0xB0
#define TANTOS_3G_RA_21_20_RA3233_TXTAG_SHIFT             10
#define TANTOS_3G_RA_21_20_RA3233_TXTAG_SIZE              2
/* Bit: 'RA3233_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_21_20_RA3233_ACT_OFFSET              0xB0
#define TANTOS_3G_RA_21_20_RA3233_ACT_SHIFT               8
#define TANTOS_3G_RA_21_20_RA3233_ACT_SIZE                2
/* Bit: 'RA320_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_21_20_RA320_VALID_OFFSET             0xB0
#define TANTOS_3G_RA_21_20_RA320_VALID_SHIFT              7
#define TANTOS_3G_RA_21_20_RA320_VALID_SIZE               1
/* Bit: 'RA320_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_21_20_RA320_SPAN_OFFSET              0xB0
#define TANTOS_3G_RA_21_20_RA320_SPAN_SHIFT               6
#define TANTOS_3G_RA_21_20_RA320_SPAN_SIZE                1
/* Bit: 'RA320_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_21_20_RA320_MG_OFFSET                0xB0
#define TANTOS_3G_RA_21_20_RA320_MG_SHIFT                 5
#define TANTOS_3G_RA_21_20_RA320_MG_SIZE                  1
/* Bit: 'RA320_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_21_20_RA320_CV_OFFSET                0xB0
#define TANTOS_3G_RA_21_20_RA320_CV_SHIFT                 4
#define TANTOS_3G_RA_21_20_RA320_CV_SIZE                  1
/* Bit: 'RA320_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_21_20_RA320_TXTAG_OFFSET             0xB0
#define TANTOS_3G_RA_21_20_RA320_TXTAG_SHIFT              2
#define TANTOS_3G_RA_21_20_RA320_TXTAG_SIZE               2
/* Bit: 'RA320_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_21_20_RA320_ACT_OFFSET               0xB0
#define TANTOS_3G_RA_21_20_RA320_ACT_SHIFT                0
#define TANTOS_3G_RA_21_20_RA320_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000023~0180C2000022' */
/* Bit: 'RA3435_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_23_22_RA3435_VALID_OFFSET            0xB1
#define TANTOS_3G_RA_23_22_RA3435_VALID_SHIFT             15
#define TANTOS_3G_RA_23_22_RA3435_VALID_SIZE              1
/* Bit: 'RA3435_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_23_22_RA3435_SPAN_OFFSET             0xB1
#define TANTOS_3G_RA_23_22_RA3435_SPAN_SHIFT              14
#define TANTOS_3G_RA_23_22_RA3435_SPAN_SIZE               1
/* Bit: 'RA3435_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_23_22_RA3435_MG_OFFSET               0xB1
#define TANTOS_3G_RA_23_22_RA3435_MG_SHIFT                13
#define TANTOS_3G_RA_23_22_RA3435_MG_SIZE                 1
/* Bit: 'RA3435_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_23_22_RA3435_CV_OFFSET               0xB1
#define TANTOS_3G_RA_23_22_RA3435_CV_SHIFT                12
#define TANTOS_3G_RA_23_22_RA3435_CV_SIZE                 1
/* Bit: 'RA3435_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_23_22_RA3435_TXTAG_OFFSET            0xB1
#define TANTOS_3G_RA_23_22_RA3435_TXTAG_SHIFT             10
#define TANTOS_3G_RA_23_22_RA3435_TXTAG_SIZE              2
/* Bit: 'RA3435_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_23_22_RA3435_ACT_OFFSET              0xB1
#define TANTOS_3G_RA_23_22_RA3435_ACT_SHIFT               8
#define TANTOS_3G_RA_23_22_RA3435_ACT_SIZE                2
/* Bit: 'RA340_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_23_22_RA340_VALID_OFFSET             0xB1
#define TANTOS_3G_RA_23_22_RA340_VALID_SHIFT              7
#define TANTOS_3G_RA_23_22_RA340_VALID_SIZE               1
/* Bit: 'RA340_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_23_22_RA340_SPAN_OFFSET              0xB1
#define TANTOS_3G_RA_23_22_RA340_SPAN_SHIFT               6
#define TANTOS_3G_RA_23_22_RA340_SPAN_SIZE                1
/* Bit: 'RA340_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_23_22_RA340_MG_OFFSET                0xB1
#define TANTOS_3G_RA_23_22_RA340_MG_SHIFT                 5
#define TANTOS_3G_RA_23_22_RA340_MG_SIZE                  1
/* Bit: 'RA340_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_23_22_RA340_CV_OFFSET                0xB1
#define TANTOS_3G_RA_23_22_RA340_CV_SHIFT                 4
#define TANTOS_3G_RA_23_22_RA340_CV_SIZE                  1
/* Bit: 'RA340_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_23_22_RA340_TXTAG_OFFSET             0xB1
#define TANTOS_3G_RA_23_22_RA340_TXTAG_SHIFT              2
#define TANTOS_3G_RA_23_22_RA340_TXTAG_SIZE               2
/* Bit: 'RA340_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_23_22_RA340_ACT_OFFSET               0xB1
#define TANTOS_3G_RA_23_22_RA340_ACT_SHIFT                0
#define TANTOS_3G_RA_23_22_RA340_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000025~0180C2000024' */
/* Bit: 'RA3637_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_25_24_RA3637_VALID_OFFSET            0xB2
#define TANTOS_3G_RA_25_24_RA3637_VALID_SHIFT             15
#define TANTOS_3G_RA_25_24_RA3637_VALID_SIZE              1
/* Bit: 'RA3637_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_25_24_RA3637_SPAN_OFFSET             0xB2
#define TANTOS_3G_RA_25_24_RA3637_SPAN_SHIFT              14
#define TANTOS_3G_RA_25_24_RA3637_SPAN_SIZE               1
/* Bit: 'RA3637_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_25_24_RA3637_MG_OFFSET               0xB2
#define TANTOS_3G_RA_25_24_RA3637_MG_SHIFT                13
#define TANTOS_3G_RA_25_24_RA3637_MG_SIZE                 1
/* Bit: 'RA3637_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_25_24_RA3637_CV_OFFSET               0xB2
#define TANTOS_3G_RA_25_24_RA3637_CV_SHIFT                12
#define TANTOS_3G_RA_25_24_RA3637_CV_SIZE                 1
/* Bit: 'RA3637_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_25_24_RA3637_TXTAG_OFFSET            0xB2
#define TANTOS_3G_RA_25_24_RA3637_TXTAG_SHIFT             10
#define TANTOS_3G_RA_25_24_RA3637_TXTAG_SIZE              2
/* Bit: 'RA3637_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_25_24_RA3637_ACT_OFFSET              0xB2
#define TANTOS_3G_RA_25_24_RA3637_ACT_SHIFT               8
#define TANTOS_3G_RA_25_24_RA3637_ACT_SIZE                2
/* Bit: 'RA360_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_25_24_RA360_VALID_OFFSET             0xB2
#define TANTOS_3G_RA_25_24_RA360_VALID_SHIFT              7
#define TANTOS_3G_RA_25_24_RA360_VALID_SIZE               1
/* Bit: 'RA360_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_25_24_RA360_SPAN_OFFSET              0xB2
#define TANTOS_3G_RA_25_24_RA360_SPAN_SHIFT               6
#define TANTOS_3G_RA_25_24_RA360_SPAN_SIZE                1
/* Bit: 'RA360_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_25_24_RA360_MG_OFFSET                0xB2
#define TANTOS_3G_RA_25_24_RA360_MG_SHIFT                 5
#define TANTOS_3G_RA_25_24_RA360_MG_SIZE                  1
/* Bit: 'RA360_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_25_24_RA360_CV_OFFSET                0xB2
#define TANTOS_3G_RA_25_24_RA360_CV_SHIFT                 4
#define TANTOS_3G_RA_25_24_RA360_CV_SIZE                  1
/* Bit: 'RA360_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_25_24_RA360_TXTAG_OFFSET             0xB2
#define TANTOS_3G_RA_25_24_RA360_TXTAG_SHIFT              2
#define TANTOS_3G_RA_25_24_RA360_TXTAG_SIZE               2
/* Bit: 'RA360_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_25_24_RA360_ACT_OFFSET               0xB2
#define TANTOS_3G_RA_25_24_RA360_ACT_SHIFT                0
#define TANTOS_3G_RA_25_24_RA360_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000027~0180C2000026' */
/* Bit: 'RA3839_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_27_26_RA3839_VALID_OFFSET            0xB3
#define TANTOS_3G_RA_27_26_RA3839_VALID_SHIFT             15
#define TANTOS_3G_RA_27_26_RA3839_VALID_SIZE              1
/* Bit: 'RA3839_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_27_26_RA3839_SPAN_OFFSET             0xB3
#define TANTOS_3G_RA_27_26_RA3839_SPAN_SHIFT              14
#define TANTOS_3G_RA_27_26_RA3839_SPAN_SIZE               1
/* Bit: 'RA3839_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_27_26_RA3839_MG_OFFSET               0xB3
#define TANTOS_3G_RA_27_26_RA3839_MG_SHIFT                13
#define TANTOS_3G_RA_27_26_RA3839_MG_SIZE                 1
/* Bit: 'RA3839_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_27_26_RA3839_CV_OFFSET               0xB3
#define TANTOS_3G_RA_27_26_RA3839_CV_SHIFT                12
#define TANTOS_3G_RA_27_26_RA3839_CV_SIZE                 1
/* Bit: 'RA3839_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_27_26_RA3839_TXTAG_OFFSET            0xB3
#define TANTOS_3G_RA_27_26_RA3839_TXTAG_SHIFT             10
#define TANTOS_3G_RA_27_26_RA3839_TXTAG_SIZE              2
/* Bit: 'RA3839_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_27_26_RA3839_ACT_OFFSET              0xB3
#define TANTOS_3G_RA_27_26_RA3839_ACT_SHIFT               8
#define TANTOS_3G_RA_27_26_RA3839_ACT_SIZE                2
/* Bit: 'RA380_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_27_26_RA380_VALID_OFFSET             0xB3
#define TANTOS_3G_RA_27_26_RA380_VALID_SHIFT              7
#define TANTOS_3G_RA_27_26_RA380_VALID_SIZE               1
/* Bit: 'RA380_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_27_26_RA380_SPAN_OFFSET              0xB3
#define TANTOS_3G_RA_27_26_RA380_SPAN_SHIFT               6
#define TANTOS_3G_RA_27_26_RA380_SPAN_SIZE                1
/* Bit: 'RA380_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_27_26_RA380_MG_OFFSET                0xB3
#define TANTOS_3G_RA_27_26_RA380_MG_SHIFT                 5
#define TANTOS_3G_RA_27_26_RA380_MG_SIZE                  1
/* Bit: 'RA380_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_27_26_RA380_CV_OFFSET                0xB3
#define TANTOS_3G_RA_27_26_RA380_CV_SHIFT                 4
#define TANTOS_3G_RA_27_26_RA380_CV_SIZE                  1
/* Bit: 'RA380_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_27_26_RA380_TXTAG_OFFSET             0xB3
#define TANTOS_3G_RA_27_26_RA380_TXTAG_SHIFT              2
#define TANTOS_3G_RA_27_26_RA380_TXTAG_SIZE               2
/* Bit: 'RA380_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_27_26_RA380_ACT_OFFSET               0xB3
#define TANTOS_3G_RA_27_26_RA380_ACT_SHIFT                0
#define TANTOS_3G_RA_27_26_RA380_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C2000029~0180C2000028' */
/* Bit: 'RA4041_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_29_28_RA4041_VALID_OFFSET            0xB4
#define TANTOS_3G_RA_29_28_RA4041_VALID_SHIFT             15
#define TANTOS_3G_RA_29_28_RA4041_VALID_SIZE              1
/* Bit: 'RA4041_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_29_28_RA4041_SPAN_OFFSET             0xB4
#define TANTOS_3G_RA_29_28_RA4041_SPAN_SHIFT              14
#define TANTOS_3G_RA_29_28_RA4041_SPAN_SIZE               1
/* Bit: 'RA4041_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_29_28_RA4041_MG_OFFSET               0xB4
#define TANTOS_3G_RA_29_28_RA4041_MG_SHIFT                13
#define TANTOS_3G_RA_29_28_RA4041_MG_SIZE                 1
/* Bit: 'RA4041_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_29_28_RA4041_CV_OFFSET               0xB4
#define TANTOS_3G_RA_29_28_RA4041_CV_SHIFT                12
#define TANTOS_3G_RA_29_28_RA4041_CV_SIZE                 1
/* Bit: 'RA4041_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_29_28_RA4041_TXTAG_OFFSET            0xB4
#define TANTOS_3G_RA_29_28_RA4041_TXTAG_SHIFT             10
#define TANTOS_3G_RA_29_28_RA4041_TXTAG_SIZE              2
/* Bit: 'RA4041_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_29_28_RA4041_ACT_OFFSET              0xB4
#define TANTOS_3G_RA_29_28_RA4041_ACT_SHIFT               8
#define TANTOS_3G_RA_29_28_RA4041_ACT_SIZE                2
/* Bit: 'RA400_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_29_28_RA400_VALID_OFFSET             0xB4
#define TANTOS_3G_RA_29_28_RA400_VALID_SHIFT              7
#define TANTOS_3G_RA_29_28_RA400_VALID_SIZE               1
/* Bit: 'RA400_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_29_28_RA400_SPAN_OFFSET              0xB4
#define TANTOS_3G_RA_29_28_RA400_SPAN_SHIFT               6
#define TANTOS_3G_RA_29_28_RA400_SPAN_SIZE                1
/* Bit: 'RA400_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_29_28_RA400_MG_OFFSET                0xB4
#define TANTOS_3G_RA_29_28_RA400_MG_SHIFT                 5
#define TANTOS_3G_RA_29_28_RA400_MG_SIZE                  1
/* Bit: 'RA400_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_29_28_RA400_CV_OFFSET                0xB4
#define TANTOS_3G_RA_29_28_RA400_CV_SHIFT                 4
#define TANTOS_3G_RA_29_28_RA400_CV_SIZE                  1
/* Bit: 'RA400_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_29_28_RA400_TXTAG_OFFSET             0xB4
#define TANTOS_3G_RA_29_28_RA400_TXTAG_SHIFT              2
#define TANTOS_3G_RA_29_28_RA400_TXTAG_SIZE               2
/* Bit: 'RA400_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_29_28_RA400_ACT_OFFSET               0xB4
#define TANTOS_3G_RA_29_28_RA400_ACT_SHIFT                0
#define TANTOS_3G_RA_29_28_RA400_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200002B~0180C200002A' */
/* Bit: 'RA4243_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_2B_2A_RA4243_VALID_OFFSET            0xB5
#define TANTOS_3G_RA_2B_2A_RA4243_VALID_SHIFT             15
#define TANTOS_3G_RA_2B_2A_RA4243_VALID_SIZE              1
/* Bit: 'RA4243_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_2B_2A_RA4243_SPAN_OFFSET             0xB5
#define TANTOS_3G_RA_2B_2A_RA4243_SPAN_SHIFT              14
#define TANTOS_3G_RA_2B_2A_RA4243_SPAN_SIZE               1
/* Bit: 'RA4243_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_2B_2A_RA4243_MG_OFFSET               0xB5
#define TANTOS_3G_RA_2B_2A_RA4243_MG_SHIFT                13
#define TANTOS_3G_RA_2B_2A_RA4243_MG_SIZE                 1
/* Bit: 'RA4243_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_2B_2A_RA4243_CV_OFFSET               0xB5
#define TANTOS_3G_RA_2B_2A_RA4243_CV_SHIFT                12
#define TANTOS_3G_RA_2B_2A_RA4243_CV_SIZE                 1
/* Bit: 'RA4243_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_2B_2A_RA4243_TXTAG_OFFSET            0xB5
#define TANTOS_3G_RA_2B_2A_RA4243_TXTAG_SHIFT             10
#define TANTOS_3G_RA_2B_2A_RA4243_TXTAG_SIZE              2
/* Bit: 'RA4243_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_2B_2A_RA4243_ACT_OFFSET              0xB5
#define TANTOS_3G_RA_2B_2A_RA4243_ACT_SHIFT               8
#define TANTOS_3G_RA_2B_2A_RA4243_ACT_SIZE                2
/* Bit: 'RA420_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_2B_2A_RA420_VALID_OFFSET             0xB5
#define TANTOS_3G_RA_2B_2A_RA420_VALID_SHIFT              7
#define TANTOS_3G_RA_2B_2A_RA420_VALID_SIZE               1
/* Bit: 'RA420_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_2B_2A_RA420_SPAN_OFFSET              0xB5
#define TANTOS_3G_RA_2B_2A_RA420_SPAN_SHIFT               6
#define TANTOS_3G_RA_2B_2A_RA420_SPAN_SIZE                1
/* Bit: 'RA420_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_2B_2A_RA420_MG_OFFSET                0xB5
#define TANTOS_3G_RA_2B_2A_RA420_MG_SHIFT                 5
#define TANTOS_3G_RA_2B_2A_RA420_MG_SIZE                  1
/* Bit: 'RA420_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_2B_2A_RA420_CV_OFFSET                0xB5
#define TANTOS_3G_RA_2B_2A_RA420_CV_SHIFT                 4
#define TANTOS_3G_RA_2B_2A_RA420_CV_SIZE                  1
/* Bit: 'RA420_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_2B_2A_RA420_TXTAG_OFFSET             0xB5
#define TANTOS_3G_RA_2B_2A_RA420_TXTAG_SHIFT              2
#define TANTOS_3G_RA_2B_2A_RA420_TXTAG_SIZE               2
/* Bit: 'RA420_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_2B_2A_RA420_ACT_OFFSET               0xB5
#define TANTOS_3G_RA_2B_2A_RA420_ACT_SHIFT                0
#define TANTOS_3G_RA_2B_2A_RA420_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200002D~0180C200002C' */
/* Bit: 'RA4445_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_2D_2C_RA4445_VALID_OFFSET            0xB6
#define TANTOS_3G_RA_2D_2C_RA4445_VALID_SHIFT             15
#define TANTOS_3G_RA_2D_2C_RA4445_VALID_SIZE              1
/* Bit: 'RA4445_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_2D_2C_RA4445_SPAN_OFFSET             0xB6
#define TANTOS_3G_RA_2D_2C_RA4445_SPAN_SHIFT              14
#define TANTOS_3G_RA_2D_2C_RA4445_SPAN_SIZE               1
/* Bit: 'RA4445_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_2D_2C_RA4445_MG_OFFSET               0xB6
#define TANTOS_3G_RA_2D_2C_RA4445_MG_SHIFT                13
#define TANTOS_3G_RA_2D_2C_RA4445_MG_SIZE                 1
/* Bit: 'RA4445_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_2D_2C_RA4445_CV_OFFSET               0xB6
#define TANTOS_3G_RA_2D_2C_RA4445_CV_SHIFT                12
#define TANTOS_3G_RA_2D_2C_RA4445_CV_SIZE                 1
/* Bit: 'RA4445_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_2D_2C_RA4445_TXTAG_OFFSET            0xB6
#define TANTOS_3G_RA_2D_2C_RA4445_TXTAG_SHIFT             10
#define TANTOS_3G_RA_2D_2C_RA4445_TXTAG_SIZE              2
/* Bit: 'RA4445_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_2D_2C_RA4445_ACT_OFFSET              0xB6
#define TANTOS_3G_RA_2D_2C_RA4445_ACT_SHIFT               8
#define TANTOS_3G_RA_2D_2C_RA4445_ACT_SIZE                2
/* Bit: 'RA440_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_2D_2C_RA440_VALID_OFFSET             0xB6
#define TANTOS_3G_RA_2D_2C_RA440_VALID_SHIFT              7
#define TANTOS_3G_RA_2D_2C_RA440_VALID_SIZE               1
/* Bit: 'RA440_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_2D_2C_RA440_SPAN_OFFSET              0xB6
#define TANTOS_3G_RA_2D_2C_RA440_SPAN_SHIFT               6
#define TANTOS_3G_RA_2D_2C_RA440_SPAN_SIZE                1
/* Bit: 'RA440_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_2D_2C_RA440_MG_OFFSET                0xB6
#define TANTOS_3G_RA_2D_2C_RA440_MG_SHIFT                 5
#define TANTOS_3G_RA_2D_2C_RA440_MG_SIZE                  1
/* Bit: 'RA440_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_2D_2C_RA440_CV_OFFSET                0xB6
#define TANTOS_3G_RA_2D_2C_RA440_CV_SHIFT                 4
#define TANTOS_3G_RA_2D_2C_RA440_CV_SIZE                  1
/* Bit: 'RA440_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_2D_2C_RA440_TXTAG_OFFSET             0xB6
#define TANTOS_3G_RA_2D_2C_RA440_TXTAG_SHIFT              2
#define TANTOS_3G_RA_2D_2C_RA440_TXTAG_SIZE               2
/* Bit: 'RA440_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_2D_2C_RA440_ACT_OFFSET               0xB6
#define TANTOS_3G_RA_2D_2C_RA440_ACT_SHIFT                0
#define TANTOS_3G_RA_2D_2C_RA440_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Reserve Action for 0180C200002F~0180C200002E' */
/* Bit: 'RA4647_VALID' */
/* Description: 'Valid bit for 0180C2000001' */
#define TANTOS_3G_RA_2F_2E_RA4647_VALID_OFFSET            0xB7
#define TANTOS_3G_RA_2F_2E_RA4647_VALID_SHIFT             15
#define TANTOS_3G_RA_2F_2E_RA4647_VALID_SIZE              1
/* Bit: 'RA4647_SPAN' */
/* Description: 'Span bit for 0180C2000001' */
#define TANTOS_3G_RA_2F_2E_RA4647_SPAN_OFFSET             0xB7
#define TANTOS_3G_RA_2F_2E_RA4647_SPAN_SHIFT              14
#define TANTOS_3G_RA_2F_2E_RA4647_SPAN_SIZE               1
/* Bit: 'RA4647_MG' */
/* Description: 'Management bit for 0180C2000001' */
#define TANTOS_3G_RA_2F_2E_RA4647_MG_OFFSET               0xB7
#define TANTOS_3G_RA_2F_2E_RA4647_MG_SHIFT                13
#define TANTOS_3G_RA_2F_2E_RA4647_MG_SIZE                 1
/* Bit: 'RA4647_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000001' */
#define TANTOS_3G_RA_2F_2E_RA4647_CV_OFFSET               0xB7
#define TANTOS_3G_RA_2F_2E_RA4647_CV_SHIFT                12
#define TANTOS_3G_RA_2F_2E_RA4647_CV_SIZE                 1
/* Bit: 'RA4647_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000001' */
#define TANTOS_3G_RA_2F_2E_RA4647_TXTAG_OFFSET            0xB7
#define TANTOS_3G_RA_2F_2E_RA4647_TXTAG_SHIFT             10
#define TANTOS_3G_RA_2F_2E_RA4647_TXTAG_SIZE              2
/* Bit: 'RA4647_ACT' */
/* Description: 'Action bit for 0180C2000001' */
#define TANTOS_3G_RA_2F_2E_RA4647_ACT_OFFSET              0xB7
#define TANTOS_3G_RA_2F_2E_RA4647_ACT_SHIFT               8
#define TANTOS_3G_RA_2F_2E_RA4647_ACT_SIZE                2
/* Bit: 'RA460_VALID' */
/* Description: 'Valid bit for 0180C2000000' */
#define TANTOS_3G_RA_2F_2E_RA460_VALID_OFFSET             0xB7
#define TANTOS_3G_RA_2F_2E_RA460_VALID_SHIFT              7
#define TANTOS_3G_RA_2F_2E_RA460_VALID_SIZE               1
/* Bit: 'RA460_SPAN' */
/* Description: 'Span bit for 0180C2000000' */
#define TANTOS_3G_RA_2F_2E_RA460_SPAN_OFFSET              0xB7
#define TANTOS_3G_RA_2F_2E_RA460_SPAN_SHIFT               6
#define TANTOS_3G_RA_2F_2E_RA460_SPAN_SIZE                1
/* Bit: 'RA460_MG' */
/* Description: 'Management bit for 0180C2000000' */
#define TANTOS_3G_RA_2F_2E_RA460_MG_OFFSET                0xB7
#define TANTOS_3G_RA_2F_2E_RA460_MG_SHIFT                 5
#define TANTOS_3G_RA_2F_2E_RA460_MG_SIZE                  1
/* Bit: 'RA460_CV' */
/* Description: 'Cross_VLAN bit for 0180C2000000' */
#define TANTOS_3G_RA_2F_2E_RA460_CV_OFFSET                0xB7
#define TANTOS_3G_RA_2F_2E_RA460_CV_SHIFT                 4
#define TANTOS_3G_RA_2F_2E_RA460_CV_SIZE                  1
/* Bit: 'RA460_TXTAG' */
/* Description: 'TXTAG bit for 0180C2000000' */
#define TANTOS_3G_RA_2F_2E_RA460_TXTAG_OFFSET             0xB7
#define TANTOS_3G_RA_2F_2E_RA460_TXTAG_SHIFT              2
#define TANTOS_3G_RA_2F_2E_RA460_TXTAG_SIZE               2
/* Bit: 'RA460_ACT' */
/* Description: 'Action bit for 0180C2000000' */
#define TANTOS_3G_RA_2F_2E_RA460_ACT_OFFSET               0xB7
#define TANTOS_3G_RA_2F_2E_RA460_ACT_SHIFT                0
#define TANTOS_3G_RA_2F_2E_RA460_ACT_SIZE                 2
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter 0' */
/* Bit: 'PFR1' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_0_PFR1_OFFSET                        0xB8
#define TANTOS_3G_PF_0_PFR1_SHIFT                         8
#define TANTOS_3G_PF_0_PFR1_SIZE                          8
/* Bit: 'PFR0' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_0_PFR0_OFFSET                        0xB8
#define TANTOS_3G_PF_0_PFR0_SHIFT                         0
#define TANTOS_3G_PF_0_PFR0_SIZE                          8
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter 3 and 2' */
/* Bit: 'PFR1' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_1_PFR1_OFFSET                        0xB9
#define TANTOS_3G_PF_1_PFR1_SHIFT                         8
#define TANTOS_3G_PF_1_PFR1_SIZE                          8
/* Bit: 'PFR0' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_1_PFR0_OFFSET                        0xB9
#define TANTOS_3G_PF_1_PFR0_SHIFT                         0
#define TANTOS_3G_PF_1_PFR0_SIZE                          8
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter 5 and 4' */
/* Bit: 'PFR1' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_2_PFR1_OFFSET                        0xBA
#define TANTOS_3G_PF_2_PFR1_SHIFT                         8
#define TANTOS_3G_PF_2_PFR1_SIZE                          8
/* Bit: 'PFR0' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_2_PFR0_OFFSET                        0xBA
#define TANTOS_3G_PF_2_PFR0_SHIFT                         0
#define TANTOS_3G_PF_2_PFR0_SIZE                          8
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter 7 and 6' */
/* Bit: 'PFR1' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_3_PFR1_OFFSET                        0xBB
#define TANTOS_3G_PF_3_PFR1_SHIFT                         8
#define TANTOS_3G_PF_3_PFR1_SIZE                          8
/* Bit: 'PFR0' */
/* Description: 'Value Compared with Protocol in IP Header' */
#define TANTOS_3G_PF_3_PFR0_OFFSET                        0xBB
#define TANTOS_3G_PF_3_PFR0_SHIFT                         0
#define TANTOS_3G_PF_3_PFR0_SIZE                          8
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Control 0 Register' */
/* Bit: 'PHYIE6' */
/* Description: 'PHY Initial Enable for port 6' */
#define TANTOS_3G_PHYIC0_PHYIE6_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE6_SHIFT                     14
#define TANTOS_3G_PHYIC0_PHYIE6_SIZE                      1
/* Bit: 'PHYIE5' */
/* Description: 'PHY Initial Enable for port 5' */
#define TANTOS_3G_PHYIC0_PHYIE5_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE5_SHIFT                     13
#define TANTOS_3G_PHYIC0_PHYIE5_SIZE                      1
/* Bit: 'PHYIE4' */
/* Description: 'PHY Initial Enable for port 4' */
#define TANTOS_3G_PHYIC0_PHYIE4_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE4_SHIFT                     12
#define TANTOS_3G_PHYIC0_PHYIE4_SIZE                      1
/* Bit: 'PHYIE3' */
/* Description: 'PHY Initial Enable for port 3' */
#define TANTOS_3G_PHYIC0_PHYIE3_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE3_SHIFT                     11
#define TANTOS_3G_PHYIC0_PHYIE3_SIZE                      1
/* Bit: 'PHYIE2' */
/* Description: 'PHY Initial Enable for port 2' */
#define TANTOS_3G_PHYIC0_PHYIE2_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE2_SHIFT                     10
#define TANTOS_3G_PHYIC0_PHYIE2_SIZE                      1
/* Bit: 'PHYIE1' */
/* Description: 'PHY Initial Enable for port 1' */
#define TANTOS_3G_PHYIC0_PHYIE1_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE1_SHIFT                     9
#define TANTOS_3G_PHYIC0_PHYIE1_SIZE                      1
/* Bit: 'PHYIE0' */
/* Description: 'PHY Initial Enable for port 0' */
#define TANTOS_3G_PHYIC0_PHYIE0_OFFSET                    0xD0
#define TANTOS_3G_PHYIC0_PHYIE0_SHIFT                     8
#define TANTOS_3G_PHYIC0_PHYIE0_SIZE                      1
/* Bit: 'REGA0' */
/* Description: 'Register Address 0' */
#define TANTOS_3G_PHYIC0_REGA0_OFFSET                     0xD0
#define TANTOS_3G_PHYIC0_REGA0_SHIFT                      0
#define TANTOS_3G_PHYIC0_REGA0_SIZE                       5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Control 1 Register' */
/* Bit: 'PHYIE6' */
/* Description: 'PHY Initial Enable for port 6' */
#define TANTOS_3G_PHYIC1_PHYIE6_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE6_SHIFT                     14
#define TANTOS_3G_PHYIC1_PHYIE6_SIZE                      1
/* Bit: 'PHYIE5' */
/* Description: 'PHY Initial Enable for port 5' */
#define TANTOS_3G_PHYIC1_PHYIE5_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE5_SHIFT                     13
#define TANTOS_3G_PHYIC1_PHYIE5_SIZE                      1
/* Bit: 'PHYIE4' */
/* Description: 'PHY Initial Enable for port 4' */
#define TANTOS_3G_PHYIC1_PHYIE4_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE4_SHIFT                     12
#define TANTOS_3G_PHYIC1_PHYIE4_SIZE                      1
/* Bit: 'PHYIE3' */
/* Description: 'PHY Initial Enable for port 3' */
#define TANTOS_3G_PHYIC1_PHYIE3_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE3_SHIFT                     11
#define TANTOS_3G_PHYIC1_PHYIE3_SIZE                      1
/* Bit: 'PHYIE2' */
/* Description: 'PHY Initial Enable for port 2' */
#define TANTOS_3G_PHYIC1_PHYIE2_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE2_SHIFT                     10
#define TANTOS_3G_PHYIC1_PHYIE2_SIZE                      1
/* Bit: 'PHYIE1' */
/* Description: 'PHY Initial Enable for port 1' */
#define TANTOS_3G_PHYIC1_PHYIE1_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE1_SHIFT                     9
#define TANTOS_3G_PHYIC1_PHYIE1_SIZE                      1
/* Bit: 'PHYIE0' */
/* Description: 'PHY Initial Enable for port 0' */
#define TANTOS_3G_PHYIC1_PHYIE0_OFFSET                    0xD2
#define TANTOS_3G_PHYIC1_PHYIE0_SHIFT                     8
#define TANTOS_3G_PHYIC1_PHYIE0_SIZE                      1
/* Bit: 'REGA0' */
/* Description: 'Register Address 0' */
#define TANTOS_3G_PHYIC1_REGA0_OFFSET                     0xD2
#define TANTOS_3G_PHYIC1_REGA0_SHIFT                      0
#define TANTOS_3G_PHYIC1_REGA0_SIZE                       5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Control 2 Register' */
/* Bit: 'PHYIE6' */
/* Description: 'PHY Initial Enable for port 6' */
#define TANTOS_3G_PHYIC2_PHYIE6_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE6_SHIFT                     14
#define TANTOS_3G_PHYIC2_PHYIE6_SIZE                      1
/* Bit: 'PHYIE5' */
/* Description: 'PHY Initial Enable for port 5' */
#define TANTOS_3G_PHYIC2_PHYIE5_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE5_SHIFT                     13
#define TANTOS_3G_PHYIC2_PHYIE5_SIZE                      1
/* Bit: 'PHYIE4' */
/* Description: 'PHY Initial Enable for port 4' */
#define TANTOS_3G_PHYIC2_PHYIE4_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE4_SHIFT                     12
#define TANTOS_3G_PHYIC2_PHYIE4_SIZE                      1
/* Bit: 'PHYIE3' */
/* Description: 'PHY Initial Enable for port 3' */
#define TANTOS_3G_PHYIC2_PHYIE3_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE3_SHIFT                     11
#define TANTOS_3G_PHYIC2_PHYIE3_SIZE                      1
/* Bit: 'PHYIE2' */
/* Description: 'PHY Initial Enable for port 2' */
#define TANTOS_3G_PHYIC2_PHYIE2_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE2_SHIFT                     10
#define TANTOS_3G_PHYIC2_PHYIE2_SIZE                      1
/* Bit: 'PHYIE1' */
/* Description: 'PHY Initial Enable for port 1' */
#define TANTOS_3G_PHYIC2_PHYIE1_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE1_SHIFT                     9
#define TANTOS_3G_PHYIC2_PHYIE1_SIZE                      1
/* Bit: 'PHYIE0' */
/* Description: 'PHY Initial Enable for port 0' */
#define TANTOS_3G_PHYIC2_PHYIE0_OFFSET                    0xD4
#define TANTOS_3G_PHYIC2_PHYIE0_SHIFT                     8
#define TANTOS_3G_PHYIC2_PHYIE0_SIZE                      1
/* Bit: 'REGA0' */
/* Description: 'Register Address 0' */
#define TANTOS_3G_PHYIC2_REGA0_OFFSET                     0xD4
#define TANTOS_3G_PHYIC2_REGA0_SHIFT                      0
#define TANTOS_3G_PHYIC2_REGA0_SIZE                       5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Control 3 Register' */
/* Bit: 'PHYIE6' */
/* Description: 'PHY Initial Enable for port 6' */
#define TANTOS_3G_PHYIC3_PHYIE6_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE6_SHIFT                     14
#define TANTOS_3G_PHYIC3_PHYIE6_SIZE                      1
/* Bit: 'PHYIE5' */
/* Description: 'PHY Initial Enable for port 5' */
#define TANTOS_3G_PHYIC3_PHYIE5_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE5_SHIFT                     13
#define TANTOS_3G_PHYIC3_PHYIE5_SIZE                      1
/* Bit: 'PHYIE4' */
/* Description: 'PHY Initial Enable for port 4' */
#define TANTOS_3G_PHYIC3_PHYIE4_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE4_SHIFT                     12
#define TANTOS_3G_PHYIC3_PHYIE4_SIZE                      1
/* Bit: 'PHYIE3' */
/* Description: 'PHY Initial Enable for port 3' */
#define TANTOS_3G_PHYIC3_PHYIE3_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE3_SHIFT                     11
#define TANTOS_3G_PHYIC3_PHYIE3_SIZE                      1
/* Bit: 'PHYIE2' */
/* Description: 'PHY Initial Enable for port 2' */
#define TANTOS_3G_PHYIC3_PHYIE2_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE2_SHIFT                     10
#define TANTOS_3G_PHYIC3_PHYIE2_SIZE                      1
/* Bit: 'PHYIE1' */
/* Description: 'PHY Initial Enable for port 1' */
#define TANTOS_3G_PHYIC3_PHYIE1_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE1_SHIFT                     9
#define TANTOS_3G_PHYIC3_PHYIE1_SIZE                      1
/* Bit: 'PHYIE0' */
/* Description: 'PHY Initial Enable for port 0' */
#define TANTOS_3G_PHYIC3_PHYIE0_OFFSET                    0xD6
#define TANTOS_3G_PHYIC3_PHYIE0_SHIFT                     8
#define TANTOS_3G_PHYIC3_PHYIE0_SIZE                      1
/* Bit: 'REGA0' */
/* Description: 'Register Address 0' */
#define TANTOS_3G_PHYIC3_REGA0_OFFSET                     0xD6
#define TANTOS_3G_PHYIC3_REGA0_SHIFT                      0
#define TANTOS_3G_PHYIC3_REGA0_SIZE                       5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Data 0 Register' */
/* Bit: 'REGD0' */
/* Description: 'Register Data 0' */
#define TANTOS_3G_PHYID0_REGD0_OFFSET                     0xD1
#define TANTOS_3G_PHYID0_REGD0_SHIFT                      0
#define TANTOS_3G_PHYID0_REGD0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Data 1 Register' */
/* Bit: 'REGD1' */
/* Description: 'Register Data 0' */
#define TANTOS_3G_PHYID1_REGD1_OFFSET                     0xD3
#define TANTOS_3G_PHYID1_REGD1_SHIFT                      0
#define TANTOS_3G_PHYID1_REGD1_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Data 2 Register' */
/* Bit: 'REGD2' */
/* Description: 'Register Data 0' */
#define TANTOS_3G_PHYID2_REGD2_OFFSET                     0xD5
#define TANTOS_3G_PHYID2_REGD2_SHIFT                      0
#define TANTOS_3G_PHYID2_REGD2_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Initial Data 3 Register' */
/* Bit: 'REGD3' */
/* Description: 'Register Data 0' */
#define TANTOS_3G_PHYID3_REGD3_OFFSET                     0xD7
#define TANTOS_3G_PHYID3_REGD3_SHIFT                      0
#define TANTOS_3G_PHYID3_REGD3_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Interrupt Enable Register' */
/* Bit: 'LTADIE' */
/* Description: 'Leaning Table Access Done Interrupt Enable' */
#define TANTOS_3G_IE_LTADIE_OFFSET                        0xD8
#define TANTOS_3G_IE_LTADIE_SHIFT                         8
#define TANTOS_3G_IE_LTADIE_SIZE                          1
/* Bit: 'PSVIE' */
/* Description: 'Port Security Violation Interrupt Enable' */
#define TANTOS_3G_IE_PSVIE_OFFSET                         0xD8
#define TANTOS_3G_IE_PSVIE_SHIFT                          1
#define TANTOS_3G_IE_PSVIE_SIZE                           7
/* Bit: 'PSCIE' */
/* Description: 'Port Status Change Interrupt Enable' */
#define TANTOS_3G_IE_PSCIE_OFFSET                         0xD8
#define TANTOS_3G_IE_PSCIE_SHIFT                          0
#define TANTOS_3G_IE_PSCIE_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Interrupt Status Register' */
/* Bit: 'LTAD' */
/* Description: 'Leaning Table Access Done' */
#define TANTOS_3G_IS_LTAD_OFFSET                          0xD9
#define TANTOS_3G_IS_LTAD_SHIFT                           8
#define TANTOS_3G_IS_LTAD_SIZE                            1
/* Bit: 'PSV' */
/* Description: 'Port Security Violation' */
#define TANTOS_3G_IS_PSV_OFFSET                           0xD9
#define TANTOS_3G_IS_PSV_SHIFT                            1
#define TANTOS_3G_IS_PSV_SIZE                             7
/* Bit: 'PSC' */
/* Description: 'Port Status Change' */
#define TANTOS_3G_IS_PSC_OFFSET                           0xD9
#define TANTOS_3G_IS_PSC_SHIFT                            0
#define TANTOS_3G_IS_PSC_SIZE                             1
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter Action 0' */
/* Bit: 'ATF7' */
/* Description: 'Action for Type Filter 7' */
#define TANTOS_3G_TFA0_ATF7_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF7_SHIFT                         14
#define TANTOS_3G_TFA0_ATF7_SIZE                          2
/* Bit: 'ATF6' */
/* Description: 'Action for Type Filter 6' */
#define TANTOS_3G_TFA0_ATF6_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF6_SHIFT                         12
#define TANTOS_3G_TFA0_ATF6_SIZE                          2
/* Bit: 'ATF5' */
/* Description: 'Action for Type Filter 5' */
#define TANTOS_3G_TFA0_ATF5_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF5_SHIFT                         10
#define TANTOS_3G_TFA0_ATF5_SIZE                          2
/* Bit: 'ATF4' */
/* Description: 'Action for Type Filter 4' */
#define TANTOS_3G_TFA0_ATF4_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF4_SHIFT                         8
#define TANTOS_3G_TFA0_ATF4_SIZE                          2
/* Bit: 'ATF3' */
/* Description: 'Action for Type Filter 3' */
#define TANTOS_3G_TFA0_ATF3_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF3_SHIFT                         6
#define TANTOS_3G_TFA0_ATF3_SIZE                          2
/* Bit: 'ATF2' */
/* Description: 'Action for Type Filter 2' */
#define TANTOS_3G_TFA0_ATF2_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF2_SHIFT                         4
#define TANTOS_3G_TFA0_ATF2_SIZE                          2
/* Bit: 'ATF1' */
/* Description: 'Action for Type Filter 1' */
#define TANTOS_3G_TFA0_ATF1_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF1_SHIFT                         2
#define TANTOS_3G_TFA0_ATF1_SIZE                          2
/* Bit: 'ATF0' */
/* Description: 'Action for Type Filter 0' */
#define TANTOS_3G_TFA0_ATF0_OFFSET                        0xDA
#define TANTOS_3G_TFA0_ATF0_SHIFT                         0
#define TANTOS_3G_TFA0_ATF0_SIZE                          2
/* -------------------------------------------------------------------------- */
/* Register: 'Type Filter Action 1' */
/* Bit: 'QATF7' */
/* Description: 'Destination Queue for Type Filter 7' */
#define TANTOS_3G_TFA1_QATF7_OFFSET                       0xDB
#define TANTOS_3G_TFA1_QATF7_SHIFT                        14
#define TANTOS_3G_TFA1_QATF7_SIZE                         2
/* Bit: 'QATF6' */
/* Description: 'Destination Queue for Type Filter 6' */
#define TANTOS_3G_TFA1_QATF6_OFFSET                       0xDB
#define TANTOS_3G_TFA1_QATF6_SHIFT                        12
#define TANTOS_3G_TFA1_QATF6_SIZE                         2
/* Bit: 'QTF5' */
/* Description: 'Destination Queue for Type Filter 5' */
#define TANTOS_3G_TFA1_QTF5_OFFSET                        0xDB
#define TANTOS_3G_TFA1_QTF5_SHIFT                         10
#define TANTOS_3G_TFA1_QTF5_SIZE                          2
/* Bit: 'QTF4' */
/* Description: 'Destination Queue for Type Filter 4' */
#define TANTOS_3G_TFA1_QTF4_OFFSET                        0xDB
#define TANTOS_3G_TFA1_QTF4_SHIFT                         8
#define TANTOS_3G_TFA1_QTF4_SIZE                          2
/* Bit: 'QTF3' */
/* Description: 'Destination Queue for Type Filter 3' */
#define TANTOS_3G_TFA1_QTF3_OFFSET                        0xDB
#define TANTOS_3G_TFA1_QTF3_SHIFT                         6
#define TANTOS_3G_TFA1_QTF3_SIZE                          2
/* Bit: 'QTF2' */
/* Description: 'Destination Queue for Type Filter 2' */
#define TANTOS_3G_TFA1_QTF2_OFFSET                        0xDB
#define TANTOS_3G_TFA1_QTF2_SHIFT                         4
#define TANTOS_3G_TFA1_QTF2_SIZE                          2
/* Bit: 'QTF1' */
/* Description: 'Destination Queue for Type Filter 1' */
#define TANTOS_3G_TFA1_QTF1_OFFSET                        0xDB
#define TANTOS_3G_TFA1_QTF1_SHIFT                         2
#define TANTOS_3G_TFA1_QTF1_SIZE                          2
/* Bit: 'QTF0' */
/* Description: 'Destination Queue for Type Filter 0' */
#define TANTOS_3G_TFA1_QTF0_OFFSET                        0xDB
#define TANTOS_3G_TFA1_QTF0_SHIFT                         0
#define TANTOS_3G_TFA1_QTF0_SIZE                          2
/* -------------------------------------------------------------------------- */
/* Register: 'ARP/RARP Register' */
/* Bit: 'MACA' */
/* Description: 'MAC Control Action' */
#define TANTOS_3G_AR_MACA_OFFSET                          0xDC
#define TANTOS_3G_AR_MACA_SHIFT                           14
#define TANTOS_3G_AR_MACA_SIZE                            2
/* Bit: 'UPT' */
/* Description: 'Unicast packet Treated as Cross_VLAN packet' */
#define TANTOS_3G_AR_UPT_OFFSET                           0xDC
#define TANTOS_3G_AR_UPT_SHIFT                            13
#define TANTOS_3G_AR_UPT_SIZE                             1
/* Bit: 'RPT' */
/* Description: 'RARP Packet Treated as Cross_VLAN Packet' */
#define TANTOS_3G_AR_RPT_OFFSET                           0xDC
#define TANTOS_3G_AR_RPT_SHIFT                            12
#define TANTOS_3G_AR_RPT_SIZE                             1
/* Bit: 'RAPA' */
/* Description: 'RARP/ARP Packet Action' */
#define TANTOS_3G_AR_RAPA_OFFSET                          0xDC
#define TANTOS_3G_AR_RAPA_SHIFT                           10
#define TANTOS_3G_AR_RAPA_SIZE                            2
/* Bit: 'RAPPE' */
/* Description: 'RARP/ARP Packet Priority Enable' */
#define TANTOS_3G_AR_RAPPE_OFFSET                         0xDC
#define TANTOS_3G_AR_RAPPE_SHIFT                          9
#define TANTOS_3G_AR_RAPPE_SIZE                           1
/* Bit: 'RAPP' */
/* Description: 'RARP/ARP Packet Priority' */
#define TANTOS_3G_AR_RAPP_OFFSET                          0xDC
#define TANTOS_3G_AR_RAPP_SHIFT                           7
#define TANTOS_3G_AR_RAPP_SIZE                            2
/* Bit: 'RAPOTH' */
/* Description: 'RARP/ARP Packet Output Tag Handle' */
#define TANTOS_3G_AR_RAPOTH_OFFSET                        0xDC
#define TANTOS_3G_AR_RAPOTH_SHIFT                         5
#define TANTOS_3G_AR_RAPOTH_SIZE                          2
/* Bit: 'APT' */
/* Description: 'ARP Packet Treated as Cross _ VLAN Packet' */
#define TANTOS_3G_AR_APT_OFFSET                           0xDC
#define TANTOS_3G_AR_APT_SHIFT                            4
#define TANTOS_3G_AR_APT_SIZE                             1
/* Bit: 'RAPTM' */
/* Description: 'RARP/ARP Packet Treated as Management Packet' */
#define TANTOS_3G_AR_RAPTM_OFFSET                         0xDC
#define TANTOS_3G_AR_RAPTM_SHIFT                          3
#define TANTOS_3G_AR_RAPTM_SIZE                           1
/* Bit: 'TAPTS' */
/* Description: 'RARP/ARP Packet Treated as Span Packet' */
#define TANTOS_3G_AR_TAPTS_OFFSET                         0xDC
#define TANTOS_3G_AR_TAPTS_SHIFT                          2
#define TANTOS_3G_AR_TAPTS_SIZE                           1
/* Bit: 'TAP' */
/* Description: 'Trap ARP Packet' */
#define TANTOS_3G_AR_TAP_OFFSET                           0xDC
#define TANTOS_3G_AR_TAP_SHIFT                            1
#define TANTOS_3G_AR_TAP_SIZE                             1
/* Bit: 'TRP' */
/* Description: 'Trap RARP Packet' */
#define TANTOS_3G_AR_TRP_OFFSET                           0xDC
#define TANTOS_3G_AR_TRP_SHIFT                            0
#define TANTOS_3G_AR_TRP_SIZE                             1
/* -------------------------------------------------------------------------- */
/* Register: 'Packet Identification Option' */
/* Bit: 'DIVS' */
/* Description: 'Do not Identify VLAN after SNAP' */
#define TANTOS_3G_PIOFGPM_DIVS_OFFSET                     0xDD
#define TANTOS_3G_PIOFGPM_DIVS_SHIFT                      14
#define TANTOS_3G_PIOFGPM_DIVS_SIZE                       1
/* Bit: 'DII6P' */
/* Description: 'Do not Identify IPV6 in PPPOE' */
#define TANTOS_3G_PIOFGPM_DII6P_OFFSET                    0xDD
#define TANTOS_3G_PIOFGPM_DII6P_SHIFT                     13
#define TANTOS_3G_PIOFGPM_DII6P_SIZE                      1
/* Bit: 'DIIPS' */
/* Description: 'Do not Identify IP in PPPOE after SNAP' */
#define TANTOS_3G_PIOFGPM_DIIPS_OFFSET                    0xDD
#define TANTOS_3G_PIOFGPM_DIIPS_SHIFT                     12
#define TANTOS_3G_PIOFGPM_DIIPS_SIZE                      1
/* Bit: 'DIE' */
/* Description: 'Do not Identify Ether-Type = 0x0800, IP VER = 6
as IPV6 packets' */
#define TANTOS_3G_PIOFGPM_DIE_OFFSET                      0xDD
#define TANTOS_3G_PIOFGPM_DIE_SHIFT                       11
#define TANTOS_3G_PIOFGPM_DIE_SIZE                        1
/* Bit: 'DIIP' */
/* Description: 'Do not Identify IP in PPPOE' */
#define TANTOS_3G_PIOFGPM_DIIP_OFFSET                     0xDD
#define TANTOS_3G_PIOFGPM_DIIP_SHIFT                      10
#define TANTOS_3G_PIOFGPM_DIIP_SIZE                       1
/* Bit: 'DIS' */
/* Description: 'Do not Identify SNAP' */
#define TANTOS_3G_PIOFGPM_DIS_OFFSET                      0xDD
#define TANTOS_3G_PIOFGPM_DIS_SHIFT                       9
#define TANTOS_3G_PIOFGPM_DIS_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'Switch Global Control Register 1' */
/* Bit: 'TSIPGE' */
/* Description: 'Transmit Short IPG Enable' */
#define TANTOS_3G_SGC1_TSIPGE_OFFSET                      0xE0
#define TANTOS_3G_SGC1_TSIPGE_SHIFT                       15
#define TANTOS_3G_SGC1_TSIPGE_SIZE                        1
/* Bit: 'PHYBA' */
/* Description: 'PHY Base Address' */
#define TANTOS_3G_SGC1_PHYBA_OFFSET                       0xE0
#define TANTOS_3G_SGC1_PHYBA_SHIFT                        14
#define TANTOS_3G_SGC1_PHYBA_SIZE                         1
/* Bit: 'DPWECH' */
/* Description: 'Drop Packet When Excessive Collision Happen' */
#define TANTOS_3G_SGC1_DPWECH_OFFSET                      0xE0
#define TANTOS_3G_SGC1_DPWECH_SHIFT                       13
#define TANTOS_3G_SGC1_DPWECH_SIZE                        1
/* Bit: 'ATS' */
/* Description: 'Aging Timer Select' */
#define TANTOS_3G_SGC1_ATS_OFFSET                         0xE0
#define TANTOS_3G_SGC1_ATS_SHIFT                          10
#define TANTOS_3G_SGC1_ATS_SIZE                           3
/* Bit: 'MPL' */
/* Description: 'Max Packet Length (MAXPKTLEN)' */
#define TANTOS_3G_SGC1_MPL_OFFSET                         0xE0
#define TANTOS_3G_SGC1_MPL_SHIFT                          8
#define TANTOS_3G_SGC1_MPL_SIZE                           2
/* Bit: 'DMQ3' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q3)' */
#define TANTOS_3G_SGC1_DMQ3_OFFSET                        0xE0
#define TANTOS_3G_SGC1_DMQ3_SHIFT                         6
#define TANTOS_3G_SGC1_DMQ3_SIZE                          2
/* Bit: 'DMQ2' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q2)' */
#define TANTOS_3G_SGC1_DMQ2_OFFSET                        0xE0
#define TANTOS_3G_SGC1_DMQ2_SHIFT                         4
#define TANTOS_3G_SGC1_DMQ2_SIZE                          2
/* Bit: 'DMQ1' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q1)' */
#define TANTOS_3G_SGC1_DMQ1_OFFSET                        0xE0
#define TANTOS_3G_SGC1_DMQ1_SHIFT                         2
#define TANTOS_3G_SGC1_DMQ1_SIZE                          2
/* Bit: 'DMQ0' */
/* Description: 'Discard Mode (Drop scheme for Packets Classified
as Q0)' */
#define TANTOS_3G_SGC1_DMQ0_OFFSET                        0xE0
#define TANTOS_3G_SGC1_DMQ0_SHIFT                         0
#define TANTOS_3G_SGC1_DMQ0_SIZE                          2
/* -------------------------------------------------------------------------- */
/* Register: 'Switch Global Control Register 2' */
/* Bit: 'SE' */
/* Description: 'Switch Enable. (when WAIT_INIT=1)' */
#define TANTOS_3G_SGC2_SE_OFFSET                          0xE1
#define TANTOS_3G_SGC2_SE_SHIFT                           15
#define TANTOS_3G_SGC2_SE_SIZE                            1
/* Bit: 'ICRCCD' */
/* Description: 'CRC Check Disable' */
#define TANTOS_3G_SGC2_ICRCCD_OFFSET                      0xE1
#define TANTOS_3G_SGC2_ICRCCD_SHIFT                       14
#define TANTOS_3G_SGC2_ICRCCD_SIZE                        1
/* Bit: 'ITRUNK' */
/* Description: 'Port 2 and Port 3 Trunk Enable' */
#define TANTOS_3G_SGC2_ITRUNK_OFFSET                      0xE1
#define TANTOS_3G_SGC2_ITRUNK_SHIFT                       12
#define TANTOS_3G_SGC2_ITRUNK_SIZE                        1
/* Bit: 'ITENLMT' */
/* Description: '10Mbit/s Drop Packet before 100Mbit/s Enable' */
#define TANTOS_3G_SGC2_ITENLMT_OFFSET                     0xE1
#define TANTOS_3G_SGC2_ITENLMT_SHIFT                      11
#define TANTOS_3G_SGC2_ITENLMT_SIZE                       1
/* Bit: 'RVID0' */
/* Description: 'Replace VID0' */
#define TANTOS_3G_SGC2_RVID0_OFFSET                       0xE1
#define TANTOS_3G_SGC2_RVID0_SHIFT                        9
#define TANTOS_3G_SGC2_RVID0_SIZE                         1
/* Bit: 'RVID1' */
/* Description: 'Replace VID1' */
#define TANTOS_3G_SGC2_RVID1_OFFSET                       0xE1
#define TANTOS_3G_SGC2_RVID1_SHIFT                        8
#define TANTOS_3G_SGC2_RVID1_SIZE                         1
/* Bit: 'RVIDFFF' */
/* Description: 'Replace VIDFFF' */
#define TANTOS_3G_SGC2_RVIDFFF_OFFSET                     0xE1
#define TANTOS_3G_SGC2_RVIDFFF_SHIFT                      7
#define TANTOS_3G_SGC2_RVIDFFF_SIZE                       1
/* Bit: 'DUPCOLSP' */
/* Description: 'Dupcol LED Separate' */
#define TANTOS_3G_SGC2_DUPCOLSP_OFFSET                    0xE1
#define TANTOS_3G_SGC2_DUPCOLSP_SHIFT                     6
#define TANTOS_3G_SGC2_DUPCOLSP_SIZE                      1
/* Bit: 'PCR' */
/* Description: 'Priority Change Rule' */
#define TANTOS_3G_SGC2_PCR_OFFSET                         0xE1
#define TANTOS_3G_SGC2_PCR_SHIFT                          4
#define TANTOS_3G_SGC2_PCR_SIZE                           1
/* Bit: 'PCE' */
/* Description: 'Priority Change Enable' */
#define TANTOS_3G_SGC2_PCE_OFFSET                         0xE1
#define TANTOS_3G_SGC2_PCE_SHIFT                          3
#define TANTOS_3G_SGC2_PCE_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'CPU Port & Mirror Control Register' */
/* Bit: 'SPN' */
/* Description: 'Sniffer Port Number' */
#define TANTOS_3G_CMH_SPN_OFFSET                          0xE2
#define TANTOS_3G_CMH_SPN_SHIFT                           13
#define TANTOS_3G_CMH_SPN_SIZE                            3
/* Bit: 'MCA' */
/* Description: 'Mirror CRC Also' */
#define TANTOS_3G_CMH_MCA_OFFSET                          0xE2
#define TANTOS_3G_CMH_MCA_SHIFT                           12
#define TANTOS_3G_CMH_MCA_SIZE                            1
/* Bit: 'MRA' */
/* Description: 'Mirror RXER Also' */
#define TANTOS_3G_CMH_MRA_OFFSET                          0xE2
#define TANTOS_3G_CMH_MRA_SHIFT                           11
#define TANTOS_3G_CMH_MRA_SIZE                            1
/* Bit: 'MPA' */
/* Description: 'Mirror PAUSE Also' */
#define TANTOS_3G_CMH_MPA_OFFSET                          0xE2
#define TANTOS_3G_CMH_MPA_SHIFT                           10
#define TANTOS_3G_CMH_MPA_SIZE                            1
/* Bit: 'MLA' */
/* Description: 'Mirror Long Also' */
#define TANTOS_3G_CMH_MLA_OFFSET                          0xE2
#define TANTOS_3G_CMH_MLA_SHIFT                           9
#define TANTOS_3G_CMH_MLA_SIZE                            1
/* Bit: 'MSA' */
/* Description: 'Mirror Short Also' */
#define TANTOS_3G_CMH_MSA_OFFSET                          0xE2
#define TANTOS_3G_CMH_MSA_SHIFT                           8
#define TANTOS_3G_CMH_MSA_SIZE                            1
/* Bit: 'CPN' */
/* Description: 'CPU Port Number' */
#define TANTOS_3G_CMH_CPN_OFFSET                          0xE2
#define TANTOS_3G_CMH_CPN_SHIFT                           5
#define TANTOS_3G_CMH_CPN_SIZE                            3
/* Bit: 'STRE' */
/* Description: 'Special TAG Receive Enable' */
#define TANTOS_3G_CMH_STRE_OFFSET                         0xE2
#define TANTOS_3G_CMH_STRE_SHIFT                          4
#define TANTOS_3G_CMH_STRE_SIZE                           1
/* Bit: 'STTE' */
/* Description: 'Special TAG Transmit Enable' */
#define TANTOS_3G_CMH_STTE_OFFSET                         0xE2
#define TANTOS_3G_CMH_STTE_SHIFT                          3
#define TANTOS_3G_CMH_STTE_SIZE                           1
/* Bit: 'PAST' */
/* Description: 'Pause also adds Special Tag when Special TAG Transmit
is enabled' */
#define TANTOS_3G_CMH_PAST_OFFSET                         0xE2
#define TANTOS_3G_CMH_PAST_SHIFT                          2
#define TANTOS_3G_CMH_PAST_SIZE                           1
/* Bit: 'CCCRC' */
/* Description: 'CPU Port doesn't check CRC for packets with Special
Tag' */
#define TANTOS_3G_CMH_CCCRC_OFFSET                        0xE2
#define TANTOS_3G_CMH_CCCRC_SHIFT                         1
#define TANTOS_3G_CMH_CCCRC_SIZE                          1
/* Bit: 'IGSTA' */
/* Description: 'Interframe gap for Special Tag application' */
#define TANTOS_3G_CMH_IGSTA_OFFSET                        0xE2
#define TANTOS_3G_CMH_IGSTA_SHIFT                         0
#define TANTOS_3G_CMH_IGSTA_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'Multicast Snooping Register' */
/* Bit: 'SCPA' */
/* Description: 'Snooping Control Packet Action' */
#define TANTOS_3G_MS_SCPA_OFFSET                          0xE3
#define TANTOS_3G_MS_SCPA_SHIFT                           14
#define TANTOS_3G_MS_SCPA_SIZE                            2
/* Bit: 'SCPPE' */
/* Description: 'Snooping Control Packet Priority Enable' */
#define TANTOS_3G_MS_SCPPE_OFFSET                         0xE3
#define TANTOS_3G_MS_SCPPE_SHIFT                          13
#define TANTOS_3G_MS_SCPPE_SIZE                           1
/* Bit: 'SCPP' */
/* Description: 'Snooping Control Packet Priority' */
#define TANTOS_3G_MS_SCPP_OFFSET                          0xE3
#define TANTOS_3G_MS_SCPP_SHIFT                           11
#define TANTOS_3G_MS_SCPP_SIZE                            2
/* Bit: 'SCPTTH' */
/* Description: 'Snooping Control Packet Transmission Tag Handle' */
#define TANTOS_3G_MS_SCPTTH_OFFSET                        0xE3
#define TANTOS_3G_MS_SCPTTH_SHIFT                         9
#define TANTOS_3G_MS_SCPTTH_SIZE                          2
/* Bit: 'SCPTCP' */
/* Description: 'Snooping Control Packet Treated as Cross_VLAN Packet' */
#define TANTOS_3G_MS_SCPTCP_OFFSET                        0xE3
#define TANTOS_3G_MS_SCPTCP_SHIFT                         8
#define TANTOS_3G_MS_SCPTCP_SIZE                          1
/* Bit: 'SCPTMP' */
/* Description: 'Snooping Control Packet Treated as Management Packet' */
#define TANTOS_3G_MS_SCPTMP_OFFSET                        0xE3
#define TANTOS_3G_MS_SCPTMP_SHIFT                         7
#define TANTOS_3G_MS_SCPTMP_SIZE                          1
/* Bit: 'SCPTSP' */
/* Description: 'Snooping Control Packet Treated as Span Packet' */
#define TANTOS_3G_MS_SCPTSP_OFFSET                        0xE3
#define TANTOS_3G_MS_SCPTSP_SHIFT                         6
#define TANTOS_3G_MS_SCPTSP_SIZE                          1
/* Bit: 'ASC' */
/* Description: 'Additional Snooping Control. These bits are used
when the packets on the incoming port with the Ethernet destination
address = 01-00-5E-XX-XX-XX/33-33-XX-XX-XX-XX are not IGMP_IP/MLD_IPV/ MLD_IPV6
packets and not found in the learning table or the hardware IGMP
table' */
#define TANTOS_3G_MS_ASC_OFFSET                           0xE3
#define TANTOS_3G_MS_ASC_SHIFT                            4
#define TANTOS_3G_MS_ASC_SIZE                             2
/* Bit: 'IPMPT' */
/* Description: 'IP Multicast Packet Treated as Cross_VLAN packet' */
#define TANTOS_3G_MS_IPMPT_OFFSET                         0xE3
#define TANTOS_3G_MS_IPMPT_SHIFT                          2
#define TANTOS_3G_MS_IPMPT_SIZE                           1
/* Bit: 'RV' */
/* Description: 'Robust Variable' */
#define TANTOS_3G_MS_RV_OFFSET                            0xE3
#define TANTOS_3G_MS_RV_SHIFT                             0
#define TANTOS_3G_MS_RV_SIZE                              2
/* -------------------------------------------------------------------------- */
/* Register: 'Hardware IGMP Control Register' */
/* Bit: 'QI' */
/* Description: 'Query Interval' */
#define TANTOS_3G_HIC_QI_OFFSET                           0xE4
#define TANTOS_3G_HIC_QI_SHIFT                            8
#define TANTOS_3G_HIC_QI_SIZE                             8
/* Bit: 'HIPI' */
/* Description: 'Hardware IGMP Packet Ignore CPU Port' */
#define TANTOS_3G_HIC_HIPI_OFFSET                         0xE4
#define TANTOS_3G_HIC_HIPI_SHIFT                          7
#define TANTOS_3G_HIC_HIPI_SIZE                           1
/* Bit: 'DRP' */
/* Description: 'Default Router Portmap' */
#define TANTOS_3G_HIC_DRP_OFFSET                          0xE4
#define TANTOS_3G_HIC_DRP_SHIFT                           0
#define TANTOS_3G_HIC_DRP_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'Unicast Port Map and Broadcast Port Map' */
/* Bit: 'UP' */
/* Description: 'Unicast Portmap' */
#define TANTOS_3G_UPMBPM_UP_OFFSET                        0xE5
#define TANTOS_3G_UPMBPM_UP_SHIFT                         8
#define TANTOS_3G_UPMBPM_UP_SIZE                          7
/* Bit: 'BP' */
/* Description: 'Broadcast Portmap' */
#define TANTOS_3G_UPMBPM_BP_OFFSET                        0xE5
#define TANTOS_3G_UPMBPM_BP_SHIFT                         0
#define TANTOS_3G_UPMBPM_BP_SIZE                          7
/* -------------------------------------------------------------------------- */
/* Register: 'Multicast Port Map and Reserve Port Map' */
/* Bit: 'MP' */
/* Description: 'Multicast Portmap' */
#define TANTOS_3G_MPMRPM_MP_OFFSET                        0xE6
#define TANTOS_3G_MPMRPM_MP_SHIFT                         8
#define TANTOS_3G_MPMRPM_MP_SIZE                          7
/* Bit: 'RP' */
/* Description: 'Reserve Portmap' */
#define TANTOS_3G_MPMRPM_RP_OFFSET                        0xE6
#define TANTOS_3G_MPMRPM_RP_SHIFT                         0
#define TANTOS_3G_MPMRPM_RP_SIZE                          7
/* -------------------------------------------------------------------------- */
/* Register: 'Protocol Filter Action' */
/* Bit: 'APF7' */
/* Description: 'Action for Protocol Filter 7' */
#define TANTOS_3G_PFA_APF7_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF7_SHIFT                          14
#define TANTOS_3G_PFA_APF7_SIZE                           2
/* Bit: 'APF6' */
/* Description: 'Action for Protocol Filter 6' */
#define TANTOS_3G_PFA_APF6_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF6_SHIFT                          12
#define TANTOS_3G_PFA_APF6_SIZE                           2
/* Bit: 'APF5' */
/* Description: 'Action for Protocol Filter 5' */
#define TANTOS_3G_PFA_APF5_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF5_SHIFT                          10
#define TANTOS_3G_PFA_APF5_SIZE                           2
/* Bit: 'APF4' */
/* Description: 'Action for Protocol Filter 4' */
#define TANTOS_3G_PFA_APF4_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF4_SHIFT                          8
#define TANTOS_3G_PFA_APF4_SIZE                           2
/* Bit: 'APF3' */
/* Description: 'Action for Protocol Filter 3' */
#define TANTOS_3G_PFA_APF3_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF3_SHIFT                          6
#define TANTOS_3G_PFA_APF3_SIZE                           2
/* Bit: 'APF2' */
/* Description: 'Action for Protocol Filter 2' */
#define TANTOS_3G_PFA_APF2_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF2_SHIFT                          4
#define TANTOS_3G_PFA_APF2_SIZE                           2
/* Bit: 'APF1' */
/* Description: 'Action for Protocol Filter 1' */
#define TANTOS_3G_PFA_APF1_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF1_SHIFT                          2
#define TANTOS_3G_PFA_APF1_SIZE                           2
/* Bit: 'APF0' */
/* Description: 'Action for Protocol Filter 0' */
#define TANTOS_3G_PFA_APF0_OFFSET                         0xE7
#define TANTOS_3G_PFA_APF0_SHIFT                          0
#define TANTOS_3G_PFA_APF0_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: '802.1p Priority Map Register' */
/* Bit: '1PPQ7' */
/* Description: 'Priority Queue 7' */
#define TANTOS_3G_1PPM_1PPQ7_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ7_SHIFT                        14
#define TANTOS_3G_1PPM_1PPQ7_SIZE                         2
/* Bit: '1PPQ6' */
/* Description: 'Priority Queue 6' */
#define TANTOS_3G_1PPM_1PPQ6_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ6_SHIFT                        12
#define TANTOS_3G_1PPM_1PPQ6_SIZE                         2
/* Bit: '1PPQ5' */
/* Description: 'Priority Queue 5' */
#define TANTOS_3G_1PPM_1PPQ5_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ5_SHIFT                        10
#define TANTOS_3G_1PPM_1PPQ5_SIZE                         2
/* Bit: '1PPQ4' */
/* Description: 'Priority Queue 4' */
#define TANTOS_3G_1PPM_1PPQ4_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ4_SHIFT                        8
#define TANTOS_3G_1PPM_1PPQ4_SIZE                         2
/* Bit: '1PPQ3' */
/* Description: 'Priority Queue 3' */
#define TANTOS_3G_1PPM_1PPQ3_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ3_SHIFT                        6
#define TANTOS_3G_1PPM_1PPQ3_SIZE                         2
/* Bit: '1PPQ2' */
/* Description: 'Priority Queue 2' */
#define TANTOS_3G_1PPM_1PPQ2_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ2_SHIFT                        4
#define TANTOS_3G_1PPM_1PPQ2_SIZE                         2
/* Bit: '1PPQ1' */
/* Description: 'Priority Queue 1' */
#define TANTOS_3G_1PPM_1PPQ1_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ1_SHIFT                        2
#define TANTOS_3G_1PPM_1PPQ1_SIZE                         2
/* Bit: '1PPQ0' */
/* Description: 'Priority Queue 0' */
#define TANTOS_3G_1PPM_1PPQ0_OFFSET                       0xE8
#define TANTOS_3G_1PPM_1PPQ0_SHIFT                        0
#define TANTOS_3G_1PPM_1PPQ0_SIZE                         2
/* -------------------------------------------------------------------------- */
/* Register: 'Switch MAC Address Register 1' */
/* Bit: 'ADDR47_41' */
/* Description: 'Address [47:41]' */
#define TANTOS_3G_SMA1_ADDR47_41_OFFSET                   0xE9
#define TANTOS_3G_SMA1_ADDR47_41_SHIFT                    9
#define TANTOS_3G_SMA1_ADDR47_41_SIZE                     7
/* Bit: 'PAC' */
/* Description: 'Pause Address Change' */
#define TANTOS_3G_SMA1_PAC_OFFSET                         0xE9
#define TANTOS_3G_SMA1_PAC_SHIFT                          8
#define TANTOS_3G_SMA1_PAC_SIZE                           1
/* Bit: 'ADDR39_32' */
/* Description: 'Address [39:32]' */
#define TANTOS_3G_SMA1_ADDR39_32_OFFSET                   0xE9
#define TANTOS_3G_SMA1_ADDR39_32_SHIFT                    0
#define TANTOS_3G_SMA1_ADDR39_32_SIZE                     8
/* -------------------------------------------------------------------------- */
/* Register: 'Switch MAC Address Register 2' */
/* Bit: 'ADDR31_16' */
/* Description: 'Address [31:16]' */
#define TANTOS_3G_SMA2_ADDR31_16_OFFSET                   0xEA
#define TANTOS_3G_SMA2_ADDR31_16_SHIFT                    0
#define TANTOS_3G_SMA2_ADDR31_16_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Switch MAC Address Register 3' */
/* Bit: 'ADDR15_0' */
/* Description: 'Address [15:0]' */
#define TANTOS_3G_SMA3_ADDR15_0_OFFSET                    0xEB
#define TANTOS_3G_SMA3_ADDR15_0_SHIFT                     0
#define TANTOS_3G_SMA3_ADDR15_0_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'Global Bucket Size Base counter' */
/* Bit: 'Base15_0' */
/* Description: 'Base[15:0]' */
#define TANTOS_3G_GBSBC_BASE15_0_OFFSET                   0xEC
#define TANTOS_3G_GBSBC_BASE15_0_SHIFT                    0
#define TANTOS_3G_GBSBC_BASE15_0_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Global Bucket Size Extend Base Counter' */
/* Bit: 'EBase15_0' */
/* Description: 'Extend Base[15:0]' */
#define TANTOS_3G_GBSEBC_EBASE15_0_OFFSET                 0xED
#define TANTOS_3G_GBSEBC_EBASE15_0_SHIFT                  0
#define TANTOS_3G_GBSEBC_EBASE15_0_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'Global Bucket Size Counter High Bits' */
/* Bit: 'EBase17_16' */
/* Description: 'EBase[17:16]' */
#define TANTOS_3G_GBSCHB_EBASE17_16_OFFSET                0xEE
#define TANTOS_3G_GBSCHB_EBASE17_16_SHIFT                 8
#define TANTOS_3G_GBSCHB_EBASE17_16_SIZE                  2
/* Bit: 'Base17_16' */
/* Description: 'Base[17:16]' */
#define TANTOS_3G_GBSCHB_BASE17_16_OFFSET                 0xEE
#define TANTOS_3G_GBSCHB_BASE17_16_SHIFT                  0
#define TANTOS_3G_GBSCHB_BASE17_16_SIZE                   2
/* -------------------------------------------------------------------------- */
/* Register: 'Congestion Control register' */
/* Bit: 'EDSTX' */
/* Description: 'Drop scheme selection' */
#define TANTOS_3G_CCR_EDSTX_OFFSET                        0xEF
#define TANTOS_3G_CCR_EDSTX_SHIFT                         15
#define TANTOS_3G_CCR_EDSTX_SIZE                          1
/* Bit: 'IRSJA' */
/* Description: 'IGMP Report Supression and Join Aggregation control' */
#define TANTOS_3G_CCR_IRSJA_OFFSET                        0xEF
#define TANTOS_3G_CCR_IRSJA_SHIFT                         8
#define TANTOS_3G_CCR_IRSJA_SIZE                          2
/* Bit: 'IJT' */
/* Description: 'Input Jam Threshold' */
#define TANTOS_3G_CCR_IJT_OFFSET                          0xEF
#define TANTOS_3G_CCR_IJT_SHIFT                           0
#define TANTOS_3G_CCR_IJT_SIZE                            7
/* -------------------------------------------------------------------------- */
/* Register: 'Storm control Register 0' */
/* Bit: 'STORM_B' */
/* Description: 'Storm Enable for Broadcast Packets' */
#define TANTOS_3G_SCR0_STORM_B_OFFSET                     0xF0
#define TANTOS_3G_SCR0_STORM_B_SHIFT                      15
#define TANTOS_3G_SCR0_STORM_B_SIZE                       1
/* Bit: 'STORM_M' */
/* Description: 'Storm Enable for Multicast Packets' */
#define TANTOS_3G_SCR0_STORM_M_OFFSET                     0xF0
#define TANTOS_3G_SCR0_STORM_M_SHIFT                      14
#define TANTOS_3G_SCR0_STORM_M_SIZE                       1
/* Bit: 'STORM_U' */
/* Description: 'Storm Enable for Un-learned Unicast Packets' */
#define TANTOS_3G_SCR0_STORM_U_OFFSET                     0xF0
#define TANTOS_3G_SCR0_STORM_U_SHIFT                      13
#define TANTOS_3G_SCR0_STORM_U_SIZE                       1
/* Bit: 'STORM_100_TH' */
/* Description: '100M Threshold' */
#define TANTOS_3G_SCR0_STORM_100_TH_OFFSET                0xF0
#define TANTOS_3G_SCR0_STORM_100_TH_SHIFT                 0
#define TANTOS_3G_SCR0_STORM_100_TH_SIZE                  13
/* -------------------------------------------------------------------------- */
/* Register: 'Storm Control Register 1' */
/* Bit: 'STORM_10_TH' */
/* Description: '10M Threshold' */
#define TANTOS_3G_SCR1_STORM_10_TH_OFFSET                 0xF1
#define TANTOS_3G_SCR1_STORM_10_TH_SHIFT                  0
#define TANTOS_3G_SCR1_STORM_10_TH_SIZE                   13
/* -------------------------------------------------------------------------- */
/* Register: 'Management Clock Select Register' */
/* Bit: 'MCS' */
/* Description: 'Management Clock Select' */
#define TANTOS_3G_MCSR_MCS_OFFSET                         0xF2
#define TANTOS_3G_MCSR_MCS_SHIFT                          0
#define TANTOS_3G_MCSR_MCS_SIZE                           8
/* -------------------------------------------------------------------------- */
/* Register: 'RGMII/GMII Port Control Register' */
/* Bit: 'P6SPD' */
/* Description: 'Port 6 Speed' */
#define TANTOS_3G_RGMIICR_P6SPD_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P6SPD_SHIFT                     10
#define TANTOS_3G_RGMIICR_P6SPD_SIZE                      2
/* Bit: 'P6DUP' */
/* Description: 'Port 6 Duplex mode' */
#define TANTOS_3G_RGMIICR_P6DUP_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P6DUP_SHIFT                     9
#define TANTOS_3G_RGMIICR_P6DUP_SIZE                      1
/* Bit: 'P6FCE' */
/* Description: 'Port 6 Flow Control Enable' */
#define TANTOS_3G_RGMIICR_P6FCE_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P6FCE_SHIFT                     8
#define TANTOS_3G_RGMIICR_P6FCE_SIZE                      1
/* Bit: 'P5SPD' */
/* Description: 'Port 5 Speed' */
#define TANTOS_3G_RGMIICR_P5SPD_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P5SPD_SHIFT                     6
#define TANTOS_3G_RGMIICR_P5SPD_SIZE                      2
/* Bit: 'P5DUP' */
/* Description: 'Port 5 Duplex mode' */
#define TANTOS_3G_RGMIICR_P5DUP_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P5DUP_SHIFT                     5
#define TANTOS_3G_RGMIICR_P5DUP_SIZE                      1
/* Bit: 'P5FCE' */
/* Description: 'Port 5 Flow Control Enable' */
#define TANTOS_3G_RGMIICR_P5FCE_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P5FCE_SHIFT                     4
#define TANTOS_3G_RGMIICR_P5FCE_SIZE                      1
/* Bit: 'P4SPD' */
/* Description: 'Port 4 Speed' */
#define TANTOS_3G_RGMIICR_P4SPD_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P4SPD_SHIFT                     2
#define TANTOS_3G_RGMIICR_P4SPD_SIZE                      2
/* Bit: 'P4DUP' */
/* Description: 'Port 4 Duplex mode' */
#define TANTOS_3G_RGMIICR_P4DUP_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P4DUP_SHIFT                     1
#define TANTOS_3G_RGMIICR_P4DUP_SIZE                      1
/* Bit: 'P4FCE' */
/* Description: 'Port 4 Flow Control Enable' */
#define TANTOS_3G_RGMIICR_P4FCE_OFFSET                    0xF5
#define TANTOS_3G_RGMIICR_P4FCE_SHIFT                     0
#define TANTOS_3G_RGMIICR_P4FCE_SIZE                      1
/* -------------------------------------------------------------------------- */
/* Register: 'Hardware IGMP Option Register' */
/* Bit: 'TIMERC' */
/* Description: 'Timer configuration for IGMP' */
#define TANTOS_3G_HIOR_TIMERC_OFFSET                      0xF6
#define TANTOS_3G_HIOR_TIMERC_SHIFT                       9
#define TANTOS_3G_HIOR_TIMERC_SIZE                        1
/* Bit: 'HISE' */
/* Description: 'Hardware IGMP Snooping Enable' */
#define TANTOS_3G_HIOR_HISE_OFFSET                        0xF6
#define TANTOS_3G_HIOR_HISE_SHIFT                         8
#define TANTOS_3G_HIOR_HISE_SIZE                          1
/* Bit: 'HISFL' */
/* Description: 'Hardware IGMP Snooping fast Leave option' */
#define TANTOS_3G_HIOR_HISFL_OFFSET                       0xF6
#define TANTOS_3G_HIOR_HISFL_SHIFT                        7
#define TANTOS_3G_HIOR_HISFL_SIZE                         1
/* Bit: 'IGMPV3E' */
/* Description: 'IGMPv3 enable' */
#define TANTOS_3G_HIOR_IGMPV3E_OFFSET                     0xF6
#define TANTOS_3G_HIOR_IGMPV3E_SHIFT                      6
#define TANTOS_3G_HIOR_IGMPV3E_SIZE                       1
/* Bit: 'SARE' */
/* Description: 'SA Replacement Enable' */
#define TANTOS_3G_HIOR_SARE_OFFSET                        0xF6
#define TANTOS_3G_HIOR_SARE_SHIFT                         5
#define TANTOS_3G_HIOR_SARE_SIZE                          1
/* Bit: 'PPPoEHR' */
/* Description: 'PPPoE header remove for IGMP data stream enable' */
#define TANTOS_3G_HIOR_PPPOEHR_OFFSET                     0xF6
#define TANTOS_3G_HIOR_PPPOEHR_SHIFT                      4
#define TANTOS_3G_HIOR_PPPOEHR_SIZE                       1
/* Bit: 'B33' */
/* Description: 'Broadcast 33.33.0.0.0.x disable' */
#define TANTOS_3G_HIOR_B33_OFFSET                         0xF6
#define TANTOS_3G_HIOR_B33_SHIFT                          3
#define TANTOS_3G_HIOR_B33_SIZE                           1
/* Bit: 'B01' */
/* Description: 'Broadcast 01.0.5E.0.0.x disable' */
#define TANTOS_3G_HIOR_B01_OFFSET                         0xF6
#define TANTOS_3G_HIOR_B01_SHIFT                          2
#define TANTOS_3G_HIOR_B01_SIZE                           1
/* Bit: 'B224' */
/* Description: 'Broadcast 224.0.0.x disable' */
#define TANTOS_3G_HIOR_B224_OFFSET                        0xF6
#define TANTOS_3G_HIOR_B224_SHIFT                         1
#define TANTOS_3G_HIOR_B224_SIZE                          1
/* Bit: 'DAIPS' */
/* Description: 'DA or IP for Search group ID' */
#define TANTOS_3G_HIOR_DAIPS_OFFSET                       0xF6
#define TANTOS_3G_HIOR_DAIPS_SHIFT                        0
#define TANTOS_3G_HIOR_DAIPS_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'PPPoE Session ID Register' */
/* Bit: 'PPPoESID' */
/* Description: 'PPPoE Session ID' */
#define TANTOS_3G_PSIDR_PPPOESID_OFFSET                   0xF7
#define TANTOS_3G_PSIDR_PPPOESID_SHIFT                    0
#define TANTOS_3G_PSIDR_PPPOESID_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Chip Identifier 0' */
/* Bit: 'BOND' */
/* Description: 'Bonding Option' */
#define TANTOS_3G_CI0_BOND_OFFSET                         0x100
#define TANTOS_3G_CI0_BOND_SHIFT                          4
#define TANTOS_3G_CI0_BOND_SIZE                           1
/* Bit: 'VN' */
/* Description: 'Version Number' */
#define TANTOS_3G_CI0_VN_OFFSET                           0x100
#define TANTOS_3G_CI0_VN_SHIFT                            0
#define TANTOS_3G_CI0_VN_SIZE                             4
/* -------------------------------------------------------------------------- */
/* Register: 'Chip Identifier 1' */
/* Bit: 'PC' */
/* Description: 'Product Code' */
#define TANTOS_3G_CI1_PC_OFFSET                           0x101
#define TANTOS_3G_CI1_PC_SHIFT                            0
#define TANTOS_3G_CI1_PC_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Global Status and Hardware Setting Register' */
/* Bit: 'LTBR' */
/* Description: 'Learning Table Bist Result' */
#define TANTOS_3G_GSHS_LTBR_OFFSET                        0x102
#define TANTOS_3G_GSHS_LTBR_SHIFT                         11
#define TANTOS_3G_GSHS_LTBR_SIZE                          1
/* Bit: 'LLTBR' */
/* Description: 'Linklist Table Bist Result' */
#define TANTOS_3G_GSHS_LLTBR_OFFSET                       0x102
#define TANTOS_3G_GSHS_LLTBR_SHIFT                        10
#define TANTOS_3G_GSHS_LLTBR_SIZE                         1
/* Bit: 'CTBR' */
/* Description: 'Control Table Bist Result' */
#define TANTOS_3G_GSHS_CTBR_OFFSET                        0x102
#define TANTOS_3G_GSHS_CTBR_SHIFT                         9
#define TANTOS_3G_GSHS_CTBR_SIZE                          1
/* Bit: 'HISTBR' */
/* Description: 'Hardware IGMP Source List Table Bist Result' */
#define TANTOS_3G_GSHS_HISTBR_OFFSET                      0x102
#define TANTOS_3G_GSHS_HISTBR_SHIFT                       8
#define TANTOS_3G_GSHS_HISTBR_SIZE                        1
/* Bit: 'HIGTBR' */
/* Description: 'Hardware IGMP Group Table Bist Result' */
#define TANTOS_3G_GSHS_HIGTBR_OFFSET                      0x102
#define TANTOS_3G_GSHS_HIGTBR_SHIFT                       7
#define TANTOS_3G_GSHS_HIGTBR_SIZE                        1
/* Bit: 'DBBR' */
/* Description: 'Data Buffer Bist Result' */
#define TANTOS_3G_GSHS_DBBR_OFFSET                        0x102
#define TANTOS_3G_GSHS_DBBR_SHIFT                         6
#define TANTOS_3G_GSHS_DBBR_SIZE                          1
/* Bit: 'P6M' */
/* Description: 'P6 Mode' */
#define TANTOS_3G_GSHS_P6M_OFFSET                         0x102
#define TANTOS_3G_GSHS_P6M_SHIFT                          4
#define TANTOS_3G_GSHS_P6M_SIZE                           2
/* Bit: 'P5M' */
/* Description: 'P5 Mode' */
#define TANTOS_3G_GSHS_P5M_OFFSET                         0x102
#define TANTOS_3G_GSHS_P5M_SHIFT                          2
#define TANTOS_3G_GSHS_P5M_SIZE                           2
/* Bit: 'P4M' */
/* Description: 'P4 Mode' */
#define TANTOS_3G_GSHS_P4M_OFFSET                         0x102
#define TANTOS_3G_GSHS_P4M_SHIFT                          0
#define TANTOS_3G_GSHS_P4M_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 0 Register' */
/* Bit: 'ADDR15_0' */
/* Description: 'Address [15:0]' */
#define TANTOS_3G_ATC0_ADDR15_0_OFFSET                    0x104
#define TANTOS_3G_ATC0_ADDR15_0_SHIFT                     0
#define TANTOS_3G_ATC0_ADDR15_0_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 1 Register' */
/* Bit: 'ADDR31_16' */
/* Description: 'Address [31:16]' */
#define TANTOS_3G_ATC1_ADDR31_16_OFFSET                   0x105
#define TANTOS_3G_ATC1_ADDR31_16_SHIFT                    0
#define TANTOS_3G_ATC1_ADDR31_16_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 2 Register' */
/* Bit: 'ADDR47_32' */
/* Description: 'Address [47:32]' */
#define TANTOS_3G_ATC2_ADDR47_32_OFFSET                   0x106
#define TANTOS_3G_ATC2_ADDR47_32_SHIFT                    0
#define TANTOS_3G_ATC2_ADDR47_32_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 3 Register' */
/* Bit: 'PMAP' */
/* Description: 'Port Map' */
#define TANTOS_3G_ATC3_PMAP_OFFSET                        0x107
#define TANTOS_3G_ATC3_PMAP_SHIFT                         4
#define TANTOS_3G_ATC3_PMAP_SIZE                          7
/* Bit: 'FID' */
/* Description: 'FID group' */
#define TANTOS_3G_ATC3_FID_OFFSET                         0x107
#define TANTOS_3G_ATC3_FID_SHIFT                          0
#define TANTOS_3G_ATC3_FID_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 4 Register' */
/* Bit: 'INFOT' */
/* Description: 'Info Type: Static address' */
#define TANTOS_3G_ATC4_INFOT_OFFSET                       0x108
#define TANTOS_3G_ATC4_INFOT_SHIFT                        12
#define TANTOS_3G_ATC4_INFOT_SIZE                         1
/* Bit: 'ITAT' */
/* Description: 'Info_Ctrl/Age Timer' */
#define TANTOS_3G_ATC4_ITAT_OFFSET                        0x108
#define TANTOS_3G_ATC4_ITAT_SHIFT                         0
#define TANTOS_3G_ATC4_ITAT_SIZE                          11
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Control 5 Register' */
/* Bit: 'FCE' */
/* Description: 'Find within the current entry' */
#define TANTOS_3G_ATC5_FCE_OFFSET                         0x109
#define TANTOS_3G_ATC5_FCE_SHIFT                          7
#define TANTOS_3G_ATC5_FCE_SIZE                           1
/* Bit: 'CMD' */
/* Description: 'Command' */
#define TANTOS_3G_ATC5_CMD_OFFSET                         0x109
#define TANTOS_3G_ATC5_CMD_SHIFT                          4
#define TANTOS_3G_ATC5_CMD_SIZE                           3
/* Bit: 'AC' */
/* Description: 'Access Control' */
#define TANTOS_3G_ATC5_AC_OFFSET                          0x109
#define TANTOS_3G_ATC5_AC_SHIFT                           0
#define TANTOS_3G_ATC5_AC_SIZE                            4
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 0 Register' */
/* Bit: 'ADDRS15_0' */
/* Description: 'Address [15:0]' */
#define TANTOS_3G_ATS0_ADDRS15_0_OFFSET                   0x10A
#define TANTOS_3G_ATS0_ADDRS15_0_SHIFT                    0
#define TANTOS_3G_ATS0_ADDRS15_0_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 1 Register' */
/* Bit: 'ADDRS31_16' */
/* Description: 'Address [31:16]' */
#define TANTOS_3G_ATS1_ADDRS31_16_OFFSET                  0x10B
#define TANTOS_3G_ATS1_ADDRS31_16_SHIFT                   0
#define TANTOS_3G_ATS1_ADDRS31_16_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 2 Register' */
/* Bit: 'ADDRS47_32' */
/* Description: 'Address [47:32]' */
#define TANTOS_3G_ATS2_ADDRS47_32_OFFSET                  0x10C
#define TANTOS_3G_ATS2_ADDRS47_32_SHIFT                   0
#define TANTOS_3G_ATS2_ADDRS47_32_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 3 Register' */
/* Bit: 'PMAPS' */
/* Description: 'Port Map' */
#define TANTOS_3G_ATS3_PMAPS_OFFSET                       0x10D
#define TANTOS_3G_ATS3_PMAPS_SHIFT                        4
#define TANTOS_3G_ATS3_PMAPS_SIZE                         7
/* Bit: 'FIDS' */
/* Description: 'FID group' */
#define TANTOS_3G_ATS3_FIDS_OFFSET                        0x10D
#define TANTOS_3G_ATS3_FIDS_SHIFT                         0
#define TANTOS_3G_ATS3_FIDS_SIZE                          2
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 4 Register' */
/* Bit: 'BAD' */
/* Description: 'Bad Status' */
#define TANTOS_3G_ATS4_BAD_OFFSET                         0x10E
#define TANTOS_3G_ATS4_BAD_SHIFT                          14
#define TANTOS_3G_ATS4_BAD_SIZE                           1
/* Bit: 'OCP' */
/* Description: 'Occupy' */
#define TANTOS_3G_ATS4_OCP_OFFSET                         0x10E
#define TANTOS_3G_ATS4_OCP_SHIFT                          13
#define TANTOS_3G_ATS4_OCP_SIZE                           1
/* Bit: 'INFOTS' */
/* Description: 'Info Type: Static address' */
#define TANTOS_3G_ATS4_INFOTS_OFFSET                      0x10E
#define TANTOS_3G_ATS4_INFOTS_SHIFT                       12
#define TANTOS_3G_ATS4_INFOTS_SIZE                        1
/* Bit: 'ITATS' */
/* Description: 'Info_Ctrl/Age Timer Status' */
#define TANTOS_3G_ATS4_ITATS_OFFSET                       0x10E
#define TANTOS_3G_ATS4_ITATS_SHIFT                        0
#define TANTOS_3G_ATS4_ITATS_SIZE                         11
/* -------------------------------------------------------------------------- */
/* Register: 'Address Table Status 5 Register' */
/* Bit: 'BUSY' */
/* Description: 'Busy' */
#define TANTOS_3G_ATS5_BUSY_OFFSET                        0x10F
#define TANTOS_3G_ATS5_BUSY_SHIFT                         15
#define TANTOS_3G_ATS5_BUSY_SIZE                          1
/* Bit: 'RSLT' */
/* Description: 'Result' */
#define TANTOS_3G_ATS5_RSLT_OFFSET                        0x10F
#define TANTOS_3G_ATS5_RSLT_SHIFT                         12
#define TANTOS_3G_ATS5_RSLT_SIZE                          3
/* Bit: 'FCE' */
/* Description: 'Find within the current entry' */
#define TANTOS_3G_ATS5_FCE_OFFSET                         0x10F
#define TANTOS_3G_ATS5_FCE_SHIFT                          7
#define TANTOS_3G_ATS5_FCE_SIZE                           1
/* Bit: 'CMD' */
/* Description: 'Command' */
#define TANTOS_3G_ATS5_CMD_OFFSET                         0x10F
#define TANTOS_3G_ATS5_CMD_SHIFT                          4
#define TANTOS_3G_ATS5_CMD_SIZE                           3
/* Bit: 'AC' */
/* Description: 'Access Control' */
#define TANTOS_3G_ATS5_AC_OFFSET                          0x10F
#define TANTOS_3G_ATS5_AC_SHIFT                           0
#define TANTOS_3G_ATS5_AC_SIZE                            4
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Control 0 Register' */
/* Bit: 'SIP15_0' */
/* Description: 'Source IP[15:0]' */
#define TANTOS_3G_IGMPTC0_SIP15_0_OFFSET                  0x110
#define TANTOS_3G_IGMPTC0_SIP15_0_SHIFT                   0
#define TANTOS_3G_IGMPTC0_SIP15_0_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Control 1 Register' */
/* Bit: 'SIP31_16' */
/* Description: 'Source IP[31:16]' */
#define TANTOS_3G_IGMPTC1_SIP31_16_OFFSET                 0x111
#define TANTOS_3G_IGMPTC1_SIP31_16_SHIFT                  0
#define TANTOS_3G_IGMPTC1_SIP31_16_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Control 2 Register' */
/* Bit: 'SIP47_32' */
/* Description: 'Source IP[47:32] for IPv6 MLD' */
#define TANTOS_3G_IGMPTC2_SIP47_32_OFFSET                 0x112
#define TANTOS_3G_IGMPTC2_SIP47_32_SHIFT                  0
#define TANTOS_3G_IGMPTC2_SIP47_32_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Control 3 Register' */
/* Bit: 'GID15_0' */
/* Description: 'Group ID [15:0]' */
#define TANTOS_3G_IGMPTC3_GID15_0_OFFSET                  0x113
#define TANTOS_3G_IGMPTC3_GID15_0_SHIFT                   0
#define TANTOS_3G_IGMPTC3_GID15_0_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Control 4 Register' */
/* Bit: 'GID31_16' */
/* Description: 'Group ID [31:16]. For IPv4 only [23:16] are used.' */
#define TANTOS_3G_IGMPTC4_GID31_16_OFFSET                 0x114
#define TANTOS_3G_IGMPTC4_GID31_16_SHIFT                  0
#define TANTOS_3G_IGMPTC4_GID31_16_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Control 5 Register' */
/* Bit: 'ICMD' */
/* Description: 'IGMP Command' */
#define TANTOS_3G_IGMPTC5_ICMD_OFFSET                     0x115
#define TANTOS_3G_IGMPTC5_ICMD_SHIFT                      8
#define TANTOS_3G_IGMPTC5_ICMD_SIZE                       3
/* Bit: 'INVC' */
/* Description: 'Invalid control' */
#define TANTOS_3G_IGMPTC5_INVC_OFFSET                     0x115
#define TANTOS_3G_IGMPTC5_INVC_SHIFT                      5
#define TANTOS_3G_IGMPTC5_INVC_SIZE                       1
/* Bit: 'FMODE' */
/* Description: 'Filter Mode' */
#define TANTOS_3G_IGMPTC5_FMODE_OFFSET                    0x115
#define TANTOS_3G_IGMPTC5_FMODE_SHIFT                     4
#define TANTOS_3G_IGMPTC5_FMODE_SIZE                      1
/* Bit: 'PORT' */
/* Description: 'Port number associated with this command' */
#define TANTOS_3G_IGMPTC5_PORT_OFFSET                     0x115
#define TANTOS_3G_IGMPTC5_PORT_SHIFT                      0
#define TANTOS_3G_IGMPTC5_PORT_SIZE                       3
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Status 0 Register' */
/* Bit: 'SIPGID0' */
/* Description: 'Source IP or Group ID bit [15:0]' */
#define TANTOS_3G_IGMPTS0_SIPGID0_OFFSET                  0x116
#define TANTOS_3G_IGMPTS0_SIPGID0_SHIFT                   0
#define TANTOS_3G_IGMPTS0_SIPGID0_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Status 1 Register' */
/* Bit: 'SIPGID1' */
/* Description: 'Source IP or Group ID bit [31:16]' */
#define TANTOS_3G_IGMPTS1_SIPGID1_OFFSET                  0x117
#define TANTOS_3G_IGMPTS1_SIPGID1_SHIFT                   0
#define TANTOS_3G_IGMPTS1_SIPGID1_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Status 2 Register' */
/* Bit: 'SIPGID2' */
/* Description: 'Source IP or Group ID bit [47:32]' */
#define TANTOS_3G_IGMPTS2_SIPGID2_OFFSET                  0x118
#define TANTOS_3G_IGMPTS2_SIPGID2_SHIFT                   0
#define TANTOS_3G_IGMPTS2_SIPGID2_SIZE                    16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Status 3 Register' */
/* Bit: 'S3PMI' */
/* Description: 'Port Map Include/exclude Status' */
#define TANTOS_3G_IGMPTS3_S3PMI_OFFSET                    0x119
#define TANTOS_3G_IGMPTS3_S3PMI_SHIFT                     7
#define TANTOS_3G_IGMPTS3_S3PMI_SIZE                      7
/* Bit: 'S3PMV' */
/* Description: 'Port Map Valid Status' */
#define TANTOS_3G_IGMPTS3_S3PMV_OFFSET                    0x119
#define TANTOS_3G_IGMPTS3_S3PMV_SHIFT                     0
#define TANTOS_3G_IGMPTS3_S3PMV_SIZE                      7
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Table Status 4 Register' */
/* Bit: 'S4BUSY' */
/* Description: 'Busy' */
#define TANTOS_3G_IGMPTS4_S4BUSY_OFFSET                   0x11A
#define TANTOS_3G_IGMPTS4_S4BUSY_SHIFT                    3
#define TANTOS_3G_IGMPTS4_S4BUSY_SIZE                     1
/* Bit: 'S4R' */
/* Description: 'Result' */
#define TANTOS_3G_IGMPTS4_S4R_OFFSET                      0x11A
#define TANTOS_3G_IGMPTS4_S4R_SHIFT                       0
#define TANTOS_3G_IGMPTS4_S4R_SIZE                        3
/* -------------------------------------------------------------------------- */
/* Register: 'RMON Counter Control Register' */
/* Bit: 'BAS' */
/* Description: 'Busy/Access Start' */
#define TANTOS_3G_RCC_BAS_OFFSET                          0x11B
#define TANTOS_3G_RCC_BAS_SHIFT                           11
#define TANTOS_3G_RCC_BAS_SIZE                            1
/* Bit: 'CAC' */
/* Description: 'Command for access counter' */
#define TANTOS_3G_RCC_CAC_OFFSET                          0x11B
#define TANTOS_3G_RCC_CAC_SHIFT                           9
#define TANTOS_3G_RCC_CAC_SIZE                            2
/* Bit: 'PORTC' */
/* Description: 'Port' */
#define TANTOS_3G_RCC_PORTC_OFFSET                        0x11B
#define TANTOS_3G_RCC_PORTC_SHIFT                         6
#define TANTOS_3G_RCC_PORTC_SIZE                          3
/* Bit: 'OFFSET' */
/* Description: 'Counter Offset' */
#define TANTOS_3G_RCC_OFFSET_OFFSET                       0x11B
#define TANTOS_3G_RCC_OFFSET_SHIFT                        0
#define TANTOS_3G_RCC_OFFSET_SIZE                         6
/* -------------------------------------------------------------------------- */
/* Register: 'RMON Counter Status Low Register' */
/* Bit: 'COUNTER' */
/* Description: 'Counter [15:0] or Counter[47:32] for byte count' */
#define TANTOS_3G_RCSL_COUNTER_OFFSET                     0x11C
#define TANTOS_3G_RCSL_COUNTER_SHIFT                      0
#define TANTOS_3G_RCSL_COUNTER_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'RMON Counter Status High Register' */
/* Bit: 'COUNTER' */
/* Description: 'Counter [31:16] or Counter[63:48] for byte count' */
#define TANTOS_3G_RCSH_COUNTER_OFFSET                     0x11D
#define TANTOS_3G_RCSH_COUNTER_SHIFT                      0
#define TANTOS_3G_RCSH_COUNTER_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'MII Indirect Access Control' */
/* Bit: 'MBUSY' */
/* Description: 'Busy state' */
#define TANTOS_3G_MIIAC_MBUSY_OFFSET                      0x120
#define TANTOS_3G_MIIAC_MBUSY_SHIFT                       15
#define TANTOS_3G_MIIAC_MBUSY_SIZE                        1
/* Bit: 'OP' */
/* Description: 'Operation Code' */
#define TANTOS_3G_MIIAC_OP_OFFSET                         0x120
#define TANTOS_3G_MIIAC_OP_SHIFT                          10
#define TANTOS_3G_MIIAC_OP_SIZE                           2
/* Bit: 'PHYAD' */
/* Description: 'PHY Address' */
#define TANTOS_3G_MIIAC_PHYAD_OFFSET                      0x120
#define TANTOS_3G_MIIAC_PHYAD_SHIFT                       5
#define TANTOS_3G_MIIAC_PHYAD_SIZE                        5
/* Bit: 'REGAD' */
/* Description: 'Register Address' */
#define TANTOS_3G_MIIAC_REGAD_OFFSET                      0x120
#define TANTOS_3G_MIIAC_REGAD_SHIFT                       0
#define TANTOS_3G_MIIAC_REGAD_SIZE                        5
/* -------------------------------------------------------------------------- */
/* Register: 'MII Indirect Write Data' */
/* Bit: 'WD' */
/* Description: 'The Write Data to the MII register' */
#define TANTOS_3G_MIIWD_WD_OFFSET                         0x121
#define TANTOS_3G_MIIWD_WD_SHIFT                          0
#define TANTOS_3G_MIIWD_WD_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'MII Indirect Read Data' */
/* Bit: 'RD' */
/* Description: 'The Read Data' */
#define TANTOS_3G_MIIRD_RD_OFFSET                         0x122
#define TANTOS_3G_MIIRD_RD_SHIFT                          0
#define TANTOS_3G_MIIRD_RD_SIZE                           16
/* -------------------------------------------------------------------------- */
#endif /* #ifndef _TANTOS3G_H */
