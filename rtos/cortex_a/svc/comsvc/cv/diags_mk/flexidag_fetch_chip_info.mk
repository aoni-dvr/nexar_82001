
.PHONY: flexidag_fetch_chip_info flexidag_fetch_chip_info_install

diags: flexidag_fetch_chip_info

diags_install: flexidag_fetch_chip_info_install

flexidag_fetch_chip_info:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_fetch_chip_info
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_fetch_chip_info-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_fetch_chip_info-build

flexidag_fetch_chip_info_OUT_DIR = $(O)/out/cv/flexidag_fetch_chip_info
flexidag_fetch_chip_info_install:
	$(Q)mkdir -p $(flexidag_fetch_chip_info_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_fetch_chip_info/bin/flexibin $(flexidag_fetch_chip_info_OUT_DIR)
