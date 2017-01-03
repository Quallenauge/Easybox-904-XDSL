/******************************************************************************

                               Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _VR9_TOP_H
#define _VR9_TOP_H
/* -------------------------------------------------------------------------- */
/* Register: 'Global Control Register0' */
/* Bit: 'SE' */
/* Description: 'Global Switch Macro Enable' */
#define VR9_GLOB_CTRL_SE_OFFSET                           0x000
#define VR9_GLOB_CTRL_SE_SHIFT                            15
#define VR9_GLOB_CTRL_SE_SIZE                             1
/* Bit: 'HWRES' */
/* Description: 'Global Hardware Reset' */
#define VR9_GLOB_CTRL_HWRES_OFFSET                        0x000
#define VR9_GLOB_CTRL_HWRES_SHIFT                         1
#define VR9_GLOB_CTRL_HWRES_SIZE                          1
/* Bit: 'SWRES' */
/* Description: 'Global Software Reset' */
#define VR9_GLOB_CTRL_SWRES_OFFSET                        0x000
#define VR9_GLOB_CTRL_SWRES_SHIFT                         0
#define VR9_GLOB_CTRL_SWRES_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'MDIO Control Register' */
/* Bit: 'MBUSY' */
/* Description: 'MDIO Busy' */
#define VR9_MDIO_CTRL_MBUSY_OFFSET                        0x008
#define VR9_MDIO_CTRL_MBUSY_SHIFT                         12
#define VR9_MDIO_CTRL_MBUSY_SIZE                          1
/* Bit: 'OP' */
/* Description: 'Operation Code' */
#define VR9_MDIO_CTRL_OP_OFFSET                           0x008
#define VR9_MDIO_CTRL_OP_SHIFT                            10
#define VR9_MDIO_CTRL_OP_SIZE                             2
/* Bit: 'PHYAD' */
/* Description: 'PHY Address' */
#define VR9_MDIO_CTRL_PHYAD_OFFSET                        0x008
#define VR9_MDIO_CTRL_PHYAD_SHIFT                         5
#define VR9_MDIO_CTRL_PHYAD_SIZE                          5
/* Bit: 'REGAD' */
/* Description: 'Register Address' */
#define VR9_MDIO_CTRL_REGAD_OFFSET                        0x008
#define VR9_MDIO_CTRL_REGAD_SHIFT                         0
#define VR9_MDIO_CTRL_REGAD_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'MDIO Read Data Register' */
/* Bit: 'RDATA' */
/* Description: 'Read Data' */
#define VR9_MDIO_READ_RDATA_OFFSET                        0x009
#define VR9_MDIO_READ_RDATA_SHIFT                         0
#define VR9_MDIO_READ_RDATA_SIZE                          16
/* -------------------------------------------------------------------------- */
/* Register: 'MDIO Write Data Register' */
/* Bit: 'WDATA' */
/* Description: 'Write Data' */
#define VR9_MDIO_WRITE_WDATA_OFFSET                       0x00A
#define VR9_MDIO_WRITE_WDATA_SHIFT                        0
#define VR9_MDIO_WRITE_WDATA_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'MDC Clock ConfigurationRegister 0' */
/* Bit: 'PEN_5' */
/* Description: 'Polling State Machine Enable' */
#define VR9_MDC_CFG_0_PEN_5_OFFSET                        0x00B
#define VR9_MDC_CFG_0_PEN_5_SHIFT                         5
#define VR9_MDC_CFG_0_PEN_5_SIZE                          1
/* Bit: 'PEN_4' */
/* Description: 'Polling State Machine Enable' */
#define VR9_MDC_CFG_0_PEN_4_OFFSET                        0x00B
#define VR9_MDC_CFG_0_PEN_4_SHIFT                         4
#define VR9_MDC_CFG_0_PEN_4_SIZE                          1
/* Bit: 'PEN_3' */
/* Description: 'Polling State Machine Enable' */
#define VR9_MDC_CFG_0_PEN_3_OFFSET                        0x00B
#define VR9_MDC_CFG_0_PEN_3_SHIFT                         3
#define VR9_MDC_CFG_0_PEN_3_SIZE                          1
/* Bit: 'PEN_2' */
/* Description: 'Polling State Machine Enable' */
#define VR9_MDC_CFG_0_PEN_2_OFFSET                        0x00B
#define VR9_MDC_CFG_0_PEN_2_SHIFT                         2
#define VR9_MDC_CFG_0_PEN_2_SIZE                          1
/* Bit: 'PEN_1' */
/* Description: 'Polling State Machine Enable' */
#define VR9_MDC_CFG_0_PEN_1_OFFSET                        0x00B
#define VR9_MDC_CFG_0_PEN_1_SHIFT                         1
#define VR9_MDC_CFG_0_PEN_1_SIZE                          1
/* Bit: 'PEN_0' */
/* Description: 'Polling State Machine Enable' */
#define VR9_MDC_CFG_0_PEN_0_OFFSET                        0x00B
#define VR9_MDC_CFG_0_PEN_0_SHIFT                         0
#define VR9_MDC_CFG_0_PEN_0_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'MDC Clock ConfigurationRegister 1' */
/* Bit: 'RES' */
/* Description: 'MDIO Hardware Reset' */
#define VR9_MDC_CFG_1_RES_OFFSET                          0x00C
#define VR9_MDC_CFG_1_RES_SHIFT                           15
#define VR9_MDC_CFG_1_RES_SIZE                            1
/* Bit: 'MCEN' */
/* Description: 'Management Clock Enable' */
#define VR9_MDC_CFG_1_MCEN_OFFSET                         0x00C
#define VR9_MDC_CFG_1_MCEN_SHIFT                          8
#define VR9_MDC_CFG_1_MCEN_SIZE                           1
/* Bit: 'FREQ' */
/* Description: 'MDIO Interface Clock Rate' */
#define VR9_MDC_CFG_1_FREQ_OFFSET                         0x00C
#define VR9_MDC_CFG_1_FREQ_SHIFT                          0
#define VR9_MDC_CFG_1_FREQ_SIZE                           8
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 5' */
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define VR9_PHY_ADDR_5_LNKST_OFFSET                       0x010
#define VR9_PHY_ADDR_5_LNKST_SHIFT                        13
#define VR9_PHY_ADDR_5_LNKST_SIZE                         2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PHY_ADDR_5_SPEED_OFFSET                       0x010
#define VR9_PHY_ADDR_5_SPEED_SHIFT                        11
#define VR9_PHY_ADDR_5_SPEED_SIZE                         2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_PHY_ADDR_5_FDUP_OFFSET                        0x010
#define VR9_PHY_ADDR_5_FDUP_SHIFT                         9
#define VR9_PHY_ADDR_5_FDUP_SIZE                          2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define VR9_PHY_ADDR_5_FCONTX_OFFSET                      0x010
#define VR9_PHY_ADDR_5_FCONTX_SHIFT                       7
#define VR9_PHY_ADDR_5_FCONTX_SIZE                        2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define VR9_PHY_ADDR_5_FCONRX_OFFSET                      0x010
#define VR9_PHY_ADDR_5_FCONRX_SHIFT                       5
#define VR9_PHY_ADDR_5_FCONRX_SIZE                        2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define VR9_PHY_ADDR_5_ADDR_OFFSET                        0x010
#define VR9_PHY_ADDR_5_ADDR_SHIFT                         0
#define VR9_PHY_ADDR_5_ADDR_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 4' */
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define VR9_PHY_ADDR_4_LNKST_OFFSET                       0x011
#define VR9_PHY_ADDR_4_LNKST_SHIFT                        13
#define VR9_PHY_ADDR_4_LNKST_SIZE                         2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PHY_ADDR_4_SPEED_OFFSET                       0x011
#define VR9_PHY_ADDR_4_SPEED_SHIFT                        11
#define VR9_PHY_ADDR_4_SPEED_SIZE                         2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_PHY_ADDR_4_FDUP_OFFSET                        0x011
#define VR9_PHY_ADDR_4_FDUP_SHIFT                         9
#define VR9_PHY_ADDR_4_FDUP_SIZE                          2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define VR9_PHY_ADDR_4_FCONTX_OFFSET                      0x011
#define VR9_PHY_ADDR_4_FCONTX_SHIFT                       7
#define VR9_PHY_ADDR_4_FCONTX_SIZE                        2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define VR9_PHY_ADDR_4_FCONRX_OFFSET                      0x011
#define VR9_PHY_ADDR_4_FCONRX_SHIFT                       5
#define VR9_PHY_ADDR_4_FCONRX_SIZE                        2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define VR9_PHY_ADDR_4_ADDR_OFFSET                        0x011
#define VR9_PHY_ADDR_4_ADDR_SHIFT                         0
#define VR9_PHY_ADDR_4_ADDR_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 3' */
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define VR9_PHY_ADDR_3_LNKST_OFFSET                       0x012
#define VR9_PHY_ADDR_3_LNKST_SHIFT                        13
#define VR9_PHY_ADDR_3_LNKST_SIZE                         2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PHY_ADDR_3_SPEED_OFFSET                       0x012
#define VR9_PHY_ADDR_3_SPEED_SHIFT                        11
#define VR9_PHY_ADDR_3_SPEED_SIZE                         2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_PHY_ADDR_3_FDUP_OFFSET                        0x012
#define VR9_PHY_ADDR_3_FDUP_SHIFT                         9
#define VR9_PHY_ADDR_3_FDUP_SIZE                          2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define VR9_PHY_ADDR_3_FCONTX_OFFSET                      0x012
#define VR9_PHY_ADDR_3_FCONTX_SHIFT                       7
#define VR9_PHY_ADDR_3_FCONTX_SIZE                        2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define VR9_PHY_ADDR_3_FCONRX_OFFSET                      0x012
#define VR9_PHY_ADDR_3_FCONRX_SHIFT                       5
#define VR9_PHY_ADDR_3_FCONRX_SIZE                        2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define VR9_PHY_ADDR_3_ADDR_OFFSET                        0x012
#define VR9_PHY_ADDR_3_ADDR_SHIFT                         0
#define VR9_PHY_ADDR_3_ADDR_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 2' */
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define VR9_PHY_ADDR_2_LNKST_OFFSET                       0x013
#define VR9_PHY_ADDR_2_LNKST_SHIFT                        13
#define VR9_PHY_ADDR_2_LNKST_SIZE                         2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PHY_ADDR_2_SPEED_OFFSET                       0x013
#define VR9_PHY_ADDR_2_SPEED_SHIFT                        11
#define VR9_PHY_ADDR_2_SPEED_SIZE                         2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_PHY_ADDR_2_FDUP_OFFSET                        0x013
#define VR9_PHY_ADDR_2_FDUP_SHIFT                         9
#define VR9_PHY_ADDR_2_FDUP_SIZE                          2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define VR9_PHY_ADDR_2_FCONTX_OFFSET                      0x013
#define VR9_PHY_ADDR_2_FCONTX_SHIFT                       7
#define VR9_PHY_ADDR_2_FCONTX_SIZE                        2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define VR9_PHY_ADDR_2_FCONRX_OFFSET                      0x013
#define VR9_PHY_ADDR_2_FCONRX_SHIFT                       5
#define VR9_PHY_ADDR_2_FCONRX_SIZE                        2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define VR9_PHY_ADDR_2_ADDR_OFFSET                        0x013
#define VR9_PHY_ADDR_2_ADDR_SHIFT                         0
#define VR9_PHY_ADDR_2_ADDR_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 1' */
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define VR9_PHY_ADDR_1_LNKST_OFFSET                       0x014
#define VR9_PHY_ADDR_1_LNKST_SHIFT                        13
#define VR9_PHY_ADDR_1_LNKST_SIZE                         2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PHY_ADDR_1_SPEED_OFFSET                       0x014
#define VR9_PHY_ADDR_1_SPEED_SHIFT                        11
#define VR9_PHY_ADDR_1_SPEED_SIZE                         2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_PHY_ADDR_1_FDUP_OFFSET                        0x014
#define VR9_PHY_ADDR_1_FDUP_SHIFT                         9
#define VR9_PHY_ADDR_1_FDUP_SIZE                          2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define VR9_PHY_ADDR_1_FCONTX_OFFSET                      0x014
#define VR9_PHY_ADDR_1_FCONTX_SHIFT                       7
#define VR9_PHY_ADDR_1_FCONTX_SIZE                        2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define VR9_PHY_ADDR_1_FCONRX_OFFSET                      0x014
#define VR9_PHY_ADDR_1_FCONRX_SHIFT                       5
#define VR9_PHY_ADDR_1_FCONRX_SIZE                        2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define VR9_PHY_ADDR_1_ADDR_OFFSET                        0x014
#define VR9_PHY_ADDR_1_ADDR_SHIFT                         0
#define VR9_PHY_ADDR_1_ADDR_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 0' */
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define VR9_PHY_ADDR_0_LNKST_OFFSET                       0x015
#define VR9_PHY_ADDR_0_LNKST_SHIFT                        13
#define VR9_PHY_ADDR_0_LNKST_SIZE                         2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PHY_ADDR_0_SPEED_OFFSET                       0x015
#define VR9_PHY_ADDR_0_SPEED_SHIFT                        11
#define VR9_PHY_ADDR_0_SPEED_SIZE                         2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define VR9_PHY_ADDR_0_FDUP_OFFSET                        0x015
#define VR9_PHY_ADDR_0_FDUP_SHIFT                         9
#define VR9_PHY_ADDR_0_FDUP_SIZE                          2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define VR9_PHY_ADDR_0_FCONTX_OFFSET                      0x015
#define VR9_PHY_ADDR_0_FCONTX_SHIFT                       7
#define VR9_PHY_ADDR_0_FCONTX_SIZE                        2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define VR9_PHY_ADDR_0_FCONRX_OFFSET                      0x015
#define VR9_PHY_ADDR_0_FCONRX_SHIFT                       5
#define VR9_PHY_ADDR_0_FCONRX_SIZE                        2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define VR9_PHY_ADDR_0_ADDR_OFFSET                        0x015
#define VR9_PHY_ADDR_0_ADDR_SHIFT                         0
#define VR9_PHY_ADDR_0_ADDR_SIZE                          5
/* -------------------------------------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define VR9_MDIO_STAT_0_CLK_STOP_CAPABLE_OFFSET           0x016
#define VR9_MDIO_STAT_0_CLK_STOP_CAPABLE_SHIFT            8
#define VR9_MDIO_STAT_0_CLK_STOP_CAPABLE_SIZE             1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define VR9_MDIO_STAT_0_EEE_CAPABLE_OFFSET                0x016
#define VR9_MDIO_STAT_0_EEE_CAPABLE_SHIFT                 7
#define VR9_MDIO_STAT_0_EEE_CAPABLE_SIZE                  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define VR9_MDIO_STAT_0_PACT_OFFSET                       0x016
#define VR9_MDIO_STAT_0_PACT_SHIFT                        6
#define VR9_MDIO_STAT_0_PACT_SIZE                         1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define VR9_MDIO_STAT_0_LSTAT_OFFSET                      0x016
#define VR9_MDIO_STAT_0_LSTAT_SHIFT                       5
#define VR9_MDIO_STAT_0_LSTAT_SIZE                        1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_MDIO_STAT_0_SPEED_OFFSET                      0x016
#define VR9_MDIO_STAT_0_SPEED_SHIFT                       3
#define VR9_MDIO_STAT_0_SPEED_SIZE                        2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define VR9_MDIO_STAT_0_FDUP_OFFSET                       0x016
#define VR9_MDIO_STAT_0_FDUP_SHIFT                        2
#define VR9_MDIO_STAT_0_FDUP_SIZE                         1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define VR9_MDIO_STAT_0_RXPAUEN_OFFSET                    0x016
#define VR9_MDIO_STAT_0_RXPAUEN_SHIFT                     1
#define VR9_MDIO_STAT_0_RXPAUEN_SIZE                      1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define VR9_MDIO_STAT_0_TXPAUEN_OFFSET                    0x016
#define VR9_MDIO_STAT_0_TXPAUEN_SHIFT                     0
#define VR9_MDIO_STAT_0_TXPAUEN_SIZE                      1
/* -------------------------------------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define VR9_ANEG_EEE_0_CLK_STOP_CAPABLE_OFFSET            0x01C
#define VR9_ANEG_EEE_0_CLK_STOP_CAPABLE_SHIFT             2
#define VR9_ANEG_EEE_0_CLK_STOP_CAPABLE_SIZE              2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define VR9_ANEG_EEE_0_EEE_CAPABLE_OFFSET                 0x01C
#define VR9_ANEG_EEE_0_EEE_CAPABLE_SHIFT                  0
#define VR9_ANEG_EEE_0_EEE_CAPABLE_SIZE                   2
/* -------------------------------------------------------------------------- */
/* Register: 'xMII Port 0 ConfigurationRegister' */
/* Bit: 'RES' */
/* Description: 'Hardware Reset' */
#define VR9_MII_CFG_0_RES_OFFSET                          0x036
#define VR9_MII_CFG_0_RES_SHIFT                           15
#define VR9_MII_CFG_0_RES_SIZE                            1
/* Bit: 'EN' */
/* Description: 'xMII Interface Enable' */
#define VR9_MII_CFG_0_EN_OFFSET                           0x036
#define VR9_MII_CFG_0_EN_SHIFT                            14
#define VR9_MII_CFG_0_EN_SIZE                             1
/* Bit: 'ISOL' */
/* Description: 'ISOLATE xMII Interface' */
#define VR9_MII_CFG_0_ISOL_OFFSET                         0x036
#define VR9_MII_CFG_0_ISOL_SHIFT                          13
#define VR9_MII_CFG_0_ISOL_SIZE                           1
/* Bit: 'LDCLKDIS' */
/* Description: 'Link Down Clock Disable' */
#define VR9_MII_CFG_0_LDCLKDIS_OFFSET                     0x036
#define VR9_MII_CFG_0_LDCLKDIS_SHIFT                      12
#define VR9_MII_CFG_0_LDCLKDIS_SIZE                       1
/* Bit: 'CRS' */
/* Description: 'CRS Sensitivity Configuration' */
#define VR9_MII_CFG_0_CRS_OFFSET                          0x036
#define VR9_MII_CFG_0_CRS_SHIFT                           9
#define VR9_MII_CFG_0_CRS_SIZE                            2
/* Bit: 'RGMII_IBS' */
/* Description: 'RGMII In Band Status' */
#define VR9_MII_CFG_0_RGMII_IBS_OFFSET                    0x036
#define VR9_MII_CFG_0_RGMII_IBS_SHIFT                     8
#define VR9_MII_CFG_0_RGMII_IBS_SIZE                      1
/* Bit: 'RMII' */
/* Description: 'RMII Reference Clock Direction of the Port' */
#define VR9_MII_CFG_0_RMII_OFFSET                         0x036
#define VR9_MII_CFG_0_RMII_SHIFT                          7
#define VR9_MII_CFG_0_RMII_SIZE                           1
/* Bit: 'MIIRATE' */
/* Description: 'xMII Port Interface Clock Rate' */
#define VR9_MII_CFG_0_MIIRATE_OFFSET                      0x036
#define VR9_MII_CFG_0_MIIRATE_SHIFT                       4
#define VR9_MII_CFG_0_MIIRATE_SIZE                        3
/* Bit: 'MIIMODE' */
/* Description: 'xMII Interface Mode' */
#define VR9_MII_CFG_0_MIIMODE_OFFSET                      0x036
#define VR9_MII_CFG_0_MIIMODE_SHIFT                       0
#define VR9_MII_CFG_0_MIIMODE_SIZE                        4
/* -------------------------------------------------------------------------- */
/* Register: 'Configuration of ClockDelay for Port 0' */
/* Bit: 'RXLOCK' */
/* Description: 'Lock Status MDL of Receive PCDU' */
#define VR9_PCDU_0_RXLOCK_OFFSET                          0x037
#define VR9_PCDU_0_RXLOCK_SHIFT                           15
#define VR9_PCDU_0_RXLOCK_SIZE                            1
/* Bit: 'TXLOCK' */
/* Description: 'Lock Status of MDL of Transmit PCDU' */
#define VR9_PCDU_0_TXLOCK_OFFSET                          0x037
#define VR9_PCDU_0_TXLOCK_SHIFT                           14
#define VR9_PCDU_0_TXLOCK_SIZE                            1
/* Bit: 'RXDLY' */
/* Description: 'Configure Receive Clock Delay' */
#define VR9_PCDU_0_RXDLY_OFFSET                           0x037
#define VR9_PCDU_0_RXDLY_SHIFT                            7
#define VR9_PCDU_0_RXDLY_SIZE                             3
/* Bit: 'TXDLY' */
/* Description: 'Configure Transmit PCDU' */
#define VR9_PCDU_0_TXDLY_OFFSET                           0x037
#define VR9_PCDU_0_TXDLY_SHIFT                            0
#define VR9_PCDU_0_TXDLY_SIZE                             3
/* -------------------------------------------------------------------------- */
/* Register: 'xMII Port 1 ConfigurationRegister' */
/* Bit: 'RES' */
/* Description: 'Hardware Reset' */
#define VR9_MII_CFG_1_RES_OFFSET                          0x038
#define VR9_MII_CFG_1_RES_SHIFT                           15
#define VR9_MII_CFG_1_RES_SIZE                            1
/* Bit: 'EN' */
/* Description: 'xMII Interface Enable' */
#define VR9_MII_CFG_1_EN_OFFSET                           0x038
#define VR9_MII_CFG_1_EN_SHIFT                            14
#define VR9_MII_CFG_1_EN_SIZE                             1
/* Bit: 'ISOL' */
/* Description: 'ISOLATE xMII Interface' */
#define VR9_MII_CFG_1_ISOL_OFFSET                         0x038
#define VR9_MII_CFG_1_ISOL_SHIFT                          13
#define VR9_MII_CFG_1_ISOL_SIZE                           1
/* Bit: 'LDCLKDIS' */
/* Description: 'Link Down Clock Disable' */
#define VR9_MII_CFG_1_LDCLKDIS_OFFSET                     0x038
#define VR9_MII_CFG_1_LDCLKDIS_SHIFT                      12
#define VR9_MII_CFG_1_LDCLKDIS_SIZE                       1
/* Bit: 'CRS' */
/* Description: 'CRS Sensitivity Configuration' */
#define VR9_MII_CFG_1_CRS_OFFSET                          0x038
#define VR9_MII_CFG_1_CRS_SHIFT                           9
#define VR9_MII_CFG_1_CRS_SIZE                            2
/* Bit: 'RGMII_IBS' */
/* Description: 'RGMII In Band Status' */
#define VR9_MII_CFG_1_RGMII_IBS_OFFSET                    0x038
#define VR9_MII_CFG_1_RGMII_IBS_SHIFT                     8
#define VR9_MII_CFG_1_RGMII_IBS_SIZE                      1
/* Bit: 'RMII' */
/* Description: 'RMII Reference Clock Direction of the Port' */
#define VR9_MII_CFG_1_RMII_OFFSET                         0x038
#define VR9_MII_CFG_1_RMII_SHIFT                          7
#define VR9_MII_CFG_1_RMII_SIZE                           1
/* Bit: 'MIIRATE' */
/* Description: 'xMII Port Interface Clock Rate' */
#define VR9_MII_CFG_1_MIIRATE_OFFSET                      0x038
#define VR9_MII_CFG_1_MIIRATE_SHIFT                       4
#define VR9_MII_CFG_1_MIIRATE_SIZE                        3
/* Bit: 'MIIMODE' */
/* Description: 'xMII Interface Mode' */
#define VR9_MII_CFG_1_MIIMODE_OFFSET                      0x038
#define VR9_MII_CFG_1_MIIMODE_SHIFT                       0
#define VR9_MII_CFG_1_MIIMODE_SIZE                        4
/* -------------------------------------------------------------------------- */
/* Register: 'Configuration of ClockDelay for Port 1' */
/* Bit: 'RXLOCK' */
/* Description: 'Lock Status MDL of Receive PCDU' */
#define VR9_PCDU_1_RXLOCK_OFFSET                          0x039
#define VR9_PCDU_1_RXLOCK_SHIFT                           15
#define VR9_PCDU_1_RXLOCK_SIZE                            1
/* Bit: 'TXLOCK' */
/* Description: 'Lock Status of MDL of Transmit PCDU' */
#define VR9_PCDU_1_TXLOCK_OFFSET                          0x039
#define VR9_PCDU_1_TXLOCK_SHIFT                           14
#define VR9_PCDU_1_TXLOCK_SIZE                            1
/* Bit: 'RXDLY' */
/* Description: 'Configure Receive Clock Delay' */
#define VR9_PCDU_1_RXDLY_OFFSET                           0x039
#define VR9_PCDU_1_RXDLY_SHIFT                            7
#define VR9_PCDU_1_RXDLY_SIZE                             3
/* Bit: 'TXDLY' */
/* Description: 'Configure Transmit PCDU' */
#define VR9_PCDU_1_TXDLY_OFFSET                           0x039
#define VR9_PCDU_1_TXDLY_SHIFT                            0
#define VR9_PCDU_1_TXDLY_SIZE                             3
/* -------------------------------------------------------------------------- */
/* Register: 'xMII Port 5 ConfigurationRegister' */
/* Bit: 'RES' */
/* Description: 'Hardware Reset' */
#define VR9_MII_CFG_5_RES_OFFSET                          0x040
#define VR9_MII_CFG_5_RES_SHIFT                           15
#define VR9_MII_CFG_5_RES_SIZE                            1
/* Bit: 'EN' */
/* Description: 'xMII Interface Enable' */
#define VR9_MII_CFG_5_EN_OFFSET                           0x040
#define VR9_MII_CFG_5_EN_SHIFT                            14
#define VR9_MII_CFG_5_EN_SIZE                             1
/* Bit: 'ISOL' */
/* Description: 'ISOLATE xMII Interface' */
#define VR9_MII_CFG_5_ISOL_OFFSET                         0x040
#define VR9_MII_CFG_5_ISOL_SHIFT                          13
#define VR9_MII_CFG_5_ISOL_SIZE                           1
/* Bit: 'LDCLKDIS' */
/* Description: 'Link Down Clock Disable' */
#define VR9_MII_CFG_5_LDCLKDIS_OFFSET                     0x040
#define VR9_MII_CFG_5_LDCLKDIS_SHIFT                      12
#define VR9_MII_CFG_5_LDCLKDIS_SIZE                       1
/* Bit: 'CRS' */
/* Description: 'CRS Sensitivity Configuration' */
#define VR9_MII_CFG_5_CRS_OFFSET                          0x040
#define VR9_MII_CFG_5_CRS_SHIFT                           9
#define VR9_MII_CFG_5_CRS_SIZE                            2
/* Bit: 'RGMII_IBS' */
/* Description: 'RGMII In Band Status' */
#define VR9_MII_CFG_5_RGMII_IBS_OFFSET                    0x040
#define VR9_MII_CFG_5_RGMII_IBS_SHIFT                     8
#define VR9_MII_CFG_5_RGMII_IBS_SIZE                      1
/* Bit: 'MIIRATE' */
/* Description: 'xMII Port Interface Clock Rate' */
#define VR9_MII_CFG_5_MIIRATE_OFFSET                      0x040
#define VR9_MII_CFG_5_MIIRATE_SHIFT                       4
#define VR9_MII_CFG_5_MIIRATE_SIZE                        3
/* Bit: 'MIIMODE' */
/* Description: 'xMII Interface Mode' */
#define VR9_MII_CFG_5_MIIMODE_OFFSET                      0x040
#define VR9_MII_CFG_5_MIIMODE_SHIFT                       0
#define VR9_MII_CFG_5_MIIMODE_SIZE                        4
/* -------------------------------------------------------------------------- */
/* Register: 'Configuration of ClockDelay for External Port 5' */
/* Bit: 'RXLOCK' */
/* Description: 'Lock Status MDL of Receive PCDU' */
#define VR9_PCDU_5_RXLOCK_OFFSET                          0x041
#define VR9_PCDU_5_RXLOCK_SHIFT                           15
#define VR9_PCDU_5_RXLOCK_SIZE                            1
/* Bit: 'TXLOCK' */
/* Description: 'Lock Status of MDL of Transmit PCDU' */
#define VR9_PCDU_5_TXLOCK_OFFSET                          0x041
#define VR9_PCDU_5_TXLOCK_SHIFT                           14
#define VR9_PCDU_5_TXLOCK_SIZE                            1
/* Bit: 'RXDLY' */
/* Description: 'Configure Receive Clock Delay' */
#define VR9_PCDU_5_RXDLY_OFFSET                           0x041
#define VR9_PCDU_5_RXDLY_SHIFT                            7
#define VR9_PCDU_5_RXDLY_SIZE                             3
/* Bit: 'TXDLY' */
/* Description: 'Configure Transmit PCDU' */
#define VR9_PCDU_5_TXDLY_OFFSET                           0x041
#define VR9_PCDU_5_TXDLY_SHIFT                            0
#define VR9_PCDU_5_TXDLY_SIZE                             3
/* -------------------------------------------------------------------------- */
/* Register: 'Receive Buffer ControlRegister for Port 0' */
/* Bit: 'RBUF_UFL' */
/* Description: 'Receive Buffer Underflow Indicator' */
#define VR9_RXB_CTL_0_RBUF_UFL_OFFSET                     0x056
#define VR9_RXB_CTL_0_RBUF_UFL_SHIFT                      15
#define VR9_RXB_CTL_0_RBUF_UFL_SIZE                       1
/* Bit: 'RBUF_OFL' */
/* Description: 'Receive Buffer Overflow Indicator' */
#define VR9_RXB_CTL_0_RBUF_OFL_OFFSET                     0x056
#define VR9_RXB_CTL_0_RBUF_OFL_SHIFT                      14
#define VR9_RXB_CTL_0_RBUF_OFL_SIZE                       1
/* Bit: 'RBUF_DLY_WP' */
/* Description: 'Delay' */
#define VR9_RXB_CTL_0_RBUF_DLY_WP_OFFSET                  0x056
#define VR9_RXB_CTL_0_RBUF_DLY_WP_SHIFT                   0
#define VR9_RXB_CTL_0_RBUF_DLY_WP_SIZE                    3
/* -------------------------------------------------------------------------- */
/* Register: 'Receive Buffer ControlRegister External Port 1' */
/* Bit: 'RBUF_UFL' */
/* Description: 'Receive Buffer Underflow Indicator' */
#define VR9_RXB_CTL_1_RBUF_UFL_OFFSET                     0x057
#define VR9_RXB_CTL_1_RBUF_UFL_SHIFT                      15
#define VR9_RXB_CTL_1_RBUF_UFL_SIZE                       1
/* Bit: 'RBUF_OFL' */
/* Description: 'Receive Buffer Overflow Indicator' */
#define VR9_RXB_CTL_1_RBUF_OFL_OFFSET                     0x057
#define VR9_RXB_CTL_1_RBUF_OFL_SHIFT                      14
#define VR9_RXB_CTL_1_RBUF_OFL_SIZE                       1
/* Bit: 'RBUF_DLY_WP' */
/* Description: 'Delay' */
#define VR9_RXB_CTL_1_RBUF_DLY_WP_OFFSET                  0x057
#define VR9_RXB_CTL_1_RBUF_DLY_WP_SHIFT                   0
#define VR9_RXB_CTL_1_RBUF_DLY_WP_SIZE                    3
/* -------------------------------------------------------------------------- */
/* Register: 'Receive Buffer ControlRegister External Port 5' */
/* Bit: 'RBUF_UFL' */
/* Description: 'Receive Buffer Underflow Indicator' */
#define VR9_RXB_CTL_5_RBUF_UFL_OFFSET                     0x05B
#define VR9_RXB_CTL_5_RBUF_UFL_SHIFT                      15
#define VR9_RXB_CTL_5_RBUF_UFL_SIZE                       1
/* Bit: 'RBUF_OFL' */
/* Description: 'Receive Buffer Overflow Indicator' */
#define VR9_RXB_CTL_5_RBUF_OFL_OFFSET                     0x05B
#define VR9_RXB_CTL_5_RBUF_OFL_SHIFT                      14
#define VR9_RXB_CTL_5_RBUF_OFL_SIZE                       1
/* Bit: 'RBUF_DLY_WP' */
/* Description: 'Delay' */
#define VR9_RXB_CTL_5_RBUF_DLY_WP_OFFSET                  0x05B
#define VR9_RXB_CTL_5_RBUF_DLY_WP_SHIFT                   0
#define VR9_RXB_CTL_5_RBUF_DLY_WP_SIZE                    3
/* -------------------------------------------------------------------------- */
/* Register: 'Debug Control Register' */
/* Bit: 'DBG_EN' */
/* Description: 'Debug enable' */
#define VR9_DBG_CTL_DBG_EN_OFFSET                         0x081
#define VR9_DBG_CTL_DBG_EN_SHIFT                          14
#define VR9_DBG_CTL_DBG_EN_SIZE                           1
/* Bit: 'DBG_SEL' */
/* Description: 'Debug select' */
#define VR9_DBG_CTL_DBG_SEL_OFFSET                        0x081
#define VR9_DBG_CTL_DBG_SEL_SHIFT                         0
#define VR9_DBG_CTL_DBG_SEL_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Header ControlRegister' */
/* Bit: 'FC' */
/* Description: 'Enable Flow Control' */
#define VR9_PMAC_HD_CTL_FC_OFFSET                         0x082
#define VR9_PMAC_HD_CTL_FC_SHIFT                          10
#define VR9_PMAC_HD_CTL_FC_SIZE                           1
/* Bit: 'CCRC' */
/* Description: 'Check CRC' */
#define VR9_PMAC_HD_CTL_CCRC_OFFSET                       0x082
#define VR9_PMAC_HD_CTL_CCRC_SHIFT                        9
#define VR9_PMAC_HD_CTL_CCRC_SIZE                         1
/* Bit: 'RST' */
/* Description: 'Remove Special Tag' */
#define VR9_PMAC_HD_CTL_RST_OFFSET                        0x082
#define VR9_PMAC_HD_CTL_RST_SHIFT                         8
#define VR9_PMAC_HD_CTL_RST_SIZE                          1
/* Bit: 'AST' */
/* Description: 'Add Special Tag' */
#define VR9_PMAC_HD_CTL_AST_OFFSET                        0x082
#define VR9_PMAC_HD_CTL_AST_SHIFT                         7
#define VR9_PMAC_HD_CTL_AST_SIZE                          1
/* Bit: 'RXSH' */
/* Description: 'Status Header' */
#define VR9_PMAC_HD_CTL_RXSH_OFFSET                       0x082
#define VR9_PMAC_HD_CTL_RXSH_SHIFT                        6
#define VR9_PMAC_HD_CTL_RXSH_SIZE                         1
/* Bit: 'RL2' */
/* Description: 'Remove Layer-2 Header' */
#define VR9_PMAC_HD_CTL_RL2_OFFSET                        0x082
#define VR9_PMAC_HD_CTL_RL2_SHIFT                         5
#define VR9_PMAC_HD_CTL_RL2_SIZE                          1
/* Bit: 'RC' */
/* Description: 'Remove CRC' */
#define VR9_PMAC_HD_CTL_RC_OFFSET                         0x082
#define VR9_PMAC_HD_CTL_RC_SHIFT                          4
#define VR9_PMAC_HD_CTL_RC_SIZE                           1
/* Bit: 'AS' */
/* Description: 'Add Status Header' */
#define VR9_PMAC_HD_CTL_AS_OFFSET                         0x082
#define VR9_PMAC_HD_CTL_AS_SHIFT                          3
#define VR9_PMAC_HD_CTL_AS_SIZE                           1
/* Bit: 'AC' */
/* Description: 'Add CRC' */
#define VR9_PMAC_HD_CTL_AC_OFFSET                         0x082
#define VR9_PMAC_HD_CTL_AC_SHIFT                          2
#define VR9_PMAC_HD_CTL_AC_SIZE                           1
/* Bit: 'TAG' */
/* Description: 'Add TAG' */
#define VR9_PMAC_HD_CTL_TAG_OFFSET                        0x082
#define VR9_PMAC_HD_CTL_TAG_SHIFT                         1
#define VR9_PMAC_HD_CTL_TAG_SIZE                          1
/* Bit: 'ADD' */
/* Description: 'ADD Header' */
#define VR9_PMAC_HD_CTL_ADD_OFFSET                        0x082
#define VR9_PMAC_HD_CTL_ADD_SHIFT                         0
#define VR9_PMAC_HD_CTL_ADD_SIZE                          1
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Type/Length Register' */
/* Bit: 'TYPE_LEN' */
/* Description: 'TYPE or Lenght Value' */
#define VR9_PMAC_TL_TYPE_LEN_OFFSET                       0x083
#define VR9_PMAC_TL_TYPE_LEN_SHIFT                        0
#define VR9_PMAC_TL_TYPE_LEN_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Source Address Register1' */
/* Bit: 'SA_47_32' */
/* Description: 'Source Address 47..32' */
#define VR9_PMAC_SA1_SA_47_32_OFFSET                      0x084
#define VR9_PMAC_SA1_SA_47_32_SHIFT                       0
#define VR9_PMAC_SA1_SA_47_32_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Source Address Register2' */
/* Bit: 'SA_31_16' */
/* Description: 'Source Address 31..16' */
#define VR9_PMAC_SA2_SA_31_16_OFFSET                      0x085
#define VR9_PMAC_SA2_SA_31_16_SHIFT                       0
#define VR9_PMAC_SA2_SA_31_16_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Source Address Register3' */
/* Bit: 'SA_15_0' */
/* Description: 'Source Address 15..0' */
#define VR9_PMAC_SA3_SA_15_0_OFFSET                       0x086
#define VR9_PMAC_SA3_SA_15_0_SHIFT                        0
#define VR9_PMAC_SA3_SA_15_0_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Destination AddressRegister 1' */
/* Bit: 'SA_47_32' */
/* Description: 'Destination Address 47..32' */
#define VR9_PMAC_DA1_SA_47_32_OFFSET                      0x087
#define VR9_PMAC_DA1_SA_47_32_SHIFT                       0
#define VR9_PMAC_DA1_SA_47_32_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Destination AddressRegister 2' */
/* Bit: 'DA_31_16' */
/* Description: 'Destination Address 31..16' */
#define VR9_PMAC_DA2_DA_31_16_OFFSET                      0x088
#define VR9_PMAC_DA2_DA_31_16_SHIFT                       0
#define VR9_PMAC_DA2_DA_31_16_SIZE                        16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Destination AddressRegister 3' */
/* Bit: 'DA_15_0' */
/* Description: 'Destination Address 15..0' */
#define VR9_PMAC_DA3_DA_15_0_OFFSET                       0x089
#define VR9_PMAC_DA3_DA_15_0_SHIFT                        0
#define VR9_PMAC_DA3_DA_15_0_SIZE                         16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC VLAN Register' */
/* Bit: 'PRI' */
/* Description: 'VLAN Priority' */
#define VR9_PMAC_VLAN_PRI_OFFSET                          0x08A
#define VR9_PMAC_VLAN_PRI_SHIFT                           13
#define VR9_PMAC_VLAN_PRI_SIZE                            3
/* Bit: 'CFI' */
/* Description: 'Canonical Format Identifier' */
#define VR9_PMAC_VLAN_CFI_OFFSET                          0x08A
#define VR9_PMAC_VLAN_CFI_SHIFT                           12
#define VR9_PMAC_VLAN_CFI_SIZE                            1
/* Bit: 'VLAN_ID' */
/* Description: 'VLAN ID' */
#define VR9_PMAC_VLAN_VLAN_ID_OFFSET                      0x08A
#define VR9_PMAC_VLAN_VLAN_ID_SHIFT                       0
#define VR9_PMAC_VLAN_VLAN_ID_SIZE                        12
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Inter Packet Gapin RX Direction' */
/* Bit: 'REQ_DS_THRES' */
/* Description: 'Request Deassertion Threshold' */
#define VR9_PMAC_RX_IPG_REQ_DS_THRES_OFFSET               0x08B
#define VR9_PMAC_RX_IPG_REQ_DS_THRES_SHIFT                8
#define VR9_PMAC_RX_IPG_REQ_DS_THRES_SIZE                 1
/* Bit: 'REQ_AS_THRES' */
/* Description: 'Request Assertion Threshold' */
#define VR9_PMAC_RX_IPG_REQ_AS_THRES_OFFSET               0x08B
#define VR9_PMAC_RX_IPG_REQ_AS_THRES_SHIFT                4
#define VR9_PMAC_RX_IPG_REQ_AS_THRES_SIZE                 4
/* Bit: 'IPG_CNT' */
/* Description: 'IPG Counter' */
#define VR9_PMAC_RX_IPG_IPG_CNT_OFFSET                    0x08B
#define VR9_PMAC_RX_IPG_IPG_CNT_SHIFT                     0
#define VR9_PMAC_RX_IPG_IPG_CNT_SIZE                      4
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Special Tag Ethertype' */
/* Bit: 'ST_ETYPE' */
/* Description: 'Special Tag Ethertype' */
#define VR9_PMAC_ST_ETYPE_ST_ETYPE_OFFSET                 0x08C
#define VR9_PMAC_ST_ETYPE_ST_ETYPE_SHIFT                  0
#define VR9_PMAC_ST_ETYPE_ST_ETYPE_SIZE                   16
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Ethernet WAN Group' */
/* Bit: 'EWAN' */
/* Description: 'Ethernet WAN Group' */
#define VR9_PMAC_EWAN_EWAN_OFFSET                         0x08D
#define VR9_PMAC_EWAN_EWAN_SHIFT                          0
#define VR9_PMAC_EWAN_EWAN_SIZE                           6
/* -------------------------------------------------------------------------- */
/* Register: 'PMAC Control Register' */
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define VR9_PMAC_CTL_SPEED_OFFSET                         0x08E
#define VR9_PMAC_CTL_SPEED_SHIFT                          1
#define VR9_PMAC_CTL_SPEED_SIZE                           1
/* Bit: 'EN' */
/* Description: 'PMAC Enable' */
#define VR9_PMAC_CTL_EN_OFFSET                            0x08E
#define VR9_PMAC_CTL_EN_SHIFT                             0
#define VR9_PMAC_CTL_EN_SIZE                              1
/* -------------------------------------------------------------------------- */
#endif /* #ifndef _VR9_TOP_H */
