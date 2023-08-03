################################################################################
#
# optee-test
#
################################################################################

OPTEE_TEST_VERSION = 3.16.0
OPTEE_TEST_SITE = $(call github,OP-TEE,optee_test,$(OPTEE_TEST_VERSION))
OPTEE_TEST_LICENSE = GPL-2.0, BSD-2-Clause,
OPTEE_TEST_LICENSE_FILES = LICENSE.md

# Ambarella customization
override OPTEE_TEST_VERSION = amba
override OPTEE_TEST_SITE_METHOD = local
override OPTEE_TEST_SITE = $(AMBARELLA_PKG_DIR)/optee_test
ifneq ($(call qstrip,$(BR2_TARGET_OPTEE_OS_PLATFORM_FLAVOR)),)
OPTEE_TEST_MAKE_OPTS += PLATFORM_ARCH=$(call qstrip,$(BR2_TARGET_OPTEE_OS_PLATFORM_FLAVOR))
endif

OPTEE_TEST_DEPENDENCIES = optee-client optee-os

ifeq ($(BR2_PACKAGE_LIBOPENSSL),y)
OPTEE_TEST_DEPENDENCIES += libopenssl
endif

OPTEE_TEST_CONF_OPTS = -DOPTEE_TEST_SDK=$(OPTEE_OS_SDK)

# Trusted Application are not built from CMake due to ta_dev_kit
# dependencies. We must build and install them on target.
define OPTEE_TEST_BUILD_TAS
	$(foreach f,$(wildcard $(@D)/ta/*_lib/Makefile) $(wildcard $(@D)/ta/*/Makefile) $(wildcard $(@D)/host/*/Makefile), \
		$(TARGET_CONFIGURE_OPTS) \
		$(MAKE) CROSS_COMPILE=$(TARGET_CROSS) \
			TA_DEV_KIT_DIR=$(OPTEE_OS_SDK) $(OPTEE_TEST_MAKE_OPTS) \
			-C $(dir $f) all
	)
endef
ifdef BR2_PACKAGE_OPTEE_TEST_AMBA
define OPTEE_TEST_INSTALL_TAS
	@mkdir -p $(TARGET_DIR)/lib/optee_armtz
	@$(INSTALL) -D -m 444 -t $(TARGET_DIR)/lib/optee_armtz $(@D)/ta/*/*.ta
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/data_at_rest_test/out/data_at_rest_test/data_at_rest_test
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/test_data_encryption_aes/out/test_data_encryption_aes/test_data_encryption_aes
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/test_program_otp/out/test_program_otp/test_program_otp
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/test_secure_credential_provisioning/out/test_secure_credential_provisioning/test_secure_credential_provisioning
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/test_secure_storage/out/test_secure_storage/test_secure_storage
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/data_encryption_aes_v2/out/data_encryption_aes_v2/data_encryption_aes_v2_test
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/data_encryption_aes_v2_verify/out/data_encryption_aes_v2_verify/data_encryption_aes_v2_verify
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/openssl_sign_engine/out/openssl_sign_engine_test/openssl_sign_engine_test
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/optee_tls_test/out/optee_tls_test/optee_tls_test
	@$(INSTALL) -D -m 755 -t $(TARGET_DIR)/usr/bin/ $(@D)/host/secure_storage_simple_test/out/secure_storage_simple_test/secure_storage_simple_test
endef
else
define OPTEE_TEST_INSTALL_TAS
	@mkdir -p $(TARGET_DIR)/lib/optee_armtz
	@$(INSTALL) -D -m 444 -t $(TARGET_DIR)/lib/optee_armtz $(@D)/ta/*/*.ta
endef
endif
OPTEE_TEST_POST_BUILD_HOOKS += OPTEE_TEST_BUILD_TAS
OPTEE_TEST_POST_INSTALL_TARGET_HOOKS += OPTEE_TEST_INSTALL_TAS

$(eval $(cmake-package))
