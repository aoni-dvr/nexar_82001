#!/bin/bash

CUSTOMER=nexar
PROJECT_NAME=${1}
ROOT_DIR=${PWD}
RTOS_PATH=${ROOT_DIR}/../rtos/cortex_a
CHIP=""
if [ "${PROJECT_NAME}" == "d081" ]; then
CHIP=h32
RTOS_CONFIG=h32_nexar_d081_icam_spinand_ps5256_defconfig
AMBALINK_CONFIG=nexar_h32_d081_spinand_defconfig
elif [ "${PROJECT_NAME}" == "d080" ]; then
CHIP=cv25
RTOS_CONFIG=nexar_d080_cv_nand_gc4653_gc2053_ambalink_defconfig
AMBALINK_CONFIG=nexar_cv25_d080_nand_defconfig
else
echo "invalid project name. Usage: $0 d080|d081"
exit 0
fi

need_clean=0
if [ "${1}" = "auto_copy" ] || [ "${1}" = "sdk_release" ]; then
	need_clean=1
fi

build_ambalink()
{
	cd ${RTOS_PATH}/
	source build/maintenance/env/env_set.sh ${CHIP} /opt/cvtools/
	cd -
	cp -f platform_scripts/${PROJECT_NAME}/bt.conf pkg/network_turnkey/source/usr/local/share/script/bt.conf
	cp -f platform_scripts/${PROJECT_NAME}/wifi_start.sh pkg/network_turnkey/source/usr/local/share/script/wifi_start.sh
	cp -f platform_scripts/${PROJECT_NAME}/wifi_sdio_id.h linux-5.15/drivers/mmc/host/wifi_sdio_id.h
	cp -f platform_scripts/${PROJECT_NAME}/inittab ambarella/package/ambarella_customize/source/etc/inittab
	cd ambarella/
	make AMBA_OUT_TARGET=ambalink TARGET=ambalink prepare_oem
	make O=../output.oem/ambalink ${AMBALINK_CONFIG}
	cd ../output.oem/ambalink/
	cd build
	rm -rf network_turnkey-*
	rm -rf amba_examples-*
	rm -rf ipcdef-*
	rm -rf bcmdhd-* ../target/usr/local/bcmdhd
	rm -rf cypress-* ../target/usr/local/cypress
	rm -rf wpa_supplicant-* hostapd-* rtl8188fu-* rtl8821au-*
	rm -rf rtl8188fu-*
	rm -rf bluez5_utils-*
	#rm -rf busybox-*
	#rm -rf tzdata-*
	rm -rf http_control_server-*
	rm -rf iot_device_sdk_app-*
	rm -rf ambacv-*
	rm -rf qconnect_manager-*
	##rm -rf webrtc-*
	#rm -rf openssh-*
	#rm -rf iot_device_sdk_app-*
	#rm -rf aws-iot-device-sdk-embedded-c-*
	cd ..
	make BR2_WGET="wget --passive-ftp -nd -t 3 --no-check-certificate" -j8

	if [ $? != 0 ]; then
		echo "ambalink build fail"
		exit 0
	fi
	linux_image_dir=${RTOS_PATH}/svc/comsvc/ambalink/linux_image/${CHIP}/${CUSTOMER}_${PROJECT_NAME}/
	mkdir -p ${linux_image_dir}
	DTB_NAME=h32mbub_ambalink.dtb
	if [ "$CHIP" == "cv25" ]; then
		DTB_NAME=cv25bub_ambalink.dtb
	fi
	cp -f ${ROOT_DIR}/output.oem/ambalink/images/${DTB_NAME} ${linux_image_dir}
	cp -f ${ROOT_DIR}/output.oem/ambalink/images/Image ${linux_image_dir}
	cp -f ${ROOT_DIR}/output.oem/ambalink/images/rootfs.squashfs ${linux_image_dir}
}

build_rtos()
{
	cd ${RTOS_PATH}/
	source build/maintenance/env/env_set.sh ${CHIP} /opt/cvtools/
	if [ "${need_clean}" = "1" ]; then
		rm -rf output output.64
	fi
	make ${CHIP}/icam/tx32/${RTOS_CONFIG}
	make diags
	make -j8
	if [  $? != 0 ]; then
		echo "rtos build fail"
		exit 0
	fi
}

build_ambalink
build_rtos
