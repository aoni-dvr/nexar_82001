ifneq ($(wildcard $(srctree)/$(src)/dcfcore),)
obj-$(CONFIG_BUILD_COMSVC_DCFCORE) += dcfcore/
endif

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

obj-y = libcomsvc_dcf.a

COMSVC_DCF_SRC = ../../src

ccflags-y += \
        -I$(srctree)/svc/comsvc/dcf \
        -I$(srctree)/svc/comsvc/dcf/inc
		
ccflags-y += -I$(srctree)/svc/apps/icam/cardv
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)

#libcomsvc_dcf-objs += $(COMSVC_DCF_SRC)/AmbaDCF_Camera.o
libcomsvc_dcf-objs += $(COMSVC_DCF_SRC)/AmbaDCF_Dashcam.o
libcomsvc_dcf-objs += $(COMSVC_DCF_SRC)/AmbaDCF.o

install-files += libcomsvc_dcf.a