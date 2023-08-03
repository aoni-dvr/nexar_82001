# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.


include $(CURDIR)/../../../../../../output/include/config/auto.conf #FIXME


# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
EXTRA_SRCVPATH :=
SRCS :=

ifneq ($(wildcard $(CURDIR)/../../src/ac),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/ac
SRCS += AmbaAcRef_Flow.c
endif

#EXTRA_INCVPATH := $(subst -I,,$(CAL_INC))

EXTRA_INCVPATH += $(CURDIR)/../../../../stereoutility/inc/common
EXTRA_INCVPATH += $(CURDIR)/../../../../stereoutility/refflow/inc/ac
EXTRA_INCVPATH += $(CURDIR)/../../../../misc
EXTRA_INCVPATH += $(CURDIR)/../../../../print
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/dsp/$(CONFIG_SOC_NAME)
EXTRA_INCVPATH += $(CURDIR)/../../../../cv/cv_common/inc/svccvalgo/
EXTRA_INCVPATH += $(CURDIR)/../../../../cv/cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(CURDIR)/../../../../cv/cv_common/inc/cvapi
EXTRA_INCVPATH += $(CURDIR)/../../../../../../soc/vision/cv_common/inc/
EXTRA_INCVPATH += $(CURDIR)/../../../../../../soc/vision/cv_common/inc/cvsched_common
EXTRA_INCVPATH += $(CURDIR)/../../../../../../soc/vision/cv_common/inc/cavalry
EXTRA_INCVPATH += $(CURDIR)/../../../../../../soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(CURDIR)/../../../../../../soc/vision/cv_common/inc/rtos
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/io
# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
