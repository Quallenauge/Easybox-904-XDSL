/******************************************************************************
**
** FILE NAME    : ifxmips_hsnand.c
** PROJECT      : UEIP
** MODULES      : NAND Flash
**
** DATE         : 23 July 2010
** AUTHOR       : Md Firdaus B Alias Thani
** DESCRIPTION  : HSNAND Flash MTD Driver
** COPYRIGHT    :       Copyright (c) 2010
**                      Lantiq Asia Pacific
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author      		    $Version   $Comment
** 24 July 2010  Mohammad Firdaus B Alias Thani   1.0        initial version
** 15 Oct  2010  Mohammad Firdaus B Alias Thani   1.1        Ported to incl. Kern 2.6.32 
** 3  Nov  2010  Mohammad Firdaus B Alias Thani   1.2        Fixed performance issue
** 4  Jan  2011	 Mohammad Firdaus B Alias Thani   1.3	     Scattered DMA transfer with 2 descriptors
*******************************************************************************/

#include <linux/version.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/smp_lock.h>
#include <asm/io.h>
#include <asm/system.h>

/* Project header */
#include <asm/ifx/ifx_types.h>
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/ifx_gpio.h>
#include <asm/ifx/common_routines.h>
#include <asm/ifx/irq.h>
#include <ifx_dma_core.h>

#include "ifxmips_mtd_nand.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32))
#include "ifxmips_mtd_nand_ecc.h"
#endif

#define IFX_MTD_HSNAND_BANK_NAME "ifx_hsnand"
#define HSNAND_DMA_BURST_LEN    DMA_BURSTL_8DW
#define SAMSUNG_512		0x0000EC75
#define SAMSUNG			0xEC
#define INFINEON		0xC1
#define QIMONDA			0xC0
#define STM			0x20
#define HSNAND_READ		0	
#define HSNAND_WRITE		0x80
#define HSNAND_WRITE_COMPLETE   0x00000200
#define HSNAND_READ_COMPLETE    0x00000100
#define SINGLE_BYTE_XFER	1
#define HSNAND_ECC_ON		1
#define HSNAND_ECC_OFF		0
#define NAND_READ_OOB           5
#define NAND_READ_ECC		0x50
#define HSNAND_ADDR_MASK	2
#define SINGLE_BIT_CORRECTABLE  1
#define NON_CORRECTABLE_ERROR   2
#define HSNAND_ECC_BYTES	3
#define HSNAND_ECCSIZE		512
#define READ_SIZE		512
#define HSNAND_PAGE_SIZE	4096
#define MAX_ECC_BYTES		3
#define NAND_CON_DEFAULT        0x80F005F3	
#define IFX_EBU_ECC_ISTAT	IFX_EBU_PCC_ISTAT
#define IFX_EBU_ECC_INT_OUT	IFX_EBU_PCC_INT_OUT
#define CHECK_ECC_STATE		(IFX_REG_R32(IFX_EBU_NAND_ECC_CR) & 0x00000003);
#define GET_ECC_ROW_POSITION	MS(IFX_REG_R32(IFX_EBU_NAND_ECC_CR), IFX_EBU_NAND_ECC_ROW_VAL);
#define GET_ECC_BIT_POSITION	MS(IFX_REG_R32(IFX_EBU_NAND_ECC_CR), IFX_EBU_NAND_ECC_BIT_POS);
#define READ_HW_ECC(val)	val = IFX_REG_R32(IFX_EBU_NAND_ECC0);
#define UPDATE_RX_CNT(val)	IFX_REG_W32(val,IFX_RX_CNT);
#define WAIT_ECC_DONE		while((IFX_REG_R32(IFX_EBU_ECC_ISTAT) & 0x00000020) == 0x00000000 ) {}
#define READ_NAND 		*((volatile u8*)(NAND_BASE_ADDRESS | (NAND_READ_DATA)))
#define WRITE_NAND(d)		NAND_WRITE(NAND_WRITE_DATA,d);
#define DEBUG_LINE		printk("fn: %s, ln: %d \n", __func__, __LINE__);
#define HSNAND_EVENT		1
#define MAX_CHUNKS              7  // 3 overhead + 4/2 descriptors
#define HSNAND_MAX_PAGE_SIZE	544

#if defined(CONFIG_MTD_PARTITIONS) 
#if defined(CONFIG_MTD_CMDLINE_PARTS)
static const char *part_probes[] = { "cmdlinepart", NULL };
#endif /* CONFIG_MTD_PARTITIONS */
#endif /*  CONFIG_MTD_CMDLINE_PARTS */

static struct dma_device_info *dma_device = NULL;
static struct mtd_info *ifx_hsnand_mtd = NULL;
extern const struct mtd_partition g_ifx_mtd_nand_partitions[];
extern const int g_ifx_mtd_nand_partion_num;

static inline void NAND_DISABLE_CE(struct nand_chip *nand) { IFX_REG_W32_MASK(NAND_CON_CE,0,IFX_EBU_NAND_CON); }
static inline void NAND_ENABLE_CE(struct nand_chip *nand)  { IFX_REG_W32_MASK(0,NAND_CON_CE,IFX_EBU_NAND_CON); }
u32 latchcmd = 0;
void init_ecc(void);
int dma_device_write_sdio (struct dma_device_info *dma_dev, u32 *chunkdataptr, void *opt);

/* HSNAND private structure */
struct hsnand_info {
    u8 sw_ecc_data[MAX_ECC_BYTES];
    atomic_t rx_flag;
    atomic_t tx_flag;
    int rx_length;
    int ecc_step;
    int chip_id; 
    int addr_cycle;
    u32 *buf_aligned;
    u32 chip_info;
    u32 ndac_ctl_1;
    u32 ndac_ctl_2;
    u32 current_page;
    u32 ecc_status;
    u32 hw_ecc_data;
    struct mtd_info *mtd_prv;

    wait_queue_head_t hsnand_wait;
    volatile long wait_flag;
    
};

struct hsnand_info hsnand_dev;
u8 *tmp_wr_buf;
u8 *tmp_rd_buf;
u8 *write_ptr;
u32 chunk_ptr[MAX_CHUNKS];

struct chunt_t {
    u32 len;
    void *data;
};

/*!
  \fn static int ifx_hsnand_dma_intr_handler(struct dma_device_info *dma_dev, int status)
  \ingroup  IFX_HSNAND_DRV
  \brief  DMA interrupt handler routine function
  \param  dma_dev DMA device structure
  \param  status Type of interrupt generated
  \return none
*/

static int ifx_hsnand_dma_intr_handler(struct dma_device_info *dma_dev, int status)
{
    int i;
    struct hsnand_info *hsnand = &hsnand_dev;
    //struct mtd_info *mtd = hsnand->mtd_prv;
    //struct nand_chip *chip = mtd->priv;
    //int *eccpos = chip->ecc.layout->eccpos;
    //int eccbytes = chip->ecc.bytes;
    //int current_eccpos = hsnand->ecc_step;
    //int chan = dma_dev->current_tx_chan;

    switch (status) 
    {
        case RCV_INT:

#if 0
            /* Prepare the NAND flash controller to use conventional NAND
             * read to read the oob bytes. To improve performance, the 
             * configuration is done here instead of the read OOB function.
	     * Only used for nand page size larger than 512 
             */            
	    if (mtd->writesize > 512) {
                IFX_REG_W32(NAND_CON_DEFAULT, IFX_EBU_NAND_CON);
    	        DEBUG(MTD_DEBUG_LEVEL2, "Reading ecc info on page %d . . \n", hsnand->current_page);
                IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) & ~(1<<18), IFX_EBU_NAND_CON);
                chip->cmdfunc(mtd, NAND_CMD_READOOB, eccpos[current_eccpos], hsnand->current_page);
	    }
#endif
	    /* have to close channel here, otherwise DMA will keep 
             * triggering interrupts continuously.
             */
	    for (i = 0; i < dma_dev->num_rx_chan; i++)
	        dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);

#ifdef MTD_DEBUG_LEVEL1
            /* The ECC0 register has to be read here, otherwise, the value 
             * of the ECC0 register will be correct in the first read but 
             * it will be not correct after subsequent reads. Read the ECC0
             * register and store it in an intermediate variable.
             * */
            READ_HW_ECC(hsnand->hw_ecc_data);
            asm("sync");
            DEBUG(MTD_DEBUG_LEVEL1, "---- ECC0: 0x%08x ---- \n", hsnand->hw_ecc_data);
            DEBUG(MTD_DEBUG_LEVEL1, "---- ECC_CR: 0x%08x ---- \n", 
                               IFX_REG_R32(IFX_EBU_NAND_ECC_CR));
            asm("sync");
#endif

	    hsnand->ecc_status = IFX_REG_R32(IFX_EBU_NAND_ECC_CR);

            set_bit(HSNAND_EVENT, &hsnand->wait_flag);
            wake_up_interruptible(&hsnand->hsnand_wait);
            break;

        case TX_BUF_FULL_INT:
            for (i = 0; i < dma_dev->num_tx_chan; i++)
            {
                 if (dma_dev->tx_chan[i]->control == IFX_DMA_CH_ON)
                     dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
            }  
            break;    

        case TRANSMIT_CPT_INT:
            for (i = 0; i < dma_dev->num_tx_chan; i++)
                 dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);

            set_bit(HSNAND_EVENT, &hsnand->wait_flag);
            wake_up_interruptible(&hsnand->hsnand_wait);
            break;

    }     
    return IFX_SUCCESS;
}

void debug_nand(void)
{   

    printk("\n**** Dumping HSNAND/EBU Registers *****\n");
    printk("EBU_CLC:   	  0x%08x\n", IFX_REG_R32(IFX_EBU_CLC));
    printk("IEN_ECC:      0x%08x\n", IFX_REG_R32(IFX_EBU_ECC_IEN));
    printk("EBU CON:	  0x%08x\n", IFX_REG_R32(IFX_EBU_CON));
    printk("ADDRSEL1:     0x%08x\n", IFX_REG_R32(IFX_EBU_ADDSEL1));
    printk("EBU_NAND_CON: 0x%08x\n", IFX_REG_R32(IFX_EBU_NAND_CON));
    printk("EBU_BUSCON:   0x%08x\n", IFX_REG_R32(IFX_EBU_BUSCON1));
    printk("CS_BASE_A:    0x%08x\n", IFX_REG_R32(IFX_CS_BASE_A));
    printk("HSNAND_DEV:	  0x%08x\n", IFX_REG_R32(IFX_NAND_INFO));
    printk("NDAC_CTL1: 	  0x%08x\n", IFX_REG_R32(IFX_NDAC_CTL1));
    printk("NDAC_CTL2: 	  0x%08x\n", IFX_REG_R32(IFX_NDAC_CTL2));
    printk("HSMD_CTL:  	  0x%08x\n", IFX_REG_R32(IFX_HSMD_CTRL));
    printk("RX_CNT:    	  0x%08x\n", IFX_REG_R32(IFX_RX_CNT));
    printk("NAND_ECC_CR:  0x%08x\n", IFX_REG_R32(IFX_EBU_NAND_ECC_CR));
    printk("INTR_MSK_CTRL: 0x%08x\n",IFX_REG_R32(IFX_HSNAND_INTR_MASK_CTRL));
    printk("INTR_STAT: 	  0x%08x\n", IFX_REG_R32(IFX_HSNAND_INTR_STAT));
    printk("ECC CR:	  0x%08x\n", IFX_REG_R32(IFX_EBU_NAND_ECC_CR));
    printk("ECC0 :	  0x%08x\n", IFX_REG_R32(IFX_EBU_NAND_ECC0));
    printk("RX_CNT:	  0x%08x\n", IFX_REG_R32(IFX_RX_CNT));
    printk("DX_PLUS:	  0x%08x\n", IFX_REG_R32(IFX_DPLUS_CTRL));
}

