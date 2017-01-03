/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************

    \file ifxmips_gphy_sw.h
   \remarks implement GPHY driver on VR9 platform
 *****************************************************************************/

#ifndef _IFXMIPS_GPHY_SW_H_
#define _IFXMIPS_GPHY_SW_H_

#include <asm/ifx/ifx_rcu.h>
#include <asm/ifx/ifx_pmu.h>
#include <asm/ifx/ifx_gpio.h>

/**  default board related configuration  */
#if defined(CONFIG_MII0_PORT_ENABLED)
#define CONFIG_MAC0                         1
#else
#define CONFIG_MAC0                         0
#endif

#if defined(CONFIG_MII1_PORT_ENABLED)
#define CONFIG_MAC1                         1
#else
#define CONFIG_MAC1                         0
#endif

#if defined(CONFIG_MII2_PORT_ENABLED)
#define CONFIG_MAC2                         1
#else
#define CONFIG_MAC2                         0
#endif

#if defined(CONFIG_MII3_PORT_ENABLED)
#define CONFIG_MAC3                         1
#else
#define CONFIG_MAC3                         0
#endif

#if defined(CONFIG_MII4_PORT_ENABLED)
#define CONFIG_MAC4                         1
#else
#define CONFIG_MAC4                         0
#endif

#if defined(CONFIG_MII5_PORT_ENABLED)
#define CONFIG_MAC5                         1
#else
#define CONFIG_MAC5                         0
#endif

#if defined(CONFIG_MII0_RGMII_MAC_MODE)
#define MII0_MODE_SETUP                 RGMII_MODE
#elif defined(CONFIG_MII0_RMII_MAC_MODE)
#define MII0_MODE_SETUP                 RMII_MAC_MODE
#elif defined(CONFIG_MII0_RMII_PHY_MODE)
#define MII0_MODE_SETUP                 RMII_PHY_MODE
#elif defined(CONFIG_MII0_MII_MAC_MODE)
#define MII0_MODE_SETUP                 MII_MAC_MODE
#elif defined(CONFIG_MII0_MII_PHY_MODE)
#define MII0_MODE_SETUP                 MII_PHY_MODE
#endif

#if defined(CONFIG_MII1_RGMII_MAC_MODE)
#define MII1_MODE_SETUP                 RGMII_MODE
#elif defined(CONFIG_MII1_RMII_MAC_MODE)
#define MII1_MODE_SETUP                 RMII_MAC_MODE
#elif defined(CONFIG_MII1_RMII_PHY_MODE)
#define MII1_MODE_SETUP                 RMII_PHY_MODE
#elif defined(CONFIG_MII1_MII_MAC_MODE)
#define MII1_MODE_SETUP                 MII_MAC_MODE
#elif defined(CONFIG_MII1_MII_PHY_MODE)
#define MII1_MODE_SETUP                 MII_PHY_MODE
#endif

#if defined(CONFIG_MII2_GMII_MODE)
#define MII2_MODE_SETUP                 GMII_MAC_MODE
#elif defined(CONFIG_MII2_MII_MAC_MODE)
#define MII2_MODE_SETUP                 MII_MAC_MODE
#endif

#if defined(CONFIG_MII3_MII_MAC_MODE)
#define MII3_MODE_SETUP                 MII_MAC_MODE
#endif

#if defined(CONFIG_MII4_GMII_MODE)
#define MII4_MODE_SETUP                 GMII_MAC_MODE
#elif defined(CONFIG_MII4_MII_MAC_MODE)
#define MII4_MODE_SETUP                 MII_MAC_MODE
#endif

#if defined(CONFIG_MII5_RGMII_MODE)
#define MII5_MODE_SETUP                 RGMII_MODE
#elif defined(CONFIG_MII5_MII_MAC_MODE)
#define MII5_MODE_SETUP                 MII_PHY_MODE
#endif


#ifdef CONFIG_SW_ROUTING_MODE
    #define CONFIG_PMAC_DMA_ENABLE          1   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          1   /*g_dma_pmac*/
#else
    #define CONFIG_PMAC_DMA_ENABLE          0   /*g_pmac_dma */
    #define CONFIG_DMA_PMAC_ENABLE          0   /*g_dma_pmac*/
#endif

#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    #define NUM_ETH_INF                     2
#else
    #define NUM_ETH_INF                     1
#endif


#define GPHY_FW_LEN                         65536
static const char gphy_fw_data[GPHY_FW_LEN];

#endif /*_IFXMIPS_GPHY_SW_H_  */
