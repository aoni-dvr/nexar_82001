SFC = sfc

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

include $(current_dir)/$(shell echo $(CONFIG_CV_CONFIG_FILE))

obj-y += $(patsubst %.csv,%.pdf,$(shell echo $(APP_CSV_FILES)))

.NOTPARALLEL:
$(obj)/%.pdf: $(current_dir)/%.csv
	@echo "  SFC     $@"
	@$(SFC) -O pdf -o $@ $<
