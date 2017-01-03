/*****************************************************************************
 **   FILE NAME       : ifxpcd_setup.c
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
 \file ifxpcd_setup.c
 \ingroup IFXUSB_DRIVER_V3
 \brief Routines to deal with EP0 SETUP commands
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
 \brief This function configures EPO to receive SETUP packets.
        - Program the following fields in the endpoint specific registers
          for Control OUT EP 0, in order to receive a setup packet
        - DOEPTSIZ0.Packet Count = 3 (To receive up to 3 back to back
          setup packets)
        - DOEPTSIZE0.Transfer Size = 24 Bytes (To receive up to 3 back
          to back setup packets)
        - In DMA mode, DOEPDMA0 Register with a memory address to
          store any setup packets received

 */

void ep0_out_start(void)
{
	ifxusb_core_if_t *core_if= &ifxusb_pcd.core_if;
	depctl_data_t doepctl;

	doepctl.d32 = ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);

	#ifdef __VERBOSE__
		IFX_DEBUGPL(DBG_PCDV,"%s() doepctl0=%0x\n", __func__, doepctl.d32);
	#endif

	if(doepctl.b.epena)
		return;

	#if defined(__DESC_DMA__)
	{
		desc_sts_data_t dma_desc_sts;
		uint32_t *dma_desc;
		deptsiz_data_t doeptsize0;

		doeptsize0.d32 = ifxusb_rreg( &core_if->out_ep_regs[0]->doeptsiz);
		doeptsize0.b0.supcnt = 3;
		ifxusb_wreg( &core_if->out_ep_regs[0]->doeptsiz, doeptsize0.d32 );

		ifxusb_pcd.setup_desc_index = (ifxusb_pcd.setup_desc_index + 1) & 1;
		dma_desc=(uint32_t *)(ifxusb_pcd.setup_desc + ifxusb_pcd.setup_desc_index*2);

		/** DMA Descriptor Setup */
		dma_desc_sts.b.bs       = BS_HOST_READY;
		dma_desc_sts.b.l        = 1;
		dma_desc_sts.b.ioc      = 1;
		dma_desc_sts.b.mtrf     = 0;
		dma_desc_sts.b.bytes    = ifxusb_pcd.ifxep[0].mps;
		write_desc(dma_desc+0,dma_desc_sts.d32);

		/** DOEPDMA0 Register write */
		ifxusb_wreg(&core_if->out_ep_regs[0]->doepdma, (uint32_t)(CPHYSADDR(dma_desc)));
	}
	#else //!defined(__DESC_DMA__)
	{
		deptsiz_data_t doeptsize0;
		doepint_data_t doepint = {.d32=0};
		doepint.b.outtknepdis  = 1;
		ifxusb_wreg(&core_if->out_ep_regs[0]->doepint, doepint.d32);

		doeptsize0.d32 = ifxusb_rreg( &core_if->out_ep_regs[0]->doeptsiz);
		//doeptsize0.b0.supcnt = 3;
		doeptsize0.b0.pktcnt = 1;
		doeptsize0.b0.xfersize = 8*3;
		ifxusb_wreg( &core_if->out_ep_regs[0]->doeptsiz, doeptsize0.d32 );

		{
			int i;
			uint32_t *adr;
			adr= (uint32_t *)ifxusb_pcd.setup_pkt;
			for(i=0;i<SetupPacketMax*2;i++,adr++)
				*adr=0;
			if((uint32_t)(ifxusb_pcd.setup_pkt) != (uint32_t)(KSEG1ADDR(ifxusb_pcd.setup_pkt)))
				dma_cache_wback_inv((unsigned long) ifxusb_pcd.setup_pkt, doeptsize0.b0.xfersize);
			ifxusb_wreg(&core_if->out_ep_regs[0]->doepdma, CPHYSADDR(ifxusb_pcd.setup_pkt));
		}
	}
	#endif //defined(__DESC_DMA__)

	doepctl.d32 = ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl);

	#ifdef __VERBOSE__
		IFX_DEBUGPL(DBG_PCDV,"%s() doepctl0=%0x\n", __func__, doepctl.d32);
	#endif

	// EP enable
	doepctl.b.epena = 1;
	doepctl.b.epdis = 0;
	doepctl.b.snak  = 0;
	doepctl.b.cnak  = 1;
	doepctl.b.usbactep = 1;
	ifxusb_wreg(&core_if->out_ep_regs[0]->doepctl, doepctl.d32);

	#ifdef __VERBOSE__
		IFX_DEBUGPL(DBG_PCDV,"doepctl0=%0x\n", ifxusb_rreg(&core_if->out_ep_regs[0]->doepctl));
		IFX_DEBUGPL(DBG_PCDV,"diepctl0=%0x\n", ifxusb_rreg(&core_if->in_ep_regs[0]->diepctl));
	#endif
}


