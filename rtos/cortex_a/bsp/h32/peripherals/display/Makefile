
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/display/inc

DISPLAYPATH=display
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_WDF9648W.o
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_T30P61.o
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_MAXIM_Dual_TFT1280768.o
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_MAXIM_Dual_ZS095BH.o

else

ifdef CONFIG_FPD_WDF9648W
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_WDF9648W.o
endif

ifdef CONFIG_FPD_T30P61
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_T30P61.o
endif

ifdef CONFIG_FPD_MAXIM_DUAL_TFT1280768
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_MAXIM_Dual_TFT1280768.o
endif

ifdef CONFIG_FPD_MAXIM_DUAL_ZS095BH
libperif_$(AMBA_CHIP_ID)-objs += $(DISPLAYPATH)/AmbaFPD_MAXIM_Dual_ZS095BH.o
endif

endif