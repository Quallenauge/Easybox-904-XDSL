/*
 * u_ether.c -- Ethernet-over-USB link layer utilities for Gadget stack
 *
 * Copyright (C) 2003-2005,2008 David Brownell
 * Copyright (C) 2003-2004 Robert Schwebel, Benedikt Spranger
 * Copyright (C) 2008 Nokia Corporation
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* #define VERBOSE_DEBUG */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/ctype.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>

#include "u_ether.h"


#if defined(__IFX_USB_GADGET__ )
	static void *gadget_alloc_buffer(unsigned _bytes)
	{
		void *buf;
		size_t size;
		u32 *cached,*uncached;
		u32 totalsize,page;

		/* Check dword alignment */
 		if(!_bytes)
			return NULL;

		if ((_bytes & 0x3UL) != 0)
			printk(KERN_INFO "%s() Buffer size is not a multiple of DWORD size (%d)",__func__, _bytes);

		size=(_bytes+3)&0xFFFFFFFC;
		totalsize=size + 12;
		page=get_order(totalsize);

		cached = (void *) __get_free_pages(( GFP_ATOMIC | GFP_DMA), page);
		if(!cached)
		{
			printk(KERN_INFO "%s Allocation Failed size:%d\n",__func__,size);
			return NULL;
		}
		uncached = (u32 *)(KSEG1ADDR(cached));
		memset(uncached, 0, totalsize);
		*(uncached+0)=totalsize;
		*(uncached+1)=page;
		*(uncached+2)=(u32)cached;
		buf = (void *)(uncached+3);
		return buf;
	}

	static void gadget_free_buffer(void *_buf)
	{
		u32 totalsize,page;
		u32 *cached,*uncached;

		if(_buf != NULL)
		{
			uncached=_buf;
			uncached-=3;
			totalsize=*(uncached+0);
			page=*(uncached+1);
			cached=(u32 *)(*(uncached+2));
			if(totalsize && page==get_order(totalsize) && cached==(u32 *)(KSEG0ADDR(uncached)))
				free_pages((unsigned long)cached, page);
		}
		return;
	}
#endif

/*
 * This component encapsulates the Ethernet link glue needed to provide
 * one (!) network link through the USB gadget stack, normally "usb0".
 *
 * The control and data models are handled by the function driver which
 * connects to this code; such as CDC Ethernet (ECM or EEM),
 * "CDC Subset", or RNDIS.  That includes all descriptor and endpoint
 * management.
 *
 * Link level addressing is handled by this component using module
 * parameters; if no such parameters are provided, random link level
 * addresses are used.  Each end of the link uses one address.  The
 * host end address is exported in various ways, and is often recorded
 * in configuration databases.
 *
 * The driver which assembles each configuration using such a link is
 * responsible for ensuring that each configuration includes at most one
 * instance of is network link.  (The network layer provides ways for
 * this single "physical" link to be used by multiple virtual links.)
 */

#define UETH__VERSION	"29-May-2008"

struct eth_dev {
	/* lock is held while accessing port_usb
	 * or updating its backlink port_usb->ioport
	 */
	spinlock_t		lock;
	struct gether		*port_usb;

	struct net_device	*net;
	struct usb_gadget	*gadget;

	spinlock_t		req_lock;	/* guard {rx,tx}_reqs */
	struct list_head	tx_reqs, rx_reqs;
	atomic_t		tx_qlen;

	struct sk_buff_head	rx_frames;

	unsigned		header_len;
	#if defined(__IFX_USB_GADGET__) && defined(__RETAIN_BUF_TX__)
		int (*wrap)(struct gether *port,
						u8 *buf,
						struct sk_buff *skb,
						int max_len);
	#else
		struct sk_buff *(*wrap)(struct gether *, struct sk_buff *skb);
	#endif

	#if defined(__IFX_USB_GADGET__) && defined(__RETAIN_BUF_RX__)
		int		(*unwrap)(	struct gether *port,
							u8 *buf, u16 len,
							struct sk_buff_head *list);
	#else
		int		(*unwrap)(struct gether *,
						struct sk_buff *skb,
						struct sk_buff_head *list);
	#endif

	struct work_struct	work;

	unsigned long		todo;
#define	WORK_RX_MEMORY		0

	bool			zlp;
	u8			host_mac[ETH_ALEN];

	#if defined(__IFX_USB_GADGET__ )
		int                      tx_len;
		int                      tx_wml;
		int                      tx_wmh;
		int                      gfp_flags;
	#endif
	#ifdef __MAC_ECM_FIX__
		int                  ecm_only;
			int                  ecm_only_postive;
			int                  ecm_only_negtive;
				#define ecm_only_postive_max 3
				#define ecm_only_negtive_max 5
	#endif
};

/*-------------------------------------------------------------------------*/

#define RX_EXTRA	20	/* bytes guarding against rx overflows */

#if defined(__IFX_USB_GADGET__ )
	#define DEFAULT_QLEN_TX_FS 10
	#define DEFAULT_QLEN_TX_HS 10
	#define DEFAULT_QLEN_RX_FS 20
	#define DEFAULT_QLEN_RX_HS 10

	static ushort qlen_tx_fs=DEFAULT_QLEN_TX_FS;
	module_param(qlen_tx_fs, ushort, S_IRUGO);
	MODULE_PARM_DESC(qlen_tx_fs, "QLen fot FS Tx");
	static ushort qlen_rx_fs=DEFAULT_QLEN_RX_FS;
	module_param(qlen_rx_fs, ushort, S_IRUGO);
	MODULE_PARM_DESC(qlen_rx_fs, "QLen fot FS Rx");

	#ifdef CONFIG_USB_GADGET_DUALSPEED
		static ushort qlen_tx_hs=DEFAULT_QLEN_TX_HS;
		module_param(qlen_tx_hs, ushort, S_IRUGO);
		MODULE_PARM_DESC(qlen_tx_hs, "QLen fot HS Tx");
		#define qlen_tx(gadget) ((gadget->speed == USB_SPEED_HIGH) ? qlen_tx_hs : qlen_tx_fs)
		static ushort qlen_rx_hs=DEFAULT_QLEN_RX_HS;
		module_param(qlen_rx_hs, ushort, S_IRUGO);
		MODULE_PARM_DESC(qlen_rx_hs, "QLen fot HS Rx");
		#define qlen_rx(gadget) ((gadget->speed == USB_SPEED_HIGH) ? qlen_rx_hs : qlen_rx_fs)
	#else	/* full speed (low speed doesn't do bulk) */
		#define qlen_tx(gadget) (qlen_tx_fs)
		#define qlen_rx(gadget) (qlen_rx_fs)
	#endif
