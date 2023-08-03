# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.
EXTRA_SRCVPATH := \
		  $(CURDIR)/../../src/common/libc \
		  $(CURDIR)/../../src/common/libm \
		  $(CURDIR)/../../src/qnx

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := \
	AmbaMemcpy.o	\
	\
	abs.o	\
	memcmp.o	\
	memmove.o	\
	memset.o	\
	rand.o		\
	srand.o		\
	\
	acos.o		\
	asin.o		\
	atan.o		\
	atan2.o		\
	ceil.o		\
	cos.o		\
	fabs.o		\
	floor.o		\
	fmod.o		\
	log10.o		\
	log2.o		\
	pow.o		\
	sin.o		\
	sqrt.o		\
	tan.o

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
