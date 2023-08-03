ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

obj-y += libnetstack_rtsp.a

COMSVC_RTSP_SRC = rtsp/lib/src/common

ifeq ($(CONFIG_RTSP_LINUX),y)
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtspLink.o
else
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcAacSubsession.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcPcmSubsession.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcBase64.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcH264Subsession.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcMediaSession.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcMediaSubsession.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtcp.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtpSink.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtsp.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtspClntSession.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtspRandom.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcRtspServer.o
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcStreamMgr.o

ifeq ($(CONFIG_AMBA_RTSP_HEVC),y)
libnetstack_rtsp-objs += $(COMSVC_RTSP_SRC)/SvcH265Subsession.o
endif

endif

ccflags-y += -I$(srctree)/svc/comsvc/net/rtsp/inc
ccflags-y += -I$(srctree)/svc/comsvc/cfs/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/inc
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

ifeq ($(CONFIG_RTSP_LINUX),y)
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc \
             -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog \
             -I$(srctree)/svc/comsvc/ambalink/core/inc \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
             -I$(srctree)/soc/io/src/common/inc/arm
endif

ifeq ($(CONFIG_NETX_ENET),y)
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform/threadx
endif

# LWIP
ifeq ($(CONFIG_LWIP_ENET),y)
ccflags-y += -I$(srctree)/vendors/foss/lwip/ambarella/inc
endif

install-files += libnetstack_rtsp.a
