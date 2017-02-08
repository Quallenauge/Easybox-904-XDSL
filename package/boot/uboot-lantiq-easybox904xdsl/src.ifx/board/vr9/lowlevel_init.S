
/*
 *  Memory sub-system initialization code for INCA-IP2 development board.
 *  Andre Messerschmidt
 *  Copyright (c) 2005  Infineon Technologies AG 
 *
 *  Based on Inca-IP code 
 *  Copyright (c) 2003  Wolfgang Denk <wd@denx.de>
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
/* History:
      Wu Qi Ming Sep 18, 2009 initial version
 */
#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>

#ifdef CONFIG_VR9_DDR1
  #ifdef CONFIG_VR9_CPU_333M_RAM_166M
   #ifdef CONFIG_ENABLE_POWER_MANAGEMENT
    #include "ddr1/xrx200_ddr1_167M_lp_settings.h"
   #else
    #include "ddr1/xrx200_ddr1_167M_settings.h"
   #endif
  #elif defined(CONFIG_VR9_CPU_393M_RAM_196M)
    #ifdef CONFIG_ENABLE_POWER_MANAGEMENT
        #include "ddr1/xrx200_ddr1_200M_lp_settings.h"
	#else
        #include "ddr1/xrx200_ddr1_200M_settings.h"
	#endif	
  #else /*500M/200M, use the same ddr setting file*/
    #include "ddr1/xrx200_ddr1_200M_settings.h"
  #endif
#endif

#ifdef CONFIG_VR9_DDR2
  #if defined(CONFIG_VR9_CPU_393M_RAM_196M) || defined(CONFIG_VR9_CPU_500M_RAM_200M) || defined(CONFIG_VR9_CPU_600M_RAM_200M)
   #ifdef CONFIG_ENABLE_POWER_MANAGEMENT
       #include "ddr2/xrx200_ddr2_200M_lp_settings.h"
   #else
       #include "ddr2/xrx200_ddr2_200M_settings.h"
   #endif	
  #elif defined(CONFIG_VR9_CPU_500M_RAM_250M) 
   #ifdef CONFIG_ENABLE_POWER_MANAGEMENT
       #include "ddr2/xrx200_ddr2_250M_lp_settings.h"
   #else
       #include "ddr2/xrx200_ddr2_250M_settings.h"
   #endif
  #elif defined(CONFIG_VR9_CPU_600M_RAM_300M) 
   #ifdef CONFIG_ENABLE_POWER_MANAGEMENT
       #include "ddr2/xrx200_ddr2_300M_lp_settings.h"
   #else
       #include "ddr2/xrx200_ddr2_300M_settings.h"
   #endif 
  #else /*333M/166M, use the same ddr setting file*/
   #ifdef CONFIG_ENABLE_POWER_MANAGEMENT
       #include "ddr2/xrx200_ddr2_167M_lp_settings.h"
   #else
       #include "ddr2/xrx200_ddr2_167M_settings.h"
   #endif
  #endif
#endif

#define PMU_OFFSET  0xBF102000
#define PMU_PWDCR1  (PMU_OFFSET + 0x0024)

#define CGU_OFFSET  0xBF103000
#define PLL0_CFG    (CGU_OFFSET + 0x0004)
#define PLL1_CFG    (CGU_OFFSET + 0x0008)    
#define CGU_SYS     (CGU_OFFSET + 0x000C)
#define CGU_CLKFSR  (CGU_OFFSET + 0x0010)
#define CGU_CLKGSR  (CGU_OFFSET + 0x0014)
#define CGU_CLKGCR0 (CGU_OFFSET + 0x0018)
#define CGU_CLKGCR1 (CGU_OFFSET + 0x001C)
#define CGU_UPDATE  (CGU_OFFSET + 0x0020)
#define IF_CLK      (CGU_OFFSET + 0x0024)
#define CGU_DDR     (CGU_OFFSET + 0x0028)
#define CGU_CT1SR   (CGU_OFFSET + 0x002C)
#define CGU_CT_KVAL (CGU_OFFSET + 0x0030)
#define CGU_PCMCR   (CGU_OFFSET + 0x0034)
#define PCI_CR      (CGU_OFFSET + 0x0038)
#define GPHY1_Cfg   (CGU_OFFSET + 0x0040)
#define GPHY0_Cfg   (CGU_OFFSET + 0x004C)
#define PLL2_CFG    (CGU_OFFSET + 0x0060)


#define RCU_OFFSET   0xBF203000
#define RST_REQ      (RCU_OFFSET + 0x0010)

