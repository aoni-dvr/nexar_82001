################################################################
#                             ThreadX                          #
################################################################
ifeq ($(CONFIG_THREADX), y)

obj-y += libcv_CtfwTask.a

libcv_CtfwTask-objs += ../../src/Ctfw_SinkTask.o
libcv_CtfwTask-objs += ../../src/Ctfw_SourceTask.o

ifeq ($(CONFIG_SOC_CV2), y)
libcv_CtfwTask-objs += ../../src/Ctfw_FexTask.o
libcv_CtfwTask-objs += ../../src/Ctfw_SpuFexTask.o
libcv_CtfwTask-objs += ../../src/Ctfw_SpuFusionTask.o
libcv_CtfwTask-objs += ../../src/Ctfw_FmaTask.o
endif

ifeq ($(CONFIG_SOC_CV2)$(CONFIG_SOC_CV2FS), y)

ifeq ($(CONFIG_CV_FLEXIDAG_STIXEL), y)
libcv_CtfwTask-objs += ../../src/Ctfw_StixelTask.o
endif

endif

ccflags-y := 

ccflags-y += -I$(srctree)/svc/comsvc/cv/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/cv/cv_common/inc/svccvalgo
ccflags-y += -I$(srctree)/svc/comsvc/cv/cv_common/inc/svccvalgo/threadx
ccflags-y += -I$(srctree)/soc/vision/cv_common/inc/
ccflags-y += -I$(srctree)/soc/vision/cv_common/inc/cvsched_common
ccflags-y += -I$(srctree)/soc/vision/cv_common/inc/cavalry
ccflags-y += -I$(srctree)/soc/vision/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/kal/threadx
ccflags-y += -I$(srctree)/vendors/ambarella/inc
# ccflags-y += -I$(srctree)/soc/vision/cv_common/inc/rtos
ccflags-y += -I$(srctree)/soc/vision/arm_framework/scheduler/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/SCAStixel
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp

ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/Ctfw/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtfwTask/inc

ccflags-y += -O3

install-files += libcv_CtfwTask.a

endif