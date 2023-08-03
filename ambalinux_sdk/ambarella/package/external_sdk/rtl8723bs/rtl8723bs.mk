RTL8723BS_VERSION		= amba
RTL8723BS_SITE_METHOD		= local
RTL8723BS_SITE			= ../external_sdk/realtek/rtl8723bs
RTL8723BS_DEPENDENCIES		= linux openssl libnl network_turnkey iw
RTL8723BS_INSTALL_STAGING	= NO
RTL8723BS_INSTALL_IMAGES	= NO
RTL8723BS_INSTALL_TARGET	= YES

RTL8723BS_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8723BS_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8723BS_DRIVER_DIR	= $(@D)/driver/rtl8723BS_WiFi_linux_v4.3.16_13854.20150410_BTCOEX20150119-5844
RTL8723BS_DEST_DIR	= /usr/local/rtl8723bs
RTL8723BS_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8723BS_HOSTAPD	= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r11967.20140818/hostapd
RTL8723BS_WPA		= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r11967.20140818/wpa_supplicant


RTL8723BS_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
				KVER=$(RTL8723BS_KRNL_VERSION) \
				KSRC=$(LINUX_DIR) \
				MODDESTDIR=$(RTL8723BS_DEST_DIR)

RTL8723BS_AMBA_UTILITIES = load.sh unload.sh preload.sh wifi_softmac.sh


define RTL8723BS_CONFIGURE_DRIVER
	@if [ "$(BR2_PACKAGE_RTL8723BS_WOWLAN_EN)" == "y" ]; then \
		$(SED) "s/^CONFIG_WOWLAN = n/CONFIG_WOWLAN = y/" $(RTL8723BS_DRIVER_DIR)/Makefile; \
		$(SED) "s/^CONFIG_GPIO_WAKEUP = n/CONFIG_GPIO_WAKEUP = y/" $(RTL8723BS_DRIVER_DIR)/Makefile; \
	else \
		$(SED) "s/^CONFIG_WOWLAN = y/CONFIG_WOWLAN = n/" $(RTL8723BS_DRIVER_DIR)/Makefile; \
		$(SED) "s/^CONFIG_GPIO_WAKEUP = y/CONFIG_GPIO_WAKEUP = n/" $(RTL8723BS_DRIVER_DIR)/Makefile; \
	fi
	if [ "$(BR2_PACKAGE_RTL8723BS_REDUCE_TX_CPU_LOADING)" == "y" ]; then \
		$(SED) "s/^CONFIG_REDUCE_TX_CPU_LOADING = n/CONFIG_REDUCE_TX_CPU_LOADING = y/" $(RTL8723BS_DRIVER_DIR)/Makefile; \
	else \
		$(SED) "s/^CONFIG_REDUCE_TX_CPU_LOADING = y/CONFIG_REDUCE_TX_CPU_LOADING = n/" $(RTL8723BS_DRIVER_DIR)/Makefile; \
	fi
endef

# NOTE: For A7l, using CONFIG_LIBNL20=y; for A9, using libnl32
define RTL8723BS_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8723BS_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8723BS_WPA)/.config
endef

define RTL8723BS_CONFIGURE_CMDS
	$(RTL8723BS_CONFIGURE_DRIVER)
	$(RTL8723BS_CONFIGURE_WPA)
endef

define RTL8723BS_BUILD_CMDS
	@cd $(RTL8723BS_DRIVER_DIR) && \
		$(RTL8723BS_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8723BS_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8723BS_CFLAGS)" LDFLAGS="$(RTL8723BS_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8723BS_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8723BS_CFLAGS)" LDFLAGS="$(RTL8723BS_LDFLAGS)" \
			$(MAKE)
endef

define RTL8723BS_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/

	@cd $(RTL8723BS_DRIVER_DIR) && \
		cp -avf 8723bs.ko $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/8723bs.ko
	@ln -fs $(RTL8723BS_DEST_DIR)/8723bs.ko $(TARGET_DIR)/lib/modules/

	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8723BS_HOSTAPD)/$${i} $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/; \
		ln -fs $(RTL8723BS_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8723BS_WPA)/$${i} $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/; \
		ln -fs $(RTL8723BS_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$${i} $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/; \
		ln -fs $(RTL8723BS_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@cp -a $(@D)/firmware $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/
	@mkdir -p $(TARGET_DIR)/lib/firmware/
	@for i in `ls $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/firmware/`; do \
		ln -fs $(RTL8723BS_DEST_DIR)/firmware/$${i} $(TARGET_DIR)/lib/firmware/$${i}; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in $(RTL8723BS_AMBA_UTILITIES); do \
		cp -vf $(@D)/scripts/$${i} $(TARGET_DIR)/usr/local/share/script/; \
	done
	@$(SED) 's|^HCI_DRIVER=\(.*\)|HCI_DRIVER=rtk_h5|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
endef

define RTL8723BS_UNINSTALL_TARGET_CMDS
	@for i in `ls $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/firmware/`; do \
		rm -rf $(TARGET_DIR)/lib/firmware/$${i}; \
	done
	@rm -rf $(TARGET_DIR)/$(RTL8723BS_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/8723bs.ko

	@for i in hostapd hostapd_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in $(RTL8723BS_AMBA_UTILITIES); do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$${i}; \
	done
endef

define RTL8723BS_CLEAN_CMDS
	@cd $(RTL8723BS_DRIVER_DIR) && \
		$(RTL8723BS_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8723BS_HOSTAPD)/ && $(MAKE) clean
	@cd $(RTL8723BS_WPA)/ && $(MAKE) clean
	rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

