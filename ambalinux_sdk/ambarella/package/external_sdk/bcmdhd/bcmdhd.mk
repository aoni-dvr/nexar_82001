BCMDHD_VERSION		= amba
BCMDHD_SITE_METHOD	= local
BCMDHD_SITE		= ../external_sdk/broadcom/bcmdhd
BCMDHD_DEPENDENCIES	= linux network_turnkey openssl libnl iw
BCMDHD_INSTALL_STAGING	= NO
BCMDHD_INSTALL_IMAGES	= NO
BCMDHD_INSTALL_TARGET	= YES

BCMDHD_DEBUG		= NO
BCMDHD_BUILD_WL		= NO

BCMDHD_DRIVER_AMPAK	= YES

# Module Type
ifeq ($(BR2_PACKAGE_BCMDHD_43340),y)
BCMDHD_MTYPE		= bcm43340
BCMDHD_BT_FUN		= YES
BCMDHD_FW_CONFIG	= fw_bcm43341b0_ag_apsta.bin
else ifeq ($(BR2_PACKAGE_BCMDHD_43436),y)
BCMDHD_MTYPE		= bcm43436
BCMDHD_BT_FUN		= YES
BCMDHD_FW_CONFIG	= fw_bcm43436b0_apsta.bin
else ifeq ($(BR2_PACKAGE_BCMDHD_43438),y)
BCMDHD_MTYPE		= bcm43438
BCMDHD_BT_FUN		= YES
ifeq ($(BR2_PACKAGE_BCMDHD_43438_A0),y)
BCMDHD_FW_CONFIG	= fw_bcm43438a0_apsta.bin
else
BCMDHD_FW_CONFIG	= fw_bcm43438a1_apsta.bin
endif
else ifeq ($(BR2_PACKAGE_BCMDHD_43455),y)
BCMDHD_MTYPE		= bcm43455
BCMDHD_BT_FUN		= YES
BCMDHD_FW_CONFIG	= fw_bcm43455c0_ag_apsta.bin
else ifeq ($(BR2_PACKAGE_BCMDHD_43596),y)
BCMDHD_MTYPE		= bcm43596
BCMDHD_BT_FUN		= NO
BCMDHD_FW_CONFIG	= fw_bcm4359c0_ag_apsta.bin
BCMDHD_DRIVER_AMPAK	:= YES
else ifeq ($(BR2_PACKAGE_BCMDHD_43456),y)
BCMDHD_MTYPE		= bcm43456
BCMDHD_BT_FUN		= YES
BCMDHD_FW_CONFIG	= fw_bcm43456c5_ag_apsta.bin
BCMDHD_DRIVER_AMPAK	:= YES
else
ifeq ($(BR2_PACKAGE_BCMDHD),y)
  $(error Unknown module type!)
endif
endif

# Vendor/Maker
ifeq ($(BR2_PACKAGE_BCMDHD_AW),y)
BCMDHD_MVENDOR		= aw
else ifeq ($(BR2_PACKAGE_BCMDHD_AMPAK),y)
BCMDHD_MVENDOR		= ampak
else ifeq ($(BR2_PACKAGE_BCMDHD_USI),y)
BCMDHD_MVENDOR		= usi
else
ifeq ($(BR2_PACKAGE_BCMDHD),y)
  $(error Unknown module vendor/maker!)
endif
endif

ifeq ($(BCMDHD_DRIVER_AMPAK),YES)
BCMDHD_DRV_DIR		= $(@D)/driver/bcmdhd.1.579.77.41.x.ambatw
else
BCMDHD_DRV_DIR		= $(@D)/driver/v1.363.59.154
endif

ifeq ($(BR2_arm),y)
BCMDHD_DRV_MKTARGET	= dhd-cdc-sdmmc-android-cfg80211-p2p-swtxglom-ambarella-armv7l
else
BCMDHD_DRV_MKTARGET	= dhd-cdc-sdmmc-android-cfg80211-p2p-swtxglom-ambarella
endif

ifeq ($(BCMDHD_DEBUG),YES)
BCMDHD_DRV_MKTARGET	:= $(addsuffix -debug,$(BCMDHD_DRV_MKTARGET))
endif

