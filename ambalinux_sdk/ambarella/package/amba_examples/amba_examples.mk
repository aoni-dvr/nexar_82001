#############################################################
#
# amba_examples
#
#############################################################

pkg			= AMBA_EXAMPLES

$(pkg)_VERSION		= 1.0
$(pkg)_SITE_METHOD	= local
$(pkg)_SITE		= $(AMBARELLA_PKG_DIR)/amba_examples
$(pkg)_SOURCE		= amba_examples-$($(pkg)_VERSION).tar.gz
$(pkg)_INSTALL_STAGING	= NO
$(pkg)_INSTALL_IMAGES	= NO
$(pkg)_INSTALL_TARGET	= YES
$(pkg)_LICENSE		= MIT
$(pkg)_LICENSE_FILES	= License_MIT.txt

ifeq ($(BR2_arm),y)
ARMBADFLAGS = -DUSE_ARM
endif

#Prepre command line for sub packages
ifeq ($(BR2_PACKAGE_AMBA_EVENTNOTIFIER_EXAMPLE),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util libeventnotifier
  INSTALL_AMBA_EVENTNOTIFIER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaEventNotifier -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EVENTNOTIFIER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaEventNotifier -f br.mk
  CLEAN_AMBA_EVENTNOTIFIER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaEventNotifier -f br.mk clean
else
  INSTALL_AMBA_EVENTNOTIFIER_EXAMPLE =
  BUILD_AMBA_EVENTNOTIFIER_EXAMPLE =
  CLEAN_AMBA_EVENTNOTIFIER_EXAMPLE =
endif

ifeq ($(BR2_PACKAGE_AMBA_TRANSFER_EXAMPLE),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util
  $(pkg)_INSTALL_STAGING = YES
  INSTALL_AMBA_TRANSFER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaTransfer -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_TRANSFER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaTransfer -f br.mk
  CLEAN_AMBA_TRANSFER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaTransfer -f br.mk clean
  INSTALL_STAGING_AMBA_TRANSFER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaTransfer -f br.mk DESTDIR=$(STAGING_DIR) lib_install
  UNINSTALL_STAGING_AMBA_TRANSFER_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaTransfer -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
else
  INSTALL_AMBA_TRANSFER_EXAMPLE =
  BUILD_AMBA_TRANSFER_EXAMPLE =
  CLEAN_AMBA_TRANSFER_EXAMPLE =
  INSTALL_STAGING_AMBA_TRANSFER_EXAMPLE =
  UNINSTALL_STAGING_AMBA_TRANSFER_EXAMPLE =
endif

ifeq ($(BR2_PACKAGE_AMBA_FLEXIDAGIO_EXAMPLE),y)
ifeq ($(BR2_PACKAGE_AMBAIPC),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util
endif
  $(pkg)_INSTALL_STAGING = YES
  INSTALL_AMBA_FLEXIDAGIO_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaFlexidagIO -f br.mk DESTDIR=$(TARGET_DIR) BR2_PACKAGE_AMBAIPC=$(BR2_PACKAGE_AMBAIPC) install
  BUILD_AMBA_FLEXIDAGIO_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaFlexidagIO -f br.mk BR2_PACKAGE_AMBAIPC=$(BR2_PACKAGE_AMBAIPC)
  CLEAN_AMBA_FLEXIDAGIO_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaFlexidagIO -f br.mk clean
  INSTALL_STAGING_AMBA_FLEXIDAGIO_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaFlexidagIO -f br.mk DESTDIR=$(STAGING_DIR) BR2_PACKAGE_AMBAIPC=$(BR2_PACKAGE_AMBAIPC) lib_install
  UNINSTALL_STAGING_AMBA_FLEXIDAGIO_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/AmbaFlexidagIO -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
else
  INSTALL_AMBA_FLEXIDAGIO_EXAMPLE =
  BUILD_AMBA_FLEXIDAGIO_EXAMPLE =
  CLEAN_AMBA_FLEXIDAGIO_EXAMPLE =
  INSTALL_STAGING_AMBA_FLEXIDAGIO_EXAMPLE =
  UNINSTALL_STAGING_AMBA_FLEXIDAGIO_EXAMPLE =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_FRAMER),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util libnetfifo
  INSTALL_AMBA_EXAMPLE_FRAMER = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_Framer -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_FRAMER =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_Framer -f br.mk
  CLEAN_AMBA_EXAMPLE_FRAMER = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_Framer -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_FRAMER =
  BUILD_AMBA_EXAMPLE_FRAMER =
  CLEAN_AMBA_EXAMPLE_FRAMER =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_SHAREPREVIEW),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util
  INSTALL_AMBA_EXAMPLE_SHAREPREVIEW = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_SharePreview -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_SHAREPREVIEW =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_SharePreview -f br.mk
  CLEAN_AMBA_EXAMPLE_SHAREPREVIEW = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_SharePreview -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_SHAREPREVIEW =
  BUILD_AMBA_EXAMPLE_SHAREPREVIEW =
  CLEAN_AMBA_EXAMPLE_SHAREPREVIEW =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_RTOS_API),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util
  INSTALL_AMBA_EXAMPLE_RTOS_API = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_RtosApi -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_RTOS_API =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_RtosApi -f br.mk
  CLEAN_AMBA_EXAMPLE_RTOS_API = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_RtosApi -f br.mk clean
  INSTALL_STAGING_AMBA_RTOS_API_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_RtosApi -f br.mk DESTDIR=$(STAGING_DIR) lib_install
  UNINSTALL_STAGING_AMBA_RTOS_API_EXAMPLE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_RtosApi -f br.mk DESTDIR=$(STAGING_DIR) lib_uninstall
