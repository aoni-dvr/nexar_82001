#
# The Buildroot user manual:
#   https://buildroot.org/downloads/manual/manual.html#writing-rules-mk
#

################################################################################
#
# ambdram_statis
#
################################################################################

ifneq ($(BR2_PACKAGE_AMBA_DRAM_STATIS),)
AMBA_DRAM_STATIS_VERSION		= amba
# Usinge local files.
AMBA_DRAM_STATIS_SITE_METHOD	= local
# Relative path to ambarella/
AMBA_DRAM_STATIS_SITE		= ../ambarella/package/amba_dram_statis
AMBA_DRAM_STATIS_DEPENDENCIES	= linux
# If it will be referred by other packages, cf. https://buildroot.org/downloads/manual/manual.html#_infrastructure_for_packages_with_specific_build_systems
AMBA_DRAM_STATIS_INSTALL_STAGING	= NO
AMBA_DRAM_STATIS_INSTALL_TARGET	= YES

AMBA_DRAM_STATIS_LICENSE		= GPL v2
AMBA_DRAM_STATIS_LICENSE_FILES	= LICENSE


AMBA_DRAM_STATIS_DEBUG		= NO


AMBA_DRAM_STATIS_MKOPTION_APP	=
AMBA_DRAM_STATIS_MKOPTION_MOD	= CROSS_COMPILE=$(TARGET_CROSS) \
			  LINUXDIR=$(LINUX_DIR) \
			  LINUXVER=$(shell cat $(LINUX_DIR)/include/config/kernel.release) \
			  ARCH=arm64 \
			  AMBARELLA_ARCH_VAR=$(AMBARELLA_ARCH) \
			  BR2_PACKAGE_AMBA_DRAM_STATIS_VAR=$(BR2_PACKAGE_AMBA_DRAM_STATIS) \
			  AMBA_DRAM_STATIS_DRV_VAR=$(AMBA_DRAM_STATIS_DRV)

define AMBA_DRAM_STATIS_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_MAKE_ENV) \
		$(AMBA_DRAM_STATIS_MKOPTION_MOD) \
		$(MAKE) -C $(@D)/mod modules
endef

define AMBA_DRAM_STATIS_INSTALL_TARGET_CMDS
	$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_MAKE_ENV) \
		$(AMBA_DRAM_STATIS_MKOPTION_MOD) \
		INSTALL_MOD_PATH=$(TARGET_DIR) \
		$(MAKE) -C $(@D)/mod modules_install
endef

#
# Notes:
#   Buildroot had dropped uninstall and clean CMDS.
#

# Depends on your building system
$(eval $(generic-package))

endif	## ifneq ($(BR2_PACKAGE_AMBA_DRAM_STATIS),)

