#
# Add new diags for SDK release.
#
################################################################################
# Diags part: include actually diags to build
################################################################################
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_bisenet_mnv2_raw),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_bisenet_mnv2_raw.mk
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_mnetssd_sensor),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_mnetssd_sensor.mk
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_fetch_chip_info),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_fetch_chip_info.mk
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/flexidag/${PROJECT}/flexidag_openod),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_openod.mk
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/flexidag/${PROJECT}/flexidag_openseg),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_openseg.mk
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_yield_test),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_yield_test.mk
endif

ifeq ($(CONFIG_SOC_CV2), y)

ifneq ($(wildcard $(RTOS_DIR)/$(CV_CORE_DIR)/cv_common/libs/orc/cvtask_cv2/orcvp/Stereo_FeatureExtraction/.),)
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaspufex),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaspufex.mk
endif
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambafex),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambafex.mk
endif
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaspufusion_02),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaspufusion_02.mk
endif
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaspufusion_024),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaspufusion_024.mk
endif
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_CORE_DIR)/cv_common/libs/orc/cvtask_cv2/orcvp/FeatureMatching/.),)
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambafma),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambafma.mk
endif
endif

else ifeq ($(CONFIG_SOC_CV2FS), y)

ifneq ($(wildcard $(RTOS_DIR)/$(CV_CORE_DIR)/cv_common/libs/orc/cvtask_cv2fs/orcvp/SpFex4CV2A/.),)
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambafex_cv2a),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambafex_cv2a.mk
endif
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaoffex_cv2a),)
include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaoffex_cv2a.mk
endif
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaspufex_cv2a),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaspufex_cv2a.mk
endif
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_CORE_DIR)/cv_common/libs/orc/cvtask_cv2fs/orcvp/DisparityFusion4CV2A/.),)
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaspufusion_cv2a),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaspufusion_cv2a.mk
endif
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_imgproc),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_imgproc.mk
endif

endif


diags:

diags_install:
