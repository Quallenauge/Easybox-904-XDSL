#
# Makefile for the switch api
#
EXTRA_CFLAGS := -I$(srctree)/include/switch_api

obj-$(CONFIG_IFX_ETHSW_API)	+= drv_switch_api.o
obj-$(CONFIG_GPHY_DRIVER)     += GPHY/

drv_switch_api-objs	:= ifx_ethsw_linux.o
drv_switch_api-objs	+= ifx_ethsw_init.o

##################### AR9 ######################
ifeq ($(CONFIG_AR9),y)
  EXTRA_CFLAGS += -DAR9
  drv_switch_api-objs   += ifx_ethsw_tantosG_api.o
  drv_switch_api-objs   += ifx_ethsw_PSB6970_ll_table.o
  drv_switch_api-objs   += ifx_ethsw_rml.o
  drv_switch_api-objs   += ifx_ethsw_ral.o
ifeq ($(CONFIG_IFX_ETHSW_API_COC),y)
  EXTRA_CFLAGS += -DIFX_ETHSW_API_COC
  drv_switch_api-objs	+= ifx_ethsw_pm.o
  drv_switch_api-objs	+= ifx_ethsw_pm_plat.o
ifeq ($(CONFIG_IFX_ETHSW_API_COC_PMCU),y)
  EXTRA_CFLAGS += -DIFX_ETHSW_API_COC_PMCU
  drv_switch_api-objs	+= ifx_ethsw_pm_pmcu.o
endif
endif
  drv_switch_api-objs   += AR9.o
  drv_switch_api-objs   += Tantos3G.o
endif

##################### Danube ######################
ifeq ($(CONFIG_DANUBE),y)
  EXTRA_CFLAGS += -DDANUBE
  drv_switch_api-objs   += ifx_ethsw_tantosG_api.o
  drv_switch_api-objs   += ifx_ethsw_PSB6970_ll_table.o
  drv_switch_api-objs   += ifx_ethsw_rml.o
  drv_switch_api-objs   += ifx_ethsw_ral_danube.o
ifeq ($(CONFIG_IFX_ETHSW_API_COC),y)
  EXTRA_CFLAGS += -DIFX_ETHSW_API_COC
  drv_switch_api-objs	+= ifx_ethsw_pm.o
  drv_switch_api-objs	+= ifx_ethsw_pm_plat.o
ifeq ($(CONFIG_IFX_ETHSW_API_COC_PMCU),y)
  EXTRA_CFLAGS += -DIFX_ETHSW_API_COC_PMCU
  drv_switch_api-objs	+= ifx_ethsw_pm_pmcu.o
endif
endif
  drv_switch_api-objs   += AR9.o
  drv_switch_api-objs   += Tantos3G.o
endif

##################### Amazon-SE ######################
ifeq ($(CONFIG_AMAZON_SE),y)
  EXTRA_CFLAGS += -DAMAZON_SE
ifeq ($(CONFIG_AMAZON_SE_TANTOS0G_GPIO),y)
  EXTRA_CFLAGS += -DAMAZON_SE_TANTOS0G_GPIO
endif
  drv_switch_api-objs   += ifx_ethsw_tantosG_api.o
  drv_switch_api-objs   += ifx_ethsw_PSB6970_ll_table.o
  drv_switch_api-objs   += ifx_ethsw_rml.o
  drv_switch_api-objs   += ifx_ethsw_ral_amazon_se.o
ifeq ($(CONFIG_IFX_ETHSW_API_COC),y)
  EXTRA_CFLAGS += -DIFX_ETHSW_API_COC
  drv_switch_api-objs	+= ifx_ethsw_pm.o
  drv_switch_api-objs	+= ifx_ethsw_pm_plat.o
ifeq ($(CONFIG_IFX_ETHSW_API_COC_PMCU),y)
  EXTRA_CFLAGS += -DIFX_ETHSW_API_COC_PMCU
  drv_switch_api-objs	+= ifx_ethsw_pm_pmcu.o
endif
endif
  drv_switch_api-objs   += AR9.o
  drv_switch_api-objs   += Tantos3G.o
endif

##################### VR9 ######################
ifeq ($(CONFIG_VR9),y)
  EXTRA_CFLAGS += -DVR9
  drv_switch_api-objs   += ifx_ethsw_core_platform_vr9.o
  drv_switch_api-objs   += ifx_ethsw_flow_api.o
  drv_switch_api-objs   += ifx_ethsw_flow_ll_table.o
  drv_switch_api-objs   += ifx_ethsw_pce.o
  drv_switch_api-objs   += ifx_ethsw_vr9_ral.o
  drv_switch_api-objs   += ifx_ethsw_vr9_reg_direct_access.o
endif

