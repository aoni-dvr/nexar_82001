#############################################################
#
# ambaipc
#
#############################################################

pkg			= AMBAIPC

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/ambaipc
$(pkg)_SOURCE		= ambaipc-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ipcdef
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define COPY_AIPC_MSG_HEADER
  rsync -au $(TOPDIR)/../linux-$(BR2_AMBARELLA_LINUX_VERSION)/include/linux/aipc_msg.h $(@D)/
endef
AMBAIPC_POST_RSYNC_HOOKS += COPY_AIPC_MSG_HEADER

define COPY_AMBAIPC_HEADER
  rsync -au $(TOPDIR)/../linux-$(BR2_AMBARELLA_LINUX_VERSION)/include/linux/AmbaIPC_Rpc_Def.h $(@D)/
endef
AMBAIPC_POST_RSYNC_HOOKS += COPY_AMBAIPC_HEADER

define AMBAIPC_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) install_staging
endef

define AMBAIPC_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBAIPC_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define AMBAIPC_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef


#define AMBAIPC_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#AMBAIPC_POST_LEGAL_INFO_HOOKS += AMBAIPC_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
