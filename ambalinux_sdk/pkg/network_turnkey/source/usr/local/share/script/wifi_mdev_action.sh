#!/bin/sh

# Input from /etc/mdev.conf: $ACTION $DEVPATH
echo $@ >> /tmp/net_action_log.txt

# Only handle USB WiFi
# ACTION:add SUBSYSTEM:net DEVNAME:(null)
# DEVPATH:/devices/platform/ambarella-ehci/usb1/1-1/1-1.3/1-1.3:1.0/net/wlan0

usb=`echo $2 | awk -F "/usb" '{print $2}'`
net=`echo $2 | awk -F "net/" '{print $2}'`
if [ `echo $net | cut -c 0-3` != "rtl" ]; then
	exit 0
fi
if [ "$1" == "add" ]; then
	if [ "${usb}" != "" ]; then
		/usr/local/share/script/usb_wifi/wifi_start.sh "$net"
	fi
else
	if [ "${usb}" != "" ]; then
		/usr/local/share/script/usb_wifi/wifi_stop.sh "$net"
	fi
fi
