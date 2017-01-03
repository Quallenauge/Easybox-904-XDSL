/******************************************************************************
**
** FILE NAME    : ifxmips_ledc.h
** PROJECT      : UEIP
** MODULES      : LED Controller (Serial Out)
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : LED Controller driver header file
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 16 JUL 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFXMIPS_LEDC_H
#define IFXMIPS_LEDC_H



#include <asm/ifx/ifx_ledc.h>



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  Compile Options
 */

#define ENABLE_DEBUG                    1

#define ENABLE_ASSERT                   1

#define INLINE

/*
 *  Constant
 */

#define IFX_LEDC_MAJOR                  242

#define IFX_LEDC_MAX_LED                24

/*
 *  Debug/Assert/Error Message
 */

#define DBG_ENABLE_MASK_ERR             (1 << 0)
#define DBG_ENABLE_MASK_DEBUG_PRINT     (1 << 1)
#define DBG_ENABLE_MASK_ASSERT          (1 << 2)
#define DBG_ENABLE_MASK_ALL             (DBG_ENABLE_MASK_ERR | DBG_ENABLE_MASK_DEBUG_PRINT | DBG_ENABLE_MASK_ASSERT)

#define err(format, arg...)             do { if ( (g_dbg_enable & DBG_ENABLE_MASK_ERR) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
  #undef  dbg
  #define dbg(format, arg...)           do { if ( (g_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT) ) printk(KERN_WARNING __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #if !defined(dbg)
    #define dbg(format, arg...)
  #endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
  #define ASSERT(cond, format, arg...)  do { if ( (g_dbg_enable & DBG_ENABLE_MASK_ASSERT) && !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
  #define ASSERT(cond, format, arg...)
#endif

/*
 *  Register Definition
 */

/* LED Registers Mapping */
#ifndef IFX_LED
  #define IFX_LED                       (KSEG1 | 0x1E100BB0)
  #define IFX_LED_CON0                  ((volatile unsigned int *)(IFX_LED + 0x0000))
  #define IFX_LED_CON1                  ((volatile unsigned int *)(IFX_LED + 0x0004))
  #define IFX_LED_CPU0                  ((volatile unsigned int *)(IFX_LED + 0x0008))
  #define IFX_LED_CPU1                  ((volatile unsigned int *)(IFX_LED + 0x000C))
  #define IFX_LED_AR                    ((volatile unsigned int *)(IFX_LED + 0x0010))
#endif

/* LED Control 0 Register */
#define LED_CON0_SW_UPDATE              (IFX_REG_R32(IFX_LED_CON0) & (1 << 31))
#define LED_CON0_SW_UPDATE_SET(dummy)   IFX_REG_W32_MASK(1 << 31, 1 << 31, IFX_LED_CON0)
#define LED_CON0_FALLING_EDGE           (IFX_REG_R32(IFX_LED_CON0) & (1 << 26))
#define LED_CON0_FALLING_EDGE_SET(x)    IFX_REG_W32_MASK(1 << 26, (x) ? (1 << 26) : 0, IFX_LED_CON0)
#define LED_CON0_LEDn_BLINK(n)          (IFX_REG_R32(IFX_LED_CON0) & (1 << (n)))
#define LED_CON0_LEDn_BLINK_SET(n, x)   IFX_REG_W32_MASK(1 << (n), (x) ? (1 << (n)) : 0, IFX_LED_CON0)

