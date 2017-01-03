/*
 * Atmel AT45xxx DataFlash MTD driver for lightweight SPI framework
 *
 * Largely derived from at91_dataflash.c:
 *  Copyright (C) 2003-2005 SAN People (Pty) Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
#include <linux/math64.h>
#endif 
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/ifx_ssc.h>


/*
 * DataFlash is a kind of SPI flash.  Most AT45 chips have two buffers in
 * each chip, which may be used for double buffered I/O; but this driver
 * doesn't (yet) use these for any kind of i/o overlap or prefetching.
 *
 * Sometimes DataFlash is packaged in MMC-format cards, although the
 * MMC stack can't use SPI (yet), or distinguish between MMC and DataFlash
 * protocols during enumeration.
 */

#define CONFIG_DATAFLASH_WRITE_VERIFY

/* reads can bypass the buffers */
#define OP_READ_CONTINUOUS  0xE8
#define OP_READ_PAGE        0xD2

/* group B requests can run even while status reports "busy" */
#define OP_READ_STATUS      0xD7    /* group B */

/* move data between host and buffer */
#define OP_READ_BUFFER1     0xD4    /* group B */
#define OP_READ_BUFFER2     0xD6    /* group B */
#define OP_WRITE_BUFFER1    0x84    /* group B */
#define OP_WRITE_BUFFER2    0x87    /* group B */

/* erasing flash */
#define OP_ERASE_PAGE       0x81
#define OP_ERASE_BLOCK      0x50

/* move data between buffer and flash */
#define OP_TRANSFER_BUF1    0x53
#define OP_TRANSFER_BUF2    0x55
#define OP_MREAD_BUFFER1    0xD4
#define OP_MREAD_BUFFER2    0xD6
#define OP_MWERASE_BUFFER1  0x83
#define OP_MWERASE_BUFFER2  0x86
#define OP_MWRITE_BUFFER1   0x88    /* sector must be pre-erased */
#define OP_MWRITE_BUFFER2   0x89    /* sector must be pre-erased */

/* write to buffer, then write-erase to flash */
#define OP_PROGRAM_VIA_BUF1 0x82
#define OP_PROGRAM_VIA_BUF2 0x85

/* compare buffer to flash */
#define OP_COMPARE_BUF1     0x60
#define OP_COMPARE_BUF2     0x61

/* read flash to buffer, then write-erase to flash */
#define OP_REWRITE_VIA_BUF1 0x58
#define OP_REWRITE_VIA_BUF2 0x59

/* newer chips report JEDEC manufacturer and device IDs; chip
 * serial number and OTP bits; and per-sector writeprotect.
 */
#define OP_READ_ID          0x9F
#define OP_READ_SECURITY    0x77
#define OP_WRITE_SECURITY   0x9A    /* OTP bits */

#define IFX_OP_READY       0x80
#define IFX_OP_COMPARE     0x40

#define IFX_MAX_DUMMY_CYCLES   12

struct ifx_dataflash;
struct dataflash_t {
    u8                  command[4];
    char                name[24];

    unsigned short      page_offset;    /* offset in flash address */
    unsigned int        page_size;  /* of bytes per page */
    
    struct mtd_info     mtd;
    struct ifx_dataflash *spi;
    struct semaphore	lock;    
};

struct ifx_dataflash {
    struct dataflash_t    *flash;
    IFX_SSC_HANDLE      handler;
    int                 addr_cycles;
    int                 dummy_cycles;
};

//#define IFX_DATA_FLASH_DBG

