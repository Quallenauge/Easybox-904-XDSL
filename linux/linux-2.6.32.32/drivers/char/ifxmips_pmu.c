/******************************************************************************
**
** FILE NAME    : ifxmips_pmu.c
** PROJECT      : IFX UEIP
** MODULES      : PMU
**
** DATE         : 28 May 2009
** AUTHOR       : Lei Chuanhua
** DESCRIPTION  : IFX Power Management Unit driver
** COPYRIGHT    :       Copyright (c) 2009
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
** 28 May 2009  Lei Chuanhua    The first UEIP release
*******************************************************************************/
/*!
  \defgroup IFX_PMU_FUNCTIONS External Clock Gating APIs
  \ingroup IFX_PMU
  \brief IFX PMU driver external functions
*/

/*!
  \defgroup IFX_PMU_PG_FUNCTIONS External Power Gating APIs
  \ingroup IFX_PMU
  \brief IFX PMU driver external functions
*/

/*!
  \defgroup IFX_PMU_OS OS APIs
  \ingroup IFX_PMU
  \brief IFX PMU driver OS interface functions
*/

/*!
  \defgroup IFX_PMU_INTERNAL Internal functions
  \ingroup IFX_PMU
  \brief IFX PMU driver internal functions
*/

/*!
  \file ifxmips_pmu.c
  \ingroup IFX_PMU
  \brief PMU driver source file
*/
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include<linux/module.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>

#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_pmu.h"

//#define IFX_PMU_DBG

#if defined(IFX_PMU_DBG)
#define IFX_PMU_PRINT(format, arg...)   \
    printk("%s: " format, __func__, ##arg)
#define INLINE 
#else
#define IFX_PMU_PRINT(format, arg...)   \
    do {} while(0)
#define INLINE inline
#endif

#define IFX_PMU_VER_MAJOR          1
#define IFX_PMU_VER_MID            2
#define IFX_PMU_VER_MINOR          2
#define IFX_PMU_NAME          "ifx_pmu"

#define IFX_PMU_MAX_COUNTER   1000000

static ifx_pmu_dev_t g_pmu_dev;
static struct proc_dir_entry *ifx_pmu_proc;
static DEFINE_SPINLOCK(ifx_pmu_lock);
#ifdef CONFIG_IFX_PMU_POWER_GATING
static DEFINE_SPINLOCK(ifx_pmu_pg_lock);
#endif /* CONFIG_IFX_PMU_POWER_GATING */

/**
 * \fn static int ifx_pmu_clk_idx_to_module(u32 module_idx, u32 *real_module)
 * \brief This function is used to convert power domain index to real power domain
 * 
 * \param   module_idx    The specified clock gating module index
 * \param   real_module   Pointer to returned real clock gating module
 
 * \return  -EINVAL       Invalid clock gating module
 * \return  0             Successfully return clock gating module
 * \ingroup IFX_PMU_INTERNAL 
 */
static int
ifx_pmu_clk_idx_to_module(u32 module_idx, u32 *real_module)
{

    if (module_idx < 0 || module_idx >= ARRAY_SIZE(g_ifx_pmu.module)) {
        return -EINVAL;
    }

    /* Check if there are some holes */
    if (g_ifx_pmu.module[module_idx].module_idx == -1) {
        return -EINVAL;
    }
  
    *real_module = g_ifx_pmu.module[module_idx].module;
    return 0;
}

/**
 * \fn static int ifx_pmu_clk_get_state(u32 module_idx, u32 *state)
 * \brief This function is used to get power gating state for the specified power domain
 * 
 * \param   domain_idx    The specified power domain index
 * \param   state         Pointer to returned power state
 
 * \return  -EINVAL       Invalid power domain
 * \return  0             Successfully get power state
 * \ingroup IFX_PMU_INTERNAL 
 */
static int
ifx_pmu_clk_get_state(u32 module_idx, u32 *state)
{
    u32 reg;
    int regidx; /* Register index in array */
    int regbit; /* Bit index in respective register */    
    u32 real_module;
    
    /* Sanity Check */
    if (ifx_pmu_clk_idx_to_module(module_idx, &real_module) != 0) {
        *state = 0;
        return -EINVAL;
    }

    regidx = real_module >> 5;  /* Equal to module/32 */
    regbit = real_module & 0x1F; /* Equal to module%32 */
    reg = IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx]);
    *state = (reg & regbit) > 0 ? 1: 0;
    return 0;
}

