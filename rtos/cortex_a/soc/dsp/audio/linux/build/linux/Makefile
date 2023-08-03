obj-y += libdsp_audio.a

DSP_AUDIO_OS_NAME := linux

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/$(DSP_WRAP_OS_NAME)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/dsp/audio/linux
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/common/linux
ccflags-y += -I$(srctree)/soc/osal
ccflags-y += -I$(srctree)/soc/dsp/visual/wrapper/inc/linux

libdsp_audio-objs := ../../audio_api.o
libdsp_audio-objs += ../../AmbaAudio_AENC.o
libdsp_audio-objs += ../../AmbaAudio_ADEC.o
libdsp_audio-objs += ../../$(AMBA_CHIP_ID_DSP)/AmbaAudio_AIN.o
libdsp_audio-objs += ../../$(AMBA_CHIP_ID_DSP)/AmbaAudio_AOUT.o

install-files := libdsp_audio.a