#define DDR_OFFSET   0xbf401000
#define DDR_CCR00      (DDR_OFFSET + 0x0000 )
#define DDR_CCR01      (DDR_OFFSET + 0x0010 )
#define DDR_CCR02      (DDR_OFFSET + 0x0020 )
#define DDR_CCR03      (DDR_OFFSET + 0x0030 )
#define DDR_CCR04      (DDR_OFFSET + 0x0040 )
#define DDR_CCR05      (DDR_OFFSET + 0x0050 )
#define DDR_CCR06      (DDR_OFFSET + 0x0060 )
#define DDR_CCR07      (DDR_OFFSET + 0x0070 )
#define DDR_CCR08      (DDR_OFFSET + 0x0080 )
#define DDR_CCR09      (DDR_OFFSET + 0x0090 )
#define DDR_CCR10      (DDR_OFFSET + 0x00A0 )
#define DDR_CCR11      (DDR_OFFSET + 0x00B0 )
#define DDR_CCR12      (DDR_OFFSET + 0x00C0 )
#define DDR_CCR13      (DDR_OFFSET + 0x00D0 )
#define DDR_CCR14      (DDR_OFFSET + 0x00E0 )
#define DDR_CCR15      (DDR_OFFSET + 0x00F0 )
#define DDR_CCR16      (DDR_OFFSET + 0x0100 )
#define DDR_CCR17      (DDR_OFFSET + 0x0110 )
#define DDR_CCR18      (DDR_OFFSET + 0x0120 )
#define DDR_CCR19      (DDR_OFFSET + 0x0130 )
#define DDR_CCR20      (DDR_OFFSET + 0x0140 )
#define DDR_CCR21      (DDR_OFFSET + 0x0150 )
#define DDR_CCR22      (DDR_OFFSET + 0x0160 )
#define DDR_CCR23      (DDR_OFFSET + 0x0170 )
#define DDR_CCR24      (DDR_OFFSET + 0x0180 )
#define DDR_CCR25      (DDR_OFFSET + 0x0190 )
#define DDR_CCR26      (DDR_OFFSET + 0x01A0 )
#define DDR_CCR27      (DDR_OFFSET + 0x01B0 )
#define DDR_CCR28      (DDR_OFFSET + 0x01C0 )
#define DDR_CCR29      (DDR_OFFSET + 0x01D0 )
#define DDR_CCR30      (DDR_OFFSET + 0x01E0 )
#define DDR_CCR31      (DDR_OFFSET + 0x01F0 )
#define DDR_CCR32      (DDR_OFFSET + 0x0200 )
#define DDR_CCR33      (DDR_OFFSET + 0x0210 )
#define DDR_CCR34      (DDR_OFFSET + 0x0220 )
#define DDR_CCR35      (DDR_OFFSET + 0x0230 )
#define DDR_CCR36      (DDR_OFFSET + 0x0240 )
#define DDR_CCR37      (DDR_OFFSET + 0x0250 )
#define DDR_CCR38      (DDR_OFFSET + 0x0260 )
#define DDR_CCR39      (DDR_OFFSET + 0x0270 )
#define DDR_CCR40      (DDR_OFFSET + 0x0280 )
#define DDR_CCR41      (DDR_OFFSET + 0x0290 )
#define DDR_CCR42      (DDR_OFFSET + 0x02A0 )
#define DDR_CCR43      (DDR_OFFSET + 0x02B0 )
#define DDR_CCR44      (DDR_OFFSET + 0x02C0 )
#define DDR_CCR45      (DDR_OFFSET + 0x02D0 )
#define DDR_CCR46      (DDR_OFFSET + 0x02E0 )
#define DDR_CCR47      (DDR_OFFSET + 0x02F0 )
#define DDR_CCR48      (DDR_OFFSET + 0x0300 )
#define DDR_CCR49      (DDR_OFFSET + 0x0310 )
#define DDR_CCR50      (DDR_OFFSET + 0x0320 )
#define DDR_CCR51      (DDR_OFFSET + 0x0330 )
#define DDR_CCR52      (DDR_OFFSET + 0x0340 )
#define DDR_CCR53      (DDR_OFFSET + 0x0350 )
#define DDR_CCR54      (DDR_OFFSET + 0x0360 )
#define DDR_CCR55      (DDR_OFFSET + 0x0370 )
#define DDR_CCR56      (DDR_OFFSET + 0x0380 )
#define DDR_CCR57      (DDR_OFFSET + 0x0390 )
#define DDR_CCR58      (DDR_OFFSET + 0x03A0 )
#define DDR_CCR59      (DDR_OFFSET + 0x03B0 )
#define DDR_CCR60      (DDR_OFFSET + 0x03C0 )
#define DDR_CCR61      (DDR_OFFSET + 0x03D0 )
#define DDR_CCR62      (DDR_OFFSET + 0x03E0 )
#define DDR_CCR63      (DDR_OFFSET + 0x03F0 )
#define DDR_CCR64      (DDR_OFFSET + 0x0400 )
#define DDR_CCR65      (DDR_OFFSET + 0x0410 )
#define DDR_CCR66      (DDR_OFFSET + 0x0420 )
#define DDR_CCR67      (DDR_OFFSET + 0x0430 )
#define DDR_CCR68      (DDR_OFFSET + 0x0440 )
#define DDR_CCR69      (DDR_OFFSET + 0x0450 )
#define DDR_CCR70      (DDR_OFFSET + 0x0460 )
#define DDR_CCR71      (DDR_OFFSET + 0x0470 )
#define DDR_CCR72      (DDR_OFFSET + 0x0480 )
#define DDR_CCR73      (DDR_OFFSET + 0x0490 )
#define DDR_CCR74      (DDR_OFFSET + 0x04A0 )
#define DDR_CCR75      (DDR_OFFSET + 0x04B0 )
#define DDR_CCR76      (DDR_OFFSET + 0x04C0 )
#define DDR_CCR77      (DDR_OFFSET + 0x04D0 )
#define DDR_CCR78      (DDR_OFFSET + 0x04E0 )
#define DDR_CCR79      (DDR_OFFSET + 0x04F0 )
#define DDR_CCR80      (DDR_OFFSET + 0x0500 )
#define DDR_CCR81      (DDR_OFFSET + 0x0510 )
#define DDR_CCR82      (DDR_OFFSET + 0x0520 )
#define DDR_CCR83      (DDR_OFFSET + 0x0530 )
#define DDR_CCR84      (DDR_OFFSET + 0x0540 )
#define DDR_CCR85      (DDR_OFFSET + 0x0550 )
#define DDR_CCR86      (DDR_OFFSET + 0x0560 )
#define DDR_CCR87      (DDR_OFFSET + 0x0570 )
#define DDR_CCR88      (DDR_OFFSET + 0x0580 )
#define DDR_CCR89      (DDR_OFFSET + 0x0590 )
#define DDR_CCR90      (DDR_OFFSET + 0x05A0 )
#define DDR_CCR91      (DDR_OFFSET + 0x05B0 )
#define DDR_CCR92      (DDR_OFFSET + 0x05C0 )
#define DDR_CCR93      (DDR_OFFSET + 0x05D0 )
#define DDR_CCR94      (DDR_OFFSET + 0x05E0 )
#define DDR_CCR95      (DDR_OFFSET + 0x05F0 )
#define DDR_CCR96      (DDR_OFFSET + 0x0600 )
#define DDR_CCR97      (DDR_OFFSET + 0x0610 )
#define DDR_CCR98      (DDR_OFFSET + 0x0620 )
#define DDR_CCR99      (DDR_OFFSET + 0x0630 )
#define DDR_CCR100     (DDR_OFFSET + 0x0640 )
#define DDR_CCR101     (DDR_OFFSET + 0x0650 )
#define DDR_CCR102     (DDR_OFFSET + 0x0660 )
#define DDR_CCR103     (DDR_OFFSET + 0x0670 )
#define DDR_CCR104     (DDR_OFFSET + 0x0680 )
#define DDR_CCR105     (DDR_OFFSET + 0x0690 )
#define DDR_CCR106     (DDR_OFFSET + 0x06A0 )
#define DDR_CCR107     (DDR_OFFSET + 0x06B0 )
#define DDR_CCR108     (DDR_OFFSET + 0x06C0 )
#define DDR_CCR109     (DDR_OFFSET + 0x06D0 )
#define DDR_CCR110     (DDR_OFFSET + 0x06E0 )
#define DDR_CCR111     (DDR_OFFSET + 0x06F0 )
#define DDR_CCR112     (DDR_OFFSET + 0x0700 )
#define DDR_CCR113     (DDR_OFFSET + 0x0710 )
#define DDR_CCR114     (DDR_OFFSET + 0x0720 )
#define DDR_CCR115     (DDR_OFFSET + 0x0730 )
#define DDR_CCR116     (DDR_OFFSET + 0x0740 )
#define DDR_CCR117     (DDR_OFFSET + 0x0750 )
#define DDR_CCR118     (DDR_OFFSET + 0x0760 )
#define DDR_CCR119     (DDR_OFFSET + 0x0770 )
#define DDR_CCR120     (DDR_OFFSET + 0x0780 )
#define DDR_CCR121     (DDR_OFFSET + 0x0790 )
#define DDR_CCR122     (DDR_OFFSET + 0x07A0 )
#define DDR_CCR123     (DDR_OFFSET + 0x07B0 )
#define DDR_CCR124     (DDR_OFFSET + 0x07C0 )
#define DDR_CCR125     (DDR_OFFSET + 0x07D0 )
#define DDR_CCR126     (DDR_OFFSET + 0x07E0 )
#define DDR_CCR127     (DDR_OFFSET + 0x07F0 )
#define DDR_CCR128     (DDR_OFFSET + 0x0800 )
#define DDR_CCR129     (DDR_OFFSET + 0x0810 )
#define DDR_CCR130     (DDR_OFFSET + 0x0820 )
#define DDR_CCR131     (DDR_OFFSET + 0x0830 )
#define DDR_CCR132     (DDR_OFFSET + 0x0840 )
#define DDR_CCR133     (DDR_OFFSET + 0x0850 )
#define DDR_CCR134     (DDR_OFFSET + 0x0860 )
#define DDR_CCR135     (DDR_OFFSET + 0x0870 )
#define DDR_CCR136     (DDR_OFFSET + 0x0880 )
#define DDR_CCR137     (DDR_OFFSET + 0x0890 )
#define DDR_CCR138     (DDR_OFFSET + 0x08A0 )
#define DDR_CCR139     (DDR_OFFSET + 0x08B0 )
#define DDR_CCR140     (DDR_OFFSET + 0x08C0 )
#define DDR_CCR141     (DDR_OFFSET + 0x08D0 )
#define DDR_CCR142     (DDR_OFFSET + 0x08E0 )
#define DDR_CCR143     (DDR_OFFSET + 0x08F0 )
#define DDR_CCR144     (DDR_OFFSET + 0x0900 )
#define DDR_CCR145     (DDR_OFFSET + 0x0910 )
#define DDR_CCR146     (DDR_OFFSET + 0x0920 )
#define DDR_CCR147     (DDR_OFFSET + 0x0930 )
#define DDR_CCR148     (DDR_OFFSET + 0x0940 )
#define DDR_CCR149     (DDR_OFFSET + 0x0950 )
#define DDR_CCR150     (DDR_OFFSET + 0x0960 )
#define DDR_CCR151     (DDR_OFFSET + 0x0970 )
#define DDR_CCR152     (DDR_OFFSET + 0x0980 )
#define DDR_CCR153     (DDR_OFFSET + 0x0990 )
#define DDR_CCR154     (DDR_OFFSET + 0x09A0 )
#define DDR_CCR155     (DDR_OFFSET + 0x09B0 )
#define DDR_CCR156     (DDR_OFFSET + 0x09C0 )
#define DDR_CCR157     (DDR_OFFSET + 0x09D0 )
#define DDR_CCR158     (DDR_OFFSET + 0x09E0 )
#define DDR_CCR159     (DDR_OFFSET + 0x09F0 )
#define DDR_CCR160     (DDR_OFFSET + 0x0A00 )
#define DDR_CCR161     (DDR_OFFSET + 0x0A10 )
#define DDR_CCR162     (DDR_OFFSET + 0x0A20 )
#define DDR_CCR163     (DDR_OFFSET + 0x0A30 )
#define DDR_CCR164     (DDR_OFFSET + 0x0A40 )
#define DDR_CCR165     (DDR_OFFSET + 0x0A50 )
#define DDR_CCR166     (DDR_OFFSET + 0x0A60 )
#define DDR_CCR167     (DDR_OFFSET + 0x0A70 )
#define DDR_CCR168     (DDR_OFFSET + 0x0A80 )
#define DDR_CCR169     (DDR_OFFSET + 0x0A90 )
#define DDR_CCR170     (DDR_OFFSET + 0x0AA0 )
#define DDR_CCR171     (DDR_OFFSET + 0x0AB0 )
#define DDR_CCR172     (DDR_OFFSET + 0x0AC0 )
#define DDR_CCR173     (DDR_OFFSET + 0x0AD0 )
#define DDR_CCR174     (DDR_OFFSET + 0x0AE0 )
#define DDR_CCR175     (DDR_OFFSET + 0x0AF0 )
#define DDR_CCR176     (DDR_OFFSET + 0x0B00 )
#define DDR_CCR177     (DDR_OFFSET + 0x0B10 )
#define DDR_CCR178     (DDR_OFFSET + 0x0B20 )
#define DDR_CCR179     (DDR_OFFSET + 0x0B30 )
#define DDR_CCR180     (DDR_OFFSET + 0x0B40 )
#define DDR_CCR181     (DDR_OFFSET + 0x0B50 )
#define DDR_CCR182     (DDR_OFFSET + 0x0B60 )
#define DDR_CCR183     (DDR_OFFSET + 0x0B70 )
#define DDR_CCR184     (DDR_OFFSET + 0x0B80 )
#define DDR_CCR185     (DDR_OFFSET + 0x0B90 )
#define DDR_CCR186     (DDR_OFFSET + 0x0BA0 )
#define DDR_CCR187     (DDR_OFFSET + 0x0BB0 )
#define DDR_CCR188     (DDR_OFFSET + 0x0BC0 )
#define DDR_CCR189     (DDR_OFFSET + 0x0BD0 )
#define DDR_CCR190     (DDR_OFFSET + 0x0BE0 )
#define DDR_CCR191     (DDR_OFFSET + 0x0BF0 )

