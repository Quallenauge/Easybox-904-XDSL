/**
** FILE NAME    : ifxmips_mmc_block.c
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
  \file ifxmips_mmc_block.c
  \ingroup IFX_MMC_DRV
  \brief MMC block driver for SD Card
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/kdev_t.h>
#include <linux/blkdev.h>
#include <linux/mutex.h>
#include <linux/scatterlist.h>
#include <linux/kthread.h>
#include <linux/mmc/protocol.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

#include <asm/ifx/ifx_regs.h>
#include <asm/ifx/common_routines.h>
#include "ifxmips_mmc_core.h"

#define IFX_MMC_DRV_MODULE_NAME         "ifxmips_sd_card"
#define IFX_DRV_MODULE_VERSION          "1.0.2"
static char version[] __devinitdata =
        IFX_MMC_DRV_MODULE_NAME ".c:v" IFX_DRV_MODULE_VERSION " \n";
/*proc file directory*/
static struct proc_dir_entry*   ifx_mmc_block_dir;
       
#define MMC_SHIFT                       3
#define MMC_QUEUE_SUSPENDED             (1 << 0)

static int major;

#define MMC_NUM_MINORS	                (256 >> MMC_SHIFT)

static unsigned long dev_use[MMC_NUM_MINORS/(8*sizeof(unsigned long))];

static inline int mmc_blk_readonly(struct mmc_card *card) {
	return mmc_card_readonly(card) || 
	       !(card->csd.cmdclass & CCC_BLOCK_WRITE);
}

static DEFINE_MUTEX(open_lock);

/*
 * Prepare a MMC request.  Essentially, this means passing the
 * preparation off to the media driver.  The media driver will
 * create a mmc_io_request in req->special.
 */
static int ifx_mmc_prep_request(struct request_queue *q, struct request *req)
{
    struct ifx_mmc_queue *mq = q->queuedata;
    int ret = BLKPREP_KILL;
    
    if (blk_special_request(req)) {
        /*
            * Special commands already have the command
            * blocks already setup in req->special.
        */
        BUG_ON(!req->special);
        ret = BLKPREP_OK;
    } else if (blk_fs_request(req) || blk_pc_request(req)) {
        /*
            * Block I/O requests need translating according
            * to the protocol.
        */
        ret = mq->prep_fn(mq, req);
    } else {
        /*
            * Everything else is invalid.
        */
        blk_dump_rq_flags(req, "MMC bad request");
    }
    if (ret == BLKPREP_OK) {
        req->cmd_flags |= REQ_DONTPREP;
    }
    
    return ret;
}

static int ifx_mmc_queue_thread(void *d)
{
    struct ifx_mmc_queue *mq = d;
    struct request_queue *q = mq->queue;
    
    /*
        * Set iothread to ensure that we aren't put to sleep by
        * the process freezing.  We handle suspension ourselves.
    */
    current->flags |= PF_MEMALLOC|PF_NOFREEZE;
    
    mutex_lock(&mq->mutex);
    do {
        struct request *req = NULL;
        
        spin_lock_irq(q->queue_lock);
        set_current_state(TASK_INTERRUPTIBLE);
        if (!blk_queue_plugged(q))
            req = elv_next_request(q);
        mq->req = req;
        spin_unlock_irq(q->queue_lock);
        
        if (!req) {
            if (kthread_should_stop()) {
                set_current_state(TASK_RUNNING);
                break;
            }
            mutex_unlock(&mq->mutex);
            schedule();
            mutex_lock(&mq->mutex);
            continue;
        }
        set_current_state(TASK_RUNNING);
        
        mq->issue_fn(mq, req);
    } while (1);
    mutex_unlock(&mq->mutex);
    
    return 0;
}

/*
 * Generic MMC request handler.  This is called for any queue on a
 * particular host.  When the host is not busy, we look for a request
 * on any queue on this host, and attempt to issue it.  This may
 * not be the queue we were asked to process.
 */
