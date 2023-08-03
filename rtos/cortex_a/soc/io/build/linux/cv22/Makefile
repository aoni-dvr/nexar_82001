ifeq ($(CONFIG_BUILD_SSP_DRIVER),y)
SRCPATH := ../../../src/$(AMBA_CHIP_ID)

obj-y := libio_$(AMBA_CHIP_ID).a
install-files += libio_$(AMBA_CHIP_ID).a

#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_ADC.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_DMA.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_DMIC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_FIO.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_GDMA.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_GIC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_GPIO.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_HDMI.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_I2C.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_I2S.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_IRIF.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_NAND.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PLL.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PWC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_RCT.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_RTC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_SD.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_SKPD.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_SPI.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_UART.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_VIN.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_VOUT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_SPINAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_SpiNOR.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_Stepper.o

#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_ADC.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_DMA.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_DMIC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_DRAMC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_FIO.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_GDMA.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_HDMI.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_HDMI_Audio.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_HDMI_Video.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_I2S.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_IRIF.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_NAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_NAND_BBM.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_NAND_OP.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_NAND_Partition.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_ORC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_Config.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_GetClkFreq.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_SetClkFreq.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PWC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_PWM.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_RTC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_SD.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_SD_Ctrl.o
#libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_SPI.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_Stepper.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_TMR.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_VIN.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_VOUT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_WDT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_WDT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_EMMC_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_SPINAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_SpiNOR.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_SpiNOR_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_GPIO.o

libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/AmbaTime.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSYS.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaADC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaCVBS.o
# libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaDiag_Dram.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/linux/AmbaDiag_Int.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaDiag_Vout.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaDiag_Vin.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaDMA.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaDrvEntry.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaDramc.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaEMMC_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaGDMA.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaGPIO.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaHDMI.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaHDMI_CEC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaHDMI_Info.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaI2C.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaI2S.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaIRIF.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaMemProt.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND_BBM.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND_FTL.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND_OP.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNFTL.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNVM_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaPWM.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaRNG.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaRTC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSD.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSD_Card.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSD_DelayCtrl.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSPI.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSPINAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSpiNOR.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaSpiNOR_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaTMR.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaUART.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaVIN.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaVOUT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaVOUT_CSC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaWDT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaCache.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaMMU.o

ifeq ($(CONFIG_PIO_MEM_ALLOC),y)
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaPIO.o
endif

ifeq ($(CONFIG_ARM32),y)
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaMemMap_A32.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaMmioBase_A32.o
else ifeq ($(CONFIG_BLD_SEQ_ATF_LINUX),y)
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND_BBM.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaNAND_OP.o
endif


ifeq ($(CONFIG_ARM64),y)
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaMemMap_A64.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/linux/AmbaMmioBase_A64.o
endif

libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/io-mw/linux/AmbaVer_LibIo.o

ccflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/csl \
        -I$(srctree)/soc/io/src/common/inc/rtsl \
        -I$(srctree)/soc/io/src/common/inc/reg \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/linux_public \
        -I$(srctree)/svc/comsvc/misc/

asflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/csl \
        -I$(srctree)/soc/io/src/common/inc/reg \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

endif # $(CONFIG_BUILD_SSP_DRIVER)