#if !defined(__DED_FIFO__) && !defined(__DED_INTR__)
	/*!
	 \brief This function starts the Zero-Length Packet for the IN status phase
	        of a 2 stage control transfer. (SETUP OUT XFER), and set the next
	 */
	void do_setup_in_status_phase_with_next(int next)
	{
		ifxpcd_ep_t *ep0 = &ifxusb_pcd.ifxep[0];
		if (ifxusb_pcd.ep0state == EP0_STALL)
			return;
		ifxusb_pcd.ep0state = EP0_IN_STATUS_PHASE;

		/* Prepare for more SETUP Packets */
		IFX_DEBUGPL(DBG_PCD, "EP0 IN ZLP\n");
		ep0->buf            =0;
		ep0->xfer_len_totl  =0;
		ep0->xfer_pkt_totl  =1;
		ep0->is_in          =1;
		ep0->zlp_sp         =1;
		ep0->nextep         =next;
		ep0->xfer_len_done = 0;
		ep0->xfer_pkt_done = 0;

		#if defined(__UNALIGNED_BUF_ADJ__)
			ep0->using_aligned_buf=0;
		#endif
		ep0->ep_xfer_stage = Setup;
		ifxpcd_ep_start_tx_transfer(ep0);

		/* Prepare for more SETUP Packets */
		ep0_out_start();
	}
#endif

/*!
 \brief This function starts the Zero-Length Packet for the IN status phase
        of a 2 stage control transfer. (SETUP OUT XFER)
 */
void do_setup_in_status_phase(void)
{
	ifxpcd_ep_t *ep0 = &ifxusb_pcd.ifxep[0];
	if (ifxusb_pcd.ep0state == EP0_STALL)
		return;

	/* Prepare for more SETUP Packets */
	IFX_DEBUGPL(DBG_PCD, "EP0 IN ZLP\n");
	ep0->buf           =0;
	ep0->xfer_len_totl =0;
	ep0->xfer_pkt_totl =1;
	ep0->is_in         =1;
	ep0->zlp_sp        =1;
	ep0->xfer_len_done = 0;
	ep0->xfer_pkt_done = 0;

	#if !defined(__DESC_DMA__) && defined(__UNALIGNED_BUF_ADJ__)
		ep0->using_aligned_buf=0;
	#endif
	ifxusb_pcd.ep0state = EP0_IN_STATUS_PHASE;
	ep0->ep_xfer_stage = Setup;
	ifxpcd_ep_start_tx_transfer(ep0);

	/* Prepare for more SETUP Packets */
	ep0_out_start();
}

/*!
 \brief This function starts the Zero-Length Packet for the OUT status phase
        of a 2 stage control transfer. (SETUP IN XFER)
 */
void do_setup_out_status_phase(void)
{
	ifxpcd_ep_t *ep0 = &ifxusb_pcd.ifxep[0];
	if (ifxusb_pcd.ep0state == EP0_STALL)
		return;
	ifxusb_pcd.ep0state = EP0_OUT_STATUS_PHASE;
	IFX_DEBUGPL(DBG_PCD, "EP0 OUT ZLP\n");

	{
//		int i;
//		for(i=0;i<2*SetupPacketMax;i++) ifxusb_pcd.setup_pkt_buf[i]=0xFFFFFFFF;
	}

	ep0->buf          =(uint8_t *) ifxusb_pcd.setup_pkt;
	ep0->xfer_len_totl =ep0->mps;
	ep0->xfer_pkt_totl =1;
	ep0->is_in         =0;
	ep0->zlp_sp        =1;
	ep0->xfer_len_done =0;
	ep0->xfer_pkt_done =0;
	ep0->ep_xfer_stage = Setup;

	ifxpcd_ep_start_rx_transfer(ep0);
}


