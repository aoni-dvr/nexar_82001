#!/bin/bash
#
# ---- Parameters declaration ---- #
# $1: [clean | abs-path/SYS-name.elf]

case "${KBUILD_VERBOSE}" in
*1*)
	set -x
	;;
esac

# ---- Functions declaration ---- #

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
	r=`grep "linker stubs" $1 | head -1 | awk '{if($4 == "linker"){print $3}else{print $2}}'`
	if [ "x${r}" = "x" ]; then
		echo '0x0'
	elif [ "x${r}" = "xlinker" ]; then
		# pattern: LOAD linker stubs
		echo '0x0'
	else
		echo ${r}
	fi
}

extract_linker_stub()
{
	r=`grep "linker stubs" $1 | head -1 | awk '{if($4 == "linker"){print $2}else{print $1}}'`
	if [ "x${r}" = "x" ]; then
		echo '0x0'
	elif [ "x${r}" = "xLOAD" ]; then
		# pattern: LOAD linker stubs
		echo '0x0'
	else
		echo ${r}
	fi
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
elif [ "$1" != "" ]; then
	SYS_ELF=$1
else
	SYS_ELF=""
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


if [ "${SYS_ELF}" == "" ] ; then
REGION_INFO="unsigned int Region0RoBase   = 0x0;\n
             unsigned int Region0RoLength = 0x0;\n
             unsigned int Region0RwBase   = 0x0;\n
             unsigned int Region0RwLength = 0x0;\n

             unsigned int Region1RoBase   = 0x0;\n
             unsigned int Region1RoLength = 0x0;\n
             unsigned int Region1RwBase   = 0x0;\n
             unsigned int Region1RwLength = 0x0;\n

             unsigned int Region2RoBase   = 0x0;\n
             unsigned int Region2RoLength = 0x0;\n
             unsigned int Region2RwBase   = 0x0;\n
             unsigned int Region2RwLength = 0x0;\n

             unsigned int Region3RoBase   = 0x0;\n
             unsigned int Region3RoLength = 0x0;\n
             unsigned int Region3RwBase   = 0x0;\n
             unsigned int Region3RwLength = 0x0;\n

             unsigned int Region4RoBase   = 0x0;\n
             unsigned int Region4RoLength = 0x0;\n
             unsigned int Region4RwBase   = 0x0;\n
             unsigned int Region4RwLength = 0x0;\n

             unsigned int Region5RoBase   = 0x0;\n
             unsigned int Region5RoLength = 0x0;\n
             unsigned int Region5RwBase   = 0x0;\n
             unsigned int Region5RwLength = 0x0;\n
             unsigned int LinkerStubBase  = 0x0;\n
             unsigned int LinkerStubLength= 0x0;\n
             unsigned int DspBufBase      = 0x0;\n
             unsigned int DspBufLength    = 0x0;\n"
else
SYS_MAP=`echo ${SYS_ELF} | sed -e 's/.elf/.map/'`

if [ ! -e ${SYS_ELF} ]; then
REGION_INFO="unsigned int Region0RoBase   = 0x0;\n
             unsigned int Region0RoLength = 0x0;\n
             unsigned int Region0RwBase   = 0x0;\n
             unsigned int Region0RwLength = 0x0;\n

             unsigned int Region1RoBase   = 0x0;\n
             unsigned int Region1RoLength = 0x0;\n
             unsigned int Region1RwBase   = 0x0;\n
             unsigned int Region1RwLength = 0x0;\n

             unsigned int Region2RoBase   = 0x0;\n
             unsigned int Region2RoLength = 0x0;\n
             unsigned int Region2RwBase   = 0x0;\n
             unsigned int Region2RwLength = 0x0;\n

             unsigned int Region3RoBase   = 0x0;\n
             unsigned int Region3RoLength = 0x0;\n
             unsigned int Region3RwBase   = 0x0;\n
             unsigned int Region3RwLength = 0x0;\n

             unsigned int Region4RoBase   = 0x0;\n
             unsigned int Region4RoLength = 0x0;\n
             unsigned int Region4RwBase   = 0x0;\n
             unsigned int Region4RwLength = 0x0;\n

             unsigned int Region5RoBase   = 0x0;\n
             unsigned int Region5RoLength = 0x0;\n
             unsigned int Region5RwBase   = 0x0;\n
             unsigned int Region5RwLength = 0x0;\n
             unsigned int LinkerStubBase  = 0x0;\n
             unsigned int LinkerStubLength= 0x0;\n
             unsigned int DspBufBase      = 0x0;\n
             unsigned int DspBufLength    = 0x0;\n"
else # [ -e ${SYS_ELF} ];


REGION_INFO="unsigned int Region0RoBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region0_ro_start);\n
             unsigned int Region0RoLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region0_ro_end) - 0x$(extract_symbol ${SYS_ELF} __ddr_region0_ro_start));\n  \
             unsigned int Region0RwBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region0_rw_start);\n
             unsigned int Region0RwLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region0_rw_end) - \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region0_rw_start));\n \

             unsigned int Region1RoBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region1_ro_start);\n  \
             unsigned int Region1RoLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region1_ro_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region1_ro_start));\n \
             unsigned int Region1RwBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region1_rw_start);\n  \
             unsigned int Region1RwLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region1_rw_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region1_rw_start));\n  \

             unsigned int Region2RoBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region2_ro_start);\n  \
             unsigned int Region2RoLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region2_ro_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region2_ro_start));\n \
             unsigned int Region2RwBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region2_rw_start);\n  \
             unsigned int Region2RwLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region2_rw_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region2_rw_start));\n  \

             unsigned int Region3RoBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region3_ro_start);\n  \
             unsigned int Region3RoLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region3_ro_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region3_ro_start));\n \
             unsigned int Region3RwBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region3_rw_start);\n  \
             unsigned int Region3RwLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region3_rw_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region3_rw_start));\n  \

             unsigned int Region4RoBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region4_ro_start);\n  \
             unsigned int Region4RoLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region4_ro_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region4_ro_start));\n \
             unsigned int Region4RwBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region4_rw_start);\n  \
             unsigned int Region4RwLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region4_rw_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region4_rw_start));\n  \

             unsigned int Region5RoBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region5_ro_start);\n  \
             unsigned int Region5RoLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region5_ro_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region5_ro_start));\n \
             unsigned int Region5RwBase   = 0x$(extract_symbol ${SYS_ELF} __ddr_region5_rw_start);\n  \
             unsigned int Region5RwLength = (0x$(extract_symbol ${SYS_ELF} __ddr_region5_rw_end) -   \
                                            0x$(extract_symbol ${SYS_ELF} __ddr_region5_rw_start));\n  \
             unsigned int LinkerStubBase   = $(extract_linker_stub ${SYS_MAP});\n   \
             unsigned int LinkerStubLength = $(extract_linker_stub_length ${SYS_MAP});\n   \

             unsigned int DspBufBase      = 0x$(extract_symbol ${SYS_ELF} __dsp_buf_start);\n
             unsigned int DspBufLength    = (0x$(extract_symbol ${SYS_ELF} __dsp_buf_end) - 0x$(extract_symbol ${SYS_ELF} __dsp_buf_start));\n"