static void mmc_request(request_queue_t *q)
{
    struct ifx_mmc_queue *mq = q->queuedata;
    struct request *req;
    int ret;
    
    if (!mq) {
        printk(KERN_ERR "MMC: killing requests for dead queue\n");
        while ((req = elv_next_request(q)) != NULL) {
            do {
                printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
                ret = end_that_request_chunk(req, 0, req->current_nr_sectors << 9);
            } while (ret);
        }
        return;
    }
    
    if (!mq->req) {
        wake_up_process(mq->thread);
    }
}

/**
 * ifx_mmc_init_queue - initialise a queue structure.
 * @mq: mmc queue
 * @card: mmc card to attach this queue
 * @lock: queue lock
 *
 * Initialise a MMC card request queue.
 */
int ifx_mmc_init_queue(struct ifx_mmc_queue *mq, struct mmc_card *card, spinlock_t *lock)
{
    struct mmc_host *host = card->host;
    int ret;
    
/*  u64 limit = BLK_BOUNCE_HIGH; */
    mq->card = card;
    mq->queue = blk_init_queue(mmc_request, lock);
    if (!mq->queue) {
        printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
        return -ENOMEM;
    }
    
    blk_queue_prep_rq(mq->queue, ifx_mmc_prep_request);
/*  blk_queue_bounce_limit(mq->queue, limit); */
    blk_queue_max_sectors(mq->queue, host->max_sectors);
    blk_queue_max_phys_segments(mq->queue, host->max_phys_segs);
    blk_queue_max_hw_segments(mq->queue, host->max_hw_segs);
    blk_queue_max_segment_size(mq->queue, host->max_seg_size);
    
    mq->queue->queuedata = mq;
    mq->req = NULL;
    
    mq->sg = kmalloc(sizeof(struct scatterlist) * host->max_phys_segs, GFP_KERNEL);
    if (!mq->sg) {
        printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
        ret = -ENOMEM;
        goto cleanup_queue;
    }
    
    mutex_init(&mq->mutex);
    mq->thread = kthread_run(ifx_mmc_queue_thread, mq, "ifxmmcqd");
    if (IS_ERR(mq->thread)) {
        ret = PTR_ERR(mq->thread);
        goto free_sg;
    }
    
    return 0;
    
 free_sg:
    kfree(mq->sg);
    mq->sg = NULL;
 cleanup_queue:
    blk_cleanup_queue(mq->queue);
    return ret;
}

void ifx_sd_card_cleanup_queue(struct ifx_mmc_queue *mq)
{
    request_queue_t *q = mq->queue;
    unsigned long flags;
    
    /* Mark that we should start throwing out stragglers */
    spin_lock_irqsave(q->queue_lock, flags);
    q->queuedata = NULL;
    spin_unlock_irqrestore(q->queue_lock, flags);
    
    /* Then terminate our worker thread */
    kthread_stop(mq->thread);
    kfree(mq->sg);
    mq->sg = NULL;
    blk_cleanup_queue(mq->queue);
    mq->card = NULL;
}

/**
 * ifx_mmc_queue_suspend - suspend a MMC request queue
 * @mq: MMC queue to suspend
 *
 * Stop the block request queue, and wait for our thread to
 * complete any outstanding requests.  This ensures that we
 * won't suspend while a request is being processed.
 */
void ifx_mmc_queue_suspend(struct ifx_mmc_queue *mq)
{
    request_queue_t *q = mq->queue;
    unsigned long flags;
    
    if (!(mq->flags & MMC_QUEUE_SUSPENDED)) {
        mq->flags |= MMC_QUEUE_SUSPENDED;
        spin_lock_irqsave(q->queue_lock, flags);
        blk_stop_queue(q);
        spin_unlock_irqrestore(q->queue_lock, flags);
        mutex_lock(&mq->mutex);
    }
}

/**
 * ifx_mmc_queue_resume - resume a previously suspended MMC request queue
 * @mq: MMC queue to resume
 */
