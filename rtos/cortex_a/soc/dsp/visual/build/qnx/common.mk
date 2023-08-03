ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

# Compiling options
CCFLAGS += -O2 \
	   -Winline \
	   -fomit-frame-pointer -fno-strength-reduce \
	   -D_FILE_OFFSET_BITS=64

EXCLUDE_OBJS=

# module description
NAME := devdsp
USEFILE = $(PROJECT_ROOT)/Usemsg
define PINFO
PINFO DESCRIPTION="Driver for dsp visual Interface"
endef
INSTALLDIR = usr/lib

LIBS = cache wrap_std m kal-ambarella comsvc-ambarella-misc comsvc-ambarella-print
ifdef CONFIG_DEVICE_TREE_SUPPORT
LIBS += fdt
endif
ifdef CONFIG_ENABLE_DSP_MONITOR
LIBS += c socket
LIBS += rpmsg_api
LIBPREF_rpmsg_api = -Bstatic
LIBPOST_rpmsg_api = -Bdynamic
endif

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
