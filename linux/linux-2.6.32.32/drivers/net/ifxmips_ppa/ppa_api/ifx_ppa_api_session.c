/*******************************************************************************
**
** FILE NAME    : ifx_ppa_api_session.c
** PROJECT      : PPA
** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
**
** DATE         : 4 NOV 2008
** AUTHOR       : Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Session Operation Functions
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 04 NOV 2008  Xu Liang        Initiate Version
*******************************************************************************/



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/autoconf.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/version.h>
//#include <linux/types.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#if defined(CONFIG_IFX_PPA_API_PROC)
//#include <linux/proc_fs.h>
//#endif
//#include <linux/netdevice.h>
//#include <linux/in.h>
//#include <net/sock.h>
//#include <net/ip_vs.h>
//#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include <net/ifx_ppa_api.h>
#include <net/ifx_ppa_ppe_hal.h>
#include "ifx_ppa_api_misc.h"
#include "ifx_ppa_api_netif.h"
#include "ifx_ppa_api_session.h"
#include "ifx_ppe_drv_wrapper.h"
#if defined(CONFIG_IFX_PMCU) || defined(CONFIG_IFX_PMCU_MODULE)
#include "ifx_ppa_api_pwm.h"
#endif


/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 *  hash calculation
 */

#define SESSION_LIST_HASH_SHIFT                 8
#define SESSION_LIST_HASH_BIT_LENGTH            9
#define SESSION_LIST_HASH_MASK                  ((1 << SESSION_LIST_HASH_BIT_LENGTH) - 1)
#define SESSION_LIST_HASH_TABLE_SIZE            (1 << (SESSION_LIST_HASH_BIT_LENGTH + 1))
#define SESSION_LIST_HASH_VALUE(x, is_reply)    (((((uint32_t)(x) >> SESSION_LIST_HASH_SHIFT) & SESSION_LIST_HASH_MASK) << 1) | ((is_reply) ? 1 : 0))

#define SESSION_LIST_MC_HASH_SHIFT              0
#define SESSION_LIST_MC_HASH_BIT_LENGTH         6
#define SESSION_LIST_MC_HASH_MASK               ((1 << SESSION_LIST_MC_HASH_BIT_LENGTH) - 1)
#define SESSION_LIST_MC_HASH_TABLE_SIZE         (1 << SESSION_LIST_MC_HASH_BIT_LENGTH)
#define SESSION_LIST_MC_HASH_VALUE(x)           (((uint32_t)(x) >> SESSION_LIST_MC_HASH_SHIFT) & SESSION_LIST_MC_HASH_MASK)

#define BRIDGING_SESSION_LIST_HASH_BIT_LENGTH   8
#define BRIDGING_SESSION_LIST_HASH_MASK         ((1 << BRIDGING_SESSION_LIST_HASH_BIT_LENGTH) - 1)
#define BRIDGING_SESSION_LIST_HASH_TABLE_SIZE   (1 << BRIDGING_SESSION_LIST_HASH_BIT_LENGTH)
#define BRIDGING_SESSION_LIST_HASH_VALUE(x)     ( ( ((uint32_t)((uint8_t *)(x))[4] << 8) | ((uint8_t *)(x))[5] ) & BRIDGING_SESSION_LIST_HASH_MASK )



/*
 * ####################################
 *              Data Type
 * ####################################
 */



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  implemented in PPA PPE Low Level Driver (Data Path)
 */


//  routing session list item operation
static INLINE void ppa_init_session_list_item(struct session_list_item *);
/*static INLINE */struct session_list_item *ppa_alloc_session_list_item(void);
/*static INLINE*/ void ppa_free_session_list_item(struct session_list_item *);
/*static INLINE*/ void ppa_insert_session_item(struct session_list_item *);
static INLINE void ppa_remove_session_item(struct session_list_item *);
static void ppa_free_session_list(void);

//  multicast routing group list item operation
void ppa_init_mc_group_list_item(struct mc_group_list_item *);
static INLINE struct mc_group_list_item *ppa_alloc_mc_group_list_item(void);
static INLINE void ppa_free_mc_group_list_item(struct mc_group_list_item *);
static INLINE void ppa_insert_mc_group_item(struct mc_group_list_item *);
static INLINE void ppa_remove_mc_group_item(struct mc_group_list_item *);
static void ppa_free_mc_group_list(void);

//  routing session timeout help function
static INLINE uint32_t ppa_get_default_session_timeout(void);
static void ppa_check_hit_stat(unsigned long);

//  bridging session list item operation
static INLINE void ppa_bridging_init_session_list_item(struct bridging_session_list_item *);
static INLINE struct bridging_session_list_item *ppa_bridging_alloc_session_list_item(void);
static INLINE void ppa_bridging_free_session_list_item(struct bridging_session_list_item *);
static INLINE void ppa_bridging_insert_session_item(struct bridging_session_list_item *);
static INLINE void ppa_bridging_remove_session_item(struct bridging_session_list_item *);
static void ppa_free_bridging_session_list(void);

//  bridging session timeout help function
static INLINE uint32_t ppa_bridging_get_default_session_timeout(void);
static void ppa_bridging_check_hit_stat(unsigned long);

//  help function for special function
static INLINE void ppa_remove_routing_sessions_on_netif(PPA_IFNAME *, uint32_t);
static INLINE void ppa_remove_mc_groups_on_netif(PPA_IFNAME *);
static INLINE void ppa_remove_bridging_sessions_on_netif(PPA_IFNAME *);

/*ctc*/
#ifdef CONFIG_ARC_PPA_FILTER
 #define	ARC_PPA_BUF_TYPE_UCAST	1
 #define	ARC_PPA_BUF_TYPE_MCAST	2
 #define	ARC_PPA_BUF_TYPE_BRIDGE	3
 typedef struct {
	uint8_t*	mac;
	PPA_NETIF*	rx_if;
 } ARC_PPA_BRIDGE;
 int arc_ppa_filter_match( void* pData, int iType );
#endif


/*
 * ####################################
 *           Global Variable
 * ####################################
 */

/*
 *  routing session table
 */
static PPA_LOCK                     g_session_list_lock;
static struct session_list_item    *g_session_list_hash_table[SESSION_LIST_HASH_TABLE_SIZE] = {0};
static uint32_t                     g_session_list_length = 0;
static PPA_MEM_CACHE               *g_session_item_cache = NULL;
static PPA_TIMER                    g_hit_stat_timer;
static uint32_t                     g_hit_polling_time = DEFAULT_HIT_POLLING_TIME;

/*
 *  multicast routing session table
 */
static PPA_LOCK                     g_mc_group_list_lock;
static struct mc_group_list_item   *g_mc_group_list_hash_table[SESSION_LIST_MC_HASH_TABLE_SIZE] = {0};
static uint32_t                     g_mc_group_list_length = 0;
static PPA_MEM_CACHE               *g_mc_group_item_cache = NULL;

/*
 *  bridging session table
 */
static PPA_LOCK                             g_bridging_session_list_lock;
static struct bridging_session_list_item   *g_bridging_session_list_hash_table[BRIDGING_SESSION_LIST_HASH_TABLE_SIZE] = {0};
static PPA_MEM_CACHE                       *g_bridging_session_item_cache = NULL;
static PPA_TIMER                            g_bridging_hit_stat_timer;
static uint32_t                             g_bridging_hit_polling_time = DEFAULT_BRIDGING_HIT_POLLING_TIME;



/*
 * ####################################
 *           Extern Variable
 * ####################################
 */



/*
 * ####################################
 *            Local Function
 * ####################################
 */

/*
 *  routing session list item operation
 */

static INLINE void ppa_init_session_list_item(struct session_list_item *p_item)
{
    ppa_memset(p_item, 0, sizeof(*p_item));
    p_item->mtu             = DEFAULT_MTU;
    p_item->routing_entry   = ~0;
    p_item->pppoe_entry     = ~0;
    p_item->mtu_entry       = ~0;
    p_item->src_mac_entry   = ~0;
    p_item->out_vlan_entry  = ~0;
    p_item->tunnel_idx      = ~0;
}

/*static INLINE */ struct session_list_item *ppa_alloc_session_list_item(void)
{
    struct session_list_item *p_item;

    p_item = ppa_mem_cache_alloc(g_session_item_cache);
    if ( p_item )
        ppa_init_session_list_item(p_item);

    return p_item;
}

/*static INLINE */void ppa_free_session_list_item(struct session_list_item *p_item)
{
    ppa_hw_del_session(p_item);
    ppa_mem_cache_free(p_item, g_session_item_cache);
}

/*static INLINE*/void ppa_insert_session_item(struct session_list_item *p_item)
{
    uint32_t idx;

    idx = SESSION_LIST_HASH_VALUE(p_item->session, p_item->flags & SESSION_IS_REPLY);
    ppa_lock_get(&g_session_list_lock);
    p_item->next = g_session_list_hash_table[idx];
    g_session_list_hash_table[idx] = p_item;
    g_session_list_length++;
    ppa_lock_release(&g_session_list_lock);
}

static INLINE void ppa_remove_session_item(struct session_list_item *p_item)
{
    uint32_t idx;
    struct session_list_item *p_prev, *p_cur;

    idx = SESSION_LIST_HASH_VALUE(p_item->session, p_item->flags & SESSION_IS_REPLY);
    p_prev = NULL;
    ppa_lock_get(&g_session_list_lock);
    p_cur = g_session_list_hash_table[idx];
    while ( p_cur && p_cur != p_item )
    {
        p_prev = p_cur;
        p_cur = p_cur->next;
    }
    if ( p_cur )
    {
        if ( !p_prev )
            g_session_list_hash_table[idx] = p_cur->next;
        else
            p_prev->next = p_cur->next;

        p_cur->next = NULL;

        g_session_list_length--;
    }
    ppa_lock_release(&g_session_list_lock);
}

static void ppa_free_session_list(void)
{
    struct session_list_item *p_item, *p_next;
    int i;

    ppa_lock_get(&g_session_list_lock);
    for ( i = 0; i < NUM_ENTITY(g_session_list_hash_table); i++ )
    {
        for ( p_item = g_session_list_hash_table[i]; p_item; p_item = p_next )
        {
            p_next = p_item->next;

            ppa_free_session_list_item(p_item);
        }
        g_session_list_hash_table[i] = NULL;
    }
    ppa_lock_release(&g_session_list_lock);
}

/*
 *  multicast routing group list item operation
 */

void ppa_init_mc_group_list_item(struct mc_group_list_item *p_item)
{
    ppa_memset(p_item, 0, sizeof(*p_item));
    p_item->mc_entry        = ~0;
    p_item->src_mac_entry   = ~0;
    p_item->out_vlan_entry  = ~0;
}

static INLINE struct mc_group_list_item *ppa_alloc_mc_group_list_item(void)
{
    struct mc_group_list_item *p_item;

    p_item = ppa_mem_cache_alloc(g_mc_group_item_cache);
    if ( p_item )
        ppa_init_mc_group_list_item(p_item);

    return p_item;
}

static INLINE void ppa_free_mc_group_list_item(struct mc_group_list_item *p_item)
{
    ppa_hw_del_mc_group(p_item);
    ppa_mem_cache_free(p_item, g_mc_group_item_cache);
}

static INLINE void ppa_insert_mc_group_item(struct mc_group_list_item *p_item)
{
    uint32_t idx;

    idx = SESSION_LIST_MC_HASH_VALUE(p_item->ip_mc_group.ip.ip);
    ppa_lock_get(&g_mc_group_list_lock);
    p_item->next = g_mc_group_list_hash_table[idx];
    g_mc_group_list_hash_table[idx] = p_item;
    g_mc_group_list_length++;
    ppa_lock_release(&g_mc_group_list_lock);
}

static INLINE void ppa_remove_mc_group_item(struct mc_group_list_item *p_item)
{
    uint32_t idx;
    struct mc_group_list_item *p_prev, *p_cur;

    idx = SESSION_LIST_MC_HASH_VALUE(p_item->ip_mc_group.ip.ip);
    p_prev = NULL;
    ppa_lock_get(&g_mc_group_list_lock);
    p_cur = g_mc_group_list_hash_table[idx];
    while ( p_cur && p_cur != p_item )
    {
        p_prev = p_cur;
        p_cur = p_cur->next;
    }
    if ( p_cur )
    {
        if ( !p_prev )
            g_mc_group_list_hash_table[idx] = p_cur->next;
        else
            p_prev->next = p_cur->next;

        p_cur->next = NULL;

        g_mc_group_list_length--;
    }
    ppa_lock_release(&g_mc_group_list_lock);
}

static void ppa_free_mc_group_list(void)
{
    struct mc_group_list_item *p_mc_item, *p_mc_next;
    int i;

    ppa_lock_get(&g_mc_group_list_lock);
    for ( i = 0; i < NUM_ENTITY(g_mc_group_list_hash_table); i++ )
    {
        for ( p_mc_item = g_mc_group_list_hash_table[i]; p_mc_item; p_mc_item = p_mc_next )
        {
            p_mc_next = p_mc_item->next;

            ppa_free_mc_group_list_item(p_mc_item);
        }
        g_mc_group_list_hash_table[i] = NULL;
    }
    ppa_lock_release(&g_mc_group_list_lock);
}

/*
 *  routing session timeout help function
 */

static INLINE uint32_t ppa_get_default_session_timeout(void)
{
    return DEFAULT_TIMEOUT_IN_SEC;
}


static void ppa_check_hit_stat(unsigned long dummy)
{
    struct session_list_item *p_item;
    uint32_t i;
    PPE_ROUTING_INFO route={0};

    ppa_lock_get(&g_session_list_lock);
    for ( i = 0; i < NUM_ENTITY(g_session_list_hash_table); i++ )
    {
        for ( p_item = g_session_list_hash_table[i]; p_item; p_item = p_item->next )
        {
            route.entry = p_item->routing_entry;
            ifx_ppa_drv_test_and_clear_hit_stat( &route, 0);
            if ( route.f_hit )   {
                p_item->last_hit_time = ppa_get_time_in_sec();                
                ifx_ppa_drv_get_routing_entry_bytes(&route, 0);
                
                if( route.bytes >= p_item->last_bytes) 
                    p_item->acc_bytes += route.bytes - p_item->last_bytes;
                else
                    p_item->acc_bytes += route.bytes + 0xFFFFFFFF - p_item->last_bytes;
                p_item->last_bytes = route.bytes;
            }
        }
    }
    ppa_lock_release(&g_session_list_lock);

    ppa_timer_add(&g_hit_stat_timer, g_hit_polling_time);
    //printk("jiffy=%lu, HZ=%lu, poll_timer=%lu\n", jiffies, HZ, ppa_api_get_session_poll_timer());
}

/*
 *  bridging session list item operation
 */

static INLINE void ppa_bridging_init_session_list_item(struct bridging_session_list_item *p_item)
{
    ppa_memset(p_item, 0, sizeof(*p_item));
    p_item->bridging_entry = ~0;
}

static INLINE struct bridging_session_list_item *ppa_bridging_alloc_session_list_item(void)
{
    struct bridging_session_list_item *p_item;

    p_item = ppa_mem_cache_alloc(g_bridging_session_item_cache);
    if ( p_item )
        ppa_bridging_init_session_list_item(p_item);

    return p_item;
}

static INLINE void ppa_bridging_free_session_list_item(struct bridging_session_list_item *p_item)
{
    ppa_bridging_hw_del_session(p_item);
    ppa_mem_cache_free(p_item, g_bridging_session_item_cache);
}