#if defined(IFX_DATA_FLASH_DBG)
#define IFX_DATA_FLASH_PRINT(format, arg...)   \
    printk("%s: " format, __func__, ##arg)
#define INLINE 
#else
#define IFX_DATA_FLASH_PRINT(format, arg...)   \
    do {} while(0)
#define INLINE inline
#endif

static struct ifx_dataflash ifx_data_flash;

#ifdef IFX_DATA_FLASH_DBG
#include <linux/ctype.h>
static void 
at45_dump(const char *title, const u8 *buf, size_t len)
{
    int i, llen, lenlab = 0;
    const u8 *pos = buf;
    const int line_len = 16;

    printk("%s - hex_ascii(len=%lu):\n", title, (unsigned long) len);
    while (len) {
        llen = len > line_len ? line_len : len;
        printk("%08x: ", lenlab);
        for (i = 0; i < llen; i++)
            printk(" %02x", pos[i]);
        for (i = llen; i < line_len; i++)
            printk("   ");
        printk("   ");
        for (i = 0; i < llen; i++) {
            if (isprint(pos[i]))
                printk("%c", pos[i]);
            else
                printk(".");
        }
        for (i = llen; i < line_len; i++)
            printk(" ");
        printk("\n");
        pos += llen;
        len -= llen;
        lenlab += line_len;
    }
}
#endif /* IFX_SPI_FLASH_DBG */

/*
 * Return the status of the DataFlash device.
 */
static int 
ifx_dataflash_status(struct ifx_dataflash *spi)
{
    u8 status;
    u8 cmd = OP_READ_STATUS;

    ifx_sscLock(spi->handler);
    ifx_sscTx(spi->handler, &cmd, 1);
    ifx_sscRx(spi->handler, &status, 1);
    ifx_sscUnlock(spi->handler);

    return status;
}

/*
 * Poll the DataFlash device until it is READY.
 * This usually takes 5-20 msec or so; more for sector erase.
 */
static int 
ifx_dataflash_waitready(struct ifx_dataflash *spi)
{
    int status;

    for (;;) {
        status = ifx_dataflash_status(spi);
        if (status < 0) {
            IFX_DATA_FLASH_PRINT("status %d?\n", status);
            status = 0;
        }

        if ((status & IFX_OP_READY)) /* Ready */ 
            return status;

        msleep(3);
    }
}

static int 
ifx_dataflash_session(struct ifx_dataflash *spi, u8 *cmd, u32 cmd_size, u8 dummy_cycles, 
                 u8 * wbuf, u32 wcnt, u8 * rbuf, u32 rcnt)
{
    char buf[IFX_MAX_DUMMY_CYCLES] = {0};

    ifx_dataflash_waitready(spi);

    ifx_sscLock(spi->handler);
    
    /* Command and addr */
    ifx_sscTx(spi->handler, cmd, cmd_size);

    /* Dummy cycles */
    if (dummy_cycles > 0 && dummy_cycles < IFX_MAX_DUMMY_CYCLES) {
        ifx_sscTx(spi->handler, buf, dummy_cycles);
    }

    /* TX */
    if (wcnt != 0) {
        ifx_sscTx(spi->handler, wbuf, wcnt);
    }

    if (rcnt != 0) {
        ifx_sscRx(spi->handler, rbuf, rcnt);
    }
    
    ifx_sscUnlock(spi->handler);
    return 0;
}

/*
 * Erase pages of flash.
 */
static int 
ifx_dataflash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
    struct dataflash_t *priv = (struct dataflash_t *)mtd->priv;
    unsigned         blocksize = priv->page_size << 3;
    u8               *command;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
    uint32_t		rem;

    IFX_DATA_FLASH_PRINT("erase addr=0x%llx len %lld\n",
            (long long)instr->addr, (long long)instr->len);
#else
    IFX_DATA_FLASH_PRINT("erase addr=0x%x len 0x%x\n",
            instr->addr, instr->len);
#endif
	/* Sanity checks */
    if (instr->addr + instr->len > mtd->size)
        return -EINVAL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
    div_u64_rem(instr->len, priv->page_size, &rem);
    if (rem)
        return -EINVAL;
    div_u64_rem(instr->addr, priv->page_size, &rem);
    if (rem)
        return -EINVAL;
#else
    if (((instr->len % priv->page_size) != 0) 
        || ((instr->addr % priv->page_size) != 0)) {
        return -EINVAL;
    }
#endif

    command = priv->command;
    down(&priv->lock);
    while (instr->len > 0) {
        unsigned int    pageaddr;
        int     do_block;

        /* Calculate flash page address; use block erase (for speed) if
         * we're at a block boundary and need to erase the whole block.
         */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
        pageaddr = div_u64(instr->addr, priv->page_size);
#else
        pageaddr = instr->addr / priv->page_size;
#endif 
        do_block = (pageaddr & 0x7) == 0 && instr->len >= blocksize;
        pageaddr = pageaddr << priv->page_offset;

        command[0] = do_block ? OP_ERASE_BLOCK : OP_ERASE_PAGE;
        command[1] = (u8)(pageaddr >> 16);
        command[2] = (u8)(pageaddr >> 8);
        command[3] = 0;

        IFX_DATA_FLASH_PRINT("ERASE %s: (%x) %x %x %x [%i]\n",
            do_block ? "block" : "page",
            command[0], command[1], command[2], command[3],
            pageaddr);

        ifx_dataflash_session(priv->spi, command, 4, 0, NULL, 0, NULL, 0);
        if (do_block) {
            instr->addr += blocksize;
            instr->len -= blocksize;
        } else {
            instr->addr += priv->page_size;
            instr->len -= priv->page_size;
        }
    }
    up(&priv->lock);
    /* Inform MTD subsystem that erase is complete */
    instr->state = MTD_ERASE_DONE;
    mtd_erase_callback(instr);
    return 0;
}

