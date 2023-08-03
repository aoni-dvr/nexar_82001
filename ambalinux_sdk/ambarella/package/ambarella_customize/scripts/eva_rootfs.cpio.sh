#!/bin/bash

AMBA_CUST_DIR=${AMBARELLA_PKG_DIR}/eva/rootfs_source
deleteme=`find ${TARGET_DIR}/lib/modules/ -name "*.ko"|grep -Ev "(ubifs.ko|\
phy-cadence-torrent.ko|\
i2c-ambarella.ko|\
rpmsg_echo.ko|\
veth.ko|\
virtio_rpmsg_bus.ko|\
rpmsg_core.ko|\
rpmsg_char.ko|\
rpmsg_ns.ko|\
sdhci-ambarella.ko|\
sdhci-pltfm.ko|\
sdhci.ko|\
mmc_block.ko|\
mmc_core.ko|\
amba_rproc.ko|\
ambarella_combo_nand.ko|\
ambarella-spinor.ko|\
spi-nor.ko|\
ubi.ko|\
bch.ko)"`

#if [ "${deleteme}" != "" ]; then
#	rm -fv ${deleteme}
#fi

#log to dram cause OOM
rm -f ${TARGET_DIR}/etc/init.d/S01syslogd ${TARGET_DIR}/etc/init.d/S02klogd
#export PS1='\w# '
sed -i -e 's|export PS1='\''# '\''|export PS1='\''\\w# '\''|g' ${TARGET_DIR}/etc/profile
#export PS1='\w$ '
sed -i -e 's|export PS1='\''$ '\''|export PS1='\''\\w$ '\''|g' ${TARGET_DIR}/etc/profile

#allow root login
rm -f ${TARGET_DIR}/etc/securetty
cp ${AMBA_CUST_DIR}/etc/inittab ${TARGET_DIR}/etc/
if [ "${BR2_AMBARELLA_LINUX_RESPAWN_SH}" != "y" ]; then
	sed -i -e 's|::respawn:-/bin/sh|#::respawn:-/bin/sh|g' ${TARGET_DIR}/etc/inittab
fi
if [ "${BR2_AMBARELLA_LINUX_TTY}" != "\"\"" ]; then
	AMBARELLA_LINUX_TTY=`echo ${BR2_AMBARELLA_LINUX_TTY}|sed -e 's/"//g'`
	sed -i -e 's|::respawn:-/bin/sh|'${AMBARELLA_LINUX_TTY}'::respawn:-/bin/sh|g' ${TARGET_DIR}/etc/inittab
	sed -i -e 's|::sysinit:/etc/init.d/rcS|'${AMBARELLA_LINUX_TTY}'::sysinit:/etc/init.d/rcS|g' ${TARGET_DIR}/etc/inittab
fi

rm -fv ${TARGET_DIR}/usr/bin/amba_svc
rm -fv ${TARGET_DIR}/etc/init.d/S50_amba_camera_service
rm -fv ${TARGET_DIR}/etc/init.d//etc/init.d/S99bootdone
cp ${AMBA_CUST_DIR}/etc/init.d/S99cluster ${TARGET_DIR}/etc/init.d/
