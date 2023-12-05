#!/bin/bash

ROOT_DIR=${PWD}
PROJECT_NAME=${1}
CUSTOMER=nexar

USB_FIRMWARE_PATH=("${PWD}/../rtos/cortex_a/output.64/out/fwprog/bst_bld_pba_atf_sys_xtb_sec_rom_lnx_rfs.elf")
USB_UPGRADE_NAME=("bst_bld_pba_atf_sys_xtb_sec_rom_lnx_rfs.elf")
SD_FIRMWARE_PATH=("${PWD}/../rtos/cortex_a/output.64/out/fwprog/devfw/sys_sec_rom_lnx_rfs_dtb_xtb.devfw")
SD_UPGRADE_NAME=("nexar.bin")

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
PLATFORM_NAME=amba_${CHIP}

version_num="`cat ${ROOT_DIR}/../rtos/cortex_a/svc/apps/icam/cardv/tmp_version.txt`"

RELEASE_ROOT_DIR=${ROOT_DIR}/../fw_bin

FW_RELEASE_DIRS=("dev")

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
	rm -rf output output.64
	make ${CHIP}/icam/tx32/${RTOS_CONFIG}
	make diags
	make -j8
	if [  $? != 0 ]; then
		echo "rtos build fail"
		exit 0
	fi
}

build_ambalink

# gen release dir
DATETIME=$(date +%Y%m%d_%H%M)
VERSION_DIR_NAME="${PLATFORM_NAME}_${CUSTOMER}_${PROJECT_NAME}_evt_${version_num}_${DATETIME}"
export VERSION_DIR_PATH="${RELEASE_ROOT_DIR}/${VERSION_DIR_NAME}"
mkdir -p "${VERSION_DIR_PATH}"

d_length=${#FW_RELEASE_DIRS[@]}
for ((d_index = 0; d_index < ${d_length}; d_index++)); do
	fw_type="${FW_RELEASE_DIRS[$d_index]}"
	build_rtos
	cd "${VERSION_DIR_PATH}"
	# copy usb, sd firmware to release dir and cal md5
	if [ "${#USB_FIRMWARE_PATH[@]}" != "0" ] && [ "${#USB_UPGRADE_NAME[@]}" != "0" ]; then
		length=${#USB_FIRMWARE_PATH[@]}
		tmp_fw_path=${fw_type}_fw/usb
		mkdir -p ${tmp_fw_path}
		for ((index = 0; index < ${length}; index++)); do
			ITEM_PATH=${USB_FIRMWARE_PATH[$index]}
			ITEM_NAME=${USB_UPGRADE_NAME[$index]}
			cp -f "${ITEM_PATH}" "${tmp_fw_path}/${ITEM_NAME}"
			md5sum "${tmp_fw_path}/${ITEM_NAME}" > "${tmp_fw_path}/${ITEM_NAME}.md5sum"
		done
	fi
	if [ "${#SD_FIRMWARE_PATH[@]}" != "0" ] && [ "${#SD_UPGRADE_NAME[@]}" != "0" ]; then
		length=${#SD_FIRMWARE_PATH[@]}
		tmp_fw_path=${fw_type}_fw/sd
		mkdir -p ${tmp_fw_path}
		for ((index = 0; index < ${length}; index++)); do
			ITEM_PATH=${SD_FIRMWARE_PATH[$index]}
			ITEM_NAME=${SD_UPGRADE_NAME[$index]}
			cp -f "${ITEM_PATH}" "${tmp_fw_path}/${ITEM_NAME}"
			md5sum "${tmp_fw_path}/${ITEM_NAME}" > "${tmp_fw_path}/${ITEM_NAME}.md5sum"
			cp -f "${ITEM_PATH}" "${RTOS_PATH}/${ITEM_NAME}"
		done
	fi

	#generate sd full image
	sd_full_path="${fw_type}_fw/sd/nexar_full.bin"
	echo -n "FULL" > ${sd_full_path}
	let BLD_SIZE=`stat -c %s "${RTOS_PATH}/output.64/out/fwprog/devfw/bst_bld_pba.devfw"`
	echo `printf "%.2X%.2X%.2X%.2X" $((BLD_SIZE & 0xff)) $((BLD_SIZE >> 8 & 0xff)) $((BLD_SIZE >> 16 & 0xff)) $((BLD_SIZE >> 24 & 0xff)) ` | xxd -r -ps >> ${sd_full_path}
	cat "${RTOS_PATH}/output.64/out/fwprog/devfw/bst_bld_pba.devfw" >> ${sd_full_path}
	cat "${RTOS_PATH}/output.64/out/fwprog/devfw/sys_sec_rom_lnx_rfs_dtb_xtb.devfw" >> ${sd_full_path}
	md5sum "${sd_full_path}" > "${sd_full_path}.md5sum"
done

# gen release info
echo "***************************************************************" > release_info.txt
echo "* This file is auto generated. Please don't edit by yourself! *" >> release_info.txt
echo "***************************************************************" >> release_info.txt
echo "       build date: "`date` >> release_info.txt
echo "    From computer: "`whoami`@`hostname` >> release_info.txt
echo "  From git branch: "`git branch | awk -F' ' '{print $2}'` >> release_info.txt
echo "       git remote: "`git remote -v | head -1 | awk -F' ' '{print $2}'` >> release_info.txt
echo "     git revision: "`git rev-parse HEAD | cut -b -10` >> release_info.txt

echo "[ ${PLATFORM_NAME} ${PROJECT_NAME} for ${CUSTOMER} Project Build Success!!! ]"

