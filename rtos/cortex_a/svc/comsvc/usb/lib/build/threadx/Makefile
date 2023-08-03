ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

obj-y += libcomsvc_usb.a

COMSVC_USB_SRC = ../../src/common

libcomsvc_usb-objs += $(COMSVC_USB_SRC)/AmbaSvcUsb.o
libcomsvc_usb-objs += $(COMSVC_USB_SRC)/AmbaSvcUsbCmd.o
libcomsvc_usb-objs += $(COMSVC_USB_SRC)/MSCDevice.o
libcomsvc_usb-objs += $(COMSVC_USB_SRC)/MTPDevice.o
libcomsvc_usb-objs += $(COMSVC_USB_SRC)/UVCDevice.o
libcomsvc_usb-objs += $(COMSVC_USB_SRC)/PlatformPort.o

ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb/usbx
ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/usb/inc
ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/tune/ituner
ccflags-y += -I$(srctree)/svc/comsvc/usb/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/svc/apps/icam/core/src/sys/inc
ccflags-y += -I$(srctree)/svc/apps/iout/src/diag/common/usb/device_classes/uvc
ccflags-y += -I$(src)/$(COMSVC_USB_SRC)

install-files := libcomsvc_usb.a
