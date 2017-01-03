/**
** FILE NAME    : ifxmips_mmc.h
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
#ifndef _IFXMIPS_MMC_H_
#define _IFXMIPS_MMC_H_

/*!
  \defgroup IFX_MMC_DRV UEIP Project - MMC host driver (SD Card)
  \brief UEIP Project - MMC driver module, supports IFX CPE platforms(Danube/ASE/ARx/VRx).
*/

/*!
  \defgroup IFX_MMC_DRV_API Externl APIs
  \ingroup IFX_MMC_DRV
  \brief Externl APIs for other modules.
*/

/*!
  \defgroup IFX_MMC_DRV_STRUCTURE Driver Structures
  \ingroup IFX_MMC_DRV
  \brief Definitions/Structures of MMC module (for SD card).
*/

/*!
  \file ifxmips_mmc.h
  \ingroup IFX_MMC_DRV
  \brief Header file for IFX MMC host driver 
*/


/** ==========================  */
/* Include files                */
/** =========================== */
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

/*!
  \addtogroup IFX_MMC_DRV_STRUCTURE
 */
/*@{*/

/*! \struct ifx_sdio_host
* \brief This structure is used to handle the MMC host controller
*/
struct ifx_sdio_host {
    void __iomem            *base;  /*!< io memory map */
    struct mmc_request      *mrq;  /*!< pointer a structure with defined mmc request*/
    struct mmc_command      *cmd;  /*!< pointer a structure with defined mmc command */
    struct mmc_data         *data; /*!< pointer a structure with defined mmc data */
    struct mmc_host         *mmc;  /*!< pointer a structure with defined mmc host */
    unsigned int            data_xfered; /*!< transfered bytes */
    spinlock_t              lock; /*!< host busy lock*/
    unsigned int            mclk; /*!< max host support clock */
    unsigned int            cclk; /*!< current clock */
    unsigned int            pwr;  /*!< power */
    struct mmc_platform_data *plat;  /*!< pointer a structure with defined mmc platform data*/
    struct timer_list       timer; /*!< struct timer */
    unsigned int            prev_stat; /*!< Previous status */
    unsigned int            sg_len;  /*!< scatter buffer length*/
    struct scatterlist      *sg_ptr; /*!< pointer a structure with defined scatterlist*/
    unsigned int            sg_off; /*!< scatter offset */
    unsigned int            size; /*!< size of packet*/
};

/*! \typedef ifx_sdio_controller_priv_t
* \brief This structure is used to handle the MMC controller private data
*/
typedef struct ifx_sdio_controller_priv {
    struct dma_device_info  *dma_device;    /*!< pointer a structure with defined dma device info*/
    unsigned int            mclk_speed;     /*!< mmc clock speed */
    unsigned int            current_speed;  /*!< current clock frequncy */
    struct ifx_sdio_host *host;             /*!< pointer a structure with defined ifx sdio host*/
} ifx_sdio_controller_priv_t;

/* @} */

static inline void ifx_sdio_init_sg(struct ifx_sdio_host *host, struct mmc_data *data)
{
    /*Ideally, we want the higher levels to pass us a scatter list. */
    host->sg_len = data->sg_len;
    host->sg_ptr = data->sg;
    host->sg_off = 0;
}

static inline int ifx_sdio_next_sg(struct ifx_sdio_host *host)
{
    host->sg_ptr++;
    host->sg_off = 0;
    return --host->sg_len;
}

static inline char *ifx_sdio_kmap_atomic(struct ifx_sdio_host *host, unsigned long *flags)
{
    struct scatterlist *sg = host->sg_ptr;
#if 0
    local_irq_save(*flags);
    return kmap(sg->page ) + sg->offset;
#else
   return kmap_atomic(sg->page, KM_BIO_SRC_IRQ) + sg->offset;
#endif
}

static inline void ifx_sdio_kunmap_atomic(struct ifx_sdio_host *host, void *buffer, unsigned long *flags)
{ 
#if 0
    kunmap(buffer);
    local_irq_restore(*flags);
#else
    kunmap_atomic(buffer, KM_BIO_SRC_IRQ);
#endif
}

static inline void *ifx_mmc_priv(struct mmc_host *host)
{
    return (void *)host->private;
}

#define ifx_mmc_dev(x)      ((x)->parent)
#define ifx_mmc_hostname(x) ((x)->class_dev.bus_id)

extern struct mmc_host *ifx_mmc_alloc_host(struct mmc_host *host, struct device *dev);
extern int ifx_mmc_add_host(struct mmc_host *);
extern void ifx_mmc_remove_host(struct mmc_host *);
extern void ifx_mmc_free_host(struct mmc_host *);
extern int ifx_mmc_suspend_host(struct mmc_host *, pm_message_t);
extern int ifx_mmc_resume_host(struct mmc_host *);
extern void ifx_mmc_detect_change(struct mmc_host *, unsigned long delay);
extern void ifx_mmc_request_done(struct mmc_host *, struct mmc_request *);
extern int ifx_sdio_controller_set_ops (int type, uint32_t data);

#endif /* _IFXMIPS_MMC_H_ */
