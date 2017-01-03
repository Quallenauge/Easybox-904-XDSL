#!/bin/sh

#
# copied from /flash/BSP-Test-VR9/bringup_xdsl_mode.sh
# merge /etc/init.d/network
#

OUT_FILE=/dev/console
RC_CONF=/etc/config/rc.conf
SYS_CONF=/etc/config.sh
KERNEL_VER=`uname -r`

DFT_LAN1_VID="66"
DFT_CPU_VID="60"
DFT_WAN_VID="61"
DFT_WAN_PPP_VID="62"
DFT_BR_PVC_VID_LIST="4080 4081 4082 4083 4084 4085 4086 4087 4088 4089 4090 4091 4092 4093 4094"  # MUST NOT CHANGE
DFT_BR_COMMON_VID="60"
DFT_VOIP_VID="63"
DFT_IPTV_VID="64"
DFT_CPU_FID="1"
DFT_LAN_FID="1"
DFT_WAN_FID="2"
AUTO_SENSING_WORKAROUNG="0"
umngProg=""

set_eth1_mac()
{
	if [ `ifconfig eth1 | grep -cw "UP"` -ge 1 ] ; then
		ifconfig eth1 down
		ifconfig eth1 hw ether `getmacaddr.sh wan 0`
		ifconfig eth1 up
	else
		ifconfig eth1 hw ether `getmacaddr.sh wan 0`
	fi
}

# $1~$* - wan interfaces
add_ppa_netdev()
{
	ifconfig eth0 down
	ifconfig eth0 hw ether `getmacaddr.sh lan 0`
	ifconfig eth0 up
	ifconfig br-lan up
	ppacmd addlan -i eth0    >& $OUT_FILE
	ppacmd addlan -i eth0.66    >& $OUT_FILE
	ppacmd addlan -i eth0.67    >& $OUT_FILE
	ppacmd addlan -i eth0.68    >& $OUT_FILE
	ppacmd addlan -i eth0.69    >& $OUT_FILE
	ppacmd addlan -i eth0.70    >& $OUT_FILE
	ppacmd addlan -i eth0.71    >& $OUT_FILE
#	echo add lan eth0 > /proc/ppa/api/netif
	ppacmd addlan -i br-lan  >& $OUT_FILE
#	echo add lan br-lan > /proc/ppa/api/netif
	echo update br-lan > /proc/ppa/api/netif

#   add br-lan1 in ppa (for WLAN2 in 904 DSL)
	ppacmd addlan -i br-lan1  >& $OUT_FILE
	echo update br-lan1 > /proc/ppa/api/netif

	if [ -z "$1" ] ; then
		return
	fi

#	if [ "${1:0:3}" == "lte" ] ; then
#		return
#	fi

	ethaddr=`getmacaddr.sh wan 0`
	while [ -n "$1" ] ; do
		ifconfig $1 down
		ifconfig $1 hw ether $ethaddr
		ifconfig $1 up
		ppacmd addwan -i $1  >& $OUT_FILE
#		echo add wan $1 > /proc/ppa/api/netif
		echo update $1 > /proc/ppa/api/netif
		shift
		ethaddr=`echo $ethaddr | next_macaddr 0`
	done
}

del_ppa_netdev()
{
#	ALLNETIF=`ifconfig | grep "Link encap:Ethernet" | awk '{ print $1 }'`
#	for NETIF in $ALLNETIF ; do
#		ifconfig $NETIF down
#	done

#	ALLNETIF=`ppacmd getlan 2> /dev/null | awk '{ print $1 }'` # AR9
	ALLNETIF=`ppacmd getlan 2> /dev/null | awk '{ print $3 }'` # VR9
	for NETIF in $ALLNETIF ; do
		ppacmd dellan -i $NETIF  >& $OUT_FILE
#		echo del lan $NETIF > /proc/ppa/api/netif
	done

#	ALLNETIF=`ppacmd getwan 2> /dev/null | awk '{ print $1 }'` # AR9
	ALLNETIF=`ppacmd getwan 2> /dev/null | awk '{ print $3 }'` # VR9
	for NETIF in $ALLNETIF ; do
		ppacmd delwan -i $NETIF  >& $OUT_FILE
#		echo del wan $NETIF > /proc/ppa/api/netif
	done
}

# $1~$* - wan interfaces
init_netdev()
{
	echo disable > /proc/eth/flowcontrol

	/sbin/insmod ksysinfo.ko

	echo 1 > /proc/sys/net/ipv4/ip_forward

	# only for VoIP
	/bin/echo "t0 0x0" > /proc/mips/mtsched
	/bin/echo "t1 0x1" > /proc/mips/mtsched
	/bin/echo "v0 0x0" > /proc/mips/mtsched

	echo enable > /proc/ppa/api/hook # after this line, PPA hook adds itself default interfaces

	usleep 100000

	ppacmd control --disable-lan --disable-wan  >& $OUT_FILE

	del_ppa_netdev # remove PPA hook default interfaces, e.g. br0 and eth0.5

	ppacmd control --enable-lan --enable-wan  >& $OUT_FILE

	add_ppa_netdev $*
}

check_bridging_vlan()
{
    local tmp_wan_type=$1
    local vlan1_phy_ports=$2
    local vlan2_phy_ports=$3
    local vlan3_phy_ports=$4
    local vlan4_phy_ports=$5
    local wan_base
    local brwan_mask
    local vlan_vid
    local vlan_ports
    local wan_vid
    local wan_sec
    local wan_brvlan_idx
    local wan_tagged
    local v_eb
    local w_eb
    local m1
    local m2
    local CNT1
    local CNT2
    local p_port
    local PCE_rule_cnt

    case "$tmp_wan_type" in
        "1")
            wan_base=0;;
        "2")
            wan_base=100;;
        "3")
            wan_base=50;;
        *)
            wan_base=0;;
    esac

    PCE_rule_cnt=0
    while [ $PCE_rule_cnt -lt 4 ]; do
    	switch_utility PCE_RuleDelete $PCE_rule_cnt
	let PCE_rule_cnt=$PCE_rule_cnt+1
    done
    PCE_rule_cnt=0

    CNT1=1
    while [ $CNT1 -le 4 ]; do
        v_eb=`ccfg_cli get adminstate@lan$(($CNT1-1))`
        if [ "$v_eb" != "enable" ]; then
            let CNT1=$CNT1+1
            continue
        fi
        #brwan_mask=`ccfg_cli get vlan${CNT1}_brwan@vlan`
        brwan_mask=`ccfg_cli get brwan@lan$(($CNT1-1))`
        if [ $brwan_mask -gt 0 ]; then
            CNT2=0
            while [ $CNT2 -lt 8 ]; do
                m1=$((1<<$CNT2))
                m2=$(($m1&$brwan_mask))
                if [ "$m2" -gt 0 ]; then
                    wan_sec=`printf "wan%03d" $(($CNT2+$wan_base))`
                    w_eb=`ccfg_cli get adminstate@${wan_sec}`
                    if [ "$w_eb" == "enable" ]; then
                        wan_brvlan_idx=`ccfg_cli get brvlan_idx_1b@${wan_sec}`
                        if [ "$wan_brvlan_idx" == "$CNT1" ]; then
		                    wan_tagged=`ccfg_cli get vlan_tagged@${wan_sec}`
		                    if [ "$wan_tagged" == "1" ]; then
			                    wan_vid=`ccfg_cli get vlan_id@${wan_sec}`
			                else
			                    wan_vid=$DFT_WAN_VID
		                    fi

		                    vlan_ports=$(eval "echo \$vlan${CNT1}_phy_ports")
		                    for p_port in $vlan_ports; do

		                        ## ygchen, since we use different FIDs for LAN and WAN groups, it's not necessary to specially add port member to $wan_vid
		                        ## and use PCE_Rule to direct all LAN ingress traffic to WAN
		                        ## cons: WAN to LAN bridging traffic will be up to CPU/PPE, not directly forwarded by the switch
		                        #switch_utility VLAN_PortMemberAdd $wan_vid $p_port 0
                                #echo "switch_utility VLAN_PortMemberAdd $wan_vid $p_port 0" >> /tmp/sync_rst

		                        # have all LAN-to-WAN bridging traffic up to CPU/PPE
		                        #switch_utility PCE_RuleWrite $PCE_rule_cnt 1 1 $p_port 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0 0 00:00:00:00:00:00 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0x0 0x0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0 0 2
                                #echo "switch_utility PCE_RuleWrite $PCE_rule_cnt 1 1 $p_port 0 0 0..." >> /tmp/sync_rst

		                        let PCE_rule_cnt=$PCE_rule_cnt+1
                            done
                        fi
                    fi
                fi
                let CNT2=$CNT2+1
            done
        fi
        let CNT1=$CNT1+1
    done
}

mc_remove_pce()
{
	# remove old PCE
	switch_utility PCE_RuleDelete 23
	switch_utility PCE_RuleDelete 24
	switch_utility PCE_RuleDelete 25
	switch_utility PCE_RuleDelete 26
	switch_utility PCE_RuleDelete 27
	switch_utility PCE_RuleDelete 28
	switch_utility PCE_RuleDelete 29

	# in UGW 5.1, the MulticastSnoopCfgSet PCE rules are added in #50~55.
	switch_utility PCE_RuleDelete 50
	switch_utility PCE_RuleDelete 51
	switch_utility PCE_RuleDelete 52
	switch_utility PCE_RuleDelete 53
	switch_utility PCE_RuleDelete 54
	switch_utility PCE_RuleDelete 55
}

mc_add_pce()
{
	# workaround for IGMP packet forward between LAN and CPU port issue in UGW4.3.1 and UGW5.1
	switch_utility PCE_RuleWrite 23 1 1 6 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 0    0 0 0 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 0 0 0 0 0 0 0 0 1  1 0 0 0 0 0 0 0 0 0 0 0

	# re-add PCE
	switch_utility PCE_RuleWrite 24 1 0 0 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 1 4608 1 3 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 1 1 0 0 0 0 0 0 4 64 0 0 0 0 0 0 0 0 0 0 0
	switch_utility PCE_RuleWrite 25 1 0 0 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 1 4864 1 3 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 1 1 0 0 0 0 0 0 4 64 0 0 0 0 0 0 0 0 0 0 0
	switch_utility PCE_RuleWrite 26 1 0 0 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 1 5632 1 3 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 1 1 0 0 0 0 0 0 4 64 0 0 0 0 0 0 0 0 0 0 0
	switch_utility PCE_RuleWrite 27 1 0 0 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 1 5888 1 3 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 1 1 0 0 0 0 0 0 4 64 0 0 0 0 0 0 0 0 0 0 0
	switch_utility PCE_RuleWrite 28 1 0 0 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 1 8704 1 3 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 1 1 0 0 0 0 0 0 4 64 0 0 0 0 0 0 0 0 0 0 0
	switch_utility PCE_RuleWrite 29 1 0 0 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0x0 0 00:00:00:00:00:00 0x0 1 4352 1 3 0 0 0 0 0 000.000.000.000 0x0 0 000.000.000.000 0x0 1 2048 0x0 1 2 0x0 0 0 0 0 1 1 0 0 0 0 0 0 4 64 0 0 0 0 0 0 0 0 0 0 0
}

mc_snoop_enable()
{
	# Enable Multicast snooping function
	## echo "Enable Multicast snooping function"
	## switch_utility MulticastSnoopCfgSet 1 0 0 0 0 0 3 0x64 0 1 1
	switch_utility MulticastSnoopCfgSet 2 1 0 3 6 1 3 100 2 1 1

	mc_remove_pce
	mc_add_pce
}

mc_snoop_disable()
{
	switch_utility MulticastSnoopCfgSet 0 0 0 0 6 1 3 100 2 1 1

	mc_remove_pce
}

vr9_switch_mac_table_flush_patch()
{
	switch_utility RegisterSet 0x440 0x1f1f
	switch_utility RegisterSet 0x441 0x7f0f
	switch_utility RegisterSet 0x442 0x7f7f
	switch_utility RegisterSet 0x443 0x7f7f
	switch_utility RegisterSet 0x444 0x1f7f
	switch_utility RegisterSet 0x445 0x1f7f
	switch_utility RegisterSet 0x446 0x3f3f
	switch_utility RegisterSet 0x447 0x7f1f
	switch_utility RegisterSet 0x44D 0x0200
	switch_utility RegisterSet 0x44C 0x0000
	switch_utility RegisterSet 0x44B 0x0000
	switch_utility RegisterSet 0x44A 0x0000
	switch_utility RegisterSet 0x449 0x03
	switch_utility RegisterSet 0x44E 0x0 # use otherwise unused Traffic flow index
	switch_utility RegisterSet 0x44F 0x902f
}

