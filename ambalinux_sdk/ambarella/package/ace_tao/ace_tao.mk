################################################################################
#
# ACE + TAO
#
################################################################################

ACE_TAO_VERSION = 2.2a_p22
ACE_TAO_SOURCE = ACE+TAO-$(ACE_TAO_VERSION)_complete_NO_makefiles.tar.gz
ACE_TAO_SITE = http://download.objectcomputing.com/TAO-2.2a_patches
ACE_TAO_LICENSE = DOC
ACE_TAO_LICENSE_FILES = COPYING
ACE_TAO_CPE_ID_VENDOR = vanderbilt
ACE_TAO_CPE_ID_PRODUCT = ACE_TAO

#ACE_TAO_INSTALL_STAGING = NO
#ACE_TAO_INSTALL_TARGET = NO

define ACE_TAO_CONFIGURE_CMDS
endef # define ACE_TAO_CONFIGURE_CMDS

define ACE_TAO_BUILD_CMDS
endef # ACE_TAO_BUILD_CMDS

define ACE_TAO_INSTALL_STAGING_CMDS
endef # ACE_TAO_INSTALL_STAGING_CMDS

define ACE_TAO_INSTALL_TARGET_CMDS
endef # ACE_TAO_INSTALL_TARGET_CMDS

$(eval $(generic-package))

