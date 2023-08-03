
.PHONY: flexidag_ambaspufex flexidag_ambaspufex_install

diags: flexidag_ambaspufex

diags_install: flexidag_ambaspufex_install

flexidag_ambaspufex:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaspufex
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufex-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufex-build

FLEXIDAG_AMBASPUFEX_OUT_DIR = $(O)/out/cv/flexidag_ambaspufex
flexidag_ambaspufex_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBASPUFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufex/bin/flexibin $(FLEXIDAG_AMBASPUFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufex/bin/golden $(FLEXIDAG_AMBASPUFEX_OUT_DIR)
