#!/bin/bash
#
#   Called by env_set.sh
#

# Debug
#D_TC=1


# Debug start
[ "${D_TC}" = "1" ] && set -x

E_FILE=${BASH_SOURCE[0]}
if [ "${E_FILE}" = "${0}" ]; then
    echo "Usage: source ${E_FILE}"
    exit
fi


# 'source' common patterns and variables
PATH_ENV=$(dirname ${E_FILE})
if [ "x${ENV_COMMON}" = "x" ]; then
    source ${PATH_ENV}/env_common.sh
    fn_amba_env_set
fi


function fn_amba_env_toolchain_set()
{
    NPATH=
    for p in `echo $PATH | sed -e 's/:/ /g'`; do
        case "${p}" in
            "/bin" | "/sbin" | "/usr/bin" | "/usr/sbin")
                NPATH=${NPATH}:${p}
                ;;
            "/usr/local/bin" | "/usr/local/sbin")
                NPATH=${NPATH}:${p}
                ;;
            *)
                # Remove GCC toolchains
                if [ ! -e ${p}/${CROSS_32b}gcc ] && [ ! -e ${p}/${CROSS_64b}gcc ]; then
                    NPATH=${NPATH}:${p}
                else
                    echo "Remove exist toolchain path: ${p}"
                fi
                ;;
        esac
    done
    if [ "`echo ${NPATH} | cut -c 1`" = ":" ]; then
        NPATH=`echo ${NPATH} | cut -c 2-`
    fi

    # Re-add GCC toolchains
    #echo "Old Path"
    #echo "    $PATH"
    echo "Insert toolchain PATH: ${PATH_32b} and ${PATH_64b}"
    export PATH=${PATH_32b}:${PATH_64b}:${NPATH}
#    echo "New Path:"
#    echo "    $PATH"
    unset NPATH
}

function fn_amba_env_toolchain_unset()
{
    NPATH=
    for p in `echo $PATH | sed -e 's/:/ /g'`; do
        case "${p}" in
            "${PATH_32b}" | "${PATH_64b}")
                echo "Remove exist toolchain path: ${p}"
                ;;
            *)
                if [ "x${NPATH}" = "x" ]; then
                    NPATH=${p}
                else
                    NPATH=${NPATH}:${p}
                fi
                ;;
        esac
    done
    if [ "`echo ${NPATH} | cut -c 1`" = ":" ]; then
        NPATH=`echo ${NPATH} | cut -c 2-`
    fi

    if [ "$PATH" != "${NPATH}" ]; then
        export PATH=${NPATH}
#        echo "New Path:"
#        echo "    $PATH"
#    else
#        echo "Keep Path:"
#        echo "    $PATH"
    fi

    unset NPATH
}

# Debug end
[ "${D_TC}" = "1" ] && set +x
unset D_TC

