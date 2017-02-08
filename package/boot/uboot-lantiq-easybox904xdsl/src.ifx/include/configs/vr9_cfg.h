/* ============================================================================
 * Copyright (C) 2003[- 2004] ? Infineon Technologies AG.
 *
 * All rights reserved.
 * ============================================================================
 *
 * ============================================================================
 *
 * This document contains proprietary information belonging to Infineon
 * Technologies AG. Passing on and copying of this document, and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * ============================================================================
 *
 * File Name: ifx_cfg.h
 * Author : Mars Lin (mars.lin@infineon.com)
 * Date:
 *
 * ===========================================================================
 *
 * Project:
 * Block:
 *
 * ===========================================================================
 * Contents:  This file contains the data structures and definitions used
 *        by the core iptables and the sip alg modules.
 * ===========================================================================
 * References:
 */

/*
 * This file contains the configuration parameters for the IFX board.
 */


/*-----------------------------------------------------------------------
 * Board specific configurations
 */
#if defined(CONFIG_BOOT_FROM_NOR) || defined(CONFIG_BOOT_FROM_UART)

#ifdef CONFIG_NOR_FLASH_2M
#define IFX_CONFIG_FLASH_SIZE 2
#elif CONFIG_NOR_FLASH_4M
#define IFX_CONFIG_FLASH_SIZE 4
#elif CONFIG_NOR_FLASH_8M
#define IFX_CONFIG_FLASH_SIZE 8
#elif CONFIG_NOR_FLASH_16M
#define IFX_CONFIG_FLASH_SIZE 16
#elif CONFIG_NOR_FLASH_32M
#define IFX_CONFIG_FLASH_SIZE 32
#endif

#ifndef IFX_CONFIG_FLASH_SIZE
#define IFX_CONFIG_FLASH_SIZE 8
#endif

#ifdef CONFIG_FIRMWARE_IN_ROOTFS
//2MB flash partition
#if (IFX_CONFIG_FLASH_SIZE == 2)
  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "total_part=2\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
		"data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
		"data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
		"data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
		"data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
		"data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
		"data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
		"total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0xB0000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0xB0020000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0xB01FADFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0xB01FB200
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x4000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0xB01FEDFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0xB01FEE00
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0xB01FFDFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0xB01FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0xB01FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0xB01FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "total_part=2\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME           "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR           0xB0000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME          "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR          0xB0020000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME          "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR          0xB03F6DFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME            "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR            0xB03ECE00
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                  0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR              0xB03FCDFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME             "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR             0xB03FEE00
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                   0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR               0xB03FFDFF


  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME         "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR         0xB03FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE               0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR           0xB03FFFFF

  #define IFX_CFG_FLASH_END_ADDR                         0xB03FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 8)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "total_part=2\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0xB0000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0xB0040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0xB07EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0xB07EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0xB07FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0xB07FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0xB07FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0xB07FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0xB07FF3FF

  #define IFX_CFG_FLASH_END_ADDR                         0xB07FFFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME			"/dev/mtdblock1"
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_START_ADDR	IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR

