# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

EXTRA_SRCVPATH := \
		  $(CURDIR)/../../src/common/parser \
		  $(CURDIR)/../../src/common
include $(CURDIR)/../../../../../../output/include/config/auto.conf#FIXME

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS :=
SRCS += AmbaCT_1DVigRule.o
SRCS += AmbaCT_1DVigTuner.o
SRCS += AmbaCT_AvmRule.o
SRCS += AmbaCT_AvmTuner.o
SRCS += AmbaCT_BpcRule.o
SRCS += AmbaCT_BpcTuner.o
SRCS += AmbaCT_CaRule.o
SRCS += AmbaCT_CaTuner.o
SRCS += AmbaCT_EmirRule.o
SRCS += AmbaCT_EmirTuner.o
SRCS += AmbaCT_LdccRule.o
SRCS += AmbaCT_LdccTuner.o
SRCS += AmbaCT_LdcRule.o
SRCS += AmbaCT_LdcTuner.o
SRCS += AmbaCT_Logging.o
SRCS += AmbaCT_OcCbRule.o
SRCS += AmbaCT_OcCbTuner.o
SRCS += AmbaCT_OcRule.o
SRCS += AmbaCT_OcTuner.o
ifeq ($(CONFIG_SOC_CV2), y)
SRCS += AmbaCT_StereoRule.o
SRCS += AmbaCT_StereoTuner.o
endif
ifeq ($(CONFIG_SOC_CV2FS), y)
SRCS += AmbaCT_StereoRule.o
SRCS += AmbaCT_StereoTuner.o
endif
SRCS += AmbaCT_TextHdlr.o

SRCS += AmbaCT_Parser.o
SRCS += AmbaCT_ProcRule.o
SRCS += AmbaCT_SystemApi.o

#EXTRA_INCVPATH := $(subst -I,,$(CAL_INC))
EXTRA_INCVPATH += $(CURDIR)/../../
EXTRA_INCVPATH += $(CURDIR)/../../inc/common
EXTRA_INCVPATH += $(CURDIR)/../../inc/common/parser
EXTRA_INCVPATH += $(CURDIR)/../../../../print
EXTRA_INCVPATH += $(CURDIR)/../../../../../../soc/io/src/$(AMBA_CHIP_ID)/qnx/inc

EXTRA_INCVPATH += $(CURDIR)/../../../../imgcal
EXTRA_INCVPATH += $(CURDIR)/../../../../imgcal/arch
EXTRA_INCVPATH += $(CURDIR)/../../../../misc
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/wrapper/kal/qnx/public
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)


# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
