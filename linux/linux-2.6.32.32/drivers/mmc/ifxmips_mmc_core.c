/**
** FILE NAME    : ifxmips_mmc_core.c
** PROJECT      : IFX UEIP
** MODULES      : MMC module 
** DATE         : 08 July 2009
** AUTHOR       : Reddy Mallikarjuna 
** DESCRIPTION  : IFX Cross-Platform MMC core device driver file
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
/*!
  \file ifxmips_mmc_core.c
  \ingroup IFX_MMC_DRV
  \brief MMC core driver for SD Card
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pagemap.h>
#include <linux/err.h>
#include <asm/scatterlist.h>
#include <linux/scatterlist.h>
#include <linux/idr.h>
#include <linux/workqueue.h>
#include <linux/mmc/protocol.h>
#include <linux/proc_fs.h>

#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
/*Platform specific header files */
#include "ifxmips_mmc.h"
#include "ifxmips_mmc_core.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#define CMD_RETRIES	3

#define IFX_MMC_DRV_MODULE_NAME         "ifxmips_mmc_core"
#define IFX_DRV_MODULE_VERSION          "1.0.1"
static char version[] __devinitdata =
        IFX_MMC_DRV_MODULE_NAME ".c:v" IFX_DRV_MODULE_VERSION " \n";
/*proc file directory*/
static struct proc_dir_entry*   ifx_mmc_core_dir;

#define dev_to_mmc_card(d)      container_of(d, struct mmc_card, dev)
#define to_mmc_driver(d)        container_of(d, struct mmc_driver, drv)
#define cls_dev_to_mmc_host(d)  container_of(d, struct mmc_host, class_dev)

#define MMC_ATTR(name, fmt, args...)                                    \
static ssize_t mmc_##name##_show (struct device *dev, struct device_attribute *attr, char *buf) \
{                                                                       \
        struct mmc_card *card = dev_to_mmc_card(dev);                   \
        return sprintf(buf, fmt, args);                                 \
}

MMC_ATTR(cid, "%08x%08x%08x%08x\n", card->raw_cid[0], card->raw_cid[1],
        card->raw_cid[2], card->raw_cid[3]);
MMC_ATTR(csd, "%08x%08x%08x%08x\n", card->raw_csd[0], card->raw_csd[1],
        card->raw_csd[2], card->raw_csd[3]);
MMC_ATTR(scr, "%08x%08x\n", card->raw_scr[0], card->raw_scr[1]);
MMC_ATTR(date, "%02d/%04d\n", card->cid.month, card->cid.year);
MMC_ATTR(fwrev, "0x%x\n", card->cid.fwrev);
MMC_ATTR(hwrev, "0x%x\n", card->cid.hwrev);
MMC_ATTR(manfid, "0x%06x\n", card->cid.manfid);
MMC_ATTR(name, "%s\n", card->cid.prod_name);
MMC_ATTR(oemid, "0x%04x\n", card->cid.oemid);
MMC_ATTR(serial, "0x%08x\n", card->cid.serial);

#define MMC_ATTR_RO(name) __ATTR(name, S_IRUGO, mmc_##name##_show, NULL)

/*OCR Bit positions to 10s of Vdd mV. */
static const unsigned short mmc_ocr_bit_to_vdd[] = {
	150,	155,	160,	165,	170,	180,	190,	200,
	210,	220,	230,	240,	250,	260,	270,	280,
	290,	300,	310,	320,	330,	340,	350,	360
};

