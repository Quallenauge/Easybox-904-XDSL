/*****************************************************************************
 **   FILE NAME       : ifxpcd.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : The Peripheral Controller Driver (PCD) for Linux will implement the
 **                     Gadget API, so that the existing Gadget drivers can be used.
 **                     Support Transfer: CTRL(IN/OUT), BULK(IN/OUT), INTR(IN)
 **   FUNCTIONS       :
 **   COMPILER        : gcc
 **   REFERENCE       : Synopsys DWC-OTG Driver 2.7
 **   COPYRIGHT       :  Copyright (c) 2010
 **                      LANTIQ DEUTSCHLAND GMBH,
 **                      Am Campeon 3, 85579 Neubiberg, Germany
 **
 **    This program is free software; you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published by
 **    the Free Software Foundation; either version 2 of the License, or
 **    (at your option) any later version.
 **
 **  Version Control Section  **
 **   $Author$
 **   $Date$
 **   $Revisions$
 **   $Log$       Revision history
 *****************************************************************************/

/*
 * This file contains code fragments from Synopsys HS OTG Linux Software Driver.
 * For this code the following notice is applicable:
 *
 * ==========================================================================
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/*!
 \file ifxpcd.c
 \ingroup IFXUSB_DRIVER_V3
 \brief This file contains the implementation of the PCD connecting to Gadget API
*/

#include <linux/version.h>
#include "ifxusb_version.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <asm/irq.h>

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"

#include "ifxpcd.h"

extern ifxpcd_pcd_t ifxusb_pcd;



/*!
  \brief  This function terminates all the requsts in the EP request queue.
 */
void request_nuke( ifxpcd_ep_t *_ifxep )
{
	ifxpcd_request_t   *ifxreq;

	if(!_ifxep)
	{
		IFX_WARN("%s() %d invalid _ifxep\n",__func__,__LINE__);
		return;
	}

	while (!list_empty(&_ifxep->queue_cmpt))
	{
		ifxreq = list_entry(_ifxep->queue_cmpt.next, ifxpcd_request_t,trq);
		list_del_init(&ifxreq->trq);
		if(ifxreq->sysreq.complete)
		{
			#ifdef __DO_PCD_UNLOCK__
				SPIN_UNLOCK(&ifxusb_pcd.lock);
			#endif
			ifxreq->sysreq.complete(&_ifxep->sysep, &ifxreq->sysreq);
			#ifdef __DO_PCD_UNLOCK__
				SPIN_LOCK(&ifxusb_pcd.lock);
			#endif
		}
	}

	while (!list_empty(&_ifxep->queue))
	{
		ifxreq = list_entry(_ifxep->queue.next, ifxpcd_request_t,trq);
		ifxreq->sysreq.status = -ESHUTDOWN;
		list_del_init(&ifxreq->trq);
		if(ifxreq->sysreq.complete)
		{
			#ifdef __DO_PCD_UNLOCK__
				SPIN_UNLOCK(&ifxusb_pcd.lock);
			#endif
			ifxreq->sysreq.complete(&_ifxep->sysep, &ifxreq->sysreq);
			#ifdef __DO_PCD_UNLOCK__
				SPIN_LOCK(&ifxusb_pcd.lock);
			#endif
		}
	}
}

#if defined(__DED_FIFO__)
	/*!
	  \brief  This function assigns Tx FIFO to an EP in dedicated Tx FIFO mode
	          Dedicated FIFO mode only
	*/
	static uint32_t assign_tx_fifo(uint32_t fifo_num)
	{
		if(ifxusb_pcd.tx_msk & (1 << (fifo_num - 1)))
			return 0;
		
		ifxusb_pcd.tx_msk |= (1 << (fifo_num - 1));
		return fifo_num;
	}

	/*!
	  \brief  This function releases Tx FIFO in dedicated Tx FIFO mode
	      Dedicated FIFO mode only
	 */
	static void release_tx_fifo(uint32_t fifo_num)
	{
		ifxusb_pcd.tx_msk = (ifxusb_pcd.tx_msk & (1 << (fifo_num - 1))) ^ ifxusb_pcd.tx_msk;
	}
#endif //__DED_FIFO__

int start_next_request( ifxpcd_ep_t *_ifxep )
{
	if (!_ifxep->ep_act)
		return 0;
	if (_ifxep->ep_stalled)
		return 0;
	if (list_empty(&_ifxep->queue))
		return 0;
	if ( _ifxep->stall_clear_flag)
		return 0;
	if(_ifxep->ep_xfer_stage == InExec)
		return 0;

	if ( _ifxep->ep_xfer_stage == Setup) // Continue
	{
		if(_ifxep->is_in)
			ifxpcd_ep_start_tx_transfer(_ifxep);
		else
			ifxpcd_ep_start_rx_transfer(_ifxep);
	}
	else
	{
		ifxpcd_request_t   *ifxreq = 0;
		ifxreq = list_entry(_ifxep->queue.next, ifxpcd_request_t, trq);
		ifxpcd_start_new_transfer(ifxreq,_ifxep);
	}
	if(_ifxep->is_in && _ifxep->ep_xfer_stage == InExec )
		return 1;
	return 0;
}

void restart_requests()
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	int i;

	IFX_DEBUGPL(DBG_PCDV, "Start xfer tasklet CTRL EP0=%d\n",ifxusb_pcd.ifxep[0].ep_xfer_stage);

	#if    defined(__DED_FIFO__)
		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_CTRL)
				start_next_request(&ifxusb_pcd.ifxep[i]);

		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR)
				start_next_request(&ifxusb_pcd.ifxep[i]);

		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK)
				start_next_request(&ifxusb_pcd.ifxep[i]);
	#elif  defined(__DED_INTR__)
		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR)
				start_next_request(&ifxusb_pcd.ifxep[i]);

		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_CTRL)
				if(start_next_request(&ifxusb_pcd.ifxep[i]))
					return;

		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK)
				if(start_next_request(&ifxusb_pcd.ifxep[i]))
					return;
	#else
		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_CTRL)
				if(start_next_request(&ifxusb_pcd.ifxep[i]))
					return;
		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR)
				if(start_next_request(&ifxusb_pcd.ifxep[i]))
					return;
		for (i=0; i <=core_if->hwcfg2.b.num_dev_ep; i++)
			if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK)
				if(start_next_request(&ifxusb_pcd.ifxep[i]))
					return;
	#endif

	IFX_DEBUGPL(DBG_PCDV, "Start xfer tasklet FINISH\n");
}




/////////////////////////////////////////////////////////////////////////////////
// Gadget EP operations
//   These EP dendant functions are implemented in the PCD for Gadget Driver to call
//   through Gadget-API.
/////////////////////////////////////////////////////////////////////////////////

static int ifxpcd_ep_enable(struct usb_ep *_sysep, const struct usb_endpoint_descriptor *_desc);
static int ifxpcd_ep_disable(struct usb_ep *_sysep);

static struct usb_request *ifxpcd_alloc_request(struct usb_ep *_sysep, gfp_t _gfp_flags);
static void  ifxpcd_free_request(struct usb_ep *_sysep, struct usb_request *_sysreq);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	static void *ifxpcd_alloc_buffer(struct usb_ep *_sysep, unsigned _bytes, dma_addr_t *_dma, gfp_t _gfp_flags);
	static void  ifxpcd_free_buffer(struct usb_ep *_sysep, void *_buf, dma_addr_t _dma, unsigned _bytes);
#endif

static int ifxpcd_ep_queue(struct usb_ep *_sysep, struct usb_request *_sysreq, gfp_t _gfp_flags);
static int ifxpcd_ep_dequeue(struct usb_ep *_sysep, struct usb_request *_sysreq);

static int ifxpcd_ep_set_halt(struct usb_ep *_sysep, int _value);

static struct usb_ep_ops ifxpcd_ep_ops =
{
	.enable             = ifxpcd_ep_enable,
	.disable            = ifxpcd_ep_disable,

	.alloc_request      = ifxpcd_alloc_request,
	.free_request       = ifxpcd_free_request,

	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	.alloc_buffer       = ifxpcd_alloc_buffer,
	.free_buffer        = ifxpcd_free_buffer,
	#endif

	.queue              = ifxpcd_ep_queue,
	.dequeue            = ifxpcd_ep_dequeue,

	.set_halt           = ifxpcd_ep_set_halt,
	.fifo_status        = 0,
	.fifo_flush         = 0,
};


void request_done_tasklet(unsigned long data);

/*!
	 \fn    static int ifxpcd_ep_enable(struct usb_ep *_sysep, const struct usb_endpoint_descriptor *_desc)
	 \brief This function is called to enable an EP.
	 \param _sysep Pointer to list of end points
	 \param _desc Pointer to endpoint descriptor
	 \ingroup  IFXUSB_PCD
 */
