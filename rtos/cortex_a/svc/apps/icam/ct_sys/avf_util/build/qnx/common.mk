ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

# Compiling options
CCFLAGS += -O2 \
	   -Winline \
	   -fomit-frame-pointer -fno-strength-reduce \
	   -D_FILE_OFFSET_BITS=64

# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
#LDFLAGS+=-Wl,--warn-once
LDFLAGS+=-Wl,--allow-shlib-undefined

define PINFO
PINFO DESCRIPTION="amba_vision_flexi utility"
endef

USEFILE=$(PROJECT_ROOT)/avf_util.use

NAME := avf_util

LIBS = io-mw

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

INSTALLDIR = usr/bin

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