/*
 * Read from the DataFlash device.
 *   from   : Start offset in flash device
 *   len    : Amount to read
 *   retlen : About of data actually read
 *   buf    : Buffer containing the data
 */
static int 
ifx_dataflash_read(struct mtd_info *mtd, loff_t from, size_t len,
                   size_t *retlen, u_char *buf)
{
    struct dataflash_t    *priv = (struct dataflash_t *)mtd->priv;
     unsigned int        addr;
    u8          *command;

    IFX_DATA_FLASH_PRINT("read 0x%x..0x%x\n", (unsigned)from, (unsigned)(from + len));

    *retlen = 0;

    /* Sanity checks */
    if (!len)
        return 0;
    if (from + len > mtd->size)
        return -EINVAL;

    /* Calculate flash page/byte address */
    addr = (((unsigned)from / priv->page_size) << priv->page_offset)
        + ((unsigned)from % priv->page_size);

    command = priv->command;
    down(&priv->lock);
    /* Continuous read, max clock = f(car) which may be less than
     * the peak rate available.  Some chips support commands with
     * fewer "don't care" bytes.  Both buffers stay unchanged.
     */
    command[0] = OP_READ_CONTINUOUS;
    command[1] = (u8)(addr >> 16);
    command[2] = (u8)(addr >> 8);
    command[3] = (u8)(addr >> 0);

    ifx_dataflash_session(priv->spi, command, 4, 4, NULL, 0, buf, len);
    *retlen = len;
    up(&priv->lock);
#ifdef IFX_DATA_FLASH_DBG
    at45_dump("RX", buf, len);
#endif
    return 0;
}

/*
 * Write to the DataFlash device.
 *   to     : Start offset in flash device
 *   len    : Amount to write
 *   retlen : Amount of data actually written
 *   buf    : Buffer containing the data
 */
