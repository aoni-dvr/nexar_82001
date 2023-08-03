#### QNX

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

EXTRA_SRCVPATH := \
		  $(CURDIR)/../..
		  
include $(CURDIR)/../../../../../output/include/config/auto.conf

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := lz4.o
SRCS := lz4hc.o
SRCS += AmbaLZ4_IF.o

EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/foss/lz4/
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(CURDIR)/../../../../../vendors
EXTRA_INCVPATH += $(CURDIR)/../../../../../svc/comsvc/print

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
