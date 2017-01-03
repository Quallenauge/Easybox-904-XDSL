/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_proc.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 3 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Proc Filesystem Functions
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 03 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <net/sock.h>
#include <net/ip_vs.h>
#include <asm/time.h>
#include <linux/kallsyms.h>

/*
 *  PPA Specific Head File
 */
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppe_drv_wrapper.h"
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppa_api_proc.h"



/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  Compilation Switch
 */



/*
 * ####################################
 *              Data Type
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */

static int proc_read_dbg(char *, char **, off_t, int, int *, void *);
static int proc_write_dbg(struct file *, const char *, unsigned long, void *);

static int proc_read_hook(char *, char **, off_t, int, int *, void *);
static int proc_write_hook(struct file *, const char *, unsigned long, void *);

static void *proc_read_phys_port_seq_start(struct seq_file *, loff_t *);
static void *proc_read_phys_port_seq_next(struct seq_file *, void *, loff_t *);
static void proc_read_phys_port_seq_stop(struct seq_file *, void *);
static int proc_read_phys_port_seq_show(struct seq_file *, void *);
static int proc_read_phys_port_seq_open(struct inode *, struct file *);

static void *proc_read_netif_seq_start(struct seq_file *, loff_t *);
static void *proc_read_netif_seq_next(struct seq_file *, void *, loff_t *);
static void proc_read_netif_seq_stop(struct seq_file *, void *);
static int proc_read_netif_seq_show(struct seq_file *, void *);
static int proc_read_netif_seq_open(struct inode *, struct file *);
static ssize_t proc_file_write_netif(struct file *, const char __user *, size_t, loff_t *);

static void *proc_read_session_seq_start(struct seq_file *, loff_t *);
static void *proc_read_session_seq_next(struct seq_file *, void *, loff_t *);
static void proc_read_session_seq_stop(struct seq_file *, void *);
static void print_session_ifid(struct seq_file *, char *, uint32_t);
static void print_session_flags(struct seq_file *, char *, uint32_t);
static void print_session_debug_flags(struct seq_file *, char *, uint32_t);
static INLINE int proc_read_routing_session_seq_show(struct seq_file *, void *);
static INLINE int proc_read_mc_group_seq_show(struct seq_file *, void *);
static INLINE int proc_read_bridging_session_seq_show(struct seq_file *, void *);
static int proc_read_session_seq_show(struct seq_file *, void *);
static int proc_read_session_seq_open(struct inode *, struct file *);

#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
  static void *proc_read_directpath_seq_start(struct seq_file *, loff_t *);
  static void *proc_read_directpath_seq_next(struct seq_file *, void *, loff_t *);
  static void proc_read_directpath_seq_stop(struct seq_file *, void *);
  static int proc_read_directpath_seq_show(struct seq_file *, void *);
  static int proc_read_directpath_seq_open(struct inode *, struct file *);
  static ssize_t proc_file_write_directpath(struct file *, const char __user *, size_t, loff_t *);
#endif

//  string process help function
static int stricmp(const char *, const char *);
static int strincmp(const char *, const char *, int);



/*
 * ####################################
 *           Global Variable
 * ####################################
 */

static int g_ppa_proc_dir_flag = 0;
static int g_ppa_api_proc_dir_flag = 0;
static struct proc_dir_entry *g_ppa_proc_dir = NULL;
static struct proc_dir_entry *g_ppa_api_proc_dir = NULL;

static struct seq_operations g_proc_read_phys_port_seq_ops = {
    .start      = proc_read_phys_port_seq_start,
    .next       = proc_read_phys_port_seq_next,
    .stop       = proc_read_phys_port_seq_stop,
    .show       = proc_read_phys_port_seq_show,
};
static struct file_operations g_proc_file_phys_port_seq_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_phys_port_seq_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
};
static uint32_t g_proc_read_phys_port_pos = 0;

static struct seq_operations g_proc_read_netif_seq_ops = {
    .start      = proc_read_netif_seq_start,
    .next       = proc_read_netif_seq_next,
    .stop       = proc_read_netif_seq_stop,
    .show       = proc_read_netif_seq_show,
};
static struct file_operations g_proc_file_netif_seq_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_netif_seq_open,
    .read       = seq_read,
    .write      = proc_file_write_netif,
    .llseek     = seq_lseek,
    .release    = seq_release,
};
static uint32_t g_proc_read_netif_pos = 0;

static struct seq_operations g_proc_read_session_seq_ops = {
    .start      = proc_read_session_seq_start,
    .next       = proc_read_session_seq_next,
    .stop       = proc_read_session_seq_stop,
    .show       = proc_read_session_seq_show,
};
static struct file_operations g_proc_file_session_seq_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_session_seq_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
};
static uint32_t g_proc_read_session_pos = 0;
static uint32_t g_proc_read_session_pos_prev = ~0;


/* ctc */
#ifdef CONFIG_ARC_PPA_FILTER

static void*   proc_read_filter_seq_start(struct seq_file *, loff_t *);
static void*   proc_read_filter_seq_next(struct seq_file *, void *, loff_t *);
static void    proc_read_filter_seq_stop(struct seq_file *, void *);
static int     proc_read_filter_seq_show(struct seq_file *, void *);
static int     proc_read_filter_seq_open(struct inode *, struct file *);
static ssize_t proc_write_filter(struct file *, const char __user *, size_t, loff_t *);

static struct seq_operations g_proc_read_filter_seq_ops = {
    .start      = proc_read_filter_seq_start,
    .next       = proc_read_filter_seq_next,
    .stop       = proc_read_filter_seq_stop,
    .show       = proc_read_filter_seq_show,
};

static struct file_operations g_proc_file_filter_seq_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_filter_seq_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = seq_release,
    .write      = proc_write_filter,
};

#endif /*CONFIG_ARC_PPA_FILTER*/


static char *g_str_dest[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
  static struct seq_operations g_proc_read_directpath_seq_ops = {
      .start      = proc_read_directpath_seq_start,
      .next       = proc_read_directpath_seq_next,
      .stop       = proc_read_directpath_seq_stop,
      .show       = proc_read_directpath_seq_show,
  };
  static struct file_operations g_proc_file_directpath_seq_fops = {
      .owner      = THIS_MODULE,
      .open       = proc_read_directpath_seq_open,
      .read       = seq_read,
      .write      = proc_file_write_directpath,
      .llseek     = seq_lseek,
      .release    = seq_release,
  };
  static uint32_t g_proc_read_directpath_pos = 0;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
extern struct proc_dir_entry proc_root;
#endif



/*
 * ####################################
 *           Extern Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */
struct ppa_dgb_info
{
    char *cmd;
    char *description;
    uint32_t flag;
};

static struct ppa_dgb_info dbg_enable_mask_str[] = {
    {"err",      "error print",                     DBG_ENABLE_MASK_ERR },
    {"dbg",       "debug print",                    DBG_ENABLE_MASK_DEBUG_PRINT},
    {"assert",    "assert",                         DBG_ENABLE_MASK_ASSERT},
    {"uc",        "dump unicast routing session",   DBG_ENABLE_MASK_DUMP_ROUTING_SESSION},
    {"mc",        "dump multicast session",         DBG_ENABLE_MASK_DUMP_MC_GROUP },
    {"br",        "dump bridging session",          DBG_ENABLE_MASK_DUMP_BRIDGING_SESSION},
    {"init",      "dump init",                      DBG_ENABLE_MASK_DUMP_INIT},
    {"qos",       "dbg qos",                        DBG_ENABLE_MASK_QOS},
    {"pwm",       "dbg pwm",                        DBG_ENABLE_MASK_PWM},
    {"mfe",       "dbg multiple field",             DBG_ENABLE_MASK_MFE},
    {"pri",       "test qos queue only",            DBG_ENABLE_MASK_PRI_TEST},

    /*the last one */
    {"all",       "enable all debug",                -1}
};
static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int i;

    if ( off )
        return off;

    for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str) -1; i++ )  //skip -1
    {
        len += ppa_sprintf(page + off + len, "%-10s(%-40s):        %-5s\n", dbg_enable_mask_str[i].cmd, dbg_enable_mask_str[i].description, (g_ppa_dbg_enable & dbg_enable_mask_str[i].flag)  ? "enabled" : "disabled");
    }

    *eof = 1;

    return off + len;
}

static int proc_write_dbg(struct file *file, const char *buf, unsigned long count, void *data)
{
    int len;
    char str[64];
    char *p;

    int f_enable = 0;
    int i;

    len = min(count, (unsigned long)sizeof(str) - 1);
    len -= ppa_copy_from_user(str, buf, len);
    while ( len && str[len - 1] <= ' ' )
        len--;
    str[len] = 0;
    for ( p = str; *p && *p <= ' '; p++, len-- );
    if ( !*p )
        return count;

    if ( strincmp(p, "enable", 6) == 0 )
    {
        p += 6 + 1;  //skip enable and one blank
        f_enable = 1;
    }
    else if ( strincmp(p, "disable", 7) == 0 )
    {
        p += 7 + 1;  //skip disable and one blank
        f_enable = -1;
    }
    else if ( strincmp(p, "help", 4) == 0 || *p == '?' )
    {
         printk("echo <enable/disable> [");
         for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ ) printk("%s/", dbg_enable_mask_str[i].cmd );
         printk("] > /proc/ppa/api/dbg\n");
    }

    if ( f_enable )
    {
        if ( *p == 0 )
        {
            if ( f_enable > 0 )
                g_ppa_dbg_enable |= DBG_ENABLE_MASK_ALL;
            else
                g_ppa_dbg_enable &= ~DBG_ENABLE_MASK_ALL;
        }
        else
        {
            do
            {
                for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ )
                    if ( strincmp(p, dbg_enable_mask_str[i].cmd, strlen(dbg_enable_mask_str[i].cmd) ) == 0 )
                    {
                        if ( f_enable > 0 )
                            g_ppa_dbg_enable |= dbg_enable_mask_str[i].flag;
                        else
                            g_ppa_dbg_enable &= ~dbg_enable_mask_str[i].flag;
                        p += strlen(dbg_enable_mask_str[i].cmd) + 1; //skip one blank
                        break;
                    }
            } while ( i < NUM_ENTITY(dbg_enable_mask_str) );
        }
    }

    return count;
}

static int proc_read_hook(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    if ( off )
        return off;

    if ( !ifx_ppa_is_init() )
        len += ppa_sprintf(page + off + len, "PPA: not init\n");
    else
    {
        len += ppa_sprintf(page + off + len, "PPA routing\n");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_session_add            - %s\n", ppa_hook_session_add_fn ?       "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_session_delete         - %s\n", ppa_hook_session_del_fn ?       "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_inactivity_status      - %s\n", ppa_hook_inactivity_status_fn ? "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_set_session_inactivity - %s\n", ppa_hook_set_inactivity_fn ?    "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_mc_group_update        - %s\n", ppa_hook_mc_group_update_fn ?   "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_mc_group_get           - %s\n", ppa_hook_mc_group_get_fn ?      "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "PPA bridging\n");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_bridge_entry_add               - %s\n", ppa_hook_bridge_entry_add_fn ?               "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_bridge_entry_delete            - %s\n", ppa_hook_bridge_entry_delete_fn ?            "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_set_bridge_entry_timeout       - %s\n", ppa_hook_set_bridge_entry_timeout_fn ?       "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_bridge_entry_inactivity_status - %s\n", ppa_hook_bridge_entry_inactivity_status_fn ? "hooked" : "not hooked");
        len += ppa_sprintf(page + off + len, "  ifx_ppa_bridge_entry_hit_time          - %s\n", ppa_hook_bridge_entry_hit_time_fn ?          "hooked" : "not hooked");
    }

    *eof = 1;

    return off + len;
}

