#!/bin/bash

#O_x      offset in MB
#x_M      size in MB
#x_ADDR   addr in 0xabcdabcd

# ================ Import .config and *nm ================
if [ "$1" != "" ]; then
	srctree=$1
	objtree=$2

	CONFIG_PATH=$objtree/.config
	DTB_PATH=$objtree/$3
else
	CONFIG_PATH=output/.config
	DTB_PATH=sdk8.dtb
	CFG_TEST=1
fi

if [ ! -e ${CONFIG_PATH} ]; then
	exit 0
fi
source ${CONFIG_PATH}

if [ "$1" != "" ] && [ "${CONFIG_BUILD_QNX_SYS_IMAGE}" == "" ] && [ "${CONFIG_THREADX}" != "" ]; then
	NM_PATH=`find $srctree/output/out/ -name '*.nm'|awk '!/amba_img.nm/'|head -n 1`
fi
echo -e "\033[037m CONFIG_PATH(${CONFIG_PATH}) \033[0m"
echo -e "\033[037m DTB_PATH(${DTB_PATH}) \033[0m"
echo -e "\033[037m NM_PATH(${NM_PATH}) \033[0m"
# ================  Funtion ================
parse_from_nm(){
	if [ "`grep $1 ${NM_PATH}`" != "" ]; then
		echo 0x`grep $1 $NM_PATH | cut -d " " -f1`
	fi
}

check_from_nm(){
	if [ "`grep $1 ${NM_PATH}`" == "" ]; then
		return 0;
	else
		return 1;
	fi
}

# ================  DRAM variable ================
if [ "${CONFIG_DDR_SIZE}" != "" ]; then
	tmp=0
	CFG_DRAM_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DDR_SIZE}"`
	CFG_DRAM_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_MEM_BASE+$CFG_DRAM_MEM_SIZE}"`
	CFG_DRAM_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_MEM_BASE}"`
	CFG_DRAM_TOP_DOWN_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_MEM_END}"`
	CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
	
	if [ "${CONFIG_ATT_8GB}" == "" ] && [ $((CFG_DRAM_BOTTOM_UP_BASE)) -gt $((0x100000000)) ]; then
		tmp=0x100000000
	else
		tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE}"`
	fi
	CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
else
	echo -e "\033[031m no CONFIG_DDR_SIZE symbol \033[0m"
	exit 1
fi
# ================  ATF variable ================
if [ "${CONFIG_ATF_BL31_BASE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_ATF_BL31_BASE}"`
	CFG_ATF_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_ATF_BL31_SIZE}"`
	CFG_ATF_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_ATF_MEM_BASE+$CFG_ATF_MEM_SIZE}"`
	CFG_ATF_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_FWPROG_ATF_LOADADDR}"`
	CFG_ATF_MEM_LOAD=`printf "0x%010X\n" ${tmp}`

	if [ "${CONFIG_ATF_HAVE_BL2}" == "" ] && [ "${CONFIG_FWPROG_ATF_LOADADDR}" != "" ]; then
		# Check load
		if [ $((CFG_ATF_MEM_BASE)) -ne $((CFG_ATF_MEM_LOAD)) ]; then
			echo -e "\033[031m CFG_ATF_MEM_BASE($CFG_ATF_MEM_BASE) != CFG_ATF_MEM_LOAD($CFG_ATF_MEM_LOAD) \033[0m"
			exit 1
		fi
	fi

	# Check range
	if [ $((CFG_DRAM_TOP_DOWN_BASE)) -gt $((CFG_ATF_MEM_BASE)) ]; then
		echo -e "\033[031m CFG_DRAM_TOP_DOWN_BASE($CFG_DRAM_TOP_DOWN_BASE) > CFG_ATF_MEM_BASE($CFG_ATF_MEM_BASE) \033[0m"
		exit 1
	else
		# Shift CFG_DRAM_TOP_DOWN_BASE
		tmp=`awk "BEGIN {print $CFG_ATF_MEM_END}"`
		CFG_DRAM_TOP_DOWN_BASE=`printf "0x%010X\n" ${tmp}`
	fi
else
	echo -e "\033[031m no CONFIG_ATF_BL31_BASE symbol \033[0m"
	exit 1
fi
# ================  OPTEE variable ================
if [ "${CONFIG_ATF_BL32_BASE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_ATF_BL32_BASE}"`
	CFG_OPTEE_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_ATF_BL32_SIZE}"`
	CFG_OPTEE_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_OPTEE_MEM_BASE+$CFG_OPTEE_MEM_SIZE}"`
	CFG_OPTEE_MEM_END=`printf "0x%010X\n" ${tmp}`

	# Check range
	if [ $((CFG_DRAM_TOP_DOWN_BASE)) -gt $((CFG_OPTEE_MEM_BASE)) ]; then
		echo -e "\033[031m CFG_ATF_MEM_END($CFG_DRAM_TOP_DOWN_BASE) > CFG_OPTEE_MEM_BASE($CFG_OPTEE_MEM_BASE) \033[0m"
		exit 1
	else
		# Shift CFG_DRAM_TOP_DOWN_BASE
		tmp=`awk "BEGIN {print $CFG_OPTEE_MEM_END}"`
		CFG_DRAM_TOP_DOWN_BASE=`printf "0x%010X\n" ${tmp}`
	fi
else
	tmp=0
	CFG_OPTEE_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_OPTEE_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_OPTEE_MEM_END=`printf "0x%010X\n" ${tmp}`
fi
# ================  XTB variable ================
CFG_XTB_MEM_NAME="xtb"
if [ "${CONFIG_DTB_LOADADDR}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_DTB_LOADADDR}"`
	CFG_XTB_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DTB_SIZE}"`
	CFG_XTB_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_XTB_MEM_BASE+$CFG_XTB_MEM_SIZE}"`
	CFG_XTB_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_FWPROG_XTB_LOADADDR}"`
	CFG_XTB_MEM_LOAD=`printf "0x%010X\n" ${tmp}`

	# Check load
	if [ $((CFG_XTB_MEM_BASE)) -ne $((CFG_XTB_MEM_LOAD)) ]; then
		echo -e "\033[031m CFG_XTB_MEM_BASE($CFG_XTB_MEM_BASE) != CFG_XTB_MEM_LOAD($CFG_XTB_MEM_LOAD) \033[0m"
		exit 1
	fi

	# Check range
	if [ $((CFG_DRAM_TOP_DOWN_BASE)) -gt $((CFG_XTB_MEM_BASE)) ]; then
		echo -e "\033[031m CFG_DRAM_TOP_DOWN_BASE($CFG_DRAM_TOP_DOWN_BASE) > CFG_XTB_MEM_BASE($CFG_XTB_MEM_BASE) \033[0m"
		exit 1
	else
		# Shift CFG_DRAM_TOP_DOWN_BASE
		tmp=`awk "BEGIN {print $CFG_XTB_MEM_END}"`
		CFG_DRAM_TOP_DOWN_BASE=`printf "0x%010X\n" ${tmp}`
	fi

	if [ "${CONFIG_LINUX}" != "" ]; then
		# Check alignment
		tmp=`awk "BEGIN {print $((CFG_XTB_MEM_BASE&~0x00001FFFFF))}"`
		CFG_XTB_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_XTB_MEM_BASE)) -ne $((CFG_XTB_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_XTB_MEM_BASE${CFG_XTB_MEM_BASE} CFG_XTB_MEM_ALIGN${CFG_XTB_MEM_ALIGN} should 2MB align \033[0m"
			exit 1
		fi
	fi
else
	tmp=0
	CFG_XTB_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_XTB_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_XTB_MEM_END=`printf "0x%010X\n" ${tmp}`
fi
# ================  R52 SYS variable ================
if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_FWPROG_R52SYS_LOADADDR}"`
	CFG_R52SYS_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_MEM_END-$CFG_R52SYS_MEM_BASE}"`
	CFG_R52SYS_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_MEM_END}"`
	CFG_R52SYS_MEM_END=`printf "0x%010X\n" ${tmp}`

	# Check range
	if [ $((CFG_DRAM_BOTTOM_UP_BASE)) -lt $((CFG_R52SYS_MEM_END)) ]; then
		echo -e "\033[031m CFG_DRAM_BOTTOM_UP_BASE($CFG_DRAM_BOTTOM_UP_BASE) < CFG_R52SYS_MEM_END($CFG_R52SYS_MEM_END) \033[0m"
		exit 1
	else
		# Shift CFG_DRAM_BOTTOM_UP_BASE
		tmp=`awk "BEGIN {print $CFG_R52SYS_MEM_BASE}"`
		CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
	fi
