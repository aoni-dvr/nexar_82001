ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(CURDIR)/../../../../../../../../../../output/include/config/auto.conf

# Compiling options
CCFLAGS += -O2 \
	   -Winline \
	   -fomit-frame-pointer -fno-strength-reduce \
	   -D_FILE_OFFSET_BITS=64 \
	   -DCONFIG_QNX

# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
#LDFLAGS+=-Wl,--warn-once
LDFLAGS+=-Wl,--allow-shlib-undefined

# Project specific start
define PINFO
PINFO DESCRIPTION="cv_CtfwTask"
endef

NAME := cv_CtfwTask

SRCS += Ctfw_SinkTask.o
SRCS += Ctfw_SourceTask.o

ifneq ($(findstring $(CONFIG_SOC_CV2)$(CONFIG_SOC_CV2FS), y),)
## Stixel
SRCS += Ctfw_StixelTask.o
endif

ifneq ($(findstring $(CONFIG_SOC_CV2), y),)
SRCS += Ctfw_FexTask.o
SRCS += Ctfw_FmaTask.o
SRCS += Ctfw_SpuFexTask.o
SRCS += Ctfw_SpuFusionTask.o

## MVAC
SRCS += Ctfw_MvacVoTask.o
SRCS += Ctfw_MvacAcTask.o
endif

ifeq ($(CONFIG_SOC_CV2FS), y)
# spu offex
SRCS += Ctfw_OffexTask_CV2A.o
# monoVo
SRCS += Ctfw_MonoVoTask.o
endif

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


EXTRA_INCVPATH += $(PROJECT_ROOT)/../../inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../Ctfw/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/wrapper/kal/qnx
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc
# EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/cv_common/inc/rtos
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../soc/vision/arm_framework/scheduler/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../svc/comsvc/cv/arm_framework/app/SCAStixel
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../svc/comsvc/imgcal/arch
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../svc/comsvc/imgcal
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../../../vendors/ambarella/inc/dsp


# Project specific end

USEFILE = $(PROJECT_ROOT)/Usemsg
INSTALLDIR = usr/lib

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
#USE_INSTALL_ROOT=1


include $(MKFILES_ROOT)/qtargets.mk

# Post-set make

