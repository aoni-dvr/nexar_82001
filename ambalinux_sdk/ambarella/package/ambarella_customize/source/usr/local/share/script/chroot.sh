#!/bin/sh
mount -t devtmpfs devtmpfs /dev
mount -a
modprobe realtek

S60nfs ()
{
	#cluster0 only
	if [ "`xxd -p /sys/firmware/devicetree/base/cpus/cpu@0/reg`" == "00000000" ]; then
		mkdir -p /tmp/share

		echo "/ *(rw,sync,no_subtree_check,wdelay,insecure,no_root_squash)" > /etc/exports
		echo "/tmp/ *(fsid=1,rw,sync,no_subtree_check,wdelay,insecure,no_root_squash)" >> /etc/exports
		echo "/tmp/SD0/ *(fsid=2,rw,sync,no_subtree_check,wdelay,insecure,no_root_squash)" >> /etc/exports
		echo "/tmp/SD1/ *(fsid=3,rw,sync,no_subtree_check,wdelay,insecure,no_root_squash)" >> /etc/exports
		echo "/tmp/nvme/ *(fsid=4,rw,sync,no_subtree_check,wdelay,insecure,no_root_squash)" >> /etc/exports

		#mount bind must happen before S60nfs
		/etc/init.d/S60nfs start
	elif [ "`xxd -p /sys/firmware/devicetree/base/cpus/cpu@0/reg`" == "00010000" ]; then
	        mkdir -p /tmp/share
	        mount -t nfs 172.16.1.1:/tmp/share /tmp/share
	elif [ "`xxd -p /sys/firmware/devicetree/base/cpus/cpu@0/reg`" == "00020000" ]; then
	        mkdir -p /tmp/share
	        mount -t nfs 172.16.2.1:/tmp/share /tmp/share
	elif [ "`xxd -p /sys/firmware/devicetree/base/cpus/cpu@0/reg`" == "00030000" ]; then
	        mkdir -p /tmp/share
	        mount -t nfs 172.16.3.1:/tmp/share /tmp/share
	fi
}

#common init for all clusters
for i in /etc/init.d/S??* ;do
	case "$i" in
		/etc/init.d/S40network)
			#network already started
			;;
		/etc/init.d/S60nfs)
			S60nfs
			;;
		/etc/init.d/S99bootdone)
			#not used
			;;
		*)
			$i start
			;;
	esac
done

#/tmp/mnt: external root filesystem
mkdir /tmp/mnt
export LD_LIBRARY_PATH=/tmp/mnt/usr/lib:/tmp/mnt/lib
export PATH=/tmp/mnt/sbin:/tmp/mnt/usr/sbin:/tmp/mnt/bin:/tmp/mnt/usr/bin:$PATH

setsid cttyhack /bin/sh -l
