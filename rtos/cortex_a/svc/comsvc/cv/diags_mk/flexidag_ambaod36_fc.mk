
.PHONY: flexidag_ambaod36_fc flexidag_ambaod36_fc_install

diags: flexidag_ambaod36_fc

diags_install: flexidag_ambaod36_fc_install

flexidag_ambaod36_fc:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaod36_fc
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod36_fc-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod36_fc-build

AMBAOD36_FC_OUT_DIR = $(O)/out/cv/flexidag_ambaod36_fc
flexidag_ambaod36_fc_install:
	$(Q)mkdir -p $(AMBAOD36_FC_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod36_fc/bin/flexibin $(AMBAOD36_FC_OUT_DIR)
	install -m 644 $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaod36_fc/arm/ambaod_cfg.bin $(AMBAOD36_FC_OUT_DIR)
	install -m 644 $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaod36_fc/arm/ambaodfc.lcs $(AMBAOD36_FC_OUT_DIR)