fi # if [ ! -e ${SYS_ELF} ]; then
fi # if [ "${SYS_ELF}" == "" ] ; then

# Put at srctree, because Makefile read it from srctree
__REGION_INFO_FILE__=fwprog/AmbaFwLoader_RegionInfo
REGION_INFO_FILE="${objtree}/${__REGION_INFO_FILE__}"
mkdir -p ${objtree}/fwprog

rm -f ${REGION_INFO_FILE}.c
echo -e "  GEN     ${__REGION_INFO_FILE__}.c"

echo -e "/*"						> ${REGION_INFO_FILE}.c
echo -e " * Automatically generated file: don't edit" 	>> ${REGION_INFO_FILE}.c
echo -e " */"						>> ${REGION_INFO_FILE}.c
echo -e ""							>> ${REGION_INFO_FILE}.c
echo -e ${REGION_INFO}				>> ${REGION_INFO_FILE}.c
echo -e ""							>> ${REGION_INFO_FILE}.c

# ---- Definition for link target dependent ---- #

LOCAL_CFLAGS="`echo ${KBUILD_CFLAGS} | sed 's/\"//g'`"

BUILD_CMD="${CC} ${LOCAL_CFLAGS} -c ${REGION_INFO_FILE}.c -o ${REGION_INFO_FILE}.o"

