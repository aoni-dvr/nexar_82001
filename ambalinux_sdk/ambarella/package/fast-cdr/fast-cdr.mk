################################################################################
#
# Fast-CDR
#
################################################################################

FAST_CDR_VERSION = 1.0.23
FAST_CDR_SITE = $(call github,eProsima,Fast-CDR,v$(FAST_CDR_VERSION))
FAST_CDR_LICENSE = Apache License 2.0
FAST_CDR_LICENSE_FILES = LICENSE
FAST_CDR_INSTALL_STAGING = YES

$(eval $(cmake-package))

