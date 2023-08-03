MRVL_VERSION		= amba
MRVL_SITE_METHOD	= local
MRVL_SITE		= ../external_sdk/marvell
MRVL_DEPENDENCIES	= linux network_turnkey openssl libnl iw
MRVL_INSTALL_STAGING	= NO
MRVL_INSTALL_IMAGES	= NO
MRVL_INSTALL_TARGET	= YES


# Module Type
ifeq ($(BR2_PACKAGE_MRVL_88W8977),y)
MRVL_MTYPE		= 88w8977
MRVL_BT_FUN		= YES
MRVL_SDKO_NAME		= sd8977.ko
else ifeq ($(BR2_PACKAGE_MRVL_88W8801),y)
MRVL_MTYPE		= 88w8801
MRVL_BT_FUN		= NO
MRVL_SDKO_NAME		= sd8801.ko
else
ifeq ($(BR2_PACKAGE_MRVL),y)
  $(error Unknown module type!)
endif
endif

MRVL_DRV_DIR		= $(@D)/$(MRVL_MTYPE)/wlan_src
# 'build' will also build utilities. But install define do not install utilities.
MRVL_DRV_MKTARGET	= build
#MRVL_DRV_MKTARGET	=
MRVL_FW_DIR		= $(@D)/$(MRVL_MTYPE)/fw

MRVL_BT_DRV_DIR		= $(@D)/$(MRVL_MTYPE)/mbt_src

MRVL_DEST_DIR		= usr/local/mrvl

MRVL_KVER		= $(shell cat $(LINUX_DIR)/include/config/kernel.release)
MRVL_KO_DIR		= $(MRVL_DRV_DIR)
MRVL_DRV_MKOPTION	= ARCH=arm64 \
				CROSS_COMPILE=$(TARGET_CROSS) \
				KERNELDIR=$(LINUX_DIR) \
				LINUXVER=$(MRVL_KVER)

MRVL_APP_WL = \
	      mapp/mlanconfig/mlanconfig \
	      mapp/mlanutl/mlanutl \
	      mapp/mlan2040coex/mlan2040coex \
	      mapp/uaputl/uaputl.exe \
	      mapp/mlanevent/mlanevent.exe
MRVL_APP_BT =

ifeq ($(BR2_PACKAGE_MRVL_88W8977),y)
MRVL_APP_WL += mapp/wifidirectutl/wifidirectutl
MRVL_APP_BT += app/fm_app/fmapp
endif

MRVL_AMBA_UTILITIES = load.sh preload.sh unload.sh wifi_softmac.sh


define MRVL_CONFIGURE_DRIVER
endef

define MRVL_CONFIGURE_CMDS
	$(MRVL_CONFIGURE_DRIVER)
endef


define MRVL_BUILD_DRIVER
	@cd $(MRVL_DRV_DIR)/ && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(MRVL_DRV_MKOPTION) \
		$(shell which make) $(MRVL_DRV_MKTARGET)
endef

ifeq ($(MRVL_BT_FUN),YES)
define MRVL_BUILD_BT_DRIVER
	@cd $(MRVL_BT_DRV_DIR)/ && \
		$(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) \
		$(MRVL_DRV_MKOPTION) \
		$(shell which make) $(MRVL_DRV_MKTARGET)
endef # MRVL_BUILD_BT_DRIVER
endif

define MRVL_BUILD_CMDS
	$(MRVL_BUILD_DRIVER)
	$(MRVL_BUILD_BT_DRIVER)
endef

ifeq ($(MRVL_DRV_MKTARGET),build)
define MRVL_INSTALL_MAPP
	@for f in $(MRVL_APP_WL); do \
		cp -avf $(MRVL_DRV_DIR)/$${f} $(TARGET_DIR)/$(MRVL_DEST_DIR); \
	done
	@for f in $(MRVL_APP_BT); do \
		cp -avf $(MRVL_BT_DRV_DIR)/$${f} $(TARGET_DIR)/$(MRVL_DEST_DIR); \
	done
endef
endif

define MRVL_INSTALL_WLAN
	@mkdir -p $(TARGET_DIR)/$(MRVL_DEST_DIR)
	@mkdir -p $(TARGET_DIR)/lib/firmware/mrvl

	@cp -avf $(MRVL_KO_DIR)/mlan.ko $(TARGET_DIR)/$(MRVL_DEST_DIR)/
	@cp -avf $(MRVL_KO_DIR)/$(MRVL_SDKO_NAME) $(TARGET_DIR)/$(MRVL_DEST_DIR)/$(MRVL_SDKO_NAME)
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(MRVL_DEST_DIR)/mlan.ko
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(MRVL_DEST_DIR)/$(MRVL_SDKO_NAME)
	@ln -fs /$(MRVL_DEST_DIR)/mlan.ko $(TARGET_DIR)/lib/modules/
	@ln -fs /$(MRVL_DEST_DIR)/$(MRVL_SDKO_NAME) $(TARGET_DIR)/lib/modules/
endef

ifeq ($(MRVL_BT_FUN),YES)
define MRVL_INSTALL_BT
	@cp -avf $(MRVL_BT_DRV_DIR)/bt8xxx.ko $(TARGET_DIR)/$(MRVL_DEST_DIR)/
	@$(STRIPCMD) --strip-unneeded $(TARGET_DIR)/$(MRVL_DEST_DIR)/bt8xxx.ko
	@ln -fs /$(MRVL_DEST_DIR)/bt8xxx.ko $(TARGET_DIR)/lib/modules/

	@mkdir -p $(TARGET_DIR)/lib/firmware/mrvl
	@cp -avf $(MRVL_BT_DRV_DIR)/config/bt_cal_data.conf $(TARGET_DIR)/lib/firmware/mrvl/
	@$(SED) 's|^HCI_DRIVER=\(.*\)|HCI_DRIVER=mrvl|g' $(TARGET_DIR)/usr/local/share/script/bt.conf
endef # MRVL_INSTALL_BT
endif

define MRVL_INSTALL_MISC
	@cp -avf $(MRVL_FW_DIR)/* $(TARGET_DIR)/lib/firmware/mrvl/

	@for f in ${MRVL_AMBA_UTILITIES}; do \
		cp -vf $(@D)/$(MRVL_MTYPE)/scripts/$${f} $(TARGET_DIR)/usr/local/share/script/; \
	done
endef

define MRVL_INSTALL_TARGET_CMDS
	$(MRVL_INSTALL_WLAN)
	$(MRVL_INSTALL_BT)
	$(MRVL_INSTALL_MISC)
	$(MRVL_INSTALL_MAPP)
endef

define MRVL_UNINSTALL_TARGET_CMDS
	@rm -rf $(TARGET_DIR)/$(MRVL_DEST_DIR)
	@rm -f  $(TARGET_DIR)/lib/modules/mlan.ko
	@rm -f  $(TARGET_DIR)/lib/modules/sd8xxx.ko

	@rm -rf $(TARGET_DIR)/lib/firmware/mrvl

	@for f in ${MRVL_AMBA_UTILITIES}; do \
		rm -f ${TARGET_DIR}/usr/local/share/script/$${f}; \
	done
endef

define MRVL_CLEAN_CMDS
endef

$(eval $(generic-package))