static int 
ifx_dataflash_write(struct mtd_info *mtd, loff_t to, size_t len,
                size_t * retlen, const u_char * buf)
{
    struct dataflash_t    *priv = (struct dataflash_t *)mtd->priv;

    unsigned int        pageaddr, addr, offset, writelen;
    size_t          remaining = len;
    u_char          *writebuf = (u_char *) buf;
    int         status = -EINVAL;
    u8          *command;

    IFX_DATA_FLASH_PRINT("write 0x%x..0x%x\n", (unsigned)to, (unsigned)(to + len));

    *retlen = 0;

    /* Sanity checks */
    if (!len)
        return 0;
    if ((to + len) > mtd->size)
        return -EINVAL;

    command = priv->command;
    
    pageaddr = ((unsigned)to / priv->page_size);
    offset = ((unsigned)to % priv->page_size);
    if (offset + len > priv->page_size)
        writelen = priv->page_size - offset;
    else
        writelen = len;
    down(&priv->lock);
    while (remaining > 0) {
        IFX_DATA_FLASH_PRINT("write @ %i:%i len=%i\n",
            pageaddr, offset, writelen);

        /* REVISIT:
         * (a) each page in a sector must be rewritten at least
         *     once every 10K sibling erase/program operations.
         * (b) for pages that are already erased, we could
         *     use WRITE+MWRITE not PROGRAM for ~30% speedup.
         * (c) WRITE to buffer could be done while waiting for
         *     a previous MWRITE/MWERASE to complete ...
         * (d) error handling here seems to be mostly missing.
         *
         * Two persistent bits per page, plus a per-sector counter,
         * could support (a) and (b) ... we might consider using
         * the second half of sector zero, which is just one block,
         * to track that state.  (On AT91, that sector should also
         * support boot-from-DataFlash.)
         */

        addr = pageaddr << priv->page_offset;

        /* (1) Maybe transfer partial page to Buffer1 */
        if (writelen != priv->page_size) {
            command[0] = OP_TRANSFER_BUF1; /* From flash to buffer1 */
            command[1] = (addr & 0x00FF0000) >> 16;
            command[2] = (addr & 0x0000FF00) >> 8;
            command[3] = 0;

            IFX_DATA_FLASH_PRINT("TRANSFER: (%x) %x %x %x\n",
                command[0], command[1], command[2], command[3]);

            ifx_dataflash_session(priv->spi, command, 4, 0, NULL, 0, NULL, 0);
        }

        /* (2) Program full page via Buffer1 */
        addr += offset;
        command[0] = OP_PROGRAM_VIA_BUF1;
        command[1] = (addr & 0x00FF0000) >> 16;
        command[2] = (addr & 0x0000FF00) >> 8;
        command[3] = (addr & 0x000000FF);

        IFX_DATA_FLASH_PRINT("PROGRAM: (%x) %x %x %x\n",
            command[0], command[1], command[2], command[3]);
    #ifdef IFX_DATA_FLASH_DBG        
        at45_dump("WX", writebuf, writelen);
    #endif
        ifx_dataflash_session(priv->spi, command, 4, 0, writebuf, writelen, NULL, 0);

#ifdef  CONFIG_DATAFLASH_WRITE_VERIFY

        /* (3) Compare to Buffer1 */
        addr = pageaddr << priv->page_offset;
        command[0] = OP_COMPARE_BUF1;
        command[1] = (addr & 0x00FF0000) >> 16;
        command[2] = (addr & 0x0000FF00) >> 8;
        command[3] = 0;

        IFX_DATA_FLASH_PRINT("COMPARE: (%x) %x %x %x\n",
            command[0], command[1], command[2], command[3]);

        ifx_dataflash_session(priv->spi, command, 4, 0, NULL, 0, NULL, 0);
        status = ifx_dataflash_waitready(priv->spi);
        /* Check result of the compare operation */
        if (status & IFX_OP_COMPARE) {
            printk(KERN_ERR "%s: compare page %u, err %d\n",
                __func__, pageaddr, status);
            remaining = 0;
            status = -EIO;
            break;
        } else
            status = 0;

#endif  /* CONFIG_DATAFLASH_WRITE_VERIFY */
        remaining = remaining - writelen;
        pageaddr++;
        offset = 0;
        writebuf += writelen;
        *retlen += writelen;

        if (remaining > priv->page_size)
            writelen = priv->page_size;
        else
            writelen = remaining;
    }
    up(&priv->lock);
    return status;
}

static const struct mtd_partition ifx_spi_atmel_partitions[7][3] = {
    {{0, 0, 0}},

/* 256K Byte */
    {{
        .name   =      "spi-boot",      /* U-Boot firmware */
        .offset =      0x00000000,
        .size   =      0x00021000,         /* 256 */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }, {0}, {0},
    },

/* 512K Byte */ 
    {{0, 0, 0}},

/* 1M Byte */ 
    {{0, 0, 0}},

/* 2M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00021000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-firmware", /* firmware */
        .offset =       0x00021000,
        .size   =       0x00042000,        /* 192K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00063000,
        .size   =       0x001B0000,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},

/* 4M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00021000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-firmware", /* firmware */
        .offset =       0x00021000,
        .size   =       0x00042000,        /* 256K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00063000,
        .size   =       0x003bd000,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},

/* 8M Byte */
    {{
        .name   =       "spi-boot",     /* U-Boot firmware */
        .offset =       0x00000000,
        .size   =       0x00021000,        /* 128K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =      "spi-firmware",  /* firmware */
        .offset =      0x00021000,
        .size   =      0x00042000,         /* 192K */
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    },
    {
        .name   =       "spi-rootfs,kernel,Data,Environment",       /* default partition */
        .offset =       0x00063000,
        .size   =       0x003bd000,
    /*  mask_flags:   MTD_WRITEABLE,    force read-only */
    }},
};

static int
ifx_data_flash_size_to_index(u32 size)
{
    int i;
    int index = 0;
    
    i = (size >> 17); /* 128 KB minimum */
    if (i <= 1) {
        index = 0;
    }
    else if (i <= 2) {
        index = 1;
    }
    else if (i <= 4) {
        index = 2;
    }
    else if (i <= 8) {
        index = 3;
    }
    else if (i <= 16) {
        index = 4;
    }
    else if (i <= 32) {
        index = 5;
    }
    else if (i <= 64) {
        index = 6;
    }
    else {
        index = 7;
    }
    return index;
}

/*
 * Register DataFlash device with MTD subsystem.
 */
