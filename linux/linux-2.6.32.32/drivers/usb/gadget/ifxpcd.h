/*****************************************************************************
 **   FILE NAME       : ifxpcd.h
 **   PROJECT         : Infineon USB Host and Device driver
 **   MODULES         : USB Host and Device driver
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   AUTHOR          : Chen, Howard based on Synopsys original souurce code
 **   DESCRIPTION     : This file contains the structures, constants, and interfaces for
 **                     the Perpherial Contoller Driver (PCD).
 **
 **                     The Peripheral Controller Driver (PCD) for Linux will implement the
 **                     Gadget API, so that the existing Gadget drivers can be used.  For
 **                     the Mass Storage Function driver the File-backed USB Storage Gadget
 **                     (FBS) driver will be used.  The FBS driver supports the
 **                     Control-Bulk (CB), Control-Bulk-Interrupt (CBI), and Bulk-Only
 **                     transports.
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
 \defgroup IFXUSB_PCD PCD Interface
 \ingroup IFXUSB_DRIVER_V3
 \brief  The Peripheral Controller Driver (PCD)
 */


/*!
 \file ifxpcd.h
 \ingroup IFXUSB_DRIVER_V3
 \brief This file contains the implementation of the PCD. In Linux,
  the PCD implements the Gadget API.
*/

#if !defined(__IFXPCD_H__)
#define __IFXPCD_H__

#include <linux/types.h>
#include <linux/list.h>
#include <linux/errno.h>

#include <linux/device.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	#include <linux/usb_ch9.h>
	#include <linux/usb_gadget.h>
#else
	#include <linux/usb/ch9.h>
	#include <linux/usb/gadget.h>
#endif
#include <linux/interrupt.h>


#include "ifxusb_cif.h"
#include "ifxusb_plat.h"

#include <linux/dma-mapping.h>

#ifndef container_of
	#define container_of list_entry
#endif


/*!
 \addtogroup IFXUSB_PCD
 */
/*@{*/

/** Invalid DMA Address */
#define	DMA_ADDR_INVALID	(~(dma_addr_t)0)
/** Maxpacket size for EP0 */
#define	MAX_EP0_SIZE	64
/** Maxpacket size for any EP */
#define MAX_PACKET_SIZE 1024

/** States of EP0. */
/*!
  \enum ep0state_e
  \brief States of EP0.
*/
typedef enum ep0_state
{
	EP0_DISCONNECT,		/* no host */
	EP0_IDLE,
	EP0_IN_DATA_PHASE,
	EP0_OUT_DATA_PHASE,
	EP0_IN_STATUS_PHASE,
	EP0_OUT_STATUS_PHASE,
	EP0_OUT_POST_DATA_PHASE,
	EP0_STALL,
} ep0state_e;

typedef enum _gnaksource_
{
	None=0,
	SwitchEP,
	Mismatch,
	Timeout,
} gnaksource_e;


typedef enum _ep_xfer_stage_
{
	Idle=0,
	Setup,
	InExec,
	Cmpt,
} ep_xfer_stage_e;

#define MAX_EP0_SETUP_PACKET  3
#define MAX_EP0_STATUS_BUFFER 4

/** Fordward declaration.*/
struct ifxpcd_pcd;
struct ifxpcd_request;

/*! typedef ifxpcd_ep_t
  \brief PCD EP structure.
  This structure describes an EP, there is an array of EPs in the PCD
  structure.
 */
