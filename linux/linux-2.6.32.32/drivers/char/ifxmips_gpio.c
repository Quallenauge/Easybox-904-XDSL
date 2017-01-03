/******************************************************************************
**
** FILE NAME    : ifxmips_gpio.c
** PROJECT      : IFX UEIP
** MODULES      : GPIO
**
** DATE         : 21 Jun 2004
** AUTHOR       : btxu
** DESCRIPTION  : Global IFX GPIO driver source file
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
** 21 Jun 2005   Jin-Sze.Sow     Comments edited
** 01 Jan 2006   Huang Xiaogang  Modification & verification on Amazon_S chip
** 11 Feb 2008   Lei Chuanhua    Detect GPIO conflicts
** 28 May 2008   Lei Chuanhua    Added forth port support & cleanup source code
** 10 Nov 2008   Lei Chuanhua    Ported to VR9 and use new register defs
** 12 Mar 2009   Lei Chuanhua    Added GPIO/module display
** 22 May 2009   Xu Liang        UEIP
*******************************************************************************/



/*!
  \file ifxmips_gpio.c
  \ingroup IFX_GPIO
  \brief GPIO driver main source file.
*/



/*
 * ####################################
 *              Version No.
 * ####################################
 */

#define IFX_GPIO_VER_MAJOR              1
#define IFX_GPIO_VER_MID                2
#define IFX_GPIO_VER_MINOR              14

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
#include <asm/uaccess.h>
/*ctc*/
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_gpio.h"

/*
 * ####################################
 *              Definition
 * ####################################
 */

#define GPIO_IOCTL_OPR(func_clear, func_set, ptr_parm, user_arg, ret)           do {    \
    ret = copy_from_user((void *)(ptr_parm), (void *)(user_arg), sizeof(*(ptr_parm)));  \
    if ( ret == IFX_SUCCESS ) {                                                         \
        (ptr_parm)->module |= GPIO_PIN_STATUS_APP_MASK;                                 \
        if ( (ptr_parm)->value == 0 )                                                   \
            ret = (func_clear)(IFX_GPIO_PIN_ID((ptr_parm)->port, (ptr_parm)->pin),      \
                             (ptr_parm)->module);                                       \
        else                                                                            \
            ret = (func_set)(IFX_GPIO_PIN_ID((ptr_parm)->port, (ptr_parm)->pin),        \
                           (ptr_parm)->module);                                         \
    }                                                                                   \
} while ( 0 )

#define GPIO_FUNC_PIN_ID_CHECK(pin)                     do {        \
    if ( pin < 0 || pin > g_max_pin_id ) {                          \
        err("Invalid port pin ID %d!", pin);                        \
        return IFX_ERROR;                                           \
    }                                                               \
} while ( 0 )

#define GPIO_FUNC_MODULE_ID_CHECK(module_id)            do {                        \
    if ( (module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) <= IFX_GPIO_MODULE_MIN        \
        || (module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) >= IFX_GPIO_MODULE_MAX ) {  \
        err("Invalid module ID %d!", module_id & GPIO_PIN_STATUS_MODULE_ID_MASK);   \
        return IFX_ERROR;                                                           \
    }                                                                               \
} while ( 0 )

#define GPIO_FUNC_PIN_ID_BREAKDOWN(pin_id, port, pin)   do {        \
    port = IFX_GPIO_PIN_ID_TO_PORT(pin_id);                         \
    pin = IFX_GPIO_PIN_ID_TO_PIN(pin_id);                           \
} while ( 0 )

#if 1 /*ctc*/
#define GPIO_FUNC_OPR(x)                                do {        \
    int port;                                                       \
    unsigned long sys_flags;                                        \
                                                                    \
    if ( g_max_pin_id >= 0 ) {                                      \
        GPIO_FUNC_PIN_ID_CHECK(pin);                                \
        GPIO_FUNC_PIN_ID_BREAKDOWN(pin, port, pin);                 \
		if (module_id != IFX_GPIO_PIN_AVAILABLE || g_gpio_port_priv[port].pin_status[pin] != IFX_GPIO_PIN_AVAILABLE) \
	        GPIO_FUNC_MODULE_ID_CHECK(module_id);                   \
                                                                    \
        spin_lock_irqsave(&g_gpio_lock, sys_flags);                 \
        if ( (g_gpio_port_priv[port].pin_status[pin] & GPIO_PIN_STATUS_MODULE_ID_MASK) == (module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) ) {  \
            { x }                                                   \
            spin_unlock_irqrestore(&g_gpio_lock, sys_flags);        \
            return IFX_SUCCESS;                                     \
        }                                                           \
        spin_unlock_irqrestore(&g_gpio_lock, sys_flags);            \
    }                                                               \
    else {                                                          \
        GPIO_FUNC_PIN_ID_BREAKDOWN(pin, port, pin);                 \
        { x }                                                       \
        return IFX_SUCCESS;                                         \
    }                                                               \
    return IFX_ERROR;                                               \
} while ( 0 )
#else
#define GPIO_FUNC_OPR(x)                                do {        \
    int port;                                                       \
    unsigned long sys_flags;                                        \
                                                                    \
    if ( g_max_pin_id >= 0 ) {                                      \
        GPIO_FUNC_PIN_ID_CHECK(pin);                                \
        GPIO_FUNC_MODULE_ID_CHECK(module_id);                       \
        GPIO_FUNC_PIN_ID_BREAKDOWN(pin, port, pin);                 \
                                                                    \
        spin_lock_irqsave(&g_gpio_lock, sys_flags);                 \
        if ( (g_gpio_port_priv[port].pin_status[pin] & GPIO_PIN_STATUS_MODULE_ID_MASK) == (module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) ) {  \
            { x }                                                   \
            spin_unlock_irqrestore(&g_gpio_lock, sys_flags);        \
            return IFX_SUCCESS;                                     \
        }                                                           \
        spin_unlock_irqrestore(&g_gpio_lock, sys_flags);            \
    }                                                               \
    else {                                                          \
        GPIO_FUNC_PIN_ID_BREAKDOWN(pin, port, pin);                 \
        { x }                                                       \
        return IFX_SUCCESS;                                         \
    }                                                               \
    return IFX_ERROR;                                               \
} while ( 0 )
#endif

/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  File Operations
 */
static int ifx_gpio_open(struct inode *inode, struct file *filep);
static int ifx_gpio_release(struct inode *inode, struct file *filelp);
static int ifx_gpio_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

/*
 *  Proc File Functions
 */
