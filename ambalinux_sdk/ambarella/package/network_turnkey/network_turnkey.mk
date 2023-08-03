
NETWORK_TURNKEY_VERSION		= 1.0
NETWORK_TURNKEY_SITE_METHOD	= local
NETWORK_TURNKEY_SITE		= $(AMBARELLA_PKG_DIR)/network_turnkey
NETWORK_TURNKEY_SOURCE		= network_turnkey-$(NETWORK_TURNKEY_VERSION).tar.gz
NETWORK_TURNKEY_INSTALL_STAGING	= NO
NETWORK_TURNKEY_INSTALL_IMAGES	= NO
NETWORK_TURNKEY_INSTALL_TARGET	= YES
NETWORK_TURNKEY_LICENSE		= Dual MIT/GPLv2
NETWORK_TURNKEY_LICENSE_FILES	= License_GPLv2.txt License_MIT.txt

ifeq ($(BR2_PACKAGE_TURNKEY_WIFI_EN_GPIO),y)
export BR2_PACKAGE_TURNKEY_WIFI_EN_GPIO_PIN
define PACKAGE_TURNKEY_WIFI_EN_GPIO
sed -i -e 's|^#WIFI_EN_GPIO\(.*\)|WIFI_EN_GPIO='$${BR2_PACKAGE_TURNKEY_WIFI_EN_GPIO_PIN}'|g' $(TARGET_DIR)/usr/local/share/script/wifi.conf
endef
export BR2_PACKAGE_TURNKEY_WIFI_EN_GPIO_STATUS
define PACKAGE_TURNKEY_WIFI_EN_GPIO_STATUS
sed -i -e 's|^#WIFI_EN_STATUS\(.*\)|WIFI_EN_STATUS='$${BR2_PACKAGE_TURNKEY_WIFI_EN_GPIO_STATUS}'|g' $(TARGET_DIR)/usr/local/share/script/wifi.conf
endef
endif

ifeq ($(BR2_PACKAGE_TURNKEY_WIFI_SWITCH_GPIO),y)
export BR2_PACKAGE_TURNKEY_WIFI_SWITCH_GPIO_PIN
define PACKAGE_TURNKEY_WIFI_SWITCH_GPIO
sed -i -e 's|^#WIFI_SWITCH_GPIO\(.*\)|WIFI_SWITCH_GPIO='$${BR2_PACKAGE_TURNKEY_WIFI_SWITCH_GPIO_PIN}'|g' $(TARGET_DIR)/usr/local/share/script/wifi.conf
endef
endif

ifeq ($(BR2_PACKAGE_TURNKEY_BT_EN_GPIO),y)
export BR2_PACKAGE_TURNKEY_BT_EN_GPIO_PIN
define PACKAGE_TURNKEY_BT_EN_GPIO
sed -i -e 's|^#BT_EN_GPIO\(.*\)|BT_EN_GPIO='$${BR2_PACKAGE_TURNKEY_BT_EN_GPIO_PIN}'|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
endef
export BR2_PACKAGE_TURNKEY_BT_EN_GPIO_STATUS
define PACKAGE_TURNKEY_BT_EN_GPIO_STATUS
sed -i -e 's|^#BT_EN_STATUS\(.*\)|BT_EN_STATUS='$${BR2_PACKAGE_TURNKEY_BT_EN_GPIO_STATUS}'|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
endef
endif

ifeq ($(BR2_PACKAGE_NETWORK_TURNKEY_STARTUP_KEEP_POWER),y)
define PACKAGE_NETWORK_TURNKEY_STARTUP
rm -f $(TARGET_DIR)/etc/init.d/S91wifi
cp -a $(@D)/source/etc/init.d/S52wifi $(TARGET_DIR)/etc/init.d/S52wifi
endef
else
ifeq ($(BR2_PACKAGE_NETWORK_TURNKEY_STARTUP),y)
define PACKAGE_NETWORK_TURNKEY_STARTUP
rm -f $(TARGET_DIR)/etc/init.d/S52wifi
cp -a $(@D)/source/etc/init.d/S52wifi $(TARGET_DIR)/etc/init.d/S91wifi
endef
else
define PACKAGE_NETWORK_TURNKEY_STARTUP
rm -f $(TARGET_DIR)/etc/init.d/S52wifi $(TARGET_DIR)/etc/init.d/S91wifi
endef
endif
endif

ifeq ($(BR2_PACKAGE_BT_TURNKEY_STARTUP),y)
define PACKAGE_BT_TURNKEY_STARTUP
cp -a $(@D)/source/etc/init.d/S53bt $(TARGET_DIR)/etc/init.d/
endef
else
define PACKAGE_BT_TURNKEY_STARTUP
rm -f $(TARGET_DIR)/etc/init.d/S53bt
endef
endif

ifeq ($(BR2_PACKAGE_LTE_TURNKEY_STARTUP),y)
define PACKAGE_LTE_TURNKEY_STARTUP
cp -a $(@D)/source/etc/init.d/S54lte $(TARGET_DIR)/etc/init.d/
endef
else
define PACKAGE_LTE_TURNKEY_STARTUP
rm -f $(TARGET_DIR)/etc/init.d/S54lte
endef
endif

define NETWORK_TURNKEY_INSTALL_TARGET_CMDS
	cp -f ${AMBARELLA_PKG_DIR}/tools/* $(TARGET_DIR)/usr/sbin/
	mkdir -p $(TARGET_DIR)/usr/local/share/script
	cp -a $(@D)/source/* $(TARGET_DIR)/
	$(PACKAGE_NETWORK_TURNKEY_STARTUP)
	$(PACKAGE_BT_TURNKEY_STARTUP)
	$(PACKAGE_LTE_TURNKEY_STARTUP)
	$(PACKAGE_TURNKEY_WIFI_EN_GPIO)
	$(PACKAGE_TURNKEY_WIFI_SWITCH_GPIO)
	$(PACKAGE_TURNKEY_BT_EN_GPIO)
	$(PACKAGE_TURNKEY_BT_EN_GPIO_STATUS)
	$(PACKAGE_TURNKEY_WIFI_EN_GPIO_STATUS)
endef

#
# buildroot will skip collecting legal-info of local packages.
# Need hook function to copy license files and update csv file, etc.
#
#define NETWORK_TURNKEY_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#NETWORK_TURNKEY_POST_LEGAL_INFO_HOOKS += NETWORK_TURNKEY_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
