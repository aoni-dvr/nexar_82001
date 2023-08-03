ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

$(OBJ_NAME)-objs += imumanager/AmbaImg_ImuManager.o
#obj-y := libimg_imumgr.a
#libimg_imumgr-objs += AmbaImg_ImuManager.o

ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)/
ccflags-y += -I$(srctree)/svc/comsvc/imumanager/inc

#
#
#install-files := libimg_imumgr.a
