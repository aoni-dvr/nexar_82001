# -*- makefile -*-

##############################################################################

FLEXIPATCH_DIR          := util/create_flexipatch
FLEXIPATCH_SOURCES      := $(ORC_COMMON_DIR)/../flexidag/util/create_flexipatch
FLEXIPATCH_TOOL         := $(FLEXIPATCH_DIR)/create_flexipatch

$(FLEXIPATCH_TOOL) : $(FLEXIPATCH_SOURCES)
	@echo "Compiling helper tool: $@"
	mkdir -p $(FLEXIPATCH_DIR)
	cp $(FLEXIPATCH_SOURCES) $(FLEXIPATCH_TOOL)

TOOL_SOURCES            += $(FLEXIPATCH_SOURCES)
TOOLS                   += $(FLEXIPATCH_TOOL)
