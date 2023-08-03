
RTL8189ES_VERSION		= amba
RTL8189ES_SITE_METHOD		= local
RTL8189ES_SITE			= ../external_sdk/realtek/rtl8189es
RTL8189ES_DEPENDENCIES		= linux openssl libnl iw
RTL8189ES_INSTALL_STAGING	= NO
RTL8189ES_INSTALL_IMAGES	= NO
RTL8189ES_INSTALL_TARGET	= YES

RTL8189ES_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8189ES_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8189ES_DRIVER_DIR	= $(@D)/driver/rtl8189ES_linux_v4.3.18.1_15373.20151005
RTL8189ES_DEST_DIR	= /usr/local/rtl8189es
RTL8189ES_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8189ES_HOSTAPD	= $(@D)/utilities/wpa_supplicant_8_L_5.x_rtw_r14967.20150811/hostapd
RTL8189ES_WPA		= $(@D)/utilities/wpa_supplicant_8_L_5.x_rtw_r14967.20150811/wpa_supplicant


RTL8189ES_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
				KVER=$(RTL8189ES_KRNL_VERSION) \
				KSRC=$(LINUX_DIR) \
				MODDESTDIR=$(RTL8189ES_DEST_DIR)

RTL8189ES_AMBA_UTILITIES = load.sh unload.sh preload.sh wifi_softmac.sh


define RTL8189ES_CONFIGURE_DRIVER
	@if [ "$(BR2_PACKAGE_RTL8189ES_WOWLAN_EN)" == "y" ]; then \
		$(SED) "s/^CONFIG_WOWLAN = n/CONFIG_WOWLAN = y/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
		$(SED) "s/^CONFIG_GPIO_WAKEUP = n/CONFIG_GPIO_WAKEUP = y/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
		$(SED) "s/^CONFIG_AP_WOWLAN = n/CONFIG_AP_WOWLAN = y/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
	else \
		$(SED) "s/^CONFIG_WOWLAN = y/CONFIG_WOWLAN = n/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
		$(SED) "s/^CONFIG_GPIO_WAKEUP = y/CONFIG_GPIO_WAKEUP = n/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
		$(SED) "s/^CONFIG_AP_WOWLAN = y/CONFIG_AP_WOWLAN = n/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
	fi
	if [ "$(BR2_PACKAGE_RTL8189ES_REDUCE_TX_CPU_LOADING)" == "y" ]; then \
		$(SED) "s/^CONFIG_REDUCE_TX_CPU_LOADING = n/CONFIG_REDUCE_TX_CPU_LOADING = y/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
	else \
		$(SED) "s/^CONFIG_REDUCE_TX_CPU_LOADING = y/CONFIG_REDUCE_TX_CPU_LOADING = n/" $(RTL8189ES_DRIVER_DIR)/Makefile; \
	fi
endef

define RTL8189ES_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8189ES_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8189ES_WPA)/.config
endef

define RTL8189ES_CONFIGURE_CMDS
	$(RTL8189ES_CONFIGURE_DRIVER)
	$(RTL8189ES_CONFIGURE_WPA)
endef

define RTL8189ES_BUILD_CMDS
	@cd $(RTL8189ES_DRIVER_DIR) && \
		$(RTL8189ES_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8189ES_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8189ES_CFLAGS)" LDFLAGS="$(RTL8189ES_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8189ES_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8189ES_CFLAGS)" LDFLAGS="$(RTL8189ES_LDFLAGS)" \
			$(MAKE)
endef

define RTL8189ES_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/

	@cd $(RTL8189ES_DRIVER_DIR) && \
		cp -avf 8189es.ko $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/8189es.ko
	@ln -fs $(RTL8189ES_DEST_DIR)/8189es.ko $(TARGET_DIR)/lib/modules/

	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8189ES_HOSTAPD)/$${i} $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/; \
		ln -fs $(RTL8189ES_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8189ES_WPA)/$${i} $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/; \
		ln -fs $(RTL8189ES_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$${i} $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/; \
		ln -fs $(RTL8189ES_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in $(RTL8189ES_AMBA_UTILITIES); do \
		cp -vf $(@D)/scripts/$${i} $(TARGET_DIR)/usr/local/share/script/; \
	done
endef

define RTL8189ES_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(RTL8189ES_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/8189es.ko

	@for i in hostapd hostapd_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in $(RTL8189ES_AMBA_UTILITIES); do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$${i}; \
	done
endef

define RTL8189ES_CLEAN_CMDS
	@cd $(RTL8189ES_DRIVER_DIR) && \
		$(RTL8189ES_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8189ES_HOSTAPD) && $(MAKE) clean
	@cd $(RTL8189ES_WPA) && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

