#!/bin/sh

WLAN_IFNAME=wlan0

get_p2p_ht20_class()
{
	case ${P2P_OPER_CHANNEL} in
	1|2|3|4|5|6|7|8|9|10|11)
		p2p_listen_reg_class=81
	;;
	36|40|44|48)
		p2p_listen_reg_class=115
	;;
	52|56|60|64)
		p2p_listen_reg_class=118
	;;
	149|153|157|161)
		p2p_listen_reg_class=124
	;;
	100|104|108|112|116|120|124|128|132|136|140)
		p2p_listen_reg_class=121
	;;
	165)
		p2p_listen_reg_class=125
	;;
	*)
		echo "Usage: invalid channel"
		exit 1
	;;
	esac
}

get_p2p_ht40_class()
{
	case ${P2P_OPER_CHANNEL} in
	1|2|3|4|5)
		p2p_listen_reg_class=83
	;;
	6|7|8|9|10|11)
		p2p_listen_reg_class=84
	;;
	36|44)
		p2p_listen_reg_class=116
	;;
	52|60)
		p2p_listen_reg_class=119
	;;
	100|108|116|124|132)
		p2p_listen_reg_class=122
	;;
	149|157)
		p2p_listen_reg_class=126
	;;
	40|48)
		p2p_listen_reg_class=117
	;;
	56|64)
		p2p_listen_reg_class=120
	;;
	104|112|120|128|136)
		p2p_listen_reg_class=123
	;;
	153|161)
		p2p_listen_reg_class=127
	;;
	*)
		echo "Usage: invalid channel"
		exit 1
	;;
	esac
}

if [ -e /sys/module/ar6000 ]; then
	driver=ar6003
else
	driver=nl80211
fi

if [ -f /mnt/extsd/settings.ini ]; then
	conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
else
	conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`
#export ap_post_ssid=`ifconfig ${WLAN_IFNAME} | grep HWaddr | awk '{print $NF}' | sed 's/://g' | cut -c $((7))- | tr 'a-z' 'A-Z'`

export P2P_PASSWD=${ap_password}
export P2P_AUTO_CONNECT=no
# check channel
export P2P_OPER_CHANNEL=${wifi_channel}
if [ "${wifi_mode}" == "5g" ]; then
	if [ $P2P_OPER_CHANNEL -le 0 ] || [ $P2P_OPER_CHANNEL -lt 36 ] || [ $P2P_OPER_CHANNEL -gt 48 ]; then
		P2P_OPER_CHANNEL=`echo $(( ($RANDOM % 4) * 4 + 36 ))`
	fi
	export P2P_DEVICE_NAME="Nexar-One(5G)-${ap_post_ssid}"
else
	if [ $P2P_OPER_CHANNEL -le 0 ] || [ $P2P_OPER_CHANNEL -gt 11 ]; then
		P2P_OPER_CHANNEL=`echo $(( ($RANDOM % 11) + 1 ))`
	fi
	export P2P_DEVICE_NAME="Nexar-One-${ap_post_ssid}"
fi

if [ -f /mnt/extsd/wifi_ssid.txt ]; then
	export P2P_DEVICE_NAME=`cat /mnt/extsd/wifi_ssid.txt`
fi

if [ "${1}" != "" ] && [ -e /tmp/p2p.${WLAN_IFNAME}.conf ]; then
	cat /tmp/p2p.${WLAN_IFNAME}.conf
else
	echo "ctrl_interface=/var/run/wpa_supplicant" > /tmp/p2p.${WLAN_IFNAME}.conf
	echo "device_type=1-0050F204-1" >> /tmp/p2p.${WLAN_IFNAME}.conf
	#echo "config_methods=display push_button keypad" >> /tmp/p2p.${WLAN_IFNAME}.conf
	echo "config_methods=pbc" >> /tmp/p2p.${WLAN_IFNAME}.conf
	echo "persistent_reconnect=1" >> /tmp/p2p.${WLAN_IFNAME}.conf
	#device_name
	echo "device_name=${P2P_DEVICE_NAME}" >> /tmp/p2p.${WLAN_IFNAME}.conf
	echo "p2p_intra_bss=0" >> /tmp/p2p.${WLAN_IFNAME}.conf

	#p2p_go_intent
	#if [ "${P2P_GO_INTENT}" != "" ]; then
		#echo "p2p_go_intent=${P2P_GO_INTENT}" >> /tmp/p2p.${WLAN_IFNAME}.conf
	#fi

	#p2p_go_ht40
	if [ "${P2P_GO_HT40}" == "1" ]; then
		echo "p2p_go_ht40=${P2P_GO_HT40}" >> /tmp/p2p.${WLAN_IFNAME}.conf
	fi

	#p2p_oper_channel
	if [ "${P2P_OPER_CHANNEL}" != "" ]; then
		echo "p2p_oper_channel=${P2P_OPER_CHANNEL}" >> /tmp/p2p.${WLAN_IFNAME}.conf
		#echo "p2p_listen_channel=${P2P_OPER_CHANNEL}" >> /tmp/p2p.${WLAN_IFNAME}.conf
		if [ "${P2P_GO_HT40}" == "1" ]; then
			get_p2p_ht40_class
		else
			get_p2p_ht20_class
		fi
		echo "p2p_oper_reg_class=${p2p_listen_reg_class}" >> /tmp/p2p.${WLAN_IFNAME}.conf
		echo "p2p_listen_reg_class=${p2p_listen_reg_class}" >> /tmp/p2p.${WLAN_IFNAME}.conf
		echo "country=US" >> /tmp/p2p.${WLAN_IFNAME}.conf
	fi
	if [ -e /sys/module/sd8xxx ]; then
		echo "p2p_no_group_iface=1" >> /tmp/p2p.${WLAN_IFNAME}.conf
	fi
fi

wpa_supplicant -i ${WLAN_IFNAME} -c /tmp/p2p.${WLAN_IFNAME}.conf -D ${driver} -B
wpa_cli -B -a /usr/local/share/script/wpa_event.sh
wpa_cli p2p_set ssid_postfix "${P2P_DEVICE_NAME}"
wpa_cli p2p_set passphrase "${P2P_PASSWD}"
wpa_cli p2p_group_add

#if [ "${P2P_AUTO_CONNECT}" != "yes" ]; then
#	wpa_cli -i ${WLAN_IFNAME} -p wpa_supplicant_${WLAN_IFNAME} p2p_listen
#else
#	wpa_cli -i ${WLAN_IFNAME} -p wpa_supplicant_${WLAN_IFNAME} p2p_find
#fi
