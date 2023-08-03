ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/b8/inc

SENSORPATH=sensor
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor.o 
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_UnitTest.o

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX290_MIPI.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX290_MIPITable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX577.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX577Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_AR0239_RGBIR.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_AR0239_RGBIRTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX390.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX390Table.o
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
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2778_RGBIR.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2778_RGBIRTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_AR0233.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_AR0233Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX224.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX224Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_VG1762_RGBIR.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_VG1762_RGBIRTable.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX622.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX622Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX623.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX623Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0323.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0323Table.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OX03C.o
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OX03CTable.o
else

ifdef CONFIG_SENSOR_SONY_IMX290_MIPI
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX290_MIPI.o \
                                 $(SENSORPATH)/AmbaSensor_IMX290_MIPITable.o
endif

ifdef CONFIG_SENSOR_SONY_IMX577
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_IMX577.o \
                                 $(SENSORPATH)/AmbaSensor_IMX577Table.o
endif

ifdef CONFIG_SENSOR_MX00_IMX390
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390Table.o
endif

ifdef CONFIG_SENSOR_MX01_IMX390
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX390.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX390Table.o
endif

ifdef CONFIG_SENSOR_MX01_IMX224
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX224.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_96712_IMX224Table.o
endif

ifdef CONFIG_SENSOR_MX03_AR0144
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX96707_9286_AR0144.o \
                                 $(SENSORPATH)/AmbaSensor_MAX96707_9286_AR0144Table.o
endif

ifdef CONFIG_SENSOR_MX00_AR0239
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIR.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0239_RGBIRTable.o
endif

ifdef CONFIG_SENSOR_MX00_AR0233
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0233.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0233Table.o
endif

ifdef CONFIG_SENSOR_MX00_IMX390_RCCB
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390_RCCB.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX390_RCCBTable.o
endif

ifdef CONFIG_SENSOR_MX00_IMX424
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX424_RCCB.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX424_RCCBTable.o
endif

ifdef CONFIG_SENSOR_MX00_OV2312_RGBIR
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2312_RGBIR.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2312_RGBIRTable.o
endif

ifdef CONFIG_SENSOR_B8N_IMX290
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_B8N_IMX290.o \
                                 $(SENSORPATH)/AmbaSensor_B8N_IMX290Table.o
endif

ifdef CONFIG_SENSOR_MX00_OV2778_RGBIR
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2778_RGBIR.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_OV2778_RGBIRTable.o
endif

ifdef CONFIG_SENSOR_MX00_IMX490
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX490Table.o
endif

ifdef CONFIG_SENSOR_MX00_VG1762_RGBIR
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_VG1762_RGBIR.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_VG1762_RGBIRTable.o
endif

ifdef CONFIG_SENSOR_MX00_IMX622
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX622.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX622Table.o
endif

ifdef CONFIG_SENSOR_MX00_IMX623
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX623.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_IMX623Table.o
endif

ifdef CONFIG_SENSOR_MX00_AR0323
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0323.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_AR0323Table.o
endif

ifdef CONFIG_SENSOR_MX00_OX03C
libperif_$(AMBA_CHIP_ID)-objs += $(SENSORPATH)/AmbaSensor_MAX9295_9296_OX03C.o \
                                 $(SENSORPATH)/AmbaSensor_MAX9295_9296_OX03CTable.o
endif

endif
