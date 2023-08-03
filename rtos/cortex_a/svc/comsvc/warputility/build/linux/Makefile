obj-y += libimg_warputility.a
libimg_warputility-objs += ../../src/AmbaWU_WarpUtility.o
libimg_warputility-objs += ../../src/AmbaWU_SystemApi.o
libimg_warputility-objs += ../../src/AmbaWU_WarpExecuteUtility.o

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/warputility

ccflags-y += -I$(srctree)/svc/comsvc/warputility/inc
ccflags-y += -I$(srctree)/svc/comsvc/print

install-files := libimg_warputility.a
