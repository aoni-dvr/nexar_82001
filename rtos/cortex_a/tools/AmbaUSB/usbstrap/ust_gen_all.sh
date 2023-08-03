#!/bin/bash

if [ "$1" == "" ]; then
    echo "Usage: $0 [ini file]"
    exit -1
else
    input=$1
fi

filename=`echo $input | sed -e 's/\.ini//'`

lpddr4=`echo $filename | grep 'lpddr4'`
if [ "$lpddr4" == "" ]; then
    dram_type="lpddr5"
else
    dram_type="lpddr4"
fi

export CROSS_COMPILE=aarch64-none-elf-

make clean
make LOCAL_OUTPUT=out DRAM_CFG=config/${filename}.ini DRAM_TYPE=${dram_type} parameter
make LOCAL_OUTPUT=out DRAM_CFG=config/${filename}.ini DRAM_TYPE=${dram_type} all
cp out/ambust_${dram_type}_signed.bin ${filename}.bin

make clean
make LOCAL_OUTPUT=out DRAM_CFG=config/${filename}.ini DRAM_TYPE=${dram_type} PROTOCOL=ambusb parameter
make LOCAL_OUTPUT=out DRAM_CFG=config/${filename}.ini DRAM_TYPE=${dram_type} PROTOCOL=ambusb all
cp out/ambust_${dram_type}_signed.bin ${filename}.ust