static inline void proc_file_create(void);
static inline void proc_file_delete(void);
static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int proc_read_dbg(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int proc_write_dbg(struct file *file, const char *buf, unsigned long count, void *data);
static int proc_read_setup(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int proc_read_module(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int proc_write_module(struct file *file, const char *buf, unsigned long count, void *data);
static int proc_read_board(char *buf, char **start, off_t offset, int count, int *eof, void *data);

/*ctc*/
int ifx_gpio_pin_reserve(int pin, int module_id);

/*
 *  Proc File help functions
 */
static inline int stricmp(const char *, const char *);
static inline int strincmp(const char *, const char *, int);
static inline int get_token(char **, char **, int *, int *);
static inline int print_board(char *, struct ifx_gpio_ioctl_pin_config *);


#define	ARC_GPIO_PROC		1	/* ctc */

#ifdef ARC_GPIO_PROC

 #define	ARC_BUTTON_RESET					0x0001
 #define	ARC_BUTTON_RESTART					0x0002
 #define	ARC_BUTTON_WLAN						0x0004
 #define	ARC_BUTTON_WPS						0x0008
 #define	ARC_BUTTON_REGISTER					0x0010
 #define	ARC_BUTTON_PAGE						0x0020
 #define	ARC_BUTTON_DIAG						0x0040
 #define	ARC_BUTTON_EOC						0x0080

 int ifx_gpio_setup_get( int iGpioId, int iField );
 static void proc_help_setup( void );
 static int  proc_write_setup( struct file* file, const char* buffer, unsigned long count, void* data );
 static int arc_gpio_daemon(void * dummy);
 static struct task_struct*	_pstGpioDaemon = 0;
 static unsigned long		_iGpioButtonPressed = 0x0;
 static int					_iGpioDaemonEnable = 1;

#endif

/*
 *  Init Help Functions
 */
static inline int ifx_gpio_version(char *);
static inline void ifx_gpio_early_register(void);

/*
 *  External Variable
 */
extern struct ifx_gpio_ioctl_pin_config g_board_gpio_pin_map[]; //  defined in board specific C file



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

static spinlock_t g_gpio_lock;  //  use spinlock rather than semaphore or mutex
                                //  because most functions run in user context
                                //  and they do not take much time to finish operation

static IFX_GPIO_DECLARE_MODULE_NAME(g_gpio_module_name);

static int g_max_pin_id = -1;   //  console driver may need GPIO driver before it's init

static struct file_operations g_gpio_fops = {
    .open    = ifx_gpio_open,
    .release = ifx_gpio_release,
    .ioctl   = ifx_gpio_ioctl
};

static unsigned int g_dbg_enable = DBG_ENABLE_MASK_ERR;

static struct proc_dir_entry* g_gpio_dir = NULL;



/*
 * ####################################
 *            Local Function
 * ####################################
 */


static int ifx_gpio_open(struct inode *inode, struct file *filep)
{
    return IFX_SUCCESS;
}

static int ifx_gpio_release(struct inode *inode, struct file *filelp)
{
    return IFX_SUCCESS;
}

static int ifx_gpio_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret;
    struct ifx_gpio_ioctl_parm parm;

    //  check magic number
    if ( _IOC_TYPE(cmd) != IFX_GPIO_IOC_MAGIC )
        return -EINVAL;

    //  check read/write right
    if ( ((_IOC_DIR(cmd) & _IOC_WRITE) && !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd)))
        || ((_IOC_DIR (cmd) & _IOC_READ) && !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd))) )
        return -EACCES;

    switch ( cmd ) {
    case IFX_GPIO_IOC_OD:
        GPIO_IOCTL_OPR(ifx_gpio_open_drain_clear, ifx_gpio_open_drain_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_PUDSEL:
        GPIO_IOCTL_OPR(ifx_gpio_pudsel_clear, ifx_gpio_pudsel_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_PUDEN:
        GPIO_IOCTL_OPR(ifx_gpio_puden_clear, ifx_gpio_puden_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_STOFF:
        GPIO_IOCTL_OPR(ifx_gpio_stoff_clear, ifx_gpio_stoff_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_DIR:
        GPIO_IOCTL_OPR(ifx_gpio_dir_in_set, ifx_gpio_dir_out_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_OUTPUT:
        GPIO_IOCTL_OPR(ifx_gpio_output_clear, ifx_gpio_output_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_ALTSEL0:
        GPIO_IOCTL_OPR(ifx_gpio_altsel0_clear, ifx_gpio_altsel0_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_ALTSEL1:
        GPIO_IOCTL_OPR(ifx_gpio_altsel1_clear, ifx_gpio_altsel1_set, &parm, arg, ret);
        break;
    case IFX_GPIO_IOC_INPUT:
        ret = copy_from_user((void *)&parm, (void *)arg, sizeof(parm));
        if ( ret == IFX_SUCCESS ) {
            parm.module |= GPIO_PIN_STATUS_APP_MASK;
            ret = ifx_gpio_input_get(IFX_GPIO_PIN_ID(parm.port, parm.pin), parm.module, &parm.value);
            if ( ret == IFX_SUCCESS )
                ret = copy_to_user((void *)arg, (void *)&parm, sizeof(parm));
        }
        break;
    case IFX_GPIO_IOC_VERSION:
        {
            struct ifx_gpio_ioctl_version version = {
                .major = IFX_GPIO_VER_MAJOR,
                .mid   = IFX_GPIO_VER_MID,
                .minor = IFX_GPIO_VER_MINOR
            };
            ret = copy_to_user((void *)arg, (void *)&version, sizeof(version));
        }
        break;
    case IFX_GPIO_IOC_REGISTER:
        if ( (int)arg > IFX_GPIO_MODULE_MIN && (int)arg < IFX_GPIO_MODULE_MAX )
            ret = ifx_gpio_register((int)arg | GPIO_PIN_STATUS_APP_MASK);
        else
            ret = IFX_ERROR;
        break;
    case IFX_GPIO_IOC_DEREGISTER:
        if ( (int)arg > IFX_GPIO_MODULE_MIN && (int)arg < IFX_GPIO_MODULE_MAX )
            ret = ifx_gpio_deregister((int)arg | GPIO_PIN_STATUS_APP_MASK);
        else
            ret = IFX_ERROR;
        break;
    case IFX_GPIO_IOC_PIN_RESERVE:
    case IFX_GPIO_IOC_PIN_FREE:
        {
            struct ifx_gpio_ioctl_pin_reserve reserve;

            ret = copy_from_user((void *)&reserve, (void *)arg, sizeof(reserve));
            if ( ret == IFX_SUCCESS ) {
                reserve.module_id |= GPIO_PIN_STATUS_APP_MASK;
                if ( cmd == IFX_GPIO_IOC_PIN_RESERVE )
                    ret = ifx_gpio_pin_reserve(reserve.pin, reserve.module_id);
                else
                    ret = ifx_gpio_pin_free(reserve.pin, reserve.module_id);
            }
        }
        break;
    case IFX_GPIO_IOC_PIN_CONFIG:
        {
            int (*func[])(int, int) = {
                ifx_gpio_open_drain_set,    //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET        (1 << 0)
                ifx_gpio_open_drain_clear,  //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR      (1 << 1)
                ifx_gpio_pudsel_set,        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET    (1 << 2)
                ifx_gpio_pudsel_clear,      //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_CLEAR  (1 << 3)
                ifx_gpio_puden_set,         //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET     (1 << 4)
                ifx_gpio_puden_clear,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_CLEAR   (1 << 5)
                ifx_gpio_stoff_set,         //  #define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_SET     (1 << 6)
                ifx_gpio_stoff_clear,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_CLEAR   (1 << 7)
                ifx_gpio_dir_out_set,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT       (1 << 8)
                ifx_gpio_dir_in_set,        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN        (1 << 9)
                ifx_gpio_output_set,        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET    (1 << 10)
                ifx_gpio_output_clear,      //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_CLEAR  (1 << 11)
                ifx_gpio_altsel0_set,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET   (1 << 12)
                ifx_gpio_altsel0_clear,     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR (1 << 13)
                ifx_gpio_altsel1_set,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET   (1 << 14)
                ifx_gpio_altsel1_clear,     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR (1 << 15)
            };
            struct ifx_gpio_ioctl_pin_config config;
            unsigned int bit;
            int i;

            ret = copy_from_user((void *)&config, (void *)arg, sizeof(config));
            if ( ret == IFX_SUCCESS ) {
                config.module_id |= GPIO_PIN_STATUS_APP_MASK;
                for ( bit = IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET, i = 0; bit <= IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR; bit <<= 1, i++ )
                    if ( (config.config & bit) ) {
                        ret = func[i](config.pin, config.module_id);
                        if ( ret != IFX_SUCCESS )
                            break;
                    }
            }
        }
        break;
    case IFX_GPIO_IOC_MODULE_QUERY:
        {
            struct ifx_gpio_ioctl_module_query query = {0};
            unsigned int i;

            ret = copy_from_user((void *)&query, (void *)arg, sizeof(query));
            if ( ret == IFX_SUCCESS ) {
                if ( query.module_id >= 0 && query.module_id < NUM_ENTITY(g_gpio_port_priv) ) {
                    query.pin_num = g_gpio_port_priv[query.module_id].pin_num;
                    for ( i = 0; i < query.pin_num && i < g_gpio_port_priv[query.module_id].pin_num; i++ )
                        query.pin[i] = g_gpio_port_priv[query.module_id].pin_status[i];
                    ret = copy_to_user((void *)arg, (void *)&query, sizeof(query));
                }
                else
                    ret = IFX_ERROR;
            }
        }
        break;
    case IFX_GPIO_IOC_PIN_QUERY:
        {
            struct ifx_gpio_ioctl_pin_query query = {0};
            int port, pin;

            ret = copy_from_user((void *)&query, (void *)arg, sizeof(query));
            if ( ret == IFX_SUCCESS ) {
                if ( query.pin >= 0 && query.pin <= g_max_pin_id ) {
                    port = IFX_GPIO_PIN_ID_TO_PORT(query.pin);
                    pin  = IFX_GPIO_PIN_ID_TO_PIN(query.pin);
                    query.module_id = g_gpio_port_priv[port].pin_status[pin];
                    ret = ifx_gpio_input_get(query.pin, query.module_id, &query.input);
                    if ( ret == IFX_SUCCESS )
                        ret = copy_to_user((void *)arg, (void *)&query, sizeof(query));
                }
                else
                    ret = IFX_ERROR;
            }
        }
        break;
    default:
        ret = -EOPNOTSUPP;
    }

    return ret;
}

static inline void proc_file_create(void)
{
	struct proc_dir_entry*	res;
	/*ctc*/
	struct proc_dir_entry*	proc_file_conf;

    g_gpio_dir = proc_mkdir("driver/ifx_gpio", NULL);

    create_proc_read_entry("version",
                            0,
                            g_gpio_dir,
                            proc_read_version,
                            NULL);

    res = create_proc_entry("dbg",
                            0,
                            g_gpio_dir);
    if ( res != NULL ) {
        res->read_proc  = proc_read_dbg;
        res->write_proc = proc_write_dbg;
    }

  /*ctc*/
  #ifndef ARC_GPIO_PROC
    create_proc_read_entry("setup",
                            0,
                            g_gpio_dir,
                            proc_read_setup,
                            NULL);
  #else
	proc_file_conf = create_proc_entry("setup", 0666, g_gpio_dir);
	if (proc_file_conf == NULL) {
		printk( "error creating entry in /proc/driver/ifx_gpio/setup\n" );
	} else {
		proc_file_conf->write_proc = proc_write_setup;
		proc_file_conf->read_proc  = proc_read_setup;
	}
	if ( _pstGpioDaemon )
		kthread_stop( _pstGpioDaemon );
	_pstGpioDaemon = kthread_create( arc_gpio_daemon, 0, "gpio-daemon");
	if (IS_ERR(_pstGpioDaemon)) {
		_pstGpioDaemon = 0;
		printk(KERN_WARNING "Unable to start GPIO daemon: %ld\n", PTR_ERR(_pstGpioDaemon));
	} else {
		wake_up_process(_pstGpioDaemon);
	}
  #endif

    res = create_proc_entry("module",
                            0,
                            g_gpio_dir);
    if ( res != NULL ) {
        res->read_proc  = proc_read_module;
        res->write_proc = proc_write_module;
    }

    create_proc_read_entry("board",
                            0,
                            g_gpio_dir,
                            proc_read_board,
                            NULL);
}

static inline void proc_file_delete(void)
{
    remove_proc_entry("board", g_gpio_dir);

    remove_proc_entry("module", g_gpio_dir);

    remove_proc_entry("setup", g_gpio_dir);

    remove_proc_entry("dbg", g_gpio_dir);

    remove_proc_entry("version", g_gpio_dir);

    remove_proc_entry("driver/ifx_gpio", NULL);

  #if ARC_GPIO_PROC == 1 /* ctc */
	if ( _pstGpioDaemon ) {
		kthread_stop( _pstGpioDaemon );
		_pstGpioDaemon = 0;
	}
  #endif
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_gpio_version(buf + len);

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

static int proc_read_dbg(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf + len, "error print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ERR)              ? "enabled" : "disabled");
    len += sprintf(buf + len, "debug print      - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT)      ? "enabled" : "disabled");
    len += sprintf(buf + len, "assert           - %s\n", (g_dbg_enable & DBG_ENABLE_MASK_ASSERT)           ? "enabled" : "disabled");

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

static int proc_write_dbg(struct file *file, const char *buf, unsigned long count, void *data)
{
    static const char *dbg_enable_mask_str[] = {
        " error print",
        " err",
        " debug print",
        " dbg",
        " assert",
        " assert",
        " all"
    };
    static const int dbg_enable_mask_str_len[] = {
        12, 4,
        12, 4,
        7,  7,
        4
    };
    u32 dbg_enable_mask[] = {
        DBG_ENABLE_MASK_ERR,
        DBG_ENABLE_MASK_DEBUG_PRINT,
        DBG_ENABLE_MASK_ASSERT,
        DBG_ENABLE_MASK_ALL
    };

    char str[2048];
    char *p;

    int len, rlen;

    int f_enable = 0;
    int i;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

    if ( strincmp(p, "enable", 6) == 0 ) {
        p += 6;
        f_enable = 1;
    }
    else if ( strincmp(p, "disable", 7) == 0 ) {
        p += 7;
        f_enable = -1;
    }
    else if ( strincmp(p, "help", 4) == 0 || *p == '?' )
        printk("echo <enable/disable> [err/dbg/assert/all] > /proc/driver/ifx_gpio/dbg\n");

    if ( f_enable ) {
        if ( *p == 0 ) {
            if ( f_enable > 0 )
                g_dbg_enable |= DBG_ENABLE_MASK_ALL;
            else
                g_dbg_enable &= ~DBG_ENABLE_MASK_ALL;
        }
        else {
            do {
                for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ )
                    if ( strincmp(p, dbg_enable_mask_str[i], dbg_enable_mask_str_len[i]) == 0 ) {
                        if ( f_enable > 0 )
                            g_dbg_enable |= dbg_enable_mask[i >> 1];
                        else
                            g_dbg_enable &= ~dbg_enable_mask[i >> 1];
                        p += dbg_enable_mask_str_len[i];
                        break;
                    }
            } while ( i < NUM_ENTITY(dbg_enable_mask_str) );
        }
    }

    return count;
}

/*
 *  proc_read_setup - Create proc file output
 *  @buf:      Buffer to write the string to
 *  @start:    not used (Linux internal)
 *  @offset:   not used (Linux internal)
 *  @count:    not used (Linux internal)
 *  @eof:      Set to 1 when all data is stored in buffer
 *  @data:     not used (Linux internal)
 *
 *  This function creates the output for the port proc file, by reading
 *  all appropriate registers and displaying the content as a table.
 *
 *      Return Value:
 *  @len = Lenght of data in buffer
 */
static int proc_read_setup(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    static char *gpio_suffix_str[] = {
        "OUT",
        "IN",
        "DIR",
        "ALTSEL0",
        "ALTSEL1",
        "OD",
        "STOFF",
        "PUDSEL",
        "PUDEN",
    };

    int len = 0;

    unsigned long reg_val;
    int i, j, k;

    len += sprintf(buf + len, "\nIFX GPIO Pin Settings\n");
    for ( i = 0; i < NUM_ENTITY(gpio_suffix_str); i++ ) {
        if ( i != 0 )
            len += sprintf(buf + len, "\n~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        len += sprintf(buf + len, "%-7s: FEDCBA9876543210\n", gpio_suffix_str[i]);
        len += sprintf(buf + len, "-------------------------\n");
        for ( j = 0; j < NUM_ENTITY(g_gpio_port_priv); j++ ) {
            //printk("addr = %08x, %08x\n", (unsigned int)(&g_gpio_port_priv[j].reg.gpio_out + i), (unsigned int)*(&g_gpio_port_priv[j].reg.gpio_out + i));
            if ( *(&g_gpio_port_priv[j].reg.gpio_out + i) )
                reg_val = **(&g_gpio_port_priv[j].reg.gpio_out + i);
            else
                reg_val = 0;
            reg_val &= (1 << g_gpio_port_priv[j].pin_num) - 1;
            len += sprintf(buf + len, "     P%d: ", j);
            for ( k = 15; k >= 0; k-- )
                len += sprintf(buf + len, (reg_val & (1 << k)) ? "X" : " ");
            len += sprintf(buf + len, "\n");
        }
    }

    if ( offset >= len ) {
        *start = buf;
        *eof = 1;
        return 0;
    }
    *start = buf + offset;
    if ( (len -= offset) > count )
        return count;
    *eof = 1;


  #if ARC_GPIO_PROC == 1 /* ctc */
	len += sprintf (buf + len, "Reset button: %d\n", (_iGpioButtonPressed & ARC_BUTTON_RESET ? 1 : 0));
	len += sprintf (buf + len, "WPS button: %d\n", (_iGpioButtonPressed & ARC_BUTTON_WPS ? 1 : 0));
  #endif


    return len;
}

/*
 *  proc_read_module - Create proc file output
 *  @buf:      Buffer to write the string to
 *  @start:    not used (Linux internal)
 *  @offset:   not used (Linux internal)
 *  @count:    not used (Linux internal)
 *  @eof:      Set to 1 when all data is stored in buffer
 *  @data:     not used (Linux internal)
 *
 *  This function creates the output for GPIO module usage.
 *
 *      Return Value:
 *  @len = Lenght of data in buffer
 */
static int proc_read_module(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    long len = 0;

    int module_id;
    int i, j;
    char *p;

    len += sprintf(buf + len,       "\nIFX GPIO Pin Usage\n");

    /* The first part */
    len += sprintf(buf + len,       "    0         1         2         3         4         5         6         7\n");
    len += sprintf(buf + len,       "-------------------------------------------------------------------------------\n");
    for ( i = 0; i < NUM_ENTITY(g_gpio_port_priv); i++ ) {
        len += sprintf(buf + len,   "P%d  ", i);
        for ( j = 0; j < 8; j++ ) {
            if ( j < g_gpio_port_priv[i].pin_num ) {
                module_id = g_gpio_port_priv[i].pin_status[j];
                p = g_gpio_module_name[module_id & GPIO_PIN_STATUS_MODULE_ID_MASK];
            }
            else
                p = "N/A";
            len += sprintf(buf + len, "%-10s", p);
        }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf(buf + len,       "\n");
    len += sprintf(buf + len,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    len += sprintf(buf + len,       "\n");

    /* The second part */
    len += sprintf(buf + len,       "    8         9         10        11        12        13        14        15\n");
    len += sprintf(buf + len,       "-------------------------------------------------------------------------------\n");
    for ( i = 0; i < NUM_ENTITY(g_gpio_port_priv); i++ ) {
        len += sprintf(buf + len,   "P%d  ", i);
        for ( j = 8; j < 16; j++ ) {
            if ( j < g_gpio_port_priv[i].pin_num ) {
                module_id = g_gpio_port_priv[i].pin_status[j];
                p = g_gpio_module_name[module_id & GPIO_PIN_STATUS_MODULE_ID_MASK];
            }
            else
                p = "N/A";
            len += sprintf(buf + len, "%-10s", p);
        }
        len += sprintf(buf + len, "\n");
    }
    len += sprintf (buf + len, "\n");

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

static int proc_write_module(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len;
    char *p1, *p2;
    int colon = 1;

    unsigned int cmd = 0;
    char *str_cmd[] = {"register", "reg", "deregister", "dereg", "unreg"};
#define PROC_GPIO_REGISTER_MODULE       1
#define PROC_GPIO_DEREGISTER_MODULE     2
    unsigned int str_to_cmd_map[] = {PROC_GPIO_REGISTER_MODULE, PROC_GPIO_REGISTER_MODULE, PROC_GPIO_DEREGISTER_MODULE, PROC_GPIO_DEREGISTER_MODULE, PROC_GPIO_DEREGISTER_MODULE};
    int i;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        for ( i = 0; i < NUM_ENTITY(str_cmd); i++ )
            if ( stricmp(p1, str_cmd[i]) == 0 ) {
                cmd = str_to_cmd_map[i];
                goto PROC_WRITE_MODULE_CONTINUE_LOOP;
            }

        if ( cmd ) {
            for ( i = IFX_GPIO_MODULE_MIN + 1; i < NUM_ENTITY(g_gpio_module_name); i++ )
                if ( stricmp(p1, g_gpio_module_name[i]) == 0 ) {
                    if ( cmd == PROC_GPIO_REGISTER_MODULE ) {
                        if ( ifx_gpio_register(i | GPIO_PIN_STATUS_APP_MASK) != IFX_SUCCESS )
                            printk("failed in registering module \"%s\"\n", g_gpio_module_name[i]);
                    }
                    else
                        ifx_gpio_deregister(i | GPIO_PIN_STATUS_APP_MASK);
                    break;
                }
            if ( i == NUM_ENTITY(g_gpio_module_name) )
                printk("unkown module name, please read help with command \"echo help > /proc/driver/ifx_gpio/module\"\n");
        }

PROC_WRITE_MODULE_CONTINUE_LOOP:
        p1 = p2;
        colon = 1;
    }

    if ( !cmd ) {
        printk("Usage:\n");
        printk("  echo <register|deregister> <module name> > /proc/driver/ifx_gpio/module\n");
        printk("Module Name List:\n");
        for ( i = IFX_GPIO_MODULE_MIN + 1; i < NUM_ENTITY(g_gpio_module_name); i++ )
            printk("  %s\n", g_gpio_module_name[i]);
    }

    return count;
}

static int proc_read_board(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = offset + count;
    char *pstr;
    char str[1024];
    int llen;

    struct ifx_gpio_ioctl_pin_config *p;
    int item_no;
    int i;

    pstr = *start = buf;

    for ( i = IFX_GPIO_MODULE_MIN + 1; i < IFX_GPIO_MODULE_MAX; i++ ) {
        p = g_board_gpio_pin_map;
        item_no = 0;
        while ( p->module_id != IFX_GPIO_PIN_AVAILABLE ) {
            if ( (p->module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) == i ) {
                llen = item_no++ == 0 ? sprintf(str, "%s\n", g_gpio_module_name[i]) : 0;
                llen += print_board(str + llen, p);
                if ( len <= offset && len + llen > offset ) {
                    memcpy(pstr, str + offset - len, len + llen - offset);
                    pstr += len + llen - offset;
                }
                else if ( len > offset ) {
                    memcpy(pstr, str, llen);
                    pstr += llen;
                }
                len += llen;
                if ( len >= len_max )
                    goto PROC_READ_BOARD_OVERRUN_END;
            }

            p++;
        }
    }

    *eof = 1;

    return len - offset;

PROC_READ_BOARD_OVERRUN_END:

    return len - llen - offset;
}

static inline int stricmp(const char *p1, const char *p2)
{
    int c1, c2;

    while ( *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
    }

    return *p1 - *p2;
}

static inline int strincmp(const char *p1, const char *p2, int n)
{
    int c1 = 0, c2;

    while ( n && *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
        n--;
    }

    return n ? *p1 - *p2 : c1;
}

static inline int get_token(char **p1, char **p2, int *len, int *colon)
{
    int tlen = 0;

    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z') || (**p1 >= '0' && **p1<= '9')) )
    {
        (*p1)++;
        (*len)--;
    }
    if ( !*len )
        return 0;

    if ( *colon )
    {
        *colon = 0;
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            if ( **p2 == ':' )
            {
                *colon = 1;
                break;
            }
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }
    else
    {
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }

    return tlen;
}

static inline int print_board(char *buf, struct ifx_gpio_ioctl_pin_config *p)
{
    char *str_flag[] = {
        "OD(1)",        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET        (1 << 0)
        "OD(0)",        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR      (1 << 1)
        "PUDSEL(1)",    //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET    (1 << 2)
        "PUDSEL(0)",    //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_CLEAR  (1 << 3)
        "PUDEN(1)",     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET     (1 << 4)
        "PUDEN(0)",     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_CLEAR   (1 << 5)
        "STOFF(1)",     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_SET     (1 << 6)
        "STOFF(0)",     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_CLEAR   (1 << 7)
        "DIROUT",       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT       (1 << 8)
        "DIRIN",        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN        (1 << 9)
        "OUTPUT(1)",    //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET    (1 << 10)
        "OUTPUT(0)",    //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_CLEAR  (1 << 11)
        "ALTSEL0(1)",   //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET   (1 << 12)
        "ALTSEL0(0)",   //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR (1 << 13)
        "ALTSEL1(1)",   //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET   (1 << 14)
        "ALTSEL1(0)",   //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR (1 << 15)
    };
    int len = 0;
    unsigned int config;
    int bit;

    len += sprintf(buf + len, "  %d.%-2d -", IFX_GPIO_PIN_ID_TO_PORT(p->pin), IFX_GPIO_PIN_ID_TO_PIN(p->pin));

    for ( bit = clz(config = p->config); bit >= 0; bit = clz(config ^= 1 << bit) )
        len += sprintf(buf + len, " %s", str_flag[bit]);
    len += sprintf(buf + len, "\n");

    return len;
}

static inline int ifx_gpio_version(char *buf)
{
    return ifx_drv_ver(buf, "GPIO", IFX_GPIO_VER_MAJOR, IFX_GPIO_VER_MID, IFX_GPIO_VER_MINOR);
}

static inline void ifx_gpio_early_register(void)
{
    int i;

    for ( i = IFX_GPIO_MODULE_MIN + 1; i < IFX_GPIO_MODULE_MAX; i++ )
        ifx_gpio_register(i | IFX_GPIO_MODULE_EARLY_REGISTER);
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*!
  \fn       int ifx_gpio_register(int module_id)
  \brief    Register GPIO pins for module ID.

            This function search GPIO configuration table defined in board dependant
            file. Each entry with given module ID will be reserved and configured.

  \param    module_id     ID of module to be registered.
  \return   IFX_SUCCESS   Register successfully.
  \return   IFX_ERROR     Register fail.
  \ingroup  IFX_GPIO_API
 */
int ifx_gpio_register(int module_id)
{
    int (*func[])(int, int) = {
        ifx_gpio_open_drain_set,    //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET        (1 << 0)
        ifx_gpio_open_drain_clear,  //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OD_CLEAR      (1 << 1)
        ifx_gpio_pudsel_set,        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_SET    (1 << 2)
        ifx_gpio_pudsel_clear,      //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDSEL_CLEAR  (1 << 3)
        ifx_gpio_puden_set,         //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_SET     (1 << 4)
        ifx_gpio_puden_clear,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_PUDEN_CLEAR   (1 << 5)
        ifx_gpio_stoff_set,         //  #define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_SET     (1 << 6)
        ifx_gpio_stoff_clear,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_STOFF_CLEAR   (1 << 7)
        ifx_gpio_dir_out_set,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_OUT       (1 << 8)
        ifx_gpio_dir_in_set,        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_DIR_IN        (1 << 9)
        ifx_gpio_output_set,        //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_SET    (1 << 10)
        ifx_gpio_output_clear,      //  #define IFX_GPIO_IOCTL_PIN_CONFIG_OUTPUT_CLEAR  (1 << 11)
        ifx_gpio_altsel0_set,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_SET   (1 << 12)
        ifx_gpio_altsel0_clear,     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL0_CLEAR (1 << 13)
        ifx_gpio_altsel1_set,       //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_SET   (1 << 14)
        ifx_gpio_altsel1_clear,     //  #define IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR (1 << 15)
    };
    struct ifx_gpio_ioctl_pin_config *p = g_board_gpio_pin_map;
    int tmp_module_id = module_id & (GPIO_PIN_STATUS_MODULE_ID_MASK | IFX_GPIO_MODULE_EARLY_REGISTER);
    int early_register_mask = (module_id & IFX_GPIO_MODULE_EARLY_REGISTER) ? ~0 : ~IFX_GPIO_MODULE_EARLY_REGISTER;
    int p_module_id;

    int ret;
    unsigned int bit;
    int i;

/*ctc*/
if ( (module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) < IFX_GPIO_MODULE_MAX )
	printk( "\n======= %s (%d,%s) ======= %s\n"
			, __FUNCTION__
			, module_id & ~(GPIO_PIN_STATUS_APP_MASK|IFX_GPIO_MODULE_EARLY_REGISTER)
			, g_gpio_module_name[ module_id & GPIO_PIN_STATUS_MODULE_ID_MASK ]
			, (module_id & GPIO_PIN_STATUS_APP_MASK) ? "user level application" : ((module_id & IFX_GPIO_MODULE_EARLY_REGISTER) ? "kernel init" : "kernel module")
		);
else
	printk( "\n======= %s (%d,NA) ======= %s\n"
			, __FUNCTION__
			, module_id & ~(GPIO_PIN_STATUS_APP_MASK|IFX_GPIO_MODULE_EARLY_REGISTER)
			, (module_id & GPIO_PIN_STATUS_APP_MASK) ? "user level application" : ((module_id & IFX_GPIO_MODULE_EARLY_REGISTER) ? "kernel init" : "kernel module")
		);
/*****/
    while ( p->module_id != IFX_GPIO_PIN_AVAILABLE ) {
        p_module_id = p->module_id & early_register_mask;
        if ( tmp_module_id == p_module_id ) {
            ret = ifx_gpio_pin_reserve(p->pin, module_id);
            if ( ret != IFX_SUCCESS )
                goto IFX_GPIO_REGISTER_FAIL;
            for ( bit = IFX_GPIO_IOCTL_PIN_CONFIG_OD_SET, i = 0; bit <= IFX_GPIO_IOCTL_PIN_CONFIG_ALTSEL1_CLEAR; bit <<= 1, i++ )
                if ( (p->config & bit) ) {
                    ret = func[i](p->pin, module_id);
                    if ( ret != IFX_SUCCESS )
                        goto IFX_GPIO_REGISTER_FAIL;
                }
        }

        p++;
    }

    return IFX_SUCCESS;

IFX_GPIO_REGISTER_FAIL:
    ifx_gpio_deregister(module_id);
    return ret;
}
EXPORT_SYMBOL(ifx_gpio_register);

/*!
  \fn       int ifx_gpio_deregister(int module_id)
  \brief    Free GPIO pins for module ID.

            This function search GPIO configuration table defined in board dependant
            file. Each entry with given module ID will be freed.

  \param    module_id     ID of module to be freed.
  \return   IFX_SUCCESS   Register successfully.
  \return   IFX_ERROR     Register fail.
  \ingroup  IFX_GPIO_API
 */
int ifx_gpio_deregister(int module_id)
{
    int tmp_module_id = module_id & GPIO_PIN_STATUS_MODULE_ID_MASK;
    int i, j;

    for ( i = 0; i < NUM_ENTITY(g_gpio_port_priv); i++ ) {
        for ( j = 0; j < g_gpio_port_priv[i].pin_num; j++ ) {
            if ( tmp_module_id == (g_gpio_port_priv[i].pin_status[j] & GPIO_PIN_STATUS_MODULE_ID_MASK) )
                ifx_gpio_pin_free(IFX_GPIO_PIN_ID(i, j), module_id);
        }
    }

    return IFX_SUCCESS;
}
EXPORT_SYMBOL(ifx_gpio_deregister);

/*!
  \fn       int ifx_gpio_pin_reserve(int pin, int module_id)
  \brief    Reserve pin for usage.

            This function reserves a given pin for usage by the given module.

  \param    pin        Pin to be reserved.
  \param    module_id  Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS   Pin reserved successfully.
  \return   IFX_ERROR     Invalid pin ID or already used.
  \ingroup  IFX_GPIO_API
*/
int ifx_gpio_pin_reserve(int pin, int module_id)
{
    int port;
    unsigned long sys_flags;

/*ctc*/
if ( (module_id & GPIO_PIN_STATUS_MODULE_ID_MASK) < IFX_GPIO_MODULE_MAX )
	printk( "\n======= %s %d (%d,%s) ======= %s\n"
			, __FUNCTION__
			, pin
			, module_id & ~(GPIO_PIN_STATUS_APP_MASK|IFX_GPIO_MODULE_EARLY_REGISTER)
			, g_gpio_module_name[ module_id & GPIO_PIN_STATUS_MODULE_ID_MASK ]
			, (module_id & GPIO_PIN_STATUS_APP_MASK) ? "user level application" : ((module_id & IFX_GPIO_MODULE_EARLY_REGISTER) ? "kernel init" : "kernel module")
		);
else
	printk( "\n======= %s %d (%d,NA) ======= %s\n"
			, __FUNCTION__
			, pin
			, module_id & ~(GPIO_PIN_STATUS_APP_MASK|IFX_GPIO_MODULE_EARLY_REGISTER)
			, (module_id & GPIO_PIN_STATUS_APP_MASK) ? "user level application" : ((module_id & IFX_GPIO_MODULE_EARLY_REGISTER) ? "kernel init" : "kernel module")
		);
/*****/

    if ( g_max_pin_id >= 0 ) {
        GPIO_FUNC_PIN_ID_CHECK(pin);
        GPIO_FUNC_MODULE_ID_CHECK(module_id);
        GPIO_FUNC_PIN_ID_BREAKDOWN(pin, port, pin);

        spin_lock_irqsave(&g_gpio_lock, sys_flags);
        if ( g_gpio_port_priv[port].pin_status[pin] == IFX_GPIO_PIN_AVAILABLE ) {
            g_gpio_port_priv[port].pin_status[pin] = module_id;
            spin_unlock_irqrestore(&g_gpio_lock, sys_flags);
            return IFX_SUCCESS;
        }
        else
            module_id = g_gpio_port_priv[port].pin_status[pin];
        spin_unlock_irqrestore(&g_gpio_lock, sys_flags);
    }
    else {
        //GPIO_FUNC_PIN_ID_BREAKDOWN(pin, port, pin);
        //g_gpio_port_priv[port].pin_status[pin] = module_id;
        return IFX_SUCCESS;
    }

    err("Pin ID %d (port %d, pin %d) has been reserved by module %s from %s!",
        IFX_GPIO_PIN_ID(port, pin), port, pin,
        g_gpio_module_name[module_id & GPIO_PIN_STATUS_MODULE_ID_MASK],
        (module_id & GPIO_PIN_STATUS_APP_MASK) ? "user level application" : ((module_id & IFX_GPIO_MODULE_EARLY_REGISTER) ? "kernel init" : "kernel module"));

    return IFX_ERROR;
}
EXPORT_SYMBOL(ifx_gpio_pin_reserve);

/*!
  \fn       int ifx_gpio_pin_free(int pin, int module_id)
  \brief    Free pin.

            This function frees a given pin and clears the entry in the usage
            map.

  \param    pin         Pin to be freed.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS   Pin freed successfully.
  \return   IFX_ERROR     Invalid pin ID or not used.
  \ingroup  IFX_GPIO_API
 */
int ifx_gpio_pin_free(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        g_gpio_port_priv[port].pin_status[pin] = IFX_GPIO_PIN_AVAILABLE;
    );
}
EXPORT_SYMBOL(ifx_gpio_pin_free);

/*!
  \fn       int ifx_gpio_open_drain_set(int pin, int module_id)
  \brief    Enable Open Drain for given pin.

            This function enables Open Drain mode for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_open_drain_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_od )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_od);
    );
}
EXPORT_SYMBOL(ifx_gpio_open_drain_set);

/*!
  \fn       int ifx_gpio_open_drain_clear(int pin, int module_id)
  \brief    Disable Open Drain for given pin.

            This function disables Open Drain mode for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_open_drain_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_od )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_od);
    );
}
EXPORT_SYMBOL(ifx_gpio_open_drain_clear);

/*!
  \fn       int ifx_gpio_pudsel_set(int pin, int module_id)
  \brief    Enable Pull-Up resistor for given pin.

            This function enables Pull-Up resistor for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_pudsel_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_pudsel )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_pudsel);
    );
}
EXPORT_SYMBOL(ifx_gpio_pudsel_set);

/*!
  \fn       int ifx_gpio_pudsel_clear(int pin, int module_id)
  \brief    Enable Pull-Down resistor for given pin.

            This function enables Pull-Down resistor for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_pudsel_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_pudsel )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_pudsel);
    );
}
EXPORT_SYMBOL(ifx_gpio_pudsel_clear);

/*!
  \fn       int ifx_gpio_puden_set(int pin, int module_id)
  \brief    Enable Pull-Up/Down feature for given pin.

            This function enables Pull-Up/Down feature for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_puden_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_puden )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_puden);
    );
}
EXPORT_SYMBOL(ifx_gpio_puden_set);

/*!
  \fn       int ifx_gpio_puden_set(int pin, int module_id)
  \brief    Disable Pull-Up/Down feature for given pin.

            This function disables Pull-Up/Down feature for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_puden_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_puden )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_puden);
    );
}
EXPORT_SYMBOL(ifx_gpio_puden_clear);

/*!
  \fn       int ifx_gpio_stoff_set(int pin, int module_id)
  \brief    Enable Schmitt Trigger for given pin.

            This function enables Schmitt Trigger for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_stoff_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_stoff )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_stoff);
    );
}
EXPORT_SYMBOL(ifx_gpio_stoff_set);

/*!
  \fn       int ifx_gpio_stoff_clear(int pin, int module_id)
  \brief    Disable Schmitt Trigger for given pin.

            This function disables Schmitt Trigger for the given pin.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_stoff_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_stoff )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_stoff);
    );
}
EXPORT_SYMBOL(ifx_gpio_stoff_clear);

/*!
  \fn       int ifx_gpio_dir_out_set(int pin, int module_id)
  \brief    Configure given pin for output.

            This function configures the given pin for output.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_dir_out_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_dir )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_dir);
    );
}
EXPORT_SYMBOL(ifx_gpio_dir_out_set);

/*!
  \fn       int ifx_gpio_dir_in_set(int pin, int module_id)
  \brief    Configure given pin for input.

            This function configures the given pin for input.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_dir_in_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_dir )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_dir);
    );
}
EXPORT_SYMBOL(ifx_gpio_dir_in_set);

/*!
  \fn       int ifx_gpio_output_set(int pin, int module_id)
  \brief    Configure given pin to output 1 (high level).

            This function configures the given pin to output 1 (high level).

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_output_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_out )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_out);
    );
}
EXPORT_SYMBOL(ifx_gpio_output_set);

/*!
  \fn       int ifx_gpio_output_clear(int pin, int module_id)
  \brief    Configure given pin to output 0 (low level).

            This function configures the given pin to output 0 (low level).

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_output_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_out )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_out);
    );
}
EXPORT_SYMBOL(ifx_gpio_output_clear);

/*!
  \fn       int ifx_gpio_input_get(int pin, int module_id, int *input)
  \brief    Get input from given pin.

            This function gets input from the given pin.

  \param    pin         Pin to get input.
  \param    module_id   Module ID to identify the owner of a pin.
  \param    input       Pointer to variable to store the input value.

  \return   IFX_SUCCESS Input value got successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_input_get(int pin, int module_id, int *input)
{
    if ( (unsigned int)input < KSEG0 )
        return IFX_ERROR;

    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_in )
            *input = (IFX_REG_R32(g_gpio_port_priv[port].reg.gpio_in) >> pin) & 0x01;
    );
}
EXPORT_SYMBOL(ifx_gpio_input_get);

/*!
  \fn       int ifx_gpio_altsel0_set(int pin, int module_id)
  \brief    Set Alternative Select 0 with 1.

            This function sets Alternative Select 0 of the given pin with 1.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_altsel0_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_altsel0 )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_altsel0);
    );
}
EXPORT_SYMBOL(ifx_gpio_altsel0_set);

/*!
  \fn       int ifx_gpio_altsel0_clear(int pin, int module_id)
  \brief    Clear Alternative Select 0 with 0.

            This function clears Alternative Select 0 of the given pin with 0.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_altsel0_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_altsel0 )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_altsel0);
    );
}
EXPORT_SYMBOL(ifx_gpio_altsel0_clear);

/*!
  \fn       int ifx_gpio_altsel1_set(int pin, int module_id)
  \brief    Set Alternative Select 1 with 1.

            This function sets Alternative Select 0 of the given pin with 1.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_altsel1_set(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_altsel1 )
            IFX_REG_W32_MASK(0, 1 << pin, g_gpio_port_priv[port].reg.gpio_altsel1);
    );
}
EXPORT_SYMBOL (ifx_gpio_altsel1_set);

/*!
  \fn       int ifx_gpio_altsel1_clear(int pin, int module_id)
  \brief    Clear Alternative Select 1 with 0.

            This function clears Alternative Select 0 of the given pin with 0.

  \param    pin         Pin to be configured.
  \param    module_id   Module ID to identify the owner of a pin.

  \return   IFX_SUCCESS Pin configured successfully.
  \return   IFX_ERROR   Invalid pin ID or not used.
  \ingroup IFX_GPIO_API
 */
int ifx_gpio_altsel1_clear(int pin, int module_id)
{
    GPIO_FUNC_OPR(
        if ( g_gpio_port_priv[port].reg.gpio_altsel1 )
            IFX_REG_W32_MASK(1 << pin, 0, g_gpio_port_priv[port].reg.gpio_altsel1);
    );
}
EXPORT_SYMBOL(ifx_gpio_altsel1_clear);



#if ARC_GPIO_PROC == 1 /* ctc */

#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/delay.h>

#define	PORT_GPIO_MIN						0
#define	PORT_GPIO_MAX						49 /*g_max_pin_id*/
#define	PORT_MODULE_MAX						IFX_GPIO_MODULE_MAX
//#define	PINS_PER_PORT					16
//#define	IFX_GPIO_PIN_ID_TO_PORT( id )	( (id) / PINS_PER_PORT )
//#define	IFX_GPIO_PIN_ID_TO_PIN( id )	( (id) % PINS_PER_PORT )
#define	IFX_GPIO_USAGE( port, pin )			( g_gpio_port_priv[port].pin_status[pin] /*& ~0x8000000*/ )

#define	IFX_GPIO_BUTTON_MOD_ID				IFX_GPIO_MODULE_BUTTON

#define	IFX_GPIO_TASK_WAIT_TIME				10	/* in milli-seconds */
#define	IFX_GPIO_LED_MOD_ID					IFX_GPIO_MODULE_LED
#define	IFX_LED_PAT_INT_DEF					100		/* wait time 1 second */
#define	IFX_LED_PAT_DEF						0x01	/* default LED pattern */
#define	IFX_LED_PAT_BIT_DEF					2		/* default LED pattern bit count */

#define	ARC_LED_IGNORED						0x01
#define	ARC_LED_ON							0x02
#define	ARC_LED_NEW							0x04
#define	ARC_LED_ONE_SHOT					0x08
#define	ARC_LED_BUS_ADDR					(0xB6000200)

#define	BUS_LED_NUM						16

typedef struct {
	unsigned long	lPattern;
	unsigned char	iPatternBit;
	unsigned char	iInterval; /* interval of each bit of pattern, in 10 msec */
	unsigned char	iFlags;
	unsigned char	iStartTick; /* which tick the LED start up */
} stLedCb;

static stLedCb			arc_led[ PORT_GPIO_MAX + 1 ];
static stLedCb			arc_led2[ BUS_LED_NUM ];
static unsigned long	giGpioLoopCnt = 0;
static unsigned short	giLedBusOut = 0xffff;


static void proc_help_setup( void )
{
	printk(	"\nGPIO setup:\n"
			"        help                  - show this help message\n"
			"        od-set <gpio>         - set     open drain\n"
			"        od-clear <gpio>       - clear   open drain\n"
			"        pudsel-set <gpio>     - set     pull-up/pull-down select\n"
			"        pudsel-clear <gpio>   - clear   pull-up/pull-down select\n"
			"        puden-set <gpio>      - set     pull-up/pull-down enable\n"
			"        puden-clear <gpio>    - clear   pull-up/pull-down enable\n"
			"        stoff-set <gpio>      - set     schmitt trigger\n"
			"        stoff-clear <gpio>    - clear   schmitt trigger\n"
			"        dir-out <gpio>        - set     direction out\n"
			"        dir-in <gpio>         - set     direction in\n"
			"        dataout-set <gpio>    - set     data output\n"
			"        dataout-clear <gpio>  - clear   data output\n"
			"        datain-get <gpio>     - get     data input\n"
			"        altsel0-set <gpio>    - set     alternate-select-0\n"
			"        altsel0-clear <gpio>  - clear   alternate-select-0\n"
			"        altsel1-set <gpio>    - set     alternate-select-1\n"
			"        altsel1-clear <gpio>  - clear   alternate-select-1\n");
	printk(	"        reserve <gpio> <mod>  - reserve GPIO\n"
			"        free <gpio>           - free    GPIO\n"
			"        show <gpio>           - show    GPIO status\n" );
	printk(	"        pat-show [gpio]       - show    pattern data out settings\n"
			"        pat-en   <gpio>       - enable  pattern data out\n"
			"        pat-dis  <gpio>       - disable pattern data out\n"
			"        pat-pat  <gpio> <pat> - set     pattern data\n"
			"        pat-int  <gpio> <val> - set     interval of each pattern data bit, in 10 msec\n"
			"        pat-os   <gpio>       - set     one-shot pattern\n" );
	printk(	"        button-wait <msec>    - wait    msec mill-seconds until some buttons pressed\n"
			"        button-clear          - clear   button status\n" );
	printk(	"        led-show [led]        - show    LED settings\n"
			"        led-en   <led>        - enable  LED\n"
			"        led-dis  <led>        - disable LED\n"
			"        led-pat  <led> <pat>  - set     LED pattern\n"
			"        led-int  <led> <val>  - set     interval of each LED pattern data bit, in 10 msec\n"
			"        led-os   <led>        - set     one-shot LED pattern\n" );
	printk(	"        enable                - enable  LED & button daemon\n"
			"        disable               - disable LED & button daemon\n" );
	printk(	"  <gpio>: GPIO identifier, %d~%d\n"
			"  <mod>:  function module identifier, %d~%d\n"
			"  <pat> : binary string pattern, 1-32 characters, e.g. 00111, default 01\n"
			"  <val> : value, 1-255, in 20 msec\n"
			"  <led> : LED ID, 0-%d\n"
			, PORT_GPIO_MIN, PORT_GPIO_MAX
			, IFX_GPIO_MODULE_MIN+1, IFX_GPIO_MODULE_MAX-1
			, BUS_LED_NUM-1
		  );
}

static long arc_simple_strtol( const char *cp, char **endp, unsigned int base )
{
	while (*cp==' ' || *cp=='\t')
		cp++;

	if(*cp=='-') {
		if (*(cp+1) < '0' || *(cp+1) > '9')
			return -1;
		return -simple_strtoul(cp+1,endp,base);
	} else {
		if (*cp == '\0')
			return -1;
		return simple_strtoul(cp,endp,base);
	}
}

int gpioIdAndModGet( char* pStr, char** pRes, int* piMod, int* piPort, int* piPin )
{
	int	iGpioId;
	int	iPort, iPin;

	while (*pStr==' ' || *pStr=='\t')
	{
		pStr++;
	}

	iGpioId = 0;

	for (iGpioId=0; isdigit(*pStr); pStr++)
	{
		iGpioId = iGpioId * 10 + (*pStr - '0');
	}

	if (*pStr != ' ' && *pStr != '\t' && *pStr != '\0')
	{
		return -1;
	}

	if (iGpioId < 0 || iGpioId > g_max_pin_id)
		return -1;

	if (pRes != 0)
		*pRes = pStr;

	GPIO_FUNC_PIN_ID_BREAKDOWN( iGpioId, iPort, iPin );

	if (piMod != 0)
		*piMod = g_gpio_port_priv[iPort].pin_status[iPin] & GPIO_PIN_STATUS_MODULE_ID_MASK;

	if (piPort)
		*piPort = iPort;

	if (piPin)
		*piPin = iPin;

	return iGpioId;
}
EXPORT_SYMBOL(gpioIdAndModGet);

static char* ifx_gpio_moduleid2name( int iGpioId )
{
	int			iMod, iAbsMod;
	static char	_sModName[ PORT_MODULE_MAX + 16 ];

	if (iGpioId < PORT_GPIO_MIN || iGpioId > PORT_GPIO_MAX)
	{
		sprintf( _sModName, "na (wrong GPIO %d)", iGpioId );
		return _sModName;
	}

	iMod = g_gpio_port_priv[ IFX_GPIO_PIN_ID_TO_PORT(iGpioId) ].pin_status[ IFX_GPIO_PIN_ID_TO_PIN(iGpioId) ];

	if (iMod == IFX_GPIO_PIN_AVAILABLE)
	{
		sprintf( _sModName, "free (0x%08x)", iMod );
		return _sModName;
	}

	iAbsMod = iMod & ~0x80000000;

	if (iAbsMod <= PORT_MODULE_MAX) {
		sprintf( _sModName, "%s (0x%08x)", g_gpio_module_name[ iAbsMod ], iMod );
		return _sModName;
	}

	sprintf( _sModName, "na (0x%08x)", iMod );
	return _sModName;
}

static void ifx_gpio_data_io( int iGpioId, int bOut )
{
	int		iPort, iPin;
	int		iMod;

	if (iGpioId < PORT_GPIO_MIN || iGpioId > PORT_GPIO_MAX)
		return;

	iPort = IFX_GPIO_PIN_ID_TO_PORT( iGpioId );
	iPin  = IFX_GPIO_PIN_ID_TO_PIN(  iGpioId );

	if (bOut)
		iMod = IFX_GPIO_LED_MOD_ID;
	else
		iMod = IFX_GPIO_BUTTON_MOD_ID;

	if (IFX_GPIO_USAGE( IFX_GPIO_PIN_ID_TO_PORT(iGpioId), IFX_GPIO_PIN_ID_TO_PIN(iGpioId) ) == IFX_GPIO_PIN_AVAILABLE)
		ifx_gpio_pin_reserve( iGpioId, iMod );

	if (IFX_GPIO_USAGE( IFX_GPIO_PIN_ID_TO_PORT(iGpioId), IFX_GPIO_PIN_ID_TO_PIN(iGpioId) ) != iMod)
		return;

	ifx_gpio_altsel0_clear( iGpioId, iMod );
	ifx_gpio_altsel1_clear( iGpioId, iMod );
	ifx_gpio_open_drain_set( iGpioId, iMod );
	ifx_gpio_output_set( iGpioId, iMod );
	if (bOut)
		ifx_gpio_dir_out_set( iGpioId, iMod );
	else
		ifx_gpio_dir_in_set( iGpioId, iMod );
}

/*******************************************************************************
 * Description
 *		set GPIO driver data via /proc/driver/ifx_gpio/setup virtual file.
 *
 ******************************************************************************/
static int proc_write_setup(	struct file*	file,	/* this parameter is usually ignored */
								const char*		buffer,	/* it contains data that is passed to the
															module (at maximum of possible).
															buffer is not in kernel memory(ro) */
								unsigned long	count,	/* says how many bytes should be read */
								void*			data )	/* this parameter could be set to use our own
															buffer (i.e.: using a method for many files */
{
	char*	pPtr;
	int		iGpioId;
	int		iPort, iPin;
	int		iMod;
	int		iCnt, iFrom, iTo, iBit;
	char	sBuf[128];
	long	lVal;

	sBuf[0] = sBuf[sizeof(sBuf)-1] = '\0';

	/* trim the tailing space, tab and LF/CR*/
	if ( count > 0 )
	{
		memset( sBuf, 0, sizeof(sBuf) );

		if (count >= sizeof(sBuf))
			count = sizeof(sBuf) - 1;

		if (copy_from_user(sBuf, buffer, count))
			return count;

		pPtr = (char*)sBuf + count - 1;

		for (; *pPtr==' ' || *pPtr=='\t' || *pPtr=='\n' || *pPtr=='\r'; pPtr++)
		{
			*pPtr = '\0';
		}
	}

	/* od-set */
	if ( strnicmp( sBuf, "od-set", sizeof("od-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("od-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_open_drain_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* od-clear */
	if ( strnicmp( sBuf, "od-clear", sizeof("od-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("od-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_open_drain_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* pudsel-set */
	if ( strnicmp( sBuf, "pudsel-set", sizeof("pudsel-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pudsel-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_pudsel_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* pudsel-clear */
	if ( strnicmp( sBuf, "pudsel-clear", sizeof("pudsel-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pudsel-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_pudsel_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* puden-set */
	if ( strnicmp( sBuf, "puden-set", sizeof("puden-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("puden-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_puden_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* puden-clear */
	if ( strnicmp( sBuf, "puden-clear", sizeof("puden-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("puden-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_puden_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* stoff-set */
	if ( strnicmp( sBuf, "stoff-set", sizeof("stoff-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("stoff-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_stoff_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* stoff-clear */
	if ( strnicmp( sBuf, "stoff-clear", sizeof("stoff-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("stoff-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_stoff_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* dir-out */
	if ( strnicmp( sBuf, "dir-out", sizeof("dir-out")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("dir-out"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_dir_out_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* dir-in */
	if ( strnicmp( sBuf, "dir-in", sizeof("dir-in")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("dir-in"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_dir_in_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* dataout-set */
	if ( strnicmp( sBuf, "dataout-set", sizeof("dataout-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("dataout-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_output_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* dataout-clear */
	if ( strnicmp( sBuf, "dataout-clear", sizeof("dataout-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("dataout-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_output_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* datain-get */
	if ( strnicmp( sBuf, "datain-get", sizeof("datain-get")-1 ) == 0 )
	{
		int	data;
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("datain-get"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_input_get( iGpioId, iMod, &data );
			printk( "          data: 0x%08x\n", data );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* altsel0-set */
	if ( strnicmp( sBuf, "altsel0-set", sizeof("altsel0-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("altsel0-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_altsel0_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* altsel0-clear */
	if ( strnicmp( sBuf, "altsel0-clear", sizeof("altsel0-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("altsel0-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_altsel0_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* altsel1-set */
	if ( strnicmp( sBuf, "altsel1-set", sizeof("altsel1-set")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("altsel1-set"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_altsel1_set( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* altsel1-clear */
	if ( strnicmp( sBuf, "altsel1-clear", sizeof("altsel1-clear")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("altsel1-clear"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_altsel1_clear( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* reserve */
	if ( strnicmp( sBuf, "reserve", sizeof("reserve")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("reserve"), &pPtr, 0, 0, 0 );
		if (iGpioId < 0)
		{
			proc_help_setup();
			return count;
		}
		/* <mod> */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		for (iMod=0; isdigit(*pPtr); pPtr++)
		{
			iMod = iMod * 10 + (*pPtr - '0');
		}
		if (*pPtr != ' ' && *pPtr != '\t' && *pPtr != '\0')
		{
			proc_help_setup();
			return count;
		}
		/**/
		if (iMod>IFX_GPIO_MODULE_MIN && iMod<IFX_GPIO_MODULE_MAX) {
			ifx_gpio_pin_reserve( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* free */
	if ( strnicmp( sBuf, "free", sizeof("free")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("free"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			ifx_gpio_pin_free( iGpioId, iMod );
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* show */
	if ( strnicmp( sBuf, "show", sizeof("show")-1 ) == 0 )
	{
		int	iPort, iPin;
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("show"), 0, &iMod, 0, 0 );
		if (iGpioId >= 0) {
			iPort = IFX_GPIO_PIN_ID_TO_PORT( iGpioId );
			iPin  = IFX_GPIO_PIN_ID_TO_PIN(  iGpioId );
			printk( "GPIO %d, port %d pin %d\n", iGpioId, iPort, iPin );
			printk( "module  : %s\n", g_gpio_module_name[ g_gpio_port_priv[iPort].pin_status[iPin] & GPIO_PIN_STATUS_MODULE_ID_MASK] );
			printk( "out     : %c\n", ifx_gpio_setup_get(iGpioId,0) == 1 ? '1' : '0');
			printk( "in      : %c\n", ifx_gpio_setup_get(iGpioId,1) == 1 ? '1' : '0');
			printk( "dir     : %c\n", ifx_gpio_setup_get(iGpioId,2) == 1 ? '1' : '0');
			printk( "altsel0 : %c\n", ifx_gpio_setup_get(iGpioId,3) == 1 ? '1' : '0');
			printk( "altsel1 : %c\n", ifx_gpio_setup_get(iGpioId,4) == 1 ? '1' : '0');
			printk( "od      : %c\n", ifx_gpio_setup_get(iGpioId,5) == 1 ? '1' : '0');
			printk( "stoff   : %c\n", ifx_gpio_setup_get(iGpioId,6) == 1 ? '1' : '0');
			printk( "pudsel  : %c\n", ifx_gpio_setup_get(iGpioId,7) == 1 ? '1' : '0');
			printk( "puden   : %c\n", ifx_gpio_setup_get(iGpioId,8) == 1 ? '1' : '0');
		} else {
			proc_help_setup();
		}
		return count;
	}

	/* pat-show */
	if ( strnicmp( sBuf, "pat-show", sizeof("pat-show")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pat-show"), &pPtr, 0, 0, 0 );
		if (iGpioId < 0) {
			iFrom = PORT_GPIO_MIN;
			iTo   = PORT_GPIO_MAX;
		} else {
			iFrom = iTo = iGpioId;
		}
		printk( " gpio  enable  status  interval  pattern (hex)  bit  \n" );
		printk( " ====  ======  ======  ========  =============  ===\n" );
		for ( iCnt = iFrom; iCnt <= iTo; iCnt++ ) {
			printk( "   %2d  %6c  %6s  %8u  0x%08lx     %3d\n"
					, iCnt
					, (arc_led[iCnt].iFlags & ARC_LED_IGNORED) ? 'n' : 'Y'
					, (arc_led[iCnt].iFlags & ARC_LED_ON) ? "on" : "off"
					, arc_led[iCnt].iInterval
					, arc_led[iCnt].lPattern
					, arc_led[iCnt].iPatternBit
				  );
		}
		return count;
	}

	/* pat-en */
	if ( strnicmp( sBuf, "pat-en", sizeof("pat-en")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pat-en"), &pPtr, &iMod, &iPort, &iPin );
		if (iGpioId < 0) {
			proc_help_setup();
			return count;
		}
		/**/
		if (IFX_GPIO_USAGE(iPort,iPin) == IFX_GPIO_PIN_AVAILABLE)
			ifx_gpio_pin_reserve( iGpioId, IFX_GPIO_LED_MOD_ID );
		if (IFX_GPIO_USAGE(iPort,iPin) != IFX_GPIO_LED_MOD_ID) {
			printk( "GPIO is occupied by module: %s\n", ifx_gpio_moduleid2name(iMod) );
			return count;
		}
		ifx_gpio_altsel0_clear( iGpioId, IFX_GPIO_LED_MOD_ID );
		ifx_gpio_altsel1_clear( iGpioId, IFX_GPIO_LED_MOD_ID );
		ifx_gpio_dir_out_set( iGpioId, IFX_GPIO_LED_MOD_ID );
		ifx_gpio_open_drain_set( iGpioId, IFX_GPIO_LED_MOD_ID );
		arc_led[ iGpioId ].iFlags = ARC_LED_NEW;
		return count;
	}

	/* pat-dis */
	if ( strnicmp( sBuf, "pat-dis", sizeof("pat-dis")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pat-dis"), &pPtr, 0, &iPort, &iPin );
		if (iGpioId < 0) {
			proc_help_setup();
			return count;
		}
		/**/
		if (IFX_GPIO_USAGE(iPort,iPin) == IFX_GPIO_LED_MOD_ID) {
			ifx_gpio_output_set( iGpioId, IFX_GPIO_LED_MOD_ID );
			ifx_gpio_pin_free( iGpioId, IFX_GPIO_LED_MOD_ID );
		}
		arc_led[ iGpioId ].iFlags = ARC_LED_IGNORED;
		return count;
	}

	/* pat-pat */
	if ( strnicmp( sBuf, "pat-pat", sizeof("pat-pat")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pat-pat"), &pPtr, 0, 0, 0 );
		if (iGpioId < 0)
		{
			proc_help_setup();
			return count;
		}
		/* <pat> */
		while (*pPtr ==' ')
			pPtr++;
		iBit = 0;
		while (*(pPtr+iBit) =='0' || *(pPtr+iBit) =='1')
			iBit++;
		lVal = arc_simple_strtol( pPtr, 0, 2 );
		if (iBit == 0 || lVal < 0 || lVal >= 0x7FFFFFFF )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led[ iGpioId ].iPatternBit = iBit;
		arc_led[ iGpioId ].lPattern = lVal;
		arc_led[ iGpioId ].iFlags = ARC_LED_NEW;
		return count;
	}

	/* pat-int */
	if ( strnicmp( sBuf, "pat-int", sizeof("pat-int")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pat-int"), &pPtr, 0, 0, 0 );
		if (iGpioId < 0)
		{
			proc_help_setup();
			return count;
		}
		/* <val> */
		lVal = arc_simple_strtol( pPtr, 0, 0 );
		if (lVal < 1 || lVal > 255 )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led[ iGpioId ].iInterval = lVal;
		arc_led[ iGpioId ].iFlags = ARC_LED_NEW;
		return count;
	}

	/* pat-os */
	if ( strnicmp( sBuf, "pat-os", sizeof("pat-os")-1 ) == 0 )
	{
		iGpioId = gpioIdAndModGet( (char*)sBuf + sizeof("pat-os"), &pPtr, 0, 0, 0 );
		if (iGpioId < 0)
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led[ iGpioId ].iFlags = (ARC_LED_NEW|ARC_LED_ONE_SHOT);
		return count;
	}

	/* button-wait */
	if ( strnicmp( sBuf, "button-wait", sizeof("button-wait")-1 ) == 0 )
	{
		/* msec */
		lVal = arc_simple_strtol( (char*)sBuf + sizeof("button-wait"), 0, 0 );
		if (lVal < 0 )
		{
			proc_help_setup();
			return count;
		}
		/**/
		for (; lVal>0; lVal-=IFX_GPIO_TASK_WAIT_TIME) {
			if (_iGpioButtonPressed == 0x0)
				msleep_interruptible( (unsigned int) IFX_GPIO_TASK_WAIT_TIME );
		}
		return count;
	}

	/* button-clear */
	if ( strnicmp( sBuf, "button-clear", sizeof("button-clear")-1 ) == 0 )
	{
		_iGpioButtonPressed = 0x0;
		return count;
	}

	/* led-show */
	if ( strnicmp( sBuf, "led-show", sizeof("led-show")-1 ) == 0 )
	{
		iPort = (int)arc_simple_strtol( (char*)sBuf + sizeof("led-show"), &pPtr, 0 );
		if (iPort < 0 || iPort >= BUS_LED_NUM )
		{
			iFrom = 0;
			iTo   = BUS_LED_NUM - 1;
		} else {
			iFrom = iTo = iPort;
		}
		printk( "current status: 0x%04x\n", giLedBusOut );
		printk( " led  enable  status  interval  pattern (hex)  bit  \n" );
		printk( " ===  ======  ======  ========  =============  ===\n" );
		for ( iCnt = iFrom; iCnt <= iTo; iCnt++ ) {
			printk( "  %2d  %6c  %6s  %8u  0x%08lx     %3d\n"
					, iCnt
					, (arc_led2[iCnt].iFlags & ARC_LED_IGNORED) ? 'n' : 'Y'
					, (arc_led2[iCnt].iFlags & ARC_LED_ON) ? "on" : "off"
					, arc_led2[iCnt].iInterval
					, arc_led2[iCnt].lPattern
					, arc_led2[iCnt].iPatternBit
				  );
		}
		return count;
	}

	/* led-en */
	if ( strnicmp( sBuf, "led-en", sizeof("led-en")-1 ) == 0 )
	{
		iPort = (int)arc_simple_strtol( (char*)sBuf + sizeof("led-en"), &pPtr, 0 );
		if (iPort < 0 || iPort >= BUS_LED_NUM )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led2[ iPort ].iFlags = ARC_LED_NEW;
		return count;
	}

	/* led-dis */
	if ( strnicmp( sBuf, "led-dis", sizeof("led-dis")-1 ) == 0 )
	{
		iPort = (int)arc_simple_strtol( (char*)sBuf + sizeof("led-dis"), &pPtr, 0 );
		if (iPort < 0 || iPort >= BUS_LED_NUM )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led2[ iPort ].iFlags = ARC_LED_IGNORED;
		return count;
	}

	/* led-pat */
	if ( strnicmp( sBuf, "led-pat", sizeof("led-pat")-1 ) == 0 )
	{
		iPort = (int)arc_simple_strtol( (char*)sBuf + sizeof("led-pat"), &pPtr, 0 );
		if (iPort < 0 || iPort >= BUS_LED_NUM )
		{
			proc_help_setup();
			return count;
		}
		/* <pat> */
		while (*pPtr ==' ')
			pPtr++;
		iBit = 0;
		while (*(pPtr+iBit) =='0' || *(pPtr+iBit) =='1')
			iBit++;
		lVal = arc_simple_strtol( pPtr, 0, 2 );
		if (iBit == 0 || lVal < 0 || lVal >= 0x7FFFFFFF )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led2[ iPort ].iPatternBit = iBit;
		arc_led2[ iPort ].lPattern = lVal;
		arc_led2[ iPort ].iFlags = ARC_LED_NEW;
		return count;
	}

	/* led-int */
	if ( strnicmp( sBuf, "led-int", sizeof("led-int")-1 ) == 0 )
	{
		iPort = (int)arc_simple_strtol( (char*)sBuf + sizeof("led-int"), &pPtr, 0 );
		if (iPort < 0 || iPort >= BUS_LED_NUM )
		{
			proc_help_setup();
			return count;
		}
		/* <val> */
		lVal = arc_simple_strtol( pPtr, 0, 0 );
		if (lVal < 1 || lVal > 255 )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led2[ iPort ].iInterval = lVal;
		arc_led2[ iPort ].iFlags = ARC_LED_NEW;
		return count;
	}

	/* led-os */
	if ( strnicmp( sBuf, "led-os", sizeof("led-os")-1 ) == 0 )
	{
		iPort = (int)arc_simple_strtol( (char*)sBuf + sizeof("led-os"), &pPtr, 0 );
		if (iPort < 0 || iPort >= BUS_LED_NUM )
		{
			proc_help_setup();
			return count;
		}
		/**/
		arc_led2[ iPort ].iFlags = (ARC_LED_NEW|ARC_LED_ONE_SHOT);
		return count;
	}

	/* enable */
	if ( strnicmp( sBuf, "enable", sizeof("enable")-1 ) == 0 )
	{
		_iGpioDaemonEnable = 1;
		return count;
	}

	/* disable */
	if ( strnicmp( sBuf, "disable", sizeof("disable")-1 ) == 0 )
	{
		_iGpioDaemonEnable = 1;
		return count;
	}

	/* unknown */
	proc_help_setup();

	return count; /* return how many chars we have consumed, or we will receive them again... */
}

void arc_gpio_led_init( unsigned long iLoop )
{
	int		iGpio;

	for ( iGpio = PORT_GPIO_MIN; iGpio <= PORT_GPIO_MAX; iGpio++ ) {
		arc_led[ iGpio ].lPattern = IFX_LED_PAT_DEF;
		arc_led[ iGpio ].iPatternBit = IFX_LED_PAT_BIT_DEF;
		arc_led[ iGpio ].iInterval = IFX_LED_PAT_INT_DEF;
		arc_led[ iGpio ].iFlags = ARC_LED_IGNORED;
	}
}

void arc_bus_led_out( unsigned short iVal )
{
}

void arc_bus_led_init( unsigned long iLoop )
{
}

void arc_gpio_init( unsigned long iLoop )
{
	/* Reset button */
	ifx_gpio_data_io( 40, 0 );
	/* Restart button */
//	ifx_gpio_data_io( 5, 0 );
	/* WLAN button */
//	ifx_gpio_data_io( 29, 0 );
	/* WPS button */
	ifx_gpio_data_io( 3, 0 );
	/* DECT Register button */
//	ifx_gpio_data_io( 41, 0 );
}

void arc_gpio_led_loop( unsigned long iLoop )
{
	int		iGpio;
	int		iBit;

	for ( iGpio = PORT_GPIO_MIN; iGpio <= PORT_GPIO_MAX; iGpio++ ) {
		/* skip ignored LED */
		if ( arc_led[ iGpio ].iFlags & ARC_LED_IGNORED || arc_led[ iGpio ].iInterval==0 || arc_led[ iGpio ].iPatternBit==0)
			continue;
		/* process newly-updated LED */
		if ( arc_led[ iGpio ].iFlags & ARC_LED_NEW ) {
			arc_led[ iGpio ].iStartTick = giGpioLoopCnt;
			if ( !(arc_led[ iGpio ].lPattern & 0x1) ) {
				ifx_gpio_output_set( iGpio, IFX_GPIO_LED_MOD_ID );
				arc_led[ iGpio ].iFlags &= ~ARC_LED_ON;
			}
			else if ( (arc_led[ iGpio ].lPattern & 0x1) ) {
				ifx_gpio_output_clear( iGpio, IFX_GPIO_LED_MOD_ID );
				arc_led[ iGpio ].iFlags |= ARC_LED_ON;
			}
			arc_led[ iGpio ].iFlags &= ~ARC_LED_NEW;
			continue;
		}
		/* get duby bit */
		iBit = (giGpioLoopCnt-arc_led[ iGpio ].iStartTick) / arc_led[ iGpio ].iInterval;
		iBit %= arc_led[ iGpio ].iPatternBit;
		/* update LED */
		if ( (arc_led[ iGpio ].iFlags & ARC_LED_ON) && !(arc_led[ iGpio ].lPattern & (0x1<<iBit)) ) {
			ifx_gpio_output_set( iGpio, IFX_GPIO_LED_MOD_ID );
			arc_led[ iGpio ].iFlags &= ~ARC_LED_ON;
		}
		else if ( !(arc_led[ iGpio ].iFlags & ARC_LED_ON) && (arc_led[ iGpio ].lPattern & (0x1<<iBit)) ) {
			ifx_gpio_output_clear( iGpio, IFX_GPIO_LED_MOD_ID );
			arc_led[ iGpio ].iFlags |= ARC_LED_ON;
		}
		/* one-shot */
		if ((arc_led[ iGpio ].iFlags & ARC_LED_ONE_SHOT) && iBit==(arc_led[ iGpio ].iPatternBit-1)) {
			arc_led[ iGpio ].iFlags &= ~ARC_LED_ONE_SHOT;
			arc_led[ iGpio ].iFlags |= ARC_LED_IGNORED;
		}
	}
}

void arc_bus_led_loop( unsigned long iLoop )
{
}

void arc_gpio_loop( unsigned long iLoop )
{
	int		iGpioId;
	int		iPort, iPin;
	int		iMod;
	int		data;

	/* Reset button */
	iGpioId = 40;
	iPort = IFX_GPIO_PIN_ID_TO_PORT( iGpioId );
	iPin  = IFX_GPIO_PIN_ID_TO_PIN(  iGpioId );
	iMod  = IFX_GPIO_USAGE( iPort, iPin );
	ifx_gpio_input_get( iGpioId, iMod, &data );
	ifx_gpio_output_clear( iGpioId, iMod );
	if (data == 0) {
		_iGpioButtonPressed |= ARC_BUTTON_RESET;
	  #if 0
		printk( "Reset button press\n" );
	  #endif
	}

	/* WPS button */
	iGpioId = 3;
	iPort = IFX_GPIO_PIN_ID_TO_PORT( iGpioId );
	iPin  = IFX_GPIO_PIN_ID_TO_PIN(  iGpioId );
	iMod  = IFX_GPIO_USAGE( iPort, iPin );
	ifx_gpio_input_get( iGpioId, iMod, &data );
	ifx_gpio_output_clear( iGpioId, iMod );
	if (data == 0) {
		_iGpioButtonPressed |= ARC_BUTTON_WPS;
	  #if 0
		printk( "WPS button press\n" );
	  #endif
	}
}


typedef void (*arc_gpio_func)( unsigned long iLoop );

static arc_gpio_func arc_gpio_init_func[] = {
			  arc_gpio_led_init
			, arc_gpio_init
			, arc_bus_led_init
		};

static arc_gpio_func arc_gpio_loop_func[] = {
			  arc_gpio_led_loop
			, arc_gpio_loop
			, arc_bus_led_loop
		};


static int arc_gpio_daemon(void * dummy)
{
	int		iCnt;
	int		iArrayCnt;

	iArrayCnt = sizeof(arc_gpio_init_func) / sizeof(arc_gpio_init_func[0]);
	for (iCnt=0; iCnt<iArrayCnt; iCnt++) {
		(*arc_gpio_init_func[iCnt])( 0 );
	}

	giGpioLoopCnt = 0;

	iArrayCnt = sizeof(arc_gpio_loop_func) / sizeof(arc_gpio_loop_func[0]);

	while ( 1 ) {

		msleep_interruptible( IFX_GPIO_TASK_WAIT_TIME );

		if ( kthread_should_stop() )
			return 0;

		if (_iGpioDaemonEnable == 0)
			continue;

		for (iCnt=0; iCnt<iArrayCnt; iCnt++) {
			(*arc_gpio_loop_func[iCnt])( giGpioLoopCnt );
		}

		giGpioLoopCnt++;

	}

	return 0;
}

#endif /* #if ARC_GPIO_PROC == 1 */



/************************************************
 *
 * iGpioId: GPIO ID, 0~49 for VR9
 * iField:  data field, 0-out, 1-in, 2-dir, 3-altsel0, 4-altsel1, 5-od, 6-stoff, 7-pudsel, 8-puden
 *
 * return: -1-error, 0-clear, 1-set
 *
 ************************************************/
int ifx_gpio_setup_get( int iGpioId, int iField )
{
	int	iPort, iPin;

	if (iGpioId < 0 || iGpioId > g_max_pin_id)
		return -1;

	if (iField < 0 || iField > 8)
		return -1;

	iPort = IFX_GPIO_PIN_ID_TO_PORT( iGpioId );
	iPin  = IFX_GPIO_PIN_ID_TO_PIN(  iGpioId );
	if ( *(&g_gpio_port_priv[iPort].reg.gpio_out + iField)
	 && **(&g_gpio_port_priv[iPort].reg.gpio_out + iField) & (1<<iPin) )
		return 1;

	return 0;
}
EXPORT_SYMBOL(ifx_gpio_setup_get);


/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

static int __init ifx_gpio_pre_init(void)
{
    int max_port;

    spin_lock_init(&g_gpio_lock);

    max_port = NUM_ENTITY(g_gpio_port_priv);
    g_max_pin_id = (max_port - 1) * IFX_GPIO_PIN_NUMBER_PER_PORT + g_gpio_port_priv[max_port - 1].pin_num - 1;

    return 0;
}

/*
 *  ifx_gpio_init - Initialize port structures
 *
 *  This function initializes the internal data structures of the driver
 *  and will create the proc file entry and device.
 *
 *      Return Value:
 *  @OK = OK
 */
static int __init ifx_gpio_init(void)
{
    int ret;
    char ver_str[128] = {0};

    /* register port device */
    ret = register_chrdev(IFX_GPIO_MAJOR, "ifx_gpio", &g_gpio_fops);
    if ( ret != 0 ) {
        err("Can not register GPIO device - %d", ret);
        return ret;
    }

    proc_file_create();

    ifx_gpio_early_register();

    ifx_gpio_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    return IFX_SUCCESS;
}

static void __exit ifx_gpio_exit(void)
{
    proc_file_delete();

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    if ( unregister_chrdev(IFX_GPIO_MAJOR, "ifx_gpio") ) {
        err("Can not unregister GPIO device (major %d)!", IFX_GPIO_MAJOR);
        return;
    }
#else
    unregister_chrdev(IFX_GPIO_MAJOR, "ifx_gpio");
#endif
}
/* To be called earlier than other drivers as well as kernel modules */
postcore_initcall(ifx_gpio_pre_init);
module_init(ifx_gpio_init);
module_exit(ifx_gpio_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Bing-Tao.Xu@infineon.com");
MODULE_SUPPORTED_DEVICE ("Infineon IFX CPE Reference Board");
MODULE_DESCRIPTION ("Infineon technologies GPIO device driver");

