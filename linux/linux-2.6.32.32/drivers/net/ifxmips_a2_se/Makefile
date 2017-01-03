#
# Makefile for the platform specific kernel interface routines under Linux.
#

obj-$(CONFIG_IFX_A2_SE) += ifxmips_a2_se.o

ifxmips_a2_se-objs   := ifxmips_atm_core.o
ifeq ($(CONFIG_AMAZON_SE),y)
  ifxmips_a2_se-objs += ifxmips_atm_amazon_se.o
endif
