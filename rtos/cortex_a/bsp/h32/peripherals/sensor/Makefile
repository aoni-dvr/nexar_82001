ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/inc

SENSORPATH=sensor
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor.o 
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_UnitTest.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX290_MIPI.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX290_MIPITable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX577.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX577Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIR.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIRTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX390.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX390Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX96707_9286_AR0144.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX96707_9286_AR0144Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIR.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIRTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390_RCCB.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390_RCCBTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX424_RCCB.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX424_RCCBTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0233.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0233Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2312_RGBIR.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2312_RGBIRTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV9284_CCCC.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV9284_CCCCTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_K351P.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_K351PTable.o

else

ifdef CONFIG_SENSOR_SONY_IMX290_MIPI
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX290_MIPI.o \
                                 $(SENSORPATH)/AmbaSensor_IMX290_MIPITable.o
endif

ifdef CONFIG_SENSOR_SONY_IMX577
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX577.o \
                                 $(SENSORPATH)/AmbaSensor_IMX577Table.o
endif

ifdef CONFIG_SENSOR_SONY_IMX390
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX390.o \
                                 $(SENSORPATH)/AmbaSensor_IMX390Table.o
endif

ifdef CONFIG_SENSOR_MX00_IMX390
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390Table.o
endif

ifdef CONFIG_SENSOR_MX03_AR0144
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX96707_9286_AR0144.o \
                                 $(SENSORPATH)/AmbaSensor_MAX96707_9286_AR0144Table.o
endif


ifdef CONFIG_SENSOR_MX00_AR0239
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIR.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIRTable.o
endif

ifdef CONFIG_SENSOR_MX00_IMX390_RCCB
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390_RCCB.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390_RCCBTable.o
endif

ifdef CONFIG_SENSOR_MX00_IMX424
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX424_RCCB.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX424_RCCBTable.o
endif

ifdef CONFIG_SENSOR_MX00_AR0233
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0233.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0233Table.o
endif

ifdef CONFIG_SENSOR_MX00_OV2312_RGBIR
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2312_RGBIR.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2312_RGBIRTable.o
endif

ifdef CONFIG_SENSOR_MX00_IMX490
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490Table.o
endif

ifdef CONFIG_SENSOR_MX00_OV9284_CCCC
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV9284_CCCC.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV9284_CCCCTable.o
endif

ifdef CONFIG_SENSOR_JX_K351P
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_K351P.o \
                                 $(SENSORPATH)/AmbaSensor_K351PTable.o
endif
endif