################################################################################
#
# Xen
#
################################################################################

XEN_VERSION = 4.14.4
XEN_SITE = https://downloads.xenproject.org/release/xen/$(XEN_VERSION)
XEN_LICENSE = GPL-2.0
XEN_LICENSE_FILES = COPYING
XEN_CPE_ID_VENDOR = xen
XEN_CPE_ID_PREFIX = cpe:2.3:o
XEN_DEPENDENCIES = host-acpica host-python3

# Calculate XEN_ARCH
ifeq ($(ARCH),aarch64)
XEN_ARCH = arm64
else ifeq ($(ARCH),arm)
XEN_ARCH = arm32
endif

XEN_CONF_OPTS = \
	--disable-golang \
	--disable-ocamltools \
	--with-initddir=/etc/init.d

# Ambarella customization start
XEN_CONF_OPTS += \
	--disable-stubdom \
	--disable-docs \
	--disable-githttp \
	--disable-systemd \
	--disable-seabios \
	--disable-rombios \
	--disable-ovmf \
	--disable-qemu-traditional
# Ambarella customization end


XEN_CONF_ENV = PYTHON=$(HOST_DIR)/bin/python3
XEN_MAKE_ENV = \
	XEN_TARGET_ARCH=$(XEN_ARCH) \
	CROSS_COMPILE=$(TARGET_CROSS) \
	HOST_EXTRACFLAGS="-Wno-error" \
	XEN_HAS_CHECKPOLICY=n \
	$(TARGET_CONFIGURE_OPTS)

ifeq ($(BR2_PACKAGE_XEN_HYPERVISOR),y)
XEN_MAKE_OPTS += dist-xen
XEN_INSTALL_IMAGES = YES
define XEN_INSTALL_IMAGES_CMDS
	cp $(@D)/xen/xen $(BINARIES_DIR)
endef
else
XEN_CONF_OPTS += --disable-xen
endif

ifeq ($(BR2_PACKAGE_XEN_TOOLS),y)
XEN_DEPENDENCIES += \
	dtc libaio libglib2 ncurses openssl pixman slirp util-linux yajl
ifeq ($(BR2_PACKAGE_ARGP_STANDALONE),y)
XEN_DEPENDENCIES += argp-standalone
endif
XEN_INSTALL_TARGET_OPTS += DESTDIR=$(TARGET_DIR) install-tools
XEN_MAKE_OPTS += dist-tools
XEN_CONF_OPTS += --with-extra-qemuu-configure-args="--disable-sdl --disable-opengl"

define XEN_INSTALL_INIT_SYSV
	mv $(TARGET_DIR)/etc/init.d/xencommons $(TARGET_DIR)/etc/init.d/S50xencommons
	mv $(TARGET_DIR)/etc/init.d/xen-watchdog $(TARGET_DIR)/etc/init.d/S50xen-watchdog
	mv $(TARGET_DIR)/etc/init.d/xendomains $(TARGET_DIR)/etc/init.d/S60xendomains
endef
else
XEN_INSTALL_TARGET = NO
XEN_CONF_OPTS += --disable-tools
endif

# Ambarella customization start
define XEN_POST_PATCH_HOOKS_CP_AMBA_FILE
	cp -rf $(TOPDIR)/$(XEN_PKGDIR)amba_xen/* $(@D)/
endef # XEN_POST_PATCH_HOOKS

XEN_POST_PATCH_HOOKS += XEN_POST_PATCH_HOOKS_CP_AMBA_FILE

define XEN_PRE_BUILD_HOOKS_DEFCONFIG
	$(XEN_MAKE_ENV) $(MAKE) -C $(@D)/xen ambarella_defconfig
endef # XEN_PRE_BUILD_HOOKS_DEFCONFIG

XEN_PRE_BUILD_HOOKS += XEN_PRE_BUILD_HOOKS_DEFCONFIG

define XEN_POST_INSTALL_IMAGES_HOOKS_CP_XEN_SYMS
	cp $(@D)/xen/xen-syms $(BINARIES_DIR)/
endef # XEN_POST_INSTALL_IMAGES_HOOKS_SYMS

XEN_POST_INSTALL_IMAGES_HOOKS += XEN_POST_INSTALL_IMAGES_HOOKS_CP_XEN_SYMS

# Ambarella customization end

$(eval $(autotools-package))
