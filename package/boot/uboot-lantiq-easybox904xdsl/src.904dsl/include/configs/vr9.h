/*
 * (C) Copyright 2003-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * This file contains the configuration parameters for the AR9 board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/autoconf.h>
#include <configs/lq_cfg.h>
#include <asm/vr9.h>


#ifdef CONFIG_VR9_CPU_111M_RAM_111M
#define CPU_CLOCK_RATE          111000000   /* 111 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          111000000   /* 111 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_125M_RAM_125M)
#define CPU_CLOCK_RATE          125000000   /* 125 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          125000000   /* 125 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_166M_RAM_166M)
#define CPU_CLOCK_RATE          166000000   /* 166 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          166000000   /* 166 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_333M_RAM_111M)
#define CPU_CLOCK_RATE          333000000   /* 333 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          111000000   /* 111 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_333M_RAM_166M)
#define CPU_CLOCK_RATE          333000000   /* 333 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          166000000   /* 166 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_393M_RAM_196M)
#define CPU_CLOCK_RATE          393000000   /* 393 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          196000000   /* 196 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_442M_RAM_221M)
#define CPU_CLOCK_RATE          442000000   /* 442 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          221000000   /* 221 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_442M_RAM_147M)
#define CPU_CLOCK_RATE          442000000   /* 442 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          147000000   /* 147 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_500M_RAM_166M)
#define CPU_CLOCK_RATE          500000000   /* 500 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          166000000   /* 166 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_500M_RAM_250M)
#define CPU_CLOCK_RATE          500000000   /* 500 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          250000000   /* 250 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_500M_RAM_200M)
#define CPU_CLOCK_RATE          500000000   /* 500 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          200000000   /* 250 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_600M_RAM_200M)
#define CPU_CLOCK_RATE          600000000   /* 600 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          200000000   /* 200 MHz clock for RAM           */
#elif defined(CONFIG_VR9_CPU_600M_RAM_300M)
#define CPU_CLOCK_RATE          600000000   /* 600 MHz clock for the MIPS core */
#define RAM_CLOCK_RATE          300000000   /* 300 MHz clock for RAM           */
#endif

#define F_SSC_CLK  get_fpi_clk()
#define CONFIG_SYS_HZ 1000
#define CONFIG_ENV_SPI_CS 3
#define CONFIG_ENV_SPI_MODE CONFIG_SF_DEFAULT_MODE



#ifndef	CPU_CLOCK_RATE
/* allowed values: 100000000, 133000000, and 150000000 */
#define CPU_CLOCK_RATE	150000000	/* default: 150 MHz clock for the MIPS core */
#endif

#if 1 /* ctc */
 #define CONFIG_BOOTDELAY	1	/* autoboot after 1 seconds	*/
#else
 #define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/
#endif
#define CONFIG_BAUDRATE		115200


/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_SYS_ASC_BASE 0xbe100c00
#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */

#define CONFIG_PREBOOT	"echo;"	\
	"echo Type \\\"run flash_nfs\\\" to mount root filesystem over NFS;" \
	"echo"

#undef	CONFIG_BOOTARGS

#define CONFIG_EXTRA_ENV_SETTINGS   <configs/lq_extra_env.h>


/*
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP				/* undef to save memory      */
#define	CONFIG_SYS_PROMPT		"VR9 # "	/* Monitor Command Prompt    */
#define	CONFIG_SYS_CBSIZE		512		/* Console I/O Buffer Size   */
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)  /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		32		/* max number of command args*/

#if 0 /* ctc */
  #define CONFIG_SYS_MALLOC_LEN		(4096*1024)
  #define CONFIG_SYS_BOOTPARAMS_LEN	(256*1024)
#else
  #define CONFIG_SYS_MALLOC_LEN      ((4096+256)*1024)
  #define CONFIG_SYS_BOOTPARAMS_LEN	(256*1024)
#endif


/********************************************************************************************
 * CONFIG_SYS_MIPS_TIMER_FREQ will be used as the frequency of the mips cp0 count register(CC)
 * The resolution should be obtained from rdhwr like following:
 * .set    mips32r2
 * li      t1, 0x8
 * mtc0    t1, CP0_HWRENA
 * rdhwr   t0, $3
 * The value in t0(CCRes) denotes the number of cycles between update of the register
 * In the 34K core, the CCRes value is 2 to indicate that the CC register increments every
 * second CPU cycle, which means that the CC frequency is half of the cpu frequency
***********************************************************************************************/
#define CONFIG_SYS_MIPS_TIMER_FREQ	(CPU_CLOCK_RATE/2)


#define CONFIG_SYS_SDRAM_BASE		0x80000000

#define	CONFIG_SYS_LOAD_ADDR		0x80100000	/* default load address	*/

#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	(256)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xb0000000 /* Flash Bank #1 */
#define PHYS_FLASH_2		0xb0800000 /* Flash Bank #2 */