void ifx_mmc_queue_resume(struct ifx_mmc_queue *mq)
{
    request_queue_t *q = mq->queue;
    unsigned long flags;
    
    if (mq->flags & MMC_QUEUE_SUSPENDED) {
        mq->flags &= ~MMC_QUEUE_SUSPENDED;
        mutex_unlock(&mq->mutex);
        spin_lock_irqsave(q->queue_lock, flags);
        blk_start_queue(q);
        spin_unlock_irqrestore(q->queue_lock, flags);
    }
}

static struct mmc_blk_data *mmc_blk_get(struct gendisk *disk)
{
    struct mmc_blk_data *md;
    
    mutex_lock(&open_lock);
    md = disk->private_data;
    if (md && md->usage == 0) {
        md = NULL;
    }
    if (md)
        md->usage++;
    mutex_unlock(&open_lock);
    
    return md;
}

static void mmc_blk_put(struct mmc_blk_data *md)
{
    mutex_lock(&open_lock);
    md->usage--;
    if (md->usage == 0) {
        put_disk(md->disk);
        kfree(md);
    }
    mutex_unlock(&open_lock);
}

static int mmc_blk_open(struct inode *inode, struct file *filp)
{
    struct mmc_blk_data *md;
    int ret = -ENXIO;
    
    md = mmc_blk_get(inode->i_bdev->bd_disk);
    if (md) {
        if (md->usage == 2){
            check_disk_change(inode->i_bdev);
        }
        ret = 0;
        
        if ((filp->f_mode & FMODE_WRITE) && md->read_only) {
            printk("%s[%d] Read only disk!!!\n",__FUNCTION__,__LINE__);
            ret = -EROFS;
        }
    }
    return ret;
}

static int mmc_blk_release(struct inode *inode, struct file *filp)
{
    struct mmc_blk_data *md = inode->i_bdev->bd_disk->private_data;
    
    mmc_blk_put(md);
    return 0;
}

static int
mmc_blk_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
    geo->cylinders = get_capacity(bdev->bd_disk) / (4 * 16);
    geo->heads = 4;
    geo->sectors = 16;
    return 0;
}


static int ifx_mmc_blk_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long data)
{
    /*TO DO*/
    return 0;
}

static  int ifx_mmc_blk_media_changed (struct gendisk *disk)
{    /*TO DO*/
	return 0;
}

static int ifx_mmc_blk_revalidate_disk (struct gendisk *disk)
{    /*TO DO*/
	return 0;
}

static struct block_device_operations mmc_bdops = {
    .open			= mmc_blk_open,
    .release		= mmc_blk_release,
    .getgeo			= mmc_blk_getgeo,
    .ioctl			= ifx_mmc_blk_ioctl,
    .media_changed		= ifx_mmc_blk_media_changed,
    .revalidate_disk	= ifx_mmc_blk_revalidate_disk,
    .owner			= THIS_MODULE,
};

struct mmc_blk_request {
    struct mmc_request	mrq;
    struct mmc_command	cmd;
    struct mmc_command	stop;
    struct mmc_data		data;
};

/* prepare the block device to request data write/read*/
static int mmc_blk_prep_rq(struct ifx_mmc_queue *mq, struct request *req)
{
    struct mmc_blk_data *md = mq->data;
    int stat = BLKPREP_OK;
	/*
	 * If we have no device, we haven't finished initialising.
	 */
    if (!md || !mq->card) {
        printk(KERN_ERR "%s: killing request - no device/host\n",
		       req->rq_disk->disk_name);
		stat = BLKPREP_KILL;
	}
	
	return stat;
}

