
.PHONY: flexidag_ambaspufex_cv2a flexidag_ambaspufex_cv2a_install

diags: flexidag_ambaspufex_cv2a

diags_install: flexidag_ambaspufex_install

flexidag_ambaspufex_cv2a:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaspufex_cv2a
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufex_cv2a-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufex_cv2a-build

FLEXIDAG_AMBASPUFEX_OUT_DIR = $(O)/out/cv/flexidag_ambaspufex
flexidag_ambaspufex_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBASPUFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufex_cv2a/bin/flexibin $(FLEXIDAG_AMBASPUFEX_OUT_DIR)