#elif (IFX_CONFIG_FLASH_SIZE == 16)

 #if defined(CONFIG_IN_SUPERTASK) && (CONFIG_IN_SUPERTASK==1)

	#define IFX_CFG_FLASH_PARTITIONS_INFO								\
			"part0_begin=0xB0000000\0"									\
			"part1_begin=0xB0020000\0"									\
			"part2_begin=0xB0860000\0"									\
			"part3_begin=0xB0880000\0"									\
			"part4_begin=0xB0FA0000\0"									\
			"part5_begin=0xB0FC0000\0"									\
			"total_part=6\0"

	#define IFX_CFG_FLASH_DATA_BLOCKS_INFO								\
			"data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
			"data_block1=" IFX_CFG_FLASH_SUPERTASK1_BLOCK_NAME "\0"		\
			"data_block2=" IFX_CFG_FLASH_UBOOT_BIN_BLOCK_NAME "\0"		\
			"data_block3=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
			"data_block4=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
			"data_block5=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
			"data_block6=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
			"data_block7=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
			"data_block8=" IFX_CFG_FLASH_SUPERTASK2_BLOCK_NAME "\0"		\
			"total_db=9\0"

	#define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME		"uboot"
	#define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR		0xB0000000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE				0x00020000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME		"/dev/mtdblock0"

	#define IFX_CFG_FLASH_SUPERTASK1_BLOCK_NAME			"supertask1"
	#define IFX_CFG_FLASH_SUPERTASK1_START_ADDR 		0xB0020000
	#define IFX_CFG_FLASH_SUPERTASK1_SIZE				0x00840000
	#define IFX_CFG_FLASH_SUPERTASK1_MTDBLOCK_NAME		"/dev/mtdblock1"

	#define IFX_CFG_FLASH_UBOOT_BIN_BLOCK_NAME			"uboot_bin"
	#define IFX_CFG_FLASH_UBOOT_BIN_START_ADDR			0xB0860000
	#define IFX_CFG_FLASH_UBOOT_BIN_SIZE				0x00020000
	#define IFX_CFG_FLASH_UBOOT_BIN_MTDBLOCK_NAME		"/dev/mtdblock2"

	#define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME		"rootfs"
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR 		0xB0880000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE				0x00720000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock3"

	#define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME		"kernel"
	#define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR		0xB0F9FFFF
	#define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE				0

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME			"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR			0xB0FA0000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE				0x0001EC00
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR			0xB0FBEBFF
	#define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME		"/dev/mtdblock4"

	#define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME			"ubootconfig"
	#define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR			0xB0FBEC00
	#define IFX_CFG_FLASH_UBOOT_CFG_SIZE				0x00001000
	#define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR			0xB0FBFBFF

	#define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME		"fwdiag"
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR		0xB0FBFC00
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE			0x00000400
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR		0xB0FBFFFF

	#define IFX_CFG_FLASH_SUPERTASK2_BLOCK_NAME			"supertask2"
	#define IFX_CFG_FLASH_SUPERTASK2_START_ADDR 		0xB0FC0000
	#define IFX_CFG_FLASH_SUPERTASK2_SIZE				0x00040000
	#define IFX_CFG_FLASH_SUPERTASK2_MTDBLOCK_NAME		"/dev/mtdblock5"

	#define IFX_CFG_FLASH_END_ADDR						0xB0FFFFFF

	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME		IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME
	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_START_ADDR	IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR

  #else

	#define IFX_CFG_FLASH_PARTITIONS_INFO								\
			"part0_begin=0xB0000000\0"									\
			"part1_begin=0xB0040000\0"									\
			"part2_begin=0xB0D00000\0"									\
			"part3_begin=0xB0F00000\0"									\
			"part4_begin=0xB0FC0000\0"									\
			"part5_begin=0xB0FE0000\0"									\
			"total_part=6\0"

	#define IFX_CFG_FLASH_DATA_BLOCKS_INFO								\
			"data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
			"data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
			"data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
			"data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
			"data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
			"data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
			"total_db=6\0"

	#define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME		"uboot"
	#define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR		0xB0000000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE				0x00040000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME		"/dev/mtdblock0"

	#define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME		"rootfs"
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR 		0xB0040000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE				0x00CC0000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock1"

	#define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME		"kernel"
	#define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR		0xB0EFFFFF
	#define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE				0x00200000
	#define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock2"

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME			"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR			0xB0F00000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE				0x000C0000
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR			0xB0FBFFFF
	#define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME		"/dev/mtdblock3"

	#define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME			"ubootconfig"
	#define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR			0xB0FC0000
	#define IFX_CFG_FLASH_UBOOT_CFG_SIZE				0x00020000
	#define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR			0xB0FDFFFF
	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAMEX		"/dev/mtdblock4"

	#define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME		"fwdiag"
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR		0xB0FE0000
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE			0x00020000
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR		0xB0FFFFFF
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME	"/dev/mtdblock5"

	#define IFX_CFG_FLASH_END_ADDR						0xB0FFFFFF

	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME		IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAMEX
	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_START_ADDR	IFX_CFG_FLASH_UBOOT_CFG_START_ADDR

  #endif

