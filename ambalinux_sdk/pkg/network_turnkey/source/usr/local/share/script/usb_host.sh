#!/bin/sh
# Get USB owner
modprobe usbcore 2>/dev/null
modprobe ehci-hcd 2>/dev/null
modprobe ehci-ambarella 2>/dev/null
modprobe ohci-hcd 2>/dev/null

# usb network
modprobe usbnet 2>/dev/null
modprobe cdc_wdm 2>/dev/null
modprobe qmi_wwan 2>/dev/null
modprobe cdc_ether 2>/dev/null
modprobe rndis_host 2>/dev/null

# uvc camera
#modprobe uvcvideo 2>/dev/null
#mjpg_streamer -i "/usr/lib/mjpg-streamer/input_uvc.so" -o "/usr/lib/mjpg-streamer/output_http.so -p 8090 -w /usr/share/mjpg-streamer/www"

# Needed for P2P.
modprobe rfkill 2> /dev/null
modprobe cfg80211 ieee80211_regdom="US" 2> /dev/null
modprobe usb_common 2> /dev/null
# rtl8188fu wifi driver
#mac=`burn_info lte read wifi_mac`
#echo "${mac}" > /tmp/usb_wifi_mac
#if [ "${mac}" != "00:00:00:00:00:00" ] &&  [ "${mac}" != "" ] && [ ${#mac} == 17 ]
#then
#	insmod /lib/modules/8188fu.ko rtw_initmac=${mac}
#else
#	insmod /lib/modules/8188fu.ko
#fi
#echo 0 > /proc/net/rtl8188fu/log_level
# rtl8811au wifi driver
#insmod /lib/modules/8811au.ko
modprobe cp210x

# for ftdi usb2uart driver
#modprobe ftdi_sio

# for usb audio
#modprobe snd-usb-audio

# Skip A12 because USB NOT shared with RTOS
if [ ! -e /sys/devices/ec170000.rct/ec170050.usbphy ]; then
	echo host > /proc/ambarella/usbphy0
fi

# Coldboot there is no hotplug mdev event for /dev/ttyACM0
# So we need to use modprobe to trigger mdev
modprobe usbserial 2>/dev/null
modprobe usb_wwan 2>/dev/null
modprobe option 2>/dev/null
modprobe qcserial 2>/dev/null
modprobe cdc-acm 2>/dev/null
exit 0
