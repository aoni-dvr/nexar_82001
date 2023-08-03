#!/bin/sh

WLAN_IFNAME=${1}
if [ "${WLAN_IFNAME}" == "" ]; then
	echo "[$0] no wlan name provide"
	exit 1
fi

# return: 1 as setting as nl80211
chk_nl80211()
{
	return 1
}

# return: 1 as setting as HT80
chk_hostapd_ac_HT80()
{
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
	#AP_CHANNEL
	if [ $AP_CHANNEL -le 0 ] || [ $AP_CHANNEL -gt 11 ]; then
		AP_CHANNEL=`echo $(( ($RANDOM % 11) + 1 ))`
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

	hostapd_conf
	hostapd_usb -B /tmp/hostapd.${WLAN_IFNAME}.conf
	rval=$?
	if [ ${rval} -ne 0 ]; then
		return 1
	fi

	return 0
}

if [ -f /mnt/extsd/settings.ini ]; then
	conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
else
	conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`
#export usb_ap_post_ssid=`ifconfig ${WLAN_IFNAME} | grep HWaddr | awk '{print $NF}' | sed 's/://g' | cut -c $((7))- | tr 'a-z' 'A-Z'`

export AP_CHANNEL=${usb_wifi_channel}
export AP_SSID="Nexar-${usb_ap_post_ssid}"
export AP_PASSWD=${usb_ap_password}
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
	killall -9 hostapd_usb wpa_supplicant_usb 2>/dev/null
	apply_ap_conf
fi