void debug_dma(int channel)
{
    u32 tmp;
    printk("\n**** Dumping DMA Registers for channel %d\n", channel);
    tmp = IFX_REG_R32(IFX_DMA_CS(0));
    IFX_REG_W32(channel, IFX_DMA_CS(0));
    printk("DMA_CS	: 0x%08x\n", IFX_REG_R32(IFX_DMA_CS(0)));
    printk("DMA_CDLEN   : 0x%08x\n", IFX_REG_R32(IFX_DMA_CDLEN(0)));
    printk("CIE		: 0x%08x\n", IFX_REG_R32(IFX_DMA_CIE(0)));
    printk("CIS		: 0x%08x\n", IFX_REG_R32(IFX_DMA_CIS(0)));
    printk("CPOLL	: 0x%08x\n", IFX_REG_R32(IFX_DMA_CPOLL));
    printk("CDBA	: 0x%08x\n", IFX_REG_R32(IFX_DMA_CDBA(0)));
    printk("CCTRL	: 0x%08x\n", IFX_REG_R32(IFX_DMA_CCTRL(0)));
    printk("IRNEN	: 0x%08x\n", IFX_REG_R32(IFX_DMA_IRNEN));
    printk("IRNCR	: 0x%08x\n", IFX_REG_R32(IFX_DMA_IRNCR));
    printk("IRNICR	: 0x%08x\n", IFX_REG_R32(IFX_DMA_IRNICR));
    IFX_REG_W32(tmp, IFX_DMA_CS(0));
    tmp = IFX_REG_R32(IFX_DMA_PS(0));
    IFX_REG_W32(6, IFX_DMA_PS(0));
    printk("DMA_PS	: 0x%08x\n", IFX_REG_R32(IFX_DMA_PS(0)));
    printk("PCTRL	: 0x%08x\n", IFX_REG_R32(IFX_DMA_PCTRL(0)));
    IFX_REG_W32(tmp, IFX_DMA_PS(0));
    printk("IM4_IER	: 0x%08x\n", IFX_REG_R32(IFX_ICU_IM4_IER));
    
}

/*!
  \fn static void init_rdwr_hsnand(int size, int rdwr, int mode)
  \ingroup  IFX_HSNAND_DRV
  \brief  Initialize HSNAND registers to prepare device for HSNAND transfer
  \param  size Size of data to be transfered
  \param  rdwr Control passed (read or write initialization)
  \param  mode determinds whether HWECC is enabled/disabled
  \return none
*/

static void init_rdwr_hsnand(int size, int rdwr, int mode)
{
    u32 reg = 0x0;
    u32 ecc_status;
    int chip_id;
    struct hsnand_info *hsnand = &hsnand_dev; 

    chip_id = hsnand->chip_id;
    
    if (mode & HSNAND_ECC_ON) 
    {
        reg = SM(IFX_EBU_ECC_IEN_ENABLE, IFX_EBU_ECC_IEN_IR);
        IFX_REG_W32(reg | IFX_REG_R32(IFX_EBU_ECC_IEN), IFX_EBU_ECC_IEN);
        ecc_status = IFX_EBU_NAND_CON_ECC_ON;
    }
    else {
        reg = SM(IFX_EBU_ECC_IEN_DISABLE, IFX_EBU_ECC_IEN_IR);
        IFX_REG_W32(reg & 0xFFFFFFDF, IFX_EBU_ECC_IEN);
        ecc_status = IFX_EBU_NAND_CON_ECC_OFF;
    }

    DEBUG(MTD_DEBUG_LEVEL3, "--- IEN_ECC: 0x%08x ----\n", IFX_REG_R32(IFX_EBU_ECC_IEN));

    IFX_REG_W32(0x0, IFX_EBU_CLC);
    DEBUG(MTD_DEBUG_LEVEL3, "---- IFX_EBU_CLC: 0x%08x -----\n", IFX_REG_R32(IFX_EBU_CLC));

    IFX_REG_W32(chip_id, IFX_NAND_INFO); // samsung nand *fix it to accomodate other nands
    reg = SM(ecc_status, IFX_EBU_NAND_CON_ECC) |
          SM(IFX_EBU_NAND_CON_LAT_EN_DEF, IFX_EBU_NAND_CON_LAT_EN) | 
          SM(IFX_EBU_NAND_CON_OUT_CS1, IFX_EBU_NAND_CON_OUT_CS) |
          SM(IFX_EBU_NAND_CON_IN_CS1, IFX_EBU_NAND_CON_IN_CS) |
          SM(IFX_EBU_NAND_CON_PRE_P_LOW, IFX_EBU_NAND_CON_PRE_P) |
          SM(IFX_EBU_NAND_CON_WP_P_LOW, IFX_EBU_NAND_CON_WP_P) |
          SM(IFX_EBU_NAND_CON_SE_P_LOW, IFX_EBU_NAND_CON_SE_P) | 
          SM(IFX_EBU_NAND_CON_CS_P_LOW, IFX_EBU_NAND_CON_CS_P) | 
          SM(IFX_EBU_NAND_CON_CLE_P_HIGH, IFX_EBU_NAND_CON_CLE_P) |
          SM(IFX_EBU_NAND_CON_ALE_P_HIGH, IFX_EBU_NAND_CON_ALE_P) |
          SM(IFX_EBU_NAND_CON_CSMUX_E_ENABLE, IFX_EBU_NAND_CON_CSMUX_E) |
          SM(IFX_EBU_NAND_CON_NANDM_ENABLE, IFX_EBU_NAND_CON_NANDM);
    IFX_REG_W32(reg, IFX_EBU_NAND_CON); //EBU NAND CON
    
    DEBUG(MTD_DEBUG_LEVEL3, "--- IFX_EBU_NAND_CON: 0x%08x----\n", 
           IFX_REG_R32(IFX_EBU_NAND_CON));

    reg = NAND_BASE_ADDRESS & 0x1fffffff; // NAND BASE ADDR
    IFX_REG_W32(reg, IFX_CS_BASE_A);
    DEBUG(MTD_DEBUG_LEVEL3, "--- IFX_CS_BASE_A: 0x%08x -----\n", IFX_REG_R32(IFX_CS_BASE_A)); 

    reg = (NAND_BASE_ADDRESS & 0x1fffff00)| IFX_EBU_ADDSEL1_MASK(2)| IFX_EBU_ADDSEL1_REGEN;
    IFX_REG_W32(reg, IFX_EBU_ADDSEL1); // EBU ADDRSEL 1

    DEBUG(MTD_DEBUG_LEVEL3, "---- ADDRSEL1: 0x%08x -----\n", IFX_REG_R32(IFX_EBU_ADDSEL1));

    reg = IFX_EBU_BUSCON1_SETUP | SM(IFX_EBU_BUSCON1_ALEC3, IFX_EBU_BUSCON1_ALEC) |
           SM(IFX_EBU_BUSCON1_BCGEN_CS, IFX_EBU_BUSCON1_BCGEN) | 
           SM(IFX_EBU_BUSCON1_WAITWRC1, IFX_EBU_BUSCON1_WAITWRC) |
           SM(IFX_EBU_BUSCON1_WAITRDC3, IFX_EBU_BUSCON1_WAITRDC) | 
           SM(IFX_EBU_BUSCON1_HOLDC1, IFX_EBU_BUSCON1_HOLDC)     |
           SM(IFX_EBU_BUSCON1_RECOVC0, IFX_EBU_BUSCON1_RECOVC)   |
           SM(IFX_EBU_BUSCON1_CMULT4, IFX_EBU_BUSCON1_CMULT); 
    IFX_REG_W32(reg,IFX_EBU_BUSCON1);  // EBU BUSCON1
    //IFX_REG_W32(0x0040c29f,IFX_EBU_BUSCON1);  // EBU BUSCON1
    DEBUG(MTD_DEBUG_LEVEL1, "---- BUSCON1: 0x%08x ----\n", IFX_REG_R32(IFX_EBU_BUSCON1));
    
    reg = hsnand->ndac_ctl_1;
    IFX_REG_W32(reg,IFX_NDAC_CTL1);
    reg = hsnand->ndac_ctl_2;
    IFX_REG_W32(reg,IFX_NDAC_CTL2);
    reg = 0x0;
    reg |= 0xf;
    IFX_REG_W32(reg, IFX_HSNAND_INTR_MASK_CTRL);

    DEBUG(MTD_DEBUG_LEVEL3, "---- IFX_HSNAND_INTR_MASK_CTRL 0x%08x -----\n", 
            IFX_REG_R32(IFX_HSNAND_INTR_MASK_CTRL));    

    if (rdwr == HSNAND_READ)
    {
        UPDATE_RX_CNT(0x0);
        IFX_REG_W32(0x0, IFX_EBU_NAND_ECC0); // clear ECC0 register
        reg = SM(IFX_HSNAND_CE_SEL_NONE, IFX_HSNAND_CE_SEL) |
              SM(IFX_HSNAND_NO_XFER, IFX_HSNAND_XFER_SEL) |
              SM(IFX_HSNAND_FSM_DISABLED, IFX_HSNAND_FSM) |
              SM(IFX_HSNAND_ENR_HSDMA, IFX_HSNAND_ENR);
        IFX_REG_W32(reg & 0xFFFFFF7F , IFX_HSMD_CTRL);
        reg = SM(IFX_EBU_NAND_ECC_CRM_ENABLE, IFX_EBU_NAND_ECC_CRM) |
              SM(IFX_EBU_NAND_ECC_PAGE_512, IFX_EBU_NAND_ECC_PAGE);
        IFX_REG_W32(0x80004000, IFX_EBU_NAND_ECC_CR);
        
    }
    else if (rdwr == HSNAND_WRITE)
    {
    	UPDATE_RX_CNT(0x0);
        reg = SM(IFX_HSNAND_CE_SEL_NONE, IFX_HSNAND_CE_SEL) |
              SM(IFX_HSNAND_NO_XFER, IFX_HSNAND_XFER_SEL) |
              SM(IFX_HSNAND_FSM_DISABLED, IFX_HSNAND_FSM) |
              SM(IFX_HSNAND_ENR_HSDMA, IFX_HSNAND_ENR);
        IFX_REG_W32(reg & 0xFFFFFF7F , IFX_HSMD_CTRL);
        IFX_REG_W32(0x0, IFX_EBU_NAND_ECC0);
        reg = IFX_REG_R32(IFX_EBU_NAND_ECC_CR) |
              SM(IFX_EBU_NAND_ECC_PAGE_512, IFX_EBU_NAND_ECC_PAGE);
        reg &= ~(1 << 31);
        IFX_REG_W32(reg, IFX_EBU_NAND_ECC_CR); 
	//IFX_REG_W32(0x00004000, IFX_EBU_NAND_ECC_CR);

    }
    else {
        printk("Unknown DMA init mode\n");
        return;
    }
    
    DEBUG(MTD_DEBUG_LEVEL3, "---- RX_CNT: 0x%08x ----\n", IFX_REG_R32(IFX_RX_CNT));
    DEBUG(MTD_DEBUG_LEVEL3, "---- HSMD_CTRL: 0x%08x ----\n", IFX_REG_R32(IFX_HSMD_CTRL));
    DEBUG(MTD_DEBUG_LEVEL3, "---- ECC0: 0x%08x ---- \n", IFX_REG_R32(IFX_EBU_NAND_ECC0));
    DEBUG(MTD_DEBUG_LEVEL3, "---- EBU_NAND_ECC_CR: 0x%08x ----\n", 
           IFX_REG_R32(IFX_EBU_NAND_ECC_CR));

    //debug_nand();
    return;
}

