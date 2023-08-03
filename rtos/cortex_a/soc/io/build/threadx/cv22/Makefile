ifeq ($(CONFIG_BUILD_SSP_DRIVER),y)
SRCPATH := ../../../src/$(AMBA_CHIP_ID)

obj-y := libio_$(AMBA_CHIP_ID).a
install-files += libio_$(AMBA_CHIP_ID).a

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PLL.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_PWC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_RCT.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_SKPD.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/csl/AmbaCSL_VIN.o

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_ORC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_Config.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_GetClkFreq.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PLL_SetClkFreq.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_PWC.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/rtsl/AmbaRTSL_VIN.o

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSYS.o

ifeq ($(CONFIG_ARM32),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDiag_Dram.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDiag_Int.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaDrvEntry.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMemProt.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaSD_DelayCtrl.o

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMemMap_A32.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMmioBase_A32.o
else ifeq ($(CONFIG_BLD_SEQ_ATF_LINUX),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaNAND.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaNAND_BBM.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaNAND_OP.o
endif

ifeq ($(CONFIG_ARM64),y)
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMemMap_A64.o
libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaMmioBase_A64.o
endif

libio_$(AMBA_CHIP_ID)-objs += $(SRCPATH)/io-mw/threadx/AmbaVer_LibIo.o

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
