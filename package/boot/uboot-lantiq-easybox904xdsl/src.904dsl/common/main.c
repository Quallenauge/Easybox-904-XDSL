/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Add to readline cmdline-editing by
 * (C) Copyright 2005
 * JinHua Luo, GuangDong Linux Center, <luo.jinhua@gd-linux.com>
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

/* #define	DEBUG	*/

#include <common.h>
#include <watchdog.h>
#include <command.h>
#ifdef CONFIG_MODEM_SUPPORT
#include <malloc.h>		/* for free() prototype */
#endif

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#include <post.h>

#if defined(CONFIG_SILENT_CONSOLE) || defined(CONFIG_POST) || defined(CONFIG_CMDLINE_EDITING)
DECLARE_GLOBAL_DATA_PTR;
#endif

#if 1 // ctc
 #include <lzma/LzmaTypes.h>
 #include <lzma/LzmaDec.h>
 #include <lzma/LzmaTools.h>
#endif

// Arcadyan Begin
#ifdef CONFIG_DRIVER_VR9
#include <asm/vr9.h>
#endif

static int pass_mode = 0;

//#define LCD_IMAGE_RESCUE_OFFSET              81              // white background
#define LCD_IMAGE_RESCUE_OFFSET                86                      // red background
#define LCD_IMAGE_BOOT_FAILURE         (LCD_IMAGE_RESCUE_OFFSET + 0)
#define LCD_IMAGE_RESCUE                       (LCD_IMAGE_RESCUE_OFFSET + 4)
#define LCD_IMAGE_RESCUE_UPGRADE       (LCD_IMAGE_RESCUE_OFFSET + 2)
#define LCD_IMAGE_RESCUE_DONE          (LCD_IMAGE_RESCUE_OFFSET + 3)
#define LCD_IMAGE_RESCUE_ERR           (LCD_IMAGE_RESCUE_OFFSET + 1)

#define BOOT_RETRY_MAX 6
// Arcadyan End
/*
 * Board-specific Platform code can reimplement show_boot_progress () if needed
 */
void __attribute__((weak)) show_boot_progress(int val) {}

#if defined(CONFIG_BOOT_RETRY_TIME) && defined(CONFIG_RESET_TO_RETRY)
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);		/* for do_reset() prototype */
#endif

extern int do_bootd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#if defined(CONFIG_UPDATE_TFTP)
void update_tftp (void);
#endif /* CONFIG_UPDATE_TFTP */

#define MAX_DELAY_STOP_STR 32

#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
static int abortboot(int);
#endif

#undef DEBUG_PARSER

char        console_buffer[CONFIG_SYS_CBSIZE + 1];	/* console I/O buffer	*/

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
static char erase_seq[] = "\b \b";		/* erase sequence	*/
static char   tab_seq[] = "        ";		/* used to expand TABs	*/

#ifdef CONFIG_BOOT_RETRY_TIME
static uint64_t endtime = 0;  /* must be set, default is instant timeout */
static int      retry_time = -1; /* -1 so can call readline before main_loop */
#endif

#define	endtick(seconds) (get_ticks() + (uint64_t)(seconds) * get_tbclk())

#ifndef CONFIG_BOOT_RETRY_MIN
#define CONFIG_BOOT_RETRY_MIN CONFIG_BOOT_RETRY_TIME
#endif

#ifdef CONFIG_MODEM_SUPPORT
int do_mdm_init = 0;
extern void mdm_init(void); /* defined in board.c */
#endif

#ifdef CONFIG_DRIVER_VR9 /* ctc, RTL8367RB and power down PHY */
//u32 rtl_reset_elapsed_msec;
 u32 rtl_min_wait = 4400;
 extern u16  vr9_mdio_read( u8 phyaddr, u8 phyreg );
 extern void vr9_mdio_write( u8 phyaddr, u8 phyreg, u16 data );
#endif

/***************************************************************************
 * Watch for 'delay' seconds for autoboot stop or autoboot delay string.
 * returns: 0 -  no key string, allow autoboot
 *          1 - got key string, abort
 */
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
# if defined(CONFIG_AUTOBOOT_KEYED)
static __inline__ int abortboot(int bootdelay)
{
	int abort = 0;
	uint64_t etime = endtick(bootdelay);
	struct {
		char* str;
		u_int len;
		int retry;
	}
	delaykey [] = {
		{ str: getenv ("bootdelaykey"),  retry: 1 },
		{ str: getenv ("bootdelaykey2"), retry: 1 },
		{ str: getenv ("bootstopkey"),   retry: 0 },
		{ str: getenv ("bootstopkey2"),  retry: 0 },
	};

	char presskey [MAX_DELAY_STOP_STR];
	u_int presskey_len = 0;
	u_int presskey_max = 0;
	u_int i;

#  ifdef CONFIG_AUTOBOOT_PROMPT
	printf(CONFIG_AUTOBOOT_PROMPT);
#  endif

#  ifdef CONFIG_AUTOBOOT_DELAY_STR
	if (delaykey[0].str == NULL)
		delaykey[0].str = CONFIG_AUTOBOOT_DELAY_STR;
#  endif
#  ifdef CONFIG_AUTOBOOT_DELAY_STR2
	if (delaykey[1].str == NULL)
		delaykey[1].str = CONFIG_AUTOBOOT_DELAY_STR2;
#  endif
#  ifdef CONFIG_AUTOBOOT_STOP_STR
	if (delaykey[2].str == NULL)
		delaykey[2].str = CONFIG_AUTOBOOT_STOP_STR;
#  endif
#  ifdef CONFIG_AUTOBOOT_STOP_STR2
	if (delaykey[3].str == NULL)
		delaykey[3].str = CONFIG_AUTOBOOT_STOP_STR2;
#  endif

	for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i ++) {
		delaykey[i].len = delaykey[i].str == NULL ?
				    0 : strlen (delaykey[i].str);
		delaykey[i].len = delaykey[i].len > MAX_DELAY_STOP_STR ?
				    MAX_DELAY_STOP_STR : delaykey[i].len;

		presskey_max = presskey_max > delaykey[i].len ?
				    presskey_max : delaykey[i].len;

#  if DEBUG_BOOTKEYS
		printf("%s key:<%s>\n",
		       delaykey[i].retry ? "delay" : "stop",
		       delaykey[i].str ? delaykey[i].str : "NULL");
#  endif
	}

	/* In order to keep up with incoming data, check timeout only
	 * when catch up.
	 */
	do {
		if (tstc()) {
			if (presskey_len < presskey_max) {
				presskey [presskey_len ++] = getc();
			}
			else {
				for (i = 0; i < presskey_max - 1; i ++)
					presskey [i] = presskey [i + 1];

				presskey [i] = getc();
			}
		}

		for (i = 0; i < sizeof(delaykey) / sizeof(delaykey[0]); i ++) {
			if (delaykey[i].len > 0 &&
			    presskey_len >= delaykey[i].len &&
			    memcmp (presskey + presskey_len - delaykey[i].len,
				    delaykey[i].str,
				    delaykey[i].len) == 0) {
#  if DEBUG_BOOTKEYS
				printf("got %skey\n",
				       delaykey[i].retry ? "delay" : "stop");
#  endif

#  ifdef CONFIG_BOOT_RETRY_TIME
				/* don't retry auto boot */
				if (! delaykey[i].retry)
					retry_time = -1;
#  endif
				abort = 1;
			}
		}
	} while (!abort && get_ticks() <= etime);

#  if DEBUG_BOOTKEYS
	if (!abort)
		puts("key timeout\n");
#  endif

#ifdef CONFIG_SILENT_CONSOLE
	if (abort)
		gd->flags &= ~GD_FLG_SILENT;
#endif

	return abort;
}

# else	/* !defined(CONFIG_AUTOBOOT_KEYED) */

#ifdef CONFIG_MENUKEY
static int menukey = 0;
#endif

static __inline__ int abortboot(int bootdelay)
{
	int abort = 0;
  #if defined(CONFIG_DRIVER_VR9) /* ctc for RTL8367RB */
	u32 rtl_elapsed_sec = 0;
  #endif

#ifdef CONFIG_MENUPROMPT
	printf(CONFIG_MENUPROMPT);
#else
	printf("Hit any key to stop autoboot: %2d ", bootdelay);
#endif

#if defined CONFIG_ZERO_BOOTDELAY_CHECK
	/*
	 * Check if key already pressed
	 * Don't check if bootdelay < 0
	 */
	if (bootdelay >= 0) {
		if (tstc()) {	/* we got a key press	*/
			(void) getc();  /* consume input	*/
			puts ("\b\b\b 0");
			abort = 1;	/* don't auto boot	*/
		}
	}
#endif

	while ((bootdelay > 0) && (!abort)) {
		int i;

		--bootdelay;
		/* delay 100 * 10ms */
		for (i=0; !abort && i<100; ++i) {
			if (tstc()) {	/* we got a key press	*/
				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
# ifdef CONFIG_MENUKEY
				menukey = getc();
# else
				(void) getc();  /* consume input	*/
# endif

			  #if 0 //defined(CONFIG_DRIVER_VR9) /* ctc for RTL8367RB */
				rtl_reset_elapsed_msec = rtl_elapsed_sec * 1000 + i * 10;
			  #endif
				
				break;
			}
			udelay(10000);
		}

	  #if defined(CONFIG_DRIVER_VR9) /* ctc for RTL8367RB */
		rtl_elapsed_sec++;
	  #endif

		printf("\b\b\b%2d ", bootdelay);
	}

	putc('\n');

#ifdef CONFIG_SILENT_CONSOLE
	if (abort)
		gd->flags &= ~GD_FLG_SILENT;
#endif

  #if 0 //defined(CONFIG_DRIVER_VR9) /* ctc for RTL8367RB */
	if (!abort)
		rtl_reset_elapsed_msec = rtl_elapsed_sec * 1000;
  #endif

	return abort;
}
# endif	/* CONFIG_AUTOBOOT_KEYED */
#endif	/* CONFIG_BOOTDELAY >= 0  */

//#define UDEBUG	//Trace Uboot Debug Message
#ifdef CONFIG_DRIVER_VR9
extern int rtl_sw_reset(u32 delay_msec);
extern int smi_read(unsigned int mAddrs, unsigned int *rData);
extern int rtl8367b_setAsicPHYReg(unsigned int phyNo, unsigned int phyAddr, unsigned int value);
extern int rtl_sw_init(void);
#endif
/****************************************************************************/

