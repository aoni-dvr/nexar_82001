#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

AMBA_LIBS="
libbsp_${AMBA_BSP_NAME}.a
libsvc_app.a
libfs_${AMBA_CHIP_ID}.a
libperif_${AMBA_CHIP_ID}.a
"

LDP=""
if [ "${CONFIG_LINUX}" != "" ]; then
	LDP="-Wl,"
	#libio_common.a is non-os AmBoot, fwprog only, not used in linux amba_svc
	#libfdt.a is not used in linux amba_svc
	AMBA_LIBS="${AMBA_LIBS} libprint.a libwrap_std.a libmisc.a libshell.a"
fi

if [ "${CONFIG_THREADX}" != "" ]; then
	AMBA_LIBS="${AMBA_LIBS} \
	libio_common.a \
	libprfile.a \
	libcomsvc.a \
	libusb_uvcd.a \
	libthreadX_${CONFIG_KAL_THREADX_SMP_NUM_CORES}core.a \
	"
	if [ "${CONFIG_BUILD_FOSS_LIBFDT}" == "y" ]; then
		AMBA_LIBS="${AMBA_LIBS} libfdt.a"
	fi

	if [ "${CONFIG_ENABLE_AMBALINK}" == "y" ] || [ "${CONFIG_OPENAMP}" == "y" ] || [ "${CONFIG_BUILD_FOSS_LIBFDT}" == "y" ]; then
		AMBA_LIBS="${AMBA_LIBS} libwrapc.a"
	fi
fi

AMBA_LIBS="${AMBA_LIBS} libio_${AMBA_CHIP_ID}.a"

AMBA_LIBS="${AMBA_LIBS} libeng_perif_${AMBA_CHIP_ID}.a"

