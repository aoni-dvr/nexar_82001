#
# Add new diags for dev test.
#

################################################################################
# Diags part: include actually diags to build
################################################################################
# include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaseg37_avm.mk
# include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaseg37_fc.mk
# include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaod36_fc.mk

ifeq ($(CONFIG_RCPRJ_REGRESSION), y)
ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_openseg_regression),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_openseg_regression.mk
endif

ifneq ($(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaod_fc_regression),)
	include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_ambaod_fc_regression.mk
endif
endif

#include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/flexidag_bisenet_pelee.mk

ifeq ($(CONFIG_SOC_CV2), y)
# for cv2
else
# for cv22 and cv25
ifeq ($(CONFIG_SOC_CV22), y)
# for cv22
else
# for cv25
endif
endif


diags:

diags_install:
