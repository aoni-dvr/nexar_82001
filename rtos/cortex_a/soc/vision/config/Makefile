ifeq ("$(wildcard $(srctree)/tools/cv/sfc)","")
SFC = sfc
else
SFC = $(srctree)/tools/cv/sfc
endif

include $(srctree)/$(CV_CORE_DIR)/config/$(shell echo $(CONFIG_CV_CONFIG_FILE))
obj-y += $(patsubst %.csv,%.pdf,$(shell echo $(APP_CSV_FILES)))

.NOTPARALLEL:
$(CV_CORE_DIR)/config/%.pdf: $(srctree)/$(CV_CORE_DIR)/config/%.csv
	@echo "  SFC     $@"
	@$(SFC) -O pdf -o $@ $<
