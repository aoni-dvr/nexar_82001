# Front-end makefile to build orc binaries.
# Add new diags when new diag is avaiable.
#

################################################################################
# Usage:
# directly invoke:
# 	make -f $(srctree)/$(CV_COMSVC_DIR)/diags.mk
# or
# invoke from rtos build system, xxx_diags is user defined:
# 	make xxx_diags
#
################################################################################
#
# RTOS_DIR: absolute path of rtos
# CV_DIAG_DIR: relative to $(RTOS_DIR) or $(RTOS_DIR)/output.
# CUR_DIAG_DIR: absolute path of diags
#
################################################################################

RTOS_DIR:=$(srctree)

################################################################################
# Common part: diags_inc.mk inclue first
################################################################################
include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/diags_lib_inc.mk

# Read config if it is not applied
ifeq ($(CONFIG_FWPROG_SYS),)
-include $(O)/include/config/auto.conf
endif

################################################################################
# Diags part: include actually diags to build
################################################################################
include $(RTOS_DIR)/$(CV_COMSVC_DIR)/diags_mk/installer.mk