static void NAND_WAIT_READY(struct nand_chip *nand)
{
    while(!NAND_READY){}
}


static int ifx_hsnand_ready(struct mtd_info *mtd)
{
      struct nand_chip *nand = mtd->priv;

      NAND_WAIT_READY(nand);
      return 1;
}

/*!
  \fn static void ifx_hsnand_hwctl(struct mtd_info *mtd, int mode)
  \ingroup  IFX_HSNAND_DRV
  \brief function to enable/disable hardware ecc
  \param  mtd  MTD device structure
  \param mode  control mode to enable/disable hardware ecc
  \return none
*/ 

static void ifx_hsnand_hwctl(struct mtd_info *mtd, int mode) 
{
   u32 reg=0x0;
   if (mode & HSNAND_ECC_ON) 
   {
        IFX_REG_W32(reg,IFX_EBU_NAND_ECC0);
        reg = SM(IFX_EBU_ECC_IEN_DISABLE, IFX_EBU_ECC_IEN_IR);
        IFX_REG_W32(reg, IFX_EBU_ECC_IEN);
        IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) | (1<<31), IFX_EBU_NAND_CON);
        reg &= 0x0;
        reg = IFX_REG_R32(IFX_EBU_NAND_ECC_CR) & ~(3<<14);
        IFX_REG_W32(reg,IFX_EBU_NAND_ECC_CR);
        IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_ECC_CR) | (1<<14), IFX_EBU_NAND_ECC_CR);
        IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_ECC_CR) | (1<<31), IFX_EBU_NAND_ECC_CR);
        DEBUG(MTD_DEBUG_LEVEL3, "NAND_CON: 0x%08x, NAND_ECC_CR: 0x%08x\n", 
              IFX_REG_R32(IFX_EBU_NAND_CON), IFX_REG_R32(IFX_EBU_NAND_ECC_CR));
   }
   else if (mode == HSNAND_ECC_OFF )
   {
        IFX_REG_W32(reg,IFX_EBU_NAND_ECC0);
        reg = SM(IFX_EBU_NAND_CON_OUT_CS1, IFX_EBU_NAND_CON_OUT_CS) |
              SM(IFX_EBU_NAND_CON_IN_CS1, IFX_EBU_NAND_CON_IN_CS) |
              IFX_EBU_NAND_CON_PRE_P | IFX_EBU_NAND_CON_WP_P |
              IFX_EBU_NAND_CON_SE_P | IFX_EBU_NAND_CON_CS_P |
              SM(IFX_EBU_NAND_CON_CLE_P_HIGH, IFX_EBU_NAND_CON_CLE_P) |
              SM(IFX_EBU_NAND_CON_ALE_P_HIGH, IFX_EBU_NAND_CON_ALE_P) |
              IFX_EBU_NAND_CON_CSMUX_E | IFX_EBU_NAND_CON_NANDM;
        IFX_REG_W32(reg, IFX_EBU_NAND_CON);
        //IFX_REG_W32(0x000005f3, IFX_EBU_NAND_CON);
        reg = IFX_REG_R32(IFX_EBU_NAND_ECC_CR) & ~(3<<14);
        IFX_REG_W32(reg,IFX_EBU_NAND_ECC_CR);
        IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_ECC_CR) & ~(1<<31), IFX_EBU_NAND_ECC_CR);
        DEBUG(MTD_DEBUG_LEVEL3, "NAND_CON: 0x%08x, NAND_ECC_CR: 0x%08x\n", 
               IFX_REG_R32(IFX_EBU_NAND_CON), IFX_REG_R32(IFX_EBU_NAND_ECC_CR));
   }

   else {
        printk("Unknown ECC mode settings: ln: %d, fn: %s\n", __LINE__, __func__);
   }

   return;
}

/*!
  \fn static void ifx_hsnand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf) 
  \ingroup  IFX_HSNAND_DRV
  \brief write page raw without considering ECC
  \param  mtd  MTD device structure
  \param  chip Nand chip device structure
  \param  buf  memory location to get write data from
  \return none
*/ 

static void ifx_hsnand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf)
{
   int write_length = mtd->writesize;
   struct hsnand_info *hsnand = &hsnand_dev;
    
   chip->write_buf(mtd, buf, write_length);
   chip->ecc.write_oob(mtd, mtd->priv, hsnand->current_page);
   
   return;
}


/*!
  \fn static void ifx_hsnand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf)
  \ingroup  IFX_HSNAND_DRV
  \brief write page with hardware ECC checking mechanism
  \param  mtd  MTD device structure
  \param  chip Nand chip device structure
  \param  buf  memory location to get write data from
  \return none
*/

static void ifx_hsnand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const u8 *buf)
{
    int i, eccsize = chip->ecc.size;
    int eccbytes = chip->ecc.bytes;
    int eccsteps = chip->ecc.steps;
    int write_size = mtd->writesize;
    int *eccpos = chip->ecc.layout->eccpos;
    u8 *ecc_data = chip->buffers->ecccalc;
    const u8 *dataptr = buf;
    int dma_write_size = write_size + mtd->oobsize;
    u32 single_chunk[] = {dma_write_size, dma_write_size, (u32) buf };
    struct chunk_t *chunk = (struct chunk_t *) (&single_chunk[1]);

    init_rdwr_hsnand(write_size, HSNAND_WRITE, HSNAND_ECC_OFF);
    memset(chunk_ptr, 0, sizeof(chunk_ptr));

    for (i = 0; eccsteps; eccsteps--, i += eccbytes, dataptr += eccsize)
    {
        chip->ecc.calculate(mtd, dataptr, &ecc_data[i]);
        DEBUG(MTD_DEBUG_LEVEL1, "sw ecc: %02x-%02x-%02x\n",
              ecc_data[i+2], ecc_data[i+1], ecc_data[i]);
    }


    for (i = 0; i < chip->ecc.total; i++) {
        chip->oob_poi[eccpos[i]] = ecc_data[i];
        DEBUG(MTD_DEBUG_LEVEL1, "data: %02x, pos: %d \n",
              chip->oob_poi[eccpos[i]], eccpos[i]);
    }

    chunk_ptr[0] = (u32) dma_write_size;
    chunk_ptr[1] = (u32) write_size;
    chunk_ptr[2] = (u32) buf;
    chunk_ptr[3] = (u32) mtd->oobsize;
    chunk_ptr[4] = (u32) chip->oob_poi;
    chunk_ptr[5] = 0;
    chunk_ptr[6] = 0;

//  printk("chunk[2]: %08x, buf mem: %08x, oob mem: %08x\n", chunk_ptr[2], buf, chip->oob_poi);

    chunk = (void *) chunk_ptr + sizeof(uint32_t);


    chip->write_buf(mtd, (u8 *) chunk , dma_write_size);

    /* can debug write by reading back oob */
    //chip->ecc.read_oob(mtd, chip, hsnand->current_page, NAND_READ_ECC);
    //chip->ecc.hwctl(mtd, HSNAND_ECC_OFF);

    return;

}

/*!
  \fn static int ifx_hsnand_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
  \ingroup  IFX_HSNAND_DRV
  \brief write oob area with data
  \param  mtd  MTD device structure
  \param  chip Nand chip device structure
  \param  page The page in the NAND device to write the OOB data to
  \return none
*/

static int ifx_hsnand_write_oob(struct mtd_info *mtd, struct nand_chip *chip,
                                int page)
{
    int i;
    const u8 *buf = chip->oob_poi;
    int length = mtd->oobsize;

    chip->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
    IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) & ~(1<<18), IFX_EBU_NAND_CON);
  
    for (i = 0; i < length; i++) {
        WRITE_NAND(buf[i]); 
    }

    chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);   

    chip->dev_ready(mtd);

    IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) | (1<<18), IFX_EBU_NAND_CON);
    return 0;

}

/*!
  \fn static int ifx_hsnand_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page,
                                    int sndcmd)
  \ingroup  IFX_HSNAND_DRV
  \brief read OOB area to buffer. 
  \param  mtd  MTD device structure
  \param  chip Nand chip device structure
  \param  page The page in the NAND device to read the OOB data from
  \param  sndcmd Command to be sent to the function
  \return none
*/

