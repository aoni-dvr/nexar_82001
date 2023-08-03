#!/bin/sh

WLAN_IFNAME=wlan0

if [ -e /sys/module/ar6000 ]; then
	driver=wext
elif [ -e /sys/module/dhd ]; then
	driver=wext
	wl ap 0
	wl mpc 0
	wl frameburst 1
	wl up
else
	driver=nl80211
fi

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
	killall -9 wpa_supplicant 2>/dev/null
	rm -rf /var/run/wpa_supplicant_${WLAN_IFNAME}
	if [ -e /sys/module/bcmdhd ] || [ -e /sys/module/ifxahd ] || [ -e /sys/module/cywdhd ]; then
		echo "p2p_disabled=1" > /tmp/wpa_scan.conf
		wpa_supplicant -D${driver} -i${WLAN_IFNAME} -C /var/run/wpa_supplicant_${WLAN_IFNAME} -s -B -c /tmp/wpa_scan.conf
	else
		wpa_supplicant -D${driver} -i${WLAN_IFNAME} -C /var/run/wpa_supplicant_${WLAN_IFNAME} -s -B
	fi
	ifconfig ${WLAN_IFNAME} up
	#-A option: block and return after CTRL-EVENT-SCAN-RESULTS
	scan_result=`wpa_cli -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} -A /usr/local/share/script/wpa_event.sh`
	if [ $? -ne 0 ]; then
		wpa_cli -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} scan
		sleep 1
		scan_result=`wpa_cli -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} scan_r`
		n=1
		while [ "${scan_result}" == "" ] && [ $n -ne 8 ]; do
			sleep 0.5
			n=$(($n + 1))
			scan_result=`wpa_cli -i ${WLAN_IFNAME} -p /var/run/wpa_supplicant_${WLAN_IFNAME} scan_r`
		done
	fi

	scan_entry=`echo "${scan_result}" | tr '\t' ' ' | grep " ${ESSID}$" | tail -n 1`
	#echo "${scan_result}"
}

WPA_GO ()
{
	killall -9 wpa_supplicant 2>/dev/null
	rm -rf /var/run/wpa_supplicant_${WLAN_IFNAME}
	wpa_supplicant -D${driver} -i${WLAN_IFNAME} -c/tmp/wpa_supplicant.${WLAN_IFNAME}.conf -s -B
	udhcpc -i ${WLAN_IFNAME} -A 1 -b
	wait_ip_done
}

if [ -f /mnt/extsd/settings.ini ]; then
	conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
else
	conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`

export ESSID=${sta_ssid}
export PASSWORD=${sta_password}
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

if [ -e /sys/module/bcmdhd ] || [ -e /sys/module/ifxahd ] || [ -e /sys/module/cywdhd ]; then
	rm -f /tmp/wpa_scan.conf
	echo "p2p_disabled=1" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
fi
if [ -e /sys/module/8189es ] || [ -e /sys/module/8723bs ]; then
	if [ "`uname -r`" != "2.6.38.8" ]; then
		echo "wowlan_triggers=any" >> /tmp/wpa_supplicant.${WLAN_IFNAME}.conf
	fi
fi

WPA_GO
