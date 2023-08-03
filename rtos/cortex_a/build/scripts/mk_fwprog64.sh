#!/bin/bash

set -e
#set -x

FWO=${O}_part64

mkdir -p ${FWO}

cp -f ${O}/.config ${FWO}/
sed -i -e '/CONFIG_ARM32/d' ${FWO}/.config
sed -i -e '/CONFIG_ARM64/d' ${FWO}/.config
echo "# CONFIG_ARM32 is not set" >> ${FWO}/.config
echo "CONFIG_ARM64=y" >> ${FWO}/.config

CROSS_COMPILE_64=$(dirname $(which aarch64-none-elf-gcc | sed -e 's!/\+!/!g'))/aarch64-none-elf-

cd ${FWO}

make O=${FWO} -f ${srctree}/Makefile CROSS_COMPILE=${CROSS_COMPILE_64} olddefconfig
make O=${FWO} -f ${srctree}/Makefile CROSS_COMPILE=${CROSS_COMPILE_64} comsvc/

cd - > /dev/null

