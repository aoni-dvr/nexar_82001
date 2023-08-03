#
# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_Library_and_application.html
#

# Defaultl lines
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#
# Your Compiling options, enable by remove '#'
#
CCFLAGS += -DALTQ -O3 -DCONFIG_SOC_CV2 #FIXME
#CCFLAGS += -DALTQ -O3
# gcc sometime after 2.95.3 added a builtin log()
CCFLAGS_gcc += -fno-builtin-log
# currently doesnt handle individual builtins
CCFLAGS_clang += -fno-builtin
CCFLAGS += $(CCFLAGS_$(COMPILER_TYPE))

#
# Your Linking options, enable by remove '#'
#
# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
LDFLAGS += -Wl,--allow-shlib-undefined -Wl
#LDFLAGS+=-Wl,--warn-once
#LDFLAGS+=-Wl,--allow-shlib-undefined

# cf. https://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.neutrino.prog/topic/make_convent_qtargets.mk.html
# Information to go into the *.pinfo file.
#
define PINFO
PINFO DESCRIPTION="ambarella image calibration tuner library."
endef

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
# The basename() of the executable or library being built. Defaults to $(PROJECT).
# Do not add prefix (e.g. lib) or postfix (e.g. .so)
NAME := img_tuner

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
# INSTALLDIR = dev/null


# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
#USE_INSTALL_ROOT=1
LIBS = comsvc-ambarella-print kal-ambarella dsp_imgcal

#define POST_INSTALL
#  @echo "INSTALL libdsp_imgcal.a to vendors/ambarella/lib/$(AMBA_CHIP_ID)/qnx/"
#  @cp -f $(srctree)/soc/dsp/imgkernel/imgcal/aarch64/a.le/libdsp_imgcal.a $(srctree)/vendors/ambarella/lib/$(AMBA_CHIP_ID)/qnx/
#endef

include $(MKFILES_ROOT)/qtargets.mk