BCMDHD_FW_DIR		= $(@D)/fw/$(BCMDHD_MTYPE)
ifeq ($(BR2_PACKAGE_BCMDHD_43438),y)
ifeq ($(BR2_PACKAGE_BCMDHD_43438_A0),y)
BCMDHD_FW_NAME		= fw_apsta.a0.bin
BCMDHD_NVRAM_NAME	= nvram.a0.txt
BCMDHD_BT_HCD_NAME	= bt.a0.hcd
else
BCMDHD_FW_NAME		= fw_apsta.a1.bin
ifeq ($(BR2_PACKAGE_BCMDHD_43438_A1_WOT),y)
BCMDHD_NVRAM_NAME	= nvram_ap6212sd.txt
else
ifeq ($(BR2_PACKAGE_BCMDHD_43438_A1_GPS),y)
BCMDHD_NVRAM_NAME	= nvram_ap6474.txt
else
BCMDHD_NVRAM_NAME	= nvram.a1.txt
endif # BR2_PACKAGE_BCMDHD_43438_A1_GPS
endif # BR2_PACKAGE_BCMDHD_43438_A1_WOT
BCMDHD_BT_HCD_NAME	= bt.a1.hcd
endif # BR2_PACKAGE_BCMDHD_43438_A0/A1
else
ifeq ($(BR2_PACKAGE_BCMDHD_43340_EXP),y)
BCMDHD_FW_NAME		= fw_bcm43341b0_ag-rxglom.bin
else ifeq ($(BR2_PACKAGE_BCMDHD_43596),y)
BCMDHD_FW_NAME		= fw_bcm4359c0_ag_apsta.bin
else ifeq ($(BR2_PACKAGE_BCMDHD_43436),y)
BCMDHD_FW_NAME		= fw_bcm43436b0_apsta.bin
else ifeq ($(BR2_PACKAGE_BCMDHD_43456),y)
BCMDHD_FW_NAME		= fw_bcm43456c5_ag_apsta.bin
BCMDHD_MFG_FW_NAME  = fw_bcm43456c5_ag_mfg.bin
else
BCMDHD_FW_NAME		= fw_apsta.bin
endif
ifeq ($(BR2_PACKAGE_BCMDHD_43436),y)
BCMDHD_NVRAM_NAME	= nvram_ap6236.txt
BCMDHD_BT_HCD_NAME	= BCM4343B0.hcd
else
BCMDHD_NVRAM_NAME	= nvram.txt
BCMDHD_BT_HCD_NAME	= bt.hcd
endif
endif

BCMDHD_NVRAM_DIR	= $(@D)/nvram/$(BCMDHD_MVENDOR)/$(BCMDHD_MTYPE)
BCMDHD_DEST_DIR		= /usr/local/bcmdhd

BCMDHD_WL_TOOL_SRCDIR	= $(@D)/wl_tool/src/wl/exe
BCMDHD_WL_TOOL_BINDIR	= $(@D)/wl_tool/bin

ifeq ($(BR2_TOOLCHAIN_EXTERNAL_LINARO_ARM),y)
BCMDHD_WL_POST		= .linaro
else
BCMDHD_WL_POST		=
endif

ifeq ($(BCMDHD_BUILD_WL),YES)
BCMDHD_WL_TOOL		= wl
else
ifeq ($(BR2_ARCH_IS_64),y)
BCMDHD_WL_TOOL		= wl.arm64
else ifeq ($(BR2_PACKAGE_BCMDHD_43455),y)
BCMDHD_WL_TOOL		= wl.bcm43455
else
ifeq ($(BR2_PACKAGE_BCMDHD_43438),y)
BCMDHD_WL_TOOL		= wl.bis$(BCMDHD_WL_POST)
else
BCMDHD_WL_TOOL		= wl.pho$(BCMDHD_WL_POST)
endif
endif

endif # BCMDHD_BUILD_WL

ifeq ($(BR2_PACKAGE_BCMDHD_43438_A1_GPS),y)
BCMDHD_GPS		= YES
endif

BCMDHD_BT_TOOL_SRCDIR	= $(@D)/bt_tool/
BCMDHD_BT_FW_DIR	= $(@D)/bt_fw/$(BCMDHD_MVENDOR)/$(BCMDHD_MTYPE)
BCMDHD_GPS_DIR		= $(@D)/utilities/dasheng
# Name of patch code for GPS module
BCMDHD_GPS_PCNAME	= SensorHub_115200.patch

BCMDHD_KVER		= $(shell cat $(LINUX_DIR)/include/config/kernel.release)

BCMDHD_KO_NAME		= bcmdhd.ko

ifeq ($(BCMDHD_DRIVER_AMPAK),YES)
BCMDHD_KO_DIR		= $(BCMDHD_DRV_DIR)
else
BCMDHD_KO_DIR		= $(BCMDHD_DRV_DIR)/open-src/src/dhd/linux/$(BCMDHD_DRV_MKTARGET)-$(BCMDHD_KVER)
endif

BCMDHD_DRV_MKOPTION	= CROSS_COMPILE=$(TARGET_CROSS) \
			  LINUXDIR=$(LINUX_DIR) \
			  LINUXVER=$(BCMDHD_KVER)

ifeq ($(BR2_PACKAGE_BCMDHD_AUTOFW),y)
DFLAGS := -DAMBA_AUTOFW
endif

