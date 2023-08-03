#!/bin/bash

AMBA_CUST_DIR=${AMBARELLA_PKG_DIR}/eva/rootfs_source

mkdir -p ${TARGET_DIR}/pref
mkdir -p ${TARGET_DIR}/usr/local/share/script

if [ "${BR2_AMBARELLA_SIMPLE_RCS}" != "y" ]; then
	cp ${AMBA_CUST_DIR}/etc/init.d/rcS ${TARGET_DIR}/etc/init.d/rcS
else
	cp ${AMBA_CUST_DIR}/etc/init.d/rcS.min ${TARGET_DIR}/etc/init.d/rcS
fi

mkdir -p ${TARGET_DIR}/{media/recordings,media/SD0,sdcard/recordings,media/FL0,opt/amba/}
mkdir -p ${TARGET_DIR}/{media/calib,media/SD0,sdcard/calib,media/FL0,opt/amba/}
mkdir -p ${TARGET_DIR}/{media/sdag,media/SD0,sdcard/sdag,media/FL0,opt/amba/}
mkdir -p ${TARGET_DIR}/{media/fw,media/SD0,sdcard/fw,media/FL0,opt/amba/}
mkdir -p ${TARGET_DIR}/media/SSD
mkdir -p ${TARGET_DIR}/opt/amba/{etc/camcfg/,etc/idsp/,etc/superdag/,bin/superdag/,bin/flexidag/,lib/modules/,share/scripts/}
mkdir -p ${TARGET_DIR}/opt/amba/var/{log/,run/}
mkdir -p ${TARGET_DIR}/opt/amba/etc/idsp/calibration
mkdir -p ${TARGET_DIR}/opt/amba/run/idsp/calibration
mkdir -p ${TARGET_DIR}/fs0p1

rm -f ${TARGET_DIR}/etc/init.d/S50service


cp ${AMBA_CUST_DIR}/etc/init.d/S01rsyslogd ${TARGET_DIR}/etc/init.d/K01rsyslogd
cp ${AMBA_CUST_DIR}/etc/init.d/S40moduleinit ${TARGET_DIR}/etc/init.d/S40moduleinit
cp ${AMBA_CUST_DIR}/etc/init.d/S50network ${TARGET_DIR}/etc/init.d/S50network
cp ${AMBA_CUST_DIR}/etc/init.d/S60amba_camera_service_dk ${TARGET_DIR}/etc/init.d/S60amba_camera_service
cp ${AMBA_CUST_DIR}/etc/init.d/S99bootdone ${TARGET_DIR}/etc/init.d/
cp ${AMBA_CUST_DIR}/etc/exports ${TARGET_DIR}/etc/exports
cp ${AMBA_CUST_DIR}/etc/network/interfaces_dk ${TARGET_DIR}/etc/network/interfaces
cp ${AMBA_CUST_DIR}/etc/rsyslog.conf ${TARGET_DIR}/etc/

if [ "${BR2_AMBARELLA_AUTOHIBER}" == "y" ]; then
	ln -sf /usr/local/share/script/ambernation.sh ${TARGET_DIR}/etc/init.d/S90hibernation
else
	rm -f ${TARGET_DIR}/etc/init.d/S90hibernation
fi

BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV=`grep BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV ${BR2_CONFIG} | awk -F '=' '{print $2}'`
if [ "${BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV}" = "y" ]; then
	cp -f ${AMBA_CUST_DIR}/etc/mdev.conf ${TARGET_DIR}/etc/mdev.conf
fi

#cp ${AMBA_CUST_DIR}/etc/cherokee.conf ${TARGET_DIR}/etc
#cp -r ${AMBA_CUST_DIR}/usr/local/share/cherokee ${TARGET_DIR}/usr/share
#mkdir -p ${TARGET_DIR}/var/www/live
#mkdir -p ${TARGET_DIR}/var/www/DCIM
#mkdir -p ${TARGET_DIR}/var/www/shutter
#mkdir -p ${TARGET_DIR}/var/www/mjpeg
#mkdir -p ${TARGET_DIR}/var/www/pref

#for optee secure_storage
mkdir -p ${TARGET_DIR}/data/tee/

cp -r ${AMBA_CUST_DIR}/usr/local/share/script ${TARGET_DIR}/usr/local/share
#cp -r ${AMBA_CUST_DIR}/usr/local/share/precap_clips ${TARGET_DIR}/usr/local

#fix tmpfs OOM
rm -f ${TARGET_DIR}/etc/init.d/S01rsyslogd ${TARGET_DIR}/etc/init.d/S02klogd
rm -f ${TARGET_DIR}/etc/securetty

#remove header files and man pages
rm -rf ${TARGET_DIR}/usr/local/include ${TARGET_DIR}/usr/local/share/man
#make /var/lib writable
#mkdir -p ${TARGET_DIR}/tmp/lib
#cp -a ${TARGET_DIR}/var/lib/* ${TARGET_DIR}/tmp/lib
#rm -rf ${TARGET_DIR}/var/lib
#ln -fs ../tmp/lib ${TARGET_DIR}/var/lib

#make /etc/ppp writable
ln -fs /tmp/ppp ${TARGET_DIR}/etc/ppp

