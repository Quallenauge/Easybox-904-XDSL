. /lib/functions/lantiq.sh

PART_NAME=firmware
REQUIRE_IMAGE_METADATA=1

platform_check_image() {
	return 0
}

platform_pre_upgrade() {
	local board=$(lantiq_board_name)

	case "$board" in
	BTHOMEHUBV2B|BTHOMEHUBV3A|BTHOMEHUBV5A|P2812HNUF*|VGV952CJW33-E-IR )
		nand_do_upgrade $1
		;;
	esac
}

platform_nand_pre_upgrade() {
	local board=$(lantiq_board_name)
	echo "platform_nand_pre_upgrade()"

	case "$board" in
	VGV952CJW33-E-IR )
		# This is a global defined in nand.sh, sets another VID header offset than default.
		echo "Set header offset"
		CU_UBI_VID_HEADER_OFFSET="2048"
		;;
	esac
}

# use default for platform_do_upgrade()

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}
append sysupgrade_pre_upgrade disable_watchdog
