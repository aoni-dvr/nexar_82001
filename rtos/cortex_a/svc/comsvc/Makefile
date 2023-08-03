# THIS FILE WON'T WORK!!
# If you want to make this file work, please add following line in Makefile.Amba
#   ssp_common_dirs += $(call if_exist,ssp)

# if makefile is in svc/comsvc
preflix =
# if makefile move to svc/comsvc/build/threadx
# preflix = ../../

OBJ_NAME = libcomsvc

obj-$(CONFIG_BUILD_SVC_COMMON_SERVICE) += $(OBJ_NAME).a
$(OBJ_NAME)-objs :=

ccflags-y :=

ifneq ($(wildcard $(srctree)/$(src)/misc),)
include $(srctree)/$(src)/misc/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/print),)
include $(srctree)/$(src)/print/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/shell),)
include $(srctree)/$(src)/shell/Makefile
endif

ifdef CONFIG_BUILD_COMMON_SERVICE_IMUMGR
ifneq ($(wildcard $(srctree)/$(src)/imumanager),)
include $(srctree)/$(src)/imumanager/build/threadx/Makefile
endif
endif

#ifdef CONFIG_BUILD_IMGFRW_EIS
ifneq ($(wildcard $(srctree)/$(src)/eis),)
include $(srctree)/$(src)/eis/build/threadx/Makefile
endif
#endif

#ifdef CONFIG_BUILD_COMSVC_IMGFRW
ifneq ($(wildcard $(srctree)/$(src)/$(preflix)imgfrw),)
include $(srctree)/$(src)/$(preflix)imgfrw/build/threadx/Makefile
#obj-y += $(preflix)imgfrw/build/threadx/
endif
#endif

ifdef CONFIG_BUILD_COMSVC_IMGAAA
ifneq ($(wildcard $(srctree)/$(src)/imgproc/imgaaa),)
include $(srctree)/$(src)/imgproc/imgaaa/Makefile
endif
endif

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32)),)
ifdef CONFIG_BUILD_COMSVC_IQPARAM
ifneq ($(wildcard $(srctree)/$(src)/iqparams/cv2x),)
include $(srctree)/$(src)/iqparams/cv2x/Makefile
endif
endif
else
ifdef CONFIG_BUILD_COMSVC_IQPARAM
ifneq ($(wildcard $(srctree)/$(src)/iqparams/$(AMBA_CHIP_ID)),)
include $(srctree)/$(src)/iqparams/$(AMBA_CHIP_ID)/Makefile
endif
endif
endif


ifneq ($(wildcard $(srctree)/$(src)/af),)
include $(srctree)/$(src)/af/build/threadx/Makefile
endif

ifneq ($(wildcard $(srctree)/$(src)/$(preflix)monfrw),)
include $(srctree)/$(src)/$(preflix)monfrw/build/threadx/Makefile
#obj-y += $(preflix)monfrw/build/threadx/
endif

ifneq ($(wildcard $(srctree)/$(src)/tune/ituner/build/threadx/),)
obj-$(CONFIG_BUILD_COMMON_TUNE_ITUNER) += tune/ituner/build/threadx/
endif

ifneq ($(wildcard $(srctree)/$(src)/tune/ctuner/),)
obj-$(CONFIG_BUILD_COMMON_TUNE_CTUNER) += tune/ctuner/build/threadx/
endif

ifneq ($(wildcard $(srctree)/$(src)/warputility/build/threadx/),)
obj-$(CONFIG_BUILD_COMMON_WARP_UTILITY) += warputility/build/threadx/
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
obj-$(CONFIG_BUILD_COMMON_SERVICE_DCF) += dcf/lib/build/threadx/
endif

ifneq ($(wildcard $(srctree)/$(src)/exif),)
obj-$(CONFIG_BUILD_COMMON_SERVICE_EXIF) += exif/lib/build/threadx/
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

ifdef CONFIG_BUILD_COMSVC_DRAMSHMOO
ifneq ($(wildcard $(srctree)/$(src)/dramshmoo),)
obj-$(CONFIG_BUILD_COMSVC_DRAMSHMOO) += dramshmoo/lib/build/threadx/
endif
endif

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/lens/inc
else 
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ccflags-$ += -I$(srctree)/$(src)/eis/inc
ccflags-$(CONFIG_BUILD_COMMON_SERVICE_IMUMGR)+= -I$(srctree)/$(src)/imumanager/inc
ccflags-$(CONFIG_ICAM_IMGPROC_USED) += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)/
ccflags-$(CONFIG_BUILD_COMMON_SERVICE_IMUMGR) += -I$(srctree)/svc/comsvc/imumanager/inc
#$(error $(ccflags-y) ....)

install-files   := $(OBJ_NAME).a