cp ${AMBA_CUST_DIR}/etc/inittab ${TARGET_DIR}/etc/
if [ "${BR2_AMBARELLA_LINUX_RESPAWN_SH}" != "y" ]; then
	sed -i -e 's|::respawn:-/bin/sh|#::respawn:-/bin/sh|g' ${TARGET_DIR}/etc/inittab
fi
if [ "${BR2_AMBARELLA_LINUX_TTY}" != "\"\"" ]; then
	AMBARELLA_LINUX_TTY=`echo ${BR2_AMBARELLA_LINUX_TTY}|sed -e 's/"//g'`
	sed -i -e 's|::respawn:-/bin/sh|'${AMBARELLA_LINUX_TTY}'::respawn:-/bin/sh|g' ${TARGET_DIR}/etc/inittab
	sed -i -e 's|::sysinit:/etc/init.d/rcS|'${AMBARELLA_LINUX_TTY}'::sysinit:/etc/init.d/rcS|g' ${TARGET_DIR}/etc/inittab
fi

cp ${AMBA_CUST_DIR}/etc/inputrc ${TARGET_DIR}/etc/
sed -i -e 's|^\tfind /sys/|\t# find /sys/|g' ${TARGET_DIR}/etc/init.d/S10mdev
sed -i -e 's|export PS1='\''# '\''|export PS1='\''\\w# '\''|g' ${TARGET_DIR}/etc/profile
sed -i -e 's|export PS1='\''$ '\''|export PS1='\''\\w$ '\''|g' ${TARGET_DIR}/etc/profile
sed -i -e 's|root:\*:|root::|g' ${TARGET_DIR}/etc/shadow

cp ${AMBA_CUST_DIR}/etc/asound.conf ${TARGET_DIR}/etc/

cp ${AMBA_CUST_DIR}/etc/shells ${TARGET_DIR}/etc/

# EVA APP
cp ${AMBARELLA_PKG_DIR}/eva/eva.conf ${TARGET_DIR}/opt/amba/share/scripts
cp ${AMBARELLA_PKG_DIR}/eva/.eva_api_version ${TARGET_DIR}/usr/local/share/
cp ${AMBARELLA_PKG_DIR}/eva/cam_config.sh ${TARGET_DIR}/opt/amba/share/scripts
cp ${AMBARELLA_PKG_DIR}/eva/calib_link.sh ${TARGET_DIR}/opt/amba/share/scripts
chmod +x ${TARGET_DIR}/opt/amba/share/scripts/cam_config.sh
cp ${AMBARELLA_PKG_DIR}/eva/superdag/start_run_idsprx.sh ${TARGET_DIR}/opt/amba/share/scripts
chmod +x ${TARGET_DIR}/opt/amba/share/scripts/start_run_idsprx.sh

#cp ${AMBARELLA_PKG_DIR}/supercam/auto_run.sh ${TARGET_DIR}/opt/amba/share/scripts
#chmod +x ${TARGET_DIR}/opt/amba/share/scripts/auto_run.sh
#cp -r ${AMBARELLA_PKG_DIR}/supercam/superdag/share/scripts/* ${TARGET_DIR}/opt/amba/share/scripts/
#chmod +x ${TARGET_DIR}/opt/amba/share/scripts/*
#cp ${AMBARELLA_PKG_DIR}/supercam/superdag/start_run.sh ${TARGET_DIR}/usr/local/share/script
#chmod +x ${TARGET_DIR}/usr/local/share/script/start_run.sh
##cp ${AMBARELLA_PKG_DIR}/supercam/superdag/flexidag_start_run.sh ${TARGET_DIR}/usr/local/share/script
##chmod +x ${TARGET_DIR}/usr/local/share/script/flexidag_start_run.sh
#cp ${AMBARELLA_PKG_DIR}/supercam/script/reboot.sh ${TARGET_DIR}/usr/local/share/script
#chmod +x ${TARGET_DIR}/usr/local/share/script/reboot.sh
#cp ${AMBARELLA_PKG_DIR}/supercam/script/umount.sh ${TARGET_DIR}/usr/local/share/script
#chmod +x ${TARGET_DIR}/usr/local/share/script/umount.sh
#cp ${AMBARELLA_PKG_DIR}/supercam/script/kill_superdag.sh ${TARGET_DIR}/usr/local/share/script
#chmod +x ${TARGET_DIR}/usr/local/share/script/kill_superdag.sh
#cp ${AMBARELLA_PKG_DIR}/supercam/script/calib.sh ${TARGET_DIR}/usr/local/share/script
#chmod +x ${TARGET_DIR}/usr/local/share/script/calib.sh
#cp ${AMBARELLA_PKG_DIR}/supercam/script/remount_calib.sh ${TARGET_DIR}/usr/local/share/script
#chmod +x ${TARGET_DIR}/usr/local/share/script/remount_calib.sh
#cp ${TARGET_DIR}/lib/modules/4.14.183/kernel/drivers/ambacv/ambacv.ko ${TARGET_DIR}/opt/amba/lib/modules
#cp ${TARGET_DIR}/lib/modules/4.14.183/kernel/drivers/ambasys/ambasys.ko ${TARGET_DIR}/opt/amba/lib/modules
