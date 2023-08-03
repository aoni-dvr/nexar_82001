#!/bin/bash
#
#

# Debug
#D_SET=1

echo "[Warning] When using docker/podman, please use /opt/amba/env/env_set.sh"

# Debug start
[ "${D_SET}" = "1" ] && set -x

E_FILE=${BASH_SOURCE[0]}
if [ "${E_FILE}" = "${0}" ]; then
    echo "Usage: source ${E_FILE}"
    exit
fi

# Show usage
if  [ $# != 0 ]; then
    if  [ "$1" == "--help" ]; then
        echo $"Usage: source ${E_FILE} [CHIP] [AMBA_ROOT]"
        echo $"       CHIP,         If use cv tools, please source with chip name"
        echo $"                     e.g. cv2, cv22, cv25, cv28, cv2fs, cv22fs, cv5"
        echo $"       AMBA_ROOT,    If use cv tools, you can change tools path"
        echo $"                     default. /opt/amba"
        return
    fi
fi

# 'source' common patterns and variables
PATH_ENV=$(dirname ${E_FILE})
source ${PATH_ENV}/env_common.sh
fn_amba_env_set
ENV_CHK=`${PATH_ENV}/env_chk.sh`

if [ "${ENV_CHK}" != "0" ]; then
    # Set path of toolchain
    source ${PATH_ENV}/env_toolchain.sh
    fn_amba_env_toolchain_set

    # export pattern of release
fi

# 'source' CV patterns
if  [ $# != 0 ]; then
	source ${PATH_ENV}/tv2.bashrc
	fn_amba_tw2_bashrc_set $1 $2
fi

# Unset patterns and variables
fn_amba_env_unset

echo "Current Path:"
echo "    $PATH"

# Debug end
unset D_SET
set +x

