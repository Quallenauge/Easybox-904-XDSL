/******************************************************************************
**
** FILE NAME    : amazon_s_pmcu.c
** PROJECT      : Amazon_S
** MODULES      : PMCU
**
** DATE         : 10 NOV 2008
** AUTHOR       : WU QI MING
** DESCRIPTION  : Power control management unit driver
** COPYRIGHT    :   Copyright (c) 2006
**      Infineon Technologies AG
**      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 10 NOV 2008  Wu Qi Ming      Initial Version, to comply with COC
*******************************************************************************/
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif /* AUTOCONF_INCLUDED */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/poll.h>
#include <linux/interrupt.h>

/* #include <asm/amazon_s/amazon_s.h> */
#include <asm/ifx/ifx_pmcu_v1.h>


/* major number definition */
#define PMCU_DYNAMIC_MAJOR_NO
#ifdef PMCU_DYNAMIC_MAJOR_NO
   static int ifx_pmcu_major = 0;
#else
   #define IFX_PMCU_MAJOR            243
#endif
#define IFX_PMCU_DRV_VERSION     "1.0.0"

wait_queue_head_t pmcu_wait_q;
unsigned int poll_mask = 0;

STATE_TRANSITION_MAP_t pmcu_require_table[]={
   /*{{IFX_PMCU_MODULE_CPU,0,IFX_PMCU_STATE_D3},{IFX_PMCU_MODULE_VE,0,IFX_PMCU_STATE_D3}}*/
};

STATE_TRANSITION_MAP_t pmcu_nallow_table[]={
    {{IFX_PMCU_MODULE_CPU,0,IFX_PMCU_STATE_D3},{IFX_PMCU_MODULE_VE,0,IFX_PMCU_STATE_D0}} 
};

PMCU_MAP_t pmcu_map[MAX_PMCU_MODULE][MAX_PMCU_SUB_MODULE];
struct proc_dir_entry* g_pmcu_dir;
char pmcu_device_name[MAX_PMCU_MODULE][20]={
    {"CPU   "},
    {"ETH   "},
    {"USB   "},
    {"DSL   "},
    {"WLAN  "},
    {"DECT  "},
    {"FXS   "},
    {"FXO   "},
    {"VE    "},
    {"PPE   "},
    {"SWITCH"},
    {"UART  "},
    {"SPI   "},
    {"SDIO  "},
    {"PCI   "},
    {"VLYNQ "},
    {"DEU   "}};

static IFX_return_t amazon_s_pmcu_set_status(IFX_PMCU_MODULE_STATE_t module_state)
{
	      int ret=0;
	      PMCU_MAP_t* dev=NULL;
        STATE_LIST_t* req;
        STATE_LIST_t* nallow;
        int major, minor;
	      
	      //printk("amazon_s_pmcu_set_status!\n");
	      dev=&pmcu_map[module_state.pmcuModule][(int)module_state.pmcuModuleNr];
	      if(!dev->ifx_pmcu_state_req) return IFX_ERROR; /*state request not supported*/
	      	
	      	
        for(req=dev->state[module_state.pmcuState].req;req!=NULL;req=req->next)
        {
          major=req->state.pmcuModule;
          minor=req->state.pmcuModuleNr;
          if(
          	  req->state.pmcuModule==major &&\
          	  req->state.pmcuModuleNr==minor &&\
          	  req->state.pmcuState !=pmcu_map[major][minor].pmcuState &&\
          	  pmcu_map[major][minor].pmcuState!=-1) 
          	{
          		ret=IFX_ERROR;  
          		goto exit;
          	}
        }

        //printk("nallow now!\n");
        for(nallow=dev->state[module_state.pmcuState].nallow;nallow!=NULL;nallow=nallow->next)
        {
          major=nallow->state.pmcuModule;
          minor=nallow->state.pmcuModuleNr;
          if( nallow->state.pmcuModule==major &&\
          	  nallow->state.pmcuModuleNr==minor &&\
          	  nallow->state.pmcuState ==pmcu_map[major][minor].pmcuState &&\
          	  pmcu_map[major][minor].pmcuState!=-1) 
          	{
          		ret=IFX_ERROR;
          		goto exit;
          	}
        }
        ret=dev->ifx_pmcu_state_req(module_state.pmcuModuleNr, module_state.pmcuState);
exit:
		    return ret;	
}


