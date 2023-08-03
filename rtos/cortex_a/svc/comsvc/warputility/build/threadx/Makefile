obj-y += libimg_warputility.a
libimg_warputility-objs += ../../src/AmbaWU_WarpUtility.o
#libimg_warputility-objs += ../../src/AmbaWU_SystemApi.o
libimg_warputility-objs += ../../src/AmbaWU_WarpExecuteUtility.o

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/svc/comsvc/warputility
ccflags-y += -I$(srctree)/svc/comsvc/warputility/inc
ccflags-y += -I$(srctree)/svc/comsvc/print

install-files := libimg_warputility.a