static int ifxpcd_ep_enable(struct usb_ep *_sysep, const struct usb_endpoint_descriptor *_desc)
{
	ifxpcd_ep_t *ifxep = NULL;

	IFX_DEBUGPL(DBG_PCDV,"%s(%p,%p)\n", __func__, _sysep, _desc );

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);

	if (!_sysep || !_desc || ifxep->desc || _desc->bDescriptorType != USB_DT_ENDPOINT)
	{
		if (!_sysep )
			IFX_WARN( "%s, bad sysep\n", __func__);
		if (!_desc)
			IFX_WARN( "%s, descriptor(DESC=NULL)\n", __func__);
		if (ifxep->desc)
			IFX_WARN( "%s, exsiting ifxep descriptor\n", __func__);
		if (_desc)
			if (_desc->bDescriptorType != USB_DT_ENDPOINT)
				IFX_WARN( "%s, wrong descriptor type(%d)\n", __func__,_desc->bDescriptorType);
		return -EINVAL;
	}

	#if defined(__GADGET_LED__)
		if (ifxep->num)
			ifxusb_led_init(&ifxusb_pcd.core_if);
	#endif

	/* Check FIFO size? */
	if (!_desc->wMaxPacketSize)
	{
		IFX_WARN("%s, bad %s MaxPacketSize\n", __func__, _sysep->name);
		return -ERANGE;
	}

	if (!ifxusb_pcd.driver || ifxusb_pcd.gadget.speed == USB_SPEED_UNKNOWN)
	{
		IFX_WARN("%s, bogus device state\n", __func__);
		return -ESHUTDOWN;
	}

	if (ifxep->num == 0)
	{
		IFX_WARN("%s, bad ep(0)\n", __func__);
		ifxep->ep_act=1;
		return -EINVAL;
	}

	if (ifxep->ep_act)
	{
		IFX_WARN("%s, ep%d reactivate\n", __func__,ifxep->num);
		return -ESHUTDOWN;
	}

	SPIN_LOCK(&ifxusb_pcd.lock);
	if (ifxep->num && (ifxusb_pcd.power_status == 1))
	{
		ifxusb_pcd.power_status = 0;
		del_timer(&ifxusb_pcd.device_probe_timer);
	}
	else if(ifxep->num && (ifxusb_pcd.power_status == 2))
	{
		ifxusb_pcd.power_status = 0;
		del_timer(&ifxusb_pcd.device_autoprobe_timer);
		del_timer(&ifxusb_pcd.device_probe_timer);
	}
	ifxep->desc = _desc;
	_sysep->maxpacket = le16_to_cpu (_desc->wMaxPacketSize);

	ifxep->is_in = (USB_DIR_IN & _desc->bEndpointAddress) != 0;
	ifxep->mps   = _sysep->maxpacket;
	ifxep->type  = _desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;

	#if   defined(__UNALIGNED_BUF_ADJ_)
		ifxep->using_aligned_buf=0;
		if(ifxep->aligned_buf)
			ifxusb_free_buf_d(ifxep->aligned_buf);
		ifxep->aligned_buf=NULL;
		ifxep->aligned_buf_len=0;
	#endif //defined(__UNALIGNED_BUF_ADJ_)

	#if   defined(__DED_FIFO__)
		ifxep->tx_fifo_num = 0;
		if(ifxep->is_in)
		{
			#if 1
				if(ifxep->num==3)
					ifxep->tx_fifo_num = assign_tx_fifo(2);
				else if(ifxep->num)
					ifxep->tx_fifo_num = assign_tx_fifo(1);
			#else
				ifxep->tx_fifo_num = assign_tx_fifo();
			#endif
		}
	#elif defined(__DED_INTR__)
		ifxep->tx_fifo_num = 0;
		if(ifxep->is_in && ifxep->type == IFXUSB_EP_TYPE_INTR)
			ifxep->tx_fifo_num = 1;
	#endif

	#ifdef __DESC_DMA__
		if (ifxep->type == IFXUSB_EP_TYPE_CTRL)
		{
			if(ifxep->dma_desc)
				ep_free_desc((uint32_t *)(ifxep->dma_desc));
			ifxep->dma_desc = ep_alloc_desc(1);
			if(!ifxep->dma_desc)
			{
				IFX_WARN("%s, can't allocate DMA descriptor\n", __func__);
				return -ENOMEM;
			}
		}
		else if(ifxep->is_in)
		{
			if(ifxep->dma_desc)
				ep_free_desc((uint32_t *)(ifxep->dma_desc));
			ifxep->dma_desc = ep_alloc_desc(1);
			if(!ifxep->dma_desc)
			{
				IFX_WARN("%s, can't allocate DMA descriptor\n", __func__);
				return -ENOMEM;
			}
		}
		else
		{
			if(ifxep->dma_desc)
				ep_free_desc((uint32_t *)(ifxep->dma_desc));
			ifxep->dma_desc = ep_alloc_desc(1);
			if(!ifxep->dma_desc)
			{
				IFX_WARN("%s, can't allocate DMA descriptor\n", __func__);
				return -ENOMEM;
			}
		}
	#endif //__DESC_DMA__

	#if !defined(__DED_FIFO__)
		ifxep->ep_gnakdisabling=0;
		ifxep->ep_hold         =0;
	#endif
	ifxep->ep_stalled      =0;
	ifxep->ep_xfer_stage   =Idle;
	#if defined(__GADGET_TASKLET_TX__) || defined(__GADGET_TASKLET_RX__)
		ifxep->cmpt_tasklet_in_process   =0;
		ifxep->cmpt_tasklet.next = NULL;
		ifxep->cmpt_tasklet.state = 0;
		atomic_set( &ifxep->cmpt_tasklet.count, 0);
		ifxep->cmpt_tasklet.func = &request_done_tasklet;
		ifxep->cmpt_tasklet.data = (unsigned long)ifxep;
	#endif

	IFX_DEBUGPL(DBG_PCD, "Activate %s-%s: type=%d, mps=%d desc=%p\n",
	                _sysep->name, (ifxep->is_in ?"IN":"OUT"),
	                ifxep->type, ifxep->mps, ifxep->desc );

	ifxpcd_ep_activate( ifxep );
	SPIN_UNLOCK(&ifxusb_pcd.lock);
	ifxep->ep_act =1;
	return 0;
}

/*!
   \brief This function is called when an EP is disabled due to disconnect or
          change in configuration. Any pending requests will terminate with a
          status of -ESHUTDOWN.
 */
static int ifxpcd_ep_disable(struct usb_ep *_sysep)
{
	ifxpcd_ep_t *ifxep;

	IFX_DEBUGPL(DBG_PCDV,"%s(%p)\n", __func__, _sysep);

	if (!_sysep)
	{
		IFX_DEBUGPL(DBG_PCD, "%s, %s not enabled\n", __func__, _sysep ? _sysep->name : NULL);
		return -EINVAL;
	}

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);
	if (!ifxep->desc)
	{
		IFX_DEBUGPL(DBG_PCD, "%s, %s not enabled\n", __func__, _sysep ? _sysep->name : NULL);
		return -EINVAL;
	}

	SPIN_LOCK(&ifxusb_pcd.lock);

	request_nuke( ifxep );
	ifxpcd_ep_deactivate( ifxep );

	ifxep->desc = 0;
	ifxep->ep_act          =0;
	ifxep->ep_stalled      =1;
	#if !defined(__DED_FIFO__)
		ifxep->ep_gnakdisabling=0;
		ifxep->ep_hold         =0;
	#endif
	ifxep->ep_xfer_stage   =Idle;

	#if   defined(__UNALIGNED_BUF_ADJ_)
		ifxep->using_aligned_buf=0;
		if(ifxep->aligned_buf)
			ifxusb_free_buf_d(ifxep->aligned_buf);
		ifxep->aligned_buf=NULL;
		ifxep->aligned_buf_len=0;
	#endif //defined(__UNALIGNED_BUF_ADJ_)

	if(ifxep->is_in)
	{
		#if   defined(__DED_FIFO__)
			ifxusb_flush_tx_fifo_d(&ifxusb_pcd.core_if, ifxep->tx_fifo_num);
			release_tx_fifo(ifxep->tx_fifo_num);
		#elif defined(__DED_INTR__)
			if(ifxep->type == IFXUSB_EP_TYPE_INTR)
				ifxusb_flush_tx_fifo_d(&ifxusb_pcd.core_if, 1);
			else
				ifxusb_flush_tx_fifo_d(&ifxusb_pcd.core_if, 0);
		#else
			ifxusb_flush_tx_fifo_d(&ifxusb_pcd.core_if, 0);
		#endif
	}
	#ifdef __DESC_DMA__
		/* Free DMA Descriptors */
		if(ifxep->dma_desc)
			ep_free_desc((uint32_t *)(ifxep->dma_desc));
		ifxep->dma_desc=0;
	#endif //__DESC_DMA__

	SPIN_UNLOCK(&ifxusb_pcd.lock);

	IFX_DEBUGPL(DBG_PCD, "%s disabled\n", _sysep->name);
	return 0;
}

 /*!
	 \fn    static struct usb_request *ifxpcd_alloc_request(struct usb_ep *_sysep, gfp_t _gfp_flags)
	 \brief This function allocates a request object to use with the specified endpoint.
	 \param _sysep Pointer to list of end points
	 \param _gfp_flags gfp flags
	 \Return *usb_request Pointer to usb_request
	 \ingroup  IFXUSB_PCD
 */
static struct usb_request *ifxpcd_alloc_request(struct usb_ep *_sysep, gfp_t _gfp_flags)
{
	ifxpcd_ep_t *ifxep;
	ifxpcd_request_t *ifxreq;
	if (! _sysep )
	{
		IFX_WARN("%s() %s\n", __func__, "Invalid EP!\n");
		return 0;
	}

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);

	ifxreq = kmalloc( sizeof(ifxpcd_request_t), _gfp_flags);
	if (!ifxreq)
	{
		IFX_ERROR("%s() %s\n", __func__, "request allocation failed!\n");
		return 0;
	}
	memset(ifxreq, 0, sizeof(ifxpcd_request_t));
	ifxreq->sysreq.dma = DMA_ADDR_INVALID;
	INIT_LIST_HEAD(&ifxreq->eplist);
	INIT_LIST_HEAD(&ifxreq->trq);
	INIT_LIST_HEAD(&ifxreq->sysreq.list);

	ifxreq->ifxep=ifxep;
	list_add_tail(&ifxreq->eplist, &ifxep->reqs);

	#ifdef __req_num_dbg__
	{
		int i;
		struct list_head *q;
		ifxpcd_request_t *req;
		for(i=0,q=ifxep->reqs.next;q!=&ifxep->reqs;q=q->next,i++)
		{
			req = container_of(q, ifxpcd_request_t, eplist);
			req->reqid=i+1;
		}
		ifxep->req_buf_num=i;
	}
	#endif

	return &ifxreq->sysreq;
}

/*!
   \brief This function frees a request object.
 */
 /*!
	 \fn    static void ifxpcd_free_request(struct usb_ep *_sysep, struct usb_request *_sysreq)
	 \brief This function frees a usb request object.
	 \param _sysep Pointer to list of end points
	 \param _sysreq usb request object
	 \Return None
	 \ingroup  IFXUSB_PCD
 */
static void ifxpcd_free_request(struct usb_ep *_sysep, struct usb_request *_sysreq)
{
	ifxpcd_request_t *ifxreq = NULL;
	ifxpcd_ep_t *ifxep;

	IFX_DEBUGPL(DBG_PCDV,"%s(%p,%p)\n", __func__, _sysep, _sysreq);

	if (! _sysep || ! _sysreq)
	{
		IFX_WARN("%s() %s\n", __func__, "Invalid ep or req argument!\n");
		return;
	}

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);

