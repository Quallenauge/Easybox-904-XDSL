/*****************************************************************************
 **   FILE NAME       : ifxpcd.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : This file contains the implementation of the PCD Interrupt handlers.
 **
 **                     The PCD handles the device interrupts.  Many conditions can cause a
 **                     device interrupt. When an interrupt occurs, the device interrupt
 **                     service routine determines the cause of the interrupt and
 **                     dispatches handling to the appropriate function. These interrupt
 **                     handling functions are described below.
 **                     All interrupt registers are processed from LSB to MSB.
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
 \file ifxpcd_intr.c
 \ingroup IFXUSB_DRIVER_V3
  \brief This file contains the implementation of the PCD Interrupt handlers.
         The PCD handles the device interrupts.  Many conditions can cause a
         device interrupt. When an interrupt occurs, the device interrupt
         service routine determines the cause of the interrupt and
         dispatches handling to the appropriate function. These interrupt
          handling functions are described below.
          All interrupt registers are processed from LSB to MSB.
*/
#include <linux/version.h>
#include "ifxusb_version.h"

#include <linux/interrupt.h>

#include <linux/dma-mapping.h>

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"
#include "ifxpcd.h"

extern ifxpcd_pcd_t ifxusb_pcd;

//#define USE_MISMATCH_COUNTDOWN 3
#ifdef USE_MISMATCH_COUNTDOWN
	static int mismatch_countdown = USE_MISMATCH_COUNTDOWN;
#endif



/*! \brief This function prints the ep0 state for debug purposes. */
void print_ep0_state(void)
{
	#ifdef __DEBUG__
		char str[40];
		switch (ifxusb_pcd.ep0state)
		{
			case EP0_DISCONNECT:       strcpy(str, "EP0_DISCONNECT");     break;
			case EP0_IDLE:             strcpy(str, "EP0_IDLE");           break;
			case EP0_IN_DATA_PHASE:    strcpy(str, "EP0_IN_DATA_PHASE");  break;
			case EP0_OUT_DATA_PHASE:   strcpy(str, "EP0_OUT_DATA_PHASE"); break;
			case EP0_IN_STATUS_PHASE:  strcpy(str, "EP0_IN_STATUS");      break;
			case EP0_OUT_STATUS_PHASE: strcpy(str, "EP0_OUT_STATUS");     break;
			case EP0_STALL:            strcpy(str, "EP0_STALL");          break;
			default:                   strcpy(str, "EP0_INVALID");
		}
		IFX_DEBUGPL(DBG_ANY, "%s(%d)\n", str, ifxusb_pcd.ep0state);
	#endif
}

#ifndef __DED_FIFO__
	static void DoGNAKNext(ifxusb_core_if_t *_core_if)
	{
		dctl_data_t dctl = {.d32=0};
		int timeout =10 ;

		/* Flush the Tx FIFO */
		ifxusb_flush_tx_fifo_d( _core_if, 0 );
		dctl.d32 = ifxusb_rreg(&_core_if->dev_global_regs->dctl);
		if(dctl.b.gnpinnaksts)
		{
			dctl.d32 = 0;
			dctl.b.cgnpinnak = 1;
			ifxusb_mreg(&_core_if->dev_global_regs->dctl, dctl.d32, dctl.d32);

			dctl.d32 = ifxusb_rreg(&_core_if->dev_global_regs->dctl);
			while(dctl.b.gnpinnaksts)
			{
				if(timeout == 0)
				{
					IFX_WARN("DCTL_GNPIN_NAK_STS_BIT not cleared in 10 usec...\n");
					break;
				}
				timeout--;
				udelay(1);
				dctl.d32 = ifxusb_rreg(&_core_if->dev_global_regs->dctl);
			}
		}

		//Point every EP's Next to be Target
		{
			int i;
			for (i=0; i <= _core_if->hwcfg2.b.num_dev_ep ; i++)
			{
				depctl_data_t diepctl;
				diepctl.d32 = ifxusb_rreg(&_core_if->in_ep_regs[i]->diepctl);
				#ifdef __DED_INTR__
				if(diepctl.b.txfnum==0)
				#endif
				{
					diepctl.b.nextep=ifxusb_pcd.gnaknext;
					diepctl.b.snak = 0;
					diepctl.b.cnak = 0;
					diepctl.b.epena = 0;
					diepctl.b.epdis = 0;
					ifxusb_wreg(&_core_if->in_ep_regs[i]->diepctl,diepctl.d32 );
				}
			}
		}
		ifxusb_pcd.ifxep[ifxusb_pcd.gnaknext].ep_stalled = 0;
		if(ifxusb_pcd.gnaksource==SwitchEP)
			restart_requests();
		else
			tasklet_schedule (ifxusb_pcd.start_xfer_tasklet);
		ifxusb_pcd.gnaksource=0;
		ifxusb_pcd.gnaknext=0;
	}
#endif //__DED_FIFO__

