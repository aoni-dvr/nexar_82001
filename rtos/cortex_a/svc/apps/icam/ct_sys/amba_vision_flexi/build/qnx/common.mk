ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

objoutdir = $(CURDIR)/../../../../../../../../../output

# Include system configuration
include $(objoutdir)/include/config/auto.conf

# Compiling options
CCFLAGS += -O2 \
	   -Winline \
	   -fomit-frame-pointer -fno-strength-reduce \
	   -D_FILE_OFFSET_BITS=64

ifeq ($(CONFIG_CV_EXTERNAL_MSG_THREAD),y)
CCFLAGS += -DCV_EXTERNAL_MSG_THREAD
endif

# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
#LDFLAGS+=-Wl,--warn-once
LDFLAGS+=-Wl,--allow-shlib-undefined

define PINFO
PINFO DESCRIPTION="amba_vision_flexi"
endef

USEFILE=$(PROJECT_ROOT)/amba_vision_flexi.use

NAME := amba_vision_flexi

LIBS = m cache
LIBS += cv_flexidag_user

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/core
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/core/qnx
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/armutil
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/armutil/qnx
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/CvCommFlexi

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/core
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/core/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/armutil
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/CvCommFlexi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../soc/vision/cv_common/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../vendors/ambarella/inc

EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../../soc/vision/arm_framework/app/flexidag_user/build/qnx/$(CPU)/so.le

INSTALLDIR = usr/bin

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

# Copy orcvp.bin to target directory
define POST_INSTALL
	cp -a $(objoutdir)/out/cv/orc_scheduler/visorc/orcvp.bin $(objoutdir)/install.qnx/aarch64le/bin
endef

include $(MKFILES_ROOT)/qtargets.mk