static const unsigned int tran_exp[] = {
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const unsigned int tacc_exp[] = {
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

/*!
  \fn void ifx_mmc_request_done(struct mmc_host *host, struct mmc_request *mrq)
  \ingroup  IFX_MMC_CORE_API 
  \brief Processing of MMC request done.
  
  MMC drivers should call this function when they have completed their processing of a request.
  
  \param    host     --> pointer to MMC Host structure
  \param    mrq      --> pointer to MMC request structure
  \return   None	
 */
void ifx_mmc_request_done(struct mmc_host *host, struct mmc_request *mrq)
{
	struct mmc_command *cmd = mrq->cmd;
	struct ifx_sdio_host *host1 = ifx_mmc_priv(host);
	int err = cmd->error;
//	printk("%s[%d] ......\n",__func__,__LINE__);
	pr_debug("%s: req done (CMD%u): %d/%d/%d: %08x %08x %08x %08x\n", \
	    ifx_mmc_hostname(host), cmd->opcode, err, \
	    mrq->data ? mrq->data->error : 0, \
	    mrq->stop ? mrq->stop->error : 0, \
		 cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3]);

	if (err && cmd->retries) {
		cmd->retries--;
		cmd->error = 0;
		/* ifx_sdio_request() requeires this to be NULL */
		host1->mrq = NULL; 
		host->ops->request(host, mrq);
	} else if (mrq->done) {
		mrq->done(mrq);
	}
}
EXPORT_SYMBOL(ifx_mmc_request_done);

/*!
  \fn void ifx_mmc_start_request(struct mmc_host *host, struct mmc_request *mrq)
  \ingroup  IFX_MMC_CORE_API 
  \brief start command request on a host.
  
  Queue a command on the specified host.  
  We expect the caller to be holding the host lock with interrupts disabled
  
  \param    host     --> pointer to MMC Host structure
  \param    mrq      --> pointer to MMC request structure
  \return   None	
 */
void
ifx_mmc_start_request(struct mmc_host *host, struct mmc_request *mrq)
{
	pr_debug("%s: starting CMD%u arg %08x flags %08x\n",
		 ifx_mmc_hostname(host), mrq->cmd->opcode,
		 mrq->cmd->arg, mrq->cmd->flags);


	WARN_ON(host->card_busy == NULL);

	mrq->cmd->error = 0;
	mrq->cmd->mrq = mrq;
	if (mrq->data) {
		mrq->cmd->data = mrq->data;
		mrq->data->error = 0;
		mrq->data->mrq = mrq;
		if (mrq->stop) {
			mrq->data->stop = mrq->stop;
			mrq->stop->error = 0;
			mrq->stop->mrq = mrq;
		}
	}
	host->ops->request(host, mrq);
}

/** MMC wait done */
static void mmc_wait_done(struct mmc_request *mrq)
{
	complete(mrq->done_data);
}

/*!
  \fn int ifx_mmc_wait_for_req(struct mmc_host *host, struct mmc_request *mrq)
  \ingroup  IFX_MMC_CORE_API 
  \brief Wait for request from host
  
  It will wait untill request is done.
  once request is done on host side, then release the host.
  
  \param    host     --> pointer to MMC Host structure
  \param    mrq      --> pointer to MMC request structure
  \return   0	
 */
int ifx_mmc_wait_for_req(struct mmc_host *host, struct mmc_request *mrq)
{
    struct ifx_sdio_host *host1 = ifx_mmc_priv(host);
	DECLARE_COMPLETION_ONSTACK(complete);

	mrq->done_data = &complete;
	mrq->done = mmc_wait_done;

	ifx_mmc_start_request(host, mrq);

	wait_for_completion(&complete);
	host1->mrq = NULL;
	host1->cmd = NULL;
	return 0;
}
EXPORT_SYMBOL(ifx_mmc_wait_for_req);

/*!
  \fn int ifx_mmc_wait_for_cmd(struct mmc_host *host, struct mmc_command *cmd, int retries)
  \ingroup  IFX_MMC_CORE_API 
  \brief Start a command and wait for completion
  
  Start a new MMC command for a host, and wait for the command
  to complete.  Return any error that occurred while the command
  was executing.  Do not attempt to parse the response.
  
  \param    host    --> pointer to MMC Host structure
  \param    cmd     --> pointer to MMC command structure
  \param    retries --> maximum number of retries
  \return   error code	
 */
int ifx_mmc_wait_for_cmd(struct mmc_host *host, struct mmc_command *cmd, int retries)
{
	struct mmc_request mrq;

	BUG_ON(host->card_busy == NULL);

	memset(&mrq, 0, sizeof(struct mmc_request));

	memset(cmd->resp, 0, sizeof(cmd->resp));
	cmd->retries = retries;

	mrq.cmd = cmd;
	cmd->data = NULL;

	ifx_mmc_wait_for_req(host, &mrq);

	return cmd->error;
}
EXPORT_SYMBOL(ifx_mmc_wait_for_cmd);

/*!
  \fn int ifx_mmc_wait_for_app_cmd(struct mmc_host *host, unsigned int rca, struct mmc_command *cmd, int retries)
  \ingroup  IFX_MMC_CORE_API 
  \brief start an application command and wait for completion
  
  Sends a MMC_APP_CMD, checks the card response, sends the command
  in the parameter and waits for it to complete. Return any error
  that occurred while the command was executing.  Do not attempt to
  parse the response.
  
  \param    host    --> pointer to MMC Host structure
  \param    rca     --> RCA to send MMC_APP_CMD to
  \param    cmd     --> pointer to MMC command structure
  \param    retries --> maximum number of retries
  \return   error code 
 */
int ifx_mmc_wait_for_app_cmd(struct mmc_host *host, unsigned int rca,
	struct mmc_command *cmd, int retries)
{
    struct mmc_request mrq;
    struct mmc_command appcmd;
    int i, err;
    
    BUG_ON(host->card_busy == NULL);
	BUG_ON(retries < 0);
	err = MMC_ERR_INVALID;
	
	/*
	 * We have to resend MMC_APP_CMD for each attempt so
	 * we cannot use the retries field in mmc_command.
	 */
    for (i = 0;i <= retries;i++) {
        memset(&mrq, 0, sizeof(struct mmc_request));
        appcmd.opcode = MMC_APP_CMD;
        appcmd.arg = rca << 16;
        appcmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
		appcmd.retries = 0;
		memset(appcmd.resp, 0, sizeof(appcmd.resp));
		appcmd.data = NULL;

		mrq.cmd = &appcmd;
		appcmd.data = NULL;

		ifx_mmc_wait_for_req(host, &mrq);

		if (appcmd.error) {
		    /* printk("%s[%d] Err (0x%08x)!! \n",__FUNCTION__,__LINE__,appcmd.error); */
		    err = appcmd.error;
		    continue;
		}
		/* Check that card supported application commands */
		if (!(appcmd.resp[0] & R1_APP_CMD)) {
		    printk("%s[%d] Err!!(0x%08x) \n",__FUNCTION__,__LINE__, appcmd.resp[0]);
		    return MMC_ERR_FAILED;
		}
		memset(&mrq, 0, sizeof(struct mmc_request));
		memset(cmd->resp, 0, sizeof(cmd->resp));
		cmd->retries = 0;
		mrq.cmd = cmd;
		cmd->data = NULL;
		ifx_mmc_wait_for_req(host, &mrq);
		err = cmd->error;
		if (cmd->error == MMC_ERR_NONE)
		    break;
	}
	return err;
}

/*!
  \fn void ifx_mmc_set_data_timeout(struct mmc_data *data, const struct mmc_card *card, int write)
  \ingroup  IFX_MMC_CORE_API 
  \brief Set the timeout for a data command
  
  \param    data    --> pointer to MMC data structure
  \param    card    --> pointer to MMC card structure associated with the data transfer
  \param    write   --> flag to differentiate reads from writes
  \return   None
 */
void ifx_mmc_set_data_timeout(struct mmc_data *data, const struct mmc_card *card, int write)
{
    unsigned int mult;
    /* SD cards use a 100 multiplier rather than 10  */
    mult = mmc_card_sd(card) ? 100 : 10;
    /* Scale up the multiplier (and therefore the timeout) by the r2w factor for writes.*/
    if (write)
        mult <<= card->csd.r2w_factor;
    data->timeout_ns = card->csd.tacc_ns * mult;
    data->timeout_clks = card->csd.tacc_clks * mult;
    /* SD cards also have an upper limit on the timeout.*/
    if (mmc_card_sd(card)) {
        unsigned int timeout_us, limit_us;
        timeout_us = data->timeout_ns / 1000;
        timeout_us += data->timeout_clks * 1000 /
			(card->host->ios.clock / 1000);
	    if (write)
	        limit_us = 250000;
		else
			limit_us = 100000;

		if (timeout_us > limit_us) {
			data->timeout_ns = limit_us * 1000;
			data->timeout_clks = 0;
		}
	}
}
EXPORT_SYMBOL(ifx_mmc_set_data_timeout);

static int ifx_mmc_select_card(struct mmc_host *host, struct mmc_card *card);

/*!
  \fn int __ifx_mmc_claim_host(struct mmc_host *host, struct mmc_card *card)
  \ingroup  IFX_MMC_CORE_API 
  \brief Claim a host for a set of operations
  
  Claim a host for a set of operations.  If a valid card
  is passed and this wasn't the last card selected, select
  the card before returning.
  Note: should use ifx_mmc_card_claim_host or ifx_mmc_claim_host.
  
  \param    host    --> pointer to MMC Host structure
  \param    card     --> pointer to MMC card structure
  \return   error code 
 */
int __ifx_mmc_claim_host(struct mmc_host *host, struct mmc_card *card)
{
	DECLARE_WAITQUEUE(wait, current);
	unsigned long flags;
	int err = 0;

	add_wait_queue(&host->wq, &wait);
	spin_lock_irqsave(&host->lock, flags);
	while (1) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (host->card_busy == NULL)
			break;
		spin_unlock_irqrestore(&host->lock, flags);
		schedule();
		spin_lock_irqsave(&host->lock, flags);
	}
	set_current_state(TASK_RUNNING);
	host->card_busy = card;
	spin_unlock_irqrestore(&host->lock, flags);
	remove_wait_queue(&host->wq, &wait);

	if (card != (void *)-1) {
		err = ifx_mmc_select_card(host, card);
		if (err != MMC_ERR_NONE) {
			printk("%s[%d] Err!!!  \n",__FUNCTION__,__LINE__);
			return err;
		}
	}

	return err;
}
EXPORT_SYMBOL(__ifx_mmc_claim_host);

/*!
  \fn void ifx_mmc_release_host(struct mmc_host *host)
  \ingroup  IFX_MMC_CORE_API 
  \brief Release a host
  
  Release a MMC host, allowing others to claim the host for their operations.
  
  \param    host    --> pointer to MMC Host structure
  \return   None
 */
void ifx_mmc_release_host(struct mmc_host *host)
{
	unsigned long flags;

	BUG_ON(host->card_busy == NULL);

	spin_lock_irqsave(&host->lock, flags);
	host->card_busy = NULL;
	spin_unlock_irqrestore(&host->lock, flags);

	wake_up(&host->wq);
}
EXPORT_SYMBOL(ifx_mmc_release_host);

static inline void ifx_mmc_set_ios(struct mmc_host *host)
{
	struct mmc_ios *ios = &host->ios;

	pr_debug("%s: clock %uHz busmode %u powermode %u cs %u Vdd %u width %u\n",
		 ifx_mmc_hostname(host), ios->clock, ios->bus_mode,
		 ios->power_mode, ios->chip_select, ios->vdd,
		 ios->bus_width);
	
	host->ops->set_ios(host, ios);
}