/*! \brief Handler for the IN EP disabled interrupt. */
static inline void handle_in_ep_disable_intr(const uint32_t _epnum)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	ifxpcd_ep_t      *ifxep;

	#if defined(__DESC_DMA__)
		desc_sts_data_t desc_sts;
	#else
		deptsiz_data_t  deptsiz;
	#endif

	ifxep = &ifxusb_pcd.ifxep[ _epnum ];

	IFX_DEBUGPL(DBG_PCD,"diepctl%d=%0x\n", _epnum, ifxusb_rreg(&core_if->in_ep_regs[_epnum]->diepctl));

	#if defined(__DESC_DMA__)
	{
		if(_epnum==0)
			desc_sts.d32 = read_desc((uint32_t *)(ifxusb_pcd.in_desc));
		else
			desc_sts.d32 = read_desc((uint32_t *)(ifxep->dma_desc));
		#ifdef __DEBUG__
			IFX_DEBUGPL(DBG_ANY, "size=%d\n", desc_sts.b.bytes);
		#endif
	}
	#else //!defined(__DESC_DMA__)
		deptsiz.d32 = ifxusb_rreg(&core_if->in_ep_regs[_epnum]->dieptsiz);
		#ifdef __DEBUG__
			if(_epnum==0)
				IFX_DEBUGPL(DBG_ANY, "pktcnt=%d size=%d\n",
				     deptsiz.b0.pktcnt,
				     deptsiz.b0.xfersize );
			else
				IFX_DEBUGPL(DBG_ANY, "pktcnt=%d size=%d\n",
				     deptsiz.b.pktcnt,
				     deptsiz.b.xfersize );
		#endif
	#endif //!defined(__DESC_DMA__)

	if(ifxep->ep_xfer_stage == InExec)
	{
		#if !defined(__DESC_DMA__)
			ifxep->xfer_pkt_done+=(ifxep->xfer_pkt_running-deptsiz.b.pktcnt);
			ifxep->xfer_len_done+=(ifxep->xfer_pkt_running-deptsiz.b.pktcnt)*ifxep->mps;
		#else
			ifxep->xfer_len_done+=ifxep->xfer_len_running - desc_sts.b.bytes;
			if(ifxep->xfer_len_done>=ifxep->xfer_len_totl)
				ifxep->xfer_pkt_done=ifxep->xfer_pkt_totl;
			else
				ifxep->xfer_pkt_done+=(ifxep->xfer_len_running - desc_sts.b.bytes)/ifxep->mps;
		#endif

		if(ifxep->xfer_pkt_done>=ifxep->xfer_pkt_totl)
		{
			ifxpcd_request_t *ifxreq = 0;

			/* Get any pending requests */
			if (!list_empty(&ifxep->queue))
			{
				ifxreq = list_entry(ifxep->queue.next, ifxpcd_request_t, trq);
				if (!ifxreq)
				{
					ifxep->ep_xfer_stage=Idle;
					IFX_WARN("%s() complete_ep 0x%p, req = NULL!\n",__func__,ifxep);
				}
				else
				{
					ifxep->ep_xfer_stage=Cmpt;
					ifxreq->sysreq.actual = ifxep->xfer_len_done;
					request_done(ifxep, ifxreq, 0);
				}
			}
			else
			{
				ifxep->ep_xfer_stage=Idle;
				IFX_WARN("%s() complete_ep 0x%p, ep->queue empty!\n",__func__,ifxep);
			}
		}
		else
			ifxep->ep_xfer_stage=Setup;
	}

	if(!ifxep->ep_act)
		return;

	#if defined(__DED_FIFO__)
		{
			ifxusb_dev_in_ep_regs_t  *in_ep_regs  = core_if->in_ep_regs[_epnum];
			depctl_data_t  depctl;
			depctl.d32=ifxusb_rreg (&in_ep_regs->diepctl);
			depctl.b.epdis=0;
			depctl.b.epena=0;
			ifxusb_wreg (&in_ep_regs->diepctl,depctl.d32);
			ifxusb_flush_tx_fifo_d( core_if, ifxep->tx_fifo_num);
		}
	#else //!defined(__DESC_DMA__)
		if(ifxusb_pcd.gnaksource!=None)	// come from global nak
		{
			int allstopped=1;
			int i;
			ifxep->ep_gnakdisabling=0;
			for (i=0; i <= core_if->hwcfg2.b.num_dev_ep ; i++)
			{
				if(ifxusb_pcd.ifxep[i].ep_gnakdisabling)
					allstopped=0;
			}
			if(allstopped)
				DoGNAKNext(core_if);
			return;
		}
		else if(ifxep->type==IFXUSB_EP_TYPE_INTR)
			ifxep->ep_xfer_stage=Idle;
		else if(ifxep->ep_xfer_stage>0 && !ifxep->ep_stalled)
			ifxpcd_ep_start_tx_transfer(ifxep);
	#endif //!defined(__DESC_DMA__)
}



/*! \brief Handler for the IN EP timeout handshake interrupt. */
static inline void handle_in_ep_timeout_intr(const uint32_t _epnum)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;

	#ifdef __DEBUG__
		deptsiz_data_t dieptsiz = {.d32=0};
	#endif

	dctl_data_t    dctl = {.d32=0};
	ifxpcd_ep_t   *ifxep = &ifxusb_pcd.ifxep[ _epnum ];
	gint_data_t intr_mask = {.d32 = 0};

	IFX_PRINT( "IN-EP %d Timeout\n",_epnum);

	ifxusb_pcd.gnaksource    =Timeout;
	ifxusb_pcd.gnaknext      =_epnum;
	ifxusb_pcd.gnakstopping  =1;
	ifxep->ep_stalled=1;

	/* Set Global IN NAK */
	dctl.d32 = ifxusb_rreg(&core_if->dev_global_regs->dctl);
	if (!dctl.b.gnpinnaksts)
	{
		dctl.d32 = 0;
		dctl.b.sgnpinnak = 1;
		ifxusb_mreg(&core_if->dev_global_regs->dctl, dctl.d32, dctl.d32);
	}

	/* Enable the Global IN NAK Effective Interrupt */
	intr_mask.b.ginnakeff = 1;
	ifxusb_mreg( &core_if->core_global_regs->gintmsk, 0, intr_mask.d32);

	#ifdef __DEBUG__
		dieptsiz.d32 = ifxusb_rreg(&core_if->in_ep_regs[_epnum]->dieptsiz);
		IFX_DEBUGPL(DBG_ANY, "pktcnt=%d size=%d\n",
		    dieptsiz.b.pktcnt,
		    dieptsiz.b.xfersize );
	#endif
}