/*!
 \brief Clear the EP halt (STALL) and if pending requests start the
        transfer.
 */
static inline void pcd_clear_halt( ifxpcd_ep_t *_ifxep )
{
	if(_ifxep->stall_clear_flag == 0)
		ifxusb_dev_ep_clear_stall( &ifxusb_pcd.core_if, _ifxep->num,_ifxep->type,_ifxep->is_in );

	/* Reactive the EP */
	ifxpcd_ep_activate( _ifxep );
	if (_ifxep->ep_stalled)
	{
		_ifxep->ep_stalled = 0;
		tasklet_schedule (ifxusb_pcd.start_xfer_tasklet);
	}

	/* Start Control Status Phase */
	do_setup_in_status_phase();
}

/*!
 \brief This function is called when the SET_FEATURE TEST_MODE Setup packet
        is sent from the host.  The Device Control register is written with
        the Test Mode bits set to the specified Test Mode.  This is done as
        a tasklet so that the "Status" phase of the control transfer
        completes before transmitting the TEST packets.
 */
static inline void do_test_mode( unsigned long _data )
{
	dctl_data_t		dctl;
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;

	IFX_WARN("%s() has not been tested since being rewritten!\n", __func__);
	dctl.d32 = ifxusb_rreg(&core_if->dev_global_regs->dctl);
	switch (_data)
	{
		case 1: // TEST_J
			IFX_PRINT("%s() Doing Test Mode [TEST_J]\n",__func__);
			dctl.b.tstctl = 1;
			break;
		case 2: // TEST_K
			IFX_PRINT("%s() Doing Test Mode [TEST_K]\n",__func__);
			dctl.b.tstctl = 2;
			break;
		case 3: // TEST_SE0_NAK
			IFX_PRINT("%s() Doing Test Mode [TEST_SE0_NAK]\n",__func__);
			dctl.b.tstctl = 3;
			break;
		case 4: // TEST_PACKET
			IFX_PRINT("%s() Doing Test Mode [TEST_PACKET]\n",__func__);
			dctl.b.tstctl = 4;
			break;
		case 5: // TEST_FORCE_ENABLE
			IFX_PRINT("%s() Doing Test Mode [TEST_FORCE_ENABLE]\n",__func__);
			dctl.b.tstctl = 5;
			break;
		default:
			IFX_PRINT("%s() Invalid Test Mode %lx\n",__func__,_data);
	}
	ifxusb_wreg(&core_if->dev_global_regs->dctl, dctl.d32);
}


/*!
 \brief This funcion stalls EP0.
 */
static void ep0_do_stall(const int err_val )
{
	ifxpcd_ep_t *ep0 = &ifxusb_pcd.ifxep[0];

	#ifdef __DEBUG__
	{
		IFX_WARN("req %02x.%02x protocol STALL; err %d\n",
			ifxusb_pcd.last_req.bRequestType, ifxusb_pcd.last_req.bRequest, err_val);
	}
	#endif
	ep0->is_in = 1;
	ifxusb_dev_ep_set_stall( &ifxusb_pcd.core_if, 0, 1);
	ep0->ep_stalled = 1;
	ifxusb_pcd.ep0state = EP0_IDLE;
	ep0_out_start();
}


/*!
 \brief This functions delegates the setup command to the gadget driver.*/
