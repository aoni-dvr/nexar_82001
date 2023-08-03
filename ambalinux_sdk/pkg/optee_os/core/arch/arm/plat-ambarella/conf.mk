include core/arch/arm/cpu/cortex-armv8-0.mk

# 32-bit flags
core_arm32-platform-aflags	+= -mfpu=neon

$(call force,CFG_AMBARELLA_UART,y)
$(call force,CFG_AMBARELLA_TRNG,y)
$(call force,CFG_GENERIC_BOOT,y)
$(call force,CFG_PM_STUBS,y)
$(call force,CFG_SECURE_TIME_SOURCE_CNTPCT,y)
$(call force,CFG_WITH_ARM_TRUSTED_FW,y)

ifeq ($(CFG_ARM64_core),y)
$(call force,CFG_WITH_LPAE,y)
ta-targets += ta_arm64
else
$(call force,CFG_ARM32_core,y)
endif

# only support ta_arm64 on ambarella platform
supported-ta-targets:=$(ta-targets)

CFG_NUM_THREADS ?= 4
CFG_CRYPTO_WITH_CE ?= y
CFG_WITH_STACK_CANARIES ?= y

CFG_TEE_CORE_EMBED_INTERNAL_TESTS ?= y
CFG_WITH_STATS ?= y

CFG_WITH_AMBARELLA_OTP ?= y

arm32-platform-cflags += -Wno-error=cast-align
arm64-platform-cflags += -Wno-error=cast-align

GCC10 = $(shell expr `$(CROSS_COMPILE)gcc -dumpversion | cut -f1 -d.` \>= 10)
ifeq ($(GCC10),1)
arm64-platform-cflags += -mno-outline-atomics
endif

#$(call force,CFG_CRYPTO_SHA256_ARM32_CE,n)
#$(call force,CFG_CRYPTO_SHA256_ARM64_CE,n)
#$(call force,CFG_CRYPTO_SHA1_ARM32_CE,n)
#$(call force,CFG_CRYPTO_SHA1_ARM64_CE,n)
#$(call force,CFG_CRYPTO_AES_ARM64_CE,n)
