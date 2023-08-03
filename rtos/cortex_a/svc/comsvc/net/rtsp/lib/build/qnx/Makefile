# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

COMSVC_RTSP_SRC = ../../src/common

EXTRA_SRCVPATH := \
		  $(CURDIR)/$(COMSVC_RTSP_SRC) \
		  $(CURDIR)/../../src/qnx

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
ifeq ($(CONFIG_AMBALINK_BOOT_OS),y)
SRCS := $(COMSVC_RTSP_SRC)/SvcRtspLink.o
else
SRCS := $(COMSVC_RTSP_SRC)/SvcAacSubsession.o
SRCS += $(COMSVC_RTSP_SRC)/SvcBase64.o
SRCS += $(COMSVC_RTSP_SRC)/SvcH264Subsession.o
SRCS += $(COMSVC_RTSP_SRC)/SvcMediaSession.o
SRCS += $(COMSVC_RTSP_SRC)/SvcMediaSubsession.o
SRCS += $(COMSVC_RTSP_SRC)/SvcRtcp.o
SRCS += $(COMSVC_RTSP_SRC)/SvcRtpSink.o
SRCS += $(COMSVC_RTSP_SRC)/SvcRtsp.o
SRCS += $(COMSVC_RTSP_SRC)/SvcRtspClntSession.o
SRCS += $(COMSVC_RTSP_SRC)/SvcRtspRandom.o
SRCS += $(COMSVC_RTSP_SRC)/SvcRtspServer.o
SRCS += $(COMSVC_RTSP_SRC)/SvcStreamMgr.o
endif

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/rtsp/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cfs/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ifeq ($(CONFIG_AMBALINK_BOOT_OS),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/inc/rpcprog
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/core/inc
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(srctree)/soc/io/src/common/inc/arm
endif
ifeq ($(CONFIG_NETX_ENET),y)
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/platform/threadx
endif
ifeq ($(CONFIG_LWIP_ENET),y)
# lwip
EXTRA_INCVPATH += $(srctree)/vendors/foss/lwip/ambarella/inc
endif

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