typedef struct ifxpcd_ep
{
	/** Pointer to PCD */
	struct ifxpcd_pcd *ifxpcd;

	/** USB EP data */
	struct usb_ep                            sysep;

	/** USB EP Descriptor */
	const struct usb_endpoint_descriptor    *desc;

	struct list_head                         reqs;

	#ifdef __req_num_dbg__
		int                                  req_buf_num;
	#endif

	/** queue of ifxpcd_requests for bulk and intr. */
	struct list_head                         queue;
	struct list_head                         queue_cmpt;

	/** Run-time Info */
	uint8_t  num;                   // EP number used for register address lookup */
	unsigned mps              : 11; // Max Packet bytes */
	unsigned type             : 2; /** EP type:
	                                  0 - Control
	                                  1 - ISOC
	                                  2 - BULK
	                                  3 - INTR
	                                 */
	unsigned is_in            : 1;  // EP direction 0 = OUT */
	#if defined(__DED_FIFO__) || defined(__DED_INTR__)
		unsigned tx_fifo_num  : 4; /** Share FIFO:
		                                non-ISOC: Always 0
		                               Dedicated FIFO:
		                                TX FIFO num
		                                 */
	#endif

	unsigned zlp_sp           : 1; /** Send ZLP(TX) or Allow ShortPacket(Rx) */
	unsigned stall_clear_flag : 1; /** stall clear flag */

	//unsigned data_pid_start   : 1; /** DATA start PID for INTR and BULK EP */
	unsigned ep_act           : 1;  // EP active.
	unsigned ep_stalled       : 1; //  This EP is stopped (active or flow ctrl)

	#if !defined(__DED_FIFO__)
		unsigned ep_gnakdisabling : 1; // np ep manual stopping
		unsigned ep_hold          : 1; // hold because of higher priority
	#endif
	ep_xfer_stage_e ep_xfer_stage ; //

	uint8_t           *buf                ; /** Pointer to the very beginning of the transfer buffer */

	unsigned xfer_len_totl    :19; /** Number of bytes to transfer */
	unsigned xfer_len_done    :19; /** Number of bytes transferred. */
	unsigned xfer_len_running :19; /** Number of bytes transferring. */

	/** Number of packet */
	unsigned xfer_pkt_totl     :10;
	unsigned xfer_pkt_done     :10;
	unsigned xfer_pkt_running  :10;

	#if !defined(__DED_FIFO__) && !defined(__DED_INTR__)
		int nextep;
	#endif

	#if defined(__DESC_DMA__)
		volatile uint32_t *dma_desc;
	#endif

	#if   defined(__UNALIGNED_BUF_ADJ__)
		uint8_t   using_aligned_buf;
		uint8_t  *aligned_buf;
		unsigned  aligned_buf_len : 19;
	#endif

	#if defined(__GADGET_TASKLET_TX__) || defined(__GADGET_TASKLET_RX__)
		 struct tasklet_struct cmpt_tasklet;
		 uint8_t               cmpt_tasklet_in_process;
		 spinlock_t            cmp_lock;
	#endif

}ifxpcd_ep_t;



#define SetupPacketMax 5

/*! typedef ifxpcd_pcd_t
  \brief PCD structure.
  This structure encapsulates the data for the IFX PCD.
 */
