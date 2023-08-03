
RTL8821AS_VERSION		= amba
RTL8821AS_SITE_METHOD		= local
RTL8821AS_SITE			= ../external_sdk/realtek/rtl8821as
RTL8821AS_DEPENDENCIES		= linux openssl libnl iw
RTL8821AS_INSTALL_STAGING	= NO
RTL8821AS_INSTALL_IMAGES	= NO
RTL8821AS_INSTALL_TARGET	= YES

RTL8821AS_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8821AS_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8821AS_DRIVER_DIR	= $(@D)/driver/rtl8821AS_linux_v4.3.7_11952.20140730_BTCOEX20130816-4150
RTL8821AS_MOD_NAME	= 8821as
RTL8821AS_DEST_DIR	= /usr/local/rtl$(RTL8821AS_MOD_NAME)
RTL8821AS_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8821AS_HOSTAPD	= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r10450.20140220/hostapd
RTL8821AS_WPA		= $(@D)/utilities/wpa_supplicant_8_kk_4.4_rtw_r10450.20140220/wpa_supplicant
#RTL8821AS_HOSTAPD	= $(@D)/utilities/wpa_supplicant_8_jb_4.2_rtw_r8680.20130821/hostapd
#RTL8821AS_WPA		= $(@D)/utilities/wpa_supplicant_8_jb_4.2_rtw_r8680.20130821/wpa_supplicant


RTL8821AS_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
					KVER=$(RTL8821AS_KRNL_VERSION) \
					KSRC=$(LINUX_DIR) \
					MODDESTDIR=$(RTL8821AS_DEST_DIR)


define RTL8821AS_CONFIGURE_DRIVER
endef

# NOTE: For A7l, using CONFIG_LIBNL20=y
define RTL8821AS_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8821AS_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8821AS_WPA)/.config
endef

define RTL8821AS_CONFIGURE_CMDS
	$(RTL8821AS_CONFIGURE_DRIVER)
	$(RTL8821AS_CONFIGURE_WPA)
endef

define RTL8821AS_BUILD_CMDS
	@cd $(RTL8821AS_DRIVER_DIR) && \
		$(RTL8821AS_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8821AS_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8821AS_CFLAGS)" LDFLAGS="$(RTL8821AS_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8821AS_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8821AS_CFLAGS)" LDFLAGS="$(RTL8821AS_LDFLAGS)" \
			$(MAKE)
endef

define RTL8821AS_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/

	@cd $(RTL8821AS_DRIVER_DIR) && \
		cp -avf $(RTL8821AS_MOD_NAME).ko $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/$(RTL8821AS_MOD_NAME).ko
	@ln -fs $(RTL8821AS_DEST_DIR)/$(RTL8821AS_MOD_NAME).ko $(TARGET_DIR)/lib/modules/

	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8821AS_HOSTAPD)/$${i} $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/; \
		ln -fs $(RTL8821AS_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8821AS_WPA)/$$i $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/; \
		ln -fs $(RTL8821AS_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$$i $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/; \
		ln -fs $(RTL8821AS_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@cp -a $(@D)/firmware $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/
	@mkdir -p $(TARGET_DIR)/lib/firmware/
	@for i in `ls $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/firmware/`; do \
		ln -fs $(RTL8821AS_DEST_DIR)/firmware/$${i} $(TARGET_DIR)/lib/firmware/$${i}; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load unload preload wifi_softmac; do \
		cp -avf $(@D)/scripts/$${i}.sh $(TARGET_DIR)/usr/local/share/script/$${i}.sh; \
	done
	@$(SED) 's|^HCI_DRIVER=\(.*\)|HCI_DRIVER=rtk_h5|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
endef

define RTL8821AS_UNINSTALL_TARGET_CMDS
	@for i in `ls $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/firmware/`; do \
		rm -rf $(TARGET_DIR)/lib/firmware/$${i}; \
	done
	@rm -rf $(TARGET_DIR)/$(RTL8821AS_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/$(RTL8821AS_MOD_NAME).ko

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

define RTL8821AS_CLEAN_CMDS
	@cd $(RTL8821AS_DRIVER_DIR) && \
		$(RTL8821AS_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8821AS_HOSTAPD)/ && $(MAKE) clean
	@cd $(RTL8821AS_WPA)/ && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

