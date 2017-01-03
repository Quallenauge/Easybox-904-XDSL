/******************************************************************************
**
** FILE NAME    : ifxmips_ptm_vdsl.c
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : PTM driver common source file (core functions for VR9)
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
** 07 JUL 2009  Xu Liang        Init Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/etherdevice.h>

/*
 *  Chip Specific Head File
 */
#include <asm/ifx/ifx_types.h>
//#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/ifx_clk.h>
#include "ifxmips_ptm_vdsl.h"



/*
 * ####################################
 *   Parameters to Configure Driver
 * ####################################
 */

static unsigned int lanport = 4;
static unsigned int devaddr = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
  #define MODULE_PARM_ARRAY(a, b)   module_param_array(a, int, NULL, 0)
  #define MODULE_PARM(a, b)         module_param(a, int, 0)
#else
  #define MODULE_PARM_ARRAY(a, b)   MODULE_PARM(a, b)
#endif

MODULE_PARM(lanport, "i");
MODULE_PARM_DESC(lanport, "Physical Port ID of LAN Port (0 - 5)");
MODULE_PARM(devaddr, "i");
MODULE_PARM_DESC(devaddr, "PPE Address of Device (0 stands for host only)");



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
 *  Proc File Functions
 */
static inline void proc_file_create(void);
static inline void proc_file_delete(void);
static int proc_read_dev(char *, char **, off_t, int, int *, void *);
static int proc_write_dev(struct file *, const char *, unsigned long, void *);
static int proc_read_version(char *, char **, off_t, int, int *, void *);
static int proc_read_wanmib(char *, char **, off_t, int, int *, void *);
static int proc_write_wanmib(struct file *, const char *, unsigned long, void *);
static int proc_read_qos(char *, char **, off_t, int, int *, void *);
static int proc_read_gamma(char *, char **, off_t, int, int *, void *);
static int port0_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port1_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port2_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port3_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port4_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port5_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port6_counters_proc_read(char *, char **, off_t, int, int *, void *);
static int port_counters_proc_write(struct file *, const char *, unsigned long, void *);
static int print_pce_entry(char *, int);
static int proc_read_pce(char *, char **, off_t, int, int *, void *);
static int proc_read_dbg(char *, char **, off_t, int, int *, void *);
static int proc_write_dbg(struct file *, const char *, unsigned long, void *);
static int proc_write_mem(struct file *, const char *, unsigned long, void *);
static int proc_read_pp32(char *, char **, off_t, int, int *, void *);
static int proc_write_pp32(struct file *, const char *, unsigned long, void *);

/*
 *  Proc Help Functions
 */
static int stricmp(const char *, const char *);
static int strincmp(const char *, const char *, int);
static int get_token(char **, char **, int *, int *);
static int get_number(char **, int *, int);
static inline void ignore_space(char **, int *);
static int ifx_ptm_version(char *);

/*
 *  External variable
 */
#if defined(CONFIG_IFXMIPS_DSL_CPE_MEI) || defined(CONFIG_IFXMIPS_DSL_CPE_MEI_MODULE)
  extern int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *);
  extern int (*ifx_mei_atm_showtime_exit)(void);
#else
  int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *) = NULL;
  EXPORT_SYMBOL(ifx_mei_atm_showtime_enter);
  int (*ifx_mei_atm_showtime_exit)(void) = NULL;
  EXPORT_SYMBOL(ifx_mei_atm_showtime_exit);
#endif



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

unsigned int ifx_ptm_dbg_enable = DBG_ENABLE_MASK_ERR;

static struct proc_dir_entry* g_ptm_dir = NULL;

static unsigned int g_wrx_total_pdu[2][4] = {{0}};
static unsigned int g_wrx_err_pdu[2][4] = {{0}};



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static inline void proc_file_create(void)
{
    struct proc_dir_entry *res;

    g_ptm_dir = proc_mkdir("driver/ifx_ptm", NULL);

    res = create_proc_entry("dev",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = proc_read_dev;
        res->write_proc = proc_write_dev;
    }

    create_proc_read_entry("version",
                            0,
                            g_ptm_dir,
                            proc_read_version,
                            NULL);

    res = create_proc_entry("wanmib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = proc_read_wanmib;
        res->write_proc = proc_write_wanmib;
    }

    res = create_proc_entry("qos",
                            0,
                            g_ptm_dir);
    if ( res )
        res->read_proc  = proc_read_qos;

    res = create_proc_entry("gamma",
                            0,
                            g_ptm_dir);
    if ( res )
        res->read_proc  = proc_read_gamma;

    res = create_proc_entry("port0mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port0_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port1mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port1_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port2mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port2_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port3mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port3_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port4mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port4_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port5mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port5_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    res = create_proc_entry("port6mib",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = port6_counters_proc_read;
        res->write_proc = port_counters_proc_write;
    }

    create_proc_read_entry("pce",
                            0,
                            g_ptm_dir,
                            proc_read_pce,
                            NULL);

    res = create_proc_entry("dbg",
                            0,
                            g_ptm_dir);
    if ( res ) {
        res->read_proc  = proc_read_dbg;
        res->write_proc = proc_write_dbg;
    }

    res = create_proc_entry("mem",
                            0,
                            g_ptm_dir);
    if ( res != NULL )
        res->write_proc = proc_write_mem;
    else
        printk("%s:%s:%d: failed to create proc mem!", __FILE__, __func__, __LINE__);

    res = create_proc_entry("pp32",
                            0,
                            g_ptm_dir);
    if ( res != NULL ) {
        res->read_proc = proc_read_pp32;
        res->write_proc = proc_write_pp32;
    }
    else
        printk("%s:%s:%d: failed to create proc pp32!", __FILE__, __func__, __LINE__);
}

static inline void proc_file_delete(void)
{
    remove_proc_entry("pp32", g_ptm_dir);

    remove_proc_entry("mem", g_ptm_dir);

    remove_proc_entry("dbg", g_ptm_dir);

    remove_proc_entry("pce", g_ptm_dir);

    remove_proc_entry("port6mib", g_ptm_dir);

    remove_proc_entry("port5mib", g_ptm_dir);

    remove_proc_entry("port4mib", g_ptm_dir);

    remove_proc_entry("port3mib", g_ptm_dir);

    remove_proc_entry("port2mib", g_ptm_dir);

    remove_proc_entry("port1mib", g_ptm_dir);

    remove_proc_entry("port0mib", g_ptm_dir);

    remove_proc_entry("gamma", g_ptm_dir);

    remove_proc_entry("qos", g_ptm_dir);

    remove_proc_entry("wanmib", g_ptm_dir);

    remove_proc_entry("version", g_ptm_dir);

    remove_proc_entry("dev", g_ptm_dir);

    remove_proc_entry("driver/ifx_ptm", NULL);
}

