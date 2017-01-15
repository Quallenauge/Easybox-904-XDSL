
/*
 * AMAZON_S internal switch ethernet driver.
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>


#include <config.h>
#include <malloc.h>
#include <net.h>
#include <asm/vr9.h>
#include <asm/addrspace.h>
#include <asm/pinstrap.h>

#ifdef CONFIG_BOOT_FROM_NAND
#include <nand.h>
#endif

#ifdef CONFIG_BOOT_FROM_SPI
#include <spi_flash.h>
#endif


#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>

#ifdef CONFIG_DRIVER_VR9 /* ctc for RTL8367RB */
#include <rtk_error.h>
#include <rtl8367b_reg.h>
#endif

#define TX_CHAN_NO   1
#define RX_CHAN_NO   0

#define NUM_RX_DESC	PKTBUFSRX
#define NUM_TX_DESC	8
#define MAX_PACKET_SIZE 	1536
#define TOUT_LOOP	100


#define RGMII 0
#define GMII  1
#define TMII  2
#define MII   3
#define RMII  4
				
#define CONFIG_VR9_SW_PORT(NUM)   CONFIG_VR9_SW_PORT_##NUM

#define MDIO_WRITE_CMD  ((0 << 11)| (1 <<10))
#define MDIO_READ_CMD  ((1 << 11) | (0<<10))

extern int dcache_linesize_mips32(void);
extern void dcache_writeback_invalidate(u32 size, u32  dcache_line_size, u32 start_addr);
extern void dcache_hit_invalidate(u32 size, u32  dcache_line_size, u32 start_addr);
extern void mdelay (unsigned long msec);

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 reserved		 :3;
			volatile u32 Byteoffset		 :2;
			volatile u32 reserve             :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;

	volatile u32 DataPtr;
} dma_rx_descriptor_t;

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 Byteoffset		 :5;
			volatile u32 reserved            :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;

	volatile u32 DataPtr;
} dma_tx_descriptor_t;

typedef struct
{
    int on;
	int miimode;
	int miirate;
}vr9_sw_port;


static dma_rx_descriptor_t rx_des_ring[NUM_RX_DESC] __attribute__ ((aligned(8)));
static dma_tx_descriptor_t tx_des_ring[NUM_TX_DESC] __attribute__ ((aligned(8)));
static vr9_sw_port vr9_port[7];
static int tx_num, rx_num;
#if CONFIG_VR9_SW_PORT_2 | CONFIG_VR9_SW_PORT_3 | CONFIG_VR9_SW_PORT_4 | CONFIG_VR9_SW_PORT_5b 
static u8 gphy_fw[65536] __attribute__ ((aligned(32)));
#endif

int vr9_switch_init(struct eth_device *dev, bd_t * bis);
int vr9_switch_send(struct eth_device *dev, volatile void *packet,int length);
int vr9_switch_recv(struct eth_device *dev);
void vr9_switch_halt(struct eth_device *dev);
static void vr9_sw_chip_init(void);
static void vr9_dma_init(void);

u16 vr9_mdio_init(void)
{
    *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0xc00);
    *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0xc00);
    *BSP_GPIO_P2_DIR = *BSP_GPIO_P2_DIR  | 0x800;
    *BSP_GPIO_P2_OD = *BSP_GPIO_P2_OD | 0xc00;

		return 0;
}
	
u16 vr9_mdio_read(u8 phyaddr, u8 phyreg)
{
        u16 i=0;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000);
        REG32(VR9_ETHSW_MDIO_CTRL) = MDIO_READ_CMD | (((u32)phyaddr)<<5) | ((u32) phyreg)|0x1000 ;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000)
        {
        	i++;
        	if(i>0x7fff)
        	{
        		  printf("MDIO access time out!\n");
        		  break;
        	}
        }
        return((u16) (REG32(VR9_ETHSW_MDIO_READ)));
}

void vr9_mdio_write(u8 phyaddr, u8 phyreg, u16 data)
{
        u16 i=0;
        
        REG32(VR9_ETHSW_MDIO_WRITE)= data;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000);
        REG32(VR9_ETHSW_MDIO_CTRL) =  MDIO_WRITE_CMD | (((u32)phyaddr)<<5) | ((u32) phyreg)|0x1000 ;
        while(REG32(VR9_ETHSW_MDIO_CTRL) & 0x1000)
        {
        	i++;
        	if(i>0x7fff)
        	{
        		  printf("MDIO access time out!\n");
        		  break;
        	}
        }
}
					


int vr9_eth_initialize(bd_t * bis)
{
		struct eth_device *dev;

		if(!(dev = (struct eth_device *) malloc (sizeof *dev)))
    {
        printf("Failed to allocate memory\n");
        return 0;
    }

		memset(dev, 0, sizeof(*dev));

    asm("sync");
    vr9_sw_chip_init();
    asm("sync");
		
		/***************/
		sprintf(dev->name, "vr9 Switch");
		dev->init = vr9_switch_init;
		dev->halt = vr9_switch_halt;
		dev->send = vr9_switch_send;
		dev->recv = vr9_switch_recv;

		eth_register(dev);

		return 1;
}

int vr9_switch_init(struct eth_device *dev, bd_t * bis)
{
		int i;
		tx_num=0;
		rx_num=0;

		/* Reset DMA
		 */

    *BSP_DMA_CS=RX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
    /*set descriptor base*/
    *BSP_DMA_CDBA=(u32)rx_des_ring;
    *BSP_DMA_CDLEN=NUM_RX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30000;

    *BSP_DMA_CS=TX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
    *BSP_DMA_CDBA=(u32)tx_des_ring;
    *BSP_DMA_CDLEN=NUM_TX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30100;

	for(i=0;i < NUM_RX_DESC; i++)
	{
		dma_rx_descriptor_t * rx_desc = KSEG1ADDR(&rx_des_ring[i]);
		rx_desc->status.word=0;
		rx_desc->status.field.OWN=1;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;   /* 1536  */
		rx_desc->DataPtr=(u32)NetRxPackets[i] & 0x0ffffff0;
	}

	for(i=0;i < NUM_TX_DESC; i++)
	{
		dma_tx_descriptor_t * tx_desc = KSEG1ADDR(&tx_des_ring[i]);
		memset(tx_desc, 0, sizeof(tx_des_ring[0]));
	}
		/* turn on DMA rx & tx channel
		 */
    *BSP_DMA_CS=RX_CHAN_NO;
    *BSP_DMA_CCTRL|=1;/*reset and turn on the channel*/
	return 0;
}

void vr9_switch_halt(struct eth_device *dev)
{
    int i;
        for(i=0;i<8;i++)
	{
	   *BSP_DMA_CS=i;
	   *BSP_DMA_CCTRL&=~1;/*stop the dma channel*/
	}
}

int vr9_switch_send(struct eth_device *dev, volatile void *packet,int length)
{

	int                 	i;
	int 		 	res = -1;
    int datalen, cache_linesize;
	dma_tx_descriptor_t * tx_desc= KSEG1ADDR(&tx_des_ring[tx_num]);

	if (length <= 0)
	{
		printf ("%s: bad packet size: %d\n", dev->name, length);
		goto Done;
	}

	for(i=0; tx_desc->status.field.OWN==1; i++)
	{
		if(i>=TOUT_LOOP)
		{
			printf("NO Tx Descriptor...");
			goto Done;
		}
	}

	//serial_putc('s');

	tx_desc->status.field.Sop=1;
	tx_desc->status.field.Eop=1;
	tx_desc->status.field.C=0;
	//tx_desc->DataPtr = (u32)KSEG1ADDR(packet);
	tx_desc->DataPtr = (u32)packet & 0x0ffffff0;
        if(length<60)
		tx_desc->status.field.DataLen = 60;
	else
		tx_desc->status.field.DataLen = (u32)length;
        cache_linesize = dcache_linesize_mips32() ;
	datalen = cache_linesize *((tx_desc->status.field.DataLen/cache_linesize)+1);
        dcache_writeback_invalidate(datalen, cache_linesize, (u32)packet );
	asm("SYNC");

	tx_desc->status.field.OWN=1;

	res=length;
	tx_num++;
        if(tx_num==NUM_TX_DESC) tx_num=0;
	*BSP_DMA_CS=TX_CHAN_NO;

	if(!(*BSP_DMA_CCTRL & 1))
	*BSP_DMA_CCTRL|=1;

Done:
	return res;
}

int vr9_switch_recv(struct eth_device *dev)
{

			int length  = 0;
	dma_rx_descriptor_t * rx_desc;
	int datalen, cache_linesize;
	for (;;)
	{
	        rx_desc = KSEG1ADDR(&rx_des_ring[rx_num]);
               

		if ((rx_desc->status.field.C == 0) || (rx_desc->status.field.OWN == 1))
		{
                        //printf("@");
                        break;
                        //continue;
		}

                
		length = rx_desc->status.field.DataLen;
		if (length)
		{
            cache_linesize = dcache_linesize_mips32() ;
	        datalen = cache_linesize *((rx_desc->status.field.DataLen/cache_linesize)+1);
            dcache_hit_invalidate(datalen, cache_linesize, (u32)NetRxPackets[rx_num] );
			NetReceive((void*)KSEG1ADDR(NetRxPackets[rx_num]), length - 4);
			//serial_putc('*');
		}
		else
		{
			printf("Zero length!!!\n");
		}

		rx_desc->status.field.Sop=0;
		rx_desc->status.field.Eop=0;
		rx_desc->status.field.C=0;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;
		rx_desc->status.field.OWN=1;
		rx_num++;
		if(rx_num==NUM_RX_DESC) rx_num=0;

	}
	return length;
}


static void vr9_dma_init(void)
{
    *BSP_PMU_PWDCR &=~(BSP_PMU_DMA);/*enable DMA from PMU*/

    /* Reset DMA
		 */
    *BSP_DMA_CTRL|=1;
    *BSP_DMA_IRNEN=0;/*disable all the interrupts first*/

    /* Clear Interrupt Status Register
	*/
    *BSP_DMA_IRNCR=0xfffff;
    /*disable all the dma interrupts*/
    *BSP_DMA_IRNEN=0;
	/*disable channel 0 and channel 1 interrupts*/
    
    *BSP_DMA_CS=RX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
     /*set descriptor base*/
    *BSP_DMA_CDBA=(u32)rx_des_ring;
    *BSP_DMA_CDLEN=NUM_RX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30000;

    *BSP_DMA_CS=TX_CHAN_NO;
    *BSP_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
    *BSP_DMA_CPOLL= 0x80000040;
    *BSP_DMA_CDBA=(u32)tx_des_ring;
    *BSP_DMA_CDLEN=NUM_TX_DESC;
    *BSP_DMA_CIE = 0;
    *BSP_DMA_CCTRL=0x30100;
     /*enable the poll function and set the poll counter*/
     //*AMAZON_S_DMA_CPOLL=AMAZON_S_DMA_POLL_EN | (AMAZON_S_DMA_POLL_COUNT<<4);
     /*set port properties, enable endian conversion for switch*/
     __asm__("sync");
    *BSP_DMA_PS=0;
    //*BSP_DMA_PCTRL = 0x1f28;
    *BSP_DMA_PCTRL = 0x1f68;/*fix me, should I enable dma drop?*/
	//*BSP_DMA_PCTRL|=0xf<<8;/*enable 32 bit endian conversion*/
    //*BSP_DMA_PCTRL=0x1f14;
    __asm__("sync");
    return;
}

void vr9_sw_gpio_configure(u8 port, u8 mode)
{
    switch(port)
	{
      case 0:
         if ((mode  == MII) || (mode == RMII))
		 {
		    *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0x0303);
		    *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0x0303);
		    *BSP_GPIO_P2_DIR     = (*BSP_GPIO_P2_DIR & ~(0x0303)) | 0x0100;
		    *BSP_GPIO_P2_OD      = *BSP_GPIO_P2_OD | 0x0100;
		 }
	     break;

      case 1:
         if ((mode  == MII) || (mode == RMII))
         {
             *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0xf000);
             *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0xf000);
             *BSP_GPIO_P2_DIR     = (*BSP_GPIO_P2_DIR & ~(0xf000))|0x2000;
             *BSP_GPIO_P2_OD      = *BSP_GPIO_P2_OD | 0x2000;
         }
													 
	     break;
    
      default:
	     break;
	}
}

void config_mdio_gpio(void)
{
	  *BSP_GPIO_P2_ALTSEL0 = *BSP_GPIO_P2_ALTSEL0 | (0xc00);
    *BSP_GPIO_P2_ALTSEL1 = *BSP_GPIO_P2_ALTSEL1 & ~(0xc00);
    *BSP_GPIO_P2_DIR     |= 0xc00;
    *BSP_GPIO_P2_OD      |= 0xc00;
}

void config_port(void)
{
    int i;
		
	  for(i=0;i<7;i++)
	    vr9_port[i].on=0;

#ifdef CONFIG_VR9_SW_PORT_0
    vr9_port[0].on=1;
    vr9_port[0].miimode=CONFIG_VR9_SW_PORT0_MIIMODE;
    vr9_port[0].miirate=CONFIG_VR9_SW_PORT0_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_1
    vr9_port[1].on=1;
		vr9_port[1].miimode=CONFIG_VR9_SW_PORT1_MIIMODE;
		vr9_port[1].miirate=CONFIG_VR9_SW_PORT1_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_2
	  vr9_port[2].on=1;
		vr9_port[2].miimode=CONFIG_VR9_SW_PORT2_MIIMODE;
		vr9_port[2].miirate=CONFIG_VR9_SW_PORT2_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_3
    vr9_port[3].on=1;
		vr9_port[3].miimode=CONFIG_VR9_SW_PORT3_MIIMODE;
		vr9_port[3].miirate=CONFIG_VR9_SW_PORT3_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_4
	  vr9_port[4].on=1;
		vr9_port[4].miimode=CONFIG_VR9_SW_PORT4_MIIMODE;
		vr9_port[4].miirate=CONFIG_VR9_SW_PORT4_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_5a
    vr9_port[5].on=1;
		vr9_port[5].miimode=CONFIG_VR9_SW_PORT5a_MIIMODE;
		vr9_port[5].miirate=CONFIG_VR9_SW_PORT5a_MIIRATE;
#endif
#ifdef CONFIG_VR9_SW_PORT_5b
	  vr9_port[5].on=1;
		vr9_port[5].miimode=CONFIG_VR9_SW_PORT5b_MIIMODE;
		vr9_port[5].miirate=CONFIG_VR9_SW_PORT5b_MIIRATE;
#endif

		#if 0
	  for(i=0;i<7;i++)
	  {
				printf("[%s:%d] vr9_port[%d].on=%d\n",__func__,__LINE__,i,vr9_port[i].on);
				printf("[%s:%d] vr9_port[%d].miimode=0x%x\n",__func__,__LINE__,i,vr9_port[i].miimode);
				printf("[%s:%d] vr9_port[%d].miirate=0x%x\n\n",__func__,__LINE__,i,vr9_port[i].miirate);
		}
		#endif
											  
    return;
}

