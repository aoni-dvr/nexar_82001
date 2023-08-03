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

# Project specific start
define PINFO
PINFO DESCRIPTION="cv_SCAOpenOD"
endef

NAME := cv_SCAOpenOD

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvapi/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/svccvalgo
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc/cvsched_common
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc/cavalry
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../svc/comsvc/shell/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../svc/comsvc/misc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../svc/comsvc/print
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
# Project specific end

USEFILE = $(PROJECT_ROOT)/Usemsg
INSTALLDIR = usr/lib

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
#USE_INSTALL_ROOT=1


include $(MKFILES_ROOT)/qtargets.mk

# Post-set make

