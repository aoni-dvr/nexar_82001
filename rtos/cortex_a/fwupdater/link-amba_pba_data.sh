#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

AMBA_LIBS="
    libio_common.a
    libio_${AMBA_CHIP_ID}.a
    libperif_${AMBA_CHIP_ID}.a
    libdsp_visualHL.a
    libdsp_visualLL.a
    libbsp_${AMBA_BSP_NAME}.a
    ${AMBA_CHIP_ID}/libimg_cal.a
    ${AMBA_CHIP_ID}/libimg_proc.a
    libfs_${AMBA_CHIP_ID}.a
    libsvc_icamcore.a
	libprfile.a
    libpba.a
"

AMBA_LIBS="${AMBA_LIBS} libkal.a libthreadX_${CONFIG_KAL_THREADX_SMP_NUM_CORES}core.a"

if [ "${CONFIG_SOC_CV2FS}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libdsp_imgkernel.a"
    if [ -e "${srctree}/vendors/ambarella/lib/${AMBA_CHIP_ID}/libdsp_imgkernel_similar.a" ]; then
        AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libdsp_imgkernel_similar.a"
    fi
else
    AMBA_LIBS="${AMBA_LIBS} libdsp_imgkernel.a"
fi

if [ -e "${srctree}/vendors/ambarella/lib/${AMBA_CHIP_ID}/libdsp_imgkernelcore.a" ]; then
    AMBA_LIBS="${AMBA_LIBS} ${AMBA_CHIP_ID}/libdsp_imgkernelcore.a"
fi


if [ "${CONFIG_BUILD_SVC_COMMON_SERVICE}" = "y" ]; then
	AMBA_LIBS="${AMBA_LIBS} libcomsvc.a"
fi

if [ "${CONFIG_ENABLE_AMBALINK}" = "y" ]; then
    AMBA_LIBS="${AMBA_LIBS} libambalink.a libfdt.a libwrapc.a"
fi

AMBA_LIBS=$(echo ${AMBA_LIBS} | tr  " " "\n" | sort | tr  "\n" " ")
AMBA_LIBS_NUM=$(echo ${AMBA_LIBS} | wc -w)

# Search lib path
__LINK_AMBA_LIBS="--whole-archive"
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


__LINK_AMBA_LIBS=$(echo ${__LINK_AMBA_LIBS} | tr -s [:space:])
if [ "x${CONFIG_CC_LTO}" != "x" ]; then
    __LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} -lm -lgcc -lc --no-whole-archive "
    __LINK_C_LIBS=" "
else
    __LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} --no-whole-archive "
    __LINK_C_LIBS="-lwrap_std -lm -lgcc -lc "
fi

LINK_LIBS="${__LINK_AMBA_LIBS} "
LINK_LDS="${AMBA_PBA_LDS}"
LINK_LDFLAGS="${LDFLAGS} ${LDFLAGS_amba_common} "
if [ "x${CONFIG_CC_USESTD}" != "x" ]; then
    LINK_LIBS+="-lnosys "
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libc.a`)) "
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libgcc.a`)) "
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libstdc++.a`)) "
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libnosys.a`)) "
fi
LINK_LDFLAGS+="-L${srctree}/${O}/lib "

if [ ${KBUILD_AMBA_OUT_DIR} ]; then
	LINK_AMBA_OUT_DIR=${KBUILD_AMBA_OUT_DIR}
	mkdir -p ${LINK_AMBA_OUT_DIR}
else
	LINK_AMBA_OUT_DIR=out
fi

LINK_OUTPUT_ELF=${IMG_NAME}
LINK_OUTPUT_BIN=$(echo ${IMG_NAME} | sed -e 's/elf/bin/')
LINK_OUTPUT_MAP=$(echo ${IMG_NAME} | sed -e 's/elf/map/')
LINK_OUTPUT_NM=$(echo ${IMG_NAME} | sed -e 's/elf/nm/')

LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${AMBA_PBA_LDS} --start-group ${LINK_LIBS} ${LINK_OBJS} ${__LINK_C_LIBS} --end-group "
LINK_CMD+=" -Map=${LINK_OUTPUT_MAP} --cref "
LINK_CMD+="--wrap=memcpy"
NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"
OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"

