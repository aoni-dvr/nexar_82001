
.PHONY: flexidag_ambafex flexidag_ambafex_install

diags: flexidag_ambafex

diags_install: flexidag_ambafex_install

flexidag_ambafex:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambafex
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambafex-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambafex-build

FLEXIDAG_AMBAFEX_OUT_DIR = $(O)/out/cv/flexidag_ambafex
flexidag_ambafex_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBAFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambafex/bin/flexibin $(FLEXIDAG_AMBAFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambafex/bin/golden $(FLEXIDAG_AMBAFEX_OUT_DIR)
