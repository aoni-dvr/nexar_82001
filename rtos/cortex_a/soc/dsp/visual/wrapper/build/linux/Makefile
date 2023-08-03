obj-y += libdsp_wrapper.a

DSP_WRAP_OS_NAME := linux

ifdef CONFIG_SOC_CV2FS
DSP_WRAP_FAMILY_PATH:=fs
else
DSP_WRAP_FAMILY_PATH:=auto
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/$(DSP_WRAP_OS_NAME)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/soc/dsp/visual/wrapper/inc/$(DSP_WRAP_OS_NAME)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/common
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/LL/$(DSP_WRAP_FAMILY_PATH)
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
			 
libdsp_wrapper-objs := ../../src/$(DSP_WRAP_OS_NAME)/dsp_api.o
libdsp_wrapper-objs += ../../src/$(DSP_WRAP_OS_NAME)/dsp_wrapper_osal.o

install-files := libdsp_wrapper.a


