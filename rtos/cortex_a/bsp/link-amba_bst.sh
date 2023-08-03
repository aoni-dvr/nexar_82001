#!/bin/bash

# Error out on error
set -e

BST_PATH=${objtree}/boot/${AMBA_CHIP_ID}/secure/bootstrap

# Delete output files in case of error
trap cleanup SIGHUP SIGINT SIGQUIT SIGTERM ERR
cleanup()
{
	echo "cleanup Bootstrap"

	rm -f ${BST_PATH}/.*.cmd
	rm -f ${BST_PATH}/*ddr*
	rm -f ${BST_PATH}/amba_bst.lds
    rm -f ${KBUILD_SRC}/${O}/${KBUILD_AMBA_OUT_DIR}/amba_bst*.*
}

#
# Use "make V=1" to debug this script
#
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

if [ "$1" = "clean" ]; then
	cleanup
	exit 0
fi

# We need access to CONFIG_ symbols
case "${KCONFIG_CONFIG}" in
*/*)
	. "${KCONFIG_CONFIG}"
	;;
*)
	# Force using a file from the current directory
	. "./${KCONFIG_CONFIG}"
esac

SKIP_BUILD_BST_IMG=n
if [ "${CONFIG_ARM32}" = "y" ]; then
	if [ "${CONFIG_BUILD_32BIT_BOOT_CODE}" != "y" ]; then
		# Do not build 32-bit firmware programmer by default
		SKIP_BUILD_BST_IMG=y
	fi
fi

AMBA_LIB64_FOLDER=${srctree}/vendors/ambarella/lib64
mkdir -p ${AMBA_LIB64_FOLDER}

if [ "${SKIP_BUILD_BST_IMG}" != "y" ]; then

BOOT_DRAM_TYPE=`grep "CONFIG_DRAM_TYPE*" ${KCONFIG_CONFIG} | grep "=y" | awk -F "_" '{printf $4}' | awk -F "=" '{printf $1}'`
BOOT_FLASH_TYPE=`grep -E "CONFIG_ENABLE_[A-Z]+[A-Z]*_BOOT=y" ${KCONFIG_CONFIG} | awk -F "_" '{printf $3}'`

# ---- Definition for link target dependent ---- #
chk_lib()
{
	if [ -s ${AMBA_O_LIB}/$1 ]; then
		echo ${AMBA_O_LIB}/$1
	elif [ -s ${AMBA_LIB_FOLDER}/$1 ]; then
		echo ${AMBA_LIB_FOLDER}/$1
	elif [ -s ${AMBA_LIB64_FOLDER}/$1 ]; then
		echo ${AMBA_LIB64_FOLDER}/$1
	else
		echo "Can not found $1"
		exit 1
	fi
}

if [ "${BOOT_DRAM_TYPE}" == "LPDDR5" ]; then
    BST_LIB_PREFIX="libbst_lpddr5"
elif [ "${BOOT_DRAM_TYPE}" == "DDR4" ]; then
    BST_LIB_PREFIX="libbst_ddr4"
elif [ "${BOOT_DRAM_TYPE}" == "LPDDR4" ]; then
    BST_LIB_PREFIX="libbst_lpddr4"
else
    echo "Unknown BOOT_DRAM_TYPE \"${BOOT_DRAM_TYPE}\""
fi

if [ "${BOOT_FLASH_TYPE}" == "EMMC" ]; then
    BST_LIB="${BST_LIB_PREFIX}_emmc.a"
elif [ "${BOOT_FLASH_TYPE}" == "NAND" ]; then
    BST_LIB="${BST_LIB_PREFIX}_nand.a"
elif [ "${BOOT_FLASH_TYPE}" == "SPINOR" ]; then
    BST_LIB="${BST_LIB_PREFIX}_spinor.a"
elif [ "${BOOT_FLASH_TYPE}" == "SPINAND" ]; then
    BST_LIB="${BST_LIB_PREFIX}_spinand.a"
else
    echo "Unknown BOOT_FLASH_TYPE \"${BOOT_FLASH_TYPE}\""
fi

LIBS="`chk_lib ${BST_LIB}` \
      `chk_lib libbsp_${AMBA_BSP_NAME}.a`"

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=${DIR_amba_bst}
fi

if [ "${BOOT_DRAM_TYPE}" == "LPDDR4" ]; then
    OUTPUT=${LINK_AMBA_OUT_DIR}/amba_bst_lpddr4.elf
	MAP=${LINK_AMBA_OUT_DIR}/amba_bst_lpddr4.map
	OUTPUT_BIN=${LINK_AMBA_OUT_DIR}/amba_bst.bin
	OUTPUT_BIN_NOSIG="${LINK_AMBA_OUT_DIR}/amba_bst_lpddr4_nosig.bin"
	OUTPUT_BIN_SIG="${LINK_AMBA_OUT_DIR}/amba_bst_lpddr4_nosig.bin.sig"
elif [ "${BOOT_DRAM_TYPE}" == "DDR4" ]; then
    OUTPUT=${LINK_AMBA_OUT_DIR}/amba_bst_ddr4.elf
	MAP=${LINK_AMBA_OUT_DIR}/amba_bst_lpddr4.map
	OUTPUT_BIN=${LINK_AMBA_OUT_DIR}/amba_bst.bin
	OUTPUT_BIN_NOSIG="${LINK_AMBA_OUT_DIR}/amba_bst_ddr4_nosig.bin"
	OUTPUT_BIN_SIG="${LINK_AMBA_OUT_DIR}/amba_bst_ddr4_nosig.bin.sig"
