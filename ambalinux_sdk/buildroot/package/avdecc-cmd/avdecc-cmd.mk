################################################################################
#
# avdecc-cmd
#
################################################################################

AVDECC_CMD_VERSION = 67367b336ea52caba87f5dd13879172094c36cb4
AVDECC_CMD_SITE = git://github.com/jdkoftinoff/avdecc-cmd.git
AVDECC_CMD_LICENSE = BSD-2c
AVDECC_CMD_LICENSE_FILES = LICENSE
AVDECC_CMD_DEPENDENCIES = libpcap jdksavdecc-c

define AVDECC_CMD_INSTALL_TARGET_CMDS
	@$(INSTALL) -v $(@D)/avdecc-get-control $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/avdecc-discover $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/avdecc-read-descriptor $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/avdecc-set-control $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/avdecc $(TARGET_DIR)/usr/sbin/
	@$(INSTALL) -v $(@D)/libavdecc-cmd.so $(TARGET_DIR)/usr/lib/
endef

$(eval $(cmake-package))

