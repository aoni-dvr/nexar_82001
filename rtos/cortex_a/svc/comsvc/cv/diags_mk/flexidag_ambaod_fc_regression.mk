
.PHONY: flexidag_ambaod_fc_regression flexidag_ambaod_fc_regression_install

diags: flexidag_ambaod_fc_regression

diags_install: flexidag_ambaod_fc_regression_install

flexidag_ambaod_fc_regression:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaod_fc_regression
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod_fc_regression-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod_fc_regression-build

FLEXIDAG_AMBAOD_FC_REGRESSION_OUT_DIR = $(O)/out/cv/flexidag_ambaod_fc_regression
flexidag_ambaod_fc_regression_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBAOD_FC_REGRESSION_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod_fc_regression/bin/flexibin $(FLEXIDAG_AMBAOD_FC_REGRESSION_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod_fc_regression/bin/golden $(FLEXIDAG_AMBAOD_FC_REGRESSION_OUT_DIR)
