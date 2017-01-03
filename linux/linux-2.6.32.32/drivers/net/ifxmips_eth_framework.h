/******************************************************************************
**
** FILE NAME    : ifxmips_eth_framework.h
** PROJECT      : UEIP
** MODULES      : ETH
**
** DATE         : 2 Nov 2010
** AUTHOR       : Xu Liang
** DESCRIPTION  : IFX ETH driver framework header file
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 02 NOV 2010  Xu Liang        Init Version
*******************************************************************************/



#ifndef IFXMIPS_ETH_FRAMEWORK_H
#define IFXMIPS_ETH_FRAMEWORK_H



#include <asm/ifx/ifx_eth_framework.h>



/*
 * ####################################
 *              Data Type
 * ####################################
 */

struct ifx_eth_fw_priv_data {
    struct net_device              *dev;
    void                           *priv;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
    struct net_device_ops           netdev_ops;
    struct napi_struct              napi;
#endif

    ifx_eth_fw_poll_ret_t (*poll)(struct net_device *dev, int work_to_do, int *work_done);
    int (*open)(struct net_device *dev);
    int (*stop)(struct net_device *dev);
    struct net_device_stats *(*get_stats)(struct net_device *dev);

    struct list_head                list;
    int                             sizeof_priv;
    unsigned int                    malloc_size;
#define ETHFW_PRIV_DATA_FLAG_ALLOC          0
#define ETHFW_PRIV_DATA_FLAG_REG            1
#define ETHFW_PRIV_DATA_FLAG_REG_NETDEV     2
    unsigned long                   flags;

    struct net_device_stats         stats;
    struct net_device_stats         stats_bak;
};



#endif  //  IFXMIPS_ETH_FRAMEWORK_H