else
	tmp=0
	CFG_R52SYS_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_R52SYS_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_R52SYS_MEM_END=`printf "0x%010X\n" ${tmp}`
fi
# ================  OPTEE SHM variable ================
if [ "${CONFIG_ATF_BL32_SHMEM_SIZE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_ATF_BL32_SHMEM_SIZE}"`
	CFG_OPTEE_SHM_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE-$CFG_OPTEE_SHM_MEM_SIZE}"`
	CFG_OPTEE_SHM_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE}"`
	CFG_OPTEE_SHM_MEM_END=`printf "0x%010X\n" ${tmp}`

	# Shift CFG_DRAM_BOTTOM_UP_BASE
	tmp=`awk "BEGIN {print $CFG_OPTEE_SHM_MEM_BASE}"`
	CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
else
	tmp=0
	CFG_OPTEE_SHM_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_OPTEE_SHM_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_OPTEE_SHM_MEM_END=`printf "0x%010X\n" ${tmp}`
fi
# ================  RPMSG G2 variable ================
if [ "${CONFIG_AMBALINK_RPMSG_G2}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_AMBALINK_G2_SHARED_MEM_ADDR}"`
	CFG_RPMSG_G2_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_AMBALINK_G2_SHARED_MEM_SIZE}"`
	CFG_RPMSG_G2_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_RPMSG_G2_MEM_BASE+$CFG_RPMSG_G2_MEM_SIZE}"`
	CFG_RPMSG_G2_MEM_END=`printf "0x%010X\n" ${tmp}`

	# Check range
	if [ $((CFG_R52SYS_MEM_BASE)) -gt $((CFG_RPMSG_G2_MEM_BASE)) ]; then
		echo -e "\033[031m CFG_R52SYS_MEM_BASE($CFG_R52SYS_MEM_BASE) > CFG_RPMSG_G2_MEM_BASE($CFG_RPMSG_G2_MEM_BASE) \033[0m"
		exit 1
	fi

	# Check range
	if [ $((CFG_R52SYS_MEM_END)) -lt $((CFG_RPMSG_G2_MEM_END)) ]; then
		echo -e "\033[031m CFG_R52SYS_MEM_END($CFG_R52SYS_MEM_END) > CFG_RPMSG_G2_MEM_END($CFG_RPMSG_G2_MEM_END) \033[0m"
		exit 1
	fi
else
	tmp=0
	CFG_RPMSG_G2_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_RPMSG_G2_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_RPMSG_G2_MEM_END=`printf "0x%010X\n" ${tmp}`
fi

# ================  PIO variable ================
if [ "${CONFIG_PIO_SIZE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_PIO_SIZE}"`
	CFG_PIO_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	CFG_PIO_MEM_END=$CFG_DRAM_BOTTOM_UP_BASE

	tmp=`awk "BEGIN {print $CFG_PIO_MEM_END-$CFG_PIO_MEM_SIZE}"`
	CFG_PIO_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_PIO_BASE}"`
	CFG_PIO_MEM_CHECK_BASE=`printf "0x%010X\n" ${tmp}`
	# Check range
	if [ $((CFG_PIO_MEM_BASE)) -ne $((CFG_PIO_MEM_CHECK_BASE)) ]; then
		echo -e "\033[031m CONFIG_PIO_BASE should be ${CFG_PIO_MEM_BASE}, not ${CFG_PIO_MEM_CHECK_BASE} \033[0m"
	#	exit 1
	fi
	CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${CFG_PIO_MEM_BASE}`
else
	tmp=0
	CFG_PIO_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_PIO_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_PIO_MEM_END=`printf "0x%010X\n" ${tmp}`
fi

# ================  DSP variable ================
if [ "${CONFIG_BUILD_DSP_VISUAL}" != "" ] && [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" -a "${CONFIG_ATT_8GB}" != "" ]; then
	# DSP total size
	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE + $CONFIG_DSP_BIN_BUF_SIZE + $CONFIG_DSP_DATA_BUF_SIZE}"`
	CONFIG_DSP_TOTAL_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE - $CONFIG_DSP_TOTAL_BUF_SIZE}"`
	CONFIG_DSP_TOTAL_BUF_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE + $CONFIG_DSP_TOTAL_BUF_SIZE}"`
	CONFIG_DSP_TOTAL_BUF_END=`printf "0x%010X\n" ${tmp}`

	# DSP PROT size
	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE}"`
	CONFIG_DSP_PROT_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE}"`
	CONFIG_DSP_PROT_BUF_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_BASE + $CONFIG_DSP_PROT_BUF_SIZE}"`
	CONFIG_DSP_PROT_BUF_END=`printf "0x%010X\n" ${tmp}`

	# DSP PROT CACHED size
	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
	CONFIG_DSP_PROT_CACHE_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_END}"`
	CONFIG_DSP_PROT_CACHE_BUF_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_BASE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
	CONFIG_DSP_PROT_CACHE_BUF_END=`printf "0x%010X\n" ${tmp}`

	# DSP BIN size
	tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_SIZE}"`
	CONFIG_DSP_BIN_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_END}"`
	CONFIG_DSP_BIN_BUF_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_BASE + $CONFIG_DSP_BIN_BUF_SIZE}"`
	CONFIG_DSP_BIN_BUF_END=`printf "0x%010X\n" ${tmp}`

	# DSP DATA size
	tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_SIZE}"`
	CONFIG_DSP_DATA_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_END}"`
	CONFIG_DSP_DATA_BUF_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_BASE + $CONFIG_DSP_DATA_BUF_SIZE}"`
	CONFIG_DSP_DATA_BUF_END=`printf "0x%010X\n" ${tmp}`

	# Shift CFG_DRAM_BOTTOM_UP_BASE
	tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE}"`
	CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
else
	tmp=0
	CONFIG_DSP_TOTAL_BUF_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CONFIG_DSP_TOTAL_BUF_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CONFIG_DSP_TOTAL_BUF_END=`printf "0x%010X\n" ${tmp}`
fi

