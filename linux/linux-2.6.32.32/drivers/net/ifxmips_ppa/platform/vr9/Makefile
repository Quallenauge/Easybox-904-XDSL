ifeq ($(CONFIG_IFX_PPA_DATAPATH),m)
 ifneq ($(CONFIG_IFX_PPA_D5),y)
  obj-y	+= a5/ e5/
 endif
  obj-y	+= d5/
else
  obj-$(CONFIG_IFX_PPA_A5)	+= a5/
  obj-$(CONFIG_IFX_PPA_E5)	+= e5/
  obj-$(CONFIG_IFX_PPA_D5)	+= d5/
endif
