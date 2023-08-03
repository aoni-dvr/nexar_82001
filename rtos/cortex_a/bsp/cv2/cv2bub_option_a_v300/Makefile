
# BUB V300
ATT_MAP:=$(subst $\",,$(CONFIG_ATT_MAP))
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaNAND_S34ML04G2.o            \
                                $(AMBA_BSP_NAME)/AmbaSpiNOR_MT25QL256ABA.o       \
                                $(AMBA_BSP_NAME)/AmbaSPINAND_MT29F2G01ABAGDWB.o  \
                                $(AMBA_BSP_NAME)/AmbaUserMemMap.o                \
                                $(AMBA_BSP_NAME)/AmbaUserDefinedPreBoot.o        \
                                $(AMBA_BSP_NAME)/AmbaUserGpioConfig.o            \
                                $(AMBA_BSP_NAME)/bsp.o \
                                $(AMBA_BSP_NAME)/$(ATT_MAP).o

ifdef CONFIG_CV2_LPDDR4_MT53D512M32D2DS_1596MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1596Mhz.o
else
ifdef CONFIG_CV2_LPDDR4_MT53D512M32D2DS_1800MHZ
#Maximal stable freq is 1596MHz
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1800Mhz.o
else
ifdef CONFIG_CV2_LPDDR4_MT53D512M32D2DS_1608MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1608Mhz.o
else
ifdef CONFIG_CV2_LPDDR4_MT53D512M32D2DS_1680MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1680Mhz.o
else
ifdef CONFIG_CV2_LPDDR4_MT53D512M32D2DS_1560MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1560Mhz.o
else
#All others apply Auto-Grade setting
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M32D2DS_1392MHz_CPU_756MHz_normal.o
endif
endif
endif
endif
endif

ifeq ($(CONFIG_SVC_APPS_ICAM),)
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaUserNandPartition.o
endif
