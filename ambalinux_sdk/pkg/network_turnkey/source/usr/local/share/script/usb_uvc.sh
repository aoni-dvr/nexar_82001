#!/bin/sh

echo device > /proc/ambarella/usbphy0
modprobe usbcore
modprobe ehci-hcd
modprobe ohci-hcd
modprobe ambarella_udc
modprobe libcomposite
modprobe g_webcam