static int ifx_hsnand_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page,
                               int sndcmd)
{
    struct hsnand_info *hsnand = &hsnand_dev;
    int i;
    //int eccbytes = chip->ecc.bytes;
    u8 *ecc_data = hsnand->sw_ecc_data;
    u8 *buf  = chip->oob_poi;
		 
    if (sndcmd)
    {
        /* For HWECC to calculate correctly, we can only
         * read 3 bytes from the OOB area. Otherwise,
         * the ecc value generated from the HWECC will 
         * be correct for the first time read and wrong
         * for subsequent reads.
         */
		
        if (sndcmd == NAND_READ_ECC)
        {
#if 0
            /* to ensure NAND read via conventional method 
             * works with HSNAND configuration
             */
            IFX_REG_W32(NAND_CON_DEFAULT, IFX_EBU_NAND_CON);

    	    DEBUG(MTD_DEBUG_LEVEL2, "Reading ecc info on page %d . . \n", page);
            IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) & ~(1<<18), IFX_EBU_NAND_CON);

            chip->cmdfunc(mtd, NAND_CMD_READOOB, eccpos[current_eccpos], page);

            /* delay for READOOB cmd*/
            if (hsnand->chip_info == SAMSUNG_512) 
                udelay(10);
#endif

            for (i = 0; i < 3; i++) {
                ecc_data[i] = READ_NAND;
            } 
            /* Need to put the NAND controller to default state */
            IFX_REG_W32(NAND_CON_DEFAULT, IFX_EBU_NAND_CON);

            for (i = 0; i < 3; i++) {
               DEBUG(MTD_DEBUG_LEVEL1, "/ pos: %d, data: %02x /\n", i, ecc_data[i]);
            }

            sndcmd = 0;
            return sndcmd;
        }
        else {
    	    chip->ecc.hwctl(mtd, HSNAND_ECC_OFF);
            chip->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
            chip->dev_ready(mtd);
            sndcmd = 0; 
        }
    }

    for (i = 0; i < mtd->oobsize; i++)
        buf[i] = READ_NAND;        

    return sndcmd;
}

/*!
  \fn static int ifx_hsnand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf)
  \ingroup  IFX_HSNAND_DRV
  \brief Read page raw without ECC
  \param  mtd  MTD device structure
  \param chip  Nand device structre
  \param buf  Buffer to read the data into
  \return none
*/ 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20))
static int ifx_hsnand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page) 
#else
static int ifx_hsnand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf) 
#endif

{
    int read_size = mtd->writesize; 
    int sndcmd = 1;
    int i, eccsize = chip->ecc.size;
	int *eccpos = chip->ecc.layout->eccpos;
	u8 *ecc_read = chip->oob_poi;
    struct hsnand_info *hsnand = &hsnand_dev;

    /* The RAW read should be done in 512B size. Otherwise,
     * there will be some issues generating the correct hardware
     * ECC later on during normal reads. */ 
    if (read_size > READ_SIZE) {
        init_rdwr_hsnand(READ_SIZE, HSNAND_READ, HSNAND_ECC_ON);
        chip->read_buf(mtd, tmp_rd_buf, read_size + mtd->oobsize);
#if 1		
    	memcpy(buf, tmp_rd_buf, read_size);
		printk("debug: %d, fn: %s\n", __LINE__, __func__);
		for (i = 0; i < mtd->oobsize; i++) {
		    ecc_read[i] = tmp_rd_buf[read_size + i];
		}
#endif
        //chip->ecc.read_oob(mtd, mtd->priv, hsnand->current_page, sndcmd);	
		//printk("done read raw\n");
	}
	else {
        for (i = 0; i < read_size/READ_SIZE; i++, buf += eccsize) {
            init_rdwr_hsnand(READ_SIZE, HSNAND_READ, HSNAND_ECC_ON);
            chip->read_buf(mtd, buf, READ_SIZE);  
        }
		chip->ecc.read_oob(mtd, mtd->priv, hsnand->current_page, sndcmd);
	}

    return 0;
}

/*!
  \fn static int ifx_hsnand_correct(struct mtd_info *mtd, u8 *data, u8 *read_ecc,
                                  u8 *calc_ecc)
  \ingroup  IFX_HSNAND_DRV
  \brief Correct ECC error, 1-bit ECC error
  \param  mtd  MTD device structure
  \param data  Data that is needed to be corrected
  \param read_ecc Value of ECC that is read from the NAND device
  \param calc_ecc Value of calculated ECC 
  \return none
*/

static int ifx_hsnand_correct(struct mtd_info *mtd, u8 *data, u8 *read_ecc,
                                  u8 *calc_ecc)
{
    int row, bit_number;
    
    row = GET_ECC_ROW_POSITION;
    bit_number = GET_ECC_BIT_POSITION;
  
    data[row] ^= (1 << bit_number); 
    return 0;

}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20))
static int ifx_hsnand_read_page_swecc(struct mtd_info *mtd, struct nand_chip *chip,
                                      u8 *buf, int page)
#else
static int ifx_hsnand_read_page_swecc(struct mtd_info *mtd, struct nand_chip *chip,
                                      u8 *buf)
#endif
{
    int i, read_size, eccsize = chip->ecc.size;
    int eccbytes = chip->ecc.bytes;
    int eccsteps = chip->ecc.steps;
    int writesize = mtd->writesize;
    int oobsize = mtd->oobsize;
    int *eccpos = chip->ecc.layout->eccpos;
    u8 *ecc_data = chip->buffers->ecccalc;
    //u8 ecc_read[chip->ecc.total];
    u8 *ecc_read = chip->buffers->ecccode;
    u8 *ecc_oob = chip->oob_poi;
    u8 *p = buf;
    u8 *tmp_rd;
    //struct hsnand_info *hsnand = &hsnand_dev;

    read_size = writesize + oobsize;

    init_rdwr_hsnand(eccsize, HSNAND_READ, HSNAND_ECC_ON);
    chip->read_buf(mtd, tmp_rd_buf, read_size);

    tmp_rd = tmp_rd_buf;

    for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize, tmp_rd += eccsize) {
        memcpy(p, tmp_rd, eccsize);
        chip->ecc.calculate(mtd, p, &ecc_data[i]);
        DEBUG(MTD_DEBUG_LEVEL1, "sw ecc: %02x-%02x-%02x\n",
              ecc_data[i+2], ecc_data[i+1], ecc_data[i]);
    }

    for (i = 0; i < chip->ecc.total; i++) {
        ecc_read[i] = tmp_rd_buf[mtd->writesize + eccpos[i]];
        ecc_oob[i] = tmp_rd_buf[mtd->writesize + eccpos[i]];

        if (!((i+1)%3) && i!=0 )
            DEBUG(MTD_DEBUG_LEVEL1, "stored ecc: %02x-%02x-%02x\n",
                  ecc_read[i], ecc_read[i-1], ecc_read[i-2]);
    }
    for (i = 0; i < mtd->oobsize; i++)
	printk("%02x", tmp_rd_buf[mtd->writesize + i]);
    printk("\n");

    p = buf;
    eccsteps = chip->ecc.steps;

    for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
        int stat;
        stat = chip->ecc.correct(mtd, p, &ecc_read[i], &ecc_data[i]);
            if (stat < 0)
                mtd->ecc_stats.failed++;
            else
                mtd->ecc_stats.corrected += stat;
   }
     
   return 0;
}

/*!
  \fn static int ifx_hsnand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
                                      u8 *buf)
  \ingroup  IFX_HSNAND_DRV
  \brief Read page with Hardware checked ECC
  \param  mtd  MTD device structure
  \param chip  Nand device structre
  \param buf  Buffer to read the data into
  \return none
*/
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20))
static int ifx_hsnand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, 
                                      u8 *buf, int page)
#else
static int ifx_hsnand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, 
                                      u8 *buf)
#endif
{
    int i, eccsize = chip->ecc.size;
    int eccbytes = chip->ecc.bytes;
    int eccsteps = chip->ecc.steps;
    int ecc_state = 0;
    struct hsnand_info *hsnand = &hsnand_dev;

    /* set the default ecc position to point to 1st position */
    hsnand->ecc_step = 0;

    /* for nand size bigger than 512 bytes, we have to break the data
     * into chunks of 512 bytes in order to get the correct ECC value as 
     * max software ECC in linux supports up to 512B ECC calculation
     */
    for (i = 0, eccsteps; eccsteps--; i += eccbytes, buf += eccsize)
    {
        init_rdwr_hsnand(eccsize, HSNAND_READ, HSNAND_ECC_ON);
	
        if (mtd->writesize <= READ_SIZE) {         
            chip->read_buf(mtd, tmp_rd_buf, READ_SIZE + 3);
            memcpy(buf, tmp_rd_buf, READ_SIZE);
	}
	else 
	    chip->read_buf(mtd, buf, READ_SIZE);
        
        /* swap software ECC value to align with HWECC value */
        
	DEBUG(MTD_DEBUG_LEVEL3, "hw ecc data: %08x\n", hsnand->hw_ecc_data);
        
        ecc_state = (hsnand->ecc_status & 0x3);
        if (ecc_state) {
            if (!(ecc_state >> 1)) {
    	         DEBUG(MTD_DEBUG_LEVEL1, "Read with bit error/s, ECC state: %d\n", ecc_state);
                 chip->ecc.correct(mtd, buf, NULL, NULL);
                 mtd->ecc_stats.corrected++;
            }
            else if (!(ecc_state >> 2)) {
	         DEBUG(MTD_DEBUG_LEVEL1, 
		    "Read with uncorrectable bit error/s, ECC state: %d\n", ecc_state);
                 mtd->ecc_stats.failed++;
            }
            else {
	        DEBUG(MTD_DEBUG_LEVEL1,
                    "Read with uncorrectable bit error/s, unknown ECC state: %d\n", ecc_state);
	    }
	}

#if 0
        chip->ecc.calculate(mtd, buf, ecc_data);
        printk("re sw ecc: %02x %02x %02x\n", ecc_data[2], ecc_data[1], ecc_data[0]);
        asm("sync");
#endif
    
        /* for page size larger than 512B, we need to update the column location of the 
	 * ndac_1 register to the next 512B location
         */
        if (mtd->writesize > READ_SIZE) {
                /* step to next ecc postion */
                hsnand->ecc_step += 3; 
                /* update ndac address to the next 512 column */
	        hsnand->ndac_ctl_1 += (eccsize << 8);
        }

        memset(chip->oob_poi, 0xff, mtd->oobsize);
    }
   
    return 0;
}

/*!
  \fn static u_char ifx_hsnand_read_byte(struct mtd_info *mtd) 
  \ingroup  IFX_HSNAND_DRV
  \brief Read data byte-by-byte
  \param  mtd  MTD device structure
  \return ret data read 
*/

static u_char ifx_hsnand_read_byte(struct mtd_info *mtd)
{
      u_char ret;

 //     IFX_REG_W32(0x0040c29f,IFX_EBU_BUSCON1);  // EBU BUSCON1
      asm("sync");
      NAND_READ(NAND_READ_DATA, ret);
      asm("sync");
      return ret;

}

/*!
  \fn static void ifx_hsnand_select_chip(struct mtd_info *mtd, int chip)
  \ingroup  IFX_HSNAND_DRV
  \brief Selects NAND chip to enable/disable
  \param  mtd  MTD device structure
  \paran chip  Control chip number. -1 to deselect chip
  \return none
*/