static void do_gadget_setup( struct usb_ctrlrequest * _ctrl)
{
	int ret = 0;
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	
	#ifndef __DED_FIFO__
		if(_ctrl->bRequest == 0) //USB_CDC_SEND_ENCAPSULATED_COMMAND
		{
			u32 MsgType;
			int i;
			MsgType = *( (u32 *) ifxusb_pcd.ifxep[0].buf );
			switch(MsgType)
			{
				case 0x01000000: //REMOTE_NDIS_PACKET_MSG\n");
				case 0x02000000: //REMOTE_NDIS_INITIALIZE_MSG\n");
//				case 0x03000000: //REMOTE_NDIS_HALT_MSG\n");
				case 0x04000000: //REMOTE_NDIS_QUERY_MSG\n");
				case 0x05000000: //REMOTE_NDIS_SET_MSG\n");
				case 0x06000000: //REMOTE_NDIS_RESET_MSG\n");
//				case 0x07000000: //REMOTE_NDIS_INDICATE_STATUS_MSG\n");
				case 0x08000000: //REMOTE_NDIS_KEEPALIVE_MSG
					#ifdef __DED_INTR__
						ifxusb_pcd.RNDIS_Pending=1;
						for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
							if(ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK)
								ifxusb_pcd.ifxep[i].ep_hold=1;
					#else
						// Next should be INTR, enble INTR, Keep Bulk Hold
						for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
							if(ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR)
							{
								ifxusb_pcd.ifxep[i].ep_hold=0;
								ifxusb_pcd.INTR_Pending=i;
								ifxusb_pcd.ifxep[0].nextep=i;
							}
					#endif
					break;
				case 0x03000000: //REMOTE_NDIS_HALT_MSG\n");
					#ifdef __DED_INTR__
						ifxusb_pcd.RNDIS_Pending=0;
						for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
							if(ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK)
								ifxusb_pcd.ifxep[i].ep_hold=0;
					#else
						for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
							if(ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR)
							{
								ifxusb_pcd.ifxep[i].ep_hold=1;
								ifxusb_pcd.ifxep[i].nextep=0;
							}
					#endif
					break;
				default:
					#ifdef __DED_INTR__
						ifxusb_pcd.RNDIS_Pending=0;
						for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
							if(ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_BULK)
								ifxusb_pcd.ifxep[i].ep_hold=0;
					#else
						ifxusb_pcd.ifxep[0].nextep=0;
						ifxusb_pcd.INTR_Pending=0;
						for (i = 0; i <= ifxusb_pcd.core_if.hwcfg2.b.num_dev_ep; i++)
							if(ifxusb_pcd.ifxep[i].type==IFXUSB_EP_TYPE_INTR)
								ifxusb_pcd.ifxep[i].ep_hold=0;
					#endif
			}
		}
	#endif //__DED_FIFO__
	
	if (ifxusb_pcd.driver && ifxusb_pcd.driver->setup)
	{
		#ifdef __DO_PCD_UNLOCK__
			SPIN_UNLOCK(&ifxusb_pcd.lock);
		#endif
		ret = ifxusb_pcd.driver->setup(&ifxusb_pcd.gadget, _ctrl);
		#ifdef __DO_PCD_UNLOCK__
			SPIN_LOCK(&ifxusb_pcd.lock);
		#endif

		if (ret < 0)
			ep0_do_stall( ret );

		/** @todo This is a g_file_storage gadget driver specific
		 * workaround: a DELAYED_STATUS result from the fsg_setup
		 * routine will result in the gadget queueing a EP0 IN status
		 * phase for a two-stage control transfer.  Exactly the same as
		 * a SET_CONFIGURATION/SET_INTERFACE except that this is a class
		 * specific request.  Need a generic way to know when the gadget
		 * driver will queue the status phase.  Can we assume when we
		 * call the gadget driver setup() function that it will always
		 * queue and require the following flag?  Need to look into
		 * this.
		 */

		if (ret == 256 + 999)
		{
			ifxusb_pcd.request_config = 1;
		}
	}
}

/*!
 \brief This functions gets a pointer to an EP from the wIndex address
        value of the control request.
 */
static ifxpcd_ep_t *get_ep_by_addr (u16 _wIndex)
{
	ifxpcd_ep_t *ifxep;

	if ((_wIndex & USB_ENDPOINT_NUMBER_MASK) == 0)
		return &ifxusb_pcd.ifxep[0];
	list_for_each_entry( ifxep, &ifxusb_pcd.gadget.ep_list, sysep.ep_list)
	{
		u8 bEndpointAddress;

		if (!ifxep->desc)
			continue;
		bEndpointAddress = ifxep->desc->bEndpointAddress;
		if(    (_wIndex          & (USB_DIR_IN | USB_ENDPOINT_NUMBER_MASK))
			== (bEndpointAddress & (USB_DIR_IN | USB_ENDPOINT_NUMBER_MASK)))
			return ifxep;
	}
	return NULL;
}


