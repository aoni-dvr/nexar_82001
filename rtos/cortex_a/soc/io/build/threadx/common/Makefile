ifeq ($(CONFIG_BUILD_SSP_DRIVER),y)
SRCVPATH := ../../../src/common

#ifeq ($(CONFIG_DEVICE_TREE_SUPPORT),y)
#KBUILD_CFLAGS += -Wno-pedantic -Wno-switch-default
#endif

obj-y := libio_common.a
install-files += libio_common.a

ifneq ($(AMBA_FWPROG),y)
libio_common-objs += $(SRCVPATH)/AmbaCache.o
libio_common-objs += $(SRCVPATH)/AmbaINT.o
libio_common-objs += $(SRCVPATH)/AmbaTime.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaSD.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaSD_Card.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaSPINOR.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNAND_OP.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNAND.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNAND_BBM.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaSPINAND.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNAND_FTL.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNFTL.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNVM_Partition.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaNAND_Partition.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaSpiNOR_Partition.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaEMMC_Partition.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_I2C.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_I2C.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaI2C.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_SPI.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_SPI.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaSPI.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_GDMA.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_GDMA.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaGDMA.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaUART.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_I2S.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_I2S.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaI2S.o

ifneq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32)),)
    libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaDMIC.o
    libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_DMIC.o
    libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_DMIC.o
endif

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22)),)
# if select above chips, compile common
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_Stepper.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_Stepper.o
endif

libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_PWM.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaPWM.o

endif

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_DMA.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_DMA.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaDMA.o

libio_common-objs += $(SRCVPATH)/AmbaMMU.o
libio_common-objs += $(SRCVPATH)/AmbaMisraFix.o
libio_common-objs += $(SRCVPATH)/AmbaIOUtility.o
libio_common-objs += $(SRCVPATH)/AmbaIOUtility_Crc32Hw.o

ifeq ($(CONFIG_PIO_MEM_ALLOC),y)
libio_common-objs += ../../../src/common/io-mw/threadx/AmbaPIO.o
endif

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_USB.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_USB_PHY.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_USB.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_OTP.o

libio_common-objs += $(SRCVPATH)/arm/AmbaSysStack.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_Cache.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_GIC.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_GIC.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_MMU.o

ifeq ($(CONFIG_ARM32),y)
libio_common-objs += $(SRCVPATH)/arm/AmbaMisra_A32.o
libio_common-objs += $(SRCVPATH)/arm/AmbaASM_IntrinsicA32.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_CPU_A32.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_MMU_VMSAv8-32.o
endif

ifeq ($(CONFIG_ARM64),y)
libio_common-objs += $(SRCVPATH)/arm/AmbaMisra_A64.o
libio_common-objs += $(SRCVPATH)/arm/AmbaASM_IntrinsicA64.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_CPU_A64.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_MMU_VMSAv8-64.o
endif

ifneq ($(AMBA_FWPROG),y)
ifeq ($(CONFIG_BUILD_SSP_ENET),y)
obj-y += libplat_enet.a
libplat_enet-objs += $(SRCVPATH)/csl/AmbaCSL_ENET.o
libplat_enet-objs += $(SRCVPATH)/rtsl/AmbaRTSL_ENET.o
libplat_enet-objs += $(SRCVPATH)/eth/AmbaENET.o
install-files += libplat_enet.a
endif # $(CONFIG_BUILD_SSP_ENET)
endif

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_RTC.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_RTC.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaRTC.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_UART.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_UART.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_GPIO.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_GPIO.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaGPIO.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_TMR.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_TMR.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaTMR.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaWDT.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_WDT.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_WDT.o

libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_SD_Ctrl.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_SD.o

libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_FIO.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_SpiNOR.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_NAND.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_SPINAND.o

libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_SpiNOR_Partition.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_NAND_Partition.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_EMMC_Partition.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_NAND_BBM.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_NAND_OP.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_SpiNOR.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_SPINAND.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_FIO.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_NAND.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_SD.o

libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaI2C.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_I2C.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_I2C.o

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32),$(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),)
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaDramc.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_DRAMC.o
endif

ifeq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),)
ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32),$(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),)
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_DDRC.o
endif
endif

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32)),)
    # CV2X and H32
    libio_common-objs += $(SRCVPATH)/rtsl/cv2x/AmbaRTSL_OTP.o
else ifneq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),)
    # CV5X
    libio_common-objs += $(SRCVPATH)/rtsl/cv5x/AmbaRTSL_OTP.o
endif

ifeq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_H32)),)
    libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaCAN.o
    libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_CAN.o
    libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_CAN.o
endif

    libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaRNG.o
    libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_RNG.o
    libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_RNG.o

ifneq ($(AMBA_FWPROG),y)
ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52),$(CONFIG_SOC_CV25),$(CONFIG_SOC_H32)),)
    # if select above chips, compile common
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaHDMI.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaHDMI_CEC.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaHDMI_Info.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_HDMI.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_HDMI_Audio.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_HDMI_Video.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_HDMI.o
endif

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32),$(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),)
    # if not cv2fs, compile common
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaCVBS.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaDiag_Vout.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaVOUT.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaVOUT_CSC.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_VOUT.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_VOUT.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_IRIF.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_IRIF.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaIRIF.o

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_ADC.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_ADC.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaADC.o
endif

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32)),)
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaVIN.o
libio_common-objs += $(SRCVPATH)/io-mw/threadx/AmbaDiag_Vin.o
endif

endif

libio_common-objs += $(SRCVPATH)/AmbaVer_LibIoCommon.o

ifeq ($(CONFIG_XEN_SUPPORT),y)
libio_common-objs += $(SRCVPATH)/AmbaDTS.o
endif

CFLAGS_AmbaVer_LibIoCommon.o =                       \
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
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/common/inc/rtsl \
        -I$(srctree)/soc/io/src/common/inc/csl \
        -I$(srctree)/soc/io/src/common/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc

ccflags-$(CONFIG_DEVICE_TREE_SUPPORT) += \
        -I$(srctree)/vendors/foss/libfdt \
        -I$(srctree)/svc/comsvc/misc \
        -I$(srctree)/vendors/foss/libwrapc

ccflags-$(CONFIG_XEN_SUPPORT) += -I$(srctree)/vendors/foss/xen/include

asflags-y := \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

endif # $(CONFIG_BUILD_SSP_DRIVER)