static INLINE void ppa_bridging_insert_session_item(struct bridging_session_list_item *p_item)
{
    uint32_t idx;

    idx = BRIDGING_SESSION_LIST_HASH_VALUE(p_item->mac);
    ppa_lock_get(&g_bridging_session_list_lock);
    p_item->next = g_bridging_session_list_hash_table[idx];
    g_bridging_session_list_hash_table[idx] = p_item;
    ppa_lock_release(&g_bridging_session_list_lock);
}

static INLINE void ppa_bridging_remove_session_item(struct bridging_session_list_item *p_item)
{
    uint32_t idx;
    struct bridging_session_list_item *p_prev, *p_cur;

    idx = BRIDGING_SESSION_LIST_HASH_VALUE(p_item->mac);
    p_prev = NULL;
    ppa_lock_get(&g_bridging_session_list_lock);
    p_cur = g_bridging_session_list_hash_table[idx];
    while ( p_cur && p_cur != p_item )
    {
        p_prev = p_cur;
        p_cur = p_cur->next;
    }
    if ( p_cur )
    {
        if ( !p_prev )
            g_bridging_session_list_hash_table[idx] = p_cur->next;
        else
            p_prev->next = p_cur->next;

        p_cur->next = NULL;
    }
    ppa_lock_release(&g_bridging_session_list_lock);
}

static void ppa_free_bridging_session_list(void)
{
    struct bridging_session_list_item *p_brg_item, *p_brg_next;
    int i;

    ppa_lock_get(&g_bridging_session_list_lock);
    for ( i = 0; i < NUM_ENTITY(g_bridging_session_list_hash_table); i++ )
    {
        for ( p_brg_item = g_bridging_session_list_hash_table[i]; p_brg_item; p_brg_item = p_brg_next )
        {
            p_brg_next = p_brg_item->next;

            ppa_bridging_free_session_list_item(p_brg_item);
        }
        g_bridging_session_list_hash_table[i] = NULL;
    }
    ppa_lock_release(&g_bridging_session_list_lock);
}

/*
 *  bridging session timeout help function
 */

static INLINE uint32_t ppa_bridging_get_default_session_timeout(void)
{
    return DEFAULT_BRIDGING_TIMEOUT_IN_SEC;
}

static void ppa_bridging_check_hit_stat(unsigned long dummy)
{
    struct bridging_session_list_item *p_item;
    uint32_t i;
    PPE_BR_MAC_INFO br_mac={0};

    ppa_lock_get(&g_bridging_session_list_lock);
    for ( i = 0; i < NUM_ENTITY(g_bridging_session_list_hash_table); i++ )
    {
        for ( p_item = g_bridging_session_list_hash_table[i]; p_item; p_item = p_item->next )
        {
            br_mac.p_entry = p_item->bridging_entry;
            if ( !(p_item->flags & SESSION_STATIC) )
            {
                ifx_ppa_drv_test_and_clear_bridging_hit_stat( &br_mac, 0);
                if( br_mac.f_hit ) 
                    p_item->last_hit_time = ppa_get_time_in_sec();
            }
        }
    }
    ppa_lock_release(&g_bridging_session_list_lock);

    ppa_timer_add(&g_bridging_hit_stat_timer, g_bridging_hit_polling_time);
}

/*
 *  help function for special function
 */

static INLINE void ppa_remove_routing_sessions_on_netif(PPA_IFNAME *ifname, uint32_t lan_wan_flag)
{
    uint32_t idx;
    struct session_list_item *p_prev, *p_cur;
    struct session_list_item *p_list = NULL, *p_to_del;

    if ( lan_wan_flag == 3 )
    {
        ppa_lock_get(&g_session_list_lock);
        for ( idx = 0; idx < NUM_ENTITY(g_session_list_hash_table); idx++ )
        {
            p_prev = NULL;
            p_cur = g_session_list_hash_table[idx];
            while ( p_cur )
            {
                if ( ppa_is_netif_name(p_cur->rx_if, ifname) || ppa_is_netif_name(p_cur->tx_if, ifname) )
                {
                    p_to_del = p_cur;
                    p_cur = p_cur->next;
                    if ( !p_prev )
                        g_session_list_hash_table[idx] = p_cur;
                    else
                        p_prev->next = p_cur;
                    g_session_list_length--;
                    p_to_del->next = p_list;
                    p_list = p_to_del;
                }
                else
                {
                    p_prev = p_cur;
                    p_cur = p_cur->next;
                }
            }
            ppa_lock_release(&g_session_list_lock);
        }
    }
    else
    {
        uint32_t rx_netif_flag, tx_netif_flag;

        rx_netif_flag = lan_wan_flag == 1 ? SESSION_LAN_ENTRY : SESSION_WAN_ENTRY;
        tx_netif_flag = lan_wan_flag == 1 ? SESSION_WAN_ENTRY : SESSION_LAN_ENTRY;

        ppa_lock_get(&g_session_list_lock);
        for ( idx = 0; idx < NUM_ENTITY(g_session_list_hash_table); idx++ )
        {
            p_prev = NULL;
            p_cur = g_session_list_hash_table[idx];
            while ( p_cur )
            {
                if ( (ppa_is_netif_name(p_cur->rx_if, ifname) && (p_cur->flags & rx_netif_flag))
                    || (ppa_is_netif_name(p_cur->tx_if, ifname) && (p_cur->flags & tx_netif_flag)) )
                {
                    p_to_del = p_cur;
                    p_cur = p_cur->next;
                    if ( !p_prev )
                        g_session_list_hash_table[idx] = p_cur;
                    else
                        p_prev->next = p_cur;
                    g_session_list_length--;
                    p_to_del->next = p_list;
                    p_list = p_to_del;
                }
                else
                {
                    p_prev = p_cur;
                    p_cur = p_cur->next;
                }
            }
            ppa_lock_release(&g_session_list_lock);
        }
    }

    for ( p_to_del = p_list; p_to_del; p_to_del = p_list )
    {
        p_list = p_list->next;
        ppa_free_session_list_item(p_to_del);
    }
}

static INLINE void ppa_remove_mc_groups_on_netif(PPA_IFNAME *ifname)
{
    uint32_t idx;
    struct mc_group_list_item *p_prev, *p_cur;
    struct mc_group_list_item *p_list = NULL, *p_to_del;
    int i, j;

    ppa_lock_get(&g_mc_group_list_lock);
    for ( idx = 0; idx < NUM_ENTITY(g_mc_group_list_hash_table); idx++ )
    {
        p_prev = NULL;
        p_cur = g_mc_group_list_hash_table[idx];
        while ( p_cur )
        {
            for ( i = 0; i < p_cur->num_ifs; i++ )
                if ( ppa_is_netif_name(p_cur->netif[i], ifname) )
                    p_cur->netif[i] = NULL;
            for ( i = j = 0; i < p_cur->num_ifs; i++ )
                if ( p_cur->netif[i] != NULL )
                {
                    if ( i != j )
                    {
                        p_cur->netif[j] = p_cur->netif[i];
                        p_cur->ttl[j] = p_cur->ttl[i];
                    }
                    j++;
                }

            if ( j != 0 )
            {
                p_cur->num_ifs = j;
                p_prev = p_cur;
                p_cur = p_cur->next;
            }
            else
            {
                p_to_del = p_cur;
                p_cur = p_cur->next;
                if ( !p_prev )
                    g_mc_group_list_hash_table[idx] = p_cur;
                else
                    p_prev->next = p_cur;
                g_mc_group_list_length--;
                p_to_del->next = p_list;
                p_list = p_to_del;
            }
        }
    }
    ppa_lock_release(&g_mc_group_list_lock);

    for ( p_to_del = p_list; p_to_del; p_to_del = p_list )
    {
        p_list = p_list->next;
        ppa_free_mc_group_list_item(p_to_del);
    }
}

