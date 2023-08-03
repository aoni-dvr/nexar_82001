################################################################################
#
# foonathan / memory
#
################################################################################

FOONATHAN_MEMORY_VERSION = 0.7-1
FOONATHAN_MEMORY_SOURCE = v$(FOONATHAN_MEMORY_VERSION).tar.gz
FOONATHAN_MEMORY_SITE = https://github.com/foonathan/memory/archive/refs/tags
FOONATHAN_MEMORY_LICENSE = Zlib
FOONATHAN_MEMORY_LICENSE_FILES = LICENSE
FOONATHAN_MEMORY_INSTALL_STAGING = YES

FOONATHAN_MEMORY_CONF_OPTS += -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF
FOONATHAN_MEMORY_CONF_OPTS += -DFOONATHAN_MEMORY_BUILD_TESTS=OFF

$(eval $(cmake-package))

