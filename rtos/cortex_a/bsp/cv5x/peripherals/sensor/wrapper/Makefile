ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/wrapper/inc

SENSORPATH=sensor
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

else
ifdef CONFIG_SENSOR_MX01_WRAPPER
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/wrapper/AmbaSensor_MAX9295_96712_Wrapper.o \
                                 $(SENSORPATH)/wrapper/AmbaSensor_MAX9295_96712_IMX424_RCCB_VC_.o \
                                 $(SENSORPATH)/wrapper/AmbaSensor_MAX9295_96712_IMX390_VC_.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX424_RCCBTable.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX390Table.o
endif
endif
