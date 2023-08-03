
.PHONY: flexidag_ambaoffex_cv2a flexidag_ambaoffex_cv2a_install

diags: flexidag_ambaoffex_cv2a

diags_install: flexidag_ambaoffex_install

flexidag_ambaoffex_cv2a:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaoffex_cv2a
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaoffex_cv2a-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaoffex_cv2a-build

FLEXIDAG_AMBAOFFEX_OUT_DIR = $(O)/out/cv/flexidag_ambaoffex
flexidag_ambaoffex_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBAOFFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaoffex_cv2a/bin/flexibin $(FLEXIDAG_AMBAOFFEX_OUT_DIR)