/***obtain lzma compressed gphy firmware from src address***/
/** return value: 0 success, 1 fail*************************/
int get_gphy_firmware(u32 dst, u32 src)
{
    int result = 1;
    //int srcLen;
	 	u8* load = (unsigned char*)0xa0140000;
	 	ulong unc_len = 0;
	 	u8* image_start = NULL;
	 	ulong image_len = 0;
    image_header_t *pimg_header = NULL;
	 	int fw_version = 1; /*1->version 1; 2->version 2*/
	 	int fw_type = 0; /*0 FE, 1 GE*/
	
     pimg_header = (image_header_t *)src;	
     while(1){ 
	    if(pimg_header->ih_magic != IH_MAGIC){
              printf("bad image magic number\n");
              result = 1;
              break;

	     }
	 switch(pimg_header->ih_type){
	   case IH_TYPE_MULTI:
         image_start = (u8*)pimg_header+sizeof(image_header_t)+8;
	     image_len = pimg_header->ih_size; 

	     if(lzmaBuffToBuffDecompress(load, &unc_len,image_start, image_len)){
			 printf("gphy image name incorrect...\n");
		     result = 1;
		     goto exit;
		     }
		  pimg_header = (image_header_t *)load;  
	    break;
      
	   case IH_TYPE_FIRMWARE:
          if(strncmp(pimg_header->ih_name, "GPHY_FW_PHY11G_A1X", sizeof(pimg_header->ih_name)) == 0){
                  fw_version = 1;
				  fw_type = 1;
		  }else if(strncmp(pimg_header->ih_name, "GPHY_FW_PHY22F_A1X", sizeof(pimg_header->ih_name)) == 0){
                  fw_version = 1;
				  fw_type = 0;
		  }else if(strncmp(pimg_header->ih_name, "GPHY_FW_PHY22F_A2X", sizeof(pimg_header->ih_name)) == 0){
		          fw_version = 2;
		          fw_type = 0;
		  }else if(strncmp(pimg_header->ih_name, "GPHY_FW_PHY11G_A2X", sizeof(pimg_header->ih_name)) == 0){
		          fw_version = 2;
		          fw_type = 1;
		  }
  
		  if(((REG32(BSP_MPS_CHIPID)>>28) & 0x7) == fw_version){ 
#ifdef CONFIG_VR9_SW_PORT2_GMII
               if(fw_type == 1){    
                    image_start = (u8*)pimg_header+sizeof(image_header_t);
					image_len = pimg_header->ih_size;
					memcpy((u8*)dst, image_start, image_len);
					result = 0;
					goto exit;
			   }
#elif CONFIG_VR9_SW_PORT2_MII
               if(fw_type == 0){
                    image_start = (u8*)pimg_header+sizeof(image_header_t);
					image_len = pimg_header->ih_size;
				    memcpy((u8*)dst, image_start, image_len);		
					result = 0;
					goto exit;
			   }
#endif
           }                 
		  pimg_header = (u8*)pimg_header+sizeof(image_header_t)+(pimg_header->ih_size);
	      break;
		 
	  }
	 }
    	
exit:	 
     return result;
}


static void vr9_sw_chip_init(void)
{
    int i,num;
	  u32 fw_addr,gphy_reset_value,fw_src_addr; 
	  //u8 interface_mode;
	 
		#ifdef CONFIG_ENABLE_DCDC
    config_dcdc(0x7f);
		#else // ctc
		REG8(PDI_DCDC_CONF_TEST_ANA) = 0x78;
		REG8(PDI_DCDC_CONF_TEST_DIG) = 0x00;
		REG8(PDI_DCDC_GENERAL) = 0x88;
		#endif
	 	
		#if (CONFIG_VR9_SW_PORT_2 | CONFIG_VR9_SW_PORT_3 | CONFIG_VR9_SW_PORT_4 | CONFIG_VR9_SW_PORT_5b)
		#ifdef CONFIG_VR9_GPHY_DEBUG

    /*GPHY0_UTX*/
    REG32(BSP_GPIO_P0_ALTSEL0)|=1<<13;
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<13;
    REG32(BSP_GPIO_P0_DIR)|=1<<13;
    
    /*GPHY1_UTX, p1.14*/
	  REG32(BSP_GPIO_P1_ALTSEL0)|=1<<14;
	  REG32(BSP_GPIO_P1_ALTSEL1)|=1<<14;
	  REG32(BSP_GPIO_P1_DIR)|=1<<14;
	  REG32(BSP_GPIO_P1_OD)|=1<<14;
				  
	  /*GJTAG_sel*/
	  REG32(BSP_GPIO_P1_ALTSEL0)|=1;
	  REG32(BSP_GPIO_P1_ALTSEL1)|=1;
	  REG32(BSP_GPIO_P1_DIR)&=~1;

    /*TDI p2.1*/
	  REG32(BSP_GPIO_P2_ALTSEL0) &=~1;
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1;
	  REG32(BSP_GPIO_P2_DIR)&=~1;

	  /*TDO, gpio33, P2.1*/
	  REG32(BSP_GPIO_P2_ALTSEL0) &=~(1<<1);
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1<<1;
	  REG32(BSP_GPIO_P2_DIR) |=1<<1;
	  REG32(BSP_GPIO_P2_OD) &=~(1<<1); /*to be checked, affecting debugging speed*/

	  /*TMS, p2.9*/
    REG32(BSP_GPIO_P2_ALTSEL0) |=1<<9;
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1<<9;
	  REG32(BSP_GPIO_P2_DIR) &=~(1<<9);
				  
	  /*TCK, p2.8*/
	  REG32(BSP_GPIO_P2_ALTSEL0) |=1<<8;
	  REG32(BSP_GPIO_P2_ALTSEL1)|=1<<8;
	  REG32(BSP_GPIO_P2_DIR) &=~(1<<8);
	  
		#if 0
	  /*Configure LED for GPHY*/
	  /*gpio2, p0.2, gphy led 2*/
	  REG32(BSP_GPIO_P0_ALTSEL0)|=1<<2;
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<2;
    REG32(BSP_GPIO_P0_DIR)|=1<<2;
    REG32(BSP_GPIO_P0_DIR)|=1<<2;
	  
	  /*gpio5, p0.5, gph0 led0*/
	  REG32(BSP_GPIO_P0_ALTSEL0)&=~(1<<5);
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<5;
    REG32(BSP_GPIO_P0_DIR)|=1<<5;
    REG32(BSP_GPIO_P0_DIR)|=1<<5;
    
    /*gpio7, p0.7, gph0 led1*/
	  REG32(BSP_GPIO_P0_ALTSEL0)|=1<<7;
    REG32(BSP_GPIO_P0_ALTSEL1)|=1<<7;
    REG32(BSP_GPIO_P0_DIR)|=1<<7;
    REG32(BSP_GPIO_P0_DIR)|=1<<7;
    
    /*gpio44, p2.12, gph1 led0*/
	  REG32(BSP_GPIO_P2_ALTSEL0)|=1<<12;
    REG32(BSP_GPIO_P2_ALTSEL1)|=1<<12;
    REG32(BSP_GPIO_P2_DIR)|=1<<12;
    REG32(BSP_GPIO_P2_DIR)|=1<<12;
    
    /*gpio 45, p2.13,gph1 led1*/
    REG32(BSP_GPIO_P2_ALTSEL0)&=~(1<<13);
    REG32(BSP_GPIO_P2_ALTSEL1)|=1<<13;
    REG32(BSP_GPIO_P2_DIR)|=1<<13;
    REG32(BSP_GPIO_P2_DIR)|=1<<13;
    
    /*gpio 47, p2.15, gphy1 led2*/
    REG32(BSP_GPIO_P2_ALTSEL0)&=~(1<<15);
    REG32(BSP_GPIO_P2_ALTSEL1)|=1<<15;
    REG32(BSP_GPIO_P2_DIR)|=1<<15;
    REG32(BSP_GPIO_P2_DIR)|=1<<15;
		#endif  // #if 0
		#endif

		#ifdef CONFIG_GPHY_LED_GPIO
    /*GPIO 5 , 7 , 44 , 45 config as GPHY LED , and GPIO7 is CFRAME pin*/ 
    *BSP_GPIO_P0_ALTSEL0 &= (~0x20);
    *BSP_GPIO_P0_ALTSEL1 |= (0x20);
    *BSP_GPIO_P0_DIR |= (0x20);
    *BSP_GPIO_P0_OD |= (0x20);
    *BSP_GPIO_P0_ALTSEL0 |=0x80;
    *BSP_GPIO_P0_ALTSEL1 |=0x80;
    *BSP_GPIO_P0_DIR |= (0x80);
    *BSP_GPIO_P0_DIR |= (0x80);

    *BSP_GPIO_P2_ALTSEL0 |= (0x1000);
    *BSP_GPIO_P2_ALTSEL1 |= (0x1000);
    *BSP_GPIO_P2_DIR |= (0x1000);
    *BSP_GPIO_P2_OD |= (0x1000);
    *BSP_GPIO_P2_ALTSEL0 &=(~0x2000);
    *BSP_GPIO_P2_ALTSEL1 |=0x2000;
    *BSP_GPIO_P2_DIR |= (0x2000);
    *BSP_GPIO_P2_DIR |= (0x2000);  
		#else
  	#ifdef CONFIG_GPHY_SHIFT_REGISTER /* ctc */
	  /*enable led for gphy link status*/
	  *(BSP_GPIO_P0_DIR) = (*BSP_GPIO_P0_DIR)|(0x070);
	  *(BSP_GPIO_P0_ALTSEL0) = ((*BSP_GPIO_P0_ALTSEL0)|(0x70));
	  *(BSP_GPIO_P0_ALTSEL1) = (*BSP_GPIO_P0_ALTSEL1)&(~0x70);
	  *BSP_GPIO_P0_OD = (*BSP_GPIO_P0_OD)|0x70;
							  
    *BSP_LED_CON1 = 0x81a38007; // all LEDs enable, software update mode, FPI clock for shifting, no delay
	  *BSP_LED_CON0 = 0x3f000000; // software update
  	#endif
		#endif					 

	  asm("sync");
		
    /*turn on ephy, switch and dma in PMU*/
    *BSP_PMU_PWDCR = 0x061592be;
    mdelay(100);

    *BSP_RCU_RST_REQ = 0xa0200300;
	  mdelay(100);
	  
	  /*Turn on switch macro*/  
    REG32(VR9_ETHSW_GLOB_CTRL) |= (1<<15);
    
		#if defined(CONFIG_VR9_CRYSTAL_6M) || defined(CONFIG_VR9_CRYSTAL_CPLD)
	  /*Config GPIO3 clock,CLK_OUT2 used as input*/
	  REG32(BSP_GPIO_P0_ALTSEL0)|=1<<3;
	  REG32(BSP_GPIO_P0_ALTSEL1)&=~(1<<3);
	  REG32(BSP_GPIO_P0_DIR)&=~(1<<3);
	  
	  /*Config GPHY clock mux to take clock from GPIO3*/
	  REG32(BSP_IF_CLK)=0x16e00010;
		#elif defined(CONFIG_VR9_CRYSTAL_25M)
    REG32(BSP_IF_CLK)=0x16e00006;
		#else /*36Mhz crystal clock*/
    REG32(BSP_IF_CLK)=0x16e00008;   
		#endif
    
    REG32(BSP_GPHY1_Cfg)=0x1fe70000;
    REG32(BSP_GPHY0_Cfg)=0x1fe70000;/*gphy0 config for 25Mhz input clock only*/

		mdelay(1);

		asm("sync");
	  fw_addr=0xa0200000;
		#ifdef CONFIG_VR9_GPHY_FW_EMBEDDED	  
    fw_src_addr = CONFIG_VR9_GPHY_FW_ADDR;
	  //memcpy(fw_addr, CONFIG_VR9_GPHY_FW_ADDR, 65536); 
		#endif	 

		#ifdef CONFIG_VR9_EXTERN_GPHY_FW_ADDR
 		#ifdef CONFIG_BOOT_FROM_NOR
    fw_src_addr = CONFIG_VR9_EXTERN_GPHY_FW_ADDR;
 		#elif defined(CONFIG_BOOT_FROM_NAND)
    {
	   		nand_info_t *nand = &nand_info[0];
	   		u64 size=65536;
	   		fw_src_addr = 0xa0120000;
       	nand_read_skip_bad(nand, CONFIG_VR9_EXTERN_GPHY_FW_ADDR, &size,(u_char *)fw_src_addr);
    }
 		#elif defined(CONFIG_BOOT_FROM_SPI)
    {
        static struct spi_flash *flash;
				fw_src_addr = 0xa0110000;
	    	flash = spi_flash_probe(0, CONFIG_ENV_SPI_CS,CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
	    	spi_flash_read(flash, CONFIG_VR9_EXTERN_GPHY_FW_ADDR, 65536, (void*)fw_src_addr);					 
	  }
 		#endif 
		#endif

    if(get_gphy_firmware(fw_addr,fw_src_addr)){
           printf("cannnot find gphy firmware!\n");
    }
					 
	  mdelay(100);

		/* step - 1 */
	  /* program fw base address */
	  REG32(0xBF203020) = 0x200000;  // address inside DDR1/2
	  REG32(0xBF203068) = 0x200000;  // address inside DDR1/2
    REG32(0xBF203010) = 0xa0000021;
	  mdelay(100);

		/* step - 2 */
	  // Bring GPHY-0/GFS-0 GPHY-1/GFS-1 out of reset
	  //REG32(0xBF203010) = 0x0;
		#ifdef CONFIG_POWER_DOWN_REGULATOR	 
    *BSP_RCU_RST_REQ = 0x04000000;
		#else
    *BSP_RCU_RST_REQ = 0x0;
		#endif	  
	  mdelay(100); /*wait 200ms*/

		REG32(BSP_SWITCH_PMAC_RX_IPG) = 0x8b;
		#endif

	  /*Enable PHY polling for each port*/		
	  REG32(VR9_ETHSW_MDC_CFG_0)=0x3f;
	  /*2.5 MHz MDIO clock*/
		#if defined(CONFIG_VR9_CRYSTAL_6M) || defined(CONFIG_VR9_CRYSTAL_CPLD)
    REG32(VR9_ETHSW_MDC_CFG_1)=0x00008102;
		#else
    REG32(VR9_ETHSW_MDC_CFG_1)=0x00008105;
		#endif
		udelay(100);
		
/**************************************************/	  
/***********configure phy now**********************/
/**************************************************/
		asm("sync");

	 	/*config mdio gpio for external gphy only*/ 
	  config_mdio_gpio();

	 	/*Configure port 2,4 phy address and auto-polling mode now*/
	  REG32(VR9_ETHSW_PHY_ADDR_2) = 0x1811;
    REG32(VR9_ETHSW_PHY_ADDR_3) = 0x1812;
    REG32(VR9_ETHSW_PHY_ADDR_4) = 0x1813;
		#ifdef CONFIG_VR9_SW_PORT_5b    
    REG32(VR9_ETHSW_PHY_ADDR_5) = 0x1814;  
		#elif CONFIG_VR9_SW_PORT_5a
    REG32(VR9_ETHSW_PHY_ADDR_5) = 0x1805;  
		#endif  
     
		if(((REG32(BSP_MPS_CHIPID)>>28) & 0x7) ==0x1)
		{
				/* v1 */
				/*****disable "clear on read" ability for link status bits**/
		    vr9_mdio_write(0x11,0xd,0x1F);
		    vr9_mdio_write(0x11,0xe,0x1FF);
		    vr9_mdio_write(0x11,0xd,0x401F);
		    vr9_mdio_write(0x11,0xe,0x1);
		    vr9_mdio_write(0x11,0x14,0x8106);
		    vr9_mdio_write(0x11,0x0,0x8000);

		    vr9_mdio_write(0x13,0xd,0x1F);
		    vr9_mdio_write(0x13,0xe,0x1FF);
		    vr9_mdio_write(0x13,0xd,0x401F);
		    vr9_mdio_write(0x13,0xe,0x1);
		    vr9_mdio_write(0x13,0x14,0x8106);
		    vr9_mdio_write(0x13,0x0,0x8000);

		    vr9_mdio_write(0x0,0xd,0x1F);
		    vr9_mdio_write(0x0,0xe,0x1FF);
		    vr9_mdio_write(0x0,0xd,0x401F);
		    vr9_mdio_write(0x0,0xe,0x1);
		    vr9_mdio_write(0x0,0x14,0x8106);
		    vr9_mdio_write(0x0,0x0,0x8000);

		    vr9_mdio_write(0x1,0xd,0x1F);
		    vr9_mdio_write(0x1,0xe,0x1FF);
		    vr9_mdio_write(0x1,0xd,0x401F);
		    vr9_mdio_write(0x1,0xe,0x1);
		    vr9_mdio_write(0x1,0x14,0x8106);
		    vr9_mdio_write(0x1,0x0,0x8000);

		    vr9_mdio_write(0x5,0xd,0x1F);
				vr9_mdio_write(0x5,0xe,0x1FF);
				vr9_mdio_write(0x5,0xd,0x401F);
				vr9_mdio_write(0x5,0xe,0x1);
				vr9_mdio_write(0x5,0x14,0x8106);
				vr9_mdio_write(0x5,0x0,0x8000);
		}

		/***enable sticky bit handling for external phy*********/
    vr9_mdio_write(0x0,0x14,0x8102);
    vr9_mdio_write(0x1,0x14,0x8102);
    vr9_mdio_write(0x5,0x14,0x8102);

		/**advertise on half and full duplex mode, multi device*/
    vr9_mdio_write(0x0,0x9,0x700);
    vr9_mdio_write(0x1,0x9,0x700);
    vr9_mdio_write(0x5,0x9,0x700);
    vr9_mdio_write(0x11,0x9,0x700);
    vr9_mdio_write(0x13,0x9,0x700);
/**************************************/
/*end of phy configuration*************/
/**************************************/

/*************************************/
/***mac configuration now*************/
/*************************************/
	  config_port();
  
	  for(num=0;num<7;num++)
		{
        if(vr9_port[num].on)
				{
        		//printf("port%d init!\n",num);
				    REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= (1<<VR9_ETHSW_MII_CFG_RES);  /*Hardware reset */
		        for (i=0; i<100;i++)
		          udelay(10);

		        /* set xMII port interface clock rate */
		        REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) &= ~(0x7<<VR9_ETHSW_MII_CFG_MIIRATE);
		        REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= ((vr9_port[num].miirate & 0x7)<<VR9_ETHSW_MII_CFG_MIIRATE);
																	 
		        /*xMII  interface mode */
		        //interface_mode = 4;
			    	REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) &= ~(0xf<<VR9_ETHSW_MII_CFG_MIIMODE);
			    	REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= ((vr9_port[num].miimode & 0xf)<<VR9_ETHSW_MII_CFG_MIIMODE);
								  
		        /*xMII  interface enable*/
			    	REG32(VR9_ETHSW_MII_CFG_0 + ( num * 8)) |= (1<<VR9_ETHSW_MII_CFG_EN);
			    
		        REG16(VR9_ETHSW_MAC_CTRL_0 + ( num * 0x30))= 0x180; /*auto MII/GMII interface selection, enable padding*/
		        
		        REG16(VR9_ETHSW_MAC_CTRL_2 + ( num * 0x30))= 0x0d; /*JUMBO configured value, including any number of VLAN tags*/
       	}
    }

		asm("sync");  

		// Apply workaround for buffer congestion
		REG32(VR9_ETHSW_MAC_CTRL_1 + ( 6* 0x30)) |= ((1<< 0x8)); // Short Preamble
		REG32(VR9_ETHSW_MAC_CTRL_1 + ( 6* 0x30)) &= ~((0xF)); // Setting TX IPG
		REG32(VR9_ETHSW_MAC_CTRL_1 + ( 6* 0x30)) |= ((0xC));  // to 7 bytes
		REG32(VR9_ETHSW_MAC_CTRL_6 + ( 6* 0x30)) |= (1 << 6); // Setting RX Buffer to bypass

		// Limiting global threshholds to 254 to avoid systematical concept weakness ACM bug
		REG32(VR9_ETHSW_BM_FSQM_GCTRL) &= (0xFD);
		REG32(VR9_ETHSW_BM_GCTRL) |= (0x01);
		REG32(VR9_ETHSW_BM_GCTRL) &= (0xFFFE);
	  #if 1 // ctc
		REG32(VR9_ETHSW_BM_QUEUE_GCTRL) = (0x0518);
	  #endif

    /*Fix switch MAC drop bug*/ 
    REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_KEY_0) = 0xf;
		REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_VAL_0) = 0x40;
    REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_ADDR)  = 0x3;
		REG32(ADR_ETHSW_PCE_PDI_PCE_TBL_CTRL)  = 0x902f;
   
		REG32(0xBE10a530) = 0x107;
		REG32(0xBE10a544) |= 0x40;
		
		REG32(VR9_ETHSW_PMAC_HD_CTL) |= (0x1 << 2);	

    /*To avoid the packet drop issue, please set the TXDLY bits of PCDU0, 1 , 5 to 1.*/
    REG32(VR9_ETHSW_PCDU_0)=(REG32(VR9_ETHSW_PCDU_0) & (~0x7)) | 0x1;
    REG32(VR9_ETHSW_PCDU_1)=(REG32(VR9_ETHSW_PCDU_1) & (~0x7)) | 0x1;
	  REG32(VR9_ETHSW_PCDU_5)=(REG32(VR9_ETHSW_PCDU_5) & (~0x7)) | 0x1;
	  
	  /*Switch MACBAT WorkAround*/
	  /*Set PCE_TBL_KEY_0 (0x447) to 0xF*/
    REG32(0xBE10911C) = 0xF;

    /*Set PCE_TBL_VAL_0 (0x44D) to 0x40*/
    REG32(0xBE109134) = 0x40;

    /*Set PCE_TBL_ADDR (0x44E) to 0x3*/
    REG32(0xBE109138) = 0x3;

    /*Set PCE_TBL_CTRL (0x44F) to 0x902F*/
    REG32(0xBE10913C) = 0x902F;

	  REG32(0xbf203048) = 0xc0;
	  mdelay(200);
	  REG32(0xbf203048) = 0x0;
	  mdelay(200);

    asm("sync");  

		#if (CONFIG_VR9_SW_PORT_2 | CONFIG_VR9_SW_PORT_3 | CONFIG_VR9_SW_PORT_4 | CONFIG_VR9_SW_PORT_5b)
  	#ifdef CONFIG_VR9_SW_PORT2_MII
    printf("Internal phy(FE) firmware version: 0x%04x\n",vr9_mdio_read(0x11, 0x1e));
  	#else
    printf("Internal phy(GE) firmware version: 0x%04x\n",vr9_mdio_read(0x11, 0x1e));
  	#endif
		#endif	

		vr9_dma_init();
	  
  	#ifdef CONFIG_DRIVER_VR9 /* ctc for RTL8367RB */
		REG32(VR9_ETHSW_PHY_ADDR_0) = (REG32(VR9_ETHSW_PHY_ADDR_0) & (~0x7E00)) | 0x3200;
		REG32(VR9_ETHSW_MAC_CTRL_0) = (REG32(VR9_ETHSW_MAC_CTRL_0) & (~0x0003)) | 0x0002;
		REG32(VR9_ETHSW_MII_CFG_0) = (REG32(VR9_ETHSW_MII_CFG_0) & (~0x007F)) | 0x0044;
		REG32(VR9_ETHSW_PCDU_0) = (REG32(VR9_ETHSW_PCDU_0) & (~0x0387)) | 0x0003;
  	#endif

   
		#if 0  /* Connect with RTL8367RB interface */
		printf("[%s:%d]VR9_ETHSW_PHY_ADDR_0=0x%x\n\n",__func__,__LINE__,REG32(VR9_ETHSW_PHY_ADDR_0));
		printf("[%s:%d]VR9_ETHSW_MAC_CTRL_0=0x%x\n",__func__,__LINE__,REG32(VR9_ETHSW_MAC_CTRL_0));

		printf("[%s:%d]VR9_ETHSW_MII_CFG_0=0x%x\n",__func__,__LINE__,REG32(VR9_ETHSW_MII_CFG_0));
		printf("[%s:%d]VR9_ETHSW_PCDU_0=0x%x\n",__func__,__LINE__,REG32(VR9_ETHSW_PCDU_0));
		printf("[%s:%d]ADR_ETHSW_MII_PDI_PCDU_0=0x%x\n",__func__,__LINE__,REG32(ADR_ETHSW_MII_PDI_PCDU_0));
		#endif

}


