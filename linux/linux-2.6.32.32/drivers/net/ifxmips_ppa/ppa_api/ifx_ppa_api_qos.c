/******************************************************************************
**
** FILE NAME    : ifx_ppa_api_qos.c
** PROJECT      : UEIP
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 11 DEC 2009
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA Protocol Stack QOS API Implementation
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author            $Comment
** 11 DEC 2009  Shao Guohua        Initiate Version
*******************************************************************************/


#include <linux/autoconf.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/version.h>
//#include <linux/types.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#if defined(CONFIG_IFX_PPA_API_PROC)
//#include <linux/proc_fs.h>
//#endif
//#include <linux/netdevice.h>
//#include <linux/in.h>
//#include <net/sock.h>
//#include <net/ip_vs.h>
//#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppa_api_qos.h"
#include "ifx_ppe_drv_wrapper.h"

#ifdef CONFIG_IFX_PPA_QOS
int32_t ppa_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
    int res = IFX_FAILURE;
       
    if ( copy_from_user( &cmd_info->qos_status_info, (void *)arg, sizeof(cmd_info->qos_status_info)) != 0 )
        return IFX_FAILURE;
   
    res = ifx_ppa_drv_get_qos_status( &cmd_info->qos_status_info.qstat, cmd_info->qos_status_info.flags);
   
    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_status_info, sizeof(cmd_info->qos_status_info)) != 0 )
        return IFX_FAILURE;

   return res;
}

int32_t ifx_ppa_get_qos_qnum( uint32_t portid, uint32_t flag)
{
    PPE_QOS_COUNT_CFG  count={0};

    count.portid = portid;
    count.flags = flag;
    
    ifx_ppa_drv_get_qos_qnum( &count, 0);
    
    return count.num;
}

int32_t ppa_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
    int res = IFX_FAILURE;
    PPE_QOS_COUNT_CFG count={0};
    
    ppa_memset(&cmd_info->qnum_info, 0, sizeof(cmd_info->qnum_info) );
    if ( copy_from_user( &cmd_info->qnum_info, (void *)arg, sizeof(cmd_info->qnum_info)) != 0 )
        return IFX_FAILURE;
    count.portid = cmd_info->qnum_info.portid;
    res = ifx_ppa_drv_get_qos_qnum( &count, 0);
    cmd_info->qnum_info.queue_num = count.num;
    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qnum_info, sizeof(cmd_info->qnum_info)) != 0 )
        return IFX_FAILURE;

   return res;
}

int32_t ifx_ppa_get_qos_mib( uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag)
{
    uint32_t res;
    PPE_QOS_MIB_INFO mib_info={0};

    mib_info.portid = portid;
    mib_info.queueid = queueid;
    mib_info.flag = flag;
    res = ifx_ppa_drv_get_qos_mib( &mib_info, 0);

    *mib = mib_info.mib;
    
    return res;
}

int32_t ppa_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{
    int res = IFX_FAILURE;
    PPE_QOS_MIB_INFO mib={0};
    
    ppa_memset(&cmd_info->qos_mib_info, 0, sizeof(cmd_info->qos_mib_info) );
    if ( copy_from_user( &cmd_info->qos_mib_info, (void *)arg, sizeof(cmd_info->qos_mib_info)) != 0 )
        return IFX_FAILURE;

    mib.portid = cmd_info->qos_mib_info.portid;
    mib.flag = cmd_info->qos_mib_info.flags;
    res = ifx_ppa_drv_get_qos_mib( &mib, 0);
    
    cmd_info->qos_mib_info.mib = mib.mib;

    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_mib_info, sizeof(cmd_info->qos_mib_info)) != 0 )
        return IFX_FAILURE;

   return res;
}




#ifdef CONFIG_IFX_PPA_QOS_WFQ
int32_t ifx_ppa_set_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag )
{
    PPE_QOS_WFQ_CFG wfq={0};

    wfq.portid = portid;
    wfq.queueid = queueid;
    wfq.weight_level = weight_level;
    wfq.flag = flag;

    return ifx_ppa_drv_set_qos_wfq( &wfq, 0);    
}

int32_t ifx_ppa_get_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag)
{
    PPE_QOS_WFQ_CFG wfq={0};
    int32_t res;

    wfq.portid = portid;
    wfq.queueid = queueid;
    wfq.flag = flag;

    res = ifx_ppa_drv_get_qos_wfq (&wfq, 0);

    if( weight_level ) *weight_level = wfq.weight_level;
    
    return res;
}