#ifdef CONFIG_AR10
static inline int
ifx_pmu_clk_on(int regidx, int regbit, int module)
{
    u32 cr;
    int counter = IFX_PMU_MAX_COUNTER;

    cr = (1 << regbit);
    IFX_REG_W32(cr, g_ifx_pmu.pmu_pwdcr[regidx]);
    IFX_PMU_PRINT("Actual enabling..........\n");
    while ( --counter ){
        /* 1 means enable */
        if (((IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx])) & (1 << regbit))){
            break;
        }
    }
    if (counter == 0) {
        printk(KERN_ERR "%s: failed to activate module %s!\n", 
            __func__, g_ifx_pmu.module[module].name);
        return -EIO;
    }     
    IFX_PMU_PRINT("PMU_PWDSR%d = 0x%08x regid %d regbit %d\n",
        regidx, IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx]), regidx, regbit);
    return 0;
    
}

static inline int
ifx_pmu_clk_off(int regidx, int regbit, int module)
{
    u32 cr;
    int counter = IFX_PMU_MAX_COUNTER;

    cr = (1 << regbit);
    IFX_REG_W32(cr, g_ifx_pmu.pmu_pwdcrb[regidx]);
    IFX_PMU_PRINT("Actual disabling..........\n");
    while ( --counter ){
        /* 0 means disable */
        if (!((IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx])) & (1 << regbit))){
            break;
        }
    }
    if (counter == 0) {
        printk(KERN_ERR "%s: failed to disactivate module %s!\n", 
            __func__, g_ifx_pmu.module[module].name);
        return -EIO;
    }
    IFX_PMU_PRINT("PMU_PWDSR%d = 0x%08x regid %d regbit %d\n",
        regidx, IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx]), regidx, regbit);
    return 0;
    
}

#else
static inline int
ifx_pmu_clk_on(int regidx, int regbit, int module)
{
    u32 cr;
    int counter = IFX_PMU_MAX_COUNTER;
    
    cr = IFX_REG_R32(g_ifx_pmu.pmu_pwdcr[regidx]);
    cr &= ~(1 << regbit);
    IFX_REG_W32(cr, g_ifx_pmu.pmu_pwdcr[regidx]);
    IFX_PMU_PRINT("Actual enabling..........\n");
    while ( --counter ){
        /* Zero means enable */
        if (!((IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx])) & (1 << regbit))){
            break;
        }
    }
    if (counter == 0) {
        printk(KERN_ERR "%s: failed to activate module %s!\n", 
            __func__, g_ifx_pmu.module[module].name);
        return -EIO;
    }     
    IFX_PMU_PRINT("PMU_PWDCR%d = 0x%08x regid %d regbit %d\n",
        regidx, cr, regidx, regbit);
    return 0;
}

static inline int
ifx_pmu_clk_off(int regidx, int regbit, int module)
{
    u32 cr;
    int counter = IFX_PMU_MAX_COUNTER;
    
    cr = IFX_REG_R32(g_ifx_pmu.pmu_pwdcr[regidx]);
    cr |= (1 << regbit);
    IFX_REG_W32(cr, g_ifx_pmu.pmu_pwdcr[regidx]);
    IFX_PMU_PRINT("Actual disabling..........\n");
    while ( --counter ){
        /* One means disable */
        if (((IFX_REG_R32(g_ifx_pmu.pmu_sr[regidx])) & (1 << regbit))){
            break;
        }
    }
    if (counter == 0) {
        printk(KERN_ERR "%s: failed to disactivate module %s!\n", 
            __func__, g_ifx_pmu.module[module].name);
        return -EIO;
    }    
    IFX_PMU_PRINT("PMU_PWDCR%d = 0x%08x regid %d regbit %d\n",
        regidx, cr, regidx, regbit);
    return 0;
}
#endif /* CONFIG_AR10 */

/**
 * \fn     int ifx_pmu_set(int module, int enable)
 *
 * \brief  This function is used to enable/dislabe every individual module
 *
 * \param  module  module id index defined in ifx_pmu.h
 * \param  enable  enable<1>/disable <0> the specified module.
 *
 * \return -EINVAL  invalid module or the module not used. This is for cross
 *                  platform sanity check
 * \return -1       Failed to enable or disable the specified module during 
 *                  IFX_PMU_MAX_COUNTER
 * \return 0       Successfull enable/disable the module
 * \ingroup IFX_PMU_FUNCTIONS
 */
