/*
 *	Generic parts
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/llc.h>
#include <net/llc.h>
#include <net/stp.h>

#include "br_private.h"

int (*br_should_route_hook)(struct sk_buff *skb);

static const struct stp_proto br_stp_proto = {
	.rcv	= br_stp_rcv,
};

static struct pernet_operations br_net_ops = {
	.exit	= br_net_exit,
};


#ifdef CONFIG_ARC_BR_PORT_ISOLATION /*ctc*/

 #include <linux/proc_fs.h>

 #define	BR_FILTER_MAX_ENTRY		8

 typedef struct {
	char	sIfname[2][IFNAMSIZ]; /* sIfname[0][0]=='\0' means entry free */
 } stBrIfIsoPair;

 static int				_stBrIfIsoEnable = 0;
 static int				_stBrIfIsoDebug = 0;
 static stBrIfIsoPair	_stBrIfIso[ BR_FILTER_MAX_ENTRY ];

 #define	BR_FILTER_ENTRY_OCCUPY( id )	(_stBrIfIso[id].sIfname[0][0] = ' ')
 #define	BR_FILTER_ENTRY_FREE( id )		(_stBrIfIso[id].sIfname[0][0] = '\0')
 #define	BR_FILTER_ENTRY_ISFREE( id )	(_stBrIfIso[id].sIfname[0][0] == '\0')

static void br_if_iso_proc_init( void )
{
	int	cnt;

	_stBrIfIsoEnable = 0;
	_stBrIfIsoDebug = 0;

	for (cnt=0; cnt<BR_FILTER_MAX_ENTRY; cnt++) {
		BR_FILTER_ENTRY_FREE(cnt);
	}
}

/*******************************************************************************
 * Description
 *		display port-isolated interface pair data via /proc/net/br virtual file.
 *
 ******************************************************************************/
static int br_if_iso_proc_read(	char*	page,	/* buffer where we should write*/
								char**	start,	/* seem to be never used in the kernel */
								off_t	off,	/* where we should start to write */
								int		count,	/* how many character we could write */
								int*	eof,	/* used to signal the end of file */
								void*	data )	/* only used if we have defined our own buffer */
{
	int		len, cnt, cnt2;

    page += off ;
    page [ 0 ] = 0 ;

	len = 0;
    cnt = sprintf( page+len, "administrative status : %s\n", (_stBrIfIsoEnable==0 ? "disable" : "enable") ) ;
	len += cnt;
    cnt = sprintf( page+len, "debug status          : %s\n", (_stBrIfIsoDebug==0 ? "off" : "on") ) ;
	len += cnt;
	for (cnt2=0; cnt2<BR_FILTER_MAX_ENTRY; cnt2++) {
		if ( BR_FILTER_ENTRY_ISFREE(cnt2) )
			continue;
	    cnt = sprintf( page+len, "pair %d                : %s / %s\n", cnt2+1, _stBrIfIso[cnt2].sIfname[0], _stBrIfIso[cnt2].sIfname[1] ) ;
		len += cnt;
	}

    *eof = 1;
    
    return len; /* return number of bytes returned */
}

static void br_if_iso_proc_write_help( void )
{
	printk(	"\n"
			"Bridge interface/port isolation commands:\n"
			"    help                    - show    this help message\n"
			"    enable                  - enable  port isolation function\n"
			"    disable                 - disable port isolation function\n"
			"    debugOn                 - enable  port isolation debugging\n"
			"    debugOff                - disable port isolation debugging\n"
			"    add <ifname1> <ifname2> - stop    traffic between interfaces ifname1 and ifnames\n"
			"    del <ifname1> <ifname2> - allow   traffic between interfaces ifname1 and ifnames\n"
			"  Max entries       : %d\n"
			"  ifname max length : %d\n"
			"\n", BR_FILTER_MAX_ENTRY, IFNAMSIZ-1
		  );
}

static int br_if_iso_get_free_entry( void )
{
	int	cnt;

	for (cnt=0; cnt<BR_FILTER_MAX_ENTRY; cnt++) {
		if ( BR_FILTER_ENTRY_ISFREE(cnt) )
			return cnt;
	}

	return -1;
}

static int br_if_iso_entry_id( char* ifname1, char* ifname2 )
{
	int	cnt;

	if (ifname1 == 0 || ifname2 == 0)
		return -1;

	for (cnt=0; cnt<BR_FILTER_MAX_ENTRY; cnt++) {
		if ( BR_FILTER_ENTRY_ISFREE(cnt) )
			continue;
		if (strcmp(_stBrIfIso[cnt].sIfname[0],ifname1) == 0 
		 && strcmp(_stBrIfIso[cnt].sIfname[1],ifname2) == 0)
			return cnt;
		if (strcmp(_stBrIfIso[cnt].sIfname[0],ifname2) == 0 
		 && strcmp(_stBrIfIso[cnt].sIfname[1],ifname1) == 0)
			return cnt;
	}

	return -1;
}