#ifdef CONFIG_DRIVER_VR9 /* ctc for RTL8367RB */

//#define RTK_DBG

typedef unsigned long long		rtk_uint64;
typedef long long							rtk_int64;
typedef unsigned int					rtk_uint32;
typedef int										rtk_int32;
typedef unsigned short				rtk_uint16;
typedef short									rtk_int16;
typedef unsigned char					rtk_uint8;
typedef char									rtk_int8;

typedef rtk_int32             rtk_api_ret_t;
typedef rtk_int32             ret_t;
typedef rtk_uint64            rtk_u_long_t;

typedef rtk_uint32  					rtk_data_t;
typedef rtk_uint32  					rtk_port_t;        /* port is type */

#ifdef __KERNEL__
#define rtlglue_printf 	printk
#else
#define rtlglue_printf 	printf
#endif
#define PRINT						rtlglue_printf

#ifdef NULL
#undef NULL
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define NULL 0
#define TRUE 1
#define FALSE 0

#define CONST			const

#define RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST    1


typedef enum rtk_l2_lookup_type_e
{
    LOOKUP_MAC = 0,
    LOOKUP_SIP_DIP,
    LOOKUP_DIP,
    LOOKUP_END
} rtk_l2_lookup_type_t;

typedef struct rtk_portmask_s
{
    rtk_uint32  bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST];
} rtk_portmask_t;

typedef enum rtk_ext_port_e
{
    EXT_PORT_0 = 0,
    EXT_PORT_1,
    EXT_PORT_2,
    EXT_PORT_END
}rtk_ext_port_t;

typedef enum rtk_mode_ext_e
{
    MODE_EXT_DISABLE = 0,
    MODE_EXT_RGMII,
    MODE_EXT_MII_MAC,
    MODE_EXT_MII_PHY,
    MODE_EXT_TMII_MAC,
    MODE_EXT_TMII_PHY,
    MODE_EXT_GMII,
    MODE_EXT_RMII_MAC,
    MODE_EXT_RMII_PHY,
    MODE_EXT_RGMII_33V,
    MODE_EXT_END
} rtk_mode_ext_t;

enum EXTMODE
{
    EXT_DISABLE = 0,
    EXT_RGMII,
    EXT_MII_MAC,
    EXT_MII_PHY,
    EXT_TMII_MAC,
    EXT_TMII_PHY,
    EXT_GMII,
    EXT_RMII_MAC,
    EXT_RMII_PHY,
    EXT_END
};

typedef struct  rtl8367b_port_ability_s{
#ifdef _LITTLE_ENDIAN
    rtk_uint16 speed:2;
    rtk_uint16 duplex:1;
    rtk_uint16 reserve1:1;
    rtk_uint16 link:1;
    rtk_uint16 rxpause:1;
    rtk_uint16 txpause:1;
    rtk_uint16 nway:1;
    rtk_uint16 mstmode:1;
    rtk_uint16 mstfault:1;
    rtk_uint16 reserve2:2;
    rtk_uint16 forcemode:1;
    rtk_uint16 reserve3:3;
#else
    rtk_uint16 reserve3:3;
    rtk_uint16 forcemode:1;
    rtk_uint16 reserve2:2;
    rtk_uint16 mstfault:1;
    rtk_uint16 mstmode:1;
    rtk_uint16 nway:1;
    rtk_uint16 txpause:1;
    rtk_uint16 rxpause:1;
    rtk_uint16 link:1;
    rtk_uint16 reserve1:1;
    rtk_uint16 duplex:1;
    rtk_uint16 speed:2;

#endif
}rtl8367b_port_ability_t;

typedef struct  rtk_port_mac_ability_s
{
    rtk_uint32 forcemode;
    rtk_uint32 speed;
    rtk_uint32 duplex;
    rtk_uint32 link;
    rtk_uint32 nway;
    rtk_uint32 txpause;
    rtk_uint32 rxpause;
}rtk_port_mac_ability_t;


#define MDC_MDIO_OPERATION
#define CHIP_RTL8367RB

#define RTK_MAX_NUM_OF_PORT         8
#define RTK_PORT_ID_MAX            	(RTK_MAX_NUM_OF_PORT-1)
#define RTK_PHY_ID_MAX              (RTK_MAX_NUM_OF_PORT-4)

#define MDC_MDIO_DUMMY_ID           0
#define MDC_MDIO_CTRL0_REG          31
#define MDC_MDIO_CTRL1_REG          21
#define MDC_MDIO_ADDRESS_REG        23
#define MDC_MDIO_DATA_WRITE_REG     24
#define MDC_MDIO_DATA_READ_REG      25
#define MDC_MDIO_START_REG          29
#define MDC_MDIO_PREAMBLE_LEN       32

#define MDC_MDIO_START_OP          	0xFFFF
#define MDC_MDIO_ADDR_OP            0x000E
#define MDC_MDIO_READ_OP            0x0001
#define MDC_MDIO_WRITE_OP           0x0003

#define MDC_MDIO_WRITE( preLen, phy, reg, data )		vr9_mdio_write( phy, reg, data )
#define MDC_MDIO_READ(  preLen, phy, reg, pdata )		do { *(pdata)=vr9_mdio_read( phy, reg ); } while (0)

#if defined(CONFIG_RTL865X_CLE) || defined (RTK_X86_CLE)
rtk_uint32 cleDebuggingDisplay;
#endif

rtk_uint16 (*init_para)[2];
rtk_uint16 init_size;

#define RTL8367B_REGDATAMAX        	0xFFFF

#define RTL8367B_PHY_INTERNALNOMAX	0x4
#define RTL8367B_PHY_REGNOMAX		    0x1F
#define RTL8367B_REGBITLENGTH       16
#define RTL8367B_EXTNO       				3

#define RTL8367B_PORTMASK           0xFF
#define RTK_MAX_PORT_MASK           0xFF

rtk_port_mac_ability_t rtl8367_ext_port_cfg[EXT_PORT_END] =
{
		/* EXT_PORT_0 */
		{0,0,0,0,0,0,0},
		/* EXT_PORT_1 */
		{1,2,1,1,0,0,0},
		/* EXT_PORT_2 */
		{0,0,0,0,0,0,0}
};

