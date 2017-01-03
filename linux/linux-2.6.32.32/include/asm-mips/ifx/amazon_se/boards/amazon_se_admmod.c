/******************************************************************************
**
** FILE NAME    : admmod.c
** PROJECT      : Danube
** MODULES      : ADM6996
**
** DATE         : 1 SEP 2004
** AUTHOR       : Joe Lin
** DESCRIPTION  : ADM6996 Switch Driver
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
*******************************************************************************/

/******************************************************************************
**  2009/12/17  Lantiq Inc. Revised by Richard Hsu
**  Rename to amazon_se_admmod.c.
**  We use GPIO to simulate MDIO/MDC operations on AmazonSE WAVE board.
**  ASE WAVE board uses PHY2PHY connection to Tantos 0G 
** 
*******************************************************************************/  



#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <asm/atomic.h>


#include <asm/ifx/amazon_se/amazon_se_admmod.h>
#include <asm/ifx/amazon_se/amazon_se.h>

// Using GPIO to access Tantos as default
#define CONFIG_SWITCH_GPIO_ACCESS

#ifdef CONFIG_SWITCH_GPIO_ACCESS
  #undef CONFIG_SWITCH_ADM6996_MDIO
#endif 

#define ADM_SW_IOCTL_PORTSTS    _IO(ADM_MAGIC, KEY_IOCTL_ADM_SW_PORTSTS)
#define ADM_SW_IOCTL_INIT 	_IO(ADM_MAGIC, KEY_IOCTL_ADM_SW_INIT)

/*
  initialize GPIO pins.
  output mode, low
*/

// We use GPIO24/27 on ASE WAVE board for simulation
#define ASE_TANTOS_MDIO  0x00000100
#define ASE_TANTOS_MDC  0x00000800

#define ASE_TANTOS_MDIO_COMPLEMENT  0xfffffeff
#define ASE_TANTOS_MDC_COMPLEMENT   0xfffff7ff
#define TANTOS_SW_MDC_UP_DELAY      5
#define TANTOS_SW_MDC_DOWN_DELAY    5

void ifx_gpio_init(void)
{
 //GPIO24: MDIO P1.8 
 //GPIO27: MDC  P1.11

/* 
 P1.8 as GPIO Out: 
  AMAZON_SE_GPIO_P1_ALTSEL0.8=0
  AMAZON_SE_GPIO_P1_ALTSEL1.8=0
  AMAZON_SE_GPIO_P1_DIR.8 = 1
  
 P1.8 as MDIO pin: 
  AMAZON_SE_GPIO_P1_ALTSEL0.8=1
  AMAZON_SE_GPIO_P1_ALTSEL1.8=1
  AMAZON_SE_GPIO_P1_DIR.8 =1 
  
P1.11 as GPIO Out:  
  AMAZON_SE_GPIO_P1_ALTSEL0.8=0
  AMAZON_SE_GPIO_P1_ALTSEL1.8=0
  AMAZON_SE_GPIO_P1_DIR.8 =1
  
P1.11 as MDC pin: 
  AMAZON_SE_GPIO_P1_ALTSEL0.8=1
  AMAZON_SE_GPIO_P1_ALTSEL1.8=1
  AMAZON_SE_GPIO_P1_DIR.8 =1 
*/ 
 

 *(AMAZON_SE_GPIO_P1_OUT) |= (ASE_TANTOS_MDC|ASE_TANTOS_MDIO);
 *(AMAZON_SE_GPIO_P1_ALTSEL0) &= ASE_TANTOS_MDC_COMPLEMENT;  
 *(AMAZON_SE_GPIO_P1_ALTSEL0) &= ASE_TANTOS_MDIO_COMPLEMENT;
 *(AMAZON_SE_GPIO_P1_ALTSEL1) &= ASE_TANTOS_MDC_COMPLEMENT;
 *(AMAZON_SE_GPIO_P1_ALTSEL1) &= ASE_TANTOS_MDIO_COMPLEMENT;
 *(AMAZON_SE_GPIO_P1_DIR) |= (ASE_TANTOS_MDC|ASE_TANTOS_MDIO);

}