#elif (IFX_CONFIG_FLASH_SIZE == 32)

  #if defined(CONFIG_IN_SUPERTASK) && (CONFIG_IN_SUPERTASK==1)

	#define IFX_CFG_FLASH_PARTITIONS_INFO								\
			"part0_begin=0xB0000000\0"									\
			"part1_begin=0xB0020000\0"									\
			"part2_begin=0xB0860000\0"									\
			"part3_begin=0xB0880000\0"									\
			"part4_begin=0xB0FA0000\0"									\
			"part5_begin=0xB0FC0000\0"									\
			"total_part=6\0"

	#define IFX_CFG_FLASH_DATA_BLOCKS_INFO								\
			"data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
			"data_block1=" IFX_CFG_FLASH_SUPERTASK1_BLOCK_NAME "\0"		\
			"data_block2=" IFX_CFG_FLASH_UBOOT_BIN_BLOCK_NAME "\0"		\
			"data_block3=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
			"data_block4=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
			"data_block5=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
			"data_block6=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
			"data_block7=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
			"data_block8=" IFX_CFG_FLASH_SUPERTASK2_BLOCK_NAME "\0"		\
			"total_db=9\0"

	#define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME		"uboot"
	#define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR		0xB0000000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE				0x00020000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME		"/dev/mtdblock0"

	#define IFX_CFG_FLASH_SUPERTASK1_BLOCK_NAME			"supertask1"
	#define IFX_CFG_FLASH_SUPERTASK1_START_ADDR 		0xB0020000
	#define IFX_CFG_FLASH_SUPERTASK1_SIZE				0x00840000
	#define IFX_CFG_FLASH_SUPERTASK1_MTDBLOCK_NAME		"/dev/mtdblock1"

	#define IFX_CFG_FLASH_UBOOT_BIN_BLOCK_NAME			"uboot_bin"
	#define IFX_CFG_FLASH_UBOOT_BIN_START_ADDR			0xB0860000
	#define IFX_CFG_FLASH_UBOOT_BIN_SIZE				0x00020000
	#define IFX_CFG_FLASH_UBOOT_BIN_MTDBLOCK_NAME		"/dev/mtdblock2"

	#define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME		"rootfs"
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR 		0xB0880000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE				0x00720000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock3"

	#define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME		"kernel"
	#define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR		0xB0F9FFFF
	#define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE				0

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME			"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR			0xB0FA0000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE				0x0001EC00
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR			0xB0FBEBFF
	#define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME		"/dev/mtdblock4"

	#define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME			"ubootconfig"
	#define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR			0xB0FBEC00
	#define IFX_CFG_FLASH_UBOOT_CFG_SIZE				0x00001000
	#define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR			0xB0FBFBFF

	#define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME		"fwdiag"
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR		0xB0FBFC00
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE			0x00000400
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR		0xB0FBFFFF

	#define IFX_CFG_FLASH_SUPERTASK2_BLOCK_NAME			"supertask2"
	#define IFX_CFG_FLASH_SUPERTASK2_START_ADDR 		0xB0FC0000
	#define IFX_CFG_FLASH_SUPERTASK2_SIZE				0x01040000
	#define IFX_CFG_FLASH_SUPERTASK2_MTDBLOCK_NAME		"/dev/mtdblock5"

	#define IFX_CFG_FLASH_END_ADDR						0xB1FFFFFF

	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME		IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME
	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_START_ADDR	IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR

  #else

	#define IFX_CFG_FLASH_PARTITIONS_INFO								\
			"part0_begin=0xB0000000\0"									\
			"part1_begin=0xB0040000\0"									\
			"part2_begin=0xB1D00000\0"									\
			"part3_begin=0xB1F00000\0"									\
			"part4_begin=0xB1FC0000\0"									\
			"part5_begin=0xB1FE0000\0"									\
			"total_part=6\0"

	#define IFX_CFG_FLASH_DATA_BLOCKS_INFO								\
			"data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
			"data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
			"data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
			"data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
			"data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
			"data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
			"total_db=6\0"

	#define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME		"uboot"
	#define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR		0xB0000000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE				0x00040000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME		"/dev/mtdblock0"

	#define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME		"rootfs"
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR 		0xB0040000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE				0x01CC0000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock1"

	#define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME		"kernel"
	#define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR		0xB1EFFFFF
	#define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE				0x00200000
	#define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock2"

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME			"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR			0xB1F00000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE				0x000C0000
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR			0xB1FBFFFF
	#define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME		"/dev/mtdblock3"

	#define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME			"ubootconfig"
	#define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR			0xB1FC0000
	#define IFX_CFG_FLASH_UBOOT_CFG_SIZE				0x00020000
	#define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR			0xB1FDFFFF
	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAMEX		"/dev/mtdblock4"

	#define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME		"fwdiag"
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR		0xB1FE0000
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE			0x00020000
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR		0xB1FFFFFF
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME	"/dev/mtdblock5"

	#define IFX_CFG_FLASH_END_ADDR						0xB1FFFFFF

	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME		IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAMEX
	#define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_START_ADDR	IFX_CFG_FLASH_UBOOT_CFG_START_ADDR

  #endif

