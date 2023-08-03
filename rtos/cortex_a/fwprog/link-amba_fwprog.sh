#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

# Delete output files in case of error
trap cleanup SIGHUP SIGINT SIGQUIT SIGTERM ERR
cleanup()
{
#	echo "cleanup FlashFwProg"
#	rm -rf ${KBUILD_SRC}/${O}/${KBUILD_AMBA_FWPROG_OUT_DIR}
#	rm -rf ${KBUILD_SRC}/${O}/${DIR_amba_fwprog}/${DIR_amba_fwprog_out}
	echo ""
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

	if [ -s ../${O}.64/${KCONFIG_CONFIG} ]; then
		make clean -C ../${O}.64  O=${O}.64 -f ${KBUILD_SRC}/Makefile
	fi

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

SKIP_BUILD_SDFW=n
if [ "${CONFIG_ARM32}" = "y" ]; then
if [ "${CONFIG_BUILD_32BIT_FWPROG}" != "y" ]; then
	# Do not build 32-bit firmware programmer by default
	SKIP_BUILD_SDFW=y
fi
fi

if [ "${SKIP_BUILD_SDFW}" != "y" ]; then
#echo "================================================="
#echo "Generate SD firmware"
#echo "================================================="
rm -rf ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/
mkdir -p ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/

if [ ${HOST_OS} == "CYGWIN" ]; then
	srctree=`echo ${srctree} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
	objtree=`echo ${objtree} | sed -e 's@/cygdrive/\([a-zA-Z]\)@\1:@g'`
fi

# Need abs-path and '/'
${AMBA_HOST_SDFW_GEN} `readlink -f ${objtree}/fwprog/.temp` \
    `readlink -f ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/`/ \
    `readlink -f ${objtree}/${KBUILD_AMBA_OUT_DIR}/`/ \
    `readlink -f ${objtree}/fwprog/`/

${AMBA_HOST_PTB_GEN} `readlink -f ${objtree}/fwprog/.temp` \
    `readlink -f ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}/devfw/`/ \
    `readlink -f ${objtree}/${KBUILD_AMBA_OUT_DIR}/`/ \
    `readlink -f ${objtree}/fwprog/`/

echo "Build fwprog done!"
echo "===> All *.elf files are put at ${objtree}/${KBUILD_AMBA_FWPROG_OUT_DIR}"
fi

