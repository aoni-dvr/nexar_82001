#############################################################
#
# ipcdef
#
#############################################################

pkg			= IPCDEF

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/ipcdef
$(pkg)_SOURCE		= ipcdef-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	=
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define COPY_IPCDEF_HEADER
  rsync -au $(TOPDIR)/../linux-$(BR2_AMBARELLA_LINUX_VERSION)/include/linux/AmbaIPC_Rpc_Def.h $(@D)/
endef
IPCDEF_POST_RSYNC_HOOKS += COPY_IPCDEF_HEADER

define IPCDEF_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) install_staging
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f priv_br.mk DESTDIR=$(STAGING_DIR) install_staging
endef

define IPCDEF_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f priv_br.mk DESTDIR=$(TARGET_DIR) install
endef

define IPCDEF_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_EVN) $(MAKE) -C $(@D) -f br.mk
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_EVN) $(MAKE) -C $(@D) -f priv_br.mk
endef

define IPCDEF_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f priv_br.mk clean
endef

#define IPCDEF_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#IPCDEF_POST_LEGAL_INFO_HOOKS += IPCDEF_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
