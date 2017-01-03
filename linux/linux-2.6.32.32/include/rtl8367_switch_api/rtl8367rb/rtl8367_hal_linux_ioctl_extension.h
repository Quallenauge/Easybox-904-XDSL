#ifndef __RTL8367_HAL_LINUX_IOCTL_EXTENSION__
#define __RTL8367_HAL_LINUX_IOCTL_EXTENSION__

#include <linux/ioctl.h> 									/* needed for the _IOW etc stuff used later */

#include <rtl83XX_hal.h>

/*-------------------------------------------------------------
  RTL8367RB HAL structure -- GLOBAL
--------------------------------------------------------------- */

/*-------------------------------------------------------------
  RTL8367RB HAL IOCTL Definitions -- GLOBAL
--------------------------------------------------------------- */
/* Check from Documentation/ioctl/ioctl-number.txt */
#define  RTL8367_IOC_MAGIC								0x21
#define  RTL8367_IO(reqType)							_IO(RTL8367_IOC_MAGIC, reqType)
#define  RTL8367_IOR(reqType, dataType)		_IOR(RTL8367_IOC_MAGIC, reqType, dataType)
#define  RTL8367_IOW(reqType, dataType)		_IOW(RTL8367_IOC_MAGIC, reqType, dataType)
#define  RTL8367_IOWR(reqType, dataType)	_IOWR(RTL8367_IOC_MAGIC, reqType, dataType)

#define  RTL8367_IOC_MAXNR 		99 

/*
I/O Command List

No. 	 Class
[0x00~0x0F] 
0x00 : RTL8367_IOCTL_INIT
0x01 : RTL8367_IOCTL_MAXPKTLEN_SET
0x02 : RTL8367_IOCTL_MAXPKTLEN_GET
0x03 : RTL8367_IOCTL_PORTMAXPKTLEN_SET
0x04 : RTL8367_IOCTL_PORTMAXPKTLEN_GET
0x05 : RTL8367_IOCTL_CPUPORTTAGEN_SET
0x06 : RTL8367_IOCTL_CPUPORTTAGEN_GET
0x07 : RTL8367_IOCTL_CPUPORTTAG_SET
0x08 : RTL8367_IOCTL_CPUPORTTAG_GET
0x09 : 
0x0A : 
0x04 : 

[0x10~0x1F]			L2 LookUpTbl 
0x10 : RTL8367_IOCTL_L2LUT_INIT
0x11 : RTL8367_IOCTL_L2LUT_TYPE_SET
0x12 : RTL8367_IOCTL_L2LUT_TYPE_GET
0x13 :
0x14 :
0x15 : RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_ADD
0x16 : RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DEL
0x17 : RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DUMP

[0x20~0x2F]     Port MAC/PHY Configuration
0x20 : RTL8367_IOCTL_PORTLINKCFG_GET
0x21 : RTL8367_IOCTL_PORTLINKPHYCFG_GET
0x22 : RTL8367_IOCTL_PORTFORCELINKEXTCFG_SET
0x23 : RTL8367_IOCTL_PORTFORCELINKEXTCFG_GET
0x24 : RTL8367_IOCTL_PORTRGMIICLKEXT_SET
0x25 : RTL8367_IOCTL_PORTRGMIICLKEXT_GET
0x26 : RTL8367_IOCTL_PHYTESTMODE_SET
0x27 : RTL8367_IOCTL_PHYTESTMODE_GET
0x28 : 
0x29 : 
0x2A : 

[0x30~0x3F]			HW IGMP
0x30 :
0x31 :
0x32 : 
0x33 :
0x34 :
0x35 :
0x36 : 
0x37 :

[0x40~0x4F]			CPU Port 
0x40 : 
0x41 : 
0x42 : 
0x43 : 
0x44 : 
0x45 :
0x46 : 
0x47 :
0x48 : 
0x49 :

[0x50~0x5F]




*/



/* [0x00~0x0F] =========================================================================== */ 
/* Set chip to default configuration enviroment */
#define RTL8367_IOCTL_INIT														RTL8367_IO(0x00)

/* Set the max packet length for whole system setting, 1st configuration */
#define RTL8367_IOCTL_MAXPKTLEN_SET										RTL8367_IOW(0x01, RTK_PORT_MAXPKTLEN)

/* Get the max packet length for whole system setting, 1st configuration */
#define RTL8367_IOCTL_MAXPKTLEN_GET										RTL8367_IOR(0x02, RTK_PORT_MAXPKTLEN)

/* Set the max packet length for certain port setting, 2st configuration */
#define RTL8367_IOCTL_PORTMAXPKTLEN_SET								RTL8367_IOW(0x03, RTK_PORT_MAXPKTLEN)

/* Get the max packet length for certain port setting, 2st configuration */
#define RTL8367_IOCTL_PORTMAXPKTLEN_GET								RTL8367_IOR(0x04, RTK_PORT_MAXPKTLEN)

/* Set cpu tag function enable/disable, If CPU tag function is disabled,  
	 CPU tag will not be added to frame forwarded to CPU port, and all ports 
	 cannot parse CPU tag. */
#define	RTL8367_IOCTL_CPUPORTTAGEN_SET								RTL8367_IOW(0x05, unsigned int)

/* Get cpu tag function enable/disable */
#define	RTL8367_IOCTL_CPUPORTTAGEN_GET								RTL8367_IOR(0x06, unsigned int)

