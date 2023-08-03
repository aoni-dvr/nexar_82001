################################################################################
#                                This is ThreadX build
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CV),y)
################################################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
OUTDIR := $(objtree)/$(obj)
CVTASK := $(notdir $(CURDIR))
.NOTPARALLEL:
.PHONY: force_build_info

CMNDIR        ?= $(srctree)/$(CV_CORE_DIR)/cv_common
BUILDINFO_CMD ?= $(CMNDIR)/build/create_build_version.sh

$(obj)/build_version.h: force_build_info
	@mkdir -p $(OUTDIR)
	@if [ -f $(BUILDINFO_CMD) ] ; then                                      \
	  $(BUILDINFO_CMD) $(CURDIR) $(OUTDIR) $(CMNDIR);                       \
	fi

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

-include $(CURDIR)/private.mk

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
	@ctc $@ -m $<
endif
tbar: $(TBAR)

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

CFLAGS += -I$(OUTDIR)

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