#if CONFIG_VR9_CRYSTAL_6M /*6M crystal clock*/
     #define CONFIG_VR9_PLL0_CFG_VALUE 0x00B00C49
     #define CONFIG_VR9_PLL1_CFG_VALUE 0x9D267001    
	   #define CONFIG_VR9_PLL2_CFG_VALUE 0x293011c1
#elif CONFIG_VR9_CRYSTAL_CPLD /*6M change to 36M via CPLD*/
     /*fix me, change to the correct value*/
     #define CONFIG_VR9_PLL0_CFG_VALUE 0x00B00621 
     #define CONFIG_VR9_PLL1_CFG_VALUE 0x9BA67015 /*default value for 36M crystal*/
     #define CONFIG_VR9_PLL2_CFG_VALUE 0x293002C1 /*default value for 36M crystal*/
#elif CONFIG_VR9_CRYSTAL_25M /*25M crystal clock, for GRX288*/
    /*fix me, change to the correct value*/
    #define CONFIG_VR9_PLL0_CFG_VALUE 0x00F018D1   /*default value for 25M crystal*/
    #define CONFIG_VR9_PLL1_CFG_VALUE 0x984E6F4D   /*default value for 25M crystal*/
    #define CONFIG_VR9_PLL2_CFG_VALUE 0x0A200441
    #define PLL2_DISABLE	0x0a200000
	#define PLL2_HIGH_M	0x0a20003d
	#define PLL2_DEFAULT	0x0a200441
