#!/bin/bash
#
#

# Debug
#D_SET=1

echo "[Warning] When using docker/podman, please use /opt/amba/env/env_unset.sh"

# Debug start
[ "${D_SET}" = "1" ] && set -x

E_FILE=${BASH_SOURCE[0]}
if [ "${E_FILE}" = "${0}" ]; then
    echo "Usage: source ${E_FILE}"
    exit
fi


# 'source' common patterns and variables
PATH_ENV=$(dirname ${E_FILE})
source ${PATH_ENV}/env_common.sh
fn_amba_env_set

source ${PATH_ENV}/env_toolchain.sh
fn_amba_env_toolchain_unset

# 'source' CV patterns
source ${PATH_ENV}/tv2.bashrc
fn_amba_tw2_bashrc_unset $1 $2

# Unset patterns and variables
fn_amba_env_unset

echo "Current Path:"
echo "    $PATH"

# Debug end
unset D_SET
set +x

