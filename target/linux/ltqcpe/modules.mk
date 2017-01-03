
LTQCPE_MENU:=LTQCPE support
SUBTARGET_SUFFIX:=$(shell echo $(subst .,_,$(subst -,_,$(subst /,_,$(SUBTARGET)))) | cut -d_ -f2-)

define KernelPackage/ltqcpe_nand
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX NAND Support
 DEPENDS:=@TARGET_ltqcpe
 ### ctc ###
# KCONFIG:= \
#	CONFIG_MTD_NAND=y \
#	CONFIG_MTD_IFX_NAND=y \
#	CONFIG_MTD_NAND_VERIFY_WRITE=y \
#	CONFIG_MTD_IFX_NOR=n \
#	CONFIG_MTD_ROOTFS_SPLIT=n
 KCONFIG:= \
	CONFIG_MTD_NAND=y \
	CONFIG_MTD_IFX_NAND=y \
	CONFIG_MTD_NAND_VERIFY_WRITE=y \
	CONFIG_MTD_IFX_NOR=n
 ###########
endef
# CONFIG_MTD_NAND_VERIFY_WRITE enabled for some nand chips to do a stable write.
# Can be removed if performance is impacted.

define KernelPackage/ltqcpe_nand/description
  Kernel Support NAND 
endef

$(eval $(call KernelPackage,ltqcpe_nand))


define KernelPackage/ltqcpe_imq
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX Support for IMQ 
 KCONFIG:= \
	CONFIG_IMQ \
	CONFIG_IMQ_BEHAVIOR_AA=y \
	CONFIG_IMQ_NUM_DEVS=3 \
	CONFIG_NETFILTER_XT_TARGET_IMQ=y
FILES:=$(LINUX_DIR)/drivers/net/imq.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_imq/description
  Kernel Support IMQ 
endef

$(eval $(call KernelPackage,ltqcpe_imq))

define KernelPackage/ltqcpe_qos
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX Support for Qos
 #DEPENDS:= +ebtables +ebtables-utils 
 KCONFIG:= \
	CONFIG_IFX_IPQOS=y \
	CONFIG_NET_SCHED=y \
	CONFIG_NET_SCH_FIFO=y \
	CONFIG_NET_SCH_CLK_GETTIMEOFDAY=y \
	CONFIG_NET_SCH_CBQ=y \
	CONFIG_NET_SCH_HTB=y \
	CONFIG_NET_SCH_RED=y \
	CONFIG_NET_SCH_DSMARK=y \
	CONFIG_NET_CLS=y \
	CONFIG_NET_CLS_FW=y \
	CONFIG_NET_CLS_U32=y \
	CONFIG_NETFILTER_XT_MATCH_U32=y \
	CONFIG_IFX_ALG_QOS=y \
	CONFIG_IP_NF_TARGET_TOS=y \
	CONFIG_NETFILTER_XT_TARGET_DSCP=y

endef

define KernelPackage/ltqcpe_qos/description
  Kernel Support for QoS. This package enables classifier and queuing disciplines (HTB, CBQ, FIFO etc) in Kernel configuration.
endef

$(eval $(call KernelPackage,ltqcpe_qos))

define KernelPackage/ltqcpe_pci
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX Supoort PCI 
 KCONFIG:= \
	CONFIG_PCI=y \
	CONFIG_IFX_PCI=y \
	CONFIG_IFX_PCI_HW_SWAP=y \
	CONFIG_IFX_PCI_INTERNAL_CLK_SRC_33=y
endef

define KernelPackage/ltqcpe_pci/description
  Kernel Support IFX PCI 
endef

$(eval $(call KernelPackage,ltqcpe_pci))


define KernelPackage/ltqcpe_switchapi
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX Switch API Support
 DEPENDS:=@TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9||TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_amazon_se
 KCONFIG:= \
	CONFIG_IFX_ETHSW_API=y \
	CONFIG_EXTERNAL_T3G=y
endef

define KernelPackage/ltqcpe_switchapi/description
 Kernel support for IFX Switch API
endef

$(eval $(call KernelPackage,ltqcpe_switchapi))

define KernelPackage/ltqcpe_vr9_7port
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=VR9 7PORT switch support
 DEPENDS:=@TARGET_ltqcpe_platform_vr9
 KCONFIG:= \
	CONFIG_IFX_7PORT_SWITCH=y
endef

define KernelPackage/ltqcpe_vr9_7port/description
Kernel support for VR9 7 port switch
endef

$(eval $(call KernelPackage,ltqcpe_vr9_7port))

define KernelPackage/ltqcpe_eth_drv_builtin
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:= IFX Ethernet Driver (Built-in Support)
 DEPENDS:= @!PACKAGE_kmod-ltqcpe_eth_drv_mod
 KCONFIG:= \
	CONFIG_IFX_7PORT_SWITCH=y \
	CONFIG_IFX_3PORT_SWITCH=y \
	CONFIG_IFX_ETOP_ETHERNET=y
endef

define KernelPackage/ltqcpe_eth_drv_builtin/description
 Kernel support for IFX Ethernet Driver (Built-in Support)
endef

$(eval $(call KernelPackage,ltqcpe_eth_drv_builtin))

