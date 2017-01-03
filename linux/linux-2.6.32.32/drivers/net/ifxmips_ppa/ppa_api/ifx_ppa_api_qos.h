#ifndef __IFX_PPA_API_MFE_H__20100427_1703
#define __IFX_PPA_API_MFE_H__20100427_1703
/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_qos.h
** PROJECT      : PPA
** MODULES      : PPA API ( PPA QOS  APIs)
**
** DATE         : 27 April 2010
** AUTHOR       : Shao Guohua
** DESCRIPTION  : PPA QOS APIs 
**                File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date                $Author         $Comment
** 16 Dec 2009  Shao Guohua             Initiate Version
*******************************************************************************/

/*! \file ifx_ppa_api_qos.h
    \brief This file contains es.
           provide PPA power management API.
*/

/** \addtogroup  PPA_API_QOS */
/*@{*/



/*
 * ####################################
 *              Definition
 * ####################################
 */





/*
 * ####################################
 *              Data Type
 * ####################################
 */


/*
 * ####################################
 *             Declaration
 * ####################################
 */
#ifdef CONFIG_IFX_PPA_QOS
extern int32_t ppa_ioctl_get_qos_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ifx_ppa_get_qos_qnum( uint32_t portid, uint32_t flag);
extern int32_t ppa_ioctl_get_qos_qnum(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ifx_ppa_get_qos_mib( uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag);
extern int32_t ppa_ioctl_get_qos_mib(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);

#ifdef CONFIG_IFX_PPA_QOS_WFQ
extern int32_t ifx_ppa_set_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag );
extern int32_t ifx_ppa_get_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
extern int32_t ifx_ppa_reset_qos_wfq( uint32_t portid, uint32_t queueid, uint32_t flag);
extern int32_t ifx_ppa_set_ctrl_qos_wfq(uint32_t portid,  uint32_t f_enable, uint32_t flag);
extern int32_t ifx_ppa_get_ctrl_qos_wfq(uint32_t portid,  uint32_t *f_enable, uint32_t flag);

extern int32_t ppa_ioctl_set_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_get_ctrl_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_set_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_get_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_reset_qos_wfq(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info); 
#endif //end of CONFIG_IFX_PPA_QOS_WFQ

#ifdef CONFIG_IFX_PPA_QOS_RATE_SHAPING
extern int32_t ifx_ppa_set_ctrl_qos_rate(uint32_t portid,  uint32_t f_enable, uint32_t flag);
extern int32_t ifx_ppa_get_ctrl_qos_rate(uint32_t portid,  uint32_t *f_enable, uint32_t flag);
extern int32_t ifx_ppa_set_qos_rate( uint32_t portid, uint32_t queueid, uint32_t rate, uint32_t burst, uint32_t flag );
extern int32_t ifx_ppa_get_qos_rate( uint32_t portid, uint32_t queueid, uint32_t *rate, uint32_t *burst, uint32_t flag);
extern int32_t ifx_ppa_reset_qos_rate( uint32_t portid, uint32_t queueid, uint32_t flag);

extern int32_t ppa_ioctl_set_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_get_ctrl_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_set_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_reset_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
extern int32_t ppa_ioctl_get_qos_rate(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#endif //END OF CONFIG_IFX_PPA_QOS_RATE_SHAPING

#endif //END OF CONFIG_IFX_PPA_QOS


#endif  //end of __IFX_PPA_API_MFE_H__20100427_1703

