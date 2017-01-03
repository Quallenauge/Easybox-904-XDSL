#ifndef __IFX_PPA_API_DIRECTPATH_H__20081119_1144__
#define __IFX_PPA_API_DIRECTPATH_H__20081119_1144__



/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_directpath.h
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 19 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Directpath Functions Header
**                File
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 19 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/
/*! \file ifx_ppa_api_directpath.h
    \brief This file contains: PPA direct path api. 
*/



#include <net/ifx_ppa_api_common.h>



/*
 * ####################################
 *              Data Type
 * ####################################
 */
/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  Directpath API Flags
 */
/*!
    \brief PPA_F_DIRECTPATH_REGISTER
*/ 
#define PPA_F_DIRECTPATH_REGISTER               0x00100000  /*!< Directpath register flag*/

/*!
    \brief PPA_F_DIRECTPATH_CORE1
*/ 
#define PPA_F_DIRECTPATH_CORE1                  0x00200000 /*!< Directpath flag: run in CPU Core 1*/
/*!
    \brief PPA_F_DIRECTPATH_ETH_IF
*/ 
#define PPA_F_DIRECTPATH_ETH_IF                 0x00400000  /*!< Directpath flag: Ethernet type*/

/*
 *  Directpath Internal Flags
 */
 /*!
    \brief PPE_DIRECTPATH_DATA_ENTRY_VALID
*/ 
#define PPE_DIRECTPATH_DATA_ENTRY_VALID         (1 << 31)  /*!< define flag */
 /*!
    \brief PPE_DIRECTPATH_DATA_RX_ENABLE
*/ 
#define PPE_DIRECTPATH_DATA_RX_ENABLE           (1 << 0)   /*!< define flag */
 /*!
    \brief PPE_DIRECTPATH_ETH
*/ 
#define PPE_DIRECTPATH_ETH                      (1 << 4) /*!< define flag */
 /*!
    \brief PPE_DIRECTPATH_CORE0
*/ 
#define PPE_DIRECTPATH_CORE0                    (1 << 8) /*!< define flag */
 /*!
    \brief PPE_DIRECTPATH_CORE1
*/ 
#define PPE_DIRECTPATH_CORE1                    (1 << 9) /*!< define flag */
 /*!
    \brief PPE_DIRECTPATH_ITF_TYPE_MASK
*/ 
#define PPE_DIRECTPATH_ITF_TYPE_MASK            (PPE_DIRECTPATH_ETH)  /*!< define flag */
 /*!
    \brief PPE_DIRECTPATH_CORE_MASK
*/ 
#define PPE_DIRECTPATH_CORE_MASK                (PPE_DIRECTPATH_CORE0 | PPE_DIRECTPATH_CORE1)  /*!< define flag */  
 /*!
    \brief PPE_DIRECTPATH_MASK
*/ 
#define PPE_DIRECTPATH_MASK                     (PPE_DIRECTPATH_ITF_TYPE_MASK | PPE_DIRECTPATH_CORE_MASK) /*!< define flag */


/** \addtogroup  PPA_API_DIRECTPATH */
/*@{*/
/*! \fn PPA_FP_STOP_TX_FN
    \brief This is the definition for the PPA DirectPath Stop Transmit function callback used for flow control when transmitting
            packets through the PPA DirectPath. The PPA DirectPath calls this hook to indicate to the device driver that it
            cannot receive any more packets for transmission.
    \param[in] dev  The protocol stack network interface structure on which to signal Stop Tx for flow control
    \return  The return value can be any one of the following:  \n
               IFX_SUCCESS on sucess \n
               IFX_FAILURE on error if the driver cannot handle flow control   
    \note This function must be provided by the CPU-bound interface driver and will be called by the PPA Directpath to pass
    on frames directly to the driver for transmitting out of its interface.          
*/ 
typedef int32_t (*PPA_FP_STOP_TX_FN)(PPA_NETIF *dev);

