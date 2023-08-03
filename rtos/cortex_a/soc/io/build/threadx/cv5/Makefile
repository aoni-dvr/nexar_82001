ifeq ($(CONFIG_BUILD_SSP_DRIVER),y)
SRCPATH := ../../../src/$(AMBA_CHIP_ID)

obj-y := libio_$(AMBA_CHIP_ID).a
install-files += libio_$(AMBA_CHIP_ID).a

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_ADC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_DMIC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_GDMA.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_GIC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_GPIO.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_HDMI.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_IRIF.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PLL.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PWC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_RCT.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_RTC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_SD.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_SKPD.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_TMR.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_VIN.o

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_ADC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_DMIC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_GDMA.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_HDMI.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_HDMI_Audio.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_HDMI_Video.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_IRIF.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_ORC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_Config.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_GetClkFreq.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_SetClkFreq.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PWC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PWM.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_RTC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_SD.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_SD_Ctrl.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_TMR.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_VIN.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_VOUT_LCD.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_VOUT_TV.o

ifneq ($(AMBA_FWPROG),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaADC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaCVBS.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDiag_Dram.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDiag_Int.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDiag_Vout.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDiag_Vin.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDMIC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDrvEntry.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaGDMA.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaGPIO.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaHDMI.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaHDMI_CEC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaHDMI_Info.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaIRIF.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMemProt.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaNAND.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaNAND_BBM.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaNAND_OP.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSpiNOR.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaPWM.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaRTC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSD.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSD_Card.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSD_DelayCtrl.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSYS.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaTMR.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaVIN.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaVIN_Table.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaVOUT.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaVOUT_CSC.o
endif


libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_SPINAND.o


ifeq ($(CONFIG_ARM32),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMemMap_A32.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMmioBase_A32.o
endif

ifeq ($(CONFIG_ARM64),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMemMap_A64.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMmioBase_A64.o
endif

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaVer_LibIo.o

ifeq ($(CONFIG_BUILD_SSP_PCIE_LIB),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PCIE.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PCIE.o
obj-y += $(SRCPATH)/io-mw/threadx/pcie/
endif

CFLAGS_AmbaVer_LibIo.o =             \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/common/inc/rtsl \
        -I$(srctree)/soc/io/src/common/inc/csl \
        -I$(srctree)/soc/io/src/common/inc/reg

ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc

asflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

endif # $(CONFIG_BUILD_SSP_DRIVER)
