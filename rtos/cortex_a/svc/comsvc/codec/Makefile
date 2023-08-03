ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CODEC), y)

ifneq ($(wildcard $(srctree)/$(src)/lib/src/common/dataflow),)
include $(srctree)/$(src)/lib/src/common/dataflow/Makefile
endif

obj-y += libcomsvc_codec.a

ifneq ($(wildcard $(srctree)/$(src)/lib/src/common/audio),)
include $(srctree)/$(src)/lib/build/threadx/Makefile.audio
endif

ifneq ($(wildcard $(srctree)/$(src)/lib/src/common/player),)
include $(srctree)/$(src)/lib/build/threadx/Makefile.player
endif

ifneq ($(wildcard $(srctree)/$(src)/lib/src/common/recorder),)
include $(srctree)/$(src)/lib/build/threadx/Makefile.recorder
endif

ifneq ($(wildcard $(srctree)/$(src)/lib/src/common/refplayer),)
include $(srctree)/$(src)/lib/build/threadx/Makefile.refplayer
endif

ifneq ($(wildcard $(srctree)/$(src)/lib/src/common/utility),)
include $(srctree)/$(src)/lib/build/threadx/Makefile.utility
endif

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/audio/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/HL
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/svc/comsvc/cfs/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc/format
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc/stream
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/demuxer
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/fifo/inc
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif

ifeq ($(CONFIG_AMBA_RTSP),y)
ccflags-y += -I$(srctree)/svc/comsvc/net/inc
ccflags-y += -I$(srctree)/svc/comsvc/net/rtsp/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
endif

ifeq ($(CONFIG_LWIP_ENET),y)
ccflags-y += -I$(srctree)/vendors/foss/lwip/ambarella/inc
endif

install-files := libcomsvc_codec.a

endif


