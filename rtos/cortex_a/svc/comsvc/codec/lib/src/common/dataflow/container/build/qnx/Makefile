# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := \
		  $(CURDIR)/../..	\
		  $(CURDIR)/../../demuxer	\
		  $(CURDIR)/../../index	\
		  $(CURDIR)/../../iso	\
		  $(CURDIR)/../../stream

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := \
	BitOp.o	\
	ByteOp.o	\
	FormatAPI.o	\
	H264.o	\
	H265.o	\
	MemByteOp.o

SRCS += \
	IndexCache.o

SRCS += \
	Tiff.o

ifdef CONFIG_BUILD_COMSVC_MUXER
SRCS += \
	Muxer.o
SRCS += \
	IsoMux.o
ifdef CONFIG_BUILD_COMSVC_MX_FMP4
SRCS += \
	Fmp4MuxImpl.o
SRCS += \
	Fmp4Mux.o
endif
ifdef CONFIG_BUILD_COMSVC_MX_EXIF
SRCS += \
	ExifMux.o
endif
endif

ifdef CONFIG_BUILD_COMSVC_DEMUXER
SRCS += \
	Demuxer.o	\
	DemuxerImpl.o	\
	DemuxerImage.o	\
	DemuxerMovie.o	\
	FrameFeeder.o	\
	FeedingRuleDefault.o	\
	FeedingRuleH264.o	\
	FeedingRuleH265.o
SRCS += \
	IsoDmx.o
ifdef CONFIG_BUILD_COMSVC_DX_FMP4
SRCS += \
	Fmp4DmxImpl.o	\
	Fmp4Idx.o
SRCS += \
	Fmp4Dmx.o
endif
ifdef CONFIG_BUILD_COMSVC_DX_MP4
SRCS += \
	Mp4DmxImpl.o	\
	Mp4Idx.o
SRCS += \
	Mp4Dmx.o
endif
ifdef CONFIG_BUILD_COMSVC_DX_EXIF
SRCS += \
	ExifDmx.o
endif

endif

ifdef CONFIG_BUILD_COMSVC_FILE
SRCS += \
	stream/File.o
endif

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