static void ifx_hsnand_select_chip(struct mtd_info *mtd, int chip)
{
    struct nand_chip *nand = mtd->priv;

    switch (chip) {
        case -1:
             NAND_DISABLE_CE(nand);
                 IFX_REG_W32_MASK(IFX_EBU_NAND_CON_NANDM, 0, IFX_EBU_NAND_CON);
             break;
        case 0:
            IFX_REG_W32_MASK(0, IFX_EBU_NAND_CON_NANDM, IFX_EBU_NAND_CON);
             NAND_ENABLE_CE(nand);
             NAND_WRITE(NAND_WRITE_CMD, NAND_WRITE_CMD_RESET); // Reset nand chip
             break;

        default:
             printk("Unknown chip select option\n");
        }
}

/*!
  \fn static int ifx_hsnand_verify_buf(struct mtd_info *mtd, const u8 *buf, int len)
  \ingroup  IFX_HSNAND_DRV
  \brief Function to verify buffer after a write
  \param  mtd MTD device structure
  \param buf Buffer to be verified
  \param len Length of buffer to be verified 
  \return none
*/

static int ifx_hsnand_verify_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
    u8 *tmp_buf = NULL;
    int i;
    struct nand_chip *chip = mtd->priv;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20))
    chip->ecc.read_page_raw(mtd, chip, tmp_buf, 0);
#else
    chip->ecc.read_page_raw(mtd, chip, tmp_buf);
#endif
   
    for (i = 0; i < len; i++)
    {
        if (tmp_buf[i] != buf[i])
            return -EFAULT;
    }
    return 0;
}

/*!
  \fn static void ifx_hsnand_read_buf(struct mtd_info *mtd, u8 *buf, int len)
  \ingroup  IFX_HSNAND_DRV
  \brief Low-level read function to read data from NAND into buffer
  \param  mtd MTD device structure
  \param buf Buffer that is used for storing read data
  \param len Length to be read
  \return none
*/

static void ifx_hsnand_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
    int aligned = 0;
    //unsigned long timeout = 0;
    struct nand_chip *chip = mtd->priv;
    struct dma_device_info *dma_dev = dma_device;
    struct hsnand_info *hsnand = &hsnand_dev;
    int chan = dma_dev->current_rx_chan;

    memset(buf, 0x0, len);
    
    DEBUG(MTD_DEBUG_LEVEL2, "rx_ndac1: %08x, rx_ndac2: %08x\n", 
          IFX_REG_R32(IFX_NDAC_CTL1), IFX_REG_R32(IFX_NDAC_CTL2));

    /* Need to make sure that the buffer provided is 
     * aligned to the burst length of the DMA. 
     * Otherwise the HWECC value will be wrongly
     * calculated 
     */
    if (((u32)buf) & 0x10) {
	DEBUG(MTD_DEBUG_LEVEL1, " ****** buffer not aligned ******** \n");
	memcpy(hsnand->buf_aligned, buf, len);   
        dma_device_desc_setup(dma_dev, (u8 *)hsnand->buf_aligned, len);
        aligned = 0;
    }
    else {
        if (len == (READ_SIZE + 3))
            dma_device_desc_setup(dma_dev, buf, HSNAND_MAX_PAGE_SIZE);
        else
	    dma_device_desc_setup(dma_dev, buf, len);

        aligned = 1;
    }

//    if (len > (READ_SIZE + 3))
//        chip->ecc.hwctl(mtd, HSNAND_ECC_OFF);
//    else  
    if (len <= READ_SIZE + 3)
        chip->ecc.hwctl(mtd, HSNAND_ECC_ON);

    UPDATE_RX_CNT(len);

    dma_dev->rx_chan[chan]->open(dma_dev->rx_chan[chan]);

    /* start reading from NAND device */
    IFX_REG_W32(IFX_REG_R32(IFX_HSMD_CTRL) | 0x00000081, IFX_HSMD_CTRL);

    wait_event_interruptible(hsnand->hsnand_wait, test_bit(HSNAND_EVENT, 
                             &hsnand->wait_flag));
    clear_bit(HSNAND_EVENT, &hsnand->wait_flag);


    /* The reading of stored ECC has to be done straight away too, otherwise 
     * subsequent reads will yield wrong ECC0 register value. On top of that, 
     * this read has to be different from normal OOB reads as we are only 
     * allowed to read 3 bytes. Otherwise, ECC0 register will not reflect the
     * correct ECC value. 
     * Do this for only 512B reads (nand flash with pages > 512B)
     */
    if (len <= READ_SIZE)
        chip->ecc.read_oob(mtd, chip, hsnand->current_page, NAND_READ_ECC);

    /* copy back to buffer */
    if (!aligned)
        memcpy(buf, (u8 *)hsnand->buf_aligned, len);

    chip->ecc.hwctl(mtd, HSNAND_ECC_OFF);

    return;

}

/*!
  \fn static void ifx_hsnand_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
  \ingroup  IFX_HSNAND_DRV
  \brief Low-level write function to write data from buffer to NAND device
  \param  mtd MTD device structure
  \param buf Buffer that is needed to write data into
  \param len Length to be write
  \return none
*/

static void ifx_hsnand_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
   struct dma_device_info *dma_dev = dma_device;
    struct hsnand_info *hsnand = &hsnand_dev;
    int i, chan = dma_dev->current_tx_chan;
    _dma_channel_info *pCh;

    pCh = dma_dev->tx_chan[chan];

    //chip->ecc.hwctl(mtd, HSNAND_ECC_OFF); // if never comment, write will be wrong!!!!!

    /* start writing to NAND device */
    dma_device_write_sdio(dma_dev, (void *) buf - sizeof(uint32_t), NULL);
    IFX_REG_W32(IFX_REG_R32(IFX_HSMD_CTRL) | 0x00000081, IFX_HSMD_CTRL);

    /* enable DMA tx chan interrupt for multi desc write*/
    for (i = 0; i < dma_dev->num_tx_chan; i++)
    {
        if (dma_dev->tx_chan[i]->control == IFX_DMA_CH_ON)
            dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
    }
 
    wait_event_interruptible(hsnand->hsnand_wait, test_bit(HSNAND_EVENT,
                             &hsnand->wait_flag));
    clear_bit(HSNAND_EVENT, &hsnand->wait_flag);

#if 0
    /* wait till NAND access complete */
    while ((IFX_REG_R32(IFX_HSMD_CTRL) & 0x00000200) != 0x00000200) {
    };
#endif

    return;
}

/*!
  \fn static void ifx_hsnand_cmd_ctrl (struct mtd_info *mtd, int data, unsigned int ctrl)
  \ingroup  IFX_HSNAND_DRV
  \brief Control commands that is used to control hardware specific lines
  \param  mtd MTD device structure
  \param data Command that is needed to be sent to the NAND controller
  \param ctrl Control commands that determines to control lines enabled/disabled
  \return none
*/

static void ifx_hsnand_cmd_ctrl (struct mtd_info *mtd, int data, unsigned int ctrl)
{
    struct nand_chip *this = mtd->priv;

    if (ctrl & NAND_CTRL_CHANGE){
         if (ctrl & NAND_CLE) latchcmd=NAND_WRITE_CMD;
         else if(ctrl & NAND_ALE) latchcmd=NAND_WRITE_ADDR;
	 
       }

    if(data != NAND_CMD_NONE){
        *(volatile u8*)((u32)this->IO_ADDR_W | latchcmd)=data;
        while((IFX_REG_R32(IFX_EBU_NAND_WAIT) & IFX_EBU_NAND_WAIT_WR_C) == 0);
      }

    return;

}

/* update hsnand access ctrl register for large pages */
static void update_hsnand_addr_lp(struct mtd_info *mtd, int page_addr, int column, int cmd)
{
    u32 tmp_page, addr_0, addr_1, addr_2, addr_3, addr_4;
    struct hsnand_info *hsnand = &hsnand_dev;
    int writesize = mtd->writesize;

    hsnand->current_page = page_addr;

    if (writesize == 2048)
        tmp_page = page_addr << 12;
    else if (writesize == 4096)
	tmp_page = page_addr << 13;    
    else 
	tmp_page = page_addr << 14;

    if (hsnand->addr_cycle == 4) {
    	addr_0 = column & 0x000000FF;
    	addr_0 = addr_0 << 8;
    	addr_1 = tmp_page & 0x00001FE00;
    	addr_1 = addr_1 << 8;
    	addr_2 = tmp_page & 0x001FE0000;
    	addr_2 = addr_2 << 4;
	addr_2 = addr_2 << 8;
    
    	addr_3 = tmp_page & 0x02000000;
    	addr_3 = addr_3 >> 4;
    	addr_3 = addr_3 >> 8;
    	addr_3 = addr_3 >> 8;
	addr_4 = 0x0;

    	hsnand->ndac_ctl_1 = addr_2 | addr_1 | addr_0 | cmd;
    	hsnand->ndac_ctl_2 = 0x40030000 | addr_4 | addr_3;
    }
    else {
        addr_0 = column & 0x000000FF;
        addr_0 = addr_0 << 8;
        addr_1 = column & 0x00000F00;
        addr_1 = addr_1 << 8;
        addr_2 = tmp_page & 0x000FF000;
        addr_2 = addr_2 << 4;
        addr_2 = addr_2 << 8;

    	addr_3 = tmp_page & 0x0FF00000;
    	addr_3 = addr_3 >> 4;
    	addr_3 = addr_3 >> 16;
    	addr_4 = tmp_page & 0x30000000;
    	addr_4 = addr_4 >> 2;
    	addr_4 = addr_4 >> 18;
    
    	hsnand->ndac_ctl_1 = addr_2 | addr_1 | addr_0 | cmd;
    	hsnand->ndac_ctl_2 = 0x40030000 | addr_4 | addr_3;
    }


    DEBUG(MTD_DEBUG_LEVEL1, "lp: ndac1: %08x, ndac2: %08x @ page: %d\n", 
          hsnand->ndac_ctl_1, hsnand->ndac_ctl_2, page_addr);
    return;
}

/* update hsnand access ctrl register for small nand pages*/
static void update_hsnand_addr_sp(struct nand_chip *chip, int page_addr, int cmd)
{
    u32 addr_0, addr_1, addr_2, addr_3, addr_4;
    struct hsnand_info *hsnand = &hsnand_dev;

    hsnand->current_page = page_addr;

    addr_0 = 0x0;

    addr_1 = page_addr << 8;
    addr_1 = addr_1 << 8;
    addr_2 = (page_addr & 0x001FE0000) << 8;
    addr_2 = addr_2 << 4;
    addr_2 = addr_2 << 8; 
    hsnand->ndac_ctl_1 = addr_2 | addr_1 | addr_0 | cmd;
    
    addr_3 = 0x0;
    addr_4 = 0x0;

    hsnand->ndac_ctl_2 = 0x10010000 | addr_4 | addr_3;

    DEBUG(MTD_DEBUG_LEVEL1, "sp: ndac1: %08x, ndac2: %08x\n @ page: %d\n", 
          hsnand->ndac_ctl_1, hsnand->ndac_ctl_2, page_addr);
    
    return;
}

