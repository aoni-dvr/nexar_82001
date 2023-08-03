#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

if [ "$AMBA_CHIP_ID" = "h32" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_H32
elif [ "$AMBA_CHIP_ID" = "cv2" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_CV2
elif [ "$AMBA_CHIP_ID" = "cv22" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_CV22
elif [ "$AMBA_CHIP_ID" = "cv25" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_CV25
elif [ "$AMBA_CHIP_ID" = "cv28" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_CV28
elif [ "$AMBA_CHIP_ID" = "cv2fs" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_CV2FS
elif [ "$AMBA_CHIP_ID" = "cv5x" ]; then
    AMBA_CHIP_CONFIG=CONFIG_SOC_CV5
fi

DIR_amba_fwprog=fwprog
DIR_amba_fwprog_out=.temp
FWPROG_OUT_DIR=${DIR_amba_fwprog}/$DIR_amba_fwprog_out

FWLIST_DIR=$DIR_amba_fwprog/$DIR_amba_fwprog_out/
FWLIST=$DIR_amba_fwprog/$DIR_amba_fwprog_out/.fwlist

gcc -o ${srctree}/fwprog/hostutils/exec/lnx/host_ptb_gen ${srctree}/fwprog/hostutils/host_ptb_gen.c \
        -I ${srctree}/vendors/ambarella/inc/io/ -I ${srctree}/vendors/ambarella/inc/ -I ${srctree}/fwprog/inc/ \
        -DFIRM_MEM_ADDR=${FIRM_MEM_ADDR} -D$AMBA_CHIP_CONFIG \
        -m32

gcc -o ${srctree}/fwprog/hostutils/exec/lnx/host_sdfw_gen ${srctree}/fwprog/hostutils/host_sdfw_gen.c -I ${srctree}/vendors/ambarella/inc/io/ -I ${srctree}/vendors/ambarella/inc/ -I ${O}/include/generated/ -I${srctree}/bsp/${1}/${2} -DFIRM_MEM_ADDR=${FIRM_MEM_ADDR} -m32
#gcc -o ${srctree}/fwprog/hostutils/exec/lnx/host_romfs_gen ${srctree}/fwprog/hostutils/AmbaRomGen.c -I ${srctree}/vendors/ambarella/inc/io/ -I ${srctree}/vendors/ambarella/inc/ -I ${srctree}/soc/io/cv22/inc/ -m32
echo "Gen ${FWLIST}"
mkdir -p ${FWLIST_DIR}
echo -n "FIRMWARES =      " > ${FWLIST}
${AMBA_HOST_PERMUTATE} -m ${HOST_PERMUTATE_ARG} >> ${FWLIST}
${AMBA_HOST_PERMUTATE} -m ${HOST_PERMUTATE_LNX_ARG} >> ${FWLIST}
