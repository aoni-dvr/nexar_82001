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
. ${home_sh}/link-amba_pba_func.sh
. ${home_sh}/link-amba_pba_data.sh

run_ld_cmd
run_nm_cmd
run_objcopy_cmd
run_bincopy_cmd
echo "$0 Done..."