static int ifx_mmc_select_card(struct mmc_host *host, struct mmc_card *card)
{
	int err;
	struct mmc_command cmd;

	BUG_ON(host->card_busy == NULL);

	if (host->card_selected == card)
	{
		return MMC_ERR_NONE;
	}

	host->card_selected = card;

	cmd.opcode = MMC_SELECT_CARD;
	cmd.arg = card->rca << 16;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

	err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
	if (err != MMC_ERR_NONE) {
		printk("%s[%d] Err!! \n",__FUNCTION__,__LINE__);
		return err;
	}

	/*
	 * We can only change the bus width of SD cards when
	 * they are selected so we have to put the handling
	 * here.
	 *
	 * The card is in 1 bit mode by default so
	 * we only need to change if it supports the
	 * wider version.
	 */
	if (mmc_card_sd(card) &&
		(card->scr.bus_widths & SD_SCR_BUS_WIDTH_4)) {

		/*
		* Default bus width is 1 bit.
		*/
		host->ios.bus_width = MMC_BUS_WIDTH_1;

		if (host->caps & MMC_CAP_4_BIT_DATA) {
			struct mmc_command cmd;
			cmd.opcode = SD_APP_SET_BUS_WIDTH;
			cmd.arg = SD_BUS_WIDTH_4;
			cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
			
			err = ifx_mmc_wait_for_app_cmd(host, card->rca, &cmd,
				CMD_RETRIES);
			if (err != MMC_ERR_NONE) {
				printk("%s[%d] Err!! \n",__FUNCTION__,__LINE__);
				return err;
			}

			host->ios.bus_width = MMC_BUS_WIDTH_4;
		}
	}

	ifx_mmc_set_ios(host);

	return MMC_ERR_NONE;
}

/*
 * Ensure that no card is selected.
 */
static void ifx_mmc_deselect_cards(struct mmc_host *host)
{
	struct mmc_command cmd;
	if (host->card_selected) {
		host->card_selected = NULL;
		cmd.opcode = MMC_SELECT_CARD;
		cmd.arg = 0;
		cmd.flags = MMC_RSP_NONE | MMC_CMD_AC;
		ifx_mmc_wait_for_cmd(host, &cmd, 0);
	}
}


static inline void ifx_mmc_delay(unsigned int ms)
{
	if (ms < 1000 / HZ) {
		cond_resched();
		mdelay(ms);
	} else {
		msleep(ms);
	}
}

/*
 * Mask off any voltages we don't support and select
 * the lowest voltage
 */
static u32 ifx_mmc_select_voltage(struct mmc_host *host, u32 ocr)
{
	int bit;
	ocr &= host->ocr_avail;
	bit = ffs(ocr);
	if (bit) {
		bit -= 1;
		ocr &= 3 << bit;
	} else {
		ocr = 0;
	}
	return ocr;
}

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static void ifx_mmc_decode_cid(struct mmc_card *card)
{
	u32 *resp = card->raw_cid;
	memset(&card->cid, 0, sizeof(struct mmc_cid));
	if (mmc_card_sd(card)) {
		/*
		 * SD doesn't currently have a version field so we will
		 * have to assume we can parse this.
		 */
		card->cid.manfid		= UNSTUFF_BITS(resp, 120, 8);
		card->cid.oemid			= UNSTUFF_BITS(resp, 104, 16);
		card->cid.prod_name[0]		= UNSTUFF_BITS(resp, 96, 8);
		card->cid.prod_name[1]		= UNSTUFF_BITS(resp, 88, 8);
		card->cid.prod_name[2]		= UNSTUFF_BITS(resp, 80, 8);
		card->cid.prod_name[3]		= UNSTUFF_BITS(resp, 72, 8);
		card->cid.prod_name[4]		= UNSTUFF_BITS(resp, 64, 8);
		card->cid.hwrev			= UNSTUFF_BITS(resp, 60, 4);
		card->cid.fwrev			= UNSTUFF_BITS(resp, 56, 4);
		card->cid.serial		= UNSTUFF_BITS(resp, 24, 32);
		card->cid.year			= UNSTUFF_BITS(resp, 12, 8);
		card->cid.month			= UNSTUFF_BITS(resp, 8, 4);

		card->cid.year += 2000; /* SD cards year offset */
	} else {
		/*
		 * The selection of the format here is based upon published
		 * specs from sandisk and from what people have reported.
		 */
		switch (card->csd.mmca_vsn) {
		case 0: /* MMC v1.0 - v1.2 */
		case 1: /* MMC v1.4 */
			card->cid.manfid	= UNSTUFF_BITS(resp, 104, 24);
			card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
			card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
			card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
			card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
			card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
			card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
			card->cid.prod_name[6]	= UNSTUFF_BITS(resp, 48, 8);
			card->cid.hwrev		= UNSTUFF_BITS(resp, 44, 4);
			card->cid.fwrev		= UNSTUFF_BITS(resp, 40, 4);
			card->cid.serial	= UNSTUFF_BITS(resp, 16, 24);
			card->cid.month		= UNSTUFF_BITS(resp, 12, 4);
			card->cid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
			break;

		case 2: /* MMC v2.0 - v2.2 */
		case 3: /* MMC v3.1 - v3.3 */
		case 4: /* MMC v4 */
			card->cid.manfid	= UNSTUFF_BITS(resp, 120, 8);
			card->cid.oemid		= UNSTUFF_BITS(resp, 104, 16);
			card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
			card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
			card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
			card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
			card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
			card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
			card->cid.serial	= UNSTUFF_BITS(resp, 16, 32);
			card->cid.month		= UNSTUFF_BITS(resp, 12, 4);
			card->cid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
			break;

		default:
			printk("%s: card has unknown MMCA version %d\n",
				ifx_mmc_hostname(card->host), card->csd.mmca_vsn);
			mmc_card_set_bad(card);
			break;
		}
	}
}

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static void ifx_mmc_decode_csd(struct mmc_card *card)
{
	struct mmc_csd *csd = &card->csd;
	unsigned int e, m, csd_struct;
	u32 *resp = card->raw_csd;

	if (mmc_card_sd(card)) {
		csd_struct = UNSTUFF_BITS(resp, 126, 2);
		if (csd_struct != 0) {
			printk("%s: unrecognised CSD structure version %d\n",
				ifx_mmc_hostname(card->host), csd_struct);
			mmc_card_set_bad(card);
			return;
		}

		m = UNSTUFF_BITS(resp, 115, 4);
		e = UNSTUFF_BITS(resp, 112, 3);
		csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		csd->capacity	  = (1 + m) << (e + 2);

		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
	} else {
		/*
		 * We only understand CSD structure v1.1 and v1.2.
		 * v1.2 has extra information in bits 15, 11 and 10.
		 */
		csd_struct = UNSTUFF_BITS(resp, 126, 2);
		if (csd_struct != 1 && csd_struct != 2) {
			printk("%s: unrecognised CSD structure version %d\n",
				ifx_mmc_hostname(card->host), csd_struct);
			mmc_card_set_bad(card);
			return;
		}

		csd->mmca_vsn	 = UNSTUFF_BITS(resp, 122, 4);
		m = UNSTUFF_BITS(resp, 115, 4);
		e = UNSTUFF_BITS(resp, 112, 3);
		csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		csd->capacity	  = (1 + m) << (e + 2);

		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
	}
}

/*
 * Given a 64-bit response, decode to our card SCR structure.
 */
static void ifx_mmc_decode_scr(struct mmc_card *card)
{
	struct sd_scr *scr = &card->scr;
	unsigned int scr_struct;
	u32 resp[4];

	BUG_ON(!mmc_card_sd(card));

	resp[3] = card->raw_scr[1];
	resp[2] = card->raw_scr[0];

	resp[1] = 0;
	resp[0] = 0 ;
	scr->bus_widths = UNSTUFF_BITS(resp, 48, 4);

	scr_struct = UNSTUFF_BITS(resp, 60, 4);
	if (scr_struct != 0) {
		printk("%s: unrecognised SCR structure version %d\n",
			ifx_mmc_hostname(card->host), scr_struct);
		mmc_card_set_bad(card);
		return;
	}

	scr->sda_vsn = UNSTUFF_BITS(resp, 56, 4);
	scr->bus_widths = UNSTUFF_BITS(resp, 48, 4);
	
	printk("%s[%d] bus width: %d\n",__func__,__LINE__,scr->bus_widths);
}

