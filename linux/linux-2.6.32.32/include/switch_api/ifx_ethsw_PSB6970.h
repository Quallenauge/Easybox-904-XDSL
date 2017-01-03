/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _IFX_ETHSW_PSB6970_H_
#define _IFX_ETHSW_PSB6970_H_

#include "ifx_types.h"

/* =================================== */
/* Global typedef forward declarations */
/* =================================== */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* IOCTL MAGIC */
#define IFX_PSB6970_MAGIC ('P')

/* Group definitions for Doxygen */
/** \defgroup PSB6970_IOCTL XWAY-ARX100 / XWAY TANTOS-xG Family Specific Ethernet Switch API
    This chapter describes the XWAY ARX100 Family, XWAY TANTOS-0G,
    XWAY TANTOS-2G and XWAY TANTOS-3G  specific interfaces for accessing and
    configuring the services of the Ethernet switch module. */
/*@{*/
/** \defgroup PSB6970_IOCTL_DEBUG Debug Features
    TANTOS specific features for system integration and debug sessions.
*/
/** \defgroup PSB6970_IOCTL_OAM Operation, Administration, and Management Functions
    This chapter summarizes the functions that are provided to monitor the
    data traffic passing through the device.
*/
/** \defgroup PSB6970_IOCTL_POWER Power Management
    Configure the TANTOS hardware specific power management.
*/
/** \defgroup PSB6970_IOCTL_QOS Quality of Service Functions
    Switch and port configuration for QoS.
*/
/*@}*/

/** \addtogroup PSB6970_IOCTL_OAM */
/*@{*/

/** Reset selection.
    Used by \ref IFX_PSB6970_reset_t. */
typedef enum
{
   /** On-chip Ethernet PHY reset */
   IFX_PSB6970_RESET_EPHY
}IFX_PSB6970_resetMode_t;

/** Reset selection.
    Used by \ref IFX_PSB6970_RESET. */
typedef struct
{
   /** Reset selection. */
   IFX_PSB6970_resetMode_t              eReset;
}IFX_PSB6970_reset_t;

/*@}*/ /* PSB6970_IOCTL_OAM */

/** \addtogroup PSB6970_IOCTL_QOS */
/*@{*/

/** WFQ Algorithm Selector per port.
    Used by \ref IFX_PSB6970_QoS_portShaperCfg_t. */
typedef enum
{
   /** Weight. WFQ instances are configured to assign a weight (ratio) to a queue instance.
       All WFQ egress queues are configured using a ratio. */
   IFX_PSB6970_QoS_WFQ_WEIGHT           = 0,
   /** Rate. WFQ instances are configured to limit the egress traffic to a configured rate. */
   IFX_PSB6970_QoS_WFQ_RATE             = 1
}IFX_PSB6970_QoS_WFQ_t;

/** Port-specific QoS configuration.
    Used by \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET
    and \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_GET. */
typedef struct
{
   /** Port index. */
   IFX_uint32_t                         nPort;
   /** WFQ Algorithm. Selection between ratio behavior
       and rate limitation. */
   IFX_PSB6970_QoS_WFQ_t                eWFQ_Type;
}IFX_PSB6970_QoS_portShaperCfg_t;

/** Port-specific configuration for the priority queue rate shaper.
    Used by \ref IFX_PSB6970_QOS_PORT_SHAPER_STRICT_SET
    and \ref IFX_PSB6970_QOS_PORT_SHAPER_STRICT_GET. */
typedef struct
{
   /** Port index. */
   IFX_uint32_t                         nPort;
   /** Priority queue index (counting from zero). */
   IFX_uint8_t                          nTrafficClass;
   /** Maximum average rate [in Mbit/s]. */
   IFX_uint32_t                         nRate;
}IFX_PSB6970_QoS_portShaperStrictCfg_t;

/** Port-specific configuration for the weight fair queuing rate shaper.
    The egress queues could be scheduled by assigned weights,
    or the traffic limited by configured rate shapers.
    The \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET command configures
    the queues to work in weight or rate shaper mode.
    Used by \ref IFX_PSB6970_QOS_PORT_SHAPER_WFQ_SET
    and \ref IFX_PSB6970_QOS_PORT_SHAPER_WFQ_GET. */
