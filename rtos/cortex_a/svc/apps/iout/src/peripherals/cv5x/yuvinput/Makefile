ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput/inc
BSPYUVPATH=../../../bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput

ifndef CONFIG_YUVINPUT_LT6911
libeng_perif_${AMBA_CHIP_ID}-objs += $(BSPYUVPATH)/AmbaYuv_LT6911.o \
                                  $(BSPYUVPATH)/AmbaYuv_LT6911Table.o
endif