/*
 * Locate a MMC card on this MMC host given a raw CID.
 */
static struct mmc_card *ifx_mmc_find_card(struct mmc_host *host, u32 *raw_cid)
{
	struct mmc_card *card;
	list_for_each_entry(card, &host->cards, node) {
		if (memcmp(card->raw_cid, raw_cid, sizeof(card->raw_cid)) == 0)
			return card;
	}
	return NULL;
}

/*
 * Allocate a new MMC card, and assign a unique RCA.
 */
static struct mmc_card *
ifx_mmc_alloc_card(struct mmc_host *host, u32 *raw_cid, unsigned int *frca)
{
	struct mmc_card *card, *c;
	unsigned int rca = *frca;
	card = kmalloc(sizeof(struct mmc_card), GFP_KERNEL);
	if (!card) {
		printk("%s[%d] Err!! \n",__FUNCTION__,__LINE__);
		return ERR_PTR(-ENOMEM);
	}
	ifx_mmc_init_card(card, host);
	memcpy(card->raw_cid, raw_cid, sizeof(card->raw_cid));
 again:
	list_for_each_entry(c, &host->cards, node) {
		if (c->rca == rca) {
			rca++;
			goto again;
		}
	}
	card->rca = rca;
	*frca = rca;
	return card;
}

/*
 * Tell attached cards to go to IDLE state
 */
static void ifx_mmc_idle_cards(struct mmc_host *host)
{
	struct mmc_command cmd;
	cmd.opcode = MMC_GO_IDLE_STATE;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_NONE | MMC_CMD_BC;
	ifx_mmc_wait_for_cmd(host, &cmd, 0);
	ifx_mmc_delay(1);
}

/*
 * Apply power to the MMC stack.  This is a two-stage process.
 * First, we enable power to the card without the clock running.
 * We then wait a bit for the power to stabilise.  Finally,
 * enable the bus drivers and clock to the card.
 *
 * We must _NOT_ enable the clock prior to power stablising.
 *
 * If a host does all the power sequencing itself, ignore the
 * initial MMC_POWER_UP stage.
 */
static void ifx_mmc_power_up(struct mmc_host *host)
{

	host->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
	host->ios.power_mode = MMC_POWER_UP;
	host->ios.bus_width = MMC_BUS_WIDTH_1;
	ifx_mmc_set_ios(host);
	ifx_mmc_delay(1);
	host->ios.clock = host->f_min;
	host->ios.power_mode = MMC_POWER_ON;
	ifx_mmc_set_ios(host);
	ifx_mmc_delay(2);
}

static void ifx_mmc_power_off(struct mmc_host *host)
{
	host->ios.clock = 0;
	host->ios.vdd = 0;
	host->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
	host->ios.power_mode = MMC_POWER_OFF;
	host->ios.bus_width = MMC_BUS_WIDTH_1;
}

static int ifx_mmc_send_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr)
{
	struct mmc_command cmd;
	int i, err = 0;
	cmd.opcode = MMC_SEND_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_R3 | MMC_CMD_BCR;
	for (i = 100; i; i--) {
		err = ifx_mmc_wait_for_cmd(host, &cmd, 0);
		if (err != MMC_ERR_NONE)
			break;
		if (cmd.resp[0] & MMC_CARD_BUSY || ocr == 0)
			break;
		err = MMC_ERR_TIMEOUT;
		ifx_mmc_delay(10);
	}
	if (rocr)
		*rocr = cmd.resp[0];
	return err;
}

static int ifx_mmc_send_app_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr)
{
	struct mmc_command cmd;
	int i, err = 0;
	cmd.opcode = SD_APP_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_R3 | MMC_CMD_BCR;
	for (i = 100; i; i--) {
		err = ifx_mmc_wait_for_app_cmd(host, 0, &cmd, CMD_RETRIES);
		if (err != MMC_ERR_NONE) {
		    break;
		}
		if (cmd.resp[0] & MMC_CARD_BUSY || ocr == 0) {
		    break;
		}
		err = MMC_ERR_TIMEOUT;
		ifx_mmc_delay(10);
	}
	if (rocr) {
	    *rocr = cmd.resp[0];
	}
	return err;
}

/*
 * Discover cards by requesting their CID.  If this command
 * times out, it is not an error; there are no further cards
 * to be discovered.  Add new cards to the list.
 *
 * Create a mmc_card entry for each discovered card, assigning
 * it an RCA, and save the raw CID for decoding later.
 */
static void ifx_mmc_discover_cards(struct mmc_host *host)
{
	struct mmc_card *card;
	unsigned int first_rca = 1, err;

	while (1) {
		struct mmc_command cmd;
		cmd.opcode = MMC_ALL_SEND_CID;
		cmd.arg = 0;
		cmd.flags = MMC_RSP_R2 | MMC_CMD_BCR;
		err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
		if (err == MMC_ERR_TIMEOUT) {
			err = MMC_ERR_NONE;
			break;
		}
		if (err != MMC_ERR_NONE) {
			printk(KERN_ERR "%s: error requesting CID: %d\n",
				ifx_mmc_hostname(host), err);
			break;
		}
		card = ifx_mmc_find_card(host, cmd.resp);
		if (!card) {
			card = ifx_mmc_alloc_card(host, cmd.resp, &first_rca);
			if (IS_ERR(card)) {
				printk("%s[%d]: Err!!! \n",__FUNCTION__,__LINE__);
				err = PTR_ERR(card);
				break;
			}
			list_add(&card->node, &host->cards);
		}
		card->state &= ~MMC_STATE_DEAD;
		if (host->mode == MMC_MODE_SD) {
			mmc_card_set_sd(card);
			cmd.opcode = SD_SEND_RELATIVE_ADDR;
			cmd.arg = 0;
			cmd.flags = MMC_RSP_R6 | MMC_CMD_BCR;
			err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
			if (err != MMC_ERR_NONE) {
				printk("%s[%d]: Err!!! \n",__FUNCTION__,__LINE__);
				mmc_card_set_dead(card);
			} else {
			    card->rca = cmd.resp[0] >> 16;
			    if (!host->ops->get_ro) {
					printk(KERN_WARNING "%s: host does not "
						"support reading read-only "
						"switch. assuming write-enable.\n",
						ifx_mmc_hostname(host));
				} else {
					if (host->ops->get_ro(host))
						mmc_card_set_readonly(card);
				}
			}
		} else {
			cmd.opcode = MMC_SET_RELATIVE_ADDR;
			cmd.arg = card->rca << 16;
			cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

			err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
			if (err != MMC_ERR_NONE)
				mmc_card_set_dead(card);
		}
	}
}

static void ifx_mmc_read_csds(struct mmc_host *host)
{
	struct mmc_card *card;

	list_for_each_entry(card, &host->cards, node) {
		struct mmc_command cmd;
		int err;

		if (card->state & (MMC_STATE_DEAD|MMC_STATE_PRESENT)) {
			continue;
		}
		cmd.opcode = MMC_SEND_CSD;
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R2 | MMC_CMD_AC;
		err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
		if (err != MMC_ERR_NONE) {
			mmc_card_set_dead(card);
			continue;
		}
		memcpy(card->raw_csd, cmd.resp, sizeof(card->raw_csd));
		ifx_mmc_decode_csd(card);
		ifx_mmc_decode_cid(card);
	}
}

