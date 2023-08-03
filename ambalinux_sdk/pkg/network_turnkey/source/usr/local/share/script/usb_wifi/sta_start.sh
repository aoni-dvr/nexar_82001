#!/bin/sh

WLAN_IFNAME=${1}
if [ "${WLAN_IFNAME}" == "" ]; then
	echo "[$0] no wlan name provide"
	exit 1
fi

driver=nl80211

wait_ip_done ()
{
	n=0
	wlan_ready=`ifconfig ${WLAN_IFNAME} | grep "inet addr"`
	while [ "${wlan_ready}" == "" ] && [ $n -ne 10 ]; do
		wlan_ready=`ifconfig ${WLAN_IFNAME} | grep "inet addr"`
		n=$(($n + 1))
		sleep 1
	done

	if [ "${wlan_ready}" == "" ]; then
		echo "[$0] Cannot get IP within 10 sec"
	fi
}

WPA_SCAN ()
{
	killall -9 wpa_supplicant_usb 2>/dev/null
	rm -rf /var/run/wpa_supplicant_${WLAN_IFNAME}
	wpa_supplicant_usb -D${driver} -i${WLAN_IFNAME} -C /var/run/wpa_supplicant_${WLAN_IFNAME} -B
	ifconfig ${WLAN_IFNAME} up
	wpa_cli_usb -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} scan
	sleep 1
	scan_result=`wpa_cli_usb -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} scan_r`
	#echo "${scan_result}"
	n=1
	while [ "${scan_result}" == "" ] && [ $n -ne 8 ]; do
		sleep 0.5
		n=$(($n + 1))
		scan_result=`wpa_cli_usb -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} scan_r`
	done
	scan_entry=`echo "${scan_result}" | tr '\t' ' ' | grep " ${ESSID}$" | tail -n 1`
	#echo "${scan_result}"
}

WPA_GO ()
{
	killall -9 wpa_supplicant_usb 2>/dev/null
	rm -rf /var/run/wpa_supplicant_${WLAN_IFNAME}
	wpa_supplicant_usb -D${driver} -i${WLAN_IFNAME} -c/tmp/wpa_supplicant.${WLAN_IFNAME}.conf -B
	udhcpc -i ${WLAN_IFNAME} -A 1 -b
	wait_ip_done
}

if [ -f /mnt/extsd/factory2.txt ]; then
	conf=`cat /mnt/extsd/factory2.txt | grep -Ev "^#" | grep -Ev "^\["`
else
	if [ -f /mnt/extsd/settings.ini ]; then
		conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
	else
		conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
	fi
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`

export ESSID=${usb_sta_ssid}
export PASSWORD=${usb_sta_password}
export STA_SKIP_SCAN=yes

WPA_SCAN
if [ "${scan_entry}" == "" ]; then
	while [ "${scan_entry}" == "" ]; do
		#echo -e "\e[1;35m $0 will retry for $FORCE_RESCAN_TIMES times, start re-scan $scan_retry \e[0m"
		WPA_SCAN
	done
fi

#echo -e "\033[031m ${scan_entry} \033[0m"
echo "ctrl_interface=/var/run/wpa_supplicant_${WLAN_IFNAME}" > /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
echo "network={" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
echo "ssid=\"${ESSID}\"" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
echo "scan_ssid=1" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
WEP=`echo "${scan_entry}" | grep WEP`
WPA=`echo "${scan_entry}" | grep WPA`
WPA2=`echo "${scan_entry}" | grep WPA2`
CCMP=`echo "${scan_entry}" | grep CCMP`
TKIP=`echo "${scan_entry}" | grep TKIP`

if [ "${WPA}" != "" ]; then
	echo "key_mgmt=WPA-PSK" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf

	if [ "${WPA2}" != "" ]; then
		echo "proto=WPA2" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
	else
		echo "proto=WPA" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
	fi

	if [ "${CCMP}" != "" ]; then
		echo "pairwise=CCMP" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
	else
		echo "pairwise=TKIP" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
	fi

	echo "psk=\"${PASSWORD}\"" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
fi

if [ "${WEP}" != "" ] && [ "${WPA}" == "" ]; then
	echo "key_mgmt=NONE" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
        echo "wep_key0=${PASSWORD}" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
        echo "wep_tx_keyidx=0" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
fi

if [ "${WEP}" == "" ] && [ "${WPA}" == "" ]; then
	echo "key_mgmt=NONE" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
fi

echo "}" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf

WPA_GO