/* set the device number of write blocks */
static u32 mmc_sd_num_wr_blocks(struct mmc_card *card)
{
    int err;
    u32 blocks;
    
    struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
	unsigned int timeout_us;
	struct scatterlist sg;
	
    memset(&cmd, 0, sizeof(struct mmc_command));
    
    cmd.opcode = MMC_APP_CMD;
	cmd.arg = card->rca << 16;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;

	err = ifx_mmc_wait_for_cmd(card->host, &cmd, 0);
	if ((err != MMC_ERR_NONE) || !(cmd.resp[0] & R1_APP_CMD)) {	
		printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
		return (u32)-1;
	}
	memset(&cmd, 0, sizeof(struct mmc_command));

	cmd.opcode = SD_APP_SEND_NUM_WR_BLKS;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;

	memset(&data, 0, sizeof(struct mmc_data));

	data.timeout_ns = card->csd.tacc_ns * 100;
	data.timeout_clks = card->csd.tacc_clks * 100;

	timeout_us = data.timeout_ns / 1000;
	timeout_us += data.timeout_clks * 1000 /
		(card->host->ios.clock / 1000);

	if (timeout_us > 100000) {
		data.timeout_ns = 100000000;
		data.timeout_clks = 0;
	}

	data.blksz = 4;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;

	memset(&mrq, 0, sizeof(struct mmc_request));

	mrq.cmd = &cmd;
	mrq.data = &data;

	sg_init_one(&sg, &blocks, 4);

	ifx_mmc_wait_for_req(card->host, &mrq);

	if (cmd.error != MMC_ERR_NONE || data.error != MMC_ERR_NONE) {
		printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
		return (u32)-1;
	}

	blocks = ntohl(blocks);

	return blocks;
}

/* isuuse a request from the block device  to read/write data from the device*/
static int mmc_blk_issue_rq(struct ifx_mmc_queue *mq, struct request *req)
{
	struct mmc_blk_data *md = mq->data;
	struct mmc_card *card = md->queue.card;
	struct mmc_blk_request brq;
	int ret = 1;

	if (ifx_mmc_card_claim_host(card)) {
		printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
		goto flush_queue;
	}

	do {
		struct mmc_command cmd;
		u32 readcmd, writecmd;
		int cnt=0;
ReTransmit:
		memset(&brq, 0, sizeof(struct mmc_blk_request));
		brq.mrq.cmd = &brq.cmd;
		brq.mrq.data = &brq.data;

		brq.cmd.arg = req->sector << 9;
		brq.cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
		brq.data.blksz = 1 << md->block_bits;
		brq.data.blocks = req->nr_sectors >> (md->block_bits - 9);
		brq.stop.opcode = MMC_STOP_TRANSMISSION;
		brq.stop.arg = 0;
		brq.stop.flags = MMC_RSP_R1B | MMC_CMD_AC;

		ifx_mmc_set_data_timeout(&brq.data, card, rq_data_dir(req) != READ);

		/*
		 * If the host doesn't support multiple block writes, force
		 * block writes to single block. SD cards are excepted from
		 * this rule as they support querying the number of
		 * successfully written sectors.
		 */
		 brq.data.blocks = 1;
		 
		if (brq.data.blocks > 1) {
			brq.data.flags |= MMC_DATA_MULTI;
			brq.mrq.stop = &brq.stop;
			readcmd = MMC_READ_MULTIPLE_BLOCK;
			writecmd = MMC_WRITE_MULTIPLE_BLOCK;
		} else 	{
			brq.mrq.stop = NULL;
			readcmd = MMC_READ_SINGLE_BLOCK;
			writecmd = MMC_WRITE_BLOCK;
		}

		if (rq_data_dir(req) == READ) {
			brq.cmd.opcode = readcmd;
			brq.data.flags |= MMC_DATA_READ;
		} else {
			brq.cmd.opcode = writecmd;
			brq.data.flags |= MMC_DATA_WRITE;
		}

		brq.data.sg = mq->sg;
		brq.data.sg_len = blk_rq_map_sg(req->q, req, brq.data.sg);

		ifx_mmc_wait_for_req(card->host, &brq.mrq);
		if (brq.cmd.error) {
			printk(KERN_ERR "%s: error %d sending read/write command\n",
			       req->rq_disk->disk_name, brq.cmd.error);
			goto cmd_err;
		}

		if (brq.data.error) {
			printk(KERN_ERR "%s: error %d transferring data\n",
			       req->rq_disk->disk_name, brq.data.error);
 			if (brq.data.error == MMC_ERR_FIFO) {
				cnt++;
				if (cnt > 5 )
				    goto cmd_err;
				else
				    goto ReTransmit;
			}
		}

		if (brq.stop.error) {
			printk(KERN_ERR "%s: error %d sending stop command\n",
			       req->rq_disk->disk_name, brq.stop.error);
			goto cmd_err;
		}

		if (rq_data_dir(req) != READ) {
			do {
			    int err;
			    
				cmd.opcode = MMC_SEND_STATUS;
				cmd.arg = card->rca << 16;
				cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
				err = ifx_mmc_wait_for_cmd(card->host, &cmd, 5);
				if (err) {
					printk(KERN_ERR "%s: error %d requesting status\n",
					       req->rq_disk->disk_name, err);
					goto cmd_err;
				}
			} while (!(cmd.resp[0] & R1_READY_FOR_DATA));

/*
			if (cmd.resp[0] & ~0x00000900)
				printk(KERN_ERR "%s: status = %08x\n",
				       req->rq_disk->disk_name, cmd.resp[0]);
			if (mmc_decode_status(cmd.resp))
				goto cmd_err;
*/
		}

		/*
		 * A block was successfully transferred.
		 */
		spin_lock_irq(&md->lock);
		ret = end_that_request_chunk(req, 1, brq.data.bytes_xfered);
		if (!ret) {
			/*
			 * The whole request completed successfully.
			 */
			add_disk_randomness(req->rq_disk);
			blkdev_dequeue_request(req);
			end_that_request_last(req, 1);
		}
		spin_unlock_irq(&md->lock);
	} while (ret);

	ifx_mmc_card_release_host(card);

	return 1;

 cmd_err:
 	/*
 	 * If this is an SD card and we're writing, we can first
 	 * mark the known good sectors as ok.
 	 *
	 * If the card is not SD, we can still ok written sectors
	 * if the controller can do proper error reporting.
	 *
	 * For reads we just fail the entire chunk as that should
	 * be safe in all cases.
	 */
 	if (rq_data_dir(req) != READ && mmc_card_sd(card)) {
		u32 blocks;
		unsigned int bytes;

		blocks = mmc_sd_num_wr_blocks(card);
		if (blocks != (u32)-1) {
			if (card->csd.write_partial)
				bytes = blocks << md->block_bits;
			else
				bytes = blocks << 9;
			spin_lock_irq(&md->lock);
			ret = end_that_request_chunk(req, 1, bytes);
			spin_unlock_irq(&md->lock);
		}
	} else if (rq_data_dir(req) != READ &&
		   (card->host->caps & MMC_CAP_MULTIWRITE)) {
		spin_lock_irq(&md->lock);
		ret = end_that_request_chunk(req, 1, brq.data.bytes_xfered);
		spin_unlock_irq(&md->lock);
	}

	ifx_mmc_card_release_host(card);

flush_queue:
	spin_lock_irq(&md->lock);
	while (ret) {
		ret = end_that_request_chunk(req, 0,
				req->current_nr_sectors << 9);
	}

	add_disk_randomness(req->rq_disk);
	blkdev_dequeue_request(req);
	end_that_request_last(req, 0);
	spin_unlock_irq(&md->lock);

	return 0;
}

