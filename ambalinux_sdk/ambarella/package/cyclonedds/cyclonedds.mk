################################################################################
#
# Eclipse Cyclone DDS
#
################################################################################

CYCLONEDDS_VERSION = 0.8.2
CYCLONEDDS_SITE = $(call github,eclipse-cyclonedds,cyclonedds,$(CYCLONEDDS_VERSION))
CYCLONEDDS_LICENSE = Eclipse Public License v. 2.0
CYCLONEDDS_LICENSE_FILES = LICENSE
CYCLONEDDS_INSTALL_STAGING = YES

HOST_CYCLONEDDS_DEPENDENCIES = host-bison

CYCLONEDDS_DEPENDENCIES = host-cyclonedds
CYCLONEDDS_DEPENDENCIES += openssl
#CYCLONEDDS_DEPENDENCIES += iceoryx


HOST_CYCLONEDDS_CONF_OPTS += -DENABLE_SSL=OFF

# TODO: Security

CYCLONEDDS_CONF_OPTS += -DBUILD_DDSCONF=ON

ifneq ($(BR2_ENABLE_DEBUG),)
CYCLONEDDS_CONF_OPTS += -DCMAKE_BUILD_TYPE=Debug
endif
#CYCLONEDDS_CONF_OPTS += -DBUILD_EXAMPLES=ON


$(eval $(cmake-package))
$(eval $(host-cmake-package))

