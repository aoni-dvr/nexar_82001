#############################################################
#
# ambaipc_test
#
#############################################################

pkg			= AMBAIPC_TEST

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/ambaipc_test
$(pkg)_SOURCE		= ambaipc_test-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambaipc ambaipc_util
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Dual MIT/GPLv2
$(pkg)_LICENSE_FILES	= License_GPLv2.txt License_MIT.txt

define COPY_AMBAIPC_TEST_HEADER
	rsync -au $(TOPDIR)/../linux-$(BR2_AMBARELLA_LINUX_VERSION)/include/linux/AmbaIPC_Rpc_Def.h $(@D)/
endef
AMBAIPC_POST_RSYNC_HOOKS += COPY_AMBAIPC_TEST_HEADER

define AMBAIPC_TEST_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBAIPC_TEST_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk
endef

define AMBAIPC_TEST_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/ambafs_notify -f br.mk clean
endef

#define AMBAIPC_TEST_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#AMBAIPC_TEST_POST_LEGAL_INFO_HOOKS += AMBAIPC_TEST_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