static int proc_write_hook(struct file *file, const char *buf, unsigned long count, void *data)
{
    int len;
    char str[64];
    char *p;
    int i;
    PPA_VERSION ppe_fw={0};

    len = min(count, (unsigned long)sizeof(str) - 1);
    len -= ppa_copy_from_user(str, buf, len);
    while ( len && str[len - 1] <= ' ' )
        len--;
    str[len] = 0;
    for ( p = str; *p && *p <= ' '; p++, len-- );
    if ( !*p )
        return count;

    if ( stricmp(p, "enable") == 0 )
    {
        PPA_IFINFO lanif_info[] = {
            {"eth0",        0},
            {"eth1",        0},
            {"eth0.5",      0},
            {"br0",         0},
        };
        PPA_IFINFO wanif_info[] = {
            {"eth1",        0},
            {"ppp0",        0},
            {"nas0",        0},
            {"nas1",        0},
        };
        PPA_INIT_INFO ppa_init_info = {
            lan_rx_checks : {
                f_ip_verify         : 1,
                f_tcp_udp_verify    : 1,
                f_tcp_udp_err_drop  : 0,
                f_drop_on_no_hit    : 0,
                f_mc_drop_on_no_hit : 0
            },
            wan_rx_checks : {
                f_ip_verify         : 1,
                f_tcp_udp_verify    : 1,
                f_tcp_udp_err_drop  : 0,
                f_drop_on_no_hit    : 0,
                f_mc_drop_on_no_hit : 0
            },
            num_lanifs              : NUM_ENTITY(lanif_info),
            p_lanifs                : lanif_info,
            num_wanifs              : NUM_ENTITY(wanif_info),
            p_wanifs                : wanif_info,
            max_lan_source_entries  : 64,
            max_wan_source_entries  : 64,
            max_mc_entries          : 32,
            max_bridging_entries    : 128,
            add_requires_min_hits   : 2
        };

        uint32_t f_incorrect_fw = 1;

        ppe_fw.index = 1;
        if( ifx_ppa_drv_get_firmware_id(&ppe_fw, 0) != IFX_SUCCESS )
        {
            ppe_fw.index = 0;
            ifx_ppa_drv_get_firmware_id(&ppe_fw, 0);
        }

        switch ( ppe_fw.family )
        {
        case 1: //  Danube/Twinpass
        case 2:
            if ( ppe_fw.itf == 1 )         //  D4
            {
                ppa_init_info.max_lan_source_entries = 128;
                ppa_init_info.max_wan_source_entries = 128;
                ppa_init_info.max_mc_entries         = 32;
                ppa_init_info.max_bridging_entries   = 256;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
            }
            else if ( ppe_fw.itf == 2 )    //  A4
            {
                ppa_init_info.max_lan_source_entries = 64;
                ppa_init_info.max_wan_source_entries = 64;
                ppa_init_info.max_mc_entries         = 32;
                ppa_init_info.max_bridging_entries   = 128;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
                for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
                    if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
                        wanif_info[i].ifname = NULL;
            }
            break;
        case 3: //  Amazon SE
            if ( ppe_fw.itf == 2 )         //  A4
            {
                ppa_init_info.max_lan_source_entries = 44;
                ppa_init_info.max_wan_source_entries = 44;
                ppa_init_info.max_mc_entries         = 32;
                ppa_init_info.max_bridging_entries   = 128;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
                for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
                    if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
                        wanif_info[i].ifname = NULL;
            }
            else if ( ppe_fw.itf == 3 )    //  E4
            {
                ppa_init_info.max_lan_source_entries = 0;
                ppa_init_info.max_wan_source_entries = 88;
                ppa_init_info.max_mc_entries         = 32;
                ppa_init_info.max_bridging_entries   = 128;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
                for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
                    if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
                        wanif_info[i].ifname = "ptm0";
            }
            break;
        case 5: //  AR9/GR9
        case 6:
            if ( ppe_fw.itf == 1 )         //  D5
            {
                ppa_init_info.max_lan_source_entries = 576;
                ppa_init_info.max_wan_source_entries = 576;
                ppa_init_info.max_mc_entries         = 64;
                ppa_init_info.max_bridging_entries   = 0;   //  bridging done by internal switch
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
            }
            else if ( ppe_fw.itf == 4 )    //  A5
            {
                ppa_init_info.max_lan_source_entries = 192;
                ppa_init_info.max_wan_source_entries = 192;
                ppa_init_info.max_mc_entries         = 64;
                ppa_init_info.max_bridging_entries   = 512;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
                    if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
                        wanif_info[i].ifname = NULL;
            }
            else if ( ppe_fw.itf == 5 )    //  E5
            {
                ppa_init_info.max_lan_source_entries = 192;
                ppa_init_info.max_wan_source_entries = 192;
                ppa_init_info.max_mc_entries         = 64;
                ppa_init_info.max_bridging_entries   = 512;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
                    if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
                        wanif_info[i].ifname = "ptm0";
            }
            break;
        case 7:
            if ( ppe_fw.itf == 1 )         //  D5/E5
            {
                ppa_init_info.max_lan_source_entries = 192;
                ppa_init_info.max_wan_source_entries = 192;
                ppa_init_info.max_mc_entries         = 64;
                ppa_init_info.max_bridging_entries   = 0;   //  bridging done by internal switch;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
            }
            else if ( ppe_fw.itf == 4 )         //  A5
            {
                ppa_init_info.max_lan_source_entries = 192;
                ppa_init_info.max_wan_source_entries = 192;
                ppa_init_info.max_mc_entries         = 64;
                ppa_init_info.max_bridging_entries   = 0;   //  bridging done by internal switch;
                f_incorrect_fw = 0;
                for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
                    if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
                        lanif_info[i].ifname = NULL;
            }
            break;
        }

        if ( !f_incorrect_fw )
        {
            PPA_MAX_ENTRY_INFO entry={0};

            ifx_ppa_drv_get_max_entries(&entry, 0);

            ppa_init_info.max_lan_source_entries = entry.max_lan_entries;
            ppa_init_info.max_wan_source_entries = entry.max_wan_entries;
            ppa_init_info.max_mc_entries         = entry.max_mc_entries;
            ppa_init_info.max_bridging_entries   = entry.max_bridging_entries;

            ifx_ppa_exit();
            ifx_ppa_init(&ppa_init_info, 0);
            ifx_ppa_enable(1, 1, 0);
        }
        else
        {
           printk("wrong version:family=%d, and itf=%d\n", ppe_fw.family, ppe_fw.itf );
        }
    }
    else if ( stricmp(p, "disable") == 0 )
    {
        ifx_ppa_enable(0, 0, 0);
        ifx_ppa_exit();
    }
    else
        printk("echo enable/disable > /proc/ppa/api/hook\n");

    return count;
}

static void *proc_read_phys_port_seq_start(struct seq_file *seq, loff_t *ppos)
{
    struct phys_port_info *ifinfo;

    if ( *ppos != 0 )
        (*ppos)--;

    g_proc_read_phys_port_pos = (uint32_t)*ppos;
    if ( ppa_phys_port_start_iteration(&g_proc_read_phys_port_pos, &ifinfo) == IFX_SUCCESS )
    {
        *ppos = g_proc_read_phys_port_pos;
        return ifinfo;
    }
    else
        return NULL;
}

static void *proc_read_phys_port_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
    struct phys_port_info *ifinfo = (struct phys_port_info *)v;

    g_proc_read_phys_port_pos = (uint32_t)*ppos;
    if ( ppa_phys_port_iterate_next(&g_proc_read_phys_port_pos, &ifinfo) == IFX_SUCCESS )
    {
        *ppos = g_proc_read_phys_port_pos;
        return ifinfo;
    }
    else
        return NULL;
}

static void proc_read_phys_port_seq_stop(struct seq_file *seq, void *v)
{
    ppa_phys_port_stop_iteration();
}

static int proc_read_phys_port_seq_show(struct seq_file *seq, void *v)
{
    static const char *str_mode[] = {
        "CPU",
        "LAN",
        "WAN",
        "MIX (LAN/WAN)"
    };
    static const char *str_type[] = {
        "CPU",
        "ATM",
        "ETH",
        "EXT"
    };
    static const char *str_vlan[] = {
        "no VLAN",
        "inner VLAN",
        "outer VLAN",
        "N/A"
    };

    struct phys_port_info *ifinfo = (struct phys_port_info *)v;
    static int extra_itf = 0;

    if ( g_proc_read_phys_port_pos == 1 )
    {
        seq_printf(seq, "Physical Port List\n");
        if ( IFX_PPA_IS_PORT_CPU0_AVAILABLE() )
        {
            seq_printf(seq, "  no. %u\n", ++extra_itf);
            seq_printf(seq, "    mode             = %s\n", str_mode[0]);
            seq_printf(seq, "    type             = %s\n", str_type[0]);
            seq_printf(seq, "    vlan             = %s\n", str_vlan[0]);
            seq_printf(seq, "    port             = %d\n", IFX_PPA_PORT_CPU0);
        }
        if ( IFX_PPA_IS_PORT_ATM_AVAILABLE() )
        {
            seq_printf(seq, "  no. %u\n", ++extra_itf);
            seq_printf(seq, "    mode             = %s\n", str_mode[2]);
            seq_printf(seq, "    type             = %s\n", str_type[1]);
            seq_printf(seq, "    vlan             = %s\n", str_vlan[IFX_PPA_PORT_ATM_VLAN_FLAGS]);
            seq_printf(seq, "    port             = %d\n", IFX_PPA_PORT_ATM);
        }
    }

    seq_printf(seq,     "  no. %u\n", g_proc_read_phys_port_pos + extra_itf);
    seq_printf(seq,     "    next             = %08X\n", (uint32_t)ifinfo->next);
    seq_printf(seq,     "    mode             = %s\n", str_mode[ifinfo->mode]);
    seq_printf(seq,     "    type             = %s\n", str_type[ifinfo->type]);
    seq_printf(seq,     "    vlan             = %s\n", str_vlan[ifinfo->vlan]);
    seq_printf(seq,     "    port             = %d\n", ifinfo->port);
    seq_printf(seq,     "    ifname           = %s\n", ifinfo->ifname);

    return 0;
}

static int proc_read_phys_port_seq_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &g_proc_read_phys_port_seq_ops);
}

static void *proc_read_netif_seq_start(struct seq_file *seq, loff_t *ppos)
{
    struct netif_info *ifinfo;

    g_proc_read_netif_pos = (uint32_t)*ppos;
    if ( ppa_netif_start_iteration(&g_proc_read_netif_pos, &ifinfo) == IFX_SUCCESS )
    {
        return ifinfo;
    }
    else
        return NULL;
}

static void *proc_read_netif_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
    struct netif_info *ifinfo = (struct netif_info *)v;

    ++*ppos;                                //  workaround for wrong display
    g_proc_read_netif_pos = (uint32_t)*ppos;
    if ( ppa_netif_iterate_next(&g_proc_read_netif_pos, &ifinfo) == IFX_SUCCESS )
    {
        return ifinfo;
    }
    else
        return NULL;
}

static void proc_read_netif_seq_stop(struct seq_file *seq, void *v)
{
    ppa_netif_stop_iteration();
}