#else /*36M crystal clock, for VRX288*/
    #define CONFIG_VR9_PLL0_CFG_VALUE 0x00b01f21 /*default value for 36M crystal*/
    #define CONFIG_VR9_PLL1_CFG_VALUE 0x9BA67015 /*default value for 36M crystal*/
    #define CONFIG_VR9_PLL2_CFG_VALUE 0x293002C1 /*default value for 36M crystal*/
    #define PLL2_DISABLE	0x29300000
	#define PLL2_HIGH_M	0x2930003d
	#define PLL2_DEFAULT	0x293002c1

#endif


#ifdef CONFIG_VR9_CPU_125M_RAM_125M
   #define CONFIG_VR9_CGU_SYS_VALUE    0x90
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03020000
#elif defined(CONFIG_VR9_CPU_333M_RAM_166M)
   #define CONFIG_VR9_CGU_SYS_VALUE    0x32
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03020000
#elif defined(CONFIG_VR9_CPU_393M_RAM_196M)
   #define CONFIG_VR9_CGU_SYS_VALUE    0x22
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03020000
#elif defined(CONFIG_VR9_CPU_500M_RAM_250M)
   #define CONFIG_VR9_CGU_SYS_VALUE    0x12
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03010000 /*PPE 432M freq*/
#elif defined(CONFIG_VR9_CPU_600M_RAM_200M) 
   #define CONFIG_VR9_CGU_SYS_VALUE    0x04
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03010000 /*PPE 432M freq*/
#elif defined(CONFIG_VR9_CPU_600M_RAM_300M)
   #define CONFIG_VR9_CGU_SYS_VALUE    0x02
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03000000 /*PPE 432M freq*/  
#else
      /*fix me, put correct values here*/
   #define CONFIG_VR9_CGU_SYS_VALUE    0x13
   #define CONFIG_VR9_CGU_CLKFSR_VALUE 0x03020000
