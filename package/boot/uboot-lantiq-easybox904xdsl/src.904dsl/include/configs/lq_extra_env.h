  "mem=" CONFIG_MEM "\0"
  "phym=" CONFIG_PHYM "\0"
  "ipaddr=" CONFIG_IP_ADDRESS "\0"
  "serverip=" CONFIG_SERVER_IP_ADDRESS "\0"
  "ethaddr=" CONFIG_ETHERNET_ADDRESS "\0"
  "netdev=" CONFIG_ETHERNET_DEVICE "\0"
  "console=" CONFIG_CONSOLE "\0"
  "baudrate=" CONFIG_ASC_BAUDRATE "\0"
  "tftppath=" CONFIG_TFTPPATH "\0"
  "loadaddr=" CONFIG_TFTP_LOAD_ADDRESS "\0"
  "rootpath=" CONFIG_ROOT_PATH "\0"
  "rootfsmtd=" CONFIG_ROOTFSMTD "\0"
#if 0
  "nfsargs= " CONFIG_NFSARGS "\0"
  "ramargs=" CONFIG_RAMARGS "\0"
#endif
  "addip=" CONFIG_ADDIP "\0"
  "addmisc=" CONFIG_ADDMISC "\0"
#if 0
  "flash_nfs=" CONFIG_FLASH_NFS "\0"
  "net_nfs=" CONFIG_NET_NFS "\0"
  "net_flash=" CONFIG_NET_FLASH "\0"
  "net_ram=" CONFIG_NET_RAM "\0"
#endif
  "u-boot=" CONFIG_U_BOOT "\0"
  "rootfs=" CONFIG_ROOTFS "\0"
#ifndef CONFIG_FIRMWARE_IN_ROOTFS
  "firmware=" CONFIG_FIRMWARE "\0"
#endif
  "fullimage=" CONFIG_FULLIMAGE "\0"
#ifdef CONFIG_JFFS2IMAGE
  "jffs2image=" CONFIG_JFFS2IMAGE "\0"
#endif
  "totalimage=" CONFIG_TOTALIMAGE "\0"
#if 0
  "load=tftp $(loadaddr) $(u-boot)\0"
  "update=protect off 1:0-2;era 1:0-2;cp.b $(loadaddr) B0000000 $(filesize)\0"
#endif
  "flashargs=" CONFIG_FLASHARGS "\0"
  "flash_flash=" CONFIG_FLASH_FLASH "\0"
#ifdef CONFIG_BOOT_FROM_NAND
  "update_nandboot=" CONFIG_UPDATE_NANDBOOT "\0"
#endif
  "update_uboot=" CONFIG_UPDATE_UBOOT "\0"
  "update_kernel=" CONFIG_UPDATE_KERNEL "\0"
  "update_rootfs=" CONFIG_UPDATE_ROOTFS "\0"
  "update_all=" CONFIG_UPDATE_ALL "\0"
 #ifdef CONFIG_DUAL_IMAGE
  "update_kernel2=" CONFIG_UPDATE_KERNEL2 "\0"
  "update_rootfs2=" CONFIG_UPDATE_ROOTFS2 "\0"
  "update_all2=" CONFIG_UPDATE_ALL2 "\0"
  "update_dual=" CONFIG_UPDATE_DUAL "\0"
 #endif
#ifndef CONFIG_FIRMWARE_IN_ROOTFS
  "update_firmware=" CONFIG_UPDATE_FIRMWARE "\0"
#endif
  "update_fullimage=" CONFIG_UPDATE_FULLIMAGE "\0"
#if 0
  "update_totalimage=" CONFIG_UPDATE_TOTALIMAGE "\0"
#endif
#ifdef CONFIG_UPDATE_SYSCONFIG
  "sysconfig=" CONFIG_SYSCONFIG "\0"
  "update_sysconfig=" CONFIG_UPDATE_SYSCONFIG "\0"
#endif
#ifndef CONFIG_ENV_IS_NOWHERE
#if 0
  "reset_ddr_config=prot off " MK_STR(IFX_CFG_FLASH_DDR_CFG_START_ADDR) " \
  "MK_STR(IFX_CFG_FLASH_DDR_CFG_END_ADDR)";\
  erase " MK_STR(IFX_CFG_FLASH_DDR_CFG_START_ADDR) " " MK_STR(IFX_CFG_FLASH_DDR_CFG_END_ADDR) "\0"
#endif
  "mtdparts=" CONFIG_MTDPARTS "\0"
#ifdef CONFIG_BOOT_FROM_NOR  
  "reset_uboot_config=prot off $(f_ubootconfig_addr) +$(f_ubootconfig_size); erase $(f_ubootconfig_addr) +$(f_ubootconfig_size)\0"
