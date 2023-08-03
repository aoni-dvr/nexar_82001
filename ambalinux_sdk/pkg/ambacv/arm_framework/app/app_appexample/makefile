ifndef APP
APP := $(notdir $(abspath $(lastword $(MAKEFILE_LIST)/..)))
endif

.PHONY: $(APP) sysflow cvtable mpxcopy

LMKDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TOPDIR := $(abspath $(LMKDIR)/../../..)
FMWDIR := $(TOPDIR)/arm_framework
MKNAME := $(notdir $(lastword $(MAKEFILE_LIST)))

ifndef CMNDIR
CMNDIR = $(TOPDIR)/cv_common
endif

ifndef BUILDDIR
BUILDDIR = $(TOPDIR)/build
endif

include $(CMNDIR)/build/cvtask_lib.inc

TOP_BINDIR = $(BUILDDIR)/bin

OBJDIR = $(BUILDDIR)/output/app/$(APP)
BINDIR = $(TOP_BINDIR)/app/$(APP)

APP_SRCS := $(wildcard $(LMKDIR)/*.c)
APP_CPP_SRCS := $(wildcard $(LMKDIR)/*.cpp)

all: app sysflow mpxcopy cvtable

include $(CMNDIR)/build/cvtask_lib.inc
include $(CMNDIR)/build/cvtable.inc
include $(CMNDIR)/build/sysflow.inc
include $(CMNDIR)/build/mpxcopy.inc
include $(CMNDIR)/build/linker_script.inc

CFLAGS := -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi -D$(CVCHIP) -I$(TOPDIR)/../ipcdef
LFLAGS := -L$(TOP_BINDIR)/lib -L$(PREBUILDDIR)
LFLAGS += -pthread -lambadag -lm -lrt
ifdef CV_COMMON_LFLAGS
LFLAGS += $(CV_COMMON_LFLAGS)
endif
ifeq ($(ENABLE_CVTASK_CAMCTRL_IPC),y)
LFLAGS += -lambaipc -laipc_cfg -lambatransfer -lambaflexidagio
endif

LFLAGS += -lstdc++ -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
LFLAGS += -Xlinker -Map=$(OBJDIR)/$(APP).map

APP_OBJS   = $(subst $(LMKDIR),$(OBJDIR),$(APP_SRCS:.c=.o))
APP_OBJS  += $(subst $(LMKDIR),$(OBJDIR),$(APP_CPP_SRCS:.cpp=.o))

ifneq ("$(wildcard $(TOP_BINDIR)/lib/libambadag.a)","")
SCHEDULER_LIB = $(TOP_BINDIR)/lib/libambadag.a
endif

CXXFLAGS += -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi -D$(CVCHIP)
CXXFLAGS += -std=c++11 -I$(STAGING_DIR)/usr/include/opencv4

CFLAGS += -g -rdynamic
LFLAGS += -g -rdynamic
CXXFLAGS += -g -rdynamic

#$(LFLAGS) has to put after $(CVTASK_LIBS_FLAGS) 
app: $(BINDIR)/$(APP)
$(BINDIR)/$(APP): $(APP_OBJS) $(APP_LSR) $(SCVTASK_LIBS) $(SCVLIB_LIBS) $(SCHEDULER_LIB)
	@echo "        Building App: $@"
	@mkdir -p $(dir $@)
	$(CC) -o $@ -T $(APP_LSR) $(APP_OBJS) $(CVTASK_LIBS_FLAGS) $(LFLAGS)
ifeq ("$(STATIC_LINK)","n")
ifneq ("$(LCVTASK_LIBS) $(LCVLIB_LIBS)"," ")
	install -m 755 -D -t $(BUILDDIR)/bin/lib/prebuild $(LCVTASK_LIBS) $(LCVLIB_LIBS)
endif
endif
ifneq ("$(wildcard flexibin/*)","")
	@mkdir -p $(BINDIR)/flexibin
	@cp -rf flexibin/* $(BINDIR)/flexibin
endif

-include $(APP_OBJS:.o=.d)

$(OBJDIR)/%.o: $(LMKDIR)/%.c
	@echo "        Compiling:    $<"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d

$(OBJDIR)/%.o: $(LMKDIR)/%.cpp
	@echo "        Compiling:    $<"
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CXX) -MM $(CXXFLAGS) $< >> $(OBJDIR)/$*.d