int 
ifx_pmu_set(int module_idx, int enable)
{
    int regidx; /* Register index in array */
    int regbit; /* Bit index in respective register */
    int module;
    unsigned long flags;

    if (!g_ifx_pmu.module[module_idx].used) {
        return -EINVAL;
    }

    if (module_idx < IFX_PMU_MODULE_ID_IDX_MIN || module_idx > IFX_PMU_MODULE_ID_IDX_MAX) {
        return -EINVAL;
    }

    if (ifx_pmu_clk_idx_to_module(module_idx, &module) != 0) {
        return -EINVAL;
    }
    
    regidx = module >> 5;  /* Equal to module/32  */
    regbit = module & 0x1F; /* Equal to module%32 */
 
    /* Make sure it is enabled/disabled */
    if (enable) {
        spin_lock_irqsave(&ifx_pmu_lock, flags);
        if (g_ifx_pmu.module[module].refcnt == 0) {
            if (ifx_pmu_clk_on(regidx, regbit, module) != 0) {
                spin_unlock_irqrestore(&ifx_pmu_lock, flags);
                return -EIO;
            }
        }
        g_ifx_pmu.module[module].refcnt++; 
        spin_unlock_irqrestore(&ifx_pmu_lock, flags);
        IFX_PMU_PRINT("Module %s has been activated %d times before\n", 
            g_ifx_pmu.module[module].name, g_ifx_pmu.module[module].refcnt);
              
    }
    else {
        spin_lock_irqsave(&ifx_pmu_lock, flags);
        if (g_ifx_pmu.module[module].refcnt > 0) {
            g_ifx_pmu.module[module].refcnt--;
            IFX_PMU_PRINT("Module %s still is being used %d times\n", 
                g_ifx_pmu.module[module].name, g_ifx_pmu.module[module].refcnt);
        }
        
        /* Nobody is using. So we can disable it */
        if (g_ifx_pmu.module[module].refcnt == 0) {
            if (ifx_pmu_clk_off(regidx, regbit, module) != 0) {
                spin_unlock_irqrestore(&ifx_pmu_lock, flags);
                return -EIO;
            }
        }
        spin_unlock_irqrestore(&ifx_pmu_lock, flags);
    }

    if (g_ifx_pmu.module[module].refcnt == 0) {    
        IFX_PMU_PRINT("Module %s %s!!!\n", 
            g_ifx_pmu.module[module].name, enable ? "activated" : "disactivated"); 
    }
    return 0;
}
EXPORT_SYMBOL(ifx_pmu_set);

/**
 * \fn     void ifx_pmu_enable_all_modules(void)
 *
 * \brief  This function is used to clock on all modules
 *
 *
 * \return  none
 * \ingroup IFX_PMU_FUNCTIONS
 */
void 
ifx_pmu_enable_all_modules(void)
{
    int i;

    for (i = IFX_PMU_MODULE_ID_IDX_MIN; i < IFX_PMU_MODULE_ID_IDX_MAX; i++) {
        ifx_pmu_set(i, IFX_PMU_ENABLE);
    }
}
EXPORT_SYMBOL(ifx_pmu_enable_all_modules);
/**
 * \fn     void ifx_pmu_disable_all_modules(void)
 *
 * \brief  This function is used to clock off all modules
 *
 *
 * \return  none
 * \ingroup IFX_PMU_FUNCTIONS
 */
void 
ifx_pmu_disable_all_modules(void)
{
    int i;

    for (i = IFX_PMU_MODULE_ID_IDX_MIN; i < IFX_PMU_MODULE_ID_IDX_MAX; i++) {
        ifx_pmu_set(i, IFX_PMU_DISABLE);
    }
}
EXPORT_SYMBOL(ifx_pmu_disable_all_modules);
#ifdef CONFIG_IFX_PMU_POWER_GATING

/**
 * \fn static int ifx_pmu_pg_domain_is_valid(u32 domain_idx)
 * \brief check if the supplied power domain is valid
 * 
 * \param   domain_idx  The specified power domain index 
 * \return  1  The specified power domain is valid
 * \return  0  The specified power domain is invalid
 * \ingroup IFX_PMU_INTERNAL 
 */
static int 
ifx_pmu_pg_domain_is_valid(u32 domain_idx)
{
    return ifx_pmu_power_domain_valid(domain_idx);
}

/**
 * \fn static int ifx_pmu_pg_index_to_domain(u32 domain_idx, u32 *real_domain)
 * \brief This function is used to convert power domain index to real power domain
 * 
 * \param   domain_idx    The specified power domain index
 * \param   real_domain   Pointer to returned real power domain
 
 * \return  -EINVAL       Invalid power domain
 * \return  0             Successfully get power domain
 * \ingroup IFX_PMU_INTERNAL 
 */
static int
ifx_pmu_pg_index_to_domain(u32 domain_idx, u32 *real_domain)
{
    /* Sanity ceck first */
    if (domain_idx < 0 || domain_idx >= ARRAY_SIZE(g_ifx_pmu_pg)) {
        return -EINVAL;
    }
    *real_domain = g_ifx_pmu_pg[domain_idx].real_domain;
    return 0;
}

/**
 * \fn static int ifx_pmu_pg_get_state(u32 domain_idx, u32 *state)
 * \brief This function is used to get power gating state for the specified power domain
 * 
 * \param   domain_idx    The specified power domain index
 * \param   state         Pointer to returned power state
 
 * \return  -EINVAL       Invalid power domain
 * \return  0             Successfully get power state
 * \ingroup IFX_PMU_INTERNAL 
 */
