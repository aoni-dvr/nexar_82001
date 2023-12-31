################################################################################
#
# bluez5_utils
#
################################################################################

## Keep the version and patches in sync with bluez5_utils-headers
BLUEZ5_UTILS_VERSION = 5.50
#BLUEZ5_UTILS_VERSION = 5.41
BLUEZ5_UTILS_SOURCE = bluez-$(BLUEZ5_UTILS_VERSION).tar.xz
BLUEZ5_UTILS_SITE = $(BR2_KERNEL_MIRROR)/linux/bluetooth
BLUEZ5_UTILS_INSTALL_STAGING = YES
BLUEZ5_UTILS_LICENSE = GPL-2.0+, LGPL-2.1+
BLUEZ5_UTILS_LICENSE_FILES = COPYING COPYING.LIB

BLUEZ5_UTILS_DEPENDENCIES = dbus libglib2 amba_examples

#BLUEZ5_UTILS_DEPENDENCIES = \
#	$(if $(BR2_PACKAGE_BLUEZ5_UTILS_HEADERS),bluez5_utils-headers) \
#	dbus \
#	libglib2
#
#BLUEZ5_UTILS_CONF_OPTS = \
#	--enable-tools \
#	--enable-library \
#	--disable-cups
BLUEZ5_UTILS_CONF_OPTS = 	\
	--enable-tools 		\
	--enable-library 	\
	--disable-cups 		\
	--disable-network 	\
	--localstatedir=/var --sysconfdir=/tmp \
	LIBS=-lpthread \
#	--enable-maintainer-mode \

ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_OBEX),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-obex
BLUEZ5_UTILS_DEPENDENCIES += libical
else
BLUEZ5_UTILS_CONF_OPTS += --disable-obex
endif

ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_CLIENT),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-client
BLUEZ5_UTILS_DEPENDENCIES += readline
else
BLUEZ5_UTILS_CONF_OPTS += --disable-client
endif

# experimental plugins
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_EXPERIMENTAL),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-experimental
else
BLUEZ5_UTILS_CONF_OPTS += --disable-experimental
endif

# enable health plugin
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_PLUGINS_HEALTH),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-health
else
BLUEZ5_UTILS_CONF_OPTS += --disable-health
endif

# enable midi profile
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_PLUGINS_MIDI),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-midi
BLUEZ5_UTILS_DEPENDENCIES += alsa-lib
else
BLUEZ5_UTILS_CONF_OPTS += --disable-midi
endif

# enable nfc plugin
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_PLUGINS_NFC),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-nfc
else
BLUEZ5_UTILS_CONF_OPTS += --disable-nfc
endif

# enable sap plugin
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_PLUGINS_SAP),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-sap
else
BLUEZ5_UTILS_CONF_OPTS += --disable-sap
endif

# enable sixaxis plugin
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_PLUGINS_SIXAXIS),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-sixaxis
else
BLUEZ5_UTILS_CONF_OPTS += --disable-sixaxis
endif

# install gatttool (For some reason upstream choose not to do it by default)
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_GATTTOOL),y)
define BLUEZ5_UTILS_INSTALL_GATTTOOL
	$(INSTALL) -D -m 0755 $(@D)/attrib/gatttool $(TARGET_DIR)/usr/bin/gatttool
endef
BLUEZ5_UTILS_POST_INSTALL_TARGET_HOOKS += BLUEZ5_UTILS_INSTALL_GATTTOOL
# hciattach_bcm43xx defines default firmware path in `/etc/firmware`, but
# Broadcom firmware blobs are usually located in `/lib/firmware`.
BLUEZ5_UTILS_CONF_ENV += \
	CPPFLAGS='$(TARGET_CPPFLAGS) -DFIRMWARE_DIR=\"/lib/firmware\"'
BLUEZ5_UTILS_CONF_OPTS += --enable-deprecated
else
BLUEZ5_UTILS_CONF_OPTS += --disable-deprecated
endif

# enable test
ifeq ($(BR2_PACKAGE_BLUEZ5_UTILS_TEST),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-test
else
BLUEZ5_UTILS_CONF_OPTS += --disable-test
endif

# use udev if available
ifeq ($(BR2_PACKAGE_HAS_UDEV),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-udev
BLUEZ5_UTILS_DEPENDENCIES += udev
else
BLUEZ5_UTILS_CONF_OPTS += --disable-udev
endif

# integrate with systemd if available
ifeq ($(BR2_PACKAGE_SYSTEMD),y)
BLUEZ5_UTILS_CONF_OPTS += --enable-systemd
BLUEZ5_UTILS_DEPENDENCIES += systemd
else
BLUEZ5_UTILS_CONF_OPTS += --disable-systemd
endif

define BLUEZ5_UTILS_INSTALL_INIT_SYSTEMD
	mkdir -p $(TARGET_DIR)/etc/systemd/system/bluetooth.target.wants
	ln -fs ../../../../usr/lib/systemd/system/bluetooth.service \
		$(TARGET_DIR)/etc/systemd/system/bluetooth.target.wants/bluetooth.service
	ln -fs ../../../usr/lib/systemd/system/bluetooth.service \
		$(TARGET_DIR)/etc/systemd/system/dbus-org.bluez.service
endef

define BLUEZ5_UTILS_PATCH_HOOKS
	rsync -a $(PKGDIR)/hciattach_rtk.c $(@D)/tools/
	rsync -a $(PKGDIR)/btgatt-server.c $(@D)/tools/
	rsync -a $(PKGDIR)/custom_gatt_service.c $(@D)/tools/
	rsync -a $(PKGDIR)/custom_ibeacon.c $(@D)/tools/
	rsync -a $(PKGDIR)/stream_notify.c $(@D)/tools/
	rsync -a $(PKGDIR)/../../../pkg/http_control_server/cmd_ctrl.c $(@D)/tools/
	rsync -a $(PKGDIR)/../../../pkg/http_control_server/debug.h $(@D)/tools/
	rsync -a $(PKGDIR)/../../../pkg/http_control_server/global.h $(@D)/tools/
	rsync -a $(PKGDIR)/../../../pkg/http_control_server/cmd_response.h $(@D)/tools/
endef
BLUEZ5_UTILS_PRE_PATCH_HOOKS += BLUEZ5_UTILS_PATCH_HOOKS

#fix maintainer-mode compile error
define BLUEZ5_UTILS_CONFIGURE_HOOKS
	$(SED) 's!-Werror!!g' $(@D)/acinclude.m4
	$(SED) 's!with_cflags=""!with_cflags="-Wall"!g' $(@D)/acinclude.m4
endef
BLUEZ5_UTILS_PRE_CONFIGURE_HOOKS += BLUEZ5_UTILS_CONFIGURE_HOOKS

define BLUEZ5_UTILS_INSTALL_HOOKS
	mkdir -p $(TARGET_DIR)/etc/bluetooth/
	install -D -m 0644 $(@D)/src/main.conf $(TARGET_DIR)/etc/bluetooth/
	install -D -m 0777 $(@D)/tools/btgatt-server $(TARGET_DIR)/usr/sbin/
endef
BLUEZ5_UTILS_POST_INSTALL_TARGET_HOOKS += BLUEZ5_UTILS_INSTALL_HOOKS

$(eval $(autotools-package))