ifeq ($(BR2_PACKAGE_BCMDHD_4334),y)
BCMDHD_DRV_MKOPTION += MY_C_DEFINES=-DBCM4334_CHIP
endif
ifeq ($(BR2_ARCH_IS_64),y)
BCMDHD_DRV_MKOPTION += ARCH=arm64
else
BCMDHD_DRV_MKOPTION += ARCH=arm
endif

BCMDHD_AMBA_UTILITIES = load.sh unload.sh


define BCMDHD_CONFIGURE_DRIVER
endef

#define BCMDHD_CONFIGURE_SCRIPTS
#	@$(SED) 's/KO=bcmdhd/KO=dhd/' $(@D)/scripts/load.sh
#endef

define BCMDHD_CONFIGURE_CMDS
	$(BCMDHD_CONFIGURE_DRIVER)
	$(BCMDHD_CONFIGURE_SCRIPTS)
endef

ifeq ($(BCMDHD_DRIVER_AMPAK),YES)
define BCMDHD_BUILD_DRIVER
	@cd $(BCMDHD_DRV_DIR)/ && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(BCMDHD_DRV_MKOPTION) \
		$(shell which make)
endef
else
define BCMDHD_BUILD_DRIVER
	@cd $(BCMDHD_DRV_DIR)/open-src/src/dhd/linux/ && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(BCMDHD_DRV_MKOPTION) \
		TARGET=$(BCMDHD_DRV_MKTARGET) \
		DFLAGS=$(DFLAGS) \
		$(shell which make) $(BCMDHD_DRV_MKTARGET)
endef
endif