#else
  #error "ERROR!! Define flash size first!"
#endif

#else /*Not defined CONFIG_FIRMWARE_IN_ROOTFS*/
//2MB flash partition
#if (IFX_CONFIG_FLASH_SIZE == 2)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "part2_begin=0xB00A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0xB0000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME       "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR       0xB0020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE             0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME    "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0xB00A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0xB01FADFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0xB01FAE00
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x4000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0xB01FEDFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0xB01FEE00
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0xB01FFDFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0xB01FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0xB01FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0xB01FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "part2_begin=0xB00A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME           "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR           0xB0000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME        "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR        0xB0020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE              0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME     "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME          "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR          0xB00A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0xB03EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0xB03EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0xB03FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME             "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR             0xB03FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                   0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR               0xB03FEFFF


  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME         "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0xB03FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0xB03FF3FF


  #define IFX_CFG_FLASH_END_ADDR                         0xB03FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 8)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "part2_begin=0xB00A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0xB0000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME         "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0xB0020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0xB00A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0xB07EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0xB07EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0xB07FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0xB07FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0xB07FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0xB07FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0xB07FF3FF
  
   #define IFX_CFG_FLASH_END_ADDR                          0xB07FFFFF


#else
  #error "ERROR!! Define flash size first!"
#endif

#endif /*CONFIG_FIRMWARE_IN_ROOTFS*/


#endif /*CONFIG_BOOT_FROM_NOR*/

/***************************************************************************/
/***************************************************************************/
/***************SPI FLASH map starts here***********************************/
/***************************************************************************/
/***************************************************************************/

#ifdef CONFIG_BOOT_FROM_SPI

#ifdef CONFIG_SPI_FLASH_1M
#define IFX_CONFIG_FLASH_SIZE 1
#elif CONFIG_SPI_FLASH_2M
#define IFX_CONFIG_FLASH_SIZE 2
#elif CONFIG_SPI_FLASH_4M
#define IFX_CONFIG_FLASH_SIZE 4
#elif CONFIG_SPI_FLASH_8M
#define IFX_CONFIG_FLASH_SIZE 8
#endif

#ifndef IFX_CONFIG_FLASH_SIZE
#define IFX_CONFIG_FLASH_SIZE 4
#endif

#ifdef CONFIG_FIRMWARE_IN_ROOTFS
//1MB flash partition
#if (IFX_CONFIG_FLASH_SIZE == 1)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00020000\0"                                      \
        "total_part=2\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0x00020000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0x000F5FFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0x000F6000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0x001FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0x000FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0x000FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0x000FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0x000FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0x000FFFFF
//2MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 2)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "total_part=2\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0x00010000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0x001F5FFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0x001F6000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0x001FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0x001FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0x001FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0x001FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0x001FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0x001FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "total_part=2\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"
        
  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME           "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR           0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME          "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR          0x00010000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME          "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR          0x003F5FFF  
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME            "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR            0x003F6000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                  0x8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR              0x003FDFFF


  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME             "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR             0x003FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                   0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR               0x003FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME         "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR         0x003FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE               0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR           0x003FFFFF

  #define IFX_CFG_FLASH_END_ADDR                         0x003FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 8)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00020000\0"                                      \
        "total_part=2\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00020000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x007EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME            "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR            0x007EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                  0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR              0x007FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME             "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR             0x007FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                   0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR               0x007FEFFF


  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME         "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR         0x007FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE               0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR           0x007FFFFF

  #define IFX_CFG_FLASH_END_ADDR                         0x007FFFFF