static INLINE void ppa_remove_bridging_sessions_on_netif(PPA_IFNAME *ifname)
{
    uint32_t idx;
    struct bridging_session_list_item *p_prev, *p_cur;
    struct bridging_session_list_item *p_list = NULL, *p_to_del;

    ppa_lock_get(&g_bridging_session_list_lock);
    for ( idx = 0; idx < NUM_ENTITY(g_bridging_session_list_hash_table); idx++ )
    {
        p_prev = NULL;
        p_cur = g_bridging_session_list_hash_table[idx];
        while ( p_cur )
        {
            if ( ppa_is_netif_name(p_cur->netif, ifname) )
            {
                p_to_del = p_cur;
                p_cur = p_cur->next;
                if ( !p_prev )
                    g_bridging_session_list_hash_table[idx] = p_cur;
                else
                    p_prev->next = p_cur;
                p_to_del->next = p_list;
                p_list = p_to_del;
            }
            else
            {
                p_prev = p_cur;
                p_cur = p_cur->next;
            }
        }
    }
    ppa_lock_release(&g_bridging_session_list_lock);

    for ( p_to_del = p_list; p_to_del; p_to_del = p_list )
    {
        p_list = p_list->next;
        ppa_bridging_free_session_list_item(p_to_del);
    }
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

/*
 *  routing session operation
 */

int32_t ppa_lookup_session(PPA_SESSION *p_session, uint32_t is_reply, struct session_list_item **pp_item)
{
    int32_t ret;
    struct session_list_item *p_prev, *p_cur;
    struct session_list_item **pp_header;

    ASSERT(pp_item != NULL, "pp_item == NULL");

    ret = IFX_PPA_SESSION_NOT_ADDED;

    pp_header = g_session_list_hash_table + SESSION_LIST_HASH_VALUE(p_session, is_reply);
    p_prev = NULL;
    ppa_lock_get(&g_session_list_lock);
    p_cur = *pp_header;
    while ( p_cur && !ppa_is_session_equal(p_session, p_cur->session) )
    {
        p_prev = p_cur;
        p_cur = p_cur->next;
    }
    if ( p_cur )
    {
        if ( p_prev )
        {
            p_prev->next = p_cur->next;
            p_cur->next = *pp_header;
            *pp_header = p_cur;
        }

        ret = IFX_PPA_SESSION_EXISTS;
    }
    ppa_lock_release(&g_session_list_lock);

    *pp_item = p_cur;

    return ret;
}

int32_t ppa_add_session(PPA_BUF *ppa_buf, PPA_SESSION *p_session, struct session_list_item **pp_item, uint32_t flags)
{
    struct session_list_item *p_item;

  /*ctc*/
  #ifdef CONFIG_ARC_PPA_FILTER
	if ( arc_ppa_filter_match( ppa_buf, ARC_PPA_BUF_TYPE_UCAST ) >= 0 )
		return IFX_ENOTPOSSIBLE;
  #endif

    if ( !p_session )
    {
        p_session = ppa_get_session(ppa_buf);
        if ( !p_session )
            return -1;
    }

    p_item = ppa_alloc_session_list_item();
    if ( !p_item )
    {
        err("failed in memory allocation");
        return IFX_ENOMEM;
    }

    dump_list_item(p_item, "ppa_add_session (after init)");

    dbg("ppa_get_session(ppa_buf) = %08X", (uint32_t)ppa_get_session(ppa_buf));

    p_item->session       = p_session;

    if ( (flags & PPA_F_SESSION_REPLY_DIR) )
        p_item->flags    |= SESSION_IS_REPLY;

    p_item->ip_proto      = ppa_get_pkt_ip_proto(ppa_buf);
    p_item->ip_tos        = ppa_get_pkt_ip_tos(ppa_buf);
    p_item->src_ip        = ppa_get_pkt_src_ip(ppa_buf);
    p_item->src_port      = ppa_get_pkt_src_port(ppa_buf);
    p_item->dst_ip        = ppa_get_pkt_dst_ip(ppa_buf);
    p_item->dst_port      = ppa_get_pkt_dst_port(ppa_buf);
    p_item->rx_if         = ppa_get_pkt_src_if(ppa_buf);
    p_item->timeout       = ppa_get_default_session_timeout();
    p_item->last_hit_time = ppa_get_time_in_sec();

    if(ppa_is_pkt_ipv6(ppa_buf)){
        p_item->flags |= SESSION_IS_IPV6;
    }
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
    p_item->priority      = ppa_get_pkt_priority(ppa_buf);
#endif
    ppa_get_pkt_rx_src_mac_addr(ppa_buf, p_item->src_mac);

    ppa_insert_session_item(p_item);

    dump_list_item(p_item, "ppa_add_session (after setting)");

    *pp_item = p_item;

    return 0;
}

int32_t ppa_update_session(PPA_BUF *ppa_buf, struct session_list_item *p_item, uint32_t flags)
{
    int32_t ret = IFX_SUCCESS;
    PPA_NETIF *netif;
    PPA_IPADDR ip;
    uint32_t port;
    uint32_t dscp;
    struct netif_info *rx_ifinfo, *tx_ifinfo;
    //uint32_t vlan_tag;
    int f_is_ipoa_or_pppoa = 0;
    int qid;

  /*ctc*/
  #ifdef CONFIG_ARC_PPA_FILTER
	if ( arc_ppa_filter_match( ppa_buf, ARC_PPA_BUF_TYPE_UCAST ) >= 0 )
		return IFX_ENOTPOSSIBLE;
  #endif

    p_item->tx_if = ppa_get_pkt_dst_if(ppa_buf);

    /*
     *  update and get rx/tx information
     */

    if ( ppa_netif_update(p_item->rx_if, NULL) != IFX_SUCCESS )
    {
        dbg("failed in collecting info of rx_if (%s)", ppa_get_netif_name(p_item->rx_if));
        SET_DBG_FLAG(p_item, SESSION_DBG_RX_IF_UPDATE_FAIL);
        return IFX_EAGAIN;
    }

    if ( ppa_netif_update(p_item->tx_if, NULL) != IFX_SUCCESS )
    {
        dbg("failed in collecting info of tx_if (%s)", ppa_get_netif_name(p_item->tx_if));
        SET_DBG_FLAG(p_item, SESSION_DBG_TX_IF_UPDATE_FAIL);
        return IFX_EAGAIN;
    }

    if ( ppa_netif_lookup(ppa_get_netif_name(p_item->rx_if), &rx_ifinfo) != IFX_SUCCESS )
    {
        dbg("failed in getting info structure of rx_if (%s)", ppa_get_netif_name(p_item->rx_if));
        SET_DBG_FLAG(p_item, SESSION_DBG_RX_IF_NOT_IN_IF_LIST);
        return IFX_ENOTPOSSIBLE;
    }

    if ( ppa_netif_lookup(ppa_get_netif_name(p_item->tx_if), &tx_ifinfo) != IFX_SUCCESS )
    {
        dbg("failed in getting info structure of tx_if (%s)", ppa_get_netif_name(p_item->tx_if));
        SET_DBG_FLAG(p_item, SESSION_DBG_TX_IF_NOT_IN_IF_LIST);
        ppa_netif_put(rx_ifinfo);
        return IFX_ENOTPOSSIBLE;
    }

    /*
     *  PPPoE is highest level, collect PPPoE information
     */

    p_item->flags &= ~SESSION_VALID_PPPOE;

    if ( (rx_ifinfo->flags & (NETIF_WAN_IF | NETIF_PPPOE)) == (NETIF_WAN_IF | NETIF_PPPOE) )
    {
        //  src interface is WAN and PPPoE
        p_item->pppoe_session_id = rx_ifinfo->pppoe_session_id;
        p_item->flags |= SESSION_VALID_PPPOE;
        SET_DBG_FLAG(p_item, SESSION_DBG_RX_PPPOE);
    }

    //  if destination interface is PPPoE, it covers the previous setting
    if ( (tx_ifinfo->flags & (NETIF_WAN_IF | NETIF_PPPOE)) == (NETIF_WAN_IF | NETIF_PPPOE) )
    {
        ASSERT(!(p_item->flags & SESSION_VALID_PPPOE), "both interfaces are WAN PPPoE interface, not possible");
        p_item->pppoe_session_id = tx_ifinfo->pppoe_session_id;
        p_item->flags |= SESSION_VALID_PPPOE;
        SET_DBG_FLAG(p_item, SESSION_DBG_TX_PPPOE);
        //  adjust MTU to ensure ethernet frame size does not exceed 1518 (without VLAN)
        p_item->mtu = 1492;
    }

    /*
     *  detect bridge and get the real effective device under this bridge
     *  do not support VLAN interface created on bridge
     */

    if ( (rx_ifinfo->flags & (NETIF_BRIDGE | NETIF_PPPOE)) == NETIF_BRIDGE )
    //  can't handle PPPoE over bridge properly, because src mac info is corrupted
    {
        if ( !(rx_ifinfo->flags & NETIF_PHY_IF_GOT)
            || (netif = ppa_get_netif(rx_ifinfo->phys_netif_name)) == NULL )
        {
            dbg("failed in get underlying interface of PPPoE interface (RX)");
            ret = IFX_ENOTPOSSIBLE;
            goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
        }
        while ( (rx_ifinfo->flags & NETIF_BRIDGE) )
        {
            if ( (ret = ppa_get_br_dst_port_with_mac(netif, p_item->src_mac, &netif)) != IFX_SUCCESS )
            {
                SET_DBG_FLAG(p_item, SESSION_DBG_SRC_BRG_IF_NOT_IN_BRG_TBL);
                if ( ret != IFX_EAGAIN )
                    ret = IFX_FAILURE;
                goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
            }
            else
            {
                CLR_DBG_FLAG(p_item, SESSION_DBG_SRC_BRG_IF_NOT_IN_BRG_TBL);
            }

            if ( ppa_netif_update(netif, NULL) != IFX_SUCCESS )
            {
                dbg("failed in collecting info of dst_rx_if (%s)", ppa_get_netif_name(netif));
                SET_DBG_FLAG(p_item, SESSION_DBG_RX_IF_UPDATE_FAIL);
                ret = IFX_EAGAIN;
                goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
            }

            ppa_netif_put(rx_ifinfo);

            if ( ppa_netif_lookup(ppa_get_netif_name(netif), &rx_ifinfo) != IFX_SUCCESS )
            {
                dbg("failed in getting info structure of dst_rx_if (%s)", ppa_get_netif_name(netif));
                SET_DBG_FLAG(p_item, SESSION_DBG_SRC_IF_NOT_IN_IF_LIST);
                ppa_netif_put(tx_ifinfo);
                return IFX_ENOTPOSSIBLE;
            }
        }
    }

    if ( (tx_ifinfo->flags & NETIF_BRIDGE) )
    {
        if ( !(tx_ifinfo->flags & NETIF_PHY_IF_GOT)
            || (netif = ppa_get_netif(tx_ifinfo->phys_netif_name)) == NULL )
        {
            dbg("failed in get underlying interface of PPPoE interface (TX)");
            ret = IFX_ENOTPOSSIBLE;
            goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
        }
        while ( (tx_ifinfo->flags & NETIF_BRIDGE) )
        {
            if ( (ret = ppa_get_br_dst_port(netif, ppa_buf, &netif)) != IFX_SUCCESS )
            {
                SET_DBG_FLAG(p_item, SESSION_DBG_DST_BRG_IF_NOT_IN_BRG_TBL);
                if ( ret != IFX_EAGAIN )
                    ret = IFX_FAILURE;
                goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
            }
            else
            {
                CLR_DBG_FLAG(p_item, SESSION_DBG_DST_BRG_IF_NOT_IN_BRG_TBL);
            }

            if ( ppa_netif_update(netif, NULL) != IFX_SUCCESS )
            {
                dbg("failed in collecting info of dst_tx_if (%s)", ppa_get_netif_name(netif));
                SET_DBG_FLAG(p_item, SESSION_DBG_TX_IF_UPDATE_FAIL);
                ret = IFX_EAGAIN;
                goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
            }

            ppa_netif_put(tx_ifinfo);

            if ( ppa_netif_lookup(ppa_get_netif_name(netif), &tx_ifinfo) != IFX_SUCCESS )
            {
                dbg("failed in getting info structure of dst_tx_if (%s)", ppa_get_netif_name(netif));
                SET_DBG_FLAG(p_item, SESSION_DBG_DST_IF_NOT_IN_IF_LIST);
                ppa_netif_put(rx_ifinfo);
                return IFX_ENOTPOSSIBLE;
            }
        }
    }

    /*
     *  check whether physical port is determined or not
     */

    if ( !(tx_ifinfo->flags & NETIF_PHYS_PORT_GOT) )
    {
        ret = IFX_FAILURE;
        goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
    }

    /*
     *  decide which table to insert session, LAN side table or WAN side table
     */

    if ( (rx_ifinfo->flags & (NETIF_LAN_IF | NETIF_WAN_IF)) == (NETIF_LAN_IF | NETIF_WAN_IF) )
    {
        switch ( tx_ifinfo->flags & (NETIF_LAN_IF | NETIF_WAN_IF) )
        {
        case NETIF_LAN_IF: p_item->flags |= SESSION_WAN_ENTRY; break;
        case NETIF_WAN_IF: p_item->flags |= SESSION_LAN_ENTRY; break;
        default:
            ret = IFX_FAILURE;
            goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
        }
    }
    else
    {
        switch ( rx_ifinfo->flags & (NETIF_LAN_IF | NETIF_WAN_IF) )
        {
        case NETIF_LAN_IF: p_item->flags |= SESSION_LAN_ENTRY; break;
        case NETIF_WAN_IF: p_item->flags |= SESSION_WAN_ENTRY; break;
        default:
            ret = IFX_FAILURE;
            goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
        }
    }
    
    /*
     *  Check 6RD
     */
    
    if((p_item->flags & SESSION_LAN_ENTRY  && p_item->tx_if ->type == ARPHRD_SIT) || 
        (p_item->flags & SESSION_WAN_ENTRY  && p_item->rx_if ->type == ARPHRD_SIT)){
        p_item->flags |= SESSION_TUNNEL_6RD;
    }

    /*
     *  collect VLAN information (outer/inner)
     */

    //  do not support VLAN interface created on bridge

    if ( (rx_ifinfo->flags & NETIF_VLAN_CANT_SUPPORT) || (tx_ifinfo->flags & NETIF_VLAN_CANT_SUPPORT) )
    {
        dbg("physical interface has limited VLAN support");
        p_item->flags |= SESSION_CAN_NOT_ACCEL;
        goto PPA_UPDATE_SESSION_DONE_SHOTCUT;
    }

    if ( (rx_ifinfo->flags & NETIF_VLAN_OUTER) )
        p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
    if ( (tx_ifinfo->flags & NETIF_VLAN_OUTER) )
    {
        if( tx_ifinfo->out_vlan_netif == NULL )
        {
            p_item->out_vlan_tag = tx_ifinfo->outer_vid; //  ignore prio and cfi
        }
        else
        {
            p_item->out_vlan_tag = ( tx_ifinfo->outer_vid & PPA_VLAN_TAG_MASK ) | ppa_vlan_dev_get_egress_qos_mask(tx_ifinfo->out_vlan_netif, ppa_buf);
        }

        p_item->flags |= SESSION_VALID_OUT_VLAN_INS;
    }

    if ( (rx_ifinfo->flags & NETIF_VLAN_INNER) )
        p_item->flags |= SESSION_VALID_VLAN_RM;
    if ( (tx_ifinfo->flags & NETIF_VLAN_INNER) )
    {
        if( tx_ifinfo->in_vlan_netif == NULL )
        {
            p_item->new_vci = tx_ifinfo->inner_vid ;     //  ignore prio and cfi
        }
        else
        {
            p_item->new_vci = ( tx_ifinfo->inner_vid & PPA_VLAN_TAG_MASK ) | ppa_vlan_dev_get_egress_qos_mask(tx_ifinfo->in_vlan_netif, ppa_buf);
        }
        p_item->flags |= SESSION_VALID_VLAN_INS;
    }

    /*
     *  decide destination list
     *  if tx interface is based on DSL, determine which PVC it is (QID)
     */

    p_item->dest_ifid = tx_ifinfo->phys_port;
    if ( (tx_ifinfo->flags & NETIF_PHY_ATM) )
    {
        qid = ifx_ppa_drv_get_netif_qid_with_pkt(ppa_buf, tx_ifinfo->vcc, 1);
        if ( qid >= 0 )
            p_item->dslwan_qid = qid;
        else
            p_item->dslwan_qid = tx_ifinfo->dslwan_qid;
        p_item->flags |= SESSION_VALID_DSLWAN_QID;
        if ( (tx_ifinfo->flags & NETIF_EOA) )
        {
            SET_DBG_FLAG(p_item, SESSION_DBG_TX_BR2684_EOA);
        }
        else if ( (tx_ifinfo->flags & NETIF_IPOA) )
        {
            p_item->flags |= SESSION_TX_ITF_IPOA;
            SET_DBG_FLAG(p_item, SESSION_TX_ITF_IPOA);
            f_is_ipoa_or_pppoa = 1;
        }
        else if ( (tx_ifinfo->flags & NETIF_PPPOATM) )
        {
            p_item->flags |= SESSION_TX_ITF_PPPOA;
            SET_DBG_FLAG(p_item, SESSION_TX_ITF_PPPOA);
            f_is_ipoa_or_pppoa = 1;
        }
    }
    else
    {
        netif = ppa_get_netif(tx_ifinfo->phys_netif_name);
        
        if ( g_ppa_dbg_enable & DBG_ENABLE_MASK_PRI_TEST ) //for test qos queue only depends on tos last 4 bits value
        {
            ppa_set_pkt_priority( ppa_buf, ppa_get_pkt_ip_tos(ppa_buf) % 8 );            
        }
        
        qid = ifx_ppa_drv_get_netif_qid_with_pkt(ppa_buf, netif, 0);
        if ( qid >= 0 )
        {
            p_item->dslwan_qid = qid;
            p_item->flags |= SESSION_VALID_DSLWAN_QID;
        }
    }

    /*
     *  collect src IP/Port, dest IP/Port information
     */

    //  only port change with same IP not supported here, not really useful
    ip = ppa_get_pkt_src_ip(ppa_buf);
    if ( ppa_memcmp(&ip, &p_item->src_ip, ppa_get_pkt_ip_len(ppa_buf)) != 0 )
    {
        p_item->nat_ip = ip;
        p_item->flags |= SESSION_VALID_NAT_IP;

        port = ppa_get_pkt_src_port(ppa_buf);
        if ( port != p_item->src_port )
        {
            p_item->nat_port = port;
            p_item->flags |= SESSION_VALID_NAT_PORT | SESSION_VALID_NAT_SNAT;
        }
    }
    else
    {
        ip = ppa_get_pkt_dst_ip(ppa_buf);
        if ( ppa_memcmp(&ip, &p_item->dst_ip, ppa_get_pkt_ip_len(ppa_buf)) != 0 )
        {
            p_item->nat_ip = ip;
            p_item->flags |= SESSION_VALID_NAT_IP;

            port = ppa_get_pkt_dst_port(ppa_buf);
            if ( port != p_item->dst_port )
            {
                p_item->nat_port = port;
                p_item->flags |= SESSION_VALID_NAT_PORT;
            }
       }
    }

    /*
     *  calculate new DSCP value if necessary
     */

    dscp = ppa_get_pkt_ip_tos(ppa_buf);
    if ( dscp != p_item->ip_tos )
    {
        p_item->new_dscp = dscp >> 2;
        p_item->flags |= SESSION_VALID_NEW_DSCP;
    }

    /*
     *  IPoA/PPPoA does not have MAC address
     */

    if ( f_is_ipoa_or_pppoa )
        goto PPA_UPDATE_SESSION_DONE_SHOTCUT;

    /*
     *  get new dest MAC address for ETH, EoA
     */

    if ( ppa_get_dst_mac(ppa_buf, p_item->session, p_item->dst_mac) != IFX_SUCCESS )
    {
        dbg("session:%x can not get dst mac!", (u32)ppa_get_session(ppa_buf));
        SET_DBG_FLAG(p_item, SESSION_DBG_GET_DST_MAC_FAIL);
        ret = IFX_EAGAIN;
    }

PPA_UPDATE_SESSION_DONE_SHOTCUT:
    ppa_netif_put(rx_ifinfo);
    ppa_netif_put(tx_ifinfo);
    return ret;
}

int32_t ppa_update_session_extra(PPA_SESSION_EXTRA *p_extra, struct session_list_item *p_item, uint32_t flags)
{
    if ( (flags & PPA_F_SESSION_NEW_DSCP) )
    {
        if ( p_extra->dscp_remark )
        {
            p_item->flags |= SESSION_VALID_NEW_DSCP;
            p_item->new_dscp = p_extra->new_dscp;
        }
        else
            p_item->flags &= ~SESSION_VALID_NEW_DSCP;
    }

    if ( (flags & PPA_F_SESSION_VLAN) )
    {
        if ( p_extra->vlan_insert )
        {
            p_item->flags |= SESSION_VALID_VLAN_INS;
            p_item->new_vci = ((uint32_t)p_extra->vlan_prio << 13) | ((uint32_t)p_extra->vlan_cfi << 12) | p_extra->vlan_id;
        }
        else
        {
            p_item->flags &= ~SESSION_VALID_VLAN_INS;
            p_item->new_vci = 0;
        }

        if ( p_extra->vlan_remove )
            p_item->flags |= SESSION_VALID_VLAN_RM;
        else
            p_item->flags &= ~SESSION_VALID_VLAN_RM;
    }

    if ( (flags & PPA_F_MTU) )
    {
        p_item->flags |= SESSION_VALID_MTU;
        p_item->mtu = p_extra->mtu;
    }

    if ( (flags & PPA_F_SESSION_OUT_VLAN) )
    {
        if ( p_extra->out_vlan_insert )
        {
            p_item->flags |= SESSION_VALID_OUT_VLAN_INS;
            p_item->out_vlan_tag = p_extra->out_vlan_tag;
        }
        else
        {
            p_item->flags &= ~SESSION_VALID_OUT_VLAN_INS;
            p_item->out_vlan_tag = 0;
        }

        if ( p_extra->out_vlan_remove )
            p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
        else
            p_item->flags &= ~SESSION_VALID_OUT_VLAN_RM;
    }

    if ( (flags & PPA_F_ACCEL_MODE) ) //only for hook and ioctl, not for ppe fw and HAL 
    {
        if( p_extra->accel_enable )
            p_item->flags &= ~SESSION_CAN_NOT_ACCEL;
        else
            p_item->flags |= SESSION_CAN_NOT_ACCEL;
    }

    return IFX_SUCCESS;
}

void ppa_remove_session(struct session_list_item *p_item)
{
    ppa_remove_session_item(p_item);

    ppa_free_session_list_item(p_item);
}

void dump_list_item(struct session_list_item *p_item, char *comment)
{
#if defined(DEBUG_DUMP_LIST_ITEM) && DEBUG_DUMP_LIST_ITEM
	int8_t strbuf[64];
    if ( !(g_ppa_dbg_enable & DBG_ENABLE_MASK_DUMP_ROUTING_SESSION) )
        return;

    if ( comment )
        printk("dump_list_item - %s\n", comment);
    else
        printk("dump_list_item\n");
    printk("  next             = %08X\n", (uint32_t)p_item->next);
    printk("  session          = %08X\n", (uint32_t)p_item->session);
    printk("  ip_proto         = %08X\n", p_item->ip_proto);
    printk("  src_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->src_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
    printk("  src_port         = %d\n",   p_item->src_port);
    printk("  src_mac[6]       = %s\n",   ppa_get_pkt_mac_string(p_item->src_mac, strbuf));
    printk("  dst_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->dst_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
    printk("  dst_port         = %d\n",   p_item->dst_port);
    printk("  dst_mac[6]       = %s\n",   ppa_get_pkt_mac_string(p_item->dst_mac, strbuf));
    printk("  nat_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->nat_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
    printk("  nat_port         = %d\n",   p_item->nat_port);
    printk("  rx_if            = %08X\n", (uint32_t)p_item->rx_if);
    printk("  tx_if            = %08X\n", (uint32_t)p_item->tx_if);
    printk("  timeout          = %d\n",   p_item->timeout);
    printk("  last_hit_time    = %d\n",   p_item->last_hit_time);
    printk("  num_adds         = %d\n",   p_item->num_adds);
    printk("  pppoe_session_id = %d\n",   p_item->pppoe_session_id);
    printk("  new_dscp         = %d\n",   p_item->new_dscp);
    printk("  new_vci          = %08X\n",  p_item->new_vci);
    printk("  mtu              = %d\n",   p_item->mtu);
    printk("  flags            = %08X\n", p_item->flags);
    printk("  routing_entry    = %08X\n", p_item->routing_entry);
    printk("  pppoe_entry      = %08X\n", p_item->pppoe_entry);
    printk("  mtu_entry        = %08X\n", p_item->mtu_entry);
    printk("  src_mac_entry    = %08X\n", p_item->src_mac_entry);

#endif
}

int32_t ppa_session_start_iteration(uint32_t *ppos, struct session_list_item **pp_item)
{
    struct session_list_item *p = NULL;
    int idx;
    uint32_t l;

    l = *ppos + 1;

    ppa_lock_get(&g_session_list_lock);

    for ( idx = 0; l && idx < NUM_ENTITY(g_session_list_hash_table); idx++ )
    {
        for ( p = g_session_list_hash_table[idx]; p; p = p->next )
            if ( !--l )
                break;
    }

    if ( l == 0 && p )
    {
        ++*ppos;
        *pp_item = p;
        return IFX_SUCCESS;
    }
    else
    {
        *pp_item = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_session_iterate_next(uint32_t *ppos, struct session_list_item **pp_item)
{
    uint32_t idx;

    if ( *pp_item == NULL )
        return IFX_FAILURE;

    if ( (*pp_item)->next != NULL )
    {
        ++*ppos;
        *pp_item = (*pp_item)->next;
        return IFX_SUCCESS;
    }
    else
    {
        for ( idx = SESSION_LIST_HASH_VALUE((*pp_item)->session, (*pp_item)->flags & SESSION_IS_REPLY) + 1;
              idx < NUM_ENTITY(g_session_list_hash_table);
              idx++ )
            if ( g_session_list_hash_table[idx] != NULL )
            {
                ++*ppos;
                *pp_item = g_session_list_hash_table[idx];
                return IFX_SUCCESS;
            }
        *pp_item = NULL;
        return IFX_FAILURE;
    }
}

void ppa_session_stop_iteration(void)
{
    ppa_lock_release(&g_session_list_lock);
}

/*
 *  routing session hardware/firmware operation
 */

int32_t ppa_hw_add_session(struct session_list_item *p_item, uint32_t f_test)
{
    uint32_t dest_list = 0;
    PPE_ROUTING_INFO route={0};
    
    int ret = IFX_SUCCESS;

    //  Only add session in H/w when the called from the post-NAT hook
    ppa_memset( &route, 0, sizeof(route) );
    route.src_mac.mac_ix = ~0;
    route.pppoe_info.pppoe_ix = ~0;
    route.out_vlan_info.vlan_entry = ~0;
    route.mtu_info.mtu_ix = ~0;
    
    route.route_type = (p_item->flags & SESSION_VALID_NAT_IP) ? ((p_item->flags & SESSION_VALID_NAT_PORT) ? IFX_PPA_ROUTE_TYPE_NAPT : IFX_PPA_ROUTE_TYPE_NAT) : IFX_PPA_ROUTE_TYPE_IPV4;
    if ( (p_item->flags & SESSION_VALID_NAT_IP) )
    {
        route.new_ip.f_ipv6 = 0;
        memcpy( &route.new_ip.ip, &p_item->nat_ip, sizeof(route.new_ip.ip) ); //since only IPv4 support NAT, translate it to IPv4 format
    }
    if ( (p_item->flags & SESSION_VALID_NAT_PORT) )
        route.new_port = p_item->nat_port;

    if ( (p_item->flags & (SESSION_VALID_PPPOE | SESSION_LAN_ENTRY)) == (SESSION_VALID_PPPOE | SESSION_LAN_ENTRY) )
    {
        route.pppoe_info.pppoe_session_id = p_item->pppoe_session_id;
        if ( ifx_ppa_drv_add_pppoe_entry( &route.pppoe_info, 0) == IFX_SUCCESS )
            p_item->pppoe_entry = route.pppoe_info.pppoe_ix;
        else
        {
            dbg("add pppoe_entry error");
            SET_DBG_FLAG(p_item, SESSION_DBG_ADD_PPPOE_ENTRY_FAIL);
            goto SESSION_VALID_PPPOE_ERROR;
        }
    }

    route.mtu_info.mtu = p_item->mtu;
    if ( ifx_ppa_drv_add_mtu_entry(&route.mtu_info, 0) == IFX_SUCCESS )
    {
        p_item->mtu_entry = route.mtu_info.mtu_ix;
        p_item->flags |= SESSION_VALID_MTU;
    }
    else
    {
        SET_DBG_FLAG(p_item, SESSION_DBG_ADD_MTU_ENTRY_FAIL);
        goto MTU_ERROR;
    }

    if((p_item->flags & (SESSION_TUNNEL_6RD | SESSION_LAN_ENTRY)) == (SESSION_TUNNEL_6RD | SESSION_LAN_ENTRY)){
        dbg("add 6RD entry to FW, tx dev: %s", p_item->tx_if->name);
        route.tnnl_info.tunnel_type = SESSION_TUNNEL_6RD;
        route.tnnl_info.tx_dev = p_item->tx_if;
        if(ifx_ppa_drv_add_tunnel_entry(&route.tnnl_info, 0) == IFX_SUCCESS){
            p_item->tunnel_idx = route.tnnl_info.tunnel_idx;
        }else{
            dbg("add tunnel 6rd entry error");
            goto MTU_ERROR;
        }
    }
    if(p_item->flags & SESSION_TUNNEL_6RD){
        route.tnnl_info.tunnel_idx = p_item->tunnel_idx << 1 | 1;
    }
    if ( !(p_item->flags & SESSION_TX_ITF_IPOA_PPPOA_MASK) )
    {
        if( !f_test )
            ppa_get_netif_hwaddr(p_item->tx_if, route.src_mac.mac);
        else //for testing only: used for ioctl to add a fake routing accleration entry in PPE 
            ppa_memcpy(route.src_mac.mac, p_item->src_mac, sizeof(route.src_mac.mac) );
        if ( ifx_ppa_drv_add_mac_entry(&route.src_mac, 0) == IFX_SUCCESS )
        {
            p_item->src_mac_entry = route.src_mac.mac_ix;
            p_item->flags |= SESSION_VALID_NEW_SRC_MAC;
        }
        else
        {
            SET_DBG_FLAG(p_item, SESSION_DBG_ADD_MAC_ENTRY_FAIL);
            goto NEW_SRC_MAC_ERROR;
        }
    }

    if ( (p_item->flags & SESSION_VALID_OUT_VLAN_INS) )
    {
        route.out_vlan_info.vlan_id = p_item->out_vlan_tag;
        if ( ifx_ppa_drv_add_outer_vlan_entry( &route.out_vlan_info, 0 ) == IFX_SUCCESS )
        {            
            p_item->out_vlan_entry = route.out_vlan_info.vlan_entry;
        }
        else
        {
            dbg("add out_vlan_ix error");
            SET_DBG_FLAG(p_item, SESSION_DBG_ADD_OUT_VLAN_ENTRY_FAIL);
            goto OUT_VLAN_ERROR;
        }
    }

    if ( (p_item->flags & SESSION_VALID_NEW_DSCP) )
        route.new_dscp = p_item->new_dscp;

    route.dest_list = 1 << p_item->dest_ifid;
    dbg("dest_list = %02X", dest_list);

    route.f_is_lan = p_item->flags & SESSION_LAN_ENTRY;
    ppa_memcpy(route.new_dst_mac, p_item->dst_mac, PPA_ETH_ALEN);
    route.dst_port = p_item->dst_port;
    route.src_port = p_item->src_port;
    route.f_is_tcp = p_item->flags & SESSION_IS_TCP;
    route.f_new_dscp_enable = p_item->flags & SESSION_VALID_NEW_DSCP;
    route.f_vlan_ins_enable =p_item->flags & SESSION_VALID_VLAN_INS;
    route.new_vci = p_item->new_vci;
    route.f_vlan_rm_enable = p_item->flags & SESSION_VALID_VLAN_RM;
    route.pppoe_mode = p_item->flags & SESSION_VALID_PPPOE;
    route.f_out_vlan_ins_enable =p_item->flags & SESSION_VALID_OUT_VLAN_INS;    
    route.f_out_vlan_rm_enable = p_item->flags & SESSION_VALID_OUT_VLAN_RM;
    route.dslwan_qid = p_item->dslwan_qid;
    
#if defined(CONFIG_IFX_PPA_IPv6_ENABLE)     
    if( p_item->flags & SESSION_IS_IPV6 )
    {
        route.src_ip.f_ipv6 = 1;
        ppa_memcpy( route.src_ip.ip.ip6, p_item->src_ip.ip6, sizeof(route.src_ip.ip.ip6));

        route.dst_ip.f_ipv6 = 1;
        ppa_memcpy( route.dst_ip.ip.ip6, p_item->dst_ip.ip6, sizeof(route.dst_ip.ip.ip6)); 
    }else
#endif
    {
        route.src_ip.f_ipv6 = 0;
        route.src_ip.ip.ip= p_item->src_ip.ip;

        route.dst_ip.f_ipv6 = 0;
        route.dst_ip.ip.ip= p_item->dst_ip.ip; 

        route.new_ip.f_ipv6 = 0;
    }
     if ( (ret = ifx_ppa_drv_add_routing_entry( &route, 0) ) == IFX_SUCCESS )
    {
        p_item->routing_entry = route.entry;
        p_item->flags |= SESSION_ADDED_IN_HW;
        return IFX_SUCCESS;
    }
    

    //  fail in add_routing_entry
    dbg("fail in add_routing_entry");
    p_item->out_vlan_entry = ~0;
    ifx_ppa_drv_del_outer_vlan_entry( &route.out_vlan_info, 0);
OUT_VLAN_ERROR:
    p_item->src_mac_entry = ~0;
    ifx_ppa_drv_del_mac_entry( &route.src_mac, 0);
NEW_SRC_MAC_ERROR:
    p_item->mtu_entry = ~0;
    ifx_ppa_drv_del_mtu_entry(&route.mtu_info, 0);
MTU_ERROR:
    p_item->pppoe_entry = ~0;    
    ifx_ppa_drv_del_pppoe_entry( &route.pppoe_info, 0);
SESSION_VALID_PPPOE_ERROR:
    return ret;
}

int32_t ppa_hw_update_session_extra(struct session_list_item *p_item, uint32_t flags)
{    
    PPE_ROUTING_INFO route={0};

    route.mtu_info.mtu_ix = ~0;
    route.out_vlan_info.vlan_entry = ~0;

    if ( (flags & PPA_F_SESSION_NEW_DSCP) )
        route.update_flags |= IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_DSCP_EN | IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_DSCP;

    if ( (flags & PPA_F_SESSION_VLAN) )
        route.update_flags |= IFX_PPA_UPDATE_ROUTING_ENTRY_VLAN_INS_EN | IFX_PPA_UPDATE_ROUTING_ENTRY_NEW_VCI | IFX_PPA_UPDATE_ROUTING_ENTRY_VLAN_RM_EN;

    if ( (flags & PPA_F_MTU) )
    {
        route.mtu_info.mtu_ix = p_item->mtu_entry;
        if ( ifx_ppa_drv_get_mtu_entry( &route.mtu_info, 0) == IFX_SUCCESS )
        {
            if ( route.mtu_info.mtu == p_item->mtu )
            {
                //  entry not changed
                route.mtu_info.mtu_ix = p_item->mtu_entry;
                goto PPA_HW_UPDATE_SESSION_EXTRA_MTU_GOON;
            }
            else
            {
                //  entry changed, so delete old first and create new one later
                ifx_ppa_drv_del_mtu_entry( &route.mtu_info, 0);
                p_item->mtu_entry = ~0;
            }
        }
        //  create new MTU entry
        route.mtu_info.mtu = p_item->mtu;
        if ( ifx_ppa_drv_add_mtu_entry( &route.mtu_info, 0) == IFX_SUCCESS )
        {
            //  success
            p_item->mtu_entry = route.mtu_info.mtu_ix;
            route.update_flags |= IFX_PPA_UPDATE_ROUTING_ENTRY_MTU_IX;
        }
        else
            return IFX_EAGAIN;
    }
PPA_HW_UPDATE_SESSION_EXTRA_MTU_GOON:

    if ( (flags & PPA_F_SESSION_OUT_VLAN) )
    {
        route.out_vlan_info.vlan_entry = p_item->out_vlan_entry;
        if ( ifx_ppa_drv_get_outer_vlan_entry(&route.out_vlan_info, 0) == IFX_SUCCESS )
        {
            if ( route.out_vlan_info.vlan_id == p_item->out_vlan_tag )
            {
                //  entry not changed
                goto PPA_HW_UPDATE_SESSION_EXTRA_OUT_VLAN_GOON;
            }
            else
            {
                //  entry changed, so delete old first and create new one later                
                ifx_ppa_drv_del_outer_vlan_entry(&route.out_vlan_info, 0);
                p_item->out_vlan_entry = ~0;
            }
        }
        //  create new OUT VLAN entry
        route.out_vlan_info.vlan_id = p_item->out_vlan_tag;        
        if ( ifx_ppa_drv_add_outer_vlan_entry( &route.out_vlan_info, 0) == IFX_SUCCESS )
        {
            p_item->out_vlan_entry = route.out_vlan_info.vlan_entry;
            route.update_flags |= IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_IX;
        }
        else
            return IFX_EAGAIN;

        route.update_flags |= IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_INS_EN | IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_RM_EN;
    }
PPA_HW_UPDATE_SESSION_EXTRA_OUT_VLAN_GOON:

    route.entry = p_item->routing_entry;
    route.f_new_dscp_enable = p_item->flags & SESSION_VALID_NEW_DSCP;
    if ( (p_item->flags & SESSION_VALID_NEW_DSCP) )
        route.new_dscp = p_item->new_dscp;
    
    route.f_vlan_ins_enable =p_item->flags & SESSION_VALID_VLAN_INS;
    route.new_vci = p_item->new_vci;
    
    route.f_vlan_rm_enable = p_item->flags & SESSION_VALID_VLAN_RM;

    route.f_out_vlan_ins_enable =p_item->flags & SESSION_VALID_OUT_VLAN_INS;
    route.out_vlan_info.vlan_entry = p_item->out_vlan_entry,    
    
    route.f_out_vlan_rm_enable = p_item->flags & SESSION_VALID_OUT_VLAN_RM;
    
    
    ifx_ppa_drv_update_routing_entry(&route, 0);
    return IFX_SUCCESS;
}

void ppa_hw_del_session(struct session_list_item *p_item)
{
    PPE_ROUTING_INFO route={0};
    
    if ( (p_item->flags & SESSION_ADDED_IN_HW) )
    {
        //  when init, these entry values are ~0, the max the number which can be detected by these functions
        route.entry = p_item->routing_entry;
        ifx_ppa_drv_del_routing_entry(&route, 0);
        p_item->routing_entry = ~0;

        route.out_vlan_info.vlan_entry = p_item->out_vlan_entry;
        ifx_ppa_drv_del_outer_vlan_entry(&route.out_vlan_info, 0);
        p_item->out_vlan_entry = ~0;

        route.pppoe_info.pppoe_ix = p_item->pppoe_entry;
        ifx_ppa_drv_del_pppoe_entry(&route.pppoe_info, 0);
        p_item->pppoe_entry = ~0;

        route.mtu_info.mtu_ix = p_item->mtu_entry;
        ifx_ppa_drv_del_mtu_entry( &route.mtu_info, 0);
        p_item->mtu_entry = ~0;

        route.src_mac.mac_ix = p_item->src_mac_entry;
        ifx_ppa_drv_del_mac_entry( &route.src_mac, 0);
        p_item->src_mac_entry = ~0;

        route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
        if(p_item->flags & SESSION_TUNNEL_6RD){
            route.tnnl_info.tunnel_type = SESSION_TUNNEL_6RD;
        }
        ifx_ppa_drv_del_tunnel_entry(&route.tnnl_info, 0);
        p_item->flags &= ~SESSION_ADDED_IN_HW;
    }
}

/*
 *  multicast routing operation
 */

int32_t ppa_lookup_mc_group(IP_ADDR_C ip_mc_group, struct mc_group_list_item **pp_item)
{
    uint32_t ret;
    struct mc_group_list_item *p_prev, *p_cur;
    struct mc_group_list_item **pp_header;

    ASSERT(pp_item != NULL, "pp_item == NULL");

    ret = IFX_PPA_SESSION_NOT_ADDED;

    pp_header = g_mc_group_list_hash_table + SESSION_LIST_MC_HASH_VALUE(ip_mc_group.ip.ip);
    p_prev = NULL;
    ppa_lock_get(&g_mc_group_list_lock);
    p_cur = *pp_header;
    while ( p_cur )
    {
        if ( ip_mc_group.f_ipv6 == 0 ) 
        {   /*ipv4 address compare */
            if ( p_cur->ip_mc_group.ip.ip == ip_mc_group.ip.ip )
                break;
        }
        else
         { /*ipv6 compare */
           if ( ppa_memcmp( p_cur->ip_mc_group.ip.ip6, ip_mc_group.ip.ip6, sizeof(ip_mc_group.ip.ip6 )  ) == 0 )
            
                break;
         }

        p_prev = p_cur;
        p_cur = p_cur->next;
    }
    if ( p_cur )
    {
        if ( p_prev )
        {
            p_prev->next = p_cur->next;
            p_cur->next = *pp_header;
            *pp_header = p_cur;
        }

        ret = IFX_PPA_SESSION_EXISTS;
    }
    ppa_lock_release(&g_mc_group_list_lock);

    *pp_item = p_cur;

    return ret;
}

/*
    ppa_mc_group_checking: check whether it is valid acceleration session. the result value :
    1) if not found any valid downstream interface, includes num_ifs is zero: return IFX_FAILURE
    2) if downstream interface's VLAN tag not complete same: return IFX_FAILURE
    3) if p_item is NULL: return IFX_ENOMEM;
    
    
*/
int32_t ppa_mc_group_checking(PPA_MC_GROUP *p_mc_group, struct mc_group_list_item *p_item, uint32_t flags)
{
    struct netif_info *p_netif_info;
    uint32_t bit;
    uint32_t idx;
    uint32_t i, bfAccelerate=1, tmp_flag = 0, tmp_out_vlan_tag=0;
    uint16_t  tmp_new_vci=0, bfFirst = 1 ;
    
  /*ctc*/
  #ifdef CONFIG_ARC_PPA_FILTER
	if ( arc_ppa_filter_match( p_mc_group, ARC_PPA_BUF_TYPE_MCAST ) >= 0 )
		return IFX_ENOTPOSSIBLE;
  #endif
    if ( !p_item )
        return IFX_ENOMEM;

    //before updating p_item, need to clear some previous values, but cannot memset all p_item esp for p_item's next pointer.
    p_item->dest_ifid = 0;
    //p_item->flags = 0;  //don't simple clear original flag value, esp for flag "SESSION_ADDED_IN_HW"
    p_item->if_mask = 0;
    p_item->new_dscp = 0;
    
    p_item->bridging_flag = p_mc_group->bridging_flag;

    for ( i = 0, bit = 1, idx = 0; i < PPA_MAX_MC_IFS_NUM && idx < p_mc_group->num_ifs; i++, bit <<= 1 )
    {
        if ( p_mc_group->if_mask & bit)
        {
            if ( ppa_netif_lookup(p_mc_group->array_mem_ifs[i].ifname, &p_netif_info) == IFX_SUCCESS )
            {
                //  dest interface
                if ( ppa_netif_update(NULL, p_mc_group->array_mem_ifs[i].ifname) != IFX_SUCCESS
                    || !(p_netif_info->flags & NETIF_PHYS_PORT_GOT) )
                {
                    ppa_netif_put(p_netif_info);
                    ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "Warning: No PHYS found for interface %s", p_mc_group->array_mem_ifs[i].ifname);
                    bfAccelerate = 0;
                    break;
                }
                if ( bfFirst )
                {  /* keep the first interface's flag. Make sure all interface's vlan action should be same, otherwise PPE FW cannot do it */
                    tmp_flag = p_netif_info->flags;
                    tmp_new_vci = p_netif_info->inner_vid;
                    tmp_out_vlan_tag = p_netif_info->outer_vid;
                    bfFirst = 0;
                }
                else
                {
                    if ( ( tmp_flag & ( NETIF_VLAN_OUTER | NETIF_VLAN_INNER ) )  != ( p_netif_info->flags & ( NETIF_VLAN_OUTER | NETIF_VLAN_INNER ) ) )
                    {
                        bfAccelerate = 0;
                        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_add_mc_group not same flag (%0x_%0x)\n", tmp_flag & (NETIF_VLAN_OUTER | NETIF_VLAN_INNER ), p_netif_info->flags & (NETIF_VLAN_OUTER | NETIF_VLAN_INNER) );
                        ppa_netif_put(p_netif_info);
                        break;
                    }
                    else if ( tmp_out_vlan_tag != p_netif_info->outer_vid )
                    {
                        bfAccelerate = 0;
                        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_add_mc_group not same out vlan tag (%0x_%0x)\n", tmp_out_vlan_tag, p_netif_info->outer_vid);
                        ppa_netif_put(p_netif_info);
                        break;
                    }
                    else if ( tmp_new_vci != p_netif_info->inner_vid )
                    {
                        bfAccelerate = 0;
                        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_add_mc_group not same inner vlan (%0x_%0x)\n", tmp_new_vci , p_netif_info->inner_vid);
                        ppa_netif_put(p_netif_info);
                        break;
                    }
                }

                p_item->dest_ifid |= 1 << p_netif_info->phys_port;  //  sgh change xuliang's original architecture, but for unicast routing/bridging, still keep old one

                ASSERT(p_netif_info->flags & NETIF_MAC_ENTRY_CREATED, "ETH physical interface must have MAC address");
                p_item->src_mac_entry = p_netif_info->mac_entry;
                if ( !p_mc_group->bridging_flag )
                    p_item->flags |= SESSION_VALID_SRC_MAC;
                else //otherwise clear this bit in case it is set beofre calling this API
                {
                    p_item->flags &= ~SESSION_VALID_SRC_MAC;
                }
                
                p_item->netif[idx] = p_netif_info->netif;
                p_item->ttl[idx]   = p_mc_group->array_mem_ifs[i].ttl;
                p_item->if_mask |= 1 << idx;

                ppa_netif_put(p_netif_info);

                idx++;
            }
            else
            {
                bfAccelerate = 0;
                ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_add_mc_group cannot find the interface:%s)\n", p_mc_group->array_mem_ifs[i].ifname);
                break;
            }
        }
    }

    if ( bfAccelerate == 0 || idx == 0 || (!p_mc_group->bridging_flag && !(p_item->flags & SESSION_VALID_SRC_MAC)) )
    {        
        return IFX_FAILURE;
    }

    //  VLAN
    ASSERT(tmp_flag & NETIF_VLAN_CANT_SUPPORT, "MC processing can support two layers of VLAN only");
    if ( (tmp_flag & NETIF_VLAN_OUTER) )
    {
        p_item->out_vlan_tag = tmp_out_vlan_tag;
        p_item->flags |= SESSION_VALID_OUT_VLAN_INS;
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "set SESSION_VALID_OUT_VLAN_INS:%x\n", p_item->out_vlan_tag, tmp_new_vci);
    }
    else //otherwise clear this bit in case it is set beofre calling this API
    {
        p_item->flags &= ~SESSION_VALID_OUT_VLAN_INS;
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "unset SESSION_VALID_OUT_VLAN_INS\n");
    }
    
    if ( (tmp_flag & NETIF_VLAN_INNER) )
    {
        p_item->new_vci = tmp_new_vci;

        p_item->flags |= SESSION_VALID_VLAN_INS;
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "set SESSION_VALID_VLAN_INS:%x\n", p_item->new_vci);
    }
    else //otherwise clear this bit in case it is set beofre calling this API
    {
        p_item->flags &= ~SESSION_VALID_VLAN_INS;
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "unset SESSION_VALID_VLAN_INS\n");
    }

   //PPPOE
    if ( !p_mc_group->bridging_flag )
        p_item->flags |= SESSION_VALID_PPPOE;   //  firmware will remove PPPoE header, if and only if the PPPoE header available
    else //otherwise clear this bit in case it is set beofre calling this API
    {
        p_item->flags &= ~SESSION_VALID_PPPOE;
    }

    //  multicast  address
    ppa_memcpy( &p_item->ip_mc_group, &p_mc_group->ip_mc_group, sizeof(p_mc_group->ip_mc_group ) ) ;

    if ( p_mc_group->src_ifname && ppa_netif_lookup(p_mc_group->src_ifname, &p_netif_info) == IFX_SUCCESS )
    {
        //  src interface

        if ( ppa_netif_update(NULL, p_mc_group->src_ifname) == IFX_SUCCESS
            && (p_netif_info->flags & NETIF_PHYS_PORT_GOT) )
        {
            //  PPPoE
            if ( !p_mc_group->bridging_flag && (p_netif_info->flags & NETIF_PPPOE) )
                p_item->flags |= SESSION_VALID_PPPOE;
            else //otherwise clear this bit in case it is set beofre calling this API
            {
                p_item->flags &= ~SESSION_VALID_PPPOE;
            }

            //  VLAN
            ASSERT(p_netif_info->flags & NETIF_VLAN_CANT_SUPPORT, "MC processing can support two layers of VLAN only");
            if ( (p_netif_info->flags & NETIF_VLAN_OUTER) )
                p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
            else //otherwise clear this bit in case it is set beofre calling this API
            {
                p_item->flags &= ~SESSION_VALID_OUT_VLAN_RM;
            }
            
            if ( (p_netif_info->flags & NETIF_VLAN_INNER) )
                p_item->flags |= SESSION_VALID_VLAN_RM;
             else //otherwise clear this bit in case it is set beofre calling this API
            {
                p_item->flags &= ~SESSION_VALID_VLAN_RM;
            }
             
        }
        else  /*not allowed to support non-physical interfaces,like bridge */
        {
            ppa_netif_put(p_netif_info);
            return IFX_FAILURE;
        }
        p_item->src_netif = p_netif_info->netif;

        ppa_netif_put(p_netif_info);
    }

    p_item->num_ifs = idx;

    p_item->dslwan_qid = p_mc_group->dslwan_qid;

    //force update some status by hook itself
    if ( (flags & PPA_F_SESSION_VLAN) )
    {
        if ( p_mc_group->vlan_insert )
        {
            p_item->flags |= SESSION_VALID_VLAN_INS;
            p_item->new_vci = ((uint32_t)p_mc_group->vlan_prio << 13) | ((uint32_t)p_mc_group->vlan_cfi << 12) | (uint32_t)p_mc_group->vlan_id;
        }
        else
        {
            p_item->flags &= ~SESSION_VALID_VLAN_INS;
            p_item->new_vci = 0;
        }

        if ( p_mc_group->vlan_remove )
            p_item->flags |= SESSION_VALID_VLAN_RM;
        else
            p_item->flags &= ~SESSION_VALID_VLAN_RM;
    }     

    if ( (flags & PPA_F_SESSION_OUT_VLAN) )
    {
        if ( p_mc_group->out_vlan_insert )
        {
            p_item->flags |= SESSION_VALID_OUT_VLAN_INS;
            p_item->out_vlan_tag = p_mc_group->out_vlan_tag;
        }
        else
        {
            p_item->flags &= ~SESSION_VALID_OUT_VLAN_INS;
            p_item->out_vlan_tag = 0;
        }

        if ( p_mc_group->out_vlan_remove )
            p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
        else
            p_item->flags &= ~SESSION_VALID_OUT_VLAN_RM;
    }
   
    if ( (flags & PPA_F_SESSION_NEW_DSCP) )
    {
        if ( p_mc_group->new_dscp_en )
        {
            p_item->new_dscp = p_mc_group->new_dscp;
            p_item->flags |= SESSION_VALID_NEW_DSCP;
        }
        else
            p_item->new_dscp &= ~SESSION_VALID_NEW_DSCP;
    }
    
    ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "mc flag:%x\n", p_item->flags);
    return IFX_SUCCESS;
}    

