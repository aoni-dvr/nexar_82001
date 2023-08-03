#!/bin/bash
#
#

# Debug
#D_COMM=0


# Debug start
[ "${D_COMM}" = "1" ] && set -x

E_FILE=${BASH_SOURCE[0]}
if [ "${E_FILE}" = "${0}" ]; then
    echo "Usage: source ${E_FILE}"
    exit
fi


# Set Patterns and specified variable here
function fn_amba_env_set()
{
    # 'source' this file
    ENV_COMMON=1

    # Install depended
    case "`lsb_release -r | awk '{print $2}'`" in
        "18.04")
            PATH_32b=/usr/local/gcc-arm-none-eabi-10-2020-q4-major-amba-A_R-Profile_r1_20210623/bin
            PTN_32b="gcc version 10.2.1 20201103 (release)(with DYNAMIC_REENT by Ambarella, r1) (GNU Arm Embedded Toolchain 10-2020-q4-major-amba-A_R-Profile_r1_20210623)"
            ;;
        "20.04")
            PATH_32b=/usr/local/gcc-arm-none-eabi-10-2020-q4-major-amba-A_R-Profile_r1_20210623/bin
            PTN_32b="gcc version 10.2.1 20201103 (release)(with DYNAMIC_REENT by Ambarella, r1) (GNU Arm Embedded Toolchain 10-2020-q4-major-amba-A_R-Profile_r1_20210623)"
            ;;
        *)
            PATH_32b=/usr/local/gcc-arm-none-eabi-10-2020-q4-major-amba-A_R-Profile_r1_20210623/bin
            PTN_32b="gcc version 10.2.1 20201103 (release)(with DYNAMIC_REENT by Ambarella, r1) (GNU Arm Embedded Toolchain 10-2020-q4-major-amba-A_R-Profile_r1_20210623)"
            ;;
    esac
    PATH_64b=/usr/local/gcc-linaro-10.2.1-amba-r1_20210622-x86_64_aarch64-none-elf/bin

    CROSS_32b=arm-none-eabi-
    CROSS_64b=aarch64-none-elf-

    # Toolchain patterns
    PTN_64b="gcc version 10.2.1 20201103(with DYNAMIC_REENT by Ambarella, r1) (GCC)"

    # SDK release pattern
    PTN_RLS=""
}

function fn_amba_env_unset()
{
    # Unset variable of scripts
    unset ENV_COMMON
    unset E_FILE
    unset PATH_ENV

    # Unset patterns
    unset PATH_32b
    unset PATH_64b

    unset CROSS_32b
    unset CROSS_64b

    unset PTN_32b
    unset PTN_64b
    unset PTN_RLS
}


# Debug end
[ "${D_COMM}" = "1" ] && set +x
unset D_COMM

