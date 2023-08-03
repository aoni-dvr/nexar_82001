
ifneq ($(CONFIG_ARM64),)
ifneq ($(CONFIG_ATF_BUILD),)

obj-y += .stamp_build_atf

ATF_TARGET := bl31
# Workaround for buildroot -> kbuild (RTOS)
ifneq ($(KBUILD_SRC),)
ATF_BUILD_BASE=$(CURDIR)/$(obj)/build
ATF_KCONFIG_INCLUDE=-I. -include $(CURDIR)/include/generated/autoconf.h
else
ATF_BUILD_BASE=$(objtree)/$(obj)/build
ATF_KCONFIG_INCLUDE=$(LINUXINCLUDE)
endif

#CROSS_COMPILE=aarch64-none-elf-
ATF_DEFAULT_PLAT=ambarella

ifneq ($(CONFIG_CPU_CORTEX_A53),)
ARM_ARCH_MINOR=0
else
ARM_ARCH_MINOR=2
endif

ifneq ($(CONFIG_SOC_CV22),)
ATF_CHIP=cv22
else ifneq ($(CONFIG_SOC_CV2),)
ATF_CHIP=cv2
else ifneq ($(CONFIG_SOC_CV25),)
ATF_CHIP=cv25
else ifneq ($(CONFIG_SOC_CV28),)
ATF_CHIP=cv28
else ifneq ($(CONFIG_SOC_CV5),)
ATF_CHIP=cv5
else ifneq ($(CONFIG_SOC_CV52),)
ATF_CHIP=cv5
else ifneq ($(CONFIG_SOC_H22),)
ATF_CHIP=s5l
else ifneq ($(CONFIG_SOC_H32),)
ATF_CHIP=s6lm
else ifneq ($(CONFIG_SOC_CV2FS),)
ATF_CHIP=cv2fs
else ifneq ($(CONFIG_SOC_CV22FS),)
ATF_CHIP=cv2fs
else
$(error Not supported chip!)
endif

# Boot with cache enable, need to define WARMBOOT_ENABLE_DCACHE_EARLY for makefile
WARMBOOT_ENABLE_DCACHE_EARLY=1

MBEDTLSDIR  := ls $(srctree)/vendors/foss/mbedtls/src | grep -E "mbedtls-[0-9]+.[0-9]+.[0-9]+" | tail -n 1

V=$(KBUILD_VERBOSE)

ifneq ($(CONFIG_ATF_DEBUG),)
ATF_DEBUG=1
cmd_find_binary := find $(ATF_BUILD_BASE) | grep $(ATF_DEFAULT_PLAT)/debug
else
ATF_DEBUG=0
cmd_find_binary := find $(ATF_BUILD_BASE) | grep $(ATF_DEFAULT_PLAT)/release
endif

ifneq ($(CONFIG_ATF_HAVE_BL2),)
BL2_AT_EL3=1
ATF_TARGET += bl2
else
BL2_AT_EL3=0
endif

ifneq ($(CONFIG_ATF_SPD_OPTEE),)
SPD=opteed
else
SPD=none
endif

ifneq ($(CONFIG_ATF_HAVE_BL2),)
ifneq ($(CONFIG_AUTO_DRAM_TRAINING),)
DRAM_TRAINING=y
endif
ifneq ($(CONFIG_ENABLE_NAND_BOOT),)
BOOT="nand"
endif
ifneq ($(CONFIG_ENABLE_SPINOR_BOOT),)
BOOT="spinor"
endif
ifneq ($(CONFIG_ENABLE_EMMC_BOOT),)
BOOT="emmc"
endif
ifneq ($(CONFIG_ENABLE_SPINAND_BOOT),)
BOOT="spinand"
endif
endif # CONFIG_ATF_HAVE_BL2

ifneq ($(CONFIG_ATF_HAVE_BL2),)
ifneq ($(CONFIG_SOC_CV2FS),)
ATF_BL2 := $(CONFIG_FWPROG_BLD2)
else # others
ATF_BL2 := $(CONFIG_FWPROG_BLD)
endif # CV2FS
endif # CONFIG_ATF_HAVE_BL2

ifneq ($(CONFIG_ENABLE_SECURITY),)
TRUSTED_BOARD_BOOT=1
GENERATE_COT=1
ifneq ($(CONFIG_ATF_HAVE_BL2),)
ifneq ($(CONFIG_SECURE_TYPE_ECC),)
OPT_ATF_EMBED_PUB_COT_ROOT = PLAT_CFG_ATF_EMBED_PUB_COT_ROOT=y
OPT_PUB_COT_ROOT_DER = PLAT_CFG_PUB_COT_ROOT_DER=$(abspath $(obj)/file.der)
# Add dependence
$(obj)/.stamp_build_atf: $(obj)/file.der

