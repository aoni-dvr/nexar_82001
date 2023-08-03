#############################################################
#
# libeventnotifier
#
#############################################################

pkg			= LIBEVENTNOTIFIER

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/libeventnotifier
$(pkg)_SOURCE		= libeventnotifier-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambaipc amba_util
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define LIBEVENTNOTIFIER_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_install
endef

define LIBEVENTNOTIFIER_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define LIBEVENTNOTIFIER_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define LIBEVENTNOTIFIER_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define LIBEVENTNOTIFIER_UNINSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define LIBEVENTNOTIFIER_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
LIBEVENTNOTIFIER_POST_LEGAL_INFO_HOOKS += LIBEVENTNOTIFIER_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
