
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc

IMUPATH=imu
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/imu.o

ifdef CONFIG_ENABLE_IMU
ifdef CONFIG_IMU_ICM20948
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/imu_icm20948.o
endif
ifdef CONFIG_IMU_BMI160
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/imu_bmi160.o
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/bmi160.o
endif
ifdef CONFIG_IMU_SH3001
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/imu_sh3001.o
endif
ifdef CONFIG_IMU_ICM42670
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/imu_icm42670.o
endif
ifdef CONFIG_IMU_ICM42631
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/imu_icm42631.o
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/Icm426xxDriver_HL.o
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/Icm426xxTransport.o
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/Icm426xxDriver_HL_apex.o
endif
endif
