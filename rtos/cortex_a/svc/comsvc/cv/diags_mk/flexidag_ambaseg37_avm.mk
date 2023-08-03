
.PHONY: flexidag_ambaseg37_avm flexidag_ambaseg37_avm_install

diags: flexidag_ambaseg37_avm

diags_install: flexidag_ambaseg37_avm_install

flexidag_ambaseg37_avm:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaseg37_avm
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaseg37_avm-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaseg37_avm-build

flexidag_ambaseg37_avm_OUT_DIR = $(O)/out/cv/flexidag_ambaseg37_avm
flexidag_ambaseg37_avm_install:
	$(Q)mkdir -p $(flexidag_ambaseg37_avm_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaseg37_avm/bin/flexibin $(flexidag_ambaseg37_avm_OUT_DIR)
	# $(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaseg37_avm/bin/golden $(flexidag_ambaseg37_avm_OUT_DIR)
