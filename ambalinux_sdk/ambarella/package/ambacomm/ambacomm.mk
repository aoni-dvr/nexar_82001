#############################################################
#
# UsageEnvironment
#
#############################################################

pkg                     = AMBACOMM

$(pkg)_VERSION          = 1.0
$(pkg)_SITE_METHOD      = local
$(pkg)_SITE             = $(AMBARELLA_PKG_DIR)/ambacomm
$(pkg)_SOURCE           =
$(pkg)_DEPENDENCIES     =
$(pkg)_INSTALL_STAGING  = YES
$(pkg)_INSTALL_IMAGES   = NO
$(pkg)_INSTALL_TARGET   = YES
CMAKE_EXE_LINKER_FLAGS  = -lm

$(pkg)_CONF_OPTS =  -DBUILD_TESTING=ON -DENABLE_DOCS=OFF -DENABLE_SDK=ON

ifeq ($(BR2_PACKAGE_AMBACOMM_EXAMPLES), y)
 $(pkg)_CONF_OPTS += -DENABLE_EXAMPLES=ON
else
 $(pkg)_CONF_OPTS += -DENABLE_EXAMPLES=OFF
endif

ifeq ($(BR2_PACKAGE_AMBACOMM_TOOLS), y)
 $(pkg)_CONF_OPTS += -DENABLE_TOOLS=ON
else
 $(pkg)_CONF_OPTS += -DENABLE_TOOLS=OFF
endif

ifeq ($(BR2_PACKAGE_AMBACOMM_TESTS), y)
 $(pkg)_CONF_OPTS += -DENABLE_TESTS=ON
else
 $(pkg)_CONF_OPTS += -DENABLE_TESTS=OFF
endif

$(eval $(cmake-package))
