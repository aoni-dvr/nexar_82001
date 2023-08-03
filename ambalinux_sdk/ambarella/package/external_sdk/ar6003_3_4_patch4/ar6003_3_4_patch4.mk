AR6003_3_4_PATCH4_VERSION		= amba
AR6003_3_4_PATCH4_SITE_METHOD	= local
AR6003_3_4_PATCH4_SITE		= ../external_sdk/atheros/ar6003_3_4_patch4
AR6003_3_4_PATCH4_DEPENDENCIES	= linux openssl libnl libglib2
AR6003_3_4_PATCH4_INSTALL_STAGING	= NO
AR6003_3_4_PATCH4_INSTALL_TARGET	= YES

define AR6003_3_4_PATCH4_INSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/hostapd
	mkdir -p $(TARGET_DIR)/usr/local/share/script
	cp -a $(@D)/load.sh $(@D)/unload.sh $(TARGET_DIR)/usr/local/share/script/
	mkdir -p $(TARGET_DIR)/lib/firmware/ath6k/AR6003/hw2.1.1/
	for i in bdata.bin softmac fw-4.bin athwlan.bin athtcmd_ram.bin; do \
	cp -a $(@D)/Firmware_Package/target/AR6003/hw2.1.1/fw_smartphone/$$i $(TARGET_DIR)/lib/firmware/ath6k/AR6003/hw2.1.1/; \
	done
	rm -f $(TARGET_DIR)/lib/modules/3.8.0/kernel/net/wireless/cfg80211.ko $(TARGET_DIR)/lib/modules/3.8.0/kernel/net/mac80211/mac80211.ko
	cd $(@D)/Generic_Packages/compat-wireless
	ARCH=arm KLIB_BUILD=$(LINUX_DIR) KLIB=$(TARGET_DIR) CROSS_COMPILE=$(TARGET_CROSS) DESTDIR=$(TARGET_DIR) \
	$(TARGET_MAKE_ENV) $(MAKE) install-modules -C $(@D)/Generic_Packages/compat-wireless/
	cd ../..
	find $(TARGET_DIR)/lib/modules/3.10.0/updates/ -name "*.ko" -exec $(TARGET_STRIP) --strip-unneeded '{}' \;
	mkdir -p $(TARGET_DIR)/usr/local/ath6kl/
	cp -a $(@D)/Generic_Packages/iw/iw $(TARGET_DIR)/usr/local/ath6kl/
	cp -a $(@D)/Generic_Packages/wpa_supplicant_8/wpa_supplicant/wpa_supplicant $(TARGET_DIR)/usr/local/ath6kl/
	cp -a $(@D)/Generic_Packages/wpa_supplicant_8/wpa_supplicant/wpa_cli $(TARGET_DIR)/usr/local/ath6kl/
	for i in `ls $(TARGET_DIR)/usr/local/ath6kl/`; do \
	ln -fs /usr/local/ath6kl/$$i $(TARGET_DIR)/usr/sbin/; \
	done
endef

define AR6003_3_4_PATCH4_UNINSTALL_TARGET_CMDS
	@V=`cat $(LINUX_DIR)/include/generated/utsrelease.h | grep UTS_RELEASE | awk -F "\"" '{ print $$2 }'` && \
		rm -rf $(TARGET_DIR)/lib/modules/$$V/updates && \
		depmod -a -b $(TARGET_DIR) $$V

	@for i in `ls $(TARGET_DIR)/usr/local/ath6kl/`; do \
		rm -f $(TARGET_DIR)/usr/sbin/$$i; \
	done
	@rm -rf $(TARGET_DIR)/usr/local/ath6kl/

	@rm -rf $(TARGET_DIR)/lib/firmware/ath6k/AR6003

	@for i in load.sh unload.sh preload.sh; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$$i; \
	done
endef

define AR6003_3_4_PATCH4_COMPAT-WIRELESS
	cd $(@D)/Generic_Packages/compat-wireless/ && ./scripts/driver-select ath6kl && cd -
	ARCH=arm KLIB_BUILD=$(LINUX_DIR) KLIB=$(TARGET_DIR) CROSS_COMPILE=$(TARGET_CROSS) DESTDIR=$(TARGET_DIR) \
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Generic_Packages/compat-wireless/
endef

define AR6003_3_4_PATCH4_IW
	$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
	$(MAKE) -C $(@D)/Generic_Packages/iw/
endef

define AR6003_3_4_PATCH4_SUPPLICANT
	$(TARGET_MAKE_ENV) CONFIG_LIBNL32=y CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/Generic_Packages/hostap/wpa_supplicant/
endef

define AR6003_3_4_PATCH4_SUPPLICANT_8
	$(TARGET_MAKE_ENV) CONFIG_LIBNL32=y CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/Generic_Packages/wpa_supplicant_8/wpa_supplicant/
endef

define AR6003_3_4_PATCH4_BUILD_CMDS
	$(AR6003_3_4_PATCH4_COMPAT-WIRELESS)
	$(AR6003_3_4_PATCH4_IW)
	$(AR6003_3_4_PATCH4_SUPPLICANT_8)
endef

$(eval $(generic-package))
