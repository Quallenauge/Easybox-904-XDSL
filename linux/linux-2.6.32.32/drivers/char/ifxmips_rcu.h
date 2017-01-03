/******************************************************************************
**
** FILE NAME    : ifxmips_rcu.h
** PROJECT      : UEIP
** MODULES      : RCU (Reset Control Unit)
**
** DATE         : 17 Jun 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : RCU driver header file
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
** 17 JUN 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFXMIPS_RCU_H
#define IFXMIPS_RCU_H



#include <asm/ifx/ifx_rcu.h>



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

#define IFX_RCU_MAJOR                   241

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

typedef struct __ifx_rcu_handler {
    struct __ifx_rcu_handler   *next;
    ifx_rcu_callbackfn          fn;
    unsigned long               arg;
    unsigned int                module_id;
} ifx_rcu_handler_t;

typedef struct {
    unsigned int                affected_domains;   //  domains affected by reset to this domain (inclusive)
    unsigned int                rst_req_value;      //  value write to RST_REQ register
    unsigned int                rst_req_mask;       //  mask of RST_REQ register
    unsigned int                rst_stat_mask;      //  mask of RST_STAT register
    unsigned int                latch;
    unsigned int                udelay;
    ifx_rcu_handler_t          *handlers;
} ifx_rcu_domain_t;



/*
 * ####################################
 *        Platform Header File
 * ####################################
 */

#if defined(CONFIG_DANUBE)
  #include "ifxmips_rcu_danube.h"
#elif defined(CONFIG_AMAZON_SE)
  #include "ifxmips_rcu_amazon_se.h"
#elif defined(CONFIG_AR9)
  #include "ifxmips_rcu_ar9.h"
#elif defined(CONFIG_VR9)
  #include "ifxmips_rcu_vr9.h"
#elif defined(CONFIG_AR10)
  #include "ifxmips_rcu_ar10.h"
#else
  #error Platform is not specified!
#endif



#endif  //  IFXMIPS_RCU_H