int32_t ppa_add_mc_group(PPA_MC_GROUP *p_mc_group, struct mc_group_list_item **pp_item, uint32_t flags)
{
    struct mc_group_list_item *p_item;

    p_item = ppa_alloc_mc_group_list_item();
    if ( !p_item )
        return IFX_ENOMEM;
     
    if( ppa_mc_group_checking(p_mc_group, p_item, flags ) !=IFX_SUCCESS )
    {
        ppa_free_mc_group_list_item(p_item);
        return IFX_FAILURE;
    }
    
    ppa_insert_mc_group_item(p_item);

    *pp_item = p_item;

    return IFX_SUCCESS;
}

int32_t ppa_update_mc_group_extra(PPA_SESSION_EXTRA *p_extra, struct mc_group_list_item *p_item, uint32_t flags)
{
    if ( (flags & PPA_F_SESSION_NEW_DSCP) )
    {
        if ( p_extra->dscp_remark )
        {
            p_item->flags |= SESSION_VALID_NEW_DSCP;
            p_item->new_dscp = p_extra->new_dscp;
        }
        else
            p_item->flags &= ~SESSION_VALID_NEW_DSCP;
    }

    if ( (flags & PPA_F_SESSION_VLAN) )
    {
        if ( p_extra->vlan_insert )
        {
            p_item->flags |= SESSION_VALID_VLAN_INS;
            p_item->new_vci = ((uint32_t)p_extra->vlan_prio << 13) | ((uint32_t)p_extra->vlan_cfi << 12) | p_extra->vlan_id;
        }
        else
        {
            p_item->flags &= ~SESSION_VALID_VLAN_INS;
            p_item->new_vci = 0;
        }

        if ( p_extra->vlan_remove )
            p_item->flags |= SESSION_VALID_VLAN_RM;
        else
            p_item->flags &= ~SESSION_VALID_VLAN_RM;
    }

    if ( (flags & PPA_F_SESSION_OUT_VLAN) )
    {
        if ( p_extra->out_vlan_insert )
        {
            p_item->flags |= SESSION_VALID_OUT_VLAN_INS;
            p_item->out_vlan_tag = p_extra->out_vlan_tag;
        }
        else
        {
            p_item->flags &= ~SESSION_VALID_OUT_VLAN_INS;
            p_item->out_vlan_tag = 0;
        }

        if ( p_extra->out_vlan_remove )
            p_item->flags |= SESSION_VALID_OUT_VLAN_RM;
        else
            p_item->flags &= ~SESSION_VALID_OUT_VLAN_RM;
    }

    if ( p_extra->dslwan_qid_remark )
        p_item->dslwan_qid = p_extra->dslwan_qid;

    return IFX_SUCCESS;
}

