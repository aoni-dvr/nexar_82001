#!/bin/sh

if [ -f /mnt/extsd/wifi_start.sh ]; then
	/mnt/extsd/wifi_start.sh
	exit 0
fi
if [ -f /mnt/extsd/WifiDisable.txt ]; then
	exit 0
fi
if [ -f /mnt/extsd/UsbWifiTestFile.txt ]; then
	exit 0
fi
FACTORY_MODE=0
if [ -f /mnt/extsd/WifiTestFile.txt ]; then
	FACTORY_MODE=1
fi

sysctl -w net.core.wmem_default=4194304
#sysctl -w net.ipv4.ip_forward=1
#or
#echo 1 > /proc/sys/net/ipv4/ip_forward
#iptables -t nat -A POSTROUTING -s 192.168.50.0/24 -o rtl0 -j MASQUERADE
WLAN_IFNAME=wlan0

WIFI_EN_GPIO=32

wait_mmc_add()
{
	if [ "${WIFI_EN_STATUS}" == "" ]; then
		WIFI_EN_STATUS=1
	fi
	/usr/local/share/script/t_gpio.sh ${WIFI_EN_GPIO} $(($(($WIFI_EN_STATUS + 1)) % 2))
	sleep 0.5
	/usr/local/share/script/t_gpio.sh ${WIFI_EN_GPIO} ${WIFI_EN_STATUS}
	mmci=`grep mmc /proc/ambarella/mmc_fixed_cd |awk $'{print $1}'|cut -c 4|tail -n 1`
	echo "${mmci} 1" > /proc/ambarella/mmc_fixed_cd

	n=0
	while [ -z "`ls /sys/bus/sdio/devices`" ] && [ $n -ne 30 ]; do
		n=$(($n + 1))
		sleep 0.1
	done
}

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
	dos2unix /mnt/extsd/settings.ini
	conf=`cat /mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
else
	conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
fi
export `echo "${conf}" | grep -vI $'^\xEF\xBB\xBF'`

export WIFI_MODE=${boot_mode}

if [ "${WIFI_EN_GPIO}" != "" ] && [ -z "`ls /sys/bus/sdio/devices`" ]; then
	wait_mmc_add
fi

#load driver
if [ $FACTORY_MODE -ne 0 ]; then
	/usr/local/share/script/load.sh "${WIFI_MODE}" wifi_test
else
	/usr/local/share/script/load.sh "${WIFI_MODE}"
fi

waitagain=1
if [ "`ls /sys/bus/sdio/devices`" != "" ] || [ "`ls /sys/bus/usb/devices 2>/dev/null`" != "" ]; then
	wait_wlan
fi
if [ $waitagain -ne 0 ]; then
	echo "There is no WIFI interface!"
	exit 1
fi
echo "found WIFI interface!"

ifconfig ${WLAN_IFNAME} up
wl country US

if [ $FACTORY_MODE -ne 0 ]; then
	exit 0
fi

if [ "${WIFI_MODE}" == "p2p" ] ; then
	/usr/local/share/script/p2p_start.sh $@
	echo 'wifibootdone' > /sys/module/rpmsg_echo/parameters/example_printk
elif [ "${WIFI_MODE}" == "sta" ] ; then
	echo 'wifibootdone' > /sys/module/rpmsg_echo/parameters/example_printk
	/usr/local/share/script/sta_start.sh $@
else
	/usr/local/share/script/ap_start.sh $@
	echo 'wifibootdone' > /sys/module/rpmsg_echo/parameters/example_printk
fi

