
.PHONY: flexidag_ambafma flexidag_ambafma_install

diags: flexidag_ambafma

diags_install: flexidag_ambafma_install

flexidag_ambafma:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambafma
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambafma-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambafma-build

FLEXIDAG_AMBAFMA_OUT_DIR = $(O)/out/cv/flexidag_ambafma
flexidag_ambafma_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBAFMA_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambafma/bin/flexibin $(FLEXIDAG_AMBAFMA_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambafma/bin/golden $(FLEXIDAG_AMBAFMA_OUT_DIR)