#endif


  .set  noreorder


#ifdef CONFIG_CHECK_PLL2_LOCK
   .align 4
   .globl  pll2_lock_workaround
   .ent  pll2_lock_workaround

pll2_lock_workaround: /* workaround code for PLL2 */
   	move    t4,ra
	li	t0,0xbf103000
    lw	t1,0x60(t0)
	andi	t1,t1,0x2
	bne	zero,t1,3f  /* PLL2 lock branch to continue */
	nop
/* disable_pll2 */

	li	t3,0x10
1:
	li	t1, PLL2_DISABLE
	sw      t1,0x60(t0)
    li	t1,0x1
	sw	t1,0x20(t0)
	bal	delay_asm_1ms
	nop
							
/* enable pll2 at low freq */
	li	t1, PLL2_HIGH_M
	sw      t1,0x60(t0)
	li	t1,0x1
	sw	t1,0x20(t0)
    bal	delay_asm_1ms
	nop

/* Check PLL2 lock at low freq */
	lw	t1,0x60(t0)
    andi	t1,t1,0x2
	bne	zero,t1,2f  /* PLL2 lock branch to continue */
	nop
	addi	t3,t3,-1
	bne	zero,t3,1b
	nop

/* Set PLL2 to Default Freq */
2:
	li	t1, PLL2_DEFAULT
	sw      t1,0x60(t0)
    li	t1,0x1
	sw	t1,0x20(t0)
	bal	delay_asm_1ms
	nop
	
/* Continue execution */
3:
	move	ra,t4
    j	ra
	nop

/* Delay routine for 1ms */
  .align 4
delay_asm_1ms:
	li    k1, (CPU_CLOCK_RATE/2000)
	mtc0  zero, CP0_COUNT
1:
  mfc0  k0, CP0_COUNT
	sub   k0,k1,k0
	bgez  k0,1b
	nop
    j  ra
	nop
	
    .end  pll2_lock_workaround
#endif



/*
 * void cgu_init(long)
 */
  .globl  cgu_init
  .ent  cgu_init
cgu_init:
	li  t2, CGU_SYS
    lw  t2, 0(t2)
    bne t2, CONFIG_VR9_CGU_SYS_VALUE, 1f /*check if CGU_SYS is correct*/
    nop
	
    li  t2, CGU_CLKFSR
	lw  t2, 0(t2)
	bne t2, CONFIG_VR9_CGU_CLKFSR_VALUE, 1f /*check if CGU_CLKFSR is correct*/
	nop
	
freq_up2date:
    move  t5, ra
#ifdef CONFIG_CHECK_PLL2_LOCK
    bal pll2_lock_workaround
    nop
#endif
    move  ra, t5
    j   ra    /*all values are correct, return*/
	nop
	