define KernelPackage/ltqcpe_eth_drv_mod
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:= IFX Ethernet Driver (Loadable Module Support)
 DEPENDS:=@!PACKAGE_kmod-fs-nfs-root
 KCONFIG:= \
	CONFIG_IFX_7PORT_SWITCH=m \
	CONFIG_IFX_3PORT_SWITCH=m \
	CONFIG_IFX_ETOP_ETHERNET=m
  FILES:=$(LINUX_DIR)/drivers/net/ifxmips_eth_drv.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_eth_drv_mod/description
 Kernel support for IFX Ethernet Driver (Loadable Module Support)
endef

$(eval $(call KernelPackage,ltqcpe_eth_drv_mod))


define KernelPackage/ltqcpe_dma_descr
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=DMA DESCRITPTOR NUMBER
 KCONFIG:= \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64
endef

define KernelPackage/ltqcpe_dma_descr/description
 Configure DMA DESCRIPTOR NUMBER
endef

$(eval $(call KernelPackage,ltqcpe_dma_descr))

define KernelPackage/ltqcpe_ase_optimization
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=amazon_se ram optimizations
 DEPENDS:=@TARGET_ltqcpe_platform_amazon_se
 KCONFIG:= \
	CONFIG_IFX_A2_DESC_NUM=y
endef

define KernelPackage/ltqcpe_ase_optimization/description
 Configure descriptors
endef

$(eval $(call KernelPackage,ltqcpe_ase_optimization))

define KernelPackage/ltqcpe_spi
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX SPI Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
	CONFIG_SPI=y \
	CONFIG_SPI_DEBUG=n \
	CONFIG_SPI_MASTER=y \
	CONFIG_SPI_BITBANG=n \
	CONFIG_IFX_SPI=y \
	CONFIG_IFX_SPI_DEBUG=n \
	CONFIG_IFX_SPI_ASYNCHRONOUS=y \
	CONFIG_IFX_SPI_EEPROM=y \
	CONFIG_IFX_SPI_AT45_FLASH=n 
endef

define KernelPackage/ltqcpe_spi/description
 Kernel support for IFX SPI 
endef

$(eval $(call KernelPackage,ltqcpe_spi))

define KernelPackage/ltqcpe_spi_flash
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX SPI flash Support
 DEPENDS:=@TARGET_ltqcpe
 ### ctc ###
# KCONFIG:= \
#	CONFIG_IFX_SPI_EEPROM=n \
#	CONFIG_IFX_SPI_FLASH=y \
#	CONFIG_MTD_ROOTFS_SPLIT=y \
#	CONFIG_MTD_IFX_NOR=n
 KCONFIG:= \
	CONFIG_IFX_SPI_EEPROM=n \
	CONFIG_IFX_SPI_FLASH=y \
	CONFIG_MTD_IFX_NOR=n
 ###########
endef

define KernelPackage/ltqcpe_spi_flash/description
 Kernel support for IFX SPI Flash
endef

$(eval $(call KernelPackage,ltqcpe_spi_flash))

define KernelPackage/ltqcpe_complex_mtd
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX Complex MTD Mappings Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
 	CONFIG_MTD_COMPLEX_MAPPINGS=y
endef

define KernelPackage/ltqcpe_complex_mtd/description
 Kernel support for Complex MTD Mappings
endef

$(eval $(call KernelPackage,ltqcpe_complex_mtd))

define KernelPackage/ltqcpe_nor
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX NOR Support
 DEPENDS:=@TARGET_ltqcpe
 ### ctc ###
# KCONFIG:= \
#	CONFIG_MTD_IFX_NOR=y \
#	CONFIG_MTD_ROOTFS_SPLIT=y
 KCONFIG:= \
	CONFIG_MTD_IFX_NOR=y
 ###########
endef

define KernelPackage/ltqcpe_nor/description
 Kernel support for IFX NOR
endef

$(eval $(call KernelPackage,ltqcpe_nor))


define KernelPackage/atm_stack
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=ATM stack
  DEPENDS:=@TARGET_ltqcpe
  KCONFIG:= \
	CONFIG_ATM=y \
	CONFIG_ATM_CLIP=y \
	CONFIG_ATM_DRIVERS=y \
	CONFIG_ATM_BR2684=y \
	CONFIG_ATM_MPOA=y \
	CONFIG_PPPOATM=y \
	CONFIG_ATM_LANE=y \
	CONFIG_IFX_OAM=y
endef

define KernelPackage/atm_stack/description
 Kernel built-in support for ATM stack
endef

$(eval $(call KernelPackage,atm_stack))


define KernelPackage/ltqcpe_atm_builtin
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IFX ATM Support (built-in)
  DEPENDS:=@!PACKAGE_kmod-ltqcpe_atm_mod @TARGET_ltqcpe +kmod-atm_stack
  KCONFIG:= \
	CONFIG_IFX_ATM=y \
	CONFIG_IFX_ATM_TASKLET=y
endef

define KernelPackage/ltqcpe_atm_builtin/description
 Kernel support for IFX ATM - built-in
endef

$(eval $(call KernelPackage,ltqcpe_atm_builtin))

