
.PHONY: flexidag_bisenet_mnv2_raw flexidag_bisenet_mnv2_raw_install

diags: flexidag_bisenet_mnv2_raw

diags_install: flexidag_bisenet_mnv2_raw_install

flexidag_bisenet_mnv2_raw:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_bisenet_mnv2_raw
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_bisenet_mnv2_raw-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_bisenet_mnv2_raw-build

FLEXIDAG_BISENET_MNV2_RAW_OUT_DIR = $(O)/out/cv/flexidag_bisenet_mnv2_raw
flexidag_bisenet_mnv2_raw_install:
	$(Q)mkdir -p $(FLEXIDAG_BISENET_MNV2_RAW_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_bisenet_mnv2_raw/bin/flexibin $(FLEXIDAG_BISENET_MNV2_RAW_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_bisenet_mnv2_raw/bin/golden $(FLEXIDAG_BISENET_MNV2_RAW_OUT_DIR)
