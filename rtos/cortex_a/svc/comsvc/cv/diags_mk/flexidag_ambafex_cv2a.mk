
.PHONY: flexidag_ambafex_cv2a flexidag_ambafex_cv2a_install

diags: flexidag_ambafex_cv2a

diags_install: flexidag_ambafex_install

flexidag_ambafex_cv2a:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_ambafex_cv2a
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambafex_cv2a-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_ambafex_cv2a-build

FLEXIDAG_AMBAFEX_OUT_DIR = $(O)/out/cv/flexidag_ambafex
flexidag_ambafex_install:
	$(Q)mkdir -p $(FLEXIDAG_AMBAFEX_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_ambafex_cv2a/bin/flexibin $(FLEXIDAG_AMBAFEX_OUT_DIR)