typedef struct
{
   /** Port index. */
   IFX_uint32_t                         nPort;
   /** Priority queue index (counting from zero). */
   IFX_uint8_t                          nTrafficClass;
   /** Rate / Weight.

       - Weight: Maximum average rate [in ratio], in case eWFQ_Type=IFX_PSB6970_QoS_WFQ_WEIGHT.
       - Rate: Maximum average rate [in Mbit/s], in case eWFQ_Type=IFX_PSB6970_QoS_WFQ_RATE.
   */
   IFX_uint32_t                         nRate;
}IFX_PSB6970_QoS_portShaperWFQ_Cfg_t;

/** Port-specific configuration for the ingress rate policing.
    Used by \ref IFX_PSB6970_QOS_PORT_POLICER_SET
    and \ref IFX_PSB6970_QOS_PORT_POLICER_GET. */
typedef struct
{
   /** Port index. */
   IFX_uint32_t                         nPort;
   /** Maximum average rate [in Mbit/s]. */
   IFX_uint32_t                         nRate;
}IFX_PSB6970_QoS_portPolicerCfg_t;

/** Qos storm control for egress packets. Different packet types can be
    discarded if the egress packet rate reaches a defined threshold.
    Used by \ref IFX_PSB6970_QOS_STORM_SET and \ref IFX_PSB6970_QOS_STORM_GET. */
typedef struct
{
   /** Storm control for received boardcast packets. */
   IFX_boolean_t                        bBroadcast;
   /** Storm control for received multicast packets. */
   IFX_boolean_t                        bMulticast;
   /** Storm control for received unicasst packets. */
   IFX_boolean_t                        bUnicast;
   /** 10 Mbit/s link threshold [in Mbit/s] for the storm control to discard. */
   IFX_uint32_t                         nThreshold10M;
   /** 100 Mbit/s link threshold [in Mbit/s] for the storm control
       to discard. */
   IFX_uint32_t                         nThreshold100M;
}IFX_PSB6970_QoS_stormCfg_t;

/** Multi-Field priority classification fields.
    Used by \ref IFX_PSB6970_QoS_MfcMatchField_t. */
typedef enum
{
   /** UDP/TCP Source Port Filter. */
   IFX_PSB6970_QOS_MF_SRCPORT           = 1,
   /** UDP/TCP Destination Port Filter. */
   IFX_PSB6970_QOS_MF_DSTPORT           = 2,
   /** IP Protocol Filter. */
   IFX_PSB6970_QOS_MF_PROTOCOL          = 4,
   /** Ethertype Filter. */
   IFX_PSB6970_QOS_MF_ETHERTYPE         = 8
}IFX_PSB6970_QoS_MfPrioClassfields_t;

/** Qos multi-field priority classification configuration for Ethernet ports.
    Used by \ref IFX_PSB6970_QOS_MFC_PORT_CFG_SET
    and \ref IFX_PSB6970_QOS_MFC_PORT_CFG_GET. */
typedef struct
{
   /** Port index. */
   IFX_uint32_t                         nPort;
   /** Use the UDP/TCP Port MFC priority classification rules to assign the
       traffic class for ingress packets that match against a rule. */
   IFX_boolean_t                        bPriorityPort;
   /** Use the EtherType MFC priority classification rules to assign the
       traffic class for ingress packets that match against a rule. */
   IFX_boolean_t                        bPriorityEtherType;
}IFX_PSB6970_QoS_MfcPortCfg_t;

/** QoS multi-field priority classification match fields structure.
    Used by \ref IFX_PSB6970_QoS_MfcCfg_t and \ref IFX_PSB6970_QOS_MFC_DEL. */
typedef struct
{
   /** Source port base. */
   IFX_uint16_t                         nPortSrc;
   /** Destination port base */
   IFX_uint16_t                         nPortDst;
   /** Check from nPortSrc till smaller nPortSrc + nPortSrcRange. */
   IFX_uint16_t                         nPortSrcRange;
   /** Check from nPortDst till smaller nPortDst + nPortDstRange. */
   IFX_uint16_t                         nPortDstRange;
   /** Protocol type. */
   IFX_uint8_t                          nProtocol;
   /** Ether type. */
   IFX_uint16_t                         nEtherType;
   /** Select the filtering field.*/
   IFX_PSB6970_QoS_MfPrioClassfields_t     eFieldSelection;
}IFX_PSB6970_QoS_MfcMatchField_t;

/** QoS multi-field priority classification info structure.
    Used by \ref IFX_PSB6970_QoS_MfcCfg_t. */
