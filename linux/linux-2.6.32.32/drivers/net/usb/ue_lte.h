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

#ifndef __UE_LTE_H__
#define __UE_LTE_H__
#define UE_CONTROL_MSG_SIZE 1024

struct uedev {
	int carrier_on;
	u8 rx_ethhdr[ETH_HLEN]; /* when receiving data, we copy this header in front of each payload fragment to simulate ethernet behavior. */
//urbs lists;
	int num_tx_urbs; //set to default on bind(), may be changed via sysfs if the ue->carrier_on==0
	spinlock_t txurb_lock;
	struct list_head tx_urbs;//txurbs for use in future transactions
	spinlock_t txurb_done_lock;
	struct list_head done_tx_urbs;//txurbs returned via tx_complete()
	struct sk_buff_head tx_skbs;//skbs pending xmit 
//urb memory pool. for pre-allocation 
	struct txurb* upool;
	struct tasklet_struct	do_tx;
//	struct tasklet_struct	txurb_cleanup;
	int max_tx_urb_size;
	int fw_loaded;
	struct usbnet *usn;
//tty related
	struct usb_interface *intf; /* reference to the control interface */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	struct tty_port port; /* our tty port data */
#else
	struct tty_struct *tty_port;
	int	open_count;	/* number of times this port has been opened */
#endif
	int major; /* device node */
	int minor;
	//// daniel  2010/12/13 14:14 
	struct work_struct	readwork; /* when an interrupt arrives that we have a response pending, we run this task to retrieve the data */
	//struct delayed_work	readwork; /* when an interrupt arrives that we have a response pending, we run this task to retrieve the data */
	unsigned int ctrlin;				/* input control lines (DCD, DSR, RI, break, overruns) */
	unsigned int ctrlout;				/* output control lines (DTR, RTS) */
	struct driver_info ue_dinfo; /* in order to support multiple instances, we clone the static info, but change the "data" pointer. */
	/*
	  NOTE: this code assumes two things:
	  1. we have only one control opened
	  2. the device only responds to commands and the opening process will read the response.
	  this leads to the simple solution that each response erases the previous data stored in the rxdata buffer.
	*/
	u8 readdata[UE_CONTROL_MSG_SIZE]; /* comm responses are stored here */
	//tx concatenation related data
	int bpflag;
	int urbs_in_flight;
	volatile int queued_data_len;
	int single_sent;
	int concat_sent;
	int backpressure;
	int myerrors;
};

//minors table
extern struct uedev *uedevs_table[];

// sysfs support
extern struct attribute_group ue_dev_attr_group;

//defined in ue_tty.c, called from ue_cdc.c
int uetty_init(void);
void uetty_exit(void);
void uetty_bind(struct uedev *ue);
void uetty_unbind(struct uedev *ue);

#endif
