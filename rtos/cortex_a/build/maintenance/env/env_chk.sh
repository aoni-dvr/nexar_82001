#!/bin/bash
#
#

# Debug
#D_CHK=1


# Debug start
[ "${D_CHK}" = "1" ] && set -x

E_FILE=${BASH_SOURCE[0]}
if [ "${E_FILE}" != "${0}" ]; then
    echo "Usage: `pwd`/${E_FILE}"
    return
fi


# Get common data
PATH_ENV=$(dirname ${E_FILE})
source ${PATH_ENV}/env_common.sh
fn_amba_env_set


# Return error code and Makefile will check it. Also unset vairabls set by 'source'
function env_chk_end()
{
    echo $1
    fn_amba_env_unset
    unset D_CHK
    exit 0
}


# GCC 32b/64b toolchains
ptn_32b="`${CROSS_32b}gcc -v 2>&1 | grep 'gcc version' | sed -e  's/[ \t]*$//'`"
ptn_64b="`${CROSS_64b}gcc -v 2>&1 | grep 'gcc version' | sed -e  's/[ \t]*$//'`"

if [ "${ptn_32b}" != "${PTN_32b}" ]; then
    env_chk_end 1
fi

if [ "${ptn_64b}" != "${PTN_64b}" ]; then
    env_chk_end 2
fi


# Normal, OK
env_chk_end 0