/* Allocate the buffer for block device */
static struct mmc_blk_data *mmc_blk_alloc(struct mmc_card *card)
{
	struct mmc_blk_data *md;
	int devidx, ret;

	devidx = find_first_zero_bit(dev_use, MMC_NUM_MINORS);
	if (devidx >= MMC_NUM_MINORS) {
		printk("%s[%d] Minor number Err!!!\n",__FUNCTION__,__LINE__);
		return ERR_PTR(-ENOSPC);
	}
	__set_bit(devidx, dev_use);

	md = kmalloc(sizeof(struct mmc_blk_data), GFP_KERNEL);
	if (!md) {
		printk("%s[%d] mem allocation failed!!!\n",__FUNCTION__,__LINE__);
		ret = -ENOMEM;
		goto out;
	}

	memset(md, 0, sizeof(struct mmc_blk_data));
	/*
	 * Set the read-only status based on the supported commands
	 * and the write protect switch.
	 */
	md->read_only = mmc_blk_readonly(card);

	/*
	 * Both SD and MMC specifications state (although a bit
	 * unclearly in the MMC case) that a block size of 512
	 * bytes must always be supported by the card.
	 */
	md->block_bits = 9;

	md->disk = alloc_disk(1 << MMC_SHIFT);
	if (md->disk == NULL) {
		printk("%s[%d] alloc disk failed!!!\n",__FUNCTION__,__LINE__);
		ret = -ENOMEM;
		goto err_kfree;
	}
	spin_lock_init(&md->lock);
	md->usage = 1;