/* LED Control 1 Register */
#define LED_CON1_UPDATE_SRC             (IFX_REG_R32(IFX_LED_CON1) >> 30)
#define LED_CON1_UPDATE_SRC_SET(x)      IFX_REG_W32_MASK(3 << 30, (x) << 30, IFX_LED_CON1)
#define LED_CON1_CLOCK_STORE            (IFX_REG_R32(IFX_LED_CON1) & (1 << 28))
#define LED_CON1_CLOCK_STORE_SET(x)     IFX_REG_W32_MASK(1 << 28, (x) ? (1 << 28) : 0, IFX_LED_CON1)
#define LED_CON1_FPID                   GET_BITS(IFX_REG_R32(IFX_LED_CON1), 27, 23)
#define LED_CON1_FPID_SET(x)            IFX_REG_W32_MASK(0x1F << 23, (x) << 23, IFX_LED_CON1)
#define LED_CON1_FPIS                   GET_BITS(IFX_REG_R32(IFX_LED_CON1), 21, 20)
#define LED_CON1_FPIS_SET(x)            IFX_REG_W32_MASK(3 << 20, (x) << 20, IFX_LED_CON1)
#define LED_CON1_DATA_OFFSET            GET_BITS(IFX_REG_R32(IFX_LED_CON1), 19, 18)
#define LED_CON1_DATA_OFFSET_SET(x)     IFX_REG_W32_MASK(3 << 18, (x) << 18, IFX_LED_CON1)
#define LED_CON1_GROUP                  (IFX_REG_R32(IFX_LED_CON1) & 0x07)
#define LED_CON1_GROUP_SET(x)           IFX_REG_W32_MASK(7, x, IFX_LED_CON1)

/* LED Data Output CPU 0 Register */
#define LED_CPU0_LEDn_ON(n)             (IFX_LED_CON1(IFX_LED_CPU0) & (1 << n))
#define LED_CPU0_LEDn_ON_SET(n, x)      IFX_REG_W32_MASK(1 << (n), (x) ? (1 << (n)) : 0, IFX_LED_CPU0)

/* LED Data Output CPU 1 Register */
#define LED_CPU1_LEDn_ON(n)             (IFX_LED_CON1(IFX_LED_CPU1) & (1 << n))

/* LED Data Output Access Rights Register */
#define LED_MIPS1_ACCESS_LEDn(n)        (IFX_LED_CON1(IFX_LED_AR) & (1 << n))
#define LED_MIPS1_ACCESS_LEDn_SET(n, x) IFX_REG_W32_MASK(1 << (n), (x) ? (1 << (n)) : 0, IFX_LED_AR)

/* This field (EXT_SRC) is quite special, across two registers and not identical in different platform. */
#if defined(CONFIG_DANUBE) || defined(CONFIG_AR9)
  #define LED_CON0_EXT_SRC_MAX          2
  #define LED_CON0_EXT_SRC_CON1         LED_CON0_EXT_SRC_MAX
  #define LED_CON0_EXT_SRC_CON1_SHIFT   0
#elif defined(CONFIG_AMAZON_SE)
  #define LED_CON0_EXT_SRC_MAX          6
  #define LED_CON0_EXT_SRC_CON1         LED_CON0_EXT_SRC_MAX
  #define LED_CON0_EXT_SRC_CON1_SHIFT   0
#elif defined(CONFIG_VR9)
  #define LED_CON0_EXT_SRC_MAX          8
  #define LED_CON0_EXT_SRC_CON1         5
  #define LED_CON0_EXT_SRC_CON1_SHIFT   15
#elif defined(CONFIG_AR10)
  #define LED_CON0_EXT_SRC_MAX          17