#if defined(CHIP_RTL8367RB)
u16 ChipData30[][2]= {
	/*Code of Func*/
	{0x1B03, 0x0876}, {0x1200, 0x7FC4}, {0x0301, 0x0026}, {0x1722, 0x0E14},
	{0x205F, 0x0002}, {0x2059, 0x1A00}, {0x205F, 0x0000}, {0x207F, 0x0002},
	{0x2077, 0x0000}, {0x2078, 0x0000}, {0x2079, 0x0000}, {0x207A, 0x0000},
	{0x207B, 0x0000}, {0x207F, 0x0000}, {0x205F, 0x0002}, {0x2053, 0x0000},
	{0x2054, 0x0000}, {0x2055, 0x0000}, {0x2056, 0x0000}, {0x2057, 0x0000},
	{0x205F, 0x0000}, {0x12A4, 0x110A}, {0x12A6, 0x150A}, {0x13F1, 0x0013},
	{0x13F4, 0x0010}, {0x13F5, 0x0000}, {0x0018, 0x0F00}, {0x0038, 0x0F00},
	{0x0058, 0x0F00}, {0x0078, 0x0F00}, {0x0098, 0x0F00}, {0x12B6, 0x0C02},
	{0x12B7, 0x030F}, {0x12B8, 0x11FF}, {0x12BC, 0x0004}, {0x1362, 0x0115},
	{0x1363, 0x0002}, {0x1363, 0x0000}, {0x133F, 0x0030}, {0x133E, 0x000E},
	{0x221F, 0x0007}, {0x221E, 0x002D}, {0x2218, 0xF030}, {0x221F, 0x0007},
	{0x221E, 0x0023}, {0x2216, 0x0005}, {0x2215, 0x00B9}, {0x2219, 0x0044},
	{0x2215, 0x00BA}, {0x2219, 0x0020}, {0x2215, 0x00BB}, {0x2219, 0x00C1},
	{0x2215, 0x0148}, {0x2219, 0x0096}, {0x2215, 0x016E}, {0x2219, 0x0026},
	{0x2216, 0x0000}, {0x2216, 0x0000}, {0x221E, 0x002D}, {0x2218, 0xF010},
	{0x221F, 0x0007}, {0x221E, 0x0020}, {0x2215, 0x0D00}, {0x221F, 0x0000},
	{0x221F, 0x0000}, {0x2217, 0x2160}, {0x221F, 0x0001}, {0x2210, 0xF25E},
	{0x221F, 0x0007}, {0x221E, 0x0042}, {0x2215, 0x0F00}, {0x2215, 0x0F00},
	{0x2216, 0x7408}, {0x2215, 0x0E00}, {0x2215, 0x0F00}, {0x2215, 0x0F01},
	{0x2216, 0x4000}, {0x2215, 0x0E01}, {0x2215, 0x0F01}, {0x2215, 0x0F02},
	{0x2216, 0x9400}, {0x2215, 0x0E02}, {0x2215, 0x0F02}, {0x2215, 0x0F03},
	{0x2216, 0x7408}, {0x2215, 0x0E03}, {0x2215, 0x0F03}, {0x2215, 0x0F04},
	{0x2216, 0x4008}, {0x2215, 0x0E04}, {0x2215, 0x0F04}, {0x2215, 0x0F05},
	{0x2216, 0x9400}, {0x2215, 0x0E05}, {0x2215, 0x0F05}, {0x2215, 0x0F06},
	{0x2216, 0x0803}, {0x2215, 0x0E06}, {0x2215, 0x0F06}, {0x2215, 0x0D00},
	{0x2215, 0x0100}, {0x221F, 0x0001}, {0x2210, 0xF05E}, {0x221F, 0x0000},
	{0x2217, 0x2100}, {0x221F, 0x0000}, {0x220D, 0x0003}, {0x220E, 0x0015},
	{0x220D, 0x4003}, {0x220E, 0x0006}, {0x221F, 0x0000}, {0x2200, 0x1340},
	{0x133F, 0x0010}, {0x12A0, 0x0058}, {0x12A1, 0x0058}, {0x133E, 0x000E},
	{0x133F, 0x0030}, {0x221F, 0x0000}, {0x2210, 0x0166}, {0x221F, 0x0000},
	{0x133E, 0x000E}, {0x133F, 0x0010}, {0x133F, 0x0030}, {0x133E, 0x000E},
	{0x221F, 0x0005}, {0x2205, 0xFFF6}, {0x2206, 0x0080}, {0x2205, 0x8B6E},
	{0x2206, 0x0000}, {0x220F, 0x0100}, {0x2205, 0x8000}, {0x2206, 0x0280},
	{0x2206, 0x28F7}, {0x2206, 0x00E0}, {0x2206, 0xFFF7}, {0x2206, 0xA080},
	{0x2206, 0x02AE}, {0x2206, 0xF602}, {0x2206, 0x0153}, {0x2206, 0x0201},
	{0x2206, 0x6602}, {0x2206, 0x80B9}, {0x2206, 0xE08B}, {0x2206, 0x8CE1},
	{0x2206, 0x8B8D}, {0x2206, 0x1E01}, {0x2206, 0xE18B}, {0x2206, 0x8E1E},
	{0x2206, 0x01A0}, {0x2206, 0x00E7}, {0x2206, 0xAEDB}, {0x2206, 0xEEE0},
	{0x2206, 0x120E}, {0x2206, 0xEEE0}, {0x2206, 0x1300}, {0x2206, 0xEEE0},
	{0x2206, 0x2001}, {0x2206, 0xEEE0}, {0x2206, 0x2166}, {0x2206, 0xEEE0},
	{0x2206, 0xC463}, {0x2206, 0xEEE0}, {0x2206, 0xC5E8}, {0x2206, 0xEEE0},
	{0x2206, 0xC699}, {0x2206, 0xEEE0}, {0x2206, 0xC7C2}, {0x2206, 0xEEE0},
	{0x2206, 0xC801}, {0x2206, 0xEEE0}, {0x2206, 0xC913}, {0x2206, 0xEEE0},
	{0x2206, 0xCA30}, {0x2206, 0xEEE0}, {0x2206, 0xCB3E}, {0x2206, 0xEEE0},
	{0x2206, 0xDCE1}, {0x2206, 0xEEE0}, {0x2206, 0xDD00}, {0x2206, 0xEEE2},
	{0x2206, 0x0001}, {0x2206, 0xEEE2}, {0x2206, 0x0100}, {0x2206, 0xEEE4},
	{0x2206, 0x8860}, {0x2206, 0xEEE4}, {0x2206, 0x8902}, {0x2206, 0xEEE4},
	{0x2206, 0x8C00}, {0x2206, 0xEEE4}, {0x2206, 0x8D30}, {0x2206, 0xEEEA},
	{0x2206, 0x1480}, {0x2206, 0xEEEA}, {0x2206, 0x1503}, {0x2206, 0xEEEA},
	{0x2206, 0xC600}, {0x2206, 0xEEEA}, {0x2206, 0xC706}, {0x2206, 0xEE85},
	{0x2206, 0xEE00}, {0x2206, 0xEE85}, {0x2206, 0xEF00}, {0x2206, 0xEE8B},
	{0x2206, 0x6750}, {0x2206, 0xEE8B}, {0x2206, 0x6632}, {0x2206, 0xEE8A},
	{0x2206, 0xD448}, {0x2206, 0xEE8A}, {0x2206, 0xD548}, {0x2206, 0xEE8A},
	{0x2206, 0xD649}, {0x2206, 0xEE8A}, {0x2206, 0xD7F8}, {0x2206, 0xEE8B},
	{0x2206, 0x85E2}, {0x2206, 0xEE8B}, {0x2206, 0x8700}, {0x2206, 0xEEFF},
	{0x2206, 0xF600}, {0x2206, 0xEEFF}, {0x2206, 0xF7FC}, {0x2206, 0x04F8},
	{0x2206, 0xE08B}, {0x2206, 0x8EAD}, {0x2206, 0x2023}, {0x2206, 0xF620},
	{0x2206, 0xE48B}, {0x2206, 0x8E02}, {0x2206, 0x2877}, {0x2206, 0x0225},
	{0x2206, 0xC702}, {0x2206, 0x26A1}, {0x2206, 0x0281}, {0x2206, 0xB302},
	{0x2206, 0x8496}, {0x2206, 0x0202}, {0x2206, 0xA102}, {0x2206, 0x27F1},
	{0x2206, 0x0228}, {0x2206, 0xF902}, {0x2206, 0x2AA0}, {0x2206, 0x0282},
	{0x2206, 0xB8E0}, {0x2206, 0x8B8E}, {0x2206, 0xAD21}, {0x2206, 0x08F6},
	{0x2206, 0x21E4}, {0x2206, 0x8B8E}, {0x2206, 0x0202}, {0x2206, 0x80E0},
	{0x2206, 0x8B8E}, {0x2206, 0xAD22}, {0x2206, 0x05F6}, {0x2206, 0x22E4},
	{0x2206, 0x8B8E}, {0x2206, 0xE08B}, {0x2206, 0x8EAD}, {0x2206, 0x2305},
	{0x2206, 0xF623}, {0x2206, 0xE48B}, {0x2206, 0x8EE0}, {0x2206, 0x8B8E},
	{0x2206, 0xAD24}, {0x2206, 0x08F6}, {0x2206, 0x24E4}, {0x2206, 0x8B8E},
	{0x2206, 0x0227}, {0x2206, 0x6AE0}, {0x2206, 0x8B8E}, {0x2206, 0xAD25},
	{0x2206, 0x05F6}, {0x2206, 0x25E4}, {0x2206, 0x8B8E}, {0x2206, 0xE08B},
	{0x2206, 0x8EAD}, {0x2206, 0x260B}, {0x2206, 0xF626}, {0x2206, 0xE48B},
	{0x2206, 0x8E02}, {0x2206, 0x830D}, {0x2206, 0x021D}, {0x2206, 0x6BE0},
	{0x2206, 0x8B8E}, {0x2206, 0xAD27}, {0x2206, 0x05F6}, {0x2206, 0x27E4},
	{0x2206, 0x8B8E}, {0x2206, 0x0281}, {0x2206, 0x4402}, {0x2206, 0x045C},
	{0x2206, 0xFC04}, {0x2206, 0xF8E0}, {0x2206, 0x8B83}, {0x2206, 0xAD23},
	{0x2206, 0x30E0}, {0x2206, 0xE022}, {0x2206, 0xE1E0}, {0x2206, 0x2359},
	{0x2206, 0x02E0}, {0x2206, 0x85EF}, {0x2206, 0xE585}, {0x2206, 0xEFAC},
	{0x2206, 0x2907}, {0x2206, 0x1F01}, {0x2206, 0x9E51}, {0x2206, 0xAD29},
	{0x2206, 0x20E0}, {0x2206, 0x8B83}, {0x2206, 0xAD21}, {0x2206, 0x06E1},
	{0x2206, 0x8B84}, {0x2206, 0xAD28}, {0x2206, 0x42E0}, {0x2206, 0x8B85},
	{0x2206, 0xAD21}, {0x2206, 0x06E1}, {0x2206, 0x8B84}, {0x2206, 0xAD29},
	{0x2206, 0x36BF}, {0x2206, 0x34BF}, {0x2206, 0x022C}, {0x2206, 0x31AE},
	{0x2206, 0x2EE0}, {0x2206, 0x8B83}, {0x2206, 0xAD21}, {0x2206, 0x10E0},
	{0x2206, 0x8B84}, {0x2206, 0xF620}, {0x2206, 0xE48B}, {0x2206, 0x84EE},
	{0x2206, 0x8ADA}, {0x2206, 0x00EE}, {0x2206, 0x8ADB}, {0x2206, 0x00E0},
	{0x2206, 0x8B85}, {0x2206, 0xAD21}, {0x2206, 0x0CE0}, {0x2206, 0x8B84},
	{0x2206, 0xF621}, {0x2206, 0xE48B}, {0x2206, 0x84EE}, {0x2206, 0x8B72},
	{0x2206, 0xFFBF}, {0x2206, 0x34C2}, {0x2206, 0x022C}, {0x2206, 0x31FC},
	{0x2206, 0x04F8}, {0x2206, 0xFAEF}, {0x2206, 0x69E0}, {0x2206, 0x8B85},
	{0x2206, 0xAD21}, {0x2206, 0x42E0}, {0x2206, 0xE022}, {0x2206, 0xE1E0},
	{0x2206, 0x2358}, {0x2206, 0xC059}, {0x2206, 0x021E}, {0x2206, 0x01E1},
	{0x2206, 0x8B72}, {0x2206, 0x1F10}, {0x2206, 0x9E2F}, {0x2206, 0xE48B},
	{0x2206, 0x72AD}, {0x2206, 0x2123}, {0x2206, 0xE18B}, {0x2206, 0x84F7},
	{0x2206, 0x29E5}, {0x2206, 0x8B84}, {0x2206, 0xAC27}, {0x2206, 0x10AC},
	{0x2206, 0x2605}, {0x2206, 0x0205}, {0x2206, 0x23AE}, {0x2206, 0x1602},
	{0x2206, 0x0535}, {0x2206, 0x0282}, {0x2206, 0x30AE}, {0x2206, 0x0E02},
	{0x2206, 0x056A}, {0x2206, 0x0282}, {0x2206, 0x75AE}, {0x2206, 0x0602},
	{0x2206, 0x04DC}, {0x2206, 0x0282}, {0x2206, 0x04EF}, {0x2206, 0x96FE},
	{0x2206, 0xFC04}, {0x2206, 0xF8F9}, {0x2206, 0xE08B}, {0x2206, 0x87AD},
	{0x2206, 0x2321}, {0x2206, 0xE0EA}, {0x2206, 0x14E1}, {0x2206, 0xEA15},
	{0x2206, 0xAD26}, {0x2206, 0x18F6}, {0x2206, 0x27E4}, {0x2206, 0xEA14},
	{0x2206, 0xE5EA}, {0x2206, 0x15F6}, {0x2206, 0x26E4}, {0x2206, 0xEA14},
	{0x2206, 0xE5EA}, {0x2206, 0x15F7}, {0x2206, 0x27E4}, {0x2206, 0xEA14},
	{0x2206, 0xE5EA}, {0x2206, 0x15FD}, {0x2206, 0xFC04}, {0x2206, 0xF8F9},
	{0x2206, 0xE08B}, {0x2206, 0x87AD}, {0x2206, 0x233A}, {0x2206, 0xAD22},
	{0x2206, 0x37E0}, {0x2206, 0xE020}, {0x2206, 0xE1E0}, {0x2206, 0x21AC},
	{0x2206, 0x212E}, {0x2206, 0xE0EA}, {0x2206, 0x14E1}, {0x2206, 0xEA15},
	{0x2206, 0xF627}, {0x2206, 0xE4EA}, {0x2206, 0x14E5}, {0x2206, 0xEA15},
	{0x2206, 0xE2EA}, {0x2206, 0x12E3}, {0x2206, 0xEA13}, {0x2206, 0x5A8F},
	{0x2206, 0x6A20}, {0x2206, 0xE6EA}, {0x2206, 0x12E7}, {0x2206, 0xEA13},
	{0x2206, 0xF726}, {0x2206, 0xE4EA}, {0x2206, 0x14E5}, {0x2206, 0xEA15},
	{0x2206, 0xF727}, {0x2206, 0xE4EA}, {0x2206, 0x14E5}, {0x2206, 0xEA15},
	{0x2206, 0xFDFC}, {0x2206, 0x04F8}, {0x2206, 0xF9E0}, {0x2206, 0x8B87},
	{0x2206, 0xAD23}, {0x2206, 0x38AD}, {0x2206, 0x2135}, {0x2206, 0xE0E0},
	{0x2206, 0x20E1}, {0x2206, 0xE021}, {0x2206, 0xAC21}, {0x2206, 0x2CE0},
	{0x2206, 0xEA14}, {0x2206, 0xE1EA}, {0x2206, 0x15F6}, {0x2206, 0x27E4},
	{0x2206, 0xEA14}, {0x2206, 0xE5EA}, {0x2206, 0x15E2}, {0x2206, 0xEA12},
	{0x2206, 0xE3EA}, {0x2206, 0x135A}, {0x2206, 0x8FE6}, {0x2206, 0xEA12},
	{0x2206, 0xE7EA}, {0x2206, 0x13F7}, {0x2206, 0x26E4}, {0x2206, 0xEA14},
	{0x2206, 0xE5EA}, {0x2206, 0x15F7}, {0x2206, 0x27E4}, {0x2206, 0xEA14},
	{0x2206, 0xE5EA}, {0x2206, 0x15FD}, {0x2206, 0xFC04}, {0x2206, 0xF8FA},
	{0x2206, 0xEF69}, {0x2206, 0xE08B}, {0x2206, 0x86AD}, {0x2206, 0x2146},
	{0x2206, 0xE0E0}, {0x2206, 0x22E1}, {0x2206, 0xE023}, {0x2206, 0x58C0},
	{0x2206, 0x5902}, {0x2206, 0x1E01}, {0x2206, 0xE18B}, {0x2206, 0x651F},
	{0x2206, 0x109E}, {0x2206, 0x33E4}, {0x2206, 0x8B65}, {0x2206, 0xAD21},
	{0x2206, 0x22AD}, {0x2206, 0x272A}, {0x2206, 0xD400}, {0x2206, 0x01BF},
	{0x2206, 0x34F2}, {0x2206, 0x022C}, {0x2206, 0xA2BF}, {0x2206, 0x34F5},
	{0x2206, 0x022C}, {0x2206, 0xE0E0}, {0x2206, 0x8B67}, {0x2206, 0x1B10},
	{0x2206, 0xAA14}, {0x2206, 0xE18B}, {0x2206, 0x660D}, {0x2206, 0x1459},
	{0x2206, 0x0FAE}, {0x2206, 0x05E1}, {0x2206, 0x8B66}, {0x2206, 0x590F},
	{0x2206, 0xBF85}, {0x2206, 0x6102}, {0x2206, 0x2CA2}, {0x2206, 0xEF96},
	{0x2206, 0xFEFC}, {0x2206, 0x04F8}, {0x2206, 0xF9FA}, {0x2206, 0xFBEF},
	{0x2206, 0x79E2}, {0x2206, 0x8AD2}, {0x2206, 0xAC19}, {0x2206, 0x2DE0},
	{0x2206, 0xE036}, {0x2206, 0xE1E0}, {0x2206, 0x37EF}, {0x2206, 0x311F},
	{0x2206, 0x325B}, {0x2206, 0x019E}, {0x2206, 0x1F7A}, {0x2206, 0x0159},
	{0x2206, 0x019F}, {0x2206, 0x0ABF}, {0x2206, 0x348E}, {0x2206, 0x022C},
	{0x2206, 0x31F6}, {0x2206, 0x06AE}, {0x2206, 0x0FF6}, {0x2206, 0x0302},
	{0x2206, 0x0470}, {0x2206, 0xF703}, {0x2206, 0xF706}, {0x2206, 0xBF34},
	{0x2206, 0x9302}, {0x2206, 0x2C31}, {0x2206, 0xAC1A}, {0x2206, 0x25E0},
	{0x2206, 0xE022}, {0x2206, 0xE1E0}, {0x2206, 0x23EF}, {0x2206, 0x300D},
	{0x2206, 0x311F}, {0x2206, 0x325B}, {0x2206, 0x029E}, {0x2206, 0x157A},
	{0x2206, 0x0258}, {0x2206, 0xC4A0}, {0x2206, 0x0408}, {0x2206, 0xBF34},
	{0x2206, 0x9E02}, {0x2206, 0x2C31}, {0x2206, 0xAE06}, {0x2206, 0xBF34},
	{0x2206, 0x9C02}, {0x2206, 0x2C31}, {0x2206, 0xAC1B}, {0x2206, 0x4AE0},
	{0x2206, 0xE012}, {0x2206, 0xE1E0}, {0x2206, 0x13EF}, {0x2206, 0x300D},
	{0x2206, 0x331F}, {0x2206, 0x325B}, {0x2206, 0x1C9E}, {0x2206, 0x3AEF},
	{0x2206, 0x325B}, {0x2206, 0x1C9F}, {0x2206, 0x09BF}, {0x2206, 0x3498},
	{0x2206, 0x022C}, {0x2206, 0x3102}, {0x2206, 0x83C5}, {0x2206, 0x5A03},
	{0x2206, 0x0D03}, {0x2206, 0x581C}, {0x2206, 0x1E20}, {0x2206, 0x0207},
	{0x2206, 0xA0A0}, {0x2206, 0x000E}, {0x2206, 0x0284}, {0x2206, 0x17AD},
	{0x2206, 0x1817}, {0x2206, 0xBF34}, {0x2206, 0x9A02}, {0x2206, 0x2C31},
	{0x2206, 0xAE0F}, {0x2206, 0xBF34}, {0x2206, 0xC802}, {0x2206, 0x2C31},
	{0x2206, 0xBF34}, {0x2206, 0xC502}, {0x2206, 0x2C31}, {0x2206, 0x0284},
	{0x2206, 0x52E6}, {0x2206, 0x8AD2}, {0x2206, 0xEF97}, {0x2206, 0xFFFE},
	{0x2206, 0xFDFC}, {0x2206, 0x04F8}, {0x2206, 0xBF34}, {0x2206, 0xDA02},
	{0x2206, 0x2CE0}, {0x2206, 0xE58A}, {0x2206, 0xD3BF}, {0x2206, 0x34D4},
	{0x2206, 0x022C}, {0x2206, 0xE00C}, {0x2206, 0x1159}, {0x2206, 0x02E0},
	{0x2206, 0x8AD3}, {0x2206, 0x1E01}, {0x2206, 0xE48A}, {0x2206, 0xD3D1},
	{0x2206, 0x00BF}, {0x2206, 0x34DA}, {0x2206, 0x022C}, {0x2206, 0xA2D1},
	{0x2206, 0x01BF}, {0x2206, 0x34D4}, {0x2206, 0x022C}, {0x2206, 0xA2BF},
	{0x2206, 0x34CB}, {0x2206, 0x022C}, {0x2206, 0xE0E5}, {0x2206, 0x8ACE},
	{0x2206, 0xBF85}, {0x2206, 0x6702}, {0x2206, 0x2CE0}, {0x2206, 0xE58A},
	{0x2206, 0xCFBF}, {0x2206, 0x8564}, {0x2206, 0x022C}, {0x2206, 0xE0E5},
	{0x2206, 0x8AD0}, {0x2206, 0xBF85}, {0x2206, 0x6A02}, {0x2206, 0x2CE0},
	{0x2206, 0xE58A}, {0x2206, 0xD1FC}, {0x2206, 0x04F8}, {0x2206, 0xE18A},
	{0x2206, 0xD1BF}, {0x2206, 0x856A}, {0x2206, 0x022C}, {0x2206, 0xA2E1},
	{0x2206, 0x8AD0}, {0x2206, 0xBF85}, {0x2206, 0x6402}, {0x2206, 0x2CA2},
	{0x2206, 0xE18A}, {0x2206, 0xCFBF}, {0x2206, 0x8567}, {0x2206, 0x022C},
	{0x2206, 0xA2E1}, {0x2206, 0x8ACE}, {0x2206, 0xBF34}, {0x2206, 0xCB02},
	{0x2206, 0x2CA2}, {0x2206, 0xE18A}, {0x2206, 0xD3BF}, {0x2206, 0x34DA},
	{0x2206, 0x022C}, {0x2206, 0xA2E1}, {0x2206, 0x8AD3}, {0x2206, 0x0D11},
	{0x2206, 0xBF34}, {0x2206, 0xD402}, {0x2206, 0x2CA2}, {0x2206, 0xFC04},
	{0x2206, 0xF9A0}, {0x2206, 0x0405}, {0x2206, 0xE38A}, {0x2206, 0xD4AE},
	{0x2206, 0x13A0}, {0x2206, 0x0805}, {0x2206, 0xE38A}, {0x2206, 0xD5AE},
	{0x2206, 0x0BA0}, {0x2206, 0x0C05}, {0x2206, 0xE38A}, {0x2206, 0xD6AE},
	{0x2206, 0x03E3}, {0x2206, 0x8AD7}, {0x2206, 0xEF13}, {0x2206, 0xBF34},
	{0x2206, 0xCB02}, {0x2206, 0x2CA2}, {0x2206, 0xEF13}, {0x2206, 0x0D11},
	{0x2206, 0xBF85}, {0x2206, 0x6702}, {0x2206, 0x2CA2}, {0x2206, 0xEF13},
	{0x2206, 0x0D14}, {0x2206, 0xBF85}, {0x2206, 0x6402}, {0x2206, 0x2CA2},
	{0x2206, 0xEF13}, {0x2206, 0x0D17}, {0x2206, 0xBF85}, {0x2206, 0x6A02},
	{0x2206, 0x2CA2}, {0x2206, 0xFD04}, {0x2206, 0xF8E0}, {0x2206, 0x8B85},
	{0x2206, 0xAD27}, {0x2206, 0x2DE0}, {0x2206, 0xE036}, {0x2206, 0xE1E0},
	{0x2206, 0x37E1}, {0x2206, 0x8B73}, {0x2206, 0x1F10}, {0x2206, 0x9E20},
	{0x2206, 0xE48B}, {0x2206, 0x73AC}, {0x2206, 0x200B}, {0x2206, 0xAC21},
	{0x2206, 0x0DAC}, {0x2206, 0x250F}, {0x2206, 0xAC27}, {0x2206, 0x0EAE},
	{0x2206, 0x0F02}, {0x2206, 0x84CC}, {0x2206, 0xAE0A}, {0x2206, 0x0284},
	{0x2206, 0xD1AE}, {0x2206, 0x05AE}, {0x2206, 0x0302}, {0x2206, 0x84D8},
	{0x2206, 0xFC04}, {0x2206, 0xEE8B}, {0x2206, 0x6800}, {0x2206, 0x0402},
	{0x2206, 0x84E5}, {0x2206, 0x0285}, {0x2206, 0x2804}, {0x2206, 0x0285},
	{0x2206, 0x4904}, {0x2206, 0xEE8B}, {0x2206, 0x6800}, {0x2206, 0xEE8B},
	{0x2206, 0x6902}, {0x2206, 0x04F8}, {0x2206, 0xF9E0}, {0x2206, 0x8B85},
	{0x2206, 0xAD26}, {0x2206, 0x38D0}, {0x2206, 0x0B02}, {0x2206, 0x2B4D},
	{0x2206, 0x5882}, {0x2206, 0x7882}, {0x2206, 0x9F2D}, {0x2206, 0xE08B},
	{0x2206, 0x68E1}, {0x2206, 0x8B69}, {0x2206, 0x1F10}, {0x2206, 0x9EC8},
	{0x2206, 0x10E4}, {0x2206, 0x8B68}, {0x2206, 0xE0E0}, {0x2206, 0x00E1},
	{0x2206, 0xE001}, {0x2206, 0xF727}, {0x2206, 0xE4E0}, {0x2206, 0x00E5},
	{0x2206, 0xE001}, {0x2206, 0xE2E0}, {0x2206, 0x20E3}, {0x2206, 0xE021},
	{0x2206, 0xAD30}, {0x2206, 0xF7F6}, {0x2206, 0x27E4}, {0x2206, 0xE000},
	{0x2206, 0xE5E0}, {0x2206, 0x01FD}, {0x2206, 0xFC04}, {0x2206, 0xF8FA},
	{0x2206, 0xEF69}, {0x2206, 0xE08B}, {0x2206, 0x86AD}, {0x2206, 0x2212},
	{0x2206, 0xE0E0}, {0x2206, 0x14E1}, {0x2206, 0xE015}, {0x2206, 0xAD26},
	{0x2206, 0x9CE1}, {0x2206, 0x85E0}, {0x2206, 0xBF85}, {0x2206, 0x6D02},
	{0x2206, 0x2CA2}, {0x2206, 0xEF96}, {0x2206, 0xFEFC}, {0x2206, 0x04F8},
	{0x2206, 0xFAEF}, {0x2206, 0x69E0}, {0x2206, 0x8B86}, {0x2206, 0xAD22},
	{0x2206, 0x09E1}, {0x2206, 0x85E1}, {0x2206, 0xBF85}, {0x2206, 0x6D02},
	{0x2206, 0x2CA2}, {0x2206, 0xEF96}, {0x2206, 0xFEFC}, {0x2206, 0x0464},
	{0x2206, 0xE48C}, {0x2206, 0xFDE4}, {0x2206, 0x80CA}, {0x2206, 0xE480},
	{0x2206, 0x66E0}, {0x2206, 0x8E70}, {0x2206, 0xE076}, {0x2205, 0xE142},
	{0x2206, 0x0701}, {0x2205, 0xE140}, {0x2206, 0x0405}, {0x220F, 0x0000},
	{0x221F, 0x0000}, {0x2200, 0x1340}, {0x133E, 0x000E}, {0x133F, 0x0010},
	{0x13EB, 0x11BB}, {0x13E0, 0x0010}

	#if 0
	/* Interface setting */
	{0x1305, 0xC010}, {0x1311, 0x1016}, {0x1307, 0x0008}, {0x1B0E, 0x0000},
	{0x1B03, 0x0222},
	#endif
};
/*End of ChipData30[][2]*/