else
  INSTALL_AMBA_EXAMPLE_RTOS_API =
  BUILD_AMBA_EXAMPLE_RTOS_API =
  CLEAN_AMBA_EXAMPLE_RTOS_API =
  INSTALL_STAGING_AMBA_RTOS_API_EXAMPLE =
  UNINSTALL_STAGING_AMBA_RTOS_API_EXAMPLE =
endif
ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_FRAMESHARE),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util
  INSTALL_AMBA_EXAMPLE_FRAMESHARE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_FRAMESHARE =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare -f br.mk
  CLEAN_AMBA_EXAMPLE_FRAMESHARE = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_FRAMESHARE =
  BUILD_AMBA_EXAMPLE_FRAMESHARE =
  CLEAN_AMBA_EXAMPLE_FRAMESHARE =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_STREAMIN),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util libnetfifo
  INSTALL_AMBA_EXAMPLE_STREAMIN = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_StreamIn -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_STREAMIN =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_StreamIn -f br.mk
  CLEAN_AMBA_EXAMPLE_STREAMIN = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_StreamIn -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_STREAMIN =
  BUILD_AMBA_EXAMPLE_STREAMIN =
  CLEAN_AMBA_EXAMPLE_STREAMIN =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_CAMCTRL_SERVER),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util
  INSTALL_AMBA_EXAMPLE_CAMCTRL_SERVER = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_CamctrlServer -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_CAMCTRL_SERVER = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_CamctrlServer -f br.mk
  CLEAN_AMBA_EXAMPLE_CAMCTRL_SERVER = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_CamctrlServer -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_CAMCTRL_SERVER =
  BUILD_AMBA_EXAMPLE_CAMCTRL_SERVER =
  CLEAN_AMBA_EXAMPLE_CAMCTRL_SERVER =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_OSDDRAW),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util opencv4
  INSTALL_AMBA_EXAMPLE_OSDDRAW = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_OsdDraw -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_OSDDRAW =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_OsdDraw -f br.mk
  CLEAN_AMBA_EXAMPLE_OSDDRAW = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_OsdDraw -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_OSDDRAW =
  BUILD_AMBA_EXAMPLE_OSDDRAW =
  CLEAN_AMBA_EXAMPLE_OSDDRAW =
endif

ifdef BR2_PACKAGE_AMBA_CAMERA
ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_FRAMESHARE_RTSP),y)
  $(pkg)_DEPENDENCIES += amba_util libev
  CONFIG_CAMERA = -DAMBA_CAMERA=$(BR2_PACKAGE_AMBA_CAMERA)
  INSTALL_AMBA_EXAMPLE_FRAMESHARE_RTSP = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare_Rtsp -f br_camera.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_FRAMESHARE_RTSP = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare_Rtsp -f br_camera.mk CONFIG_CAMERA=$(CONFIG_CAMERA)
  CLEAN_AMBA_EXAMPLE_FRAMESHARE_RTSP = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare_Rtsp -f br_camera.mk clean
else
  INSTALL_AMBA_EXAMPLE_FRAMESHARE_RTSP =
  BUILD_AMBA_EXAMPLE_FRAMESHARE_RTSP =
  CLEAN_AMBA_EXAMPLE_FRAMESHARE_RTSP =
