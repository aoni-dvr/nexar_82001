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
PINFO DESCRIPTION="iCAM CNNTESTBED"
endef

NAME := icam_cnntestbed

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/entry
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/dataflow

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/entry
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc/dataflow
EXTRA_INCVPATH += $(srctree)/svc/apps/icam/core/src/draw/inc
EXTRA_INCVPATH += $(srctree)/svc/apps/icam/core/src/sys/inc
EXTRA_INCVPATH += $(srctree)/svc/apps/icam/main/src/config/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/cv_common/inc/cvapi
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/cv_common/inc/svccvalgo
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtUtils/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFD/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFDHeader/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtSSD/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtDetCls/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtFrcnnProc/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(srctree)/soc/vision/cv_common/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

INSTALLDIR = usr/lib

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
