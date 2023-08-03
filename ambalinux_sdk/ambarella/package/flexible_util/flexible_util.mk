#
# The Buildroot user manual:
#   https://buildroot.org/downloads/manual/manual.html#writing-rules-mk
#

################################################################################
#
# flexible_util
#
################################################################################

FLEXIBLE_UTIL_VERSION		= amba
# Usinge local files.
FLEXIBLE_UTIL_SITE_METHOD	= local
# Relative path to ambarella/
FLEXIBLE_UTIL_SITE		= ../ambarella/package/flexible_util
FLEXIBLE_UTIL_DEPENDENCIES	= linux
# If it will be referred by other packages, cf. https://buildroot.org/downloads/manual/manual.html#_infrastructure_for_packages_with_specific_build_systems
FLEXIBLE_UTIL_INSTALL_STAGING	= YES
FLEXIBLE_UTIL_INSTALL_TARGET	= YES

FLEXIBLE_UTIL_LICENSE		= MIT
FLEXIBLE_UTIL_LICENSE_FILES	= LICENSE


FLEXIBLE_UTIL_DEBUG		= NO


FLEXIBLE_UTIL_MKOPTION_APP	=
FLEXIBLE_UTIL_MKOPTION_MOD	= CROSS_COMPILE=$(TARGET_CROSS) \
			  LINUXDIR=$(LINUX_DIR) \
			  LINUXVER=$(shell cat $(LINUX_DIR)/include/config/kernel.release) \
			  ARCH=arm64

define FLEXIBLE_UTIL_BUILD_CMDS
	$(Q)$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_MAKE_ENV) \
		$(FLEXIBLE_UTIL_MKOPTION_APP) \
		$(MAKE) -C $(@D)/app
	$(Q)$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_MAKE_ENV) \
		$(FLEXIBLE_UTIL_MKOPTION_MOD) \
		$(MAKE) -C $(@D)/mod
endef

define FLEXIBLE_UTIL_INSTALL_TARGET_CMDS
	$(Q)$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_MAKE_ENV) \
	$(FLEXIBLE_UTIL_MKOPTION_MOD) \
	INSTALL_MOD_PATH=$(TARGET_DIR) \
	$(MAKE) -C $(@D)/mod modules_install

endef

#
# Notes:
#   Buildroot had dropped uninstall and clean CMDS.
#

# Depends on your building system
$(eval $(generic-package))

