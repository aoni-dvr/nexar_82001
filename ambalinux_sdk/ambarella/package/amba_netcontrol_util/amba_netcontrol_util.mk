#############################################################
#
# amba_netcontrol_util
#
#############################################################

pkg			= AMBA_NETCONTROL_UTIL

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/amba_netcontrol_util
$(pkg)_SOURCE		= amba_netcontrol_util-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambaipc amba_util openssl file_pkg_encap
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

ifeq ($(BR2_arm),y)
ARMBADFLAGS = -DUSE_ARM
endif

define AMBA_NETCONTROL_UTIL_INSTALL_STAGING_CMDS
  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_install
endef

define AMBA_NETCONTROL_UTIL_INSTALL_TARGET_CMDS
  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBA_NETCONTROL_UTIL_BUILD_CMDS
  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define AMBA_NETCONTROL_UTIL_CLEAN_CMDS
  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define AMBA_NETCONTROL_UTIL_UNINSTALL_STAGING_CMDS
  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define AMBA_NETCONTROL_UTIL_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
AMBA_NETCONTROL_UTIL_POST_LEGAL_INFO_HOOKS += AMBA_NETCONTROL_UTIL_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
