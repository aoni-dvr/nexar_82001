#!/bin/bash

# ---- Functions declaration ---- #

BLD_PATH=${objtree}/boot/${AMBA_CHIP_ID}/secure/bootloader

cleanup()
{
	echo "cleanup Bootloader"

	if [ -s ${KBUILD_SRC}/${O}/${KBUILD_AMBA_OUT_DIR}/ ]; then
		find ${KBUILD_SRC}/${O}/${KBUILD_AMBA_OUT_DIR}/ \
			\( \
			   -name "${__LINK_FILE_STEM}*.*" \
			\) -type f -print | xargs rm -f
	fi
}

extract_symbol()
{
	${NM} $1 | grep $2 | awk -F" " '{ print $1 }'
}

extract_linker_stub_length()
{
	grep "linker stubs" $1 | head -1 | awk '{if($4 == "linker"){print $3}else{print $2}}'
}

extract_linker_stub()
{
	grep "linker stubs" $1 | head -1 | awk '{if($4 == "linker"){print $2}else{print $1}}'
}
show_variables()
{
	echo "================================================="
	echo "---- show_variables() ----"
	echo ""
	echo "cmd_link-amba_bld: ${0} ${1} ${2} ${3} ${4}"
	echo "arg0=${0} arg1=${1} arg2=${2} arg3=${3} arg4=${4}"
	echo ""
	echo "---- Global variable ----"
	echo "LD=${LD}"
	echo "LDS_amba_bld=${LDS_amba_bld}"
	echo "LDFLAGS=${LDFLAGS}"
	echo "LDFLAGS_amba_bld=${LDFLAGS_amba_bld}"
	echo "srctree=${srctree}"
	echo "objtree=${objtree}"
	echo "DIR_amba_bld=${DIR_amba_bld}"
	echo ""
	echo "---- Local variable ----"
	echo "LINK_OBJS=${LINK_OBJS}"
	echo "LINK_LIBS=${LINK_LIBS}"
	echo "LINK_LDS=${LINK_LDS}"
	echo "LINK_LDFLAGS=${LINK_LDFLAGS}"
	echo ""
	echo "LINK_OUTPUT_ELF=${LINK_OUTPUT_ELF}"
	echo "LINK_OUTPUT_BIN=${LINK_OUTPUT_BIN}"
	echo "LINK_OUTPUT_MAP=${LINK_OUTPUT_MAP}"
	echo "LINK_OUTPUT_NM=${LINK_OUTPUT_NM}"
	echo ""
	echo "LINK_CMD=${LINK_CMD}"
	echo "NM_CMD=${NM_CMD}"
	echo "OBJCOPY_CMD=${OBJCOPY_CMD}"
	echo ""
	echo "================================================="
}

run_ld_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  LD      ${LINK_OUTPUT_ELF}"
		echo "  LD_MAP  ${LINK_OUTPUT_MAP}"
	fi

	${LINK_CMD}
}

run_nm_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  NM      ${LINK_OUTPUT_NM}"
	fi

	${NM_CMD} > ${LINK_OUTPUT_NM}
}

run_objcopy_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  OBJCOPY ${LINK_OUTPUT_BIN}"
	fi

	${OBJCOPY_CMD}
#	if [ "${CONFIG_ENABLE_SECURITY}" = "y" ] && [ "x${CONFIG_ATF_HAVE_BL2}" != "xy" ]; then
#		truncate --size %256 ${LINK_OUTPUT_BIN_NOSIG}
#	else
		truncate --size %256 ${LINK_OUTPUT_BIN}
#	fi
}

run_objdump_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  OBJDUMP ${LINK_OUTPUT_ELF}"
	fi

        ${OBJDUMP} -d ${LINK_OUTPUT_ELF} > ${LINK_OUTPUT_SYM}
}

rpc_crc_calc()
{
	${RPC_CRC_CMD}
	${BUILD_RPC_CRC_CMD}
}

# ---- Main start of script ---- #

# Common for link shell script

# Error out on error
set -e

# Delete output files in case of error
trap cleanup SIGHUP SIGINT SIGQUIT SIGTERM ERR

# Use "make V=1" to debug this script
case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

__LINK_FILE_STEM="amba_bld"

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

AMBA_LIB64_FOLDER=${srctree}/vendors/ambarella/lib64
mkdir -p ${AMBA_LIB64_FOLDER}

