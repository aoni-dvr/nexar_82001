
ifdef CONFIG_H32_LPDDR4_MT53D512M16D1DS_1596MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M16D1DS_1596Mhz.o
endif
ifdef CONFIG_H32_LPDDR4_MT53D512M16D1DS_1200MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M16D1DS_1200Mhz.o
endif
ifdef CONFIG_H32_LPDDR4_MT53D512M16D1DS_840MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_MT53D512M16D1DS_840Mhz.o
endif
ifdef CONFIG_H32_LPDDR4_IS43LQ16256A062BLI_1596MHZ
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaLPDDR4_IS43LQ16256A062BLI_1596Mhz.o
endif

# BUB V200
ATT_MAP:=$(subst $\",,$(CONFIG_ATT_MAP))
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaUserMemMap.o                \
                                $(AMBA_BSP_NAME)/AmbaSpiNOR_MT25QL512ABA.o       \
                                $(AMBA_BSP_NAME)/AmbaSPINAND_MT29F2G01ABAGDWB.o  \
                                $(AMBA_BSP_NAME)/AmbaUserDefinedPreBoot.o        \
                                $(AMBA_BSP_NAME)/AmbaUserGpioConfig.o            \
                                $(AMBA_BSP_NAME)/bsp.o                           \
                                $(AMBA_BSP_NAME)/$(ATT_MAP).o

ifeq ($(CONFIG_SVC_APPS_ICAM),)
libbsp_$(AMBA_BSP_NAME)-objs += $(AMBA_BSP_NAME)/AmbaUserNandPartition.o
endif
