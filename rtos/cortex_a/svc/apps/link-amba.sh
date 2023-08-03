#!/bin/bash

# Error out on error
set -e
# Debug
#set -x

# $1: image name

IMG_NAME=$1

# We need access to CONFIG_ symbols
case "${KCONFIG_CONFIG}" in
*/*)
	. "${KCONFIG_CONFIG}"
	;;
*)
	# Force using a file from the current directory
	. "./${KCONFIG_CONFIG}"
esac

source ${srctree}/build/scripts/link_common.sh

home_sh=$(dirname `readlink -f $0`)
. ${home_sh}/link-amba_func.sh
if [ "$CONFIG_SVC_APPS_ICAM" = "y" ]; then
    if [ "$CONFIG_THREADX" = "y" ]; then
        . ${home_sh}/${CONFIG_SVC_APP_NAME}/main/build/threadx/link-amba_data.sh
    else
        . ${home_sh}/${CONFIG_SVC_APP_NAME}/main/build/linux/link-amba_data.sh
    fi
else
if [ "$CONFIG_THREADX" = "y" ]; then
. ${home_sh}/${CONFIG_SVC_APP_NAME}/link-amba_data.sh
else
if [ "$CONFIG_SVC_APP_NAME" = "ut" ] || [ "$CONFIG_SVC_APP_NAME" = "cvut" ]; then
. ${home_sh}/${CONFIG_SVC_APP_NAME}/build/linux/link-amba_data.sh
elif [ "$CONFIG_SVC_APP_NAME" = "diagnosis" ]; then
. ${home_sh}/${CONFIG_SVC_APP_NAME}/build/linux/link-amba_data.sh
else
. ${home_sh}/${CONFIG_SVC_APP_NAME}/link-amba_data.sh
fi

fi
fi

if [ "$CONFIG_ENABLE_AMBALINK" = "y" ]; then
	rpc_crc_calc
fi

run_remove_cmd()
{
    if [ "$CONFIG_LINUX" = "y" ]; then
        echo "## remove ${objtree}/fwprog ##"
        rm -rf ${objtree}/fwprog
        echo "## remove ${objtree}/out/fwprog ##"
        rm -rf ${objtree}/out/fwprog
    else
        echo "## remove ${srctree}/${O}.64/fwprog ##"
        rm -rf ${srctree}/${O}.64/fwprog
        echo "## remove ${srctree}/${O}.64/out ##"
        rm -rf ${srctree}/${O}.64/out
    fi
}

run_remove_cmd
run_ld_cmd
run_nm_cmd
run_objcopy_cmd
run_bincopy_cmd
echo "$0 Done..."