static int
ifx_pmu_pg_get_state(u32 domain_idx, u32 *state)
{
    u32 reg;
    u32 real_domain;
    
    /* Sanity Check */
    if (!ifx_pmu_pg_domain_is_valid(domain_idx)) {
        *state = 0;
        return -EINVAL;
    }

    if (ifx_pmu_pg_index_to_domain(domain_idx, &real_domain) != 0) {
        *state = 0;
        return -EINVAL;
    }

    /* Select Power domain */
    IFX_REG_W32(real_domain, IFX_PMU_PD_SEL);
    reg = IFX_REG_R32(IFX_PMU_PWCSR);  
    *state = (reg & IFX_PMU_PWCSR_POWER_OFF) ? 0 : 1;
    return 0;
}

/**
 * \fn     int ifx_pmu_pg_enable(ifx_pmu_pg_t *pg)
 *
 * \brief  This function is used to enable the specified power domain
 *
 * \param  pg  pointer to ifx_pmu_pg_t which includes power domain index
 *
 * \return -EINVAL  Invalid power domain
 * \return -EIO     Failed to enable the specified power domain
 * \return 0        Successfull enable the power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int ifx_pmu_pg_enable(ifx_pmu_pg_t *pg)
{
    u32 reg;
    unsigned long flags;
    u32 domain_idx = pg->power_domain;
    u32 real_domain;
    u32 read_domain;

    /* Sanity Check */
    if (!ifx_pmu_pg_domain_is_valid(domain_idx)) {
        return -EINVAL;
    }

    if (ifx_pmu_pg_index_to_domain(domain_idx, &real_domain) != 0) {
        return -EINVAL;
    }

    spin_lock_irqsave(&ifx_pmu_pg_lock, flags);
    if (g_ifx_pmu_pg[domain_idx].refcnt == 0) {
        /* Block all wakeup and power down event */
        IFX_REG_W32(IFX_PMU_WKUP_GT_BLOCK_ALL, IFX_PMU_WKUP_GT);

        /* Power on the specified module */
        reg = real_domain;
        IFX_REG_W32(reg, IFX_PMU_PWCSR);

        /* Read it back to check status */
        IFX_REG_W32(real_domain, IFX_PMU_PD_SEL);

        reg = IFX_REG_R32(IFX_PMU_PWCSR);
        read_domain = MS(reg, IFX_PMU_PWCSR_POWER_DOMAIN);
        if ((read_domain != real_domain) || ((reg & IFX_PMU_PWCSR_POWER_OFF)) != 0) {
            spin_unlock_irqrestore(&ifx_pmu_lock, flags);
            IFX_PMU_PRINT("Power Domain %s enable failed\n", g_ifx_pmu_pg[domain_idx].domain_name);
            return -EIO;
        }
    }
    g_ifx_pmu_pg[domain_idx].refcnt++;
    spin_unlock_irqrestore(&ifx_pmu_pg_lock, flags);
    IFX_PMU_PRINT("Power Domain %s enable succeed refcnt %d\n",
        g_ifx_pmu_pg[domain_idx].domain_name, g_ifx_pmu_pg[domain_idx].refcnt);
    return 0;
}
EXPORT_SYMBOL(ifx_pmu_pg_enable);

/**
 * \fn     void ifx_pmu_pg_enable_all_domains(void)
 *
 * \brief  This function is used to enable all power domains except MIPS power domain
 *
 *
 * \return  none
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
void 
ifx_pmu_pg_enable_all_domains(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(g_ifx_pmu_pg); i++) {
        ifx_pmu_pg_t pg = {0};

        pg.power_domain = g_ifx_pmu_pg[i].pg.power_domain;
        /* MIPS power domain is an exception */
        if (pg.power_domain != IFX_PMU_PG_DOMAIN_MIPS) {
            ifx_pmu_pg_enable(&pg);
        }
    }
}
EXPORT_SYMBOL(ifx_pmu_pg_enable_all_domains);