static int proc_read_dev(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;
    unsigned int ppe_addr = ifx_ptm_get_base_address();

    len += sprintf(buf + len, "%s mode - 0x%08x\n", ppe_addr == IFX_PPE_ORG ? "host" : "device", ppe_addr);

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

static int proc_write_dev(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return count;

    if ( stricmp(p, "host") == 0 ) {
        ifx_ptm_set_base_address(0);
        printk("host mode - PPE base address 0x%08x\n", ifx_ptm_get_base_address());
    }
    else if ( stricmp(p, "device") == 0 ) {
        if ( devaddr == 0 )
            printk("no device mode support, please assign \"devaddr\" during bootup\n");
        else {
            ifx_ptm_set_base_address(devaddr);
            printk("device mode - PPE base address 0x%08x\n", ifx_ptm_get_base_address());
        }
    }
    else
        printk("echo <host|device> > /proc/driver/ifx_ptm/dev");

    return count;
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_ptm_version(buf + len);

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

static int proc_read_wanmib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    volatile unsigned int *wrx_total_pdu[4] = {DREG_AR_AIIDLE_CNT0, DREG_AR_HEC_CNT0, DREG_AR_AIIDLE_CNT1, DREG_AR_HEC_CNT1};
    volatile unsigned int *wrx_err_pdu[4] = {DREG_AR_CERRN_CNT0, DREG_AR_CERRN_CNT1, DREG_AR_CERRNP_CNT0, DREG_AR_CERRNP_CNT1};
    int is_host = ifx_ptm_get_base_address() == IFX_PPE_ORG ? 0 : 1;
    int i;

    len += sprintf(page + off + len, "RX:\n");
    len += sprintf(page + off + len, "  wrx_total_pdu:   ");
    for ( i = 0; i < 4; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", IFX_REG_R32(wrx_total_pdu[i]) - g_wrx_total_pdu[is_host][i]);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wrx_dropdes_pdu: ");
    for ( i = 0; i < 4; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_RX_MIB_TABLE(i)->wrx_dropdes_pdu);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wrx_err_pdu:     ");
    for ( i = 0; i < 4; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", IFX_REG_R32(wrx_err_pdu[i]) - g_wrx_err_pdu[is_host][i]);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wrx_total_bytes: ");
    for ( i = 0; i < 4; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_RX_MIB_TABLE(i)->wrx_total_bytes);
    }
    len += sprintf(page + off + len, "\n");

    len += sprintf(page + off + len, "TX:\n");
    len += sprintf(page + off + len, "  wrx_total_pdu:           ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wrx_total_pdu);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wrx_total_bytes:         ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wrx_total_bytes);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wtx_total_pdu:           ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wtx_total_pdu);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wtx_total_bytes:         ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wtx_total_bytes);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wtx_cpu_dropdes_pdu:     ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wtx_cpu_dropdes_pdu);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wtx_cpu_dropsmall_pdu:   ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wtx_cpu_dropsmall_pdu);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wtx_fast_dropdes_pdu:    ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wtx_fast_dropdes_pdu);
    }
    len += sprintf(page + off + len, "\n");
    len += sprintf(page + off + len, "  wtx_fast_dropsmall_pdu:  ");
    for ( i = 0; i < 8; i++ ) {
        if ( i != 0 )
            len += sprintf(page + off + len, ", ");
        len += sprintf(page + off + len, "%10u", WAN_TX_MIB_TABLE(i)->wtx_fast_dropsmall_pdu);
    }
    len += sprintf(page + off + len, "\n");

    *eof = 1;

    return len;
}

static int proc_write_wanmib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    volatile unsigned int *wrx_total_pdu[4] = {DREG_AR_AIIDLE_CNT0, DREG_AR_HEC_CNT0, DREG_AR_AIIDLE_CNT1, DREG_AR_HEC_CNT1};
    volatile unsigned int *wrx_err_pdu[4] = {DREG_AR_CERRN_CNT0, DREG_AR_CERRN_CNT1, DREG_AR_CERRNP_CNT0, DREG_AR_CERRNP_CNT1};
    int is_host = ifx_ptm_get_base_address() == IFX_PPE_ORG ? 0 : 1;
    int i;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return count;

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clean") == 0 ) {
        void *tmp;
        for ( i = 0; i < 4; i++ ) {
            tmp = (void*)WAN_RX_MIB_TABLE(i);
            memset(tmp, 0, sizeof(*WAN_RX_MIB_TABLE(i)));
            g_wrx_total_pdu[is_host][i] = IFX_REG_R32(wrx_total_pdu[i]);
            g_wrx_err_pdu[is_host][i] = IFX_REG_R32(wrx_err_pdu[i]);
        }
        for ( i = 0; i < 8; i++ ) {
            tmp = (void*)WAN_TX_MIB_TABLE(i);
            memset(tmp, 0, sizeof(*WAN_TX_MIB_TABLE(i)));
        }
    }

    return count;
}

static int proc_read_qos(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[200];
    int llen;
    int i;
    struct wan_tx_mib_table qos_queue_mib;
    volatile struct tx_qos_cfg tx_qos_cfg = *TX_QOS_CFG;
    volatile struct wtx_qos_q_desc_cfg qos_q_desc_cfg;

    pstr = *start = page;

    __sync();

    llen = sprintf(pstr, "\n  qos         : %s\n  wfq         : %s\n  Rate shaping: %s\n\n",
                    tx_qos_cfg.eth1_qss ?"enabled":"disabled",
                    tx_qos_cfg.wfq_en?"enabled":"disabled",
                    tx_qos_cfg.shape_en ?"enabled":"disabled");
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  Ticks  =%u,    overhd    =%u,       qnum=%u  @%p\n", (u32)tx_qos_cfg.time_tick, (u32)tx_qos_cfg.overhd_bytes, (u32)tx_qos_cfg.eth1_eg_qnum, TX_QOS_CFG );
    pstr += llen;
    len += llen;

    llen = sprintf(pstr, "  PPE clk=%u MHz, basic tick=%u\n", (u32)cgu_get_pp32_clock() / 1000000, TX_QOS_CFG->time_tick / (cgu_get_pp32_clock() / 1000000));
    pstr += llen;
    len += llen;

    if ( tx_qos_cfg.eth1_eg_qnum )
    {
        llen = sprintf(pstr, "\n  MIB : rx_pkt/rx_bytes         tx_pkt/tx_bytes       cpu_small_drop/cpu_drop  fast_small_drop/fast_drop_cnt\n");
        pstr += llen;
        len += llen;
        for ( i = 0; i < 8; i++ )
        {
            qos_queue_mib = *WAN_TX_MIB_TABLE(i);

            llen = sprintf(str, "    %2u: %010u/%010u  %010u/%010u  %010u/%010u  %010u/%010u  @0x%p\n", i,
                qos_queue_mib.wrx_total_pdu, qos_queue_mib.wrx_total_bytes,
                qos_queue_mib.wtx_total_pdu, qos_queue_mib.wtx_total_bytes,
                qos_queue_mib.wtx_cpu_dropsmall_pdu, qos_queue_mib.wtx_cpu_dropdes_pdu,
                qos_queue_mib.wtx_fast_dropsmall_pdu, qos_queue_mib.wtx_fast_dropdes_pdu,
                WAN_TX_MIB_TABLE(i));

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
                goto PROC_READ_MAC_OVERRUN_END;
        }


        //QOS queue descriptor
        llen = sprintf(pstr, "\n  Desc: threshold  num    base_addr  rd_ptr   wr_ptr\n");
        pstr += llen;
        len += llen;
        for ( i = 0; i < 8; i++ )
        {
            qos_q_desc_cfg = *WTX_QOS_Q_DESC_CFG(i);

            llen = sprintf(pstr, "    %2u: 0x%02x       0x%02x   0x%04x     0x%04x   0x%04x  @0x%p\n", i,
                qos_q_desc_cfg.threshold,
                qos_q_desc_cfg.length,
                qos_q_desc_cfg.addr,
                qos_q_desc_cfg.rd_ptr,
                qos_q_desc_cfg.wr_ptr,
                WTX_QOS_Q_DESC_CFG(i) );

            pstr += llen;
            len += llen;
        }
    }

    *eof = 1;

    return len - off;

PROC_READ_MAC_OVERRUN_END:
    return len - llen - off;
}

