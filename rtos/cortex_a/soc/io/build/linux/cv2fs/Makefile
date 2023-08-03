ifeq ($(CONFIG_BUILD_SSP_DRIVER),y)

obj-y := libio_$(AMBA_CHIP_ID).a
install-files += libio_$(AMBA_CHIP_ID).a

libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaI2C.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaDMA.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/csl/AmbaCSL_WDT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/rtsl/AmbaRTSL_WDT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/common/AmbaTime.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaRTC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaRNG.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaCAN.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaCANPeer.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaCEHU.o
#libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaDiag_Dram.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaDiag_Int.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaDiag_Vin.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaDiag_Vout.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaDrvEntry.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaEMMC_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaGDMA.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaGPIO.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaI2S.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaMemProt.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNAND_BBM.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNAND_FTL.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNAND_OP.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNAND_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNFTL.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaNVM_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaPWM.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSD.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSD_Card.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSD_DelayCtrl.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSPI.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSPINAND.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSpiNOR.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSpiNOR_Partition.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaSYS.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaTempSensor.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaTMR.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaUART.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaVIN.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaVOUT.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaVOUT_CSC.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaWDT.o

ifeq ($(CONFIG_ARM32),y)
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaCortexA53Fusa.o
endif

ifeq ($(CONFIG_ARM64),y)
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaMemMap_A64.o
libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaMmioBase_A64.o
endif

libio_$(AMBA_CHIP_ID)-objs += ../../../src/$(AMBA_CHIP_ID)/io-mw/linux/AmbaVer_LibIo.o

ccflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc

asflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport

endif # $(CONFIG_BUILD_SSP_DRIVER)