define KernelPackage/ltqcpe_atm_retx
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IFX ATM ReTransmission Support
  DEPENDS:=@PACKAGE_kmod-ltqcpe_atm_builtin||PACKAGE_kmod-ltqcpe_atm_mod
  KCONFIG:= \
	CONFIG_IFX_ATM_RETX=y
endef

define KernelPackage/ltqcpe_atm_retx/description
 Kernel support for IFX ATM ReTransmission 
endef

$(eval $(call KernelPackage,ltqcpe_atm_retx))

define KernelPackage/ltqcpe_atm_mod
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IFX ATM Support (pluggable module)
  DEPENDS:=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_ATM \
	CONFIG_IFX_ATM_TASKLET=y
  FILES:=$(LINUX_DIR)/drivers/atm/ifxmips_atm/ifxmips_atm.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_atm_mod/description
 Kernel module for IFX ATM - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_atm_mod))


define KernelPackage/ltqcpe_ptm_builtin
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IFX PTM support
  DEPENDS:=@!PACKAGE_kmod-ltqcpe_ptm_mod @TARGET_ltqcpe
  KCONFIG:= \
	CONFIG_ATM=y \
	CONFIG_ATM_DRIVERS=y \
	CONFIG_IFX_PTM=y
endef

define KernelPackage/ltqcpe_ptm_builtin/description
 Kernel built-in PTM support
endef

$(eval $(call KernelPackage,ltqcpe_ptm_builtin))

define KernelPackage/ltqcpe_ptm_mod
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=PTM support
  DEPENDS:=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe
  KCONFIG:= \
	CONFIG_ATM=y \
	CONFIG_ATM_DRIVERS=y \
	CONFIG_IFX_PTM
  FILES:=$(LINUX_DIR)/drivers/net/ifxmips_ptm/ifxmips_ptm.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ptm_mod/description
 Kernel module for PTM support
endef

$(eval $(call KernelPackage,ltqcpe_ptm_mod))


define KernelPackage/ltqcpe_ppa
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX PPA Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
	CONFIG_IFX_PPA=y \
	CONFIG_IFX_PPA_API=y \
	CONFIG_IFX_PPA_API_DIRECTPATH=y \
	CONFIG_IFX_PPA_API_DIRECTPATH_BRIDGING=y \
	CONFIG_IFX_PPA_API_PROC=y \
	CONFIG_IFX_PPA_DATAPATH=y
endef

define KernelPackage/ltqcpe_ppa/description
 Kernel support for PPA acceleration
endef

### ctc ###
$(eval $(call KernelPackage,ltqcpe_ppa))
###########


define KernelPackage/ltqcpe_ppa_mod
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX PPA Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
	CONFIG_IFX_PPA=y \
	CONFIG_IFX_PPA_API \
	CONFIG_IFX_PPA_API_DIRECTPATH=y \
	CONFIG_IFX_PPA_API_DIRECTPATH_BRIDGING=y \
	CONFIG_IFX_PPA_API_PROC \
	CONFIG_IFX_PPA_DATAPATH
 FILES:= \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/ppa_api/ifx_ppa_api.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/ppa_api/ifx_ppa_api_proc.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_mod/description
 Kernel support for PPA acceleration
endef

define KernelPackage/ltqcpe_ppa_a2_builtin
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IFX PPA built-in A2 mode
  DEPENDS:=@TARGET_ltqcpe_platform_amazon_se +kmod-atm_stack
  KCONFIG:= \
	CONFIG_IFX_ETOP_ETHERNET=n \
	CONFIG_IFX_A2_SE=y \
	CONFIG_IFX_EPHY_MODE=y \
	CONFIG_IFX_ATM_TASKLET=y \
	CONFIG_IFX_ETH_NAPI=y
endef

define KernelPackage/ltqcpe_ppa_a2_builtin/description
  PPA A2 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_a2_builtin))

define KernelPackage/ltqcpe_ppa_a2_mod
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IFX PPA module A2 mode
  DEPENDS:=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_amazon_se +kmod-atm_stack
  KCONFIG:= \
	CONFIG_IFX_A2_SE \
	CONFIG_IFX_EPHY_MODE=y \
	CONFIG_IFX_ATM_TASKLET=y \
	CONFIG_IFX_ETH_NAPI=y
  FILES:=$(LINUX_DIR)/drivers/net/ifxmips_a2_se/ifxmips_a2_se.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_a2_mod/description
  PPA A2 Mode - mod
endef

$(eval $(call KernelPackage,ltqcpe_ppa_a2_mod))

define KernelPackage/ltqcpe_ppa_a5_builtin
  $(call KernelPackage/ltqcpe_ppa)
  TITLE+=(A5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-ltqcpe_ppa_a5_mod @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9 +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64 \
	CONFIG_IFX_PPA_A5=y
endef

define KernelPackage/ltqcpe_ppa_a5_builtin/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA A5 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_a5_builtin))


define KernelPackage/ltqcpe_ppa_a5_mod
  $(call KernelPackage/ltqcpe_ppa_mod)
  TITLE+=(A5 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9 +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
  	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64 \
	CONFIG_IFX_PPA_A5=y
  FILES += \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/a5/ifxmips_ppa_datapath_$(SUBTARGET_SUFFIX)_a5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/a5/ifxmips_ppa_hal_$(SUBTARGET_SUFFIX)_a5.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_a5_mod/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA A5 Mode - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_ppa_a5_mod))