u16 ChipData31[][2]= {
	/*Code of Func*/
	{0x1B03, 0x0876}, {0x1200, 0x7FC4}, {0x1305, 0xC000}, {0x121E, 0x03CA},
	{0x1233, 0x0352}, {0x1234, 0x0064}, {0x1237, 0x0096}, {0x1238, 0x0078},
	{0x1239, 0x0084}, {0x123A, 0x0030}, {0x205F, 0x0002}, {0x2059, 0x1A00},
	{0x205F, 0x0000}, {0x207F, 0x0002}, {0x2077, 0x0000}, {0x2078, 0x0000},
	{0x2079, 0x0000}, {0x207A, 0x0000}, {0x207B, 0x0000}, {0x207F, 0x0000},
	{0x205F, 0x0002}, {0x2053, 0x0000}, {0x2054, 0x0000}, {0x2055, 0x0000},
	{0x2056, 0x0000}, {0x2057, 0x0000}, {0x205F, 0x0000}, {0x133F, 0x0030},
	{0x133E, 0x000E}, {0x221F, 0x0005}, {0x2205, 0x8B86}, {0x2206, 0x800E},
	{0x221F, 0x0000}, {0x133F, 0x0010}, {0x12A3, 0x2200}, {0x6107, 0xE58B},
	{0x6103, 0xA970}, {0x0018, 0x0F00}, {0x0038, 0x0F00}, {0x0058, 0x0F00},
	{0x0078, 0x0F00}, {0x0098, 0x0F00}, {0x133F, 0x0030}, {0x133E, 0x000E},
	{0x221F, 0x0005}, {0x2205, 0x8B6E}, {0x2206, 0x0000}, {0x220F, 0x0100},
	{0x2205, 0xFFF6}, {0x2206, 0x0080}, {0x2205, 0x8000}, {0x2206, 0x0280},
	{0x2206, 0x2BF7}, {0x2206, 0x00E0}, {0x2206, 0xFFF7}, {0x2206, 0xA080},
	{0x2206, 0x02AE}, {0x2206, 0xF602}, {0x2206, 0x0153}, {0x2206, 0x0201},
	{0x2206, 0x6602}, {0x2206, 0x8044}, {0x2206, 0x0201}, {0x2206, 0x7CE0},
	{0x2206, 0x8B8C}, {0x2206, 0xE18B}, {0x2206, 0x8D1E}, {0x2206, 0x01E1},
	{0x2206, 0x8B8E}, {0x2206, 0x1E01}, {0x2206, 0xA000}, {0x2206, 0xE4AE},
	{0x2206, 0xD8EE}, {0x2206, 0x85C0}, {0x2206, 0x00EE}, {0x2206, 0x85C1},
	{0x2206, 0x00EE}, {0x2206, 0x8AFC}, {0x2206, 0x07EE}, {0x2206, 0x8AFD},
	{0x2206, 0x73EE}, {0x2206, 0xFFF6}, {0x2206, 0x00EE}, {0x2206, 0xFFF7},
	{0x2206, 0xFC04}, {0x2206, 0xF8E0}, {0x2206, 0x8B8E}, {0x2206, 0xAD20},
	{0x2206, 0x0302}, {0x2206, 0x8050}, {0x2206, 0xFC04}, {0x2206, 0xF8F9},
	{0x2206, 0xE08B}, {0x2206, 0x85AD}, {0x2206, 0x2548}, {0x2206, 0xE08A},
	{0x2206, 0xE4E1}, {0x2206, 0x8AE5}, {0x2206, 0x7C00}, {0x2206, 0x009E},
	{0x2206, 0x35EE}, {0x2206, 0x8AE4}, {0x2206, 0x00EE}, {0x2206, 0x8AE5},
	{0x2206, 0x00E0}, {0x2206, 0x8AFC}, {0x2206, 0xE18A}, {0x2206, 0xFDE2},
	{0x2206, 0x85C0}, {0x2206, 0xE385}, {0x2206, 0xC102}, {0x2206, 0x2DAC},
	{0x2206, 0xAD20}, {0x2206, 0x12EE}, {0x2206, 0x8AE4}, {0x2206, 0x03EE},
	{0x2206, 0x8AE5}, {0x2206, 0xB7EE}, {0x2206, 0x85C0}, {0x2206, 0x00EE},
	{0x2206, 0x85C1}, {0x2206, 0x00AE}, {0x2206, 0x1115}, {0x2206, 0xE685},
	{0x2206, 0xC0E7}, {0x2206, 0x85C1}, {0x2206, 0xAE08}, {0x2206, 0xEE85},
	{0x2206, 0xC000}, {0x2206, 0xEE85}, {0x2206, 0xC100}, {0x2206, 0xFDFC},
	{0x2206, 0x0400}, {0x2205, 0xE142}, {0x2206, 0x0701}, {0x2205, 0xE140},
	{0x2206, 0x0405}, {0x220F, 0x0000}, {0x221F, 0x0000}, {0x133E, 0x000E},
	{0x133F, 0x0010}, {0x13E0, 0x0010}, {0x207F, 0x0002}, {0x2073, 0x1D22},
	{0x207F, 0x0000}, {0x133F, 0x0030}, {0x133E, 0x000E}, {0x2200, 0x1340},
	{0x133E, 0x000E}, {0x133F, 0x0010}, 

	#if 0
	/* Interface setting */
	{0x1305, 0xC010}, {0x1311, 0x1016}, {0x1307, 0x0008}, {0x1B0E, 0x0000},
	{0x1B03, 0x0222},
	#endif
};
/*End of ChipData31[][2]*/
#endif