static int proc_read_netif_seq_show(struct seq_file *seq, void *v)
{
    static const char *str_flag[] = {
        "NETIF_VLAN",               //  0x00000001
        "NETIF_BRIDGE",
        "INVALID",
        "INVALID",
        "NETIF_PHY_ETH",            //  0x00000010
        "NETIF_PHY_ATM",
        "NETIF_PHY_TUNNEL",
        "INVALID",
        "NETIF_BR2684",             //  0x00000100
        "NETIF_EOA",
        "NETIF_IPOA",
        "NETIF_PPPOATM",
        "NETIF_PPPOE",              //  0x00001000
        "NETIF_VLAN_INNER",
        "NETIF_VLAN_OUTER",
        "NETIF_VLAN_CANT_SUPPORT",
        "NETIF_LAN_IF",             //  0x00010000
        "NETIF_WAN_IF",
        "NETIF_PHY_IF_GOT",
        "NETIF_PHYS_PORT_GOT",
        "NETIF_MAC_AVAILABLE",      //  0x00100000
        "NETIF_MAC_ENTRY_CREATED",
    };

    struct netif_info *ifinfo = (struct netif_info *)v;
    int flag;
    unsigned long bit;
    int i;

    if ( g_proc_read_netif_pos == 1 )
        seq_printf(seq, "NetIF List\n");

    seq_printf(seq,     "  no. %u\n", g_proc_read_netif_pos);
    seq_printf(seq,     "    next             = %08X\n", (uint32_t)ifinfo->next);
    seq_printf(seq,     "    count            = %d\n", ppa_atomic_read(&ifinfo->count));
    seq_printf(seq,     "    name             = %s\n", ifinfo->name);
    if ( (ifinfo->flags & NETIF_PHY_IF_GOT) )
        seq_printf(seq, "    phys_netif_name  = %s\n", ifinfo->phys_netif_name);
    else
        seq_printf(seq, "    phys_netif_name  = N/A\n");
    if ( ifinfo->netif != NULL )
        seq_printf(seq, "    netif            = %08X\n", (uint32_t)ifinfo->netif);
    else
        seq_printf(seq, "    netif            = N/A\n");
    if ( (ifinfo->flags & NETIF_MAC_AVAILABLE) )
        seq_printf(seq, "    mac              = %02x:%02x:%02x:%02x:%02x:%02x\n", (u32)ifinfo->mac[0], (u32)ifinfo->mac[1], (u32)ifinfo->mac[2], (u32)ifinfo->mac[3], (u32)ifinfo->mac[4], (u32)ifinfo->mac[5]);
    else
        seq_printf(seq, "    mac              = N/A\n");
    seq_printf(seq,     "    flags            = ");
    flag = 0;
    for ( bit = 1, i = 0; i < sizeof(str_flag) / sizeof(*str_flag); bit <<= 1, i++ )
        if ( (ifinfo->flags & bit) )
        {
            if ( flag++ )
                seq_printf(seq, " | ");
            seq_printf(seq, str_flag[i]);
        }
    if ( flag )
        seq_printf(seq, "\n");
    else
        seq_printf(seq, "NULL\n");
    seq_printf(seq,     "    vlan_layer       = %u\n", ifinfo->vlan_layer);
    if ( (ifinfo->flags & NETIF_VLAN_INNER) )
        seq_printf(seq, "    inner_vid        = 0x%x\n", ifinfo->inner_vid);
    if ( (ifinfo->flags & NETIF_VLAN_OUTER) )
        seq_printf(seq, "    outer_vid        = 0x%x\n", ifinfo->outer_vid);
    if ( (ifinfo->flags & NETIF_PPPOE) )
        seq_printf(seq, "    pppoe_session_id = %u\n", ifinfo->pppoe_session_id);
    if ( (ifinfo->flags & NETIF_PHY_ATM) )
        seq_printf(seq, "    dslwan_qid       = %u (RX), %u (TX)\n", (ifinfo->dslwan_qid >> 8) & 0xFF, ifinfo->dslwan_qid & 0xFF);
    if ( (ifinfo->flags & NETIF_PHYS_PORT_GOT) )
        seq_printf(seq, "    phys_port        = %u\n", ifinfo->phys_port);
    else
        seq_printf(seq, "    phys_port        = N/A\n");
    if ( ifinfo->mac_entry == ~0 )
        seq_printf(seq, "    mac_entry        = N/A\n");
    else
        seq_printf(seq, "    mac_entry        = %u\n", ifinfo->mac_entry);

    if( ifinfo->out_vlan_netif )
        seq_printf(seq, "    out_vlan_if      = %s\n", ppa_get_netif_name(ifinfo->out_vlan_netif) );

    if( ifinfo->in_vlan_netif )
        seq_printf(seq, "    in_vlan_if       = %s\n", ppa_get_netif_name(ifinfo->in_vlan_netif) );

    return 0;
}

static int proc_read_netif_seq_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &g_proc_read_netif_seq_ops);
}

/*ctc*/
static void proc_file_write_netif_help( void )
{
	printk( "add lan <netif> - add LAN interface\n"
			"add wan <netif> - add WAN interface\n"
			"del lan <netif> - delete LAN interface\n"
			"del wan <netif> - delete WAN interface\n"
			"update <netif>  - update interface info\n"
		  );
}

static ssize_t proc_file_write_netif(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int len;
    char str[64];
    char *p;

    PPA_IFINFO ifinfo;
    int f_is_lanif = -1;

    len = min(count, (size_t)sizeof(str) - 1);
    len -= ppa_copy_from_user(str, buf, len);
    while ( len && str[len - 1] <= ' ' )
        len--;
    str[len] = 0;
    for ( p = str; *p && *p <= ' '; p++, len-- );
    if ( !*p )
        return count;

    if ( strincmp(p, "add ", 4) == 0 )
    {
        p += 4;

        while ( *p && *p <= ' ' )
            p++;

        if ( strincmp(p, "lan ", 4) == 0 )
            f_is_lanif = 1;
        else if ( strincmp(p, "wan ",4) == 0 )
            f_is_lanif = 0;

        if ( f_is_lanif >= 0 )
        {
            p += 4;

            while ( *p && *p <= ' ' )
                p++;

            ifinfo.ifname = p;
            ifinfo.if_flags = f_is_lanif ? PPA_F_LAN_IF : 0;
            ifx_ppa_add_if(&ifinfo, 0);
        }
    }
    else if ( strincmp(p, "del ", 4) == 0 )
    {
        p += 4;

        while ( *p && *p <= ' ' )
            p++;

        if ( strincmp(p, "lan ", 4) == 0 )
            f_is_lanif = 1;
        else if ( strincmp(p, "wan ",4) == 0 )
            f_is_lanif = 0;

        if ( f_is_lanif >= 0 )
        {
            p += 4;

            while ( *p && *p <= ' ' )
                p++;

            ifinfo.ifname = p;
            ifinfo.if_flags = f_is_lanif ? PPA_F_LAN_IF : 0;
            ifx_ppa_del_if(&ifinfo, 0);
        }
        else
        {
            ifinfo.ifname = p;
            ifinfo.if_flags = 0;
            ifx_ppa_del_if(&ifinfo, 0);
            ifinfo.if_flags = PPA_F_LAN_IF;
            ifx_ppa_del_if(&ifinfo, 0);
        }
    }
    else if ( strincmp(p, "update ", 7) == 0 )
    {
        p += 7;

        while ( *p && *p <= ' ' )
            p++;

        if ( *p )
        {
            int32_t ret = ppa_netif_update(NULL, p);

		  #if 0 /* ctc */
            if ( ret == IFX_SUCCESS )
                printk("Successfully\n");
            else
                printk("Failed: %d\n", ret);
		  #endif
        }
    }
  #if 1 /* ctc */
	else
		proc_file_write_netif_help();
  #endif

    return count;
}

static void *proc_read_session_seq_start(struct seq_file *seq, loff_t *ppos)
{
    struct session_list_item *p_rt_item;
    struct mc_group_list_item *p_mc_item;
    struct bridging_session_list_item *p_br_item;
    uint32_t type;

    if ( *ppos == 0 )
    {
        g_proc_read_session_pos = 0;
        g_proc_read_session_pos_prev = ~0;
    }
    else
        g_proc_read_session_pos = g_proc_read_session_pos_prev;
    type = g_proc_read_session_pos & 0x30000000;
    g_proc_read_session_pos &= 0x0FFFFFFF;
    switch ( type )
    {
    case 0x00000000:
        if ( ppa_session_start_iteration(&g_proc_read_session_pos, &p_rt_item) == IFX_SUCCESS )
        {
            *ppos = g_proc_read_session_pos;
            return p_rt_item;
        }
        else
        {
            ppa_session_stop_iteration();
            g_proc_read_session_pos = 0;
        }
    case 0x10000000:
        if ( ppa_mc_group_start_iteration(&g_proc_read_session_pos, &p_mc_item) == IFX_SUCCESS )
        {
            g_proc_read_session_pos |= 0x10000000;
            *ppos = g_proc_read_session_pos;
            return p_mc_item;
        }
        else
        {
            ppa_mc_group_stop_iteration();
            g_proc_read_session_pos = 0;
        }
    case 0x20000000:
        if ( ppa_bridging_session_start_iteration(&g_proc_read_session_pos, &p_br_item) == IFX_SUCCESS )
        {
            g_proc_read_session_pos |= 0x20000000;
            *ppos = g_proc_read_session_pos;
            return p_br_item;
        }
        else
        {
            g_proc_read_session_pos |= 0x20000000;
            *ppos = g_proc_read_session_pos;
        }
    }

    return NULL;
}

static void *proc_read_session_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
    uint32_t type;

    g_proc_read_session_pos_prev = (uint32_t)*ppos;
    g_proc_read_session_pos = (uint32_t)*ppos;
    type = g_proc_read_session_pos & 0x30000000;
    g_proc_read_session_pos &= 0x0FFFFFFF;
    switch ( type )
    {
    case 0x00000000:
        if ( ppa_session_iterate_next(&g_proc_read_session_pos, (struct session_list_item **)&v) == IFX_SUCCESS )
        {
            *ppos = g_proc_read_session_pos;
            return v;
        }
        else
        {
            ppa_session_stop_iteration();
            g_proc_read_session_pos = 0;
            if ( ppa_mc_group_start_iteration(&g_proc_read_session_pos, (struct mc_group_list_item **)&v) == IFX_SUCCESS )
            {
                g_proc_read_session_pos |= 0x10000000;
                *ppos = g_proc_read_session_pos;
                return v;
            }
            ppa_mc_group_stop_iteration();
            g_proc_read_session_pos = 0;
            if ( ppa_bridging_session_start_iteration(&g_proc_read_session_pos, (struct bridging_session_list_item **)&v) == IFX_SUCCESS )
            {
                g_proc_read_session_pos |= 0x20000000;
                *ppos = g_proc_read_session_pos;
                return v;
            }
            g_proc_read_session_pos |= 0x20000000;
            *ppos = g_proc_read_session_pos;
            return NULL;
        }
        break;
    case 0x10000000:
        if ( ppa_mc_group_iterate_next(&g_proc_read_session_pos, (struct mc_group_list_item **)&v) == IFX_SUCCESS )
        {
            g_proc_read_session_pos |= 0x10000000;
            *ppos = g_proc_read_session_pos;
            return v;
        }
        else
        {
            ppa_mc_group_stop_iteration();
            g_proc_read_session_pos = 0;
            if ( ppa_bridging_session_start_iteration(&g_proc_read_session_pos, (struct bridging_session_list_item **)&v) == IFX_SUCCESS )
            {
                g_proc_read_session_pos |= 0x20000000;
                *ppos = g_proc_read_session_pos;
                return v;
            }
            g_proc_read_session_pos |= 0x20000000;
            *ppos = g_proc_read_session_pos;
            return NULL;
        }
        break;
    case 0x20000000:
        if ( ppa_bridging_session_iterate_next(&g_proc_read_session_pos, (struct bridging_session_list_item **)&v) == IFX_SUCCESS )
        {
            g_proc_read_session_pos |= 0x20000000;
            *ppos = g_proc_read_session_pos;
            return v;
        }
        else
        {
            g_proc_read_session_pos |= 0x20000000;
            *ppos = g_proc_read_session_pos;
            return NULL;
        }
        break;
    default:
        return NULL;
    }
}

static void proc_read_session_seq_stop(struct seq_file *seq, void *v)
{
    switch ( g_proc_read_session_pos & 0x30000000 )
    {
    case 0x00000000: ppa_session_stop_iteration(); break;
    case 0x10000000: ppa_mc_group_stop_iteration(); break;
    case 0x20000000: ppa_bridging_session_stop_iteration();
    }
}

static void print_session_ifid(struct seq_file *seq, char *str, uint32_t ifid)
{
    const char *pstr =  ifid < NUM_ENTITY(g_str_dest) ? g_str_dest[ifid] : "INVALID";

    seq_printf(seq, str);
    seq_printf(seq, "%u - %s\n", ifid, pstr);
}

