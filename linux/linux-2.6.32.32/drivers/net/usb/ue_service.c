/*
 * This driver is based on the usb-skeleton driver from the Linux kernel code.
 * The hardware, it is not intended for general usage except by Altair's software
 * and personnel.
 *
 * Copyright (C) 2010 by Altair Semiconductor Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the linux kernel you work with; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
  This is a driver that exports character device access to the device's service and debug ports.
  this driver claims the 3rd interface of the UE device.

  we have 5 endpoints: 
  0x83 fw-in/log
  0x2 fw-out/console
  0x3 reset
  0x4 vl1-out
  0x84 vl1-in

  The driver exports two character device nodes for each device
  /dev/ueservice<N> - user reads from EP 0x83, writes to EP 0x2
  /dev/uedebug<N> - user reads from EP 0x84, writes to EP 0x4
  the reset port 0x3 will be operated via ioctl only(). the ioctl will be supported on /dev/ueservice<N> interface
  for simplicity, this driver's I/O is synchronous as we don't need any speed here.

  since this is not a general purpose driver but a driver that is intended to be opened by internally developed
  applications, we know that we will not lose data due to rx messages that are not being read. also, we assume
  that the reading application reads the entire message. therefore, each read() causes the buffer to be reused
  by the RX loop.
  RX flow:
  we have two buffers: inbuf[0] is always submitted to the USB stack via a urb, and inbuf[1] contains data that was
  returned from a previously completed urb. data_len contains the number of bytes available for a client application.
  on open() we submit a URB for the first time. when the URB's callback is returned, we either have data, or we are
  recovering from disconnect/error.
  if we have data we look at data_len. if data_len is 0, that means that the application consumed any previous RX data
  and we can swap buffers - we swap buffers and set data_len to the urb's actual_length.
  however, if data_len is not 0, that means that the application did not read the data from a previously submitted URB
  in such a case, we resubmit inbuf[0] and lose the most recent data.
  the read() operation, copies data from inbuf[1], and sets data_len to 0. this frees the buffer to be swapped when
  the currently submitted urb returns.
*/

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include "ue.h"

MODULE_AUTHOR("Yanir Lubetkin");
MODULE_DESCRIPTION("Altair LTE device - debug/service ports");
MODULE_LICENSE("GPL");

#define UESERVICE_MINORS (2 * UE_DEVS)

//#define PDEBUG(fmt, ...) printk(KERN_DEBUG "debug  %s:%d %s() : " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

//#define PDEBUG(fmt, ...) do{}while(0)

#define MAX_TRANSFER		(PAGE_SIZE * 16)

/*
  For each device, we have two character interfaces. the device structure represents it in the following manner.
  usb data which is common to both char devices is stored in struct ueservice. it also contains two members of
  struct uechr. an instance of struct uechr contains all the data required to implement the character device access.
*/

struct ueservice;
struct uechr{
	struct ueservice *ue;
	int minor;
	struct urb*     read_urb;
	struct cdev cdev;
	struct device *device;
	struct mutex		io_mutex;		/* synchronize I/O with disconnect */
	wait_queue_head_t waitq;       /* read  queue */
	u8 in_ep;
	u8 out_ep;
	u8* in_buf[2]; //we swap buffers on the RX path. while one buffer is submitted, the other may contain the most recent data from the device.
	dma_addr_t in_dma[2];
	int in_size;
	volatile int data_len;
};	

struct ueservice {
	struct kref kref;
	struct usb_device	*udev;			/* the usb device for this device */
	struct usb_interface	*intf;		/* the interface for this device */
	u8			reset_ep;	/* the address of the bulk reset endpoint */
	int			errors;			/* the last request tanked */
	spinlock_t		err_lock;		/* lock for errors */

	struct uechr dbg; /* char device vl1 (debug) context */
	struct uechr ctl; /* char device log/console/fwdownload (service) context */
	int open_count;
};

static struct class *ue_class;
static struct class *ue2_class;
static struct ueservice * ueservice_table[UE_DEVS];
static int major;

#define kref_to_ueservice(d) container_of(d, struct ueservice, kref)

static int ueservice_probe(struct usb_interface *intf, const struct usb_device_id *id);
static void ueservice_disconnect(struct usb_interface *intf);
static int submit_read_urb(struct uechr *uechr);

