# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.
ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif


include $(CURDIR)/../../../../../../output/include/config/auto.conf #FIXME


# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
EXTRA_SRCVPATH :=
SRCS :=

ifneq ($(wildcard $(CURDIR)/../../src/psd),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/psd
SRCS += psd/RefFlow_PSD.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/bsd),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/bsd
SRCS += bsd/RefFlow_BSD.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/ldws),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/ldws
SRCS += ldws/RefFlow_LDWS.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/ld),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/ld
SRCS += ld/RefFlow_LD.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/fc),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/fc
SRCS += fc/RefFlow_FC.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/rcta),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/rcta
SRCS += rcta/RefFlow_RCTA.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/sr),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/sr
SRCS += AmbaSRRef_Flow.o
endif
ifneq ($(wildcard $(CURDIR)/../../src/autocal),)
EXTRA_SRCVPATH += $(CURDIR)/../../src/autocal
SRCS += RefFlow_AutoCal.o
endif

#EXTRA_INCVPATH := $(subst -I,,$(CAL_INC))

EXTRA_INCVPATH += $(CURDIR)/../../../../adas/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/bsd
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/fc
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/ldws
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/ld
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/psd
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/rcta
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/sr
EXTRA_INCVPATH += $(CURDIR)/../../../../adas/refflow/inc/autocal
EXTRA_INCVPATH += $(CURDIR)/../../../../imgcal
EXTRA_INCVPATH += $(CURDIR)/../../../../imgcal/arch
EXTRA_INCVPATH += $(CURDIR)/../../../../misc
EXTRA_INCVPATH += $(CURDIR)/../../../../print
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(CURDIR)/../../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
