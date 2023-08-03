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

run_readelf_cmd()
{
	if [ ${KBUILD_VERBOSE} == 0 ]; then
		echo "  READELF ${LINK_OUTPUT_FSYM}"
		echo "  READELF ${LINK_OUTPUT_VSYM}"
	fi

    ${READELF_CMD} | grep 'FUNC    ' | grep -v '     0 FUNC    ' | awk '{ print $2, $3, $8 }' | sort > ${LINK_OUTPUT_FSYM}
    ${srctree}/fwprog/hostutils/exec/lnx/host_symtab_gen ${LINK_OUTPUT_FSYM} ${srctree}/engineering/project/basic/romfs func
    ${READELF_CMD} | grep 'OBJECT  GLOBAL' | grep -v '     0 OBJECT  ' | awk '{ print $8, $2, $3 }' | sort | awk '{ print $2, $3, $1 }' > ${LINK_OUTPUT_VSYM}
    ${srctree}/fwprog/hostutils/exec/lnx/host_symtab_gen ${LINK_OUTPUT_VSYM} ${srctree}/engineering/project/basic/romfs var
}

run_bincopy_cmd()
{
if [ -n "${CONFIG_SYS}" ] ; then
    LINK_OUTPUT_TMP_BIN="${CONFIG_SYS}.tmp"

    cp ${LINK_OUTPUT_BIN} ${LINK_OUTPUT_TMP_BIN}

    # Copy 32-bit OS binary
    SYS_BIN="`echo ${CONFIG_SYS} | sed 's/.gz//g'`"

    mkdir -p ../${O}.64/${KBUILD_AMBA_OUT_DIR}

    cp -f ../${O}/${SYS_BIN} ../${O}.64/${CONFIG_SYS}.tmp
    cp -f ../${O}/${CONFIG_SYS} ../${O}.64/${KBUILD_AMBA_OUT_DIR}
fi
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