/*! \brief This function process the GET_STATUS Setup Commands. */
static inline void do_get_status(void)
{
	ifxpcd_ep_t *ifxep;
	ifxpcd_ep_t *ep0    = &ifxusb_pcd.ifxep[0];
	uint16_t    *status = ifxusb_pcd.status_buf;

	#ifdef __DEBUG_EP0__
		IFX_DEBUGPL(DBG_PCD,
				"GET_STATUS %02x.%02x v%04x i%04x l%04x\n",
				ifxusb_pcd.last_req.bRequestType, ifxusb_pcd.last_req.bRequest,
				ifxusb_pcd.last_req.wValue, ifxusb_pcd.last_req.wIndex, ifxusb_pcd.last_req.wLength);
	#endif

	switch (ifxusb_pcd.last_req.bRequestType & USB_RECIP_MASK)
	{
		case USB_RECIP_DEVICE:
			*status  = 0x1; /* Self powered */
			*status |= ifxusb_pcd.remote_wakeup_enable << 1;
			break;

		case USB_RECIP_INTERFACE:
			*status = 0;
			break;

		case USB_RECIP_ENDPOINT:
			ifxep = get_ep_by_addr(ifxusb_pcd.last_req.wIndex);
			if (ifxep == 0 || ifxusb_pcd.last_req.wLength > 2)
			{
				ep0_do_stall(-EOPNOTSUPP);
				return;
			}
			/** @todo check for EP stall */
			*status = ifxep->ep_stalled;
			break;
	}

	ep0->buf          =(uint8_t *)status;
	ep0->xfer_len_totl=2;
	ep0->is_in        =1;
	ep0->zlp_sp       =1;
	ifxpcd_ep_start_tx_transfer(ep0);
}

/*! \brief This function process the SET_FEATURE Setup Commands. */
static inline void do_set_feature(void)
{
	ifxpcd_ep_t	*ifxep = 0;

	IFX_DEBUGPL(DBG_PCD, "SET_FEATURE:%02x.%02x v%04x i%04x l%04x\n",
	                      ifxusb_pcd.last_req.bRequestType, ifxusb_pcd.last_req.bRequest,
	                      ifxusb_pcd.last_req.wValue, ifxusb_pcd.last_req.wIndex, ifxusb_pcd.last_req.wLength);

	switch (ifxusb_pcd.last_req.bRequestType & USB_RECIP_MASK)
	{
		case USB_RECIP_DEVICE:
			switch (ifxusb_pcd.last_req.wValue)
			{
				case USB_DEVICE_TEST_MODE:
					/* Setup the Test Mode tasklet to do the Test
					 * Packet generation after the SETUP Status
					 * phase has completed. */
					ifxusb_pcd.test_mode_tasklet.next = 0;
					ifxusb_pcd.test_mode_tasklet.state = 0;
					atomic_set( &ifxusb_pcd.test_mode_tasklet.count, 0);
					ifxusb_pcd.test_mode_tasklet.func = do_test_mode;
					ifxusb_pcd.test_mode_tasklet.data = (unsigned long)(ifxusb_pcd.last_req.wIndex >> 8);
					tasklet_schedule(&ifxusb_pcd.test_mode_tasklet);
					break;
				case USB_DEVICE_REMOTE_WAKEUP:
					ifxusb_pcd.remote_wakeup_enable = 1;
					break;
				case USB_DEVICE_B_HNP_ENABLE:
					ep0_do_stall( -EOPNOTSUPP);
					return;
				case USB_DEVICE_A_HNP_SUPPORT:
					ep0_do_stall( -EOPNOTSUPP);
					return;
				case USB_DEVICE_A_ALT_HNP_SUPPORT:
					ep0_do_stall( -EOPNOTSUPP);
					return;
				default:
					ep0_do_stall( -EOPNOTSUPP);
					return;
			}
			/* Start Control Status Phase */
			do_setup_in_status_phase();
			break;
		case USB_RECIP_INTERFACE:
			do_gadget_setup(&ifxusb_pcd.last_req);
			break;
		case USB_RECIP_ENDPOINT:
			if (ifxusb_pcd.last_req.wValue == USB_ENDPOINT_HALT)
			{
				ifxep = get_ep_by_addr(ifxusb_pcd.last_req.wIndex);
				if (ifxep == 0)
				{
					ep0_do_stall(-EOPNOTSUPP);
					return;
				}
				ifxep->ep_stalled = 1;
				ifxusb_dev_ep_set_stall( &ifxusb_pcd.core_if, ifxep->num,ifxep->is_in );
			}
			/* Start Control Status Phase */
			do_setup_in_status_phase();
			break;
	}
}

