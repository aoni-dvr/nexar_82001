#!/bin/bash

if [ "$1" == "" ]; then
    echo "Usage: $0 [ini file]"
    exit -1
else
    input=$1
fi

bst_bin=./bstiniparser
filename=`echo $input | sed -e 's/\.ini//'`

$bst_bin --for jtag ${filename}.ini > ${filename}.csf
$bst_bin --for usb  ${filename}.ini > ${filename}.ads
$bst_bin --for bst  ${filename}.ini > ${filename}.h