#else
  #error "ERROR!! Define flash size first!"
#endif

#else /*Not defined CONFIG_FIRMWARE_IN_ROOTFS*/
#if (IFX_CONFIG_FLASH_SIZE == 1)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00020000\0"                                      \
        "part2_begin=0x000A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME       "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR       0x00020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE             0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME    "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0x000A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0x000F5FFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0x000F6000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0x000FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0x000FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0x000FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0x000FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0x000FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0x000FFFFF

//2MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 2)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00020000\0"                                      \
        "part2_begin=0x000A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME       "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR       0x00020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE             0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME    "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0x000A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0x001F5FFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0x001F6000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0x001FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0x001FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0x001FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0x001FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0x001FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0x001FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00020000\0"                                      \
        "part2_begin=0x000A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME           "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR           0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME        "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR        0x00020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE              0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME     "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME          "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR          0x000A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME          "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR          0x003F5FFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME            "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR            0x003F6000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                  0x8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR              0x003FDFFF


  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME             "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR             0x003FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                   0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR               0x003FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME         "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR         0x003FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE               0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR           0x003FFFFF

  #define IFX_CFG_FLASH_END_ADDR                         0x003FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 8)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00020000\0"                                      \
        "part2_begin=0x000A0000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME         "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0x00020000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x000A0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME          "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR          0x007EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME            "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR            0x007EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                  0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR              0x007FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME             "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR             0x007FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                   0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR               0x007FEFFF


  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME         "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR         0x007FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE               0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR           0x007FF3FF

  #define IFX_CFG_FLASH_END_ADDR                         0x007FFFFF


#else
  #error "ERROR!! Define flash size first!"
#endif

#endif /*CONFIG_FIRMWARE_IN_ROOTFS*/


#endif /*CONFIG_BOOT_FROM_SPI*/
/* End of Board specific configurations
 *-----------------------------------------------------------------------
 */



/***************************************************************************/
/***************************************************************************/
/***************NAND FLASH map starts here***********************************/
/***************************************************************************/
/***************************************************************************/

#ifdef CONFIG_BOOT_FROM_NAND

#ifdef CONFIG_NAND_FLASH_2M
#define IFX_CONFIG_FLASH_SIZE 2
#elif CONFIG_NAND_FLASH_4M
#define IFX_CONFIG_FLASH_SIZE 4
#elif CONFIG_NAND_FLASH_8M
#define IFX_CONFIG_FLASH_SIZE 8
#elif CONFIG_NAND_FLASH_16M
#define IFX_CONFIG_FLASH_SIZE 16
#elif CONFIG_NAND_FLASH_32M
#define IFX_CONFIG_FLASH_SIZE 32
#elif CONFIG_NAND_FLASH_128M
#define IFX_CONFIG_FLASH_SIZE 128
#elif CONFIG_NAND_FLASH_512M
#define IFX_CONFIG_FLASH_SIZE 512
#endif

#ifndef IFX_CONFIG_FLASH_SIZE
#define IFX_CONFIG_FLASH_SIZE 4
#endif

#ifdef CONFIG_FIRMWARE_IN_ROOTFS