static void ifx_mmc_process_ext_csds(struct mmc_host *host)
{
	int err;
	struct mmc_card *card;
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
	struct scatterlist sg;

	/* As the ext_csd is so large and mostly unused, we don't store the
	* raw block in mmc_card. */
	u8 *ext_csd;
	ext_csd = kmalloc(512, GFP_KERNEL);
	if (!ext_csd) {
		printk("%s: could not allocate a buffer to receive the ext_csd."
		       "mmc v4 cards will be treated as v3.\n",
			ifx_mmc_hostname(host));
		return;
	}

	list_for_each_entry(card, &host->cards, node) {
		if (card->state & (MMC_STATE_DEAD|MMC_STATE_PRESENT))
			continue;
		if (mmc_card_sd(card))
			continue;
		if (card->csd.mmca_vsn < CSD_SPEC_VER_4)
			continue;

		err = ifx_mmc_select_card(host, card);
		if (err != MMC_ERR_NONE) {
			mmc_card_set_dead(card);
			continue;
		}

		memset(&cmd, 0, sizeof(struct mmc_command));
		cmd.opcode = MMC_SEND_EXT_CSD;
		cmd.arg = 0;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		memset(&data, 0, sizeof(struct mmc_data));
		ifx_mmc_set_data_timeout(&data, card, 0);
		data.blksz = 512;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		data.sg = &sg;
		data.sg_len = 1;
		memset(&mrq, 0, sizeof(struct mmc_request));
		mrq.cmd = &cmd;
		mrq.data = &data;
		sg_init_one(&sg, ext_csd, 512);
		ifx_mmc_wait_for_req(host, &mrq);
		if (cmd.error != MMC_ERR_NONE || data.error != MMC_ERR_NONE) {
			mmc_card_set_dead(card);
			continue;
		}

		switch (ext_csd[EXT_CSD_CARD_TYPE]) {
		case EXT_CSD_CARD_TYPE_52 | EXT_CSD_CARD_TYPE_26:
			card->ext_csd.hs_max_dtr = 52000000;
			break;
		case EXT_CSD_CARD_TYPE_26:
			card->ext_csd.hs_max_dtr = 26000000;
			break;
		default:
			/* MMC v4 spec says this cannot happen */
			printk("%s: card is mmc v4 but doesn't support "
			       "any high-speed modes.\n",
				ifx_mmc_hostname(card->host));
			mmc_card_set_bad(card);
			continue;
		}

		/* Activate highspeed support. */
		cmd.opcode = MMC_SWITCH;
		cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
			  (EXT_CSD_HS_TIMING << 16) |
			  (1 << 8) |
			  EXT_CSD_CMD_SET_NORMAL;
		cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;

		err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
		if (err != MMC_ERR_NONE) {
			printk("%s: failed to switch card to mmc v4 "
			       "high-speed mode.\n",
			       ifx_mmc_hostname(card->host));
			continue;
		}
		mmc_card_set_highspeed(card);

		/* Check for host support for wide-bus modes. */
		if (!(host->caps & MMC_CAP_4_BIT_DATA)) {
/*			printk("%s[%d] Set 4 -bit mode!!!\n",__FUNCTION__,__LINE__); */
			continue;
		}
		/* Activate 4-bit support. */
		cmd.opcode = MMC_SWITCH;
		cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
			  (EXT_CSD_BUS_WIDTH << 16) |
			  (EXT_CSD_BUS_WIDTH_4 << 8) |
			  EXT_CSD_CMD_SET_NORMAL;
		cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;

		err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
		if (err != MMC_ERR_NONE) {
			printk("%s: failed to switch card to "
			       "mmc v4 4-bit bus mode.\n",
			       ifx_mmc_hostname(card->host));
			continue;
		}

		host->ios.bus_width = MMC_BUS_WIDTH_4;
	}
	kfree(ext_csd);
	ifx_mmc_deselect_cards(host);
}

static void ifx_mmc_read_scrs(struct mmc_host *host)
{
	int err;
	struct mmc_card *card;
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
	struct scatterlist sg;

	list_for_each_entry(card, &host->cards, node) {
		if (card->state & (MMC_STATE_DEAD|MMC_STATE_PRESENT)){
			continue;
		}
		if (!mmc_card_sd(card))	{
			continue;
		}

		err = ifx_mmc_select_card(host, card);
		if (err != MMC_ERR_NONE) {
/*			printk("%s[%d]: Err!!!\n",__FUNCTION__,__LINE__); */
			mmc_card_set_dead(card);
			continue;
		}
		memset(&cmd, 0, sizeof(struct mmc_command));
		cmd.opcode = MMC_APP_CMD;
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
		err = ifx_mmc_wait_for_cmd(host, &cmd, 0);
		if ((err != MMC_ERR_NONE) || !(cmd.resp[0] & R1_APP_CMD)) {
/*			printk("%s[%d]: Err!!!\n",__FUNCTION__,__LINE__); */
			mmc_card_set_dead(card);
			continue;
		}

		memset(&cmd, 0, sizeof(struct mmc_command));
		cmd.opcode = SD_APP_SEND_SCR;
		cmd.arg = 0;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		memset(&data, 0, sizeof(struct mmc_data));
		ifx_mmc_set_data_timeout(&data, card, 0);
		data.blksz = 1 << 3;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		data.sg = &sg;
		data.sg_len = 1;
		memset(&mrq, 0, sizeof(struct mmc_request));
		mrq.cmd = &cmd;
		mrq.data = &data;
		sg_init_one(&sg, (u8*)card->raw_scr, 8);
		ifx_mmc_wait_for_req(host, &mrq);
		if (cmd.error != MMC_ERR_NONE || data.error != MMC_ERR_NONE) {
			printk("%s[%d]: Err!!!\n",__FUNCTION__,__LINE__);
			mmc_card_set_dead(card);
			continue;
		}
		card->raw_scr[0] = ntohl(card->raw_scr[0]);
		card->raw_scr[1] = ntohl(card->raw_scr[1]);
		ifx_mmc_decode_scr(card);
	}
	ifx_mmc_deselect_cards(host);
}

static void ifx_mmc_read_switch_caps(struct mmc_host *host)
{
	int err;
	struct mmc_card *card;
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
	unsigned char *status;
	struct scatterlist sg;

	status = kmalloc(64, GFP_KERNEL);
	if (!status) {
		printk(KERN_WARNING "%s: Unable to allocate buffer for "
			"reading switch capabilities.\n",
			ifx_mmc_hostname(host));
		return;
	}

	list_for_each_entry(card, &host->cards, node) {
		if (card->state & (MMC_STATE_DEAD|MMC_STATE_PRESENT)) {
			continue;
		}
		if (!mmc_card_sd(card))	{
			continue;
		}
		if (card->scr.sda_vsn < SCR_SPEC_VER_1)	{
			continue;
		}

		err = ifx_mmc_select_card(host, card);
		if (err != MMC_ERR_NONE) {
		/*	printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__); */
			mmc_card_set_dead(card);
			continue;
		}
		memset(&cmd, 0, sizeof(struct mmc_command));
		cmd.opcode = SD_SWITCH;
		cmd.arg = 0x00FFFFF1;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		memset(&data, 0, sizeof(struct mmc_data));
		ifx_mmc_set_data_timeout(&data, card, 0);
		data.blksz = 64;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		data.sg = &sg;
		data.sg_len = 1;
		memset(&mrq, 0, sizeof(struct mmc_request));
		mrq.cmd = &cmd;
		mrq.data = &data;
		sg_init_one(&sg, status, 64);
		ifx_mmc_wait_for_req(host, &mrq);
		if (cmd.error != MMC_ERR_NONE || data.error != MMC_ERR_NONE) {
/*			printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__); */
			mmc_card_set_dead(card);
			continue;
		}

		if (status[13] & 0x02)
			card->sw_caps.hs_max_dtr = 50000000;
		memset(&cmd, 0, sizeof(struct mmc_command));
		cmd.opcode = SD_SWITCH;
		cmd.arg = 0x80FFFFF1;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		memset(&data, 0, sizeof(struct mmc_data));
		ifx_mmc_set_data_timeout(&data, card, 0);
		data.blksz = 64;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		data.sg = &sg;
		data.sg_len = 1;
		memset(&mrq, 0, sizeof(struct mmc_request));
		mrq.cmd = &cmd;
		mrq.data = &data;
		sg_init_one(&sg, status, 64);
		ifx_mmc_wait_for_req(host, &mrq);
		if (cmd.error != MMC_ERR_NONE || data.error != MMC_ERR_NONE) {
			printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
			mmc_card_set_dead(card);
			continue;
		}
		if ((status[16] & 0xF) != 1) {
/*			printk(KERN_WARNING "%s: Problem switching card " "into high-speed mode! status:0x%08x\n",ifx_mmc_hostname(host),status[16]); */
			continue;
		}
		mmc_card_set_highspeed(card);
	}
	kfree(status);
	ifx_mmc_deselect_cards(host);
}

