# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := \
		  $(CURDIR)/../../src/common/audio \
		  $(CURDIR)/../../src/common/player \
          $(CURDIR)/../../src/common/recorder \
          $(CURDIR)/../../src/common/refplayer \
          $(CURDIR)/../../src/common/utility \
		  $(CURDIR)/../../src/qnx

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS :=

ifeq ($(CONFIG_BUILD_COMAMBA_CODEC_AUDIO), y)
SRCS += AmbaAEncFlow.o AmbaADecFlow.o AmbaAudioBSBuf.o AmbaAudioBuf.o AmbaAudio.o AmbaAudioEfx.o
endif
ifeq ($(CONFIG_BUILD_COMSVC_PLAYER), y)
SRCS += AmbaDmux.o AmbaPlayer.o
endif
ifeq ($(CONFIG_BUILD_COMSVC_RECODER), y)
SRCS += AmbaRbxMP4.o AmbaRdtEmgEvt.o AmbaRdtFile.o AmbaRdtNmlEvt.o AmbaRecMaster.o AmbaRscData.o \
        AmbaRscVA.o AmbaRdtNet.o
endif
ifeq ($(CONFIG_BUILD_COMSVC_REFPLAYER), y)
SRCS += RefCmptDemuxer.o RefCmptPlayer.o
endif
ifeq ($(CONFIG_BUILD_COMAMBA_CODEC_COMMON), y)
SRCS += AmbaBitsFifo.o AmbaSvcGdma.o AmbaUtc.o AmbaVfs.o
endif

ifeq ($(CONFIG_ICAM_ENABLE_REGRESSION_FLOW),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/link_user
SRCS += AmbaRPCStreamWrap.o
else
SRCS += AmbaStreamWrap.o
endif

EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/audio/inc
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
EXTRA_INCVPATH += $(srctree)/soc/dsp/visual/inc/HL
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cfs/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc/format
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc/stream
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/demuxer
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/lib/src/common/dataflow/fifo/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif
ifeq ($(CONFIG_AMBA_RTSP),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/rtsp/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/threadx
endif
ifeq ($(CONFIG_LWIP_ENET),y)
EXTRA_INCVPATH += $(srctree)/vendors/foss/lwip/ambarella/inc
endif

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
