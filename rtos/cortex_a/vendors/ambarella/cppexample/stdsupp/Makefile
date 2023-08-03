
ifeq ($(CONFIG_THREADX),y)
# For debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))
#

obj-y += libstdsupp.a

ccflags-y := \
	-I$(srctree)/svc/comsvc/print \
    -I$(srctree)/vendors/ambarella/inc/io \
	-I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
	-I$(srctree)/vendors/ambarella/inc/threadx \
    -I$(srctree)/vendors/ambarella/inc

libstdsupp-objs := \
	src/AmbaCRT0.o \
	src/AmbaMemPoolUtil.o \
	src/malloc.o \
	src/free.o \
	src/calloc.o \
	src/realloc.o

ifneq ($(CONFIG_CC_CXX_SUPPORT),)
libstdsupp-objs += src/AmbaCtorDtor.o
libstdsupp-objs += test/cpp_case.o
endif

libstdsupp-objs += AmbaVer.o
CFLAGS_AmbaVer.o = \
	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE) \
	-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
	-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM) \
	-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE) \
	-DAMBA_CI_COUNT=$(AMBA_CI_COUNT) \
	-DAMBA_CI_ID=$(AMBA_CI_ID)

install-files += libstdsupp.a

endif
# end of ifeq ($(CONFIG_THREADX),y)