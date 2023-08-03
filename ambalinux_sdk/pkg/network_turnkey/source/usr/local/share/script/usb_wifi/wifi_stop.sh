#!/bin/sh

WLAN_IFNAME=${1}
if [ "${WLAN_IFNAME}" == "" ]; then
	echo "[$0] no wlan name provide"
	exit 1
fi

killall hostapd_usb hostapd_cli_usb udhcpc wpa_supplicant_usb wpa_cli_usb 2> /dev/null
rm -rf /var/run/hostapd_${WLAN_IFNAME} /var/run/wpa_supplicant_${WLAN_IFNAME}
ifconfig ${WLAN_IFNAME} 0.0.0.0
ifconfig ${WLAN_IFNAME} down