define KernelPackage/ltqcpe_ppa_d5_builtin
  $(call KernelPackage/ltqcpe_ppa)
  TITLE+=(D5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-ltqcpe_ppa_d5_mod @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64 \
	CONFIG_IFX_PPA_D5=y
endef

define KernelPackage/ltqcpe_ppa_d5_builtin/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA D5 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_d5_builtin))


define KernelPackage/ltqcpe_ppa_d5_mod
  $(call KernelPackage/ltqcpe_ppa_mod)
  TITLE+=(D5 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64 \
	CONFIG_IFX_PPA_D5=y
  FILES += \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/d5/ifxmips_ppa_datapath_$(SUBTARGET_SUFFIX)_d5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/d5/ifxmips_ppa_hal_$(SUBTARGET_SUFFIX)_d5.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_d5_mod/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA D5 Mode - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_ppa_d5_mod))


define KernelPackage/ltqcpe_ppa_e5_builtin
  $(call KernelPackage/ltqcpe_ppa)
  TITLE+=(E5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-ltqcpe_ppa_e5_mod @TARGET_ltqcpe_platform_ar9||@TARGET_ltqcpe_platform_vr9 +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64 \
	CONFIG_IFX_PPA_E5=y
endef

define KernelPackage/ltqcpe_ppa_e5_builtin/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA E5 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_e5_builtin))


define KernelPackage/ltqcpe_ppa_e5_mod
  $(call KernelPackage/ltqcpe_ppa_mod)
  TITLE+=(E5 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_ar9||@TARGET_ltqcpe_platform_vr9 +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64 \
	CONFIG_IFX_PPA_E5=y
  FILES += \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/e5/ifxmips_ppa_datapath_$(SUBTARGET_SUFFIX)_e5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/e5/ifxmips_ppa_hal_$(SUBTARGET_SUFFIX)_e5.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_e5_mod/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA E5 Mode - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_ppa_e5_mod))


define KernelPackage/ltqcpe_ppa_d4_builtin
  $(call KernelPackage/ltqcpe_ppa)
  TITLE+=(D4 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-ltqcpe_ppa_d4_mod @TARGET_ltqcpe_platform_danube @!PACKAGE_kmod-ltqcpe_ppa_a4_builtin
  KCONFIG += \
	CONFIG_IFX_PPA_D4=y
endef

define KernelPackage/ltqcpe_ppa_d4_builtin/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA D4 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_d4_builtin))


define KernelPackage/ltqcpe_ppa_d4_mod
  $(call KernelPackage/ltqcpe_ppa_mod)
  TITLE+=(D4 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_danube @!PACKAGE_kmod-ltqcpe_ppa_a4
 #disabling d4 module, since a4.ko is being overwritten by d4.ko
 # KCONFIG += \
	CONFIG_IFX_PPA_D4=y
  FILES += \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/d4/ifxmips_ppa_datapath_$(SUBTARGET_SUFFIX)_d4.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/d4/ifxmips_ppa_hal_$(SUBTARGET_SUFFIX)_d4.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/d4/ifxmips_ppa_lite_datapath_danube_d4.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_d4_mod/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA D4 Mode - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_ppa_d4_mod))


define KernelPackage/ltqcpe_ppa_a4_builtin
  $(call KernelPackage/ltqcpe_ppa)
  TITLE+=(A4 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-ltqcpe_ppa_a4_mod @TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_amazon_se +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_PPA_A4=y
endef

define KernelPackage/ltqcpe_ppa_a4_builtin/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA A4 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_a4_builtin))


define KernelPackage/ltqcpe_ppa_a4_mod
  $(call KernelPackage/ltqcpe_ppa_mod)
  TITLE+=(A4 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_amazon_se +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_PPA_A4=y
  FILES += \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/a4/ifxmips_ppa_datapath_$(SUBTARGET_SUFFIX)_a4.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/a4/ifxmips_ppa_hal_$(SUBTARGET_SUFFIX)_a4.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_a4_mod/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA A4 Mode - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_ppa_a4_mod))