endif
endif  # ifneq ($(CONFIG_ATF_HAVE_BL2),)
else # No-Secure
TRUSTED_BOARD_BOOT=0
GENERATE_COT=0
endif

# auto.conf from Kbuild
AUTOCONF=$(objtree)/include/config/auto.conf

$(obj)/.stamp_build_atf: FORCE
	$(Q)INCLUDES="$(ATF_KCONFIG_INCLUDE)" \
		$(MAKE) \
		-C $(srctree)/$(src)/src/arm-trusted-firmware/ \
		BUILD_BASE=$(ATF_BUILD_BASE) \
		V=$(V) \
		ARM_ARCH_MINOR=$(ARM_ARCH_MINOR) \
		DEBUG=$(ATF_DEBUG) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		DEFAULT_PLAT=$(ATF_DEFAULT_PLAT) \
		CHIP=$(ATF_CHIP) \
		WARMBOOT_ENABLE_DCACHE_EARLY=$(WARMBOOT_ENABLE_DCACHE_EARLY) \
		BL2_AT_EL3=$(BL2_AT_EL3) \
		TRUSTED_BOARD_BOOT=$(TRUSTED_BOARD_BOOT) GENERATE_COT=$(GENERATE_COT) \
		MBEDTLS_DIR=$(srctree)/vendors/foss/mbedtls/src/`$(MBEDTLSDIR)` \
		SPD=$(SPD) \
		DRAM_TRAINING=$(DRAM_TRAINING) BOOT=$(BOOT) AMBA_BSP_NAME=$(AMBA_BSP_NAME) \
		AUTOCONF=$(AUTOCONF) \
		$(OPT_ATF_EMBED_PUB_COT_ROOT) \
		$(OPT_PUB_COT_ROOT_DER) \
		all
	$(Q)install -D `$(cmd_find_binary) | grep bl31.bin$$` $(CONFIG_ATF_BL31_BIN)
	$(Q)if [ "$(CONFIG_ATF_HAVE_BL2)" != "" ]; then \
		install -D `$(cmd_find_binary) | grep bl2.bin$$` $(ATF_BL2); \
	fi
	$(Q)if [ "$(CONFIG_ATF_HAVE_BL2)" != "" ]; then \
		$(MAKE) \
			-C $(srctree)/$(src)/src/arm-trusted-firmware \
			BUILD_BASE=$(ATF_BUILD_BASE) \
			fiptool certtool; \
		mkdir -p $(objtree)/$(obj)/cert/bin; \
		cp -f $(srctree)/$(src)/src/arm-trusted-firmware/tools/fiptool/fiptool $(objtree)/$(obj)/cert/bin/; \
		cp -f $(srctree)/$(src)/src/arm-trusted-firmware/tools/cert_create/cert_create $(objtree)/$(obj)/cert/bin/; \
	fi
	$(Q)touch $@

endif # CONFIG_ATF_BUILD


ifneq ($(CONFIG_ATF_HAVE_BL2),)
obj-y += .stamp_cert_fip

ATF_BL2_ALIGN_SH := $(srctree)/$(src)/align_bl2.sh

$(obj)/.stamp_cert_fip: $(if $(CONFIG_ATF_BUILD),$(obj)/.stamp_build_atf,) $(shell echo $(CONFIG_ATF_BL32_BIN) $(CONFIG_ATF_BL33_BIN) $(if $(CONFIG_ATF_FIP_RTOS),$(CONFIG_FWPROG_SYS)))
	$(Q)$(MAKE) -C $(srctree)/$(src)/cert
	$(Q)touch $@
ifneq ($(CONFIG_ENABLE_SECURITY),)
	$(Q)$(ATF_BL2_ALIGN_SH) $(ATF_BL2) $(CONFIG_RSA_PRIVATE_KEY_PATH)
endif
ifneq ($(CONFIG_ATF_FIP_RTOS),)
	@echo "Generate dummy system image: ${CONFIG_FWPROG_SYS} ..."
	$(Q)rm -f ${CONFIG_FWPROG_SYS} && touch ${CONFIG_FWPROG_SYS}
endif


ifneq ($(CONFIG_ENABLE_SECURITY),)
ifneq ($(CONFIG_SECURE_TYPE_ECC),)
# NOTE: Only RSA for ATF now, so ECC key in OTP can not work.
$(obj)/file.der: $(abspath $(strip $(subst ",,$(CONFIG_RSA_PRIVATE_KEY_PATH))))
	$(Q)$(CONFIG_OPENSSL_PATH) rsa -in $< -pubout -outform der -out $@
endif
endif

endif # ifneq ($(CONFIG_ATF_HAVE_BL2),)

endif # ($(CONFIG_ARM64),)


clean-files += .stamp_build_atf
clean-files += .stamp_cert_fip