typedef struct
{
   /** Egress priority queue priority queues, Q3 > Q2 > Q1 > Q0.
       The queue index starts counting from zero. */
   IFX_uint8_t                          nTrafficClass;
   /** Output port selection. */
   IFX_ETHSW_portForward_t              ePortForward;
}IFX_PSB6970_QoS_MfcInfo_t;

/** QoS multi-field priority classification rule config structure.
    Used by \ref IFX_PSB6970_QoS_MfcEntryRead_t and \ref IFX_PSB6970_QOS_MFC_ADD. */
typedef struct
{
   /** Match fields.  */
   IFX_PSB6970_QoS_MfcMatchField_t      sFilterMatchField;
   /** Filter info.   */
   IFX_PSB6970_QoS_MfcInfo_t            sFilterInfo;
}IFX_PSB6970_QoS_MfcCfg_t;

/** QoS multi-field priority classification rule get all structure.
    Used by \ref IFX_PSB6970_QOS_MFC_ENTRY_READ. */
typedef struct
{
   /** Restart the get operation from the beginning of the table. Otherwise
       return the next table entry (next to the entry that was returned
       during the previous get operation). This boolean parameter is set by the
       calling application. */
   IFX_boolean_t                        bInitial;
   /** Indicates that the read operation has reached the last valid entry in the
       table. This boolean parameter is set by the switch API. */
   IFX_boolean_t                        bLast;
   /** This filter is filled out by the switch API. */
   IFX_PSB6970_QoS_MfcCfg_t             sFilter;
}IFX_PSB6970_QoS_MfcEntryRead_t;

/*@}*/ /* PSB6970_IOCTL_QOS */

/** \addtogroup PSB6970_IOCTL_POWER */
/*@{*/

/** Parameter structure for configuring the power management.
    Used by \ref IFX_PSB6970_POWER_MANAGEMENT_SET
    and \ref IFX_PSB6970_POWER_MANAGEMENT_GET. */
typedef struct
{
   /** Enable/disable power management on this switch device instance. */
   IFX_boolean_t                        bEnable;
}IFX_PSB6970_powerManagement_t;

/*@}*/ /* PSB6970_IOCTL_POWER */

/** \addtogroup PSB6970_IOCTL_DEBUG */
/*@{*/

/** Register access parameter to directly read or write switch
    internal registers.
    Used by \ref IFX_PSB6970_REGISTER_SET and \ref IFX_PSB6970_REGISTER_GET. */
typedef struct
{
   /** Register Address Offset for read or write access. */
   IFX_uint32_t                         nRegAddr;
   /** Value to write to or read from 'nRegAddr'. */
   IFX_uint32_t                         nData;
}IFX_PSB6970_register_t;

/*@}*/ /* PSB6970_IOCTL_DEBUG */

/* ------------------------------------------------------------------------- */
/*                       IOCTL Command Definitions                           */
/* ------------------------------------------------------------------------- */

/** \addtogroup PSB6970_IOCTL_OAM */
/*@{*/

/**
   Forces a hardware reset of the switch device or switch macro. The device
   automatically comes back out of reset and contains the initial values.
   All previous configurations are lost.

   \param IFX_PSB6970_reset_t Pointer to an \ref IFX_PSB6970_reset_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

   \remarks Not supported for all devices
*/
#define IFX_PSB6970_RESET                      _IOW(IFX_PSB6970_MAGIC, 0x01, IFX_PSB6970_reset_t)

/*@}*/ /* PSB6970_IOCTL_OAM */

/** \addtogroup PSB6970_IOCTL_QOS */
/*@{*/