1:  
#if CONFIG_VR9_CRYSTAL_CPLD
    /*Pull high GPIO44, P2.12, used as output*/
    /*P2_ALTSEL0 &=~(1<<12)*/
	li t1, 0xBE100B7C
	lw t2, 0(t1)
	li t3, 0xffffefff
	and t2, t2, t3
	sw t2, 0(t1)

    /*P2_ALTSEL1 &=~(1<<12)*/
	li t1, 0xBE100B80
	lw t2, 0(t1)
	li t3, 0xffffefff
	and t2, t2, t3
	sw t2, 0(t1)

    /*P2_DIR |=1<<12*/
	li t1, 0xBE100B78
	lw t2, 0(t1)
	li t3, 0x1000
	or t2, t2, t3
	sw t2, 0(t1)
	
	/*P2_OD |=1<<12*/
    li t1, 0xBE100B84
	lw t2, 0(t1)
	li t3, 0x1000
	or t2, t2, t3
	sw t2, 0(t1)
					
	/*P2_OUT |=1<<12*/
	li t1, 0xBE100B70
	lw t2, 0(t1)
	li t3, 0x1000
	or t2, t2, t3
	sw t2, 0(t1)

#if 0
    /*Pull high GPIO41, P2.9, used as output*/
	/*P2_ALTSEL0 &=~(1<<9)*/
	li t1, 0xBE100B7C
	lw t2, 0(t1)
	li t3, 0xfffffdff
    and t2, t2, t3
    sw t2, 0(t1)

    /*P2_ALTSEL1 &=~(1<<9)*/
	li t1, 0xBE100B80
	lw t2, 0(t1)
	li t3, 0xfffffdff
	and t2, t2, t3
    sw t2, 0(t1)

    /*P2_DIR |=1<<9*/
	li t1, 0xBE100B78
	lw t2, 0(t1)
	li t3, 0x200
	or t2, t2, t3
	sw t2, 0(t1)

	/*P2_OD |=1<<9*/
	li t1, 0xBE100B84
	lw t2, 0(t1)
	li t3, 0x200
	or t2, t2, t3
	sw t2, 0(t1)

	/*P2_OUT |=1<<9*/
	li t1, 0xBE100B70
	lw t2, 0(t1)
	li t3, 0x200
	or t2, t2, t3
	sw t2, 0(t1)
																														
#endif

    /*setup GPIO13*/ 
	/*REG32(0xBE100B1C) &= ~0x2000*/
	li t1, 0xBE100B1C
	lw t2, 0(t1)
	li t3, 0xffffdfff
	and t2, t2, t3
	sw t2, 0(t1)

	/*REG32(0xBE100B20) &= ~0x2000*/
	li t1, 0xBE100B20
	lw t2, 0(t1)
	li t3, 0xffffdfff
	and t2, t2, t3
	sw t2, 0(t1)
						
	/*REG32(0xBE100B18) |= 0x2000*/
	li t1, 0xBE100B18
	lw t2, 0(t1)
	li t3, 0x2000
	or t2, t2, t3
	sw t2, 0(t1)

	/*REG32(0xBE100B24) |= 0x2000*/
	li t1, 0xBE100B24
	lw t2, 0(t1)
	li t3, 0x2000
	or t2, t2, t3
	sw t2, 0(t1)
					
    /*Toggle GPIO13 5 times*/					
	li t0, 0 /*5 times*/
	li t4, 4
1:
	
	/*REG32(0xBE100B10) |= 0x2000;*/
	li t1, 0xBE100B10
    lw t2, 0(t1)
    li t3, 0x2000
    or t2, t2, t3
    sw t2, 0(t1)

    /*delay*/
    li t1, 0
delay1:
	li t2, 0x200
	addi t1, t1, 1
	bne t1, t2, delay1
	nop

	/*REG32(0xBE100B10) &= ~0x2000;*/
	li t1, 0xBE100B10
	lw t2, 0(t1)
	li t3, 0xffffdfff
	and t2, t2, t3
	sw t2, 0(t1)
	
    /*delay*/
    li t1, 0
delay2:
	li t2, 0x200
	addi t1, t1, 1
	bne t1, t2, delay2
	nop
	
    addi t0, t0, 1
	bne t0, t4, 1b
    nop
#endif

    li  t2, CGU_SYS
    li  a0, CONFIG_VR9_CGU_SYS_VALUE
    sw  a0, 0(t2)    /*store predefined value to CGU_SYS*/
    
	li  t2, CGU_CLKFSR
    li  a0, CONFIG_VR9_CGU_CLKFSR_VALUE /*store predefined value to CGU_CLKFSR*/
    sw  a0, 0(t2)
//#if defined(CONFIG_VR9_CRYSTAL_6M) || defined(CONFIG_VR9_CRYSTAL_CPLD)
	li  t2, PLL0_CFG
    li  a0, CONFIG_VR9_PLL0_CFG_VALUE /*store predefined value to PLL0_CFG*/
	sw  a0, 0(t2)

	li  t2, PLL1_CFG
	li  a0, CONFIG_VR9_PLL1_CFG_VALUE /*store predefined value to PLL1_CFG*/
	sw  a0, 0(t2)

	li  t2, PLL2_CFG
	li  a0, CONFIG_VR9_PLL2_CFG_VALUE /*store predefined value to PLL2_CFG*/
	sw  a0, 0(t2)
