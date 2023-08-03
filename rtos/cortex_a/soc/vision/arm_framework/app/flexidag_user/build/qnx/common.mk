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
PINFO DESCRIPTION="cv_flexidag_user"
endef

NAME := cv_flexidag_user

EXCLUDE_OBJS= schdr_resource_threadx.o

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../arm_framework/scheduler/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvsched_common
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cavalry
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvapi

include ../../../../../../cvchip.mk

LIBS = cache

CCFLAGS += -DDISABLE_ARM_CVTASK -DDISABLE_KAL
CCFLAGS += -D$(CVCHIP) -DAMBACV_KERNEL_SUPPORT -DLIBRARY_VERSION='"UNVERSIONED"' -DTOOL_VERSION='"UNVERSIONED"'
# Project specific end

USEFILE = $(PROJECT_ROOT)/Usemsg
INSTALLDIR = usr/lib

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
#USE_INSTALL_ROOT=1


include $(MKFILES_ROOT)/qtargets.mk

# Post-set make





