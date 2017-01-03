obj-y += ifx_ppa_hook.o ifx_ppe_drv_wrapper.o ifx_ppa_stack_al.o ifx_ppa_api_tools.o
obj-$(CONFIG_IFX_PPA_API) += ifx_ppa_api.o

ifx_ppa_api-objs = 


ifeq ($(CONFIG_IFX_PPA_MFE),y)
ifx_ppa_api-objs += ifx_ppa_api_mfe.o
endif

ifeq ($(CONFIG_IFX_PPA_QOS),y)
ifx_ppa_api-objs += ifx_ppa_api_qos.o
endif

ifx_ppa_api-objs += ifx_ppa_api_mib.o ifx_ppa_api_core.o ifx_ppa_api_misc.o ifx_ppa_api_netif.o ifx_ppa_api_session.o

ifneq ($(CONFIG_IFX_PMCU),)
   ifx_ppa_api-objs += ifx_ppa_api_pwm.o ifx_ppa_api_pwm_logic.o
endif

ifeq ($(CONFIG_IFX_PPA_API_DIRECTPATH),y)
   ifx_ppa_api-objs += ifx_ppa_api_directpath.o
endif
ifeq ($(CONFIG_IFX_PPA_API_PROC),y)
  ifx_ppa_api-objs += ifx_ppa_api_proc.o
endif
ifeq ($(CONFIG_IFX_PPA_API_PROC),m)
  obj-$(CONFIG_IFX_PPA_API_PROC) += ifx_ppa_api_proc.o
endif

