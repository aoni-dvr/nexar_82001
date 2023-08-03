# -*- makefile -*-

##############################################################################

FMTRELOC_DIR     := util/fmtreloc
FMTRELOC_SOURCES := util/fmtreloc/fmtreloc.c
FMTRELOC_TOOL    := $(FMTRELOC_DIR)/fmtreloc

$(FMTRELOC_TOOL) : $(FMTRELOC_SOURCES)
	@echo "Compiling helper tool: $@"
	mkdir -p $(FMTRELOC_DIR)
	gcc -I./ -o $@ $(filter %.c,$^)

TOOL_SOURCES     += $(FMTRELOC_SOURCES)
TOOLS            += $(FMTRELOC_TOOL)

