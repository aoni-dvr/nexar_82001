#############################################################
#
# quectel-gnss-fota
#
#############################################################

pkg			= QUECTEL_GNSS_FOTA

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE             = $(AMBARELLA_PKG_DIR)/quectel-gnss-fota
$(pkg)_SOURCE		= 
$(pkg)_DEPENDENCIES     = ambaipc amba_examples
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

#BuildRoot macro
define QUECTEL_GNSS_FOTA_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define QUECTEL_GNSS_FOTA_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define QUECTEL_GNSS_FOTA_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

$(eval $(generic-package))
