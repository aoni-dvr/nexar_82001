# Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# Neither the name of ARM nor the names of its contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

override ENABLE_ASSERTIONS		:= 1
override CRASH_REPORTING                := 1
override ERROR_DEPRECATED		:= 1

override PROGRAMMABLE_RESET_ADDRESS	:= 0
override COLD_BOOT_SINGLE_CPU		:= 1
override RESET_TO_BL31			:= 1
override PSCI_EXTENDED_STATE_ID		:= 1
override A53_DISABLE_NON_TEMPORAL_HINT	:= 0
override SEPARATE_CODE_AND_RODATA	:= 1

override ENABLE_PIE			:= 1


### How to find chip revision : MPIDR_EL1
### On CV5: MPIDR_EL1=0x414FD0B1 which means r4p1
ifeq (${CHIP}, cv5)
ERRATA_A76_1073348			:= 0
ERRATA_A76_1130799			:= 0
ERRATA_A76_1220197			:= 0
ERRATA_A76_1257314			:= 0
ERRATA_A76_1262606			:= 0
ERRATA_A76_1262888			:= 0
ERRATA_A76_1275112			:= 0
ERRATA_A76_1286807			:= 0
ERRATA_A76_1791580			:= 0
ERRATA_A76_1165522			:= 1
ERRATA_A76_1868343			:= 0
ERRATA_A76_1946160			:= 1

CTX_INCLUDE_AARCH32_REGS		:= 0

HW_ASSISTED_COHERENCY			:= 1

USE_COHERENT_MEM			:= 0

else

# Enable workarounds for selected Cortex-A53 errata.
ERRATA_A53_835769			:= 1
ERRATA_A53_836870			:= 1
ERRATA_A53_843419			:= 1
ERRATA_A53_855873			:= 1

endif

# enable assert() for release/debug builds
PLAT_LOG_LEVEL_ASSERT			:= 40
$(eval $(call add_define,PLAT_LOG_LEVEL_ASSERT))

# enable dynamic memory mapping
PLAT_XLAT_TABLES_DYNAMIC		:= 1
$(eval $(call add_define,PLAT_XLAT_TABLES_DYNAMIC))

#
# Add Ambarella definition
#

-include $(AUTOCONF)

ifeq (${CHIP}, cv2)
$(eval $(call add_define,AMBARELLA_CV2))
endif
ifeq (${CHIP}, cv22)
$(eval $(call add_define,AMBARELLA_CV22))
endif
ifeq (${CHIP}, cv25)
$(eval $(call add_define,AMBARELLA_CV25))
endif
ifeq (${CHIP}, s6lm)
$(eval $(call add_define,AMBARELLA_S6LM))
endif
ifeq (${CHIP}, cv28)
$(eval $(call add_define,AMBARELLA_CV28))
endif
ifeq (${CHIP}, cv5)
$(eval $(call add_define,AMBARELLA_CV5))
endif
ifeq (${CHIP}, cv2fs)
$(eval $(call add_define,AMBARELLA_CV2FS))
LDLIBS += -Lplat/ambarella/lib -lfusa_ca53
endif
ifeq (${CHIP}, cv22fs)
$(eval $(call add_define,AMBARELLA_CV22FS))
LDLIBS += -Lplat/ambarella/lib -lfusa_ca53
endif

# Libraries
include lib/xlat_tables_v2/xlat_tables.mk
include lib/libfdt/libfdt.mk

PLAT_INCLUDES		:=	-Iinclude/drivers/ambarella/uart/		\
				-Iplat/ambarella/include