void ppa_remove_mc_group(struct mc_group_list_item *p_item)
{
    ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_remove_mc_group:  remove %d from PPA\n", p_item->mc_entry);
    ppa_remove_mc_group_item(p_item);

    ppa_free_mc_group_list_item(p_item);
}

int32_t ppa_mc_group_start_iteration(uint32_t *ppos, struct mc_group_list_item **pp_item)
{
    struct mc_group_list_item *p = NULL;
    int idx;
    uint32_t l;

    l = *ppos + 1;

    ppa_lock_get(&g_mc_group_list_lock);

    for ( idx = 0; l && idx < NUM_ENTITY(g_mc_group_list_hash_table); idx++ )
    {
        for ( p = g_mc_group_list_hash_table[idx]; p; p = p->next )
            if ( !--l )
                break;
    }

    if ( l == 0 && p )
    {
        ++*ppos;
        *pp_item = p;
        return IFX_SUCCESS;
    }
    else
    {
        *pp_item = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_mc_group_iterate_next(uint32_t *ppos, struct mc_group_list_item **pp_item)
{
    uint32_t idx;

    if ( *pp_item == NULL )
        return IFX_FAILURE;

    if ( (*pp_item)->next != NULL )
    {
        ++*ppos;
        *pp_item = (*pp_item)->next;
        return IFX_SUCCESS;
    }
    else
    {
        for ( idx = SESSION_LIST_MC_HASH_VALUE((*pp_item)->ip_mc_group.ip.ip) + 1;
              idx < NUM_ENTITY(g_mc_group_list_hash_table);
              idx++ )
            if ( g_mc_group_list_hash_table[idx] != NULL )
            {
                ++*ppos;
                *pp_item = g_mc_group_list_hash_table[idx];
                return IFX_SUCCESS;
            }
        *pp_item = NULL;
        return IFX_FAILURE;
    }
}

void ppa_mc_group_stop_iteration(void)
{
    ppa_lock_release(&g_mc_group_list_lock);
}

/*
 *  multicast routing hardware/firmware operation
 */

int32_t ppa_hw_add_mc_group(struct mc_group_list_item *p_item)
{
    PPE_MC_INFO mc={0};

    mc.out_vlan_info.vlan_entry = ~0;    
    mc.route_type = p_item->bridging_flag ? IFX_PPA_ROUTE_TYPE_NULL : IFX_PPA_ROUTE_TYPE_IPV4;

    //  must be LAN port
    //dest_list = 1 << p_item->dest_ifid;   //  sgh remove it since it is already shifted already
    mc.dest_list = p_item->dest_ifid;          //  due to multiple destination support, the dest_ifid here is a bitmap of destination rather than ifid

    if ( (p_item->flags & SESSION_VALID_OUT_VLAN_INS) )
    {
        mc.out_vlan_info.vlan_id = p_item->out_vlan_tag;
        if ( ifx_ppa_drv_add_outer_vlan_entry( &mc.out_vlan_info, 0) == 0 )
        {
            p_item->out_vlan_entry = mc.out_vlan_info.vlan_entry;
        }
        else
        {
            dbg("add out_vlan_ix error");
            goto OUT_VLAN_ERROR;
        }
    }
    //So far it only support IPv4 yet
    mc.dest_ip_compare = p_item->ip_mc_group.ip.ip;
    
    mc.f_vlan_ins_enable =p_item->flags & SESSION_VALID_VLAN_INS;
    mc.new_vci = p_item->new_vci;
    mc.f_vlan_rm_enable = p_item->flags & SESSION_VALID_VLAN_RM;
    mc.f_src_mac_enable = p_item->flags & SESSION_VALID_SRC_MAC;
    mc.src_mac_ix = p_item->src_mac_entry;
    mc.pppoe_mode = p_item->flags & SESSION_VALID_PPPOE;
    mc.f_out_vlan_ins_enable =p_item->flags & SESSION_VALID_OUT_VLAN_INS;
    mc.out_vlan_info.vlan_entry = p_item->out_vlan_entry;
    mc.f_out_vlan_rm_enable =  p_item->flags & SESSION_VALID_OUT_VLAN_RM;
    mc.f_new_dscp_enable = p_item->flags & SESSION_VALID_NEW_DSCP;
    mc.new_dscp = p_item->new_dscp;
    mc.dest_qid = p_item->dslwan_qid;

    if ( ifx_ppa_drv_add_wan_mc_entry( &mc, 0) == IFX_SUCCESS )
    {
        p_item->mc_entry = mc.p_entry;
        p_item->flags |= SESSION_ADDED_IN_HW;
        return IFX_SUCCESS;
    }

    p_item->out_vlan_entry = ~0;
    ifx_ppa_drv_del_outer_vlan_entry(&mc.out_vlan_info, 0);
OUT_VLAN_ERROR:
    return IFX_EAGAIN;
}

int32_t ppa_hw_update_mc_group_extra(struct mc_group_list_item *p_item, uint32_t flags)
{
    uint32_t update_flags = 0;
    PPE_MC_INFO mc={0};

    if ( (flags & PPA_F_SESSION_NEW_DSCP) )
        update_flags |= IFX_PPA_UPDATE_WAN_MC_ENTRY_NEW_DSCP_EN | IFX_PPA_UPDATE_WAN_MC_ENTRY_NEW_DSCP;

    if ( (flags & PPA_F_SESSION_VLAN) )
        update_flags |=IFX_PPA_UPDATE_WAN_MC_ENTRY_VLAN_INS_EN |IFX_PPA_UPDATE_WAN_MC_ENTRY_NEW_VCI | IFX_PPA_UPDATE_WAN_MC_ENTRY_VLAN_RM_EN;

    if ( (flags & PPA_F_SESSION_OUT_VLAN) )
    {
        mc.out_vlan_info.vlan_entry = p_item->out_vlan_entry;
        if ( ifx_ppa_drv_get_outer_vlan_entry( &mc.out_vlan_info , 0) == IFX_SUCCESS )
        {            
            if ( mc.out_vlan_info.vlan_id == p_item->out_vlan_tag )
            {
                //  entry not changed
                goto PPA_HW_UPDATE_MC_GROUP_EXTRA_OUT_VLAN_GOON;
            }
            else
            {
                //  entry changed, so delete old first and create new one later
                ifx_ppa_drv_del_outer_vlan_entry(&mc.out_vlan_info, 0);
                p_item->out_vlan_entry = ~0;
            }
        }
        //  create new OUT VLAN entry
        mc.out_vlan_info.vlan_id = p_item->out_vlan_tag;
        if ( ifx_ppa_drv_add_outer_vlan_entry( &mc.out_vlan_info, 0) == IFX_SUCCESS )
        {
            p_item->out_vlan_entry = mc.out_vlan_info.vlan_entry;
            update_flags |= IFX_PPA_UPDATE_WAN_MC_ENTRY_OUT_VLAN_IX;
        }
        else
        {
            err("add_outer_vlan_entry fail");
            return IFX_EAGAIN;
        }

        update_flags |= IFX_PPA_UPDATE_WAN_MC_ENTRY_OUT_VLAN_INS_EN | IFX_PPA_UPDATE_WAN_MC_ENTRY_OUT_VLAN_RM_EN ; //IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_INS_EN | IFX_PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_RM_EN;
    }
PPA_HW_UPDATE_MC_GROUP_EXTRA_OUT_VLAN_GOON:
    update_flags |= IFX_PPA_UPDATE_WAN_MC_ENTRY_DEST_QID;  //sgh chnage to update qid, since there is no such flag defined at present

    mc.p_entry = p_item->mc_entry;
    mc.f_vlan_ins_enable = p_item->flags & SESSION_VALID_VLAN_INS;
    mc.new_vci = p_item->new_vci;
    mc.f_vlan_rm_enable = p_item->flags & SESSION_VALID_VLAN_RM;
    mc.f_src_mac_enable = p_item->flags & SESSION_VALID_SRC_MAC;
    mc.src_mac_ix = p_item->src_mac_entry;
    mc.pppoe_mode = p_item->flags & SESSION_VALID_PPPOE;
    mc.f_out_vlan_ins_enable = p_item->flags & SESSION_VALID_OUT_VLAN_INS;
    mc.out_vlan_info.vlan_entry= p_item->out_vlan_entry;
    mc.f_out_vlan_rm_enable = p_item->flags & SESSION_VALID_OUT_VLAN_RM;
    mc.f_new_dscp_enable= p_item->flags & SESSION_VALID_NEW_DSCP;
    mc.new_dscp = p_item->new_dscp;
    mc.dest_qid = p_item->dslwan_qid;
    mc.dest_list = 0;
    mc.update_flags= update_flags;
    ifx_ppa_drv_update_wan_mc_entry(&mc, 0);

    return IFX_SUCCESS;
}

void ppa_hw_del_mc_group(struct mc_group_list_item *p_item)
{
    if ( (p_item->flags & SESSION_ADDED_IN_HW) )
    {
        PPE_MC_INFO mc;
        PPE_OUT_VLAN_INFO out_vlan={0};

        mc.p_entry = p_item->mc_entry;
        out_vlan.vlan_entry = p_item->out_vlan_entry;
        //  when init, these entry values are ~0, the max the number which can be detected by these functions
        ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_del_mc_group:  remove %d from HW\n", p_item->mc_entry);
        ifx_ppa_drv_del_wan_mc_entry(&mc, 0);
        p_item->mc_entry = ~0;

        //  taken from netif_info, so don't need to be removed from MAC table
        p_item->src_mac_entry = ~0;
        
        ifx_ppa_drv_del_outer_vlan_entry( &out_vlan, 0);
        p_item->out_vlan_entry = ~0;

        p_item->flags &= ~SESSION_ADDED_IN_HW;
    }
}

/*
 *  routing polling timer
 */

void ppa_set_polling_timer(uint32_t polling_time, uint32_t force)
{
    if ( polling_time < g_hit_polling_time )
    {
        //  remove timer
        ppa_timer_del(&g_hit_stat_timer);

        //  timeout can not be zero
        g_hit_polling_time = polling_time < 1 ? 1 : polling_time;

        //  check hit stat in case the left time is less then the new timeout
        ppa_check_hit_stat(0);  //  timer is added in this function
    }
    else if ( (polling_time > g_hit_polling_time) && force )
    {
        g_hit_polling_time = polling_time;
    }
}

/*
 *  bridging session operation
 */

int32_t ppa_bridging_lookup_session(uint8_t *mac, PPA_NETIF *netif, struct bridging_session_list_item **pp_item)
{
    int32_t ret;
    struct bridging_session_list_item *p_prev, *p_cur;
    struct bridging_session_list_item **pp_header;

    ASSERT(pp_item != NULL, "pp_item == NULL");

    ret = IFX_PPA_SESSION_NOT_ADDED;

    pp_header = g_bridging_session_list_hash_table + BRIDGING_SESSION_LIST_HASH_VALUE(mac);
    p_prev = NULL;
    ppa_lock_get(&g_bridging_session_list_lock);
    p_cur = *pp_header;
    while ( p_cur && ppa_memcmp(mac, p_cur->mac, PPA_ETH_ALEN) != 0 /* !(*(uint32_t *)mac == *(uint32_t *)p_cur->mac && *((uint16_t *)mac + 2) == *((uint16_t *)p_cur->mac + 2)) */ )
    {
        p_prev = p_cur;
        p_cur = p_cur->next;
    }
    if ( p_cur )
    {
        if ( p_prev )
        {
            p_prev->next = p_cur->next;
            p_cur->next = *pp_header;
            *pp_header = p_cur;
        }

        ret = IFX_PPA_SESSION_EXISTS;
    }
    ppa_lock_release(&g_bridging_session_list_lock);

    *pp_item = p_cur;

    return ret;
}

int32_t ppa_bridging_add_session(uint8_t *mac, PPA_NETIF *netif, struct bridging_session_list_item **pp_item, uint32_t flags)
{
    struct bridging_session_list_item *p_item;
    struct netif_info *ifinfo;

  /*ctc*/
  #ifdef CONFIG_ARC_PPA_FILTER
	ARC_PPA_BRIDGE	stBr = { mac, netif };
	if ( arc_ppa_filter_match( &stBr, ARC_PPA_BUF_TYPE_BRIDGE ) >= 0 )
		return IFX_ENOTPOSSIBLE;
  #endif

    if ( ppa_netif_update(netif, NULL) != IFX_SUCCESS )
        return IFX_ENOTPOSSIBLE;

    if ( ppa_netif_lookup(ppa_get_netif_name(netif), &ifinfo) != IFX_SUCCESS )
        return IFX_FAILURE;

#if !defined(CONFIG_IFX_PPA_API_DIRECTPATH_BRIDGING)  
    if( ifx_ppa_get_ifid_for_netif(netif) > 0 ) return IFX_FAILURE;   // no need to learn and program mac address in ppe/switch if directpath bridging feature is disabled
#endif

    p_item = ppa_bridging_alloc_session_list_item();
    if ( !p_item )
    {
        ppa_netif_put(ifinfo);
        return IFX_ENOMEM;
    }

    dump_bridging_list_item(p_item, "ppa_bridging_add_session (after init)");

    ppa_memcpy(p_item->mac, mac, PPA_ETH_ALEN);
    p_item->netif         = netif;
    p_item->timeout       = ppa_bridging_get_default_session_timeout();
    p_item->last_hit_time = ppa_get_time_in_sec();

    //  TODO: vlan related fields

    //  decide destination list
    /*yixin: VR9 need to add mac address to switch only if the source mac not from switch port  */
    if ( !(ifinfo->flags & NETIF_PHYS_PORT_GOT) )
    {
        PPE_COUNT_CFG count={0};
        ifx_ppa_drv_get_number_of_phys_port( &count, 0);
        p_item->dest_ifid =  count.num + 1;   //trick here: it will be used for PPE Driver's HAL     
    }else{
        p_item->dest_ifid = ifinfo->phys_port;
    }
    if ( (ifinfo->flags & NETIF_PHY_ATM) )
        p_item->dslwan_qid = ifinfo->dslwan_qid;

    if ( (flags & PPA_F_STATIC_ENTRY) )
    {
        p_item->flags    |= SESSION_STATIC;
        p_item->timeout   = ~0; //  max timeout
    }

    if ( (flags & PPA_F_DROP_PACKET) )
        p_item->flags    |= SESSION_DROP;

    ppa_bridging_insert_session_item(p_item);

    dump_bridging_list_item(p_item, "ppa_bridging_add_session (after setting)");

    *pp_item = p_item;

    ppa_netif_put(ifinfo);

    return IFX_SUCCESS;
}

void ppa_bridging_remove_session(struct bridging_session_list_item *p_item)
{
    ppa_bridging_remove_session_item(p_item);

    ppa_bridging_free_session_list_item(p_item);
}

void dump_bridging_list_item(struct bridging_session_list_item *p_item, char *comment)
{
#if defined(DEBUG_DUMP_LIST_ITEM) && DEBUG_DUMP_LIST_ITEM

    if ( !(g_ppa_dbg_enable & DBG_ENABLE_MASK_DUMP_BRIDGING_SESSION) )
        return;

    if ( comment )
        printk("dump_bridging_list_item - %s\n", comment);
    else
        printk("dump_bridging_list_item\n");
    printk("  next             = %08X\n", (uint32_t)p_item->next);
    printk("  mac[6]           = %02x:%02x:%02x:%02x:%02x:%02x\n", p_item->mac[0], p_item->mac[1], p_item->mac[2], p_item->mac[3], p_item->mac[4], p_item->mac[5]);
    printk("  netif            = %08X\n", (uint32_t)p_item->netif);
    printk("  timeout          = %d\n",   p_item->timeout);
    printk("  last_hit_time    = %d\n",   p_item->last_hit_time);
    printk("  flags            = %08X\n", p_item->flags);
    printk("  bridging_entry   = %08X\n", p_item->bridging_entry);

#endif
}

int32_t ppa_bridging_session_start_iteration(uint32_t *ppos, struct bridging_session_list_item **pp_item)
{
    struct bridging_session_list_item *p = NULL;
    int idx;
    uint32_t l;

    l = *ppos + 1;

    ppa_lock_get(&g_bridging_session_list_lock);

    for ( idx = 0; l && idx < NUM_ENTITY(g_bridging_session_list_hash_table); idx++ )
    {
        for ( p = g_bridging_session_list_hash_table[idx]; p; p = p->next )
            if ( !--l )
                break;
    }

    if ( l == 0 && p )
    {
        ++*ppos;
        *pp_item = p;
        return IFX_SUCCESS;
    }
    else
    {
        *pp_item = NULL;
        return IFX_FAILURE;
    }
}

int32_t ppa_bridging_session_iterate_next(uint32_t *ppos, struct bridging_session_list_item **pp_item)
{
    uint32_t idx;

    if ( *pp_item == NULL )
        return IFX_FAILURE;

    if ( (*pp_item)->next != NULL )
    {
        ++*ppos;
        *pp_item = (*pp_item)->next;
        return IFX_SUCCESS;
    }
    else
    {
        for ( idx = BRIDGING_SESSION_LIST_HASH_VALUE((*pp_item)->mac) + 1;
              idx < NUM_ENTITY(g_bridging_session_list_hash_table);
              idx++ )
            if ( g_bridging_session_list_hash_table[idx] != NULL )
            {
                ++*ppos;
                *pp_item = g_bridging_session_list_hash_table[idx];
                return IFX_SUCCESS;
            }
        *pp_item = NULL;
        return IFX_FAILURE;
    }
}

void ppa_bridging_session_stop_iteration(void)
{
    ppa_lock_release(&g_bridging_session_list_lock);
}

/*
 *  bridging session hardware/firmware operation
 */

int32_t ppa_bridging_hw_add_session(struct bridging_session_list_item *p_item)
{
    PPE_BR_MAC_INFO br_mac={0};

    br_mac.port = p_item->dest_ifid;

    if ( (p_item->flags & SESSION_DROP) )
        br_mac.dest_list = 0;  //  no dest list, dropped
    else
        br_mac.dest_list = 1 << p_item->dest_ifid;

    ppa_memcpy(br_mac.mac, p_item->mac, sizeof(br_mac.mac));
    br_mac.f_src_mac_drop = p_item->flags & SESSION_SRC_MAC_DROP_EN;
    br_mac.dslwan_qid = p_item->dslwan_qid;

    if ( ifx_ppa_drv_add_bridging_entry(&br_mac, 0) == IFX_SUCCESS )
    {
        p_item->bridging_entry = br_mac.p_entry;
        p_item->flags |= SESSION_ADDED_IN_HW;
        return IFX_SUCCESS;
    }

    return IFX_FAILURE;
}

void ppa_bridging_hw_del_session(struct bridging_session_list_item *p_item)
{
    if ( (p_item->flags & SESSION_ADDED_IN_HW) )
    {
        PPE_BR_MAC_INFO br_mac={0};
        br_mac.p_entry = p_item->bridging_entry;
        ifx_ppa_drv_del_bridging_entry(&br_mac, 0);
        p_item->bridging_entry = ~0;

        p_item->flags &= ~SESSION_ADDED_IN_HW;
    }
}

/*
 *  bridging polling timer
 */

void ppa_bridging_set_polling_timer(uint32_t polling_time)
{
    if ( polling_time < g_bridging_hit_polling_time )
    {
        //  remove timer
        ppa_timer_del(&g_bridging_hit_stat_timer);

        //  timeout can not be zero
        g_bridging_hit_polling_time = polling_time < 1 ? 1 : polling_time;

        //  check hit stat in case the left time is less then the new timeout
        ppa_bridging_check_hit_stat(0); //  timer is added in this function
    }
}

/*
 *  special function
 */

void ppa_remove_sessions_on_netif(PPA_IFNAME *ifname, int f_is_lan)
{
    struct netif_info *ifinfo;
    uint32_t lan_wan_flag;      //  bit 0: lan, bit 1: wan

    if ( ppa_netif_lookup(ifname, &ifinfo) == IFX_SUCCESS )
        lan_wan_flag = 3;
    else
        lan_wan_flag = f_is_lan ? 1 : 2;

    ppa_remove_routing_sessions_on_netif(ifname, lan_wan_flag);
    if ( lan_wan_flag == 2 )
        ppa_remove_mc_groups_on_netif(ifname);
    if ( lan_wan_flag == 3 )
        ppa_remove_bridging_sessions_on_netif(ifname);
}




/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

int32_t ppa_api_session_manager_init(void)
{
    //  start timer
    ppa_timer_init(&g_hit_stat_timer, ppa_check_hit_stat);
    ppa_timer_add(&g_hit_stat_timer, g_hit_polling_time);
    ppa_timer_init(&g_bridging_hit_stat_timer, ppa_bridging_check_hit_stat);
    ppa_timer_add(&g_bridging_hit_stat_timer, g_bridging_hit_polling_time);

    return IFX_SUCCESS;
}

void ppa_api_session_manager_exit(void)
{
    ppa_timer_del(&g_hit_stat_timer);
    ppa_timer_del(&g_bridging_hit_stat_timer);

    ppa_free_session_list();
    ppa_free_mc_group_list();
    ppa_free_bridging_session_list();

    ppa_kmem_cache_shrink(g_session_item_cache);
    ppa_kmem_cache_shrink(g_mc_group_item_cache);
    ppa_kmem_cache_shrink(g_bridging_session_item_cache);
}

int32_t ppa_api_session_manager_create(void)
{
    if ( ppa_mem_cache_create("ppa_session_item", sizeof(struct session_list_item), &g_session_item_cache) )
    {
        err("Failed in creating mem cache for routing session list.");
        goto PPA_API_SESSION_MANAGER_CREATE_FAIL;
    }

    if ( ppa_mem_cache_create("mc_group_item", sizeof(struct mc_group_list_item), &g_mc_group_item_cache) )
    {
        err("Failed in creating mem cache for multicast group list.");
        goto PPA_API_SESSION_MANAGER_CREATE_FAIL;
    }

    if ( ppa_mem_cache_create("bridging_sess_item", sizeof(struct bridging_session_list_item), &g_bridging_session_item_cache) )
    {
        err("Failed in creating mem cache for bridging session list.");
        goto PPA_API_SESSION_MANAGER_CREATE_FAIL;
    }

    if ( ppa_lock_init(&g_session_list_lock) )
    {
        err("Failed in creating lock for routing session list.");
        goto PPA_API_SESSION_MANAGER_CREATE_FAIL;
    }

    if ( ppa_lock_init(&g_mc_group_list_lock) )
    {
        err("Failed in creating lock for multicast group list.");
        goto PPA_API_SESSION_MANAGER_CREATE_FAIL;
    }

    if ( ppa_lock_init(&g_bridging_session_list_lock) )
    {
        err("Failed in creating lock for bridging session list.");
        goto PPA_API_SESSION_MANAGER_CREATE_FAIL;
    }

    return IFX_SUCCESS;

PPA_API_SESSION_MANAGER_CREATE_FAIL:
    ppa_api_session_manager_destroy();
    return IFX_EIO;
}

void ppa_api_session_manager_destroy(void)
{
    if ( g_session_item_cache )
    {
        ppa_mem_cache_destroy(g_session_item_cache);
        g_session_item_cache = NULL;
    }

    if ( g_mc_group_item_cache )
    {
        ppa_mem_cache_destroy(g_mc_group_item_cache);
        g_mc_group_item_cache = NULL;
    }

    if ( g_bridging_session_item_cache )
    {
        ppa_mem_cache_destroy(g_bridging_session_item_cache);
        g_bridging_session_item_cache = NULL;
    }

    ppa_lock_destroy(&g_session_list_lock);

    ppa_lock_destroy(&g_mc_group_list_lock);

    ppa_lock_destroy(&g_bridging_session_list_lock);
}

uint32_t ppa_api_get_session_poll_timer(void)
{
    return g_hit_polling_time;
}
    

EXPORT_SYMBOL(ppa_bridging_session_start_iteration);
EXPORT_SYMBOL(ppa_bridging_session_iterate_next);
EXPORT_SYMBOL(ppa_bridging_session_stop_iteration);
EXPORT_SYMBOL(ppa_session_start_iteration);
EXPORT_SYMBOL(ppa_session_iterate_next);
EXPORT_SYMBOL(ppa_session_stop_iteration);
EXPORT_SYMBOL(ppa_mc_group_start_iteration);
EXPORT_SYMBOL(ppa_mc_group_iterate_next);
EXPORT_SYMBOL(ppa_mc_group_stop_iteration);
EXPORT_SYMBOL(ppa_alloc_session_list_item);
EXPORT_SYMBOL(ppa_free_session_list_item);
EXPORT_SYMBOL(ppa_insert_session_item);
EXPORT_SYMBOL(ppa_api_get_session_poll_timer);



/* ctc */
#ifdef CONFIG_ARC_PPA_FILTER


int				arc_ppa_filter_enabled = 1;
int				arc_ppa_filter_debug_level = 0;
int				arc_ppa_filter_num = 0;
unsigned long	arc_ppa_filter_hit_total = 0;
unsigned long	arc_ppa_filter_miss_total = 0;
stArcPpaFilter	arc_ppa_filters[ ARC_PPA_FILTER_MAX ];

int arc_ppa_filter_strcmp( char* str1, char* str2 )
{
	int	cnt;

	if ( str1 == NULL && str2 == NULL )
		return 0;

	if ( str1 == NULL && str2 != NULL )
		return -1;

	if ( str1 != NULL && str2 == NULL )
		return 1;

	for ( cnt = 0; ; cnt++ ) {
		if ( str1[cnt] == str2[cnt] ) {
			if ( str1[cnt] == 0 )
				return 0;
			continue;
		}
		if ( str1[cnt] == 0 )
			return -1;
		return 1;
	}
}

int arc_ppa_filter_is_enable( void )
{
	return arc_ppa_filter_enabled;
}

int arc_ppa_filter_enable( void )
{
	arc_ppa_filter_enabled = 1;

	return 0;
}

int arc_ppa_filter_disable( void )
{
	arc_ppa_filter_enabled = 0;

	return 0;
}

int arc_ppa_filter_get_debug_level( void )
{
	return arc_ppa_filter_debug_level;
}

int arc_ppa_filter_set_debug_level( int level )
{
	int old = arc_ppa_filter_debug_level;
	arc_ppa_filter_debug_level = level;
	return old;
}

int arc_ppa_filter_cnt( void )
{
	return arc_ppa_filter_num;
}

int arc_ppa_filter_hit( void )
{
	return arc_ppa_filter_hit_total;
}

int arc_ppa_filter_miss( void )
{
	return arc_ppa_filter_miss_total;
}

int arc_ppa_filter_clear( int iId )
{
	if ( iId < 0 && iId >= arc_ppa_filter_num)
		return -1;

	arc_ppa_filters[ iId ].hit = 0;

	return 0;
}

int arc_ppa_filter_clear_all( void )
{
	int	cnt;

	arc_ppa_filter_hit_total = 0;
	arc_ppa_filter_miss_total = 0;

	for ( cnt = 0; cnt < arc_ppa_filter_num ; cnt++ ) {
		arc_ppa_filters[ cnt ].hit = 0;
	}

	return 0;
}

#define	VALID_ADDR( addr ) ( ((void*)(addr)) > (void*)0x8000000 && ((void*)(addr)) < (void*)0x90000000 )

int arc_ppa_filter_match( void* pData, int iType )
{
	int				cnt, cnt2;
	stArcPpaFilter	stFilter;
	stArcPpaFilter*	pPtr;
	char*			ifname;
	PPA_BUF*		ppa_buf = (PPA_BUF*)pData;
	PPA_MC_GROUP*	mc_group = (PPA_MC_GROUP*)pData;
	ARC_PPA_BRIDGE*	ppa_bridge = (ARC_PPA_BRIDGE*)pData;
	uint8_t*		pL2Pkt=0;
	uint8_t*		pL3Pkt=0;
	uint8_t*		pL4Pkt=0;
	uint8_t*		pPktData=0;
	uint8_t*		pPktEnd=0;

	if (arc_ppa_filter_enabled == 0)
		return -1;

	if (arc_ppa_filter_num <= 0)
		return -1;

	if (pData == NULL)
		return -1;

	switch ( iType ) {
	  case ARC_PPA_BUF_TYPE_UCAST:
				stFilter.mask = ARC_PPA_FILTER_MASK_RXIF | ARC_PPA_FILTER_MASK_PKTDATA | ARC_PPA_FILTER_MASK_MAC;
				ifname             = ppa_get_netif_name( ppa_get_pkt_src_if( ppa_buf ) );
				pPktData           = (uint8_t*)((uint32_t*)(ppa_buf->cb+sizeof(ppa_buf->cb))-1);
				pPktData           = (uint8_t*)*(uint32_t*)((uint32_t)pPktData & ~0x3);
				if ( ! VALID_ADDR( pPktData ) ) {
					//printk( "%s: pPktData %p invalid\n", __FUNCTION__, pPktData );
					return -1;
				}
				if (pPktData >= ppa_buf->head && pPktData <= ppa_buf->tail)
					pL2Pkt         = pPktData;
				else
					pL2Pkt         = ppa_buf->mac_header;
				if ( ! VALID_ADDR( pL2Pkt ) ) {
					//printk( "%s: pL2Pkt %p invalid\n", __FUNCTION__, pL2Pkt );
					return -1;
				}
				pL3Pkt             = ppa_buf->network_header;
				if ( ! VALID_ADDR( pL3Pkt ) ) {
					//printk( "%s: pL3Pkt %p invalid\n", __FUNCTION__, pL3Pkt );
					return -1;
				}
				pL4Pkt             = pL3Pkt + (pL3Pkt[0] & 0xf)*4;
				if ( ! VALID_ADDR( pL4Pkt ) ) {
					//printk( "%s: pL4Pkt %p invalid\n", __FUNCTION__, pL4Pkt );
					return -1;
				}
				pPktEnd            = ppa_buf->tail;
				if ( ! VALID_ADDR( pPktEnd ) ) {
					//printk( "%s: pPktEnd %p invalid\n", __FUNCTION__, pPktEnd );
					return -1;
				}
			    stFilter.srcmac[0] = pL2Pkt[6];
			    stFilter.srcmac[1] = pL2Pkt[7];
			    stFilter.srcmac[2] = pL2Pkt[8];
			    stFilter.srcmac[3] = pL2Pkt[9];
			    stFilter.srcmac[4] = pL2Pkt[10];
			    stFilter.srcmac[5] = pL2Pkt[11];
				pPktData           = pPktData + 12;
				while ( (pPktData[0]==0x81 && pPktData[0]==0x00)
					 || (pPktData[0]==0x91 && pPktData[0]==0x00)
					 || (pPktData[0]==0x88 && pPktData[0]==0xa8) ) /* skip VLAN-tag */
				{
					if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_PRIO) ) {
						stFilter.mask |= ARC_PPA_FILTER_MASK_PRIO;
						stFilter.prio  = pPktData[2] >> 5;
					}
					pPktData += 4;
				}
				if ( (pPktData[0]==0x08 && pPktData[1]==0x00) || (pPktData[0]==0x88 && pPktData[1]==0x64) )
				{ /*IP & PPPoE session*/
					stFilter.mask |= (ARC_PPA_FILTER_MASK_PROTO | ARC_PPA_FILTER_MASK_TOS
							  		| ARC_PPA_FILTER_MASK_SRCIP | ARC_PPA_FILTER_MASK_DSTIP );
					stFilter.tos       = ppa_get_pkt_ip_tos( ppa_buf );
					stFilter.src_ip    = ppa_get_pkt_src_ip( ppa_buf );
					stFilter.dst_ip    = ppa_get_pkt_dst_ip( ppa_buf );
					stFilter.l4proto   = ppa_get_pkt_ip_proto( ppa_buf );
					if (stFilter.l4proto == IFX_IPPROTO_TCP || stFilter.l4proto == IFX_IPPROTO_UDP) {
						stFilter.src_port  = ppa_get_pkt_src_port( ppa_buf );
						stFilter.dst_port  = ppa_get_pkt_dst_port( ppa_buf );
						stFilter.mask |= (ARC_PPA_FILTER_MASK_SRCPORT | ARC_PPA_FILTER_MASK_DSTPORT);
					}
				}
				else if (pPktData[0]==0x08 && pPktData[1]==0x06) /*ARP*/
				{
					pPktData += 2;
					stFilter.mask |= ( ARC_PPA_FILTER_MASK_SRCIP | ARC_PPA_FILTER_MASK_DSTIP );
					stFilter.src_ip.ip =  ((uint32_t)pPktData[28] << 24)
										+ ((uint32_t)pPktData[29] << 16)
										+ ((uint32_t)pPktData[30] <<  8)
										+ ((uint32_t)pPktData[31]);
					stFilter.dst_ip.ip =  ((uint32_t)pPktData[38] << 24)
										+ ((uint32_t)pPktData[39] << 16)
										+ ((uint32_t)pPktData[40] <<  8)
										+ ((uint32_t)pPktData[41]);
				}

				if (arc_ppa_filter_debug_level)
					printk( "PPA filter: ucast from %s smac %02x:%02x:%02x:%02x:%02x:%02x proto %d sip %d.%d.%d.%d sport %d dip %d.%d.%d.%d dport %d\n"
							, (ifname ? ifname : "-")
							, stFilter.srcmac[0], stFilter.srcmac[1], stFilter.srcmac[2]
							, stFilter.srcmac[3], stFilter.srcmac[4], stFilter.srcmac[5]
							, stFilter.l4proto
							, (stFilter.src_ip.ip>>24),      (stFilter.src_ip.ip>>16)&0xff
							, (stFilter.src_ip.ip>> 8)&0xff, (stFilter.src_ip.ip    )&0xff
							, stFilter.src_port
							, (stFilter.dst_ip.ip>>24),      (stFilter.dst_ip.ip>>16)&0xff
							, (stFilter.dst_ip.ip>> 8)&0xff, (stFilter.dst_ip.ip    )&0xff
							, stFilter.dst_port );
				break;
	  case ARC_PPA_BUF_TYPE_MCAST:
				if (mc_group->ip_mc_group.f_ipv6)
					return -1;
				stFilter.mask = ARC_PPA_FILTER_MASK_RXIF | ARC_PPA_FILTER_MASK_SRCIP;
				stFilter.src_ip.ip     = mc_group->ip_mc_group.ip.ip;
				stFilter.src_ip.ip6[0] = mc_group->ip_mc_group.ip.ip6[0];
				stFilter.src_ip.ip6[1] = mc_group->ip_mc_group.ip.ip6[1];
				stFilter.src_ip.ip6[2] = mc_group->ip_mc_group.ip.ip6[2];
				stFilter.src_ip.ip6[3] = mc_group->ip_mc_group.ip.ip6[3];
				ifname             = mc_group->src_ifname;

				if (arc_ppa_filter_debug_level)
					printk( "PPA filter: mcast from %s mcip %d.%d.%d.%d\n"
							, (ifname ? ifname : "-")
							, (stFilter.src_ip.ip>>24),      (stFilter.src_ip.ip>>16)&0xff
							, (stFilter.src_ip.ip>> 8)&0xff, (stFilter.src_ip.ip    )&0xff );
				break;
	  case ARC_PPA_BUF_TYPE_BRIDGE:
				stFilter.mask = ARC_PPA_FILTER_MASK_RXIF | ARC_PPA_FILTER_MASK_MAC;
				if (ppa_bridge->mac)
						ppa_memcpy( stFilter.srcmac, ppa_bridge->mac, sizeof(stFilter.srcmac) );
				  else	stFilter.mask &=~ ARC_PPA_FILTER_MASK_MAC;
				ifname             = ppa_get_netif_name( ppa_bridge->rx_if);

				if (arc_ppa_filter_debug_level) {
					if (stFilter.mask & ARC_PPA_FILTER_MASK_MAC)
						printk( "PPA filter: bridge from %s mac %02x:%02x:%02x:%02x:%02x:%02x\n"
								, (ifname ? ifname : "-")
								, stFilter.srcmac[0], stFilter.srcmac[1], stFilter.srcmac[2]
								, stFilter.srcmac[3], stFilter.srcmac[4], stFilter.srcmac[5] );
					else
						printk( "PPA filter: bridge from %s mac -\n"
								, (ifname ? ifname : "-") );
				}
				break;
	  default:
				return -1;
	}

	for ( cnt=0, pPtr=arc_ppa_filters; cnt < arc_ppa_filter_num ; cnt++, pPtr++ ) {
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_DSTPORT )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_DSTPORT) || stFilter.dst_port  != pPtr->dst_port )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_PROTO )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_PROTO)   || stFilter.l4proto   != pPtr->l4proto )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_SRCPORT )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_SRCPORT) || stFilter.src_port  != pPtr->src_port )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_PRIO )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_PRIO)    || stFilter.prio      != pPtr->prio )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_TOS )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_TOS)     || stFilter.tos       != pPtr->tos )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_RXIF )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_RXIF)    || (ifname && strcmp(ifname, pPtr->rxif)!=0) )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_MAC )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_MAC)     || ppa_memcmp( stFilter.srcmac, pPtr->srcmac, sizeof(pPtr->srcmac) ) != 0 )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_SRCIP )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_SRCIP)   || stFilter.src_ip.ip != pPtr->src_ip.ip )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_DSTIP )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_DSTIP)   || stFilter.dst_ip.ip != pPtr->dst_ip.ip )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_SRCIPV6 )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_SRCIPV6) || ppa_memcmp( stFilter.src_ip.ip6, pPtr->src_ip.ip6, sizeof(pPtr->src_ip.ip6) ) != 0 )
			continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_DSTIPV6 )
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_DSTIPV6) || ppa_memcmp( stFilter.dst_ip.ip6, pPtr->dst_ip.ip6, sizeof(pPtr->dst_ip.ip6) ) != 0 )
				continue;
		if ( pPtr->mask & ARC_PPA_FILTER_MASK_PKTDATA) {
			if ( !(stFilter.mask & ARC_PPA_FILTER_MASK_PKTDATA) )
				continue;
			for ( cnt2 = 0; cnt2 < ARC_PPA_FILTER_PKTDATA_MAX; cnt2++ ) {
				if (pPtr->pd_type[cnt2] == 2 && pL2Pkt == pL3Pkt) // no L2 header
					break;
				switch ( pPtr->pd_type[cnt2] ) {
					case 2:	pPktData = pL2Pkt + pPtr->pd_off[cnt2];	break;
					case 3:	pPktData = pL3Pkt + pPtr->pd_off[cnt2];	break;
					case 4:	pPktData = pL4Pkt + pPtr->pd_off[cnt2];	break;
					default:	continue;
				}
				if ( pPktData > pPktEnd )
					break;
				if ( ( *pPktData & pPtr->pd_mask[cnt2] ) != pPtr->pd_val[cnt2] )
					break;
			}
			if ( cnt2 < ARC_PPA_FILTER_PKTDATA_MAX )
				continue;
		}

		pPtr->hit++;
		arc_ppa_filter_hit_total++;

		if (arc_ppa_filter_debug_level)
			printk( "PPA filter: hit id %d\n", cnt );

		return cnt;
	}

	arc_ppa_filter_miss_total++;

	if (arc_ppa_filter_debug_level)
		printk( "PPA filter: miss\n" );

	return -1;
}

