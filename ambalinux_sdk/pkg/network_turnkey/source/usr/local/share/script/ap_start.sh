#!/bin/sh

WLAN_IFNAME=wlan0

# return: 1 as setting as nl80211
chk_nl80211()
{
	# Atheros
	if [ -e /sys/module/ar6000 ]; then
		return 0
	fi

	# Broadcom
	if [ -e /sys/module/dhd ]; then
		return 0
	fi

	return 1
}

# return: 1 as setting as HT80
chk_hostapd_ac_HT80()
{
	if [ -e /sys/module/sd8997 ] || [ -e /sys/module/usb8997 ]; then
		return 1
	fi

	if [ -e /sys/module/sd8987 ]; then
		return 1
	fi

	if [ -e /sys/module/sd8887 ]; then
		return 1
	fi

	return 0
}

hostapd_conf()
{
	#generate hostapd.conf
	echo "interface=${WLAN_IFNAME}" > /tmp/hostapd.${WLAN_IFNAME}.conf
	echo "ctrl_interface=/var/run/hostapd_${WLAN_IFNAME}" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	echo "beacon_int=100" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	echo "dtim_period=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	echo "preamble=0" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	#WPS support
	echo "wps_state=2" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	echo "eap_server=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	#AP_SSID
	echo "AP_SSID=${AP_SSID}"
	echo "ssid=${AP_SSID}" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	#AP_MAXSTA
	echo "max_num_sta=${AP_MAXSTA}" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	#AP_ISOLATE
	echo "ap_isolate=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	#AP_CHANNEL
	if [ "${AP_MODE}" = "5g" ]; then
		if [ $AP_CHANNEL -le 0 ] || [ $AP_CHANNEL -lt 36 ] || [ $AP_CHANNEL -gt 48 ]; then
			AP_CHANNEL=`echo $(( ($RANDOM % 4) * 4 + 36 ))`
		fi
	else
		if [ $AP_CHANNEL -le 0 ] || [ $AP_CHANNEL -gt 11 ]; then
			AP_CHANNEL=`echo $(( ($RANDOM % 11) + 1 ))`
		fi
	fi
	echo "channel=${AP_CHANNEL}" >> /tmp/hostapd.${WLAN_IFNAME}.conf

	#WEP, WPA, No Security
	echo "AP_PUBLIC=${AP_PUBLIC}"
	if [ "${AP_PUBLIC}" != "yes" ]; then
		#WPA
		echo "wpa=2" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		echo "wpa_pairwise=CCMP" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		echo "wpa_passphrase=${AP_PASSWD}" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		echo "wpa_key_mgmt=WPA-PSK" >> /tmp/hostapd.${WLAN_IFNAME}.conf
	fi

	# Check nl80211
	chk_nl80211
	rval=$?
	if [ ${rval} -eq 1 ]; then
		echo "driver=nl80211" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		if [ ${AP_CHANNEL} -gt 14 ]; then
			echo "hw_mode=a" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		else
			echo "hw_mode=g" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		fi
		echo "ieee80211n=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		if [ ! -e /sys/module/bcmdhd ] && [ ! -e /sys/module/ifxahd ] && [ ! -e /sys/module/cywdhd ] && [ ! -e /sys/module/sd8801 ]; then
			case ${AP_CHANNEL} in
				1 | 2 | 3 | 4 | 5 | 6 | 36 | 44 | 52 | 60 | 149 | 157)
					# HT40+ for 1-7 (1-9 in Europe/Japan)
					echo "ht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+]" >> /tmp/hostapd.${WLAN_IFNAME}.conf
					;;
				7 | 8 | 9 | 10 | 11 | 12 | 40 | 48 | 56 | 64 | 153 | 161)
					# HT40- for 5-13
					echo "ht_capab=[SHORT-GI-20][SHORT-GI-40][HT40-]" >> /tmp/hostapd.${WLAN_IFNAME}.conf
					;;
				*)
					echo "ht_capab=[SHORT-GI-20][SHORT-GI-40]" >> /tmp/hostapd.${WLAN_IFNAME}.conf
					;;
			esac
		fi
		# 802.11ac checking.
		if [ ${AP_CHANNEL} -gt 14 ]; then
			# Check HT80 for 802.11ac, TODO: by 'iw list', e.g. short GI (80 MHz)
			chk_hostapd_ac_HT80
			rval=$?
			if [ ${rval} -eq 1 ]; then
				case ${AP_CHANNEL} in
					36|40|44|48) seg0_idx=42;;
					52|56|60|64) seg0_idx=58;;
					100|104|108|112) seg0_idx=106;;
					116|120|124|128) seg0_idx=122;;
					132|136|140|144) seg0_idx=138;;
					149|153|157|161) seg0_idx=155;;
					*) seg0_idx=0;;
				esac
				if [ ${seg0_idx} -gt 0 ]; then
					echo "ieee80211ac=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
					echo "vht_capab=[SHORT-GI-80][HTC-VHT]" >> /tmp/hostapd.${WLAN_IFNAME}.conf
					echo "vht_oper_chwidth=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
					echo "vht_oper_centr_freq_seg0_idx=${seg0_idx}" >> /tmp/hostapd.${WLAN_IFNAME}.conf
				fi
			fi
		fi

		#echo "wme_enabled=1" >> /tmp/hostapd.${WLAN_IFNAME}.conf
		#echo "wpa_group_rekey=86400" >> /tmp/hostapd.${WLAN_IFNAME}.conf

		if [ -e /sys/module/8189es ]; then
			if [ "`uname -r`" != "2.6.38.8" ]; then
				echo "wowlan_triggers=any" >> /tmp/hostapd.${WLAN_IFNAME}.conf
			fi
		fi
	fi
}