typedef struct ifxpcd_pcd
{
	struct device *dev;

	/** the core interface structure. */
	ifxusb_core_if_t core_if;
	spinlock_t       lock;

	/******************************/
	/** Interface to Linux system */
	/******************************/
	struct usb_gadget         gadget;              /** USB gadget */
	struct usb_gadget_driver *driver;              /** USB gadget driver pointer*/

	/******************************/
	/** EPs                       */
	/******************************/
	ifxpcd_ep_t ifxep[ MAX_EPS_CHANNELS ];         /** Array of EPs.*/

	/** SETUP packet for EP0 with enough space for up to 3 setup packets. */
	struct usb_ctrlrequest last_req;

	uint32_t setup_pkt_buf[2*SetupPacketMax],*setup_pkt;

	/** 2-byte dma buffer used to return status from GET_STATUS */
	uint16_t *status_buf;

	/**Enemulated  Device Speed	0: Unknown, 1: LS, 2:FS, 3: HS */
	uint8_t  speed;

	#if defined(__DED_FIFO__)
		uint32_t tx_msk;          /** FIFOs allowed to do Tx **/
	#endif // __DED_FIFO__

	/** EP0 Run-time Info */
	ep0state_e ep0state;                  /** State of EP0 */
	unsigned   request_config       : 1; /** Indicates when SET CONFIGURATION Request is in process */
	unsigned   remote_wakeup_enable : 1; /** The state of the Remote Wakeup Enable. */

	#ifndef __DED_FIFO__           // Shared FIFO only
		#ifdef __DED_INTR__
			uint8_t RNDIS_Pending;			/** In RNDIS CTRL session, NextEP should be zero */
		#else
			uint8_t INTR_Pending;			/** There is a INTR pending at this endpoint */
		#endif
	#endif // __DED_FIFO__

	#ifdef __DESC_DMA__
		volatile uint32_t *setup_desc;          /** 2 descriptors for SETUP packets */
		uint32_t           setup_desc_index;    /** Index of current SETUP handler descriptor */
		/** Descriptor for Data In or Status In phases */
		volatile uint32_t *in_desc;          /** 2 descriptors for SETUP packets */
	#endif
	#ifdef __DED_FIFO__
		/** Thresholding enable flags and length varaiables **/
		uint16_t rx_thr_en;
		uint16_t non_iso_tx_thr_en;

		uint16_t rx_thr_length;
		uint16_t tx_thr_length;

		/** Setup Packet Detected - if set clear NAK when queueing */
		uint32_t setup_packet_detected;
	#endif

	/** Tasklet to defer starting of TEST mode transmissions until
	 *  Status Phase has been completed.
	 */
	struct tasklet_struct  test_mode_tasklet;

	/** Tasklet to delay starting of xfer in DMA mode */
	struct tasklet_struct *start_xfer_tasklet;

	gnaksource_e gnaksource ; // none, SwitchEP, Mismatch, Timeout
	unsigned gnaknext   ;
	unsigned gnakstopping   ;
	unsigned enumdone   ;
	struct timer_list device_probe_timer;
	struct timer_list device_autoprobe_timer;

	unsigned	power_status;
	int		probe_sec;
	int		autoprobe_sec;
} ifxpcd_pcd_t;


/*! typedef ifxpcd_request_t
  \brief IFXPCD request structure.
   This structure is a list of requests.
 */
typedef struct ifxpcd_request
{
	struct list_head    eplist; /**< queue of these requests. */
	struct list_head    trq   ; /**< queue of these requests. */
	struct usb_request  sysreq;   /**< USB Request. */
	ifxpcd_ep_t        *ifxep ;
	uint8_t            *buf   ; /** Pointer to the very beginning of the transfer buffer */
	uint32_t            len   ;
	#ifdef __req_num_dbg__
		uint32_t            reqid;
	#endif
} ifxpcd_request_t;

// In ifxpcd_tran.c
/*!
 \fn    void ifxpcd_start_new_transfer(ifxpcd_request_t   *_ifxreq,ifxpcd_ep_t *_ifxep)
 \brief Start a Tx transfer of the EP.
 \param _ifxreq       Pointer of req structure
 \param _ifxep        Pointer of ep structure
 */
extern void ifxpcd_start_new_transfer(ifxpcd_request_t   *_ifxreq,ifxpcd_ep_t *_ifxep);

/*!
 \fn    void ifxpcd_ep_start_tx_transfer(ifxpcd_ep_t *_ifxep)
 \brief Start a Tx transfer of the EP.
 \param _ifxep        Pointer of ep structure
 */
extern void ifxpcd_ep_start_tx_transfer(ifxpcd_ep_t *_ifxep);

/*!
 \fn    void ifxpcd_ep_start_rx_transfer(ifxpcd_ep_t *_ifxep)
 \brief Start a Rx transfer of the EP.
 \param _ifxep        Pointer of ep structure
 */
extern void ifxpcd_ep_start_rx_transfer(ifxpcd_ep_t *_ifxep);

// In ifxpcd_cmpt.c
/*!
 \fn    void complete_ep_tx( ifxpcd_ep_t *_ifxep )
 \brief Handle EPn CMLT Interrupt of Tx Transfer
 */
extern void complete_ep_tx( ifxpcd_ep_t *_ifxep );
/*!
 \fn    void complete_ep_rx( ifxpcd_ep_t *_ifxep )
 \brief Handle EPn CMLT Interrupt of Rx Transfer
 */
extern void complete_ep_rx( ifxpcd_ep_t *_ifxep );

/*!
 \fn    void handle_ep0_tx(void)
 \brief Handle EP0 CMLT Interrupt of Tx Transfer
 */
