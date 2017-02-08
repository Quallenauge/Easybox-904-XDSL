/*
 * (C) Copyright 2003
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
#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <asm/vr9.h>
#include <environment.h>
#include "dsl_address_define.h"
#include <nand.h>
#include <spi_flash.h>

#if !defined(DEBUG_ENABLE_BOOTSTRAP_PRINTF) && defined(CFG_BOOTSTRAP_CODE)
#define printf
#endif

#ifndef CONFIG_ENV_SPI_BUS
# define CONFIG_ENV_SPI_BUS 0
#endif
#ifndef CONFIG_ENV_SPI_CS
# define CONFIG_ENV_SPI_CS      0
#endif
#ifndef CONFIG_ENV_SPI_MAX_HZ
# define CONFIG_ENV_SPI_MAX_HZ  1000000
#endif
#ifndef CONFIG_ENV_SPI_MODE
# define CONFIG_ENV_SPI_MODE    SPI_MODE_3
#endif

#ifdef CONFIG_NOR_FLASH_AUTO
void save_extra_env()
{
   extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
   extern xway_flash_partition_map_info flash_map[];
   int i,j,total_db,total_part;
   int mtd_part;
   u32  tmp;
   char strTemp[32], strCommand[128];
   char strimg_addr[32],strimg_size[32],strimg_end[32];
   i=0;
   while(flash_map[i].size !=0){
    	 if(flash_map[i].size == flash_info[0].size>>20) break;
    	 i++;
    }
   total_db = 0;
   mtd_part = -1;
   total_part = 0;
   for(j=0;j<XWAY_MAX_BLOCK_NUM;j++){
   	     if(strcmp(flash_map[i].block[j].blockName,"")!=0){
   	     	 total_db++;

   	     	 sprintf(strTemp,"data_block%d",j);
   	     	 if(!getenv(strTemp)) setenv(strTemp, flash_map[i].block[j].blockName);
           sprintf(strTemp,"f_%s_addr",flash_map[i].block[j].blockName);
           sprintf(strimg_addr,"0x%08x",flash_map[i].block[j].startAddr);
           if(!getenv(strTemp)) setenv(strTemp,strimg_addr);
           if(flash_map[i].block[j].mtdNum!=mtd_part){
   	     	    mtd_part = flash_map[i].block[j].mtdNum;
   	     	    total_part ++;
   	     	    sprintf(strTemp, "part%d_begin", j);
   	     	    if(!getenv(strTemp)) setenv(strTemp,strimg_addr);
   	     	 }

           	sprintf(strTemp,"f_%s_size",flash_map[i].block[j].blockName);
            sprintf(strimg_size,"0x%08x",flash_map[i].block[j].sizeBytes);
            if(!getenv(strTemp)) setenv(strTemp,strimg_size);
            sprintf(strTemp,"f_%s_end",flash_map[i].block[j].blockName);
            sprintf(strimg_end,"0x%08x", flash_map[i].block[j].startAddr + flash_map[i].block[j].sizeBytes - 1);
            if(!getenv(strTemp)) setenv(strTemp,strimg_end);

            if(strcmp(flash_map[i].block[j].blockName,"ubootconfig")==0) {
           	   sprintf(strCommand,"prot off 0x%08x %s;erase 0x%08x %s 1",\
           	           flash_map[i].block[j].startAddr,strimg_end,\
           	           flash_map[i].block[j].startAddr,strimg_end);
           	   sprintf(strTemp,"reset_uboot_config");
           	   if(!getenv(strTemp)) setenv(strTemp,strCommand);

            }


        }
    }
    sprintf(strTemp, "%d", total_db);
    if(!getenv(strTemp)) setenv("total_db", strTemp);
    sprintf(strTemp, "%d", total_part);
    if(!getenv(strTemp)) setenv("total_part", strTemp);
    sprintf(strTemp, "0x%08x", 0xb0000000 + flash_info[0].size);
    if(!getenv(strTemp)) setenv("flash_end", strTemp);
    sprintf(strTemp, "mtdparts");
    if(!getenv(strTemp)) setenv(strTemp,flash_map[i].mtdparts);

   return;
}
#endif



phys_size_t initdram(int board_type)
{
	return (1024*1024*CONFIG_IFX_MEMORY_SIZE);
}


int flash_probe(void)
{
   (*BSP_EBU_BUSCON0) = CONFIG_EBU_BUSCON0;  //0x1d7ff; ctc, value from Aikann, should be used on the real chip
   (*BSP_EBU_ADDSEL0) = CONFIG_EBU_ADDSEL0;  //0x10000021; ctc extend to 32MB, //starting address from 0xb0000000
  #if defined(CONFIG_SYS_MAX_FLASH_BANKS) && (CONFIG_SYS_MAX_FLASH_BANKS > 1) /* ctc */
	(*BSP_EBU_BUSCON1) = CONFIG_EBU_BUSCON1;  //0x1d7ff; ctc, value from Aikann, should be used on the real chip
	(*BSP_EBU_ADDSEL1) = CONFIG_EBU_ADDSEL1;  //0x10000021; ctc extend to 32MB, //starting address from 0xb0000000
  #endif
   (*BSP_EBU_NAND_CON)=0;
   return 1;
}