#elif defined(CONFIG_BOOT_FROM_SPI)
  "reset_uboot_config=sf probe " MK_STR(CONFIG_ENV_SPI_CS) "; sf erase $(f_ubootconfig_addr) $(f_ubootconfig_size)\0" 
#elif defined(CONFIG_BOOT_FROM_NAND)
  "reset_uboot_config=nand erase $(f_ubootconfig_addr) $(f_ubootconfig_size)\0" 
#endif
  IFX_CFG_FLASH_PARTITIONS_INFO
  "flash_end=" MK_STR(IFX_CFG_FLASH_END_ADDR) "\0"
  IFX_CFG_FLASH_DATA_BLOCKS_INFO
  "f_uboot_addr=" MK_STR(IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR) "\0"
  "f_uboot_size=" MK_STR(IFX_CFG_FLASH_UBOOT_IMAGE_SIZE) "\0"
  "f_ubootconfig_addr=" MK_STR(IFX_CFG_FLASH_UBOOT_CFG_START_ADDR) "\0"
  "f_ubootconfig_size=" MK_STR(IFX_CFG_FLASH_UBOOT_CFG_SIZE) "\0"
  "f_ubootconfig_end="  MK_STR(IFX_CFG_FLASH_UBOOT_CFG_END_ADDR) "\0"
  "f_kernel_addr=" MK_STR(IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR) "\0"
  "f_kernel_size=" MK_STR(IFX_CFG_FLASH_KERNEL_IMAGE_SIZE) "\0"
  "f_kernel_end="  MK_STR(IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR) "\0"
  "f_rootfs_addr=" MK_STR(IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR) "\0"
  "f_rootfs_size=" MK_STR(IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE) "\0"
  "f_rootfs_end="  MK_STR(IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR) "\0"
#ifdef CONFIG_DUAL_IMAGE
  "f_kernel2_addr=" MK_STR(IFX_CFG_FLASH_KERNEL2_IMAGE_START_ADDR) "\0"
  "f_kernel2_size=" MK_STR(IFX_CFG_FLASH_KERNEL2_IMAGE_SIZE) "\0"
  "f_kernel2_end="  MK_STR(IFX_CFG_FLASH_KERNEL2_IMAGE_END_ADDR) "\0"
  "f_rootfs2_addr=" MK_STR(IFX_CFG_FLASH_ROOTFS2_IMAGE_START_ADDR) "\0"
  "f_rootfs2_size=" MK_STR(IFX_CFG_FLASH_ROOTFS2_IMAGE_SIZE) "\0"
  "f_rootfs2_end="  MK_STR(IFX_CFG_FLASH_ROOTFS2_IMAGE_END_ADDR) "\0"
  "rootfsmtd2="CONFIG_ROOTFSMTD2"\0"
  "flashargs2=" CONFIG_FLASHARGS2 "\0"
  "flash_flash2=" CONFIG_FLASH_FLASH2 "\0"
#endif
#ifdef IFX_CFG_FLASH_LCD_IMAGE_BLOCK_NAME
  "f_lcdimage_addr=" MK_STR(IFX_CFG_FLASH_LCD_IMAGE_START_ADDR) "\0"
  "f_lcdimage_size=" MK_STR(IFX_CFG_FLASH_LCD_IMAGE_SIZE) "\0"
  "f_lcdimage_end="  MK_STR(IFX_CFG_FLASH_LCD_IMAGE_END_ADDR) "\0"
  "lcdimage=lcd_boot_image\0"
  "update_lcdimage=tftp $(loadaddr) $(lcdimage) ; nand erase $(f_lcdimage_addr) $(f_lcdimage_size) ; nand write $(loadaddr) $(f_lcdimage_addr) $(filesize)\0"
  "bootlcd=1\0"
#endif
#ifdef IFX_CFG_FLASH_MFG_CFG_BLOCK_NAME
  "f_mfgconfig_addr=" MK_STR(IFX_CFG_FLASH_MFG_CFG_START_ADDR) "\0"
  "f_mfgconfig_size=" MK_STR(IFX_CFG_FLASH_MFG_CFG_SIZE) "\0"
  "f_mfgconfig_end="  MK_STR(IFX_CFG_FLASH_MFG_CFG_END_ADDR) "\0"
#endif
#ifdef IFX_CFG_FLASH_SIP_DATA_BLOCK_NAME
  "f_sipdata_addr=" MK_STR(IFX_CFG_FLASH_SIP_DATA_START_ADDR) "\0"
  "f_sipdata_size=" MK_STR(IFX_CFG_FLASH_SIP_DATA_SIZE) "\0"
  "f_sipdata_end="  MK_STR(IFX_CFG_FLASH_SIP_DATA_END_ADDR) "\0"