extern void handle_ep0_tx(void);

/*!
 \fn    void handle_ep0_rx(int _case)
 \brief Handle EP0 CMLT Interrupt of Rx Transfer
 */
extern void handle_ep0_rx(int _case);


/*!
 \fn    void handle_ep0_setup(void)
 \brief Handle EP0 SETUP Interrupt
 */
extern void handle_ep0_setup(void);

// In ifxpcd_intr.c

/*! \brief This function prints the ep0 state for debug purposes. */
extern void print_ep0_state(void);

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
extern void pcd_setup(void);


#ifndef __DED_FIFO__
	/*!
	 \brief This function starts the Zero-Length Packet for the IN status phase
	        of a 2 stage control transfer. (SETUP OUT XFER), and set the next
	 */
	extern void do_setup_in_status_phase_with_next(int next);
#endif

/*!
 \brief This function starts the Zero-Length Packet for the IN status phase
        of a 2 stage control transfer. (SETUP OUT XFER)
 */
extern void do_setup_in_status_phase(void);

/*!
 \brief This function starts the Zero-Length Packet for the OUT status phase
        of a 2 stage control transfer. (SETUP IN XFER)
 */
extern void do_setup_out_status_phase(void);


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
extern void ep0_out_start(void);

/*!
 \fn    int32_t ifxpcd_handle_intr(void)
 \brief PCD interrupt handler.
        The PCD handles the device interrupts.  Many conditions can cause a
        device interrupt. When an interrupt occurs, the device interrupt
        service routine determines the cause of the interrupt and
        dispatches handling to the appropriate function. These interrupt
        handling functions are described below.

        All interrupt registers are processed from LSB to MSB.
 */
extern int32_t ifxpcd_handle_intr(void);

// In ifxpcd.c
/*!
 \fn     void request_done(ifxpcd_ep_t *_ifxep, ifxpcd_request_t *_ifxreq, int _status)
  \brief This function completes a request.  It call's the request call back.
 */
extern void request_done(ifxpcd_ep_t *_ifxep, ifxpcd_request_t *_ifxreq, int _status);

/*!
  \fn     request_nuke( ifxpcd_ep_t *_ifxep )
  \brief  This function terminates all the requsts in the EP request queue.
 */
extern void request_nuke( ifxpcd_ep_t *_ifxep );

/*!
  \fn     int start_next_request( ifxpcd_ep_t *_ifxep )
   \brief This function check and start a request from a EP
   return 1 if there is one request started
 */
extern int start_next_request( ifxpcd_ep_t *_ifxep );

/*!
   \fn    void restart_requests(void)
   \brief This function choose request(s) accross EP
 */
extern void restart_requests(void);

/////////////////////////////////////////////////////////////////////////////////
// Gadget operations
//   These EP indendant functions are implemented in the PCD for Gadget Driver to call
//   through Gadget-API.
/////////////////////////////////////////////////////////////////////////////////

/*!
   \brief this function set core to send RemoteWakeUp signal to host
          may be called by gadget driver or user level app like PM
 */
extern void ifxpcd_remote_wakeup(int set);

/////////////////////////////////////////////////////////////////////////////////
// USB bus signal operations
//   Suspend
//   Resume
//   Reset
/////////////////////////////////////////////////////////////////////////////////
/*!
   \brief This interrupt occurs when a USB Suspend is detected.
 */
extern void ifxpcd_suspend(void);

/*!
   \brief This interrupt occurs when a USB Resume is detected.
 */
extern void ifxpcd_resume(void);

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
extern void ifxpcd_usbreset(void);


/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////

/*!
   \fn    void ifxpcd_reinit(void)
   \brief This function initialized the pcd Dp structures to there default state.
 */
extern void ifxpcd_reinit(void);
/////////////////////////////////////////////////////////////////////////////////
// PCD initialization and removal
/////////////////////////////////////////////////////////////////////////////////

/*!
  \fn    irqreturn_t ifxpcd_irq(int _irq
                             ,void *_dev
                              )
  \brief This function is the top level PCD interrupt handler.
 */
