#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ltqcpe
BOARDNAME:=Lantiq CPE
FEATURES:=squashfs jffs2 atm
### ctc ###
SUBTARGETS=platform_vr9
###########

LINUX_VERSION:=2.6.32.32

### ctc ###
#CFLAGS=-Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time
CFLAGS=-Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time -fhonour-copts
###########

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES+=uboot-lantiq

define Target/Description
	Build firmware images for Lantiq CPE
endef

$(eval $(call BuildTarget))