int32_t ifx_ppa_reset_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t flag)
{
    PPE_QOS_WFQ_CFG cfg={0};

    cfg.portid = portid;
    cfg.queueid = queueid;
    cfg.flag = flag;
   return ifx_ppa_drv_reset_qos_wfq(&cfg, 0);
}

int32_t ifx_ppa_set_ctrl_qos_wfq(uint32_t portid,  uint32_t f_enable, uint32_t flag)
{
    int i;
    PPE_QOS_COUNT_CFG count={0};
    PPE_QOS_ENABLE_CFG enable_cfg={0};

    count.portid = portid;
    count.flags = flag;
    ifx_ppa_drv_get_qos_qnum( &count, 0);
    
    if( count.num <= 0 )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ifx_ppa_set_ctrl_qos_wfq: count.num not valid (%d) to %s wfq on port %d\n", count.num, f_enable?"enable":"disable", portid );
        return IFX_FAILURE;
    }

    enable_cfg.portid = portid;
    enable_cfg.flag = flag;
    enable_cfg.f_enable = f_enable;
    ppa_debug(DBG_ENABLE_MASK_QOS, "ifx_ppa_set_ctrl_qos_wfq to %s wfq on port %d\n", f_enable?"enable":"disable", portid );
    ifx_ppa_drv_set_ctrl_qos_wfq( &enable_cfg, 0);    

    for( i=0; i<count.num; i++ )
           ifx_ppa_reset_qos_wfq( portid, i, 0);
    return IFX_SUCCESS;
}

int32_t ifx_ppa_get_ctrl_qos_wfq(uint32_t portid,  uint32_t *f_enable, uint32_t flag)
{ 
    int32_t res = IFX_FAILURE;
    
    if( f_enable )
    {
        PPE_QOS_ENABLE_CFG enable_cfg={0};

        enable_cfg.portid = portid;
        enable_cfg.flag = flag;
            
        res = ifx_ppa_drv_get_ctrl_qos_wfq(&enable_cfg, 0);

        if( f_enable ) *f_enable = enable_cfg.f_enable;
    }
    
    return res;
}

int32_t ppa_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int32_t res;
    
    ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
        return IFX_FAILURE;
  
     res = ifx_ppa_set_ctrl_qos_wfq(cmd_info->qos_ctrl_info.portid, cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
     if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_ctrl_qos_wfq fail");
        res = IFX_FAILURE;
    }

     return res;
}

int32_t ppa_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int res = IFX_FAILURE;
        
    ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_get_ctrl_qos_wfq(cmd_info->qos_ctrl_info.portid, &cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
    if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_ctrl_qos_wfq fail");
        res = IFX_FAILURE;
    }

    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_ctrl_info, sizeof(cmd_info->qos_ctrl_info)) != 0 )
        return IFX_FAILURE;

    return res;
}

int32_t ppa_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int32_t res;
    
    ppa_memset(&cmd_info->qos_wfq_info, 0, sizeof(cmd_info->qos_wfq_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_wfq_info, (void *)arg, sizeof(cmd_info->qos_wfq_info)) != 0 )
        return IFX_FAILURE;
  
     res = ifx_ppa_set_qos_wfq(cmd_info->qos_wfq_info.portid, cmd_info->qos_wfq_info.queueid, cmd_info->qos_wfq_info.weight, cmd_info->qos_wfq_info.flags);
     if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_qos_wfq fail");
        res = IFX_FAILURE;
    }

     return res;
}

int32_t ppa_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int res = IFX_FAILURE;
    
    ppa_memset(&cmd_info->qos_wfq_info, 0, sizeof(cmd_info->qos_wfq_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_wfq_info, (void *)arg, sizeof(cmd_info->qos_wfq_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_reset_qos_wfq(cmd_info->qos_wfq_info.portid, cmd_info->qos_wfq_info.queueid, cmd_info->qos_wfq_info.flags);
    if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_reset_qos_wfq fail");
        res = IFX_FAILURE;
    }

    return res;   
}

int32_t ppa_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int res = IFX_FAILURE;
        
    ppa_memset(&cmd_info->qos_wfq_info, 0, sizeof(cmd_info->qos_wfq_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_wfq_info, (void *)arg, sizeof(cmd_info->qos_wfq_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_get_qos_wfq(cmd_info->qos_wfq_info.portid, cmd_info->qos_wfq_info.queueid, &cmd_info->qos_wfq_info.weight, cmd_info->qos_wfq_info.flags);
    if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_qos_wfq fail");
        res = IFX_FAILURE;
    }

    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_wfq_info, sizeof(cmd_info->qos_wfq_info)) != 0 )
        return IFX_FAILURE;

    return res;
}

