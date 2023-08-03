obj-y += libsvc_icamcore.a

CORE_INC_PREFIX = $(src)/../../src

# include path
ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/linux
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb/usbx
ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/usb/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/svc/comsvc/exif/inc
ccflags-y += -I$(srctree)/svc/comsvc/cfs/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/inc
ccflags-y += -I$(srctree)/svc/comsvc/dcf/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/draw/inc
ccflags-y += -I$(srctree)/svc/comsvc/usb/inc
ccflags-y += -I$(srctree)/svc/comsvc/eis/inc
ccflags-y += -I$(srctree)/svc/comsvc/imumanager/inc
ccflags-y += -I$(srctree)/svc/comsvc/net/app
ccflags-y += -I$(srctree)/svc/comsvc/net/rtsp/inc
ccflags-y += -I$(srctree)/svc/comsvc/net/inc
ccflags-y += -I$(O)
ccflags-y += -I$(CORE_INC_PREFIX)/adas/inc
ccflags-y += -I$(CORE_INC_PREFIX)/cal/inc
ccflags-y += -I$(CORE_INC_PREFIX)/config/inc
ccflags-y += -I$(CORE_INC_PREFIX)/driver/inc
ccflags-y += -I$(CORE_INC_PREFIX)/dsp/inc
ccflags-y += -I$(CORE_INC_PREFIX)/draw/inc
ccflags-y += -I$(CORE_INC_PREFIX)/playback/inc
ccflags-y += -I$(CORE_INC_PREFIX)/record/inc
ccflags-y += -I$(CORE_INC_PREFIX)/sys/inc

ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/inc
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog
ccflags-$(CONFIG_BUILD_FOSS_LVGL) += -I$(srctree)/vendors/foss/lvgl/src/lvgl/src
ccflags-$(CONFIG_BUILD_FOSS_LVGL) += -I$(srctree)/vendors/foss/lvgl/src/lvgl
ccflags-$(CONFIG_BUILD_FOSS_LVGL) += -I$(srctree)/vendors
ccflags-$(CONFIG_BUILD_FOSS_LVGL) += -I$(srctree)
ccflags-$(CONFIG_BUILD_COMSVC_DRAMSHMOO) += -I$(srctree)/svc/comsvc/dramshmoo/inc

include $(src)/Makefile.adas
include $(src)/Makefile.cal
include $(src)/Makefile.config
ifeq ($(CONFIG_BUILD_CV), y)
include $(src)/Makefile.cv
endif
include $(src)/Makefile.draw
include $(src)/Makefile.driver
include $(src)/Makefile.dsp
include $(src)/Makefile.img
include $(src)/Makefile.playback
include $(src)/Makefile.record
include $(src)/Makefile.safe
include $(src)/Makefile.sys

install-files := libsvc_icamcore.a