void fpi_dfe_reg_write(u32 offset, u32 data)
{
    REG32(FPIAD(offset)) = data;
}

u32 fpi_dfe_reg_read(u32 offset)
{
    return(REG32(FPIAD(offset)));
}

void afe_reg_write(u32 addr, u32 data)
{
    fpi_dfe_reg_write(V_AFE_SERIAL_ADDR, addr);
    fpi_dfe_reg_write(V_AFE_SERIAL_WDATA, data);
}


unsigned short config_afe(void)
{
     *BSP_PMU_PWDCR &= ~(1<<9); /*turn on afe*/
	 REG32(0xbf20302c) |= 0x40000000;
	 fpi_dfe_reg_write(V_AFE_SERIAL_CFG, 0x90);
	 afe_reg_write(0x324, 0x8402);
	 REG32(0xbf20302c) &=~(0x40000000);
     return 0;
}

void show_boot_progress(int arg)
{
  return;
}

void config_dcdc(u8 value)
{
	 u8 inc=0;
	 u8 orig=REG8(PDI_DCDC_DIG_REF);



   REG8(PDI_DCDC_DUTY_CYCLE_MAX_SAT) = 0x5A;  /*DUTY_CYCLE_SAT_MAX = 90*/
   REG8(PDI_DCDC_DUTY_CYCLE_MIN_SAT) = 0x46;  /*DUTY_CYCLE_SAT_MIN = 70*/

   REG8(PDI_DCDC_CONF_TEST_DIG) |=(u8)(1<<5); /*FREEZE_PID=1*/

   /*Programming of new coefficients (6 registers a 8 bits)*/
   REG8(PDI_DCDC_PID_HI_B0) = 0x00;
   REG8(PDI_DCDC_PID_LO_B0) = 0x00;
   REG8(PDI_DCDC_PID_HI_B1) = 0xff;
   REG8(PDI_DCDC_PID_LO_B1) = 0xe6;
   REG8(PDI_DCDC_PID_HI_B2) = 0x00;
   REG8(PDI_DCDC_PID_LO_B2) = 0x1b;
#ifdef CONFIG_GRX200
   REG8(PDI_DCDC_NON_OV_DELAY) = 0x5B;
#else
   REG8(PDI_DCDC_NON_OV_DELAY) = 0x8B;
#endif

   REG8(PDI_DCDC_DUTY_CYCLE_MAX_SAT) = 0x6C;     /*DUTY_CYCLE_SAT_MAX = 108 (0x6C)*/
   REG8(PDI_DCDC_DUTY_CYCLE_MIN_SAT) = 0x3C;     /*DUTY_CYCLE_SAT_MIN = 60*/
   REG8(PDI_DCDC_CONF_TEST_DIG) &=(u8)(~(1<<5)); /*FREEZE_PID=0*/

   REG8(PDI_DCDC_CLK_SET0) |= 0x40; /*Set bit 6, select PWM*/
   REG8(PDI_DCDC_CLK_SET1) |= 0x20;
   REG8(PDI_DCDC_PWM0) = 0xF9; // 249 in decimal

   if(orig > value) inc=-1;
	 else if(orig <value)	inc=1;
	 else return;
	 while( REG8(PDI_DCDC_DIG_REF)!= value)
	 {
	 	  mdelay(1);
	 	  REG8(PDI_DCDC_DIG_REF) +=inc;
	 }

}

