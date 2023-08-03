
.PHONY: flexidag_mnetssd_sensor flexidag_mnetssd_sensor_install

diags: flexidag_mnetssd_sensor

diags_install: flexidag_mnetssd_sensor_install

flexidag_mnetssd_sensor:
	$(Q)rm -rf $(CV_DIAG_DIR)/flexidag_mnetssd_sensor
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_mnetssd_sensor-init
	$(Q)$(MAKE) -C $(CV_DIAG_DIR) flexidag_mnetssd_sensor-build

FLEXIDAG_MNETSSD_SENSOR_OUT_DIR = $(O)/out/cv/flexidag_mnetssd_sensor
flexidag_mnetssd_sensor_install:
	$(Q)mkdir -p $(FLEXIDAG_MNETSSD_SENSOR_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_mnetssd_sensor/bin/flexibin $(FLEXIDAG_MNETSSD_SENSOR_OUT_DIR)
	$(Q)cp -rLf $(CV_DIAG_DIR)/flexidag_mnetssd_sensor/bin/golden $(FLEXIDAG_MNETSSD_SENSOR_OUT_DIR)
	$(Q)install -m 644 $(RTOS_DIR)/$(CV_COMSVC_DIR)/arm_framework/cvtask/HLMobilenetSSD/prior_box/mbox_priorbox.bin $(FLEXIDAG_MNETSSD_SENSOR_OUT_DIR)
