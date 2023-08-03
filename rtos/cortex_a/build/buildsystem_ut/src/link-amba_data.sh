#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

AMBA_LIBS="
	libcrt.a
	libmain.a
	"
AMBA_LIBS+="libwrap_std_ut.a"

AMBA_LIBS=$(echo ${AMBA_LIBS} | tr  " " "\n" | sort | tr  "\n" " ")
AMBA_LIBS_NUM=$(echo ${AMBA_LIBS} | wc -w)

# Search lib path
__LINK_AMBA_LIBS="--whole-archive"
for i in ${AMBA_LIBS}; do
	if [ -s ${AMBA_O_LIB}/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_O_LIB}/${i}"
	elif [ -s ${AMBA_LIB_FOLDER}/${i} ]; then
		__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} ${AMBA_LIB_FOLDER}/${i}"
	else
		echo "Can not found ${i}"
		exit 1
	fi
done

__LINK_AMBA_LIBS="${__LINK_AMBA_LIBS} --no-whole-archive"
__LINK_AMBA_LIBS=$(echo ${__LINK_AMBA_LIBS} | tr -s [:space:])

__LINK_C_LIBS="-lwrap_std "

if [ "x${CONFIG_CC_GCOV}" != "x" ]; then
	__LINK_C_LIBS+="-lgcov "
fi
if [ "x${CONFIG_CC_CXX_SUPPORT}" != "x" ]; then
	__LINK_C_LIBS+="-lstdc++  "
fi
__LINK_C_LIBS+="-lm -lc -lgcc "

LINK_LIBS="${__LINK_AMBA_LIBS} "
LINK_LDS="${AMBA_LDS}"
LINK_LDFLAGS="${LDFLAGS} ${LDFLAGS_amba_common} "
if [ "x${CONFIG_CC_USESTD}" != "x" ]; then
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libc.a`)) "
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libgcc.a`)) "
    LINK_LDFLAGS+="-L$(dirname $(readlink -f `${CC} ${KBUILD_CFLAGS} -print-file-name=libstdc++.a`)) "
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

LINK_CMD="${LD} ${LINK_LDFLAGS} -o ${LINK_OUTPUT_ELF} -T ${AMBA_LDS} --start-group ${LINK_LIBS} --end-group ${LINK_OBJS} ${__LINK_C_LIBS} "
LINK_CMD+=" -Map=${LINK_OUTPUT_MAP} --cref "
if [ "x${CONFIG_ARM32}" != "x" ]; then
    LINK_CMD+="--wrap=memcpy "
fi
NM_CMD="${NM} -n -l ${LINK_OUTPUT_ELF}"
OBJCOPY_CMD="${OBJCOPY} -O binary ${LINK_OUTPUT_ELF} ${LINK_OUTPUT_BIN}"