# ================  Ambalink variable ================
if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_AMBALINK_SHARED_MEM_ADDR+$CONFIG_AMBALINK_SHARED_MEM_SIZE}"`
	CFG_AMBALINK_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_AMBALINK_MEM_SIZE-$CONFIG_AMBALINK_SHARED_MEM_SIZE}"`
	CFG_AMBALINK_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_AMBALINK_MEM_BASE+$CFG_AMBALINK_MEM_SIZE}"`
	CFG_AMBALINK_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_AMBALINK_MEM_BASE+0x0004000000}"`
	CFG_AMBALINK_RSV_MEM_END=`printf "0x%010X\n" ${tmp}`

	if [ "${CONFIG_AMBALINK_BOOT_OS_LEGACY_5_7}" != "" ]; then
		tmp=`awk "BEGIN {print $CONFIG_FWPROG_LNX_LOADADDR-0x0000080000}"`
	else # Linux v5.8 and later
		# Image must be placed at 2MB alignment
		tmp=$(($CONFIG_FWPROG_LNX_LOADADDR & 0x00001FFFFF))
		if [ $tmp -gt 0 ]; then
			echo -e "\033[031m CONFIG_FWPROG_LNX_LOADADDR($CONFIG_FWPROG_LNX_LOADADDR) must be 2MB alignment \033[0m"
			exit 1
		fi
		tmp=`awk "BEGIN {print $CONFIG_FWPROG_LNX_LOADADDR}"`
	fi
	CFG_AMBALINK_MEM_LOAD=`printf "0x%010X\n" ${tmp}`

	# Check load
	if [ $((CFG_AMBALINK_MEM_BASE)) -ne $((CFG_AMBALINK_MEM_LOAD)) ]; then
		echo -e "\033[031m CFG_AMBALINK_MEM_BASE($CFG_AMBALINK_MEM_BASE) != CFG_AMBALINK_MEM_LOAD($CFG_AMBALINK_MEM_LOAD) \033[0m"
		exit 1
	fi

	# Check range
	if [ $((CFG_DRAM_BOTTOM_UP_BASE)) -lt $((CFG_AMBALINK_MEM_END)) ]; then
		echo -e "\033[031m CFG_DRAM_BOTTOM_UP_BASE($CFG_DRAM_BOTTOM_UP_BASE) < CFG_AMBALINK_MEM_END($CFG_AMBALINK_MEM_END) \033[0m"
		exit 1
	else
		# Shift CFG_DRAM_BOTTOM_UP_BASE
		tmp=`awk "BEGIN {print $CFG_AMBALINK_MEM_BASE}"`
		CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
	fi

	# Check lds
	if [ "${NM_PATH}" != "" ]; then
		   tmp=$(parse_from_nm __a53_shm_end)
			if [ "${tmp}" != "" ]; then
				CFG_AMBALINK_SYMBOL_START=`printf "0x%010X\n" ${tmp}`
				# Check range
				if [ $((CFG_AMBALINK_MEM_BASE)) -ne $((CFG_AMBALINK_SYMBOL_START)) ]; then
					echo -e "\033[031m CFG_AMBALINK_MEM_BASE($CFG_AMBALINK_MEM_BASE) != CFG_AMBALINK_SYMBOL_START($CFG_AMBALINK_SYMBOL_START) \033[0m"
					exit 1
				fi
			fi
	fi
else
	tmp=0
	CFG_AMBALINK_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_AMBALINK_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_AMBALINK_MEM_END=`printf "0x%010X\n" ${tmp}`
fi
# ================  RPMSG G1 variable ================
if [ "${CONFIG_AMBALINK_RPMSG_G1}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_AMBALINK_SHARED_MEM_ADDR}"`
	CFG_RPMSG_G1_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_AMBALINK_SHARED_MEM_SIZE}"`
	CFG_RPMSG_G1_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_RPMSG_G1_MEM_BASE+$CFG_RPMSG_G1_MEM_SIZE}"`
	CFG_RPMSG_G1_MEM_END=`printf "0x%010X\n" ${tmp}`

	# Check range
	if [ $((CFG_DRAM_BOTTOM_UP_BASE)) -lt $((CFG_RPMSG_G1_MEM_END)) ]; then
		echo -e "\033[031m CFG_DRAM_BOTTOM_UP_BASE($CFG_DRAM_BOTTOM_UP_BASE) < CFG_RPMSG_G1_MEM_END($CFG_RPMSG_G1_MEM_END) \033[0m"
		exit 1
	else
		# Shift CFG_DRAM_BOTTOM_UP_BASE
		tmp=`awk "BEGIN {print $CFG_RPMSG_G1_MEM_BASE}"`
		CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`
	fi

	if [ "${NM_PATH}" != "" ]; then
		tmp=$(parse_from_nm __linux_start)
		CFG_LINUX_SYMBOL_START=`printf "0x%010X\n" ${tmp}`
		# Check range
		if [ $((CFG_RPMSG_G1_MEM_BASE)) -ne $((CFG_LINUX_SYMBOL_START)) ]; then
			echo -e "\033[031m CFG_RPMSG_G1_MEM_BASE($CFG_RPMSG_G1_MEM_BASE) != CFG_LINUX_SYMBOL_START($CFG_LINUX_SYMBOL_START) \033[0m"
			exit 1
		fi
	fi
else
	tmp=0
	CFG_RPMSG_G1_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_RPMSG_G1_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_RPMSG_G1_MEM_END=`printf "0x%010X\n" ${tmp}`
fi

# ================  APP variable ================
if [ "${CONFIG_APP_MEM_SIZE}" != "" ]; then
	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE}"`
	CFG_APP_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_APP_MEM_SIZE}"`
	CFG_APP_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE-$CONFIG_APP_MEM_SIZE}"`
	CFG_APP_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	# Shift CFG_DRAM_BOTTOM_UP_BASE
	tmp=`awk "BEGIN {print $CFG_APP_MEM_BASE}"`
	CFG_DRAM_BOTTOM_UP_BASE=`printf "0x%010X\n" ${tmp}`

	if [ "${NM_PATH}" != "" ]; then
		tmp=$(parse_from_nm __app_start)
		CFG_APP_SYMBOL_START=`printf "0x%010X\n" ${tmp}`
		# Check range
		if [ $((CFG_APP_MEM_BASE)) -ne $((CFG_APP_SYMBOL_START)) ]; then
			echo -e "\033[031m CFG_APP_MEM_BASE($CFG_APP_MEM_BASE) != CFG_APP_SYMBOL_START($CFG_APP_SYMBOL_START) \033[0m"
			exit 1
		fi
	fi
else
	tmp=0
	CFG_APP_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_APP_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_APP_MEM_END=`printf "0x%010X\n" ${tmp}`
fi

# ================  SDK variable ================
if [ "${CONFIG_BUILD_QNX_SYS_IMAGE}" != "" ]; then
	tmp=`awk "BEGIN {print $CFG_DRAM_TOP_DOWN_BASE}"`
	CFG_SDK_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE}"`
	CFG_SDK_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_SDK_MEM_END-$CFG_SDK_MEM_BASE}"`
	CFG_SDK_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_SDK_MEM_BASE+0x0004000000}"`
	CFG_SDK_RSV_MEM_END=`printf "0x%010X\n" ${tmp}`
elif [ "${CONFIG_THREADX}" != "" ]; then
	tmp=`awk "BEGIN {print $CONFIG_FWPROG_SYS_LOADADDR}"`
	CFG_SDK_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE}"`
	CFG_SDK_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_SDK_MEM_END-$CFG_SDK_MEM_BASE}"`
	CFG_SDK_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	if [ "${NM_PATH}" == "" ]; then
		tmp=`awk "BEGIN {print $CFG_SDK_MEM_BASE+0x0004000000}"`
		CFG_SDK_RSV_MEM_END=`printf "0x%010X\n" ${tmp}`
	else
		tmp=$(parse_from_nm __ddr_region0_ro_start)
		CFG_SDK_RSV_MEM_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=$(parse_from_nm __non_cache_heap_end)
		CFG_SDK_RSV_MEM_END=`printf "0x%010X\n" ${tmp}`

		# Check load
		if [ $((CFG_SDK_MEM_BASE)) -ne $((CFG_SDK_RSV_MEM_BASE)) ]; then
			echo -e "\033[031m CFG_SDK_MEM_BASE($CFG_SDK_MEM_BASE) != CFG_SDK_RSV_MEM_BASE($CFG_SDK_RSV_MEM_BASE) \033[0m"
			exit 1
		fi
	fi
