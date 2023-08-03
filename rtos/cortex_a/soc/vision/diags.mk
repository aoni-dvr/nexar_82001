# Front-end makefile to build orc binaries.
# Add new diags when new diag is avaiable.
#
include $(srctree)/$(CV_CORE_DIR)/arm_framework/cvchip.mk

################################################################################
# Usage:
# directly invoke:
# 	make -f $(srctree)/$(CV_CORE_DIR)/diags.mk
# or
# invoke from rtos build system, xxx_diags is user defined:
# 	make xxx_diags
#
################################################################################
#
# RTOS_DIR: absolute path of rtos
# CV_DIAG_DIR: relative to $(srctree) or $(srctree)/output.
# CUR_DIAG_DIR: absolute path of diags
#
################################################################################

RTOS_DIR:=$(srctree)

################################################################################

ifneq ("$(V)", "")
	Q=
else
	Q=@
endif

CUR_DIAG_DIR=$(O)/$(CV_CORE_DIR)/diags
CV_DIAG_DIR =$(CUR_DIAG_DIR)
################################################################################
# make rules
################################################################################
all: prepare_source

prepare_source: init_diag
	$(Q)sh $(srctree)/$(CV_CORE_DIR)/prepare_package.sh $(srctree)/$(CV_CORE_DIR)
	$(Q)mkdir -p $(CUR_DIAG_DIR)/
	$(Q)$(MAKE) -f $(srctree)/$(CV_CORE_DIR)/diags.mk diags


################################################################################
# init_diag rules
################################################################################

ifneq ("$(CONFIG_BUILD_CV_THREADX)", "y")
# Export variables for init_diags.mk
export CONFIG_BUILD_CV_THREADX=y
export objtree=$(O)
export obj=$(CV_CORE_DIR)
endif

init_diag:
	$(Q)$(MAKE) -f $(srctree)/$(CV_CORE_DIR)/cv_common/build/init_diags.mk
	$(Q)echo "" > $(CUR_DIAG_DIR)/.target_make_env



################################################################################
# Diags part: include actually diags to build
################################################################################
include $(srctree)/$(CV_CORE_DIR)/diags_mk/orc_scheduler.mk
ifeq ($(CVCHIP),CHIP_CV2A)
include $(srctree)/$(CV_CORE_DIR)/diags_mk/orc_scheduler_ar.mk
else ifeq ($(CVCHIP),CHIP_CV22A)
include $(srctree)/$(CV_CORE_DIR)/diags_mk/orc_scheduler_ar.mk
else ifeq ($(CVCHIP),CHIP_CV2FS)
include $(srctree)/$(CV_CORE_DIR)/diags_mk/orc_scheduler_ar.mk
else ifeq ($(CVCHIP),CHIP_CV22FS)
include $(srctree)/$(CV_CORE_DIR)/diags_mk/orc_scheduler_ar.mk
endif
