#############################################################
#
# ambavx
#
#############################################################

pkg			= AMBAVX

$(pkg)_VERSION		= 0.9
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/ambavx/openvx_tests/buildroot/pkg/ambavx
$(pkg)_SOURCE		= ambavx-$($(pkg)_VERSION).tar.gz
$(pkg)_DEPENDENCIES	= ambacv
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= Ambarella
$(pkg)_LICENSE_FILES	= License_Ambarella.txt

AMBAVX_MAKE_ENV = $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV)
AMBAVX_MAKE_ENV += WORKDIR=$(AMBARELLA_PKG_DIR)
AMBAVX_MAKE_ENV += AMBACV=$(AMBARELLA_PKG_DIR)/ambacv
AMBAVX_MAKE_ENV += AMBALINK=$(AMBARELLA_PKG_DIR)/..
AMBAVX_MAKE_ENV += AMBALINK_OUTPUT=$(O)


#$(info !! AMBAVX_MAKE_ENV=$(AMBAVX_MAKE_ENV))

define AMBAVX_INSTALL_TARGET_CMDS
	echo "!! ambavx.mk: AMBAVX_INSTALL_TARGET_CMDS in $(@D)"
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild install_target CROSS_COMPILE=1
endef

define AMBAVX_INSTALL_STAGING_CMDS
	echo "!! ambavx.mk: AMBAVX_INSTALL_STAGING_CMDS in $(@D)"
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild install_staging create_ambavx_diags CROSS_COMPILE=1
endef

define AMBAVX_BUILD_CMDS
	echo "!! ambavx.mk: AMBAVX_BUILD_CMDS in $(@D)"
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild lib              CROSS_COMPILE=0
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild install_prebuild CROSS_COMPILE=0
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild lib              CROSS_COMPILE=1
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild install_prebuild CROSS_COMPILE=1
endef

define AMBAVX_CLEAN_CMDS
	echo "!! ambavx.mk: AMBAVX_CLEAN_CMDS in $(@D)"
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild clean CROSS_COMPILE=0
	$(AMBAVX_MAKE_ENV) make -C $(@D) -f Makefile.prebuild clean CROSS_COMPILE=1
endef

define AMBAVX_POST_LEGAL_INFO_HOOKS_CMD
	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
endef
AMBAVX_POST_LEGAL_INFO_HOOKS += AMBAVX_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