int br_if_iso_entry_existed( char* ifname1, char* ifname2 )
{
	return (_stBrIfIsoEnable != 0 && br_if_iso_entry_id( ifname1, ifname2 ) >= 0);
}

int br_if_iso_is_debug_on( void )
{
	return (_stBrIfIsoDebug != 0);
}

static long arc_strlen_nospace( char *str )
{
	int	cnt;

	if (str == 0)
		return -1;

	while (*str==' ' || *str=='\t')
		str++;

	cnt = 0;
	while (*str != '\0' && *str!=' ' && *str!='\t') {
		cnt++;
		str++;
	}

	return cnt;
}

static long arc_strcpy_nospace( char *dst, char *src, char **endp )
{
	if (dst==0 || src==0)
		return -1;

	if (arc_strlen_nospace(src) >= IFNAMSIZ)
		return -1;

	while (*src==' ' || *src=='\t')
		src++;

	while (*src != '\0' && *src != ' ' && *src != '\t')
		*dst++ = *src++;

	*dst = '\0';

	if (endp != 0)
		*endp = src;

	return 0;
}

/*******************************************************************************
 * Description
 *		set port-isolated interface pair data via /proc/net/br virtual file.
 *
 ******************************************************************************/
static int br_if_iso_proc_write(struct file*	file,	/* this parameter is usually ignored */
								const char*		buffer,	/* it contains data that is passed to the
															module (at maximum of possible).
															buffer is not in kernel memory(ro) */
								unsigned long	count,	/* says how many bytes should be read */
								void*			data )	/* this parameter could be set to use our own
															buffer (i.e.: using a method for many files */
{
	char*	pPtr;

	/* trim the tailing space, tab and LF/CR*/
	if ( count > 0 )
	{
		pPtr = (char*)buffer + count - 1;
		for (; *pPtr==' ' || *pPtr=='\t' || *pPtr=='\n' || *pPtr=='\r'; pPtr++)
		{
			*pPtr = '\0';
		}
	}

	/* enable */
	if ( strnicmp( buffer, "enable", sizeof("enable")-1 ) == 0 )
	{
		_stBrIfIsoEnable = 1;
		printk( "port isolation is enabled\n" );
		return count;
	}

	/* disable */
	if ( strnicmp( buffer, "disable", sizeof("disable")-1 ) == 0 )
	{
		_stBrIfIsoEnable = 0;
		printk( "port isolation is disabled\n" );
		return count;
	}

	/* debugOn */
	if ( strnicmp( buffer, "debugOn", sizeof("debugOn")-1 ) == 0 )
	{
		_stBrIfIsoDebug = 1;
		printk( "port isolation debugging is enabled\n" );
		return count;
	}

	/* disable */
	if ( strnicmp( buffer, "debugOff", sizeof("debugOff")-1 ) == 0 )
	{
		_stBrIfIsoDebug = 0;
		printk( "port isolation debugging is disabled\n" );
		return count;
	}

	/* add */
	if ( strnicmp( buffer, "add", sizeof("add")-1 ) == 0 )
	{
		int	 id;
		char sIfname[2][IFNAMSIZ];
		if ((id=br_if_iso_get_free_entry()) < 0) {
			printk( "no free entry\n" );
			return count;
		}
		pPtr = (char*)buffer + sizeof("add");
		/* <ifname1> */
		if (arc_strcpy_nospace( sIfname[0], pPtr, &pPtr ) < 0)
		{
			printk( "<ifname1> error\n" );
			return count;
		}
		/* <ifname2> */
		if (arc_strcpy_nospace( sIfname[1], pPtr, &pPtr ) < 0)
		{
			printk( "<ifname2> error\n" );
			return count;
		}
		/**/
		if (strcmp(sIfname[0],sIfname[1]) == 0)
		{
			printk( "<ifname1> <ifname2> are identical.\n" );
			return count;
		}
		/**/
		if (br_if_iso_entry_id(sIfname[0],sIfname[1]) >= 0)
		{
			printk( "entry existed.\n" );
			return count;
		}
		/**/
		BR_FILTER_ENTRY_OCCUPY( id );
		strcpy( _stBrIfIso[id].sIfname[0],sIfname[0] );
		strcpy( _stBrIfIso[id].sIfname[1],sIfname[1] );
		printk( "new entry is added\n" );
		return count;
	}

	/* del */
	if ( strnicmp( buffer, "del", sizeof("del")-1 ) == 0 )
	{
		int	 id;
		char sIfname[2][IFNAMSIZ];
		pPtr = (char*)buffer + sizeof("add");
		/* <ifname1> */
		if (arc_strcpy_nospace( sIfname[0], pPtr, &pPtr ) < 0)
		{
			printk( "<ifname1> error\n" );
			return count;
		}
		/* <ifname2> */
		if (arc_strcpy_nospace( sIfname[1], pPtr, &pPtr ) < 0)
		{
			printk( "<ifname2> error\n" );
			return count;
		}
		/**/
		if ((id=br_if_iso_entry_id(sIfname[0],sIfname[1])) < 0)
		{
			printk( "entry non-existed.\n" );
			return count;
		}
		/**/
		BR_FILTER_ENTRY_FREE( id );
		printk( "entry is deleted\n" );
		return count;
	}

	br_if_iso_proc_write_help();

	return count; /* return how many chars we have consumed, or we will receive them again... */
}

