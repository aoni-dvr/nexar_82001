ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

obj-y += libdsp_audio.a

ccflags-y := \
             -I$(srctree)/vendors/ambarella/inc \
             -I$(srctree)/vendors/ambarella/inc/io \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
             -I$(srctree)/vendors/ambarella/inc/dsp \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
             -I$(srctree)/soc/io/src/common/inc/csl \
             -I$(srctree)/soc/io/src/common/inc/rtsl \
             -I$(srctree)/soc/io/src/common/inc/reg \
             -I$(srctree)/svc/comsvc/print \
             -I$(srctree)/svc/comsvc/misc


libdsp_audio-objs += \
                      AmbaAudio_AENC.o \
                      AmbaAudio_ADEC.o \
                      $(AMBA_CHIP_ID_DSP)/AmbaAudio_AIN.o \
                      $(AMBA_CHIP_ID_DSP)/AmbaAudio_AOUT.o

libdsp_audio-objs += AmbaVerSetDspAudio.o
$(obj)/AmbaVerSetDspAudio.o : $(call amba_ver_dep, AmbaVerSetDspAudio.o, $(libdsp_audio-objs))
CFLAGS_AmbaVerSetDspAudio.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)            \
                              -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)    \
                              -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)      \
                              -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)      \
                              -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)                \
                              -DAMBA_CI_ID=$(AMBA_CI_ID)

install-files := libdsp_audio.a