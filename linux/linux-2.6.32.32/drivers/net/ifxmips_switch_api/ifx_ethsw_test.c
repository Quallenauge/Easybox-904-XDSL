/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_test.c
   \remarks implement Kerenl API Test routine.
 *****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <ifx_ethsw_kernel_api.h>
#include <ifx_ethsw_api.h>
#include <asm-generic/ioctl.h>


unsigned int g_cmd=0;
module_param(g_cmd, uint, 0);
union ifx_sw_param x;

int CfgGet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.cfg_Data, 0x00, sizeof(x.cfg_Data));
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_CFG_GET, (unsigned int)&x.cfg_Data);


    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_CFG_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tMAC_Table Age Timer = %d\n", x.cfg_Data.eMAC_TableAgeTimer);
    printk("\tVLAN_Aware = %d\n", x.cfg_Data.bVLAN_Aware);
    printk("\tMax Packet Len = %d\n", x.cfg_Data.nMaxPacketLen);
    printk("\tPause MAC Mode = %d\n", x.cfg_Data.bPauseMAC_ModeSrc);
    printk("\tPause MAC Src = %x:%x:%x:%x:%x:%x\n",
                                             x.cfg_Data.nPauseMAC_Src[0],
                                             x.cfg_Data.nPauseMAC_Src[1],
                                             x.cfg_Data.nPauseMAC_Src[2],
                                             x.cfg_Data.nPauseMAC_Src[3],
                                             x.cfg_Data.nPauseMAC_Src[4],
                                             x.cfg_Data.nPauseMAC_Src[5]
                                             );

    return IFX_SUCCESS;
}

int CfgSet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.cfg_Data, 0x00, sizeof(x.cfg_Data));
    x.cfg_Data.eMAC_TableAgeTimer = 1; // 10 seconds
    x.cfg_Data.bVLAN_Aware = 1; // Disable
    x.cfg_Data.nMaxPacketLen = 1; // 1518 bytes
    x.cfg_Data.bPauseMAC_ModeSrc = 1; // Enable;
    x.cfg_Data.nPauseMAC_Src[0] = 0x0;
    x.cfg_Data.nPauseMAC_Src[1] = 0x11;
    x.cfg_Data.nPauseMAC_Src[2] = 0x22;
    x.cfg_Data.nPauseMAC_Src[3] = 0x33;
    x.cfg_Data.nPauseMAC_Src[4] = 0x44;
    x.cfg_Data.nPauseMAC_Src[5] = 0x66;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_CFG_SET, (unsigned int)&x.cfg_Data);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_CFG_SET Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int PortCfgGet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.portcfg, 0x00, sizeof(x.portcfg));
    x.portcfg.nPortId = 0;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_PORT_CFG_GET, (unsigned int)&x.portcfg);


    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_PORT_CFG_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tPort Id                 = %d\n", x.portcfg.nPortId);
    printk("\tPort Enable             = %d\n", x.portcfg.eEnable);
    printk("\tUnicast Unkown Drop     = %d\n", x.portcfg.bUnicastUnknownDrop);
    printk("\tMulticast Unkown Drop   = %d\n", x.portcfg.bMulticastUnknownDrop);
    printk("\tReserved Packet Drop    = %d\n", x.portcfg.bReservedPacketDrop);
    printk("\tBroadcast Packet Drop   = %d\n", x.portcfg.bBroadcastDrop);
    printk("\tAging                   = %d\n", x.portcfg.bAging);
    printk("\tLearning Mac Port Lock  = %d\n", x.portcfg.bLearningMAC_PortLock);
    printk("\tLearning Limit          = %d\n", x.portcfg.nLearningLimit);
    printk("\tPort Monitor            = %d\n", x.portcfg.ePortMonitor);
    printk("\tFlow Control            = %d\n", x.portcfg.eFlowCtrl);

    return IFX_SUCCESS;
}