#endif
static inline int LED_CON_EXT_SRCn_map(unsigned int n, volatile unsigned int **ppreg, unsigned int *bit)
{
    /*
     *   0 - DSL LED 0
     *   1 - DSL LED 1
     *   2 - Amazon-SE: Ephy Link/Act LED,  VR9/AR10: GPHY1 LED0
     *   3 - Amazon-SE: Ephy Speed LED,     VR9/AR10: GPHY1 LED1
     *   4 - Amazon-SE: Ephy Collision LED, VR9/AR10: GPHY1 LED2
     *   5 - Amazon-SE: Ephy Duplex LED,    VR9/AR10: GPHY0 LED0
     *   6 - VR9/AR10: GPHY0 LED1
     *   7 - VR9/AR10: GPHY0 LED2
     *   8 - AR10: GPHY2 LED0
     *   9 - AR10: GPHY2 LED1
     *  10 - AR10: GPHY2 LED2
     *  11 - AR10: WLAN Link Quality Single Band LED
     *  12 - AR10: WLAN Link Quality Dual Band LED
     *  13 - AR10: WLAN Link/Activity Single Band LED
     *  14 - AR10: WLAN Link/Activity Dual Band LED
     *  15 - AR10: WLAN Security LED0 (Dual Color)
     *  16 - AR10: WLAN Security LED1 (Dual Color)
     */
#if defined(CONFIG_DANUBE) || defined(CONFIG_AMAZON_SE) || defined(CONFIG_AR9) || defined(CONFIG_VR9)
    if ( n < LED_CON0_EXT_SRC_MAX ) {
        if ( n < LED_CON0_EXT_SRC_CON1 ) {
            *ppreg = IFX_LED_CON0;
            *bit = n < 2 ? (24 + n) : (25 + n);
        }
        else {
            *ppreg = IFX_LED_CON1;
            *bit = (n - LED_CON0_EXT_SRC_CON1) + LED_CON0_EXT_SRC_CON1_SHIFT;
        }
        return 0;
    }
#elif defined(CONFIG_AR10)
    volatile unsigned int *preg_map[LED_CON0_EXT_SRC_MAX] = {
        IFX_LED_CON0,   //  0 - DSL LED 0
        IFX_LED_CON0,   //  1 - DSL LED 1
        IFX_LED_CON1,   //  2 - GPHY1 LED0
        IFX_LED_CON1,   //  3 - GPHY1 LED1
        IFX_LED_CON1,   //  4 - GPHY1 LED2
        IFX_LED_CON0,   //  5 - GPHY0 LED0
        IFX_LED_CON0,   //  6 - GPHY0 LED1
        IFX_LED_CON0,   //  7 - GPHY0 LED2
        IFX_LED_CON1,   //  8 - GPHY2 LED0
        IFX_LED_CON1,   //  9 - GPHY2 LED1
        IFX_LED_CON1,   // 10 - GPHY2 LED2
        IFX_LED_CON1,   // 11 - WLAN Link Quality Single Band LED
        IFX_LED_CON1,   // 12 - WLAN Link Quality Dual Band LED
        IFX_LED_CON1,   // 13 - WLAN Link/Activity Single Band LED
        IFX_LED_CON1,   // 14 - WLAN Link/Activity Dual Band LED
        IFX_LED_CON1,   // 15 - WLAN Security LED0 (Dual Color)
        IFX_LED_CON1,   // 16 - WLAN Security LED1 (Dual Color)
    };
    unsigned int bit_map[LED_CON0_EXT_SRC_MAX] = {
        24, //  0 - DSL LED 0
        25, //  1 - DSL LED 1
        3,  //  2 - GPHY1 LED0
        4,  //  3 - GPHY1 LED1
        5,  //  4 - GPHY1 LED2
        27, //  5 - GPHY0 LED0
        28, //  6 - GPHY0 LED1
        29, //  7 - GPHY0 LED2
        6,  //  8 - GPHY2 LED0
        7,  //  9 - GPHY2 LED1
        8,  // 10 - GPHY2 LED2
        9,  // 11 - WLAN Link Quality Single Band LED
        10, // 12 - WLAN Link Quality Dual Band LED
        11, // 13 - WLAN Link/Activity Single Band LED
        12, // 14 - WLAN Link/Activity Dual Band LED
        13, // 15 - WLAN Security LED0 (Dual Color)
        14, // 16 - WLAN Security LED1 (Dual Color)
    };

    if ( n < LED_CON0_EXT_SRC_MAX ) {
        *ppreg = preg_map[n];
        *bit = bit_map[n];
        return 0;
    }
#endif

    return -1;  //  error
}
static inline unsigned int LED_CON0_EXT_SRCn(unsigned int n)    //  n range: Danube - 0~1, Amazon-SE - 0~5, AR9 - 0~1, VR9 - 0~7, AR10 - 0~16
{
    volatile unsigned int *preg;
    unsigned int bit;

    if ( LED_CON_EXT_SRCn_map(n, &preg, &bit) != 0 )
        return 0;   //  invalid input

    return (IFX_REG_R32(preg) >> bit) & 0x01;
}
static inline void LED_CON0_EXT_SRCn_SET(unsigned int n, unsigned int value)
{
    volatile unsigned int *preg;
    unsigned int bit;
    unsigned mask;

    if ( LED_CON_EXT_SRCn_map(n, &preg, &bit) == 0 ) {
        mask = 1 << bit;
        IFX_REG_W32_MASK(mask, value ? mask : 0, preg);
    }
}



/*
 * ####################################
 *              Data Type
 * ####################################
 */



#endif  //  IFXMIPS_LEDC_H