rtk_int32 smi_read(rtk_uint32 mAddrs, rtk_uint32 *rData)
{
#if defined(MDC_MDIO_OPERATION)

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write read control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Read data from register 25 */
    MDC_MDIO_READ(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_READ_REG, rData);

    return RT_ERR_OK;

#elif defined(SPI_OPERATION)
    /* Write 8 bits READ OP_CODE */
    SPI_WRITE(SPI_READ_OP, SPI_READ_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Read 16 bits data */
    SPI_READ(rData, SPI_DATA_LEN);

#else
    rtk_uint32 rawData=0, ACK;
    rtk_uint8  con;
    rtk_uint32 ret = RT_ERR_OK;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    rtlglue_drvMutexLock();

    _smi_start();                                /* Start SMI */

    _smi_writeBit(0x0b, 4);                     /* CTRL code: 4'b1011 for RTL8370 */

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x1, 1);                        /* 1: issue READ command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing READ command*/
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK by RTL8369 */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_readBit(8, &rawData);                    /* Read DATA [7:0] */
    *rData = rawData&0xff;

    _smi_writeBit(0x00, 1);                        /* ACK by CPU */

    _smi_readBit(8, &rawData);                    /* Read DATA [15: 8] */

    _smi_writeBit(0x01, 1);                        /* ACK by CPU */
    *rData |= (rawData<<8);

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
#endif /* end of #if defined(MDC_MDIO_OPERATION) */
}

rtk_int32 smi_write(rtk_uint32 mAddrs, rtk_uint32 rData)
{
#if defined(MDC_MDIO_OPERATION)

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write data to register 24 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_WRITE_REG, rData);

    /* Write Start command to register 29 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);

    /* Write data control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

    return RT_ERR_OK;

#elif defined(SPI_OPERATION)
    /* Write 8 bits WRITE OP_CODE */
    SPI_WRITE(SPI_WRITE_OP, SPI_WRITE_OP_LEN);

    /* Write 16 bits register address */
    SPI_WRITE(mAddrs, SPI_REG_LEN);

    /* Write 16 bits data */
    SPI_WRITE(rData, SPI_DATA_LEN);
#else

/*
    if ((mAddrs > 0x018A) || (rData > 0xFFFF))  return    RT_ERR_FAILED;
*/
    rtk_int8 con;
    rtk_uint32 ACK;
    rtk_uint32 ret = RT_ERR_OK;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
       rtlglue_drvMutexLock();

    _smi_start();                                 /* Start SMI */

    _smi_writeBit(0x0b, 4);                       /* CTRL code: 4'b1011 for RTL8370*/

    _smi_writeBit(0x4, 3);                        /* CTRL code: 3'b100 */

    _smi_writeBit(0x0, 1);                        /* 0: issue WRITE command */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for issuing WRITE command*/
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);             /* Set reg_addr[7:0] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);                 /* Set reg_addr[15:8] */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for setting reg_addr[15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData&0xff, 8);                /* Write Data [7:0] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                    /* ACK for writting data [7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData>>8, 8);                    /* Write Data [15:8] out */

    con = 0;
    do {
        con++;
        _smi_readBit(1, &ACK);                        /* ACK for writting data [15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_stop();

    rtlglue_drvMutexUnlock();/*enable CPU interrupt*/

    return ret;
#endif /* end of #if defined(MDC_MDIO_OPERATION) */
}

/* Function Name:
 *      rtl8367b_setAsicRegBits
 * Description:
 *      Set bits value of a specified register
 * Input:
 *      reg 	- register's address
 *      bits 	- bits mask for setting
 *      value 	- bits value for setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_INPUT  	- Invalid input parameter
 * Note:
 *      Set bits of a specified register to value. Both bits and value are be treated as bit-mask
 */
ret_t rtl8367b_setAsicRegBits(rtk_uint32 reg, rtk_uint32 bits, rtk_uint32 value)
{

#if defined(RTK_X86_ASICDRV)

		rtk_uint32 regData;
		ret_t retVal;
		rtk_uint32 bitsShift;
		rtk_uint32 valueShifted;

		if(bits >= (1 << RTL8367B_REGBITLENGTH) )
			return RT_ERR_INPUT;

		bitsShift = 0;
		while(!(bits & (1 << bitsShift)))
		{
			bitsShift++;
			if(bitsShift >= RTL8367B_REGBITLENGTH)
				return RT_ERR_INPUT;
		}

		valueShifted = value << bitsShift;
		if(valueShifted > RTL8367B_REGDATAMAX)
			return RT_ERR_INPUT;

		retVal = Access_Read(reg, 2, &regData);
		if(TRUE != retVal)
			return RT_ERR_SMI;

		if(0x8367B == cleDebuggingDisplay)
			PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);

		regData = regData & (~bits);
		regData = regData | (valueShifted & bits);

		retVal = Access_Write(reg,2, regData);
		if(TRUE != retVal)
			return RT_ERR_SMI;

		if(0x8367B == cleDebuggingDisplay)
			PRINT("W[0x%4.4x]=0x%4.4x\n", reg, regData);

#elif defined(CONFIG_RTL8367B_ASICDRV_TEST)

		rtk_uint32 regData;
		rtk_uint32 bitsShift;
		rtk_uint32 valueShifted;

		if(bits >= (1 << RTL8367B_REGBITLENGTH) )
			return RT_ERR_INPUT;

		bitsShift = 0;
		while(!(bits & (1 << bitsShift)))
		{
			bitsShift++;
			if(bitsShift >= RTL8367B_REGBITLENGTH)
				return RT_ERR_INPUT;
		}
		valueShifted = value << bitsShift;

		if(valueShifted > RTL8367B_REGDATAMAX)
			return RT_ERR_INPUT;

		if(reg >= CLE_VIRTUAL_REG_SIZE)
			return RT_ERR_OUT_OF_RANGE;

		regData = CleVirtualReg[reg] & (~bits);
		regData = regData | (valueShifted & bits);

		CleVirtualReg[reg] = regData;

		if(0x8367B == cleDebuggingDisplay)
			PRINT("W[0x%4.4x]=0x%4.4x\n", reg, regData);

#elif defined(EMBEDDED_SUPPORT)

    rtk_uint32 regData;
    rtk_uint32 bitsShift;
    rtk_uint32 valueShifted;

    if(reg > RTL8367B_REGDATAMAX )
	    return RT_ERR_INPUT;

    if(bits >= (1 << RTL8367B_REGBITLENGTH) )
        return RT_ERR_INPUT;

    bitsShift = 0;
    while(!(bits & (1 << bitsShift)))
    {
        bitsShift++;
        if(bitsShift >= RTL8367B_REGBITLENGTH)
            return RT_ERR_INPUT;
    }

    valueShifted = value << bitsShift;
    if(valueShifted > RTL8367B_REGDATAMAX)
        return RT_ERR_INPUT;

    regData = getReg(reg);
    regData = regData & (~bits);
    regData = regData | (valueShifted & bits);

		setReg(reg, regData);

#else

		rtk_uint32 regData;
		ret_t retVal;
		rtk_uint32 bitsShift;
		rtk_uint32 valueShifted;

		if(bits >= (1 << RTL8367B_REGBITLENGTH) )
			return RT_ERR_INPUT;

		bitsShift = 0;
		while(!(bits & (1 << bitsShift)))
		{
			bitsShift++;
			if(bitsShift >= RTL8367B_REGBITLENGTH)
				return RT_ERR_INPUT;
		}
		valueShifted = value << bitsShift;

		if(valueShifted > RTL8367B_REGDATAMAX)
			return RT_ERR_INPUT;

		retVal = smi_read(reg, &regData);
		if(retVal != RT_ERR_OK)
			return RT_ERR_SMI;
		
	  #ifdef CONFIG_RTL865X_CLE
		if(0x8367B == cleDebuggingDisplay)
			PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);
	  #endif

		regData = regData & (~bits);
		regData = regData | (valueShifted & bits);

		retVal = smi_write(reg, regData);
		if(retVal != RT_ERR_OK)
			return RT_ERR_SMI;
		
	  #ifdef CONFIG_RTL865X_CLE
		if(0x8367B == cleDebuggingDisplay)
			PRINT("W[0x%4.4x]=0x%4.4x\n", reg, regData);
	  #endif
		
#endif

		return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_setAsicRegBit
 * Description:
 *      Set a bit value of a specified register
 * Input:
 *      reg 	- register's address
 *      bit 	- bit location
 *      value 	- value to set. It can be value 0 or 1.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_INPUT  	- Invalid input parameter
 * Note:
 *      Set a bit of a specified register to 1 or 0.
 */
ret_t rtl8367b_setAsicRegBit(rtk_uint32 reg, rtk_uint32 bit, rtk_uint32 value)
{

#if defined(RTK_X86_ASICDRV)
		rtk_uint32 regData;
		ret_t retVal;

		if(bit >= RTL8367B_REGBITLENGTH)
				return RT_ERR_INPUT;

		retVal = Access_Read(reg, 2, &regData);
		if(TRUE != retVal)
				return RT_ERR_SMI;

		if(0x8367B == cleDebuggingDisplay)
				PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);

		if(value)
				regData = regData | (1 << bit);
		else
				regData = regData & (~(1 << bit));

		retVal = Access_Write(reg,2, regData);
		if(TRUE != retVal)
				return RT_ERR_SMI;

		if(0x8367B == cleDebuggingDisplay)
				PRINT("W[0x%4.4x]=0x%4.4x\n", reg, regData);

#elif defined(CONFIG_RTL8367B_ASICDRV_TEST)

		if(bit >= RTL8367B_REGBITLENGTH)
				return RT_ERR_INPUT;
		else if(reg >= CLE_VIRTUAL_REG_SIZE)
				return RT_ERR_OUT_OF_RANGE;

		if(value)
		{
			CleVirtualReg[reg] =  CleVirtualReg[reg] | (1 << bit);
		}
		else
		{
			CleVirtualReg[reg] =  CleVirtualReg[reg] & (~(1 << bit));
		}

		if(0x8367B == cleDebuggingDisplay)
				PRINT("W[0x%4.4x]=0x%4.4x\n", reg, CleVirtualReg[reg]);

#elif defined(EMBEDDED_SUPPORT)
    rtk_uint16 tmp;

    if(reg > RTL8367B_REGDATAMAX || value > 1)
	    	return RT_ERR_INPUT;

		tmp = getReg(reg);
		tmp &= (1 << bitIdx);
		tmp |= (value << bitIdx);
		setReg(reg, tmp);

#else
		rtk_uint32 regData;
		ret_t retVal;

		if(bit >= RTL8367B_REGBITLENGTH)
				return RT_ERR_INPUT;

		retVal = smi_read(reg, &regData);
		if(retVal != RT_ERR_OK)
				return RT_ERR_SMI;

  	#ifdef CONFIG_RTL865X_CLE
		if(0x8367B == cleDebuggingDisplay)
				PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);
  	#endif

		if(value)
				regData = regData | (1 << bit);
		else
				regData = regData & (~(1 << bit));

		retVal = smi_write(reg, regData);
		if(retVal != RT_ERR_OK)
				return RT_ERR_SMI;

	  #ifdef CONFIG_RTL865X_CLE
		if(0x8367B == cleDebuggingDisplay)
			PRINT("W[0x%4.4x]=0x%4.4x\n", reg, regData);
	  #endif

#endif

		return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_setAsicReg
 * Description:
 *      Set content of asic register
 * Input:
 *      reg 	- register's address
 *      value 	- Value setting to register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 * Note:
 *      The value will be set to ASIC mapping address only and it is always return RT_ERR_OK while setting un-mapping address registers
 */
