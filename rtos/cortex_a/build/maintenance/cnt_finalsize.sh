#!/bin/bash

#
# To calculate text and data size used in elf finnaly by parsing .map file.
# Usage:
#   cnt_finalsize path_of_map_file
#

if [ "$1" == "" ];
then
    MAP=output/out/amba_ssp_ut.map
else
    MAP=$1
fi

if [ ! -e ${MAP} ];
then
    echo "${MAP} not found!"
    exit
fi

cnt_size()
{
    PTN=$1

    grep ${PTN} text.map | \
        awk '{ if (NF > 2) { print }}' | \
        grep -v 0x0000000000000000 | \
        awk '{print $(NF-1)}' | \
        xargs printf "%d\n" | \
        awk '{sum += $1} END {print "\tro Data: "sum}'

    grep ${PTN} data.map | \
        awk '{ if (NF > 2) { print }}' | \
        grep -v 0x0000000000000000 | \
        awk '{print $(NF-1)}' | \
        xargs printf "%d\n" | \
        awk '{sum += $1} END {print "\trw Data: "sum}'
}

le=`grep -n "Discarded input sections" ${MAP} | awk -F ":" '{print $1}'`
ts=`grep -n "__ddr_start = ." ${MAP} | awk -F ":" '{print $1}'`
ds=`grep -n "__ddr_region0_rw_start = ." ${MAP} | awk -F ":" '{print $1}'`
de=`grep -n "__bss_end = ." ${MAP} | awk -F ":" '{print $1}'`

sed -n 1,${le}p ${MAP} > lib.map
sed -n ${ts},${ds}p ${MAP} > text.map
sed -n ${ds},${de}p ${MAP} > data.map

libs=`grep ".a(" lib.map | awk -F "(" '{print $1}' | awk -F "/" '{print $NF}' | sort  -u`

for l in ${libs}; do
    echo ${l}
    cnt_size ${l}
done

echo ""
echo "ro Data: text, read-only data, constant data"
echo "rw Data: read-write data, bss, unwind data"
echo ""

rm -f lib.map text.map data.map