static int 
ifx_add_dataflash(char *name,
        int nr_pages, int pagesize, int pageoffset)
{
    int index;
    
    struct dataflash_t        *priv;
    struct mtd_info         *device;

    priv = kzalloc(sizeof *priv, GFP_KERNEL);
    if (!priv)
        return -ENOMEM;
	init_MUTEX(&priv->lock);
    priv->page_size = pagesize;
    priv->page_offset = pageoffset;
    priv->spi         = &ifx_data_flash;

    /* name must be usable with cmdlinepart */
    sprintf(priv->name, "%s",name);

    device = &priv->mtd;
    device->name =  priv->name;
    device->size = nr_pages * pagesize;
    device->erasesize = pagesize;
    device->writesize = pagesize;
    device->numeraseregions    =   0;
    device->eraseregions       =   NULL;    
    device->owner = THIS_MODULE;
    device->type = MTD_DATAFLASH;
    device->flags = MTD_WRITEABLE;
    device->erase = ifx_dataflash_erase;
    device->read = ifx_dataflash_read;
    device->write = ifx_dataflash_write;
    device->priv = priv;
    ifx_data_flash.flash = priv;

    index = ifx_data_flash_size_to_index(device->size);
    
    IFX_DATA_FLASH_PRINT("%s (%d KBytes)\n", name, device->size/1024);
    add_mtd_partitions(device, ifx_spi_atmel_partitions[index], ARRAY_SIZE(ifx_spi_atmel_partitions[index]));
    return 0;
}

#define IFX_DATA_FLASH_MODE                IFX_SSC_MODE_0
#define IFX_DATA_FLASH_PRIORITY            IFX_SSC_PRIO_LOW
#define IFX_DATA_FLASH_FRAGSIZE            560
#define IFX_DATA_FLASH_MAXFIFOSIZE         32
#define IFX_DATA_FLASH_DRV_VERSION         "0.0.2"
#define IFX_DATA_FLASH_CS                  IFX_SSC_WHBGPOSTAT_OUT0_POS
#define IFX_DATA_FLASH_NAME                "ifx_dataflash"
#ifdef CONFIG_USE_EMULATOR
#define IFX_DATA_FLASH_BAUDRATE            10000                        /*  10K Hz */
#else
#define IFX_DATA_FLASH_BAUDRATE            20000000                       /*  2 MHz */
#endif
#define IFX_DATA_FLASH_ADDR_CYCLES         3                             /* 24 bit address */

static INLINE int 
ifx_data_flash_cs_handler(u32 csq, IFX_CS_DATA cs_data)
{
    if (csq == IFX_SSC_CS_ON) { /* Low active */
        return ifx_ssc_cs_low(cs_data);
    }
    else {
        return ifx_ssc_cs_high(cs_data);
    }
}

static INLINE IFX_SSC_HANDLE
ifx_data_flash_register(char *dev_name) 
{
    IFX_SSC_CONFIGURE_t ssc_cfg = {0};
    
    ssc_cfg.baudrate     = IFX_DATA_FLASH_BAUDRATE;
    ssc_cfg.csset_cb     = ifx_data_flash_cs_handler;
    ssc_cfg.cs_data      = IFX_DATA_FLASH_CS;
    ssc_cfg.fragSize     = IFX_DATA_FLASH_FRAGSIZE;
    ssc_cfg.maxFIFOSize  = IFX_DATA_FLASH_MAXFIFOSIZE;
    ssc_cfg.ssc_mode     = IFX_DATA_FLASH_MODE;
    ssc_cfg.ssc_prio     = IFX_DATA_FLASH_PRIORITY;
    return ifx_sscAllocConnection(dev_name, &ssc_cfg);
}

static int
ifx_data_flash_probe(IFX_SSC_HANDLE handler)
{
    u8  cmd = OP_READ_ID;
    u8 manufacturer_id, device_id1, device_id2;
    
    ifx_sscLock(handler);
    /* Send the request for the part identification */
    ifx_sscTx(handler, &cmd, sizeof(cmd));
    /* Now read in the manufacturer id bytes */
    do {
        ifx_sscRx(handler, &manufacturer_id, 1);
        if (manufacturer_id == 0x7F) {
            printk("Warning: unhandled manufacturer continuation byte!\n");
        }
    } while (manufacturer_id == 0x7F); 

    /* Now read in the first device id byte */
    ifx_sscRx(handler, &device_id1, 1);
    /* Now read in the second device id byte */
    ifx_sscRx(handler, &device_id2, 1);
    ifx_sscUnlock(handler);
    
    IFX_DATA_FLASH_PRINT("Vendor %02x Type %02x sig %02x\n", manufacturer_id, 
        device_id1, device_id2);
    return 0;    
}

