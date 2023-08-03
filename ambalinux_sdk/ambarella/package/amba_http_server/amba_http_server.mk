#############################################################
#
# httpd_server
#
#############################################################

pkg			= AMBA_HTTP_SERVER

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE             = $(AMBARELLA_PKG_DIR)/amba_http_server
$(pkg)_SOURCE		= amba_http_server-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES = 
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

#BuildRoot macro
define AMBA_HTTP_SERVER_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBA_HTTP_SERVER_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define AMBA_HTTP_SERVER_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

$(eval $(generic-package))
