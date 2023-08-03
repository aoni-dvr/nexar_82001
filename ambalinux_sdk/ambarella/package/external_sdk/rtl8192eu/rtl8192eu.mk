
RTL8192EU_VERSION		= amba
RTL8192EU_SITE_METHOD		= local
RTL8192EU_SITE			= ../external_sdk/realtek/rtl8192eu
RTL8192EU_DEPENDENCIES		= linux openssl libnl iw
RTL8192EU_INSTALL_STAGING	= NO
RTL8192EU_INSTALL_IMAGES	= NO
RTL8192EU_INSTALL_TARGET	= YES

RTL8192EU_CFLAGS		= $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/
RTL8192EU_LDFLAGS		= $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/

RTL8192EU_DRIVER_DIR	= $(@D)/driver/rtl8192EU_linux_v4.4.1.1_18873.20160805_BTCOEX20160412-0042
RTL8192EU_DEST_DIR	= /usr/local/rtl8192eu
RTL8192EU_KRNL_VERSION	= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
#RTL8192EU_VER_SUPP	= wpa_supplicant_8_kk_4.4_rtw_r10450.20140220
#RTL8192EU_VER_SUPP	= wpa_supplicant_8_kk_4.4_rtw_r16450.20151224
RTL8192EU_VER_SUPP	= wpa_supplicant_8_M_6.x_rtw_r17190.20160415
RTL8192EU_HOSTAPD	= $(@D)/utilities/$(RTL8192EU_VER_SUPP)/hostapd
RTL8192EU_WPA		= $(@D)/utilities/$(RTL8192EU_VER_SUPP)/wpa_supplicant


RTL8192EU_KRNL_ENV	= CROSS_COMPILE=$(TARGET_CROSS) \
					KVER=$(RTL8192EU_KRNL_VERSION) \
					KSRC=$(LINUX_DIR) \
					MODDESTDIR=$(RTL8192EU_DEST_DIR) \
					ARCH=arm64


define RTL8192EU_CONFIGURE_DRIVER
endef

define RTL8192EU_CONFIGURE_WPA
	echo "CONFIG_LIBNL32=y" >> $(RTL8192EU_HOSTAPD)/.config
	echo "CONFIG_LIBNL32=y" >> $(RTL8192EU_WPA)/.config
endef

define RTL8192EU_CONFIGURE_CMDS
	$(RTL8192EU_CONFIGURE_DRIVER)
	$(RTL8192EU_CONFIGURE_WPA)
endef

define RTL8192EU_BUILD_CMDS
	@cd $(RTL8192EU_DRIVER_DIR) && \
		$(RTL8192EU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(@D)/utilities/wireless_tools.30.rtl && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE)
	@cd $(RTL8192EU_HOSTAPD)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8192EU_CFLAGS)" LDFLAGS="$(RTL8192EU_LDFLAGS)" \
			$(MAKE)
	@cd $(RTL8192EU_WPA)/ && \
		$(MAKE) clean && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(RTL8192EU_CFLAGS)" LDFLAGS="$(RTL8192EU_LDFLAGS)" \
			$(MAKE)
endef

define RTL8192EU_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/

	@cd $(RTL8192EU_DRIVER_DIR) && \
		cp -avf 8192eu.ko $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/8192eu.ko
	@ln -fs $(RTL8192EU_DEST_DIR)/8192eu.ko $(TARGET_DIR)/lib/modules/

	@for i in hostapd hostapd_cli; do \
		cp -avf $(RTL8192EU_HOSTAPD)/$$i $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/; \
		ln -fs $(RTL8192EU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in wpa_supplicant wpa_cli; do \
		cp -avf $(RTL8192EU_WPA)/$$i $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/; \
		ln -fs $(RTL8192EU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@for i in iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename; do \
		cp -avf $(@D)/wireless_tools.30.rtl/$$i $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/; \
		ln -fs $(RTL8192EU_DEST_DIR)/$$i $(TARGET_DIR)/usr/sbin/; \
	done

	@mkdir -p $(TARGET_DIR)/usr/local/share/script
	@for i in load unload preload wifi_softmac; do \
		cp -avf $(@D)/scripts/$${i}.sh $(TARGET_DIR)/usr/local/share/script/$${i}.sh; \
	done
endef

define RTL8192EU_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(RTL8192EU_DEST_DIR)/

	@rm -f $(TARGET_DIR)/lib/modules/8192eu.ko

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

define RTL8192EU_CLEAN_CMDS
	@cd $(RTL8192EU_DRIVER_DIR) && \
		$(RTL8192EU_KRNL_ENV) $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) clean
	@cd $(@D)/wireless_tools.30.rtl && $(MAKE) clean
	@cd $(RTL8192EU_HOSTAPD) && $(MAKE) clean
	@cd $(RTL8192EU_WPA) && $(MAKE) clean
	@rm -f $(@D)/.stamp_built $(@D)/.stamp_target_installed
endef

$(eval $(generic-package))