void msleep( u32 msec )
{
	u32	i;

	for ( i = 0 ; i < msec ; i++ )
	{
		udelay( 1000 );
	}
}

int check_pll1_lock(void)
{
   int result = 0;/*0 fail, 1 success*/
   int i;
#ifdef CONFIG_VR9_CRYSTAL_25M
   u32 pll1_value[4]={0x984E6785,0x984E63C1,0x984E6B09,0x984E6F4D};
   int LOOP = 4;
#elif CONFIG_VR9_CRYSTAL_36M
   u32 pll1_value[5]={0x9BA66545,0x9BA66809,0x9BA66ACD,0x9BA66D91,0x9BA67015};
   int LOOP = 5;

#endif
   for(i=0;i<LOOP;i++){
     if(!(*BSP_PLL1_CFG & 0x2)) {
	     if(i==LOOP-1) {
               result = 0;
			   printf("PLL1 lock failed!\n");
			   break;
		               }
	     *BSP_PLL1_CFG &=~1;  /*disable PLL1*/
		 *BSP_CGU_UPDATE |=1;
		 *BSP_PLL1_CFG = pll1_value[i];
		 *BSP_CGU_UPDATE |=1;
		 mdelay(1);
		 //printf("i=%d,PLL1_CFG=%08x\n",i,*BSP_PLL1_CFG);

	     }
     else{
	         if((i!=0) && (i!= LOOP-1)){
                  i=LOOP-2;
				  *BSP_PLL1_CFG = pll1_value[LOOP-1];
				  *BSP_CGU_UPDATE |=1;
				  mdelay(1);
				  //printf("2. i=%d,PLL1_CFG=%08x\n",i,*BSP_PLL1_CFG);
                }else{
                 result = 1;
			     break;
                }
	      }/*if(!(*BSP_PLL1_CFG & 0x2))*/
	 }/*for(i=0;i<LOOP;i++)*/
   return result;
}