static void print_session_flags(struct seq_file *seq, char *str, uint32_t flags)
{
    static const char *str_flag[] = {
        "IS_REPLY",                 //  0x00000001
        "BRIDGING_VCI_CHECK",
        "SESSION_IS_TCP",
        "INVALID",
        "ADDED_IN_HW",              //  0x00000010
        "CAN_NOT_ACCEL",
        "STATIC",
        "DROP",
        "VALID_NAT_IP",             //  0x00000100
        "VALID_NAT_PORT",
        "VALID_NAT_SNAT",
        "INVALID",
        "VALID_VLAN_INS",           //  0x00001000
        "VALID_VLAN_RM",
        "SESSION_VALID_OUT_VLAN_INS",
        "SESSION_VALID_OUT_VLAN_RM",
        "VALID_PPPOE",              //  0x00010000
        "VALID_NEW_SRC_MAC",
        "VALID_MTU",
        "VALID_NEW_DSCP",
        "SESSION_VALID_DSLWAN_QID", //  0x00100000
        "SESSION_TX_ITF_IPOA",
        "SESSION_TX_ITF_PPPOA",
        "INVALID",
        "SRC_MAC_DROP_EN",          //  0x01000000
        "SESSION_TUNNEL_6RD",
        "INVALID",
        "INVALID",
        "LAN_ENTRY",                //  0x10000000
        "WAN_ENTRY",
        "IPV6",
        "INVALID",
    };

    int flag;
    unsigned long bit;
    int i;

    seq_printf(seq, str);

    flag = 0;
    for ( bit = 1, i = 0; bit; bit <<= 1, i++ )
        if ( (flags & bit) )
        {
            if ( flag++ )
                seq_printf(seq, "| ");
            seq_printf(seq, str_flag[i]);
            //seq_printf(seq, " ");
        }
    if ( flag )
        seq_printf(seq, "\n");
    else
        seq_printf(seq, "NULL\n");
}

static void print_session_debug_flags(struct seq_file *seq, char *str, uint32_t flags)
{
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
  #define SESSION_DBG_NOT_REACH_MIN_HITS        0x00000001
  #define SESSION_DBG_ALG                       0x00000002
  #define SESSION_DBG_ZERO_DST_MAC              0x00000004
  #define SESSION_DBG_TCP_NOT_ESTABLISHED       0x00000008
  #define SESSION_DBG_RX_IF_NOT_IN_IF_LIST      0x00000010
  #define SESSION_DBG_TX_IF_NOT_IN_IF_LIST      0x00000020
  #define SESSION_DBG_RX_IF_UPDATE_FAIL         0x00000040
  #define SESSION_DBG_TX_IF_UPDATE_FAIL         0x00000080
  #define SESSION_DBG_SRC_BRG_IF_NOT_IN_BRG_TBL 0x00000100
  #define SESSION_DBG_SRC_IF_NOT_IN_IF_LIST     0x00000200
  #define SESSION_DBG_DST_BRG_IF_NOT_IN_BRG_TBL 0x00000400
  #define SESSION_DBG_DST_IF_NOT_IN_IF_LIST     0x00000800
  #define SESSION_DBG_ADD_PPPOE_ENTRY_FAIL      0x00001000
  #define SESSION_DBG_ADD_MTU_ENTRY_FAIL        0x00002000
  #define SESSION_DBG_ADD_MAC_ENTRY_FAIL        0x00004000
  #define SESSION_DBG_ADD_OUT_VLAN_ENTRY_FAIL   0x00008000
  #define SESSION_DBG_RX_PPPOE                  0x00010000
  #define SESSION_DBG_TX_PPPOE                  0x00020000
  #define SESSION_DBG_TX_BR2684_EOA             0x00040000
  #define SESSION_DBG_TX_BR2684_IPOA            0x00080000
  #define SESSION_DBG_TX_PPPOA                  0x00100000
  #define SESSION_DBG_GET_DST_MAC_FAIL          0x00200000
  #define SESSION_DBG_RX_INNER_VLAN             0x00400000
  #define SESSION_DBG_RX_OUTER_VLAN             0x00800000
  #define SESSION_DBG_TX_INNER_VLAN             0x01000000
  #define SESSION_DBG_TX_OUTER_VLAN             0x02000000
  #define SESSION_DBG_RX_VLAN_CANT_SUPPORT      0x04000000
  #define SESSION_DBG_TX_VLAN_CANT_SUPPORT      0x08000000

    static const char *str_flag[] = {
        "NOT_REACH_MIN_HITS",           //  0x00000001
        "ALG",
        "ZERO_DST_MAC",
        "TCP_NOT_ESTABLISHED",
        "RX_IF_NOT_IN_IF_LIST",         //  0x00000010
        "TX_IF_NOT_IN_IF_LIST",
        "RX_IF_UPDATE_FAIL",
        "TX_IF_UPDATE_FAIL",
        "SRC_BRG_IF_NOT_IN_BRG_TBL",    //  0x00000100
        "SRC_IF_NOT_IN_IF_LIST",
        "DST_BRG_IF_NOT_IN_BRG_TBL",
        "DST_IF_NOT_IN_IF_LIST",
        "ADD_PPPOE_ENTRY_FAIL",         //  0x00001000
        "ADD_MTU_ENTRY_FAIL",
        "ADD_MAC_ENTRY_FAIL",
        "ADD_OUT_VLAN_ENTRY_FAIL",
        "RX_PPPOE",                     //  0x00010000
        "TX_PPPOE",
        "TX_BR2684_EOA",
        "TX_BR2684_IPOA",
        "TX_PPPOA",                     //  0x00100000
        "GET_DST_MAC_FAIL",
        "RX_INNER_VLAN",
        "RX_OUTER_VLAN",
        "TX_INNER_VLAN",                //  0x01000000
        "TX_OUTER_VLAN",
        "RX_VLAN_CANT_SUPPORT",
        "TX_VLAN_CANT_SUPPORT",
        "INVALID",                      //  0x10000000
        "INVALID",
        "INVALID",
        "INVALID",
    };

    int flag;
    unsigned long bit;
    int i;

    seq_printf(seq, str);

    flag = 0;
    for ( bit = 1, i = 0; bit; bit <<= 1, i++ )
        if ( (flags & bit) )
        {
            if ( flag++ )
                seq_printf(seq, "| ");
            seq_printf(seq, str_flag[i]);
            seq_printf(seq, " ");
        }
    if ( flag )
        seq_printf(seq, "\n");
    else
        seq_printf(seq, "NULL\n");
#endif
}

static INLINE int proc_read_routing_session_seq_show(struct seq_file *seq, void *v)
{
    struct session_list_item *p_item = v;
    uint32_t pos = g_proc_read_session_pos & 0x0FFFFFFF;
    int8_t strbuf[64];

    if ( pos == 1 )
        seq_printf(seq, "Session List (Unicast Routing)\n");

    seq_printf(seq,     "  no. %d\n", pos);
    seq_printf(seq,     "    next             = %08X\n", (uint32_t)p_item->next);
    seq_printf(seq,     "    session          = %08X\n", (uint32_t)p_item->session);
    seq_printf(seq,     "    ip_proto         = %u\n",   (uint32_t)p_item->ip_proto);
    seq_printf(seq,     "    ip_tos           = %u\n",   (uint32_t)p_item->ip_tos);
    seq_printf(seq,     "    src_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->src_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
    seq_printf(seq,     "    src_port         = %u\n",   (uint32_t)p_item->src_port);
    seq_printf(seq,     "    src_mac[6]       = %s\n",	 ppa_get_pkt_mac_string(p_item->src_mac, strbuf));
    seq_printf(seq,     "    dst_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->dst_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
    seq_printf(seq,     "    dst_port         = %u\n",   (uint32_t)p_item->dst_port);
    seq_printf(seq,     "    dst_mac[6]       = %s\n",   ppa_get_pkt_mac_string(p_item->dst_mac, strbuf));
    seq_printf(seq,     "    nat_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->nat_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
    seq_printf(seq,     "    nat_port         = %u\n",   (uint32_t)p_item->nat_port);
    seq_printf(seq,     "    num_adds         = %u\n",   (uint32_t)p_item->num_adds);
    if ( (uint32_t)p_item->rx_if < KSEG0 || (uint32_t)p_item->rx_if >= KSEG1 )
        seq_printf(seq,     "    rx_if            = %s (%08X)\n", "N/A", (uint32_t)p_item->rx_if);
    else
    seq_printf(seq,     "    rx_if            = %s (%08X)\n", p_item->rx_if == NULL ? "N/A" : ppa_get_netif_name(p_item->rx_if), (uint32_t)p_item->rx_if);
    if ( (uint32_t)p_item->tx_if < KSEG0 || (uint32_t)p_item->tx_if >= KSEG1 )
        seq_printf(seq,     "    tx_if            = %s (%08X)\n", "N/A(may no ip output hook)", (uint32_t)p_item->tx_if);
    else
    seq_printf(seq,     "    tx_if            = %s (%08X)\n", p_item->tx_if == NULL ? "N/A" : ppa_get_netif_name(p_item->tx_if), (uint32_t)p_item->tx_if);
    seq_printf(seq,     "    timeout          = %u\n",   p_item->timeout);
    seq_printf(seq,     "    last_hit_time    = %u (now %u)\n",   p_item->last_hit_time, ppa_get_time_in_sec());
    seq_printf(seq,     "    new_dscp         = %u\n",   (uint32_t)p_item->new_dscp);
    seq_printf(seq,     "    pppoe_session_id = %u\n",   (uint32_t)p_item->pppoe_session_id);
    seq_printf(seq,     "    new_vci          = %04X\n", (uint32_t)p_item->new_vci);
    seq_printf(seq,     "    out_vlan_tag     = %08X\n", p_item->out_vlan_tag);
    seq_printf(seq,     "    mtu              = %u\n",   p_item->mtu);
    seq_printf(seq,     "    dslwan_qid       = %u (RX), %u (TX)\n",   ((uint32_t)p_item->dslwan_qid >> 8) & 0xFF, (uint32_t)p_item->dslwan_qid & 0xFF);
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
    seq_printf(seq,     "    skb priority     = %02d\n",   p_item->priority);
#endif
    print_session_ifid(seq, "    dest_ifid        = ",   p_item->dest_ifid);

    print_session_flags(seq, "    flags            = ", p_item->flags);

    print_session_debug_flags(seq, "    debug_flags      = ", p_item->debug_flags);

    if ( p_item->routing_entry == ~0 )
        seq_printf(seq, "    routing_entry    = N/A\n");
    else
        seq_printf(seq, "    routing_entry    = %u (%s)\n", p_item->routing_entry & 0x7FFFFFFF, (p_item->routing_entry & 0x80000000) ? "LAN" : "WAN");
    if ( p_item->pppoe_entry == ~0 )
        seq_printf(seq, "    pppoe_entry      = N/A\n");
    else
        seq_printf(seq, "    pppoe_entry      = %d\n", p_item->pppoe_entry);
    if ( p_item->mtu_entry == ~0 )
        seq_printf(seq, "    mtu_entry        = N/A\n");
    else
        seq_printf(seq, "    mtu_entry        = %d\n", p_item->mtu_entry);
    if ( p_item->src_mac_entry == ~0 )
        seq_printf(seq, "    src_mac_entry    = N/A\n");
    else
        seq_printf(seq, "    src_mac_entry    = %d\n", p_item->src_mac_entry);
    if ( p_item->out_vlan_entry == ~0 )
        seq_printf(seq, "    out_vlan_entry   = N/A\n");
    else
        seq_printf(seq, "    out_vlan_entry   = %u\n",   p_item->out_vlan_entry);
    seq_printf(seq, 	"    mips bytes       = %llu\n",   p_item->mips_bytes);
    seq_printf(seq, 	"    hw accel bytes   = %llu\n",   p_item->acc_bytes);

    return 0;
}

static INLINE int proc_read_mc_group_seq_show(struct seq_file *seq, void *v)
{
    struct mc_group_list_item *p_item = v;
    uint32_t pos = g_proc_read_session_pos & 0x0FFFFFFF;
    int i, j;
    uint32_t bit;

    if ( pos == 1 )
        seq_printf(seq,     "Session List (Multicast Routing)\n");

    seq_printf(seq,         "  no. %d\n", pos);
    seq_printf(seq,         "    next             = %08X\n", (uint32_t)p_item->next);
    seq_printf(seq,         "    mode             = %s\n", p_item->bridging_flag ?  "bridging": "routing");
    seq_printf(seq,         "    ip_mc_group      = %u.%u.%u.%u\n", p_item->ip_mc_group.ip.ip >> 24, (p_item->ip_mc_group.ip.ip >> 16) & 0xFF, (p_item->ip_mc_group.ip.ip >> 8) & 0xFF, p_item->ip_mc_group.ip.ip & 0xFF);
    seq_printf(seq,         "    interfaces       = %d\n", p_item->num_ifs);
    for ( i = 0; i < p_item->num_ifs; i++ )
        if ( (p_item->if_mask & (1 << i)) && p_item->netif[i] != NULL )
            seq_printf(seq, "      %d. %16s (TTL %u)\n", i, ppa_get_netif_name(p_item->netif[i]), p_item->ttl[i]);
        else
            seq_printf(seq, "      %d. N/A              (mask %d, netif %s)\n", i, (p_item->if_mask & (1 << i)) ? 1 : 0, p_item->netif[i] ? ppa_get_netif_name(p_item->netif[i]) : "NULL");
    seq_printf(seq,         "    src_interface    = %s\n",   p_item->src_netif ? ppa_get_netif_name(p_item->src_netif) : "N/A");
    seq_printf(seq,         "    new_dscp         = %04X\n", (uint32_t)p_item->new_dscp);
    seq_printf(seq,         "    new_vci          = %04X\n", (uint32_t)p_item->new_vci);
    seq_printf(seq,         "    out_vlan_tag     = %08X\n", p_item->out_vlan_tag);
    seq_printf(seq,         "    dslwan_qid       = %u\n",   (uint32_t)p_item->dslwan_qid);

    for ( i = j = 0, bit = 1; i < sizeof(p_item->dest_ifid) * 8; i++, bit <<= 1 )
        if ( (p_item->dest_ifid & bit) )
        {
            print_session_ifid(seq, "    dest_ifid        = ", i);
        }

    print_session_flags(seq, "    flags            = ", p_item->flags);

    print_session_debug_flags(seq, "    debug_flags      = ", p_item->debug_flags);

    if ( p_item->mc_entry == ~0 )
        seq_printf(seq,     "    mc_entry         = N/A\n");
    else
        seq_printf(seq,     "    mc_entry         = %d\n", p_item->mc_entry);
    if ( p_item->src_mac_entry == ~0 )
        seq_printf(seq,     "    src_mac_entry    = N/A\n");
    else
        seq_printf(seq,     "    src_mac_entry    = %d\n", p_item->src_mac_entry);
    if ( p_item->out_vlan_entry == ~0 )
        seq_printf(seq,     "    out_vlan_entry   = N/A\n");
    else
        seq_printf(seq,     "    out_vlan_entry   = %d\n",   p_item->out_vlan_entry);

    return 0;
}

static INLINE int proc_read_bridging_session_seq_show(struct seq_file *seq, void *v)
{
    struct bridging_session_list_item *p_item = v;
    uint32_t pos = g_proc_read_session_pos & 0x0FFFFFFF;

    if ( pos == 1 )
        seq_printf(seq, "Session List (Bridging)\n");

    seq_printf(seq,     "  no. %d\n", pos);
    seq_printf(seq,     "    next             = %08X\n", (uint32_t)p_item->next);
    seq_printf(seq,     "    mac[6]           = %02x:%02x:%02x:%02x:%02x:%02x\n", (uint32_t)p_item->mac[0], (uint32_t)p_item->mac[1], (uint32_t)p_item->mac[2], (uint32_t)p_item->mac[3], (uint32_t)p_item->mac[4], (uint32_t)p_item->mac[5]);
    seq_printf(seq,     "    netif            = %s (%08X)\n", p_item->netif == NULL ? "N/A" : ppa_get_netif_name(p_item->netif), (uint32_t)p_item->netif);
    seq_printf(seq,     "    vci              = %04X\n", (uint32_t)p_item->vci);
    seq_printf(seq,     "    new_vci          = %04X\n", (uint32_t)p_item->new_vci);
    seq_printf(seq,     "    timeout          = %d\n",   p_item->timeout);
    seq_printf(seq,     "    last_hit_time    = %d\n",   p_item->last_hit_time);
    seq_printf(seq,     "    dslwan_qid       = %d\n",   (uint32_t)p_item->dslwan_qid);

    print_session_ifid(seq, "    dest_ifid        = ",   p_item->dest_ifid);

    print_session_flags(seq, "    flags            = ", p_item->flags);

    print_session_debug_flags(seq, "    debug_flags      = ", p_item->debug_flags);

    if ( p_item->bridging_entry == ~0 )
        seq_printf(seq, "    bridging_entry   = N/A\n");
    else if ( (p_item->bridging_entry & 0x80000000) )
        seq_printf(seq, "    bridging_entry   = %08X\n", p_item->bridging_entry);
    else
        seq_printf(seq, "    bridging_entry   = %d\n", p_item->bridging_entry);

    return 0;
}

static int proc_read_session_seq_show(struct seq_file *seq, void *v)
{
    switch ( g_proc_read_session_pos & 0x30000000 )
    {
    case 0x00000000: return proc_read_routing_session_seq_show(seq, v);
    case 0x10000000: return proc_read_mc_group_seq_show(seq, v);
    case 0x20000000: return proc_read_bridging_session_seq_show(seq, v);
    default:         return 0;
    }
}

static int proc_read_session_seq_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &g_proc_read_session_seq_ops);
}

#ifdef CONFIG_IFX_PPA_API_DIRECTPATH

static void *proc_read_directpath_seq_start(struct seq_file *seq, loff_t *ppos)
{
    struct ppe_directpath_data *info;

    if ( *ppos != 0 )
        (*ppos)--;

    g_proc_read_directpath_pos = (uint32_t)*ppos;
    if ( ppa_directpath_data_start_iteration(&g_proc_read_directpath_pos, &info) == IFX_SUCCESS )
    {
        *ppos = g_proc_read_directpath_pos;
        return info;
    }
    else
        return NULL;
}

static void *proc_read_directpath_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
    struct ppe_directpath_data *info = (struct ppe_directpath_data *)v;

    g_proc_read_directpath_pos = (uint32_t)*ppos;
    if ( ppa_directpath_data_iterate_next(&g_proc_read_directpath_pos, &info) == IFX_SUCCESS )
    {
        *ppos = g_proc_read_directpath_pos;
        return info;
    }
    else
        return NULL;
}

