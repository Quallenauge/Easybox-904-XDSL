/**
** FILE NAME    : ifxmips_mmc_core.h
** PROJECT      : IFX UEIP
** MODULES      : MMC module 
** DATE         : 08 July 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : IFX Cross-Platform MMC driver header file
** COPYRIGHT    :       Copyright (c) 2009
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date                        $Author                 $Comment
** 08 July 2009         Reddy Mallikarjuna              Initial release
*******************************************************************************/

#ifndef _IFXMIPS_MMC_CORE_H_
#define _IFXMIPS_MMC_CORE_H_

/*!
  \file ifxmips_mmc_core.h
  \ingroup IFX_MMC_DRV
  \brief Header file for IFX MMC core driver 
*/

/*!
  \defgroup IFX_MMC_CORE_API Externl APIs
  \ingroup IFX_MMC_DRV
  \brief Externl APIs for other modules.
*/
/** ==========================  */
/* Include files                */
/** =========================== */
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>

struct request;
struct task_struct;

/*!
  \addtogroup IFX_MMC_DRV_STRUCTURE
 */
/*@{*/

/*! \struct ifx_mmc_queue
* \brief This structure is used to handle the MMC queue
*/
struct ifx_mmc_queue {
    struct mmc_card         *card;  /*!< pointer a structure with defined mmc card*/
    struct task_struct      *thread;    /*!< pointer a structure with defined test struct*/
    struct semaphore        thread_sem; /*!< structure with defined semaphre*/
    struct mutex		    mutex; /*!< structure with defined mutex */
    unsigned int            flags;      /*!< mmc queue flags */
    struct request          *req;   /*!< pointer a structure with defined request */
    int                     (*prep_fn)(struct ifx_mmc_queue *, struct request *); /*!< prepare a call back function to issue a request to handle the data */
    int                     (*issue_fn)(struct ifx_mmc_queue *, struct request *); /*!<issue a request to handle the data transfer */
    void                    *data;      /*!< pointer a data variable which define generic pointer */
    struct request_queue    *queue;     /*!< pointer a structure with defined request queue */
    struct scatterlist      *sg;    /*!< pointer a structure with defined scatterlist */
};
/* @} */

/*! \struct mmc_blk_data
* \brief This structure is used to handle mmc block data
*/
struct mmc_blk_data {
    spinlock_t	            lock;   /*!< spin lock  */
    struct gendisk	        *disk;  /*!< pointer a structure with defined getdisk */
    struct ifx_mmc_queue    queue;  /*!<  pointer a structure with defined ifx mmc queue */
    unsigned int	        usage;  /*!< This slot is already used or not */
    unsigned int	        block_bits; /*!< block size */
    unsigned int	        read_only;  /*!< The disk is ready only */
};

#define ifx_mmc_card_release_host(c)        ifx_mmc_release_host((c)->host)
extern void ifx_mmc_init_card(struct mmc_card *card, struct mmc_host *host);
extern int ifx_mmc_register_card(struct mmc_card *card);
extern void ifx_mmc_remove_card(struct mmc_card *card);
struct mmc_host *ifx_mmc_alloc_host_sysfs(struct mmc_host *host, struct device *dev);
extern int ifx_mmc_add_host_sysfs(struct mmc_host *host);
extern void ifx_mmc_remove_host_sysfs(struct mmc_host *host);
extern void ifx_mmc_free_host_sysfs(struct mmc_host *host);
extern int ifx_mmc_schedule_delayed_work(struct delayed_work *work, unsigned long delay);
extern void ifx_mmc_flush_scheduled_work(void);
extern int ifx_mmc_init_queue(struct ifx_mmc_queue *, struct mmc_card *, spinlock_t *);
extern void ifx_sd_card_cleanup_queue(struct ifx_mmc_queue *);
extern void ifx_mmc_queue_suspend(struct ifx_mmc_queue *);
extern void ifx_mmc_queue_resume(struct ifx_mmc_queue *);
extern int ifx_mmc_register_driver(struct mmc_driver *);
extern void ifx_mmc_unregister_driver(struct mmc_driver *);
extern int __ifx_mmc_claim_host(struct mmc_host *host, struct mmc_card *card);

static inline int ifx_mmc_card_claim_host(struct mmc_card *card) {
    return __ifx_mmc_claim_host(card->host, card);
}
extern int ifx_mmc_wait_for_req(struct mmc_host *, struct mmc_request *);
extern int ifx_mmc_wait_for_cmd(struct mmc_host *, struct mmc_command *, int);
extern int ifx_mmc_wait_for_app_cmd(struct mmc_host *, unsigned int, struct mmc_command *, int);
extern void ifx_mmc_set_data_timeout(struct mmc_data *, const struct mmc_card *, int);

static inline void ifx_mmc_claim_host(struct mmc_host *host)
{
    __ifx_mmc_claim_host(host, (struct mmc_card *)-1);
}
extern void ifx_mmc_release_host(struct mmc_host *host);

#endif /* _IFXMIPS_MMC_CORE_H_ */
