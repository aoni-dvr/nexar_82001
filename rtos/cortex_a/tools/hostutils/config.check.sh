#!/bin/bash
#O_x      offset in MB
#x_M      size in MB
#x_ADDR   addr in 0xabcdabcd

if [ -e .config ]; then
	a53_config=.config
elif  [ -e output/.config ]; then
	a53_config=output/.config
	r52_config=../cortex_r/output/.config
else
	a53_config=`find -name .config|head -n 1`
fi

if [ ! -e ${a53_config} ]; then
	exit 0
fi

#if [ "`grep CONFIG_ENABLE_AMBALINK=y ${a53_config}`" == "" ]; then
#	exit 0
#fi

if [ "`grep CONFIG_DDR_SIZE ${a53_config}`" == "" ]; then
	echo -e "\033[031m CONFIG_DDR_SIZE is not set \033[0m"
	exit 0
fi

if [ "`grep CONFIG_ATF_BL32_SHMEM_SIZE ${a53_config}`" == "" ]; then
	CONFIG_ATF_BL32_SHMEM_SIZE=0
fi

DTB_SIZE=0x00080000
source ${a53_config}

if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" == "" ]; then
	A53_DDR_END=$CONFIG_DDR_SIZE
else
	if [ "`printf \"%d\n\" ${CONFIG_FWPROG_R52SYS_LOADADDR}`" == "0" ]; then
		A53_DDR_END=$CONFIG_DDR_SIZE
	else
		A53_DDR_END=$CONFIG_FWPROG_R52SYS_LOADADDR
	fi
fi

if [ "${CONFIG_AMBALINK_MEM_SIZE}" == "" ]; then
	CONFIG_AMBALINK_MEM_SIZE=0
fi

if [ "${CONFIG_AMBALINK_SHARED_MEM_SIZE}" == "" ]; then
	CONFIG_AMBALINK_SHARED_MEM_SIZE=0
fi

if [ "${CONFIG_PIO_MEM_ALLOC}" == "" ]; then
	CONFIG_PIO_SIZE=0
fi

O_DDR=`awk "BEGIN {print $CONFIG_DDR_SIZE/1024/1024}"`
O_A53DDR=`awk "BEGIN {print $A53_DDR_END/1024/1024}"`
LNX_M=`awk "BEGIN {print $CONFIG_AMBALINK_MEM_SIZE/1024/1024}"`

tmp=`awk "BEGIN {print $LNX_M/2}"`
if [ "`echo ${tmp}|grep '\.'`" != "" ]; then
	echo -e "\033[031m CONFIG_AMBALINK_MEM_SIZE=${CONFIG_AMBALINK_MEM_SIZE} is not 2MB-aligned \033[0m"
	exit 1
fi

if [ "`echo ${LNX_M}|grep '\.'`" != "" ]; then
	echo -e "\033[031m CONFIG_AMBALINK_MEM_SIZE=${CONFIG_AMBALINK_MEM_SIZE} is not 1MB-aligned \033[0m"
	exit 1
fi

#DTB_K=`awk "BEGIN {print $DTB_SIZE/1024}"`
if [ ${LNX_M} -gt 0 ]; then
	G1_M=`awk "BEGIN {print $CONFIG_AMBALINK_SHARED_MEM_SIZE/1024/1024}"`
	G1_min=`awk "BEGIN {print $CONFIG_AMBALINK_RPMSG_SIZE*$CONFIG_AMBALINK_RPMSG_NUM/1024/1024+1}"|cut -d . -f 1`
	#buffer + IPC 1MB
	if [ "${G1_min}" -gt "${G1_M}" ]; then
		echo -e "\033[031m CONFIG_AMBALINK_SHARED_MEM_SIZE ${G1_M}MB is too small, at least ${G1_min}MB \033[0m"
		exit 1
	fi
	LNX_M=`awk "BEGIN {print $LNX_M-$G1_M}"`
	if [ ${LNX_M} -le 0 ]; then
		echo -e "\033[031m CONFIG_AMBALINK_SHARED_MEM_SIZE=${CONFIG_AMBALINK_SHARED_MEM_SIZE} is too big \033[0m"
		exit 1
	fi
