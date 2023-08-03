
###############################################################################
#
# amba_camera: For Ambarella Camera applications.
#
# cf. "17.12. Infrastructure for packages using kconfig for configuration files"
#     https://buildroot.org/downloads/manual/manual.html#_infrastructure_for_packages_using_kconfig_for_configuration_files
#
###############################################################################
ifeq ($(BR2_PACKAGE_AMBA_CAMERA),y)

AMBA_CAMERA_VERSION		= amba
AMBA_CAMERA_SITE_METHOD		= local
AMBA_CAMERA_SITE		= $(call qstrip,$(BR2_PACKAGE_AMBA_CAMERA_SOURCE))
AMBA_CAMERA_SITE_OOSB		= $(call qstrip,$(BR2_PACKAGE_AMBA_CAMERA_OOSB))
ifeq ($(BR2_PACKAGE_AMBAGPU),y)
AMBA_CAMERA_DEPENDENCIES	= linux ambagpu
else
AMBA_CAMERA_DEPENDENCIES	= linux
endif

ifeq ($(BR2_PACKAGE_EVA),y)
AMBA_CAMERA_DEPENDENCIES += lua
endif

AMBA_CAMERA_INSTALL_STAGING	= YES
AMBA_CAMERA_INSTALL_IMAGES	= NO
AMBA_CAMERA_INSTALL_TARGET	= YES

AMBA_CAMERA_LICENSE = PROPRIETARY
#AMBA_CAMERA_LICENSE_FILES = COPYING

define AMBA_CAMERA_HELP_CMDS
	@echo '  amba_camera-menuconfig       - Run menuconfig'
	@echo '  amba_camera-savedefconfig    - Run savedefconfig'
	@echo '  amba_camera-update-defconfig - Save the configuration to the path specified'
endef


# Platform vairables:
#   - ARCH=$(KERNEL_ARCH) -- All
#   - CROSS_COMPILE="$(TARGET_CROSS)" -- All
#   - AMBA_CAMERA_DIR="$(AMBA_CAMERA_DIR)" -- All, binary output directory
#   - AMBARELLA_PKG_DIR=$(AMBA_CAMERA_SITE) -- All, source directory
#   - $(TARGET_MAKE_ENV) -- Buildroot
#   - TARGET_CFLAGS="$(TARGET_CFLAGS)" -- Buildroot
#   - TARGET_CXXFLAGS="$(TARGET_CXXFLAGS)" -- Buildroot
#   - TARGET_LDFLAGS="$(TARGET_LDFLAGS)" -- Buildroot
#   - KBUILD_KCONFIG=$(BR2_PACKAGE_AMBA_CAMERA_KCONFIG) -- Amba_camera
#   - KBUILD_AMBA_MKFILE=$(BR2_PACKAGE_AMBA_CAMERA_AMBA_MKFILE) -- Amba_camera
# Linux vairables:
#   - LINUXDIR=$(LINUX_DIR) -- Linux (building) directory
#   - INSTALL_MOD_PATH="$(TARGET_DIR)" -- Directory to install modules
AMBA_CAMERA_MAKE_ENV = \
		$(TARGET_MAKE_ENV) \
		BINARIES_DIR=$(BINARIES_DIR) \
		ARCH=$(KERNEL_ARCH) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		LINUXDIR=$(LINUX_DIR) \
		KBUILD_KCONFIG=$(BR2_PACKAGE_AMBA_CAMERA_KCONFIG) \
		KBUILD_AMBA_MKFILE=$(BR2_PACKAGE_AMBA_CAMERA_AMBA_MKFILE) \
		TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		TARGET_CXXFLAGS="$(TARGET_CXXFLAGS)" \
		TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
		INSTALL_MOD_PATH="$(TARGET_DIR)" \
		AMBA_CAMERA_DIR="$(AMBA_CAMERA_DIR)" \
		AMBARELLA_PKG_DIR=$(AMBA_CAMERA_SITE)


AMBA_CAMERA_KCONFIG_FILE = $(call qstrip,$(BR2_PACKAGE_AMBA_CAMERA_DEFCONFIG))
AMBA_CAMERA_KCONFIG_OPTS = \
		$(AMBA_CAMERA_MAKE_ENV)

###############################################################################
# Out-of-source-build
###############################################################################