static const struct usb_device_id	products [] = {
	{
		/* altair LTE 216f:0040*/
		USB_DEVICE_AND_INTERFACE_INFO(0x216f, 0x0040, 0xff, 0, 0),
	},
    {
		/* altair LTE 216f:0041*/
		USB_DEVICE_AND_INTERFACE_INFO(0x216f, 0x0041, 0xff, 0, 0),
	},
	{ },// END
};
MODULE_DEVICE_TABLE(usb, products);

static struct usb_driver ueservice_driver = {
	.name =		"ueservice",
	.id_table =	products,
	.probe =	ueservice_probe,
	.disconnect =	ueservice_disconnect,
};

static void teardown_read_urb(struct uechr *uechr)
{
	if(uechr->in_buf[0])
		usb_buffer_free(uechr->read_urb->dev, uechr->in_size, uechr->in_buf[0], uechr->in_dma[0]);
	if(uechr->in_buf[1])
		usb_buffer_free(uechr->read_urb->dev, uechr->in_size, uechr->in_buf[1], uechr->in_dma[1]);
	usb_free_urb(uechr->read_urb);
}

static void ueservice_destroy(struct kref *kref)
{
	dev_t devnum;
	//called with read urbs killed
	//and with the usb interface already dissociated from this instance.
	struct ueservice *ue = kref_to_ueservice(kref);

	teardown_read_urb(&ue->dbg);
	teardown_read_urb(&ue->ctl);

	devnum = MKDEV(major, ue->ctl.minor);
	device_destroy(ue_class, devnum);

	devnum = MKDEV(major, ue->dbg.minor);
	device_destroy(ue2_class, devnum);

	cdev_del(&ue->dbg.cdev);
	cdev_del(&ue->ctl.cdev);
	usb_put_dev(ue->udev);
	ueservice_table[ue->ctl.minor] = 0;
	kfree(ue);
}

static void read_bulk_callback(struct urb *urb)
{
	struct uechr *uechr = urb->context;
	void *ptr = uechr->in_buf[0];
	if(!uechr->data_len){
        //swap the buffers
		uechr->in_buf[0] = uechr->in_buf[1];
		uechr->in_buf[1] = ptr;

		if(uechr->read_urb->actual_length > 0){
			uechr->data_len = uechr->read_urb->actual_length;
			wake_up_interruptible(&uechr->waitq);
		}
	}
    //if uechr->data_len !=0 then the process did not clear us to use the 2nd buffer - we drop a message and reuse the first buffer
	//PDEBUG("done: length = %d\n", uechr->read_urb->actual_length);
	submit_read_urb(uechr);
}

static int submit_read_urb(struct uechr *uechr)
{
	int result;
//	PDEBUG("entering\n");
	usb_fill_bulk_urb(uechr->read_urb, uechr->ue->udev,
					  usb_rcvbulkpipe(uechr->ue->udev,uechr->in_ep),
					  uechr->in_buf[0],uechr->in_size,
					  read_bulk_callback, uechr);

	result = usb_submit_urb(uechr->read_urb, GFP_ATOMIC);
	if (result)
		dev_err(uechr->device,"%s - submit read urb, error %d\n",
							__func__, result);
//	PDEBUG("done %d\n", result);
	return result;
}

static int ueservice_open(struct inode *inode, struct file *file)
{
	int retval = 0;
	struct cdev *cdev = inode->i_cdev;
	struct uechr * uechr = container_of(cdev, struct uechr, cdev);
	struct ueservice *ue = uechr->ue;
//	PDEBUG("testing if connected\n");
	if(!ue->intf)
		return -ENODEV;

//	PDEBUG("entering\n");
	/* increment our usage count for the device */
	kref_get(&ue->kref);
	/* lock the device to allow correctly handling errors
	 * in resumption */
	mutex_lock(&uechr->io_mutex);

	if (!ue->open_count++) {
        // notify the power management that we are busy
		retval = usb_autopm_get_interface(ue->intf);
		if (retval) {
			ue->open_count--;
			mutex_unlock(&uechr->io_mutex);
			kref_put(&ue->kref, ueservice_destroy);
			dev_err(uechr->device,"%s - usb_autopm_get_interface() returned error %d\n",
					__func__, retval);
			return retval;
		}
	}

	file->private_data = uechr;
	submit_read_urb(uechr);
	mutex_unlock(&uechr->io_mutex);
//	PDEBUG("exiting\n");
	return retval;
}