//	if(_sysreq->buf)
//		IFX_WARN("%s() %s\n", __func__, "REQ buf not freed!\n");

	ifxreq = container_of(_sysreq, ifxpcd_request_t, sysreq);

	if(ifxep!=ifxreq->ifxep)
		IFX_WARN("%s() %s\n", __func__, "EP not match!\n");

	list_del_init(&ifxreq->eplist);

	#ifdef __req_num_dbg__
	{
		int i;
		struct list_head *q;
		ifxpcd_request_t *req;
		for(i=0,q=ifxep->reqs.next;q!=&ifxep->reqs;q=q->next,i++)
		{
			req = container_of(q, ifxpcd_request_t, eplist);
			req->reqid=i;
		}
		ifxep->req_buf_num=i;
	}
	#endif

	kfree(ifxreq);
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
 /*!
	 \fn    static void *ifxpcd_alloc_buffer(struct usb_ep *_sysep, unsigned _bytes,
                                 dma_addr_t *_dma, gfp_t _gfp_flags)
	 \brief This function allocates an I/O buffer to be used for a transfer to/from the specified endpoint.
	 \param _sysep Pointer to list of end points
	 \param _bytes Size of buffer
	 \param _dma Dma attributes
	 \Return *buf Pointer to the buffer allocated
	 \ingroup  IFXUSB_PCD
 */
static void *ifxpcd_alloc_buffer(struct usb_ep *_sysep, unsigned _bytes,
                                 dma_addr_t *_dma, gfp_t _gfp_flags)
{
	void *buf;

	/* Check dword alignment */
	if ((_bytes & 0x3UL) != 0)
		IFX_WARN("%s() Buffer size is not a multiple of DWORD size (%d)",__func__, _bytes);

	buf=ifxusb_alloc_buf_d(_bytes, 1);

	if(!buf)
		IFX_ERROR("%s() %s\n", __func__, "buffer allocation failed!\n");

	/* Check dword alignment */
	if (((int)buf & 0x3UL) != 0)
		IFX_WARN("%s() Buffer is not DWORD aligned (%p)",__func__, buf);

	if(_dma)
		*_dma=CPHYSADDR(buf);
	return buf;
}


 /*!
	 \fn    static void ifxpcd_free_buffer(struct usb_ep *_sysep, void *_buf,
                                    dma_addr_t _dma, unsigned _bytes)
	 \brief function frees an I/O buffer that was allocated by alloc_buffer.
	 \param _sysep Pointer to list of end points
	 \param _buf Pointer to the buffer allocated
	 \param _dma Dma attributes
	 \param _bytes Size of buffer
	 \ingroup  IFXUSB_PCD
 */
static void ifxpcd_free_buffer(struct usb_ep *_sysep, void *_buf,
                                    dma_addr_t _dma, unsigned _bytes)
{
	ifxusb_free_buf_d(_buf);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
   \fn static int ifxpcd_ep_queue(struct usb_ep *_sysep, struct usb_request *_sysreq, gfp_t _gfp_flags)
   \brief This function is used to submit an I/O Request to an EP.
       - When the request completes the request's completion callback
         is called to return the request to the driver.
       - An EP, except control EPs, may have multiple requests pending.
       - Once submitted the request cannot be examined or modified.
       - Each request is turned into one or more packets.
       - A BULK EP can queue any amount of data; the transfer is packetized.
       - Zero length Packets are specified with the request 'zero' flag.
   \param _sysep Pointer to list of end points
   \param _sysreq usb request object
   \param _gfp_flags gfp flags
   \Return -EINVAL Error
    \ingroup  IFXUSB_PCD
 */
static int ifxpcd_ep_queue(struct usb_ep *_sysep, struct usb_request *_sysreq, gfp_t _gfp_flags)
{
	ifxpcd_request_t *ifxreq;
	ifxpcd_ep_t      *ifxep;
	unsigned long     flags = 0;

	IFX_DEBUGPL(DBG_PCDV,"%s(%p,%p,%d)\n", __func__, _sysep, _sysreq, _gfp_flags);

	if (!_sysreq)
	{
		IFX_WARN("%s, bad params, no sysreq\n", __func__);
		return -EINVAL;
	}

	if (!_sysreq->complete)
	{
		IFX_WARN("%s, bad params, no complete function\n", __func__);
		return -EINVAL;
	}

	if (_sysreq->length && !_sysreq->buf)
	{
		IFX_WARN("%s, bad params, length==0 but buf=%p\n", __func__,_sysreq->buf);
		return -EINVAL;
	}

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);
	ifxreq = container_of(_sysreq, ifxpcd_request_t, sysreq);

	if (!list_empty(&ifxreq->trq))
	{
		IFX_WARN("%s, EP%d bad params sysreq already hooked to some queue (%p,%p)\n", __func__,ifxep->num, &ifxreq->trq,ifxreq->trq.next);
		return -EINVAL;
	}

	if (!_sysep || (!ifxep->desc && ifxep->num != 0))
	{
		if(!_sysep)
			IFX_WARN("%s, null ep\n", __func__);
		else
			IFX_WARN("%s, Null Descriptor\n", __func__);
		return -EINVAL;
	}

	if (!ifxusb_pcd.driver || ifxusb_pcd.gadget.speed == USB_SPEED_UNKNOWN)
	{
		IFX_DEBUGPL(DBG_PCDV, "gadget.speed=%d\n", ifxusb_pcd.gadget.speed);
		IFX_WARN("%s, bogus device state\n", __func__);
		return -ESHUTDOWN;
	}

	if (!ifxep->is_in && !_sysreq->buf)
	{
		IFX_WARN("%s, bad params, OUT but buf=%p\n", __func__,_sysreq->buf);
		return -EINVAL;
	}

	if(ifxep->num==0) ifxep->ep_act=1;

	if (!ifxep->ep_act)
	{
		//IFX_WARN("%s, EP %d not activated\n", __func__,ifxep->num);
		return -EINVAL;
	}

	if (ifxep->ep_stalled)
	{
		IFX_WARN("%s, EP %d stalled\n", __func__,ifxep->num);
		return -EINVAL;
	}

	IFX_DEBUGPL(DBG_PCD, "%s queue req %p, len %d buf %p\n",
	      _sysep->name, _sysreq, _sysreq->length, _sysreq->buf);

	SPIN_LOCK_IRQSAVE(&ifxusb_pcd.lock, flags);

	#if defined(__DEBUG__) & defined(__VERBOSE__)
		ifxusb_dump_msg_d(_sysreq->buf, _sysreq->length);
	#endif

	ifxreq->buf     = _sysreq->buf;
	ifxreq->len     = _sysreq->length;

	_sysreq->status = -EINPROGRESS;
	_sysreq->actual = 0;

	ifxreq->ifxep=ifxep;

	if(ifxep->type==IFXUSB_EP_TYPE_INTR)
	{
		#if   defined(__DED_FIFO__) || defined(__DED_INTR__)
			if (!list_empty(&ifxep->queue))
				request_nuke(ifxep );
			ifxep->ep_xfer_stage=Idle;
			list_add_tail(&ifxreq->trq, &ifxep->queue);
			ifxpcd_start_new_transfer(ifxreq,ifxep);
		#else
			if(ifxusb_pcd.INTR_Pending)
			{
				if (!list_empty(&ifxep->queue))
					request_nuke(ifxep );
				if (ifxep->ep_act && !ifxep->ep_stalled)
				{
					ifxep->ep_xfer_stage=Idle;
					list_add_tail(&ifxreq->trq, &ifxep->queue);
					ifxpcd_start_new_transfer(ifxreq,ifxep);
				}
				else
					list_add_tail(&ifxreq->trq, &ifxep->queue);
			}
			else
			{
				list_add_tail(&ifxreq->trq, &ifxep->queue);
				ifxep->ep_hold=1;
			}
		#endif
	}
	else if (ifxep->num == 0)
	{
		request_nuke( ifxep );
		if(ifxusb_pcd.ep0state == EP0_IN_DATA_PHASE || ifxusb_pcd.ep0state == EP0_OUT_DATA_PHASE)
		{
			if (ifxusb_pcd.request_config) //special case
			{
				/* Complete STATUS PHASE */
				ifxep->is_in = 1;
				ifxusb_pcd.ep0state = EP0_IN_STATUS_PHASE;
				ep0_out_start();
				IFX_DEBUGPL(DBG_PCD, "%s ep0: EP0_IN_STATUS_PHASE (Config)\n", __func__);
			}
			else if(ifxusb_pcd.ep0state == EP0_IN_DATA_PHASE)
				IFX_DEBUGPL(DBG_PCD, "%s ep0: EP0_IN_DATA_PHASE\n", __func__);
			else
				IFX_DEBUGPL(DBG_PCD, "%s ep0: EP0_OUT_DATA_PHASE\n", __func__);
		}
		else if(ifxusb_pcd.ep0state == EP0_OUT_STATUS_PHASE)
		{
			IFX_DEBUGPL(DBG_PCD, "%s ep0: EP0_OUT_STATUS_PHASE\n", __func__);
		}
		else if(ifxusb_pcd.ep0state == EP0_IN_STATUS_PHASE)
		{
			ep0_out_start();
			IFX_DEBUGPL(DBG_PCD, "%s ep0: EP0_IN_STATUS_PHASE\n", __func__);
		}
		else
		{
			IFX_DEBUGPL(DBG_ANY, "ep0: odd state %d\n",ifxusb_pcd.ep0state);
			SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
			return -EL2HLT;
		}
		list_add_tail(&ifxreq->trq, &ifxep->queue);
		ifxpcd_start_new_transfer(ifxreq,ifxep);
	}
	else if (list_empty(&ifxep->queue) && ifxep->ep_xfer_stage == Idle)
	{
		list_add_tail(&ifxreq->trq, &ifxep->queue);
		ifxpcd_start_new_transfer(ifxreq,ifxep);
	}
	else
		list_add_tail(&ifxreq->trq, &ifxep->queue);

	SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
	return 0;
}

 /*!
   \fn static int ifxpcd_ep_dequeue(struct usb_ep *_sysep, struct usb_request *_sysreq)
   \brief This function cancels an I/O request from an EP.
   \param _sysep Pointer to list of end points
   \param _sysreq usb request object
   \Return -EINVAL Error
    \ingroup  IFXUSB_PCD
 */