int PortCfgSet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.portcfg, 0x00, sizeof(x.portcfg));
    x.portcfg.nPortId = 0;
    x.portcfg.eEnable = 1;
    x.portcfg.bUnicastUnknownDrop = 1;
    x.portcfg.bMulticastUnknownDrop = 1;
    x.portcfg.bReservedPacketDrop = 1;
    x.portcfg.bBroadcastDrop = 1;
    x.portcfg.bAging = 1;
    x.portcfg.bLearningMAC_PortLock = 1;
    x.portcfg.nLearningLimit = 3;
    x.portcfg.ePortMonitor = 1;
    x.portcfg.eFlowCtrl = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_PORT_CFG_SET, (unsigned int)&x.portcfg);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_PORT_CFG_SET Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int PortLinkCfgGet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.portlinkcfgGet, 0x00, sizeof(x.portlinkcfgGet));
    x.portlinkcfgGet.nPortId = 0;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_PORT_LINK_CFG_GET, (unsigned int)&x.portlinkcfgGet);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_PORT_LINK_CFG_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tPort Id         = %d\n", x.portlinkcfgGet.nPortId);
    printk("\tPort Duplex     = %d\n", x.portlinkcfgGet.eDuplex);
    printk("\tLink Speed      = %d\n", x.portlinkcfgGet.eSpeed);
    printk("\tLink Status     = %d\n", x.portlinkcfgGet.eLink);
    printk("\tMII Mode        = %d\n", x.portlinkcfgGet.eMII_Mode);
    printk("\tClock Mode      = %d\n", x.portlinkcfgGet.eClkMode);

    return IFX_SUCCESS;
}

int PortLinkCfgSet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.portlinkcfgSet, 0x00, sizeof(x.portlinkcfgSet));
    x.portlinkcfgSet.nPortId = 0;
    x.portlinkcfgSet.bDuplexForce = 1;
    x.portlinkcfgSet.eDuplex = 1;
    x.portlinkcfgSet.bSpeedForce = 1;
    x.portlinkcfgSet.eSpeed = 1;
    x.portlinkcfgSet.bLinkForce = 1;
    x.portlinkcfgSet.eLink = 1;
    x.portlinkcfgSet.eMII_Mode = 1;
    x.portlinkcfgSet.eClkMode = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_PORT_LINK_CFG_SET, (unsigned int)&x.portlinkcfgSet);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_PORT_LINK_CFG_SET Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int PortRedirectGet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.portRedirectData, 0x00, sizeof(x.portRedirectData));
    x.portRedirectData.nPortId = 0;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_PORT_REDIRECT_GET, (unsigned int)&x.portRedirectData);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_PORT_REDIRECT_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tPort Id                 = %d\n", x.portRedirectData.nPortId);
    printk("\tPort Redirect Egress    = %d\n", x.portRedirectData.bRedirectEgress);
    printk("\tPort Redirect Ingress   = %d\n", x.portRedirectData.bRedirectIngress);
    return IFX_SUCCESS;
}

int PortRedirectSet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.portRedirectData, 0x00, sizeof(x.portRedirectData));
    x.portRedirectData.nPortId = 0;
    x.portRedirectData.bRedirectEgress = 1;
    x.portRedirectData.bRedirectIngress = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_PORT_REDIRECT_SET, (unsigned int)&x.portRedirectData);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_PORT_REDIRECT_SET Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int MAC_tableRead(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.MAC_tableRead, 0x00, sizeof(x.MAC_tableRead));
    x.MAC_tableRead.bInitial = 0;
    x.MAC_tableRead.nPortId = 0;
    x.MAC_tableRead.bLast = 0;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_READ, (unsigned int)&x.MAC_tableRead);


    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_MAC_TABLE_ENTRY_READ Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int MAC_tableAdd(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.MAC_tableAdd, 0x00, sizeof(x.MAC_tableAdd));
    x.MAC_tableAdd.nFId = 1;
    x.MAC_tableAdd.nPortId = 1;
    x.MAC_tableAdd.nAgeTimer = 0;
    x.MAC_tableAdd.bStaticEntry = 1;
    x.MAC_tableAdd.nMAC[0] = 0x0;
    x.MAC_tableAdd.nMAC[1] = 0x11;
    x.MAC_tableAdd.nMAC[2] = 0x22;
    x.MAC_tableAdd.nMAC[3] = 0x33;
    x.MAC_tableAdd.nMAC[4] = 0x44;
    x.MAC_tableAdd.nMAC[5] = 0x55;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_ADD, (unsigned int)&x.MAC_tableAdd);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_MAC_TABLE_ENTRY_ADD Error\n");
        return IFX_ERROR;
    }
    return IFX_SUCCESS;
}

