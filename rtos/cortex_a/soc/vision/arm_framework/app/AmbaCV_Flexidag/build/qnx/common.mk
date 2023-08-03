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
PINFO DESCRIPTION="cv_AmbaCV_Flexidag"
endef

NAME := cv_AmbaCV_Flexidag

EXCLUDE_OBJS= schdr_resource_threadx.o

include ../../../../../../cvchip.mk

ifeq ($(CONFIG_SOC_CV1),y)
ifneq ($(CVCHIP),CHIP_CV1)
$(error PROJECT is not cv1 !!!)
endif

else ifeq ($(CONFIG_SOC_CV2),y)
ifneq ($(CVCHIP),CHIP_CV2)
$(error PROJECT is not cv2 !!!)
endif

else ifeq ($(CONFIG_SOC_CV22),y)
ifneq ($(CVCHIP),CHIP_CV22)
$(error PROJECT is not cv2s or cv22 !!!)
endif

else ifeq ($(CONFIG_SOC_CV25),y)
ifneq ($(CVCHIP),CHIP_CV25)
$(error PROJECT is not cv25 !!!)
endif

else ifeq ($(CONFIG_SOC_CV28),y)
ifneq ($(CVCHIP),CHIP_CV28)
$(error PROJECT is not cv28 !!!)
endif

else ifeq ($(CONFIG_SOC_CV2FS),y)
ifeq ($(CVCHIP),CHIP_CV2FS)
else ifeq ($(CVCHIP),CHIP_CV22FS)
else ifeq ($(CVCHIP),CHIP_CV2A)
else
$(error PROJECT is not cv2fs or cv22fs !!!)
endif

else ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52),y)
ifneq ($(CVCHIP),CHIP_CV5)
$(error PROJECT is not cv5/cv52 !!!)
endif
endif

ifeq ($(CONFIG_BUILD_MAL),y)
CCFLAGS += -DENABLE_AMBA_MAL
endif

CCFLAGS += -DUSE_AMBA_KAL -D$(CVCHIP)

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvsched_common
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cavalry
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../cv_common/inc/cvapi
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../svc/comsvc/misc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../svc/comsvc/print
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../svc/comsvc/shell/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../vendors/ambarella/inc/io
# Project specific end

USEFILE = $(PROJECT_ROOT)/Usemsg
INSTALLDIR = usr/lib

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
#USE_INSTALL_ROOT=1


include $(MKFILES_ROOT)/qtargets.mk

# Post-set make





