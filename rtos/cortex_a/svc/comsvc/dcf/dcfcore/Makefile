obj-y += libcomsvc_dcfcore.a

ccflags-y += \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/svc/comsvc/misc \
        -I$(srctree)/svc/comsvc/print \
        -I$(srctree)/svc/comsvc/dcf/dcfcore \
        -I$(srctree)/svc/comsvc/dcf/dcfcore/inc

libcomsvc_dcfcore-objs := RefDcf.o
libcomsvc_dcfcore-objs += RefDcfImpl.o

install-files := libcomsvc_dcfcore.a
