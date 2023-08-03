
WOLFSSL_VER := 4.8.1


#obj-$(CONFIG_WOLFSSL) += .stamp_build
obj-y := libwolfssl.a
obj-y += libwolfssl_test.a

# Need to disable flags to use headers of toolchain.
#NOSTDINC_FLAGS :=


# For debug
KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

# omit some warning
KBUILD_CFLAGS := $(subst -Werror=,-Wno-error=,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -Werror , ,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -Werror-implicit-function-declaration , ,$(KBUILD_CFLAGS))
KBUILD_CFLAGS := $(subst -include reent.h , ,$(KBUILD_CFLAGS))


# Need syscall APIs for OS
WOLFSSL_CFLAGS := -specs=nosys.specs
## wolfSSL definitions
WOLFSSL_CFLAGS += -DWOLFSSL_USER_SETTINGS
#
#WOLFSSL_CFLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include)
#WOLFSSL_CFLAGS += -mcpu=cortex-a53 -mthumb -mabi=aapcs -nostdinc -nostdlib
#WOLFSSL_CFLAGS += $(ARCHFLAGS) -std=gnu99 -Wall -Wno-cpp
#WOLFSSL_CFLAGS += -ffunction-sections -fdata-sections -fno-builtin

KBUILD_CFLAGS += $(WOLFSSL_CFLAGS)

ccflags-y += -I$(srctree)/$(src)/Header
ccflags-y += -I$(srctree)/$(src)/wolfssl-$(WOLFSSL_VER)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx

# FILES

# Port and Test/Benchmark
libwolfssl-objs := Source/AmbaCWrap.o
#libwolfssl-objs += ./Source/wolf_main.o
#libwolfssl-objs += ./Source/armtarget.o
#libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/test/test.o
#libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/benchmark/benchmark.o

# WOLFSSL TLS FILES
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/crl.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/internal.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/keys.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/ocsp.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/sniffer.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/ssl.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/tls.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/tls13.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/src/wolfio.o

# wolfCrypt Core (FIPS)
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/wolfcrypt_first.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/aes.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/cmac.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/des3.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/dh.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/ecc.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/hmac.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/random.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/rsa.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sha.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sha256.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sha512.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sha3.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/fips.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/fips_test.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/wolfcrypt_last.o

# wolfCrypt Additional
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/asn.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/chacha.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/cmac.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/coding.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/compress.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/cpuid.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/cryptocb.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/curve25519.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/ed25519.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/error.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/fe_low_mem.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/fe_operations.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/ge_low_mem.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/ge_operations.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/hash.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/integer.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/logging.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/md5.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/memory.o

# not define NO_INLINE, no need this file
#libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/misc.o

libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/pkcs12.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/pkcs7.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/poly1305.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/pwdbased.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/signature.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/srp.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_arm32.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_arm64.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_armthumb.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_c32.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_c64.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_cortexm.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/sp_int.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/tfm.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/wc_encrypt.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/wc_port.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/wolfevent.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/wolfmath.o

# wolfCrypt non-standard algorithms (disabled by default)
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/arc4.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/blake2b.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/camellia.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/dsa.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/hc128.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/idea.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/md2.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/md4.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/rabbit.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/ripemd.o

# for ARMASMa without FIP
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-sha256.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-aes.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-sha512.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-sha512-asm.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-32-sha512-asm.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-poly1305.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-chacha.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-32-curve25519.o
libwolfssl-objs += wolfssl-$(WOLFSSL_VER)/wolfcrypt/src/port/arm/armv8-curve25519.o



libwolfssl_test-objs := Source/benchmark.o

libwolfssl_test.a: $(obj)/libwolfssl.a


install-files := libwolfssl.a libwolfssl_test.a
#clean-files += .stamp_conf .stamp_build
#clean-dirs  += src

