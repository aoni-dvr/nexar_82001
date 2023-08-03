#############################################################
#
# AMBA_TVM: Ambarella TVM hardware abstraction layer
#
#############################################################

pkg			= AMBA_TVM

$(pkg)_VERSION		= 0.0.1
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/amba_tvm
$(pkg)_SOURCE		= amba_tvm-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	=
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Apache
$(pkg)_LICENSE_FILES	= apache-2.0.txt


TARGET_CONFIGURE_OPTS_TVM = $(TARGET_CONFIGURE_OPTS) AMBA_DLR_ROOT_DIR=$(BR2_PACKAGE_AMBA_DLR_ROOT_DIR)

define AMBA_TVM_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS_TVM) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) install_staging
endef

define AMBA_TVM_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS_TVM) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBA_TVM_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS_TVM) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define AMBA_TVM_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS_TVM) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
  $(TARGET_CONFIGURE_OPTS_TVM) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) uninstall
endef

define AMBA_TVM_UNINSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS_TVM) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) uninstall
endef

define AMBA_TVM_POST_LEGAL_INFO_HOOKS_CMD
	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
endef
AMBA_TVM_POST_LEGAL_INFO_HOOKS += AMBA_TVM_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
