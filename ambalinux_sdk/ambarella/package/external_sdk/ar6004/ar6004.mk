AR6004_VERSION		= amba
AR6004_SITE_METHOD	= local
AR6004_SITE		= ../external_sdk/atheros/ar6004
AR6004_DEPENDENCIES	= linux openssl libnl libglib2 network_turnkey
AR6004_INSTALL_STAGING	= NO
AR6004_INSTALL_TARGET	= YES

AR6004_DRV		= $(@D)/WLAN-OSR-AMBA
AR6004_WPA		= $(@D)/external/hostap
AR6004_KVER		= $(shell cat $(LINUX_DIR)/include/generated/utsrelease.h | \
				grep UTS_RELEASE | awk -F "\"" '{ print $$2 }')

define AR6004_INSTALL_TARGET_CMDS
	@rm -f $(TARGET_DIR)/usr/sbin/hostapd
	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load.sh unload.sh wifi_softmac.sh; do \
		cp -a $(@D)/scripts/$$i $(TARGET_DIR)/usr/local/share/script/; \
	done
	@$(SED) 's|^HCI_DRIVER=\(.*\)|HCI_DRIVER=ath3k|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
	@cp -a $(@D)/scripts/preload_A9.sh $(TARGET_DIR)/usr/local/share/script/preload.sh

	@mkdir -p $(TARGET_DIR)/lib/firmware/ath6k/AR6004/hw3.0/
	@for i in bdata.bin fw.ram.bin otp.bin softmac.bin; do \
		cp -a $(@D)/hw3.0/$$i $(TARGET_DIR)/lib/firmware/ath6k/AR6004/hw3.0/; \
	done

	@mkdir -p $(TARGET_DIR)/lib/modules/$(AR6004_KVER)/updates/drivers/net/wireless/ath/ath6kl-3.5
	@cp -f $(AR6004_DRV)/drivers/ath6kl/ath6kl_sdio.ko \
		$(TARGET_DIR)/lib/modules/$(AR6004_KVER)/updates/drivers/net/wireless/ath/ath6kl-3.5/
	@find $(TARGET_DIR)/lib/modules/$(AR6004_KVER)/updates/ -name "*.ko" \
		-exec $(TARGET_STRIP) --strip-unneeded '{}' \;
	@depmod -a -b $(TARGET_DIR) $(AR6004_KVER)

	@mkdir -p $(TARGET_DIR)/usr/local/ath6kl/
	@cp -a $(@D)/external/iw-3.0/iw $(TARGET_DIR)/usr/local/ath6kl/
	@cp -a $(AR6004_WPA)/wpa_supplicant/wpa_supplicant $(TARGET_DIR)/usr/local/ath6kl/
	@cp -a $(AR6004_WPA)/wpa_supplicant/wpa_cli $(TARGET_DIR)/usr/local/ath6kl/
	@cp -a $(@D)/external/tcmd/athtestcmd.libnl3 $(TARGET_DIR)/usr/local/ath6kl/athtestcmd
	@cp -a $(@D)/external/btfilter/abtfilt.libnl3 $(TARGET_DIR)/usr/local/ath6kl/abtfilt
	@for i in `ls $(TARGET_DIR)/usr/local/ath6kl/`; do \
		ln -fs /usr/local/ath6kl/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@cp -a $(@D)/firmware ${TARGET_DIR}/lib/
endef

define AR6004_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/lib/modules/$(AR6004_KVER)/updates
	@depmod -a -b $(TARGET_DIR) $(AR6004_KVER)

	@for i in `ls $(TARGET_DIR)/usr/local/ath6kl/`; do \
		rm -f $(TARGET_DIR)/usr/sbin/$$i; \
	done
	@rm -rf $(TARGET_DIR)/usr/local/ath6kl/

	@rm -rf $(TARGET_DIR)/lib/firmware/ath6k/AR6004
	@rm -rf $(TARGET_DIR)/lib/firmware/ath3k

	@for i in load.sh unload.sh preload.sh wifi_softmac.sh; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$$i; \
	done
endef

define AR6004_DRIVER
	@cd $(AR6004_DRV)/build/ && \
		ARCH=arm KLIB_BUILD=$(LINUX_DIR) KLIB=$(TARGET_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS) DESTDIR=$(TARGET_DIR) \
		$(TARGET_MAKE_ENV) \
		KERNELARCH=arm KERNELPATH=$(LINUX_DIR) TOOLPREFIX=$(TARGET_CROSS) \
		BOARD_TYPE=ambarella kernel=$(AR6004_KVER) \
		$(MAKE) drivers
endef

define AR6004_IW
	@$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) LDFLAGS="$(TARGET_LDFLAGS) -lm" \
		$(MAKE) -C $(@D)/external/iw-3.0/
endef

define AR6004_SUPPLICANT
	@$(TARGET_MAKE_ENV) CONFIG_LIBNL32=y \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) CC="$(TARGET_CC)" \
		-C $(AR6004_WPA)/wpa_supplicant/
endef

define AR6004_BUILD_CMDS
	$(AR6004_DRIVER)
	$(AR6004_IW)
	$(AR6004_SUPPLICANT)
endef

$(eval $(generic-package))