int checkboard (void)
{
	unsigned long temp;
    printf("CLOCK CPU %dM RAM %dM\n",CPU_CLOCK_RATE/1000000,RAM_CLOCK_RATE/1000000);
    config_afe();
#ifdef CONFIG_VR9_CRYSTAL_25M
	REG8(0xbf106b00)=0x3;
#endif

  #if 1 /*ctc, reset GPIO */
	*BSP_GPIO_P0_OUT     = 0x00000000;
	*BSP_GPIO_P0_DIR     = 0x00006c10;
	*BSP_GPIO_P0_ALTSEL0 = 0x00002010;
	*BSP_GPIO_P0_ALTSEL1 = 0x00000080;
	*BSP_GPIO_P0_OD      = 0x00006c70;
	*BSP_GPIO_P0_PUDSEL  = 0x00001070;
	*BSP_GPIO_P0_PUDEN   = 0x00001070;
#if 0
	*BSP_GPIO_P1_OUT     = 0x00000010;
	*BSP_GPIO_P1_DIR     = 0x000081d8;
	*BSP_GPIO_P1_ALTSEL0 = 0x000085e0;
	*BSP_GPIO_P1_ALTSEL1 = 0x00000000;
	*BSP_GPIO_P1_OD      = 0x000081d8;
#else	// set default value for lcd panel back light
	/*
	GPIP 21 used as output for Wireless reset
	PI_OUT.5
	P1_ALTSEL0.5=0
	P1_ALTSEL1.5=0
	P1_DIR.5=1
	*/
	*BSP_GPIO_P1_DIR     = 0x000091f8;
	*BSP_GPIO_P1_ALTSEL0 = 0x000085c0;
	*BSP_GPIO_P1_ALTSEL1 = 0x00000000;
	*BSP_GPIO_P1_OD      = 0x000091f8;
	// output to low
	temp = *BSP_GPIO_P1_OUT;
	temp = temp & 0xffffffdf;
	*BSP_GPIO_P1_OUT      = temp;
#endif
	*BSP_GPIO_P1_PUDSEL  = 0x00008509;
	*BSP_GPIO_P1_PUDEN   = 0x000085a9;
	*BSP_GPIO_P2_OUT     = 0x00001200;
	*BSP_GPIO_P2_DIR     = 0x00001a02;
	*BSP_GPIO_P2_ALTSEL0 = 0x00000000;
	*BSP_GPIO_P2_ALTSEL1 = 0x00000000;
	*BSP_GPIO_P2_OD      = 0x00001202;
	*BSP_GPIO_P2_PUDSEL  = 0x00000400;
	*BSP_GPIO_P2_PUDEN   = 0x00000400;
	*BSP_GPIO_P3_OUT     = 0x00000000;
	*BSP_GPIO_P3_DIR     = 0x00000002;
	*BSP_GPIO_P3_ALTSEL0 = 0x00000003;
	*BSP_GPIO_P3_ALTSEL1 = 0x00000000;
	*BSP_GPIO_P3_OD      = 0x00000002;
	*BSP_GPIO_P3_PUDSEL  = 0x00000000;
	*BSP_GPIO_P3_PUDEN   = 0x00000000;
  #else
	printf( "BSP_GPIO_P0_OUT     : 0x%08x\n", *BSP_GPIO_P0_OUT );
	printf( "BSP_GPIO_P0_DIR     : 0x%08x\n", *BSP_GPIO_P0_DIR );
	printf( "BSP_GPIO_P0_ALTSEL0 : 0x%08x\n", *BSP_GPIO_P0_ALTSEL0 );
	printf( "BSP_GPIO_P0_ALTSEL1 : 0x%08x\n", *BSP_GPIO_P0_ALTSEL1 );
	printf( "BSP_GPIO_P0_OD      : 0x%08x\n", *BSP_GPIO_P0_OD );
	printf( "BSP_GPIO_P0_PUDSEL  : 0x%08x\n", *BSP_GPIO_P0_PUDSEL );
	printf( "BSP_GPIO_P0_PUDEN   : 0x%08x\n", *BSP_GPIO_P0_PUDEN );
	printf( "BSP_GPIO_P1_OUT     : 0x%08x\n", *BSP_GPIO_P1_OUT );
	printf( "BSP_GPIO_P1_DIR     : 0x%08x\n", *BSP_GPIO_P1_DIR );
	printf( "BSP_GPIO_P1_ALTSEL0 : 0x%08x\n", *BSP_GPIO_P1_ALTSEL0 );
	printf( "BSP_GPIO_P1_ALTSEL1 : 0x%08x\n", *BSP_GPIO_P1_ALTSEL1 );
	printf( "BSP_GPIO_P1_OD      : 0x%08x\n", *BSP_GPIO_P1_OD );
	printf( "BSP_GPIO_P1_PUDSEL  : 0x%08x\n", *BSP_GPIO_P1_PUDSEL );
	printf( "BSP_GPIO_P1_PUDEN   : 0x%08x\n", *BSP_GPIO_P1_PUDEN );
	printf( "BSP_GPIO_P2_OUT     : 0x%08x\n", *BSP_GPIO_P2_OUT );
	printf( "BSP_GPIO_P2_DIR     : 0x%08x\n", *BSP_GPIO_P2_DIR );
	printf( "BSP_GPIO_P2_ALTSEL0 : 0x%08x\n", *BSP_GPIO_P2_ALTSEL0 );
	printf( "BSP_GPIO_P2_ALTSEL1 : 0x%08x\n", *BSP_GPIO_P2_ALTSEL1 );
	printf( "BSP_GPIO_P2_OD      : 0x%08x\n", *BSP_GPIO_P2_OD );
	printf( "BSP_GPIO_P2_PUDSEL  : 0x%08x\n", *BSP_GPIO_P2_PUDSEL );
	printf( "BSP_GPIO_P2_PUDEN   : 0x%08x\n", *BSP_GPIO_P2_PUDEN );
	printf( "BSP_GPIO_P3_OUT     : 0x%08x\n", *BSP_GPIO_P3_OUT );
	printf( "BSP_GPIO_P3_DIR     : 0x%08x\n", *BSP_GPIO_P3_DIR );
	printf( "BSP_GPIO_P3_ALTSEL0 : 0x%08x\n", *BSP_GPIO_P3_ALTSEL0 );
	printf( "BSP_GPIO_P3_ALTSEL1 : 0x%08x\n", *BSP_GPIO_P3_ALTSEL1 );
	printf( "BSP_GPIO_P3_OD      : 0x%08x\n", *BSP_GPIO_P3_OD );
	printf( "BSP_GPIO_P3_PUDSEL  : 0x%08x\n", *BSP_GPIO_P3_PUDSEL );
	printf( "BSP_GPIO_P3_PUDEN   : 0x%08x\n", *BSP_GPIO_P3_PUDEN );
  #endif

    *EBU_ADDR_SEL_0 = CONFIG_EBU_ADDSEL0; //0x10000021; ctc extend to 32MB, /*has to do this or nand probing will hang*/
    *EBU_CON0 = CONFIG_EBU_BUSCON0;
  #if defined(CONFIG_SYS_MAX_FLASH_BANKS) && (CONFIG_SYS_MAX_FLASH_BANKS > 1) /* ctc */
	*EBU_ADDR_SEL_1 = CONFIG_EBU_ADDSEL1; //0x10000021; ctc extend to 32MB, /*has to do this or nand probing will hang*/
	*EBU_CON1 = CONFIG_EBU_BUSCON1;
  #endif

  #if 1 /* ctc for LCD control via EBU bus */
	*BSP_GPIO_P2_ALTSEL0 |= 0x0001;
	*BSP_GPIO_P2_ALTSEL1 |= 0x0001;
	*BSP_GPIO_P2_DIR     |= 0x0001;
	*BSP_GPIO_P2_OD      |= 0x0001;
	*(EBU_ADDR_SEL_0+2)   = 0x160000f1; /* EBU_ADDR_SEL_2 */
	*(EBU_CON_0+2)        = 0x0001d7ff; /* EBU_CON_2 */
  #endif

	asm( "sync" );

#ifndef CONFIG_VR9_CPU_393M_RAM_196M
    if(!check_pll1_lock()) return 1;
#endif


    return 0;
}


