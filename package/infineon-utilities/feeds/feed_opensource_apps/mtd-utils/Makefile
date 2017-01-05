#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mtd-utils
PKG_VERSION:=20090505
PKG_BASE_NAME:=mtd
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=40f40f2c33309fbfeb3910c47c2b48cf
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
        SECTION:=utils
        CATEGORY:=Base system
        TITLE:=mtd utils
	URL:=http://git.infradead.org/mtd-utils
endef

define Package/$(PKG_NAME)/description
  This package contains an utility useful to upgrade firmware images
endef

CFLAGS += $(PKG_CFLAGS) -I$(PKG_BUILD_DIR)/include -L$(PKG_BUILD_DIR) -L$(STAGING_DIR)/lib -DNO_NATIVE_SUPPORT
ifneq ($(HOST_OS),Linux)
 CFLAGS += -Dloff_t=off_t -D__BYTE_ORDER=BYTE_ORDER -include getline.h -include endian.h -I$(CURDIR)/include -include fls.h
endif

MTD_MAKEOPTS = \
	CFLAGS="$(CFLAGS)" \
	NO_LZO=1 WITHOUT_XATTR=1 \
	SUBDIRS="" \
	BUILDDIR="$(PKG_BUILD_DIR)"

### ctc ###
#MTD_UTIL_BINS := flash_erase flash_eraseall flashcp \
#	$(if $(CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND), nanddump flash_lock flash_unlock flash_info nandwrite nandtest)
MTD_UTIL_BINS := flash_erase flash_eraseall flashcp mtd_debug \
	$(if $(CONFIG_UBOOT_CONFIG_NAND_FLASH), nanddump flash_lock flash_unlock flash_info nandwrite nandtest)
###########

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		$(MTD_MAKEOPTS) \
		TARGETS="$(MTD_UTIL_BINS)"
	$(if $(CONFIG_PACKAGE_kmod-ubi_mtd),\
	$(MAKE) -C $(PKG_BUILD_DIR)/ubi-utils \
		$(TARGET_CONFIGURE_OPTS) \
		$(MTD_MAKEOPTS) \
		TARGETS="$(UBI_UTILS_BIN)" \
	)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(foreach mtdbin, $(MTD_UTIL_BINS) $(if $(CONFIG_PACKAGE_kmod-ubi_mtd),$(UBI_UTILS_BIN)), \
	  $(INSTALL_BIN) $(PKG_BUILD_DIR)/$(mtdbin) $(1)/usr/sbin/;)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/flash_erase $(1)/usr/sbin/flash_erase_nand

endef

$(eval $(call BuildPackage,$(PKG_NAME)))
