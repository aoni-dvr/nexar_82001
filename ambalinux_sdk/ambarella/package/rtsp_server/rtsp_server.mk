#############################################################
#
# rtsp_server
#
#############################################################

pkg			= RTSP_SERVER

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/supercam/RTSP/rtsp_server
$(pkg)_SOURCE		=
$(pkg)_DEPENDENCIES = usage_environment basic_usage_environment groupsock live_media supercam
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES

define RTSP_SERVER_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define RTSP_SERVER_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define RTSP_SERVER_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

$(eval $(generic-package))
