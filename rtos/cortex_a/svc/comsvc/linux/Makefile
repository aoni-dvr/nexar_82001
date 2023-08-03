# THIS FILE WON'T WORK!!
# If you want to make this file work, please add following line in Makefile.Amba
#   ssp_common_dirs += $(call if_exist,ssp)

OBJ_NAME = libcomsvc

obj-$(CONFIG_BUILD_SVC_COMMON_SERVICE) += $(OBJ_NAME).a
$(OBJ_NAME)-objs :=

ifneq ($(wildcard $(srctree)/$(src)/misc),)
include $(srctree)/$(src)/misc/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/print),)
include $(srctree)/$(src)/print/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/shell),)
include $(srctree)/$(src)/shell/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/imgfrw),)
include $(srctree)/$(src)/imgfrw/Makefile
#obj-y += imgfrw/
endif

ifneq ($(wildcard $(srctree)/$(src)/imgproc/imgaaa),)
include $(srctree)/$(src)/imgproc/imgaaa/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/monfrw),)
include $(srctree)/$(src)/monfrw/Makefile
#obj-y += monfrw/
endif

ifneq ($(wildcard $(srctree)/$(src)/tune/ituner/),)
obj-$(CONFIG_BUILD_COMMON_TUNE_ITUNER) += tune/ituner/
endif

ifneq ($(wildcard $(srctree)/$(src)/tune/ctuner/),)
obj-$(CONFIG_BUILD_COMMON_TUNE_CTUNER) += tune/ctuner/build/threadx/
endif

ifneq ($(wildcard $(srctree)/$(src)/warputility/),)
obj-$(CONFIG_BUILD_COMMON_TUNE_ITUNER) += warputility/
endif

ifneq  ($(wildcard $(srctree)/$(src)/stereoutility/),)
obj-$(CONFIG_BUILD_COMMON_STEREO_UTILITY) += stereoutility/build/threadx/
endif

ifneq ($(wildcard $(srctree)/$(src)/net),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_NET) += net/
obj-$(CONFIG_BUILD_COMMON_SERVICE_NET) += net/app/
endif

ifneq ($(wildcard $(srctree)/$(src)/codec),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_CODEC) += codec/
endif

ifneq ($(wildcard $(srctree)/$(src)/draw),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_DRAW) += draw/
endif

ifneq ($(wildcard $(srctree)/$(src)/cfs),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_CFS) += cfs/
endif

ifneq ($(wildcard $(srctree)/$(src)/dcf),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_DCF) += dcf/
endif

ifneq ($(wildcard $(srctree)/$(src)/exif),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_EXIF) += exif/
endif

ifneq ($(wildcard $(srctree)/$(src)/adas),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_ADAS) += adas/
endif

ifneq ($(wildcard $(srctree)/$(src)/usb),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_USB) += usb/
endif

ifneq ($(wildcard $(srctree)/$(src)/anim),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_ANIM) += anim/
endif

ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ccflags-y += -I$(srctree)/$(src)/print
ccflags-y += -I$(srctree)/$(src)/shell

install-files   := $(OBJ_NAME).a

