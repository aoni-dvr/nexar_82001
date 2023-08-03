#############################################################
#
# UsageEnvironment
#
#############################################################

pkg			= GROUPSOCK

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/supercam/RTSP/groupsock
$(pkg)_SOURCE		=
$(pkg)_DEPENDENCIES = 
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= NO

define GROUPSOCK_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(STAGING_DIR) install
endef

define GRUOPSOCK_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define GROUPSOCK_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

$(eval $(generic-package))