static int ifxpcd_ep_dequeue(struct usb_ep *_sysep, struct usb_request *_sysreq)
{
	ifxpcd_request_t   *ifxreq;
	ifxpcd_ep_t        *ifxep;
	unsigned long flags;

	IFX_DEBUGPL(DBG_PCDV,"%s(%p,%p)\n", __func__, _sysep, _sysreq);

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);
	if (!_sysep || !_sysreq || (!ifxep->desc && ifxep->num != 0))
	{
		IFX_WARN("%s, bad argument\n", __func__);
		return -EINVAL;
	}

	if (!ifxusb_pcd.driver || ifxusb_pcd.gadget.speed == USB_SPEED_UNKNOWN)
	{
		IFX_WARN("%s, bogus device state\n", __func__);
		return -ESHUTDOWN;
	}

	SPIN_LOCK_IRQSAVE(&ifxusb_pcd.lock, flags);
	IFX_DEBUGPL(DBG_PCDV, "%s %s %s %p\n", __func__, _sysep->name,
	         ifxep->is_in ? "IN" : "OUT",
	         _sysreq);

	/* make sure it's actually queued on this endpoint */
	list_for_each_entry( ifxreq, &ifxep->queue, trq)
	{
		if (&ifxreq->sysreq == _sysreq)
			break;
	}

	if (&ifxreq->sysreq != _sysreq)
	{
		SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
		return -EINVAL;
	}
	if (!list_empty(&ifxreq->trq))
		request_done(ifxep, ifxreq, -ECONNRESET);
	else
		ifxreq = 0;

	SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
	return ifxreq ? 0 : -EOPNOTSUPP;
}


/*!
   \fn static int ifxpcd_ep_set_halt(struct usb_ep *_sysep, int _value)
   \brief usb_ep_set_halt stalls an endpoint.
          usb_ep_clear_halt clears an endpoint halt and resets its data toggle.
          Both of these functions are implemented with the same underlying
          function. The behavior depends on the value argument.
           _value
                     - 0 means clear_halt.
                     - 1 means set_halt,
                     - 2 means clear stall lock flag.
                     - 3 means set  stall lock flag.
   \param _sysep Pointer to list of end points
   \param _value value
   \return retval
    \ingroup  IFXUSB_PCD
 */
static int ifxpcd_ep_set_halt(struct usb_ep *_sysep, int _value)
{
	int retval = 0;
	unsigned long flags;
	ifxpcd_ep_t *ifxep = 0;

	IFX_DEBUGPL(DBG_PCD,"HALT %s %d\n", _sysep->name, _value);

	ifxep = container_of(_sysep, ifxpcd_ep_t, sysep);
	if (!_sysep ||
	    (!ifxep->desc && ifxep != &ifxusb_pcd.ifxep[0]) ||
	    (ifxep->desc && ifxep->desc->bmAttributes == USB_ENDPOINT_XFER_ISOC))
	{
		if (!_sysep )
			IFX_WARN("%s, null ep\n", __func__);
		if ( !ifxep->desc && ifxep != &ifxusb_pcd.ifxep[0])
			IFX_WARN("%s, ep not equal to ep0\n", __func__);
		if( ifxep->desc && ifxep->desc->bmAttributes == USB_ENDPOINT_XFER_ISOC)
	        IFX_WARN("%s, bad ep ISOC\n", __func__);
		return -EINVAL;
	}

	SPIN_LOCK_IRQSAVE(&ifxusb_pcd.lock, flags);

	if (!list_empty(&ifxep->queue))
	{
		IFX_WARN("%s() %s XFer In process\n", __func__, _sysep->name);
		retval = -EAGAIN;
	}
	else if (_value == 0)
		ifxusb_dev_ep_clear_stall( &ifxusb_pcd.core_if, ifxep->num,ifxep->type,ifxep->is_in );
	else if(_value == 1)
	{
		#ifdef __DED_FIFO__
			if (ifxep->is_in)
			{
				dtxfsts_data_t txstatus;
				fifosize_data_t txfifosize;

				txfifosize.d32 = ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->dptxfsiz_dieptxf[ifxep->tx_fifo_num]);
				txstatus.d32   = ifxusb_rreg(&ifxusb_pcd.core_if.in_ep_regs[ifxep->num]->dtxfsts);

				if(txstatus.b.txfspcavail < txfifosize.b.depth)
				{
					IFX_WARN("%s() %s Data In Tx Fifo\n", __func__, _sysep->name);
					retval = -EAGAIN;
				}
			}
		#endif //__DED_FIFO__
		if(retval==0)
		{
			if (ifxep->num == 0)
				ifxusb_pcd.ep0state = EP0_STALL;
			ifxep->ep_stalled = 1;
			ifxusb_dev_ep_set_stall( &ifxusb_pcd.core_if, ifxep->num,ifxep->is_in );
		}
	}
	else if (_value == 2)
		ifxep->stall_clear_flag = 0;
	else if (_value == 3)
		ifxep->stall_clear_flag = 1;
	else
		IFX_WARN("%s, Invalid Stall value %d\n", __func__,_value);
	SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
	return retval;
}

/////////////////////////////////////////////////////////////////////////////////
// Gadget operations
//   These EP indendant functions are implemented in the PCD for Gadget Driver to call
//   through Gadget-API.
/////////////////////////////////////////////////////////////////////////////////

static int ifxpcd_get_frame(struct usb_gadget *_gadget);
static int ifxpcd_wakeup(struct usb_gadget *gadget);
static int ifxpcd_pullup(struct usb_gadget *gadget,int pullup);

static const struct usb_gadget_ops ifxpcd_ops =
{
	.get_frame        = ifxpcd_get_frame,
	.wakeup           = ifxpcd_wakeup,
	.set_selfpowered  = 0,
	.vbus_session     = 0,
	.vbus_draw        = 0,
	.pullup           = ifxpcd_pullup,
	.ioctl            = 0,
};


/*!
   \brief Gets the USB Frame number of the last SOF.
 */
static int ifxpcd_get_frame(struct usb_gadget *_gadget)
{
	IFX_DEBUGPL(DBG_PCDV,"%s(%p)\n", __func__, _gadget);
	#if 1
		if (_gadget == 0)
			return -ENODEV;
		return ifxusb_dev_get_frame_number( &ifxusb_pcd.core_if );
	#else
		ifxusb_dev_get_frame_number( &ifxusb_pcd.core_if );
		return 0;
	#endif
}

/*!
   \brief this function set core to send RemoteWakeUp signal to host
          may be called by gadget driver or user level app like PM
 */
void ifxpcd_remote_wakeup(int set)
{
	dctl_data_t dctl = {.d32=0};
	volatile uint32_t *addr = &(ifxusb_pcd.core_if.dev_global_regs->dctl);

	if (ifxusb_pcd.remote_wakeup_enable)
	{
		if (set)
		{
			dctl.b.rmtwkupsig = 1;
			ifxusb_mreg(addr, 0, dctl.d32);
			IFX_DEBUGPL(DBG_PCD, "Set Remote Wakeup\n");
			mdelay(1);
			ifxusb_mreg(addr, dctl.d32, 0);
			IFX_DEBUGPL(DBG_PCD, "Clear Remote Wakeup\n");
		}
	}
	else
		IFX_DEBUGPL(DBG_PCD, "Remote Wakeup is disabled\n");
	return;
}

/*!
   \brief Gadget API function to Initiates Session Request Protocol (SRP)
          to wakeup the host if no session is in progress. If a session is
          already in progress, but the device is suspended, remote wakeup
          signaling is started.
 */
static int ifxpcd_wakeup(struct usb_gadget *gadget)
{
	unsigned long flags;
	dsts_data_t		dsts;
	gotgctl_data_t	gotgctl;

	IFX_DEBUGPL(DBG_PCDV,"%s(%p)\n", __func__, gadget);

	if (gadget == 0)
		return -ENODEV;
	SPIN_LOCK_IRQSAVE(&ifxusb_pcd.lock, flags);

	/* Check if valid session */
	gotgctl.d32 = ifxusb_rreg(&(ifxusb_pcd.core_if.core_global_regs->gotgctl));
	if (gotgctl.b.bsesvld)
	{
		/* Check if suspend state */
		dsts.d32 = ifxusb_rreg(&(ifxusb_pcd.core_if.dev_global_regs->dsts));
		if (dsts.b.suspsts)
			ifxpcd_remote_wakeup(1);
	}
	SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
	return 0;
}

/*!
   \brief Gadget API function to turn on/off the D+/D- bus, implemented
          using Soft-Disconnect bit
 */
static int ifxpcd_pullup(struct usb_gadget *gadget,int pullup)
{
	unsigned long flags;
	dctl_data_t dctl = {.d32=0};

	IFX_DEBUGPL(DBG_PCDV,"%s(%p)\n", __func__, gadget);
	if (gadget == 0)
		return -ENODEV;

	SPIN_LOCK_IRQSAVE(&ifxusb_pcd.lock, flags);

	dctl.d32=ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->dctl);

	if     (pullup==1)
		dctl.b.sftdiscon=0;
	if     (pullup==0)
		dctl.b.sftdiscon=1;

	ifxusb_wreg(&ifxusb_pcd.core_if.dev_global_regs->dctl,dctl.d32);

	SPIN_UNLOCK_IRQRESTORE(&ifxusb_pcd.lock, flags);
	return 0;
}

