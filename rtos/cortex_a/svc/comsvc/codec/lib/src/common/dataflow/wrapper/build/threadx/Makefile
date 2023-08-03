
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libcomsvc_wrapper.a

libcomsvc_wrapper-objs := \
	../../stream/BufferStream.o

libcomsvc_wrapper-objs += \
	../../WrpFormatLib.o

ifdef CONFIG_BUILD_COMSVC_MX_FMP4
libcomsvc_wrapper-objs += \
	../../SvcMp4Fmt.o
endif
ifdef CONFIG_BUILD_COMSVC_MX_EXIF
libcomsvc_wrapper-objs += \
	../../AmbaExifFormatter.o
endif

# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libcomsvc_wrapper-objs))

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -Ivendors/ambarella/inc	\
			-Ivendors/ambarella/inc/io	\
			-Ivendors/ambarella/inc/dsp	\
			-Ivendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)	\
			-Isvc/comsvc/misc	\
			-Isvc/comsvc/print	\
			-Isvc/comsvc/cfs/inc	\
			-Isvc/comsvc/codec/inc	\
			-Isvc/comsvc/imgproc/imgaaa/inc/aaa	\
			-I$(srctree)/$(src)/../../../fifo/inc	\
			-I$(srctree)/$(src)/../../../container/inc	\
			-I$(srctree)/$(src)/../..

install-files := libcomsvc_wrapper.a