ROUTING="1"
WAN_TYPE="0"
ACTIVE_WAN_TYPE="2"
ENABLE_VLAN=""
ADSL_WAN_ENABLE="0"
ETH_WAN_ENABLE="0"
VDSL_WAN_ENABLE="0"
LTE_WAN_ENABLE="0"
WAN_SECTION_NAME=""
VOIP_SECTION_NAME=""
IPTV_SECTION_NAME=""
WAN_INTERFACE_ENABLE="0"
VOIP_INTERFACE_ENABLE="0"
IPTV_INTERFACE_ENABLE="0"
ETH_WAN_LOGICAL_PORT=""
LOGICAL_CPU_PORT=""
PORT_PHYCONF=""
LAN_PORT=""
ETH_WAN_PORT_SEQ=""
ETH_WAN_PORT=""
# For auto-sensing case with ADSL/VDSL/Ether-Uplink mode, we must control WAN Port PHY when no cable line connecting or Ether-Uplink line connecting.
CONFIG_ETH_WAN_PORT=""
# ETH_LAN1_PORT(LTE)="0 1 2 3 4 5"  # "1 2 3 4 5"
# ETH_LAN1_PORT(non-LTE)="0 1 2 3 4 5 7 8"  # "1 2 3 4 5"
ETH_LAN1_PORT="0 1 2 3 4 5 7 8"  # "1 2 3 4 5"
ETH_LAN2_PORT=""
ETH_LAN3_PORT=""
ETH_LAN4_PORT=""
ETH_PORT="$ETH_WAN_PORT $ETH_LAN1_PORT"
PPP_VPORT="10" # PPPoA or IPoA
DSL_VPORT="11" # PPPoE or IPoE
DSL_PORT="$DSL_VPORT $PPP_VPORT"
CPU_PORT_SEQ=""
CPU_PORT="6"
NA_PORT="9" # unknown ports
CPU_VID=$DFT_CPU_VID
WAN_VID=$DFT_WAN_VID
VOIP_VID=$DFT_VOIP_VID
IPTV_VID=$DFT_IPTV_VID
LAN_VID=$DFT_LAN1_VID
LAN_VLAN_ID=""
LAN1_VID=$DFT_LAN1_VID
LAN2_VID=""
LAN3_VID=""
LAN4_VID=""
LAN1_TAGGED_VLAN="0"
LAN2_TAGGED_VLAN="0"
LAN3_TAGGED_VLAN="0"
LAN4_TAGGED_VLAN="0"
WAN_PPP_VID=$DFT_WAN_PPP_VID
# for bridging
BR_PVC_VID_LIST=$DFT_BR_PVC_VID_LIST
BR_COMMON_VID=$DFT_BR_COMMON_VID
# end of for bridging
WAN_TAGGED_VLAN="0"
VOIP_TAGGED_VLAN="0"
IPTV_TAGGED_VLAN="0"
LAN_SECTION_NAME="lan0"
LTE_VPORT="7" #
LTE2_VPORT="8" #
LTE3_VPORT="8" #


start_ethsw_set_variable_to_default_value()
{
	local	wan_type
	local	active_wan_type

	ROUTING="1"
	WAN_TYPE="0"
	ACTIVE_WAN_TYPE="2"
	ENABLE_VLAN=""
	ADSL_WAN_ENABLE="0"
	ETH_WAN_ENABLE="0"
	VDSL_WAN_ENABLE="0"
	LTE_WAN_ENABLE="0"
	WAN_SECTION_NAME=""
	VOIP_SECTION_NAME=""
	IPTV_SECTION_NAME=""
	WAN_INTERFACE_ENABLE="0"
	VOIP_INTERFACE_ENABLE="0"
	IPTV_INTERFACE_ENABLE="0"
	ETH_WAN_LOGICAL_PORT=""
	LOGICAL_CPU_PORT=""
	PORT_PHYCONF=""
	LAN_PORT=""
	ETH_WAN_PORT=""
	ETH_LAN1_PORT="0 1 2 3 4 5 7 8"  # "1 2 3 4 5"
	ETH_LAN2_PORT=""
	ETH_LAN3_PORT=""
	ETH_LAN4_PORT=""
	ETH_PORT="$ETH_WAN_PORT $ETH_LAN1_PORT"
	PPP_VPORT="10" # PPPoA or IPoA
	DSL_VPORT="11" # PPPoE or IPoE
	DSL_PORT="$DSL_VPORT $PPP_VPORT"
	CPU_PORT="6"
	NA_PORT="9" # unknown ports
	CPU_VID=$DFT_CPU_VID
	WAN_VID=$DFT_WAN_VID
	VOIP_VID=$DFT_VOIP_VID
	IPTV_VID=$DFT_IPTV_VID
	LAN_VID=$DFT_LAN1_VID
	LAN_VLAN_ID=""
	LAN1_VID=$DFT_LAN1_VID
	LAN2_VID=""
	LAN3_VID=""
	LAN4_VID=""
	LAN1_TAGGED_VLAN="0"
	LAN2_TAGGED_VLAN="0"
	LAN3_TAGGED_VLAN="0"
	LAN4_TAGGED_VLAN="0"
	WAN_PPP_VID=$DFT_WAN_PPP_VID
	# for bridging
	BR_PVC_VID_LIST=$DFT_BR_PVC_VID_LIST
	BR_COMMON_VID=$DFT_BR_COMMON_VID
	# end of for bridging
	WAN_TAGGED_VLAN="0"
	VOIP_TAGGED_VLAN="0"
	IPTV_TAGGED_VLAN="0"
	LAN_SECTION_NAME="lan0"
	LTE_VPORT="7"
	LTE2_VPORT="8"
	LTE3_VPORT="8"

	wan_type=`ccfg_cli get wan_type@system`
	if [ "$wan_type" == "0" ] ; then
		# auto-detection
		active_wan_type=`ccfg_cli get active_wan_type@system`
		if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
			active_wan_type=`ccfg_cli get pre_dsl_wan_type@system`
			if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
				active_wan_type="1"
			fi
		fi
	else
		active_wan_type=$wan_type
	fi

	if [ "$active_wan_type" == "5" ] ; then
		# LTE mode
		ETH_LAN1_PORT="0 1 2 3 4 5"  # "1 2 3 4 5"
		ETH_PORT="$ETH_WAN_PORT $ETH_LAN1_PORT"
	fi
}

