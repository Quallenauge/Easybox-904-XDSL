/******************************************************************************
**
** FILE NAME    : ifxmips_ebu_led.h
** PROJECT      : UEIP
** MODULES      : LED Controller (Serial Out)
**
** DATE         : 16 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : EBU LED Controller driver header file
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

#ifndef IFXMIPS_EBU_LED_H
#define IFXMIPS_EBU_LED_H



#include <asm/ifx/ifx_ebu_led.h>



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
 *  Register Definition
 */
#define EBU_CS1_BASE_ADDR               IFX_EBU_ADDSEL1_BASE
#define EBU_LED_BASE_ADDR               ((volatile unsigned int *)(EBU_CS1_BASE_ADDR | 0x00400000))



/*
 * ####################################
 *              Data Type
 * ####################################
 */



#endif  //  IFXMIPS_EBU_LED_H
