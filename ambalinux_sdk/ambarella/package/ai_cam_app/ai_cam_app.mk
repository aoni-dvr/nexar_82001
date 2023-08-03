AI_CAM_APP_VERSION          = amba
AI_CAM_APP_SITE_METHOD      = local
AI_CAM_APP_SITE             = ../pkg/ai_cam/app
AI_CAM_APP_DEPENDENCIES     = libjpeg
AI_CAM_APP_INSTALL_STAGING  = YES
AI_CAM_APP_INSTALL_TARGET   = YES
AI_CAM_APP_CONF_OPTS        =


ifndef BR2_PACKAGE_AI_CAM_LIB_PREBUILT
AI_CAM_APP_DEPENDENCIES += ai_cam_lib

define AI_CAM_APP_INSTALL_PREBUILT
	@cp -avf $(@D)/prebuilt/opt_sc_algos_interface.h $(STAGING_DIR)/usr/include
endef

else

AI_CAM_APP_SO = $(notdir $(wildcard $(AI_CAM_APP_SITE)/prebuilt/libamba_opt_sc_algos.so.*))
AI_CAM_APP_SO_VER = $(shell echo $(AI_CAM_APP_SO) | awk -F ".so." '{print $$2}')

define AI_CAM_APP_INSTALL_PREBUILT
	@cp -avf $(@D)/prebuilt/opt_sc_algos_interface.h $(STAGING_DIR)/usr/include
	@cp -avf $(@D)/prebuilt/$(AI_CAM_APP_SO) $(STAGING_DIR)/usr/lib/
	@ln -s $(STAGING_DIR)/usr/lib/$(AI_CAM_APP_SO) $(STAGING_DIR)/usr/lib/libamba_opt_sc_algos.so
	@cp -avf $(@D)/prebuilt/$(AI_CAM_APP_SO) $(TARGET_DIR)/usr/lib/
	@ln -s $(TARGET_DIR)/usr/lib/$(AI_CAM_APP_SO) $(TARGET_DIR)/usr/lib/libamba_opt_sc_algos.so
	@cp -avf $(@D)/prebuilt/libamba_opt_sc_algos.sign $(TARGET_DIR)/usr/lib/
endef

endif

AI_CAM_APP_PRE_BUILD_HOOKS += AI_CAM_APP_INSTALL_PREBUILT

$(eval $(cmake-package))

