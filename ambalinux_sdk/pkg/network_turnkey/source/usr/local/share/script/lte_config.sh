#!/bin/sh

LOGGER ()
{
    if [ "${sysl}" != "" ]; then
        logger "${b0}:${@}"
    else
        echo "$@"
    fi
}

sysl=`ps | grep syslogd | grep -v grep`
b0=`basename ${0}`

at_dev="/dev/ttyUSB3"
C_OPER="Unknown"
C_APN="internet"
C_USER=""
C_PASS=""

if [ "`lsusb | grep 2c7c:6002`" != "" ]; then
    at_dev="/dev/ttyUSB2"
else
    at_dev="/dev/ttyUSB3"
fi

send_at_cmd()
{
    if [ -z "$1" ]; then
        return 1
    fi
    if ! ans=$(echo -e "$1\r" | microcom -s 115200 -t 1000 ${at_dev}) || ! echo "${ans}" | grep -q 'OK'; then
        echo "${ans}"
        return 1
    fi
    echo "${ans}" | grep -v "OK\|$1" | tr -d '\r' || :
    return 0
}

send_at_cmd_rtr()
{
    n=1
    while true; do
        ans=$(send_at_cmd "$1") && break
        if [ ${n} -lt 3 ]; then
            n=$(echo "${n} + 1" | bc -l)
        else
            echo "AT command failed: $1 -> ${ans}" >&2
            return 1
        fi
    done
    at_cmd=$(echo "$1" | tr '[:lower:]' '[:upper:]' | sed 's/^AT[^0-9A-Z]*\([^?=]*\).*$/\1/g') || :
    echo "${ans}" | sed "s/^[ \t+]*${at_cmd}:[ \t]*\(.*\)$/\1/g" | grep . || :
    return 0
}

function check_pin()
{
    local n=0
    local PIN_STATE=""
    while [ "${PIN_STATE}" != "READY" ] && [ $n -ne 5 ]; do
        n=$(($n + 1))
        local PIN_STATE=$(send_at_cmd_rtr "AT+CPIN?") || :
    done
    [ "${PIN_STATE}" == "READY" ] && return 0
    return 1
}

send_at_cmd "ATE0"
#send_at_cmd "AT+QGPS=1"
if [ "`lsusb | grep 2c7c:6007`" != "" ]; then
	send_at_cmd 'AT+QCFG="band",0,42000000000000381A'
fi

# load usb wifi driver
#if [ ! -f /tmp/usb_wifi_loaded ]; then
#	echo `date` > /tmp/usb_wifi_loaded
#	mac=`burn_info lte read wifi_mac`
#	echo "${mac}" > /tmp/usb_wifi_mac
#	if [ "${mac}" != "00:00:00:00:00:00" ] &&  [ "${mac}" != "" ] && [ ${#mac} == 17 ]
#	then
#		insmod /lib/modules/8188fu.ko rtw_initmac=${mac}
#	else
#		insmod /lib/modules/8188fu.ko
#	fi
#	echo 0 > /proc/net/rtl8188fu/log_level
#fi

# for remote wakeup
#send_at_cmd "AT+QSCLK=1"
#send_at_cmd "AT+QSCLKEX=1,2,2"
#send_at_cmd "AT+QCFG=\"risignaltype\",\"physical\""
#send_at_cmd "AT+QCFG=\"urc/ri/ring\",\"pulse\",2000,1000,5000,\"off\",1"
#send_at_cmd "AT+QCFG=\"urc/ri/other\",\"pulse\",2000"
sleep 3
echo -e 'AT+QSCLK=1\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+QCFG="risignaltype","physical"\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+QCFG="urc/ri/ring","pulse",2000,1000,5000,"off",1\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+QCFG="urc/ri/other","pulse",2000\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+CNMI=2,1\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+CMGF=1\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+QURCCFG="urcport","usbmodem"\r' | microcom -s 115200 -t 1000 ${at_dev} && \
echo -e 'AT+QCFG="urc/ri/smsincoming","pulse",2000,1\r' | microcom -s 115200 -t 1000 ${at_dev}

PIN_STATE=""
check_pin && PIN_STATE="READY"
if [ "$PIN_STATE" != "READY" ]; then
    LOGGER "No SIM-chip detected. Nothing to do."
    echo 'ltebootdone' > /sys/module/rpmsg_echo/parameters/example_printk
    exit 1
fi

CIMI=$(send_at_cmd_rtr "AT+CIMI") || :
if [ "$CIMI" != "" ]; then
    MCC=${CIMI:0:3}
    MNC=${CIMI:3:2}
fi
if [ "${MCC}" == "" ] || [ "${MNC}" == "" ]; then
    LOGGER "CIMI failed, fallback to COPS"
    send_at_cmd "AT+COPS=3,2"
    n=0
    while [ "${MCC}" == "" ] && [ $n -ne 5 ]; do
        COPS=$(send_at_cmd_rtr "AT+COPS?") || :
        n=$(($n + 1))
        MCC=`echo $COPS | grep -o -E "\".*\"" | sed -e 's/"//g' | cut -c 1-3`
        MNC=`echo $COPS | grep -o -E "\".*\"" | sed -e 's/"//g' | cut -c 4-5`
    done
fi
if [ "$MNC" != "" ] && [ "$MCC" != "" ]; then
    LOGGER "MCC: $MCC MNC: $MNC"
    CONFIG=$(grep -E "$MCC"$'\t'"$MNC" /usr/local/share/db/apns.tsv) || :
    if [ "$CONFIG" != "" ]; then
        LOGGER "CONFIG:$CONFIG"
        C_OPER=$(echo "$CONFIG" | awk -F$'\t' '{print $4}')
        C_APN=$(echo "$CONFIG" | awk -F$'\t' '{print $5}')
        C_USER=$(echo "$CONFIG" | awk -F$'\t' '{print $6}')
        C_PASS=$(echo "$CONFIG" | awk -F$'\t' '{print $7}')
    else
        LOGGER "Operator $MCC-$MNC not found in database. Using default config."
    fi
else
    LOGGER "Cannot get MNC,MCC. Using default config."
fi
LOGGER "Operator:$C_OPER\nAPN:$C_APN\nUser:$C_USER\nPassword:$C_PASS"
#ifconfig wwan0 down
#echo 1 > /sys/class/net/wwan0/qmi/raw_ip
#ifconfig wwan0 up
quectel-CM -s $C_APN $C_USER $C_PASS -f /tmp/lte.log &

echo 'ltebootdone' > /sys/module/rpmsg_echo/parameters/example_printk
