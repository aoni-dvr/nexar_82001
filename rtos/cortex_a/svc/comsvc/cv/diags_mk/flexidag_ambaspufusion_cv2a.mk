
.PHONY: flexidag_ambaspufusion_cv2a flexidag_ambaspufusion_cv2a_install

diags: flexidag_ambaspufusion_cv2a

diags_install: flexidag_ambaspufusion_install

flexidag_ambaspufusion_cv2a:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambaspufusion_cv2a
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufusion_cv2a-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambaspufusion_cv2a-build

FLEXIDAG_AMBASPUFUSION_OUT_DIR = $(O)/out/cv/flexidag_ambaspufusion
flexidag_ambaspufusion_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBASPUFUSION_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambaspufusion_cv2a/bin/flexibin $(FLEXIDAG_AMBASPUFUSION_OUT_DIR)