static int proc_read_gamma(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "PSAVE_CFG     (0x%08X): start_state - %s, sleep_en - %s\n", (unsigned int)PSAVE_CFG, PSAVE_CFG->start_state ? "partial reset" : "full reset", PSAVE_CFG->sleep_en ? "on" : "off");
    len += sprintf(page + off + len, "EG_BWCTRL_CFG (0x%08X): fdesc_wm - %d, class_len - %d\n", (unsigned int)EG_BWCTRL_CFG, EG_BWCTRL_CFG->fdesc_wm, EG_BWCTRL_CFG->class_len);
    len += sprintf(page + off + len, "TEST_MODE     (0x%08X): mib_clear_mode - %s, test_mode - %s\n", (unsigned int)TEST_MODE, TEST_MODE->mib_clear_mode ? "on" : "off", TEST_MODE->test_mode ? "on" : "off");
    len += sprintf(page + off + len, "RX_BC_CFG:\t0x%08x\t0x%08x\n",   (unsigned int)RX_BC_CFG(0),                (unsigned int)RX_BC_CFG(1));
    len += sprintf(page + off + len, "  local_state:    %8d\t  %8d\n", (unsigned int)RX_BC_CFG(0)->local_state,   (unsigned int)RX_BC_CFG(1)->local_state);
    len += sprintf(page + off + len, "  remote_state:   %8d\t  %8d\n", (unsigned int)RX_BC_CFG(0)->remote_state,  (unsigned int)RX_BC_CFG(1)->remote_state);
    len += sprintf(page + off + len, "  to_false_th:    %8d\t  %8d\n", (unsigned int)RX_BC_CFG(0)->to_false_th,   (unsigned int)RX_BC_CFG(1)->to_false_th);
    len += sprintf(page + off + len, "  to_looking_th:  %8d\t  %8d\n", (unsigned int)RX_BC_CFG(0)->to_looking_th, (unsigned int)RX_BC_CFG(1)->to_looking_th);
    len += sprintf(page + off + len, "TX_BC_CFG:\t0x%08x\t0x%08x\n",   (unsigned int)TX_BC_CFG(0),                (unsigned int)TX_BC_CFG(1));
    len += sprintf(page + off + len, "  fill_wm:\t  %8d\t  %8d\n", (unsigned int)TX_BC_CFG(0)->fill_wm,   (unsigned int)TX_BC_CFG(1)->fill_wm);
    len += sprintf(page + off + len, "  uflw_wm:\t  %8d\t  %8d\n", (unsigned int)TX_BC_CFG(0)->uflw_wm,   (unsigned int)TX_BC_CFG(1)->uflw_wm);
    len += sprintf(page + off + len, "RX_GAMMA_ITF_CFG:\t0x%08x\t0x%08x\t0x%08x\t0x%08x\n",       (unsigned int)RX_GAMMA_ITF_CFG(0),                        (unsigned int)RX_GAMMA_ITF_CFG(1),                        (unsigned int)RX_GAMMA_ITF_CFG(2),                        (unsigned int)RX_GAMMA_ITF_CFG(3));
    len += sprintf(page + off + len, "  receive_state: \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->receive_state,         (unsigned int)RX_GAMMA_ITF_CFG(1)->receive_state,         (unsigned int)RX_GAMMA_ITF_CFG(2)->receive_state,         (unsigned int)RX_GAMMA_ITF_CFG(3)->receive_state);
    len += sprintf(page + off + len, "  rx_min_len:    \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_min_len,            (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_min_len,            (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_min_len,            (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_min_len);
    len += sprintf(page + off + len, "  rx_pad_en:     \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_pad_en,             (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_pad_en,             (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_pad_en,             (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_pad_en);
    len += sprintf(page + off + len, "  rx_eth_fcs_ver_dis:\t  %8d\t  %8d\t  %8d\t  %8d\n",       (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_eth_fcs_ver_dis,    (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_eth_fcs_ver_dis,    (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_eth_fcs_ver_dis,    (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_eth_fcs_ver_dis);
    len += sprintf(page + off + len, "  rx_rm_eth_fcs: \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_rm_eth_fcs,         (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_rm_eth_fcs,         (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_rm_eth_fcs,         (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_rm_eth_fcs);
    len += sprintf(page + off + len, "  rx_tc_crc_ver_dis:\t  %8d\t  %8d\t  %8d\t  %8d\n",        (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_tc_crc_ver_dis,     (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_tc_crc_ver_dis,     (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_tc_crc_ver_dis,     (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_tc_crc_ver_dis);
    len += sprintf(page + off + len, "  rx_tc_crc_size:\t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_tc_crc_size,        (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_tc_crc_size,        (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_tc_crc_size,        (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_tc_crc_size);
    len += sprintf(page + off + len, "  rx_eth_fcs_result:\t0x%8X\t0x%8X\t0x%8X\t0x%8X\n",        (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_eth_fcs_result,     (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_eth_fcs_result,     (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_eth_fcs_result,     (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_eth_fcs_result);
    len += sprintf(page + off + len, "  rx_tc_crc_result:\t0x%8X\t0x%8X\t0x%8X\t0x%8X\n",         (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_tc_crc_result,      (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_tc_crc_result,      (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_tc_crc_result,      (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_tc_crc_result);
    len += sprintf(page + off + len, "  rx_crc_cfg:    \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_crc_cfg,            (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_crc_cfg,            (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_crc_cfg,            (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_crc_cfg);
    len += sprintf(page + off + len, "  rx_eth_fcs_init_value:0x%08X\t0x%08X\t0x%08X\t0x%08X\n",  (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_eth_fcs_init_value, (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_eth_fcs_init_value, (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_eth_fcs_init_value, (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_eth_fcs_init_value);
    len += sprintf(page + off + len, "  rx_tc_crc_init_value:\t0x%08X\t0x%08X\t0x%08X\t0x%08X\n", (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_tc_crc_init_value,  (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_tc_crc_init_value,  (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_tc_crc_init_value,  (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_tc_crc_init_value);
    len += sprintf(page + off + len, "  rx_max_len_sel:\t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_max_len_sel,        (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_max_len_sel,        (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_max_len_sel,        (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_max_len_sel);
    len += sprintf(page + off + len, "  rx_edit_num2:  \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_num2,          (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_num2,          (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_num2,          (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_num2);
    len += sprintf(page + off + len, "  rx_edit_pos2:  \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_pos2,          (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_pos2,          (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_pos2,          (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_pos2);
    len += sprintf(page + off + len, "  rx_edit_type2: \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_type2,         (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_type2,         (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_type2,         (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_type2);
    len += sprintf(page + off + len, "  rx_edit_en2:   \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_en2,           (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_en2,           (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_en2,           (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_en2);
    len += sprintf(page + off + len, "  rx_edit_num1:  \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_num1,          (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_num1,          (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_num1,          (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_num1);
    len += sprintf(page + off + len, "  rx_edit_pos1:  \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_pos1,          (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_pos1,          (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_pos1,          (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_pos1);
    len += sprintf(page + off + len, "  rx_edit_type1: \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_type1,         (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_type1,         (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_type1,         (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_type1);
    len += sprintf(page + off + len, "  rx_edit_en1:   \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_edit_en1,           (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_edit_en1,           (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_edit_en1,           (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_edit_en1);
    len += sprintf(page + off + len, "  rx_inserted_bytes_1l:\t0x%08X\t0x%08X\t0x%08X\t0x%08X\n", (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_inserted_bytes_1l,  (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_inserted_bytes_1l,  (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_inserted_bytes_1l,  (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_inserted_bytes_1l);
    len += sprintf(page + off + len, "  rx_inserted_bytes_1h:\t0x%08X\t0x%08X\t0x%08X\t0x%08X\n", (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_inserted_bytes_1h,  (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_inserted_bytes_1h,  (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_inserted_bytes_1h,  (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_inserted_bytes_1h);
    len += sprintf(page + off + len, "  rx_inserted_bytes_2l:\t0x%08X\t0x%08X\t0x%08X\t0x%08X\n", (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_inserted_bytes_2l,  (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_inserted_bytes_2l,  (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_inserted_bytes_2l,  (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_inserted_bytes_2l);
    len += sprintf(page + off + len, "  rx_inserted_bytes_2h:\t0x%08X\t0x%08X\t0x%08X\t0x%08X\n", (unsigned int)RX_GAMMA_ITF_CFG(0)->rx_inserted_bytes_2h,  (unsigned int)RX_GAMMA_ITF_CFG(1)->rx_inserted_bytes_2h,  (unsigned int)RX_GAMMA_ITF_CFG(2)->rx_inserted_bytes_2h,  (unsigned int)RX_GAMMA_ITF_CFG(3)->rx_inserted_bytes_2h);
    len += sprintf(page + off + len, "RX_GAMMA_ITF_CFG:\t0x%08x\t0x%08x\t0x%08x\t0x%08x\n",       (unsigned int)TX_GAMMA_ITF_CFG(0),                        (unsigned int)TX_GAMMA_ITF_CFG(1),                        (unsigned int)TX_GAMMA_ITF_CFG(2),                        (unsigned int)TX_GAMMA_ITF_CFG(3));
    len += sprintf(page + off + len, "  tx_len_adj:    \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_len_adj,            (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_len_adj,            (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_len_adj,            (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_len_adj);
    len += sprintf(page + off + len, "  tx_crc_off_adj:\t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_crc_off_adj,        (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_crc_off_adj,        (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_crc_off_adj,        (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_crc_off_adj);
    len += sprintf(page + off + len, "  tx_min_len:    \t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_min_len,            (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_min_len,            (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_min_len,            (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_min_len);
    len += sprintf(page + off + len, "  tx_eth_fcs_gen_dis:\t  %8d\t  %8d\t  %8d\t  %8d\n",       (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_eth_fcs_gen_dis,    (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_eth_fcs_gen_dis,    (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_eth_fcs_gen_dis,    (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_eth_fcs_gen_dis);
    len += sprintf(page + off + len, "  tx_tc_crc_size:\t  %8d\t  %8d\t  %8d\t  %8d\n",           (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_tc_crc_size,        (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_tc_crc_size,        (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_tc_crc_size,        (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_tc_crc_size);
    len += sprintf(page + off + len, "  tx_crc_cfg:    \t0x%08X\t0x%08X\t0x%08X\t0x%08X\n",       (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_crc_cfg,            (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_crc_cfg,            (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_crc_cfg,            (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_crc_cfg);
    len += sprintf(page + off + len, "  tx_eth_fcs_init_value:0x%08X\t0x%08X\t0x%08X\t0x%08X\n",  (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_eth_fcs_init_value, (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_eth_fcs_init_value, (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_eth_fcs_init_value, (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_eth_fcs_init_value);
    len += sprintf(page + off + len, "  tx_tc_crc_init_value:\t0x%08X\t0x%08X\t0x%08X\t0x%08X\n", (unsigned int)TX_GAMMA_ITF_CFG(0)->tx_tc_crc_init_value,  (unsigned int)TX_GAMMA_ITF_CFG(1)->tx_tc_crc_init_value,  (unsigned int)TX_GAMMA_ITF_CFG(2)->tx_tc_crc_init_value,  (unsigned int)TX_GAMMA_ITF_CFG(3)->tx_tc_crc_init_value);
    len += sprintf(page + off + len, "  queue_mapping: \t0x%08X\t0x%08X\t0x%08X\t0x%08X\n",       (unsigned int)TX_GAMMA_ITF_CFG(0)->queue_mapping,         (unsigned int)TX_GAMMA_ITF_CFG(1)->queue_mapping,         (unsigned int)TX_GAMMA_ITF_CFG(2)->queue_mapping,         (unsigned int)TX_GAMMA_ITF_CFG(3)->queue_mapping);

    *eof = 1;

    return len;
}

static INLINE int read_port_counters(int port, char *buf)
{
    int len = 0;
    unsigned int counter;
    unsigned long long  byteCnt;

    len += sprintf(buf+len, "\n\tPort [%d] counters\n\n", port);

    len += sprintf(buf+len, "Rx Total PckCnt     :");
    counter = sw_get_rmon_counter(port, RX_TOTAL_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx Unicast PckCnt   :");
    counter = sw_get_rmon_counter(port, RX_UNICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx Multicast PckCnt :");
    counter = sw_get_rmon_counter(port, RX_MULTICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx Total PckCnt     :");
    counter = sw_get_rmon_counter(port, TX_TOTAL_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx Unicase PckCnt   :");
    counter = sw_get_rmon_counter(port, TX_UNICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx Multicase PckCnt :");
    counter = sw_get_rmon_counter(port, TX_MULTICAST_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Rx 64B PckCnt       :");
    counter = sw_get_rmon_counter(port, RX_SIZE_64B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [65-127B] PckCnt :");
    counter = sw_get_rmon_counter(port, RX_SIZE_65_127B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [128~255B] PckCnt:");
    counter = sw_get_rmon_counter(port, RX_SIZE_128_255B_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);
    len += sprintf(buf+len, "Rx [256~511B] PckCnt:");
    counter = sw_get_rmon_counter(port, RX_SIZE_256_511B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [512~1023B]PckCnt:");
    counter = sw_get_rmon_counter(port, RX_SIZE_512_1023B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Rx [ >1024B] PckCnt :");
    counter = sw_get_rmon_counter(port, RX_SIZE_1024B_MAX_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx [64B] PckCnt     :");
    counter = sw_get_rmon_counter(port, TX_SIZE_64B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [65~127B] PckCnt :");
    counter = sw_get_rmon_counter(port, TX_SIZE_65_127B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [128~255B] PckCnt:");
    counter = sw_get_rmon_counter(port, TX_SIZE_128_255B_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);
    len += sprintf(buf+len, "Tx [256~511B] PckCnt:");
    counter = sw_get_rmon_counter(port, TX_SIZE_256_511B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [512~1023B]PckCnt:");
    counter = sw_get_rmon_counter(port, TX_SIZE_512_1023B_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);
    len += sprintf(buf+len, "Tx [>1024B] PckCnt  :");
    counter = sw_get_rmon_counter(port, TX_SIZE_1024B_MAX_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Rx CRC err PckCnt   :");
    counter = sw_get_rmon_counter(port, RX_CRC_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Unsize err PCkCnt:");
    counter = sw_get_rmon_counter(port, RX_UNDERSIZE_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Ovsize err PckCnt:");
    counter = sw_get_rmon_counter(port, RX_OVER_SIZE_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\n",counter );

    len += sprintf(buf+len, "Rx UnsizeGood PckCnt:");
    counter = sw_get_rmon_counter(port, RX_UNDERSIZE_GOOD_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx OvsizeGood PckCnt:");
    counter = sw_get_rmon_counter(port, RX_OVER_SIZE_GOOD_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Good Pause PckCnt:");
    counter = sw_get_rmon_counter(port, RX_GOOD_PAUSE_PCKNT);
    len += sprintf(buf+len, "0x%08x\n",counter );

    len += sprintf(buf+len, "Rx Align err PckCnt :");
    counter = sw_get_rmon_counter(port, RX_ALLIGN_ERR_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx filterd errPckCnt:");
    counter = sw_get_rmon_counter(port, RX_FILTERED_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Tx Single col Cnt   :");
    counter = sw_get_rmon_counter(port, TX_SINGLE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx Multiple col Cnt :");
    counter = sw_get_rmon_counter(port, TX_MULTIPLE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Tx Late col  Cnt    :");
    counter = sw_get_rmon_counter(port,TX_LATE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Tx Excessive col Cnt:");
    counter = sw_get_rmon_counter(port, TX_EXCESSIVE_COLLISION_CNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Tx  Pause Cnt       :");
    counter = sw_get_rmon_counter(port, TX_PAUSE_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);


    len += sprintf(buf+len, "Tx Drop PckCnt      :");
    counter =sw_get_rmon_counter(port, TX_DROP_PCKNT);
    len += sprintf(buf+len, "0x%08x\t", counter);

    len += sprintf(buf+len, "Rx Drop PckCnt      :");
    counter = sw_get_rmon_counter(port, RX_DROP_PCKNT);
    len += sprintf(buf+len, "0x%08x\n", counter);

    len += sprintf(buf+len, "Rx Good Byte Cnt    :");
    byteCnt = sw_get_rmon_counter(port, RX_GOOD_BYTE_CNT_HIGH);
    byteCnt <<= 32;
    byteCnt += sw_get_rmon_counter(port, RX_GOOD_BYTE_CNT_LOW);
    len += sprintf(buf+len, "0x%llx \t", byteCnt);

    len += sprintf(buf+len, "Tx Good Byte Cnt    :");
    byteCnt = sw_get_rmon_counter(port, TX_GOOD_BYTE_CNT_HIGH);
    byteCnt <<= 32;
    byteCnt += sw_get_rmon_counter(port, TX_GOOD_BYTE_CNT_LOW);
    len += sprintf(buf+len, "0x%llx \n", byteCnt);

    len += sprintf(buf+len, "Rx Bad Byte Cnt     :");
    byteCnt = sw_get_rmon_counter(port, RX_BAD_BYTE_CNT_HIGH);
    byteCnt <<= 32;
    byteCnt += sw_get_rmon_counter(port, RX_BAD_BYTE_CNT_LOW);
    len += sprintf(buf+len, "0x%llx \t", byteCnt);

    len += sprintf(buf+len, "Rx Discard Cong Cnt :");
    byteCnt = sw_get_rmon_counter(port, RX_DISCARD_CONGESTION_PCKNT);
    len += sprintf(buf+len, "0x%llx \n", byteCnt);

    return len;
}

static int port0_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(0, buf + len);

    *eof = 1;

    return len;
}

static int port1_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(1, buf + len);

    *eof = 1;

    return len;
}

static int port2_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(2, buf + len);

    *eof = 1;

    return len;
}

static int port3_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(3, buf + len);

    *eof = 1;

    return len;
}

static int port4_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(4, buf + len);

    *eof = 1;

    return len;
}

static int port5_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(5, buf + len);

    *eof = 1;

    return len;
}

static int port6_counters_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += read_port_counters(6, buf + len);

    *eof = 1;

    return len;
}

static int port_counters_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;

    int port;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
    {
        return 0;
    }

    if ( stricmp(p, "clean") == 0 || stricmp(p, "clear") == 0 )
    {
        strcpy(str, file->f_dentry->d_iname);
        port = str[4] - '0';
        str[4] = ' ';
        if ( stricmp(str, "port mib") == 0 && port >= 0 && port < 7 )
            sw_clr_rmon_counter(port);
        else
            printk("unknown entry - %s\n", file->f_dentry->d_iname);
    }
    else if ( stricmp(p, "clean all") == 0 || stricmp(p, "clear all") == 0 )
    {
        for ( port = 0; port < 7; port++ )
            sw_clr_rmon_counter(port);
    }

    return count;
}

static int print_pce_entry(char *buf, int index)
{
    static char *out_fields[] = {
        "OUT_MAC0  ",   //  0
        "OUT_MAC1  ",   //  1
        "OUT_MAC2  ",   //  2
        "OUT_MAC3  ",   //  3
        "OUT_MAC4  ",   //  4
        "OUT_MAC5  ",   //  5
        "OUT_ETHTYP",   //  6
        "OUT_VTAG0 ",   //  7
        "OUT_VTAG1 ",   //  8
        "OUT_ITAG0 ",   //  9
        "OUT_ITAG1 ",   //  10
        "OUT_ITAG2 ",   //  11
        "OUT_ITAG3 ",   //  12
        "OUT_IP0   ",   //  13
        "OUT_IP1   ",   //  14
        "OUT_IP2   ",   //  15
        "OUT_IP3   ",   //  16
        "OUT_SIP0  ",   //  17
        "OUT_SIP1  ",   //  18
        "OUT_SIP2  ",   //  19
        "OUT_SIP3  ",   //  20
        "OUT_SIP4  ",   //  21
        "OUT_SIP5  ",   //  22
        "OUT_SIP6  ",   //  23
        "OUT_SIP7  ",   //  24
        "OUT_DIP0  ",   //  25
        "OUT_DIP1  ",   //  26
        "OUT_DIP2  ",   //  27
        "OUT_DIP3  ",   //  28
        "OUT_DIP4  ",   //  29
        "OUT_DIP5  ",   //  30
        "OUT_DIP6  ",   //  31
        "OUT_DIP7  ",   //  32
        "OUT_SESID ",   //  33
        "OUT_PROT  ",   //  34
        "OUT_APP0  ",   //  35
        "OUT_APP1  ",   //  36
        "OUT_IGMP0 ",   //  37
        "OUT_IGMP1 ",   //  38
        "OUT_IPOFF ",   //  39
        "OUT_NONE  ",   //  63
    };
    static char *types[] = {
        "INSTR  ",  //  0
        "IPV6   ",  //  1
        "LENACCU",  //  2
    };
    static char *flags[] = {
        "FLAG_ITAG ",  //  0
        "FLAG_VLAN ",  //  1
        "FLAG_SNAP ",  //  2
        "FLAG_PPPOE",  //  3
        "FLAG_IPV6 ",  //  4
        "FLAG_IPV6F",  //  5
        "FLAG_IPV4 ",  //  6
        "FLAG_IGMP ",  //  7
        "FLAG_TU   ",  //  8
        "FLAG_HOP  ",  //  9
        "FLAG_NN1  ",  //  10
        "FLAG_NN2  ",  //  11
        "FLAG_END  ",  //  12
        "FLAG_NO   ",  //  13
    };
    static char *titles[] = {
        "0 : IFXTAG    ",
        "1 : C_VTAG    ",
        "2 : ET_IPV4   ",
        "3 : ET_IPV6   ",
        "4 : ET_PPPOE_S",
        "5 : ET_PPPOE_D",
        "6 : S_VTAG    ",
        "7 : C_VTAG2   ",
        "8 : EL_LES_800",
        "9 : ET_OTHER  ",
        "10: EL_GRE_600",
        "11: EL_LES_600",
        "12: SNAP1     ",
        "13: NO_SNAP1  ",
        "14: SNAP2     ",
        "15: NO_SNAP2  ",
        "16: SESID_IGN ",
        "17: SESID     ",
        "18: PPPOE_IP  ",
        "19: PPPOE_NOIP",
        "20: IPV4_VER  ",
        "21: IPV6_VER  ",
        "22: NO_IP     ",
        "23: IPV4_UDP1 ",
        "24: IPV4_TCP  ",
        "25: IPV4_IGMP1",
        "26: IPV4_OTH1 ",
        "27: IPV4_UDP2 ",
        "28: IPV4_UDP3 ",
        "29: IPV4_OTH2 ",
        "30: IPV4_OTH3 ",
        "31: IPV4_IGMP2",
        "32: IPV4_IGMP3",
        "33: IPV4_IGMP4",
        "34: IPV6_UDP  ",
        "35: IPV6_TCP  ",
        "36: IPV6_HOP  ",
        "37: IPV6_ROU  ",
        "38: IPV6_DES  ",
        "39: IPV6_OTH  ",
        "40: NXT_HD_UDP",
        "41: NXT_HD_TCP",
        "42: NXT_HD_HOP",
        "43: NXT_HD_ROU",
        "44: NXT_HD_DES",
        "45: NXT_HD_OTH",
        "46: TU_IP     ",
        "47: TU_PORTS  ",
        "48: IPV6_IP   ",
        "49: END       ",
        "50: END       ",
        "51: END       ",
        "52: END       ",
        "53: END       ",
        "54: END       ",
        "55: END       ",
        "56: END       ",
        "57: END       ",
        "58: END       ",
        "59: END       ",
        "60: END       ",
        "61: END       ",
        "62: END       ",
        "63: END       ",
    };

    int len = 0;
    unsigned int val[4];
    unsigned int value;
    unsigned int mask;
    unsigned int ns;
    char *p_out_field;
    unsigned int L;
    char *p_type;
    char *p_flag;
    unsigned int ipv4_len;
    char *p_title;
    unsigned int tmp;

    if ( index < 0 )
    {
        len += sprintf(buf + len, "//------------------------------------------------------------------------------------------\n");
        len += sprintf(buf + len, "//                value    mask   ns  out_fields   L  type     flags       ipv4_len\n");
        len += sprintf(buf + len, "//------------------------------------------------------------------------------------------\n");
    }
    else
    {
        while ( (*PCE_TBL_CTRL & (1 << 15)) );
        *PCE_TBL_ADDR   = index;
        *PCE_TBL_CTRL   = 0x8000;   //  read micro code
        while ( (*PCE_TBL_CTRL & (1 << 15)) );
        val[3] = *PCE_TBL_VAL(3);
        val[2] = *PCE_TBL_VAL(2);
        val[1] = *PCE_TBL_VAL(1);
        val[0] = *PCE_TBL_VAL(0);

        value = val[3] & 0xFFFF;
        mask = val[2] & 0xFFFF;
        ns = (val[1] >> 10) & ((1 << 6) - 1);
        tmp = (val[1] >> 4) & ((1 << 6) - 1);
        if ( tmp <= 39 )
            p_out_field = out_fields[tmp];
        else if ( tmp == 63 )
            p_out_field = out_fields[40];
        else
            p_out_field = "reserved";
        L = ((val[1] & ((1 << 4) - 1)) << 1) | ((val[0] >> 15) & 0x01);
        tmp = (val[0] >> 13) & ((1 << 2) - 1);
        if ( tmp <= sizeof(types) / sizeof(*types) )
            p_type = types[tmp];
        else
            p_type = "reserved";
        tmp = (val[0] >> 9) & ((1 << 4) - 1);
        if ( tmp <= sizeof(flags) / sizeof(*flags) )
            p_flag = flags[tmp];
        else
            p_flag = "reserved";
        ipv4_len = (val[0] >> 8) & 0x01;
        tmp = index;
        if ( tmp <= sizeof(titles) / sizeof(*titles) )
            p_title = titles[tmp];
        else
            p_title = "reserved";
        len += sprintf(buf + len, "IFX_FLOW_PCE_MC_M(0x%04X, 0x%04X, %-2d, %s, %2d, %s, %s, %d),   // %s\n",
                       value, mask, ns, p_out_field, L, p_type, p_flag, ipv4_len, p_title);
    }

    return len;
}

static int proc_read_pce(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[1024];
    int llen;

    int i;

    pstr = *start = page;

    for ( i = -1; i < 64; i++ )
    {
        llen = print_pce_entry(str, i);
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
            goto PROC_READ_PCE_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_PCE_OVERRUN_END:
    return len - llen - off;
}

static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "error print      - %s\n", (ifx_ptm_dbg_enable & DBG_ENABLE_MASK_ERR)              ? "enabled" : "disabled");
    len += sprintf(page + off + len, "debug print      - %s\n", (ifx_ptm_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "assert           - %s\n", (ifx_ptm_dbg_enable & DBG_ENABLE_MASK_ASSERT)           ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump rx skb      - %s\n", (ifx_ptm_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_RX)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump tx skb      - %s\n", (ifx_ptm_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_TX)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "mac swap         - %s\n", (ifx_ptm_dbg_enable & DBG_ENABLE_MASK_MAC_SWAP)         ? "enabled" : "disabled");

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
        " dump rx skb",
        " rx",
        " dump tx skb",
        " tx",
        " dump init",
        " init",
        " dump qos",
        " qos",
        " mac swap",
        " swap",
        " all"
    };
    static const int dbg_enable_mask_str_len[] = {
        12, 4,
        12, 4,
        7,  7,
        12, 3,
        12, 3,
        10, 5,
        9,  4,
        9,  5,
        4
    };
    unsigned int dbg_enable_mask[] = {
        DBG_ENABLE_MASK_ERR,
        DBG_ENABLE_MASK_DEBUG_PRINT,
        DBG_ENABLE_MASK_ASSERT,
        DBG_ENABLE_MASK_DUMP_SKB_RX,
        DBG_ENABLE_MASK_DUMP_SKB_TX,
        DBG_ENABLE_MASK_DUMP_INIT,
        DBG_ENABLE_MASK_DUMP_QOS,
        DBG_ENABLE_MASK_MAC_SWAP,
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

    //  debugging feature for enter/leave showtime
    if ( strincmp(p, "enter", 5) == 0 && ifx_mei_atm_showtime_enter != NULL )
        ifx_mei_atm_showtime_enter(NULL, NULL);
    else if ( strincmp(p, "leave", 5) == 0 && ifx_mei_atm_showtime_exit != NULL )
        ifx_mei_atm_showtime_exit();

    if ( strincmp(p, "enable", 6) == 0 ) {
        p += 6;
        f_enable = 1;
    }
    else if ( strincmp(p, "disable", 7) == 0 ) {
        p += 7;
        f_enable = -1;
    }
    else if ( strincmp(p, "help", 4) == 0 || *p == '?' ) {
        printk("echo <enable/disable> [err/dbg/assert/rx/tx/init/qos/swap/all] > /proc/driver/ifx_ptm/dbg\n");
    }

    if ( f_enable ) {
        if ( *p == 0 ) {
            if ( f_enable > 0 )
                ifx_ptm_dbg_enable |= DBG_ENABLE_MASK_ALL & ~DBG_ENABLE_MASK_MAC_SWAP;
            else
                ifx_ptm_dbg_enable &= ~DBG_ENABLE_MASK_ALL | DBG_ENABLE_MASK_MAC_SWAP;
        }
        else {
            do {
                for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ )
                    if ( strincmp(p, dbg_enable_mask_str[i], dbg_enable_mask_str_len[i]) == 0 ) {
                        if ( f_enable > 0 )
                            ifx_ptm_dbg_enable |= dbg_enable_mask[i >> 1];
                        else
                            ifx_ptm_dbg_enable &= ~dbg_enable_mask[i >> 1];
                        p += dbg_enable_mask_str_len[i];
                        break;
                    }
            } while ( i < NUM_ENTITY(dbg_enable_mask_str) );
        }
    }

    return count;
}

static inline unsigned long sb_addr_to_fpi_addr_convert(unsigned long sb_addr)
{
#define PP32_SB_ADDR_END        0xFFFF

    if ( sb_addr < PP32_SB_ADDR_END) {
        return (unsigned long ) SB_BUFFER(sb_addr);
    }
    else {
        return sb_addr;
    }
}

static int proc_write_mem(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *p1, *p2;
    int len;
    int colon;
    unsigned long *p;
    char local_buf[1024];
    int i, n, l;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    colon = 1;
    while ( get_token(&p1, &p2, &len, &colon) )
    {
        if ( stricmp(p1, "w") == 0 || stricmp(p1, "write") == 0 || stricmp(p1, "r") == 0 || stricmp(p1, "read") == 0 )
            break;

        p1 = p2;
        colon = 1;
    }

    if ( *p1 == 'w' )
    {
        ignore_space(&p2, &len);
        p = (unsigned long *)get_number(&p2, &len, 1);
        p = (unsigned long *)sb_addr_to_fpi_addr_convert( (unsigned long) p);

        if ( (u32)p >= KSEG0 )
            while ( 1 )
            {
                ignore_space(&p2, &len);
                if ( !len || !((*p2 >= '0' && *p2 <= '9') || (*p2 >= 'a' && *p2 <= 'f') || (*p2 >= 'A' && *p2 <= 'F')) )
                    break;

                *p++ = (u32)get_number(&p2, &len, 1);
            }
    }
    else if ( *p1 == 'r' )
    {
        ignore_space(&p2, &len);
        p = (unsigned long *)get_number(&p2, &len, 1);
        p = (unsigned long *)sb_addr_to_fpi_addr_convert( (unsigned long) p);

        if ( (u32)p >= KSEG0 )
        {
            ignore_space(&p2, &len);
            n = (int)get_number(&p2, &len, 0);
            if ( n )
            {
                char str[32] = {0};
                char *pch = str;
                int k;
                u32 data;
                char c;

                n += (l = ((int)p >> 2) & 0x03);
                p = (unsigned long *)((u32)p & ~0x0F);
                for ( i = 0; i < n; i++ )
                {
                    if ( (i & 0x03) == 0 )
                    {
                        printk("%08X:", (u32)p);
                        pch = str;
                    }
                    if ( i < l )
                    {
                        printk("         ");
                        sprintf(pch, "    ");
                    }
                    else
                    {
                        data = (u32)*p;
                        printk(" %08X", data);
                        for ( k = 0; k < 4; k++ )
                        {
                            c = ((char*)&data)[k];
                            pch[k] = c < ' ' ? '.' : c;
                        }
                    }
                    p++;
                    pch += 4;
                    if ( (i & 0x03) == 0x03 )
                    {
                        pch[0] = 0;
                        printk(" ; %s\n", str);
                    }
                }
                if ( (n & 0x03) != 0x00 )
                {
                    for ( k = 4 - (n & 0x03); k > 0; k-- )
                        printk("         ");
                    pch[0] = 0;
                    printk(" ; %s\n", str);
                }
            }
        }
    }

    return count;
}

static int proc_read_pp32(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *stron = " on";
    static const char *stroff = "off";

    int len = 0;
    int cur_context;
    int f_stopped;
    char str[256];
    char strlength;
    int i, j;

    int pp32;

    for ( pp32 = 0; pp32 < NUM_OF_PP32; pp32++ )
    {
        f_stopped = 0;

        len += sprintf(page + off + len, "===== pp32 core %d =====\n", pp32);

#ifdef CONFIG_VR9
        if ( (*PP32_FREEZE & (1 << (pp32 << 4))) != 0 )
        {
            sprintf(str, "freezed");
            f_stopped = 1;
        }
#else
        if ( 0 )
        {
        }
#endif
        else if ( PP32_CPU_USER_STOPPED(pp32) || PP32_CPU_USER_BREAKIN_RCV(pp32) || PP32_CPU_USER_BREAKPOINT_MET(pp32) )
        {
            strlength = 0;
            if ( PP32_CPU_USER_STOPPED(pp32) )
                strlength += sprintf(str + strlength, "stopped");
            if ( PP32_CPU_USER_BREAKPOINT_MET(pp32) )
                strlength += sprintf(str + strlength, strlength ? " | breakpoint" : "breakpoint");
            if ( PP32_CPU_USER_BREAKIN_RCV(pp32) )
                strlength += sprintf(str + strlength, strlength ? " | breakin" : "breakin");
            f_stopped = 1;
        }
        else if ( PP32_CPU_CUR_PC(pp32) == PP32_CPU_CUR_PC(pp32) )
        {
            unsigned int pc_value[64] = {0};

            f_stopped = 1;
            for ( i = 0; f_stopped && i < NUM_ENTITY(pc_value); i++ )
            {
                pc_value[i] = PP32_CPU_CUR_PC(pp32);
                for ( j = 0; j < i; j++ )
                    if ( pc_value[j] != pc_value[i] )
                    {
                        f_stopped = 0;
                        break;
                    }
            }
            if ( f_stopped )
                sprintf(str, "hang");
        }
        if ( !f_stopped )
            sprintf(str, "running");
        cur_context = PP32_BRK_CUR_CONTEXT(pp32);
        len += sprintf(page + off + len, "Context: %d, PC: 0x%04x, %s\n", cur_context, PP32_CPU_CUR_PC(pp32), str);

        if ( PP32_CPU_USER_BREAKPOINT_MET(pp32) )
        {
            strlength = 0;
            if ( PP32_BRK_PC_MET(pp32, 0) )
                strlength += sprintf(str + strlength, "pc0");
            if ( PP32_BRK_PC_MET(pp32, 1) )
                strlength += sprintf(str + strlength, strlength ? " | pc1" : "pc1");
            if ( PP32_BRK_DATA_ADDR_MET(pp32, 0) )
                strlength += sprintf(str + strlength, strlength ? " | daddr0" : "daddr0");
            if ( PP32_BRK_DATA_ADDR_MET(pp32, 1) )
                strlength += sprintf(str + strlength, strlength ? " | daddr1" : "daddr1");
            if ( PP32_BRK_DATA_VALUE_RD_MET(pp32, 0) )
            {
                strlength += sprintf(str + strlength, strlength ? " | rdval0" : "rdval0");
                if ( PP32_BRK_DATA_VALUE_RD_LO_EQ(pp32, 0) )
                {
                    if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 0) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 0) )
                    strlength += sprintf(str + strlength, " >=");
            }
            if ( PP32_BRK_DATA_VALUE_RD_MET(pp32, 1) )
            {
                strlength += sprintf(str + strlength, strlength ? " | rdval1" : "rdval1");
                if ( PP32_BRK_DATA_VALUE_RD_LO_EQ(pp32, 1) )
                {
                    if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 1) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 1) )
                    strlength += sprintf(str + strlength, " >=");
            }
            if ( PP32_BRK_DATA_VALUE_WR_MET(pp32, 0) )
            {
                strlength += sprintf(str + strlength, strlength ? " | wtval0" : "wtval0");
                if ( PP32_BRK_DATA_VALUE_WR_LO_EQ(pp32, 0) )
                {
                    if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 0) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 0) )
                    strlength += sprintf(str + strlength, " >=");
            }
            if ( PP32_BRK_DATA_VALUE_WR_MET(pp32, 1) )
            {
                strlength += sprintf(str + strlength, strlength ? " | wtval1" : "wtval1");
                if ( PP32_BRK_DATA_VALUE_WR_LO_EQ(pp32, 1) )
                {
                    if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 1) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 1) )
                    strlength += sprintf(str + strlength, " >=");
            }
            len += sprintf(page + off + len, "break reason: %s\n", str);
        }

        if ( f_stopped )
        {
            len += sprintf(page + off + len, "General Purpose Register (Context %d):\n", cur_context);
            for ( i = 0; i < 4; i++ )
            {
                for ( j = 0; j < 4; j++ )
                    len += sprintf(page + off + len, "   %2d: %08x", i + j * 4, *PP32_GP_CONTEXTi_REGn(pp32, cur_context, i + j * 4));
                len += sprintf(page + off + len, "\n");
            }
        }

        len += sprintf(page + off + len, "break out on: break in - %s, stop - %s\n",
                                            PP32_CTRL_OPT_BREAKOUT_ON_BREAKIN(pp32) ? stron : stroff,
                                            PP32_CTRL_OPT_BREAKOUT_ON_STOP(pp32) ? stron : stroff);
        len += sprintf(page + off + len, "     stop on: break in - %s, break point - %s\n",
                                            PP32_CTRL_OPT_STOP_ON_BREAKIN(pp32) ? stron : stroff,
                                            PP32_CTRL_OPT_STOP_ON_BREAKPOINT(pp32) ? stron : stroff);
        len += sprintf(page + off + len, "breakpoint:\n");
        len += sprintf(page + off + len, "     pc0: 0x%08x, %s\n", *PP32_BRK_PC(pp32, 0), PP32_BRK_GRPi_PCn(pp32, 0, 0) ? "group 0" : "off");
        len += sprintf(page + off + len, "     pc1: 0x%08x, %s\n", *PP32_BRK_PC(pp32, 1), PP32_BRK_GRPi_PCn(pp32, 1, 1) ? "group 1" : "off");
        len += sprintf(page + off + len, "  daddr0: 0x%08x, %s\n", *PP32_BRK_DATA_ADDR(pp32, 0), PP32_BRK_GRPi_DATA_ADDRn(pp32, 0, 0) ? "group 0" : "off");
        len += sprintf(page + off + len, "  daddr1: 0x%08x, %s\n", *PP32_BRK_DATA_ADDR(pp32, 1), PP32_BRK_GRPi_DATA_ADDRn(pp32, 1, 1) ? "group 1" : "off");
        len += sprintf(page + off + len, "  rdval0: 0x%08x\n", *PP32_BRK_DATA_VALUE_RD(pp32, 0));
        len += sprintf(page + off + len, "  rdval1: 0x%08x\n", *PP32_BRK_DATA_VALUE_RD(pp32, 1));
        len += sprintf(page + off + len, "  wrval0: 0x%08x\n", *PP32_BRK_DATA_VALUE_WR(pp32, 0));
        len += sprintf(page + off + len, "  wrval1: 0x%08x\n", *PP32_BRK_DATA_VALUE_WR(pp32, 1));
    }

    *eof = 1;

    return len;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    u32 addr;

    int len, rlen;

    int pp32 = 0;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

#if defined(ENABLE_ADTRAN) && ENABLE_ADTRAN
    if ( strincmp(p, "test", 4) == 0 )
    {
        unsigned int pattern[] = {0x00000000, 0xFFFFFFFF, 0x50505050, 0x05050505, 0xA0A0A0A0, 0x0A0A0A0A, 0x55555555, 0xAAAAAAAA};
        int size = 0x2000;
        int i, j;

        for ( i = 0; i < NUM_ENTITY(pattern); i++ )
        {
            for ( j = 0; j < size; j++ )
                *CDM_DATA_MEMORY(1, j) = pattern[i];
            for ( j = 0; j < size; j++ )
                if ( *CDM_DATA_MEMORY(1, j) != pattern[i] )
                {
                    printk("dword %d (addr 0x%08x) - write 0x%08x, read 0x%08x\n", j, (unsigned int)CDM_DATA_MEMORY(1, j), pattern[i], *CDM_DATA_MEMORY(1, j));
                    return count;
                }
        }

        return count;
    }
#endif

    if ( strincmp(p, "pp32 ", 5) == 0 )
    {
        p += 5;
        rlen -= 5;

        while ( rlen > 0 && *p >= '0' && *p <= '9' )
        {
            pp32 += *p - '0';
            p++;
            rlen--;
        }
        while ( rlen > 0 && *p && *p <= ' ' )
        {
            p++;
            rlen--;
        }

        if ( pp32 >= NUM_OF_PP32 )
        {
            printk(KERN_ERR __FILE__ ":%d:%s: incorrect pp32 index - %d\n", __LINE__, __FUNCTION__, pp32);
            return count;
        }
    }

    if ( stricmp(p, "start") == 0 )
        *PP32_CTRL_CMD(pp32) = PP32_CTRL_CMD_RESTART;
    else if ( stricmp(p, "stop") == 0 )
        *PP32_CTRL_CMD(pp32) = PP32_CTRL_CMD_STOP;
    else if ( stricmp(p, "step") == 0 )
        *PP32_CTRL_CMD(pp32) = PP32_CTRL_CMD_STEP;
#ifdef CONFIG_VR9
    else if ( stricmp(p, "restart") == 0 )
        *PP32_FREEZE &= ~(1 << (pp32 << 4));
    else if ( stricmp(p, "freeze") == 0 )
        *PP32_FREEZE |= 1 << (pp32 << 4);
#endif
    else if ( strincmp(p, "pc0 ", 4) == 0 )
    {
        p += 4;
        rlen -= 4;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_OFF(0, 0);
            *PP32_BRK_PC_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_PC(pp32, 0) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_PC(pp32, 0) = addr;
            *PP32_BRK_PC_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_ON(0, 0);
        }
    }
    else if ( strincmp(p, "pc1 ", 4) == 0 )
    {
        p += 4;
        rlen -= 4;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_OFF(1, 1);
            *PP32_BRK_PC_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_PC(pp32, 1) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_PC(pp32, 1) = addr;
            *PP32_BRK_PC_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_ON(1, 1);
        }
    }
    else if ( strincmp(p, "daddr0 ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_OFF(0, 0);
            *PP32_BRK_DATA_ADDR_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_DATA_ADDR(pp32, 0) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_DATA_ADDR(pp32, 0) = addr;
            *PP32_BRK_DATA_ADDR_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_ON(0, 0);
        }
    }
    else if ( strincmp(p, "daddr1 ", 7) == 0 )
    {
        p += 7;
        rlen -= 7;
        if ( stricmp(p, "off") == 0 )
        {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_OFF(1, 1);
            *PP32_BRK_DATA_ADDR_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_DATA_ADDR(pp32, 1) = 0;
        }
        else
        {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_DATA_ADDR(pp32, 1) = addr;
            *PP32_BRK_DATA_ADDR_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_ON(1, 1);
        }
    }
    else
    {

        printk("echo \"<command>\" > /proc/driver/ifx_ptm/pp32\n");
        printk("  command:\n");
        printk("    start  - run pp32\n");
        printk("    stop   - stop pp32\n");
        printk("    step   - run pp32 with one step only\n");
        printk("    pc0    - pc0 <addr>/off, set break point PC0\n");
        printk("    pc1    - pc1 <addr>/off, set break point PC1\n");
        printk("    daddr0 - daddr0 <addr>/off, set break point data address 0\n");
        printk("    daddr0 - daddr1 <addr>/off, set break point data address 1\n");
        printk("    help   - print this screen\n");
    }

    if ( *PP32_BRK_TRIG(pp32) )
        *PP32_CTRL_OPT(pp32) = PP32_CTRL_OPT_STOP_ON_BREAKPOINT_ON;
    else
        *PP32_CTRL_OPT(pp32) = PP32_CTRL_OPT_STOP_ON_BREAKPOINT_OFF;

    return count;
}

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

static int get_token(char **p1, char **p2, int *len, int *colon)
{
    int tlen = 0;

    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z')) )
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

static int get_number(char **p, int *len, int is_hex)
{
    int ret = 0;
    int n = 0;

    if ( (*p)[0] == '0' && (*p)[1] == 'x' )
    {
        is_hex = 1;
        (*p) += 2;
        (*len) -= 2;
    }

    if ( is_hex )
    {
        while ( *len && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F')) )
        {
            if ( **p >= '0' && **p <= '9' )
                n = **p - '0';
            else if ( **p >= 'a' && **p <= 'f' )
               n = **p - 'a' + 10;
            else if ( **p >= 'A' && **p <= 'F' )
                n = **p - 'A' + 10;
            ret = (ret << 4) | n;
            (*p)++;
            (*len)--;
        }
    }
    else
    {
        while ( *len && **p >= '0' && **p <= '9' )
        {
            n = **p - '0';
            ret = ret * 10 + n;
            (*p)++;
            (*len)--;
        }
    }

    return ret;
}

static inline void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}

static int ifx_ptm_version(char *buf)
{
    int len = 0;
    unsigned int major, minor;

    ifx_ptm_get_fw_ver(&major, &minor);

    len += ifx_drv_ver(buf + len, "PTM", IFX_PTM_VER_MAJOR, IFX_PTM_VER_MID, IFX_PTM_VER_MINOR);
    len += sprintf(buf + len, "    PTM (E1) firmware version %d.%d\n", major, minor);

    return len;
}

static int ptm_showtime_enter(struct port_cell_info *port_cell, void *xdata_addr)
{
//    ASSERT(port_cell != NULL, "port_cell is NULL");
//    ASSERT(xdata_addr != NULL, "xdata_addr is NULL");

    //  TODO: ReTX set xdata_addr
//    g_xdata_addr = xdata_addr;

//    g_showtime = 1;

//    IFX_REG_W32(0x0F, UTP_CFG);

    printk("enter showtime\n");

    return IFX_SUCCESS;
}

static int ptm_showtime_exit(void)
{
//    if ( !g_showtime )
//        return IFX_ERROR;

//    IFX_REG_W32(0x00, UTP_CFG);

//    g_showtime = 0;

    //  TODO: ReTX clean state
//    g_xdata_addr = NULL;

    printk("leave showtime\n");

    return IFX_SUCCESS;
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

static int __init ifx_ptm_test_init(void)
{
    int ret;
    char ver_str[256];

    printk("%s: lanport = %u, devaddr = 0x%08x\n", __func__, lanport, devaddr);

    ifx_ptm_set_base_address(0);

    ret = ifx_ptm_init_chip(lanport);
    if ( ret != IFX_SUCCESS ) {
        err("INIT_CHIP_FAIL");
        goto INIT_CHIP_FAIL;
    }

    ret = ifx_pp32_start(0);
    if ( ret ) {
        err("ifx_pp32_start fail!");
        goto PP32_START_FAIL;
    }
    IFX_REG_W32(0x0F, UTP_CFG);

    if ( devaddr != 0 ) {
        ifx_ptm_set_base_address(devaddr);

        ret = ifx_ptm_init_chip(lanport);
        if ( ret != IFX_SUCCESS ) {
            err("device INIT_CHIP_FAIL");
            goto DEVICE_INIT_CHIP_FAIL;
        }

        ret = ifx_pp32_start(0);
        if ( ret ) {
            err("device ifx_pp32_start fail!");
            goto DEVICE_PP32_START_FAIL;
        }
        IFX_REG_W32(0x0F, UTP_CFG);
    }

    proc_file_create();

    ifx_ptm_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    ifx_mei_atm_showtime_enter = ptm_showtime_enter;
    ifx_mei_atm_showtime_exit  = ptm_showtime_exit;

    printk("ifxmips_ptm: PTM init succeed\n");

    return IFX_SUCCESS;

DEVICE_PP32_START_FAIL:
    ifx_ptm_uninit_chip();
DEVICE_INIT_CHIP_FAIL:
    ifx_ptm_set_base_address(0);
    ifx_pp32_stop(0);
PP32_START_FAIL:
    ifx_ptm_uninit_chip();
INIT_CHIP_FAIL:
    printk("ifxmips_ptm: PTM init failed\n");
    return ret;
}

static void __exit ifx_ptm_test_exit(void)
{
    ifx_mei_atm_showtime_enter = NULL;
    ifx_mei_atm_showtime_exit  = NULL;

    proc_file_delete();

    ifx_ptm_set_base_address(0);

    ifx_pp32_stop(0);

    ifx_ptm_uninit_chip();

    if ( devaddr != 0 ) {
        ifx_ptm_set_base_address(devaddr);

        ifx_pp32_stop(0);

        ifx_ptm_uninit_chip();
    }
}

static int __init lanport_setup(char *line)
{
    lanport = simple_strtoul(line, NULL, 0);

    if ( lanport < 0 || lanport > 5 )
        lanport = 4;

    return 0;
}

static int __init devaddr_setup(char *line)
{
    devaddr = CPHYSADDR(simple_strtoul(line, NULL, 0));

    return 0;
}

module_init(ifx_ptm_test_init);
module_exit(ifx_ptm_test_exit);
#ifndef MODULE
  __setup("lanport=", lanport_setup);
  __setup("devaddr=", devaddr_setup);
#endif