//2MB flash partition
#if (IFX_CONFIG_FLASH_SIZE == 2)

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "total_part=2\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0x00010000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0x001FADFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0x001FB200
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x4000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0x001FEDFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0x001FEE00
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0x001FFDFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0x001FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0x001FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0x001FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "total_part=2\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"
        
  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME           "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR           0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME          "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR          0x00010000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x003EDFFF  
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x007EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x007FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x003FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x003FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x003FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x003FF3FF

  #define IFX_CFG_FLASH_END_ADDR                          0x003FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 8)

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "total_part=2\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00010000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x007EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x007EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x007FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x007FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x007FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x007FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x007FF3FF

 
  #define IFX_CFG_FLASH_END_ADDR                         0x007FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 32)

 #ifndef CONFIG_DUAL_IMAGE

  #define IFX_CFG_FLASH_PARTITIONS_INFO						\
        "part0_begin=0x00000000\0" /* uboot, 256KB*/		\
        "part1_begin=0x00040000\0" /* rootfs, 29440KB*/		\
        "part2_begin=0x01D00000\0" /* kernel, 2M*/			\
        "part3_begin=0x01F00000\0" /* sysconfig, 864KB*/	\
        "part4_begin=0x01FD8000\0" /* ubootconfig, 48KB*/	\
        "part5_begin=0x01FE4000\0" /* fwdiag, 48KB*/		\
        "part6_begin=0x01FF0000\0" /* BBT, 64KB*/			\
        "total_part=7\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0x00040000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_END_ADDR              0x0003FFFF
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x01CC0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR             0x01CFFFFF
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x01D00000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0x00200000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR             0x01EFFFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x01F00000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x000D8000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x01FD7FFF
  #define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME          "/dev/mtdblock3"

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x01FD8000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x0000C000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x01FE3FFF
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock4"

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x01FE4000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x0000C000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x01FEFFFF
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME       "/dev/mtdblock5"

  #define IFX_CFG_FLASH_END_ADDR                          0x01FFFFFF

 #else /* CONFIG_DUAL_IMAGE */

  #define IFX_CFG_FLASH_PARTITIONS_INFO						\
        "part0_begin=0x00000000\0" /* uboot, 256KB*/		\
        "part1_begin=0x00040000\0" /* rootfs, 13M*/			\
        "part2_begin=0x00D40000\0" /* kernel, 2M*/			\
        "part3_begin=0x00F40000\0" /* rootfs2, 13M*/		\
        "part4_begin=0x01C40000\0" /* kernel2, 2M*/			\
        "part5_begin=0x01E40000\0" /* sysconfig, 768KB*/	\
        "part6_begin=0x01F00000\0" /* ubootconfig, 64KB*/	\
        "part7_begin=0x01F10000\0" /* fwdiag, 64KB*/		\
        "part8_begin=0x01E20000\0" /* voicemail, 832KB*/	\
        "part9_begin=0x01FF0000\0" /* BBT, 64KB*/			\
        "total_part=10\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO							\
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
        "data_block3=" IFX_CFG_FLASH_ROOTFS2_IMAGE_BLOCK_NAME "\0"	\
        "data_block4=" IFX_CFG_FLASH_KERNEL2_IMAGE_BLOCK_NAME "\0"	\
        "data_block5=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
        "data_block6=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
        "data_block7=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
        "data_block8=" IFX_CFG_FLASH_VOICE_MAIL_BLOCK_NAME "\0"		\
        "total_db=9\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0x00040000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_END_ADDR              0x0003FFFF
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x00D00000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR             0x00D3FFFF
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x00D40000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0x00200000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR             0x00F3FFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_BLOCK_NAME          "rootfs2"
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_START_ADDR          0x00F40000
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_SIZE                0x00D00000
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_END_ADDR            0x01C3FFFF
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock3"

  #define IFX_CFG_FLASH_KERNEL2_IMAGE_BLOCK_NAME          "kernel2"
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_START_ADDR          0x01C40000
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_SIZE                0x00200000
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_END_ADDR            0x01E3FFFF
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock4"

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x01E40000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x000C0000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x01EFFFFF
  #define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME          "/dev/mtdblock5"

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x01F00000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x00010000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x01F0FFFF
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock6"

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x01F10000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x00010000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x01F1FFFF
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME       "/dev/mtdblock7"

  #define IFX_CFG_FLASH_VOICE_MAIL_BLOCK_NAME             "voicemail"
  #define IFX_CFG_FLASH_VOICE_MAIL_START_ADDR             0x01F20000
  #define IFX_CFG_FLASH_VOICE_MAIL_SIZE                   0x000D0000
  #define IFX_CFG_FLASH_VOICE_MAIL_END_ADDR               0x01FEFFFF
  #define IFX_CFG_FLASH_VOICE_MAIL_MTDBLOCK_NAME          "/dev/mtdblock8"

  #define IFX_CFG_FLASH_END_ADDR                          0x01FFFFFF

 #endif /* CONFIG_DUAL_IMAGE */