#endif
#ifdef IFX_CFG_FLASH_VOICE_BLOCK_NAME
  "f_voice_addr=" MK_STR(IFX_CFG_FLASH_VOICE_START_ADDR) "\0"
  "f_voice_size=" MK_STR(IFX_CFG_FLASH_VOICE_SIZE) "\0"
  "f_voice_end="  MK_STR(IFX_CFG_FLASH_VOICE_END_ADDR) "\0"
#endif
#ifdef IFX_CFG_FLASH_MISC_BLOCK_NAME
  "f_misc_addr=" MK_STR(IFX_CFG_FLASH_MISC_START_ADDR) "\0"
  "f_misc_size=" MK_STR(IFX_CFG_FLASH_MISC_SIZE) "\0"
  "f_misc_end="  MK_STR(IFX_CFG_FLASH_MISC_END_ADDR) "\0"
#endif
#ifndef CONFIG_FIRMWARE_IN_ROOTFS
  "f_firmware_addr=" MK_STR(IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR) "\0"
  "f_firmware_size=" MK_STR(IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE) "\0"
#endif
  "f_tmp1_addr=" MK_STR(IFX_CFG_FLASH_TMP1_START_ADDR) "\0"
  "f_tmp1_size=" MK_STR(IFX_CFG_FLASH_TMP1_SIZE) "\0"
  "f_tmp1_end=" MK_STR(IFX_CFG_FLASH_TMP1_END_ADDR) "\0"
  "f_tmp2_addr=" MK_STR(IFX_CFG_FLASH_TMP2_START_ADDR) "\0"
  "f_tmp2_size=" MK_STR(IFX_CFG_FLASH_TMP2_SIZE) "\0"
  "f_tmp2_end=" MK_STR(IFX_CFG_FLASH_TMP2_END_ADDR) "\0"
  "f_sysconfig_addr=" MK_STR(IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR) "\0"
  "f_sysconfig_size=" MK_STR(IFX_CFG_FLASH_SYSTEM_CFG_SIZE) "\0"
  "f_sysconfig_end=" MK_STR(IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR) "\0"
  "f_fwdiag_addr=" MK_STR(IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR) "\0"
  "f_fwdiag_size=" MK_STR(IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE) "\0"
  "f_fwdiag_end=" MK_STR(IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR) "\0"
#if 0
  "f_calibration_addr= " MK_STR(IFX_CFG_FLASH_CALIBRATION_START_ADDR) "\0"
  "f_calibration_size=" MK_STR(IFX_CFG_FLASH_CALIBRATION_CFG_SIZE) "\0"
#endif
  "f_ddrconfig_addr=" MK_STR(IFX_CFG_FLASH_DDR_CFG_START_ADDR) "\0"
  "f_ddrconfig_size=" MK_STR(IFX_CFG_FLASH_DDR_CFG_SIZE) "\0"
#if defined(CONFIG_IN_SUPERTASK) && (CONFIG_IN_SUPERTASK==1)
  "f_supertask1_addr=" MK_STR(IFX_CFG_FLASH_SUPERTASK1_START_ADDR) "\0"
  "f_supertask1_size=" MK_STR(IFX_CFG_FLASH_SUPERTASK1_SIZE) "\0"
  "f_uboot_bin_addr=" MK_STR(IFX_CFG_FLASH_UBOOT_BIN_START_ADDR) "\0"
  "f_uboot_bin_size=" MK_STR(IFX_CFG_FLASH_UBOOT_BIN_SIZE) "\0"
  "f_supertask2_addr=" MK_STR(IFX_CFG_FLASH_SUPERTASK2_START_ADDR) "\0"
  "f_supertask2_size=" MK_STR(IFX_CFG_FLASH_SUPERTASK2_SIZE) "\0"
#endif
#endif //CONFIG_ENV_IS_NOWHERE
#ifdef CONFIG_UPDATE_JFFS2IMAGE
  "update_jffs2image=" CONFIG_UPDATE_JFFS2IMAGE "\0"
 #ifdef CONFIG_DUAL_IMAGE
  "update_jffs2image2=" CONFIG_UPDATE_JFFS2IMAGE2 "\0"
 #endif
#endif
#ifdef CONFIG_UPDATE_FLASH
  "update_flash=" CONFIG_UPDATE_FLASH "\0"
#endif
  "sw_version=" CONFIG_SW_VERSION "\0"
  "hw_version=" CONFIG_HW_VERSION "\0"
  "serial=" CONFIG_SERIAL "\0"
  "wpa_key=1234567890\0"
