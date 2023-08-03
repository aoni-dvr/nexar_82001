#!/bin/sh

LOGGER ()
{
	if [ "${sysl}" != "" ]; then
		logger "${b0}: ${@}"
	else
		echo "$@"
	fi
}

sysl=`ps | grep syslogd | grep -v grep`
b0=`basename ${0}`

while true
do
	ntpdate cn.pool.ntp.org > /dev/null 2>&1
	if [ $? -eq 0 ]; then
		hwclock -w
		LOGGER "sync time success. exit!"
		break
	fi
	sleep 5
done
