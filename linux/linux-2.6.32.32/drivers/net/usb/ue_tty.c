/*
 * This driver is based on the cdc_ether driver from the Linux kernel code.
 * The hardware, although identifying as CDC ECM class, requires a TTY interface
 * for control.
 * The code is also "inspired" by: usb-serial, usb-skeleton, cdc-acm, and more
 *
 * CDC Ethernet based networking peripherals
 * Copyright (C) 2003-2005 by David Brownell
 * Copyright (C) 2006 by Ole Andre Vadla Ravnas (ActiveSync)
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/workqueue.h>
#include <linux/mii.h>
#include <linux/usb.h>
#include <linux/usb/cdc.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)) //ctc
  #include "usbnet.h"
#else
  #include <linux/usb/usbnet.h>
#endif
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include "ue.h"
#include "ue_lte.h"

// if we decide to remove the UE SERVICE driver and load firmware from flash 
// remove the following define
#define UE_SERVICE_EXISTS

#define UE_CONTROL_TIMEOUT_MS 5000

/* Output control lines. */
#define UECTL_CTRL_DTR		0x01
#define UECTL_CTRL_RTS		0x02

/* Input control lines and line errors. */
#define UECTL_CTRL_DCD		0x01
#define UECTL_CTRL_DSR		0x02
#define UECTL_CTRL_BRK		0x04
#define UECTL_CTRL_RI		0x08
#define UECTL_CTRL_FRAMING	0x10
#define UECTL_CTRL_PARITY	0x20
#define UECTL_CTRL_OVERRUN	0x40

/* marshal connect/disconnect and open/close */
static DEFINE_MUTEX(open_mutex);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
static const struct tty_port_operations uectl_port_ops = {
};
#define UECTL_READY(uectl)	(uectl && uectl->intf && uectl->port.count)
#else
#define UECTL_READY(uectl)	(uectl && uectl->intf && uectl->open_count)
#endif

//initialized below.
static struct tty_driver *uectl_driver;
static int buffer_left = UE_CONTROL_MSG_SIZE;


/* TTY operations */
//static void uectl_hangup(struct tty_struct *tty);
static int uectl_open(struct tty_struct *tty, struct file *filp);
static void uectl_close(struct tty_struct *tty, struct file *filp);
static int uectl_write(struct tty_struct *tty, const unsigned char *buf, int count);
static int uectl_write_room(struct tty_struct *tty);
static int uectl_chars_in_buffer(struct tty_struct *tty);
static int uectl_tiocmget(struct tty_struct *tty, struct file *file);
static int uectl_tiocmset(struct tty_struct *tty, struct file *file,
						  unsigned int set, unsigned int clear);
/* tty read path */
static int uectl_syncread(struct uedev *ue);
//// daniel  2010/12/13 14:18 
static void uectl_readerwork(struct work_struct *work);
//static void uectl_readerwork(struct delayed_work *work);

//tty ops virtual table.
static const struct tty_operations uectl_ops = {
	.open =			uectl_open,
	.close =		uectl_close,
	.write =		uectl_write,
	.write_room =	uectl_write_room,
	.tiocmget =		uectl_tiocmget,
	.tiocmset =		uectl_tiocmset,
	.chars_in_buffer =	uectl_chars_in_buffer,
//	.hangup =		uectl_hangup,
};

/* destroy an instance data. this may be called only after all open() calls were matched by close()
   and disconnect() was also called.
*/
static void uectl_unregister(struct uedev *ue)
{
	PDEBUG("entering");
	tty_unregister_device(uectl_driver, ue->minor);
	uedevs_table[ue->minor] = NULL;
	kfree(ue);
	PDEBUG("exiting");
}

static int uectl_open(struct tty_struct *tty, struct file *filp)
{
	struct uedev *ue;
	int rv = -ENODEV;
	PDEBUG("Entering uectl_open. process info: %i\t%s\n", current->pid, current->comm);

	mutex_lock(&open_mutex);
	PDEBUG("opening minor %d\n", tty->index);
	ue = uedevs_table[tty->index];
	// daniel  2010/12/12 16:37  
	ue->fw_loaded = 1;
	
	if (!ue){
		PDEBUG("ue is NULL\n");
		goto out;
	}else if(ue->fw_loaded == 0){
		rv = -EINPROGRESS;
		goto out;
	}else if(!ue->intf){
		PDEBUG("ue->intf is NULL\n");
		goto out;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	}else if(ue->port.count) {
		PDEBUG("forbidden multi open\n");
		goto out;
	} else
		rv = 0;
	tty_port_tty_set(&ue->port, tty);
	ue->port.count++;
	rv = tty_port_block_til_ready(&ue->port, tty, filp);
#else
	}else if(ue->open_count) {
		PDEBUG("forbidden multi open\n");
		goto out;
	}
	else
		rv = 0;
	ue->tty_port = tty;
	ue->open_count++;
#endif
	set_bit(TTY_NO_WRITE_SPLIT, &tty->flags);
	tty->driver_data = ue;
