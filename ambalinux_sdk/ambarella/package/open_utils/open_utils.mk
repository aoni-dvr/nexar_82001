#############################################################
#
# open_utils
#
#############################################################

pkg			= OPEN_UTILS

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/open_utils
$(pkg)_SOURCE		= open_utils-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	=
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= GPLv2/MIT
$(pkg)_LICENSE_FILES	= License_fsstress_GPLv2.txt License_inotify_GPLv2.txt License_inotify_MIT.txt

ifeq ($(BR2_PACKAGE_OPEN_UTIL_FSSTRESS),y)
  INSTALL_OPEN_UTIL_FSSTRESS = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/fsstress -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_OPEN_UTIL_FSSTRESS = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/fsstress -f br.mk
  CLEAN_OPEN_UTIL_FSSTRESS = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/fsstress -f br.mk clean
else
  INSTALL_OPEN_UTIL_FSSTRESS =
  BUILD_OPEN_UTIL_FSSTRESS =
  CLEAN_OPEN_UTIL_FSSTRESS =
endif

ifeq ($(BR2_PACKAGE_OPEN_UTIL_INOTIFY),y)
  INSTALL_OPEN_UTIL_INOTIFY = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/inotify -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_OPEN_UTIL_INOTIFY = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/inotify -f br.mk
  CLEAN_OPEN_UTIL_INOTIFY = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/inotify -f br.mk clean
else
  INSTALL_OPEN_UTIL_INOTIFY =
  BUILD_OPEN_UTIL_INOTIFY =
  CLEAN_OPEN_UTIL_INOTIFY =
endif

define OPEN_UTILS_INSTALL_TARGET_CMDS
  $(INSTALL_OPEN_UTIL_FSSTRESS)
  $(INSTALL_OPEN_UTIL_INOTIFY)
endef

define OPEN_UTILS_BUILD_CMDS
  $(BUILD_OPEN_UTIL_FSSTRESS)
  $(BUILD_OPEN_UTIL_INOTIFY)
endef

define OPEN_UTILS_CLEAN_CMDS
  $(CLEAN_OPEN_UTIL_FSSTRESS)
  $(CLEAN_OPEN_UTIL_INOTIFY)
endef

define OPEN_UTILS_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
OPEN_UTILS_POST_LEGAL_INFO_HOOKS += OPEN_UTILS_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
