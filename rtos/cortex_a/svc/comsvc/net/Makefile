
#NOSTDINC_FLAGS := $(subst -nostdinc,,$(NOSTDINC_FLAGS))

ifeq ($(CONFIG_AMBA_RTSP),y)
ifneq ($(wildcard $(srctree)/$(src)/rtsp),)
include $(srctree)/$(src)/rtsp/lib/build/threadx/Makefile
endif
endif

ifeq ($(CONFIG_AMBA_AVDECC),y)
include $(srctree)/$(src)/avdecc/Makefile
endif

obj-y += libnetstack.a

libnetstack-objs += EnetUtility.o

ifneq ($(CONFIG_NETX_ENET),)
libnetstack-objs += netx/nx_enet_network_driver.o
libnetstack-objs += netx/NetXStack.o
endif

ifneq ($(CONFIG_AMBA_UDPIP),)
libnetstack-objs += demo/DemoArp.o
libnetstack-objs += demo/DemoIcmp.o
libnetstack-objs += demo/DemoUdp.o
libnetstack-objs += demo/DemoStack.o
endif

ifneq ($(CONFIG_AMBA_AVB),)
libnetstack-objs += avb/AvbStack.o
libnetstack-objs += avb/gptp/AvbPtp.o
libnetstack-objs += avb/gptp/AvbPtpP2P.o
libnetstack-objs += avb/gptp/AvbPtpE2E.o
libnetstack-objs += avb/avtp/AvbAvtp.o
libnetstack-objs += avb/avtp/H264Subsession.o
libnetstack-objs += avb/avtp/ControlSubsession.o
libnetstack-objs += avb/avtp/BufQueue.o
endif

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/$(src)/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/$(src)/avb/avtp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx

install-files   += libnetstack.a