IFX_return_t ifx_pmcu_info (IFX_PMCU_INFO_t* pmcuInfo)
{
    PMCU_MAP_t* dev=&pmcu_map[pmcuInfo->pmcuModule][(int)(pmcuInfo->pmcuModuleNr)];
    IFX_PMCU_MODULE_STATE_t module_state={0};
    int ret=IFX_SUCCESS;
    dev->pmcuState=pmcuInfo->pmcuState;
   
    switch(pmcuInfo->pmcuModule){
      case IFX_PMCU_MODULE_VE:
      case IFX_PMCU_MODULE_SWITCH:
        if(pmcu_map[pmcuInfo->pmcuModule][0].pmcuState==IFX_PMCU_STATE_D0 &&\
        	 pmcu_map[IFX_PMCU_MODULE_CPU][0].pmcuState!=IFX_PMCU_STATE_D0)
          {/*Voice engine is active, turn cpu into high speed if it is not*/
            module_state.pmcuModule=IFX_PMCU_MODULE_CPU;
            module_state.pmcuModuleNr=0;
            module_state.pmcuState=IFX_PMCU_STATE_D0;
            ret=amazon_s_pmcu_set_status(module_state);
          }
        break;
      default:
        break;
    }
    wake_up_interruptible(&pmcu_wait_q);
    return ret;
}

IFX_return_t ifx_pmcu_register (IFX_PMCU_REGISTER_t* pmcuRegister )
{
    PMCU_MAP_t* dev=&pmcu_map[pmcuRegister->pmcuModule][(int)(pmcuRegister->pmcuModuleNr)];
    dev->ifx_pmcu_state_req=pmcuRegister->ifx_pmcu_state_req;
    dev->ifx_pmcu_state_get=pmcuRegister->ifx_pmcu_state_get;
    if(dev->ifx_pmcu_state_get)
    {
      dev->pmcuState=dev->ifx_pmcu_state_get((int)(pmcuRegister->pmcuModuleNr)); /*read the status immediately*/
    }
    init_waitqueue_head(&dev->inq);
    init_waitqueue_head(&dev->outq);
    return IFX_SUCCESS;
}

IFX_return_t ifx_pmcu_unregister (IFX_PMCU_REGISTER_t* pmcuUnregister)
{
    return IFX_SUCCESS;
};

static int amazon_s_pmcu_open(struct inode * inode, struct file * file)
{
    //printk("amazon_s_pmcu_open!\n");
    return 0;
}

static int amazon_s_pmcu_release(struct inode *inode, struct file *file)
{
    /*release the resources*/
    //printk("amazon_s_pmcu_release!\n");
    return 0;
}


static int amazon_s_pmcu_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret=0;
    IFX_PMCU_MODULE_STATE_t module_state;
    PMCU_MAP_t* dev=NULL;
    
    switch (cmd)
    {
      case IFX_PMCU_STATE_GET:
      copy_from_user(&module_state,(IFX_PMCU_MODULE_STATE_t*)arg,sizeof(IFX_PMCU_MODULE_STATE_t));
      dev=&pmcu_map[module_state.pmcuModule][(int)module_state.pmcuModuleNr];
      
      dev->pmcuState=dev->ifx_pmcu_state_get((int)module_state.pmcuModuleNr);
      module_state.pmcuState=dev->pmcuState;
      copy_to_user((IFX_PMCU_MODULE_STATE_t*)arg,&module_state,sizeof(IFX_PMCU_MODULE_STATE_t));
        //poll_mask = POLLIN | POLLRDNORM;
      
    break;

     case IFX_PMCU_STATE_SET:
        copy_from_user(&module_state,(IFX_PMCU_MODULE_STATE_t*)arg,sizeof(IFX_PMCU_MODULE_STATE_t));
        ret=amazon_s_pmcu_set_status(module_state);
        
        break;

     default:
        ret = -ENOTTY;
  }

    return ret;
}


