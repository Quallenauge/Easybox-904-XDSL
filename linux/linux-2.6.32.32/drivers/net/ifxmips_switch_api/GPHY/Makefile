#
# Makefile for the switch api
#
EXTRA_CFLAGS := -I./include/switch_api

ifeq ($(CONFIG_VR9),y)
EXTRA_CFLAGS += -DVR9
endif

ifeq ($(CONFIG_GE_MODE),y)
EXTRA_CFLAGS += -DGE_MODE
endif

ifeq ($(CONFIG_FE_MODE),y)
EXTRA_CFLAGS += -DFE_MODE
endif

  obj-$(CONFIG_GPHY_DRIVER)     += drv_ethsw_gphy.o 

ifeq ($(CONFIG_VR9),y)
  drv_ethsw_gphy-objs   += ifxmips_vr9_gphy.o
#  drv_ethsw_gphy-objs   += gphy_ge_fw.o
#  drv_ethsw_gphy-objs   += gphy_fe_fw.o
endif