#ifdef CONFIG_NAND_FLASH
void nand_gpio_init(void)
{
	  *BSP_GPIO_P3_DIR=0x2;
    *BSP_GPIO_P3_ALTSEL0=0x3;
    *BSP_GPIO_P3_ALTSEL1=0x0;
    *BSP_GPIO_P3_OD=0x3;

    /* set GPIO pins for NAND */
    /* P0.13 FL_A24 01:output*/
    /* P1.8 FL_A23 01:output*/
    *BSP_GPIO_P0_ALTSEL0 |= 0x2000;
    *BSP_GPIO_P0_ALTSEL1 &= (~0x2000);
    *BSP_GPIO_P0_DIR |= (0x2000);
    *BSP_GPIO_P1_ALTSEL0 |= 0x100;
    *BSP_GPIO_P1_ALTSEL1 &= (~0x100);
    *BSP_GPIO_P1_DIR |= (0x100);

    /*Set GPIO7 as general GPIO output instead of parking at CFRAME*/
    *BSP_GPIO_P0_ALTSEL0 &=(~0x80);
    *BSP_GPIO_P0_ALTSEL1 &=(~0x80);
    *BSP_GPIO_P0_DIR |= (0x80);
    *BSP_GPIO_P0_DIR |= (0x80);

}
#endif



