AMBAGPU_SOURCE = Imagination.tar.gz
AMBAGPU_VERSION = 1.17 # DDK version 1.17
#AMBAGPU_LICENSE = ?
#AMBAGPU_LICENSE_FILES = ?
AMBAGPU_SITE_METHOD	= local
AMBAGPU_SITE		= ../external_sdk/IMG
AMBAGPU_DEPENDENCIES	= linux
AMBAGPU_INSTALL_STAGING	= YES
AMBAGPU_INSTALL_TARGET	= YES

define AMBAGPU_INSTALL_STAGING_CMDS
        cp -a $(@D)/target/cv3x/usr/lib/* $(STAGING_DIR)/usr/lib/
endef

define AMBAGPU_INSTALL_TARGET_CMDS
        cp -a $(@D)/target/cv3x/* $(TARGET_DIR)
endef

$(eval $(generic-package))
