
RTL8821AU_VERSION		= amba
RTL8821AU_SITE_METHOD		= local
RTL8821AU_SITE			= ../external_sdk/realtek/rtl8821au
RTL8821AU_DEPENDENCIES		= linux openssl libnl iw
RTL8821AU_INSTALL_STAGING	= NO
RTL8821AU_INSTALL_IMAGES	= NO
RTL8821AU_INSTALL_TARGET	= YES

RTL8821AU_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8821AU_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8821AU_DRIVER_DIR	= $(@D)/driver/rtl8821AU_linux_v4.3.14_13455.20150212_BTCOEX20150128-51
RTL8821AU_MOD_NAME	= 8821au
RTL8821AU_DEST_DIR	= /usr/local/rtl$(RTL8821AU_MOD_NAME)
RTL8821AU_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8821AU_HOSTAPD	= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r11967.20140818/hostapd
RTL8821AU_WPA		= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r11967.20140818/wpa_supplicant
RTL8821AU_BTDRV_DIR	= $(@D)/driver/bluetooth_usb_driver


RTL8821AU_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
					ARCH=arm \
					KVER=$(RTL8821AU_KRNL_VERSION) \
					KSRC=$(LINUX_DIR) \
					MODDESTDIR=$(RTL8821AU_DEST_DIR)


define RTL8821AU_CONFIGURE_DRIVER
endef

ifeq ($(BR2_PACKAGE_RTL8821AU_BT),y)
define RTL8821AU_BT_BUILD_CMDS
	@cd $(RTL8821AU_BTDRV_DIR) && \
		$(RTL8821AU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
endef
define RTL8821AU_BT_INSTALL_TARGET_CMDS
	@cp -vf $(RTL8821AU_BTDRV_DIR)/rtk_btusb.ko $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/rtk_btusb.ko
	@cp -fv $(@D)/bt_fw/rtl8821a_fw $(TARGET_DIR)/lib/firmware/
endef
endif

# NOTE: For A7l, using CONFIG_LIBNL20=y
define RTL8821AU_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8821AU_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8821AU_WPA)/.config
endef

define RTL8821AU_CONFIGURE_CMDS
	$(RTL8821AU_CONFIGURE_DRIVER)
	$(RTL8821AU_CONFIGURE_WPA)
endef

define RTL8821AU_BUILD_CMDS
	@cd $(RTL8821AU_DRIVER_DIR) && \
		$(RTL8821AU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8821AU_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8821AU_CFLAGS)" LDFLAGS="$(RTL8821AU_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8821AU_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8821AU_CFLAGS)" LDFLAGS="$(RTL8821AU_LDFLAGS)" \
			$(MAKE)
	$(RTL8821AU_BT_BUILD_CMDS)
endef

define RTL8821AU_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/

	@cd $(RTL8821AU_DRIVER_DIR) && \
		cp -avf $(RTL8821AU_MOD_NAME).ko $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/$(RTL8821AU_MOD_NAME).ko

	@ln -fs $(RTL8821AU_DEST_DIR)/$(RTL8821AU_MOD_NAME).ko $(TARGET_DIR)/lib/modules/

	@mkdir -p $(TARGET_DIR)/lib/firmware


	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8821AU_HOSTAPD)/$$i $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/; \
		ln -fs $(RTL8821AU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8821AU_WPA)/$$i $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/; \
		ln -fs $(RTL8821AU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$$i $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/; \
		ln -fs $(RTL8821AU_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load unload preload wifi_softmac; do \
		cp -avf $(@D)/scripts/$${i}.sh $(TARGET_DIR)/usr/local/share/script/$${i}.sh; \
	done

	$(RTL8821AU_BT_INSTALL_TARGET_CMDS)
endef

define RTL8821AU_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(RTL8821AU_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/$(RTL8821AU_MOD_NAME).ko
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

define RTL8821AU_CLEAN_CMDS
	@cd $(RTL8821AU_DRIVER_DIR) && \
		$(RTL8821AU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(RTL8821AU_BTDRV_DIR) && \
		$(RTL8821AU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8821AU_HOSTAPD) && $(MAKE) clean
	@cd $(RTL8821AU_WPA) && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