static void StopAllInEP(void)
{
	depctl_data_t diepctl = { .d32 = 0};
	gint_data_t gintr_status;
	dctl_data_t dctl = {.d32=0};
	int i;
	uint32_t cntdown;
	uint32_t dir;
	int todo=0;

	IFX_DEBUGPL(DBG_PCDV,"%s()\n", __func__);

	dir=ifxusb_pcd.core_if.hwcfg1.d32;
	for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++,dir>>=2)
	{
		if((dir&0x00000003)==0 || (dir&0x00000003)==1)
		{
			diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl);
			if(diepctl.b.epena)
				todo=1;
		}
	}
	if(!todo)
		return;

	gintr_status.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gintsts);
	if(!gintr_status.b.ginnakeff)
	{
		dctl.d32=0;
		dctl.b.sgnpinnak = 1;

		ifxusb_mreg( &ifxusb_pcd.core_if.dev_global_regs->dctl,0, dctl.d32);

		cntdown=0x0000FFFF;
		do
		{
			gintr_status.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gintsts);
			cntdown--;
		}while(!gintr_status.b.ginnakeff&&cntdown);

		if(!gintr_status.b.ginnakeff)
			IFX_WARN("%s() Can't GINNAKEFF\n",__func__);
	}

	dir=ifxusb_pcd.core_if.hwcfg1.d32;
	for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++,dir>>=2)
	{
		if((dir&0x00000003)==0 || (dir&0x00000003)==1)
		{
			#ifndef __DED_FIFO__
				diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
//				if(diepctl.b.epena)
				if(diepctl.b.epena&&diepctl.b.usbactep) //Howard 101006
				{
					deptsiz_data_t sz;
					int j;
					for(j=0;j< ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; j++)
					{
						diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[j]->diepctl  );
						if(diepctl.b.nextep!=i)
						{
							sz.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[j]->dieptsiz  );
							diepctl.b.nextep=i;
							ifxusb_wreg( &ifxusb_pcd.core_if.in_ep_regs[j]->diepctl, diepctl.d32 );
						}
					}
				}
			#endif

			diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
			if(diepctl.b.usbactep) // Howard 101006
			{
				if(diepctl.b.epena)
				{
					diepctl.b.cnak  = 0;
					diepctl.b.snak  = 1;
					diepctl.b.epena = 0;
					diepctl.b.epdis = 1;
					ifxusb_wreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl, diepctl.d32 );
					cntdown=0x0000FFFF;
					do
					{
						diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
						cntdown--;
					}while(diepctl.b.epena && cntdown);
				}

				diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
				if(diepctl.b.epena)
				{
					diepctl.b.cnak  = 0;
					diepctl.b.snak  = 1;
					diepctl.b.epena = 1;
					diepctl.b.epdis = 1;
					ifxusb_wreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl, diepctl.d32 );
					cntdown=0x0000FFFF;
					do
					{
						diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
						cntdown--;
					}while(diepctl.b.epena && cntdown);
				}

				diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
				if(diepctl.b.epena)
					IFX_WARN( "%s() DISABLING IN EP %d Failed %08X\n",__func__,i,diepctl.d32);

				diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
				if(diepctl.b.epdis)
				{
					diepctl.b.cnak  = 0;
					diepctl.b.snak  = 1;
					diepctl.b.epena = 1;
					diepctl.b.epdis = 1;
					ifxusb_wreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl, diepctl.d32 );
					cntdown=0x0000FFFF;
					do
					{
						diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
						cntdown--;
					}while(diepctl.b.epdis && cntdown);
				}

				diepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.in_ep_regs[i]->diepctl  );
				if(diepctl.b.epena || diepctl.b.epdis)
					IFX_WARN( "%s() IN EP %d In ABNORMAL Condition %08X \n",__func__,i,diepctl.d32);
			}
		}
	}
	dctl.b.sgnpinnak = 0;
	dctl.b.cgnpinnak = 1;
	ifxusb_mreg( &ifxusb_pcd.core_if.dev_global_regs->dctl,0, dctl.d32);
}

static void StopAllOutEP(void)
{
	depctl_data_t doepctl = { .d32 = 0};
	gint_data_t gintr_status;
	dctl_data_t dctl = {.d32=0};
	int i;
	uint32_t cntdown;
	uint32_t dir;
	int todo=0;

	dir=ifxusb_pcd.core_if.hwcfg1.d32;
	dir>>=2;
	for (i = 1; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++,dir>>=2)
//	for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++,dir>>=2)
	{
		if(((dir&0x00000003)==0 || (dir&0x00000003)==2))
		{
			doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
			if(doepctl.b.epena)
				todo=1;
		}
	}
	if(!todo)
		return;

	gintr_status.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gintsts);
	if(!gintr_status.b.goutnakeff)
	{
		dctl.d32=0;
		dctl.b.sgoutnak = 1;

		ifxusb_mreg( &ifxusb_pcd.core_if.dev_global_regs->dctl,0, dctl.d32);

		cntdown=0x0000FFFF;
		do
		{
			gintr_status.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gintsts);
			cntdown--;
		}while(!gintr_status.b.goutnakeff&&cntdown);

//		if(!gintr_status.b.goutnakeff)
//			IFX_WARN( "%s() Can't GOUTNAKEFF\n",__func__);
	}

	dir=ifxusb_pcd.core_if.hwcfg1.d32;
	dir>>=2;
	for (i = 1; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++,dir>>=2)
	{
		if(((dir&0x00000003)==0 || (dir&0x00000003)==2))
		{
			doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
			if(doepctl.b.usbactep)// Howard 101006
			{
				if(doepctl.b.epena)
				{
					doepctl.b.cnak =0;
					doepctl.b.epena=0;
					doepctl.b.epdis=1;
					doepctl.b.snak =1;
					ifxusb_wreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl, doepctl.d32 );
					cntdown=0x0000FFFF;
					do
					{
						doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
						cntdown--;
					}while(doepctl.b.epena && cntdown);
				}
				doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
				if(doepctl.b.epena)
				{
					doepctl.b.cnak =0;
					doepctl.b.epena=1;
					doepctl.b.epdis=1;
					doepctl.b.snak =1;
					ifxusb_wreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl, doepctl.d32 );
					cntdown=0x0000FFFF;
					do
					{
						doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
						cntdown--;
					}while(doepctl.b.epena && cntdown);
				}
				doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
//				if(doepctl.b.epena)
//					IFX_WARN( "%s() DISABLING OUT EP %d Failed %08X\n",__func__,i,doepctl.d32);

				if(doepctl.b.epdis)
				{
					doepctl.b.cnak =0;
					doepctl.b.epena=1;
					doepctl.b.epdis=1;
					doepctl.b.snak =1;
					ifxusb_wreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl, doepctl.d32 );
					cntdown=0x0000FFFF;
					do
					{
						doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
						cntdown--;
					}while(doepctl.b.epdis && cntdown);
				}
				doepctl.d32=ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[i]->doepctl);
//				if(doepctl.b.epena || doepctl.b.epdis )
//					IFX_WARN( "%s() OUT EP %d In ABNORMAL Condition %08X\n",__func__,i,doepctl.d32);
			}
		}
	}
	dctl.b.sgoutnak = 0;
	dctl.b.cgoutnak = 1;
	ifxusb_mreg( &ifxusb_pcd.core_if.dev_global_regs->dctl,0, dctl.d32);
}

/////////////////////////////////////////////////////////////////////////////////
// USB bus signal operations
//   Suspend
//   Resume
//   Reset
/////////////////////////////////////////////////////////////////////////////////
/*!
   \brief This interrupt occurs when a USB Suspend is detected.
 */
void ifxpcd_suspend(void)
{
	int i;


	ifxusb_pcd.enumdone=0;
	#if defined(__GADGET_LED__)
		ifxusb_led_free(&ifxusb_pcd.core_if);
	#endif

	#if 1
		StopAllOutEP();
		ifxusb_flush_rx_fifo_d( &ifxusb_pcd.core_if);
	#endif

	/* Stop All IN (tx) EPs */
	#if 1
		StopAllInEP();
	#endif
	ifxusb_flush_tx_fifo_d( &ifxusb_pcd.core_if, 0x10 );

	/* prevent new request submissions, kill any outstanding requests  */
	for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
	{
		ifxpcd_ep_t *ifxep = &ifxusb_pcd.ifxep[i];

		ifxep->ep_act          =0;
		ifxep->ep_stalled      =1;
		#if !defined(__DED_FIFO__)
			ifxep->ep_gnakdisabling=0;
			ifxep->ep_hold         =0;
		#endif
		ifxep->ep_xfer_stage   =Idle;
		request_nuke(ifxep);
		#if   defined(__UNALIGNED_BUF_ADJ_)
			ifxep->using_aligned_buf=0;
			if(ifxep->aligned_buf)
				ifxusb_free_buf_d(ifxep->aligned_buf);
			ifxep->aligned_buf=NULL;
			ifxep->aligned_buf_len=0;
		#endif //defined(__UNALIGNED_BUF_ADJ_)
	}

	if (ifxusb_pcd.driver && ifxusb_pcd.driver->suspend)
	{
		SPIN_UNLOCK(&ifxusb_pcd.lock);
		ifxusb_pcd.driver->suspend(&ifxusb_pcd.gadget);
		SPIN_LOCK(&ifxusb_pcd.lock);
	}
}

/*!
   \brief This interrupt occurs when a USB Resume is detected.
 */
void ifxpcd_resume(void)
{
	if (ifxusb_pcd.driver && ifxusb_pcd.driver->resume)
	{
		SPIN_UNLOCK(&ifxusb_pcd.lock);
		ifxusb_pcd.driver->resume(&ifxusb_pcd.gadget);
		SPIN_LOCK(&ifxusb_pcd.lock);
	}
}

/*!
   \brief This interrupt occurs when a USB Reset is detected.  When the USB
          Reset Interrupt occurs the device state is set to DEFAULT and the
          EP0 state is set to IDLE.
           - Set the NAK bit for all OUT endpoints (DOEPCTLn.SNAK = 1)
           - Unmask the following interrupt bits
               - DAINTMSK.INEP0 = 1 (Control 0 IN endpoint)
               - DAINTMSK.OUTEP0 = 1 (Control 0 OUT endpoint)
               - DOEPMSK.SETUP = 1
               - DOEPMSK.XferCompl = 1
               - DIEPMSK.XferCompl = 1
               - DIEPMSK.TimeOut = 1
           - Program the following fields in the endpoint specific registers
             for Control OUT EP 0, in order to receive a setup packet
           - DOEPTSIZ0.Packet Count = 3 (To receive up to 3 back to back
             setup packets)
           - DOEPTSIZE0.Transfer Size = 24 Bytes (To receive up to 3 back
             to back setup packets)
           - In DMA mode, DOEPDMA0 Register with a memory address to
             store any setup packets received
           At this point, all the required initialization, except for enabling
           the control 0 OUT endpoint is done, for receiving SETUP packets.
 */
