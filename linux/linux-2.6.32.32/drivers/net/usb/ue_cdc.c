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
#include <linux/list.h>
#include <linux/skbuff.h>
#include "ue.h"
#include "ue_lte.h"
#include <linux/time.h>

MODULE_AUTHOR("Yanir Lubetkin");
MODULE_DESCRIPTION("Altair LTE device");
MODULE_LICENSE("GPL");

/* 
   TX design:
   requirements:
   * fixed number of init time allocated TX urbs.
   * skb may be sent immediately if there is a urb, or queued and sent as part of a concatenated chain
     when one of the urbs is returned from the usb stack
   design considerations:
   a cleanup operation should be performed aftre a urb is returned from the stack via interrupt.
   * release all skbs that were sent via this urb
   * return the urb to the pool for further use.
   a send operation may occur when one of two events happen:
   * a urb is released from the usb stack to the pool and there are pending skbs.
   * a skb is delivered for tx from the ip stack and there are free urbs. (process context)
   error handling work may occur - it must not run atomic context. it is very rare.
   cleanup work is too long to be performed during the interrupt handler

   there are several choices to answer "which context(s) will execute the cleanup and send sequences:
   1. one of the existing contexts (interrupt/process)
   2. the system's workqueue, via schedule_work()
   3. a dedicated thread, either coded loop, or via create_singlethreaded_workqueue() and queue_work().
   4. a tasklet.

   in the case of #3: there is the question weather to use a workqueue or a coded loop:
   considerations:
   * hand made code is easier to port to non-linux OS
   * workqueue is GPL bounded
   * scheduling policy may be applied to a thread, but not to the workqueue.

   if proper locking is in place, all methods are interchangeable. the "worker" code can be written
   to run properly from all the above contexts.

   data structures and locking schema:
   we have the following queues for each device:
   pending skbs queue: waiting to be transmitted.
   free urbs queue: may be used in future transactions
   done urbs queue: returned from the usb stack, with skbs on them, waiting to be returned to the pool.

   pending skbs: inserted during process transmit, removed by the send worker
   free urbs queue: inserted by the cleanup worker, removed by the send worker
   done urbs queue: inserted by the interrupt handler, removed by the cleanup worker.

   
 */
#define USE_PACKET_CONCATENATION_CODE

#define DEFAULT_NUM_TX_URBS 4
#define TX_URB_BUFSIZE 65535
#define RX_URB_BUFSIZE 65535
#define DEFAULT_TX_URB_SIZE 10640
// reawaken network queue this soon after stopping; else watchdog barks
#define TX_TIMEOUT_JIFFIES	(5*HZ)

// throttle rx/tx briefly after some faults, so khubd might disconnect()
// us (it polls at HZ/4 usually) before we report too many false errors.
#define THROTTLE_JIFFIES	(HZ/8)

#define RX_MAX_QUEUE_MEMORY (60 * 1518)
#define	RX_QLEN(dev) (((dev)->udev->speed == USB_SPEED_HIGH) ? \
			(RX_MAX_QUEUE_MEMORY/(dev)->rx_urb_size) : 4)
#define	TX_QLEN(dev) (((dev)->udev->speed == USB_SPEED_HIGH) ? \
			(RX_MAX_QUEUE_MEMORY/(dev)->hard_mtu) : 4)
#define BACKPRESSURE_COND (ue->queued_data_len > ue->num_tx_urbs * ue->max_tx_urb_size)
//#define BACKPRESSURE_COND (ue->tx_skbs.qlen > 6)

// between wakeups
#define UNLINK_TIMEOUT_MS	3

struct txurb {
	struct list_head list; //txurb may itself be a part of a list (either done pool, or free pool)
	struct urb *urb;
	struct usbnet *usn;
	struct uedev *ue;
	struct list_head skbs;//list head of all skbs sent over this urb
	int nskbs;//number of skbs sent by (and in the skbs list of) this urb.
	//each urb has a pre allocated buffer for concatenation cases.
	dma_addr_t buf_dma;
	u8 * buf;
	int len;
};

/*
  a word about packet format and packet concatenation:
  * the device receives packets in the following format:
  32bit altair LTE header
  16 bit, big endian - reserved
  16 bit, big endian - size of payload
  payload:
  the payload is the same as the ethernet payload.
  padding:
  if the size of the header+payload does not end on a 32 bit boundery, we start the next header on the next 32 bit word.
*/

struct ue_hdr{
	__be16 reserved;
	__be16 len;
};

/* for each rx packet we prepend this header and setup the last two bytes based on the protocol. */
const u8 s_rxhdr[] = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x44, 0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0x64, 0x08, 0x00};

/* one instance for each inserted device */
struct uedev *uedevs_table[UE_DEVS];

/* we are a "usbnet" driver - we plug into the usbnet framework and override whatever we need. */
static int ue_rx_fixup(struct usbnet *dev, struct sk_buff *skb);
static struct sk_buff * ue_tx_fixup(struct usbnet *dev, struct sk_buff *skb, gfp_t flags);
static void ue_unbind(struct usbnet *dev, struct usb_interface *intf);
static void dumpspeed(struct usbnet *dev, __le32 *speeds);
static void ue_status(struct usbnet *dev, struct urb *urb);
static int ue_bind(struct usbnet *dev, struct usb_interface *intf);
static int ue_check_connect(struct usbnet *dev);
static int ue_start_xmit (struct sk_buff *skb, struct net_device *net);
static void tx_complete (struct urb *urb);

// daniel  2010/12/03 08:19 
static struct net_device_stats internal_stats;
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)
unsigned long stamp_1;

/* startup/teardown code */
static int __init ue_init(void);
static void __exit ue_exit(void);

/*we register to the usbnet framework with this generic instance, but we replace it
  with a dynamically allocated copy during bind(). the dynamically allocated copy contains
  the "per instance" data handle. we need this static instance to register properly with the
  framework. */
static const struct driver_info	ue_info = {
	.description =	"Altair LTE Device",
//	.flags =	FLAG_WWAN,  in later kernels this will be the type we will want to be */
	.bind =		ue_bind,
	.unbind =	ue_unbind,
	.status =	ue_status,
	.rx_fixup =	ue_rx_fixup,
	.tx_fixup =     ue_tx_fixup,
	.check_connect = ue_check_connect,
};

