
.PHONY: orc_scheduler_ar orc_scheduler_ar_install

diags: orc_scheduler_ar

diags_install: orc_scheduler_ar_install

orc_scheduler_ar:
	$(Q)rm -rf $(CV_DIAG_DIR)/orc_scheduler_ar
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) orc_scheduler_ar-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) orc_scheduler_ar-build

ORC_SCHEDULER_AR_OUT_DIR = $(O)/out/cv/orc_scheduler_ar
orc_scheduler_ar_install:
	$(Q)mkdir -p $(ORC_SCHEDULER_AR_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/orc_scheduler_ar/bin/visorc $(ORC_SCHEDULER_AR_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/orc_scheduler_ar/orc/build/visorc_cvtable.tbar $(ORC_SCHEDULER_AR_OUT_DIR)/visorc