/**
 * \fn     int ifx_pmu_pg_disable(ifx_pmu_pg_t *pg)
 *
 * \brief  This function is used to disable the specified power domain
 *
 * \param  pg  pointer to ifx_pmu_pg_t which includes power domain index
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable the specified power domain
 * \return 0        Successfull disable the power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_disable(ifx_pmu_pg_t *pg)
{
    u32 reg;
    unsigned long flags;
    u32 domain_idx = pg->power_domain;
    u32 real_domain;
    u32 read_domain;

    /* Sanity Check */
    if (!ifx_pmu_pg_domain_is_valid(domain_idx)) {
        return -EINVAL;
    }

    if (ifx_pmu_pg_index_to_domain(domain_idx, &real_domain) != 0) {
        return -EINVAL;
    }

    /* XXX, MIPS power gating is not so good according to test, fall back to 
     * SI_sleep. It is subject to change later.
     */
    if (domain_idx == IFX_PMU_PG_DOMAIN_MIPS) {
        reg = 0; /* Unblock everything so that external events can wake up */
    }
    else {
        reg = IFX_PMU_WKUP_GT_BLOCK_ALL;
    }

    spin_lock_irqsave(&ifx_pmu_pg_lock, flags);

    /* Consider disabling the module when powering on the system */
    if (g_ifx_pmu_pg[domain_idx].refcnt > 0) {
        g_ifx_pmu_pg[domain_idx].refcnt--;
        IFX_PMU_PRINT("Power Domain %s still is being used %d times\n", 
            g_ifx_pmu_pg[domain_idx].domain_name, g_ifx_pmu_pg[domain_idx].refcnt);
    }

    if (g_ifx_pmu_pg[domain_idx].refcnt == 0) {
        IFX_REG_W32(reg, IFX_PMU_WKUP_GT);

        /* XXX, subject to Change for MIPS Power Gating */
        if (domain_idx != IFX_PMU_PG_DOMAIN_MIPS) {
            reg = SM(real_domain, IFX_PMU_PWCSR_POWER_DOMAIN) | IFX_PMU_PWCSR_POWER_OFF;
            IFX_REG_W32(reg, IFX_PMU_PWCSR);

            /* Read it back to check status */
            IFX_REG_W32(real_domain, IFX_PMU_PD_SEL);
            reg = IFX_REG_R32(IFX_PMU_PWCSR);
            spin_unlock_irqrestore(&ifx_pmu_pg_lock, flags);
            read_domain = MS(reg, IFX_PMU_PWCSR_POWER_DOMAIN);
            if ((read_domain == real_domain) && ((reg & IFX_PMU_PWCSR_POWER_OFF)) != 0) {
                IFX_PMU_PRINT("Power Domain %s disable succeed\n", g_ifx_pmu_pg[domain_idx].domain_name);
                return 0;
            }
            else {
                IFX_PMU_PRINT("Power Domain %s disable failed\n", g_ifx_pmu_pg[domain_idx].domain_name);
                return -EIO;
            }
        }
        /* MIPS power gating fall through */
        goto done;
    }
done:
    spin_unlock_irqrestore(&ifx_pmu_pg_lock, flags);
    return 0;
}
EXPORT_SYMBOL(ifx_pmu_pg_disable);

/**
 * \fn     int ifx_pmu_pg_disable_all_domains(void)
 *
 * \brief  This function is used to disable all power domains except MIPS power domain
 *
 *
 * \return  none
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
void 
ifx_pmu_pg_disable_all_domains(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(g_ifx_pmu_pg); i++) {
        ifx_pmu_pg_t pg = {0};

        pg.power_domain = g_ifx_pmu_pg[i].pg.power_domain;
        /* MIPS power domain is an exception */
        if (pg.power_domain != IFX_PMU_PG_DOMAIN_MIPS) {
            ifx_pmu_pg_disable(&pg);
        }
    }
}
EXPORT_SYMBOL(ifx_pmu_pg_disable_all_domains);