/**
   Configures the QoS rate shaper for the Ethernet port egress strict priority queues.
   The current configuration can be
   retrieved using \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_GET.

   \param IFX_PSB6970_QoS_portShaperCfg_t Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET    _IOW(IFX_PSB6970_MAGIC, 0x02, IFX_PSB6970_QoS_portShaperCfg_t)

/**
   Reads out the current rate shaper for the Ethernet port egress strict priority queues.
   The configuration can be set using \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET.

   \param IFX_PSB6970_QoS_portShaperCfg_t Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_SHAPER_CFG_GET    _IOWR(IFX_PSB6970_MAGIC, 0x03, IFX_PSB6970_QoS_portShaperCfg_t)

/**
   Configures the rate shaper for the Ethernet port egress strict priority queues.
   The current configuration can be
   retrieved using \ref IFX_PSB6970_QOS_PORT_SHAPER_STRICT_GET.

   \param IFX_PSB6970_QoS_portShaperStrictCfg_t Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperStrictCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_SHAPER_STRICT_SET _IOW(IFX_PSB6970_MAGIC, 0x04, IFX_PSB6970_QoS_portShaperStrictCfg_t)

/**
   Reads out the current rate shaper for the Ethernet port egress strict priority queues.
   The configuration can be set using \ref IFX_PSB6970_QOS_PORT_SHAPER_STRICT_SET.

   \param IFX_PSB6970_QoS_portShaperStrictCfg_t Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperStrictCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_SHAPER_STRICT_GET _IOWR(IFX_PSB6970_MAGIC, 0x05, IFX_PSB6970_QoS_portShaperStrictCfg_t)

/**
   Configures the rate shaper for the Ethernet port egress WFQ priority queues.
   The current configuration can be
   retrieved using \ref IFX_PSB6970_QOS_PORT_SHAPER_WFQ_GET.

   \param IFX_PSB6970_QoS_portShaperWFQ_Cfg_t Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperWFQ_Cfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_SHAPER_WFQ_SET    _IOW(IFX_PSB6970_MAGIC, 0x06, IFX_PSB6970_QoS_portShaperWFQ_Cfg_t)

/**
   Reads out the current rate shaper for the Ethernet port egress WFQ priority queues.
   The configuration can be set using \ref IFX_PSB6970_QOS_PORT_SHAPER_WFQ_SET.

   \param IFX_PSB6970_QoS_portShaperWFQ_Cfg_t Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperWFQ_Cfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_SHAPER_WFQ_GET    _IOWR(IFX_PSB6970_MAGIC, 0x07, IFX_PSB6970_QoS_portShaperWFQ_Cfg_t)

/**
   Configures the Ethernet port rate policing for the ingress packets.
   The current configuration can be
   retrieved using \ref IFX_PSB6970_QOS_PORT_POLICER_GET.

   \param IFX_PSB6970_QoS_portPolicerCfg_t Pointer to the rate
      policing parameter \ref IFX_PSB6970_QoS_portPolicerCfg_t for
      the port.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_POLICER_SET       _IOW(IFX_PSB6970_MAGIC, 0x08, IFX_PSB6970_QoS_portPolicerCfg_t)

/**
   Read out the Ethernet port rate policing for the ingress packets.
   The configuration can be set using \ref IFX_PSB6970_QOS_PORT_POLICER_SET.

   \param IFX_PSB6970_QoS_portPolicerCfg_t Pointer to the rate
      policing parameter \ref IFX_PSB6970_QoS_portPolicerCfg_t for
      the port.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_PORT_POLICER_GET       _IOWR(IFX_PSB6970_MAGIC, 0x09, IFX_PSB6970_QoS_portPolicerCfg_t)

/**
   Configure and apply the QoS Multi-field priority classification rules on
   Ethernet port level.

   \param IFX_PSB6970_QoS_MfcPortCfg_t Pointer to a QOS Multi-field
      classification rule \ref IFX_PSB6970_QoS_MfcPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_MFC_PORT_CFG_SET       _IOW(IFX_PSB6970_MAGIC, 0x0A, IFX_PSB6970_QoS_MfcPortCfg_t)

/**
   Read out the current status and configuration of the Ethernet port
   level usage of the QoS Multi-field priority classification rules.

   \param IFX_PSB6970_QoS_MfcPortCfg_t Pointer to a QOS Multi-field
      classification rule \ref IFX_PSB6970_QoS_MfcPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_MFC_PORT_CFG_GET       _IOWR(IFX_PSB6970_MAGIC, 0x0B, IFX_PSB6970_QoS_MfcPortCfg_t)

/**
   Set a QoS Multi-field priority classification rule.

   \param IFX_PSB6970_QoS_MfcCfg_t Pointer to a QOS Multi-field
      classification rule \ref IFX_PSB6970_QoS_MfcCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_MFC_ADD                _IOW(IFX_PSB6970_MAGIC, 0x0C, IFX_PSB6970_QoS_MfcCfg_t)

/**
   Remove a QoS Multi-field priority classification rule.

   \param IFX_PSB6970_QoS_MfcMatchField_t Pointer to a
      QOS Multi-field classification rule \ref IFX_PSB6970_QoS_MfcMatchField_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_MFC_DEL                _IOW(IFX_PSB6970_MAGIC, 0x0D, IFX_PSB6970_QoS_MfcMatchField_t)

/**
   Read an entry of the QoS Multi-field priority classification rules.

   If the parameter 'bInitial=TRUE', the GET operation starts at the beginning
   of the rule set. Otherwise it continues at the entry that
   follows the previous rule.
   The function sets all fields to zero in case the end of the rule set has been reached.
   To read out the complete rule set, this function can be called in a loop.
   The Switch API sets 'bLast=IFX_TRUE' when the last entry has been read out.
   This 'bLast' parameter could be the loop exit criteria.

   \param IFX_PSB6970_QoS_MfcEntryRead_t Pointer to a
      QOS Multi-field classification rule \ref IFX_PSB6970_QoS_MfcEntryRead_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_MFC_ENTRY_READ         _IOWR(IFX_PSB6970_MAGIC, 0x0E, IFX_PSB6970_QoS_MfcEntryRead_t)

/**
   Set the egress storm control for different packet types.
   It allows configuration of different threshold values for different link types.
   The current configuration can be read out using \ref IFX_PSB6970_QOS_STORM_GET.

   \param IFX_PSB6970_QoS_stormCfg_t Pointer to a
      QOS storm control configuration \ref IFX_PSB6970_QoS_stormCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_STORM_SET              _IOW(IFX_PSB6970_MAGIC, 0x0F, IFX_PSB6970_QoS_stormCfg_t)

/**
   Read out the current configuration for the egress storm control.
   The storm control is used for different packet types.
   The configuration can be set using \ref IFX_PSB6970_QOS_STORM_SET.

   \param IFX_PSB6970_QoS_stormCfg_t Pointer to a
   QOS storm control configuration \ref IFX_PSB6970_QoS_stormCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_QOS_STORM_GET              _IOWR(IFX_PSB6970_MAGIC, 0x10, IFX_PSB6970_QoS_stormCfg_t)

/*@}*/ /* PSB6970_IOCTL_QOS */

