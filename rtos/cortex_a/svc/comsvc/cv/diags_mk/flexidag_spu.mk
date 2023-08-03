
.PHONY: flexidag_spu flexidag_spu_install

diags: flexidag_spu

diags_install: flexidag_spu_install

flexidag_spu:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_spu
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_spu-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_spu-build

flexidag_spu_OUT_DIR = $(O)/out/cv/flexidag_spu
flexidag_spu_install:
	$(Q)mkdir -p $(flexidag_spu_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_spu/bin/flexibin $(flexidag_spu_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_spu/bin/golden $(flexidag_spu_OUT_DIR)