/**
 * \fn     int ifx_pmu_pg_mips_enable(void)
 *
 * \brief  This function is used to power on MIPS power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable MIPS power domain
 * \return 0        Successfull enable MIPS power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_mips_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_MIPS;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_mips_enable);

/**
 * \fn     int ifx_pmu_pg_mips_disable(void)
 *
 * \brief  This function is used to power off MIPS power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable MIPS power domain
 * \return 0        Successfull disable MIPS power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_mips_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_MIPS;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_mips_disable);

/**
 * \fn     int ifx_pmu_pg_ppe_enable(void)
 *
 * \brief  This function is used to power on two PP32 power domains
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable two PP32 power domains
 * \return 0        Successfull enable PP32 power domains
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_ppe_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_PPE;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_ppe_enable);

/**
 * \fn     int ifx_pmu_pg_ppe_disable(void)
 *
 * \brief  This function is used to power off two PP32 power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable two PP32 power domains
 * \return 0        Successfull disable two PP32 power domains
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_ppe_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_PPE;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_ppe_disable);

/**
 * \fn     int ifx_pmu_pg_usb_enable(void)
 *
 * \brief  This function is used to power on USB power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable USB power domain
 * \return 0        Successfull enable USB power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_usb_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_USB;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_usb_enable);

/**
 * \fn     int ifx_pmu_pg_usb_disable(void)
 *
 * \brief  This function is used to power off USB power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable USB power domain
 * \return 0        Successfull disable USB power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_usb_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_USB;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_usb_disable);

/**
 * \fn     int ifx_pmu_pg_pcie_enable(void)
 *
 * \brief  This function is used to power on PCIe power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable PCIe power domain
 * \return 0        Successfull enable PCIe power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_pcie_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_PCIE;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_pcie_enable);

/**
 * \fn     int ifx_pmu_pg_pcie_disable(void)
 *
 * \brief  This function is used to power off PCIe power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable PCIe power domain
 * \return 0        Successfull disable PCIe power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_pcie_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_PCIE;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_pcie_disable);

/**
 * \fn     int ifx_pmu_pg_switch_enable(void)
 *
 * \brief  This function is used to power on switch power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable switch power domain
 * \return 0        Successfull enable switch power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_switch_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_SWITCH;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_switch_enable);

/**
 * \fn     int ifx_pmu_pg_switch_disable(void)
 *
 * \brief  This function is used to power off switch power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable switch power domain
 * \return 0        Successfull disable switch power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_switch_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_SWITCH;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_switch_disable);

/**
 * \fn     int ifx_pmu_pg_deu_enable(void)
 *
 * \brief  This function is used to power on DEU power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable DEU power domain
 * \return 0        Successfull enable DEU power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_deu_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_DEU;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_deu_enable);

/**
 * \fn     int ifx_pmu_pg_deu_disable(void)
 *
 * \brief  This function is used to power off DEU power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable DEU power domain
 * \return 0        Successfull disable DEU power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_deu_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_DEU;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_deu_disable);

/**
 * \fn     int ifx_pmu_pg_dsl_dfe_enable(void)
 *
 * \brief  This function is used to power on DSL DFE power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable DSL DFE power domain
 * \return 0        Successfull enable DSL DFE power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_dsl_dfe_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_DSL_DFE;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_dsl_dfe_enable);

/**
 * \fn     int ifx_pmu_pg_dsl_dfe_disable(void)
 *
 * \brief  This function is used to power off DSL DFE power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable DSL DFE power domain
 * \return 0        Successfull disable DSL DFE power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_dsl_dfe_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_DSL_DFE;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_dsl_dfe_disable);

/**
 * \fn     int ifx_pmu_pg_slic_tdm_enable(void)
 *
 * \brief  This function is used to power on SLIC TDM power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable SLIC TDM power domain
 * \return 0        Successfull enable SLIC TDM power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_slic_tdm_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_SLIC_TDM;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_slic_tdm_enable);

/**
 * \fn     int ifx_pmu_pg_slic_tdm_disable(void)
 *
 * \brief  This function is used to power off SLIC TDM power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable SLIC TDM power domain
 * \return 0        Successfull disable SLIC TDM power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_slic_tdm_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_SLIC_TDM;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_slic_tdm_disable);

/**
 * \fn     int ifx_pmu_pg_fpi_top_enable(void)
 *
 * \brief  This function is used to power on FPI TOP power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to enable FPI TOP power domain
 * \return 0        Successfull enable FPI TOP power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_fpi_top_enable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_FPI_TOP;
    return ifx_pmu_pg_enable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_fpi_top_enable);

/**
 * \fn     int ifx_pmu_pg_fpi_top_disable(void)
 *
 * \brief  This function is used to power off FPI TOP power domain
 *
 *
 * \return -EINVAL  Invalid power domain or domain index
 * \return -EIO     Failed to disable FPI TOP power domain
 * \return 0        Successfull disable FPI TOP power domain
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_fpi_top_disable(void)
{
    ifx_pmu_pg_t pg = {0};

    pg.power_domain = IFX_PMU_PG_DOMAIN_FPI_TOP;
    return ifx_pmu_pg_disable(&pg);
}
EXPORT_SYMBOL(ifx_pmu_pg_fpi_top_disable);

/**
 * \fn     int ifx_pmu_pg_wkup_loc_setup(void *loc)
 *
 * \brief  This function is used to set up MIPS wakeup relocation address
 *
 * \param  loc  pointer to recovery routine address
 *
 * \return -EINVAL  Invalid relocation address. Relocation address must belong to KSEG0
 * \return 0        Successfull set up wakeup relocation address
 * \ingroup IFX_PMU_PG_FUNCTIONS
 */
int 
ifx_pmu_pg_wkup_loc_setup(void *loc)
{
    u32 reg;

    if (loc == NULL || KSEGX((u32)loc) != KSEG0) {
        return -EINVAL;
    }

    reg = (u32)loc;
    IFX_REG_W32(reg, IFX_PMU_WKUP_LOC);
    
    return 0;
}
EXPORT_SYMBOL(ifx_pmu_pg_wkup_loc_setup);
#endif /* CONFIG_IFX_PMU_POWER_GATING */

/** 
 * \fn int  ifx_pmu_open(struct inode *inode, struct file *filp)
 * 
 * \param   inode Inode of device or 0/1 for calls from kernel mode 
 * \param   filp  File pointer of device 
 * \return  0     OK 
 * \ingroup IFX_PMU_OS 
 */
