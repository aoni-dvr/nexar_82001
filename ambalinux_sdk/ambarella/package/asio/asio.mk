################################################################################
#
# Asio
#
################################################################################

ASIO_VERSION = 1.21.0
ASIO_SOURCE = asio-$(ASIO_VERSION).tar.bz2
ASIO_SITE = http://sourceforge.net/projects/asio/files
ASIO_LICENSE = Boost Software License v1.0
ASIO_LICENSE_FILES = LICENSE_1_0.txt COPYING
ASIO_INSTALL_STAGING = YES
ASIO_DEPENDENCIES = openssl
#ASIO_DEPENDENCIES += boost



$(eval $(autotools-package))

