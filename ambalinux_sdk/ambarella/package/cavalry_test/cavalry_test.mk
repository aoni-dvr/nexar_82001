#############################################################
#
# cavalry_test
#
#############################################################

pkg			= CAVALRY_TEST

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/cavalry_test
$(pkg)_SOURCE		= cavalry_test-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= cavalry_lib
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= MIT
$(pkg)_LICENSE_FILES	= License_MIT.txt

#Prepre command line for sub packages
ifeq ($(BR2_PACKAGE_CAVALRY_TEST),y)
  $(pkg)_INSTALL_STAGING = YES
  INSTALL_CAVALRY_TEST = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_CAVALRY_TEST = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
  CLEAN_CAVALRY_TEST = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  INSTALL_STAGING_CAVALRY_TEST = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) install
  UNINSTALL_STAGING_CAVALRY_TEST = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) uninstall
else
  INSTALL_CAVALRY_TEST =
  BUILD_CAVALRY_TEST =
  CLEAN_CAVALRY_TEST =
  INSTALL_STAGING_CAVALRY_TEST =
  UNINSTALL_STAGING_CAVALRY_TEST =
endif

#BuildRoot macro
define CAVALRY_TEST_INSTALL_TARGET_CMDS
  $(INSTALL_CAVALRY_TEST)
endef

define CAVALRY_TEST_BUILD_CMDS 
  $(BUILD_CAVALRY_TEST)
endef

define CAVALRY_TEST_CLEAN_CMDS
  $(CLEAN_CAVALRY_TEST)
endef

define CAVALRY_TEST_INSTALL_STAGING_CMDS 
  $(INSTALL_STAGING_CAVALRY_TEST)
endef

define CAVALRY_TEST_UNINSTALL_STAGING_CMDS 
  $(UNINSTALL_STAGING_CAVALRY_TEST)
endef

define CAVALRY_TEST_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
CAVALRY_TEST_POST_LEGAL_INFO_HOOKS += CAVALRY_TEST_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
