/****************************************************************************
                              Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 *****************************************************************************
   \file ifx_ethsw_linux.c
   \remarks implement IOCTL wrapper function 
 *****************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <ifx_ethsw_linux.h>
#include <ifx_ethsw_kernel_api.h>
#include <linux/string.h>

/********************/
/* Global Variables */
/********************/
IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIOCTL_Wrapper_Ctx = IFX_NULL;

unsigned int g_debug=0;
module_param(g_debug, uint, 0);

/******************************/
/* Local Macros & Definitions */
/******************************/
#define IFX_ETHSW_SWITCH_DEV_NAME "switch_api"
/*  define and declare a semaphore, named swapi_sem, with a count of one */
static DECLARE_MUTEX(swapi_sem);

/************************/
/* Function Declaration */
/************************/
static int IFX_ETHSW_Open( struct inode *inode, struct file *filp );
static int IFX_ETHSW_Release( struct inode *inode, struct file *filp );
static int IFX_ETHSW_Ioctl(struct inode *inode, struct file *filp, unsigned int nCmd, unsigned long nArg);

typedef enum
{
    IFX_ETHSW_API_USERAPP = 0,
    IFX_ETHSW_API_KERNEL 
} IFX_ETHSW_apiType_t;

/*****************/
/* Function Body */
/*****************/
/**
   This is the switch ioctl wrapper function.

   \param pHandle This parameter is a pointer to the ioctl handle context.
   \param command This is ioctl command.
   \param arg This ioctl parameter pass from user space.

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurred.
*/
IFX_return_t IFX_ETHSW_LowLevelCommandSearch (void *pHandle, int command, int arg, IFX_ETHSW_apiType_t apiType)
{
	IFX_ETHSW_ioctlHandle_t *pDrv = (IFX_ETHSW_ioctlHandle_t*) pHandle;
	IFX_ETHSW_lowLevelFkts_t *pLlTable = pDrv->pLlTable;
	/*  attempt to acquire the semaphore ... */
	if (down_interruptible(&swapi_sem)) {
		/*  signal received, semaphore not acquired ... */
		return IFX_ERROR;
	}
#ifdef DEBUG
	IFXOS_PRINT_INT_RAW("DEBUG> %s[%d]: IOC_TYPE=%x, IOC_NR=%x \n",__func__,__LINE__,	\
    	_IOC_TYPE(command), _IOC_NR(command));
#endif
	do {
		if (_IOC_TYPE(command) == pLlTable->nType) {
			/* This table contains the low-level function for the IOCTL commands with the same MAGIC-Type numer. */
			IFX_ll_fkt     fkt;
			IFX_uint32_t   size;
			IFX_return_t   retvalue;
			IFX_uint32_t   cmdNr = _IOC_NR(command);
			/* Number out of range. No function available for this command number. */
			if (cmdNr >= pLlTable->nNumFkts) {
#ifdef DEBUG
            IFXOS_PRINT_INT_RAW("DEBUG> %s[%d]: cmdNr=%d, nNumFkts=%d \n",__func__,__LINE__, cmdNr, pLlTable->nNumFkts);
#endif	
				goto Fail;
			}
			fkt = pLlTable->pFkts[cmdNr];
			/* No low-level function given for this command. */
			if (fkt == IFX_NULL) {
				IFXOS_PRINT_INT_RAW("ERROR %s[%d]: cmdNr=%d, nNumFkts=%d \n",__func__,__LINE__, cmdNr, pLlTable->nNumFkts);
				goto Fail;
			}
			/* Copy parameter from userspace. */
			size = _IOC_SIZE(command);
			/* Local temporary buffer to store the parameter is to small. */
			if (size > PARAM_BUFFER_SIZE) {
				IFXOS_PRINT_INT_RAW("ERROR %s[%d]: cmdNr=%d, nNumFkts=%d \n",__func__,__LINE__, cmdNr, pLlTable->nNumFkts);
				goto Fail;
			}
			if ( apiType == IFX_ETHSW_API_USERAPP ) {
				copy_from_user((void *)(pDrv->paramBuffer),(const void __user *) arg, (unsigned long)size);
				/* Now call the low-level function with the right low-level context \
				handle and the local copy of the parameter structure of 'arg'. */
				
#ifdef DEBUG_LINUX_WRAPPER
            IFXOS_PRINT_INT_RAW("pDrv=%p, pDrv->pLlHandle=%p\n", pDrv, pDrv->pLlHandle);
#endif
				retvalue = fkt(pDrv->pLlHandle, (IFX_uint32_t)pDrv->paramBuffer);
				/* Copy parameter to userspace. */
				if (_IOC_DIR(command) & _IOC_READ) {
					/* Only copy back to userspace if really required */
					copy_to_user((void __user *)arg, (const void *)(pDrv->paramBuffer), (unsigned long)size);
				}
			} else {
				memcpy((void *)(pDrv->paramBuffer),(const void *) arg, (unsigned long)size);
				retvalue = fkt(pDrv->pLlHandle, (IFX_uint32_t)pDrv->paramBuffer);
				memcpy((void *)arg, (const void *)(pDrv->paramBuffer), (unsigned long)size);
			}
			up(&swapi_sem);
			return retvalue;
		}
		/* If command was not found in the current table index, look for the next \
		linked table. Search till it is found or we run out of tables. */
		pLlTable = pLlTable->pNext;
	}while (pLlTable != IFX_NULL);
#ifdef DEBUG
		IFXOS_PRINT_INT_RAW("DEBUG> %s[%d]: No supported command low-level function found.\n",__func__,__LINE__);
#endif
Fail:
	/*  release the given semaphore */
	up(&swapi_sem);
   /* No supported command low-level function found. */
   return IFX_ERROR;
}