/* Set CPU port and inserting proprietary CPU tag mode to the frame that 
	 transmitting to CPU port. */
#define	RTL8367_IOCTL_CPUPORTTAG_SET									RTL8367_IOW(0x07, RTK_CPUPORT_TAG)

/* Get cpu tag function enable/disable */
#define	RTL8367_IOCTL_CPUPORTTAG_GET									RTL8367_IOR(0x08, RTK_CPUPORT_TAG)


/* [0x10~0x1F] =========================================================================== */ 
/* Initialize l2 module of the specified device. */
#define RTL8367_IOCTL_L2LUT_INIT											RTL8367_IO(0x10)

/* Set Lut IP multicast lookup function
	 If users set the lookup type to DIP, the group in exception table will be lookup by DIP+SIP
   If users set the lookup type to DIP+SIP, the group in exception table will be lookup by only DIP */
#define RTL8367_IOCTL_L2LUT_TYPE_SET									RTL8367_IOW(0x11, unsigned int)

/* Get Lut IP multicast lookup function */
#define RTL8367_IOCTL_L2LUT_TYPE_GET									RTL8367_IOR(0x12, unsigned int)

/* Get MAC table entry */
#define RTL8367_IOCTL_L2LUT_MAC_GET										RTL8367_IOR(0x13, RTK_L2LUT_STATUS)

#define RTL8367_IOCTL_L2LUT_MAC_GETNEXT								RTL8367_IOR(0x14, RTK_L2LUT_STATUS)

#if 0
#define RTL8367_IOCTL_L2LUT_MACADDR_ADD								RTL8367_IOW(0x15, RTK_MACTBL_STATUS)

#define RTL8367_IOCTL_L2LUT_MACADDR_DEL								RTL8367_IOW(0x16, RTK_MACTBL_STATUS)

/* Read out the unicast membership table that is located inside the switch hardware by port. */
#define RTL8367_IOCTL_L2LUT_MAC_DUMP_BY_PORT					RTL8367_IOR(0x17, unsigned int)
#endif

/* Read out the unicast membership table that is located inside the switch hardware. */
#define RTL8367_IOCTL_L2LUT_MAC_DUMP      						RTL8367_IO(0x18)

#if 0
#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_GET      	RTL8367_IOWR(0x19, RTK_L2LUT_STATUS)
#endif

#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_GETNEXT   RTL8367_IOR(0x1A, RTK_L2LUT_STATUS)


/* Adds a multicast group configuration to the multicast table. 
   No new entry is added in case this multicast group already exists in the table. 
   This commands adds a host member to the multicast group.*/
#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_ADD				RTL8367_IOW(0x1B, RTK_L2LUT_STATUS)

/* Remove an host member from a multicast group. 
   The multicast group entry is completely removed from the multicast table in case it has no host member port left. */
#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DEL				RTL8367_IOW(0x1C, RTK_L2LUT_STATUS)

/* Read out the multicast membership table that is located inside the switch hardware. */
#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DUMP      RTL8367_IO(0x1D)


/* [0x20~0x2F] =========================================================================== */ 
/* Get port link configuration. */
#define RTL8367_IOCTL_PORTLINKCFG_GET									RTL8367_IOR(0x20, RTK_PORT_LINK_STATUS)

/* Get port PHY status configuration. */
#define RTL8367_IOCTL_PORTLINKPHYCFG_GET							RTL8367_IOR(0x21, RTK_PORT_LINKPHY_STATUS)

/* Set port force linking configuration. */
#define RTL8367_IOCTL_PORTFORCELINKEXTCFG_SET					RTL8367_IOW(0x22, RTK_PORT_FORCELINKEXT_STATUS)

/* Get port force linking configuration. */
#define RTL8367_IOCTL_PORTFORCELINKEXTCFG_GET					RTL8367_IOR(0x23, RTK_PORT_FORCELINKEXT_STATUS)

/* Set port force linking configuration. */
#define RTL8367_IOCTL_PORTRGMIICLKEXT_SET							RTL8367_IOW(0x24, RTK_PORT_RGMIIDELAYEXT)

/* Get RGMII interface delay value for TX and RX. */
#define RTL8367_IOCTL_PORTRGMIICLKEXT_GET							RTL8367_IOR(0x25, RTK_PORT_RGMIIDELAYEXT)

/* Set PHY in test mode and only one PHY can be in test mode at the same time. */
#define RTL8367_IOCTL_PHYTESTMODE_SET									RTL8367_IOW(0x26, RTK_PORT_PHYTESTMODE)

/* Get PHY in which test mode. */
#define RTL8367_IOCTL_PHYTESTMODE_GET									RTL8367_IOR(0x27, RTK_PORT_PHYTESTMODE)

/* [0x50~0x5F] =========================================================================== */ 
/* Set Register Data */
#define RTL8367_IOCTL_REGISTER_SET										RTL8367_IOW(0x50, RTK_REGISTER)

/* Get Register Data */
#define RTL8367_IOCTL_REGISTER_GET										RTL8367_IOR(0x51, RTK_REGISTER)

/* Set PHY on of the specific port. */
#define RTL8367_IOCTL_PORTPOWER_ONOFF									RTL8367_IOW(0x52, RTK_PORTPOWER)


#endif // __RTL8367_HAL_LINUX_IOCTL_EXTENSION__