define KernelPackage/ltqcpe_ppa_e4_builtin
  $(call KernelPackage/ltqcpe_ppa)
  TITLE+=(E4 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-ltqcpe_ppa_e4_mod @TARGET_ltqcpe_platform_amazon_se +kmod-atm_stack
endef

define KernelPackage/ltqcpe_ppa_e4_builtin/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA E4 Mode - built-in
endef

$(eval $(call KernelPackage,ltqcpe_ppa_e4_builtin))


define KernelPackage/ltqcpe_ppa_e4_mod
  $(call KernelPackage/ltqcpe_ppa_mod)
  TITLE+=(E4 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @TARGET_ltqcpe_platform_amazon_se +kmod-atm_stack
  FILES += \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/e4/ifxmips_ppa_datapath_$(SUBTARGET_SUFFIX)_e4.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/ifxmips_ppa/platform/$(SUBTARGET_SUFFIX)/e4/ifxmips_ppa_hal_$(SUBTARGET_SUFFIX)_e4.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_ppa_e4_mod/description
  $(call KernelPackage/ltqcpe_ppa/description)
  PPA E4 Mode - pluggable module
endef

$(eval $(call KernelPackage,ltqcpe_ppa_e4_mod))


define KernelPackage/ltqcpe_usb3
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=Lantiq USB 3.0 support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
	CONFIG_USB_HOST_IFX_XHCI=m \
	CONFIG_USB_SUPPORT=y \
	CONFIG_USB_ARCH_HAS_HCD=y \
	CONFIG_USB=y \
	CONFIG_PCI=y

 FILES:=$(LINUX_DIR)/drivers/usb/host/xhci.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_usb3/description
 Kernel module for Lantiq USB 3.0 XHCI package
endef

$(eval $(call KernelPackage,ltqcpe_usb3))


define KernelPackage/ltqcpe_usb_host_base
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=Lantiq USB Host support
 DEPENDS:=@TARGET_ltqcpe @+FEATURE_IFX_USB_HOST
 KCONFIG:= \
	CONFIG_USB_HOST_IFX=m \
	CONFIG_USB_SUPPORT=y \
	CONFIG_USB_ARCH_HAS_HCD=y \
	CONFIG_USB_ARCH_HAS_OHCI=y \
	CONFIG_USB_ARCH_HAS_EHCI=y \
	CONFIG_USB=y \
	CONFIG_PCI=y \
	CONFIG_USB_HOST_IFX_UNALIGNED_ADJ=y \
	CONFIG_USB_LTQ_BULK_TESTER=m
 
 FILES:=$(LINUX_DIR)/drivers/usb/host/ifxusb_host.$(LINUX_KMOD_SUFFIX)
endef


define KernelPackage/ltqcpe_usb_host_base/description
 Kernel module for Lantiq USB Host base package
endef


define KernelPackage/ltqcpe_usb_host
 $(call KernelPackage/ltqcpe_usb_host_base)
 TITLE+= on Both USB ports
 KCONFIG += \
	CONFIG_USB_HOST_IFX_B=y
endef

define KernelPackage/ltqcpe_usb_host/description
 Kernel module for Lantiq USB Host support on both USB ports
endef

$(eval $(call KernelPackage,ltqcpe_usb_host))

define KernelPackage/ltqcpe_usb_host_port1
 $(call KernelPackage/ltqcpe_usb_host_base)
 TITLE+= only on Port 1
 KCONFIG += \
	CONFIG_USB_HOST_IFX_1=y
endef

define KernelPackage/ltqcpe_usb_host_port1/description
 Kernel module for Lantiq USB Host support on Port 1
endef

$(eval $(call KernelPackage,ltqcpe_usb_host_port1))

define KernelPackage/ltqcpe_usb_host_port2
 $(call KernelPackage/ltqcpe_usb_host_base)
 TITLE+= only on Port 2
 KCONFIG += \
	CONFIG_USB_HOST_IFX_2=y
endef

define KernelPackage/ltqcpe_usb_host_port2/description
 Kernel module for Lantiq USB Host support on Port 2
endef

$(eval $(call KernelPackage,ltqcpe_usb_host_port2))

define KernelPackage/ltqcpe_usb_device_base
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=Lantiq USB Device support
 DEPENDS:=@TARGET_ltqcpe @+FEATURE_IFX_USB_DEVICE
 KCONFIG:= \
	CONFIG_USB_GADGET=m \
	CONFIG_USB_SUPPORT=y \
	CONFIG_USB_GADGET_SELECTED=y \
	CONFIG_USB_GADGET_IFX=m \
	CONFIG_USB_GADGET_DUALSPEED=y \
	CONFIG_USB_ETH=m \
	CONFIG_USB_ETH_RNDIS=y
 FILES:= \
	$(LINUX_DIR)/drivers/usb/gadget/ifxusb_gadget.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/usb/gadget/g_ether.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_usb_device_base/description
 Kernel module for Lantiq USB Device support
endef

define KernelPackage/ltqcpe_usb_device
 $(call KernelPackage/ltqcpe_usb_device_base)
 TITLE+= on USB port
 KCONFIG += \
	CONFIG_USB_GADGET_IFX_0=y
endef

define KernelPackage/ltqcpe_usb_device/description
 Kernel module for Lantiq USB device support on USB port if device has only one port
endef

$(eval $(call KernelPackage,ltqcpe_usb_device))

define KernelPackage/ltqcpe_usb_device_port1
 $(call KernelPackage/ltqcpe_usb_device_base)
 TITLE+= only on Port 1 
 KCONFIG += \
	CONFIG_USB_GADGET_IFX_1=y
endef

define KernelPackage/ltqcpe_usb_device_port1/description
 Kernel module for Lantiq USB device support on Port 1 
endef

$(eval $(call KernelPackage,ltqcpe_usb_device_port1))

define KernelPackage/ltqcpe_usb_device_port2
 $(call KernelPackage/ltqcpe_usb_device_base)
 TITLE+= only on Port 2
 KCONFIG += \
	CONFIG_USB_GADGET_IFX_2=y
endef

define KernelPackage/ltqcpe_usb_device_port2/description
 Kernel module for Lantiq USB device support on Port 2
endef

$(eval $(call KernelPackage,ltqcpe_usb_device_port2))

define KernelPackage/ltqcpe_usb_class_drivers
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=Lantiq USB Class Drivers Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
	CONFIG_USB_SUPPORT=y \
	CONFIG_USB_PRINTER=y \
	CONFIG_USB_PEGASUS=y \
	CONFIG_USB_STORAGE=y \
	CONFIG_USB_STORAGE_DATAFAB=y \
	CONFIG_USB_STORAGE_FREECOM=y \
	CONFIG_USB_STORAGE_DPCM=y \
	CONFIG_USB_STORAGE_USBAT=y \
	CONFIG_USB_STORAGE_SDDR09=y \
	CONFIG_USB_STORAGE_SDDR55=y \
	CONFIG_USB_STORAGE_JUMPSHOT=y \
	CONFIG_USB_STORAGE_ALAUDA=y \
	CONFIG_SCSI=y \
	CONFIG_BLK_DEV_SD=y
endef

define KernelPackage/ltqcpe_usb_class_drivers/description
 Kernel package for Lantiq USB Class Drivers Support
endef

$(eval $(call KernelPackage,ltqcpe_usb_class_drivers))


#define KernelPackage/ltqcpe_mei_core
#  SUBMENU:=$(LTQCPE_MENU)
#  TITLE:=DSL support
#  DEPENDS:=@TARGET_ltqcpe
#  KCONFIG:= CONFIG_IFXMIPS_DSL_CPE_MEI=y
#endef

#define KernelPackage/ltqcpe_mei_core/description
# Kernel module for DSL support
#endef

#$(eval $(call KernelPackage,ltqcpe_mei_core))


define KernelPackage/ltqcpe_pcie
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX VRX PCIe Support
 DEPENDS:=@TARGET_ltqcpe_platform_vr9
 KCONFIG:= \
	CONFIG_HW_HAS_PCI=y \
	CONFIG_PCI=y \
	CONFIG_PCIEPORTBUS=y \
	CONFIG_IFX_PCIE=y \
	CONFIG_IFX_PCIE_PHY_36MHZ_MODE=y \
	CONFIG_IFX_PCIE_HW_SWAP=y \
	CONFIG_MMU=y
endef

define KernelPackage/ltqcpe_pcie/description
 Kernel support for IFX XRX288 PCIe bus
endef

$(eval $(call KernelPackage,ltqcpe_pcie))


define KernelPackage/ltqcpe_pcie_enable_ssc
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=VRX PCIe spread spectrum Support
 DEPENDS:=@TARGET_ltqcpe_platform_vr9
 KCONFIG:= \
	CONFIG_IFX_PCIE_PHY_36MHZ_MODE=n \
	CONFIG_IFX_PCIE_PHY_36MHZ_SSC_MODE=y
endef

define KernelPackage/ltqcpe_pcie_enable_ssc/description
 Kernel support for VRX PCIe spread spectrum Support
endef

$(eval $(call KernelPackage,ltqcpe_pcie_enable_ssc))


define KernelPackage/ltqcpe_pcie_grx
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=IFX VRX PCIe Support for GRX variants
 DEPENDS:=@TARGET_ltqcpe_platform_vr9
 KCONFIG:= \
	CONFIG_HW_HAS_PCI=y \
	CONFIG_PCI=y \
	CONFIG_PCIEPORTBUS=y \
	CONFIG_IFX_PCIE=y \
	CONFIG_IFX_PCIE_PHY_25MHZ_MODE=y \
	CONFIG_IFX_PCIE_HW_SWAP=y \
	CONFIG_MMU=y
endef

define KernelPackage/ltqcpe_pcie_grx/description
 Kernel support for IFX XRX288 PCIe bus for grx variants
endef

$(eval $(call KernelPackage,ltqcpe_pcie_grx))

FS_MENU:=Filesystems

define KernelPackage/fs-nfs-root
  SUBMENU:=$(FS_MENU)
  TITLE:=NFS root support
  KCONFIG:= \
	CONFIG_NFS_FS=y \
	CONFIG_IP_PNP=y \
	CONFIG_IP_PNP_DHCP=y \
	CONFIG_IP_PNP_BOOTP=y \
	CONFIG_IP_PNP_RARP=y \
	CONFIG_LOCKD=y \
	CONFIG_ROOT_NFS=y \
	CONFIG_RPCSEC_GSS_KRB5=y \
	CONFIG_SUNRPC=y \
	CONFIG_SUNRPC_GSS=y

endef

define KernelPackage/fs-nfs-root/description
 Enables kernel settings for NFS root support
endef

$(eval $(call KernelPackage,fs-nfs-root))


define KernelPackage/ltqcpe_iproxyd
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IGMP Proxy & Snooping support
  DEPENDS:=@TARGET_ltqcpe @!PACKAGE_kmod-lqcpe_igmpd
  KCONFIG:= \
	CONFIG_IP_MULTICAST=y \
	CONFIG_IP_MROUTE=y \
	CONFIG_IFX_IGMP_PROXY=m
	
 FILES:= \
	$(LINUX_DIR)/net/ipv4/iproxyd/iproxyd.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ltqcpe_iproxyd/description
 Kernel module for IGMP Proxy & Snooping support
endef

$(eval $(call KernelPackage,ltqcpe_iproxyd))

define KernelPackage/lqcpe_igmpd
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=IGMPv3 Proxy & Snooping support
  DEPENDS:=@TARGET_ltqcpe
  KCONFIG:= \
	CONFIG_IP_MULTICAST=y \
	CONFIG_IP_MROUTE=y
endef

define KernelPackage/lqcpe_igmpd/description
  User daemon for IGMPv3 Proxy & Snooping support
endef

$(eval $(call KernelPackage,lqcpe_igmpd))

define KernelPackage/ltqcpe_mmc
   SUBMENU:=$(LTQCPE_MENU)
   TITLE:=MMC/SD Card Support
   DEPENDS:=@TARGET_ltqcpe_platform_amazon_se
   KCONFIG:= \
	CONFIG_MMC=y \
	CONFIG_MMC_BLOCK=y \
	CONFIG_MMC_DEBUG=n \
	CONFIG_MMC_UNSAFE_RESUME=n \
	CONFIG_MMC_BLOCK_BOUNCE=y \
	CONFIG_MMC_SDHCI=n \
	CONFIG_MMC_TIFM_SD=n \
	CONFIG_MMC_WBSD=n \
	CONFIG_SDIO_UART=n \
	CONFIG_IFX_MMC=y \
	CONFIG_IFX_MMC_BLOCK_DEV=n \
	CONFIG_IFX_MMC_WLAN=y
endef

define KernelPackage/mmc/description
 Kernel support for MMC/SD cards
endef

$(eval $(call KernelPackage,ltqcpe_mmc))

define KernelPackage/ipv6-kernel
  SUBMENU:=Netfilter Extensions
  TITLE:=Extra IPv6 kernel options
  KCONFIG:= \
	CONFIG_IPV6=y \
	CONFIG_IPV6_ROUTER_PREF=y \
	CONFIG_IPV6_ROUTE_INFO=y \
	CONFIG_NF_CONNTRACK_IPV6=y \
	CONFIG_IP6_NF_QUEUE=n
endef

define KernelPackage/ipv6-kernel/description
  Enable extra IPv6 kernel options
endef

$(eval $(call KernelPackage,ipv6-kernel))

define KernelPackage/nf_alg
 SUBMENU:=Netfilter Extensions
 TITLE:=ALG support for SIP, H323
 KCONFIG:= \
	CONFIG_NF_CONNTRACK_H323=y \
	CONFIG_NF_CONNTRACK_SIP=y
endef

define KernelPackage/nf_alg/description
  ALG support for SIP, H323
endef

$(eval $(call KernelPackage,nf_alg))

define KernelPackage/wan-vlan
 SUBMENU:=Netfilter Extensions
 TITLE:=Enable WAN VLAN support in kernel
 KCONFIG:= \
	CONFIG_WAN_VLAN_SUPPORT=y
endef

define KernelPackage/wan-vlan/description
 Enable WAN VLAN support in kernel also enable WAN VLAN support in br2684
endef

$(eval $(call KernelPackage,wan-vlan))

define KernelPackage/mroute6
 SUBMENU:=Netfilter Extensions
 TITLE:=Enable mroute for IPv6 in kernel
 KCONFIG:= \
	CONFIG_IPV6_MROUTE=y \
	CONFIG_IPV6_PIMSM_V2=y
endef

define KernelPackage/mroute6/description
 Enable mroute for IPv6 in kernel
endef

$(eval $(call KernelPackage,mroute6))

define KernelPackage/jffs2
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=JFFS2 FS Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
        CONFIG_JFFS2_FS=y
endef

define KernelPackage/jffs2/description
  Kernel Built-in support for JFFS2 File System
endef

$(eval $(call KernelPackage,jffs2))

define KernelPackage/ubi_mtd
 SUBMENU:=$(LTQCPE_MENU)
 TITLE:=UBI Support
 DEPENDS:=@TARGET_ltqcpe
 KCONFIG:= \
	CONFIG_MTD_UBI=y \
	CONFIG_MTD_UBI_WL_THRESHOLD=4096 \
	CONFIG_MTD_UBI_BEB_RESERVE=1 \
	CONFIG_MTD_UBI_GLUEBI=y \
	CONFIG_MTD_UBI_DEBUG=n
endef

define KernelPackage/ubi_mtd/description
  Kernel Built-in support for ubi and gluebi mtd
endef

$(eval $(call KernelPackage,ubi_mtd))

define KernelPackage/ubifs
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=UBI Filesystem Support
  DEPENDS:=@TARGET_ltqcpe +kmod-ubi_mtd
  KCONFIG:= \
	CONFIG_UBIFS_FS=y \
	CONFIG_UBIFS_FS_XATTR=n \
	CONFIG_UBIFS_FS_ADVANCED_COMPR=y \
	CONFIG_UBIFS_FS_LZO=y \
	CONFIG_UBIFS_FS_ZLIB=n \
	CONFIG_UBIFS_FS_DEBUG=n
endef

define KernelPackage/ubifs/description
  Kernel Built-in support for ubi filesystem
endef

$(eval $(call KernelPackage,ubifs))

define KernelPackage/esp_conntrack_mod
  SUBMENU:=Netfilter Extensions
  TITLE:=ESP Connection Tracking Module
  KCONFIG:= \
	CONFIG_NF_CT_PROTO_ESP=m

  FILES:=$(LINUX_DIR)/net/netfilter/nf_conntrack_proto_esp.$(LINUX_KMOD_SUFFIX) 
endef

define KernelPackage/esp_conntrack_mod/description
  ESP Connection Tracking Module
endef

$(eval $(call KernelPackage,esp_conntrack_mod))

define KernelPackage/ltqcpe_gw188_support
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=Kernel support for GW188 model
  KCONFIG:= \
	CONFIG_IFX_GW188=y \
	CONFIG_IFX_PCI_CLOCK_DELAY_NANO_SECONDS=3 \
	CONFIG_IFX_DUAL_MINI_PCI=y
endef

define KernelPackage/ltqcpe_gw188_support/description
  Kernel support option for GW188 model
endef

$(eval $(call KernelPackage,ltqcpe_gw188_support))

define KernelPackage/ltqcpe_rt288_support
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=Kernel support for RT288 model
  DEPENDS:=@TARGET_ltqcpe_platform_vr9_RT288
  KCONFIG:= \
	CONFIG_LTQ_RT288=y \
	CONFIG_IFX_DUAL_MINI_PCI=y
endef

define KernelPackage/ltqcpe_rt288_support/description
  Kernel support option(s) for RT288 model
endef

$(eval $(call KernelPackage,ltqcpe_rt288_support))

define KernelPackage/ltqcpe_vb300_support
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=Kernel support for VB300 model
  KCONFIG:= \
	CONFIG_LTQ_VB300=y \
	CONFIG_EXTERNAL_T3G=n \
	CONFIG_INTERNAL_AR9=y
endef

define KernelPackage/ltqcpe_vb300_support/description
  Kernel support option for VB300 model
endef

$(eval $(call KernelPackage,ltqcpe_vb300_support))

define KernelPackage/ipsec_ike_mod
  SUBMENU:=Netfilter Extensions
  TITLE:=IPSec IKE ALG Module
  KCONFIG:= \
	CONFIG_NF_CONNTRACK_IPSEC_IKE=m

  FILES:=$(LINUX_DIR)/net/netfilter/nf_conntrack_ipsec_ike.$(LINUX_KMOD_SUFFIX) 
endef

define KernelPackage/ipsec_ike_mod/description
  IPSec IKE ALG  Module
endef

$(eval $(call KernelPackage,ipsec_ike_mod))

define KernelPackage/ltqcpe_ipsec
  SUBMENU:=Netfilter Extensions
  TITLE:=IPSec Support
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_PWR_SAVE_MODE=n \
	CONFIG_XFRM_USER=y \
	CONFIG_XFRM_SUB_POLICY=y \
	CONFIG_XFRM_IPCOMP=y \
	CONFIG_NET_KEY=y \
	CONFIG_INET_AH=y \
	CONFIG_INET_ESP=y \
	CONFIG_INET_IPCOMP=y \
	CONFIG_INET_XFRM_TUNNEL=y \
	CONFIG_INET_TUNNEL=y \
	CONFIG_INET_XFRM_MODE_TRANSPORT=y \
	CONFIG_INET_XFRM_MODE_TUNNEL=y \
	CONFIG_CRYPTO_AEAD=y \
	CONFIG_CRYPTO_AUTHENC=y \
	CONFIG_CRYPTO_HMAC=y \
	CONFIG_CRYPTO_DEFLATE=y \
	CONFIG_NETFILTER_XT_MATCH_POLICY=y \
	CONFIG_NET_IPIP=y \
	CONFIG_CRYPTO_RNG=y \
	CONFIG_CRYPTO_GF128MUL=y \
	CONFIG_CRYPTO_GCM=y \
	CONFIG_CRYPTO_SEQIV=y \
	CONFIG_CRYPTO_CTR=y \
	CONFIG_CRYPTO_ECB=y \
	CONFIG_CRYPTO_XCBC=y \
	CONFIG_CRYPTO_GHASH=y \
	CONFIG_CRYPTO_MD5=y \
	CONFIG_CRYPTO_BLOWFISH=y
endef

define KernelPackage/ltqcpe_ipsec/description
  IPSec Support
endef

$(eval $(call KernelPackage,ltqcpe_ipsec))

define KernelPackage/ltqcpe_hardware_deu
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=Hardware DEU support
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_DEU=y
endef

define KernelPackage/ltqcpe_hardware_deu/description
  IPSec IKE ALG  Module
endef

$(eval $(call KernelPackage,ltqcpe_hardware_deu))

define KernelPackage/ltq_optimization
  SUBMENU:=$(LTQCPE_MENU)
  TITLE:=Kernel LTQ Optimizations
  KCONFIG:= \
	CONFIG_LTQ_OPTIMIZATION=y
endef

define KernelPackage/ltq_optimization/description
  Kernel LTQ Optimizations
endef

$(eval $(call KernelPackage,ltq_optimization))