/** The driver callbacks that will be registered with the kernel*/
static struct file_operations swapi_fops =
{
	owner:	THIS_MODULE,
	ioctl:	IFX_ETHSW_Ioctl,
	open:	IFX_ETHSW_Open,
	release:	IFX_ETHSW_Release
};

/**
   This is the switch ioctl wrapper function.
   \param *inode Pointer to struct inode
   \param *filp Pointer to struct file
   \param nCmd This is ioctl command.
   \param nArg This ioctl parameter pass from user space. 

   \return Return value as follows:
   - IFX_SUCCESS: if successful
   - An error code in case an error occurred.
*/
static int IFX_ETHSW_Ioctl(struct inode *inode, struct file *filp,unsigned int nCmd, unsigned long nArg)
{
	IFX_ETHSW_devoneData_t *p;
	IFX_int32_t ret;
	IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pDev;
	p = filp->private_data;
	pDev=pIOCTL_Wrapper_Ctx;
	
#ifdef DEBUG
	IFXOS_PRINT_INT_RAW("DEBUG>  Minor number = %d\n", p->minor_number);
#endif
	if ( !p->minor_number ) {
		if ( pDev->bInternalSwitch == IFX_TRUE )
			pDev->pIoctlHandle->pLlHandle = pDev->pEthSWDev[0];
        else {
        	IFXOS_PRINT_INT_RAW("\nNot support internal switch\n\n" );
        	return IFX_ERROR;
        }
    } else {
		if ( p->minor_number <= pDev->nExternalSwitchNum   ) {
			pDev->pIoctlHandle->pLlHandle = pDev->pEthSWDev[p->minor_number];
		} else {
    		IFXOS_PRINT_INT_RAW("ERROR: (Not support external switch number: %d) %s:%s:%d \n",p->minor_number,	\
    			__FILE__, __FUNCTION__, __LINE__);
    		return IFX_ERROR;
    	}
    }
    ret = IFX_ETHSW_LowLevelCommandSearch (pDev->pIoctlHandle, nCmd, nArg, IFX_ETHSW_API_USERAPP);
    return ret;
}

