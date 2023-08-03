
.PHONY: flexidag_openod flexidag_openod_install

diags: flexidag_openod

diags_install: flexidag_openod_install

flexidag_openod:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_openod

FLEXIDAG_OPENOD_OUT_DIR = $(O)/out/cv/flexidag_openod
flexidag_openod_install:
	$(Q)mkdir -p $(FLEXIDAG_OPENOD_OUT_DIR)
	$(Q)cp -rLf $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/flexidag/${PROJECT}/flexidag_openod/* $(FLEXIDAG_OPENOD_OUT_DIR)
