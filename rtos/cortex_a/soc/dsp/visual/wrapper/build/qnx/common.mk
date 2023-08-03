ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

EXCLUDE_OBJS=

# module description
NAME := dsp_wrapper
USEFILE = $(PROJECT_ROOT)/Usemsg
define PINFO
PINFO DESCRIPTION="Wrapper for dsp visual Interface"
endef
INSTALLDIR = usr/lib

LIBS = io-mw dsp_imgkernel dsp_imgkernelcore
ifeq ($(CONFIG_SOC_CV2FS),y)
LIBS += dsp_imgkernel_similar
endif

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
