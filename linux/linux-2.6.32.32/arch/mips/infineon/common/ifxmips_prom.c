/******************************************************************************
**
** FILE NAME    : ifxmips_prom.c
** PROJECT      : IFX UEIP
** MODULES      : BSP Basic
**
** DATE         : 27 May 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : common source file
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
** 27 May 2009   Xu Liang        The first UEIP release
*******************************************************************************/



/*
 * PROM interface routines.
 */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>

#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/model.h>

extern void prom_printf(const char * fmt, ...);
extern void ifx_chip_setup(void);

/* for Multithreading (APRP), vpe.c will use it */
unsigned long physical_memsize = 0;
unsigned long cp0_memsize = 0;

/* Glag to indicate whether the user put mem= in the command line */
#ifdef CONFIG_BLK_DEV_INITRD
extern unsigned long initrd_start, initrd_end;
#endif

unsigned long g_ifx_nor_flash_start = -1;
unsigned long g_ifx_nor_flash_size = 0;

static unsigned int *chip_cp1_base = NULL;
static unsigned int chip_cp1_size = 0;

#if defined(CONFIG_ATM_BONDING_BM) || defined(CONFIG_USE_EMULATOR)
#  define USE_BUILTIN_PARAMETER
#else
#  undef  USE_BUILTIN_PARAMETER
#endif

#ifdef USE_BUILTIN_PARAMETER
static char * chip_arg[3] =
{
    "<ignored>",
    //"root=/dev/ram rw initcall_debug 1 loglevel=10 ip=172.20.80.222:172.17.69.210::255.255.252.0::eth0:off ether=0,0,eth0 ethaddr=00:01:02:03:04:05 console=ttyS1,115200 panic=1 rd_start=0xa1000000 rd_size=532480",
    //"root=/dev/ram rw ip=172.20.80.222:172.17.69.210::255.255.252.0::eth0:off ether=0,0,eth0 ethaddr=00:01:02:03:04:05 console=ttyS1,115200 panic=1 rd_start=0x81000000 rd_size=1166337",
    //goodramdisk "root=/dev/ram rw ip=172.20.80.222:172.17.69.210::255.255.252.0::eth0:off ether=0,0,eth0 ethaddr=00:01:02:03:04:05 console=ttyS1,115200 panic=1",
    //"initrd=0xa1000000, 441156 root=/dev/ram rw ip=172.20.80.222:172.17.69.210::255.255.252.0::eth0:off ether=0,0,eth0 ethaddr=00:01:02:03:04:05 console=ttyS1,115200 panic=1",
   #if 1
    //"root=/dev/ram rw ip=172.20.80.222:172.17.69.210::255.255.252.0::eth0:off ether=0,0,eth0 ethaddr=00:01:02:03:04:05 console=ttyS1,4800 panic=1 mem=16M initcall_debug=1",
    //"root=/dev/ram rw ip=172.20.80.222:172.17.69.210::255.255.252.0::eth0:off ether=0,0,eth0 ethaddr=00:01:02:03:04:05 console=ttyS1,4800 panic=1 mem=16M nowait=1",
    //"root=/dev/nfs rw nfsroot=172.20.80.7:/mnt/root26_small ip=172.20.80.28:172.20.80.7::::eth0:on console=ttyS1,115200 ethaddr=00:E0:92:01:02:4F mem=31M panic=1",
    //"root=/dev/ram rw ip=172.20.80.222:172.20.80.1::::eth0:off console=ttyS0,115200 ethaddr=00:01:02:03:04:05 phym=16M mem=14M panic=1 nowait=1 migration_debug=1 max_cache_size=1048576",
    //"root=/dev/ram rw ip=172.20.80.222:172.20.80.1::::eth0:off console=ttyS0,38400 ethaddr=00:01:02:03:04:05 phym=16M mem=14M panic=1 nowait=1",
    "root=/dev/ram rw ip=172.20.80.222:172.20.80.1::::eth0:off console=ttyS0,38400 ethaddr=00:01:02:03:04:05 phym=16M mem=16M panic=1 nowait=1",
    //"root=/dev/ram rw ip=172.20.80.222:172.20.80.1::::eth0:off console=ttyUSIF0,38400 ethaddr=00:01:02:03:04:05 phym=16M mem=14M panic=1 nowait=1",
    //"root=/dev/nfs rw nfsroot=172.20.80.1:/mnt/root26_20 ip=172.20.80.222:172.20.80.1::::eth0:on console=ttyS1,9600 ethaddr=00:01:02:03:04:05 phym=16M mem=14M panic=1 nowait=1 migration_debug=1 migration_cost=5000000,6000000 max_cache_size=1048576",
   #else
    "root=/dev/mtdblock2 ip=172.20.80.28:172.20.80.7::::eth0:on console=ttyS1,115200 ethaddr=00:E0:92:01:02:4F mem=31M panic=1",
   #endif
    NULL
};