#elif (IFX_CONFIG_FLASH_SIZE == 128)

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                 \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00040000\0"                                      \
        "part2_begin=0x07A00000\0"                                      \
        "part3_begin=0x07D00000\0"                                      \
        "part4_begin=0x07D80000\0"                                      \
        "part5_begin=0x07DC0000\0"                                      \
        "part6_begin=0x07F00000\0"                                      \
        "total_part=7\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0x00040000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_END_ADDR              0x0003FFFF
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x079C0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR             0x079FFFFF
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x07A00000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0x00300000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR             0x07CFFFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x07D00000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x00100000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x07DFFFFF
  #define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME          "/dev/mtdblock3"

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x07E00000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x00100000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x07EFFFFF
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock4"

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x07F00000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x00080000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x07F7FFFF
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME       "/dev/mtdblock5"

  #define IFX_CFG_FLASH_END_ADDR                          0x07FFFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 512)

 #ifndef CONFIG_DUAL_IMAGE

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00040000\0"                                      \
        "part2_begin=0x1FA00000\0"                                      \
        "part3_begin=0x1FD00000\0"                                      \
        "part4_begin=0x1FD80000\0"                                      \
        "part5_begin=0x1FDC0000\0"                                      \
        "part6_begin=0x1FF00000\0"                                      \
        "total_part=7\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block5=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=6\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0x00040000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_END_ADDR              0x0003FFFF
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x1F9C0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR             0x1F9FFFFF
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x1FA00000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0x00300000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR             0x1FCFFFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x1FD00000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x00180000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x1FE7FFFF
  #define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME          "/dev/mtdblock3"

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x1FE80000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x00040000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x1FEBFFFF
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock4"

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x1FEC0000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x00040000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x1FEFFFFF
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME       "/dev/mtdblock5"

  #define IFX_CFG_FLASH_END_ADDR                          0x1FFFFFFF

 #else /* CONFIG_DUAL_IMAGE */

  #define IFX_CFG_FLASH_PARTITIONS_INFO							\
        "part0_begin=0x00000000\0" /* uboot, 256KB*/			\
        "part1_begin=0x00040000\0" /* rootfs, 30M*/				\
        "part2_begin=0x01E40000\0" /* kernel, 4M*/				\
        "part3_begin=0x02240000\0" /* rootfs2, 30M*/			\
        "part4_begin=0x04040000\0" /* kernel2, 4M*/				\
        "part5_begin=0x04440000\0" /* sysconfig, 1M*/		\
        "part6_begin=0x04540000\0" /* ubootconfig, 1M*/		\
        "part7_begin=0x04640000\0" /* fwdiag, 768K*/			\
        "part8_begin=0x04700000\0" /* voicemail, 440M*/	\
        "part9_begin=0x1FF00000\0" /* BBT, 1M*/				\
        "total_part=10\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO							\
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
        "data_block1=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
        "data_block3=" IFX_CFG_FLASH_ROOTFS2_IMAGE_BLOCK_NAME "\0"	\
        "data_block4=" IFX_CFG_FLASH_KERNEL2_IMAGE_BLOCK_NAME "\0"	\
        "data_block5=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
        "data_block6=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
        "data_block7=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
        "data_block8=" IFX_CFG_FLASH_VOICE_MAIL_BLOCK_NAME "\0"		\
        "total_db=9\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0x00040000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_END_ADDR              0x0003FFFF
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x01E00000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR             0x01E3FFFF
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x01E40000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0x00400000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR             0x0223FFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_BLOCK_NAME          "rootfs2"
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_START_ADDR          0x02240000
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_SIZE                0x01E00000
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_END_ADDR            0x0403FFFF
  #define IFX_CFG_FLASH_ROOTFS2_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock3"

  #define IFX_CFG_FLASH_KERNEL2_IMAGE_BLOCK_NAME          "kernel2"
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_START_ADDR          0x04040000
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_SIZE                0x00400000
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_END_ADDR            0x0443FFFF
  #define IFX_CFG_FLASH_KERNEL2_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock4"

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x04440000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x00100000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x0453FFFF
  #define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME          "/dev/mtdblock5"

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x04540000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x00100000
  #define IFX_CFG_FLASH_UBOOT_CFG_REAL_SIZE               0x00040000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x0463FFFF
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock6"

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x04640000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x000C0000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x046FFFFF
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME       "/dev/mtdblock7"

  #define IFX_CFG_FLASH_VOICE_MAIL_BLOCK_NAME             "voicemail"
  #define IFX_CFG_FLASH_VOICE_MAIL_START_ADDR             0x04700000
  #define IFX_CFG_FLASH_VOICE_MAIL_SIZE                   0x1B800000
  #define IFX_CFG_FLASH_VOICE_MAIL_END_ADDR               0x1FEFFFFF
  #define IFX_CFG_FLASH_VOICE_MAIL_MTDBLOCK_NAME          "/dev/mtdblock8"

  #define IFX_CFG_FLASH_END_ADDR                          0x1FFFFFFF

 #endif /* CONFIG_DUAL_IMAGE */