out:
	mutex_unlock(&open_mutex);
	PDEBUG("returning %d\n", rv);
	return rv;
}
/*
static void uectl_hangup(struct tty_struct *tty)
{
	struct uedev *uectl = tty->driver_data;
	PDEBUG("\n");
	tty_port_hangup(&uectl->port);
}
*/
static void uectl_close(struct tty_struct *tty, struct file *filp)
{
	struct uedev *uectl;

	if(!tty){
		PDEBUG("tty is NULL\n");
		return;// -ENODEV;
	}
	uectl = tty->driver_data;
	PDEBUG("Entering uectl_close. process info: %i\t%s\n", current->pid, current->comm);
	/* Perform the closing process and see if we need to do the hardware
	   shutdown */
	if (!uectl){
		PDEBUG("tty was not openned\n");
		return;
	}
	PDEBUG("closing minor %d\n", uectl->minor);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	if(uectl->port.count == 0){
		PDEBUG("port wasn't opened\n");
		return;
	}
	if(tty_port_close_start(&uectl->port, tty, filp) == 0){
		PDEBUG("not the last to close...\n");
		return;
	}
#else
	if(uectl->open_count == 0)
		return;
#endif
	mutex_lock(&open_mutex);
	PDEBUG("hanging up and resetting driver data\n");
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	tty_port_hangup(&uectl->port);
#else
	tty_hangup(uectl->tty_port);
	uectl->open_count--;
#endif
	tty->driver_data = NULL;
	if(!uectl->intf){
		PDEBUG("calling uectl_unregister()\n");		
		uectl_unregister(uectl);
	}
	PDEBUG("close end and port_set to NULL\n");
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	tty_port_close_end(&uectl->port, tty);
	tty_port_tty_set(&uectl->port, NULL);
#else
	uectl->tty_port = NULL;
#endif
	//we were ejected but our tty was still open. time to close down...
	mutex_unlock(&open_mutex);
	PDEBUG("done\n");
}

static int uectl_write(struct tty_struct *tty,
					 const unsigned char *buf, int count)
{
	struct uedev *ue = tty->driver_data;
	int retval = 0, ifnum;
	struct usb_device *udev;
	PDEBUG("enterintg: count=%d\n", count);
//	for(i=0;i<count;i++)
//		printk(KERN_ERR "(0x%02x) ", buf[i]);
	printk(KERN_ERR "\n");
	if(!ue || !ue->intf)
		return -ENODEV;
	udev = interface_to_usbdev (ue->intf);
	ifnum = ue->intf->cur_altsetting->desc.bInterfaceNumber;

    // send down the encapuslated command.
	retval = usb_control_msg(
		udev, usb_sndctrlpipe(udev, 0),
		USB_CDC_SEND_ENCAPSULATED_COMMAND,
		USB_TYPE_CLASS | USB_RECIP_INTERFACE,
		0, ifnum, (u8*)buf, count,
		UE_CONTROL_TIMEOUT_MS);
	PDEBUG("control msg returned %d\n", retval);
	if(retval < 0)
		return retval;
	return count;
}

static int uectl_write_room(struct tty_struct *tty)
{
	return UE_CONTROL_MSG_SIZE;
}

static int uectl_chars_in_buffer(struct tty_struct *tty)
{
    struct uedev *uectl = tty->driver_data;
    if (!UECTL_READY(uectl))
        return -EINVAL;
    /*   
     * This is inaccurate (overcounts), but it works.
     */
    return buffer_left;
}

static int uectl_tiocmget(struct tty_struct *tty, struct file *file)
{
	struct uedev *uectl = tty->driver_data;
	int ret = -EINVAL;
	mutex_lock(&open_mutex);
	if (!UECTL_READY(uectl))
		goto out;
	ret =  (uectl->ctrlout & UECTL_CTRL_DTR ? TIOCM_DTR : 0) |
	       (uectl->ctrlout & UECTL_CTRL_RTS ? TIOCM_RTS : 0) |
	       (uectl->ctrlin  & UECTL_CTRL_DSR ? TIOCM_DSR : 0) |
	       (uectl->ctrlin  & UECTL_CTRL_RI  ? TIOCM_RI  : 0) |
	       (uectl->ctrlin  & UECTL_CTRL_DCD ? TIOCM_CD  : 0) |
	       TIOCM_CTS;
out:
	mutex_unlock(&open_mutex);
	return ret;
}

static int uectl_tiocmset(struct tty_struct *tty, struct file *file,
						  unsigned int set, unsigned int clear)
{
	int ret = -EINVAL;
	struct uedev *uectl = tty->driver_data;
	unsigned int newctrl;
	mutex_lock(&open_mutex);
	if (!UECTL_READY(uectl))
		goto out;
	newctrl = uectl->ctrlout;
	set = (set & TIOCM_DTR ? UECTL_CTRL_DTR : 0) |
		(set & TIOCM_RTS ? UECTL_CTRL_RTS : 0);
	clear = (clear & TIOCM_DTR ? UECTL_CTRL_DTR : 0) |
		(clear & TIOCM_RTS ? UECTL_CTRL_RTS : 0);
	newctrl = (newctrl & ~clear) | set;
	uectl->ctrlout = newctrl;
	ret = 0;
out:
	mutex_unlock(&open_mutex);	
	return ret;
}

