#!/bin/bash
#
#   1. Parsing api_libXXX.a.txt
#   2. cp libXXX.a.orig as libXXX.a
#   3. Localize APIs that are not list in api_libXXX.a.txt
#   4. Final data: libXXX.bin api_libXXX.a.txt

set -e
#set -x

CROSS_COMPILE=arm-none-eabi-
#CROSS_COMPILE=aarch64-none-elf-

STRIP=${CROSS_COMPILE}strip
OBJCOPY=${CROSS_COMPILE}objcopy
NM=${CROSS_COMPILE}nm

LIBS='libc.a libgcc.a libm.a'

for l in ${LIBS}; do
    rm -f ${l} ${l}.bin

    api=`[ -s api_${l}.txt ] && cat api_${l}.txt || echo ""`
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
    fi
done


