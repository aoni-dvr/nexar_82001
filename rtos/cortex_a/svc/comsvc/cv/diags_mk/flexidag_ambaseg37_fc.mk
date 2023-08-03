
.PHONY: flexidag_ambaseg37_fc flexidag_ambaseg37_fc_install

diags: flexidag_ambaseg37_fc

diags_install: flexidag_ambaseg37_fc_install

flexidag_ambaseg37_fc:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaseg37_fc
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaseg37_fc-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaseg37_fc-build

AMBASEG37_FC_OUT_DIR = $(O)/out/cv/flexidag_ambaseg37_fc
flexidag_ambaseg37_fc_install:
	$(Q)mkdir -p $(AMBASEG37_FC_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaseg37_fc/bin/flexibin $(AMBASEG37_FC_OUT_DIR)
	$(Q)if [ "$(wildcard $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaseg37_fc/install_data/hltask_data/*.bin)" != "" ]; then \
		install -m 644 $(RTOS_DIR)/$(CV_COMSVC_DIR)/tests/flexidag_ambaseg37_fc/install_data/hltask_data/*.bin $(AMBASEG37_FC_OUT_DIR); \
	fi;