#if !defined(__DED_FIFO__)
	/*! \brief Handler for the IN EP Mismatch interrupt. */
	static inline void handle_in_ep_mismatch_intr(const uint32_t _epnum)
	{
		ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;

		deptsiz_data_t   dieptsiz = {.d32=0};
		dctl_data_t      dctl = {.d32=0};
		gint_data_t   intr_mask = {.d32 = 0};

		/* Enable the Global IN NAK Effective Interrupt */
		intr_mask.b.ginnakeff = 1;
		ifxusb_mreg( &core_if->core_global_regs->gintmsk, 0, intr_mask.d32);

		/* Set Global IN NAK */
		dctl.d32 = ifxusb_rreg(&core_if->dev_global_regs->dctl);
		if (!dctl.b.gnpinnaksts)
		{
			ifxusb_pcd.gnaksource=Mismatch;
			ifxusb_pcd.gnaknext  =_epnum;
			dctl.b.sgnpinnak = 1;
			ifxusb_mreg(&core_if->dev_global_regs->dctl, dctl.d32, dctl.d32);
		}

		#ifdef __DEBUG__
			dieptsiz.d32 = ifxusb_rreg(&core_if->in_ep_regs[_epnum]->dieptsiz);
			IFX_DEBUGPL(DBG_ANY, "pktcnt=%d size=%d\n",
			    dieptsiz.b.pktcnt,
			    dieptsiz.b.xfersize );
		#endif
	}
#endif  //!defined(__DED_FIFO__)

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



void ifxpcd_ep_start_intr_transfer(void);
/*!
 \fn static int32_t handle_inepintr(uint32_t epnum,uint32_t  data)
 \brief This interrupt indicates that an IN EP has a pending Interrupt.
        The sequence for handling the IN EP interrupt is shown below:
          - Read the Device All Endpoint Interrupt register
          - Repeat the following for each IN EP interrupt bit set (from
            LSB to MSB).
          - Read the Device Endpoint Interrupt (DIEPINTn) register
          - If "Transfer Complete" call the request complete function
          - If "Endpoint Disabled" complete the EP disable procedure.
          - If "AHB Error Interrupt" log error
          - If "Time-out Handshake" log error
          - If "IN Token Received when TxFIFO Empty" write packet to Tx
            FIFO.
          - If "IN Token EP Mismatch" (disable, this is handled by EP
            Mismatch Interrupt)
  \param  epnum End point number
  \param data IN EP Interrupt register and Interrupt Mask
  \return 1 OK
  \ingroup  IFXUSB_PCD
 */
