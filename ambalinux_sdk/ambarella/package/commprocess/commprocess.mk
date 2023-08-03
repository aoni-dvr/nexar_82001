#############################################################
#
# UsageEnvironment
#
#############################################################

pkg                     = COMMPROCESS

$(pkg)_VERSION          =
$(pkg)_SITE_METHOD      = local
$(pkg)_SITE             = $(AMBARELLA_PKG_DIR)/commprocess
$(pkg)_SOURCE           =
$(pkg)_DEPENDENCIES     = ambacv ambacomm
$(pkg)_INSTALL_STAGING  = NO
$(pkg)_INSTALL_IMAGES   = NO
$(pkg)_INSTALL_TARGET   = YES
$(pkg)_CONF_OPTS        = -DBACE=ON
CMAKE_EXE_LINKER_FLAGS  =

$(eval $(cmake-package))

