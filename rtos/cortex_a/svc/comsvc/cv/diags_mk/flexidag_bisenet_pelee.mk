
.PHONY: flexidag_bisenet_pelee flexidag_bisenet_pelee_install

diags: flexidag_bisenet_pelee

diags_install: flexidag_bisenet_pelee_install

flexidag_bisenet_pelee:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_bisenet_pelee
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_bisenet_pelee-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_bisenet_pelee-build

FLEXIDAG_BISENET_PELEE_OUT_DIR = $(O)/out/cv/flexidag_bisenet_pelee
flexidag_bisenet_pelee_install:
	$(Q)mkdir -p $(FLEXIDAG_BISENET_PELEE_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_bisenet_pelee/bin/flexibin $(FLEXIDAG_BISENET_PELEE_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_bisenet_pelee/bin/golden $(FLEXIDAG_BISENET_PELEE_OUT_DIR)