static unsigned int ifx_mmc_calculate_clock(struct mmc_host *host)
{
	struct mmc_card *card;
	unsigned int max_dtr = host->f_max;

	list_for_each_entry(card, &host->cards, node) {
		if (!mmc_card_dead(card)) {
			if (mmc_card_highspeed(card) && mmc_card_sd(card)) {
				if (max_dtr > card->sw_caps.hs_max_dtr)
					max_dtr = card->sw_caps.hs_max_dtr;
			} else if (mmc_card_highspeed(card) && !mmc_card_sd(card)) {
				if (max_dtr > card->ext_csd.hs_max_dtr)
					max_dtr = card->ext_csd.hs_max_dtr;
			} else if (max_dtr > card->csd.max_dtr) {
				max_dtr = card->csd.max_dtr;
			}
		}
	}
	pr_debug("%s: selected %d.%03dMHz transfer rate\n",
		 ifx_mmc_hostname(host),
		 max_dtr / 1000000, (max_dtr / 1000) % 1000);

	return max_dtr;
}

/*
 * Check whether cards we already know about are still present.
 * We do this by requesting status, and checking whether a card
 * responds.
 *
 * A request for status does not cause a state change in data
 * transfer mode.
 */
static void ifx_mmc_check_cards(struct mmc_host *host)
{
	struct list_head *l, *n;
	ifx_mmc_deselect_cards(host);
	list_for_each_safe(l, n, &host->cards) {
		struct mmc_card *card = mmc_list_to_card(l);
		struct mmc_command cmd;
		int err;
		
		cmd.opcode = MMC_SEND_STATUS;
		cmd.arg = card->rca << 16;
		cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
		err = ifx_mmc_wait_for_cmd(host, &cmd, CMD_RETRIES);
		if (err == MMC_ERR_NONE)
			continue;
		mmc_card_set_dead(card);
	}
}

static void ifx_mmc_setup(struct mmc_host *host)
{
	if (host->ios.power_mode != MMC_POWER_ON) {
		int err;
		u32 ocr;

		host->mode = MMC_MODE_SD;
		ifx_mmc_power_up(host);
		ifx_mmc_idle_cards(host);
		err = ifx_mmc_send_app_op_cond(host, 0, &ocr);
		/* If we fail to detect any SD cards then try
		 * searching for MMC cards.	 */
		if (err != MMC_ERR_NONE) {
			host->mode = MMC_MODE_MMC;
			err = ifx_mmc_send_op_cond(host, 0, &ocr);
			if (err != MMC_ERR_NONE) {
				return ;
			}
		}
		host->ocr = ifx_mmc_select_voltage(host, ocr);
		/*
		 * Since we're changing the OCR value, we seem to
		 * need to tell some cards to go back to the idle
		 * state.  We wait 1ms to give cards time to
		 * respond.
		 */
		if (host->ocr)
			ifx_mmc_idle_cards(host);
	} else {
		host->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
		host->ios.clock = host->f_min;
		ifx_mmc_set_ios(host);

		/*
		 * We should remember the OCR mask from the existing
		 * cards, and detect the new cards OCR mask, combine
		 * the two and re-select the VDD.  However, if we do
		 * change VDD, we should do an idle, and then do a
		 * full re-initialisation.  We would need to notify
		 * drivers so that they can re-setup the cards as
		 * well, while keeping their queues at bay.
		 *
		 * For the moment, we take the easy way out - if the
		 * new cards don't like our currently selected VDD,
		 * they drop off the bus.
		 */
	}

	if (host->ocr == 0)	{
		return;
	}
	/*
	 * Send the selected OCR multiple times... until the cards
	 * all get the idea that they should be ready for CMD2.
	 * (My SanDisk card seems to need this.)
	 */
	if (host->mode == MMC_MODE_SD) {
		ifx_mmc_send_app_op_cond(host, host->ocr, NULL);
	} else {
		ifx_mmc_send_op_cond(host, host->ocr, NULL);
	}
	ifx_mmc_discover_cards(host);
	/* Ok, now switch to push-pull mode. */
	ifx_mmc_read_csds(host);
	if (host->mode == MMC_MODE_SD) {
		ifx_mmc_read_scrs(host);
		ifx_mmc_read_switch_caps(host);
	} else
		ifx_mmc_process_ext_csds(host);
}

/*!
  \fn void ifx_mmc_detect_change(struct mmc_host *host, unsigned long delay)
  \ingroup  IFX_MMC_CORE_API 
  \brief process change of state on a MMC socket
  
  All we know is that card(s) have been inserted or removed
  from the socket(s).  We don't know which socket or cards.
  
  \param    host    --> pointer to MMC Host structure
  \param    delay   --> optional delay to wait before detection (jiffies)
  \return   None	
 */
void ifx_mmc_detect_change(struct mmc_host *host, unsigned long delay)
{
	ifx_mmc_schedule_delayed_work(&host->detect, delay);
}
EXPORT_SYMBOL(ifx_mmc_detect_change);

static void ifx_mmc_rescan(struct work_struct *work)
{
	struct mmc_host *host =
		container_of(work, struct mmc_host, detect.work);
	struct list_head *l, *n;
	unsigned char power_mode;
	/** printk("%s[%d] Start............\n",__FUNCTION__,__LINE__); */
	ifx_mmc_claim_host(host);
	/*
	 * Check for removed cards and newly inserted ones. We check for
	 * removed cards first so we can intelligently re-select the VDD.
	 */
	power_mode = host->ios.power_mode;
	if (power_mode == MMC_POWER_ON)
		ifx_mmc_check_cards(host);
	ifx_mmc_setup(host);
	/*
	 * Some broken cards process CMD1 even in stand-by state. There is
	 * no reply, but an ILLEGAL_COMMAND error is cached and returned
	 * after next command. We poll for card status here to clear any
	 * possibly pending error.
	 */
	if (power_mode == MMC_POWER_ON)
		ifx_mmc_check_cards(host);

	if (!list_empty(&host->cards)) {
		/*
		 * (Re-)calculate the fastest clock rate which the
		 * attached cards and the host support.
		 */
		host->ios.clock = ifx_mmc_calculate_clock(host);
		ifx_mmc_set_ios(host);
	}

	ifx_mmc_release_host(host);

	list_for_each_safe(l, n, &host->cards) {
		struct mmc_card *card = mmc_list_to_card(l);

		/* If this is a new and good card, register it. */
		if (!mmc_card_present(card) && !mmc_card_dead(card)) {
			if (ifx_mmc_register_card(card)) {
	/*			printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__); */
				mmc_card_set_dead(card);
			} else {
/*				printk("%s[%d] card present\n",__FUNCTION__,__LINE__);  */
				mmc_card_set_present(card);
			}
		}

		/* If this card is dead, destroy it.*/
		if (mmc_card_dead(card)) {
			list_del(&card->node);
			ifx_mmc_remove_card(card);
		}
	}

	/*
	 * If we discover that there are no cards on the
	 * bus, turn off the clock and power down.
	 */
	if (list_empty(&host->cards)) {
		ifx_mmc_power_off(host);
	}
}