ret_t rtl8367b_setAsicReg(rtk_uint32 reg, rtk_uint32 value)
{

#if defined(RTK_X86_ASICDRV)/*RTK-CNSD2-NickWu-20061222: for x86 compile*/

		ret_t retVal;

		retVal = Access_Write(reg,2,value);
		if(TRUE != retVal) return RT_ERR_SMI;

		if(0x8367B == cleDebuggingDisplay)
				PRINT("W[0x%4.4x]=0x%4.4x\n",reg,value);

#elif defined(CONFIG_RTL8367B_ASICDRV_TEST)

		/*MIBs emulating*/
		if(reg == RTL8367B_REG_MIB_ADDRESS)
		{
			CleVirtualReg[RTL8367B_MIB_COUNTER_BASE_REG] = 0x1;
			CleVirtualReg[RTL8367B_MIB_COUNTER_BASE_REG+1] = 0x2;
			CleVirtualReg[RTL8367B_MIB_COUNTER_BASE_REG+2] = 0x3;
			CleVirtualReg[RTL8367B_MIB_COUNTER_BASE_REG+3] = 0x4;
		}

		if(reg >= CLE_VIRTUAL_REG_SIZE)
			return RT_ERR_OUT_OF_RANGE;

		CleVirtualReg[reg] = value;

		if(0x8367B == cleDebuggingDisplay)
				PRINT("W[0x%4.4x]=0x%4.4x\n",reg,CleVirtualReg[reg]);

#elif defined(EMBEDDED_SUPPORT)
    if(reg > RTL8367B_REGDATAMAX || value > RTL8367B_REGDATAMAX )
	    return RT_ERR_INPUT;

    setReg(reg, value);

#else
		ret_t retVal;

		retVal = smi_write(reg, value);
		
		if(retVal != RT_ERR_OK)
			return RT_ERR_SMI;

		#ifdef CONFIG_RTL865X_CLE
		if(0x8367B == cleDebuggingDisplay)
			PRINT("W[0x%4.4x]=0x%4.4x\n",reg,value);
	  #endif

#endif

		return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_getAsicReg
 * Description:
 *      Get content of asic register
 * Input:
 *      reg 	- register's address
 *      value 	- Value setting to register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 * Note:
 *      Value 0x0000 will be returned for ASIC un-mapping address
 */
ret_t rtl8367b_getAsicReg(rtk_uint32 reg, rtk_uint32 *pValue)
{

#if defined(RTK_X86_ASICDRV)

		rtk_uint32 regData;
		ret_t retVal;

		retVal = Access_Read(reg, 2, &regData);
		if(TRUE != retVal)
			return RT_ERR_SMI;

		*pValue = regData;

		if(0x8367B == cleDebuggingDisplay)
				PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);

#elif defined(CONFIG_RTL8367B_ASICDRV_TEST)

		if(reg >= CLE_VIRTUAL_REG_SIZE)
				return RT_ERR_OUT_OF_RANGE;

		*pValue = CleVirtualReg[reg];

		if(0x8367B == cleDebuggingDisplay)
				PRINT("R[0x%4.4x]=0x%4.4x\n", reg, CleVirtualReg[reg]);

#elif defined(EMBEDDED_SUPPORT)

		if(reg > RTL8367B_REGDATAMAX)
	    return RT_ERR_INPUT;

		*value = getReg(reg);

#else
		rtk_uint32 regData;
		ret_t retVal;

		retVal = smi_read(reg, &regData);
		if(retVal != RT_ERR_OK)
				return RT_ERR_SMI;

		*pValue = regData;

		#ifdef CONFIG_RTL865X_CLE
		if(0x8367B == cleDebuggingDisplay)
				PRINT("R[0x%4.4x]=0x%4.4x\n", reg, regData);
  	#endif

#endif

		return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_setAsicPHYReg
 * Description:
 *      Set PHY registers
 * Input:
 *      phyNo 	- Physical port number (0~4)
 *      phyAddr - PHY address (0~31)
 *      phyData - Writing data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_PHY_REG_ID  		- invalid PHY address
 *      RT_ERR_PHY_ID  			- invalid PHY no
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      None
 */
ret_t rtl8367b_setAsicPHYReg( rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 value)
{
		rtk_uint32 regAddr;

    if(phyNo > RTL8367B_PHY_INTERNALNOMAX)
        return RT_ERR_PORT_ID;
		#if 0
    if(phyAddr > RTL8367B_PHY_REGNOMAX)
        return RT_ERR_PHY_REG_ID;
		#endif

		regAddr = 0x2000 + (phyNo << 5) + phyAddr;

    return rtl8367b_setAsicReg(regAddr, value);
}

/* Function Name:
 *      rtl8367b_getAsicPHYReg
 * Description:
 *      Get PHY registers
 * Input:
 *      phyNo 	- Physical port number (0~4)
 *      phyAddr - PHY address (0~31)
 *      pRegData - Writing data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 *      RT_ERR_PHY_REG_ID  		- invalid PHY address
 *      RT_ERR_PHY_ID  			- invalid PHY no
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      None
 */
ret_t rtl8367b_getAsicPHYReg( rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 *value)
{
		rtk_uint32 regAddr;

    if(phyNo > RTL8367B_PHY_INTERNALNOMAX)
        return RT_ERR_PORT_ID;

    if(phyAddr > RTL8367B_PHY_REGNOMAX)
        return RT_ERR_PHY_REG_ID;

    regAddr = 0x2000 + (phyNo << 5) + phyAddr;

    return rtl8367b_getAsicReg(regAddr, value);
}

static rtk_api_ret_t _rtk_switch_init_setreg(rtk_uint32 reg, rtk_uint32 data)
{
#ifndef MDC_MDIO_OPERATION
    rtk_uint32      busyFlag, cnt;
#endif
    rtk_api_ret_t   retVal;

#ifdef MDC_MDIO_OPERATION
    if((retVal = rtl8367b_setAsicReg(reg, data) != RT_ERR_OK))
            return retVal;
#else
    if ((reg & 0xF000) == 0x2000)
    {
        cnt = 0;
        busyFlag = 1;
        while (busyFlag&&cnt<5)
        {
            cnt++;
            if ((retVal = rtl8367b_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                return retVal;
        }
				
        if (5 == cnt)
            return RT_ERR_BUSYWAIT_TIMEOUT;

        if ((retVal = rtl8367b_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, data)) !=  RT_ERR_OK)
            return retVal;
        if ((retVal = rtl8367b_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, reg)) !=  RT_ERR_OK)
            return retVal;
        if ((retVal = rtl8367b_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
            return retVal;
    }
    else
    {
        if((retVal = rtl8367b_setAsicReg(reg, data)) != RT_ERR_OK)
            return retVal;
    }
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_switch_init
 * Description:
 *      Set chip to default configuration enviroment
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */
rtk_api_ret_t rtk_switch_init(void)
{
    rtk_uint16      i;
    rtk_uint32      data;
    rtk_api_ret_t   retVal;
    rtk_uint32      phy;
#if defined(CHIP_AUTO_DETECT)
    rtk_uint32      polling_time;
    rtk_uint32      chip_idx = 0;
#endif

    if((retVal = rtl8367b_setAsicReg(0x13C2, 0x0249)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_getAsicReg(0x1301, &data)) != RT_ERR_OK)
        return retVal;

#if defined(CHIP_AUTO_DETECT)
    if((retVal = rtl8367b_setAsicReg(0x1371, 0x000F)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_setAsicReg(0x1370, 0x0002)) != RT_ERR_OK)
        return retVal;

    polling_time = 0;
    while(polling_time < 10000)
    {
        if((retVal = rtl8367b_getAsicReg(0x1370, &data)) != RT_ERR_OK)
            return retVal;

        if((data & 0x0004) == 0)
            break;

        polling_time++;
    }

    if(polling_time >= 10000)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if((retVal = rtl8367b_getAsicReg(0x1373, &data)) != RT_ERR_OK)
        return retVal;

    chip_idx |= ((data & 0x000F) << 4);

    if((retVal = rtl8367b_setAsicReg(0x1371, 0x000D)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_setAsicReg(0x1370, 0x0002)) != RT_ERR_OK)
        return retVal;

    polling_time = 0;
    while(polling_time < 10000)
    {
        if((retVal = rtl8367b_getAsicReg(0x1370, &data)) != RT_ERR_OK)
            return retVal;

        if((data & 0x0004) == 0)
            break;

        polling_time++;
    }

    if(polling_time >= 10000)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if((retVal = rtl8367b_getAsicReg(0x1373, &data)) != RT_ERR_OK)
        return retVal;

    chip_idx |= (data & 0x000F);


    if((chip_idx & 0x00F0) == 0x0000)
    {
        /* RTL8365MB & RTL8305MB */
        if((chip_idx & 0x000F) >> 3)
        {
            /* RTL8305MB */
            init_para = ChipData81;
            init_size = (sizeof(ChipData81) / ((sizeof(rtk_uint16))*2));
        }
        else if((chip_idx & 0x000F) >> 2)
        {
            /* RTL8365MB */
            init_para = ChipData11;
            init_size = (sizeof(ChipData11) / ((sizeof(rtk_uint16))*2));
        }
        else if((chip_idx & 0x000F) >> 1)
        {
            /* RTL8305MB */
            init_para = ChipData81;
            init_size = (sizeof(ChipData81) / ((sizeof(rtk_uint16))*2));
        }
        else if(chip_idx & 0x000F)
        {
            /* RTL8365MB */
            init_para = ChipData11;
            init_size = (sizeof(ChipData11) / ((sizeof(rtk_uint16))*2));
        }
        else
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

#elif defined(RTK_X86_ASICDRV)
    if(init_para == ChipData00)
    {
        if(data & 0xF000)
        {
            init_para = ChipData01;
            init_size = (sizeof(ChipData01) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData00) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData10)
    {
        if(data & 0xF000)
        {
            init_para = ChipData11;
            init_size = (sizeof(ChipData11) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData10) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData20)
    {
        if(data & 0xF000)
        {
            init_para = ChipData21;
            init_size = (sizeof(ChipData21) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData20) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData30)
    {
        if(data & 0xF000)
        {
            init_para = ChipData31;
            init_size = (sizeof(ChipData31) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData30) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData40)
    {
        if(data & 0xF000)
        {
            init_para = ChipData41;
            init_size = (sizeof(ChipData41) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData40) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData50)
    {
        if(data & 0xF000)
        {
            init_para = ChipData51;
            init_size = (sizeof(ChipData51) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData50) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData60)
    {
        if(data & 0xF000)
        {
            init_para = ChipData61;
            init_size = (sizeof(ChipData61) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData60) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData70)
    {
        if(data & 0xF000)
        {
            init_para = ChipData71;
            init_size = (sizeof(ChipData71) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData70) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData80)
    {
        if(data & 0xF000)
        {
            init_para = ChipData81;
            init_size = (sizeof(ChipData81) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData80) / ((sizeof(rtk_uint16))*2));
    }
    else if(init_para == ChipData90)
    {
        if(data & 0xF000)
        {
            init_para = ChipData91;
            init_size = (sizeof(ChipData91) / ((sizeof(rtk_uint16))*2));
        }
        else
            init_size = (sizeof(ChipData90) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8363SB)
    if(data & 0xF000)
    {
        init_para = ChipData01;
        init_size = (sizeof(ChipData01) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData00;
        init_size = (sizeof(ChipData00) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8365MB)
    if(data & 0xF000)
    {
        init_para = ChipData11;
        init_size = (sizeof(ChipData11) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData10;
        init_size = (sizeof(ChipData10) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367_VB)
    if(data & 0xF000)
    {
        init_para = ChipData21;
        init_size = (sizeof(ChipData21) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData20;
        init_size = (sizeof(ChipData20) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367RB)
    if(data & 0xF000)
    {
        init_para = ChipData31;
        init_size = (sizeof(ChipData31) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData30;
        init_size = (sizeof(ChipData30) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367R_VB)
    if(data & 0xF000)
    {
        init_para = ChipData41;
        init_size = (sizeof(ChipData41) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData40;
        init_size = (sizeof(ChipData40) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367MB)
    if(data & 0xF000)
    {
        init_para = ChipData51;
        init_size = (sizeof(ChipData51) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData50;
        init_size = (sizeof(ChipData50) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8367M_VB)
    if(data & 0xF000)
    {
        init_para = ChipData61;
        init_size = (sizeof(ChipData61) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData60;
        init_size = (sizeof(ChipData60) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8368MB)
    if(data & 0xF000)
    {
        init_para = ChipData71;
        init_size = (sizeof(ChipData71) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData70;
        init_size = (sizeof(ChipData70) / ((sizeof(rtk_uint16))*2));
    }

#elif defined(CHIP_RTL8305MB)
    if(data & 0xF000)
    {
        init_para = ChipData81;
        init_size = (sizeof(ChipData81) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData80;
        init_size = (sizeof(ChipData80) / ((sizeof(rtk_uint16))*2));
    }
#elif defined(CHIP_RTL8307M_VB)
    if(data & 0xF000)
    {
        init_para = ChipData91;
        init_size = (sizeof(ChipData91) / ((sizeof(rtk_uint16))*2));
    }
    else
    {
        init_para = ChipData90;
        init_size = (sizeof(ChipData90) / ((sizeof(rtk_uint16))*2));
    }

#else
    /* Not define CHIP, Error */
    init_para = NULL;
#endif

    if(init_para == NULL)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    /* Analog parameter update. ID:0001 */
    for(phy = 0; phy <= RTK_PHY_ID_MAX; phy++)
    {
        if((retVal = rtl8367b_setAsicPHYReg(phy, 31, 0x7)) != RT_ERR_OK)
            return retVal;

        if((retVal = rtl8367b_setAsicPHYReg(phy, 30, 0x2c)) != RT_ERR_OK)
            return retVal;

        if((retVal = rtl8367b_setAsicPHYReg(phy, 25, 0x0504)) != RT_ERR_OK)
            return retVal;

        if((retVal = rtl8367b_setAsicPHYReg(phy, 31, 0x0)) != RT_ERR_OK)
            return retVal;
    }

    for(i = 0; i < init_size; i++)
    {
        if((retVal = _rtk_switch_init_setreg((rtk_uint32)init_para[i][0], (rtk_uint32)init_para[i][1])) != RT_ERR_OK)
            return retVal;
    }

    /* Analog parameter update. ID:0002 */
    if((retVal = rtl8367b_setAsicPHYReg(1, 31, 0x2)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_getAsicPHYReg(1, 17, &data)) != RT_ERR_OK)
        return retVal;

    data |= 0x01E0;

    if((retVal = rtl8367b_setAsicPHYReg(1, 17, data)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_setAsicPHYReg(1, 31, 0x0)) != RT_ERR_OK)
        return retVal;


    if((retVal = rtl8367b_setAsicRegBit(0x18e0, 0, 0)) != RT_ERR_OK)
        return retVal;

    if((retVal = rtl8367b_setAsicReg(0x1303, 0x0778)) != RT_ERR_OK)
        return retVal;
    if((retVal = rtl8367b_setAsicReg(0x1304, 0x7777)) != RT_ERR_OK)
        return retVal;
    if((retVal = rtl8367b_setAsicReg(0x13E2, 0x01FE)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_setAsicPortExtMode
 * Description:
 *      Set external interface mode configuration
 * Input:
 *      id 		- external interface id (0~2)
 *      mode 	- external interface mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
ret_t rtl8367b_setAsicPortExtMode(rtk_uint32 id, rtk_uint32 mode)
{
    ret_t   retVal;

    if(id >= RTL8367B_EXTNO)
        return RT_ERR_OUT_OF_RANGE;

    if(mode >= EXT_END)
        return RT_ERR_OUT_OF_RANGE;

    if(mode == EXT_GMII)
    {
        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_EXT0_RGMXF, RTL8367B_EXT0_RGTX_INV_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_EXT1_RGMXF, RTL8367B_EXT1_RGTX_INV_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367b_setAsicRegBits(RTL8367B_REG_EXT_TXC_DLY, RTL8367B_EXT1_GMII_TX_DELAY_MASK, 5)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367b_setAsicRegBits(RTL8367B_REG_EXT_TXC_DLY, RTL8367B_EXT0_GMII_TX_DELAY_MASK, 6)) != RT_ERR_OK)
            return retVal;
    }

    if( (mode == EXT_TMII_MAC) || (mode == EXT_TMII_PHY) )
    {
        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_BYPASS_LINE_RATE, id, 1)) != RT_ERR_OK)
            return retVal;
    }
    else
    {
        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_BYPASS_LINE_RATE, id, 0)) != RT_ERR_OK)
            return retVal;
    }

	if(0 == id || 1 == id)
   		return rtl8367b_setAsicRegBits(RTL8367B_REG_DIGITAL_INTERFACE_SELECT, RTL8367B_SELECT_GMII_0_MASK << (id * RTL8367B_SELECT_GMII_1_OFFSET), mode);
	else
   		return rtl8367b_setAsicRegBits(RTL8367B_REG_DIGITAL_INTERFACE_SELECT_1, RTL8367B_SELECT_RGMII_2_MASK, mode);
}

/* Function Name:
 *      rtl8367b_getAsicPortForceLinkExt
 * Description:
 *      Get external interface force linking configuration
 * Input:
 *      id 			- external interface id (0~1)
 *      pPortAbility - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
ret_t rtl8367b_getAsicPortForceLinkExt(rtk_uint32 id, rtl8367b_port_ability_t *pPortAbility)
{
    rtk_uint32  reg_data;
    rtk_uint16  ability_data;
    ret_t       retVal;

    /* Invalid input parameter */
    if(id >= RTL8367B_EXTNO)
        return RT_ERR_OUT_OF_RANGE;

		if(0 == id || 1 == id)
    	retVal = rtl8367b_getAsicReg(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE+id, &reg_data);
		else
	    retVal = rtl8367b_getAsicReg(RTL8367B_REG_DIGITAL_INTERFACE2_FORCE, &reg_data);

    if(retVal != RT_ERR_OK)
        return retVal;

    ability_data = (rtk_uint16)reg_data;
    memcpy(pPortAbility, &ability_data, sizeof(rtl8367b_port_ability_t));
    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_setAsicPortForceLinkExt
 * Description:
 *      Set external interface force linking configuration
 * Input:
 *      id 			- external interface id (0~2)
 *      portAbility - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
ret_t rtl8367b_setAsicPortForceLinkExt(rtk_uint32 id, rtl8367b_port_ability_t *pPortAbility)
{
    rtk_uint32  reg_data;

    /* Invalid input parameter */
    if(id >= RTL8367B_EXTNO)
        return RT_ERR_OUT_OF_RANGE;

    reg_data = (rtk_uint32)(*(rtk_uint16 *)pPortAbility);

		if(0 == id || 1 == id)
	    return rtl8367b_setAsicReg(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE + id, reg_data);
		else
	    return rtl8367b_setAsicReg(RTL8367B_REG_DIGITAL_INTERFACE2_FORCE, reg_data);
}

/* Function Name:
 *      rtk_port_macForceLinkExt_set
 * Description:
 *      Set external interface force linking configuration.
 * Input:
 *      port            - external port ID
 *      mode            - external interface mode
 *      pPortability    - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      This API can set external interface force mode properties.
 *      The external interface can be set to:
 *      - MODE_EXT_DISABLE,
 *      - MODE_EXT_RGMII,
 *      - MODE_EXT_MII_MAC,
 *      - MODE_EXT_MII_PHY,
 *      - MODE_EXT_TMII_MAC,
 *      - MODE_EXT_TMII_PHY,
 *      - MODE_EXT_GMII,
 *      - MODE_EXT_RMII_MAC,
 *      - MODE_EXT_RMII_PHY,
 */
rtk_api_ret_t rtk_port_macForceLinkExt_set(rtk_ext_port_t port, rtk_mode_ext_t mode, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtl8367b_port_ability_t ability;
    rtk_uint32  reg, mask;

    if (port >= EXT_PORT_END)
        return RT_ERR_INPUT;

    if (mode >=MODE_EXT_END)
        return RT_ERR_INPUT;

    if (mode == MODE_EXT_RGMII_33V)
        return RT_ERR_INPUT;

    if (pPortability->forcemode > 1 || pPortability->speed > 2 || pPortability->duplex > 1 ||
       pPortability->link > 1 || pPortability->nway > 1 || pPortability->txpause > 1 || pPortability->rxpause > 1)
        return RT_ERR_INPUT;

    if(port == EXT_PORT_0)
    {
        reg = RTL8367B_REG_DIGITAL_INTERFACE_SELECT;
        mask = RTL8367B_SELECT_GMII_0_MASK;
    }
    else if(port == EXT_PORT_1)
    {
        reg = RTL8367B_REG_DIGITAL_INTERFACE_SELECT;
        mask = RTL8367B_SELECT_GMII_1_MASK;
    }
    else if(port == EXT_PORT_2)
    {
        reg = RTL8367B_REG_DIGITAL_INTERFACE_SELECT_1;
        mask = RTL8367B_SELECT_RGMII_2_MASK;
    }
    else
        return RT_ERR_INPUT;

    if ((retVal = rtl8367b_setAsicPortExtMode(port, mode)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8367b_getAsicPortForceLinkExt( (rtk_uint32)port, &ability)) != RT_ERR_OK)
        return retVal;

    ability.forcemode = pPortability->forcemode;
    ability.speed     = pPortability->speed;
    ability.duplex    = pPortability->duplex;
    ability.link      = pPortability->link;
    ability.nway      = pPortability->nway;
    ability.txpause   = pPortability->txpause;
    ability.rxpause   = pPortability->rxpause;

    if ((retVal = rtl8367b_setAsicPortForceLinkExt( (rtk_uint32)port, &ability)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_rgmiiDelayExt_set
 * Description:
 *      Set RGMII interface delay value for TX and RX.
 * Input:
 *      txDelay - TX delay value, 1 for delay 2ns and 0 for no-delay
 *      rxDelay - RX delay value, 0~7 for delay setup.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      This API can set external interface 2 RGMII delay.
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.
 */
rtk_api_ret_t rtk_port_rgmiiDelayExt_set(rtk_ext_port_t port, rtk_data_t txDelay, rtk_data_t rxDelay)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regAddr, regData;

    if ((txDelay > 1) || (rxDelay > 7))
        return RT_ERR_INPUT;

    if (port >= EXT_PORT_END)
        return RT_ERR_INPUT;

    if(port == EXT_PORT_0)
        regAddr = RTL8367B_REG_EXT0_RGMXF;
    else if(port == EXT_PORT_1)
        regAddr = RTL8367B_REG_EXT1_RGMXF;
    else if(port == EXT_PORT_2)
        regAddr = RTL8367B_REG_EXT2_RGMXF;
    else
        return RT_ERR_INPUT;

    if ((retVal = rtl8367b_getAsicReg(regAddr, &regData)) != RT_ERR_OK)
        return retVal;

    regData = (regData & 0xFFF0) | ((txDelay << 3) & 0x0008) | (rxDelay & 0x0007);

    if ((retVal = rtl8367b_setAsicReg(regAddr, regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8367b_setAsicIGMPStaticRouterPort
 * Description:
 *      Set IGMP static router port mask
 * Input:
 *      pmsk 	- Static portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_PORT_MASK  	- Invalid port mask
 * Note:
 *      None
 */
ret_t rtl8367b_setAsicIGMPStaticRouterPort(rtk_uint32 pmsk)
{
    if(pmsk > RTL8367B_PORTMASK)
        return RT_ERR_PORT_MASK;

    return rtl8367b_setAsicRegBits(RTL8367B_REG_IGMP_STATIC_ROUTER_PORT, RTL8367B_IGMP_STATIC_ROUTER_PORT_MASK, pmsk);
}

/* Function Name:
 *      rtk_igmp_static_router_port_set
 * Description:
 *      Configure static router port
 * Input:
 *      portmask    - Static Port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API set static router port
 */
rtk_api_ret_t rtk_igmp_static_router_port_set(rtk_portmask_t portmask)
{
    rtk_api_ret_t retVal;

    if ( portmask.bits[0] > RTK_MAX_PORT_MASK)
        return RT_ERR_PORT_MASK;

    if ((retVal = rtl8367b_setAsicIGMPStaticRouterPort(portmask.bits[0]))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

void rtl_msleep(u32 msec)
{
		u32	i;

		for(i=0;i<msec;i++) 
		{
				udelay(1000);
		}
}

int rtl_sw_reset(u32 delay_msec)
{
		u16	bit9 = 0x200;

		//printf("[%s:%d]\n",__func__,__LINE__);

		// GPIO settings
		// clear P2_ALTSEL0_BIT9
		REG32(BSP_GPIO_P2_ALTSEL0) &= ~bit9;
		// clear P2_ALTSEL1_BIT9
		REG32(BSP_GPIO_P2_ALTSEL1) &= ~bit9;
		// set P2_DIR_BIT9
		REG32(BSP_GPIO_P2_DIR) |= bit9;
		// set P2_OD_BIT9
		REG32(BSP_GPIO_P2_OD) |= bit9;
		// P2_OUT_BIT9, pull-high first
		REG32(BSP_GPIO_P2_OUT) |= bit9;

		// pull-low
		REG32(BSP_GPIO_P2_OUT) &= ~bit9;

		asm("sync");

		// wait
		if(delay_msec > 0)
			rtl_msleep(delay_msec);

		// pull-high
		REG32(BSP_GPIO_P2_OUT) |= bit9;

		asm("sync");

		return 0;
}

#if 0
u32 rtl_smi_read(u16 addr, u16 *data)
{
		/* Write address control code to register 31 */
	  vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
    vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);
		/* Write address to register 23 */
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, addr);
		/* Write read control code to register 21 */
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);
		/* Read data from register 25 */
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		*data = vr9_mdio_read(MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_READ_REG);

		return 0;
}

u32 rtl_smi_write(u16 addr, u16 data)
{
		/* Write address control code to register 31 */
	  vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);
		/* Write address to register 23 */
	  vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, addr);
		/* Write data to register 24 */
	  vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_WRITE_REG, data);
		/* Write data control code to register 21 */
	  vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_START_REG, MDC_MDIO_START_OP);
		vr9_mdio_write(MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

		return 0;
}


int rtl_port_power(u16 port, u16 bOn)
{
		u16	addr;
		u16	data;

		addr = 0x2000 + (port<<5) + 0/*reg0*/;
		
		rtl_smi_read(addr, &data);
		//printf("[%s:%d]reg[0x%x]=0x%x, ",__func__,__LINE__,addr,data);

		if ( bOn ) // Normal operation
			rtl_smi_write(addr, data & ~0x0800);
		else // Power down
			rtl_smi_write(addr, data | 0x0800);

		//printf("write 0x%x into\n", bOn ? data & ~0x0800 : data | 0x0800);

		return 0;
}

u16 rtl_phyreg_set(u16 phyNo, u16 phyAddr, u16 value)
{
		u32 regAddr;

    if(phyNo > 0x4/*RTL8367B_PHY_INTERNALNOMAX*/)
        return -1;//RT_ERR_PORT_ID;

    if(phyAddr > 0x1F/*RTL8367B_PHY_REGNOMAX*/)
        return -1;//RT_ERR_PHY_REG_ID;

    regAddr = 0x2000 + (phyNo << 5) + phyAddr;

    return (u16)rtl_smi_write(regAddr, value);
}

#endif

#if 1
int rtl_sw_init(void)
{
		int s32RetVal;
		unsigned int i,phyNo,data;

		unsigned char u8PortMode, u8TxDelay, u8RxDelay;

		rtk_portmask_t stStaticRouterPortMask;

		if(0 != (s32RetVal = rtk_switch_init()))
		{
				printf("rtk_switch_init error!, error = 0x%x\n", s32RetVal);

				/* Power down PHY */
				for(phyNo=0;phyNo<4;phyNo++)
				{
						if(0 != (s32RetVal = smi_read(0x2000+(phyNo<<5),&data)))
						{
								printf("[%s:%d] read PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
								//return RT_ERR_SMI;
						}
						else
						{
								if(0 != (s32RetVal = rtl8367b_setAsicPHYReg(phyNo,0x2000+(phyNo<<5),(data|0x0800))))
								{
										printf("[%s:%d] set PCS reg[0x%x] failed!!!\n",__func__,__LINE__,0x2000+(phyNo<<5));
								}
						}
				}
				
				return -1;
		}

		for(i=EXT_PORT_1; i<EXT_PORT_END; i++)
		{
				if (i==EXT_PORT_1)	
				{	
						u8PortMode 	= 1;
						u8TxDelay 	= 1;
						u8RxDelay 	= 0;
				}
				else	
				{
						u8PortMode 	= 0;
						u8TxDelay 	= 0;
						u8RxDelay 	= 0;
				}
				
				if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_set((rtk_port_t)i, (rtk_mode_ext_t)u8PortMode, (rtk_port_mac_ability_t *)&rtl8367_ext_port_cfg[i])))
				{
						printf("Set port %d force linking configuration mode %d failed! error = 0x%x\n", i, u8PortMode, s32RetVal);
						return -1;
				}

				if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_set((rtk_ext_port_t)i, (rtk_data_t)u8TxDelay, (rtk_data_t)u8RxDelay)))
				{
						printf("Set extension port %d RGMII failed!, error = 0x%x\n", i, s32RetVal);
						return -1;
				}

		}

		stStaticRouterPortMask.bits[0] = (0x1<<6);
		if(RT_ERR_OK != (s32RetVal = rtk_igmp_static_router_port_set(stStaticRouterPortMask)))
		{
				printf("Set IGMP static router port failed!, errno = 0x%x\n", s32RetVal);
				return -1;
		}
		#if 0
		if(RT_ERR_OK != (s32RetVal = rtk_l2_init()))
		{
				printf("Error enable L2 LUT, errno = 0x%x\n", s32RetVal);
				return -1;
		}

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_set(LOOKUP_DIP)))
		{
				printf("Error enable L2 LUT IP look-up function, errno = 0x%x\n", s32RetVal);
				return -1;
		}
		#endif
		return 0;
}
#else
int rtl_sw_init(void)
{
    u16 i;
    u16 data;
		u16 init_size;
    u32 retVal;
    u32 phyNo;
		u32 regAddr;
		
		u16 (*init_para)[2];

		#if defined(RTK_DBG)
		printf("[%s:%d]\n",__func__,__LINE__);
		#endif

    if((retVal = rtl_smi_write(0x13C2, 0x0249)) != 0)
    {
				printf("[%s:%d] write reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x13C2,retVal);
				return retVal;
		}
        
    if((retVal = rtl_smi_read(0x1301, &data)) != 0)
    {
				printf("[%s:%d] read reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x1301,retVal);
				return retVal;
    }
		
    if(data & 0xF000)
    {
        init_para = ChipData31;
        init_size = (sizeof(ChipData31) / ((sizeof(u16))*2));
    }
    else
    {
        init_para = ChipData30;
        init_size = (sizeof(ChipData30) / ((sizeof(u16))*2));
    }

    /* Analog parameter update. ID:0001 */
    for(phyNo=0; phyNo<=3/*RTK_PHY_ID_MAX*/; phyNo++)
    {
		    if((retVal = rtl_phyreg_set(phyNo, 31, 0x7)) != 0)
		    {
						printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,phyNo,31);
						return retVal;
				}

		    if((retVal = rtl_phyreg_set(phyNo, 30, 0x2c)) != 0)
		    {
						printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,phyNo,30);
						return retVal;
				}

		    if((retVal = rtl_phyreg_set(phyNo, 25, 0x0504)) != 0)
		    {
						printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,phyNo,25);
						return retVal;
				}

		    if((retVal = rtl_phyreg_set(phyNo, 31, 0x0)) != 0)
		    {
						printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,phyNo,31);
						return retVal;
				}
    }

		/* Init parameters */
    for(i=0; i<init_size; i++)
    {
        if((retVal = rtl_smi_write((u16)init_para[i][0], (u16)init_para[i][1])) != 0)
		    {
						printf("[%s:%d] write reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,(u16)init_para[i][0],retVal);
						return retVal;
				}
    }

    /* Analog parameter update. ID:0002 */
    if((retVal = rtl_phyreg_set(1, 31, 0x2)) != 0)
    {
				printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,1,31);
				return retVal;
		}

	  regAddr = 0x2000 + (1 << 5) + 17;
    if((retVal = rtl_smi_read(regAddr, &data)) != 0)
    {
				printf("[%s:%d] read reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,regAddr,retVal);
				return retVal;
		}
		
    data |= 0x01E0;

    if((retVal = rtl_phyreg_set(1, 17, data)) != 0)
    {
				printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,1,17);
				return retVal;
		}

    if((retVal = rtl_phyreg_set(1, 31, 0)) != 0)
    {
				printf("[%s:%d] write phyNo=%d, reg=%d failed!, errno=%d\n",__func__,__LINE__,1,31);
				return retVal;
		}

    if((retVal = rtl_smi_read(0x18e0, &data)) != 0)
    {
				printf("[%s:%d] read reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x18e0,retVal);
				return retVal;
		}
		
		data &= ~(0x0001);

		if((retVal = rtl_smi_write(0x18e0, data)) != 0)
    {
				printf("[%s:%d] write reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x18e0,retVal);
				return retVal;
		}

		if((retVal = rtl_smi_write(0x1303, 0x0778)) != 0)
    {
				printf("[%s:%d] write reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x1303,retVal);
				return retVal;
		}

		if((retVal = rtl_smi_write(0x1304, 0x7777)) != 0)
    {
				printf("[%s:%d] write reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x1304,retVal);
				return retVal;
		}

		if((retVal = rtl_smi_write(0x13E2, 0x01FE)) != 0)
    {
				printf("[%s:%d] write reg[0x%x] failed!, errno=%d\n",__func__,__LINE__,0x13E2,retVal);
				return retVal;
		}

    return 0;
}
#endif

#endif /* CONFIG_DRIVER_VR9 */ /* ctc for RTL8367RB */


