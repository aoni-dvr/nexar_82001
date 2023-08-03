################################################################################
#
# OpenDDS
#
################################################################################

OPENDDS_VERSION = 3.19
OPENDDS_SOURCE = DDS-$(OPENDDS_VERSION).tar.gz
OPENDDS_SITE = https://github.com/objectcomputing/OpenDDS/archive/refs/tags
OPENDDS_LICENSE = public domain
OPENDDS_LICENSE_FILES = LICENSE
OPENDDS_INSTALL_STAGING = YES
#OPENDDS_DEPENDENCIES = perl
OPENDDS_DEPENDENCIES += ace_tao

_OPENDDS_ACE_TAO_DL = NO

# TODO:
#   --ipv6                          IPv6 support (no)
#   --security                      DDS Security plugin (no) Implies --openssl and
#                                   --xerces3
#                                   cf. https://opendds.org/quickstart/GettingStartedPi.html
#
define OPENDDS_CONFIGURE_CMDS
	(cd $(OPENDDS_SRCDIR) && rm -rf config.cache && \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	CONFIG_SITE=/dev/null \
	ACE_ROOT=$(@D)/../ace_tao-$(ACE_TAO_VERSION) \
	./configure \
		--target=linux-cross \
		--target-compiler=$(TARGET_CC) \
		--prefix=/usr \
		$(QUIET) \
		$(if $(VERBOSE),-v) \
	)
endef # define OPENDDS_CONFIGURE_CMDS

# Also need to instal host tools
define OPENDDS_INSTALL_STAGING_CMDS
	$(HOST_MAKE_ENV)   $(OPENDDS_MAKE_ENV) $(MAKE) $(OPENDDS_INSTALL_OPTS) DESTDIR=$(HOST_DIR) -C $(OPENDDS_SRCDIR)build/host
	$(TARGET_MAKE_ENV) $(OPENDDS_MAKE_ENV) $(MAKE) $(OPENDDS_INSTALL_STAGING_OPTS) -C $(OPENDDS_SRCDIR)build/target
	mkdir -p $(STAGING_DIR)/usr/share/dds/dds/idl/
	install $(OPENDDS_SRCDIR)build/target/dds/idl/IDLTemplate.txt $(STAGING_DIR)/usr/share/dds/dds/idl/
endef # OPENDDS_INSTALL_STAGING_CMDS

# TODO: remove files from target/
#   usr/include/ace/
#   usr/include/dds/
#   usr/include/FACE/
#   usr/include/model/
#   usr/include/orbsvcs/
#   usr/include/tao/
#
#   usr/lib/cmake/
#   usr/lib/pkgconfig/
#
#   usr/share/ace/
#   usr/share/dds/
#   usr/share/tao/
#
define OPENDDS_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(OPENDDS_MAKE_ENV) $(MAKE) $(OPENDDS_INSTALL_TARGET_OPTS) -C $(OPENDDS_SRCDIR)build/target
endef # OPENDDS_INSTALL_TARGET_CMDS

$(eval $(autotools-package))