/*! \fn PPA_FP_RESTART_TX_FN
    \brief   This is the definition for the PPA DirectPath Restart Transmit function callback used for flow control when
                transmitting packets through the PPA DirectPath. The PPA DirectPath calls this hook to indicate to the device
                driver that it is ready to receive packets for transmission after having asserted stop flow control using
                PPA_FP_STOP_TX_FN.
    \param[in] dev  The protocol stack network interface structure on which to signal restart Tx for flow control.
    \return  The return value can be any one of the following:  \n
               IFX_SUCCESS on sucess \n
               IFX_FAILURE on error if the driver cannot handle flow control   
    \note It is recommended for a device driver to use the PPA DirectPath flow control functions for efficient packet
            processing. This callback must always be used in conjunction with the PPA_FP_STOP_TX_FN callback, i.e.,
            either both callbacks or none must be configure
*/
typedef int32_t (*PPA_FP_RESTART_TX_FN)(PPA_NETIF *dev);
/*! \fn PPA_FP_RX_FN
    \brief This is the definition for the PPA DirectPath receive function callback that passes packet from PPA DirectPath to
            the CPU bound driver.  This callback is registered for the device with the PPA module by the device driver. This is required to allow the PPA acceleration layer to directly pass packets to the relevant device driver bypassing the protocol stack. It is also the only way for the PPA to deliver packets to drivers running on Core 1.
            The packet buffer passed to the rx_fn callback will have the data pointer point to the start of Link layer header (i.e. Ethernet header). For eg., on Linux, skb->data will point to the Ethernet header.
    \param[in] rxif  PPA Receive interface pointer on which the packet is received.
    \param[in] txif  PPA Tx interface pointer to which the packet is transmitted.
    \param[in] skb   Pointer to SKB buffer received.
    \param[in] len  Length of packet frame.
    \return  The return value can be any one of the following:  \n
               IFX_SUCCESS on sucess. \n
               IFX_FAILURE on error if the driver process the packet for some reason.
    \note This function must be provided by the CPU-bound interface driver and will be called by the PPA Directpath to pass on frames directly to the driver for transmitting out of its interface.          
*/
typedef int32_t (*PPA_FP_RX_FN)(PPA_NETIF *rxif, PPA_NETIF *txif, PPA_BUF *skb, int32_t len);

/*@}*/ /*PPA_API_DIRECTPATH */


/** \addtogroup  PPA_API_DIRECTPATH */
/*@{*/

/*!
    \brief This is the data structure for the PPA DirectPath device registration which provides the necessary callback
    registration for receiving packets from PPA DirectPath to the driver for transmit, and for flow control to be asserted
    by the PPA DirectPath for packets from the driver to the PPA DirectPath. 
*/
typedef struct {
    PPA_FP_STOP_TX_FN       stop_tx_fn; /*!< Pointer to the Driver Stop Tx function callback. Providing a NULL pointer disables the callback functionality. */
    PPA_FP_RESTART_TX_FN    start_tx_fn; /*!< Pointer to the Driver Restart Tx function callback. Providing a NULL pointer disables the callback functionality.*/
    PPA_FP_RX_FN            rx_fn; /*!< Device Receive Function callback for packets. Setting a value of NULL pointer disables Receive callback for the device */
} PPA_DIRECTPATH_CB;

/*
 *  Internal Structure of directpath
 */
