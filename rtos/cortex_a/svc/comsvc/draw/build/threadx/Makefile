
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libcomsvc_draw.a

libcomsvc_draw-objs := \
	../../AmbaDraw_Common.o

libcomsvc_draw-objs += \
	../../AmbaDraw.o	\
	../../AmbaDraw_Main.o	\
	../../AmbaDraw_Render.o	\
	../../bmp/AmbaDraw_BMP.o	\
	../../shape/AmbaDraw_Shape.o	\
	../../shape/AmbaDraw_CubicSpline.o	\
	../../string/AmbaDraw_String.o	\
	../../string/AmbaDraw_BMPFont.o

# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libcomsvc_draw-objs))

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y += -Ivendors/ambarella/inc	\
			-I$(srctree)/$(src)/../../inc	\
			-I$(srctree)/$(src)/../..
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)

install-files := libcomsvc_draw.a


