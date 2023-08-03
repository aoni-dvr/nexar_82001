#############################################################
#
# file_pkg_encap
#
#############################################################

pkg			= FILE_PKG_ENCAP

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/file_pkg_encap
$(pkg)_SOURCE		= file_pkg_encap-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	=
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define FILE_PKG_ENCAP_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_install
endef

define FILE_PKG_ENCAP_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define FILE_PKG_ENCAP_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define FILE_PKG_ENCAP_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
FILE_PKG_ENCAP_POST_LEGAL_INFO_HOOKS += FILE_PKG_ENCAP_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
