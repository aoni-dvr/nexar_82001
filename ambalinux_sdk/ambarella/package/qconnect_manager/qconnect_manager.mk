#############################################################
#
# qconnect_manager
#
#############################################################

pkg			= QCONNECT_MANAGER

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE             = $(AMBARELLA_PKG_DIR)/quectel-CM
$(pkg)_SOURCE		= 
$(pkg)_DEPENDENCIES = 
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

#BuildRoot macro
define QCONNECT_MANAGER_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define QCONNECT_MANAGER_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define QCONNECT_MANAGER_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

$(eval $(generic-package))