static int ueservice_release(struct inode *inode, struct file *file)
{
	struct uechr *uechr = (struct uechr *)file->private_data;
	struct ueservice *ue;
	if (uechr == NULL)
		return -ENODEV;
//	PDEBUG("entering\n");
	ue = uechr->ue;
	/* allow the device to be autosuspended */
	mutex_lock(&uechr->io_mutex);
	// daniel  2010/12/08 13:14 
	usb_kill_urb(uechr->read_urb);
	if (!--ue->open_count && ue->intf)
		usb_autopm_put_interface(ue->intf);

	mutex_unlock(&uechr->io_mutex);
	/* decrement the count on our device */
	kref_put(&ue->kref, ueservice_destroy);
	file->private_data = 0;
//	PDEBUG("exiting\n");
	return 0;
}

int ueservice_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	__u32 val = __constant_cpu_to_be32(1);
	int retval = 0, bytes_written;
	struct ueservice *ue;
	struct uechr *uechr = (struct uechr *)filp->private_data;
	if (uechr == NULL)
		return -ENODEV;
	ue = uechr->ue;
	PDEBUG("entering\n");
	switch(cmd){
	case 0x0d04e5e7:
		retval = usb_bulk_msg(uechr->ue->udev,
							  usb_sndbulkpipe(ue->udev, ue->reset_ep),
							  &val, 4, &bytes_written, 100);
		PDEBUG("exiting: written %d, retval %d\n", bytes_written, retval);
		break;
	default:
		PDEBUG("ioctl %x out of range, exiting\n", cmd);
		return -ERANGE;
	}
	return retval;
}

static ssize_t ueservice_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
	struct uechr * uechr = (struct uechr *)filp->private_data;
	int retval = 0;
//	PDEBUG("entering - count = %ld\n", count);
	//PDEBUG("1====daniel==== %d\n", retval);

	if(!uechr)
		return -ENODEV;

	mutex_lock(&uechr->io_mutex);
	if (!uechr->ue->intf) {
		retval = -ENODEV;
	//PDEBUG("2====daniel==== %d\n", retval);
		goto exit;
	}

	if(uechr->data_len == 0){
//		PDEBUG("no data\n");
		if (filp->f_flags & O_NONBLOCK){
			retval = -EAGAIN;
	//PDEBUG("3====daniel==== %d\n", retval);
			goto exit;
		}
//		PDEBUG("waiting for data\n");
		mutex_unlock(&uechr->io_mutex);
		if(wait_event_interruptible(uechr->waitq, (uechr->data_len != 0))){
			retval = -ERESTARTSYS;
	//PDEBUG("4====daniel==== %d\n", retval);
		}
		mutex_lock(&uechr->io_mutex);
		if(retval)
			goto exit;
	}

	//PDEBUG("got data: uechr->data_len = %d\n", uechr->data_len);
	if(count < uechr->data_len){
		if (copy_to_user(buffer, uechr->in_buf[1], count)){
			retval = -EFAULT;
	//PDEBUG("5====daniel==== %d\n", retval);
			goto exit;
		}
		retval = count;
	//PDEBUG("6====daniel==== %d\n", retval);
		uechr->data_len -= count;
		memmove(uechr->in_buf[1], uechr->in_buf[1] + count, uechr->data_len);
	}else{
		if (copy_to_user(buffer, uechr->in_buf[1], uechr->data_len)){
			retval = -EFAULT;
	//PDEBUG("7====daniel==== %d\n", retval);
			goto exit;
		}
		retval = uechr->data_len;
	//PDEBUG("8====daniel==== %d\n", retval);
		uechr->data_len = 0;
	}
exit:
	mutex_unlock(&uechr->io_mutex);
	//PDEBUG("9====daniel==== %d\n", retval);
	PDEBUG("done: retval = %d\n", retval);
	return retval;
}