/**
   This is the switch char device open function.
   \param inode Pointer to struct inode.
   \param filp Pointer to struct file.
   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
static int IFX_ETHSW_Open( struct inode *inode, struct file *filp )
{
	unsigned int minorNum, majorNum;
	IFX_ETHSW_devoneData_t *p;

	minorNum = MINOR(inode->i_rdev);
	majorNum = MAJOR(inode->i_rdev);
	p = kmalloc(sizeof(IFX_ETHSW_devoneData_t), GFP_KERNEL);
    if (!p) {
    	IFXOS_PRINT_INT_RAW(KERN_INFO "IFX_ETHSW_Open: kmalloc failed\n\r");
    	return -ENOMEM;
    }
    p->minor_number = minorNum;
    filp->private_data = p;
    return IFX_SUCCESS;
}

/**
   This is the switch char device release function.
   \param inode Pointer to struct inode.
   \param filp Pointer to struct file.
   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
static int IFX_ETHSW_Release( struct inode *inode, struct file *filp )
{
	if (filp->private_data) {
		kfree(filp->private_data);
		filp->private_data = NULL;
	}
    return IFX_SUCCESS;
}

/**
   This is the switch char device register function.
   \param Major Manor number.
   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_Drv_Register(IFX_uint32_t Major)
{
	IFX_int32_t result;
	
	result = register_chrdev (Major, IFX_ETHSW_SWITCH_DEV_NAME, &swapi_fops);
	if (result < 0) {
		IFXOS_PRINT_INT_RAW (KERN_INFO "SWAPI: Register Char Dev failed with %d !!!\n",result);
		return  result;
	}
	IFXOS_PRINT_INT_RAW (KERN_INFO "SWAPI: Registered character device [%s] with major no [%d]\n", \
		IFX_ETHSW_SWITCH_DEV_NAME, Major);
	return IFX_SUCCESS;
}

/**
   This is the switch char device unregister function.
   \param Major Manor number.
   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_Drv_UnRegister (IFX_uint32_t Major)
{
	unregister_chrdev(Major, IFX_ETHSW_SWITCH_DEV_NAME);
	return IFX_SUCCESS;
}

/**
   This is the switch ioctl wrapper init function.
   \param pInit Pointer to init data structure
   \return Return value as follows:
   - pDev: if successful
*/
IFX_void_t *IFX_ETHSW_IOCTL_WrapperInit(IFX_ETHSW_IOCTL_wrapperInit_t *pInit)
{
	IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pDev;
	IFX_uint8_t i;
	pDev = (IFX_ETHSW_IOCTL_WRAPPER_CTX_t*) kmalloc (sizeof (IFX_ETHSW_IOCTL_WRAPPER_CTX_t), GFP_KERNEL);
	if (!pDev) {
    	IFXOS_PRINT_INT_RAW(KERN_INFO "IFX_ETHSW_Open: kmalloc failed\n\r");
    	return pDev;
    }
    pDev->bInternalSwitch = IFX_FALSE;
    pDev->nExternalSwitchNum = 0;
    pDev->pIoctlHandle = (IFX_ETHSW_ioctlHandle_t*) kmalloc (sizeof (IFX_ETHSW_ioctlHandle_t), GFP_KERNEL);
    if (!pDev->pIoctlHandle) {
    	IFXOS_PRINT_INT_RAW(KERN_INFO "IFX_ETHSW_Open: kmalloc failed\n\r");
    	if(pDev) kfree(pDev);
    	pDev = IFX_NULL;
    	return (pDev->pIoctlHandle);
    }
    pDev->pIoctlHandle->pLlTable = pInit->pLlTable;
    for (i=0; i<IFX_ETHSW_DEV_MAX; i++) {
    	pDev->pEthSWDev[i] = IFX_NULL;
    }
    pIOCTL_Wrapper_Ctx = pDev;
    return pDev;
}

