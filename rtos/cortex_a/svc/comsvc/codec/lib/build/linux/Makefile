ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CODEC), y)

obj-y += libcomsvc_codec.a

include $(srctree)/$(src)/Makefile.audio
include $(srctree)/$(src)/Makefile.player
include $(srctree)/$(src)/Makefile.recorder
include $(srctree)/$(src)/Makefile.refplayer
include $(srctree)/$(src)/Makefile.utility

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
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
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