#SYS_BIN=${CONFIG_FWPROG_SYS}
#SYS_ELF=`echo ${SYS_BIN} | sed -e 's/.bin/.elf/'`

rm -f ${REGION_INFO_FILE}.list
echo -e "  GEN     ${__REGION_INFO_FILE__}.list"
touch ${REGION_INFO_FILE}.list

##  SYS_OUTPUT= rtos/output ---- It is 32 bit output file folder. #
#if [ "${O}" != "" ]; then
#	SYS_OUTPUT=${O}
#else # TODO
#	SYS_OUTPUT="${srctree}/output"
#fi
#
## We can find the sys.elf in now 64 bit working directory. We need to switch to 32 bit output folder. #
#if [ ! -e ${SYS_ELF} ]; then
#    cd ${SYS_OUTPUT}
#fi

if [ "${SYS_ELF}" == "" ]  || [ ! -e ${SYS_ELF} ]; then
    echo "REGION0_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION0_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION0_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION0_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION1_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION1_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION1_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION1_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION2_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION2_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION2_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION2_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION3_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION3_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION3_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION3_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION4_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION4_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION4_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION4_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION5_RO_BASE  0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION5_RO_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION5_RW_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "REGION5_RW_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "DSP_BUF_BASE 0x00000000" >> ${REGION_INFO_FILE}.list
    echo "DSP_BUF_LENGTH 0x00000000 - 0x00000000" >> ${REGION_INFO_FILE}.list
else # [ -e ${SYS_ELF} ]
    REGION_INFO="REGION0_RO_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region0_ro_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION0_RO_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region0_ro_end) -    \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region0_ro_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION0_RW_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region0_rw_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION0_RW_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region0_rw_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region0_rw_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list

    REGION_INFO="REGION1_RO_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region1_ro_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION1_RO_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region1_ro_end) -    \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region1_ro_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION1_RW_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region1_rw_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION1_RW_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region1_rw_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region1_rw_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list

    REGION_INFO="REGION2_RO_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region2_ro_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION2_RO_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region2_ro_end) -    \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region2_ro_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION2_RW_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region2_rw_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION2_RW_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region2_rw_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region2_rw_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list

    REGION_INFO="REGION3_RO_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region3_ro_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION3_RO_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region3_ro_end) -    \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region3_ro_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION3_RW_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region3_rw_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION3_RW_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region3_rw_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region3_rw_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list

    REGION_INFO="REGION4_RO_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region4_ro_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION4_RO_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region4_ro_end) -    \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region4_ro_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION4_RW_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region4_rw_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION4_RW_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region4_rw_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region4_rw_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list

    REGION_INFO="REGION5_RO_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region5_ro_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION5_RO_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region5_ro_end) -    \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region5_ro_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION5_RW_BASE    0x$(extract_symbol ${SYS_ELF} __ddr_region5_rw_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="REGION5_RW_LENGTH    (0x$(extract_symbol ${SYS_ELF} __ddr_region5_rw_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __ddr_region5_rw_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="DSP_BUF_BASE    0x$(extract_symbol ${SYS_ELF} __dsp_buf_start)"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="DSP_BUF_LENGTH    (0x$(extract_symbol ${SYS_ELF} __dsp_buf_end) -   \
                                                0x$(extract_symbol ${SYS_ELF} __dsp_buf_start))"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="LINKER_STUB_BASE    $(extract_linker_stub ${SYS_MAP})"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list
    REGION_INFO="LINKER_STUB_LENGTH    $(extract_linker_stub_length ${SYS_MAP})"
    echo ${REGION_INFO} >> ${REGION_INFO_FILE}.list

fi # if [ ! -e ${SYS_ELF} ]; then

# We need to switch to 64 bit output folder for below bst and bld output. #
cd ${objtree}

# ---- Run function ---- #
#show_variables
#echo ${BUILD_CMD}
echo -e "  CC      ${__REGION_INFO_FILE__}.o"
${BUILD_CMD}


echo "$0 Done..."


