ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc
ccflags-y += -I$(srctree)/svc/comsvc/imumanager/inc
ccflags-y += -I$(srctree)/svc/comsvc/eis/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)


IMUPATH=imu
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/AmbaIMU.o 
#libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/AmbaIMU_UnitTest.o 


ifdef CONFIG_IMU_MPU6500
libperif_$(AMBA_CHIP_ID)-objs += $(IMUPATH)/AmbaIMU_MPU6500.o
endif

