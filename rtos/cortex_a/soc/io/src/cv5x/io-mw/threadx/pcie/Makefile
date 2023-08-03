

# Build PCIE libraries

libpcie-objs += \
		src/AmbaPCIE_Drv_Entry.o \
		src/AmbaPCIE_Drv_Udma.o \
		src/AmbaPCIE_Drv_UdmaInfo.o \
		src/AmbaPCIE_Drv_UdmaInterrupt.o \
		src/AmbaPCIE_Drv_RpMgmt.o \
		src/AmbaPCIE_Drv_AxiConfig.o \
		src/AmbaPCIE_Drv_Lm.o \
		src/AmbaPCIE_Drv_LmLink.o \
		src/AmbaPCIE_Drv_LmTiming.o \
		src/AmbaPCIE_Drv_LmCredit.o \
		src/AmbaPCIE_Drv_LmInterrupt.o \
		src/AmbaPCIE_Drv_LmDbgMux.o \
		src/AmbaPCIE_Drv_LmCount.o \
		src/AmbaPCIE_Drv_LmBar.o \
		src/AmbaPCIE_Deprecated.o \
		src/AmbaPCIE_System.o \
		src/AmbaPCIE_Utility.o \
		src/AmbaPCIE_Phy.o \
		src/AmbaPCIE_Common.o \
		src/AmbaPCIE_Root.o \
		src/AmbaPCIE_Endpoint.o

obj-y += libpcie.a
install-files += libpcie.a



CFLAGS_AmbaVer_UsbSys.o = \
        -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)


ccflags-y += -D_AMBA_KAL_ \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/vendors/ambarella/inc/prfile2 \
        -I$(srctree)/soc/io/src/common/inc \
		-I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/io-mw/threadx/pcie/inc \
		-I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc \
		-I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc/aarch64 \
		-I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc/aarch32 \
        -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/common/inc/reg

#ccflags-y += -Werror

# Disable GCC 9 warning about pointers to members of a packed struct
ccflags-y += -Wno-address-of-packed-member
# Disabl ISO C90 forbids mixed declarations and code
ccflags-y += -Wno-declaration-after-statement

ccflags-y += -D__CORE__=5 -D__ARM4TM__=1 -D__CPU_MODE__=2
ccflags-y += -D_AMBA_KAL_