#else
	#define DEFAULT_QLEN	2	/* double buffering by default */

		#ifdef CONFIG_USB_GADGET_DUALSPEED
			static unsigned qmult = 5;
			module_param(qmult, uint, S_IRUGO|S_IWUSR);
			MODULE_PARM_DESC(qmult, "queue length multiplier at high speed");
		#else	/* full speed (low speed doesn't do bulk) */
			#define qmult		1
		#endif
		/* for dual-speed hardware, use deeper queues at highspeed */
		static inline int qlen(struct usb_gadget *gadget)
		{
			if (gadget_is_dualspeed(gadget) && gadget->speed == USB_SPEED_HIGH)
				return qmult * DEFAULT_QLEN;
			else
				return DEFAULT_QLEN;
		}
#endif

/*-------------------------------------------------------------------------*/

/* REVISIT there must be a better way than having two sets
 * of debug calls ...
 */

#undef DBG
#undef VDBG
#undef ERROR
#undef INFO

#define xprintk(d, level, fmt, args...) \
	printk(level "%s: " fmt , (d)->net->name , ## args)

#ifdef DEBUG
#undef DEBUG
#define DBG(dev, fmt, args...) \
	xprintk(dev , KERN_DEBUG , fmt , ## args)
#else
#define DBG(dev, fmt, args...) \
	do { } while (0)
#endif /* DEBUG */

#ifdef VERBOSE_DEBUG
#define VDBG	DBG
#else
#define VDBG(dev, fmt, args...) \
	do { } while (0)
#endif /* DEBUG */

#define ERROR(dev, fmt, args...) \
	xprintk(dev , KERN_ERR , fmt , ## args)
#define INFO(dev, fmt, args...) \
	xprintk(dev , KERN_INFO , fmt , ## args)

/*-------------------------------------------------------------------------*/

/* NETWORK DRIVER HOOKUP (to the layer above this driver) */

static int ueth_change_mtu(struct net_device *net, int new_mtu)
{
	struct eth_dev	*dev = netdev_priv(net);
	unsigned long	flags;
	int		status = 0;

	/* don't change MTU on "live" link (peer won't know) */
	spin_lock_irqsave(&dev->lock, flags);
	if (dev->port_usb)
		status = -EBUSY;
	else if (new_mtu <= ETH_HLEN || new_mtu > ETH_FRAME_LEN)
		status = -ERANGE;
	else
		net->mtu = new_mtu;
	spin_unlock_irqrestore(&dev->lock, flags);

	return status;
}

static void eth_get_drvinfo(struct net_device *net, struct ethtool_drvinfo *p)
{
	struct eth_dev	*dev = netdev_priv(net);

	strlcpy(p->driver, "g_ether", sizeof p->driver);
	strlcpy(p->version, UETH__VERSION, sizeof p->version);
	strlcpy(p->fw_version, dev->gadget->name, sizeof p->fw_version);
	strlcpy(p->bus_info, dev_name(&dev->gadget->dev), sizeof p->bus_info);
}

/* REVISIT can also support:
 *   - WOL (by tracking suspends and issuing remote wakeup)
 *   - msglevel (implies updated messaging)
 *   - ... probably more ethtool ops
 */

static const struct ethtool_ops ops = {
	.get_drvinfo = eth_get_drvinfo,
	.get_link = ethtool_op_get_link,
};

static void defer_kevent(struct eth_dev *dev, int flag)
{
	if (test_and_set_bit(flag, &dev->todo))
		return;
	if (!schedule_work(&dev->work))
		ERROR(dev, "kevent %d may have been dropped\n", flag);
	else
		DBG(dev, "kevent %d scheduled\n", flag);
}

static void rx_complete(struct usb_ep *ep, struct usb_request *req);

static int
rx_submit(struct eth_dev *dev, struct usb_request *req, gfp_t gfp_flags)
{
	#if !defined(__IFX_USB_GADGET__) || !defined(__RETAIN_BUF_RX__)
		struct sk_buff	*skb;
	#endif
	int		retval = -ENOMEM;
	size_t		size = 0;
	struct usb_ep	*out;
	#if !defined(__IFX_USB_GADGET__)
		unsigned long	flags;
	#endif

	#if defined(__IFX_USB_GADGET__)
		spin_lock(&dev->req_lock);
	#else
		spin_lock_irqsave(&dev->lock, flags);
	#endif
	if (dev->port_usb)
		out = dev->port_usb->out_ep;
	else
		out = NULL;
	#if defined(__IFX_USB_GADGET__)
		spin_unlock(&dev->req_lock);
	#else
		spin_unlock_irqrestore(&dev->lock, flags);
	#endif

	if (!out)
		return -ENOTCONN;

	#if !defined(__IFX_USB_GADGET__) || !defined(__RETAIN_BUF_RX__)
		/* Padding up to RX_EXTRA handles minor disagreements with host.
		 * Normally we use the USB "terminate on short read" convention;
		 * so allow up to (N*maxpacket), since that memory is normally
		 * already allocated.  Some hardware doesn't deal well with short
		 * reads (e.g. DMA must be N*maxpacket), so for now don't trim a
		 * byte off the end (to force hardware errors on overflow).
		 *
		 * RNDIS uses internal framing, and explicitly allows senders to
		 * pad to end-of-packet.  That's potentially nice for speed, but
		 * means receivers can't recover lost synch on their own (because
		 * new packets don't only start after a short RX).
		 */
		size += sizeof(struct ethhdr) + dev->net->mtu + RX_EXTRA;
		size += dev->port_usb->header_len;
		size += out->maxpacket - 1;
		size -= size % out->maxpacket;

		skb = alloc_skb(size + NET_IP_ALIGN, gfp_flags);
		if (skb == NULL) {
			DBG(dev, "no rx skb\n");
			goto enomem;
		}

		/* Some platforms perform better when IP packets are aligned,
		 * but on at least one, checksumming fails otherwise.  Note:
		 * RNDIS headers involve variable numbers of LE32 values.
		 */
		skb_reserve(skb, NET_IP_ALIGN);

		req->buf = skb->data;
		req->length = size;
		req->complete = rx_complete;
		req->context = skb;

		retval = usb_ep_queue(out, req, gfp_flags);
		if (retval == -ENOMEM)
	enomem:
			defer_kevent(dev, WORK_RX_MEMORY);
		if (retval) {
			DBG(dev, "rx submit --> %d\n", retval);
			if (skb)
				#if defined(__IFX_USB_GADGET__)
					free_skb("rx_submit","fail",skb,0);
				#else
					dev_kfree_skb_any(skb);
				#endif
			spin_lock_irqsave(&dev->req_lock, flags);
			list_add(&req->list, &dev->rx_reqs);
			spin_unlock_irqrestore(&dev->req_lock, flags);
		}
		return retval;
	#else
		dev->gfp_flags=gfp_flags;
		if (!req->buf)
		{
			printk(KERN_INFO "%s() %d ERROR Invalid Rx.Buf\n",__func__, __LINE__);
			req->length=0;
 			req->context = NULL;
			spin_lock(&dev->req_lock);
			list_add(&req->list, &dev->rx_reqs);
			spin_unlock(&dev->req_lock);
			return 0;
		}
		else
		{
			req->length   = alloc_size_rx;
			req->complete = rx_complete;
			retval = usb_ep_queue(out, req, gfp_flags);
			return retval;
		}
	#endif
}

static void rx_complete(struct usb_ep *ep, struct usb_request *req)
{
	#if !defined(__IFX_USB_GADGET__) || !defined(__RETAIN_BUF_RX__)
		struct sk_buff	*skb = req->context, *skb2;
		struct eth_dev	*dev = ep->driver_data;
		int		status = req->status;

		switch (status) {

		/* normal completion */
		case 0:
			skb_put(skb, req->actual);

			if (dev->unwrap) {
				unsigned long	flags;

				spin_lock_irqsave(&dev->lock, flags);
				if (dev->port_usb) {
					status = dev->unwrap(dev->port_usb,
								skb,
								&dev->rx_frames);
				} else {
					#if defined(__IFX_USB_GADGET__)
						free_skb("rx_complete","NOTCONN",skb,0);
					#else
						dev_kfree_skb_any(skb);
					#endif
					status = -ENOTCONN;
				}
				spin_unlock_irqrestore(&dev->lock, flags);
			} else {
				skb_queue_tail(&dev->rx_frames, skb);
			}
			skb = NULL;

			skb2 = skb_dequeue(&dev->rx_frames);
			while (skb2) {
				if (status < 0
						|| ETH_HLEN > skb2->len
						|| skb2->len > ETH_FRAME_LEN) {
					dev->net->stats.rx_errors++;
					dev->net->stats.rx_length_errors++;
					DBG(dev, "rx length %d\n", skb2->len);
					#if defined(__IFX_USB_GADGET__)
						free_skb("rx_submit","netif_rx_fail",skb2,0);
					#else
						dev_kfree_skb_any(skb2);
					#endif
					goto next_frame;
				}
				skb2->protocol = eth_type_trans(skb2, dev->net);
				dev->net->stats.rx_packets++;
				dev->net->stats.rx_bytes += skb2->len;

				/* no buffer copies needed, unless hardware can't
				 * use skb buffers.
				 */
				status = netif_rx(skb2);
	next_frame:
				skb2 = skb_dequeue(&dev->rx_frames);
			}
			break;

		/* software-driven interface shutdown */
		case -ECONNRESET:		/* unlink */
		case -ESHUTDOWN:		/* disconnect etc */
			VDBG(dev, "rx shutdown, code %d\n", status);
			goto quiesce;

		/* for hardware automagic (such as pxa) */
		case -ECONNABORTED:		/* endpoint reset */
			DBG(dev, "rx %s reset\n", ep->name);
			defer_kevent(dev, WORK_RX_MEMORY);
	quiesce:
			#if defined(__IFX_USB_GADGET__)
				free_skb("rx_submit","CONNABORTED",skb,0);
			#else
				dev_kfree_skb_any(skb);
			#endif
			goto clean;

		/* data overrun */
		case -EOVERFLOW:
			dev->net->stats.rx_over_errors++;
			/* FALLTHROUGH */

		default:
			dev->net->stats.rx_errors++;
			DBG(dev, "rx status %d\n", status);
			break;
		}

		if (skb)
			#if defined(__IFX_USB_GADGET__)
				free_skb("rx_submit","skb remain",skb,0);
			#else
				dev_kfree_skb_any(skb);
			#endif
		if (!netif_running(dev->net)) {
	clean:
			spin_lock(&dev->req_lock);
			list_add(&req->list, &dev->rx_reqs);
			spin_unlock(&dev->req_lock);
			req = NULL;
		}
		if (req)
			rx_submit(dev, req, GFP_ATOMIC);
	#else
		struct eth_dev	*dev = ep->driver_data;
		int		status = req->status;
		struct  sk_buff *skb2;

		switch (status)
		{
			case 0:
				if (!dev->port_usb)
					status = -ENOTCONN;
				else if (dev->unwrap)
				{
					unsigned long	flags;
					spin_lock_irqsave(&dev->lock, flags);
					status = dev->unwrap(dev->port_usb,
								req->buf,req->actual,
								&dev->rx_frames);
					spin_unlock_irqrestore(&dev->lock, flags);
				}
				else
					status = -EINVAL;

				skb2 = skb_dequeue(&dev->rx_frames);
				while (skb2)
				{
					if (status < 0
							|| ETH_HLEN > skb2->len
							|| skb2->len > ETH_FRAME_LEN)
					{
						dev->net->stats.rx_errors++;
						dev->net->stats.rx_length_errors++;
						DBG(dev, "rx length %d\n", skb2->len);
						free_skb("rx_submit","netif_rx_fail",skb2,0);
					}
					else
					{
						skb2->protocol = eth_type_trans(skb2, dev->net);
						dev->net->stats.rx_packets++;
						dev->net->stats.rx_bytes += skb2->len;
						status = netif_rx(skb2);
					}
					skb2 = skb_dequeue(&dev->rx_frames);
			}
				break;
			/* software-driven interface shutdown */
			case -ECONNRESET:		/* unlink */
 			case -ESHUTDOWN:		/* disconnect etc */
				VDBG(dev, "rx shutdown, code %d\n", status);
				goto clean;
			case -ECONNABORTED:		/* endpoint reset */
				DBG(dev, "rx %s reset\n", ep->name);
				defer_kevent(dev, WORK_RX_MEMORY);
				goto clean;
			/* data overrun */
			case -EOVERFLOW:
				dev->net->stats.rx_over_errors++;
				/* FALLTHROUGH */
			default:
				dev->net->stats.rx_errors++;
				DBG(dev, "rx status %d\n", status);
				break;
		}
		if (netif_running(dev->net) && req)
		{
			rx_submit(dev, req, GFP_ATOMIC);
			req = NULL;
		}
	clean:
		if (req)
		{
			spin_lock(&dev->req_lock);
			list_add(&req->list, &dev->rx_reqs);
			spin_unlock(&dev->req_lock);
		}
	#endif
}

#if defined(__IFX_USB_GADGET__) && (defined(__RETAIN_BUF_TX__) || defined(__RETAIN_BUF_RX__))
	static int prealloc(struct list_head *list, struct usb_ep *ep, unsigned n,int bufsiz)
#else
	static int prealloc(struct list_head *list, struct usb_ep *ep, unsigned n)
#endif
{
	unsigned		i;
	struct usb_request	*req;

	if (!n)
		return -ENOMEM;

	/* queue/recycle up to N requests */
	i = n;
	list_for_each_entry(req, list, list) {
		if (i-- == 0)
			goto extra;
	}

	while (i--)
	{
		req = usb_ep_alloc_request(ep, GFP_ATOMIC);
		if (!req)
			return list_empty(list) ? -ENOMEM : 0;

		req->buf=NULL;
		#if defined(__IFX_USB_GADGET__) && (defined(__RETAIN_BUF_TX__) || defined(__RETAIN_BUF_RX__))
			if(bufsiz>0)
				req->buf=gadget_alloc_buffer(bufsiz);
		#endif
		list_add(&req->list, list);
	}
	return 0;

extra:
	/* free extras */
	for (;;) {
		struct list_head	*next;

		next = req->list.next;
		list_del(&req->list);
		#if defined(__IFX_USB_GADGET__)
			if(!req->context && req->buf)
				gadget_free_buffer(req->buf);
		#endif
		usb_ep_free_request (ep, req);

		if (next == list)
			break;

		req = container_of(next, struct usb_request, list);
	}
	return 0;
}

#if defined(__IFX_USB_GADGET__)
	static int alloc_requests(struct eth_dev *dev, struct gether *link, unsigned txn, unsigned rxn)
#else
	static int alloc_requests(struct eth_dev *dev, struct gether *link, unsigned n)
#endif
{
	int	status;

	spin_lock(&dev->req_lock);

	#if defined(__IFX_USB_GADGET__ )
		dev->tx_len = txn;
		dev->tx_wml = txn /4-1;
		if(dev->tx_wml<1) dev->tx_wml=1;
		dev->tx_wmh = txn *3/4+1;
		if(dev->tx_wmh>dev->tx_len-1) dev->tx_wmh=dev->tx_len-1;
 		#if defined(__RETAIN_BUF_TX__) || defined(__RETAIN_BUF_RX__)
			status = prealloc (&dev->tx_reqs, link->in_ep, txn,alloc_size_tx);
		#else
			status = prealloc (&dev->tx_reqs, link->in_ep, txn);
		#endif
	#else
		status = prealloc(&dev->tx_reqs, link->in_ep, n);
	#endif

	if (status < 0)
		goto fail;

	#if defined(__IFX_USB_GADGET__ )
 		#if defined(__RETAIN_BUF_TX__) || defined(__RETAIN_BUF_RX__)
			status = prealloc (&dev->rx_reqs, link->out_ep, rxn,alloc_size_rx);
		#else
			status = prealloc (&dev->rx_reqs, link->out_ep, rxn);
		#endif
	#else
		status = prealloc(&dev->rx_reqs, link->out_ep, n);
	#endif

	if (status < 0)
		goto fail;
	goto done;
fail:
	DBG(dev, "can't alloc requests\n");
done:
	spin_unlock(&dev->req_lock);
	return status;
}

static void rx_fill(struct eth_dev *dev, gfp_t gfp_flags)
{
	struct usb_request	*req;
	unsigned long		flags;

	/* fill unused rxq slots with some skb */
	spin_lock_irqsave(&dev->req_lock, flags);
	while (!list_empty(&dev->rx_reqs)) {
		req = container_of(dev->rx_reqs.next,
				struct usb_request, list);
		list_del_init(&req->list);
		spin_unlock_irqrestore(&dev->req_lock, flags);

		if (rx_submit(dev, req, gfp_flags) < 0) {
			defer_kevent(dev, WORK_RX_MEMORY);
			return;
		}

		spin_lock_irqsave(&dev->req_lock, flags);
	}
	spin_unlock_irqrestore(&dev->req_lock, flags);
}

static void eth_work(struct work_struct *work)
{
	struct eth_dev	*dev = container_of(work, struct eth_dev, work);

	if (test_and_clear_bit(WORK_RX_MEMORY, &dev->todo)) {
		if (netif_running(dev->net))
			rx_fill(dev, GFP_KERNEL);
	}

	if (dev->todo)
		DBG(dev, "work done, flags = 0x%lx\n", dev->todo);
}

static void tx_complete(struct usb_ep *ep, struct usb_request *req)
{
	#if !defined(__IFX_USB_GADGET__) || !defined(__RETAIN_BUF_TX__)
		struct sk_buff	*skb = req->context;
	#endif
	struct eth_dev	*dev = ep->driver_data;

	switch (req->status) {
	default:
		dev->net->stats.tx_errors++;
		VDBG(dev, "tx err %d\n", req->status);
		/* FALLTHROUGH */
	case -ECONNRESET:		/* unlink */
	case -ESHUTDOWN:		/* disconnect etc */
		break;
	case 0:
		#if !defined(__IFX_USB_GADGET__) || !defined(__RETAIN_BUF_TX__)
			dev->net->stats.tx_bytes += skb->len;
		#else
			dev->net->stats.tx_bytes += req->length;
		#endif
	}
	dev->net->stats.tx_packets++;

	spin_lock(&dev->req_lock);
	#if defined(__IFX_USB_GADGET__)
		dev->tx_len++;
	#endif
	list_add(&req->list, &dev->tx_reqs);
	spin_unlock(&dev->req_lock);

	#if defined(__IFX_USB_GADGET__)
		#if !defined(__RETAIN_BUF_TX__)
			free_skb("tx_complete","OK",skb,0);
		#endif
		req->context=NULL;
		req->length=0;
	#else
		dev_kfree_skb_any(skb);
 	#endif

	atomic_dec(&dev->tx_qlen);
	if (netif_carrier_ok(dev->net))
		netif_wake_queue(dev->net);
}

static inline int is_promisc(u16 cdc_filter)
{
	return cdc_filter & USB_CDC_PACKET_TYPE_PROMISCUOUS;
}

static netdev_tx_t eth_start_xmit(struct sk_buff *skb,
					struct net_device *net)
{
	struct eth_dev		*dev = netdev_priv(net);
	int			length = skb->len;
	int			retval;
	struct usb_request	*req = NULL;
	unsigned long		flags;
	struct usb_ep		*in;
	u16			cdc_filter;

	spin_lock_irqsave(&dev->lock, flags);
	if (dev->port_usb) {
		in = dev->port_usb->in_ep;
		cdc_filter = dev->port_usb->cdc_filter;
	} else {
		in = NULL;
		cdc_filter = 0;
	}
	spin_unlock_irqrestore(&dev->lock, flags);

	if (!in) {
		#if defined(__IFX_USB_GADGET__)
			free_skb("eth_start_xmit","port_off",skb,0);
		#else
			dev_kfree_skb_any(skb);
		#endif
		return NETDEV_TX_OK;
	}

	#if defined(__IFX_USB_GADGET__)
		if (list_empty (&dev->tx_reqs))
		{
			//dev->stats.tx_dropped++;
			netif_stop_queue (net);
			free_skb("eth_start_xmit","empty queue",skb,0);
			printk(KERN_INFO "%s() %d QUEUE STOPPED\n",__func__,__LINE__);
			return 0;
		}
	#endif

	/* apply outgoing CDC or RNDIS filters */
	if (!is_promisc(cdc_filter)) {
		u8		*dest = skb->data;

		if (is_multicast_ether_addr(dest)) {
			u16	type;

			/* ignores USB_CDC_PACKET_TYPE_MULTICAST and host
			 * SET_ETHERNET_MULTICAST_FILTERS requests
			 */
			if (is_broadcast_ether_addr(dest))
				type = USB_CDC_PACKET_TYPE_BROADCAST;
			else
				type = USB_CDC_PACKET_TYPE_ALL_MULTICAST;
			if (!(cdc_filter & type))
			{
				#if defined(__IFX_USB_GADGET__)
					//dev->stats.tx_dropped++;
					free_skb("eth_start_xmit","filted",skb,0);
				#else
					dev_kfree_skb_any (skb);
				#endif
				return NETDEV_TX_OK;
			}
		}
		/* ignores USB_CDC_PACKET_TYPE_DIRECTED */
	}

	spin_lock_irqsave(&dev->req_lock, flags);
	/*
	 * this freelist can be empty if an interrupt triggered disconnect()
	 * and reconfigured the gadget (shutting down this queue) after the
	 * network stack decided to xmit but before we got the spinlock.
	 */
	if (list_empty(&dev->tx_reqs)) {
		spin_unlock_irqrestore(&dev->req_lock, flags);
		return NETDEV_TX_BUSY;
	}

	req = container_of(dev->tx_reqs.next, struct usb_request, list);
	list_del(&req->list);

	/* temporarily stop TX queue when the freelist empties */

	#if defined(__IFX_USB_GADGET__)
		dev->tx_len--;
		if (dev->tx_len < dev->tx_wml || list_empty (&dev->tx_reqs))
	#else
		if (list_empty (&dev->tx_reqs))
	#endif
			netif_stop_queue (net);

	spin_unlock_irqrestore(&dev->req_lock, flags);

	/* no buffer copies needed, unless the network stack did it
	 * or the hardware can't use skb buffers.
	 * or there's not enough space for extra headers we need
	 */
	#ifdef __IFX_USB_GADGET__
		if (!dev->port_usb)
			goto drop;
	#endif

	#ifdef __IFX_USB_GADGET__
		if (dev->wrap)
		{
			unsigned long	flags;

			#ifdef __RETAIN_BUF_TX__
				spin_lock_irqsave(&dev->lock, flags);
				length = dev->wrap(dev->port_usb,req->buf, skb,alloc_size_tx);
				spin_unlock_irqrestore(&dev->lock, flags);
				if(length < 0)
					goto drop;
				free_skb("eth_start_xmit","copy and free",skb,0);
				skb=NULL;
			#else
				spin_lock_irqsave(&dev->lock, flags);
				skb = dev->wrap(dev->port_usb, skb);
				spin_unlock_irqrestore(&dev->lock, flags);
				if (!skb)
					goto drop;
				length = skb->len;
				req->buf = skb->data;
				req->context = skb;
			#endif
		}
		else
		{
			#ifdef __RETAIN_BUF_TX__
				goto drop;
			#else
				req->buf = skb->data;
				req->context = skb;
			#endif
		}
	#else
		if (dev->wrap) {
			unsigned long	flags;

			spin_lock_irqsave(&dev->lock, flags);
			if (dev->port_usb)
				skb = dev->wrap(dev->port_usb, skb);
			spin_unlock_irqrestore(&dev->lock, flags);
			if (!skb)
				goto drop;

			length = skb->len;
		}
		req->buf = skb->data;
		req->context = skb;
	#endif
	req->complete = tx_complete;

	/* use zlp framing on tx for strict CDC-Ether conformance,
	 * though any robust network rx path ignores extra padding.
	 * and some hardware doesn't like to write zlps.
	 */
	req->zero = 1;
	if (!dev->zlp && (length % in->maxpacket) == 0)
		length++;

	req->length = length;

	#if defined(__IFX_USB_GADGET__)
		req->no_interrupt = 0;
	#else
		/* throttle highspeed IRQ rate back slightly */
		if (gadget_is_dualspeed(dev->gadget))
			req->no_interrupt = (dev->gadget->speed == USB_SPEED_HIGH)
				? ((atomic_read(&dev->tx_qlen) % qmult) != 0)
				: 0;
	#endif

	retval = usb_ep_queue(in, req, GFP_ATOMIC);
	switch (retval) {
	default:
		DBG(dev, "tx queue err %d\n", retval);
		break;
	case 0:
		net->trans_start = jiffies;
		atomic_inc(&dev->tx_qlen);
	}

	if (retval) {
drop:
		if(skb)
			#if defined(__IFX_USB_GADGET__)
				free_skb("eth_start_xmit","dropped",skb,0);
			#else
				dev_kfree_skb_any (skb);
			#endif
		dev->net->stats.tx_dropped++;

		spin_lock_irqsave(&dev->req_lock, flags);
		if (list_empty(&dev->tx_reqs))
			netif_start_queue(net);
		list_add(&req->list, &dev->tx_reqs);
		#if defined(__IFX_USB_GADGET__)
			dev->tx_len++;
			if (netif_running (dev->net) && netif_carrier_ok (dev->net) && dev->tx_len >= dev->tx_wmh)
			{
				if(netif_queue_stopped(net))
					netif_wake_queue (net);
			}
		#endif
		spin_unlock_irqrestore(&dev->req_lock, flags);
	}
	return NETDEV_TX_OK;
}

/*-------------------------------------------------------------------------*/

static void eth_start(struct eth_dev *dev, gfp_t gfp_flags)
{
	DBG(dev, "%s\n", __func__);

	/* fill the rx queue */
	rx_fill(dev, gfp_flags);

	/* and open the tx floodgates */
	atomic_set(&dev->tx_qlen, 0);
	netif_wake_queue(dev->net);
}

static int eth_open(struct net_device *net)
{
	struct eth_dev	*dev = netdev_priv(net);
	struct gether	*link;

	DBG(dev, "%s\n", __func__);
	if (netif_carrier_ok(dev->net))
		eth_start(dev, GFP_KERNEL);

	spin_lock_irq(&dev->lock);
	link = dev->port_usb;
	if (link && link->open)
		link->open(link);
	spin_unlock_irq(&dev->lock);

	return 0;
}

static int eth_stop(struct net_device *net)
{
	struct eth_dev	*dev = netdev_priv(net);
	unsigned long	flags;

	VDBG(dev, "%s\n", __func__);
	netif_stop_queue(net);

	DBG(dev, "stop stats: rx/tx %ld/%ld, errs %ld/%ld\n",
		dev->net->stats.rx_packets, dev->net->stats.tx_packets,
		dev->net->stats.rx_errors, dev->net->stats.tx_errors
		);

	/* ensure there are no more active requests */
	spin_lock_irqsave(&dev->lock, flags);
	if (dev->port_usb) {
		struct gether	*link = dev->port_usb;

		if (link->close)
			link->close(link);

		/* NOTE:  we have no abort-queue primitive we could use
		 * to cancel all pending I/O.  Instead, we disable then
		 * reenable the endpoints ... this idiom may leave toggle
		 * wrong, but that's a self-correcting error.
		 *
		 * REVISIT:  we *COULD* just let the transfers complete at
		 * their own pace; the network stack can handle old packets.
		 * For the moment we leave this here, since it works.
		 */
		usb_ep_disable(link->in_ep);
		usb_ep_disable(link->out_ep);
		if (netif_carrier_ok(net)) {
			DBG(dev, "host still using in/out endpoints\n");
			usb_ep_enable(link->in_ep, link->in);
			usb_ep_enable(link->out_ep, link->out);
		}
	}
	spin_unlock_irqrestore(&dev->lock, flags);

	return 0;
}

/*-------------------------------------------------------------------------*/

/* initial value, changed by "ifconfig usb0 hw ether xx:xx:xx:xx:xx:xx" */
static char *dev_addr;
module_param(dev_addr, charp, S_IRUGO);
MODULE_PARM_DESC(dev_addr, "Device Ethernet Address");

/* this address is invisible to ifconfig */
static char *host_addr;
module_param(host_addr, charp, S_IRUGO);
MODULE_PARM_DESC(host_addr, "Host Ethernet Address");


static u8 __init nibble(unsigned char c)
{
	if (isdigit(c))
		return c - '0';
	c = toupper(c);
	if (isxdigit(c))
		return 10 + c - 'A';
	return 0;
}

static int __init get_ether_addr(const char *str, u8 *dev_addr)
{
	if (str) {
		unsigned	i;

		for (i = 0; i < 6; i++) {
			unsigned char num;

			if ((*str == '.') || (*str == ':'))
				str++;
			num = nibble(*str++) << 4;
			num |= (nibble(*str++));
			dev_addr [i] = num;
		}
		if (is_valid_ether_addr(dev_addr))
			return 0;
	}
	random_ether_addr(dev_addr);
	return 1;
}

#ifdef __IFX_USB_GADGET__
	static char *SerialNumber;
	module_param(SerialNumber, charp, S_IRUGO);
	MODULE_PARM_DESC(SerialNumber, "Serial Number");

	static char host_addr1[18];
	static char dev_addr1[18];

	static int __init get_ether_addr_2(const char *str, u8 *dev_addr)
	{
		if (str)
		{
			unsigned	i;

			for (i = 0; i < 6; i++) {
				unsigned char num;

				if ((*str == '.') || (*str == ':'))
					str++;
				num = nibble(*str++) << 4;
				num |= (nibble(*str++));
				dev_addr [i] = num;
			}
			if (is_valid_ether_addr(dev_addr))
				return 0;
		}
		return -1;
	}

	static void next_ether_addr (u8 *dev_addr,u8 *from_addr, int increment)
	{
		unsigned int i;
		for (i = 0; i < 6; i++)
			dev_addr[i]=from_addr[i];
		if(increment)
		{
			dev_addr[5] += increment;
			if (dev_addr[5] == 0)
			{
				dev_addr[4]++;
				if (dev_addr[4] == 0)
						dev_addr[3]++;
			}
		}
		return;
	}

	static int __init addr_setup (char *line)
	{
		u8 addr_str[18];
		char host_addr2[6];
		char dev_addr2[6];

		strncpy(addr_str, line, 17);
		addr_str[17] = '\0';

		if(get_ether_addr_2(addr_str,dev_addr2)==0)
		{
			next_ether_addr (host_addr2,dev_addr2,1);
			if(!dev_addr)
			{
				snprintf (dev_addr1, 17, "%02X:%02X:%02X:%02X:%02X:%02X",
					dev_addr2[0], dev_addr2[1], dev_addr2[2],
					dev_addr2[3], dev_addr2[4], dev_addr2[5]);
				dev_addr=dev_addr1;
			}
			if(!host_addr)
			{
				snprintf (host_addr1, 17, "%02X:%02X:%02X:%02X:%02X:%02X",
					host_addr2[0], host_addr2[1], host_addr2[2],
					host_addr2[3], host_addr2[4], host_addr2[5]);
				host_addr=host_addr1;
			}
		}
		return 0;
	}
	__setup("ethaddr=", addr_setup);
#endif


static struct eth_dev *the_dev;

static const struct net_device_ops eth_netdev_ops = {
	.ndo_open		= eth_open,
	.ndo_stop		= eth_stop,
	.ndo_start_xmit		= eth_start_xmit,
	.ndo_change_mtu		= ueth_change_mtu,
	.ndo_set_mac_address 	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};

/**
 * gether_setup - initialize one ethernet-over-usb link
 * @g: gadget to associated with these links
 * @ethaddr: NULL, or a buffer in which the ethernet address of the
 *	host side of the link is recorded
 * Context: may sleep
 *
 * This sets up the single network link that may be exported by a
 * gadget driver using this framework.  The link layer addresses are
 * set up using module parameters.
 *
 * Returns negative errno, or zero on success
 */
#ifdef __IFX_USB_GADGET__
	extern char serial_number [];
#endif
int __init gether_setup(struct usb_gadget *g, u8 hostaddr[ETH_ALEN], u8 devaddr[ETH_ALEN])
{
	struct eth_dev		*dev;
	struct net_device	*net;
	int			status;

	if (the_dev)
		return -EBUSY;

	net = alloc_etherdev(sizeof *dev);
	if (!net)
		return -ENOMEM;

	dev = netdev_priv(net);
	spin_lock_init(&dev->lock);
	spin_lock_init(&dev->req_lock);
	INIT_WORK(&dev->work, eth_work);
	INIT_LIST_HEAD(&dev->tx_reqs);
	INIT_LIST_HEAD(&dev->rx_reqs);

	skb_queue_head_init(&dev->rx_frames);

	/* network device setup */
	dev->net = net;
	strcpy(net->name, "usb%d");

	if (get_ether_addr(dev_addr, net->dev_addr))
		dev_warn(&g->dev,
			"using random %s ethernet address\n", "self");
	if (get_ether_addr(host_addr, dev->host_mac))
		dev_warn(&g->dev,
			"using random %s ethernet address\n", "host");

	if (hostaddr)
		memcpy(hostaddr, dev->host_mac, ETH_ALEN);
	if (devaddr)
		memcpy(devaddr, net->dev_addr, ETH_ALEN);
	#ifdef __IFX_USB_GADGET__
		if(SerialNumber)
			strncpy(serial_number, SerialNumber, 12);
		else
			snprintf (serial_number, 12, "%02X%02X%02X%02X%02X%02X",
				net->dev_addr[0], net->dev_addr[1],
				net->dev_addr[2], net->dev_addr[3],
				net->dev_addr[4], net->dev_addr[5]);
	#endif

	net->netdev_ops = &eth_netdev_ops;

	SET_ETHTOOL_OPS(net, &ops);

	/* two kinds of host-initiated state changes:
	 *  - iff DATA transfer is active, carrier is "on"
	 *  - tx queueing enabled if open *and* carrier is "on"
	 */
	netif_stop_queue(net);
	netif_carrier_off(net);

	dev->gadget = g;
	#if !defined(__IFX_USB_GADGET__ )
		SET_NETDEV_DEV(net, &g->dev);
	#endif
	status = register_netdev(net);
	if (status < 0) {
		dev_dbg(&g->dev, "register_netdev failed, %d\n", status);
		free_netdev(net);
	} else {
		INFO(dev, "MAC %pM\n", net->dev_addr);
		INFO(dev, "HOST MAC %pM\n", dev->host_mac);

		the_dev = dev;
	}
	return status;
}

/**
 * gether_cleanup - remove Ethernet-over-USB device
 * Context: may sleep
 *
 * This is called to free all resources allocated by @gether_setup().
 */
void gether_cleanup(void)
{
	if (!the_dev)
		return;

	unregister_netdev(the_dev->net);
	free_netdev(the_dev->net);

	/* assuming we used keventd, it must quiesce too */
	flush_scheduled_work();

	the_dev = NULL;
}


/**
 * gether_connect - notify network layer that USB link is active
 * @link: the USB link, set up with endpoints, descriptors matching
 *	current device speed, and any framing wrapper(s) set up.
 * Context: irqs blocked
 *
 * This is called to activate endpoints and let the network layer know
 * the connection is active ("carrier detect").  It may cause the I/O
 * queues to open and start letting network packets flow, but will in
 * any case activate the endpoints so that they respond properly to the
 * USB host.
 *
 * Verify net_device pointer returned using IS_ERR().  If it doesn't
 * indicate some error code (negative errno), ep->driver_data values
 * have been overwritten.
 */
struct net_device *gether_connect(struct gether *link)
{
	struct eth_dev		*dev = the_dev;
	int			result = 0;

	if (!dev)
		return ERR_PTR(-EINVAL);

	link->in_ep->driver_data = dev;
	result = usb_ep_enable(link->in_ep, link->in);
	if (result != 0) {
		DBG(dev, "enable %s --> %d\n",
			link->in_ep->name, result);
		goto fail0;
	}

	link->out_ep->driver_data = dev;
	result = usb_ep_enable(link->out_ep, link->out);
	if (result != 0) {
		DBG(dev, "enable %s --> %d\n",
			link->out_ep->name, result);
		goto fail1;
	}

	if (result == 0)
		#if defined(__IFX_USB_GADGET__)
			result = alloc_requests( dev, link, qlen_tx (dev->gadget),qlen_rx (dev->gadget));
		#else
			result = alloc_requests (dev, link, qlen(dev->gadget));
		#endif

	if (result == 0) {
		dev->zlp = link->is_zlp_ok;
		DBG(dev, "qlen %d\n", qlen(dev->gadget));

		dev->header_len = link->header_len;
		dev->unwrap = link->unwrap;
		dev->wrap = link->wrap;

		spin_lock(&dev->lock);
		dev->port_usb = link;
		link->ioport = dev;
		if (netif_running(dev->net)) {
			if (link->open)
				link->open(link);
		} else {
			if (link->close)
				link->close(link);
		}
		spin_unlock(&dev->lock);

		netif_carrier_on(dev->net);
		if (netif_running(dev->net))
			eth_start(dev, GFP_ATOMIC);

	/* on error, disable any endpoints  */
	} else {
		(void) usb_ep_disable(link->out_ep);
fail1:
		(void) usb_ep_disable(link->in_ep);
	}
fail0:
	/* caller is responsible for cleanup on error */
	if (result < 0)
		return ERR_PTR(result);
	return dev->net;
}

/**
 * gether_disconnect - notify network layer that USB link is inactive
 * @link: the USB link, on which gether_connect() was called
 * Context: irqs blocked
 *
 * This is called to deactivate endpoints and let the network layer know
 * the connection went inactive ("no carrier").
 *
 * On return, the state is as if gether_connect() had never been called.
 * The endpoints are inactive, and accordingly without active USB I/O.
 * Pointers to endpoint descriptors and endpoint private data are nulled.
 */
void gether_disconnect(struct gether *link)
{
	struct eth_dev		*dev = link->ioport;
	struct usb_request	*req;

	WARN_ON(!dev);
	if (!dev)
		return;

	DBG(dev, "%s\n", __func__);

	netif_stop_queue(dev->net);
	netif_carrier_off(dev->net);

	/* disable endpoints, forcing (synchronous) completion
	 * of all pending i/o.  then free the request objects
	 * and forget about the endpoints.
	 */
	usb_ep_disable(link->in_ep);
	spin_lock(&dev->req_lock);
	while (!list_empty(&dev->tx_reqs)) {
		req = container_of(dev->tx_reqs.next,
					struct usb_request, list);
		list_del(&req->list);

		spin_unlock(&dev->req_lock);
		#if defined(__IFX_USB_GADGET__)
			if(!req->context && req->buf)
				gadget_free_buffer(req->buf);
 		#endif
		usb_ep_free_request (link->in_ep, req);
		spin_lock(&dev->req_lock);
	}
	spin_unlock(&dev->req_lock);
	link->in_ep->driver_data = NULL;
	link->in = NULL;

	usb_ep_disable(link->out_ep);
	spin_lock(&dev->req_lock);
	while (!list_empty(&dev->rx_reqs)) {
		req = container_of(dev->rx_reqs.next,
					struct usb_request, list);
		list_del(&req->list);

		spin_unlock(&dev->req_lock);
		#if defined(__IFX_USB_GADGET__)
			if(!req->context && req->buf)
				gadget_free_buffer(req->buf);
 		#endif
		usb_ep_free_request (link->out_ep, req);
		spin_lock(&dev->req_lock);
	}
	spin_unlock(&dev->req_lock);
	link->out_ep->driver_data = NULL;
	link->out = NULL;

	/* finish forgetting about this USB link episode */
	dev->header_len = 0;
	dev->unwrap = NULL;
	dev->wrap = NULL;

	spin_lock(&dev->lock);
	dev->port_usb = NULL;
	link->ioport = NULL;
	#if defined(__MAC_ECM_FIX__)
		dev->ecm_only_postive = ecm_only_postive_max;
		dev->ecm_only_negtive = ecm_only_negtive_max;
	#endif
	spin_unlock(&dev->lock);
}