/* the filter used to determine if the device should be claimed by us */
static const struct usb_device_id	products [] = {
	{
		/* altair LTE 216f:0040*/
		USB_DEVICE_AND_INTERFACE_INFO(0x216f, 0x0040, USB_CLASS_COMM,
									  USB_CDC_SUBCLASS_ETHERNET, USB_CDC_ACM_PROTO_VENDOR),
		.driver_info = (unsigned long) &ue_info,
	},
	{
		USB_DEVICE_AND_INTERFACE_INFO(0x216f, 0x0040, USB_CLASS_COMM,
									  USB_CDC_SUBCLASS_ETHERNET, USB_CDC_PROTO_NONE),
		.driver_info = (unsigned long) &ue_info,
	},
    {
		/* altair LTE 216f:0041*/
		USB_DEVICE_AND_INTERFACE_INFO(0x216f, 0x0041, USB_CLASS_COMM,
									  0xff, USB_CDC_ACM_PROTO_VENDOR),
		.driver_info = (unsigned long) &ue_info,
	},
	{
		USB_DEVICE_AND_INTERFACE_INFO(0x216f, 0x0041, USB_CLASS_COMM,
									  0xff, USB_CDC_PROTO_NONE),
		.driver_info = (unsigned long) &ue_info,
	},
	{ },// END
};
MODULE_DEVICE_TABLE(usb, products);