static ssize_t ueservice_write(struct file *filp, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct uechr * uechr = (struct uechr *)filp->private_data;
	int retval = -ENODEV;
	int bytes_written;
	char *buf;
//	PDEBUG("entering - len=%ld\n", count);
	mutex_lock(&uechr->io_mutex);
	if (!uechr->ue->intf) {
		retval = -ENODEV;
		goto exit;
	}
	buf = kmalloc(count, GFP_KERNEL);
	if(!buf){
		retval = -ENOMEM;
		goto exit;
	}
	if(copy_from_user(buf, user_buffer, count)){
		retval = -EFAULT;
		goto exit_free;
	}
	//fflush(stdout);
	retval = usb_bulk_msg(uechr->ue->udev,
			      usb_sndbulkpipe(uechr->ue->udev, uechr->out_ep),
			      buf, count, &bytes_written, 1000);
	if(!retval)
		retval = bytes_written;
exit_free:
	kfree(buf);
exit:
	mutex_unlock(&uechr->io_mutex);
	PDEBUG("done - retval = %d\n", retval);
	return retval;
}


static const struct file_operations ueservice_fops = {
	.owner =	THIS_MODULE,
	.ioctl =	ueservice_ioctl,
	.read =		ueservice_read,
	.write =	ueservice_write,
	.open =		ueservice_open,
	.release =	ueservice_release,
};

static int init_read_urb(struct uechr *uechr)
{
    // allcate the urb descriptor and 2 buffers for swapping.
	int retval = 0;
	struct ueservice *ue = uechr->ue;
	uechr->read_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!uechr->read_urb) {
		return -ENOMEM;
		goto error;
	}
	uechr->in_buf[0] = usb_buffer_alloc(ue->udev, uechr->in_size, GFP_KERNEL, &uechr->in_dma[0]);
	if (!uechr->in_buf[0]) {
		retval = -ENOMEM;
		goto error;
	}
	uechr->in_buf[1] = usb_buffer_alloc(ue->udev, uechr->in_size, GFP_KERNEL, &uechr->in_dma[1]);
	if (!uechr->in_buf[1]) {
		retval = -ENOMEM;
		goto error;
	}
    // prepare the urb for submission with buf[0]
	
	//printk("----daniel---- read ep%d\n", usb_pipeendpoint(usb_rcvbulkpipe(uechr->ue->udev,uechr->in_ep)));
	//fflush(stdout);
	usb_fill_bulk_urb(uechr->read_urb, uechr->ue->udev,
					  usb_rcvbulkpipe(uechr->ue->udev,uechr->in_ep),
					  uechr->in_buf[0],uechr->in_size,
					  read_bulk_callback, uechr);

	return 0;
error:
	teardown_read_urb(uechr);
	return retval;
}