#else
  #error "ERROR!! Define flash size first!"
#endif

#else /*Not defined CONFIG_FIRMWARE_IN_ROOTFS*/
//2MB flash partition
#if (IFX_CONFIG_FLASH_SIZE == 2)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "part2_begin=0x00040000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO          \
  "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"  \
  "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0" \
  "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0" \
  "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0" \
  "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"   \
  "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"    \
  "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"  \
  "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME          "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR          0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME       "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR       0x00010000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE             0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME    "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME         "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR         0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME         "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR         0x001FADFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE               0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME           "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR           0x001FAE00
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                 0x4000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR             0x001FEDFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME            "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR            0x001FEE00
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                  0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR              0x001FFDFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME        "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR        0x001FFE00
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE              0x0200
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR          0x001FFFFF

  #define IFX_CFG_FLASH_END_ADDR                        0x001FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "part2_begin=0x00040000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME           "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR           0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME        "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR        0x00010000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE              0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME     "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME          "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR          0x00040000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME       "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x003EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x003EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x003FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x003FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x003FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x003FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x003FF3FF
  
  #define IFX_CFG_FLASH_END_ADDR                         0x003FFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 8)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00010000\0"                                      \
        "part2_begin=0x00050000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME         "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0x00010000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x00050000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x007EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x007EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x007FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x007FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x1000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x007FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x007FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x007FF3FF
  
 
  #define IFX_CFG_FLASH_END_ADDR                          0x007FFFFF
  
#elif (IFX_CONFIG_FLASH_SIZE == 128)

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00040000\0"                                      \
        "part2_begin=0x000C0000\0"                                      \
        "part3_begin=0x002C0000\0"                                      \
        "part4_begin=0x07000000\0"                                      \
        "part5_begin=0x07040000\0"                                      \
        "part6_begin=0x07080000\0"                                      \
        "total_part=7\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME         "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0x00040000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x000C0000
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x002C0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x06D40000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock3"

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x07000000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x40000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x0703FFFF
  #define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME          "/dev/mtdblock4"

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x07040000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x10000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x0704FFFF
  #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock5"

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x07080000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x40000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x070BFFFF
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_MTDBLOCK_NAME       "/dev/mtdblock6"

  #define IFX_CFG_FLASH_END_ADDR                          0x07FFFFFF

#elif (IFX_CONFIG_FLASH_SIZE == 512)

  #define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0x00000000\0"                                      \
        "part1_begin=0x00040000\0"                                      \
        "part2_begin=0x000C0000\0"                                      \
        "total_part=3\0"

  #define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"      \
        "total_db=7\0"

  #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
  #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0x00000000
  #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0
  #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME         "firmware"
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0x00040000
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE               0
  #define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME      "/dev/mtdblock1"

  #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0x000C0000
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0
  #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

  #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
  #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0x007EDFFF
  #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0

  #define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME             "sysconfig"
  #define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR             0x007EE000
  #define IFX_CFG_FLASH_SYSTEM_CFG_SIZE                   0x10000
  #define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR               0x007FDFFF

  #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
  #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0x007FE000
  #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x10000
  #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0x007FEFFF

  #define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME          "fwdiag"
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR          0x007FF000
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE                0x400
  #define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR            0x007FF3FF
  
 
  #define IFX_CFG_FLASH_END_ADDR                          0x007FFFFF



#else
  #error "ERROR!! Define flash size first!"
#endif

#endif /*CONFIG_FIRMWARE_IN_ROOTFS*/


#endif /*CONFIG_BOOT_FROM_SPI*/


/* End of Board specific configurations
 *-----------------------------------------------------------------------
 */

/*************************************************************************/

