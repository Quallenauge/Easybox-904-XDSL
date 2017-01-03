/*****************************************************************************
 **   FILE NAME       : ifxpcd_tran.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : Routines to Start/Continue a Tx/Rx transfer of an EP.
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
 \file ifxpcd_tran.c
 \ingroup IFXUSB_DRIVER_V3
 \brief Routines to Start/Continue a Tx/Rx transfer of an EP
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 \brief This function start a new transfer for this request
 */
void ifxpcd_start_new_transfer(ifxpcd_request_t   *_ifxreq,ifxpcd_ep_t *_ifxep)
{
	IFX_DEBUGPL((DBG_PCDV | DBG_CILV), "%s()\n", __func__);

	if( _ifxep->num==0 || _ifxep->type != IFXUSB_EP_TYPE_BULK)
	{
		if(_ifxreq->len && !_ifxreq->buf)
		{
			IFX_WARN("%s()%d :WARN ep%d-%s buf=%p size=%d\n",__func__,__LINE__,
			    _ifxep->num, (_ifxep->is_in?"IN":"OUT"),
			     _ifxreq->buf, _ifxreq->len);
			return;
		}

		_ifxep->buf           =_ifxreq->buf;
		_ifxep->xfer_len_totl =_ifxreq->len;
		_ifxep->xfer_len_done = 0;
		_ifxep->xfer_pkt_done = 0;

		#if defined(__UNALIGNED_BUF_ADJ__)
			_ifxep->using_aligned_buf=0;
		#endif

		if( _ifxep->is_in) //Tx
		{
			if(_ifxreq->sysreq.zero && (_ifxep->xfer_len_totl % _ifxep->mps == 0))
				_ifxep->zlp_sp = 1;
			else
				_ifxep->zlp_sp = 0;
			_ifxep->xfer_pkt_totl= (_ifxep->xfer_len_totl+_ifxep->mps-1)/_ifxep->mps+_ifxep->zlp_sp;

			#if !defined(__DESC_DMA__) &&  (defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__) )
				if (_ifxep->xfer_len_totl && (((unsigned long)_ifxep->buf) & _ifxep->ifxpcd->core_if.unaligned_mask))
					IFX_WARN("UNALIGNED BUFFER in REQUEST\n");
			#endif //!defined(__DESC_DMA__) &&  defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__) )

			#if 1 // cache write-back, so DMA engine can get correct content. Precaution
				if(_ifxep->xfer_len_totl)
					if(((uint32_t)_ifxep->buf)==((uint32_t)(KSEG0ADDR(_ifxep->buf))))
						dma_cache_wback_inv((unsigned long)_ifxep->buf, _ifxep->xfer_len_totl);
			#endif
			_ifxep->ep_xfer_stage = Setup;
			ifxpcd_ep_start_tx_transfer(_ifxep);
		}
		else
		{
			if(_ifxep->xfer_len_totl < _ifxep->mps)
				_ifxep->xfer_len_totl = _ifxep->mps;

			if(_ifxreq->sysreq.short_not_ok)
				_ifxep->zlp_sp = 0;
			else
				_ifxep->zlp_sp = 1;

			_ifxep->xfer_pkt_totl = (_ifxep->xfer_len_totl+_ifxep->mps-1)/_ifxep->mps;

			#if ( defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__) )
				if (((unsigned long)_ifxep->buf) & _ifxep->ifxpcd->core_if.unaligned_mask)
					IFX_WARN("UNALIGNED BUFFER in REQUEST\n");
			#endif //defined(__UNALIGNED_BUF_ADJ__) || defined(__UNALIGNED_BUF_CHK__) )

			#if 1 // cache write-back, so DMA engine can get correct content. Precaution
				if(((uint32_t)_ifxep->buf)==((uint32_t)(KSEG0ADDR(_ifxep->buf))))
					dma_cache_wback_inv((unsigned long)_ifxep->buf, _ifxep->xfer_len_totl);
			#endif
			_ifxep->ep_xfer_stage = Setup;
			ifxpcd_ep_start_rx_transfer(_ifxep);
		}
	}
	else if( _ifxep->is_in) //Tx Bulk
	{
		if(_ifxreq->len && !_ifxreq->buf)
		{
			IFX_WARN("%s()%d :WARN ep%d-%s buf=%p size=%d\n",__func__,__LINE__,
			    _ifxep->num, (_ifxep->is_in?"IN":"OUT"),
			    _ifxreq->buf, _ifxreq->len);
			return;
		}

		_ifxep->buf           =_ifxreq->buf;
		_ifxep->xfer_len_totl =_ifxreq->len;
		_ifxep->xfer_len_done = 0;
		_ifxep->xfer_pkt_done = 0;

		if(_ifxreq->sysreq.zero && (_ifxep->xfer_len_totl % _ifxep->mps == 0))
			_ifxep->zlp_sp = 1;
		else
			_ifxep->zlp_sp = 0;

		_ifxep->xfer_pkt_totl= (_ifxep->xfer_len_totl+_ifxep->mps-1)/_ifxep->mps+_ifxep->zlp_sp;

		#if defined(__UNALIGNED_BUF_ADJ__)
			_ifxep->using_aligned_buf=0;
		#endif

		#if !defined(__DESC_DMA__) && defined(__UNALIGNED_BUF_ADJ__)
			if (_ifxep->xfer_len_totl && (((unsigned long)_ifxep->buf) & _ifxep->ifxpcd->core_if.unaligned_mask))
			{
				if(_ifxep->aligned_buf
				    && _ifxep->aligned_buf_len > 0
				    && _ifxep->aligned_buf_len < _ifxep->xfer_len_totl)
				{
					ifxusb_free_buf_d(_ifxep->aligned_buf);
					_ifxep->aligned_buf=NULL;
					_ifxep->aligned_buf_len=0;
				}
				if(! _ifxep->aligned_buf || !_ifxep->aligned_buf_len)
				{
					int len;
					len=_ifxep->xfer_len_totl+3;
					len/=4;
					len*=4;
					_ifxep->aligned_buf = ifxusb_alloc_buf_d(len, 0);
					if(_ifxep->aligned_buf)
						_ifxep->aligned_buf_len = len;
				}
				if(_ifxep->aligned_buf && _ifxep->aligned_buf_len >= _ifxep->xfer_len_totl)
				{
					memcpy(_ifxep->aligned_buf, _ifxep->buf, _ifxep->xfer_len_totl);
					_ifxep->using_aligned_buf=1;
				}
				else
					IFX_WARN("%s():%d\n",__func__,__LINE__);
			}
		#elif  !defined(__DESC_DMA__) && defined(__UNALIGNED_BUF_CHK__)
			if (_ifxep->xfer_len_totl && (((unsigned long)_ifxep->buf) & _ifxep->ifxpcd->core_if.unaligned_mask))
				IFX_WARN("UNALIGNED BUFFER in REQUEST\n");
		#endif //!defined(__DESC_DMA__) && defined(__UNALIGNED_BUF_ADJ__)

		#if 1 // cache write-back, so DMA engine can get correct content. Precaution
			if(_ifxep->xfer_len_totl)
			{
				#if !defined(__DESC_DMA__) && defined(__UNALIGNED_BUF_ADJ__)
					if(_ifxep->using_aligned_buf)
					{
						if(((uint32_t)_ifxep->aligned_buf)==((uint32_t)(KSEG0ADDR(_ifxep->aligned_buf))))
							dma_cache_wback_inv((unsigned long)_ifxep->aligned_buf, _ifxep->xfer_len_totl);
					}
					else
				#endif
					if(((uint32_t)_ifxep->buf)==((uint32_t)(KSEG0ADDR(_ifxep->buf))))
						dma_cache_wback_inv((unsigned long)_ifxep->buf, _ifxep->xfer_len_totl);
			}
		#endif
		_ifxep->ep_xfer_stage = Setup;
		ifxpcd_ep_start_tx_transfer(_ifxep);
	}
	else //Bulk Rx
	{
		if(!_ifxreq->buf)
		{
			IFX_WARN("%s()%d :WARN ep%d-%s buf=%p size=%d\n",__func__,__LINE__,
			    _ifxep->num, (_ifxep->is_in?"IN":"OUT"),
			    _ifxreq->buf, _ifxreq->len);
			return;
		}

		_ifxep->buf           =_ifxreq->buf;
		_ifxep->xfer_len_totl =_ifxreq->len;
		_ifxep->xfer_len_done = 0;
		_ifxep->xfer_pkt_done = 0;

		if(_ifxep->xfer_len_totl < _ifxep->mps)
			_ifxep->xfer_len_totl = _ifxep->mps;

		if(_ifxreq->sysreq.short_not_ok)
			_ifxep->zlp_sp = 0;
		else
			_ifxep->zlp_sp = 1;

		_ifxep->xfer_pkt_totl= (_ifxep->xfer_len_totl+_ifxep->mps-1)/_ifxep->mps;

		#if defined(__UNALIGNED_BUF_ADJ__)
			_ifxep->using_aligned_buf=0;
		#endif

		#if defined(__UNALIGNED_BUF_ADJ__)
			if (((unsigned long)_ifxep->buf) & _ifxep->ifxpcd->core_if.unaligned_mask)
			{
				if(_ifxep->aligned_buf
				    && _ifxep->aligned_buf_len > 0
				    && _ifxep->aligned_buf_len < _ifxep->xfer_len_totl)
				{
					ifxusb_free_buf_d(_ifxep->aligned_buf);
					_ifxep->aligned_buf=NULL;
					_ifxep->aligned_buf_len=0;
				}

				if(! _ifxep->aligned_buf || !_ifxep->aligned_buf_len)
				{
					_ifxep->aligned_buf = ifxusb_alloc_buf_d(_ifxep->xfer_len_totl, 0);
					if(_ifxep->aligned_buf)
						_ifxep->aligned_buf_len = _ifxep->xfer_len_totl;
				}
				if(_ifxep->aligned_buf && _ifxep->aligned_buf_len >= _ifxep->xfer_len_totl)
					_ifxep->using_aligned_buf=1;
				else
					IFX_WARN("%s():%d\n",__func__,__LINE__);
			}
		#elif  defined(__UNALIGNED_BUF_CHK__)
			if (((unsigned long)_ifxep->buf) & _ifxep->ifxpcd->core_if.unaligned_mask)
				IFX_WARN("UNALIGNED BUFFER in REQUEST\n");
		#endif //defined(__UNALIGNED_BUF_ADJ__)


		#if 1 // cache write-back, so DMA engine can get correct content. Precaution
			#if defined(__UNALIGNED_BUF_ADJ__)
				if(_ifxep->using_aligned_buf)
				{
					if(((uint32_t)_ifxep->aligned_buf)==((uint32_t)(KSEG0ADDR(_ifxep->aligned_buf))))
						dma_cache_wback_inv((unsigned long)_ifxep->aligned_buf, _ifxep->xfer_len_totl);
				}
				else
			#endif
				if(((uint32_t)_ifxep->buf)==((uint32_t)(KSEG0ADDR(_ifxep->buf))))
					dma_cache_wback_inv((unsigned long)_ifxep->buf, _ifxep->xfer_len_totl);
		#endif
		_ifxep->ep_xfer_stage = Setup;
		ifxpcd_ep_start_rx_transfer(_ifxep);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*!
 \brief Start a Tx transfer of the EP.
 \param _ifxep        Pointer of ep structure
 */
void ifxpcd_ep_start_tx_transfer(ifxpcd_ep_t *_ifxep)
{
	depctl_data_t     depctl;
	int               send_zero=0;
	#if defined(__DESC_DMA__)
		desc_sts_data_t dma_desc_sts;
		uint32_t *dma_desc;
	#endif //defined(__DESC_DMA__)
	ifxusb_dev_in_ep_regs_t *in_regs = ifxusb_pcd.core_if.in_ep_regs[_ifxep->num];

	IFX_DEBUGPL((DBG_PCDV | DBG_CILV), "%s()\n", __func__);


	/********************************************
	 * Handling holding condition
	 ********************************************/
	#if !defined(__DED_FIFO__)
		if(ifxusb_pcd.gnakstopping)
			return;
		if(_ifxep->ep_hold)
			return;
	#endif

	IFX_DEBUGPL(DBG_PCD, "ep%d-%s len=%d/%d pkg=%d/%d zlp=%d\n",
	    _ifxep->num, (_ifxep->is_in?"IN":"OUT"),
	    _ifxep->xfer_len_done,_ifxep->xfer_len_totl,
	    _ifxep->xfer_pkt_done,_ifxep->xfer_pkt_totl,
	    _ifxep->zlp_sp);

	send_zero=_ifxep->zlp_sp;

	_ifxep->xfer_len_running=_ifxep->xfer_len_totl - _ifxep->xfer_len_done;
	_ifxep->xfer_pkt_running=_ifxep->xfer_pkt_totl - _ifxep->xfer_pkt_done;

	if(_ifxep->type!=IFXUSB_EP_TYPE_BULK)
		_ifxep->xfer_pkt_running=1;

	if(_ifxep->xfer_pkt_running * _ifxep->mps < _ifxep->xfer_len_running)
		_ifxep->xfer_len_running = _ifxep->xfer_pkt_running * _ifxep->mps;

	if(_ifxep->xfer_pkt_running+_ifxep->xfer_pkt_done < _ifxep->xfer_pkt_totl)
		send_zero=0;

	/********************************************
	 * Handling FIFO full condition
	 ********************************************/
	#ifndef  __DED_FIFO__
		#ifdef  __DED_INTR__
		if(_ifxep->type!=IFXUSB_EP_TYPE_INTR)
		#endif
		{
			unsigned long countdown=10;
			gnptxsts_data_t gtxstatus;
			gtxstatus.d32 = ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gnptxsts);
			#if 1
				while (gtxstatus.b.nptxqspcavail == 0 && countdown >0)
				{
					mdelay(1);
					gtxstatus.d32 =
						ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gnptxsts);
					countdown--;
				}
			#endif
			if(gtxstatus.b.nptxqspcavail == 0)
			{
				IFX_DEBUGPL(DBG_PCD, "TX Queue Full (0x%0x)\n", gtxstatus.d32);
				return;
			}
		}
	#endif // __DED_FIFO__

	#ifdef  __DED_FIFO__
		if(_ifxep->type!=IFXUSB_EP_TYPE_BULK)
			ifxusb_flush_tx_fifo_d(&ifxusb_pcd.core_if, _ifxep->tx_fifo_num);
	#else
		#ifdef  __DED_INTR__
			if(_ifxep->type==IFXUSB_EP_TYPE_INTR)
			{
				ifxusb_flush_tx_fifo_d(&ifxusb_pcd.core_if, _ifxep->tx_fifo_num);
				_ifxep->xfer_pkt_totl=_ifxep->xfer_pkt_running=1;
			}
		#endif
	#endif // __DED_FIFO__


	/********************************************
	 * DMA Setting
	 ********************************************/

	#if   defined(__DESC_DMA__)
		if(_ifxep->num==0)
			dma_desc=(uint32_t *)(ifxusb_pcd.in_desc);   // Data In or Status In phases
		else
			dma_desc=(uint32_t *)(_ifxep->dma_desc);     // Bulk-In or Intr-In

		dma_desc_sts.b.bs       = BS_HOST_BUSY;
		dma_desc_sts.b.l        = 1;
		dma_desc_sts.b.ioc      = 1;
		dma_desc_sts.b.bytes    = _ifxep->xfer_len_running;
		dma_desc_sts.b.mtrf     = 0;
		if(_ifxep->xfer_len_running % _ifxep->mps != 0)
			dma_desc_sts.b.sp   = 1;
		else if(_ifxep->xfer_pkt_done + _ifxep->xfer_pkt_running < _ifxep->xfer_pkt_totl)
			dma_desc_sts.b.sp   = 0;
		else
			dma_desc_sts.b.sp   = send_zero;

		write_desc(dma_desc+0,dma_desc_sts.d32);
		write_desc(dma_desc+1,(uint32_t)(CPHYSADDR(_ifxep->buf)) + _ifxep->xfer_len_done);
		dma_desc_sts.b.bs       = BS_HOST_READY;
		write_desc(dma_desc+0,dma_desc_sts.d32);

		ifxusb_wreg (&(in_regs->diepdma), (uint32_t)(CPHYSADDR(dma_desc)));
	#else //!defined(__DESC_DMA__)
		#if    defined(__UNALIGNED_BUF_ADJ__)
			if(_ifxep->using_aligned_buf)
				ifxusb_wreg (&(in_regs->diepdma), (uint32_t)(CPHYSADDR(_ifxep->aligned_buf+_ifxep->xfer_len_done)));
			else
				ifxusb_wreg (&(in_regs->diepdma), (uint32_t)(CPHYSADDR(_ifxep->buf+_ifxep->xfer_len_done)));
		#elif  defined(__UNALIGNED_BUF_CHK__)
			if (_ifxep->xfer_len_totl && (((unsigned long)_ifxep->buf+_ifxep->xfer_len_done) & _ifxep->ifxpcd->core_if.unaligned_mask))
				IFX_WARN("%s():%d UNALIGNED BUFFER\n",__func__,__LINE__);
			ifxusb_wreg (&(in_regs->diepdma), (uint32_t)(CPHYSADDR(_ifxep->buf+_ifxep->xfer_len_done)));
		#else  //defined(__UNALIGNED_BUF_CHK__)
			ifxusb_wreg (&(in_regs->diepdma), (uint32_t)(CPHYSADDR(_ifxep->buf+_ifxep->xfer_len_done)));
		#endif //defined(__UNALIGNED_BUF_CHK__)
	#endif //!defined(__DESC_DMA__)

	#if   !defined(__DESC_DMA__)
	{
		deptsiz_data_t    deptsiz;

		deptsiz.d32  = ifxusb_rreg(&(in_regs->dieptsiz));
		if(_ifxep->num==0)
		{
			deptsiz.b0.xfersize = _ifxep->xfer_len_running;
			deptsiz.b0.pktcnt   = _ifxep->xfer_pkt_running;
			deptsiz.b0.supcnt = 3;
		}
		else
		{
			deptsiz.b.xfersize = _ifxep->xfer_len_running;
			deptsiz.b.pktcnt   = _ifxep->xfer_pkt_running;
			if (_ifxep->xfer_len_running == 0)
				deptsiz.b.mc=0;
			else if(_ifxep->xfer_pkt_running<4)
				deptsiz.b.mc=_ifxep->xfer_pkt_running;
			else
				deptsiz.b.mc=3;
		}
		ifxusb_wreg(&in_regs->dieptsiz, deptsiz.d32);
	}
	#endif

	depctl.d32 = ifxusb_rreg(&in_regs->diepctl);
	if(depctl.b.epdis)
	{
		unsigned long countdown=100;
		while (depctl.b.epdis  && countdown >0)
		{
			#ifndef __DED_FIFO__
			{
				int i;
				uint32_t dir;
				dir=ifxusb_pcd.core_if.hwcfg1.d32;
				for (i=0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep ; i++,dir>>=2 )
				{
					if((dir&0x00000003)==0 || (dir&0x00000003)==1)
					{
						depctl_data_t diepctltemp;
						diepctltemp.d32 = ifxusb_rreg(&ifxusb_pcd.core_if.in_ep_regs[i]->diepctl);
						diepctltemp.b.nextep=_ifxep->num;
						ifxusb_wreg(&ifxusb_pcd.core_if.in_ep_regs[i]->diepctl,diepctltemp.d32 );
					}
				}
			}
			#endif
			mdelay(1);
			depctl.d32 = ifxusb_rreg(&in_regs->diepctl);
			countdown--;
		}
		if(depctl.b.epdis)
			IFX_WARN("%s() WARNING EP%d is disabling\n",__func__,_ifxep->num);
	}

	#ifndef __DED_FIFO__  // Shared Mode, Handling NEXT field
		#ifdef __DED_INTR__
			//  JUMP
			if (_ifxep->type == IFXUSB_EP_TYPE_CTRL )         //CTRL
			{
				depctl_data_t ctl;
				int nextep=0;
				int i;
				for (i = 1; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
				{
					if(ifxusb_pcd.ifxep[i].is_in)
					{
						ctl.d32 = ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[i]->diepctl);
						if(ctl.b.usbactep && ctl.b.txfnum==0)
						{
							nextep=i;
							ctl.b.nextep = _ifxep->num;
							ifxusb_wreg (&ifxusb_pcd.core_if.in_ep_regs[i]->diepctl,ctl.d32);
						}
					}
				}

				if((_ifxep->xfer_pkt_done + _ifxep->xfer_pkt_running) < _ifxep->xfer_pkt_totl)
					depctl.b.nextep=0;
				else if(ifxusb_pcd.RNDIS_Pending)
					depctl.b.nextep=0;
				else
					depctl.b.nextep=nextep;
			}
			else if(_ifxep->type == IFXUSB_EP_TYPE_INTR)
				depctl.b.nextep=_ifxep->num;
			else // if this is not EP0, then the EP0's NEXT should point to this.
			{
				depctl_data_t ctl;
				ctl.d32 = ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
				ctl.b.nextep = _ifxep->num;
				ifxusb_wreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl,ctl.d32);

				#if 1
					if((_ifxep->xfer_pkt_done + _ifxep->xfer_pkt_running) < _ifxep->xfer_pkt_totl)
						depctl.b.nextep=_ifxep->num;
					else
						depctl.b.nextep=0;
				#endif
			}
		#else
			//  JUMP
			if (_ifxep->type == IFXUSB_EP_TYPE_CTRL )         //CTRL
			{
				depctl_data_t ctl;
				int i;
				for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
				{
					if(ifxusb_pcd.ifxep[i].is_in &&
					    (  ifxusb_pcd.ifxep[i].type == IFXUSB_EP_TYPE_BULK
					    || ifxusb_pcd.ifxep[i].type == IFXUSB_EP_TYPE_INTR
					    )
					  )
					{
						ctl.d32 = ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[i]->diepctl);
						if(ctl.b.usbactep && ctl.b.txfnum==0)
						{
							ctl.b.nextep = _ifxep->num;
							ifxusb_wreg (&ifxusb_pcd.core_if.in_ep_regs[i]->diepctl,ctl.d32);
						}
					}
				}
			}
			else // if this is not EP0, then the EP0's NEXT should point to this.
			{
				depctl_data_t ctl;
				ctl.d32 = ifxusb_rreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl);
				ctl.b.nextep = _ifxep->num;
				ifxusb_wreg (&ifxusb_pcd.core_if.in_ep_regs[0]->diepctl,ctl.d32);
			}
			#if 1
				if((_ifxep->xfer_pkt_done + _ifxep->xfer_pkt_running) < _ifxep->xfer_pkt_totl)
					depctl.b.nextep=_ifxep->num;
				else if (_ifxep->type == IFXUSB_EP_TYPE_INTR )
					depctl.b.nextep=0;
				else if(_ifxep->nextep>=0)
				{
					depctl.b.nextep=_ifxep->nextep;
					_ifxep->nextep=_ifxep->num;
				}
				else if (_ifxep->type == IFXUSB_EP_TYPE_CTRL )         //CTRL
					depctl.b.nextep=0;
	//			else if (_ifxep->type == IFXUSB_EP_TYPE_INTR )
	//				depctl.b.nextep=0;
				else
					depctl.b.nextep=_ifxep->num;
			#endif
		#endif
	#endif
	depctl.b.snak  = 0;
	depctl.b.cnak  = 1;
	depctl.b.epena = 1;
	depctl.b.epdis = 0;
	ifxusb_wreg(&in_regs->diepctl, depctl.d32);
	_ifxep->ep_xfer_stage = InExec;

	#if !defined(__DED_FIFO__) && !defined(__DED_INTR__)
		if(_ifxep->type == IFXUSB_EP_TYPE_INTR)
			ifxusb_pcd.INTR_Pending=0;
	#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 \brief Start a Rx transfer of the EP.
 \param _ifxep        Pointer of ep structure
 */