/*******************************************************************************
 * Description
 *		initialize bridge filter /proc/net/br virtual file.
 *
 ******************************************************************************/
static int br_filter_proc_init(void)
{
	struct proc_dir_entry*	proc_file_conf;

	/* make conf file */
	proc_file_conf = create_proc_entry("net/br", 0666, NULL);
	if (proc_file_conf == NULL)
	{
		printk( "error creating entry in br_filter_proc_init\n" );
		return -1;
	}

	/* set callback functions on file */
	br_if_iso_proc_init();
	proc_file_conf->write_proc = br_if_iso_proc_write;
	proc_file_conf->read_proc  = br_if_iso_proc_read;

	return 0;
}

/*******************************************************************************
 * Description
 *		cleanup bridge filter /proc/net/br virtual file.
 *
 ******************************************************************************/
static int br_filter_proc_cleanup(void)
{
    remove_proc_entry ("net/br", NULL);

	return 0;
}

#endif /* CONFIG_ARC_BR_PORT_ISOLATION */

static int __init br_init(void)
{
	int err;

  #ifdef CONFIG_ARC_BR_PORT_ISOLATION /*ctc*/
	br_filter_proc_init();
  #endif

	err = stp_proto_register(&br_stp_proto);
	if (err < 0) {
		printk(KERN_ERR "bridge: can't register sap for STP\n");
		return err;
	}

	err = br_fdb_init();
	if (err)
		goto err_out;

	err = register_pernet_subsys(&br_net_ops);
	if (err)
		goto err_out1;

	err = br_netfilter_init();
	if (err)
		goto err_out2;

	err = register_netdevice_notifier(&br_device_notifier);
	if (err)
		goto err_out3;

	err = br_netlink_init();
	if (err)
		goto err_out4;

	brioctl_set(br_ioctl_deviceless_stub);
	br_handle_frame_hook = br_handle_frame;

#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
	br_fdb_test_addr_hook = br_fdb_test_addr;
#endif

#ifdef CONFIG_IFX_IGMP_SNOOPING
	br_mcast_snoop_init();
#endif

	return 0;
err_out4:
	unregister_netdevice_notifier(&br_device_notifier);
err_out3:
	br_netfilter_fini();
err_out2:
	unregister_pernet_subsys(&br_net_ops);
err_out1:
	br_fdb_fini();
err_out:
	stp_proto_unregister(&br_stp_proto);
	return err;
}

static void __exit br_deinit(void)
{
	stp_proto_unregister(&br_stp_proto);

	br_netlink_fini();
	unregister_netdevice_notifier(&br_device_notifier);
	brioctl_set(NULL);

	unregister_pernet_subsys(&br_net_ops);

	rcu_barrier(); /* Wait for completion of call_rcu()'s */

	br_netfilter_fini();
#if defined(CONFIG_ATM_LANE) || defined(CONFIG_ATM_LANE_MODULE)
	br_fdb_test_addr_hook = NULL;
#endif

	br_handle_frame_hook = NULL;
	br_fdb_fini();

#ifdef CONFIG_IFX_IGMP_SNOOPING
	br_mcast_snoop_deinit();
#endif

  #ifdef CONFIG_ARC_BR_PORT_ISOLATION /*ctc*/
	br_filter_proc_cleanup();
  #endif
}

EXPORT_SYMBOL(br_should_route_hook);

module_init(br_init)
module_exit(br_deinit)
MODULE_LICENSE("GPL");
MODULE_VERSION(BR_VERSION);