wpa_supplicant_conf()
{
	if [ -e /sys/module/ar6000 ]; then
		driver=wext
	else
		driver=nl80211
	fi

	#generate /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	echo "ctrl_interface=/var/run/wpa_supplicant_${WLAN_IFNAME}" > /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	echo "ap_scan=2" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf

	#AP_MAXSTA
	echo "max_num_sta=${AP_MAXSTA}" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf

	echo "network={" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	#AP_SSID
	echo "AP_SSID=${AP_SSID}"
	echo "ssid=\"${AP_SSID}\"" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf

	#AP_CHANNEL
	echo "AP_CHANNEL=${AP_CHANNEL}"
	if [ "${AP_MODE}" = "5g" ]; then
		if [ $AP_CHANNEL -le 0 ] || [ $AP_CHANNEL -lt 36 ] || [ $AP_CHANNEL -gt 48 ]; then
			AP_CHANNEL=`echo $(( ($RANDOM % 4) * 4 + 36 ))`
		fi
		# 5G: 5000 + ch * 5
		echo "frequency=$((5000 + $AP_CHANNEL * 5))" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	else
		if [ $AP_CHANNEL -le 0 ] || [ $AP_CHANNEL -gt 11 ]; then
			AP_CHANNEL=`echo $(( ($RANDOM % 11) + 1 ))`
		fi
		# 2.4G: 2412 + (ch-1) * 5
		echo "frequency=$((2412 + ($AP_CHANNEL - 1) * 5))" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	fi

	#WEP, WPA, No Security
	if [ "${AP_PUBLIC}" != "yes" ]; then
		# proto defaults to: WPA RSN
		echo "proto=WPA2" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
		echo "pairwise=CCMP" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
		echo "group=CCMP" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
		echo "psk=\"${AP_PASSWD}\"" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
		echo "key_mgmt=WPA-PSK" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	else
		echo "key_mgmt=NONE" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	fi
	echo "mode=2" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	echo "}" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	if [ -e /sys/module/bcmdhd ] || [ -e /sys/module/ifxahd ] || [ -e /sys/module/cywdhd ]; then
		echo "p2p_disabled=1" >> /tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf
	fi
}