if [ "${CONFIG_BUILD_SSP_USB_LIBRARY}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libusb_cdcacmd.a libusb_mscd.a libusb_msch.a libusb_mtpd.a libusb_sys.a"
fi

if [ "${CONFIG_BUILD_SSP_USBX_LIBRARY}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libusbx_sys.a libusbx_device.a libusbx_host.a libusbx_cdcacmd.a libusbx_mtpd.a libusbx_mscd.a libusbx_msch.a libusbx_hostcontroller.a"
fi

if [ "${CONFIG_BUILD_SSP_NETX}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libnetx.a"
	if [ "${CONFIG_NETX_TELNET}" = "y" ]; then
		AMBA_LIBS="${AMBA_LIBS} libnetx_telnet.a"
	fi
fi
if [ "${CONFIG_BUILD_SSP_ENET}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libplat_enet.a libnetstack.a"
	if [ "${CONFIG_BUILD_COMMON_SERVICE_NET}" = "y" ]; then
		AMBA_LIBS="${AMBA_LIBS} libenet_refcode.a"
		if [ "${CONFIG_LWIP_ENET}" = "y" ]; then
			AMBA_LIBS="${AMBA_LIBS} libenet_reflwip.a"
		fi
	fi
fi
if [ "${CONFIG_BUILD_FOSS_LWIP}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} liblwip.a"
fi

if [ "${CONFIG_BUILD_GPL_LIB}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libgpl.a"
fi

if [ "${CONFIG_BUILD_SSP_KAL}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libkal.a"
fi

if [ "${CONFIG_BUILD_MAL}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libmal.a"
fi

if [ "${CONFIG_BUILD_COMSVC_DRAMSHMOO}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libdramshmoo.a"
fi

if [ "${CONFIG_ENABLE_AMBALINK}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libambalink.a"
fi

if [ "${CONFIG_OPENAMP}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libopen_amp.a libmetal.a"
fi

if [ "${CONFIG_XEN_SUPPORT}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libxensupport.a"
fi

if [ "${CONFIG_BUILD_SSP_PCIE_LIB}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libpcie.a libnvme.a"
fi

AMBA_LIBS=$(echo ${AMBA_LIBS} | tr " " "\n" | uniq | tr "\n" " ")
AMBA_LIBS_NUM=$(echo ${AMBA_LIBS} | wc -w)

# Search lib path
__LINK_AMBA_LIBS="${LDP}--whole-archive"
if [ "${CONFIG_ARM64}" = "y" ]; then
	for i in ${AMBA_LIBS}; do
		if [ -s ${AMBA_O_LIB}/${i} ]; then
			__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_O_LIB}/${i}"
		elif [ -s ${AMBA_LIB64_FOLDER}/${i} ]; then
			__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB64_FOLDER}/${i}"
		elif [ -s ${AMBA_LIB64_FOLDER}/${AMBA_CHIP_ID}/threadx/${i} ]; then
			__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB64_FOLDER}/${AMBA_CHIP_ID}/threadx/${i}"
		else
			echo "Can not found ${i}"
			exit 1
		fi
	done
else
	for i in ${AMBA_LIBS}; do
		if [ -s ${AMBA_O_LIB}/${i} ]; then
			__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_O_LIB}/${i}"
		elif [ -s ${AMBA_LIB_FOLDER}/${i} ]; then
			__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB_FOLDER}/${i}"
		elif [ -s ${AMBA_LIB_FOLDER}/${AMBA_CHIP_ID}/${i} ]; then
			__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB_FOLDER}/${AMBA_CHIP_ID}/${i}"
		else
			echo "Can not found ${i}"
			exit 1
		fi
	done
fi

__LINK_AMBA_LIBS=$(echo ${__LINK_AMBA_LIBS} | tr -s [:space:])
if [ "x${CONFIG_CC_LTO}" != "x" ]; then
	__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} -lm -lgcc -lc ${LDP}--no-whole-archive "
	__LINK_C_LIBS=" "
else
	if [ "${CONFIG_LINUX}" == "" ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} --no-whole-archive "
		__LINK_C_LIBS="-lwrap_std -lm -lgcc -lc "
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${__LINK_C_LIBS}"
	else
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${LDP}--no-whole-archive "
		__LINK_C_LIBS="-lpthread -lm -lc -lrt"
	fi
fi

LINK_LIBS="${__LINK_AMBA_LIBS} "
LINK_LDS="${AMBA_LDS}"
if [ "${CONFIG_LINUX}" != "" ]; then
	tmp=
	for f in ${LDFLAGS_amba_common}; do
		tmp+="${LDP}${f} "
	done
	LDFLAGS_amba_common=${tmp}
fi
LINK_LDFLAGS="${LDFLAGS} ${LDFLAGS_amba_common} "
if [ "${CONFIG_LINUX}" == "" ]; then
	if [ "x${CONFIG_CC_USESTD}" != "x" ]; then
		LINK_LIBS+="-lnosys "
		LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libc.a`)) "
		LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libgcc.a`)) "
		LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libstdc++.a`)) "
		LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libnosys.a`)) "
	fi
	LINK_LDFLAGS+="-L${AMBA_O_LIB} "
else
	LINK_LDFLAGS+="-L${O}/lib "
fi

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=out
fi

# ---- For calculating rpc crc in AmbaLink ------- #
AMBALINK_DIR="${srctree}/svc/comsvc/ambalink"
RPC_CFLAGS="`echo ${KBUILD_CFLAGS} | sed 's/\"//g'`"
RPC_CRC_EXEC_DIR="${AMBALINK_DIR}/tools/exec"
RPC_CRC_SH="${RPC_CRC_EXEC_DIR}/rpc_crc.sh"
RPC_CRC_OUTPUT="${LINK_AMBA_OUT_DIR}/AmbaIPC_RpcHeaderCrc"
RPC_CRC_TYPE="public"
RPC_CRC_HEADERS="${AMBALINK_DIR}/inc/rpcprog"
RPC_CRC_CMD="${RPC_CRC_SH} ${RPC_CRC_OUTPUT} ${RPC_CRC_EXEC_DIR} ${RPC_CRC_TYPE} ${RPC_CRC_HEADERS}"
BUILD_RPC_CRC_CMD="${CC} ${RPC_CFLAGS} -c ${RPC_CRC_OUTPUT}.c -o ${RPC_CRC_OUTPUT}.o"
AMBA_HOST_CRC="${srctree}/svc/comsvc/ambalink/tools/exec/lnx/host_crc32"

# ---- Definition general rules for link target ---- #
if [ "$CONFIG_ENABLE_AMBALINK" = "y" ]; then
	LINK_OBJS="${LINK_OBJS} ${RPC_CRC_OUTPUT}.o"
fi

LINK_OUTPUT_ELF=${IMG_NAME}
if [ "${CONFIG_LINUX}" == "" ]; then
	LINK_OUTPUT_BIN=$(echo ${IMG_NAME} | sed -e 's/elf/bin/')
else
	LINK_OUTPUT_BIN=$(echo ${IMG_NAME} | sed -e 's/.elf//')
fi
LINK_OUTPUT_MAP=$(echo ${IMG_NAME} | sed -e 's/elf/map/')
LINK_OUTPUT_NM=$(echo ${IMG_NAME} | sed -e 's/elf/nm/')
LINK_OUTPUT_FSYM=$(echo ${IMG_NAME} | sed -e 's/elf/fsym/')
LINK_OUTPUT_VSYM=$(echo ${IMG_NAME} | sed -e 's/elf/vsym/')

READELF=$(echo ${OBJCOPY} | sed -e 's/objcopy/readelf/')
if [ "${CONFIG_LINUX}" == "" ]; then
	if [ "${CONFIG_ARM64}" = "y" ]; then
		LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${AMBA_LDS} --start-group ${LINK_LIBS} --end-group ${LINK_OBJS} -Map=${LINK_OUTPUT_MAP} --cref"
	else
		LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${AMBA_LDS} --start-group ${LINK_LIBS} --end-group ${LINK_OBJS} -Map=${LINK_OUTPUT_MAP} --cref --wrap=memcpy"
	fi
	# use__wrap_rand() to replace stdlib rand()
	#LINK_CMD+=" --wrap=rand"
	NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"
	OBJCOPY_CMD="${OBJCOPY} -R .cv_rtos_user -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"
	READELF_CMD="${READELF} -W --symbols ${LINK_OUTPUT_ELF}"
else
	LINK_CMD="${CC} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_BIN} -T ${AMBA_LDS} -Wl,--start-group ${LINK_LIBS} -Wl,--end-group ${LINK_OBJS} ${__LINK_C_LIBS} "
	LINK_CMD+="-Wl,-Map=${LINK_OUTPUT_MAP} -Wl,--cref "
	NM_CMD=""
	OBJCOPY_CMD=""
fi
