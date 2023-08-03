#!/bin/bash

# Error out on error
set -e
# Debug
#set -x


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
}

rpc_crc_calc()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  GEN     ${RPC_CRC_OUTPUT}.c"
		echo "  CC      ${RPC_CRC_OUTPUT}.o"
	fi
	echo "${RPC_CRC_CMD}"
	echo "${BUILD_RPC_CRC_CMD}"
	${RPC_CRC_CMD}
	${BUILD_RPC_CRC_CMD}
}

run_bincopy_cmd()
{
if [ -n "${CONFIG_FWPROG_SYS}" ] ; then
    LINK_OUTPUT_TMP_BIN="${CONFIG_FWPROG_SYS}.tmp"

    if [ -n "${CONFIG_XEN_SUPPORT}" ]; then
        gzip -9 -c ${LINK_OUTPUT_BIN} > ${LINK_OUTPUT_BIN}.gz
        mv ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_BIN}.orig
        mv ${LINK_OUTPUT_BIN}.gz ${LINK_OUTPUT_BIN}
    fi

    # lz4
    if [ -n "${CONFIG_FASTBOOT_LZ4}" ]; then
        lz4 -B4 ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_BIN}.lz4
        mv ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_BIN}.orig
        mv ${LINK_OUTPUT_BIN}.lz4 ${LINK_OUTPUT_BIN}
    fi

    cp ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_TMP_BIN}

    # Copy 32-bit OS binary
    SYS_BIN="`echo ${CONFIG_FWPROG_SYS} | sed 's/.gz//g'`"

    mkdir -p ../${O}.64/${KBUILD_AMBA_OUT_DIR}

    cp -f ../${O}/${SYS_BIN} ../${O}.64/${CONFIG_FWPROG_SYS}.tmp
    cp -f ../${O}/${CONFIG_FWPROG_SYS} ../${O}.64/${KBUILD_AMBA_OUT_DIR}
fi
}

