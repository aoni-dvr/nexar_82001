ifeq ($(CONFIG_BUILD_SSP_FS),y)

obj-y           := libfs_$(AMBA_CHIP_ID).a
install-files   := libfs_$(AMBA_CHIP_ID).a

libfs_$(AMBA_CHIP_ID)-objs += \
        src/AmbaFS_PrFile2.o \
        src/AmbaFS_Format.o \
        kal/AmbaFS_KAL.o \
        kal/pf_critical.o \
        driver/AmbaFS_PrFile2_SD.o \
        driver/AmbaFS_PrFile2_NAND.o

# Build FS library
ifneq ($(wildcard $(srctree)/vendors/esol/prfile2),)
PRFILE2_SRC_PATH = ../../../../esol/prfile2
include $(src)/$(PRFILE2_SRC_PATH)/Makefile.in

obj-y           += libprfile.a
install-files   += libprfile.a
endif

ccflags-y += -DAMBA_FS_KAL
ccflags-y +=  -I$(srctree)/vendors/ambarella/inc
ccflags-y +=  -I$(srctree)/vendors/ambarella/inc/io
ccflags-y +=  -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y +=  -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y +=  -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc
ccflags-y +=  -I$(srctree)/soc/io/src/common/inc
ccflags-y +=  -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y +=  -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc


ccflags-y += -D__CORE__=5 -D__ARM4TM__=1 -D__CPU_MODE__=2
ccflags-y += -DAMBA_FS_KAL


endif # $(CONFIG_BUILD_SSP_FS)