static unsigned int amazon_s_pmcu_poll(struct file *filp, poll_table *wait)
{


  poll_wait(filp, &pmcu_wait_q,  wait);

  return poll_mask;

}

static struct file_operations pmcu_fops = {
      .open    = amazon_s_pmcu_open,
      .release = amazon_s_pmcu_release,
      .poll    = amazon_s_pmcu_poll,
      .ioctl   = amazon_s_pmcu_ioctl
};


void ifx_pmcu_add_req(STATE_TRANSITION_MAP_t* map_req)
{
     int module_num=map_req->Target.pmcuModule;
     int sub_module_num=map_req->Target.pmcuModule;
     PMCU_MAP_t* dev=&pmcu_map[module_num][sub_module_num];
     IFX_PMCU_STATE_t target_state=map_req->Target.pmcuState;
     STATE_LIST_t* req=NULL;
       
     for(req=dev->state[target_state].req;req!=NULL;req=req->next)
     {
           if(req->state.pmcuModule==map_req->Target.pmcuModule && \
         req->state.pmcuModuleNr==map_req->Target.pmcuModuleNr)
       {
         /*already exists, return*/
         return;
       }
       if(req->next==NULL) break;
     }
       if(req)
     	{
     		req->next=(STATE_LIST_t*)kmalloc(sizeof(STATE_LIST_t),GFP_ATOMIC);
     		req=req->next;
     	}
     	else
     	{
        dev->state[target_state].req=(STATE_LIST_t*)kmalloc(sizeof(STATE_LIST_t),GFP_ATOMIC);
        req=dev->state[target_state].req;
      }
     req->state.pmcuModule=map_req->Req.pmcuModule;
     req->state.pmcuModuleNr=map_req->Req.pmcuModuleNr;
     req->state.pmcuState=map_req->Req.pmcuState;
     req->next=NULL;

     return;

}


void ifx_pmcu_add_nallow(STATE_TRANSITION_MAP_t* map_req)
{
     int module_num;
     int sub_module_num;
     PMCU_MAP_t* dev;
     IFX_PMCU_STATE_t target_state;
    
     STATE_LIST_t* nallow=NULL;
     
     
     module_num=map_req->Target.pmcuModule;
     sub_module_num=map_req->Target.pmcuModule;
     dev=&pmcu_map[module_num][sub_module_num];
     target_state=map_req->Target.pmcuState;
     
          
     for(nallow=dev->state[target_state].nallow;nallow!=NULL;nallow=nallow->next)
     {
         if(nallow->state.pmcuModule==map_req->Target.pmcuModule && \
         nallow->state.pmcuModuleNr==map_req->Target.pmcuModuleNr)
       {
         /*already exists, return*/
         return;
       }
         if(nallow->next==NULL) break;
     }
     
     
     if(nallow)
     	{
     		nallow->next=(STATE_LIST_t*)kmalloc(sizeof(STATE_LIST_t),GFP_ATOMIC);
     		nallow=nallow->next;
     	}
     	else
     	{
        dev->state[target_state].nallow=(STATE_LIST_t*)kmalloc(sizeof(STATE_LIST_t),GFP_ATOMIC);
        nallow=dev->state[target_state].nallow;
      }
     nallow->state.pmcuModule=map_req->Req.pmcuModule;
     nallow->state.pmcuModuleNr=map_req->Req.pmcuModuleNr;
     nallow->state.pmcuState=map_req->Req.pmcuState;
     nallow->next=NULL;
     
     
     return;

}