void gpio_info()
{
/*
 printk("*(AMAZON_SE_GPIO_P1_ALTSEL0) is %04x\n",*(AMAZON_SE_GPIO_P1_ALTSEL0));	
 printk("*(AMAZON_SE_GPIO_P1_ALTSEL1) is %04x\n",*(AMAZON_SE_GPIO_P1_ALTSEL1));	
 printk("*(AMAZON_SE_GPIO_P1_DIR) is %04x\n\n",*(AMAZON_SE_GPIO_P1_DIR));	
*/
}
/* read one bit from mdio port */
int ifx_sw_mdio_readbit(void)
{
    //GPIO24 as MDIO 
    return *(AMAZON_SE_GPIO_P1_IN)& 0x0100 ;	
}

/*
  MDIO mode selection
  1 -> output
  0 -> input

  switch input/output mode of GPIO 0
*/
void ifx_mdio_mode(int mode)
{
    if (mode)
    {*(AMAZON_SE_GPIO_P1_DIR) |= ASE_TANTOS_MDIO;}
    else 
    {*(AMAZON_SE_GPIO_P1_DIR) &= ASE_TANTOS_MDIO_COMPLEMENT;}    
}

void ifx_mdc_hi(void)
{
     *AMAZON_SE_GPIO_P1_OUT |= (ASE_TANTOS_MDC);
}

void ifx_mdio_hi(void)
{
     *AMAZON_SE_GPIO_P1_OUT |= (ASE_TANTOS_MDIO);
}

void ifx_mdcs_hi(void)
{
}

void ifx_mdc_lo(void)
{
   *AMAZON_SE_GPIO_P1_OUT &= (ASE_TANTOS_MDC_COMPLEMENT);
}

void ifx_mdio_lo(void)
{
    *AMAZON_SE_GPIO_P1_OUT &= (ASE_TANTOS_MDIO_COMPLEMENT);
}

void ifx_mdcs_lo(void)
{
}

/*
  mdc pulse
  0 -> 1 -> 0
*/
static void ifx_sw_mdc_pulse(void)
{

    ifx_mdc_lo();
    udelay(TANTOS_SW_MDC_DOWN_DELAY);
    ifx_mdc_hi();
    udelay(TANTOS_SW_MDC_DOWN_DELAY);
    ifx_mdc_lo();

}

/*
  mdc toggle
  1 -> 0
*/
static void ifx_sw_mdc_toggle(void)
{

    ifx_mdc_hi();
    udelay(TANTOS_SW_MDC_DOWN_DELAY);
    ifx_mdc_lo();
    udelay(TANTOS_SW_MDC_DOWN_DELAY);

}

/*
  enable eeprom write
  For ATC 93C66 type EEPROM; accessing ADM6996 internal EEPROM type registers
*/
static void ifx_sw_eeprom_write_enable(void)
{
  // no need for ASE WAVE board
}

/*
  disable eeprom write
*/
static void ifx_sw_eeprom_write_disable(void)
{
}



#ifdef CONFIG_SWITCH_ADM6996_MDIO //605112:fchang.added
static int ifx_sw_read_adm6996_smi(unsigned int addr, unsigned int *dat)
{
//060620:henryhsu modify for vlan   addr=((addr<<16)|(1<<21))&0x3ff0000;
	addr=(addr<<16)&0x3ff0000;
	*AMAZON_SE_PPE32_ETOP_MDIO_ACC =(0xC0000000|addr);
	while ((*AMAZON_SE_PPE32_ETOP_MDIO_ACC)&0x80000000){};
	*dat=((*AMAZON_SE_PPE32_ETOP_MDIO_ACC)&0x0FFFF);
	return 0;
}
#endif


