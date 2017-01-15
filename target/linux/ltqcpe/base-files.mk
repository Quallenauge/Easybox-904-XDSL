#
# LTQCPE rootfs addon for OpenWRT basefiles
#

### ctc ###
define Package/base-files/install-target
endef
###########
define Package/base-files/install-targetX
	rm -f $(1)/var
	rm -f $(1)/etc/{passwd,hosts,fstab}
	rm -rf $(1)/tmp
	rm -rf $(1)/etc/init.d

	mkdir -p $(1)/{usr/bin,usr/lib,etc/dsl_api,etc/ppp,etc/init.d,etc/rc.d,proc,root,ramdisk,ramdisk_copy,mnt/overlay}
	mkdir -p $(1)/ramdisk/{tmp,var,flash}
	mkdir -p $(1)/ramdisk_copy/{var/tmp,var/run,var/log,etc/Wireless,etc/dnrd,etc/ppp/peers,tftp_upload}

	if [ -d $(PLATFORM_DIR)/$(PROFILE)/base-files/etc/init.d/. ]; then \
		$(CP) $(PLATFORM_DIR)/$(PROFILE)/base-files/etc/init.d/* $(1)/etc/init.d/; \
	fi
	$(if $(filter-out $(PLATFORM_DIR),$(PLATFORM_SUBDIR)), \
		if [ -d $(PLATFORM_SUBDIR)/base-files/etc/init.d/. ]; then \
			$(CP) $(PLATFORM_SUBDIR)/base-files/etc/init.d/* $(1)/etc/init.d/; \
		fi \
	)
	-sed -i -e '/\/etc\/passwd/d' -e '/\/etc\/hosts/d' $(1)/CONTROL/conffiles

	cp -f $(PLATFORM_DIR)/base-files/etc/fstab $(1)/etc/

	cd $(1); \
		mkdir -p lib/firmware/$(LINUX_VERSION); \
		ln -sf lib/firmware/$(LINUX_VERSION) firmware; \
		ln -sf ramdisk/tmp tmp; \
		ln -sf ramdisk/var var; \
		ln -sf ramdisk/flash flash

	mkdir -p $(STAGING_DIR)/usr/include/
	$(if $(CONFIG_PACKAGE_ifx-utilities),cp -f $(PLATFORM_DIR)/base-files/etc/rc.conf $(STAGING_DIR)/usr/include/)

	mkdir -p $(1)/etc
	cd $(1)/etc; \
		$(if $(CONFIG_PACKAGE_ifx-utilities),gzip rc.conf;) \
		rm -f rc.conf; \
		ln -sf ../ramdisk/etc/dnrd dnrd; \
		ln -sf ../ramdisk/etc/hostapd.conf hostapd.conf; \
		ln -sf ../ramdisk/etc/hosts hosts; \
		ln -sf ../ramdisk/etc/ilmid ilmid; \
		ln -sf ../ramdisk/etc/inetd.conf inetd.conf; \
		ln -sf ../ramdisk/etc/ipsec.conf ipsec.conf; \
		ln -sf ../ramdisk/etc/ipsec.secrets ipsec.secrets; \
		ln -sf ../proc/mounts mtab; \
		ln -sf ../flash/passwd passwd; \
		ln -sf ../ramdisk/flash/rc.conf rc.conf; \
		ln -sf ../ramdisk/etc/resolv.conf resolv.conf; \
		ln -sf ../ramdisk/etc/ripd.conf ripd.conf; \
		ln -sf ../ramdisk/etc/snmp snmp; \
		ln -sf ../ramdisk/etc/tr69 tr69; \
		ln -sf ../ramdisk/etc/udhcpd.conf udhcpd.conf; \
		ln -sf ../ramdisk/etc/zebra.conf zebra.conf; \
		ln -sf ../ramdisk/etc/TZ TZ

	mkdir -p $(1)/etc/ppp
	cd $(1)/etc/ppp; \
		ln -sf ../../ramdisk/etc/ppp/options options; \
		ln -sf ../../ramdisk/etc/ppp/peers peers; \
		ln -sf ../../ramdisk/etc/ppp/resolv.conf resolv.conf

	mkdir -p $(1)/etc/rc.d
	cd $(1)/etc/rc.d; \
		ln -sf ../init.d init.d; \
		ln -sf ../config.sh config.sh

	$(if $(CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND),mkdir -p $(1)/etc/sysconfig)
	$(if $(CONFIG_TARGET_ROOTFS_GLUBI_SUPPORT),mkdir -p $(1)/mnt/data)
	$(if $(CONFIG_TARGET_DATAFS_JFFS2),echo jffs2 > $(1)/mnt/data/fs)
	$(if $(CONFIG_TARGET_DATAFS_UBIFS),echo ubifs > $(1)/mnt/data/fs)

endef