void ifxpcd_usbreset(void)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;

	/* Clear the Remote Wakeup Signalling */
	{
		dctl_data_t dctl = {.d32=0};
		dctl.b.rmtwkupsig = 1;
		ifxusb_mreg( &core_if->dev_global_regs->dctl, dctl.d32, 0 );
	}

	#if 0
		/* Set NAK for all OUT EPs */
		{
			depctl_data_t doepctl = { .d32 = 0};
			int i;
			doepctl.b.snak = 1;
			for (i = 0; i <= core_if->hwcfg2.b.num_dev_ep; i++)
				ifxusb_wreg( &core_if->out_ep_regs[i]->doepctl, doepctl.d32 );
		}
	#endif

	#if 1
		StopAllOutEP();
		ifxusb_flush_rx_fifo_d( &ifxusb_pcd.core_if);
	#endif

	/* Stop All IN (tx) EPs */
	#if 1
		StopAllInEP();
	#endif
	ifxusb_flush_tx_fifo_d( &ifxusb_pcd.core_if, 0x10 );

	ifxpcd_stop();

	/* Flush the Learning Queue */
	#ifndef __DED_FIFO__
	{
		grstctl_t resetctl = { .d32=0 };
		resetctl.b.intknqflsh = 1;
		ifxusb_wreg( &core_if->core_global_regs->grstctl, resetctl.d32);
	}
	#endif

	/* Clear and setup Interrupts */
	{
		daint_data_t daintmsk = { .d32 = 0};
		daintmsk.ep.inep00  = 1;
		daintmsk.ep.outep00 = 1;
		ifxusb_wreg( &core_if->dev_global_regs->daintmsk, daintmsk.d32 );
	}

	{
		doepint_data_t doepmsk = { .d32 = 0};
		doepmsk.b.setup = 1;
		doepmsk.b.xfercompl = 1;
		doepmsk.b.ahberr = 1;
		doepmsk.b.epdisabled = 1;
		#if defined( __DED_FIFO__)
			doepmsk.b.stsphsercvd = 1;
			doepmsk.b.bna = 1;
		#endif
		ifxusb_wreg( &core_if->dev_global_regs->doepmsk, doepmsk.d32 );
	}
	{
		diepint_data_t diepmsk = { .d32 = 0};
		diepmsk.b.xfercompl = 1;
		diepmsk.b.timeout = 1;
		diepmsk.b.epdisabled = 1;
		diepmsk.b.ahberr = 1;
		diepmsk.b.intknepmis = 1;
		#if defined( __DED_FIFO__)
			diepmsk.b.bna = 1;
		#endif
		ifxusb_wreg( &core_if->dev_global_regs->diepmsk, diepmsk.d32 );
	}

	{
		dcfg_data_t dcfg = { .d32=0 };
		/* Reset Device Address */
		dcfg.d32 = ifxusb_rreg( &core_if->dev_global_regs->dcfg);
		dcfg.b.devaddr = 0;
		ifxusb_wreg( &core_if->dev_global_regs->dcfg, dcfg.d32);
	}

	if (ifxusb_pcd.driver && ifxusb_pcd.driver->disconnect)
	{
		SPIN_UNLOCK(&ifxusb_pcd.lock);
		ifxusb_pcd.driver->disconnect(&ifxusb_pcd.gadget);
		SPIN_LOCK(&ifxusb_pcd.lock);
	}
	/* setup EP0 to receive SETUP packets */
	//ep0_out_start();
	{
		gint_data_t gintmsk = { .d32 = 0 };
		gintmsk.b.sofintr = 1;
		ifxusb_mreg( &ifxusb_pcd.core_if.core_global_regs->gintmsk,0, gintmsk.d32);
	}
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////

/*!
   \brief This function initialized the pcd Dp structures to there default state.
 */
