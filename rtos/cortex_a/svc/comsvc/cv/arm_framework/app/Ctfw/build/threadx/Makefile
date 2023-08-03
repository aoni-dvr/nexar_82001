################################################################
#                             ThreadX                          #
################################################################
ifeq ($(CONFIG_THREADX), y)

obj-y += libcv_Ctfw.a

libcv_Ctfw-objs += ../../src/Ctfw_TaskTemplate.o
libcv_Ctfw-objs += ../../src/Ctfw_TaskUtil.o

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

ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/Ctfw/inc

ccflags-y += -O3

install-files += libcv_Ctfw.a

endif