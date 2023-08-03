################################################################################
#
# jdksavdecc-c
#
################################################################################

JDKSAVDECC_C_VERSION = 4dcedc3ef101ba5b68f7fd3642971fb128830231
JDKSAVDECC_C_SITE = git://github.com/jdkoftinoff/jdksavdecc-c.git
JDKSAVDECC_C_LICENSE = BSD-3c
JDKSAVDECC_C_LICENSE_FILES = COPYRIGHT
JDKSAVDECC_C_INSTALL_STAGING = YES

define JDKSAVDECC_C_INSTALL_STAGING_CMDS
	@echo $(STAGING_DIR) ....
	$(INSTALL) $(@D)/include/*.h $(STAGING_DIR)/usr/include/
	@$(INSTALL) -v $(@D)/libjdksavdecc-c.so $(STAGING_DIR)/usr/lib/
endef

define JDKSAVDECC_C_INSTALL_TARGET_CMDS
	@$(INSTALL) -v $(@D)/jdksavdecc-tool-print-pdu $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/jdksavdecc-tool-gen-acmpdu $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/libjdksavdecc-c.so $(TARGET_DIR)/usr/lib/
endef

$(eval $(cmake-package))

