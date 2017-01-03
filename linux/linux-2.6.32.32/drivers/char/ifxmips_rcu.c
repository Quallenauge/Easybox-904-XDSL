/******************************************************************************
**
** FILE NAME    : ifxmips_rcu.c
** PROJECT      : UEIP
** MODULES      : RCU (Reset Control Unit)
**
** DATE         : 17 Jun 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : RCU driver common source file
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



/*!
  \file ifxmips_rcu.c
  \ingroup IFX_RCU
  \brief RCU driver main source file.
*/



/*
 * ####################################
 *              Version No.
 * ####################################
 */

#define IFX_RCU_VER_MAJOR               1
#define IFX_RCU_VER_MID                 0
#define IFX_RCU_VER_MINOR               8



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kallsyms.h>
//#include <asm/semaphore.h>
//#include <asm/uaccess.h>
#include <asm/delay.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_rcu.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  File Operations
 */
static int ifx_rcu_open(struct inode *inode, struct file *filep);
static int ifx_rcu_release(struct inode *inode, struct file *filelp);
static int ifx_rcu_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

/*
 *  Proc File Functions
 */
static inline void proc_file_create(void);
static inline void proc_file_delete(void);
static int proc_read_version(char *, char **, off_t, int, int *, void *);
static int proc_read_domain(char *, char **, off_t, int, int *, void *);

/*
 *  Proc Help Functions
 */
static inline int print_reset_domain(char *, int);
static inline int print_reset_handler(char *, int, ifx_rcu_handler_t *);

/*
 *  Init Help Functions
 */
static inline int ifx_rcu_version(char *);



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

spinlock_t g_rcu_lock;          //  must be locked before g_rcu_register_lock locked
                                //  and be released after g_rcu_register_lock released

spinlock_t g_rcu_register_lock; //  use spinlock rather than semaphore or mutex
                                //  because most functions run in user context
                                //  and they do not take much time to finish operation

static IFX_RCU_DECLARE_DOMAIN_NAME(g_rcu_domain_name);

static IFX_RCU_DECLARE_MODULE_NAME(g_rcu_module_name);

static struct file_operations g_rcu_fops = {
    .open    = ifx_rcu_open,
    .release = ifx_rcu_release,
    .ioctl   = ifx_rcu_ioctl
};

static unsigned int g_dbg_enable = DBG_ENABLE_MASK_ERR;

static struct proc_dir_entry* g_rcu_dir = NULL;



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static int ifx_rcu_open(struct inode *inode, struct file *filep)
{
    return IFX_SUCCESS;
}

static int ifx_rcu_release(struct inode *inode, struct file *filelp)
{
    return IFX_SUCCESS;
}

static int ifx_rcu_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;

    //  check magic number
    if ( _IOC_TYPE(cmd) != IFX_RCU_IOC_MAGIC )
        return IFX_ERROR;

    //  check read/write right
    if ( ((_IOC_DIR(cmd) & _IOC_WRITE) && !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd)))
        || ((_IOC_DIR (cmd) & _IOC_READ) && !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd))) )
        return IFX_ERROR;

    switch (cmd) {
    case IFX_RCU_IOC_VERSION:
        {
            struct ifx_rcu_ioctl_version version = {
                .major = IFX_RCU_VER_MAJOR,
                .mid   = IFX_RCU_VER_MID,
                .minor = IFX_RCU_VER_MINOR
            };
            ret = copy_to_user((void *)arg, (void *)&version, sizeof(version));
        }
        break;
    case IFX_RCU_IOC_QUERY_RST_DOMAIN:
        {
            struct ifx_rcu_ioctl_query_rst_domain query = {0};

            ret = copy_from_user((void *)&query, (void *)arg, sizeof(query));
            if ( ret == IFX_SUCCESS ) {
                if ( query.domain_id < NUM_ENTITY(g_rcu_domains) ) {
                    query.f_reset = ifx_rcu_stat_get(query.domain_id);
                    ret = copy_to_user((void *)arg, (void *)&query, sizeof(query));
                }
                else
                    ret = IFX_ERROR;
            }
        }
        break;
    default:
        ret = IFX_ERROR;
    }

    return ret;
}

static inline void proc_file_create(void)
{
    g_rcu_dir = proc_mkdir("driver/ifx_rcu", NULL);

    create_proc_read_entry("version",
                            0,
                            g_rcu_dir,
                            proc_read_version,
                            NULL);

    create_proc_read_entry("domain",
                            0,
                            g_rcu_dir,
                            proc_read_domain,
                            NULL);
}

static inline void proc_file_delete(void)
{
    remove_proc_entry("domain", g_rcu_dir);

    remove_proc_entry("version", g_rcu_dir);

    remove_proc_entry("driver/ifx_rcu", NULL);
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_rcu_version(buf + len);

    if ( offset >= len ) {
        *start = buf;
        *eof = 1;
        return 0;
    }
    *start = buf + offset;
    if ( (len -= offset) > count )
        return count;
    *eof = 1;
    return len;
}

static int proc_read_domain(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen;
    int i, j;
    ifx_rcu_handler_t *p_handler;

    pstr = *start = page;

    for ( i = 0; i < NUM_ENTITY(g_rcu_domains); i++ ) {
        llen = print_reset_domain(str, i);
        if ( len <= off && len + llen > off )
        {
            memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_DOMAIN_OVERRUN_END;

        for ( j = 1, p_handler = g_rcu_domains[i].handlers; p_handler != NULL; j++, p_handler = p_handler->next ) {
            llen = print_reset_handler(str, j, p_handler);
            if ( len <= off && len + llen > off )
            {
                memcpy(pstr, str + off - len, len + llen - off);
                pstr += len + llen - off;
            }
            else if ( len > off )
            {
                memcpy(pstr, str, llen);
                pstr += llen;
            }
            len += llen;
            if ( len >= len_max )
                goto PROC_READ_DOMAIN_OVERRUN_END;
        }
    }

    *eof = 1;

    return len - off;

PROC_READ_DOMAIN_OVERRUN_END:
    return len - llen - off;
}

static inline int print_reset_domain(char *buf, int i)
{
    int len = 0;
    unsigned int domain_id_bits;
    int domain_id;
    int flag;

    len += sprintf(buf + len,       "Reset Domain - %s\n", g_rcu_domain_name[i]);
    flag = 0;
    domain_id_bits = g_rcu_domains[i].affected_domains & ~(1 << i);
    while ( domain_id_bits != 0 ) {
        domain_id = clz(domain_id_bits);
        domain_id_bits ^= 1 << domain_id;

        if ( !flag ) {
            len += sprintf(buf + len, "  affected domains: %s", g_rcu_domain_name[domain_id]);
            flag++;
        }
        else
            len += sprintf(buf + len, ", %s", g_rcu_domain_name[domain_id]);

        if ( domain_id_bits == 0 )
            len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len,       "  rst_req_value - %#08x, rst_req_mask - %#08x, rst_stat_mask - %#08x\n", g_rcu_domains[i].rst_req_value, g_rcu_domains[i].rst_req_mask, g_rcu_domains[i].rst_stat_mask);
    if ( g_rcu_domains[i].latch )
        len += sprintf(buf + len,   "  latch mode, udelay - %d\n", g_rcu_domains[i].udelay);
    else
        len += sprintf(buf + len,   "  pulse mode\n");
    if ( g_rcu_domains[i].handlers )
        len += sprintf(buf + len,   "  handlers:\n");
    else
        len += sprintf(buf + len,   "  handlers: NULL\n");

    return len;
}

static inline int print_reset_handler(char *buf, int i, ifx_rcu_handler_t *handler)
{
    int len = 0;
    const char *name = NULL;
#if defined(CONFIG_KALLSYMS)
    unsigned long offset, size;
    char *modname;
    char namebuf[KSYM_NAME_LEN+1];
#endif

#if defined(CONFIG_KALLSYMS)
    name = kallsyms_lookup((unsigned long)handler->fn, &size, &offset, &modname, namebuf);
#endif

    len += sprintf(buf + len,     "  %d. next   - %#08x\n", i, (unsigned int)handler->next);
    if ( name != NULL )
        len += sprintf(buf + len, "     fn     - %s (%#08x)\n", name, (unsigned int)handler->fn);
    else
        len += sprintf(buf + len, "     fn     - %#08x\n", (unsigned int)handler->fn);
    len += sprintf(buf + len,     "     arg    - %#08lx\n", handler->arg);
    len += sprintf(buf + len,     "     module - %s\n", g_rcu_module_name[handler->module_id]);

    return len;
}

static inline int ifx_rcu_version(char *buf)
{
    return sprintf(buf, "Infineon Technologies RCU driver version %d.%d.%d \n", IFX_RCU_VER_MAJOR, IFX_RCU_VER_MID, IFX_RCU_VER_MINOR);
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*!
  \fn       unsigned int ifx_rcu_rst_req_read(void)
  \brief    Read register "IFX_RCU_RST_REQ" to get reset status.

            This function provide direct access to register "IFX_RCU_RST_REQ" for
            status query. Protection for multiple access is provided in this function.

  \return   unsigned int - value of register "IFX_RCU_RST_REQ"
  \ingroup  IFX_RCU_API
 */
unsigned int ifx_rcu_rst_req_read(void)
{
    unsigned long sys_flags;
    unsigned int ret;

    spin_lock_irqsave(&g_rcu_register_lock, sys_flags);
    ret = IFX_REG_R32(IFX_RCU_RST_REQ);
    spin_unlock_irqrestore(&g_rcu_register_lock, sys_flags);

    return ret;
}
EXPORT_SYMBOL(ifx_rcu_rst_req_read);

/*!
  \fn       void ifx_rcu_rst_req_write(unsigned int value, unsigned int mask)
  \brief    Write register "IFX_RCU_RST_REQ" to reset individual hardware module.

            This function provide direct access to register "IFX_RCU_RST_REQ" for
            reseting individual hardware module. Protection for multiple access
            is provided in this function.

  \param    value - unsigned int, each bit stands for one hardware module
  \param    mask  - usnigned int, value AND mask before writing to register
  \ingroup  IFX_RCU_API
 */
void ifx_rcu_rst_req_write(unsigned int value, unsigned int mask)
{
    unsigned long sys_flags;

    spin_lock_irqsave(&g_rcu_register_lock, sys_flags);
    IFX_REG_W32_MASK(mask, value & mask, IFX_RCU_RST_REQ);
    spin_unlock_irqrestore(&g_rcu_register_lock, sys_flags);
}
EXPORT_SYMBOL(ifx_rcu_rst_req_write);

/*!
  \fn       unsigned int ifx_rcu_rst_stat_read(void)
  \brief    Read register "IFX_RCU_RST_STAT" to get reset status.

            This function provide direct access to register "IFX_RCU_RST_STAT" for
            status query. Protection for multiple access is provided in this function.

  \return   unsigned int - value of register "IFX_RCU_RST_STAT"
  \ingroup  IFX_RCU_API
 */
unsigned int ifx_rcu_rst_stat_read(void)
{
    unsigned long sys_flags;
    unsigned int ret;

    spin_lock_irqsave(&g_rcu_register_lock, sys_flags);
    ret = IFX_REG_R32(IFX_RCU_RST_STAT);
    spin_unlock_irqrestore(&g_rcu_register_lock, sys_flags);

    return ret;
}
EXPORT_SYMBOL(ifx_rcu_rst_stat_read);

/*!
  \fn       int ifx_rcu_request(unsigned int reset_domain_id, unsigned int module_id, ifx_rcu_callbackfn callbackfn, unsigned long arg)
  \brief    Register ownership of one hardware module.

            User uses this function to register ownership of one hardware module and register
            callback function to receive reset event. When reset is triggered by other component,
            one pre-reset event is received through callback function before reset, and one
            post-reset event is received through callback function after reset.

  \param    reset_domain_id - unsigned int, hardware module ID
  \param    module_id       - unsigned int, driver/component module ID
  \param    callbackfn      - ifx_rcu_callbackfn, callback function to receive event
  \param    arg             - unsigned long, parameter of callback function
  \return   IFX_SUCCESS     Register successfully.
  \return   IFX_ERROR       Register fail.
  \ingroup  IFX_RCU_API
 */
int ifx_rcu_request(unsigned int reset_domain_id, unsigned int module_id, ifx_rcu_callbackfn callbackfn, unsigned long arg)
{
    unsigned long sys_flags;
    ifx_rcu_handler_t *p_handler;
    ifx_rcu_handler_t *p_cur;

    if ( reset_domain_id >= NUM_ENTITY(g_rcu_domains) || module_id >= IFX_RCU_MODULE_MAX || callbackfn == NULL )
        return IFX_ERROR;

    p_handler = (ifx_rcu_handler_t *)kmalloc(sizeof(*p_handler), GFP_KERNEL);
    if ( p_handler == NULL )
        return IFX_ERROR;

    p_handler->fn           = callbackfn;
    p_handler->arg          = arg;
    p_handler->module_id    = module_id;

    spin_lock_irqsave(&g_rcu_lock, sys_flags);
    for ( p_cur = g_rcu_domains[reset_domain_id].handlers; p_cur != NULL; p_cur = p_cur->next )
        if ( p_cur->module_id == module_id )
            break;
    if ( p_cur != NULL ) {
        p_cur->fn   = callbackfn;
        p_cur->arg  = arg;
    }
    else {
        p_handler->next = g_rcu_domains[reset_domain_id].handlers;
        g_rcu_domains[reset_domain_id].handlers = p_handler;
    }
    spin_unlock_irqrestore(&g_rcu_lock, sys_flags);

    if ( p_cur != NULL )
        kfree(p_handler);

    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_rcu_request);

/*!
  \fn       int ifx_rcu_free(unsigned int reset_domain_id, unsigned int module_id)
  \brief    Release ownership of one hardware module.

            User uses this function to release ownership of one hardware module.
            Driver will not notify this component for reset event if this function return
            successfully.

  \param    reset_domain_id - unsigned int, hardware module ID
  \param    module_id       - unsigned int, driver/component module ID
  \return   IFX_SUCCESS     Release successfully.
  \return   IFX_ERROR       Release fail.
  \ingroup  IFX_RCU_API
 */
int ifx_rcu_free(unsigned int reset_domain_id, unsigned int module_id)
{
    unsigned long sys_flags;
    ifx_rcu_handler_t *p_cur, *p_prev;

    if ( reset_domain_id >= NUM_ENTITY(g_rcu_domains) || module_id >= IFX_RCU_MODULE_MAX )
        return IFX_ERROR;

    spin_lock_irqsave(&g_rcu_lock, sys_flags);
    for ( p_prev = NULL, p_cur = g_rcu_domains[reset_domain_id].handlers; p_cur != NULL; p_prev = p_cur, p_cur = p_cur->next )
        if ( p_cur->module_id == module_id ) {
            if ( p_prev != NULL )
                p_prev->next = p_cur->next;
            else
                g_rcu_domains[reset_domain_id].handlers = p_cur->next;
            break;
        }
    spin_unlock_irqrestore(&g_rcu_lock, sys_flags);

    if ( p_cur != NULL ) {
        kfree(p_cur);
        return IFX_SUCCESS;
    }
    else
        return IFX_ERROR;
}
EXPORT_SYMBOL(ifx_rcu_free);

/*!
  \fn       int ifx_rcu_stat_get(unsigned int reset_domain_id)
  \brief    Get reset status of given hardware module.

            User uses this function to get reset status of given hardware module.
            It's a wrapping of function "ifx_rcu_rst_stat_read".

  \param    reset_domain_id - unsigned int, hardware module ID
  \return   1   in reset status
  \return   0   normal status
  \ingroup  IFX_RCU_API
 */
int ifx_rcu_stat_get(unsigned int reset_domain_id)
{
    if ( reset_domain_id >= NUM_ENTITY(g_rcu_domains) )
        return 0;

    return (ifx_rcu_rst_stat_read() & g_rcu_domains[reset_domain_id].rst_stat_mask) ? 1 : 0;
}
EXPORT_SYMBOL(ifx_rcu_stat_get);

/*!
  \fn       int ifx_rcu_rst(unsigned int reset_domain_id, unsigned int module_id)
  \brief    Trigger reset of one hardware module.

            User uses this function to trigger reset of one hardware module.
            Driver will give notification to all drivers/components registering
            for reset of this hardware module.

  \param    reset_domain_id - unsigned int, hardware module ID
  \param    module_id       - unsigned int, driver/component module ID
  \return   IFX_SUCCESS     Reset successfully.
  \return   IFX_ERROR       Reset fail.
  \ingroup  IFX_RCU_API
 */
int ifx_rcu_rst(unsigned int reset_domain_id, unsigned int module_id)
{
    unsigned long sys_flags;
    unsigned int domain_id_bits;
    int domain_id;
    ifx_rcu_handler_t *p_cur;

    spin_lock_irqsave(&g_rcu_lock, sys_flags);
    //  trigger pre-reset event, each handler must be as fast as possible, because interrupt is disabled
    domain_id_bits = g_rcu_domains[reset_domain_id].affected_domains;
    while ( domain_id_bits != 0 ) {
        domain_id = clz(domain_id_bits);
        domain_id_bits ^= 1 << domain_id;

        for ( p_cur = g_rcu_domains[domain_id].handlers; p_cur != NULL; p_cur = p_cur->next )
            if ( p_cur->module_id != module_id )
                p_cur->fn(domain_id, module_id, 0, p_cur->arg);
    }
    //  issue reset
    ifx_rcu_rst_req_write(g_rcu_domains[reset_domain_id].rst_req_value, g_rcu_domains[reset_domain_id].rst_req_mask);
    if ( g_rcu_domains[reset_domain_id].latch ) {
        udelay(g_rcu_domains[reset_domain_id].udelay);
        ifx_rcu_rst_req_write(~g_rcu_domains[reset_domain_id].rst_req_value, g_rcu_domains[reset_domain_id].rst_req_mask);
    }
    else {
        int max_count = 1000;

        while ( (ifx_rcu_rst_stat_read() & g_rcu_domains[reset_domain_id].rst_stat_mask) && max_count-- );
        if ( max_count < 0 ) {
            err("timeout during reset domain - %s", g_rcu_domain_name[reset_domain_id]);
        }
    }
    //  trigger post-reset event, each handler must be as fast as possible, because interrupt is disabled
    domain_id_bits = g_rcu_domains[reset_domain_id].affected_domains;
    while ( domain_id_bits != 0 ) {
        domain_id = clz(domain_id_bits);
        domain_id_bits ^= 1 << domain_id;

        for ( p_cur = g_rcu_domains[domain_id].handlers; p_cur != NULL; p_cur = p_cur->next )
            if ( p_cur->module_id != module_id )
                p_cur->fn(domain_id, module_id, 1, p_cur->arg);
    }
    spin_unlock_irqrestore(&g_rcu_lock, sys_flags);

    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_rcu_rst);



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

static int __init ifx_rcu_pre_init(void)
{
    int i;

    spin_lock_init(&g_rcu_lock);
    spin_lock_init(&g_rcu_register_lock);

    for ( i = 0; i < NUM_ENTITY(g_rcu_domains); i++ )
        g_rcu_domains[i].handlers = NULL;

    return IFX_SUCCESS;
}

/**
 *  ifx_port_init - Initialize port structures
 *
 *  This function initializes the internal data structures of the driver
 *  and will create the proc file entry and device.
 *
 *      Return Value:
 *  @OK = OK
 */
static int __init ifx_rcu_init(void)
{
    int ret;
    char ver_str[64];

    /* register port device */
    ret = register_chrdev(IFX_RCU_MAJOR, "ifx_rcu", &g_rcu_fops);
    if ( ret != 0 ) {
        err("Can not register RCU device - %d", ret);
        return ret;
    }

    proc_file_create();

    ifx_rcu_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    return IFX_SUCCESS;
}

static void __exit ifx_rcu_exit(void)
{
    ifx_rcu_handler_t *p_cur, *p_free;
    int i;

    proc_file_delete();

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    if ( unregister_chrdev(IFX_RCU_MAJOR, "ifx_rcu") ) {
        err("Can not register RCU device (major %d)!", IFX_RCU_MAJOR);
        return;
    }
#else
    unregister_chrdev(IFX_RCU_MAJOR, "ifx_rcu");
#endif

    for ( i = 0; i < NUM_ENTITY(g_rcu_domains); i++ ) {
        for ( p_free = p_cur = g_rcu_domains[i].handlers; p_cur != NULL; p_free = p_cur) {
            p_cur = p_cur->next;
            kfree(p_free);
        }
    }
}

postcore_initcall(ifx_rcu_pre_init);   //  to be called earlier than other drivers as well as kernel modules
module_init(ifx_rcu_init);
module_exit(ifx_rcu_exit);