static char * chip_env[] =
{
    "flash_start=0x10000000",
    "flash_size=0x00400000",
    "memsize=16 *1024 *1024",
    NULL
};
#endif /* USE_BUILTIN_PARAMETER */

static struct callvectors *debug_vectors;

unsigned int* ifx_get_cp1_base(void)
{
    return chip_cp1_base;
}
EXPORT_SYMBOL(ifx_get_cp1_base);

unsigned int ifx_get_cp1_size(void)
{
    return chip_cp1_size;
}
EXPORT_SYMBOL(ifx_get_cp1_size);

//#define DEBUG_PROM

void __init prom_init(void)
{
    int argc = fw_arg0;
    char **argv = (char **) fw_arg1;
    char **envp = (char **) fw_arg2;
    struct callvectors *cv = (struct callvectors *) fw_arg3;

    int i;
    unsigned long memsz = 0;
#ifdef CONFIG_BLK_DEV_INITRD
    unsigned long rdstart = 0, rdsize = 0;
#endif
    char *scr;

    prom_printf("Lantiq xDSL CPE " BOARD_SYSTEM_TYPE "\n");

    ifx_chip_setup();

#ifdef CONFIG_USE_EMULATOR
    prom_printf("press any key to continue...\n");
    while (((*IFX_ASC1_FSTAT)& 0x003F /* ASCFSTAT_RXFFLMASK */) == 0x00) ;
#endif

    debug_vectors = cv;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
    mips_machgroup = MACH_GROUP_IFX;
#else
    mips_machtype  = MACH_TYPE_IFX;
#endif

#if defined(USE_BUILTIN_PARAMETER) && defined(CONFIG_IFX_ASC_DEFAULT_BAUDRATE) \
    && !defined(CONFIG_IFX_USIF_UART_CONSOLE)
    //  change some paramter to default settings for emulation purpose
    scr= (char *)KSEG1ADDR((unsigned long)chip_arg[1]);
    scr = strstr(scr, "console=");
    if ( scr != NULL )
    {
        scr += 14;  //  bypass "console=ttyS0,"
        i = sprintf(scr, "%d", CONFIG_IFX_ASC_DEFAULT_BAUDRATE);
        for ( ; i < 6; i++ )
            scr[i] = ' ';
    }

    argv = (char **)KSEG1ADDR((unsigned long)chip_arg);
    envp = (char **)KSEG1ADDR((unsigned long)chip_env);
    argc = 2;
    /* USIF has fixed clock input, no need to parse baudrate settting */
#elif defined(USE_BUILTIN_PARAMETER) && defined(CONFIG_SERIAL_IFX_USIF_UART)
    argv = (char **)KSEG1ADDR((unsigned long)chip_arg);
    envp = (char **)KSEG1ADDR((unsigned long)chip_env);
    argc = 2;
#else
    argv = (char **)KSEG1ADDR((unsigned long)argv);
    envp = (char **)KSEG1ADDR((unsigned long)envp);
#endif
#ifdef DEBUG_PROM
    prom_printf("[%s %s %d]: argc %d, fw_arg0 %p, fw_arg1 %p, fw_arg2 %p fw_arg3 %p\n", __FILE__, __func__, __LINE__,
                argc, fw_arg0, fw_arg1, fw_arg2, fw_arg3);
#endif
    /* arg[0] is "g", the rest is boot parameters */
    arcs_cmdline[0] = '\0';
    for (i = 1; i < argc; i++) {
        argv[i] = (char *)KSEG1ADDR(argv[i]);
        if (!argv[i])
            continue;
        if (strlen(arcs_cmdline) + strlen(argv[i] + 1) >= sizeof(arcs_cmdline))
            break;
        strcat(arcs_cmdline, argv[i]);
        strcat(arcs_cmdline, " ");
    }
#ifdef DEBUG_PROM
    prom_printf("[%s %d]: arcs_cmdline - %s\n", __func__, __LINE__, arcs_cmdline);
#endif
    if ( (scr = strstr(arcs_cmdline, "phym=")) )
    {
        scr += 5;
        memsz = 0;
        while ( *scr >= '0' && *scr <= '9' )
        {
            memsz = memsz * 10 + *scr - '0';
            scr++;
        }
        if ( *scr == 'm' || *scr == 'M' )
            memsz *= 1024 * 1024;
        else if ( *scr == 'k' || *scr == 'K' )
            memsz *= 1024;
        physical_memsize = memsz;
    }
    if ( (scr = strstr(arcs_cmdline, "mem=")) )
    {
        scr += 4;
        memsz = 0;
        while ( *scr >= '0' && *scr <= '9' )
        {
            memsz = memsz * 10 + *scr - '0';
            scr++;
        }
        if ( *scr == 'm' || *scr == 'M' )
            memsz *= 1024 * 1024;
        else if ( *scr == 'k' || *scr == 'K' )
            memsz *= 1024;
        cp0_memsize = memsz;
    }

    /* now handle envp */
    if (envp != (char **)KSEG1ADDR(0)) {
        /* assume for now that exactly 3 values get passed */
        while (*envp) {
            *envp = (char *)KSEG1ADDR(*envp);
            /* check for memsize */
            if (physical_memsize == 0 && strncmp(*envp, "memsize=", 8) == 0) {
                scr = *envp + 8;
                memsz = (int)simple_strtoul(scr, NULL, 0);
                physical_memsize = memsz * 1024 * 1024;
#ifdef DEBUG_PROM
                prom_printf("[%s %s %d]: memsize=%d\n", __FILE__, __func__, __LINE__, memsz);
#endif
            }
#ifdef CONFIG_BLK_DEV_INITRD
            /* check for initrd_start */
            if (strncmp(*envp, "initrd_start=", 13) == 0) {
                scr = *envp + 13;
                rdstart = (int)simple_strtoul(scr, NULL, 0);
                rdstart = KSEG1ADDR(rdstart);
  #ifdef DEBUG_PROM
                prom_printf("initrd_start=%#x\n", (unsigned int)rdstart);
  #endif
            }
            /* check for initrd_size */
            if (strncmp(*envp, "initrd_size=", 12) == 0)
            {
                scr = *envp + 12;
                rdsize = (int)simple_strtoul(scr, NULL, 0);
  #ifdef DEBUG_PROM
                prom_printf("initrd_size=%ul\n", (unsigned int)rdsize);
  #endif
            }
#endif /* CONFIG_BLK_DEV_INITRD */

#ifdef CONFIG_MTD_IFX_NOR
            /* check for flash address and size */
            if (strncmp(*envp, "flash_start=", 12) == 0) {
                scr = *envp + 12;
                g_ifx_nor_flash_start = simple_strtoul(scr, NULL, 0);
  #ifdef DEBUG_PROM
                prom_printf("flash_start=%#x\n", g_ifx_nor_flash_start);
  #endif
            }
            if (strncmp(*envp, "flash_size=", 11) == 0) {
                scr = *envp + 11;
                g_ifx_nor_flash_size = simple_strtoul(scr, NULL, 0);
  #ifdef DEBUG_PROM
                prom_printf("flash_size=%ul\n", g_ifx_nor_flash_size);
  #endif
            }
#endif  /* CONFIG_MTD_IFX_NOR */
            envp++;
        }
    }

    if ( physical_memsize ==  0 )
        physical_memsize = cp0_memsize;
    else if ( cp0_memsize ==  0 )
        cp0_memsize = physical_memsize;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
    max_pfn = PFN_DOWN(cp0_memsize);
#else
    max_pfn = PFN_DOWN(cp0_memsize - CPHYSADDR(PHYS_OFFSET));
#endif
    printk("phym = %08lx, mem = %08lx, max_pfn = %08lx\n", physical_memsize, cp0_memsize, max_pfn);

    chip_cp1_base = (unsigned int*)(KSEG1 | cp0_memsize);
    chip_cp1_size = physical_memsize - cp0_memsize;
    printk("Reserving memory for CP1 @0x%08x, size 0x%08x\n", (unsigned int)chip_cp1_base, chip_cp1_size);

#ifdef CONFIG_BLK_DEV_INITRD
    /* u-boot always passes a non-zero start, but a 0 size if there */
    /* is no ramdisk */
    if (rdstart != 0 && rdsize != 0)
    {
        initrd_start = rdstart;
        initrd_end = rdstart + rdsize;
    }
#endif
    /* Set the I/O base address */
    set_io_port_base(0);

    /* Set memory regions */
    ioport_resource.start = 0;      /* Should be KSEGx ???  */
    ioport_resource.end = 0xffffffff;   /* Should be ???    */

#ifdef DEBUG_PROM
    prom_printf("[%s %s %d]: finished\n", __FILE__, __func__, __LINE__);
#endif
}

void __init prom_free_prom_memory(void)
{
    return;
}

const char *get_system_type(void)
{
    return BOARD_SYSTEM_TYPE;
}

