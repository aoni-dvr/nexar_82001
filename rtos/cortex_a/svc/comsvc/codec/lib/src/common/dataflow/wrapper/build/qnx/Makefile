# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := \
		  $(CURDIR)/../..	\
		  $(CURDIR)/../../stream

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := \
	WrpFormatLib.o BufferStream.o

ifdef CONFIG_BUILD_COMSVC_MX_FMP4
SRCS += \
	SvcMp4Fmt.o
endif

ifdef CONFIG_BUILD_COMSVC_MX_EXIF
SRCS += \
	AmbaExifFormatter.o
endif

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