int arc_ppa_filter_search( stArcPpaFilter* pData )
{
	int				cnt, cnt2;
	stArcPpaFilter*	pPtr;

	for ( cnt=0, pPtr=arc_ppa_filters; cnt < arc_ppa_filter_num ; cnt++, pPtr++ ) {
		if ( pPtr->mask != pData->mask )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_DSTPORT) && pData->dst_port != pPtr->dst_port )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_PROTO)   && pData->l4proto != pPtr->l4proto )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_SRCPORT) && pData->src_port != pPtr->src_port )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_PRIO)    && pData->prio != pPtr->prio )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_TOS)     && pData->tos != pPtr->tos )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_RXIF)    && arc_ppa_filter_strcmp( pData->rxif, pPtr->rxif ) != 0 )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_MAC)     && ppa_memcmp( pData->srcmac, pPtr->srcmac, sizeof(pPtr->srcmac) ) != 0 )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_SRCIP)   && pData->src_ip.ip != pPtr->src_ip.ip )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_DSTIP)   && pData->dst_ip.ip != pPtr->dst_ip.ip )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_SRCIPV6) && ppa_memcmp( pData->src_ip.ip6, pPtr->src_ip.ip6, sizeof(pPtr->src_ip.ip6) ) != 0 )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_DSTIPV6) && ppa_memcmp( pData->dst_ip.ip6, pPtr->dst_ip.ip6, sizeof(pPtr->dst_ip.ip6) ) != 0 )
			continue;
		if ( (pPtr->mask & ARC_PPA_FILTER_MASK_PKTDATA) ) {
			for ( cnt2 = 0; cnt2 < ARC_PPA_FILTER_PKTDATA_MAX; cnt2++ ) {
				if ( pData->pd_type[cnt2] != pPtr->pd_type[cnt2]
				  || ( pData->pd_type[cnt2] != 0 && pData->pd_off[cnt2]  != pPtr->pd_off[cnt2] )
				  || ( pData->pd_type[cnt2] != 0 && pData->pd_mask[cnt2] != pPtr->pd_mask[cnt2] )
				  || ( pData->pd_type[cnt2] != 0 && pData->pd_val[cnt2]  != pPtr->pd_val[cnt2] ) )
					break;
			}
			if ( cnt2 < ARC_PPA_FILTER_PKTDATA_MAX )
				continue;
		}
		return cnt;
	}

	return -1;
}

