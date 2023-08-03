ifeq ($(CONFIG_BUILD_SSP_KAL),y)

obj-y += libkal.a

libkal-objs += AmbaMisra.o
libkal-objs += AmbaMisraFix.o
libkal-objs += AmbaKAL_LINUX.o
libkal-objs += AmbaVer_LibKal.o

CFLAGS_AmbaVer_LibKal.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += \
        -I$(srctree)/soc/io/src/common/inc/ \
        -I$(srctree)/soc/io/src/common/inc/arm/ \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/ \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl/ \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/ \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/ \
        -I$(srctree)/vendors/ambarella/inc/ \
        -I$(srctree)/vendors/ambarella/inc/io/ \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/vendors/ambarella/inc/linux

asflags-y += \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/ \
        -I$(srctree)/vendors/ambarella/inc/ \
        -I$(srctree)/vendors/ambarella/inc/io/ \
        -I$(srctree)/vendors/ambarella/inc/linux/

install-files := libkal.a

endif
