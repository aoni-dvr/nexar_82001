AR6003_3_1_568_VERSION		= amba
AR6003_3_1_568_SITE_METHOD	= local
AR6003_3_1_568_SITE		= ../external_sdk/atheros/ar6003_3_1_568
AR6003_3_1_568_DEPENDENCIES	= linux openssl
AR6003_3_1_568_INSTALL_STAGING	= NO
AR6003_3_1_568_INSTALL_IMAGES	= NO
AR6003_3_1_568_INSTALL_TARGET	= YES

ifeq ($(BR2_TOOLCHAIN_EXTERNAL_LINARO_ARM),y)
AR6003_3_1_568_POST		= .linaro
else
AR6003_3_1_568_POST		=
endif

export ATH_CROSS_COMPILE_TYPE=$(TARGET_CROSS)
export WORKAREA=$(@D)
export KDIR=$(LINUX_DIR)
export TDIR=$(TARGET_DIR)

#AR6003_3_1_568_ENV	= WORKAREA=$(@D) KDIR=$(LINUX_DIR) TDIR=$(TARGET_DIR)
#	ATH_LINUXPATH=$(LINUX_DIR) ATH_CROSS_COMPILE_TYPE=$(TARGET_CROSS) \
#	ATH_ARCH_CPU_TYPE=arm ATH_ANDROID_ENV=yes ATH_BUILD_P2P=yes \
#	ATH_BUS_SUBTYPE=linux_sdio ATH_AR6K_HCI_BRIDGE=no \
#	ATH_BUILD_TYPE=LOCAL_i686_NATIVEMMC ATH_BUS_TYPE=SDIO \
#	ATH_OS_SUB_TYPE=linux_2_6 ATH_BUILD_SYSTEM_TOOLS=no \
#	ATH_BUILD_BTFILTER=no NETIF=wlan0 ATH_BUILD_PAL_APP=no \
#	ATH_AR6K_HCI_PAL=no ATH_BUILD_3RDPARTY=yes
#	ATH_BUS_DRIVER_OPTIONS="DefaultOperClock=50000000" \
#	ATH_BTHCI_ARGS="ar3khcibaud=3000000 hciuartscale=1 hciuartstep=8937" \

define AR6003_3_1_568_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/host
#	$(MAKE) -C $(@D)/host/tools/tcmd/
endef

define AR6003_3_1_568_INSTALL_TARGET_CMDS
	$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/host install

	mkdir -p ${TARGET_DIR}/usr/local/ath6k/target/AR6003/hw2.1.1/bin/
	cp -av $(@D)/target/AR6003/hw2.1.1/bin/*.bin ${TARGET_DIR}/usr/local/ath6k/target/AR6003/hw2.1.1/bin/
	for i in wmiconfig athtestcmd; do \
		cp -av $(@D)/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image$(AR6003_3_1_568_POST)/$$i $(TARGET_DIR)/usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/; \
		ln -fs /usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/$$i ${TARGET_DIR}/usr/sbin; \
	done

	mkdir -p ${TARGET_DIR}/usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/
	for i in hostapd hostapd_cli wpa_supplicant wpa_cli wmiconfig athtestcmd; do \
		cp -av $(@D)/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/$$i $(TARGET_DIR)/usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/; \
		ln -fs /usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/$$i ${TARGET_DIR}/usr/sbin; \
	done

	cp -av $(@D)/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/ar6000.ko $(TARGET_DIR)/usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/ar6000.ko
	${STRIP} --strip-unneeded $(TARGET_DIR)/usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/ar6000.ko
	ln -fs /usr/local/ath6k/host/.output/LOCAL_i686_NATIVEMMC-SDIO/image/ar6000.ko ${TARGET_DIR}/lib/modules

	mkdir -p ${TARGET_DIR}/usr/local/share/script
	for i in load.sh unload.sh; do \
	cp -av $(@D)/host/support/$$i ${TARGET_DIR}/usr/local/share/script/; \
	done
endef

define AR6003_3_1_568_UNINSTALL_TARGET_CMDS
	rm -rf ${TARGET_DIR}/usr/local/ath6k
	rm -f ${TARGET_DIR}/lib/modules/ar6000.ko

	for i in hostapd hostapd_cli wpa_supplicant wpa_cli wmiconfig athtestcmd; do \
		rm -f ${TARGET_DIR}/usr/sbin/$$i; \
	done

	for i in load.sh unload.sh preload.sh; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$$i; \
	done
endef

$(eval $(generic-package))