int MAC_tableRemove(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.MAC_tableRemove, 0x00, sizeof(x.MAC_tableRemove));
    x.MAC_tableRemove.nFId = 1;
    x.MAC_tableRemove.nMAC[0] = 0x0;
    x.MAC_tableRemove.nMAC[1] = 0x11;
    x.MAC_tableRemove.nMAC[2] = 0x22;
    x.MAC_tableRemove.nMAC[3] = 0x33;
    x.MAC_tableRemove.nMAC[4] = 0x44;
    x.MAC_tableRemove.nMAC[5] = 0x55;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE, (unsigned int)&x.MAC_tableRemove);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE Error\n");
        return IFX_ERROR;
    }
    return IFX_SUCCESS;
}

int MAC_TableClear(IFX_ETHSW_HANDLE handle)
{
    int ret;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MAC_TABLE_CLEAR, (unsigned int)IFX_NULL);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_MAC_TABLE_CLEAR Error\n");
        return IFX_ERROR;
    }
    return IFX_SUCCESS;
}

int VLAN_IdCreate(IFX_ETHSW_HANDLE handle)
{
    int ret;

    memset(&x.vlan_IdCreate, 0x00, sizeof(x.vlan_IdCreate));
    x.vlan_IdCreate.nVId = 2;
    x.vlan_IdCreate.nFId = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_ID_CREATE, (unsigned int)&x.vlan_IdCreate);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_ID_CREATE Error\n");
        return IFX_ERROR;
    }

    memset(&x.vlan_IdCreate, 0x00, sizeof(x.vlan_IdCreate));
    x.vlan_IdCreate.nVId = 3;
    x.vlan_IdCreate.nFId = 2;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_ID_CREATE, (unsigned int)&x.vlan_IdCreate);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_ID_CREATE Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int VLAN_IdDelete(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.vlan_IdDelete, 0x00, sizeof(x.vlan_IdDelete));
    x.vlan_IdDelete.nVId = 2;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_ID_DELETE, (unsigned int)&x.vlan_IdDelete);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_ID_DELETE Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int VLAN_IdGet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.vlan_IdGet, 0x00, sizeof(x.vlan_IdGet));
    x.vlan_IdGet.nVId = 2;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_ID_GET, (unsigned int)&x.vlan_IdGet);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_ID_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tVId = %d\n", x.vlan_IdGet.nVId);
    printk("\tFId = %d\n", x.vlan_IdGet.nFId);

    return IFX_SUCCESS;
}

int VLAN_PortMemberAdd(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.vlan_portMemberAdd, 0x00, sizeof(x.vlan_portMemberAdd));
    x.vlan_portMemberAdd.nVId = 2;
    x.vlan_portMemberAdd.nPortId = 1;
    x.vlan_portMemberAdd.bVLAN_TagEgress = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_PORT_MEMBER_ADD, (unsigned int)&x.vlan_portMemberAdd);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_PORT_MEMBER_ADD Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int VLAN_PortMemberRemove(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.vlan_portMemberRemove, 0x00, sizeof(x.vlan_portMemberRemove));
    x.vlan_portMemberRemove.nVId = 2;
    x.vlan_portMemberRemove.nPortId = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_PORT_MEMBER_REMOVE, (unsigned int)&x.vlan_portMemberRemove);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_PORT_MEMBER_REMOVE Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int VLAN_PortCfgGet(IFX_ETHSW_HANDLE handle)
{

    int ret;
    memset(&x.vlan_portcfg, 0x00, sizeof(x.vlan_portcfg));
    x.vlan_portcfg.nPortId = 0;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_PORT_CFG_GET, (unsigned int)&x.vlan_portcfg);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_PORT_CFG_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tVLAN PortId           = %d\n", x.vlan_portcfg.nPortId);
    printk("\tVLAN PortVId          = %d\n", x.vlan_portcfg.nPortVId);
    printk("\tVLAN Unknown Drop     = %d\n", x.vlan_portcfg.bVLAN_UnknownDrop);
    printk("\tVLAN ReAssign         = %d\n", x.vlan_portcfg.bVLAN_ReAssign);
    printk("\tVLAN Violation Member = %d\n", x.vlan_portcfg.eVLAN_MemberViolation);
    printk("\tVLAN Admit Mode       = %d\n", x.vlan_portcfg.eAdmitMode);
    printk("\tVLAN TVM              = %d\n", x.vlan_portcfg.bTVM);

    return IFX_SUCCESS;
}