# ---- Definition for link target dependent ---- #
BOOT_TYPE=`grep "CONFIG_ENABLE*" ${KCONFIG_CONFIG} | grep "=y" | grep "BOOT" | awk -F "_" '{printf $3}'`

chk_lib()
{
	if [ -s ${AMBA_O_LIB}/$1 ]; then
		echo ${AMBA_O_LIB}/$1
	elif [ -s ${AMBA_LIB_FOLDER}/$1 ]; then
		echo ${AMBA_LIB_FOLDER}/$1
	elif [ -s ${AMBA_LIB64_FOLDER}/$1 ]; then
		echo ${AMBA_LIB64_FOLDER}/$1
	else
		echo "$1 NOT found"
		exit 1
	fi
}

BLD_LIBS=" \
		  `chk_lib libio_common.a`  \
		  `chk_lib libwrap_std.a`   \
		  -Wl,--whole-archive  `chk_lib libbsp_${AMBA_BSP_NAME}.a` -Wl,--no-whole-archive "

BLD_LIBS="${BLD_LIBS} `chk_lib libio_${AMBA_CHIP_ID}.a`"

if [ "${BOOT_TYPE}" == "EMMC" ]; then
BLD_LIBS="${BLD_LIBS} `chk_lib libbld_emmc.a`"
elif [ "${BOOT_TYPE}" == "SPINOR" ]; then
BLD_LIBS="${BLD_LIBS} `chk_lib libbld_spinor.a`"
else
BLD_LIBS="${BLD_LIBS} `chk_lib libbld_nand.a`"
fi

if [ "x${CONFIG_BUILD_FOSS_LIBFDT}" != "x" ]; then
	BLD_LIBS="${BLD_LIBS} `chk_lib libfdt.a`"
	if [ "${CONFIG_CC_USESTD}" == "" ]; then
		BLD_LIBS="${BLD_LIBS} `chk_lib libwrapc.a`"
	fi
fi

if [ "${CONFIG_BUILD_FOSS_MBEDTLS}" = "y" ]; then
    BLD_LIBS="${BLD_LIBS} `chk_lib libmbedtls.a`"
fi

if [ "${CONFIG_FASTBOOT_LZ4}" = "y" ]; then
    BLD_LIBS="${BLD_LIBS} `chk_lib libfoss_lz4.a`"
fi

if [ "x${CONFIG_BUILD_QNX_IPL}" != "x" ] && [ "x${CONFIG_ATF_HAVE_BL2}" != "x" ]; then
    BLD_LIBS+=" ${srctree}/vendors/qnx/ipl/boards/${AMBA_CHIP_ID}/aarch64/a.le/libipl-${AMBA_CHIP_ID}.a "
    BLD_LIBS+=" ${srctree}/vendors/qnx/ipl/lib/aarch64/a.le/libipl.a "
fi

BLD_LIBS=$(echo ${BLD_LIBS} | tr -s [:space:])

__LINK_C_LIBS=""

# It's necessary to do relocation by using libraries of Linux toolchain,
# so we use libraries from bare-metal.
if [ "x${CONFIG_LINUX}" != "x" ]; then
	__LINK_C_LIBS+="${srctree}/vendors/ambarella/wrapper/std/bin/64b/orig/libc.a.bin "
	__LINK_C_LIBS+="${srctree}/vendors/ambarella/wrapper/std/bin/64b/orig/libm.a.bin "
	__LINK_C_LIBS+="${srctree}/vendors/ambarella/wrapper/std/bin/64b/orig/libgcc.a.bin "
fi


# ld serachdir: -L option

BSP_DIR="bsp"

LINK_LIBS="-nostartfiles ${BLD_LIBS} ${__LINK_C_LIBS}"
LINK_LDS="${AMBA_BLD_LDS}"

LOCAL_LDFLAGS="-L${AMBA_O_LIB} -L${AMBA_LIB_FOLDER} -Wl,--gc-sections "

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=${DIR_amba_bld}
fi

LINK_OUTPUT_ELF="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.elf"
LINK_OUTPUT_BIN="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.bin"
LINK_OUTPUT_MAP="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.map"
LINK_OUTPUT_NM="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.nm"
LINK_OUTPUT_SYM="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}.sym"

