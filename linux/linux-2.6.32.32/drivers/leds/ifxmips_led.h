/******************************************************************************
**
** FILE NAME    : ifxmips_led.h
** PROJECT      : UEIP
** MODULES      : LED Driver
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : LED driver header file
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

#ifndef IFXMIPS_LED_H
#define IFXMIPS_LED_H



#include <asm/ifx/ifx_led.h>



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
 *  Debug/Assert/Error Message
 */

#define DBG_ENABLE_MASK_ERR             (1 << 0)
#define DBG_ENABLE_MASK_DEBUG_PRINT     (1 << 1)
#define DBG_ENABLE_MASK_ASSERT          (1 << 2)

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
 * ####################################
 *              Data Type
 * ####################################
 */

struct ifx_led_trigger {
    struct led_trigger  trigger;

    struct list_head    next_trig;

    unsigned int        delay_on;
    unsigned int        delay_off;
    unsigned int        timeout;
    unsigned int        def_value;

    char                name[1];
};

struct ifx_led_trigger_data {
    unsigned int        delay_on;
    unsigned int        delay_off;
    struct timer_list   blink_timer;

    unsigned int        timeout;
    unsigned int        def_value;
    struct timer_list   timeout_timer;
    unsigned long       jiffies;

    unsigned int        org_brightness;
};



/*
 * ####################################
 *        Platform Header File
 * ####################################
 */

//#if defined(CONFIG_DANUBE)
//  #include "ifxmips_ledc_danube.h"
//#elif defined(CONFIG_AMAZON_SE)
//  #include "ifxmips_ledc_amazon_se.h"
//#elif defined(CONFIG_AR9)
//  #include "ifxmips_ledc_ar9.h"
//#elif defined(CONFIG_VR9)
//  #include "ifxmips_ledc_vr9.h"
//#else
//  #error Platform is not specified!
//#endif



#endif  //  IFXMIPS_LED_H
