#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

if [ "x${CONFIG_LINUX}" != "x" ]; then
LDP="-Wl,"
fi

AMBA_LIBS="
    libbsp_${AMBA_BSP_NAME}.a
    libperif_${AMBA_CHIP_ID}.a
    libio_${AMBA_CHIP_ID}.a
    libfs_${AMBA_CHIP_ID}.a
    libkal.a
    libprint.a
    libmisc.a
    libshell.a
    libwrap_std.a
    libsvc_icamcore.a
    libsvc_icam.a
    libdsp_wrapper.a
    libdsp_imgkernel.a
    libdsp_imgkernelcore.a
    libimg_proc.a
"

#    libio_common.a

AMBA_USBLIBS="
    libusbx_hostcontroller.a
    libusbx_cdcacmd.a
    libusbx_msch.a
    libusbx_mtpd.a
    libusbx_mscd.a
    libusbx_sys.a
    libusbx_device.a
    libusbx_host.a
    libusb_sys.a
    libusb_cdcacmd.a
    libusb_mscd.a
    libusb_mtpd.a
    libusb_msch.a
    libcomsvc_usb.a
"
if [ "${CONFIG_BUILD_MAL}" = "y" ]; then
     AMBA_LIBS="${AMBA_LIBS} libmal.a"
fi

if [ "${CONFIG_BUILD_DSP_AUDIO}" = "y" ]; then
     AMBA_LIBS="${AMBA_LIBS} libdsp_audio.a"
fi

if [ "${CONFIG_ICAM_IMGCAL_USED}" = "y" ]; then
     AMBA_LIBS="${AMBA_LIBS} libimg_cal.a"
fi

if [ "${CONFIG_SOC_CV2FS}" = "y" ]; then
    if [ -e "${srctree}/vendors/ambarella/lib64/${AMBA_CHIP_ID}/libdsp_imgkernel_similar.a" ]; then
        AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libdsp_imgkernel_similar.a"
    fi
    AMBA_LIBS="${AMBA_LIBS} libdsp_visualSimilar.a"
fi

if [ "${CONFIG_ENABLE_DSP_MONITOR}" = "y" ]; then
     AMBA_LIBS="${AMBA_LIBS} libdsp_visualmonitor.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_USB}" = "y" ]; then
    AMBA_LIBS="${AMBA_USBLIBS} ${AMBA_LIBS}"
fi

