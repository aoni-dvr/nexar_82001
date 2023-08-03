
.PHONY: flexidag_imgproc flexidag_imgproc_install

diags: flexidag_imgproc

diags_install: flexidag_imgproc_install

flexidag_imgproc:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_imgproc
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_imgproc-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_imgproc-build

flexidag_imgproc_OUT_DIR = $(O)/out/cv/flexidag_imgproc
flexidag_imgproc_install:
	$(Q)mkdir -p $(flexidag_imgproc_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_imgproc/bin/flexibin $(flexidag_imgproc_OUT_DIR)
