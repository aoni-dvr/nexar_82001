CYPRESS_VERSION		= amba
CYPRESS_SITE_METHOD	= local
CYPRESS_SITE		= ../external_sdk/cypress/bcmdhd_cypress
CYPRESS_DEPENDENCIES	= linux openssl libnl iw
CYPRESS_INSTALL_STAGING	= NO
CYPRESS_INSTALL_IMAGES	= NO
CYPRESS_INSTALL_TARGET	= YES

CYPRESS_DEBUG		= NO
CYPRESS_BUILD_WL	= NO

# Module Type
ifeq ($(BR2_PACKAGE_CY_43455),y)
CYPRESS_MTYPE		= bcm43455
CYPRESS_BT_FUN		= YES
#	FW name released from vendors
ifeq ($(BR2_PACKAGE_CY_AW),y)
CYPRESS_FW_CONFIG	= cyw43455-7.45.100.18.bin
CYPRESS_MFG_FW_CONFIG	= cyw43455-mfgtest-7.45.100.18.bin
else ifeq ($(BR2_PACKAGE_CY_QUECTEL),y)
CYPRESS_FW_CONFIG	= cyfmac43455-sdio.bin
CYPRESS_MFG_FW_CONFIG	= cyw43455-sdio-mfgtest.bin
CYPRESS_CLM_CONFIG	= cyfmac43455-sdio.clm_blob
CYPRESS_MFG_CLM_CONFIG	= cyw43455-sdio-mfgtest.clm_blob
endif
else ifeq ($(BR2_PACKAGE_CY_43456),y)
CYPRESS_MTYPE		= bcm43456
CYPRESS_BT_FUN		= YES
#	FW name released from vendors
CYPRESS_FW_CONFIG	= fw_bcm43456c5_ag_apsta.bin
CYPRESS_MFG_FW_CONFIG	= fw_bcm43456c5_ag_mfg.bin
else
ifeq ($(BR2_PACKAGE_CYPRESS),y)
  $(error Unknown module type!)
endif
endif

# Vendor/Maker
ifeq ($(BR2_PACKAGE_CY_AW),y)
CYPRESS_MVENDOR		= aw
else ifeq ($(BR2_PACKAGE_CY_AMPAK),y)
CYPRESS_MVENDOR		= ampak
else ifeq ($(BR2_PACKAGE_CY_USI),y)
CYPRESS_MVENDOR		= usi
else ifeq ($(BR2_PACKAGE_CY_QUECTEL),y)
CYPRESS_MVENDOR		= quectel
else
ifeq ($(BR2_PACKAGE_CYPRESS),y)
  $(error Unknown module vendor/maker!)
endif
endif

ifeq ($(BR2_PACKAGE_CY_QUECTEL),y)
CYPRESS_DRV_DIR		= $(@D)/driver/dhd-quectel-fc905a
else
CYPRESS_DRV_DIR		= $(@D)/driver/dhd-android-1.363.125.19
endif

CYPRESS_FW_DIR		= $(@D)/fw/$(CYPRESS_MTYPE)
#	FW name used by load.sh
CYPRESS_FW_NAME		= fw_apsta.bin
CYPRESS_MFG_FW_NAME = mfg_fw_apsta.bin
CYPRESS_NVRAM_NAME	= nvram.txt
CYPRESS_BT_HCD_NAME	= bt.hcd
CYPRESS_CLM_NAME	= sdio.clm_blob

CYPRESS_NVRAM_DIR	= $(@D)/nvram/$(CYPRESS_MVENDOR)/$(CYPRESS_MTYPE)
CYPRESS_DEST_DIR	= /usr/local/cypress

CYPRESS_WL_TOOL_SRCDIR	= $(@D)
CYPRESS_WL_TOOL_BINDIR	= $(@D)/wl_tool/bin

ifeq ($(BR2_TOOLCHAIN_EXTERNAL_LINARO_ARM),y)
CYPRESS_WL_POST		= .linaro
else
CYPRESS_WL_POST		=
endif

ifeq ($(CYPRESS_BUILD_WL),YES)
CYPRESS_WL_TOOL		= wl
else
ifeq ($(BR2_ARCH_IS_64),y)
CYPRESS_WL_TOOL		= wl.arm64
else
CYPRESS_WL_TOOL		= wl.bcm43455
endif
endif


CYPRESS_BT_TOOL_SRCDIR	= $(@D)/bt_tool/
CYPRESS_BT_FW_DIR	= $(@D)/bt_fw/$(CYPRESS_MVENDOR)/$(CYPRESS_MTYPE)

CYPRESS_KVER		= $(shell cat $(LINUX_DIR)/include/config/kernel.release)

ifeq ($(BR2_PACKAGE_CY_QUECTEL),y)
CYPRESS_KO_NAME		= ifxahd.ko
else
CYPRESS_KO_NAME		= cywdhd.ko
endif
CYPRESS_KO_DIR		= $(CYPRESS_DRV_DIR)

CYPRESS_DRV_MKOPTION	= CROSS_COMPILE=$(TARGET_CROSS) \
				LINUXDIR=$(LINUX_DIR) \
				LINUXVER=$(CYPRESS_KVER)

CYPRESS_AMBA_UTILITIES = load.sh unload.sh


define CYPRESS_CONFIGURE_DRIVER
endef

define CYPRESS_CONFIGURE_CMDS
	$(CYPRESS_CONFIGURE_DRIVER)