//#endif	
    li  t2, CGU_UPDATE
    li  a0, 1
    sw  a0, 0(t2) /*write 0x1 to CGU_UPDATE*/
    li  t2, RST_REQ
    li  a0, 0xe0000000
    sw  a0, 0(t2)  /*issue global software reset*/
wait_reset:
    b   wait_reset
    nop
    .end  cgu_init



/*
 * void ddrram_init(long)
 */
  .globl  ddrram_init
  .ent  ddrram_init
ddrram_init:
#ifdef CONFIG_ENABLE_POWER_MANAGEMENT
   li  t2, PMU_PWDCR1
   lw  t3, 0(t2)
   li  t4, 0xffffffBf
   and t3, t3, t4 /*bit6 set to 0*/
   sw  t3, 0(t2)
#endif
   li  t2, DDR_CCR07
   li  a0, 0
   sw  a0, 0(t2) /*put DDR controller inactive*/
  
   li  t2, DDR_CCR00
   li  a0, MC_CCR00_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR01
   li  a0, MC_CCR01_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR02
   li  a0, MC_CCR02_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR03
   li  a0, MC_CCR03_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR04
   li  a0, MC_CCR04_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR05
   li  a0, MC_CCR05_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR06
   li  a0, MC_CCR06_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR07
   li  a0, MC_CCR07_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR08
   li  a0, MC_CCR08_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR09
   li  a0, MC_CCR09_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR10
   li  a0, MC_CCR10_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR11
   li  a0, MC_CCR11_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR12
   li  a0, MC_CCR12_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR13
   li  a0, MC_CCR13_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR14
   li  a0, MC_CCR14_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR15
   li  a0, MC_CCR15_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR16
   li  a0, MC_CCR16_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR17
   li  a0, MC_CCR17_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR18
   li  a0, MC_CCR18_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR19
   li  a0, MC_CCR19_VALUE
#ifdef CONFIG_LOWPOWER_AUTO_ENABLE
   li   t1, 0xe0ffffff 
   and  a0, a0, t1
   li   t1, CONFIG_LOWPOWER_AUTO_ENABLE
   andi t1, t1, 0x1f
   sll  t1, t1, 24
   add  a0, a0, t1 
#endif
   sw  a0, 0(t2)

   li  t2, DDR_CCR20
   li  a0, MC_CCR20_VALUE
#ifdef CONFIG_LOWPOWER_CONTROL
   li  t1, 0xffffffe0
   and a0, a0, t1
   li  t1, CONFIG_LOWPOWER_CONTROL
   andi t1, t1, 0x1f
   add a0, a0, t1
#endif   
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR21
   li  a0, MC_CCR21_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR22
   li  a0, MC_CCR22_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR23
   li  a0, MC_CCR23_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR24
   li  a0, MC_CCR24_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR25
   li  a0, MC_CCR25_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR26
   li  a0, MC_CCR26_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR27
   li  a0, MC_CCR27_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR28
   li  a0, MC_CCR28_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR29
   li  a0, MC_CCR29_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR30
   li  a0, MC_CCR30_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR31
   li  a0, MC_CCR31_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR32
   li  a0, MC_CCR32_VALUE
#ifdef CONFIG_LOWPOWER_POWER_DOWN_CNT
   li  t1, 0xffff0000
   and a0, a0, t1
   li  t1, CONFIG_LOWPOWER_POWER_DOWN_CNT
   andi t1, t1, 0xffff
   add a0, a0, t1
#endif      
   sw  a0, 0(t2)

   li  t2, DDR_CCR33
   li  a0, MC_CCR33_VALUE
#ifdef CONFIG_LOWPOWER_SELF_REFRESH_CNT
   li  t1, 0xffff0000
   and a0, a0, t1
   li  t1, CONFIG_LOWPOWER_SELF_REFRESH_CNT
   andi t1, t1, 0xffff
   add a0, a0, t1
#endif      
   sw  a0, 0(t2)

   li  t2, DDR_CCR34
   li  a0, MC_CCR34_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR35
   li  a0, MC_CCR35_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR36
   li  a0, MC_CCR36_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR37
   li  a0, MC_CCR37_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR38
   li  a0, MC_CCR38_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR39
   li  a0, MC_CCR39_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR40
   li  a0, MC_CCR40_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR41
   li  a0, MC_CCR41_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR42
   li  a0, MC_CCR42_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR43
   li  a0, MC_CCR43_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR44
   li  a0, MC_CCR44_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR45
   li  a0, MC_CCR45_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR46
   li  a0, MC_CCR46_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR47
   li  a0, MC_CCR47_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR48
   li  a0, MC_CCR48_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR49
   li  a0, MC_CCR49_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR50
   li  a0, MC_CCR50_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR51
   li  a0, MC_CCR51_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR52
   li  a0, MC_CCR52_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR53
   li  a0, MC_CCR53_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR54
   li  a0, MC_CCR54_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR55
   li  a0, MC_CCR55_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR56
   li  a0, MC_CCR56_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR57
   li  a0, MC_CCR57_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR58
   li  a0, MC_CCR58_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR59
   li  a0, MC_CCR59_VALUE
   sw  a0, 0(t2)

   li  t2, DDR_CCR60
   li  a0, MC_CCR60_VALUE
   sw  a0, 0(t2)
   
   li  t2, DDR_CCR61
   li  a0, MC_CCR61_VALUE
   sw  a0, 0(t2)

