################################################################################
#                                This is ThreadX build
ifeq ($(CONFIG_BUILD_CV_THREADX),y)
################################################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
OUTDIR := $(objtree)/$(obj)
CVTASK := $(notdir $(CURDIR))
.NOTPARALLEL:
.PHONY: force_build_info

CMNDIR        ?= $(srctree)/$(CV_CORE_DIR)/cv_common
BUILDINFO_CMD ?= $(CMNDIR)/build/create_build_version.sh

-include $(CURDIR)/private.mk

$(obj)/build_version.h: force_build_info
	@mkdir -p $(OUTDIR)
	@if [ -f $(BUILDINFO_CMD) ] ; then                                      \
	  $(BUILDINFO_CMD) $(CURDIR) $(OUTDIR) $(CMNDIR);                       \
	fi

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/transfer
ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform
ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform/threadx
ccflags-y += -I$(srctree)/platform/inc/driver/rtsl
ccflags-y += -I$(srctree)/platform/inc
ccflags-y += -I$(srctree)/platform/inc/driver
ccflags-y += -I$(srctree)/platform/fs/inc
ccflags-y += -I$(srctree)/comsvc/printf
ccflags-y += -I$(srctree)/comsvc/shell/inc
ccflags-y += -I$(srctree)/comsvc/misc
ccflags-y += -I$(srctree)/comsvc/shell/commands
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvsched_common
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

obj-y += libcv_$(CVTASK).a

ifndef C_SRCS
C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
endif

libcv_$(CVTASK)-objs = $(C_SRCS:%.c=%.o)
$(foreach m, $(libcv_$(CVTASK)-objs), $(obj)/$m): $(obj)/build_version.h

install-files += libcv_$(CVTASK).a

################################################################################
#                                  This is Linux build
else
################################################################################
CVTDIR := $(patsubst $(CVROOT)/%,%,$(CURDIR))
CVTASK := $(notdir $(CVTDIR))
.PHONY: $(CVTASK)

-include private.mk

all: lib tbar

ifneq ("$(wildcard $(CVTASK).mnft)","")
TBAR    = $(BINDIR)/cvtask/$(CVTDIR)/$(CVTASK).tbar
$(TBAR):  $(CVTASK).mnft
	@echo "        Building:     $@"
	@mkdir -p $(dir $@)
	@table_ar -c $@ -a $<
endif
tbar: $(TBAR)

ifndef SCHDAPI
SCHDAPI = $(abspath $(CVROOT)/../scheduler)
endif

ifndef HOSTAPI
HOSTAPI = $(abspath $(wildcard $(CVROOT)/../../../hostapi*))
endif

ifndef CMNDIR
CMNDIR = $(abspath $(CVROOT)/../../cv_common)
endif

ifndef C_SRCS
C_SRCS = $(wildcard *.c)
endif

ifndef CPP_SRCS
CPP_SRCS = $(wildcard *.cpp)
endif

OUTDIR  = $(OBJDIR)/$(CVTDIR)
OBJS    = $(addprefix $(OUTDIR)/, $(C_SRCS:.c=.o))
OBJS   += $(addprefix $(OUTDIR)/, $(CPP_SRCS:.cpp=.o))
ifeq ("$(STATIC_LINK)","y")
LIB     = $(BINDIR)/cvtask/$(CVTDIR)/lib$(CVTASK).a
else
LIB     = $(BINDIR)/cvtask/$(CVTDIR)/lib$(CVTASK).so
endif

$(LIB): $(OBJS)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
ifeq ("$(STATIC_LINK)","y")
ifeq ("$(CVTASK_HIDE_ALL)","y")
	$(LD) -o $(OUTDIR)/all.o -r $(OBJS)
	$(OBJCOPY) --localize-hidden $(OUTDIR)/all.o
	$(AR) rcs $@ $(OUTDIR)/all.o
else
	@$(AR) rcs $@ $(OBJS)
endif
else
	@$(CC) -shared -Wl,--as-needed -Wl,--warn-common -o $@ $(OBJS) -Wl,-soname,$(notdir $@)
endif

lib:
	@mkdir -p $(OUTDIR)
	@$(CMNDIR)/build/create_build_version.sh $(CURDIR) $(OUTDIR) $(CMNDIR)
	$(MAKE) $(LIB)

ifneq ("$(MAKECMDGOALS)","tbar")
-include $(OBJS:.o=.d)
endif

CFLAGS += -I$(OUTDIR) -D$(CV_OSTYPE) -I$(SCHDAPI)/inc -I$(HOSTAPI)/target/inc -I$(HOSTAPI)/common/inc -I$(CMNDIR)/stream -I$(CMNDIR)/inc/cvsched_common

ifeq ($(CVTASK_RELEASE_BUILD),1)
CFLAGS += -DCVTASK_RELEASE_BUILD
endif

ifeq ("$(CVTASK_HIDE_ALL)","y")
CFLAGS += -fvisibility=hidden
endif

ifeq ("$(CV_OSTYPE)","PACE_LINUX")
CFLAGS  += -DENV_IS_PACE_LINUX
endif

$(OUTDIR)/%.o: %.c
	@echo "        Compiling:    $(CURDIR)/$<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OUTDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OUTDIR)/$*.d

$(OUTDIR)/%.o: %.cpp
	@echo "        Compiling:    $(CURDIR)/$<"
	@mkdir -p $(dir $@)
	@$(CXX) -c $(CXXFLAGS) -o $@ $<
	@echo -n $(dir $@) >> $(OUTDIR)/$*.d
	@$(CXX) -MM $(CXXFLAGS) $< >> $(OUTDIR)/$*.d


################################################################################
#                                end of file
endif
################################################################################
