#!/bin/sh

if [ ! -e /tmp/wifi.preloaded ]; then
	/usr/local/share/script/preload.sh
fi
rm -f /tmp/wifi.preloaded

KO=ifxahd.ko
# BCM will get name by configure.
BCM=cypress
if [ "$2" = "wifi_test" ]; then
MFG_MODE=1
P_FW="firmware_path=/usr/local/${BCM}/cyw43455-sdio-mfgtest.bin"
P_CLM="clm_path=/usr/local/${BCM}/cyw43455-sdio-mfgtest.clm_blob"
else
MFG_MODE=0
if [ -e /mnt/extsd/cyfmac43455-sdio.bin ]; then
P_FW="firmware_path=/mnt/extsd/cyfmac43455-sdio.bin"
else
P_FW="firmware_path=/usr/local/${BCM}/cyfmac43455-sdio.bin"
fi
if [ -e /mnt/extsd/cyfmac43455-sdio.clm_blob ]; then
P_CLM="clm_path=/mnt/extsd/cyfmac43455-sdio.clm_blob"
else
P_CLM="clm_path=/usr/local/${BCM}/cyfmac43455-sdio.clm_blob"
fi
fi

if [ -e /mnt/extsd/nvram.txt ]; then
P_NVRAM="nvram_path=/mnt/extsd/nvram.txt"
else
P_NVRAM="nvram_path=/usr/local/${BCM}/nvram.txt"
fi
P_IF="iface_name=wlan"
#P_DBG="dhd_msg_level=0x1 dhd_console_ms=0x10"
P_DBG=""

load_mod()
{
	# use custom wifi mac
	if [ -f /mnt/extsd/wifi_mac ]; then
		mac=`cat /mnt/extsd/wifi_mac`
	else
		mac=`burn_info wifi_mac`
	fi
	echo "${mac}" > /tmp/wifi_mac
	if [ ${MFG_MODE} == 0 ] && [ "${mac}" != "00:00:00:00:00:00" ] && [ "${mac}" != "" ] && [ ${#mac} == 17 ]; then
		insmod /lib/modules/${KO} ${P_FW} ${P_NVRAM} ${P_CLM} ${P_DBG} mac_saddr=${mac//:/}
	else
		insmod /lib/modules/${KO} ${P_FW} ${P_NVRAM} ${P_CLM} ${P_DBG}
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

load_mod

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
