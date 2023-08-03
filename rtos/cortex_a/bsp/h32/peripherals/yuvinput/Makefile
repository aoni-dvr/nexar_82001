ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput/inc

YUVINPUTPATH=yuvinput
libperif_$(AMBA_CHIP_ID)-objs += $(YUVINPUTPATH)/AmbaYuv.o 

ifeq ($(CONFIG_SVC_APPS_ICAM),y)

libperif_$(AMBA_CHIP_ID)-objs += $(YUVINPUTPATH)/AmbaYuv_LT6911.o
libperif_$(AMBA_CHIP_ID)-objs += $(YUVINPUTPATH)/AmbaYuv_LT6911Table.o

else

ifdef CONFIG_YUVINPUT_LT6911
libperif_$(AMBA_CHIP_ID)-objs += $(YUVINPUTPATH)/AmbaYuv_LT6911.o \
                      $(YUVINPUTPATH)/AmbaYuv_LT6911Table.o
endif

endif