static int32_t handle_inepintr(uint32_t epnum,uint32_t  data,uint32_t  mask)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	ifxpcd_ep_t *ifxep;
	depctl_data_t diepctl  = {.d32=0};
	diepint_data_t diepint,diepintdata,diepintmsk;

	ifxep = &ifxusb_pcd.ifxep[ epnum ];
	diepintdata.d32=data;
	diepintmsk.d32=mask;
	diepint.d32=data&mask;
	
	#ifdef __VERBOSE__
		IFX_DEBUGPL(DBG_PCDV,
		    "EP%d-%s: type=%d, mps=%d\n",
		    ifxep->num, (ifxep->is_in ?"IN":"OUT"),
		    ifxep->type, ifxep->mps );
	#endif

	#ifdef __QUICKNAK__  /// Make this IN EP send NAK
		diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[epnum]->diepctl);
		diepctl.b.snak=1;
		diepctl.b.cnak=0;
		ifxusb_wreg(&core_if->in_ep_regs[epnum]->diepctl,diepctl.d32);
	#endif

	/* Endpoint disable  */
	if ( diepint.b.epdisabled )
	{
		IFX_DEBUGPL(DBG_ANY,"EP%d IN disabled\n", epnum);
		#ifdef USE_MISMATCH_COUNTDOWN
			mismatch_countdown = USE_MISMATCH_COUNTDOWN;
		#endif
		handle_in_ep_disable_intr(epnum );
		/* Clear the bit in DIEPINTn for this interrupt */
		CLEAR_IN_EP_INTR(epnum,epdisabled);
	}

	/* AHB Error */
	if ( diepint.b.ahberr )
	{
		IFX_DEBUGPL(DBG_ANY,"EP%d IN AHB Error\n", epnum);
		/* Clear the bit in DIEPINTn for this interrupt */
		CLEAR_IN_EP_INTR(epnum,ahberr);
	}

	/* TimeOUT Handshake (non-ISOC IN EPs) */
	if ( diepint.b.timeout )
	{
		IFX_DEBUGPL(DBG_ANY,"EP%d IN Time-out\n", epnum);
		handle_in_ep_timeout_intr( epnum );
		CLEAR_IN_EP_INTR(epnum,timeout);
	}

	/** IN Token Received with EP mismatch */
	#ifndef __DED_FIFO__
		if (diepint.b.intknepmis)
		{
			IFX_DEBUGPL(DBG_ANY,"EP%d IN TKN EP Mismatch\n", epnum);
			#ifndef __DED_FIFO__
			{
				gnptxsts_data_t txstatus = {.d32 = 0};
				depctl_data_t ctl0,ctl1,ctl3;
				txstatus.d32=ifxusb_rreg(&core_if->core_global_regs->gnptxsts );
				ctl0.d32=ifxusb_rreg(&core_if->in_ep_regs[0]->diepctl);
				ctl1.d32=ifxusb_rreg(&core_if->in_ep_regs[1]->diepctl);
				ctl3.d32=ifxusb_rreg(&core_if->in_ep_regs[3]->diepctl);
//				MDELAY(10);
				diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[epnum]->diepctl);
				#ifdef USE_MISMATCH_COUNTDOWN
					if(mismatch_countdown >0)
						mismatch_countdown --;
					else
				#endif
					if (diepctl.b.epena)
					{
						handle_in_ep_mismatch_intr( epnum);
						#ifdef USE_MISMATCH_COUNTDOWN
							mismatch_countdown = USE_MISMATCH_COUNTDOWN;
						#endif
					}
			}
			#endif //__DED_FIFO__
			CLEAR_IN_EP_INTR(epnum,intknepmis);
		}
	#endif

	/** IN Token received with TxF Empty */
	if (diepint.b.intktxfemp)
	{
		IFX_DEBUGPL(DBG_ANY,"EP%d IN TKN TxFifo Empty\n",epnum);
		#ifdef USE_MISMATCH_COUNTDOWN
			mismatch_countdown = USE_MISMATCH_COUNTDOWN;
		#endif
		if (!ifxep->ep_stalled && epnum != 0)
		{
			diepint_data_t diepmsk = { .d32 = 0};
			diepmsk.b.intktxfemp = 1;
			ifxusb_mreg(&core_if->dev_global_regs->diepmsk, diepmsk.d32, 0);
			start_next_request(ifxep);
		}
		#ifdef __DED_FIFO__
			else if(epnum == 0 && ifxusb_pcd.ep0state == EP0_OUT_STATUS_PHASE)
			{
				// EP0 IN set STALL
				diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[epnum]->diepctl);
				/* set the disable and stall bits */
				if (diepctl.b.epena)
					diepctl.b.epdis = 1;
				diepctl.b.stall = 1;
				ifxusb_wreg(&core_if->in_ep_regs[epnum]->diepctl, diepctl.d32);
			}
		#endif //__DED_FIFO__
		CLEAR_IN_EP_INTR(epnum,intktxfemp);
	}

	/** IN Endpoint HAK Effective */
	if (diepint.b.inepnakeff)
	{
		IFX_DEBUGPL(DBG_ANY,"EP%d IN EP NAK Effective\n", epnum);
		CLEAR_IN_EP_INTR(epnum,inepnakeff);
		MASK_IN_EP_INTR(inepnakeff);
	}

	/** IN EP Tx FIFO Empty Intr */
	if (diepint.b.emptyintr)
	{
		IFX_DEBUGPL(DBG_ANY,"EP%d Tx FIFO Empty Intr \n", epnum);
		MASK_IN_EP_INTR(emptyintr);
		CLEAR_IN_EP_INTR(epnum,emptyintr);
	}

	/** IN EP BNA Intr */
	if (diepint.b.bna)
	{
		#ifdef __DED_FIFO__
			{
				dctl_data_t dctl = {.d32=0};
				dctl.d32 = ifxusb_rreg(&core_if->dev_global_regs->dctl);
				/* If Global Continue on BNA is disabled - disable EP */
IFX_DEBUGP("IN BNA\n");
				if(!dctl.b.gcontbna)
				{
					diepctl.d32 = 0;
					diepctl.b.snak = 1;
					diepctl.b.epdis = 1;
					ifxusb_wreg(&core_if->in_ep_regs[epnum]->diepctl, diepctl.d32);
				}
			}
		#else
			MASK_IN_EP_INTR(bna);
		#endif
		CLEAR_IN_EP_INTR(epnum,bna);
	}

	/* Transfer complete */
	if ( diepint.b.xfercompl )
	{
		IFX_DEBUGPL(DBG_PCD,"EP%d IN Xfer Complete\n", epnum);
		#ifdef __DED_FIFO__
			/* Disable the Tx FIFO Empty Interrupt for this EP */
			{
				uint32_t fifoemptymsk = 0x1 << epnum;
				ifxusb_mreg(&core_if->dev_global_regs->dtknqr4_fifoemptymsk, fifoemptymsk, 0);
			}
		#else
			{
				gint_data_t intr_mask = {.d32 = 0};
				intr_mask.b.nptxfempty = 1;
				ifxusb_mreg( &core_if->core_global_regs->gintmsk, intr_mask.d32, 0);
			}
		#endif
		/* Clear the bit in DIEPINTn for this interrupt */
		CLEAR_IN_EP_INTR(epnum,xfercompl);
		/* Complete the transfer */
		diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[epnum]->diepctl);
		if (diepctl.b.epena)
			IFX_WARN("IN EP%d COMPLETE WITH ENABLE\n",epnum);

		if (epnum == 0)
			handle_ep0_tx();
		else
			complete_ep_tx( ifxep );

		#if defined(USE_MISMATCH_COUNTDOWN) && !defined(__DED_FIFO__)
			mismatch_countdown = USE_MISMATCH_COUNTDOWN;
		#endif
	}
	return 1;
}

/*!
 \fn static int32_t handle_outepintr(uint32_t epnum,uint32_t  data)
 \brief This interrupt indicates that an OUT EP has a pending Interrupt.
        The sequence for handling the OUT EP interrupt is shown below:
          - Read the Device All Endpoint Interrupt register
          - Repeat the following for each OUT EP interrupt bit set (from
            LSB to MSB).
          - Read the Device Endpoint Interrupt (DOEPINTn) register
          - If "Transfer Complete" call the request complete function
          - If "Endpoint Disabled" complete the EP disable procedure.
          - If "AHB Error Interrupt" log error
          - If "Setup Phase Done" process Setup Packet (See Standard USB
            Command Processing)
  \param  epnum End point number
  \param data OUT EP Interrupt register and Interrupt Mask
  \ingroup  IFXUSB_PCD
 */