elif [ "${BOOT_DRAM_TYPE}" == "LPDDR5" ]; then
    OUTPUT=${LINK_AMBA_OUT_DIR}/amba_bst_lpddr5.elf
	MAP=${LINK_AMBA_OUT_DIR}/amba_bst_lpddr5.map
	OUTPUT_BIN=${LINK_AMBA_OUT_DIR}/amba_bst.bin
	OUTPUT_BIN_NOSIG="${LINK_AMBA_OUT_DIR}/amba_bst_lpddr5_nosig.bin"
	OUTPUT_BIN_SIG="${LINK_AMBA_OUT_DIR}/amba_bst_lpddr5_nosig.bin.sig"
else
    echo "Unknown BOOT_DRAM_TYPE \"${BOOT_DRAM_TYPE}\""
fi

LOCAL_LDFLAGS="-nostartfiles -L${AMBA_O_LIB} -L${AMBA_LIB_FOLDER}"

BUILD_CMD="${CC} ${LOCAL_LDFLAGS} -Wl,-T${AMBA_BST_LDS} -Wl,--start-group ${LIBS} -Wl,--end-group -Wl,-Map=${MAP} -o ${OUTPUT}"
BUILD_CMD="${BUILD_CMD} -Wl,--build-id=none"
if [ "x${AMBA_CAMERA_DIR}" != "x" ]; then
    # From buildroot, do not use libc from toolchain.
    BUILD_CMD="${BUILD_CMD} -nolibc -nostdlib -static"
fi

#if [ "${CONFIG_ENABLE_SECURITY}" = "y" ]; then
#    # secure boot firmware
#    OBJCOPY_CMD="${OBJCOPY} -j .bst_stage1 -j .bst_stage2 -j .data -S -g -O binary ${OUTPUT} ${OUTPUT_BIN_NOSIG}"
#    COPY_CMD="cp -f ${OUTPUT_BIN_NOSIG} ${OUTPUT_BIN}"
#    if [ "${CONFIG_SOC_CV5}" = "y" ] || [ "${CONFIG_SOC_CV52}" = "y" ]; then
#        # cv5x secure boot firmware
#        SIGN_CMD="${CONFIG_OPENSSL_PATH} pkeyutl -sign -inkey ${CONFIG_ECC_PRIVATE_KEY_PATH} -out ${OUTPUT_BIN_SIG} -rawin -in ${OUTPUT_BIN}"
#        UST_HEADER_CMD="python ${srctree}/tools/hostutils/ust_header_modify.py ${OUTPUT_BIN_NOSIG} 0 0 0"
#    else
#        # cv2x/cv2xfs secure boot firmware
#        SIGN_CMD="/usr/bin/openssl dgst -sha256 -sign ${CONFIG_RSA_PRIVATE_KEY_PATH} -out ${OUTPUT_BIN_SIG} ${OUTPUT_BIN}"
#    fi
#else
    if [ "${CONFIG_SOC_CV5}" = "y" ] || [ "${CONFIG_SOC_CV52}" = "y" ] && [ "${CONFIG_BST_LARGE_SIZE}" = "y" ]; then
        OBJCOPY_CMD="${OBJCOPY} -j .bst_stage1 -j .bst_stage2 -j .data -j .bst_training -S -g -O binary ${OUTPUT} ${OUTPUT_BIN}"
    else
        OBJCOPY_CMD="${OBJCOPY} -j .bst_stage1 -j .bst_stage2 -j .data -S -g -O binary ${OUTPUT} ${OUTPUT_BIN}"
    fi
COPY_CMD="cp ${BST_PATH}/*.bin ${OUTPUT_BIN}"
#fi


#echo "================================================="
#echo ""
#echo "LD=${LD}"
#echo "LOCAL_LDFLAGS=${LOCAL_LDFLAGS}"
#echo "srctree=${srctree}"
#echo "objtree=${objtree}"
#echo "LINK_LIBS=${LIBS}"
#echo ""
#echo "lds=${LDS_amba_bst}"
#echo "LINK_OUTPUT=${OUTPUT}"
#echo "BUILD_CMD=${BUILD_CMD}"
#echo "OBJCOPY_CMD=${OBJCOPY_CMD}"
#echo ""
#echo "KCONFIG=${KCONFIG_CONFIG}"
#echo "BOOT_DRAM_TYPE=${BOOT_DRAM_TYPE}"
#echo "OUTPUT_BIN_NOSIG=${OUTPUT_BIN_NOSIG}"
#echo "OUTPUT_BIN=${OUTPUT_BIN}"
#echo "================================================="

${BUILD_CMD}
${OBJCOPY_CMD}
#${COPY_CMD}

#if [ "${CONFIG_ENABLE_SECURITY}" = "y" ]; then
#    # secure boot firmware
#    if [ "${CONFIG_SOC_CV5}" = "y" ] || [ "${CONFIG_SOC_CV52}" = "y" ]; then
#        # need to update UST header information before signing
#        echo ${UST_HEADER_CMD}
#        ${UST_HEADER_CMD}
#    fi
#    ${COPY_CMD}
#    ${SIGN_CMD}
#    # append RSA signature to bst file
#    cat ${OUTPUT_BIN_SIG} >> ${OUTPUT_BIN}
#fi

fi # if [ "${SKIP_BUILD_BST_IMG}" != "y" ]; then