#LINK_OUTPUT_BIN_NOSIG="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}_nosig.bin"
#LINK_OUTPUT_BIN_SIG="${LINK_AMBA_OUT_DIR}/${__LINK_FILE_STEM}_nosig.bin.sig"

#  SYS_OUTPUT= rtos/output ---- It is 32 bit output file folder. #
#SYS_OUTPUT="${srctree}/output"

LOCAL_CFLAGS="`echo ${KBUILD_CFLAGS} | sed 's/\"//g'`"

BUILD_CMD="${CC} ${LOCAL_CFLAGS} -c ${REGION_INFO_FILE}.c -o ${REGION_INFO_FILE}.o"
LINK_CMD="${CC} ${LOCAL_LDFLAGS} -Wl,-T${LINK_LDS} -Wl,--start-group ${LINK_LIBS} ${LINK_OBJS} -Wl,--end-group -Wl,--build-id=none -Wl,-Map=${LINK_OUTPUT_MAP} -o ${LINK_OUTPUT_ELF} -Wl,--cref"
if [ "x${CONFIG_LINUX}" != "x" ]; then
	LINK_CMD="${LINK_CMD} -static -Wl,-static"
fi
LINK_CMD="${LINK_CMD} -Wl,--build-id=none"
if [ "x${AMBA_CAMERA_DIR}" != "x" ]; then
    # From buildroot, do not use libc from toolchain.
    LINK_CMD="${LINK_CMD} -nolibc -nostdlib"
fi
NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"

#if [ "${CONFIG_ENABLE_SECURITY}" = "y" ] && [ "x${CONFIG_ATF_HAVE_BL2}" != "xy" ]; then
## secure boot firmware
#OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN_NOSIG}"
#COPY_CMD="cp ${LINK_OUTPUT_BIN_NOSIG} ${LINK_OUTPUT_BIN}"
#if [ "${CONFIG_SOC_CV5}" = "y" ] || [ "${CONFIG_SOC_CV52}" = "y" ]; then
#    # cv5x secure boot firmware
#    SIGN_CMD="${CONFIG_OPENSSL_PATH} pkeyutl -sign -inkey ${CONFIG_ECC_PRIVATE_KEY_PATH} -out ${LINK_OUTPUT_BIN_SIG} -rawin -in ${LINK_OUTPUT_BIN}"
#    UST_HEADER_CMD="python ${srctree}/tools/hostutils/ust_header_modify.py ${LINK_OUTPUT_BIN_NOSIG} 0 0 0"
#else
#    # cv2x/cv2xfs secure boot firmware
#    SIGN_CMD="/usr/bin/openssl dgst -sha256 -sign ${CONFIG_RSA_PRIVATE_KEY_PATH} -out ${LINK_OUTPUT_BIN_SIG} ${LINK_OUTPUT_BIN}"
#fi
#else
OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"
#fi

#SYS_BIN=${CONFIG_FWPROG_SYS}
#SYS_ELF=`echo ${SYS_BIN} | sed -e 's/.bin/.elf/'`

rm -f ${REGION_INFO_FILE}.list
echo -e "  GEN     ${__REGION_INFO_FILE__}.list"

# We can find the sys.elf in now 64 bit working directory. We need to switch to 32 bit output folder. #
#if [ ! -e ${SYS_ELF} ]; then
#    cd ${SYS_OUTPUT}
#fi


# We need to switch to 64 bit output folder for below bst and bld output. #
cd ${objtree}

# ---- Run function ---- #
#show_variables
#echo ${BUILD_CMD}
run_ld_cmd
run_nm_cmd
run_objcopy_cmd
run_objdump_cmd

#if [ "${CONFIG_ENABLE_SECURITY}" = "y" ] && [ "x${CONFIG_ATF_HAVE_BL2}" != "xy" ]; then
#	# secure boot firmware
#    if [ "${CONFIG_SOC_CV5}" = "y" ] || [ "${CONFIG_SOC_CV52}" = "y" ]; then
#        # need to update UST header information before signing
#        echo ${UST_HEADER_CMD}
#        ${UST_HEADER_CMD}
#    fi
#	${COPY_CMD}
#	${SIGN_CMD}
#	# append RSA signature to bld file
#	cat ${LINK_OUTPUT_BIN_SIG} >> ${LINK_OUTPUT_BIN}
#fi

echo "$0 Done..."


