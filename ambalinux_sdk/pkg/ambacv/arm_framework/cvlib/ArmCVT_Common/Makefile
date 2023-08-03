################################################################################
#                                This is ThreadX build
ifeq ($(CONFIG_BUILD_CV_THREADX),y)
################################################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
OUTDIR := $(objtree)/$(obj)
CVLIB  := $(notdir $(CURDIR))
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
	     -O3 -g

obj-y += build_version.h libcv_$(CVLIB).a

ifndef C_SRCS
C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
endif

libcv_$(CVLIB)-objs = $(C_SRCS:%.c=%.o)
install-files += libcv_$(CVLIB).a

################################################################################
#                                  This is Linux build
else
################################################################################
CVTDIR := $(patsubst $(CVROOT)/%,%,$(CURDIR))
CVLIB  := $(notdir $(CVTDIR))
.PHONY: $(CVLIB)

-include private.mk

all: lib tbar

ifneq ("$(wildcard $(CVLIB).mnft)","")
TBAR    = $(BINDIR)/cvlib/$(CVTDIR)/$(CVLIB).tbar
$(TBAR):  $(CVLIB).mnft
	@echo "        Building:     $@"
	@mkdir -p $(dir $@)
	@table_ar -c $@ -a $<
endif
tbar: $(TBAR)

ifndef C_SRCS
C_SRCS = $(wildcard *.c)
endif

ifndef AS_SRCS
AS_SRCS = $(wildcard *.S)
endif

ifndef CPP_SRCS
CPP_SRCS = $(wildcard *.cpp)
endif

CPU_PARAMS_FILE := $(CMNDIR)/../arm_framework/cvlib/Extern_NMS/arm_priorbox.bin
MODEL_ARTEFACTS := $(CPU_PARAMS_FILE) 
DEFINES = -DCPU_PARAMS=\"$(CPU_PARAMS_FILE)\"

OUTDIR  = $(OBJDIR)/$(CVTDIR)
OBJS    = $(addprefix $(OUTDIR)/, $(C_SRCS:.c=.o))
OBJS   += $(addprefix $(OUTDIR)/, $(AS_SRCS:.S=.o))
OBJS   += $(addprefix $(OUTDIR)/, $(CPP_SRCS:.cpp=.o))
ifeq ("$(STATIC_LINK)","y")
LIB     = $(BINDIR)/cvlib/$(CVTDIR)/lib$(CVLIB).a
else
LIB     = $(BINDIR)/cvlib/$(CVTDIR)/lib$(CVLIB).so
endif

$(LIB): $(OBJS)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
ifeq ("$(STATIC_LINK)","y")
	@$(AR) rcs $@ $(OBJS)
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

CFLAGS += -I$(OUTDIR) -D$(CV_OSTYPE)

ifeq ("$(CV_OSTYPE)","PACE_LINUX")
CFLAGS  += -DENV_IS_PACE_LINUX
endif

$(OUTDIR)/%.o: %.c
	@echo "        Compiling:    $(CURDIR)/$<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OUTDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OUTDIR)/$*.d

$(OUTDIR)/%.o: %.S $(MODEL_ARTEFACTS)
	@echo "        Compiling:    $(CURDIR)/$<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) $(DEFINES) -o $@ $<
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