ifeq ($(BCMDHD_BUILD_WL),YES)
define BCMDHD_BUILD_WL_TOOL
	@cd $(BCMDHD_WL_TOOL_SRCDIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(BCMDHD_DRV_MKOPTION) \
		HOSTENV=linux HOSTOS=unix \
		TARGETENV=linuxarm_le TARGETOS=unix \
		TARGETARCH=arm_le \
		$(MAKE) all && \
		cp wlarm_le $(BCMDHD_WL_TOOL_BINDIR)/wl
endef

define BCMDHD_CLEAN_WL_TOOL
	@cd $(BCMDHD_WL_TOOL_SRCDIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(BCMDHD_DRV_MKOPTION) \
		HOSTENV=linux HOSTOS=unix \
		TARGETENV=linuxarm_le TARGETOS=unix \
		TARGETARCH=arm_le \
		$(MAKE) clean
endef
endif

ifeq ($(BCMDHD_BT_FUN),YES)
define BCMDHD_BUILD_BT_TOOL
	@cd $(BCMDHD_BT_TOOL_SRCDIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(BCMDHD_DRV_MKOPTION) \
		$(MAKE)
endef
endif

ifeq ($(BCMDHD_GPS),YES)
define BCMDHD_BUILD_GPS
	@cd $(BCMDHD_GPS_DIR)/ && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(TARGET_CFLAGS)" \
			LDFLAGS="$(TARGET_LDFLAGS)" \
			$(MAKE)
endef # BCMDHD_BUILD_GPS
endif

ifeq ($(BR2_PACKAGE_BCMDHD_43596),y)
define BCMDHD_BUILD_UTILS
	@cd $(@D)/utilities/dhd_priv/ && \
		CROSS_COMPILE=$(TARGET_CROSS) \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
			CFLAGS="$(TARGET_CFLAGS)" \
			LDFLAGS="$(TARGET_LDFLAGS)" \
			$(MAKE)
endef # BCMDHD_BUILD_UTILS
endif

define BCMDHD_BUILD_CMDS
	$(BCMDHD_BUILD_DRIVER)
	$(BCMDHD_BUILD_WL_TOOL)
	$(BCMDHD_BUILD_BT_TOOL)
	$(BCMDHD_BUILD_GPS)
	$(BCMDHD_BUILD_UTILS)
endef

ifeq ($(BCMDHD_BT_FUN),YES)
define BCMDHD_INSTALL_BT
	@cp -avf $(BCMDHD_BT_TOOL_SRCDIR)/brcm_patchram_plus $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/
	@$(STRIPCMD) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/brcm_patchram_plus
	@ln -fs $(BCMDHD_DEST_DIR)/brcm_patchram_plus $(TARGET_DIR)/usr/sbin/
	@cp -avf $(BCMDHD_BT_FW_DIR)/$(BCMDHD_BT_HCD_NAME) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/bt.hcd
endef
endif

ifeq ($(BCMDHD_GPS),YES)
define BCMDHD_INSTALL_GPS
	@cd $(BCMDHD_GPS_DIR)/ && \
		cp -avf dasheng $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/ && \
		cp -avf $(BCMDHD_GPS_PCNAME) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/
endef
endif

ifeq ($(BR2_PACKAGE_BCMDHD_43596),y)
define BCMDHD_INSTALL_UTILS
	@cd $(@D)/utilities/dhd_priv/ && \
		cp -avf dhd_priv $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/
endef # BCMDHD_BUILD_UTILS
endif

ifeq ($(BR2_PACKAGE_BCMDHD_AUTOFW),y)
define BCMDHD_INSTALL_AUTOFW
	@cp -avf $(@D)/fw/bcm43438/fw_apsta.a1.bin $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/fw_bcm43438a1_apsta.bin
	@cp -avf $(@D)/fw/bcm43340/fw_apsta.bin $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/fw_bcm43340_apsta.bin
	@cp -avf $(@D)/fw/bcm43455/fw_apsta.bin $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/fw_bcm43455_apsta.bin

	@cp -avf $(@D)/nvram/$(BCMDHD_MVENDOR)/bcm43438/nvram.a1.txt $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/nv_bcm43438a1.txt
	@cp -avf $(@D)/nvram/$(BCMDHD_MVENDOR)/bcm43340/nvram.txt $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/nv_bcm43340.txt
	@cp -avf $(@D)/nvram/$(BCMDHD_MVENDOR)/bcm43455/nvram.txt $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/nv_bcm43455.txt
endef
endif

define BCMDHD_INSTALL_TARGET_CMDS
@#	@rm -f $(TARGET_DIR)/usr/sbin/hostapd
	@mkdir -p $(TARGET_DIR)/$(BCMDHD_DEST_DIR)

	@cp -avf $(BCMDHD_KO_DIR)/$(BCMDHD_KO_NAME) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/$(BCMDHD_KO_NAME)
	@ln -fs $(BCMDHD_DEST_DIR)/$(BCMDHD_KO_NAME) $(TARGET_DIR)/lib/modules/

	@if [ "$(BR2_PACKAGE_BCMDHD_AUTOFW)" != "y" ]; then \
		cp -avf $(BCMDHD_FW_DIR)/$(BCMDHD_FW_NAME) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/fw_apsta.bin; \
		ln -fs $(BCMDHD_DEST_DIR)/fw_apsta.bin $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/$(BCMDHD_FW_CONFIG); \
		cp -avf $(BCMDHD_FW_DIR)/$(BCMDHD_MFG_FW_NAME) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/mfg_fw_apsta.bin; \
		cp -avf $(BCMDHD_NVRAM_DIR)/$(BCMDHD_NVRAM_NAME) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/nvram.txt; \
	fi

	@cp -avf $(BCMDHD_WL_TOOL_BINDIR)/$(BCMDHD_WL_TOOL) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/wl
	@$(STRIPCMD) $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/wl
	@ln -fs $(BCMDHD_DEST_DIR)/wl $(TARGET_DIR)/usr/sbin/

	$(BCMDHD_INSTALL_BT)

	@for f in ${BCMDHD_AMBA_UTILITIES}; do \
		cp -vf $(@D)/scripts/$${f} $(TARGET_DIR)/usr/local/share/script/; \
	done
	@cp -vf $(@D)/scripts/preload.sh.A9 $(TARGET_DIR)/usr/local/share/script/preload.sh
	@cp -vf $(@D)/scripts/wifi_start_profile.sh $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/
	@cp -vf $(@D)/scripts/wpa_supplicant.ap.conf $(TARGET_DIR)/$(BCMDHD_DEST_DIR)/

	@$(SED) 's|^HCI_DRIVER=\(.*\)|HCI_DRIVER=brcm|g' $(TARGET_DIR)/usr/local/share/script/bt.conf

	$(BCMDHD_INSTALL_GPS)
	$(BCMDHD_INSTALL_UTILS)

	$(BCMDHD_INSTALL_AUTOFW)
endef

define BCMDHD_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(BCMDHD_DEST_DIR)
	@rm -f  $(TARGET_DIR)/lib/modules/$(BCMDHD_KO_NAME)
	@rm -f  $(TARGET_DIR)/usr/sbin/wl
	@rm -f  $(TARGET_DIR)/usr/sbin/brcm_patchram_plus

	@for i in hostapd hostapd_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done
	@for i in wpa_supplicant wpa_cli; do \
		rm -f $(TARGET_DIR)/usr/sbin/$${i}; \
	done

	@for f in ${BCMDHD_AMBA_UTILITIES}; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$${f}; \
	done
	rm -f ${TARGET_DIR}/usr/local/share/script/preload.sh
endef

define BCMDHD_CLEAN_CMDS
	@cd $(BCMDHD_DRV_DIR)/open-src/src/dhd/linux/ && $(MAKE) clean
	$(BCMDHD_CLEAN_WL_TOOL)
endef

$(eval $(generic-package))