if [ "${CONFIG_BUILD_CV}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libambacv.a libcv_schdr.a"
    CONFIG_LIST="${objtree}/svc/comsvc/cv/cvtask_libs
                 ${objtree}/soc/vision/cvtask_libs"
    for c in ${CONFIG_LIST}; do
        . $c
        for m in ${CVTASK_AS_SRC}; do
            m=`basename $m`
            AMBA_LIBS="${AMBA_LIBS} libcv_${m}.a"
        done
        for m in ${CVTASK_AS_BIN}; do
            m=`basename $m`
            AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libcv_${m}.a"
        done
        for m in ${CVAPP_AS_SRC}; do
            m=$( echo "$m" |cut -d/ -f1 )
            AMBA_LIBS="${AMBA_LIBS} libcv_${m}.a"
        done
        for m in ${CVAPP_AS_BIN}; do
            m=`basename $m`
            AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libcv_${m}.a"
        done
    done
    if [ "${CONFIG_BUILD_COMMON_SERVICE_CV}" = "y" ]; then
        AMBA_LIBS="${AMBA_LIBS} libflexidagio.a"
    fi
    AMBA_LIBS="${AMBA_LIBS}"
fi

# audio aac
if [ "${CONFIG_ENABLE_AAC_ENCODER}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libaudio_aacenc.a"
fi

if [ "${CONFIG_ENABLE_AAC_DECODER}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libaudio_aacdec.a"
fi

if [ "${CONFIG_BUILD_AMBA_ADAS}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libadas.a"
	AMBA_LIBS="${AMBA_LIBS} libadas_refflow.a"
fi

if [ "${CONFIG_BUILD_IMGFRW_EIS}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libimg_eis.a"
fi

if [ "${CONFIG_ICAM_CV_STEREO}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libimg_stu.a"
fi

if [ "${CONFIG_ICAM_CV_STIXEL}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libimg_camphdet.a"
fi

if [ "${CONFIG_ICAM_VIEWCTRL_USED}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libimg_warputility.a"
fi

if [ -e "${srctree}/vendors/ambarella/lib64/${AMBA_CHIP_ID}/libimg_dirtdetect.a" ]; then
    AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libimg_dirtdetect.a"
fi

# image framework if enabled
if [ "${CONFIG_BUILD_COMSVC_IMGFRW}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libimgfrw.a"
fi

if [ "${CONFIG_BUILD_IMGFRW_AAA}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libimgaaa.a"
fi

# safety framework if enabled
if [ "${CONFIG_BUILD_COMSVC_MONFRW}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libmonfrw.a"
fi

if [ "${CONFIG_BUILD_SSP_AIO}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libaudio_aio.a"
fi

if [ "${CONFIG_BUILD_COMSVC_DRAMSHMOO}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libdramshmoo.a"
fi

if [ "${CONFIG_BUILD_COMMON_TUNE_ITUNER}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libimg_ituner.a"
fi

if [ "${CONFIG_BUILD_COMMON_TUNE_CTUNER}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libimg_tuner.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_FTCM}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_ftcm.a"
fi

if [ "${CONFIG_BUILD_SSP_ENET}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libplat_enet.a"
fi

if [ "${CONFIG_BUILD_FOSS_LVGL}" = "y" ]; then
       AMBA_LIBS="${AMBA_LIBS} liblvgl.a"
fi


if [ "${CONFIG_BUILD_COMMON_SERVICE_DRAW}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_draw.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_CODEC}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_codec.a"
fi

if [ "${CONFIG_BUILD_COMSVC_CONTAINER}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_container.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_DCF}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_dcf.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_EXIF}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libcomsvc_exif.a"
fi

if [ "${CONFIG_BUILD_COMSVC_FIFO}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_fifo.a"
fi

if [ "${CONFIG_BUILD_COMSVC_CONTAINER}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_wrapper.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_CFS}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc_cfs.a"
fi

if [ "${CONFIG_BUILD_SSP_NETX}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libnetx.a"
    if [ "${CONFIG_NETX_TELNET}" = "y" ]; then
        AMBA_LIBS="${AMBA_LIBS} libnetx_telnet.a"
    fi
fi

if [ "${CONFIG_AMBA_RTSP}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libnetstack_rtsp.a"
fi

if [ "${CONFIG_BUILD_COMMON_SERVICE_ANIM}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libcomsvc_anim.a"
fi

if [ "${CONFIG_ENABLE_AMBALINK}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libambalink.a"
fi

#if [ "${CONFIG_ENABLE_AMBALINK}" == "y" ] || [ "${CONFIG_OPENAMP}" == "y" ] || [ "${CONFIG_BUILD_FOSS_LIBFDT}" == "y" ]; then
#    AMBA_LIBS="${AMBA_LIBS} libwrapc.a"
#fi

#if [ "${CONFIG_BUILD_FOSS_LIBFDT}" == "y" ]; then
#    AMBA_LIBS="${AMBA_LIBS} libfdt.a"
#fi

if [ "${CONFIG_BUILD_FOSS_LWIP}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} liblwip.a"
fi

if [ "${CONFIG_BUILD_FOSS_LIB}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libfoss.a"
fi

if [ "${CONFIG_BUILD_FOSS_MBEDTLS}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libmbedtls.a"
fi

AMBA_LIBS=$(echo ${AMBA_LIBS} | tr  " " "\n" | sort | tr  "\n" " ")
AMBA_LIBS_NUM=$(echo ${AMBA_LIBS} | wc -w)


if [ "${CONFIG_OPENAMP}" = "y" ]; then
     AMBA_LIBS="${AMBA_LIBS} libopen_amp.a libmetal.a"
fi


# Search lib path
__LINK_AMBA_LIBS="${LDP}--whole-archive"
for i in ${AMBA_LIBS}; do
	if [ -s ${AMBA_O_LIB}/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_O_LIB}/${i}"
	elif [ -s ${AMBA_LIB64_FOLDER}/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB64_FOLDER}/${i}"
	elif [ -s ${AMBA_LIB64_FOLDER}/${AMBA_CHIP_ID}/linux/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB64_FOLDER}/${AMBA_CHIP_ID}/linux/${i}"
	else
		echo "Can not found ${i}"
		exit 1
	fi
done


__LINK_AMBA_LIBS=$(echo ${__LINK_AMBA_LIBS} | tr -s [:space:])
if [ "x${CONFIG_CC_LTO}" != "x" ]; then
    __LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} -lm -lgcc -lc ${LDP}--no-whole-archive "
    __LINK_C_LIBS=" "
else
	if [ "x${CONFIG_LINUX}" = "x" ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} --no-whole-archive "
		__LINK_C_LIBS="-lwrap_std -lm -lgcc -lc "
	else
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${LDP}--no-whole-archive "
		__LINK_C_LIBS="-lpthread -lm -lc -lrt"
	fi
fi

LINK_LIBS="${__LINK_AMBA_LIBS} "
LINK_LDS="${AMBA_LDS}"
if [ "x${CONFIG_LINUX}" != "x" ]; then
	tmp=
	for f in ${LDFLAGS_amba_common}; do
		tmp+="${LDP}${f} "
	done
	LDFLAGS_amba_common=${tmp}
fi
LINK_LDFLAGS="${LDFLAGS} ${LDFLAGS_amba_common} "
if [ "x${CONFIG_LINUX}" = "x" ]; then
    if [ "x${CONFIG_CC_USESTD}" != "x" ]; then
        LINK_LIBS+="-lnosys "
        LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libc.a`)) "
        LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libgcc.a`)) "
        LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libstdc++.a`)) "
        LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libnosys.a`)) "
    fi
    LINK_LDFLAGS+="-L${srctree}/${O}/lib "
else
	LINK_LDFLAGS+="-L${O}/lib "
fi

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=out
fi

if [ "$CONFIG_ENABLE_AMBALINK" = "y" ]; then
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
    AMBA_HOST_CRC="${RPC_CRC_EXEC_DIR}/lnx/host_crc32"

    # ---- Definition general rules for link target ---- #
	LINK_OBJS="${LINK_OBJS} ${RPC_CRC_OUTPUT}.o"
fi

LINK_OUTPUT_ELF=${IMG_NAME}
if [ "x${CONFIG_LINUX}" = "x" ]; then
LINK_OUTPUT_BIN=$(echo ${IMG_NAME} | sed -e 's/elf/bin/')
else
LINK_OUTPUT_BIN=$(echo ${IMG_NAME} | sed -e 's/.elf//')
fi
LINK_OUTPUT_MAP=$(echo ${IMG_NAME} | sed -e 's/elf/map/')
LINK_OUTPUT_NM=$(echo ${IMG_NAME} | sed -e 's/elf/nm/')

if [ "x${CONFIG_LINUX}" = "x" ]; then
LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${AMBA_LDS} --start-group ${LINK_LIBS} ${LINK_OBJS} ${__LINK_C_LIBS} --end-group "
LINK_CMD+=" -Map=${LINK_OUTPUT_MAP} --cref "
LINK_CMD+="--wrap=memcpy"
NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"
OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"
else
LINK_CMD="${CC} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_BIN} -T ${AMBA_LDS} -Wl,--start-group ${LINK_LIBS} -Wl,--end-group ${LINK_OBJS} ${__LINK_C_LIBS} "
LINK_CMD+="-Wl,-Map=${LINK_OUTPUT_MAP} -Wl,--cref "
NM_CMD=""
OBJCOPY_CMD=""
fi
