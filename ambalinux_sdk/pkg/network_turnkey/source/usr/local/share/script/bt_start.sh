#!/bin/sh

if [ -f /mnt/extsd/BtDisable.txt ]; then
	exit 0
fi

if [ -f /mnt/extsd/WifiTestFile.txt ] || [ -f /mnt/extsd/UsbWifiTestFile.txt ]; then
	exit 0
fi

# use nice to prevent data loss
CMD_NICER ()
{
	echo "$@"
	nice -n -20 $@
}

CMD_LOGGER ()
{
	echo "$@"
	$@
}

reset_conf ()
{
	echo "reset bt.conf"
	btconf=`cat /usr/local/share/script/bt.conf | sed -e 's/$/\r/'`
}

reset_bluez ()
{
	killall bluetoothd bluetoothctl 2>/dev/null
	echo "reset bluetoothd"
	rm -f /pref/bluetooth/* #${fuse_d}/MISC/bluetooth/*
	START_BLUEZ
}

START_BLUEZ ()
{
	if [ "${bluez_5}" == "1" ]; then
		sed -i -e 's|\(.*\)PairableTimeout =\(.*\)|PairableTimeout = '${BT_PAIRABLE_TIMEOUT}'|g' /tmp/bluetooth/main.conf
		sed -i -e 's|\(.*\)DiscoverableTimeout =\(.*\)|DiscoverableTimeout = '${BT_DISCOVERABLE_TIMEOUT}'|g' /tmp/bluetooth/main.conf

		if [ "${GATT_PERIPHERAL}" == "yes" ] && [ "${bluez_5_29_higher}" == "" ]; then
			#bluez less or equal 5.28: use gatt-example plugin
			CMD_LOGGER /usr/libexec/bluetooth/bluetoothd -E -C -a
		else
			#bluez greater or equal 5.29: gatt-example plugin do not work
			CMD_LOGGER /usr/libexec/bluetooth/bluetoothd -E -C
		fi
	else
		if [ "${GATT_PERIPHERAL}" == "yes" ]; then
			CMD_LOGGER bluetoothd -a
		else
			CMD_LOGGER bluetoothd
		fi
	fi
}

wait_hci0 ()
{
	n=0
	#while [ ! -e /sys/class/bluetooth/hci0/address ] && [ $n -ne 30 ]; do
	while [ ! -e /sys/class/bluetooth/hci0 ] && [ $n -ne 30 ]; do
		n=$(($n + 1))
		#CMD_LOGGER sleep 0.1
		sleep 0.1
	done
	if [ ! -e /sys/class/bluetooth/hci0 ]; then
		echo "There is no BT interface!"
		#exit 1
	fi
}

HCI0_BRINGUP ()
{
	if [ "${UART_NODE}" == "" ]; then
		reset_conf
		return 1
	fi

	UART_BUSY=`grep ${UART_NODE##/*/} /etc/inittab`
	if [ "${UART_BUSY}" != "" ]; then
		echo "Wrong bluetooth UART config in /etc/inittab: ${UART_BUSY}"
		exit 1
	fi

	if [ -e /proc/ambarella/uart1_rcvr ]; then
		#echo "set UART1 to 1 byte FIFO threshold"
		echo 0 > /proc/ambarella/uart1_rcvr
	fi

	if [ "${BT_EN_STATUS}" == "" ]; then
		BT_EN_STATUS=1
	fi
	if [ "${BT_EN_GPIO}" != "" ] && [ ! -e /sys/module/sd8xxx ]; then
		/usr/local/share/script/t_gpio.sh ${BT_EN_GPIO} $(($(($BT_EN_STATUS + 1)) % 2))
		/usr/local/share/script/t_gpio.sh ${BT_EN_GPIO} ${BT_EN_STATUS}
		#CMD_LOGGER sleep 0.1
		sleep 0.1
	fi

	# if [ "${HCI_DRIVER}" == "mrvl" ] && [ "`ls /sys/bus/sdio/devices`" == "" ]; then
	# 	wait_mmc_add
	# fi
	if [ "${UART_BAUD}" == "" ]; then
		UART_BAUD=750000
	fi

	if [ "${HCI_DRIVER}" == "brcm" ]; then
		if [ -e /usr/local/cypress ]; then
			bt_hcd_path=/usr/local/cypress/bt.hcd
		else
			bt_hcd_path=/usr/local/bcmdhd/bt.hcd
		fi
		mac=`burn_info bt_mac`
		echo "${mac}" > /tmp/bt_mac
		if [ "${mac}" != "00:00:00:00:00:00" ] &&  [ "${mac}" != "" ] && [ "${#mac}" == 17 ]; then
			CMD_NICER brcm_patchram_plus --enable_hci --baudrate ${UART_BAUD} --use_baudrate_for_download --patchram ${bt_hcd_path} --no2bytes ${UART_NODE} --scopcm=0,2,1,0,0,0,0,0,0,0 --bd_addr ${mac}
		else
			CMD_NICER brcm_patchram_plus --enable_hci --baudrate ${UART_BAUD} --use_baudrate_for_download --patchram ${bt_hcd_path} --no2bytes ${UART_NODE} --scopcm=0,2,1,0,0,0,0,0,0,0
		fi
		#enable bluesleep dynamic power saving
		#echo 1 > /proc/bluetooth/sleep/proto
	elif [ "${HCI_DRIVER}" == "rtk_btusb" ]; then
		if [ -e /sys/module/rtk_btusb/parameters/bd_addr ]; then
			if [ -e /tmp/wifi1_mac ]; then
				cat /tmp/wifi1_mac > /sys/module/rtk_btusb/parameters/bd_addr
			fi
		else
			if [ -e /tmp/wifi1_mac ]; then
				insmod /usr/local/*/rtk_btusb.ko bd_addr=`cat /tmp/wifi1_mac`

			else
				insmod /usr/local/*/rtk_btusb.ko
			fi
		fi
		rtl8723bu_workaround=`lsmod|grep ehci`
		if [ "${rtl8723bu_workaround}" == "" ]; then
			modprobe ehci_hcd 2> /dev/null
			echo "host" > /proc/ambarella/usbphy0
			echo -e "\e[1;31m Apply workaround for rtl8723bu iOS cannot connect BLE: sleep 4\n\n \e[0m"
			sleep 4
		fi
	elif [ "${HCI_DRIVER}" == "mrvl" ]; then
		if [ -e /tmp/wifi1_mac ]; then
			insmod /usr/local/mrvl/bt8xxx.ko drv_mode=1 psmode=1 cal_cfg=mrvl/bt_cal_data.conf bt_mac=`cat /tmp/wifi1_mac`
		else
			insmod /usr/local/mrvl/bt8xxx.ko drv_mode=1 psmode=1 cal_cfg=mrvl/bt_cal_data.conf
		fi
	else
		HCIATTACH=hciattach
		which rtk_hciattach && HCIATTACH=rtk_hciattach
		if [ -e /tmp/wifi1_mac ]; then
			CMD_NICER ${HCIATTACH} ${UART_NODE} ${HCI_DRIVER} ${UART_BAUD} flow sleep -d -m /tmp/wifi1_mac
		else
			CMD_NICER ${HCIATTACH} ${UART_NODE} ${HCI_DRIVER} ${UART_BAUD} flow sleep -d
		fi
	fi

	wait_hci0

	#save /tmp/lib/bluetooth
	mounted=`cat /proc/mounts | grep /tmp/lib/bluetooth`
	if [ "${mounted}" == "" ]; then
		mkdir -p /tmp/lib/bluetooth/${wifi1_mac}
		#mkdir -p ${fuse_d}/MISC/bluetooth
		#if [ -e ${fuse_d}/MISC/bluetooth ]; then
		#	echo "skip mount --bind ${fuse_d}/MISC/bluetooth/ /tmp/lib/bluetooth/${wifi1_mac}"
		#	#mount --bind ${fuse_d}/MISC/bluetooth/ /tmp/lib/bluetooth/${wifi1_mac}
		#fi
	fi

	START_BLUEZ
#	CMD_NICER hciconfig hci0 up
	echo 'power on' | bluetoothctl
}

DO_BT_AGENT ()
{
	if [ "${bluez_5}" == "1" ]; then
		if [ "${BT_LEGACY_PIN}" == "" ]; then
			CMD_LOGGER bluetoothctl -d -p 0000 -a NoInputNoOutput
		else
			CMD_LOGGER bluetoothctl -d -p ${BT_LEGACY_PIN} -a NoInputNoOutput
		fi
	else
		if [ "${BT_LEGACY_PIN}" == "" ]; then
			CMD_LOGGER bt-agent -d --auto_pk -s NoInputNoOutput
		else
			echo -e "\033[032m Bluetooth PIN code=${BT_LEGACY_PIN} \033[0m"
			CMD_LOGGER bt-agent -d --auto_pk -p ${BT_LEGACY_PIN}
		fi
		#CMD_LOGGER bt-monitor_headset -s /usr/local/share/script/bt_speakergain.sh -d
	fi
}

SET_PISCAN ()
{
	if [ "${bluez_5}" == "1" ]; then
		if [ "${ISCAN}" == "yes" ]; then
			echo 'discoverable on'| bluetoothctl
		else
			echo 'discoverable off'| bluetoothctl
		fi
		if [ "${PSCAN}" == "yes" ]; then
			echo 'pairable on'| bluetoothctl
		else
			echo 'pairable off'| bluetoothctl
		fi
	else
		if [ "${ISCAN}" == "yes" ] && [ "${PSCAN}" == "yes" ]; then
			CMD_NICER hciconfig hci0 piscan
		elif [ "${ISCAN}" == "yes" ]; then
			CMD_NICER hciconfig hci0 iscan
		elif [ "${PSCAN}" == "yes" ]; then
			CMD_NICER hciconfig hci0 pscan
		else
			CMD_NICER hciconfig hci0 noscan
		fi
		if [ "${BT_DISCOVERABLE_TIMEOUT}" != "0" ]; then
			CMD_LOGGER bt-adapter --set DiscoverableTimeout ${BT_DISCOVERABLE_TIMEOUT}
		fi
		if [ "${BT_PAIRABLE_TIMEOUT}" != "0" ]; then
			CMD_LOGGER bt-adapter --set PairableTimeout ${BT_PAIRABLE_TIMEOUT}
		fi
	fi
}

##### main ##########################################

if [ "${1}" == "fast" ]; then
	hciconfig hci0 pscan
	hciconfig hci0 leadv
	exit 0
fi

bt_conf=`cat /usr/local/share/script/bt.conf | grep -Ev "^#"`
export `echo "${bt_conf}"|grep -vI $'^\xEF\xBB\xBF'`
which bluetoothctl && bluez_5=1 && which btgatt-server && bluez_5_29_higher=1

if [ "${DEVICE_NAME}" = "" ]; then
	DEVICE_NAME=Nexar-`hwid | sed 's/.*\(....\)$/\1/'`
fi
sed -i -e 's|\(.*\)Name =\(.*\)|Name = '${DEVICE_NAME}'|g' /tmp/bluetooth/main.conf

if [ ! -e /sys/class/bluetooth/hci0 ]; then
	HCI0_BRINGUP ${@}
else
	START_BLUEZ
	CMD_NICER hciconfig hci0 up
fi

SET_PISCAN

#DO_BT_AGENT

#in case bluetoothd is dead
tmp=`ps|grep -v grep|grep bluetoothd`
if [ "${tmp}" == "" ]; then
	reset_bluez
	#DO_BT_AGENT
fi

if [ "${GATT_PERIPHERAL}" == "yes" ] && [ "${bluez_5_29_higher}" == "1" ]; then
	CMD_LOGGER btgatt-server -n ${DEVICE_NAME}
fi

#wait [CHG] Controller xx:xx:xx:xx:xx:xx Powered: yes
if [ "${BT_LEGACY_PIN}" == "" ]; then
	CMD_NICER hciconfig hci0 sspmode 1
else
	CMD_NICER hciconfig hci0 sspmode 0
fi

# enable ibeacon
# enable LE advertising
#hciconfig hci0 leadv 3
#02 ,// AD structure的长度 这里表示接下来还有两个字节
#01, //AD Type 这是由蓝牙联盟定义的标识，这里表示蓝牙支持类型
#06, //0x06=0000 0110 每个位的意义如下
#        bit0=0 OFF // LE Limited Discoverable Mode
#        bit1=1 ON  // LE General Discoverable Mode
#        bit2=1 ON  // BR/EDR Not Supported
#        bit3=0 OFF // Simultaneous LE and BR/EDR to Same Device Capable (controller)
#        bit4=0 OFF //Simultaneous LE and BR/EDR to Same Device Capable (Host)
#1a,               // AD structure长度，这里是26个字节
#ff,               // ADType. 因为ibeacon并未写入官方协议，这里使用自定义类型Manufacturer Specific Data
#4c, 00,          //这个指公司， 00 4c是APPLE
#02, 15,          //这是ibeacon的标识，指示这个广播是ibeacon广播
#8a21c76b-74e3-4341-92be-33fcfdefdd63 ,//16字节的proximity UUID 由设备厂商和设备用途决定
#27, 11,           // Major 
#30, 39           //Minor  Major和Minor共同决定设备的编号
#1c               //1米距离上的信号功率
#hcitool -i hci0 cmd 0x08 0x0008 1E 02 01 06 1A FF 4C 00 02 15 8A 21 C7 6B 74 E3 43 41 92 BE 33 FC FD EF DD 63 27 11 30 39 1C 00

echo 'btbootdone' > /sys/module/rpmsg_echo/parameters/example_printk