elif [ "${CONFIG_LINUX}" != "" ]; then
	tmp=`awk "BEGIN {print $CFG_DRAM_TOP_DOWN_BASE}"`
	CFG_SDK_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_DRAM_BOTTOM_UP_BASE}"`
	CFG_SDK_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CONFIG_FWPROG_LNX_LOADADDR}"`
	CFG_SDK_MEM_LOAD=`printf "0x%010X\n" ${tmp}`
	# Check load
	tmp=$(($CONFIG_FWPROG_LNX_LOADADDR & 0x00001FFFFF))
	# Linux v5.8 and later, Image must be placed at 2MB alignment, otherwise check it.
	if [ $tmp -gt 0 ]; then
		if [ $((CFG_SDK_MEM_BASE)) -ne $((CFG_SDK_MEM_LOAD)) ]; then
			echo -e "\033[031m CFG_SDK_MEM_BASE($CFG_SDK_MEM_BASE) != CFG_SDK_MEM_LOAD($CFG_SDK_MEM_LOAD) \033[0m"
			exit 1
		fi
	else
		if [ "$(($CFG_XTB_MEM_END <= $CFG_SDK_MEM_LOAD))" != "1" ]; then
			echo -e "\033[031m CFG_XTB_MEM_END($CFG_XTB_MEM_END) > CFG_SDK_MEM_LOAD($CFG_SDK_MEM_LOAD) \033[0m"
			exit 1
		fi
		CFG_SDK_MEM_BASE=$CFG_SDK_MEM_LOAD
	fi

	tmp=`awk "BEGIN {print $CFG_SDK_MEM_END-$CFG_SDK_MEM_BASE}"`
	CFG_SDK_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_SDK_MEM_BASE+0x0004000000}"`
	CFG_SDK_RSV_MEM_END=`printf "0x%010X\n" ${tmp}`
else
	echo -e "\033[031m no support OS \033[0m"
	exit 1
fi

# Check range
if [ $((CFG_DRAM_TOP_DOWN_BASE)) -gt $((CFG_SDK_MEM_BASE)) ]; then
	echo -e "\033[031m CFG_DRAM_TOP_DOWN_BASE($CFG_DRAM_TOP_DOWN_BASE) > CFG_SDK_MEM_BASE($CFG_SDK_MEM_BASE) \033[0m"
	exit 1
fi

# Check range
if [ $((CFG_DRAM_BOTTOM_UP_BASE)) -lt $((CFG_SDK_MEM_END)) ]; then
	echo -e "\033[031m CFG_DRAM_BOTTOM_UP_BASE($CFG_DRAM_BOTTOM_UP_BASE) < CFG_SDK_MEM_END($CFG_SDK_MEM_END) \033[0m"
	exit 1
fi

# ================  DSP variable ================
if [ "${CONFIG_BUILD_DSP_VISUAL}" != "" ] && [ "${CONFIG_AMBALINK_MEM_SIZE}" == "" -o "${CONFIG_ATT_8GB}" == "" ]; then
	if [ "${CONFIG_BUILD_QNX_SYS_IMAGE}" != "" ]; then
		# DSP total size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE + $CONFIG_DSP_BIN_BUF_SIZE + $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CFG_SDK_MEM_END - $CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE + $CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP PROT size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE}"`
		CONFIG_DSP_PROT_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE}"`
		CONFIG_DSP_PROT_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_BASE + $CONFIG_DSP_PROT_BUF_SIZE}"`
		CONFIG_DSP_PROT_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP PROT CACHED size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
		CONFIG_DSP_PROT_CACHE_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_END}"`
		CONFIG_DSP_PROT_CACHE_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_BASE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
		CONFIG_DSP_PROT_CACHE_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP BIN size
		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_SIZE}"`
		CONFIG_DSP_BIN_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_END}"`
		CONFIG_DSP_BIN_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_BASE + $CONFIG_DSP_BIN_BUF_SIZE}"`
		CONFIG_DSP_BIN_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP DATA size
		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_DATA_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_END}"`
		CONFIG_DSP_DATA_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_BASE + $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_DATA_BUF_END=`printf "0x%010X\n" ${tmp}`
	elif [ "${CONFIG_THREADX}" != "" ]; then
		# DSP total size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE + $CONFIG_DSP_BIN_BUF_SIZE + $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CFG_SDK_MEM_END - $CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE + $CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP PROT size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE}"`
		CONFIG_DSP_PROT_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE}"`
		CONFIG_DSP_PROT_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_BASE + $CONFIG_DSP_PROT_BUF_SIZE}"`
		CONFIG_DSP_PROT_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP PROT CACHED size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
		CONFIG_DSP_PROT_CACHE_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_END}"`
		CONFIG_DSP_PROT_CACHE_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_BASE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
		CONFIG_DSP_PROT_CACHE_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP BIN size
		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_SIZE}"`
		CONFIG_DSP_BIN_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_END}"`
		CONFIG_DSP_BIN_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_BASE + $CONFIG_DSP_BIN_BUF_SIZE}"`
		CONFIG_DSP_BIN_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP DATA size
		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_DATA_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_END}"`
		CONFIG_DSP_DATA_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_BASE + $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_DATA_BUF_END=`printf "0x%010X\n" ${tmp}`
	elif [ "${CONFIG_LINUX}" != "" ]; then
		# DSP total size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE + $CONFIG_DSP_BIN_BUF_SIZE + $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CFG_SDK_MEM_END - $CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE + $CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CONFIG_DSP_TOTAL_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP PROT size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_SIZE}"`
		CONFIG_DSP_PROT_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_TOTAL_BUF_BASE}"`
		CONFIG_DSP_PROT_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_BASE + $CONFIG_DSP_PROT_BUF_SIZE}"`
		CONFIG_DSP_PROT_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP PROT CACHED size
		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
		CONFIG_DSP_PROT_CACHE_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_BUF_END}"`
		CONFIG_DSP_PROT_CACHE_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_BASE + $CONFIG_DSP_PROT_CACHE_BUF_SIZE}"`
		CONFIG_DSP_PROT_CACHE_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP BIN size
		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_SIZE}"`
		CONFIG_DSP_BIN_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_PROT_CACHE_BUF_END}"`
		CONFIG_DSP_BIN_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_BASE + $CONFIG_DSP_BIN_BUF_SIZE}"`
		CONFIG_DSP_BIN_BUF_END=`printf "0x%010X\n" ${tmp}`

		# DSP DATA size
		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_DATA_BUF_SIZE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_BIN_BUF_END}"`
		CONFIG_DSP_DATA_BUF_BASE=`printf "0x%010X\n" ${tmp}`

		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_BASE + $CONFIG_DSP_DATA_BUF_SIZE}"`
		CONFIG_DSP_DATA_BUF_END=`printf "0x%010X\n" ${tmp}`
	else
		echo -e "\033[031m no support OS \033[0m"
		exit 1
	fi
fi