#if defined(CONFIG_TUNE_DDR) && defined(CONFIG_BOOT_FROM_NOR)
  #if 1 /*ctc add to extend address space*/
	li	s4, 0xBE105300
	li	s5, CONFIG_EBU_ADDSEL0 /* 0x10000021, ctc extend to 32MB */
	sw	s5, 0x20(s4)
  #endif
   li      t3, IFX_CFG_FLASH_DDR_CFG_START_ADDR
   lw      t4, 0(t3)
   li      t3, 0x88888888
   bne     t3, t4, ddr_not_configured
   nop
/*calculate the crc value*/
    li      t3, IFX_CFG_FLASH_DDR_CFG_START_ADDR
    addi    t3, t3, 0x4
    lw      t4, 0(t3)
    lw      t5, 4(t3)
    xor     t4, t4, t5
    lw      t5, 8(t3)
    xor     t4, t4, t5
    lw      t5, 0xc(t3)
    xor     t4, t4, t5
    lw      t5, 0x10(t3)
    bne     t4, t5, ddr_not_configured
    nop
/*crc correct, load the stored value*/
    li      t2, IFX_CFG_FLASH_DDR_CFG_START_ADDR
    addi    t2, t2, 0x04
    lw      t2, 0(t2)
    li      t1, DDR_CCR39
    sw      t2, 0(t1)

    li      t2, IFX_CFG_FLASH_DDR_CFG_START_ADDR
    addi    t2, t2, 0x08
    lw      t2, 0(t2)
    li      t1, DDR_CCR40
    sw      t2, 0(t1)
    
    li      t2, IFX_CFG_FLASH_DDR_CFG_START_ADDR
    addi    t2, t2, 0x0c
    lw      t2, 0(t2)
    li      t1, DDR_CCR43
    sw      t2, 0(t1)
    
    li      t2, IFX_CFG_FLASH_DDR_CFG_START_ADDR
    addi    t2, t2, 0x10
    lw      t2, 0(t2)
    li      t1, DDR_CCR44
    sw      t2, 0(t1)
    
    /*ddr use stored value, put 0 in 0xBE22FF20*/
    li      t3, 0xBE22FF20
    li      t4, 0x0
    sw      t4, 0(t3)
    b       2f
    nop

ddr_not_configured:
    /*ddr not configured, put 0xff in 0xBE22FF20*/
    li      t3, 0xBE22FF20
    li      t4, 0xff
    sw      t4, 0(t3)
    
    /*set 0 to the 3rd Most significant byte of DDR_CCR39*/
    li      t2, DDR_CCR39
    lw      t1, 0(t2);
    li      a0, 0xFFFF00FF;
    and     t1, t1, a0
    sw      t1, 0(t2)
    
    /*set 0 to the 3rd Most significant byte of DDR_CCR40*/
    li      t2, DDR_CCR40
    lw      t1, 0(t2);
    li      a0, 0xFFFF00FF;
    and     t1, t1, a0
    sw      t1, 0(t2)
    
#endif
2:
   /*need to disable fpi burst in register "always_last"*/
   li      t3, 0xBF400430
   li      t4, 0x04
   sw      t4, 0(t3)

   li  t2, DDR_CCR07
   li  a0, 0x1010100
   sw  a0, 0(t2) /*put DDR controller active*/
  
   /*DENALI_CTL_47 and DENALI_CTL_48 registers bit 0 indicates DLL lock status*/
1:
   li  t2, DDR_CCR47
   lw  t2, 0(t2)
   li  t3, 0x01
   and t2, t3
   bne t2, t3, 1b 
   nop
   li  t2, DDR_CCR48
   lw  t2, 0(t2)
   li  t3, 0x01
   and t2, t3
   bne t2, t3, 1b
   nop 			   
   j  ra
   nop
   .end ddrram_init
   
   
   
  .globl  lowlevel_init
  .ent  lowlevel_init
lowlevel_init:
  /* EBU, CGU and SDRAM/DDR-RAM Initialization.
   */
  move  t6, ra

    bal cgu_init
    nop
 
#ifdef CONFIG_TUNE_DDR
    li      t1, DDR_CCR07
    lw      t1, 0(t1)
    andi    t1, t1, 0x100
    bne     t1, zero, ddr_init_finish
    nop
#endif
 
 
  bal ddrram_init
  nop


ddr_init_finish:
  move  ra, t6
  j ra
  nop

  .end  lowlevel_init
