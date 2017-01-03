/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef ____INCLUDE_IFX_ETHSW_PSB6970_LL_H
#define ____INCLUDE_IFX_ETHSW_PSB6970_LL_H

#include "ifx_ethsw.h"
#include "ifx_ethsw_PSB6970.h"

/* Group definitions for Doxygen */
/** \defgroup PSB6970_LL Ethernet Switch Application Kernel Interface
    This chapter describes the entire interface to access and
    configure the services of the switch module in OS kernel space. */
/*@{*/
/** \defgroup PSB6970_LL_BRIDGE Ethernet Bridging Functions
    Ethernet bridging (or switching) is the basic task of the device. It
    provides individual configurations per port and standard global
    switch features.
*/
/** \defgroup PSB6970_LL_DEBUG Debug Features
    TANTOS specific features for system integration and debug sessions.
*/
/** \defgroup PSB6970_LL_MULTICAST Multicast Functions
    IGMP/MLD snooping configuration and support for IGMPv1, IGMPv2, IGMPv3,
    MLDv1, and MLDv2.
*/
/** \defgroup PSB6970_LL_OAM Operation, Administration, and Management Functions
    This chapter summarizes the functions that are provided to monitor the
    data traffic passing through the device.
*/
/** \defgroup PSB6970_LL_POWER Power Management
    Configure the TANTOS hardware specific power management.
*/
/** \defgroup PSB6970_LL_QOS Quality of Service Functions
    Switch and port configuration for Quality of Service (QoS).
*/
/** \defgroup PSB6970_LL_VLAN VLAN Functions
    This chapter describes VLAN bridging functionality.
*/
/*@}*/

/* ------------------------------------------------------------------------- */
/*                       Function Declaration                                */
/* ------------------------------------------------------------------------- */