/*!
  \fn struct mmc_host *ifx_mmc_alloc_host(struct mmc_host *host, struct device *dev)
  \ingroup  IFX_MMC_CORE_API 
  \brief Alloacte the buffer per host
  
  Initialise the per-host structure 
  
  \param    host    --> pointer to MMC Host structure
  \param    dev     --> pointer to standard device structure
  \return   error code 
 */
struct mmc_host *ifx_mmc_alloc_host(struct mmc_host *host, struct device *dev)
{

	host = ifx_mmc_alloc_host_sysfs(host, dev);
	if (host) {
		spin_lock_init(&host->lock);
		init_waitqueue_head(&host->wq);
		INIT_LIST_HEAD(&host->cards);
		INIT_DELAYED_WORK(&host->detect, ifx_mmc_rescan);

		/*
		 * By default, hosts do not support SGIO or large requests.
		 * They have to set these according to their abilities.
		 */
		host->max_hw_segs = 1;
		host->max_phys_segs = 1;
		host->max_sectors = 1 << (PAGE_CACHE_SHIFT - 9);
		host->max_seg_size = PAGE_CACHE_SIZE;
	}
	return host;
}
EXPORT_SYMBOL(ifx_mmc_alloc_host);

/*!
  \fn int ifx_mmc_add_host(struct mmc_host *host)
  \ingroup  IFX_MMC_CORE_API 
  \brief Add the host system structure
  
  Register and add the card associated with this host.
  
  \param    host    --> pointer to MMC Host structure
  \return   error code 
 */
int ifx_mmc_add_host(struct mmc_host *host)
{
	int ret;

	ret = ifx_mmc_add_host_sysfs(host);
	if (ret == 0) {
		ifx_mmc_power_off(host);
		ifx_mmc_detect_change(host, 0);
	}

	return ret;
}
EXPORT_SYMBOL(ifx_mmc_add_host);

/*!
  \fn void ifx_mmc_remove_host(struct mmc_host *host)
  \ingroup  IFX_MMC_CORE_API 
  \brief Remove the host system structure
  
  Unregister and remove all cards associated with this host.
  
  \param    host    --> pointer to MMC Host structure
  \return   None
 */
void ifx_mmc_remove_host(struct mmc_host *host)
{
	struct list_head *l, *n;

	list_for_each_safe(l, n, &host->cards) {
		struct mmc_card *card = mmc_list_to_card(l);

		ifx_mmc_remove_card(card);
	}

	ifx_mmc_power_off(host);
	ifx_mmc_remove_host_sysfs(host);
}
EXPORT_SYMBOL(ifx_mmc_remove_host);

/*!
  \fn void ifx_mmc_free_host(struct mmc_host *host)
  \ingroup  IFX_MMC_CORE_API 
  \brief Free the host 
  
  Free the host once all references to it have been dropped
  
  \param    host    --> pointer to MMC Host structure
  \return   None
 */
void ifx_mmc_free_host(struct mmc_host *host)
{
	ifx_mmc_flush_scheduled_work();
	ifx_mmc_free_host_sysfs(host);
}

EXPORT_SYMBOL(ifx_mmc_free_host);

#ifdef CONFIG_PM

/*!
  \fn int ifx_mmc_suspend_host(struct mmc_host *host, pm_message_t state)
  \ingroup  IFX_MMC_CORE_API 
  \brief Suspend the host
  
  \param    host    --> pointer to MMC Host structure
  \return   0
 */
int ifx_mmc_suspend_host(struct mmc_host *host, pm_message_t state)
{
	ifx_mmc_claim_host(host);
	ifx_mmc_deselect_cards(host);
	ifx_mmc_power_off(host);
	ifx_mmc_release_host(host);

	return 0;
}
EXPORT_SYMBOL(ifx_mmc_suspend_host);

/*!
  \fn int ifx_mmc_resume_host(struct mmc_host *host)
  \ingroup  IFX_MMC_CORE_API 
  \brief Resume the host
  
  \param    host    --> pointer to MMC Host structure
  \return   0
 */
int ifx_mmc_resume_host(struct mmc_host *host)
{
	ifx_mmc_rescan(&host->detect.work);

	return 0;
}
EXPORT_SYMBOL(ifx_mmc_resume_host);

#endif

static struct device_attribute ifx_mmc_dev_attrs[] = {
    MMC_ATTR_RO(cid),
    MMC_ATTR_RO(csd),
    MMC_ATTR_RO(date),
    MMC_ATTR_RO(fwrev),
    MMC_ATTR_RO(hwrev),
    MMC_ATTR_RO(manfid),
    MMC_ATTR_RO(name),
    MMC_ATTR_RO(oemid),
    MMC_ATTR_RO(serial),
    __ATTR_NULL
};

static struct device_attribute ifx_mmc_dev_attr_scr = MMC_ATTR_RO(scr);

static void ifx_mmc_release_card(struct device *dev)
{
    struct mmc_card *card = dev_to_mmc_card(dev);
    if(card)
        kfree(card);
}

/*
 * This currently matches any MMC driver to any MMC card - drivers
 * themselves make the decision whether to drive this card in their
 * probe method.  However, we force "bad" cards to fail.
 */
static int ifx_mmc_bus_match(struct device *dev, struct device_driver *drv)
{
    struct mmc_card *card = dev_to_mmc_card(dev);
    return !mmc_card_bad(card);
}
static int
ifx_mmc_bus_uevent(struct device *dev, char **envp, int num_envp, char *buf,
                int buf_size)
{
    struct mmc_card *card = dev_to_mmc_card(dev);
    char ccc[13];
    int i = 0;
    
    

#define add_env(fmt,val)                                \
    ({                                                  \
        int len, ret = -ENOMEM;                         \
        if (i < num_envp) {                             \
            envp[i++] = buf;                            \
            len = snprintf(buf, buf_size, fmt, val) + 1;\
            buf_size -= len;                            \
            buf += len;                                 \
            if (buf_size >= 0)                          \
                ret = 0;                                \
        }                                               \
        ret;                                            \
    })
    for (i = 0; i < 12; i++)
        ccc[i] = card->csd.cmdclass & (1 << i) ? '1' : '0';
    ccc[12] = '\0';
    
    i = 0;
    add_env("MMC_CCC=%s", ccc);
    add_env("MMC_MANFID=%06x", card->cid.manfid);
    add_env("MMC_NAME=%s", mmc_card_name(card));
    add_env("MMC_OEMID=%04x", card->cid.oemid);
    return 0;
}

static int ifx_mmc_bus_suspend(struct device *dev, pm_message_t state)
{
    struct mmc_driver *drv = to_mmc_driver(dev->driver);
    struct mmc_card *card = dev_to_mmc_card(dev);
    int ret = 0;
    
    if (dev->driver && drv->suspend)
        ret = drv->suspend(card, state);
    return ret;
}

static int ifx_mmc_bus_resume(struct device *dev)
{
    struct mmc_driver *drv = to_mmc_driver(dev->driver);
    struct mmc_card *card = dev_to_mmc_card(dev);
    int ret = 0;
    
    if (dev->driver && drv->resume)
        ret = drv->resume(card);
    return ret;
}

static int ifx_mmc_bus_probe(struct device *dev)
{
    struct mmc_driver *drv = to_mmc_driver(dev->driver);
    struct mmc_card *card = dev_to_mmc_card(dev);
    return drv->probe(card);
}

static int ifx_mmc_bus_remove(struct device *dev)
{
    struct mmc_driver *drv = to_mmc_driver(dev->driver);
    struct mmc_card *card = dev_to_mmc_card(dev);
    
    drv->remove(card);
    return 0;
}

