#############################################################
#
# superdrone
#
#############################################################

pkg			= SUPERDRONE

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/superdrone
$(pkg)_SOURCE		= superdrone-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambaipc amba_util
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= MIT
$(pkg)_LICENSE_FILES	= License_MIT.txt

#Prepre command line for sub packages
ifeq ($(BR2_PACKAGE_SUPERDRONE_STATUSUPDATE),y)
  INSTALL_SUPERDRONE_STATUSUPDATE = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_StatusUpdate -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_SUPERDRONE_STATUSUPDATE =  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_StatusUpdate -f br.mk
  CLEAN_SUPERDRONE_STATUSUPDATE = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_StatusUpdate -f br.mk clean
else
  INSTALL_SUPERDRONE_STATUSUPDATE =
  BUILD_SUPERDRONE_STATUSUPDATE =
  CLEAN_SUPERDRONE_STATUSUPDATE =
endif

#BuildRoot macro
define SUPERDRONE_INSTALL_TARGET_CMDS
  $(INSTALL_SUPERDRONE_STATUSUPDATE)
endef

define SUPERDRONE_BUILD_CMDS
  $(BUILD_SUPERDRONE_STATUSUPDATE)
endef

define SUPERDRONE_CLEAN_CMDS
  $(CLEAN_SUPERDRONE_STATUSUPDATE)
endef

define SUPERDRONE_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
SUPERDRONE_POST_LEGAL_INFO_HOOKS += SUPERDRONE_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
