#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

INPUT_MODULES_MENU:=Input modules

define KernelPackage/keyboard-eb904
  SUBMENU:=$(INPUT_MODULES_MENU)
  TITLE:=EasyBox 904 keypad support
  DEPENDS:=@(TARGET_lantiq_xrx200_DEVICE_VGV952CJW33) +kmod-input-matrixkmap +kmod-i2c-core +kmod-i2c-algo-bit +kmod-i2c-algo-bit +kmod-i2c-gpio-custom
  KCONFIG:= \
	CONFIG_KEYBOARD_EB904=m \
	CONFIG_INPUT_KEYBOARD=y
  FILES:=$(LINUX_DIR)/drivers/input/keyboard/eb904_keypad.ko
  AUTOLOAD:=$(call AutoLoad,60,eb904_keypad)
endef

define KernelPackage/keyboard-eb904/description
 Enable support for EasyBox 904 keypad port.
endef

$(eval $(call KernelPackage,keyboard-eb904))