ifeq ($(BR2_PACKAGE_AMBA_CAMERA_OOSB),y)
# Used bu kconfig for rsync.
AMBA_CAMERA_KERNEL_OOSB = $(AMBA_CAMERA_SITE) O=
# Used by HOOK_CMDS, because $(@D) will be changed after building steps.
#AMBA_CAMERA_MK_DIR = $(AMBA_CAMERA_SITE) O=$(AMBA_CAMERA_DIR)
AMBA_CAMERA_MK_DIR = $(AMBA_CAMERA_DIR)
else # BR2_PACKAGE_AMBA_CAMERA_OOSB
AMBA_CAMERA_KERNEL_OOSB =
AMBA_CAMERA_MK_DIR = $(AMBA_CAMERA_DIR)
endif # BR2_PACKAGE_AMBA_CAMERA_OOSB

###############################################################################
# Commands of each building step
###############################################################################

#define AMBA_CAMERA_CONFIGURE_CMDS
#	@echo "configure"
#endef # AMBA_CAMERA_CONFIGURE_CMDS

ifeq ($(BR2_PACKAGE_AMBA_CAMERA_DIAGS_LIB),y)
define AMBA_CAMERA_BUILD_CMDS
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		diags_lib
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_all
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_ko_clean
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_ko
endef # AMBA_CAMERA_BUILD_CMDS
else # BR2_PACKAGE_AMBA_CAMERA_DIAGS_LIB
ifeq ($(BR2_PACKAGE_AMBA_CAMERA_DIAGS),y)
define AMBA_CAMERA_BUILD_CMDS
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		diags
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_all
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_ko_clean
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_ko
endef # AMBA_CAMERA_BUILD_CMDS
else # BR2_PACKAGE_AMBA_CAMERA_DIAGS
define AMBA_CAMERA_BUILD_CMDS
	@echo "BR2_PACKAGE_EVA=${BR2_PACKAGE_EVA}"
    $(Q)if [ ! -z $(BR2_PACKAGE_EVA) ] ; then \
        cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/AmbaEvaUserPartition.c  ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/cv3devbub_option_a_v100/AmbaUserPartition.c; \
        cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/AmbaEvaPrint.c  ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/comsvc/print/src/linux/AmbaPrint.c; \
        echo "BR2_PACKAGE_AMBA_CAMERA_BRIDGE_SENSOR=${BR2_PACKAGE_AMBA_CAMERA_BRIDGE_SENSOR}"; \
        if [ $(BR2_PACKAGE_AMBA_CAMERA_BRIDGE_SENSOR) == "y" ] ; then \
            echo ">>>> Copy EVA-only bridge file for SENSOR................................"; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/AmbaSbrg_Max9295_96712_sensor.c ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/bridge/AmbaSbrg_Max9295_96712.c; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/inc/AmbaSbrg_Max9295_96712_sensor.h ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/bridge/inc/AmbaSbrg_Max9295_96712.h; \
        else \
            echo ">>>> Copy EVA-only bridge file for RADAR................................"; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/AmbaSbrg_Max9295_96712_Radar.c ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/bridge/AmbaSbrg_Max9295_96712.c; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/AmbaSbrg_Max9295_96712_Radar.h ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/bridge/inc/AmbaSbrg_Max9295_96712.h; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/AmbaRadar_MAX9295_96712_AWR2243.c ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/yuvinput/AmbaRadar_MAX9295_96712_AWR2243.c; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/AmbaRadar_MAX9295_96712_AWR2243.h ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/yuvinput/inc/AmbaRadar_MAX9295_96712_AWR2243.h; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/AmbaRadar_MAX9295_96712_AWR2243Table.c ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/yuvinput/AmbaRadar_MAX9295_96712_AWR2243Table.c; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/yuvinput_Makefile ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/bsp/cv3x/peripherals/yuvinput/Makefile; \
            cp ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/svc/apps/eva/src/main/peripherals/bridge/yuvinput_Kconfig ${BR2_PACKAGE_AMBA_CAMERA_SOURCE}/build/kconfigs/bsp/cv3x/peripherals/yuvinput/Kconfig; \
        fi; \
    fi;
    $(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_all
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_ko_clean
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_ko
endef # AMBA_CAMERA_BUILD_CMDS
endif # BR2_PACKAGE_AMBA_CAMERA_DIAGS
endif # BR2_PACKAGE_AMBA_CAMERA_DIAGS_LIB

define AMBA_CAMERA_INSTALL_IMAGES_CMDS
	@echo "TODO: install Images"
endef # AMBA_CAMERA_INSTALL_IMAGES_CMDS

define AMBA_CAMERA_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/out/amba_svc $(TARGET_DIR)/usr/bin
endef # AMBA_CAMERA_INSTALL_TARGET_CMDS

define AMBA_CAMERA_INSTALL_STAGING_CMDS
	@echo "TODO: install Staging"
endef # AMBA_CAMERA_INSTALL_STAGING_CMDS

##################
# BuildRoot hooks
##################
AMBA_CAMERA_CV_KCONFIG_OPTS = \
		TERMINFO=/lib/terminfo \
		KBUILD_KCONFIG=$(BR2_PACKAGE_AMBA_CAMERA_KCONFIG) \
		KBUILD_AMBA_MKFILE=$(BR2_PACKAGE_AMBA_CAMERA_AMBA_MKFILE) \
		HOSTCC="$(HOSTCC) $(HOST_CFLAGS) $(HOST_LDFLAGS)" \
		ARCH=$(KERNEL_ARCH) \
		CROSS_COMPILE="$(TARGET_CROSS)"

AMBA_CAMERA_CV_KMODULE_OPTS = \
		ARCH=$(KERNEL_ARCH) \
		LINUXDIR=$(LINUX_DIR) \
		AMBARELLA_PKG_DIR=$(BR2_PACKAGE_AMBA_CAMERA_SOURCE) \
		INSTALL_MOD_PATH=$(TARGET_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)"

AMBA_CAMERA_CV_MAKE_ENV = \
		$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_MAKE_ENV) \
		CFLAGS="$(TARGET_CFLAGS)"