	ret = ifx_mmc_init_queue(&md->queue, card, &md->lock);
	if (ret) {
		printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
		goto err_putdisk;
	}
	md->queue.prep_fn = mmc_blk_prep_rq;
	md->queue.issue_fn = mmc_blk_issue_rq;
	md->queue.data = md;

	md->disk->major	= major;
	md->disk->first_minor = devidx << MMC_SHIFT;
	md->disk->fops = &mmc_bdops;
	md->disk->private_data = md;
	md->disk->queue = md->queue.queue;
	md->disk->driverfs_dev = &card->dev;

	/*
	 * As discussed on lkml, GENHD_FL_REMOVABLE should:
	 *
	 * - be set for removable media with permanent block devices
	 * - be unset for removable block devices with permanent media
	 *
	 * Since MMC block devices clearly fall under the second
	 * case, we do not set GENHD_FL_REMOVABLE.  Userspace
	 * should use the block device creation/destruction hotplug
	 * messages to tell when the card is present.
	 */
	md->disk->flags = GENHD_FL_REMOVABLE;

	sprintf(md->disk->disk_name, "mmcblk%d", devidx);

	blk_queue_hardsect_size(md->queue.queue, 1 << md->block_bits);

	/*
	 * The CSD capacity field is in units of read_blkbits.
	 * set_capacity takes units of 512 bytes.
	 */
	set_capacity(md->disk, card->csd.capacity << (card->csd.read_blkbits - 9));
	return md;

 err_putdisk:
	put_disk(md->disk);
 err_kfree:
	kfree(md);
 out:
	return ERR_PTR(ret);
}

/* set the SD card block size */
static int
mmc_blk_set_blksize(struct mmc_blk_data *md, struct mmc_card *card)
{
	struct mmc_command cmd;
	int err;

	ifx_mmc_card_claim_host(card);
	cmd.opcode = MMC_SET_BLOCKLEN;
	cmd.arg = 1 << md->block_bits;
	cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
	err = ifx_mmc_wait_for_cmd(card->host, &cmd, 5);
	ifx_mmc_card_release_host(card);

	if (err) {
		printk(KERN_ERR "%s: unable to set block size to %d: %d\n",
			md->disk->disk_name, cmd.arg, err);
		return -EINVAL;
	}

	return 0;
}

/* probe the block device */
static int ifx_sd_blk_probe(struct mmc_card *card)
{
	struct mmc_blk_data *md;
	int err;

	/*
	 * Check that the card supports the command class(es) we need.
	 */
	if (!(card->csd.cmdclass & CCC_BLOCK_READ)) {
		printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
		return -ENODEV;
	}

	md = mmc_blk_alloc(card);
	if (IS_ERR(md))
		return PTR_ERR(md);

	err = mmc_blk_set_blksize(md, card);
	if (err) {
		printk("%s[%d] Err!!!\n",__FUNCTION__,__LINE__);
		goto out;
	}

	printk(KERN_INFO "%s: %s %s %lluKiB %s\n",
		md->disk->disk_name, mmc_card_id(card), mmc_card_name(card),
		(unsigned long long)(get_capacity(md->disk) >> 1),
		md->read_only ? "(ro)" : " r/w");

	mmc_set_drvdata(card, md);
	add_disk(md->disk);
	return 0;

 out:
	mmc_blk_put(md);

	return err;
}