/*!
    \brief This is the data structure for the PPA DirectPath device status
*/
struct ppe_directpath_data {
    PPA_DIRECTPATH_CB               callback; /*!< Callback Pointer to PPA_DIRECTPATH_CB */ 
    PPA_NETIF                       *netif;     /*!< pointer to  PPA_NETIF*/
    uint32_t                        ifid;   /*!< directpath interface id */
    uint8_t                         mac[(PPA_ETH_ALEN + sizeof(uint32_t) - 1) / sizeof(uint32_t) * sizeof(uint32_t)];  /*!< interface mac address */
    PPA_BUF                         *skb_list;   /*!< the directpath interface's skb list */
    uint32_t                        skb_list_len; /*!< the skb list length */
    uint32_t                        skb_list_size;  /*!< the skb list size */
    uint32_t                        rx_fn_rxif_pkt;  /*!< received packet counter */
    uint32_t                        rx_fn_txif_pkt; /*!< transmitted packet coutner */
    uint32_t                        tx_pkt;            /*!< transmitted packet counter */
    uint32_t                        tx_pkt_dropped; /*!< dropped packet counter */
    uint32_t                        tx_pkt_queued; /*!< queued packet counter */
    uint32_t                        flags;  /*!<  bit 0 - directpath send valid, 31 - entry valid  */
};


/*@}*/ /* PPA_API_DIRECTPATH */


/*
 * ####################################
 *             Declaration
 * ####################################
 */
#ifdef NO_DOXY
extern struct ppe_directpath_data g_ppe_directpath_data[];
#endif

/** \addtogroup  PPA_API_DIRECTPATH */
/*@{*/
#ifdef CONFIG_IFX_PPA_API_DIRECTPATH
/*!  
    \brief   This function allows a device driver to register or deregister a network device to the PPA.
    \param[out] if_id  PPA specific Interface Identifier. It is currently a number between 0 to 7. This Id is returned by the PPA module.
    \param[in]  dev  Pointer to the network device structure in the protocol stack. For e.g. pointer to a struct netdevice.
    \param[in]  pDirectpathCb  Pointer to the DirectPath callback structure which provides the driver callbacks for rx_fn, stop_tx_fn and restart_tx_fn. This parameter is only available on new PPA API. 
    \param[in]  flags  Flag to indicate if device is being registered or deregistered. Valid values are:\n 
                       - PPA_F_DIRECTPATH_DEREGISTER, if de-registering, zero otherwise \n 
                       - PPA_F_DIRECTPATH_CORE1 if the driver for the network interface is running on Core 1 (i.e. 2nd CPU) \n 
                       - PPA_F_DIRECTPATH_ETH_IF if the network interface is an Ethernet-like interface
    \return  The return value can be any one of the following:  \n
		-	IFX_SUCCESS on sucess \n
		-	IFX_FAILURE    
    \note 
*/
int32_t ifx_ppa_directpath_register_dev(uint32_t *if_id, PPA_NETIF *dev, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);

/*!  \brief   This function allows the device driver to transmit a packet using the PPA DirectPath interface. The packet buffer passed to the function must have its "packet data" pointer set to the IP header with the Ethernet header/Link layer header etc. preceding the IP header (For e.g., on Linux skb->data points to IP header, and doing the appropriate skb_push() will allow skb->data to move backwards and point to Ethernet header). In other words, PPA Directpath must be able to access the full frame even though the packet buffer "points" to the IP header as required by the Linux netif_rx() function.
    \param[in] rx_if_id  Receive interface Id in the protocol stack.
    \param[in] buf  Pointer to the packet buffer structure of the stack for the packet which is to be transmitted.
    \param[in] len  Size of packet in bytes. 
    \param[in] flags  Currently Reserved 
    \return  The return value can be any one of the following:  \n
		-	IFX_SUCCESS on sucess \n
		-	IFX_FAILURE    
    \note The DirectPath Tx API can have internal "shortcut" path to the destination or fallback to passing the packet to the protocol stack. The aim is to insulate the device driver calling the API from such details. For Linux, the driver must call this function through the hook pointer where it passes packets to the network stack by calling the "netif_rx()" or "netif_receive_skb()" functions. \n
            Note: The CPU-bound device driver is strongly recommended to call this API from tasklet mode (or equivalent non-interrupt context on non-Linux OS) and not from IRQ context for better system dynamics.
*/
  int32_t ifx_ppa_directpath_send(uint32_t rx_if_id, PPA_BUF *buf, int32_t len, uint32_t flags);

