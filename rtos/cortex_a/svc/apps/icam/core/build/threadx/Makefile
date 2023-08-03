obj-y += libsvc_icamcore.a

CORE_INC_PREFIX = $(src)/../../src

# include path
ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else 
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc                             \
             -I$(srctree)/vendors/ambarella/inc/dsp                         \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)     \
             -I$(srctree)/vendors/ambarella/inc/io                          \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)          \
             -I$(srctree)/vendors/ambarella/inc/perif                       \
             -I$(srctree)/vendors/ambarella/inc/prfile2                     \
             -I$(srctree)/vendors/ambarella/inc/threadx                     \
             -I$(srctree)/vendors/ambarella/inc/usb                         \
             -I$(srctree)/vendors/ambarella/inc/usb/usbx                    \
             -I$(srctree)/vendors/ambarella/wrapper/usb/inc                 \
             -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc          \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc           \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/                              \
             -I$(srctree)/soc/io/src/common/inc                             \
             -I$(srctree)/soc/io/src/common/inc/rtsl                        \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc                    \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl               \
             -I$(srctree)/svc/comsvc/exif/inc                               \
             -I$(srctree)/svc/comsvc/cfs/inc                                \
             -I$(srctree)/svc/comsvc/codec/inc                              \
             -I$(srctree)/svc/comsvc/dcf/inc                                \
             -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)            \
             -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa                 \
             -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam             \
             -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib             \
             -I$(srctree)/svc/comsvc/misc                                   \
             -I$(srctree)/svc/comsvc/print                                  \
             -I$(srctree)/svc/comsvc/shell/inc                              \
             -I$(srctree)/svc/comsvc/draw/inc                               \
             -I$(srctree)/svc/comsvc/usb/inc                                \
             -I$(srctree)/svc/comsvc/eis/inc                                \
             -I$(srctree)/svc/comsvc/imumanager/inc                         \
             -I$(srctree)/svc/comsvc/net/app                                \
             -I$(srctree)/svc/comsvc/net/rtsp/inc                           \
             -I$(srctree)/svc/comsvc/net/inc                                \
             -I$(O)                                            \
             -I$(CORE_INC_PREFIX)/adas/inc                                  \
             -I$(CORE_INC_PREFIX)/cal/inc                                   \
             -I$(CORE_INC_PREFIX)/config/inc                                \
             -I$(CORE_INC_PREFIX)/driver/inc                                \
             -I$(CORE_INC_PREFIX)/dsp/inc                                   \
             -I$(CORE_INC_PREFIX)/draw/inc                                  \
             -I$(CORE_INC_PREFIX)/playback/inc                              \
             -I$(CORE_INC_PREFIX)/record/inc                                \
             -I$(CORE_INC_PREFIX)/sys/inc

ifeq ($(CONFIG_BUILD_AMBA_ADAS),y)
ccflags-y += -I$(srctree)/svc/comsvc/adas/inc
endif


ifdef CONFIG_ENABLE_AMBALINK
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog
endif

ifdef CONFIG_BUILD_FOSS_LVGL
ccflags-y += -I$(srctree)/vendors/foss/lvgl/src/lvgl/src
ccflags-y += -I$(srctree)/vendors/foss/lvgl/src/lvgl
ccflags-y += -I$(srctree)/vendors/foss/lvgl/src/ambarella/inc
ccflags-y += -I$(srctree)/vendors/foss/lvgl/src/ambarella/inc/std

ccflags-y += -I$(srctree)/vendors
ccflags-y += -I$(srctree)
endif

ifdef CONFIG_BUILD_FOSS_MBEDTLS
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/mbedtls-2.16.4/include
ccflags-y += -I$(srctree)/vendors/foss/mbedtls/src/mbedtls-2.16.4/include/mbedtls
ccflags-y += -I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib
ccflags-y += -I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc/
ccflags-y += -I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc/aarch32
endif

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
