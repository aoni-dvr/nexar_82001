
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libwrap_std.a

libwrap_std-objs := \
	../../src/common/AmbaVer.o

libwrap_std-objs += \
	../../src/linux/AmbaMemcpy.o	\
	\
	../../src/common/libc/abs.o		\
	../../src/common/libc/memcmp.o	\
	../../src/common/libc/memmove.o	\
	../../src/common/libc/memset.o	\
	../../src/common/libc/rand.o		\
	../../src/common/libc/srand.o	\
	\
	../../src/common/libm/acos.o		\
	../../src/common/libm/asin.o		\
	../../src/common/libm/atan.o		\
	../../src/common/libm/ceil.o		\
	../../src/common/libm/cos.o		\
	../../src/common/libm/exp.o		\
	../../src/common/libm/fabs.o		\
	../../src/common/libm/floor.o	\
	../../src/common/libm/log10.o	\
	../../src/common/libm/log.o		\
	../../src/common/libm/log2.o		\
	../../src/common/libm/pow.o		\
	../../src/common/libm/sin.o		\
	../../src/common/libm/sqrt.o		\
	../../src/common/libm/tan.o


# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libwrap_std-objs))

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -Ivendors/ambarella/inc

install-files := libwrap_std.a


