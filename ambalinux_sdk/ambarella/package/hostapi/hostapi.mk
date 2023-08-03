#############################################################
#
# AMBACV: ambarella cv sub-scheduler
#
#############################################################

pkg			= HOSTAPI

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/hostapi
$(pkg)_SOURCE		= ambacv-$($(pkg)_VERSION).tar.gz
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt


define HOSTAPI_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/target -f linux.mk DESTDIR=$(TARGET_DIR) install
endef

define HOSTAPI_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/target -f linux.mk all
endef

define HOSTAPI_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/target -f linux.mk clean
endef


#define HOSTAPI_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#HOSTAPI_POST_LEGAL_INFO_HOOKS += HOSTAPI_POST_LEGAL_INFO_HOOKS_CMD


$(eval $(generic-package))