static void proc_read_directpath_seq_stop(struct seq_file *seq, void *v)
{
    ppa_directpath_data_stop_iteration();
}

static int proc_read_directpath_seq_show(struct seq_file *seq, void *v)
{
    struct ppe_directpath_data *info = (struct ppe_directpath_data *)v;
    uint32_t start_ifid;
#if defined(CONFIG_KALLSYMS) && defined(CONFIG_IFX_PPA_API_PROC)
    const char *name;
    unsigned long offset, size;
    char *modname;
    char namebuf[KSYM_NAME_LEN+1];
#endif

    ppa_directpath_get_ifid_range(&start_ifid, NULL);

    if ( g_proc_read_directpath_pos == 1 )
        seq_printf(seq, "Directpath Registry List\n");

    seq_printf(seq,     "  ifid. %u\n", start_ifid + g_proc_read_directpath_pos - 1);
    if ( (info->flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
    {
        seq_printf(seq, "    callback\n");
#if defined(CONFIG_KALLSYMS) && defined(CONFIG_IFX_PPA_API_PROC)
        if ( info->callback.stop_tx_fn && (name = kallsyms_lookup((unsigned long)info->callback.stop_tx_fn, &size, &offset, &modname, namebuf)) )
            seq_printf(seq, "      stop_tx_fn     = %s (%08x)\n", name, (u32)info->callback.stop_tx_fn);
        else
            seq_printf(seq, "      stop_tx_fn     = %08x\n", (u32)info->callback.stop_tx_fn);
        if ( info->callback.start_tx_fn && (name = kallsyms_lookup((unsigned long)info->callback.start_tx_fn, &size, &offset, &modname, namebuf)) )
            seq_printf(seq, "      start_tx_fn    = %s (%08x)\n", name, (u32)info->callback.start_tx_fn);
        else
            seq_printf(seq, "      start_tx_fn    = %08x\n", (u32)info->callback.start_tx_fn);
        if ( info->callback.rx_fn && (name = kallsyms_lookup((unsigned long)info->callback.rx_fn, &size, &offset, &modname, namebuf)) )
            seq_printf(seq, "      rx_fn          = %s (%08x)\n", name, (u32)info->callback.rx_fn);
        else
            seq_printf(seq, "      rx_fn          = %08x\n", (u32)info->callback.rx_fn);
#else
        seq_printf(seq,     "      stop_tx_fn     = %08x\n", (u32)info->callback.stop_tx_fn);
        seq_printf(seq,     "      start_tx_fn    = %08x\n", (u32)info->callback.start_tx_fn);
        seq_printf(seq,     "      rx_fn          = %08x\n", (u32)info->callback.rx_fn);
#endif
        if ( info->netif )
            seq_printf(seq, "    netif            = %s (%08x)\n", info->netif->name, (u32)info->netif);
        else
            seq_printf(seq, "    netif            = N/A\n");
        seq_printf(seq, "    ifid             = %d\n", (u32)info->ifid);
        seq_printf(seq, "    mac              = %02x:%02x:%02x:%02x:%02x:%02x\n", (u32)info->mac[0], (u32)info->mac[1], (u32)info->mac[2], (u32)info->mac[3], (u32)info->mac[4], (u32)info->mac[5]);
        seq_printf(seq, "    skb_list         = %08x\n", (u32)info->skb_list);
        seq_printf(seq, "    skb_list_len     = %u\n", info->skb_list_len);
        seq_printf(seq, "    skb_list_size    = %u\n", info->skb_list_size);
        seq_printf(seq, "    rx_fn_rxif_pkt   = %u\n", info->rx_fn_rxif_pkt);
        seq_printf(seq, "    rx_fn_txif_pkt   = %u\n", info->rx_fn_txif_pkt);
        seq_printf(seq, "    tx_pkt           = %u\n", info->tx_pkt);
        seq_printf(seq, "    tx_pkt_dropped   = %u\n", info->tx_pkt_dropped);
        seq_printf(seq, "    tx_pkt_queued    = %u\n", info->tx_pkt_queued);
        seq_printf(seq, "    flags            = VALID");
        if ( (info->flags & PPE_DIRECTPATH_DATA_RX_ENABLE) )
            seq_printf(seq, " | RX_EN");
        if ( (info->flags & PPE_DIRECTPATH_ETH) )
            seq_printf(seq, " | ETH");
        if ( (info->flags & PPE_DIRECTPATH_CORE0) )
            seq_printf(seq, " | CPU0");
        if ( (info->flags & PPE_DIRECTPATH_CORE1) )
            seq_printf(seq, " | CPU1");
        seq_printf(seq, "\n");
    }
    else
        seq_printf(seq, "    flags            = INVALID\n");

    return 0;
}

static int proc_read_directpath_seq_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &g_proc_read_directpath_seq_ops);
}

#if 0 //yixin remove test codes.
static int dp_dummy_eth_init(PPA_NETIF *dev)
{
    ether_setup(dev);   /*  assign some members */

    /*  hook network operations */

    SET_MODULE_OWNER(dev);

    dev->dev_addr[0] = 0x00;
    dev->dev_addr[1] = 0xe0;
    dev->dev_addr[2] = 0x92;
    dev->dev_addr[3] = 0x00;
    dev->dev_addr[4] = 0xf3;
    dev->dev_addr[5] = 0xf4;

    return 0;
}
static int32_t dp_dummy_fp_stop_tx(PPA_NETIF *dev)
{
    return 0;
}
static int32_t dp_dummy_fp_restart_tx(PPA_NETIF *dev)
{
    return 0;
}
static int32_t dp_dummy_fp_rx(PPA_NETIF *rxif, PPA_NETIF *txif, struct sk_buff *skb, int32_t len)
{
    return 0;
}
static uint32_t g_ifid[10] = {~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0};
static PPA_NETIF g_directpath_net_dev = {
    name:   "dp_dummy",
    init:   dp_dummy_eth_init
};
static PPA_DIRECTPATH_CB g_dp_dummy_cp = {
    stop_tx_fn:     dp_dummy_fp_stop_tx,
    start_tx_fn:    dp_dummy_fp_restart_tx,
    rx_fn:          dp_dummy_fp_rx
};

