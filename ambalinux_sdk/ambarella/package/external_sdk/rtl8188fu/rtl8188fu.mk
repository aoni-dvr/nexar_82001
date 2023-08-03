
RTL8188FU_VERSION		= amba
RTL8188FU_SITE_METHOD		= local
RTL8188FU_SITE			= ../external_sdk/realtek/rtl8188fu
RTL8188FU_DEPENDENCIES		= linux openssl libnl iw
RTL8188FU_INSTALL_STAGING	= NO
RTL8188FU_INSTALL_IMAGES	= NO
RTL8188FU_INSTALL_TARGET	= YES

RTL8188FU_CFLAGS	= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8188FU_LDFLAGS	= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8188FU_DRIVER_DIR	= $(@D)/driver/rtl8188FU_linux_v4.3.23.4_19019.20160811
RTL8188FU_MOD_NAME	= 8188fu
RTL8188FU_DEST_DIR	= /usr/local/rtl$(RTL8188FU_MOD_NAME)
RTL8188FU_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
RTL8188FU_VER_SUPP	= wpa_supplicant_8_M_6.x_rtw_r17190.20160415
RTL8188FU_HOSTAPD	= $(@D)/utilities/$(RTL8188FU_VER_SUPP)/hostapd
RTL8188FU_WPA		= $(@D)/utilities/$(RTL8188FU_VER_SUPP)/wpa_supplicant


RTL8188FU_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
					KVER=$(RTL8188FU_KRNL_VERSION) \
					KSRC=$(LINUX_DIR) \
					MODDESTDIR=$(RTL8188FU_DEST_DIR) \
					ARCH=arm64


define RTL8188FU_CONFIGURE_DRIVER
endef

# NOTE: For A7l, using CONFIG_LIBNL20=y
define RTL8188FU_CONFIGURE_WPA
	@echo "CONFIG_LIBNL32=y" >> $(RTL8188FU_HOSTAPD)/.config
	@echo "CONFIG_LIBNL32=y" >> $(RTL8188FU_WPA)/.config
endef

define RTL8188FU_CONFIGURE_CMDS
	$(RTL8188FU_CONFIGURE_DRIVER)
	$(RTL8188FU_CONFIGURE_WPA)
endef

define RTL8188FU_BUILD_CMDS
	@cd $(RTL8188FU_DRIVER_DIR) && \
		$(RTL8188FU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8188FU_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8188FU_CFLAGS)" LDFLAGS="$(RTL8188FU_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8188FU_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8188FU_CFLAGS)" LDFLAGS="$(RTL8188FU_LDFLAGS)" \
			$(MAKE)
endef

define RTL8188FU_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/

	@cd $(RTL8188FU_DRIVER_DIR) && \
		cp -avf $(RTL8188FU_MOD_NAME).ko $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/$(RTL8188FU_MOD_NAME).ko
	@ln -fs $(RTL8188FU_DEST_DIR)/$(RTL8188FU_MOD_NAME).ko $(TARGET_DIR)/lib/modules/

	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8188FU_HOSTAPD)/$$i $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/; \
		ln -fs $(RTL8188FU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8188FU_WPA)/$$i $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/; \
		ln -fs $(RTL8188FU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/utilities/wireless_tools.30.rtl/$$i $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/; \
		ln -fs $(RTL8188FU_DEST_DIR)/$${i} $(TARGET_DIR)/usr/sbin/; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load unload preload wifi_softmac; do \
		cp -avf $(@D)/scripts/$${i}.sh $(TARGET_DIR)/usr/local/share/script/$${i}.sh; \
	done
endef

define RTL8188FU_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(RTL8188FU_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/$(RTL8188FU_MOD_NAME).ko

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

define RTL8188FU_CLEAN_CMDS
	@cd $(RTL8188FU_DRIVER_DIR) && \
		$(RTL8188FU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/utilities/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8188FU_HOSTAPD) && $(MAKE) clean
	@cd $(RTL8188FU_WPA) && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

