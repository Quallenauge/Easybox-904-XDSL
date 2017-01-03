/*****************************************************************************
 **   FILE NAME       : ifxpcd_cmpt.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : Routines to deal with CMPT of EP transfer
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
 \file ifxpcd_cmpt.c
 \ingroup IFXUSB_DRIVER_V3
 \brief Routines to deal with CMPT of EP transfer
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


//////////////////////////////////////////////////////////////////

/*!
  \brief This function completes a request.  It call's the request call back.
 */
static void request_done_ep0(ifxpcd_request_t *_ifxreq, int _status)
{
	IFX_DEBUGPL(DBG_PCDV, "%s(%p)\n", __func__,_ifxreq);
	if(!_ifxreq)
	{
		IFX_ERROR("%s() %d invalid _ifxreq\n",__func__,__LINE__);
		return;
	}
	_ifxreq->sysreq.status = _status;

	list_del_init(&_ifxreq->trq);

	#ifdef __DO_PCD_UNLOCK__
		SPIN_UNLOCK(&ifxusb_pcd.lock);
	#endif

	if(_ifxreq->sysreq.complete)
		_ifxreq->sysreq.complete(&ifxusb_pcd.ifxep[0].sysep, &_ifxreq->sysreq);

	#ifdef __DO_PCD_UNLOCK__
		SPIN_LOCK(&ifxusb_pcd.lock);
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLEAR_OUT_EP_INTR(__epnum,__intr) \
	do { \
		doepint_data_t doepint = {.d32=0}; \
		doepint.b.__intr = 1; \
		ifxusb_wreg(&ifxusb_pcd.core_if.out_ep_regs[__epnum]->doepint, doepint.d32); \
	} while (0)

#define CLEAR_IN_EP_INTR(__epnum,__intr) \
	do { \
		diepint_data_t diepint = {.d32=0}; \
		diepint.b.__intr = 1; \
		ifxusb_wreg(&ifxusb_pcd.core_if.in_ep_regs[__epnum]->diepint, diepint.d32); \
	} while (0)

#define MASK_OUT_EP_INTR(__intr) \
	do { \
		doepint_data_t doepmsk = {.d32=0}; \
		doepmsk.b.__intr = 1; \
		ifxusb_mreg(&ifxusb_pcd.core_if.dev_global_regs->doepmsk, doepmsk.d32,0); \
	} while (0)

#define MASK_IN_EP_INTR(__intr) \
	do { \
		diepint_data_t diepmsk = {.d32=0}; \
		diepmsk.b.__intr = 1; \
		ifxusb_mreg(&ifxusb_pcd.core_if.dev_global_regs->diepmsk, diepmsk.d32,0); \
	} while (0)



#if defined(__GADGET_TASKLET_TX__) || defined(__GADGET_TASKLET_RX__)
	void request_done_tasklet(unsigned long data)
	{
		ifxpcd_ep_t *_ifxep = (ifxpcd_ep_t *)data;
		ifxpcd_request_t   *req;
		unsigned long flags=0;

		int k=10;
		_ifxep->cmpt_tasklet_in_process=1;

		while (k)
		{
			SPIN_LOCK_IRQSAVE(&_ifxep->cmp_lock,flags);
			if(_ifxep->queue_cmpt.next != &_ifxep->queue_cmpt)
			{
				req = list_entry(_ifxep->queue_cmpt.next, ifxpcd_request_t,trq);
				list_del_init(&req->trq);
				SPIN_UNLOCK_IRQRESTORE(&_ifxep->cmp_lock,flags);

				if(req->sysreq.complete)
					req->sysreq.complete(&_ifxep->sysep, &req->sysreq);
				else
				{
					#ifdef __req_num_dbg__
						IFX_ERROR("%s() no complete EP%d Req%d\n",__func__,_ifxep->num, req->reqid);
					#else
						IFX_ERROR("%s() no complete EP%d Req %p\n",__func__,_ifxep->num, req);
					#endif
				}
			}
			else
			{
				SPIN_UNLOCK_IRQRESTORE(&_ifxep->cmp_lock,flags);
				break;
			}
			k--;
		}

		if(!list_empty(&_ifxep->queue_cmpt))
		{
			#ifdef __GADGET_TASKLET_HIGH__
				tasklet_hi_schedule(&_ifxep->cmpt_tasklet);
			#else
				tasklet_schedule(&_ifxep->cmpt_tasklet);
			#endif
		}
		else
			_ifxep->cmpt_tasklet_in_process=0;
	}
#endif

/*!
  \brief This function completes a request.  It call's the request call back.
 */

void request_done(ifxpcd_ep_t *_ifxep, ifxpcd_request_t *_ifxreq, int _status)
{
	IFX_DEBUGPL(DBG_PCDV, "%s(%p,%p)\n", __func__, _ifxep,_ifxreq);

	if(!_ifxep)
	{
		IFX_ERROR("%s() %d invalid _ifxep\n",__func__,__LINE__);
		return;
	}
	if(!_ifxreq)
	{
		IFX_ERROR("%s() %d invalid _ifxreq\n",__func__,__LINE__);
		return;
	}

	if(_ifxep->num==0)
	{
		request_done_ep0(_ifxreq, _status);
		return;
	}

	_ifxreq->sysreq.status = _status;

	if(_ifxep->type==IFXUSB_EP_TYPE_INTR)
	{
		list_del_init(&_ifxreq->trq);

		if(_ifxreq->sysreq.complete)
		{
			#ifdef __DO_PCD_UNLOCK__
				SPIN_UNLOCK(&ifxusb_pcd.lock);
			#endif

			_ifxreq->sysreq.complete(&_ifxep->sysep, &_ifxreq->sysreq);

			#ifdef __DO_PCD_UNLOCK__
				SPIN_LOCK(&ifxusb_pcd.lock);
			#endif
		}

	}
	else if(_ifxep->is_in) // Tx
	{
		#if defined(__GADGET_TASKLET_TX__)
			list_del_init(&_ifxreq->trq);
			list_add_tail(&_ifxreq->trq, &_ifxep->queue_cmpt);

			if(!_ifxreq->sysreq.no_interrupt && !_ifxep->cmpt_tasklet_in_process)
			{
				#ifdef __GADGET_TASKLET_HIGH__
					tasklet_hi_schedule(&_ifxep->cmpt_tasklet);
				#else
					tasklet_schedule(&_ifxep->cmpt_tasklet);
				#endif
			}
		#else
			list_del_init(&_ifxreq->trq);

			if(!_ifxreq->sysreq.no_interrupt)
			{
				while (!list_empty(&_ifxep->queue_cmpt))
				{
					ifxpcd_request_t   *req;
					req = list_entry(_ifxep->queue_cmpt.next, ifxpcd_request_t,trq);
					list_del_init(&req->trq);

					if(req->sysreq.complete)
					{
						#ifdef __DO_PCD_UNLOCK__
							SPIN_UNLOCK(&ifxusb_pcd.lock);
						#endif
						req->sysreq.complete(&_ifxep->sysep, &req->sysreq);
						#ifdef __DO_PCD_UNLOCK__
							SPIN_LOCK(&ifxusb_pcd.lock);
						#endif
					}
					else
					{
						#ifdef __req_num_dbg__
							IFX_ERROR("%s() no complete EP%d Req%d\n",__func__,_ifxep->num, req->reqid);
						#else
							IFX_ERROR("%s() no complete EP%d Req %p\n",__func__,_ifxep->num, req);
						#endif
					}
				}

				if(_ifxreq->sysreq.complete)
				{
					#ifdef __DO_PCD_UNLOCK__
						SPIN_UNLOCK(&ifxusb_pcd.lock);
					#endif
					_ifxreq->sysreq.complete(&_ifxep->sysep, &_ifxreq->sysreq);
					#ifdef __DO_PCD_UNLOCK__
						SPIN_LOCK(&ifxusb_pcd.lock);
					#endif
				}
				else
				{
					#ifdef __req_num_dbg__
						IFX_ERROR("%s() no complete EP%d Req%d\n",__func__,_ifxep->num, _ifxreq->reqid);
					#else
						IFX_ERROR("%s() no complete EP%d Req %p\n",__func__,_ifxep->num, _ifxreq);
					#endif
				}

			}
			else
				list_add_tail(&_ifxreq->trq, &_ifxep->queue_cmpt);
		#endif
	}
	else  // Rx
	{
		#if defined(__GADGET_TASKLET_RX__)
			if(list_empty(&_ifxep->queue)) // Rx Empty, Reuse
			{
				_ifxreq->sysreq.actual=0;
				_ifxreq->sysreq.status=0;
			}
			else if(!_ifxreq->sysreq.no_interrupt && !_ifxep->cmpt_tasklet_in_process)
			{
				_ifxep->cmpt_tasklet_in_process=1;
				list_move_tail(&_ifxreq->trq, &_ifxep->queue_cmpt);
				#ifdef __GADGET_TASKLET_HIGH__
					tasklet_hi_schedule(&_ifxep->cmpt_tasklet);
				#else
					tasklet_schedule(&_ifxep->cmpt_tasklet);
				#endif
			}
			else
				list_move_tail(&_ifxreq->trq, &_ifxep->queue_cmpt);
		#else
			if(!_ifxreq->sysreq.no_interrupt)
			{
				ifxpcd_request_t   *req2;

				while (!list_empty(&_ifxep->queue_cmpt))
				{
					req = list_entry(_ifxep->queue_cmpt.next, ifxpcd_request_t,trq);
					list_del_init(&req->trq);

					if(req->sysreq.complete)
					{
						#ifdef __DO_PCD_UNLOCK__
							SPIN_UNLOCK(&ifxusb_pcd.lock);
						#endif
						req->sysreq.complete(&_ifxep->sysep, &req->sysreq);
						#ifdef __DO_PCD_UNLOCK__
							SPIN_LOCK(&ifxusb_pcd.lock);
						#endif
					}
					else
					{
					#ifdef __req_num_dbg__
						IFX_ERROR("%s() no complete EP%d Req%d\n",__func__,_ifxep->num, req->reqid);
					#else
						IFX_ERROR("%s() no complete EP%d Req %p\n",__func__,_ifxep->num, req);
					#endif
					}
				}

				if(list_empty(&_ifxep->queue) // Rx Empty, Reuse
				{
					_ifxreq->sysreq.actual=0;
					_ifxreq->sysreq.status=0;
					list_add_tail(&_ifxreq->trq, &_ifxep->queue);
				}
				else
				{
					if(_ifxreq->sysreq.complete)
					{
						#ifdef __DO_PCD_UNLOCK__
							SPIN_UNLOCK(&ifxusb_pcd.lock);
						#endif
						_ifxreq->sysreq.complete(&_ifxep->sysep, &_ifxreq->sysreq);
						#ifdef __DO_PCD_UNLOCK__
							SPIN_LOCK(&ifxusb_pcd.lock);
						#endif
					}
					else
					{
						#ifdef __req_num_dbg__
							IFX_ERROR("%s() no complete EP%d Req%d\n",__func__,_ifxep->num, req->reqid);
						#else
							IFX_ERROR("%s() no complete EP%d Req %p\n",__func__,_ifxep->num, req);
						#endif
					}
				}
			}
		#endif
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 \brief Handle EPn CMLT Interrupt of Tx Transfer
 */
void complete_ep_tx( ifxpcd_ep_t *_ifxep )
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	ifxpcd_request_t *ifxreq = 0;

	IFX_DEBUGPL(DBG_PCDV,"%s() %s-%s\n", __func__, _ifxep->sysep.name, (_ifxep->is_in?"IN":"OUT"));

	/* Get any pending requests */
	if (list_empty(&_ifxep->queue))
	{
		if(_ifxep->type==IFXUSB_EP_TYPE_INTR)
			IFX_ERROR("%s() unreq intr complete!\n",__func__);
		else
			IFX_ERROR("%s() complete_ep 0x%p, ep->queue empty!\n",__func__,_ifxep);

		_ifxep->ep_xfer_stage=Idle;
		return;
	}

	ifxreq = list_entry(_ifxep->queue.next, ifxpcd_request_t, trq);

	if(_ifxep->ep_xfer_stage==Cmpt) // Need review, Already completed by other(like Disable), HOWARD
	{
		#if defined(__GADGET_LED__)
			if(_ifxep->type==IFXUSB_EP_TYPE_BULK)
				ifxusb_led(core_if);
		#endif

		// Will complete triggered even with force-disabled and still finish?
		#if !defined(__GADGET_TASKLET_TX__)
			#warning "__GADGET_TASKLET_TX__ NOT DEFINED"
		#endif

		request_done(_ifxep, ifxreq, 0);
		_ifxep->ep_xfer_stage=Idle;
		restart_requests();
		return;
	}

	#if defined(__DED_FIFO__) && defined(__DESC_DMA__)
	{
		desc_sts_data_t desc_sts;
		int ne=0;

//		for(int i=0;i<_ifxep->tx_desc_num&&ne==0;i++,dma_desc++)
		{
			desc_sts.d32 = read_desc((uint32_t *)(_ifxep->dma_desc));
			if(desc_sts.b.bytes!=0)
				ne=1;
		}
		if(ne)
		{
//			for(int i=0;i<_ifxep->tx_desc_num&&ne==0;i++,dma_desc++)
			{
				desc_sts.d32 = read_desc((uint32_t *)(_ifxep->dma_desc));
				IFX_WARN("   Desc remaining %d\n",desc_sts.b.bytes);
			}
			return;
		}
	}
	#else
	{
		deptsiz_data_t  deptsiz;
		ifxusb_dev_in_ep_regs_t  *in_ep_regs  = core_if->in_ep_regs[_ifxep->num];

		deptsiz.d32 = ifxusb_rreg( &in_ep_regs->dieptsiz);
		if (deptsiz.b.xfersize != 0 || deptsiz.b.pktcnt != 0)
		{
			#if defined(__DED_INTR__) || defined(__DED_FIFO__)
			if(deptsiz.b.pktcnt > _ifxep->xfer_pkt_running ) // INTR Underrun
			{
				_ifxep->ep_xfer_stage=Setup;
				ifxpcd_ep_start_tx_transfer(_ifxep);
			}
			else
			#endif
			{
				IFX_WARN("Incomplete transfer (%d)(%s-%s [siz=%d pkt=%d %08X DMA:%08X]) orig[%d]\n",__LINE__,
				     _ifxep->sysep.name, (_ifxep->is_in?"IN":"OUT"),
				     deptsiz.b.xfersize, deptsiz.b.pktcnt,deptsiz.d32,ifxusb_rreg( &in_ep_regs->diepdma),
				     _ifxep->xfer_len_totl);
				IFX_WARN("diepint = %08X   diepctl = %08x\n",
					ifxusb_rreg( &in_ep_regs->diepint),
					ifxusb_rreg( &in_ep_regs->diepctl));
			}
			return;
		}
	}
	#endif

	_ifxep->xfer_len_done+=_ifxep->xfer_len_running;
	_ifxep->xfer_pkt_done+=_ifxep->xfer_pkt_running;

	if(_ifxep->xfer_pkt_done>=_ifxep->xfer_pkt_totl)
	{
		_ifxep->ep_xfer_stage=Cmpt;
		ifxreq->sysreq.actual= _ifxep->xfer_len_done;
		request_done(_ifxep, ifxreq, 0);
		_ifxep->ep_xfer_stage=Idle;
		restart_requests();
	}
	else
	{
		_ifxep->ep_xfer_stage=Setup;
		ifxpcd_ep_start_tx_transfer(_ifxep);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 \brief Handle EPn CMLT Interrupt of Rx Transfer
 */
void complete_ep_rx( ifxpcd_ep_t *_ifxep )
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	ifxpcd_request_t *ifxreq = 0;
	int trsize=0;

	IFX_DEBUGPL(DBG_PCDV,"%s() %s-%s\n", __func__, _ifxep->sysep.name,
	    (_ifxep->is_in?"IN":"OUT"));

	if (list_empty(&_ifxep->queue))
	{
		IFX_WARN("%s() complete_ep 0x%p, ep->queue empty!\n",__func__,_ifxep);
		_ifxep->ep_xfer_stage=Idle;
		return;
	}

	/* Get any pending requests */
	ifxreq = list_entry(_ifxep->queue.next, ifxpcd_request_t, trq);
	if (!ifxreq)
	{
		IFX_WARN("%s() complete_ep 0x%p, req = NULL!\n",__func__,_ifxep);
		return;
	}

	#if defined(__DESC_DMA__)
	{
		desc_sts_data_t desc_sts;
		desc_sts.d32 = read_desc((uint32_t *)(_ifxep->dma_desc));
		trsize=_ifxep->xfer_len_running - desc_sts.b.bytes;
	}
	#else
	{
		deptsiz_data_t  deptsiz;
		ifxusb_dev_out_ep_regs_t *out_ep_regs = core_if->out_ep_regs[_ifxep->num];

		deptsiz.d32 = ifxusb_rreg( &out_ep_regs->doeptsiz);
		trsize=_ifxep->xfer_len_running - deptsiz.b.xfersize;
	}
	#endif

	#if defined(__GADGET_LED__)
		if(_ifxep->type==IFXUSB_EP_TYPE_BULK)
			ifxusb_led(core_if);
	#endif

	ifxreq->sysreq.actual = trsize;

	_ifxep->ep_xfer_stage=Cmpt;

	#if    defined(__UNALIGNED_BUF_ADJ__)
		if(_ifxep->using_aligned_buf)
			memcpy(ifxreq->buf ,_ifxep->aligned_buf,trsize);
		_ifxep->using_aligned_buf=0;
	#endif

	request_done(_ifxep, ifxreq, 0);
	_ifxep->ep_xfer_stage=Idle;
	start_next_request( _ifxep );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

static void stop_ep0_tx(void)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	ifxusb_dev_in_ep_regs_t  *in_ep_regs  = core_if->in_ep_regs[0];
	depctl_data_t  depctl;
	diepint_data_t diepint;

	depctl.d32=ifxusb_rreg (&in_ep_regs->diepctl);
	if(   depctl.b.usbactep && depctl.b.epena )
	{
		depctl.b.snak=1;
		ifxusb_wreg (&in_ep_regs->diepctl,depctl.d32);
		do {
			diepint.d32=ifxusb_rreg(&in_ep_regs->diepint);
		} while(!diepint.b.inepnakeff);
		depctl.d32=ifxusb_rreg (&in_ep_regs->diepctl);
		depctl.b.snak=1;
		depctl.b.epdis=1;
		depctl.b.cnak=0;
		depctl.b.epena=0;
		ifxusb_wreg (&in_ep_regs->diepctl,depctl.d32);
	}
}

void handle_ep0_setup(void)
	{
	#ifdef __DEBUG_EP0__
		IFX_DEBUGPL(DBG_PCDV, "%s()\n", __func__);
	#endif
	#ifndef __DED_FIFO__
		ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
		ifxpcd_ep_t      *ep0  = &ifxusb_pcd.ifxep[0];

		if(   ifxusb_pcd.ep0state==EP0_IDLE
		   || ifxusb_pcd.ep0state==EP0_DISCONNECT
		   || ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE 
		   || ifxusb_pcd.ep0state==EP0_OUT_STATUS_PHASE
		)
		{
			int i;
			ifxusb_pcd.request_config = 0;
			for (i=0; i <= core_if->hwcfg2.b.num_dev_ep ; i++)
			{
				if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].is_in )
				{
					if(  ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK
					#ifndef __DED_INTR__
					   ||ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR
					#endif
					)
						ifxusb_pcd.ifxep[i].ep_hold=1;
				}
			}
			for (i=0; i <= core_if->hwcfg2.b.num_dev_ep ; i++)
			{
				if(ifxusb_pcd.ifxep[i].ep_act && ifxusb_pcd.ifxep[i].is_in )
				{
					if(  ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK
					#ifndef __DED_INTR__
					   ||ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR
					#endif
					)
					{
						ifxpcd_request_t *ifxreq;

						IFX_DEBUGPL(DBG_PCDV, "%s() %s\n", __func__, ep0->sysep.name);

						{
							dctl_data_t dctl      = {.d32 = 0};
							gint_data_t intr_mask = {.d32 = 0};
							ifxusb_pcd.gnaksource=SwitchEP;
							ifxusb_pcd.gnaknext=0;
							dctl.b.sgnpinnak = 1;
							ifxusb_mreg(&ifxusb_pcd.core_if.dev_global_regs->dctl, dctl.d32, dctl.d32);
							intr_mask.b.ginnakeff = 1;
							ifxusb_mreg( &ifxusb_pcd.core_if.core_global_regs->gintmsk, 0, intr_mask.d32);
							// We only send a gnak, then the disable will be issued by nakeff
							break;
						}
					}
				}
			}
		}
	#endif
}


static int check_setup(void)
{
	uint8_t *setupreq=(uint8_t *)&ifxusb_pcd.last_req;
	uint8_t *setuppkt;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	#if defined(__DESC_DMA__)
	{
		desc_sts_data_t dma_desc_sts;
		uint32_t *dma_desc;
		uint32_t dmaaddr;
		ifxpcd_ep_t  *ep0        = &ifxusb_pcd.ifxep[0];

		dma_desc=(uint32_t *)(ifxusb_pcd.setup_desc + ifxusb_pcd.setup_desc_index*2);

		/** DMA Descriptor Setup */
		dma_desc_sts.d32=read_desc((uint32_t *)(dma_desc+0));

//		if(dma_desc_sts.b.bs != BS_DMA_DONE)
//		{
//			return (100+dma_desc_sts.b.bs);
//		}
		if(!dma_desc_sts.b.sr)
		{
			return 200;
		}
		if((ep0->mps-dma_desc_sts.b.bytes) < 8)
		{
			return (300+(ep0->mps-dma_desc_sts.b.bytes));
		}
		dmaaddr         =read_desc((uint32_t *)(dma_desc+1));
		setuppkt=(uint8_t *)(KSEG1ADDR(dmaaddr))+(ep0->mps-dma_desc_sts.b.bytes);
		setuppkt-=8;
	}
	#else //!defined(__DESC_DMA__)
	{
		uint32_t dmaaddr;
		dmaaddr =ifxusb_rreg( &ifxusb_pcd.core_if.out_ep_regs[0]->doepdma);
		setuppkt=(uint8_t *)(KSEG1ADDR(dmaaddr));
		if(setuppkt-(uint8_t *)ifxusb_pcd.setup_pkt <8)
		{
			return (300+(setuppkt-(uint8_t *)ifxusb_pcd.setup_pkt));
		}
		setuppkt-=8;
	}
	#endif //defined(__DESC_DMA__)

	*(setupreq+0)=*(setuppkt+0); *(setupreq+1)=*(setuppkt+1);
	*(setupreq+2)=*(setuppkt+3); *(setupreq+3)=*(setuppkt+2);
	*(setupreq+4)=*(setuppkt+5); *(setupreq+5)=*(setuppkt+4);
	*(setupreq+6)=*(setuppkt+7); *(setupreq+7)=*(setuppkt+6);

	#ifdef __DEBUG_EP0__
		IFX_DEBUGPL(DBG_PCD, "%s() SETUP %02x.%02x v%04x i%04x l%04x\n",__func__,
	                    ifxusb_pcd.last_req.bRequestType, ifxusb_pcd.last_req.bRequest,
	                    ifxusb_pcd.last_req.wValue, ifxusb_pcd.last_req.wIndex, ifxusb_pcd.last_req.wLength);
	#endif
	return 0;
}



#ifdef __DESC_DMA__
	static void ep0_rx_next(int with_sts,int with_cmpt)
#else
static void ep0_rx_next(int with_cmpt)
#endif
	{
		ifxpcd_ep_t      *ep0  = &ifxusb_pcd.ifxep[0];

		uint32_t          remained   =0;
		uint32_t          transferred=0;
		#ifdef __DESC_DMA__
			desc_sts_data_t   desc_sts;
		#else
			deptsiz_data_t    deptsiz;
		#endif

		#ifdef __DEBUG_EP0__
			IFX_DEBUGPL(DBG_PCDV, "%s()\n", __func__);
			print_ep0_state();
		#endif

		#ifdef __DEBUG_EP0__
		#ifdef __DESC_DMA__
			IFX_DEBUGPL(DBG_PCD, "DATA_OUT EP0-%s: type=%d, mps=%d buf=%p len=%d/%d/%d pkt=%d/%d/%d idx=%d\n",
			     (ep0->is_in ?"IN":"OUT"),
			     ep0->type, ep0->mps,
			     ep0->buf,
			     ep0->xfer_len_totl,ep0->xfer_len_done,ep0->xfer_len_running,
			     ep0->xfer_pkt_totl,ep0->xfer_pkt_done,ep0->xfer_pkt_running,
			     ifxusb_pcd.setup_desc
			     );
		#else
			IFX_DEBUGPL(DBG_PCD, "DATA_OUT EP0-%s: type=%d, mps=%d buf=%p len=%d/%d/%d pkt=%d/%d/%d\n",
			     (ep0->is_in ?"IN":"OUT"),
			     ep0->type, ep0->mps,
			     ep0->buf,
			     ep0->xfer_len_totl,ep0->xfer_len_done,ep0->xfer_len_running,
			     ep0->xfer_pkt_totl,ep0->xfer_pkt_done,ep0->xfer_pkt_running
			     );
		#endif
		#endif

		#ifdef __DESC_DMA__
			desc_sts.d32 = read_desc((uint32_t *)(ifxusb_pcd.setup_desc + ifxusb_pcd.setup_desc_index*2));
			remained     = desc_sts.b.bytes;
		#else
			deptsiz.d32  = ifxusb_rreg(&ifxusb_pcd.core_if.in_ep_regs[0]->dieptsiz);
			remained     = deptsiz.b.xfersize;
		#endif

		if(with_cmpt)
		{
			if(remained==0
			#ifdef __DESC_DMA__
			   &&!with_sts
			#endif
			)
			{
				ep0->xfer_len_done   +=ep0->mps;
				if(ep0->xfer_len_done>=ep0->xfer_len_totl)
					ep0->xfer_len_done=ep0->xfer_len_totl;
				else
				{
					IFX_DEBUGPL(DBG_PCD, "CONTINUE RX TRANSFER\n");
					ifxpcd_ep_start_rx_transfer(ep0);
					return;
				}
			}
			else
			{
				transferred      = ep0->mps - remained;
				ep0->xfer_len_done   +=transferred;
				if(ep0->xfer_len_done>=ep0->xfer_len_totl)
					ep0->xfer_len_done=ep0->xfer_len_totl;
			}
		}
		ep0_out_start();
		{
			ifxpcd_request_t *ifxreq;
			IFX_DEBUGPL(DBG_PCDV, "%s() %s\n", __func__, ep0->sysep.name);
			if(!list_empty(&ep0->queue))
			{
				ifxreq = list_entry(ep0->queue.next, ifxpcd_request_t, trq);
					/* ep0-OUT */
				ifxreq->sysreq.actual = ep0->xfer_len_done;
				if (ifxreq->sysreq.zero)
				    ifxreq->sysreq.zero = 0;
				#ifdef __DEBUG_EP0__
					IFX_DEBUGPL(DBG_PCDV, "Setup Tx ZLP\n");
				#endif
				request_done_ep0(ifxreq, 0);
			}
		}

		#ifdef __DESC_DMA__
			if(with_sts)
				do_setup_in_status_phase();
			else
			{
				ifxusb_pcd.ep0state = EP0_OUT_POST_DATA_PHASE;
			}
			/* Prepare for more SETUP Packets */
		#else
			do_setup_in_status_phase();
		#endif
	}


	void handle_ep0_rx(int _case)
	{
		ifxpcd_ep_t      *ep0  = &ifxusb_pcd.ifxep[0];

	#ifdef __DEBUG_EP0__
		IFX_DEBUGPL(DBG_PCDV, "%s() %d %d\n", __func__,_case,ifxusb_pcd.ep0state);
		print_ep0_state();
	#endif
	
	
		switch(ifxusb_pcd.ep0state)
		{
			case EP0_IDLE             :
				switch(_case)
				{
				#ifdef __DESC_DMA__
					case 1:
						//StsPhse-no, STP-no, Xfercmpl-yes
						//Core has update OUT desc.
						//  SR=1 Record STP
						//  Then Restart RX
						check_setup();    // record
						ep0_out_start();
						break;
					case 2:
						//StsPhse-no, STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						pcd_setup();     // take the latest result directly
						break;
					case 3:
						//StsPhse-no, STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//Check and decode STP, if not->restart EP0-OUT
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
					case 4:
						//StsPhse-yes, STP-no, Xfercmpl-no
						//Host Switch to Sts during CTRL-WR
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IDLE case:%d\n",__func__,_case);
						break;
					case 5:
						//StsPhse-yes, STP-no, Xfercmpl-yes
						//Combination of case 1 and 4, could be CTRL_RD_DAT or SETUP.
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IDLE case:%d\n",__func__,_case);
						break;
				#else
					case 1:
						//STP-no, Xfercmpl-yes
						//Core has update OUT desc.
						//  SR=1 Record STP
						//  Then Restart RX
						check_setup();    // record
						ep0_out_start();
						break;
					case 2:
						//STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
					case 3:
						//STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//Check and decode STP, if not->restart EP0-OUT
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
				#endif
				}
				break;
			case EP0_IN_DATA_PHASE    :
				switch(_case)
				{
				#ifdef __DESC_DMA__
					case 1:
						//StsPhse-no, STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.

						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}

						ifxusb_pcd.ep0state=EP0_IDLE;
						//  SR=1 Record STP
						//  Then Restart RX
						check_setup();    // record
						ep0_out_start();
						break;
					case 2:
						//StsPhse-no, STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IN_DATA_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//StsPhse-no, STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.

						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
								ep0_out_start();
						else
							pcd_setup();
						break;
					case 4:
						//StsPhse-yes, STP-no, Xfercmpl-no
						//Host Switch to Sts during CTRL-WR
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IN_DATA_PHASE case:%d\n",__func__,_case);
							break;
					case 5:
						//StsPhse-yes, STP-no, Xfercmpl-yes
						//Combination of case 1 and 4, could be CTRL_RD_DAT or SETUP.
						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						check_setup();
						ep0_out_start();
						break;
				#else
					case 1:
						//STP-no, Xfercmpl-yes
						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}

						ifxusb_pcd.ep0state=EP0_IDLE;
						//  SR=1 Record STP
						//  Then Restart RX
						check_setup();    // record
						ep0_out_start();
						break;
					case 2:
						//STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IN_DATA_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.

						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
				#endif
				}
				break;
			case EP0_OUT_DATA_PHASE   :
				switch(_case)
				{
				#ifdef __DESC_DMA__
					case 1:
						//StsPhse-no, STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						if(!check_setup())
						{
							ifxusb_pcd.ep0state=EP0_IDLE;
							ep0_out_start();
						}
						else
							ep0_rx_next(0,1);
						break;
					case 2:
						//StsPhse-no, STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						// Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_DATA_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//StsPhse-no, STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.

						//Abort, Decode SR and STP
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
					case 4:
						//StsPhse-yes, STP-no, Xfercmpl-no
						//Host Switch to Sts during CTRL-WR
						//Switch to CTRL-WR-STS(IN)
						ep0_rx_next(1,0);
						break;
					case 5:
						//StsPhse-yes, STP-no, Xfercmpl-yes
						//Combination of case 1 and 4, could be CTRL_RD_DAT or SETUP.
						//DMA done, check Desc, Switch to CTRL-WR-STS(IN)
						ep0_rx_next(1,1);
						break;
				#else
					case 1:
						//STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						ep0_rx_next(1);
						break;
					case 2:
						//STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						// Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_DATA_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.

						//Abort, Decode SR and STP
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
				#endif
				}
				break;
			case EP0_OUT_POST_DATA_PHASE   :
				switch(_case)
				{
				#ifdef __DESC_DMA__
					case 1:
						//StsPhse-no, STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							IFX_PRINT("%s() Invalid Sts:EP0_OUT_POST_DATA_PHASE case:%d\n",__func__,_case);
						else
							ep0_out_start();
						break;
					case 2:
						//StsPhse-no, STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.

						// Invalid
						ifxusb_pcd.ep0state=EP0_IDLE;
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_DATA_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//StsPhse-no, STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//Abort, Decode SR and STP
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
					case 4:
						//StsPhse-yes, STP-no, Xfercmpl-no
						//Host Switch to Sts during CTRL-WR
						//Switch to CTRL-WR-STS(IN)
						do_setup_in_status_phase();
						break;
					case 5:
						//StsPhse-yes, STP-no, Xfercmpl-yes
						//Combination of case 1 and 4, could be CTRL_RD_DAT or SETUP.
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_POST_DATA_PHASE case:%d\n",__func__,_case);
						break;
				#else
					case 1:
						//STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						ifxusb_pcd.ep0state=EP0_IDLE;
						ep0_out_start();
						break;
					case 2:
						//STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						// Invalid
						ifxusb_pcd.ep0state=EP0_IDLE;
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_DATA_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//Abort, Decode SR and STP
						ifxusb_pcd.ep0state=EP0_IDLE;
						pcd_setup();
						break;
				#endif
				}
				break;
			case EP0_IN_STATUS_PHASE  :
				switch(_case)
				{
				#ifdef __DESC_DMA__
					case 1:
						//StsPhse-no, STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						//Check SR to record the STP
						check_setup();
						//Restart EP0-OUT
						ep0_out_start();
						break;
					case 2:
						//StsPhse-no, STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IN_STATUS_PHASE case:%d\n",__func__,_case);
						break;
					case 3:
						//StsPhse-no, STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
					case 4:
						//StsPhse-yes, STP-no, Xfercmpl-no
						//Host Switch to Sts during CTRL-WR

						//Do nothing, normal
						IFX_PRINT("%s() Invalid Sts:EP0_IN_STATUS_PHASE case:%d\n",__func__,_case);
						break;
					case 5:
						//StsPhse-yes, STP-no, Xfercmpl-yes
						//Combination of case 1 and 4, could be CTRL_RD_DAT or SETUP.
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_IN_STATUS_PHASE case:%d\n",__func__,_case);
						break;
				#else
					case 1:
						//STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						//Restart EP0-OUT
						ep0_out_start();
						break;
					case 2:
						//STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						//Invalid
					case 3:
						//STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//Abort Tx with flush, restart EP0-OUT
						//Abort Tx
						//Flush Tx
						{
							depctl_data_t depctl;
							depctl.d32=ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
							if(   depctl.b.usbactep && depctl.b.epena )
							{
								request_nuke(ep0);
								stop_ep0_tx();
							}
						}
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
				#endif
				}
				break;
			case EP0_OUT_STATUS_PHASE :
				switch(_case)
				{
				#ifdef __DESC_DMA__
					case 1:
						//StsPhse-no, STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						//IDLE, restart EP0-OUT
						ifxusb_pcd.ep0state=EP0_IDLE;
						check_setup();
						ep0_out_start();
						break;
					case 2:
						//StsPhse-no, STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						//IDLE, restart EP0-OUT
						ep0_out_start();
						break;
					case 3:
						//StsPhse-no, STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//IDLE, Check and decode STP, if not->restart EP0-OUT
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
					case 4:
						//StsPhse-yes, STP-no, Xfercmpl-no
						//Host Switch to Sts during CTRL-WR
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_STATUS_PHASE case:%d\n",__func__,_case);
						break;
					case 5:
						//StsPhse-yes, STP-no, Xfercmpl-yes
						//Combination of case 1 and 4, could be CTRL_RD_DAT or SETUP.
						//Invalid
						IFX_PRINT("%s() Invalid Sts:EP0_OUT_STATUS_PHASE case:%d\n",__func__,_case);
						break;
				#else
					case 1:
						//STP-no, Xfercmpl-yes
						//Core has update OUT desc. Check SR to see data received is STP or not.
						//IDLE, restart EP0-OUT
						ifxusb_pcd.ep0state=EP0_IDLE;
						ep0_out_start();
						break;
					case 2:
						//STP-yes, Xfercmpl-no
						//a IN/OUT token received and NAKED, folloing a previous recrived SETUP.
						//IDLE, restart EP0-OUT
						ep0_out_start();
						break;
					case 3:
						//STP-yes, Xfercmpl-yes
						//Combination of case 1 and 2, could be CTRL_RD_STS+SETUP or SETUP only.
						//IDLE, Check and decode STP, if not->restart EP0-OUT
						ifxusb_pcd.ep0state=EP0_IDLE;
						if(check_setup())
							ep0_out_start();
						else
							pcd_setup();
						break;
				#endif
				}
				break;
			default:
			IFX_WARN("%s() : Invalid state : %s %s type=%d, mps=%d _case:%d\n",__func__,
				     (ifxusb_pcd.ep0state==EP0_DISCONNECT)?"EP0_DISCONNECT":
				      (
				        (ifxusb_pcd.ep0state==EP0_IDLE)?"EP0_IDLE":
				         (
				          (ifxusb_pcd.ep0state==EP0_IN_DATA_PHASE)?"EP0_IN_DATA_PHASE":
				           (
				            (ifxusb_pcd.ep0state==EP0_OUT_DATA_PHASE)?"EP0_OUT_DATA_PHASE":
				             (
				              (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_IN_STATUS":
				               (
				                (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_OUT_STATUS":
				                 (
				                  (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_OUT_STATUS":
				                   (
			                    (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_OUT_POST_DATA_PHASE":
			                     (
				                    (ifxusb_pcd.ep0state==EP0_STALL)?"EP0_STALL":"EP0_INVALID"
				                   )
				                 )
				               )
				             )
				           )
				         )
			         )
				      ),
				     (ep0->is_in ?"IN":"OUT"),
			     ep0->type, ep0->mps,_case);
				}
			}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ep0_tx_next(void)
	{
		ifxpcd_ep_t      *ep0  = &ifxusb_pcd.ifxep[0];
		uint32_t          transferred=0;
		#ifndef __DESC_DMA__
			uint32_t          transferred_packet=0;
		#endif
		ifxpcd_request_t *ifxreq;
		#ifdef __DESC_DMA__
			desc_sts_data_t   desc_sts;
		#else
			deptsiz_data_t    deptsiz;
		#endif

		#ifdef __DEBUG_EP0__
			IFX_DEBUGPL(DBG_PCDV, "%s()\n", __func__);
			print_ep0_state();
		#endif

	#ifdef __DESC_DMA__
		#ifdef __DEBUG_EP0__
			IFX_DEBUGPL(DBG_PCD, "DATA_IN EP0-%s: type=%d, mps=%d buf=%p len=%d/%d/%d pkt=%d/%d/%d idx=%d\n",
			     (ep0->is_in ?"IN":"OUT"),
			     ep0->type, ep0->mps,
			     ep0->buf,
			     ep0->xfer_len_totl,ep0->xfer_len_done,ep0->xfer_len_running,
			     ep0->xfer_pkt_totl,ep0->xfer_pkt_done,ep0->xfer_pkt_running,
			     ifxusb_pcd.setup_desc
			     );
		#endif
		desc_sts.d32 = read_desc((uint32_t *)(ifxusb_pcd.in_desc));
		transferred  = ep0->xfer_len_running - desc_sts.b.bytes;
	#else
		#ifdef __DEBUG_EP0__
			IFX_DEBUGPL(DBG_PCD, "DATA_IN EP0-%s: type=%d, mps=%d buf=%p len=%d/%d/%d pkt=%d/%d/%d\n",
					     (ep0->is_in ?"IN":"OUT"),
					     ep0->type, ep0->mps,
					     ep0->buf,
					     ep0->xfer_len_totl,ep0->xfer_len_done,ep0->xfer_len_running,
					     ep0->xfer_pkt_totl,ep0->xfer_pkt_done,ep0->xfer_pkt_running
					     );
				#endif
		deptsiz.d32  = ifxusb_rreg(&ifxusb_pcd.core_if.in_ep_regs[0]->dieptsiz);
		transferred  = ep0->xfer_len_running - deptsiz.b.xfersize;
		transferred_packet  = ep0->xfer_pkt_running - deptsiz.b.pktcnt;
	#endif

	if(ep0->xfer_len_done+transferred>ep0->xfer_len_totl)
		transferred=ep0->xfer_len_totl-ep0->xfer_len_done;
	ep0->xfer_len_done   +=transferred;

	if(ep0->xfer_pkt_done+transferred_packet>ep0->xfer_pkt_totl)
		transferred_packet=ep0->xfer_pkt_totl-ep0->xfer_pkt_done;
	ep0->xfer_pkt_done   +=transferred_packet;


	if(ep0->xfer_len_done>=ep0->xfer_len_totl)
	{
		ep0->xfer_len_done=ep0->xfer_len_totl;
		#ifndef __DESC_DMA__
		if(ep0->xfer_pkt_done<ep0->xfer_pkt_totl)
		{
			IFX_DEBUGPL(DBG_PCD, "CONTINUE TX TRANSFER(ZLP)\n");
			ifxpcd_ep_start_tx_transfer(ep0);
			return;
		}
		#endif
	}
	else
	{
		IFX_DEBUGPL(DBG_PCD, "CONTINUE TX TRANSFER\n");
		ifxpcd_ep_start_tx_transfer(ep0);
		return;
	}
	IFX_DEBUGPL(DBG_PCD, "COMPLETE TRANSFER\n");
	
	if(!list_empty(&ep0->queue))
	{
		ifxreq = list_entry(ep0->queue.next, ifxpcd_request_t, trq);
		/* ep0-IN */
		ifxreq->sysreq.actual = ep0->xfer_len_done;
		request_done_ep0(ifxreq, 0);
		if (ifxreq->sysreq.zero)
		    ifxreq->sysreq.zero = 0;
	}
	ep0->ep_xfer_stage=Idle;
	do_setup_out_status_phase();
	#if !defined(__DESC_DMA__) && !defined(__DED_FIFO__)
		#ifdef __DED_INTR__
		if(ifxusb_pcd.RNDIS_Pending==0)
		#else
		if(ifxusb_pcd.INTR_Pending==0)
		#endif
		{
			int i;
			for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
				ifxusb_pcd.ifxep[i].ep_hold=0;
			restart_requests();
		}
	#endif //!defined(__DESC_DMA__) && !defined(__DED_FIFO__)
}





void handle_ep0_tx()
{
	ifxpcd_ep_t      *ep0     = &ifxusb_pcd.ifxep[0];
	
	#ifdef __DEBUG_EP0__
		IFX_DEBUGPL(DBG_PCDV, "%s()\n", __func__);
		print_ep0_state();
	#endif
	switch(ifxusb_pcd.ep0state)
					{
		case EP0_IN_DATA_PHASE    :
			//DMA done, check Desc, Switch to CTRL-WR-STS(IN)
			ep0_tx_next();
				break;
		case EP0_IN_STATUS_PHASE  :
			//DMA done, check Desc, Switch to Idle
					ifxusb_pcd.ep0state = EP0_IDLE;
			#if !defined(__DESC_DMA__) && !defined(__DED_FIFO__)
				#ifdef __DED_INTR__
				if(ifxusb_pcd.RNDIS_Pending==0)
				#else
				if(ifxusb_pcd.INTR_Pending==0)
				#endif
				{
					int i;
					for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
						ifxusb_pcd.ifxep[i].ep_hold=0;
					restart_requests();
				}
			#endif //!defined(__DESC_DMA__) && !defined(__DED_FIFO__)
				break;
			default:
			IFX_WARN("%s() : Invalid state : %s %s type=%d, mps=%d\n",__func__,
				     (ifxusb_pcd.ep0state==EP0_DISCONNECT)?"EP0_DISCONNECT":
				      (
				        (ifxusb_pcd.ep0state==EP0_IDLE)?"EP0_IDLE":
				         (
				          (ifxusb_pcd.ep0state==EP0_IN_DATA_PHASE)?"EP0_IN_DATA_PHASE":
				           (
				            (ifxusb_pcd.ep0state==EP0_OUT_DATA_PHASE)?"EP0_OUT_DATA_PHASE":
				             (
				              (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_IN_STATUS":
				               (
				                (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_OUT_STATUS":
				                 (
				                  (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_OUT_STATUS":
				                   (
			                    (ifxusb_pcd.ep0state==EP0_IN_STATUS_PHASE)?"EP0_OUT_POST_DATA_PHASE":
			                     (
				                    (ifxusb_pcd.ep0state==EP0_STALL)?"EP0_STALL":"EP0_INVALID"
				                   )
				                 )
				               )
				             )
				           )
				         )
					  )
			      ),
			     (ep0->is_in ?"IN":"OUT"),
			     ep0->type, ep0->mps );
		}
	}