/** \addtogroup PSB6970_LL_BRIDGE */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_8021X_EAPOL_RULE_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_8021X_EAPOL_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_8021X_EAPOL_RuleGet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_EAPOL_Rule_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_8021X_EAPOL_RULE_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_8021X_EAPOL_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_8021X_EAPOL_RuleSet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_EAPOL_Rule_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_8021X_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      802.1x port authorized state port
      configuration \ref IFX_ETHSW_8021X_portCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_8021X_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_8021X_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      802.1x port authorized state port
      configuration \ref IFX_ETHSW_8021X_portCfg_t

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_8021X_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_8021X_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MAC_TABLE_CLEAR command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MAC_TableClear(IFX_void_t *pDevCtx);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MAC_TABLE_ENTRY_ADD command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a MAC table entry
   \ref IFX_ETHSW_MAC_tableAdd_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MAC_TableEntryAdd(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableAdd_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MAC_TABLE_ENTRY_QUERY command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a MAC table entry
   \ref IFX_ETHSW_MAC_tableQuery_t structure that is filled out by the switch
   implementation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MAC_TableEntryQuery(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableQuery_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MAC_TABLE_ENTRY_READ command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a MAC table entry
   \ref IFX_ETHSW_MAC_tableRead_t structure that is filled out by the switch
   implementation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MAC_TableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRead_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a MAC table entry
   \ref IFX_ETHSW_MAC_tableRemove_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MAC_TableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_MAC_tableRemove_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_STP_BPDU_RULE_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_STP_BPDU_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_STP_BPDU_RuleGet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_BPDU_Rule_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_STP_BPDU_RULE_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_STP_BPDU_Rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_STP_BPDU_RuleSet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_BPDU_Rule_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_STP_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_STP_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_STP_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_STP_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_STP_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_STP_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_STP_portCfg_t *pPar);

/*@}*/ /* PSB6970_LL_BRIDGE */
/** \addtogroup PSB6970_LL_VLAN */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_ID_CREATE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_VLAN_IdCreate_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_IdCreate(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdCreate_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_ID_DELETE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an
          \ref IFX_ETHSW_VLAN_IdDelete_t structure element.

   \remarks A VLAN ID can only be removed in case it was created by
         \ref IFX_ETHSW_VLAN_ID_CREATE and is currently not assigned
         to any Ethernet port (done using \ref IFX_ETHSW_VLAN_PORT_MEMBER_ADD).

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_IdDelete(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdDelete_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_ID_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_VLAN_IdGet_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_IdGet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_IdGet_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an
      \ref IFX_ETHSW_VLAN_portCfg_t structure element. Based on the parameter
      'nPortId', the switch API implementation fills out the remaining structure
      elements.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an \ref IFX_ETHSW_VLAN_portCfg_t
      structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_PORT_MEMBER_ADD command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_VLAN_portMemberAdd_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_PortMemberAdd(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberAdd_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_PORT_MEMBER_READ command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_VLAN_portMemberRead_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_PortMemberRead(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberRead_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_PORT_MEMBER_REMOVE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_VLAN_portMemberRemove_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_PortMemberRemove(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_portMemberRemove_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_RESERVED_ADD command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_VLAN_reserved_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_ReservedAdd(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_reserved_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VLAN_RESERVED_REMOVE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_VLAN_reserved_t structure element.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_VLAN_ReservedRemove(IFX_void_t *pDevCtx, IFX_ETHSW_VLAN_reserved_t *pPar);

/*@}*/ /* PSB6970_LL_VLAN */
/** \addtogroup PSB6970_LL_QOS */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_QOS_DSCP_CLASS_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the QoS filter parameters
   \ref IFX_ETHSW_QoS_DSCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_DSCP_ClassGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_ClassCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_QOS_DSCP_CLASS_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the QoS filter parameters
   \ref IFX_ETHSW_QoS_DSCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_DSCP_ClassSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_DSCP_ClassCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_QOS_PCP_CLASS_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the QoS filter parameters
   \ref IFX_ETHSW_QoS_PCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PCP_ClassGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_PCP_ClassCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_QOS_PCP_CLASS_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the QoS filter parameters
   \ref IFX_ETHSW_QoS_PCP_ClassCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PCP_ClassSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_PCP_ClassCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_QOS_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      QOS port priority control configuration \ref IFX_ETHSW_QoS_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_QOS_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      QOS port priority control configuration \ref IFX_ETHSW_QoS_portCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_QoS_portCfg_t *pPar);

/*@}*/ /* PSB6970_LL_QOS */
/** \addtogroup PSB6970_LL_MULTICAST */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_ROUTER_PORT_ADD command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_multicastRouter_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MulticastRouterPortAdd(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouter_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_ROUTER_PORT_READ command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_multicastRouterRead_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs (e.g. Ethernet port parameter out of range)
*/
IFX_return_t IFX_PSB6970_MulticastRouterPortRead(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouterRead_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_ROUTER_PORT_REMOVE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_multicastRouter_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs (e.g. Ethernet port parameter out of range)
*/
IFX_return_t IFX_PSB6970_MulticastRouterPortRemove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastRouter_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_SNOOP_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the
   multicast configuration \ref IFX_ETHSW_multicastSnoopCfg_t.

   \remarks IGMP/MLD snooping is disabled when
   'eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'.
   Then all other structure parameters are unused.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MulticastSnoopCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_multicastSnoopCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_SNOOP_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the
   multicast configuration \ref IFX_ETHSW_multicastSnoopCfg_t.

   \remarks IGMP/MLD snooping is disabled when
   'eIGMP_Mode = IFX_ETHSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'.
   Then all other structure parameters are unused.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MulticastSnoopCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_multicastSnoopCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_TABLE_ENTRY_ADD command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
      to \ref IFX_ETHSW_multicastTable_t.

   \remarks The Source IP parameter is ignored in case IGMPv3 support is
      not enabled in the hardware.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MulticastTableEntryAdd(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_TABLE_ENTRY_READ command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
      to \ref IFX_ETHSW_multicastTableRead_t.

   \remarks The 'bInitial' parameter is reset during the read operation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MulticastTableEntryRead(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTableRead_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MULTICAST_TABLE_ENTRY_REMOVE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
      to \ref IFX_ETHSW_multicastTable_t.

   \remarks The Source IP parameter is ignored in case IGMPv3 support is
      not enabled in the hardware.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MulticastTableEntryRemove(IFX_void_t *pDevCtx, IFX_ETHSW_multicastTable_t *pPar);

/*@}*/ /* PSB6970_LL_MULTICAST */
/** \addtogroup PSB6970_LL_OAM */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CPU_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_CPU_PortCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_CPU_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CPU_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_CPU_PortCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_CPU_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CPU_PORT_EXTEND_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_CPU_PortExtendCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
IFX_return_t IFX_PSB6970_CPU_PortExtendCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortExtendCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CPU_PORT_EXTEND_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_CPU_PortExtendCfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_CPU_PortExtendCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_CPU_PortExtendCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CAP_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to pre-allocated capability
      list structure \ref IFX_ETHSW_cap_t.
      The switch API implementation fills out the structure with the supported
      features, based on the provided 'nCapType' parameter.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
IFX_return_t IFX_PSB6970_CapGet(IFX_void_t *pDevCtx, IFX_ETHSW_cap_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an \ref IFX_ETHSW_cfg_t structure.
      The structure is filled out by the switch implementation.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an \ref IFX_ETHSW_cfg_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_DISABLE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_Disable(IFX_void_t *pDevCtx);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_ENABLE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_Enable(IFX_void_t *pDevCtx);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_HW_INIT command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to pre-allocated initialization structure
   \ref IFX_ETHSW_HW_Init_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_HW_Init(IFX_void_t *pDevCtx, IFX_ETHSW_HW_Init_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MDIO_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_MDIO_cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MDIO_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MDIO_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_MDIO_cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MDIO_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MDIO_DATA_READ command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_MDIO_data_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MDIO_DataRead(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_data_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MDIO_DATA_WRITE command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_MDIO_data_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

*/
IFX_return_t IFX_PSB6970_MDIO_DataWrite(IFX_void_t *pDevCtx, IFX_ETHSW_MDIO_data_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MONITOR_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
          to \ref IFX_ETHSW_monitorPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MonitorPortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_monitorPortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_MONITOR_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
          to \ref IFX_ETHSW_monitorPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_MonitorPortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_monitorPortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a port configuration
   \ref IFX_ETHSW_portCfg_t structure to fill out by the driver.
   The parameter 'nPortId' tells the driver which port parameter is requested.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an \ref IFX_ETHSW_portCfg_t structure
   to configure the switch port hardware.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_LINK_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_portLinkCfg_t structure to read out the port status.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortLinkCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portLinkCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_LINK_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_portLinkCfg_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortLinkCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portLinkCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_PHY_ADDR_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_portPHY_Addr_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortPHY_AddrGet(IFX_void_t *pDevCtx, IFX_ETHSW_portPHY_Addr_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_PHY_QUERY command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_portPHY_Query_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortPHY_Query(IFX_void_t *pDevCtx, IFX_ETHSW_portPHY_Query_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_RGMII_CLK_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_portRGMII_ClkCfg_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortRGMII_ClkCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_portRGMII_ClkCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_RGMII_CLK_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to
      an \ref IFX_ETHSW_portRGMII_ClkCfg_t structure to set the port configuration.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortRGMII_ClkCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_portRGMII_ClkCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_REDIRECT_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
          to \ref IFX_ETHSW_portRedirectCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.
   \remarks Not all hardware platforms support this feature. The function
            returns an error if this feature is not supported.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortRedirectGet(IFX_void_t *pDevCtx, IFX_ETHSW_portRedirectCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_PORT_REDIRECT_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer
          to \ref IFX_ETHSW_portRedirectCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.
   \remarks Not all hardware platforms support this feature. The function
            returns an error if this feature is not supported.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PortRedirectSet(IFX_void_t *pDevCtx, IFX_ETHSW_portRedirectCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_RMON_CLEAR command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar  Pointer to a pre-allocated
   \ref IFX_ETHSW_RMON_clear_t structure. The structure element 'nPortId' is
   an input parameter stating on which port to clear all RMON counters.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_RMON_Clear(IFX_void_t *pDevCtx, IFX_ETHSW_RMON_clear_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_RMON_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar  Pointer to pre-allocated
   \ref IFX_ETHSW_RMON_cnt_t structure. The structure element 'nPortId' is
   an input parameter that describes from which port to read the RMON counter.
   All remaining structure elements are filled with the counter values.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_RMON_Get(IFX_void_t *pDevCtx, IFX_ETHSW_RMON_cnt_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_VERSION_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar* The parameter points to a
   \ref IFX_ETHSW_version_t structure.

   \return Returns value as follows:
   - IFX_SUCCESS: if successful
   - IFX_ERROR: in case of an error

*/
IFX_return_t IFX_PSB6970_VersionGet(IFX_void_t *pDevCtx, IFX_ETHSW_version_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_WOL_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_WoL_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_WoL_CfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_Cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_WOL_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_WoL_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_WoL_CfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_Cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_WOL_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_WoL_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_WoL_PortCfgGet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_PortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_ETHSW_WOL_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_ETHSW_WoL_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_WoL_PortCfgSet(IFX_void_t *pDevCtx, IFX_ETHSW_WoL_PortCfg_t *pPar);

/*@}*/ /* PSB6970_LL_OAM */
/** \addtogroup PSB6970_LL_OAM */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_RESET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to an \ref IFX_PSB6970_reset_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs

   \remarks Not supported for all devices
*/
IFX_return_t IFX_PSB6970_Reset(IFX_void_t *pDevCtx, IFX_PSB6970_reset_t *pPar);

/*@}*/ /* PSB6970_LL_OAM */
/** \addtogroup PSB6970_LL_QOS */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_MFC_ADD command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a QOS Multi-field
      classification rule \ref IFX_PSB6970_QoS_MfcCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_MfcAdd(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_MFC_DEL command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      QOS Multi-field classification rule \ref IFX_PSB6970_QoS_MfcMatchField_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_MfcDel(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcMatchField_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_MFC_ENTRY_READ command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      QOS Multi-field classification rule \ref IFX_PSB6970_QoS_MfcEntryRead_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_MfcEntryRead(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcEntryRead_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_MFC_PORT_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a QOS Multi-field
      classification rule \ref IFX_PSB6970_QoS_MfcPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_MfcPortCfgGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcPortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_MFC_PORT_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a QOS Multi-field
      classification rule \ref IFX_PSB6970_QoS_MfcPortCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_MfcPortCfgSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_MfcPortCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_POLICER_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
      policing parameter \ref IFX_PSB6970_QoS_portPolicerCfg_t for
      the port.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortPolicerGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portPolicerCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_POLICER_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
      policing parameter \ref IFX_PSB6970_QoS_portPolicerCfg_t for
      the port.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortPolicerSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portPolicerCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortShaperCfgGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortShaperCfgSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_SHAPER_STRICT_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperStrictCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortShaperStrictGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperStrictCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_SHAPER_STRICT_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperStrictCfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortShaperStrictSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperStrictCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_SHAPER_WFQ_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperWFQ_Cfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortShaperWfqGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperWFQ_Cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_PORT_SHAPER_WFQ_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to the rate
   shaping parameter \ref IFX_PSB6970_QoS_portShaperWFQ_Cfg_t for
   the port-specific priority queue.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_PortShaperWfqSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_portShaperWFQ_Cfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_STORM_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
   QOS storm control configuration \ref IFX_PSB6970_QoS_stormCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_StormGet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_stormCfg_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_QOS_STORM_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to a
      QOS storm control configuration \ref IFX_PSB6970_QoS_stormCfg_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_QoS_StormSet(IFX_void_t *pDevCtx, IFX_PSB6970_QoS_stormCfg_t *pPar);

/*@}*/ /* PSB6970_LL_QOS */
/** \addtogroup PSB6970_LL_POWER */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_POWER_MANAGEMENT_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_PSB6970_powerManagement_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PowerManagementGet(IFX_void_t *pDevCtx, IFX_PSB6970_powerManagement_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_POWER_MANAGEMENT_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_PSB6970_powerManagement_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_STATUS_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_PowerManagementSet(IFX_void_t *pDevCtx, IFX_PSB6970_powerManagement_t *pPar);

/*@}*/ /* PSB6970_LL_POWER */
/** \addtogroup PSB6970_LL_DEBUG */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_REGISTER_GET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_PSB6970_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_RegisterGet(IFX_void_t *pDevCtx, IFX_PSB6970_register_t *pPar);

/**
   This is the switch API low-level function for
   the \ref IFX_PSB6970_REGISTER_SET command.

   \param pDevCtx This parameter is a pointer to the device context
   which contains all information related to this special instance of the device.
   \param pPar Pointer to \ref IFX_PSB6970_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref IFX_ETHSW_status_t.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurs
*/
IFX_return_t IFX_PSB6970_RegisterSet(IFX_void_t *pDevCtx, IFX_PSB6970_register_t *pPar);

/*@}*/ /* PSB6970_LL_DEBUG */
#endif /* ____INCLUDE_IFX_ETHSW_PSB6970_LL_H */
