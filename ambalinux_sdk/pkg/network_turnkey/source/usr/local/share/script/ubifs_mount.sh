#!/bin/sh

need_erase=`cat /proc/cmdline | grep erase_data=1`
if [ "$need_erase" != "" ]; then
	ubiformat /dev/mtd14 -y
	ubiattach /dev/ubi_ctrl -m 14
else
	ubiattach /dev/ubi_ctrl -m 14
	if [ $? != 0 ]; then
		ubiformat /dev/mtd14 -y
		ubiattach /dev/ubi_ctrl -m 14
	fi
fi

if [ ! -f /dev/ubi0_0 ]; then
	echo "no /dev/ubi0_0, ubimkvol create vol"
	ubimkvol /dev/ubi0 -s 94MiB -N my_ubi_vol
	#dev_num=`cat /sys/class/ubi/ubi0_0/dev`
	#mknod /dev/ubi0_0 c `awk -F':' '{print $1}'` `awk -F':' '{print $2}'`
fi

mount -t ubifs ubi0_0 /data

#export part_size=`df /data/ | tail -n 1 | awk -F' ' '{print $2}'`
#if [ ${part_size} -lt 85000 ]; then
#	umount /data
#	ubirmvol /dev/ubi0 -N my_ubi_vol
#	ubidetach /dev/ubi_ctrl -m 14
#	ubiattach /dev/ubi_ctrl -m 14
#	ubimkvol /dev/ubi0 -s 94MiB -N my_ubi_vol
#	mount -t ubifs ubi0_0 /data
#fi
