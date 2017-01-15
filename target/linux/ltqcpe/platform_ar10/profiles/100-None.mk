#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/None
  NAME:=Generic Profile
endef

define Profile/None/Description
	Basic profile
endef
$(eval $(call Profile,None))