int pmcu_device_list_proc_read(char *buf, char **start, off_t offset,
                         int count, int *eof, void *data)
{
       int len=0;
       int i,j;
     int exist_flag=0;
     len += sprintf(buf+len, "M/sub\t0\t1\t2\t3\t4\t5\n");

     for(i=0;i<MAX_PMCU_MODULE;i++)
     {
       exist_flag=0;
           for(j=0;j<MAX_PMCU_SUB_MODULE;j++)
       {
                if(pmcu_map[i][j].pmcuState!=-1)
        {
           exist_flag=1;
                     break;
        }
       }
       if (exist_flag==0) continue;

       len += sprintf(buf+len, "%s",pmcu_device_name[i]);
       for(j=0;j<MAX_PMCU_SUB_MODULE;j++)
       {
                if(pmcu_map[i][j].pmcuState!=-1)
          len += sprintf(buf+len,"\t%d",pmcu_map[i][j].pmcuState);
        else
                    len += sprintf(buf+len,"\t ");

       }
           len += sprintf(buf+len,"\n");
     }


     return len;

}


static int ifx_init_pmcu_table(void)
{
     int i,j,k;
     int req_num=0;
     int nallow_num=0;
     
     
     /*initialize pmcu device table*/
     for(i=0;i<MAX_PMCU_MODULE;i++)
     {
          for(j=0;j<MAX_PMCU_SUB_MODULE;j++)
      {
             //memset(&pmcu_map[i][j],0,sizeof(pmcu_map[i][j]));
             pmcu_map[i][j].pmcuState=-1; /*-1 indicates non-exist module*/
             pmcu_map[i][j].ifx_pmcu_state_req=NULL;
             pmcu_map[i][j].ifx_pmcu_state_get=NULL;
             for(k=0;k<4;k++)
       {
                pmcu_map[i][j].state[k].req=NULL;
                pmcu_map[i][j].state[k].nallow=NULL;
       }
      }
     }
        /*calculate the default condition number*/
      req_num=sizeof(pmcu_require_table)/sizeof(STATE_TRANSITION_MAP_t);
      nallow_num=sizeof(pmcu_nallow_table)/sizeof(STATE_TRANSITION_MAP_t);
      

    /*add required conditions*/
    for(i=0;i<req_num;i++)
    {
            ifx_pmcu_add_req(&pmcu_require_table[i]);
    }

        /*add required conditions*/
    for(i=0;i<nallow_num;i++)
    {
            ifx_pmcu_add_nallow(&pmcu_nallow_table[i]);
    }
    return IFX_SUCCESS;
}

int __init amazon_s_pmcu_init(void)
{
    int err = 0;
  printk("PMCU initiates!\n");
  
  
   /* register port device */
#ifdef PMCU_DYNAMIC_MAJOR_NO
   err = register_chrdev (0, "amazon_s-pmcu", &pmcu_fops);
   if (err < 0) {
      printk("amazon_s-pmcu: Error! Could not register pmcu device. #%d\n", err);
      return err;
   }
   ifx_pmcu_major = err;
#else
   err = register_chrdev (IFX_PMCU_MAJOR, "amazon_s-pmcu", &pmcu_fops);
   if (err != 0) {
      printk ("amazon_s-pmcu: Error! Could not register pmcu device. #%d\n", err);
      return err;
   }
#endif


  /*initialize pmcu device table*/
  ifx_init_pmcu_table();

  /*create pmcu proc entry*/
    g_pmcu_dir=proc_mkdir("pmcu",NULL);
    create_proc_read_entry("device_list",
                            0,
                            g_pmcu_dir,
                            pmcu_device_list_proc_read,
                            NULL);

  init_waitqueue_head(&pmcu_wait_q);
   return 0;
}

static void __exit
amazon_s_pmcu_exit (void)
{
#ifdef PMCU_DYNAMIC_MAJOR_NO
   if (ifx_pmcu_major != 0)
      unregister_chrdev(ifx_pmcu_major, "amazon_s-pmcu");
#else
   unregister_chrdev(IFX_PMCU_MAJOR,"amazon_s-pmcu");
#endif
}


module_init (amazon_s_pmcu_init);
module_exit(amazon_s_pmcu_exit);
EXPORT_SYMBOL(ifx_pmcu_info);
EXPORT_SYMBOL(ifx_pmcu_register);
EXPORT_SYMBOL(ifx_pmcu_unregister);
MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Infineon SD");
MODULE_SUPPORTED_DEVICE ("Infineon Amazon_S");
MODULE_DESCRIPTION ("Infineon technologies PMCU device driver");




