This is the buildsystem for the OpenWrt Linux distribution.

Please use "make menuconfig" to choose your preferred
configuration for the toolchain and firmware.

You need to have installed gcc, binutils, bzip2, flex, python, perl, make,
find, grep, diff, unzip, gawk, getopt, subversion, libz-dev and libc headers.

Run "./scripts/feeds update -a" to get all the latest package definitions
defined in feeds.conf / feeds.conf.default respectively
and "./scripts/feeds install -a" to install symlinks of all of them into
package/feeds/.

Use "make menuconfig" to configure your image.

Simply running "make" will build your firmware.
It will download all sources, build the cross-compile toolchain, 
the kernel and all choosen applications.

To build your own firmware you need to have access to a Linux, BSD or MacOSX system
(case-sensitive filesystem required). Cygwin will not be supported because of
the lack of case sensitiveness in the file system.


Sunshine!
	Your OpenWrt Community
	http://www.openwrt.org

# Attention for upgrading an older Image
If you want to upgrade an older image ( <2018-03-11 ) with the current one, please backup the data and perform an recovery via fullimage.img method. If not, you end up in a bootloop, which then also has to be resolved via the fullimage.img method.

EasyBox 904 Notes
=================

The project is intended to use the EasyBox as simple IOT device with only network support.
It is possible to make use all of the 5 ports of the switch and run some services.

There fore it is useful to connect to the board via serial adapter.
(Of course you need the unlocked uboot, see https://forum.openwrt.org/viewtopic.php?pid=284534#p284534 how to flash).

## Compilation

### Build Instructions
    git clone https://github.com/Quallenauge/Easybox-904-XDSL.git Easybox-904-XDSL
    cd Easybox-904-XDSL
	./scripts/feeds update && ./scripts/feeds install -a
    cp vr9_default.config .config
    make oldconfig
    make V=s

The resulting image can be found on
`bin/targets/lantiq/xrx200/openwrt-lantiq-xrx200-lantiq_vgv952cjw33-e-ir-squashfs-rootfs-ubinized.bin`

## Prebuilts
There are some snapshots from the current build. Please refer [this link](https://app.box.com/s/hvqg535dnubt4r2ontpmtodpvt6ydf00) for details.

## Installing
Refer  [Install instructions at the openwrt wiki](https://openwrt.org/toh/astoria/arcadyan_astoria_easybox_904xdsl_r01?s%5B%5D=arcadyan&s%5B%5D=astoria&s%5B%5D=easybox&s%5B%5D=904xdsl#installing_lede) as base.
For reference the images will be uploaded here: [Firmware Snapshots](https://app.box.com/s/hvqg535dnubt4r2ontpmtodpvt6ydf00/folder/36913951101 "Firmware Snapshots")
Lately it seems, the fullimage.img doesn't work right now, please use the newest one from: [fullimage.img](https://app.box.com/s/tjeobifjb8ohj90m5k2u7g1efgq8308y) .

## Tips&Tricks
### UBoot
A compile able uboot is integrated in the master-lede branch. It is untested!
A working uboot can be compiled when using this branch: https://github.com/Quallenauge/Easybox-904-XDSL/tree/master.

It is modified to accept all provided passwords in the same way the one at the openwrt post is used.
As addition the bootnum functionality is disabled to save some uboot settings re-flash cycles at boot.
Also the network is always switched on, to support boot over tftp per default.

A prebuilt one can be found [here](https://mega.nz/#!bA1gADqB!TtTdK3ePNdRjYnCfmM4an4JLbbucFJ2KvEe3U3cnTvo).

### Stock-Rootfs from sdcard
The uboot environment variables has to be modified in the following way, assuming the rootfs is of type `ext2` and is stored at `sda1`.

    setenv addbootargs setenv bootargs \\${bootargs} root=/dev/sda1 rootdelay=7 init=/etc/preinit rootfstype=ext2
    setenv bootflash nand read.e \\$(loadaddr) \\$(f\_kernel\_addr) \\$(f\_kernel\_size) \\; run addip addmisc addbootargs \\; bootm \\$(loadaddr)
    setenv bootcmd run bootflash
    saveenv

### Manual installing a customized kernel
The kernel is updated via (assume the uImage is in the current path, PLEASE refer to the ToDos at openwrt to avoid bricks
https://openwrt.org/toh/astoria/arcadyan_astoria_easybox_904xdsl_r01?s[]=arcadyan&s[]=astoria&s[]=easybox&s[]=904xdsl#installing_openwrt

    flash_eraseall /dev/mtd2
    nandwrite -p /dev/mtd2 uImage


The memory card should have a partition which contains the rootfs. The partition should correspond
to the uboot root setting  (e.g. /dev/sda1).
