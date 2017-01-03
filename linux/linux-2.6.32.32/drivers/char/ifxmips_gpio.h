/******************************************************************************
**
** FILE NAME    : ifxmips_gpio.h
** PROJECT      : IFX UEIP
** MODULES      : GPIO
**
** DATE         : 21 Jun 2004
** AUTHOR       : btxu
** DESCRIPTION  : IFX GPIO driver header file
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
** 21 Jun 2004   btxu            Generate from INCA-IP project
** 22 May 2009   Xu Liang        UEIP
*********************************************************************/

#ifndef IFXMIPS_GPIO_H
#define IFXMIPS_GPIO_H

#include <asm/ifx/ifx_gpio.h>

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

/*
 *  Constant
 */

#define IFX_GPIO_MAJOR                  240

#define GPIO_PIN_STATUS_APP_MASK        0x80000000
#define GPIN_PIN_STATUS_RES_MASK        0xFF000000
#define GPIO_PIN_STATUS_MODULE_ID_MASK  (~GPIN_PIN_STATUS_RES_MASK)

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
 * ####################################
 *              Data Type
 * ####################################
 */

typedef struct {
    volatile unsigned long *gpio_out;
    volatile unsigned long *gpio_in;
    volatile unsigned long *gpio_dir;
    volatile unsigned long *gpio_altsel0;
    volatile unsigned long *gpio_altsel1;
    volatile unsigned long *gpio_od;
    volatile unsigned long *gpio_stoff;
    volatile unsigned long *gpio_pudsel;
    volatile unsigned long *gpio_puden;
} ifx_gpio_reg_t;

typedef struct {
    ifx_gpio_reg_t          reg;
    int                     pin_num;
    unsigned int            pin_status[IFX_GPIO_PIN_NUMBER_PER_PORT];
} ifx_gpio_port_t;

/*
 * ####################################
 *        Platform Header File
 * ####################################
 */

#if defined(CONFIG_DANUBE)
#include "ifxmips_gpio_danube.h"
#elif defined(CONFIG_AMAZON_SE)
#include "ifxmips_gpio_amazon_se.h"
#elif defined(CONFIG_AR9)
#include "ifxmips_gpio_ar9.h"
#elif defined(CONFIG_VR9)
#include "ifxmips_gpio_vr9.h"
#elif defined(CONFIG_AR10)
#include "ifxmips_gpio_ar10.h"
#else
#error Platform is not specified!
#endif

#endif  /* IFXMIPS_GPIO_H */