extern irqreturn_t ifxpcd_irq(int _irq
                             ,void *_dev
                              );
/*!
  \fn void ifxpcd_stop(void)
 \brief This function is called when the Device is disconnected.  It stops
        any active requests and informs the Gadget driver of the
        disconnect.
 */
// This entry is called by RESET ..HOWARD
extern void ifxpcd_stop(void);

/*!
  \fn int ifxpcd_init(void)
   \brief This function initialized the PCD portion of the driver.
 */
extern int ifxpcd_init(void);

/*!
  \fn void ifxpcd_remove(void)
   \brief Cleanup the PCD.
 */
void ifxpcd_remove(void);

/////////////////////////////////////////////////////////////////////////////////
// Hardware level EP activation and deactivation
/////////////////////////////////////////////////////////////////////////////////
/*!
  \fn void ifxpcd_ep0_activate(void)
   \brief This function enables EP0 OUT to receive SETUP packets and configures EP0
          IN for transmitting packets.  It is normally called when the
          "Enumeration Done" interrupt occurs.
          Note: Hardware level
 */
extern void ifxpcd_ep0_activate(void);

/*!
  \fn void ifxpcd_ep_activate(ifxpcd_ep_t *_ifxep)
   \brief This function activates an EP.  The Device EP control register for
          the EP is configured as defined in the ep structure.
          Note: for EP0, call ifxpcd_ep0_activate() instead
          Note: Hardware level
 */
extern void ifxpcd_ep_activate(ifxpcd_ep_t *_ifxep);

/*!
  \fn void ifxpcd_ep_deactivate(ifxpcd_ep_t *_ifxep)
   \brief This function deactivates an EP.  This is done by clearing the USB Active
          EP bit in the Device EP control register.
          Note: This function is not used for EP0. EP0 cannot be deactivated.
          Note: Hardware level
 */
extern void ifxpcd_ep_deactivate(ifxpcd_ep_t *_ifxep);


#if defined(__DESC_DMA__)
	/*!
	  \fn  static inline uint32_t* ep_alloc_desc(int num)
	  \brief  This function allocates a DMA Descriptor chain for the Endpoint
	  buffer to be used for a transfer to/from the specified endpoint.
	  Dedicated FIFO mode only
	 */
	static inline uint32_t* ep_alloc_desc(int num)
	{
		return ((uint32_t*)ifxusb_alloc_buf_d(8*num, 1));
	}

	/*!
	  \fn  static inline void ep_free_desc(uint32_t* desc_addr)
	  \brief  This function frees a DMA Descriptor chain that was allocated by ep_alloc_desc.
	 	Dedicated FIFO mode only
	 */
	//static void ep_free_desc(ifxusb_dma_desc_t* desc_addr)
	static inline void ep_free_desc(uint32_t* desc_addr)
	{
		ifxusb_free_buf_d((void *)desc_addr);
	}

	static inline void write_desc(uint32_t* desc_addr,uint32_t data)
	{
		uint32_t t;
		uint8_t *p;
		p=(uint8_t *)&t;
		*(p+0)=(uint8_t)((data    ) & 0x000000FF);
		*(p+1)=(uint8_t)((data>>8 ) & 0x000000FF);
		*(p+2)=(uint8_t)((data>>16) & 0x000000FF);
		*(p+3)=(uint8_t)((data>>24) & 0x000000FF);
		writel(t, (volatile uint32_t *)desc_addr);
		return;
	}

	static inline uint32_t read_desc(uint32_t* desc_addr)
	{
		uint32_t t,data;
		uint8_t *p;

		data=readl((volatile uint32_t *)desc_addr);

		p=(uint8_t *)&t;
		*(p+0)=(uint8_t)((data    ) & 0x000000FF);
		*(p+1)=(uint8_t)((data>>8 ) & 0x000000FF);
		*(p+2)=(uint8_t)((data>>16) & 0x000000FF);
		*(p+3)=(uint8_t)((data>>24) & 0x000000FF);
		return (t);
	}
#endif //__DESC_DMA__


/*@}*//*IFXUSB_PCD*/


#endif // __IFXPCD_H__