int VLAN_PortCfgSet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.vlan_portcfg, 0x00, sizeof(x.vlan_portcfg));
    x.vlan_portcfg.nPortId = 0;
    x.vlan_portcfg.nPortVId = 3;
    x.vlan_portcfg.bVLAN_UnknownDrop = 1;
    x.vlan_portcfg.bVLAN_ReAssign = 1;
    x.vlan_portcfg.eVLAN_MemberViolation = 1;
    x.vlan_portcfg.eAdmitMode = 1;
    x.vlan_portcfg.bTVM = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_PORT_CFG_SET, (unsigned int)&x.vlan_portcfg);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_PORT_CFG_SET Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int VLAN_ReservedAdd(IFX_ETHSW_HANDLE handle)
{
    int ret;

    memset(&x.vlan_Reserved, 0x00, sizeof(x.vlan_Reserved));
    x.vlan_Reserved.nVId = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_RESERVED_ADD, (unsigned int)&x.vlan_Reserved);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_RESERVED_ADD Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int VLAN_ReservedRemove(IFX_ETHSW_HANDLE handle)
{

    int ret;
    memset(&x.vlan_Reserved, 0x00, sizeof(x.vlan_Reserved));
    x.vlan_Reserved.nVId = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_VLAN_RESERVED_REMOVE, (unsigned int)&x.vlan_Reserved);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_VLAN_RESERVED_REMOVE Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int MDIO_DataRead(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.mdio_Data, 0x00, sizeof(x.mdio_Data));
    x.mdio_Data.nAddressDev = 0;
    x.mdio_Data.nAddressReg = 0;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MDIO_DATA_READ, (unsigned int)&x.mdio_Data);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_MDIO_DATA_READ Error\n");
        return IFX_ERROR;
    }
    printk("\tAddress Device = %d\n", x.mdio_Data.nAddressDev);
    printk("\tAddress Register = %d\n", x.mdio_Data.nAddressReg);
    printk("\tData = 0x%x\n", x.mdio_Data.nData);

    return IFX_SUCCESS;
}

int MDIO_DataWrite(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.mdio_Data, 0x00, sizeof(x.mdio_Data));
    x.mdio_Data.nAddressDev = 0;
    x.mdio_Data.nAddressReg = 0;
    x.mdio_Data.nData = 0x3100;
    ret = ifx_ethsw_kioctl(handle, IFX_ETHSW_MDIO_DATA_WRITE, (unsigned int)&x.mdio_Data);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_ETHSW_MDIO_DATA_WRITE Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

int RegisterGet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.register_access, 0x00, sizeof(x.register_access));
    x.register_access.nRegAddr = 1;
    ret = ifx_ethsw_kioctl(handle, IFX_FLOW_REGISTER_GET, (unsigned int)&x.register_access);
    if(ret != IFX_SUCCESS)
    {
        printk("IFX_FLOW_REGISTER_GET Error\n");
        return IFX_ERROR;
    }
    printk("\tnRegAddr = 0x%x\n", x.register_access.nRegAddr);
    printk("\tnData = 0x%x\n", x.register_access.nData);

    return IFX_SUCCESS;
}

int RegisterSet(IFX_ETHSW_HANDLE handle)
{
    int ret;
    memset(&x.register_access, 0x00, sizeof(x.register_access));
    x.register_access.nRegAddr = 1;
    x.register_access.nData = 0xff;
    ret = ifx_ethsw_kioctl(handle, IFX_FLOW_REGISTER_SET, (unsigned int)&x.register_access);

    if(ret != IFX_SUCCESS)
    {
        printk("IFX_FLOW_REGISTER_SET Error\n");
        return IFX_ERROR;
    }

    return IFX_SUCCESS;
}