static int ifx_sw_read_tantos(unsigned int addr, unsigned int *dat)
{
    unsigned int op;
    ifx_gpio_init();

    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_lo();

    udelay(ADM_SW_CS_DELAY);

    /* preamble, 32 bit 1 */
    ifx_mdio_hi();
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* command start (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_START)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* read command (10b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_READ)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* send address A9 ~ A0 */
    op = ADM_SW_BIT_MASK_10;
    while (op)
    {
        if (op & addr)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* turnaround bits */
    op = ADM_SW_BIT_MASK_2;
    ifx_mdio_hi();
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    udelay(ADM_SW_MDC_DOWN_DELAY);

    /* set MDIO pin to input mode */
   
    ifx_mdio_mode(ADM_SW_MDIO_INPUT);

    /* start read data */

    *dat = 0;
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        *dat <<= 1;
        if (ifx_sw_mdio_readbit()) *dat |= 1;
        ifx_sw_mdc_toggle();

        op >>= 1;
    }

    /* set MDIO to output mode */
    ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);

    /* dummy clock */
    op = ADM_SW_BIT_MASK_4;
    ifx_mdio_lo();
    while(op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    ifx_mdc_lo();
    ifx_mdio_lo();
    ifx_mdcs_hi();

    /* EEPROM registers */
    *dat >>= 16;
    
    return 0;
}

int ifx_sw_read(unsigned int addr, unsigned int *dat)
{
//printk("ifx_sw_read\n");
//605112:fchang.removed #ifdef ADM6996_MDC_MDIO_MODE //smi mode ////000001.joelin
#ifdef CONFIG_SWITCH_ADM6996_MDIO //605112:fchang.added
	ifx_sw_read_adm6996_smi(addr,dat);
#else
	#ifdef CONFIG_SWITCH_GPIO_ACCESS //605112:fchang.added
  ifx_sw_read_tantos(addr,dat);
  #endif
#endif
	return 0;
}

/*
  write register to ADM6996 eeprom registers
*/
//for adm6996i -start
//605112:fchang.removed #ifdef ADM6996_MDC_MDIO_MODE //smi mode //000001.joelin
#ifdef CONFIG_SWITCH_ADM6996_MDIO //605112:fchang.added
static int ifx_sw_write_adm6996_smi(unsigned int addr, unsigned int dat)
{
//060620:henryhsu modify for vlan *DANUBE_PPE32_ETOP_MDIO_ACC = (((addr<<16)|(1<<21))&0x3ff0000)|dat|0x80000000;
	*AMAZON_SE_PPE32_ETOP_MDIO_ACC = ((addr<<16) &0x3ff0000)|dat|0x80000000;
	while ((*AMAZON_SE_PPE32_ETOP_MDIO_ACC )&0x80000000){};
	return 0;
}
#endif //ADM6996_MDC_MDIO_MODE //000001.joelin