/*! \brief This function process the CLEAR_FEATURE Setup Commands. */
static inline void do_clear_feature(void)
{
	ifxpcd_ep_t *ifxep = 0;

	IFX_DEBUGPL(DBG_PCD,
	     "CLEAR_FEATURE:%02x.%02x v%04x i%04x l%04x\n",
	     ifxusb_pcd.last_req.bRequestType, ifxusb_pcd.last_req.bRequest,
	     ifxusb_pcd.last_req.wValue, ifxusb_pcd.last_req.wIndex, ifxusb_pcd.last_req.wLength);

	switch (ifxusb_pcd.last_req.bRequestType & USB_RECIP_MASK)
	{
		case USB_RECIP_DEVICE:
			switch (ifxusb_pcd.last_req.wValue)
			{
				case USB_DEVICE_REMOTE_WAKEUP:
					ifxusb_pcd.remote_wakeup_enable = 0;
					break;
				case USB_DEVICE_TEST_MODE:
					/** @todo Add CLEAR_FEATURE for TEST modes. */
					// Necessary? The clr_feature command never get here.
					break;
				default:
					ep0_do_stall( -EOPNOTSUPP);
					return;
			}
			/* Start Control Status Phase */
			do_setup_in_status_phase();
			break;
		case USB_RECIP_ENDPOINT:
			ifxep = get_ep_by_addr(ifxusb_pcd.last_req.wIndex);
			if (ifxep == 0)
			{
				ep0_do_stall(-EOPNOTSUPP);
				return;
			}
			pcd_clear_halt(ifxep );
			break;
	}
}

/*! \brief This function process the SET_ADDRESS Setup Commands. */
static inline void do_set_address(void)
{
	ifxusb_core_if_t *core_if = &ifxusb_pcd.core_if;

	if (ifxusb_pcd.last_req.bRequestType == USB_RECIP_DEVICE)
	{
		dcfg_data_t dcfg = {.d32=0};

		#ifdef __DEBUG_EP0__
			IFX_DEBUGPL(DBG_PCDV, "SET_ADDRESS:%d\n", ifxusb_pcd.last_req.wValue);
		#endif

		dcfg.d32=ifxusb_rreg( &core_if->dev_global_regs->dcfg);
		dcfg.b.devaddr = ifxusb_pcd.last_req.wValue;
		ifxusb_wreg(&core_if->dev_global_regs->dcfg, dcfg.d32);
		/* Start Control Status Phase */
		do_setup_in_status_phase();
	}
}



