#!/bin/bash
#
#	Need to have .map file defined in $F
#   1. Parsing api_libXXX.a.txt
#   2. cp libXXX.a.orig as libXXX.a
#   3. Localize APIs that are not list in api_libXXX.a.txt
#   4. Final data: libXXX.bin api_libXXX.a.txt

set -e
#set -x

F=amba_img.map

CROSS_COMPILE=arm-none-eabi-
STRIP=${CROSS_COMPILE}strip
OBJCOPY=${CROSS_COMPILE}objcopy
NM=${CROSS_COMPILE}nm

LIBS='libc.a libgcc.a libm.a libnosys.a'

S=`grep -n "^Symbol" ${F} | awk -F ":" '{print $1}'`

for l in ${LIBS}; do
    if [ "${ptn}" != "" ]; then
        ptn="${ptn}\|${l}"
    else
        ptn="${l}"
    fi
done
sed -n ${S},\$p ${F} > tmp.map
(cat tmp.map | \
 grep "${ptn}" | \
 awk '{if (NF == 2) { print $1, $2}}' | \
 awk -F "(" '{if (NF == 2) { print $1 } }' | \
 sort -k 2) > tmp2.map

for l in ${LIBS}; do
    (grep ${l} tmp2.map | awk '{print $1}' | sort) > api_${l}.txt

    rm -f ${l} ${l}.bin

    api=`cat api_${l}.txt`
    if [ "${api}" != "" ]; then
        # Parsing only global Text/Weak
        (${NM} ${l}.orig 2>/dev/null | grep " T " | awk '{print $3}'| sort) > api_${l}.orig.txt
        # Remove kept symbols
        for a in $api; do
            sed -i -e "/^${a}$/d" api_${l}.orig.txt
        done
        cp ${l}.orig ${l}
        # Some API can not be removed because it is named in a relocation.
        # So mark it as local
        $OBJCOPY --localize-symbols=api_${l}.orig.txt --strip-unneeded ${l}
        mv ${l} ${l}.bin
        rm -f api_${l}.orig.txt
    else
        rm -f api_${l}.txt
    fi
done

rm -f tmp.map tmp2.map