static int ifx_sw_write_tantos(unsigned int addr, unsigned int dat)
{
#if 1 
    unsigned int op;

    ifx_gpio_init();

    ifx_mdcs_hi();
    udelay(ADM_SW_CS_DELAY);

    ifx_mdcs_lo();
    ifx_mdc_lo();
    ifx_mdio_lo();

    udelay(ADM_SW_CS_DELAY);

    /* preamble, 32 bit 1 */
    ifx_mdio_hi();
    op = ADM_SW_BIT_MASK_32;
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* command start (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_START)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* write command (01b) */
    op = ADM_SW_BIT_MASK_2;
    while (op)
    {
        if (op & ADM_SW_SMI_WRITE)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* send address A9 ~ A0 */
    op = ADM_SW_BIT_MASK_10;
    while (op)
    {
        if (op & addr)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    /* turnaround bits */
    op = ADM_SW_BIT_MASK_2;
    ifx_mdio_hi();
    while (op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    udelay(ADM_SW_MDC_DOWN_DELAY);

    /* set MDIO pin to output mode */
    ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);


    /* start write data */
    op = ADM_SW_BIT_MASK_16;
    while (op)
    {
        if (op & dat)
            ifx_mdio_hi();
        else
            ifx_mdio_lo();

        ifx_sw_mdc_toggle();
        op >>= 1;
    }

 //   /* set MDIO to output mode */
 //   ifx_mdio_mode(ADM_SW_MDIO_OUTPUT);

    /* dummy clock */
    op = ADM_SW_BIT_MASK_4;
    ifx_mdio_lo();
    while(op)
    {
        ifx_sw_mdc_pulse();
        op >>= 1;
    }

    ifx_mdc_lo();
    ifx_mdio_lo();
    ifx_mdcs_hi();

#endif
    return 0;
}

int ifx_sw_write(unsigned int addr, unsigned int dat)
{
//605112:fchang.removed #ifdef ADM6996_MDC_MDIO_MODE //smi mode ////000001.joelin
#ifdef CONFIG_SWITCH_ADM6996_MDIO //605112:fchang.added
	ifx_sw_write_adm6996_smi(addr,dat);
#else	//000001.joelin
	#ifdef CONFIG_SWITCH_GPIO_ACCESS //605112:fchang.added
		ifx_sw_write_tantos(addr,dat);
	#endif //605112:fchang.added
#endif	//000001.joelin
	return 0;
}

/*
  do switch PHY reset
*/
int ifx_sw_reset(void)
{
    /* reset PHY */
    ifx_sw_write(ADM_SW_PHY_RESET, 0);
    return 0;
}


static int ifx_sw_init(void)
{
	ifx_printf(("Setting default ADM6996 registers... \n"));
	/*
	ifx_sw_write(0x11,0xe300);
	ifx_sw_write(0x2e,0);
	ifx_sw_write(0x13,0x1d5);
	ifx_sw_write(0x14,0x1d5);
	ifx_sw_write(0x15,0x1d5);
	ifx_sw_write(0x16,0x1d5);
	ifx_sw_write(0x17,0x1d5);
	ifx_sw_write(0x19,0xffd5);
	ifx_sw_write(0x1a,0xffd5);
	ifx_sw_write(0x1b,0xffd5);
	*/
  return 0;
}


int adm_open(struct inode *node, struct file *filp)
{
    //MOD_INC_USE_COUNT;
    return 0;
}

ssize_t adm_read(struct file *filep, char *buf, size_t count, loff_t *ppos)
{
    return count;
}

ssize_t adm_write(struct file *filep, const char *buf, size_t count, loff_t *ppos)
{
    return count;
}

/* close */
int adm_release(struct inode *inode, struct file *filp)
{
    //MOD_DEC_USE_COUNT;
    return 0;
}

/* IOCTL function */
int adm_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long args)
{

    PREGRW uREGRW;
    unsigned int rtval;
    unsigned int val;	
    unsigned int control[6] ;	
    unsigned int status[6] ;

    if (_IOC_TYPE(cmd) != ADM_MAGIC)
    {
        printk("adm_ioctl: IOC_TYPE(%x) != ADM_MAGIC(%x)! \n", _IOC_TYPE(cmd), ADM_MAGIC);
        return (-EINVAL);
    }

    if(_IOC_NR(cmd) >= KEY_IOCTL_MAX_KEY)
    {
        printk(KERN_WARNING "adm_ioctl: IOC_NR(%x) invalid! \n", _IOC_NR(cmd));
        return (-EINVAL);
    }

    switch (cmd)
    {
        case ADM_IOCTL_REGRW:
        {
            uREGRW = (PREGRW)kmalloc(sizeof(REGRW), GFP_KERNEL);
            rtval = copy_from_user(uREGRW, (PREGRW)args, sizeof(REGRW));
            if (rtval != 0)
            {
                printk("ADM_IOCTL_REGRW: copy from user FAILED!! \n");
                return (-EFAULT);
            }

            switch(uREGRW->mode)
            {
                case REG_READ:
                    uREGRW->value = 0x12345678;//inl(uREGRW->addr);
                    copy_to_user((PREGRW)args, uREGRW, sizeof(REGRW));
                    break;
                case REG_WRITE:
                    //outl(uREGRW->value, uREGRW->addr);
                    break;

                default:
                    printk("No such Register Read/Write function!! \n");
                    return (-EFAULT);
            }
            kfree(uREGRW);
            break;
        }

        case ADM_SW_IOCTL_REGRW:
        {
            unsigned int val = 0xff;

            uREGRW = (PREGRW)kmalloc(sizeof(REGRW), GFP_KERNEL);
            rtval = copy_from_user(uREGRW, (PREGRW)args, sizeof(REGRW));
            if (rtval != 0)
            {
                printk("ADM_IOCTL_REGRW: copy from user FAILED!! \n");
                return (-EFAULT);
            }

            switch(uREGRW->mode)
            {
                case REG_READ:
                    ifx_sw_read(uREGRW->addr, &val);
                    uREGRW->value = val;
                    copy_to_user((PREGRW)args, uREGRW, sizeof(REGRW));
                    break;

                case REG_WRITE:
                    ifx_sw_write(uREGRW->addr, uREGRW->value);
                    break;
                default:
                    printk("No such Register Read/Write function!! \n");
                    return (-EFAULT);
            }
            kfree(uREGRW);
            break;
        }

        //case ADM_SW_IOCTL_INIT:
        //    ifx_sw_init();
        //    break;

        // others 
        default:
            return -EFAULT;
    }
    // end of switch 

    return 0;
}

struct file_operations adm_ops =
{
    read: adm_read,
    write: adm_write,
    open: adm_open,
    release: adm_release,
    ioctl: adm_ioctl
};

int adm_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf+len, " ************ Registers ************ \n");
    *eof = 1;
    return len;
}



