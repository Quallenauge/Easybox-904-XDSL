ifeq ($(CONFIG_IFX_PPA_DATAPATH),m)
 ifneq ($(CONFIG_IFX_PPA_D4),y)
  obj-y	+= a4/
 endif
  obj-y	+= d4/
else
  obj-$(CONFIG_IFX_PPA_A4)	+= a4/
  obj-$(CONFIG_IFX_PPA_D4)	+= d4/
endif
