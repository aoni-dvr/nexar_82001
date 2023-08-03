
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_NET),y)
obj-y += libenet_refcode.a

ifeq ($(CONFIG_LWIP_ENET),y)
obj-y += libenet_reflwip.a
libenet_reflwip-objs += RefCode_LwIP.o
endif

libenet_refcode-objs += AmbaShell_Enet.o
libenet_refcode-objs += AmbaUserEnetConfig.o

ifeq ($(CONFIG_AMBA_AVTP_REFCODE),y)
libenet_refcode-objs += RefCode_Avtp.o
libenet_refcode-objs += RefCode_FileFeed.o
endif


ifeq ($(CONFIG_NETX_ENET),y)
libenet_refcode-objs += RefCode_NetX.o
endif

ifneq ($(AVDECC_REFCODE),)
libenet_refcode-objs += RefCode_Avdecc.o
#libenet_refcode-objs += AmbaShell_AvbSvc.o
libenet_refcode-objs += \
	avdecc/src/util.o \
	avdecc/src/avdecc.o \
	avdecc/src/adp.o \
	avdecc/src/acmp.o \
	avdecc/src/aecp.o \
	avdecc/src/talker.o \
	avdecc/src/listener.o
endif

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc/
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/debugport
ccflags-y += -I$(srctree)/vendors/foss/lwip/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/foss/lwip/ambarella/inc/std
ccflags-y += -I$(srctree)/svc/comsvc/net/inc
ccflags-y += -I$(srctree)/$(src)/

ifeq ($(CONFIG_AMBA_AVB),y)
ccflags-y += \
        -I$(src)/avdecc/inc
endif

install-files   += libenet_refcode.a
ifeq ($(CONFIG_LWIP_ENET),y)
install-files   += libenet_reflwip.a
endif
endif