/*!
 \fn static void ifx_hsnand_command(struct mtd_info *mtd, unsigned int command,
                         int column, int page_addr)
 \brief Send command to NAND device. This function is used for small page
        devices (256/512 Bytes per page)
 \param mtd MTD device structure
 \param command the command to be sent
 \param column the column address for this command, -1 if none
 \param page_addr the page address for this command, -1 if none
 \return none
*/

static void ifx_hsnand_command(struct mtd_info *mtd, unsigned int command,
                         int column, int page_addr)
{
        register struct nand_chip *chip = mtd->priv;
        int ctrl = NAND_CTRL_CLE | NAND_CTRL_CHANGE;
        struct hsnand_info *hsnand = &hsnand_dev;

        /* for commands with NAND_CMD_SEQIN or NAND_CMD_READ0 with 
         * column bigger than pagesize are meant for oob reads which
         * we still want it to do in normal nand mode. 
         */
		 		 
        /* write page command */
        if ((command == NAND_CMD_SEQIN) && (column < mtd->writesize)) 
        {
            if (mtd->writesize <= 512)
                update_hsnand_addr_sp(chip, page_addr, HSNAND_WRITE);
            else 
                update_hsnand_addr_lp(mtd, page_addr, column, HSNAND_WRITE);

	    return;
        }
        /* read page command */
        if (command == NAND_CMD_READ0 && (column < mtd->writesize))
        {
            if (mtd->writesize <= 512)
                update_hsnand_addr_sp(chip, page_addr, HSNAND_READ);
            else 
                update_hsnand_addr_lp(mtd, page_addr, column, HSNAND_READ);

	    return;
        }   

        /*
         * Write out the command to the device.
         */
        if (command == NAND_CMD_SEQIN) {
                int readcmd;

                if (column >= mtd->writesize) {
                        /* OOB area */
                        column -= mtd->writesize;
                        readcmd = NAND_CMD_READOOB;
                } else if (column < 256) {
                        /* First 256 bytes --> READ0 */
                        readcmd = NAND_CMD_READ0;
                } else {
                        column -= 256;
                        readcmd = NAND_CMD_READ1;
                }
                chip->cmd_ctrl(mtd, readcmd, ctrl);
                ctrl &= ~NAND_CTRL_CHANGE;
        }
        chip->cmd_ctrl(mtd, command, ctrl);

        /*
         * Address cycle, when necessary
         */
        ctrl = NAND_CTRL_ALE | NAND_CTRL_CHANGE;

        /* Samsung requirements for the chip during OOB access */
        if ((command == NAND_CMD_READOOB) && hsnand->chip_info == SAMSUNG_512)
            ndelay(10);

        /* Serially input address */
        if (column != -1) {
                /* Adjust columns for 16 bit buswidth */
                if (chip->options & NAND_BUSWIDTH_16)
                        column >>= 1;
                chip->cmd_ctrl(mtd, column, ctrl);
                ctrl &= ~NAND_CTRL_CHANGE;
        }
        if (page_addr != -1) {
                chip->cmd_ctrl(mtd, page_addr, ctrl);
                ctrl &= ~NAND_CTRL_CHANGE;
                chip->cmd_ctrl(mtd, page_addr >> 8, ctrl);
                /* One more address cycle for devices > 32MiB */
                if (chip->chipsize > (32 << 20))
                        chip->cmd_ctrl(mtd, page_addr >> 16, ctrl);
        }
        chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

        /*
         * program and erase have their own busy handlers
         * status and sequential in needs no delay
         */
        switch (command) {

        case NAND_CMD_PAGEPROG:
        case NAND_CMD_ERASE1:
        case NAND_CMD_ERASE2:
        case NAND_CMD_SEQIN:
        case NAND_CMD_STATUS:
                return;

        case NAND_CMD_RESET:
                if (chip->dev_ready)
                        break;
                udelay(chip->chip_delay);
                chip->cmd_ctrl(mtd, NAND_CMD_STATUS,
                               NAND_CTRL_CLE | NAND_CTRL_CHANGE);
                chip->cmd_ctrl(mtd,
                               NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
                while (!(chip->read_byte(mtd) & NAND_STATUS_READY)) ;
                return;

                /* This applies to read commands */
 
        //case NAND_CMD_READOOB:
                //return;

        default:
                /*
                 * If we don't have access to the busy pin, we apply the given
                 * command delay
                 */
                if (!chip->dev_ready) {
                        udelay(chip->chip_delay);
                        return;
                }
        }
        /* Apply this short delay always to ensure that we do wait tWB in
         * any case on any machine. */
        ndelay(100);

        nand_wait_ready(mtd);
}


/*!
 \fn static void ifx_hsnand_command_lp(struct mtd_info *mtd, unsigned int command,
                            int column, int page_addr)
 \brief Send command to NAND device. This is the version for the new large page
        devices We dont have the separate regions as we have in the small page
        devices.  We must emulate NAND_CMD_READOOB to keep the code compatible.
 \param mtd MTD device structure
 \param command the command to be sent
 \param column  the column address for this command, -1 if none
 \param page_addr the page address for this command, -1 if none
 \return none
 */

static void ifx_hsnand_command_lp(struct mtd_info *mtd, unsigned int command,
                            int column, int page_addr)
{
        register struct nand_chip *chip = mtd->priv;
        
        /* write page command */
        if ((command == NAND_CMD_SEQIN) && (column < mtd->writesize))
        {
            if (mtd->writesize <= 512)
                update_hsnand_addr_sp(chip, page_addr, HSNAND_WRITE);
            else 
                update_hsnand_addr_lp(mtd, page_addr, column, HSNAND_WRITE);

            return;
        }
        /* read page command */
        if (command == NAND_CMD_READ0 && (column < mtd->writesize))
        {
            if (mtd->writesize <= 512)
                update_hsnand_addr_sp(chip, page_addr, HSNAND_READ);
            else 
                update_hsnand_addr_lp(mtd, page_addr, column, HSNAND_READ);

            return;
        }

        /* Emulate NAND_CMD_READOOB */
        if (command == NAND_CMD_READOOB) {
                column += mtd->writesize;
                command = NAND_CMD_READ0;
        }
        
        asm("sync");	
	NAND_CE_SET;
        asm("sync");

        /* Command latch cycle */
        chip->cmd_ctrl(mtd, command & 0xff,
                       NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
	asm("sync");

        if (column != -1 || page_addr != -1) {
                int ctrl = NAND_CTRL_CHANGE | NAND_ALE | NAND_NCE;
		NAND_ALE_SET;

		//ndelay(100);

        	DEBUG(MTD_DEBUG_LEVEL2, "Reading @ column 0x%x, page: %d\n", 
                      column, page_addr);

                /* Serially input address */
                if (column != -1) {
                        /* Adjust columns for 16 bit buswidth */
                        if (chip->options & NAND_BUSWIDTH_16)
                                column >>= 1;
                        chip->cmd_ctrl(mtd, column & 0xff, ctrl);
                        ctrl &= ~NAND_CTRL_CHANGE;
                        chip->cmd_ctrl(mtd, (column >> 8) & 0xff, ctrl);
                }
                if (page_addr != -1) {
                        chip->cmd_ctrl(mtd, page_addr & 0xff, ctrl);
                        chip->cmd_ctrl(mtd, (page_addr >> 8) & 0xff,
                                       ctrl);
                        /* One more address cycle for devices > 128MiB */
                        if (chip->chipsize > (128 << 20))
                                chip->cmd_ctrl(mtd, (page_addr >> 16) & 0xff,
                                               NAND_NCE | NAND_ALE);
		NAND_ALE_CLEAR;
	        }
        }

        chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

        /*
         * program and erase have their own busy handlers
         * status, sequential in, and deplete1 need no delay
         */
        switch (command) {

        case NAND_CMD_CACHEDPROG:
        case NAND_CMD_PAGEPROG:
        case NAND_CMD_ERASE1:
        case NAND_CMD_ERASE2:
        case NAND_CMD_SEQIN:
        case NAND_CMD_RNDIN:
        case NAND_CMD_STATUS:
        case NAND_CMD_DEPLETE1:
                return;

                /*
                 * read error status commands require only a short delay
                 */
        case NAND_CMD_STATUS_ERROR:
        case NAND_CMD_STATUS_ERROR0:
        case NAND_CMD_STATUS_ERROR1:
        case NAND_CMD_STATUS_ERROR2:
        case NAND_CMD_STATUS_ERROR3:
                udelay(chip->chip_delay);
                return;

        case NAND_CMD_RESET:
                if (chip->dev_ready)
                        break;
                udelay(chip->chip_delay);
                chip->cmd_ctrl(mtd, NAND_CMD_STATUS,
                               NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
                chip->cmd_ctrl(mtd, NAND_CMD_NONE,
                               NAND_NCE | NAND_CTRL_CHANGE);
                while (!(chip->read_byte(mtd) & NAND_STATUS_READY)) ;
                return;

        case NAND_CMD_RNDOUT:
                /* No ready / busy check necessary */
                chip->cmd_ctrl(mtd, NAND_CMD_RNDOUTSTART,
                               NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
                chip->cmd_ctrl(mtd, NAND_CMD_NONE,
                               NAND_NCE | NAND_CTRL_CHANGE);
                return;
	
        case NAND_CMD_READ0:
                /* This applies to read commands, newer high density 
                 * flash device needs a 2nd read cmd for READ0. 
                 */
                chip->cmd_ctrl(mtd, NAND_CMD_READSTART,
                               NAND_CLE | NAND_CTRL_CHANGE);

		chip->dev_ready(mtd);
		return;



        default:
                /*
                 * If we don't have access to the busy pin, we apply the given
                 * command delay
                 */
                if (!chip->dev_ready) {
                        udelay(chip->chip_delay);
                        return;
                }
        }

        /* Apply this short delay always to ensure that we do wait tWB in
         * any case on any machine. */
        ndelay(100);

        nand_wait_ready(mtd);
}

/* buffer alloc for DMA*/
static u8* ifx_hsnand_buffer_alloc(int len, int *byte_offset, void **opt)
{
#if 0
    u8 *tmp;
    struct hsnand_info *hsnand = &hsnand_dev;

    tmp = hsnand->rx_buf;
    hsnand->rx_buf = hsnand->rx_buf2;
    hsnand->rx_buf2 = tmp;
 
    return hsnand->rx_buf;
#endif

    return NULL;
}

/* free buffer */
static int ifx_hsnand_buffer_free(u8 *dataptr, void *opt)
{
    return 0;
}

/* initialize DMA */
static int hsnand_dma_setup(struct mtd_info *mtd)
{

    int i, rx_packet_size;
    
    if (mtd->writesize <= READ_SIZE)
        rx_packet_size = HSNAND_MAX_PAGE_SIZE;
    else
	rx_packet_size = mtd->writesize + mtd->oobsize;

    dma_device = dma_device_reserve("HSNAND");
    if (dma_device == NULL)
    {
        printk("Reserve DMA for HSNAND failed!\n");
	return -EAGAIN;
    } 

    dma_device->intr_handler = &ifx_hsnand_dma_intr_handler;
    dma_device->buffer_alloc = &ifx_hsnand_buffer_alloc;
    dma_device->buffer_free = &ifx_hsnand_buffer_free;
    dma_device->tx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_device->rx_endianness_mode = IFX_DMA_ENDIAN_TYPE3;
    dma_device->tx_burst_len = HSNAND_DMA_BURST_LEN;
    dma_device->rx_burst_len = HSNAND_DMA_BURST_LEN;
    dma_device->num_rx_chan = 1;
    dma_device->num_tx_chan = 1;
    
    /* DMA Channel Config for TX direction */ 
    for (i = 0; i < dma_device->num_tx_chan; i++) 
    {
        dma_device->tx_chan[i]->desc_len = 2;
        dma_device->tx_chan[i]->byte_offset = 0;
	//dma_device->tx_chan[i]->packet_size = HSNAND_PAGE_SIZE;
        dma_device->tx_chan[i]->control = IFX_DMA_CH_ON;
    }  

    /* DMA Channel Config for RX direction */
    for (i = 0; i < dma_device->num_rx_chan; i++)
    {
        dma_device->rx_chan[i]->desc_len = 1;
        dma_device->rx_chan[i]->byte_offset = 0;
	//dma_device->rx_chan[i]->packet_size = HSNAND_PAGE_SIZE;
	dma_device->rx_chan[i]->packet_size = rx_packet_size;
        dma_device->rx_chan[i]->control = IFX_DMA_CH_ON;
    }
   
    dma_device->current_tx_chan = 0;
    dma_device->current_rx_chan = 0;
 
    i = dma_device_register(dma_device);
    if (i != IFX_SUCCESS) {
    	printk(KERN_ERR "%s[%d]: DMA register failed!\n", __func__, __LINE__);    
        return -EAGAIN;
    }
#if 1   
    for (i = 0; i < dma_device->num_rx_chan; i++) {
        dma_device->rx_chan[i]->reset(dma_device->rx_chan[i]);  
        dma_device->rx_chan[i]->close(dma_device->rx_chan[i]);
    }
#endif
 
    return 0; 

}

//GPIO initialization 
int ifx_hsnand_chip_init(void)
{
    u32 reg;
    int retval = 0;
    
    /* FL_CS1 to use for output */
    ifx_gpio_pin_reserve(IFX_NAND_CS1, IFX_GPIO_MODULE_NAND);
    ifx_gpio_dir_out_set(IFX_NAND_CS1, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel0_set(IFX_NAND_CS1, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel1_clear(IFX_NAND_CS1, IFX_GPIO_MODULE_NAND);
    ifx_gpio_open_drain_set(IFX_NAND_CS1, IFX_GPIO_MODULE_NAND);

    /* FL_A23 NAND_CLE used as output */
    ifx_gpio_pin_reserve(IFX_NAND_CLE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_dir_out_set(IFX_NAND_CLE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel0_set(IFX_NAND_CLE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel1_clear(IFX_NAND_CLE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_open_drain_set(IFX_NAND_CLE, IFX_GPIO_MODULE_NAND);

    /* FL_A24 NAND_ALE used as output */
    ifx_gpio_pin_reserve(IFX_NAND_ALE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_dir_out_set(IFX_NAND_ALE, IFX_GPIO_MODULE_NAND);
    	ifx_gpio_altsel0_set(IFX_NAND_ALE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel1_clear(IFX_NAND_ALE, IFX_GPIO_MODULE_NAND);
    ifx_gpio_open_drain_set(IFX_NAND_ALE, IFX_GPIO_MODULE_NAND);

    /* FL_A48 NAND Read Busy used as input*/
    ifx_gpio_pin_reserve(IFX_NAND_RDY, IFX_GPIO_MODULE_NAND);
    ifx_gpio_dir_in_set(IFX_NAND_RDY, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel0_set(IFX_NAND_RDY, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel1_clear(IFX_NAND_RDY, IFX_GPIO_MODULE_NAND);

    /* FL_A49 NAND READ used as output */
    ifx_gpio_pin_reserve(IFX_NAND_RD, IFX_GPIO_MODULE_NAND);
    ifx_gpio_dir_out_set(IFX_NAND_RD, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel0_set(IFX_NAND_RD, IFX_GPIO_MODULE_NAND);
    ifx_gpio_altsel1_clear(IFX_NAND_RD, IFX_GPIO_MODULE_NAND);
    ifx_gpio_open_drain_set(IFX_NAND_RD, IFX_GPIO_MODULE_NAND);
                   
     /* byte swap;minimum delay*/
    reg = IFX_EBU_BUSCON1_SETUP | 
          SM(IFX_EBU_BUSCON1_ALEC3,IFX_EBU_BUSCON1_ALEC) |
          SM(IFX_EBU_BUSCON1_BCGEN_RES,IFX_EBU_BUSCON1_BCGEN) |
          SM(IFX_EBU_BUSCON1_WAITWRC2,IFX_EBU_BUSCON1_WAITWRC) |
          SM(IFX_EBU_BUSCON1_WAITRDC2,IFX_EBU_BUSCON1_WAITRDC) |
          SM(IFX_EBU_BUSCON1_HOLDC1,IFX_EBU_BUSCON1_HOLDC) |
          SM(IFX_EBU_BUSCON1_RECOVC1,IFX_EBU_BUSCON1_RECOVC) |
          SM(IFX_EBU_BUSCON1_CMULT4,IFX_EBU_BUSCON1_CMULT);
    IFX_REG_W32(reg, IFX_EBU_BUSCON1); /* BUSCON1: 0x0040F267 */

    reg = SM(IFX_EBU_NAND_CON_NANDM_ENABLE, IFX_EBU_NAND_CON_NANDM) |
          SM(IFX_EBU_NAND_CON_CSMUX_E_ENABLE,IFX_EBU_NAND_CON_CSMUX_E) |
          SM(IFX_EBU_NAND_CON_CS_P_LOW,IFX_EBU_NAND_CON_CS_P) |
          SM(IFX_EBU_NAND_CON_SE_P_LOW,IFX_EBU_NAND_CON_SE_P) |
          SM(IFX_EBU_NAND_CON_WP_P_LOW,IFX_EBU_NAND_CON_WP_P) |
          SM(IFX_EBU_NAND_CON_PRE_P_LOW,IFX_EBU_NAND_CON_PRE_P) |
          SM(IFX_EBU_NAND_CON_IN_CS1,IFX_EBU_NAND_CON_IN_CS) |
          SM(IFX_EBU_NAND_CON_OUT_CS1,IFX_EBU_NAND_CON_OUT_CS);
    IFX_REG_W32(reg,IFX_EBU_NAND_CON); /* 0x000005F3*/

    asm("sync");
    init_ecc();
    asm("sync");

     asm("sync");
     /* Set bus signals to inactive */
     NAND_WRITE(NAND_WRITE_CMD, NAND_WRITE_CMD_RESET); // Reset nand chip

    return retval;
     
}

#if 0
static int hsnand_bad_block(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	return 0;
}

int hsnand_scan_bbt(struct mtd_info *mtd)
{

    return 0;
}
#endif

void init_ecc(void)
{
       u32 reg = 0x0;

       reg = (NAND_BASE_ADDRESS & 0x1fffff00)| IFX_EBU_ADDSEL1_MASK(2) |
              IFX_EBU_ADDSEL1_REGEN;
       IFX_REG_W32(reg,IFX_EBU_ADDSEL1); /* ADDSEL1: 0x14000021*/
       reg = 0x0;
       reg = SM(IFX_HSNAND_FSM_ENABLED, IFX_HSNAND_FSM) |
             SM(IFX_HSNAND_ENR_NONE, IFX_HSNAND_ENR);
       IFX_REG_W32(reg, IFX_HSMD_CTRL);
       printk("IFX_HSMD_CTRL: %08x\n", IFX_REG_R32(IFX_HSMD_CTRL));

       IFX_REG_W32(reg,IFX_EBU_NAND_ECC0);
       asm("sync");
       reg = SM(IFX_EBU_NAND_CON_OUT_CS1, IFX_EBU_NAND_CON_OUT_CS) |
             SM(IFX_EBU_NAND_CON_IN_CS1, IFX_EBU_NAND_CON_IN_CS) |
             IFX_EBU_NAND_CON_PRE_P | IFX_EBU_NAND_CON_WP_P |
             IFX_EBU_NAND_CON_SE_P | IFX_EBU_NAND_CON_CS_P |
             SM(IFX_EBU_NAND_CON_CLE_P_HIGH, IFX_EBU_NAND_CON_CLE_P) |
             SM(IFX_EBU_NAND_CON_ALE_P_HIGH, IFX_EBU_NAND_CON_ALE_P) |
             IFX_EBU_NAND_CON_CSMUX_E | IFX_EBU_NAND_CON_NANDM;
       IFX_REG_W32(reg, IFX_EBU_NAND_CON);

       IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_CON) | (1<<31), IFX_EBU_NAND_CON);
       asm("sync");
       reg &= 0x0;
       asm("sync");
       reg = IFX_REG_R32(IFX_EBU_NAND_ECC_CR) & ~(3<<14);
       asm("sync");
       IFX_REG_W32(reg,IFX_EBU_NAND_ECC_CR);
       asm("sync");
       IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_ECC_CR) | (1<<14), IFX_EBU_NAND_ECC_CR);
       asm("sync");
       IFX_REG_W32(IFX_REG_R32(IFX_EBU_NAND_ECC_CR) | (1<<31), IFX_EBU_NAND_ECC_CR);
       asm("sync");
       DEBUG(MTD_DEBUG_LEVEL2, "NAND_CON: 0x%08x, NAND_ECC_CR: 0x%08x\n", 
             IFX_REG_R32(IFX_EBU_NAND_CON), IFX_REG_R32(IFX_EBU_NAND_ECC_CR));

}

u32 get_chip_info(struct mtd_info *mtd)
{
    int addr_cycle_count = 0;
    int writesize = mtd->writesize;
    int chipshift, pageshift;
    u8 maf_id, chip_id;
    u32 addr_cycle;
    struct hsnand_info *hsnand = &hsnand_dev;
    struct nand_chip *chip = mtd->priv;

    chipshift = chip->chip_shift;
    pageshift = chip->page_shift;
    
    for (; writesize > 0; ) {
	addr_cycle_count++;
	writesize >>= 10;
    } 
    
    addr_cycle = chipshift - (pageshift + 1);
    for (; addr_cycle > 0; ) {
        addr_cycle_count++;
        addr_cycle >>= 2;
    }
    hsnand->addr_cycle = addr_cycle_count;

    chip->select_chip(mtd, 0);
    chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);
    maf_id = chip->read_byte(mtd);
    chip_id = chip->read_byte(mtd);

    return (chip_id | (maf_id << 8));

}

/*!
  \fn int ifx_nand_init(void)
  \ingroup  IFX_HSNAND_DRV
  \brief  Main initialization routine
  \param  none
  \return error message
*/

static int __init ifx_hsnand_init(void)
{
    struct nand_chip *this;
    struct hsnand_info *hsnand = &hsnand_dev;
    struct nand_extra_info *chip_type = NULL;
    int i, err = 0;

    /* clear address */
    hsnand->ndac_ctl_1 = 0x0;
    hsnand->ndac_ctl_2 = 0x0;
    hsnand->current_page = 0x0;
 
    printk("Initializing HSNAND driver\n");
    ifx_hsnand_chip_init();
 
    ifx_hsnand_mtd = kmalloc(sizeof(struct mtd_info) + 
                             sizeof (struct nand_chip), GFP_KERNEL);
    if (!ifx_hsnand_mtd)
    {
        printk("Unable to allocate HSNAND MTD device structure\n");
        err = -ENOMEM;
        return err;
    }
  
    hsnand->buf_aligned = kmalloc(HSNAND_PAGE_SIZE, GFP_ATOMIC);       
	
    this = (struct nand_chip *)(&ifx_hsnand_mtd[1]);
    memset(ifx_hsnand_mtd, 0, sizeof(struct mtd_info));
    memset(this, 0, sizeof(struct nand_chip));
 
    ifx_hsnand_mtd->name = (char *) kmalloc(16, GFP_KERNEL);
    if (ifx_hsnand_mtd->name == NULL)
    {
        printk("Unable to allocate HSNAND MTD device name\n");
        err = -ENOMEM;
        goto out;
    }

    memset(ifx_hsnand_mtd, 0, 16);
    sprintf((char *)ifx_hsnand_mtd->name, IFX_MTD_HSNAND_BANK_NAME);

    /* Associate MTD priv members with the current MTD info*/
    ifx_hsnand_mtd->priv = this;
    ifx_hsnand_mtd->owner = THIS_MODULE;

    this->IO_ADDR_R = (void *) NAND_BASE_ADDRESS;
    this->IO_ADDR_W = (void *) NAND_BASE_ADDRESS;
    this->cmd_ctrl = ifx_hsnand_cmd_ctrl;
    
    /* 30 us command delay, similar to NAND driver specs */
    this->chip_delay = 30;    
    /* hw ecc specific read/write functions */
    this->ecc.mode = NAND_ECC_HW;
    this->ecc.hwctl = ifx_hsnand_hwctl;
    this->ecc.write_page_raw = ifx_hsnand_write_page_raw;
    this->ecc.write_page = ifx_hsnand_write_page_hwecc;
    this->ecc.read_page_raw = ifx_hsnand_read_page_raw;
    this->ecc.read_page = ifx_hsnand_read_page_hwecc;
    this->ecc.read_oob = ifx_hsnand_read_oob; //can leave the write to default
    //this->ecc.correct = ifx_hsnand_correct;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,20))
    this->ecc.calculate = nand_calculate_ecc;
#else
    this->ecc.calculate = ifx_nand_calculate_ecc;
#endif
    
    this->options = NAND_USE_FLASH_BBT | NAND_SKIP_BBTSCAN;

#if 0
    this->scan_bbt = hsnand_scan_bbt;
    this->block_bad = hsnand_bad_block;
#endif
   
    this->read_byte = ifx_hsnand_read_byte;
    this->read_buf = ifx_hsnand_read_buf;
    this->write_buf = ifx_hsnand_write_buf;
    this->ecc.write_oob = ifx_hsnand_write_oob;
    this->verify_buf = ifx_hsnand_verify_buf;
    this->dev_ready = ifx_hsnand_ready;
    this->select_chip = ifx_hsnand_select_chip;
    this->cmdfunc = ifx_hsnand_command; 
    this->ecc.bytes = HSNAND_ECC_BYTES;
    this->ecc.size = HSNAND_ECCSIZE;

    err = hsnand_dma_setup(ifx_hsnand_mtd);
    if (err < 0)
    {
        printk("HSNAND DMA setup failed\n");
        goto out;
    }
	
    hsnand->chip_info = get_chip_info(ifx_hsnand_mtd);
    DEBUG(MTD_DEBUG_LEVEL1, "Chip id: %08x\n", hsnand->chip_info);
    
    if ((hsnand->chip_info & 0x0000ff00) == SAMSUNG)
        hsnand->chip_id = 0;
    else if ((hsnand->chip_info & 0x0000ff00) == QIMONDA || 
                  (hsnand->chip_info & 0x0000ff00) == INFINEON)
        hsnand->chip_id = 1;
    else if ((hsnand->chip_info & 0x0000ff00) == STM)
        hsnand->chip_id = 2;
    
    for (i = 0; chip_extra_info[i].chip_id != 0xFFFF ; i++) {
        if (hsnand->chip_info == chip_extra_info[i].chip_id) {
            chip_type = &chip_extra_info[i];
            break;
        }
    }

    if (chip_type->addr_cycle > 3)
	 this->ecc.size = 256;

    printk("Probe for NAND Flash. . \n");
    if (nand_scan(ifx_hsnand_mtd, 1))
    {
        printk("Probing for NAND flash failed, flash not found!\n");
        err = -ENXIO;
        goto out;
    }
	
    init_waitqueue_head(&hsnand->hsnand_wait);	

    /* Allocate buffer for write access. Used for buffer manipulation 
     * so as to allow writing data and OOB in a single DMA transaction 
     */
    tmp_wr_buf = kmalloc(ifx_hsnand_mtd->writesize +
                         ifx_hsnand_mtd->oobsize, GFP_ATOMIC);

    if (tmp_wr_buf == NULL) 
    {
        printk("Unable to allocate HSNAND write buffer!\n");
        err = -ENOMEM;
        goto out;
    }
    write_ptr = tmp_wr_buf;
	
	if (ifx_hsnand_mtd->writesize > READ_SIZE)
        tmp_rd_buf = kmalloc(ifx_hsnand_mtd->writesize + 
                             ifx_hsnand_mtd->oobsize, GFP_KERNEL);
    else
        tmp_rd_buf = kmalloc(HSNAND_MAX_PAGE_SIZE, GFP_KERNEL);

    if (tmp_rd_buf == NULL)
    {
        printk("Unable to allocate HSNAND read buffer!\n");
        err = -ENOMEM;
        goto out;
    } 
    
    printk("Setting up command ctrl & ecc layout\n");
    if (ifx_hsnand_mtd->writesize <= READ_SIZE)
    {
        if (chip_type == NULL)
            chip_type = &chip_extra_info[i];

	this->ecc.correct = ifx_hsnand_correct;	
        this->cmdfunc = ifx_hsnand_command; 
        this->ecc.steps = 1;
        this->ecc.layout = chip_type->chip_ecclayout;
        this->bbt_td = chip_type->chip_bbt_main_descr;
        this->bbt_md = chip_type->chip_bbt_mirror_descr;
        DEBUG(MTD_DEBUG_LEVEL2, "Command ctrl set for writesize %d, oobsize %d\n", 
	      ifx_hsnand_mtd->writesize, ifx_hsnand_mtd->oobsize);
    }
    else if (ifx_hsnand_mtd->writesize == 2048)
    {
        if (chip_type == NULL)
            chip_type = &chip_extra_info[i+1];

        this->cmdfunc = ifx_hsnand_command_lp; 
        this->ecc.correct = nand_correct_data;
        //this->ecc.steps = 4;
        this->ecc.layout = chip_type->chip_ecclayout;
	this->ecc.size = 256;
        this->ecc.read_page = ifx_hsnand_read_page_swecc;
        this->bbt_td = chip_type->chip_bbt_main_descr;
        this->bbt_md = chip_type->chip_bbt_mirror_descr;
        DEBUG(MTD_DEBUG_LEVEL2, "Command ctrl set for writesize %d, oobsize %d\n", 
              ifx_hsnand_mtd->writesize, ifx_hsnand_mtd->oobsize);
    }    
	
	/* scan & create a bbt table where appropriate*/
    this->scan_bbt(ifx_hsnand_mtd);

#ifdef CONFIG_MTD_PARTITIONS
#ifdef CONFIG_MTD_CMDLINE_PARTS
    int n = 0;
    struct mtd_partition *mtd_parts = NULL;

    n = parse_mtd_partitions(ifx_hsnand_mtd, part_probes, &mtd_parts, 0);
    if (n <= 0)
    {
        kfree(ifx_hsnand_mtd);
        return err;
    }
    add_mtd_partitions(ifx_hsnand_mtd, mtd_parts, n);
#else
    err = add_mtd_partitions(ifx_hsnand_mtd, g_ifx_mtd_nand_partitions, g_ifx_mtd_nand_partion_num);
#endif /* CONFIG_MTD_CMDLINE_PARTS */
    err = add_mtd_device(ifx_hsnand_mtd);
#endif /* CONFIG_MTD_PARTITIONS */

    printk("chip eccsize: %d\n", this->ecc.layout->eccbytes);
    hsnand->mtd_prv = ifx_hsnand_mtd;
    printk("Success in initializing HSNAND\n");


out:
    return err;

}

static void __exit ifx_hsnand_exit(void) 
{
    // NAND_ALE
    ifx_gpio_pin_free(IFX_NAND_ALE, IFX_GPIO_MODULE_NAND);
    // NAND_CLE
    ifx_gpio_pin_free(IFX_NAND_CLE, IFX_GPIO_MODULE_NAND);
    // CS1
    ifx_gpio_pin_free(IFX_NAND_CS1, IFX_GPIO_MODULE_NAND);
    // READY
    ifx_gpio_pin_free(IFX_NAND_RDY, IFX_GPIO_MODULE_NAND);
    // READ
    ifx_gpio_pin_free(IFX_NAND_RD, IFX_GPIO_MODULE_NAND);

    dma_device_release(dma_device);
    dma_device_unregister(dma_device);

    /* Release resources, unregister device */
    nand_release (ifx_hsnand_mtd);

    /* Free the MTD device structure */
    kfree (ifx_hsnand_mtd);   
    kfree(tmp_wr_buf); 
	kfree(tmp_rd_buf);
}

module_init(ifx_hsnand_init);
module_exit(ifx_hsnand_exit);