endif
else
ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_FRAMESHARE_RTSP),y)
  $(pkg)_DEPENDENCIES += ambaipc amba_util libev
  CONFIG_CAMERA = -DAMBA_CAMERA=$(BR2_PACKAGE_AMBA_CAMERA)
  INSTALL_AMBA_EXAMPLE_FRAMESHARE_RTSP = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare_Rtsp -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_FRAMESHARE_RTSP = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare_Rtsp -f br.mk CONFIG_CAMERA=$(CONFIG_CAMERA)
  CLEAN_AMBA_EXAMPLE_FRAMESHARE_RTSP = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_FrameShare_Rtsp -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_FRAMESHARE_RTSP =
  BUILD_AMBA_EXAMPLE_FRAMESHARE_RTSP =
  CLEAN_AMBA_EXAMPLE_FRAMESHARE_RTSP =
endif
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_ROMFS),y)
  INSTALL_AMBA_EXAMPLE_ROMFS = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_ROMFS -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_ROMFS = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_ROMFS -f br.mk
  CLEAN_AMBA_EXAMPLE_ROMFS = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_ROMFS -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_ROMFS =
  BUILD_AMBA_EXAMPLE_ROMFS =
  CLEAN_AMBA_EXAMPLE_ROMFS =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_AMBAPRINT),y)
  INSTALL_AMBA_EXAMPLE_PRINT = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_AmbaPrint -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_PRINT = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_AmbaPrint -f br.mk
  CLEAN_AMBA_EXAMPLE_PRINT = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_AmbaPrint -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_PRINT =
  BUILD_AMBA_EXAMPLE_PRINT =
  CLEAN_AMBA_EXAMPLE_PRINT =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_AUDIO),y)
  $(pkg)_DEPENDENCIES += alsa-lib
  INSTALL_AMBA_EXAMPLE_AUDIO = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_AmbaAudio -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_AUDIO = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_AmbaAudio -f br.mk
  CLEAN_AMBA_EXAMPLE_AUDIO = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_AmbaAudio -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_AUDIO =
  BUILD_AMBA_EXAMPLE_AUDIO =
  CLEAN_AMBA_EXAMPLE_AUDIO =
endif

ifeq ($(BR2_PACKAGE_AMBA_EXAMPLE_SOCKET_CLIENT),y)
  $(pkg)_DEPENDENCIES += ambamal
  INSTALL_AMBA_EXAMPLE_SOCKET_CLIENT = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_SocketClient -f br.mk DESTDIR=$(TARGET_DIR) install
  BUILD_AMBA_EXAMPLE_SOCKET_CLIENT =  ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_SocketClient -f br.mk
  CLEAN_AMBA_EXAMPLE_SOCKET_CLIENT = ARMBADFLAGS="$(ARMBADFLAGS)" $(TARGET_CONFIGURE_OPTS) $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)/Example_SocketClient -f br.mk clean
else
  INSTALL_AMBA_EXAMPLE_SOCKET_CLIENT =
  BUILD_AMBA_EXAMPLE_SOCKET_CLIENT =
  CLEAN_AMBA_EXAMPLE_SOCKET_CLIENT =
endif




#BuildRoot macro
define AMBA_EXAMPLES_INSTALL_TARGET_CMDS
  $(INSTALL_AMBA_EVENTNOTIFIER_EXAMPLE)
  $(INSTALL_AMBA_TRANSFER_EXAMPLE)
  $(INSTALL_AMBA_FLEXIDAGIO_EXAMPLE)
  $(INSTALL_AMBA_EXAMPLE_FRAMER)
  $(INSTALL_AMBA_EXAMPLE_SOCKET_CLIENT)
  $(INSTALL_AMBA_EXAMPLE_SHAREPREVIEW)
  $(INSTALL_AMBA_EXAMPLE_RTOS_API)
  $(INSTALL_AMBA_EXAMPLE_FRAMESHARE)
  $(INSTALL_AMBA_EXAMPLE_STREAMIN)
  $(INSTALL_AMBA_EXAMPLE_CAMCTRL_SERVER)
  $(INSTALL_AMBA_EXAMPLE_PRINT)
  $(INSTALL_AMBA_EXAMPLE_BBXHANDLER)
  $(INSTALL_AMBA_EXAMPLE_OSDDRAW)
  $(INSTALL_AMBA_EXAMPLE_FRAMESHARE_RTSP)
  $(INSTALL_AMBA_EXAMPLE_ROMFS)
  $(INSTALL_AMBA_EXAMPLE_AUDIO)
