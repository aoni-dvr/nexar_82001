#############################################################
#
# ambaipc_util
#
#############################################################

pkg			= AMBAIPC_UTIL

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/ambaipc_util
$(pkg)_SOURCE		= ambaipc_util-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambaipc
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define COPY_AMBAIPC_UTIL_HEADER
	rsync -au $(TOPDIR)/../linux-$(BR2_AMBARELLA_LINUX_VERSION)/include/linux/AmbaIPC_Rpc_Def.h $(@D)/
endef
AMBAIPC_POST_RSYNC_HOOKS += COPY_AMBAIPC_UTIL_HEADER

define AMBAIPC_UTIL_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBAIPC_UTIL_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk DESTDIR=$(STAGING_DIR) install_staging
endef

define AMBAIPC_UITL_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk
endef

define AMBAIPC_UTIL_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk clean
endef

#define AMBAIPC_UTIL_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#AMBAIPC_UTIL_POST_LEGAL_INFO_HOOKS += AMBAIPC_UTIL_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
