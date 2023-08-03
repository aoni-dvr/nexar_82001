#############################################################
#
# youtube_livestream
#
#############################################################

pkg			= YOUTUBE_LIVESTREAM

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/youtube_livestream
$(pkg)_SOURCE		= youtube_livestream-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES = openssl ca-certificates json-c libcurl ffmpeg libev ambaipc libnetfifo
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	=

#BuildRoot macro
define YOUTUBE_LIVESTREAM_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk DESTDIR=$(TARGET_DIR) install
endef

define YOUTUBE_LIVESTREAM_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk
endef

define YOUTUBE_LIVESTREAM_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) -f br.mk clean
endef

define YOUTUBE_LIVESTREAM_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
YOUTUBE_LIVESTREAM_POST_LEGAL_INFO_HOOKS += YOUTUBE_LIVESTREAM_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
