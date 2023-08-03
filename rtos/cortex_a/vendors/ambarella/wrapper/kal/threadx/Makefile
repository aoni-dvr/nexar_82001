ifeq ($(CONFIG_BUILD_SSP_KAL),y)

NOSTDINC_FLAGS :=
KBUILD_CFLAGS += -DCONFIG_SSP_THREADX_NEWLIB -D__DYNAMIC_REENT__ -include reent.h -DCONFIG_KAL_THREADX_NEWLIB
KBUILD_AFLAGS += -DCONFIG_SSP_THREADX_NEWLIB -D__DYNAMIC_REENT__

obj-y += libkal.a

ifeq ($(CONFIG_ARM32),y)
libkal-objs += \
        tx_initialize_low_level_A32.o \
        tx_execution_profile.o \
        tx_misra_A32.o \
        AmbaKAL_ThreadX.o \
        AmbaDiag_ThreadX.o \
        AmbaBoot_A32.o

ifeq ($(CONFIG_XEN_SUPPORT),y)
libkal-objs += AmbaBoot_A32_xen.o
endif

endif # $(CONFIG_ARM32)

ifeq ($(CONFIG_ARM64),y)
libkal-objs += \
        tx_initialize_low_level_A64.o \
        tx_execution_profile_A64.o \
        tx_misra.o \
        AmbaKAL_ThreadX_A64.o \
        AmbaDiag_ThreadX_A64.o \
        AmbaBoot_A64.o

ifeq ($(CONFIG_XEN_SUPPORT),y)
libkal-objs += AmbaBoot_A64_xen.o
endif
endif

libkal-objs += AmbaMain.o
libkal-objs += AmbaSYS_DispInfo.o
libkal-objs += AmbaVer_LibKal.o

CFLAGS_AmbaVer_LibKal.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/
asflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/

ccflags-y += -I$(srctree)/soc/io/src/common/inc/
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx

ccflags-$(CONFIG_XEN_SUPPORT) += -I$(srctree)/vendors/foss/xen/include

asflags-y += -I$(srctree)/vendors/ambarella/inc/
asflags-y += -I$(srctree)/vendors/ambarella/inc/io/
asflags-y += -I$(srctree)/vendors/ambarella/inc/threadx/

ifeq ($(CONFIG_CPU_V7_CA9),y)
ccflags-y += -D__CORE__=5 -D__ARM4TM__=1 -D__CPU_MODE__=2
endif

ifneq ($(CONFIG_KAL_THREADX_SMP),y)
ccflags-y += -DAMBA_KAL_NO_SMP
endif

ifeq ($(CONFIG_KAL_THREADX_FIQ),y)
asflags-y += -DTX_ENABLE_FIQ_SUPPORT
endif

install-files := libkal.a

endif