bcm_ap_start()
{
	#AP_SSID
	echo "AP_SSID=${AP_SSID}"

	#AP_MAXSTA
	echo "AP_MAXSTA=${AP_MAXSTA}"

	#AP_CHANNEL
	echo "AP_CHANNEL=${AP_CHANNEL}"
	if [ ${AP_CHANNEL} -lt 0 ]; then
		reset_conf
		return 1
	fi
	if [ $AP_CHANNEL -eq 0 ]; then
		#choose 1~10 for HT40
		#AP_CHANNEL=`echo $(( $RANDOM % 11 +1 ))`
		AP_CHANNEL=`echo $(( ($RANDOM % 3) * 5 + 1 ))`
		echo "Random AP_CHANNEL=${AP_CHANNEL}"
	fi

	ifconfig ${WLAN_IFNAME} down
	wl down
	wl ap 0
	wl ap 1
	wl ssid "$AP_SSID"
	#wl bssmax $AP_MAXSTA
	wl channel $AP_CHANNEL

	#WEP, WPA, No Security
	echo "AP_PUBLIC=${AP_PUBLIC}"

	# auth: set/get 802.11 authentication. 0 = OpenSystem, 1 = SharedKey, 2 = Open/Shared.
	# wpa_auth
	#	Bitvector of WPA authorization modes:
	#	1    WPA-NONE
	#	2    WPA-802.1X/WPA-Professional
	#	4    WPA-PSK/WPA-Personal
	#	64   WPA2-802.1X/WPA2-Professional
	#	128  WPA2-PSK/WPA2-Personal
	#	0    disable WPA
	if [ "${AP_PUBLIC}" != "yes" ]; then
		wl wpa_auth 128

		# wsec  wireless security bit vector
		#	1 - WEP enabled
		#	2 - TKIP enabled
		#	4 - AES enabled
		#	8 - WSEC in software
		#	0x80 - FIPS enabled
		#	0x100 - WAPI enabled
		wl wsec 4

		echo "AP_PASSWD=${AP_PASSWD}"
		wl set_pmk "$AP_PASSWD"
	else
		wl auth 0
		wl wpa_auth 0
		wl wsec 0
	fi

	wl mpc 0
	wl frameburst 1
	wl up
	ifconfig ${WLAN_IFNAME} up

	return 0
}

apply_ap_conf()
{
	#LOCAL_IP
	killall udhcpc
	ifconfig ${WLAN_IFNAME} $LOCAL_IP
	if [ $? -ne 0 ]; then
		return 1
	fi
	#route add default gw $LOCAL_IP

	#LOCAL_NETMASK
	ifconfig ${WLAN_IFNAME} netmask $LOCAL_NETMASK
	if [ $? -ne 0 ]; then
		return 1
	fi

	if [ -e /sys/module/dhd ]; then
		# Broadcom bcm43362, etc.
		bcm_ap_start
		rval=$?
	else
		which hostapd
		if [ $? -ne 0 ] || [ -e /sys/module/ath6kl_sdio ]; then
			wpa_supplicant_conf
			rval=$?
			if [ ${rval} -ne 0 ]; then
				return 1
			fi
			wpa_supplicant -D${driver} -i${WLAN_IFNAME} -c/tmp/wpa_supplicant.ap.${WLAN_IFNAME}.conf -s -B
			rval=$?
		else
			hostapd_conf
			rval=$?
			if [ ${rval} -ne 0 ]; then
				return 1
			fi
			hostapd -B /tmp/hostapd.${WLAN_IFNAME}.conf
			rval=$?
		fi
	fi

	if [ ${rval} -ne 0 ]; then
		return 1
	fi
	if [ -e /sys/module/ar6000 ] && [ $AP_CHANNEL -eq 0 ]; then
		#ACS (Automatic Channel Selection) between 1, 6, 11
		iwconfig ${WLAN_IFNAME} channel 0
		iwconfig ${WLAN_IFNAME} commit
	fi

	return 0
}

if [ -f /mnt/extsd/settings.ini ]; then
	conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
else
	conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`
#export ap_post_ssid=`ifconfig ${WLAN_IFNAME} | grep HWaddr | awk '{print $NF}' | sed 's/://g' | cut -c $((7))- | tr 'a-z' 'A-Z'`

export AP_CHANNEL=${wifi_channel}
if [ -f /mnt/extsd/wifi_ssid.txt ]; then
	export AP_SSID=`cat /mnt/extsd/wifi_ssid.txt`
else
	if [ "${wifi_mode}" == "5g" ]; then
		export AP_SSID="Nexar-One(5G)-${ap_post_ssid}"
	else
		export AP_SSID="Nexar-One-${ap_post_ssid}"
	fi
fi
export AP_PASSWD=${ap_password}
export AP_MODE=${wifi_mode}
export AP_HIDE_SSID=0
export AP_MAXSTA=20

if [ "${AP_PASSWD}" == "" ]; then
	export AP_PUBLIC=yes
else
	export AP_PUBLIC=no
fi
export LOCAL_IP=${local_ip}
export LOCAL_NETMASK=255.255.255.0
export DHCP_IP_START=${LOCAL_IP%*.*}.20
export DHCP_IP_END=${LOCAL_IP%*.*}.200

#Load the parameter settings
apply_ap_conf
rval=$?
echo -e "rval=${rval}\n"
if [ ${rval} -ne 0 ]; then
	killall -9 hostapd wpa_supplicant 2>/dev/null
	apply_ap_conf
fi
