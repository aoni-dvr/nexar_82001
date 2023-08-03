# -*- makefile -*-

include $(CV_COMMON_DIR)/../arm_framework/cvchip.mk

##############################################################################

my_SOURCES		:= main.c
FBC_DIR			:= util/flexibin
FBC_SOURCES		:= $(CV_COMMON_DIR)/$(FBC_DIR)/$(PROJECT)/flexibin_create
FBC_TOOL		:= $(FBC_DIR)/flexibin_create

$(FBC_TOOL) : $(FBC_SOURCES)
	@echo "  Compiling helper tool $@"
	mkdir -p $(FBC_DIR)
	cp $(FBC_SOURCES) $(FBC_TOOL)

TOOL_SOURCES		+= $(FBC_SOURCES)
TOOLS			+= $(FBC_TOOL)

##############################################################################



