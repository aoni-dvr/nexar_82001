
.PHONY: flexidag_ambaspufusion_024 flexidag_ambaspufusion_024_install

diags: flexidag_ambaspufusion_024

diags_install: flexidag_ambaspufusion_024_install

flexidag_ambaspufusion_024:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaspufusion_024
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufusion_024-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufusion_024-build

FLEXIDAG_AMBASPUFUSION_024_OUT_DIR = $(O)/out/cv/flexidag_ambaspufusion_024
flexidag_ambaspufusion_024_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBASPUFUSION_024_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufusion_024/bin/flexibin $(FLEXIDAG_AMBASPUFUSION_024_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufusion_024/bin/golden $(FLEXIDAG_AMBASPUFUSION_024_OUT_DIR)
