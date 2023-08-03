#
# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_Library_and_application.html
#

# Defaultl lines
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc

#
# Your Compiling options, enable by remove '#'
#
#CCFLAGS += -O2 \
#	   -Winline \
#	   -fomit-frame-pointer -fno-strength-reduce \
#	   -D_FILE_OFFSET_BITS=64

#
# Your Linking options, enable by remove '#'
#
# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
#LDFLAGS+=-Wl,--warn-once
#LDFLAGS+=-Wl,--allow-shlib-undefined

# cf. https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# Information to go into the *.pinfo file.
#
define PINFO
PINFO DESCRIPTION="ambarella comsvc codec libraries"
endef

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
# The basename() of the executable or library being built. Defaults to $(PROJECT).
# Do not add prefix (e.g. lib) or postfix (e.g. .so)
NAME := comsvc_exif

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
#USE_INSTALL_ROOT=1

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# Extra commands to run before and after the install target.
# Define your own macro without '#'
#define POST_INSTALL
#    @echo "Hello, it's POST_INSTALL"
#endef

# Defaultl lines
include $(MKFILES_ROOT)/qtargets.mk

# Post-set make