#define CONFIG_SYS_FLASH_SWAP_ADDR

#define FLASH_FIXUP_ADDR_8(addr)        ((void*)((ulong)(addr)^2))
#define FLASH_FIXUP_ADDR_16(addr)       ((void*)((ulong)(addr)^2))

#define CONFIG_SYS_FLASH_CFI_WIDTH      FLASH_CFI_16BIT

#if 1 /* ctc */
 #define CONFIG_EBU_ADDSEL0               0x10000021 //0x10000031, ctc extend to 32MB
#else
 #define CONFIG_EBU_ADDSEL0               0x10000031
#endif
#define CONFIG_EBU_BUSCON0               0x0001D7FF


/* The following #defines are needed to get flash environment right */
#define	CONFIG_SYS_MONITOR_BASE	CONFIG_RAM_TEXT_BASE

#define	CONFIG_SYS_MONITOR_LEN		(512 << 10)

#define CONFIG_SYS_INIT_SP_OFFSET	0x400000

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Write */


#define CONFIG_FLASH_16BIT

#define CONFIG_NR_DRAM_BANKS	1

#define CONFIG_NET_MULTI

#define CONFIG_SYS_MAX_NAND_DEVICE    1
#define CONFIG_SYS_NAND_BASE   0xb4000000

#define NAND_BASE_ADDRESS  0xB4000000
#define CFG_NAND_BASE NAND_BASE_ADDRESS
#define NAND_WRITE(addr, val)     *((volatile u8*)(NAND_BASE_ADDRESS | (addr))) = val;while((*EBU_NAND_WAIT & 0x08) == 0);
//#define NAND_WRITE(addr, val)     *((volatile u8*)(NAND_BASE_ADDRESS | (addr)))=val;
#define NAND_READ(addr, val)      val = *((volatile u8*)(NAND_BASE_ADDRESS | (addr))); while((*EBU_NAND_WAIT & 0x08) == 0);
#define NAND_CE_SET         *EBU_NAND_CON   = 0x00F405F3
#define NAND_CE_CLEAR       *EBU_NAND_CON   = 0x000005F3
#define NAND_READY       ( ((*EBU_NAND_WAIT)&0x07) == 7)

#define NAND_READY_CLEAR  *EBU_NAND_WAIT = 0;
#define WRITE_CMD    0x18
#define WRITE_ADDR   0x14
#define WRITE_LADDR  0x10
#define WRITE_DATA  0x10
#define READ_DATA    0x10
#define READ_LDATA   0x00
#define ACCESS_WAIT
#define IFX_ATC_NAND 0xc176
#define IFX_BTC_NAND 0xc166
#define ST_512WB2_NAND 0x2076
#define NAND_LATCH_ENABLE 
#define NAND_CLRALE  *EBU_NAND_CON &=~(1<<18);
#define NAND_SETALE  *EBU_NAND_CON |=1<<18;
#define NAND_CLRCLE  *EBU_NAND_CON &=~(1<<19);
#define NAND_SETCLE  *EBU_NAND_CON |=1<<19;


#define NAND_DISABLE_CE(nand) *EBU_NAND_CON = *EBU_NAND_CON & ~(1<<20);
#define NAND_ENABLE_CE(nand)  *EBU_NAND_CON = *EBU_NAND_CON |(1<<20);
#define NAND_WAIT_READY(nand) while(!NAND_READY){}
#define WRITE_NAND_COMMAND(d, adr) NAND_WRITE(WRITE_CMD,d);
#define WRITE_NAND_ADDRESS(d, adr) NAND_WRITE(WRITE_ADDR,d);
#define WRITE_NAND(d, adr) NAND_WRITE(WRITE_DATA,d);
#define READ_NAND(adr) *((volatile u8*)(NAND_BASE_ADDRESS | (READ_DATA)))
#define IFX_NAND_CTL_CLRALE *EBU_NAND_CON &=~(1<<18);
#define IFX_NAND_CTL_SETALE *EBU_NAND_CON |=1<<18;


#if 1 //ctc
 #define CONFIG_SYS_NAND_PAGE_SIZE   CONFIG_NAND_PAGE_SIZE   /* NAND chip page size        */
 #if CONFIG_SYS_NAND_PAGE_SIZE <= 512
  #define CONFIG_SYS_NAND_BLOCK_SIZE  ( CONFIG_SYS_NAND_PAGE_SIZE << 5 ) /* NAND chip block size       */
 #else
  #define CONFIG_SYS_NAND_BLOCK_SIZE  ( CONFIG_SYS_NAND_PAGE_SIZE << 7 ) /* NAND chip block size       */
 #endif
#else
 #define CONFIG_SYS_NAND_PAGE_SIZE   ( 2<<10 )   /* NAND chip page size        */
 #define CONFIG_SYS_NAND_BLOCK_SIZE  ( 256 << 10 ) /* NAND chip block size       */
#endif
#define CONFIG_SYS_NAND_PAGE_COUNT  (CONFIG_SYS_NAND_BLOCK_SIZE / CONFIG_SYS_NAND_PAGE_SIZE)
                        /* NAND chip page count       */
#if CONFIG_SYS_NAND_PAGE_SIZE <= 512 //ctc
 #define CONFIG_SYS_NAND_BAD_BLOCK_POS   5       /* Location of bad block marker*/
#else
 #define CONFIG_SYS_NAND_BAD_BLOCK_POS   0       /* Location of bad block marker*/
#endif
#define CONFIG_SYS_NAND_5_ADDR_CYCLE            /* Fifth addr used (<=128MB)  */

#if CONFIG_SYS_NAND_PAGE_SIZE <= 512 //ctc
 #define CONFIG_SYS_NAND_ECCSIZE 512
 #define CONFIG_SYS_NAND_ECCBYTES    2
 #define CONFIG_SYS_NAND_ECCSTEPS    (CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
 #define CONFIG_SYS_NAND_OOBSIZE 16
 #define CONFIG_SYS_NAND_ECCTOTAL    (CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
 #define CONFIG_SYS_NAND_ECCPOS      {5, 6}
#else
 #define CONFIG_SYS_NAND_ECCSIZE 256
 #define CONFIG_SYS_NAND_ECCBYTES    3
 #define CONFIG_SYS_NAND_ECCSTEPS    (CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_ECCSIZE)
 #define CONFIG_SYS_NAND_OOBSIZE 64
 #define CONFIG_SYS_NAND_ECCTOTAL    (CONFIG_SYS_NAND_ECCBYTES * CONFIG_SYS_NAND_ECCSTEPS)
 #define CONFIG_SYS_NAND_ECCPOS      {40, 41, 42, 43, 44, 45, 46, 47, \
					                  48, 49, 50, 51, 52, 53, 54, 55, \
    				                  56, 57, 58, 59, 60, 61, 62, 63}
#endif
														  


#define CONFIG_SYS_NAND_U_BOOT_SIZE  ( 256 << 10 )
#define CONFIG_SYS_NAND_U_BOOT_DST   CONFIG_BOOTSTRAP_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_START CONFIG_BOOTSTRAP_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_OFFS  16384

#if defined(CONFIG_BOOT_FROM_NOR)
 #if defined(CONFIG_NAND_FLASH)
  #define IFX_CFG_FLASH_DDR_CFG_START_ADDR      0xB003FFE8
  #define IFX_CFG_FLASH_DDR_CFG_SIZE            24
  #define IFX_CFG_FLASH_DDR_CFG_END_ADDR        0xb003ffff
 #else
  #define IFX_CFG_FLASH_DDR_CFG_START_ADDR      0xB001FFE8
  #define IFX_CFG_FLASH_DDR_CFG_SIZE            24
  #define IFX_CFG_FLASH_DDR_CFG_END_ADDR        0xb001ffff
 #endif
#elif defined(CONFIG_BOOT_FROM_SPI)
#define IFX_CFG_FLASH_DDR_CFG_START_ADDR      0x0000FFE8
#define IFX_CFG_FLASH_DDR_CFG_SIZE            24
#define IFX_CFG_FLASH_DDR_CFG_END_ADDR        0x0000ffff
#define CONFIG_ENV_SECT_SIZE 0x1000
#else /*CONFIG_BOOT_FRON_NAND*/
#define IFX_CFG_FLASH_DDR_CFG_START_ADDR      0x00003FE8 //(CONFIG_SYS_NAND_U_BOOT_SIZE-24)
#define IFX_CFG_FLASH_DDR_CFG_SIZE            24
#define IFX_CFG_FLASH_DDR_CFG_END_ADDR        0x00003fff //(CONFIG_SYS_NAND_U_BOOT_SIZE-1)
#endif

/* Address and size of Primary Environment Sector	*/
#define CONFIG_ENV_OFFSET		IFX_CFG_FLASH_UBOOT_CFG_START_ADDR
#define CONFIG_ENV_ADDR		  IFX_CFG_FLASH_UBOOT_CFG_START_ADDR
#ifdef IFX_CFG_FLASH_UBOOT_CFG_REAL_SIZE /* ctc */
  #define CONFIG_ENV_SIZE		  IFX_CFG_FLASH_UBOOT_CFG_REAL_SIZE
#else
  #define CONFIG_ENV_SIZE		  IFX_CFG_FLASH_UBOOT_CFG_SIZE
#endif

#define CONFIG_TUNING_STATUS 0xBE22FF20 
#define CONFIG_TUNING_SP     0xBE22FF00 


#define MTDIDS_DEFAULT      "nand0=ifx_nand"
#define MTDPARTS_DEFAULT    "mtdparts=ifx_nand:256k(uboot),10m(rootfs),-(res)"


#endif	/* __CONFIG_H */
