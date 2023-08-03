
.PHONY: flexidag_ambaspufusion_02 flexidag_ambaspufusion_02_install

diags: flexidag_ambaspufusion_02

diags_install: flexidag_ambaspufusion_02_install

flexidag_ambaspufusion_02:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaspufusion_02
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufusion_02-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufusion_02-build

FLEXIDAG_AMBASPUFUSION_02_OUT_DIR = $(O)/out/cv/flexidag_ambaspufusion_02
flexidag_ambaspufusion_02_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBASPUFUSION_02_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufusion_02/bin/flexibin $(FLEXIDAG_AMBASPUFUSION_02_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufusion_02/bin/golden $(FLEXIDAG_AMBASPUFUSION_02_OUT_DIR)
