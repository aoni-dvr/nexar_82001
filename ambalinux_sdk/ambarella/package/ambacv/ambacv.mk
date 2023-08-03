#############################################################
#
# AMBACV: ambarella cv sub-scheduler
#
#############################################################

pkg			= AMBACV

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/ambacv
$(pkg)_SOURCE		= ambacv-$($(pkg)_VERSION).tar.gz
$(pkg)_INSTALL_STAGING	= YES
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
#$(pkg)_LICENSE		= Ambarella
#$(pkg)_LICENSE_FILES	= cv_common/License_Ambarella.txt

ifeq ($(BR2_PACKAGE_BUILD_AMBACV_DIAGS_OOSB),y)
$(pkg)_OVERRIDE_SRCDIR_RSYNC_EXCLUSIONS = -K --include .git
else
$(pkg)_OVERRIDE_SRCDIR_RSYNC_EXCLUSIONS = -K --include .git --exclude tests
endif

ifeq ($(BR2_PACKAGE_BOOST),y)
$(pkg)_DEPENDENCIES     += boost
endif

ifeq ($(BR2_PACKAGE_EIGEN),y)
$(pkg)_DEPENDENCIES     += eigen
endif

ifeq ($(BR2_PACKAGE_AMBAMAL),y)
$(pkg)_DEPENDENCIES	+= ambamal
endif

ifeq ($(BR2_PACKAGE_CAMCTRL_IPC),y)
$(pkg)_DEPENDENCIES     += ambaipc amba_examples
endif

ifeq ($(BR2_PACKAGE_AMBA_FLEXIDAGIO_EXAMPLE),y)
$(pkg)_DEPENDENCIES     += amba_examples
endif


ifeq ($(BR2_PACKAGE_RAPIDJSON),y)
$(pkg)_DEPENDENCIES     += rapidjson
endif

ifndef BR2_PACKAGE_EVA
ifneq ("$(wildcard $(AMBARELLA_PKG_DIR)/ambacv/orc_framework/orcvp.mk)","")
  BUILD_ORCVP = yes
ifeq ($(PROJECT), cv2fs)
  BUILD_ORCVP_AR = yes
else ifeq ($(PROJECT), cv22fs)
  BUILD_ORCVP_AR = yes
else ifeq ($(PROJECT), cv2a)
  BUILD_ORCVP_AR = yes
endif
endif
endif

ifndef BR2_PACKAGE_BUILD_AMBACV_SHARED_LIB
BR2_PACKAGE_BUILD_AMBACV_SHARED_LIB = n
endif

ifndef BR2_PACKAGE_BUILD_AMBACV_DIAGS_SHARED_LIB
BR2_PACKAGE_BUILD_AMBACV_DIAGS_SHARED_LIB = n
endif

ifndef BR2_PACKAGE_AMBAIPC
BR2_PACKAGE_AMBAIPC = n
endif

ifndef BR2_PACKAGE_AMBAMAL
BR2_PACKAGE_AMBAMAL = n
endif

TARGET_CONFIGURE_OPTS_EXT1 = $(TARGET_CONFIGURE_OPTS) BUILD_AMBACV_SHARED_LIB=$(BR2_PACKAGE_BUILD_AMBACV_SHARED_LIB) PACKAGE_AMBAIPC=$(BR2_PACKAGE_AMBAIPC) AMBA_DLR_ROOT_DIR=$(BR2_PACKAGE_AMBA_DLR_ROOT_DIR) BR2_PACKAGE_AMBAMAL=$(BR2_PACKAGE_AMBAMAL)
TARGET_CONFIGURE_OPTS_EXT2 = $(TARGET_CONFIGURE_OPTS) BUILD_AMBACV_DIAGS_SHARED_LIB=$(BR2_PACKAGE_BUILD_AMBACV_DIAGS_SHARED_LIB) AMBA_DLR_ROOT_DIR=$(BR2_PACKAGE_AMBA_DLR_ROOT_DIR) BR2_PACKAGE_AMBAMAL=$(BR2_PACKAGE_AMBAMAL)

define AMBACV_INSTALL_STAGING_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk DESTDIR=$(STAGING_DIR) install_staging
endef

ifeq ($(BUILD_ORCVP_AR),yes)
# build orcvp with ar
define AMBACV_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/orc_framework -f orcvp.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/orc_framework -f orcvp_ar.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBACV_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk all
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/orc_framework -f orcvp.mk all
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/orc_framework -f orcvp_ar.mk all
endef

else ifeq ($(BUILD_ORCVP),yes)
# build orcvp
define AMBACV_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk DESTDIR=$(TARGET_DIR) install
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/orc_framework -f orcvp.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBACV_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk all
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/orc_framework -f orcvp.mk all
endef

else
# Not build orcvp
define AMBACV_INSTALL_TARGET_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk DESTDIR=$(TARGET_DIR) install
endef

define AMBACV_BUILD_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk  all
endef
endif

define AMBACV_CLEAN_CMDS
  $(TARGET_CONFIGURE_OPTS_EXT1) $(MAKE) -C $(@D)/arm_framework -f linux.mk clean
endef


#define AMBACV_POST_LEGAL_INFO_HOOKS_CMD
#	@$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$(F),$$($(PKG)_DIR)/$(F))$$(sep))
#endef
#AMBACV_POST_LEGAL_INFO_HOOKS += AMBACV_POST_LEGAL_INFO_HOOKS_CMD

define AMBACV_PREPARE_REPO
	@cd $(BASE_DIR) && ln -sf ../../.repo
endef

define AMBACV_PREPARE_PACKAGE
	@sh $(AMBARELLA_PKG_DIR)/../ambarella/package/ambacv/pre-package.sh $(SRCDIR)
endef

AMBACV_PRE_RSYNC_HOOKS += AMBACV_PREPARE_REPO AMBACV_PREPARE_PACKAGE

ifeq ($(BR2_PACKAGE_BUILD_AMBACV_DIAGS_OOSB),y)
define AMBACV_PREPARE_DIAGS
	@cd $(BASE_DIR) && mkdir -p diags && echo '$(TARGET_CONFIGURE_OPTS_EXT2)' > diags/.target_make_env && make -f $(SRCDIR)/cv_common/build/init_diags.mk CV_DIR=$(@D)
endef
else
define AMBACV_PREPARE_DIAGS
	@cd $(BASE_DIR) && mkdir -p diags && echo '$(TARGET_CONFIGURE_OPTS_EXT2)' > diags/.target_make_env && make -f $(SRCDIR)/cv_common/build/init_diags.mk
endef
endif
AMBACV_POST_RSYNC_HOOKS += AMBACV_PREPARE_DIAGS


$(eval $(generic-package))