/* Remove the block device */
static void ifx_sd_blk_remove(struct mmc_card *card)
{
	struct mmc_blk_data *md = mmc_get_drvdata(card);

	if (md) {
		int devidx;

		/* Stop new requests from getting into the queue */
		del_gendisk(md->disk);

		/* Then flush out any already in there */
		ifx_sd_card_cleanup_queue(&md->queue);

		devidx = md->disk->first_minor >> MMC_SHIFT;
		__clear_bit(devidx, dev_use);

		mmc_blk_put(md);
	}
	mmc_set_drvdata(card, NULL);
}

/** Driver version info */
static inline int mmc_block_drv_ver(char *buf)
{
    return sprintf(buf, "IFX MMC block driver driver, version %s,(c)2009 Infineon Technologies AG\n", version);
}

#ifdef CONFIG_PM
static int ifx_sd_blk_suspend(struct mmc_card *card, pm_message_t state)
{
	struct mmc_blk_data *md = mmc_get_drvdata(card);

	if (md) {
		ifx_mmc_queue_suspend(&md->queue);
	}
	return 0;
}

static int ifx_sd_blk_resume(struct mmc_card *card)
{
	struct mmc_blk_data *md = mmc_get_drvdata(card);

	if (md) {
		mmc_blk_set_blksize(md, card);
		ifx_mmc_queue_resume(&md->queue);
	}
	return 0;
}
#else
#define	ifx_sd_blk_suspend	NULL
#define ifx_sd_blk_resume	NULL
#endif

static struct mmc_driver mmc_driver = {
	.drv		= {
		.name	= "ifx_mmc_blk",
	},
	.probe		= ifx_sd_blk_probe,
	.remove		= ifx_sd_blk_remove,
	.suspend	= ifx_sd_blk_suspend,
	.resume		= ifx_sd_blk_resume,
};

/** Displays the version of DMA module via proc file */
static int mmc_block_proc_version(char *buf, char **start, off_t offset,
                         int count, int *eof, void *data)
{
    int len = 0;
    /* No sanity check cos length is smaller than one page */
    len += mmc_block_drv_ver(buf + len);
    *eof = 1;
    return len;    
}

/** create proc for debug  info, \used ifx_mmc_blk_init */
static int create_proc_mmc_block(void)
{
    /* procfs */
    ifx_mmc_block_dir = proc_mkdir ("driver/ifx_mmc_block", NULL);
    if (ifx_mmc_block_dir == NULL) {
        printk(KERN_ERR "%s: Create proc directory (/driver/ifx_mmc_block) failed!!!\n", __func__);
        return IFX_ERROR;
    }
    create_proc_read_entry("version", 0, ifx_mmc_block_dir, \
                    mmc_block_proc_version,  NULL);
    return IFX_SUCCESS;
}

/** remove of the proc entries, \used ifx_mmc_blk_exit */
static void delete_proc_mmc_block(void)
{ 
    remove_proc_entry("version", ifx_mmc_block_dir);
    remove_proc_entry("driver/ifx_mmc_block",  NULL);
}

/** Init of the MMC block module*/
static int __init ifx_mmc_blk_init(void)
{
	int res = -ENOMEM;
	char ver_str[128] = {0};
	
	res = register_blkdev(major, "ifx_sda");
	if (res < 0) {
		printk(KERN_WARNING "Unable to get major %d for MMC media: %d\n",
		       major, res);
		goto out;
	}
	if (major == 0)
		major = res;
    create_proc_mmc_block();
    /* Print the driver version info */                      
    mmc_block_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    res = ifx_mmc_register_driver(&mmc_driver);
    if (res)
        goto out2;
    return 0;
 out2:
    unregister_blkdev(major, "ifx_sda");
 out:
	return res;
}

/** Clean up MMC block moule */
static void __exit ifx_mmc_blk_exit(void)
{
	ifx_mmc_unregister_driver(&mmc_driver);
	unregister_blkdev(major, "ifx_mmc");
	 /** remove of the proc entries */
    delete_proc_mmc_block();
}

module_init(ifx_mmc_blk_init);
module_exit(ifx_mmc_blk_exit);
MODULE_DESCRIPTION("SD memory card device driver");
module_param(major, int, 0444);
MODULE_PARM_DESC(major, "specify the major device number for SD card drvice driver");
