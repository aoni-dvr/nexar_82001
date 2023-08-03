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
PINFO DESCRIPTION="CNN Testbed Algorithm Module"
endef

NAME := CtSingleFDHeader

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../CtUtils/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/svccvalgo
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc/io

INSTALLDIR = usr/lib

include $(MKFILES_ROOT)/qtargets.mk