/**
   This is the switch ioctl wrapper add device function.
   \param pIoctlDev Pointer to ioctl wrapper context. 
   \param pCoreDev Pointer to low level device context.
   \param eDevType Device type. 
   \param eMinorNum Minor number.
   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_IOCTL_WrapperDeviceAdd(IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pIoctlDev, IFX_void_t *pCoreDev, IFX_uint8_t nMinorNum)
{
	if ( nMinorNum > IFX_ETHSW_DEV_MAX ) {
		IFXOS_PRINT_INT_RAW("ERROR: (Device number: %d) %s:%s:%d \n",nMinorNum, __FILE__, __FUNCTION__, __LINE__);
		return IFX_ERROR;
	}
	pIoctlDev->pEthSWDev[nMinorNum] = pCoreDev;
	if ( !nMinorNum )
		pIoctlDev->bInternalSwitch = IFX_TRUE;
	else /* other than 0 means external switch */
		pIoctlDev->nExternalSwitchNum++;
    
#ifdef DEBUG_LINUX_WRAPPER
    IFXOS_PRINT_INT_RAW("\n Device Number  = %d\n\n", nMinorNum);
#endif
    return IFX_SUCCESS;
}

/**
   This is the switch ioctl wrapper cleanup function.
   \return Return value as follows:
   - IFX_SUCCESS: if successful
*/
IFX_return_t IFX_ETHSW_IOCTL_WrapperCleanUp(IFX_void_t)
{
	IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pDev = pIOCTL_Wrapper_Ctx;
	
	if ( pDev != IFX_NULL ) {
		if (pDev->pIoctlHandle != IFX_NULL ) {
			kfree(pDev->pIoctlHandle);
			pDev->pIoctlHandle = IFX_NULL;
		}
		kfree(pDev);
		pDev = IFX_NULL;
	}
    return IFX_SUCCESS;
}

IFX_ETHSW_HANDLE ifx_ethsw_kopen(char *name)
{
	IFX_ETHSW_IOCTL_WRAPPER_CTX_t *pDev;
	/* process /dev/switch/minor string */
	char *needle = "/";
    char *buf = strstr(name, needle);
    pDev=pIOCTL_Wrapper_Ctx;
    name=buf+strlen(needle); /* pointer to dev */
    buf = strstr(name, needle);
    name=buf+strlen(needle); /* pointer to switch */
    buf = strstr(name, needle);
    name=buf+strlen(needle); /* pointer to minor */
     
    if (!strcmp(name, "0") ) {
    	if ( pDev->bInternalSwitch == IFX_TRUE )
    		pDev->pIoctlHandle->pLlHandle = pDev->pEthSWDev[0];
    	else {
    		IFXOS_PRINT_INT_RAW("\nNot support internal switch\n\n" );
    		return 0;
    	}
    } else if (!strcmp(name, "1") )
    	pDev->pIoctlHandle->pLlHandle = pDev->pEthSWDev[1];
    else {
    	IFXOS_PRINT_INT_RAW("\nNot support external switch number = %s\n\n", name);
        return 0;
    } 
    return (IFX_ETHSW_HANDLE)pDev->pIoctlHandle;
}

int ifx_ethsw_kioctl(IFX_ETHSW_HANDLE handle, unsigned int command, unsigned int arg)
{
    return (IFX_ETHSW_LowLevelCommandSearch ((void *)handle, command, arg, IFX_ETHSW_API_KERNEL));
}

int ifx_ethsw_kclose(IFX_ETHSW_HANDLE handle)
{
	IFX_ETHSW_ioctlHandle_t *pDrv = (IFX_ETHSW_ioctlHandle_t*) handle;
	
	if (pDrv->pLlHandle != IFX_NULL)
		pDrv->pLlHandle = IFX_NULL;
	return IFX_SUCCESS;
}

EXPORT_SYMBOL(ifx_ethsw_kopen);
EXPORT_SYMBOL(ifx_ethsw_kioctl);
EXPORT_SYMBOL(ifx_ethsw_kclose);
