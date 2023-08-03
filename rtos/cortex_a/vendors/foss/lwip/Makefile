ifeq ($(CONFIG_BUILD_FOSS_LWIP),y)

# For debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))
#

obj-y += liblwip.a

LWIP_VER := lwip-2.1.3
LWIPDIR  := $(LWIP_VER)/src
include $(src)/$(LWIPDIR)/Filelists.mk
#	Minimum set
liblwip-objs := \
		$(patsubst %.c,%.o,$(COREFILES))	\
		$(patsubst %.c,%.o,$(CORE4FILES))

#	ipv6
#liblwip-objs += $(patsubst %.c,%.o,$(CORE6FILES))

#	APIFILES: The files which implement the sequential and socket APIs.
liblwip-objs += $(patsubst %.c,%.o,$(APIFILES))

#	NETIFFILES: Files implementing various generic network interface functions
#liblwip-objs += $(patsubst %.c,%.o,$(NETIFFILES))
ifeq ($(CONFIG_LWIP_ENET),y)
liblwip-objs += $(LWIPDIR)/netif/ethernet.o
endif

#	SIXLOWPAN: 6LoWPAN
#liblwip-objs += $(patsubst %.c,%.o,$(SIXLOWPAN))

#	PPPFILES: PPP
#liblwip-objs += $(patsubst %.c,%.o,$(PPPFILES))

#	SNMPFILES: SNMPv2c agent
#liblwip-objs += $(patsubst %.c,%.o,$(SNMPFILES))

#	HTTPFILES: HTTP server + client
#liblwip-objs += $(patsubst %.c,%.o,$(HTTPFILES))

#	MAKEFSDATA: MAKEFSDATA HTTP server host utility
#liblwip-objs += $(patsubst %.c,%.o,$(MAKEFSDATAFILES))

#	LWIPERFFILES: IPERF server
liblwip-objs += $(patsubst %.c,%.o,$(LWIPERFFILES))

#	SMTPFILES: SMTP client
#liblwip-objs += $(patsubst %.c,%.o,$(SMTPFILES))

#	SNTPFILES: SNTP client
#liblwip-objs += $(patsubst %.c,%.o,$(SNTPFILES))

#	MDNSFILES: MDNS responder
#liblwip-objs += $(patsubst %.c,%.o,$(MDNSFILES))

#	NETBIOSNSFILES: NetBIOS name server
#liblwip-objs += $(patsubst %.c,%.o,$(NETBIOSNSFILES))

#	TFTPFILES: TFTP server files
#liblwip-objs += $(patsubst %.c,%.o,$(TFTPFILES))


# Porting to Ambarella platform.
liblwip-objs += ambarella/sys_arch.o
ifeq ($(CONFIG_LWIP_ENET),y)
liblwip-objs += ambarella/netif/enetif.o
endif

ccflags-y := \
	-I$(srctree)/$(src)/ambarella/inc \
	-I$(srctree)/$(src)/$(LWIPDIR)/include


ccflags-y += -I$(srctree)/vendors/ambarella/inc \
		-I$(srctree)/vendors \
		-I$(srctree)

ifndef CONFIG_CC_USESTD
ccflags-y += -I$(srctree)/$(src)/ambarella/inc/std
endif

#ccflags-y += -DLWIP_DEBUG

install-files := liblwip.a
endif # $(CONFIG_BUILD_FOSS_LWIP)

