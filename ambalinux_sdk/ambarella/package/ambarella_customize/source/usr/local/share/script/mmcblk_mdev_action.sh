#!/bin/sh

#touch /tmp/mmcblk0p1.mdev to handle hotplug
if [ ! -e /tmp/${2}.mdev ] ; then
	exit 0
fi

if [ "${1}" == "add" ] && [ -e /tmp/${2}.mdev ] ; then
	if [ "${2}" == "mmcblk0p1" ] && [ "`cat /proc/mounts |grep /tmp/SD0`" == "" ]; then
		mount /dev/mmcblk0p1 /tmp/SD0
		logger "mount /dev/mmcblk0p1 /tmp/SD0"
		exportfs -arv 2>/dev/null
	fi
	if [ "${2}" == "mmcblk1p1" ] && [ "`cat /proc/mounts |grep /tmp/SD1`" == "" ]; then
		mount /dev/mmcblk1p1 /tmp/SD1
		logger "mount /dev/mmcblk1p1 /tmp/SD1"
		exportfs -arv 2>/dev/null
	fi
fi
if [ "${1}" == "remove" ] && [ -e /tmp/${2}.mdev ] ; then
	if [ "${2}" == "mmcblk0p1" ]; then
		umount -f /tmp/SD0
		logger "umount -f /tmp/SD0"
	fi
	if [ "${2}" == "mmcblk1p1" ]; then
		umount -f /tmp/SD1
		logger "umount -f /tmp/SD1"
	fi
fi