int __init init_adm6996_module(void)
{
    unsigned int val = 000;
    unsigned int val1 = 000;

    ifx_sw_init();

//605112:fchang.removed #ifdef ADM6996_MDC_MDIO_MODE //smi mode //000001.joelin
#ifdef CONFIG_SWITCH_ADM6996_MDIO //605112:fchang.added
    register_chrdev(69, "adm6996", &adm_ops);
    *AMAZON_SE_PPE32_ETOP_MDIO_CFG=0;
    *AMAZON_SE_PPE32_ENET_MAC_CFG &= ~0x18; //ENET0 MAC Configuration
    ifx_sw_read(0xa0, &val);
    ifx_sw_read(0xa1, &val1);
    val=((val1&0x0f)<<16)|val;
    printk ("\n6996I SMI Mode-");
    printk ("Chip ID:%5x \n ", val);
 //ADM6996_MDC_MDIO_MODE //smi mode //000001.joelin
#endif 

#ifdef CONFIG_SWITCH_GPIO_ACCESS //605112:fchang.added
    printk("Loading Tantos 0G GPIO driver\n");
    ifx_gpio_init();
    register_chrdev(69, "adm6996", &adm_ops);
#endif //605112:fchang.added
    return 0;
}

void __exit cleanup_adm6996_module(void)
{
    printk("Free 6996 device driver... \n");
    unregister_chrdev(69, "adm6996");
}


static int ifx_hw_reset(void)
{
     printk("Free 6996 device driver... \n");
     unregister_chrdev(69, "adm6996");
	   udelay(200000);
	   ifx_sw_init();
	   udelay(200000);
     return init_adm6996_module();
}
    
    
//int (*adm6996_hw_reset)(void) = ifx_hw_reset;
//EXPORT_SYMBOL(adm6996_hw_reset);
//int (*adm6996_sw_read)(unsigned int addr, unsigned int *data) = ifx_sw_read;
//EXPORT_SYMBOL(adm6996_sw_read);
//int (*adm6996_sw_write)(unsigned int addr, unsigned int data) = ifx_sw_write;
//EXPORT_SYMBOL(adm6996_sw_write);

//EXPORT_SYMBOL(switch_model);
EXPORT_SYMBOL(ifx_sw_read);
EXPORT_SYMBOL(ifx_sw_write);


MODULE_DESCRIPTION("ADMtek 6996 Driver");
MODULE_AUTHOR("Joe Lin <joe.lin@infineon.com>");
MODULE_LICENSE("GPL");

module_init(init_adm6996_module);
module_exit(cleanup_adm6996_module);

