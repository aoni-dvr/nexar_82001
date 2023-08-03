# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.
EXTRA_SRCVPATH := \
		  $(subst ambalink/build,ambalink/src,$(CURDIR))

EXTRA_INCVPATH := \
		  $(realpath $(subst ambalink/build,ambalink/src,$(CURDIR))/../../inc) \
		  $(srctree)/vendors/ambarella/inc \
		  $(srctree)/vendors/ambarella/inc/io \
		  $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
		  $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
		  $(srctree)/soc/io/src/common/inc \
		  $(srctree)/soc/io/src/common/inc/arm \
		  $(srctree)/svc/comsvc/shell/inc \
		  $(srctree)/svc/comsvc/print \
		  $(srctree)/svc/comsvc/misc \
		  $(srctree)/svc/comsvc/ambalink/inc \
		  $(srctree)/svc/comsvc/ambalink/core/inc \
		  $(srctree)/svc/comsvc/ambalink/core \
		  $(srctree)/svc/comsvc/ambalink/inc/rpcprog

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := \
	main.c \
	AmbaIPC_Vq.c \
	AmbaIPC_RPMSG.c \
	AmbaIPC_mmap.c \
	AmbaIPC_mpool.c \
	AmbaLinkMemoryCfg.c \
	AmbaIPC_crc32.c

# QNX doesn't need AmbaSafety_IPC.c
# ifeq ($(CONFIG_AMBALINK_RPMSG_ASIL),y)
# SRCS += $(srctree)/svc/comsvc/ambalink/core/AmbaSafety_IPC.c
# endif



# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk

