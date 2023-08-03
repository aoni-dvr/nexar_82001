TARGET_APP_LIB := libsvc_app
TARGET_APP_SRC_PATH := src
obj-y += $(TARGET_APP_LIB).a
install-files := $(TARGET_APP_LIB).a

KBUILD_CFLAGS := $(subst -Werror ,,$(KBUILD_CFLAGS))

# Link script
ifneq ($(CONFIG_XEN_SUPPORT),)
obj-y += amba_svc_xen.lds
else
ifneq ($(CONFIG_THREADX),)
obj-y += amba_svc.lds
endif
ifneq ($(CONFIG_LINUX),)
obj-y += amba_svc_linux.lds
endif
endif

ifeq ($(CONFIG_THREADX),y)
$(TARGET_APP_LIB)-objs := $(TARGET_APP_SRC_PATH)/threadx/AmbaUserSysInit.o
ifeq ($(CONFIG_THREADX64),y)
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaBackTrace_A64.o
else
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaBackTrace.o
endif
include $(srctree)/svc/apps/iout/$(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/Makefile.mk
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdGDMA.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdI2C.o
endif

ifneq ($(CONFIG_LINUX),)
$(TARGET_APP_LIB)-objs := src/linux/main.o
$(TARGET_APP_LIB)-objs += src/linux/AmbaIOUTDiag_CmdDiag.o
$(TARGET_APP_LIB)-objs += src/linux/AmbaIOUTDiag_CmdGDMA.o
$(TARGET_APP_LIB)-objs += src/linux/AmbaIOUTDiag_CmdI2C.o
endif

$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdFio.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdConsole.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdDMA.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdDRAM.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdGPIO.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdOS.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdPWM.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdSPI.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdUART.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdWDT.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdCRC.o
ifeq ($(CONFIG_BUILD_SSP_USB_LIBRARY),y)
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AmbaIOUTDiag_CmdUsb.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCACMDesc.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCACMDevice.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MSCDevice.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MSCHost.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MTPDataBase.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MTPDesc.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MTPDevice.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MTPDeviceProp.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_MTPObjectProp.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_PlatformPort.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_SoftwareFifo.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_Util.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_UVCDesc.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_UVCDevice.o
ifeq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),y)
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCMSCDesc.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCMSCDevice.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCMTPDesc.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCMTPDevice.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCACMMultiDesc.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCACMMultiClass.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/usb/AppUSB_CDCACMMultiDevice.o
endif
endif
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdRNG.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdRandVerify.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/AmbaIOUTDiag_CmdVIN.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdVOUT.o
ifeq ($(or $(CONFIG_SOC_CV2FS),$(CONFIG_SOC_CV22FS)),)
# non CV2FS/CV22FS
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdCVBS.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdIR.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdADC.o
ifeq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),y)
ifeq ($(CONFIG_BUILD_COMSVC_DRAMSHMOO),y)
else
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/AmbaIOUTDiag_CmdDDR.o
endif
else
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdDDR.o
endif
ifeq ($(CONFIG_SOC_CV2),)
#   non CV2
else
#   CV2
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/AmbaIOUTDiag_CmdDramShmoo.o
endif
ifeq ($(CONFIG_SOC_CV28),)
#non CV28
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdHDMI.o
endif
endif
ifeq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_H32)),)
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/common/AmbaIOUTDiag_CmdCAN.o
endif
#libssp-objs := AmbaUserSysInit.o AmbaMemTest_Bandwidth_A32.o AmbaMemTest_Bandwidth.o AmbaShell_MemTest.o
#libssp-objs += AmbaShell_FsRandVerify.o
#libssp-objs += AmbaShell_DbgSym.o
ifeq ($(CONFIG_ENABLE_INTERNAL_DIAG_CMDS),y)
TARGET_DIAG_SRC_PATH := ../../../engineering
include $(srctree)/engineering/iout/src/threadx/diag/$(AMBA_CHIP_ID)/Makefile.mk
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb/usbx/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb/usbx/usbx/
ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/reg

ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/inc
ifeq ($(CONFIG_BUILD_COMSVC_DRAMSHMOO),y)
ifneq ($(findstring $(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52), y y),)
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/$(AMBA_CHIP_ID)
endif
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/common
endif
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/apps/iout/inc
ccflags-y += -I$(srctree)/svc/apps/iout/inc/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(CONFIG_BSP_NAME)
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)

ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc

ccflags-$(CONFIG_XEN_SUPPORT) += -I$(srctree)/vendors/foss/xen/include
ccflags-$(CONFIG_BUILD_SSP_PCIE_LIB) += -I$(srctree)/vendors/foss/libnvme

ifeq ($(CONFIG_BUILD_SSP_USB_LIBRARY),y)
include $(srctree)/svc/apps/iout/$(TARGET_APP_SRC_PATH)/diag/common/usb/device_classes/uvc/Makefile.mk
endif
include $(srctree)/svc/apps/iout/$(TARGET_APP_SRC_PATH)/peripherals/$(AMBA_CHIP_ID)/Makefile.mk
