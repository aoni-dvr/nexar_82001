#!/bin/sh

WLAN_IFNAME=wlan0

#  Note: wpa_supplicant from bcmdhd does not set interface down when exit.
if [ -e /sys/module/bcmdhd ] || [ -e /sys/module/cywdhd ]; then
	# Note: Need wl to set interface "real down".
	wl down
	wpa_cli -i ${WLAN_IFNAME} terminate
fi

killall hostapd hostapd_cli udhcpc wpa_supplicant wpa_cli 2> /dev/null
rm -rf /var/run/hostapd_${WLAN_IFNAME} /var/run/wpa_supplicant_${WLAN_IFNAME}
ifconfig ${WLAN_IFNAME} 0.0.0.0
ifconfig ${WLAN_IFNAME} down
if [ "${1}" = "unload" ]; then
	rmmod cywdhd
fi
