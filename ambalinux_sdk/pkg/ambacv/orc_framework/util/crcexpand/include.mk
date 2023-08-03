# -*- makefile -*-

##############################################################################

CRCEXPAND_DIR		:= util/crcexpand
CRCEXPAND_SOURCES	:= util/crcexpand/crcexpand.c
CRCEXPAND_TOOL		:= $(CRCEXPAND_DIR)/crcexpand

$(CRCEXPAND_TOOL) : $(CRCEXPAND_SOURCES)
	@echo "Compiling helper tool: $@"
	mkdir -p $(CRCEXPAND_DIR)
	gcc -I./ -I$(CV_COMMON_DIR)/inc -I$(CV_COMMON_DIR)/inc/cvapi -o $@ $(filter %.c,$^)

TOOL_SOURCES		+= $(CRCEXPAND_SOURCES)
TOOLS			+= $(CRCEXPAND_TOOL)

