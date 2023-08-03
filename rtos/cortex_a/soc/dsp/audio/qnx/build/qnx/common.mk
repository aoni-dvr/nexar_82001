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
NAME := dsp_audio
USEFILE = $(PROJECT_ROOT)/Usemsg
define PINFO
PINFO DESCRIPTION="Driver for dsp audio Interface"
endef
INSTALLDIR = usr/lib

LIBS = cache kal-ambarella comsvc-ambarella-misc comsvc-ambarella-print asound

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