// usb ops virtual table
static struct usb_driver ue_driver = {
	.name =		"ue_lte",
	.id_table =	products,
	.probe =	usbnet_probe,
	.disconnect =	usbnet_disconnect,
	.suspend =	usbnet_suspend,
	.resume =	usbnet_resume,
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
/* we have to replace the netdev_ops that usbnet provides with our own as theirs is const.*/
static const struct net_device_ops ue_netdev_ops = {
	.ndo_open		= usbnet_open,
	.ndo_stop		= usbnet_stop,
#ifdef USE_PACKET_CONCATENATION_CODE
	.ndo_start_xmit		= ue_start_xmit,
#else
	.ndo_start_xmit		= usbnet_start_xmit,
#endif
	.ndo_tx_timeout		= usbnet_tx_timeout,
	.ndo_change_mtu		= usbnet_change_mtu,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};
#endif

static int init_tx_urbs(struct usbnet *usn, int nurbs)
{
	int i;
	struct uedev *ue = (struct uedev *)usn->driver_info->data;
	PDEBUG("entering: ue=%p, nurbs=%d\n", ue, nurbs);
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	ue->upool = kzalloc(nurbs * sizeof(struct txurb), GFP_KERNEL);
	INIT_LIST_HEAD(&ue->tx_urbs);
	if(!ue->upool)
		return -ENOMEM;
	for(i=0;i<nurbs;i++){
		ue->upool[i].urb = usb_alloc_urb(0, GFP_KERNEL);
		if(!ue->upool[i].urb)
			return -ENOMEM;
		ue->upool[i].ue = ue;
		ue->upool[i].usn = usn;
		INIT_LIST_HEAD(&ue->upool[i].list);
		INIT_LIST_HEAD(&ue->upool[i].skbs);
		ue->upool[i].buf = usb_buffer_alloc(usn->udev, TX_URB_BUFSIZE, GFP_KERNEL, &ue->upool[i].buf_dma);
		if(!ue->upool[i].buf)
			return -ENOMEM;
		list_add(&ue->upool[i].list, &ue->tx_urbs);
	}
	PDEBUG("done\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	return 0;
}

static void destroy_tx_urbs(struct uedev *ue, int nurbs)
{
	int i;
	for(i=0;i<nurbs;i++){
			usb_kill_urb(ue->upool[i].urb);
		if(ue->upool[i].buf)
			usb_buffer_free(ue->upool[i].usn->udev, TX_URB_BUFSIZE,
							ue->upool[i].buf, ue->upool[i].buf_dma);
		if(ue->upool[i].urb)
			usb_free_urb(ue->upool[i].urb);
	}
	kfree(ue->upool);
	ue->upool = 0;
	INIT_LIST_HEAD(&ue->tx_urbs);
}


static int ue_rx_fixup(struct usbnet *dev, struct sk_buff *skb)
{
	u8 *mac;
	struct sk_buff		*skb2;
	struct net_device	*net = dev->net;
	int datalen;
	struct iphdr *ip;
	struct ue_hdr* hdr;
	struct uedev *ue = (struct uedev *)dev->driver_info->data;
	/* peripheral may have batched packets to us... */
	PDEBUG("entering\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	if(skb->len < sizeof(*hdr)){//we got a bad skb
		PDEBUG("ERROR: skb=%p, len=%d\n", skb, skb->len);
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.rx_errors++;
	  #else
		net->stats.rx_errors++;
	  #endif
		internal_stats.rx_errors++;
	}
	while (likely(skb->len > sizeof(*hdr))) {
		hdr = (struct ue_hdr *)skb->data;
		datalen = be16_to_cpu(hdr->len);
		PDEBUG("skb=%p, len=%d, datalen=%d\n", skb, skb->len, datalen);
		if (datalen == 0 || datalen > skb->len ){
			PDEBUG("error: skb=%p, len=%d, datalen=%d\n", skb, skb->len, datalen);
		  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
			dev->stats.rx_errors++;
		  #else
			net->stats.rx_errors++;
		  #endif
			internal_stats.rx_errors++;
			ue->myerrors++;
			break;
		}
		skb_pull(skb, sizeof(*hdr));
		//dump_ip((struct iphdr *)skb->data);
		if ((skb2 = alloc_skb (datalen + ETH_HLEN + NET_IP_ALIGN, GFP_ATOMIC)) == NULL) {
			if (netif_msg_rx_err (dev))
				devdbg (dev, "no rx skb");
			return 0;
		}
		skb_reserve (skb2, NET_IP_ALIGN);

        // first put the fake eth header
		memcpy(skb2->data, ue->rx_ethhdr, ETH_HLEN);
		mac = skb2->data;
		skb_put(skb2,ETH_HLEN);

        // check if we are ipv6 and change eth header accordingly
		ip = (struct iphdr *)skb->data;
		if(ip->version == 6){
			skb2->data[12] = 0x86;
			skb2->data[13] = 0xdd;
		}
		if(unlikely(ip->daddr == 0xffffffff)){//this is a broadcast IP packet we need an ethernet broadcast dest.
			mac[0] = 0xff;
			mac[1] = 0xff;
			mac[2] = 0xff;
			mac[3] = 0xff;
			mac[4] = 0xff;
			mac[5] = 0xff;
		}
        //copy ip payload
		memcpy(skb_tail_pointer(skb2), skb->data, datalen);
		skb_put(skb2, datalen);

        // submit the skb2 to the ip stack.
		usbnet_skb_return(dev, skb2);

        // setup the ptr in the original skb to the next concatinated ip packet.
		if(skb->len > datalen)// 
			skb_pull(skb, ALIGN(datalen, 4));
		else
			break;
		//skb_pull(skb, datalen);
	}
	PDEBUG("done\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	//hack - usbnet regards this as an error since we didn't submit the original skb, thus we se the rx_errors -1.

	//if our firmware will add a spacer of size ETH_HLEN - 4 between packets, we could have avoided this
	//we also could have avoided the entire copy procedure and just skb_clone() and point the clone to the
	//right segment of the payload.
  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
	dev->stats.rx_errors--;
  #else
	net->stats.rx_errors--;
  #endif
	internal_stats.rx_errors--;

	return 0; //original skb will become an error
}

static struct sk_buff * ue_tx_fixup(struct usbnet *dev, struct sk_buff *skb, gfp_t flags)
{
	int align;
	struct ue_hdr* hdr;
	PDEBUG("\n");
/*
  in this version, we do not handle concatenation on the tx path,
  we simply replace the 14 bytes of the ethernet header with our own 4 bytes header
  just before the start of the IP data. also, to comply with the 23 bit alignment
  requirement of the device, we pad the end of the data as required.
*/
	skb_pull(skb, ETH_HLEN);
	hdr = (struct ue_hdr *) skb_push(skb, sizeof(*hdr));
	
    // fill the header
	hdr->reserved = 0;
	hdr->len = cpu_to_be16(skb->len - sizeof(*hdr));
	
	align = ALIGN(skb->len, 4) - skb->len;
	skb_put(skb, align);
	return skb;
}

//we need the nibble function for our sysfs support even if there is one in usbnet.
static u8 nibble(unsigned char c)
{
	if (likely(isdigit(c)))
		return c - '0';
	c = toupper(c);
	if (likely(isxdigit(c)))
		return 10 + c - 'A';
	return 0;
}

//introduced to the vanilla kernel in 2.6.31
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))

static int usbnet_get_ethernet_addr(struct usbnet *dev, int iMACAddress)
{
	int 		tmp, i;
	unsigned char	buf [13];

	tmp = usb_string(dev->udev, iMACAddress, buf, sizeof buf);
	if (tmp != 12) {
		dev_dbg(&dev->udev->dev,
			"bad MAC string %d fetch, %d\n", iMACAddress, tmp);
		if (tmp >= 0)
			tmp = -EINVAL;
		return tmp;
	}
	for (i = tmp = 0; i < 6; i++, tmp += 2)
		dev->net->dev_addr [i] =
			(nibble(buf [tmp]) << 4) + nibble(buf [tmp + 1]);
	return 0;
}

#endif


/*
  sysfs support
================
  supported attributes

  fw_loaded r/w
  num_tx_urbs r/w
  peer_mac_addr r/w
*/


static struct uedev *dev_to_ue(struct device *dev)
{
	int i;
	PDEBUG("dev=%p\n", dev);
	for(i=0;i<UE_DEVS;i++){
		if(uedevs_table[i])
			if( &uedevs_table[i]->intf->dev == dev){
				return uedevs_table[i];
		}
	}
	return NULL;
}

static ssize_t ue_fw_loaded_show(struct device *dev,
								 struct device_attribute *attr,
								  char *buf) //size is always PAGE_SIZE for sysfs buffers
{
	ssize_t result = -EINVAL;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	result = sprintf(buf, "%d", ue->fw_loaded);
	return result;
}
static ssize_t ue_fw_loaded_store(struct device *dev,
								  struct device_attribute *attr,
								  const char *buf, size_t size)
{
	int val;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	if (sscanf(buf, "%d\n", &val) != 1)
		return -EINVAL;
	if(val != 0 && val != 1)
		return -EINVAL;
	ue->fw_loaded = val;
	return size;
}

static ssize_t ue_num_tx_urbs_show(struct device *dev,
								 struct device_attribute *attr,
								  char *buf) //size is always PAGE_SIZE for sysfs buffers
{
	ssize_t result = -EINVAL;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	result = sprintf(buf, "%d", ue->num_tx_urbs);
	return result;
}

static ssize_t ue_num_tx_urbs_store(struct device *dev,
								  struct device_attribute *attr,
								  const char *buf, size_t size)
{
	int val;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	if (sscanf(buf, "%d\n", &val) != 1)
		return -EINVAL;
	if(val != 0 && val != 1)
		return -EINVAL;
	if(ue->carrier_on != 0)
		return -EINVAL;
	destroy_tx_urbs(ue, ue->num_tx_urbs);
	ue->num_tx_urbs = val;
	val = init_tx_urbs(ue->usn, ue->num_tx_urbs);
	if(val < 0)
		return val;
	return size;
}


static ssize_t ue_max_tx_urb_size_show(struct device *dev,
								 struct device_attribute *attr,
								  char *buf) //size is always PAGE_SIZE for sysfs buffers
{
	ssize_t result = -EINVAL;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	result = sprintf(buf, "%d", ue->max_tx_urb_size);
	return result;
}

static ssize_t ue_max_tx_urb_size_store(struct device *dev,
								  struct device_attribute *attr,
								  const char *buf, size_t size)
{
	int val;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	if (sscanf(buf, "%d\n", &val) != 1)
		return -EINVAL;
	if(val != 0 && val != 1)
		return -EINVAL;
	if(ue->carrier_on != 0)
		return -EINVAL;
	ue->max_tx_urb_size = val;
	return size;
}

static ssize_t ue_peer_mac_addr_show(struct device *dev,
								 struct device_attribute *attr,
								  char *buf) //size is always PAGE_SIZE for sysfs buffers
{
	ssize_t result = -EINVAL;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	result = sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
					 ue->rx_ethhdr[6], ue->rx_ethhdr[7],
					 ue->rx_ethhdr[8], ue->rx_ethhdr[9],
					 ue->rx_ethhdr[10], ue->rx_ethhdr[11]);
	return result;
}