/*
  reader support for the control path.
  we run it in a work queue as we don't need any speed or throughput here.
  also, the use of this port is very rare and short.
  we expect only specialized software (modem managers) to use this interface and these
  apps are writing and reading responses based on a known sequence and protocol.
 */

static int uectl_syncread(struct uedev *ue)
{
	struct tty_struct *tty;
	int retval = UE_CONTROL_MSG_SIZE, ifnum;
	struct usb_device *udev = interface_to_usbdev (ue->intf);
	ifnum = ue->intf->cur_altsetting->desc.bInterfaceNumber;
	PDEBUG("performing the read\n");
	retval = usb_control_msg( udev, usb_rcvctrlpipe(udev, 0),
		USB_CDC_GET_ENCAPSULATED_RESPONSE,
		USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
		0, ifnum, ue->readdata, UE_CONTROL_MSG_SIZE, UE_CONTROL_TIMEOUT_MS);
	PDEBUG("usb read returned %d\n", retval);
	buffer_left = UE_CONTROL_MSG_SIZE - retval;

	if(retval > 0){
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
		tty = tty_port_tty_get(&ue->port);
#else
		tty = ue->tty_port;
#endif
		if(tty){
			// push to the tty layer the data
			//// daniel  2010/12/14 19:59  
			int ret;
			//printk("====daniel==== tty_insert_flip_string\n");
			ret = tty_insert_flip_string(tty, ue->readdata, retval);
			//printk("====daniel==== tty : %p\n", tty);
			//printk("====daniel==== ue->readdata : %p\n", ue->readdata);
			//printk("====daniel==== ue->readdata_data : %s\n", ue->readdata);
			printk("%s\n", ue->readdata);
			//printk("====daniel==== tty_insert_flip_string : %d\n", ret);
			//printk("====daniel==== tty_insert_buffer_push\n");
			tty_flip_buffer_push(tty);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
			tty_kref_put(tty);
#endif
		}
	}
	memset(ue->readdata, 0, UE_CONTROL_MSG_SIZE);
	return retval;
}

//// daniel  2010/12/13 14:17 
//static void uectl_readerwork(struct delayed_work *work)
static void uectl_readerwork(struct work_struct *work)
{
	struct uedev *u = container_of(work, struct uedev, readwork);
	PDEBUG("\n");
	uectl_syncread(u);
}

void uetty_unbind(struct uedev *ue)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	struct tty_struct *tty;
#endif
	mutex_lock(&open_mutex);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	if (ue->port.count == 0) {
		PDEBUG("calling uectl_unregister()\n");
		uectl_unregister(ue);
	} else {
		PDEBUG("someone is still open\n");
		tty = tty_port_tty_get(&ue->port);
		if (tty) {
			PDEBUG("hangup and kref put\n");
//			tty_port_hangup(&ue->port);
//			tty_hangup(tty);
			tty_kref_put(tty);
		}
	}
#else
	if (ue->open_count == 0) {
		PDEBUG("calling uectl_unregister()\n");
		uectl_unregister(ue);
	}
#endif
	mutex_unlock(&open_mutex);
}

void uetty_bind(struct uedev *ue)
{
    // init the reader worker queue.
	//// daniel  2010/12/13 14:17 
	INIT_WORK(&ue->readwork, uectl_readerwork);
	//INIT_DELAYED_WORK(&ue->readwork, uectl_readerwork);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
    // init the tty device.
	tty_port_init(&ue->port);
	ue->port.ops = &uectl_port_ops;
#else
	ue->open_count = 0;
#endif
	tty_register_device(uectl_driver, ue->minor, &ue->intf->dev);

}

int __init uetty_init(void)
{
	int ret = 0;
	uectl_driver = alloc_tty_driver(UE_DEVS);
	if (!uectl_driver)
		return -ENOMEM;
	uectl_driver->owner = THIS_MODULE;
	uectl_driver->driver_name = "uectl";
	uectl_driver->name = "ttyLTE";
	uectl_driver->major = 0; //auto assign
	uectl_driver->minor_start = 0;
	uectl_driver->type = TTY_DRIVER_TYPE_SERIAL;
	uectl_driver->subtype = SERIAL_TYPE_NORMAL;
	uectl_driver->flags = /*TTY_DRIVER_REAL_RAW |*/ TTY_DRIVER_DYNAMIC_DEV;
	uectl_driver->init_termios = tty_std_termios;
	uectl_driver->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	tty_set_operations(uectl_driver, &uectl_ops);
	ret = tty_register_driver(uectl_driver);
	if (ret)
		put_tty_driver(uectl_driver);
	return ret;
}

void __exit uetty_exit(void)
{
	tty_unregister_driver(uectl_driver);
	put_tty_driver(uectl_driver);
}
