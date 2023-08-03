
.PHONY: flexidag_ambaod_em flexidag_ambaod_em_install

diags: flexidag_ambaod_em

diags_install: flexidag_ambaod_em_install

flexidag_ambaod_em:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaod_em
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod_em-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaod_em-build

flexidag_ambaod_em_OUT_DIR = $(O)/out/cv/flexidag_ambaod_em
flexidag_ambaod_em_install:
	$(Q)mkdir -p $(flexidag_ambaod_em_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod_em/bin/flexibin $(flexidag_ambaod_em_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaod_em/bin/golden $(flexidag_ambaod_em_OUT_DIR)
