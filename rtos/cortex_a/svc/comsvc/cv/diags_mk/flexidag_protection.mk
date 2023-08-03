
.PHONY: flexidag_protection flexidag_protection_install

diags: flexidag_protection

diags_install: flexidag_protection_install

flexidag_protection:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_protection
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_protection-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_protection-build

FLEXIDAG_protection_OUT_DIR = $(O)/out/cv/flexidag_protection
flexidag_protection_install:
	$(Q)mkdir -p $(FLEXIDAG_protection_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_protection/bin/flexibin $(FLEXIDAG_protection_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_protection/bin/golden $(FLEXIDAG_protection_OUT_DIR)