#ifdef CONFIG_LANTIQ_SPI

int spi_gpio_init(unsigned int cs)
{
   /* enable SSC1 */
        //*DANUBE_PMU_PM_GEN |= DANUBE_PMU_PM_GEN_EN11;

        /* SSC1 Ports */
        /* p0.15 SPI_CS1(EEPROM), P0.13 SPI_CS3, P0.9 SPI_CS5, P0.10 SPI_CS4, P0.11 SPI_CS6 */
        /* Set p0.10 to alternative 01, others to 00 (In/OUT)*/
        *(BSP_GPIO_P0_DIR) = (*BSP_GPIO_P0_DIR)|(0xAE00);

        *(BSP_GPIO_P0_ALTSEL0) = (((*BSP_GPIO_P0_ALTSEL0)&(~0x0400)) & (~(0xAA00)));
        *(BSP_GPIO_P0_ALTSEL1) = (((*BSP_GPIO_P0_ALTSEL1)|(0x0400)) & (~(0xAA00)) );

        *(BSP_GPIO_P0_OD) = (*BSP_GPIO_P0_OD)|0xAE00;

        /* p1.6 SPI_CS2(SFLASH), p1.0 SPI_DIN, p1.1 SPI_DOUT, p1.2 SPI_CLK */
        *(BSP_GPIO_P1_DIR) = ((*BSP_GPIO_P1_DIR)|(0x46))&(~1);
        *(BSP_GPIO_P1_ALTSEL0) = ((*BSP_GPIO_P1_ALTSEL0)|(0x47));
        *(BSP_GPIO_P1_ALTSEL1) = (*BSP_GPIO_P1_ALTSEL1)&(~0x47);
        *(BSP_GPIO_P1_OD) = (*BSP_GPIO_P1_OD)|0x0046;

	 return 1;
}
#endif

#ifdef CONFIG_TUNE_DDR
#ifdef CONFIG_BOOT_FROM_NOR
void save_ddr_param(void)
{
   int rcode;
	 ulong   ddr_magic=0x88888888;
	 ulong erase_addr1=0, erase_addr2=0;
	 volatile ulong* ptr=IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	 ulong  ecc;
	 ulong  buffer[6];
	 erase_addr1 = IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	 erase_addr2 = IFX_CFG_FLASH_DDR_CFG_START_ADDR + IFX_CFG_FLASH_DDR_CFG_SIZE;
	 if(*(u32*)0xBE22FF20!=0x2)  /*0xBE1A5F20 contains the tuning status*/
	    {
		    /*no new parameter, return immediately*/
		    return;
	    }
  if(flash_sect_protect (0, erase_addr1, erase_addr2-1))
     {
		      printf("protect off error!\n");
		 }

  if (flash_sect_erase (erase_addr1, erase_addr2-1))
     {
		       printf("erase error!\n");
		 }
	 rcode = flash_sect_erase(erase_addr1, erase_addr2-1);


   memcpy ((u8*)buffer,&ddr_magic,4);
	 memcpy ((u8*)(buffer+1),(u8*)0xBF401270,4);
	 memcpy ((u8*)(buffer+2),(u8*)0xBF401280,4);
	 memcpy ((u8*)(buffer+3),(u8*)0xBF4012B0,4);
	 memcpy ((u8*)(buffer+4),(u8*)0xBF4012C0,4);
	 ecc=(*(u32*)0xBF401270)^(*(u32*)0xBF401280)^(*(u32*)0xBF4012B0)^(*(u32*)0xBF4012C0);
	 memcpy ((u8*)(buffer+5),&ecc,4);

	 flash_write((char *)buffer, IFX_CFG_FLASH_DDR_CFG_START_ADDR, 24);/*one magic word,4 parameters,1cc,24bytes*/

	 //(void) flash_sect_protect (1, erase_addr1, erase_addr2-1);

     return;
}
#elif defined(CONFIG_BOOT_FROM_NAND)
extern nand_info_t nand_info[];
//extern int nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf);

