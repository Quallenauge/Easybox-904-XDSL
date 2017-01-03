#
# Makefile for the platform specific kernel interface routines under Linux.
#

obj-$(CONFIG_IFX_PTM) += ifxmips_ptm.o

ifeq ($(CONFIG_DANUBE),y)
  ifxmips_ptm-objs := ifxmips_ptm_adsl.o ifxmips_ptm_danube.o
endif
ifeq ($(CONFIG_AMAZON_SE),y)
  ifxmips_ptm-objs := ifxmips_ptm_adsl.o ifxmips_ptm_amazon_se.o
endif
ifeq ($(CONFIG_AR9),y)
  ifxmips_ptm-objs := ifxmips_ptm_adsl.o ifxmips_ptm_ar9.o
endif
ifeq ($(CONFIG_VR9),y)
  ifxmips_ptm-objs := ifxmips_ptm_vdsl.o ifxmips_ptm_vr9.o
endif

ifeq ($(CONFIG_IFX_PTM_TEST_PROC),y)
  ifeq ($(CONFIG_IFX_PTM),y)
    ifxmips_ptm-objs += ifxmips_ptm_test.o
  else
    obj-$(CONFIG_IFX_PTM) += ifxmips_ptm_test.o
  endif
endif
