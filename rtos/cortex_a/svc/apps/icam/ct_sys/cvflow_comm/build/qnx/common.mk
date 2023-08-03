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
PINFO DESCRIPTION="cvflow_comm"
endef

USEFILE=$(PROJECT_ROOT)/cvflow_comm.use

NAME := cvflow_comm

LIBS = socket m cache
LIBS += cv_flexidag_user flexidagio SvcCvAlgoUtil
LIBS += CtUtils CtSSD CtDetCls CtFrcnnProc CtSingleFD CtSingleFDHeader

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/qnx

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/flexidagio/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtSSD/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtUtils/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtDetCls/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtSingleFD/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtFrcnnProc/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtSingleFDHeader/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/cv_common/inc/svccvalgo
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../soc/vision/cv_common/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../vendors/ambarella/inc

EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtSSD/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtUtils/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtDetCls/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtSingleFD/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtSingleFDHeader/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/CtFrcnnProc/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/cv/arm_framework/app/SvcCvAlgoUtil/build/qnx/$(CPU)/so.le

EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../comsvc/flexidagio/build/qnx/$(CPU)/so.le
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../../../../../../../soc/vision/arm_framework/app/flexidag_user/build/qnx/$(CPU)/so.le

INSTALLDIR = usr/bin

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