void main_loop (void)
{
	int bootid, bootnum;
	char *s1;
	char buf[16];
	
	#ifndef CONFIG_SYS_HUSH_PARSER
	static char lastcommand[CONFIG_SYS_CBSIZE] = { 0, };
	int len;
	int rc = 1;
	int flag;
	#endif

	#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	char *s;
	int bootdelay;
	#endif

	#ifdef CONFIG_PREBOOT
	char *p;
	#endif

	#ifdef CONFIG_BOOTCOUNT_LIMIT
	unsigned long bootcount = 0;
	unsigned long bootlimit = 0;
	char *bcs;
	char bcs_set[16];
	#endif /* CONFIG_BOOTCOUNT_LIMIT */

	#if 1 /* ctc, save bootloader version */
	char* val;
	extern const char version_string[];
	extern int do_sleep_msec( u32 delay );
  #endif

	#ifdef CONFIG_DRIVER_VR9
	unsigned int data, retVal, phyNo, timespent=0;
	#endif
	
	#if defined(CONFIG_VFD) && defined(VFD_TEST_LOGO)
	ulong bmp = 0;		/* default bitmap */
	extern int trab_vfd (ulong bitmap);

	#ifdef CONFIG_MODEM_SUPPORT
	if (do_mdm_init)
		bmp = 1;	/* alternate bitmap */
	#endif
	trab_vfd (bmp);
	#endif	/* CONFIG_VFD && VFD_TEST_LOGO */

#if 0 //defined(UDEBUG)
	printf("[%s:%d]\n", __func__, __LINE__);
#endif


	#ifdef CONFIG_BOOTCOUNT_LIMIT
	bootcount = bootcount_load();
	bootcount++;
	bootcount_store (bootcount);
	sprintf (bcs_set, "%lu", bootcount);
	setenv ("bootcount", bcs_set);
	bcs = getenv ("bootlimit");
	bootlimit = bcs ? simple_strtoul (bcs, NULL, 10) : 0;
	#endif /* CONFIG_BOOTCOUNT_LIMIT */

	#ifdef CONFIG_MODEM_SUPPORT
	debug ("DEBUG: main_loop:   do_mdm_init=%d\n", do_mdm_init);
	if (do_mdm_init) {
		char *str = strdup(getenv("mdm_cmd"));
		setenv ("preboot", str);  /* set or delete definition */
		if (str != NULL)
			free (str);
		mdm_init(); /* wait for modem connection */
	}
	#endif  /* CONFIG_MODEM_SUPPORT */

	#ifdef CONFIG_VERSION_VARIABLE
	{
		extern char version_string[];

		setenv ("ver", version_string);  /* set version variable */
	}
	#endif /* CONFIG_VERSION_VARIABLE */

	#ifdef CONFIG_SYS_HUSH_PARSER
	u_boot_hush_start ();
	#endif

	#if defined(CONFIG_HUSH_INIT_VAR)
	hush_init_var ();
	#endif

	#ifdef CONFIG_AUTO_COMPLETE
	install_auto_complete();
	#endif

	#ifdef CONFIG_PREBOOT
	if ((p = getenv ("preboot")) != NULL) {
# ifdef CONFIG_AUTOBOOT_KEYED
		int prev = disable_ctrlc(1);	/* disable Control C checking */
# endif

# ifndef CONFIG_SYS_HUSH_PARSER
		run_command (p, 0);
# else
		parse_string_outer(p, FLAG_PARSE_SEMICOLON |
				    FLAG_EXIT_FROM_LOOP);
# endif

# ifdef CONFIG_AUTOBOOT_KEYED
		disable_ctrlc(prev);	/* restore Control C checking */
# endif
	}
	#endif /* CONFIG_PREBOOT */

	#if defined(CONFIG_UPDATE_TFTP)
	update_tftp ();
	#endif /* CONFIG_UPDATE_TFTP */

	#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	s = getenv ("bootdelay");
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

#if defined(UDEBUG)
	printf("[%s:%d]### main_loop entered: bootdelay=%d\n\n",__func__,__LINE__,bootdelay);
#endif
	debug ("### main_loop entered: bootdelay=%d\n\n", bootdelay);

# ifdef CONFIG_BOOT_RETRY_TIME
	init_cmd_timeout ();
# endif	/* CONFIG_BOOT_RETRY_TIME */

	#ifdef CONFIG_POST
	if (gd->flags & GD_FLG_POSTFAIL) {
		s = getenv("failbootcmd");
	}
	else
	#endif /* CONFIG_POST */
	#ifdef CONFIG_BOOTCOUNT_LIMIT
	if (bootlimit && (bootcount > bootlimit)) {
		printf ("Warning: Bootlimit (%u) exceeded. Using altbootcmd.\n",
		        (unsigned)bootlimit);
		s = getenv ("altbootcmd");
	}
	else
	#endif /* CONFIG_BOOTCOUNT_LIMIT */
		s = getenv ("bootcmd");

#if defined(UDEBUG)
	printf("[%s:%d]### main_loop: bootcmd=\"%s\"\n",__func__,__LINE__,s ? s : "<UNDEFINED>");
#endif
	debug ("### main_loop: bootcmd=\"%s\"\n", s ? s : "<UNDEFINED>");

  #ifdef CONFIG_DRIVER_VR9 /* ctc, power down PHY */
	#if 0 //defined(UDEBUG)
	printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
	#endif
	vr9_mdio_write( 0x00, 0x0, vr9_mdio_read( 0x00, 0x0 ) | 0x0800 );
	vr9_mdio_write( 0x01, 0x0, vr9_mdio_read( 0x01, 0x0 ) | 0x0800 );
	vr9_mdio_write( 0x11, 0x0, vr9_mdio_read( 0x11, 0x0 ) | 0x0800 );
	vr9_mdio_write( 0x12, 0x0, vr9_mdio_read( 0x12, 0x0 ) | 0x0800 );
	vr9_mdio_write( 0x13, 0x0, vr9_mdio_read( 0x13, 0x0 ) | 0x0800 );
	vr9_mdio_write( 0x14, 0x0, vr9_mdio_read( 0x14, 0x0 ) | 0x0800 );
	#if 0 //defined(UDEBUG)
	printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
	#endif
	//printf("rtl8367rb reset waiting 1.5 sec ....\n");	
	rtl_sw_reset(100);
	for(phyNo=0;phyNo<4;phyNo++)
	{
			if(0 != (retVal = smi_read(0x2000+(phyNo<<5),&data)))
			{
					printf("[%s:%d] read PCS reg[0x%x] failed!!!, errno=%x\n",__func__,__LINE__,0x2000+(phyNo<<5),retVal);
			}
			else
			{
					if(0 != (retVal = rtl8367b_setAsicPHYReg(phyNo,0x2000+(phyNo<<5),(data|0x0800))))
					{
							printf("[%s:%d] set PCS reg[0x%x] failed!!!, errno=%x\n",__func__,__LINE__,0x2000+(phyNo<<5),retVal);
					}
			}
	}

	do_sleep_msec(1000);
	/* Check reset successful */
	do{
			if(0 != (retVal = smi_read(0x1202,&data)))
			{
					printf("[%s:%d] read reg[0x%x]=0x%x, Warrning : rtl8367rb reset failed!!!\n",__func__,__LINE__,0x1202,data);
					break;
			}

			do_sleep_msec(100);
			timespent+=100;
			if(timespent == 500)
			{
					printf("Warrning : rtl8367rb reset failed!!!\n");
			}
			
	}while(0x88A8 != data);
 	//printf("rtl8367rb bootcode 2012.10.03 .....\n");
  #endif

  #if 1 /* ctc, save bootloader version */
	val = getenv ("bl_version");
	if ( val == NULL || strcmp(val,version_string) != 0 ) {
		setenv ("bl_version", (char*)version_string);
		saveenv();
	}
  #endif

  #if 1 /* ctc, save key pad default calibration value */
	val = getenv ("key1_cal");
	if ( val == NULL ) {
		setenv( "key1_cal", "10" );
		setenv( "key2_cal", "10" );
		setenv( "key3_cal", "10" );
		setenv( "key4_cal", "10" );
		setenv( "key5_cal", "10" );
		saveenv();
	}
  #endif

	// Arcadyan Begin, check reset button
#if defined(UDEBUG)
	printf( "GPIO 40 (Reset) = %d\n", (*BSP_GPIO_P2_IN & 0x100) >> 8 );   // Reset button
	//printf( "GPIO 41 ( WPS ) = %d\n", (*BSP_GPIO_P2_IN & 0x200) >> 9 );   // WPS button
#endif

	int count = 0;
	while ((*BSP_GPIO_P2_IN & 0x100) == 0) {        // GPIO 40, 0 active
		count++;
		printf("reset button pressed --> %d\n", count);
		udelay(1000000);
	}
	
	if (count > 0) {
		int ret;

		printf("enter rescue process ...\n");

		// power on phys
		vr9_mdio_write( 0x00, 0x0, vr9_mdio_read( 0x00, 0x0 ) & ~0x0800 );
		vr9_mdio_write( 0x01, 0x0, vr9_mdio_read( 0x01, 0x0 ) & ~0x0800 );
		vr9_mdio_write( 0x11, 0x0, vr9_mdio_read( 0x11, 0x0 ) & ~0x0800 );
		vr9_mdio_write( 0x12, 0x0, vr9_mdio_read( 0x12, 0x0 ) & ~0x0800 );
		vr9_mdio_write( 0x13, 0x0, vr9_mdio_read( 0x13, 0x0 ) & ~0x0800 );
		vr9_mdio_write( 0x14, 0x0, vr9_mdio_read( 0x14, 0x0 ) & ~0x0800 );
		#if 0 //defined(UDEBUG)
		printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
		printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
		printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
		printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
		printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
		printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
		#endif

	  #ifdef CONFIG_DRIVER_VR9 
		if(0 != rtl_sw_init())
		{
				printf("[%s:%d] rtl8367rb init failed!!! ....\n",__func__,__LINE__);	
		}
		
		for(phyNo=0;phyNo<4;phyNo++)
		{
				if(0 != (retVal = smi_read(0x2000+(phyNo<<5),&data)))
				{
						printf("[%s:%d] read PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
				}
				else
				{
						if(0 != (retVal = rtl8367b_setAsicPHYReg(phyNo,0x2000+(phyNo<<5),(data & (~0x0800)))))
						{
								printf("[%s:%d] set PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
						}
				}
		}
		#endif

		lcd_DisplayPage(LCD_IMAGE_RESCUE);

		udelay(5000000);                                                        // wait 5 seconds

		run_command ("tftpboot $(loadaddr) $(tftppath)$(fullimage)", 0);                // tftp full image

		if (verify_firmware() != 0) {
up_error:
			// display error message
			lcd_DisplayPage(LCD_IMAGE_RESCUE_ERR);

			while(1);
		}

		lcd_DisplayPage(LCD_IMAGE_RESCUE_UPGRADE);

		//udelay(5000000);                                                      // wait 5 seconds

		run_command ("nand erase $(f_sysconfig_addr) $(f_sysconfig_size)", 0);  // erase sysconfig
		run_command ("nand erase $(f_rootfs2_addr) $(f_rootfs2_size)", 0);              // erase rootfs 2
		run_command ("nand erase $(f_kernel2_addr) $(f_kernel2_size)", 0);              // erase kernel 2
		run_command ("unset bootnum", 0);										// reset "bootnum"
		run_command ("unset bootid; save", 0);                                                                  // reset "bootid" environment variable
		run_command ("upgrade $(loadaddr) $(filesize)", 0);                                             // upgrade fullimage

		//printf("get_up_result = %d\n", get_up_result());

		if (get_up_result() != 0) {
			goto up_error;
		}

		lcd_DisplayPage(LCD_IMAGE_RESCUE_DONE);

		while(1);               // wait here for rebooting
	}
	// Arcadyan End

	if (bootdelay >= 0 && s && !abortboot (bootdelay)) 
	{
	  #ifdef CONFIG_DRIVER_VR9 /* ctc, power up PHY */
		int lcd_boot_Init(void);
		if ( lcd_boot_Init() != 0 )
			goto ctrlc_lcd_Init;
	  #endif

		// Arcadyan Begin
		s1      = getenv ("bootnum");
		bootnum = s1 ? (int)simple_strtol(s1, NULL, 10) : 0;
		s1      = getenv ("bootid");
		bootid  = s1 ? (int)simple_strtol(s1, NULL, 10) : 0;

		printf("1. bootid : %d, bootnum : %d\n", bootid, bootnum);

		if (bootnum >= 2 * BOOT_RETRY_MAX) {
			goto boot_fail;
		}

		// switch another partition
		if (bootnum == BOOT_RETRY_MAX) {

			if (bootid == 2) {
				bootid = 0;
				run_command ("unset bootid", 0);
			}
			else {
				bootid = 2;
				run_command ("setenv bootid 2", 0);
			}

			s = getenv ("bootcmd");
		}

		bootnum++;
		sprintf(buf, "%d", bootnum);
		setenv("bootnum", buf);
// Qualle: Disable bootnum failure behavior for now to save the flash
//		saveenv();
		printf("2. bootid : %d, bootnum : %d\n", bootid, bootnum);

		// Arcadyan End


# ifdef CONFIG_AUTOBOOT_KEYED
		int prev = disable_ctrlc(1);	/* disable Control C checking */
# endif

# ifndef CONFIG_SYS_HUSH_PARSER
		run_command (s, 0);
# else
		parse_string_outer(s, FLAG_PARSE_SEMICOLON |
				    FLAG_EXIT_FROM_LOOP);
# endif

# ifdef CONFIG_AUTOBOOT_KEYED
		disable_ctrlc(prev);	/* restore Control C checking */
# endif

  #ifdef CONFIG_DRIVER_VR9 /* ctc, power up PHY */
	#if 1 //defined(UDEBUG)
	printf("-------------- Current state before activating switch ------\n");
	printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
	printf("------------------------------------------------------------\n");
	#endif
	do_sleep_msec( 100/*1000*/ );
	vr9_mdio_write( 0x00, 0x0, vr9_mdio_read( 0x00, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x01, 0x0, vr9_mdio_read( 0x01, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x11, 0x0, vr9_mdio_read( 0x11, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x12, 0x0, vr9_mdio_read( 0x12, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x13, 0x0, vr9_mdio_read( 0x13, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x14, 0x0, vr9_mdio_read( 0x14, 0x0 ) & ~0x0800 );
	#if 1 //defined(UDEBUG)
	printf("-------------- Current state after activating switch -------\n");
	printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
	printf("------------------------------------------------------------\n");
	#endif
	
	if(0 != rtl_sw_init())
	{
			printf("[%s:%d] rtl8367rb init failed!!! ....\n",__func__,__LINE__);	
	}
	
	for(phyNo=0;phyNo<4;phyNo++)
	{
			if(0 != (retVal = smi_read(0x2000+(phyNo<<5),&data)))
			{
					printf("[%s:%d] read PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
			}
			else
			{
					if(0 != (retVal = rtl8367b_setAsicPHYReg(phyNo,0x2000+(phyNo<<5),(data & (~0x0800)))))
					{
							printf("[%s:%d] set PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
					}
			}
	}
	
	printf("rtl8367rb init successful ....\n");	
  #endif
		// Arcadyan Begin
		// if you come to here, the kernel in current partition is wrong.
		if (bootnum < BOOT_RETRY_MAX) {
			bootnum = BOOT_RETRY_MAX;

			if (bootid == 2) {
				bootid = 0;
				run_command ("unset bootid", 0);
			}
			else {
				bootid = 2;
				run_command ("setenv bootid 2", 0);
			}

			bootnum++;
			sprintf(buf, "%d", bootnum);
			setenv("bootnum", buf);

			saveenv();

			printf("3. bootid : %d, bootnum : %d\n", bootid, bootnum);

			s = getenv ("bootcmd");
			run_command (s, 0);
		}

		bootnum = 2 * BOOT_RETRY_MAX;
		sprintf(buf, "%d", bootnum);
		setenv("bootnum", buf);
// Qualle: Disable bootnum failure behavior for now to save the flash
//		saveenv();



boot_fail:
		// booting failed, notice user to enter rescue process
		lcd_DisplayPage(LCD_IMAGE_BOOT_FAILURE);
		// Arcadyan End
	}

ctrlc_lcd_Init:
	// Arcadyan Begin , check password
	pass_mode = 1;
	len = readline("\n\nplease input password : ");
	if (verify_password(console_buffer) != 0) {
		int i = 5;
		printf("\n\nwrong password, rebooting ...  ");
		
		while (i >= 0) {
			printf("\b%d", i--);
			udelay(1000000);
		}
		
		run_command("reset", 0);
		
		while(1);
	}
	pass_mode = 0;
  // Arcadyan End

  #ifdef CONFIG_DRIVER_VR9 /* ctc, power up PHY */
	#if 1 //defined(UDEBUG)
	printf("-------------- Current state before activating switch ------\n");
	printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
	printf("------------------------------------------------------------\n");
	#endif
	do_sleep_msec( 100/*1000*/ );
	vr9_mdio_write( 0x00, 0x0, vr9_mdio_read( 0x00, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x01, 0x0, vr9_mdio_read( 0x01, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x11, 0x0, vr9_mdio_read( 0x11, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x12, 0x0, vr9_mdio_read( 0x12, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x13, 0x0, vr9_mdio_read( 0x13, 0x0 ) & ~0x0800 );
	vr9_mdio_write( 0x14, 0x0, vr9_mdio_read( 0x14, 0x0 ) & ~0x0800 );
	#if 1 //defined(UDEBUG)
	printf("-------------- Current state after activating switch -------\n");
	printf("[%s:%d]vr9_mdio_read( 0x00, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x00, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x01, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x01, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x11, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x11, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x12, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x12, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x13, 0x0 )=0x%x\n",__func__,__LINE__,vr9_mdio_read( 0x13, 0x0 ));
	printf("[%s:%d]vr9_mdio_read( 0x14, 0x0 )=0x%x\n\n",__func__,__LINE__,vr9_mdio_read( 0x14, 0x0 ));
	printf("------------------------------------------------------------\n");
	#endif
	
	if(0 != rtl_sw_init())
	{
			printf("[%s:%d] rtl8367rb init failed!!! ....\n",__func__,__LINE__);	
	}
	
	for(phyNo=0;phyNo<4;phyNo++)
	{
			if(0 != (retVal = smi_read(0x2000+(phyNo<<5),&data)))
			{
					printf("[%s:%d] read PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
			}
			else
			{
					if(0 != (retVal = rtl8367b_setAsicPHYReg(phyNo,0x2000+(phyNo<<5),(data & (~0x0800)))))
					{
							printf("[%s:%d] set PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
					}
			}
	}
	
	printf("rtl8367rb init successful ....\n");	
  #endif


# ifdef CONFIG_MENUKEY
	if (menukey == CONFIG_MENUKEY) {
	    s = getenv("menucmd");
	    if (s) {
# ifndef CONFIG_SYS_HUSH_PARSER
		run_command (s, 0);
# else
		parse_string_outer(s, FLAG_PARSE_SEMICOLON |
				    FLAG_EXIT_FROM_LOOP);
# endif
	    }
	}
#endif /* CONFIG_MENUKEY */
#endif	/* CONFIG_BOOTDELAY */

#ifdef CONFIG_AMIGAONEG3SE
	{
	    extern void video_banner(void);
	    video_banner();
	}
#endif

	/*
	 * Main Loop for Monitor Command Processing
	 */
#ifdef CONFIG_SYS_HUSH_PARSER
	parse_file_outer();
	/* This point is never reached */
	for (;;);
#else
	for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
		if (rc >= 0) {
			/* Saw enough of a valid command to
			 * restart the timeout.
			 */
			reset_cmd_timeout();
		}
#endif
		len = readline (CONFIG_SYS_PROMPT);

		flag = 0;	/* assume no special flags for now */
		if (len > 0)
			strcpy (lastcommand, console_buffer);
		else if (len == 0)
			flag |= CMD_FLAG_REPEAT;
#ifdef CONFIG_BOOT_RETRY_TIME
		else if (len == -2) {
			/* -2 means timed out, retry autoboot
			 */
			puts ("\nTimed out waiting for command\n");
# ifdef CONFIG_RESET_TO_RETRY
			/* Reinit board to run initialization code again */
			do_reset (NULL, 0, 0, NULL);
# else
			return;		/* retry autoboot */
# endif
		}
#endif

		if (len == -1)
			puts ("<INTERRUPT>\n");
		else
			rc = run_command (lastcommand, flag);

		if (rc <= 0) {
			/* invalid command or not repeatable, forget it */
			lastcommand[0] = 0;
		}
	}
#endif /*CONFIG_SYS_HUSH_PARSER*/
}

#ifdef CONFIG_BOOT_RETRY_TIME
/***************************************************************************
 * initialize command line timeout
 */
void init_cmd_timeout(void)
{
	char *s = getenv ("bootretry");

	if (s != NULL)
		retry_time = (int)simple_strtol(s, NULL, 10);
	else
		retry_time =  CONFIG_BOOT_RETRY_TIME;

	if (retry_time >= 0 && retry_time < CONFIG_BOOT_RETRY_MIN)
		retry_time = CONFIG_BOOT_RETRY_MIN;
}

/***************************************************************************
 * reset command line timeout to retry_time seconds
 */
void reset_cmd_timeout(void)
{
	endtime = endtick(retry_time);
}
#endif

#ifdef CONFIG_CMDLINE_EDITING

/*
 * cmdline-editing related codes from vivi.
 * Author: Janghoon Lyu <nandy@mizi.com>
 */

#define putnstr(str,n)	do {			\
		printf ("%.*s", (int)n, str);	\
	} while (0)

#define CTL_CH(c)		((c) - 'a' + 1)

#define MAX_CMDBUF_SIZE		CONFIG_SYS_CBSIZE

#define CTL_BACKSPACE		('\b')
#define DEL			((char)255)
#define DEL7			((char)127)
#define CREAD_HIST_CHAR		('!')

#define getcmd_putch(ch)	putc(ch)
#define getcmd_getch()		getc()
#define getcmd_cbeep()		getcmd_putch('\a')

#define HIST_MAX		20
#define HIST_SIZE		MAX_CMDBUF_SIZE

static int hist_max = 0;
static int hist_add_idx = 0;
static int hist_cur = -1;
unsigned hist_num = 0;

char* hist_list[HIST_MAX];
char hist_lines[HIST_MAX][HIST_SIZE + 1];	 /* Save room for NULL */

#define add_idx_minus_one() ((hist_add_idx == 0) ? hist_max : hist_add_idx-1)

static void hist_init(void)
{
	int i;

	hist_max = 0;
	hist_add_idx = 0;
	hist_cur = -1;
	hist_num = 0;

	for (i = 0; i < HIST_MAX; i++) {
		hist_list[i] = hist_lines[i];
		hist_list[i][0] = '\0';
	}
}

static void cread_add_to_hist(char *line)
{
	strcpy(hist_list[hist_add_idx], line);

	if (++hist_add_idx >= HIST_MAX)
		hist_add_idx = 0;

	if (hist_add_idx > hist_max)
		hist_max = hist_add_idx;

	hist_num++;
}

static char* hist_prev(void)
{
	char *ret;
	int old_cur;

	if (hist_cur < 0)
		return NULL;

	old_cur = hist_cur;
	if (--hist_cur < 0)
		hist_cur = hist_max;

	if (hist_cur == hist_add_idx) {
		hist_cur = old_cur;
		ret = NULL;
	} else
		ret = hist_list[hist_cur];

	return (ret);
}

static char* hist_next(void)
{
	char *ret;

	if (hist_cur < 0)
		return NULL;

	if (hist_cur == hist_add_idx)
		return NULL;

	if (++hist_cur > hist_max)
		hist_cur = 0;

	if (hist_cur == hist_add_idx) {
		ret = "";
	} else
		ret = hist_list[hist_cur];

	return (ret);
}

#ifndef CONFIG_CMDLINE_EDITING
static void cread_print_hist_list(void)
{
	int i;
	unsigned long n;

	n = hist_num - hist_max;

	i = hist_add_idx + 1;
	while (1) {
		if (i > hist_max)
			i = 0;
		if (i == hist_add_idx)
			break;
		printf("%s\n", hist_list[i]);
		n++;
		i++;
	}
}
#endif /* CONFIG_CMDLINE_EDITING */

#define BEGINNING_OF_LINE() {			\
	while (num) {				\
		getcmd_putch(CTL_BACKSPACE);	\
		num--;				\
	}					\
}

#define ERASE_TO_EOL() {				\
	if (num < eol_num) {				\
		int tmp;				\
		for (tmp = num; tmp < eol_num; tmp++)	\
			getcmd_putch(' ');		\
		while (tmp-- > num)			\
			getcmd_putch(CTL_BACKSPACE);	\
		eol_num = num;				\
	}						\
}

#define REFRESH_TO_EOL() {			\
	if (num < eol_num) {			\
		wlen = eol_num - num;		\
		putnstr(buf + num, wlen);	\
		num = eol_num;			\
	}					\
}

static void cread_add_char(char ichar, int insert, unsigned long *num,
	       unsigned long *eol_num, char *buf, unsigned long len)
{
	unsigned long wlen;

	/* room ??? */
	if (insert || *num == *eol_num) {
		if (*eol_num > len - 1) {
			getcmd_cbeep();
			return;
		}
		(*eol_num)++;
	}

	if (insert) {
		wlen = *eol_num - *num;
		if (wlen > 1) {
			memmove(&buf[*num+1], &buf[*num], wlen-1);
		}

		buf[*num] = ichar;
		putnstr(buf + *num, wlen);
		(*num)++;
		while (--wlen) {
			getcmd_putch(CTL_BACKSPACE);
		}
	} else {
		/* echo the character */
		wlen = 1;
		buf[*num] = ichar;
		putnstr(buf + *num, wlen);
		(*num)++;
	}
}

static void cread_add_str(char *str, int strsize, int insert, unsigned long *num,
	      unsigned long *eol_num, char *buf, unsigned long len)
{
	while (strsize--) {
		cread_add_char(*str, insert, num, eol_num, buf, len);
		str++;
	}
}

static int cread_line(const char *const prompt, char *buf, unsigned int *len)
{
	unsigned long num = 0;
	unsigned long eol_num = 0;
	unsigned long wlen;
	char ichar;
	int insert = 1;
	int esc_len = 0;
	char esc_save[8];
	int init_len = strlen(buf);

	if (init_len)
		cread_add_str(buf, init_len, 1, &num, &eol_num, buf, *len);

	while (1) {
#ifdef CONFIG_BOOT_RETRY_TIME
		while (!tstc()) {	/* while no incoming data */
			if (retry_time >= 0 && get_ticks() > endtime)
				return (-2);	/* timed out */
			WATCHDOG_RESET();
		}
#endif

		ichar = getcmd_getch();

		if ((ichar == '\n') || (ichar == '\r')) {
			putc('\n');
			break;
		}

		/*
		 * handle standard linux xterm esc sequences for arrow key, etc.
		 */
		if (esc_len != 0) {
			if (esc_len == 1) {
				if (ichar == '[') {
					esc_save[esc_len] = ichar;
					esc_len = 2;
				} else {
					cread_add_str(esc_save, esc_len, insert,
						      &num, &eol_num, buf, *len);
					esc_len = 0;
				}
				continue;
			}

			switch (ichar) {

			case 'D':	/* <- key */
				ichar = CTL_CH('b');
				esc_len = 0;
				break;
			case 'C':	/* -> key */
				ichar = CTL_CH('f');
				esc_len = 0;
				break;	/* pass off to ^F handler */
			case 'H':	/* Home key */
				ichar = CTL_CH('a');
				esc_len = 0;
				break;	/* pass off to ^A handler */
			case 'A':	/* up arrow */
				ichar = CTL_CH('p');
				esc_len = 0;
				break;	/* pass off to ^P handler */
			case 'B':	/* down arrow */
				ichar = CTL_CH('n');
				esc_len = 0;
				break;	/* pass off to ^N handler */
			default:
				esc_save[esc_len++] = ichar;
				cread_add_str(esc_save, esc_len, insert,
					      &num, &eol_num, buf, *len);
				esc_len = 0;
				continue;
			}
		}

		switch (ichar) {
		case 0x1b:
			if (esc_len == 0) {
				esc_save[esc_len] = ichar;
				esc_len = 1;
			} else {
				puts("impossible condition #876\n");
				esc_len = 0;
			}
			break;

		case CTL_CH('a'):
			BEGINNING_OF_LINE();
			break;
		case CTL_CH('c'):	/* ^C - break */
			*buf = '\0';	/* discard input */
			return (-1);
		case CTL_CH('f'):
			if (num < eol_num) {
				getcmd_putch(buf[num]);
				num++;
			}
			break;
		case CTL_CH('b'):
			if (num) {
				getcmd_putch(CTL_BACKSPACE);
				num--;
			}
			break;
		case CTL_CH('d'):
			if (num < eol_num) {
				wlen = eol_num - num - 1;
				if (wlen) {
					memmove(&buf[num], &buf[num+1], wlen);
					putnstr(buf + num, wlen);
				}

				getcmd_putch(' ');
				do {
					getcmd_putch(CTL_BACKSPACE);
				} while (wlen--);
				eol_num--;
			}
			break;
		case CTL_CH('k'):
			ERASE_TO_EOL();
			break;
		case CTL_CH('e'):
			REFRESH_TO_EOL();
			break;
		case CTL_CH('o'):
			insert = !insert;
			break;
		case CTL_CH('x'):
		case CTL_CH('u'):
			BEGINNING_OF_LINE();
			ERASE_TO_EOL();
			break;
		case DEL:
		case DEL7:
		case 8:
			if (num) {
				wlen = eol_num - num;
				num--;
				memmove(&buf[num], &buf[num+1], wlen);
				getcmd_putch(CTL_BACKSPACE);
				putnstr(buf + num, wlen);
				getcmd_putch(' ');
				do {
					getcmd_putch(CTL_BACKSPACE);
				} while (wlen--);
				eol_num--;
			}
			break;
		case CTL_CH('p'):
		case CTL_CH('n'):
		{
			char * hline;

			esc_len = 0;

			if (ichar == CTL_CH('p'))
				hline = hist_prev();
			else
				hline = hist_next();

			if (!hline) {
				getcmd_cbeep();
				continue;
			}

			/* nuke the current line */
			/* first, go home */
			BEGINNING_OF_LINE();

			/* erase to end of line */
			ERASE_TO_EOL();

			/* copy new line into place and display */
			strcpy(buf, hline);
			eol_num = strlen(buf);
			REFRESH_TO_EOL();
			continue;
		}
#ifdef CONFIG_AUTO_COMPLETE
		case '\t': {
			int num2, col;

			/* do not autocomplete when in the middle */
			if (num < eol_num) {
				getcmd_cbeep();
				break;
			}

			buf[num] = '\0';
			col = strlen(prompt) + eol_num;
			num2 = num;
			if (cmd_auto_complete(prompt, buf, &num2, &col)) {
				col = num2 - num;
				num += col;
				eol_num += col;
			}
			break;
		}
#endif
		default:
			cread_add_char(ichar, insert, &num, &eol_num, buf, *len);
			break;
		}
	}
	*len = eol_num;
	buf[eol_num] = '\0';	/* lose the newline */

	if (buf[0] && buf[0] != CREAD_HIST_CHAR)
		cread_add_to_hist(buf);
	hist_cur = hist_add_idx;

	return 0;
}

#endif /* CONFIG_CMDLINE_EDITING */

/****************************************************************************/

/*
 * Prompt for input and read a line.
 * If  CONFIG_BOOT_RETRY_TIME is defined and retry_time >= 0,
 * time out when time goes past endtime (timebase time in ticks).
 * Return:	number of read characters
 *		-1 if break
 *		-2 if timed out
 */
int readline (const char *const prompt)
{
	/*
	 * If console_buffer isn't 0-length the user will be prompted to modify
	 * it instead of entering it from scratch as desired.
	 */
	console_buffer[0] = '\0';

	return readline_into_buffer(prompt, console_buffer);
}


int readline_into_buffer (const char *const prompt, char * buffer)
{
	char *p = buffer;
#ifdef CONFIG_CMDLINE_EDITING
	unsigned int len=MAX_CMDBUF_SIZE;
	int rc;
	static int initted = 0;

	/*
	 * History uses a global array which is not
	 * writable until after relocation to RAM.
	 * Revert to non-history version if still
	 * running from flash.
	 */
	if (gd->flags & GD_FLG_RELOC) {
		if (!initted) {
			hist_init();
			initted = 1;
		}

		if (prompt)
			puts (prompt);

		rc = cread_line(prompt, p, &len);
		return rc < 0 ? rc : len;

	} else {
#endif	/* CONFIG_CMDLINE_EDITING */
	char * p_buf = p;
	int	n = 0;				/* buffer index		*/
	int	plen = 0;			/* prompt length	*/
	int	col;				/* output column cnt	*/
	char	c;

	/* print prompt */
	if (prompt) {
		plen = strlen (prompt);
		puts (prompt);
	}
	col = plen;

	for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
		while (!tstc()) {	/* while no incoming data */
			if (retry_time >= 0 && get_ticks() > endtime)
				return (-2);	/* timed out */
			WATCHDOG_RESET();
		}
#endif
		WATCHDOG_RESET();		/* Trigger watchdog, if needed */

#ifdef CONFIG_SHOW_ACTIVITY
		while (!tstc()) {
			extern void show_activity(int arg);
			show_activity(0);
			WATCHDOG_RESET();
		}
#endif
		c = getc();

		/*
		 * Special character handling
		 */
		switch (c) {
		case '\r':				/* Enter		*/
		case '\n':
			*p = '\0';
			puts ("\r\n");
			return (p - p_buf);

		case '\0':				/* nul			*/
			continue;

		case 0x03:				/* ^C - break		*/
			p_buf[0] = '\0';	/* discard input */
			return (-1);

		case 0x15:				/* ^U - erase line	*/
			while (col > plen) {
				puts (erase_seq);
				--col;
			}
			p = p_buf;
			n = 0;
			continue;

		case 0x17:				/* ^W - erase word	*/
			p=delete_char(p_buf, p, &col, &n, plen);
			while ((n > 0) && (*p != ' ')) {
				p=delete_char(p_buf, p, &col, &n, plen);
			}
			continue;

		case 0x08:				/* ^H  - backspace	*/
		case 0x7F:				/* DEL - backspace	*/
			p=delete_char(p_buf, p, &col, &n, plen);
			continue;

		default:
			/*
			 * Must be a normal character then
			 */
			if (n < CONFIG_SYS_CBSIZE-2) {
				if (c == '\t') {	/* expand TABs		*/
#ifdef CONFIG_AUTO_COMPLETE
					/* if auto completion triggered just continue */
					*p = '\0';
					if (cmd_auto_complete(prompt, console_buffer, &n, &col)) {
						p = p_buf + n;	/* reset */
						continue;
					}
#endif
					puts (tab_seq+(col&07));
					col += 8 - (col&07);
				} else {
					++col;		/* echo input		*/
					if (pass_mode == 0) {		// Arcadyan
						putc (c);	
					}
					else {
						putc ('*');
					}
				}
				*p++ = c;
				++n;
			} else {			/* Buffer full		*/
				putc ('\a');
			}
		}
	}
#ifdef CONFIG_CMDLINE_EDITING
	}
#endif
}

/****************************************************************************/

static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
	char *s;

	if (*np == 0) {
		return (p);
	}

	if (*(--p) == '\t') {			/* will retype the whole line	*/
		while (*colp > plen) {
			puts (erase_seq);
			(*colp)--;
		}
		for (s=buffer; s<p; ++s) {
			if (*s == '\t') {
				puts (tab_seq+((*colp) & 07));
				*colp += 8 - ((*colp) & 07);
			} else {
				++(*colp);
				putc (*s);
			}
		}
	} else {
		puts (erase_seq);
		(*colp)--;
	}
	(*np)--;
	return (p);
}

/****************************************************************************/

int parse_line (char *line, char *argv[])
{
	int nargs = 0;

#ifdef DEBUG_PARSER
	printf ("parse_line: \"%s\"\n", line);
#endif
	while (nargs < CONFIG_SYS_MAXARGS) {

		/* skip any white space */
		while ((*line == ' ') || (*line == '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		printf ("parse_line: nargs=%d\n", nargs);
#endif
			return (nargs);
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && (*line != ' ') && (*line != '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
#ifdef DEBUG_PARSER
		printf ("parse_line: nargs=%d\n", nargs);
#endif
			return (nargs);
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	printf ("** Too many args (max. %d) **\n", CONFIG_SYS_MAXARGS);

#ifdef DEBUG_PARSER
	printf ("parse_line: nargs=%d\n", nargs);
#endif
	return (nargs);
}

/****************************************************************************/

static void process_macros (const char *input, char *output)
{
	char c, prev;
	const char *varname_start = NULL;
	int inputcnt = strlen (input);
	int outputcnt = CONFIG_SYS_CBSIZE;
	int state = 0;		/* 0 = waiting for '$'  */

	/* 1 = waiting for '(' or '{' */
	/* 2 = waiting for ')' or '}' */
	/* 3 = waiting for '''  */
#ifdef DEBUG_PARSER
	char *output_start = output;

	printf ("[PROCESS_MACROS] INPUT len %d: \"%s\"\n", strlen (input),
		input);
#endif

	prev = '\0';		/* previous character   */

	while (inputcnt && outputcnt) {
		c = *input++;
		inputcnt--;

		if (state != 3) {
			/* remove one level of escape characters */
			if ((c == '\\') && (prev != '\\')) {
				if (inputcnt-- == 0)
					break;
				prev = c;
				c = *input++;
			}
		}

		switch (state) {
		case 0:	/* Waiting for (unescaped) $    */
			if ((c == '\'') && (prev != '\\')) {
				state = 3;
				break;
			}
			if ((c == '$') && (prev != '\\')) {
				state++;
			} else {
				*(output++) = c;
				outputcnt--;
			}
			break;
		case 1:	/* Waiting for (        */
			if (c == '(' || c == '{') {
				state++;
				varname_start = input;
			} else {
				state = 0;
				*(output++) = '$';
				outputcnt--;

				if (outputcnt) {
					*(output++) = c;
					outputcnt--;
				}
			}
			break;
		case 2:	/* Waiting for )        */
			if (c == ')' || c == '}') {
				int i;
				char envname[CONFIG_SYS_CBSIZE], *envval;
				int envcnt = input - varname_start - 1;	/* Varname # of chars */

				/* Get the varname */
				for (i = 0; i < envcnt; i++) {
					envname[i] = varname_start[i];
				}
				envname[i] = 0;

				/* Get its value */
				envval = getenv (envname);

				/* Copy into the line if it exists */
				if (envval != NULL)
					while ((*envval) && outputcnt) {
						*(output++) = *(envval++);
						outputcnt--;
					}
				/* Look for another '$' */
				state = 0;
			}
			break;
		case 3:	/* Waiting for '        */
			if ((c == '\'') && (prev != '\\')) {
				state = 0;
			} else {
				*(output++) = c;
				outputcnt--;
			}
			break;
		}
		prev = c;
	}

	if (outputcnt)
		*output = 0;
	else
		*(output - 1) = 0;

#ifdef DEBUG_PARSER
	printf ("[PROCESS_MACROS] OUTPUT len %d: \"%s\"\n",
		strlen (output_start), output_start);
#endif
}

/****************************************************************************
 * returns:
 *	1  - command executed, repeatable
 *	0  - command executed but not repeatable, interrupted commands are
 *	     always considered not repeatable
 *	-1 - not executed (unrecognized, bootd recursion or too many args)
 *           (If cmd is NULL or "" or longer than CONFIG_SYS_CBSIZE-1 it is
 *           considered unrecognized)
 *
 * WARNING:
 *
 * We must create a temporary copy of the command since the command we get
 * may be the result from getenv(), which returns a pointer directly to
 * the environment data, which may change magicly when the command we run
 * creates or modifies environment variables (like "bootp" does).
 */

int run_command (const char *cmd, int flag)
{
	cmd_tbl_t *cmdtp;
	char cmdbuf[CONFIG_SYS_CBSIZE];	/* working copy of cmd		*/
	char *token;			/* start of token in cmdbuf	*/
	char *sep;			/* end of token (separator) in cmdbuf */
	char finaltoken[CONFIG_SYS_CBSIZE];
	char *str = cmdbuf;
	char *argv[CONFIG_SYS_MAXARGS + 1];	/* NULL terminated	*/
	int argc, inquotes;
	int repeatable = 1;
	int rc = 0;

#ifdef DEBUG_PARSER
	printf ("[RUN_COMMAND] cmd[%p]=\"", cmd);
	puts (cmd ? cmd : "NULL");	/* use puts - string may be loooong */
	puts ("\"\n");
#endif

	clear_ctrlc();		/* forget any previous Control C */

	if (!cmd || !*cmd) {
		return -1;	/* empty command */
	}

	if (strlen(cmd) >= CONFIG_SYS_CBSIZE) {
		puts ("## Command too long!\n");
		return -1;
	}

	strcpy (cmdbuf, cmd);

	/* Process separators and check for invalid
	 * repeatable commands
	 */

#ifdef DEBUG_PARSER
	printf ("[PROCESS_SEPARATORS] %s\n", cmd);
#endif
	while (*str) {

		/*
		 * Find separator, or string end
		 * Allow simple escape of ';' by writing "\;"
		 */
		for (inquotes = 0, sep = str; *sep; sep++) {
			if ((*sep=='\'') &&
			    (*(sep-1) != '\\'))
				inquotes=!inquotes;

			if (!inquotes &&
			    (*sep == ';') &&	/* separator		*/
			    ( sep != str) &&	/* past string start	*/
			    (*(sep-1) != '\\'))	/* and NOT escaped	*/
				break;
		}

		/*
		 * Limit the token to data between separators
		 */
		token = str;
		if (*sep) {
			str = sep + 1;	/* start of command for next pass */
			*sep = '\0';
		}
		else
			str = sep;	/* no more commands for next pass */
#ifdef DEBUG_PARSER
		printf ("token: \"%s\"\n", token);
#endif

		/* find macros in this token and replace them */
		process_macros (token, finaltoken);

		/* Extract arguments */
		if ((argc = parse_line (finaltoken, argv)) == 0) {
			rc = -1;	/* no command at all */
			continue;
		}

		/* Look up command in command table */
		if ((cmdtp = find_cmd(argv[0])) == NULL) {
			printf ("Unknown command '%s' - try 'help'\n", argv[0]);
			rc = -1;	/* give up after bad command */
			continue;
		}

		/* found - check max args */
		if (argc > cmdtp->maxargs) {
			cmd_usage(cmdtp);
			rc = -1;
			continue;
		}

#if defined(CONFIG_CMD_BOOTD)
		/* avoid "bootd" recursion */
		if (cmdtp->cmd == do_bootd) {
#ifdef DEBUG_PARSER
			printf ("[%s]\n", finaltoken);
#endif
			if (flag & CMD_FLAG_BOOTD) {
				puts ("'bootd' recursion detected\n");
				rc = -1;
				continue;
			} else {
				flag |= CMD_FLAG_BOOTD;
			}
		}
#endif

		/* OK - call function to do the command */
		if ((cmdtp->cmd) (cmdtp, flag, argc, argv) != 0) {
			rc = -1;
		}

		repeatable &= cmdtp->repeatable;

		/* Did the user stop this? */
		if (had_ctrlc ())
			return -1;	/* if stopped then not repeatable */
	}

	return rc ? rc : repeatable;
}

/****************************************************************************/

#if defined(CONFIG_CMD_RUN)
int do_run (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int i;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	for (i=1; i<argc; ++i) {
		char *arg;

		if ((arg = getenv (argv[i])) == NULL) {
			printf ("## Error: \"%s\" not defined\n", argv[i]);
			return 1;
		}
#ifndef CONFIG_SYS_HUSH_PARSER
		if (run_command (arg, flag) == -1)
			return 1;
#else
		if (parse_string_outer(arg,
		    FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0)
			return 1;
#endif
	}
	return 0;
}
#endif



#ifdef CONFIG_DRIVER_VR9 // ctc, LCD

/*******************************************************/

//#include <asm/vr9.h>

#define	LCD_GPIO_RST		6 // 19
#define	LCD_GPIO_BL			28
#define	LCD_GPIO_LED		31

#define	LCD_CMD_MMAP_ADDR	0xB6000002
#define	LCD_DAT_MMAP_ADDR	0xB6000000
#define	LCD_CMD_MASK		0x0
#define	LCD_DAT_MASK		0x0
#define	LCD_VAL_SHIFT		8
#define	LCD_SET_CMD( val )	\
			do { \
				lcd_DelayNs( 20 ); \
				*(unsigned short*)LCD_CMD_MMAP_ADDR = ( 0x0 | LCD_CMD_MASK ); \
				lcd_DelayNs( 20 ); \
				*(unsigned short*)LCD_CMD_MMAP_ADDR = ( ( (unsigned short)(val) << LCD_VAL_SHIFT ) | LCD_CMD_MASK ); \
			} while ( 0 )
#define	LCD_SET_DAT( val )	\
			do { \
				lcd_DelayNs( 20 ); \
				*(unsigned short*)LCD_DAT_MMAP_ADDR = ( ( ((unsigned short)(val) >> 8) << LCD_VAL_SHIFT ) | LCD_DAT_MASK ); \
				lcd_DelayNs( 20 ); \
				*(unsigned short*)LCD_DAT_MMAP_ADDR = ( ( ((unsigned short)(val)&0xff) << LCD_VAL_SHIFT ) | LCD_DAT_MASK ); \
			} while ( 0 )
#define	LCD_GET_DAT( val )	\
			do { \
				unsigned short	__tmp__h, __tmp__l; \
				lcd_DelayNs( 20 ); \
				__tmp__h = *(unsigned short*)LCD_DAT_MMAP_ADDR; \
				lcd_DelayNs( 20 ); \
				__tmp__l = *(unsigned short*)LCD_DAT_MMAP_ADDR; \
				(val) = ( ( ( __tmp__h >> LCD_VAL_SHIFT ) & 0xff ) << 8 ) | ( ( __tmp__l >> LCD_VAL_SHIFT ) & 0xff ); \
			} while ( 0 )

#define	LCD_COL				240
#define	LCD_ROW				320

#define	LCD_PIXEL_NUM		(LCD_COL*LCD_ROW)
#define	LCD_GRAM_SIZE		(sizeof(color_t)*LCD_PIXEL_NUM)

#define	LCD_BL_MIN			0			// dark
#define	LCD_BL_MAX			16			// full
#define	LCD_BL_DFT			LCD_BL_MAX

#define	LCD_CONTRAST_MIN	0
#define	LCD_CONTRAST_MAX	31
#define	LCD_CONTRAST_DFT	12

#define	LCD_LED_MIN			0			// dark
#define	LCD_LED_MAX			16			// full
#define	LCD_LED_DFT			LCD_LED_MAX

#define	LCD_ORIENTATION_PORTRAIT	0
#define	LCD_ORIENTATION_LANDSCAPE	1
#define	LCD_ORIENTATION_DEFAULT		LCD_ORIENTATION_LANDSCAPE

#define	RGB16( r, g, b )		( ( ((color_t)(r) & 0x1F) << 11 ) | ( ((color_t)(g) & 0x3F) << 5 ) | ((color_t)(b) & 0x1F) )
#define	COLOR16_R( color16 )	( ((color16)>>11) & 0x1F )
#define	COLOR16_G( color16 )	( ((color16)>> 5) & 0x3F )
#define	COLOR16_B( color16 )	( ((color16)>> 0) & 0x1F )
#define	RGB16_TO_BGR16(color16)	RGB16( COLOR16_B( color16 ), COLOR16_G( color16 ), COLOR16_R( color16 ) )

#define	RGB24( r, g, b )		( ( ((unsigned long)(r) & 0xFF) << 16 ) | ( ((unsigned long)(g) & 0xFF) << 8 ) | ((unsigned long)(b) & 0xFF) )
#define	COLOR24_R( color24 )	( ((color24)>>24) & 0xFF )
#define	COLOR24_G( color24 )	( ((color24)>>16) & 0xFF )
#define	COLOR24_B( color24 )	( ((color24)>> 8) & 0xFF )

#define	RGB24_TO_16( r, g, b ) ( ( (((unsigned long)(r)>>3) & 0x1F) << 11 ) | ( (((unsigned long)(g)>>2) & 0x3F) << 5 ) | (((unsigned long)(b)>>3) & 0x1F) )

#define	COLOR16_BLACK			RGB24_TO_16( 0x00, 0x00, 0x00 )
#define	COLOR16_RED				RGB24_TO_16( 0xFF, 0x00, 0x00 )
#define	COLOR16_GREEN			RGB24_TO_16( 0x00, 0xFF, 0x00 )
#define	COLOR16_BLUE			RGB24_TO_16( 0x00, 0x00, 0xFF )
#define	COLOR16_YELLOW			RGB24_TO_16( 0xFF, 0xFF, 0x00 )
#define	COLOR16_MAGENTA			RGB24_TO_16( 0xFF, 0x00, 0xFF )
#define	COLOR16_CYAN			RGB24_TO_16( 0x00, 0xFF, 0xFF )
#define	COLOR16_WHITE			RGB24_TO_16( 0xFF, 0xFF, 0xFF )
#define	COLOR16_DEFAULT			COLOR16_BLACK

typedef unsigned short		coord_t;
typedef unsigned short		color_t;
typedef unsigned char		lcdOrientation_t;

typedef struct {
	coord_t			width;			// LCD width in pixels (in current orientation)
	coord_t			height;			// LCD height in pixels (in current orientation)
	unsigned char	orientation;	// Whether the LCD orientation can be modified
	unsigned char	backlight;		// backlight level, LCD_BL_MIN~LCD_BL_MAX
	unsigned char	contrast;		// contrast level, LCD_CONTRAST_MIN~LCD_CONTRAST_MAX
	unsigned char	led;			// LED level, LCD_LED_MIN~LCD_LED_MAX
} lcdProperties_t;

static coord_t			lcd_WinTl_x, lcd_WinTl_y, lcd_WinBr_x, lcd_WinBr_y, lcd_Cur_x, lcd_Cur_y;
static lcdProperties_t	lcd_Properties = { LCD_COL, LCD_ROW, LCD_ORIENTATION_DEFAULT, LCD_BL_DFT, LCD_CONTRAST_DFT, LCD_LED_DFT }; // width & height are in portrait orientation
static color_t*			lcd_Gram_Mirror = (color_t*)0x82100000;

int					lcd_IsInited( void );
void				lcd_FillSolid( color_t color );
coord_t				lcd_GetWidth(void);
coord_t				lcd_GetHeight(void);
unsigned char		lcd_GetContrast( void );
lcdOrientation_t	lcd_GetOrientation(void);


typedef struct {
	char			devName[32];
	int				(*probe)( void );
	unsigned short	(*getControllerID)( void );
	void			(*setCursor)( coord_t x, coord_t y );
	void			(*home)( void );
	void			(*setWindow)( coord_t x1, coord_t x2, coord_t y1, coord_t y2 );
	color_t			(*getPixel)( void );
	void			(*setOrientation)( lcdOrientation_t orientation );
	void			(*registerInit)( void );
	void			(*init)( void );
	void			(*shutdown)( void );
	int				bFixedDirection;
} stLcdChipFunc;


stLcdChipFunc* lcd_CurrChip = 0;


void lcd_DelayNs( unsigned long nsec )
{
	unsigned long	lCnt;
	unsigned long	lDummy;

	for ( lCnt=0, lDummy=0 ; lCnt < nsec ; lCnt++ )
	{
		lDummy = lDummy + 1;
	}

	lDummy--;
}

void lcd_DelayMs( unsigned long msec )
{
	ulong start = get_timer(0);

	while (get_timer(start) < msec)
	{
		if ( ctrlc() )
			return;
		udelay(100);
	}
}

void lcd_DelayUs( unsigned long usec )
{
	udelay( usec );
}

void lcd_GpioSet( int pin, int val )
{
	unsigned long*	pGpioOut;
	unsigned long	bit;

	if ( pin >=0 && pin <= 15 )
		pGpioOut = (unsigned long*)BSP_GPIO_P0_OUT;
	else if ( pin >=16 && pin <= 31 )
		pGpioOut = (unsigned long*)BSP_GPIO_P1_OUT;
	else if ( pin >=32 && pin <= 47 )
		pGpioOut = (unsigned long*)BSP_GPIO_P2_OUT;
	else if ( pin >=48 && pin <= 49 )
		pGpioOut = (unsigned long*)BSP_GPIO_P3_OUT;
	else
		return;

	bit = ( 0x1 << (pin%16) );

	if ( val )
		*pGpioOut |= bit;
	else
		*pGpioOut &= ~bit;
}

void lcd_WriteCommand( unsigned char iReg )
{
	LCD_SET_CMD( iReg );
}

void lcd_WriteData( unsigned short iData )
{
	LCD_SET_DAT( iData );
}

unsigned short lcd_ReadData( void )
{
	unsigned short	iData;

	LCD_GET_DAT( iData );

	return iData;
}

void lcd_WriteRegister( unsigned char iReg, unsigned short iData )
{
	lcd_WriteCommand( iReg );

	lcd_WriteData( iData );
}

void lcd_ReadRegister( unsigned char iReg, unsigned short* pData )
{
	if ( pData == 0 )
		return;

	lcd_WriteCommand( iReg );

	*pData = lcd_ReadData();
}


/**********************************************************************/


void ili9325_SetCursor( coord_t x, coord_t y )
{
	lcd_WriteRegister( 0x20, x );     // GRAM Horizontal Address Start (R20h)
	lcd_WriteRegister( 0x21, y );     // GRAM Vertical Address Start (R21h)
}

void ili9325_Home( void )
{
	lcd_WriteCommand( 0x22 );	// Write Data to GRAM (R22h)
}

void ili9325_SetWindow( coord_t x1, coord_t x2, coord_t y1, coord_t y2 )
{
	lcd_WriteRegister( 0x50, x1 );
	lcd_WriteRegister( 0x51, x2 );
	lcd_WriteRegister( 0x52, y1 );
	lcd_WriteRegister( 0x53, y2 );
}

color_t ili9325_GetPixel( void )
{
	color_t	iData;

	lcd_ReadRegister( 0x22, &iData );

	return iData;
}

void ili9325_SetOrientation( lcdOrientation_t orientation )
{
	unsigned short	entryMode;

	if ( orientation == LCD_ORIENTATION_LANDSCAPE)
		entryMode = 0x1028;
	else
		entryMode = 0x1030;

	lcd_WriteRegister( 0x03, entryMode );
}

unsigned short ili9325_GetControllerID( void )
{
	unsigned short	iData;

	lcd_ReadRegister( 0x0, &iData );

	return iData;
}

int ili9325_Probe( void )
{
	return ( ili9325_GetControllerID() == 0x9325 );
}

void ili9325_RegisterInit( void )
{
	lcd_WriteRegister( 0x01, 0x0100); // Driver Output Control Register (R01h)
	lcd_WriteRegister( 0x02, 0x0700);     // LCD Driving Waveform Control (R02h)
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE)
		lcd_WriteRegister( 0x03, 0x1028); // Entry Mode (R03h)
	else
		lcd_WriteRegister( 0x03, 0x1030); // Entry Mode (R03h)
	lcd_WriteRegister( 0x08, 0x0302);     // Display Control 2 (R08h)
	lcd_WriteRegister( 0x09, 0x0000);     // Display Control 3 (R09h)
	lcd_WriteRegister( 0x0A, 0x0000);     // Frame Marker Position (R0Ah)
	lcd_WriteRegister( 0x0C, 0x0001);     // RGB Display Interface Control 1 (R0Ch), 16-bit RGB interface
	lcd_WriteRegister( 0x10, 0x0000);     // Power Control 1 (R10h)
	lcd_WriteRegister( 0x11, 0x0007);     // Power Control 2 (R11h)
	lcd_WriteRegister( 0x12, 0x0000);     // Power Control 3 (R12h)
	lcd_WriteRegister( 0x13, 0x0000);     // Power Control 4 (R13h)
	lcd_DelayMs( 100 );
//	lcd_WriteRegister( 0x10, 0x14B0);     // Power Control 1 (R10h)  
	lcd_WriteRegister( 0x10, 0x1490);     // Power Control 1 (R10h)  
	lcd_DelayMs( 100 );
	lcd_WriteRegister( 0x11, 0x0007);     // Power Control 2 (R11h)  
	lcd_DelayMs( 100 );
	lcd_WriteRegister( 0x12, 0x008E);     // Power Control 3 (R12h)
	lcd_WriteRegister( 0x13, (unsigned short)lcd_GetContrast()<<8 ); // Power Control 4 (R13h)
	lcd_WriteRegister( 0x29, 0x0015);     // Power Control 7 (R29h)
	lcd_DelayMs( 100 );
	lcd_WriteRegister( 0x30, 0x0000);     // Gamma Control 1
	lcd_WriteRegister( 0x31, 0x0107);     // Gamma Control 2
	lcd_WriteRegister( 0x32, 0x0000);     // Gamma Control 3
	lcd_WriteRegister( 0x35, 0x0203);     // Gamma Control 6
	lcd_WriteRegister( 0x36, 0x0402);     // Gamma Control 7
	lcd_WriteRegister( 0x37, 0x0000);     // Gamma Control 8
	lcd_WriteRegister( 0x38, 0x0207);     // Gamma Control 9
	lcd_WriteRegister( 0x39, 0x0000);     // Gamma Control 10
	lcd_WriteRegister( 0x3C, 0x0203);     // Gamma Control 13
	lcd_WriteRegister( 0x3D, 0x0403);     // Gamma Control 14
	lcd_WriteRegister( 0x50, 0x0000);     // Window Horizontal RAM Address Start (R50h)
	lcd_WriteRegister( 0x51, LCD_COL - 1);// Window Horizontal RAM Address End (R51h)
	lcd_WriteRegister( 0x52, 0X0000);     // Window Vertical RAM Address Start (R52h)
	lcd_WriteRegister( 0x53, LCD_ROW - 1);// Window Vertical RAM Address End (R53h)
}

void ili9325_Init( void )
{
	ili9325_RegisterInit();

	// non-reinitialized registers
	lcd_WriteRegister( 0x2B, 0x000B);     // Frame Rate (R2Bh)
	lcd_WriteRegister( 0x60, 0xa700);     // Driver Output Control (R60h)
	lcd_WriteRegister( 0x61, 0x0003);     // Driver Output Control (R61h) - enable VLE
	lcd_WriteRegister( 0x90, 0X0010);     // Panel Interface Control 1 (R90h)

	// Display On
	lcd_WriteRegister( 0x07, 0x0133);     // Display Control (R07h)
	lcd_DelayMs( 100 );

	ili9325_Home();
}

void ili9325_Shutdown( void )
{
}


/**********************************************************************/


void hx8347_SetCursor( coord_t x, coord_t y )
{
//	lcd_WriteRegister( 0x02, x >> 8 );
//	lcd_WriteRegister( 0x03, x & 0xff );
//	lcd_WriteRegister( 0x06, y >> 8 );
//	lcd_WriteRegister( 0x07, y & 0xff );
}

void hx8347_Home( void )
{
	lcd_WriteCommand( 0x22 );	// Write Data to GRAM (R22h)
}

void hx8347_SetWindow( coord_t x1, coord_t x2, coord_t y1, coord_t y2 )
{
	lcd_WriteRegister( 0x02, x1 >> 8 );
	lcd_WriteRegister( 0x03, x1 & 0xff );
	lcd_WriteRegister( 0x04, x2 >> 8 );
	lcd_WriteRegister( 0x05, x2 & 0xff );
	lcd_WriteRegister( 0x06, y1 >> 8 );
	lcd_WriteRegister( 0x07, y1 & 0xff );
	lcd_WriteRegister( 0x08, y2 >> 8 );
	lcd_WriteRegister( 0x09, y2 & 0xff );
}

color_t hx8347_GetPixel( void )
{
	return 0x0000;
}

void hx8347_SetOrientation( lcdOrientation_t orientation )
{
	unsigned short	entryMode;

	if ( orientation == LCD_ORIENTATION_LANDSCAPE)
		entryMode = 0x0000;
	else
		entryMode = 0x0060;

	lcd_WriteRegister( 0x16, entryMode );
}

unsigned short hx8347_GetControllerID( void )
{
	unsigned short	iData;

	lcd_ReadRegister( 0x0, &iData );

	return iData & 0x00ff;
}

int hx8347_Probe( void )
{
	return ( hx8347_GetControllerID() == 0x75 );
}

void hx8347_RegisterInit( void )
{
	//Driving ability Setting
	lcd_WriteRegister( 0x2E, 0X0089 ); //GDOFF
	lcd_WriteRegister( 0x29, 0X008F ); //RTN
	lcd_WriteRegister( 0x2B, 0x0002 ); //DUM
	lcd_WriteRegister( 0xE2, 0x0000 ); //VREF
	lcd_WriteRegister( 0xE4, 0x0001 ); //EQ
	lcd_WriteRegister( 0xE5, 0x0010 ); //EQ
	lcd_WriteRegister( 0xE6, 0x0001 ); //EQ
	lcd_WriteRegister( 0xE7, 0x0010 ); //EQ
	lcd_WriteRegister( 0xE8, 0x0070 ); //OPON
	lcd_WriteRegister( 0xF2, 0x0000 ); //GEN
	lcd_WriteRegister( 0xEA, 0x0000 ); //PTBA
	lcd_WriteRegister( 0xEB, 0x0020 ); //PTBA
	lcd_WriteRegister( 0xEC, 0x003C ); //STBA
	lcd_WriteRegister( 0xED, 0x00C8 ); //STBA
	lcd_WriteRegister( 0xE9, 0x0038 ); //OPON1
	lcd_WriteRegister( 0xF1, 0x0001 ); //OTPS1B

	//Gamma 2.2 Setting
	lcd_WriteRegister( 0x40, 0x0000 ); //
	lcd_WriteRegister( 0x41, 0x0000 ); //
	lcd_WriteRegister( 0x42, 0x0000 ); //
	lcd_WriteRegister( 0x43, 0x0015 ); //
	lcd_WriteRegister( 0x44, 0x0013 ); //
	lcd_WriteRegister( 0x45, 0x003f ); //
	lcd_WriteRegister( 0x46, 0x0001 ); //
	lcd_WriteRegister( 0x47, 0x0055 ); //
	lcd_WriteRegister( 0x48, 0x0000 ); //
	lcd_WriteRegister( 0x49, 0x0012 ); //
	lcd_WriteRegister( 0x4A, 0x0019 ); //
	lcd_WriteRegister( 0x4B, 0x0019 ); //
	lcd_WriteRegister( 0x4C, 0x0016 ); //
	lcd_WriteRegister( 0x50, 0x0000 ); //
	lcd_WriteRegister( 0x51, 0x002c ); //
	lcd_WriteRegister( 0x52, 0x002a ); //
	lcd_WriteRegister( 0x53, 0x003F ); //
	lcd_WriteRegister( 0x54, 0x003F ); //
	lcd_WriteRegister( 0x55, 0x003F ); //
	lcd_WriteRegister( 0x56, 0x002a ); //
	lcd_WriteRegister( 0x57, 0x007e ); //
	lcd_WriteRegister( 0x58, 0x0009 ); //
	lcd_WriteRegister( 0x59, 0x0006 ); //
	lcd_WriteRegister( 0x5A, 0x0006 ); //
	lcd_WriteRegister( 0x5B, 0x000d ); //
	lcd_WriteRegister( 0x5C, 0x001F ); //
	lcd_WriteRegister( 0x5D, 0x00FF ); //

	//Power Voltage Setting
	lcd_WriteRegister( 0x1B, 0x001B );
	lcd_WriteRegister( 0x1A, 0x0002 );
	lcd_WriteRegister( 0x24, 0x0065 );//61
	lcd_WriteRegister( 0x25, 0x005C );

	//VCOM offset///
	lcd_WriteRegister( 0x23, 0x0062 );//60

	//Power on Setting
//	lcd_WriteRegister( 0x18, 0x0036 ); //RADJ 70Hz  36
	lcd_WriteRegister( 0x19, 0x0001 ); //OSC_EN=1
	lcd_WriteRegister( 0x1F, 0x0088 ); // GAS=1, VOMG=00
	lcd_DelayMs( 5 );
	lcd_WriteRegister( 0x1F, 0x0080 ); // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
	lcd_DelayMs( 5 );
	lcd_WriteRegister( 0x1F, 0x0090 ); // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
	lcd_DelayMs( 5 );
	lcd_WriteRegister( 0x1F, 0x00D4 ); // GAS=1, VOMG=10, PON=1, DK=0, XDK=1, DDVDH_TRI=0, STB=0
	lcd_DelayMs( 5 );

	//262k/65k color selection
	lcd_WriteRegister( 0x17, 0x0055 ); //default 0x66 262k ,0x55 65k color

	//SET PANEL
	lcd_WriteRegister( 0x36, 0x0009 ); //SS_P, 

	//Display ON Setting
//	lcd_WriteRegister( 0x28, 0x0038 ); //GON=1, DTE=1, 
//	lcd_DelayMs( 40 );
//	lcd_WriteRegister( 0x28, 0x003C ); //GON=1, DTE=1, D=1100

	//Set Orientation
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE)
		lcd_WriteRegister( 0x16, 0x0000);
	else
		lcd_WriteRegister( 0x16, 0x0060);

	//Set Contrast
//	lcd_WriteRegister( 0x13, (unsigned short)lcd_GetContrast()<<8 ); // Power Control 4 (R13h)

	lcd_WriteRegister( 0x02, 0x0000 );
	lcd_WriteRegister( 0x03, 0x0000 ); //Column Start
	lcd_WriteRegister( 0x04, (LCD_COL-1)>>8 );
	lcd_WriteRegister( 0x05, (LCD_COL-1)%0xff ); //Column End
	lcd_WriteRegister( 0x06, 0x0000 );
	lcd_WriteRegister( 0x07, 0x0000 ); //Row Start
	lcd_WriteRegister( 0x08, (LCD_ROW-1)>>8 );
	lcd_WriteRegister( 0x09, (LCD_ROW-1)%0xff ); //Row End
}

void hx8347_Init( void )
{
	hx8347_RegisterInit();

	// non-reinitialized registers
	lcd_WriteRegister( 0x18, 0x0036 ); // RADJ 70Hz  36
	lcd_WriteRegister( 0x60, 0xa700 ); // Driver Output Control (R60h)
	lcd_WriteRegister( 0x61, 0x0003 ); // Driver Output Control (R61h) - enable VLE
	lcd_WriteRegister( 0x90, 0X0010 ); // Panel Interface Control 1 (R90h)

	// Display On
	lcd_WriteRegister( 0x28, 0x0038 ); //GON=1, DTE=1, 
	lcd_DelayMs( 40 );
	lcd_WriteRegister( 0x28, 0x003C ); //GON=1, DTE=1, D=1100

	hx8347_Home();
}

void hx8347_Shutdown( void )
{
}

/**********************************************************************/


void ili9341_SetCursor( coord_t x, coord_t y )
{
}

void ili9341_Home( void )
{
	lcd_WriteCommand( 0x2C );	// Write Data to GRAM (R2Ch)
}

void ili9341_SetWindow( coord_t x1, coord_t x2, coord_t y1, coord_t y2 )
{
	lcd_WriteCommand( 0x2A);
	lcd_WriteData( x1 );
	lcd_WriteData( x2 );				
	lcd_WriteCommand( 0x2B);
	lcd_WriteData( y1 );
	lcd_WriteData( y2 );			
}

color_t ili9341_GetPixel( void )
{
	color_t	iData;

	lcd_ReadRegister( 0x2E, &iData );

	return iData;
}

void ili9341_SetOrientation( lcdOrientation_t orientation )
{
	unsigned short	entryMode;
	
	if ( orientation == LCD_ORIENTATION_LANDSCAPE)
		entryMode = 0x4800;
	else
		entryMode = 0x3800;
	lcd_WriteRegister( 0x36, entryMode );
	
	lcd_WriteData( entryMode );				
}

unsigned short ili9341_GetControllerID( void )
{
	unsigned short	iParameter1;
	unsigned short	iParameter2;	
	
	lcd_WriteCommand( 0xD3);

	iParameter1 = lcd_ReadData();
	iParameter2 = lcd_ReadData();

	return iParameter2;
}

int ili9341_Probe( void )
{
	return ( ili9341_GetControllerID() == 0x9341 );
}

void ili9341_RegisterInit( void )
{

// VCI=2.8V 

//************* Start Initial Sequence **********// 
 
	lcd_WriteCommand(0xCB);  
	lcd_WriteData(0x392C); 
	lcd_WriteData(0x0034); 
	lcd_WriteData(0x0200); 
 
	lcd_WriteCommand(0xCF);  
	lcd_WriteData(0x00C1); 
	lcd_WriteData(0X3000); 
 
	lcd_WriteCommand(0xE8);  
	lcd_WriteData(0x8510);  
	lcd_WriteData(0x7900); 


	lcd_WriteCommand(0xEA);  
	lcd_WriteData(0x0000);  

	lcd_WriteCommand(0xED);  
	lcd_WriteData(0x6403);  
	lcd_WriteData(0X1281); 
 
	lcd_WriteCommand(0xF7);  
	lcd_WriteData(0x2000); 
 
	lcd_WriteCommand(0xC0);    //Power control 
	lcd_WriteData(0x2100);   //VRH[5:0] 
 
	lcd_WriteCommand(0xC1);    //Power control 
	lcd_WriteData(0x1200);   //SAP[2:0];BT[3:0] 
 
	lcd_WriteCommand(0xC5);    //VCM control 
	lcd_WriteData(0x243F);  
 
	lcd_WriteCommand(0xC7);    //VCM control2 
	lcd_WriteData(0xC200); 

	lcd_WriteCommand(0xb1); 
	lcd_WriteData(0x0016);  
 
	lcd_WriteCommand(0x36);    // Memory Access Control 
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE)
	  lcd_WriteData(0x4800);//08 48	
	else	
	  lcd_WriteData(0x3800);

	lcd_WriteCommand(0x3A);    
	lcd_WriteData(0x5500);  
 
	lcd_WriteCommand(0xF2);    // 3Gamma Function Disable 
	lcd_WriteData(0x0000); 
 
	lcd_WriteCommand(0x26);    //Gamma curve selected 
	lcd_WriteData(0x0100); 
 
	lcd_WriteCommand(0xE0);    //Set Gamma 
	lcd_WriteData(0x0F1B);  
	lcd_WriteData(0x170C);  
	lcd_WriteData(0x0D08);  
	lcd_WriteData(0x40A9);  
	lcd_WriteData(0x2806);  
	lcd_WriteData(0x0D03);  
	lcd_WriteData(0x1003);  
	lcd_WriteData(0x0000); 
 
	lcd_WriteCommand(0XE1);    //Set Gamma 
	lcd_WriteData(0x0024); 
	lcd_WriteData(0x2803);  
	lcd_WriteData(0x1207);  
	lcd_WriteData(0x3F56);  
	lcd_WriteData(0x5709); 
	lcd_WriteData(0x120C);  
	lcd_WriteData(0x2F3C);  
	lcd_WriteData(0x0F00); 
 
	lcd_WriteCommand(0x11);    //Exit Sleep 
	lcd_DelayMs(120); 
	lcd_WriteCommand(0x29);    //Display on
}

void ili9341_Init( void )
{
	ili9341_RegisterInit();
	ili9341_Home();	
	
}

void ili9341_Shutdown( void )
{
}


/**********************************************************************/


// NEVER user static function table assignment in boot loader, or function pointer will be wrong
stLcdChipFunc lcd_ChipTbl[3];

void lcd_FuncTblPrepare( void )
{
	strcpy( lcd_ChipTbl[0].devName, "ILI9325D" );
	lcd_ChipTbl[0].probe			= ili9325_Probe;
	lcd_ChipTbl[0].getControllerID	= ili9325_GetControllerID;
	lcd_ChipTbl[0].setCursor		= ili9325_SetCursor;
	lcd_ChipTbl[0].home				= ili9325_Home;
	lcd_ChipTbl[0].setWindow		= ili9325_SetWindow;
	lcd_ChipTbl[0].getPixel			= ili9325_GetPixel;
	lcd_ChipTbl[0].setOrientation	= ili9325_SetOrientation;
	lcd_ChipTbl[0].registerInit		= ili9325_RegisterInit;
	lcd_ChipTbl[0].init				= ili9325_Init;
	lcd_ChipTbl[0].shutdown			= ili9325_Shutdown;
	lcd_ChipTbl[0].bFixedDirection	= 0;

	strcpy( lcd_ChipTbl[1].devName, "HX8347-G" );
	lcd_ChipTbl[1].probe			= hx8347_Probe;
	lcd_ChipTbl[1].getControllerID	= hx8347_GetControllerID;
	lcd_ChipTbl[1].setCursor		= hx8347_SetCursor;
	lcd_ChipTbl[1].home				= hx8347_Home;
	lcd_ChipTbl[1].setWindow		= hx8347_SetWindow;
	lcd_ChipTbl[1].getPixel			= hx8347_GetPixel;
	lcd_ChipTbl[1].setOrientation	= hx8347_SetOrientation;
	lcd_ChipTbl[1].registerInit		= hx8347_RegisterInit;
	lcd_ChipTbl[1].init				= hx8347_Init;
	lcd_ChipTbl[1].shutdown			= hx8347_Shutdown;
	lcd_ChipTbl[1].bFixedDirection	= 1;

	strcpy( lcd_ChipTbl[2].devName, "ILI9341" );
	lcd_ChipTbl[2].probe			= ili9341_Probe;
	lcd_ChipTbl[2].getControllerID	= ili9341_GetControllerID;
	lcd_ChipTbl[2].setCursor		= ili9341_SetCursor;
	lcd_ChipTbl[2].home				= ili9341_Home;
	lcd_ChipTbl[2].setWindow		= ili9341_SetWindow;
	lcd_ChipTbl[2].getPixel			= ili9341_GetPixel;
	lcd_ChipTbl[2].setOrientation	= ili9341_SetOrientation;
	lcd_ChipTbl[2].registerInit		= ili9341_RegisterInit;
	lcd_ChipTbl[2].init				= ili9341_Init;
	lcd_ChipTbl[2].shutdown			= ili9341_Shutdown;
	lcd_ChipTbl[2].bFixedDirection	= 1;	

}

/**********************************************************************/


coord_t lcd_GetWidth(void)
{
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE ) 
		return lcd_Properties.height;
	else // LCD_ORIENTATION_PORTRAIT
		return lcd_Properties.width;
}

coord_t lcd_GetHeight(void)
{
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE ) 
		return lcd_Properties.width;
	else // LCD_ORIENTATION_PORTRAIT
		return lcd_Properties.height;
}

void lcd_SetCursor( coord_t x, coord_t y )
{
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE)
	{
		lcd_Cur_x = lcd_GetHeight() - y;
		lcd_Cur_y = x;
	}
	else
	{
		lcd_Cur_x = x;
		lcd_Cur_y = y;
	}

	if ( lcd_CurrChip && lcd_CurrChip->setCursor )
		lcd_CurrChip->setCursor( lcd_Cur_x, lcd_Cur_y );
}

void lcd_Home( void )
{
	lcd_SetCursor( 0, 0 );

	if ( lcd_CurrChip && lcd_CurrChip->home )
		lcd_CurrChip->home();
}

void lcd_SetWindow( coord_t x, coord_t y, coord_t width, coord_t height )
{
	if ( x >= lcd_GetWidth() )
		x = lcd_GetWidth() - 1;
	if ( (x+width) > lcd_GetWidth() )
		width = lcd_GetWidth() - x;
	if ( y >= lcd_GetHeight() )
		y = lcd_GetHeight() - 1;
	if ( (y+height) > lcd_GetHeight() )
		height = lcd_GetHeight() - y;
	if ( lcd_GetOrientation() == LCD_ORIENTATION_LANDSCAPE )
	{
//		lcd_WinTl_x = lcd_GetHeight() - y - height + 1;
		lcd_WinTl_x = lcd_GetHeight() - y - height;
		lcd_WinTl_y = x;
		lcd_WinBr_x = lcd_GetHeight() - y - 1;
		lcd_WinBr_y = x + width - 1;
	}
	else
	{
		lcd_WinTl_x = x;
		lcd_WinTl_y = y;
		lcd_WinBr_x = x + width - 1;
		lcd_WinBr_y = y + height - 1;
	}

	if ( lcd_CurrChip && lcd_CurrChip->setWindow )
		lcd_CurrChip->setWindow( lcd_WinTl_x, lcd_WinBr_x, lcd_WinTl_y, lcd_WinBr_y );

	lcd_SetCursor( x, y );
}

lcdOrientation_t lcd_GetOrientation(void)
{
  return lcd_Properties.orientation;
}

unsigned short lcd_GetControllerID( void )
{
	if ( lcd_CurrChip && lcd_CurrChip->getControllerID )
		return lcd_CurrChip->getControllerID();

	return 0;
}

void lcd_SetMirrorGram( coord_t x, coord_t y, color_t color )
{
	if ( x >= lcd_GetWidth() )
		return;

	if ( y >= lcd_GetHeight() )
		return;

	lcd_Gram_Mirror[ y*lcd_GetWidth() + x ] = color;
}

void lcd_UpdateGram( coord_t x1, coord_t y1, coord_t x2, coord_t y2 )
{
	long	row, col, offset;

	if ( x1 >= lcd_GetWidth() )
		return;

	if ( x2 >= lcd_GetWidth() )
		x2 = lcd_GetWidth() - 1;

	if ( y1 >= lcd_GetHeight() )
		return;

	if ( y2 >= lcd_GetHeight() )
		y2 = lcd_GetHeight() - 1;

	if (x1 > x2)
	{
		col = x1;
		x1  = x2;
		x2  = col;
	}

	if (y1 > y2)
	{
		row = y1;
		y1  = y2;
		y2  = row;
	}

	// reset window address
	lcd_SetWindow( x1, y1, x2-x1+1, y2-y1+1 );

	if ( lcd_CurrChip && lcd_CurrChip->home )
		lcd_CurrChip->home();

	if ( lcd_CurrChip == 0 || lcd_CurrChip->bFixedDirection == 0 || lcd_GetOrientation() != LCD_ORIENTATION_LANDSCAPE )
	{
		for ( row=y1; row <= y2; row++ )
		{
			offset = row * lcd_GetWidth();
			for ( col=x1; col <= x2; col++ )
			{
				lcd_WriteData( lcd_Gram_Mirror[ offset + col ] );
			}
		}
	}
	else
	{
		offset = lcd_GetWidth();
		for ( col=x1; col <= x2; col++ )
		{
			for ( row=y2; row >= y1; row-- )
			{
				lcd_WriteData( lcd_Gram_Mirror[ offset*row + col ] );
			}
		}
	}
}

/******************************************************************/

void lcd_Flush( void )
{
	lcd_UpdateGram( 0, 0, lcd_GetWidth()-1, lcd_GetHeight()-1 );
}

void lcd_FillSolidRect( coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color )
{
	unsigned long	row, col;

	if ( x1 >= lcd_GetWidth() )
		x1 = lcd_GetWidth() - 1;
	if ( x2 >= lcd_GetWidth() )
		x2 = lcd_GetWidth() - 1;

	if ( y1 >= lcd_GetHeight() )
		y1 = lcd_GetHeight() - 1;
	if ( y2 >= lcd_GetHeight() )
		y2 = lcd_GetHeight() - 1;

	if (x1 > x2)
	{
		col = x1;
		x1  = x2;
		x2  = col;
	}

	if (y1 > y2)
	{
		row = y1;
		y1  = y2;
		y2  = row;
	}

	for ( row=y1; row <= y2; row++ )
	{
        for ( col=x1; col <= x2; col++ )
		{
			lcd_SetMirrorGram( col, row, color );
		}
	}

	lcd_UpdateGram( x1, y1, x2, y2 );
}

void lcd_FillSolid( color_t color )
{
	lcd_FillSolidRect( 0, 0, lcd_GetWidth()-1, lcd_GetHeight()-1, color );
}

void lcd_FillGradientRect( coord_t x1, coord_t y1, coord_t x2, coord_t y2, color_t color1, color_t color2 )
{
	unsigned long	row, col;
	unsigned long	row_max_no, col_max_no;
	color_t			color1R = COLOR16_R(color1);
	color_t			color1G = COLOR16_G(color1);
	color_t			color1B = COLOR16_B(color1);
	color_t			color2R = COLOR16_R(color2);
	color_t			color2G = COLOR16_G(color2);
	color_t			color2B = COLOR16_B(color2);
	color_t			colorR;
	color_t			colorG;
	color_t			colorB;

	if ( x1 >= lcd_GetWidth() )
		x1 = lcd_GetWidth() - 1;
	if ( x2 >= lcd_GetWidth() )
		x2 = lcd_GetWidth() - 1;

	if ( y1 >= lcd_GetHeight() )
		y1 = lcd_GetHeight() - 1;
	if ( y2 >= lcd_GetHeight() )
		y2 = lcd_GetHeight() - 1;

	if (x1 > x2)
	{
		col = x1;
		x1  = x2;
		x2  = col;
	}

	if (y1 > y2)
	{
		row = y1;
		y1  = y2;
		y2  = row;
	}

	row_max_no = y2 - y1 + 1;
	col_max_no = x2 - x1 + 1;

	for ( row=y1; row <= y2; row++ )
	{
        for ( col=x1; col <= x2; col++ )
		{
			colorR =  ((y2-row)*color1R + (row-y1)*color2R) / row_max_no
					+ ((x2-col)*color1R + (col-x1)*color2R) / col_max_no;
			colorG =  ((y2-row)*color1G + (row-y1)*color2G) / row_max_no
					+ ((x2-col)*color1G + (col-x1)*color2G) / col_max_no;
			colorB =  ((y2-row)*color1B + (row-y1)*color2B) / row_max_no
					+ ((x2-col)*color1B + (col-x1)*color2B) / col_max_no;
			lcd_SetMirrorGram( col, row, RGB16( colorR/2, colorG/2, colorB/2 ) );
		}
	}

	lcd_UpdateGram( x1, y1, x2, y2 );
}

void lcd_FillGradient( color_t color1, color_t color2 )
{
	lcd_FillGradientRect( 0, 0, lcd_GetWidth()-1, lcd_GetHeight()-1, color1, color2 );
}

unsigned char lcd_GetBackLight( void )
{
	return lcd_Properties.backlight;
}

void lcd_SetBackLight( unsigned char iData, int bForce )
{
	int		iCnt;
	int		iDec;

	if (iData < LCD_BL_MIN)
		iData = LCD_BL_MIN;

	if ( iData > LCD_BL_MAX )
		iData = LCD_BL_MAX;

	if ( bForce == 0 && lcd_Properties.backlight == iData )
		return;

	lcd_Properties.backlight = iData;

	lcd_GpioSet( LCD_GPIO_BL, 0 );
	lcd_DelayMs( 5 );

	if ( iData == 0 )
		return;

	lcd_GpioSet( LCD_GPIO_BL, 1 );
	lcd_DelayUs( 50 );

	for ( iCnt=0, iDec=LCD_BL_MAX-lcd_Properties.backlight; iCnt < iDec; iCnt++ )
	{
		lcd_GpioSet( LCD_GPIO_BL, 0 );
		lcd_DelayUs( 1 );
		lcd_GpioSet( LCD_GPIO_BL, 1 );
		lcd_DelayUs( 1 );
	}
}

unsigned char lcd_GetLed( void )
{
	return lcd_Properties.led;
}

void lcd_SetLed( unsigned char iData, int bForce )
{
	int		iCnt;
	int		iDec;

	if (iData < LCD_LED_MIN)
		iData = LCD_LED_MIN;

	if ( iData > LCD_LED_MAX )
		iData = LCD_LED_MAX;

	if ( bForce == 0 && lcd_Properties.led == iData )
		return;

	lcd_Properties.led = iData;

	lcd_GpioSet( LCD_GPIO_LED, 0 );
	lcd_DelayMs( 5 );

	if ( iData == 0 )
		return;

	lcd_GpioSet( LCD_GPIO_LED, 1 );
	lcd_DelayUs( 50 );

	for ( iCnt=0, iDec=LCD_LED_MAX-lcd_Properties.led; iCnt < iDec; iCnt++ )
	{
		lcd_GpioSet( LCD_GPIO_LED, 0 );
		lcd_DelayUs( 1 );
		lcd_GpioSet( LCD_GPIO_LED, 1 );
		lcd_DelayUs( 1 );
	}
}

unsigned char lcd_GetContrast( void )
{
	return lcd_Properties.contrast;
}

void lcd_GpioInit( void )
{
	// reset, GPIO 6
	*BSP_GPIO_P0_ALTSEL0 &= ~(0x0040);
	*BSP_GPIO_P0_ALTSEL1 &= ~(0x0040);
	*BSP_GPIO_P0_DIR     |= 0x0040;
	*BSP_GPIO_P0_OD      |= 0x0040;
	lcd_GpioSet( LCD_GPIO_RST, 1 );
	// backlight, GPIO 28
	lcd_GpioSet( LCD_GPIO_BL, 0 );
	*BSP_GPIO_P1_ALTSEL0 &= ~(0x1000);
	*BSP_GPIO_P1_ALTSEL1 &= ~(0x1000);
	*BSP_GPIO_P1_DIR     |= 0x1000;
	*BSP_GPIO_P1_OD      |= 0x1000;
//	lcd_GpioSet( LCD_GPIO_BL, 1 );
	// LED, GPIO 31
	*BSP_GPIO_P1_ALTSEL0 &= ~(0x8000);
	*BSP_GPIO_P1_ALTSEL1 &= ~(0x8000);
	*BSP_GPIO_P1_DIR     |= 0x8000;
	*BSP_GPIO_P1_OD      |= 0x8000;
	lcd_GpioSet( LCD_GPIO_LED, 1 );

	// reset, GPIO 6
	lcd_DelayMs( 1 );
	lcd_GpioSet( LCD_GPIO_RST, 0 );
	lcd_DelayMs( 2 );
	lcd_GpioSet( LCD_GPIO_RST, 1 );
	// backlight, GPIO 28
//	lcd_GpioSet( LCD_GPIO_BL, 0 );
//	lcd_DelayMs( 20 );
//	lcd_GpioSet( LCD_GPIO_BL, 1 );
	// LED, GPIO 31
//	lcd_GpioSet( LCD_GPIO_LED, 1 );
	//
	lcd_DelayMs( 100 );
}

void lcd_Init( void )
{
	int		iCnt;

	*(unsigned long*)0xbe105328 = 0x160000f1;
	*(unsigned long*)0xbe105368 = 0x1d3dd;

	lcd_FuncTblPrepare();

	lcd_GpioInit();

	lcd_CurrChip = 0;

	for ( iCnt=0 ; iCnt < sizeof(lcd_ChipTbl)/sizeof(lcd_ChipTbl[0]) ; iCnt++ )
	{
		if ( lcd_ChipTbl[iCnt].probe!=0 && lcd_ChipTbl[iCnt].probe() )
		{
			lcd_CurrChip = &lcd_ChipTbl[iCnt];
			break;
		}
	}

	if ( lcd_CurrChip && lcd_CurrChip->init )
		lcd_CurrChip->init();

	lcd_FillSolid( COLOR16_DEFAULT );
	lcd_SetBackLight( lcd_GetBackLight(), LCD_BL_DFT );
}

int lcd_DisplayPage( int page )
{
	char*                   pBootLcd;
	unsigned char*  pLzma = (unsigned char*)0x82000000;
	unsigned char*  pImages = (unsigned char*)0x82400000;
	char                    sCmd[64];
	SizeT                   lLzmaSize;
	SizeT                   lAllImageSize;
	unsigned long   lFileCnt;
	char*                   pFiles;
	unsigned long   lCnt;
	unsigned char*  pImagePtr;
	ulong                   start;
	ulong                   interval;
	unsigned long   lPixel;
	int                             bFirstImage = 1;

	static int              init = 0;

	/* MUST set env var "bootlcd", or LCD is disabled at bootup */

	pBootLcd = getenv( "bootlcd" );

	if ( pBootLcd==0 || strcmp( pBootLcd, "0" ) == 0 )
		return 0;

	if (init == 0) {

		printf ( "Uncompressing LCD bootup images ............... " );

		sprintf( sCmd, "nand read.e %p $(f_lcdimage_addr) $(f_lcdimage_size)", pLzma );
		run_command( sCmd, 0 );

		if ( strcmp( (char*)pLzma, "lcd_boot_image" ) != 0 ) // wrong image ID
		{
			printf( "wrong image ID\n" );
			goto err_Boot_Lcd_Init;
		}

		lLzmaSize = (SizeT)simple_strtoul( (char*)pLzma+16, 0, 10 );

		if ( lLzmaSize >= 0x400000 )
		{
			printf( "wrong LZMA image size\n" );
			goto err_Boot_Lcd_Init;
		}

		if ( lzmaBuffToBuffDecompress( pImages, &lAllImageSize, pLzma+160, lLzmaSize ) != SZ_OK)
		{
			printf( "wrong LZMA image format\n" );
			goto err_Boot_Lcd_Init;
		}

		if ( lAllImageSize != (SizeT)simple_strtoul( (char*)pLzma+32, 0, 10 ) )
		{
			printf( "wrong uncompressed image size\n" );
			goto err_Boot_Lcd_Init;
		}

		lFileCnt = (SizeT)simple_strtoul( (char*)pLzma+28, 0, 10 );
		interval = (ulong)simple_strtoul( (char*)pLzma+48, 0, 10 );
		pFiles = (char*)(pLzma + 52);

		lcd_Init();

		if ( lcd_CurrChip == 0 )
			return 0;

		init = 1;
	}

       // move to the beginning of the page
	pImagePtr = pImages + (LCD_PIXEL_NUM*3)*page;

	for ( lPixel=0; lPixel < LCD_PIXEL_NUM; lPixel++, pImagePtr+=3 )
		lcd_Gram_Mirror[lPixel] = RGB24_TO_16( pImagePtr[0], pImagePtr[1], pImagePtr[2] );

	lcd_Flush();
	//lcd_SetBackLight( lcd_GetBackLight(), 16 );
	lcd_SetBackLight( LCD_BL_MAX, 0);

	return 0;


err_Boot_Lcd_Init:

	lcd_FillSolid( COLOR16_MAGENTA );
	lcd_SetBackLight( lcd_GetBackLight(), 1 );

	return 0;
}

int lcd_boot_Init( void )
{
	char*			pBootLcd;
	unsigned char*	pLzma = (unsigned char*)0x82000000;
	unsigned char*	pImages = (unsigned char*)0x82400000;
	char			sCmd[64];
	SizeT			lLzmaSize;
	SizeT			lAllImageSize;
	unsigned long	lFileCnt;
	char*			pFiles;
	unsigned long	lCnt;
	unsigned char*	pImagePtr;
	ulong			start;
	ulong			interval;
	unsigned long	lPixel;
	int				bFirstImage = 1;

	/* MUST set env var "bootlcd", or LCD is disabled at bootup */

	pBootLcd = getenv( "bootlcd" );

	if ( pBootLcd==0 || strcmp( pBootLcd, "0" ) == 0 )
		return 0;

	lcd_Init();

	if ( lcd_CurrChip == 0 )
		return 0;

	printf ( "Uncompressing LCD bootup images ............... " );

	sprintf( sCmd, "nand read.e %p $(f_lcdimage_addr) $(f_lcdimage_size)", pLzma );
	run_command( sCmd, 0 );

	if ( strcmp( (char*)pLzma, "lcd_boot_image" ) != 0 ) // wrong image ID
	{
		printf( "wrong image ID\n" );
		goto err_Boot_Lcd_Init;
	}

	lLzmaSize = (SizeT)simple_strtoul( (char*)pLzma+16, 0, 10 );

	if ( lLzmaSize >= 0x400000 )
	{
		printf( "wrong LZMA image size\n" );
		goto err_Boot_Lcd_Init;
	}

	if ( lzmaBuffToBuffDecompress( pImages, &lAllImageSize, pLzma+160, lLzmaSize ) != SZ_OK)
	{
		printf( "wrong LZMA image format\n" );
		goto err_Boot_Lcd_Init;
	}

	if ( lAllImageSize != (SizeT)simple_strtoul( (char*)pLzma+32, 0, 10 ) )
	{
		printf( "wrong uncompressed image size\n" );
		goto err_Boot_Lcd_Init;
	}

	lFileCnt = (SizeT)simple_strtoul( (char*)pLzma+28, 0, 10 );
	interval = (ulong)simple_strtoul( (char*)pLzma+48, 0, 10 );
	pFiles = (char*)(pLzma + 52);

	for ( lCnt=0, pImagePtr=pImages; lCnt < lFileCnt; lCnt++ )
	{
		if (pFiles[lCnt] == '0')
		{
			pImagePtr += (LCD_PIXEL_NUM*3);
			continue;
		}

		for ( lPixel=0; lPixel < LCD_PIXEL_NUM; lPixel++, pImagePtr+=3 )
			lcd_Gram_Mirror[lPixel] = RGB24_TO_16( pImagePtr[0], pImagePtr[1], pImagePtr[2] );

		start = get_timer(0);

		lcd_Flush();

		if ( bFirstImage )
		{
			bFirstImage = 0;
			lcd_SetBackLight( lcd_GetBackLight(), 16 );
		}

		while (get_timer(start) < interval)
		{
			if ( ctrlc() )
				return 1;
			udelay(100);
		}
	}

	return 0;


err_Boot_Lcd_Init:

	lcd_FillSolid( COLOR16_MAGENTA );
	lcd_SetBackLight( lcd_GetBackLight(), 1 );

	return 0;
}

int do_lcd(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char*			cmd;
	unsigned char	reg;
	unsigned short	data;
	unsigned short	x0, x1, y0, y1;
	unsigned short	color2;

	if ( argc < 2 )
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "info") == 0)
	{
		putc('\n');
		if ( lcd_CurrChip && lcd_CurrChip->devName )
			printf( "device name: %s\n", lcd_CurrChip->devName );
		printf( "device ID:   0x%04x\n", lcd_GetControllerID() );
		printf( "width:       %u\n", lcd_GetWidth() );
		printf( "height:      %u\n", lcd_GetHeight() );
		printf( "orientation: %u\n", (lcd_GetOrientation()==LCD_ORIENTATION_PORTRAIT ? LCD_ORIENTATION_PORTRAIT : LCD_ORIENTATION_LANDSCAPE) );
		printf( "backlight:   %u\n", lcd_GetBackLight() );
		printf( "led:         %u\n", lcd_GetLed() );
		return 0;
	}

	if (strcmp(cmd, "init") == 0)
	{
		putc('\n');
		lcd_boot_Init();
		return 0;
	}

	if (strcmp(cmd, "reg_get") == 0)
	{
		if ( argc < 3 )
			goto usage;
		reg = (unsigned char)simple_strtoul( argv[2], NULL, 16 );
		putc('\n');
		lcd_ReadRegister( reg, &data );
		printf( "register 0x%02x = 0x%04x\n", reg, data );
		return 0;
	}

	if (strcmp(cmd, "reg_set") == 0)
	{
		if ( argc < 4 )
			goto usage;
		reg = (unsigned char)simple_strtoul( argv[2], NULL, 16 );
		data = (unsigned short)simple_strtoul( argv[3], NULL, 16 );
		lcd_WriteRegister( reg, data );
		putc('\n');
		lcd_ReadRegister( reg, &data );
		printf( "register 0x%02x = 0x%04x\n", reg, data );
		return 0;
	}

	if (strcmp(cmd, "color") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 16 );
		lcd_FillSolid( data );
		return 0;
	}

	if (strcmp(cmd, "gradient") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 16 );
		if ( argc >= 4 )
			color2 = (unsigned short)simple_strtoul( argv[3], NULL, 16 );
		else
			color2 = ~data;
		lcd_FillGradient( data, color2 );
		return 0;
	}

	if (strcmp(cmd, "rect") == 0)
	{
		if ( argc < 7 )
			goto usage;
		x0 = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		y0 = (unsigned short)simple_strtoul( argv[3], NULL, 10 );
		x1 = (unsigned short)simple_strtoul( argv[4], NULL, 10 );
		y1 = (unsigned short)simple_strtoul( argv[5], NULL, 10 );
		data = (unsigned short)simple_strtoul( argv[6], NULL, 16 );
		lcd_FillSolidRect( x0, y0, x1, y1, data );
		return 0;
	}

	if (strcmp(cmd, "grect") == 0)
	{
		if ( argc < 7 )
			goto usage;
		x0 = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		y0 = (unsigned short)simple_strtoul( argv[3], NULL, 10 );
		x1 = (unsigned short)simple_strtoul( argv[4], NULL, 10 );
		y1 = (unsigned short)simple_strtoul( argv[5], NULL, 10 );
		data = (unsigned short)simple_strtoul( argv[6], NULL, 16 );
		if ( argc >= 8 )
			color2 = (unsigned short)simple_strtoul( argv[7], NULL, 16 );
		else
			color2 = ~data;
		lcd_FillGradientRect( x0, y0, x1, y1, data, color2 );
		return 0;
	}

	if (strcmp(cmd, "bl") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		lcd_SetBackLight( (unsigned char)data, 1 );
		return 0;
	}

	if (strcmp(cmd, "led") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		lcd_SetLed( (unsigned char)data, 1 );
		return 0;
	}

	if (strcmp(cmd, "msleep") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		lcd_DelayMs( (unsigned char)data );
		return 0;
	}

	if (strcmp(cmd, "usleep") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		lcd_DelayUs( (unsigned char)data );
		return 0;
	}

	if (strcmp(cmd, "nsleep") == 0)
	{
		if ( argc < 3 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		lcd_DelayNs( (unsigned char)data );
		return 0;
	}

	if (strcmp(cmd, "gpio") == 0)
	{
		if ( argc < 4 )
			goto usage;
		data = (unsigned short)simple_strtoul( argv[2], NULL, 10 );
		if ( simple_strtoul( argv[3], NULL, 16 ) == 0 )
			lcd_GpioSet( (int)data, 0 );
		else
			lcd_GpioSet( (int)data, 1 );
		return 0;
	}


  usage:

	cmd_usage(cmdtp);

	return 1;
}

U_BOOT_CMD( lcd, CONFIG_SYS_MAXARGS, 1, do_lcd,
	"lcd sub-system",
	"info                  - show LCD devices info\n"
	"lcd init                  - initialize\n"
	"lcd reg_get  <reg>        - get register, hex-decimal\n"
	"lcd reg_set  <reg> <val>  - set register, hex-decimal\n"
	"lcd color    <color>      - draw full screen solid color, hex-decimal\n"
	"lcd rect     <x0> <y0> <x1> <y1> <color>\n"
    "                          - draw rectangle solid color\n"
	"                          -   x, y decimal, color hex-decimal\n"
	"lcd gradient <color> [color2]\n"
	"                          - draw full screen gradient color, hex-decimal\n"
	"lcd grect    <x0> <y0> <x1> <y1> <color> [color2]\n"
    "                          - draw rectangle solid color\n"
	"                          -   x, y decimal, color hex-decimal\n"
	"lcd bl       <level>      - backlight level, decimal\n"
	"lcd led      <level>      - LED level, decimal\n"
	"lcd gpio     <gpio> <0|1> - GPIO, decimal\n"
	"lcd msleep   <msec>       - sleep msec, milli-seconds\n"
	"lcd usleep   <usec>       - sleep usec, micro-seconds\n"
	"lcd nsleep   <nsec>       - sleep nsec, nano-seconds\n"
);


#endif // CONFIG_DRIVER_VR9 ctc LCD