fi

tmp=`awk "BEGIN {print $A53_DDR_END-$CONFIG_AMBALINK_MEM_SIZE+$CONFIG_AMBALINK_SHARED_MEM_SIZE-$CONFIG_ATF_BL32_SHMEM_SIZE}"`
DTB_ADDR=`printf "0x%08x\n" ${tmp}`
O_DTB=`awk "BEGIN {print $DTB_ADDR/1024/1024}"`
if [ ${O_DTB} -le 0 ]; then
	echo -e "\033[031m CONFIG_AMBALINK_MEM_SIZE=${CONFIG_AMBALINK_MEM_SIZE} is too big \033[0m"
	exit 1
fi

tmp=`awk "BEGIN {print $A53_DDR_END-$CONFIG_AMBALINK_MEM_SIZE-$CONFIG_ATF_BL32_SHMEM_SIZE}"`
G1_ADDR=`printf "0x%08x\n" ${tmp}`

O_SHM=`awk "BEGIN {print $G1_ADDR/1024/1024}"`
if [ ${O_SHM} -le 0 ]; then
	echo -e "\033[031m CONFIG_AMBALINK_SHARED_MEM_SIZE=${CONFIG_AMBALINK_SHARED_MEM_SIZE} is too big \033[0m"
	exit 1
fi

