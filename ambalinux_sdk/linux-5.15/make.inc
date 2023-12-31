##
## kernel/linux/make.inc
##
## History:
##    2012/06/01 - [Cao Rongrong] Created file
##
## Copyright 2008-2015 Ambarella Inc.  All Rights Reserved.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_LINUX_KERNEL_VERSION),"5.15")

LOCAL_PATH:=$(call my-dir)

AMBARELLA_BUILD_KERNEL := ARCH=$(CPU_ARCH) $(LINUX_INSTALL_FLAG)
AMBARELLA_BUILD_KERNEL += -C $(LINUX_SRC_DIR) O=$(LINUX_OUT_DIR)
AMBARELLA_BUILD_KERNEL += LOCALVERSION=

ifeq ($(CONFIG_BOOT_MEDIA_EMMC)$(CONFIG_BOOT_MEDIA_STACK), y)
AMBARELLA_BUILD_KERNEL += CONFIG_MMC=y CONFIG_MMC_BLOCK=y CONFIG_MMC_AMBARELLA=y
AMBARELLA_BUILD_KERNEL += CONFIG_MMC_SDHCI=y CONFIG_MMC_SDHCI_PLTFM=y CONFIG_MMC_SDHCI_AMBARELLA=y
AMBARELLA_BUILD_KERNEL += CONFIG_JBD2=y CONFIG_FS_MBCACHE=y CONFIG_EXT4_FS=y
endif

ifeq ($(CONFIG_UBOOT)_$(CONFIG_BOOT_FROM_EMMC), y_y)
AMBARELLA_BUILD_KERNEL += CONFIG_MMC=y CONFIG_MMC_BLOCK=y CONFIG_MMC_AMBARELLA=y
AMBARELLA_BUILD_KERNEL += CONFIG_MMC_SDHCI=y CONFIG_MMC_SDHCI_PLTFM=y CONFIG_MMC_SDHCI_AMBARELLA=y
AMBARELLA_BUILD_KERNEL += CONFIG_JBD2=y CONFIG_FS_MBCACHE=y CONFIG_EXT4_FS=y
AMBARELLA_BUILD_KERNEL += CONFIG_EFI_PARTITION=y
endif

ifeq ($(CONFIG_ARCH_CV5), y)
ifeq ($(CONFIG_AARCH64_TRUSTZONE), y)
AMBARELLA_BUILD_KERNEL += CONFIG_AMBARELLA_LPDDR5_ADJUST=y
ifeq ($(CONFIG_ATF_AMBALINK), y)
AMBARELLA_BUILD_KERNEL += CONFIG_AMBARELLA_DRAMC_STATS=y
endif
endif
endif

ifeq ($(CONFIG_BUILD_INITRAMFS), y)
AMBARELLA_BUILD_KERNEL += CONFIG_INITRAMFS_SOURCE=${INITRAMFS_OUT_DIR}/rootfs.cpio.gz
endif

ifeq ($(CONFIG_ARCH_CV3), y)
AMBARELLA_BUILD_KERNEL += KCFLAGS="-I$(AMB_TOPDIR)/safety/armhsm/include \
			-I$(AMB_TOPDIR)/safety/armhsm/build/linux-aarch64/cryptoki"
endif

#####

.PHONY: linux

linux: $(DOT_CONFIG)
	@mkdir -p $(LINUX_OUT_DIR)
	@if [ -f $(LINUX_OUT_DIR)/.config ]; then \
		echo "Build Linux-$(CONFIG_LINUX_KERNEL_VERSION) with previous configuration ..."; \
	else \
		$(MAKE) $(AMBA_MAKE_PARA) ARCH=$(CPU_ARCH) defconfig_public_linux; \
	fi
	$(AMBA_MAKEFILE_V)$(MAKE) $(AMBA_MAKE_PARA) $(AMBARELLA_BUILD_KERNEL) olddefconfig
	$(AMBA_MAKEFILE_V)$(MAKE) $(AMBA_MAKE_PARA) $(AMBARELLA_BUILD_KERNEL) all
	@if [ -n "$(KERNEL_INSTALL_PATH)" ]; then \
		echo "Install Linux modules to $(KERNEL_INSTALL_PATH) ..."; \
		mkdir -p $(KERNEL_INSTALL_PATH); \
		$(MAKE) $(AMBA_MAKE_PARA) $(AMBARELLA_BUILD_KERNEL) modules_install; \
		find $(KERNEL_INSTALL_PATH)/lib/modules/ -name build | xargs -l1 rm -rf; \
		find $(KERNEL_INSTALL_PATH)/lib/modules/ -name source | xargs -l1 rm -rf; \
	fi
	@cp -dpRf $(LINUX_OUT_DIR)/arch/$(CPU_ARCH)/boot/Image $(KERNEL_OUT_DIR)/Image
	@if [ -f $(LINUX_OUT_DIR)/arch/$(CPU_ARCH)/boot/zImage ]; then \
		cp -dpRf $(LINUX_OUT_DIR)/arch/$(CPU_ARCH)/boot/zImage $(KERNEL_OUT_DIR)/zImage; \
	fi
	@echo "Build $@ Done."

$(call add-target-into-build, linux)

###

.PHONY: menuconfig_public_linux

menuconfig_public_linux: $(DOT_CONFIG)
	@mkdir -p $(LINUX_OUT_DIR)
	@$(MAKE) $(AMBA_MAKE_PARA) $(AMBARELLA_BUILD_KERNEL) menuconfig

###

.PHONY: defconfig_public_linux

defconfig_public_linux: $(DOT_CONFIG)
	@echo "Build Linux-$(CONFIG_LINUX_KERNEL_VERSION) with $(KERNEL_DEFCONFIG) ..."
	@mkdir -p $(LINUX_OUT_DIR)
	@if [ -f $(AMB_BOARD_DIR)/config/kernel/$(KERNEL_DEFCONFIG) ]; then \
		cp -dpRf $(AMB_BOARD_DIR)/config/kernel/$(KERNEL_DEFCONFIG) $(LINUX_OUT_DIR)/.config; \
	else \
		$(MAKE) $(AMBA_MAKE_PARA) $(AMBARELLA_BUILD_KERNEL) $(KERNEL_DEFCONFIG); \
	fi

#####

savedefconfig_public_linux: $(DOT_CONFIG)
	@$(MAKE) $(AMBA_MAKE_PARA) $(AMBARELLA_BUILD_KERNEL) savedefconfig
	cp -rf $(LINUX_OUT_DIR)/defconfig $(LINUX_SRC_DIR)/arch/arm64/configs/$(KERNEL_DEFCONFIG)
endif