EXPORT_SYMBOL(ifx_ppa_set_ctrl_qos_wfq);
EXPORT_SYMBOL(ifx_ppa_get_ctrl_qos_wfq);
EXPORT_SYMBOL(ifx_ppa_set_qos_wfq);
EXPORT_SYMBOL(ifx_ppa_get_qos_wfq);
EXPORT_SYMBOL(ifx_ppa_reset_qos_wfq);
EXPORT_SYMBOL(ppa_ioctl_set_ctrl_qos_wfq);
EXPORT_SYMBOL(ppa_ioctl_get_ctrl_qos_wfq);
EXPORT_SYMBOL(ppa_ioctl_set_qos_wfq);
EXPORT_SYMBOL(ppa_ioctl_reset_qos_wfq);
EXPORT_SYMBOL(ppa_ioctl_get_qos_wfq);
#endif  //end of CONFIG_IFX_PPA_QOS_WFQ

#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
int32_t ifx_ppa_set_ctrl_qos_rate(uint32_t portid,  uint32_t f_enable, uint32_t flag)
{
    uint32_t i;
    PPE_QOS_COUNT_CFG count={0};
    PPE_QOS_ENABLE_CFG enable_cfg={0};
    PPE_QOS_RATE_SHAPING_CFG rate={0};

    count.portid = portid;
    count.flags = flag;
    ifx_ppa_drv_get_qos_qnum( &count, 0);

    if( count.num <= 0 )
        return IFX_FAILURE;

    enable_cfg.portid = portid;
    enable_cfg.flag = flag;
    enable_cfg.f_enable = f_enable;
    ifx_ppa_drv_set_ctrl_qos_rate( &enable_cfg, 0);    

    for( i=0; i<count.num; i++ )
    {
        rate.flag = 0;
        rate.portid = portid;
        ifx_ppa_drv_reset_qos_rate( &rate, 0);
    }

    return IFX_SUCCESS;
}

int32_t ifx_ppa_get_ctrl_qos_rate(uint32_t portid,  uint32_t *f_enable, uint32_t flag)
{ 
    PPE_QOS_ENABLE_CFG enable_cfg={0};
    int32_t res;

    enable_cfg.portid = portid;
    enable_cfg.flag = flag;

    res= ifx_ppa_drv_get_ctrl_qos_rate( &enable_cfg, 0);

    if( *f_enable ) *f_enable = enable_cfg.f_enable;
    return res;
}

int32_t ifx_ppa_set_qos_rate( uint32_t portid, uint32_t queueid, uint32_t rate, uint32_t burst, uint32_t flag )
{
    PPE_QOS_RATE_SHAPING_CFG rate_cfg={0};

    rate_cfg.portid = portid;
    rate_cfg.queueid = queueid;
    rate_cfg.rate_in_kbps = rate;
    rate_cfg.burst = burst;
    rate_cfg.flag = flag;
    
    return ifx_ppa_drv_set_qos_rate( &rate_cfg, 0);
}

int32_t ifx_ppa_get_qos_rate( uint32_t portid, uint32_t queueid, uint32_t *rate, uint32_t *burst, uint32_t flag)
{
    PPE_QOS_RATE_SHAPING_CFG rate_cfg={0};
    int32_t res = IFX_FAILURE;

    rate_cfg.portid = portid;
    rate_cfg.flag = flag;
    rate_cfg.queueid = queueid;
    
    res = ifx_ppa_drv_get_qos_rate( &rate_cfg, 0);

    if( rate ) *rate = rate_cfg.rate_in_kbps;
    if( burst )  *burst = rate_cfg.burst;

    return res;

}

int32_t ifx_ppa_reset_qos_rate( uint32_t portid, uint32_t queueid, uint32_t flag)
{
    PPE_QOS_RATE_SHAPING_CFG rate_cfg={0};

    rate_cfg.portid = portid;
    rate_cfg.queueid = queueid;
    rate_cfg.flag = flag;
    
    return ifx_ppa_drv_reset_qos_rate( &rate_cfg, 0);
}