static int32_t handle_outepintr(uint32_t epnum,uint32_t  data,uint32_t  mask)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	ifxpcd_ep_t *ifxep;
	doepint_data_t doepint,doepintdata,doepintmsk;

	ifxep = &ifxusb_pcd.ifxep[ epnum ];
	doepintdata.d32=data;
	doepintmsk.d32=mask;
	doepint.d32=data&mask;

	#ifdef __VERBOSE__
		IFX_DEBUGPL(DBG_PCDV,
		    "EP%d-%s: type=%d, mps=%d\n",
		    ifxep->num, (ifxep->is_in ?"IN":"OUT"),
		    ifxep->type, ifxep->mps );
	#endif


	/* Endpoint disable  */
	if ( doepint.b.epdisabled )
	{
		IFX_DEBUGPL(DBG_PCD,"EP%d OUT disabled\n", epnum);
		/* Clear the bit in DOEPINTn for this interrupt */
		CLEAR_OUT_EP_INTR(epnum,epdisabled);
		MASK_OUT_EP_INTR(epdisabled);
	}

	/* AHB Error */
	if ( doepint.b.ahberr )
	{
		IFX_DEBUGPL(DBG_PCD,"EP%d OUT AHB Error\n", epnum);
		CLEAR_OUT_EP_INTR(epnum,ahberr);
	}

	/* OUT Token Received when EndPoint is disabled */
	if ( doepint.b.outtknepdis )
	{
		IFX_DEBUGPL(DBG_PCD,"EP%d OUT Token Received when EndPoint is disabled\n", epnum);
		/* Clear the bit in DOEPINTn for this interrupt */
		CLEAR_OUT_EP_INTR(epnum,outtknepdis);
		MASK_OUT_EP_INTR(outtknepdis);
		/* Currently do nothing */
	}

	#if !defined(__DESC_DMA__)
		/* Setup Phase Done (contorl EPs) */
		if ( doepint.b.setup )
		{
			#ifdef __DEBUG_EP0__
				IFX_DEBUGPL(DBG_PCD,"EP%d SETUP Done\n", epnum);
			#endif
			handle_ep0_setup();
		}
	#endif

	/** OUT EP BNA Intr */
	if (doepint.b.bna)
	{
		#ifdef __DED_FIFO__
		{
			dctl_data_t dctl = {.d32=0};
			depctl_data_t doepctl = {.d32=0};
IFX_DEBUGP("OUT BNA\n");
			dctl.d32=ifxusb_rreg(&core_if->dev_global_regs->dctl);
			if(!dctl.b.gcontbna)
			{
				doepctl.b.snak = 1;
				doepctl.b.epdis = 1;
				ifxusb_mreg(&core_if->out_ep_regs[epnum]->doepctl, doepctl.d32, doepctl.d32);
			}
		}
		#else
			MASK_OUT_EP_INTR(bna);
		#endif
		CLEAR_OUT_EP_INTR(epnum,bna);
	}
	
	if(epnum==0)
	{
		if     ( doepintdata.b.stsphsercvd)
		{
			#if defined(__DESC_DMA__)
				if(!doepintdata.b.setup && !doepintdata.b.xfercompl) // Case D
				{
					IFX_DEBUGPL(DBG_PCD,"EP%d OUT Xfer Complete Case D\n", epnum);
					/* Clear the bit in DOEPINTn for this interrupt */
					#ifndef __DED_FIFO__
					{
						depctl_data_t doepctl = {.d32=0};
						doepctl.d32=ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);
						doepctl.b.snak= 1;
						doepctl.b.cnak= 0;
						ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);
					}
					#endif
					handle_ep0_rx(4);
				}
				else if(!doepintdata.b.setup &&  doepintdata.b.xfercompl) // Case E
				{
					IFX_DEBUGPL(DBG_PCD,"EP%d OUT Xfer Complete Case E\n", epnum);
					/* Clear the bit in DOEPINTn for this interrupt */
					#ifndef __DED_FIFO__
					{
						depctl_data_t doepctl = {.d32=0};
						doepctl.d32=ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);
						doepctl.b.snak= 1;
						doepctl.b.cnak= 0;
						ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);
					}
					#endif
					handle_ep0_rx(5);
				}
				else
					IFX_WARN("%s() %d Unattend condition %08X\n",__func__,__LINE__,doepintdata.d32);
				CLEAR_OUT_EP_INTR(epnum,stsphsercvd);
				CLEAR_OUT_EP_INTR(epnum,setup);
				CLEAR_OUT_EP_INTR(epnum,xfercompl);
			#else
				CLEAR_OUT_EP_INTR(epnum,stsphsercvd);
//				#ifdef __DED_FIFO__
//					/* Start Control Status Phase */
//					do_setup_in_status_phase();
//				#endif
			#endif
		}
		else
		{
			if     ( !doepintdata.b.setup &&  doepintdata.b.xfercompl) // Case A
			{
				IFX_DEBUGPL(DBG_PCD,"EP%d OUT Xfer Complete Case A\n", epnum);
				/* Clear the bit in DOEPINTn for this interrupt */
				CLEAR_OUT_EP_INTR(epnum,xfercompl);
				#ifndef __DED_FIFO__
				{
					depctl_data_t doepctl = {.d32=0};
					doepctl.d32=ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);
					doepctl.b.snak= 1;
					doepctl.b.cnak= 0;
					ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);
				}
				#endif
				handle_ep0_rx(1);
			}
			else if( doepintdata.b.setup && !doepintdata.b.xfercompl) // Case B
			{
				IFX_DEBUGPL(DBG_PCD,"EP%d OUT Xfer Complete Case B\n", epnum);
				/* Clear the bit in DOEPINTn for this interrupt */
				CLEAR_OUT_EP_INTR(epnum,setup);
				#ifndef __DED_FIFO__
				{
					depctl_data_t doepctl = {.d32=0};
					doepctl.d32=ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);
					doepctl.b.snak= 1;
					doepctl.b.cnak= 0;
					ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);
				}
				#endif
				handle_ep0_rx(2);
			}
			else if( doepintdata.b.setup &&  doepintdata.b.xfercompl) // Case C
			{
				IFX_DEBUGPL(DBG_PCD,"EP%d OUT Xfer Complete Case C\n", epnum);
				/* Clear the bit in DOEPINTn for this interrupt */
				CLEAR_OUT_EP_INTR(epnum,setup);
				CLEAR_OUT_EP_INTR(epnum,xfercompl);
				#ifndef __DED_FIFO__
				{
					depctl_data_t doepctl = {.d32=0};
					doepctl.d32=ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);
					doepctl.b.snak= 1;
					doepctl.b.cnak= 0;
					ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);
				}
				#endif
				handle_ep0_rx(3);
			}
		}
	}
	else if ( doepint.b.xfercompl )
	{
		IFX_DEBUGPL(DBG_PCD,"EP%d OUT Xfer Complete\n", epnum);
		/* Clear the bit in DOEPINTn for this interrupt */
		CLEAR_OUT_EP_INTR(epnum,xfercompl);
		complete_ep_rx( ifxep );
	}
	
	return 1;
}

