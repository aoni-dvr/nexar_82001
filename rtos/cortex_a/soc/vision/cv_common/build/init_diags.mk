# This makefile sets up build environments for unified ambacv build

MK_DIR := $(abspath $(lastword $(MAKEFILE_LIST))/..)
ifndef CV_DIR
	CV_DIR := $(abspath $(MK_DIR)/../..)
endif

ifeq ($(CONFIG_BUILD_CV_THREADX),y)
# This is ThreadX build
DIAG_MK = $(objtree)/$(obj)/diags/Makefile
else
# This is Linux
DIAG_MK = $(BASE_DIR)/diags/Makefile
endif

all: $(DIAG_MK)

$(DIAG_MK): $(MK_DIR)/mkmakefile
	mkdir -p $(dir $@)
	@$< $(CV_DIR) $(dir $@)
