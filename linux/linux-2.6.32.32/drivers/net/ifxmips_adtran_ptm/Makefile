#
# Makefile for the platform specific kernel interface routines under Linux.
#

obj-$(CONFIG_IFX_ADTRAN_PTM) += ifxmips_ptm.o

ifeq ($(CONFIG_VR9),y)
  #ifxmips_ptm-objs := ifxmips_ptm_vdsl.o ifxmips_ptm_vr9.o ifxmips_ptm_vr9_misc.o ifxmips_ptm_test.o
  ifxmips_ptm-objs := ifxmips_ptm_vr9.o ifxmips_ptm_vr9_misc.o ifxmips_ptm_test.o
endif