/*! \brief This function handles the Global IN NAK Effective interrupt. */
static inline int32_t handle_ginnakeff(void)    //06
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;
	gint_data_t intr_mask = { .d32 = 0};
	depctl_data_t diepctl = { .d32 = 0};
	gint_data_t   gintsts  = {.d32=0};

	int i;
	int n=0;

	IFX_DEBUGPL(DBG_PCD, "Global IN NAK Effective\n");

	/* Disable the Global IN NAK Effective Interrupt */
	intr_mask.b.ginnakeff = 1;
	ifxusb_mreg( &core_if->core_global_regs->gintmsk, intr_mask.d32, 0);

	for (i=0; i <= core_if->hwcfg2.b.num_dev_ep ; i++)
	{
		#if !defined(__DED_FIFO__)
			ifxusb_pcd.ifxep[i].ep_gnakdisabling=0;
		#endif

		diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[i]->diepctl);
		if (diepctl.b.epena)
		{
			diepctl.b.epdis = 1;
			diepctl.b.snak  = 1;
			diepctl.b.epena = 0;
			diepctl.b.cnak  = 0;
			ifxusb_wreg( &core_if->in_ep_regs[i]->diepctl, diepctl.d32 );
			#if !defined(__DED_FIFO__)
				ifxusb_pcd.ifxep[i].ep_gnakdisabling=1;
			#endif
			n++;
		}
	}

	#if !defined(__DED_FIFO__)
		if(n>0)
		{
			for (i=0; i <= core_if->hwcfg2.b.num_dev_ep ; i++)
			{
				if(ifxusb_pcd.ifxep[i].ep_gnakdisabling)
				{
					diepctl.d32 = ifxusb_rreg(&core_if->in_ep_regs[i]->diepctl);
					if(diepctl.b.epena)
					{
						diepctl.b.epdis = 1;
						diepctl.b.epena = 0;
						if(diepctl.b.naksts)
							diepctl.b.snak = 0;
						else
							diepctl.b.snak = 1;
						diepctl.b.cnak = 0;
						ifxusb_wreg( &core_if->in_ep_regs[i]->diepctl, diepctl.d32 );
					}
				}
			}
		}
	#endif //!__DED_FIFO__

	#if !defined(__DED_FIFO__)
		if(n==0 && ifxusb_pcd.gnaksource != None && ifxusb_pcd.gnaknext>=0)
			DoGNAKNext(core_if);
	#endif //__DED_FIFO__

	gintsts.b.ginnakeff = 1;
	ifxusb_wreg( &core_if->core_global_regs->gintsts, gintsts.d32);
	return 1;
}


/*!
 \brief This interrupt occurs when a USB Reset is detected.  When the USB
        Reset Interrupt occurs the device state is set to DEFAULT and the
        EP0 state is set to IDLE.
 */
