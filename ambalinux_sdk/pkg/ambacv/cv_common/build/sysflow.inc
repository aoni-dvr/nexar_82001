################################################################################
#
#                    convert sysflow table if it exist
#
# NOTE : cvchip.mk is not always included here, so SYSFLOW_TOOL is based on
# project, not on $(CVCHIP).
################################################################################
SYSFLOW = $(notdir $(basename $(wildcard *.csv)))
ifneq ("$(SYSFLOW)", "")
SYSFLOW_BIN    = $(addprefix $(BINDIR)/, $(SYSFLOW))
SYSFLOW_SFB    = $(addsuffix .sfb, $(SYSFLOW_BIN))
SYSFLOW_PDF    = $(addsuffix .pdf, $(SYSFLOW_BIN))
endif
sysflow: $(SYSFLOW_SFB) $(SYSFLOW_PDF)
SYSFLOW_TOOL   = $(shell tv2 -which sfc > /dev/null 2>&1; echo $$?)
ifeq ($(SYSFLOW_TOOL), 0)
    SYSFLOW_TOOL    = sfc
else
    SYSFLOW_TOOL    = sysflow_convert
endif

$(BINDIR)/%.sfb: %.csv
	@mkdir -p $(dir $@)
	$(SYSFLOW_TOOL) -I csv -O sfb -o $@ $<

$(BINDIR)/%.pdf: %.csv
	@mkdir -p $(dir $@)
	$(SYSFLOW_TOOL) -I csv -O dot $< | dot -Tpdf >> $@