int arc_ppa_filter_add( stArcPpaFilter* pNew )
{
	int	id;

	if ( pNew == NULL || pNew->mask == 0 )
		return -1;

	if ( arc_ppa_filter_search( pNew ) >= 0 )
		return -2;

	if ( arc_ppa_filter_num == ARC_PPA_FILTER_MAX )
		return -3;

	id = arc_ppa_filter_num;
	arc_ppa_filter_num++;
	arc_ppa_filters[id] = *pNew;
	arc_ppa_filters[id].hit = 0;

	return id;
}

int arc_ppa_filter_del( int iId )
{
	if ( iId < 0 || iId >= arc_ppa_filter_num )
		return -1;

	if ( iId < (arc_ppa_filter_num-1) )
		ppa_memcpy( &arc_ppa_filters[iId], &arc_ppa_filters[iId+1], sizeof(stArcPpaFilter)*(arc_ppa_filter_num-iId-1) );

	arc_ppa_filter_num--;

	return 0;
}

int arc_ppa_filter_del_all( void )
{
	arc_ppa_filter_num = 0;

	arc_ppa_filter_clear_all();

	return 0;
}

int arc_ppa_filter_get( int iId, stArcPpaFilter* pBuf )
{
	if ( iId < 0 || iId >= arc_ppa_filter_num || pBuf == NULL )
		return -1;

	ppa_memcpy( pBuf, &arc_ppa_filters[iId], sizeof(stArcPpaFilter) );

	return 0;
}

EXPORT_SYMBOL( arc_ppa_filter_is_enable );
EXPORT_SYMBOL( arc_ppa_filter_enable );
EXPORT_SYMBOL( arc_ppa_filter_disable );
EXPORT_SYMBOL( arc_ppa_filter_get_debug_level );
EXPORT_SYMBOL( arc_ppa_filter_set_debug_level );
EXPORT_SYMBOL( arc_ppa_filter_add );
EXPORT_SYMBOL( arc_ppa_filter_del );
EXPORT_SYMBOL( arc_ppa_filter_del_all );
EXPORT_SYMBOL( arc_ppa_filter_get );
EXPORT_SYMBOL( arc_ppa_filter_match );
EXPORT_SYMBOL( arc_ppa_filter_search );
EXPORT_SYMBOL( arc_ppa_filter_cnt );
EXPORT_SYMBOL( arc_ppa_filter_hit );
EXPORT_SYMBOL( arc_ppa_filter_miss );
EXPORT_SYMBOL( arc_ppa_filter_clear );
EXPORT_SYMBOL( arc_ppa_filter_clear_all );


#endif /*CONFIG_ARC_PPA_FILTER*/

