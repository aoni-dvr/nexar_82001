################################################################################
#                                This is ThreadX build
ifeq ($(CONFIG_BUILD_CV),y)
################################################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
OUTDIR := $(objtree)/$(obj)
CVTASK := $(notdir $(CURDIR))
.NOTPARALLEL:
.PHONY: force_build_info

-include $(CURDIR)/private.mk

$(obj)/build_version.h: force_build_info
	@mkdir -p $(OUTDIR)
	@if [ -f $(BUILDINFO_CMD) ] ; then                                      \
	  $(BUILDINFO_CMD) $(CURDIR) $(OUTDIR) $(CMNDIR);                       \
	fi

ccflags-y += -I$(srctree)/vendors/ambarella/inc                                 \
	     -I$(srctree)/vendors/ambarella/inc/ssp                             \
	     -I$(srctree)/vendors/ambarella/inc/ssp/fs                          \
	     -I$(srctree)/vendors/ambarella/inc/ssp/prfile2                     \
	     -I$(srctree)/vendors/ambarella/inc/ssp/soc                         \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys                  \
	     -I$(srctree)/cv/cv_common/inc                                      \
	     -I$(srctree)/cv/cv_common/inc/cvapi                                \
	     -I$(OUTDIR)							\
	     -DTHREADX_BUILD -O3 -g

obj-y += build_version.h libcv_$(CVTASK).a

ifndef C_SRCS
C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
endif

libcv_$(CVTASK)-objs = $(C_SRCS:%.c=%.o)
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
LIB     = $(BINDIR)/cvtask/$(CVTDIR)/lib$(CVTASK).a

$(LIB): $(OBJS)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $(OBJS)

lib:
	@mkdir -p $(OUTDIR)
	@$(CMNDIR)/build/create_build_version.sh $(CURDIR) $(OUTDIR) $(CMNDIR)
	$(MAKE) $(LIB)

ifneq ("$(MAKECMDGOALS)","tbar")
-include $(OBJS:.o=.d)
endif

CFLAGS += -I$(OUTDIR) -D$(CV_OSTYPE) -I$(SCHDAPI)/inc -I$(HOSTAPI)/target/inc -I$(HOSTAPI)/common/inc -I$(CMNDIR)/stream

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