start_ethsw_ethernet()
{
	local IFNAME=""
	local tmp_lan_port=""
	echo "[setup_netdev.sh start_ethsw_ethernet]" >> $OUT_FILE

	if [ "$ROUTING" == "1" ] ; then # routing/NAT
		# find physical wan port, set CPU_PORT value, set ETH_WAN_PORT value
		ETH_WAN_PORT=""
		tmp_idx="0"
		for argument in $PORT_PHYCONF ; do
			if [ "$ETH_WAN_ENABLE" == "1" ] ; then
				if [ "$ETH_WAN_LOGICAL_PORT" == "$tmp_idx" ] ; then
					ETH_WAN_PORT="$argument"
				fi
			fi
			tmp_idx=`expr $tmp_idx + 1`
		done
		echo "[start_ethsw_ethernet] ETH_WAN_PORT=$ETH_WAN_PORT"

		# set ETH_PORT value
		if [ "$ENABLE_VLAN" == "0" ] ; then
			ETH_PORT="$ETH_WAN_PORT $ETH_LAN1_PORT"
		else
			tmp_lan_port=$ETH_PORT
			ETH_PORT="$ETH_WAN_PORT $tmp_lan_port"
		fi
		echo "[start_ethsw_ethernet] ETH_PORT=$ETH_PORT"

		# Set global switch parameter
		## echo "Set global switch parameter"
		switch_utility CfgSet 3 1 1536 0 00:00:00:00:00:00
		if [ -n "$ETH_WAN_PORT" ] ; then
			switch_utility RegisterSet 0xCCD $(( 1 << $ETH_WAN_PORT ))
		else
			switch_utility RegisterSet 0xCCD 0x0
		fi

		# Mirror Portmap
		## echo "Mirror Portmap"
		switch_utility RegisterSet 0x453 0x0
		# Unknown Unicast/multicast Packet
		## echo "Unknown Unicast/multicast Packet"
		switch_utility RegisterSet 0x454 0x07f
		switch_utility RegisterSet 0x455 0x07f

		# Create switch internal LAN VLAN ID
		switch_utility VLAN_IdCreate $LAN1_VID $DFT_LAN_FID

		if [ "$LAN2_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN2_VID $DFT_LAN_FID
		fi

		if [ "$LAN3_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN3_VID $DFT_LAN_FID
		fi

		if [ "$LAN4_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN4_VID $DFT_LAN_FID
		fi

		# Create switch internal CPU VLAN ID
		switch_utility VLAN_IdCreate $CPU_VID $DFT_CPU_FID
		# Create switch internal WAN VLAN IDs
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "[start_ethsw_ethernet] call VLAN_IdCreate for WAN"
			switch_utility VLAN_IdCreate $WAN_VID $DFT_WAN_FID
		fi

		if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
			echo "[start_ethsw_ethernet] call VLAN_IdCreate for VOIP"
			switch_utility VLAN_IdCreate $VOIP_VID $DFT_WAN_FID
		fi

		if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
			echo "[start_ethsw_ethernet] call VLAN_IdCreate for IPTV"
			switch_utility VLAN_IdCreate $IPTV_VID $DFT_WAN_FID
		fi

		# Set ETH LAN Port VLAN configuration
		## echo "Set ETH LAN Port VLAN configuration"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortCfgSet $argument $LAN1_VID 0 0 3 0 0
		done

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN2_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN3_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN4_VID 0 0 3 0 0
			done
		fi

		# CPU Port VLAN configuration
		## echo "CPU Port VLAN configuration"
		switch_utility VLAN_PortCfgSet $CPU_PORT $CPU_VID 0 0 3 0 0

		# Ethernet WAN Port VLAN configuration
		if [ -n "$ETH_WAN_PORT" ] ; then
			## echo "Ethernet WAN Port VLAN configuration, eth_wan"

			if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $ETH_WAN_PORT $WAN_VID 0 0 3 0 0
				IFNAME=`ccfg_cli get ifname@$WAN_SECTION_NAME`
				ifconfig $IFNAME down
				ifconfig $IFNAME hw ether `getmacaddr.sh wan 0`
				ifconfig $IFNAME up

				switch_utility MAC_TableEntryRemove 0 `getmacaddr.sh wan 0`
				switch_utility MAC_TableEntryAdd 2 $ETH_WAN_PORT 0 1 `getmacaddr.sh wan 0`
				switch_utility MAC_TableEntryAdd 2 $CPU_PORT 0 1 `getmacaddr.sh wan 0`
			fi

			if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $ETH_WAN_PORT $VOIP_VID 0 0 3 0 0
				#VLAN_TAGGED=`ccfg_cli get vlan_tagged@$VOIP_SECTION_NAME`
				if [ "$VOIP_TAGGED_VLAN" != "1" ]; then
					IF_MAC=`getmacaddr.sh wan 1`
				else
					IF_MAC=`getmacaddr.sh wan 0`
				fi
				IFNAME=`ccfg_cli get ifname@$VOIP_SECTION_NAME`
				ifconfig $IFNAME down
				ifconfig $IFNAME hw ether $IF_MAC
				ifconfig $IFNAME up

				switch_utility MAC_TableEntryRemove 0 $IF_MAC
				switch_utility MAC_TableEntryAdd 2 $ETH_WAN_PORT 0 1 $IF_MAC
				switch_utility MAC_TableEntryAdd 2 $CPU_PORT 0 1 $IF_MAC
			fi

			if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $ETH_WAN_PORT $IPTV_VID 0 0 3 0 0
				#VLAN_TAGGED=`ccfg_cli get vlan_tagged@$IPTV_SECTION_NAME`
				if [ "$IPTV_TAGGED_VLAN" != "1" ]; then
					IF_MAC=`getmacaddr.sh wan 2`
				else
					IF_MAC=`getmacaddr.sh wan 0`
				fi 
				IFNAME=`ccfg_cli get ifname@$IPTV_SECTION_NAME`
				ifconfig $IFNAME down
				ifconfig $IFNAME hw ether $IF_MAC
				ifconfig $IFNAME up

				switch_utility MAC_TableEntryRemove 0 $IF_MAC
				switch_utility MAC_TableEntryAdd 2 $ETH_WAN_PORT 0 1 $IF_MAC
				switch_utility MAC_TableEntryAdd 2 $CPU_PORT 0 1 $IF_MAC
			fi
		fi

		# LAN VLAN membership
		## echo "LAN VLAN membership"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortMemberAdd $LAN1_VID $argument $LAN1_TAGGED_VLAN
		done

		if [ "$ENABLE_VLAN" == "1" ] ; then
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 1
		else
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 0
		fi

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN2_VID $argument $LAN2_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN3_VID $argument $LAN3_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN4_VID $argument $LAN4_TAGGED_VLAN
		done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 0
			fi
		fi

		# WAN VLAN membership
		## echo "WAN VLAN membership"
		if [ -n "$ETH_WAN_PORT" ] ; then
			## echo "eth_wan"
			if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
				echo "WAN_TAGGED_VLAN=${WAN_TAGGED_VLAN}, call VLAN_PortMemberAdd for WAN"
				for argument in $ETH_WAN_PORT ; do
					switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
				done
				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
				done
			fi

			if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				echo "VOIP_TAGGED_VLAN=${VOIP_TAGGED_VLAN}, call VLAN_PortMemberAdd for VOIP"
				for argument in $ETH_WAN_PORT ; do
					switch_utility VLAN_PortMemberAdd $VOIP_VID $argument $VOIP_TAGGED_VLAN
				done
				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $VOIP_VID $argument $VOIP_TAGGED_VLAN
				done
			fi

			if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				echo "IPTV_TAGGED_VLAN=${IPTV_TAGGED_VLAN}, call VLAN_PortMemberAdd for IPTV"
				for argument in $ETH_WAN_PORT ; do
					switch_utility VLAN_PortMemberAdd $IPTV_VID $argument $IPTV_TAGGED_VLAN
				done
				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $IPTV_VID $argument $IPTV_TAGGED_VLAN
				done
			fi
		fi

		# CPU VLAN membership ( Super group )
		## echo "CPU VLAN membership"
		if [ -n "$ETH_WAN_PORT" ] ; then
			## echo "eth_wan"
			for argument in $ETH_PORT $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $CPU_VID $argument 0
			done
		fi

		# CPU Port is default router port map
		## echo "CPU Port is default router port map"
		switch_utility MulticastRouterPortAdd $CPU_PORT
		# Ethernet WAN Port is default router port map
		## echo "Ethernet WAN Port is default router port map"
		if [ -n "$ETH_WAN_PORT" ] ; then
			## echo "eth_wan"
			switch_utility MulticastRouterPortAdd $ETH_WAN_PORT
		fi

		## ygchen, since we use different FIDs for LAN and WAN groups, it's not necessary to do the function below
		## but please don't remove it (for reference)
	    #check_bridging_vlan $ACTIVE_WAN_TYPE "$ETH_LAN1_PORT" "$ETH_LAN2_PORT" "$ETH_LAN3_PORT" "$ETH_LAN4_PORT"
	else # bridging
		# Set global switch parameter
		## echo "Set global switch parameter"
		switch_utility CfgSet 3 1 1536 0 00:00:00:00:00:00
		if [ -n "$ETH_WAN_PORT" ] ; then
			switch_utility RegisterSet 0xCCD $(( 1 << $ETH_WAN_PORT ))
		else
			switch_utility RegisterSet 0xCCD 0x0
		fi

		# Mirror Portmap
		## echo "Mirror Portmap"
		switch_utility RegisterSet 0x453 0x0
		# Unknown Unicast/multicast Packet
		## echo "Unknown Unicast/multicast Packet"
		switch_utility RegisterSet 0x454 0x07f
		switch_utility RegisterSet 0x455 0x07f

		if [ -n "$ETH_WAN_PORT" ] ; then
			# Ethernet WAN Port is default router port map
			switch_utility MulticastRouterPortAdd $ETH_WAN_PORT
		fi
	fi
}

start_ethsw_lte()
{
	local tmp_lan_port=""

	echo "[setup_netdev.sh start_ethsw_lte]" >> $OUT_FILE

	if [ "$ROUTING" == "1" ] ; then # routing/NAT
		# find physical wan port, set CPU_PORT value, set ETH_WAN_PORT value
		ETH_WAN_PORT=""
		tmp_idx="0"
		for argument in $PORT_PHYCONF ; do
			if [ "$ETH_WAN_ENABLE" == "1" ] ; then
				if [ "$ETH_WAN_LOGICAL_PORT" == "$tmp_idx" ] ; then
					ETH_WAN_PORT="$argument"
				fi
			fi
			tmp_idx=`expr $tmp_idx + 1`
		done
		echo "[start_ethsw_lte] ETH_WAN_PORT=$ETH_WAN_PORT"

		# set ETH_PORT value
		if [ "$ENABLE_VLAN" == "0" ] ; then
			ETH_PORT="$ETH_WAN_PORT $ETH_LAN1_PORT"
		else
			tmp_lan_port=$ETH_PORT
			ETH_PORT="$ETH_WAN_PORT $tmp_lan_port"
		fi
		echo "[start_ethsw_lte] ETH_PORT=$ETH_PORT"

		# Set global switch parameter
		## echo "Set global switch parameter"
		switch_utility CfgSet 3 1 1536 0 00:00:00:00:00:00
		switch_utility RegisterSet 0xCCD 0x0

		# Mirror Portmap
		## echo "Mirror Portmap"
		switch_utility RegisterSet 0x453 0x0
		# Unknown Unicast/multicast Packet
		## echo "Unknown Unicast/multicast Packet"
		switch_utility RegisterSet 0x454 0x07f
		switch_utility RegisterSet 0x455 0x07f

		# Create switch internal LAN VLAN ID
		switch_utility VLAN_IdCreate $LAN1_VID 1

		if [ "$LAN2_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN2_VID 1
		fi

		if [ "$LAN3_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN3_VID 1
		fi

		if [ "$LAN4_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN4_VID 1
		fi

		# Create switch internal CPU VLAN ID
		switch_utility VLAN_IdCreate $CPU_VID 1
		# Create switch internal WAN VLAN IDs
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "[start_ethsw_lte] call VLAN_IdCreate for WAN"
			switch_utility VLAN_IdCreate $WAN_VID 2
		fi

		if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				echo "[start_ethsw_lte] call VLAN_IdCreate for VOIP"
				switch_utility VLAN_IdCreate $VOIP_VID 2
			fi

		if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				echo "[start_ethsw_lte] call VLAN_IdCreate for IPTV"
				switch_utility VLAN_IdCreate $IPTV_VID 2
			fi

		# Set ETH LAN Port VLAN configuration
		## echo "Set ETH LAN Port VLAN configuration"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortCfgSet $argument $LAN1_VID 0 0 3 0 0
		done

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN2_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN3_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN4_VID 0 0 3 0 0
			done
		fi

		# CPU Port VLAN configuration
		## echo "CPU Port VLAN configuration"
		switch_utility VLAN_PortCfgSet $CPU_PORT $CPU_VID 0 0 3 0 0

		# DSL WAN Port VLAN configuration
		## echo "DSL WAN Port VLAN configuration, vdsl, adsl_a, adsl_b"
		###### switch_utility VLAN_PortCfgSet $DSL_VPORT $WAN_VID 0 0 3 0 1

		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			switch_utility VLAN_PortCfgSet $LTE_VPORT $WAN_VID 0 0 3 0 1
		fi

			if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $LTE2_VPORT $VOIP_VID 0 0 3 0 1
			fi

			if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $LTE3_VPORT $IPTV_VID 0 0 3 0 1
			fi

		# LAN VLAN membership
		## echo "LAN VLAN membership"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortMemberAdd $LAN1_VID $argument $LAN1_TAGGED_VLAN
		done

		if [ "$ENABLE_VLAN" == "1" ] ; then
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 1
		else
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 0
		fi

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN2_VID $argument $LAN2_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN3_VID $argument $LAN3_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN4_VID $argument $LAN4_TAGGED_VLAN
		done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 0
			fi
		fi

		# WAN VLAN membership
		## echo "WAN VLAN membership"
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "WAN_TAGGED_VLAN=${WAN_TAGGED_VLAN}, call VLAN_PortMemberAdd for WAN"
			tmp_vport=$LTE_VPORT
			for argument in $tmp_vport ; do
				switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
			done

			for argument in $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
			done
		fi

			if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				echo "VOIP_TAGGED_VLAN=${VOIP_TAGGED_VLAN}, call VLAN_PortMemberAdd for VOIP"
			tmp_vport=$LTE2_VPORT
				for argument in $tmp_vport ; do
					switch_utility VLAN_PortMemberAdd $VOIP_VID $argument $VOIP_TAGGED_VLAN
				done

				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $VOIP_VID $argument $VOIP_TAGGED_VLAN
				done
			fi

			if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				echo "IPTV_TAGGED_VLAN=${IPTV_TAGGED_VLAN}, call VLAN_PortMemberAdd for IPTV"
			tmp_vport=$LTE3_VPORT
				for argument in $tmp_vport ; do
					switch_utility VLAN_PortMemberAdd $IPTV_VID $argument $IPTV_TAGGED_VLAN
				done

				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $IPTV_VID $argument $IPTV_TAGGED_VLAN
				done
			fi

		# CPU VLAN membership ( Super group )
		## echo "CPU VLAN membership"
		tmp_vport=$LTE_VPORT
		for argument in $ETH_PORT $tmp_vport $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $CPU_VID $argument 0
		done

		# CPU Port is default router port map
		## echo "CPU Port is default router port map"
		switch_utility MulticastRouterPortAdd $CPU_PORT
		# Ethernet WAN Port is default router port map

		## ygchen, since we use different FIDs for LAN and WAN groups, it's not necessary to do the function below
		## but please don't remove it (for reference)
	    #check_bridging_vlan $ACTIVE_WAN_TYPE "$ETH_LAN1_PORT" "$ETH_LAN2_PORT" "$ETH_LAN3_PORT" "$ETH_LAN4_PORT"
	else # bridging
		# Set global switch parameter
		## echo "Set global switch parameter"
		switch_utility CfgSet 3 1 1536 0 00:00:00:00:00:00
		switch_utility RegisterSet 0xCCD 0x0

		# Mirror Portmap
		## echo "Mirror Portmap"
		switch_utility RegisterSet 0x453 0x0
		# Unknown Unicast/multicast Packet
		## echo "Unknown Unicast/multicast Packet"
		switch_utility RegisterSet 0x454 0x07f
		switch_utility RegisterSet 0x455 0x07f

		tmp_vport=$LTE_VPORT
		# Create DSL WAN Per PVC VLAN IDs
		for argument in $BR_PVC_VID_LIST ; do
			switch_utility VLAN_IdCreate $argument 1
		done
		# Create Common VLAN ID
		switch_utility VLAN_IdCreate $BR_COMMON_VID 1
		# ETH/CPU Port VLAN Configuration
		for argument in $ETH_PORT $CPU_PORT ; do
			switch_utility VLAN_PortCfgSet $argument $BR_COMMON_VID 0 0 3 0 1
		done
		# DSL WAN Port VLAN configuration
		switch_utility VLAN_PortCfgSet $tmp_vport $BR_COMMON_VID 0 0 3 0 0
		# Common VLAN membership
		for argument in $ETH_PORT $CPU_PORT $tmp_vport ; do
			switch_utility VLAN_PortMemberAdd $BR_COMMON_VID $argument 0
		done
		# DSL PCVs VLAN membership
		for PVC_argument in $BR_PVC_VID_LIST ; do
			for argument in $ETH_LAN1_PORT $tmp_vport ; do
				switch_utility VLAN_PortMemberAdd $PVC_argument $argument 0
			done
		done
		# CPU Port VLAN membership
		for argument in $BR_PVC_VID_LIST ; do
			switch_utility VLAN_PortMemberAdd $argument $CPU_PORT 1
		done
		# Traffic flow port 10 is forwarded to CPU directly
		switch_utility PCE_RuleWrite 0 1 1 10 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0 0 00:00:00:00:00:00 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0x0 0x0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0 0 2

		switch_utility MulticastRouterPortAdd $LTE_VPORT
	fi
}

start_ethsw_adsl()
{
	local argument

	echo "[setup_netdev.sh start_ethsw_adsl]" >> $OUT_FILE

	# Set global switch parameter
	## echo "Set global switch parameter"
	switch_utility CfgSet 3 1 1536 0 00:00:00:00:00:00
	switch_utility RegisterSet 0xCCD 0x0

	# Mirror Portmap
	## echo "Mirror Portmap"
	switch_utility RegisterSet 0x453 0x0
	# Unknown Unicast/multicast Packet
	## echo "Unknown Unicast/multicast Packet"
	switch_utility RegisterSet 0x454 0x07f
	switch_utility RegisterSet 0x455 0x07f

	if [ "$ROUTING" == "1" ] ; then # routing/NAT
		# Create switch internal LAN VLAN ID
		switch_utility VLAN_IdCreate $LAN1_VID 1

		if [ "$LAN2_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN2_VID 1
		fi

		if [ "$LAN3_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN3_VID 1
		fi

		if [ "$LAN4_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN4_VID 1
		fi

		# Create switch internal CPU VLAN ID
		switch_utility VLAN_IdCreate $CPU_VID 1
		# Create switch internal WAN VLAN IDs
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "call VLAN_IdCreate for WAN"
			if [ "${WAN_TAGGED_VLAN}" == "1" ] ; then
				WAN_VID=${DFT_WAN_VID}
			fi
			switch_utility VLAN_IdCreate $WAN_VID 2
		fi

		# Create 2nd WAN VLAN IDs for PPPoA/IPoA
		switch_utility VLAN_IdCreate $WAN_PPP_VID 2

		# Set ETH LAN Port VLAN configuration
		## echo "Set ETH LAN Port VLAN configuration"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortCfgSet $argument $LAN1_VID 0 0 3 0 0
		done

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN2_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN3_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN4_VID 0 0 3 0 0
			done
		fi

		# CPU Port VLAN configuration
		## echo "CPU Port VLAN configuration"
		switch_utility VLAN_PortCfgSet $CPU_PORT $CPU_VID 0 0 3 0 0

		# DSL WAN Port VLAN configuration
		## echo "DSL WAN Port VLAN configuration, vdsl, adsl_a, adsl_b"
		###### switch_utility VLAN_PortCfgSet $DSL_VPORT $WAN_VID 0 0 3 0 1

		#
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			switch_utility VLAN_PortCfgSet $DSL_VPORT $WAN_VID 0 0 3 0 1
		fi

		# 2nd DSL WAN Port VLAN configuration
		## echo "2nd DSL WAN Port VLAN configuration, adsl_a, adsl_b"
		switch_utility VLAN_PortCfgSet $PPP_VPORT $WAN_PPP_VID 0 0 3 0 1

		# LAN VLAN membership
		## echo "LAN VLAN membership"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortMemberAdd $LAN1_VID $argument $LAN1_TAGGED_VLAN
		done

		if [ "$ENABLE_VLAN" == "1" ] ; then
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 1
		else
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 0
		fi

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN2_VID $argument $LAN2_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN3_VID $argument $LAN3_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN4_VID $argument $LAN4_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 0
			fi
		fi

		# WAN VLAN membership
		## echo "WAN VLAN membership"
		## echo "vdsl, adsl_a, adsl_b"
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "WAN_TAGGED_VLAN=${WAN_TAGGED_VLAN}, call VLAN_PortMemberAdd for WAN"
			for argument in $DSL_VPORT ; do
				switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
			done

			for argument in $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $WAN_VID $argument 0
			done
		fi

		# 2nd DSL WAN VLAN membership
		## echo "2nd DSL WAN VLAN membership"
		if [ "$ADSL_WAN_ENABLE" == "1" ] ; then
			## echo "adsl_a, adsl_b"
			for argument in $PPP_VPORT $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $WAN_PPP_VID $argument 0
			done
		fi
		# CPU VLAN membership ( Super group )
		## echo "CPU VLAN membership"
		if [ "$VDSL_WAN_ENABLE" == "1" ] || [ "$ADSL_WAN_ENABLE" == "1" ] ; then
			## echo "vdsl, adsl_a, adsl_b"
			for argument in $ETH_LAN1_PORT $DSL_VPORT $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $CPU_VID $argument 0
			done
		fi
        # Add 2nd DSL WAN to Super group
		## echo "Add 2nd DSL WAN to Super group"
		if [ "$ADSL_WAN_ENABLE" == "1" ] ; then
			## echo "adsl_a, adsl_b"
			switch_utility VLAN_PortMemberAdd $CPU_VID $PPP_VPORT 0
		fi
		# CPU Port is default router port map
		## echo "CPU Port is default router port map"
		switch_utility MulticastRouterPortAdd $CPU_PORT

		## ygchen, since we use different FIDs for LAN and WAN groups, it's not necessary to do the function below
		## but please don't remove it (for reference)
	    #check_bridging_vlan $ACTIVE_WAN_TYPE "$ETH_LAN1_PORT" "$ETH_LAN2_PORT" "$ETH_LAN3_PORT" "$ETH_LAN4_PORT"
                local protocol=`ccfg_cli get proto@wan000`
                if [ "$protocol" == "none" ]; then
                        switch_utility VLAN_PortMemberAdd 66 11 0
                        switch_utility VLAN_PortCfgSet 11 66 0 0 3 0 0
                fi
	else # bridging
		# Create DSL WAN Per PVC VLAN IDs
		for argument in $BR_PVC_VID_LIST ; do
			switch_utility VLAN_IdCreate $argument 1
		done
		# Create Common VLAN ID
		switch_utility VLAN_IdCreate $BR_COMMON_VID 1
		# ETH/CPU Port VLAN Configuration
		for argument in $ETH_PORT $CPU_PORT ; do
			switch_utility VLAN_PortCfgSet $argument $BR_COMMON_VID 0 0 3 0 1
		done
		# DSL WAN Port VLAN configuration
		switch_utility VLAN_PortCfgSet $DSL_VPORT $BR_COMMON_VID 0 0 3 0 0
		# Common VLAN membership
		for argument in $ETH_PORT $CPU_PORT $DSL_VPORT ; do
			switch_utility VLAN_PortMemberAdd $BR_COMMON_VID $argument 0
		done
		# DSL PCVs VLAN membership
		for PVC_argument in $BR_PVC_VID_LIST ; do
			for argument in $ETH_LAN1_PORT $DSL_VPORT ; do
				switch_utility VLAN_PortMemberAdd $PVC_argument $argument 0
			done
		done
		# CPU Port VLAN membership
		for argument in $BR_PVC_VID_LIST ; do
			switch_utility VLAN_PortMemberAdd $argument $CPU_PORT 1
		done
		# Traffic flow port 10 is forwarded to CPU directly
		switch_utility PCE_RuleWrite 0 1 1 10 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0 0 00:00:00:00:00:00 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0x0 0x0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0 0 2

		# DSL WAN Port is default router port map
		switch_utility MulticastRouterPortAdd $DSL_VPORT
	fi
}

start_ethsw_vdsl()
{
	echo "[setup_netdev.sh start_ethsw_vdsl]" >> $OUT_FILE

	# Set global switch parameter
	## echo "Set global switch parameter"
	switch_utility CfgSet 3 1 1536 0 00:00:00:00:00:00
	switch_utility RegisterSet 0xCCD 0x0

	# Mirror Portmap
	## echo "Mirror Portmap"
	switch_utility RegisterSet 0x453 0x0
	# Unknown Unicast/multicast Packet
	## echo "Unknown Unicast/multicast Packet"
	switch_utility RegisterSet 0x454 0x07f
	switch_utility RegisterSet 0x455 0x07f

	if [ "$ROUTING" == "1" ] ; then # routing/NAT
		# Create switch internal LAN VLAN ID
		switch_utility VLAN_IdCreate $LAN1_VID $DFT_LAN_FID

		if [ "$LAN2_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN2_VID $DFT_LAN_FID
		fi

		if [ "$LAN3_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN3_VID $DFT_LAN_FID
		fi

		if [ "$LAN4_VID" != "" ] ; then
			switch_utility VLAN_IdCreate $LAN4_VID $DFT_LAN_FID
		fi

		# Create switch internal CPU VLAN ID
		switch_utility VLAN_IdCreate $CPU_VID $DFT_CPU_FID
		# Create switch internal WAN VLAN IDs
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "call VLAN_IdCreate for WAN"
			switch_utility VLAN_IdCreate $WAN_VID $DFT_WAN_FID
		fi

		if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
			echo "call VLAN_IdCreate for VOIP"
			switch_utility VLAN_IdCreate $VOIP_VID $DFT_WAN_FID
		fi

		if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
			echo "call VLAN_IdCreate for IPTV"
			switch_utility VLAN_IdCreate $IPTV_VID $DFT_WAN_FID
		fi

		# Set ETH LAN Port VLAN configuration
		## echo "Set ETH LAN Port VLAN configuration"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortCfgSet $argument $LAN1_VID 0 0 3 0 0
		done

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN2_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN3_VID 0 0 3 0 0
			done
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortCfgSet $argument $LAN4_VID 0 0 3 0 0
			done
		fi

		# CPU Port VLAN configuration
		## echo "CPU Port VLAN configuration"
		switch_utility VLAN_PortCfgSet $CPU_PORT $CPU_VID 0 0 3 0 0

		# DSL WAN Port VLAN configuration
		## echo "DSL WAN Port VLAN configuration, vdsl, adsl_a, adsl_b"
		###### switch_utility VLAN_PortCfgSet $DSL_VPORT $WAN_VID 0 0 3 0 1

		#
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			switch_utility VLAN_PortCfgSet $DSL_VPORT $WAN_VID 0 0 3 0 0
		fi

		if [ "$VDSL_WAN_ENABLE" == "1" ] || [ "$ADSL_WAN_ENABLE" == "1" -a "$WAN_TAGGED_VLAN" == "1" ] ; then
			if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $DSL_VPORT $VOIP_VID 0 0 3 0 0
			fi

			if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				switch_utility VLAN_PortCfgSet $DSL_VPORT $IPTV_VID 0 0 3 0 0
			fi
		fi

		# LAN VLAN membership
		## echo "LAN VLAN membership"
		for argument in $ETH_LAN1_PORT ; do
			switch_utility VLAN_PortMemberAdd $LAN1_VID $argument $LAN1_TAGGED_VLAN
		done

		if [ "$ENABLE_VLAN" == "1" ] ; then
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 1
		else
			switch_utility VLAN_PortMemberAdd $LAN1_VID $CPU_PORT 0
		fi

		if [ "$LAN2_VID" != "" ] ; then
			for argument in $ETH_LAN2_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN2_VID $argument $LAN2_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN2_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN3_VID" != "" ] ; then
			for argument in $ETH_LAN3_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN3_VID $argument $LAN3_TAGGED_VLAN
			done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN3_VID $CPU_PORT 0
			fi
		fi

		if [ "$LAN4_VID" != "" ] ; then
			for argument in $ETH_LAN4_PORT ; do
				switch_utility VLAN_PortMemberAdd $LAN4_VID $argument $LAN4_TAGGED_VLAN
		done

			if [ "$ENABLE_VLAN" == "1" ] ; then
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 1
			else
				switch_utility VLAN_PortMemberAdd $LAN4_VID $CPU_PORT 0
			fi
		fi

		# WAN VLAN membership
		## echo "WAN VLAN membership"
		## echo "vdsl, adsl_a, adsl_b"
		if [ "$WAN_INTERFACE_ENABLE" == "1" ] ; then
			echo "WAN_TAGGED_VLAN=${WAN_TAGGED_VLAN}, call VLAN_PortMemberAdd for WAN"
			for argument in $DSL_VPORT ; do
				switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
			done

			for argument in $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $WAN_VID $argument $WAN_TAGGED_VLAN
			done
		fi

		if [ "$VDSL_WAN_ENABLE" == "1" ] || [ "$ADSL_WAN_ENABLE" == "1" -a "$WAN_TAGGED_VLAN" == "1" ] ; then
			if [ "$VOIP_INTERFACE_ENABLE" == "1" ] ; then
				echo "VOIP_TAGGED_VLAN=${VOIP_TAGGED_VLAN}, call VLAN_PortMemberAdd for VOIP"
				for argument in $DSL_VPORT ; do
					switch_utility VLAN_PortMemberAdd $VOIP_VID $argument $VOIP_TAGGED_VLAN
				done

				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $VOIP_VID $argument $VOIP_TAGGED_VLAN
				done
			fi

			if [ "$IPTV_INTERFACE_ENABLE" == "1" ] ; then
				echo "IPTV_TAGGED_VLAN=${IPTV_TAGGED_VLAN}, call VLAN_PortMemberAdd for IPTV"
				for argument in $DSL_VPORT ; do
					switch_utility VLAN_PortMemberAdd $IPTV_VID $argument $IPTV_TAGGED_VLAN
				done

				for argument in $CPU_PORT ; do
					switch_utility VLAN_PortMemberAdd $IPTV_VID $argument $IPTV_TAGGED_VLAN
				done
			fi
		fi

		# CPU VLAN membership ( Super group )
		## echo "CPU VLAN membership"
		if [ "$VDSL_WAN_ENABLE" == "1" ] || [ "$ADSL_WAN_ENABLE" == "1" ] ; then
			## echo "vdsl, adsl_a, adsl_b"
			for argument in $ETH_LAN1_PORT $DSL_VPORT $CPU_PORT ; do
				switch_utility VLAN_PortMemberAdd $CPU_VID $argument 0
			done
		fi

		# CPU Port is default router port map
		## echo "CPU Port is default router port map"
		switch_utility MulticastRouterPortAdd $CPU_PORT

		## ygchen, since we use different FIDs for LAN and WAN groups, it's not necessary to do the function below
		## but please don't remove it (for reference)
	    #check_bridging_vlan $ACTIVE_WAN_TYPE "$ETH_LAN1_PORT" "$ETH_LAN2_PORT" "$ETH_LAN3_PORT" "$ETH_LAN4_PORT"
		local protocol=`ccfg_cli get proto@wan050`
		if [ "$protocol" == "none" ]; then
			switch_utility VLAN_PortMemberAdd 66 11 0
			switch_utility VLAN_PortCfgSet 11 66 0 0 3 0 0
		fi
	else # bridging
		# DSL WAN Port is default router port map
		switch_utility MulticastRouterPortAdd $DSL_VPORT
	fi
}

start_ethsw()
{
	local tmp_idx=""
	local argument=""
	local argument1=""
	local vlan_id=""
	local vlan_id_val=""
	local vlan_tagged=""
	local vlan_tagged_val=""
	local tmp_max_vlan=""
	local vlan_port=""
	local vlan_port_val=""
	local vlan_idx=""
	local max_vlan=""
	local tmp_lan_port=""

	echo "[setup_netdev.sh start_ethsw]" >> $OUT_FILE

	# set default value to global variables
	start_ethsw_set_variable_to_default_value

	if [ -n "`pgrep midcore`" ] ; then
		umngProg=umngcli
	else
		umngProg=umngcli2
	fi
	echo "umngProg=${umngProg}"

	# WAN_TYPE => 0: auto; 1: adsl; 2: ethernet; 3:vdsl; 5:LTE
	# If WAN_TYPE is not auto-detection mode, we must set this WAN_TYPE value to ACTIVE_WAN_TYPE.

	if [ `/bin/grep -ci bridge /proc/cmdline` -ge 1 ] ; then
		ROUTING=0
		ifconfig eth1 up
		brctl addif br-lan eth1
		ifconfig ptm0 up
		brctl addif br-lan ptm0
	fi

	WAN_TYPE=`ccfg_cli get wan_type@system`
	if [ "$WAN_TYPE" == "0" ] ; then
		# auto-detection
		ACTIVE_WAN_TYPE=`ccfg_cli get active_wan_type@system`
		if [ "$ACTIVE_WAN_TYPE" != "1" ] && [ "$ACTIVE_WAN_TYPE" != "3" ] ; then
			ACTIVE_WAN_TYPE=`ccfg_cli get pre_dsl_wan_type@system`
			if [ "$ACTIVE_WAN_TYPE" != "1" ] && [ "$ACTIVE_WAN_TYPE" != "3" ] ; then
				ACTIVE_WAN_TYPE="1"
			fi
		fi
	else
		ACTIVE_WAN_TYPE=$WAN_TYPE
	fi
	echo "WAN_TYPE=${WAN_TYPE}, ACTIVE_WAN_TYPE=${ACTIVE_WAN_TYPE}"

	ENABLE_VLAN=`ccfg_cli get enable_vlan@vlan`
	if [ "$ACTIVE_WAN_TYPE" == "1" ] ; then
		# adsl wan mode
		ADSL_WAN_ENABLE="1"
		WAN_SECTION_NAME="wan000"
		VOIP_SECTION_NAME="wan001"
		IPTV_SECTION_NAME="wan002"
	elif [ "$ACTIVE_WAN_TYPE" == "2" ]; then
		# ethernet wan mode
		ETH_WAN_ENABLE="1"
		WAN_SECTION_NAME="wan100"
		VOIP_SECTION_NAME="wan101"
		IPTV_SECTION_NAME="wan102"
	elif [ "$ACTIVE_WAN_TYPE" == "3" ]; then
		# vdsl wan mode
		VDSL_WAN_ENABLE="1"
		WAN_SECTION_NAME="wan050"
		VOIP_SECTION_NAME="wan051"
		IPTV_SECTION_NAME="wan052"
	elif [ "$ACTIVE_WAN_TYPE" == "5" ]; then
		# LTE wan mode
		LTE_WAN_ENABLE="1"
		WAN_SECTION_NAME="wan300"
		VOIP_SECTION_NAME="wan301"
		IPTV_SECTION_NAME="wan302"
	fi
	echo "ENABLE_VLAN=${ENABLE_VLAN}, ADSL_WAN_ENABLE=${ADSL_WAN_ENABLE}, ETH_WAN_ENABLE=${ETH_WAN_ENABLE}, VDSL_WAN_ENABLE=${VDSL_WAN_ENABLE}, LTE_WAN_ENABLE=${LTE_WAN_ENABLE}"

	LOGICAL_CPU_PORT=`ccfg_cli get logical_cpu_port@vlan`
	PORT_PHYCONF=`ccfg_cli get port_phyconf@vlan`
	if [ "$WAN_TYPE" == "2" ] || [ "`$umngProg get wan_lan_share@vlan`" != "1" ] ; then
		ETH_WAN_LOGICAL_PORT=`ccfg_cli get wan_port@vlan`
	else
		ETH_WAN_LOGICAL_PORT=""
	fi
	echo "LOGICAL_CPU_PORT=${LOGICAL_CPU_PORT}, PORT_PHYCONF=${PORT_PHYCONF}, ETH_WAN_LOGICAL_PORT=${ETH_WAN_LOGICAL_PORT}"

	# collect global data
	if [ -n "${WAN_SECTION_NAME}" ] ; then
		WAN_INTERFACE_ENABLE=`ccfg_cli get adminstate@${WAN_SECTION_NAME}`
		if [ "$WAN_INTERFACE_ENABLE" == "enable" ] ; then
			WAN_INTERFACE_ENABLE="1"
			WAN_TAGGED_VLAN=`ccfg_cli get vlan_tagged@${WAN_SECTION_NAME}`
			if [ "$WAN_TAGGED_VLAN" == "1" ] ; then
				WAN_VID=`ccfg_cli get vlan_id@${WAN_SECTION_NAME}`
			fi
		else
			WAN_INTERFACE_ENABLE="0"
			WAN_VID=""
		fi
	fi

	if [ -n "${VOIP_SECTION_NAME}" ] ; then
		VOIP_INTERFACE_ENABLE=`ccfg_cli get adminstate@${VOIP_SECTION_NAME}`
		if [ "$VOIP_INTERFACE_ENABLE" == "enable" ] ; then
			VOIP_INTERFACE_ENABLE="1"
			VOIP_TAGGED_VLAN=`ccfg_cli get vlan_tagged@${VOIP_SECTION_NAME}`
			if [ "$VOIP_TAGGED_VLAN" == "1" ] ; then
				VOIP_VID=`ccfg_cli get vlan_id@${VOIP_SECTION_NAME}`
			fi
		else
			VOIP_INTERFACE_ENABLE="0"
			VOIP_VID=""
		fi
	fi

	if [ -n "${IPTV_SECTION_NAME}" ] ; then
		IPTV_INTERFACE_ENABLE=`ccfg_cli get adminstate@${IPTV_SECTION_NAME}`
		if [ "$IPTV_INTERFACE_ENABLE" == "enable" ] ; then
			IPTV_INTERFACE_ENABLE="1"
			IPTV_TAGGED_VLAN=`ccfg_cli get vlan_tagged@${IPTV_SECTION_NAME}`
			if [ "$IPTV_TAGGED_VLAN" == "1" ] ; then
				IPTV_VID=`ccfg_cli get vlan_id@${IPTV_SECTION_NAME}`
			fi
		else
			IPTV_INTERFACE_ENABLE="0"
			IPTV_VID=""
		fi
	fi

	# set CPU_PORT value
	tmp_idx="0"
	for argument in $PORT_PHYCONF ; do
		if [ "$LOGICAL_CPU_PORT" == "$tmp_idx" ] ; then
			CPU_PORT="$argument"
			break
		fi

	     tmp_idx=`expr $tmp_idx + 1`
	done
	echo "CPU_PORT=$CPU_PORT"

	if [ "$ENABLE_VLAN" == "0" ] ; then
		if [ "$ETH_WAN_ENABLE" == "1" ] || [ "$LTE_WAN_ENABLE" == "1" ] ; then
			# if ENABLE_VLAN=0, we should use LAN_PORT and LAN_VLAN_ID to create vlan for lan ports.
			LAN_PORT=`ccfg_cli get lan_port@vlan`
			LAN_VLAN_ID=`ccfg_cli get lan_vlan_id@vlan`
			LAN_VID=${LAN_VLAN_ID}

			echo "ENABLE_VLAN=${ENABLE_VLAN}, LAN_PORT=${LAN_PORT}, LAN_VLAN_ID=${LAN_VLAN_ID}, LAN_VID=${LAN_VID}"

			# set ETH_LAN1_PORT value
			ETH_LAN1_PORT=""
			# find physical lan port
			for argument in $LAN_PORT ; do
				tmp_idx="0"
				for argument1 in $PORT_PHYCONF ; do
					if [ "$argument" == "$tmp_idx" ] ; then
						if [ "$ETH_LAN1_PORT" == "" ] ; then
							ETH_LAN1_PORT="$argument1"
						else
							ETH_LAN1_PORT="${ETH_LAN1_PORT} $argument1"
						fi
					fi
					tmp_idx=`expr $tmp_idx + 1`
				done
			done
			echo "ETH_LAN1_PORT=$ETH_LAN1_PORT"

			# set all vlan id(lan and wan)
			LAN1_VID="$LAN_VLAN_ID"
		fi
	else # ENABLE_VLAN=1
		# set ETH_LAN1_PORT, LAN1_VID and LAN1_TAGGED_VLAN value
		ETH_LAN1_PORT=""
		ETH_LAN2_PORT=""
		ETH_LAN3_PORT=""
		ETH_LAN4_PORT=""

		LAN1_VID=""
		LAN2_VID=""
		LAN3_VID=""
		LAN4_VID=""

		LAN1_TAGGED_VLAN="0"
		LAN2_TAGGED_VLAN="0"
		LAN3_TAGGED_VLAN="0"
		LAN4_TAGGED_VLAN="0"

		# set ETH_PORT value
		ETH_PORT=""

		# get all vlan id
		max_vlan=`ccfg_cli get max_vlan@vlan`

		vlan_idx=1
		tmp_max_vlan=$max_vlan
		tmp_max_vlan=$(($tmp_max_vlan+1))
		while [ "$vlan_idx" != "$tmp_max_vlan" ]
		do
       		vlan_id=vlan${vlan_idx}_id
			vlan_tagged=vlan${vlan_idx}_tagged
			vlan_port=vlan${vlan_idx}_port
			echo "vlan_id=$vlan_id, vlan_tagged=$vlan_tagged, vlan_port=$vlan_port"

			vlan_id_val=`ccfg_cli get ${vlan_id}@vlan`
			vlan_tagged_val=`ccfg_cli get ${vlan_tagged}@vlan`
			vlan_port_val=`ccfg_cli get ${vlan_port}@vlan`
			if [ "$vlan_port_val" != "" ] ; then
				echo "vlan_id_val=$vlan_id_val, vlan_tagged_val=$vlan_tagged_val, vlan_port_val=$vlan_port_val"
			fi

			# find physical lan port
			tmp_lan_port=""
			for argument in $vlan_port_val ; do
				tmp_idx="0"
				for argument1 in $PORT_PHYCONF ; do
					if [ "$argument" == "$tmp_idx" ] ; then
						if [ "$tmp_lan_port" == "" ] ; then
							tmp_lan_port="$argument1"
						else
							tmp_lan_port="${tmp_lan_port} $argument1"
						fi
					fi
					tmp_idx=`expr $tmp_idx + 1`
				done
			done

			if [ "$vlan_idx" == "1" -a "$vlan_port_val" != "" ] ; then
				LAN1_VID=$vlan_id_val
				LAN1_TAGGED_VLAN=$vlan_tagged_val
				ETH_LAN1_PORT=$tmp_lan_port
				echo "LAN1_VID=$LAN1_VID, LAN1_TAGGED_VLAN=$LAN1_TAGGED_VLAN, ETH_LAN1_PORT=$ETH_LAN1_PORT"
				if [ "$ETH_PORT" == "" ] ; then
					ETH_PORT="$ETH_LAN1_PORT"
				else
					ETH_PORT="$ETH_PORT $ETH_LAN1_PORT"
				fi
			fi

			if [ "$vlan_idx" == "2" -a "$vlan_port_val" != "" ] ; then
				LAN2_VID=$vlan_id_val
				LAN2_TAGGED_VLAN=$vlan_tagged_val
				ETH_LAN2_PORT=$tmp_lan_port
				echo "LAN2_VID=$LAN2_VID, LAN2_TAGGED_VLAN=$LAN2_TAGGED_VLAN, ETH_LAN2_PORT=$ETH_LAN2_PORT"
				if [ "$ETH_PORT" == "" ] ; then
					ETH_PORT="$ETH_LAN2_PORT"
				else
					ETH_PORT="$ETH_PORT $ETH_LAN2_PORT"
				fi
			fi

			if [ "$vlan_idx" == "3" -a "$vlan_port_val" != "" ] ; then
				LAN3_VID=$vlan_id_val
				LAN3_TAGGED_VLAN=$vlan_tagged_val
				ETH_LAN3_PORT=$tmp_lan_port
				echo "LAN3_VID=$LAN3_VID, LAN3_TAGGED_VLAN=$LAN3_TAGGED_VLAN, ETH_LAN3_PORT=$ETH_LAN3_PORT"
				if [ "$ETH_PORT" == "" ] ; then
					ETH_PORT="$ETH_LAN3_PORT"
				else
					ETH_PORT="$ETH_PORT $ETH_LAN3_PORT"
				fi
			fi

			if [ "$vlan_idx" == "4" -a "$vlan_port_val" != "" ] ; then
				LAN4_VID=$vlan_id_val
				LAN4_TAGGED_VLAN=$vlan_tagged_val
				ETH_LAN4_PORT=$tmp_lan_port
				echo "LAN4_VID=$LAN4_VID, LAN4_TAGGED_VLAN=$LAN4_TAGGED_VLAN, ETH_LAN4_PORT=$ETH_LAN4_PORT"
				if [ "$ETH_PORT" == "" ] ; then
					ETH_PORT="$ETH_LAN4_PORT"
				else
					ETH_PORT="$ETH_PORT $ETH_LAN4_PORT"
				fi
			fi

			vlan_idx=$(($vlan_idx+1))
		done

		echo "ETH_PORT=$ETH_PORT"
	fi

	# display global config data
	echo "WAN_INTERFACE_ENABLE=${WAN_INTERFACE_ENABLE}, VOIP_INTERFACE_ENABLE=${VOIP_INTERFACE_ENABLE}, IPTV_INTERFACE_ENABLE=${IPTV_INTERFACE_ENABLE}"
	echo "LAN1_VID=$LAN1_VID, LAN2_VID=$LAN2_VID, LAN3_VID=$LAN3_VID, LAN4_VID=$LAN4_VID"
	echo "LAN1_TAGGED_VLAN=$LAN1_TAGGED_VLAN, LAN2_TAGGED_VLAN=$LAN2_TAGGED_VLAN, LAN3_TAGGED_VLAN=$LAN3_TAGGED_VLAN, LAN4_TAGGED_VLAN=$LAN4_TAGGED_VLAN"
	echo "ETH_LAN1_PORT=$ETH_LAN1_PORT, ETH_LAN2_PORT=$ETH_LAN2_PORT, ETH_LAN3_PORT=$ETH_LAN3_PORT, ETH_LAN4_PORT=$ETH_LAN4_PORT"
	echo "WAN_VID=${WAN_VID}, WAN_TAGGED_VLAN=$WAN_TAGGED_VLAN"
	echo "VOIP_VID=${VOIP_VID}, VOIP_TAGGED_VLAN=$VOIP_TAGGED_VLAN"
	echo "IPTV_VID=${IPTV_VID}, IPTV_TAGGED_VLAN=$IPTV_TAGGED_VLAN"

	if [ "$ADSL_WAN_ENABLE" == "1" ] ; then
		start_ethsw_adsl
	fi

	if [ "$VDSL_WAN_ENABLE" == "1" ] ; then
		start_ethsw_vdsl
	fi

	if [ "$ETH_WAN_ENABLE" == "1" ] ; then
		start_ethsw_ethernet
	fi

	if [ "$LTE_WAN_ENABLE" == "1" ] ; then
		start_ethsw_lte
	fi

	switch_utility RegisterSet 0x4a 0x118

	# Fix Internal GPHY FCS error issue, bitonic
	switch_utility MDIO_DataWrite 0x11 0x14 0x8002
	switch_utility MDIO_DataWrite 0x13 0x14 0x8002
	# end of Fix Internal GPHY FCS error issue

	mc_snoop_enable

	vr9_switch_mac_table_flush_patch
}

init_system()
{
	echo "[setup_netdev.sh init_system]" >> $OUT_FILE

	grep "xDSL_MODE_VRX" $RC_CONF
	echo "[setup_netdev.sh init_system] \$?=$?" >> $OUT_FILE

	if [ $? != "0" ] ; then
		MODEL=`sed -n "/CONFIG_IFX_MODEL_NAME/p" $SYS_CONF`
		echo $MODEL | grep GRX288_GW_VRX_BSP
		echo "[setup_netdev.sh init_system] MODEL=$MODEL" >> $OUT_FILE

		if [ $? = "0" ] ; then
			echo "#<< xDSL_MODE_VRX"     >> $RC_CONF
			echo "xDSL_MODE_VRX=eth_wan" >> $RC_CONF
			echo "#>> xDSL_MODE_VRX"     >> $RC_CONF
			echo "#<< ACCELERATION"      >> $RC_CONF
			echo "ACCELERATION=acc_yes"  >> $RC_CONF
			echo "#>> ACCELERATION"      >> $RC_CONF
			echo " "
			echo "No mode was set , set ethernet WAN mode as default"
		else
			echo "#<< xDSL_MODE_VRX"     >> $RC_CONF
			echo "xDSL_MODE_VRX=vdsl"    >> $RC_CONF
			echo "#>> xDSL_MODE_VRX"     >> $RC_CONF
			echo "#<< ACCELERATION"      >> $RC_CONF
			ACC_STATE=`sed -n "/CONFIG_IFX_MODEL_NAME/p" $SYS_CONF`
			echo $MODEL | grep NOPPA
			if [ $? = "0" ] ; then
				echo "ACCELERATION=acc_no"  >> $RC_CONF
			else
				echo "ACCELERATION=acc_yes" >> $RC_CONF
			fi
			echo "#>> ACCELERATION"      >> $RC_CONF
			echo " "
			echo "No mode was set , set VDSL mode as default"
		fi
		ccfg_cli commitcfg
	fi

	echo 1    > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
	echo 3000 > /proc/sys/vm/min_free_kbytes     # -- OOM issue
	echo 10   > /proc/sys/net/core/netdev_budget # -- bad voice quality

	### bitonic(20121130)
	### When I check the DSL auto-sensing function of UGW 5.1.1, I find that Lantiq does the following setting.
	### copy from Lantiq load_ppa_modules.sh
	# changed min_free_kbytes to have more free memory a given point of execution
	#### echo 1024 > /proc/sys/vm/min_free_kbytes => We use 3000 to fix OOM issue now
	echo 4096 > /proc/sys/net/ipv4/route/max_size

	# memory tunning for all platform lowmem_reserve_ratio is a MUST
	echo 250 >  /proc/sys/vm/lowmem_reserve_ratio
	echo 2 > /proc/sys/vm/dirty_background_ratio
	echo 250 > /proc/sys/vm/dirty_writeback_centisecs
	echo 10 > /proc/sys/vm/dirty_ratio
	echo 16384 > /proc/sys/vm/max_map_count
	echo 2 > /proc/sys/vm/page-cluster
	echo 70 > /proc/sys/vm/swappiness
	### end of copy from Lantiq load_ppa_modules.sh

	echo "[setup_netdev.sh init_system] xDSL_MODE_VRX=$xDSL_MODE_VRX" >> $OUT_FILE
	if [ "$xDSL_MODE_VRX" == "vdsl" ] ; then

		echo "[setup_netdev.sh init_system] Setting in flash is VDSL mode" >> $OUT_FILE
		
		echo "%% disable annex J: value should be 0x8f" > /dev/console
		echo mask_set 0x00 0x20 > /proc/driver/sipo #disable annex J relay

		if [ "$ACCELERATION" != "acc_yes" ] ; then
			if [ -e /lib/modules/${KERNEL_VER}/ifxmips_ptm.ko ] ; then
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ptm.ko
				usleep 100000
			else
				ACCELERATION="acc_yes"
				echo "Try to change to PPA mode"
			fi
		fi
		if [ "$ACCELERATION" == "acc_yes" ] ; then
			echo "[setup_netdev.sh init_system] Enter PPA mode" >> $OUT_FILE
			#disable_vrx_switch_ports
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_e5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko
			fi

			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_e5.ko
			insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
			insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko
			#enable_vrx_switch_ports
			usleep 100000
		fi

		init_netdev ptm0
		set_eth1_mac

#       ## has been set in ifx_cpe_control_init.sh and vdsl.scr
#		if [ -s /opt/lantiq/bin/dsl_cpe_pipe.sh ] ; then
#			/opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 0 0 0 0 0 0 0 7
#			/opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
#		fi

	elif [ "$xDSL_MODE_VRX" == "adsl_a" ] ; then

		echo "[setup_netdev.sh init_system] Setting in flash is ADSL Annex A mode" >> $OUT_FILE

		if [ "$ACCELERATION" != "acc_yes" ] ; then
			if [ -e /lib/modules/${KERNEL_VER}/ifxmips_atm.ko ] ; then
				insmod /lib/modules/${KERNEL_VER}/ifxmips_atm.ko
				usleep 100000
			else
				ACCELERATION="acc_yes"
				echo "Try to change to PPA mode"
			fi
		fi
		if [ "$ACCELERATION" == "acc_yes" ] ; then
			echo "[setup_netdev.sh init_system] Enter PPA mode" >> $OUT_FILE
			#disable_vrx_switch_ports
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_a5.ko

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_a5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_a5.ko
			fi

			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_a5.ko
			insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
			insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko
			#enable_vrx_switch_ports
			usleep 100000
		fi

		init_netdev nas0
		set_eth1_mac

       ## has been set in ifx_cpe_control_init.sh and adsl.scr
#		if [ -s /opt/lantiq/bin/dsl_cpe_pipe.sh ] ; then
#			/opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 5 0 4 0 C 1 0 0 # MUST check rc.conf::ADSL_MODE
#			/opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
#		fi

		/usr/bin/oamd &

	elif [ "$xDSL_MODE_VRX" == "adsl_b" ] ; then

		echo "[setup_netdev.sh init_system] Setting in flash is ADSL Annex B mode" > /dev/console

		if [ "$ACCELERATION" != "acc_yes" ] ; then
			if [ -e /lib/modules/${KERNEL_VER}/ifxmips_atm.ko ] ; then
				insmod /lib/modules/${KERNEL_VER}/ifxmips_atm.ko
				usleep 100000
			else
				ACCELERATION="acc_yes"
				echo "Try to change to PPA mode"
			fi
		fi
		if [ "$ACCELERATION" == "acc_yes" ] ; then
			echo "[setup_netdev.sh init_system] Enter PPA mode" >> $OUT_FILE
			#disable_vrx_switch_ports
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_a5.ko

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_a5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_a5.ko
			fi

			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_a5.ko
			insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
			insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko
			#enable_vrx_switch_ports
			usleep 100000
		fi

		init_netdev nas0
		set_eth1_mac

#       ## has been set in ifx_cpe_control_init.sh and adsl.scr
#		if [ -s /opt/lantiq/bin/dsl_cpe_pipe.sh ] ; then
#			/opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 10 0 10 0 0 4 0 0 # MUST check rc.conf::ADSL_MODE
#			/opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
#		fi

		/usr/bin/oamd &

	elif [ "$xDSL_MODE_VRX" == "eth_wan" ] ; then

		echo "[setup_netdev.sh init_system] Setting in flash is Ethernet WAN mode" >> $OUT_FILE

		#disable_vrx_switch_ports
		if [ -f /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko ] ; then
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_d5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko
			fi
		else
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko ethwan=2

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_e5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko ethwan=2
			fi
		fi

		if [ -f /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_d5.ko ] ; then
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_d5.ko
		else
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_e5.ko
		fi
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko
		#enable_vrx_switch_ports

		usleep 100000

		init_netdev eth1

	elif [ "$xDSL_MODE_VRX" == "lte" ] ; then

		echo "[setup_netdev.sh init_system] Setting in flash is LTE mode" >> $OUT_FILE

		#disable_vrx_switch_ports
		if [ -f /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko ] ; then
			#insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko
			echo "3:ADLEY:PPA:: datapath_vr9_d5.ko ethwan=2 wanitf=8" > /dev/console
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko ethwan=2 wanitf=8

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_d5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko ethwan=2 wanitf=8
			fi
		else
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko ethwan=2

			# We use remove module, insert moudle to workaround IAD can not receive packets during auto-sensing.
			if [ "$AUTO_SENSING_WORKAROUNG" == "1" ] ; then
				sleep 1
				rmmod ifxmips_ppa_datapath_vr9_e5
				sleep 1
				insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko ethwan=2
			fi
		fi

		if [ -f /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_d5.ko ] ; then
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_d5.ko
		else
			insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_e5.ko
		fi
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko
		#enable_vrx_switch_ports

		usleep 100000

		init_netdev lte0

	else

		echo "[setup_netdev.sh init_system] No mode was found" > /dev/console
		exit 1

	fi

	# ++ 20120604 Titan Liu -- Workaround for CPU v1.2
	# echo smi-write 0x1307 0x8 > /proc/driver/rtl8367
	switch_utility RegisterSet 0xc77 0xc003
	# -- 20120604 Titan Liu
}


init_ppadev()
{
	echo "[setup_netdev.sh init_ppadev] xDSL_MODE_VRX=$xDSL_MODE_VRX" >> $OUT_FILE

	if [ "$xDSL_MODE_VRX" == "vdsl" ] ; then
		init_netdev ptm0
	elif [ "$xDSL_MODE_VRX" == "adsl_a" ] || [ "$xDSL_MODE_VRX" == "adsl_b" ] ; then
		init_netdev nas0
	elif [ "$xDSL_MODE_VRX" == "eth_wan" ] ; then
		init_netdev eth1
	elif [ "$xDSL_MODE_VRX" == "lte" ] ; then
		init_netdev lte0
	else
		echo "No mode was found"
		exit 1
	fi
}

# enable/disable switch ports and phys, not include WAN Port
# $1: 0-disable, 1-enable
admin_sw_ports()
{
	local	CPU_PORT="6"
	local	PORTS="0 1 2 3 4 5 ${CPU_PORT} 7 8 9 10 11"
	# LAN_PORTS(LTE)="0 1 2 3 4 5"
	# LAN_PORTS(non-LTE)="0 1 2 3 4 5 7 8 9"
	local	LAN_PORTS="0 1 2 3 4 5 7 8 9"
	local	PORT=""
	local	ETH_WAN_PORT_SEQ=""
	local	ETH_WAN_PORT=""
	local	wan_type=""
	local	active_wan_type=""

	echo "[setup_netdev.sh admin_sw_ports] \$1=$1" >> $OUT_FILE

	wan_type=`ccfg_cli get wan_type@system`
	if [ "$wan_type" == "0" ] ; then
		# auto-detection
		active_wan_type=`ccfg_cli get active_wan_type@system`
		if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
			active_wan_type=`ccfg_cli get pre_dsl_wan_type@system`
			if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
				active_wan_type="1"
			fi
		fi
	else
		active_wan_type=$wan_type
	fi

	if [ "$active_wan_type" == "5" ] ; then
		# LTE mode
		LAN_PORTS="0 1 2 3 4 5"
	fi

	if [ "$1" == "0" ] ; then
		# power down
		if [ -f /usr/bin/rtl8367_switch_utility ] ; then
			rtl8367_switch_utility PortPowerOnOff 0 0
			rtl8367_switch_utility PortPowerOnOff 1 0
			rtl8367_switch_utility PortPowerOnOff 2 0
			rtl8367_switch_utility PortPowerOnOff 3 0
		fi

		for PORT in $LAN_PORTS ; do
			if [ "$PORT" -gt "6" ] ; then
				continue
			fi
			MDIO_PHY=`switch_utility PHY_AddrGet $PORT | cut -d '=' -f 2`
			VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
			VAL=`printf %d $VAL`
			let PD=$VAL/2048%2
			if [ "$PD" != "1" ] ; then
				let VAL=$VAL+2048
				VAL=`printf 0x%04x $VAL`
				switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
				echo "[setup_netdev.sh admin_sw_ports] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
			fi
		done

		# disable
		for PORT in $PORTS ; do
			if [ "$PORT" == "$CPU_PORT" ] ; then
				switch_utility PortCfgSet $PORT 0 0 0 0 0 1 0 255 0 4
				echo "[setup_netdev.sh admin_sw_ports] switch_utility PortCfgSet $PORT 0 0 0 0 0 1 0 255 0 4" >> $OUT_FILE
			else
				switch_utility PortCfgSet $PORT 0 0 0 0 0 1 0 255 0 0
				echo "[setup_netdev.sh admin_sw_ports] switch_utility PortCfgSet $PORT 0 0 0 0 0 1 0 255 0 0" >> $OUT_FILE
			fi
		done
	else
		if [ -n "`pgrep midcore`" ] ; then
			umngProg=umngcli
		else
			umngProg=umngcli2
		fi
		if [ "`/bin/grep -ci bridge /proc/cmdline`" -le 0 ] &&
		   [ "`$umngProg get wan_lan_share@vlan`" != "1" ] &&
		   [ "`$umngProg get wan_type@system`" != "2" ] ; then
			ETH_WAN_PORT_SEQ=`$umngProg get wan_port@vlan`
			let ETH_WAN_PORT_SEQ=$ETH_WAN_PORT_SEQ+1
			ETH_WAN_PORT=`$umngProg get port_phyconf@vlan | cut -d ' ' -f $ETH_WAN_PORT_SEQ`
		fi

		# enable
		for PORT in $PORTS ; do
			if [ "$PORT" == "$CPU_PORT" ] ; then
				switch_utility PortCfgSet $PORT 1 0 0 0 0 1 0 255 0 4
				echo "switch_utility PortCfgSet $PORT 1 0 0 0 0 1 0 255 0 4" >> $OUT_FILE
			elif [ "$PORT" != "$ETH_WAN_PORT" ] ; then
				switch_utility PortCfgSet $PORT 1 0 0 0 0 1 0 255 0 0
				echo "[setup_netdev.sh admin_sw_ports] switch_utility PortCfgSet $PORT 1 0 0 0 0 1 0 255 0 0" >> $OUT_FILE
			fi

		done

		# power up
		for PORT in $LAN_PORTS ; do
			if [ "$PORT" -gt "6" ] ; then
				continue
			fi
			MDIO_PHY=`switch_utility PHY_AddrGet $PORT | cut -d '=' -f 2`
			VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
			VAL=`printf %d $VAL`
			let PD=$VAL/2048%2
			if [ "$PD" == "1" ] ; then
				let VAL=$VAL-2048
				VAL=`printf 0x%04x $VAL`
				switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
				echo "[setup_netdev.sh admin_sw_ports] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
			fi
		done

		if [ -f /usr/bin/rtl8367_switch_utility ] ; then
			rtl8367_switch_utility PortPowerOnOff 0 1
			rtl8367_switch_utility PortPowerOnOff 1 1
			rtl8367_switch_utility PortPowerOnOff 2 1
			rtl8367_switch_utility PortPowerOnOff 3 1
		fi
	fi
}

# only enable/disable switch lan phys
# $1: 0-disable, 1-enable
admin_sw_lan_phys()
{
	local	CPU_PORT="6"
	local	PORTS="0 1 2 3 4 5 ${CPU_PORT} 7 8 9 10 11"
	# LAN_PORTS(LTE)="0 1 2 3 4 5"
	# LAN_PORTS(non-LTE)="0 1 2 3 4 5 7 8 9"
	local	LAN_PORTS="0 1 2 3 4 5 7 8 9"
	local	PORT=""
	local	ETH_WAN_PORT_SEQ=""
	local	ETH_WAN_PORT=""
	local	wan_type=""
	local	active_wan_type=""

	echo "[setup_netdev.sh admin_sw_lan_phys] \$1=$1" >> $OUT_FILE

	if [ -n "`pgrep midcore`" ] ; then
		umngProg=umngcli
	else
		umngProg=umngcli2
	fi

	wan_type=`ccfg_cli get wan_type@system`
	if [ "$wan_type" == "0" ] ; then
		# auto-detection
		active_wan_type=`ccfg_cli get active_wan_type@system`
		if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
			active_wan_type=`ccfg_cli get pre_dsl_wan_type@system`
			if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
				active_wan_type="1"
			fi
		fi
	else
		active_wan_type=$wan_type
	fi

	if [ "$active_wan_type" == "5" ] ; then
		# LTE mode
		LAN_PORTS="0 1 2 3 4 5"
	fi

	if [ "$1" == "0" ] ; then
		# power down
		echo "[setup_netdev.sh admin_sw_lan_phys] power down" >> $OUT_FILE
		if [ -f /usr/bin/rtl8367_switch_utility ] ; then
			rtl8367_switch_utility PortPowerOnOff 0 0
			rtl8367_switch_utility PortPowerOnOff 1 0
			rtl8367_switch_utility PortPowerOnOff 2 0
			rtl8367_switch_utility PortPowerOnOff 3 0
		fi

		for PORT in $LAN_PORTS ; do
			if [ "$PORT" -gt "6" -o "$PORT" == "$CONFIG_ETH_WAN_PORT" ] ; then
				continue
			fi

			MDIO_PHY=`switch_utility PHY_AddrGet $PORT | cut -d '=' -f 2`
			VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
			VAL=`printf %d $VAL`
			let PD=$VAL/2048%2
			if [ "$PD" != "1" ] ; then
				let VAL=$VAL+2048
				VAL=`printf 0x%04x $VAL`
				switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
				echo "[setup_netdev.sh admin_sw_lan_phys] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
			fi
		done
	else
		# power up
		echo "[setup_netdev.sh admin_sw_lan_phys] power up" >> $OUT_FILE
		for PORT in $LAN_PORTS ; do
			if [ "$PORT" -gt "6" -o "$PORT" == "$CONFIG_ETH_WAN_PORT" ] ; then
				continue
			fi
			MDIO_PHY=`switch_utility PHY_AddrGet $PORT | cut -d '=' -f 2`
			VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
			VAL=`printf %d $VAL`
			let PD=$VAL/2048%2
			if [ "$PD" == "1" ] ; then
				let VAL=$VAL-2048
				VAL=`printf 0x%04x $VAL`
				switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
				echo "[setup_netdev.sh admin_sw_lan_phys] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
			fi
		done

		if [ -f /usr/bin/rtl8367_switch_utility ] ; then
			rtl8367_switch_utility PortPowerOnOff 0 1
			rtl8367_switch_utility PortPowerOnOff 1 1
			rtl8367_switch_utility PortPowerOnOff 2 1
			rtl8367_switch_utility PortPowerOnOff 3 1
		fi
	fi
}

admin_sw_wan_phy()
{
	if [ "$CONFIG_ETH_WAN_PORT" == "" ] ; then
		return
	fi

	echo "[setup_netdev.sh admin_sw_wan_phy] \$1=$1" >> $OUT_FILE

	if [ "$1" == "0" ] ; then
		# power down
		echo "[setup_netdev.sh admin_sw_wan_phy] power down" >> $OUT_FILE

		MDIO_PHY=`switch_utility PHY_AddrGet $CONFIG_ETH_WAN_PORT | cut -d '=' -f 2`
		VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
		VAL=`printf %d $VAL`
		let PD=$VAL/2048%2
		if [ "$PD" != "1" ] ; then
			let VAL=$VAL+2048
			VAL=`printf 0x%04x $VAL`
			switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
			echo "[setup_netdev.sh admin_sw_wan_phy] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
		fi
	else
		# power up
		echo "[setup_netdev.sh admin_sw_wan_phy] power up" >> $OUT_FILE

		MDIO_PHY=`switch_utility PHY_AddrGet $CONFIG_ETH_WAN_PORT | cut -d '=' -f 2`
		VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
		VAL=`printf %d $VAL`
		let PD=$VAL/2048%2
		if [ "$PD" == "1" ] ; then
			let VAL=$VAL-2048
			VAL=`printf 0x%04x $VAL`
			switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
			echo "[setup_netdev.sh admin_sw_wan_phy] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
		fi
	fi
}

admin_sw_phys()
{
	local	CPU_PORT="6"
	local	PORTS="0 1 2 3 4 5 ${CPU_PORT} 7 8 9 10 11"
	# LAN_PORTS(LTE)="0 1 2 3 4 5"
	# LAN_PORTS(non-LTE)="0 1 2 3 4 5 7 8 9"
	local	LAN_PORTS="0 1 2 3 4 5 7 8 9"
	local	PORT=""
	local	ETH_WAN_PORT_SEQ=""
	local	ETH_WAN_PORT=""
	local	wan_type=""
	local	active_wan_type=""

	echo "[setup_netdev.sh admin_sw_phys] \$1=$1" >> $OUT_FILE

	if [ -n "`pgrep midcore`" ] ; then
		umngProg=umngcli
	else
		umngProg=umngcli2
	fi

	wan_type=`ccfg_cli get wan_type@system`
	if [ "$wan_type" == "0" ] ; then
		# auto-detection
		active_wan_type=`ccfg_cli get active_wan_type@system`
		if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
			active_wan_type=`ccfg_cli get pre_dsl_wan_type@system`
			if [ "$active_wan_type" != "1" ] && [ "$active_wan_type" != "3" ] ; then
				active_wan_type="1"
			fi
		fi
	else
		active_wan_type=$wan_type
	fi

	if [ "$active_wan_type" == "5" ] ; then
		# LTE mode
		LAN_PORTS="0 1 2 3 4 5"
	fi

	if [ "$1" == "0" ] ; then
		# power down
		echo "[setup_netdev.sh admin_sw_phys] power down" >> $OUT_FILE
		if [ -f /usr/bin/rtl8367_switch_utility ] ; then
			rtl8367_switch_utility PortPowerOnOff 0 0
			rtl8367_switch_utility PortPowerOnOff 1 0
			rtl8367_switch_utility PortPowerOnOff 2 0
			rtl8367_switch_utility PortPowerOnOff 3 0
		fi

		for PORT in $LAN_PORTS ; do
			if [ "$PORT" -gt "6" ] ; then
				continue
			fi

			MDIO_PHY=`switch_utility PHY_AddrGet $PORT | cut -d '=' -f 2`
			VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
			VAL=`printf %d $VAL`
			let PD=$VAL/2048%2
			if [ "$PD" != "1" ] ; then
				let VAL=$VAL+2048
				VAL=`printf 0x%04x $VAL`
				switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
				echo "[setup_netdev.sh admin_sw_lan_phys] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
			fi
		done
	else
		# power up
		echo "[setup_netdev.sh admin_sw_lan_phys] power up" >> $OUT_FILE
		for PORT in $LAN_PORTS ; do
			if [ "$PORT" -gt "6" ] ; then
				continue
			fi
			MDIO_PHY=`switch_utility PHY_AddrGet $PORT | cut -d '=' -f 2`
			VAL=`switch_utility MDIO_DataRead $MDIO_PHY 0 | grep "Data = " | cut -d '=' -f 2`
			VAL=`printf %d $VAL`
			let PD=$VAL/2048%2
			if [ "$PD" == "1" ] ; then
				let VAL=$VAL-2048
				VAL=`printf 0x%04x $VAL`
				switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL
				echo "[setup_netdev.sh admin_sw_lan_phys] switch_utility MDIO_DataWrite $MDIO_PHY 0 $VAL" >> $OUT_FILE
			fi
		done

		if [ -f /usr/bin/rtl8367_switch_utility ] ; then
			rtl8367_switch_utility PortPowerOnOff 0 1
			rtl8367_switch_utility PortPowerOnOff 1 1
			rtl8367_switch_utility PortPowerOnOff 2 1
			rtl8367_switch_utility PortPowerOnOff 3 1
		fi
	fi
}

admin_sw_wan_mac()
{
	if [ "$CONFIG_ETH_WAN_PORT" == "" ] ; then
		return
	fi

	echo "[setup_netdev.sh admin_sw_wan_mac] \$1=$1" >> $OUT_FILE

	if [ "$1" == "0" ] ; then
		# power down
		echo "[setup_netdev.sh admin_sw_wan_mac] power down" >> $OUT_FILE

		switch_utility PortCfgSet $CONFIG_ETH_WAN_PORT 0 0 0 0 0 1 0 255 0 0
		echo "[setup_netdev.sh admin_sw_wan_mac] switch_utility PortCfgSet $CONFIG_ETH_WAN_PORT 1 0 0 0 0 1 0 255 0 0" >> $OUT_FILE
	else
		# power up
		echo "[setup_netdev.sh admin_sw_wan_mac] power up" >> $OUT_FILE

		switch_utility PortCfgSet $CONFIG_ETH_WAN_PORT 1 0 0 0 0 1 0 255 0 0
		echo "[setup_netdev.sh admin_sw_wan_mac] switch_utility PortCfgSet $CONFIG_ETH_WAN_PORT 1 0 0 0 0 1 0 255 0 0" >> $OUT_FILE
	fi
}

# only disable switch port, not to disable PHY
disable_vrx_switch_ports() {
	echo "[setup_netdev.sh disable_vrx_switch_ports]" > /dev/console
	SWITCH_PORT="0 1 2 3 4 5 7 8 9 10 11"
	for argument in $SWITCH_PORT ;do
		#switch_utility PortCfgSet $argument 0 0 0 0 0 0 1 255 0 0
		switch_utility PortCfgSet $argument 0 0 0 0 0 1 0 255 0 0
		echo "[disable_vrx_switch_ports] switch_utility PortCfgSet $argument 0 0 0 0 0 1 0 255 0 0" > $OUT_FILE
	done

	SWITCH_PORT="6"
	for argument in $SWITCH_PORT ;do
		#switch_utility PortCfgSet $argument 0 0 0 0 0 0 0 255 0 0
		switch_utility PortCfgSet $argument 0 0 0 0 0 1 0 255 0 4
		echo "[disable_vrx_switch_ports] switch_utility PortCfgSet $argument 0 0 0 0 0 1 0 255 0 4" > $OUT_FILE
	done
}

# only enable switch port, not to enable PHY
enable_vrx_switch_ports() {
	echo "[setup_netdev.sh enable_vrx_switch_ports]" > /dev/console
	SWITCH_PORT="6"
	for argument in $SWITCH_PORT ;do
		#switch_utility PortCfgSet $argument 1 0 0 0 0 0 0 255 0 0
		switch_utility PortCfgSet $argument 1 0 0 0 0 1 0 255 0 4
		echo "[enable_vrx_switch_ports] switch_utility PortCfgSet $argument 1 0 0 0 0 1 0 255 0 4" > $OUT_FILE
	done

	SWITCH_PORT="0 1 2 3 4 5 7 8 9 10 11"
	for argument in $SWITCH_PORT ;do
		#switch_utility PortCfgSet $argument 1 0 0 0 0 0 0 255 0 0
		switch_utility PortCfgSet $argument 1 0 0 0 0 1 0 255 0 0
		echo "[enable_vrx_switch_ports] switch_utility PortCfgSet $argument 1 0 0 0 0 1 0 255 0 0" > $OUT_FILE
	done
}

## for the WMM issue, we need to configure remarking at the egress port connecting to WiFi
lan_to_wlan_1p_remarking() {
	local wlan_port_in_port_phyconf
	local wlan_port

	wlan_port_in_port_phyconf=`ccfg_cli get wlan_port@vlan`
	let wlan_port_in_port_phyconf=$wlan_port_in_port_phyconf+1
	wlan_port=`ccfg_cli get port_phyconf@vlan | cut -d ' ' -f $wlan_port_in_port_phyconf`

    ### set LAN port (#0-3) ingress tc mapping method
    switch_utility QOS_PortCfgSet 0 3 0 #based on DSCP-PCP, dft tc=0
    switch_utility QOS_PortCfgSet 1 3 0 #based on DSCP-PCP, dft tc=0
    switch_utility QOS_PortCfgSet 2 3 0 #based on DSCP-PCP, dft tc=0
    switch_utility QOS_PortCfgSet 3 3 0 #based on DSCP-PCP, dft tc=0
    ### set CPU port (#6) ingress tc mapping method
    switch_utility QOS_PortCfgSet 6 3 0 #based on DSCP-PCP, dft tc=0
    ### set WLAN port ingress tc mapping method
    switch_utility QOS_PortCfgSet $wlan_port 3 0 #based on DSCP-PCP, dft tc=0
    
    ### set ingress dscp-to-tc mapping (global)
    ### switch_utility QOS_DscpClassSet <Index 0-63> <nTrafficClass data 0-15>
    dscp_idx=0
    tc_idx=0
    while [ "$dscp_idx" -le 64 ]; do
    switch_utility QOS_DscpClassSet $dscp_idx $tc_idx
    dscp_idx=$(($dscp_idx+1))
    tc_idx=$(($dscp_idx/8))
    done
    
    ### set ingress pcp-to-tc mapping (global)
    ### switch_utility QOS_PcpClassSet <Index> <nTrafficClass data>
    pcp_idx=0
    tc_idx=0
    while [ "$pcp_idx" -le 8 ]; do
    switch_utility QOS_PcpClassSet $pcp_idx $tc_idx
    pcp_idx=$(($pcp_idx+1))
    tc_idx=$pcp_idx
    done
    
    ### set egress tc-to-pcp mapping (global)
    ### switch_utility QOS_ClassPCPSet <Index> <nTrafficClass data>
    tc_idx=0
    pcp_idx=0
    while [ "$pcp_idx" -le 8 ]; do
    switch_utility QOS_ClassPCPSet $tc_idx $pcp_idx
    tc_idx=$(($tc_idx+1))
    pcp_idx=$(($pcp_idx+1))
    done
    
    ### set LAN port (#0-3) ingress remarking configuration
    ### switch_utility QOS_PortRemarkingCfgSet <nPortId> <eDSCP_IngressRemarkingEnable> <bDSCP_EgressRemarkingEnable> <bPCP_IngressRemarkingEnable> <bPCP_EgressRemarkingEnable>
    ###     nPortId:               0..6
    ###     eDSCP_IngressRemarkingEnable:            0: Disable 1:TC6 2:TC3 3:DP3 4:DP3_TC3
    ###     bDSCP_EgressRemarkingEnable:             0: Disable 1:Enable
    ###     bPCP_IngressRemarkingEnable:             0: Disable 1:Enable
    ###     bPCP_EgressRemarkingEnable:              0: Disable 1:Enable
    switch_utility QOS_PortRemarkingCfgSet 0 0 0 1 1
    switch_utility QOS_PortRemarkingCfgSet 1 0 0 1 1
    switch_utility QOS_PortRemarkingCfgSet 2 0 0 1 1
    switch_utility QOS_PortRemarkingCfgSet 3 0 0 1 1
#    ### set CPU port (#6) ingress remarking configuration
#    switch_utility QOS_PortRemarkingCfgSet 6 0 0 1 1   ##marked because this will affect WAN egress 802.1p remarking
#    ### set WLAN port egress remarking configuration
    switch_utility QOS_PortRemarkingCfgSet $wlan_port 0 0 1 1
}

# . $RC_CONF

case "`ccfg_cli get wan_type@system`" in
	"1") # ADSL
		if [ `grep FW_XDSL_ $SYS_CONF | grep -c ANNEX_B` -ge 1 ] ; then
			xDSL_MODE_VRX="adsl_b"
		else
			xDSL_MODE_VRX="adsl_a"
		fi
		;;
	"2") # ETH
		xDSL_MODE_VRX="eth_wan"
		;;
	"3") # VDSL
		xDSL_MODE_VRX="vdsl"
		;;
	"5") # LTE
		xDSL_MODE_VRX="lte"
		;;
	"0") # auto-DSL
		xDSL_MODE_VRX="vdsl"
		case "`ccfg_cli get active_wan_type@system`" in
			"1") #ADSL
				if [ `grep FW_XDSL_ $SYS_CONF | grep -c ANNEX_B` -ge 1 ] ; then
					xDSL_MODE_VRX="adsl_b"
				else
					xDSL_MODE_VRX="adsl_a"
				fi
				;;
			"3") # VDSL
				xDSL_MODE_VRX="vdsl"
				;;
#			"2") # ETH
#				xDSL_MODE_VRX="eth_wan"
#				;;
			*) # others
				case "`ccfg_cli get pre_dsl_wan_type@system`" in
					"3") # VDSL
						xDSL_MODE_VRX="vdsl"
						;;
#					"1") #ADSL
					*) #ADSL
						if [ `grep FW_XDSL_ $SYS_CONF | grep -c ANNEX_B` -ge 1 ] ; then
							xDSL_MODE_VRX="adsl_b"
						else
							xDSL_MODE_VRX="adsl_a"
						fi
						;;
				esac
				;;
		esac
		;;
esac

# get Ether-Uplink WAN Port and CPU Port
CPU_PORT_SEQ=""
ETH_WAN_PORT_SEQ=""
ETH_WAN_PORT=""
CONFIG_ETH_WAN_PORT=""
if [ -n "`pgrep midcore`" ] ; then
	umngProg=umngcli
else
	umngProg=umngcli2
fi
if [ "`/bin/grep -ci bridge /proc/cmdline`" -le 0 ] &&
   [ "`$umngProg get wan_lan_share@vlan`" != "1" ] ; then
	ETH_WAN_PORT_SEQ=`$umngProg get wan_port@vlan`
	let ETH_WAN_PORT_SEQ=$ETH_WAN_PORT_SEQ+1
	CONFIG_ETH_WAN_PORT=`$umngProg get port_phyconf@vlan | cut -d ' ' -f $ETH_WAN_PORT_SEQ`
	ETH_WAN_PORT=$CONFIG_ETH_WAN_PORT
fi

CPU_PORT_SEQ=`$umngProg get logical_cpu_port@vlan`
let CPU_PORT_SEQ=$CPU_PORT_SEQ+1
CPU_PORT=`$umngProg get port_phyconf@vlan | cut -d ' ' -f $CPU_PORT_SEQ`

echo "[setup_netdev.sh] ETH_WAN_PORT=$ETH_WAN_PORT, CONFIG_ETH_WAN_PORT=$CONFIG_ETH_WAN_PORT, CPU_PORT=$CPU_PORT" > /dev/console

case "$1" in
	""|"init")
		echo "[setup_netdev.sh] init_system and start_ethsw" > /dev/console
		admin_sw_ports 0
		init_system
		start_ethsw
		admin_sw_ports 1
		;;
	"system")
		echo "[setup_netdev.sh] init_system" > /dev/console
		init_system
		;;
	"ppadev")
		echo "[setup_netdev.sh] init_ppadev" > /dev/console
		init_ppadev
		;;
	"switch")
		echo "[setup_netdev.sh] start_ethsw" > /dev/console
		start_ethsw
		lan_to_wlan_1p_remarking
		;;
	"port-enable")
		echo "[setup_netdev.sh] port-enable" > /dev/console
		admin_sw_ports 1
		;;
	"port-disable")
		echo "[setup_netdev.sh] port-disable" > /dev/console
		admin_sw_ports 0
		;;
	"mc-snoop-enable")
		echo "[setup_netdev.sh] mc-snoop-enable" > /dev/console
		mc_snoop_enable
		;;
	"mc-snoop-disable")
		echo "[setup_netdev.sh] mc-snoop-disable" > /dev/console
		mc_snoop_disable
		;;
	"wan-phy-enable")
		echo "[setup_netdev.sh] wan-phy-enable" > /dev/console
		admin_sw_wan_phy 1
		;;
	"wan-phy-disable")
		echo "[setup_netdev.sh] wan-phy-disable" > /dev/console
		admin_sw_wan_phy 0
		;;
	"switch-phy-disable")
		echo "[setup_netdev.sh] switch-phy-disable" > /dev/console
		admin_sw_phys 0
		;;
	"wan-mac-enable")
		echo "[setup_netdev.sh] wan-mac-enable" > /dev/console
		admin_sw_wan_mac 1
		;;
	"wan-mac-disable")
		echo "[setup_netdev.sh] wan-mac-disable" > /dev/console
		admin_sw_wan_mac 0
		;;
	"lan-phy-enable")
		echo "[setup_netdev.sh] lan-phy-enable" > /dev/console
		admin_sw_lan_phys 1
		;;
	"lan-phy-disable")
		echo "[setup_netdev.sh] lan-phy-disable" > /dev/console
		admin_sw_lan_phys 0
		;;
	"switch-mac-enable")
		echo "[setup_netdev.sh] switch-mac-enable" > /dev/console
		enable_vrx_switch_ports
		;;
	"switch-mac-disable")
		echo "[setup_netdev.sh] switch-mac-disable" > /dev/console
		disable_vrx_switch_ports
		;;
	*)
		echo "$0 [init]           - init all Ethernet components"
		echo "$0 system           - init PPA & kernel modules"
		echo "$0 switch           - start switch"
		echo "$0 port-enable      - enable  all switch ports"
		echo "$0 port-disable     - disable all switch ports"
		echo "$0 mc-snoop-enable  - enable  multicsat snooping function of switch"
		echo "$0 mc-snoop-disable - disable multicsat snooping function of switch"
		;;
esac

exit 0