# ================  CV variable ================
if [ "${CONFIG_BUILD_CV}" != "" ]; then
	# cv size
	tmp=`awk "BEGIN {print $CONFIG_CV_MEM_SIZE}"`
	CFG_CV_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
		tmp=`awk "BEGIN {print $CFG_AMBALINK_MEM_END-$CFG_CV_MEM_SIZE}"`
		CFG_CV_MEM_BASE=`printf "0x%010X\n" ${tmp}`

		# Check range
		if [ $((CFG_AMBALINK_RSV_MEM_END)) -gt $((CFG_CV_MEM_BASE)) ]; then
			echo -e "\033[031m CFG_AMBALINK_RSV_MEM_END($CFG_AMBALINK_RSV_MEM_END) > CFG_CV_MEM_BASE($CFG_CV_MEM_BASE) \033[0m"
			exit 1
		fi
	else
		tmp=`awk "BEGIN {print $CFG_SDK_MEM_END-$CFG_CV_MEM_SIZE-$CONFIG_DSP_TOTAL_BUF_SIZE}"`
		CFG_CV_MEM_BASE=`printf "0x%010X\n" ${tmp}`

		# Check range
		if [ $((CFG_SDK_RSV_MEM_END)) -gt $((CFG_CV_MEM_BASE)) ]; then
			echo -e "\033[031m CFG_SDK_RSV_MEM_END($CFG_SDK_RSV_MEM_END) > CFG_CV_MEM_BASE($CFG_CV_MEM_BASE) \033[0m"
			exit 1
		fi
	fi

	tmp=`awk "BEGIN {print $CFG_CV_MEM_BASE+$CFG_CV_MEM_SIZE}"`
	CFG_CV_MEM_END=`printf "0x%010X\n" ${tmp}`


	# cv schdr, cv linux user, cv rtos user
	tmp=`awk "BEGIN {print $CONFIG_CV_MEM_SCHDR_SIZE}"`
	CFG_CV_SCHDR_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
		tmp=`awk "BEGIN {print $CONFIG_CV_MEM_RTOS_SIZE}"`
		CFG_CV_RTOS_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	else
		tmp=0
		CFG_CV_RTOS_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	fi

	# Check range
	if [ $((CFG_CV_MEM_SIZE)) -lt $((CFG_CV_RTOS_MEM_SIZE+CFG_CV_SCHDR_MEM_SIZE)) ]; then
		echo -e "\033[031m CFG_CV_MEM_SIZE($CFG_CV_MEM_SIZE) < CFG_CV_RTOS_MEM_SIZE($CFG_CV_RTOS_MEM_SIZE)+CFG_CV_SCHDR_MEM_SIZE($CFG_CV_SCHDR_MEM_SIZE)\033[0m"
		exit 1
	fi

	tmp=`awk "BEGIN {print $CFG_CV_MEM_BASE+$CFG_CV_SCHDR_MEM_SIZE}"`
	CFG_CV_USER_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_CV_MEM_SIZE-$CFG_CV_RTOS_MEM_SIZE-$CFG_CV_SCHDR_MEM_SIZE}"`
	CFG_CV_USER_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_CV_SCHDR_MEM_SIZE+$CFG_CV_MEM_BASE}"`
	CFG_CV_SCHDR_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_CV_USER_MEM_SIZE+$CFG_CV_SCHDR_MEM_SIZE+$CFG_CV_MEM_BASE}"`
	CFG_CV_SYS_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=`awk "BEGIN {print $CFG_CV_USER_MEM_SIZE+$CFG_CV_SCHDR_MEM_SIZE}"`
	CFG_CV_SYS_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ] || [ "${CONFIG_LINUX}" != "" ]; then
		# Check alignment 4MB
		tmp=`awk "BEGIN {print $((CFG_CV_MEM_BASE&~0x00003FFFFF))}"`
		CFG_CV_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_CV_MEM_BASE)) -ne $((CFG_CV_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_CV_MEM_BASE${CFG_CV_MEM_BASE} should 4MB align \033[0m"
			exit 1
		fi

		tmp=`awk "BEGIN {print $((CFG_CV_MEM_SIZE&~0x00003FFFFF))}"`
		CFG_CV_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_CV_MEM_SIZE)) -ne $((CFG_CV_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_CV_MEM_SIZE${CFG_CV_MEM_SIZE} should 4MB align \033[0m"
			exit 1
		fi

		tmp=`awk "BEGIN {print $((CFG_CV_SYS_MEM_SIZE&~0x00003FFFFF))}"`
		CFG_CV_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_CV_SYS_MEM_SIZE)) -ne $((CFG_CV_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_CV_SYS_MEM_SIZE${CFG_CV_SYS_MEM_SIZE} should 4MB align \033[0m"
			exit 1
		fi
	elif [ "${CONFIG_THREADX64}" != "" ]; then
		# Check alignment 2MB
		tmp=`awk "BEGIN {print $((CFG_CV_MEM_BASE&~0x00001FFFFF))}"`
		CFG_CV_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_CV_MEM_BASE)) -ne $((CFG_CV_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_CV_MEM_BASE${CFG_CV_MEM_BASE} should 2MB align \033[0m"
			exit 1
		fi

		tmp=`awk "BEGIN {print $((CFG_CV_MEM_SIZE&~0x00001FFFFF))}"`
		CFG_CV_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_CV_MEM_SIZE)) -ne $((CFG_CV_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_CV_MEM_SIZE${CFG_CV_MEM_SIZE} should 2MB align \033[0m"
			exit 1
		fi

		tmp=`awk "BEGIN {print $((CFG_CV_SYS_MEM_SIZE&~0x00001FFFFF))}"`
		CFG_CV_MEM_ALIGN=`printf "0x%010X\n" ${tmp}`
		if [ $((CFG_CV_SYS_MEM_SIZE)) -ne $((CFG_CV_MEM_ALIGN)) ]; then
			echo -e "\033[031m CFG_CV_SYS_MEM_SIZE${CFG_CV_SYS_MEM_SIZE} should 2MB align \033[0m"
			exit 1
		fi
	fi
else
	tmp=0
	CFG_CV_MEM_BASE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_CV_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
	tmp=0
	CFG_CV_MEM_END=`printf "0x%010X\n" ${tmp}`

	tmp=0
	CFG_CV_SCHDR_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=0
	CFG_CV_USER_MEM_BASE=`printf "0x%010X\n" ${tmp}`

	tmp=0
	CFG_CV_USER_MEM_SIZE=`printf "0x%010X\n" ${tmp}`

	tmp=0
	CFG_CV_RTOS_MEM_SIZE=`printf "0x%010X\n" ${tmp}`
fi

# ================  show ================
tmp=`awk "BEGIN {print $CFG_DRAM_MEM_BASE}"`
CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
echo -e "\033[032m +-----------------------------------+  ${CFG_DRAM_MEM_BASE}  \033[0m"
echo -e "\033[037m | Reserved                          |     Size: ${CFG_ATF_MEM_BASE} \033[0m"
echo -e "\033[032m +-----------------------------------+  ${CFG_ATF_MEM_BASE}  \033[0m"
if [ $((CFG_ATF_MEM_SIZE)) -gt $((0x0000000000)) ]; then
	echo -e "\033[037m | ATF (BL31)                        |     Size: ${CFG_ATF_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_ATF_MEM_END}  \033[0m"
fi
tmp=`awk "BEGIN {print $CFG_ATF_MEM_END}"`
CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
if [ $((CFG_OPTEE_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_OPTEE_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_OPTEE_MEM_BASE}  \033[0m"
	fi
	echo -e "\033[037m | OPTEE (BL32)                      |     Size: ${CFG_OPTEE_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_OPTEE_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_OPTEE_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_XTB_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_XTB_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_XTB_MEM_BASE}  \033[0m"
	fi
	echo -e "\033[037m | DTB                               |     Size: ${CFG_XTB_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_XTB_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_XTB_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_SDK_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_SDK_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_SDK_MEM_BASE}  \033[0m"
	fi
	if [ "${CONFIG_AMBALINK_MEM_SIZE}" == "" ]; then
		tmp=`awk "BEGIN {print $CFG_SDK_MEM_SIZE-$CFG_CV_MEM_SIZE-$CONFIG_DSP_TOTAL_BUF_SIZE}"`
	else
		tmp=`awk "BEGIN {print $CFG_SDK_MEM_SIZE}"`
	fi
	CFG_SDK_REMAIN_SIZE=`printf "0x%010X\n" ${tmp}`
	echo -e "\033[037m | PRIMARY OS                        |     Size: ${CFG_SDK_REMAIN_SIZE} \033[0m"
	if [ "${CONFIG_BUILD_CV}" != "" ] && [ "${CONFIG_AMBALINK_MEM_SIZE}" == "" ]; then
		echo -e "\033[032m +    -------------------------------+  ${CFG_CV_MEM_BASE} \033[0m"
		echo -e "\033[037m |     CV SCHDR                      |     Size: ${CFG_CV_SCHDR_MEM_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CFG_CV_SCHDR_MEM_END} \033[0m"
		echo -e "\033[037m |     CV USER                       |     Size: ${CFG_CV_USER_MEM_SIZE} \033[0m"
	fi
	if [ "${CONFIG_BUILD_DSP_VISUAL}" != "" ] && [ "${CONFIG_AMBALINK_MEM_SIZE}" == "" -o "${CONFIG_ATT_8GB}" == "" ]; then
		if [ $((CONFIG_DSP_TOTAL_BUF_SIZE)) -gt $((0x0000000000)) ]; then
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_TOTAL_BUF_BASE} \033[0m"
		echo -e "\033[037m |     DSP PROT                      |     Size: ${CONFIG_DSP_PROT_BUF_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_PROT_BUF_END} \033[0m"
		echo -e "\033[037m |     DSP PROT CACHED               |     Size: ${CONFIG_DSP_PROT_CACHE_BUF_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_PROT_CACHE_BUF_END} \033[0m"
		echo -e "\033[037m |     DSP BIN                       |     Size: ${CONFIG_DSP_BIN_BUF_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_BIN_BUF_END} \033[0m"
		echo -e "\033[037m |     DSP DATA                      |     Size: ${CONFIG_DSP_DATA_BUF_SIZE} \033[0m"
		fi
	fi
	echo -e "\033[032m +-----------------------------------+  ${CFG_SDK_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_SDK_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_APP_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_APP_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_APP_MEM_BASE}  \033[0m"
	fi
	echo -e "\033[037m | APP                               |     Size: ${CFG_APP_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_APP_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_APP_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_RPMSG_G1_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_RPMSG_G1_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_RPMSG_G1_MEM_BASE}  \033[0m"
	fi
	echo -e "\033[037m | RPMSG G1                          |     Size: ${CFG_RPMSG_G1_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_RPMSG_G1_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_RPMSG_G1_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_AMBALINK_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_AMBALINK_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_AMBALINK_MEM_BASE}  \033[0m"
	fi
	if [ "${CONFIG_BUILD_CV}" != "" ]; then
		tmp=`awk "BEGIN {print $CFG_AMBALINK_MEM_SIZE-$CFG_CV_MEM_SIZE}"`
		CFG_AMBALINK_REMAIN_SIZE=`printf "0x%010X\n" ${tmp}`
		echo -e "\033[037m | AMBALINK                          |     Size: ${CFG_AMBALINK_REMAIN_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CFG_CV_MEM_BASE} \033[0m"
		echo -e "\033[037m |     CV SCHDR                      |     Size: ${CFG_CV_SCHDR_MEM_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CFG_CV_SCHDR_MEM_END} \033[0m"
		echo -e "\033[037m |     CV LINUX USER                 |     Size: ${CFG_CV_USER_MEM_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CFG_CV_SYS_MEM_END} \033[0m"
		echo -e "\033[037m |     CV RTOS USER                  |     Size: ${CFG_CV_RTOS_MEM_SIZE} \033[0m"
	else
		echo -e "\033[037m | AMBALINK                          |     Size: ${CFG_AMBALINK_MEM_SIZE} \033[0m"
	fi
	echo -e "\033[032m +-----------------------------------+  ${CFG_AMBALINK_MEM_END}  \033[0m"
	
	tmp=`awk "BEGIN {print $CFG_AMBALINK_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ "${CONFIG_BUILD_DSP_VISUAL}" != "" ] && [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" -a "${CONFIG_ATT_8GB}" != "" ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CONFIG_DSP_TOTAL_BUF_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CONFIG_DSP_TOTAL_BUF_BASE}  \033[0m"
	fi

	if [ $((CONFIG_DSP_TOTAL_BUF_SIZE)) -gt $((0x0000000000)) ]; then
		echo -e "\033[037m |     DSP PROT                      |     Size: ${CONFIG_DSP_PROT_BUF_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_PROT_BUF_END} \033[0m"
		echo -e "\033[037m |     DSP PROT CACHED               |     Size: ${CONFIG_DSP_PROT_CACHE_BUF_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_PROT_CACHE_BUF_END} \033[0m"
		echo -e "\033[037m |     DSP BIN                       |     Size: ${CONFIG_DSP_BIN_BUF_SIZE} \033[0m"
		echo -e "\033[032m +    -------------------------------+  ${CONFIG_DSP_BIN_BUF_END} \033[0m"
		echo -e "\033[037m |     DSP DATA                      |     Size: ${CONFIG_DSP_DATA_BUF_SIZE} \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CONFIG_DSP_DATA_BUF_END}  \033[0m"
		tmp=`awk "BEGIN {print $CONFIG_DSP_DATA_BUF_END}"`
		CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
	fi
fi
if [ $((CFG_PIO_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_PIO_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_PIO_MEM_BASE}  \033[0m"
	fi

	echo -e "\033[037m | PIO                               |     Size: ${CFG_PIO_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_PIO_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_PIO_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_OPTEE_SHM_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_OPTEE_SHM_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_OPTEE_SHM_MEM_BASE}  \033[0m"
	fi

	echo -e "\033[037m | OPTEE SHM                         |     Size: ${CFG_OPTEE_SHM_MEM_SIZE} \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_OPTEE_SHM_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_OPTEE_SHM_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_R52SYS_MEM_BASE)) -gt $((0x0000000000)) ]; then
	if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_R52SYS_MEM_BASE)) ]; then
		echo -e "\033[037m |                                   |      \033[0m"
		echo -e "\033[032m +-----------------------------------+  ${CFG_R52SYS_MEM_BASE}  \033[0m"
	fi
	if [ "${CONFIG_AMBALINK_RPMSG_G2}" != "" ]; then
		echo -e "\033[037m | R52SYS & RPMSG G2                 |     Size: ${CFG_R52SYS_MEM_SIZE} \033[0m"
	else
		echo -e "\033[037m | R52SYS                            |     Size: ${CFG_R52SYS_MEM_SIZE} \033[0m"
	fi
	echo -e "\033[032m +-----------------------------------+  ${CFG_R52SYS_MEM_END}  \033[0m"
	tmp=`awk "BEGIN {print $CFG_R52SYS_MEM_END}"`
	CFG_SHOW_TOP_DOWN=`printf "0x%010X\n" ${tmp}`
fi
if [ $((CFG_SHOW_TOP_DOWN)) -ne $((CFG_DRAM_MEM_END)) ]; then
	echo -e "\033[037m |                                   |      \033[0m"
	echo -e "\033[032m +-----------------------------------+  ${CFG_DRAM_MEM_END}  \033[0m"
fi

# ================  insert dtb ================
tmp=0
CFG_NULL_SIZE=`printf "0x%010X\n" ${tmp}`

if [ "${CFG_TEST}" != "" ]; then
	dtc -I dts -O dtb -p 2048 -o $DTB_PATH -i "bsp/"${CONFIG_SOC_NAME} "."${CONFIG_DTS}  -W no-unit_address_vs_reg -W no-simple_bus_reg
fi

if [ "${CONFIG_CPU_CORTEX_A76}" != "" ]; then
	# memory
	#if [ "${CONFIG_LINUX}" != "" ] && [ $((CFG_DRAM_BOTTOM_UP_BASE)) -gt $((0x100000000)) ]; then
	#	OVER4G_BASE=0x100000000
	#	OVER4G_SIZE=$((CFG_DRAM_BOTTOM_UP_BASE - OVER4G_BASE))
	#fi
	fdtput -t u $DTB_PATH /memory reg $((CFG_SDK_MEM_BASE >> 32)) $((CFG_SDK_MEM_BASE & 0xFFFFFFFF)) $(((CFG_DRAM_BOTTOM_UP_BASE - CFG_SDK_MEM_BASE) >> 32)) $(((CFG_DRAM_BOTTOM_UP_BASE - CFG_SDK_MEM_BASE) & 0xFFFFFFFF))

	# reserved memory
	if [ "${CONFIG_LINUX}" != "" ]; then
		fdtput -r $DTB_PATH /reserved-memory/xtb
	else
		if [ "${CONFIG_DTB_LOADADDR}" != "" ]; then
			fdtput -t u $DTB_PATH /reserved-memory/xtb reg $((CFG_XTB_MEM_BASE >> 32)) $((CFG_XTB_MEM_BASE & 0xFFFFFFFF)) $((CFG_XTB_MEM_SIZE >> 32)) $((CFG_XTB_MEM_SIZE & 0xFFFFFFFF))
			fdtput -t u $DTB_PATH /reserved-memory/xtb no-map
		fi
	fi
	if [ "${CONFIG_APP_MEM_SIZE}" != "" ]; then
		fdtput -t u $DTB_PATH /reserved-memory/app reg $((CFG_APP_MEM_BASE >> 32)) $((CFG_APP_MEM_BASE & 0xFFFFFFFF)) $((CFG_APP_MEM_SIZE >> 32)) $((CFG_APP_MEM_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /reserved-memory/app no-map
	fi
	if [ "${CONFIG_AMBALINK_RPMSG_G1}" != "" ]; then
		fdtput -t u $DTB_PATH /reserved-memory/rpmsg_g1 reg $((CFG_RPMSG_G1_MEM_BASE >> 32)) $((CFG_RPMSG_G1_MEM_BASE & 0xFFFFFFFF)) $((CFG_RPMSG_G1_MEM_SIZE >> 32)) $((CFG_RPMSG_G1_MEM_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /reserved-memory/rpmsg_g1 no-map
	fi
	if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
		fdtput -t u $DTB_PATH /reserved-memory/ambalink reg $((CFG_AMBALINK_MEM_BASE >> 32)) $((CFG_AMBALINK_MEM_BASE & 0xFFFFFFFF)) $((CFG_AMBALINK_MEM_SIZE >> 32)) $((CFG_AMBALINK_MEM_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /reserved-memory/ambalink no-map
	fi
	if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ]; then
		fdtput -t u $DTB_PATH /reserved-memory/r52_sys reg $((CFG_R52SYS_MEM_BASE >> 32)) $((CFG_R52SYS_MEM_BASE & 0xFFFFFFFF)) $((CFG_R52SYS_MEM_SIZE >> 32)) $((CFG_R52SYS_MEM_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /reserved-memory/r52_sys no-map
	fi
	if [ "${CONFIG_PIO_MEM_ALLOC}" != "" ]; then
		fdtput -c $DTB_PATH /reserved-memory/pio 2>/dev/null
		fdtput -t u $DTB_PATH /reserved-memory/pio reg $((CONFIG_PIO_BASE >> 32)) $((CONFIG_PIO_BASE & 0xFFFFFFFF)) $((CONFIG_PIO_SIZE >> 32)) $((CONFIG_PIO_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /reserved-memory/pio no-map
		fdtput -t s $DTB_PATH /reserved-memory/pio compatible "shared-dma-pool"
	fi
	if [ "${CONFIG_BUILD_CV}" != "" ]; then
		fdtput -t u $DTB_PATH /reserved-memory/flexidag_sys reg $((CFG_CV_MEM_BASE >> 32)) $((CFG_CV_MEM_BASE & 0xFFFFFFFF)) $((CFG_CV_SYS_MEM_SIZE >> 32)) $((CFG_CV_SYS_MEM_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /reserved-memory/flexidag_sys reusable
		fdtput -t s $DTB_PATH /reserved-memory/flexidag_sys compatible "shared-dma-pool"
		if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
			fdtput -t u $DTB_PATH /reserved-memory/flexidag_rtos reg $((CFG_CV_SYS_MEM_END >> 32)) $((CFG_CV_SYS_MEM_END & 0xFFFFFFFF)) $((CFG_CV_RTOS_MEM_SIZE >> 32)) $((CFG_CV_RTOS_MEM_SIZE & 0xFFFFFFFF))
			fdtput -t u $DTB_PATH /reserved-memory/flexidag_rtos no-map
			fdtput -t s $DTB_PATH /reserved-memory/flexidag_rtos compatible "shared-dma-pool"
		fi
	fi
	if [ "${CONFIG_BUILD_DSP_VISUAL}" != "" ]; then
		fdtput -t u $DTB_PATH /reserved-memory/dsp_prot_buf reg $((CONFIG_DSP_PROT_BUF_BASE >> 32)) $((CONFIG_DSP_PROT_BUF_BASE & 0xFFFFFFFF)) $((CONFIG_DSP_PROT_BUF_SIZE >> 32)) $((CONFIG_DSP_PROT_BUF_SIZE & 0xFFFFFFFF))
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t u $DTB_PATH /reserved-memory/dsp_prot_buf no-map
		else
			fdtput -t u $DTB_PATH /reserved-memory/dsp_prot_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_prot_buf compatible "shared-dma-pool"

		fdtput -t u $DTB_PATH /reserved-memory/dsp_prot_cache_buf reg $((CONFIG_DSP_PROT_CACHE_BUF_BASE >> 32)) $((CONFIG_DSP_PROT_CACHE_BUF_BASE & 0xFFFFFFFF)) $((CONFIG_DSP_PROT_CACHE_BUF_SIZE >> 32)) $((CONFIG_DSP_PROT_CACHE_BUF_SIZE & 0xFFFFFFFF))
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t u $DTB_PATH /reserved-memory/dsp_prot_cache_buf no-map
		else
			fdtput -t u $DTB_PATH /reserved-memory/dsp_prot_cache_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_prot_cache_buf compatible "shared-dma-pool"

		fdtput -t u $DTB_PATH /reserved-memory/dsp_bin_buf reg $((CONFIG_DSP_BIN_BUF_BASE >> 32)) $((CONFIG_DSP_BIN_BUF_BASE & 0xFFFFFFFF)) $((CONFIG_DSP_BIN_BUF_SIZE >> 32)) $((CONFIG_DSP_BIN_BUF_SIZE & 0xFFFFFFFF))
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t u $DTB_PATH /reserved-memory/dsp_bin_buf no-map
		else
			fdtput -t u $DTB_PATH /reserved-memory/dsp_bin_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_bin_buf compatible "shared-dma-pool"

		fdtput -t u $DTB_PATH /reserved-memory/dsp_data_buf reg $((CONFIG_DSP_DATA_BUF_BASE >> 32)) $((CONFIG_DSP_DATA_BUF_BASE & 0xFFFFFFFF)) $((CONFIG_DSP_DATA_BUF_SIZE >> 32)) $((CONFIG_DSP_DATA_BUF_SIZE & 0xFFFFFFFF))
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t u $DTB_PATH /reserved-memory/dsp_data_buf no-map
		else
			fdtput -t u $DTB_PATH /reserved-memory/dsp_data_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_data_buf compatible "shared-dma-pool"
	fi
	#if [ "${CONFIG_LINUX}" != "" ] && [ $((CFG_DRAM_BOTTOM_UP_BASE)) -gt $((0x100000000)) ]; then
	#	fdtput -c $DTB_PATH /reserved-memory/over4G 2>/dev/null
	#	fdtput -t u $DTB_PATH /reserved-memory/over4G reg $((OVER4G_BASE >> 32)) $((OVER4G_BASE & 0xFFFFFFFF)) $((OVER4G_SIZE >> 32)) $((OVER4G_SIZE & 0xFFFFFFFF))
	#	fdtput -t u $DTB_PATH /reserved-memory/over4G reusable
	#	fdtput -t s $DTB_PATH /reserved-memory/over4G compatible "shared-dma-pool"
	#fi

	# app
	if [ "${CONFIG_APP_MEM_SIZE}" != "" ]; then
		fdtput -t u $DTB_PATH /app_shm reg $((CFG_APP_MEM_BASE >> 32)) $((CFG_APP_MEM_BASE & 0xFFFFFFFF)) $((CFG_APP_MEM_SIZE >> 32)) $((CFG_APP_MEM_SIZE & 0xFFFFFFFF))
	fi

	# cv
	if [ "${CONFIG_BUILD_CV}" != "" ]; then
		fdtput -t u $DTB_PATH /sub_scheduler0 cv_schdr_size $((CFG_CV_SCHDR_MEM_SIZE >> 32)) $((CFG_CV_SCHDR_MEM_SIZE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /sub_scheduler0 cv_att_ca $((CFG_CV_MEM_BASE >> 32)) $((CFG_CV_MEM_BASE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /sub_scheduler0 cv_att_pa $((CFG_CV_MEM_BASE >> 32)) $((CFG_CV_MEM_BASE & 0xFFFFFFFF))
		fdtput -t u $DTB_PATH /sub_scheduler0 cv_att_size $((CFG_CV_MEM_SIZE >> 32)) $((CFG_CV_MEM_SIZE & 0xFFFFFFFF))
	fi

	#gclk-ddr
	if [ "${CONFIG_DRAM_TYPE_LPDDR4}" != "" ]; then
		fdtput -t x $DTB_PATH /clocks/gclk-ddr0 amb,fix-divider 4
		fdtput -t x $DTB_PATH /clocks/gclk-ddr1 amb,fix-divider 4
	fi
else
	# memory
	fdtput -t x $DTB_PATH /memory reg $CFG_SDK_MEM_BASE $CFG_SDK_MEM_SIZE

	# reserved memory
	if [ "${CONFIG_LINUX}" != "" ]; then
		fdtput -r $DTB_PATH /reserved-memory/xtb
	else
		if [ "${CONFIG_DTB_LOADADDR}" != "" ]; then
				fdtput -t x $DTB_PATH /reserved-memory/xtb reg $CFG_XTB_MEM_BASE $CFG_XTB_MEM_SIZE
				fdtput -t x $DTB_PATH /reserved-memory/xtb no-map
		fi
	fi
	if [ "${CONFIG_APP_MEM_SIZE}" != "" ]; then
		fdtput -t x $DTB_PATH /reserved-memory/app reg $CFG_APP_MEM_BASE $CFG_APP_MEM_SIZE
		fdtput -t x $DTB_PATH /reserved-memory/app no-map
	fi
	if [ "${CONFIG_AMBALINK_RPMSG_G1}" != "" ]; then
		fdtput -t x $DTB_PATH /reserved-memory/rpmsg_g1 reg $CFG_RPMSG_G1_MEM_BASE $CFG_RPMSG_G1_MEM_SIZE
		fdtput -t x $DTB_PATH /reserved-memory/rpmsg_g1 no-map
	fi
	if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
		fdtput -t x $DTB_PATH /reserved-memory/ambalink reg $CFG_AMBALINK_MEM_BASE $CFG_AMBALINK_MEM_SIZE
		fdtput -t x $DTB_PATH /reserved-memory/ambalink no-map
	fi
	if [ "${CONFIG_FWPROG_R52SYS_ENABLE}" != "" ]; then
		fdtput -t x $DTB_PATH /reserved-memory/r52_sys reg $CFG_R52SYS_MEM_BASE $CFG_R52SYS_MEM_SIZE
		fdtput -t x $DTB_PATH /reserved-memory/r52_sys no-map
	fi
	if [ "${CONFIG_PIO_MEM_ALLOC}" != "" ]; then
		fdtput -c $DTB_PATH /reserved-memory/pio 2>/dev/null
		fdtput -t x $DTB_PATH /reserved-memory/pio reg $CONFIG_PIO_BASE $CONFIG_PIO_SIZE
		fdtput -t x $DTB_PATH /reserved-memory/pio no-map
		fdtput -t s $DTB_PATH /reserved-memory/pio compatible "shared-dma-pool"
	fi
	if [ "${CONFIG_BUILD_CV}" != "" ]; then
		fdtput -t x $DTB_PATH /reserved-memory/flexidag_sys reg $CFG_CV_MEM_BASE $CFG_CV_SYS_MEM_SIZE
		fdtput -t x $DTB_PATH /reserved-memory/flexidag_sys reusable
		fdtput -t s $DTB_PATH /reserved-memory/flexidag_sys compatible "shared-dma-pool"
		if [ "${CONFIG_AMBALINK_MEM_SIZE}" != "" ]; then
			fdtput -t x $DTB_PATH /reserved-memory/flexidag_rtos reg $CFG_CV_SYS_MEM_END $CFG_CV_RTOS_MEM_SIZE
			fdtput -t x $DTB_PATH /reserved-memory/flexidag_rtos no-map
			fdtput -t s $DTB_PATH /reserved-memory/flexidag_rtos compatible "shared-dma-pool"
		fi
	fi
	if [ "${CONFIG_BUILD_DSP_VISUAL}" != "" ]; then
		fdtput -t x $DTB_PATH /reserved-memory/dsp_prot_buf reg $CONFIG_DSP_PROT_BUF_BASE $CONFIG_DSP_PROT_BUF_SIZE
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t x $DTB_PATH /reserved-memory/dsp_prot_buf no-map
		else
			fdtput -t x $DTB_PATH /reserved-memory/dsp_prot_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_prot_buf compatible "shared-dma-pool"

		fdtput -t x $DTB_PATH /reserved-memory/dsp_prot_cache_buf reg $CONFIG_DSP_PROT_CACHE_BUF_BASE $CONFIG_DSP_PROT_CACHE_BUF_SIZE
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t x $DTB_PATH /reserved-memory/dsp_prot_cache_buf no-map
		else
			fdtput -t x $DTB_PATH /reserved-memory/dsp_prot_cache_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_prot_cache_buf compatible "shared-dma-pool"

		fdtput -t x $DTB_PATH /reserved-memory/dsp_bin_buf reg $CONFIG_DSP_BIN_BUF_BASE $CONFIG_DSP_BIN_BUF_SIZE
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t x $DTB_PATH /reserved-memory/dsp_bin_buf no-map
		else
			fdtput -t x $DTB_PATH /reserved-memory/dsp_bin_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_bin_buf compatible "shared-dma-pool"

		fdtput -t x $DTB_PATH /reserved-memory/dsp_data_buf reg $CONFIG_DSP_DATA_BUF_BASE $CONFIG_DSP_DATA_BUF_SIZE
		if [ "${CONFIG_LINUX}" != "" ]; then
			fdtput -t x $DTB_PATH /reserved-memory/dsp_data_buf no-map
		else
			fdtput -t x $DTB_PATH /reserved-memory/dsp_data_buf reusable
		fi
		fdtput -t s $DTB_PATH /reserved-memory/dsp_data_buf compatible "shared-dma-pool"
	fi

	# app
	if [ "${CONFIG_APP_MEM_SIZE}" != "" ]; then
		fdtput -t x $DTB_PATH /app_shm reg $CFG_APP_MEM_BASE $CFG_APP_MEM_SIZE
	fi

	# cv
	if [ "${CONFIG_BUILD_CV}" != "" ]; then
		fdtput -t x $DTB_PATH /sub_scheduler0 cv_schdr_size $CFG_CV_SCHDR_MEM_SIZE
		fdtput -t x $DTB_PATH /sub_scheduler0 cv_att_ca $CFG_CV_MEM_BASE
		fdtput -t x $DTB_PATH /sub_scheduler0 cv_att_pa $CFG_CV_MEM_BASE
		fdtput -t x $DTB_PATH /sub_scheduler0 cv_att_size $CFG_CV_MEM_SIZE
	fi
fi
if [ "${CFG_TEST}" != "" ]; then
	dtc -I dtb -O dts -o sdk8.dts $DTB_PATH  -W no-unit_address_vs_reg -W no-simple_bus_reg
fi
