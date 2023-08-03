#!/bin/bash
#
#   Show call graph of toolchain libraries, e.g. libc.a, libm.a, libgcc.a
#
#   Need to have .map file defined in $F and
#   need "Cross Reference Table" in .map, i.e. generate it with "--cref"
#

#set -x

F=output/out/amba_img.map


color=(red green blue cyan yellow magenta)
# Find "Symbol [ ]* File"
S=`grep -n "^Symbol" ${F} | awk -F ":" '{print $1}'`

sed -n $((S + 1)),\$p ${F} > tmp.map

while read -r line
do
    lib=`echo "${line}" | grep "libc\.a\|libm\.a\|libgcc\.a\|libnosys\.a\|libc_ext\.a\|libm\.a\|libgcc_ext\.a\|libwrap_std"`
    n=`echo "${line}" | awk '{print NF}'`

    if [ "${lib}" != "" ]; then
        if [ "${n}" = "2" ]; then
            symbolName=`echo ${line} | awk '{print $1}'`
            symbolNameO="`echo ${line} | awk -F "/" '{print $NF}'`"
            echo "  \"${symbolName}\" -> \"${symbolNameO}\";" >> g.dot.tmp
            echo "  \"${symbolNameO}\" [style=filled shape=box];" >> g.dot.tmp
        elif [ "${symbolName}" != "" ]; then
            # n should be 1
            libName=`echo ${line} | awk -F "/" '{print $NF}'`
            ci=$(($RANDOM % 6))
            echo "  \"${libName}\" -> \"${symbolName}\" [color="${color[${ci}]}"];" >> g.dot.tmp
            echo "  \"${libName}\" [style=filled shape=box];" >> g.dot.tmp
        fi
    else
        symbolName=""
    fi
done < tmp.map

echo "digraph A {" > g.dot
echo "  rankdir = LR;" >> g.dot
sort -u g.dot.tmp >> g.dot
rm -f g.dot.tmp
echo "}" >> g.dot
echo "" >> g.dot

dot -Tpdf g.dot -o ToolchainApi.pdf

rm -f tmp.map g.dot


