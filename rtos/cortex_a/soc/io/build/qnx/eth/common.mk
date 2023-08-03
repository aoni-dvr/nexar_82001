#
# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_Library_and_application.html
#

# Defaultl lines
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
IOPKT_HDR_PATH=$(QNX_TARGET)/usr/include/io-pkt
EXTRA_INCVPATH += $(IOPKT_HDR_PATH) $(IOPKT_HDR_PATH)/sys-nto
AMBA_PATH=$(PROJECT_ROOT)/../../../../..
EXTRA_INCVPATH += $(AMBA_PATH)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(AMBA_PATH)/soc/io/src/common/inc/reg
EXTRA_INCVPATH += $(AMBA_PATH)/soc/io/src/common/inc/csl
EXTRA_INCVPATH += $(AMBA_PATH)/soc/io/src/common/inc/rtsl

#
# Your Compiling options, enable by remove '#'
#
#CCFLAGS += -O2 \
#	   -Winline \
#	   -fomit-frame-pointer -fno-strength-reduce \
#	   -D_FILE_OFFSET_BITS=64
CCFLAGS += -D_KERNEL -DALTQ -Wno-format-extra-args

#
# Your Linking options, enable by remove '#'
#
# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
LDFLAGS+=-Wl,--warn-once
#LDFLAGS+=-Wl,--allow-shlib-undefined

LIBS = netdrvrS fdt

# cf. https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# Information to go into the *.pinfo file.
#
define PINFO
PINFO DESCRIPTION="eth driver"
endef

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
# The basename() of the executable or library being built. Defaults to $(PROJECT).
# Do not add prefix (e.g. lib) or postfix (e.g. .so)
NAME := eth-ambarella

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# The file containing the usage message for the application.
# Defaults to none for archives and shared objects and to $(PROJECT_ROOT)/$(NAME).use for executables.
# More information: https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.cookbook/topic/s1_basics_Usage_messages.html
#
USEFILE = $(PROJECT_ROOT)/Usemsg

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# The subdirectory where the executable or library is to be installed.
# Defaults to bin for executables, and lib/dll for DLLs.
# If you set it to /dev/null, then no installation is done.
INSTALLDIR = usr/lib

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
ifeq ($(AMBA_CHIP_ID),)
EXTRA_INCVPATH += $(AMBA_PATH)/vendors/ambarella/inc
CCFLAGS += -Wall -Wextra -include $(AMBA_PATH)/output/include/generated/autoconf.h
INSTALL_ROOT_nto = $(AMBA_PATH)/output/install.qnx
USE_INSTALL_ROOT=1
endif

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# Extra commands to run before and after the install target.
# Define your own macro without '#'
#define POST_INSTALL
#    @echo "Hello, it's POST_INSTALL"
#endef

# Defaultl lines
include $(MKFILES_ROOT)/qtargets.mk

# Post-set make

