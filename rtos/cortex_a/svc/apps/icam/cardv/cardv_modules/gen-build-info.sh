#!/bin/bash

MODULES_PATH=${1}
platform_name=`cat ${MODULES_PATH}/../platform_name.txt`
VERSION_PATH="${MODULES_PATH}/../tmp_version.txt"
BUILD_TIME=`date "+%Y-%m-%d %H:%M:%S"`
BUILD_TIMESTAMP=`date +%s`
GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
GIT_COMMIT_ID=`git rev-parse HEAD | cut -b -10`

CARDV_BASE_INCLUDE_H_PATH=${MODULES_PATH}/cardv_include.h

echo "#ifndef __CARDV_BASE_INCLUDE_H__" > ${CARDV_BASE_INCLUDE_H_PATH}
echo "#define __CARDV_BASE_INCLUDE_H__" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "" >> ${CARDV_BASE_INCLUDE_H_PATH}

echo "#include \"include/include_${platform_name}.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#include \"debug/debug_${platform_name}.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#include \"modules/dqa_test_script/dqa_test_script.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#include \"modules/dqa_test_uart/dqa_test_uart.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#include \"modules/product_line/product_line.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#include \"modules/gnss/gnss_parser.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#include \"modules/gnss/gnss_setting.h\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "" >> ${CARDV_BASE_INCLUDE_H_PATH}

echo "#define PROJECT_INFO_BUILD_HOST    	\"$(whoami)@$(hostname)\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#define PROJECT_INFO_BUILD_TIME    	\"${BUILD_TIME}\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#define PROJECT_INFO_BUILD_TIMESTAMP  \"${BUILD_TIMESTAMP}\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#define PROJECT_INFO_GIT_BRANCH 		\"${GIT_BRANCH}\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#define PROJECT_INFO_GIT_COMMIT_ID	\"${GIT_COMMIT_ID}\"" >> ${CARDV_BASE_INCLUDE_H_PATH}

if [ -f ${VERSION_PATH} ]; then
    echo "#define PROJECT_INFO_SW_VERSION	        \"$(cat ${VERSION_PATH})\"" >> ${CARDV_BASE_INCLUDE_H_PATH}
fi

echo "" >> ${CARDV_BASE_INCLUDE_H_PATH}
echo "#endif//__CARDV_BASE_INCLUDE_H__" >> ${CARDV_BASE_INCLUDE_H_PATH}
