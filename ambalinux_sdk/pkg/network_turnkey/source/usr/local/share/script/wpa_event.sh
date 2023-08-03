#!/bin/sh
#/*
# * Author:
# *	Michael Yao<ccyao@ambarella.com>
# *
# * Copyright (C) 2013, Ambarella Inc.
# */

IFNAME=$1
CMD=$2

sysl=`ps | grep syslogd | grep -v grep`
LOGGER ()
{
	if [ "${sysl}" == "" ]; then
		echo "$@"
	else
		logger "$@"
	fi
}

if [ "${IFNAME}" == "" ] || [ "${CMD}" == "" ]; then
	echo "wrong usage"
	exit 0
fi

LOGGER "$0 $@"

getP2PIF ()
{
	if [ -e /sys/module/bcmdhd ] || [ -e /sys/module/cywdhd ] || [ -e /sys/module/ifxahd ]; then
		kver=`uname -r | cut -d '.' -f 1`
		if [ ${kver} -eq 2 ]; then
			P2PIF=p2p-p2p0-0
		else
			P2PIF=`ls /sys/class/net/|grep "p2p-wlan"`
		fi
	else
		P2PIF=${IFNAME}
	fi

	if ([ -e /sys/module/bcmdhd ] || [ -e /sys/module/cywdhd ] || [ -e /sys/module/ifxahd ]) && [ ${kver} -eq 3 ]; then
		n=0
		while [ "${P2PIF}" == "" ] && [ $n -ne 2 ]; do
			all_net=`ls /sys/class/net/`
			LOGGER "ls /sys/class/net/: ${all_net}"
			P2PIF=`echo "${all_net}"|grep "p2p-wlan"`
			n=$(($n + 1))
			sleep 0.5
		done
		LOGGER "P2PIF=${P2PIF}"
	fi

	if [ "${P2PIF}" == "" ]; then
		LOGGER "err: cannot find p2p interface"
	fi
}

p2p_go ()
{
	if [ "${conf}" == "" ]; then
		if [ -f /mnt/extsd/settings.ini ]; then
			conf=`cat mnt/extsd/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
		else
			conf=`cat /pref/settings.ini | grep -Ev "^#" | grep -Ev "^\["`
		fi
	fi

	#LOCAL_IP
	LOCAL_IP=`echo "${conf}" | grep local_ip | cut -c 10-`
	LOGGER "LOCAL_IP=${LOCAL_IP}"
	getP2PIF
	ifconfig ${P2PIF} $LOCAL_IP
	if [ $? -ne 0 ]; then
		return 1
	fi
	route add default gw $LOCAL_IP

	ifconfig ${P2PIF} netmask $LOCAL_NETMASK
	if [ $? -ne 0 ]; then
		return 1
	fi
}

#${IFNAME} P2P-GROUP-STARTED ${IFNAME} GO ssid=DIRECT-AN freq=2437 passphrase=Ooj4S5D1 go_dev_addr=00:03:7f:04:e0:99
#${IFNAME} P2P-GROUP-STARTED ${IFNAME} client ssid="DIRECT-FY" freq=2437 passphrase="JBmB9RjP" go_dev_addr=00:03:7f:dd:ee:ff
#${IFNAME} P2P-GROUP-STARTED ${IFNAME} client ssid=DIRECT-Uo-android344 freq=2462 psk=ddbdcde17f1e59f8111b3697b04b309f23fdde75a3d98f8d4fc882f7140c0afa go_dev_addr=86:7a:88:70:af:7e [PERSISTENT]
if [ "$CMD" == "P2P-GROUP-STARTED" ]; then
	if [ "${4}" == "GO" ]; then
		p2p_go
	fi
fi

#${IFNAME} P2P-PROV-DISC-PBC-REQ 2a:98:7b:d3:c7:ed p2p_dev_addr=2a:98:7b:d3:c7:ed pri_dev_type=10-0050F204-5 name=Android_35a0 config_methods=0x80 dev_capab=0x27 group_capab=0x0
if [ "$CMD" == "P2P-PROV-DISC-PBC-REQ" ]; then
	wpa_cli wps_pbc
fi