static struct bus_type mmc_bus_type = {
    .name           = "ifx_mmc",
    .dev_attrs      = ifx_mmc_dev_attrs,
    .match          = ifx_mmc_bus_match,
    .uevent         = ifx_mmc_bus_uevent,
    .probe          = ifx_mmc_bus_probe,
    .remove         = ifx_mmc_bus_remove,
    .suspend        = ifx_mmc_bus_suspend,
    .resume         = ifx_mmc_bus_resume,
};

/*!
  \fn int ifx_mmc_register_driver(struct mmc_driver *drv)
  \ingroup  IFX_MMC_CORE_API 
  \brief Register a  card driver
  
  \param    host    --> pointer to MMC driver structure
  \return   error code
 */
int ifx_mmc_register_driver(struct mmc_driver *drv)
{
    drv->drv.bus = &mmc_bus_type;
    return driver_register(&drv->drv);
}
EXPORT_SYMBOL(ifx_mmc_register_driver);

/*!
  \fn int ifx_mmc_register_driver(struct mmc_driver *drv)
  \ingroup  IFX_MMC_CORE_API 
  \brief Unregister a card driver
  
  \param    host    --> pointer to MMC driver structure
  \return   None
 */
void ifx_mmc_unregister_driver(struct mmc_driver *drv)
{
    drv->drv.bus = &mmc_bus_type;
    driver_unregister(&drv->drv);
}
EXPORT_SYMBOL(ifx_mmc_unregister_driver);

/*
 * Internal function.  Initialise a MMC card structure.
 */
void ifx_mmc_init_card(struct mmc_card *card, struct mmc_host *host)
{
    memset(card, 0, sizeof(struct mmc_card));
    card->host = host;
    device_initialize(&card->dev);
    card->dev.parent = ifx_mmc_dev(host);
    card->dev.bus = &mmc_bus_type;
    card->dev.release = ifx_mmc_release_card;
}

/*
 * Internal function.  Register a new MMC card with the driver model.
 */
int ifx_mmc_register_card(struct mmc_card *card)
{
    int ret;
    
    snprintf(card->dev.bus_id, sizeof(card->dev.bus_id),
        "%s:%04x", ifx_mmc_hostname(card->host), card->rca);
    ret = device_add(&card->dev);
    if (ret == 0) {
        if (mmc_card_sd(card)) {
            ret = device_create_file(&card->dev, &ifx_mmc_dev_attr_scr);
            if (ret) {
                printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
                device_del(&card->dev);
            }
        }
    }
    return ret;
}

/*
 * Internal function.  Unregister a new MMC card with the
 * driver model, and (eventually) free it.
 */
void ifx_mmc_remove_card(struct mmc_card *card)
{
    if (mmc_card_present(card)) {
        if (mmc_card_sd(card))
            device_remove_file(&card->dev, &ifx_mmc_dev_attr_scr);
            device_del(&card->dev);
    }
    put_device(&card->dev);
}


static void ifx_mmc_host_classdev_release(struct device *dev)
{
    struct mmc_host *host = cls_dev_to_mmc_host(dev);
    kfree(host);
}

static struct class ifx_mmc_host_class = {
    .name           = "ifx_mmc_host",
    .dev_release    = ifx_mmc_host_classdev_release,
};

static DEFINE_IDR(ifx_mmc_host_idr);
static DEFINE_SPINLOCK(ifx_mmc_host_lock);
/*
 * Internal function. Allocate a new MMC host.
 */
struct mmc_host *ifx_mmc_alloc_host_sysfs(struct mmc_host *host, struct device *dev)
{
    if (host) {
        host->parent = dev;
        host->class_dev.parent = dev;
        host->class_dev.class = &ifx_mmc_host_class;
        device_initialize(&host->class_dev);
    }
    return host;
}
/*
 * Internal function. Register a new MMC host with the MMC class.
 */
int ifx_mmc_add_host_sysfs(struct mmc_host *host)
{
    int err;
    
    if (!idr_pre_get(&ifx_mmc_host_idr, GFP_KERNEL)) {
        return -ENOMEM;
    }
    
    spin_lock(&ifx_mmc_host_lock);
    err = idr_get_new(&ifx_mmc_host_idr, host, &host->index);
    spin_unlock(&ifx_mmc_host_lock);
    if (err) {
        return err;
    }
    
    snprintf(host->class_dev.bus_id, BUS_ID_SIZE, "ifx_mmc%d", host->index);
    return device_add(&host->class_dev);
}

/*
 * Internal function. Unregister a MMC host with the MMC class.
 */
void ifx_mmc_remove_host_sysfs(struct mmc_host *host)
{
    device_del(&host->class_dev);
    spin_lock(&ifx_mmc_host_lock);
    idr_remove(&ifx_mmc_host_idr, host->index);
    spin_unlock(&ifx_mmc_host_lock);
}

/*
 * Internal function. Free a MMC host.
 */
void ifx_mmc_free_host_sysfs(struct mmc_host *host)
{
    put_device(&host->class_dev);
}

static struct workqueue_struct *workqueue;

/*
 * Internal function. Schedule delayed work in the MMC work queue.
 */
int ifx_mmc_schedule_delayed_work(struct delayed_work *work, unsigned long delay)
{
    return queue_delayed_work(workqueue, work, delay);
}

/*
 * Internal function. Flush all scheduled work from the MMC work queue.
 */
void ifx_mmc_flush_scheduled_work(void)
{
    flush_workqueue(workqueue);
}
/** Driver version info */
static inline int mmc_core_drv_ver(char *buf)
{
    return sprintf(buf, "IFX MMC Core driver, version %s,(c)2009 Infineon Technologies AG\n", version);
}

/** Displays the version of DMA module via proc file */
static int mmc_core_proc_version(char *buf, char **start, off_t offset,
                         int count, int *eof, void *data)
{
    int len = 0;
    /* No sanity check cos length is smaller than one page */
    len += mmc_core_drv_ver(buf + len);
    *eof = 1;
    return len;    
}

/** create proc for debug  info, \used ifx_mmc_core_modeul_init */
static int create_proc_mmc_core(void)
{
    /* procfs */
    ifx_mmc_core_dir = proc_mkdir ("driver/ifx_mmc_core", NULL);
    if (ifx_mmc_core_dir == NULL) {
        printk(KERN_ERR "%s: Create proc directory (/driver/ifx_mmc_core) failed!!!\n", __func__);
        return IFX_ERROR;
    }
    create_proc_read_entry("version", 0, ifx_mmc_core_dir, \
                    mmc_core_proc_version,  NULL);
    return IFX_SUCCESS;
}

/** remove of the proc entries, \used ifx_mmc_core_modeul_exit */
static void delete_proc_mmc_core(void)
{ 
    remove_proc_entry("version", ifx_mmc_core_dir);
    remove_proc_entry("driver/ifx_mmc_core",  NULL);
}

/** Init of the MMC core module*/
static int __init ifx_mmc_core_modeul_init(void)
{
    int ret;
    char ver_str[128] = {0};
    
	workqueue = create_singlethread_workqueue("kifx_mmcd");
	if (!workqueue)
	    return -ENOMEM;
	ret = bus_register(&mmc_bus_type);
	if (ret == 0) {
	    ret = class_register(&ifx_mmc_host_class);
	    if (ret)
	        bus_unregister(&mmc_bus_type);
	}
	create_proc_mmc_core();
	/* Print the driver version info */                 
    mmc_core_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
	return ret;
}

/** Clean up MMC core moule */
static void __exit ifx_mmc_core_modeul_exit(void)
{
    class_unregister(&ifx_mmc_host_class);
    bus_unregister(&mmc_bus_type);
    destroy_workqueue(workqueue);
     /** remove of the proc entries */
    delete_proc_mmc_core();
}

module_init(ifx_mmc_core_modeul_init);
module_exit(ifx_mmc_core_modeul_exit);
MODULE_DESCRIPTION("IFX MMC core driver");
