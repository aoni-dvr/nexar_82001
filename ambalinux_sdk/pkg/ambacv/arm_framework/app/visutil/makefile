ifndef APP
APP := $(notdir $(abspath $(lastword $(MAKEFILE_LIST)/..)))
endif

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

APPS = visload visdmsg cvdebug

CFLAGS := -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi
LFLAGS := -L$(TOP_BINDIR)/lib -L$(PREBUILDDIR) -pthread -lambadag -lm -lrt
ifdef CV_COMMON_LFLAGS
LFLAGS += $(CV_COMMON_LFLAGS)
endif

ifneq ("$(wildcard $(TOP_BINDIR)/lib/libambadag.a)","")
SCHEDULER_LIB = $(TOP_BINDIR)/lib/libambadag.a
endif

app: all

all: $(addprefix $(BINDIR)/, $(APPS))

-include $(patsubst %,$(OBJDIR)/%.d, $(APPS))
include $(CMNDIR)/build/linker_script.inc

$(BINDIR)/%: $(OBJDIR)/%.o $(APP_LSR)
	@echo "        Building App: $@"
	@mkdir -p $(dir $@)
	$(CC) -o $@ -T $(APP_LSR) $< $(LFLAGS) -Xlinker -Map=$(OBJDIR)/$*.map

$(OBJDIR)/%.o: $(LMKDIR)/%.c
	@echo "        Compiling:    $<"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d