# CV needs it?!
define AMBA_CAMERA_SAVE_BUILD_ENV
	$(Q)echo '$(AMBA_CAMERA_CV_MAKE_ENV) $(AMBA_CAMERA_CV_KCONFIG_OPTS)' > $(@D)/.target_make_env
@#	$(Q)$(TOPDIR)/ambarella/package/amba_camera/mkmakefile "$(KERNEL_MODULE)" "$(@D)" "$(AMBA_CAMERA_KMODULE_OPTS)" "$(AMBA_CAMERA_SITE)"
endef # AMBA_CAMERA_SAVE_BUILD_ENV


define AMBA_CAMERA_GEN_SINGLE_MAKE_SCRIPT
	$(Q)rm -f $(@D)/mk_amba_mod.sh
	$(Q)(echo -e "#!/bin/bash\n\
$(subst ",\",$(AMBA_CAMERA_MAKE_ENV)) \
$(subst ",\",$(MAKE)) -C \
$(subst ",\",$(AMBA_CAMERA_MK_DIR)) \x24*" > $(@D)/mk_amba_mod.sh)
	$(Q)chmod +x $(@D)/mk_amba_mod.sh
endef # MBA_CAMERA_GEN_SINGLE_MAKE_SCRIPT

define AMBA_CAMERA_POST_LEGAL_INFO_HOOKS_CMD
	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef # AMBA_CAMERA_POST_LEGAL_INFO_HOOKS_CMD

AMBA_CAMERA_POST_RSYNC_HOOKS += AMBA_CAMERA_SAVE_BUILD_ENV
AMBA_CAMERA_POST_RSYNC_HOOKS += AMBA_CAMERA_GEN_SINGLE_MAKE_SCRIPT
AMBA_CAMERA_POST_LEGAL_INFO_HOOKS += AMBA_CAMERA_POST_LEGAL_INFO_HOOKS_CMD

###############################################################################
# Staging install
###############################################################################
define AMBA_CAMERA_INSTALL_STAGING_CMDS
	rm -rf $(STAGING_DIR)/usr/include/amba_camera
	mkdir -p $(STAGING_DIR)/usr/include/amba_camera
	cp -a $(AMBA_CAMERA_SITE)/vendors/ambarella/inc/* $(STAGING_DIR)/usr/include/amba_camera/
	for f in `find $(AMBA_CAMERA_MK_DIR)/lib/ -type f | grep -v 'libc.a\|libm.a\|libg.a'`; do \
		$(INSTALL) -D -m 0755 $$f $(STAGING_DIR)/usr/lib/; \
		done
endef


###############################################################################
# using Kbuild/Kconfig
###############################################################################
$(eval $(kconfig-package))


###############################################################################
# POST_HOOKS of FS
###############################################################################

define AMBA_CAMERA_FWPROG
	$(Q)echo ">>>   Gen FwProg............................................"
	$(Q)$(AMBA_CAMERA_MAKE_ENV) \
		$(MAKE) \
		-C $(AMBA_CAMERA_MK_DIR) \
		amba_fwprog
	$(Q)rm -rf $(BINARIES_DIR)/out
	$(Q)cp -a  $(AMBA_CAMERA_MK_DIR)/out $(BINARIES_DIR)/
endef # AMBA_CAMERA_FWPROG

.PHONY: amba_camera-fwprog
amba_camera-fwprog:
	$(AMBA_CAMERA_FWPROG)

# Hook amba_camera-fwprog, all -> world -> target-post-image -> rootfs-N
amba_camera-fwprog: target-post-image
world: amba_camera-fwprog


endif # ($(BR2_PACKAGE_AMBA_CAMERA),y)