void ifxpcd_ep_start_rx_transfer(ifxpcd_ep_t *_ifxep)
{
	depctl_data_t   depctl;
	ifxusb_dev_out_ep_regs_t *out_regs = ifxusb_pcd.core_if.out_ep_regs[_ifxep->num];
	uint8_t *trbuf;

	IFX_DEBUGPL((DBG_PCDV | DBG_CILV), "%s()\n", __func__);

	IFX_DEBUGPL(DBG_PCD, "ep%d-%s len=%d/%d pkg=%d/%d zlp=%d\n",
	    _ifxep->num, (_ifxep->is_in?"IN":"OUT"),
	    _ifxep->xfer_len_done,_ifxep->xfer_len_totl,
	    _ifxep->xfer_pkt_done,_ifxep->xfer_pkt_totl,
	    _ifxep->zlp_sp);

	if(!_ifxep->buf)
	{
		IFX_WARN("%s()%d :WARN ep%d-%s rx_buf error\n",__func__,__LINE__,
		    _ifxep->num, (_ifxep->is_in?"IN":"OUT"));
		return;
	}

	_ifxep->xfer_pkt_running=_ifxep->xfer_pkt_totl-_ifxep->xfer_pkt_done;

	trbuf=_ifxep->buf;
	#if    defined(__UNALIGNED_BUF_ADJ__)
		if(_ifxep->using_aligned_buf)
			trbuf=_ifxep->aligned_buf;
	#endif

	if(_ifxep->num==0)
		_ifxep->xfer_pkt_running=1;
	else
		_ifxep->xfer_pkt_running=_ifxep->xfer_pkt_totl;

	_ifxep->xfer_len_running =  _ifxep->xfer_pkt_running * _ifxep->mps;

	trbuf += _ifxep->xfer_len_done;

	depctl.d32 = ifxusb_rreg(&out_regs->doepctl);

	#if defined(__DESC_DMA__)
	{
		desc_sts_data_t dma_desc_sts;
		uint32_t *dma_desc;

		if(_ifxep->num==0)
		{
			ifxusb_pcd.setup_desc_index = 2;
			dma_desc=(uint32_t *)(ifxusb_pcd.setup_desc + ifxusb_pcd.setup_desc_index*2);
		}
		else
			dma_desc=(uint32_t *)(_ifxep->dma_desc);

		dma_desc_sts.b.bs       = BS_HOST_BUSY;
		dma_desc_sts.b.l        = 1;
		dma_desc_sts.b.ioc      = 1;
		dma_desc_sts.b.sp       = 1;
		dma_desc_sts.b.bytes    = _ifxep->xfer_len_running;
		dma_desc_sts.b.mtrf     = 0;

		write_desc(dma_desc+0,dma_desc_sts.d32);
		write_desc(dma_desc+1,(uint32_t)(CPHYSADDR(_ifxep->buf)) + _ifxep->xfer_len_done);
		dma_desc_sts.b.bs       = BS_HOST_READY;
		write_desc(dma_desc+0,dma_desc_sts.d32);

		ifxusb_wreg (&(out_regs->doepdma), (uint32_t)(CPHYSADDR(dma_desc)));
	}
	#else
		ifxusb_wreg (&(out_regs->doepdma), (uint32_t)(CPHYSADDR(trbuf)));
	#endif

	#if !defined(__DESC_DMA__)
		if(_ifxep->num==0)
		{
			deptsiz_data_t deptsiz;
			deptsiz.d32 = ifxusb_rreg(&(out_regs->doeptsiz));
			deptsiz.b0.supcnt = 3;
			deptsiz.b0.xfersize = _ifxep->xfer_len_running;
			deptsiz.b0.pktcnt   = _ifxep->xfer_pkt_running;
			ifxusb_wreg(&out_regs->doeptsiz, deptsiz.d32);
		}
		else
		{
			deptsiz_data_t  deptsiz;
			deptsiz.d32  = ifxusb_rreg(&(out_regs->doeptsiz));
			deptsiz.b.xfersize = _ifxep->xfer_len_running;
			deptsiz.b.pktcnt   = _ifxep->xfer_pkt_running;
			ifxusb_wreg(&out_regs->doeptsiz, deptsiz.d32);
		}
	#endif

	#if 0
		if(_ifxep->num==0){
			doepint_data_t doepint = {.d32=0};
			doepint.b.outtknepdis = 1;
			ifxusb_wreg(&out_regs->doepint, doepint.d32);
		}
	#endif

	depctl.d32 = ifxusb_rreg(&out_regs->doepctl);
	if(depctl.b.epdis)
	{
		unsigned long countdown=100;
		while (depctl.b.epdis  && countdown >0)
		{
			mdelay(1);
			depctl.d32 = ifxusb_rreg(&out_regs->doepctl);
			countdown--;
		}
		if(depctl.b.epdis)
			IFX_WARN("%s() WARNING EP%d is disabling\n",__func__,_ifxep->num);
	}

	depctl.b.snak = 0;
	depctl.b.cnak = 1;
	depctl.b.epena = 1;
	depctl.b.epdis = 0;
	ifxusb_wreg(&out_regs->doepctl, depctl.d32);

	_ifxep->ep_xfer_stage = InExec;
}