void ifxpcd_reinit()
{
	uint32_t i;
	uint32_t hwcfg1;

	IFX_DEBUGPL(DBG_PCDV, "%s\n", __func__);

//	INIT_LIST_HEAD (&ifxusb_pcd.gadget.ep_list);

	ifxusb_pcd.gadget.speed = USB_SPEED_UNKNOWN;

//	INIT_LIST_HEAD (&ifxusb_pcd.gadget.ep0->ep_list);

	hwcfg1 = ifxusb_pcd.core_if.hwcfg1.d32;

	/* Initialize the EP structures.*/
	for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++,hwcfg1 >>= 2)
	{
		ifxpcd_ep_t *ifxep = &ifxusb_pcd.ifxep[i];

		/* Init EP structure */
		ifxep->desc             =0;
		ifxep->ep_stalled       =0;
		#if !defined(__DED_FIFO__)
			ifxep->ep_gnakdisabling=0;
			ifxep->ep_hold         =0;
		#endif
		ifxep->ep_xfer_stage    =Idle;
		ifxep->is_in            =0;
		ifxep->ep_act           =0;

		/* Control until ep is actvated */
		ifxep->type             = IFXUSB_EP_TYPE_CTRL;
		ifxep->mps              = MAX_PACKET_SIZE;
		ifxep->stall_clear_flag = 0;
		#ifdef __DESC_DMA__
			if (ifxep->dma_desc)
				ep_free_desc ((uint32_t *)(ifxep->dma_desc));
			ifxep->dma_desc=0;
		#endif
		#if   defined(__UNALIGNED_BUF_ADJ__)
			ifxep->using_aligned_buf=0;
			if(ifxep->aligned_buf)
				ifxusb_free_buf_d(ifxep->aligned_buf);
			ifxep->aligned_buf=NULL;
			ifxep->aligned_buf_len=0;
		#endif

		#if defined(__DED_FIFO__) || defined(__DED_INTR__)
			ifxep->tx_fifo_num = 0;
		#endif

		ifxep->sysep.maxpacket = MAX_PACKET_SIZE;

		if(ifxep->num==0)
		{
			/* dont add ep0 to the list.  There is a ep0 pointer.*/
			ifxusb_pcd.ep0state                 = EP0_DISCONNECT;
			ifxusb_pcd.ifxep[0].sysep.maxpacket = MAX_EP0_SIZE;
			ifxusb_pcd.ifxep[0].mps             = MAX_EP0_SIZE;
			ifxusb_pcd.ifxep[0].ep_act          =1;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Tasklet structures
/////////////////////////////////////////////////////////////////////////////////
static void start_xfer_tasklet_func (unsigned long data)
{
	restart_requests();
}

static struct tasklet_struct start_xfer_tasklet =
{
	.next = NULL,
	.state = 0,
	.count = ATOMIC_INIT(0),
	.func = start_xfer_tasklet_func,
	.data = 0,
};

/////////////////////////////////////////////////////////////////////////////////
// PCD initialization and removal
/////////////////////////////////////////////////////////////////////////////////

static void ifxpcd_gadget_release(struct device *_dev)
{
	IFX_DEBUGPL(DBG_PCDV,"%s(%p)\n", __func__, _dev);
}

/*!
 \brief This function is called when the Device is disconnected.  It stops
        any active requests and informs the Gadget driver of the
        disconnect.
 */
// This entry is called by RESET ..HOWARD
void ifxpcd_stop()
{
	int i;
	gint_data_t intr_mask = {.d32 = 0};

	IFX_DEBUGPL(DBG_PCDV, "%s() \n", __func__ );

	#if defined(__GADGET_LED__)
		ifxusb_led_free(&ifxusb_pcd.core_if);
	#endif

	/* don't disconnect drivers more than once */
	if (ifxusb_pcd.ep0state == EP0_DISCONNECT)
	{
		IFX_DEBUGPL(DBG_ANY, "%s() Already Disconnected\n", __func__ );
		return;
	}
	ifxusb_pcd.ep0state = EP0_DISCONNECT;

	/* Disable the NP Tx Fifo Empty Interrupt. */
	intr_mask.b.nptxfempty = 1;
	ifxusb_mreg(&ifxusb_pcd.core_if.core_global_regs->gintmsk, intr_mask.d32, 0);

	/* Flush the FIFOs */
	ifxusb_flush_tx_fifo_d( &ifxusb_pcd.core_if, 0x10);
	ifxusb_flush_rx_fifo_d( &ifxusb_pcd.core_if );

	/* prevent new request submissions, kill any outstanding requests  */
	for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
	{
		ifxpcd_ep_t *ifxep = &ifxusb_pcd.ifxep[i];

		ifxep->ep_act          =0;
		ifxep->ep_stalled      =1;
		#if !defined(__DED_FIFO__)
			ifxep->ep_gnakdisabling=0;
			ifxep->ep_hold         =0;
		#endif
		ifxep->ep_xfer_stage   =Idle;
		request_nuke(ifxep);

		#if   defined(__UNALIGNED_BUF_ADJ_)
			ifxep->using_aligned_buf=0;
			if(ifxep->aligned_buf)
				ifxusb_free_buf_d(ifxep->aligned_buf);
			ifxep->aligned_buf=NULL;
			ifxep->aligned_buf_len=0;
		#endif //defined(__UNALIGNED_BUF_ADJ_)
	}

	/* report disconnect; the driver is already quiesced */
	if (ifxusb_pcd.driver && ifxusb_pcd.driver->disconnect)
	{
		SPIN_UNLOCK(&ifxusb_pcd.lock);
		ifxusb_pcd.driver->disconnect(&ifxusb_pcd.gadget);
		SPIN_LOCK(&ifxusb_pcd.lock);
	}
}

/*!
  \brief This function is the top level PCD interrupt handler.
 */
irqreturn_t ifxpcd_irq(int _irq, void *_dev)
{
	int32_t retval=0;
	//mask_and_ack_ifx_irq (_irq);
	retval = ifxpcd_handle_intr();
	return IRQ_RETVAL(retval);
}

/*!
   \brief This function initialized the PCD portion of the driver.
 */
int ifxpcd_init()
{
	int retval = 0;

	struct device *dev = ifxusb_pcd.dev;

	static const char * names[] =
	{
	    "ep0", "ep1", "ep2", "ep3",
	    "ep4", "ep5", "ep6", "ep7",
	    "ep8", "ep9", "ep10", "ep11",
	    "ep12", "ep13", "ep14", "ep15",
	};

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	IFX_DEBUGPL(DBG_PCDV,"%s\n",__func__);

	init_timer(&ifxusb_pcd.device_probe_timer);
	init_timer(&ifxusb_pcd.device_autoprobe_timer);
	ifxusb_pcd.probe_sec = 5;
	ifxusb_pcd.autoprobe_sec = 30;

	/* Clear PCD structure */
	spin_lock_init( &ifxusb_pcd.lock );
	ifxusb_pcd.gadget.name = ifxusb_pcd.core_if.core_name;

	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
		strcpy(ifxusb_pcd.gadget.dev.bus_id, "gadget");
	#endif

	ifxusb_pcd.gadget.dev.parent = dev->parent;
	ifxusb_pcd.gadget.dev.release = ifxpcd_gadget_release;

	ifxusb_pcd.gadget.ops = &ifxpcd_ops;

	/* If the module is set to FS or if the PHY_TYPE is FS then the gadget
	 * should not report as dual-speed capable.  replace the following line
	 * with the block of code below it once the software is debugged for
	 * this.  If is_dualspeed = 0 then the gadget driver should not report
	 * a device qualifier descriptor when queried. */

	if (ifxusb_pcd.core_if.params.speed == IFXUSB_PARAM_SPEED_FULL)
		ifxusb_pcd.gadget.is_dualspeed = 0;
	else
		ifxusb_pcd.gadget.is_dualspeed = 1;

	ifxusb_pcd.gadget.is_otg = 0;
	ifxusb_pcd.driver = 0;

	INIT_LIST_HEAD (&ifxusb_pcd.gadget.ep_list);
	ifxusb_pcd.gadget.ep0 = &ifxusb_pcd.ifxep[0].sysep;
	INIT_LIST_HEAD (&ifxusb_pcd.gadget.ep0->ep_list);

	{
		uint32_t i;
		for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep && i < MAX_EPS_CHANNELS; i++)
		{
			ifxpcd_ep_t *ifxep = &ifxusb_pcd.ifxep[i];
			ifxep->num              = i;
			ifxep->ifxpcd           = &ifxusb_pcd;

			#ifdef __DESC_DMA__
				ifxep->dma_desc=0;
			#endif

			ifxep->sysep.name       = names[i];
			ifxep->sysep.ops        = &ifxpcd_ep_ops;

			#if   defined(__UNALIGNED_BUF_ADJ__)
				ifxusb_pcd.ifxep[i].using_aligned_buf=0;
				ifxusb_pcd.ifxep[i].aligned_buf      =NULL;
				ifxusb_pcd.ifxep[i].aligned_buf_len  =0;
			#endif
			#if defined(__DED_FIFO__) || defined(__DED_INTR__)
				ifxep->tx_fifo_num = 0;
			#endif
			INIT_LIST_HEAD (&ifxep->reqs);
			INIT_LIST_HEAD (&ifxep->queue);
			INIT_LIST_HEAD (&ifxep->queue_cmpt);
			#if defined(__GADGET_TASKLET_TX__) || defined(__GADGET_TASKLET_RX__)
				spin_lock_init( &ifxep->cmp_lock );
			#endif
			if(ifxep->num)
				list_add_tail (&ifxep->sysep.ep_list, &ifxusb_pcd.gadget.ep_list);
			else
			{
				/* dont add ep0 to the list.  There is a ep0 pointer.*/
				ifxusb_pcd.ep0state                 = EP0_DISCONNECT;
				ifxusb_pcd.ifxep[0].sysep.maxpacket = MAX_EP0_SIZE;
				ifxusb_pcd.ifxep[0].mps             = MAX_EP0_SIZE;
				ifxusb_pcd.ifxep[0].ep_act          =1;
			}
		}
	}

	ifxusb_pcd.setup_pkt=(uint32_t *)(KSEG1ADDR(&ifxusb_pcd.setup_pkt_buf[0]));
	ifxusb_pcd.status_buf = (uint16_t *)(ifxusb_alloc_buf_d (MAX_EP0_STATUS_BUFFER, 1));
	if (!ifxusb_pcd.status_buf)
	{
		ifxusb_free_buf_d(ifxusb_pcd.status_buf);
		ifxusb_pcd.status_buf=0;
		return -ENOMEM;
	}

	#if defined(__DESC_DMA__)
		ifxusb_pcd.setup_desc    = ep_alloc_desc(3);
		ifxusb_pcd.in_desc       = ep_alloc_desc(1);
		if (!ifxusb_pcd.setup_desc || !ifxusb_pcd.in_desc)
		{
			ifxusb_free_buf_d(ifxusb_pcd.status_buf);
			ifxusb_pcd.status_buf=0;
			if (ifxusb_pcd.setup_desc ) ep_free_desc ((uint32_t *)(ifxusb_pcd.setup_desc));
			if (ifxusb_pcd.in_desc    ) ep_free_desc ((uint32_t *)(ifxusb_pcd.in_desc   ));
			ifxusb_pcd.setup_desc=0;
			ifxusb_pcd.in_desc   =0;
			return -ENOMEM;
		}
		{
			desc_sts_data_t dma_desc_sts;

			dma_desc_sts.b.bs       = BS_HOST_READY;
			dma_desc_sts.b.l        = 1;
			dma_desc_sts.b.ioc      = 0;
			dma_desc_sts.b.mtrf     = 0;
			dma_desc_sts.b.bytes    = ifxusb_pcd.ifxep[0].mps;

			write_desc((uint32_t *)(ifxusb_pcd.setup_desc + 0),dma_desc_sts.d32);
			write_desc((uint32_t *)(ifxusb_pcd.setup_desc + 1),(uint32_t)(CPHYSADDR(ifxusb_pcd.setup_pkt)));
			write_desc((uint32_t *)(ifxusb_pcd.setup_desc + 2),dma_desc_sts.d32);
			write_desc((uint32_t *)(ifxusb_pcd.setup_desc + 3),(uint32_t)(CPHYSADDR(ifxusb_pcd.setup_pkt)));

			dma_desc_sts.b.bs       = BS_HOST_READY;
			dma_desc_sts.b.l        = 1;
			dma_desc_sts.b.ioc      = 0;
			dma_desc_sts.b.mtrf     = 0;
			dma_desc_sts.b.bytes    = 0;
			write_desc((uint32_t *)(ifxusb_pcd.setup_desc + 4),dma_desc_sts.d32);
			write_desc((uint32_t *)(ifxusb_pcd.setup_desc + 5),0);
		}
	#endif

	/* Register the gadget device */
	retval = device_register( &ifxusb_pcd.gadget.dev );

	if(retval)
	{
		ifxusb_free_buf_d(ifxusb_pcd.status_buf);
		ifxusb_pcd.status_buf=0;
		#if defined(__DESC_DMA__)
			if (ifxusb_pcd.setup_desc) ep_free_desc ((uint32_t *)(ifxusb_pcd.setup_desc));
			if (ifxusb_pcd.in_desc   ) ep_free_desc ((uint32_t *)(ifxusb_pcd.in_desc   ));
			ifxusb_pcd.setup_desc=0;
			ifxusb_pcd.in_desc   =0;
		#endif
		return -ENOMEM;
	}

	ifxpcd_reinit();

	/* Setup interupt handler */
	IFX_DEBUGPL( DBG_ANY, "registering handler for irq%d\n", ifxusb_pcd.core_if.irq);
	retval = request_irq(ifxusb_pcd.core_if.irq, &ifxpcd_irq, IRQF_DISABLED, ifxusb_pcd.gadget.name, &ifxusb_pcd);
	if (retval != 0)
	{
		IFX_ERROR("request of irq%d failed\n", ifxusb_pcd.core_if.irq);
		ifxusb_free_buf_d(ifxusb_pcd.status_buf);
		#if defined(__DESC_DMA__)
			if (ifxusb_pcd.setup_desc) ep_free_desc ((uint32_t *)(ifxusb_pcd.setup_desc));
			if (ifxusb_pcd.in_desc   ) ep_free_desc ((uint32_t *)(ifxusb_pcd.in_desc   ));
			ifxusb_pcd.setup_desc=0;
			ifxusb_pcd.in_desc   =0;
		#endif
		return -EBUSY;
	}
	/* Initialize tasklet */
	ifxusb_pcd.start_xfer_tasklet = &start_xfer_tasklet;
	return 0;
}

int usb_gadget_unregister_driver(struct usb_gadget_driver *_driver);

/*!
   \brief Cleanup the PCD.
 */
void ifxpcd_remove()
{
	int i;
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	IFX_DEBUGPL(DBG_PCDV, "%s\n", __func__);

	/* Free the IRQ */
	free_irq( ifxusb_pcd.core_if.irq, &ifxusb_pcd );

	/* start with the driver above us */
	if (ifxusb_pcd.driver)
	{
		/* should have been done already by driver model core */
		IFX_WARN("driver '%s' is still registered\n", ifxusb_pcd.driver->driver.name);
		usb_gadget_unregister_driver( ifxusb_pcd.driver);
	}

//	device_unregister(&ifxusb_pcd.gadget.dev);

	#if (defined(__UNALIGNED_BUF_ADJ__) || defined(__DESC_DMA__))
		for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
		{
			ifxpcd_ep_t *ifxep = &ifxusb_pcd.ifxep[i];

			#if   defined(__UNALIGNED_BUF_ADJ__)
				ifxep->using_aligned_buf=0;
				if(ifxep->aligned_buf)
					ifxusb_free_buf_d(ifxep->aligned_buf);
				ifxep->aligned_buf=NULL;
				ifxep->aligned_buf_len=0;
			#endif
			#if defined(__DESC_DMA__)
				if (ifxep->dma_desc)
					ep_free_desc ((uint32_t *)(ifxep->dma_desc));
				ifxep->dma_desc=0;
			#endif
		}
	#endif

	if(ifxusb_pcd.status_buf)
		ifxusb_free_buf_d(ifxusb_pcd.status_buf);
	ifxusb_pcd.status_buf=0;

	#if defined(__DESC_DMA__)
		if (ifxusb_pcd.setup_desc) ep_free_desc ((uint32_t *)(ifxusb_pcd.setup_desc));
		if (ifxusb_pcd.in_desc   ) ep_free_desc ((uint32_t *)(ifxusb_pcd.in_desc   ));
		ifxusb_pcd.setup_desc=0;
		ifxusb_pcd.in_desc   =0;
	#endif
	ifxusb_power_off_d (&ifxusb_pcd.core_if);
}

/////////////////////////////////////////////////////////////////////////////////
// Gadget driver registration and unregistration
/////////////////////////////////////////////////////////////////////////////////

/*!
   \brief This function registers a gadget driver with the PCD.
          When a driver is successfully registered, it will receive control
          requests including set_configuration(), which enables non-control
          requests.  then usb traffic follows until a suspend is reported.
 */
int usb_gadget_register_driver(struct usb_gadget_driver *_driver)
{
	int retval;

	IFX_DEBUGPL(DBG_PCD, "registering gadget driver '%s'\n", _driver->driver.name);

	if (!_driver ||
	     _driver->speed == USB_SPEED_UNKNOWN ||
	    !_driver->bind ||
//	    !_driver->unbind ||
	    !_driver->disconnect ||
	    !_driver->setup)
	{
		IFX_DEBUGPL(DBG_PCDV,"EINVAL\n");
		return -EINVAL;
	}
	if (ifxusb_pcd.driver != 0)
	{
		IFX_DEBUGPL(DBG_PCDV,"EBUSY (%p)\n", ifxusb_pcd.driver);
		return -EBUSY;
	}

	/* hook up the driver */
	ifxusb_pcd.driver = _driver;

	ifxusb_pcd.gadget.dev.driver = &_driver->driver;

	IFX_DEBUGPL(DBG_PCD, "bind to driver %s\n", _driver->driver.name);
	retval = _driver->bind(&ifxusb_pcd.gadget);
	if (retval)
	{
		IFX_ERROR("bind to driver %s --> error %d\n",
                           _driver->driver.name, retval);
		ifxusb_pcd.driver = 0;
		ifxusb_pcd.gadget.dev.driver = 0;
		return retval;
	}
	IFX_DEBUGPL(DBG_ANY, "registered gadget driver '%s'\n",
	                _driver->driver.name);

	{
		dctl_data_t dctl = {.d32=0};

		dctl.d32=ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->dctl);
		dctl.b.sftdiscon=1;
		ifxusb_wreg(&ifxusb_pcd.core_if.dev_global_regs->dctl,dctl.d32);
	}

	ifxusb_dev_enable_interrupts(&ifxusb_pcd.core_if);
	ifxusb_enable_global_interrupts_d(&ifxusb_pcd.core_if);
	ifxusb_phy_power_on_d (&ifxusb_pcd.core_if);
		mdelay(100);

	{
		dctl_data_t dctl = {.d32=0};

		dctl.d32=ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->dctl);
		dctl.b.sftdiscon=0;
		ifxusb_wreg(&ifxusb_pcd.core_if.dev_global_regs->dctl,dctl.d32);
	}

	return 0;
}
EXPORT_SYMBOL(usb_gadget_register_driver);