/*!  \brief   This function allows the device driver to indicate to the PPA that it cannot receive any further packets from the latter. The device driver can call this function for flow control.
    \param[in] if_id  Interface Id for which to apply flow control.
    \param[in] flags  Currently Reserved 
    \return  The return value can be any one of the following:  \n
		-	IFX_SUCCESS on sucess \n
		-	IFX_FAILURE    
    \note   \n
*/
  int32_t ifx_ppa_directpath_rx_stop(uint32_t if_id, uint32_t flags);

/*!  \brief     This function allows the device driver to indicate to the PPA that it can again receive packets from the latter. The device driver can call this function for flow control after it has called the "Rx Stop" function to halt supply of packets from the PPA.
    \param[in] if_id  Interface Id for which the driver requests the flow control action to restart transmission.
    \param[in] flags  Currently Reserved 
    \return  The return value can be any one of the following:  \n
		-	IFX_SUCCESS on sucess \n
		-	IFX_FAILURE    
    \note   It is recommended for a device driver to use the PPA DirectPath flow control functions for efficient packet processing. This function must be used in conjunction with the PPA_FP_STOP_TX_FN. \n
*/
  int32_t ifx_ppa_directpath_rx_restart(uint32_t if_id, uint32_t flags);

/*!  \brief     This function maps the PPA Interface Id to Protocol stack interface structure.
    \param[in] if_id  PPA Interface Identifier.
    \return  The return value can be any one of the following:  \n
		-	Pointer to the interface structure in the protocol stack \n
		-	NULL
    \note  This API may not implemented on older PPA version. \n
*/ 
  PPA_NETIF *ifx_ppa_get_netif_for_ppa_ifid(uint32_t if_id);

/*!  \brief    This function maps the Protocol stack interface structure to the PPA Interface Id.
    \param[in] dev  Pointer to the protocol stack network interface structure for the device.
    \return  The return value can be any one of the following: \n 
		-	PPA Interface Identifier, if_id \n 
		-	IFX_FAILUREPPA_NETIF. \n
    \note  This API may not implemented on older PPA version. \n
*/  
  int32_t ifx_ppa_get_ifid_for_netif(PPA_NETIF *dev);
  
/*!  \brief     This function is used to initialize the interation of ppe_directpath_data information
    \param[in,out] ppos  offset of current ppe_directpath_data, after call, its value will be changed to next offset
    \param[out]    info  Buffer to store the ppe_directpath_data.
    \return  The return value can be any one of the following: \n 
		-	IFX_SUCCESS, if succeed \n 
		-	IFX_FAILURE, if fail to get \n
*/    
  int32_t ppa_directpath_data_start_iteration(uint32_t *ppos, struct ppe_directpath_data **info);
  
/*!  \brief  This function is used to ge the next ppe_directpath_data information during its iteration
    \param[in,out] ppos  offset of ppe_directpath_data, after call, its value will be changed to next offset
    \param[out]    info  Buffer to store the ppe_directpath_data.
    \return  The return value can be any one of the following: \n 
            -	IFX_SUCCESS, if succeed \n 
            -	IFX_FAILURE, if fail to get \n
*/   
  int32_t ppa_directpath_data_iterate_next(uint32_t *ppos, struct ppe_directpath_data **info);
  
/*!  \brief     This function is used to stop ppe_directpath_data iteration
    \return NULL
*/   
  void ppa_directpath_data_stop_iteration(void);

  /*!  \brief     This function is used to get directpath's ifid range
    \param[out] p_start_ifid  Buffer to store the directpath's start ifid
    \param[out] p_end_ifid  Buffer to store the directpath's end ifid
    \return NULL\n
*/
  void ppa_directpath_get_ifid_range(uint32_t *p_start_ifid, uint32_t *p_end_ifid);
#endif

/* @}*/  /* PPA_API_DIRECTPATH */ 

#endif  //  __IFX_PPA_API_DIRECTPATH_H__20081119_1144__
