################################################################################
#
# Fast-DDS
#
################################################################################

FAST_DDS_VERSION = 2.5.0
FAST_DDS_SITE = $(call github,eProsima,Fast-DDS,v$(FAST_DDS_VERSION))
FAST_DDS_LICENSE = Apache License 2.0
FAST_DDS_LICENSE_FILES = LICENSE
FAST_DDS_INSTALL_STAGING = YES

FAST_DDS_DEPENDENCIES = fast-cdr
#FAST_DDS_DEPENDENCIES += boost
FAST_DDS_DEPENDENCIES += foonathan-memory
FAST_DDS_DEPENDENCIES += tinyxml2


# TODO: OpenSSL, libp11, SoftHSM
# cf. https://fast-dds.docs.eprosima.com/en/latest/installation/configuration/cmake_options.html

#FAST_DDS_CONF_OPTS += -DCOMPILE_EXAMPLES=ON
#FAST_DDS_CONF_OPTS += -DINSTALL_EXAMPLES=ON
#FAST_DDS_CONF_OPTS += -DINSTALL_TOOLS=ON

$(eval $(cmake-package))

