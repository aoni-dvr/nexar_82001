
# BUB V300
ATT_MAP:=$(subst $\",,$(CONFIG_ATT_MAP))
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaSpiNOR_MT25QL256ABA.o       \
                                $(AMBA_BSP_NAME)/AmbaSPINAND_MT29F2G01ABAGDWB.o  \
                                $(AMBA_BSP_NAME)/AmbaUserMemMap.o                \
                                $(AMBA_BSP_NAME)/AmbaUserDefinedPreBoot.o        \
                                $(AMBA_BSP_NAME)/AmbaUserGpioConfig.o            \
                                $(AMBA_BSP_NAME)/bsp.o \
                                $(AMBA_BSP_NAME)/$(ATT_MAP).o
								
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaNAND_MT29F4G08ABADAH4.o

ifdef CONFIG_CV25_LPDDR4_MT53D512M32D2DS_1596MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1596Mhz.o
endif
ifdef CONFIG_CV25_LPDDR4_MT53D512M32D2DS_1200MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1200Mhz.o
endif
ifdef CONFIG_CV25_LPDDR4_MT53D512M32D2DS_1392MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1392Mhz.o
endif
ifdef CONFIG_CV25_LPDDR4_MT53D512M32D2DS_1800MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1800Mhz.o
endif


ifeq ($(CONFIG_SVC_APPS_ICAM),)
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaUserNandPartition.o
endif
