#!/bin/bash

AMBA_CUST_DIR=${AMBARELLA_PKG_DIR}/../ambarella/package/ambarella_customize/source

mkdir -p ${TARGET_DIR}/pref
mkdir -p ${TARGET_DIR}/usr/local/share/script

if [ "${BR2_AMBARELLA_SIMPLE_RCS}" != "y" ]; then
	cp ${AMBA_CUST_DIR}/etc/init.d/rcS ${TARGET_DIR}/etc/init.d/rcS
else
	cp ${AMBA_CUST_DIR}/etc/init.d/rcS.min ${TARGET_DIR}/etc/init.d/rcS
fi

rm -f ${TARGET_DIR}/etc/init.d/S50service
cp ${AMBA_CUST_DIR}/etc/init.d/S50_amba_camera_service ${TARGET_DIR}/etc/init.d
#BR2_PACKAGE_AMBA_EXAMPLE_AMBAPRINT_TTY is not exported, get it from .config
BR2_PACKAGE_AMBA_EXAMPLE_AMBAPRINT_TTY=`grep BR2_PACKAGE_AMBA_EXAMPLE_AMBAPRINT_TTY ${BR2_CONFIG} | awk -F '=' '{print $2}'`
if [ "${BR2_PACKAGE_AMBA_EXAMPLE_AMBAPRINT_TTY}" != "" ]; then
	sed -i -e 's|amba_print /dev/ttyS1|amba_print '${BR2_PACKAGE_AMBA_EXAMPLE_AMBAPRINT_TTY}'|g'  ${TARGET_DIR}/etc/init.d/S50_amba_camera_service
fi

BR2_PACKAGE_AMBA_CAMERA_AUTOSTART=`grep BR2_PACKAGE_AMBA_CAMERA_AUTOSTART ${BR2_CONFIG} | awk -F '=' '{print $2}'`
if [ "${BR2_PACKAGE_AMBA_CAMERA_AUTOSTART}" != "y" ]; then
	sed -i -e 's|^amba_svc|#amba_svc|g'  ${TARGET_DIR}/etc/init.d/S50_amba_camera_service
fi

BR2_PACKAGE_AMBA_CAMERA_SYSLOGD_PARAMS=`grep BR2_PACKAGE_AMBA_CAMERA_SYSLOGD_PARAMS ${BR2_CONFIG} | awk -F '=' '{print $2}'`
if [ "${BR2_PACKAGE_AMBA_CAMERA_SYSLOGD_PARAMS}" != "\"\"" ]; then
    sed -i -e "s|syslogd -S -l 5 -O /dev/ttyS0|`echo ${BR2_PACKAGE_AMBA_CAMERA_SYSLOGD_PARAMS} | sed 's/\"//g'`|g"  ${TARGET_DIR}/etc/init.d/S50_amba_camera_service
fi

if [ "${BR2_AMBARELLA_AUTOHIBER}" == "y" ]; then
	ln -sf /usr/local/share/script/ambernation.sh ${TARGET_DIR}/etc/init.d/S90hibernation
else
	rm -f ${TARGET_DIR}/etc/init.d/S90hibernation
fi

BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV=`grep BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV ${BR2_CONFIG} | awk -F '=' '{print $2}'`
if [ "${BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV}" = "y" ]; then
	cp -f ${AMBA_CUST_DIR}/etc/mdev.conf ${TARGET_DIR}/etc/mdev.conf
fi

#for optee secure_storage
mkdir -p ${TARGET_DIR}/data/tee/

cp -r ${AMBA_CUST_DIR}/usr/local/share/script ${TARGET_DIR}/usr/local/share
#cp -r ${AMBA_CUST_DIR}/usr/local/share/precap_clips ${TARGET_DIR}/usr/local

#fix tmpfs OOM
rm -f ${TARGET_DIR}/etc/init.d/S01syslogd ${TARGET_DIR}/etc/init.d/S02klogd
rm -f ${TARGET_DIR}/etc/securetty

#remove header files and man pages
rm -rf ${TARGET_DIR}/usr/local/include ${TARGET_DIR}/usr/local/share/man

#make /var/lib writable
mkdir -p ${TARGET_DIR}/tmp/lib
cp -a ${TARGET_DIR}/var/lib/* ${TARGET_DIR}/tmp/lib
rm -rf ${TARGET_DIR}/var/lib
ln -fs ../tmp/lib ${TARGET_DIR}/var/lib

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

cp ${AMBA_CUST_DIR}/etc/profile_icam ${TARGET_DIR}/etc/profile
sed -i -e 's|export PS1='\''# '\''|export PS1='\''\\w# '\''|g' ${TARGET_DIR}/etc/profile
sed -i -e 's|export PS1='\''$ '\''|export PS1='\''\\w$ '\''|g' ${TARGET_DIR}/etc/profile
sed -i -e 's|root:*:0:0:|root::::|g' ${TARGET_DIR}/etc/shadow

cp ${AMBA_CUST_DIR}/etc/asound.conf ${TARGET_DIR}/etc/

if [ -e ${TARGET_DIR}/etc/ssh/sshd_config ]; then
    sed -i -e 's|^#HostKey|HostKey|g' ${TARGET_DIR}/etc/ssh/sshd_config
    sed -i -e 's|#PermitRootLogin prohibit-password|PermitRootLogin yes|g' ${TARGET_DIR}/etc/ssh/sshd_config
    sed -i -e 's|#PasswordAuthentication yes|PasswordAuthentication yes|g' ${TARGET_DIR}/etc/ssh/sshd_config
    sed -i -e 's|#PermitEmptyPasswords no|PermitEmptyPasswords yes|g' ${TARGET_DIR}/etc/ssh/sshd_config
fi
