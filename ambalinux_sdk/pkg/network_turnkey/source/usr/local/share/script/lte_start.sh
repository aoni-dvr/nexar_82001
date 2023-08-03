#!/bin/sh

LOGGER ()
{
	if [ "${sysl}" != "" ]; then
		logger "${b0}:${@}"
	else
		echo "$@"
	fi
}

sysl=`ps | grep syslogd | grep -v grep`
b0=`basename ${0}`

#GPS_DEV="/dev/ttyUSB1"
#GPSD=/usr/sbin/gpsd
#GPSD_PIDFILE=/var/run/gpsd.pid

LOGGER "lte action: ${ACTION}, subsystem: ${SUBSYSTEM}"

if [ "$SUBSYSTEM" != "tty" ]; then
	LOGGER "exit to prevent multiple call from mdev"
	exit 0
fi

wait_usbnet ()
{
	#ALTAIR, UBLOX, SEQUANS, GEMALTO
	if [ "${Vendor}" == "216f" ] || [ "${Vendor}" == "1546" ] || [ "${Vendor}" == "258d" ] || [ "${Vendor}" == "1e2d" ]; then
		net=`find /sys/devices/ -name net | grep ahb | grep ehci|tail -n 1`
		n=0
		while [ "${net}" == "" ]; do
			LOGGER "${n} sleep 1"
			sleep 1
			n=$(($n + 1))
			net=`find /sys/devices/ -name net | grep ahb | grep ehci|tail -n 1`
		done
		forwardif=`ls ${net}`
		LOGGER "found ${forwardif}"
	fi
}

if [ "${ACTION}" = "add" ]; then
	vidpid=`grep "Vendor=" /sys/kernel/debug/usb/devices|grep -v "Vendor=1d6b"|awk '{print $2,$3}'`
	if [ "${vidpid}" != "" ]; then
		eval `echo "${vidpid}"`
	else
		LOGGER "err: cannot get Vendor ProdID from /sys/kernel/debug/usb/devices"
		exit 1
	fi

	#if [ ! -e /dev/cdc-wdm0 ]; then
	#	echo ${Vendor} ${ProdID} > /sys/bus/usb/drivers/qmi_wwan/new_id
	#	LOGGER "echo ${Vendor} ${ProdID} > /sys/bus/usb/drivers/qmi_wwan/new_id"; sleep 0.1
	#fi

	wait_usbnet
	ifconfig usb0 up

	echo 'lteon' > /sys/module/rpmsg_echo/parameters/example_printk

	if [ -e /mnt/extsd/LteDisable.txt ]; then
		exit 0
	fi

	killall -9 quectel-CM
	#/usr/local/share/script/quectel-CM &
	/usr/local/share/script/lte_config.sh &
	#start-stop-daemon -S -q -p $GPSD_PIDFILE --exec $GPSD -- -n -P $GPSD_PIDFILE $GPS_DEV
elif [ "${ACTION}" = "remove" ]; then
	killall -9 quectel-CM
	ifconfig usb0 down
	#start-stop-daemon -K -q -p $GPSD_PIDFILE
	#rm -f $GPSD_PIDFILE
else
	exit 0
fi
