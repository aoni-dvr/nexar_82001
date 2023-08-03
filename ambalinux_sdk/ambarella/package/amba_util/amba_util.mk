#############################################################
#
# amba_util
#
#############################################################

pkg			= AMBA_UTIL

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/amba_util
$(pkg)_SOURCE		= amba_util-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	=
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define AMBA_UTIL_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libambamem -f br.mk DESTDIR=$(STAGING_DIR) lib_install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libsocksvc -f br.mk DESTDIR=$(STAGING_DIR) lib_install
endef

define AMBA_UTIL_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libambamem -f br.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libsocksvc -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBA_UTIL_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libambamem -f br.mk
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libsocksvc -f br.mk
endef

define AMBA_UTIL_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libambamem -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libambamem -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libsocksvc -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libsocksvc -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define AMBA_UTIL_UNINSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libambamem -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/libsocksvc -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

#define AMBA_UTIL_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#AMBA_UTIL_POST_LEGAL_INFO_HOOKS += AMBA_UTIL_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