/** \addtogroup PSB6970_IOCTL_POWER */
/*@{*/

/**
   Set the power management configuration.
   The parameters can be read using \ref IFX_PSB6970_POWER_MANAGEMENT_GET.

   \param IFX_PSB6970_powerManagement_t Pointer to \ref IFX_PSB6970_powerManagement_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_POWER_MANAGEMENT_SET       _IOW(IFX_PSB6970_MAGIC, 0x11, IFX_PSB6970_powerManagement_t)

/**
   Read the power management configuration.
   The parameters can be modified using \ref IFX_PSB6970_POWER_MANAGEMENT_SET.

   \param IFX_PSB6970_powerManagement_t Pointer to \ref IFX_PSB6970_powerManagement_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_POWER_MANAGEMENT_GET       _IOWR(IFX_PSB6970_MAGIC, 0x12, IFX_PSB6970_powerManagement_t)

/*@}*/ /* PSB6970_IOCTL_POWER */

/** \addtogroup PSB6970_IOCTL_DEBUG */
/*@{*/

/**
   Write to an internal register. The register offset defines which register to access
   in which table. This routine only accesses the M4599_PDI and
   the ETHSW_PDI of the switch. All PHY registers are accessed
   via \ref IFX_ETHSW_MDIO_DATA_WRITE and \ref IFX_ETHSW_MDIO_DATA_READ.
   Note that the switch API implementation checks that the given address is
   inside the valid address range. It returns with an error in case an invalid
   address is given.

   \param IFX_PSB6970_register_t Pointer to \ref IFX_PSB6970_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_REGISTER_SET               _IOW(IFX_PSB6970_MAGIC, 0x13, IFX_PSB6970_register_t)

/**
   Read an internal register. The register offset defines which register to access
   in which table. This routine only accesses the M4599_PDI and
   the ETHSW_PDI of the switch. All PHY registers are accessed
   via \ref IFX_ETHSW_MDIO_DATA_WRITE and \ref IFX_ETHSW_MDIO_DATA_READ.
   Note that the switch API implementation checks that the given address is
   inside the valid address range. It returns with an error in case an invalid
   address is given.

   \param IFX_PSB6970_register_t Pointer to \ref IFX_PSB6970_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
#define IFX_PSB6970_REGISTER_GET               _IOWR(IFX_PSB6970_MAGIC, 0x14, IFX_PSB6970_register_t)

/*@}*/ /* PSB6970_IOCTL_DEBUG */

#endif    /* _IFX_ETHSW_PSB6970_H_ */
