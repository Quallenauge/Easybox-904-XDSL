/******************************************************************************
**
** FILE NAME    : ifxmips_si.h
** PROJECT      : UEIP
** MODULES      : Serial In Controller
**
** DATE         : 26 Apr 2010
** AUTHOR       : Xu Liang
** DESCRIPTION  : Serial In Controller driver header file
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
** $Date          $Author         $Comment
** Apr 26, 2010   Xu Liang        Init Version
*******************************************************************************/



#ifndef _IFXMIPS_SI_H_
#define _IFXMIPS_SI_H_



#include <asm/ifx/ifx_si.h>



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

#define IFX_SI_MAJOR                    0

#define IFX_SI_MAX_PIN                  16

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

/*  SI Control Register */
#define CON_MASK(bits, shift)           (((1 << (bits)) - 1) << (shift))
#define CON_VAL(val, bits, shift)       (((val) << (shift)) & CON_MASK(bits, shift))
#define CON_US_BITS                     2
#define CON_US_SHIFT                    30
#define CON_US_MASK                     CON_MASK(CON_US_BITS, CON_US_SHIFT)
#define CON_US_VAL(val)                 CON_VAL(val, CON_US_BITS, CON_US_SHIFT)
#define CON_SW_UPDATE_BITS              1
#define CON_SW_UPDATE_SHIFT             29
#define CON_SW_UPDATE_MASK              CON_MASK(CON_SW_UPDATE_BITS, CON_SW_UPDATE_SHIFT)
#define CON_SW_UPDATE_VAL(val)          CON_VAL(val, CON_SW_UPDATE_BITS, CON_SW_UPDATE_SHIFT)
#define CON_SYNC_BITS                   1
#define CON_SYNC_SHIFT                  28
#define CON_SYNC_MASK                   CON_MASK(CON_SYNC_BITS, CON_SYNC_SHIFT)
#define CON_SYNC_VAL(val)               CON_VAL(val, CON_SYNC_BITS, CON_SYNC_SHIFT)
#define CON_SAMPLING_CLK_BITS           2
#define CON_SAMPLING_CLK_SHIFT          25
#define CON_SAMPLING_CLK_MASK           CON_MASK(CON_SAMPLING_CLK_BITS, CON_SAMPLING_CLK_SHIFT)
#define CON_SAMPLING_CLK_VAL(val)       CON_VAL(val, CON_SAMPLING_CLK_BITS, CON_SAMPLING_CLK_SHIFT)
#define CON_GROUP_BITS                  2
#define CON_GROUP_SHIFT                 23
#define CON_GROUP_MASK                  CON_MASK(CON_GROUP_BITS, CON_GROUP_SHIFT)
#define CON_GROUP_VAL(val)              CON_VAL(val, CON_GROUP_BITS, CON_GROUP_SHIFT)
#define CON_SHIFT_CLK_BITS              2
#define CON_SHIFT_CLK_SHIFT             0
#define CON_SHIFT_CLK_MASK              CON_MASK(CON_SHIFT_CLK_BITS, CON_SHIFT_CLK_SHIFT)
#define CON_SHIFT_CLK_VAL(val)          CON_VAL(val, CON_SHIFT_CLK_BITS, CON_SHIFT_CLK_SHIFT)



/*
 * ####################################
 *              Data Type
 * ####################################
 */

typedef struct {
    volatile unsigned int  *cpu;        //  serial in data register
    volatile unsigned int  *con;        //  serial in controller register
    volatile unsigned int  *int_con;    //  interrupt control register
    volatile unsigned int  *int_clr;    //  interrupt clear register
    volatile unsigned int  *dl_con;     //  data latch control register
} ifx_si_reg_t;

typedef struct {
    unsigned int            chipid;
    unsigned int            chipid_mask;
    ifx_si_reg_t            reg;
    unsigned int            pin_num;
    unsigned int            pin_status[IFX_SI_MAX_PIN];
} ifx_si_port_t;



/*
 * ####################################
 *        Platform Header File
 * ####################################
 */

#if defined(CONFIG_DANUBE) || defined(CONFIG_AMAZON_SE) || defined(CONFIG_AR10)
  #error Platform is not supported!
#elif defined(CONFIG_AR9)
  #include "ifxmips_si_ar9.h"
#elif defined(CONFIG_VR9)
  #include "ifxmips_si_vr9.h"
#else
  #error Platform is not specified!
#endif



#endif // _IFXMIPS_SI_H_
