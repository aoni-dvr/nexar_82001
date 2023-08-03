ifeq ($(CONFIG_BUILD_SSP_USB_LIBRARY),y)

ifeq ($(CONFIG_BUILD_SSP_USBX_LIBRARY),y)
# Build USBX libraries

ifeq ($(CONFIG_ARM64),y)

## 64-bit

USBX_SRC_PATH = ../../../express_logic/usbx64

ifneq ($(wildcard $(srctree)/vendors/express_logic/usbx64),)
include $(src)/$(USBX_SRC_PATH)/Makefile.in
endif

else

## 32-bit

USBX_SRC_PATH = ../../../express_logic/usbx

ifneq ($(wildcard $(srctree)/vendors/express_logic/usbx),)
include $(src)/$(USBX_SRC_PATH)/Makefile.in
endif

endif

endif # $(CONFIG_BUILD_SSP_USBX_LIBRARY)

# Build USB libraries

libusb_sys-objs += \
        src/AmbaUSB_Buffer.o \
        src/AmbaUSB_Utility.o \
        src/AmbaUSBD_Descriptor.o \
		src/AmbaUSBD_Drv_Function.o \
		src/AmbaUSBD_System.o \
        src/AmbaUSBH_System.o \
		src/AmbaVer_UsbSys.o

ifeq ($(CONFIG_BUILD_SYNOPSYS_USB2_DRIVER),y)

# Synopsys USB driver
libusb_sys-objs += \
        src/AmbaUSBD_Drv_ConnectChange.o \
        src/AmbaUSBD_Drv_EndpointCreate.o \
        src/AmbaUSBD_Drv_EndpointDestroy.o \
		src/AmbaUSBD_Drv_EndpointReset.o \
		src/AmbaUSBD_Drv_EndpointStall.o \
		src/AmbaUSBD_Drv_EndpointStatus.o \
		src/AmbaUSBD_Drv_FrameNumGet.o \
		src/AmbaUSBD_Drv_InitComplete.o \
		src/AmbaUSBD_Drv_Phy.o \
		src/AmbaUSBD_Drv_TransferAbort.o \
		src/AmbaUSBD_Drv_TransferCallback.o \
		src/AmbaUSBD_Drv_TransferRequest.o \
		src/AmbaUSBD_Drv_SetupHandler.o \
		src/AmbaUSBD_Drv_Udc.o \
		src/AmbaUSBD_Drv_UdcInterrupt.o \
		src/AmbaUSBH_Drv_Uhc.o

endif

ifeq ($(CONFIG_BUILD_CADENCE_USB3_DRIVER),y)

# Cadence USB driver
libusb_sys-objs += \
        src/cadence/AmbaUSBD_Interface.o \
        src/cadence/AmbaUSBD_Interrupt.o \
        src/cadence/AmbaUSBD_SetupHandler.o \
        src/cadence/AmbaUSBH_Interface.o \
        src/cadence/USBCDNS3_Cadence.o \
        src/cadence/USBCDNS3_Controller.o \
        src/cadence/USBCDNS3_Debug.o \
        src/cadence/USBCDNS3_DeviceCmd.o \
        src/cadence/USBCDNS3_DeviceInit.o \
        src/cadence/USBCDNS3_DeviceIsrHandler.o \
        src/cadence/USBCDNS3_EndpointEnable.o \
        src/cadence/USBCDNS3_EndpointStop.o \
        src/cadence/USBCDNS3_HostCmd.o \
        src/cadence/USBCDNS3_HostControlTransfer.o \
        src/cadence/USBCDNS3_HostInit.o \
        src/cadence/USBCDNS3_HostIsrHandler.o \
        src/cadence/USBCDNS3_InitRun.o \
        src/cadence/USBCDNS3_Phy.o \
        src/cadence/USBCDNS3_SanityCheck.o \
        src/cadence/USBCDNS3_Transfer.o \
        src/cadence/USBCDNS3_Utility.o \
        src/cadence/USBCDNS3_XhcCmd.o \
        src/cadence/ux_dcd_endpoint_create.o \
        src/cadence/ux_dcd_endpoint_destroy.o \
        src/cadence/ux_dcd_endpoint_reset.o \
        src/cadence/ux_dcd_endpoint_stall.o \
        src/cadence/ux_dcd_endpoint_status.o \
        src/cadence/ux_dcd_frame_number_get.o \
        src/cadence/ux_dcd_transfer_abort.o \
        src/cadence/ux_dcd_transfer_request.o \
        src/cadence/ux_hcd_xhci_destroy.o \
        src/cadence/ux_hcd_xhci_endpoint_create.o \
        src/cadence/ux_hcd_xhci_endpoint_destroy.o \
        src/cadence/ux_hcd_xhci_endpoint_reset.o \
        src/cadence/ux_hcd_xhci_frame_number_get.o \
        src/cadence/ux_hcd_xhci_port_reset.o \
        src/cadence/ux_hcd_xhci_port_status_get.o \
        src/cadence/ux_hcd_xhci_request_abort.o \
        src/cadence/ux_hcd_xhci_request_transfer.o \
        src/cadence/ux_hcd_xhci_stop.o
endif

obj-y += libusb_sys.a
install-files += libusb_sys.a

libusb_cdcacmd-objs += \
        src/AmbaUSBD_CdcAcm.o \
        src/AmbaVer_UsbCdcAcmd.o

obj-y += libusb_cdcacmd.a
install-files += libusb_cdcacmd.a

libusb_mscd-objs += \
        src/AmbaUSBD_Msc.o \
        src/AmbaVer_UsbMscd.o

obj-y += libusb_mscd.a
install-files += libusb_mscd.a

libusb_mtpd-objs += \
        src/AmbaUSBD_Mtp.o \
        src/AmbaVer_UsbMtpd.o

obj-y += libusb_mtpd.a
install-files += libusb_mtpd.a

libusb_msch-objs += \
        src/AmbaUSBH_Msc.o \
		src/AmbaUSBH_Msc_PrFile.o \
        src/AmbaVer_UsbMsch.o

obj-y += libusb_msch.a
install-files += libusb_msch.a

CFLAGS_AmbaVer_UsbSys.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

CFLAGS_AmbaVer_UsbMscd.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

CFLAGS_AmbaVer_UsbMsch.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

CFLAGS_AmbaVer_UsbMtpd.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

CFLAGS_AmbaVer_UsbCdcAcmd.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

CFLAGS_AmbaVer_UsbUvc.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

include $(src)/src/device_classes/uvc/Makefile.mk

ccflags-y += -D_AMBA_KAL_ \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
		-I$(srctree)/vendors/ambarella/inc/usb \
		-I$(srctree)/vendors/ambarella/inc/usb/usbx \
		-I$(srctree)/vendors/ambarella/inc/usb/usbx/usbx \
		-I$(srctree)/vendors/ambarella/inc/usb/usbx/usbx_device_classes \
		-I$(srctree)/vendors/ambarella/inc/usb/usbx/usbx_host_controllers \
		-I$(srctree)/vendors/ambarella/inc/usb/usbx/usbx_host_classes \
        -I$(srctree)/vendors/ambarella/inc/prfile2 \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/rtsl \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/vendors/ambarella/wrapper/usb/inc \
        -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg

#ccflags-y += -Werror

ccflags-y += -D__CORE__=5 -D__ARM4TM__=1 -D__CPU_MODE__=2
ccflags-y += -D_AMBA_KAL_

endif # $(CONFIG_BUILD_SSP_USB_LIBRARY)
