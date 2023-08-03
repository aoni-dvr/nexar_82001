AI_CAM_LIB_VERSION          = amba
AI_CAM_LIB_SITE_METHOD      = local
AI_CAM_LIB_SITE             = ../pkg/ai_cam/lib/opt_algos
AI_CAM_LIB_DEPENDENCIES     =
AI_CAM_LIB_INSTALL_STAGING  = YES
AI_CAM_LIB_INSTALL_TARGET   = YES


$(eval $(cmake-package))