endef

define AMBA_EXAMPLES_BUILD_CMDS
  $(BUILD_AMBA_EVENTNOTIFIER_EXAMPLE)
  $(BUILD_AMBA_TRANSFER_EXAMPLE)
  $(BUILD_AMBA_FLEXIDAGIO_EXAMPLE)
  $(BUILD_AMBA_EXAMPLE_FRAMER)
  $(BUILD_AMBA_EXAMPLE_SOCKET_CLIENT)
  $(BUILD_AMBA_EXAMPLE_SHAREPREVIEW)
  $(BUILD_AMBA_EXAMPLE_RTOS_API)
  $(BUILD_AMBA_EXAMPLE_FRAMESHARE)
  $(BUILD_AMBA_EXAMPLE_STREAMIN)
  $(BUILD_AMBA_EXAMPLE_CAMCTRL_SERVER)
  $(BUILD_AMBA_EXAMPLE_PRINT)
  $(BUILD_AMBA_EXAMPLE_BBXHANDLER)
  $(BUILD_AMBA_EXAMPLE_OSDDRAW)
  $(BUILD_AMBA_EXAMPLE_FRAMESHARE_RTSP)
  $(BUILD_AMBA_EXAMPLE_ROMFS)
  $(BUILD_AMBA_EXAMPLE_AUDIO)
endef

define AMBA_EXAMPLES_CLEAN_CMDS
  $(CLEAN_AMBA_EVENTNOTIFIER_EXAMPLE)
  $(CLEAN_AMBA_TRANSFER_EXAMPLE)
  $(CLEAN_AMBA_FLEXIDAGIO_EXAMPLE)
  $(CLEAN_AMBA_EXAMPLE_FRAMER)
  $(CLEAN_AMBA_EXAMPLE_SOCKET_CLIENT)
  $(CLEAN_AMBA_EXAMPLE_SHAREPREVIEW)
  $(CLEAN_AMBA_EXAMPLE_RTOS_API)
  $(CLEAN_AMBA_EXAMPLE_FRAMESHARE)
  $(CLEAN_AMBA_EXAMPLE_STREAMIN)
  $(CLEAN_AMBA_EXAMPLE_CAMCTRL_SERVER)
  $(CLEAN_AMBA_EXAMPLE_PRINT)
  $(CLEAN_AMBA_EXAMPLE_BBXHANDLER)
  $(CLEAN_AMBA_EXAMPLE_OSDDRAW)
  $(CLEAN_AMBA_EXAMPLE_FRAMESHARE_RTSP)
  $(CLEAN_AMBA_EXAMPLE_ROMFS)
  $(CLEAN_AMBA_EXAMPLE_AUDIO)
endef

define AMBA_EXAMPLES_INSTALL_STAGING_CMDS
  $(INSTALL_STAGING_AMBA_TRANSFER_EXAMPLE)
  $(INSTALL_STAGING_AMBA_FLEXIDAGIO_EXAMPLE)
  $(INSTALL_STAGING_AMBA_EXAMPLE_BBXHANDLER)
  $(INSTALL_STAGING_AMBA_RTOS_API_EXAMPLE)
endef

define AMBA_EXAMPLES_UNINSTALL_STAGING_CMDS
  $(UNINSTALL_STAGING_AMBA_TRANSFER_EXAMPLE)
  $(UNINSTALL_STAGING_AMBA_FLEXIDAGIO_EXAMPLE)
  $(UNINSTALL_STAGING_AMBA_EXAMPLE_BBXHANDLER)
  $(UNINSTALL_STAGING_AMBA_RTOS_API_EXAMPLE)
endef

#define AMBA_EXAMPLES_POST_LEGAL_INFO_HOOKS_CMD
#	$(Q)$(foreach F,$($(PKG)_LICENSE_FILES),$(call legal-license-file,$$($(PKG)_RAWNAME),$($(PKG)_BASENAME_RAW),$($(PKG)_HASH_FILE),$(F),$($(PKG)_DIR)/$(F),TARGET)$(sep))
#endef
#AMBA_EXAMPLES_POST_LEGAL_INFO_HOOKS += AMBA_EXAMPLES_POST_LEGAL_INFO_HOOKS_CMD

$(eval $(generic-package))