/*!
   \brief This function unregisters a gadget driver
 */
int usb_gadget_unregister_driver(struct usb_gadget_driver *_driver)
{
	IFX_DEBUGPL(DBG_PCDV,"%s(%p)\n", __func__, _driver);

	if (_driver == 0 || _driver != ifxusb_pcd.driver)
	{
		IFX_DEBUGPL( DBG_ANY, "%s Return(%d): driver?\n", __func__, -EINVAL);
		return -EINVAL;
	}

	_driver->unbind(&ifxusb_pcd.gadget);
	ifxusb_pcd.driver = 0;

	IFX_DEBUGPL(DBG_ANY, "unregistered driver '%s'\n", _driver->driver.name);
	return 0;
}

EXPORT_SYMBOL(usb_gadget_unregister_driver);


/////////////////////////////////////////////////////////////////////////////////
// Hardware level EP activation and deactivation
/////////////////////////////////////////////////////////////////////////////////
/*!
   \brief This function enables EP0 OUT to receive SETUP packets and configures EP0
          IN for transmitting packets.  It is normally called when the
          "Enumeration Done" interrupt occurs.
          Note: Hardware level
 */
void ifxpcd_ep0_activate()
{
	ifxusb_core_if_t *core_if=&ifxusb_pcd.core_if;
	dsts_data_t   dsts;
	depctl_data_t diepctl;
	depctl_data_t doepctl;
	dctl_data_t dctl ={.d32=0};

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );

	/* Read the Device Status and Endpoint 0 Control registers */
	dsts.d32    = ifxusb_rreg(&core_if->dev_global_regs->dsts);
	diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[0]->diepctl);
	doepctl.d32 = ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);

	/* Set the MPS of the IN EP based on the enumeration speed */
	switch (dsts.b.enumspd)
	{
		case IFXUSB_DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ:
		case IFXUSB_DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ:
		case IFXUSB_DSTS_ENUMSPD_FS_PHY_48MHZ:
			diepctl.b.mps = IFXUSB_DEP0CTL_MPS_64;
			break;
		case IFXUSB_DSTS_ENUMSPD_LS_PHY_6MHZ:
			diepctl.b.mps = IFXUSB_DEP0CTL_MPS_8;
			break;
	}

	ifxusb_wreg(&core_if->in_ep_regs[0]->diepctl, diepctl.d32);

	/* Enable OUT EP for receive */
	doepctl.b.epena = 1;
	ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);

	#ifdef __VERBOSE__
		IFX_DEBUGPL(DBG_PCDV,"doepctl0=%0x\n", ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl));
		IFX_DEBUGPL(DBG_PCDV,"diepctl0=%0x\n", ifxusb_rreg(&core_if->in_ep_regs [0]->diepctl));
	#endif

	dctl.b.cgnpinnak = 1;
	ifxusb_mreg(&core_if->dev_global_regs->dctl, dctl.d32, dctl.d32);
	IFX_DEBUGPL(DBG_PCDV,"dctl=%0x\n", ifxusb_rreg(&core_if->dev_global_regs->dctl));
	IFX_DEBUGPL(DBG_PCDV,"DAINTMSK=%0x\n", ifxusb_rreg(&core_if->dev_global_regs->daintmsk));
}

/*!
   \brief This function activates an EP.  The Device EP control register for
          the EP is configured as defined in the ep structure.
          Note: for EP0, call ifxpcd_ep0_activate() instead
          Note: Hardware level
 */
void ifxpcd_ep_activate(ifxpcd_ep_t *_ifxep)
{
	ifxusb_core_if_t *core_if=&ifxusb_pcd.core_if;
	depctl_data_t depctl;
	volatile uint32_t *addr;
	daint_data_t daintmsk ={.d32=0};

	IFX_DEBUGPL(DBG_PCDV, "%s() EP%d-%s\n", __func__, _ifxep->num,(_ifxep->is_in?"IN":"OUT"));

	if(_ifxep->num==0)
	{
		IFX_WARN(" Manually activate EP0?\n");
		ifxpcd_ep0_activate();
		return;
	}

	/* Read DEPCTLn register */
	if (_ifxep->is_in)
	{
		addr = &core_if->in_ep_regs[_ifxep->num]->diepctl;
		daintmsk.eps.in = 1<<_ifxep->num;
	}
	else
	{
		addr = &core_if->out_ep_regs[_ifxep->num]->doepctl;
		daintmsk.eps.out = 1<<_ifxep->num;
	}
	/* If the EP is already active don't change the EP Control register. */
	depctl.d32 = ifxusb_rreg(addr);
	if (!depctl.b.usbactep)
	{
		depctl.b.mps    = _ifxep->mps;
		depctl.b.eptype = _ifxep->type;

		#if defined(__DED_FIFO__ ) || defined(__DED_INTR__)
			if (_ifxep->is_in)
				depctl.b.txfnum = _ifxep->tx_fifo_num;
		#else
			if (_ifxep->is_in)
				depctl.b.txfnum = 0;
		#endif
		if (_ifxep->type != IFXUSB_EP_TYPE_ISOC)
			depctl.b.setd0pid = 1;

		depctl.b.epdis=depctl.b.epena;
		depctl.b.snak = 1;
		depctl.b.cnak = 0;
		depctl.b.usbactep = 1;

		ifxusb_wreg(addr, depctl.d32);
	}
	/* Enable the Interrupt for this EP */
	ifxusb_mreg(&core_if->dev_global_regs->daintmsk, 0, daintmsk.d32);
	_ifxep->stall_clear_flag = 0;
}

/*!
   \brief This function deactivates an EP.  This is done by clearing the USB Active
          EP bit in the Device EP control register.
          Note: This function is not used for EP0. EP0 cannot be deactivated.
          Note: Hardware level
 */
void ifxpcd_ep_deactivate(ifxpcd_ep_t *_ifxep)
{
	ifxusb_core_if_t *core_if=&ifxusb_pcd.core_if;
	depctl_data_t depctl ={.d32 = 0};
	volatile uint32_t *addr;
	daint_data_t daintmsk ={.d32=0};

	IFX_DEBUGPL(DBG_PCDV, "%s() EP%d-%s\n", __func__, _ifxep->num,
	           (_ifxep->is_in?"IN":"OUT"));

	if(_ifxep->num==0)  // EP0 not allowed to be deactivated
		return;

	/* Read DEPCTLn register */
	if (_ifxep->is_in)
	{
		addr = &core_if->in_ep_regs[_ifxep->num]->diepctl;
		daintmsk.eps.in = 1<<_ifxep->num;
	}
	else
	{
		addr = &core_if->out_ep_regs[_ifxep->num]->doepctl;
		daintmsk.eps.out = 1<<_ifxep->num;
	}

	depctl.d32 = ifxusb_rreg(addr);
	if(depctl.b.usbactep)
	{
		depctl.b.usbactep = 0;
		depctl.b.epdis = depctl.b.epena;
		depctl.b.epena = 0;
		ifxusb_wreg(addr, depctl.d32);
	}

	/* Disable the Interrupt for this EP */
	ifxusb_mreg(&core_if->dev_global_regs->daintmsk, daintmsk.d32, 0);
}

