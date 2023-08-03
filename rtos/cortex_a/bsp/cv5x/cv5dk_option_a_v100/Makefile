
# BUB V300
ATT_MAP:=$(subst $\",,$(CONFIG_ATT_MAP))
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaNAND_S34ML04G2.o            \
                                $(AMBA_BSP_NAME)/AmbaSpiNOR_MT35XU01GBBA1G.o       \
                                $(AMBA_BSP_NAME)/AmbaSPINAND_W25N02JW.o  \
                                $(AMBA_BSP_NAME)/AmbaUserMemMap.o                \
                                $(AMBA_BSP_NAME)/AmbaUserDefinedPreBoot.o        \
                                $(AMBA_BSP_NAME)/AmbaUserGpioConfig.o            \
                                $(AMBA_BSP_NAME)/bsp.o \
                                $(AMBA_BSP_NAME)/$(ATT_MAP).o

ifdef CONFIG_CV5_LPDDR5_1800MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_MT53D512M16D1DS_1800Mhz.o
endif

ifdef CONFIG_CV5_LPDDR5_1200MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_1200Mhz.o
endif

ifdef CONFIG_CV5_LPDDR5_2016MHZ_085V
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_2016Mhz_085v.o
endif

ifdef CONFIG_CV5_LPDDR5_2016MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_2016Mhz.o
endif

ifdef CONFIG_CV5_LPDDR5_2496MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_2496Mhz.o
endif

ifdef CONFIG_CV5_LPDDR5_2496MHZ_070V
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_2496Mhz_070v.o
endif

ifdef CONFIG_CV5_LPDDR5_2880MHZ_085V
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_2880Mhz_085v.o
endif

ifdef CONFIG_CV5_LPDDR5_2880MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR5_2880Mhz.o
endif

ifeq ($(CONFIG_SVC_APPS_ICAM),)
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaUserNandPartition.o
endif
