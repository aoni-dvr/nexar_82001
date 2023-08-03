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

TOP_BINDIR = $(BUILDDIR)/bin

OBJDIR = $(BUILDDIR)/output/app/$(APP)
BINDIR = $(TOP_BINDIR)/app/$(APP)

APP_SRCS := $(wildcard $(LMKDIR)/*.c)

all: app sysflow mpxcopy cvtable

include $(CMNDIR)/build/cvtask_lib.inc
include $(CMNDIR)/build/cvtable.inc
include $(CMNDIR)/build/sysflow.inc
include $(CMNDIR)/build/mpxcopy.inc
include $(CMNDIR)/build/linker_script.inc

CFLAGS := -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi
LFLAGS := -L$(TOP_BINDIR)/lib -L$(PREBUILDDIR)
LFLAGS += -pthread -lambadag -lm -lrt
ifdef CV_COMMON_LFLAGS
LFLAGS += $(CV_COMMON_LFLAGS)
endif
ifeq ($(ENABLE_CVTASK_CAMCTRL_IPC),y)
LFLAGS +=  -lambaipc -laipc_cfg -lambatransfer
endif
LFLAGS += -Xlinker -Map=$(OBJDIR)/$(APP).map

APP_OBJS   = $(subst $(LMKDIR),$(OBJDIR),$(APP_SRCS:.c=.o))
ifneq ("$(wildcard $(TOP_BINDIR)/lib/libambadag.a)","")
SCHEDULER_LIB = $(TOP_BINDIR)/lib/libambadag.a
endif

app: $(BINDIR)/$(APP)
$(BINDIR)/$(APP): $(APP_OBJS) $(APP_LSR) $(SCVTASK_LIBS) $(SCHEDULER_LIB)
	@echo "        Building App: $@"
	@mkdir -p $(dir $@)
	$(CC) -o $@ -T $(APP_LSR) $(APP_OBJS) $(CVTASK_LIBS_FLAGS) $(LFLAGS)

-include $(APP_OBJS:.o=.d)

$(OBJDIR)/%.o: $(LMKDIR)/%.c
	@echo "        Compiling:    $<"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d
