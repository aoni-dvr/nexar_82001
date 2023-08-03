obj-y           += libfwprog.a
install-files   := libfwprog.a

libfwprog-objs  := AmbaFlashFwProg_MmuMemMap.o
libfwprog-objs  += AmbaFlashFwProg_Boot_A64.o
libfwprog-objs  += AmbaFlashFwProg_HwStack_A64.o
libfwprog-objs  += AmbaVer.o

ifeq ($(CONFIG_ENABLE_EMMC_BOOT), y)
libfwprog-objs  += AmbaFlashFwProg_eMMC.o
else ifeq ($(CONFIG_ENABLE_SPINOR_BOOT), y)
libfwprog-objs  += AmbaFlashFwProg_SPI_NOR.o
ifeq ($(CONFIG_MUTI_BOOT_DEVICE), y)
libfwprog-objs  += AmbaFlashFwProg_eMMC.o
endif
else
libfwprog-objs  += AmbaFlashFwProg_NAND.o
endif

libfwprog-objs  += ../../svc/comsvc/misc/AmbaUtility.o
libfwprog-objs  += ../../svc/comsvc/misc/AmbaUtility_Crc32Hw.o

CFLAGS_AmbaVer.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -I$(srctree)/fwprog/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg

ccflags-y += -I$(srctree)/svc/comsvc/misc


asflags-y += -I$(srctree)/vendors/ambarella/inc
asflags-y += -I$(srctree)/vendors/ambarella/inc/io
asflags-y += -I$(srctree)/soc/io/src/common/inc
asflags-y += -I$(srctree)/soc/io/src/common/inc/arm

asflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
asflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
asflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
asflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg

