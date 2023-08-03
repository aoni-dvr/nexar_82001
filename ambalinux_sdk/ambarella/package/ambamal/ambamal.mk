#############################################################
#
# ambamal
#
#############################################################

pkg			= AMBAMAL

$(pkg)_VERSION			= 1.0
$(pkg)_SITE_METHOD		= local
$(pkg)_SITE				= $(AMBARELLA_PKG_DIR)/ambamal
$(pkg)_SOURCE			= ambamal-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES		=
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE			= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

AMBAMAL_TARGET_CONFIGURE_OPTS = $(TARGET_CONFIGURE_OPTS) BR2_PACKAGE_BUILD_AMBAMAL_SHARED_LIB=$(BR2_PACKAGE_BUILD_AMBAMAL_SHARED_LIB)

define AMBAMAL_INSTALL_STAGING_CMDS
	$(AMBAMAL_TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f linux/Makefile DESTDIR=$(STAGING_DIR) install_staging
endef

define AMBAMAL_INSTALL_TARGET_CMDS
	$(AMBAMAL_TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f linux/Makefile DESTDIR=$(TARGET_DIR) install
endef

define AMBAMAL_BUILD_CMDS
	$(AMBAMAL_TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f linux/Makefile
endef

define AMBAMAL_CLEAN_CMDS
	$(AMBAMAL_TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f linux/Makefile clean
endef

define AMBAMAL_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
AMBAMAL_POST_LEGAL_INFO_HOOKS += AMBAMAL_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
