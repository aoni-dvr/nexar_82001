
.PHONY: orc_scheduler orc_scheduler_install

diags: orc_scheduler

diags_install: orc_scheduler_install

orc_scheduler:
	$(Q)rm -rf $(CV_DIAG_DIR)/orc_scheduler
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) orc_scheduler-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) orc_scheduler-build

ORC_SCHEDULER_OUT_DIR = $(O)/out/cv/orc_scheduler
orc_scheduler_install:
	$(Q)mkdir -p $(ORC_SCHEDULER_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/orc_scheduler/bin/visorc $(ORC_SCHEDULER_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/orc_scheduler/orc/build/visorc_cvtable.tbar $(ORC_SCHEDULER_OUT_DIR)/visorc