int32_t ppa_ioctl_set_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int32_t res;
    
    ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
        return IFX_FAILURE;
  
     res = ifx_ppa_set_ctrl_qos_rate(cmd_info->qos_ctrl_info.portid, cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
     if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_ctrl_qos_rate fail");
        res = IFX_FAILURE;
    }

     return res;
}

int32_t ppa_ioctl_get_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int res = IFX_FAILURE;
        
    ppa_memset(&cmd_info->qos_ctrl_info, 0, sizeof(cmd_info->qos_ctrl_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_ctrl_info, (void *)arg, sizeof(cmd_info->qos_ctrl_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_get_ctrl_qos_rate(cmd_info->qos_ctrl_info.portid, &cmd_info->qos_ctrl_info.enable, cmd_info->qos_ctrl_info.flags);
    if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_ctrl_qos_rate fail");
        res = IFX_FAILURE;
    }

    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_ctrl_info, sizeof(cmd_info->qos_ctrl_info)) != 0 )
        return IFX_FAILURE;

    return res;
}

int32_t ppa_ioctl_set_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int32_t res;
    
    ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_set_qos_rate(cmd_info->qos_rate_info.portid, cmd_info->qos_rate_info.queueid, cmd_info->qos_rate_info.rate, cmd_info->qos_rate_info.burst, cmd_info->qos_rate_info.flags);
    if ( res != IFX_SUCCESS )
    {  
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_set_qos_rate fail");
        res = IFX_FAILURE;
    }

    return res;
}

int32_t ppa_ioctl_reset_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int res = IFX_FAILURE;
    
    ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_reset_qos_rate(cmd_info->qos_rate_info.portid, cmd_info->qos_rate_info.queueid, cmd_info->qos_rate_info.flags);
    if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ifx_ppa_reset_rate fail");
        res = IFX_FAILURE;
    }

    return res;   
}

int32_t ppa_ioctl_get_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info)
{ /*note, arg is a pointer from ioctl, not normally pointer  */
    int res = IFX_FAILURE;
        
    ppa_memset(&cmd_info->qos_rate_info, 0, sizeof(cmd_info->qos_rate_info) );

    if ( ppa_copy_from_user( &cmd_info->qos_rate_info, (void *)arg, sizeof(cmd_info->qos_rate_info)) != 0 )
        return IFX_FAILURE;

    res = ifx_ppa_get_qos_rate(cmd_info->qos_rate_info.portid, cmd_info->qos_rate_info.queueid, &cmd_info->qos_rate_info.rate, &cmd_info->qos_rate_info.burst, cmd_info->qos_rate_info.flags);
    if ( res != IFX_SUCCESS )
    {
        ppa_debug(DBG_ENABLE_MASK_QOS, "ppa_ioctl_get_qos_rate fail");
        res = IFX_FAILURE;
    }

    if ( ppa_copy_to_user( (void *)arg, &cmd_info->qos_rate_info, sizeof(cmd_info->qos_rate_info)) != 0 )
        return IFX_FAILURE;

    return res;
}


EXPORT_SYMBOL(ifx_ppa_set_ctrl_qos_rate);
EXPORT_SYMBOL(ifx_ppa_get_ctrl_qos_rate);
EXPORT_SYMBOL(ifx_ppa_set_qos_rate);
EXPORT_SYMBOL(ifx_ppa_get_qos_rate);
EXPORT_SYMBOL(ifx_ppa_reset_qos_rate);
EXPORT_SYMBOL(ppa_ioctl_set_ctrl_qos_rate);
EXPORT_SYMBOL(ppa_ioctl_get_ctrl_qos_rate);
EXPORT_SYMBOL(ppa_ioctl_set_qos_rate);
EXPORT_SYMBOL(ppa_ioctl_reset_qos_rate);
EXPORT_SYMBOL(ppa_ioctl_get_qos_rate);
#endif  //end of CONFIG_IFX_PPA_QOS_RATE_SHAPING

EXPORT_SYMBOL(ifx_ppa_get_qos_qnum);
EXPORT_SYMBOL(ppa_ioctl_get_qos_qnum);
EXPORT_SYMBOL(ifx_ppa_get_qos_mib);
EXPORT_SYMBOL(ppa_ioctl_get_qos_mib);
#endif  //end of CONFIG_IFX_PPA_QOS


