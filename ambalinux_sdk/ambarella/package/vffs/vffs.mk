#############################################################
#
# vffs
#
#############################################################

pkg			= VFFS

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/vffs
$(pkg)_SOURCE		= vffs-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= libfuse
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Dual MIT/GPLv2
$(pkg)_LICENSE_FILES	= License_GPLv2.txt License_MIT.txt

define VFFS_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define VFFS_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define VFFS_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

define VFFS_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
VFFS_POST_LEGAL_INFO_HOOKS += VFFS_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
