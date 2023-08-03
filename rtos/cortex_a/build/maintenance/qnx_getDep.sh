#!/bin/bash
#
# Suppose NAME and LIBS are all set in common.mk
#
# $*: qnx-dirs
#

# debug
#set -x

color=(red green blue cyan yellow magenta)
# Exclude libraries of QNX
libE='^m$\|^cache$\|^drvr\|^io-char\|^smmu\|^i2c-master\|^fs-flash3'

# Exported from Kbuild
PRJ=${AMBA_CHIP_ID}
srctree=${srctree}
# From command line
dir=$*


cat > tst.mk.temp << EOF
CPU := aarch64
include MK_FILE

show:
	@echo \$(LIBS)

EOF

echo "" > dep_example.mk
for d in $dir; do
    for f in `find $d -type f | grep common.mk$`; do
        modName="`grep '^NAME.*=' $f | awk -F '=' '{print $2}' | tr -d '\r'`"
        # Work-around for libXXX
        libname=`echo $modName | cut -c 1-3`
        if [ "${libname}" = "lib" ]; then
            modName=`echo $modName | cut -c 4-`
        fi
        if [ "x${modName}" != "x" ]; then
            if [ "x${modName}" = "x\$(PROJECT)" ]; then
                modName=`dirname $f | awk -F '/' '{print $NF}'`
            fi

            modDirName=`dirname $f | sed -e "s@${srctree}/@@g"`
            echo "  \"${modDirName} :: ${modName}\" [style=filled shape=box];" >> g.dot.tmp
            echo "${modName}=${modDirName}" >> dep_example.mk

            libs=`grep "LIBS.*=" $f`
            if [ "x${libs}" != "x" ]; then

                cp tst.mk.temp tst.mk;
                sed -i -e "s@MK_FILE@${f}@g" tst.mk;
                libs=`make -k -f tst.mk show ${MAKEFLAGS} 2>/dev/null | sort `
                libs=`echo $libs | tr ' ' '\n' | grep -v "${libE}" | tr '\n' ' '`
                if [ "x${libs}" != "x" ]; then
                    for l in ${libs}; do
                        ci=$(($RANDOM % 6))
                        echo "  \"${modName}\" -> \"${l}\" [color="${color[${ci}]}"];" >> g.dot.tmp
                        echo "\$(eval \$(call dep_add,${modDirName},${l}))" >> dep_example.mk
                    done
                fi
            fi
        fi
    done
done

echo "digraph A {" > g.dot
echo "  rankdir = LR;" >> g.dot
sort -u g.dot.tmp >> g.dot
rm -f g.dot.tmp
echo "}" >> g.dot
echo "" >> g.dot

# Draw dependency-graph of modules
dot -Tpdf g.dot -o QnxModDep.pdf

rm -f tst.mk.temp tst.mk
rm -f tmp.map g.dot

# For debug
#cp dep_example.mk dep_example.mk.orig

(grep '=' dep_example.mk | tr -d ' ' | sort -u) > tmp_target
sed -i -e "/=/d" dep_example.mk
for p in `cat tmp_target`; do
    # Some add lib
    o=`echo $p | awk -F '=' '{print $1}' | sed -e 's/^lib//'`
    n=`echo $p | awk -F '=' '{print $2}'`
    sed -i -e "s@,${o}))@,${n}))@g" dep_example.mk
done
sed -i -e "s@/${PRJ}/@/\$(AMBA_CHIP_ID)/@g" dep_example.mk
# Work-around for libabcS as libabc
sed -i -e 's@S))@))@g' dep_example.mk
# Work-around for binary-released
binRls=`awk -F ',' '{print $3}' dep_example.mk | grep -v '/' | sed -e 's/)//g' | sort -u`
if [ "x${binRls}" != "x" ]; then
    for b in ${binRls}; do
        sed -i -e "/,${b}))/d" dep_example.mk
    done
fi
sort -u dep_example.mk > new_dep
mv new_dep dep_example.mk

sed -i -e 's/=/ = /g' tmp_target
sed -i -e 's/^/# /g' tmp_target
echo '' >> dep_example.mk
cat tmp_target >> dep_example.mk
rm -f tmp_target