static ssize_t ue_peer_mac_addr_store(struct device *dev,
								  struct device_attribute *attr,
								  const char *buf, size_t size)
{
	int i;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	if(ue->carrier_on != 0)
		return -EINVAL;
	if(strlen(buf) != 17)
		return -EINVAL;
	for(i=1;i<6;i++)
		if(buf[3*i-1] != ':')
			return -EINVAL;
	for(i=0;i<6;i++)
		ue->rx_ethhdr[6+i] = nibble(buf[3*i]) << 4 | nibble(buf[3*i]+1);
	return size;
}

static ssize_t ue_stats_show(struct device *dev,
								 struct device_attribute *attr,
								  char *buf) //size is always PAGE_SIZE for sysfs buffers
{
	ssize_t result = -EINVAL;
	struct uedev *ue = dev_to_ue(dev);
	if(!ue)
		return -ENODEV;
	result = sprintf(buf, "s=%d,c=%d,bp=%d,e=%d\n", ue->single_sent, ue->concat_sent, ue->backpressure, ue->myerrors);
	ue->single_sent = 0;
	ue->concat_sent = 0;
	ue->backpressure = 0;
	ue->myerrors = 0;
	return result;
}

static DEVICE_ATTR(ue_fw_loaded, S_IRUGO | S_IWUSR,
				   ue_fw_loaded_show, ue_fw_loaded_store);
static DEVICE_ATTR(ue_num_tx_urbs, S_IRUGO | S_IWUSR,
				   ue_num_tx_urbs_show, ue_num_tx_urbs_store);
static DEVICE_ATTR(ue_peer_mac_addr, S_IRUGO | S_IWUSR,
				   ue_peer_mac_addr_show, ue_peer_mac_addr_store);
static DEVICE_ATTR(ue_max_tx_urb_size, S_IRUGO | S_IWUSR,
				   ue_max_tx_urb_size_show, ue_max_tx_urb_size_store);
static DEVICE_ATTR(ue_stats, S_IRUGO | S_IWUSR,
				   ue_stats_show, NULL);

static struct attribute *ue_dev_attrs[] = {
	&dev_attr_ue_fw_loaded.attr,
	&dev_attr_ue_num_tx_urbs.attr,
	&dev_attr_ue_max_tx_urb_size.attr,
	&dev_attr_ue_peer_mac_addr.attr,
	&dev_attr_ue_stats.attr,
	NULL,
};

struct attribute_group ue_dev_attr_group = {
	.name = NULL,		/* we want them in the same directory */
	.attrs = ue_dev_attrs,
};

/*
  we replace the default usbnet's hard_start_xmit() with our call in order to concatenate packets
  we also set the done callback
*/
//////////////////////////////

