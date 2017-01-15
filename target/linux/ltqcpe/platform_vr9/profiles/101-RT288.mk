#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RT288
  NAME:=RT288 Profile
endef

define Profile/RT288/Description
  RT288 Profile
endef

$(eval $(call Profile,RT288))
