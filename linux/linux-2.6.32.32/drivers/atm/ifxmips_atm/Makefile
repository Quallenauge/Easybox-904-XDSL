#
# Makefile for the platform specific kernel interface routines under Linux.
#

obj-$(CONFIG_IFX_ATM) += ifxmips_atm.o

ifxmips_atm-objs   := ifxmips_atm_core.o
ifeq ($(CONFIG_DANUBE),y)
  ifxmips_atm-objs += ifxmips_atm_danube.o
endif
ifeq ($(CONFIG_AMAZON_SE),y)
  ifxmips_atm-objs += ifxmips_atm_amazon_se.o
endif
ifeq ($(CONFIG_AR9),y)
  ifxmips_atm-objs += ifxmips_atm_ar9.o
endif
ifeq ($(CONFIG_VR9),y)
  ifxmips_atm-objs += ifxmips_atm_vr9.o
endif
