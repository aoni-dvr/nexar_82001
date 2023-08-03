ifeq ($(CONFIG_BUILD_SSP_DRIVER),y)
SRCVPATH := ../../../src/common

#ifeq ($(CONFIG_DEVICE_TREE_SUPPORT),y)
#KBUILD_CFLAGS += -Wno-pedantic -Wno-switch-default
#endif

obj-y := libio_common.a
install-files += libio_common.a

ifneq ($(AMBA_FWPROG),y)
#libio_common-objs += $(SRCVPATH)/AmbaCache.o
libio_common-objs += $(SRCVPATH)/AmbaINT.o
#libio_common-objs += $(SRCVPATH)/AmbaTime.o
endif

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_DMA.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_DMA.o
libio_common-objs += $(SRCVPATH)/io-mw/linux/AmbaDMA.o

#libio_common-objs += $(SRCVPATH)/AmbaMMU.o
libio_common-objs += $(SRCVPATH)/AmbaMisraFix.o
libio_common-objs += $(SRCVPATH)/AmbaIOUtility.o
libio_common-objs += $(SRCVPATH)/AmbaIOUtility_Crc32Hw.o

ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52),)
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_USB.o
libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_USB_PHY.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_USB.o
endif

libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_OTP.o

libio_common-objs += $(SRCVPATH)/arm/AmbaSysStack.o
libio_common-objs += $(SRCVPATH)/arm/AmbaRTSL_Cache.o
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

libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_UART.o
libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_I2C.o

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32)),)
    # CV2X and H32
    libio_common-objs += $(SRCVPATH)/rtsl/cv2x/AmbaRTSL_OTP.o
else ifneq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),)
    # CV5X
    libio_common-objs += $(SRCVPATH)/rtsl/cv5x/AmbaRTSL_OTP.o
endif

ifeq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_H32)),)
    libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_CAN.o
    libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_CAN.o
endif

    libio_common-objs += $(SRCVPATH)/csl/AmbaCSL_RNG.o
    libio_common-objs += $(SRCVPATH)/rtsl/AmbaRTSL_RNG.o

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