static int ueservice_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct ueservice *ue;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	struct uechr *dbg, *ctl;

	dev_t devnum;
	int i, minor;
	int retval = -ENOMEM;
	iface_desc = intf->cur_altsetting;
	if(iface_desc->desc.bNumEndpoints != 5)
		return -ENODEV;
    // looking for a free minor instance
	for(minor=0;minor < UE_DEVS && ueservice_table[minor];minor++);

	if(minor == UE_DEVS)
		return -ENODEV;

    // allocate instance context
	ue = kzalloc(sizeof(*ue), GFP_KERNEL);
	if(!ue)
		return -ENOMEM;

	ueservice_table[minor] = ue;

	dbg = &ue->dbg;
	ctl = &ue->ctl;

    //init the referece count
	kref_init(&ue->kref);

	spin_lock_init(&ue->err_lock);
	ue->udev = usb_get_dev(interface_to_usbdev(intf));

	dbg->ue = ue;
	ctl->ue = ue;
	dbg->minor = minor+UE_DEVS;// 32-63 is for the debug
	ctl->minor = minor; // 0 - 31

	mutex_init(&ctl->io_mutex);
	mutex_init(&dbg->io_mutex);

	init_waitqueue_head(&ctl->waitq);
	init_waitqueue_head(&dbg->waitq);

	devnum = MKDEV(major, minor);

    // init the service char device.
	cdev_init(&ctl->cdev, &ueservice_fops);
	retval = cdev_add(&ctl->cdev, devnum, 1);
	if(retval)
		goto error;

    // create the service char device
  #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
	ctl->device = device_create(ue_class, NULL, devnum, NULL, "ueservice%d", minor);
  #elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	ctl->device = device_create_drvdata(ue_class, NULL, devnum, NULL, "ueservice%d", minor);
  #else
	ctl->device = device_create(ue_class, NULL, devnum, "ueservice%d", minor);
	dev_set_drvdata(ctl->dev, NULL);
  #endif

	if(!ctl->device){
		retval = -ENODEV;
		goto error;
	}

	devnum = MKDEV(major, minor+UE_DEVS);
    // init the debug char device.
	cdev_init(&dbg->cdev, &ueservice_fops);
	retval = cdev_add(&dbg->cdev, devnum, 1);
	if(retval)
		goto error;

    // create the debug char device
  #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
	dbg->device = device_create(ue2_class, NULL, devnum, NULL, "uedebug%d", minor);
  #elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	dbg->device = device_create_drvdata(ue2_class, NULL, devnum, NULL, "uedebug%d", minor);
  #else
	dbg->device = device_create(ue2_class, NULL, devnum, "uedebug%d", minor);
	dev_set_drvdata(dbg->dev, NULL);
  #endif
	if(!dbg->device){
		retval = -ENODEV;
		goto error;
	}

    
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		switch(i){
		case 0:
            // this is the read ep of the service driver
			if(!usb_endpoint_is_bulk_in(endpoint))
				goto error;

			ctl->in_size = le16_to_cpu(endpoint->wMaxPacketSize);
			ctl->in_ep = endpoint->bEndpointAddress;
			break;
		case 1:
            // this is the write ep of the service driver
			if(!usb_endpoint_is_bulk_out(endpoint))
				goto error;
			ctl->out_ep = endpoint->bEndpointAddress;
			break;
		case 2:
            // this is the reset ep of the service driver
			if(!usb_endpoint_is_bulk_out(endpoint))
				goto error;
			ue->reset_ep = endpoint->bEndpointAddress;
			break;
		case 3:
            // this is the read ep of the debug driver
			if(!usb_endpoint_is_bulk_out(endpoint))
				goto error;
			dbg->out_ep = endpoint->bEndpointAddress;
			break;
		case 4:
            // this is the write ep of the debug driver
			if(!usb_endpoint_is_bulk_in(endpoint))
				goto error;
			dbg->in_size = le16_to_cpu(endpoint->wMaxPacketSize);
			dbg->in_ep = endpoint->bEndpointAddress;
			break;
		}
	}
	if (i != 5) {
		err("Could not find all required endpoints. found %d\n", i);
		goto error;
	}

	PDEBUG("endpoints initialized\n");

    // register your context with the usb interface
	usb_set_intfdata(intf, ue);
	ue->intf = intf;

    // init the urbs for both char devs endpoints
	retval = init_read_urb(ctl);
	if(retval)
		goto error;
	retval = init_read_urb(dbg);
	if(retval)
		goto error;
	return 0;
error:
	usb_set_intfdata(intf, NULL);
	kref_put(&ue->kref, ueservice_destroy);
	return retval;
}

static void ueservice_disconnect(struct usb_interface *intf)
{
	struct ueservice *ue = usb_get_intfdata(intf);
    // disconnet from usb interface.
	usb_set_intfdata(intf, NULL);

	ue->intf = NULL;

	kref_put(&ue->kref, ueservice_destroy);
}

static void __exit ueservice_exit(void)
{
	usb_deregister(&ueservice_driver);
	if(ue_class)
		class_destroy(ue_class);
	if(ue2_class)
		class_destroy(ue2_class);
	if(major)
		unregister_chrdev_region(MKDEV(major, 0), UESERVICE_MINORS);

}
module_exit(ueservice_exit);


static int __init ueservice_init(void)
{
	int retval;
	dev_t devnum;
	memset(&ueservice_table, 0, UE_DEVS*sizeof(void *));
	retval = alloc_chrdev_region(&devnum, UE_DEVS, 0, "ueservice");
	if (retval < 0)
		return retval;
	major = MAJOR(devnum);
	ue_class = class_create(THIS_MODULE, "ueservice");
	if (IS_ERR(ue_class)){
		retval = PTR_ERR(ue_class);
		goto error;
	}
	ue2_class = class_create(THIS_MODULE, "uedebug");
	if (IS_ERR(ue2_class)){
		retval = PTR_ERR(ue2_class);
		goto error;
	}
 	retval = usb_register(&ueservice_driver);
	if(!retval)
		return 0;
error:
	if(ue_class)
		class_destroy(ue_class);
	if(ue2_class)
		class_destroy(ue2_class);
	if(major)
		unregister_chrdev_region(MKDEV(major, 0), UESERVICE_MINORS);
	return retval;
}
module_init(ueservice_init);
