
.PHONY: flexidag_ambaseg_fc flexidag_ambaseg_fc_install

diags: flexidag_ambaseg_fc

diags_install: flexidag_ambaseg_fc_install

flexidag_ambaseg_fc:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaseg_fc
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaseg_fc-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaseg_fc-build

flexidag_ambaseg_fc_OUT_DIR = $(O)/out/cv/flexidag_ambaseg_fc
flexidag_ambaseg_fc_install:
	$(Q)mkdir -p $(flexidag_ambaseg_fc_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaseg_fc/bin/flexibin $(flexidag_ambaseg_fc_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaseg_fc/bin/golden $(flexidag_ambaseg_fc_OUT_DIR)