static inline int32_t handle_usbreset(void)
{
	gint_data_t gintsts  = {.d32=0};

	IFX_PRINT("USB RESET RECEIVED!\n");

	#ifdef USE_MISMATCH_COUNTDOWN
		mismatch_countdown = USE_MISMATCH_COUNTDOWN;
	#endif

	ifxpcd_usbreset();

	/* Clear interrupt */
	gintsts.b.usbreset = 1;
	ifxusb_wreg (&ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

/*!
 \brief Get the device speed from the device status register and convert it
        to USB speed constant.
 */
static inline int get_device_speed(void)
{
	dsts_data_t dsts;
	enum usb_device_speed speed = USB_SPEED_UNKNOWN;

	dsts.d32 = ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->dsts);
	switch (dsts.b.enumspd)
	{
		case IFXUSB_DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ:
			speed = USB_SPEED_HIGH;
			break;
		case IFXUSB_DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ:
		case IFXUSB_DSTS_ENUMSPD_FS_PHY_48MHZ:
			speed = USB_SPEED_FULL;
			break;
		case IFXUSB_DSTS_ENUMSPD_LS_PHY_6MHZ:
			speed = USB_SPEED_LOW;
			break;
	}
	return speed;
}

/*!
 \brief Read the device status register and set the device speed in the
        data structure.
        Set up EP0 to receive SETUP packets by calling dwc_ep0_activate.
 */
static inline int32_t handle_enumdone(void)
{
	ifxpcd_ep_t *ep0 = &ifxusb_pcd.ifxep[0];
	gint_data_t gintsts  = {.d32=0};
	gusbcfg_data_t gusbcfg;
	ifxusb_core_global_regs_t *global_regs = ifxusb_pcd.core_if.core_global_regs;

	IFX_DEBUGPL(DBG_PCD, "SPEED ENUM\n");

//	ifxpcd_ep0_activate();

	#ifdef __DEBUG_EP0__
		print_ep0_state();
	#endif

//	if (ifxusb_pcd.ep0state == EP0_DISCONNECT)
//		ifxusb_pcd.ep0state = EP0_IDLE;
//	else if (ifxusb_pcd.ep0state == EP0_STALL)
//		ifxusb_pcd.ep0state = EP0_IDLE;

	ifxusb_pcd.ep0state = EP0_IDLE;
	ep0->ep_stalled = 0;

	ifxusb_pcd.gadget.speed = get_device_speed();

	/* Set USB turnaround time based on device speed and PHY interface. */
	gusbcfg.d32 = ifxusb_rreg(&global_regs->gusbcfg);
	if (ifxusb_pcd.gadget.speed == USB_SPEED_HIGH)
	{
		gusbcfg.b.usbtrdtim = ifxusb_pcd.core_if.params.turn_around_time_hs;
		gusbcfg.b.toutcal   = ifxusb_pcd.core_if.params.timeout_cal_hs;
	}
	else /* Full or low speed */
	{
		gusbcfg.b.usbtrdtim = ifxusb_pcd.core_if.params.turn_around_time_fs;
		gusbcfg.b.toutcal   = ifxusb_pcd.core_if.params.timeout_cal_fs;
	}
	ifxusb_wreg(&global_regs->gusbcfg, gusbcfg.d32);

	/* Clear interrupt */
	gintsts.b.enumdone = 1;
	ifxusb_wreg( &global_regs->gintsts, gintsts.d32 );

	ep0->ep_act=1;

	{
		gint_data_t gintmsk = { .d32 = 0 };
		gintmsk.b.sofintr = 1;
		ifxusb_mreg( &global_regs->gintmsk,0, gintmsk.d32);
	}

	ifxusb_pcd.enumdone=1;
	return 1;
}


/*!
 \brief This interrupt indicates that SUSPEND state has been detected on
        the USB.
 */
static inline int32_t handle_usbsuspend(void)
{
	gint_data_t gintsts = {.d32=0};

	IFX_DEBUGPL(DBG_PCD,"USB SUSPEND RECEIVED!\n");

	ifxpcd_suspend();

	gintsts.b.usbsuspend = 1;
	ifxusb_wreg(&ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}


/*!
 \brief This interrupt indicates that the USB controller has detected a
        resume or remote wakeup sequence. If the USB controller is in
        low power mode, the handler must brings the controller out of low
        power mode. The controller automatically begins resume
        signaling. The handler schedules a time to stop resume signaling.
 */
static inline int32_t handle_wkupintr(void)
{
	gint_data_t gintsts = {.d32=0};
	dctl_data_t dctl = {.d32=0};

	IFX_DEBUGPL(DBG_ANY, "++Resume and Remote Wakeup Detected Interrupt++\n");
	IFX_DEBUGPL(DBG_PCD, "DSTS=0x%0x\n", ifxusb_rreg( &ifxusb_pcd.core_if.dev_global_regs->dsts));

	/* Clear the Remote Wakeup Signalling */
	dctl.b.rmtwkupsig = 1;
	ifxusb_mreg( &ifxusb_pcd.core_if.dev_global_regs->dctl, dctl.d32, 0 );

	ifxpcd_resume();

	gintsts.b.wkupintr = 1;
	ifxusb_wreg(&ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

static inline int32_t handle_erlysuspend(void)  //10
{
	gint_data_t gintsts = { .d32 = 0 };
	#if defined(__VERBOSE__)
		IFX_DEBUGPL(DBG_PCD,"Early Suspend Detected\n");
	#endif
	gintsts.b.erlysuspend = 1;
	ifxusb_wreg( &ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

static inline int32_t handle_epmismatch(void)
{
	gint_data_t     gintsts = { .d32 = 0 };
	//IFX_WARN("Global EP Mismatch Interrup\n");
	gintsts.b.epmismatch = 1;
	ifxusb_wreg(&ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

static inline int32_t handle_otgintr(void)      //02
{
	ifxusb_core_global_regs_t *global_regs = ifxusb_pcd.core_if.core_global_regs;
	gotgint_data_t gotgint;
	gint_data_t intr_mask = { .d32 = 0};

	intr_mask.b.otgintr = 1;
	ifxusb_mreg( &global_regs->gintmsk, intr_mask.d32, 0);

	gotgint.d32 = ifxusb_rreg( &global_regs->gotgint);
	ifxusb_wreg (&global_regs->gotgint, gotgint.d32);
	return 1;
}

static inline int32_t handle_modemismatch(void)
{
	gint_data_t gintsts = { .d32 = 0 };
	IFX_WARN("Mode Mismatch Interrupt: currently in %s mode\n",
	     ifxusb_mode(&ifxusb_pcd.core_if) ? "Host" : "Device");
	gintsts.b.modemismatch = 1;
	ifxusb_wreg(&ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	return 1;
}

static inline int32_t handle_sofintr(void)
{
	gint_data_t gintsts = { .d32 = 0 };
	#if defined(__VERBOSE__)
		IFX_DEBUGPL(DBG_PCD,"SOF Detected\n");
	#endif

	if(ifxusb_pcd.enumdone)
	{
		ifxusb_pcd.enumdone=0;
		gintsts.b.sofintr = 1;
		ifxusb_wreg( &ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
		ifxusb_mreg( &ifxusb_pcd.core_if.core_global_regs->gintmsk, gintsts.d32,0);
		ep0_out_start();
	}
	else
	{
		gintsts.b.sofintr = 1;
		ifxusb_wreg( &ifxusb_pcd.core_if.core_global_regs->gintsts, gintsts.d32);
	}
	return 1;
}


/*!
 \brief PCD interrupt handler.
        The PCD handles the device interrupts.  Many conditions can cause a
        device interrupt. When an interrupt occurs, the device interrupt
        service routine determines the cause of the interrupt and
        dispatches handling to the appropriate function. These interrupt
        handling functions are described below.

        All interrupt registers are processed from LSB to MSB.
 */
#define eptotal 15
int32_t ifxpcd_handle_intr(void)
{
	ifxusb_core_global_regs_t *global_regs = ifxusb_pcd.core_if.core_global_regs;
	gint_data_t    gintr_proc,gintr_status,gintr_mask;
	daint_data_t   daint_proc,daint_status,daint_mask;
	doepint_data_t doepint_proc[eptotal],doepint_status[eptotal],doepint_mask;
	diepint_data_t diepint_proc[eptotal],diepint_status[eptotal],diepint_mask;

	int32_t retval = 0;

	SPIN_LOCK(&ifxusb_pcd.lock);
	gintr_status.d32 =ifxusb_rreg(&global_regs->gintsts);
	gintr_mask.d32   =ifxusb_rreg(&global_regs->gintmsk);
	gintr_proc.d32   = gintr_status.d32 & gintr_mask.d32;

	daint_status.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->daint);
	daint_mask.d32   =ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->daintmsk);
	daint_proc.d32   = daint_status.d32 & daint_mask.d32;

	doepint_mask.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->doepmsk);
	diepint_mask.d32 =ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->diepmsk);

	{
		int epnum;
		uint32_t msk=0x00000001;
		for(epnum=0;epnum<eptotal;epnum++,msk<<=1)
		{
			diepint_status[epnum].d32=ifxusb_rreg(&ifxusb_pcd.core_if.in_ep_regs[epnum]->diepint);
			doepint_status[epnum].d32=ifxusb_rreg(&ifxusb_pcd.core_if.out_ep_regs[epnum]->doepint);
			diepint_proc[epnum].d32 = diepint_status[epnum].d32 & diepint_mask.d32;
			doepint_proc[epnum].d32 = doepint_status[epnum].d32 & doepint_mask.d32;
			if(!diepint_proc[epnum].d32)
				daint_proc.eps.in &= (~msk) ;
			if(!doepint_proc[epnum].d32)
				daint_proc.eps.out&= (~msk) ;
		}
		if(!daint_proc.eps.in)
			gintr_proc.b.inepintr=0;
		if(!daint_proc.eps.out)
			gintr_proc.b.outepintr=0;
	}

	if (!gintr_proc.d32)
	{
		SPIN_UNLOCK(&ifxusb_pcd.lock);
		return 0;
	}

	IFX_DEBUGPL(DBG_PCDV, "%s: gintsts =0x%08X mask=0x%08X proc=0x%08X\n", __func__, gintr_status.d32,gintr_mask.d32,gintr_proc.d32 );

	if (gintr_proc.b.modemismatch) //01
	{
		retval |= handle_modemismatch();
		gintr_proc.b.modemismatch=0;
	}
	if (gintr_proc.b.otgintr)      //02
	{
		retval |= handle_otgintr( );
		gintr_proc.b.otgintr=0;
	}
	if (gintr_proc.b.wkupintr)     //31
	{
		retval |= handle_wkupintr();
		gintr_proc.b.wkupintr=0;
	}
	if (gintr_proc.b.usbsuspend)  //11
	{
		retval |= handle_usbsuspend();
		gintr_proc.b.usbsuspend=0;
	}
	if (gintr_proc.b.ginnakeff)   //06
	{
		retval |= handle_ginnakeff();
		gintr_proc.b.ginnakeff=0;
	}
	if (gintr_proc.b.erlysuspend) //10
	{
		retval |= handle_erlysuspend();
		gintr_proc.b.erlysuspend=0;
	}
	if (gintr_proc.b.sofintr) //3
	{
		retval |= handle_sofintr();
		gintr_proc.b.sofintr=0;
	}
	if (gintr_proc.b.usbreset)    //12
	{
		retval |= handle_usbreset();
		gintr_proc.b.usbreset=0;
	}
	if (gintr_proc.b.enumdone)    //13
	{
		retval |= handle_enumdone();
		gintr_proc.b.enumdone=0;
	}
	if (gintr_proc.b.epmismatch) //17
	{
		retval |= handle_epmismatch();
		gintr_proc.b.epmismatch=0;
	}
	if (gintr_proc.b.inepintr || gintr_proc.b.outepintr) //18 & 19
	{
		int epnum;
		uint32_t ep_intr_o,ep_intr_i;
		epnum = 0;

		ep_intr_o= daint_proc.eps.out;
		ep_intr_i= daint_proc.eps.in;

		while(epnum < eptotal &&( ep_intr_i || ep_intr_o ))
		{
			if (ep_intr_i&0x1)
				retval |= handle_inepintr(epnum,diepint_status[epnum].d32 ,diepint_mask.d32);
			if (ep_intr_o&0x1)
				retval |= handle_outepintr(epnum,doepint_status[epnum].d32 ,doepint_mask.d32);
			epnum++;
			ep_intr_i>>=1;
			ep_intr_o>>=1;
		}
		gintr_proc.b.inepintr=0;
		gintr_proc.b.outepintr=0;
	}

//These are unattended interrupts
//sessreqintr       //30
//disconnect        //29
//conidstschng      //28
//reserved27        //27
//ptxfempty         //26
//hcintr            //25
//portintr          //24
//reserved22_23     //223
//incomplisoout     //21
//incomplisoin      //20
//intokenrx         //16
//eopframe          //15
//isooutdrop        //14
//i2cintr           //09
//reserved8         //08
//goutnakeff        //07
//curmode           //00

	if (gintr_proc.d32)
	{
		IFX_DEBUGPL(DBG_PCDV, "%s(), Unprocessed INT = 0x%08x msk:0x%08x Proc:0x%08x \n",__func__, gintr_status.d32,gintr_mask.d32,gintr_proc.d32);
		ifxusb_wreg(&global_regs->gintsts, gintr_proc.d32);
		ifxusb_mreg(&global_regs->gintmsk, gintr_proc.d32,0);
	}
	SPIN_UNLOCK(&ifxusb_pcd.lock);

	return retval;
}

