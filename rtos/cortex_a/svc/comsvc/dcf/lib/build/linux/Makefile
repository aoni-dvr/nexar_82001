ifneq ($(wildcard $(srctree)/$(src)/dcfcore),)
obj-$(CONFIG_BUILD_COMSVC_DCFCORE) += dcfcore/
endif

obj-y = libcomsvc_dcf.a

COMSVC_DCF_SRC = ../../src

ccflags-y += \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/svc/comsvc/print \
        -I$(srctree)/svc/comsvc/misc \
        -I$(srctree)/svc/comsvc/dcf \
        -I$(srctree)/svc/comsvc/dcf/inc

libcomsvc_dcf-objs += $(COMSVC_DCF_SRC)/AmbaDCF_Camera.o
libcomsvc_dcf-objs += $(COMSVC_DCF_SRC)/AmbaDCF_Dashcam.o
libcomsvc_dcf-objs += $(COMSVC_DCF_SRC)/AmbaDCF.o

install-files += libcomsvc_dcf.a