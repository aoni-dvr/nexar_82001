obj-y += libb8_$(AMBA_CHIP_ID).a

libb8_$(AMBA_CHIP_ID)-objs += AmbaB8.o \
	AmbaB8_Calibration.o\
	AmbaB8_Communicate.o \
	AmbaB8_GPIO.o \
	AmbaB8_I2C.o \
	AmbaB8_PLL.o \
	AmbaB8_Serdes.o \
	AmbaB8_SPI.o \
	AmbaB8_UnitTest.o \
	AmbaB8_Wrapper.o \
	AmbaB8_Codec.o \
	AmbaB8_Compressor.o \
	AmbaB8_Decompressor.o \
	AmbaB8_Depacker.o \
	AmbaB8_Merger.o \
	AmbaB8_Packer.o \
	AmbaB8_Prescaler.o \
	AmbaB8_PWM.o \
	AmbaB8_PwmDec.o\
	AmbaB8_PwmEnc.o\
	AmbaB8_Scratchpad.o\
	AmbaB8_PHY.o \
	AmbaB8_VIN.o \
	AmbaB8_VOUT.o \
	AmbaB8D_Serdes.o \
	AmbaB8D_Calibration.o\
	CSL/src/AmbaB8CSL_PLL.o \
	CSL/src/AmbaB8CSL_PHY.o \

libb8_$(AMBA_CHIP_ID)-objs += AmbaB8_MmioBase.o

#ccflags-y += -I$(src)/CSL/inc/
#ccflags-y += -I$(src)/CSL/inc/Reg/
#ccflags-y += -I$(srctree)/ssp/soc/CSL/inc/
#ccflags-y += -I$(srctree)/ssp/soc/CSL/inc/Reg/
#ccflags-y += -I$(srctree)/ssp/soc/CSL/inc/Reg/DebugPort/
#ccflags-y += -I$(srctree)/ssp/soc/MW/inc/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/dsp/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/soc/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/fs/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/ssp/prfile2/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaShell/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys/
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaTest/
#ccflags-y += -I$(srctree)/comsvc/printf
#ccflags-y += -I$(srctree)/peripheral/bridge/b8/inc/
#ccflags-y += -I$(srctree)/peripheral/bridge/b8/CSL/inc
#ccflags-y += -I$(srctree)/peripheral/bridge/b8/CSL/inc/Reg

ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/b8/inc/
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/b8/CSL/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/b8/CSL/inc/Reg
#ccflags-y += -I$(srctree)/peripheral/bridge/b8/inc/
#ccflags-y += -I$(srctree)/peripheral/bridge/b8/CSL/inc
#ccflags-y += -I$(srctree)/peripheral/bridge/b8/CSL/inc/Reg


ccflags-y += -std=gnu99

#asflags-y += -I$(srctree)/peripheral/bridge/b8/CSL/inc/Reg
asflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/b8/CSL/inc/Reg

install-files := libb8_$(AMBA_CHIP_ID).a
