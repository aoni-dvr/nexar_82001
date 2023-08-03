.PHONY: installer installer_install

diags: installer

diags_install: installer_install

installer:
	$(Q)rm -rf $(CV_DIAG_DIR)/installer
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) installer-init

ORC_SRC_DIR = $(O)/$(CV_DIAG_DIR)/installer/build_hw/release_lib/orc
ORC_DST_DIR = $(RTOS_DIR)/$(CV_CORE_DIR)/cv_common/libs/orc

ifeq ($(CONFIG_QNX), y)
ARM_OUT_BIN = so
ARM_SRC_DIR = $(O)/install.qnx/aarch64le/usr/lib/
ARM_DST_DIR = $(RTOS_DIR)/vendors/ambarella/lib64/$(AMBA_CHIP_ID)/qnx
else ifeq ($(CONFIG_THREADX64), y)
ARM_OUT_BIN = a
ARM_SRC_DIR = $(O)/lib64
ARM_DST_DIR = $(RTOS_DIR)/vendors/ambarella/lib64/$(AMBA_CHIP_ID)/threadx
else
ARM_OUT_BIN = a
ARM_SRC_DIR = $(O)/lib
ARM_DST_DIR = $(RTOS_DIR)/vendors/ambarella/lib/$(AMBA_CHIP_ID)
endif

LIB_SRC_DIR = $(O)/$(CV_CORE_DIR)/diags
LIB_DST_DIR = $(RTOS_DIR)/$(CV_CORE_DIR)

installer_install:
	$(Q)mkdir -p $(ORC_DST_DIR)/cvtask_$(PROJECT)/orcvp/
	$(Q)if [ ! -e $(ORC_DST_DIR)/cvtask_$(PROJECT)/orcvp/Loggers ]; then \
		cp -rf $(ORC_DST_DIR)/cvtask/orcvp/Loggers/. $(ORC_DST_DIR)/cvtask_$(PROJECT)/orcvp/Loggers; \
	fi
	$(Q)mkdir -p $(LIB_DST_DIR)/cv_common/util/flexibin/$(PROJECT)
	$(Q)cp -rLf $(LIB_SRC_DIR)/orc_scheduler/flexidag0/util/flexibin/flexibin_create $(LIB_DST_DIR)/cv_common/util/flexibin/$(PROJECT)/flexibin_create
	$(Q)cp -rLf $(LIB_SRC_DIR)/orc_scheduler/flexidag0/build/util/create_flexipatch/create_flexipatch $(LIB_DST_DIR)/orc_framework/flexidag/util/create_flexipatch
