
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/audio/inc

AUDIOPATH=audio
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_CODEC.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_CODEC.o
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_AK4951EN.o
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_TLV320AIC3007.o
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_NAU8822.o
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_MAX98090.o
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_CL1009.o

else

libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaCodec_UnitTest.o

ifdef CONFIG_AUDIO_AK4951EN
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_AK4951EN.o
endif

ifdef CONFIG_AUDIO_TITLV320AIC3007
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_TLV320AIC3007.o
endif

ifdef CONFIG_AUDIO_NAU8822
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_NAU8822.o
endif

ifdef CONFIG_AUDIO_MAX98090
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_MAX98090.o
endif

ifdef CONFIG_AUDIO_CL1009
libperif_$(AMBA_CHIP_ID)-objs += $(AUDIOPATH)/AmbaAudio_CL1009.o
endif

endif