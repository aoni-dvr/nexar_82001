
RTL8723BU_VERSION		= amba
RTL8723BU_SITE_METHOD		= local
RTL8723BU_SITE			= ../external_sdk/realtek/rtl8723bu
RTL8723BU_DEPENDENCIES		= linux openssl libnl iw network_turnkey
RTL8723BU_INSTALL_STAGING	= NO
RTL8723BU_INSTALL_IMAGES	= NO
RTL8723BU_INSTALL_TARGET	= YES

RTL8723BU_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8723BU_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8723BU_DRIVER_DIR	= $(@D)/driver/rtl8723BU_WiFi_linux_v4.3.16_14189.20150519_BTCOEX20150119-5844
RTL8723BU_MOD_NAME	= 8723bu
RTL8723BU_DEST_DIR	= /usr/local/rtl$(RTL8723BU_MOD_NAME)
RTL8723BU_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8723BU_HOSTAPD	= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r12456.20141003/hostapd
RTL8723BU_WPA		= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r12456.20141003/wpa_supplicant
RTL8723BU_BTDRV_DIR	= $(@D)/driver/bluetooth_usb_driver


RTL8723BU_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
					ARCH=arm \
					KVER=$(RTL8723BU_KRNL_VERSION) \
					KSRC=$(LINUX_DIR) \
					MODDESTDIR=$(RTL8723BU_DEST_DIR)

define RTL8723BU_BT_BUILD_CMDS
	@cd $(RTL8723BU_BTDRV_DIR) && \
		$(RTL8723BU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
endef
define RTL8723BU_BT_INSTALL_TARGET_CMDS
	@cp -vf $(RTL8723BU_BTDRV_DIR)/rtk_btusb.ko $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/rtk_btusb.ko
	@cp -fv $(@D)/bt_fw/* $(TARGET_DIR)/lib/firmware/
	@$(SED) 's|^HCI_DRIVER=\(.*\)|HCI_DRIVER=rtk_btusb|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
endef

# NOTE: For A7l, using CONFIG_LIBNL20=y
define RTL8723BU_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8723BU_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8723BU_WPA)/.config
endef

define RTL8723BU_CONFIGURE_CMDS
	$(RTL8723BU_CONFIGURE_DRIVER)
	$(RTL8723BU_CONFIGURE_WPA)
endef

define RTL8723BU_BUILD_CMDS
	@cd $(RTL8723BU_DRIVER_DIR) && \
		$(RTL8723BU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8723BU_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8723BU_CFLAGS)" LDFLAGS="$(RTL8723BU_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8723BU_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8723BU_CFLAGS)" LDFLAGS="$(RTL8723BU_LDFLAGS)" \
			$(MAKE)
	$(RTL8723BU_BT_BUILD_CMDS)
endef

define RTL8723BU_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/

	@cd $(RTL8723BU_DRIVER_DIR) && \
		cp -avf $(RTL8723BU_MOD_NAME).ko $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/$(RTL8723BU_MOD_NAME).ko

	@ln -fs $(RTL8723BU_DEST_DIR)/$(RTL8723BU_MOD_NAME).ko $(TARGET_DIR)/lib/modules/

	@mkdir -p $(TARGET_DIR)/lib/firmware


	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8723BU_HOSTAPD)/$$i $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/; \
		ln -fs $(RTL8723BU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8723BU_WPA)/$$i $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/; \
		ln -fs $(RTL8723BU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$$i $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/; \
		ln -fs $(RTL8723BU_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load unload preload wifi_softmac; do \
		cp -avf $(@D)/scripts/$${i}.sh $(TARGET_DIR)/usr/local/share/script/$${i}.sh; \
	done

	$(RTL8723BU_BT_INSTALL_TARGET_CMDS)
endef

define RTL8723BU_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(RTL8723BU_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/$(RTL8723BU_MOD_NAME).ko
	@rm -f $(TARGET_DIR)/lib/firmware/rtl8821a_fw

	@for i in hostapd hostapd_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for i in load.sh unload.sh preload.sh wifi_softmac.sh; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$${i}; \
	done
endef

define RTL8723BU_CLEAN_CMDS
	@cd $(RTL8723BU_DRIVER_DIR) && \
		$(RTL8723BU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(RTL8723BU_BTDRV_DIR) && \
		$(RTL8723BU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8723BU_HOSTAPD) && $(MAKE) clean
	@cd $(RTL8723BU_WPA) && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