//may run in an atomic context (probably tasklet)
static void txurb_done(unsigned long param)
{
	int i;
	struct uedev *ue = (struct uedev *)param;
	unsigned long flags;
	struct txurb *txurb;
	struct sk_buff *skb;
	PDEBUG("entering\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	for(i=0;i<ue->num_tx_urbs;i++){
		txurb = NULL;
		spin_lock_irqsave(&ue->txurb_done_lock, flags);
		if(!list_empty(&ue->done_tx_urbs)){
			txurb = list_first_entry(&ue->done_tx_urbs, struct txurb, list);
			list_del(&txurb->list);
		}
		spin_unlock_irqrestore(&ue->txurb_done_lock, flags);
		if(!txurb)
			break;
		PDEBUG("processing txurb %p\n", txurb);
		//cleanup all skbs, bring the txurb to a proper state, return it to the pool
		while(!list_empty(&txurb->skbs)){
			skb = (struct sk_buff*)(txurb->skbs.next);
			PDEBUG("processing skb %p\n", skb);
			list_del((struct list_head*)skb);
			dev_kfree_skb(skb);
		}
		//spin_lock_irqsave(&ue->txurb_lock, flags);
		list_add_tail(&txurb->list, &ue->tx_urbs);
		//spin_unlock_irqrestore(&ue->txurb_lock, flags);
	}
	PDEBUG("done\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
//	tasklet_schedule(&ue->do_tx);
}
/*-------------------------------------------------------------------------*/

/* some LK 2.4 HCDs oopsed if we freed or resubmitted urbs from
 * completion callbacks.  2.5 should have fixed those bugs...
 */

static void defer_bh(struct usbnet *dev, struct sk_buff *skb, struct sk_buff_head *list)
{
	unsigned long		flags;

	spin_lock_irqsave(&list->lock, flags);
	__skb_unlink(skb, list);
	spin_unlock(&list->lock);
	spin_lock(&dev->done.lock);
	__skb_queue_tail(&dev->done, skb);
	if (dev->done.qlen == 1)
		tasklet_schedule(&dev->bh);
	spin_unlock_irqrestore(&dev->done.lock, flags);
}
/*-------------------------------------------------------------------------*/

//called from interrupt context.
static void tx_complete (struct urb *urb)
{
	unsigned long		flags;
	struct txurb *txurb = (struct txurb *) urb->context;
	struct usbnet		*dev = txurb->usn;
	struct uedev *ue = txurb->ue;
	BUG_ON(list_empty(&txurb->skbs));
	PDEBUG("entering: urb = %p status=%d\n", urb, urb->status);
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	PDEBUG("txurb = %p\n", txurb);
	//we push this txurb to be handled by our tasklet
	spin_lock_irqsave(&ue->txurb_done_lock, flags);
	list_add_tail(&txurb->list, &ue->done_tx_urbs);
	spin_unlock_irqrestore(&ue->txurb_done_lock, flags);
//	tasklet_schedule(&ue->txurb_cleanup);
	tasklet_schedule(&ue->do_tx);
	if(urb->status == 0){
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.tx_packets += txurb->nskbs;
		dev->stats.tx_bytes += urb->actual_length;
	  #else
		dev->net->stats.tx_packets += txurb->nskbs;//packet statistics
		dev->net->stats.tx_bytes += urb->actual_length;
	  #endif
		internal_stats.tx_packets += txurb->nskbs;
		internal_stats.tx_bytes += urb->actual_length;
	} else {
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.tx_errors += txurb->nskbs;
	  #else
		dev->net->stats.tx_errors += txurb->nskbs;
	  #endif
		internal_stats.tx_errors += txurb->nskbs;
		switch (urb->status) {
		case -EPIPE:
			//here we halt all usb traffic. both TX and RX. run some error handler in a process context
			usbnet_defer_kevent (dev, EVENT_TX_HALT);
			break;
		// like rx, tx gets controller i/o faults during khubd delays
		// and so it uses the same throttling mechanism.
		case -EPROTO:
		case -ETIME:
		case -EILSEQ:
			//we stop the netif queue for a while(throttle). set a timer to restart it
			if (!timer_pending (&dev->delay)) {
				mod_timer (&dev->delay,
					jiffies + THROTTLE_JIFFIES);
				if (netif_msg_link (dev))
					devdbg (dev, "tx throttle %d",
							urb->status);
			}
			PDEBUG("status = %d. HOW THE HELL DID WE GET HERE????????\n", urb->status);
			netif_stop_queue (dev->net);
			break;
		default:
			if (netif_msg_tx_err (dev))
				devdbg (dev, "tx err %d", urb->status);
		/* software-driven interface shutdown */
		case -ECONNRESET:		// async unlink
		case -ESHUTDOWN:		// hardware gone
			break;
		}
	}
	PDEBUG("done\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
}

/* initiate a transaction with the device
   this code MUST be atomic!!! it may run from a tasklet.
   this code MUST NOT be called from an interrupt as it involves data copy */
static void ue_do_tx(unsigned long param)
{
	struct uedev *ue = (struct uedev *)param;
	unsigned long flags;
	struct ue_hdr *hdr;
	struct sk_buff *skb;
	struct txurb* txurb;
	int align = 0, single_skb_xfer = 0, max_loops = 10;
	PDEBUG("entering\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	txurb_done(param);
	do{
		//lock the free urbs pool and the skb queue
		spin_lock_irqsave(&ue->tx_skbs.lock, flags);
	//	spin_lock_irqsave(&ue->txurb_lock, flags);
	//	spin_lock (&ue->tx_skbs.lock);
		//verify that we have at least one urb and one skb in order to continue
		if(ue->tx_skbs.qlen == 0 || list_empty(&ue->tx_urbs)) { //nothing to do
			PDEBUG("skbs qlen:%d, urbs:%s\n", ue->tx_skbs.qlen, list_empty(&ue->tx_urbs) ? "empty" : "available");
			spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
	//		spin_unlock (&ue->tx_skbs.lock);
	//		spin_unlock_irqrestore(&ue->txurb_lock, flags);
			break;
		}
		//detach the first urb and the first skb
		txurb = list_first_entry(&ue->tx_urbs, struct txurb, list);
		list_del(&txurb->list);
		txurb->nskbs = 0;
		txurb->len = 0;
		skb = __skb_dequeue(&ue->tx_skbs);
		ue->queued_data_len -= skb->len;
		//mark if we are sending a single skb or going for concatenation (involves data copy)
		if(ue->tx_skbs.qlen == 0)
			single_skb_xfer = 1;
		//unlock the urb pool and the skb queue
		spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
	//	spin_unlock (&ue->tx_skbs.lock);
	//	spin_unlock_irqrestore(&ue->txurb_lock, flags);
		/* state is known to this point, no races from this point on */
		
		if(single_skb_xfer == 1){
			PDEBUG("single skb xfer\n");
			list_add_tail((struct list_head*)skb, &txurb->skbs);
			usb_fill_bulk_urb (txurb->urb, txurb->usn->udev, txurb->usn->out,
							   skb->data, skb->len, tx_complete, txurb);
			txurb->len += skb->len;
			txurb->nskbs++;
			ue->single_sent++;
		} else {
			PDEBUG("%%%%%%%%%%%%%%%% multi skb xfer first skb %p\n", skb);
			memcpy(txurb->buf + txurb->len, skb->data, skb->len);
			list_add_tail((struct list_head*)skb, &txurb->skbs);
			txurb->len += skb->len;
			txurb->nskbs++;
			ue->concat_sent++;
			while(1){
				PDEBUG("ue->tx_skbs.qlen = %d\n", ue->tx_skbs.qlen);
				spin_lock_irqsave(&ue->tx_skbs.lock, flags);
				if(!ue->tx_skbs.qlen){
					PDEBUG("queue is empty, continuing to checout :-)\n");
					spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
					break;
				}
				skb = skb_peek(&ue->tx_skbs);
				PDEBUG("peeking skb %p\n", skb);
				if(txurb->len + skb->len < ue->max_tx_urb_size){
					PDEBUG("size fits - dequeueing\n");
					skb = __skb_dequeue(&ue->tx_skbs);
					ue->queued_data_len -= skb->len;
				}else{
					PDEBUG("size exceeded, continuing to checkout\n");
					spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
					break;
				}
				spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
				PDEBUG("processing skb %p\n", skb);
				memcpy(txurb->buf + txurb->len, skb->data, skb->len);
				list_add_tail((struct list_head*)skb, &txurb->skbs);
				txurb->len += skb->len;
				txurb->nskbs++;
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
			}
			usb_fill_bulk_urb (txurb->urb, txurb->usn->udev, txurb->usn->out,
							   txurb->buf, txurb->len, tx_complete, txurb);
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
		}
		//unpadding the last skb
		hdr = (struct ue_hdr *)skb->data;
		align = skb->len - (hdr->len + sizeof(*hdr));
		skb_pull(skb, align);
		txurb->len -= align;
		usb_submit_urb(txurb->urb, GFP_ATOMIC);
		//daniel 
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
		PDEBUG("%s %s : %d HZ = %d\n", __FILE__, __func__, __LINE__, HZ);
		

	}while(--max_loops);
	if(!max_loops){
		PDEBUG("exiting because of live lock!!!!!!!!!!!!!!!!!!!!!!!!\n");
		return;
	}
	spin_lock_irqsave(&ue->tx_skbs.lock, flags);
	if( !BACKPRESSURE_COND && netif_queue_stopped(ue->usn->net)){
		PDEBUG("BACKPRESSURE: wake queue\n");
		netif_wake_queue (ue->usn->net);
	}
	spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
	PDEBUG("done. qlen=%d\n", ue->tx_skbs.qlen);
}

static int ue_start_xmit (struct sk_buff *skb, struct net_device *net)
{
	unsigned long flags;
	struct usbnet		*dev = netdev_priv(net);
	struct uedev *ue = (struct uedev *)dev->driver_info->data;
	struct ue_hdr *hdr;
	int			align;
	int			retval = NET_XMIT_SUCCESS;

	PDEBUG("entering\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	//fix the packet by removing the ethernet header and replacing it with our header
	skb_pull(skb, ETH_HLEN);
	hdr = (struct ue_hdr *) skb_push(skb, sizeof(*hdr));
	hdr->reserved = 0;
	hdr->len = cpu_to_be16(skb->len - sizeof(*hdr));
	align = ALIGN(skb->len, 4) - skb->len;
	skb_put(skb, align);
	spin_lock_irqsave(&ue->tx_skbs.lock, flags);
	__skb_queue_tail (&ue->tx_skbs, skb);
	net->trans_start = jiffies;
	ue->queued_data_len += skb->len;
	if(BACKPRESSURE_COND){
	   	PDEBUG("BACKPRESSURE - stopping the queue\n");
		ue->backpressure++;
		netif_stop_queue (net);
	}
	spin_unlock_irqrestore(&ue->tx_skbs.lock, flags);
	tasklet_schedule(&ue->do_tx);
	PDEBUG("done\n");
		stamp_1 = jiffies;
		PDEBUG("%s %s : %d time = %lu\n", __FILE__, __func__, __LINE__, stamp_1);
	return retval;
}
/*-------------------------------------------------------------------------*/

static void rx_complete (struct urb *urb);

static void rx_submit (struct usbnet *dev, struct urb *urb, gfp_t flags)
{
	struct sk_buff		*skb;
	struct skb_data		*entry;
	int			retval = 0;
	unsigned long		lockflags;
	size_t			size = dev->rx_urb_size;

	if ((skb = alloc_skb (size + NET_IP_ALIGN, flags)) == NULL) {
		if (netif_msg_rx_err (dev))
			devdbg (dev, "no rx skb");
		usbnet_defer_kevent (dev, EVENT_RX_MEMORY);
		usb_free_urb (urb);
		return;
	}
	skb_reserve (skb, NET_IP_ALIGN);

	entry = (struct skb_data *) skb->cb;
	entry->urb = urb;
	entry->dev = dev;
	entry->state = rx_start;
	entry->length = 0;

	usb_fill_bulk_urb (urb, dev->udev, dev->in,
		skb->data, size, rx_complete, skb);

	spin_lock_irqsave (&dev->rxq.lock, lockflags);

	if (netif_running (dev->net)
			&& netif_device_present (dev->net)
			&& !test_bit (EVENT_RX_HALT, &dev->flags)) {
		switch (retval = usb_submit_urb (urb, GFP_ATOMIC)) {
		case -EPIPE:
			usbnet_defer_kevent (dev, EVENT_RX_HALT);
			break;
		case -ENOMEM:
			usbnet_defer_kevent (dev, EVENT_RX_MEMORY);
			break;
		case -ENODEV:
			if (netif_msg_ifdown (dev))
				devdbg (dev, "device gone");
			netif_device_detach (dev->net);
			break;
		default:
			if (netif_msg_rx_err (dev))
				devdbg (dev, "rx submit, %d", retval);
			tasklet_schedule (&dev->bh);
			break;
		case 0:
			__skb_queue_tail (&dev->rxq, skb);
		}
	} else {
		if (netif_msg_ifdown (dev))
			devdbg (dev, "rx: stopped");
		retval = -ENOLINK;
	}
	spin_unlock_irqrestore (&dev->rxq.lock, lockflags);
	if (retval) {
		dev_kfree_skb_any (skb);
		usb_free_urb (urb);
	}
}


/*-------------------------------------------------------------------------*/

static inline void rx_process (struct usbnet *dev, struct sk_buff *skb)
{
	if (dev->driver_info->rx_fixup
			&& !dev->driver_info->rx_fixup (dev, skb))
		goto error;
	// else network stack removes extra byte if we forced a short packet

	if (skb->len)
		usbnet_skb_return (dev, skb);
	else {
		if (netif_msg_rx_err (dev))
			devdbg (dev, "drop");
error:
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.rx_errors++;
	  #else
		dev->net->stats.rx_errors++;
	  #endif
		internal_stats.rx_errors++;
		skb_queue_tail (&dev->done, skb);
	}
}

/*-------------------------------------------------------------------------*/

static void rx_complete (struct urb *urb)
{
	struct sk_buff		*skb = (struct sk_buff *) urb->context;
	struct skb_data		*entry = (struct skb_data *) skb->cb;
	struct usbnet		*dev = entry->dev;
	int			urb_status = urb->status;

	skb_put (skb, urb->actual_length);
	entry->state = rx_done;
	entry->urb = NULL;

	switch (urb_status) {
	/* success */
	case 0:
		if (skb->len < dev->net->hard_header_len) {
			entry->state = rx_cleanup;
		  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
			dev->stats.rx_errors++;
			dev->stats.rx_length_errors++;
		  #else
			dev->net->stats.rx_errors++;
			dev->net->stats.rx_length_errors++;
		  #endif
			internal_stats.rx_errors++;
			internal_stats.rx_length_errors++;
			if (netif_msg_rx_err (dev))
				devdbg (dev, "rx length %d", skb->len);
		}
		break;

	/* stalls need manual reset. this is rare ... except that
	 * when going through USB 2.0 TTs, unplug appears this way.
	 * we avoid the highspeed version of the ETIMEDOUT/EILSEQ
	 * storm, recovering as needed.
	 */
	case -EPIPE:
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.rx_errors++;
	  #else
		dev->net->stats.rx_errors++;
	  #endif
		internal_stats.rx_errors++;
		usbnet_defer_kevent (dev, EVENT_RX_HALT);
		// FALLTHROUGH

	/* software-driven interface shutdown */
	case -ECONNRESET:		/* async unlink */
	case -ESHUTDOWN:		/* hardware gone */
		if (netif_msg_ifdown (dev))
			devdbg (dev, "rx shutdown, code %d", urb_status);
		goto block;

	/* we get controller i/o faults during khubd disconnect() delays.
	 * throttle down resubmits, to avoid log floods; just temporarily,
	 * so we still recover when the fault isn't a khubd delay.
	 */
	case -EPROTO:
	case -ETIME:
	case -EILSEQ:
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.rx_errors++;
	  #else
		dev->net->stats.rx_errors++;
	  #endif
		internal_stats.rx_errors++;
		if (!timer_pending (&dev->delay)) {
			mod_timer (&dev->delay, jiffies + THROTTLE_JIFFIES);
			if (netif_msg_link (dev))
				devdbg (dev, "rx throttle %d", urb_status);
		}
block:
		entry->state = rx_cleanup;
		entry->urb = urb;
		urb = NULL;
		break;

	/* data overrun ... flush fifo? */
	case -EOVERFLOW:
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.rx_over_errors++;
	  #else
		dev->net->stats.rx_over_errors++;
	  #endif
		internal_stats.rx_over_errors++;
		// FALLTHROUGH

	default:
		entry->state = rx_cleanup;
	  #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) //ctc
		dev->stats.rx_errors++;
	  #else
		dev->net->stats.rx_errors++;
	  #endif
		internal_stats.rx_errors++;
		if (netif_msg_rx_err (dev))
			devdbg (dev, "rx status %d", urb_status);
		break;
	}

	defer_bh(dev, skb, &dev->rxq);

	if (urb) {
		if (netif_running (dev->net)
				&& !test_bit (EVENT_RX_HALT, &dev->flags)) {
			rx_submit (dev, urb, GFP_ATOMIC);
			return;
		}
		usb_free_urb (urb);
	}
	if (netif_msg_rx_err (dev))
		devdbg (dev, "no read resubmitted");
}
// tasklet (work deferred from completions, in_irq) or timer

#define RX_MAX_QUEUE_MEMORY (60 * 1518)
#define	RX_QLEN(dev) (((dev)->udev->speed == USB_SPEED_HIGH) ? \
			(RX_MAX_QUEUE_MEMORY/(dev)->rx_urb_size) : 4)
static void ue_usbnet_bh (unsigned long param)
{
	struct usbnet		*dev = (struct usbnet *) param;
//	struct uedev *ue = (struct uedev *)dev->driver_info->data;
	struct sk_buff		*skb;
	struct skb_data		*entry;

	while ((skb = skb_dequeue (&dev->done))) {
		entry = (struct skb_data *) skb->cb;
		switch (entry->state) {
		case rx_done:
			entry->state = rx_cleanup;
			rx_process (dev, skb);
			continue;
		case tx_done:
		case rx_cleanup:
			usb_free_urb (entry->urb);
			dev_kfree_skb (skb);
			continue;
		default:
			devdbg (dev, "bogus skb state %d", entry->state);
		}
	}

	// waiting for all pending urbs to complete?
	if (dev->wait) {
		if ((dev->txq.qlen + dev->rxq.qlen + dev->done.qlen) == 0) {
			wake_up (dev->wait);
		}

	// or are we maybe short a few urbs?
	} else if (netif_running (dev->net)
			&& netif_device_present (dev->net)
			&& !timer_pending (&dev->delay)
			&& !test_bit (EVENT_RX_HALT, &dev->flags)) {
		int	temp = dev->rxq.qlen;
		int	qlen = RX_QLEN (dev);

		if (temp < qlen) {
			struct urb	*urb;
			int		i;

			// don't refill the queue all at once
			for (i = 0; i < 10 && dev->rxq.qlen < qlen; i++) {
				urb = usb_alloc_urb (0, GFP_ATOMIC);
				if (urb != NULL)
					rx_submit (dev, urb, GFP_ATOMIC);
			}
			if (temp != dev->rxq.qlen && netif_msg_link (dev))
				devdbg (dev, "rxqlen %d --> %d",
						temp, dev->rxq.qlen);
			if (dev->rxq.qlen < qlen)
				tasklet_schedule (&dev->bh);
		}
		//our condition to release the back pressure - override usbnet
//		if (dev->txq.qlen < TX_QLEN (dev))
//		if(ue->queued_data_len < ue->num_tx_urbs * ue->max_tx_urb_size){
//			PDEBUG("BACKPRESSURE: wake queue\n");
//			netif_wake_queue (dev->net);
//		}
	}
}


//////////////////////////////

/* called as part of the usbnet device disconnection handler */
static void ue_unbind(struct usbnet *dev, struct usb_interface *intf)
{
	struct uedev *ue = (struct uedev *)dev->driver_info->data;
	sysfs_remove_group(&ue->intf->dev.kobj, &ue_dev_attr_group);
	tasklet_kill (&ue->do_tx);
//	tasklet_kill (&ue->txurb_cleanup);
	destroy_tx_urbs(ue, ue->num_tx_urbs);
	usbnet_cdc_unbind(dev, intf);
	ue->intf = NULL;
	uetty_unbind(ue);
	PDEBUG("done\n");
	return;
}

static int ue_check_connect(struct usbnet *dev)
{
	struct uedev *ue = (struct uedev *)dev->driver_info->data;
	if(!ue){
		PDEBUG("instance not found!!! reporting link DOWN\n");
		return 0;
	}
	PDEBUG("reporting link %s\n", ue->carrier_on ? "UP" : "DOWN" );
	return ue->carrier_on;
}

static void dumpspeed(struct usbnet *dev, __le32 *speeds)
{
	if (netif_msg_timer(dev))
		devinfo(dev, "link speeds: %u kbps up, %u kbps down",
			__le32_to_cpu(speeds[0]) / 1000,
		__le32_to_cpu(speeds[1]) / 1000);
}

//this is the interrupt handler. (INT ep of the control interface.)
static void ue_status(struct usbnet *dev, struct urb *urb)
{
//	printk("====daniel==== enter cdc notification\n");
	struct usb_cdc_notification	*event;
	struct uedev *ue = (struct uedev *)dev->driver_info->data;
	if (urb->actual_length < sizeof *event)
		return;
	/* SPEED_CHANGE can get split into two 8-byte packets */
	if (test_and_clear_bit(EVENT_STS_SPLIT, &dev->flags)) {
		dumpspeed(dev, (__le32 *) urb->transfer_buffer);
		return;
	}
	event = urb->transfer_buffer;
	PDEBUG("CDC: notification %02x!\n", event->bNotificationType);
	switch (event->bNotificationType) {

	case USB_CDC_NOTIFY_NETWORK_CONNECTION:
		if (netif_msg_timer(dev))
			devdbg(dev, "CDC: carrier %s",
					event->wValue ? "on" : "off");
		if (event->wValue){
			ue->carrier_on = 1;
			netif_carrier_on(dev->net);
			netif_dormant_off(dev->net);
		}else{
			ue->carrier_on = 0;
			netif_carrier_off(dev->net);
			netif_dormant_on(dev->net);
		}
		break;

	case USB_CDC_NOTIFY_SPEED_CHANGE:	/* tx/rx rates */
		if (netif_msg_timer(dev))
			devdbg(dev, "CDC: speed change (len %d)",
					urb->actual_length);
		if (urb->actual_length != (sizeof *event + 8))
			set_bit(EVENT_STS_SPLIT, &dev->flags);
		else
			dumpspeed(dev, (__le32 *) &event[1]);
		break;

	case USB_CDC_NOTIFY_RESPONSE_AVAILABLE:
		PDEBUG("RESPONSE AVAILABLE\n");
		schedule_work(&((struct uedev *)dev->driver_info->data)->readwork);
		break;
	/* USB_CDC_NOTIFY_RESPONSE_AVAILABLE can happen too (e.g. RNDIS),
	 * but there are no standard formats for the response data.
	 */
	default:
		PDEBUG("CDC: unexpected notification %02x!",
				 event->bNotificationType);
		break;
	}
}

static int ue_bind(struct usbnet *dev, struct usb_interface *intf)
{
	int status, minor;
	struct net_device	*net = dev->net;
	struct uedev *ue;
	struct cdc_state		*info = (void *) &dev->data;

	PDEBUG("entering: TX_QLEN=%d\n", TX_QLEN(dev));
    // look for free context
    for (minor = 0; minor < UE_DEVS && uedevs_table[minor]; minor++);
    if (minor == UE_DEVS)
        return -ENODEV;
	status = usbnet_generic_cdc_bind(dev, intf);
	if (status < 0){
		dev_err(&intf->dev, "GENERIC CDC BIND FAILED\n");
		PDEBUG("generic cdc bind failed, status = %d\n", status);
		return status;
	}
	// get our mac address as reported by the cdc descriptor
	status = usbnet_get_ethernet_addr(dev, info->ether->iMACAddress);
	if (status < 0) {
		goto err_macaddr;
	}
	PDEBUG("after generic bind\n");
	//modify the usbnet struct
	dev->rx_urb_size = RX_URB_BUFSIZE;
	dev->hard_mtu = net->mtu + net->hard_header_len;

	dev->maxpacket = usb_maxpacket(dev->udev, dev->out, 1);
	if (dev->maxpacket == 0) {
		if (netif_msg_probe(dev))
			dev_dbg(&intf->dev, "dev->maxpacket can't be 0\n");
		status = -EINVAL;
		goto err_maxpacket;
	}

	net->features = NETIF_F_VLAN_CHALLENGED;
    // report us as NO arp interface.
	net->flags = IFF_NOARP & (~IFF_BROADCAST & ~IFF_MULTICAST);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	net->netdev_ops = &ue_netdev_ops;
#else
	net->hard_start_xmit = ue_start_xmit;
#endif

	ue = kzalloc(sizeof(struct uedev), GFP_KERNEL);
	if(!ue){
		dev_err(&intf->dev, "cannot allocate device data\n");
		status = -ENOMEM;
		goto err_mem;
	}
	dev->bh.func = ue_usbnet_bh;
	ue->usn = dev;
	ue->num_tx_urbs = DEFAULT_NUM_TX_URBS;
	memcpy(&ue->ue_dinfo, &ue_info, sizeof(ue_info));
	ue->ue_dinfo.data = (unsigned long)ue;
	dev->driver_info = &ue->ue_dinfo;
    
    // copy the fake ethernet header
	memcpy(ue->rx_ethhdr, s_rxhdr, 14);
	// handle "our" mac address 
	memcpy(ue->rx_ethhdr, net->dev_addr, 6);
	// the peer mac address may be changed at later time via sysfs.

	netif_carrier_off(dev->net);
	netif_dormant_on(dev->net);
    // make room for alignment
	// daniel  2010/12/03 09:03  comment it now, need to fix it
	//net->needed_tailroom = 4;
	//modify the if name to lte%d
	strcpy(net->name,"lte%d");
	ue->minor = minor;
	skb_queue_head_init (&ue->tx_skbs);
	INIT_LIST_HEAD(&ue->tx_urbs);
	INIT_LIST_HEAD(&ue->done_tx_urbs);
	spin_lock_init(&ue->txurb_lock);
	spin_lock_init(&ue->txurb_done_lock);
	ue->upool = NULL;
	ue->intf = intf;
	uetty_bind(ue);
	if((status = init_tx_urbs(dev, ue->num_tx_urbs)) != 0){
		goto err_tx_urbs;
	}
	ue->do_tx.func = ue_do_tx;
	ue->do_tx.data = (unsigned long) ue;    //recordit!

//	ue->txurb_cleanup.func = txurb_done;
//	ue->txurb_cleanup.data = (unsigned long) ue;    //recordit!
	ue->max_tx_urb_size = DEFAULT_TX_URB_SIZE;
    uedevs_table[minor] = ue;
	status = sysfs_create_group(&ue->intf->dev.kobj, &ue_dev_attr_group);
	if (status < 0) {
		dev_err(&dev->udev->dev, "cannot setup sysfs: %d\n", status);
		goto err_sysfs;
	}
	PDEBUG("associated struct device is %p\n", &dev->udev->dev);
	PDEBUG("associated struct usb_device is %p\n", dev->udev);
	PDEBUG("associated struct interface is , %p\n",intf );
	PDEBUG("associated struct ue is %p\n", ue);
	PDEBUG("done\n");
	return 0;
err_sysfs:
	destroy_tx_urbs(ue, ue->num_tx_urbs);
err_tx_urbs:
	kfree(ue);
	uedevs_table[minor] = NULL;
err_mem:
err_maxpacket:
err_macaddr:
	usbnet_cdc_unbind(dev, intf);
	PDEBUG("done - status = %d\n", status);
	return status;
}


static int __init ue_init(void)
{
	int ret = 0;
	BUILD_BUG_ON((sizeof(((struct usbnet *)0)->data)
			< sizeof(struct cdc_state)));

// daniel  2011/04/20 11:34  
	//struct proc_dir_entry *lte_dir, *time_file;
	//lte_dir = proc_mkdir("lte", NULL);
	//time_file = create_proc_entry("time_stamps", 0644, lte_dir);

	
	
	
	ret = uetty_init();
	if (ret)
		return ret;
 	return usb_register(&ue_driver);
}
module_init(ue_init);

static void __exit ue_exit(void)
{
    usb_deregister(&ue_driver);
	uetty_exit();
}
module_exit(ue_exit);