void help(void)
{
    printk(" 1: CfgGet\n");
    printk(" 2: CfgSet\n");
    printk(" 3: PortCfgGet\n");
    printk(" 4: PortCfgSet\n");
    printk(" 5: PortLinkCfgGet\n");
    printk(" 6: PortLinkCfgSet\n");
    printk(" 7: PortRedirectGet\n");
    printk(" 8: PortRedirectSet\n");
    printk(" 9: MAC_tableRead\n");
    printk(" 10: MAC_tableAdd\n");
    printk(" 11: MAC_tableRemove\n");
    printk(" 12: MAC_TableClear\n");
    printk(" 13: VLAN_IdCreate\n");
    printk(" 14: VLAN_IdDelete\n");
    printk(" 15: VLAN_IdGet\n");
    printk(" 16: VLAN_PortMemberAdd\n");
    printk(" 17: VLAN_PortMemberRemove\n");
    printk(" 18: VLAN_PortCfgGet\n");
    printk(" 19: VLAN_PortCfgSet\n");
    printk(" 20: VLAN_ReservedAdd\n");
    printk(" 21: VLAN_ReservedRemove\n");
    printk(" 22: MDIO_DataRead\n");
    printk(" 23: MDIO_DataWrite\n");
    printk(" 24: RegisterGet\n");
    printk(" 25: RegisterSet\n");
}

/*
 * IFX_ETHSW_Switch_API_testModule_Init  the init function, called when the module is loaded.
 * Returns zero if successfully loaded, nonzero otherwise.
 */
//IFX_ETHSW_HANDLE handle;
static int __init IFX_ETHSW_Switch_API_testModule_Init(void)
{
    //printk("Init IFX_ETHSW_Switch_API_testModule successfully.\n");

    IFX_ETHSW_HANDLE handle;

    handle = ifx_ethsw_kopen("/dev/switch/0");
    switch (g_cmd)
    {
        case 1: CfgGet(handle); break;
        case 2: CfgSet(handle); break;
        case 3: PortCfgGet(handle); break;
        case 4: PortCfgSet(handle); break;
        case 5: PortLinkCfgGet(handle); break;
        case 6: PortLinkCfgSet(handle); break;
        case 7: PortRedirectGet(handle); break;
        case 8: PortRedirectSet(handle); break;
        case 9: MAC_tableRead(handle); break;
        case 10: MAC_tableAdd(handle); break;
        case 11: MAC_tableRemove(handle); break;
        case 12: MAC_TableClear(handle); break;
        case 13: VLAN_IdCreate(handle); break;
        case 14: VLAN_IdDelete(handle); break;
        case 15: VLAN_IdGet(handle); break;
        case 16: VLAN_PortMemberAdd(handle); break;
        case 17: VLAN_PortMemberRemove(handle); break;
        case 18: VLAN_PortCfgGet(handle); break;
        case 19: VLAN_PortCfgSet(handle); break;
        case 20: VLAN_ReservedAdd(handle); break;
        case 21: VLAN_ReservedRemove(handle); break;
        case 22: MDIO_DataRead(handle); break;
        case 23: MDIO_DataWrite(handle); break;
        case 24: RegisterGet(handle); break;
        case 25: RegisterSet(handle); break;
        default: help();  break;
    }
    ifx_ethsw_kclose(handle);

    return IFX_SUCCESS;
}

/*
 * IFX_ETHSW_Switch_API_testModule_Exit  the exit function, called when the module is removed.
 */
static void __exit IFX_ETHSW_Switch_API_testModule_Exit(void)
{
    printk("Exit from IFX_ETHSW_Switch_API_testModule successfully\n");
}

module_init(IFX_ETHSW_Switch_API_testModule_Init);
module_exit(IFX_ETHSW_Switch_API_testModule_Exit);

MODULE_AUTHOR("Lantiq");
MODULE_DESCRIPTION("IFX ethsw kernel api test module");
MODULE_LICENSE("GPL");
