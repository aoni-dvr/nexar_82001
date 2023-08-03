#!/bin/bash

CONFIG=$1
#CONFIG=a9aq_ssp_ut_defconfig
#MANIFEST=manifest_a9aq_ssp_ut_prebuild.txt
JOBS=8

#
# Suppose script is under rtos/
#

#rm -rf output/
make ${CONFIG} > /dev/null 2>&1

# Suppose O=output
sed -i -e 's/.*CONFIG_CC_ENABLE_INSTRUMENT_FUNC.*//g' output/.config
sed -i -e 's/.*CONFIG_CC_DUMP_RTL_EXPAND.*//g' output/.config
echo "CONFIG_CC_ENABLE_INSTRUMENT_FUNC=y" >> output/.config
echo "CONFIG_CC_DUMP_RTL_EXPAND=y" >> output/.config

make -j ${JOBS} > /dev/null 2>&1
if [ "$?" != "0" ]; then
    echo "make -j ${JOBS} for fast-boot failed" > fast-boot_$CONFIG.log
    exit
fi

# Strip .expand
cat > sed.script << EOF
s/funcdef_no/funcdef_no/; t;
s/(call (mem:SI (symbol_ref:SI (/(call (mem:SI (symbol_ref:SI (/; t;
D;

EOF

find output/ -type f | grep expand$ | xargs -d '\n' sed -i -f sed.script
rm -f sed.script

# Collect all .expand
tar -jcf expand_${CONFIG}.tar.bz2 `find output/ -type f | grep expand$`
#find output/ -type f | grep expand$ | xargs -d '\n' tar -jcf expand_${CONFIG}.tar.bz2
# Archive lib
tar -jcf lib_FB_${CONFIG}.tar.bz2 output/lib

# Do distrubution flow
#rm -rf output*
#make ${CONFIG}
#make -j ${JOBS}
#if [ "$?" != "0" ]; then
#    echo "make -j ${JOBS} for distribute failed"
#    exit
#fi
#make -j ${JOBS} distribute MANIFEST=${MANIFEST}

#
# backup data
#
mv expand_${CONFIG}.tar.bz2 lib_FB_${CONFIG}.tar.bz2 ../distribute/