if [ "${CONFIG_FWPROG_ATF_ENABLE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_ATF_BL31_BASE+$CONFIG_ATF_BL31_SIZE}"`
else
	tmp=`awk "BEGIN {print $CONFIG_FWPROG_SYS_LOADADDR}"`
fi
ATF_BL31_END=`printf "0x%08x\n" ${tmp}`

if [ "${CONFIG_FWPROG_XTB_LOADADDR}" != "" ]; then
	O_XTB=`awk "BEGIN {print $CONFIG_FWPROG_XTB_LOADADDR/1024/1024}"`
	XTB_K=`awk "BEGIN {print $CONFIG_DTB_SIZE/1024}"`
	tmp=`awk "BEGIN {print $CONFIG_FWPROG_XTB_LOADADDR+$CONFIG_DTB_SIZE}"`
	SYS_LOADADDR=`printf "0x%08x\n" ${tmp}`
else
	SYS_LOADADDR=${ATF_BL31_END}
fi

O_LNX=`awk "BEGIN {print $CONFIG_FWPROG_LNX_LOADADDR/1024/1024}"`
if [ ${LNX_M} -gt 0 ]; then
	tmp=`awk "BEGIN {print $A53_DDR_END-$CONFIG_AMBALINK_MEM_SIZE+$CONFIG_AMBALINK_SHARED_MEM_SIZE+0x80000-$CONFIG_ATF_BL32_SHMEM_SIZE}"`
	LNX_LOADADDR=`printf "0x%08x\n" ${tmp}`
	#ambalink 5.10 dtb loaded within shared mem
	tmp=`awk "BEGIN {print and($LNX_LOADADDR-0x80000, 0xFFE00000)}"`
	LNX_2MBADDR=`printf "0x%08x\n" ${tmp}`
else
	LNX_LOADADDR=$SYS_LOADADDR
	#pure linux 5.10 2MB round-up
	tmp=`awk "BEGIN {print and($LNX_LOADADDR+0x200000-0x80000, 0xFFE00000)}"`
	LNX_2MBADDR=`printf "0x%08x\n" ${tmp}`
fi

if [ "${CONFIG_AMBALINK_G2_SHARED_MEM_ADDR}" != "" ]; then
	G2_M=`awk "BEGIN {print $CONFIG_AMBALINK_G2_SHARED_MEM_SIZE/1024/1024}"`
	#buffer + IPC 1MB + SHADOW 1MB + TCM 1MB
	G2_min=`awk "BEGIN {print $CONFIG_AMBALINK_G2_RPMSG_SIZE*$CONFIG_AMBALINK_G2_RPMSG_NUM/1024/1024+3}"|cut -d . -f 1`
	if [ "${G2_min}" -gt "${G2_M}" ]; then
		echo -e "\033[031m CONFIG_AMBALINK_G2_SHARED_MEM_SIZE ${G2_M}MB is too small, at least ${G2_min}MB \033[0m"
		exit 1
	fi
	tmp=`awk "BEGIN {print $CONFIG_DDR_SIZE-$CONFIG_AMBALINK_G2_SHARED_MEM_SIZE}"`
	G2_ADDR=`printf "0x%08x\n" ${tmp}`
	O_G2=`awk "BEGIN {print $G2_ADDR/1024/1024}"`
	R52_M=`awk "BEGIN {print $O_G2-$O_A53DDR}"`
	if [ ${R52_M} -le 0 ]; then
		echo -e "\033[031m CONFIG_AMBALINK_G2_SHARED_MEM_SIZE=${CONFIG_AMBALINK_G2_SHARED_MEM_SIZE} is too big \033[0m"
		exit 1
	fi
fi

if [ "${CONFIG_ATF_BL32_BASE}" != "" ] && [ "${CONFIG_ATF_BL32_SIZE}" != "" ]; then
	O_C_BL32=`awk "BEGIN {print $CONFIG_ATF_BL32_BASE/1024/1024}"`
	tmp=`awk "BEGIN {print $A53_DDR_END-$CONFIG_ATF_BL32_SHMEM_SIZE}"`
	OPTEE_SHMEM_BASE=`printf "0x%08x\n" ${tmp}`
	O_OPTEE_SHMEM=`awk "BEGIN {print $OPTEE_SHMEM_BASE/1024/1024}"`

	ATF_BL32_BASE=${ATF_BL31_END}
	O_BL32=`awk "BEGIN {print $ATF_BL32_BASE/1024/1024}"`
	tmp=`awk "BEGIN {print $CONFIG_ATF_BL32_BASE+$CONFIG_ATF_BL32_SIZE}"`

	if [ "${CONFIG_FWPROG_XTB_LOADADDR}" != "" ]; then
		tmp=`awk "BEGIN {print $CONFIG_FWPROG_XTB_LOADADDR+$CONFIG_DTB_SIZE}"`
		SYS_LOADADDR=`printf "0x%08x\n" ${tmp}`
	else
		SYS_LOADADDR=`printf "0x%08x\n" ${tmp}`
	fi

	BL32_M=`awk "BEGIN {print $CONFIG_ATF_BL32_SIZE/1024/1024}"`
	OPTEESH_M=`awk "BEGIN {print $CONFIG_ATF_BL32_SHMEM_SIZE/1024/1024}"`

	tmp=`awk "BEGIN {print $A53_DDR_END-$CONFIG_ATF_BL32_SHMEM_SIZE-$CONFIG_PIO_SIZE}"`
	PIO_ADDR=`printf "0x%08x\n" ${tmp}`
	O_PIO=`awk "BEGIN {print $PIO_ADDR/1024/1024}"`
	PIO_M=`awk "BEGIN {print $CONFIG_PIO_SIZE/1024/1024}"`
else
	tmp=`awk "BEGIN {print $A53_DDR_END-$CONFIG_PIO_SIZE}"`
	PIO_ADDR=`printf "0x%08x\n" ${tmp}`
	O_PIO=`awk "BEGIN {print $PIO_ADDR/1024/1024}"`
	PIO_M=`awk "BEGIN {print $CONFIG_PIO_SIZE/1024/1024}"`
fi

O_SYS=`awk "BEGIN {print $SYS_LOADADDR/1024/1024}"`
SYS_M=`awk "BEGIN {print $O_SHM-$O_SYS}"`

config_to_compare="
CONFIG_DDR_SIZE
CONFIG_SHADOW_SHARED_MEM_ADDR
CONFIG_AMBALINK_MEM_SIZE
CONFIG_AMBALINK_SHARED_MEM_ADDR
CONFIG_AMBALINK_SHARED_MEM_SIZE
CONFIG_AMBALINK_G2_SHARED_MEM_ADDR
CONFIG_AMBALINK_G2_SHARED_MEM_SIZE
CONFIG_AMBALINK_G2_RPMSG_SIZE
CONFIG_AMBALINK_G2_RPMSG_NUM
CONFIG_FWPROG_BLD_LOADADDR
CONFIG_FWPROG_R52SYS_LOADADDR
"
if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ] && [ "${r52_config}" != "" ]; then
	eval `grep -i 0x ${r52_config}|grep -E "(TCM|SHADOW)" 2>/dev/null`
fi

echo -e "\033[032m +-----------------------------------+  0x00000000  (0 MB) \033[0m"
echo -e "\033[037m | Reserved                          |     Size: ${CONFIG_ATF_BL31_BASE} \033[0m"
if [ "${CONFIG_FWPROG_ATF_ENABLE}" != "" ]; then
	echo -e "\033[032m +-----------------------------------+  ${CONFIG_ATF_BL31_BASE} \033[0m"
	echo -e "\033[037m | ATF (BL31)                        |     Size: ${CONFIG_ATF_BL31_SIZE} \033[0m"
fi
if [ "${CONFIG_ATF_BL32_BASE}" != "" ] && [ "${CONFIG_ATF_BL32_SIZE}" != "" ]; then
	echo -e "\033[032m +-----------------------------------+  ${ATF_BL32_BASE} (${O_BL32} MB) \033[0m"
	echo -e "\033[037m | OPTEE (BL32)                      |     Size: ${BL32_M} MB \033[0m"
fi

if [ "${CONFIG_FWPROG_XTB_LOADADDR}" != "" ]; then
	echo -e "\033[032m +-----------------------------------+  ${CONFIG_FWPROG_XTB_LOADADDR}  (${O_XTB} MB) \033[0m"
	echo -e "\033[037m | DTB                               |     Size: ${XTB_K} KB \033[0m"
fi

if [ "${CONFIG_THREADX}" != "" ]; then
	echo -e "\033[032m +-----------------------------------+  ${SYS_LOADADDR} (${O_SYS} MB) \033[0m"
	echo -e "\033[037m | A53 OS                            |     Size: ${SYS_M} MB \033[0m"
else
	echo -e "\033[032m +-----------------------------------+  ${CONFIG_FWPROG_LNX_LOADADDR} (${O_LNX} MB) \033[0m"
	echo -e "\033[037m | A53 OS                            |     Size: ${SYS_M} MB \033[0m"
fi

if [ ${LNX_M} -gt 0 ]; then
	echo -e "\033[032m +-----------------------------------+  ${G1_ADDR}  (${O_SHM} MB) \033[0m"
	echo -e "\033[037m | Linux Share memory                |     Size: ${G1_M} MB \033[0m"
#	echo -e "\033[032m +-----------------------------------+  ${DTB_ADDR}  (${O_DTB} MB) \033[0m"
#	echo -e "\033[037m | Linux Ambalink DTB & MMU          |     Size: ${DTB_K} KB \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CONFIG_FWPROG_LNX_LOADADDR}  (${O_LNX} MB) \033[0m"
	echo -e "\033[037m | Linux Kernel                      |     Size: ${LNX_M} MB \033[0m"
fi

if [ "${CONFIG_PIO_MEM_ALLOC}" != "" ]; then
	echo -e "\033[032m +-----------------------------------+  ${PIO_ADDR}  (${O_PIO} MB) \033[0m"
	echo -e "\033[037m | PIO                               |     Size: ${PIO_M} MB \033[0m"
fi

if [ "${CONFIG_ATF_BL32_BASE}" != "" ] && [ "${CONFIG_ATF_BL32_SIZE}" != "" ]; then
	echo -e "\033[032m +-----------------------------------+  ${OPTEE_SHMEM_BASE}  (${O_OPTEE_SHMEM} MB) \033[0m"
	echo -e "\033[037m | OPTEE (BL32) Share memory         |     Size: ${OPTEESH_M} MB \033[0m"
fi

echo -e "\033[032m +-----------------------------------+  ${A53_DDR_END}  (${O_A53DDR} MB) \033[0m"

if [ "${CONFIG_AMBALINK_G2_SHARED_MEM_ADDR}" != "" ]; then
	echo -e "\033[037m | R52 ThreadX                       |     Size: ${R52_M} MB \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${G2_ADDR}  (${O_G2} MB) \033[0m"
	echo -e "\033[037m | R52 Share memory                  |     Size: ${G2_M} MB \033[0m"
	if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ] && [ "${r52_config}" != "" ]; then
		O_SHADOW=`awk "BEGIN {print $CONFIG_SHADOW_SHARED_MEM_ADDR/1024/1024}"`
		O_TCM=`awk "BEGIN {print $CONFIG_TCM_BASE_ADDRESS/1024/1024}"`
		if [ ${O_TCM} -gt ${O_SHADOW} ]; then
			echo -e "\033[037m | Shadow memory base                |  ${CONFIG_SHADOW_SHARED_MEM_ADDR}  (${O_SHADOW} MB) \033[0m"
			echo -e "\033[090m | TCM sram base (R52 OS)            |  ${CONFIG_TCM_BASE_ADDRESS}  (${O_TCM} MB) \033[0m"
		else
			echo -e "\033[090m | TCM sram base (R52 OS)            |  ${CONFIG_TCM_BASE_ADDRESS}  (${O_TCM} MB) \033[0m"
			echo -e "\033[037m | Shadow memory base                |  ${CONFIG_SHADOW_SHARED_MEM_ADDR}  (${O_SHADOW} MB) \033[0m"
		fi
	fi
	echo -e "\033[032m +-----------------------------------+  ${CONFIG_DDR_SIZE}  (${O_DDR} MB) \033[0m"
fi

if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ] && [ "${r52_config}" != "" ]; then
	#sync config
	r52_config_buf=`cat ${r52_config}`
	a53_config_buf=`cat ${a53_config}`
	for cfg in ${config_to_compare}; do
		r52cfg=`echo "${r52_config_buf}"|grep ${cfg} 2>/dev/null`
		a53cfg=`echo "${a53_config_buf}"|grep ${cfg} 2>/dev/null`
		if [ "${r52cfg}" != "" ] || [ "${a53cfg}" != "" ]; then
			if [ "${a53cfg}" != "${r52cfg}" ]; then
				echo "config not sync:"
				echo "	A53: ${a53cfg}"
				echo "	R52: ${r52cfg}"
			fi
		fi
	done
#	config_to_compare=`grep -i 0x ${a53_config}|grep -E "(FWPROG|AMBALINK|ATF|DDR)"|cut -d = -f 1`
#	for cfg in ${config_to_compare}; do
#		r52cfg=`echo "${r52_config_buf}"|grep ${cfg} 2>/dev/null`
#		a53cfg=`echo "${a53_config_buf}"|grep ${cfg} 2>/dev/null`
#		echo "	A53: ${a53cfg}"
#		echo "	R52: ${r52cfg}"
#	done
fi

echo "Loader:"
if [ "${CONFIG_ATF_BL33_BLD_BASE}" != "" ]; then
	if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ] && [ "${r52_config}" != "" ]; then
		O_BLD=`awk "BEGIN {print $CONFIG_FWPROG_BLD_LOADADDR/1024/1024}"`
		echo -e "\033[032m R52BLD (Temporary)                     ${CONFIG_FWPROG_BLD_LOADADDR}  (${O_BLD} MB) \033[0m"
	fi

	O_BL2=`awk "BEGIN {print $CONFIG_ATF_BL2_BASE/1024/1024}"`
	O_BL33_BLD=`awk "BEGIN {print $CONFIG_ATF_BL33_BLD_BASE/1024/1024}"`
	echo -e "\033[032m BL2 BL3-Loader (Temporary)             ${CONFIG_ATF_BL2_BASE}  (${O_BL2} MB) \033[0m"
	echo -e "\033[032m BL33 AmBoot (Temporary)                ${CONFIG_ATF_BL33_BLD_BASE}  (${O_BL33_BLD} MB) \033[0m"
elif [ "${CONFIG_AMBALINK_G2_SHARED_MEM_ADDR}" != "" ]; then
	if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ] && [ "${r52_config}" != "" ]; then
		O_BLD=`awk "BEGIN {print $CONFIG_FWPROG_BLD_LOADADDR/1024/1024}"`
		echo -e "\033[032m R52BLD (Temporary)                     ${CONFIG_FWPROG_BLD_LOADADDR}  (${O_BLD} MB) \033[0m"
	fi
	O_BLD2=`awk "BEGIN {print $CONFIG_FWPROG_BLD2_LOADADDR/1024/1024}"`
	echo -e "\033[032m A53BLD (Temporary)                     ${CONFIG_FWPROG_BLD2_LOADADDR}  (${O_BLD2} MB) \033[0m"
else
	O_BLD=`awk "BEGIN {print $CONFIG_FWPROG_BLD_LOADADDR/1024/1024}"`
	echo -e "\033[032m AmBoot (Temporary)                     ${CONFIG_FWPROG_BLD_LOADADDR}  (${O_BLD} MB) \033[0m"
fi

if [ "${CONFIG_AMBALINK_G2_SHARED_MEM_ADDR}" != "" ] && [ "`echo ${CONFIG_AMBALINK_G2_SHARED_MEM_ADDR}|tr '[A-Z]' '[a-z]'`" != "${G2_ADDR}" ]; then
	echo -e "\033[031m CONFIG_AMBALINK_G2_SHARED_MEM_ADDR should be ${G2_ADDR}, not ${CONFIG_AMBALINK_G2_SHARED_MEM_ADDR} \033[0m"
	exit 1
fi

if [ ${LNX_M} -gt 0 ] && [ "`echo ${CONFIG_AMBALINK_SHARED_MEM_ADDR}|tr '[A-Z]' '[a-z]'`" != "${G1_ADDR}" ]; then
	echo -e "\033[031m CONFIG_AMBALINK_SHARED_MEM_ADDR should be ${G1_ADDR}, not ${CONFIG_AMBALINK_SHARED_MEM_ADDR} \033[0m"
	exit 1
fi

if [ "${CONFIG_FWPROG_LNX_LOADADDR}" != "" ] && [ "`echo ${CONFIG_FWPROG_LNX_LOADADDR}|tr '[A-Z]' '[a-z]'`" != "${LNX_LOADADDR}" ]; then
	if [ "`echo ${CONFIG_FWPROG_LNX_LOADADDR}|tr '[A-Z]' '[a-z]'`" != "${LNX_2MBADDR}" ]; then
		echo -e "\033[031m CONFIG_FWPROG_LNX_LOADADDR should be 5.10 ${LNX_2MBADDR} or 5.4 ${LNX_LOADADDR}, not ${CONFIG_FWPROG_LNX_LOADADDR} \033[0m"
		exit 1
	fi
fi

if [ "${CONFIG_ATF_BL32_BASE}" != "" ] && [ "${CONFIG_ATF_BL32_SIZE}" != "" ]; then
	if [ "`echo ${CONFIG_ATF_BL32_BASE}|tr '[A-Z]' '[a-z]'`" != "${ATF_BL32_BASE}" ]; then
		echo -e "\033[031m CONFIG_ATF_BL32_BASE should be ${ATF_BL32_BASE}, not ${CONFIG_ATF_BL32_BASE} \033[0m"
		exit 1
	fi
fi

if [ "${CONFIG_THREADX}" != "" ]; then
	if [ "`echo ${CONFIG_FWPROG_SYS_LOADADDR}|tr '[A-Z]' '[a-z]'`" != "${SYS_LOADADDR}" ]; then
		echo -e "\033[031m CONFIG_FWPROG_SYS_LOADADDR should be ${SYS_LOADADDR}, not ${CONFIG_FWPROG_SYS_LOADADDR} \033[0m"
		exit 1
	fi
fi

if [ "${CONFIG_PIO_MEM_ALLOC}" != "" ] && [ "`echo ${CONFIG_PIO_BASE}|tr '[A-Z]' '[a-z]'`" != "${PIO_ADDR}" ]; then
	echo -e "\033[031m CONFIG_PIO_BASE should be ${PIO_ADDR}, not ${CONFIG_PIO_BASE} \033[0m"
	exit 1
fi

if [ `echo ${SYS_M}|cut -d . -f 1` -le 0 ]; then
	echo -e "\033[031m not enough memory for ThreadX \033[0m"
	exit 1
fi
