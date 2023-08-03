#!/bin/bash

AMBA_CUST_DIR=${AMBARELLA_PKG_DIR}/../ambarella/package/ambarella_customize/source

mkdir -p ${TARGET_DIR}/pref
mkdir -p ${TARGET_DIR}/usr/local/share/script
mkdir -p ${TARGET_DIR}/mnt/SCAM

cp ${AMBA_CUST_DIR}/etc/init.d/S50service ${TARGET_DIR}/etc/init.d
cp ${AMBA_CUST_DIR}/etc/init.d/S51pref ${TARGET_DIR}/etc/init.d
cp ${AMBA_CUST_DIR}/etc/init.d/S60postservice ${TARGET_DIR}/etc/init.d
cp ${AMBA_CUST_DIR}/etc/init.d/S99bootdone ${TARGET_DIR}/etc/init.d

if [ "${BR2_AMBARELLA_AUTOHIBER}" == "y" ]; then
	ln -sf /usr/local/share/script/ambernation.sh ${TARGET_DIR}/etc/init.d/S90hibernation
else
	rm -f ${TARGET_DIR}/etc/init.d/S90hibernation
fi

BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV=`grep BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV ${BR2_CONFIG} | awk -F '=' '{print $2}'`
if [ "${BR2_ROOTFS_DEVICE_CREATION_DYNAMIC_MDEV}" = "y" ]; then
	cp -f ${AMBA_CUST_DIR}/etc/mdev.conf ${TARGET_DIR}/etc/mdev.conf
fi


cp -r ${AMBA_CUST_DIR}/usr/local/share/script ${TARGET_DIR}/usr/local/share

rm -f ${TARGET_DIR}/etc/init.d/S01logging
rm -f ${TARGET_DIR}/etc/securetty

#remove header files and man pages
rm -rf ${TARGET_DIR}/usr/local/include ${TARGET_DIR}/usr/local/share/man
#make /var/lib writable
mkdir -p ${TARGET_DIR}/tmp/lib
cp -a ${TARGET_DIR}/var/lib/* ${TARGET_DIR}/tmp/lib
rm -rf ${TARGET_DIR}/var/lib
ln -fs ../tmp/lib ${TARGET_DIR}/var/lib

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

#supercam3 initialization script
cp ${AMBARELLA_PKG_DIR}/supercam/cam_config.sh ${TARGET_DIR}/usr/local/share/script
chmod +x ${TARGET_DIR}/usr/local/share/script/cam_config.sh
echo "/usr/local/share/script/cam_config.sh" >> ${TARGET_DIR}/etc/init.d/S50service
cp ${AMBA_CUST_DIR}/etc/vsftpd.conf ${TARGET_DIR}/etc/
cp ${AMBA_CUST_DIR}/etc/profile ${TARGET_DIR}/etc/

#make fw version file
FWTAG=`git describe --abbrev=0`
echo "$FWTAG" > ${TARGET_DIR}/usr/local/share/.cb_hal_api_version

