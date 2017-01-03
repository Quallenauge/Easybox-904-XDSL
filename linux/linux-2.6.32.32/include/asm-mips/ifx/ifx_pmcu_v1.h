/****************************************
 * header file for pmcu
 ***************************************/
#include <asm/ifx/ifx_types.h>
/*ioctl identifier*/
#define IFX_PMCU_STATE_SET 0
#define IFX_PMCU_STATE_GET 1
#define IFX_PMCU_CONSTRAINTS_SET 3
#define IFX_PMCU_CONSTRAINTS_GET 4
#define IFX_PMCU_INFO_GET 5
#define IFX_PMCU_MODULE_STATE_SET 0
#define IFX_PMCU_MODULE_STATE_GET 1
#define IFX_PMCU_MODULE_CONSTRAINTS_SET 3
#define IFX_PMCU_MODULE_CONSTRAINTS_GET 4
#define IFX_PMCU_MODULE_INFO_GET 5

#define MAX_PMCU_MODULE    17
#define MAX_PMCU_SUB_MODULE 6

/*
#define IFX_SUCCESS                     1
#define IFX_ERROR                       0


#define IFX_int32_t unsigned long
#define IFX_return_t int
*/

typedef enum
{
   IFX_PMCU_MODULE_CPU    = 0,
   IFX_PMCU_MODULE_ETH    = 1,
   IFX_PMCU_MODULE_USB    = 2,
   IFX_PMCU_MODULE_DSL    = 3,
   IFX_PMCU_MODULE_WLAN   = 4,
   IFX_PMCU_MODULE_DECT   = 5,
   IFX_PMCU_MODULE_FXS    = 6,
   IFX_PMCU_MODULE_FXO    = 7,
   IFX_PMCU_MODULE_VE     = 8,
   IFX_PMCU_MODULE_PPE    = 9,
   IFX_PMCU_MODULE_SWITCH = 10,
   IFX_PMCU_MODULE_UART   = 11,
   IFX_PMCU_MODULE_SPI    = 12,
   IFX_PMCU_MODULE_SDIO   = 13,
   IFX_PMCU_MODULE_PCI    = 14,
   IFX_PMCU_MODULE_VLYNQ  = 15,
   IFX_PMCU_MODULE_DEU    = 16,
}  IFX_PMCU_MODULE_t;

/*
 *Definition of power management state.
 */
typedef enum
{
   IFX_PMCU_STATE_D0 = 0,
   IFX_PMCU_STATE_D1 = 1,
   IFX_PMCU_STATE_D2 = 2,
   IFX_PMCU_STATE_D3 = 3
} IFX_PMCU_STATE_t;

/*
 * Event passed to the PMCU
 */
typedef enum
{
   IFX_PMCU_EVENT_NONE = 0,
   IFX_PMCU_EVENT_STATE = 1,
   IFX_PMCU_EVENT_ACTIVATED = 2,
   IFX_PMCU_EVENT_DEACTIVATED = 3,
   IFX_PMCU_EVENT_EXT1 = 4,
   IFX_PMCU_EVENT_EXT2 = 5,
} IFX_PMCU_EVENT_t;

/*
 * Structure used to set a state.
 */
typedef struct
{
   IFX_PMCU_MODULE_t pmcuModule;
   char pmcuModuleNr;
   IFX_PMCU_STATE_t pmcuState;
} IFX_PMCU_MODULE_STATE_t;

/*
 * Structure used to report an information to the PMCU
 */
typedef struct
{
    IFX_PMCU_MODULE_t pmcuModule;
    char pmcuModuleNr;
    IFX_PMCU_STATE_t pmcuState;
    IFX_PMCU_EVENT_t pmcuEvent;
} IFX_PMCU_INFO_t;


typedef struct
{
  IFX_PMCU_MODULE_t pmcuModule;
  char pmcuModuleNr;
  IFX_uint32_t    (*ifx_pmcu_state_req) ( IFX_uint8_t pmcuModuleNr,IFX_PMCU_STATE_t pmcuState );
  IFX_PMCU_STATE_t (*ifx_pmcu_state_get) ( IFX_uint8_t pmcuModuleNr);
} IFX_PMCU_REGISTER_t;

typedef struct state_list_t
{
   IFX_PMCU_MODULE_STATE_t state;
   struct state_list_t* next;
}STATE_LIST_t;

typedef struct
{
    STATE_LIST_t* req;
    STATE_LIST_t* nallow;
}STATE_PREREQ_t;

typedef struct
{
   IFX_PMCU_STATE_t pmcuState;
   STATE_PREREQ_t state[4];/*4 states:D0,D1,D2,D3*/
   wait_queue_head_t inq, outq;
   	IFX_uint32_t   (*ifx_pmcu_state_req) ( IFX_uint8_t pmcuModuleNr,IFX_PMCU_STATE_t pmcuState );
   IFX_PMCU_STATE_t  (*ifx_pmcu_state_get) ( IFX_uint8_t pmcuModuleNr );
}PMCU_MAP_t;

typedef struct
{
    IFX_PMCU_MODULE_STATE_t Target;
    IFX_PMCU_MODULE_STATE_t Req;

}STATE_TRANSITION_MAP_t;

IFX_return_t ifx_pmcu_info (IFX_PMCU_INFO_t* pmcuInfo);
IFX_return_t ifx_pmcu_register (IFX_PMCU_REGISTER_t* pmcuRegister);
IFX_return_t ifx_pmcu_unregister (IFX_PMCU_REGISTER_t* pmcuUnregister);


