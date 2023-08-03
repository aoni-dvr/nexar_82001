obj-y += libeng_perif_${AMBA_CHIP_ID}.a

include $(srctree)/svc/apps/iout/$(TARGET_APP_SRC_PATH)/peripherals/$(AMBA_CHIP_ID)/sensor/Makefile.sensor
include $(srctree)/svc/apps/iout/$(TARGET_APP_SRC_PATH)/peripherals/$(AMBA_CHIP_ID)/bridge/Makefile
include $(srctree)/svc/apps/iout/$(TARGET_APP_SRC_PATH)/peripherals/$(AMBA_CHIP_ID)/yuvinput/Makefile

ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(CONFIG_BSP_NAME)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell
ccflags-y += -I$(srctree)/svc/comsvc/misc

install-files += libeng_perif_${AMBA_CHIP_ID}.a
