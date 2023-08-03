
RTL8812AU_VERSION		= amba
RTL8812AU_SITE_METHOD		= local
RTL8812AU_SITE			= ../external_sdk/realtek/rtl8812au
RTL8812AU_DEPENDENCIES		= linux openssl libnl iw
RTL8812AU_INSTALL_STAGING	= NO
RTL8812AU_INSTALL_IMAGES	= NO
RTL8812AU_INSTALL_TARGET	= YES

RTL8812AU_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8812AU_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8812AU_DRIVER_DIR	= $(@D)/driver/rtl8812AU_linux_v5.1.5_19247.20160830
RTL8812AU_MOD_NAME	= 8812au
RTL8812AU_DEST_DIR	= /usr/local/rtl$(RTL8812AU_MOD_NAME)
RTL8812AU_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8812AU_VER_SUPP	= wpa_supplicant_8_M_6.x_rtw_r17190.20160415
RTL8812AU_HOSTAPD	= $(@D)/utilities/$(RTL8812AU_VER_SUPP)/hostapd
RTL8812AU_WPA		= $(@D)/utilities/$(RTL8812AU_VER_SUPP)/wpa_supplicant


RTL8812AU_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
					KVER=$(RTL8812AU_KRNL_VERSION) \
					KSRC=$(LINUX_DIR) \
					MODDESTDIR=$(RTL8812AU_DEST_DIR) \
					ARCH=arm64


define RTL8812AU_CONFIGURE_DRIVER
endef

# NOTE: For A7l, using CONFIG_LIBNL20=y
define RTL8812AU_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8812AU_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8812AU_WPA)/.config
endef

define RTL8812AU_CONFIGURE_CMDS
	$(RTL8812AU_CONFIGURE_DRIVER)
	$(RTL8812AU_CONFIGURE_WPA)
endef

define RTL8812AU_BUILD_CMDS
	@cd $(RTL8812AU_DRIVER_DIR) && \
		$(RTL8812AU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8812AU_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8812AU_CFLAGS)" LDFLAGS="$(RTL8812AU_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8812AU_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8812AU_CFLAGS)" LDFLAGS="$(RTL8812AU_LDFLAGS)" \
			$(MAKE)
endef

define RTL8812AU_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/

	@cd $(RTL8812AU_DRIVER_DIR) && \
		cp -avf $(RTL8812AU_MOD_NAME).ko $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/$(RTL8812AU_MOD_NAME).ko
	@ln -fs $(RTL8812AU_DEST_DIR)/$(RTL8812AU_MOD_NAME).ko $(TARGET_DIR)/lib/modules/

	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8812AU_HOSTAPD)/$$i $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/; \
		ln -fs $(RTL8812AU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8812AU_WPA)/$$i $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/; \
		ln -fs $(RTL8812AU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$$i $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/; \
		ln -fs $(RTL8812AU_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load unload preload wifi_softmac; do \
		cp -avf $(@D)/scripts/$${i}.sh $(TARGET_DIR)/usr/local/share/script/$${i}.sh; \
	done
endef

define RTL8812AU_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(RTL8812AU_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/$(RTL8812AU_MOD_NAME).ko

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

define RTL8812AU_CLEAN_CMDS
	@cd $(RTL8812AU_DRIVER_DIR) && \
		$(RTL8812AU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8812AU_HOSTAPD) && $(MAKE) clean
	@cd $(RTL8812AU_WPA) && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

