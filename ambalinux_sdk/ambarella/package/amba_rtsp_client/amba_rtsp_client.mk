#############################################################
#
# amba_rtsp_client
#
#############################################################

pkg			= AMBA_RTSP_CLIENT

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/amba_rtsp_client
$(pkg)_SOURCE		= amba_rtsp_client-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= libev ambaipc libnetfifo ffmpeg
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

define AMBA_RTSP_CLIENT_INSTALL_TARGET_CMDS
    $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBA_RTSP_CLIENT_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define AMBA_RTSP_CLIENT_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef


define AMBA_RTSP_CLIENT_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
AMBA_RTSP_CLIENT_POST_LEGAL_INFO_HOOKS += AMBA_RTSP_CLIENT_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