AMBARELLA_DRIVERS	:=	$(wildcard plat/ambarella/driver/*.c)		\
				$(wildcard plat/ambarella/driver/*.S)

# For ambarella_otp.c
ifeq (${CHIP}, cv2fs)
AMBARELLA_DRIVERS := $(filter-out plat/ambarella/driver/ambarella_otp.c, $(AMBARELLA_DRIVERS))
AMBARELLA_DRIVERS += $(wildcard plat/ambarella/driver/ambalink/cv2fs/*.c)
PLAT_INCLUDES     += -Iplat/ambarella/driver/ambalink/cv2fs
endif
ifeq (${CHIP}, cv22fs)
AMBARELLA_DRIVERS := $(filter-out plat/ambarella/driver/ambarella_otp.c, $(AMBARELLA_DRIVERS))
AMBARELLA_DRIVERS += $(wildcard plat/ambarella/driver/ambalink/cv2fs/*.c)
PLAT_INCLUDES     += -Iplat/ambarella/driver/ambalink/cv2fs
endif

TF_LDFLAGS		+= --cref

# common sources for BL2/BL31
PLAT_BL_COMMON_SOURCES	+=	plat/ambarella/ambarella_xlat_setup.c		\
				${XLAT_TABLES_LIB_SRCS}

ifeq (${CHIP}, cv5)
BL31_SOURCES		+=	lib/cpus/aarch64/cortex_a76.S
else
BL31_SOURCES		+=	lib/cpus/aarch64/cortex_a53.S
endif

BL31_SOURCES		+=	drivers/arm/gic/common/gic_common.c		\
				drivers/arm/gic/v2/gicv2_main.c			\
				drivers/arm/gic/v2/gicv2_helpers.c		\
				drivers/delay_timer/delay_timer.c		\
				drivers/delay_timer/generic_delay_timer.c	\
				drivers/gpio/gpio.c				\
				plat/common/plat_gicv2.c			\
				plat/common/plat_psci_common.c			\
				$(AMBARELLA_DRIVERS)				\
				plat/ambarella/aarch64/ambarella_suspend.S	\
				plat/ambarella/aarch64/ambarella_helpers.S	\
				plat/ambarella/aarch64/el2_runtime_exceptions.S	\
				plat/ambarella/ambarella_bl31_cpufreq.c		\
				plat/ambarella/ambarella_bl31_setup.c		\
				plat/ambarella/ambarella_security.c		\
				plat/ambarella/ambarella_boot_cookie.c		\
				plat/ambarella/ambarella_soc_fixup.c		\
				plat/ambarella/ambarella_gicv2.c		\
				plat/ambarella/ambarella_psci.c			\
				plat/ambarella/ambarella_topology.c		\
				plat/ambarella/ambarella_sip_svc.c		\
				${LIBFDT_SRCS}

ifeq ($(BL2_AT_EL3),1)

ifdef CONFIG_ATF_SPD_OPTEE
# But RTOS... cf. docs/components/exception-handling.rst
#override GICV2_G0_FOR_EL3 := 1
endif # CONFIG_ATF_SPD_OPTEE

ifeq (${CHIP}, cv5)
BL2_SOURCES		+=	lib/cpus/aarch64/cortex_a76.S
else
BL2_SOURCES		+=	lib/cpus/aarch64/cortex_a53.S
endif

BL2_SOURCES		+=	drivers/io/io_block.c			\
				drivers/io/io_fip.c			\
				drivers/io/io_storage.c			\
				common/desc_image_load.c		\
				$(AMBARELLA_DRIVERS)			\
				plat/ambarella/ambarella_bl2_setup.c	\
				plat/ambarella/ambarella_boot_cookie.c	\
				plat/ambarella/ambarella_image_desc.c	\
				plat/ambarella/ambarella_io_storage.c	\
				plat/ambarella/aarch64/ambarella_helpers.S

ifeq (${DRAM_TRAINING}, y)	#do dram training in BL2
ifndef CONFIG_BST_DRAM_TRAINING
PLAT_INCLUDES		+=	-I$(srctree)/boot/$(AMBA_CHIP_ID)/secure/bootloader/inc/	\
				-I$(srctree)/vendors/ambarella/inc/	\
				-I$(srctree)/vendors/ambarella/inc/io/	\
				-I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)	\
				-I$(srctree)/soc/io/src/common/inc/	\
				-I$(srctree)/soc/io/src/common/inc/arm/	\
				-I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/	\
				-I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl/	\
				-I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/	\
				-I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/	\
				-I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport/
ifdef CONFIG_LINUX
LDLIBS += -L$(objtree)/lib
else
LDLIBS += -L$(srctree)/output.64/lib
endif
ifeq ($(BOOT),nand)
LDLIBS += -lbld_nand
endif
ifeq ($(BOOT),spinand)
LDLIBS += -lbld_nand
endif
ifeq ($(BOOT),spinor)
LDLIBS += -lbld_spinor
endif
ifeq ($(BOOT),emmc)
LDLIBS += -lbld_emmc
endif
LDLIBS += -lbsp_$(AMBA_BSP_NAME)
LDLIBS += -lio_$(AMBA_CHIP_ID) \
          -lio_common

endif # CONFIG_BST_DRAM_TRAINING
endif	#DRAM_TRAINING

ifeq (${TRUSTED_BOARD_BOOT},1)

include drivers/auth/mbedtls/mbedtls_crypto.mk
include drivers/auth/mbedtls/mbedtls_x509.mk

BL2_SOURCES		+=	drivers/auth/auth_mod.c			\
				drivers/auth/crypto_mod.c		\
				drivers/auth/img_parser_mod.c		\
				drivers/auth/tbbr/tbbr_cot.c		\
				plat/common/tbbr/plat_tbbr.c		\
				plat/ambarella/ambarella_rotpk.S	\
				plat/ambarella/ambarella_tbbr.c

endif	# TRUSTED_BOARD_BOOT

endif

#ifneq ($(PLAT_CFG_BL33_BASE),)
#$(eval $(call add_define,PLAT_CFG_BL33_BASE))
#else
#$(error "PLAT_CFG_BL33_BASE is not defined")
#endif
#
#ifneq ($(PLAT_CFG_BL32_SHMEM_SIZE),)
#$(eval $(call add_define,PLAT_CFG_BL32_SHMEM_SIZE))
#else
#$(error "PLAT_CFG_BL32_SHMEM_SIZE is not defined")
#endif
#
ifneq ($(PLAT_CFG_ATF_EMBED_PUB_COT_ROOT),)
$(eval $(call add_define,PLAT_CFG_ATF_EMBED_PUB_COT_ROOT))
ifneq ($(PLAT_CFG_PUB_COT_ROOT_DER),)
$(eval $(call add_define_val,PLAT_CFG_PUB_COT_ROOT_DER,'"$(PLAT_CFG_PUB_COT_ROOT_DER)"'))
else
$(error "PLAT_CFG_PUB_COT_ROOT_DER is not specified")
endif
endif