#endif

/*ctc*/
static void proc_file_write_directpath_help( void )
{
	printk( "clear [fid] - clear iteration\n"
			"add [cpu1]  - add direct path\n"
			"del <fid>   - delete direct path\n"
		  );
}

static ssize_t proc_file_write_directpath(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int len;
    char str[64];
    char *p;

//    int32_t ret;
    uint32_t start_ifid, end_ifid;
//    uint32_t ifid;
//    uint32_t flags;
//    int pos;
//    int f_register_netdev = 0;
//    int i;

    if ( !ppa_hook_directpath_register_dev_fn )
    {
        printk("PPA is not inited!\n");
        return count;
    }

    len = min(count, (size_t)sizeof(str) - 1);
    len -= ppa_copy_from_user(str, buf, len);
    while ( len && str[len - 1] <= ' ' )
        len--;
    str[len] = 0;
    for ( p = str; *p && *p <= ' '; p++, len-- );
    if ( !*p )
        return count;

    ppa_directpath_get_ifid_range(&start_ifid, &end_ifid);

    if ( stricmp(p, "clean") == 0 || stricmp(p, "clear") == 0 )
    {
        struct ppe_directpath_data *info;
        uint32_t pos;

        if ( ppa_directpath_data_start_iteration(&pos, &info) == IFX_SUCCESS )
        {
            do
            {
                if ( (info->flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) )
                {
                    info->rx_fn_rxif_pkt    = 0;
                    info->rx_fn_txif_pkt    = 0;
                    info->tx_pkt            = 0;
                    info->tx_pkt_dropped    = 0;
                    info->tx_pkt_queued     = 0;
                }
            } while ( ppa_directpath_data_iterate_next(&pos, &info) == IFX_SUCCESS );
        }
        ppa_directpath_data_stop_iteration();
    }
    else if ( strincmp(p, "clean ", 6) == 0 || strincmp(p, "clear ", 6) == 0 )
    {
        struct ppe_directpath_data *info;
        uint32_t pos = 0;

        p += 6;
        for ( ; *p && *p <= ' '; p++ );

        if ( *p && ppa_directpath_data_start_iteration(&pos, &info) == IFX_SUCCESS )
        {
            do
            {
                if ( (info->flags & PPE_DIRECTPATH_DATA_ENTRY_VALID) && info->netif && strcmp(info->netif->name, p) == 0 )
                {
                    info->rx_fn_rxif_pkt    = 0;
                    info->rx_fn_txif_pkt    = 0;
                    info->tx_pkt            = 0;
                    info->tx_pkt_dropped    = 0;
                    info->tx_pkt_queued     = 0;
                }
            } while ( ppa_directpath_data_iterate_next(&pos, &info) == IFX_SUCCESS );
        }
        ppa_directpath_data_stop_iteration();
    }
#if 0
    else if ( stricmp(p, "add") == 0 || strincmp(p, "add ", 4) == 0 )
    {
        flags = PPA_F_DIRECTPATH_REGISTER | PPA_F_DIRECTPATH_ETH_IF;
        if ( p[3] == ' ' )
        {
            p += 4;
            while ( *p && *p <= ' ' )
                p++;
            if ( *p && stricmp(p, "cpu1") == 0 )
                flags |= PPA_F_DIRECTPATH_CORE1;
        }

        for ( pos = 0; pos < NUM_ENTITY(g_ifid); pos++ )
            if ( g_ifid[pos] == ~0 )
                break;
        for ( i = 0; i < NUM_ENTITY(g_ifid); i++ )
            if ( g_ifid[i] != ~0 )
                break;
        if ( i == NUM_ENTITY(g_ifid) )
        {
            ppa_memset(&g_directpath_net_dev, 0, sizeof(g_directpath_net_dev));
            ppa_strcpy(g_directpath_net_dev.name, "dp_dummy");
            g_directpath_net_dev.init = dp_dummy_eth_init;
            if ( ppa_register_netdev(&g_directpath_net_dev) == 0 )
                f_register_netdev = 1;
        }

        ret = ppa_hook_directpath_register_dev_fn(&ifid, &g_directpath_net_dev, &g_dp_dummy_cp, flags);
        printk("directpath register - ret = %d, ifid = %u\n", ret, ifid);
        if ( ret == IFX_SUCCESS )
            g_ifid[pos] = ifid;
        else if ( f_register_netdev )
            ppa_unregister_netdev(&g_directpath_net_dev);
    }
    else if ( strincmp(p, "del ", 4) == 0 )
    {
        p += 4;

        while ( *p && *p <= ' ' )
            p++;

        ifid = 0;
        while ( *p >= '0' && *p <= '9' )
        {
            ifid = ifid * 10 + *p - '0';
            p++;
        }

        if ( ifid < start_ifid || ifid >= end_ifid )
            printk("ifid is out of range (%u, %u)\n", start_ifid, end_ifid - 1);
        else
        {
            for ( i = 0; i < NUM_ENTITY(g_ifid); i++ )
                if ( g_ifid[i] == ifid )
                    break;
            if ( i < NUM_ENTITY(g_ifid) )
            {
                ret = ppa_hook_directpath_register_dev_fn(&ifid, NULL, NULL, 0);
                printk("directpath unregister - ret = %d, ifid = %u\n", ret, ifid);
                if ( ret == IFX_SUCCESS )
                {
                    for ( i = 0; i < NUM_ENTITY(g_ifid); i++ )
                        if ( g_ifid[i] == ifid )
                            g_ifid[i] = ~0;
                    for ( i = 0; i < NUM_ENTITY(g_ifid); i++ )
                        if ( g_ifid[i] != ~0 )
                            break;
                    if ( i == NUM_ENTITY(g_ifid) )
                        ppa_unregister_netdev(&g_directpath_net_dev);
                }
            }
            else
                printk("ifid %u is not registered\n", ifid);
        }
    }
#else
	else
	{
	  #if 1 /* ctc */
		proc_file_write_directpath_help();
	  #endif
	}
#endif

    return count;
}

#endif

/*
 *  string process help function
 */

static int stricmp(const char *p1, const char *p2)
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

static int strincmp(const char *p1, const char *p2, int n)
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



/*
 * ####################################
 *           Global Function
 * ####################################
 */



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