static INLINE int 
ifx_pmu_open(struct inode *inode, struct file *filp)
{
    return 0;
}

/** 
 * \fn int  ifx_pmu_release(struct inode *inode, struct file *filp)
 * \param   inode Inode of device or 0 or 1 for calls from kernel mode
 * \param   filp  File pointer of device 
 * \return  0     OK 
 * \ingroup IFX_PMU_OS
 */
static INLINE  int 
ifx_pmu_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/** 
 * \fn static int ifx_pmu_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
 *
 * \brief PMU IOCTL
 * \param   inode        Inode of device  
 * \param   filp         File structure of device 
 * \param   cmd          IOCTL command  
 * \param   arg          Argument for some IOCTL commands 
 * \return  -EINVAL      Invalid IOCTL command
 * \return  -EACCES      Wrong access rights
 * \return  -ENOIOCTLCMD  No Ioctl command
 * \return  -EFAULT      Failed to get user data
 * \ingroup IFX_PMU_OS
 */
static int  
ifx_pmu_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ionr;
    int ret = 0;

    IFX_PMU_PRINT("cmd=0x%x\n", cmd);

    /* Magic number */
    if ( _IOC_TYPE(cmd) != IFX_PMU_IOC_MAGIC ) {
        return -EINVAL;;
    }

    /* Access rights */
    if ( ((_IOC_DIR(cmd) & _IOC_WRITE) && !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd)))
        || ((_IOC_DIR (cmd) & _IOC_READ) && !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd)))){
        return -EACCES;
    }

    ionr = _IOC_NR(cmd);
    
#ifdef CONFIG_IFX_PMU_POWER_GATING
    if (ionr == IFX_PMU_IOC_PG_GET || ionr == IFX_PMU_IOC_PG_ENABLE || ionr == IFX_PMU_IOC_PG_DISABLE) {

        ifx_pmu_pg_t pg = {0};

        if (copy_from_user((void *)&pg, (void *)arg, sizeof(pg))) {
            return -EFAULT;
        }
        switch (cmd) {
            case IFX_PMU_IOC_POWER_GATING_ENABLE:
                ret = ifx_pmu_pg_enable(&pg);
                break;
            case IFX_PMU_IOC_POWER_GATING_DISABLE:
                ret = ifx_pmu_pg_disable(&pg);
                break;
            case IFX_PMU_IOC_GET_POWER_GATING:
                /* XXX, reuse flags as state */
                ifx_pmu_pg_get_state(pg.power_domain, &pg.flags);
                ret = copy_to_user((void *)arg, (void *)&pg, sizeof(pg));
                break;                
        }
        return ret;
    }
    else 
#endif /* CONFIG_IFX_PMU_POWER_GATING */
    {
        ifx_pmu_clk_t clk = {0};

        if (copy_from_user((void *)&clk, (void *)arg, sizeof(clk))) {
            return -EFAULT;
        }
        switch (cmd) {
            case IFX_PMU_IOC_CLK_GATING_ENABLE:
            case IFX_PMU_IOC_CLK_GATING_DISABLE:
                ret = ifx_pmu_set(clk.module, clk.enable);
                break;
            case IFX_PMU_IOC_GET_CLK_GATING:
                /* XXX, reuse flags as state */
                ifx_pmu_clk_get_state(clk.module, &clk.enable);
                ret = copy_to_user((void *)arg, (void *)&clk, sizeof(clk));
                break;                
        }    
        return ret;
    }
}

static struct file_operations ifx_pmu_fops = {
    .owner     = THIS_MODULE,
    .open      = ifx_pmu_open,
    .release   = ifx_pmu_release,
    .ioctl     = ifx_pmu_ioctl,
};

/**
 * \fn static inline int ifx_pmu_drv_ver(char *buf)
 * \brief Display PMU driver version after initilazation succeeds
 * 
 * \param buf  version string buffer 
 * \return number of bytes will be printed
 * \ingroup IFX_PMU_INTERNAL 
 */
static inline int 
ifx_pmu_drv_ver(char *buf)
{
    return ifx_drv_ver(buf, "PMU", IFX_PMU_VER_MAJOR, IFX_PMU_VER_MID, IFX_PMU_VER_MINOR);
}

/**
 * \fn static inline int ifx_pmu_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
 *
 * \brief  This function is used read the proc entries used by PMU.
 *         Here, the proc entry is used to read version number
 *
 * \param  buf    Proc buffer
 * \param  start  start of the proc entry
 * \param  offset if set to zero, do not proceed to print proc data
 * \param  count  Maximum proc print size
 * \param  eof    end of the proc entry
 * \param  data   unused
 *
 * \return len    Lenght of data in buffer            
 *
 * \ingroup IFX_PMU_INTERNAL
 */
