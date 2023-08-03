#!/bin/sh

if [ -f /mnt/extsd/UsbWifiDisable.txt ]; then
	exit 0
fi
if [ -f /mnt/extsd/WifiTestFile.txt ]; then
	exit 0
fi
FACTORY_MODE=0
if [ -f /mnt/extsd/UsbWifiTestFile.txt ]; then
	FACTORY_MODE=1
fi

sysctl -w net.core.wmem_default=4194304

WLAN_IFNAME=${1}
if [ "${WLAN_IFNAME}" == "" ]; then
	echo "[$0] no wlan name provide"
	exit 1
fi

wait_wlan ()
{
	n=0
	ifconfig ${WLAN_IFNAME}
	waitagain=$?
	while [ $waitagain -ne 0 ] && [ $n -ne 30 ]; do
		n=$(($n + 1))
		sleep 0.2
		ifconfig ${WLAN_IFNAME}
		waitagain=$?
	done
}

if [ -f /mnt/extsd/settings.ini ]; then
	conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
else
	conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`

export WIFI_MODE=${usb_boot_mode}

waitagain=1
if [ "`ls /sys/bus/usb/devices 2>/dev/null`" != "" ]; then
	wait_wlan
fi
if [ $waitagain -ne 0 ]; then
	echo "There is no USB WIFI interface!"
	exit 1
fi
echo "found USB WIFI interface!"
ifconfig ${WLAN_IFNAME} up

if [ $FACTORY_MODE -ne 0 ]; then
	exit 0
fi

if [ "${WIFI_MODE}" == "p2p" ] ; then
	/usr/local/share/script/usb_wifi/p2p_start.sh $@
	echo 'usbwifibootdone' > /sys/module/rpmsg_echo/parameters/example_printk
elif [ "${WIFI_MODE}" == "sta" ] ; then
	echo 'usbwifibootdone' > /sys/module/rpmsg_echo/parameters/example_printk
	/usr/local/share/script/usb_wifi/sta_start.sh $@
else
	/usr/local/share/script/usb_wifi/ap_start.sh $@
	echo 'usbwifibootdone' > /sys/module/rpmsg_echo/parameters/example_printk
fi
