/******************************************************************************

                               Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _VR9_SWITCH_H
#define _VR9_SWITCH_H
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch ResetControl Register' */
/* Bit: 'R1' */
/* Description: 'GSWIP Software Reset' */
#define VR9_ETHSW_SWRES_R1_OFFSET                         0x000
#define VR9_ETHSW_SWRES_R1_SHIFT                          1
#define VR9_ETHSW_SWRES_R1_SIZE                           1
/* Bit: 'R0' */
/* Description: 'GSWIP Hardware Reset' */
#define VR9_ETHSW_SWRES_R0_OFFSET                         0x000
#define VR9_ETHSW_SWRES_R0_SHIFT                          0
#define VR9_ETHSW_SWRES_R0_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch Clock ControlRegister ' */
/* Bit: 'CORACT_OVR' */
/* Description: 'Core Active Override' */
#define VR9_ETHSW_CLK_CORACT_OVR_OFFSET                   0x001
#define VR9_ETHSW_CLK_CORACT_OVR_SHIFT                    1
#define VR9_ETHSW_CLK_CORACT_OVR_SIZE                     1
/* Bit: 'LNKDWN_OVR' */
/* Description: 'Link Down Override' */
#define VR9_ETHSW_CLK_LNKDWN_OVR_OFFSET                   0x001
#define VR9_ETHSW_CLK_LNKDWN_OVR_SHIFT                    0
#define VR9_ETHSW_CLK_LNKDWN_OVR_SIZE                     1
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch SharedSegment Buffer Mode Register' */
/* Bit: 'ADDE' */
/* Description: 'Memory Address' */
#define VR9_ETHSW_SSB_MODE_ADDE_OFFSET                    0x003
#define VR9_ETHSW_SSB_MODE_ADDE_SHIFT                     2
#define VR9_ETHSW_SSB_MODE_ADDE_SIZE                      4
/* Bit: 'MODE' */
/* Description: 'Memory Access Mode' */
#define VR9_ETHSW_SSB_MODE_MODE_OFFSET                    0x003
#define VR9_ETHSW_SSB_MODE_MODE_SHIFT                     0
#define VR9_ETHSW_SSB_MODE_MODE_SIZE                      2
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch SharedSegment Buffer Address Register' */
/* Bit: 'ADDE' */
/* Description: 'Memory Address' */
#define VR9_ETHSW_SSB_ADDR_ADDE_OFFSET                    0x004
#define VR9_ETHSW_SSB_ADDR_ADDE_SHIFT                     0
#define VR9_ETHSW_SSB_ADDR_ADDE_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch SharedSegment Buffer Data Register' */
/* Bit: 'DATA' */
/* Description: 'Data Value' */
#define VR9_ETHSW_SSB_DATA_DATA_OFFSET                    0x005
#define VR9_ETHSW_SSB_DATA_DATA_SHIFT                     0
#define VR9_ETHSW_SSB_DATA_DATA_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 0' */
/* Bit: 'SPEED' */
/* Description: 'Clock frequency' */
#define VR9_ETHSW_CAP_0_SPEED_OFFSET                      0x006
#define VR9_ETHSW_CAP_0_SPEED_SHIFT                       0
#define VR9_ETHSW_CAP_0_SPEED_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 1' */
/* Bit: 'GMAC' */
/* Description: 'MAC operation mode' */
#define VR9_ETHSW_CAP_1_GMAC_OFFSET                       0x007
#define VR9_ETHSW_CAP_1_GMAC_SHIFT                        15
#define VR9_ETHSW_CAP_1_GMAC_SIZE                         1
/* Bit: 'QUEUE' */
/* Description: 'Number of queues' */
#define VR9_ETHSW_CAP_1_QUEUE_OFFSET                      0x007
#define VR9_ETHSW_CAP_1_QUEUE_SHIFT                       8
#define VR9_ETHSW_CAP_1_QUEUE_SIZE                        7
/* Bit: 'VPORTS' */
/* Description: 'Number of virtual ports' */
#define VR9_ETHSW_CAP_1_VPORTS_OFFSET                     0x007
#define VR9_ETHSW_CAP_1_VPORTS_SHIFT                      4
#define VR9_ETHSW_CAP_1_VPORTS_SIZE                       4
/* Bit: 'PPORTS' */
/* Description: 'Number of physical ports' */
#define VR9_ETHSW_CAP_1_PPORTS_OFFSET                     0x007
#define VR9_ETHSW_CAP_1_PPORTS_SHIFT                      0
#define VR9_ETHSW_CAP_1_PPORTS_SIZE                       4
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 2' */
/* Bit: 'PACKETS' */
/* Description: 'Number of packets' */
#define VR9_ETHSW_CAP_2_PACKETS_OFFSET                    0x008
#define VR9_ETHSW_CAP_2_PACKETS_SHIFT                     0
#define VR9_ETHSW_CAP_2_PACKETS_SIZE                      11
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 3' */
/* Bit: 'METERS' */
/* Description: 'Number of traffic meters' */
#define VR9_ETHSW_CAP_3_METERS_OFFSET                     0x009
#define VR9_ETHSW_CAP_3_METERS_SHIFT                      8
#define VR9_ETHSW_CAP_3_METERS_SIZE                       8
/* Bit: 'SHAPERS' */
/* Description: 'Number of traffic shapers' */
#define VR9_ETHSW_CAP_3_SHAPERS_OFFSET                    0x009
#define VR9_ETHSW_CAP_3_SHAPERS_SHIFT                     0
#define VR9_ETHSW_CAP_3_SHAPERS_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 4' */
/* Bit: 'PPPOE' */
/* Description: 'PPPoE table size' */
#define VR9_ETHSW_CAP_4_PPPOE_OFFSET                      0x00A
#define VR9_ETHSW_CAP_4_PPPOE_SHIFT                       8
#define VR9_ETHSW_CAP_4_PPPOE_SIZE                        8
/* Bit: 'VLAN' */
/* Description: 'Active VLAN table size' */
#define VR9_ETHSW_CAP_4_VLAN_OFFSET                       0x00A
#define VR9_ETHSW_CAP_4_VLAN_SHIFT                        0
#define VR9_ETHSW_CAP_4_VLAN_SIZE                         8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 5' */
/* Bit: 'IPPLEN' */
/* Description: 'IP packet length table size' */
#define VR9_ETHSW_CAP_5_IPPLEN_OFFSET                     0x00B
#define VR9_ETHSW_CAP_5_IPPLEN_SHIFT                      8
#define VR9_ETHSW_CAP_5_IPPLEN_SIZE                       8
/* Bit: 'PROT' */
/* Description: 'Protocol table size' */
#define VR9_ETHSW_CAP_5_PROT_OFFSET                       0x00B
#define VR9_ETHSW_CAP_5_PROT_SHIFT                        0
#define VR9_ETHSW_CAP_5_PROT_SIZE                         8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 6' */
/* Bit: 'MACDASA' */
/* Description: 'MAC DA/SA table size' */
#define VR9_ETHSW_CAP_6_MACDASA_OFFSET                    0x00C
#define VR9_ETHSW_CAP_6_MACDASA_SHIFT                     8
#define VR9_ETHSW_CAP_6_MACDASA_SIZE                      8
/* Bit: 'APPL' */
/* Description: 'Application table size' */
#define VR9_ETHSW_CAP_6_APPL_OFFSET                       0x00C
#define VR9_ETHSW_CAP_6_APPL_SHIFT                        0
#define VR9_ETHSW_CAP_6_APPL_SIZE                         8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 7' */
/* Bit: 'IPDASAM' */
/* Description: 'IP DA/SA MSB table size' */
#define VR9_ETHSW_CAP_7_IPDASAM_OFFSET                    0x00D
#define VR9_ETHSW_CAP_7_IPDASAM_SHIFT                     8
#define VR9_ETHSW_CAP_7_IPDASAM_SIZE                      8
/* Bit: 'IPDASAL' */
/* Description: 'IP DA/SA LSB table size' */
#define VR9_ETHSW_CAP_7_IPDASAL_OFFSET                    0x00D
#define VR9_ETHSW_CAP_7_IPDASAL_SHIFT                     0
#define VR9_ETHSW_CAP_7_IPDASAL_SIZE                      8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 8' */
/* Bit: 'MCAST' */
/* Description: 'Multicast table size' */
#define VR9_ETHSW_CAP_8_MCAST_OFFSET                      0x00E
#define VR9_ETHSW_CAP_8_MCAST_SHIFT                       0
#define VR9_ETHSW_CAP_8_MCAST_SIZE                        8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 9' */
/* Bit: 'FLAGG' */
/* Description: 'Flow Aggregation table size' */
#define VR9_ETHSW_CAP_9_FLAGG_OFFSET                      0x00F
#define VR9_ETHSW_CAP_9_FLAGG_SHIFT                       0
#define VR9_ETHSW_CAP_9_FLAGG_SIZE                        8
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 10' */
/* Bit: 'MACBT' */
/* Description: 'MAC bridging table size' */
#define VR9_ETHSW_CAP_10_MACBT_OFFSET                     0x010
#define VR9_ETHSW_CAP_10_MACBT_SHIFT                      0
#define VR9_ETHSW_CAP_10_MACBT_SIZE                       13
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 11' */
/* Bit: 'BSIZEL' */
/* Description: 'Packet buffer size (lower part, in byte)' */
#define VR9_ETHSW_CAP_11_BSIZEL_OFFSET                    0x011
#define VR9_ETHSW_CAP_11_BSIZEL_SHIFT                     0
#define VR9_ETHSW_CAP_11_BSIZEL_SIZE                      16
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 12' */
/* Bit: 'BSIZEH' */
/* Description: 'Packet buffer size (higher part, in byte)' */
#define VR9_ETHSW_CAP_12_BSIZEH_OFFSET                    0x012
#define VR9_ETHSW_CAP_12_BSIZEH_SHIFT                     0
#define VR9_ETHSW_CAP_12_BSIZEH_SIZE                      3
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch VersionRegister' */
/* Bit: 'MOD_ID' */
/* Description: 'Module Identification' */
#define VR9_ETHSW_VERSION_MOD_ID_OFFSET                   0x013
#define VR9_ETHSW_VERSION_MOD_ID_SHIFT                    8
#define VR9_ETHSW_VERSION_MOD_ID_SIZE                     8
/* Bit: 'REV_ID' */
/* Description: 'Hardware Revision Identification' */
#define VR9_ETHSW_VERSION_REV_ID_OFFSET                   0x013
#define VR9_ETHSW_VERSION_REV_ID_SHIFT                    0
#define VR9_ETHSW_VERSION_REV_ID_SIZE                     8
/* -------------------------------------------------------------------------- */
/* Register: 'Interrupt Enable Register' */
/* Bit: 'FDMAIE' */
/* Description: 'Fetch DMA Interrupt Enable' */
#define VR9_ETHSW_IER_FDMAIE_OFFSET                       0x014
#define VR9_ETHSW_IER_FDMAIE_SHIFT                        4
#define VR9_ETHSW_IER_FDMAIE_SIZE                         1
/* Bit: 'SDMAIE' */
/* Description: 'Store DMA Interrupt Enable' */
#define VR9_ETHSW_IER_SDMAIE_OFFSET                       0x014
#define VR9_ETHSW_IER_SDMAIE_SHIFT                        3
#define VR9_ETHSW_IER_SDMAIE_SIZE                         1
/* Bit: 'MACIE' */
/* Description: 'Ethernet MAC Interrupt Enable' */
#define VR9_ETHSW_IER_MACIE_OFFSET                        0x014
#define VR9_ETHSW_IER_MACIE_SHIFT                         2
#define VR9_ETHSW_IER_MACIE_SIZE                          1
/* Bit: 'PCEIE' */
/* Description: 'Parser and Classification Engine Interrupt Enable' */
#define VR9_ETHSW_IER_PCEIE_OFFSET                        0x014
#define VR9_ETHSW_IER_PCEIE_SHIFT                         1
#define VR9_ETHSW_IER_PCEIE_SIZE                          1
/* Bit: 'BMIE' */
/* Description: 'Buffer Manager Interrupt Enable' */
#define VR9_ETHSW_IER_BMIE_OFFSET                         0x014
#define VR9_ETHSW_IER_BMIE_SHIFT                          0
#define VR9_ETHSW_IER_BMIE_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Interrupt Status Register' */
/* Bit: 'FDMAINT' */
/* Description: 'Fetch DMA Interrupt' */
#define VR9_ETHSW_ISR_FDMAINT_OFFSET                      0x015
#define VR9_ETHSW_ISR_FDMAINT_SHIFT                       4
#define VR9_ETHSW_ISR_FDMAINT_SIZE                        1
/* Bit: 'SDMAINT' */
/* Description: 'Store DMA Interrupt' */
#define VR9_ETHSW_ISR_SDMAINT_OFFSET                      0x015
#define VR9_ETHSW_ISR_SDMAINT_SHIFT                       3
#define VR9_ETHSW_ISR_SDMAINT_SIZE                        1
/* Bit: 'MACINT' */
/* Description: 'Ethernet MAC Interrupt' */
#define VR9_ETHSW_ISR_MACINT_OFFSET                       0x015
#define VR9_ETHSW_ISR_MACINT_SHIFT                        2
#define VR9_ETHSW_ISR_MACINT_SIZE                         1
/* Bit: 'PCEINT' */
/* Description: 'Parser and Classification Engine Interrupt' */
#define VR9_ETHSW_ISR_PCEINT_OFFSET                       0x015
#define VR9_ETHSW_ISR_PCEINT_SHIFT                        1
#define VR9_ETHSW_ISR_PCEINT_SIZE                         1
/* Bit: 'BMINT' */
/* Description: 'Buffer Manager Interrupt' */
#define VR9_ETHSW_ISR_BMINT_OFFSET                        0x015
#define VR9_ETHSW_ISR_BMINT_SHIFT                         0
#define VR9_ETHSW_ISR_BMINT_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'RAM Value Register 3' */
/* Bit: 'VAL3' */
/* Description: 'Data value [15:0]' */
#define VR9_BM_RAM_VAL_3_VAL3_OFFSET                      0x040
#define VR9_BM_RAM_VAL_3_VAL3_SHIFT                       0
#define VR9_BM_RAM_VAL_3_VAL3_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'RAM Value Register 2' */
/* Bit: 'VAL2' */
/* Description: 'Data value [15:0]' */
#define VR9_BM_RAM_VAL_2_VAL2_OFFSET                      0x041
#define VR9_BM_RAM_VAL_2_VAL2_SHIFT                       0
#define VR9_BM_RAM_VAL_2_VAL2_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'RAM Value Register 1' */
/* Bit: 'VAL1' */
/* Description: 'Data value [15:0]' */
#define VR9_BM_RAM_VAL_1_VAL1_OFFSET                      0x042
#define VR9_BM_RAM_VAL_1_VAL1_SHIFT                       0
#define VR9_BM_RAM_VAL_1_VAL1_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'RAM Value Register 0' */
/* Bit: 'VAL0' */
/* Description: 'Data value [15:0]' */
#define VR9_BM_RAM_VAL_0_VAL0_OFFSET                      0x043
#define VR9_BM_RAM_VAL_0_VAL0_SHIFT                       0
#define VR9_BM_RAM_VAL_0_VAL0_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'RAM Address Register' */
/* Bit: 'ADDR' */
/* Description: 'RAM Address' */
#define VR9_BM_RAM_ADDR_ADDR_OFFSET                       0x044
#define VR9_BM_RAM_ADDR_ADDR_SHIFT                        0
#define VR9_BM_RAM_ADDR_ADDR_SIZE                         11
/* -------------------------------------------------------------------------- */
/* Register: 'RAM Access Control Register' */
/* Bit: 'BAS' */
/* Description: 'Access Busy/Access Start' */
#define VR9_BM_RAM_CTRL_BAS_OFFSET                        0x045
#define VR9_BM_RAM_CTRL_BAS_SHIFT                         15
#define VR9_BM_RAM_CTRL_BAS_SIZE                          1
/* Bit: 'OPMOD' */
/* Description: 'Lookup Table Access Operation Mode' */
#define VR9_BM_RAM_CTRL_OPMOD_OFFSET                      0x045
#define VR9_BM_RAM_CTRL_OPMOD_SHIFT                       5
#define VR9_BM_RAM_CTRL_OPMOD_SIZE                        1
/* Bit: 'ADDR' */
/* Description: 'Address for RAM selection' */
#define VR9_BM_RAM_CTRL_ADDR_OFFSET                       0x045
#define VR9_BM_RAM_CTRL_ADDR_SHIFT                        0
#define VR9_BM_RAM_CTRL_ADDR_SIZE                         5
/* -------------------------------------------------------------------------- */
/* Register: 'Free Segment Queue ManagerGlobal Control Register' */
/* Bit: 'SEGNUM' */
/* Description: 'Maximum Segment Number' */
#define VR9_BM_FSQM_GCTRL_SEGNUM_OFFSET                   0x046
#define VR9_BM_FSQM_GCTRL_SEGNUM_SHIFT                    0
#define VR9_BM_FSQM_GCTRL_SEGNUM_SIZE                     10
/* -------------------------------------------------------------------------- */
/* Register: 'Number of Consumed SegmentsRegister' */
/* Bit: 'FSEG' */
/* Description: 'Number of Consumed Segments' */
#define VR9_BM_CONS_SEG_FSEG_OFFSET                       0x047
#define VR9_BM_CONS_SEG_FSEG_SHIFT                        0
#define VR9_BM_CONS_SEG_FSEG_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'Number of Consumed PacketPointers Register' */
/* Bit: 'FQP' */
/* Description: 'Number of Consumed Packet Pointers' */
#define VR9_BM_CONS_PKT_FQP_OFFSET                        0x048
#define VR9_BM_CONS_PKT_FQP_SHIFT                         0
#define VR9_BM_CONS_PKT_FQP_SIZE                          11
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer Manager Global ControlRegister 0' */
/* Bit: 'BM_STA' */
/* Description: 'Buffer Manager Initialization Status Bit' */
#define VR9_BM_GCTRL_BM_STA_OFFSET                        0x049
#define VR9_BM_GCTRL_BM_STA_SHIFT                         13
#define VR9_BM_GCTRL_BM_STA_SIZE                          1
/* Bit: 'SAT' */
/* Description: 'RMON Counter Update Mode' */
#define VR9_BM_GCTRL_SAT_OFFSET                           0x049
#define VR9_BM_GCTRL_SAT_SHIFT                            12
#define VR9_BM_GCTRL_SAT_SIZE                             1
/* Bit: 'FR_RBC' */
/* Description: 'Freeze RMON RX Bad Byte 64 Bit Counter' */
#define VR9_BM_GCTRL_FR_RBC_OFFSET                        0x049
#define VR9_BM_GCTRL_FR_RBC_SHIFT                         11
#define VR9_BM_GCTRL_FR_RBC_SIZE                          1
/* Bit: 'FR_RGC' */
/* Description: 'Freeze RMON RX Good Byte 64 Bit Counter' */
#define VR9_BM_GCTRL_FR_RGC_OFFSET                        0x049
#define VR9_BM_GCTRL_FR_RGC_SHIFT                         10
#define VR9_BM_GCTRL_FR_RGC_SIZE                          1
/* Bit: 'FR_TGC' */
/* Description: 'Freeze RMON TX Good Byte 64 Bit Counter' */
#define VR9_BM_GCTRL_FR_TGC_OFFSET                        0x049
#define VR9_BM_GCTRL_FR_TGC_SHIFT                         9
#define VR9_BM_GCTRL_FR_TGC_SIZE                          1
/* Bit: 'I_FIN' */
/* Description: 'RAM initialization finished' */
#define VR9_BM_GCTRL_I_FIN_OFFSET                         0x049
#define VR9_BM_GCTRL_I_FIN_SHIFT                          8
#define VR9_BM_GCTRL_I_FIN_SIZE                           1
/* Bit: 'CX_INI' */
/* Description: 'PQM Context RAM initialization' */
#define VR9_BM_GCTRL_CX_INI_OFFSET                        0x049
#define VR9_BM_GCTRL_CX_INI_SHIFT                         7
#define VR9_BM_GCTRL_CX_INI_SIZE                          1
/* Bit: 'FP_INI' */
/* Description: 'FPQM RAM initialization' */
#define VR9_BM_GCTRL_FP_INI_OFFSET                        0x049
#define VR9_BM_GCTRL_FP_INI_SHIFT                         6
#define VR9_BM_GCTRL_FP_INI_SIZE                          1
/* Bit: 'FS_INI' */
/* Description: 'FSQM RAM initialization' */
#define VR9_BM_GCTRL_FS_INI_OFFSET                        0x049
#define VR9_BM_GCTRL_FS_INI_SHIFT                         5
#define VR9_BM_GCTRL_FS_INI_SIZE                          1
/* Bit: 'R_SRES' */
/* Description: 'Software Reset for RMON' */
#define VR9_BM_GCTRL_R_SRES_OFFSET                        0x049
#define VR9_BM_GCTRL_R_SRES_SHIFT                         4
#define VR9_BM_GCTRL_R_SRES_SIZE                          1
/* Bit: 'S_SRES' */
/* Description: 'Software Reset for Scheduler' */
#define VR9_BM_GCTRL_S_SRES_OFFSET                        0x049
#define VR9_BM_GCTRL_S_SRES_SHIFT                         3
#define VR9_BM_GCTRL_S_SRES_SIZE                          1
/* Bit: 'A_SRES' */
/* Description: 'Software Reset for AVG' */
#define VR9_BM_GCTRL_A_SRES_OFFSET                        0x049
#define VR9_BM_GCTRL_A_SRES_SHIFT                         2
#define VR9_BM_GCTRL_A_SRES_SIZE                          1
/* Bit: 'P_SRES' */
/* Description: 'Software Reset for PQM' */
#define VR9_BM_GCTRL_P_SRES_OFFSET                        0x049
#define VR9_BM_GCTRL_P_SRES_SHIFT                         1
#define VR9_BM_GCTRL_P_SRES_SIZE                          1
/* Bit: 'F_SRES' */
/* Description: 'Software Reset for FSQM' */
#define VR9_BM_GCTRL_F_SRES_OFFSET                        0x049
#define VR9_BM_GCTRL_F_SRES_SHIFT                         0
#define VR9_BM_GCTRL_F_SRES_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'Queue Manager GlobalControl Register 0' */
/* Bit: 'GL_MOD' */
/* Description: 'WRED Mode Signal' */
#define VR9_BM_QUEUE_GCTRL_GL_MOD_OFFSET                  0x04A
#define VR9_BM_QUEUE_GCTRL_GL_MOD_SHIFT                   10
#define VR9_BM_QUEUE_GCTRL_GL_MOD_SIZE                    1
/* Bit: 'AQUI' */
/* Description: 'Average Queue Update Interval' */
#define VR9_BM_QUEUE_GCTRL_AQUI_OFFSET                    0x04A
#define VR9_BM_QUEUE_GCTRL_AQUI_SHIFT                     7
#define VR9_BM_QUEUE_GCTRL_AQUI_SIZE                      3
/* Bit: 'AQWF' */
/* Description: 'Average Queue Weight Factor' */
#define VR9_BM_QUEUE_GCTRL_AQWF_OFFSET                    0x04A
#define VR9_BM_QUEUE_GCTRL_AQWF_SHIFT                     3
#define VR9_BM_QUEUE_GCTRL_AQWF_SIZE                      4
/* Bit: 'QAVGEN' */
/* Description: 'Queue Average Calculation Enable' */
#define VR9_BM_QUEUE_GCTRL_QAVGEN_OFFSET                  0x04A
#define VR9_BM_QUEUE_GCTRL_QAVGEN_SHIFT                   2
#define VR9_BM_QUEUE_GCTRL_QAVGEN_SIZE                    1
/* Bit: 'DPROB' */
/* Description: 'Drop Probability Profile' */
#define VR9_BM_QUEUE_GCTRL_DPROB_OFFSET                   0x04A
#define VR9_BM_QUEUE_GCTRL_DPROB_SHIFT                    0
#define VR9_BM_QUEUE_GCTRL_DPROB_SIZE                     2
/* -------------------------------------------------------------------------- */
/* Register: 'WRED Red Threshold Register0' */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define VR9_BM_WRED_RTH_0_MINTH_OFFSET                    0x04B
#define VR9_BM_WRED_RTH_0_MINTH_SHIFT                     0
#define VR9_BM_WRED_RTH_0_MINTH_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'WRED Red Threshold Register1' */
/* Bit: 'MAXTH' */
/* Description: 'Maximum Threshold' */
#define VR9_BM_WRED_RTH_1_MAXTH_OFFSET                    0x04C
#define VR9_BM_WRED_RTH_1_MAXTH_SHIFT                     0
#define VR9_BM_WRED_RTH_1_MAXTH_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'WRED Yellow ThresholdRegister 0' */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define VR9_BM_WRED_YTH_0_MINTH_OFFSET                    0x04D
#define VR9_BM_WRED_YTH_0_MINTH_SHIFT                     0
#define VR9_BM_WRED_YTH_0_MINTH_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'WRED Yellow ThresholdRegister 1' */
/* Bit: 'MAXTH' */
/* Description: 'Maximum Threshold' */
#define VR9_BM_WRED_YTH_1_MAXTH_OFFSET                    0x04E
#define VR9_BM_WRED_YTH_1_MAXTH_SHIFT                     0
#define VR9_BM_WRED_YTH_1_MAXTH_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'WRED Green ThresholdRegister 0' */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define VR9_BM_WRED_GTH_0_MINTH_OFFSET                    0x04F
#define VR9_BM_WRED_GTH_0_MINTH_SHIFT                     0
#define VR9_BM_WRED_GTH_0_MINTH_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'WRED Green ThresholdRegister 1' */
/* Bit: 'MAXTH' */
/* Description: 'Maximum Threshold' */
#define VR9_BM_WRED_GTH_1_MAXTH_OFFSET                    0x050
#define VR9_BM_WRED_GTH_1_MAXTH_SHIFT                     0
#define VR9_BM_WRED_GTH_1_MAXTH_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'Drop Threshold ConfigurationRegister 0' */
/* Bit: 'THR_FQ' */
/* Description: 'Threshold for frames marked red' */
#define VR9_BM_DROP_GTH_0_THR_FQ_OFFSET                   0x051
#define VR9_BM_DROP_GTH_0_THR_FQ_SHIFT                    0
#define VR9_BM_DROP_GTH_0_THR_FQ_SIZE                     11
/* -------------------------------------------------------------------------- */
/* Register: 'Drop Threshold ConfigurationRegister 1' */
/* Bit: 'THY_FQ' */
/* Description: 'Threshold for frames marked yellow' */
#define VR9_BM_DROP_GTH_1_THY_FQ_OFFSET                   0x052
#define VR9_BM_DROP_GTH_1_THY_FQ_SHIFT                    0
#define VR9_BM_DROP_GTH_1_THY_FQ_SIZE                     11
/* -------------------------------------------------------------------------- */
/* Register: 'Drop Threshold ConfigurationRegister 2' */
/* Bit: 'THG_FQ' */
/* Description: 'Threshold for frames marked green' */
#define VR9_BM_DROP_GTH_2_THG_FQ_OFFSET                   0x053
#define VR9_BM_DROP_GTH_2_THG_FQ_SHIFT                    0
#define VR9_BM_DROP_GTH_2_THG_FQ_SIZE                     11
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer Manager Global InterruptEnable Register' */
/* Bit: 'CNT4' */
/* Description: 'Counter Group 4 (RMON-CLASSIFICATION) Interrupt
Enable' */
#define VR9_BM_IER_CNT4_OFFSET                            0x054
#define VR9_BM_IER_CNT4_SHIFT                             7
#define VR9_BM_IER_CNT4_SIZE                              1
/* Bit: 'CNT3' */
/* Description: 'Counter Group 3 (RMON-PQM) Interrupt Enable' */
#define VR9_BM_IER_CNT3_OFFSET                            0x054
#define VR9_BM_IER_CNT3_SHIFT                             6
#define VR9_BM_IER_CNT3_SIZE                              1
/* Bit: 'CNT2' */
/* Description: 'Counter Group 2 (RMON-SCHEDULER) Interrupt Enable' */
#define VR9_BM_IER_CNT2_OFFSET                            0x054
#define VR9_BM_IER_CNT2_SHIFT                             5
#define VR9_BM_IER_CNT2_SIZE                              1
/* Bit: 'CNT1' */
/* Description: 'Counter Group 1 (RMON-QFETCH) Interrupt Enable' */
#define VR9_BM_IER_CNT1_OFFSET                            0x054
#define VR9_BM_IER_CNT1_SHIFT                             4
#define VR9_BM_IER_CNT1_SIZE                              1
/* Bit: 'CNT0' */
/* Description: 'Counter Group 0 (RMON-QSTOR) Interrupt Enable' */
#define VR9_BM_IER_CNT0_OFFSET                            0x054
#define VR9_BM_IER_CNT0_SHIFT                             3
#define VR9_BM_IER_CNT0_SIZE                              1
/* Bit: 'DEQ' */
/* Description: 'PQM dequeue Interrupt Enable' */
#define VR9_BM_IER_DEQ_OFFSET                             0x054
#define VR9_BM_IER_DEQ_SHIFT                              2
#define VR9_BM_IER_DEQ_SIZE                               1
/* Bit: 'ENQ' */
/* Description: 'PQM Enqueue Interrupt Enable' */
#define VR9_BM_IER_ENQ_OFFSET                             0x054
#define VR9_BM_IER_ENQ_SHIFT                              1
#define VR9_BM_IER_ENQ_SIZE                               1
/* Bit: 'FSQM' */
/* Description: 'Buffer Empty Interrupt Enable' */
#define VR9_BM_IER_FSQM_OFFSET                            0x054
#define VR9_BM_IER_FSQM_SHIFT                             0
#define VR9_BM_IER_FSQM_SIZE                              1
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer Manager Global InterruptStatus Register' */
/* Bit: 'CNT4' */
/* Description: 'Counter Group 4 Interrupt' */
#define VR9_BM_ISR_CNT4_OFFSET                            0x055
#define VR9_BM_ISR_CNT4_SHIFT                             7
#define VR9_BM_ISR_CNT4_SIZE                              1
/* Bit: 'CNT3' */
/* Description: 'Counter Group 3 Interrupt' */
#define VR9_BM_ISR_CNT3_OFFSET                            0x055
#define VR9_BM_ISR_CNT3_SHIFT                             6
#define VR9_BM_ISR_CNT3_SIZE                              1
/* Bit: 'CNT2' */
/* Description: 'Counter Group 2 Interrupt' */
#define VR9_BM_ISR_CNT2_OFFSET                            0x055
#define VR9_BM_ISR_CNT2_SHIFT                             5
#define VR9_BM_ISR_CNT2_SIZE                              1
/* Bit: 'CNT1' */
/* Description: 'Counter Group 1 Interrupt' */
#define VR9_BM_ISR_CNT1_OFFSET                            0x055
#define VR9_BM_ISR_CNT1_SHIFT                             4
#define VR9_BM_ISR_CNT1_SIZE                              1
/* Bit: 'CNT0' */
/* Description: 'Counter Group 0 Interrupt' */
#define VR9_BM_ISR_CNT0_OFFSET                            0x055
#define VR9_BM_ISR_CNT0_SHIFT                             3
#define VR9_BM_ISR_CNT0_SIZE                              1
/* Bit: 'DEQ' */
/* Description: 'PQM dequeue Interrupt Enable' */
#define VR9_BM_ISR_DEQ_OFFSET                             0x055
#define VR9_BM_ISR_DEQ_SHIFT                              2
#define VR9_BM_ISR_DEQ_SIZE                               1
/* Bit: 'ENQ' */
/* Description: 'PQM Enqueue Interrupt' */
#define VR9_BM_ISR_ENQ_OFFSET                             0x055
#define VR9_BM_ISR_ENQ_SHIFT                              1
#define VR9_BM_ISR_ENQ_SIZE                               1
/* Bit: 'FSQM' */
/* Description: 'Buffer Empty Interrupt' */
#define VR9_BM_ISR_FSQM_OFFSET                            0x055
#define VR9_BM_ISR_FSQM_SHIFT                             0
#define VR9_BM_ISR_FSQM_SIZE                              1
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer Manager RMON CounterInterrupt Select Register' */
/* Bit: 'PORT' */
/* Description: 'Port Number' */
#define VR9_BM_CISEL_PORT_OFFSET                          0x056
#define VR9_BM_CISEL_PORT_SHIFT                           0
#define VR9_BM_CISEL_PORT_SIZE                            4
/* -------------------------------------------------------------------------- */
/* Register: 'Debug Control Register' */
/* Bit: 'DBG_SEL' */
/* Description: 'Select Signal for Debug Multiplexer' */
#define VR9_BM_DEBUG_CTRL_DBG_SEL_OFFSET                  0x057
#define VR9_BM_DEBUG_CTRL_DBG_SEL_SHIFT                   0
#define VR9_BM_DEBUG_CTRL_DBG_SEL_SIZE                    8
/* -------------------------------------------------------------------------- */
/* Register: 'Debug Value Register' */
/* Bit: 'DBG_DAT' */
/* Description: 'Debug Data Value' */
#define VR9_BM_DEBUG_VAL_DBG_DAT_OFFSET                   0x058
#define VR9_BM_DEBUG_VAL_DBG_DAT_SHIFT                    0
#define VR9_BM_DEBUG_VAL_DBG_DAT_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer Manager PortConfiguration Register' */
/* Bit: 'IGCNT' */
/* Description: 'Ingress Special Tag RMON count' */
#define VR9_BM_PCFG_IGCNT_OFFSET                          0x080
#define VR9_BM_PCFG_IGCNT_SHIFT                           1
#define VR9_BM_PCFG_IGCNT_SIZE                            1
/* Bit: 'CNTEN' */
/* Description: 'RMON Counter Enable' */
#define VR9_BM_PCFG_CNTEN_OFFSET                          0x080
#define VR9_BM_PCFG_CNTEN_SHIFT                           0
#define VR9_BM_PCFG_CNTEN_SIZE                            1
/* -------------------------------------------------------------------------- */
/* Register: 'Buffer ManagerRMON Control Register' */
/* Bit: 'RAM2_RES' */
/* Description: 'Software Reset for RMON RAM2' */
#define VR9_BM_RMON_CTRL_RAM2_RES_OFFSET                  0x081
#define VR9_BM_RMON_CTRL_RAM2_RES_SHIFT                   1
#define VR9_BM_RMON_CTRL_RAM2_RES_SIZE                    1
/* Bit: 'RAM1_RES' */
/* Description: 'Software Reset for RMON RAM1' */
#define VR9_BM_RMON_CTRL_RAM1_RES_OFFSET                  0x081
#define VR9_BM_RMON_CTRL_RAM1_RES_SHIFT                   0
#define VR9_BM_RMON_CTRL_RAM1_RES_SIZE                    1
/* -------------------------------------------------------------------------- */
/* Register: 'Packet Queue ManagerRate Shaper Assignment Register' */
/* Bit: 'EN2' */
/* Description: 'Rate Shaper 2 Enable' */
#define VR9_PQM_RS_EN2_OFFSET                             0x101
#define VR9_PQM_RS_EN2_SHIFT                              15
#define VR9_PQM_RS_EN2_SIZE                               1
/* Bit: 'RS2' */
/* Description: 'Rate Shaper 2' */
#define VR9_PQM_RS_RS2_OFFSET                             0x101
#define VR9_PQM_RS_RS2_SHIFT                              8
#define VR9_PQM_RS_RS2_SIZE                               4
/* Bit: 'EN1' */
/* Description: 'Rate Shaper 1 Enable' */
#define VR9_PQM_RS_EN1_OFFSET                             0x101
#define VR9_PQM_RS_EN1_SHIFT                              7
#define VR9_PQM_RS_EN1_SIZE                               1
/* Bit: 'RS1' */
/* Description: 'Rate Shaper 1' */
#define VR9_PQM_RS_RS1_OFFSET                             0x101
#define VR9_PQM_RS_RS1_SHIFT                              0
#define VR9_PQM_RS_RS1_SIZE                               4
/* -------------------------------------------------------------------------- */
/* Register: 'Rate Shaper ControlRegister' */
/* Bit: 'RSEN' */
/* Description: 'Rate Shaper Enable' */
#define VR9_RS_CTRL_RSEN_OFFSET                           0x140
#define VR9_RS_CTRL_RSEN_SHIFT                            0
#define VR9_RS_CTRL_RSEN_SIZE                             1
/* -------------------------------------------------------------------------- */
/* Register: 'Rate Shaper CommittedBurst Size Register' */
/* Bit: 'CBS' */
/* Description: 'Committed Burst Size' */
#define VR9_RS_CBS_CBS_OFFSET                             0x141
#define VR9_RS_CBS_CBS_SHIFT                              0
#define VR9_RS_CBS_CBS_SIZE                               10
/* -------------------------------------------------------------------------- */
/* Register: 'Rate Shaper InstantaneousBurst Size Register' */
/* Bit: 'IBS' */
/* Description: 'Instantaneous Burst Size' */
#define VR9_RS_IBS_IBS_OFFSET                             0x142
#define VR9_RS_IBS_IBS_SHIFT                              0
#define VR9_RS_IBS_IBS_SIZE                               2
/* -------------------------------------------------------------------------- */
/* Register: 'Rate Shaper RateExponent Register' */
/* Bit: 'EXP' */
/* Description: 'Exponent' */
#define VR9_RS_CIR_EXP_EXP_OFFSET                         0x143
#define VR9_RS_CIR_EXP_EXP_SHIFT                          0
#define VR9_RS_CIR_EXP_EXP_SIZE                           4
/* -------------------------------------------------------------------------- */
/* Register: 'Rate Shaper RateMantissa Register' */
/* Bit: 'MANT' */
/* Description: 'Mantissa' */
#define VR9_RS_CIR_MANT_MANT_OFFSET                       0x144
#define VR9_RS_CIR_MANT_MANT_SHIFT                        0
#define VR9_RS_CIR_MANT_MANT_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 15' */
/* Bit: 'KEY15' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_15_KEY15_OFFSET                   0x438
#define VR9_PCE_TBL_KEY_15_KEY15_SHIFT                    0
#define VR9_PCE_TBL_KEY_15_KEY15_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 14' */
/* Bit: 'KEY14' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_14_KEY14_OFFSET                   0x439
#define VR9_PCE_TBL_KEY_14_KEY14_SHIFT                    0
#define VR9_PCE_TBL_KEY_14_KEY14_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 13' */
/* Bit: 'KEY13' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_13_KEY13_OFFSET                   0x43A
#define VR9_PCE_TBL_KEY_13_KEY13_SHIFT                    0
#define VR9_PCE_TBL_KEY_13_KEY13_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 12' */
/* Bit: 'KEY12' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_12_KEY12_OFFSET                   0x43B
#define VR9_PCE_TBL_KEY_12_KEY12_SHIFT                    0
#define VR9_PCE_TBL_KEY_12_KEY12_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 11' */
/* Bit: 'KEY11' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_11_KEY11_OFFSET                   0x43C
#define VR9_PCE_TBL_KEY_11_KEY11_SHIFT                    0
#define VR9_PCE_TBL_KEY_11_KEY11_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 10' */
/* Bit: 'KEY10' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_10_KEY10_OFFSET                   0x43D
#define VR9_PCE_TBL_KEY_10_KEY10_SHIFT                    0
#define VR9_PCE_TBL_KEY_10_KEY10_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 9' */
/* Bit: 'KEY9' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_9_KEY9_OFFSET                     0x43E
#define VR9_PCE_TBL_KEY_9_KEY9_SHIFT                      0
#define VR9_PCE_TBL_KEY_9_KEY9_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 8' */
/* Bit: 'KEY8' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_8_KEY8_OFFSET                     0x43F
#define VR9_PCE_TBL_KEY_8_KEY8_SHIFT                      0
#define VR9_PCE_TBL_KEY_8_KEY8_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 7' */
/* Bit: 'KEY7' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_7_KEY7_OFFSET                     0x440
#define VR9_PCE_TBL_KEY_7_KEY7_SHIFT                      0
#define VR9_PCE_TBL_KEY_7_KEY7_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 6' */
/* Bit: 'KEY6' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_6_KEY6_OFFSET                     0x441
#define VR9_PCE_TBL_KEY_6_KEY6_SHIFT                      0
#define VR9_PCE_TBL_KEY_6_KEY6_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 5' */
/* Bit: 'KEY5' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_5_KEY5_OFFSET                     0x442
#define VR9_PCE_TBL_KEY_5_KEY5_SHIFT                      0
#define VR9_PCE_TBL_KEY_5_KEY5_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 4' */
/* Bit: 'KEY4' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_4_KEY4_OFFSET                     0x443
#define VR9_PCE_TBL_KEY_4_KEY4_SHIFT                      0
#define VR9_PCE_TBL_KEY_4_KEY4_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 3' */
/* Bit: 'KEY3' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_3_KEY3_OFFSET                     0x444
#define VR9_PCE_TBL_KEY_3_KEY3_SHIFT                      0
#define VR9_PCE_TBL_KEY_3_KEY3_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 2' */
/* Bit: 'KEY2' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_2_KEY2_OFFSET                     0x445
#define VR9_PCE_TBL_KEY_2_KEY2_SHIFT                      0
#define VR9_PCE_TBL_KEY_2_KEY2_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 1' */
/* Bit: 'KEY1' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_1_KEY1_OFFSET                     0x446
#define VR9_PCE_TBL_KEY_1_KEY1_SHIFT                      0
#define VR9_PCE_TBL_KEY_1_KEY1_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Key Data 0' */
/* Bit: 'KEY0' */
/* Description: 'Key Value[15:0]' */
#define VR9_PCE_TBL_KEY_0_KEY0_OFFSET                     0x447
#define VR9_PCE_TBL_KEY_0_KEY0_SHIFT                      0
#define VR9_PCE_TBL_KEY_0_KEY0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Mask Write Register0' */
/* Bit: 'MASK0' */
/* Description: 'Mask Pattern [15:0]' */
#define VR9_PCE_TBL_MASK_0_MASK0_OFFSET                   0x448
#define VR9_PCE_TBL_MASK_0_MASK0_SHIFT                    0
#define VR9_PCE_TBL_MASK_0_MASK0_SIZE                     16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Value Register4' */
/* Bit: 'VAL4' */
/* Description: 'Data value [15:0]' */
#define VR9_PCE_TBL_VAL_4_VAL4_OFFSET                     0x449
#define VR9_PCE_TBL_VAL_4_VAL4_SHIFT                      0
#define VR9_PCE_TBL_VAL_4_VAL4_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Value Register3' */
/* Bit: 'VAL3' */
/* Description: 'Data value [15:0]' */
#define VR9_PCE_TBL_VAL_3_VAL3_OFFSET                     0x44A
#define VR9_PCE_TBL_VAL_3_VAL3_SHIFT                      0
#define VR9_PCE_TBL_VAL_3_VAL3_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Value Register2' */
/* Bit: 'VAL2' */
/* Description: 'Data value [15:0]' */
#define VR9_PCE_TBL_VAL_2_VAL2_OFFSET                     0x44B
#define VR9_PCE_TBL_VAL_2_VAL2_SHIFT                      0
#define VR9_PCE_TBL_VAL_2_VAL2_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Value Register1' */
/* Bit: 'VAL1' */
/* Description: 'Data value [15:0]' */
#define VR9_PCE_TBL_VAL_1_VAL1_OFFSET                     0x44C
#define VR9_PCE_TBL_VAL_1_VAL1_SHIFT                      0
#define VR9_PCE_TBL_VAL_1_VAL1_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Value Register0' */
/* Bit: 'VAL0' */
/* Description: 'Data value [15:0]' */
#define VR9_PCE_TBL_VAL_0_VAL0_OFFSET                     0x44D
#define VR9_PCE_TBL_VAL_0_VAL0_SHIFT                      0
#define VR9_PCE_TBL_VAL_0_VAL0_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Table Entry AddressRegister' */
/* Bit: 'ADDR' */
/* Description: 'Table Address' */
#define VR9_PCE_TBL_ADDR_ADDR_OFFSET                      0x44E
#define VR9_PCE_TBL_ADDR_ADDR_SHIFT                       0
#define VR9_PCE_TBL_ADDR_ADDR_SIZE                        11
/* -------------------------------------------------------------------------- */
/* Register: 'Table Access ControlRegister' */
/* Bit: 'BAS' */
/* Description: 'Access Busy/Access Start' */
#define VR9_PCE_TBL_CTRL_BAS_OFFSET                       0x44F
#define VR9_PCE_TBL_CTRL_BAS_SHIFT                        15
#define VR9_PCE_TBL_CTRL_BAS_SIZE                         1
/* Bit: 'TYPE' */
/* Description: 'Lookup Entry Type' */
#define VR9_PCE_TBL_CTRL_TYPE_OFFSET                      0x44F
#define VR9_PCE_TBL_CTRL_TYPE_SHIFT                       13
#define VR9_PCE_TBL_CTRL_TYPE_SIZE                        1
/* Bit: 'VLD' */
/* Description: 'Lookup Entry Valid' */
#define VR9_PCE_TBL_CTRL_VLD_OFFSET                       0x44F
#define VR9_PCE_TBL_CTRL_VLD_SHIFT                        12
#define VR9_PCE_TBL_CTRL_VLD_SIZE                         1
/* Bit: 'KEYFORM' */
/* Description: 'Key Format' */
#define VR9_PCE_TBL_CTRL_KEYFORM_OFFSET                   0x44F
#define VR9_PCE_TBL_CTRL_KEYFORM_SHIFT                    11
#define VR9_PCE_TBL_CTRL_KEYFORM_SIZE                     1
/* Bit: 'GMAP' */
/* Description: 'Group Map' */
#define VR9_PCE_TBL_CTRL_GMAP_OFFSET                      0x44F
#define VR9_PCE_TBL_CTRL_GMAP_SHIFT                       7
#define VR9_PCE_TBL_CTRL_GMAP_SIZE                        4
/* Bit: 'OPMOD' */
/* Description: 'Lookup Table Access Operation Mode' */
#define VR9_PCE_TBL_CTRL_OPMOD_OFFSET                     0x44F
#define VR9_PCE_TBL_CTRL_OPMOD_SHIFT                      5
#define VR9_PCE_TBL_CTRL_OPMOD_SIZE                       2
/* Bit: 'ADDR' */
/* Description: 'Lookup Table Address' */
#define VR9_PCE_TBL_CTRL_ADDR_OFFSET                      0x44F
#define VR9_PCE_TBL_CTRL_ADDR_SHIFT                       0
#define VR9_PCE_TBL_CTRL_ADDR_SIZE                        5
/* -------------------------------------------------------------------------- */
/* Register: 'Table General StatusRegister' */
/* Bit: 'TBUSY' */
/* Description: 'Table Access Busy' */
#define VR9_PCE_TBL_STAT_TBUSY_OFFSET                     0x450
#define VR9_PCE_TBL_STAT_TBUSY_SHIFT                      2
#define VR9_PCE_TBL_STAT_TBUSY_SIZE                       1
/* Bit: 'TEMPT' */
/* Description: 'Table Empty' */
#define VR9_PCE_TBL_STAT_TEMPT_OFFSET                     0x450
#define VR9_PCE_TBL_STAT_TEMPT_SHIFT                      1
#define VR9_PCE_TBL_STAT_TEMPT_SIZE                       1
/* Bit: 'TFUL' */
/* Description: 'Table Full' */
#define VR9_PCE_TBL_STAT_TFUL_OFFSET                      0x450
#define VR9_PCE_TBL_STAT_TFUL_SHIFT                       0
#define VR9_PCE_TBL_STAT_TFUL_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'Aging Counter ConfigurationRegister 0' */
/* Bit: 'EXP' */
/* Description: 'Aging Counter Exponent Value ' */
#define VR9_PCE_AGE_0_EXP_OFFSET                          0x451
#define VR9_PCE_AGE_0_EXP_SHIFT                           0
#define VR9_PCE_AGE_0_EXP_SIZE                            4
/* -------------------------------------------------------------------------- */
/* Register: 'Aging Counter ConfigurationRegister 1' */
/* Bit: 'MANT' */
/* Description: 'Aging Counter Mantissa Value ' */
#define VR9_PCE_AGE_1_MANT_OFFSET                         0x452
#define VR9_PCE_AGE_1_MANT_SHIFT                          0
#define VR9_PCE_AGE_1_MANT_SIZE                           16
/* -------------------------------------------------------------------------- */
/* Register: 'Port Map Register 1' */
/* Bit: 'MPMAP' */
/* Description: 'Monitoring Port Map' */
#define VR9_PCE_PMAP_1_MPMAP_OFFSET                       0x453
#define VR9_PCE_PMAP_1_MPMAP_SHIFT                        0
#define VR9_PCE_PMAP_1_MPMAP_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'Port Map Register 2' */
/* Bit: 'DMCPMAP' */
/* Description: 'Default Multicast Port Map' */
#define VR9_PCE_PMAP_2_DMCPMAP_OFFSET                     0x454
#define VR9_PCE_PMAP_2_DMCPMAP_SHIFT                      0
#define VR9_PCE_PMAP_2_DMCPMAP_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Port Map Register 3' */
/* Bit: 'UUCMAP' */
/* Description: 'Default Unknown Unicast Port Map' */
#define VR9_PCE_PMAP_3_UUCMAP_OFFSET                      0x455
#define VR9_PCE_PMAP_3_UUCMAP_SHIFT                       0
#define VR9_PCE_PMAP_3_UUCMAP_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Global Control Register0' */
/* Bit: 'IGMP' */
/* Description: 'IGMP Mode Selection' */
#define VR9_PCE_GCTRL_0_IGMP_OFFSET                       0x456
#define VR9_PCE_GCTRL_0_IGMP_SHIFT                        15
#define VR9_PCE_GCTRL_0_IGMP_SIZE                         1
/* Bit: 'VLAN' */
/* Description: 'VLAN-aware Switching' */
#define VR9_PCE_GCTRL_0_VLAN_OFFSET                       0x456
#define VR9_PCE_GCTRL_0_VLAN_SHIFT                        14
#define VR9_PCE_GCTRL_0_VLAN_SIZE                         1
/* Bit: 'NOPM' */
/* Description: 'No Port Map Forwarding' */
#define VR9_PCE_GCTRL_0_NOPM_OFFSET                       0x456
#define VR9_PCE_GCTRL_0_NOPM_SHIFT                        13
#define VR9_PCE_GCTRL_0_NOPM_SIZE                         1
/* Bit: 'SCONUC' */
/* Description: 'Unknown Unicast Storm Control' */
#define VR9_PCE_GCTRL_0_SCONUC_OFFSET                     0x456
#define VR9_PCE_GCTRL_0_SCONUC_SHIFT                      12
#define VR9_PCE_GCTRL_0_SCONUC_SIZE                       1
/* Bit: 'SCONMC' */
/* Description: 'Multicast Storm Control' */
#define VR9_PCE_GCTRL_0_SCONMC_OFFSET                     0x456
#define VR9_PCE_GCTRL_0_SCONMC_SHIFT                      11
#define VR9_PCE_GCTRL_0_SCONMC_SIZE                       1
/* Bit: 'SCONBC' */
/* Description: 'Broadcast Storm Control' */
#define VR9_PCE_GCTRL_0_SCONBC_OFFSET                     0x456
#define VR9_PCE_GCTRL_0_SCONBC_SHIFT                      10
#define VR9_PCE_GCTRL_0_SCONBC_SIZE                       1
/* Bit: 'SCONMOD' */
/* Description: 'Storm Control Mode' */
#define VR9_PCE_GCTRL_0_SCONMOD_OFFSET                    0x456
#define VR9_PCE_GCTRL_0_SCONMOD_SHIFT                     8
#define VR9_PCE_GCTRL_0_SCONMOD_SIZE                      2
/* Bit: 'SCONMET' */
/* Description: 'Storm Control Metering Instance' */
#define VR9_PCE_GCTRL_0_SCONMET_OFFSET                    0x456
#define VR9_PCE_GCTRL_0_SCONMET_SHIFT                     4
#define VR9_PCE_GCTRL_0_SCONMET_SIZE                      4
/* Bit: 'MC_VALID' */
/* Description: 'Access Request' */
#define VR9_PCE_GCTRL_0_MC_VALID_OFFSET                   0x456
#define VR9_PCE_GCTRL_0_MC_VALID_SHIFT                    3
#define VR9_PCE_GCTRL_0_MC_VALID_SIZE                     1
/* Bit: 'PLCKMOD' */
/* Description: 'Port Lock Mode' */
#define VR9_PCE_GCTRL_0_PLCKMOD_OFFSET                    0x456
#define VR9_PCE_GCTRL_0_PLCKMOD_SHIFT                     2
#define VR9_PCE_GCTRL_0_PLCKMOD_SIZE                      1
/* Bit: 'PLIMMOD' */
/* Description: 'MAC Address Learning Limitation Mode' */
#define VR9_PCE_GCTRL_0_PLIMMOD_OFFSET                    0x456
#define VR9_PCE_GCTRL_0_PLIMMOD_SHIFT                     1
#define VR9_PCE_GCTRL_0_PLIMMOD_SIZE                      1
/* Bit: 'MTFL' */
/* Description: 'MAC Table Flushing' */
#define VR9_PCE_GCTRL_0_MTFL_OFFSET                       0x456
#define VR9_PCE_GCTRL_0_MTFL_SHIFT                        0
#define VR9_PCE_GCTRL_0_MTFL_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Global Control Register1' */
/* Bit: 'PARSER_DBG' */
/* Description: 'Parser Debug Selection' */
#define VR9_PCE_GCTRL_1_PARSER_DBG_OFFSET                 0x457
#define VR9_PCE_GCTRL_1_PARSER_DBG_SHIFT                  14
#define VR9_PCE_GCTRL_1_PARSER_DBG_SIZE                   2
/* Bit: 'FLOWPTR_LIST_EN' */
/* Description: 'Flow Pointer List enable' */
#define VR9_PCE_GCTRL_1_FLOWPTR_LIST_EN_OFFSET            0x457
#define VR9_PCE_GCTRL_1_FLOWPTR_LIST_EN_SHIFT             4
#define VR9_PCE_GCTRL_1_FLOWPTR_LIST_EN_SIZE              1
/* Bit: 'MAC_GLOCKMOD' */
/* Description: 'MAC Address Table Lock forwarding mode' */
#define VR9_PCE_GCTRL_1_MAC_GLOCKMOD_OFFSET               0x457
#define VR9_PCE_GCTRL_1_MAC_GLOCKMOD_SHIFT                3
#define VR9_PCE_GCTRL_1_MAC_GLOCKMOD_SIZE                 1
/* Bit: 'MAC_GLOCK' */
/* Description: 'MAC Address Table Lock' */
#define VR9_PCE_GCTRL_1_MAC_GLOCK_OFFSET                  0x457
#define VR9_PCE_GCTRL_1_MAC_GLOCK_SHIFT                   2
#define VR9_PCE_GCTRL_1_MAC_GLOCK_SIZE                    1
/* Bit: 'PCE_DIS' */
/* Description: 'PCE Disable after currently processed packet' */
#define VR9_PCE_GCTRL_1_PCE_DIS_OFFSET                    0x457
#define VR9_PCE_GCTRL_1_PCE_DIS_SHIFT                     1
#define VR9_PCE_GCTRL_1_PCE_DIS_SIZE                      1
/* Bit: 'LRNMOD' */
/* Description: 'MAC Address Learning Mode' */
#define VR9_PCE_GCTRL_1_LRNMOD_OFFSET                     0x457
#define VR9_PCE_GCTRL_1_LRNMOD_SHIFT                      0
#define VR9_PCE_GCTRL_1_LRNMOD_SIZE                       1
/* -------------------------------------------------------------------------- */
/* Register: 'Three-color MarkerGlobal Control Register' */
/* Bit: 'DPRED' */
/* Description: 'Re-marking Drop Precedence Red Encoding' */
#define VR9_PCE_TCM_GLOB_CTRL_DPRED_OFFSET                0x458
#define VR9_PCE_TCM_GLOB_CTRL_DPRED_SHIFT                 6
#define VR9_PCE_TCM_GLOB_CTRL_DPRED_SIZE                  3
/* Bit: 'DPYEL' */
/* Description: 'Re-marking Drop Precedence Yellow Encoding' */
#define VR9_PCE_TCM_GLOB_CTRL_DPYEL_OFFSET                0x458
#define VR9_PCE_TCM_GLOB_CTRL_DPYEL_SHIFT                 3
#define VR9_PCE_TCM_GLOB_CTRL_DPYEL_SIZE                  3
/* Bit: 'DPGRN' */
/* Description: 'Re-marking Drop Precedence Green Encoding' */
#define VR9_PCE_TCM_GLOB_CTRL_DPGRN_OFFSET                0x458
#define VR9_PCE_TCM_GLOB_CTRL_DPGRN_SHIFT                 0
#define VR9_PCE_TCM_GLOB_CTRL_DPGRN_SIZE                  3
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Control Register' */
/* Bit: 'FAGEEN' */
/* Description: 'Force Aging of Table Entries Enable' */
#define VR9_PCE_IGMP_CTRL_FAGEEN_OFFSET                   0x459
#define VR9_PCE_IGMP_CTRL_FAGEEN_SHIFT                    15
#define VR9_PCE_IGMP_CTRL_FAGEEN_SIZE                     1
/* Bit: 'FLEAVE' */
/* Description: 'Fast Leave Enable' */
#define VR9_PCE_IGMP_CTRL_FLEAVE_OFFSET                   0x459
#define VR9_PCE_IGMP_CTRL_FLEAVE_SHIFT                    14
#define VR9_PCE_IGMP_CTRL_FLEAVE_SIZE                     1
/* Bit: 'DMRTEN' */
/* Description: 'Default Maximum Response Time Enable' */
#define VR9_PCE_IGMP_CTRL_DMRTEN_OFFSET                   0x459
#define VR9_PCE_IGMP_CTRL_DMRTEN_SHIFT                    13
#define VR9_PCE_IGMP_CTRL_DMRTEN_SIZE                     1
/* Bit: 'JASUP' */
/* Description: 'Join Aggregation Suppression Enable' */
#define VR9_PCE_IGMP_CTRL_JASUP_OFFSET                    0x459
#define VR9_PCE_IGMP_CTRL_JASUP_SHIFT                     12
#define VR9_PCE_IGMP_CTRL_JASUP_SIZE                      1
/* Bit: 'REPSUP' */
/* Description: 'Report Suppression Enable' */
#define VR9_PCE_IGMP_CTRL_REPSUP_OFFSET                   0x459
#define VR9_PCE_IGMP_CTRL_REPSUP_SHIFT                    11
#define VR9_PCE_IGMP_CTRL_REPSUP_SIZE                     1
/* Bit: 'SRPEN' */
/* Description: 'Snooping of Router Port Enable' */
#define VR9_PCE_IGMP_CTRL_SRPEN_OFFSET                    0x459
#define VR9_PCE_IGMP_CTRL_SRPEN_SHIFT                     10
#define VR9_PCE_IGMP_CTRL_SRPEN_SIZE                      1
/* Bit: 'ROB' */
/* Description: 'Robustness Variable' */
#define VR9_PCE_IGMP_CTRL_ROB_OFFSET                      0x459
#define VR9_PCE_IGMP_CTRL_ROB_SHIFT                       8
#define VR9_PCE_IGMP_CTRL_ROB_SIZE                        2
/* Bit: 'DMRT' */
/* Description: 'IGMP Default Maximum Response Time' */
#define VR9_PCE_IGMP_CTRL_DMRT_OFFSET                     0x459
#define VR9_PCE_IGMP_CTRL_DMRT_SHIFT                      0
#define VR9_PCE_IGMP_CTRL_DMRT_SIZE                       8
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Default RouterPort Map Register' */
/* Bit: 'DRPM' */
/* Description: 'IGMP Default Router Port Map' */
#define VR9_PCE_IGMP_DRPM_DRPM_OFFSET                     0x45A
#define VR9_PCE_IGMP_DRPM_DRPM_SHIFT                      0
#define VR9_PCE_IGMP_DRPM_DRPM_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Aging Register0' */
/* Bit: 'MANT' */
/* Description: 'IGMP Group Aging Time Mantissa' */
#define VR9_PCE_IGMP_AGE_0_MANT_OFFSET                    0x45B
#define VR9_PCE_IGMP_AGE_0_MANT_SHIFT                     3
#define VR9_PCE_IGMP_AGE_0_MANT_SIZE                      8
/* Bit: 'EXP' */
/* Description: 'IGMP Group Aging Time Exponent' */
#define VR9_PCE_IGMP_AGE_0_EXP_OFFSET                     0x45B
#define VR9_PCE_IGMP_AGE_0_EXP_SHIFT                      0
#define VR9_PCE_IGMP_AGE_0_EXP_SIZE                       3
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Aging Register1' */
/* Bit: 'MANT' */
/* Description: 'IGMP Router Port Aging Time Mantissa' */
#define VR9_PCE_IGMP_AGE_1_MANT_OFFSET                    0x45C
#define VR9_PCE_IGMP_AGE_1_MANT_SHIFT                     0
#define VR9_PCE_IGMP_AGE_1_MANT_SIZE                      12
/* -------------------------------------------------------------------------- */
/* Register: 'IGMP Status Register' */
/* Bit: 'IGPM' */
/* Description: 'IGMP Port Map' */
#define VR9_PCE_IGMP_STAT_IGPM_OFFSET                     0x45D
#define VR9_PCE_IGMP_STAT_IGPM_SHIFT                      0
#define VR9_PCE_IGMP_STAT_IGPM_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN ControlRegister' */
/* Bit: 'PASSEN' */
/* Description: 'WoL Password Enable' */
#define VR9_WOL_GLB_CTRL_PASSEN_OFFSET                    0x45E
#define VR9_WOL_GLB_CTRL_PASSEN_SHIFT                     0
#define VR9_WOL_GLB_CTRL_PASSEN_SIZE                      1
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN DestinationAddress Register 0' */
/* Bit: 'DA0' */
/* Description: 'WoL Destination Address [15:0]' */
#define VR9_WOL_DA_0_DA0_OFFSET                           0x45F
#define VR9_WOL_DA_0_DA0_SHIFT                            0
#define VR9_WOL_DA_0_DA0_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN DestinationAddress Register 1' */
/* Bit: 'DA1' */
/* Description: 'WoL Destination Address [31:16]' */
#define VR9_WOL_DA_1_DA1_OFFSET                           0x460
#define VR9_WOL_DA_1_DA1_SHIFT                            0
#define VR9_WOL_DA_1_DA1_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN DestinationAddress Register 2' */
/* Bit: 'DA2' */
/* Description: 'WoL Destination Address [47:32]' */
#define VR9_WOL_DA_2_DA2_OFFSET                           0x461
#define VR9_WOL_DA_2_DA2_SHIFT                            0
#define VR9_WOL_DA_2_DA2_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN Password Register0' */
/* Bit: 'PW0' */
/* Description: 'WoL Password [15:0]' */
#define VR9_WOL_PW_0_PW0_OFFSET                           0x462
#define VR9_WOL_PW_0_PW0_SHIFT                            0
#define VR9_WOL_PW_0_PW0_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN Password Register1' */
/* Bit: 'PW1' */
/* Description: 'WoL Password [31:16]' */
#define VR9_WOL_PW_1_PW1_OFFSET                           0x463
#define VR9_WOL_PW_1_PW1_SHIFT                            0
#define VR9_WOL_PW_1_PW1_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN Password Register2' */
/* Bit: 'PW2' */
/* Description: 'WoL Password [47:32]' */
#define VR9_WOL_PW_2_PW2_OFFSET                           0x464
#define VR9_WOL_PW_2_PW2_SHIFT                            0
#define VR9_WOL_PW_2_PW2_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global Interrupt Enable Register 0' */
/* Bit: 'PINT_15' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_15_OFFSET                      0x465
#define VR9_PCE_IER_0_PINT_15_SHIFT                       15
#define VR9_PCE_IER_0_PINT_15_SIZE                        1
/* Bit: 'PINT_14' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_14_OFFSET                      0x465
#define VR9_PCE_IER_0_PINT_14_SHIFT                       14
#define VR9_PCE_IER_0_PINT_14_SIZE                        1
/* Bit: 'PINT_13' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_13_OFFSET                      0x465
#define VR9_PCE_IER_0_PINT_13_SHIFT                       13
#define VR9_PCE_IER_0_PINT_13_SIZE                        1
/* Bit: 'PINT_12' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_12_OFFSET                      0x465
#define VR9_PCE_IER_0_PINT_12_SHIFT                       12
#define VR9_PCE_IER_0_PINT_12_SIZE                        1
/* Bit: 'PINT_11' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_11_OFFSET                      0x465
#define VR9_PCE_IER_0_PINT_11_SHIFT                       11
#define VR9_PCE_IER_0_PINT_11_SIZE                        1
/* Bit: 'PINT_10' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_10_OFFSET                      0x465
#define VR9_PCE_IER_0_PINT_10_SHIFT                       10
#define VR9_PCE_IER_0_PINT_10_SIZE                        1
/* Bit: 'PINT_9' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_9_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_9_SHIFT                        9
#define VR9_PCE_IER_0_PINT_9_SIZE                         1
/* Bit: 'PINT_8' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_8_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_8_SHIFT                        8
#define VR9_PCE_IER_0_PINT_8_SIZE                         1
/* Bit: 'PINT_7' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_7_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_7_SHIFT                        7
#define VR9_PCE_IER_0_PINT_7_SIZE                         1
/* Bit: 'PINT_6' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_6_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_6_SHIFT                        6
#define VR9_PCE_IER_0_PINT_6_SIZE                         1
/* Bit: 'PINT_5' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_5_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_5_SHIFT                        5
#define VR9_PCE_IER_0_PINT_5_SIZE                         1
/* Bit: 'PINT_4' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_4_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_4_SHIFT                        4
#define VR9_PCE_IER_0_PINT_4_SIZE                         1
/* Bit: 'PINT_3' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_3_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_3_SHIFT                        3
#define VR9_PCE_IER_0_PINT_3_SIZE                         1
/* Bit: 'PINT_2' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_2_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_2_SHIFT                        2
#define VR9_PCE_IER_0_PINT_2_SIZE                         1
/* Bit: 'PINT_1' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_1_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_1_SHIFT                        1
#define VR9_PCE_IER_0_PINT_1_SIZE                         1
/* Bit: 'PINT_0' */
/* Description: 'Port Interrupt Enable' */
#define VR9_PCE_IER_0_PINT_0_OFFSET                       0x465
#define VR9_PCE_IER_0_PINT_0_SHIFT                        0
#define VR9_PCE_IER_0_PINT_0_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global Interrupt Enable Register 1' */
/* Bit: 'FLOWINT' */
/* Description: 'Traffic Flow Table Interrupt Rule matched Interrupt
Enable' */
#define VR9_PCE_IER_1_FLOWINT_OFFSET                      0x466
#define VR9_PCE_IER_1_FLOWINT_SHIFT                       6
#define VR9_PCE_IER_1_FLOWINT_SIZE                        1
/* Bit: 'CPH2' */
/* Description: 'Classification Phase 2 Ready Interrupt Enable' */
#define VR9_PCE_IER_1_CPH2_OFFSET                         0x466
#define VR9_PCE_IER_1_CPH2_SHIFT                          5
#define VR9_PCE_IER_1_CPH2_SIZE                           1
/* Bit: 'CPH1' */
/* Description: 'Classification Phase 1 Ready Interrupt Enable' */
#define VR9_PCE_IER_1_CPH1_OFFSET                         0x466
#define VR9_PCE_IER_1_CPH1_SHIFT                          4
#define VR9_PCE_IER_1_CPH1_SIZE                           1
/* Bit: 'CPH0' */
/* Description: 'Classification Phase 0 Ready Interrupt Enable' */
#define VR9_PCE_IER_1_CPH0_OFFSET                         0x466
#define VR9_PCE_IER_1_CPH0_SHIFT                          3
#define VR9_PCE_IER_1_CPH0_SIZE                           1
/* Bit: 'PRDY' */
/* Description: 'Parser Ready Interrupt Enable' */
#define VR9_PCE_IER_1_PRDY_OFFSET                         0x466
#define VR9_PCE_IER_1_PRDY_SHIFT                          2
#define VR9_PCE_IER_1_PRDY_SIZE                           1
/* Bit: 'IGTF' */
/* Description: 'IGMP Table Full Interrupt Enable' */
#define VR9_PCE_IER_1_IGTF_OFFSET                         0x466
#define VR9_PCE_IER_1_IGTF_SHIFT                          1
#define VR9_PCE_IER_1_IGTF_SIZE                           1
/* Bit: 'MTF' */
/* Description: 'MAC Table Full Interrupt Enable' */
#define VR9_PCE_IER_1_MTF_OFFSET                          0x466
#define VR9_PCE_IER_1_MTF_SHIFT                           0
#define VR9_PCE_IER_1_MTF_SIZE                            1
/* -------------------------------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global Interrupt Status Register 0' */
/* Bit: 'PINT_15' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_15_OFFSET                      0x467
#define VR9_PCE_ISR_0_PINT_15_SHIFT                       15
#define VR9_PCE_ISR_0_PINT_15_SIZE                        1
/* Bit: 'PINT_14' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_14_OFFSET                      0x467
#define VR9_PCE_ISR_0_PINT_14_SHIFT                       14
#define VR9_PCE_ISR_0_PINT_14_SIZE                        1
/* Bit: 'PINT_13' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_13_OFFSET                      0x467
#define VR9_PCE_ISR_0_PINT_13_SHIFT                       13
#define VR9_PCE_ISR_0_PINT_13_SIZE                        1
/* Bit: 'PINT_12' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_12_OFFSET                      0x467
#define VR9_PCE_ISR_0_PINT_12_SHIFT                       12
#define VR9_PCE_ISR_0_PINT_12_SIZE                        1
/* Bit: 'PINT_11' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_11_OFFSET                      0x467
#define VR9_PCE_ISR_0_PINT_11_SHIFT                       11
#define VR9_PCE_ISR_0_PINT_11_SIZE                        1
/* Bit: 'PINT_10' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_10_OFFSET                      0x467
#define VR9_PCE_ISR_0_PINT_10_SHIFT                       10
#define VR9_PCE_ISR_0_PINT_10_SIZE                        1
/* Bit: 'PINT_9' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_9_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_9_SHIFT                        9
#define VR9_PCE_ISR_0_PINT_9_SIZE                         1
/* Bit: 'PINT_8' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_8_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_8_SHIFT                        8
#define VR9_PCE_ISR_0_PINT_8_SIZE                         1
/* Bit: 'PINT_7' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_7_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_7_SHIFT                        7
#define VR9_PCE_ISR_0_PINT_7_SIZE                         1
/* Bit: 'PINT_6' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_6_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_6_SHIFT                        6
#define VR9_PCE_ISR_0_PINT_6_SIZE                         1
/* Bit: 'PINT_5' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_5_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_5_SHIFT                        5
#define VR9_PCE_ISR_0_PINT_5_SIZE                         1
/* Bit: 'PINT_4' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_4_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_4_SHIFT                        4
#define VR9_PCE_ISR_0_PINT_4_SIZE                         1
/* Bit: 'PINT_3' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_3_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_3_SHIFT                        3
#define VR9_PCE_ISR_0_PINT_3_SIZE                         1
/* Bit: 'PINT_2' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_2_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_2_SHIFT                        2
#define VR9_PCE_ISR_0_PINT_2_SIZE                         1
/* Bit: 'PINT_1' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_1_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_1_SHIFT                        1
#define VR9_PCE_ISR_0_PINT_1_SIZE                         1
/* Bit: 'PINT_0' */
/* Description: 'Port Interrupt' */
#define VR9_PCE_ISR_0_PINT_0_OFFSET                       0x467
#define VR9_PCE_ISR_0_PINT_0_SHIFT                        0
#define VR9_PCE_ISR_0_PINT_0_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global Interrupt Status Register 1' */
/* Bit: 'FLOWINT' */
/* Description: 'Traffic Flow Table Interrupt Rule matched' */
#define VR9_PCE_ISR_1_FLOWINT_OFFSET                      0x468
#define VR9_PCE_ISR_1_FLOWINT_SHIFT                       6
#define VR9_PCE_ISR_1_FLOWINT_SIZE                        1
/* Bit: 'CPH2' */
/* Description: 'Classification Phase 2 Ready Interrupt' */
#define VR9_PCE_ISR_1_CPH2_OFFSET                         0x468
#define VR9_PCE_ISR_1_CPH2_SHIFT                          5
#define VR9_PCE_ISR_1_CPH2_SIZE                           1
/* Bit: 'CPH1' */
/* Description: 'Classification Phase 1 Ready Interrupt' */
#define VR9_PCE_ISR_1_CPH1_OFFSET                         0x468
#define VR9_PCE_ISR_1_CPH1_SHIFT                          4
#define VR9_PCE_ISR_1_CPH1_SIZE                           1
/* Bit: 'CPH0' */
/* Description: 'Classification Phase 0 Ready Interrupt' */
#define VR9_PCE_ISR_1_CPH0_OFFSET                         0x468
#define VR9_PCE_ISR_1_CPH0_SHIFT                          3
#define VR9_PCE_ISR_1_CPH0_SIZE                           1
/* Bit: 'PRDY' */
/* Description: 'Parser Ready Interrupt' */
#define VR9_PCE_ISR_1_PRDY_OFFSET                         0x468
#define VR9_PCE_ISR_1_PRDY_SHIFT                          2
#define VR9_PCE_ISR_1_PRDY_SIZE                           1
/* Bit: 'IGTF' */
/* Description: 'IGMP Table Full Interrupt' */
#define VR9_PCE_ISR_1_IGTF_OFFSET                         0x468
#define VR9_PCE_ISR_1_IGTF_SHIFT                          1
#define VR9_PCE_ISR_1_IGTF_SIZE                           1
/* Bit: 'MTF' */
/* Description: 'MAC Table Full Interrupt' */
#define VR9_PCE_ISR_1_MTF_OFFSET                          0x468
#define VR9_PCE_ISR_1_MTF_SHIFT                           0
#define VR9_PCE_ISR_1_MTF_SIZE                            1
/* -------------------------------------------------------------------------- */
/* Register: 'Parser Status Register' */
/* Bit: 'FSM_DAT_CNT' */
/* Description: 'Parser FSM Data Counter' */
#define VR9_PARSER_STAT_FSM_DAT_CNT_OFFSET                0x469
#define VR9_PARSER_STAT_FSM_DAT_CNT_SHIFT                 8
#define VR9_PARSER_STAT_FSM_DAT_CNT_SIZE                  8
/* Bit: 'FSM_STATE' */
/* Description: 'Parser FSM State' */
#define VR9_PARSER_STAT_FSM_STATE_OFFSET                  0x469
#define VR9_PARSER_STAT_FSM_STATE_SHIFT                   5
#define VR9_PARSER_STAT_FSM_STATE_SIZE                    3
/* Bit: 'PKT_ERR' */
/* Description: 'Packet error detected' */
#define VR9_PARSER_STAT_PKT_ERR_OFFSET                    0x469
#define VR9_PARSER_STAT_PKT_ERR_SHIFT                     4
#define VR9_PARSER_STAT_PKT_ERR_SIZE                      1
/* Bit: 'FSM_FIN' */
/* Description: 'Parser FSM finished' */
#define VR9_PARSER_STAT_FSM_FIN_OFFSET                    0x469
#define VR9_PARSER_STAT_FSM_FIN_SHIFT                     3
#define VR9_PARSER_STAT_FSM_FIN_SIZE                      1
/* Bit: 'FSM_START' */
/* Description: 'Parser FSM start' */
#define VR9_PARSER_STAT_FSM_START_OFFSET                  0x469
#define VR9_PARSER_STAT_FSM_START_SHIFT                   2
#define VR9_PARSER_STAT_FSM_START_SIZE                    1
/* Bit: 'FIFO_RDY' */
/* Description: 'Parser FIFO ready for read.' */
#define VR9_PARSER_STAT_FIFO_RDY_OFFSET                   0x469
#define VR9_PARSER_STAT_FIFO_RDY_SHIFT                    1
#define VR9_PARSER_STAT_FIFO_RDY_SIZE                     1
/* Bit: 'FIFO_FULL' */
/* Description: 'Parser's FIFO full' */
#define VR9_PARSER_STAT_FIFO_FULL_OFFSET                  0x469
#define VR9_PARSER_STAT_FIFO_FULL_SHIFT                   0
#define VR9_PARSER_STAT_FIFO_FULL_SIZE                    1
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 0' */
/* Bit: 'MCST' */
/* Description: 'Multicast Forwarding Mode Selection' */
#define VR9_PCE_PCTRL_0_MCST_OFFSET                       0x480
#define VR9_PCE_PCTRL_0_MCST_SHIFT                        13
#define VR9_PCE_PCTRL_0_MCST_SIZE                         1
/* Bit: 'EGSTEN' */
/* Description: 'Table-based Egress Special Tag Enable' */
#define VR9_PCE_PCTRL_0_EGSTEN_OFFSET                     0x480
#define VR9_PCE_PCTRL_0_EGSTEN_SHIFT                      12
#define VR9_PCE_PCTRL_0_EGSTEN_SIZE                       1
/* Bit: 'IGSTEN' */
/* Description: 'Ingress Special Tag Enable' */
#define VR9_PCE_PCTRL_0_IGSTEN_OFFSET                     0x480
#define VR9_PCE_PCTRL_0_IGSTEN_SHIFT                      11
#define VR9_PCE_PCTRL_0_IGSTEN_SIZE                       1
/* Bit: 'PCPEN' */
/* Description: 'PCP Remarking Mode' */
#define VR9_PCE_PCTRL_0_PCPEN_OFFSET                      0x480
#define VR9_PCE_PCTRL_0_PCPEN_SHIFT                       10
#define VR9_PCE_PCTRL_0_PCPEN_SIZE                        1
/* Bit: 'CLPEN' */
/* Description: 'Class Remarking Mode' */
#define VR9_PCE_PCTRL_0_CLPEN_OFFSET                      0x480
#define VR9_PCE_PCTRL_0_CLPEN_SHIFT                       9
#define VR9_PCE_PCTRL_0_CLPEN_SIZE                        1
/* Bit: 'DPEN' */
/* Description: 'Drop Precedence Remarking Mode' */
#define VR9_PCE_PCTRL_0_DPEN_OFFSET                       0x480
#define VR9_PCE_PCTRL_0_DPEN_SHIFT                        8
#define VR9_PCE_PCTRL_0_DPEN_SIZE                         1
/* Bit: 'CMOD' */
/* Description: 'Three-color Marker Color Mode' */
#define VR9_PCE_PCTRL_0_CMOD_OFFSET                       0x480
#define VR9_PCE_PCTRL_0_CMOD_SHIFT                        7
#define VR9_PCE_PCTRL_0_CMOD_SIZE                         1
/* Bit: 'VREP' */
/* Description: 'VLAN Replacement Mode' */
#define VR9_PCE_PCTRL_0_VREP_OFFSET                       0x480
#define VR9_PCE_PCTRL_0_VREP_SHIFT                        6
#define VR9_PCE_PCTRL_0_VREP_SIZE                         1
/* Bit: 'TVM' */
/* Description: 'Transparent VLAN Mode' */
#define VR9_PCE_PCTRL_0_TVM_OFFSET                        0x480
#define VR9_PCE_PCTRL_0_TVM_SHIFT                         5
#define VR9_PCE_PCTRL_0_TVM_SIZE                          1
/* Bit: 'PLOCK' */
/* Description: 'Port Locking Enable' */
#define VR9_PCE_PCTRL_0_PLOCK_OFFSET                      0x480
#define VR9_PCE_PCTRL_0_PLOCK_SHIFT                       4
#define VR9_PCE_PCTRL_0_PLOCK_SIZE                        1
/* Bit: 'AGEDIS' */
/* Description: 'Aging Disable' */
#define VR9_PCE_PCTRL_0_AGEDIS_OFFSET                     0x480
#define VR9_PCE_PCTRL_0_AGEDIS_SHIFT                      3
#define VR9_PCE_PCTRL_0_AGEDIS_SIZE                       1
/* Bit: 'PSTATE' */
/* Description: 'Port State' */
#define VR9_PCE_PCTRL_0_PSTATE_OFFSET                     0x480
#define VR9_PCE_PCTRL_0_PSTATE_SHIFT                      0
#define VR9_PCE_PCTRL_0_PSTATE_SIZE                       3
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 1' */
/* Bit: 'LRNLIM' */
/* Description: 'MAC Address Learning Limit' */
#define VR9_PCE_PCTRL_1_LRNLIM_OFFSET                     0x481
#define VR9_PCE_PCTRL_1_LRNLIM_SHIFT                      0
#define VR9_PCE_PCTRL_1_LRNLIM_SIZE                       8
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 2' */
/* Bit: 'DSCPMOD' */
/* Description: 'DSCP Mode Selection' */
#define VR9_PCE_PCTRL_2_DSCPMOD_OFFSET                    0x482
#define VR9_PCE_PCTRL_2_DSCPMOD_SHIFT                     7
#define VR9_PCE_PCTRL_2_DSCPMOD_SIZE                      1
/* Bit: 'DSCP' */
/* Description: 'Enable DSCP to select the Class of Service' */
#define VR9_PCE_PCTRL_2_DSCP_OFFSET                       0x482
#define VR9_PCE_PCTRL_2_DSCP_SHIFT                        5
#define VR9_PCE_PCTRL_2_DSCP_SIZE                         2
/* Bit: 'PCP' */
/* Description: 'Enable VLAN PCP to select the Class of Service' */
#define VR9_PCE_PCTRL_2_PCP_OFFSET                        0x482
#define VR9_PCE_PCTRL_2_PCP_SHIFT                         4
#define VR9_PCE_PCTRL_2_PCP_SIZE                          1
/* Bit: 'PCLASS' */
/* Description: 'Port-based Traffic Class' */
#define VR9_PCE_PCTRL_2_PCLASS_OFFSET                     0x482
#define VR9_PCE_PCTRL_2_PCLASS_SHIFT                      0
#define VR9_PCE_PCTRL_2_PCLASS_SIZE                       4
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 3' */
/* Bit: 'VIO_8' */
/* Description: 'Violation Type 8 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_8_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_8_SHIFT                       12
#define VR9_PCE_PCTRL_3_VIO_8_SIZE                        1
/* Bit: 'EDIR' */
/* Description: 'Egress Redirection Mode' */
#define VR9_PCE_PCTRL_3_EDIR_OFFSET                       0x483
#define VR9_PCE_PCTRL_3_EDIR_SHIFT                        11
#define VR9_PCE_PCTRL_3_EDIR_SIZE                         1
/* Bit: 'RXDMIR' */
/* Description: 'Receive Mirroring Enable for dropped frames' */
#define VR9_PCE_PCTRL_3_RXDMIR_OFFSET                     0x483
#define VR9_PCE_PCTRL_3_RXDMIR_SHIFT                      10
#define VR9_PCE_PCTRL_3_RXDMIR_SIZE                       1
/* Bit: 'RXVMIR' */
/* Description: 'Receive Mirroring Enable for valid frames' */
#define VR9_PCE_PCTRL_3_RXVMIR_OFFSET                     0x483
#define VR9_PCE_PCTRL_3_RXVMIR_SHIFT                      9
#define VR9_PCE_PCTRL_3_RXVMIR_SIZE                       1
/* Bit: 'TXMIR' */
/* Description: 'Transmit Mirroring Enable' */
#define VR9_PCE_PCTRL_3_TXMIR_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_TXMIR_SHIFT                       8
#define VR9_PCE_PCTRL_3_TXMIR_SIZE                        1
/* Bit: 'VIO_7' */
/* Description: 'Violation Type 7 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_7_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_7_SHIFT                       7
#define VR9_PCE_PCTRL_3_VIO_7_SIZE                        1
/* Bit: 'VIO_6' */
/* Description: 'Violation Type 6 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_6_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_6_SHIFT                       6
#define VR9_PCE_PCTRL_3_VIO_6_SIZE                        1
/* Bit: 'VIO_5' */
/* Description: 'Violation Type 5 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_5_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_5_SHIFT                       5
#define VR9_PCE_PCTRL_3_VIO_5_SIZE                        1
/* Bit: 'VIO_4' */
/* Description: 'Violation Type 4 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_4_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_4_SHIFT                       4
#define VR9_PCE_PCTRL_3_VIO_4_SIZE                        1
/* Bit: 'VIO_3' */
/* Description: 'Violation Type 3 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_3_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_3_SHIFT                       3
#define VR9_PCE_PCTRL_3_VIO_3_SIZE                        1
/* Bit: 'VIO_2' */
/* Description: 'Violation Type 2 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_2_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_2_SHIFT                       2
#define VR9_PCE_PCTRL_3_VIO_2_SIZE                        1
/* Bit: 'VIO_1' */
/* Description: 'Violation Type 1 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_1_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_1_SHIFT                       1
#define VR9_PCE_PCTRL_3_VIO_1_SIZE                        1
/* Bit: 'VIO_0' */
/* Description: 'Violation Type 0 Mirroring Enable' */
#define VR9_PCE_PCTRL_3_VIO_0_OFFSET                      0x483
#define VR9_PCE_PCTRL_3_VIO_0_SHIFT                       0
#define VR9_PCE_PCTRL_3_VIO_0_SIZE                        1
/* -------------------------------------------------------------------------- */
/* Register: 'Wake-on-LAN ControlRegister' */
/* Bit: 'PORT' */
/* Description: 'WoL Enable' */
#define VR9_WOL_CTRL_PORT_OFFSET                          0x484
#define VR9_WOL_CTRL_PORT_SHIFT                           0
#define VR9_WOL_CTRL_PORT_SIZE                            1
/* -------------------------------------------------------------------------- */
/* Register: 'PCE VLAN ControlRegister' */
/* Bit: 'VID0' */
/* Description: 'Priority Tagged Rule' */
#define VR9_PCE_VCTRL_VID0_OFFSET                         0x485
#define VR9_PCE_VCTRL_VID0_SHIFT                          6
#define VR9_PCE_VCTRL_VID0_SIZE                           1
/* Bit: 'VSR' */
/* Description: 'VLAN Security Rule' */
#define VR9_PCE_VCTRL_VSR_OFFSET                          0x485
#define VR9_PCE_VCTRL_VSR_SHIFT                           5
#define VR9_PCE_VCTRL_VSR_SIZE                            1
/* Bit: 'VEMR' */
/* Description: 'VLAN Egress Member Violation Rule' */
#define VR9_PCE_VCTRL_VEMR_OFFSET                         0x485
#define VR9_PCE_VCTRL_VEMR_SHIFT                          4
#define VR9_PCE_VCTRL_VEMR_SIZE                           1
/* Bit: 'VIMR' */
/* Description: 'VLAN Ingress Member Violation Rule' */
#define VR9_PCE_VCTRL_VIMR_OFFSET                         0x485
#define VR9_PCE_VCTRL_VIMR_SHIFT                          3
#define VR9_PCE_VCTRL_VIMR_SIZE                           1
/* Bit: 'VINR' */
/* Description: 'VLAN Ingress Tag Rule' */
#define VR9_PCE_VCTRL_VINR_OFFSET                         0x485
#define VR9_PCE_VCTRL_VINR_SHIFT                          1
#define VR9_PCE_VCTRL_VINR_SIZE                           2
/* Bit: 'UVR' */
/* Description: 'Unknown VLAN Rule' */
#define VR9_PCE_VCTRL_UVR_OFFSET                          0x485
#define VR9_PCE_VCTRL_UVR_SHIFT                           0
#define VR9_PCE_VCTRL_UVR_SIZE                            1
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Default PortVID Register' */
/* Bit: 'PVID' */
/* Description: 'Default Port VID Index' */
#define VR9_PCE_DEFPVID_PVID_OFFSET                       0x486
#define VR9_PCE_DEFPVID_PVID_SHIFT                        0
#define VR9_PCE_DEFPVID_PVID_SIZE                         6
/* -------------------------------------------------------------------------- */
/* Register: 'PCE Port StatusRegister' */
/* Bit: 'LRNCNT' */
/* Description: 'Learning Count' */
#define VR9_PCE_PSTAT_LRNCNT_OFFSET                       0x487
#define VR9_PCE_PSTAT_LRNCNT_SHIFT                        0
#define VR9_PCE_PSTAT_LRNCNT_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Port Interrupt Enable Register' */
/* Bit: 'FRZDRP' */
/* Description: 'MAC Table Freeze Drop Interrupt Enable' */
#define VR9_PCE_PIER_FRZDRP_OFFSET                        0x488
#define VR9_PCE_PIER_FRZDRP_SHIFT                         6
#define VR9_PCE_PIER_FRZDRP_SIZE                          1
/* Bit: 'CLDRP' */
/* Description: 'Classification Drop Interrupt Enable' */
#define VR9_PCE_PIER_CLDRP_OFFSET                         0x488
#define VR9_PCE_PIER_CLDRP_SHIFT                          5
#define VR9_PCE_PIER_CLDRP_SIZE                           1
/* Bit: 'PTDRP' */
/* Description: 'Port Drop Interrupt Enable' */
#define VR9_PCE_PIER_PTDRP_OFFSET                         0x488
#define VR9_PCE_PIER_PTDRP_SHIFT                          4
#define VR9_PCE_PIER_PTDRP_SIZE                           1
/* Bit: 'VLAN' */
/* Description: 'VLAN Violation Interrupt Enable' */
#define VR9_PCE_PIER_VLAN_OFFSET                          0x488
#define VR9_PCE_PIER_VLAN_SHIFT                           3
#define VR9_PCE_PIER_VLAN_SIZE                            1
/* Bit: 'WOL' */
/* Description: 'Wake-on-LAN Interrupt Enable' */
#define VR9_PCE_PIER_WOL_OFFSET                           0x488
#define VR9_PCE_PIER_WOL_SHIFT                            2
#define VR9_PCE_PIER_WOL_SIZE                             1
/* Bit: 'LOCK' */
/* Description: 'Port Lock Alert Interrupt Enable' */
#define VR9_PCE_PIER_LOCK_OFFSET                          0x488
#define VR9_PCE_PIER_LOCK_SHIFT                           1
#define VR9_PCE_PIER_LOCK_SIZE                            1
/* Bit: 'LIM' */
/* Description: 'Port Limit Alert Interrupt Enable' */
#define VR9_PCE_PIER_LIM_OFFSET                           0x488
#define VR9_PCE_PIER_LIM_SHIFT                            0
#define VR9_PCE_PIER_LIM_SIZE                             1
/* -------------------------------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Port Interrupt Status Register' */
/* Bit: 'FRZDRP' */
/* Description: 'MAC Table Freeze Drop Interrupt' */
#define VR9_PCE_PISR_FRZDRP_OFFSET                        0x489
#define VR9_PCE_PISR_FRZDRP_SHIFT                         6
#define VR9_PCE_PISR_FRZDRP_SIZE                          1
/* Bit: 'CLDRP' */
/* Description: 'Classification Drop Interrupt' */
#define VR9_PCE_PISR_CLDRP_OFFSET                         0x489
#define VR9_PCE_PISR_CLDRP_SHIFT                          5
#define VR9_PCE_PISR_CLDRP_SIZE                           1
/* Bit: 'PTDRP' */
/* Description: 'Port Drop Interrupt' */
#define VR9_PCE_PISR_PTDRP_OFFSET                         0x489
#define VR9_PCE_PISR_PTDRP_SHIFT                          4
#define VR9_PCE_PISR_PTDRP_SIZE                           1
/* Bit: 'VLAN' */
/* Description: 'VLAN Violation Interrupt' */
#define VR9_PCE_PISR_VLAN_OFFSET                          0x489
#define VR9_PCE_PISR_VLAN_SHIFT                           3
#define VR9_PCE_PISR_VLAN_SIZE                            1
/* Bit: 'WOL' */
/* Description: 'Wake-on-LAN Interrupt' */
#define VR9_PCE_PISR_WOL_OFFSET                           0x489
#define VR9_PCE_PISR_WOL_SHIFT                            2
#define VR9_PCE_PISR_WOL_SIZE                             1
/* Bit: 'LOCK' */
/* Description: 'Port Lock Alert Interrupt' */
#define VR9_PCE_PISR_LOCK_OFFSET                          0x489
#define VR9_PCE_PISR_LOCK_SHIFT                           1
#define VR9_PCE_PISR_LOCK_SIZE                            1
/* Bit: 'LIMIT' */
/* Description: 'Port Limitation Alert Interrupt' */
#define VR9_PCE_PISR_LIMIT_OFFSET                         0x489
#define VR9_PCE_PISR_LIMIT_SHIFT                          0
#define VR9_PCE_PISR_LIMIT_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Three-colorMarker Control Register' */
/* Bit: 'TCMEN' */
/* Description: 'Three-color Marker metering instance enable' */
#define VR9_PCE_TCM_CTRL_TCMEN_OFFSET                     0x580
#define VR9_PCE_TCM_CTRL_TCMEN_SHIFT                      0
#define VR9_PCE_TCM_CTRL_TCMEN_SIZE                       1
/* -------------------------------------------------------------------------- */
/* Register: 'Three-colorMarker Status Register' */
/* Bit: 'AL1' */
/* Description: 'Three-color Marker Alert 1 Status' */
#define VR9_PCE_TCM_STAT_AL1_OFFSET                       0x581
#define VR9_PCE_TCM_STAT_AL1_SHIFT                        1
#define VR9_PCE_TCM_STAT_AL1_SIZE                         1
/* Bit: 'AL0' */
/* Description: 'Three-color Marker Alert 0 Status' */
#define VR9_PCE_TCM_STAT_AL0_OFFSET                       0x581
#define VR9_PCE_TCM_STAT_AL0_SHIFT                        0
#define VR9_PCE_TCM_STAT_AL0_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'Three-color MarkerCommitted Burst Size Register' */
/* Bit: 'CBS' */
/* Description: 'Committed Burst Size' */
#define VR9_PCE_TCM_CBS_CBS_OFFSET                        0x582
#define VR9_PCE_TCM_CBS_CBS_SHIFT                         0
#define VR9_PCE_TCM_CBS_CBS_SIZE                          10
/* -------------------------------------------------------------------------- */
/* Register: 'Three-color MarkerExcess Burst Size Register' */
/* Bit: 'EBS' */
/* Description: 'Excess Burst Size' */
#define VR9_PCE_TCM_EBS_EBS_OFFSET                        0x583
#define VR9_PCE_TCM_EBS_EBS_SHIFT                         0
#define VR9_PCE_TCM_EBS_EBS_SIZE                          10
/* -------------------------------------------------------------------------- */
/* Register: 'Three-color MarkerInstantaneous Burst Size Register' */
/* Bit: 'IBS' */
/* Description: 'Instantaneous Burst Size' */
#define VR9_PCE_TCM_IBS_IBS_OFFSET                        0x584
#define VR9_PCE_TCM_IBS_IBS_SHIFT                         0
#define VR9_PCE_TCM_IBS_IBS_SIZE                          2
/* -------------------------------------------------------------------------- */
/* Register: 'Three-colorMarker Constant Information Rate Mantissa Register' */
/* Bit: 'MANT' */
/* Description: 'Rate Counter Mantissa' */
#define VR9_PCE_TCM_CIR_MANT_MANT_OFFSET                  0x585
#define VR9_PCE_TCM_CIR_MANT_MANT_SHIFT                   0
#define VR9_PCE_TCM_CIR_MANT_MANT_SIZE                    10
/* -------------------------------------------------------------------------- */
/* Register: 'Three-colorMarker Constant Information Rate Exponent Register' */
/* Bit: 'EXP' */
/* Description: 'Rate Counter Exponent' */
#define VR9_PCE_TCM_CIR_EXP_EXP_OFFSET                    0x586
#define VR9_PCE_TCM_CIR_EXP_EXP_SHIFT                     0
#define VR9_PCE_TCM_CIR_EXP_EXP_SIZE                      4
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Test Register' */
/* Bit: 'JTP' */
/* Description: 'Jitter Test Pattern' */
#define VR9_MAC_TEST_JTP_OFFSET                           0x8C0
#define VR9_MAC_TEST_JTP_SHIFT                            0
#define VR9_MAC_TEST_JTP_SIZE                             16
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Pause FrameSource Address Configuration Register' */
/* Bit: 'SAMOD' */
/* Description: 'Source Address Mode' */
#define VR9_MAC_PFAD_CFG_SAMOD_OFFSET                     0x8C1
#define VR9_MAC_PFAD_CFG_SAMOD_SHIFT                      0
#define VR9_MAC_PFAD_CFG_SAMOD_SIZE                       1
/* -------------------------------------------------------------------------- */
/* Register: 'Pause Frame SourceAddress Part 0 ' */
/* Bit: 'PFAD' */
/* Description: 'Pause Frame Source Address Part 0' */
#define VR9_MAC_PFSA_0_PFAD_OFFSET                        0x8C2
#define VR9_MAC_PFSA_0_PFAD_SHIFT                         0
#define VR9_MAC_PFSA_0_PFAD_SIZE                          16
/* -------------------------------------------------------------------------- */
/* Register: 'Pause Frame SourceAddress Part 1 ' */
/* Bit: 'PFAD' */
/* Description: 'Pause Frame Source Address Part 1' */
#define VR9_MAC_PFSA_1_PFAD_OFFSET                        0x8C3
#define VR9_MAC_PFSA_1_PFAD_SHIFT                         0
#define VR9_MAC_PFSA_1_PFAD_SIZE                          16
/* -------------------------------------------------------------------------- */
/* Register: 'Pause Frame SourceAddress Part 2 ' */
/* Bit: 'PFAD' */
/* Description: 'Pause Frame Source Address Part 2' */
#define VR9_MAC_PFSA_2_PFAD_OFFSET                        0x8C4
#define VR9_MAC_PFSA_2_PFAD_SHIFT                         0
#define VR9_MAC_PFSA_2_PFAD_SIZE                          16
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Frame Length Register' */
/* Bit: 'LEN' */
/* Description: 'Maximum Frame Length' */
#define VR9_MAC_FLEN_LEN_OFFSET                           0x8C5
#define VR9_MAC_FLEN_LEN_SHIFT                            0
#define VR9_MAC_FLEN_LEN_SIZE                             14
/* -------------------------------------------------------------------------- */
/* Register: 'MAC VLAN EthertypeRegister 0' */
/* Bit: 'OUTER' */
/* Description: 'Ethertype' */
#define VR9_MAC_VLAN_ETYPE_0_OUTER_OFFSET                 0x8C6
#define VR9_MAC_VLAN_ETYPE_0_OUTER_SHIFT                  0
#define VR9_MAC_VLAN_ETYPE_0_OUTER_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'MAC VLAN EthertypeRegister 1' */
/* Bit: 'INNER' */
/* Description: 'Ethertype' */
#define VR9_MAC_VLAN_ETYPE_1_INNER_OFFSET                 0x8C7
#define VR9_MAC_VLAN_ETYPE_1_INNER_SHIFT                  0
#define VR9_MAC_VLAN_ETYPE_1_INNER_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Interrupt EnableRegister' */
/* Bit: 'MACIEN' */
/* Description: 'MAC Interrupt Enable' */
#define VR9_MAC_IER_MACIEN_OFFSET                         0x8C8
#define VR9_MAC_IER_MACIEN_SHIFT                          0
#define VR9_MAC_IER_MACIEN_SIZE                           13
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Interrupt StatusRegister' */
/* Bit: 'MACINT' */
/* Description: 'MAC Interrupt' */
#define VR9_MAC_ISR_MACINT_OFFSET                         0x8C9
#define VR9_MAC_ISR_MACINT_SHIFT                          0
#define VR9_MAC_ISR_MACINT_SIZE                           13
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Port Status Register' */
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define VR9_MAC_PSTAT_PACT_OFFSET                         0x900
#define VR9_MAC_PSTAT_PACT_SHIFT                          11
#define VR9_MAC_PSTAT_PACT_SIZE                           1
/* Bit: 'GBIT' */
/* Description: 'Gigabit Speed Status' */
#define VR9_MAC_PSTAT_GBIT_OFFSET                         0x900
#define VR9_MAC_PSTAT_GBIT_SHIFT                          10
#define VR9_MAC_PSTAT_GBIT_SIZE                           1
/* Bit: 'MBIT' */
/* Description: 'Megabit Speed Status' */
#define VR9_MAC_PSTAT_MBIT_OFFSET                         0x900
#define VR9_MAC_PSTAT_MBIT_SHIFT                          9
#define VR9_MAC_PSTAT_MBIT_SIZE                           1
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define VR9_MAC_PSTAT_FDUP_OFFSET                         0x900
#define VR9_MAC_PSTAT_FDUP_SHIFT                          8
#define VR9_MAC_PSTAT_FDUP_SIZE                           1
/* Bit: 'RXPAU' */
/* Description: 'Receive Pause Status' */
#define VR9_MAC_PSTAT_RXPAU_OFFSET                        0x900
#define VR9_MAC_PSTAT_RXPAU_SHIFT                         7
#define VR9_MAC_PSTAT_RXPAU_SIZE                          1
/* Bit: 'TXPAU' */
/* Description: 'Transmit Pause Status' */
#define VR9_MAC_PSTAT_TXPAU_OFFSET                        0x900
#define VR9_MAC_PSTAT_TXPAU_SHIFT                         6
#define VR9_MAC_PSTAT_TXPAU_SIZE                          1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define VR9_MAC_PSTAT_RXPAUEN_OFFSET                      0x900
#define VR9_MAC_PSTAT_RXPAUEN_SHIFT                       5
#define VR9_MAC_PSTAT_RXPAUEN_SIZE                        1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define VR9_MAC_PSTAT_TXPAUEN_OFFSET                      0x900
#define VR9_MAC_PSTAT_TXPAUEN_SHIFT                       4
#define VR9_MAC_PSTAT_TXPAUEN_SIZE                        1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define VR9_MAC_PSTAT_LSTAT_OFFSET                        0x900
#define VR9_MAC_PSTAT_LSTAT_SHIFT                         3
#define VR9_MAC_PSTAT_LSTAT_SIZE                          1
/* Bit: 'CRS' */
/* Description: 'Carrier Sense Status' */
#define VR9_MAC_PSTAT_CRS_OFFSET                          0x900
#define VR9_MAC_PSTAT_CRS_SHIFT                           2
#define VR9_MAC_PSTAT_CRS_SIZE                            1
/* Bit: 'TXLPI' */
/* Description: 'Transmit Low-power Idle Status' */
#define VR9_MAC_PSTAT_TXLPI_OFFSET                        0x900
#define VR9_MAC_PSTAT_TXLPI_SHIFT                         1
#define VR9_MAC_PSTAT_TXLPI_SIZE                          1
/* Bit: 'RXLPI' */
/* Description: 'Receive Low-power Idle Status' */
#define VR9_MAC_PSTAT_RXLPI_OFFSET                        0x900
#define VR9_MAC_PSTAT_RXLPI_SHIFT                         0
#define VR9_MAC_PSTAT_RXLPI_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Interrupt Status Register' */
/* Bit: 'PHYERR' */
/* Description: 'PHY Error Interrupt' */
#define VR9_MAC_PISR_PHYERR_OFFSET                        0x901
#define VR9_MAC_PISR_PHYERR_SHIFT                         15
#define VR9_MAC_PISR_PHYERR_SIZE                          1
/* Bit: 'ALIGN' */
/* Description: 'Allignment Error Interrupt' */
#define VR9_MAC_PISR_ALIGN_OFFSET                         0x901
#define VR9_MAC_PISR_ALIGN_SHIFT                          14
#define VR9_MAC_PISR_ALIGN_SIZE                           1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define VR9_MAC_PISR_PACT_OFFSET                          0x901
#define VR9_MAC_PISR_PACT_SHIFT                           13
#define VR9_MAC_PISR_PACT_SIZE                            1
/* Bit: 'SPEED' */
/* Description: 'Megabit Speed Status' */
#define VR9_MAC_PISR_SPEED_OFFSET                         0x901
#define VR9_MAC_PISR_SPEED_SHIFT                          12
#define VR9_MAC_PISR_SPEED_SIZE                           1
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define VR9_MAC_PISR_FDUP_OFFSET                          0x901
#define VR9_MAC_PISR_FDUP_SHIFT                           11
#define VR9_MAC_PISR_FDUP_SIZE                            1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define VR9_MAC_PISR_RXPAUEN_OFFSET                       0x901
#define VR9_MAC_PISR_RXPAUEN_SHIFT                        10
#define VR9_MAC_PISR_RXPAUEN_SIZE                         1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define VR9_MAC_PISR_TXPAUEN_OFFSET                       0x901
#define VR9_MAC_PISR_TXPAUEN_SHIFT                        9
#define VR9_MAC_PISR_TXPAUEN_SIZE                         1
/* Bit: 'LPIOFF' */
/* Description: 'Receive Low-power Idle Mode is left' */
#define VR9_MAC_PISR_LPIOFF_OFFSET                        0x901
#define VR9_MAC_PISR_LPIOFF_SHIFT                         8
#define VR9_MAC_PISR_LPIOFF_SIZE                          1
/* Bit: 'LPION' */
/* Description: 'Receive Low-power Idle Mode is entered' */
#define VR9_MAC_PISR_LPION_OFFSET                         0x901
#define VR9_MAC_PISR_LPION_SHIFT                          7
#define VR9_MAC_PISR_LPION_SIZE                           1
/* Bit: 'JAM' */
/* Description: 'Jam Status Detected' */
#define VR9_MAC_PISR_JAM_OFFSET                           0x901
#define VR9_MAC_PISR_JAM_SHIFT                            6
#define VR9_MAC_PISR_JAM_SIZE                             1
/* Bit: 'TOOSHORT' */
/* Description: 'Too Short Frame Error Detected' */
#define VR9_MAC_PISR_TOOSHORT_OFFSET                      0x901
#define VR9_MAC_PISR_TOOSHORT_SHIFT                       5
#define VR9_MAC_PISR_TOOSHORT_SIZE                        1
/* Bit: 'TOOLONG' */
/* Description: 'Too Long Frame Error Detected' */
#define VR9_MAC_PISR_TOOLONG_OFFSET                       0x901
#define VR9_MAC_PISR_TOOLONG_SHIFT                        4
#define VR9_MAC_PISR_TOOLONG_SIZE                         1
/* Bit: 'LENERR' */
/* Description: 'Length Mismatch Error Detected' */
#define VR9_MAC_PISR_LENERR_OFFSET                        0x901
#define VR9_MAC_PISR_LENERR_SHIFT                         3
#define VR9_MAC_PISR_LENERR_SIZE                          1
/* Bit: 'FCSERR' */
/* Description: 'Frame Checksum Error Detected' */
#define VR9_MAC_PISR_FCSERR_OFFSET                        0x901
#define VR9_MAC_PISR_FCSERR_SHIFT                         2
#define VR9_MAC_PISR_FCSERR_SIZE                          1
/* Bit: 'TXPAUSE' */
/* Description: 'Pause Frame Transmitted' */
#define VR9_MAC_PISR_TXPAUSE_OFFSET                       0x901
#define VR9_MAC_PISR_TXPAUSE_SHIFT                        1
#define VR9_MAC_PISR_TXPAUSE_SIZE                         1
/* Bit: 'RXPAUSE' */
/* Description: 'Pause Frame Received' */
#define VR9_MAC_PISR_RXPAUSE_OFFSET                       0x901
#define VR9_MAC_PISR_RXPAUSE_SHIFT                        0
#define VR9_MAC_PISR_RXPAUSE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Interrupt Enable Register' */
/* Bit: 'PHYERR' */
/* Description: 'PHY Error Interrupt' */
#define VR9_MAC_PIER_PHYERR_OFFSET                        0x902
#define VR9_MAC_PIER_PHYERR_SHIFT                         15
#define VR9_MAC_PIER_PHYERR_SIZE                          1
/* Bit: 'ALIGN' */
/* Description: 'Allignment Error Interrupt' */
#define VR9_MAC_PIER_ALIGN_OFFSET                         0x902
#define VR9_MAC_PIER_ALIGN_SHIFT                          14
#define VR9_MAC_PIER_ALIGN_SIZE                           1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define VR9_MAC_PIER_PACT_OFFSET                          0x902
#define VR9_MAC_PIER_PACT_SHIFT                           13
#define VR9_MAC_PIER_PACT_SIZE                            1
/* Bit: 'SPEED' */
/* Description: 'Megabit Speed Status' */
#define VR9_MAC_PIER_SPEED_OFFSET                         0x902
#define VR9_MAC_PIER_SPEED_SHIFT                          12
#define VR9_MAC_PIER_SPEED_SIZE                           1
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define VR9_MAC_PIER_FDUP_OFFSET                          0x902
#define VR9_MAC_PIER_FDUP_SHIFT                           11
#define VR9_MAC_PIER_FDUP_SIZE                            1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define VR9_MAC_PIER_RXPAUEN_OFFSET                       0x902
#define VR9_MAC_PIER_RXPAUEN_SHIFT                        10
#define VR9_MAC_PIER_RXPAUEN_SIZE                         1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define VR9_MAC_PIER_TXPAUEN_OFFSET                       0x902
#define VR9_MAC_PIER_TXPAUEN_SHIFT                        9
#define VR9_MAC_PIER_TXPAUEN_SIZE                         1
/* Bit: 'LPIOFF' */
/* Description: 'Low-power Idle Off Interrupt Mask' */
#define VR9_MAC_PIER_LPIOFF_OFFSET                        0x902
#define VR9_MAC_PIER_LPIOFF_SHIFT                         8
#define VR9_MAC_PIER_LPIOFF_SIZE                          1
/* Bit: 'LPION' */
/* Description: 'Low-power Idle On Interrupt Mask' */
#define VR9_MAC_PIER_LPION_OFFSET                         0x902
#define VR9_MAC_PIER_LPION_SHIFT                          7
#define VR9_MAC_PIER_LPION_SIZE                           1
/* Bit: 'JAM' */
/* Description: 'Jam Status Interrupt Mask' */
#define VR9_MAC_PIER_JAM_OFFSET                           0x902
#define VR9_MAC_PIER_JAM_SHIFT                            6
#define VR9_MAC_PIER_JAM_SIZE                             1
/* Bit: 'TOOSHORT' */
/* Description: 'Too Short Frame Error Interrupt Mask' */
#define VR9_MAC_PIER_TOOSHORT_OFFSET                      0x902
#define VR9_MAC_PIER_TOOSHORT_SHIFT                       5
#define VR9_MAC_PIER_TOOSHORT_SIZE                        1
/* Bit: 'TOOLONG' */
/* Description: 'Too Long Frame Error Interrupt Mask' */
#define VR9_MAC_PIER_TOOLONG_OFFSET                       0x902
#define VR9_MAC_PIER_TOOLONG_SHIFT                        4
#define VR9_MAC_PIER_TOOLONG_SIZE                         1
/* Bit: 'LENERR' */
/* Description: 'Length Mismatch Error Interrupt Mask' */
#define VR9_MAC_PIER_LENERR_OFFSET                        0x902
#define VR9_MAC_PIER_LENERR_SHIFT                         3
#define VR9_MAC_PIER_LENERR_SIZE                          1
/* Bit: 'FCSERR' */
/* Description: 'Frame Checksum Error Interrupt Mask' */
#define VR9_MAC_PIER_FCSERR_OFFSET                        0x902
#define VR9_MAC_PIER_FCSERR_SHIFT                         2
#define VR9_MAC_PIER_FCSERR_SIZE                          1
/* Bit: 'TXPAUSE' */
/* Description: 'Transmit Pause Frame Interrupt Mask' */
#define VR9_MAC_PIER_TXPAUSE_OFFSET                       0x902
#define VR9_MAC_PIER_TXPAUSE_SHIFT                        1
#define VR9_MAC_PIER_TXPAUSE_SIZE                         1
/* Bit: 'RXPAUSE' */
/* Description: 'Receive Pause Frame Interrupt Mask' */
#define VR9_MAC_PIER_RXPAUSE_OFFSET                       0x902
#define VR9_MAC_PIER_RXPAUSE_SHIFT                        0
#define VR9_MAC_PIER_RXPAUSE_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Control Register0' */
/* Bit: 'BM' */
/* Description: 'Burst Mode Control' */
#define VR9_MAC_CTRL_0_BM_OFFSET                          0x903
#define VR9_MAC_CTRL_0_BM_SHIFT                           12
#define VR9_MAC_CTRL_0_BM_SIZE                            1
/* Bit: 'APADEN' */
/* Description: 'Automatic VLAN Padding Enable' */
#define VR9_MAC_CTRL_0_APADEN_OFFSET                      0x903
#define VR9_MAC_CTRL_0_APADEN_SHIFT                       11
#define VR9_MAC_CTRL_0_APADEN_SIZE                        1
/* Bit: 'VPAD2EN' */
/* Description: 'Stacked VLAN Padding Enable' */
#define VR9_MAC_CTRL_0_VPAD2EN_OFFSET                     0x903
#define VR9_MAC_CTRL_0_VPAD2EN_SHIFT                      10
#define VR9_MAC_CTRL_0_VPAD2EN_SIZE                       1
/* Bit: 'VPADEN' */
/* Description: 'VLAN Padding Enable' */
#define VR9_MAC_CTRL_0_VPADEN_OFFSET                      0x903
#define VR9_MAC_CTRL_0_VPADEN_SHIFT                       9
#define VR9_MAC_CTRL_0_VPADEN_SIZE                        1
/* Bit: 'PADEN' */
/* Description: 'Padding Enable' */
#define VR9_MAC_CTRL_0_PADEN_OFFSET                       0x903
#define VR9_MAC_CTRL_0_PADEN_SHIFT                        8
#define VR9_MAC_CTRL_0_PADEN_SIZE                         1
/* Bit: 'FCS' */
/* Description: 'Transmit FCS Control' */
#define VR9_MAC_CTRL_0_FCS_OFFSET                         0x903
#define VR9_MAC_CTRL_0_FCS_SHIFT                          7
#define VR9_MAC_CTRL_0_FCS_SIZE                           1
/* Bit: 'FCON' */
/* Description: 'Flow Control Mode' */
#define VR9_MAC_CTRL_0_FCON_OFFSET                        0x903
#define VR9_MAC_CTRL_0_FCON_SHIFT                         4
#define VR9_MAC_CTRL_0_FCON_SIZE                          3
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_MAC_CTRL_0_FDUP_OFFSET                        0x903
#define VR9_MAC_CTRL_0_FDUP_SHIFT                         2
#define VR9_MAC_CTRL_0_FDUP_SIZE                          2
/* Bit: 'GMII' */
/* Description: 'GMII/MII interface mode selection' */
#define VR9_MAC_CTRL_0_GMII_OFFSET                        0x903
#define VR9_MAC_CTRL_0_GMII_SHIFT                         0
#define VR9_MAC_CTRL_0_GMII_SIZE                          2
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Control Register1' */
/* Bit: 'DEFERMODE' */
/* Description: 'Defer Model' */
#define VR9_MAC_CTRL_1_DEFERMODE_OFFSET                   0x904
#define VR9_MAC_CTRL_1_DEFERMODE_SHIFT                    15
#define VR9_MAC_CTRL_1_DEFERMODE_SIZE                     1
/* Bit: 'SHORTPRE' */
/* Description: 'Short Preamble Control' */
#define VR9_MAC_CTRL_1_SHORTPRE_OFFSET                    0x904
#define VR9_MAC_CTRL_1_SHORTPRE_SHIFT                     8
#define VR9_MAC_CTRL_1_SHORTPRE_SIZE                      1
/* Bit: 'IPG' */
/* Description: 'Minimum Inter Packet Gap Size' */
#define VR9_MAC_CTRL_1_IPG_OFFSET                         0x904
#define VR9_MAC_CTRL_1_IPG_SHIFT                          0
#define VR9_MAC_CTRL_1_IPG_SIZE                           4
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Control Register2' */
/* Bit: 'MLEN' */
/* Description: 'Maximum Untagged Frame Length' */
#define VR9_MAC_CTRL_2_MLEN_OFFSET                        0x905
#define VR9_MAC_CTRL_2_MLEN_SHIFT                         3
#define VR9_MAC_CTRL_2_MLEN_SIZE                          1
/* Bit: 'LCHKL' */
/* Description: 'Frame Length Check Long Enable' */
#define VR9_MAC_CTRL_2_LCHKL_OFFSET                       0x905
#define VR9_MAC_CTRL_2_LCHKL_SHIFT                        2
#define VR9_MAC_CTRL_2_LCHKL_SIZE                         1
/* Bit: 'LCHKS' */
/* Description: 'Frame Length Check Short Enable' */
#define VR9_MAC_CTRL_2_LCHKS_OFFSET                       0x905
#define VR9_MAC_CTRL_2_LCHKS_SHIFT                        0
#define VR9_MAC_CTRL_2_LCHKS_SIZE                         2
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Control Register3' */
/* Bit: 'RCNT' */
/* Description: 'Retry Count' */
#define VR9_MAC_CTRL_3_RCNT_OFFSET                        0x906
#define VR9_MAC_CTRL_3_RCNT_SHIFT                         0
#define VR9_MAC_CTRL_3_RCNT_SIZE                          4
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Control Register4' */
/* Bit: 'GWAIT' */
/* Description: 'LPI Wait Time for 1G' */
#define VR9_MAC_CTRL_4_GWAIT_OFFSET                       0x907
#define VR9_MAC_CTRL_4_GWAIT_SHIFT                        8
#define VR9_MAC_CTRL_4_GWAIT_SIZE                         7
/* Bit: 'LPIEN' */
/* Description: 'LPI Mode Enable' */
#define VR9_MAC_CTRL_4_LPIEN_OFFSET                       0x907
#define VR9_MAC_CTRL_4_LPIEN_SHIFT                        7
#define VR9_MAC_CTRL_4_LPIEN_SIZE                         1
/* Bit: 'WAIT' */
/* Description: 'LPI Wait Time for 100M' */
#define VR9_MAC_CTRL_4_WAIT_OFFSET                        0x907
#define VR9_MAC_CTRL_4_WAIT_SHIFT                         0
#define VR9_MAC_CTRL_4_WAIT_SIZE                          7
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Control Register5' */
/* Bit: 'PJPS_NOBP' */
/* Description: 'Prolonged Jam pattern size during no-backpressure
state' */
#define VR9_MAC_CTRL_5_PJPS_NOBP_OFFSET                   0x908
#define VR9_MAC_CTRL_5_PJPS_NOBP_SHIFT                    1
#define VR9_MAC_CTRL_5_PJPS_NOBP_SIZE                     1
/* Bit: 'PJPS_BP' */
/* Description: 'Prolonged Jam pattern size during backpressure state' */
#define VR9_MAC_CTRL_5_PJPS_BP_OFFSET                     0x908
#define VR9_MAC_CTRL_5_PJPS_BP_SHIFT                      0
#define VR9_MAC_CTRL_5_PJPS_BP_SIZE                       1
/* -------------------------------------------------------------------------- */
/* Register: 'MAC Test Enable Register' */
/* Bit: 'JTEN' */
/* Description: 'Jitter Test Enable' */
#define VR9_MAC_TESTEN_JTEN_OFFSET                        0x90B
#define VR9_MAC_TESTEN_JTEN_SHIFT                         2
#define VR9_MAC_TESTEN_JTEN_SIZE                          1
/* Bit: 'TXER' */
/* Description: 'Transmit Error Insertion' */
#define VR9_MAC_TESTEN_TXER_OFFSET                        0x90B
#define VR9_MAC_TESTEN_TXER_SHIFT                         1
#define VR9_MAC_TESTEN_TXER_SIZE                          1
/* Bit: 'LOOP' */
/* Description: 'MAC Loopback Enable' */
#define VR9_MAC_TESTEN_LOOP_OFFSET                        0x90B
#define VR9_MAC_TESTEN_LOOP_SHIFT                         0
#define VR9_MAC_TESTEN_LOOP_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch FetchDMA Control Register' */
/* Bit: 'EGCNT' */
/* Description: 'Egress Special Tag RMON count' */
#define VR9_FDMA_CTRL_EGCNT_OFFSET                        0xA40
#define VR9_FDMA_CTRL_EGCNT_SHIFT                         7
#define VR9_FDMA_CTRL_EGCNT_SIZE                          1
/* Bit: 'LPI_MODE' */
/* Description: 'Low Power Idle Mode' */
#define VR9_FDMA_CTRL_LPI_MODE_OFFSET                     0xA40
#define VR9_FDMA_CTRL_LPI_MODE_SHIFT                      4
#define VR9_FDMA_CTRL_LPI_MODE_SIZE                       3
/* Bit: 'EGSTAG' */
/* Description: 'Egress Special Tag Size' */
#define VR9_FDMA_CTRL_EGSTAG_OFFSET                       0xA40
#define VR9_FDMA_CTRL_EGSTAG_SHIFT                        2
#define VR9_FDMA_CTRL_EGSTAG_SIZE                         2
/* Bit: 'IGSTAG' */
/* Description: 'Ingress Special Tag Size' */
#define VR9_FDMA_CTRL_IGSTAG_OFFSET                       0xA40
#define VR9_FDMA_CTRL_IGSTAG_SHIFT                        1
#define VR9_FDMA_CTRL_IGSTAG_SIZE                         1
/* -------------------------------------------------------------------------- */
/* Register: 'Special Tag EthertypeControl Register' */
/* Bit: 'ETYPE' */
/* Description: 'Special Tag Ethertype' */
#define VR9_FDMA_STETYPE_ETYPE_OFFSET                     0xA41
#define VR9_FDMA_STETYPE_ETYPE_SHIFT                      0
#define VR9_FDMA_STETYPE_ETYPE_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'VLAN Tag EthertypeControl Register' */
/* Bit: 'ETYPE' */
/* Description: 'VLAN Tag Ethertype' */
#define VR9_FDMA_VTETYPE_ETYPE_OFFSET                     0xA42
#define VR9_FDMA_VTETYPE_ETYPE_SHIFT                      0
#define VR9_FDMA_VTETYPE_ETYPE_SIZE                       16
/* -------------------------------------------------------------------------- */
/* Register: 'FDMA Status Register0' */
/* Bit: 'FSMS' */
/* Description: 'FSM states status' */
#define VR9_FDMA_STAT_0_FSMS_OFFSET                       0xA43
#define VR9_FDMA_STAT_0_FSMS_SHIFT                        0
#define VR9_FDMA_STAT_0_FSMS_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'Fetch DMA Global InterruptEnable Register' */
/* Bit: 'PCKD' */
/* Description: 'Packet Drop Interrupt Enable' */
#define VR9_FDMA_IER_PCKD_OFFSET                          0xA44
#define VR9_FDMA_IER_PCKD_SHIFT                           14
#define VR9_FDMA_IER_PCKD_SIZE                            1
/* Bit: 'PCKR' */
/* Description: 'Packet Ready Interrupt Enable' */
#define VR9_FDMA_IER_PCKR_OFFSET                          0xA44
#define VR9_FDMA_IER_PCKR_SHIFT                           13
#define VR9_FDMA_IER_PCKR_SIZE                            1
/* Bit: 'PCKT' */
/* Description: 'Packet Sent Interrupt Enable' */
#define VR9_FDMA_IER_PCKT_OFFSET                          0xA44
#define VR9_FDMA_IER_PCKT_SHIFT                           0
#define VR9_FDMA_IER_PCKT_SIZE                            13
/* -------------------------------------------------------------------------- */
/* Register: 'Fetch DMA Global InterruptStatus Register' */
/* Bit: 'PCKTD' */
/* Description: 'Packet Drop' */
#define VR9_FDMA_ISR_PCKTD_OFFSET                         0xA45
#define VR9_FDMA_ISR_PCKTD_SHIFT                          14
#define VR9_FDMA_ISR_PCKTD_SIZE                           1
/* Bit: 'PCKR' */
/* Description: 'Packet is Ready for Transmission' */
#define VR9_FDMA_ISR_PCKR_OFFSET                          0xA45
#define VR9_FDMA_ISR_PCKR_SHIFT                           13
#define VR9_FDMA_ISR_PCKR_SIZE                            1
/* Bit: 'PCKT' */
/* Description: 'Packet Sent Event' */
#define VR9_FDMA_ISR_PCKT_OFFSET                          0xA45
#define VR9_FDMA_ISR_PCKT_SHIFT                           0
#define VR9_FDMA_ISR_PCKT_SIZE                            13
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Control Register' */
/* Bit: 'ST_TYPE' */
/* Description: 'Special Tag Ethertype Mode' */
#define VR9_FDMA_PCTRL_ST_TYPE_OFFSET                     0xA80
#define VR9_FDMA_PCTRL_ST_TYPE_SHIFT                      5
#define VR9_FDMA_PCTRL_ST_TYPE_SIZE                       1
/* Bit: 'VLANMOD' */
/* Description: 'VLAN Modification Control' */
#define VR9_FDMA_PCTRL_VLANMOD_OFFSET                     0xA80
#define VR9_FDMA_PCTRL_VLANMOD_SHIFT                      3
#define VR9_FDMA_PCTRL_VLANMOD_SIZE                       2
/* Bit: 'DSCPRM' */
/* Description: 'DSCP Re-marking Enable' */
#define VR9_FDMA_PCTRL_DSCPRM_OFFSET                      0xA80
#define VR9_FDMA_PCTRL_DSCPRM_SHIFT                       2
#define VR9_FDMA_PCTRL_DSCPRM_SIZE                        1
/* Bit: 'STEN' */
/* Description: 'Special Tag Insertion Enable' */
#define VR9_FDMA_PCTRL_STEN_OFFSET                        0xA80
#define VR9_FDMA_PCTRL_STEN_SHIFT                         1
#define VR9_FDMA_PCTRL_STEN_SIZE                          1
/* Bit: 'EN' */
/* Description: 'FDMA Port Enable' */
#define VR9_FDMA_PCTRL_EN_OFFSET                          0xA80
#define VR9_FDMA_PCTRL_EN_SHIFT                           0
#define VR9_FDMA_PCTRL_EN_SIZE                            1
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Priority Register' */
/* Bit: 'PRIO' */
/* Description: 'FDMA PRIO' */
#define VR9_FDMA_PRIO_PRIO_OFFSET                         0xA81
#define VR9_FDMA_PRIO_PRIO_SHIFT                          0
#define VR9_FDMA_PRIO_PRIO_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Status Register 0' */
/* Bit: 'PKT_AVAIL' */
/* Description: 'Port Egress Packet Available' */
#define VR9_FDMA_PSTAT0_PKT_AVAIL_OFFSET                  0xA82
#define VR9_FDMA_PSTAT0_PKT_AVAIL_SHIFT                   15
#define VR9_FDMA_PSTAT0_PKT_AVAIL_SIZE                    1
/* Bit: 'POK' */
/* Description: 'Port Status OK' */
#define VR9_FDMA_PSTAT0_POK_OFFSET                        0xA82
#define VR9_FDMA_PSTAT0_POK_SHIFT                         14
#define VR9_FDMA_PSTAT0_POK_SIZE                          1
/* Bit: 'PSEG' */
/* Description: 'Port Egress Segment Count' */
#define VR9_FDMA_PSTAT0_PSEG_OFFSET                       0xA82
#define VR9_FDMA_PSTAT0_PSEG_SHIFT                        0
#define VR9_FDMA_PSTAT0_PSEG_SIZE                         6
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Status Register 1' */
/* Bit: 'HDR_PTR' */
/* Description: 'Header Pointer' */
#define VR9_FDMA_PSTAT1_HDR_PTR_OFFSET                    0xA83
#define VR9_FDMA_PSTAT1_HDR_PTR_SHIFT                     0
#define VR9_FDMA_PSTAT1_HDR_PTR_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'Egress TimeStamp Register 0' */
/* Bit: 'TSTL' */
/* Description: 'Time Stamp [15:0]' */
#define VR9_FDMA_TSTAMP0_TSTL_OFFSET                      0xA84
#define VR9_FDMA_TSTAMP0_TSTL_SHIFT                       0
#define VR9_FDMA_TSTAMP0_TSTL_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'Egress TimeStamp Register 1' */
/* Bit: 'TSTH' */
/* Description: 'Time Stamp [31:16]' */
#define VR9_FDMA_TSTAMP1_TSTH_OFFSET                      0xA85
#define VR9_FDMA_TSTAMP1_TSTH_SHIFT                       0
#define VR9_FDMA_TSTAMP1_TSTH_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet Switch StoreDMA Control Register' */
/* Bit: 'RMON_ALIGN' */
/* Description: 'MUX to select what to count on align error rmon
counter' */
#define VR9_SDMA_CTRL_RMON_ALIGN_OFFSET                   0xB40
#define VR9_SDMA_CTRL_RMON_ALIGN_SHIFT                    2
#define VR9_SDMA_CTRL_RMON_ALIGN_SIZE                     2
/* Bit: 'ARBIT' */
/* Description: 'SIMPLE ARBITER FOR PARSER FILLING' */
#define VR9_SDMA_CTRL_ARBIT_OFFSET                        0xB40
#define VR9_SDMA_CTRL_ARBIT_SHIFT                         1
#define VR9_SDMA_CTRL_ARBIT_SIZE                          1
/* Bit: 'TSTEN' */
/* Description: 'Time Stamp Enable' */
#define VR9_SDMA_CTRL_TSTEN_OFFSET                        0xB40
#define VR9_SDMA_CTRL_TSTEN_SHIFT                         0
#define VR9_SDMA_CTRL_TSTEN_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold1 Register' */
/* Bit: 'THR1' */
/* Description: 'Threshold 1' */
#define VR9_SDMA_FCTHR1_THR1_OFFSET                       0xB41
#define VR9_SDMA_FCTHR1_THR1_SHIFT                        0
#define VR9_SDMA_FCTHR1_THR1_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold2 Register' */
/* Bit: 'THR2' */
/* Description: 'Threshold 2' */
#define VR9_SDMA_FCTHR2_THR2_OFFSET                       0xB42
#define VR9_SDMA_FCTHR2_THR2_SHIFT                        0
#define VR9_SDMA_FCTHR2_THR2_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold3 Register' */
/* Bit: 'THR3' */
/* Description: 'Threshold 3' */
#define VR9_SDMA_FCTHR3_THR3_OFFSET                       0xB43
#define VR9_SDMA_FCTHR3_THR3_SHIFT                        0
#define VR9_SDMA_FCTHR3_THR3_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold4 Register' */
/* Bit: 'THR4' */
/* Description: 'Threshold 4' */
#define VR9_SDMA_FCTHR4_THR4_OFFSET                       0xB44
#define VR9_SDMA_FCTHR4_THR4_SHIFT                        0
#define VR9_SDMA_FCTHR4_THR4_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold5 Register' */
/* Bit: 'THR5' */
/* Description: 'Threshold 5' */
#define VR9_SDMA_FCTHR5_THR5_OFFSET                       0xB45
#define VR9_SDMA_FCTHR5_THR5_SHIFT                        0
#define VR9_SDMA_FCTHR5_THR5_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold6 Register' */
/* Bit: 'THR6' */
/* Description: 'Threshold 6' */
#define VR9_SDMA_FCTHR6_THR6_OFFSET                       0xB46
#define VR9_SDMA_FCTHR6_THR6_SHIFT                        0
#define VR9_SDMA_FCTHR6_THR6_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold7 Register' */
/* Bit: 'THR7' */
/* Description: 'Threshold 7' */
#define VR9_SDMA_FCTHR7_THR7_OFFSET                       0xB47
#define VR9_SDMA_FCTHR7_THR7_SHIFT                        0
#define VR9_SDMA_FCTHR7_THR7_SIZE                         11
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Status Register0' */
/* Bit: 'BPS_FILL' */
/* Description: 'Back Pressure Status' */
#define VR9_SDMA_STAT_0_BPS_FILL_OFFSET                   0xB48
#define VR9_SDMA_STAT_0_BPS_FILL_SHIFT                    4
#define VR9_SDMA_STAT_0_BPS_FILL_SIZE                     3
/* Bit: 'BPS_PNT' */
/* Description: 'Back Pressure Status' */
#define VR9_SDMA_STAT_0_BPS_PNT_OFFSET                    0xB48
#define VR9_SDMA_STAT_0_BPS_PNT_SHIFT                     2
#define VR9_SDMA_STAT_0_BPS_PNT_SIZE                      2
/* Bit: 'DROP' */
/* Description: 'Back Pressure Status' */
#define VR9_SDMA_STAT_0_DROP_OFFSET                       0xB48
#define VR9_SDMA_STAT_0_DROP_SHIFT                        0
#define VR9_SDMA_STAT_0_DROP_SIZE                         2
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Status Register1' */
/* Bit: 'FILL' */
/* Description: 'Buffer Filling Level' */
#define VR9_SDMA_STAT_1_FILL_OFFSET                       0xB49
#define VR9_SDMA_STAT_1_FILL_SHIFT                        0
#define VR9_SDMA_STAT_1_FILL_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Status Register2' */
/* Bit: 'FSMS' */
/* Description: 'FSM states status' */
#define VR9_SDMA_STAT_2_FSMS_OFFSET                       0xB4A
#define VR9_SDMA_STAT_2_FSMS_SHIFT                        0
#define VR9_SDMA_STAT_2_FSMS_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Interrupt Enable Register' */
/* Bit: 'BPEX' */
/* Description: 'Buffer Pointers Exceeded' */
#define VR9_SDMA_IER_BPEX_OFFSET                          0xB4B
#define VR9_SDMA_IER_BPEX_SHIFT                           15
#define VR9_SDMA_IER_BPEX_SIZE                            1
/* Bit: 'BFULL' */
/* Description: 'Buffer Full' */
#define VR9_SDMA_IER_BFULL_OFFSET                         0xB4B
#define VR9_SDMA_IER_BFULL_SHIFT                          14
#define VR9_SDMA_IER_BFULL_SIZE                           1
/* Bit: 'FERR' */
/* Description: 'Frame Error' */
#define VR9_SDMA_IER_FERR_OFFSET                          0xB4B
#define VR9_SDMA_IER_FERR_SHIFT                           13
#define VR9_SDMA_IER_FERR_SIZE                            1
/* Bit: 'FRX' */
/* Description: 'Frame Received Successfully' */
#define VR9_SDMA_IER_FRX_OFFSET                           0xB4B
#define VR9_SDMA_IER_FRX_SHIFT                            0
#define VR9_SDMA_IER_FRX_SIZE                             13
/* -------------------------------------------------------------------------- */
/* Register: 'SDMA Interrupt Status Register' */
/* Bit: 'BPEX' */
/* Description: 'Packet Descriptors Exceeded' */
#define VR9_SDMA_ISR_BPEX_OFFSET                          0xB4C
#define VR9_SDMA_ISR_BPEX_SHIFT                           15
#define VR9_SDMA_ISR_BPEX_SIZE                            1
/* Bit: 'BFULL' */
/* Description: 'Buffer Full' */
#define VR9_SDMA_ISR_BFULL_OFFSET                         0xB4C
#define VR9_SDMA_ISR_BFULL_SHIFT                          14
#define VR9_SDMA_ISR_BFULL_SIZE                           1
/* Bit: 'FERR' */
/* Description: 'Frame Error' */
#define VR9_SDMA_ISR_FERR_OFFSET                          0xB4C
#define VR9_SDMA_ISR_FERR_SHIFT                           13
#define VR9_SDMA_ISR_FERR_SIZE                            1
/* Bit: 'FRX' */
/* Description: 'Frame Received Successfully' */
#define VR9_SDMA_ISR_FRX_OFFSET                           0xB4C
#define VR9_SDMA_ISR_FRX_SHIFT                            0
#define VR9_SDMA_ISR_FRX_SIZE                             13
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Control Register' */
/* Bit: 'DTHR' */
/* Description: 'Drop Threshold Selection' */
#define VR9_SDMA_PCTRL_DTHR_OFFSET                        0xBC0
#define VR9_SDMA_PCTRL_DTHR_SHIFT                         13
#define VR9_SDMA_PCTRL_DTHR_SIZE                          2
/* Bit: 'PTHR' */
/* Description: 'Pause Threshold Selection' */
#define VR9_SDMA_PCTRL_PTHR_OFFSET                        0xBC0
#define VR9_SDMA_PCTRL_PTHR_SHIFT                         11
#define VR9_SDMA_PCTRL_PTHR_SIZE                          2
/* Bit: 'PHYEFWD' */
/* Description: 'Forward PHY Error Frames' */
#define VR9_SDMA_PCTRL_PHYEFWD_OFFSET                     0xBC0
#define VR9_SDMA_PCTRL_PHYEFWD_SHIFT                      10
#define VR9_SDMA_PCTRL_PHYEFWD_SIZE                       1
/* Bit: 'ALGFWD' */
/* Description: 'Forward Alignment Error Frames' */
#define VR9_SDMA_PCTRL_ALGFWD_OFFSET                      0xBC0
#define VR9_SDMA_PCTRL_ALGFWD_SHIFT                       9
#define VR9_SDMA_PCTRL_ALGFWD_SIZE                        1
/* Bit: 'LENFWD' */
/* Description: 'Forward Length Errored Frames' */
#define VR9_SDMA_PCTRL_LENFWD_OFFSET                      0xBC0
#define VR9_SDMA_PCTRL_LENFWD_SHIFT                       8
#define VR9_SDMA_PCTRL_LENFWD_SIZE                        1
/* Bit: 'OSFWD' */
/* Description: 'Forward Oversized Frames' */
#define VR9_SDMA_PCTRL_OSFWD_OFFSET                       0xBC0
#define VR9_SDMA_PCTRL_OSFWD_SHIFT                        7
#define VR9_SDMA_PCTRL_OSFWD_SIZE                         1
/* Bit: 'USFWD' */
/* Description: 'Forward Undersized Frames' */
#define VR9_SDMA_PCTRL_USFWD_OFFSET                       0xBC0
#define VR9_SDMA_PCTRL_USFWD_SHIFT                        6
#define VR9_SDMA_PCTRL_USFWD_SIZE                         1
/* Bit: 'FCSIGN' */
/* Description: 'Ignore FCS Errors' */
#define VR9_SDMA_PCTRL_FCSIGN_OFFSET                      0xBC0
#define VR9_SDMA_PCTRL_FCSIGN_SHIFT                       5
#define VR9_SDMA_PCTRL_FCSIGN_SIZE                        1
/* Bit: 'FCSFWD' */
/* Description: 'Forward FCS Errored Frames' */
#define VR9_SDMA_PCTRL_FCSFWD_OFFSET                      0xBC0
#define VR9_SDMA_PCTRL_FCSFWD_SHIFT                       4
#define VR9_SDMA_PCTRL_FCSFWD_SIZE                        1
/* Bit: 'PAUFWD' */
/* Description: 'Pause Frame Forwarding' */
#define VR9_SDMA_PCTRL_PAUFWD_OFFSET                      0xBC0
#define VR9_SDMA_PCTRL_PAUFWD_SHIFT                       3
#define VR9_SDMA_PCTRL_PAUFWD_SIZE                        1
/* Bit: 'MFCEN' */
/* Description: 'Metering Flow Control Enable' */
#define VR9_SDMA_PCTRL_MFCEN_OFFSET                       0xBC0
#define VR9_SDMA_PCTRL_MFCEN_SHIFT                        2
#define VR9_SDMA_PCTRL_MFCEN_SIZE                         1
/* Bit: 'FCEN' */
/* Description: 'Flow Control Enable' */
#define VR9_SDMA_PCTRL_FCEN_OFFSET                        0xBC0
#define VR9_SDMA_PCTRL_FCEN_SHIFT                         1
#define VR9_SDMA_PCTRL_FCEN_SIZE                          1
/* Bit: 'PEN' */
/* Description: 'Port Enable' */
#define VR9_SDMA_PCTRL_PEN_OFFSET                         0xBC0
#define VR9_SDMA_PCTRL_PEN_SHIFT                          0
#define VR9_SDMA_PCTRL_PEN_SIZE                           1
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Priority Register' */
/* Bit: 'MIN_IFG' */
/* Description: 'Minimum IFG,SFD and preamble' */
#define VR9_SDMA_PRIO_MIN_IFG_OFFSET                      0xBC1
#define VR9_SDMA_PRIO_MIN_IFG_SHIFT                       7
#define VR9_SDMA_PRIO_MIN_IFG_SIZE                        5
/* Bit: 'PHYEIGN' */
/* Description: 'Ignore PHY Error Frames' */
#define VR9_SDMA_PRIO_PHYEIGN_OFFSET                      0xBC1
#define VR9_SDMA_PRIO_PHYEIGN_SHIFT                       6
#define VR9_SDMA_PRIO_PHYEIGN_SIZE                        1
/* Bit: 'ALGIGN' */
/* Description: 'Ignore Alignment Error Frames' */
#define VR9_SDMA_PRIO_ALGIGN_OFFSET                       0xBC1
#define VR9_SDMA_PRIO_ALGIGN_SHIFT                        5
#define VR9_SDMA_PRIO_ALGIGN_SIZE                         1
/* Bit: 'LENIGN' */
/* Description: 'Ignore Length Errored Frames' */
#define VR9_SDMA_PRIO_LENIGN_OFFSET                       0xBC1
#define VR9_SDMA_PRIO_LENIGN_SHIFT                        4
#define VR9_SDMA_PRIO_LENIGN_SIZE                         1
/* Bit: 'OSIGN' */
/* Description: 'Ignore Oversized Frames' */
#define VR9_SDMA_PRIO_OSIGN_OFFSET                        0xBC1
#define VR9_SDMA_PRIO_OSIGN_SHIFT                         3
#define VR9_SDMA_PRIO_OSIGN_SIZE                          1
/* Bit: 'USIGN' */
/* Description: 'Ignore Undersized Frames' */
#define VR9_SDMA_PRIO_USIGN_OFFSET                        0xBC1
#define VR9_SDMA_PRIO_USIGN_SHIFT                         2
#define VR9_SDMA_PRIO_USIGN_SIZE                          1
/* Bit: 'PRIO' */
/* Description: 'SDMA PRIO' */
#define VR9_SDMA_PRIO_PRIO_OFFSET                         0xBC1
#define VR9_SDMA_PRIO_PRIO_SHIFT                          0
#define VR9_SDMA_PRIO_PRIO_SIZE                           2
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Status Register 0' */
/* Bit: 'HDR_PTR' */
/* Description: 'Port Ingress Queue Header Pointer' */
#define VR9_SDMA_PSTAT0_HDR_PTR_OFFSET                    0xBC2
#define VR9_SDMA_PSTAT0_HDR_PTR_SHIFT                     0
#define VR9_SDMA_PSTAT0_HDR_PTR_SIZE                      10
/* -------------------------------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Status Register 1' */
/* Bit: 'PPKT' */
/* Description: 'Port Ingress Packet Count' */
#define VR9_SDMA_PSTAT1_PPKT_OFFSET                       0xBC3
#define VR9_SDMA_PSTAT1_PPKT_SHIFT                        0
#define VR9_SDMA_PSTAT1_PPKT_SIZE                         10
/* -------------------------------------------------------------------------- */
/* Register: 'Ingress TimeStamp Register 0' */
/* Bit: 'TSTL' */
/* Description: 'Time Stamp [15:0]' */
#define VR9_SDMA_TSTAMP0_TSTL_OFFSET                      0xBC4
#define VR9_SDMA_TSTAMP0_TSTL_SHIFT                       0
#define VR9_SDMA_TSTAMP0_TSTL_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'Ingress TimeStamp Register 1' */
/* Bit: 'TSTH' */
/* Description: 'Time Stamp [31:16]' */
#define VR9_SDMA_TSTAMP1_TSTH_OFFSET                      0xBC5
#define VR9_SDMA_TSTAMP1_TSTH_SHIFT                       0
#define VR9_SDMA_TSTAMP1_TSTH_SIZE                        16
/* -------------------------------------------------------------------------- */
#endif /* #ifndef _VR9_SWITCH_H */