static inline int 
ifx_pmu_proc_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    /* No sanity check cos length is smaller than one page */
    len += ifx_pmu_drv_ver(buf + len);
    *eof = 1;
    return len;
} 

/**
 * \fn static int ifx_pmu_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
 *
 * \brief  This function is used read the proc entries used by PMU.
 *         Here, the proc entry is used to register status and module
 *         reference number.
 *
 * \param  buf    Proc buffer
 * \param  start  start of the proc entry
 * \param  offset if set to zero, do not proceed to print proc data
 * \param  count  Maximum proc print size
 * \param  eof    end of the proc entry
 * \param  data   unused
 *
 * \return Lenght of data in buffer            
 *
 * \ingroup IFX_PMU_INTERNAL
 */
static int 
ifx_pmu_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int i;
    int len = 0;
    int reg_num = 3; /* Maximum register number */

    len += sprintf(buf + len, "PMU Register Dump\n");
    for (i = 0; i < reg_num; i++) {
        /* No more register existing for particular platforms */
        if (g_ifx_pmu.pmu_sr[i] == NULL) {
            break;
        }
        len += sprintf(buf + len, "PMU_SR%d(%p)    : 0x%08x\n", 
                i, g_ifx_pmu.pmu_sr[i], IFX_REG_R32(g_ifx_pmu.pmu_sr[i]));
        
    }
    len += sprintf(buf + len,"Max module number %d\n", IFX_PMU_MAX_MODULE);
    for (i = 0; i < IFX_PMU_MAX_MODULE; i++) {
        if (g_ifx_pmu.module[i].module_idx != -1) {
            len += sprintf(buf + len, "Module %-2d %-12s refcnt %-4d\n",
                i, g_ifx_pmu.module[i].name, g_ifx_pmu.module[i].refcnt);
        }
    }

#ifdef CONFIG_IFX_PMU_POWER_GATING
    for (i = 0; i < ARRAY_SIZE(g_ifx_pmu_pg); i++) {
        u32 state;

        if (!g_ifx_pmu_pg[i].used) {
            continue;
        }
        ifx_pmu_pg_get_state(g_ifx_pmu_pg[i].pg.power_domain, &state);
       
        len += sprintf(buf + len, "Power Domain %s is %s\n", g_ifx_pmu_pg[i].domain_name, 
            state == 0 ? "OFF" : "ON");
    }
#endif /* CONFIG_IFX_PMU_POWER_GATING */
    *eof = 1;
    return len;
}

static void 
ifx_pmu_proc_create(void)
{
    ifx_pmu_proc = proc_mkdir("driver/ifx_pmu", NULL);

    create_proc_read_entry("version",
                            0,
                            ifx_pmu_proc,
                            ifx_pmu_proc_version,
                            NULL);

    create_proc_read_entry("pmu",
                            0,
                            ifx_pmu_proc,
                            ifx_pmu_proc_read,
                            NULL);
}

static void 
ifx_pmu_proc_delete(void)
{
    remove_proc_entry("version", ifx_pmu_proc);

    remove_proc_entry("pmu", ifx_pmu_proc);

    remove_proc_entry("driver/ifx_pmu", NULL);
}

/**
 * \fn  static int __init ifx_pmu_init(void)
 *
 * \brief  This is module initialisation function for PMU
 *
 * \return =0  OK
 * \return <0  error number
 *
 * \ingroup IFX_PMU_OS
 */
static int __init 
ifx_pmu_init(void)
{
    int result;
    char ver_str[128] = {0};

    memset(&g_pmu_dev, 0, sizeof(ifx_pmu_dev_t));

    strcpy(g_pmu_dev.name, IFX_PMU_NAME);
    g_pmu_dev.minor = 0;

    result = register_chrdev(0, g_pmu_dev.name, &ifx_pmu_fops);
    if (result > 0) {
        g_pmu_dev.major = result;
    }
    else {
        printk("%s failed to register\n", __func__);
        return result;
    }
    printk(KERN_INFO "%s: Major %d\n",  __func__, result);
    
    ifx_pmu_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    ifx_pmu_proc_create();
    return 0;
}

/**
 * \fn static void __exit ifx_pmu_exit(void)
 *
 * \brief  This is module exit function for PMU
 *
 * \return None           
 *
 * \ingroup IFX_PMU_OS
 */
static void __exit 
ifx_pmu_exit(void)
{
    unregister_chrdev(g_pmu_dev.major, g_pmu_dev.name);
    ifx_pmu_proc_delete();
}

module_init(ifx_pmu_init);
module_exit(ifx_pmu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LeiChuanhua <Chuanhua.lei@infineon.com>");
MODULE_DESCRIPTION("IFX Power Management Unit Clock Gating Driver");
MODULE_SUPPORTED_DEVICE ("Amazon_SE Danube AR9 VR9 PMU clock gating");

