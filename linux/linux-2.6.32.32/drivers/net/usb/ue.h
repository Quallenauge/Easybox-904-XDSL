#ifndef __UE_H__
#define __UE_H__

//#define __DEBUGGING__ 1

#ifdef __DEBUGGING__
#define PDEBUG(fmt, ...) printk(KERN_DEBUG "debug  %s:%d %s() : " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
//#define PDEBUG(fmt, ...) printk(KERN_NOTICE "debug  %s:%d %s() : " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define PDEBUG(fmt, ...) do{}while(0)
#endif

#define UE_DEVS 32

#define USB_DEVICE_AND_INTERFACE_INFO(vend, prod, cl, sc, pr) \
	    .match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
        | USB_DEVICE_ID_MATCH_DEVICE, \
    .idVendor = (vend), \
    .idProduct = (prod), \
    .bInterfaceClass = (cl), \
    .bInterfaceSubClass = (sc), \
    .bInterfaceProtocol = (pr)

#endif