endef


define CYPRESS_BUILD_DRIVER
	cd $(CYPRESS_DRV_DIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(CYPRESS_DRV_MKOPTION) \
		$(shell which make)
endef

ifeq ($(CYPRESS_BUILD_WL),YES)
define CYPRESS_BUILD_WL_TOOL
	cd $(CYPRESS_WL_TOOL_SRCDIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(CYPRESS_DRV_MKOPTION) \
		HOSTENV=linux HOSTOS=unix \
		TARGETENV=linuxarm_le TARGETOS=unix \
		TARGETARCH=arm_le \
		$(MAKE) all && \
		mkdir -p $(CYPRESS_WL_TOOL_BINDIR) && \
		cp wlarm_le $(CYPRESS_WL_TOOL_BINDIR)/wl
endef

define CYPRESS_CLEAN_WL_TOOL
	@cd $(CYPRESS_WL_TOOL_SRCDIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(CYPRESS_DRV_MKOPTION) \
		HOSTENV=linux HOSTOS=unix \
		TARGETENV=linuxarm_le TARGETOS=unix \
		TARGETARCH=arm_le \
		$(MAKE) clean
endef
endif

ifeq ($(CYPRESS_BT_FUN),YES)
define CYPRESS_BUILD_BT_TOOL
	@cd $(CYPRESS_BT_TOOL_SRCDIR) && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(CYPRESS_DRV_MKOPTION) \
		$(MAKE)
endef

define CYPRESS_INSTALL_BT
	@cp -avf $(CYPRESS_BT_TOOL_SRCDIR)/brcm_patchram_plus $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
	@$(STRIPCMD) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/brcm_patchram_plus
	@ln -fs $(CYPRESS_DEST_DIR)/brcm_patchram_plus $(TARGET_DIR)/usr/sbin/
	@cp -avf $(CYPRESS_BT_FW_DIR)/$(CYPRESS_BT_HCD_NAME) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/bt.hcd
endef
endif

define CYPRESS_BUILD_CMDS
	$(CYPRESS_BUILD_DRIVER)

	$(CYPRESS_BUILD_WL_TOOL)
	${CYPRESS_BUILD_BT_TOOL}
endef


define CYPRESS_INSTALL_TARGET_CMDS
	@mkdir -p $(TARGET_DIR)/$(CYPRESS_DEST_DIR)

	@cp -avf $(CYPRESS_KO_DIR)/$(CYPRESS_KO_NAME) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/$(CYPRESS_KO_NAME)
	@ln -fs $(CYPRESS_DEST_DIR)/$(CYPRESS_KO_NAME) $(TARGET_DIR)/lib/modules/

	@cp -avf $(CYPRESS_FW_DIR)/$(CYPRESS_FW_CONFIG) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
	@ln -fs $(CYPRESS_DEST_DIR)/$(CYPRESS_FW_CONFIG) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/$(CYPRESS_FW_NAME)

	@cp -avf $(CYPRESS_FW_DIR)/$(CYPRESS_MFG_FW_CONFIG) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
	@ln -fs $(CYPRESS_DEST_DIR)/$(CYPRESS_MFG_FW_CONFIG) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/$(CYPRESS_MFG_FW_NAME)

	@cp -avf $(CYPRESS_FW_DIR)/$(CYPRESS_CLM_CONFIG) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
	@cp -avf $(CYPRESS_FW_DIR)/$(CYPRESS_MFG_CLM_CONFIG) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/

	@cp -avf $(CYPRESS_NVRAM_DIR)/$(CYPRESS_NVRAM_NAME) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/nvram.txt

	@cp -avf $(CYPRESS_WL_TOOL_BINDIR)/$(CYPRESS_WL_TOOL) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/wl
	@$(STRIPCMD) $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/wl
	@ln -fs $(CYPRESS_DEST_DIR)/wl $(TARGET_DIR)/usr/sbin/

	$(CYPRESS_INSTALL_BT)

	@for f in ${CYPRESS_AMBA_UTILITIES}; do \
		cp -vf $(@D)/scripts/$${f} $(TARGET_DIR)/usr/local/share/script/; \
	done
	@cp -vf $(@D)/scripts/preload.sh $(TARGET_DIR)/usr/local/share/script/preload.sh
	@cp -vf $(@D)/scripts/wifi_start_profile.sh $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
	@cp -vf $(@D)/scripts/wpa_supplicant.ap.conf $(TARGET_DIR)/$(CYPRESS_DEST_DIR)/
endef

define CYPRESS_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(CYPRESS_DEST_DIR)
	@rm -f  $(TARGET_DIR)/lib/modules/$(CYPRESS_KO_NAME)
	@rm -f  $(TARGET_DIR)/usr/sbin/wl
	@rm -f  $(TARGET_DIR)/usr/sbin/brcm_patchram_plus

	@for f in ${CYPRESS_AMBA_UTILITIES}; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$${f}; \
	done
	rm -f ${TARGET_DIR}/usr/local/share/script/preload.sh
endef

define CYPRESS_CLEAN_CMDS
	@cd $(CYPRESS_DRV_DIR) && $(MAKE) clean
	@cd $(CYPRESS_HOSTAPD) && $(MAKE) clean
	@cd $(CYPRESS_WPA) && $(MAKE) clean
	$(CYPRESS_CLEAN_WL_TOOL)
endef

$(eval $(generic-package))