/*!
 \brief This function processes SETUP commands.  In Linux, the USB Command
        processing is done in two places - the first being the PCD and the
        second in the Gadget Driver (for example, the File-Backed Storage
        Gadget Driver).
          PCD handled Commands:
            GET_STATUS        Processed as defined in chapter 9 of the USB 2.0
                              Specification chapter 9
            CLEAR_FEATURE     The Device and Endpoint requests are the
                              ENDPOINT_HALT feature is procesed, all others the
                              interface requests are ignored.
            SET_FEATURE       The Device and Endpoint requests are processed by
                              the PCD.  Interface requests are passed to the
                              Gadget Driver.
            SET_ADDRESS       Program the DCFG reg, with device address received
            SYNC_FRAME        Display debug message.
          Gadget driver handled Commands:
            SET_FEATURE       The Device and Endpoint requests are processed by
                              the PCD.  Interface requests are passed to the
                              Gadget Driver.
            GET_DESCRIPTOR    Return the requested descriptor
            SET_DESCRIPTOR    Optional - not implemented by any of the existing
                              Gadget Drivers.
            SET_CONFIGURATION Disable all EPs and enable EPs for new configuration.
            GET_CONFIGURATION Return the current configuration
            SET_INTERFACE     Disable all EPs and enable EPs for new configuration.
            GET_INTERFACE     Return the current interface.
        When the SETUP Phase Done interrupt occurs, the PCD SETUP commands are
        processed by pcd_setup. Calling the Function Driver's setup function from
        pcd_setup processes the gadget SETUP commands.
 */

void pcd_setup(void)
{
	ifxpcd_ep_t       *ep0        = &ifxusb_pcd.ifxep[0];

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );

	#ifdef __DEBUG_EP0__
		IFX_DEBUGPL(DBG_PCD, "SETUP %02x.%02x v%04x i%04x l%04x\n",
	                    ifxusb_pcd.last_req.bRequestType, ifxusb_pcd.last_req.bRequest,
	                    ifxusb_pcd.last_req.wValue, ifxusb_pcd.last_req.wIndex, ifxusb_pcd.last_req.wLength);
	#endif

	#ifndef __DESC_DMA__
		#if 1 // Attempt to recover EP0 OUT as early as possible. HOWARD
		{
			ifxusb_core_if_t  *core_if    = &ifxusb_pcd.core_if;
			deptsiz_data_t     doeptsize0 = { .d32 = 0};

			doeptsize0.d32 = ifxusb_rreg( &core_if->out_ep_regs[0]->doeptsiz );
			doeptsize0.b0.supcnt=3;
			ifxusb_wreg( &core_if->out_ep_regs[0]->doeptsiz,doeptsize0.d32 );
			// clear interrupt
			{
				doepint_data_t doepint = {.d32=0};
				doepint.b.outtknepdis = 1;
				doepint.b.back2backsetup  = 1;
				ifxusb_wreg(&core_if->out_ep_regs[0]->doepint, doepint.d32);
			}
		}
		#endif
	#endif

	/* Clean up the request queue */
	request_nuke( ep0 );
	ep0->ep_stalled = 0;

	ifxusb_pcd.request_config = 0;
	if (ifxusb_pcd.last_req.bRequestType & USB_DIR_IN)
	{
		ep0->is_in = 1;
		ifxusb_pcd.ep0state = EP0_IN_DATA_PHASE;
	}
	else if(ifxusb_pcd.last_req.wLength == 0)
	{
		ep0->is_in = 1;
		ifxusb_pcd.ep0state = EP0_IN_STATUS_PHASE;
	}
	else
	{
		ep0->is_in = 0;
		ifxusb_pcd.ep0state = EP0_OUT_DATA_PHASE;
	}

	if ((ifxusb_pcd.last_req.bRequestType & USB_TYPE_MASK) != USB_TYPE_STANDARD)
	{
		/* handle non-standard (class/vendor) requests in the gadget driver */
		do_gadget_setup( &ifxusb_pcd.last_req );
		return;
	}

	switch (ifxusb_pcd.last_req.bRequest)
	{
		case USB_REQ_GET_STATUS:
			do_get_status();
			break;
		case USB_REQ_CLEAR_FEATURE:
			do_clear_feature();
			break;
		case USB_REQ_SET_FEATURE:
			do_set_feature();
			break;
		case USB_REQ_SET_ADDRESS:
			do_set_address();
			break;
		case USB_REQ_SET_INTERFACE:
		case USB_REQ_SET_CONFIGURATION:
			ifxusb_pcd.request_config = 1;   /* Configuration changed */
			do_gadget_setup(&ifxusb_pcd.last_req );
			break;
		default:
			/* Call the Gadget Driver's setup functions */
			do_gadget_setup(&ifxusb_pcd.last_req );
			break;
	}
}

