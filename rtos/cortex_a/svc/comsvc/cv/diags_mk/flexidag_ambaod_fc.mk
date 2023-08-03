
.PHONY: flexidag_ambaod_fc flexidag_ambaod_fc_install

diags: flexidag_ambaod_fc

diags_install: flexidag_ambaod_fc_install

flexidag_ambaod_fc:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaod_fc
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod_fc-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod_fc-build

flexidag_ambaod_fc_OUT_DIR = $(O)/out/cv/flexidag_ambaod_fc
flexidag_ambaod_fc_install:
	$(Q)mkdir -p $(flexidag_ambaod_fc_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod_fc/bin/flexibin $(flexidag_ambaod_fc_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod_fc/bin/golden $(flexidag_ambaod_fc_OUT_DIR)
