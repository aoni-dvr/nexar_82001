BRIDGEPATH=bridge

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max9295_9296.o
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max9295_96712.o
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max96707_9286.o
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max96717_96716.o
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max96717_96712.o

else

ifdef CONFIG_SENSOR_BRIDGE_MAX9295_9296
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max9295_9296.o
endif

ifdef CONFIG_SENSOR_BRIDGE_MAX9295_96712
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max9295_96712.o
endif

ifdef CONFIG_SENSOR_BRIDGE_MAX96707_9286
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max96707_9286.o
endif

ifdef CONFIG_AMBA_B8
obj-$(CONFIG_AMBA_B8) += $(BRIDGEPATH)/b8/
endif

ifdef CONFIG_SENSOR_BRIDGE_MAX96717_96716
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max96717_96716.o
endif

ifdef CONFIG_SENSOR_BRIDGE_MAX96717_96712
libperif_$(AMBA_CHIP_ID)-objs += $(BRIDGEPATH)/AmbaSbrg_Max96717_96712.o
endif

endif