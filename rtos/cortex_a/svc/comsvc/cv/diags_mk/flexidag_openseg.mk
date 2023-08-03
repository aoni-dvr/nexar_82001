
.PHONY: flexidag_openseg flexidag_openseg_install

diags: flexidag_openseg

diags_install: flexidag_openseg_install

flexidag_openseg:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_openseg

FLEXIDAG_OPENSEG_OUT_DIR = $(O)/out/cv/flexidag_openseg
flexidag_openseg_install:
	$(Q)mkdir -p $(FLEXIDAG_OPENSEG_OUT_DIR)
	$(Q)cp -rLf $(RTOS_DIR)/$(CV_COMSVC_DIR)/cv_common/flexidag/${PROJECT}/flexidag_openseg/* $(FLEXIDAG_OPENSEG_OUT_DIR)
