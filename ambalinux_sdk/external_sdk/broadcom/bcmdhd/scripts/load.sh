#!/bin/sh

if [ ! -e /tmp/wifi.preloaded ]; then
	/usr/local/share/script/preload.sh
fi
rm -f /tmp/wifi.preloaded

KO=bcmdhd.ko
# BCM will get name by configure.
BCM=bcmdhd
if [ "$2" = "wifi_test" ]; then
MFG_MODE=1
P_FW="firmware_path=/usr/local/${BCM}/mfg_fw"
else
MFG_MODE=0
P_FW="firmware_path=/usr/local/${BCM}/fw"
fi

if [ -e /mnt/extsd/nvram.txt ]; then
P_NVRAM="nvram_path=/mnt/extsd/nvram.txt"
else
P_NVRAM="nvram_path=/usr/local/${BCM}/nvram.txt"
fi
P_IF="iface_name=wlan"
P_DBG=""

load_mod()
{
	# use custom wifi mac
	if [ -e /mnt/extsd/wifi_mac.txt ]; then
		#mac="8C:CE:4E:D0:F7:D2"
		mac=`cat /mnt/extsd/wifi_mac.txt`
	else
		mac=`burn_info wifi_mac`
	fi
	echo "${mac}" > /tmp/wifi_mac
	if [ ${MFG_MODE} == 0 ] && [ "${mac}" != "00:00:00:00:00:00" ] && [ "${mac}" != "" ] && [ ${#mac} == 17 ]; then
		insmod /lib/modules/${KO} ${P_FW}_apsta.bin ${P_NVRAM} ${P_IF} ${P_DBG} $1 amba_initmac=${mac}
	else
		mac=`printf "00:05:B5:%02X:%02X:%02X" $(($RANDOM % 256)) $(($RANDOM % 256)) $(($RANDOM % 256))`
		insmod /lib/modules/${KO} ${P_FW}_apsta.bin ${P_NVRAM} ${P_IF} ${P_DBG} $1 amba_initmac=${mac}
	fi
}

#H2: 24Mhz is more stable
#if [ `grep gclk_sdio /proc/ambarella/clock|awk '{print $2}'` -gt 24000000 ] && \
#	[ "`zcat /proc/config.gz | grep CONFIG_ARCH_AMBARELLA_S5=y`" != "" ]; then
#        if [ -e /sys/kernel/debug/mmc1 ]; then
#		echo 24000000 > /sys/kernel/debug/mmc1/clock
#	else
#		echo 24000000 > /sys/kernel/debug/mmc0/clock
#	fi
#fi

case $1 in
	sta)
		load_mod op_mode=1
	;;
	p2p)
		load_mod op_mode=1
	;;
	*)
		# Set as AP
		load_mod op_mode=2
	;;
esac

# Needed for App.
touch /tmp/wifi.loaded

# Disable WiFi CPU offloading
if [ -e /sys/module/bcmdhd/parameters/tx_coll_max_time ] && [ -e /proc/ambarella/clock ]; then
	gclk_cortex=`cat /proc/ambarella/clock | grep gclk_cortex|awk '{print $2}'`
	if [ "${gclk_cortex}" != "" ] && [ ${gclk_cortex} -gt 504000000 ]; then
		echo 0 > /sys/module/bcmdhd/parameters/tx_coll_max_time
	fi
fi

# Note: BCMDHD driver should apply patch of splitting CMD53 into 4 CMD52
#fix A9S bcm43340 SDIO command 53 timeout issue
#if [ `grep gclk_sdio /proc/ambarella/clock|awk '{print $2}'` -ge 44000000 ] && \
#	[ "`zcat /proc/config.gz | grep CONFIG_PLAT_AMBARELLA_S2E=y`" != "" ] && \
#	[ -e /sys/module/bcmdhd/parameters/info_string ] && \
#	[ "`grep 1.88.45.11 /sys/module/bcmdhd/parameters/info_string`" != "" ] && \
#	[ "`grep a94c /sys/module/bcmdhd/parameters/info_string`" != "" ]; then
#		if [ -e /sys/kernel/debug/mmc1 ]; then
#			echo 43636363 > /sys/kernel/debug/mmc1/clock
#		else
#			echo 43636363 > /sys/kernel/debug/mmc0/clock
#		fi
#fi