static INLINE void 
ifx_dataflash_gpio_init(void)
{
    ifx_gpio_register(IFX_GPIO_MODULE_SPI_FLASH);
}

static INLINE void 
ifx_dataflash_gpio_release(void)
{   
    ifx_gpio_deregister(IFX_GPIO_MODULE_SPI_FLASH);
}

/*
 * Detect and initialize DataFlash device:
 *
 *   Device      Density         ID code          #Pages PageSize  Offset
 *   AT45DB011B  1Mbit   (128K)  xx0011xx (0x0c)    512    264      9
 *   AT45DB021B  2Mbit   (256K)  xx0101xx (0x14)   1025    264      9
 *   AT45DB041B  4Mbit   (512K)  xx0111xx (0x1c)   2048    264      9
 *   AT45DB081B  8Mbit   (1M)    xx1001xx (0x24)   4096    264      9
 *   AT45DB0161B 16Mbit  (2M)    xx1011xx (0x2c)   4096    528     10
 *   AT45DB0321B 32Mbit  (4M)    xx1101xx (0x34)   8192    528     10
 *   AT45DB0642  64Mbit  (8M)    xx111xxx (0x3c)   8192   1056     11
 *   AT45DB1282  128Mbit (16M)   xx0100xx (0x10)  16384   1056     11
 */
static int __init 
ifx_dataflash_init(void)
{
    int status = 0;

    ifx_dataflash_gpio_init();
    memset(&ifx_data_flash, 0, sizeof(struct ifx_dataflash ));
    
    ifx_data_flash.handler= ifx_data_flash_register(IFX_DATA_FLASH_NAME);
    if (ifx_data_flash.handler == NULL) {
        ifx_dataflash_gpio_release();
        printk("%s: failed to register dataflash\n", __func__);
        return -ENOMEM;
    }

    ifx_data_flash_probe(ifx_data_flash.handler);
  
    status = ifx_dataflash_status(&ifx_data_flash);
    if (status <= 0 || status == 0xff) {
        ifx_dataflash_gpio_release();
        printk(KERN_ERR "%s status error %d\n", __func__, status);
        if (status == 0 || status == 0xff)
            status = -ENODEV;
        return status;
    }

    /* if there's a device there, assume it's dataflash.
     * board setup should have set spi->max_speed_max to
     * match f(car) for continuous reads, mode 0 or 3.
     */
    switch (status & 0x3c) {
    case 0x0c:  /* 0 0 1 1 x x */
        status = ifx_add_dataflash("AT45DB011B", 512, 264, 9);
        break;
    case 0x14:  /* 0 1 0 1 x x */
        status = ifx_add_dataflash("AT45DB021B", 1025, 264, 9);
        break;
    case 0x1c:  /* 0 1 1 1 x x */
        status = ifx_add_dataflash("AT45DB041x", 2048, 264, 9);
        break;
    case 0x24:  /* 1 0 0 1 x x */
        status = ifx_add_dataflash("AT45DB081B", 4096, 264, 9);
        break;
    case 0x2c:  /* 1 0 1 1 x x */
        status = ifx_add_dataflash("AT45DB161x", 4096, 528, 10);
        break;
    case 0x34:  /* 1 1 0 1 x x */
        status = ifx_add_dataflash("AT45DB321x", 8192, 528, 10);
        break;
    case 0x38:  /* 1 1 1 x x x */
    case 0x3c:
        status = ifx_add_dataflash("AT45DB642x", 8192, 1056, 11);
        break;
    /* obsolete AT45DB1282 not (yet?) supported */
    default:
        printk(KERN_ERR "%s: unsupported device (%x)\n", __func__, status & 0x3c);
        status = -ENODEV;
        return status;
    }
    
    printk(KERN_INFO "IFX SPI ATMEL45XX, version %s, (c)2009 Infineon Technologies AG\n", IFX_DATA_FLASH_DRV_VERSION);
    return status;
}

static void __exit 
ifx_dataflash_exit(void)
{
    IFX_DATA_FLASH_PRINT("exit\n");

    del_mtd_partitions(&ifx_data_flash.flash->mtd);
    ifx_sscFreeConnection(ifx_data_flash.handler);
    kfree(ifx_data_flash.flash);
    ifx_dataflash_gpio_release();
}

module_init(ifx_dataflash_init);

module_exit(ifx_dataflash_exit);

