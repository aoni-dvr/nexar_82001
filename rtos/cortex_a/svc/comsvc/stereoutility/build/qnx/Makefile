# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

EXTRA_SRCVPATH := \
		  $(CURDIR)/../../src/common
include $(CURDIR)/../../../../../output/include/config/auto.conf

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS :=
SRCS += AmbaSTU_IF.c
SRCS += AmbaSTU_PGMPacker.c
SRCS += AmbaSTU_StringConverter.c
SRCS += AmbaSTU_AlgoTaskUtil.c
SRCS += AmbaSTU_FexTask.c
SRCS += AmbaSTU_FmaTask.c
SRCS += AmbaSTU_VoTask.c
SRCS += AmbaSTU_MvacTask.c
SRCS += AmbaSTU_SourceTask.c
SRCS += AmbaSTU_SinkTask.c



#EXTRA_INCVPATH := $(subst -I,,$(CAL_INC))
EXTRA_INCVPATH += $(CURDIR)/../../
EXTRA_INCVPATH += $(CURDIR)/../../inc/common
EXTRA_INCVPATH += $(CURDIR)/../../inc/common/parser
EXTRA_INCVPATH += $(CURDIR)/../../../print
EXTRA_INCVPATH += $(CURDIR)/../../../../../soc/io/src/$(AMBA_CHIP_ID)/qnx/inc
EXTRA_INCVPATH += $(CURDIR)/../../../imgcal
EXTRA_INCVPATH += $(CURDIR)/../../../imgcal/arch

#EXTRA_INCVPATH += $(CURDIR)/../../../../misc
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/ambarella/wrapper/kal/qnx/public
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(CURDIR)/../../../cv/cv_common/inc/svccvalgo/
EXTRA_INCVPATH += $(CURDIR)/../../../cv/cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(CURDIR)/../../../cv/cv_common/inc/cvapi
EXTRA_INCVPATH += $(CURDIR)/../../../../../soc/vision/cv_common/inc/
EXTRA_INCVPATH += $(CURDIR)/../../../../../soc/vision/cv_common/inc/cvsched_common
EXTRA_INCVPATH += $(CURDIR)/../../../../../soc/vision/cv_common/inc/cavalry
EXTRA_INCVPATH += $(CURDIR)/../../../../../soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(CURDIR)/../../../../../soc/vision/cv_common/inc/rtos

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk

