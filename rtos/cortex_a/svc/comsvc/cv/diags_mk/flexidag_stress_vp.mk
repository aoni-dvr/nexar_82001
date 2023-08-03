
.PHONY: flexidag_stress_vp flexidag_stress_vp_install

diags: flexidag_stress_vp

diags_install: flexidag_stress_vp_install

flexidag_stress_vp:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_stress_vp
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_stress_vp-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_stress_vp-build

flexidag_stress_vp_OUT_DIR = $(O)/out/cv/flexidag_stress_vp
flexidag_stress_vp_install:
	$(Q)mkdir -p $(flexidag_stress_vp_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_stress_vp/bin/flexibin $(flexidag_stress_vp_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_stress_vp/bin/golden $(flexidag_stress_vp_OUT_DIR)
