#############################################################
#
# controlbox
#
#############################################################

pkg			= CONTROLBOX

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/controlbox
$(pkg)_SOURCE		= controlbox-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambaipc amba_util
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt


define CONTROLBOX_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk DESTDIR=$(STAGING_DIR) lib_install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_install
endef

define CONTROLBOX_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

ifeq ($(BR2_PACKAGE_CONTROLBOX_S),y)
define CONTROLBOX_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk BR2_PACKAGE_CONTROLBOX_S=y
endef
else
define CONTROLBOX_BUILD_CMDS
  echo "ycyang ycyang ycyang ycyang"
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk DESTDIR=$(STAGING_DIR) lib_install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef
endif

define CONTROLBOX_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define CONTROLBOX_UNINSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/common -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
endef

define CONTROLBOX_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(pkg)_LICENSE_FILES),$(call legal-license-file,$$($(pkg)_RAWNAME),$(F),$$($(pkg)_DIR)/$(F))$$(sep))
endef
CONTROLBOX_POST_LEGAL_INFO_HOOKS += CONTROLBOX_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
