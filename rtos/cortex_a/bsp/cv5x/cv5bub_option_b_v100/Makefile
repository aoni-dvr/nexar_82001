
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

ifdef CONFIG_CV5_LPDDR4_MT53D512M32D2DS_540MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_540Mhz.o
endif

ifeq ($(CONFIG_CV5_LPDDR4_MT53D512M32D2DS_1824MHZ)$(CONFIG_ENABLE_ONE_DDRC_HOST),yy)
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1824Mhz_singlehost.o
else
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1824Mhz.o
endif

ifdef CONFIG_CV5_LPDDR4_MT53D512M32D2DS_2112MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_2112Mhz.o
endif

ifdef CONFIG_CV5_LPDDR4_MT53E2G32D4DE_1800MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53E2G32D4DE_1800Mhz.o
endif

ifeq ($(CONFIG_SVC_APPS_ICAM),)
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaUserNandPartition.o
endif
