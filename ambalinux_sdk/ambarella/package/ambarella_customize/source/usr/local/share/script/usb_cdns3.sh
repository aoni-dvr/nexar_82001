#!/bin/sh

if [ -e /sys/class/usb_role/ ]; then
	rsdir=`ls /sys/class/usb_role/`
fi
if [ -e /sys/class/usb_role/${rsdir}/role ]; then
	current=`cat /sys/class/usb_role/${rsdir}/role`
	echo "current cdns3 mode: ${current}"
else
	modprobe cdns3
	modprobe cdns3-ambarella
	modprobe phy-cadence-torrent 2>/dev/null
	modprobe xhci-hcd
	modprobe xhci-plat-hcd
	if [ -e /sys/class/usb_role/ ]; then
		rsdir=`ls /sys/class/usb_role/`
	fi
fi

g_mode=`cat /proc/modules |grep "^g_"|awk '{print $1}'`

if [ "${1}" == "host" ]; then
	if [ "${g_mode}" != "" ]; then
		echo "current device running ${g_mode}"
		modprobe -r ${g_mode}
	fi
	if [ "${current}" != "host" ]; then
		echo none > /sys/class/usb_role/${rsdir}/role
		modprobe -r cdns3_ambarella
		modprobe -r cdns3
		modprobe cdns3_ambarella mode_strap=1
		modprobe cdns3
	fi
	modprobe usb-storage
	modprobe scsi_mod
	modprobe sd_mod
elif [ "${1}" == "device" ]; then
	if [ "${current}" != "device" ]; then
		echo none > /sys/class/usb_role/${rsdir}/role
		modprobe -r cdns3_ambarella
		modprobe -r cdns3
		modprobe cdns3_ambarella mode_strap=2
		modprobe cdns3
	fi

	if [ "${g_mode}" == "" ]; then
		echo "1. to run g_serial:"
		echo "   modprobe g_serial"
		echo "   setsid /sbin/getty -n -L 115200 /dev/ttyGS0 &"
		#echo "2. to run g_ether:"
		#echo "   modprobe g_ether"
		#echo "   ifconfig usb0 192.168.5.55"
		echo "2. to run g_mass_storage:"
		echo "   modprobe mmc_block"
		echo "   modprobe sdhci-ambarella"
		echo "   modprobe g_mass_storage file=/dev/mmcblk0p1 removable=1 iSerialNumber=AAAABBCC stall=0"
	fi
elif [ "${1}" == "none" ] && [ "${current}" != "none" ]; then
	echo none > /sys/class/usb_role/${rsdir}/role
	if [ "${g_mode}" != "" ]; then
		modprobe -r ${g_mode}
	fi
else
	echo "cdns3 USB32 controller script usage:"
	echo "$0 device: USB device mode"
	echo "$0 host:   USB host mode"
	echo "$0 none:   USB idle state"
fi
