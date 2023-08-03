
.PHONY: flexidag_openseg_regression flexidag_openseg_regression_install

diags: flexidag_openseg_regression

diags_install: flexidag_openseg_regression_install

flexidag_openseg_regression:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_openseg_regression
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_openseg_regression-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_openseg_regression-build

FLEXIDAG_OPENSEG_REGRESSION_OUT_DIR = $(O)/out/cv/flexidag_openseg_regression
flexidag_openseg_regression_install:
	$(Q)mkdir -p $(FLEXIDAG_OPENSEG_REGRESSION_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_openseg_regression/bin/flexibin $(FLEXIDAG_OPENSEG_REGRESSION_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_openseg_regression/bin/golden $(FLEXIDAG_OPENSEG_REGRESSION_OUT_DIR)
