# svc sys function

obj-y = libcomsvc_exif.a

COMSVC_EXIF_SRC = ../../src

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/cfs/inc
ccflags-y += -I$(srctree)/svc/comsvc/cfs/src
ccflags-y += -I$(srctree)/svc/comsvc/exif
ccflags-y += -I$(srctree)/svc/comsvc/codec/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/exif/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

libcomsvc_exif-objs += $(COMSVC_EXIF_SRC)/AmbaExifParser.o
libcomsvc_exif-objs += $(COMSVC_EXIF_SRC)/AmbaExifDemuxer.o
libcomsvc_exif-objs += $(COMSVC_EXIF_SRC)/AmbaExifFile.o
libcomsvc_exif-objs += $(COMSVC_EXIF_SRC)/AmbaExifMem.o


install-files += libcomsvc_exif.a