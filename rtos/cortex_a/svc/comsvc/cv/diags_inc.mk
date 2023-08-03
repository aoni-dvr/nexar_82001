################################################################################
# Common include makefile for diags.mk
################################################################################

ifneq ("$(V)", "")
	Q=
else
	Q=@
endif

CUR_DIAG_DIR=$(O)/$(CV_COMSVC_DIR)/diags
CV_DIAG_DIR = $(CUR_DIAG_DIR)
################################################################################
# make rules
################################################################################
all: prepare_source

prepare_source:
	$(Q)rm -rf $(CUR_DIAG_DIR)/cv
	$(Q)mkdir -p $(CUR_DIAG_DIR)/cv $(CUR_DIAG_DIR)/cv/arm_framework $(CUR_DIAG_DIR)/cv/orc_framework
	$(Q)cp -rf $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags.mk $(CUR_DIAG_DIR)/cv
	$(Q)cp -rf $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk $(CUR_DIAG_DIR)/cv
	$(Q)if [ -e $(RTOS_DIR)/$(CV_COMSVC_DIR)/orc_framework/cvtask_common ]; then \
		cp -rfL $(RTOS_DIR)/$(CV_COMSVC_DIR)/orc_framework/cvtask_common/. $(CUR_DIAG_DIR)/cv/orc_framework/cvtask; \
	fi
	$(Q)$(foreach f, $(notdir $(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/orc_framework/cvtask_$(PROJECT)/orcvp/*)), \
		rm -rf $(CUR_DIAG_DIR)/cv/orc_framework/cvtask/orcvp/$(f); \
		cp -rfL $(RTOS_DIR)/$(CV_COMSVC_DIR)/orc_framework/cvtask_$(PROJECT)/orcvp/$(f) $(CUR_DIAG_DIR)/cv/orc_framework/cvtask/orcvp; \
	)
	$(Q)if [ -e $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common ]; then \
		mkdir -p $(CUR_DIAG_DIR)/cv/cv_common; \
		cp -rfL $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/inc $(CUR_DIAG_DIR)/cv/cv_common; \
		cp -rfL $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/libs $(CUR_DIAG_DIR)/cv/cv_common; \
	fi
	$(Q)if [ -e $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/libs/orc/cvtask_common ]; then \
		cp -rfL $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/libs/orc/cvtask_common/. $(CUR_DIAG_DIR)/cv/cv_common/libs/orc/cvtask; \
	fi
	$(Q)if [ -e $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/libs/orc/cvtask_$(PROJECT) ]; then \
		cp -rfL $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/libs/orc/cvtask_$(PROJECT)/. $(CUR_DIAG_DIR)/cv/cv_common/libs/orc/cvtask; \
	fi
	$(Q)if [ -e $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests ]; then \
		cp -rf $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests $(CUR_DIAG_DIR)/cv; \
	fi
	$(Q)cp -rf $(RTOS_DIR)/$(CV_CORE_DIR)/cv_common $(CUR_DIAG_DIR)/cv
	$(Q)cp -rf $(RTOS_DIR)/$(CV_CORE_DIR)/orc_framework $(CUR_DIAG_DIR)/cv
	$(Q)cp -rf $(RTOS_DIR)/$(CV_CORE_DIR)/tests $(CUR_DIAG_DIR)/cv
	$(Q)cp -rf $(RTOS_DIR)/$(CV_CORE_DIR)/arm_framework/cvchip.mk $(CUR_DIAG_DIR)/cv/arm_framework
	$(Q)ln -sf ../../../../.repo $(CUR_DIAG_DIR)/../../../.repo
	$(Q)$(MAKE) -f $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags.mk init_diag
	$(Q)$(MAKE) -f $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags.mk diags

################################################################################
# init_diag rules
################################################################################

ifneq ("$(CONFIG_BUILD_CV_THREADX)", "y")
# Export variables for init_diags.mk
export CONFIG_BUILD_CV_THREADX=y
export objtree=$(O)
export obj=$(CV_COMSVC_DIR)
endif

init_diag:
	$(Q)$(MAKE) -f $(CUR_DIAG_DIR)/cv/cv_common/build/init_diags.mk
	$(Q)echo "" > $(CUR_DIAG_DIR)/.target_make_env


