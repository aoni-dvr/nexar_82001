#!/bin/bash
#
#
#set -x

echo "SDK uses CVTOOL_VER: ${CVTOOL_VER}"
cur_cv=`dirname ${TV2_CONFIG}`/project.tv2
if [ -e ${cur_cv} ]; then
    cur_cv=`head -1 ${cur_cv}`
    echo "Current  CVTOOL_VER: ${cur_cv}"
else
    echo "Cureent  CVTOOL_VER: (Unknown)"
fi


