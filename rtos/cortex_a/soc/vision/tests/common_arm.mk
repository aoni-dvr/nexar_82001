# This makefile is included and executed in each diag's "arm" directory
# Therefore the $(CURDIR) is ambacv/tests/foo/arm

TOPDIR := $(abspath ../../..)
FMWDIR  = $(TOPDIR)/arm_framework
CMNDIR  = $(TOPDIR)/cv_common
BINDIR  = $(BUILDDIR)/bin/app/$(APP_NAME)

-include private.mk

ifndef APP_NAME
$(error Please define APP_NAME)
endif

ifndef BUILDDIR
$(error Please define BUILDDIR)
endif

ifndef APP_TYPE
$(error Please define APP_TYPE)
endif

# set up make environment for linux-flavor build
ifeq ("$(APP_TYPE)", "linux")
BUILD_ENV_FILE   := $(abspath $(BUILDDIR)/../.target_make_env)
ifneq ("$(wildcard $(BUILD_ENV_FILE))","")
TARGET_MAKE_ENV := AMBACV_KERNEL_SUPPORT=y $(shell cat $(BUILD_ENV_FILE))
else
$(warning "Can't set up BACE build env, build ARM binary in PACE mode")
endif
endif

ifndef PREBUILDDIR
PREBUILDDIR := $(shell cat $(CMNDIR)/.release_lib_root)/arm/$(APP_TYPE)
endif
export PREBUILDDIR

# set up CVTASK_LIST_INC for building the app
ifndef CVTASK_LIST_INC
ifneq ($(wildcard cvtask_list.inc),)
CVTASK_LIST_INC := $(CURDIR)/cvtask_list.inc
else ifneq ($(wildcard $(BUILDDIR)/../arm/cvtask_list.inc),)
CVTASK_LIST_INC := $(BUILDDIR)/../arm/cvtask_list.inc
else
$(error "CVTASK_LIST_INC is not explicitly defined and no local file found")
endif
endif
export CVTASK_LIST_INC

.PHONY: app sysflow cvtable mpxcopy

all: app sysflow cvtable mpxcopy

include $(CMNDIR)/build/cvtable.inc
include $(CMNDIR)/build/sysflow.inc

ifeq ("$(APP_TYPE)", "rtos")
# In rtos build, app build flow is in another path.
app:
else
app:
	@echo ""
	@echo "===============Building ARM code================"
	@echo "  release library:    $(PREBUILDDIR)"
	@echo ""
	$(TARGET_MAKE_ENV) $(MAKE) -C $(FMWDIR) -f $(APP_TYPE).mk $(APP_NAME)
endif

sysflow: app

cvtable: app


mpxcopy:
ifneq ("$(wildcard *.mpx)","")
	@mkdir -p $(BINDIR)
	cp *.mpx $(BINDIR)
endif