void ppa_api_proc_file_create(void)
{
    struct proc_dir_entry *res;
    uint32_t f_incorrect_fw = 1;
    int i;
    PPA_VERSION ver={0};

    ver.index = 1;
    if( ifx_ppa_drv_get_firmware_id(&ver, 0) != IFX_SUCCESS)
    {
        ver.index = 0;
        ifx_ppa_drv_get_firmware_id(&ver, 0);
    }
    switch ( ver.family )
    {
    case 1: //  Danube
    case 2: //  Twinpass
    case 3: //  Amazon-SE
        g_str_dest[0] = "ETH0";
        g_str_dest[2] = "CPU0";
        g_str_dest[3] = "EXT_INT1";
        g_str_dest[4] = "EXT_INT2";
        g_str_dest[5] = "EXT_INT3";
        g_str_dest[6] = "EXT_INT4";
        g_str_dest[7] = "EXT_INT5";
        if ( ver.itf == 1        //  D4
            && ver.family != 3)
        {
            g_str_dest[1] = "ETH1";
            f_incorrect_fw = 0;
        }
        else if ( ver.itf == 2 ) //  A4
        {
            g_str_dest[1] = "ATM";
            f_incorrect_fw = 0;
        }
        break;
    case 5: //  AR9
    case 6: //  GR9
        g_str_dest[0] = "ETH0";
        g_str_dest[1] = "ETH1";
        g_str_dest[2] = "CPU0";
        g_str_dest[3] = "EXT_INT1";
        g_str_dest[4] = "EXT_INT2";
        g_str_dest[5] = "EXT_INT3";
        if ( ver.itf == 1 )      //  D5
        {
            g_str_dest[6] = "EXT_INT4";
            g_str_dest[7] = "EXT_INT5";
            f_incorrect_fw = 0;
        }
        else if ( ver.itf == 4 ) //  A5
        {
            g_str_dest[6] = "ATM_res";
            g_str_dest[7] = "ATM";
            f_incorrect_fw = 0;
        }
    case 7: //  VR9
        g_str_dest[0] = "ETH0";
        g_str_dest[1] = "ETH1";
        g_str_dest[2] = "CPU0";
        g_str_dest[3] = "EXT_INT1";
        g_str_dest[4] = "EXT_INT2";
        g_str_dest[5] = "EXT_INT3";
        if ( ver.itf == 1 )      //  D5
        {
            g_str_dest[6] = "EXT_INT4";
            g_str_dest[7] = "EXT_INT5";
            f_incorrect_fw = 0;
        }
        else if ( ver.itf == 4 ) //  A5
        {
            g_str_dest[6] = "ATM_res";
            g_str_dest[7] = "ATM";
            f_incorrect_fw = 0;
        }
        else if ( ver.itf == 5 ) //  E5
        {
            g_str_dest[6] = "PTM_res";
            g_str_dest[7] = "PTM";
            f_incorrect_fw = 0;
        }
    }
    if ( f_incorrect_fw )
    {
        for ( i = 0; i < NUM_ENTITY(g_str_dest); i++ )
            g_str_dest[i] = "INVALID";
    }

    for ( res = proc_root.subdir; res; res = res->next )
        if ( res->namelen == 3
            && res->name[0] == 'p'
            && res->name[1] == 'p'
            && res->name[2] == 'a' ) //  "ppa"
        {
            g_ppa_proc_dir = res;
            break;
        }
    if ( !res )
    {
        g_ppa_proc_dir = proc_mkdir("ppa", NULL);
        g_ppa_proc_dir_flag = 1;
    }

    for ( res = g_ppa_proc_dir->subdir; res; res = res->next )
        if ( res->namelen == 3
            && res->name[0] == 'a'
            && res->name[1] == 'p'
            && res->name[2] == 'i' )    //  "api"
        {
            g_ppa_api_proc_dir = res;
            break;
        }
    if ( !res )
    {
        g_ppa_api_proc_dir = proc_mkdir("api", g_ppa_proc_dir);
        g_ppa_api_proc_dir_flag = 1;
    }

    res = create_proc_entry("dbg",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_dbg;
        res->write_proc = proc_write_dbg;
    }

    res = create_proc_entry("hook",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
    {
        res->read_proc  = proc_read_hook;
        res->write_proc = proc_write_hook;
    }

    res = create_proc_entry("phys_port",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
        res->proc_fops = &g_proc_file_phys_port_seq_fops;

    res = create_proc_entry("netif",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
        res->proc_fops = &g_proc_file_netif_seq_fops;

    res = create_proc_entry("session",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
        res->proc_fops = &g_proc_file_session_seq_fops;

#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
    res = create_proc_entry("directpath",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
        res->proc_fops = &g_proc_file_directpath_seq_fops;
#endif

	/* ctc */
  #ifdef CONFIG_ARC_PPA_FILTER
    res = create_proc_entry("filter",
                            0,
                            g_ppa_api_proc_dir);
    if ( res )
        res->proc_fops = &g_proc_file_filter_seq_fops;
  #endif /*CONFIG_ARC_PPA_FILTER*/

}

void ppa_api_proc_file_destroy(void)
{
#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
    remove_proc_entry("directpath",
                      g_ppa_api_proc_dir);
#endif

    remove_proc_entry("session",
                      g_ppa_api_proc_dir);

    remove_proc_entry("netif",
                      g_ppa_api_proc_dir);

    remove_proc_entry("phys_port",
                      g_ppa_api_proc_dir);

    remove_proc_entry("hook",
                      g_ppa_api_proc_dir);

    remove_proc_entry("dbg",
                      g_ppa_api_proc_dir);

	/* ctc */
  #ifdef CONFIG_ARC_PPA_FILTER
    remove_proc_entry("filter",
                      g_ppa_api_proc_dir);
  #endif /*CONFIG_ARC_PPA_FILTER*/

    if ( g_ppa_api_proc_dir_flag )
    {
        remove_proc_entry("api",
                          g_ppa_proc_dir);
        g_ppa_api_proc_dir = NULL;
        g_ppa_api_proc_dir_flag = 0;
    }

    if ( g_ppa_proc_dir_flag )
    {
        remove_proc_entry("ppa", NULL);
        g_ppa_proc_dir = NULL;
        g_ppa_proc_dir_flag = 0;
    }
}

#ifdef CONFIG_IFX_PPA_API_PROC_MODULE
int __init ifx_ppa_proc_driver_init(void)
{
    ppa_api_proc_file_create();

    return 0;
}

void __exit ifx_ppa_proc_driver_exit(void)
{
    ppa_api_proc_file_destroy();
}

module_init(ifx_ppa_proc_driver_init);
module_exit(ifx_ppa_proc_driver_exit);

#endif /* CONFIG_IFX_PPA_API_PROC_MODULE */



/* ctc */
#ifdef CONFIG_ARC_PPA_FILTER

extern int arc_ppa_filter_is_enable( void );
extern int arc_ppa_filter_enable( void );
extern int arc_ppa_filter_disable( void );
extern int arc_ppa_filter_get_debug_level( void );
extern int arc_ppa_filter_set_debug_level( int level );
extern int arc_ppa_filter_add( stArcPpaFilter* pNew );
extern int arc_ppa_filter_del( int iId );
extern int arc_ppa_filter_del_all( void );
extern int arc_ppa_filter_get( int iId, stArcPpaFilter* pBuf );
extern int arc_ppa_filter_search( stArcPpaFilter* pData );
extern int arc_ppa_filter_cnt( void );
extern int arc_ppa_filter_clear( int iId );
extern int arc_ppa_filter_clear_all( void );
extern int arc_ppa_filter_hit( void );
extern int arc_ppa_filter_miss( void );

static int proc_read_filter_type = 0; /* 0 - table list, 1 - detail */
static int proc_read_filter_id;
static stArcPpaFilter gstFilter;

static void *proc_read_filter_seq_start(struct seq_file *seq, loff_t *ppos)
{
    if ( *ppos != 0 )
        return NULL;

	proc_read_filter_id = (uint32_t)*ppos;

	if ( proc_read_filter_id == 0) {
		if (arc_ppa_filter_cnt() == 0) {
			printk( "PPA filter enable = %d\n", arc_ppa_filter_is_enable() );
			printk( "PPA filter debug = %d\n", arc_ppa_filter_get_debug_level() );
			printk( "PPA filter count = %d\n", arc_ppa_filter_cnt() );
			printk( "PPA filter hit count = %d\n", arc_ppa_filter_hit() );
			printk( "PPA filter miss count = %d\n", arc_ppa_filter_miss() );
			printk( "\n" );
			if (proc_read_filter_type == 0)
				printk( "id rxif mac prio tos l4proto srcip dstip srcport dstport pd_type pd_off pd_mask pd_val hit\n" );
		} else {
			seq_printf(seq, "PPA filter enable = %d\n", arc_ppa_filter_is_enable() );
			seq_printf(seq, "PPA filter debug = %d\n", arc_ppa_filter_get_debug_level() );
			seq_printf(seq, "PPA filter count = %d\n", arc_ppa_filter_cnt() );
			seq_printf(seq, "PPA filter hit count = %d\n", arc_ppa_filter_hit() );
			seq_printf(seq, "PPA filter miss count = %d\n", arc_ppa_filter_miss() );
			seq_printf(seq, "\n" );
			if (proc_read_filter_type == 0)
				seq_printf(seq, "id rxif mac prio tos l4proto sip dip sport dport pd_type pd_off pd_mask pd_val hit\n" );
		}
	}

    if ( arc_ppa_filter_get( proc_read_filter_id, &gstFilter ) == 0 )
    {
		*ppos = proc_read_filter_id + 1;
        return &gstFilter;
    }
    else
        return NULL;
}

static void *proc_read_filter_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
    proc_read_filter_id = (uint32_t)*ppos;

    if ( arc_ppa_filter_get( proc_read_filter_id, &gstFilter ) == 0 )
    {
		*ppos = proc_read_filter_id + 1;
        return &gstFilter;
    }
    else
        return NULL;
}

static void proc_read_filter_seq_stop(struct seq_file *seq, void *v)
{
    return;
}

static int proc_read_filter_seq_show(struct seq_file *seq, void *v)
{
	stArcPpaFilter*	pstFilter = (stArcPpaFilter*)v;

	if (proc_read_filter_type == 0)
	{
	    seq_printf(seq, "%d ", proc_read_filter_id );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_RXIF )
		        seq_printf(seq, "%s ", pstFilter->rxif );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_MAC )
	        	seq_printf(seq, "%02x:%02x:%02x:%02x:%02x:%02x "
							, pstFilter->srcmac[0], pstFilter->srcmac[1], pstFilter->srcmac[2]
							, pstFilter->srcmac[3], pstFilter->srcmac[4], pstFilter->srcmac[5] );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_PRIO )
		        seq_printf(seq, "%d ", pstFilter->prio );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_TOS )
		        seq_printf(seq, "%d ", pstFilter->tos );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_PROTO )
		        seq_printf(seq, "%d ", pstFilter->l4proto );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_SRCIP )
		        seq_printf(seq, "%d.%d.%d.%d "
							, (pstFilter->src_ip.ip>>24)
							, (pstFilter->src_ip.ip>>16) & 0xff
							, (pstFilter->src_ip.ip>> 8) & 0xff
							, (pstFilter->src_ip.ip    ) & 0xff );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_DSTIP )
		        seq_printf(seq, "%d.%d.%d.%d "
							, (pstFilter->dst_ip.ip>>24)
							, (pstFilter->dst_ip.ip>>16) & 0xff
							, (pstFilter->dst_ip.ip>> 8) & 0xff
							, (pstFilter->dst_ip.ip    ) & 0xff );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_SRCPORT )
		        seq_printf(seq, "%d ", pstFilter->src_port );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_DSTPORT )
		        seq_printf(seq, "%d ", pstFilter->dst_port );
		  else  seq_printf(seq, "- ");

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_PKTDATA )
		        seq_printf(seq, "%d%d%d%d 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x 0x%02x%02x%02x%02x "
							, pstFilter->pd_type[0], pstFilter->pd_type[1]
							, pstFilter->pd_type[2], pstFilter->pd_type[3]
							, pstFilter->pd_off[0], pstFilter->pd_off[1]
							, pstFilter->pd_off[2], pstFilter->pd_off[3]
							, pstFilter->pd_mask[0], pstFilter->pd_mask[1]
							, pstFilter->pd_mask[2], pstFilter->pd_mask[3]
							, pstFilter->pd_val[0], pstFilter->pd_val[1]
							, pstFilter->pd_val[2], pstFilter->pd_val[3] );
		  else  seq_printf(seq, "- - - - ");

        seq_printf(seq, "%u\n", pstFilter->hit );
	}
	else
	{
	    seq_printf(seq, "id = %d\n", proc_read_filter_id );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_RXIF )
	        seq_printf(seq, "rxif = %s\n", pstFilter->rxif );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_MAC )
	        seq_printf(seq, "srcmac = %02x:%02x:%02x:%02x:%02x:%02x\n"
						, pstFilter->srcmac[0], pstFilter->srcmac[1], pstFilter->srcmac[2]
						, pstFilter->srcmac[3], pstFilter->srcmac[4], pstFilter->srcmac[5] );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_PRIO )
	        seq_printf(seq, "prio = %d\n", pstFilter->prio );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_TOS )
	        seq_printf(seq, "tos = %d\n", pstFilter->tos );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_PROTO )
	        seq_printf(seq, "l4proto = %d\n", pstFilter->l4proto );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_SRCIP )
	        seq_printf(seq, "srcip = %d.%d.%d.%d\n"
							, (pstFilter->src_ip.ip>>24)
							, (pstFilter->src_ip.ip>>16) & 0xff
							, (pstFilter->src_ip.ip>> 8) & 0xff
							, (pstFilter->src_ip.ip    ) & 0xff );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_DSTIP )
	        seq_printf(seq, "dstip = %d.%d.%d.%d\n"
							, (pstFilter->dst_ip.ip>>24)
							, (pstFilter->dst_ip.ip>>16) & 0xff
							, (pstFilter->dst_ip.ip>> 8) & 0xff
							, (pstFilter->dst_ip.ip    ) & 0xff );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_SRCPORT )
	        seq_printf(seq, "srcport = %d\n", pstFilter->src_port );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_DSTPORT )
	        seq_printf(seq, "dstport = %d\n", pstFilter->dst_port );

	    if ( pstFilter->mask & ARC_PPA_FILTER_MASK_PKTDATA )
		        seq_printf(seq, "pd_type = %d%d%d%d\n"
							, pstFilter->pd_type[0], pstFilter->pd_type[1]
							, pstFilter->pd_type[2], pstFilter->pd_type[3] );
		        seq_printf(seq, "pd_off = 0x%02x%02x%02x%02x\n"
							, pstFilter->pd_off[0], pstFilter->pd_off[1]
							, pstFilter->pd_off[2], pstFilter->pd_off[3] );
		        seq_printf(seq, "pd_mask = 0x%02x%02x%02x%02x\n"
							, pstFilter->pd_mask[0], pstFilter->pd_mask[1]
							, pstFilter->pd_mask[2], pstFilter->pd_mask[3] );
		        seq_printf(seq, "pd_val = 0x%02x%02x%02x%02x\n"
							, pstFilter->pd_val[0], pstFilter->pd_val[1]
							, pstFilter->pd_val[2], pstFilter->pd_val[3] );

        seq_printf(seq, "hit = %u\n", pstFilter->hit );
	}

    return 0;
}

static int proc_read_filter_seq_open(struct inode *inode, struct file *file)
{
	proc_read_filter_id = 0;

    return seq_open(file, &g_proc_read_filter_seq_ops);
}

/*ctc*/
static void proc_write_filter_help( void )
{
	printk( "\n"
			"add <rxif> <srcmac> <prio> <tos> <l4proto> <sip> <dip> <sport> <dport> <pd_type> <pd_off> <pd_mask> <pd_val>\n"
			"del <rxif> <srcmac> <prio> <tos> <l4proto> <sip> <dip> <sport> <dport> <pd_type> <pd_off> <pd_mask> <pd_val>\n"
			"search <rxif> <srcmac> <prio> <tos> <l4proto> <sip> <dip> <sport> <dport> <pd_type> <pd_off> <pd_mask> <pd_val>\n"
			"del-id <id>    - delete a filter by its ID\n"
			"del-all        - delete all filters\n"
			"clear <id>     - clear a filter counter\n"
			"clear-all <id> - clear all counters\n"
			"enable         - enable PPA filter function\n"
			"disable        - disable PPA filter function\n"
			"debug-on       - enable debug mode\n"
			"debug-off      - disable debug mode\n"
			"show-list      - display in list format\n"
			"show-table     - display in table format\n"
			"help           - show this help message\n"
			"\n" );
	printk( "     <rxif>    - Rx interface name\n"
			"     <srcmac>  - source MAC, delimiter is ':' or '-'\n"
			"     <prio>    - L2 priority, 0~7\n"
			"     <tos>     - L3 ToS/DSCP value, 0~63\n"
			"     <l4proto> - L4 protocol, 0~255\n"
			"     <sip>     - source IPv4 address, delimiter is '.'\n"
			"     <dip>     - destination IPv4 address, delimiter is '.'\n"
			"     <sport>   - source L4 port, 0~65535\n"
			"     <dport>   - destination L4 port, 0~65535\n" );
	printk(	"     <pd_type> - four types of layer N packet data to be compared, decimal value.\n"
			"               -   It is a combination of four digits of 0/2/3/4. 0 is ignored\n"
			"               -   e.g. 3400 means one layer 3, one layer 4 and two ignored.\n"
			"     <pd_off>  - four offsets from packet header to compare, hexa-decimal value.\n"
			"               -   e.g. 0x010f1cff means offsets 0x01, 0x0f, 0x1c and 0xff\n"
			"     <pd_mask> - four bitwise-masks of packet data to compare to, hexa-decimal value.\n"
			"     <pd_val>  - four exptected values of packet data after masked, hexa-decimal value\n"
			"     Any arguments can be '-' to ignore those fields.\n"
			"     If any of <pd_type>, <pd_off>, <pd_mask> or <pd_val> are '-', all them are ignored.\n" );
}

