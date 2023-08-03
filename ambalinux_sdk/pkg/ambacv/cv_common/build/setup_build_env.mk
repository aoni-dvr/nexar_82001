# This makefile sets up build environments for unified ambacv build

MK_DIR := $(abspath $(lastword $(MAKEFILE_LIST))/..)
CV_DIR := $(abspath $(MK_DIR)/../..)

ifndef TC_DIR
$(error please define TC_DIR as toolchain directory)
endif

DIAG_MK  = $(CURDIR)/Makefile
DIAG_ENV = $(CURDIR)/.target_make_env

all: $(DIAG_MK) $(DIAG_ENV)

$(DIAG_MK): $(MK_DIR)/mkmakefile
	mkdir -p $(dir $@)
	@$< $(CV_DIR) $(dir $@)

$(DIAG_ENV): $(MK_DIR)/mktargetenv
	mkdir -p $(dir $@)
	@$< $(TC_DIR) $@