void save_ddr_param(void)
{
	   int rcode;
	   ulong   ddr_magic=0x88888888;
	   ulong erase_addr1=0, erase_addr2=0;
	   volatile ulong* ptr=IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	   ulong  ecc;
	   ulong  buffer[6];
	   erase_addr1 = IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	   erase_addr2 = IFX_CFG_FLASH_DDR_CFG_START_ADDR + IFX_CFG_FLASH_DDR_CFG_SIZE;
	   nand_info_t *nand;
	   nand_write_options_t opts;
	   nand = &nand_info[0];
	   u64 srcLen=24;
	   if(*(u32*)0xBE22FF20!=0x2)  /*0xBE1A5F20 contains the tuning status*/
	    {
		    /*no new parameter, return immediately*/
		    return;
	    }
	   memcpy ((u8*)buffer,&ddr_magic,4);
	   memcpy ((u8*)(buffer+1),(u8*)0xBF401270,4);
	   memcpy ((u8*)(buffer+2),(u8*)0xBF401280,4);
	   memcpy ((u8*)(buffer+3),(u8*)0xBF4012B0,4);
	   memcpy ((u8*)(buffer+4),(u8*)0xBF4012C0,4);
	   ecc=(*(u32*)0xBF401270)^(*(u32*)0xBF401280)^(*(u32*)0xBF4012B0)^(*(u32*)0xBF4012C0);
	   memcpy ((u8*)(buffer+5),&ecc,4);

	   nand_write_partial(&nand_info[0], IFX_CFG_FLASH_DDR_CFG_START_ADDR, &srcLen, (u_char*)buffer);
	   return;
}
#else /*BOOT from SPI*/
void save_ddr_param(void)
{
	 int rcode;
	 ulong   ddr_magic=0x88888888;
	 ulong erase_addr1=0, erase_addr2=0;
	 volatile ulong* ptr=IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	 ulong  ecc;
	 ulong  buffer[6];
	 static struct spi_flash *flash_spi;
	 flash_spi = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
            CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);

	 erase_addr1 = IFX_CFG_FLASH_DDR_CFG_START_ADDR;
	 erase_addr2 = IFX_CFG_FLASH_DDR_CFG_START_ADDR + IFX_CFG_FLASH_DDR_CFG_SIZE;
	 if(*(u32*)0xBE22FF20!=0x2)   /*0xBE1A5F20 contains the tuning status*/
	    {
		    /*no new parameter, return immediately*/
		    return;
	    }

	 memcpy ((u8*)buffer,&ddr_magic,4);
	 memcpy ((u8*)(buffer+1),(u8*)0xBF401270,4);
	 memcpy ((u8*)(buffer+2),(u8*)0xBF401280,4);
	 memcpy ((u8*)(buffer+3),(u8*)0xBF4012B0,4);
	 memcpy ((u8*)(buffer+4),(u8*)0xBF4012C0,4);
	 ecc=(*(u32*)0xBF401270)^(*(u32*)0xBF401280)^(*(u32*)0xBF4012B0)^(*(u32*)0xBF4012C0);
	 memcpy ((u8*)(buffer+5),&ecc,4);

	 spi_flash_write(flash_spi, erase_addr1, 24, (char *)buffer);
	 printf("saved ddr param in flash!\n");

   return;
	 return;
}
#endif
#endif

extern int vr9_eth_initialize(bd_t * bis);

int board_eth_init(bd_t *bis)
{
   if (vr9_eth_initialize(bis)<0)
             return -1;

	return 0;
}

