
.PHONY: flexidag_yield_test flexidag_yield_test_install

diags: flexidag_yield_test

diags_install: flexidag_yield_test_install

flexidag_yield_test:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_yield_test
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_yield_test-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_yield_test-build

FLEXIDAG_YIELD_TEST_OUT_DIR = $(O)/out/cv/flexidag_yield_test
flexidag_yield_test_install:
	$(Q)mkdir -p $(FLEXIDAG_YIELD_TEST_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_yield_test/bin/flexibin $(FLEXIDAG_YIELD_TEST_OUT_DIR)
