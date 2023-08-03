#!/bin/bash

[ "x${V}" != "x" ] && set -x

[ "x${DEBUG}" != "x" ] && DEBUG=1 || DEBUG=0

# We need access to CONFIG_ symbols
case "${KCONFIG_CONFIG}" in
*/*)
    . "${KCONFIG_CONFIG}"
    ;;
*)
    # Force using a file from the current directory
    . "./${KCONFIG_CONFIG}"
esac

if [ "${CONFIG_CPU_ARMV7}" == "y" ]; then
    QEMU=`which qemu-system-arm`
else
    QEMU=`which qemu-system-aarch64`
fi

if [ "x${QEMU}" = "x" ]; then
    echo "Can NOT find qemu. Can be installed with 'apt install qemu-system-arm'"
    exit
else
    # Semihost features improvement after v4.1
    # cf. https://wiki.qemu.org/ChangeLog/4.1
    ver_maj=`${QEMU} --version | grep version | awk -F 'version' '{print $2}' | awk -F '.' '{print $1}' | tr -d ' '`
    ver_min=`${QEMU} --version | grep version | awk -F 'version' '{print $2}' | awk -F '.' '{print $2}' | tr -d ' '`
    if [ "${ver_maj}" -lt "4" ]; then
        echo "Need qemu v4.1 or later"
        exit
    elif [ "${ver_maj}" -eq "4" ] && [ "${ver_min}" -lt "1" ]; then
        echo "Need qemu v4.1 or later"
        exit
    fi
fi

if [ "${CONFIG_CPU_ARMV7}" == "y" ]; then
    PARM="-M vexpress-a9"
else
    if [ "${CONFIG_CPU_CORTEX_A53}" == "y" ]; then
        if [ "${CONFIG_ARM32}" == "y" ]; then
            # QEMU does not support AArch64 switch to AArch32 directly. It needs support from KVM.
            PARM="-machine virt,secure=on -cpu cortex-a15"
        else
            PARM="-machine virt,secure=on -cpu cortex-a53"
        fi
    else
        # QEMU has no CA-76 yet (until v6.1)
        PARM="-machine virt,secure=on -cpu cortex-a53"
    fi
fi
PARM="${PARM} -smp 1"
PARM="${PARM} -m 512M"
PARM="${PARM} -nographic -display none"
PARM="${PARM} -chardev stdio,mux=on,id=stdio0"
PARM="${PARM} -mon chardev=stdio0,mode=readline -serial null"
PARM="${PARM} -semihosting-config enable=on,target=native,chardev=stdio0"
PARM="${PARM} -no-reboot"


if [ "${DEBUG}" != "0" ]; then
    # For Debug
    PARM="${PARM} -s -S"
fi

if [ -s $1 ]; then

    [ "${DEBUG}" != "0" ] && echo "Start to debug $1 @ :1234 ..."

    export QEMU_AUDIO_DRV=none
    ${QEMU} ${PARM} -kernel $1

    echo -e "\nDone..."
fi