static long arc_simple_strtol( const char *cp, char **endp, unsigned int base )
{
	while (*cp==' ' || *cp=='\t')
		cp++;

	if(*cp=='-')
		return -simple_strtoul(cp+1,endp,base);
	return simple_strtoul(cp,endp,base);
}

static ssize_t proc_write_filter(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char*	pPtr;
	char	sBuf[128];
	int		cnt;
	long	data;

	sBuf[0] = sBuf[sizeof(sBuf)-1] = '\0';

	/* trim the tailing space, tab and LF/CR*/
	if ( count > 0 )
	{
		if (count >= sizeof(sBuf))
			count = sizeof(sBuf) - 1;

		if (copy_from_user(sBuf, buf, count))
			return count;

		pPtr = (char*)sBuf + count - 1;

		for (; *pPtr==' ' || *pPtr=='\t' || *pPtr=='\n' || *pPtr=='\r'; pPtr++)
		{
			*pPtr = '\0';
		}
	}

	/* enable */
	if ( strnicmp( sBuf, "enable", sizeof("enable")-1 ) == 0 )
	{
		arc_ppa_filter_enable();
		/**/
		return count;
	}

	/* disable */
	if ( strnicmp( sBuf, "disable", sizeof("disable")-1 ) == 0 )
	{
		arc_ppa_filter_disable();
		/**/
		return count;
	}

	/* debug-on */
	if ( strnicmp( sBuf, "debug-on", sizeof("debug-on")-1 ) == 0 )
	{
		arc_ppa_filter_set_debug_level( 1 );
		/**/
		return count;
	}

	/* debug-off */
	if ( strnicmp( sBuf, "debug-off", sizeof("debug-off")-1 ) == 0 )
	{
		arc_ppa_filter_set_debug_level( 0 );
		/**/
		return count;
	}

	/* show-list */
	if ( strnicmp( sBuf, "show-list", sizeof("show-list")-1 ) == 0 )
	{
		proc_read_filter_type = 1;
		/**/
		return count;
	}

	/* show-table */
	if ( strnicmp( sBuf, "show-table", sizeof("show-table")-1 ) == 0 )
	{
		proc_read_filter_type = 0;
		/**/
		return count;
	}

	/* del-all */
	if ( strnicmp( sBuf, "del-all", sizeof("del-all")-1 ) == 0 )
	{
		arc_ppa_filter_del_all();
		/**/
		return count;
	}

	/* del-id */
	if ( strnicmp( sBuf, "del-id", sizeof("del-id")-1 ) == 0 )
	{
		for (pPtr=sBuf+sizeof("del-id"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		/* id */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		data = arc_simple_strtol(pPtr, &pPtr, 0);
		if (arc_ppa_filter_del( data ) != 0)
			printk( "%s: no such a filter\n", __FUNCTION__ );
		/**/
		return count;
	}

	/* clear-all */
	if ( strnicmp( sBuf, "clear-all", sizeof("clear-all")-1 ) == 0 )
	{
		arc_ppa_filter_clear_all();
		/**/
		return count;
	}

	/* clear */
	if ( strnicmp( sBuf, "clear", sizeof("clear")-1 ) == 0 )
	{
		for (pPtr=sBuf+sizeof("clear"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		/* id */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		data = arc_simple_strtol(pPtr, &pPtr, 0);
		if (arc_ppa_filter_clear( data ) != 0)
			printk( "%s: no such a filter\n", __FUNCTION__ );
		/**/
		return count;
	}

	/* add/del/search */
	if ( strnicmp( sBuf, "add", sizeof("add")-1 ) == 0 || strnicmp( sBuf, "del", sizeof("del")-1 ) == 0 || strnicmp( sBuf, "search", sizeof("search")-1 ) == 0 )
	{
		int				iOp;
		char*			pPtr2;
		stArcPpaFilter	stFilter;
		stFilter.mask = 0;
		if (strnicmp( sBuf, "add", sizeof("add")-1 ) == 0) {
			iOp = 1;
			for (pPtr=sBuf+sizeof("add"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		} else if (strnicmp( sBuf, "del", sizeof("del")-1 ) == 0) {
			iOp = 2;
			for (pPtr=sBuf+sizeof("del"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		} else { /* search */
			iOp = 3;
			for (pPtr=sBuf+sizeof("search"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		}
		/* rxif */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		for (pPtr2 = pPtr+1; *pPtr2>0x20 && *pPtr2<=0x7e; pPtr2++) { }
		*pPtr2 = '\0';
		if ( strlen(pPtr) >= sizeof(stFilter.rxif) )
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"-",1) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_RXIF;
			ppa_strcpy( stFilter.rxif, pPtr );
			pPtr = pPtr2 + 1;
		} else {
			pPtr += 2;
		}
		/* srcmac */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_MAC;
			for ( cnt = 0; cnt < 6; cnt++, pPtr++ ) {
				data = arc_simple_strtol(pPtr, &pPtr, 16);
				if (data < 0 || data > 0xff || (cnt != 5 && *pPtr != ':' && *pPtr != '-') || (cnt == 5 && *pPtr != ' ') )
					goto proc_write_filter_error;
				stFilter.srcmac[cnt] = (uint8_t)data;
			}
		} else {
			pPtr += 2;
		}
		/* prio */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_PRIO;
			data = arc_simple_strtol(pPtr, &pPtr, 0);
			if (data < 0 || data > 7)
				goto proc_write_filter_error;
			stFilter.prio = (uint8_t)data;
		} else {
			pPtr += 2;
		}
		/* tos */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_TOS;
			data = arc_simple_strtol(pPtr, &pPtr, 0);
			if (data < 0 || data > 63)
				goto proc_write_filter_error;
			stFilter.tos = (uint8_t)data;
		} else {
			pPtr += 2;
		}
		/* l4proto */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_PROTO;
			data = arc_simple_strtol(pPtr, &pPtr, 0);
			if (data < 0 || data > 255)
				goto proc_write_filter_error;
			stFilter.l4proto = (uint8_t)data;
		} else {
			pPtr += 2;
		}
		/* src_ip */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_SRCIP;
			stFilter.src_ip.ip = 0;
			for ( cnt = 0; cnt < 4; cnt++, pPtr++ ) {
				data = arc_simple_strtol(pPtr, &pPtr, 10);
				if (data < 0 || data > 0xff || (cnt != 3 && *pPtr != '.') || (cnt == 3 && *pPtr != ' ') )
					goto proc_write_filter_error;
				stFilter.src_ip.ip |= (((uint32_t)data) << (24-8*cnt));
			}
		} else {
			pPtr += 2;
		}
		/* dst_ip */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_DSTIP;
			stFilter.dst_ip.ip = 0;
			for ( cnt = 0; cnt < 4; cnt++, pPtr++ ) {
				data = arc_simple_strtol(pPtr, &pPtr, 10);
				if (data < 0 || data > 0xff || (cnt != 3 && *pPtr != '.') || (cnt == 3 && *pPtr != ' ') )
					goto proc_write_filter_error;
				stFilter.dst_ip.ip |= (((uint32_t)data) << (24-8*cnt));
			}
		} else {
			pPtr += 2;
		}
		/* src_port */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_SRCPORT;
			data = arc_simple_strtol(pPtr, &pPtr, 0);
			if (data < 0 || data > 65535)
				goto proc_write_filter_error;
			stFilter.src_port = (uint16_t)data;
		} else {
			pPtr += 2;
		}
		/* dst_port */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_DSTPORT;
			data = arc_simple_strtol(pPtr, &pPtr, 0);
			if (data < 0 || data > 65535)
				goto proc_write_filter_error;
			stFilter.dst_port = (uint16_t)data;
		} else {
			pPtr += 2;
		}
		/* pd_type */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",1) != 0 ) {
			stFilter.mask |= ARC_PPA_FILTER_MASK_PKTDATA;
			data = arc_simple_strtol(pPtr, &pPtr, 10);
			stFilter.pd_type[0] = (uint8_t)((data / 1000) % 10);
			if ( stFilter.pd_type[0] != 0 && stFilter.pd_type[0] != 2 && stFilter.pd_type[0] != 3 && stFilter.pd_type[0] != 4)
				goto proc_write_filter_error;
			stFilter.pd_type[1] = (uint8_t)((data / 100 ) % 10);
			if ( stFilter.pd_type[1] != 0 && stFilter.pd_type[1] != 2 && stFilter.pd_type[1] != 3 && stFilter.pd_type[1] != 4)
				goto proc_write_filter_error;
			stFilter.pd_type[2] = (uint8_t)((data / 10  ) % 10);
			if ( stFilter.pd_type[2] != 0 && stFilter.pd_type[2] != 2 && stFilter.pd_type[2] != 3 && stFilter.pd_type[2] != 4)
				goto proc_write_filter_error;
			stFilter.pd_type[3] = (uint8_t)((data       ) % 10);
			if ( stFilter.pd_type[3] != 0 && stFilter.pd_type[3] != 2 && stFilter.pd_type[3] != 3 && stFilter.pd_type[3] != 4)
				goto proc_write_filter_error;
			stFilter.pd_off[0] = stFilter.pd_off[1] = stFilter.pd_off[2] = stFilter.pd_off[3] = 0;
			stFilter.pd_mask[0] = stFilter.pd_mask[1] = stFilter.pd_mask[2] = stFilter.pd_mask[3] = 0;
			stFilter.pd_val[0] = stFilter.pd_val[1] = stFilter.pd_val[2] = stFilter.pd_val[3] = 0;
		} else {
			pPtr += 2;
		}
		/* pd_off */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			data = arc_simple_strtol(pPtr, &pPtr, 16);
			stFilter.pd_off[0] = (uint8_t)(data>>24);
			stFilter.pd_off[1] = (uint8_t)(data>>16) & 0xff;
			stFilter.pd_off[2] = (uint8_t)(data>> 8) & 0xff;
			stFilter.pd_off[3] = (uint8_t)(data    ) & 0xff;
		} else {
			stFilter.mask &= ~ARC_PPA_FILTER_MASK_PKTDATA;
			pPtr += 2;
		}
		/* pd_mask */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"- ",2) != 0 ) {
			data = arc_simple_strtol(pPtr, &pPtr, 16);
			stFilter.pd_mask[0] = (uint8_t)(data>>24);
			stFilter.pd_mask[1] = (uint8_t)(data>>16) & 0xff;
			stFilter.pd_mask[2] = (uint8_t)(data>> 8) & 0xff;
			stFilter.pd_mask[3] = (uint8_t)(data    ) & 0xff;
		} else {
			stFilter.mask &= ~ARC_PPA_FILTER_MASK_PKTDATA;
			pPtr += 2;
		}
		/* pd_val */
		for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
		if (*pPtr == '\0')
			goto proc_write_filter_error;
		if ( strnicmp(pPtr,"-",1) != 0 ) {
			data = arc_simple_strtol(pPtr, &pPtr, 16);
			stFilter.pd_val[0] = (uint8_t)(data>>24);
			stFilter.pd_val[1] = (uint8_t)(data>>16) & 0xff;
			stFilter.pd_val[2] = (uint8_t)(data>> 8) & 0xff;
			stFilter.pd_val[3] = (uint8_t)(data    ) & 0xff;
		} else {
			stFilter.mask &= ~ARC_PPA_FILTER_MASK_PKTDATA;
			pPtr++;
		}
		/**/
		if (stFilter.mask == 0)
			goto proc_write_filter_error;
		if (iOp == 1) {
			arc_ppa_filter_add( &stFilter );
		} else {
			cnt = arc_ppa_filter_search( &stFilter );
			if (cnt < 0) {
				printk( "%s: no such a filter\n", __FUNCTION__ );
			} else {
				if (iOp == 2)
					arc_ppa_filter_del( cnt );
				else
					printk( "filter found: %d\n", cnt );
			}
		}
		return count;
	}

proc_write_filter_error:

	proc_write_filter_help();

    return count;
}

#endif /*CONFIG_ARC_PPA_FILTER*/
